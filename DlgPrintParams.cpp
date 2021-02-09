// DlgPrintParams.cpp : implementation file
//

#include "stdafx.h"
#include "PMSHJ.h"
#include "DlgPrintParams.h"
#include "afxdialogex.h"
#include "tsclib.h"

// CDlgPrintParams dialog

IMPLEMENT_DYNAMIC(CDlgPrintParams, CDialogEx)

CDlgPrintParams::CDlgPrintParams(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DLG_PRINT_PARAMS, pParent)
	, m_nLabelLength(0)
	, m_nLabelWidth(0)
	, m_nCodeHeight(0)
	, m_nNumPrints(0)
	, m_strCorporateName(_T("ABCD Corpn"))
	, m_strProductName(_T("BOX"))
	, m_strPrinterName(_T("Gprinter GP-3120TU"))// Should be as seen in Windows task manager
	, m_bGetUserOKForPrinting(FALSE)
	, m_bPrintLogo(FALSE)
{
	m_bLogoUploadedToPrinter = FALSE;
}

CDlgPrintParams::~CDlgPrintParams()
{
}

void CDlgPrintParams::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_LABEL_LENGTH, m_nLabelLength);
	DDV_MinMaxInt(pDX, m_nLabelLength, 20, 200);
	DDX_Text(pDX, IDC_EDIT_LABEL_WIDTH, m_nLabelWidth);
	DDV_MinMaxInt(pDX, m_nLabelWidth, 20, 200);
	DDX_Text(pDX, IDC_EDIT_CODE_HT, m_nCodeHeight);
	DDV_MinMaxInt(pDX, m_nCodeHeight, 2, 50);
	DDX_Text(pDX, IDC_EDIT_NUM_PRINTS, m_nNumPrints);
	DDV_MinMaxInt(pDX, m_nNumPrints, 1, 50);
	DDX_Control(pDX, IDC_BN_TEST_PRINT, m_BnTestPrint);
	DDX_Control(pDX, IDOK, m_BnOK);
	DDX_Control(pDX, IDCANCEL, m_BnCancel);
	DDX_Text(pDX, IDC_EDIT_CORPORATE, m_strCorporateName);
	DDV_MaxChars(pDX, m_strCorporateName, 48);
	DDX_Text(pDX, IDC_EDIT_PRODUCT, m_strProductName);
	DDV_MaxChars(pDX, m_strProductName, 48);
	DDX_Control(pDX, IDC_CMB_FONT, m_CmbFont);
	DDX_Control(pDX, IDC_CMB_CODE, m_CmbCodeType);
	DDX_Text(pDX, IDC_EDIT_PRINTER, m_strPrinterName);
	DDX_Check(pDX, IDC_CHK_PRINT_CNFM, m_bGetUserOKForPrinting);
	DDX_Check(pDX, IDC_CHK_LOGO, m_bPrintLogo);
	DDX_Control(pDX, IDC_STATIC_LOGO_FILE, m_staticLogoFile);
	DDX_Control(pDX, IDC_BN_LOGO_FILE, m_BnSelectLogoFile);
}


BEGIN_MESSAGE_MAP(CDlgPrintParams, CDialogEx)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BN_TEST_PRINT, &CDlgPrintParams::OnBnClickedBnTestPrint)
	ON_BN_CLICKED(IDC_CHK_LOGO, &CDlgPrintParams::OnBnClickedChkLogo)
	ON_BN_CLICKED(IDC_BN_LOGO_FILE, &CDlgPrintParams::OnBnClickedBnLogoFile)
END_MESSAGE_MAP()


// CDlgPrintParams message handlers

