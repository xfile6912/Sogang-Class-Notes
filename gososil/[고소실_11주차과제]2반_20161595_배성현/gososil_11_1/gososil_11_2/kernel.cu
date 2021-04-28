#include "cuda_runtime.h"
#include "device_launch_parameters.h"

#include <stdio.h>
#include<stdio.h>
#include<stdlib.h>
#include <math.h>
#include <Windows.h>
#include <time.h>
#include <assert.h>

#define CUDA_CALL(x) { const cudaError_t a = (x); if(a != cudaSuccess) { printf("\nCuda Error: %s (err_num=%d) at line:%d\n", cudaGetErrorString(a), a, __LINE__); cudaDeviceReset(); assert(0);}}
typedef float TIMER_T;

#define USE_CPU_TIMER 1
#define USE_GPU_TIMER 1

#if USE_CPU_TIMER == 1
__int64 start, freq, end;
#define CHECK_TIME_START { QueryPerformanceFrequency((LARGE_INTEGER*)&freq); QueryPerformanceCounter((LARGE_INTEGER*)&start); }
#define CHECK_TIME_END(a) { QueryPerformanceCounter((LARGE_INTEGER*)&end); a = (float)((float)(end - start) / (freq / 1000.0f)); }
#else
#define CHECK_TIME_START
#define CHECK_TIME_END(a)
#endif

#if USE_GPU_TIMER == 1
cudaEvent_t cuda_timer_start, cuda_timer_stop;
#define CUDA_STREAM_0 (0)

void create_device_timer()
{
    CUDA_CALL(cudaEventCreate(&cuda_timer_start));
    CUDA_CALL(cudaEventCreate(&cuda_timer_stop));
}

void destroy_device_timer()
{
    CUDA_CALL(cudaEventDestroy(cuda_timer_start));
    CUDA_CALL(cudaEventDestroy(cuda_timer_stop));
}

inline void start_device_timer()
{
    cudaEventRecord(cuda_timer_start, CUDA_STREAM_0);
}

inline TIMER_T stop_device_timer()
{
    TIMER_T ms;
    cudaEventRecord(cuda_timer_stop, CUDA_STREAM_0);
    cudaEventSynchronize(cuda_timer_stop);

    cudaEventElapsedTime(&ms, cuda_timer_start, cuda_timer_stop);
    return ms;
}

#define CHECK_TIME_INIT_GPU() { create_device_timer(); }
#define CHECK_TIME_START_GPU() { start_device_timer(); }
#define CHECK_TIME_END_GPU(a) { a = stop_device_timer(); }
#define CHECK_TIME_DEST_GPU() { destroy_device_timer(); }
#else
#define CHECK_TIME_INIT_GPU()
#define CHECK_TIME_START_GPU()
#define CHECK_TIME_END_GPU(a)
#define CHECK_TIME_DEST_GPU()
#endif

#define BLOCK_SIZE 64
#define DATA_SIZE (1 << 26)

TIMER_T compute_time = 0;
TIMER_T device_time = 0;
#define N_EQUATIONS 1 << 26
int N;
float* A;
float* B;
float* C;
float* X0;
float* X1;
float* FX0;
float* FX1;
float* X0_gpu;
float* X1_gpu;
float* FX0_gpu;
float* FX1_gpu;

cudaError_t Equation_GPU(float* A, float* B, float* C, float* X0, float* X1, float* fX0, float* fX1, int n);

__global__ void Equation_Kernel(float* A, float* B, float* C, float* X0, float* X1, float* FX0, float* FX1)
{
    float a, b, c, d, x0, x1, tmp;
    int row = blockDim.y * blockIdx.y + threadIdx.y;
    int col = blockDim.x * blockIdx.x + threadIdx.x;
    int tid = gridDim.x * blockDim.x * row + col;
        a = A[tid]; b = B[tid]; c = C[tid];
        d = sqrtf(b * b - 4.0f * a * c);
        tmp = 1.0f / (2.0f * a);
        X0[tid] = x0 = (-b - d) * tmp;
        X1[tid] = x1 = (-b + d) * tmp;
        FX0[tid] = (a * x0 + b) * x0 + c;
        FX1[tid] = (a * x1 + b) * x1 + c;
}


void Equation_CPU(float *A, float * B, float *C, float *X0, float *X1, float *FX0, float *FX1, int n) {
    int i;
    float a, b, c, d, x0, x1, tmp;
    for (i = 0; i < n; i++) {
        a = A[i]; b = B[i]; c = C[i];
        d = sqrtf(b * b - 4.0f * a * c);
        tmp = 1.0f / (2.0f * a);
        X0[i] = x0 = (-b - d) * tmp;
        X1[i] = x1 = (-b + d) * tmp;
        FX0[i] = (a * x0 + b) * x0 + c;
        FX1[i] = (a * x1 + b) * x1 + c;
    }
}

