#include <assert.h>
#include <cstring>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <windows.h>
#include "differentiator.h"

const int O_SMALL = 6;

tree_function FUNCTIONS[] =
{
    {"sin", &sin, "cos(Y) * y"},
    {"cos", &cos, "0 - (sin(Y)) * y"},
    {"tan", &tan, "y / ((cos(Y))^2)"},
    {"acos", &acos, "(0 - y) / sqrt(1 + Y * Y)"},
    {"asin", &asin, "y / sqrt(1 + Y * Y)"},
    {"atan", &atan, " y / (1 + Y * Y)"},
    {"log", &log, "y * Y ^ (-1)"},
    {"sqrt", &sqrt, "y / (2 * sqrt(Y))"},
    {"exp", &exp, "y * exp(Y)"}
};
const int FUNCTIONS_COUNT = sizeof(FUNCTIONS) / sizeof(FUNCTIONS[0]);


const int SRC_STR_LEN = 1000;
char SRC_STR_BUFF[SRC_STR_LEN] = {};
char* SRC_STR = SRC_STR_BUFF;

enum PRIORITY
{
    ADD_PRIORITY,
    MULT_PRIORITY,
    POWER_PRIORITY,
    HIGH_PRIORITY
};

int print_domain(tree_branch* branch, FILE* latex)
{
    bool flag = false;
    if (branch->type == TREE_OPERATOR && make_tree_easier(branch) != TREE_INTEGER)
    {
        if(*(char*)branch->data == '/')
        {
            for(int i = 0; i < branch->kids; i++)
            {
                if(branch->next[i])
                {
                    fprintf(latex,  "\\begin{center}\n\\begin{math}\n");
                    tree_txt(branch->next[i], latex);
                    fprintf(latex, " \\neq 0");
                    fprintf(latex, "\n\\end{math}\n\\end{center}\n");
                    flag = true;
                }
            }
        }
        else if (*(char*)branch->data == '^')
        {
            if (branch->kids == 2 && make_tree_easier(branch->next[1]) == TREE_INTEGER)
            {
                double value = *(double*)branch->next[1]->data;
                if (value < 0)
                {
                    fprintf(latex,  "\\begin{center}\n\\begin{math}\n");
                    tree_txt(branch->next[0], latex);
                    fprintf(latex, " \\neq 0");
                    fprintf(latex, "\n\\end{math}\n\\end{center}\n");
                    flag = true;
                }
            }
            if (branch->kids == 2 && make_tree_easier(branch->next[0]) != TREE_INTEGER &&
                branch->next[1]->type != TREE_INTEGER)
                {
                    fprintf(latex,  "\\begin{center}\n\\begin{math}\n");
                    tree_txt(branch->next[0], latex);
                    fprintf(latex, " > 0");
                    fprintf(latex, "\n\\end{math}\n\\end{center}\n");
                    flag = true;
                }

        }
    }
    else if(branch->type == TREE_FUNCTION)
    {
        double (*func)(double) = ((tree_function*)branch->data)->ptr;
        if (func == log || func == sqrt)
        {
            fprintf(latex,  "\\begin{center}\n\\begin{math}\n");
            tree_txt(branch->next[0], latex);
            if (func == log)
                fprintf(latex, " > 0");
            else
                fprintf(latex, " \\ge 0");
            fprintf(latex, "\n\\end{math}\n\\end{center}\n");
            flag = true;
        }
        else if (func == asin || func == acos)
        {
            fprintf(latex,  "\\begin{center}\n\\begin{math}\n");
            fprintf(latex, "-1 \\le ");
            tree_txt(branch->next[0], latex);
            fprintf(latex, " \\le 1");
            fprintf(latex, "\n\\end{math}\n\\end{center}\n");
            flag = true;
        }
        else if (func == tan)
        {
            fprintf(latex,  "\\begin{center}\n\\begin{math}\n");
            tree_txt(branch->next[0], latex);
            fprintf(latex, " \\neq \\frac{\\pi}{2} + \\pi * k, k \\in \\mathbb{Z}");
            fprintf(latex, "\n\\end{math}\n\\end{center}\n");
            flag = true;
        }
    }
    for(int i = 0; i < branch->kids; i++)
    {
        if (branch->next[i])
            if (print_domain(branch->next[i], latex))
                flag = true;
    }
    return flag;
}

