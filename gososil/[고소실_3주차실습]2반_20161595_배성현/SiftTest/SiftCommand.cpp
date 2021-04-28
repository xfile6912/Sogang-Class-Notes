
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <cmath>
#include <vector>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <opencv2/imgproc/imgproc_c.h>
#include <opencv2/highgui/highgui_c.h>
#include <opencv2/core/types_c.h>
#include <opencv2/core/core_c.h>

#include "sift.h"
#include "imgfeatures.h"
#include "utils.h"
#include <Windows.h>
#pragma warning(disable:4996)
#undef _WIN32

#define debug 0
#define PI 3.141592
#define square(a) (a)*(a)

LARGE_INTEGER Start, End, Frequency;	// variables for time measure
double TimeTaken;

/* the maximum number of keypoint NN candidates to check during BBF search */
#define KDTREE_BBF_MAX_NN_CHKS 200

/* threshold on squared ratio of distances between NN and 2nd NN */
#define NN_SQ_DIST_RATIO_THR 0.49
//#define NN_SQ_DIST_RATIO_THR 0.79


char* out_img_name = NULL;
int display = 1;
int intvls = SIFT_INTVLS;
double sigma = SIFT_SIGMA;
double contr_thr = SIFT_CONTR_THR;
int curv_thr = SIFT_CURV_THR;
int img_dbl = SIFT_IMG_DBL;
int descr_width = SIFT_DESCR_WIDTH;
int descr_hist_bins = SIFT_DESCR_HIST_BINS;

using namespace cv;
using namespace std;

void tic();
double toc();
void setPBR(int);
int Problem1();
int Problem2();
void makeSample();
int MatchAndDraw(struct feature*, struct feature*, int, int, IplImage*, int);
void my_cvSmooth(IplImage*, IplImage*, int, double);
double** make_2d_Gaussian_Kernel(int, double);
cv::Mat stack_mat_imgs(cv::Mat, cv::Mat);
IplImage* convert_to_gray32(IplImage*);
IplImage* convert_to_gray8(IplImage*);

void makeSample()
{
	Mat source, dst;
	string inFile, outFile;
	inFile = "data1/box.bmp";
	outFile = "data1/box2.bmp";

	source = imread(inFile, IMREAD_UNCHANGED);

	//cv::flip(source, dst, 2);
	cv::rotate(source, dst, 0);
	cv::resize(dst, dst, Size(223, 343));

	imwrite(outFile, dst);
}

int main()
{	
	Problem1();				// Gaussian Filtering 
	//Problem2();					// Sift Descriptor Matching

	return 0;
}

