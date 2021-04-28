#define _CRT_SECURE_NO_WARNINGS
#ifndef __MY_SOLVER_H__
#define __MY_SOLVER_H__

#include <stdio.h>
#include <stdlib.h>

#define _USE_MATH_DEFINES
#include <math.h>

extern "C"
{
	int gespp_(int*, float*, int*, int*, float*);
	int solve_(int*, float*, int*, int*, float*, float*);
	int rpoly_(double*, int*, double*, double*, long int*);
	int hybrj1_(void fcn(int*, double*, double*, double*, int*, int*), int*, double*, double*, double*, int*, double*, int*, double*, int*);
	int hybrd1_(void fcn(int*, double*, double*, int*), int*, double*, double*, double*, int*, double*, int*);
};

void practice3_1();
void practice3_2();
void practice3_3();
void practice3_4();
void practice3_5();
void practice3_6();
void practice3_7();
void practice3_8();

#endif