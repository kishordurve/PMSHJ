
// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "PMSHJ.h"

#include "MainFrm.h"
#include "KeyBoardHookLib.h"
#include "tsclib.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame
static UINT UWM_KEYBOARDSTROKE = ::RegisterWindowMessageA(UWM_KEYBOARD_MSG);
static TCHAR SEPERATOR = '|';

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWndEx)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWndEx)
	ON_WM_CREATE()
	ON_COMMAND(ID_VIEW_CUSTOMIZE, &CMainFrame::OnViewCustomize)
	ON_REGISTERED_MESSAGE(UWM_KEYBOARDSTROKE, OnKeyStroke)
	ON_REGISTERED_MESSAGE(AFX_WM_CREATETOOLBAR, &CMainFrame::OnToolbarCreateNew)
	ON_COMMAND_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnApplicationLook)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnUpdateApplicationLook)
	ON_MESSAGE(WM_UPDATE_WEIGHT, OnUpdateWeight)
	//ON_MESSAGE(WM_UPDATE_CODE, OnUpdateCode)
	ON_MESSAGE(WM_FOOT_SW_TRG_RCD, OnRcdFootSwitchTrigger)
	ON_MESSAGE(WM_FOOT_SW_READ_FAIL, OnRcdFootSwitchReadFail)
	ON_MESSAGE(WM_DATA_LOG_STATUS, OnRcdDataLogStatus)
	ON_MESSAGE(WM_WEIGHT_ERROR, OnRcdWeighScaleError)
	ON_MESSAGE(WM_CODE_READ_STATUS, OnRcdCodeReadStatus)
	ON_WM_CLOSE()
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()

// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
	theApp.m_nAppLook = theApp.GetInt(_T("ApplicationLook"), ID_VIEW_APPLOOK_VS_2008);

	m_pImageView = NULL;
	m_bAbortScan = m_bWeighScaleError = FALSE;
	m_csKey = new TCHAR[sizeof(TCHAR)];

	::InitializeCriticalSection(&m_csLockScanner);

	// Setting CRT date time to match local format
	TCHAR szBuf[128];
	TCHAR szLocale[128];
	::GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SENGLANGUAGE, szBuf, 127);
	_tcscpy(szLocale, szBuf);
	::GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SENGCOUNTRY, szBuf, 127);
	if (_tcsclen(szBuf) != 0)
	{
		_tcscat(szLocale, _T("_"));
		_tcscat(szLocale, szBuf);
	}
	::GetLocaleInfo(LOCALE_SYSTEM_DEFAULT, LOCALE_IDEFAULTANSICODEPAGE, szBuf, 127);
	if (_tcsclen(szBuf) != 0)
	{
		_tcscat(szLocale, _T("."));
		_tcscat(szLocale, szBuf);
	}
	_tsetlocale(LC_ALL, szLocale);
	// Set to TRUE if you want to hide the window
	m_bForceHidden = FALSE;
}

CMainFrame::~CMainFrame()
{
	StopKBCapture();
	::DeleteCriticalSection(&m_csLockScanner);
	//delete[] m_csKey;// Causes crash!
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	// enable quick (Alt+drag) toolbar customization
	CMFCToolBar::EnableQuickCustomization();
	ModifyStyle(WS_CAPTION, 0, 0);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWndEx::PreCreateWindow(cs) )
		return FALSE;
	CRect rectDesktop;
	GetDesktopWindow()->GetWindowRect(&rectDesktop);
	if (rectDesktop.Width() > DISPLAY_WIDTH)
	{
		cs.x = (rectDesktop.Width() - DISPLAY_WIDTH) / 2;
	}
	if (rectDesktop.Height() > DISPLAY_HEIGHT)
	{
		cs.y = (rectDesktop.Height() - DISPLAY_HEIGHT) / 2;
	}

	cs.cx = DISPLAY_WIDTH;
	cs.cy = DISPLAY_HEIGHT;
	cs.style = WS_OVERLAPPED | WS_SYSMENU | WS_BORDER | WS_CAPTION;

	if (cs.hMenu != NULL)
	{
		::DestroyMenu(cs.hMenu);      // delete menu if loaded
		cs.hMenu = NULL;              // no menu for this window
	}

	return TRUE;
}

// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWndEx::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWndEx::Dump(dc);
}
#endif //_DEBUG


// CMainFrame message handlers

void CMainFrame::OnViewCustomize()
{
	CMFCToolBarsCustomizeDialog* pDlgCust = new CMFCToolBarsCustomizeDialog(this, TRUE /* scan menus */);
	pDlgCust->Create();
}

LRESULT CMainFrame::OnToolbarCreateNew(WPARAM wp,LPARAM lp)
{
	LRESULT lres = CFrameWndEx::OnToolbarCreateNew(wp,lp);
	if (lres == 0)
	{
		return 0;
	}

	CMFCToolBar* pUserToolbar = (CMFCToolBar*)lres;
	ASSERT_VALID(pUserToolbar);

	BOOL bNameValid;
	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);

	pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);
	return lres;
}

void CMainFrame::OnApplicationLook(UINT id)
{
	CWaitCursor wait;

	theApp.m_nAppLook = id;

	switch (theApp.m_nAppLook)
	{
	case ID_VIEW_APPLOOK_WIN_2000:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManager));
		break;

	case ID_VIEW_APPLOOK_OFF_XP:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOfficeXP));
		break;

	case ID_VIEW_APPLOOK_WIN_XP:
		CMFCVisualManagerWindows::m_b3DTabsXPTheme = TRUE;
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));
		break;

	case ID_VIEW_APPLOOK_OFF_2003:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2003));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	case ID_VIEW_APPLOOK_VS_2005:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2005));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	case ID_VIEW_APPLOOK_VS_2008:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2008));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	case ID_VIEW_APPLOOK_WINDOWS_7:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows7));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	default:
		switch (theApp.m_nAppLook)
		{
		case ID_VIEW_APPLOOK_OFF_2007_BLUE:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_LunaBlue);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_BLACK:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_ObsidianBlack);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_SILVER:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Silver);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_AQUA:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Aqua);
			break;
		}

		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2007));
		CDockingManager::SetDockingMode(DT_SMART);
	}

	RedrawWindow(NULL, NULL, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW | RDW_FRAME | RDW_ERASE);

	theApp.WriteInt(_T("ApplicationLook"), theApp.m_nAppLook);
}

