
// ColorConversion.h : main header file for the ColorConversion application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CColorConversionApp:
// See ColorConversion.cpp for the implementation of this class
//

class CColorConversionApp : public CWinApp
{
public:
	CColorConversionApp();


// Overrides
public:
	virtual BOOL InitInstance();

// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CColorConversionApp theApp;
