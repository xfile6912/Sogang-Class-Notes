
// PCAView.h : interface of the CPCAView class
//
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "ConsoleWnd.h"

#pragma once


class CPCAView : public CView
{
protected: // create from serialization only
	CPCAView();
	DECLARE_DYNCREATE(CPCAView)

// Attributes
public:
	CPCADoc* GetDocument() const;

// Operations
public:

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// Implementation
public:
	virtual ~CPCAView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnProblem1();

	ConsoleWnd con;
	CString m_cs;
	cv::Mat data;
	cv::Mat Proj;
};

#ifndef _DEBUG  // debug version in PCAView.cpp
inline CPCADoc* CPCAView::GetDocument() const
   { return reinterpret_cast<CPCADoc*>(m_pDocument); }
#endif

