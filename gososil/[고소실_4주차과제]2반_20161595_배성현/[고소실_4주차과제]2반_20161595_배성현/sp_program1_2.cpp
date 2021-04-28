#include "my_solver.h"

extern float (*_sp_f)(float);
extern float (*_sp_fp)(float);

/*********************************************
  Secant Method
**********************************************/
void sp_program1_2(FILE* fp) {
	float x0, x1, x2;
	int n = 0;
	scanf("%f %f", &x0, &x1);
	fprintf(fp, "i                 xn2                      |f(xn2)|\n");
	while (1)
	{
		x2 = x1 - _sp_f(x1) * ((x1 - x0) / (_sp_f(x1) - _sp_f(x0)));
		n++;
		fprintf(fp, "%d %20.18e %12.10e\n", n, x2, fabsf(_sp_f(x2)));
		if (fabsf(_sp_f(x2)) < DELTA)
			break;
		if (n == Nmax)
			break;
		if (fabsf(x2 - x1) < EPSILON)
			break;
		x0 = x1;
		x1 = x2;
	}
	printf("%d %20.18e %12.10e\n", n, x2, fabsf(_sp_f(x2)));
}
