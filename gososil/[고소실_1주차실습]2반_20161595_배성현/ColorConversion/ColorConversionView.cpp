
// ColorConversionView.cpp : implementation of the CColorConversionView class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "ColorConversion.h"
#endif

#include "ColorConversionDoc.h"
#include "ColorConversionView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CColorConversionView

IMPLEMENT_DYNCREATE(CColorConversionView, CView)

BEGIN_MESSAGE_MAP(CColorConversionView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_COMMAND(ID_PROBLEM1_SUB1, &CColorConversionView::OnProblem1Sub1)
	ON_COMMAND(ID_PROBLEM1_SUB2, &CColorConversionView::OnProblem1Sub2)
	ON_COMMAND(ID_PROBLEM2, &CColorConversionView::OnProblem2)
	ON_COMMAND(ID_PROBLEM3, &CColorConversionView::OnProblem3)
END_MESSAGE_MAP()

// CColorConversionView construction/destruction

CColorConversionView::CColorConversionView()
{
	// TODO: add construction code here
	m_width = 0;
	m_height = 0;

}

CColorConversionView::~CColorConversionView()
{
}

BOOL CColorConversionView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CColorConversionView drawing

void CColorConversionView::OnDraw(CDC* pDC)
{
	CColorConversionDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
	if (m_width > 0) {
		StretchDIBits(pDC->GetSafeHdc(), 0, 50, m_width, m_height,
			0, 0, m_width, m_height,
			m_dibFile.m_lpImage, (LPBITMAPINFO) m_dibFile.m_lpBMIH, DIB_RGB_COLORS, SRCCOPY);

		if (PRB == PRB1) {
			StretchDIBits(pDC->GetSafeHdc(), m_width+10, 50, m_width, m_height,
				0, 0, m_width, m_height,
				mg_lpImage, (LPBITMAPINFO) m_dibFile.m_lpBMIH, DIB_RGB_COLORS, SRCCOPY);
		}
		else {
			StretchDIBits(pDC->GetSafeHdc(), m_width+10, 50, m_width, m_height,
				0, 0, m_width, m_height,
				mg_lpImage, (LPBITMAPINFO) mg_lpBMIH, DIB_RGB_COLORS, SRCCOPY);
		}
		if (PRB == PRB2) {
			m_cs.Format("Conversion time: %5.2f  (millisecond)", m_result);
			pDC->TextOut(0,10,m_cs);
		}
	}
}


// CColorConversionView diagnostics

#ifdef _DEBUG
void CColorConversionView::AssertValid() const
{
	CView::AssertValid();
}

void CColorConversionView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CColorConversionDoc* CColorConversionView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CColorConversionDoc)));
	return (CColorConversionDoc*)m_pDocument;
}
#endif //_DEBUG


// CColorConversionView message handlers

void CColorConversionView::OnProblem1Sub1()
{
	// Read a grayscale image and reduce pixel values by half
	PRB = PRB1;
	CClientDC pDC(this);
	
	CFile file;
	file.Open("lena_8.bmp", CFile::modeRead);	
	if (m_dibFile.Read(&file) != TRUE) {
		file.Close();
		return;
	}
	file.Close();

	LONG width = (m_dibFile.m_lpBMIH)->biWidth;
	LONG height = (m_dibFile.m_lpBMIH)->biHeight;
	WORD bitcount = (m_dibFile.m_lpBMIH)->biBitCount; // color depth, 8, 16, or 24

	mg_lpImage = (unsigned char*) new unsigned char[height*width]; 	

	unsigned char g;
	unsigned int r, c;


	///////////////////////////////////////////////////////

	// PROBLEM 1.1: ADD YOUR CODE HERE TO REDUCE PIXEL VALUES BY HALF

	for (LONG i = 0; i < width*height; i++)
	{
			mg_lpImage[i] = m_dibFile.m_lpImage[i] / 2.0;
	}
	///////////////////////////////////////////////////////

	m_width = width;
	m_height = height;
	Invalidate();
}

void CColorConversionView::OnProblem1Sub2()
{
	// Read a grayscale image and increase pixel values by twice
	PRB = PRB1;
	CClientDC pDC(this);
	
	CFile file;
	file.Open("lena_8.bmp", CFile::modeRead);	
	if (m_dibFile.Read(&file) != TRUE) {
		file.Close();
		return;
	}
	file.Close();

	LONG width = (m_dibFile.m_lpBMIH)->biWidth;
	LONG height = (m_dibFile.m_lpBMIH)->biHeight;
	WORD bitcount = (m_dibFile.m_lpBMIH)->biBitCount; // color depth, 8, 16, or 24

	mg_lpImage = (unsigned char*) new unsigned char[height*width]; 	

	unsigned char g;
	unsigned int r, c;

	///////////////////////////////////////////////////////

	// PROBLEM 1.2:ADD YOUR CODE HERE TO INCREASE PIXEL VALUES TWICE
	for (LONG i = 0; i < width*height; i++)
	{
			if (m_dibFile.m_lpImage[i] * 2 <= 255)
				mg_lpImage[i] = m_dibFile.m_lpImage[i] * 2.0;
			else
				mg_lpImage[i] = 255;
	}

	///////////////////////////////////////////////////////

	m_width = width;
	m_height = height;
	Invalidate();
}

