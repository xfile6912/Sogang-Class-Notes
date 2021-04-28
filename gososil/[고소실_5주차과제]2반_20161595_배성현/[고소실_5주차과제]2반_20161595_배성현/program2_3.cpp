#include "my_solver.h"
#include <time.h>
#include <math.h>

double func(double lamda, double x, double u)
{
	if (x < 0)
		return -u;
	return 1 - exp((0 - lamda) * x)-u;
}

void program2_4()//과제 2-1
{
	double lambda[3] = { 2.0, 3.0, 5.0 };
	int nr, i, j;
	double lamb;
	double u;
	double* xi;
	double expectation=0;
	double dispersion=0;
	srand(unsigned int(time(NULL)));
	printf("난수개수 입력(과제2-1): ");
	scanf("%d", &nr);
	xi = (double*)malloc(sizeof(double) * nr);
	for (j = 0; j < 3; j++)
	{
		lamb = lambda[j];
		expectation = 0;
		dispersion = 0;
		for (i = 0; i < nr; i++)
		{
			u = rand() / (double)32767;

			while (u == 0 || u == 1)//(0,1)인 ui를 생성하기 위해서
				u = rand() / (double)32767;
			double a, b, x0, x1;
			int n = 0;
			a = 0; b = 100;
			x0 = (a + b) / 2;
			if (func(lamb, a, u) * func(lamb,x0, u) < 0)
			{
				b = x0;
			}
			else if (func(lamb, b, u) * func(lamb, x0, u) < 0)
			{
				a = x0;
			}
			while (1)
			{
				x1 = (a + b) / 2;
				if (func(lamb, a, u) * func(lamb, x1, u) < 0)
				{
					b = x1;
				}
				else if (func(lamb, b, u) * func(lamb, x1, u) < 0)
				{
					a = x1;
				}
				n++;
				if (fabs(func(lamb, x1, u)) < DELTA)
					break;
				if (n == Nmax)
					break;
				if (fabs(x1 - x0) < EPSILON)
					break;
				x0 = x1;
			}
			xi[i] = x1;
			expectation += x1;
			dispersion += x1 * x1;
		}
		//////Expectation
		expectation /= nr;
		//////Dispersion
		dispersion /= nr;
		dispersion = dispersion - expectation * expectation;//분산=e[x^2]-(e[x])^2
		printf("lambda: %lf, 기대값: %lf, 분산: %lf\n",lamb, expectation, dispersion);
		printf("이론과의 오차[기대값: %lf, 분산: %lf]\n\n", fabs(1 / lamb-expectation), fabs(1 / lamb / lamb-dispersion));
	}
}
// HOMEWORK
void program2_3()
{
	FILE* pdfFile, *randomFile, *histFile;
	pdfFile = fopen("pdf_table.txt", "r");
	randomFile= fopen("random_event_table.txt", "r");
	histFile = fopen("histogram.txt","w");
	int numOfSample, i, j;
	int numOfRandom;
	double** pdf;//확률 밀도 함수의 그래프 정보 저장
	double** histogram;
	double* random;
	double normalh;
	double integralValue;
	double pdfexpectation = 0;
	double pdfdispersion = 0;
	double histexpectation = 0;
	double histdispersion = 0;
	fscanf(pdfFile, "%d %lf", &numOfSample, &normalh);
	fscanf(randomFile, "%d", &numOfRandom);
	pdf = (double**)malloc(sizeof(double*) * numOfSample);
	histogram = (double**)malloc(sizeof(double*) * numOfSample);
	random= (double*)malloc(sizeof(double) * numOfRandom);
	for (i = 0; i < numOfSample; i++)
	{
		pdf[i] = (double*)malloc(sizeof(double) * 2);
		histogram[i] = (double*)malloc(sizeof(double) * 2);
		fscanf(pdfFile, "%lf %lf", &pdf[i][0], &pdf[i][1]);
		histogram[i][0] = pdf[i][0];
		histogram[i][1] = 0;
		pdfexpectation += pdf[i][1]*pdf[i][0];
	}
	for (i = 0; i < numOfRandom; i++)
	{
		fscanf(randomFile, "%lf", &random[i]);
	}
	for (i = 0; i < numOfRandom; i++)
	{
		for (j = 0; j < numOfSample-1; j++)
		{
			if (pdf[j][0] <= random[i] && random[i] < pdf[j + 1][0])
				histogram[j][1]+=1;
		}
		histexpectation += random[i];
		histdispersion += random[i]*random[i];
	}
	pdfexpectation /= numOfSample;
	for (i = 0; i < numOfSample; i++)
	{
		pdfdispersion += (pdf[i][0] - pdfexpectation) * (pdf[i][0] - pdfexpectation) * pdf[i][1];
	}
	pdfdispersion /= numOfSample;//e[x^2];

	histexpectation /= numOfRandom;
	histdispersion /= numOfRandom;//e[x^2];
	histdispersion = histdispersion - histexpectation * histexpectation;
	fprintf(histFile, "확률밀도 함수의 분산: %lf\n",pdfdispersion);
	fprintf(histFile, "난수 생성으로 만들어진 xi데이터의 분산: %lf\n", histdispersion);
	fprintf(histFile, "난수 생성으로 만들어진 xi데이터: \n");
	for (int i = 0; i < numOfSample; i++)
		fprintf(histFile, "%lf %d\n", histogram[i][0], (int)histogram[i][1]);
}

