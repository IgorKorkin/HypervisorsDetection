
// guidlg.h : header file
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "Resource.h"

// Extern code for gui
#include "ReportCtrl.h"

// 
#include "shared/common_vmmkar.h"

#include "service_functions.h"

#include "measuring_trace.h"

//#include "get_rdtsc_console/virtualization_detector.h"

// 
#include "print.h"


// CguiDlg dialog
class CguiDlg : public CDialog
{
	// Construction
public:
	static service_functions :: ServiceManager m_smCsmgr;
	static HANDLE m_hNeither;
	static void set_one_task(int Item, TaskForMeasurement & Task);	
	static void exec_one_task(int Item, TaskForMeasurement & Task);
	static void start_thread();
	static void stop_thread();
	static void check_thread();
	bool init_drv();

	//////////////////////////////////////////////////////////////////////////
	CguiDlg(CWnd* pParent = NULL);	// standard constructor

	// Dialog Data
	enum { IDD = IDD_GUI_DIALOG };
	static CReportCtrl	m_wndList; // <- список содержит названия и параметры методов обнаружения
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

	// init_report_ctrl form
	void init_report_ctrl();

	// insert data
	void insert_log(TCHAR *str);

public:
	// insert data
	void vspf(TCHAR *fmt, va_list argptr);
	void vspf(char *fmt, va_list argptr);
	void PrintLastError(char *fmt, va_list argptr);
	void PrintLastError(TCHAR *fmt, va_list argptr);

	CListBox clist_log; // <- список содержит информацию о выполненных операциях
	CMenu TrayMenu;
	afx_msg void OnBnClickedStart();
	static void exec_all();

	static CProgressCtrl m_ctrlProgress;
	static CButton m_btnExecTasks;

	static HANDLE h_ThreadTasks;
	static bool g_StopFlag;

	afx_msg void OnNMRClickListReportCtl(NMHDR *pNMHDR, LRESULT *pResult);
	void select_all();
	
	afx_msg void OnLvnItemchangedListReportCtl(NMHDR *pNMHDR, LRESULT *pResult);
};