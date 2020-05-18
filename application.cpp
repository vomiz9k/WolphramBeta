#include <assert.h>
#include <cstring>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <windows.h>
#include "differentiator.h"

int MAX_FUNC_LENGTH = 1000;

int working_with_func(char* str)
{
    bool need_free_str = false;
    if (str != 0)
    {
        printf("�����: y = %s\n", str);
        printf("�������������� ������ �������? [Y/N] ");
        char c = 0;
        scanf("%c", &c);
        if (c != 'y' && c != 'Y')
        {
            str = 0;
        }
    }
    if (str == 0)
    {
        str = (char*)calloc(sizeof(char), MAX_FUNC_LENGTH);
        assert(str);
        printf("����� ������� ����� ����������������?\n\ty = ");
        scanf(" %[^\n]", str);
        str = (char*)realloc(str, strlen(str));
        assert(str);
        need_free_str = true;
    }
    char variable = 0;
    printf("�� ����� ���������� ��������������?\n");
    scanf(" %c", &variable);
    int count = 0;
    printf("������� ����������� �����?\n");
    scanf("%d", &count);

    char* var_str = (char*)calloc(sizeof(char), MAX_FUNC_LENGTH / 50);
    char var = 0;

    double var_vals[26] = {};

    printf("������ �������� ����������. ������: \"x = 5\". �� ��������� ��� ���������� ����� ����.\n������ 0, ����� ���������.\n");
    while(*var_str != '0')
    {
        scanf(" %[^\n]", var_str);
        if (*var_str == '0')
            break;
        else
        {
            var = *var_str;
            sscanf(var_str + 3, " %lf", &(var_vals[var - 'a']));
        }
    }

    char c = 0;
    bool send_email = false;
    printf("���������� ��������� �� �����? [Y/N]: ");
    scanf(" %c", &c);
    char* email_address = (char*)calloc(MAX_FUNC_LENGTH, sizeof(char));
    if (c == 'y' || c == 'Y')
    {
        printf("����� ����� ����������� ����� ����������: ");
        scanf(" %s", email_address);
        FILE* receiver = fopen("receiver.txt", "w");
        fprintf(receiver, "%s", email_address);
        fclose(receiver);
        send_email = true;
    }
    free(var_str);
    printf("��i����");
    Sleep(300);
    printf(" .");
    Sleep(300);
    printf(" .");
    Sleep(300);
    printf(" .\n\n");

    tree_branch* branch = get_G(str);
    latex_maker(branch, variable, count, var_vals);

    if (need_free_str)
        free(str);


    printf("\n\n---------------------------------\n");
    if(send_email)
    {
        printf("����������");
        for(int i = 0; i < 3; ++i)
        {
            Sleep(300);
            printf(" .");
        }
        system("py smtp.py");

        printf("\n�����!\n");
    }
    printf("�������� ������!\n");
}
