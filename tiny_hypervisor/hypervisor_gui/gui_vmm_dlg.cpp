// GuiDlg.cpp : implementation file
//

#include "stdafx.h"
#include "gui_vmm_dlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/************************************************************************/
/*		G	L	O	B	A	L	S		                                */
/************************************************************************/

CEdit CGuiDlg :: m_fileNameDETECT;

service_functions :: ServiceManager CGuiDlg :: m_smCsmgr;
HANDLE CGuiDlg :: m_hNeither = INVALID_HANDLE_VALUE;;
bool b_stateActiveVMM = false;


//////////////////////////////////////////////////////////////////////////


class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};


void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CGuiDlg dialog




CGuiDlg::CGuiDlg(CWnd* pParent)
	: CDialog(CGuiDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CGuiDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, GUI_VMM_EDIT_FILE_NAME, m_fileNameDETECT);

	DDX_Control(pDX, GUI_VMM_EDIT_SET_DELTA, m_EditSetDelta);
	DDX_Control(pDX, GUI_VMM_LIST_LOG, m_cListLog);
}

BEGIN_MESSAGE_MAP(CGuiDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
		
	ON_BN_CLICKED(GUI_VMM_BTN_ADDSTARTOPEN, &CGuiDlg::OnBnClickedBtnAddStartOpen)
	ON_BN_CLICKED(GUI_VMM_BTN_CLOSESTOPREMOVE, &CGuiDlg::OnBnClickedBtnCloseStopRemove)
	ON_BN_CLICKED(GUI_VMM_BTN_FILEPATH, &CGuiDlg::OnBnClickedBtnFilePath)

 	ON_BN_CLICKED(GUI_VMM_BTN_GET_STATE_CPU0, &CGuiDlg::OnBnClickedBtnGetStateCpu01)

	ON_BN_CLICKED(GUI_VMM_BTN_RUNVMM0, &CGuiDlg::OnBnClickedBtnRunHYP)
	ON_BN_CLICKED(GUI_VMM_BTN_STOPVMM0, &CGuiDlg::OnBnClickedBtnStopHYP)
	
	ON_BN_CLICKED(GUI_VMM_BTN_SET_DELTA, &CGuiDlg::OnBnClickedBtnSetDelta)
END_MESSAGE_MAP()




//////////////////////////////////////////////////////////////////////////

namespace resource_functions
{
	typedef struct _RESOURCE
	{
		LPVOID  ResContent;
		DWORD   ResSize;
		HRSRC   hResFile;
		HGLOBAL hResData;
	}RESOURCE, *PRESOURCE;

	bool extract_resource(RESOURCE & resourse, LPCTSTR lpName, LPCTSTR lpType)
	{
		bool b_res = false;
		resourse.hResFile = FindResource(NULL, lpName, lpType);
		if (resourse.hResFile)
		{
			resourse.ResSize = SizeofResource(NULL, resourse.hResFile);
			if (resourse.ResSize)
			{
				resourse.hResData = LoadResource(NULL, resourse.hResFile);
				if (resourse.hResData)
				{
					resourse.ResContent = LockResource(resourse.hResData);
					if (resourse.ResContent)   {   b_res = true;   }
				}
			}
		}
		return b_res;
	}

	bool get_temp_filename(LPCTSTR lpPrefixString, LPTSTR lpTempFileName)
	{
		bool b_res = false;
		wchar_t tmp_path[MAX_PATH] = {0};
		if (GetTempPath(MAX_PATH,tmp_path) && 
			GetTempFileName(tmp_path, lpPrefixString, 0, lpTempFileName))
		{
			b_res = true;
		}
		return b_res;
	}
}

bool CGuiDlg :: init_drv()
{
	bool b_res = false;
	resource_functions::RESOURCE resourse = {0}; RtlSecureZeroMemory(&resourse, sizeof(resource_functions::RESOURCE) );
	if (resource_functions :: extract_resource(resourse, MAKEINTRESOURCE(IDR_HYPERVISOR_DRIVER_BIN_SYSFILE), TEXT("bin")))
	{
		TCHAR bin_file[MAX_PATH] = {0};
		if (resource_functions :: get_temp_filename(TEXT("BB"), bin_file))
		{
			HANDLE h_file = NULL;
			if (PathFileExists(bin_file) &&
				( (HANDLE)INVALID_HANDLE_VALUE != (h_file = CreateFile(bin_file, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_TEMPORARY, NULL)) ))
			{
				DWORD number_of_bytes_written = 0;
				if (WriteFile(h_file, resourse.ResContent, resourse.ResSize, &number_of_bytes_written, NULL) &&
					(number_of_bytes_written == resourse.ResSize))
				{   b_res = true;   }
			}
			else   {   print  ::  print_last_err(L"err file %s ", bin_file);   }
			if (h_file)   {   CloseHandle(h_file);   }

			if (b_res)
			{
				print :: print_mes("+ Driver install to %s", bin_file);

				m_smCsmgr.init(VMM_DRV_NAME_ServiseNametoGUI, bin_file);
				m_smCsmgr.stop_driver();
				m_smCsmgr.remove_driver();
				if (m_smCsmgr.add_driver())
				{
					if (m_smCsmgr.start_driver())
					{
						if (m_smCsmgr.chk_service_active())
						{
							if ((m_hNeither = m_smCsmgr.open_device(VMM_DRV_NAME_LinkNametoGUI)) != INVALID_HANDLE_VALUE)
							{
								b_res = true;
								print :: print_mes("Hypervisor driver is active");
							}
							else
							{   print :: print_last_err("error open device %s ", VMM_DRV_NAME_LinkNameToDRV);   }
						}
						else
						{   print :: print_last_err("driver %s %s is not active", VMM_DRV_NAME_ServiseNametoGUI, bin_file);   }
					}
					else
					{
						print :: print_last_err("error start driver %s %s", VMM_DRV_NAME_ServiseNametoGUI, bin_file);
						m_smCsmgr.stop_driver();
						m_smCsmgr.remove_driver();
					}
				}	
				else
				{
					print :: print_last_err("error add driver %s %s", VMM_DRV_NAME_ServiseNametoGUI, bin_file);
					m_smCsmgr.stop_driver();
					m_smCsmgr.remove_driver();
				}
			}
			//DeleteFile(bin_file);
		}
	}
	return b_res;
}

// CGuiDlg message handlers

BOOL CGuiDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon


	TCHAR lpBuffer[MAX_PATH] = {0};
	if (GetCurrentDirectory( MAX_PATH , lpBuffer ))
	{
		swprintf(lpBuffer, MAX_PATH * sizeof(PWCH) , L"%s\\%s.sys", lpBuffer, VMM_DRV_NAME );
		m_fileNameDETECT.SetWindowText( lpBuffer );
	}

	init_drv();
	
	const TCHAR label[] = TEXT("Tiny Hypervisor Configuration Tool");
	AfxGetApp()->m_pMainWnd->SetWindowText(label);

	m_EditSetDelta.SetWindowText(TEXT("1000"));	
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}


