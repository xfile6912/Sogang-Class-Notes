#ifndef CNN_H
#define CNN_H
#include"mlp.h"
#ifndef __cplusplus
typedef struct ConvLayer ConvLayer;
typedef struct PoolLayer PoolLayer;
typedef struct CNN CNN;
#endif
//간단한 Convolution Layer를 구성하는 구조체로써 본 실습에서는 Convolution시 stride는 1 로 고정한다.
struct ConvLayer {
	float* feature;		//컨볼루션의 feature. [depth][f_size][f_size] 의 크기를 가지는 3차원 매트릭스.
	float* error;			//컨볼루션의 error.  [depth][f_size][f_size] 의 크기를 가지는 3차원 매트릭스.
	float* kernel;			//feature를 만들기 위한 kernel. [depth][k_size][k_size] 의 크기를 가지는 3차원 매트릭스.
	int f_size;			//feature의 width, height
	int k_size;			//kernel의 width, height
	int depth;
};
struct PoolLayer {
	float* feature;		//풀링의 feature. [depth][f_size][f_size] 3차원 매트릭스
	float* error;			//풀링의 error.
	int depth;
	int f_size;
};
struct CNN {
	int num_of_convolution;		//컨볼루션의 개수, 풀링의 개수. 이 값이 2이면 C1->S1->C1->S2 의 형태를 가지게 됨.
	float* input;						//입력 이미지.
	int input_size;						//입력 이미지의 width,height
	int input_depth;					//입력 이미지의 채널
	ConvLayer* conv;					//ConvLayer들의 배열
	PoolLayer* pool;					//PoolLayer들의 배열
	Network network;					//Fully-Connected 레이어(MLP와 동일)
};
inline CNN CreateCNN(int input_size, int input_depth, int num_of_convolution, int* kernel_depths, int kernel_size) {
	/*
	*	@TODO
	*	1. CNN구조체의 num_of_convolution, input_size, input_depth를 초기화.
	*	2. conv, pool 레이어를 num_of_convolution만큼 할당.
	*	3. Convolution시 stride는 1로 고정.
	*	4. kernel_depths 를 통해 각 층의 depth를 초기화 할 수 있고,
	*	5. kernel_size를 통해 각 층의 feature size역시 모두 계산할 수 있슴.
	*	6. MLP와 같이 kernel을 [-1,1] 로 초기화한다.
	*/
	CNN cnn;
	cnn.num_of_convolution = num_of_convolution;
	cnn.input_size = input_size;
	cnn.input_depth = input_depth;
	cnn.conv = (ConvLayer*)calloc(cnn.num_of_convolution, sizeof(ConvLayer));
	cnn.pool = (PoolLayer*)calloc(cnn.num_of_convolution, sizeof(PoolLayer));
	int prev_size = input_size;
	int prev_depth = input_depth;
	for (int i = 0; i < num_of_convolution; i++) {
		cnn.conv[i].depth = kernel_depths[i] / prev_depth;															//새로 생성될 feature의 depth는 input kernel depth에서 prev depth를 나누어 주어야 함
		cnn.conv[i].k_size = kernel_size;
		cnn.conv[i].kernel = (float*)calloc(cnn.conv[i].k_size*cnn.conv[i].k_size*kernel_depths[i], sizeof(float));


		cnn.conv[i].f_size = prev_size - (kernel_size - 1);
		cnn.conv[i].feature = (float*)calloc(cnn.conv[i].f_size*cnn.conv[i].f_size*cnn.conv[i].depth, sizeof(float));
		cnn.conv[i].error = (float*)calloc(cnn.conv[i].f_size*cnn.conv[i].f_size*cnn.conv[i].depth, sizeof(float));
		for (int j = 0; j < cnn.conv[i].k_size*cnn.conv[i].k_size*kernel_depths[i]; j++) {
			cnn.conv[i].kernel[j] = (float)rand() / RAND_MAX * 2 - 1.0F;
		}
		printf("conv : %dx%dx%d, %dx%dx%d\n", cnn.conv[i].f_size, cnn.conv[i].f_size, cnn.conv[i].depth, cnn.conv[i].k_size, cnn.conv[i].k_size, cnn.conv[i].depth);

		////////////////////////////////////////////////////////////////////////////////
		//
		//
		// PROBLEM 2: ADD YOUR CODE BELOW
		//
		// 주어진 실습의 Pooling layer에서는 2 x 2 filter Max Pooling을 사용합니다.
		// 이 조건을 가지고 아래의 빈칸 코드를 작성하세요.
		//
		//
		////////////////////////////////////////////////////////////////////////////////


		cnn.pool[i].depth = cnn.conv[i].depth;/* ADD YOUR CODE HERE */
		cnn.pool[i].f_size = cnn.conv[i].f_size/2;/* ADD YOUR CODE HERE */
		cnn.pool[i].feature = (float*)calloc(cnn.pool[i].f_size*cnn.pool[i].f_size*cnn.pool[i].depth, sizeof(float));
		cnn.pool[i].error = (float*)calloc(cnn.pool[i].f_size*cnn.pool[i].f_size*cnn.pool[i].depth, sizeof(float));
		prev_size = cnn.pool[i].f_size;
		prev_depth = cnn.conv[i].depth;
		printf("pool : %dx%dx%d\n", cnn.pool[i].f_size, cnn.pool[i].f_size, cnn.pool[i].depth);
	}
	/*
	*	Conv뒤에 사용할 MLP를 정의.
	*/
	int size_of_layers[] = { cnn.pool[num_of_convolution - 1].f_size*cnn.pool[num_of_convolution - 1].f_size*cnn.pool[num_of_convolution - 1].depth,100,10 };
	if (num_of_convolution == 0) {
		size_of_layers[0] = input_size * input_size*input_depth;
	}
	for (int i = 0; i < sizeof(size_of_layers) / sizeof(int); i++) {
		printf("fc : %d\n", size_of_layers[i]);
	}
	cnn.network = CreateNetwork(size_of_layers, 3);
	return cnn;
}
inline void Convolution3D(float* input3D, int input_size, int input_depth, ConvLayer* conv) {
	/*
	*	@TODO
	*	3차원 텐서인 input3D와 4차원 텐서인 ConvLayer::kernel를 이용하여
	*	3차원 텐서 ConvLayer::feature를 생성.
	*	@hint
	*	직렬화된 3,4차원 텐서는 원소 접근 계산이 어려우므로, 2차원텐서로 변경하여 계산.
	*	Convolution의 기본 원리를 사용하여 구현하면 6중 for문이 나오게 됨.
	*	@warning!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	*	이 부분에서 실수하여도, 뒤쪽에 FC가 분류를 해주기 때문에  결과적으로 잘 동작하는 것처럼 보임.
	*	올바르게 구현된 CNN은 MNIST validation 데이터에서 95% 이상이 나와야 함.
	*/
	int size = input_size - conv->k_size + 1;
	int kernel_size = conv->k_size;
	for (int kd = 0; kd < conv->depth; kd++) {														//number of feature					
		float* kernel3D = conv->kernel + (conv->k_size*conv->k_size*input_depth)*kd;
		float* output3D = conv->feature + (conv->f_size*conv->f_size)*kd;
		float* tmp = (float*)calloc(conv->f_size*conv->f_size, sizeof(float));						//임시로 output값들을 저장해두는 곳
		for (int d = 0; d < input_depth; d++) {
			float* input2D = input3D + input_size * input_size*d;
			float* kernel2D = kernel3D + conv->k_size*conv->k_size*d;
			for (int y = 0; y < size; y++) {																		//size*size 크기의 output의 한 점마다 계산을 한다.
				for (int x = 0; x < size; x++) {
					for (int ky = 0; ky < conv->k_size; ky++) {														//이때 한 점은 kernel_size * kernel_size 번의 덧셈을 통해 계산
						for (int kx = 0; kx < conv->k_size; kx++) {
							////////////////////////////////////////////////////////////////////////////////
							//
							//
							// PROBLEM 3: ADD YOUR CODE BELOW
							//
							// 주어진 실습의 Pooling layer에서는 2 x 2 filter Max Pooling을 사용합니다.
							// 이 조건을 가지고 아래의 빈칸 코드를 작성하세요.
							//
							//
							////////////////////////////////////////////////////////////////////////////////
							/* ADD YOUR CODE HERE */
							tmp[y * size + x] += input2D[(y + ky) * input_size + (x + kx)] * kernel2D[ky * kernel_size + kx];
						}
					}
				}
			}
		}
		memcpy(output3D, tmp, conv->f_size * conv->f_size * sizeof(float));
		for (int y = 0; y < size; y++) {
			for (int x = 0; x < size; x++) {
				output3D[y * size + x] = Sigmoid(output3D[y*size + x]);
			}
		}
	}
}
inline void Convolution3D_B(ConvLayer* conv, PoolLayer* pool) {
	/*
	*	@TODO
	*	컨볼루션레이어의 에러를 풀링레이어의 에러로 전파.
	*	컨볼루션 피쳐를 만드는 과정을 역으로 계산해 풀링의 에러를 계산.
	*	@tip
	*	Convolutiuon 과 마찬가지로 6중 포문.
	*/

	memset(pool->error, 0, pool->f_size*pool->f_size*pool->depth * sizeof(float));

	for (int kd = 0; kd < conv->depth; kd++) {
		float* c_error3D = conv->error + (conv->f_size*conv->f_size)*kd;
		float* kernel3D = conv->kernel + (conv->k_size*conv->k_size*pool->depth)*kd;
		for (int d = 0; d < pool->depth; d++) {
			float* p_error2D = pool->error + pool->f_size*pool->f_size*d;
			float* kernel2D = kernel3D + conv->k_size*conv->k_size*d;
			for (int y = 0; y < pool->f_size; y++) {
				for (int x = 0; x < pool->f_size; x++) {
					for (int ky = 0; ky < conv->k_size; ky++) {
						for (int kx = 0; kx < conv->k_size; kx++) {
							int x_index = x + kx - conv->k_size + 1;									//conv layer의 error에서의 x index
							int y_index = y + ky - conv->k_size + 1;									//conv layer의 error에서의 y index
							if (x_index >= 0 && x_index < conv->f_size && y_index >= 0 && y_index < conv->f_size) {	//conv layer error의 index 범위 체크
								p_error2D[(y)*pool->f_size + (x)] += kernel2D[(conv->k_size - 1 - ky)*conv->k_size + (conv->k_size - 1 - kx)] * c_error3D[y_index*conv->f_size + x_index];
							}
						}
					}
				}
			}
		}
	}
}