void CMainFrame::OnUpdateApplicationLook(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(theApp.m_nAppLook == pCmdUI->m_nID);
}

void CMainFrame::StartKBCapture()
{
	//IOutputLog("Entering UpdateDisplayText() from StartKBCapture()");
	//UpdateDisplayText();		

	m_hinstDLL = LoadLibraryA(CT2A(L"KeyBoardHookLib.dll"));
	if (m_hinstDLL != NULL)
	{
		instkbhook = (inshook)GetProcAddress(m_hinstDLL, "InstallKeyBoardHook");
		if (instkbhook != NULL)
			instkbhook(m_hWnd);
		m_bKBCapture = TRUE;
		//IOutputLog(L"StartKBCapture() successful!");
	}
	else
		IOutputLog(L"Could not load dll in StartKBCapture()");
}

void CMainFrame::StopKBCapture()
{
	m_bKBCapture = FALSE;

	if (m_hinstDLL != NULL)
	{
		uninstkbhook = (inshook)GetProcAddress(m_hinstDLL, "UnInstallKeyBoardHook");
		if (uninstkbhook != NULL)
			uninstkbhook(m_hWnd);
		IOutputLog(L"StopKBCapture() successful!");
		//IOutputLog("Entering UpdateDisplayText() from StopKBCapture()");
		//UpdateDisplayText();
	}
	//IOutputLog("Entering UpdateDisplayText() from StopKBCapture()");
	//UpdateDisplayText();		
}

void CMainFrame::ReplaceString(CString& csString_io, int& nPos_io)
{
	csString_io.SetAt(nPos_io, _T('1'));
}

