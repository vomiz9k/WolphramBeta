#include <assert.h>
#include <cstring>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <windows.h>
#include "differentiator.h"

tree_branch* normalize_tree(tree_branch* branch)
{
    if (branch->type == TREE_OPERATOR && *(char*)branch->data != '^' && branch->kids > 0)
    {
        int* is_free = (int*)calloc(sizeof(int), branch->kids);

        int counter = 0;

        bool flag = false;
        if (*(char*)branch->data == '*' || *(char*)branch->data == '+')
            flag = true;
        int real_kids = 1 & flag;

        for (int i = 1 & flag; i < branch->kids; i++)
        {
            if (branch->next[i])
                real_kids++;

            else
            {
                is_free[counter] = i;
                counter++;
            }
        }
        int new_counter = 0;
        for (int i = real_kids; i < branch->kids && new_counter < counter; i++)
            if (branch->next[i])
            {
                branch->next[is_free[new_counter]] = branch->next[i];
                new_counter++;
                branch->next[i] = 0;
            }
        branch->next = (tree_branch**)realloc(branch->next, sizeof(tree_branch*) * (real_kids));
        branch->kids = real_kids;
        free(is_free);
    }
    for (int i = 0; i < branch->kids; i++)
        if (branch->next[i])
            normalize_tree(branch->next[i]);
}


int search_similar_add(tree_branch* branch)
{
    for (int i = 0; i < branch->kids; i++)
        if (branch->next[i])
            search_similar_add(branch->next[i]);

    if (branch->type == TREE_OPERATOR && *(char*)branch->data != '^')
    {
        int i = 0;
        if (*(char*)branch->data == '+' || *(char*)branch->data == '*')
            i = 1;
        for (; i < branch->kids; ++i)
            if (branch->next[i])
            {
                double similar_count = 1;

                for (int j = i + 1; j < branch->kids; ++j)
                    if (branch->next[j])
                    {
                        if (is_same(branch->next[i], branch->next[j]))
                        {
                            branch_full_clr(branch->next[j]);
                            free(branch->next[j]);
                            branch->next[j] = 0;
                            similar_count++;
                        }
                    }

                if (similar_count != 1)
                {
                    tree_branch* changing_branch = 0;
                    if (*(char*)branch->data == '+' || *(char*)branch->data == '-')
                        changing_branch = make_char_branch(changing_branch, '*', TREE_OPERATOR);
                    else
                        changing_branch = make_char_branch(changing_branch, '^', TREE_OPERATOR);

                    if (*(char*)changing_branch->data == '+' || *(char*)changing_branch->data == '*')
                    {
                        add_kid(changing_branch);
                        changing_branch->next[0] = 0;
                        changing_branch->kids++;
                    }
                    add_kid(changing_branch);
                    changing_branch->next[changing_branch->kids] = branch->next[i];
                    changing_branch->kids++;

                    add_kid(changing_branch);
                    make_value_branch(changing_branch->next[changing_branch->kids], similar_count);
                    changing_branch->kids++;
                    branch->next[i] = changing_branch;
                }
            }
    }
}

int copy_branch_values(tree_branch* value, tree_branch* dest_branch)
{
    dest_branch->data = value->data;
    dest_branch->type = value->type;
    dest_branch->next = value->next;
    dest_branch->kids = value->kids;
}

tree_branch* make_value_branch(tree_branch* branch, double value)
{
    if (branch == 0)
        branch = new_branch_construct();
    else
    {
        branch_full_clr(branch);
    }

    branch->type = TREE_INTEGER;
    branch->kids = 0;
    branch->next = 0;
    branch->data = calloc(1, sizeof(double));
    *(double*)branch->data = value;
    return branch;
}

tree_branch* make_char_branch(tree_branch* branch, char value, int branch_type)
{
    if (branch == 0)
        branch = new_branch_construct();
    else
    {
        branch_full_clr(branch);
    }

    branch->type = branch_type;
    branch->kids = 0;
    branch->next = 0;
    branch->data = calloc(1, sizeof(char));
    *(char*)branch->data = value;
    return branch;
}

double tree_count(tree_branch* branch, double* var_vals)
{
    double value = 0;
    if (branch->type == TREE_OPERATOR)
    {
        if (*(char*)branch->data == '^')
        {
            int i = 0;

            while(i < branch->kids && !branch->next[i])
                i++;

            value = tree_count(branch->next[i], var_vals);

            for (i = i + 1; i < branch->kids; ++i)
            {
                if (branch->next[i])
                    value = value_count(value, tree_count(branch->next[i], var_vals), *(char*)branch->data);
            }
            return value;
        }
        value = get_neutral_el(*(char*)branch->data);

        bool flag = false;
        if (*(char*)branch->data == '*' || *(char*)branch->data == '+')
            flag = true;
        for (int i = 1 & flag; i < branch->kids; ++i)
        {
            if (branch->next[i])
                value = value_count(value, tree_count(branch->next[i], var_vals), *(char*)branch->data);
        }

        if (flag && branch->next[0])
            for (int i = 0; i < branch->next[0]->kids; ++i)
            {
                if (branch->next[0]->next[i])
                    value = value_count(value, tree_count(branch->next[0]->next[i], var_vals), *(char*)branch->next[0]->data);
            }

        return value;
    }
    if (branch->type == TREE_INTEGER)
    {
        return *(double*)branch->data;
    }

    if (branch->type == TREE_VAR)
    {
        return var_vals[*(char*)branch->data - 'a'];
    }

    if (branch->type == TREE_FUNCTION)
    {
        double (*func)(double) = ((tree_function*)(branch->data))->ptr;
        return func(tree_count(branch->next[0], var_vals));
    }
}

