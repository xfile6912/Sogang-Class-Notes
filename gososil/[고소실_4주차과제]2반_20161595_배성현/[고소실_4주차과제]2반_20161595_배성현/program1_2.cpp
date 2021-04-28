#include "my_solver.h"

extern double (*_f)(double);
extern double (*_fp)(double);

/*********************************************
  Secant Method
**********************************************/
void program1_2(FILE* fp) {
	double x0, x1, x2;
	int n = 0;
	scanf("%lf %lf", &x0, &x1);
	fprintf(fp, "i                 xn2                      |f(xn2)|\n");
	while (1)
	{
		x2 = x1 - _f(x1) * ((x1 - x0) / (_f(x1) - _f(x0)));
		n++;
		fprintf(fp, "%d %20.18e %12.10e\n", n, x2, fabs(_f(x2)));
		if (fabs(_f(x2)) < DELTA)
			break;
		if (n == Nmax)
			break;
		if (fabs(x2 - x1) < EPSILON)
			break;
		x0 = x1;
		x1 = x2;
	}
	printf("%d %20.18e %12.10e\n", n, x2, fabs(_f(x2)));
}
