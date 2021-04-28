#include <stdio.h>

typedef struct node{
    int nodea;
    char nodeb;
    double nodec;
    char *noded;
}NODE;
typedef struct node1{
    int a;
    float b;
    double c;
}NODE1;
int globali=1;
float globalf=3.0;
int globalarr[3]={1,2,3};
char *globalp;
float *globalfp;
union UNI{
    int a;
    char b;
    int c[10];
};
void fun1(NODE nod)
{
}
void fun2(NODE1 nod1)
{
}
void fun3(char a, int b, int c, short d, int e, int f, int g)
{
}
float fun4(float a, float b, float c, double d, double e, double f, float g)
{
    return b;
}

float* fun5(double f, short g, float arrf[], union UNI unia)
{
    NODE a;
    char b[10];
    union UNI uni[2][3];
    f=1.0;
    g=2;
    arrf[0]=2.9;
    uni[0][0]=unia;
    a.nodea=3;
    b[0]=4;
    return arrf;
}
int fun6(char a, int b, float c, char* d, NODE e, short g, int h, int i, int j)
{
    char ch;
    float arrf[25];
    union UNI unia;
    a=1;
    b=2;
    c=3.0;
    d=&a;
    e.nodea=4;
    g=7;
    h=8;
    i=9;
    j=10;
    ch=11;
    arrf[0]=4.0;
    unia.a=12;

    b=globali;//정수형 전역변수 참조
    c=globalf;//실수형 전역변수 참조
    d=globalp;//포인터형 전역변수 참조
    b=globalarr[2];//배열 전역변수  참조
    unia.c[0]=i;//정수형 parameter참조
    arrf[0]=c;//실수형 parameter 참조
    globalp=d;//pointer형 parameter 참조
    globalfp=fun5(3.3, 1, arrf, unia);
    return b+e.nodea+5;
}

void main(void)
{
    char a;
    int b;
    float c;
    char *d;
    NODE e;
    NODE1 e2;
    double f;
    short g;
    char arr[3];
    a=1;
    b=2;
    c=3.0;
    d=&a;
    e.nodea=4;
    e.nodeb=5;
    e.nodec=6.0;
    e2.a=6;
    f=8.0;
    g=7;
    arr[0]=9;
    globali=b;//int형 지역변수 참조
    globalf=c;//float형 지역변수 참조
    a=arr[2];//배열 지역변수 참조
    arr[1]=a;//char형 지역변수 참조
    globalp=d;//포인터형 지역변수 참조
    fun1(e);
    fun2(e2);
    fun3(1, 2, 3, 4, 5, 6,7);
    c=fun4(1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0);
    b=fun6(a, b, c, &a, e,g, 1, 2,3);
}
