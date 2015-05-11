// GuiDlg.h : header file
//

#pragma once
#include <winioctl.h>
#include "afxwin.h"
#include "afxcmn.h"
#include "atlimage.h"
#include "gui_vmm.h"

#include "service_functions.h"


#include "print.h"


// CGuiDlg dialog
class CGuiDlg : public CDialog
{
// Construction
public:
	CGuiDlg(CWnd* pParent = NULL);	// standard constructor	

	CBitmap m_bitmap;

	static service_functions :: ServiceManager m_smCsmgr;
	static HANDLE m_hNeither;

// Dialog Data
	enum { IDD = IDD_GUI_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

// Implementation
protected:
	HICON m_hIcon;	
	
	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:	
	
	//////////////////////////////////////////////////////////////////////////
	CEdit m_linkNameDETECT;
	static CEdit m_fileNameDETECT;

	//////////////////////////////////////////////////////////////////////////

	bool init_drv();

	afx_msg void OnBnClickedBtnFilePath();
	afx_msg void OnBnClickedBtnAddStartOpen();
	afx_msg void OnBnClickedBtnCloseStopRemove();
	
	afx_msg void OnBnClickedBtnRunHYP();
	afx_msg void OnBnClickedBtnStopHYP();

	bool call_cpuid(IN int iCPUnum, OUT bool & bVirtPresent);
	bool get_vmm_state();
	afx_msg void OnBnClickedBtnGetStateCpu01();

	CEdit m_EditSetDelta;	
	afx_msg void OnBnClickedBtnSetDelta();

	// logging
	CListBox m_cListLog;
	void insert_log(TCHAR *str);
	void vspf(char *fmt, va_list argptr);
	void vspf(WCHAR *fmt, va_list argptr);
	void PrintLastError(char *fmt, va_list argptr);	
	void PrintLastError(WCHAR *fmt, va_list argptr);
};