LRESULT CMainFrame::OnKeyStroke(WPARAM wParam, LPARAM lParam)
{
	//m_nTimeStamp = GetTickCount() - m_nTimeStamp;
	if (m_nStartTimeSecs == -1)
	{
		CTime time = CTime::GetCurrentTime();
		m_nStartTimeSecs = time.GetSecond();
		//IOutputLog("Starting New Scan");
	}
	CString csKeyTyped;
	CString csKeyStatus;
	csKeyStatus = "0??????????";
	if ((wParam == VK_SPACE) || (wParam == VK_RETURN) ||
		(wParam >= 0x2f) && (wParam <= 0x100))
	{
		// CAPSTATE,LSHIFT,LCTRL, LWINDOW, LALT, RALT, RWIN, MENU,RCTRL, RSHIFT,NUMLCOK
		SHORT nCapital = GetKeyState(VK_CAPITAL);
		// Left side
		//****************************************************************************
		SHORT nVirtKeyLeftShift = GetAsyncKeyState(VK_LSHIFT);
		SHORT nLeftControl = GetAsyncKeyState(VK_LCONTROL);
		// Hard Coded
		SHORT nLWindows = GetAsyncKeyState(VK_LWIN);
		SHORT nLAlt = GetAsyncKeyState(VK_LMENU);
		//****************************************************************************		
		// Right Side
		SHORT nRAlt = GetAsyncKeyState(VK_RMENU);
		// Hard coded
		SHORT nRWin = GetAsyncKeyState(VK_RWIN);
		SHORT nMenu = GetAsyncKeyState(VK_APPS);
		SHORT nRightControl = GetAsyncKeyState(VK_LCONTROL);
		SHORT nVirtKeyRightShift = GetAsyncKeyState(VK_RSHIFT);
		//*****************************************************************************

		// Num Lock
		SHORT nNumLock = GetKeyState(VK_NUMLOCK);

		int nCurrentPos = 0;
		if (0 != nCapital)
		{
			ReplaceString(csKeyStatus, nCurrentPos);
		}
		nCurrentPos = csKeyStatus.Find(SEPERATOR, nCurrentPos);
		nCurrentPos++;
		if (0 != nVirtKeyLeftShift)
		{
			ReplaceString(csKeyStatus, nCurrentPos);
		}
		nCurrentPos = csKeyStatus.Find(SEPERATOR, nCurrentPos);
		nCurrentPos++;
		if (0 != nLeftControl)
		{
			ReplaceString(csKeyStatus, nCurrentPos);
		}
		nCurrentPos = csKeyStatus.Find(SEPERATOR, nCurrentPos);
		nCurrentPos++;
		if (0 != nLWindows)
		{
			ReplaceString(csKeyStatus, nCurrentPos);
		}
		nCurrentPos = csKeyStatus.Find(SEPERATOR, nCurrentPos);
		nCurrentPos++;
		if (0 != nLAlt)
		{
			ReplaceString(csKeyStatus, nCurrentPos);
		}
		nCurrentPos = csKeyStatus.Find(SEPERATOR, nCurrentPos);
		nCurrentPos++;
		if (0 != nRAlt)
		{
			ReplaceString(csKeyStatus, nCurrentPos);
		}
		nCurrentPos = csKeyStatus.Find(SEPERATOR, nCurrentPos);
		nCurrentPos++;
		if (0 != nRWin)
		{
			ReplaceString(csKeyStatus, nCurrentPos);
		}
		nCurrentPos = csKeyStatus.Find(SEPERATOR, nCurrentPos);
		nCurrentPos++;
		if (0 != nMenu)
		{
			ReplaceString(csKeyStatus, nCurrentPos);
		}
		nCurrentPos = csKeyStatus.Find(SEPERATOR, nCurrentPos);
		nCurrentPos++;
		if (0 != nRightControl)
		{
			ReplaceString(csKeyStatus, nCurrentPos);
		}
		nCurrentPos = csKeyStatus.Find(SEPERATOR, nCurrentPos);
		nCurrentPos++;
		if (0 != nVirtKeyRightShift)
		{
			ReplaceString(csKeyStatus, nCurrentPos);
		}
		nCurrentPos = csKeyStatus.Find(SEPERATOR, nCurrentPos);
		nCurrentPos++;
		if (0 != nNumLock)
		{
			ReplaceString(csKeyStatus, nCurrentPos);
		}
		GetKeyNameText(lParam, m_csKey, 4);
		csKeyTyped = m_csKey;
		// Ignore unwanted characters - KPD - 121019
		if ((csKeyTyped.Find(L"F1") != -1) || (csKeyTyped.Find(L"F2") != -1) || (csKeyTyped.Find(L"F3") != -1) || (csKeyTyped.Find(L"F4") != -1) ||
			(csKeyTyped.Find(L"F5") != -1) || (csKeyTyped.Find(L"F6") != -1) || (csKeyTyped.Find(L"F7") != -1) || (csKeyTyped.Find(L"F8") != -1) ||
			(csKeyTyped.Find(L"F9") != -1) || (csKeyTyped.Find(L"F10") != -1) || (csKeyTyped.Find(L"Num") != -1) || (csKeyTyped.Find(L"NUM") != -1))
			return 1;
		CString strTemp;
		strTemp.Format(L"%s\n", csKeyTyped);
		TRACE(strTemp);
		// Name of space key is Spa
		if (csKeyTyped.Find(L"Sp") >= 0)
			csKeyTyped = " ";
		if ((csKeyTyped.Find(L"Ent") >= 0) || (csKeyTyped.Find(L"ENT") >= 0) || (csKeyTyped.Find(L"CR") >= 0) || (csKeyTyped.Find(L"cr") >= 0) || (csKeyTyped.Find(L"LF") >= 0) || (csKeyTyped.Find(L"lf") >= 0))
		{
			//IOutputLog(L"Found CR Key");
			csKeyTyped = "*";
		}

		//csKeyTyped += SEPERATOR;
		//csKeyStatus += SEPERATOR;
	}
	CString csKeyFormat;
	CString csWParam;
	csWParam.Format(L"%x", wParam);
	CString csTime;
	csTime.Format(L"%d", m_nTimeStamp);
	csKeyFormat = "KeyBD";
	csKeyFormat += csKeyTyped;
	csKeyFormat += csKeyStatus;
	csKeyFormat += csWParam;
	csKeyFormat += csTime;
	csKeyFormat += "\n";
	if (!csKeyTyped.IsEmpty())
	{
		// Check for valid key character
		if ((csKeyTyped.Find('A') >= 0) || (csKeyTyped.Find('B') >= 0) || (csKeyTyped.Find('C') >= 0) || (csKeyTyped.Find('D') >= 0) || (csKeyTyped.Find('E') >= 0) || (csKeyTyped.Find('F') >= 0) ||
			(csKeyTyped.Find('G') >= 0) || (csKeyTyped.Find('H') >= 0) || (csKeyTyped.Find('I') >= 0) || (csKeyTyped.Find('J') >= 0) || (csKeyTyped.Find('K') >= 0) || (csKeyTyped.Find('L') >= 0) ||
			(csKeyTyped.Find('M') >= 0) || (csKeyTyped.Find('N') >= 0) || (csKeyTyped.Find('O') >= 0) || (csKeyTyped.Find('P') >= 0) || (csKeyTyped.Find('Q') >= 0) || (csKeyTyped.Find('R') >= 0) ||
			(csKeyTyped.Find('S') >= 0) || (csKeyTyped.Find('T') >= 0) || (csKeyTyped.Find('U') >= 0) || (csKeyTyped.Find('V') >= 0) || (csKeyTyped.Find('W') >= 0) || (csKeyTyped.Find('X') >= 0) ||
			(csKeyTyped.Find('Y') >= 0) || (csKeyTyped.Find('Z') >= 0) ||
			(csKeyTyped.Find('a') >= 0) || (csKeyTyped.Find('b') >= 0) || (csKeyTyped.Find('c') >= 0) || (csKeyTyped.Find('d') >= 0) || (csKeyTyped.Find('e') >= 0) || (csKeyTyped.Find('f') >= 0) ||
			(csKeyTyped.Find('g') >= 0) || (csKeyTyped.Find('h') >= 0) || (csKeyTyped.Find('i') >= 0) || (csKeyTyped.Find('j') >= 0) || (csKeyTyped.Find('k') >= 0) || (csKeyTyped.Find('l') >= 0) ||
			(csKeyTyped.Find('m') >= 0) || (csKeyTyped.Find('n') >= 0) || (csKeyTyped.Find('o') >= 0) || (csKeyTyped.Find('p') >= 0) || (csKeyTyped.Find('q') >= 0) || (csKeyTyped.Find('r') >= 0) ||
			(csKeyTyped.Find('s') >= 0) || (csKeyTyped.Find('t') >= 0) || (csKeyTyped.Find('u') >= 0) || (csKeyTyped.Find('v') >= 0) || (csKeyTyped.Find('w') >= 0) || (csKeyTyped.Find('x') >= 0) ||
			(csKeyTyped.Find('y') >= 0) || (csKeyTyped.Find('z') >= 0) ||
			(csKeyTyped.Find('1') >= 0) || (csKeyTyped.Find('2') >= 0) || (csKeyTyped.Find('3') >= 0) || (csKeyTyped.Find('4') >= 0) || (csKeyTyped.Find('5') >= 0) || (csKeyTyped.Find('6') >= 0) ||
			(csKeyTyped.Find('7') >= 0) || (csKeyTyped.Find('8') >= 0) || (csKeyTyped.Find('9') >= 0) || (csKeyTyped.Find('0') >= 0) || (csKeyTyped.Find('*') >= 0) || (csKeyTyped.Find('-') >= 0))
		{
			//if (!m_bBarcodeReceived)
			{
				//CString str;
				//str.Format(L"Key Rcd: %s", csKeyTyped);
				//IOutputLogString(str);
				LockBuffer();
				m_csArray.Add(csKeyTyped);
				CString csLineContent;
				int nSize = m_csArray.GetSize();
				m_strTotal.Empty();
				for (int nID = 0; nID<nSize; nID++)
				{
					csLineContent = m_csArray.GetAt(nID);// + L"\n";
					//TRACE(csLineContent);
					m_strTotal += m_csArray.GetAt(nID);
					if (m_strTotal.Find('*') != -1)
						break;
				}
				UnLockBuffer();
			}
		}
	}

	return 1;
}

