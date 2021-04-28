#include <stdio.h>
#include <string.h>
#include <random>
#include <time.h>

#include <math.h>
#include <time.h>
#include <Windows.h>

__int64 start, freq, end;
#define CHECK_TIME_START QueryPerformanceFrequency((LARGE_INTEGER*)&freq); QueryPerformanceCounter((LARGE_INTEGER*)&start)
#define CHECK_TIME_END(a) QueryPerformanceCounter((LARGE_INTEGER*)&end); a = (float)((float)(end - start) / (freq / 1000.0f))
float compute_time;
float compute_time1, compute_time2;


#define MATDIM 1024
#define HW1_N 100000
#define N 100000
double *hw1_x, hw1_E, hw1_var1, hw1_var2;
double hw2_a, hw2_b, hw2_c, hw2_naive_ans[2], hw2_pre_ans[2];

/* hw1 */
void init_hw1(int fixed);
void hw1_calc_e();
void hw1_calc_var1();
void hw1_calc_var2();
/* hw2 */
void hw2_naive();
void hw2_safe();
double hw2_verify(double x);
/* hw3 */
void method1();
void method2();
void method3();
void method4();
void method5();

void make_sample()
{
	hw1_x = (double*)malloc(sizeof(double) * HW1_N);
	double tmp = 10;
	for (int i = 0; i < HW1_N; i++)
	{
		if (i == 1) tmp -= 0.000001;
		hw1_x[i] = tmp;
	}
}

void main(void)
{
	srand((unsigned)time(NULL));

	/* hw1 */
	puts("====== hw1 ======");
	init_hw1(1);
	//make_sample();
	CHECK_TIME_START;
	hw1_calc_var1();
	CHECK_TIME_END(compute_time);
	compute_time1 = compute_time;
	printf("hw1_calc_var1 = %f ms\n", compute_time);
	printf("hw1_calc_var1 value = %.15f\n", hw1_var1);


	CHECK_TIME_START;
	hw1_calc_e();
	hw1_calc_var2();
	CHECK_TIME_END(compute_time);
	compute_time2 = compute_time;
	printf("hw1_calc_var2 = %f ms\n", compute_time);
	printf("hw1_calc_var2 value = %.15f\n", hw1_var2);
	puts("");
	
	/* hw2 */
	puts("====== hw2 ======");
	printf("a, b, c : ");
	scanf("%lf %lf %lf", &hw2_a, &hw2_b, &hw2_c);
	hw2_naive();
	hw2_safe();
	printf("naive result    : %.15f, %.15f\n", hw2_naive_ans[0], hw2_naive_ans[1]);
	printf("advanced result : %.15f, %.15f\n", hw2_pre_ans[0], hw2_pre_ans[1]);
	puts("");
	printf("Verifying naive ans    : %.15f, %.15f\n", hw2_verify(hw2_naive_ans[0]), hw2_verify(hw2_naive_ans[1]));
	printf("Verifying advanced ans : %.15f, %.15f\n", hw2_verify(hw2_pre_ans[0]), hw2_verify(hw2_pre_ans[1]));
	puts("");

	/* hw3 */
	method1();
	method2();
	method3();
	method4();
	method5();
}

void init_hw1(int fixed)
{
	double *ptr;
	hw1_x = (double *)malloc(sizeof(double)*HW1_N);

	if (fixed)
	{
		double tmp = HW1_N;
		for (int i = 0; i < HW1_N; i++)
		{
			if( i & 1) tmp -= 0.0001;
			hw1_x[i] = tmp;
		}
	}
	else
	{
		srand((unsigned)time(NULL));
		ptr = hw1_x;
		for (int i = 0; i < HW1_N; i++)
			*ptr++ = ((double)rand() / (double)RAND_MAX) * 2;
	}
}
void hw1_calc_e()
{
	int i;
	double sum = 0;
	for (i = 0; i < HW1_N; i++)
	{
		sum += hw1_x[i];
	}
	hw1_E = sum / HW1_N;
}
void hw1_calc_var1()
{

	int i;
	double powSum = 0;
	double sum = 0;
	for (i = 0; i < HW1_N; i++)
	{
		powSum += hw1_x[i] * hw1_x[i];
		sum += hw1_x[i];
	}
	//printf("n*powSum=%.15f, sum*sum=%.15f\n", HW1_N * powSum, sum * sum);
	hw1_var1 = (HW1_N * powSum - sum * sum) / HW1_N/ (HW1_N - 1);

}
void hw1_calc_var2()
{
	int i;
	double sum=0;
	for (i = 0; i < HW1_N; i++)
	{
		sum += (hw1_x[i] - hw1_E)*(hw1_x[i] - hw1_E);

	}
	hw1_var2 = sum / (HW1_N - 1);
}



