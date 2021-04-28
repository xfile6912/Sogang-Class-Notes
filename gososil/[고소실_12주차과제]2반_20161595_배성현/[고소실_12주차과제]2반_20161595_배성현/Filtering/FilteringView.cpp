
// FilteringView.cpp : implementation of the CFilteringView class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "Filtering.h"
#endif

#include "FilteringDoc.h"
#include "FilteringView.h"
#include <cmath>
#include "Sorting_q.h"
#include "cuda_code.cuh"

#define square(a) (a)*(a)

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CFilteringView

IMPLEMENT_DYNCREATE(CFilteringView, CView)

BEGIN_MESSAGE_MAP(CFilteringView, CView)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_COMMAND(ID_PROBLEM1_B1, &CFilteringView::OnProblem1B1)
	ON_COMMAND(ID_PROBLEM1_B2, &CFilteringView::OnProblem1B2)
    ON_COMMAND(ID_PROBLEM1_B3, &CFilteringView::OnProblem1B3)
	ON_COMMAND(ID_PROBLEM1_B4, &CFilteringView::OnProblem1B4)
END_MESSAGE_MAP()

// CFilteringView construction/destruction

CFilteringView::CFilteringView()
{
	// TODO: add construction code here
	m_width = 0;
	m_height = 0;

}

CFilteringView::~CFilteringView()
{
	m_mfcImage1->Destroy();
}

BOOL CFilteringView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CFilteringView drawing

void CFilteringView::OnDraw(CDC* pDC)
{
	CFilteringDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
	//if (!m_mfcImage1->IsNull() && m_width > 0)
	if (m_width > 0) {
		m_mfcImage1->Draw(pDC->GetSafeHdc(), 0, 30);  // Display image at x, y coordinates

		m_cs.Format("Filtering time: %5.2f  (milliseconds)", m_result); // print filtering time
		pDC->TextOut(0,10,m_cs);
	}
}

void CFilteringView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CFilteringView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CFilteringView diagnostics

#ifdef _DEBUG
void CFilteringView::AssertValid() const
{
	CView::AssertValid();
}

void CFilteringView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CFilteringDoc* CFilteringView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CFilteringDoc)));
	return (CFilteringDoc*)m_pDocument;
}
#endif //_DEBUG


// CFilteringView message handlers
void CFilteringView::OnProblem1B1()
{
	GaussianFilteringGPU(NO_SHARED);
}


void CFilteringView::OnProblem1B2()
{
	GaussianFilteringGPU(SHARED);
}

// CPU 버전 구현
void CFilteringView::OnProblem1B3()
{
    GaussianFilteringCPU();
}

void CFilteringView::OnProblem1B4()
{
	MovingAverageFiltering(9); // 9x9 kernel
}

void CFilteringView::GaussianFilteringGPU(int flag){
	m_mfcImage1 = new CImage();
	int r, c;
	m_result = -1;

	// We load a 24 bit image, with R, G, B channels.
	// Since all R, G, B have the same value, this is effectively a grayscale image.
	// Loading and processing an 8 bit image may have some problem because of the pixel alignment
	// - image width and bit width may not be aligned correctly. This is because the image width
	//   needs to be a multiple of 4 bytes but the bit representation does not have this restriction.
	// We will not handle this issue in detail in this lab. Most of the images we will handle will be
	// in 24 bit format in practice.
	m_mfcImage1->Load("SaltPepper_24bit.bmp");

	LONG width1 = m_mfcImage1->GetWidth();
	LONG height1 = m_mfcImage1->GetHeight();
	WORD bitcount1 = m_mfcImage1->GetBPP();
	int pitch1 = m_mfcImage1->GetPitch();

	m_width = width1;
	m_height = height1;

	COLORREF col;
	unsigned char gray;
	/*unsigned char *imageBits = new unsigned char[m_width*m_height];
	unsigned char *imageBitsFiltered = new unsigned char[m_width*m_height];*/
	unsigned char *imageBits = new unsigned char[m_width*m_height];
	unsigned char *imageBitsFiltered = new unsigned char[m_width*m_height];
	int max_gray = 0;
	int max_d = 0;

	// take R value only because R, G, B have the same value
	for (r=0; r<m_height; r++) {
		for (c=0; c<m_width; c++) {
			col = m_mfcImage1->GetPixel(c,r);
			imageBits[r*m_width+c] = GetRValue(col); 
			// GetGValue() and GetBValue() to take the multi-channel values
		}
	}

	int k, l;
	double temp, mean;
	

	QueryPerformanceFrequency(&m_frequency);
	QueryPerformanceCounter(&m_start);

	// Perform average filtering

	float time = Do_Gaussian_on_GPU(imageBits, imageBitsFiltered, m_width, m_height, flag);
	
	
	QueryPerformanceCounter(&m_end);
	m_result = time;//1000*(m_end.QuadPart-m_start.QuadPart)/m_frequency.QuadPart;	

	// Assign the same value into R, G, B channels.
	int test = 0;
	for (r=0; r<m_height; r++) {
		for (c=0; c<m_width; c++) {
			//m_mfcImage1->SetPixel(c,r,RGB(imageBits[r*m_width+c],imageBits[r*m_width+c],imageBits[r*m_width+c]));
			temp = imageBitsFiltered[r*m_width+c];
			if(temp == 0) test++;
			m_mfcImage1->SetPixel(c,r,RGB(temp,temp,temp));
		}
	}

	Invalidate();
	delete imageBits, imageBitsFiltered;
}