void CGuiDlg::OnSysCommand(UINT nID, LPARAM lParam)
{

	CDialog::OnSysCommand(nID, lParam);
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CGuiDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	if (IsIconic())
	{
		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);

	}
	else
	{
		CDialog::OnPaint();
	}

	
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.

HCURSOR CGuiDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}




/************************************************************************/
/*                                                                      */
/************************************************************************/

int g_CurSize = 0;

void CGuiDlg :: insert_log(TCHAR *str)
{
	int index = m_cListLog.InsertString(-1, (LPCTSTR)str);
	m_cListLog.SelectString(index - 1, (LPCTSTR)str);

	CClientDC dc(this);
	CFont * f = m_cListLog.GetFont();
	dc.SelectObject(f);
	CSize sz = dc.GetTextExtent(str, _tcslen(str));
	sz.cx += 3 * ::GetSystemMetrics(SM_CXBORDER);
	if(sz.cx > g_CurSize)
	{ /* extend */
		g_CurSize = sz.cx;
		m_cListLog.SetHorizontalExtent(g_CurSize);
	} /* extend */
}

void CGuiDlg :: vspf(TCHAR *fmt, va_list argptr)
{
	WCHAR wtmpbuf[MAX_PATH] = {0};
	int cnt = _vsntprintf_s(wtmpbuf, MAX_PATH, fmt, argptr);
	insert_log(wtmpbuf);	
}

