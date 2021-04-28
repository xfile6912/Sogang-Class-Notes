#include <stdio.h>
#include <string.h>
#include <random>
#include <time.h>

#include <math.h>
#include <time.h>
#include <Windows.h>

__int64 start, freq, end;
#define CHECK_TIME_START QueryPerformanceFrequency((LARGE_INTEGER*)&freq); QueryPerformanceCounter((LARGE_INTEGER*)&start)
#define CHECK_TIME_END(a) QueryPerformanceCounter((LARGE_INTEGER*)&end); a = (float)((float)(end - start) / (freq / 1000.0f))
float compute_time;


#define MATDIM 1024
double *pMatA, *pMatB, *pMatC;
void MultiplySquareMatrices_1(double *pDestMatrix, double *pLeftMatrix, double *pRightMatrix, int MatSize);
void MultiplySquareMatrices_2(double *pDestMatrix, double *pLeftMatrix, double *pRightMatrix, int MatSize);
void MultiplySquareMatrices_3(double *pDestMatrix, double *pLeftMatrix, double *pRightMatrix, int MatSize);
void MultiplySquareMatrices_4(double *pDestMatrix, double *pLeftMatrix, double *pRightMatrix, int MatSize);

void init_MatMat(void);

void main(void)
{
	init_MatMat();

	CHECK_TIME_START;
	MultiplySquareMatrices_1(pMatC, pMatA, pMatB, MATDIM);
	//Sleep(500);
	CHECK_TIME_END(compute_time);
	printf("MultiplySquareMatrices_1 : %f ms\n", compute_time);

	CHECK_TIME_START;
	MultiplySquareMatrices_2(pMatC, pMatA, pMatB, MATDIM);
	//Sleep(500);
	CHECK_TIME_END(compute_time);
	printf("MultiplySquareMatrices_2 = %f ms\n", compute_time);

	CHECK_TIME_START;
	MultiplySquareMatrices_3(pMatC, pMatA, pMatB, MATDIM);
	//Sleep(500);
	CHECK_TIME_END(compute_time);
	printf("MultiplySquareMatrices_3 = %f ms\n", compute_time);

	CHECK_TIME_START;
	MultiplySquareMatrices_4(pMatC, pMatA, pMatB, MATDIM);
	//Sleep(500);
	CHECK_TIME_END(compute_time);
	printf("MultiplySquareMatrices_4 = %f ms\n", compute_time);
}

void MultiplySquareMatrices_1(double *pDestMatrix, double *pLeftMatrix ,double *pRightMatrix, int MatSize)
{
	int i, j, k;
	for (i = 0; i < MatSize; i++)
	{
		for (j = 0; j < MatSize; j++)
		{
			pDestMatrix[i * MatSize + j] = 0;
			for (k = 0; k < MatSize; k++)
			{
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k] * pRightMatrix[k * MatSize + j];
			}
		}
	}
}

void MultiplySquareMatrices_2(double *pDestMatrix, double *pLeftMatrix, double *pRightMatrix, int MatSize)
{
	double *transR = (double*)malloc(sizeof(double) * MatSize * MatSize);//전치행렬
	int i, j, k;
	for (i = 0; i < MatSize; i++)
	{
		for (j = 0; j < MatSize; j++)
		{
			transR[i * MatSize + j] = pRightMatrix[j * MatSize + i];
		}
	}//transR에 pRightMatrix의 전치행렬 만들어줌.

	for (i = 0; i < MatSize; i++)
	{
		for (j = 0; j < MatSize; j++)
		{
			pDestMatrix[i * MatSize + j] = 0;
			for (k = 0; k < MatSize; k++)
			{
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k] * transR[j * MatSize + k];
			}
		}
	}
}

