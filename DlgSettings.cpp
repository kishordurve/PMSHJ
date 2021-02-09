// DlgSettings.cpp : implementation file
//

#include "stdafx.h"
#include "PMSHJ.h"
#include "DlgSettings.h"
#include "afxdialogex.h"
#include "DlgWtSettings.h"
#include "DlgSerialPortParams.h"
#include "DlgUDFFileEdit.h"
#include "DlgInspParams.h"
#include "DlgOptions.h"
#include "DlgPrintParams.h"
#include "DlgCalib.h"
#include "DlgFreightParams.h"
#include "DlgDataSendParams.h"

// CDlgSettings dialog

IMPLEMENT_DYNAMIC(CDlgSettings, CDialogEx)

CDlgSettings::CDlgSettings(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DLG_SETTINGS, pParent)
	, m_strMachineName(_T(""))
	, m_strBarcode(_T(""))
{
	m_bCodeTestingON = FALSE;
}

CDlgSettings::~CDlgSettings()
{
}

void CDlgSettings::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BN_WT_SETTINGS, m_BnWtSettings);
	DDX_Control(pDX, IDC_BN_COMPORT_SETTINGS, m_BnCOMPortSettings);
	DDX_Control(pDX, IDC_BN_READ_UDF_FILE, m_BnReadUDFFile);
	DDX_Control(pDX, IDC_BN_SAVE_UDF_FILE, m_BnSaveUDFFile);
	DDX_Text(pDX, IDC_EDIT_MACHINE_NAME, m_strMachineName);
	DDV_MaxChars(pDX, m_strMachineName, 32);
	DDX_Control(pDX, IDOK, m_BnOK);
	DDX_Control(pDX, IDC_BN_INSP_PARAMS, m_BnInspParams);
	DDX_Control(pDX, IDC_BN_CODE_TEST, m_BnBarcodeTest);
	DDX_Text(pDX, IDC_EDIT_CODE_TEST, m_strBarcode);
	DDX_Control(pDX, IDC_BN_OPTIONS, m_BnOptions);
	DDX_Control(pDX, IDC_BN_PRINT_PARAMS, m_BnPrintParams);
	DDX_Control(pDX, IDC_BN_CALIBRATION, m_BnCalib);
	DDX_Control(pDX, IDC_BN_FREIGHT_PARAMS, m_BnFreightParams);
	DDX_Control(pDX, IDC_BN_DATA_SEND_PARAMS, m_BnDataSendParams);
}


BEGIN_MESSAGE_MAP(CDlgSettings, CDialogEx)
	ON_BN_CLICKED(IDC_BN_COMPORT_SETTINGS, &CDlgSettings::OnBnClickedBnCOMPortSettings)
	ON_BN_CLICKED(IDC_BN_WT_SETTINGS, &CDlgSettings::OnBnClickedBnWtSettings)
	ON_BN_CLICKED(IDC_BN_READ_UDF_FILE, &CDlgSettings::OnBnClickedBnReadUDFFile)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BN_SAVE_UDF_FILE, &CDlgSettings::OnBnClickedBnSaveUDFFile)
	ON_BN_CLICKED(IDC_BN_INSP_PARAMS, &CDlgSettings::OnBnClickedBnInspParams)
	ON_BN_CLICKED(IDC_BN_CODE_TEST, &CDlgSettings::OnBnClickedBnCodeTest)
	ON_BN_CLICKED(IDC_BN_OPTIONS, &CDlgSettings::OnBnClickedBnOptions)
	ON_WM_MOVE()
	ON_WM_MOVING()
	ON_BN_CLICKED(IDC_BN_PRINT_PARAMS, &CDlgSettings::OnBnClickedBnPrintParams)
	ON_BN_CLICKED(IDC_BN_CALIBRATION, &CDlgSettings::OnBnClickedBnCalibration)
	ON_BN_CLICKED(IDC_BN_FREIGHT_PARAMS, &CDlgSettings::OnBnClickedBnFreightParams)
	ON_BN_CLICKED(IDC_BN_DATA_SEND_PARAMS, &CDlgSettings::OnBnClickedBnDataSendParams)
END_MESSAGE_MAP()


// CDlgSettings message handlers


void CDlgSettings::OnBnClickedBnCOMPortSettings()
{
	CDlgSerialPortParams dlg;
	dlg.DoModal();
}


void CDlgSettings::OnBnClickedBnWtSettings()
{
	CDlgWtSettings dlg;
	dlg.DoModal();
}


