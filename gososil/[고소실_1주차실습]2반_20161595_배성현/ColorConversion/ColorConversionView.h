
// ColorConversionView.h : interface of the CColorConversionView class
//

#pragma once

#include "atlimage.h"
#include "cdib.h"

class CColorConversionView : public CView
{
protected: // create from serialization only
	CColorConversionView();
	DECLARE_DYNCREATE(CColorConversionView)

// Attributes
public:
	CColorConversionDoc* GetDocument() const;

// Operations
public:

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

enum PRB_TYPE { PRB1, PRB2, PRB3 };
enum PRB_TYPE PRB;

// Implementation
public:
	virtual ~CColorConversionView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	void BuildGrayBitmapHeader(int, int);

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnProblem1Sub1();
	afx_msg void OnProblem1Sub2();
	afx_msg void OnProblem2();
	afx_msg void OnProblem3();

// Member variables
private:
	CDib m_dibFile;
	LPBITMAPINFOHEADER mg_lpBMIH;
	unsigned char *mg_lpImage;

	ATL::CImage m_mfcImageCol1;
	LONG m_width, m_height;
	LARGE_INTEGER m_start, m_end, m_frequency;
	double m_result;
	CString m_cs;

};

#ifndef _DEBUG  // debug version in ColorConversionView.cpp
inline CColorConversionDoc* CColorConversionView::GetDocument() const
   { return reinterpret_cast<CColorConversionDoc*>(m_pDocument); }
#endif