int latex_maker(tree_branch* func, char variable, int count, double* var_vals)
{
    assert(func);

    FILE* latex = fopen("latex.tex", "w");
    fprintf(latex, "\\documentclass[a4paper,12pt]{article}\n");
    fprintf(latex, "\\usepackage{cmap}\n");
    fprintf(latex, "\\usepackage[T2A]{fontenc}\n");
    fprintf(latex, "\\usepackage[cp1251]{inputenc}\n");
    fprintf(latex, "\\usepackage[english,russian]{babel}\n");
    fprintf(latex, "\\usepackage{amsfonts}\n");
    fprintf(latex, "\\title{���������� ����������� ��-��������.}\n");
    fprintf(latex, "\\author{������: 0 ���}\n");
    fprintf(latex, "\\date{\\today}\n");
    fprintf(latex, "\\begin{document}\n");
    fprintf(latex, "\\maketitle\n");
    fprintf(latex, "������ ����� � ��������\n");
    fprintf(latex,  "\\begin{center}\n\\begin{math}\n");

    fprintf(latex, "f(%c) = ", variable);
    tree_txt(func, latex);

    fprintf(latex, "\n\\end{math}\n\\end{center}\n");
    fprintf(latex,  "\\begin{center}\n\\begin{math}\n");

    fprintf(latex, "g(%c) = ", variable);
    tree_txt(func, latex);

    fprintf(latex, "\n\\end{math}\n\\end{center}\n");
    fprintf(latex,  "\\begin{center}\n\\begin{math}\n");

    fprintf(latex, "h(%c) = ", variable);
    tree_txt(func, latex);

    fprintf(latex, "\n\\end{math}\n\\end{center}\n");
    fprintf(latex,  "\\begin{center}\n\\begin{math}\n");

    fprintf(latex, "p(%c) = ", variable);
    tree_txt(func, latex);

    fprintf(latex, "\n\\end{math}\n\\end{center}\n");

    fprintf(latex,"� ��� � �������� ����� �������������� ������ �������:\n");
    if (!print_domain(func, latex))
    {
        fprintf(latex,  "\\begin{center}\n\\begin{math}\n");
        fprintf(latex, "%c \\in \\mathbb{R}", variable);
        fprintf(latex, "\n\\end{math}\n\\end{center}\n");
    }
    fprintf(latex, "� ������ ����� ����:\n");
    make_tree_easier(func);
    make_tree_easier(func);
    search_similar_add(func);
    make_tree_easier(func);
    normalize_tree(func);
    make_tree_easier(func);
    fprintf(latex,  "\\begin{center}\n\\begin{math}\n");

    fprintf(latex, "f(%c) = ", variable);
    tree_txt(func, latex);

    fprintf(latex, "\n\\end{math}\n\\end{center}\n");

    tree_branch* next_derr = branch_cpy(func);
    int i = 0;


    double macloren[O_SMALL] = {};

    double cache = var_vals[variable - 'a'];
    var_vals[variable - 'a'] = 0;
    macloren[0] = tree_count(next_derr, var_vals);
    var_vals[variable - 'a'] = cache;

    for( ; i < count || i < O_SMALL; ++i)
    {
        tree_branch* prev_derr = next_derr;

        next_derr = diffirintiate_tree(prev_derr, variable);
        branch_full_clr(prev_derr);
        free(prev_derr);

        make_tree_easier(next_derr);
        make_tree_easier(next_derr);
        search_similar_add(next_derr);
        make_tree_easier(next_derr);
        normalize_tree(next_derr);
        make_tree_easier(next_derr);
        make_tree_easier(next_derr);

        if (i < count)
        {
            fprintf(latex, "� ����� ����� ����\n");

            fprintf(latex,  "\\begin{center}\n\\begin{math}\n");
            fprintf(latex, "f^{(%d)}(%c) = ", i + 1, variable);
            tree_txt(next_derr, latex);
            fprintf(latex, "\n\\end{math}\n\\end{center}\n");

        }
        if (i < O_SMALL)
        {
            cache = var_vals[variable - 'a'];
            var_vals[variable - 'a'] = 0;
            macloren[i + 1] = tree_count(next_derr, var_vals);
            var_vals[variable - 'a'] = cache;
        }

    }
    double value = tree_count(next_derr, var_vals);
    fprintf(latex, "\\maketitle � ����� ����������� ���� %c = "AFTER_ZERO_ACCURACY"\\\\\n", variable, var_vals[variable - 'a']);
    if (isfinite(value))
    {
        fprintf(latex,  "\\begin{center}\n\\begin{math}\n");
        fprintf(latex, "f^{(%d)}("AFTER_ZERO_ACCURACY") = %lf", i, var_vals[variable - 'a'], value);
        fprintf(latex, "\n\\end{math}\n\\end{center}\n");
    }
    else
        fprintf(latex,  "\\\maketitle �� � ������ � ���� �� ����������, ��� ��� �� �� �������� �����.\\\\\n");

    fprintf(latex,  "\\\maketitle � ����� ������ ����� � �������������� �� ���������:\\\\\n");
    fprintf(latex,  "\\begin{center}\n\\begin{math}\n");
    fprintf(latex, "f(%c) = ", variable);
    if (macloren[0] != 0)
        fprintf(latex, AFTER_ZERO_ACCURACY, macloren[0]);
    for(int i = 1; i < O_SMALL; ++i)
    {
        if(macloren[i] != 0)
            fprintf(latex, " + \\frac{"AFTER_ZERO_ACCURACY"}{%d!} \\cdot %c^%d", macloren[i], i, variable, i);
    }
    fprintf(latex, " + o(%c^%d)", variable, O_SMALL - 1);
    fprintf(latex, ",\t%c \\longrightarrow 0.", variable);
    fprintf(latex, "\n\\end{math}\n\\end{center}\n");



    fprintf(latex, "\\maketitle �� �� ������ � ����� �������...");

    fprintf(latex, "\\end{document}\n");
    fclose(latex);
    system("pdflatex latex.tex");
    system("latex.pdf");
    tree_print(next_derr, "goutput.gv");
}

