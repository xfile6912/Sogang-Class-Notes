#include <opencv2/opencv.hpp>
#include <iostream>
#include <stdio.h>

using namespace cv;
using namespace std;

int main(int, char**)
{
	Mat img1 = imread("lena_8.bmp", IMREAD_UNCHANGED);
	printf("width: %d, height: %d\n", img1.cols, img1.rows);


	printf("widthStep: %zd\n", img1.step1());
	printf("nChannels: %d\n", img1.channels());
	printf("depth: %zd\n\n", img1.elemSize());

	namedWindow("problem1_lena_8", WINDOW_AUTOSIZE);
	imshow("problem1_lena_8", img1);

	double alpha, beta;
	//Mat A(30, 40, DataType<unsigned char>::type);


	Mat img1_Mat_half, img1_Mat_double;

	alpha = 0.5;
	beta = 0;

	/////////////////////////////////////////////////////////////////

	// PROBLEM 1.1 USE addWeighted(...) FUNCTION TO REDUCE PIXEL VALUES BY HALF
	addWeighted(img1, alpha, img1, beta, 0, img1_Mat_half);
	/////////////////////////////////////////////////////////////////


	alpha = 2;

	/////////////////////////////////////////////////////////////////

	// PROBLEM 1.2 USE addWeighted(...) FUNCTION TO INCREASE PIXEL VALUES TWICE
	addWeighted(img1, alpha, img1,beta, 0, img1_Mat_double);
	/////////////////////////////////////////////////////////////////


	namedWindow("problem1_lena_8_half", WINDOW_AUTOSIZE);
	imshow("problem1_lena_8_half", img1_Mat_half);
	namedWindow("problem1_lena_8_double", WINDOW_AUTOSIZE);
	imshow("problem1_lena_8_double", img1_Mat_double);

	imwrite("problem1_lena_8_half.bmp", img1_Mat_half);
	imwrite("problem1_lena_8_double.bmp", img1_Mat_double);


	Mat img2 = imread("lena_24.bmp", IMREAD_UNCHANGED);
	Mat img2_Mat_gray;
	printf("width: %d, height: %d\n", img2.cols, img2.rows);
	printf("widthStep: %zd\n", img2.step1());
	printf("nChannels: %d\n", img2.channels());
	printf("depth: %zd\n\n", img2.elemSize());

	namedWindow("problem2_lena_24", WINDOW_AUTOSIZE);
	imshow("problem2_lena_24", img2);


	/////////////////////////////////////////////////////////////////

	// PROBLEM 2 USE cvtColor(...) FUNCTION TO CONVERT 24 BIT COLOR IMAGE TO GRAYSCALE
	// USE imshow OR cvShowImage TO DISPLAY THE IMAGE
	// USE imwrite TO SAVE THE IMAGE
	cvtColor(img2, img2_Mat_gray, COLOR_BGR2GRAY);
	namedWindow("problem2_lena_24_gray", WINDOW_AUTOSIZE);
	imshow("problem2_lena_24_gray", img2_Mat_gray);
	imwrite("problem2_lena_24_gray.bmp", img2_Mat_gray);
	/////////////////////////////////////////////////////////////////


	Mat img3 = imread("image_16.bmp", IMREAD_UNCHANGED);
	Mat img3_Mat_gray;
	printf("width: %d, height: %d\n", img3.cols, img3.rows);
	printf("widthStep: %zd\n", img3.step1());
	printf("nChannels: %d\n", img3.channels());
	printf("depth: %zd\n\n", img3.elemSize());

	namedWindow("problem3_image_16", WINDOW_AUTOSIZE);
	imshow("problem3_image_16", img3);

	/////////////////////////////////////////////////////////////////

	// PROBLEM 3 USE cvtColor(...) FUNCTION TO CONVERT 16 BIT COLOR IMAGE TO GRAYSCALE
	// USE imshow OR cvShowImage TO DISPLAY THE IMAGE
	// USE imwrite TO SAVE THE IMAGE
	cvtColor(img3, img3_Mat_gray, COLOR_BGR2GRAY);
	namedWindow("problem3_image_16_gray", WINDOW_AUTOSIZE);
	imshow("problem3_image_16_gray", img3_Mat_gray);
	imwrite("problem3_image_16_gray.bmp", img3_Mat_gray);
	/////////////////////////////////////////////////////////////////


	printf("OpenCV regards the \"depth\" as depth for each channel.\n");
	printf("The \"depth\" information in OpenCV seems little wrong for 16 bit image, but it does the color conversion correctly. This is why sometimes it is the best choice to use the pixel value accessing functions provided by the tool under use.\n");


	waitKey(0);
	destroyWindow("problem1_lena_8");
	destroyWindow("problem1_lena_half");
	destroyWindow("problem1_lena_double");
	img1.release();
	img1_Mat_half.release();
	img1_Mat_double.release();



	destroyWindow("problem2_lena_24");
	destroyWindow("problem2_lena_24_gray");
	img2.release();
	img2_Mat_gray.release();


	destroyWindow("problem3_image_16");
	destroyWindow("problem3_lena_16_gray");
	img3.release();
	img3_Mat_gray.release();

}