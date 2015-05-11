// Gui.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols
#include "gui_vmm_dlg.h"


// CGuiApp:
// See Gui.cpp for the implementation of this class
//

class CGuiApp : public CWinApp
{
public:
	CGuiApp();

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CGuiApp theApp;