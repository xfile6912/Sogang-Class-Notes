#include "my_solver.h"

extern double (*_f)(double);
extern double (*_fp)(double);

/*********************************************
**********************************************/
void program1_3(FILE* fp) {
	double a, b, x0, x1;
	int n = 0;
	scanf("%lf %lf", &a, &b);
	fprintf(fp, "i                 xn2                      |f(xn2)|\n");
	x0 = (a + b) / 2;
	if (_f(a) * _f(x0) < 0)
	{
		b = x0;
	}
	else if (_f(b) * _f(x0) < 0)
	{
		a = x0;
	}
	while (1)
	{
		x1 = (a + b) / 2;
		if (_f(a) * _f(x1) < 0)
		{
			b = x1;
		}
		else if (_f(b) * _f(x1) < 0)
		{
			a = x1;
		}
		n++;
		fprintf(fp, "%d %20.18e %12.10e\n", n, x1, fabs(_f(x1)));
		if (fabs(_f(x1)) < DELTA)
			break;
		if (n == Nmax)
			break;
		if (fabs(x1 - x0) < EPSILON)
			break;
		x0 = x1;
	}
	printf("%d %20.18e %12.10e\n", n, x1, fabs(_f(x1)));
}