// Creates UDF File from the string passed to it
// Used when creating a new udf file (for a new printer)
void CMainFrame::CreateUDFFile(STUDFFileText& stUDFFileText)
{
	CStdioFile fileLog;
	CString udfFileName = stUDFFileText.strUDFFileNameWithPath;
	CString TitleLine;

	UINT mode1 = CFile::modeCreate | CFile::modeWrite | CFile::typeText;// modeCreate sets file length to zero i.e. wipes out contents!
	UINT mode2 = CFile::modeCreate | CFile::modeWrite | CFile::typeText;
	//SetCurrentDirectory(theApp.m_strProjectDir);
	TRY
	{
		if (!fileLog.Open(udfFileName, mode1))//if File already not created throw exception
			AfxThrowFileException(CFileException::fileNotFound);
		fileLog.SeekToBegin();//Set file pointer to the beginning of the file
	}
	CATCH(CFileException, e)//File does not exist
	{
		if (e->m_cause == CFileException::fileNotFound)
		{
			fileLog.Open(udfFileName, mode2);	//New File created
		}
	}
	END_CATCH
		for (int i = 0; i < stUDFFileText.arStrings.GetSize(); i++)
		{
			
			//fileLog.WriteString(stUDFFileText.arStrings[i]);
			CString str = stUDFFileText.arStrings[i];
			if ((str == "\r") || (str == "\n"))
			{
				str = "";
				//if (i == (stUDFFileText.arStrings.GetSize() - 1))
					fileLog.WriteString(L"\n");
			}
			else
				fileLog.WriteString(stUDFFileText.arStrings[i]);
			// Need to append \n otherwise a continuous string gets written
			if ((i < (stUDFFileText.arStrings.GetSize() - 1)))// && (str.GetLength() > 0))// No need to add LF if line empty
			{
				// Add LF only if the next line is not an empty LF
				CString str1 = stUDFFileText.arStrings[i + 1];
				if ((str1 == "\r") || (str1 == "\n"))
					str1 = "";
				if (str1.GetLength() > 0)
					fileLog.WriteString(L"\n");
			}
		}

	fileLog.Close();
}

// Converting a hex number to a decimal number
// 
int CMainFrame::HexToDecimal(char* hex, int nNumDigits)
{
	int val = 0, decimal = 0, len;
	CString str(hex);
	len = nNumDigits;
	for (int i = 0; i < str.GetLength(); i++)
	{
		/* Find the decimal representation of hex[i] */
		if (hex[i] >= '0' && hex[i] <= '9')
			val = hex[i] - 48;
		else if (hex[i] >= 'a' && hex[i] <= 'f')
			val = hex[i] - 97 + 10;
		else if (hex[i] >= 'A' && hex[i] <= 'F')
			val = hex[i] - 65 + 10;

		len--;
		decimal += val * pow(16, len);
	}
	return decimal;
}

// Replaces '|**' hex string with decimal values
// No replacement if illegal hex (G,H... or +,-,(,) etc.) characters found
// Case sensitive - a,b,c... not allowed
// 
BOOL CMainFrame::ReplaceHexVars(STUDFFileText& stUDFFileText, STInspResult stInspResult)
{
	BOOL bResult = TRUE;
	CString strSub, strNew;
	int nDecimalNumber;
	char hexCharArray[2];
	for (int i = 0; i < stUDFFileText.arStrings.GetSize(); i++)
	{
		int nIndex = stUDFFileText.arStrings[i].Find(L"|");
		while (nIndex != -1)
		{
			int nLength = stUDFFileText.arStrings[i].GetLength();
			if (nLength >= (nIndex + 3))
			{
				strSub = stUDFFileText.arStrings[i].Left(nIndex + 3);
				strSub = strSub.Right(3);// first character is '|'
				// Check if both degits are valid hex numbers
				if ((((strSub[1] >= '0') && (strSub[1] <= '9')) || ((strSub[1] >= 'A') && (strSub[1] <= 'F'))) &&
					(((strSub[2] >= '0') && (strSub[2] <= '9')) || ((strSub[2] >= 'A') && (strSub[2] <= 'F'))))
				{
					hexCharArray[0] = strSub[1];
					hexCharArray[1] = strSub[2];
					// Convert it to decimal
					nDecimalNumber = HexToDecimal(hexCharArray, 2);
					// Replace in string - we need a char and not int as per Excell Yamato
					strNew.Format(L"%c", nDecimalNumber);
					stUDFFileText.arStrings[i].Replace(strSub, strNew);
				}
				// Look for more control characters starting at current index
				nIndex = stUDFFileText.arStrings[i].Find(L"|", nIndex);
			}
			else
				nIndex = -1;
		}
	}

	return bResult;
} // ReplaceHexVars()