void init_bin_file() {
    srand((unsigned)time(NULL));
    N = N_EQUATIONS;
    A = (float*)malloc(sizeof(float) * N);
    B = (float*)malloc(sizeof(float) * N);
    C = (float*)malloc(sizeof(float) * N);
    X0 = (float*)malloc(sizeof(float) * N);
    X1 = (float*)malloc(sizeof(float) * N);
    FX0 = (float*)malloc(sizeof(float) * N);
    FX1 = (float*)malloc(sizeof(float) * N);
    X0_gpu = (float*)malloc(sizeof(float) * N);
    X1_gpu = (float*)malloc(sizeof(float) * N);
    FX0_gpu = (float*)malloc(sizeof(float) * N);
    FX1_gpu = (float*)malloc(sizeof(float) * N);//공간 할당.
    printf("***Binary File init Start!!\n");
    FILE *fp1 = fopen("A.bin", "wb");
    FILE* fp2 = fopen("B.bin", "wb");
    FILE* fp3 = fopen("C.bin", "wb");


    float random_a;
    float random_b;
    float random_c;
    for (int i = 0; i < N; i++) {
        random_a = (float)(((rand() %10)));
        random_c = (float)(((rand() %10)*(-1)));
        random_b = (float)(((rand() %10)));
        fwrite(&random_a, sizeof(float), 1, fp1);
        fwrite(&random_b, sizeof(float), 1, fp2);
        fwrite(&random_c, sizeof(float), 1, fp3);
    }

    fclose(fp1);
    fclose(fp2);
    fclose(fp3);
    printf("***Binary File init End!!\n\n");
}

void read_bin_file()
{
    printf("***Binary File Read Start!!\n");
    FILE *fp1 = fopen("a.bin", "rb");
    FILE* fp2 = fopen("b.bin", "rb");
    FILE* fp3 = fopen("c.bin", "rb");
    /*Todo*/

    int i;
    for (i = 0; i < N; i++)//N개만큼 숫자 읽어와 h_Fibonacci_number로 저장
    {
        fread(&A[i], sizeof(float), 1, fp1);
        fread(&B[i], sizeof(float), 1, fp2);
        fread(&C[i], sizeof(float), 1, fp3);
    }


    fclose(fp1);
    fclose(fp2);
    fclose(fp3);
    printf("***Binary File Read End!!\n\n");
}

int main()
{
    init_bin_file();
    read_bin_file();

    printf("The problem size is %d.\n", N);
    int i;

    //CPU 
    printf("***Equation_CPU Start!!\n");
    CHECK_TIME_START;
    Equation_CPU(A, B, C, X0, X1, FX0, FX1, N);
    CHECK_TIME_END(compute_time);
    printf("***Equation_CPU End!!\n");

    printf("CPU time = %.6f\n\n", compute_time);
    //GPU

    printf("***Equation_GPU Start!!\n");
    Equation_GPU(A, B, C, X0_gpu, X1_gpu, FX0_gpu, FX1_gpu, N);
    printf("***Equation_GPU End!!\n");

    printf("GPU time = %.6f\n", device_time);

    for (i = 0; i < N; i++)
    {
        if (fabs(X0[i] - X0_gpu[i])>0.0001 || fabs(X1[i] - X1_gpu[i])>0.0001)
        {
            printf("x0[i] %f, x0_gpu[i] %f\n", X0[i], X0_gpu[i]);
            printf("x1[i] %f, x1_gpu[i] %f\n", X1[i], X1_gpu[i]);
            break;
        }
        if (fabs(FX0_gpu[i]) > 0.0001 || fabs(FX1_gpu[i]) > 0.0001)
        {
            printf("fx0_gpu[i] %f\n", FX0_gpu[i]);
            printf("fx1_gpu[i] %f\n", FX1_gpu[i]);
            break;
        }
    }

    if (i == N)
        printf("***Kernel execution Success!!\n\n");

    // Write the output array into the output file.
    FILE *fp1 = fopen("X0.bin", "wb");
    if (!fp1) {
        fprintf(stderr, "Error: cannot open the output file...\n");
        exit(-1);
    }
    FILE* fp2 = fopen("X1.bin", "wb");
    if (!fp2) {
        fprintf(stderr, "Error: cannot open the output file...\n");
        exit(-1);
    }
    FILE* fp3 = fopen("FX0.bin", "wb");
    if (!fp3) {
        fprintf(stderr, "Error: cannot open the output file...\n");
        exit(-1);
    }
    FILE* fp4 = fopen("FX1.bin", "wb");
    if (!fp4) {
        fprintf(stderr, "Error: cannot open the output file...\n");
        exit(-1);
    }

    fwrite(&X0_gpu, sizeof(float), N, fp1);
    fwrite(&X1_gpu, sizeof(float), N, fp2);
    fwrite(&FX0_gpu, sizeof(float), N, fp3);
    fwrite(&FX1_gpu, sizeof(float), N, fp4);

    fclose(fp1);
    fclose(fp2);
    fclose(fp3);
    fclose(fp4);
    printf("end!!\n\n");

    return 0;
}