BOOL CDlgPrintParams::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	CPMSHJApp* pApp = (CPMSHJApp*)AfxGetApp();
	CString strCaption, strToolTip;
	std::wstring str;

	str = pApp->m_pLanguage->GetString(IDS_STR_OK);
	strCaption = str.c_str();
	m_BnOK.SetFont(L"Aril", 16, 1000, 0, 0);
	m_BnOK.SetTextColor(RGB(0, 0, 255));
	m_BnOK.SetToolTipText(strCaption);
	m_BnOK.SetShade(SHS_METAL, 10, 30, 20, BTN_BACK_COLOR);
	m_BnOK.SetFlat(FALSE);
	m_BnOK.SetWindowText(strCaption);

	str = pApp->m_pLanguage->GetString(IDS_STR_TEST_PRINT);
	strCaption = str.c_str();
	m_BnTestPrint.SetFont(L"Aril", 16, 1000, 0, 0);
	m_BnTestPrint.SetTextColor(RGB(0, 0, 255));
	m_BnTestPrint.SetToolTipText(strCaption);
	m_BnTestPrint.SetShade(SHS_METAL, 10, 30, 20, BTN_BACK_COLOR);
	m_BnTestPrint.SetFlat(FALSE);
	m_BnTestPrint.SetWindowText(strCaption);

	str = pApp->m_pLanguage->GetString(IDS_STR_LOGO_FILE);
	strCaption = str.c_str();
	m_BnSelectLogoFile.SetFont(L"Aril", 14, 1000, 0, 0);
	m_BnSelectLogoFile.SetTextColor(RGB(0, 0, 255));
	m_BnSelectLogoFile.SetToolTipText(strCaption);
	m_BnSelectLogoFile.SetShade(SHS_METAL, 10, 30, 20, BTN_BACK_COLOR);
	m_BnSelectLogoFile.SetFlat(FALSE);
	m_BnSelectLogoFile.SetWindowText(strCaption);

	str = pApp->m_pLanguage->GetString(IDS_STR_CANCEL);
	strCaption = str.c_str();
	m_BnCancel.SetFont(L"Aril", 16, 1000, 0, 0);
	m_BnCancel.SetTextColor(RGB(0, 0, 255));
	m_BnCancel.SetToolTipText(strCaption);
	m_BnCancel.SetShade(SHS_METAL, 10, 30, 20, BTN_BACK_COLOR);
	m_BnCancel.SetFlat(FALSE);
	m_BnCancel.SetWindowText(strCaption);

	m_nLabelLength	= pApp->m_stPrintParams.nLabelLengthMM;
	m_nLabelWidth	= pApp->m_stPrintParams.nLabelWidthMM;
	m_nCodeHeight	= pApp->m_stPrintParams.nCodeHtMM;
	m_nNumPrints	= pApp->m_stPrintParams.nNumPrints;
	m_strPrinterName	= pApp->m_stPrintParams.strPrinterName;
	m_bGetUserOKForPrinting = pApp->m_stPrintParams.bGetUserOKForPrinting;
	m_bPrintLogo = pApp->m_stPrintParams.bPrintLogo;
	m_strLogoFile = pApp->m_stPrintParams.strLogoFile;
	m_staticLogoFile.SetWindowTextW(m_strLogoFile);
	if (m_bPrintLogo)
		((CWnd*)GetDlgItem(IDC_BN_LOGO_FILE))->EnableWindow(TRUE);

	if (!pApp->m_stPrintParams.strCorporateName.IsEmpty())
		m_strCorporateName = pApp->m_stPrintParams.strCorporateName;
	if (!pApp->m_stPrintParams.strProductName.IsEmpty())
		m_strProductName = pApp->m_stPrintParams.strProductName;

	m_CmbFont.SetCurSel(pApp->m_stPrintParams.nFontSize);
	m_CmbCodeType.SetCurSel(pApp->m_stPrintParams.CodeType);

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgPrintParams::OnOK()
{
	CPMSHJApp* pApp = (CPMSHJApp*)AfxGetApp();
	UpdateData(TRUE);
	pApp->m_stPrintParams.nLabelLengthMM	= m_nLabelLength;
	pApp->m_stPrintParams.nLabelWidthMM		= m_nLabelWidth;
	pApp->m_stPrintParams.nCodeHtMM			= m_nCodeHeight;
	pApp->m_stPrintParams.nNumPrints		= m_nNumPrints;
	pApp->m_stPrintParams.nFontSize			= (eCodeType)m_CmbFont.GetCurSel();
	pApp->m_stPrintParams.CodeType			= (eCodeType)m_CmbCodeType.GetCurSel();
	pApp->m_stPrintParams.strCorporateName	= m_strCorporateName;
	pApp->m_stPrintParams.strProductName	= m_strProductName;
	pApp->m_stPrintParams.strPrinterName	= m_strPrinterName;
	pApp->m_stPrintParams.bGetUserOKForPrinting = m_bGetUserOKForPrinting;
	pApp->m_stPrintParams.bPrintLogo		= m_bPrintLogo;
	pApp->m_stPrintParams.strLogoFile		= m_strLogoFile;

	pApp->WritePrintParamsToRegistry();
	CDialogEx::OnOK();
}