void CDlgSettings::OnBnClickedBnReadUDFFile()
{
	CPMSHJApp* pApp = (CPMSHJApp*)AfxGetApp();
	// Open UDF File
	// Get File Name from user
	static char BASED_CODE szFilter[] = "UDF Files (*.udf)| *.udf; ||";
	CString strFilter(szFilter);
	CString strFileName;
	CFileDialog FileDlg(TRUE,// FOr File Open
		NULL,
		NULL,
		OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST,
		strFilter,
		NULL);

	if (FileDlg.DoModal() == IDOK)
	{
		strFileName = FileDlg.GetPathName();
		if (strFileName.Find(L".udf") == -1)
		{
			AfxMessageBox(L"Please select file with extension 'udf'");
			return;
		}
		// Set File Name
		pApp->m_stUDFFileText.strUDFFileNameWithPath = strFileName;
		// Read file putting one line in each string of pApp->m_stUDFFileText.arStrings
		pApp->ReadUDFFile(pApp->m_stUDFFileText);
		// Keep a copy of file so that the system variables can be overwritten with actual results and printed.
		pApp->m_stUDFFileTextPrint = pApp->m_stUDFFileText;

		pApp->WriteUDFFileNameToRegistry();
	}
}

HBRUSH CDlgSettings::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	CPMSHJApp* pApp = (CPMSHJApp*)AfxGetApp();
	pDC->SetBkMode(TRANSPARENT);
	if ((pWnd->GetDlgCtrlID() == IDC_EDIT_MACHINE_NAME) || (pWnd->GetDlgCtrlID() == IDC_EDIT_CODE_TEST))
	{
		return pApp->m_WhiteBrush;
	}
	else
		return pApp->m_DlgBkgndBrush;
}

