// DlgOptions.cpp : implementation file
//

#include "stdafx.h"
#include "PMSHJ.h"
#include "DlgOptions.h"
#include "afxdialogex.h"


// CDlgOptions dialog

IMPLEMENT_DYNAMIC(CDlgOptions, CDialogEx)

CDlgOptions::CDlgOptions(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DLG_OPTIONS, pParent)
	, m_nLanguage(0)
	, m_nDimUnit(0)
//	, m_bSerialPortScanner(FALSE)
	, m_nSuffixChar(0)
	, m_bShowNetWt(FALSE)
	, m_bShowGrossWt(FALSE)
	, m_bShowTareWt(FALSE)
	, m_bShowPreTareWt(FALSE)
	, m_bPrintLabel(FALSE)
	, m_nTriggerSource(0)
	, m_nScannerType(0)
	, m_bEnableWt(TRUE)
{

}

CDlgOptions::~CDlgOptions()
{
}

void CDlgOptions::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_RADIO_LANGUAGE, m_nLanguage);
	DDV_MinMaxInt(pDX, m_nLanguage, 0, 2);
	DDX_Radio(pDX, IDC_RADIO_DIM_UNIT, m_nDimUnit);
	DDV_MinMaxInt(pDX, m_nDimUnit, 0, 3);
	DDX_Control(pDX, IDOK, m_BnOK);
	DDX_Control(pDX, IDCANCEL, m_BnCancel);
	//	DDX_Check(pDX, IDC_CHK_SERIAL_SCANNER, m_bSerialPortScanner);
	DDX_Radio(pDX, IDC_RAD_CODE_SUFFIX, m_nSuffixChar);
	DDV_MinMaxInt(pDX, m_nSuffixChar, 0, 1);
	DDX_Check(pDX, IDC_CHK_NET_WT, m_bShowNetWt);
	DDX_Check(pDX, IDC_CHK_GROSS_WT, m_bShowGrossWt);
	DDX_Check(pDX, IDC_CHK_TARE_WT, m_bShowTareWt);
	DDX_Check(pDX, IDC_CHK_PRETARE_WT, m_bShowPreTareWt);
	DDX_Check(pDX, IDC_CHK_PRINT_LABEL, m_bPrintLabel);
	DDX_Radio(pDX, IDC_RADIO_TRG_SOURCE, m_nTriggerSource);
	DDV_MinMaxInt(pDX, m_nTriggerSource, 0, 1);
	DDX_Control(pDX, IDC_BN_TEST_FOOT_SWITCH, m_BnTestFootSwitch);
	DDX_Radio(pDX, IDC_RADIO_SCANNER, m_nScannerType);
	DDV_MinMaxInt(pDX, m_nScannerType, 0, 2);
	DDX_Check(pDX, IDC_CHK_ENABLE_WT, m_bEnableWt);
}


BEGIN_MESSAGE_MAP(CDlgOptions, CDialogEx)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BN_TEST_FOOT_SWITCH, &CDlgOptions::OnBnClickedBnTestFootSwitch)
	ON_BN_CLICKED(IDC_RADIO_FOOT_SWITCH, &CDlgOptions::OnBnClickedRadioFootSwitch)
	ON_BN_CLICKED(IDC_RADIO_TRG_SOURCE, &CDlgOptions::OnBnClickedRadioTrgSource)
END_MESSAGE_MAP()


// CDlgOptions message handlers


void CDlgOptions::OnOK()
{
	UpdateData(TRUE);
	CPMSHJApp* pApp = (CPMSHJApp*)AfxGetApp();
	if (m_nLanguage > 1)
	{
		m_nLanguage = pApp->m_stOptions.Language;
		AfxMessageBox(L"Selected language not yet implemented.\nReverting to old language.");
		return;
	}
	if (pApp->m_stOptions.Language != (eLanguage)m_nLanguage)
		pApp->m_pFrame->m_pImageView->ChangeLaguage((eLanguage)m_nLanguage);
	pApp->m_stOptions.Language = (eLanguage)m_nLanguage;
	if (pApp->m_stOptions.DimUnits != (eDimUnits)m_nDimUnit)
	{
		pApp->m_stOptions.DimUnits = (eDimUnits)m_nDimUnit;
		pApp->m_pFrame->m_pImageView->ModifyDisplyDimsForNewUnits(pApp->m_stOptions.DimUnits);
	}
	if (m_nScannerType == 0)
	{
		pApp->m_stOptions.bHikVisionScanner = TRUE;
		pApp->m_stOptions.bSerialPortScanner = FALSE;
	}
	else if (m_nScannerType == 1)
	{
		pApp->m_stOptions.bSerialPortScanner = TRUE;
		pApp->m_stOptions.bHikVisionScanner = FALSE;
	}
	else if (m_nScannerType == 2)
	{
		pApp->m_stOptions.bSerialPortScanner = FALSE;
		pApp->m_stOptions.bHikVisionScanner = FALSE;
	}

	pApp->m_stOptions.bPrintLabel = m_bPrintLabel;
	if (m_nSuffixChar == 0)
		pApp->m_cEndChar = '*';
	else if (m_nSuffixChar == 1)
		pApp->m_cEndChar = '\r';

	pApp->m_stOptions.bShowNetWt		= m_bShowNetWt;
	pApp->m_stOptions.bShowGrossWt		= m_bShowGrossWt;
	pApp->m_stOptions.bShowTareWt		= m_bShowTareWt;
	pApp->m_stOptions.bShowPreTareWt	= m_bShowPreTareWt;
	pApp->m_stOptions.bWtRqd			= m_bEnableWt;
	if (m_nTriggerSource == 0)
	{
		pApp->m_stOptions.bUseCodeAsTrigger = TRUE;
		pApp->m_stOptions.bUseFootSwitch	= FALSE;
	}
	else
	{
		pApp->m_stOptions.bUseCodeAsTrigger = FALSE;
		pApp->m_stOptions.bUseFootSwitch = TRUE;
	}

	pApp->m_pFrame->m_pImageView->DisplayWtLabels();

	pApp->WriteOptionsToRegistry();

	ResetEvent(pApp->m_hCheckFootSwitch);

	CDialogEx::OnOK();
}