void CFilteringView::GaussianFilteringCPU() {   
	m_mfcImage1 = new CImage();
	int r, c;
	m_result = -1;

	// We load a 24 bit image, with R, G, B channels.
	// Since all R, G, B have the same value, this is effectively a grayscale image.
	// Loading and processing an 8 bit image may have some problem because of the pixel alignment
	// - image width and bit width may not be aligned correctly. This is because the image width
	//   needs to be a multiple of 4 bytes but the bit representation does not have this restriction.
	// We will not handle this issue in detail in this lab. Most of the images we will handle will be
	// in 24 bit format in practice.
	m_mfcImage1->Load("SaltPepper_24bit.bmp");

	LONG width1 = m_mfcImage1->GetWidth();
	LONG height1 = m_mfcImage1->GetHeight();
	WORD bitcount1 = m_mfcImage1->GetBPP();
	int pitch1 = m_mfcImage1->GetPitch();

	m_width = width1;
	m_height = height1;


	COLORREF col;
	unsigned char gray;
	m_imageBits = new unsigned char[m_width * m_height];
	m_imageBitsFiltered = new unsigned char[m_width * m_height];
	int max_gray = 0;
	int max_d = 0;

	// take R value only because R, G, B have the same value
	for (r = 0; r < m_height; r++) {
		for (c = 0; c < m_width; c++) {
			col = m_mfcImage1->GetPixel(c, r);
			m_imageBits[r * m_width + c] = GetRValue(col);
			// GetGValue() and GetBValue() to take the multi-channel values
		}
	}
	float _1 = 1.0f / 256.0f;
	float _4 = _1 * 4;
	float _6 = _1 * 6;
	float _16 = _1 * 16;
	float _24 = _1 * 24;
	float _36 = _1 * 36;

	float* kernel = new float[25];

	kernel[0] = kernel[4] = kernel[20] = kernel[24] = _1;
	kernel[1] = kernel[3] = kernel[5] = kernel[9] = _4;
	kernel[15] = kernel[19] = kernel[21] = kernel[23] = _4;
	kernel[2] = kernel[10] = kernel[14] = kernel[22] = _6;
	kernel[6] = kernel[8] = kernel[16] = kernel[18] = _16;
	kernel[7] = kernel[11] = kernel[13] = kernel[17] = _24;
	kernel[12] = _36;

	int w = 2;
	float temp;
	QueryPerformanceFrequency(&m_frequency);
	QueryPerformanceCounter(&m_start);

	// Perform average filtering 적용
	for (r = 0; r < m_height; r++) {
		for (c = 0; c < m_width; c++) {
			m_imageBitsFiltered[r * m_width + c] = 0;
			for (int i = -w; i <= w; i++)
			{
				for (int j = -w; j <= w; j++)
				{
					if (r + i >= 0 && c + j >= 0 && r + i < m_height && c + j < m_width)//범위안에 있을 때
					{
						m_imageBitsFiltered[r * m_width + c] += kernel[(i+w)*5+j+w] * m_imageBits[(r + i) * m_width + (c + j)];//i+w는 0<i+w<w0, j+w는 0<j+w<w0
					}
					else//범위 밖에 있을때
					{
						m_imageBitsFiltered[r * m_width + c] += 0;
					}
				}
			}

		}
	}
	QueryPerformanceCounter(&m_end);
	m_result = 1000 * (m_end.QuadPart - m_start.QuadPart) / m_frequency.QuadPart;

	// Assign the same value into R, G, B channels.
	for (r = 0; r < m_height; r++) {
		for (c = 0; c < m_width; c++) {
			//m_mfcImage1->SetPixel(c,r,RGB(imageBits[r*m_width+c],imageBits[r*m_width+c],imageBits[r*m_width+c]));
			temp = m_imageBitsFiltered[r * m_width + c];
			m_mfcImage1->SetPixel(c, r, RGB(temp, temp, temp));
		}
	}

	Invalidate();
	delete m_imageBits, m_imageBitsFiltered;
	delete[] kernel;
}