HBRUSH CDlgPrintParams::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	CPMSHJApp* pApp = (CPMSHJApp*)AfxGetApp();
	pDC->SetBkMode(TRANSPARENT);
	if ((pWnd->GetDlgCtrlID() == IDC_EDIT_LABEL_LENGTH) || (pWnd->GetDlgCtrlID() == IDC_EDIT_LABEL_WIDTH) ||
		(pWnd->GetDlgCtrlID() == IDC_CMB_FONT) || (pWnd->GetDlgCtrlID() == IDC_CMB_CODE) || 
		(pWnd->GetDlgCtrlID() == IDC_EDIT_CORPORATE) || (pWnd->GetDlgCtrlID() == IDC_EDIT_PRODUCT) ||
		(pWnd->GetDlgCtrlID() == IDC_EDIT_CODE_HT) || (pWnd->GetDlgCtrlID() == IDC_EDIT_NUM_PRINTS) ||
		(pWnd->GetDlgCtrlID() == IDC_EDIT_PRINTER))
	{
		return pApp->m_WhiteBrush;
	}
	else
		return pApp->m_DlgBkgndBrush;
}


void CDlgPrintParams::OnBnClickedBnTestPrint()
{
	CPMSHJApp* pApp = (CPMSHJApp*)AfxGetApp();
	UpdateData(TRUE);
	if (pApp->m_bPrintLabelThreadUp)
		SetEvent(pApp->m_hPrintTestLabel);
	else
		PrintTestLabelCLExpress();
}

