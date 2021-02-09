
// PMSHJ.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "PMSHJ.h"
#include "MainFrm.h"

#include "PMSHJSet.h"
#include "PMSHJDoc.h"
#include "PMSHJView.h"
#include "Registry.h"
#include "IniFile.h"
#include "Dongle_API.h"
#include "DlgPassword.h"

#pragma comment(lib, "Dongle_s.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CPMSHJApp

BEGIN_MESSAGE_MAP(CPMSHJApp, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, &CPMSHJApp::OnAppAbout)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinAppEx::OnFilePrintSetup)
END_MESSAGE_MAP()


// CPMSHJApp construction

CPMSHJApp::CPMSHJApp()
{
	m_bHiColorIcons			= TRUE;
	m_bSameScanID			= FALSE;
	m_bCam3DAvailable		= FALSE;
	m_bStartingNewMultiItemScan = FALSE;
	m_bIsGross				= TRUE;
	m_nFootSwitchTrigBoxSlNum = 0;

	m_pThreadReadWeight		= NULL;
	m_pThreadCodeScanner	= NULL;
	m_pThreadHikVCode		 = NULL;
	m_pThreadTCP			= NULL;
	m_pThreadFootSwitch		= NULL;
	m_pThreadPrintLabel		= NULL;

	// support Restart Manager
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;
#ifdef _MANAGED
	// If the application is built using Common Language Runtime support (/clr):
	//     1) This additional setting is needed for Restart Manager support to work properly.
	//     2) In your project, you must add a reference to System.Windows.Forms in order to build.
	System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif

	// TODO: replace application ID string below with unique ID string; recommended
	// format for string is CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("PMSHJ.AppID.NoVersion"));

	m_BkgndBrush.CreateSolidBrush(DIMENSIONERO_BACK_COLOR);// DIALOG_BACK_COLOR);// This sets color for all dlg boxes
	m_DlgBkgndBrush.CreateSolidBrush(FINETUNEROI_COLOR);// DIALOG_BACK_COLOR);// This sets color for all dlg boxes
	m_WhiteBrush.CreateSolidBrush(WHITE_COLOR);// DIALOG_BACK_COLOR);// This sets color for all dlg boxes
	m_DBSetBkgndBrush.CreateSolidBrush(RGB(70,140,180));// For all DBSet edit boxes
	m_StaticTextBkgndBrush.CreateSolidBrush(RGB(160,220,230));// DIALOG_BACK_COLOR);// This sets color for all dlg boxes

	m_hReadWeight		= CreateEvent(NULL, FALSE, FALSE, NULL);// Parameter 2 is manual reset - THIS EVENT IS NOT USED AS OF NOW!
	m_hCheckFootSwitch	= CreateEvent(NULL, TRUE, FALSE, NULL);// Manual reset as thread should check swich status in a loop while even is ON
	m_hPrintLabel		= CreateEvent(NULL, FALSE, FALSE, NULL);// 
	m_hPrintTestLabel	= CreateEvent(NULL, FALSE, FALSE, NULL);// 
	m_hSendDataToServer = CreateEvent(NULL, FALSE, FALSE, NULL);// 
	
	m_hEndWtReadThread	= CreateEvent(NULL, TRUE, FALSE, NULL);// Manual Reset as checked at multiple places - Thread sets it when breaking out of its IntiInstance()
	m_hEndScannerThread = CreateEvent(NULL, TRUE, FALSE, NULL);// Manual Reset
	m_hEndTCPThread		= CreateEvent(NULL, TRUE, FALSE, NULL);// Manual Reset
	m_hEndFootSwitchThread = CreateEvent(NULL, TRUE, FALSE, NULL);// Manual Reset
	m_hEndPrintLabelThread = CreateEvent(NULL, TRUE, FALSE, NULL);// Manual Reset
	m_hEndSendDataThread = CreateEvent(NULL, TRUE, FALSE, NULL);// Manual Reset
	
	m_bPrinterPortOpened = m_bReadWtThreadUp = m_bScannerThreadUp = m_bTCPThreadUp = m_bFootSwitchThreadUp = m_bPrintLabelThreadUp = FALSE;

	InitializeCriticalSection(&m_csWeightString);
	InitializeCriticalSection(&m_csScannerString);
	InitializeCriticalSection(&m_csFootSwitchTrigger);

	m_bSaveState = FALSE;// CWinAPPEx member. We do not want to restore the old app window position and placement.
	m_cEndChar = '\r';// Default is CR
}

// The one and only CPMSHJApp object

CPMSHJApp theApp;


// CPMSHJApp initialization

BOOL CPMSHJApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	// Temporary time based Protection
	CTime time = CTime::GetCurrentTime();
	CString dateStr("");
	dateStr.Format(L"%02d%02d", time.GetMonth(), time.GetYear());
	BOOL bLicenseValid = TRUE;
	if (time.GetYear() == 2020)
	{
		if (time.GetMonth() != 12)
			bLicenseValid = FALSE;
	}
	else if (time.GetYear() == 2021)
	{
		if (time.GetMonth() > 1)
			bLicenseValid = FALSE;
	}
	else
		bLicenseValid = FALSE;

	// Set this to include all the common control classes you want to use
	// in your application.
	//InitCtrls.dwICC = ICC_WIN95_CLASSES;
	//InitCommonControlsEx(&InitCtrls);

	::CreateMutexW(NULL, NULL, m_pszExeName);
	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		AfxMessageBox(_T("Application is already running. Cannot start again!"));
		return FALSE;
	}

	CWinAppEx::InitInstance();

	// Initialize OLE libraries
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	EnableTaskbarInteraction(FALSE);

	// AfxInitRichEdit2() is required to use RichEdit control	
	// AfxInitRichEdit2();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("LT\\"));
	LoadStdProfileSettings(4);  // Load standard INI file options (including MRU)

	InitContextMenuManager();

	InitKeyboardManager();

	InitTooltipManager();
	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
		RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CPMSHJDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CPMSHJView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Enable DDE Execute open
	EnableShellOpen();
	RegisterShellFileTypes(TRUE);

	// Dispatch commands specified on the command line.  Will return FALSE if
	// app was launched with /RegServer, /Register, /Unregserver or /Unregister.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;
	if (bLicenseValid == FALSE)
	{
		if (!GetLicenseStatus())
		{
			CDlgPassword dlg;
			if (dlg.DoModal() == IDOK)
			{
				if (dlg.m_strPWD != "5ba5be36gns")
				{
					AfxMessageBox(L"License Expired!");
					return FALSE;
				}
				SetLicenseStatus(TRUE);
			}
		}
	}

	// Multi language code
	m_pLanguage = CLanguage::Instance();
	// Get working path. Set up dll path
	m_strProjectDir = GetProjectDirectory();
	CreateImageDir();

	m_nCmdShow = SW_SHOW;
	// The one and only window has been initialized, so show and update it
	m_pMainWnd->ShowWindow(SW_SHOW);//SW_SHOW / SW_HIDE to show / hide the window
	m_pMainWnd->UpdateWindow();
	IOutputLog(L"\nWindow Up!");
	// call DragAcceptFiles only if there's a suffix
	//  In an SDI app, this should occur after ProcessShellCommand
	// Enable drag/drop open
	m_pMainWnd->DragAcceptFiles();
	m_pFrame = (CMainFrame*)AfxGetMainWnd();

	CenterWindow();
	if (!VerifyDongle())
	{
		AfxMessageBox(L"Dongle Not Found or Incorrect one! Application can't run.");
		return FALSE;
	}
	m_strProjectDir = GetProjectDirectory();
	GetCurMachineNameFromRegistry();
	BOOL bCanStartScan = GetInspParamsFromRegistry();   // Registry Reading Functions. Must call before Init3DBaslerTOFCamera()
	if (!bCanStartScan)
		CreateDefaultRegistry();
	GetOptionsFromRegistry();
	wstring sDLLPath(m_strProjectDir.GetBuffer());

	sDLLPath += _T("\\Lng\\PMSEXOCN.dll");// For Chinese - need to make dll. Look at D:\\Dimensioner\\Multiligual folder
	// Load default system language
	LANGID LAN = GetSystemDefaultLangID();// Hex 809 (decimal 2057 is English), Chinese = Hex 804 (decimal 2052) 
	IOutputLog(L"Before Loading Language!");
	//m_stOptions.Language = ENGLISH;
	if (m_stOptions.Language == ENGLISH)
	{
		bool bSuccess = m_pLanguage->LoadLanguage(2057);
		AfxGetApp()->m_pMainWnd->DrawMenuBar();
		WriteLog(_T("Load Language Successful !!!"));
	}
	else // Load Chinese
	{
		bool bSuccess = m_pLanguage->LoadLanguage(sDLLPath, _T("Chinese"));
		AfxGetApp()->m_pMainWnd->DrawMenuBar();
		if (!bSuccess)
			WriteLog(_T("Load Language Failed!!!"));
	}
	m_pYield = new CYieldData;
	IOutputLog(L"Entering InitCamera()!");
	m_bCam3DAvailable = InitCameraOrbbec();
	// Verify camera sl no.
	if (m_bCam3DAvailable)
	{
		CString strSlNum = GetCameraSlNumber();
		if (strSlNum != L"18101830718")// Excell first machine - 18112130914, At LT - 18101830208 (new camera near my table), 
		{								// 18101831048 (Showroom Cabinet), 18101831260 (Showroom Column type), 18112130209 (new customer (P5)March 2020)
										// 18101831036 (GRAM) 
										// 18101831041 (New customer - 31st Aug 2020)
										// 18101730221 (HJ - 1st Sept 2020)
										// 18101831014 (New customer - 3rd Sept 2020)
										// 18101831040 (High camera system - 12th Sept 2020)
										// 18101830641 (New system - 22nd Sept 2020)
										// 18101830641 (NUC - 22nd Sept 2020)
										// 18101830976 (South Africa - 22nd Sept 2020)
										// 18101830718 (HJ second system - 22nd Sept 2020)
			std::wstring str = m_pLanguage->GetString(IDS_STR_WRONG_CAM_SL);
			CString strTemp(str.c_str());
			AfxMessageBox(strTemp);
			AfxMessageBox(strSlNum);
			return FALSE;
		}
	}
	GetPrintParamsFromRegistry();
	GetFreightParamsFromRegistry();
	GetSendDataParamsFromRegistry();

	CString strMsg;
	if (GetUDFFileNameFromRegistry())
	{
		ReadUDFFile(m_stUDFFileText);
	}
	else
	{
		std::wstring str = m_pLanguage->GetString(IDS_STR_UDF_READ_ERR);
		CString strTemp(str.c_str());
		strMsg = strTemp;
	}
	//IOutputLog(L"After call to ReadUDFFile()");
	m_stUDFFileTextPrint = m_stUDFFileText;// m_stUDFFileTextPrint will get modified in CreatePrintFile()

	::SetPriorityClass(::GetCurrentProcess(), HIGH_PRIORITY_CLASS);
	SetThreadPriority(THREAD_PRIORITY_HIGHEST);

	if (m_pFrame->m_pImageView)
	{
		m_pFrame->m_pImageView->SetDimUnits(m_stOptions.DimUnits);
		if (!m_bCam3DAvailable)
		{
			std::wstring str = m_pLanguage->GetString(IDS_STR_3DCAM_NOT_INIT);
			CString strTemp(str.c_str());
			strMsg += strTemp;
		}
		//IOutputLog(L"After SetDimUnits()");
		m_pFrame->m_pImageView->SetMessage(strMsg);
		// Start Weigh Scale thread
		if (m_stOptions.bWtRqd)
		{
			m_bReadWtThreadUp = StartWeighScaleReadThread();
			//IOutputLog(L"After StartWeighScaleReadThread()");
		}
		else
			IOutputLog(L"Weight Check OFF");
		m_bTCPThreadUp = StartTCPThread();
		if (m_stOptions.bPrintLabel)
		{
			//StartPrintLabelThread();
			//IOutputLog(L"After Call to StartPrintLabelThread()");
		}
		if (m_stServerCommParams.bSendDataToServer)
			StartSendDataThread();
		m_pFrame->m_pImageView->SetButtonsNLabels();
		if (!m_bReadWtThreadUp && m_stOptions.bWtRqd)
		{
			std::wstring str = m_pLanguage->GetString(IDS_STR_WT_NOT_READ);
			CString strTemp(str.c_str());
			AfxMessageBox(strTemp); 

			strMsg = strMsg.Left(strMsg.GetLength());// Removing exclamation mark
			strMsg += L", "; 
			strMsg += strTemp;// ", Weight cannot be read!";
			IOutputLog(L"Weighing scale thread starting error!");

			m_pFrame->m_pImageView->UpdateWeight(0, 0, 0, 0, m_stOptions.strWtUnit, 1);
		}
		IOutputLog(L"After Call to Starting Weighing scale thread");
		if ((m_stOptions.bSerialPortScanner) || (m_stOptions.bHikVisionScanner))
		{
			m_bScannerThreadUp = StartCodeScannerThread();
			IOutputLog(L"After Call to Starting Scanner thread");
		}
		if (m_stOptions.bUseFootSwitch)
		{
			m_bScannerThreadUp = StartFootSwitchThread();
			IOutputLog(L"After Call to Foot Switch thread");
		}

		m_pFrame->m_pImageView->DisplayWtLabels();

		if (m_bCam3DAvailable)// && m_bReadWtThreadUp)
		{
			//std::wstring str(");// m_pLanguage->GetString(IDS_STR_SCAN_UP);
			IOutputLog(L"After call to GetString(IDS_STR_SCAN_UP)");
			CString strTemp(L"Scan ON");// (str.c_str());
			if (bCanStartScan)
			{
				m_pFrame->m_pImageView->InitFlexGrid();
				//strMsg += strTemp;
				//m_pFrame->m_pImageView->SetMessage(strMsg, RGB(255, 0, 0));
				//IOutputLog(L"After call to SetMessage()");
				
				m_pFrame->m_pImageView->OnBnClickedBnScan();
			}
			else
			{
				strMsg += L"Camera height and other parameters may have to be reset. Go to Settings->Inspection Parameters and enter current values.";
				m_pFrame->m_pImageView->SetMessage(strMsg, RGB(255, 0, 0));
			}
		}
		else
		{
			m_pFrame->m_pImageView->InitFlexGrid();
			IOutputLog(L"3D Camera NA");
			m_pFrame->m_pImageView->SetMessage(strMsg, RGB(255,0,0));
			m_pFrame->m_pImageView->SetButtonsState(FALSE);
		}
	}
	IOutputLog(L"At the end of InitInstance()");
	return TRUE;
} // IntiInstance()

