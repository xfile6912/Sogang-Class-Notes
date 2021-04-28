#include <stdio.h>
#include <random>
#include <time.h>
#include <math.h>
#include <time.h>
#include <Windows.h>

#define N 25
double Taylor_series(double x, int n);
double Taylor_series_ex(double x, int n);
__int64 start, freq, end;
#define CHECK_TIME_START QueryPerformanceFrequency((LARGE_INTEGER*)&freq); QueryPerformanceCounter((LARGE_INTEGER*)&start)
#define CHECK_TIME_END(a) QueryPerformanceCounter((LARGE_INTEGER*)&end); a = (float)((float)(end - start) / (freq / 1000.0f))
float compute_time;

void main(void)
{
	CHECK_TIME_START;
	printf("*** f<-8.3> = %.6e \n", Taylor_series(-8.3, N));
	CHECK_TIME_END(compute_time);
	printf("***Polynomial Naive  compute time : %f ms\n", compute_time);
	CHECK_TIME_START;
	printf("*** f<-8.3> = %.6e \n", Taylor_series_ex(-8.3, N));
	CHECK_TIME_END(compute_time);
	printf("***Polynomial Naive  compute time : %f ms\n", compute_time);
	printf("*** f<-8.3> = %.6e \n", pow(2.71828182846, -8.3));
}




double Taylor_series(double x, int n)
{
	int i, j;
	double result;
	int flag = 0;
	result = 0;
	if (x < 0)
	{
		x *= -1;
		flag = 1;
	}
	for (i = 0; i <= n; i++)
	{
		double temp = pow(x, i);
		for (j = 1; j <= i; j++)
		{
			temp /= j;
		}
		result += temp;
	}
	if (flag == 1)
		result = 1 / result;
	return result;
}


double Taylor_series_ex(double x, int n)
{
	int i, j;
	float result;
	int flag = 0;
	if (x < 0)
	{
		flag = 1;
		x *= -1;
	}
	result = 1;
	for (i = n; i>=1; i--)
	{
		result = result * ((float)x )/ i + 1;
	}
	if (flag == 1)
		result = 1 / result;
	return result;
}
