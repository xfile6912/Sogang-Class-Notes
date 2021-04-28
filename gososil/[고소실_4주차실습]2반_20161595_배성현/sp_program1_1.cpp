#include "my_solver.h"

extern float (*_sp_f)(float);
extern float (*_sp_fp)(float);

/*********************************************
  Newton-Rapson Method
**********************************************/
void sp_program1_1(FILE* fp) {
	float x0, x1;
	int n = 0;
	scanf("%f", &x0);
	fprintf(fp, "i                 xn1                      |f(xn1)|\n");
	while (1)
	{
		x1 = x0 - (_sp_f(x0) / _sp_fp(x0));
		n++;
		fprintf(fp, "%d %20.18e %12.10e\n", n, x1, fabsf(_sp_f(x1)));
		if (fabsf(_sp_f(x1)) < DELTA)
			break;
		if (n == Nmax)
			break;
		if (fabsf(x1 - x0) < EPSILON)
			break;
		x0 = x1;
	}
	printf("%d %20.18e %12.10e\n", n, x1, fabsf(_sp_f(x1)));
}