void CFilteringView::MovingAverageFiltering(int w0)
{
	m_mfcImage1 = new CImage();
	int r, c;
	m_result = -1;

	// We load a 24 bit image, with R, G, B channels.
	// Since all R, G, B have the same value, this is effectively a grayscale image.
	// Loading and processing an 8 bit image may have some problem because of the pixel alignment
	// - image width and bit width may not be aligned correctly. This is because the image width
	//   needs to be a multiple of 4 bytes but the bit representation does not have this restriction.
	// We will not handle this issue in detail in this lab. Most of the images we will handle will be
	// in 24 bit format in practice.
	m_mfcImage1->Load("SaltPepper_24bit.bmp");

	LONG width1 = m_mfcImage1->GetWidth();
	LONG height1 = m_mfcImage1->GetHeight();
	WORD bitcount1 = m_mfcImage1->GetBPP();
	int pitch1 = m_mfcImage1->GetPitch();

	m_width = width1;
	m_height = height1;


	COLORREF col;
	unsigned char gray;
	m_imageBits = new unsigned char[m_width*m_height];
	m_imageBitsFiltered = new unsigned char[m_width*m_height];
	int max_gray = 0;
	int max_d = 0;

	// take R value only because R, G, B have the same value
	for (r=0; r<m_height; r++) {
		for (c=0; c<m_width; c++) {
			col = m_mfcImage1->GetPixel(c,r);
			m_imageBits[r*m_width+c] = GetRValue(col); 
			// GetGValue() and GetBValue() to take the multi-channel values
		}
	}

	int w = floor((double)w0/2);
	int k, l;
	double **kernel = new double*[w0];
	for (r=0; r<w0; r++)
		kernel[r] = new double[w0];
	for (r=0; r<w0; r++)
		for (c=0; c<w0; c++)
			kernel[r][c] = (double) 1/((double)w0*w0);
	double temp, mean;
	

	QueryPerformanceFrequency(&m_frequency);
	QueryPerformanceCounter(&m_start);

	// Perform average filtering, using moving average
	for (r=0; r<m_height; r++) {
		for (c=0; c<m_width; c++) {
			//
			// Problem 1-b1,b2,b3: imageBits 에 Moving Average Filtering 처리를 하여 imageBitsFiltered 에 할당한다
			//

			m_imageBitsFiltered[r*m_width+c] = 0;

			if(c==0){
				mean=0;
				for(l = -w ; l <= w ; l++){
					for(k = -w ; k <=w ; k++){
						if((r+l)<0 || (r+l)>=m_height || (c+k)<0 || (c+k)>=m_width) continue;
						else
							mean += m_imageBits[(r+l)*m_width+c+k]*kernel[l+w][k+w];
					}
				}
			}
			else{
				l=-w;
				for(k=-w;k<w+1;k++){
					if(r+k>=0 && c+l-1 >=0)
						mean -= m_imageBits[(r+k)*m_width+c+l-1]*kernel[k+w][l+w];
				}
				l=w;
				for(k=-w;k<w+1;k++){
					if(r+k>=0 && c+w >=0 )
						mean += m_imageBits[(r+k)*m_width+c+l]*kernel[k+w][l+w];
				}
			}
			m_imageBitsFiltered[r*m_width+c] = mean;
		}
	}
	QueryPerformanceCounter(&m_end);
	m_result = 1000*(m_end.QuadPart-m_start.QuadPart)/m_frequency.QuadPart;	

	// Assign the same value into R, G, B channels.
	for (r=0; r<m_height; r++) {
		for (c=0; c<m_width; c++) {
			//m_mfcImage1->SetPixel(c,r,RGB(imageBits[r*m_width+c],imageBits[r*m_width+c],imageBits[r*m_width+c]));
			temp = m_imageBitsFiltered[r*m_width+c];
			m_mfcImage1->SetPixel(c,r,RGB(temp,temp,temp));
		}
	}

	Invalidate();
	delete m_imageBits, m_imageBitsFiltered;
	for (r=0; r<w0; r++)
		delete kernel[r];
	delete [] kernel;
}