// Prints a test label with test dimensions and weight
BOOL CDlgPrintParams::PrintTestLabel()
{
	BOOL bResult = TRUE;
	UpdateData(TRUE);
	CPMSHJApp* pApp = (CPMSHJApp*)AfxGetApp();
	CStringA str, str1, str2;
	CString strUni;
	std::string strPrinter = CW2A(m_strPrinterName);
	//IOutputLog(L"Opening Printer Port!");
	int nRetVal = openport((char*)strPrinter.c_str());// This Works!
	if (nRetVal == 0)
		return FALSE;
	if (!pApp->m_pFrame->m_pImageView->CheckPrinterStatus())
	{
		IOutputLog(L"CheckPrinterStatus() returned FALSE!");
		nRetVal = closeport();
		return FALSE;
	}
	IOutputLog(L"In PrintTestLabel() Printer Port Opened, Printer Status OK!");
	// label width mm, label height, speed, printing density (0-15), type of sensor (0-gap sensor), Vert spacing, offset
	setup("100 mm", "80 mm", "3", "10", "0", "3", "0");
	sendcommand("SET CUTTER OFF");
	sendcommand("DIRECTION 1");
	sendcommand("GAP 5 mm");
	sendcommand("REFERENCE -170,0");
	nobackfeed();
	clearbuffer();
	// Draw outer box. BOX command params (all in dots - 200DPI: 1 mm = 8 dots): StartX, StartY, EndX, EndY, Line Thk
	sendcommand("BOX 16,48,764,592,5");
	// Draw 5 horiz lines
	sendcommand("BAR 20,128,742,4");// StartX, StartY, Line Width, Line Thk
	sendcommand("BAR 20,234,742,4");// StartX, StartY, Line Width, Line Thk
	sendcommand("BAR 20,320,742,4");// StartX, StartY, Line Width, Line Thk
	sendcommand("BAR 20,416,742,4");// StartX, StartY, Line Width, Line Thk
	sendcommand("BAR 20,512,742,4");// StartX, StartY, Line Width, Line Thk
	// Draw a box for the lone vertical line as it seems we cannot set orientation of line and max thk is 12 mm
	sendcommand("BOX 16,128,228,572,4");// StartX, StartY, EndX, EndY, Line Thk
	// Print Logo 
	if (m_bPrintLogo)
	{
		if (!m_bLogoUploadedToPrinter)
		{
			clearbuffer(); //Clear image buffer
			std::string strFileName(CW2A(m_strLogoFile.GetString()));
			nRetVal = downloadpcx((char*)(strFileName.c_str()), "LOGOCLEXP.PCX");  //Download PCX file into printer
			if (nRetVal)
			{
				nRetVal = sendcommand("MOVE");
				if (nRetVal)
					m_bLogoUploadedToPrinter = TRUE;
				else
				{
					IOutputLog(L"PrintTestLabel() Couldn't MOVE logo in printer!");
					nRetVal = closeport();
					return FALSE;
				}
			}
			else
			{
				IOutputLog(L"PrintTestLabel() Couldn't upload logo to printer!");
				nRetVal = closeport();
				return FALSE;
			}
		}
		if (m_bLogoUploadedToPrinter)
			nRetVal = sendcommand("PUTPCX 320,76,\"LOGOCLEXP.PCX\"");
		else
		{
			AfxMessageBox(L"PrintTestLabel() Couldn't upload / MOVE logo to printer!");
			nRetVal = closeport();
			return FALSE;
		}
	}
	else
	{
		// Write Corporate Name - whether Chinese or English
		if (pApp->m_pFrame->m_pImageView->IsStringChinese(m_strCorporateName))
			nRetVal = printerfont("320", "76", "TSS24.BF2", "0", "2", "2", CW2A(m_strCorporateName.GetBuffer()));
		else
			nRetVal = printerfont("320", "76", "4", "0", "1.2", "1.2", CW2A(m_strCorporateName.GetBuffer()));
	}
	if (nRetVal)
		IOutputLog(L"PrintTestLabel() after printing logo");
	else
	{
		IOutputLog(L"PrintTestLabel() printing logo failed!");
		AfxMessageBox(L"Printing Logo Failed!");
		nRetVal = closeport();
		return FALSE;
	}
	// Write Product Name Legend - First Chinese, then English
	strUni.Format(L"产品名称");
	nRetVal = printerfont("72", "154", "TSS24.BF2", "0", "1", "1", CW2A(strUni.GetBuffer()));//72
	//nRetVal = printerfont("28", "184", "3", "0", "1", "1", "Product Name");//28
	// Write Product Name - whether Chinese or English
	if (pApp->m_pFrame->m_pImageView->IsStringChinese(m_strProductName))
		nRetVal = printerfont("456", "174", "TSS24.BF2", "0", "1.5", "1.5", CW2A(m_strProductName.GetBuffer()));//384
	else
		nRetVal = printerfont("456", "174", "3", "0", "1.5", "1.5", CW2A(m_strProductName.GetBuffer()));//384
	// Write Bar code Legend - First Chinese, then English
	strUni.Format(L"条  码");
	nRetVal = printerfont("64", "248", "TSS24.BF2", "0", "1", "1", CW2A(strUni.GetBuffer()));//64
	nRetVal = printerfont("56", "278", "3", "0", "1", "1", "Bar Code");//56
	// Write Bar Code - Start point X, Y, Code type, height (dots), print text, Rot Angle, Scale X, Scale Y, Text
	str.Format("1234567890");
	barcode("416", "240", "128", "56", "1", "0", "1", "1", str.GetBuffer(str.GetLength()));// strUni does not work!//344
	// Write Specifications Legend- First Chinese, then English
	strUni.Format(L" 测   量");
	nRetVal = printerfont("64", "342", "TSS24.BF2", "0", "1", "1", CW2A(strUni.GetBuffer()));//64
	nRetVal = printerfont("24", "372", "2", "0", "1", "1", "Specifications");//
	// Write Measurements - Line 1
	str.Format("L=%dmm W=%dmm H=%dmm", 123, 234, 345);
	//windowsfont(248, 344, 40, 0, 0, 0, "Arial", str.GetBuffer(str.GetLength()));
	nRetVal = printerfont("312", "344", "3", "0", "1", "1", str.GetBuffer(str.GetLength()));//240
	// Write Specifications - Line 2 (Volume)
	strUni.Format(L"V=%2.3fm³", 2.345);
	nRetVal = printerfont("428", "376", "3", "0", "1", "1", CW2A(strUni.GetBuffer()));//356
	// Write Weight Legend- First Chinese, then English
	strUni.Format(L"重  量");
	nRetVal = printerfont("88", "432", "TSS24.BF2", "0", "1", "1", CW2A(strUni.GetBuffer()));//88
	nRetVal = printerfont("72", "468", "3", "0", "1", "1", "Weight");//72
	// Write Weight
	str.Format("%3.2f kg", 123.45);
	//str.Format("%3.2f kg", pApp->m_stInspResultSaved.dWeightGr);
	nRetVal = printerfont("416", "458", "3", "0", "1", "1", str.GetBuffer(str.GetLength()));// X is OK for this!/344
	// Write Date Legend- First Chinese, then English
	strUni.Format(L"日 期");
	nRetVal = printerfont("96", "520", "TSS24.BF2", "0", "1", "1", CW2A(strUni.GetBuffer()));//96
	nRetVal = printerfont("96", "552", "3", "0", "1", "1", "Date");//96
	// Write Date
	CTime time = CTime::GetCurrentTime();
	CString strTotal, strYear, strMonth, strDay;
	strYear.Format(L"%d年", time.GetYear());
	strMonth.Format(L"%d月", time.GetMonth());
	strDay.Format(L"%d日", time.GetDay());
	strTotal = strYear + strMonth + strDay;
	nRetVal = printerfont("392", "544", "TSS24.BF2", "0", "1", "1", CW2A(strTotal.GetBuffer()));
	//windowsfont(100, 300, 72, 0, 2, 0, "ariel", "Arial Font 72 pt");

	printlabel("1", "1");
	nRetVal = closeport();
	if (nRetVal)
		AfxMessageBox(L"Test Label Printed Successfully!");

	return bResult;
} // PrintTestLabel()