void CGuiDlg :: vspf(char *fmt, va_list argptr)
{
	WCHAR wfmt[MAX_PATH] = {0}, wtmpbuf[MAX_PATH] = {0};
	MultiByteToWideChar(CP_ACP,MB_ERR_INVALID_CHARS, fmt, -1, wfmt, 200);
	int cnt = _vsntprintf_s(wtmpbuf, MAX_PATH, wfmt, argptr);
	insert_log(wtmpbuf);
}

void CGuiDlg :: PrintLastError(char *fmt, va_list argptr)
{
	LPVOID lpMsgBuf;
	int cnt = 0;	
	WCHAR wfmt[MAX_PATH], wtmpbuf[MAX_PATH];
	memset(wfmt, 0, MAX_PATH);
	memset(wtmpbuf, 0, MAX_PATH);
	MultiByteToWideChar(CP_ACP,MB_ERR_INVALID_CHARS, fmt,-1,wfmt,200);
	cnt = _vsntprintf_s(wtmpbuf, MAX_PATH, wfmt, argptr);
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR) &lpMsgBuf,
		0,
		NULL
		);	

	cnt += _stprintf_s(wtmpbuf + cnt, MAX_PATH - cnt, TEXT(" %s"), lpMsgBuf);	
	insert_log(wtmpbuf);
	LocalFree( lpMsgBuf );
}
void CGuiDlg :: PrintLastError(TCHAR *fmt, va_list argptr)
{
	LPVOID lpMsgBuf;
	int cnt = 0;	
	WCHAR wtmpbuf[MAX_PATH] = {0};
	memset(wtmpbuf, 0, MAX_PATH);
	cnt = _vsntprintf_s(wtmpbuf, MAX_PATH, fmt, argptr);
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR) &lpMsgBuf,
		0,
		NULL
		);	
	cnt += _stprintf_s(wtmpbuf + cnt, MAX_PATH - cnt, TEXT(" %s"), lpMsgBuf);	
	insert_log(wtmpbuf);
	LocalFree( lpMsgBuf );
}

/************************************************************************/
/*                                                                      */
/************************************************************************/


void CGuiDlg::OnBnClickedBtnFilePath()
{
	CFileDialog fileDlg(true, (LPCTSTR)L"sys", (LPCTSTR)L"", OFN_CREATEPROMPT | OFN_EXPLORER | OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY, (LPCTSTR)L"Driver (*.sys)|*.sys|*.*|*.*||",0,0,1);

	if (fileDlg.DoModal()==IDOK) 
	{
		CString pathName = fileDlg.GetPathName();
		m_fileNameDETECT.SetWindowText(pathName);
	}
}

void CGuiDlg::OnBnClickedBtnAddStartOpen()
{
	init_drv();
}