BOOL CMainFrame::ReplaceSystVars(STUDFFileText& stUDFFileText, STInspResult stInspResult)
{
	BOOL bResult = TRUE;
	for (int i = 0; i < stUDFFileText.arStrings.GetSize(); i++)
	{
		int nIndex = stUDFFileText.arStrings[i].Find(L"@");
		if (nIndex != -1)
		{
			while (nIndex != -1)
			{
				if (stUDFFileText.arStrings[i].GetLength() >= (nIndex + 2))
				{
					if (stUDFFileText.arStrings[i][nIndex + 1] == '2')// System Date
					{
						// Replace @2 with system date
						stUDFFileText.arStrings[i].Replace(L"@2", GetSystemDateString());
					}
					else if (stUDFFileText.arStrings[i][nIndex + 1] == '3')// System Time
					{
						// Replace @3 with data_stamp
						stUDFFileText.arStrings[i].Replace(L"@3", GetSystemTimeString());
					}
					else if (stUDFFileText.arStrings[i][nIndex + 1] == '4')// Machine Name
					{
						// Replace @4 with machine name
						stUDFFileText.arStrings[i].Replace(L"@4", m_pImageView->m_pSet->m_sta_name);
					}
					else if (stUDFFileText.arStrings[i][nIndex + 1] == '5')// Measuring Data Time Name
					{
						// Replace @5 with measuring data time
						stUDFFileText.arStrings[i].Replace(L"@5", m_pImageView->m_pSet->m_data_stamp);
					}
					else if (stUDFFileText.arStrings[i][nIndex + 1] == '6')// Transporte No.
					{
						// Replace @6 with trans_no
						stUDFFileText.arStrings[i].Replace(L"@6", m_pImageView->m_pSet->m_tran_no);
					}
					else if (stUDFFileText.arStrings[i][nIndex + 1] == '7')// Package No.
					{
						// Replace @7 with package no.
						stUDFFileText.arStrings[i].Replace(L"@7", m_pImageView->m_pSet->m_pack_no);
					}
					else if (stUDFFileText.arStrings[i][nIndex + 1] == '9')// Package Length
					{
						// Replace @9 with package length
						CString str;
						str.Format(L"%3.1f", stInspResult.dLength);
						while (str.GetLength() < 8)
							str = L" " + str;
						stUDFFileText.arStrings[i].Replace(L"@9", str);
					}
					else if (stUDFFileText.arStrings[i][nIndex + 1] == 'A')// Package Width
					{
						// Replace @A with package Width
						CString str;
						str.Format(L"%3.1f", stInspResult.dWidth);
						while (str.GetLength() < 8)
							str = L" " + str;
						stUDFFileText.arStrings[i].Replace(L"@A", str);
					}
					else if (stUDFFileText.arStrings[i][nIndex + 1] == 'B')// Package Height
					{
						// Replace @B with package Height
						CString str;
						str.Format(L"%3.1f", stInspResult.dHeight);
						while (str.GetLength() < 8)
							str = L" " + str;
						stUDFFileText.arStrings[i].Replace(L"@B", str);
					}
					else if (stUDFFileText.arStrings[i][nIndex + 1] == 'C')// Package Volume
					{
						// Replace @C with package Height
						CString str;
						str.Format(L"%3.2f", stInspResult.dVolume);
						while (str.GetLength() < 8)
							str = L" " + str;
						stUDFFileText.arStrings[i].Replace(L"@C", str);
					}
					else if (stUDFFileText.arStrings[i][nIndex + 1] == 'D')// Volume Unit
					{
						// Replace @D with volume unit
						CString str;
						str.Format(L"M");
						//while (str.GetLength() < 8)
						//	str = L" " + str;
						stUDFFileText.arStrings[i].Replace(L"@D", str);
					}
					else if (stUDFFileText.arStrings[i][nIndex + 1] == 'E')// net weight
					{
						// Replace @E with net weight
						CString str;
						str.Format(L"%2.2f", stInspResult.dWeightNet);
						while (str.GetLength() < 8)
							str = L" " + str;
						stUDFFileText.arStrings[i].Replace(L"@E", str);
					}
					else if (stUDFFileText.arStrings[i][nIndex + 1] == 'F')// gross weight
					{
						// Replace @F with gross weight
						CString str;
						str.Format(L"%2.2f", stInspResult.dWeightGr);
						while (str.GetLength() < 8)
							str = L" " + str;
						stUDFFileText.arStrings[i].Replace(L"@F", str);
					}
					else if (stUDFFileText.arStrings[i][nIndex + 1] == 'G')// tare weight
					{
						// Replace @G with tare weight
						CString str;
						str.Format(L"%2.2f", stInspResult.dWeightTare);
						while (str.GetLength() < 8)
							str = L" " + str;
						stUDFFileText.arStrings[i].Replace(L"@G", str);
					}
					else if (stUDFFileText.arStrings[i][nIndex + 1] == 'H')// Pre tare weight
					{
						// Replace @H with pre tare weight
						CString str;
						str.Format(L"%2.2f", stInspResult.dWeightPT);
						while (str.GetLength() < 8)
							str = L" " + str;
						stUDFFileText.arStrings[i].Replace(L"@H", str);
					}
					else if (stUDFFileText.arStrings[i][nIndex + 1] == 'I')// Weight unit
					{
						if (stUDFFileText.arStrings[i][nIndex + 1] == 'I')// weight unit
							stUDFFileText.arStrings[i].Replace(L"@I", stInspResult.strWeightunit);
					}
					else if (stUDFFileText.arStrings[i][nIndex + 1] == '@')// next char is also @
					{
						// Replace @@ with @
						CString str;
						stUDFFileText.arStrings[i].Replace(L"@@", L"@");
					}
					// Look for more control characters starting at current index
					nIndex = stUDFFileText.arStrings[i].Find(L"@", nIndex);
				}
				else
					nIndex = -1;
			}
		}
	}

	return bResult;
}

// This function locates the system variables (@xx) and replaces them with appropriate values
// from stInspResult. The result file (string array) can be sent to printer
// Weight lines may have two @ markers (for ex. @F@I).
// That is why we continue to scan a string (line) till we find all '@' characters and replace them appropriately.
// 
BOOL CMainFrame::CreatePrintFile(STUDFFileText& stUDFFileText, STInspResult stInspResult)
{
	BOOL bResult = TRUE;
	// Replace '@*' with appropriate strings
	bResult = ReplaceSystVars(stUDFFileText, stInspResult);
	// Replace '|**' with appropriate decimal numbers
	bResult &= ReplaceHexVars(stUDFFileText, stInspResult);

	return bResult;
}

CString CMainFrame::GetSystemDateString()
{
	CTime time = CTime::GetCurrentTime();
	CString strDate = time.Format("%x");
	/*CString strYear, strMonth, strDay, strEmpty;

	// Year
	strYear.Format(L"%d", time.GetYear());
	strYear = strYear.Right(2);
	// Month
	strMonth.Format(L"%d", time.GetMonth());
	int nMonthChar = strMonth.GetLength();
	if (nMonthChar == 1)
		strMonth.Format(L"0%d", time.GetMonth());
	//Day
	strDay.Format(L"%d", time.GetDay());
	int nDayChar = strDay.GetLength();
	if (nDayChar == 1)
		strDay.Format(L"0%d", time.GetDay());
	
	//strFolder.Format("%s\\%d_%s%s%s_%d%d%d", g_strScanData, m_nScanCount, strYear, strMonth, strDay, time.GetHour(), time.GetMinute(), time.GetSecond());
	strDate.Format(L"%s%s%s", strYear, strMonth, strDay);*/
	strDate = get_date_in_user_format(time);
	return strDate;
}


CString CMainFrame::GetSystemTimeString()
{
	CTime time = CTime::GetCurrentTime();
	CString strTime = time.Format("%X");
	/*CString strTimeHr, strTimeMin, strTimeSec, strEmpty;

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

	//strFolder.Format("%s\\%d_%s%s%s_%d%d%d", g_strScanData, m_nScanCount, strYear, strMonth, strDay, time.GetHour(), time.GetMinute(), time.GetSecond());
	strTime.Format(L"%s:%s:%s", strTimeHr, strTimeMin, strTimeSec);*/
	strTime = get_time_in_user_format(time);
	/**/
	return strTime;
}