int search_same(tree_branch* search_into, tree_branch* what_to_search)
{
    for (int i = 0; i < search_into->kids; i++)
    {
        if (search_into->next[i])
        {
            if (is_same(search_into->next[i], what_to_search))
            {
                branch_full_clr(search_into->next[i]);
                free(search_into->next[i]);
                search_into->next[i] = 0;
                return 1;
            }
        }
    }
    return 0;
}

int is_same(tree_branch* branch_1, tree_branch* branch_2)
{
    bool flag = false;
    if (branch_1->type == branch_2->type)
    {
        switch (branch_1->type)
        {
            case TREE_INTEGER:
                if (*(double*)branch_1->data == *(double*)branch_2->data)
                    flag = true;
            break;

            case TREE_VAR:
                if (*(char*)branch_1->data == *(char*)branch_2->data)
                    flag = true;
            break;

            case TREE_FUNCTION:
                if (((tree_function*)branch_1->data)->ptr == ((tree_function*)branch_2->data)->ptr
                    && is_same(branch_1->next[0], branch_2->next[0]))
                    flag = true;
            break;

            case TREE_OPERATOR:
            {
                if (*(char*)branch_1->data != *(char*)branch_2->data)
                    return flag;

                if (*(char*)branch_1->data == '^')
                {
                    int i = 0, j = 0;
                    while(i < branch_1->kids && j < branch_2->kids)
                    {
                        while(!branch_1->next[i] && i < branch_1->kids)
                            ++i;
                        while(!branch_2->next[j] && j < branch_2->kids)
                            ++j;
                        if (i == branch_1->kids || j == branch_2->kids)
                            break;

                        if (!is_same(branch_1->next[i], branch_2->next[j]))
                        {
                            return false;
                        }
                        ++i;
                        ++j;
                    }
                    while(i < branch_1->kids || j < branch_2->kids)
                    {
                        if (i == branch_1->kids)
                        {
                            if (branch_2->next[j])
                                return false;
                            j++;
                        }
                        if (j == branch_2->kids)
                        {
                            if (branch_1->next[i])
                                return false;
                            i++;
                        }
                    }
                    return true;
                }

                tree_branch* new_branch_1 = branch_cpy(branch_1);
                tree_branch* new_branch_2 = branch_cpy(branch_2);
                for (int i = 0; i < new_branch_1->kids; ++i)
                {
                    if (new_branch_1->next[i])
                    {
                        if (!search_same(new_branch_2, new_branch_1->next[i]))
                        {
                            branch_full_clr(new_branch_1);
                            branch_full_clr(new_branch_2);
                            free(new_branch_1);
                            free(new_branch_2);
                            return flag;
                        }
                        else
                        {
                            branch_full_clr(new_branch_1->next[i]);
                            free(new_branch_1->next[i]);
                            new_branch_1->next[i] = 0;
                        }
                    }
                }
                for (int i = 0; i < new_branch_1->kids; ++i)
                {
                    if (new_branch_1->next[i])
                    {
                        branch_full_clr(new_branch_1);
                        branch_full_clr(new_branch_2);
                        free(new_branch_1);
                        free(new_branch_2);
                        return flag;
                    }
                }
                for (int i = 0; i < new_branch_2->kids; ++i)
                {
                    if (new_branch_2->next[i])
                    {
                        branch_full_clr(new_branch_1);
                        branch_full_clr(new_branch_2);
                        free(new_branch_1);
                        free(new_branch_2);
                        return flag;
                    }
                }
                branch_full_clr(new_branch_1);
                branch_full_clr(new_branch_2);
                free(new_branch_1);
                free(new_branch_2);
                flag = true;
            }
            break;
        }
        if (flag == true)
            return flag;
    }
    return flag;
}


double get_neutral_el(char operation)
{
    if (operation == '+' || operation == '-')
        return 0;
    if (operation == '*' || operation == '/')
        return 1;

    return -1;
}

int branch_full_clr(tree_branch* branch)
{
    if (branch)
    {
        for (int i = 0; i < branch->kids; ++i)
            if (branch->next[i])
            {
                branch_full_clr(branch->next[i]);
                free(branch->next[i]);
                branch->next[i] = 0;
            }
        branch_clr(branch);
    }
    return 0;
}

double value_count(double value, double difference, char operation)
{
    switch (operation)
    {
        case '+':

            return value + difference;

        case '-':
            return value - difference;

        case '*':
            return value * difference;

        case '/':
            return value / difference;

        case '^':
            return pow(value, difference);
    }
}



tree_branch* new_branch_construct()
{
    tree_branch* brunch = (tree_branch*) calloc(sizeof(tree_branch), 1);
    assert(brunch);

    brunch->type = -1;
    brunch->kids = 0;
    brunch->next = 0;
    brunch->data = 0;

    return brunch;
}

void add_kid(tree_branch* branch)
{
    assert(branch);
    branch->next = (tree_branch**)realloc(branch->next, sizeof(tree_branch*) * (branch->kids + 1));
    assert(branch->next);
    branch->next[branch->kids] = new_branch_construct();
    assert(branch->next && branch->next[branch->kids]);
}

void branch_clr(tree_branch* branch)
{
    if (branch->data)
        free(branch->data);
    if (branch->next)
        free(branch->next);
    branch->next = 0;
    branch->data = 0;
}

