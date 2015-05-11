
#include "stdafx.h"

#include "print.h"

#include "gui_vmm.h"
#include "gui_vmm_dlg.h"

namespace print
{
	void print_mes(TCHAR *fmt, ...)
	{	
		va_list argptr;
		va_start(argptr, fmt);
		((CGuiDlg*)(theApp.m_pMainWnd))->vspf(fmt, argptr);	
		va_end(argptr);
	}

	void print_last_err(TCHAR *fmt, ...)
	{
		va_list argptr;
		va_start(argptr, fmt);
		((CGuiDlg*)(theApp.m_pMainWnd))->PrintLastError(fmt, argptr);
		va_end(argptr);
	}

	void print_mes(char *fmt, ...)
	{	
		va_list argptr;
		va_start(argptr, fmt);
		((CGuiDlg*)(theApp.m_pMainWnd))->vspf(fmt, argptr);	
		va_end(argptr);
	}

	void print_last_err(char *fmt, ...)
	{
		va_list argptr;
		va_start(argptr, fmt);
		((CGuiDlg*)(theApp.m_pMainWnd))->PrintLastError(fmt, argptr);
		va_end(argptr);
	}
}