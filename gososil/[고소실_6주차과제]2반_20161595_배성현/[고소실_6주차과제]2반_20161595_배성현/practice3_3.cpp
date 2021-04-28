
#include "my_solver.h"

#define SOLNUMS 3
#define MATCOLS SOLNUMS
#define MATROWS SOLNUMS
#define TOLERANCE 0.0000001

void fcn3_3(int* n, double* x, double* fvec, double* fjac, int* ldfjac, int* iflag)
{
	// origin function F(x)
	if (*iflag == 1) {
		/********************************/
		fvec[0] = exp(2 * x[0]) - x[1] + 4;
		fvec[1] = x[1] - x[2]*x[2]  - 1;
		fvec[2] = x[2] - sin(x[0]);

		/********************************/
	}
	// Jacobi matrix J(x)
	else if (*iflag == 2) {
		/********************************/
		fjac[0] = 2 * exp(2 * x[0]);
		fjac[1] = 0;
		fjac[2] = - cos(x[0]);
		fjac[3] = -1;
		fjac[4] = 1;
		fjac[5] = 0;
		fjac[6] = 0;
		fjac[7] = - 2 * x[2];
		fjac[8] = 1;
		/********************************/
	}
}

void practice3_3(void) {
	int n = SOLNUMS;
	double x[SOLNUMS] = { 0.0, 0.0, 0.0 };	//need to initilize x0
	double fvec[SOLNUMS], fjac[MATCOLS * MATROWS];
	int ldfjac = SOLNUMS;
	double tol = TOLERANCE;
	int info;
	double wa[(SOLNUMS * (SOLNUMS + 13)) / 2];
	int lwa = (SOLNUMS * (SOLNUMS + 13)) / 2;

	FILE* fp_w = fopen("roots_3-3.txt", "w");
	if (fp_w == NULL) {
		printf("%s file open error...\n", "roots_3-3.txt");
		return;
	}

	/********************************/
	hybrj1_(fcn3_3, &n, x, fvec, fjac, &ldfjac, &tol, &info, wa, &lwa);
	double f1= exp(2 * x[0]) - x[1] + 4;
	double f2 = x[1] - x[2] * x[2] - 1;
	double f3 = x[2] - sin(x[0]);
	
	fprintf(fp_w, "초기값 1일 때(0.0, 0.0, 0.0)\n");
	fprintf(fp_w, "info: %d\n", info);
	fprintf(fp_w, "x1=%.15lf, x2=%.15lf, x3=%.15lf\n", x[0], x[1], x[2]);
	fprintf(fp_w, "|f1|=%.15lf, |f2|=%.15lf, |f3|=%.15lf\n", fabs(f1), fabs(f2), fabs(f3));
	//////////////////////////////////
	x[0] = 1.55;
	x[1] = 1.39;
	x[2] = 1.10;

	hybrj1_(fcn3_3, &n, x, fvec, fjac, &ldfjac, &tol, &info, wa, &lwa);
	f1 = exp(2 * x[0]) - x[1] + 4;
	f2 = x[1] - x[2] * x[2] - 1;
	f3 = x[2] - sin(x[0]);
	fprintf(fp_w, "초기값 2일 때(1.55, 1.39, 1.10)\n");
	fprintf(fp_w, "info: %d\n", info);
	fprintf(fp_w, "x1=%.15lf, x2=%.15lf, x3=%.15lf\n", x[0], x[1], x[2]);
	fprintf(fp_w, "|f1|=%.15lf, |f2|=%.15lf, |f3|=%.15lf\n", fabs(f1), fabs(f2), fabs(f3));

	/********************************/

	fclose(fp_w);
}
