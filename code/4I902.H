// 4i902.h : main header file for the 4i902 application
//
#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols


// Ci902App:
// See 4i902.cpp for the implementation of this class
//

class Ci902App : public CWinApp
{
public:
	Ci902App();


// Overrides
public:
	virtual BOOL InitInstance();

// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern Ci902App theApp;