void CGuiDlg::OnBnClickedBtnCloseStopRemove()
{
	TCHAR bin_file[MAX_PATH] = {0};
	CString csbin_file;
	m_fileNameDETECT.GetWindowText(csbin_file);
	_stprintf_s(bin_file, TEXT("%s"), csbin_file.GetString());

	m_smCsmgr.init(VMM_DRV_NAME_ServiseNametoGUI, bin_file);
	if (m_hNeither != INVALID_HANDLE_VALUE)
		{   m_smCsmgr.close_device(m_hNeither);   }

	if (m_smCsmgr.stop_driver())
		{   print :: print_mes("+ stop driver %s %s ", VMM_DRV_NAME_ServiseNametoGUI, bin_file );   }
	else
		{   print :: print_last_err("error stop driver %s %s ", VMM_DRV_NAME_ServiseNametoGUI, bin_file);   }

	if (m_smCsmgr.remove_driver())
		{   print :: print_mes("+ remove driver %s %s ", VMM_DRV_NAME_ServiseNametoGUI, bin_file );   }
	else
		{   print :: print_last_err("error remove driver %s %s ", VMM_DRV_NAME_ServiseNametoGUI, bin_file);   }
}

void CGuiDlg::OnBnClickedBtnSetDelta()
{
	CString cs_delta;
	__int64 delta = 0;

	m_EditSetDelta.GetWindowText(cs_delta);

	_stscanf_s( cs_delta.GetString(), TEXT("%I64d"), &delta);

	const __int64 delta_min = 0;
	const __int64 delta_max = 5000;
	if (((delta_min <= delta) && (delta <= delta_max)))
	{
		if (m_smCsmgr.send_ctrl_code(m_hNeither, SET_DELTA, &delta, sizeof(__int64), NULL, 0, 0))
			{   print :: print_mes("+ Delta is %d", delta);   }
		else
			{   print :: print_last_err("error set delta");  }
	}
	else
		{   print :: print_mes("delta = %I64d is out of range [%I64d-%I64d]", delta, delta_min, delta_max);   }
}

void CGuiDlg::OnBnClickedBtnRunHYP()
{
	if (m_smCsmgr.send_ctrl_code(m_hNeither, RUN_VMM_CPU0, NULL, 0, NULL, 0, 0))
		{   print :: print_mes("+ HYP is loaded, check state");   }
	else
		{   print :: print_last_err("error VMM load");   }
}

void CGuiDlg::OnBnClickedBtnStopHYP()
{
	if (m_smCsmgr.send_ctrl_code(m_hNeither, STOP_VMM_CPU0, NULL, 0, NULL, 0, 0))
		{   print :: print_mes("- HYP is unloaded, check state");   }
	else
		{   print :: print_last_err("error VMM unload");   }
}


bool CGuiDlg :: call_cpuid(IN int iCPUnum, OUT bool & bVirtPresent)
{
	OUT_CPUID_0_1 cpuid_01; RtlZeroMemory(&cpuid_01, sizeof(OUT_CPUID_0_1));

	CONFIG_VMM config_vm; RtlZeroMemory(&config_vm, sizeof(CONFIG_VMM));
	config_vm.kAfinity = 0x0000001;

	bool b_res = m_smCsmgr.send_ctrl_code(m_hNeither, GET_CPUID_0, &config_vm, sizeof(CONFIG_VMM), &cpuid_01, sizeof(OUT_CPUID_0_1), NULL);
	
	if (b_res && (cpuid_01.EBX == CPU01_EBX) && (cpuid_01.ECX == CPU01_ECX) && (cpuid_01.EDX == CPU01_EDX))
		{   bVirtPresent = true;   }
	return b_res;
} 

bool CGuiDlg :: get_vmm_state()
{
	bool b_res = false;
	int cpu_num = 0;
	bool v_present = false;
	if (call_cpuid(cpu_num, v_present))
	{     
		if (v_present)
		{   
			print :: print_mes("+ HYP is running");
			b_res = true;
		}
		else
		{   print :: print_mes("- no tiny HYP");   }
	}
	else
	{   print :: print_last_err("error check hypervisor state");   }

	return b_res;
}

void CGuiDlg::OnBnClickedBtnGetStateCpu01()
{
	get_vmm_state();
}




