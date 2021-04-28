
#include "cuda_runtime.h"
#include "device_launch_parameters.h"

#include <stdio.h>
#include<stdio.h>
#include<stdlib.h>
#include <math.h>
#include <Windows.h>
#include <time.h>
#include <assert.h>

//1 prac 2
//2 prac 3
#define prac 2


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

TIMER_T compute_time = 0;
TIMER_T device_time = 0;

#if prac==1



typedef struct {
    int width;
    int height;
    float *elements;
} Array;


#define MAX_N_ELEMENTS	(1 << 20)

void generate_random_float_array(float *array, int n) {

    int i;

    for (i = 0; i < n; i++) {
        array[i] = 3.1415926f*((float)rand() / RAND_MAX);
    }
}
void combine_two_arrays(float *x, float *y, float *z, int n) {
    int i;

    for (i = 0; i < n; i++) {
        z[i] = 1.0f / (sin(x[i])*cos(y[i]) + cos(x[i])*sin(y[i]));
    }
}

__global__ void CombineTwoArrraysKernel(Array A, Array B, Array C) {
    int row = blockDim.y*blockIdx.y + threadIdx.y;
    int col = blockDim.x*blockIdx.x + threadIdx.x;
    int id = gridDim.x*blockDim.x*row + col;
    C.elements[id] = 1.0f / (sin(A.elements[id])*cos(B.elements[id]) + cos(A.elements[id])*sin(B.elements[id]));
}

cudaError_t combine_two_arrays_GPU(const Array A, const Array B, Array C);

int BLOCK_SIZE = 16;

int main()
{
    int n_elements;

    srand((unsigned int)time(NULL));
    n_elements = MAX_N_ELEMENTS;
    Array A, B, C, G;
    A.width = B.width = C.width = G.width = 1024;
    A.height = B.height = C.height = G.height = MAX_N_ELEMENTS / 1024;

    A.elements = (float *)malloc(sizeof(float)*MAX_N_ELEMENTS);
    B.elements = (float *)malloc(sizeof(float)*MAX_N_ELEMENTS);
    C.elements = (float *)malloc(sizeof(float)*MAX_N_ELEMENTS);
    G.elements = (float *)malloc(sizeof(float)*MAX_N_ELEMENTS);
    generate_random_float_array(A.elements, MAX_N_ELEMENTS);
    generate_random_float_array(B.elements, MAX_N_ELEMENTS);

    CHECK_TIME_START;
    combine_two_arrays(A.elements, B.elements, C.elements, n_elements);
    CHECK_TIME_END(compute_time);

    printf("***GPU C[10] = %f/ Time taken = %.6fms\n", C.elements[10], compute_time);


    cudaError_t cudaStatus = combine_two_arrays_GPU(A, B, G);
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "combine_two_arrays_GPU failed!");
        return 1;
    }
    printf("***GPU G[10] = %f/ Time taken = %.6fms\n", G.elements[10], device_time);

    // cudaDeviceReset must be called before exiting in order for profiling and
    // tracing tools such as Nsight and Visual Profiler to show complete traces.
    cudaStatus = cudaDeviceReset();
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "cudaDeviceReset failed!");
        return 1;
    }

    return 0;
}
cudaError_t combine_two_arrays_GPU(const Array A, const Array B, Array C) {

    //아래 함수들을 사용하여 어떻게 하면 가급적 정확한 시간을 측정할 수 있을지 생각해볼 것.
    CHECK_TIME_INIT_GPU()
        CHECK_TIME_START_GPU()
        CHECK_TIME_END_GPU(device_time)
        CHECK_TIME_DEST_GPU()

        cudaError_t cudaStatus;
    // Choose which GPU to run on, change this on a multi-GPU system.
    cudaStatus = cudaSetDevice(0);
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "cudaSetDevice failed!  Do you have a CUDA-capable GPU installed?");
        goto Error;
    }/////////////  if(cu.....  ==CUDA_CALL

    Array d_A, d_B, d_C;
    size_t size;

    d_A.width = A.width; d_A.height = A.height;
    size = A.width * A.height * sizeof(float);
    CUDA_CALL(cudaMalloc(&d_A.elements, size))

        CUDA_CALL(cudaMemcpy(d_A.elements, A.elements, size, cudaMemcpyHostToDevice))

        d_B.width = B.width; d_B.height = B.height;
    size = B.width * B.height * sizeof(float);
    CUDA_CALL(cudaMalloc(&d_B.elements, size))
        CUDA_CALL(cudaMemcpy(d_B.elements, B.elements, size, cudaMemcpyHostToDevice))

        d_C.width = C.width; d_C.height = C.height;
    size = C.width * C.height * sizeof(float);
    CUDA_CALL(cudaMalloc(&d_C.elements, size))

        // Assume that width and height are multiples of BLOCK SIZE.
        dim3 dimBlock(BLOCK_SIZE, BLOCK_SIZE);
    dim3 dimGrid(A.width / dimBlock.x, A.height / dimBlock.y);
    CombineTwoArrraysKernel << < dimGrid, dimBlock >> > (d_A, d_B, d_C);

    CUDA_CALL(cudaGetLastError())

        // cudaDeviceSynchronize waits for the kernel to finish, and returns
        // any errors encountered during the launch.
        CUDA_CALL(cudaDeviceSynchronize())
        CUDA_CALL(cudaMemcpy(C.elements, d_C.elements, size, cudaMemcpyDeviceToHost))


        Error:
    cudaFree(d_A.elements);
    cudaFree(d_B.elements);
    cudaFree(d_C.elements);
    return cudaStatus;
}
#endif