// Prints a test label with test dimensions and weight
// Learning: If logo is printed BEFORE boxes and bars, the boxes and bars are NOT printed. Must do it after logo.
// Logo file name has to be in ALL CAPS. Otherwise will not work! File size should be around 2 KB. 
// If we have png or bmp file use IrfanView to convert to PCX. 
// Use monochrome pallet to reduce file size with 'Decrease Color Depth' to use black/white pallet.
BOOL CDlgPrintParams::PrintTestLabelCLExpress()
{
	BOOL bResult = TRUE;
	UpdateData(TRUE);
	CPMSHJApp* pApp = (CPMSHJApp*)AfxGetApp();
	CStringA str, str1, str2;
	CString strUni;
	std::string strPrinter = CW2A(m_strPrinterName);
	CString strMsg;
	strMsg.Format(L"Printer Name: %s", strPrinter.c_str());
	IOutputLogString(strMsg);
	int nRetVal = openport((char*)strPrinter.c_str());// This Works!
	if (nRetVal == 0)
	{
		IOutputLog(L"openport() failed!");
		return FALSE;
	}
	if (!pApp->m_pFrame->m_pImageView->CheckPrinterStatus())
	{
		AfxMessageBox(L"CheckPrinterStatus() returned FALSE!");
		nRetVal = closeport();
		return FALSE;
	}
	IOutputLog(L"PrintTestLabelCLExpress() Port Opened, Status OK!");
	// label width mm, label height, speed, printing density (0-15), type of sensor (0-gap sensor), Vert spacing, offset
	setup("70 mm", "100 mm", "3", "10", "0", "3", "0");
	sendcommand("SET CUTTER OFF");
	sendcommand("DIRECTION 0");
	sendcommand("GAP 5 mm");
	sendcommand("REFERENCE -32,12");// For Direction 0 Reference X is -32 For DIRECTION 1, Referece is 64,12
	nobackfeed();
	clearbuffer();
	// If we draw boxes and bars before drawing Logo they don't get printed!
	// Print Logo 
	if (m_bPrintLogo)
	{
		std::string strFileName(CW2A(m_strLogoFile.GetString()));
		strMsg.Format(L"Logo File: %s", strFileName.c_str());
		IOutputLogString(strMsg);
		clearbuffer(); //Clear image buffer
		if (strFileName.find(".PCX") != string::npos)// Found ".PCX"
		{
			//if (!m_bLogoUploadedToPrinter)
			{
				//nRetVal = sendcommand("KILL \"*\"");
				nRetVal = downloadpcx((char*)(strFileName.c_str()), "LOGOCLEXP.PCX");  //Download PCX file into printer
				if (nRetVal)
				{
					nRetVal = sendcommand("MOVE");
					if (nRetVal)
						m_bLogoUploadedToPrinter = TRUE;
					else
					{
						IOutputLog(L"PrintTestLabelCLExp() Couldn't MOVE logo in printer!");
						nRetVal = closeport();
						m_bLogoUploadedToPrinter = FALSE;
						return FALSE;
					}
				}
				else
				{
					IOutputLog(L"PrintTestLabelCLExp() Couldn't upload logo to printer!");
					nRetVal = closeport();
					m_bLogoUploadedToPrinter = FALSE;
					return FALSE;
				}
			}
			if (m_bLogoUploadedToPrinter)
				nRetVal = sendcommand("PUTPCX 208,24,\"LOGOCLEXP.PCX\"");
			else
			{
				AfxMessageBox(L"PrintTestLabelCLExp() Couldn't print logo!");
				m_bLogoUploadedToPrinter = FALSE;// Ensure the file gets transferred on next call
				nRetVal = closeport();
				return FALSE;
			}
		}
	}
	else
	{
		IOutputLog(L"PrintLabelCLExpress() Not printing logo");
		// Write Corporate Name - whether Chinese or English
		if (pApp->m_pFrame->m_pImageView->IsStringChinese(m_strCorporateName))
			nRetVal = printerfont("288", "36", "TSS24.BF2", "0", "2", "2", CW2A(m_strCorporateName.GetBuffer()));
		else
			nRetVal = printerfont("288", "36", "4", "0", "1.2", "1.2", CW2A(m_strCorporateName.GetBuffer()));
	}
	if (nRetVal)
		IOutputLog(L"PrintTestLabelCLExp() after printing logo");
	else
	{
		IOutputLog(L"PrintTestLabelCLExp() printing logo failed!");
		AfxMessageBox(L"Printing Logo Failed!");
		nRetVal = closeport();
		return FALSE;
	}
	sendcommand("BOX 48,104,560,760,5");
	// Draw 3 horiz lines
	sendcommand("BAR 50,194,512,4");// StartX, StartY, Line Width, Line Thk
	sendcommand("BAR 50,480,512,4");
	sendcommand("BAR 50,576,512,4");
	//sendcommand("BAR 50,652,512,4");// StartX, StartY, Line Width, Line Thk
	// Draw a box for the lone vertical line as it seems we cannot set orientation of line and max thk is 12 mm
	sendcommand("BOX 48,194,216,760,4");// StartX, StartY, EndX, EndY, Line Thk

	// Write Volume Data Legend - First Chinese, then English
	strUni.Format(L"体积数据");
	nRetVal = printerfont("80", "286", "TSS24.BF2", "0", "1", "1", CW2A(strUni.GetBuffer()));
	nRetVal = printerfont("76", "322", "3", "0", "1", "1", "Volume ");
	// Write Weight Legend- First Chinese, then English
	strUni.Format(L"实际重量");
	nRetVal = printerfont("88", "500", "TSS24.BF2", "0", "1", "1", CW2A(strUni.GetBuffer()));
	nRetVal = printerfont("88", "536", "2", "0", "1", "1", "Weight");
	// Write Freight Legend- First Chinese, then English
	//strUni.Format(L"运费");
	//nRetVal = printerfont("104", "578", "TSS24.BF2", "0", "1", "1", CW2A(strUni.GetBuffer()));
	//nRetVal = printerfont("88", "612", "2", "0", "1", "1", "Freight");
	// Write Date Legend- First Chinese, then English
	strUni.Format(L"日 期");
	nRetVal = printerfont("96", "596", "TSS24.BF2", "0", "1", "1", CW2A(strUni.GetBuffer()));
	nRetVal = printerfont("96", "634", "3", "0", "1", "1", "Date");
	// Write Bar Code - Start point X, Y, Code type, height (dots), print text, Rot Angle, Scale X, Scale Y, Text
	//if (InspResult.strBarcode.GetLength() > m_pApp->m_stInspParams.nBarcodeLength)
	//	InspResult.strBarcode = InspResult.strBarcode.Left(m_pApp->m_stInspParams.nBarcodeLength);
	// If chars are Chinese can't encode in 1D barcode!
	//if (!pApp->m_pFrame->m_pImageView->IsStringChinese(InspResult.strBarcode))
	{
		str = "1234267890ABCDEF";
		//	StartX, StartY, Code Type, Code Ht, Print text, RotDeg, scaleX, scaleY, code text
		barcode("232", "120", "128", "56", "1", "0", "1", "1", str.GetBuffer(str.GetLength()));// strUni does not work!
	}
	// Write Measurements - Line 1 (Length)
	strUni.Format(L"长");
	nRetVal = printerfont("280", "218", "TSS24.BF2", "0", "1", "1", CW2A(strUni.GetBuffer()));
	str.Format("(L) = %2.1fcm", 123.4);
	nRetVal = printerfont("308", "218", "3", "0", "1", "1", str.GetBuffer(str.GetLength()));
	// Write Measurements - Line 2 (Width)
	strUni.Format(L"宽");
	nRetVal = printerfont("280", "288", "TSS24.BF2", "0", "1", "1", CW2A(strUni.GetBuffer()));
	str.Format("(W) = %2.1fcm", 234.5);
	nRetVal = printerfont("308", "288", "3", "0", "1", "1", str.GetBuffer(str.GetLength()));
	// Write Measurements - Line 3 (Height)
	strUni.Format(L"高");
	nRetVal = printerfont("280", "336", "TSS24.BF2", "0", "1", "1", CW2A(strUni.GetBuffer()));
	str.Format("(H) = %2.1fcm", 456.7);
	nRetVal = printerfont("308", "336", "3", "0", "1", "1", str.GetBuffer(str.GetLength()));
	// Write Measurements - Line 4 (Volume)
	// If air freight write volume weight
	if (pApp->m_stFreightParams.FreightMode== FREIGHT_AIR)
	{
		strUni.Format(L"体积重");
		nRetVal = printerfont("264", "440", "TSS24.BF2", "0", "1", "1", CW2A(strUni.GetBuffer()));//408
		str.Format("  = %3.2fkg", 1.23);
		nRetVal = printerfont("316", "440", "3", "0", "1", "1", str.GetBuffer(str.GetLength()));//408
	}
	else // Marine - write normal volume
	{
		strUni.Format(L"体积");
		nRetVal = printerfont("264", "440", "TSS24.BF2", "0", "1", "1", CW2A(strUni.GetBuffer()));//408//448
		str.Format("(V) = %2.2fm3", 1.23);
		nRetVal = printerfont("316", "440", "3", "0", "1", "1", str.GetBuffer(str.GetLength()));//408
	}
	// Write Weight
	str.Format("%3.2f Kg", 567.8);
	nRetVal = printerfont("332", "500", "3", "0", "1", "1", str.GetBuffer(str.GetLength()));

	// Write Date
	CTime time = CTime::GetCurrentTime();
	CString strTotal, strYear, strMonth, strDay;
	strYear.Format(L"%d年-", time.GetYear());
	strMonth.Format(L"%d月-", time.GetMonth());
	strDay.Format(L"%d日", time.GetDay());
	strTotal = strYear + strMonth + strDay;
	nRetVal = printerfont("272", "600", "TSS24.BF2", "0", "1.2", "1.2", CW2A(strTotal.GetBuffer()));
	//windowsfont(100, 300, 72, 0, 2, 0, "ariel", "Arial Font 72 pt");

	printlabel("1", "1");
	nRetVal = closeport();
	if (nRetVal)
	{
		AfxMessageBox(L"Test Label Printed Successfully!");
		IOutputLog(L"Test Label Printed Successfully!");
	}

	return bResult;
} // PrintTestLabelCLExpress()

