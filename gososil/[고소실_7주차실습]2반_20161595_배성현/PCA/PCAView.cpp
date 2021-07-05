
// PCAView.cpp : implementation of the CPCAView class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "PCA.h"
#endif

#include "PCADoc.h"
#include "PCAView.h"
#include <stdio.h>
#include <fstream>
#include <iostream>

using namespace std;
using namespace cv;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CPCAView

IMPLEMENT_DYNCREATE(CPCAView, CView)

BEGIN_MESSAGE_MAP(CPCAView, CView)
	ON_COMMAND(ID_PROBLEM1, &CPCAView::OnProblem1)
END_MESSAGE_MAP()

// CPCAView construction/destruction

CPCAView::CPCAView()
{
	// TODO: add construction code here
	con.CreateConsole();
	// initialize data and Proj
	data = Mat(10, 200, CV_32F, 0.0f);
	Proj = Mat(20, 200, CV_32F, 0.0f);
	
}

CPCAView::~CPCAView()
{
	con.~ConsoleWnd();
}

BOOL CPCAView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CPCAView drawing

void CPCAView::OnDraw(CDC* pDC)
{
	CPCADoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
	float x, y;
	CPen Pen, Pen2;
	CPen *curPen;
	Pen.CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
	Pen2.CreatePen(PS_SOLID, 1, RGB(0, 0, 255));
	CBrush brush (RGB (0, 0, 0));
	CBrush brush2 (RGB (0, 0, 255));

	// draw x-axis
	pDC->SelectObject(&Pen);
	pDC->MoveTo(0, 250);
	pDC->LineTo(500, 250);
	pDC->MoveTo(500, 250);
	pDC->LineTo(495, 245);
	pDC->MoveTo(500, 250);
	pDC->LineTo(495, 255);
	m_cs.Format("x");
	pDC->TextOut(510,250,m_cs);

	// draw y-axis
	pDC->MoveTo(250, 0);
	pDC->LineTo(250, 500);
	pDC->MoveTo(250, 0);
	pDC->LineTo(245, 5);
	pDC->MoveTo(250, 0);
	pDC->LineTo(255, 5);
	m_cs.Format("y");
	pDC->TextOut(260,0,m_cs);

	// draw input data
	// y value increases downward on DC, but it increases upward in xy-coordinates
	// y is first flipped; both x and y are multiplied by 30 for visualization,
	// then added by 250 to be centered at (250, 250) 
	pDC->SelectObject(&Pen);
	pDC->SelectObject (&brush); // black
	for (int j=0; j<200; j++) {
		x = data.at<float>(3, j);
		y = data.at<float>(5, j);
		x = x*30+250;
		y = -y*30+250;
		pDC->Ellipse(x-2, y-2, x+2, y+2);
	}

	///////////////////////////////////////////////////////
	//
	// PROBLEM 1-d:Add YOUR CODE BELOW 
	//
	// Following the way input data is drawn above, draw the projected data with two eigenvectors
	// corresponding to the top two largest eigenvalues 
	//
	///////////////////////////////////////////////////////
	pDC->SelectObject(&Pen2);
	pDC->SelectObject (&brush2); // blue
	for (int j=0; j<200; j++) {

		// ADD YOUR CODE HERE
		x = Proj.at<float>(0, j);
		y = Proj.at<float>(1, j);
		x = x * 30 + 250;
		y = -y * 30 + 250;
		pDC->Ellipse(x - 2, y - 2, x + 2, y + 2);

	}

}


// CPCAView diagnostics

#ifdef _DEBUG
void CPCAView::AssertValid() const
{
	CView::AssertValid();
}

void CPCAView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CPCADoc* CPCAView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CPCADoc)));
	return (CPCADoc*)m_pDocument;
}
#endif //_DEBUG


// CPCAView message handlers


