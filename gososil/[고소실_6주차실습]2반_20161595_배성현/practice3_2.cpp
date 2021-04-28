
#include "my_solver.h"

double getPowR(double zeror, double zeroi, int n)//실수부 반환
{
	int j;
	double r=zeror;
	double i=zeroi;
	for (j = 1; j < n; j++)
	{
		r = r * zeror - i * zeroi;
		i = r * zeroi + i * zeror;
	}
	return r;
}
double getPowI(double zeror, double zeroi, int n)//허수부 반환
{
	int j;
	double r = zeror;
	double i = zeroi;
	for (j = 1; j < n; j++)
	{
		r = r * zeror - i * zeroi;
		i = r * zeroi + i * zeror;
	}
	return i;
}

void practice3_2() {
	char readfile[256];
	char writefile[256];

	int NCOEF, DEGREE;

	double* poly;
	double* zeror, * zeroi;
	long int fail;
	int i;

	for (int t = 1; t <= 6; t++) {

		sprintf(readfile, "polynomial_3-2_%d.txt", t);
		sprintf(writefile, "roots_3-2_%d.txt", t);

		FILE* fp_r = fopen(readfile, "r");
		if (fp_r == NULL) {
			printf("%s file open error...\n", readfile);
			return;
		}

		FILE* fp_w = fopen(writefile, "w");
		if (fp_w == NULL) {
			printf("%s file open error...\n", writefile);
			return;
		}

		fscanf(fp_r, "%d", &DEGREE);
		NCOEF = DEGREE + 1;

		poly = (double*)malloc(sizeof(double) * NCOEF);
		zeror = (double*)malloc(sizeof(double) * DEGREE);
		zeroi = (double*)malloc(sizeof(double) * DEGREE);


		/********************************/

		for (i = 0; i < NCOEF; i++)
		{
			fscanf(fp_r, "%lf", &poly[i]);
		}
		rpoly_(poly, &DEGREE, zeror, zeroi, &fail);
		if (fail) {
			fprintf(fp_w, "wrong\n");
		}
		else
		{
			for (i = 0; i < DEGREE; i++)
				fprintf(fp_w, "%10f ", zeror[i]);
			fprintf(fp_w,"\n");
			for (i = 0; i < DEGREE; i++)
				fprintf(fp_w, "%10f ", zeroi[i]);
			fprintf(fp_w, "\n");

			for (i = 0; i < DEGREE; i++)
			{
				double real= poly[NCOEF - 1];//차수가 0일때
				double imagine=0;
				for (int j = 1; j <= DEGREE; j++)
				{
					real+= poly[DEGREE - j] * getPowR(zeror[i], zeroi[i], j);
					imagine+= poly[DEGREE - j] * getPowI(zeror[i], zeroi[i], j);
				}
				if (zeroi[i] == 0)
					fprintf(fp_w, "f(x[%d])=%.15lf\n", i, real);
				else
					fprintf(fp_w, "f(x[%d])=%.15lf+(%.15lfi)\n", i,real, imagine);
			}
		}


		/********************************/


		free(zeroi);
		free(zeror);
		free(poly);

		if (fp_r != NULL) fclose(fp_r);
		if (fp_w != NULL) fclose(fp_w);
	}
}