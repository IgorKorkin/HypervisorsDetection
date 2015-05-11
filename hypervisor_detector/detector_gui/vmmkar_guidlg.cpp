
// guidlg.cpp : implementation file
//

#include "stdafx.h"
#include "vmmkar_gui.h"
#include "vmmkar_guidlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// static 
CReportCtrl CguiDlg :: m_wndList;
CProgressCtrl CguiDlg :: m_ctrlProgress;
CButton CguiDlg :: m_btnExecTasks;
service_functions :: ServiceManager CguiDlg :: m_smCsmgr;
HANDLE CguiDlg :: m_hNeither;

// Globals 
HANDLE CguiDlg ::  h_ThreadTasks = NULL;
bool CguiDlg :: g_StopFlag = false;




// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}



void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CguiDlg dialog




CguiDlg::CguiDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CguiDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CguiDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_REPORT_CTL, m_wndList);
	DDX_Control(pDX, IDC_LIST_LOG, clist_log);
	DDX_Control(pDX, IDC_PROGRESS_EXPERIMENT, m_ctrlProgress);
	DDX_Control(pDX, IDOK, m_btnExecTasks);
	
}

BEGIN_MESSAGE_MAP(CguiDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
ON_BN_CLICKED(IDOK, &CguiDlg::OnBnClickedStart)
ON_NOTIFY(NM_RCLICK, IDC_LIST_REPORT_CTL, &CguiDlg::OnNMRClickListReportCtl)
ON_COMMAND(ID_MENU_CLEAR_SELECT, &CguiDlg::select_all)
ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_REPORT_CTL, &CguiDlg::OnLvnItemchangedListReportCtl)
END_MESSAGE_MAP()


// CguiDlg message handlers

enum columns
{
	Method_type				= 0, // Метод обнаружения
	Affinity_thread			, // Маска ядра процессора, на котором выполняется проверка
	Length_of_a_trace		, // Длина трассы
	Number_of_measurements	, // Число столбцов в матрице измерений	(Число измерений)
	Number_of_repetitions	, // Число строк в матрице измерений (Число повторов)
	Delay_between_repetitions, // Задержка между повторами
	Number_of_experiments	, // Число повторов эксперимента
	Delay_between_experiments, // Задержка между повторами эксперимента
};

