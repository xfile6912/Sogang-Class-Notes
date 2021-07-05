
// PCA.h : main header file for the PCA application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CPCAApp:
// See PCA.cpp for the implementation of this class
//

class CPCAApp : public CWinApp
{
public:
	CPCAApp();


// Overrides
public:
	virtual BOOL InitInstance();

// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CPCAApp theApp;