#if prac==2

int n;
#define BLOCK_SIZE 32
#define ELEMENT_SIZE (1<<10)
const int ELEM_PER_VECTOR = 32;
float(*pVecX), (*pVecY), (*pVecY_G);
float(*pMatA);
void init_MatVec(void)
{
    srand((unsigned)time(NULL));
    FILE* fp = fopen("gen.bin", "rb");
    fread(&n, sizeof(float), 1, fp);

    pVecX = new float[n * ELEM_PER_VECTOR];
    pVecY = new float[n * ELEM_PER_VECTOR];
    pVecY_G = new float[n * ELEM_PER_VECTOR];
    pMatA = new float[ELEM_PER_VECTOR * ELEM_PER_VECTOR];

    fread(pVecX, sizeof(float), n * ELEM_PER_VECTOR, fp);
    fread(pMatA, sizeof(float), ELEM_PER_VECTOR * ELEM_PER_VECTOR, fp);
    fclose(fp);
}
void Mat_Vec_Multiply()
{
    int vec_idx, i, j;

    for (vec_idx = 0; vec_idx < ELEMENT_SIZE; vec_idx++) {
        for (i = 0; i < ELEM_PER_VECTOR; i++) {
            float sum = 0;
            for (j = 0; j < ELEM_PER_VECTOR; j++) {
                sum += pMatA[i * ELEM_PER_VECTOR + j] * pVecX[vec_idx * ELEM_PER_VECTOR + j];
            }
            pVecY[vec_idx * ELEM_PER_VECTOR + i] = sum;
        }
    }
}

__global__ void Mat_Vec_Multiply_Kernel(float *d_VecY, float *d_VecX, float *d_MatA, int Vec_Size)
{
    //하나의 task에 대하여 결과를 내는 코드.
    int row = blockDim.y * blockIdx.y + threadIdx.y;
    int col = blockDim.x * blockIdx.x + threadIdx.x;
    int id = gridDim.x * blockDim.x * row + col;
    int i, j;
    for (i = 0; i < Vec_Size; i++) {
        float sum = 0;
        for (j = 0; j < Vec_Size; j++) {
            sum += d_MatA[i * Vec_Size + j] * d_VecX[id * Vec_Size + j];
        }
        d_VecY[id * Vec_Size + i] = sum;
    }
}