void CDlgSettings::SetButtonCaptions()
{
	CPMSHJApp* pApp = (CPMSHJApp*)AfxGetApp();
	CString strCaption, strToolTip;
	std::wstring str = pApp->m_pLanguage->GetString(IDS_STR_WT_SETTINGS);
	strCaption = str.c_str();

	m_BnWtSettings.SetFont(L"Aril", 16, 1000, 0, 0);
	m_BnWtSettings.SetTextColor(RGB(0, 0, 255));
	m_BnWtSettings.SetToolTipText(strCaption);
	m_BnWtSettings.SetShade(SHS_METAL, 10, 30, 20, BTN_BACK_COLOR);
	m_BnWtSettings.SetFlat(FALSE);
	m_BnWtSettings.SetWindowText(strCaption);

	str = pApp->m_pLanguage->GetString(IDS_STR_COM_PORT_ST);
	strCaption = str.c_str();
	m_BnCOMPortSettings.SetFont(L"Aril", 16, 1000, 0, 0);
	m_BnCOMPortSettings.SetTextColor(RGB(0, 0, 255));
	m_BnCOMPortSettings.SetToolTipText(strCaption);
	m_BnCOMPortSettings.SetShade(SHS_METAL, 10, 30, 20, BTN_BACK_COLOR);
	m_BnCOMPortSettings.SetFlat(FALSE);
	m_BnCOMPortSettings.SetWindowText(strCaption);

	str = pApp->m_pLanguage->GetString(IDS_STR_READ_UDF);
	strCaption = str.c_str();
	m_BnReadUDFFile.SetFont(L"Aril", 16, 1000, 0, 0);
	m_BnReadUDFFile.SetTextColor(RGB(0, 0, 255));
	m_BnReadUDFFile.SetToolTipText(strCaption);
	m_BnReadUDFFile.SetShade(SHS_METAL, 10, 30, 20, BTN_BACK_COLOR);
	m_BnReadUDFFile.SetFlat(FALSE);
	m_BnReadUDFFile.SetWindowText(strCaption);

	str = pApp->m_pLanguage->GetString(IDS_STR_SAVE_UDF);
	strCaption = str.c_str();
	m_BnSaveUDFFile.SetFont(L"Aril", 16, 1000, 0, 0);
	m_BnSaveUDFFile.SetTextColor(RGB(0, 0, 255));
	m_BnSaveUDFFile.SetToolTipText(strCaption);
	m_BnSaveUDFFile.SetShade(SHS_METAL, 10, 30, 20, BTN_BACK_COLOR);
	m_BnSaveUDFFile.SetFlat(FALSE);
	m_BnSaveUDFFile.SetWindowText(strCaption);

	str = pApp->m_pLanguage->GetString(IDS_STR_INSP_PARAMS);
	strCaption = str.c_str();
	m_BnInspParams.SetFont(L"Aril", 16, 1000, 0, 0);
	m_BnInspParams.SetTextColor(RGB(0, 0, 255));
	m_BnInspParams.SetToolTipText(strCaption);
	m_BnInspParams.SetShade(SHS_METAL, 10, 30, 20, BTN_BACK_COLOR);
	m_BnInspParams.SetFlat(FALSE);
	m_BnInspParams.SetWindowText(strCaption);

	str = pApp->m_pLanguage->GetString(IDS_STR_PRINT_PARAMS);
	strCaption = str.c_str();
	m_BnPrintParams.SetFont(L"Aril", 16, 1000, 0, 0);
	m_BnPrintParams.SetTextColor(RGB(0, 0, 255));
	m_BnPrintParams.SetToolTipText(strCaption);
	m_BnPrintParams.SetShade(SHS_METAL, 10, 30, 20, BTN_BACK_COLOR);
	m_BnPrintParams.SetFlat(FALSE);
	m_BnPrintParams.SetWindowText(strCaption);

	str = pApp->m_pLanguage->GetString(IDS_STR_FREIGHT_PARAMS);
	strCaption = str.c_str();
	m_BnFreightParams.SetFont(L"Aril", 16, 1000, 0, 0);
	m_BnFreightParams.SetTextColor(RGB(0, 0, 255));
	m_BnFreightParams.SetToolTipText(strCaption);
	m_BnFreightParams.SetShade(SHS_METAL, 10, 30, 20, BTN_BACK_COLOR);
	m_BnFreightParams.SetFlat(FALSE);
	m_BnFreightParams.SetWindowText(strCaption);

	str = pApp->m_pLanguage->GetString(IDS_STR_CODE_TEST);
	strCaption = str.c_str();
	m_BnBarcodeTest.SetFont(L"Aril", 16, 1000, 0, 0);
	m_BnBarcodeTest.SetTextColor(RGB(0, 0, 255));
	m_BnBarcodeTest.SetToolTipText(strCaption);
	m_BnBarcodeTest.SetShade(SHS_METAL, 10, 30, 20, BTN_BACK_COLOR);
	m_BnBarcodeTest.SetFlat(FALSE);
	m_BnBarcodeTest.SetWindowText(strCaption);

	str = pApp->m_pLanguage->GetString(IDS_STR_OPTIONS);
	strCaption = str.c_str();
	m_BnOptions.SetFont(L"Aril", 16, 1000, 0, 0);
	m_BnOptions.SetTextColor(RGB(0, 0, 255));
	m_BnOptions.SetToolTipText(strCaption);
	m_BnOptions.SetShade(SHS_METAL, 10, 30, 20, BTN_BACK_COLOR);
	m_BnOptions.SetFlat(FALSE);
	m_BnOptions.SetWindowText(strCaption);

	str = pApp->m_pLanguage->GetString(IDS_STR_CALIBRATION);
	strCaption = str.c_str();
	m_BnCalib.SetFont(L"Aril", 16, 1000, 0, 0);
	m_BnCalib.SetTextColor(RGB(0, 0, 255));
	m_BnCalib.SetToolTipText(strCaption);
	m_BnCalib.SetShade(SHS_METAL, 10, 30, 20, BTN_BACK_COLOR);
	m_BnCalib.SetFlat(FALSE);
	m_BnCalib.SetWindowText(strCaption);

	str = pApp->m_pLanguage->GetString(IDS_STR_DATA_SEND_PARAMS);
	strCaption = str.c_str();
	m_BnDataSendParams.SetFont(L"Aril", 16, 1000, 0, 0);
	m_BnDataSendParams.SetTextColor(RGB(0, 0, 255));
	m_BnDataSendParams.SetToolTipText(strCaption);
	m_BnDataSendParams.SetShade(SHS_METAL, 10, 30, 20, BTN_BACK_COLOR);
	m_BnDataSendParams.SetFlat(FALSE);
	m_BnDataSendParams.SetWindowText(strCaption);

	str = pApp->m_pLanguage->GetString(IDS_STR_OK);
	strCaption = str.c_str();
	m_BnOK.SetFont(L"Aril", 16, 1000, 0, 0);
	m_BnOK.SetTextColor(RGB(0, 0, 255));
	m_BnOK.SetToolTipText(strCaption);
	m_BnOK.SetShade(SHS_METAL, 10, 30, 20, BTN_BACK_COLOR);
	m_BnOK.SetFlat(FALSE);
	m_BnOK.SetWindowText(strCaption);
}

