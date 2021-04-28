#include "my_solver.h"

void program2_1()
{
	FILE* fp_r, *fp_w;
	__int64 start, freq, end;
	float resultTime = 0;

	fp_r = fopen("sampling_table.txt", "r");
	if (fp_r == NULL) {
		printf("input file not found....\n");
		exit(0);
	}

	fp_w = fopen("pdf_table.txt", "w");

	/***************************************************/
	int numOfSample, i, j;
	double** func;//함수의 그래프 정보 저장
	double prevh;
	double normalh;
	fscanf(fp_r, "%d %lf", &numOfSample, &prevh);
	func = (double**)malloc(sizeof(double*) * numOfSample);
	for (i = 0; i < numOfSample; i++)
	{
		func[i] = (double*)malloc(sizeof(double) * 2);
		fscanf(fp_r, "%lf %lf", &func[i][0], &func[i][1]);
	}
	
	double x0, xn;//x0는 시작점 xn은 끝점.
	double diff;
	int starti, lasti;
	x0 = func[0][0];
	xn = func[numOfSample - 1][0];//시작과 끝점 설정해줘서 나중에 정규화 할 때 사용.
	diff = xn - x0;
	for (i = 0; i < numOfSample; i++)//x값들 정규화 해주는 for문 
	{
		func[i][0] = (func[i][0] - x0) / diff;//정규화 해줌.
	}
	normalh = prevh / diff;//정규화 되었을 때의 h구함.

	double integralValue;//적분값 저장하는 변수.
	integralValue = (func[0][1] + func[numOfSample - 1][1]);//가장 앞과 가장 뒤 더해줌.
	for (i = 1; i < numOfSample - 1; i++)
	{
		integralValue += 2 * (func[i][1]);
	}
	integralValue = 0.5 * integralValue * normalh;//적분값 구함.
	/***************************************************/

	for (i = 0; i < numOfSample; i++)
	{
		func[i][1] /= integralValue;//y를 적분값으로 나눠서 확률 밀도함수로 만들어줌.
	}

	///////////////////////////////////////////////////////////////출력 부분.
	integralValue = (func[0][1] + func[numOfSample - 1][1]);//가장 앞과 가장 뒤 더해줌.
	for (i = 1; i < numOfSample - 1; i++)
	{
		integralValue += 2 * (func[i][1]);
	}
	integralValue = 0.5 * integralValue * normalh;
	printf("*** Integrating the pdf from 0.0 to 1.0 = %lf\n", integralValue);
	///////////////////////////////////////////////////////////////출력 부분.
	lasti = 0;
	integralValue = 0;
	for (i = lasti; func[i][0]<=0.25; i++)
	{
		integralValue += func[i][1];
	}
	starti = lasti + 1;
	lasti = i-1;
	for (i = starti; i < lasti; i++)
	{
		integralValue += func[i][1];
	}
	integralValue = 0.5 * integralValue * normalh;
	printf("*** Integrating the pdf from 0.0 to 0.25 = %lf\n", integralValue);
	///////////////////////////////////////////////////////////////출력 부분.
	integralValue = 0;
	for (i = lasti; func[i][0] <= 0.5; i++)
	{
		integralValue += func[i][1];
	}
	starti = lasti + 1;
	lasti = i - 1;
	for (i = starti+1; i < lasti; i++)
	{
		integralValue += func[i][1];
	}
	integralValue = 0.5 * integralValue * normalh;
	printf("*** Integrating the pdf from 0.25 to 0.5 = %lf\n", integralValue);
	///////////////////////////////////////////////////////////////출력 부분.
	integralValue = 0;
	for (i = lasti; func[i][0] <= 0.75; i++)
	{
		integralValue += func[i][1];
	}
	starti = lasti + 1;
	lasti = i - 1;
	for (i = starti; i < lasti; i++)
	{
		integralValue += func[i][1];
	}
	integralValue = 0.5 * integralValue * normalh;
	printf("*** Integrating the pdf from 0.5 to 0.75 = %lf\n", integralValue);
	///////////////////////////////////////////////////////////////출력 부분.
	integralValue = 0;
	for (i = lasti; i<numOfSample; i++)
	{
		integralValue += func[i][1];
	}
	starti = lasti + 1;
	lasti = i - 1;
	for (i = starti; i < lasti; i++)
	{
		integralValue += func[i][1];
	}
	integralValue = 0.5 * integralValue * normalh;
	printf("*** Integrating the pdf from 0.75 to 1.0 = %lf\n", integralValue);

	fprintf(fp_w, "%d %lf\n", numOfSample, normalh);
	for (i = 0; i < numOfSample; i++)
	{
		fprintf(fp_w, "%lf %lf\n", func[i][0], func[i][1]);
	}

	if (fp_r != NULL) fclose(fp_r);
	if (fp_w != NULL) fclose(fp_w);
}
