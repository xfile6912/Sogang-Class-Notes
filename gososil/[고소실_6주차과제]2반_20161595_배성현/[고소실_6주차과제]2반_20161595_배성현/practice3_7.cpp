
#include "my_solver.h"

#define SOLNUMS 2
#define MATCOLS SOLNUMS
#define MATROWS SOLNUMS
#define TOLERANCE 0.0000001

void fcn3_7(int* n, double* x, double* fvec, int* iflag)
{
	/********************************/

	fvec[0] = 2 * pow(x[0], 3) - 12 * x[0] - x[1] - 1;
	fvec[1] = 3 * x[1] * x[1] - 6 * x[1] - x[0] - 3;
	/********************************/
}
typedef struct root* pointer;
typedef struct root {
	double x;
	double y;
	pointer next;
};
void practice3_7(void)
{
	int n = SOLNUMS;
	double x[SOLNUMS] = { -4.0, 5.0 };	//need to initilize x0
	double fvec[SOLNUMS];
	double tol = TOLERANCE;
	int info;
	double wa[(SOLNUMS * (3 * SOLNUMS + 13)) / 2];
	int lwa = (SOLNUMS * (3 * SOLNUMS + 13)) / 2;

	FILE* fp_w = fopen("roots_3-7.txt", "w");
	if (fp_w == NULL) {
		printf("%s file open error...\n", "roots_3-7.txt");
		return;
	}

	/********************************/
	root* front = NULL;
	root* end = NULL;
	for (float i = -5.0; i <= 5.0; i += 0.5)
	{
		for (float j = -6.0; j <= 6.0; j += 0.5)
		{
			x[0] = i;
			x[1] = j;
			hybrd1_(fcn3_7, &n, x, fvec, &tol, &info, wa, &lwa);
			if (x[0] >= -4 && x[0] <= 4 && x[1] >= -5 && x[1] <= 5 && info==1)//근에 해당한다면
			{
				if (front == NULL)
				{
					root* temp = (root*)malloc(sizeof(root));
					temp->x = x[0];
					temp->y = x[1];
					temp->next = NULL;
					front = temp;
					end = temp;
				}
				else
				{
					root* ptr;
					int flag = 0;
					for (ptr = front; ptr != NULL; ptr = ptr->next)
					{
						if (fabs(x[0] - ptr->x) < 0.00001 && fabs(x[1]-ptr->y)<0.00001)//존재한다면
							flag = 1;
					}
					if (flag == 0)//없으면 추가해줌.
					{
						root* temp = (root*)malloc(sizeof(root));
						temp->x = x[0];
						temp->y = x[1];
						temp->next = NULL;
						end->next = temp;
						end = temp;
					}
				}
			}
		}
	}
	for (root* ptr = front; ptr != NULL; )
	{
		double x, y;
		x = ptr->x;
		y = ptr->y;
		double f1 = 2 * pow(x, 3) - 12 * x - y - 1;
		double f2 = 3 * y * y - 6 * y - x - 3;
		fprintf(fp_w, "x=%.15lf, y=%.15lf\n", x, y);
		fprintf(fp_w, "|f1|=%.15lf, |f2|=%.15lf\n\n", fabs(f1), fabs(f2));
		root* temp = ptr;
		ptr = ptr->next;
		free(temp);

	}
	
	/********************************/

	fclose(fp_w);
}