int tree_txt(tree_branch* branch, FILE * fout)
{


    switch(branch->type)
    {
        case TREE_FUNCTION:
            fprintf(fout, "%s(", ((tree_function*)branch->data)->name);
            tree_txt(branch->next[0], fout);
            fprintf(fout, ")");
        break;

        case TREE_INTEGER:
        {
            int value = (int)*(double*)branch->data;
            if (*(double*)branch->data < 0)
                fprintf(fout, "(");

            if (value == *(double*)branch->data)
                fprintf(fout, "%0.0lf", *(double*)branch->data);
            else
                fprintf(fout, AFTER_ZERO_ACCURACY, *(double*)branch->data);

            if (*(double*)branch->data < 0)
                fprintf(fout, ")");

        }
        break;

        case TREE_VAR:
            fprintf(fout, "%c", *(char*)branch->data);
        break;

        case TREE_OPERATOR:
        {
            int curr_priority = get_priority(branch);
            bool flag = false;
            if (branch->type == TREE_OPERATOR && (*(char*)branch->data == '+' || *(char*)branch->data == '*'))
                flag = true;

            int last_kid = branch->kids - 1;

            while (!branch->next[last_kid])
                last_kid--;
            char operation[8] = {};
            operation[0] = *(char*)branch->data;
            if (*(char*)branch->data == '*')
                sprintf(operation, " \\cdot ");

            if (*(char*)branch->data == '*' && last_kid == 0)
            {
                fprintf(fout, "\\frac{1}{");
            }

            else if (*(char*)branch->data == '*' && branch->next[0])
            {
                fprintf(fout, "\\frac{");
            }

            int i = 1 & flag;

            for (i; i < branch->kids; ++i)
            {
                if (branch->next[i])
                {
                    int next_priority = get_priority(branch->next[i]);

                    if (*(char*)branch->data == '^')
                    {
                        bool need_hooks = false;
                        if (branch->next[i]->type == TREE_INTEGER || branch->next[i]->type == TREE_VAR)
                            fprintf(fout, "{");
                        else
                        {
                            need_hooks = true;
                            fprintf(fout, "{(");
                        }
                        tree_txt(branch->next[i], fout);
                        if (need_hooks)
                            fprintf(fout, ")}");
                        else
                            fprintf(fout, "}");
                    }

                    else if (next_priority >= curr_priority)
                        tree_txt(branch->next[i], fout);

                    else
                    {
                        fprintf(fout, "{(");
                        tree_txt(branch->next[i], fout);
                        fprintf(fout, ")}");
                    }

                    if (i != last_kid)
                        fprintf(fout, "%s", operation);
                }
            }

            if (flag && branch->next[0])
            {
                if (*(char*)branch->data == '+')
                {
                    memset(operation, 0, 6);
                    *operation = '-';
                    *(operation + 1) = 0;
                    fprintf(fout, "-");
                }
                else if (*(char*)branch->data == '*' && last_kid != 0)
                    fprintf(fout, "}{");

                last_kid = branch->next[0]->kids - 1;

                while (!branch->next[0]->next[last_kid])
                    last_kid--;

                for (i = 0; i < branch->next[0]->kids; ++i)
                {
                    if (branch->next[0]->next[i])
                    {
                        int next_priority = get_priority(branch->next[0]->next[i]);

                        if (next_priority >= curr_priority)
                            tree_txt(branch->next[0]->next[i], fout);

                        else
                        {
                            fprintf(fout, "{(");
                            tree_txt(branch->next[0]->next[i], fout);
                            fprintf(fout, ")}");
                        }

                        if (i != last_kid)
                            fprintf(fout, "%s", operation);
                    }
                }
                if (*(char*)branch->data == '*')
                    fprintf(fout, "}");
            }
        }
    }
}

