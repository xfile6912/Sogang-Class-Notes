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

int N;
int *h_Fibonacci_number;
int *h_fibonacci_value;
int *h_fibonacci_value_GPU;

cudaError_t Fibonacci_GPU(int *h_Fibo_n, int *h_Fibo_v, int *h_Fibo_v_GPU);

__global__ void Fibonacci_Kernel(int *x, int* y)
{
    float sqrt_5, x_0, x_1, tmp_0, tmp_1;//fibonacci kernel
    int j;
    /*Thread Idx*/
    int row=blockDim.y*blockIdx.y+threadIdx.y;
    int col= blockDim.x * blockIdx.x + threadIdx.x;
    int tid=gridDim.x*blockDim.x*row+col;
    //tid 구함.
    /*Todo*/
        //실습자료에 있는 int Fibonacci(int n) 함수의 n이 여기서의 x[tid]이고,
    //y[i]가 바로 y[tid]라고 할수 잇음.
    sqrt_5 = sqrtf(5.0f);
    x_0 = (1.0f + sqrt_5) / 2.0f; x_1 = (1.0f - sqrt_5) / 2.0f;
    tmp_0 = tmp_1 = 1.0f;

    for (j = 0; j < x[tid]; j++) {
        tmp_0 *= x_0; tmp_1 *= x_1;
    }
    y[tid] = (int)((tmp_0 - tmp_1) / sqrt_5 + 0.5f);
}


void Fibonacci_CPU(int *h_Fibo_n, int *h_Fibo_v) {
    int i, j;

    float sqrt_5, x_0, x_1, tmp_0, tmp_1;

    sqrt_5 = sqrtf(5.0f);
    x_0 = (1.0f + sqrt_5) / 2.0f; x_1 = (1.0f - sqrt_5) / 2.0f;
    tmp_0 = tmp_1 = 1.0f;

    for (i = 0; i < N; i++) {
        tmp_0 = tmp_1 = 1.0f;
        for (j = 0; j < h_Fibo_n[i]; j++) {
            tmp_0 *= x_0; tmp_1 *= x_1;
        }
        h_Fibo_v[i] = (int)((tmp_0 - tmp_1) / sqrt_5 + 0.5f);
    }
}

void init_bin_file(int size) {
    srand((unsigned)time(NULL));
    printf("***Binary File init Start!!\n");
    FILE *fp = fopen("x.bin", "wb");

    fwrite(&size, sizeof(int), 1, fp);

    int random_num;
    for (int i = 0; i < size; i++) {
        random_num = (int)(((double)rand() / RAND_MAX) * 40.0f);
        fwrite(&random_num, sizeof(int), 1, fp);
    }

    fclose(fp);
    printf("***Binary File init End!!\n\n");
}

void read_bin_file()
{
    printf("***Binary File Read Start!!\n");
    FILE *fp = fopen("x.bin", "rb");
    /*Todo*/
    fread(&N, sizeof(int), 1, fp);//파일에서 N을 받아서 각각에 공간 할당해줌.
     h_Fibonacci_number= (int*)malloc(sizeof(int) * N);
     h_fibonacci_value= (int*)malloc(sizeof(int) * N);//cpu로 수행한 결과가 저장됨
     h_fibonacci_value_GPU= (int*)malloc(sizeof(int) * N);//gpu로 수행한 결과가 저장됨.
    int i;
    for (i = 0; i < N; i++)//N개만큼 숫자 읽어와 h_Fibonacci_number로 저장
    {
        fread(&h_Fibonacci_number[i], sizeof(int), 1, fp);
    }


    fclose(fp);
    printf("***Binary File Read End!!\n\n");
}

int main()
{
    init_bin_file(DATA_SIZE);
    read_bin_file();

    printf("The problem size is %d.\n", N);
    int i;
    FILE *fp;

    //CPU 
    printf("***Fibonacci_CPU Start!!\n");
    CHECK_TIME_START;
    Fibonacci_CPU(h_Fibonacci_number, h_fibonacci_value);
    CHECK_TIME_END(compute_time);
    printf("***Fibonacci_CPU End!!\n");

    printf("CPU time = %.6f\n\n", compute_time);
    //GPU

    printf("***Fibonacci_GPU Start!!\n");
    Fibonacci_GPU(h_Fibonacci_number, h_fibonacci_value, h_fibonacci_value_GPU);
    printf("***Fibonacci_GPU End!!\n");

    printf("GPU time = %.6f\n\n", device_time);

    for (i = 0; i < N; i++)
    {
        if (h_fibonacci_value[i] != h_fibonacci_value_GPU[i])
        {
            printf("%u != %u\n", h_fibonacci_value[i], h_fibonacci_value_GPU[i]);
            break;
        }
    }

    if (i == N)
        printf("***Kernel execution Success!!\n\n");

    // Write the output array into the output file.
    fp = fopen("y.bin", "wb");
    if (!fp) {
        fprintf(stderr, "Error: cannot open the output file...\n");
        exit(-1);
    }

    fwrite(&N, sizeof(int), 1, fp);

    for (i = 0; i < N; i++)
        fwrite(&h_fibonacci_value_GPU[i], sizeof(int), 1, fp);

    fclose(fp);
    printf("end!!\n\n");

    return 0;
}


cudaError_t Fibonacci_GPU(int *h_Fibo_n, int *h_Fibo_v, int *h_Fibo_v_GPU)
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
    int* d_Fibo_n;
    int* d_Fibo_v_GPU;
    size_t size=N*sizeof(int);

    CUDA_CALL(cudaMalloc(&d_Fibo_n, size))//gpu에 h_Fibo_n을 저장하기 위하여 메모리 할당.

        CUDA_CALL(cudaMemcpy(d_Fibo_n, h_Fibo_n, size, cudaMemcpyHostToDevice))//gpu(d_Fibon_n)로 h_Fibo_n copy함.

        CUDA_CALL(cudaMalloc(&d_Fibo_v_GPU, size))// gpu내에 결과값이 저장될 memory 할당.


        // Assume that width and height are multiples of BLOCK SIZE.
        dim3 dimBlock(BLOCK_SIZE);//block dimension 1차원, block size
        dim3 dimGrid(N/BLOCK_SIZE);//n/block_size가 grid의 dimension
    CHECK_TIME_START_GPU()
        Fibonacci_Kernel << < dimGrid, dimBlock >> > (d_Fibo_n, d_Fibo_v_GPU);//kernel 수행
    CHECK_TIME_END_GPU(device_time)
        CUDA_CALL(cudaGetLastError())

        // cudaDeviceSynchronize waits for the kernel to finish, and returns
        // any errors encountered during the launch.
        CUDA_CALL(cudaDeviceSynchronize())
        CUDA_CALL(cudaMemcpy(h_Fibo_v_GPU, d_Fibo_v_GPU, size, cudaMemcpyDeviceToHost))//gpu메모리로부터 계산결과 카피

        Error:
    cudaFree(d_Fibo_n);
    cudaFree(d_Fibo_v_GPU);
    CHECK_TIME_DEST_GPU();
    return cudaStatus;
}