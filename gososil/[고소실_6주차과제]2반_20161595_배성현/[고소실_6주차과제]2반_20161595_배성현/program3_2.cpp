#include "my_solver.h"
#define SOLNUMS 4
#define MATCOLS SOLNUMS
#define MATROWS SOLNUMS
#define TOLERANCE 0.0000001

void fcn3_2_hw(int* n, double* x, double* fvec, int* iflag)
{
	/********************************/
	fvec[0] = x[0] + 10 * x[1] + 9;
	fvec[1] = sqrt(5) * (x[2] - x[3]) - 2 * sqrt(5);
	fvec[2] = pow(x[1] - 2 * x[2], 2) - 9;
	fvec[3] = sqrt(10) * pow(x[0] - x[3], 2) - 2 * sqrt(10);
	/********************************/
}
void program3_2()
{
	int n = SOLNUMS;
	double x[SOLNUMS] = { 0.9,-0.9,1.25,-1.25 };
	double fvec[SOLNUMS];
	double tol = TOLERANCE;
	int info;
	double wa[(SOLNUMS * (SOLNUMS * 5 + 13)) / 2];
	int lwa = (SOLNUMS * (SOLNUMS * 5 + 13)) / 2;

	FILE* fp_w = fopen("roots_found_3-2.txt", "w");
	if (fp_w == NULL) {
		printf("%s file open error...\n", "roots_found_3-2.txt");
		return;
	}




	hybrd1_(fcn3_2_hw, &n, x, fvec, &tol, &info, wa, &lwa);
	double f1 = x[0] + 10 * x[1] + 9;
	double f2 = sqrt(5) * (x[2] - x[3]) - 2 * sqrt(5);
	double f3 = pow(x[1] - 2 * x[2], 2) - 9;
	double f4 = sqrt(10) * pow(x[0] - x[3], 2) - 2 * sqrt(10);
	fprintf(fp_w, "info: %d\n", info);
	fprintf(fp_w, "x1=%.15lf, x2=%.15lf, x3=%.15lf, x4=%.15lf\n", x[0], x[1], x[2], x[3]);
	fprintf(fp_w, "|f1|=%.15lf, |f2|=%.15lf, |f3|=%.15lf, |f4|=%.15lf\n\n", fabs(f1), fabs(f2), fabs(f3), fabs(f4));
	if (fp_w != NULL) fclose(fp_w);
}