void MultiplySquareMatrices_3(double *pDestMatrix, double *pLeftMatrix, double *pRightMatrix, int MatSize)
{
	double* transR = (double*)malloc(sizeof(double) * MatSize * MatSize);//전치행렬
	int i, j, k;
	for (i = 0; i < MatSize; i++)
	{
		for (j = 0; j < MatSize; j++)
		{
			transR[i * MatSize + j] = pRightMatrix[j * MatSize + i];
		}
	}//transR에 pRightMatrix의 전치행렬 만들어줌.

	for (i = 0; i < MatSize; i++)
	{
		for (j = 0; j < MatSize; j++)
		{
			pDestMatrix[i * MatSize + j] = 0;
			for (k = 0; k < MatSize; k+=64)
			{
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 0] * transR[j * MatSize + k + 0];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 1] * transR[j * MatSize + k + 1];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 2] * transR[j * MatSize + k + 2];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 3] * transR[j * MatSize + k + 3];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 4] * transR[j * MatSize + k + 4];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 5] * transR[j * MatSize + k + 5];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 6] * transR[j * MatSize + k + 6];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 7] * transR[j * MatSize + k + 7];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 8] * transR[j * MatSize + k + 8];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 9] * transR[j * MatSize + k + 9];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 10] * transR[j * MatSize + k + 10];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 11] * transR[j * MatSize + k + 11];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 12] * transR[j * MatSize + k + 12];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 13] * transR[j * MatSize + k + 13];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 14] * transR[j * MatSize + k + 14];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 15] * transR[j * MatSize + k + 15];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 16] * transR[j * MatSize + k + 16];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 17] * transR[j * MatSize + k + 17];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 18] * transR[j * MatSize + k + 18];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 19] * transR[j * MatSize + k + 19];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 20] * transR[j * MatSize + k + 20];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 21] * transR[j * MatSize + k + 21];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 22] * transR[j * MatSize + k + 22];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 23] * transR[j * MatSize + k + 23];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 24] * transR[j * MatSize + k + 24];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 25] * transR[j * MatSize + k + 25];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 26] * transR[j * MatSize + k + 26];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 27] * transR[j * MatSize + k + 27];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 28] * transR[j * MatSize + k + 28];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 29] * transR[j * MatSize + k + 29];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 30] * transR[j * MatSize + k + 30];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 31] * transR[j * MatSize + k + 31];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 32] * transR[j * MatSize + k + 32];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 33] * transR[j * MatSize + k + 33];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 34] * transR[j * MatSize + k + 34];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 35] * transR[j * MatSize + k + 35];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 36] * transR[j * MatSize + k + 36];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 37] * transR[j * MatSize + k + 37];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 38] * transR[j * MatSize + k + 38];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 39] * transR[j * MatSize + k + 39];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 40] * transR[j * MatSize + k + 40];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 41] * transR[j * MatSize + k + 41];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 42] * transR[j * MatSize + k + 42];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 43] * transR[j * MatSize + k + 43];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 44] * transR[j * MatSize + k + 44];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 45] * transR[j * MatSize + k + 45];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 46] * transR[j * MatSize + k + 46];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 47] * transR[j * MatSize + k + 47];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 48] * transR[j * MatSize + k + 48];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 49] * transR[j * MatSize + k + 49];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 50] * transR[j * MatSize + k + 50];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 51] * transR[j * MatSize + k + 51];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 52] * transR[j * MatSize + k + 52];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 53] * transR[j * MatSize + k + 53];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 54] * transR[j * MatSize + k + 54];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 55] * transR[j * MatSize + k + 55];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 56] * transR[j * MatSize + k + 56];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 57] * transR[j * MatSize + k + 57];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 58] * transR[j * MatSize + k + 58];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 59] * transR[j * MatSize + k + 59];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 60] * transR[j * MatSize + k + 60];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 61] * transR[j * MatSize + k + 61];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 62] * transR[j * MatSize + k + 62];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 63] * transR[j * MatSize + k + 63];
			}
		}
	}
}