void CColorConversionView::OnProblem2()
{
	// Convert a 24 bit color image into a grayscale image
	PRB = PRB2;
	CClientDC pDC(this);
	
	CFile file;
	file.Open("lena_24.bmp", CFile::modeRead);	
	if (m_dibFile.Read(&file) != TRUE) {
		file.Close();
		return;
	}
	file.Close();

	LONG width = (m_dibFile.m_lpBMIH)->biWidth;
	LONG height = (m_dibFile.m_lpBMIH)->biHeight;
	WORD bitcount = (m_dibFile.m_lpBMIH)->biBitCount; // color depth, 8, 16, or 24

	BuildGrayBitmapHeader(width, height);

	::RealizePalette(pDC);

	mg_lpImage = (unsigned char*) new unsigned char[height*width]; 	

	unsigned char rd, gr, bl;
	unsigned int r, c;
	unsigned char bit = bitcount/8;

	QueryPerformanceFrequency(&m_frequency);
	QueryPerformanceCounter(&m_start);

	///////////////////////////////////////////////////////

	// PROBLEM 2:ADD YOUR CODE HERE TO CONVERT 24 BIT COLOR TO GRAYSCALE
	for (LONG i = 0; i < width*height; i++)
	{
			bl = m_dibFile.m_lpImage[3 * i + 0];
			gr = m_dibFile.m_lpImage[3 * i + 1];
			rd = m_dibFile.m_lpImage[3 * i + 2];
			mg_lpImage[i] = 0.299 * rd + 0.587 * gr + 0.114 * bl;
	}

	///////////////////////////////////////////////////////

	QueryPerformanceCounter(&m_end);
	m_result = 1000*(m_end.QuadPart-m_start.QuadPart)/m_frequency.QuadPart;		
		

	m_width = width;
	m_height = height;
	Invalidate();
}


void CColorConversionView::OnProblem3()
{
	// Convert a 16 bit color image into a grayscale image
	PRB = PRB3;
	CClientDC pDC(this);
	
	CFile file;
	file.Open("image_16.bmp", CFile::modeRead);	
	if (m_dibFile.Read(&file) != TRUE) {
		file.Close();
		return;
	}
	file.Close();

	LONG width = (m_dibFile.m_lpBMIH)->biWidth;
	LONG height = (m_dibFile.m_lpBMIH)->biHeight;
	WORD bitcount = (m_dibFile.m_lpBMIH)->biBitCount; // color depth, 8, 16, or 24

	BuildGrayBitmapHeader(width, height);

	::RealizePalette(pDC);

	mg_lpImage = (unsigned char*) new unsigned char[height*width]; 	

	unsigned short* tempim16 = (unsigned short*) new unsigned short[width*height];
	tempim16 = (unsigned short*) m_dibFile.m_lpImage;

	unsigned char rd, gr, bl;
	unsigned int r, c;
	unsigned char bit = bitcount/8;

	unsigned short p1;

	///////////////////////////////////////////////////////

	// PROBLEM 3:ADD YOUR CODE HERE TO CONVERT 16 BIT COLOR TO GRAYSCALE
	for (LONG i = 0; i < width * height; i++)
	{
		p1 = tempim16[i];
		bl = (p1 & 0b11111)<<3;
		p1 = p1 >> 5;
		gr = (p1 & 0b11111)<<3;
		p1 = p1 >> 5;
		rd = (p1 & 0b11111)<<3;
		mg_lpImage[i] = 0.299 * rd + 0.587 * gr + 0.114 * bl;
	}
	///////////////////////////////////////////////////////

	m_width = width;
	m_height = height;

	Invalidate();
}

void CColorConversionView::BuildGrayBitmapHeader (int width, int height)
{
	// Create a BMP header for grayscale images

	mg_lpBMIH = (LPBITMAPINFOHEADER) new char[sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * 256];
	mg_lpBMIH->biSize = sizeof(BITMAPINFOHEADER);
	mg_lpBMIH->biWidth = width;
	mg_lpBMIH->biHeight = height;
	mg_lpBMIH->biPlanes = 1;
	mg_lpBMIH->biBitCount = 8;
	mg_lpBMIH->biCompression = BI_RGB;
	DWORD dwBytes = ((DWORD) mg_lpBMIH->biWidth * mg_lpBMIH->biBitCount) / 32;
	if(((DWORD) mg_lpBMIH->biWidth * mg_lpBMIH->biBitCount) % 32) {
		dwBytes++;
	}
	dwBytes *= 4;
	DWORD mg_dwSizeImage = dwBytes * mg_lpBMIH->biHeight; // no compression

	mg_lpBMIH->biSizeImage = mg_dwSizeImage;
	mg_lpBMIH->biXPelsPerMeter = 0;
	mg_lpBMIH->biYPelsPerMeter = 0;
	mg_lpBMIH->biClrUsed = 256;
	mg_lpBMIH->biClrImportant = 256;
	LPVOID mg_lpvColorTable = (LPBYTE) mg_lpBMIH + sizeof(BITMAPINFOHEADER);

	// make palette
	LPLOGPALETTE pLogPal = (LPLOGPALETTE) new char[2 * sizeof(WORD) +
		256 * sizeof(PALETTEENTRY)];
	pLogPal->palVersion = 0x300;
	pLogPal->palNumEntries = 256;
	LPRGBQUAD pDibQuad = (LPRGBQUAD) mg_lpvColorTable;
	for(int i = 0; i < 256; i++) {
		pDibQuad->rgbRed = i;
		pDibQuad->rgbGreen = i;
		pDibQuad->rgbBlue = i;
		pLogPal->palPalEntry[i].peRed = pDibQuad->rgbRed;
		pLogPal->palPalEntry[i].peGreen = pDibQuad->rgbGreen;
		pLogPal->palPalEntry[i].peBlue = pDibQuad->rgbBlue;
		pLogPal->palPalEntry[i].peFlags = 0;

		pDibQuad++;
	}
	delete pLogPal;
}

