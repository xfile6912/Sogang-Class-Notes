#include "my_solver.h"
#define SOLNUMS 4
#define MATCOLS SOLNUMS
#define MATROWS SOLNUMS
#define TOLERANCE 0.0000001

double C, b;
double p1[4];
double p2[4];
double p3[4];
double t[4];
double tr[4];
char readfile[256];
char writefile1[256];
char writefile2[256];
void fcn3_1_1(int* n, double* x, double* fvec, double* fjac, int* ldfjac, int* iflag)
{
	// origin function F(x)
	// origin function F(x)
	if (*iflag == 1) {
		/********************************/
		fvec[0] = pow(x[0] - p1[0], 2) + pow(x[1] - p2[0], 2) + pow(x[2] - p3[0], 2) - pow(C * (x[3] + tr[0] - t[0]), 2);
		fvec[1] = pow(x[0] - p1[1], 2) + pow(x[1] - p2[1], 2) + pow(x[2] - p3[1], 2) - pow(C * (x[3] + tr[1] - t[1]), 2);
		fvec[2] = pow(x[0] - p1[2], 2) + pow(x[1] - p2[2], 2) + pow(x[2] - p3[2], 2) - pow(C * (x[3] + tr[2] - t[2]), 2);
		fvec[3] = pow(x[0] - p1[3], 2) + pow(x[1] - p2[3], 2) + pow(x[2] - p3[3], 2) - pow(C * (x[3] + tr[3] - t[3]), 2);
		/********************************/
	}
	// Jacobi matrix J(x)
	else if (*iflag == 2) {
		/********************************/
		fjac[0] = 2 * (x[0] - p1[0]);
		fjac[1] = 2 * (x[0] - p1[1]);
		fjac[2] = 2 * (x[0] - p1[2]);
		fjac[3] = 2 * (x[0] - p1[3]);
		fjac[4] = 2 * (x[1] - p2[0]);
		fjac[5] = 2 * (x[1] - p2[1]);
		fjac[6] = 2 * (x[1] - p2[2]);
		fjac[7] = 2 * (x[1] - p2[3]);
		fjac[8] = 2 * (x[2] - p3[0]);
		fjac[9] = 2 * (x[2] - p3[1]);
		fjac[10] = 2 * (x[2] - p3[2]);
		fjac[11] = 2 * (x[2] - p3[3]);
		fjac[12] = (-2) * C * C * (x[3] + tr[0] - t[0]);
		fjac[13] = (-2) * C * C * (x[3] + tr[1] - t[1]);
		fjac[14] = (-2) * C * C * (x[3] + tr[2] - t[2]);
		fjac[15] = (-2) * C * C * (x[3] + tr[3] - t[3]);
		/********************************/
	}
}

void fcn3_1_2(int* n, double* x, double* fvec, int* iflag)
{
	/********************************/
	fvec[0] = pow(x[0] - p1[0], 2) + pow(x[1] - p2[0], 2) + pow(x[2] - p3[0], 2) - pow(C * (x[3] + tr[0] - t[0]), 2);
	fvec[1] = pow(x[0] - p1[1], 2) + pow(x[1] - p2[1], 2) + pow(x[2] - p3[1], 2) - pow(C * (x[3] + tr[1] - t[1]), 2);
	fvec[2] = pow(x[0] - p1[2], 2) + pow(x[1] - p2[2], 2) + pow(x[2] - p3[2], 2) - pow(C * (x[3] + tr[2] - t[2]), 2);
	fvec[3] = pow(x[0] - p1[3], 2) + pow(x[1] - p2[3], 2) + pow(x[2] - p3[3], 2) - pow(C * (x[3] + tr[3] - t[3]), 2);
	/********************************/
}