void CPMSHJApp::CenterWindow()
{
	// Set Window to Center on screen
	CRect rectDesktop;
	m_pFrame->GetDesktopWindow()->GetWindowRect(&rectDesktop);
	int nXOrig = 0;
	int nYOrig = 0;
	if (rectDesktop.Width() > DISPLAY_WIDTH)
		nXOrig = (rectDesktop.Width() - DISPLAY_WIDTH) / 2;
	if (rectDesktop.Height() > DISPLAY_HEIGHT)
	{
		nYOrig = (rectDesktop.Height() - DISPLAY_HEIGHT) / 2;
	}
	WINDOWPLACEMENT wndPlacement;
	wndPlacement.rcNormalPosition.left = nXOrig;
	wndPlacement.rcNormalPosition.top = nYOrig;
	wndPlacement.rcNormalPosition.right = nXOrig + DISPLAY_WIDTH + 1;
	wndPlacement.rcNormalPosition.bottom = nYOrig + DISPLAY_HEIGHT+ 1;
	m_pFrame->SetWindowPlacement(&wndPlacement);
} // CenterWindow()

BOOL CPMSHJApp::StartWeighScaleReadThread()
{
	// Serial Communication
	GetCOMPortParamsFromRegistry();
	CString str;
	str.Format(L"COM%d", m_stWeighScaleOPParams.nComPortNum);
	std::string strCOMPort(CT2A(str.GetBuffer()));
	m_pThreadReadWeight = new CThreadReadWeight(strCOMPort, m_stWeighScaleParams.WeighScaleType, m_stWeighScaleParams.WeighScaleMode);
	if (!m_pThreadReadWeight->CreateThread(CREATE_SUSPENDED))//Some con'fn
	{
		delete m_pThreadReadWeight;
		m_bReadWtThreadUp = FALSE;
		return FALSE;
	}
	VERIFY(m_pThreadReadWeight->SetThreadPriority(THREAD_PRIORITY_HIGHEST));//THREAD_PRIORITY_ABOVE_NORMAL));
	m_pThreadReadWeight->ResumeThread();
	m_bReadWtThreadUp = TRUE;
	m_pFrame->m_bWeighScaleError = FALSE;
	CIHighResTimer timer;
	timer.HighResSleep(200);// Give time to thread to decide if it was able to open the port.
	//
	return m_bReadWtThreadUp;
} // StartWeighScaleReadThread()

