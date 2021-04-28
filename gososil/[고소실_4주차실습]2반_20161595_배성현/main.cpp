#include "my_solver.h"

extern void program1_1(FILE* fp); // Newton-Raphson Method
extern void program1_2(FILE* fp); // Secant Method

extern void sp_program1_1(FILE* fp); // Newton-Raphson Method
extern void sp_program1_2(FILE* fp); // Secant Method

double (*_f)(double);
double (*_fp)(double);

float (*_sp_f)(float);
float (*_sp_fp)(float);

int main() {
	FILE* fp;
	fp = fopen("result.txt", "w");

	_f = _f1;
	_fp = _fp1;
	printf("1-1 (1)\n");
	printf("newton-raphson: ");
	program1_1(fp);
	printf("secant: ");
	program1_2(fp);

	_f = _f2;
	_fp = _fp2;
	printf("1-1 (2)\n");
	printf("newton-raphson: ");
	program1_1(fp);
	printf("secant: ");
	program1_2(fp);

	_f = _f3;
	_fp = _fp3;
	printf("\n");
	printf("1-2\n");
	printf("근 1\n");
	printf("newton-raphson: ");
	program1_1(fp);
	printf("secant: ");
	program1_2(fp);
	printf("근 2\n");
	printf("newton-raphson: ");
	program1_1(fp);
	printf("secant: ");
	program1_2(fp);
	printf("근 3\n");
	printf("newton-raphson: ");
	program1_1(fp);
	printf("secant: ");
	program1_2(fp);
	printf("근 4\n");
	printf("newton-raphson: ");
	program1_1(fp);
	printf("secant: ");
	program1_2(fp);
	printf("\n");
	printf("1-4\n");
	_f = _f4;
	_fp = _fp4;
	printf("newton-raphson(double):");
	program1_1(fp);
	_sp_f = _sp_f4;
	_sp_fp = _sp_fp4;
	printf("newton-raphson(single):");
	sp_program1_1(fp);
	fclose(fp);
}