// HOMEWORK
void program2_2_a()
{
	__int64 start, freq, end;
	float resultTime = 0;

	FILE* fp_r, * fp_w;

	fp_r = fopen("pdf_table.txt", "r");

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
		fx[i][0] = cdf[i][0] = pdf[i][0];
	}
	cdf[0][1] = 0;
	for (j = 1; j < numOfSample; j++)
	{
		integralValue = 0;
		for (i = 0; i <= j; i++)
		{
			integralValue += pdf[i][1];
		}
		for (i = 1; i <= j - 1; i++)
		{
			integralValue += pdf[i][1];
		}
		integralValue = 0.5 * integralValue * normalh;
		cdf[j][1] = integralValue;
	}

	int nr;//생성할 난수 개수
	double u;
	srand(unsigned int(time(NULL)));
	printf("난수개수 입력(과제2-2, bisection): ");
	scanf("%d", &nr);
	CHECK_TIME_START;
	for (i = 0; i < nr; i++)
	{
		u = rand() / (double)32767;

		while (u == 0 || u == 1)//(0,1)인 ui를 생성하기 위해서
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
		for (j = 0; j < numOfSample - 1; j++)
		{
			if (x0 >= fx[j][0] && x0 <= fx[j + 1][0])
				break;
		}
		double temp = (pdf[j + 1][1] - pdf[j][1]) / (pdf[j + 1][0] - pdf[j][0]);
		temp = temp * 0.5 * (x0 - pdf[j][0]);
		temp += pdf[j][1];
		temp = temp * (x0 - pdf[j][0]);
		fx0 = cdf[j][1] + temp - u;
		if (fa * fx0 < 0)
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
			fx1 = cdf[j][1] + temp - u;
			if (fa * fx1 < 0)
			{
				b = x1;
				fb = fx1;

			}
			else if (fb * fx1 < 0)
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

	}
	CHECK_TIME_END(resultTime);
	printf("The program2_2_a run time is %f(ms)..\n", resultTime * 1000.0);
	if (fp_r != NULL) fclose(fp_r);

}