void CguiDlg :: init_report_ctrl()
{
	m_wndList.InsertColumn(Method_type, TEXT("Detection method"), LVCFMT_CENTER, 110);
	m_wndList.InsertColumn(Affinity_thread, TEXT("CPU #"), LVCFMT_CENTER, 50);
	m_wndList.InsertColumn(Length_of_a_trace, TEXT("Number of Instructions"), LVCFMT_CENTER, 120);
	m_wndList.InsertColumn(Number_of_measurements, TEXT("Number of Rows"), LVCFMT_CENTER, 100);
	m_wndList.InsertColumn(Number_of_repetitions, TEXT("Number of Columns"), LVCFMT_CENTER, 110);

	m_wndList.InsertColumn(Delay_between_repetitions, TEXT("Column Delay"), LVCFMT_CENTER, 80);

	m_wndList.InsertColumn(Number_of_experiments, TEXT("Number of Matrices"), LVCFMT_CENTER, 110);

	m_wndList.InsertColumn(Delay_between_experiments, TEXT("Matrices Delay"), LVCFMT_CENTER, 90 - 1 );

	DWORD ExStyle = m_wndList.GetExtendedStyle();

	ExStyle |= LVS_EDITLABELS |
		LVS_EX_FULLROWSELECT | 
		LVS_EX_CHECKBOXES | 
		LVS_EX_GRIDLINES | 
		LVS_SINGLESEL | 
		LVS_EX_AUTOSIZECOLUMNS;
	
	m_wndList.SetExtendedStyle(ExStyle);

	m_wndList.DeleteAllItems();

	m_wndList.InsertItem(BlockOfCPUID, TEXT("CPUID")); // Method type
	m_wndList.SetItemText(BlockOfCPUID, Affinity_thread		, TEXT("1"));
	m_wndList.SetItemText(BlockOfCPUID, Length_of_a_trace		, TEXT("10"));
	m_wndList.SetItemText(BlockOfCPUID, Number_of_measurements	, TEXT("1000"));
	m_wndList.SetItemText(BlockOfCPUID, Number_of_repetitions	, TEXT("10"));
	m_wndList.SetItemText(BlockOfCPUID, Delay_between_repetitions, TEXT("2000"));
	m_wndList.SetItemText(BlockOfCPUID, Number_of_experiments	, TEXT("20"));
 	m_wndList.SetItemText(BlockOfCPUID, Delay_between_experiments, TEXT("2000"));
	
 	m_wndList.InsertItem(CPUIDwithDisabledCache, TEXT("CPUID + NoCache")); 
	m_wndList.SetItemText(CPUIDwithDisabledCache, Affinity_thread		, TEXT("1"));
	m_wndList.SetItemText(CPUIDwithDisabledCache, Length_of_a_trace		, TEXT("10"));
	m_wndList.SetItemText(CPUIDwithDisabledCache, Number_of_measurements, TEXT("1000"));
	m_wndList.SetItemText(CPUIDwithDisabledCache, Number_of_repetitions	, TEXT("10"));
	m_wndList.SetItemText(CPUIDwithDisabledCache, Delay_between_repetitions, TEXT("10"));
	m_wndList.SetItemText(CPUIDwithDisabledCache, Number_of_experiments	, TEXT("5"));
 	m_wndList.SetItemText(CPUIDwithDisabledCache, Delay_between_experiments, TEXT("100"));
		
	m_wndList.InsertItem(BlockOfVMXON, TEXT("VMXON"));
	m_wndList.SetItemText(BlockOfVMXON, Affinity_thread		, TEXT("1"));  
	m_wndList.SetItemText(BlockOfVMXON, Length_of_a_trace		, TEXT("10"));  
	m_wndList.SetItemText(BlockOfVMXON, Number_of_measurements	, TEXT("1000")); 
	m_wndList.SetItemText(BlockOfVMXON, Number_of_repetitions	, TEXT("10")); 
	m_wndList.SetItemText(BlockOfVMXON, Delay_between_repetitions, TEXT("2000"));
	m_wndList.SetItemText(BlockOfVMXON, Number_of_experiments	, TEXT("20"));   
 	m_wndList.SetItemText(BlockOfVMXON, Delay_between_experiments, TEXT("2000")); 
	
	m_wndList.InsertItem(VMXONwithDisabledCache, TEXT("CPUID + NoCache")); 
	m_wndList.SetItemText(VMXONwithDisabledCache, Affinity_thread		, TEXT("1"));
	m_wndList.SetItemText(VMXONwithDisabledCache, Length_of_a_trace		, TEXT("10"));
	m_wndList.SetItemText(VMXONwithDisabledCache, Number_of_measurements, TEXT("1000"));
	m_wndList.SetItemText(VMXONwithDisabledCache, Number_of_repetitions	, TEXT("10"));	
	m_wndList.SetItemText(VMXONwithDisabledCache, Delay_between_repetitions, TEXT("10"));
	m_wndList.SetItemText(VMXONwithDisabledCache, Number_of_experiments	, TEXT("5"));	
 	m_wndList.SetItemText(VMXONwithDisabledCache, Delay_between_experiments, TEXT("100")); 
		
	m_wndList.InsertItem(WalkThroughMemory, TEXT("MemoryWalk")); 
	m_wndList.SetItemText(WalkThroughMemory, Affinity_thread		, TEXT("1"));  
	m_wndList.SetItemText(WalkThroughMemory, Length_of_a_trace		, TEXT("-"));	 
	m_wndList.SetItemText(WalkThroughMemory, Number_of_measurements, TEXT("-"));	 
	m_wndList.SetItemText(WalkThroughMemory, Number_of_repetitions	, TEXT("1"));	
	m_wndList.SetItemText(WalkThroughMemory, Delay_between_repetitions, TEXT("-"));
	m_wndList.SetItemText(WalkThroughMemory, Number_of_experiments	, TEXT("1"));	
	m_wndList.SetItemText(WalkThroughMemory, Delay_between_experiments, TEXT("-")); 
	
	m_wndList.SetCheckboxeStyle(RC_CHKBOX_NORMAL); // Enable checkboxes
	m_wndList.SetEditable(TRUE); // Allow sub-text edit
	m_wndList.SetGridLines(TRUE); // SHow grid lines
	m_wndList.SetCheckboxeStyle(RC_CHKBOX_NORMAL); // Enable checkboxes

	m_wndList.SetSortable(FALSE);
}

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
				if (resourse.hResData )
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

