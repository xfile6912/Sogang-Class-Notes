#ifndef MLP_H
#define MLP_H
#include<stdio.h>
#include<stdlib.h>
#include<float.h>
#include<math.h>
//#include<openblas/cblas.h>		//openblas는 선택입니다. cblas_sgemm을 사용할수 없다면 아래의 my_sgemm을 사용할 수 있다.
#ifndef CBLAS_H
#define CblasRowMajor 0
#define CblasNoTrans 111
#define CblasTrans 112	//112는 openblas의 CblasTrans의 상수
#endif
#ifndef MAX
#  define MAX(a,b)  ((a) < (b) ? (b) : (a))
#endif
#ifndef MIN
#  define MIN(a,b)  ((a) > (b) ? (b) : (a))
#endif
#ifndef __cplusplus
typedef struct FCLayer FCLayer;
typedef struct Network Network;
#endif
struct FCLayer {		// Fully connected layer
	int n;				// 뉴런의 개수
	float* w;			// 가중치 , [이전레이어의 크기] x [현재레이어의 크기] 의 2차원 행렬                         (실제로는 1차원)
	float* neuron;	    // 뉴런
	float* error;		// 오차
};
struct Network {
	FCLayer* layers;	//레이어들의 배열
	int depth;			//레이어의 개수
};
inline Network CreateNetwork(int* size_of_layers, int num_of_layers) {
	/*
	*	@TODO
	*	1. num_of_layers 만큼 레이어를 생성
	*	2. Network::depth는 num_of_layers와 같다
	*	3. 각 Layer마다 뉴런의 개수를 size_of_layers 배열을 이용해 설정
	*	4. 각 Layer의 error와 neuron을 할당
	*	5. Layer의 w를 이전레이어의크기x현재레이어의크기 의 2차원 배열로 할당
	*	6. 할당한 w를 [-1,1]의 실수 난수 값으로 초기화
	*/
	Network network;
	network.layers = (FCLayer*)calloc(num_of_layers, sizeof(FCLayer));
	network.depth = num_of_layers;
	for (int i = 0; i < num_of_layers; i++) {
		network.layers[i].n = size_of_layers[i];
		network.layers[i].error = (float*)calloc(size_of_layers[i], sizeof(float));
		if (i != 0) {	//첫번째 레이어는 가중치와 뉴런값이 없음.
			network.layers[i].w = (float*)calloc(size_of_layers[i - 1] * size_of_layers[i], sizeof(float));
			network.layers[i].neuron = (float*)calloc(size_of_layers[i], sizeof(float));
			for (int j = 0; j < size_of_layers[i - 1] * size_of_layers[i]; j++) {
				network.layers[i].w[j] = (float)rand() / RAND_MAX * 2 - 1.0F;	//[-1,1] 로 초기화
			}
		}
	}
	return network;
}
/*
M*K행렬 A와 K*N행렬 B를 곱해서 AB (M*N)을 만들어 놓고, 최종적으로 C에다가 alpha*AB + beta*C값을 덮어 씌우는 모양입니다.
나머지 변수들에 대해선 아래에 자세히 설명이 되어 있습니다.
*/
inline void my_sgemm(int major, int transA, int transB, int M, int N, int K, float alpha, float* A, int lda, float* B, int ldb, float beta, float* C, int ldc) {
	//aAB+bC=C
	for (int m = 0; m < M; m++) {
		for (int n = 0; n < N; n++) {
			float c = C[m*ldc + n];
			C[m*ldc + n] = 0.0F;
			for (int k = 0; k < K; k++) {
				float a, b;
				a = transA == CblasTrans ? A[k*lda + m] : A[m*lda + k];
				b = transB == CblasTrans ? B[n*ldb + k] : B[k*ldb + n];
				C[m*ldc + n] += a*b;
			}
			C[m*ldc + n] = alpha*C[m*ldc + n] + beta*c;
		}
	}
}
 