/* hw2 */
void hw2_naive()
{
	hw2_naive_ans[0] = (-hw2_b - sqrt(hw2_b * hw2_b - 4 * hw2_a * hw2_c)) / (2 * hw2_a);
	hw2_naive_ans[1] = (-hw2_b + sqrt(hw2_b * hw2_b - 4 * hw2_a * hw2_c)) / (2 * hw2_a);
}
void hw2_safe()
{
	if (hw2_b >= 0)
	{
		double temp1=(-hw2_b)- sqrt(hw2_b * hw2_b - 4 * hw2_a * hw2_c);
		hw2_pre_ans[0] =temp1 / (2 * hw2_a);
		double temp2 = ((-4)* hw2_a * hw2_c) / (hw2_b+ sqrt(hw2_b * hw2_b - 4 * hw2_a * hw2_c));
		hw2_pre_ans[1] = temp2 / (2 * hw2_a);

	}
	else
	{
		double temp1 = ((-4) * hw2_a * hw2_c) / (hw2_b - sqrt(hw2_b * hw2_b - 4 * hw2_a * hw2_c));
		hw2_pre_ans[0] = temp1 / (2 * hw2_a);
		double temp2 = (-hw2_b) + sqrt(hw2_b * hw2_b - 4 * hw2_a * hw2_c);
		hw2_pre_ans[1] = temp2 / (2 * hw2_a);
	}
}
double hw2_verify(double x)
{
	return hw2_a * x * x + hw2_b*x + hw2_c;
}
void method1()//locality
{
	int i, j, k;
	int n = 500;
	float* result = (float*)malloc(sizeof(float) * n * n);
	float* result1 = (float*)malloc(sizeof(float) * n * n);
	float* a = (float*)malloc(sizeof(float) * n * n);
	float* b = (float*)malloc(sizeof(float) * n * n);
	float* transb = (float*)malloc(sizeof(float) * n * n);
	int check = 1;
	for (i = 0; i < n; i++)
	{
		for (j = 0; j < n; j++)
		{
			a[i * n + j] = 2.7;
			b[i * n + j] = 2.7;
			result[i * n + j] = 0;
		}
	}
	CHECK_TIME_START;
	for (i = 0; i < n; i++)
	{
		for (j = 0; j < n; j++)
		{
			result[i * n + j] = 0;
			for (k = 0; k < n; k++)
			{
				result[i * n + j] += a[i * n + k] * b[k * n + j];
			}
		}
	}
	CHECK_TIME_END(compute_time1);
	for (i = 0; i < n; i++)
	{
		for (j = 0; j < n; j++)
		{
			a[i * n + j] = 2.7;
			b[i * n + j] = 2.7;
			result1[i * n + j] = result[i * n + j];
			result[i * n + j] = 0;
		}
	}
	for (i = 0; i < n; i++)
	{
		for (j = 0; j < n; j++)
		{
			transb[i * n + j] = b[j * n + i];
		}
	}//transb에 b의 전치행렬 만들어줌.
	CHECK_TIME_START;
	for (i = 0; i < n; i++)
	{
		for (j = 0; j < n; j++)
		{
			result[i * n + j] = 0;
			for (k = 0; k < n; k++)
			{
				result[i * n + j] += a[i * n + k] * transb[j * n + k];
			}
		}
	}
	CHECK_TIME_END(compute_time2);
	for (i = 0; i < n; i++)
	{
		for (j = 0; j < n; j++)
		{
			if (result[i * n + j] != result1[i * n + j])
			{
				check = 0;
				break;
			}
		}
	}
	printf("locality\n");
	if (check == 1)
	{

		printf("equal\n");
	}
	else
	{
		printf("not eqaul!\n");
	}
	printf("without locality = %f ms\n", compute_time1);
	printf("with locality = %f ms\n\n", compute_time2);
	free(result);
	free(result1);
	free(a);
	free(b);
	free(transb);
}
void method2()//code motion
{
	float a = 1.56;
	float b = 2.78;
	float c = 3.80;
	float result = 1;
	float temp = 0;
	int i = 0;
	int num = N;
	CHECK_TIME_START;
	while (i <= (num * 10) / 1.1)
	{
		result *= a;
		result /= a;
		result *= a;
		result /= a;
		result *= a;
		result /= a;
		result *= a;
		result *= a;
		result /= a;
		result *= a;
		result /= a;
		result *= a;
		result /= a;
		result *= a;
		i++;
	}
	CHECK_TIME_END(compute_time1);
	float result1 = result;
	a = 1.56;
	b = 2.78;
	c = 3.80;
	result = 1;
	temp = 0;
	i = 0;
	CHECK_TIME_START;
	temp = num * 10 / 1.1;
	while (i <= temp)
	{
		result *= a;
		result /= a;
		result *= a;
		result /= a;
		result *= a;
		result /= a;
		result *= a;
		result *= a;
		result /= a;
		result *= a;
		result /= a;
		result *= a;
		result /= a;
		result *= a;
		i++;
	}
	CHECK_TIME_END(compute_time2);
	printf("code motion\n");
	if (result1 == result)
	{
		printf("equal\n");

	}
	else
	{
		printf("not eqaul!\n");
	}
	printf("without code motion = %f ms\n", compute_time1);
	printf("with code motion = %f ms\n\n", compute_time2);
}
void method3()//strength reduction
{
	float a = 1.56;
	float b = 2.78;
	float c = 3.80;
	float result = 0;
	CHECK_TIME_START;
	for (int i = 0; i < N; i++)
	{
		result = i * a * b * c;
	}
	CHECK_TIME_END(compute_time1);
	float result1 = result;
	a = 1.56;
	b = 2.78;
	c = 3.80;
	result = 0;
	float temp = 0;
	CHECK_TIME_START;
	for (int i = 0; i < N; i++)
	{
		result = temp;
		temp = temp + a * b * c;
	}
	CHECK_TIME_END(compute_time2);
	printf("strength reduction\n");
	if (result1 == result)
	{
		printf("eqaul!\n");
	}
	else
	{
		printf("not eqaul!\n");
	}
	printf("without strength reduction = %f ms\n", compute_time1);
	printf("with strength reduction = %f ms\n\n", compute_time2);
}
void method4()//dead code elimination
{
	float a = 1.56;
	float b = 2.78;
	float c = 3.80;
	float d = 2.6;
	float result = 0;
	float temp = 0;
	CHECK_TIME_START;
	for (int i = 0; i < N; i++)
	{
		temp = a + b;
		a = temp - c;
		if (a == temp)//dead code
		{
			result -= temp + temp + a;
		}
		d = 5.77777 * temp * a * b / c;//dead code result에 영향 주는 코드 아니므로
		d = 5.35 * temp / a / b / c;
		result += a * temp;
	}
	CHECK_TIME_END(compute_time1);
	float result1 = result;
	a = 1.56;
	b = 2.78;
	c = 3.80;
	result = 0;
	temp = 0;
	CHECK_TIME_START;
	for (int i = 0; i < N; i++)
	{
		temp = a + b;
		a = temp - c;
		result += a * temp;
	}
	CHECK_TIME_END(compute_time2);
	printf("dead code elimination\n");
	if (result1 == result)
	{
		printf("eqaul!\n");
	}
	else
	{
		printf("not eqaul!\n");
	}
	printf("without dead code elimnation = %f ms\n", compute_time1);
	printf("with dead code elemination = %f ms\n\n", compute_time2);
}
void method5()//common subexpression elimination
{
	float a = 1.56;
	float b = 2.78;
	float c = 3.80;
	float result = 0;
	CHECK_TIME_START;
	for (int i = 0; i < N; i++)
	{
		a = a + pow(b * c, 4);
		result -= pow(b * c, 4) + pow(b * c, 4) + a;
		result += a * pow(b * c, 4);
	}
	CHECK_TIME_END(compute_time1);
	float result1 = result;
	a = 1.56;
	b = 2.78;
	c = 3.80;
	result = 0;
	float temp;
	CHECK_TIME_START;
	for (int i = 0; i < N; i++)
	{
		temp = pow(b * c, 4);
		a = a + temp;
		result -= temp + temp + a;
		result += a * temp;
	}
	CHECK_TIME_END(compute_time2);
	printf("subexpression elimination\n");
	if (result1 == result)
	{
		printf("eqaul!\n");

	}
	else
	{
		printf("not eqaul!\n");
	}
	printf("without subexpression elimination = %f ms\n", compute_time1);
	printf("with subexpression elimination = %f ms\n\n", compute_time2);
}