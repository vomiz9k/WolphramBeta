#include <assert.h>
#include <cstring>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <windows.h>
#include "differentiator.h"


int main()
{
    setlocale(LC_ALL, "Russian");

    char* func = "sin(x^3)+(cos(10*x))^2";
    working_with_func(func);
}