// Not Used!
// Prints a test label with test dimensions and weight
BOOL CDlgPrintParams::PrintTestLabelByFileTransfer()
{
	BOOL bResult = TRUE;
	CPMSHJApp* pApp = (CPMSHJApp*)AfxGetApp();
	CStringA str;
	CStringA strPrintFile;// = CreatePrintFile(pApp->m_stDeviceInfo.strDeviceID);
	strPrintFile.Format("%s\\MasterPrintFile.txt", CW2A(pApp->GetProjectDirectory()));
	if (strPrintFile.IsEmpty())
		return FALSE;
	CStringA strExeName;
	strExeName.Format("COMMAND.com /c copy");
	CStringA strTargetFileName;
	strTargetFileName = CW2A(m_strPrinterName);
	CStringA strCmdLine;
	strPrintFile = "d:\\CurFile.txt";// File name cannot be more than 8.3 chars - not case sensitive
	strCmdLine.Format("%s %s %s", strExeName, strPrintFile, strTargetFileName);
	if (!strPrintFile.IsEmpty())
	{
		int nRetVal = WinExec(strCmdLine, SW_HIDE);//SW_HIDE
		if (nRetVal == 0)
			AfxMessageBox(L"Error Occured while printing label - System Out of Memory!");
		else if (nRetVal == ERROR_BAD_FORMAT)
			AfxMessageBox(L"Error Occured while printing label - exe invalid!");
		else if (nRetVal == ERROR_FILE_NOT_FOUND)
			AfxMessageBox(L"Error Occured while printing label - Specified File Was Not Found!");
		else if (nRetVal == ERROR_PATH_NOT_FOUND)
			AfxMessageBox(L"Error Occured while printing label - Specified Path Was Not Found!");
	}
	return bResult;
}

