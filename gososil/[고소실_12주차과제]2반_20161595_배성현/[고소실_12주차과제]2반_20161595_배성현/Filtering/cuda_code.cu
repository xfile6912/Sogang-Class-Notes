#pragma once

#include "cuda_code.cuh"

#include <stdio.h>
#include<stdio.h>
#include<stdlib.h>
#include <math.h>
#include <Windows.h>
#include <time.h>
#include <assert.h>

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

#define Window 2
#define BLOCK_SIZE 512												// CUDA 커널 thread block 사이즈

#define BLOCK_WIDTH (1 << 3)
#define BLOCK_HEIGHT (BLOCK_SIZE / BLOCK_WIDTH)
__constant__ float constant_gaussian_kernel[ 25 ];

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	
//	Gaussian 필터링을 하는 커널
//	shared memory를 사용하지 않는다
//	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

__global__ void Gaussian_kernel_no_shared(IN unsigned char *d_bitmaps, OUT unsigned char *d_Gaussian, long width, long height) {
	/*Todo*/
	const unsigned block_id = blockIdx.y * gridDim.x + blockIdx.x;
	const unsigned thread_id = threadIdx.y * blockDim.x + threadIdx.x;
	const unsigned id = block_id * BLOCK_SIZE + thread_id;
	int w = 2;
	int i, j;
	int row = id / width;
	int col = id % width;
	d_Gaussian[id] = 0;
	for (i = -w; i <= w; i++)
	{
		for (j = -w; j <= w; j++)
		{
			if (row + i >= 0 && col + j >= 0 && row + i < height && col + j < width)//범위안에 있을 때
			{
				d_Gaussian[id] += constant_gaussian_kernel[(i + w) * 5 + j + w] * d_bitmaps[(row+i)*width+(col+j)];//i+w는 0<i+w<w0, j+w는 0<j+w<w0
			}
			else//범위 밖에 있을때
			{
				d_Gaussian[id] += 0;
			}
		}
	}


}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	
//	Gaussian 필터링을 하는 커널
//	shared memory를 사용한다.
//	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern __shared__ unsigned char sharedBuffer[];
__global__ void Gaussian_kernel_shared(INOUT unsigned char *d_bitmaps, OUT unsigned char *d_Gaussian, long width, long height) {
	/*Todo*/
	const unsigned block_id = blockIdx.y * gridDim.x + blockIdx.x;
	const unsigned thread_id = threadIdx.y * blockDim.x + threadIdx.x;
	const unsigned id = block_id * BLOCK_SIZE + thread_id;
	int row=id/width;
	int col=id%width;
	int sharedRowSize = BLOCK_SIZE + 4;//5개의 row가 생기는데 row size는 2+ BLOCK_SIZE+2 만큼.
	int i;
	if (thread_id == 0)
	{
		for (i = 0; i < 2; i++)
		{
			if (col + i < 2)
			{
					sharedBuffer[i + sharedRowSize * 0] = 0;
					sharedBuffer[i + sharedRowSize * 1] = 0;
					sharedBuffer[i + sharedRowSize * 2] = 0;//id 자기 자신
					sharedBuffer[i + sharedRowSize * 3] = 0;
					sharedBuffer[i + sharedRowSize * 4] = 0;
			}
			else
			{
				if (row - 2 >= 0)//id보다 두칸 위쪽 row
					sharedBuffer[i + sharedRowSize * 0] = d_bitmaps[(row - 2) * width + col+i-2];
				else
					sharedBuffer[i + sharedRowSize * 0] = 0;

				if (row - 1 >= 0)//id보다 한칸 위쪽 row
					sharedBuffer[i + sharedRowSize * 1] = d_bitmaps[(row - 1) * width + col + i - 2];
				else
					sharedBuffer[i + sharedRowSize * 1] = 0;

				sharedBuffer[i + sharedRowSize * 2] = d_bitmaps[id + i - 2];//id 자기 자신

				if (row + 1 < height)
					sharedBuffer[i + sharedRowSize * 3] = d_bitmaps[(row + 1) * width + col + i - 2];//id보다 1칸 아래쪽 row
				else
					sharedBuffer[i+ sharedRowSize * 3] = 0;

				if (row + 2 < height)
					sharedBuffer[i + sharedRowSize * 4] = d_bitmaps[(row + 2) * width + col + i - 2];//id보다 두칸 아래쪽 row
				else
					sharedBuffer[i + sharedRowSize * 4] = 0;
			}
		}
	}
	if (thread_id == BLOCK_SIZE - 1)
	{
		for (i = 1; i <= 2; i++)
		{
			if (col + i >= width)
			{
				sharedBuffer[thread_id + 2+i + sharedRowSize * 0] = 0;
				sharedBuffer[thread_id + 2+i + sharedRowSize * 1] = 0;
				sharedBuffer[thread_id + 2+i + sharedRowSize * 2] = 0;//id 자기 자신
				sharedBuffer[thread_id + 2+i + sharedRowSize * 3] = 0;
				sharedBuffer[thread_id + 2+i + sharedRowSize * 4] = 0;
			}
			else
			{
				if (row - 2 >= 0)//id보다 두칸 위쪽 row
					sharedBuffer[thread_id + 2+i + sharedRowSize * 0] = d_bitmaps[(row - 2) * width + col + i];
				else
					sharedBuffer[thread_id + 2+i + sharedRowSize * 0] = 0;

				if (row - 1 >= 0)//id보다 한칸 위쪽 row
					sharedBuffer[thread_id + 2+i + sharedRowSize * 1] = d_bitmaps[(row - 1) * width + col + i];
				else
					sharedBuffer[thread_id + 2+i + sharedRowSize * 1] = 0;

				sharedBuffer[thread_id + 2+i + sharedRowSize * 2] = d_bitmaps[id + i];//id 자기 자신

				if (row + 1 < height)
					sharedBuffer[thread_id + 2+i + sharedRowSize * 3] = d_bitmaps[(row + 1) * width + col + i];//id보다 1칸 아래쪽 row
				else
					sharedBuffer[thread_id + 2+i + sharedRowSize * 3] = 0;

				if (row + 2 < height)
					sharedBuffer[thread_id + 2+i + sharedRowSize * 4] = d_bitmaps[(row + 2) * width + col + i];//id보다 두칸 아래쪽 row
				else
					sharedBuffer[thread_id + 2+i + sharedRowSize * 4] = 0;
			}
		}
	}

	if (row - 2 >= 0)//id보다 두칸 위쪽 row
		sharedBuffer[thread_id + 2 + sharedRowSize * 0] = d_bitmaps[(row - 2) * width + col];
	else
		sharedBuffer[thread_id + 2 + sharedRowSize * 0] = 0;

	if (row - 1 >= 0)//id보다 한칸 위쪽 row
		sharedBuffer[thread_id + 2 + sharedRowSize * 1] = d_bitmaps[(row - 1) * width + col];
	else
		sharedBuffer[thread_id + 2 + sharedRowSize * 1] = 0;
	sharedBuffer[(thread_id + 2) + sharedRowSize * 2] = d_bitmaps[id];//id 자기 자신
	
	if (row + 1 < height)
		sharedBuffer[thread_id + 2 + sharedRowSize * 3] = d_bitmaps[(row + 1) * width + col];//id보다 1칸 아래쪽 row
	else
		sharedBuffer[thread_id + 2 + sharedRowSize * 3] = 0;
	if (row +2 < height)
		sharedBuffer[thread_id + 2 + sharedRowSize * 4] = d_bitmaps[(row + 2) * width + col];//id보다 두칸 아래쪽 row
	else
		sharedBuffer[thread_id + 2 + sharedRowSize * 4] = 0;

	__syncthreads();
	
	int j;
	d_Gaussian[id] = 0;
	for (i = 0; i < 5; i++)
	{
		for (j = 0; j < 5; j++)
		{
			d_Gaussian[id] += constant_gaussian_kernel[i * 5 + j] * sharedBuffer[i * sharedRowSize + thread_id + j];
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	
//	Constant variable 인 gaussian kernel을 설정하는 함수
//	후에 gaussian filtering 에서 사용한다.
//	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Set_Gaussain_Kernel(){
	float _1 = 1.0f / 256.0f;
	float _4 = _1 * 4;
	float _6 = _1 * 6;
	float _16 = _1 * 16;
	float _24 = _1 * 24;
	float _36 = _1 * 36;

	float *p_gaussian_kernel = new float[25];

	p_gaussian_kernel[0] = p_gaussian_kernel[4] = p_gaussian_kernel[20] = p_gaussian_kernel[24] = _1;
	p_gaussian_kernel[1] = p_gaussian_kernel[3] = p_gaussian_kernel[5] = p_gaussian_kernel[9]= _4;
	p_gaussian_kernel[15] = p_gaussian_kernel[19] = p_gaussian_kernel[21] = p_gaussian_kernel[23] = _4;
	p_gaussian_kernel[2] = p_gaussian_kernel[10] = p_gaussian_kernel[14] = p_gaussian_kernel[22] = _6;
	p_gaussian_kernel[6] = p_gaussian_kernel[8] = p_gaussian_kernel[16] = p_gaussian_kernel[18] = _16;
	p_gaussian_kernel[7] = p_gaussian_kernel[11] =p_gaussian_kernel[13] = p_gaussian_kernel[17] = _24;
	p_gaussian_kernel[12] = _36;

	cudaMemcpyToSymbol( constant_gaussian_kernel, p_gaussian_kernel, sizeof( float ) * 25 );

	delete[] p_gaussian_kernel;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	
//	커널을 실행하기 전 필요한 자료들 준비 및 커널을 실행할 디바이스를 설정
//	Shared_flag 입력 시 NO_SHARED 나 SHARED 중 한 개의 매크로를 넣으면
//	flag값에 맞는 커널을 실행
//	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

float Do_Gaussian_on_GPU(IN unsigned char *p_bitmaps, OUT unsigned char *p_Gaussian, long width, long height, int Shared_flag)
{
	cudaError_t cudaStatus;
	float device_time;
	CUDA_CALL(cudaSetDevice(0));

	unsigned char* d_bitmaps;
	unsigned char*d_Gaussian;
	size_t mem_size;
	mem_size = width*height* sizeof(unsigned char);
	CUDA_CALL(cudaMalloc(&d_bitmaps, mem_size));
	CUDA_CALL(cudaMalloc(&d_Gaussian, mem_size));
	CUDA_CALL(cudaMemcpy(d_bitmaps, p_bitmaps, mem_size, cudaMemcpyHostToDevice));

	Set_Gaussain_Kernel();

	dim3 blockDIm(BLOCK_SIZE);
	dim3 gridDim(width*height/BLOCK_SIZE);
	CHECK_TIME_INIT_GPU();
	CHECK_TIME_START_GPU();

	switch (Shared_flag)
	{
	case NO_SHARED:
		Gaussian_kernel_no_shared << <gridDim, blockDIm >> > (d_bitmaps, d_Gaussian, width, height);
		break;
	case SHARED:
		Gaussian_kernel_shared << <gridDim, blockDIm, sizeof(unsigned char)* (BLOCK_SIZE + 2 *2)*5>> > (d_bitmaps, d_Gaussian, width, height);
		break;

	}
	CHECK_TIME_END_GPU(device_time);
	CUDA_CALL(cudaStatus = cudaDeviceSynchronize());

	CHECK_TIME_DEST_GPU();
	CUDA_CALL(cudaMemcpy(p_Gaussian, d_Gaussian, mem_size, cudaMemcpyDeviceToHost));

	cudaFree(d_bitmaps);
	cudaFree(d_Gaussian);

	return device_time;
}