#define _USE_MATH_DEFINES
#include <math.h>

double _f1(double x) {
	return x * x - 4 * x + 4 - log(x);
}

double _fp1(double x) {
	return 2*x-4-1/x;
}
double _f2(double x) {
	return x + 1 - 2 * sin(M_PI * x);
}

double _fp2(double x) {
	return 1 - 2 * M_PI * cos(M_PI*x);
}
double _f3(double x) {
	return x * x * x * x - 11.0 * x * x * x + 42.35 * x * x - 66.55 * x + 35.1384;
}

double _fp3(double x) {
	return 4 * x * x * x - 33.0 * x * x + 84.70 * x - 66.55;
}
double _f4(double x) {
	return log(x)-1;
}

double _fp4(double x) {
	return 1/x;
}
double _f5(double x) {
	double l = 89;
	double h = 49;
	double d = 55;
	double b1 = 11.5*M_PI/180;
	double a = l * sin(b1);
	double b=l*cos(b1);
	double c = (h + 0.5 * d)*sin(b1) - 0.5 * d * tan(b1);
	double e = (h + 0.5 * d) * cos(b1) - 0.5 * d;
	return a*sin(x)*cos(x)+b*sin(x)*sin(x)-c*cos(x)-e*sin(x);
}

double _fp5(double x) {
	double l = 89;
	double h = 49;
	double d = 55;
	double b1 = 11.5 * M_PI / 180;
	double a = l * sin(b1);
	double b = l * cos(b1);
	double c = (h + 0.5 * d) * sin(b1) - 0.5 * d * tan(b1);
	double e = (h + 0.5 * d) * cos(b1) - 0.5 * d;
	return a * cos(2*x) + 2 * b * sin(x) * cos(x) + c * sin(x) - e * cos(x);;
}