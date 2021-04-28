
#include "my_solver.h"

#define SOLNUMS 3
#define MATCOLS SOLNUMS
#define MATROWS SOLNUMS
#define TOLERANCE 0.0000001

void fcn3_5(int* n, double* x, double* fvec, int* iflag)
{
	/********************************/

	fvec[0] = 10 * x[0] - 2 * x[1] * x[1] + x[1] - 2 * x[2] - 5;
	fvec[1] = 8 * x[1] * x[1] + 4 * x[2] * x[2] - 9;
	fvec[2] = 8 * x[1] * x[2] + 4;
	/********************************/
}

void practice3_5(void)
{
	int n = SOLNUMS;
	double x[SOLNUMS] = { 1.0, -1.0, 1.0 };	//need to initilize x0
	double fvec[SOLNUMS];
	double tol = TOLERANCE;
	int info;
	double wa[(SOLNUMS * (3 * SOLNUMS + 13)) / 2];
	int lwa = (SOLNUMS * (3 * SOLNUMS + 13)) / 2;

	FILE* fp_w = fopen("roots_3-5.txt", "w");
	if (fp_w == NULL) {
		printf("%s file open error...\n", "roots_3-5.txt");
		return;
	}

	/********************************/
	hybrd1_(fcn3_5, &n, x, fvec, &tol, &info, wa, &lwa);
	double f1 = 10 * x[0] - 2 * x[1] * x[1] + x[1] - 2 * x[2] - 5;
	double f2 = 8 * x[1] * x[1] + 4 * x[2] * x[2] - 9;
	double f3 = 8 * x[1] * x[2] + 4;
	fprintf(fp_w, "초기값 1일 때(1.0, -1.0, 1.0)\n");
	fprintf(fp_w, "info: %d\n", info);
	fprintf(fp_w, "x1=%.15lf, x2=%.15lf, x3=%.15lf\n", x[0], x[1], x[2]);
	fprintf(fp_w, "|f1|=%.15lf, |f2|=%.15lf, |f3|=%.15lf\n", fabs(f1), fabs(f2), fabs(f3));
	//////////////////////////////////
	x[0] = 1.0;
	x[1] = 1.0;
	x[2] = -1.0;

	hybrd1_(fcn3_5, &n, x, fvec, &tol, &info, wa, &lwa);
	f1 = 10 * x[0] - 2 * x[1] * x[1] + x[1] - 2 * x[2] - 5;
	f2 = 8 * x[1] * x[1] + 4 * x[2] * x[2] - 9;
	f3 = 8 * x[1] * x[2] + 4;
	fprintf(fp_w, "초기값 2일 때(1.0, 1.0, -1.0)\n");
	fprintf(fp_w, "info: %d\n", info);
	fprintf(fp_w, "x1=%.15lf, x2=%.15lf, x3=%.15lf\n", x[0], x[1], x[2]);
	fprintf(fp_w, "|f1|=%.15lf, |f2|=%.15lf, |f3|=%.15lf\n", fabs(f1), fabs(f2), fabs(f3));


	/********************************/

	fclose(fp_w);
}