#include <stdio.h>
#include <syscall.h>
#include <stdlib.h>
#include "../lib/user/syscall.h"


int main (int argc, char **argv)
{
	int a, b, c, d;
//	printf("1");
	a=atoi(argv[1]);
	b=atoi(argv[2]);
	c=atoi(argv[3]);
	d=atoi(argv[4]);
	//printf("%d %d %d %d\n", a, b, c,d);
	printf("%d %d\n", fibonacci(a), max_of_four_int(a, b, c, d));

	return EXIT_SUCCESS;
}