CStringA CDlgPrintParams::GetDateStringForPrinter()
{
	CTime time = CTime::GetCurrentTime();
	CStringA str;
	str.Format("%d%d%d", time.GetYear(), time.GetMonth(), time.GetDay());
	return str;
}

void CDlgPrintParams::OnBnClickedChkLogo()
{
	UpdateData(TRUE);
	// If logo is turned ON we need to get logo file name and transfer it to printer
	if (m_bPrintLogo)
		((CWnd*)GetDlgItem(IDC_BN_LOGO_FILE))->EnableWindow(TRUE);
	else
		((CWnd*)GetDlgItem(IDC_BN_LOGO_FILE))->EnableWindow(FALSE);
}


void CDlgPrintParams::OnBnClickedBnLogoFile()
{
	CString strLoadMessage;
	static TCHAR BASED_CODE szFilter[] = _TEXT("Image Files (*.*;) | *.*; ||");

	CFileDialog FileDlg(TRUE,
		NULL,
		NULL,
		OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST,
		szFilter,
		NULL);
	CString strImageFileName;
	if (FileDlg.DoModal() != IDCANCEL)
	{
		m_strLogoFile = FileDlg.GetPathName();
		m_staticLogoFile.SetWindowTextW(m_strLogoFile);
		std::string strFileName(CW2A(m_strLogoFile.GetString()));
		if (strFileName.find(".PCX") != string::npos)// Found ".PCX"
		{
			std::string strPrinter = CW2A(m_strPrinterName);
			// We are printing label in thread now and do not need to transfer logo file to printer memory!
			//nRetVal = openport((char*)strPrinter.c_str());// This Works!
			//if (nRetVal == 0)
			//{
			//	AfxMessageBox(L"Printer Port Couldn't be Opened");
			//	return;
			//}
			//int nRetVal = sendcommand("KILL \"*\"");
			//if (nRetVal == 0)
			//{
			//	IOutputLog(L"Could not KILL files");
			//}
			//clearbuffer();
			//nRetVal = downloadpcx((char*)(strFileName.c_str()), "LOGOCLEXP.PCX");  //Download PCX file into printer
			//if (nRetVal)
			//{
			//	IOutputLog(L"File downloaded to printer");
			//	// Move file to flash memory so it is available even after power off
			//	nRetVal = sendcommand("MOVE");
			//	if (nRetVal)
			//	{
			//		IOutputLog(L"Logo file moved to flash!");
			//		m_staticLogoFile.SetWindowTextW(m_strLogoFile);
			//		m_bLogoUploadedToPrinter = TRUE;
			//	}
			//	else
			//		AfxMessageBox(L"Logo file could not be moved to flash!");
			//}
			//else
			//	AfxMessageBox(L"Logo file could not be downloaded to printer!");
			//nRetVal = closeport();
			//if (nRetVal)
			//	AfxMessageBox(L"Printer Port Closed Successfully!");
		}
	}
}