bool CguiDlg :: init_drv()
{
	bool b_res = false;
	resource_functions::RESOURCE resourse = {0}; RtlSecureZeroMemory(&resourse, sizeof(resource_functions::RESOURCE) );
	if (resource_functions :: extract_resource(resourse, MAKEINTRESOURCE(IDR_DETECTOR_DRIVER_BIN_SYSFILE), TEXT("bin")))
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
				m_smCsmgr.init(KARVMM_SERVICENAME_GUI, bin_file);
				m_smCsmgr.stop_driver();
				m_smCsmgr.remove_driver();
				if (m_smCsmgr.add_driver())
				{
					if (m_smCsmgr.start_driver())
					{
						if(m_smCsmgr.chk_service_active())
						{
							if ((m_hNeither = m_smCsmgr.open_device(KARVMM_LINKNAME_GUI)) != NULL)
							{
								b_res = true;
								print :: print_mes("VK is active");
							}
							else
							{   print :: print_last_err("err open device %s ", KARVMM_LINKNAME_GUI);   }
						}
						else
							{   print :: print_last_err("err active driver %s %s", KARVMM_SERVICENAME_GUI, bin_file);   }
					}
					else
					{   
						print :: print_last_err("err start driver %s %s", KARVMM_SERVICENAME_GUI, bin_file);   
						m_smCsmgr.stop_driver();
						m_smCsmgr.remove_driver();
					}
				}else 
				{   
					print :: print_last_err("err add driver %s %s", KARVMM_SERVICENAME_GUI, bin_file);   
					m_smCsmgr.stop_driver();
					m_smCsmgr.remove_driver();
				}
			}
			//DeleteFile(bin_file);
		}
	}

	return b_res;
}