BOOL CPMSHJApp::StopWeighScaleReadThread()
{
	SetEvent(m_hEndWtReadThread);
	MSG msg;
	CIHighResTimer	HighResTimer;
	BOOL bSuccess = HighResTimer.StartHighResTimer();
	double dTime1 = HighResTimer.GetHighResElapsedTimeMsec();
	double dTime2 = dTime1;
	while (m_bReadWtThreadUp)// m_bReadWtThreadUp is set to FALSE if thread has stopped due to whatever reason
	{
		while (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
		//HighResTimer.HighResSleep(100);// This causes problem as it resets the timer
		dTime2 = HighResTimer.GetHighResElapsedTimeMsec();
		// Break out if the wait is too long
		if ((dTime2 - dTime1) > 1000)
			break;
	}
	m_pThreadReadWeight = NULL;
	return !m_bReadWtThreadUp;
} // StopWeighScaleReadThread()

BOOL CPMSHJApp::StartCodeScannerThread()
{
	if (m_stOptions.bSerialPortScanner)
	{
		// Serial Communication
		GetCOMPortParamsFromRegistry();
		CString str;
		str.Format(L"COM%d", m_stScannerPortParams.nComPortNum);
		std::string strCOMPort(CT2A(str.GetBuffer()));
		m_pThreadCodeScanner = new CThreadScanCode(strCOMPort, m_stInspParams.nBarcodeLengthMin, m_stInspParams.nBarcodeLengthMax, m_cEndChar);
		if (!m_pThreadCodeScanner->CreateThread(CREATE_SUSPENDED))
		{
			delete m_pThreadCodeScanner;
			m_bScannerThreadUp = FALSE;
			return FALSE;
		}
		VERIFY(m_pThreadCodeScanner->SetThreadPriority(THREAD_PRIORITY_HIGHEST));//THREAD_PRIORITY_ABOVE_NORMAL));
		m_pThreadCodeScanner->ResumeThread();
		m_bScannerThreadUp = TRUE;
		CIHighResTimer timer;
		timer.HighResSleep(200);// Give time to thread to decide if it was able to open the port.
	}
	else if (m_stOptions.bHikVisionScanner)
	{
		m_pThreadHikVCode = new CThreadHikVCode(m_stInspParams.nBarcodeLengthMin, m_stInspParams.nBarcodeLengthMax, m_cEndChar);
		if (!m_pThreadHikVCode->CreateThread(CREATE_SUSPENDED))
		{
			delete m_pThreadHikVCode;
			m_bScannerThreadUp = FALSE;
			return FALSE;
		}
		VERIFY(m_pThreadHikVCode->SetThreadPriority(THREAD_PRIORITY_HIGHEST));//THREAD_PRIORITY_ABOVE_NORMAL));
		m_pThreadHikVCode->ResumeThread();
		m_bScannerThreadUp = TRUE;
		CIHighResTimer timer;
		timer.HighResSleep(200);// Give time to thread to decide if it was able to start.
	}

	return m_bScannerThreadUp;
} // StartCodeScannerThread()

// Ret val indicates if the func succeeded in stopping thread
BOOL CPMSHJApp::StopCodeScannerThread()
{
	if (m_bScannerThreadUp)
		SetEvent(m_hEndScannerThread);
	else
	{
		m_pThreadCodeScanner = NULL;
		m_pThreadHikVCode = NULL;
		return TRUE;
	}
	MSG msg;
	CIHighResTimer	HighResTimer;
	BOOL bSuccess = HighResTimer.StartHighResTimer();
	double dTime1 = HighResTimer.GetHighResElapsedTimeMsec();
	double dTime2 = dTime1;
	while (m_bScannerThreadUp)// m_bScannerThreadUp is set to FALSE if thread has stopped due to whatever reason
	{
		while (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
		dTime2 = HighResTimer.GetHighResElapsedTimeMsec();
		// Break out if the wait is too long. If this happens m_bScannerThreadUp may still be TRUE.
		// Needs to be handled by calling func.
		if ((dTime2 - dTime1) > 1000)
			break;
	}
	if (!m_bScannerThreadUp)// Thread successfully ended
	{
		m_pThreadCodeScanner = NULL;
		m_pThreadHikVCode = NULL;
	}

	return !m_bScannerThreadUp;
} // StopCodeScannerThread()

BOOL CPMSHJApp::StartTCPThread()
{
	CString str;
	m_pThreadTCP = new CThreadTCP();
	if (!m_pThreadTCP->CreateThread(CREATE_SUSPENDED))//Some con'fn
	{
		delete m_pThreadTCP;
		m_bTCPThreadUp = FALSE;
		return FALSE;
	}
	VERIFY(m_pThreadTCP->SetThreadPriority(THREAD_PRIORITY_ABOVE_NORMAL));
	m_pThreadTCP->ResumeThread();
	m_bTCPThreadUp = TRUE;

	return m_bTCPThreadUp;
} // StartTCPThread()

BOOL CPMSHJApp::StopTCPThread()
{
	if (m_bTCPThreadUp)
		SetEvent(m_hEndTCPThread);
	else
	{
		m_pThreadTCP = NULL;
		return !m_bReadWtThreadUp;
	}
	MSG msg;
	CIHighResTimer	HighResTimer;
	BOOL bSuccess = HighResTimer.StartHighResTimer();
	double dTime1 = HighResTimer.GetHighResElapsedTimeMsec();
	double dTime2 = dTime1;
	while (m_bTCPThreadUp)// m_bTCPThreadUp is set to FALSE if thread has stopped due to whatever reason
	{
		while (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
		dTime2 = HighResTimer.GetHighResElapsedTimeMsec();
		if ((dTime2 - dTime1) > 1000)
			break;
	}
	m_pThreadTCP = NULL;
	return !m_bTCPThreadUp;
} // StopTCPThread()

BOOL CPMSHJApp::StartFootSwitchThread()
{
	CString str;
	str.Format(L"COM%d", m_stFootSwitchPortParams.nComPortNum);
	std::string strCOMPort(CT2A(str.GetBuffer()));
	m_pThreadFootSwitch = new CThreadFootSwitch(strCOMPort, m_stFootSwitchPortParams.nBaudRate);
	if (!m_pThreadFootSwitch->CreateThread(CREATE_SUSPENDED))//Some con'fn
	{
		delete m_pThreadFootSwitch;
		m_bFootSwitchThreadUp = FALSE;
		return FALSE;
	}
	VERIFY(m_pThreadFootSwitch->SetThreadPriority(THREAD_PRIORITY_ABOVE_NORMAL));
	m_pThreadFootSwitch->ResumeThread();
	m_bFootSwitchThreadUp = TRUE;

	return m_bFootSwitchThreadUp;
} // StartFootSwitchThread()

BOOL CPMSHJApp::StopFootSwitchThread()
{
	if (m_bFootSwitchThreadUp)
		SetEvent(m_hEndFootSwitchThread);
	else
	{
		m_pThreadFootSwitch = NULL;
		return !m_bFootSwitchThreadUp;
	}
	MSG msg;
	CIHighResTimer	HighResTimer;
	BOOL bSuccess = HighResTimer.StartHighResTimer();
	double dTime1 = HighResTimer.GetHighResElapsedTimeMsec();
	double dTime2 = dTime1;
	while (m_bFootSwitchThreadUp)// m_bFootSwitchThreadUp is set to FALSE if thread has stopped due to whatever reason
	{
		while (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
		dTime2 = HighResTimer.GetHighResElapsedTimeMsec();
		if ((dTime2 - dTime1) > 1000)
			break;
	}
	if (!m_bFootSwitchThreadUp)
	{
		m_pThreadFootSwitch = NULL;
	}
	return !m_bFootSwitchThreadUp;
} // StopFootSwitchThread()

BOOL CPMSHJApp::StartPrintLabelThread()
{
	CString str;
	m_pThreadPrintLabel = new CThreadPrintLabel(m_stPrintParams, m_hEndPrintLabelThread, m_hPrintLabel, m_hPrintTestLabel);
	if (!m_pThreadPrintLabel->CreateThread(CREATE_SUSPENDED))//Some con'fn
	{
		delete m_pThreadPrintLabel;
		m_bPrintLabelThreadUp = FALSE;
		IOutputLog(L"Could not start print label thread");
		return FALSE;
	}
	VERIFY(m_pThreadPrintLabel->SetThreadPriority(THREAD_PRIORITY_ABOVE_NORMAL));
	m_pThreadPrintLabel->ResumeThread();
	m_bPrintLabelThreadUp = TRUE;

	return m_bPrintLabelThreadUp;
} // StartPrintLabelThread()

BOOL CPMSHJApp::StopPrintLabelThread()
{
	if (m_bPrintLabelThreadUp)
		SetEvent(m_hEndPrintLabelThread);
	else
	{
		m_pThreadPrintLabel = NULL;
		return !m_bPrintLabelThreadUp;
	}
	MSG msg;
	CIHighResTimer	HighResTimer;
	BOOL bSuccess = HighResTimer.StartHighResTimer();
	double dTime1 = HighResTimer.GetHighResElapsedTimeMsec();
	double dTime2 = dTime1;
	while (m_bPrintLabelThreadUp)// m_bPrintLabelThreadUp is set to FALSE if thread has stopped due to whatever reason
	{
		while (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
		dTime2 = HighResTimer.GetHighResElapsedTimeMsec();
		if ((dTime2 - dTime1) > 1000)
			break;
	}
	if (!m_bPrintLabelThreadUp)
	{
		m_pThreadPrintLabel = NULL;
	}
	return !m_bPrintLabelThreadUp;
} // StopPrintLabelThread()

BOOL CPMSHJApp::StartSendDataThread()
{
	CString str;
	m_pThreadSendData = new CThreadSendDataToServer();
	if (!m_pThreadSendData->CreateThread(CREATE_SUSPENDED))//Some con'fn
	{
		delete m_pThreadSendData;
		m_bSendDataThreadUp = FALSE;
		return FALSE;
	}
	VERIFY(m_pThreadSendData->SetThreadPriority(THREAD_PRIORITY_ABOVE_NORMAL));
	m_pThreadSendData->ResumeThread();
	m_bSendDataThreadUp = TRUE;
	IOutputLog(L"Send Data thread started!");
	return m_bSendDataThreadUp;
} // StartSendDataThread()

BOOL CPMSHJApp::StopSendDataThread()
{
	if (m_bSendDataThreadUp)
		SetEvent(m_hEndSendDataThread);
	else
	{
		m_pThreadSendData = NULL;
		return !m_bSendDataThreadUp;
	}
	MSG msg;
	CIHighResTimer	HighResTimer;
	BOOL bSuccess = HighResTimer.StartHighResTimer();
	double dTime1 = HighResTimer.GetHighResElapsedTimeMsec();
	double dTime2 = dTime1;
	while (m_bSendDataThreadUp)// m_bPrintLabelThreadUp is set to FALSE if thread has stopped due to whatever reason
	{
		while (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
		dTime2 = HighResTimer.GetHighResElapsedTimeMsec();
		if ((dTime2 - dTime1) > 1000)
			break;
	}
	if (!m_bSendDataThreadUp)
	{
		m_pThreadSendData = NULL;
	}
	return !m_bSendDataThreadUp;
} // StopSendDataThread()

int CPMSHJApp::ExitInstance()
{
	// Stop the Weight Read thread
	StopWeighScaleReadThread();
	StopCodeScannerThread();
	StopTCPThread();
	StopFootSwitchThread();
	StopPrintLabelThread();
	if (m_bSendDataThreadUp)
		StopSendDataThread();

	Sleep(500);

	if (m_pYield)
		delete m_pYield;

	// Shutdown GDI+.
	//GdiplusShutdown(m_gdiplusToken);//v1.0.0.19

	// Wait for thread to end
	MSG msg;
	CIHighResTimer	HighResTimer;
	HighResTimer.StartHighResTimer();
	double dTime1 = HighResTimer.GetHighResElapsedTimeMsec();
	while (m_bReadWtThreadUp)// m_bReadWtThreadUp is set to FALSE if thread has stopped due to whatever reason
	{
		while (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
		double dTime2 = HighResTimer.GetHighResElapsedTimeMsec();
		// Break out if the wait is too long
		if ((dTime2 - dTime1) > 500)
			break;
	}
	// Close Orbbec camera
	m_device.close();
	openni::OpenNI::shutdown();
	// Close printer port
	//if (m_pSerialCommPrinter)
	//	delete m_pSerialCommPrinter;
	DeleteCriticalSection(&m_csWeightString);
	DeleteCriticalSection(&m_csScannerString);
	DeleteCriticalSection(&m_csFootSwitchTrigger);

	AfxOleTerm(FALSE);

	return CWinAppEx::ExitInstance();
}

// CPMSHJApp message handlers


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// App command to run the dialog
void CPMSHJApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CPMSHJApp customization load/save methods

void CPMSHJApp::PreLoadState()
{
	BOOL bNameValid;
	CString strName;
	bNameValid = strName.LoadString(IDS_EDIT_MENU);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT);
}

void CPMSHJApp::LoadCustomState()
{
}

void CPMSHJApp::SaveCustomState()
{
}

// CPMSHJApp message handlers

BOOL CPMSHJApp::GetCOMPortParamsFromRegistry()
{
	BOOL bResult = TRUE;
	m_stWeighScaleOPParams.nComPortNum	= IGetProfileInt(L"Settings", L"OPComPortNum", 1);
	m_stWeighScaleIPParams.nComPortNum	= IGetProfileInt(L"Settings", L"IPComPortNum", 1);

	m_stWeighScaleIPParams.nBaudRate	= m_stWeighScaleOPParams.nBaudRate	= IGetProfileInt(L"Settings", L"BaudRate", 9600);
	m_stWeighScaleIPParams.nByteSize	= m_stWeighScaleOPParams.nByteSize	= IGetProfileInt(L"Settings", L"ByteSize", 8);
	m_stWeighScaleIPParams.nParity		= m_stWeighScaleOPParams.nParity	= IGetProfileInt(L"Settings", L"Parity", 0);
	m_stWeighScaleIPParams.nStopBit		= m_stWeighScaleOPParams.nStopBit	= IGetProfileInt(L"Settings", L"StopBit", 1);

	m_stPrinterPortParams.nComPortNum	= IGetProfileInt(L"Settings", L"PrinterComPortNum", 1);
	m_stPrinterPortParams.nBaudRate		= IGetProfileInt(L"Settings", L"PrinterBaudRate", 9600);
	m_stPrinterPortParams.nByteSize		= IGetProfileInt(L"Settings", L"PrinterByteSize", 8);
	m_stPrinterPortParams.nParity		= IGetProfileInt(L"Settings", L"PrinterParity", 0);
	m_stPrinterPortParams.nStopBit		= IGetProfileInt(L"Settings", L"PrinterStopBit", 1);

	// Barcode scanner port params
	m_stScannerPortParams.nComPortNum = IGetProfileInt(L"Settings", L"ScannerComPortNum", 1);
	m_stScannerPortParams.nBaudRate = IGetProfileInt(L"Settings", L"ScannerBaudRate", 9600);
	m_stScannerPortParams.nByteSize = IGetProfileInt(L"Settings", L"ScannerByteSize", 8);
	m_stScannerPortParams.nParity = IGetProfileInt(L"Settings", L"ScannerParity", 0);
	m_stScannerPortParams.nStopBit = IGetProfileInt(L"Settings", L"ScannerStopBit", 1);

	// Foot  Switc (Arduino) port params
	m_stFootSwitchPortParams.nComPortNum = IGetProfileInt(L"Settings", L"FootSwitchComPortNum", 6);
	m_stFootSwitchPortParams.nBaudRate = IGetProfileInt(L"Settings", L"FootSwitchBaudRate", 9600);
	m_stFootSwitchPortParams.nByteSize = IGetProfileInt(L"Settings", L"FootSwitchByteSize", 8);
	m_stFootSwitchPortParams.nParity = IGetProfileInt(L"Settings", L"FootSwitchParity", 0);
	m_stFootSwitchPortParams.nStopBit = IGetProfileInt(L"Settings", L"FootSwitchStopBit", 1);

	GetWeighScaleParamsFromRegistry();

	return bResult;
}

BOOL CPMSHJApp::WriteCOMPortParamsToRegistry()
{
	BOOL bResult = TRUE;
	bResult = WriteProfileInt(L"Settings\\", L"OPComPortNum", m_stWeighScaleOPParams.nComPortNum);
	bResult = WriteProfileInt(L"Settings\\", L"IPComPortNum", m_stWeighScaleIPParams.nComPortNum);

	bResult = WriteProfileInt(L"Settings\\", L"BaudRate", m_stWeighScaleOPParams.nBaudRate);
	bResult = WriteProfileInt(L"Settings\\", L"ByteSize", m_stWeighScaleOPParams.nByteSize);
	bResult = WriteProfileInt(L"Settings\\", L"Parity", m_stWeighScaleOPParams.nParity);
	bResult = WriteProfileInt(L"Settings\\", L"StopBit", m_stWeighScaleOPParams.nStopBit);

	bResult = WriteProfileInt(L"Settings\\", L"PrinterComPortNum", m_stPrinterPortParams.nComPortNum);
	bResult = WriteProfileInt(L"Settings\\", L"PrinterBaudRate", m_stPrinterPortParams.nBaudRate);
	bResult = WriteProfileInt(L"Settings\\", L"PrinterByteSize", m_stPrinterPortParams.nByteSize);
	bResult = WriteProfileInt(L"Settings\\", L"PrinterParity", m_stPrinterPortParams.nParity);
	bResult = WriteProfileInt(L"Settings\\", L"PrinterStopBit", m_stPrinterPortParams.nStopBit);

	bResult = WriteProfileInt(L"Settings\\", L"ScannerComPortNum", m_stScannerPortParams.nComPortNum);
	bResult = WriteProfileInt(L"Settings\\", L"ScannerBaudRate", m_stScannerPortParams.nBaudRate);
	bResult = WriteProfileInt(L"Settings\\", L"ScannerByteSize", m_stScannerPortParams.nByteSize);
	bResult = WriteProfileInt(L"Settings\\", L"ScannerParity", m_stScannerPortParams.nParity);
	bResult = WriteProfileInt(L"Settings\\", L"ScannerStopBit", m_stScannerPortParams.nStopBit);

	bResult = WriteProfileInt(L"Settings\\", L"FootSwitchComPortNum", m_stFootSwitchPortParams.nComPortNum);
	bResult = WriteProfileInt(L"Settings\\", L"FootSwitchBaudRate", m_stFootSwitchPortParams.nBaudRate);
	bResult = WriteProfileInt(L"Settings\\", L"SFootSwitchByteSize", m_stFootSwitchPortParams.nByteSize);
	bResult = WriteProfileInt(L"Settings\\", L"FootSwitchParity", m_stFootSwitchPortParams.nParity);
	bResult = WriteProfileInt(L"Settings\\", L"FootSwitchStopBit", m_stFootSwitchPortParams.nStopBit);

	WriteWeighScaleParamsToRegistry();

	return bResult;
}

BOOL CPMSHJApp::GetPrintParamsFromRegistry()
{
	BOOL bResult = TRUE;
	m_stPrintParams.nLabelLengthMM = IGetProfileInt(L"Settings", L"LabelLengthMM", 80);
	m_stPrintParams.nLabelWidthMM = IGetProfileInt(L"Settings", L"LabelWidthMM", 100);
	m_stPrintParams.nCodeHtMM = IGetProfileInt(L"Settings", L"CodeHtMM", 10);
	m_stPrintParams.nNumPrints = IGetProfileInt(L"Settings", L"NumPrints", 1);
	m_stPrintParams.strCorporateName = IGetProfileString(L"Settings", L"CorporateName", L"");
	m_stPrintParams.strProductName = IGetProfileString(L"Settings", L"ProductName", L"");
	m_stPrintParams.strPrinterName = IGetProfileString(L"Settings", L"PrinterName", L"Gprinter GP-3120TU");//GP-1324D//Should be as seen in Windows task manager
	m_stPrintParams.nFontSize = IGetProfileInt(L"Settings", L"FontSize", 1);
	m_stPrintParams.CodeType = (eCodeType)IGetProfileInt(L"Settings", L"CodeType", 1);
	m_stPrintParams.bGetUserOKForPrinting = IGetProfileInt(L"Settings", L"GetUserOKForPrinting", 1);
	m_stPrintParams.bPrintLogo = IGetProfileInt(L"Settings", L"PrintLogo", 0);
	m_stPrintParams.strLogoFile = IGetProfileString(L"Settings", L"LogoFile", L"D:\\LT\\PMSHJ\\LOGOCLEXP.PCX");

	return bResult;
}

BOOL CPMSHJApp::WritePrintParamsToRegistry()
{
	BOOL bResult = TRUE;
	bResult &= WriteProfileInt(L"Settings\\", L"LabelLengthMM", m_stPrintParams.nLabelLengthMM);
	bResult &= WriteProfileInt(L"Settings\\", L"LabelWidthMM", m_stPrintParams.nLabelWidthMM);
	bResult &= WriteProfileInt(L"Settings\\", L"CodeHtMM", m_stPrintParams.nCodeHtMM);
	bResult &= WriteProfileInt(L"Settings\\", L"NumPrints", m_stPrintParams.nNumPrints);
	bResult &= WriteProfileString(L"Settings\\", L"CorporateName", m_stPrintParams.strCorporateName);
	bResult &= WriteProfileString(L"Settings\\", L"ProductName", m_stPrintParams.strProductName);
	bResult &= WriteProfileString(L"Settings\\", L"PrinterName", m_stPrintParams.strPrinterName);
	bResult &= WriteProfileInt(L"Settings\\", L"FontSize", m_stPrintParams.nFontSize);
	bResult &= WriteProfileInt(L"Settings\\", L"CodeType", m_stPrintParams.CodeType);
	bResult &= WriteProfileInt(L"Settings\\", L"GetUserOKForPrinting", m_stPrintParams.bGetUserOKForPrinting);
	bResult &= WriteProfileInt(L"Settings\\", L"PrintLogo", m_stPrintParams.bPrintLogo);
	bResult &= WriteProfileString(L"Settings\\", L"LogoFile", m_stPrintParams.strLogoFile);

	return bResult;
}

BOOL CPMSHJApp::GetFreightParamsFromRegistry()
{
	BOOL bResult = TRUE;
	m_stFreightParams.dSeaFreightRate = _wtof(IGetProfileString(L"Settings", L"SeaFreightRate", L"1.0"));
	m_stFreightParams.dAirFreightRate = _wtof(IGetProfileString(L"Settings", L"AirFreightRate", L"1.0"));
	m_stFreightParams.dWeightFreightRate = _wtof(IGetProfileString(L"Settings", L"WeightFreightRate", L"1.0"));
	m_stFreightParams.FreightMode = (eFreightMode)(IGetProfileInt(L"Settings", L"IsAirFreight", FREIGHT_SEA));

	return bResult;
}

BOOL CPMSHJApp::WriteFreightParamsToRegistry()
{
	BOOL bResult = TRUE;
	CString str;
	str.Format(L"%2.3f", m_stFreightParams.dSeaFreightRate);
	bResult &= WriteProfileString(L"Settings\\", L"SeaFreightRate", str);
	str.Format(L"%2.3f", m_stFreightParams.dAirFreightRate);
	bResult &= WriteProfileString(L"Settings\\", L"AirFreightRate", str);
	str.Format(L"%2.3f", m_stFreightParams.dWeightFreightRate);
	bResult &= WriteProfileString(L"Settings\\", L"WeightFreightRate", str);
	bResult &= WriteProfileInt(L"Settings\\", L"IsAirFreight", (int)(m_stFreightParams.FreightMode));

	return bResult;
}

BOOL CPMSHJApp::GetWeighScaleParamsFromRegistry()
{
	BOOL bResult = TRUE;
	m_stWeighScaleParams.WeighScaleMode = (eWeighScaleMode)IGetProfileInt(L"Settings", L"WeighScaleMode", CONT);
	m_stWeighScaleParams.WeighScaleType = (eWeighScaleType)IGetProfileInt(L"Settings", L"WeighScaleType", EXCELL);
	return bResult;
}

BOOL CPMSHJApp::WriteWeighScaleParamsToRegistry()
{
	BOOL bResult = TRUE;
	bResult = WriteProfileInt(L"Settings\\", L"WeighScaleMode", m_stWeighScaleParams.WeighScaleMode);
	bResult = WriteProfileInt(L"Settings\\", L"WeighScaleType", m_stWeighScaleParams.WeighScaleType);

	return bResult;
}

BOOL CPMSHJApp::GetSendDataParamsFromRegistry()
{
	BOOL bResult = TRUE;
	m_stServerCommParams.strKey = IGetProfileString(L"Settings", L"strKey", L"vbD#&!f&P&24$9x8XC*Q%Z^VJPiKNNg@");
	m_stServerCommParams.strURL = IGetProfileString(L"Settings", L"strURL", L"http://47.114.126.132:9983/system/packing/data");
	m_stServerCommParams.nOperateMode = IGetProfileInt(L"Settings", L"OperateMode", 1);
	m_stServerCommParams.bSendDataToServer = IGetProfileInt(L"Settings", L"SendDataToServer", 1);

	return bResult;
}

BOOL CPMSHJApp::WriteSendDataParamsToRegistry()
{
	BOOL bResult = TRUE;
	bResult &= WriteProfileString(L"Settings\\", L"strKey", m_stServerCommParams.strKey);
	bResult &= WriteProfileString(L"Settings\\", L"strURL", m_stServerCommParams.strURL);
	WriteProfileInt(L"Settings\\", L"OperateMode", m_stServerCommParams.nOperateMode);
	WriteProfileInt(L"Settings\\", L"SendDataToServer", m_stServerCommParams.bSendDataToServer);

	return bResult;
}

int CPMSHJApp::IGetProfileInt(LPCTSTR lpszSection, LPCTSTR lpszEntry, DWORD dwVal)
{
	int nRetVal = 0;
	CRegistry registry;
	registry.OpenRegistry();
	eRegDataType regDataType;
	regDataType = registry.GetRegistryDataType(lpszSection, lpszEntry);
	if (regDataType == REG_DT_INT)
		nRetVal = registry.ReadIntegerValue(lpszSection, lpszEntry, dwVal);
	else if (regDataType == REG_DT_NONE)
	{
		return dwVal;
	}
	else if (regDataType == REG_DT_STRING)
	{
		// Prompt the user with dialog.
		CString strTmp;
		strTmp.Format(L"The registry entry %s\\%s is of incorrect data type\n Default Value Initialized", lpszSection, lpszEntry);
		AfxMessageBox(strTmp);
		registry.DeleteKey(lpszSection, lpszEntry);
		WriteProfileInt(lpszSection, lpszEntry, DEFAULT_INT_VAL);
	}
	return nRetVal;
}//IGetProfileInt

CString CPMSHJApp::IGetProfileString(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCTSTR strValue)
{
	CString strRetVal;
	CRegistry registry;
	registry.OpenRegistry();
	eRegDataType regDataType = registry.GetRegistryDataType(lpszSection, lpszEntry);
	if (regDataType == REG_DT_STRING)
		strRetVal = registry.ReadStringValue(lpszSection, lpszEntry, strValue);
	else if (regDataType == REG_DT_NONE)
		return strValue;
	else if (regDataType == REG_DT_INT)		// Prompt the user.
	{
		CString strTmp;
		strTmp.Format(L"The registry entry %s\\%s is of incorrect data type\n Default Value Initialized", lpszSection, lpszEntry);
		AfxMessageBox(strTmp);
	}

	return strRetVal;
} //IGetProfileString()

// Returns path to exe folder
CString CPMSHJApp::GetProjectDirectory()
{
	CString strProjectDir("");
	GetModuleFileName(NULL, strProjectDir.GetBuffer(MAX_PATH), MAX_PATH);
	strProjectDir.ReleaseBuffer();
	CString tmpStr;
	tmpStr = strProjectDir;
	int index = tmpStr.ReverseFind('\\');
	strProjectDir = tmpStr.Left(index);

	return strProjectDir;
}//GetProjectDirectory()

BOOL CPMSHJApp::InitCameraOrbbec()
{
	openni::Status rc = openni::STATUS_OK;
	rc = openni::OpenNI::initialize();
	if (rc != openni::STATUS_OK)
	{
		CString strError(openni::OpenNI::getExtendedError());
		AfxMessageBox(strError);
		return FALSE;
	}
	if (m_device.open(openni::ANY_DEVICE) != 0)
	{
		CString strError(openni::OpenNI::getExtendedError());
		strError += "\n\t3D Camera not found!";
		IOutputLogString(strError);

		//AfxMessageBox(strError);
		return FALSE;
	}
	rc = m_color.create(m_device, openni::SENSOR_COLOR);
	rc = m_color.start();
	if (rc != openni::STATUS_OK)
	{
		CString strError(openni::OpenNI::getExtendedError());
		strError += "\n3D Camera color stream could not be started!";
		AfxMessageBox(strError);
		IOutputLog(L"Color Stream Could Not be Started");
		openni::OpenNI::shutdown();
		return FALSE;
	}
	puts("Color Stream Started");
	rc = m_depth.create(m_device, openni::SENSOR_DEPTH);
	rc = m_depth.start();
	if (rc != openni::STATUS_OK)
	{
		CString strError(openni::OpenNI::getExtendedError());
		strError += "\n3D Camera depth stream could not be started!";
		IOutputLog(L"Depth Stream Could Not be Started");
		AfxMessageBox(strError);
		openni::OpenNI::shutdown();
		return FALSE;
	}
	//IOutputLog(L"Depth Stream Started");
	openni::VideoMode paramvideo;
	paramvideo.setResolution(640, 480);
	paramvideo.setFps(30);
	paramvideo.setPixelFormat(openni::PIXEL_FORMAT_DEPTH_1_MM);
	m_depth.setVideoMode(paramvideo);
	paramvideo.setPixelFormat(openni::PIXEL_FORMAT_RGB888);
	m_color.setVideoMode(paramvideo);
	//IOutputLog(L"Video Stream Settings ok");
	openni::DeviceInfo devInfo = m_device.getDeviceInfo();
	if (m_device.getSensorInfo(openni::SENSOR_DEPTH) != NULL)
	{
		// Registration of color and depth streams ensures proper graphical representaion.
		m_device.setImageRegistrationMode(openni::IMAGE_REGISTRATION_DEPTH_TO_COLOR);
		// If the depth/color synchronisation is not necessary, start is faster :
		m_device.setDepthColorSyncEnabled(false);
	}

	return TRUE;
} // InitCameraOrbbec()

CString CPMSHJApp::GetCameraSlNumber()
{
	char serialNumber[1024];
	m_device.getProperty(openni::DEVICE_PROPERTY_SERIAL_NUMBER, &serialNumber);

	CString strSlNum(serialNumber);

	return strSlNum;
}

// This func calls WriteScanDataToIniFile() and SavePictures()
BOOL CPMSHJApp::SaveScanData()
{
	BOOL bResult = TRUE;
	// Save Scan Data to Database
	TRY
	{
		bResult = SaveScanDataToDatabase(m_stScanData);
	}CATCH(CDBException, e) 
	{
		// If a database exception occured, show error msg
		AfxMessageBox(L"Database error: " + e->m_strError);
	}
	END_CATCH;

	m_stScanData.RemoveAllItems();
	return bResult;
}

// This func returns current system time string in YY/MM/DD HH:MM:SS mmm format if bAddPuncMarks is TRUE
CString CPMSHJApp::GetSysDateTimeMS(BOOL bAddPuncMarks)
{
	SYSTEMTIME SystemTime;
	GetSystemTime(&SystemTime);
	CTime time = CTime::GetCurrentTime();
	CString strSysDateTimeMS, strYear, strMonth, strDay, strTimeHr, strTimeMin, strTimeSec, strTimeMS;
	// Year (YYYY)
	strYear.Format(L"%d", time.GetYear());// time.GetYear() returns 4 digit number
	// Month (MM)
	strMonth.Format(L"%d", time.GetMonth());
	int nMonthChar = strMonth.GetLength();
	if (nMonthChar == 1)
		strMonth.Format(L"0%d", time.GetMonth());
	//Day (DD)
	strDay.Format(L"%d", time.GetDay());
	int nDayChar = strDay.GetLength();
	if (nDayChar == 1)
		strDay.Format(L"0%d", time.GetDay());
	//TimeHour
	strTimeHr.Format(L"%d", time.GetHour());
	int nHourChar = strTimeHr.GetLength();
	if (nHourChar == 1)
		strTimeHr.Format(L"0%d", time.GetHour());
	//TimeMin
	strTimeMin.Format(L"%d", time.GetMinute());
	int nMinChar = strTimeMin.GetLength();
	if (nMinChar == 1)
		strTimeMin.Format(L"0%d", time.GetMinute());
	//TimeSec
	strTimeSec.Format(L"%d", time.GetSecond());
	int nSecChar = strTimeSec.GetLength();
	if (nSecChar == 1)
		strTimeSec.Format(L"0%d", time.GetSecond());
	strTimeSec.Left(2);
	//TimeMS
	if (SystemTime.wMilliseconds > 99)
		strTimeMS.Format(L"%d", SystemTime.wMilliseconds);
	else if (SystemTime.wMilliseconds > 9)
		strTimeMS.Format(L"0%d", SystemTime.wMilliseconds);
	else
		strTimeMS.Format(L"00%d", SystemTime.wMilliseconds);

	strSysDateTimeMS.Format(L"%s%s%s%s%s%s%s", strYear, strMonth, strDay, strTimeHr, strTimeMin, strTimeSec, strTimeMS);
	if (bAddPuncMarks)
	{
		strSysDateTimeMS.Insert(4, L"/");
		strSysDateTimeMS.Insert(7, L"/");
		strSysDateTimeMS.Insert(10, L" ");// Space
		strSysDateTimeMS.Insert(13, L":");
		strSysDateTimeMS.Insert(16, L":");
		//strSysDateTimeMS.Insert(18, L":");
		strSysDateTimeMS.Insert(19, L" ");// Space
	}
	return strSysDateTimeMS;
}

// This func opens database and writes data in ScanData to it
// If the primary key is already existing it should get overwritten, but doesn't. Need to update - hopefully!
// Returns TRUE if successful otherwise FALSE.
// Assumption: Database exists in given location and can be opened.
// 
BOOL CPMSHJApp::SaveScanDataToDatabase(STScanData ScanData)
{
	m_stInspResultSaved = ScanData.arInspResults[0];
	if (!VerifyDongle())
	{
		AfxMessageBox(L"Dongle Not Found or Incorrect PWD! Application can't scan!.");
		return FALSE;
	}
	if (ScanData.arInspResults[0].strBarcode.GetLength() > m_stInspParams.nBarcodeLengthMax)
		ScanData.arInspResults[0].strBarcode = ScanData.arInspResults[0].strBarcode.Right(m_stInspParams.nBarcodeLengthMax);
	if (m_stOptions.bUseCodeAsTrigger && (ScanData.arInspResults[0].strBarcode.GetLength() < m_stInspParams.nBarcodeLengthMin))
		return FALSE;
	m_stInspResultSaved.bResultPrinted = FALSE;// Set the flag so it is ready for printing
	BOOL bResult = TRUE;
	//STBarcodeParts stBarcodeParts;
	//SplitBarcode(ScanData.arInspResults[0].strBarcode, stBarcodeParts);
	CString SqlString;
	CString sta_name, data_stamp, wk_date, tran_no, pack_no, v_unit, wt_unit, strFootSwitchBoxSlNum;
	double dpack_l, dpack_w, dpack_h, dpack_v, dpack_net, dpack_gross, dpack_tare, dpack_ptare;
	unsigned char upd_flag = 0, del_mark = 0;
	BOOL bEdit = DoesRecordExist(ScanData.arInspResults[0].strBarcode, TRUE);// TRUE is for deleting existing record
	CDatabase PMSDatabase;
	// Update Database Fields
	sta_name	= m_pFrame->m_pImageView->m_pSet->m_sta_name		= m_stDBRecordFields.sta_name;
	data_stamp	= m_pFrame->m_pImageView->m_pSet->m_data_stamp		= m_stDBRecordFields.data_stamp = GetSysDateTimeMS(FALSE);// YYYYMMDDHHNNSSZZZ - Don't add Punctuation marks
	wk_date		= m_pFrame->m_pImageView->m_pSet->m_wk_date			= m_stDBRecordFields.wk_date	= GetSysDateTimeMS(TRUE);// YYYY/MM/DD HH:NN:SS ZZZ
	m_stInspResultSaved.strTime = ScanData.arInspResults[0].strTime = wk_date;
	if (m_stOptions.bUseCodeAsTrigger)
		if (ScanData.arInspResults[0].strBarcode.GetLength() > 12)
			tran_no = m_pFrame->m_pImageView->m_pSet->m_tran_no = m_stDBRecordFields.tran_no = ScanData.arInspResults[0].strBarcode.Left(12);
		else
			tran_no = m_pFrame->m_pImageView->m_pSet->m_tran_no = m_stDBRecordFields.tran_no = ScanData.arInspResults[0].strBarcode;
	else
	{
		strFootSwitchBoxSlNum.Format(L"FW%d", m_nFootSwitchTrigBoxSlNum);
		tran_no = m_pFrame->m_pImageView->m_pSet->m_tran_no = m_stDBRecordFields.tran_no = ScanData.arInspResults[0].strBarcode = strFootSwitchBoxSlNum;
	}
	pack_no		= m_pFrame->m_pImageView->m_pSet->m_pack_no			= m_stDBRecordFields.pack_no	= ScanData.arInspResults[0].strBarcode;
	v_unit		= m_pFrame->m_pImageView->m_pSet->m_v_unit			= m_stDBRecordFields.v_unit		= ScanData.arInspResults[0].strDimensionUnit;
	wt_unit		= m_pFrame->m_pImageView->m_pSet->m_wt_unit			= m_stDBRecordFields.wt_unit	= ScanData.arInspResults[0].strWeightunit;
	dpack_l		= m_pFrame->m_pImageView->m_pSet->m_pack_l			= m_stDBRecordFields.dpack_l	= ScanData.arInspResults[0].dLength;
	dpack_w		= m_pFrame->m_pImageView->m_pSet->m_pack_w			= m_stDBRecordFields.dpack_w	= ScanData.arInspResults[0].dWidth;
	dpack_h		= m_pFrame->m_pImageView->m_pSet->m_pack_h			= m_stDBRecordFields.dpack_h	= ScanData.arInspResults[0].dHeight;
	dpack_v		= m_pFrame->m_pImageView->m_pSet->m_pack_v			= m_stDBRecordFields.dpack_v	= ScanData.arInspResults[0].dVolume;
	dpack_net	= m_pFrame->m_pImageView->m_pSet->m_pack_net		= m_stDBRecordFields.dpack_net	= ScanData.arInspResults[0].dWeightNet;
	dpack_gross = m_pFrame->m_pImageView->m_pSet->m_pack_gross		= m_stDBRecordFields.dpack_gross= ScanData.arInspResults[0].dWeightGr;
	dpack_tare	= m_pFrame->m_pImageView->m_pSet->m_pack_tare		= m_stDBRecordFields.dpack_tare	= ScanData.arInspResults[0].dWeightTare;
	dpack_ptare = m_pFrame->m_pImageView->m_pSet->m_pack_ptare		= m_stDBRecordFields.dpack_ptare= ScanData.arInspResults[0].dFreight;// HJ doesn't need PTare
	upd_flag	= m_pFrame->m_pImageView->m_pSet->m_upd_flag		= m_stFreightParams.FreightMode;// HJ - Being used for freight type
	del_mark	= m_pFrame->m_pImageView->m_pSet->m_del_mark		= 0;
	// KPD - 260320
	CString strData;
	strData.Format(L"%s,%s,%s,%s,%s,%s,%s, %3.2f,%3.2f,%3.2f,%3.2f,%3.2f,%3.2f,%3.2f,%3.2f,",
		sta_name, data_stamp, wk_date, tran_no, pack_no, v_unit, wt_unit,
		dpack_l, dpack_w, dpack_h, dpack_v, dpack_net, dpack_gross, dpack_tare, dpack_ptare);
	IOutputLogString(strData);
	//
	if (!m_pFrame->m_pImageView->m_pSet->IsOpen())
		m_pFrame->m_pImageView->m_pSet->Open();
	m_pFrame->m_pImageView->UpdateData(FALSE);
	TRY
	{
		if (!bEdit)
		{
			m_pFrame->m_pImageView->m_pSet->AddNew();// Prepare for addition of new record
			//IOutputLog(L"After DB AddNew()");
		}
		else
		{
			m_pFrame->m_pImageView->m_pSet->Edit();// Prepare for editing current record
			//IOutputLog(L"After DB Edit()");
		}
	}CATCH(CDBException, e)
	{
		// If a database exception occured, show error msg
		if (!bEdit)
			AfxMessageBox(L"Database AddNew() error: " + e->m_strError);
		else
			AfxMessageBox(L"Database Edit() error: " + e->m_strError);
	}
	END_CATCH;
	TRY
	{
		m_pFrame->m_pImageView->UpdateData(TRUE);// Get new vals into the m_pSet variables! Important. Otherwise doesn't work!
		//IOutputLog(L"After DB UpdateData()");
	}CATCH(CDBException, e)
	{
		// If a database exception occured, show error msg
		AfxMessageBox(L"Database UpdateData() error: " + e->m_strError);
	}
	END_CATCH;
	TRY
	{
		if (m_pFrame->m_pImageView->m_pSet->CanUpdate())
		{
			bResult = m_pFrame->m_pImageView->m_pSet->Update();// Completes addition of new record
			//IOutputLog(L"After DB Update()");
		}
	}CATCH(CDBException, e)
	{
		// If a database exception occured, show error msg
		AfxMessageBox(L"Database CanUpdate() error: " + e->m_strError);
	}
	END_CATCH;
	TRY
	{
		if (!m_pFrame->m_pImageView->m_pSet->IsEOF())
		{
			m_pFrame->m_pImageView->m_pSet->MoveLast();
			//IOutputLog(L"After DB MoveLast()");
		}
		IOutputLog(L"Data Saved to DB");
		// Check the number of records - to be removed later
		/*int nCount = 0;
		while (!m_pFrame->m_pImageView->m_pSet->IsBOF())
		{
			m_pFrame->m_pImageView->m_pSet->MovePrev();
			nCount++;
		}
		TRACE("Number of records: %d\n", nCount);*/
		//m_pFrame->m_pImageView->m_pSet->Close();
		//m_pFrame->m_pImageView->m_pSet->Requery();
	}CATCH(CDBException, e) 
	{
		// If a database exception occured, show error msg
		AfxMessageBox(L"Database IsEOF() error: " + e->m_strError);
	}
	END_CATCH;
	/* Commenting off to avoid unnecessary confusion
	// Log record to csv file
	// Form string to log to csv file
	CString strLogCSV;
	strLogCSV.Format(L"%s,%s,%s,%s,%s,%s,%s,%3.2f,%3.2f,%3.2f,%3.2f,%2.2f,%2.2f,%2.2f,%2.2f,%d,%d\n",
		sta_name, data_stamp, wk_date, tran_no, pack_no, v_unit, wt_unit, dpack_l, dpack_w, dpack_h, dpack_v,
		dpack_net, dpack_gross, dpack_tare, dpack_ptare, upd_flag, del_mark);
	m_pYield->LogData(strLogCSV);
	return TRUE;
	*/
	if (m_stOptions.bUseFootSwitch)
	{
		m_nFootSwitchTrigBoxSlNum++;
		WriteFootSwitchBoxSlNumToRegistry();
	}
	
	return bResult;
}

// Actually Deletes Record does not just set flag.
// Used when adding a new record. Look for an existing record with the givn Pr Key
// Delete it if it exists.
BOOL CPMSHJApp::DoesRecordExist(CString strPrKey, BOOL bDelete)
{
	IOutputLog(L"Checking if Record exists in database");
	BOOL bResult = FALSE;
	TRY
	{
		if (!m_pFrame->m_pImageView->m_pSet->IsOpen())
			m_pFrame->m_pImageView->m_pSet->Open();
		if (!m_pFrame->m_pImageView->m_pSet->IsBOF())
			m_pFrame->m_pImageView->m_pSet->MoveFirst();
		while (!m_pFrame->m_pImageView->m_pSet->IsEOF())
		{
			if (m_pFrame->m_pImageView->m_pSet->m_pack_no == strPrKey)
			{
				if (bDelete)
				{
					m_pFrame->m_pImageView->m_pSet->Delete();
					m_pFrame->m_pImageView->m_pSet->MoveNext();
					bResult = FALSE;
				}
				else
					bResult = TRUE;
				break;
			}
			m_pFrame->m_pImageView->m_pSet->MoveNext();
		}
	}CATCH(CDBException, e)
	{
		// If a database exception occured, show error msg
		AfxMessageBox(L"Database error: " + e->m_strError);
	}
	END_CATCH;
	if (!bResult)// Did not find record with matching pr key
	{
		IOutputLog(L"Record not found in database");
		if (!m_pFrame->m_pImageView->m_pSet->IsBOF())
			m_pFrame->m_pImageView->m_pSet->MoveFirst();
	}
	else
		IOutputLog(L"Record found in database");
	//m_pFrame->m_pImageView->m_pSet->Close();
	return bResult;
}

// Deletes last scan data (in current session? - TBD)
// 
BOOL CPMSHJApp::DeleteLastScanData()
{
	BOOL bResult = TRUE;
	CString strFolder = m_pAutoSaveImage->GetCurScanDataFolder();
	if (m_bSameScanID)// Multi-item scan - remove only the last item entry in txt file and image folder 
	{
		// Remove from STInspResult array
		m_stScanData.RemoveLastItem();
		// Delete folder containing images
		CString strItemFolder;
		strItemFolder.Format(L"\\Item%d", m_stScanData.nNumItems);
		strFolder += strItemFolder;// Needs to be doubly null terminated
		strFolder += L"\0";// Needs to be doubly null terminated
		SHFILEOPSTRUCT shfo = {
			NULL,
			FO_DELETE,
			strFolder,
			NULL,
			FOF_SILENT | FOF_NOERRORUI | FOF_NOCONFIRMATION,
			FALSE,
			NULL,
			NULL };

		SHFileOperation(&shfo);

	}
	else // Remove last scan folder (contains txt file and images)
	{
		strFolder += L"\0";// Needs to be doubly null terminated
		SHFILEOPSTRUCT shfo = {
			NULL,
			FO_DELETE,
			strFolder,
			NULL,
			FOF_SILENT | FOF_NOERRORUI | FOF_NOCONFIRMATION,
			FALSE,
			NULL,
			NULL };

		SHFileOperation(&shfo);
	}

	return bResult;
}

// This func writes results for a scan to ini file.
// A scan may have multiple items and results for each are stored in a separate section
BOOL CPMSHJApp::WriteScanDataToIniFile(CString strFilePath, STScanData ScanData)
{
	BOOL bResult = TRUE;
	CString strSectionName("General");
	CIniFile iniFile;
	iniFile.SetPath(strFilePath);
	//
	iniFile.SetInteger(strSectionName, L"NumItems", ScanData.nNumItems);

	for (int i = 0; i < ScanData.nNumItems; i++)
	{
		strSectionName.Format(L"Item%d", i + 1);
		GetDataToSave(ScanData);  // Saves the data in the ScanData variable 
		//bResult = iniFile.SetCString(strSectionName, L"Length", ScanData.arInspResults[i].strLength);
		//bResult &= iniFile.SetCString(strSectionName, L"Width", ScanData.arInspResults[i].strWidth);
		//bResult &= iniFile.SetCString(strSectionName, L"Height", ScanData.arInspResults[i].strHeight);
		//bResult &= iniFile.SetCString(strSectionName, L"Weight", ScanData.arInspResults[i].strWeight);
		//bResult &= iniFile.SetCString(strSectionName, L"Volume", ScanData.arInspResults[i].strVolume);
		bResult &= iniFile.SetCString(strSectionName, L"Barcode", ScanData.arInspResults[i].strBarcode);
	}

	iniFile.WriteFile();
	return bResult;
}

void CPMSHJApp::GetDataToSave(STScanData &ScanData)
{
	int i = ScanData.nNumItems - 1;
	//ScanData.arInspResults[i].strLength = m_stInspResult.strLength;
	//ScanData.arInspResults[i].strWidth = m_stInspResult.strWidth;
	//ScanData.arInspResults[i].strHeight = m_stInspResult.strHeight;
	//ScanData.arInspResults[i].strWeight = m_stInspResult.strWeight;
	ScanData.arInspResults[i].strVolume = m_stInspResult.strVolume;
	ScanData.arInspResults[i].strBarcode = m_stInspResult.strBarcode;
}

BOOL CPMSHJApp::GetScanDataFromIniFile(CString strFilePath, STScanData& ScanData)
{
	BOOL bResult = TRUE;
	CString strSectionName("General");
	CIniFile iniFile;
	iniFile.SetPath(strFilePath);
	if (!iniFile.ReadFile())
	{
		CString error;
		error = iniFile.error;
		error += (strFilePath + L" - File Not Found");
		AfxMessageBox(error);
		return FALSE;
	}
	ScanData.nNumItems = iniFile.GetInteger(strSectionName, L"NumItems", 0);

	for (int i = 0; i < ScanData.nNumItems; i++)
	{
		strSectionName.Format(L"Item%d", i + 1);
		STInspResult stInspResult;
		stInspResult.dLength = iniFile.GetDouble(strSectionName, L"Length", 0);
		stInspResult.dWidth = iniFile.GetDouble(strSectionName, L"Width", 0);
		stInspResult.dHeight = iniFile.GetDouble(strSectionName, L"Height", 0);
		stInspResult.dWeightNet = iniFile.GetDouble(strSectionName, L"Weight", 0);
		stInspResult.strBarcode = iniFile.GetString(strSectionName, L"Barcode", CString(""));
		stInspResult.bBarcodeReadSuccess = TRUE;
		stInspResult.bDimensionReadSuccess = TRUE;
		stInspResult.bResultReady = TRUE;
		ScanData.arInspResults.Add(stInspResult);
	}

	return bResult;
}

BOOL CPMSHJApp::WriteInspParamsToRegistry()
{
	BOOL bResult = TRUE;
	CString str;
	bResult = WriteProfileInt(L"Settings\\", L"MinObjectHeight", m_stInspParams.nMinObjectHeight);
	bResult = WriteProfileInt(L"Settings\\", L"MaxObjectHeight", m_stInspParams.nMaxObjectHeight);
	str.Format(L"%3.2f", m_stInspParams.d3DCameraHeight);
	bResult = WriteProfileString(L"Settings\\", L"3DCameraHeight", str);
	bResult = WriteProfileInt(L"Settings\\", L"IsCuboid", m_stInspParams.bIsCuboid);
	bResult = WriteProfileInt(L"Settings\\", L"MinZDeviation", m_stInspParams.nMinZDeviation);
	bResult = WriteProfileInt(L"Settings\\", L"MaxZDeviation", m_stInspParams.nMaxZDeviation);
	bResult = WriteProfileInt(L"Settings\\", L"SpotRectSize", m_stInspParams.nSpotRectSize);
	bResult = WriteProfileInt(L"Settings\\", L"UseTracingMethod", m_stInspParams.bUseTracingMethod);
	bResult = WriteProfileInt(L"Settings\\", L"NumFramesToGrab", m_stInspParams.nNumFramesToGrab);
	bResult = WriteProfileInt(L"Settings\\", L"FlipImageVert", m_stInspParams.bFlipImageVert);
	bResult = WriteProfileInt(L"Settings\\", L"FlipImageHorz", m_stInspParams.bFlipImageHorz);
	bResult = WriteProfileInt(L"Settings\\", L"BarcodeLengthMin", m_stInspParams.nBarcodeLengthMin);
	bResult = WriteProfileInt(L"Settings\\", L"BarcodeLengthMax", m_stInspParams.nBarcodeLengthMax);
	bResult = WriteProfileInt(L"Settings\\", L"Accuracy", m_stInspParams.nDsplAccuracy);
	bResult = WriteProfileInt(L"Settings\\", L"CamAlignRectSize", m_stInspParams.nCamAlignRectSize);
	str.Format(L"%2.2f", m_stInspParams.dMinWt);
	bResult = WriteProfileString(L"Settings\\", L"MinWt", str);
	return bResult;
}// WriteInspParamsToRegistry()

BOOL CPMSHJApp::GetInspParamsFromRegistry()
{
	BOOL bResult = TRUE;
	m_stInspParams.nSpotRectSize = IGetProfileInt(L"Settings", L"SpotRectSize", 21);
	if (m_stInspParams.nSpotRectSize == 21)// Default means could not find registry - new machine??
	{
		AfxMessageBox(L"Need to set camera height!\nGo to Settings->Inspection Parameters\n and set camera height.");
		m_stInspParams.nSpotRectSize = 20;
		bResult = FALSE;
	}
	m_stInspParams.nMaxZDeviation = IGetProfileInt(L"Settings", L"MaxZDeviation", 30);
	m_stInspParams.nMinZDeviation = IGetProfileInt(L"Settings", L"MinZDeviation", 20);
	m_stInspParams.bIsCuboid = IGetProfileInt(L"Settings", L"IsCuboid", 1);
	m_stInspParams.bUseTracingMethod = IGetProfileInt(L"Settings", L"UseTracingMethod", 1);
	m_stInspParams.nNumFramesToGrab = IGetProfileInt(L"Settings", L"NumFramesToGrab", 30);

	m_stInspParams.nMinObjectHeight = IGetProfileInt(L"Settings", L"MinObjectHeight", 50);
	m_stInspParams.nMaxObjectHeight = IGetProfileInt(L"Settings", L"MaxObjectHeight", 800);
	m_stInspParams.d3DCameraHeight = _wtof(IGetProfileString(L"Settings", L"3DCameraHeight", L"1200.0f"));
	m_stInspParams.bFlipImageVert = IGetProfileInt(L"Settings", L"FlipImageVert", 0);
	m_stInspParams.bFlipImageHorz = IGetProfileInt(L"Settings", L"FlipImageHorz", 0);
	m_stInspParams.nBarcodeLengthMin = IGetProfileInt(L"Settings", L"BarcodeLengthMin", 16);
	m_stInspParams.nBarcodeLengthMax = IGetProfileInt(L"Settings", L"BarcodeLengthMax", 18);
	m_stInspParams.nDsplAccuracy = IGetProfileInt(L"Settings", L"Accuracy", 5);
	m_stInspParams.nCamAlignRectSize = IGetProfileInt(L"Settings", L"CamAlignRectSize", 240);

	CString str;
	str.Format(L"%2.2f", m_stInspParams.dMinWt);
	str = IGetProfileString(L"Settings\\", L"MinWt", L"0.05");

	return bResult;
}

BOOL CPMSHJApp::GetCurMachineNameFromRegistry()
{
	BOOL bResult = TRUE;
	m_stDBRecordFields.sta_name = IGetProfileString(L"Settings", L"MachineName", L"Default");
	return bResult;
}

BOOL CPMSHJApp::WriteCurMachineNameToRegistry()
{
	BOOL bResult = WriteProfileString(L"Settings\\", L"MachineName", m_stDBRecordFields.sta_name);
	return bResult;
}

BOOL CPMSHJApp::GetFootSwitchBoxSlNumFromRegistry()
{
	BOOL bResult = TRUE;
	m_nFootSwitchTrigBoxSlNum = IGetProfileInt(L"Settings", L"FootSwitchTrigBoxSlNum", 0);

	return bResult;
}

BOOL CPMSHJApp::WriteFootSwitchBoxSlNumToRegistry()
{
	BOOL bResult = WriteProfileInt(L"Settings\\", L"FootSwitchTrigBoxSlNum", m_nFootSwitchTrigBoxSlNum);
	return bResult;
}

BOOL CPMSHJApp::GetOptionsFromRegistry()
{
	BOOL bResult = TRUE;

	m_stOptions.bQty = IGetProfileInt(L"Settings", L"Qty", 1);
	m_stOptions.bBeepSound = IGetProfileInt(L"Settings", L"BeepSound", 1);
	m_stOptions.bWtRqd = IGetProfileInt(L"Settings", L"WtRqd", 1);
	m_stOptions.bSerialPortScanner = IGetProfileInt(L"Settings", L"SerialPortScanner", 0);
	m_stOptions.bHikVisionScanner = IGetProfileInt(L"Settings", L"HikVisionScanner", 1);// For HJ
	m_stOptions.bPrintLabel = IGetProfileInt(L"Settings", L"PrintLabel", 0);
	m_stOptions.bUseCodeAsTrigger = IGetProfileInt(L"Settings", L"QRCode", 1);
	if (m_stOptions.bUseCodeAsTrigger)
		m_stOptions.bUseFootSwitch = FALSE;// IGetProfileInt(L"Settings", L"UseFootSwitch", 0);
	else
		m_stOptions.bUseFootSwitch = TRUE;
	GetFootSwitchBoxSlNumFromRegistry();

	int nType = IGetProfileInt(L"Settings", L"DimUnits", 0);
	if (nType == 0)
		m_stOptions.DimUnits = DIM_MM;
	else if (nType == 1)
		m_stOptions.DimUnits = DIM_CM;
	else if (nType == 2)
		m_stOptions.DimUnits = DIM_M;
	else if (nType == 3)
		m_stOptions.DimUnits = DIM_INCH;

	nType = IGetProfileInt(L"Settings", L"Language", 0);
	if (nType == 0)
		m_stOptions.Language = ENGLISH;
	else if (nType == 1)
		m_stOptions.Language = CHINESE;
	else if (nType == 2)
		m_stOptions.Language = MALAYSIAN;

	m_stOptions.nDecimalUnits = IGetProfileInt(L"Settings", L"DecimalUnits", 0);
	m_stOptions.nWtAccuracy = IGetProfileInt(L"Settings", L"WtAccuracy", 2);
	CString strDefWtUnit = L"kg";
	m_stOptions.strWtUnit = IGetProfileString(L"Settings", L"WtUnit", strDefWtUnit);

	// Code suffix
	int nSuffixChoice = IGetProfileInt(L"Settings", L"CodeSuffixChoice", 0);
	if (nSuffixChoice == 0)
		m_cEndChar = '*';
	else if (nSuffixChoice == 1)
		m_cEndChar = '\r';
	//
	m_stOptions.bShowNetWt = IGetProfileInt(L"Settings", L"ShowNetWt", 1);
	m_stOptions.bShowGrossWt = IGetProfileInt(L"Settings", L"ShowGrossWt", 1);
	m_stOptions.bShowTareWt = IGetProfileInt(L"Settings", L"ShowTareWt", 1);
	m_stOptions.bShowPreTareWt = IGetProfileInt(L"Settings", L"ShowPreTareWt", 1);
	return bResult;
} // GetOptionsFromRegistry()

BOOL CPMSHJApp::WriteOptionsToRegistry()
{
	BOOL bResult = TRUE;

	bResult = WriteProfileInt(L"Settings\\", L"QRCode", m_stOptions.bUseCodeAsTrigger);
	bResult = WriteProfileInt(L"Settings\\", L"Qty", m_stOptions.bQty);
	bResult = WriteProfileInt(L"Settings\\", L"BeepSound", m_stOptions.bBeepSound);
	bResult = WriteProfileInt(L"Settings\\", L"WtRqd", m_stOptions.bWtRqd);
	bResult = WriteProfileInt(L"Settings\\", L"SerialPortScanner", m_stOptions.bSerialPortScanner);
	bResult = WriteProfileInt(L"Settings\\", L"HikVisionScanner", m_stOptions.bHikVisionScanner);
	bResult = WriteProfileInt(L"Settings\\", L"PrintLabel", m_stOptions.bPrintLabel);
	bResult = WriteProfileInt(L"Settings\\", L"UseFootSwitch", m_stOptions.bUseFootSwitch);

	if (m_stOptions.DimUnits == DIM_MM)
		bResult = WriteProfileInt(L"Settings\\", L"DimUnits", 0);
	else if (m_stOptions.DimUnits == DIM_CM)
		bResult = WriteProfileInt(L"Settings\\", L"DimUnits", 1);
	else if (m_stOptions.DimUnits == DIM_M)
		bResult = WriteProfileInt(L"Settings\\", L"DimUnits", 2);
	else if (m_stOptions.DimUnits == DIM_INCH)
		bResult = WriteProfileInt(L"Settings\\", L"DimUnits", 3);

	bResult = WriteProfileString(L"Settings\\", L"WtUnit", m_stOptions.strWtUnit);

	if (m_stOptions.Language == ENGLISH)
		bResult = WriteProfileInt(L"Settings\\", L"Language", 0);
	else if (m_stOptions.Language == CHINESE)
		bResult = WriteProfileInt(L"Settings\\", L"Language", 1);
	else if (m_stOptions.Language == MALAYSIAN)
		bResult = WriteProfileInt(L"Settings\\", L"Language", 2);

	bResult = WriteProfileInt(L"Settings\\", L"DecimalUnits", m_stOptions.nDecimalUnits);
	bResult = WriteProfileInt(L"Settings\\", L"WtAccuracy", m_stOptions.nWtAccuracy);
	if (m_cEndChar == '*')
		bResult = WriteProfileInt(L"Settings\\", L"CodeSuffixChoice", 0);
	else if (m_cEndChar == '\r')
		bResult = WriteProfileInt(L"Settings\\", L"CodeSuffixChoice", 1);

	// Write weight display options
	bResult = WriteProfileInt(L"Settings\\", L"ShowNetWt", m_stOptions.bShowNetWt);
	bResult = WriteProfileInt(L"Settings\\", L"ShowGrossWt", m_stOptions.bShowGrossWt);
	bResult = WriteProfileInt(L"Settings\\", L"ShowTareWt", m_stOptions.bShowTareWt);
	bResult = WriteProfileInt(L"Settings\\", L"ShowPreTareWt", m_stOptions.bShowPreTareWt);

	m_pFrame->m_pImageView->SetDimUnits(m_stOptions.DimUnits);

	return bResult;
}// WriteInspParamsToRegistry()


// Splits barcode string and fills STBarcodeParts structure
// 
BOOL CPMSHJApp::SplitBarcode(CString strBarcode, STBarcodeParts& stBarcodeParts)
{
	if (strBarcode.GetLength() < m_stInspParams.nBarcodeLengthMin)
		return FALSE;

	stBarcodeParts.yyyy = strBarcode.Left(4);
	CString strTrimmed = strBarcode.Right(strBarcode.GetLength() - 4);
	stBarcodeParts.MM = strTrimmed.Left(4);
	strTrimmed = strTrimmed.Right(strTrimmed.GetLength() - 4);
	stBarcodeParts.DD = strTrimmed.Left(4);
	strTrimmed = strTrimmed.Right(strTrimmed.GetLength() - 2);
	stBarcodeParts.tttt = strTrimmed.Left(4);
	strTrimmed = strTrimmed.Right(strTrimmed.GetLength() - 4);
	stBarcodeParts.pppp = strTrimmed.Left(4);
	strTrimmed = strTrimmed.Right(strTrimmed.GetLength() - 4);

	return TRUE;
}

BOOL CPMSHJApp::VerifyDongle()
{
	BOOL bResult = TRUE;

	DWORD dwRet = 0;
	int nCount = 0;
	char AdminPin[] = "0488AC09CBFE8866"; // 0x73 BFB2F38770F8212C Excell 1st Machine; 0x54 A555478B750F5152 Dell machine in showroom
										// 0x14 33299B2FB0BC95F3  Pillar type machine in showroom (NUC)
										// 0FD62B3BAC3E516F New customer (P5) March 2020
										// 65E6F314E57A2012 (seed 0x16) GRAM - 6th May 2020
										// B5F58A329A4A67BE (seed 0x17) new dongle on my PC (7th May 2020)
										// CEC9A8E90EB6E36F (seed 0x18) To customer (31st Aug 2020)
										// 6B6801A738E64268 (seed 0x19) HJ (1st Sept 2020) TeamViewer 417582597
										// A457DF27DDDE73A8 (seed 0x20) To customer  (3rd Sept 2020)
										// 7D51E61614F22BCC (seed 0x21) High Camera system (12th Sept 2020)
										// D218D094F5373DE3 (seed 0x22) New system - NUC (22nd Sept 2020)
										// 27CDB6629D5D8100 (seed 0x23) South Africa (22nd Sept 2020)
										// 0488AC09CBFE8866 (seed 0x24) HJ second system (22nd Sept 2020) TeamViewer 686565281
	DONGLE_HANDLE hDongle = NULL;

	dwRet = Dongle_Enum(NULL, &nCount);
	if (dwRet == 0)
		TRACE("Enum %d Dongle ARM. Success\n", nCount);
	else
	{
		TRACE("Enum %d Dongle ARM. Fail\n", nCount);
		//getchar();
		return FALSE;
	}

	dwRet = Dongle_Open(&hDongle, 0);
	if (dwRet == 0)
		TRACE("Open ARM Dongle. Success\n");
	else
	{
		TRACE("Open ARM Dongle. Fail\n");
		return FALSE;
	}
	int nRemainCount = 0;
	dwRet = Dongle_VerifyPIN(hDongle, FLAG_ADMINPIN, AdminPin, &nRemainCount);
	if (dwRet == 0)
		TRACE("Verify Admin PIN. Success\n");
	else
	{
		TRACE("Verify Admin PIN. Fail\n");
		return FALSE;
	}

	return bResult;
} // VerifyDongle()

BOOL CPMSHJApp::WriteUDFFileNameToRegistry()
{
	BOOL bResult = TRUE;
	CString str;
	str.Format(L"%s", m_stUDFFileText.strUDFFileNameWithPath);
	bResult = WriteProfileString(L"Settings\\", L"UDFFileNameWithPath", str);

	return bResult;
}

BOOL CPMSHJApp::GetUDFFileNameFromRegistry()
{
	m_stUDFFileText.strUDFFileNameWithPath = GetProfileString(L"Settings\\", L"UDFFileNameWithPath", L"Trial.udf");

	if (m_stUDFFileText.strUDFFileNameWithPath.IsEmpty())
		return FALSE;
	else
		return TRUE;
	// Read UDF File and fill m_stUDFFileText
}

// This func opens and reads a udf file and fills the string array
// 
BOOL CPMSHJApp::ReadUDFFile(STUDFFileText& stUDFFileText)
{
	BOOL bResult = TRUE;

	CStdioFile fileLog;
	CString udfFileName = stUDFFileText.strUDFFileNameWithPath;
	CString TitleLine;
	stUDFFileText.ClearAll();
	UINT mode1 = CFile::modeRead | CFile::typeText;
	SetCurrentDirectory(theApp.m_strProjectDir);
	TRY
	{
		if (!fileLog.Open(udfFileName, mode1))//if File already not created throw exception
			AfxThrowFileException(CFileException::fileNotFound);
		fileLog.SeekToBegin();//Set file pointer to the beginning of the file
	}
	CATCH(CFileException, e)//File does not exist
	{
		//AfxMessageBox(L"Could not open UDF File.");
		return FALSE;
	}
	END_CATCH
		CString str;
	while (fileLog.ReadString(str))// Reads a single line of text
		stUDFFileText.arStrings.Add(str);

	int nSize = stUDFFileText.arStrings.GetSize();
	for (int i = 0; i < nSize; i++)
	{
		CString str = stUDFFileText.arStrings[i];
		str += L"\n";
		TRACE(str);
	}
	fileLog.SeekToBegin();
	int dwRead = 0;
	char buffer[2];
	do
	{
		dwRead = fileLog.Read(buffer, 2);
		CStringA strTemp(buffer);
		//if (strTemp.Find(eof))
		if ((dwRead == 0) && (strTemp.Find("\n") != -1))
			stUDFFileText.arStrings.Add(L"\n");
	} while (dwRead > 0);

	return bResult;
}

void CPMSHJApp::WriteLog(CString logstr)
{
	CStdioFile fileLog;
	CString strLogFileName;
	CString strTitleLine;

	strTitleLine = "Log File Created\n\n";

	UINT MODE1 = CFile::modeWrite | CFile::typeText;
	UINT MODE2 = CFile::modeCreate | CFile::modeWrite | CFile::typeText;

	CTime time = CTime::GetCurrentTime();

	//Append date to log file
	CString strDate(""), strTmp("");
	strTmp.Format(CString("%d"), time.GetYear());
	strTmp = strTmp.Right(2);
	strDate.Format(CString("\\Log%02d%02d%s"), time.GetDay(), time.GetMonth(), strTmp);
	strDate += ".txt";
	strLogFileName = m_strProjectDir + CString("\\Log");

	if (CreateDirectory(strLogFileName, NULL) || ERROR_ALREADY_EXISTS == GetLastError())
	{		/* CopyFile(...)*/
	}
	else {		/*Failed to create directory.*/ }

	strLogFileName += strDate;

	TRY
	{
		if (!fileLog.Open(strLogFileName, MODE1))
		AfxThrowFileException(CFileException::fileNotFound);
	fileLog.SeekToEnd(); //Set file pointer to the end of the file
	}
		CATCH(CFileException, e) //if file does not exist
	{
		if (e->m_cause == CFileException::fileNotFound)
		{
			fileLog.Open(strLogFileName, MODE2); //NewFile Created
			fileLog.WriteString(strTitleLine);
		}
	}
	END_CATCH

	CString strLogString;
	strLogString.Format(CString("\n Date: %02d/%02d/%d\tTime: %02d:%02d:%02d\n\n"),
		time.GetDay(), time.GetMonth(), time.GetYear(), //Date
		time.GetHour(), time.GetMinute(), time.GetSecond());  //Time

	fileLog.WriteString(logstr + strLogString);
	fileLog.Close();

} // WriteLog()

void CPMSHJApp::RaiseWeightReadError()
{
	StopWeighScaleReadThread();
	m_pFrame->m_pImageView->SetWeightReadStatus(FALSE);
	
	std::wstring str = m_pLanguage->GetString(IDS_STR_SCALE_ERR);
	CString strTemp(str.c_str());
	AfxMessageBox(strTemp);
	m_pFrame->m_pImageView->m_bWtStatus = FALSE;
}

// Useful when loading exe on a new machine
void CPMSHJApp::CreateDefaultRegistry()
{
	CString str;
	BOOL bResult = WriteProfileInt(L"Settings\\", L"MinObjectHeight", 50);
	bResult = WriteProfileInt(L"Settings\\", L"MaxObjectHeight", 800);
	str.Format(L"%3.2f", 1200.00);
	bResult = WriteProfileString(L"Settings\\", L"3DCameraHeight", str);
	bResult = WriteProfileInt(L"Settings\\", L"IsCuboid", 1);
	bResult = WriteProfileInt(L"Settings\\", L"MinZDeviation", 20);
	bResult = WriteProfileInt(L"Settings\\", L"MaxZDeviation", 30);
	bResult = WriteProfileInt(L"Settings\\", L"SpotRectSize", 20);
	bResult = WriteProfileInt(L"Settings\\", L"UseTracingMethod", 1);
	bResult = WriteProfileInt(L"Settings\\", L"NumFramesToGrab", 1000);
	bResult = WriteProfileInt(L"Settings\\", L"FlipImageVert", 1);
	bResult = WriteProfileInt(L"Settings\\", L"FlipImageHorz", 0);
	bResult = WriteProfileInt(L"Settings\\", L"BarcodeLength", 16);
	str.Format(L"%2.2f", 0.1);
	bResult = WriteProfileString(L"Settings\\", L"MinWt", str);
	// Options
	bResult = WriteProfileInt(L"Settings\\", L"QRCode", 1);
	bResult = WriteProfileInt(L"Settings\\", L"Qty", 1);
	bResult = WriteProfileInt(L"Settings\\", L"BeepSound", 1);
	bResult = WriteProfileInt(L"Settings\\", L"DimUnits", 0);
	bResult = WriteProfileInt(L"Settings\\", L"DecimalUnits", 0);
	bResult = WriteProfileInt(L"Settings\\", L"WtAccuracy", 2);
	bResult = WriteProfileInt(L"Settings\\", L"Language", 0);// English
	bResult = WriteProfileInt(L"Settings\\", L"ShowNetWt", 1);
	bResult = WriteProfileInt(L"Settings\\", L"ShowGrossWt", 1);
	bResult = WriteProfileInt(L"Settings\\", L"ShowTareWt", 1);
	bResult = WriteProfileInt(L"Settings\\", L"ShowPreTareWt", 1);

	CString strDefWtUnit = L"kg";
	WriteProfileString(L"Settings\\", L"WtUnit", strDefWtUnit);
	bResult = WriteProfileString(L"Settings\\", L"MachineName", L"Default");
	// COM port vars - weigh scale ports numbers and params
	bResult = WriteProfileInt(L"Settings\\", L"OPComPortNum", 5);
	bResult = WriteProfileInt(L"Settings\\", L"IPComPortNum", 4);
	bResult = WriteProfileInt(L"Settings\\", L"BaudRate", 9600);
	bResult = WriteProfileInt(L"Settings\\", L"ByteSize", 8);
	bResult = WriteProfileInt(L"Settings\\", L"Parity", 0);
	bResult = WriteProfileInt(L"Settings\\", L"StopBit", 1);
	// Printer port number and params
	bResult = WriteProfileInt(L"Settings\\", L"PrinterComPortNum", 7);
	bResult = WriteProfileInt(L"Settings\\", L"PrinterBaudRate", 9600);
	bResult = WriteProfileInt(L"Settings\\", L"PrinterByteSize", 8);
	bResult = WriteProfileInt(L"Settings\\", L"PrinterParity", 0);
	bResult = WriteProfileInt(L"Settings\\", L"PrinterStopBit", 1);
	// Weigh scale params
	bResult = WriteProfileInt(L"Settings\\", L"WeighScaleMode", 0);
	bResult = WriteProfileInt(L"Settings\\", L"WeighScaleType", 1);
	str.Format(L"%s\\Test.udf", GetProjectDirectory());
	WriteProfileString(L"Settings\\", L"UDFFileNameWithPath", str);
} // CreateDefaultRegistry()

CString	CPMSHJApp::RespondToTCPRequest(CString strRequest)
{
	CString strResponse;

	if (strRequest == 'D')
		strResponse.Format(L"%4.0f %4.0f %4.0f", m_stInspResult.dLength, m_stInspResult.dWidth, m_stInspResult.dHeight);
	else if (strRequest == 'd')
	{
		// Dimension results are stored in mm - need to convert toappropriate units
		if (m_stOptions.DimUnits == DIM_MM)
			strResponse.Format(L"%4.0f %4.0f %4.0f", m_stInspResult.dLength, m_stInspResult.dWidth, m_stInspResult.dHeight);
		else if (m_stOptions.DimUnits == DIM_CM)
			strResponse.Format(L"%3.1f %3.1f %3.1f", m_stInspResult.dLength / 10, m_stInspResult.dWidth / 10, m_stInspResult.dHeight / 10);
		else if (m_stOptions.DimUnits == DIM_M)
			strResponse.Format(L"%1.3f %1.3f %1.3f", m_stInspResult.dLength / 10, m_stInspResult.dWidth / 10, m_stInspResult.dHeight / 10);
		else if (m_stOptions.DimUnits == DIM_INCH)
			strResponse.Format(L"%3.1f %3.1f %3.1f", m_stInspResult.dLength / 25.4, m_stInspResult.dWidth / 25.4, m_stInspResult.dHeight / 25.4);
	}
	if (strRequest == 'W')
	{
		if ((m_strWtUnit == L"Kg") || (m_strWtUnit == L"kg"))
			strResponse.Format(L"%3.2f", m_stInspResult.dWeightGr);
		else if ((m_strWtUnit == L"Gm") || (m_strWtUnit == L"gm"))
			strResponse.Format(L"%3.2f", m_stInspResult.dWeightGr / 1000);
		else if ((m_strWtUnit == L"Lb") || (m_strWtUnit == L"lb"))
			strResponse.Format(L"%3.2f", m_stInspResult.dWeightGr / KG_TO_LBS);
		else if ((m_strWtUnit == L"Oz") || (m_strWtUnit == L"oz"))
			strResponse.Format(L"%3.2f", m_stInspResult.dWeightGr / KG_TO_Oz);
	}
	if (strRequest == 'w')
		strResponse.Format(L"%3.2f", m_stInspResult.dWeightGr); // Weight is already in appropriate units
	if (strRequest == 'T') // Send test string
		strResponse.Format(L"1.23 12.3 123", m_stInspResult.dLength, m_stInspResult.dWidth, m_stInspResult.dHeight);
	if (strRequest == 'Q') // End TCP communication - what does it achieve?
		SetEvent(m_hEndTCPThread);
	if (strRequest == 'K') // Kill application
	{
		SetEvent(m_hEndTCPThread);
		SetEvent(m_hEndWtReadThread);
		m_pFrame->m_bAbortScan = TRUE;
		m_pFrame->m_pImageView->m_HighResTimer.HighResSleep(200);
		m_pFrame->SendMessage(WM_CLOSE, NULL, NULL);// Works!
	}

	return strResponse;
}

BOOL CPMSHJApp::LoadWindowPlacement(CRect& rectNormalPosition, int& nFflags, int& nShowCmd)
{
	BOOL bRet = CWinAppEx::LoadWindowPlacement(rectNormalPosition, nFflags, nShowCmd);
	nShowCmd = SW_HIDE;
	return bRet;
}

BOOL CPMSHJApp::CreateImageDir()
{
	CFileStatus fileStatus;
	SetCurrentDirectory(m_strProjectDir);
	BOOL m_bDirFound = CFile::GetStatus(g_strImagesFolder, fileStatus);
	if (!m_bDirFound)
		CreateDirectory(g_strImagesFolder, NULL);
	return TRUE;
}

CString CPMSHJApp::GetImagesDirectory()
{
	CString str = GetProjectDirectory() + L"\\" + g_strImagesFolder;

	return str;
}

// The default password for this copy is already hardcoded in the program.
BOOL CPMSHJApp::GetLicenseStatus()
{
	return (IGetProfileInt(L"Settings", L"CustCode", 0));
}

// The default password for this copy is already hardcoded in the program.
BOOL CPMSHJApp::SetLicenseStatus(BOOL bOK)
{
	BOOL bResult = WriteProfileInt(L"Settings\\", L"CustCode", bOK);
	return bResult;
}