int Problem1()
{
	setPBR(1);  
	string inFile3;
	IplImage* Image3, *Image4;
	IplImage* Image3_gray32, *Image4_gray32, *Image3_cvSmoothed, * Image4_my_cvSmoothed;
	Mat Image3_mat, Image4_mat;
	IplImage copy3, copy4;
	IplImage* Image_origin;
	Mat Image_origin_mat;
	Mat Image3_dst_mat, Image4_dst_mat;

	double k_min, k_max;

	int kernel_size = 9;
	sigma = 1.6;


	inFile3 = "data1/box.bmp";
	printf("\n========>Start cvSmooth\n\n");
	
	Image3_mat = imread(inFile3, IMREAD_UNCHANGED);
	copy3 = cvIplImage(Image3_mat);
	Image_origin = &copy3;

	Image3 = cvCloneImage(Image_origin);
	Image3_gray32 = convert_to_gray32(Image3);  // 0~255 grayscale color(8bit) -> 0~1 normalize(32bit)

	/*	cvSmooth */
	Image3_cvSmoothed = cvCloneImage(Image3_gray32);
	cvSmooth(Image3_gray32, Image3_cvSmoothed, CV_GAUSSIAN, kernel_size, kernel_size, sigma, sigma);

	Mat kx = getGaussianKernel(kernel_size, sigma);
	Mat ky = getGaussianKernel(kernel_size, sigma);
	Mat _2d_Gaussian_Kernel = ky * kx.t();
	minMaxIdx(_2d_Gaussian_Kernel, &k_min, &k_max);
	printf("cvGaussianKernel[k_size = %d, sigma = %f] min : %lf, max : %lf\n", kernel_size, sigma, k_min, k_max);
	//cout << _2d_Gaussian_Kernel << endl;

	Image3_dst_mat = cvarrToMat(Image3_cvSmoothed);
	Image_origin_mat = cvarrToMat(Image_origin);

	cv::imshow("original_data", Image_origin_mat);
	cv::imshow("cvSmooth_output", Image3_dst_mat);

	printf("\n\n========>Complete cvSmooth\n\n");
	printf("\n========>Start my_cvSmooth\n\n");

	Image4 = cvCloneImage(Image_origin);
	Image4_gray32 = convert_to_gray32(Image4);   // 0~255 grayscale color(8bit) -> 0~1 normalize(32bit)
	
	/*	my_cvSmooth */
	Image4_my_cvSmoothed = cvCloneImage(Image4_gray32);
  	my_cvSmooth(Image4_gray32, Image4_my_cvSmoothed, kernel_size, sigma);
	Image4_dst_mat = cvarrToMat(Image4_my_cvSmoothed);

	cv::imshow("my_csSmooth_output", Image4_dst_mat);

	printf("\n\n========>Complete my_cvSmooth\n\n");
	printf("\n========>Start cvSub\n\n");
	
	IplImage* sub_gray32;
	Mat sub_gray32_mat ;
	sub_gray32 = cvCreateImage(cvGetSize(Image3_gray32),IPL_DEPTH_32F, 1);


	/*	cvSmooth - my_cvSmooth */
	cvSub(Image3_cvSmoothed, Image4_my_cvSmoothed, sub_gray32);
	sub_gray32_mat = cvarrToMat(sub_gray32);

	double min = 0, max = 0;
	minMaxLoc(sub_gray32_mat, &min, &max);
	printf("sub_matrix min: %lf max: %lf\n", min, max);
	cv::imshow("The diff between cvSmooth and my_cvSmooth", sub_gray32_mat);
	printf("\n\n========>Complete cvSub\n\n");

	cv::waitKey(0);


	return 0;
}

int Problem2()
{
	setPBR(2);
	int n1, n2, sco, i, j;
	string inFile1, inFile2;
	IplImage* Image1, *Image2, *stacked;
	Mat Image1_mat, Image2_mat, stacked_mat;
	struct feature* feat1, *feat2;
	IplImage copy;
	IplImage copy2, copy3;


	// inFilename : box.bmp,box2.bmp,lena24.bmp
	inFile1 = "data1/box.bmp";
	inFile2 = "data1/box2.bmp";

	Image1_mat = imread(inFile1, IMREAD_UNCHANGED);
	copy = cvIplImage(Image1_mat);
	Image1 = &copy;

	printf("==> Start Image1 Feature Extraction \n\n");
	n1 = _sift_features(Image1, &feat1, intvls, sigma, contr_thr, curv_thr,
		img_dbl, descr_width, descr_hist_bins);

	printf("---- #feat 1 = %d ----- \n", n1);

	printf("\n==> Complete Image1 Feature Extraction \n\n");
	Image2_mat = imread(inFile2, IMREAD_UNCHANGED);
	copy2 = cvIplImage(Image2_mat);
	Image2 = &copy2;

	if (!Image2) {
		fprintf(stderr, "Error in loading image...\n");
		cvWaitKey(0);
	}
	printf("==> Start Image2 Feature Extraction Start\n\n");
	n2 = _sift_features(Image2, &feat2, intvls, sigma, contr_thr, curv_thr,
		img_dbl, descr_width, descr_hist_bins);

	printf("---- #feat 2 = %d ----- \n", n2);

	printf("\n==> Complete Image1 Feature Extraction \n\n");

	if (!Image1) {
		fprintf(stderr, "Error in loading image...\n");
		cvWaitKey(0);
	}

	if (Image1_mat.channels() == 3) {
		Mat Image1_mat_copy;
		Image1_mat.copyTo(Image1_mat_copy);
		cvtColor(Image1_mat_copy, Image1_mat, COLOR_RGB2GRAY);
	}
	if (Image2_mat.channels() == 3) {
		Mat Image2_mat_copy;
		Image2_mat.copyTo(Image2_mat_copy);
		cvtColor(Image2_mat_copy, Image2_mat, COLOR_RGB2GRAY);
	}

	stacked_mat = stack_mat_imgs(Image1_mat, Image2_mat);
	copy3 = cvIplImage(stacked_mat);
	stacked = &copy3;

	tic();
	printf("==> Start Image1 & 2 Keypoint Matching\n\n");
	int m = MatchAndDraw(feat1, feat2, n1, n2, stacked, Image1->width);

	double time = toc();
	printf("Time taken: %f sec\n", time);

	fprintf(stderr, "Found %d total matches\n\n", m);
	printf("==> Complete Image1 & 2 Keypoint Matching\n\n");
	Mat Matche1;
	Matche1 = cvarrToMat(stacked);
	cv::imshow("Matche1", Matche1);
	cv::waitKey(0);


	return 0;
}