int get_priority(tree_branch* branch)
{
    switch(branch->type)
    {
        case TREE_FUNCTION:
            return HIGH_PRIORITY;

        case TREE_INTEGER:
            return HIGH_PRIORITY;

        case TREE_VAR:
            return HIGH_PRIORITY;

        case TREE_OPERATOR:
            if (*(char*)branch->data == '^')
                return POWER_PRIORITY;

            else if (*(char*)branch->data == '+' || *(char*)branch->data == '-')
                return ADD_PRIORITY;

            else if (*(char*)branch->data == '*' || *(char*)branch->data == '/')
                return MULT_PRIORITY;
    }
}


void tree_print(tree_branch* head, char* fout_name)
{
    assert(head && fout_name);
    FILE* fout = fopen(fout_name, "w");
    assert(fout);
    fprintf(fout, "digraph First{\n");

    fprintf(fout, "node [fontname=\"Lobster\", shape=\"Mrecord\", style=\"filled\", fillcolor=\"lightblue\"];");

    if (head->type == TREE_OPERATOR || head->type == TREE_VAR)
        fprintf(fout, "\"box%0x\" [label=\"%c\"];\n", head, *(char*)(head->data));

    if (head->type == TREE_INTEGER)
        fprintf(fout, "\"box%0x\" [label=\"" AFTER_ZERO_ACCURACY "\"];\n", head, *(double*)(head->data));

    if (head->type == TREE_FUNCTION)
        fprintf(fout, "\"box%0x\" [label=\"%s\"];\n", head, ((tree_function*)(head->data))->name);


    recursive_tree_print(head, fout);

    fprintf(fout, "}");
    fclose(fout);

    system("dot -Tpng goutput.gv -o foutput.png");

    system("foutput.png");
}



void recursive_tree_print (tree_branch* branch, FILE* fout)
{
    for (int i = branch->kids - 1; i >= 0; --i)
    {
        int k = 0;
        if (branch->type == TREE_OPERATOR && *(char*)branch->data == '^')
            k = branch->kids - i - 1;
        else
            k = i;

        if (branch->next[k])
        {

            if (branch->type == TREE_FUNCTION)
                fprintf(fout, "\"box%0x\" [label=\"%s\"];\n", branch, ((tree_function*)(branch->data))->name);

            if (branch->next[k]->type == TREE_INTEGER)
                fprintf(fout, "\"box%0x\" [label=\"" AFTER_ZERO_ACCURACY "\"];\n", branch->next[k], *(double*)(branch->next[k]->data));

            else
                fprintf(fout, "\"box%0x\" [label=\"%c\"];\n", branch->next[k], *(char*)branch->next[k]->data);

            fprintf(fout, "\"box%0x\" -> \"box%0x\"[label=\"\", color=\"green\"];\n", branch, (branch->next)[k]);
            recursive_tree_print(branch->next[k], fout);
        }
    }
}


tree_branch* get_G(char* str)
{
    get_calc_string(str);
    char* local_str_ptr = SRC_STR;

    tree_branch* value = get_E();

    SRC_STR = local_str_ptr;
    return value;
}