BOOL CDlgOptions::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CPMSHJApp* pApp = (CPMSHJApp*)AfxGetApp();
	m_nLanguage				= pApp->m_stOptions.Language;
	m_nDimUnit				= pApp->m_stOptions.DimUnits;
	if (pApp->m_stOptions.bHikVisionScanner)
		m_nScannerType	= 0;
	else if (pApp->m_stOptions.bSerialPortScanner)
		m_nScannerType	= 1;
	else // KB Emulation - Neither Serial Port not HikVision Scanner
		m_nScannerType	= 2;

	m_bPrintLabel			= pApp->m_stOptions.bPrintLabel;
	if (pApp->m_stOptions.bUseCodeAsTrigger)
	{
		m_nTriggerSource = 0;
		m_BnTestFootSwitch.EnableWindow(FALSE);
	}
	else
		m_nTriggerSource = 1;

	UpdateData(FALSE);

	CString strCaption, strToolTip;
	strCaption = L"Test Foot Switch";
	m_BnTestFootSwitch.SetFont(L"Aril", 14, 1000, 0, 0);
	m_BnTestFootSwitch.SetTextColor(RGB(0, 0, 255));
	m_BnTestFootSwitch.SetToolTipText(strCaption);
	m_BnTestFootSwitch.SetShade(SHS_METAL, 10, 30, 20, BTN_BACK_COLOR);m_BnOK.SetFlat(FALSE);
	m_BnTestFootSwitch.SetWindowText(strCaption);

	std::wstring str = pApp->m_pLanguage->GetString(IDS_STR_WT_SETTINGS);
	strCaption = str.c_str();
	str = pApp->m_pLanguage->GetString(IDS_STR_OK);
	strCaption = str.c_str();
	m_BnOK.SetFont(L"Aril", 16, 1000, 0, 0);
	m_BnOK.SetTextColor(RGB(0, 0, 255));
	m_BnOK.SetToolTipText(strCaption);
	m_BnOK.SetShade(SHS_METAL, 10, 30, 20, BTN_BACK_COLOR);m_BnOK.SetFlat(FALSE);
	m_BnOK.SetWindowText(strCaption);

	str = pApp->m_pLanguage->GetString(IDS_STR_CANCEL);
	strCaption = str.c_str();
	m_BnCancel.SetFont(L"Aril", 16, 1000, 0, 0);
	m_BnCancel.SetTextColor(RGB(0, 0, 255));
	m_BnCancel.SetToolTipText(strCaption);
	m_BnCancel.SetShade(SHS_METAL, 10, 30, 20, BTN_BACK_COLOR);
	m_BnCancel.SetFlat(FALSE);
	m_BnCancel.SetWindowText(strCaption);

	if (pApp->m_cEndChar == '*')
		m_nSuffixChar = 0;
	else if (pApp->m_cEndChar == '\r')
		m_nSuffixChar = 1;

	m_bShowNetWt		= pApp->m_stOptions.bShowNetWt;
	m_bShowGrossWt		= pApp->m_stOptions.bShowGrossWt;
	m_bShowTareWt		= pApp->m_stOptions.bShowTareWt;
	m_bShowPreTareWt	= pApp->m_stOptions.bShowPreTareWt;
	m_bEnableWt			= pApp->m_stOptions.bWtRqd;

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


HBRUSH CDlgOptions::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	CPMSHJApp* pApp = (CPMSHJApp*)AfxGetApp();
	pDC->SetBkMode(TRANSPARENT);
	return pApp->m_DlgBkgndBrush;
}


void CDlgOptions::OnBnClickedBnTestFootSwitch()
{
	CPMSHJApp* pApp = (CPMSHJApp*)AfxGetApp();
	// Just set the event here. The thread sends necessary string to Arduino and receives state of switch
	if (pApp->m_bFootSwitchThreadUp)
		SetEvent(pApp->m_hCheckFootSwitch);
	else
	{
		pApp->StartFootSwitchThread();
		if (pApp->m_bFootSwitchThreadUp)
			SetEvent(pApp->m_hCheckFootSwitch);
	}
	while (!pApp->m_pFrame->m_pImageView->m_bFootSwithTrigRcd)
	{
		MSG msg;
		while (::PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
		{
			if (!pApp->PumpMessage()) //if WM_QUIT i.e User exited the application
			{
				::PostQuitMessage(0);
				return;// FT_APPLN_QUIT;
			}
		}
	}
	AfxMessageBox(L"Foot Switch Trigger Received!");
	pApp->m_pFrame->m_pImageView->m_bFootSwithTrigRcd = FALSE;
}


void CDlgOptions::OnBnClickedRadioFootSwitch()
{
	m_BnTestFootSwitch.EnableWindow(TRUE);
}


void CDlgOptions::OnBnClickedRadioTrgSource()
{
	m_BnTestFootSwitch.EnableWindow(FALSE);
}
