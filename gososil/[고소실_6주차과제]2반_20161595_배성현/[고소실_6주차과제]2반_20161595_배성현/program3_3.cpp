#include "my_solver.h"
#define SOLNUMS 2
#define MATCOLS SOLNUMS
#define MATROWS SOLNUMS
#define TOLERANCE 0.0000001

void fcn3_3_hw(int* n, double* x, double* fvec, double* fjac, int* ldfjac, int* iflag)
{

	if (*iflag == 1) {
		/********************************/
		fvec[0] = (sin(x[0] * x[1] + M_PI / 6) + (sqrt(x[0] * x[0] * x[1] * x[1] + 1))) / cos(x[0] - x[1]) + 2.8;
		fvec[1] = (x[0] * exp(x[0] * x[1] + M_PI / 6.0) - sin(x[0] - x[1])) / sqrt(x[0] * x[0] * x[1] * x[1] + 1) - 1.66;
		/********************************/
	}
	// Jacobi matrix J(x)
	else if (*iflag == 2) {
		/*********************************/
		fjac[0] = -(sqrt(x[0] * x[0] * x[1] * x[1] + 1) * (sin((6 * x[0] * x[1] + M_PI) / 6)) + (x[0] * x[0] * x[1] * x[1] + 1) * sin(x[1] - x[0]) - x[0] * x[1] * x[1] * cos(x[1] - x[0])) / (sqrt(x[0] * x[0] * x[1] * x[1] + 1) * cos(x[1] - x[0]) * cos(x[1] - x[0]));
		fjac[1] = -(sqrt(x[0] * x[0] * x[1] * x[1] + 1) * (x[0] * x[1] * x[1] * sin(x[1] - x[0]) + (x[0] * x[0] * x[1] * x[1] + 1) * cos(x[1] - x[0]) + ((-exp(M_PI / 6) * pow(x[0], 3) * pow(x[1], 3)) - exp(M_PI / 6) * x[0] * x[1] - exp(M_PI / 6)) * exp(x[0] * x[1]))) / (pow(x[0], 4) * pow(x[1], 4) + 2 * x[0] * x[0] * x[1] * x[1] + 1);
		fjac[2] = (sqrt(x[0] * x[0] * x[1] * x[1] + 1) * (sin(x[1] - x[0]) * sin((6 * x[0] * x[1] + M_PI) / 6) + x[0] * cos(x[1] - x[0]) * cos((6 * x[0] * x[1] + M_PI) / 6)) + (x[0] * x[0] * x[1] * x[1] + 1) * sin(x[1] - x[0]) + x[0] * x[0] * x[1] * cos(x[1] - x[0])) / (sqrt(x[0] * x[0] * x[1] * x[1] + 1) * cos(x[1] - x[0]) * cos(x[1] - x[0]));
		fjac[3] = -(sqrt(x[0] * x[0] * x[1] * x[1] + 1) * (x[0] * x[0] * x[1] * sin(x[1] - x[0]) + ((-x[0] * x[0] * x[1] * x[1]) - 1) * cos(x[1] - x[0]) + ((-exp(M_PI / 6) * pow(x[0], 4) * x[1] * x[1]) + exp(M_PI / 6) * pow(x[0], 3) * x[1] - exp(M_PI / 6) * x[0] * x[0]) * exp(x[0] * x[1]))) / (pow(x[0], 4) * pow(x[1], 4) + 2 * x[0] * x[0] * x[1] * x[1] + 1);
		/*********************************/
	}
}

void program3_3()
{
	int n = SOLNUMS;
	double x[SOLNUMS] = { 20.0,0.0 };
	double fvec[SOLNUMS], fjac[MATCOLS * MATROWS];
	int ldfjac = SOLNUMS;
	double tol = TOLERANCE;
	int info;
	double wa[(SOLNUMS * (SOLNUMS * 5 + 13)) / 2];
	int lwa = (SOLNUMS * (SOLNUMS * 5 + 13)) / 2;

	FILE* fp_w = fopen("roots_found_3-3.txt", "w");
	if (fp_w == NULL) {
		printf("%s file open error...\n", "roots_found_3-3.txt");
		return;
	}


	hybrj1_(fcn3_3_hw, &n, x, fvec, fjac, &ldfjac, &tol, &info, wa, &lwa);
	double f1 = (sin(x[0] * x[1] + M_PI / 6) + (sqrt(x[0] * x[0] * x[1] * x[1] + 1))) / cos(x[0] - x[1]) + 2.8;
	double f2 = (x[0] * exp(x[0] * x[1] + M_PI / 6.0) - sin(x[0] - x[1])) / sqrt(x[0] * x[0] * x[1] * x[1] + 1) - 1.66;
	fprintf(fp_w, "info: %d\n", info);
	fprintf(fp_w, "x1=%.15lf, x2=%.15lf\n", x[0], x[1]);
	fprintf(fp_w, "|f1|=%.15lf, |f2|=%.15lf\n\n", fabs(f1), fabs(f2));


	if (fp_w != NULL) fclose(fp_w);
}