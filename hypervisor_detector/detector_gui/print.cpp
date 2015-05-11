
#include "stdafx.h"

#include "print.h"

namespace print
{
	void init_progressbar()
	{
		((CguiDlg*)(theApp.m_pMainWnd))->m_ctrlProgress.SetRange(1, 100);
	}
	
	void set_progressbar(int iPos)
	{
		((CguiDlg*)(theApp.m_pMainWnd))->m_ctrlProgress.SetPos(iPos);
	}
	
	void print_mes(TCHAR *fmt, ...)
	{	
		va_list argptr;
		va_start(argptr, fmt);
		((CguiDlg*)(theApp.m_pMainWnd))->vspf(fmt, argptr);	
		va_end(argptr);
	}

	void print_last_err(TCHAR *fmt, ...)
	{
		va_list argptr;
		va_start(argptr, fmt);
		((CguiDlg*)(theApp.m_pMainWnd))->PrintLastError(fmt, argptr);
		va_end(argptr);
	}

	void print_mes(char *fmt, ...)
	{	
		va_list argptr;
		va_start(argptr, fmt);
		((CguiDlg*)(theApp.m_pMainWnd))->vspf(fmt, argptr);	
		va_end(argptr);
	}

	void print_last_err(char *fmt, ...)
	{
		va_list argptr;
		va_start(argptr, fmt);
		((CguiDlg*)(theApp.m_pMainWnd))->PrintLastError(fmt, argptr);
		va_end(argptr);
	}
}