void MultiplySquareMatrices_4(double *pDestMatrix, double *pLeftMatrix, double *pRightMatrix, int MatSize)
{
	double* transR = (double*)malloc(sizeof(double) * MatSize * MatSize);//전치행렬
	int i, j, k;
	for (i = 0; i < MatSize; i++)
	{
		for (j = 0; j < MatSize; j++)
		{
			transR[i * MatSize + j] = pRightMatrix[j * MatSize + i];
		}
	}//transR에 pRightMatrix의 전치행렬 만들어줌.

	for (i = 0; i < MatSize; i++)
	{
		for (j = 0; j < MatSize; j++)
		{
			pDestMatrix[i * MatSize + j] = 0;
			for (k = 0; k < MatSize; k += 128)
			{
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 0] * transR[j * MatSize + k + 0];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 1] * transR[j * MatSize + k + 1];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 2] * transR[j * MatSize + k + 2];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 3] * transR[j * MatSize + k + 3];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 4] * transR[j * MatSize + k + 4];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 5] * transR[j * MatSize + k + 5];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 6] * transR[j * MatSize + k + 6];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 7] * transR[j * MatSize + k + 7];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 8] * transR[j * MatSize + k + 8];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 9] * transR[j * MatSize + k + 9];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 10] * transR[j * MatSize + k + 10];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 11] * transR[j * MatSize + k + 11];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 12] * transR[j * MatSize + k + 12];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 13] * transR[j * MatSize + k + 13];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 14] * transR[j * MatSize + k + 14];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 15] * transR[j * MatSize + k + 15];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 16] * transR[j * MatSize + k + 16];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 17] * transR[j * MatSize + k + 17];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 18] * transR[j * MatSize + k + 18];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 19] * transR[j * MatSize + k + 19];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 20] * transR[j * MatSize + k + 20];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 21] * transR[j * MatSize + k + 21];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 22] * transR[j * MatSize + k + 22];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 23] * transR[j * MatSize + k + 23];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 24] * transR[j * MatSize + k + 24];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 25] * transR[j * MatSize + k + 25];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 26] * transR[j * MatSize + k + 26];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 27] * transR[j * MatSize + k + 27];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 28] * transR[j * MatSize + k + 28];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 29] * transR[j * MatSize + k + 29];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 30] * transR[j * MatSize + k + 30];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 31] * transR[j * MatSize + k + 31];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 32] * transR[j * MatSize + k + 32];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 33] * transR[j * MatSize + k + 33];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 34] * transR[j * MatSize + k + 34];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 35] * transR[j * MatSize + k + 35];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 36] * transR[j * MatSize + k + 36];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 37] * transR[j * MatSize + k + 37];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 38] * transR[j * MatSize + k + 38];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 39] * transR[j * MatSize + k + 39];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 40] * transR[j * MatSize + k + 40];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 41] * transR[j * MatSize + k + 41];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 42] * transR[j * MatSize + k + 42];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 43] * transR[j * MatSize + k + 43];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 44] * transR[j * MatSize + k + 44];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 45] * transR[j * MatSize + k + 45];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 46] * transR[j * MatSize + k + 46];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 47] * transR[j * MatSize + k + 47];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 48] * transR[j * MatSize + k + 48];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 49] * transR[j * MatSize + k + 49];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 50] * transR[j * MatSize + k + 50];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 51] * transR[j * MatSize + k + 51];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 52] * transR[j * MatSize + k + 52];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 53] * transR[j * MatSize + k + 53];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 54] * transR[j * MatSize + k + 54];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 55] * transR[j * MatSize + k + 55];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 56] * transR[j * MatSize + k + 56];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 57] * transR[j * MatSize + k + 57];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 58] * transR[j * MatSize + k + 58];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 59] * transR[j * MatSize + k + 59];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 60] * transR[j * MatSize + k + 60];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 61] * transR[j * MatSize + k + 61];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 62] * transR[j * MatSize + k + 62];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 63] * transR[j * MatSize + k + 63];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 64] * transR[j * MatSize + k + 64];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 65] * transR[j * MatSize + k + 65];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 66] * transR[j * MatSize + k + 66];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 67] * transR[j * MatSize + k + 67];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 68] * transR[j * MatSize + k + 68];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 69] * transR[j * MatSize + k + 69];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 70] * transR[j * MatSize + k + 70];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 71] * transR[j * MatSize + k + 71];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 72] * transR[j * MatSize + k + 72];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 73] * transR[j * MatSize + k + 73];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 74] * transR[j * MatSize + k + 74];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 75] * transR[j * MatSize + k + 75];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 76] * transR[j * MatSize + k + 76];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 77] * transR[j * MatSize + k + 77];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 78] * transR[j * MatSize + k + 78];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 79] * transR[j * MatSize + k + 79];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 80] * transR[j * MatSize + k + 80];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 81] * transR[j * MatSize + k + 81];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 82] * transR[j * MatSize + k + 82];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 83] * transR[j * MatSize + k + 83];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 84] * transR[j * MatSize + k + 84];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 85] * transR[j * MatSize + k + 85];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 86] * transR[j * MatSize + k + 86];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 87] * transR[j * MatSize + k + 87];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 88] * transR[j * MatSize + k + 88];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 89] * transR[j * MatSize + k + 89];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 90] * transR[j * MatSize + k + 90];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 91] * transR[j * MatSize + k + 91];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 92] * transR[j * MatSize + k + 92];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 93] * transR[j * MatSize + k + 93];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 94] * transR[j * MatSize + k + 94];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 95] * transR[j * MatSize + k + 95];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 96] * transR[j * MatSize + k + 96];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 97] * transR[j * MatSize + k + 97];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 98] * transR[j * MatSize + k + 98];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 99] * transR[j * MatSize + k + 99];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 100] * transR[j * MatSize + k + 100];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 101] * transR[j * MatSize + k + 101];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 102] * transR[j * MatSize + k + 102];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 103] * transR[j * MatSize + k + 103];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 104] * transR[j * MatSize + k + 104];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 105] * transR[j * MatSize + k + 105];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 106] * transR[j * MatSize + k + 106];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 107] * transR[j * MatSize + k + 107];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 108] * transR[j * MatSize + k + 108];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 109] * transR[j * MatSize + k + 109];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 110] * transR[j * MatSize + k + 110];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 111] * transR[j * MatSize + k + 111];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 112] * transR[j * MatSize + k + 112];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 113] * transR[j * MatSize + k + 113];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 114] * transR[j * MatSize + k + 114];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 115] * transR[j * MatSize + k + 115];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 116] * transR[j * MatSize + k + 116];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 117] * transR[j * MatSize + k + 117];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 118] * transR[j * MatSize + k + 118];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 119] * transR[j * MatSize + k + 119];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 120] * transR[j * MatSize + k + 120];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 121] * transR[j * MatSize + k + 121];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 122] * transR[j * MatSize + k + 122];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 123] * transR[j * MatSize + k + 123];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 124] * transR[j * MatSize + k + 124];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 125] * transR[j * MatSize + k + 125];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 126] * transR[j * MatSize + k + 126];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 127] * transR[j * MatSize + k + 127];
			}
		}
	}
}

void init_MatMat(void)
{
	double *ptr;
	pMatA = (double *)malloc(sizeof(double)*MATDIM*MATDIM);
	pMatB = (double *)malloc(sizeof(double)*MATDIM*MATDIM);
	pMatC = (double *)malloc(sizeof(double)*MATDIM*MATDIM);
	srand((unsigned)time(NULL));
	ptr = pMatA;
	for (int i = 0; i < MATDIM*MATDIM; i++)
		*ptr++ = (double)rand() / ((double)RAND_MAX);
	ptr = pMatB;
	for (int i = 0; i < MATDIM*MATDIM; i++)
		*ptr++ = (double)rand() / ((double)RAND_MAX);
}