BOOL CguiDlg::OnInitDialog()
{
	BOOL b_res = FALSE;
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	
	/*CLC(); // clear log*/

	print :: init_progressbar();
	init_report_ctrl();
	init_drv();
	m_btnExecTasks.SetWindowText(TEXT("Start"));

	TrayMenu.LoadMenu(IDR_MENU1);

	const TCHAR label[] = TEXT("Measure IET block ");
	AfxGetApp()->m_pMainWnd->SetWindowText(label);

	AfxBeginThread((AFX_THREADPROC)check_thread, NULL);	
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CguiDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CguiDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

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
HCURSOR CguiDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

/************************************************************************/
/*                                                                      */
/************************************************************************/

int g_CurSize = 0;

void CguiDlg :: insert_log(TCHAR *str)
{
	int index = clist_log.InsertString(-1, (LPCTSTR)str);
	clist_log.SelectString(index - 1, (LPCTSTR)str);

	CClientDC dc(this);
	CFont * f = clist_log.GetFont();
	dc.SelectObject(f);
	CSize sz = dc.GetTextExtent(str, _tcslen(str));
	sz.cx += 3 * ::GetSystemMetrics(SM_CXBORDER);
	if(sz.cx > g_CurSize)
	{ /* extend */
		g_CurSize = sz.cx;
		clist_log.SetHorizontalExtent(g_CurSize);
	} /* extend */
}

void CguiDlg::vspf(TCHAR *fmt, va_list argptr)
{
	WCHAR wtmpbuf[MAX_PATH] = {0};
	int cnt = _vsntprintf_s(wtmpbuf, MAX_PATH, fmt, argptr);
	insert_log(wtmpbuf);	
}

void CguiDlg::vspf(char *fmt, va_list argptr)
{
	WCHAR wfmt[MAX_PATH] = {0}, wtmpbuf[MAX_PATH] = {0};
	MultiByteToWideChar(CP_ACP,MB_ERR_INVALID_CHARS, fmt, -1, wfmt, 200);
	// int cnt = _vsntprintf_s(wtmpbuf, MAX_PATH, wfmt, argptr);	
	int res = _vsntprintf_s(wtmpbuf, MAX_PATH-5, MAX_PATH-10, wfmt, argptr);   
	insert_log(wtmpbuf);
}

void CguiDlg::PrintLastError(char *fmt, va_list argptr)
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
void CguiDlg::PrintLastError(TCHAR *fmt, va_list argptr)
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

void set_task_method(TaskForMeasurement & Task)
{
	switch (Task.method_type)
	{
	case BlockOfCPUID:
		Task.ctrl_code = IOCTL_BLOCK_OF_CPUID;
		break;
	case CPUIDwithDisabledCache:
		Task.ctrl_code = IOCTL_BLOCK_OF_CPUID_WITH_DISABLED_CACHE;
		break;
	case WalkThroughMemory:
		Task.ctrl_code = IOCTL_WALK_MEM_NEITHER;

		Task.phys_mem_offset = PHYSMEM_PAGE_SIZE;
		Task.start_high_phys_addr = PHYSMEM_PAGE_SIZE;
		Task.start_low_phys_addr = 0;
		Task.skipbytes = PHYSMEM_PAGE_SIZE; // MmAllocatePagesForMdl
		Task.totalbytes = PHYSMEM_PAGE_SIZE;

		Task.viewsize = PHYSMEM_PAGE_SIZE; // ZwMapViewOfSection
		break;

	case BlockOfVMXON:
		Task.ctrl_code = IOCTL_BLOCK_OF_VMXON;
		break;
	case VMXONwithDisabledCache:
		Task.ctrl_code = IOCTL_BLOCK_OF_VMXON_WITH_DISABLED_CACHE;
		break;
	
	default:{}
	}
}

void CguiDlg :: set_one_task(int Item, TaskForMeasurement & Task)
{
	Task.method_type = (MethodDetectingType)Item;

	set_task_method(Task);

	CString affinity_thread = m_wndList.GetItemText(Item, Affinity_thread);
	_stscanf_s(affinity_thread.GetBuffer(), TEXT("%d"), &Task.affinity);

	CString trace_length = m_wndList.GetItemText(Item, Length_of_a_trace);
	_stscanf_s(trace_length.GetBuffer(), TEXT("%d"), &Task.trace_length);

	CString num_measurements = m_wndList.GetItemText(Item, Number_of_measurements);
	_stscanf_s(num_measurements.GetString(), TEXT("%d"), &Task.num_measurements);

	CString num_repetitions = m_wndList.GetItemText(Item, Number_of_repetitions);
	_stscanf_s(num_repetitions.GetString(), TEXT("%d"), &Task.num_repetitions);

	CString delay_between_rep = m_wndList.GetItemText(Item, Delay_between_repetitions);
	_stscanf_s(delay_between_rep.GetString(), TEXT("%d"), &Task.delay_between_rep);

	CString num_experiment = m_wndList.GetItemText(Item, Number_of_experiments);
	_stscanf_s(num_experiment.GetString(), TEXT("%d"), &Task.num_experiment);

	CString delay_between_exp = m_wndList.GetItemText(Item, Delay_between_experiments);
	_stscanf_s(delay_between_exp.GetString(), TEXT("%d"), &Task.delay_between_exp);
}


void CguiDlg :: exec_one_task(int Item, TaskForMeasurement & Task)
{
	print :: print_mes("Start %s method, trace=%d, NumMeasurements=%d "
		"NumRepetition=%d, DelayRepetition=%d",		
		g_MethodDetectionName[Task.method_type], Task.trace_length, Task.num_measurements,
		Task.num_repetitions, Task.delay_between_rep);

	print :: print_mes("NumExperiment=%d, "
		"DelayExperiment=%d",
		Task.num_experiment,
		Task.delay_between_exp);

	print :: print_mes("Method is working ..");

	bool b_virt = false;
	int n_r_mode = 0;
	int n_v_mode = 0;
	int n_u_mode = 0;
	int pos = 0;
	for (unsigned int i = 0 ; i  < Task.num_experiment ; i++ )
	{
		if ( g_StopFlag  )
			{   break;   }

		if(measuring_trace :: get_one_matrix(m_smCsmgr, m_hNeither, Task, b_virt))
		{
			print :: print_mes(" Matrix # %d of %d", i, Task.num_experiment);
		}
		
		pos = (int)(((float)(i+1)/(float)Task.num_experiment)*100);
		print :: set_progressbar( pos );
		Sleep(Task.delay_between_exp);
	}

	print :: print_mes("Finish %s g_MethodDetectionName Rmode=%d Vmode=%d Unknown=%d ALL=%d", g_MethodDetectionName[Task.method_type], n_r_mode, n_v_mode, n_u_mode, Task.num_experiment);
}

void CguiDlg :: exec_all()
{
	TaskForMeasurement task; RtlSecureZeroMemory(&task, sizeof TaskForMeasurement);
	int i_item = -1;
	while (m_wndList.GetNextItem(i_item, LVNI_ALL) != -1)
	{
		if ( g_StopFlag )
			{   break;   }

		i_item++;
		if (m_wndList.GetCheck(i_item))
		{
			set_one_task(i_item, task);
			exec_one_task(i_item, task);
		}
	}
}

void CguiDlg :: check_thread()
{
	DWORD ExitCode = 0;
	while(true)
	{
		if ((g_StopFlag == false) && h_ThreadTasks && 
			GetExitCodeThread(h_ThreadTasks, &ExitCode) && (ExitCode != STILL_ACTIVE))
		{
			CloseHandle(h_ThreadTasks);
			h_ThreadTasks = INVALID_HANDLE_VALUE;
			print :: set_progressbar(0);
			m_btnExecTasks.SetWindowText(L"Start");
		}
		Sleep(250);
	}
}

void CguiDlg :: start_thread()
{
	g_StopFlag = false;
	h_ThreadTasks = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)(&CguiDlg::exec_all), (PVOID)(NULL), 0, NULL );
	m_btnExecTasks.SetWindowText(L"Stop");
}