//딥러닝에서는 ReLU를 많이 사용하나, 
//간단한 실습이니 Sigmoid를 사용
inline float Sigmoid(float x) {
	return 1.0F / (1.0F + expf(-x));
}
inline float Sigmoid_Derivative(float x) {
	return x*(1.0F - x);
}
inline int Forward(Network* network, float* input) {
	/*
	*	@TODO
	*	1. 첫번째 레이어의 neuron 값은 input 
	*	2. 순방향으로 레이어를 순회하면서 Neuron의 값들을 계산
	*	3. 현재 레이어의 뉴런의 값은 이전 레이어의 뉴런과 가중치의 행렬곱으로 표기할 수 있다
	*	@example
	*	[100][784] 행렬인 W와 [784][1] 행렬인 이전 뉴런값을 행렬곱하여 [100][1]인 현재 뉴런의 값을 계산할 수 있다.
	*	@gemm references
	*		gemm은 A,B,C가 행렬 a,b가 스칼라 값일때 aAB+bC를 계산합니다. 즉 A,B,C가 모두 입력 행렬 이고 C는 출력 행렬
	*		major : 행렬이 RowMajor인지 ColMajor 인지 결정합니다. C언어는 RowMajor 이므로 CblasRowMajor를 입력한다.
	*		transA : A행렬을 전치행렬로 사용할 것인지, 전치행렬이면 CblasTrans, 그대로 사용할 것이면 CblasNoTrans를 입력 한다
	*		transB : B행렬을 전치행렬로 사용할 것인지
	*		M,N,K : [M][K] 크기의 행렬과 [K][N]의 행렬 을 곱해 [M][N]의 C행렬을 생성
	*		alpha : aAB+bC 를 계산할때 곱해지는 a(스칼라) 값입니다. 단순히 AB 를 계산하고 싶으면 alpha=1.0, beta를 0.0 으로 입력
	*		A : 입력행렬 A
	*		lda : A행렬의 2번째 차원의 값. 5x4을 1차원으로 표기한 행렬의 i,j 접근은 A[i*4+j] 로 표기한다. 이때 4가 lda 
	*		B : 입력행렬 B
	*		ldb : B행렬의 2번째 차원의 값
	*		beta : b의 값
	*		C : 입출력 행렬 C
	*		ldc : C행렬의 2번째 차원의 값
	*/

	network->layers[0].neuron = input;					
	for (int i = 1; i < network->depth; i++) {
		//현재 레이어의 뉴런의 값은 이전 레이어의 뉴런과 가중치의 계산으로 나온다. aAB+bC
		my_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans
					, network->layers[i].n      // M
					, 1                         // N
					, network->layers[i - 1].n  // K
					, 1.0F    // alpha
					, network->layers[i].w, network->layers[i - 1].n    // A, lda
					, network->layers[i - 1].neuron, 1      // B, ldb
					, 0.0F    // beta
					, network->layers[i].neuron, 1);        // C, ldc
		for (int j = 0; j < network->layers[i].n; j++) {
			network->layers[i].neuron[j] = Sigmoid(network->layers[i].neuron[j]);
		}
	}
	////////////////////////////////////////////////////////////////////////////////
	//
	//
	// PROBLEM 1: ADD YOUR CODE BELOW
	//
	// 최종층 10개의 뉴런 중 가장 큰 값의 인덱스를 찾아 그 인덱스를 리턴한다.
	// 해당 인덱스가 MLP의 prediction 이다.
	//
	// 아래의 코드를 완성시키시면 됩니다. 
	//
	//
	////////////////////////////////////////////////////////////////////////////////
	int a = 0;
	int i;
	float max_value = network->layers [ network->depth - 1 ].neuron [ 0 ];
	for (i = 0; i < network->layers[network->depth - 1].n; i++)
	{
		if (network->layers[network->depth - 1].neuron[i] > max_value)
		{
			a = i;
			max_value = network->layers[network->depth - 1].neuron[i];
		}
	}
	// ADD YOUR CODE HERE

	return a;
}
inline void Backward(Network* network, int label, float learning_rate) {
	/*
	*	@TODO
	*	error와 Gradient를 계산
	*	최종층에서의 에러는 one-hot vector화 된 결과와 출력 결과를 뺸 값
	*/
	//Calculate last layer's error
	for ( int i = 0; i < network->layers [ network->depth - 1 ].n; i++ ) {
		network->layers [ network->depth - 1 ].error [ i ] = 0.0F - network->layers [ network->depth - 1 ].neuron [ i ];
	}
	network->layers [ network->depth - 1 ].error [ label ] = 1.0F - network->layers [ network->depth - 1 ].neuron [ label ];
	/*
	*	@TODO
	*	역으로 순회하면서 각 레이어의 오차를 계산한다.
	*	이전 레이어의 오차는 현재 레이어의 가중치(전치)와, 현재 레이어의 오차를 이용해 계산한다.
	*	@example
	*	[100][10] 의 전치된 가중치와 [10][1]의 오차를 행렬곱 하여 [100][1] 의 이전 레이어의 오차를 계산할 수 있다.
	*	@other
	*	사실 입력층의 오차는 계산할 필요가 없습니다. 다만 추후에 Convolution을 앞에 붙이면 입력층의 오차가 필요하다.
	*	따라서 입력층의 오차도 미리 계산해 둔다.
	*/
	////////////////////////////////////////////////////////////////////////////////
	//
	//
	// PROBLEM 2: ADD YOUR CODE BELOW
	//
	// Output layer부터 역으로 순회하면서 각각의 레이어의 오차를 계산합니다.
	//
	// Hint:
	// 이전 레이어Y(1)의 오차 = Σ(Z(i)의 오차 * W(i,1))
	//
	// 아래의 NULL 부분 코드를 완성시키시면 됩니다. 
	//
	//
	////////////////////////////////////////////////////////////////////////////////

	//Calculate other layer's error
	for ( int i = network->depth - 1; i > 0; i-- ) {
		my_sgemm(CblasRowMajor, CblasTrans, CblasNoTrans  //ORDER
					, network->layers [ i - 1 ].n, 1, network->layers [ i ].n // M N K
					, 1.0F													 // alpha
					, network->layers[i].w/*ADD YOUR CODE HERE*/ , network->layers [i - 1].n		// A, lda
					, network->layers[i].error/*ADD YOUR CODE HERE*/, 1								// B, ldb
					, 0.0F													// beta
					, network->layers [ i - 1 ].error, 1);					// C, ldc
	}
	/*
	*	@TODO
	*	가중치를 갱신하기 위해서는 Gradient와, 이전 레이어의 출력값이 필요하다.
	*	Gradient를 계산하기 위해서는 현재 레이어의 오차와, 현재 레이어의 뉴런값의 미분값이 필요하다.
	*	단순히 오차와 뉴런의 미분값을 곱하여(스칼라곱) Gradient를 계산할 수 있다.
	*	@Warning
	*	Gradient를 계산할때 오차를 저장하는 변수 error에 덮어쓰기 하지 마시오.
	*	(Convolution에 역전파할때도 마찬가지로 오차만을 전파하기 위함)
	*	@hint
	*	계산된 Gradient 역시 [10][1] 행렬 
	*	[10][1] Gradient와 [1][100]의 이전레이어의 뉴런값을 행렬곱하여 [10][100]의 업데이트될 가중치를 계산할 수 있다.
	*	행렬곱으로 나온 [10][100]에 learning_rate를 곱해 기존 Weight에 더하면 그 것이 가중치의 갱신이다.
	*	@tip
	*	gemm의 alpha와 beta를 사용하여 한번에 learning_rate를 곱해서 더하는 효과를 볼 수 있다.
	*/

	////////////////////////////////////////////////////////////////////////////////
	//
	//
	// PROBLEM 3: ADD YOUR CODE BELOW
	//
	// 모든 가중치 W(a,b)에 대해서 update를 진행합니다.
	//
	// Hint:
	// 새로운 W(a,b) = learning rate * gradient(a) * neuron(b) + 기존 W(a,b)
	// 정상적으로 코드가 진행된다면 W(1,1), W(1,2), W(1,3) .... W(2,1), W(2,2), W(2,3) .... W(10,100) 순서로 update 됩니다.
	// 아래의 NULL 부분 코드를 완성시키시면 됩니다. 
	//
	//
	////////////////////////////////////////////////////////////////////////////////

	//Update weights
	for ( int i = network->depth - 1; i >= 1; i-- ) {
		float* Gradient = ( float* ) calloc(network->layers [ i ].n, sizeof(float));
		for ( int j = 0; j < network->layers [ i ].n; j++ ) {
			//에러와 미분값을 곱하여(스칼라곱) gradient 값을 계산
			Gradient[j] =network->layers[i].error[j]*Sigmoid_Derivative(network->layers[i].neuron[j])/*ADD YOUR CODE HERE*/;
		}
		my_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans
					, network->layers [ i ].n, network->layers [ i - 1 ].n, 1		// M N K
					,learning_rate/*ADD YOUR CODE HERE*/									// alpha
					, Gradient/*ADD YOUR CODE HERE*/, 1									// A, lda
					, network->layers[i-1].neuron/*ADD YOUR CODE HERE*/, 1									// B, ldb
					, 1.0F															// beta
					, network->layers [ i ].w, network->layers [ i - 1 ].n);        // C, ldc
		free(Gradient);
	}
}

#endif