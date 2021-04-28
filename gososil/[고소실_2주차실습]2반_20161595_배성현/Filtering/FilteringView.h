
// FilteringView.h : interface of the CFilteringView class
//

#pragma once
#include "atlimage.h"

class CFilteringView : public CView
{
protected: // create from serialization only
	CFilteringView();
	DECLARE_DYNCREATE(CFilteringView)

// Attributes
public:
	CFilteringDoc* GetDocument() const;

// Operations
public:

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// Implementation
public:
	virtual ~CFilteringView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnProblem1A1();
	afx_msg void OnProblem1A2();
	afx_msg void OnProblem1A3();
	afx_msg void OnProblem1B1();
	afx_msg void OnProblem1B2();
	afx_msg void OnProblem1B3();
	afx_msg void OnProblem2A1();
	afx_msg void OnProblem2A2();
	afx_msg void OnProblem2A3();
	afx_msg void OnProblem3();

private:
	void AverageFiltering(int);
	void MovingAverageFiltering(int);
	void MedianFiltering(int);

	ATL::CImage *m_mfcImage1;
	LONG m_width, m_height;

	unsigned char *m_imageBits, *m_imageBitsFiltered; 

	LARGE_INTEGER m_start, m_end, m_frequency;
	double m_result;
	CString m_cs;
};

#ifndef _DEBUG  // debug version in FilteringView.cpp
inline CFilteringDoc* CFilteringView::GetDocument() const
   { return reinterpret_cast<CFilteringDoc*>(m_pDocument); }
#endif