// wParam contains weight and lParam contains flag for unstable, weight error and weight unit
// lParam:  0 - All is well, 1 - weight is unstable, 2 - Error reading weight, 3 - Kg, 4 - Gm, 5 - Lb, 6 - T
// wParam is pointer to wchars. Its contents are NxxxGxxxTxxxPTxxx
// This string needs to be broken up into the relevant weights
// 
LRESULT CMainFrame::OnUpdateWeight(WPARAM wParam, LPARAM lParam)
{
	CPMSHJApp* pApp = (CPMSHJApp*)AfxGetApp();
	CString str;
	if ((((int)lParam == 1) || ((int)lParam == 2)) && (pApp->m_stWeighScaleParams.WeighScaleType != GRAM))// Error or Unstable
	{
		m_pImageView->UpdateWeight(0, 0, 0, 0, str, (int)lParam);
		return 0;
	}
	else if ((int)lParam == 3)
		;
	double dWtNet = 0.0f, dWtGross = 0.0f, dWtTare = 0.0f, dWtPTare = 0.0f;
	int nIndexPTare = -1, nIndexNet = -1, nIndexTare = -1, nIndexPreTare = -1;
	CStringA strGross, strNet, strTare, strPTare, strWtUnits;
	BOOL bIsGross = TRUE;// P5 specific
	if (wParam)// If valid weight data is received
	{
		CStringA strTotal((LPCTSTR)wParam);
		EnterCriticalSection(&(theApp.m_csWeightString));
		strTotal = theApp.m_strWeightString;
		bIsGross = theApp.m_bIsGross;
		LeaveCriticalSection(&(theApp.m_csWeightString));
		if (strTotal.IsEmpty())
			return 0;

		if (pApp->m_stWeighScaleParams.WeighScaleType == EXCELL)
		{
			BOOL bIsNegative = FALSE;
		}
		else if (pApp->m_stWeighScaleParams.WeighScaleType == GRAM) // KPD - 120520
		{
			// If locale is not set the rounding off error cuts off digits after decimal point
			std::string currentLocale = setlocale(LC_NUMERIC, NULL); //retrive current locale
			setlocale(LC_NUMERIC, "C"); //change numeric locale to C, now decimal separator is '.'			
			// Composite string strTotal = strWeight + strTareWeight + strTareWtUnit;
			// Gross weight
			CString strGross = theApp.m_strWeightString.Left(8);
			// Setting wt accuracy for GRAM scale
			pApp->m_stOptions.nWtAccuracy = 8 - strGross.ReverseFind('.') - 1;
			if (pApp->m_stOptions.nWtAccuracy < 0)
				pApp->m_stOptions.nWtAccuracy = 0;
			//
			CString Temp;
			double dWeight = _wtof(strGross);
			// Tare weight
			CString strTrimmed = theApp.m_strWeightString.Right(strTotal.GetLength() - 8);
			CString strTare = strTrimmed.Left(8);
			dWtTare = _wtof(strTare);
			m_pImageView->m_dCurWt = 0;
			dWtGross = dWeight;
			// Not using strTareWtUnit as of now. Just assuming that units of Gross wt and Tare wt are same.
			//TRACE("Weight: %3.2f", m_pImageView->m_dCurWt);
			m_pImageView->m_bWtRead = TRUE;
			m_pImageView->UpdateWeight(dWtGross - dWtTare, dWtGross, dWtTare, dWtPTare, pApp->m_strWtUnit, (int)lParam);
			// Restore to original locale
			setlocale(LC_NUMERIC, currentLocale.c_str()); 
		}
		else // Standard scale
		{
			double dWeight = atof(strTotal);
			if (bIsGross)
			{
				m_pImageView->m_dCurWt = dWeight;// Making net wt and gross wt the same
				dWtGross = dWeight;
			}
			else
				m_pImageView->m_dCurWt = dWeight;// Net wt

			//TRACE("Weight: %3.2f", m_pImageView->m_dCurWt);
			m_pImageView->m_bWtRead = TRUE;
			m_pImageView->UpdateWeight(m_pImageView->m_dCurWt, dWtGross, dWtTare, dWtPTare, pApp->m_strWtUnit, (int)lParam);
			return 0;
		}
	}

	return 0;
}

// Comes here from serial port barcode scanner
// wParam contains string from scanner
// 
//LRESULT CMainFrame::OnUpdateCode(WPARAM wParam, LPARAM lParam)
//{
//	CPMSHJApp* pApp = (CPMSHJApp*)AfxGetApp();
//
//	EnterCriticalSection(&(pApp->m_csScannerString));
//	m_pImageView->ShowScanID(pApp->m_strScannerString);
//	LeaveCriticalSection(&(pApp->m_csScannerString));
//
//	return 0;
//}

// Comes here when Arduino receives trigger from foot pedal
// We will save data to DB and print label (if required) from here
LRESULT CMainFrame::OnRcdFootSwitchTrigger(WPARAM wParam, LPARAM lParam)
{
	CPMSHJApp* pApp = (CPMSHJApp*)AfxGetApp();
	EnterCriticalSection(&(pApp->m_csFootSwitchTrigger));
	m_pImageView->m_bFootSwithTrigRcd = TRUE;
	LeaveCriticalSection(&(pApp->m_csFootSwitchTrigger));
	CString strMsg("Trigger received from Foot Switch!");
	m_pImageView->SetMessage(strMsg);
	return 0;
}

// Comes here when foot pedal trigger read fails
//
LRESULT CMainFrame::OnRcdFootSwitchReadFail(WPARAM wParam, LPARAM lParam)
{
	CPMSHJApp* pApp = (CPMSHJApp*)AfxGetApp();
	EnterCriticalSection(&(pApp->m_csFootSwitchTrigger));
	m_pImageView->m_bFootSwithTrigRcd = FALSE;
	LeaveCriticalSection(&(pApp->m_csFootSwitchTrigger));
	CString strMsg("Foot Switch Read Fail!");
	m_pImageView->SetMessage(strMsg, RED_COLOR);
	return 0;
}