tree_branch* get_E()
{
    tree_branch* value = 0;
    value = make_char_branch(value, '+', TREE_OPERATOR);

    add_kid(value);
    value->next[0] = make_char_branch(value->next[0], '-', TREE_OPERATOR);
    value->kids++;

    add_kid(value);
    value->next[value->kids] = get_T();
    value->kids++;


    while (*SRC_STR == '+' || *SRC_STR == '-')
    {
        char operation = *SRC_STR;
        SRC_STR++;

        tree_branch* next_value = get_T();

        if (operation == '+')
        {
            add_kid(value);
            value->next[value->kids] = next_value;
            value->kids++;
        }
        else
        {
            add_kid(value->next[0]);
            value->next[0]->next[value->next[0]->kids] = next_value;
            value->next[0]->kids++;
        }
    }
    if (value->next[0]->kids == 0)
    {
        branch_clr(value->next[0]);
        //free(value->next[0]);
        value->next[0] = 0;
        if (value->kids == 2)
        {
            tree_branch* cache = branch_cpy(value->next[1]);
            branch_full_clr(value);
            free(value);
            return cache;
        }
    }
    return value;
}

tree_branch* get_T()
{
    tree_branch* value = 0;
    value = make_char_branch(value, '*', TREE_OPERATOR);

    add_kid(value);
    value->next[0] = make_char_branch(value->next[0], '/', TREE_OPERATOR);
    value->kids++;

    add_kid(value);
    value->next[value->kids] = get_C();
    value->kids++;

    while (*SRC_STR == '*' || *SRC_STR == '/')
    {
        char operation = *SRC_STR;
        SRC_STR++;

        tree_branch* next_value = get_C();

        if (operation == '*')
        {
            add_kid(value);
            value->next[value->kids] = next_value;
            value->kids++;
        }
        else
        {
            add_kid(value->next[0]);
            value->next[0]->next[value->next[0]->kids] = next_value;
            value->next[0]->kids++;
        }
    }
    if (value->next[0]->kids == 0)
    {
        branch_clr(value->next[0]);
        //free(value->next[0]);
        value->next[0] = 0;
        if (value->kids == 2)
        {
            tree_branch* cache = branch_cpy(value->next[1]);
            branch_full_clr(value);
            free(value);
            return cache;
        }
    }
    return value;
}

tree_branch* get_C()
{
    tree_branch* value = 0;
    value = make_char_branch(value, '^', TREE_OPERATOR);

    add_kid(value);
    value->next[value->kids] = get_P();
    value->kids++;

    while (*SRC_STR == '^')
    {
        SRC_STR++;

        tree_branch* next_value = get_P();

        add_kid(value);
        value->next[value->kids] = next_value;
        value->kids++;
    }

    if (value->kids == 1)
    {
        tree_branch* cache = branch_cpy(value->next[0]);
        branch_full_clr(value);
        free(value);
        return cache;
    }

    return value;
}

tree_branch* get_P()
{
    if (*SRC_STR == '(')
    {
        SRC_STR++;
        tree_branch* value = get_E();
        assert(*SRC_STR == ')');
        SRC_STR++;
        return value;
    }
    return get_N();
}

tree_branch* get_N()
{
    for(int i = 0; i < FUNCTIONS_COUNT; i++)
        if (strstr(SRC_STR, FUNCTIONS[i].name) == SRC_STR)
        {
            tree_branch* branch = new_branch_construct();
            branch->data = calloc(1, sizeof(tree_function));
            *(tree_function*)branch->data = FUNCTIONS[i];
            SRC_STR += strlen(FUNCTIONS[i].name);
            branch->type = TREE_FUNCTION;

            add_kid(branch);
            branch->kids++;

            branch->next[0] = get_P();
            //printf("found %s on %d\n", ((tree_function*)branch->data)->name,((tree_function*)branch->data)->name);
            return branch;
        }


    if('a' <= *SRC_STR && *SRC_STR <= 'z' || 'A' <= *SRC_STR && *SRC_STR <= 'Z')
    {
        tree_branch* branch = 0;
        branch = make_char_branch(branch, *SRC_STR, TREE_VAR);
        SRC_STR++;
        return branch;
    }
    double value = 0;

    int str_add = 0;
    sscanf(SRC_STR, "%lf%n", &value, &str_add);
    SRC_STR += str_add;
    tree_branch* branch = 0;
    branch = make_value_branch(branch, value);
    return branch;
}

void get_calc_string(char* str)
{
    memset(SRC_STR, 0, SRC_STR_LEN);
    char* local_str = SRC_STR;
    for (int i = 0; str[i] != 0; i++)
    {
        if (str[i] != ' ' && str[i] != '\n' && str[i] != 0)
        {
            *local_str = str[i];
            *local_str++;
        }
    }

}
