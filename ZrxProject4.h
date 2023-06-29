// ZrxProject4.h : main header file for the ZrxProject4 DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CZrxProject4App
// See ZrxProject4.cpp for the implementation of this class
//

class CZrxProject4App : public CWinApp
{
public:
	CZrxProject4App();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
