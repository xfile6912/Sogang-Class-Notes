
#include "my_solver.h"
#define index_2d(row, col, dim) row*dim+col

void practice3_1() {
	int i, ia, j, n, l[4];
	float a[16] = {2,4,0,1,0,1,5,1,0,1,1,1,0,1,2,3};
	float b[4] = { 2,7,8,6 };
	float x[4], s[4];
	n = ia = 4;
	/********************************/
	printf("***** Equation *****\n");
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) printf("a[%d][%d] = %f, ", i, j, a[index_2d(j, i, 4)]);
		printf("\n");
	}
	printf("b[0] = %f, b[1] = %f, b[2] = %f, b[3]=%f\n", b[0], b[1], b[2],b[3]);
	gespp_(&n, a, &ia, l, s);
	printf("***** After LU-decomposition *****\n");
	printf("s[0] = %f, s[1] = %f, s[2] = %f, s[3] = %f\n", s[0], s[1], s[2], s[3]);
	printf("l[0] = %d, l[1] = %d, l[2] = %d, l[3] = %d\n", l[0], l[1], l[2],l[3]);
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) printf("a[%d][%d] = %f, ", i, j, a[index_2d(j, i, 4)]);
		printf("\n");
	}
	solve_(&n, a, &ia, l, b, x);
	printf("***** Solution *****\n");
	printf("x[0] = %f, x[1] = %f, x[2] = %f, x[3]=%f\n", x[0], x[1], x[2], x[3]);

    /********************************/

}