void CPCAView::OnProblem1()
{
	int i, j;
	CString cs5("data1.txt");
	ifstream fin;
	fin.open(cs5, ios::in);

	float val;
	float dataTemp[2000];

	for (i=0; i<2000; i++) {
		fin >> val;
		dataTemp[i] = val;
	}
	fin.close();

	// dataTemp will be copied to data, row by row
	// CV_32F: float, CV_64F: double
	// CV_32FC3 can be used for 3 channel (for R, G, and B)
	data = Mat(10, 200, CV_32F, dataTemp).clone();
	// Mat data(10, 200, CV_32F, dataTemp);  // this works too.

	//// check the input data read
	//printf("\n");
	//for (i=0; i<10; i++)
	//	printf("%f   %f\n", dataTemp[i*200+0], dataTemp[i*200+199]);
	//printf("\n");
	//for (i=0; i<10; i++)
	//	printf("%f   %f\n", data.at<float>(i, 0), data.at<float>(i, 199));

	// 0 (instead of 0.0 or 0.0f) will cause assertion failure.
	// 0 can simply be omitted
	Mat dataMean = Mat(10, 1, CV_32F, 0.0f);  

	///////////////////////////////////////////////////////
	//
	// PROBLEM 1-a:Add YOUR CODE BELOW 
	//
	// Calculate covariance matrix
	//
	///////////////////////////////////////////////////////
	// Calculate the mean vector
	// First part is given below
	float t1;
	for (i=0; i<10; i++) {
		t1 = 0;
		for (j=0; j<200; j++) {
			 t1 += data.at<float>(i, j);
		}
		dataMean.at<Vec2f>(i,0) = t1;
	}
	// ADD YOUR CODE HERE TO COMPLETE THE MEAN VECTOR CALCULATION
	for (i = 0; i < 10; i++)
	{
		dataMean.at<Vec2f>(i, 0) /= 200.f;
	}

	// Calculate U, by subtracting mean from input data
	Mat U = Mat(10, 200, CV_32F, 0.0f);
	for (i = 0; i < 10; i++) {
		for (j = 0; j < 200; j++) {
			U.at<float>(i, j)= data.at<float>(i, j)-dataMean.at<float>(i,0);
		}
	}
	// ADD YOUR CODE HERE

	// Calculate covariance matrix
	Mat Cov = Mat(10, 10, CV_32F, 0.0f);

	// ADD YOUR CODE HERE
	Cov = U * U.t();

	//// Check covariance matrix
	printf("\n");
	for (i=0; i<10; i++) {
		for (j=0; j<10; j++) {
			printf("%f ", Cov.at<float>(i,j));
		}
		printf("\n");
	}


	Mat E, V;
	///////////////////////////////////////////////////////
	//
	// PROBLEM 1-b:ADD YOUR CODE BELOW 
	//
	// Calculate eignevalue and eigenvector
	// Sort eigenvectors according to the descending order of eigenvalues
	// Notice that the eigenvalues are already sorted in descending order
	//
	///////////////////////////////////////////////////////
	// Calculate eigenvalue and eigenvector
	// E holds eigenvalues (10x1); V holds eigenvectors (10x10)
	
	// ADD YOUR CODE HERE
	eigen(Cov, E, V);

	//// check eigenvalues and eigenvectors
	//printf("\n");
	//for (i=0; i<10; i++) {
	//	printf("%f \n", E.at<float>(i,0));
	//}
	//printf("\n");
	//for (i=0; i<10; i++) {
	//	printf("%f %f %f\n", V.at<float>(i,0), V.at<float>(i,1), V.at<float>(i,2));
	//}

	///////////////////////////////////////////////////////
	//
	// PROBLEM 1-c:ADD YOUR CODE BELOW 
	//
	// Project U (not data) to eigenvector
	//
	///////////////////////////////////////////////////////
	// Project U
	Proj= V.t() * U;
	// ADD YOUR CODE HERE


	//// check projected data
	//printf("\n");
	//for (i=0; i<10; i++) {
	//	for (j=0; j<2; j++) {
	//		printf("%f ", Proj.at<float>(i,j));
	//	}
	//	printf("\n");
	//}


	Invalidate(); // refresh drawing

	// Mat data is automatically deallocated
}