void  CguiDlg :: stop_thread()
{
	g_StopFlag = true;
	print :: print_mes("Stopping..");
	print :: print_mes("Please, waiting ..");

	m_btnExecTasks.SetWindowText(L"Stopping..");
	WaitForSingleObject( h_ThreadTasks, 3000 );
	TerminateThread(h_ThreadTasks, 0);
	CloseHandle(h_ThreadTasks);
	h_ThreadTasks = INVALID_HANDLE_VALUE;
	print :: set_progressbar(100);
	Sleep(250);
	m_btnExecTasks.SetWindowText(TEXT("Start"));
	print :: print_mes("Stop");
	print :: set_progressbar(0);
}

void CguiDlg :: OnBnClickedStart()
{
	DWORD ExitCode = 0;
	
	if ( (h_ThreadTasks != INVALID_HANDLE_VALUE) && GetExitCodeThread(h_ThreadTasks, &ExitCode) && (ExitCode == STILL_ACTIVE))
	{
		// STOP
		stop_thread();
	}
	else
	{
		//START
		start_thread();
	}
}

void CguiDlg::OnNMRClickListReportCtl(NMHDR *pNMHDR, LRESULT *pResult)
{
	CMenu *SubMenu = TrayMenu.GetSubMenu(0);
	SubMenu->ModifyMenu(ID_MENU_CLEAR_SELECT, MF_BYCOMMAND, ID_MENU_CLEAR_SELECT, TEXT("Select all"));
	SubMenu->SetDefaultItem(0, true); // Первый пункт меню выделяется "жирным" шрифтом	
	CPoint mouse;
	GetCursorPos(&mouse);	 // Получение позиции курсора
	SubMenu->TrackPopupMenu(0, mouse.x, mouse.y, this);	
}

void CguiDlg :: select_all()
{
	int i_item = -1;
	while ( m_wndList.GetNextItem(i_item, LVNI_ALL) != -1 )
	{
		i_item++;
		m_wndList.SetCheck(i_item, true);
	}
}

void CguiDlg::OnLvnItemchangedListReportCtl(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}
