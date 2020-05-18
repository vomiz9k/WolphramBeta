#include <assert.h>
#include <cstring>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <windows.h>
#include "differentiator.h"



tree_branch* diffirintiate_tree(tree_branch* branch, char variable)
{
    tree_branch* returning_branch = 0;
    switch (branch->type)
    {
        case TREE_INTEGER:
            returning_branch = make_value_branch(returning_branch, 0);
        break;

        case TREE_VAR:
            if(*(char*)branch->data == variable)
                returning_branch = make_value_branch(returning_branch, 1);
            else
                returning_branch = make_value_branch(returning_branch, 0);
        break;

        case TREE_FUNCTION:
            returning_branch = derivative_of_func(branch, variable);
        break;

        case TREE_OPERATOR:
            if(*(char*)branch->data == '+' || *(char*)branch->data == '-')
            {
                return derr_of_add(branch, variable);
            }

            else if (*(char*)branch->data == '*' || *(char*)branch->data == '/')
            {
                if (*(char*)branch->data == '*')
                {
                    if (branch->next[0] && branch->next[0]->type == TREE_OPERATOR && *(char*)branch->next[0]->data == '/')
                    {
                        return derr_of_frac(branch, variable);
                    }
                    else
                    {
                       return derr_of_mul(branch, variable);
                    }
                }
                else
                {
                    return derr_of_div(branch, variable);
                }
            }
            else if (*(char*)branch->data == '^')
            {
                return derr_of_pow(branch, variable);
            }
        break;
    }
    return returning_branch;
}

tree_branch* derivative_of_func(tree_branch* func_branch, char variable)
{
    tree_branch* answer = get_G((*(tree_function*)func_branch->data).derr);

    tree_branch* der_of_inside_func = diffirintiate_tree(func_branch->next[0], variable);

    answer = change_y_for_branch(answer, func_branch->next[0], der_of_inside_func);

    branch_full_clr(der_of_inside_func);
    free(der_of_inside_func);

    return answer;
}

tree_branch* change_y_for_branch(tree_branch* change_in, tree_branch* tree_func, tree_branch* tree_func_derivative)
{
    if(change_in->type == TREE_VAR)
    {
        if (*(char*)change_in->data == 'Y')
        {
            change_in = branch_cpy(tree_func);
            return change_in;
        }
        else if (*(char*)change_in->data == 'y')
        {
            change_in = branch_cpy(tree_func_derivative);
            return change_in;
        }
    }

    for(int i = 0; i < change_in->kids; i++)
        if (change_in->next[i])
            change_in->next[i] = change_y_for_branch(change_in->next[i], tree_func, tree_func_derivative);

    return change_in;
}

tree_branch* uv_deffirintiate(tree_branch* change_in, tree_branch* u_value, tree_branch* v_value, char variable)
{
    if(change_in->type == TREE_VAR)
    {
        switch (*(char*)change_in->data)
        {
            case 'U':
                change_in = branch_cpy(u_value);
            break;

            case 'u':
                change_in = diffirintiate_tree(u_value, variable);
            break;

            case 'V':
                change_in = branch_cpy(v_value);
            break;

            case 'v':
                change_in = diffirintiate_tree(v_value, variable);
            break;
        }
        return change_in;
    }

    for(int i = 0; i < change_in->kids; i++)
        if (change_in->next[i])
            change_in->next[i] = uv_deffirintiate(change_in->next[i], u_value, v_value, variable);

    return change_in;

}

tree_branch* branch_cpy(tree_branch* branch)
{
    tree_branch* new_branch = new_branch_construct();
    new_branch->type = branch->type;

    switch (branch->type)
    {
        case TREE_OPERATOR:
            new_branch->data = calloc(1, sizeof(char));
            *(char*)new_branch->data = *(char*)branch->data;
        break;

        case TREE_VAR:
            new_branch->data = calloc(1, sizeof(char));
            *(char*)new_branch->data = *(char*)branch->data;
        break;

        case TREE_INTEGER:
            new_branch->data = calloc(1, sizeof(double));
            *(double*)new_branch->data = *(double*)branch->data;
        break;

        case TREE_FUNCTION:
            new_branch->data = calloc(1, sizeof(tree_function));
            memcpy(new_branch->data, branch->data, sizeof(tree_function));
        break;
    }

    new_branch->kids = branch->kids;
    new_branch->next = (tree_branch**) calloc(branch->kids, sizeof(tree_branch*));

    for (int i = 0; i < branch->kids; i++)
    {
        if (branch->next[i])
            new_branch->next[i] = branch_cpy(branch->next[i]);
    }

    return new_branch;
}

