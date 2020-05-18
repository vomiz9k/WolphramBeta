#include <assert.h>
#include <cstring>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <windows.h>
#include "differentiator.h"

int make_tree_easier(tree_branch* branch)
{
    double value = 0;
    if (branch->type == TREE_OPERATOR)
    {
        if(*(char*)branch->data == '^')
        {
            int first_int_num = 0;

            for (first_int_num = 0; first_int_num < branch->kids; first_int_num++)
                if (branch->next[first_int_num] && make_tree_easier(branch->next[first_int_num]) == TREE_INTEGER)
                    break;

            if (first_int_num == branch->kids)
                return branch->type;

            value = *(double*)branch->next[first_int_num]->data;
            int int_count = 1;
            int int_number = first_int_num;
            for (int i = first_int_num + 1; i < branch->kids; ++i)
            {
                if (branch->next[i] && make_tree_easier(branch->next[i]) == TREE_INTEGER)
                {
                    if(int_number == i - 1)
                    {
                        branch_full_clr(branch->next[i - 1]);
                        free(branch->next[i - 1]);
                        branch->next[i - 1] = 0;
                        value = value_count(value, tree_count(branch->next[i], 0), '*');
                        *(double*)branch->next[i]->data = value;
                        int_count++;
                        int_number = i;
                    }
                    else
                    {
                        int_number = i;
                        int_count++;
                    }
                }
            }

            if (int_count == branch->kids)
            {
                tree_branch* cache = branch_cpy(branch->next[int_number]);
                branch_full_clr(branch);
                copy_branch_values(cache, branch);
                branch_full_clr(cache);
                return branch->type;
            }

            first_int_num = 0;;
            int real_kids = 0;

            while(branch->next[first_int_num] == 0 && first_int_num < branch->kids)
                first_int_num++;

            for(int i = 0; i < branch->kids; ++i)
                if(branch->next[i])
                    real_kids++;

            if (int_count == 1 && real_kids == 2 && branch->next[first_int_num]->type != TREE_INTEGER)
            {
                if(*(double*)branch->next[int_number]->data == 1)
                {
                    tree_branch* cache = branch_cpy(branch->next[first_int_num]);
                    branch_full_clr(branch);
                    copy_branch_values(cache, branch);
                    free(cache);
                    return branch->type;
                }
                else if(*(double*)branch->next[int_number]->data == 0)
                {
                    make_value_branch(branch, 1);
                    return branch->type;
                }
            }


            if(branch->next[first_int_num]->type == TREE_INTEGER)
            {
                if(*(double*)branch->next[first_int_num]->data == 1 || *(double*)branch->next[first_int_num]->data == 0)
                {
                    make_value_branch(branch, *(double*)branch->next[first_int_num]->data);
                }
            }
            return branch->type;
        }

        double neutral_el = get_neutral_el(*(char*)branch->data);

        int int_count = 0;
        int int_number = -1;
        value = neutral_el;

        bool flag = false;

        if (*(char*)branch->data == '*' || *(char*)branch->data == '+')
            flag = true;

        if(branch->next[0] && flag)
        {
            for(int i = 0; i < branch->next[0]->kids; ++i)
            {

                if (branch->next[0]->next[i] && *(char*)branch->next[0]->next[i]->data == *(char*)branch->data && branch->next[0]->next[i]->type == TREE_OPERATOR)
                {
                    for(int j = 1; j < branch->next[0]->next[i]->kids; ++j)
                    {
                        add_kid(branch->next[0]);
                        branch->next[0]->next[branch->next[0]->kids] = branch->next[0]->next[i]->next[j];
                        branch->next[0]->kids++;
                    }
                    if (branch->next[0]->next[i]->next[0])
                        for(int j = 0; j < branch->next[0]->next[i]->next[0]->kids; ++j)
                        {
                            add_kid(branch);
                            branch->next[branch->kids] = branch->next[0]->next[i]->next[0]->next[j];
                            branch->kids++;
                        }
                    branch_clr(branch->next[0]->next[i]);
                    free(branch->next[0]->next[i]);
                    branch->next[0]->next[i] = 0;
                }
                else if (branch->next[0]->next[i] && make_tree_easier(branch->next[0]->next[i]) == TREE_INTEGER)
                {
                    value = value_count(value, tree_count(branch->next[0]->next[i], 0), *(char*)branch->next[0]->data);
                    branch_full_clr(branch->next[0]->next[i]);
                    free(branch->next[0]->next[i]);
                    branch->next[0]->next[i] = 0;

                    int_count++;
                    int_number = i;
                }
            }
        }

        for(int i = 1 & flag; i < branch->kids; ++i)
        {
            if(branch->next[i])
            {

                if (flag && *(char*)branch->next[i]->data == *(char*)branch->data && branch->next[i]->type == TREE_OPERATOR)
                {

                    for (int j = 1; j < branch->next[i]->kids; ++j)
                    {
                        if (branch->next[i]->next[j])
                        {
                            add_kid(branch);
                            branch->next[branch->kids] = branch->next[i]->next[j];
                            branch->kids++;
                        }
                    }
                    if (branch->next[i]->next[0])
                        for(int j = 0; j < branch->next[i]->next[0]->kids; ++j)
                        {
                            if (!branch->next[0])
                            {
                                char c = 0;
                                if(*(char*)(branch->data) == '+')
                                    c = '-';
                                else
                                    c = '/';

                                branch->next[0] = make_char_branch(branch->next[0], c, TREE_OPERATOR);
                            }
                            if (branch->next[i]->next[0]->next[j])
                            {
                                add_kid(branch->next[0]);
                                branch->next[0]->next[branch->next[0]->kids] = branch->next[i]->next[0]->next[j];
                                branch->next[0]->kids++;
                            }
                        }
                    branch_clr(branch->next[i]);
                    free(branch->next[i]);
                    branch->next[i] = 0;
                }
                else if (make_tree_easier(branch->next[i]) == TREE_INTEGER)
                {
                    value = value_count(value, tree_count(branch->next[i], 0), *(char*)branch->data);
                    branch_full_clr(branch->next[i]);
                    free(branch->next[i]);
                    branch->next[i] = 0;
                    int_count++;
                    int_number = i;

                }
                else if (branch->next[0] && search_same(branch->next[0], branch->next[i]))
                {
                    branch_full_clr(branch->next[i]);
                    free(branch->next[i]);
                    branch->next[i] = 0;
                }
                else if (branch->next[i] && *(char*)branch->data == '*' && branch->next[i]->type == TREE_OPERATOR && *(char*)branch->next[i]->data == '+')
                {
                    if(branch->next[i]->next[0])
                    {
                        normalize_tree(branch->next[i]);
                        if(branch->next[i]->kids == 1)
                        {
                            add_kid(branch);
                            make_value_branch(branch->next[branch->kids], -1);
                            branch->next[i] = branch->next[i]->next[0];
                            *(char*)branch->next[i]->data = '+';
                            branch->kids++;

                            add_kid(branch->next[i]);
                            branch->next[i]->next[branch->next[i]->kids] = branch->next[i]->next[0];
                            branch->next[i]->next[0] = 0;
                            branch->next[i]->kids++;
                        }
                    }
                }
            }
        }
        if(int_number != -1)
        {
            if (value == 0 && *(char*)branch->data == '*')
            {
                make_value_branch(branch, 0);
                return TREE_INTEGER;
            }

            if(value != neutral_el)
            {
                add_kid(branch);
                make_value_branch(branch->next[branch->kids], value);
                branch->kids++;
            }
        }

        if(branch->next[0] && flag)
        {
            int real_kids = 0;
            for (int i = 0; i < branch->next[0]->kids; ++i)
                if(branch->next[0]->next[i])
                    real_kids++;
            if (real_kids == 0)
            {
                branch_full_clr(branch->next[0]);
                free(branch->next[0]);
                branch->next[0] = 0;
            }
        }

        int real_kids = 0;
        tree_branch* last_kid = 0;
        int last_kid_num = 0;
        for(int i = 0; i < branch->kids; ++i)
            if(branch->next[i])
            {
                real_kids++;
                last_kid = branch->next[i];
                last_kid_num = i;
            }

        if (real_kids == 1 && flag && last_kid_num != 0)
        {
            tree_branch* cache = branch_cpy(last_kid);
            branch_full_clr(branch);
            copy_branch_values(cache, branch);
            free(cache);

            return branch->type;
        }

        else if (real_kids == 0)
        {
            make_value_branch(branch, neutral_el);
            return branch->type;
        }
    }
    if(branch->type == TREE_FUNCTION)
    {
        if(make_tree_easier(branch->next[0]) == TREE_INTEGER)
        {
            double (*func)(double) = ((tree_function*)(branch->data))->ptr;
            make_value_branch(branch, func(*(double*)branch->next[0]->data));
            return branch->type;
        }

    }
    return branch->type;
}