void program2_2_b()
{
	__int64 start, freq, end;
	float resultTime = 0;

	FILE* fp_r, * fp_w;

	fp_r = fopen("pdf_table.txt", "r");

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
		fx[i][0] = cdf[i][0] = pdf[i][0];
	}
	cdf[0][1] = 0;
	for (j = 1; j < numOfSample; j++)
	{
		integralValue = 0;
		for (i = 0; i <= j; i++)
		{
			integralValue += pdf[i][1];
		}
		for (i = 1; i <= j - 1; i++)
		{
			integralValue += pdf[i][1];
		}
		integralValue = 0.5 * integralValue * normalh;
		cdf[j][1] = integralValue;
	}

	int nr;//생성할 난수 개수
	double u;
	srand(unsigned int(time(NULL)));
	printf("난수개수 입력(과제2-2, secant): ");
	scanf("%d", &nr);
	CHECK_TIME_START;
	for (i = 0; i < nr; i++)
	{
		u = rand() / (double)32767;

		while (u == 0 || u == 1)//(0,1)인 ui를 생성하기 위해서
			u = rand() / (double)32767;
		for (j = 0; j < numOfSample; j++)//fx=cdf-u;
		{
			fx[j][1] = cdf[j][1] - u;
		}


		double  x0, x1, x2;
		double fx0, fx1, fx2;
		double r1, r2;
		int n = 0;
		x0 = fx[0][0];
		fx0= fx[0][1];
		x1 = fx[numOfSample - 1][0];
		fx1 = fx[numOfSample - 1][1];
		while (1)
		{
			x2 = x1 - fx1 * ((x1 - x0) / (fx1 - fx0));
			n++;
			if (x2 < 0)
				x2 = 0;
			for (j = 0; j < numOfSample - 1; j++)
			{
				if (x2 >= fx[j][0] && x2 <= fx[j + 1][0])
					break;
			}
			if (j == numOfSample - 1)
			{
				j = j - 1;
				x2 = 1;
			}
			double temp = (pdf[j + 1][1] - pdf[j][1]) / (pdf[j + 1][0] - pdf[j][0]);
			temp = temp * 0.5 * (x2 - pdf[j][0]);
			temp += pdf[j][1];
			temp = temp * (x2 - pdf[j][0]);
			fx2 = cdf[j][1] + temp - u;
			if (fabs(fx2) < DELTA)
				break;
			if (n == Nmax)
				break;
			if (fabs(x2 - x1) < EPSILON)
				break;
			x0 = x1;
			fx0 = fx1;
			x1 = x2;
			fx1 = fx2;
		}
	}
	CHECK_TIME_END(resultTime);
	printf("The program2_2_b run time is %f(ms)..\n", resultTime * 1000.0);

	/////////////////////////////////////newton-raphson
	printf("난수개수 입력(과제2-2, newton-raphson): ");
	scanf("%d", &nr);
	CHECK_TIME_START;
	for (i = 0; i < nr; i++)
	{
		u = rand() / (double)32767;

		while (u == 0 || u == 1)//(0,1)인 ui를 생성하기 위해서
			u = rand() / (double)32767; 
		for (j = 0; j < numOfSample; j++)//fx=cdf-u;
		{
			fx[j][1] = cdf[j][1] - u;
		}


		double fpx0, s;
		double a, b, x0, x1;
		double fa, fb, fx0, fx1;
		double r1, r2;
		int n = 0;
		
		x0 = fx[numOfSample/2][0];
		fx0= fx[numOfSample / 2][1];
		n = 0;
		if (x0 < 0)
			x0 = 0;
		for (j = 0; j < numOfSample - 1; j++)
		{
			if (x0 >= fx[j][0] && x0 <= fx[j + 1][0])
				break;
		}
		if (j == numOfSample - 1)
		{
			j = j - 1;
			x0 = 1;
		}
		s = (x0 - fx[j][0]) / (fx[j + 1][0] - fx[j][0]);
		while (1)
		{


			fpx0 = (1 - s) * pdf[j][1] + s * pdf[j + 1][1];
			////////////////////////////////f'(x0)구해줌.
			x1 = x0 - (fx0/ fpx0);///x1구해줌.
			n++;
			if (x1 < 0)
				x1 = 0;
			for (j = 0; j < numOfSample - 1; j++)
			{
				if (x1 >= fx[j][0] && x1 <= fx[j + 1][0])
					break;
			}
			if (j == numOfSample - 1)
			{
				j = j - 1;
				x1 = 1;
			}
			s = (x1 - fx[j][0]) / (fx[j + 1][0] - fx[j][0]);
			double temp = (pdf[j + 1][1] - pdf[j][1]) / (pdf[j + 1][0] - pdf[j][0]);
			temp = temp * 0.5 * (x1 - pdf[j][0]);
			temp += pdf[j][1];
			temp = temp * (x1 - pdf[j][0]);
			fx1 = cdf[j][1] + temp - u;
			if (fabs(fx1) < DELTA)
				break;
			if (n == Nmax)
				break;
			if (fabs(x1 - x0) < EPSILON)
				break;
			x0 = x1;
			fx0 = fx1;
		}
	}
	CHECK_TIME_END(resultTime);
	printf("The program2_2_b run time is %f(ms)..\n", resultTime * 1000.0);


	///////
		/////////////////////////////////////newton-raphson
	printf("난수개수 입력(과제2-2, newton-raphson with bisection): ");
	scanf("%d", &nr);
	CHECK_TIME_START;
	for (i = 0; i < nr; i++)
	{
		u = rand() / (double)32767;

		while (u == 0 || u == 1)//(0,1)인 ui를 생성하기 위해서
			u = rand() / (double)32767;
		for (j = 0; j < numOfSample; j++)//fx=cdf-u;
		{
			fx[j][1] = cdf[j][1] - u;
		}


		double fpx0, s;
		double a, b, x0, x1;
		double fa, fb, fx0, fx1;
		double r1, r2;
		int n = 0;
		a = fx[0][0];
		fa = fx[0][1];
		b = fx[numOfSample - 1][0];
		fb = fx[numOfSample - 1][1];
		x0 = (a + b) / 2;
		for (j = 0; j < numOfSample - 1; j++)
		{
			if (x0 >= fx[j][0] && x0 <= fx[j + 1][0])
				break;
		}
		double temp = (pdf[j + 1][1] - pdf[j][1]) / (pdf[j + 1][0] - pdf[j][0]);
		temp = temp * 0.5 * (x0 - pdf[j][0]);
		temp += pdf[j][1];
		temp = temp * (x0 - pdf[j][0]);
		fx0 = cdf[j][1] + temp - u;
		if (fa * fx0 < 0)
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
			fx1 = cdf[j][1] + temp - u;
			if (fa * fx1 < 0)
			{
				b = x1;
				fb = fx1;

			}
			else if (fb * fx1 < 0)
			{
				a = x1;
				fa = fx1;
			}
			n++;
			if (fabs(fx1) < DELTA)
				break;
			if (n ==3)
				break;
			if (fabs(x1 - x0) < EPSILON)
				break;
			x0 = x1;

		}

		x0 = x1;
		fx0 = fx1;
		n = 0;
		if (x0 < 0)
			x0 = 0;
		for (j = 0; j < numOfSample - 1; j++)
		{
			if (x0 >= fx[j][0] && x0 <= fx[j + 1][0])
				break;
		}
		if (j == numOfSample - 1)
		{
			j = j - 1;
			x0 = 1;
		}
		s = (x0 - fx[j][0]) / (fx[j + 1][0] - fx[j][0]);
		while (1)
		{


			fpx0 = (1 - s) * pdf[j][1] + s * pdf[j + 1][1];
			////////////////////////////////f'(x0)구해줌.
			x1 = x0 - (fx0 / fpx0);///x1구해줌.
			n++;
			if (x1 < 0)
				x1 = 0;
			for (j = 0; j < numOfSample - 1; j++)
			{
				if (x1 >= fx[j][0] && x1 <= fx[j + 1][0])
					break;
			}
			if (j == numOfSample - 1)
			{
				j = j - 1;
				x1 = 1;
			}
			s = (x1 - fx[j][0]) / (fx[j + 1][0] - fx[j][0]);
			double temp = (pdf[j + 1][1] - pdf[j][1]) / (pdf[j + 1][0] - pdf[j][0]);
			temp = temp * 0.5 * (x1 - pdf[j][0]);
			temp += pdf[j][1];
			temp = temp * (x1 - pdf[j][0]);
			fx1 = cdf[j][1] + temp - u;
			if (fabs(fx1) < DELTA)
				break;
			if (n == Nmax)
				break;
			if (fabs(x1 - x0) < EPSILON)
				break;
			x0 = x1;
			fx0 = fx1;
		}
	}
	CHECK_TIME_END(resultTime);
	printf("The program2_2_b run time is %f(ms)..\n", resultTime * 1000.0);

		if (fp_r != NULL) fclose(fp_r);
}