// Comes here when data logging is done
//
LRESULT CMainFrame::OnRcdDataLogStatus(WPARAM wParam, LPARAM lParam)
{
	CPMSHJApp* pApp = (CPMSHJApp*)AfxGetApp();
	eDataLoggingStatus DataLoggingStatus = (eDataLoggingStatus)wParam;
	m_pImageView->SetDataLoggingStatus(DataLoggingStatus);
	if (DataLoggingStatus == DATA_OK)
		IOutputLog(L"MainFrm Logging Data Log status - OK");
	else if (DataLoggingStatus == DATA_FAIL)
		IOutputLog(L"MainFrm Logging Data Log status - FAIL");
	else if (DataLoggingStatus == DATA_NOT_DONE)
		IOutputLog(L"MainFrm Logging Data Log status - Not Done");

	return 0;
}

// Comes here when code reading is done
//
LRESULT CMainFrame::OnRcdCodeReadStatus(WPARAM wParam, LPARAM lParam)
{
	CPMSHJApp* pApp = (CPMSHJApp*)AfxGetApp();
	eBarCodeReadStatus CodeReadStatus = (eBarCodeReadStatus)wParam;
	m_pImageView->SetCodeReadStatus(CodeReadStatus);
	if (CodeReadStatus == CODE_OK)
		IOutputLog(L"Code Read status - OK");
	else if (CodeReadStatus == CODE_OUT_OF_RANGE)
		IOutputLog(L"Code out of range");
	else if (CodeReadStatus == CODE_WRONG_LENGTH)
		IOutputLog(L"Code Wrong Length");

	return 0;
}

// Comes here when data logging is done
//
LRESULT CMainFrame::OnRcdWeighScaleError(WPARAM wParam, LPARAM lParam)
{
	CPMSHJApp* pApp = (CPMSHJApp*)AfxGetApp();
	//m_bAbortScan = TRUE;
	//m_bWeighScaleError = TRUE;	
	m_pImageView->OnBnClickedBnStop();
	pApp->StopWeighScaleReadThread();
	//IOutputLog(L"MainFrame: Weigh Scale Error.");
	if (::MessageBox(NULL, L"Weigh Scale Error. Do you want to disable weight reading?", L"Weigh scale error", MB_YESNO) == IDYES)
	{
		if (pApp->m_stOptions.bWtRqd)
			IOutputLog(L"Weigh Scale Error.");
		pApp->m_stOptions.bWtRqd = FALSE;
		pApp->WriteOptionsToRegistry();
	}
	return 0;
}

CString CMainFrame::get_date_in_user_format(CTime& time)
{
	CString strTmpFormat;
	CString strDate;
	WCHAR* szData = NULL;
	int num_chars =
		GetLocaleInfoW(LOCALE_USER_DEFAULT, LOCALE_SSHORTDATE, szData, 0);
	if (num_chars != 0)
	{
		szData = new WCHAR[num_chars + 1];
		szData[num_chars] = '\0';
		GetLocaleInfoW(LOCALE_USER_DEFAULT,
			LOCALE_SSHORTDATE, szData, num_chars);
		CString strTmp(szData);
		int ind = 0;
		int len = strTmp.GetLength();
		while (ind < len)
		{
			switch (strTmp[ind])
			{
			case 'y':
			{
				int year_type = 0;
				while (ind < len && strTmp[ind] == 'y') {
					ind++;
					year_type++;
				}
				ind--;
				switch (year_type) {
				case 4: strTmpFormat.Format(_T("%d"), time.GetYear());
					strDate += strTmpFormat; break;
				case 2: strTmpFormat.Format(_T("%02d"), time.GetYear() % 100);
					strDate += strTmpFormat; break;
				case 1: strTmpFormat.Format(_T("%d"), time.GetYear() % 10);
					strDate += strTmpFormat; break;
				}
				break;
			}
			case 'M':
			{
				int month_type = 0;
				while (ind < len && strTmp[ind] == 'M') {
					ind++;
					month_type++;
				}
				ind--;
				switch (month_type) {
				case 4:
				{
					WCHAR szMonth[500] = { 0 };
					if (0<GetLocaleInfoW(LOCALE_USER_DEFAULT,
						LOCALE_SMONTHNAME1 + time.GetMonth() - 1, szMonth, 499)) {
						strDate += szMonth;
					}
					break;
				}
				case 3:
				{
					WCHAR szMonth[500] = { 0 };
					if (0<GetLocaleInfoW(LOCALE_USER_DEFAULT,
						LOCALE_SABBREVMONTHNAME1 + time.GetMonth() - 1,
						szMonth, 499)) {
						strDate += szMonth;
					}
					break;
				}
				case 2: strTmpFormat.Format(_T("%02d"), time.GetMonth());
					strDate += strTmpFormat; break;
				case 1: strTmpFormat.Format(_T("%d"), time.GetMonth());
					strDate += strTmpFormat; break;
				}
				break;
			}
			case 'd':
			{
				int day_type = 0;
				while (ind < len && strTmp[ind] == 'd') {
					ind++;
					day_type++;
				}
				ind--;
				switch (day_type) {
				case 4:
				{
					UINT DayOfWeekFull[] = {
						LOCALE_SDAYNAME7,   // Sunday
						LOCALE_SDAYNAME1,
						LOCALE_SDAYNAME2,
						LOCALE_SDAYNAME3,
						LOCALE_SDAYNAME4,
						LOCALE_SDAYNAME5,
						LOCALE_SDAYNAME6   // Saturday
					};
					WCHAR szDayOfWeek[500] = { 0 };
					if (0<GetLocaleInfoW(LOCALE_USER_DEFAULT,
						DayOfWeekFull[time.GetDayOfWeek() - 1],
						szDayOfWeek, 499)) {
						strDate += szDayOfWeek;
					}
					break;
				}
				case 3:
				{
					UINT DayOfWeekAbbr[] = {
						LOCALE_SABBREVDAYNAME7,   // Sunday
						LOCALE_SABBREVDAYNAME1,
						LOCALE_SABBREVDAYNAME2,
						LOCALE_SABBREVDAYNAME3,
						LOCALE_SABBREVDAYNAME4,
						LOCALE_SABBREVDAYNAME5,
						LOCALE_SABBREVDAYNAME6   // Saturday
					};
					WCHAR szDayOfWeek[500] = { 0 };
					if (0<GetLocaleInfoW(LOCALE_USER_DEFAULT,
						DayOfWeekAbbr[time.GetDayOfWeek() - 1],
						szDayOfWeek, 499)) {
						strDate += szDayOfWeek;
					}
					break;
				}
				case 2: strTmpFormat.Format(_T("%02d"), time.GetDay());
					strDate += strTmpFormat; break;
				case 1: strTmpFormat.Format(_T("%d"), time.GetDay());
					strDate += strTmpFormat; break;
				}
				break;
			}
			default:
				strDate += CString(strTmp[ind]);
				break;
			}
			ind++;
		}
		delete szData;
	}

	if (strDate.IsEmpty()) {
		strDate = time.Format(_T("%x")); // fallback mechanism
	}

	return strDate;
}