void hybrj()
{

	int n = SOLNUMS;
	double x[SOLNUMS];
	double fvec[SOLNUMS], fjac[MATCOLS * MATROWS];
	int ldfjac = SOLNUMS;
	double tol = TOLERANCE;
	int info;
	double wa[(SOLNUMS * (SOLNUMS + 13)) / 2];
	int lwa = (SOLNUMS * (SOLNUMS + 13)) / 2;
	FILE* fp_w1 = fopen(writefile1, "w");
	if (fp_w1 == NULL) {
		printf("%s file open error...\n", writefile1);
		return;
	}
	printf("\ninput initial value(%s, hybrj1):", readfile);
	scanf("%lf %lf %lf %lf", &x[0], &x[1], &x[2], &x[3]);//입력받음.

	hybrj1_(fcn3_1_1, &n, x, fvec, fjac, &ldfjac, &tol, &info, wa, &lwa);
	double f1 = pow(x[0] - p1[0], 2) + pow(x[1] - p2[0], 2) + pow(x[2] - p3[0], 2) - pow(C * (x[3] + tr[0] - t[0]), 2);
	double f2 = pow(x[0] - p1[1], 2) + pow(x[1] - p2[1], 2) + pow(x[2] - p3[1], 2) - pow(C * (x[3] + tr[1] - t[1]), 2);
	double f3 = pow(x[0] - p1[2], 2) + pow(x[1] - p2[2], 2) + pow(x[2] - p3[2], 2) - pow(C * (x[3] + tr[2] - t[2]), 2);
	double f4 = pow(x[0] - p1[3], 2) + pow(x[1] - p2[3], 2) + pow(x[2] - p3[3], 2) - pow(C * (x[3] + tr[3] - t[3]), 2);
	fprintf(fp_w1, "info: %d\n", info);
	fprintf(fp_w1, "x1=%.15lf, x2=%.15lf, x3=%.15lf, x4=%.15lf\n", x[0], x[1], x[2], x[3]);
	fprintf(fp_w1, "|f1|=%.15lf, |f2|=%.15lf, |f3|=%.15lf, |f4|=%.15lf\n\n", fabs(f1), fabs(f2), fabs(f3), fabs(f4));
	if (fp_w1 != NULL) fclose(fp_w1);
}
void hybrd()
{
	int n = SOLNUMS;
	double x[SOLNUMS];
	double fvec[SOLNUMS];
	double tol = TOLERANCE;
	int info;
	double wa[(SOLNUMS * (SOLNUMS * 5 + 13)) / 2];
	int lwa = (SOLNUMS * (SOLNUMS * 5 + 13)) / 2;

	FILE* fp_w2 = fopen(writefile2, "w");
	if (fp_w2 == NULL) {
		printf("%s file open error...\n", writefile2);
		return;
	}
	printf("input initial value(%s, hybrd1):", readfile);
	scanf("%lf %lf %lf %lf", &x[0], &x[1], &x[2], &x[3]);//입력받음.
	hybrd1_(fcn3_1_2, &n, x, fvec, &tol, &info, wa, &lwa);
	double f1 = pow(x[0] - p1[0], 2) + pow(x[1] - p2[0], 2) + pow(x[2] - p3[0], 2) - pow(C * (x[3] + tr[0] - t[0]), 2);
	double f2 = pow(x[0] - p1[1], 2) + pow(x[1] - p2[1], 2) + pow(x[2] - p3[1], 2) - pow(C * (x[3] + tr[1] - t[1]), 2);
	double f3 = pow(x[0] - p1[2], 2) + pow(x[1] - p2[2], 2) + pow(x[2] - p3[2], 2) - pow(C * (x[3] + tr[2] - t[2]), 2);
	double f4 = pow(x[0] - p1[3], 2) + pow(x[1] - p2[3], 2) + pow(x[2] - p3[3], 2) - pow(C * (x[3] + tr[3] - t[3]), 2);
	fprintf(fp_w2, "info: %d\n", info);
	fprintf(fp_w2, "x1=%.15lf, x2=%.15lf, x3=%.15lf, x4=%.15lf\n", x[0], x[1], x[2], x[3]);
	fprintf(fp_w2, "|f1|=%.15lf, |f2|=%.15lf, |f3|=%.15lf, |f4|=%.15lf\n\n", fabs(f1), fabs(f2), fabs(f3), fabs(f4));
	if (fp_w2 != NULL) fclose(fp_w2);
}
void program3_1()
{

	int i;
	for (int k = 0; k <= 2; k++) {

		sprintf(readfile, "GPS_signal_%d.txt", k);
		sprintf(writefile1, "GPS_position_3-1_%d.txt", k);//hybrj1
		sprintf(writefile2, "GPS_position_3-2_%d.txt", k);//hybrd1
		FILE* fp_r = fopen(readfile, "r");
		if (fp_r == NULL) {
			printf("%s file open error...\n", readfile);
			return;
		}
		fscanf(fp_r, "%lf %lf", &C, &b);
		for (i = 0; i < 4; i++)
		{
			fscanf(fp_r, "%lf %lf %lf %lf %lf", &p1[i], &p2[i], &p3[i], &t[i], &tr[i]);
		}

		hybrj();
		hybrd();


		if (fp_r != NULL) fclose(fp_r);


	}
}