BOOL CDlgSettings::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	CPMSHJApp* pApp = (CPMSHJApp*)AfxGetApp();

	SetButtonCaptions();
	m_strMachineName = pApp->m_stDBRecordFields.sta_name;

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgSettings::OnBnClickedBnSaveUDFFile()
{
	CDlgUDFFileEdit dlg;
	dlg.DoModal();
}


void CDlgSettings::OnOK()
{
	UpdateData();

	CPMSHJApp* pApp = (CPMSHJApp*)AfxGetApp();

	pApp->m_stDBRecordFields.sta_name = m_strMachineName;
	pApp->WriteCurMachineNameToRegistry();
	// Restore 'Settings' button icon
	pApp->m_pFrame->m_pImageView->m_BnSettings.SetIcon(IDI_SETTINGS);
	pApp->m_pFrame->m_pImageView->Invalidate();

	CDialogEx::OnOK();
}


void CDlgSettings::OnBnClickedBnInspParams()
{
	CDlgInspParams dlg;
	dlg.DoModal();
}


void CDlgSettings::OnBnClickedBnCodeTest()
{
	CPMSHJApp* pApp = (CPMSHJApp*)AfxGetApp();

	m_bCodeTestingON = !m_bCodeTestingON;

	if (m_bCodeTestingON)
	{
		m_BnWtSettings.EnableWindow(FALSE);
		m_BnCOMPortSettings.EnableWindow(FALSE);
		m_BnReadUDFFile.EnableWindow(FALSE);
		m_BnSaveUDFFile.EnableWindow(FALSE);
		m_BnInspParams.EnableWindow(FALSE);
		m_BnOK.EnableWindow(FALSE);
		m_BnOptions.EnableWindow(FALSE);

		m_strBarcode.Empty();
		pApp->m_pFrame->m_bBarcodeReceived = FALSE;
		pApp->m_pFrame->m_csArray.RemoveAll();
		pApp->m_pFrame->m_strTotal.Empty();
		EnterCriticalSection(&(pApp->m_csScannerString));
		pApp->m_strScannerString.Empty();
		LeaveCriticalSection(&(pApp->m_csScannerString));
		pApp->m_strScannerString.Empty();
		if ((pApp->m_stOptions.bSerialPortScanner) || (pApp->m_stOptions.bHikVisionScanner))
		{
			if (!pApp->m_bScannerThreadUp)
			{
				pApp->m_bScannerThreadUp = pApp->StartCodeScannerThread();
				if (pApp->m_bScannerThreadUp)
					IOutputLog(L"Started Code Scanner Thread");
				else
					IOutputLog(L"Could not Start Code Scanner Thread");
			}
		}
		else
			pApp->m_pFrame->StartKBCapture();
		GetDlgItem(IDC_EDIT_CODE_TEST)->SetFocus();
		ReadCode();

		UpdateData(FALSE);
		m_bCodeTestingON = FALSE;// When it comes out it must be made false

		if (!m_strBarcode.IsEmpty())
		{
			AfxMessageBox(L"Barcode Read!");
			m_strBarcode.Empty();
		}
		else
			AfxMessageBox(L"Barcode Reading Aborted!");
	}		
	UpdateData(FALSE);
	m_BnWtSettings.EnableWindow(TRUE);
	m_BnCOMPortSettings.EnableWindow(TRUE);
	m_BnReadUDFFile.EnableWindow(TRUE);
	m_BnSaveUDFFile.EnableWindow(TRUE);
	m_BnInspParams.EnableWindow(TRUE);
	m_BnOK.EnableWindow(TRUE);
	m_BnOptions.EnableWindow(TRUE);

	EnterCriticalSection(&(pApp->m_csScannerString));
	pApp->m_strScannerString.Empty();
	LeaveCriticalSection(&(pApp->m_csScannerString));
	pApp->m_pFrame->m_csArray.RemoveAll();
	pApp->m_pFrame->StopKBCapture();

}

