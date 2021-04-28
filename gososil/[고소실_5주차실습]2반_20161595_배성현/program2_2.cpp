#include "my_solver.h"
#include <time.h>
#include <math.h>
#include <stdlib.h>


void program2_2()
{
	FILE* fp_r, *fp_w;

	fp_r = fopen("pdf_table.txt", "r");
	fp_w = fopen("random_event_table.txt", "w");

	/***************************************************/
	int numOfSample, i, j;
	double** pdf;//확률 밀도 함수의 그래프 정보 저장
	double** cdf;//누적분포함수의 그래프 정보 저장.
	double** fx;
	double normalh;
	double integralValue;
	fscanf(fp_r, "%d %lf", &numOfSample, &normalh);
	pdf = (double**)malloc(sizeof(double*) * numOfSample);
	cdf = (double**)malloc(sizeof(double*) * numOfSample);
	fx = (double**)malloc(sizeof(double*) * numOfSample);
	for (i = 0; i < numOfSample; i++)
	{
		pdf[i] = (double*)malloc(sizeof(double) * 2);
		cdf[i] = (double*)malloc(sizeof(double) * 2);
		fx[i] = (double*)malloc(sizeof(double) * 2);
		fscanf(fp_r, "%lf %lf", &pdf[i][0], &pdf[i][1]);
		fx[i][0]=cdf[i][0] = pdf[i][0];
	}

	for (j = 0; j < numOfSample; j++)
	{
		integralValue = 0;
		for (i = 0; i<=j; i++)
		{
			integralValue += pdf[i][1];
		}
		for (i = 1; i <= j-1; i++)
		{
			integralValue += pdf[i][1];
		}
		integralValue = 0.5 * integralValue * normalh;
		cdf[j][1] = integralValue;
	}

	int nr;//생성할 난수 개수
	double u;
	srand(unsigned int(time(NULL)));
	scanf("%d", &nr);
	for (i = 0; i < nr; i++)
	{
		u=rand() / (double)32767;

		while(u==0 || u==1)//(0,1)인 ui를 생성하기 위해서
			u = rand() / (double)32767;
		for (j = 0; j < numOfSample; j++)//fx=cdf-u;
		{
			fx[j][1] = cdf[j][1] - u;
		}


		double a, b, x0, x1;
		double fa, fb, fx0, fx1;
		double r1, r2;
		int n = 0;
		a = fx[0][0];
		fa = fx[0][1];
		b = fx[numOfSample - 1][0];
		fb = fx[numOfSample - 1][1];
		x0 = (a + b) / 2;
		for (j = 0; j < numOfSample-1; j++)
		{
			if (x0 >= fx[j][0] && x0 <= fx[j + 1][0])
				break;
		}
		double temp = (pdf[j + 1][1] - pdf[j][1]) / (pdf[j + 1][0] - pdf[j][0]);
		temp = temp * 0.5 * (x0 - pdf[j][0]);
		temp += pdf[j][1];
		temp = temp * (x0 - pdf[j][0]);
		fx0 = cdf[j][1]+temp-u;
		if (fa *fx0 < 0)
		{
			b = x0;
			fb = fx0;
		}
		else if (fb * fx0 < 0)
		{
			a = x0;
			fa = fx0;
		}
		while (1)
		{
			x1 = (a + b) / 2;
			for (j = 0; j < numOfSample - 1; j++)
			{
				if (x1 >= fx[j][0] && x1 <= fx[j + 1][0])
					break;
			}
			double temp = (pdf[j + 1][1] - pdf[j][1]) / (pdf[j + 1][0] - pdf[j][0]);
			temp = temp * 0.5 * (x1 - pdf[j][0]);
			temp += pdf[j][1];
			temp = temp * (x1 - pdf[j][0]);
			fx1 = cdf[j][1] + temp-u;
			if (fa * fx1 < 0)
			{
				b = x1;
				fb = fx1;

			}
			else if (fb * fx1< 0)
			{
				a = x1;
				fa = fx1;
			}
			n++;
			if (fabs(fx1) < DELTA)
				break;
			if (n == Nmax)
				break;
			if (fabs(x1 - x0) < EPSILON)
				break;
			x0 = x1;

		}
		printf("%.15lf\n", x1);
		fprintf(fp_w, "%.15lf\n", x1);
	}



	/***************************************************/

	if (fp_r != NULL) fclose(fp_r);
	if (fp_w != NULL) fclose(fp_w);
}
