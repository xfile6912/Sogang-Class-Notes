#include "my_solver.h"
#define SOLNUMS 4
#define MATCOLS SOLNUMS
#define MATROWS SOLNUMS
#define TOLERANCE 0.0000001
#define index_2d(row, col, dim) row*dim+col

void program3_4()
{
	int i, ia, j, n;
	float* a, * copya;
	float* b, * copyb;
	float* x;
	float* s;
	int* l;


	FILE* fp_r = fopen("linear_system_3-4.txt", "r");
	if (fp_r == NULL) {
		printf("%s file open error...\n", "linear_system_3-4.txt");
		return;
	}
	FILE* fp_w = fopen("solution_3-4.txt", "w");
	if (fp_w == NULL) {
		printf("%s file open error...\n", "solution_3-4.txt");
		return;
	}
	fscanf(fp_r, "%d", &n);
	a = (float*)malloc(sizeof(float) * n * n);
	copya = (float*)malloc(sizeof(float) * n * n);
	b = (float*)malloc(sizeof(float) * n);
	copyb = (float*)malloc(sizeof(float) * n);
	x = (float*)malloc(sizeof(float) * n);
	s = (float*)malloc(sizeof(float) * n);
	l = (int*)malloc(sizeof(int) * n);
	ia = n;
	for (i = 0; i < n; i++)
	{
		for (j = 0; j < n; j++)
		{
			fscanf(fp_r, "%f", &a[index_2d(j, i, n)]);
			copya[index_2d(j, i, n)] = a[index_2d(j, i, n)];
		}
	}
	for (i = 0; i < n; i++)
	{
		fscanf(fp_r, "%f", &b[i]);
		copyb[i] = b[i];
	}

	gespp_(&n, a, &ia, l, s);
	solve_(&n, a, &ia, l, b, x);
	fprintf(fp_w, "%d\n", n);
	for (i = 0; i < n; i++)
	{
		fprintf(fp_w, "%f\n", x[i]);
	}

	float ax_b_ = 0;
	float b_ = 0;
	for (i = 0; i < n; i++)
	{
		float temp = 0;
		for (j = 0; j < n; j++)
		{
			temp += copya[index_2d(j, i, n)] * x[j];
		}
		temp -= copyb[i];
		ax_b_ += temp * temp;
	}
	ax_b_ = sqrt(ax_b_);
	for (i = 0; i < n; i++)
		b_ += copyb[i] * copyb[i];
	b_ = sqrt(b_);

	fprintf(fp_w, "%f", ax_b_ / b_);
	free(a);
	free(copya);
	free(copyb);
	free(b);
	free(x);
	free(s);
	free(l);
}