void Mat_Vec_Multiply_GPU(float *p_VecX, float *p_MatA, float *p_VecY_G)
{
    //아래 함수들을 사용하여 어떻게 하면 가급적 정확한 시간을 측정할 수 있을지 생각해볼 것.
    CHECK_TIME_INIT_GPU()

        

        cudaError_t cudaStatus;
    // Choose which GPU to run on, change this on a multi-GPU system.
    cudaStatus = cudaSetDevice(0);
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "cudaSetDevice failed!  Do you have a CUDA-capable GPU installed?");
        goto Error;
    }/////////////  if(cu.....  ==CUDA_CALL
    float* d_VecX;
    float* d_MatA;
    float* d_VecY;
    size_t sizeMat;//matrix의 사이즈
    size_t sizeVec;//vector의 사이즈
    sizeMat = sizeof(float) * ELEM_PER_VECTOR * ELEM_PER_VECTOR;
    sizeVec = sizeof(float) * ELEM_PER_VECTOR * n;

    CUDA_CALL(cudaMalloc(&d_VecX, sizeVec))

        CUDA_CALL(cudaMemcpy(d_VecX, p_VecX, sizeVec, cudaMemcpyHostToDevice))

    CUDA_CALL(cudaMalloc(&d_MatA, sizeMat))

        CUDA_CALL(cudaMemcpy(d_MatA, p_MatA, sizeMat, cudaMemcpyHostToDevice))

    CUDA_CALL(cudaMalloc(&d_VecY, sizeVec))


        // Assume that width and height are multiples of BLOCK SIZE.
        dim3 dimBlock(BLOCK_SIZE);//한블럭에 몇개의 thread가 들어갈지 결정.
    dim3 dimGrid(n/BLOCK_SIZE);
    int Vec_Size = ELEM_PER_VECTOR;
    CHECK_TIME_START_GPU()
    Mat_Vec_Multiply_Kernel << < dimGrid, dimBlock >> > (d_VecY, d_VecX, d_MatA, Vec_Size);
    CHECK_TIME_END_GPU(device_time)
    CUDA_CALL(cudaGetLastError())

        // cudaDeviceSynchronize waits for the kernel to finish, and returns
        // any errors encountered during the launch.
        CUDA_CALL(cudaDeviceSynchronize())
        CUDA_CALL(cudaMemcpy(pVecY_G, d_VecY, sizeVec, cudaMemcpyDeviceToHost))

        Error:
            cudaFree(d_VecX);
            cudaFree(d_MatA);
            cudaFree(d_VecY);
         CHECK_TIME_DEST_GPU()
}

void init_data(int size) {
    srand((unsigned)time(NULL));
    FILE *fp = fopen("gen.bin", "wb");
    fwrite(&size, sizeof(int), 1, fp);

    int i, j;
    float x;

    for (i = 0; i < size; i++) {
        for (j = 0; j < ELEM_PER_VECTOR; j++) {
            x = 2.0f*((float)rand() / RAND_MAX) - 1.0f;
            fwrite(&x, sizeof(float), 1, fp);
        }
    }

    for (i = 0; i < ELEM_PER_VECTOR; i++) {
        for (j = 0; j < ELEM_PER_VECTOR; j++) {
            x = 2.0f*((float)rand() / RAND_MAX) - 1.0f;
            fwrite(&x, sizeof(float), 1, fp);
        }
    }

    fclose(fp);

    return;
}

int main()
{
    init_data(ELEMENT_SIZE);
    init_MatVec();
    printf("n = %d  file open ok.\n", n);

    CHECK_TIME_START;
    Mat_Vec_Multiply();
    CHECK_TIME_END(compute_time);

    printf("***CPU C[10] = %.3f/ Time taken = %.6fms\n", pVecY[0 * ELEM_PER_VECTOR + 0], compute_time);
    Mat_Vec_Multiply_GPU(pVecX, pMatA, pVecY_G);
    printf("***GPU C[10] = %.3f/ Time taken = %.6fms\n", pVecY_G[0 * ELEM_PER_VECTOR + 0], device_time);

    int vec_idx, i;

    for (i = 0; i < ELEMENT_SIZE * ELEM_PER_VECTOR; i++) {
        if (fabs(pVecY[i] - pVecY_G[i]) > 0.001) {
            printf("Kernel execution fail!!\n\n");
            break;
        }
    }
}

#endif