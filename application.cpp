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
        printf("Имеем: y = %s\n", str);
        printf("Дифференцируем данную функцию? [Y/N] ");
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
        printf("Какую функцию будем дифференцировать?\n\ty = ");
        scanf(" %[^\n]", str);
        str = (char*)realloc(str, strlen(str));
        assert(str);
        need_free_str = true;
    }
    char variable = 0;
    printf("По какой переменной дифференцируем?\n");
    scanf(" %c", &variable);
    int count = 0;
    printf("Сколько производных берем?\n");
    scanf("%d", &count);

    char* var_str = (char*)calloc(sizeof(char), MAX_FUNC_LENGTH / 50);
    char var = 0;

    double var_vals[26] = {};

    printf("Напиши значения переменных. Пример: \"x = 5\". По умолчанию все переменные равны нулю.\nНапиши 0, когда закончишь.\n");
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
    printf("Отправляем результат на почту? [Y/N]: ");
    scanf(" %c", &c);
    char* email_address = (char*)calloc(MAX_FUNC_LENGTH, sizeof(char));
    if (c == 'y' || c == 'Y')
    {
        printf("Укажи адрес электронной почты получателя: ");
        scanf(" %s", email_address);
        FILE* receiver = fopen("receiver.txt", "w");
        fprintf(receiver, "%s", email_address);
        fclose(receiver);
        send_email = true;
    }
    free(var_str);
    printf("Поiхали");
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
        printf("Отправляем");
        for(int i = 0; i < 3; ++i)
        {
            Sleep(300);
            printf(" .");
        }
        system("py smtp.py");

        printf("\nУспех!\n");
    }
    printf("Хорошего вечера!\n");
}
