#include "my_solver.h"

extern double (*_f)(double);
extern double (*_fp)(double);

/*********************************************
  Newton-Rapson Method
**********************************************/
void program1_1(FILE* fp) {
	double x0, x1;
	int n=0;
	scanf("%lf", &x0);
	fprintf(fp, "i                 xn1                      |f(xn1)|\n");
	while (1)
	{
		x1 = x0 - (_f(x0) / _fp(x0));
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
	printf("%d %20.18e %12.10e\n", n ,x1, fabs(_f(x1)));
}