cudaError_t Equation_GPU(float* A, float* B, float* C, float* X0_gpu, float* X1_gpu, float* FX0_gpu, float* FX1_gpu, int n)
{
    CHECK_TIME_INIT_GPU();

    cudaError_t cudaStatus;
    /*Todo*/
    // Choose which GPU to run on, change this on a multi-GPU system.
    cudaStatus = cudaSetDevice(0);
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "cudaSetDevice failed!  Do you have a CUDA-capable GPU installed?");
        goto Error;
    }/////////////  if(cu.....  ==CUDA_CALL
    float* d_a;
    float* d_b;
    float* d_c;
    float* d_x0;
    float* d_x1;
    float* d_fx0;
    float* d_fx1;
    size_t size=N*sizeof(float);

    CUDA_CALL(cudaMalloc(&d_a, size))//gpu에 a를 저장하기 위하여 메모리 할당.
        CUDA_CALL(cudaMemcpy(d_a, A, size, cudaMemcpyHostToDevice))//gpu(d_a)로 a copy함.
        CUDA_CALL(cudaMalloc(&d_b, size))//gpu에 b를 저장하기 위하여 메모리 할당.
        CUDA_CALL(cudaMemcpy(d_b, B, size, cudaMemcpyHostToDevice))//gpu(d_b)로 b copy함.
        CUDA_CALL(cudaMalloc(&d_c, size))//gpu에 c를 저장하기 위하여 메모리 할당.
        CUDA_CALL(cudaMemcpy(d_c, C, size, cudaMemcpyHostToDevice))//gpu(d_c)로 c copy함.
        CUDA_CALL(cudaMalloc(&d_x0, size))// gpu내에 결과값이 저장될 memory 할당.
        CUDA_CALL(cudaMalloc(&d_x1, size))// gpu내에 결과값이 저장될 memory 할당.
        CUDA_CALL(cudaMalloc(&d_fx0, size))// gpu내에 결과값이 저장될 memory 할당.
        CUDA_CALL(cudaMalloc(&d_fx1, size))// gpu내에 결과값이 저장될 memory 할당.



        // Assume that width and height are multiples of BLOCK SIZE.
        dim3 dimBlock(BLOCK_SIZE);//block dimension 1차원, block size
        dim3 dimGrid(N/BLOCK_SIZE);//n/block_size가 grid의 dimension
    CHECK_TIME_START_GPU()
        Equation_Kernel << < dimGrid, dimBlock >> > (d_a, d_b, d_c, d_x0, d_x1, d_fx0, d_fx1);//kernel 수행
    CHECK_TIME_END_GPU(device_time)
        CUDA_CALL(cudaGetLastError())

        // cudaDeviceSynchronize waits for the kernel to finish, and returns
        // any errors encountered during the launch.
        CUDA_CALL(cudaDeviceSynchronize())
        CUDA_CALL(cudaMemcpy(X0_gpu, d_x0, size, cudaMemcpyDeviceToHost))//gpu메모리로부터 계산결과 카피
        CUDA_CALL(cudaMemcpy(X1_gpu, d_x1, size, cudaMemcpyDeviceToHost))//gpu메모리로부터 계산결과 카피
        CUDA_CALL(cudaMemcpy(FX0_gpu, d_fx0, size, cudaMemcpyDeviceToHost))//gpu메모리로부터 계산결과 카피
        CUDA_CALL(cudaMemcpy(FX1_gpu, d_fx1, size, cudaMemcpyDeviceToHost))//gpu메모리로부터 계산결과 카피

        Error:
    cudaFree(d_a);
    cudaFree(d_b);
    cudaFree(d_c);
    cudaFree(d_x0);
    cudaFree(d_x1);
    cudaFree(d_fx0);
    cudaFree(d_fx1);
    CHECK_TIME_DEST_GPU();
    return cudaStatus;
}