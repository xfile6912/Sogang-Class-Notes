#define _USE_MATH_DEFINES
#include <math.h>

float _sp_f4(float x) {
	return logf(x) - 1;
}

float _sp_fp4(float x) {
	return 1 / x;
}