CString CMainFrame::get_time_in_user_format(CTime& time)
{
	CString strTmpFormat;
	CString strTime;
	WCHAR* szData = NULL;
	int num_chars = GetLocaleInfoW(LOCALE_USER_DEFAULT,
		LOCALE_STIMEFORMAT, szData, 0);
	if (num_chars != 0)
	{
		szData = new WCHAR[num_chars + 1];
		szData[num_chars] = '\0';
		GetLocaleInfoW(LOCALE_USER_DEFAULT,
			LOCALE_STIMEFORMAT, szData, num_chars);
		CString strTmp(szData);
		int ind = 0;
		int len = strTmp.GetLength();
		while (ind < len)
		{
			switch (strTmp[ind])
			{
			case 't':
			{
				int time_marker_type = 0;
				while (ind < len && strTmp[ind] == 't') {
					ind++;
					time_marker_type++;
				}
				ind--;
				switch (time_marker_type) {
				case 2:
				case 1:
				{
					WCHAR szTimemarker[500] = { 0 };
					LCTYPE am_or_pm = LOCALE_S1159; //AM
					if (time.GetHour() >= 0 && time.GetHour() < 12) {
						am_or_pm = LOCALE_S1159; //AM
					}
					else {
						am_or_pm = LOCALE_S2359; //PM
					}
					if (0<GetLocaleInfoW(LOCALE_USER_DEFAULT,
						am_or_pm, szTimemarker, 499)) {
						if (time_marker_type == 1) {
							strTime += CString(szTimemarker, 1);
						}
						else {
							strTime += szTimemarker;
						}
					}
					break;
				}
				}
				break;
			}
			case 's':
			{
				int seconds_type = 0;
				while (ind < len && strTmp[ind] == 's') {
					ind++;
					seconds_type++;
				}
				ind--;
				switch (seconds_type) {
				case 2: strTmpFormat.Format(_T("%02d"), time.GetSecond());
					strTime += strTmpFormat; break;
				case 1: strTmpFormat.Format(_T("%d"), time.GetSecond());
					strTime += strTmpFormat; break;
				}
				break;
			}
			case 'm':
			{
				int minute_type = 0;
				while (ind < len && strTmp[ind] == 'm') {
					ind++;
					minute_type++;
				}
				ind--;
				switch (minute_type) {
				case 2: strTmpFormat.Format(_T("%02d"), time.GetMinute());
					strTime += strTmpFormat; break;
				case 1: strTmpFormat.Format(_T("%d"), time.GetMinute());
					strTime += strTmpFormat; break;
				}
				break;
			}
			case 'H':
			{
				int hour_type = 0;
				while (ind < len && strTmp[ind] == 'H') {
					ind++;
					hour_type++;
				}
				ind--;
				switch (hour_type) {
				case 2: strTmpFormat.Format(_T("%02d"), time.GetHour());
					strTime += strTmpFormat; break;
				case 1: strTmpFormat.Format(_T("%d"), time.GetHour());
					strTime += strTmpFormat; break;
				}
				break;
			}
			case 'h':
			{
				int hour_12_format = time.GetHour() % 12;
				if (hour_12_format == 0) {
					hour_12_format = 12;
				}
				int hour_type = 0;
				while (ind < len && strTmp[ind] == 'h') {
					ind++;
					hour_type++;
				}
				ind--;
				switch (hour_type) {
				case 2: strTmpFormat.Format(_T("%02d"), hour_12_format);
					strTime += strTmpFormat; break;
				case 1: strTmpFormat.Format(_T("%d"), hour_12_format);
					strTime += strTmpFormat; break;
				}
				break;
			}
			default:
				strTime += CString(strTmp[ind]);
				break;
			}
			ind++;
		}
		delete szData;
	}

	if (strTime.IsEmpty()) {
		strTime = time.Format(_T("%X")); //fallback mechanism
	}

	return strTime;
}

void CMainFrame::OnClose()
{
	PostQuitMessage(0);

	//CFrameWndEx::OnClose();
}

// Adding this function has stopped window getting minimized randomly at program launch
void CMainFrame::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CFrameWndEx::OnShowWindow(bShow, nStatus);

	static bool bOnce = true;

	if (bShow && !IsWindowVisible()
		&& bOnce)
	{
		bOnce = false;

		WINDOWPLACEMENT *lwp = new WINDOWPLACEMENT();
		POINT ptMin;
		ptMin.x = 0; ptMin.y = 2;
		lwp->rcNormalPosition.left = ptMin.x;
		lwp->rcNormalPosition.top = ptMin.x;
		lwp->rcNormalPosition.right = DISPLAY_WIDTH;
		lwp->rcNormalPosition.bottom = ptMin.y + DISPLAY_HEIGHT;

		//if (AfxGetApp()->GetProfileBinary("MainFrame", "WP", (LPBYTE*)&lwp, &nl))
		{
			SetWindowPlacement(lwp);
			delete[] lwp;
		}
	}
}

BOOL CMainFrame::CreatePrintFile()
{
	BOOL bResult = TRUE;


	return bResult;
}

CStringA CMainFrame::GetDateStringForPrinter()
{
	CTime time = CTime::GetCurrentTime();
	CStringA str;
	str.Format("%d%d%d", time.GetYear(), time.GetMonth(), time.GetDay());
	return str;
}

// This func (override) is required if you want to hide the window.
void CMainFrame::ActivateFrame(int nCmdShow)
{
	if (m_bForceHidden)
	{
		nCmdShow = SW_HIDE;
		m_bForceHidden = FALSE;
	}
	CFrameWndEx::ActivateFrame(nCmdShow);
}