int MatchAndDraw(struct feature* feat1, struct feature* feat2, int n1, int n2, IplImage* stacked, int width)
{
	int i, j, k, d, m, min_idx;
	double d0, d1, d2;
	CvPoint pt1, pt2;
	struct feature* f1, *f2;
	double* descr1, *descr2, dist;
	//feat1, 2는 각 특징점배열의 첫 주소를 포인팅
	//f1과 f2는 하나의 특징점을 나타냄.
	m = 0;
	min_idx = 0;
	if (feat1)
		d = feat1->d; // dimension of the descriptor (fixed to 128)
	for (i = 0; i < n1; i++) {
		f1 = feat1 + i;
		descr1 = f1->descr;
		//descr1과 descre2는 어떠한 한 특징점의 128차원의 vector가 들어가 있음
		d1 = d2 = DBL_MAX;//거리 d1은 가장 짧은거리, d2는 가장 긴거리
	
		
		
		////////////////////////////////////////////////////////////////////////////////

		// PROBLEM 2 : Calculate the similarity between the decriptors of the image SIFT feature.
		// Using L2 Distance, the decriptor in f1 can be accessed by f1->descr.
		// The decriptor is a 1D array with 128 elements.

		for (j = 0; j < n2; j++)
		{
			f2 = feat2 + j;
			descr2 = f2->descr;
			//descr1과 descre2는 어떠한 한 특징점의 128차원의 vector가 들어가 있음
			dist = 0;
			for (k = 0; k < d; k++)//0~128 //descr는 128차원임.128번 for문 돌기
			{
				dist += (descr1[k] - descr2[k]) * (descr1[k] - descr2[k]);

			}
			dist = sqrt(dist);//128차원의 벡터 사이의 거리를 구해줘야함.
			if (dist < d1)
			{
				d2 = d1;//원래 가장짧은 점이라고 생각했던 것을 두번째 짧은점으로 옮겨줌.
				d1 = dist;
				min_idx = j;//이미지2의 가장 거리가 짧은 특징점의 인덱스
			}
			else if (dist < d2)//두번째로 짧은 경우
			{
				d2 = dist;
			}
		}
		////////////////////////////////////////////////////////////////////////////////



		if (d2 < DBL_MAX && d1 < d2 * NN_SQ_DIST_RATIO_THR) {
			f2 = feat2 + min_idx;
			pt1 = cvPoint(cvRound(f1->x), cvRound(f1->y));
			pt2 = cvPoint(cvRound(f2->x), cvRound(f2->y));
			pt2.x += width;
			cvLine(stacked, pt1, pt2, cvScalar(255, 0, 255), 1, 8, 0); // draw a line
			m++;
		}
	}

	return m;
}
///////////////////////////////////////////////////////
/********************************************************************************/
//	tic
//
//	Measure the starting time
/********************************************************************************/
void tic()		// Starting point of measuring time
{
	QueryPerformanceCounter(&Start);
}

/********************************************************************************/
//	toc
//
//	Measure the finishing time
/********************************************************************************/
double toc()	// Finishing point of measuring time
{
	QueryPerformanceCounter(&End);
	QueryPerformanceFrequency(&Frequency);

	TimeTaken = (double)(End.QuadPart - Start.QuadPart) / Frequency.QuadPart;	// seconds

	return TimeTaken;
}