inline void Convolution3D_U(ConvLayer* conv, float* input3D, int input_size, int input_depth, float learning_rate) {
	/*
	*	@TODO
	*	MLP의 가중치 업데이트와 비슷함.
	*	컨볼루션 레이어의 에러, 미분된 컨볼루션의 값, 이전 레이어의 출력값이 필요함.
	*/
	for (int kd = 0; kd < conv->depth; kd++) {
		float* c_error3D = conv->error + (conv->f_size*conv->f_size)*kd;
		float* c_feature3D = conv->feature + (conv->f_size*conv->f_size)*kd;
		float* kernel3D = conv->kernel + (conv->k_size*conv->k_size*input_depth)*kd;
		for (int d = 0; d < input_depth; d++) {
			float* kernel2D = kernel3D + conv->k_size*conv->k_size*d;
			float* input2D = input3D + input_size * input_size*d;
			for (int ky = 0; ky < conv->k_size; ky++) {
				for (int kx = 0; kx < conv->k_size; kx++) {
					float m = 0.0F;
					for (int y = 0; y < conv->f_size; y++) {
						for (int x = 0; x < conv->f_size; x++) {
							m += (c_error3D[y*conv->f_size + x]) * input2D[(y + ky) * input_size + (x + kx)];
						}
					}
					kernel2D[ky * conv->k_size + kx] += learning_rate * m;	//모든 kernel의 원소에 대해서 각각이 영향을 미친 input과 convolution의 gradient를 곱한 값들을 더함
				}
			}
		}
	}
}
inline void SubSampling(ConvLayer* conv, PoolLayer* pool) {
	/*
	*	@TODO
	*	MAX-POOLING을 구현.
	*/
	for (int d = 0; d < conv->depth; d++) {
		float* input = conv->feature + conv->f_size*conv->f_size*d;
		float* output = pool->feature + pool->f_size*pool->f_size*d;
		for (int y = 0; y < conv->f_size; y += 2) {
			for (int x = 0; x < conv->f_size; x += 2) {
				float* p = &output[(y / 2)*pool->f_size + (x / 2)];
				*p = input[(y + 0)*conv->f_size + (x + 0)];
				*p = MAX(*p, input[(y + 0)*conv->f_size + (x + 1)]);
				*p = MAX(*p, input[(y + 1)*conv->f_size + (x + 0)]);
				*p = MAX(*p, input[(y + 1)*conv->f_size + (x + 1)]);
			}
		}
	}
}
inline void UpSampling(PoolLayer* pool, ConvLayer* conv) {
	/*
	*	@TODO
	*	최대 풀링의 에러를 컨볼루션 에러로 전파. 가장 큰 값의 컨볼루션 피쳐쪽만 그대로 전파하고 나머지는 0으로 전파.
	*/
	for (int d = 0; d < pool->depth; d++) {
		float* perror = pool->error + pool->f_size*pool->f_size*d;						//error 저장
		float* pfeature = pool->feature + pool->f_size*pool->f_size*d;
		float* cerror = conv->error + conv->f_size*conv->f_size*d;
		float* cfeature = conv->feature + conv->f_size*conv->f_size*d;
		for (int y = 0; y < conv->f_size; y += 2) {
			for (int x = 0; x < conv->f_size; x += 2) {
				int my = y;
				int mx = x;
				float m = -FLT_MAX;
				for (int px = 0; px < 2; px++) {
					for (int py = 0; py < 2; py++) {
						cerror[(y + py)*conv->f_size + (x + px)] = 0.0F;
						if (cfeature[(y + py)*conv->f_size + (x + px)] > m) {
							m = cfeature[(y + py)*conv->f_size + (x + px)];
							my = y + py;
							mx = x + px;
						}
					}
				}
				cerror[(my)*conv->f_size + (mx)] = (perror[(y / 2)*pool->f_size + (x / 2)]);
				cerror[(my)*conv->f_size + (mx)] *= Sigmoid_Derivative(cfeature[(my)*conv->f_size + (mx)]);
			}
		}
	}
}
inline int ForwardCNN(CNN* cnn, float* input) {
	cnn->input = input;
	int size = cnn->input_size;
	int depth = cnn->input_depth;
	for (int i = 0; i < cnn->num_of_convolution; i++) {
		Convolution3D(input, size, depth, &cnn->conv[i]);
		SubSampling(&cnn->conv[i], &cnn->pool[i]);
		input = cnn->pool[i].feature;
		size = cnn->pool[i].f_size;
		depth = cnn->pool[i].depth;
	}
	return Forward(&cnn->network, input);
}
inline void BackwardCNN(CNN* cnn, int label, float learning_rate) {
	Backward(&cnn->network, label, learning_rate);
	//FC입력의 에러는 마지막 풀링층의 에러와 같으니 복사한다.
	if (cnn->num_of_convolution > 0) {
		memcpy(cnn->pool[cnn->num_of_convolution - 1].error, cnn->network.layers[0].error, cnn->network.layers[0].n * sizeof(float));
	}
	for (int i = cnn->num_of_convolution - 1; i >= 0; i--) {
		UpSampling(&cnn->pool[i], &cnn->conv[i]);
		if (i != 0) {
			Convolution3D_B(&cnn->conv[i], &cnn->pool[i - 1]);
		}
	}
	float* input = cnn->input;
	int input_size = cnn->input_size;
	int input_depth = cnn->input_depth;
	for (int i = 0; i < cnn->num_of_convolution; i++) {
		Convolution3D_U(&cnn->conv[i], input, input_size, input_depth, learning_rate);
		input = cnn->pool[i].feature;
		input_size = cnn->pool[i].f_size;
		input_depth = cnn->pool[i].depth;
	}
}
inline void WriteParameters(CNN* cnn, char* file) {
	FILE* fp = fopen(file, "w");
	for (int i = 0; i < cnn->num_of_convolution; i++) {
		for (int j = 0; j < cnn->conv[i].k_size*cnn->conv[i].k_size*cnn->conv[i].depth; j++) {
			fprintf(fp, "%f ", cnn->conv[i].kernel[j]);
		}
		fputc('\n', fp);
	}
	for (int i = 0; i < cnn->network.depth; i++) {
		if (i != 0) {
			for (int j = 0; j < cnn->network.layers[i - 1].n * cnn->network.layers[i].n; j++) {
				fprintf(fp, "%f ", cnn->network.layers[i].w[j]);
			}
			fputc('\n', fp);
		}
	}
	fclose(fp);
}
inline void ReadParameters(CNN* cnn, char* file) {
	FILE* fp = fopen(file, "r");
	if (fp == NULL) {

		return;
	}
	for (int i = 0; i < cnn->num_of_convolution; i++) {
		for (int j = 0; j < cnn->conv[i].k_size*cnn->conv[i].k_size*cnn->conv[i].depth; j++) {
			fscanf(fp, "%f ", &cnn->conv[i].kernel[j]);
		}
	}
	for (int i = 0; i < cnn->network.depth; i++) {
		if (i != 0) {
			for (int j = 0; j < cnn->network.layers[i - 1].n * cnn->network.layers[i].n; j++) {
				fscanf(fp, "%f ", &cnn->network.layers[i].w[j]);
			}
		}
	}
	fclose(fp);
}
#endif