void CDlgSettings::ReadCode()
{
	CPMSHJApp* pApp = (CPMSHJApp*)AfxGetApp();
	MSG msg;
	CIHighResTimer	HighResTimer;
	double dTime1 = HighResTimer.GetHighResElapsedTimeMsec();
	while (m_bCodeTestingON)// m_bReadWtThreadUp is set to FALSE if thread has stopped due to whatever reason
	{
		if ((pApp->m_stOptions.bSerialPortScanner) || (pApp->m_stOptions.bHikVisionScanner))
		{
			while (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				::TranslateMessage(&msg);
				::DispatchMessage(&msg);
			}
			CString strCode;
			EnterCriticalSection(&(pApp->m_csScannerString));
			strCode = pApp->m_strScannerString;
			LeaveCriticalSection(&(pApp->m_csScannerString));
			if (strCode.GetLength() >= pApp->m_stInspParams.nBarcodeLengthMin)
			{
				m_strBarcode = strCode;
				if (m_strBarcode.Find('*') != -1)
					m_strBarcode = m_strBarcode.Left(m_strBarcode.Find('*'));
				if (m_strBarcode.GetLength() > pApp->m_stInspParams.nBarcodeLengthMax)
					m_strBarcode = m_strBarcode.Right(pApp->m_stInspParams.nBarcodeLengthMax);
				UpdateData(FALSE);
				break;
			}
			else if (strCode.GetLength() >= 3)// Some code was received
			{
				IOutputLog(L"Length of barcode less than param");
				continue;
			}
			else // W/F Barcode
				continue;
		}
		else
		{
			while (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				::TranslateMessage(&msg);
				::DispatchMessage(&msg);
			}
			CString csLineContent;
			m_strBarcode.Empty();
			for (int nID = 0; nID < pApp->m_pFrame->m_csArray.GetSize(); nID++)
			{
				csLineContent = pApp->m_pFrame->m_csArray.GetAt(nID) + L"\n";
				TRACE(csLineContent);
				m_strBarcode += pApp->m_pFrame->m_csArray.GetAt(nID);
			}
			if ((m_strBarcode.Find('*') == -1) || (m_strBarcode.GetLength() < pApp->m_stInspParams.nBarcodeLengthMin))
			{
				UpdateData(FALSE);
				continue;
			}
			else
			{
				//IOutputLogString(m_strBarcode);
				if (m_strBarcode.Find('*') != -1)
					m_strBarcode = m_strBarcode.Left(m_strBarcode.Find('*'));
				IOutputLogString(m_strBarcode);
				UpdateData(FALSE);
				break;
			}

			while (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				::TranslateMessage(&msg);
				::DispatchMessage(&msg);
			}
		}
	}
	int nSize = m_strBarcode.GetLength();
}


void CDlgSettings::OnBnClickedBnOptions()
{
	CDlgOptions dlg;
	dlg.DoModal();
	// Set laguage specific captions
	SetButtonCaptions();
}

void CDlgSettings::OnMoving(UINT fwSide, LPRECT pRect)
{
	CDialogEx::OnMoving(fwSide, pRect);

	// Find Current location of the dialog
	CRect CurRect;
	GetWindowRect(&CurRect);

	// Set current location as the moving location
	pRect->left = CurRect.left;
	pRect->top = CurRect.top;
	pRect->right = CurRect.right;
	pRect->bottom = CurRect.bottom;
}


void CDlgSettings::OnBnClickedBnPrintParams()
{
	CDlgPrintParams dlg;
	dlg.DoModal();
}


void CDlgSettings::OnBnClickedBnCalibration()
{
	CPMSHJApp* pApp = (CPMSHJApp*)AfxGetApp();
	//STInspResult stInspResult;
	//for (int i = 0; i < 4; i++)
	//{
	//	stInspResult.dLength = 10.1 + i;
	//	stInspResult.dWidth = 11.2 + i;
	//	stInspResult.dHeight = 12.3 + i;
	//	stInspResult.dVolume = 13.4 + i;
	//	stInspResult.dWeightNet = 145.23 + i;
	//	stInspResult.strBarcode.Format(L"Barcode%d", i+1);
	//	pApp->m_pFrame->m_pImageView->UpdateGrid(stInspResult);
	//}
	//pApp->m_pFrame->m_pImageView->SetFocusToHorzBar();
	//return;
	if (!pApp->m_bCam3DAvailable)
	{
		IOutputLog(L"3D camera NA");
		AfxMessageBox(L"3D camera NA");
		return;
	}
	CDlgCalib dlg;
	if (dlg.DoModal() == IDOK)
	{
		EndDialog(0);
		if (dlg.m_nCalibType == 0) // Camera height calibration
		{
			pApp->m_pFrame->m_pImageView->CalibForPlatformHeight();
		}
		else if (dlg.m_nCalibType == 1) // Camera alignment calibration
		{
			pApp->m_pFrame->m_pImageView->CalibForCamAlignment();
		}
	}
}

void CDlgSettings::OnBnClickedBnFreightParams()
{
	CDlgFreightParams dlg;
	dlg.DoModal();
}


void CDlgSettings::OnBnClickedBnDataSendParams()
{
	CDlgDataSendParams dlg;
	dlg.DoModal();
}
