// ThreadPrintLabel.cpp : implementation file
//

#include "stdafx.h"
#include "PMSHJ.h"
#include "ThreadPrintLabel.h"
#include "tsclib.h"


// CThreadPrintLabel

IMPLEMENT_DYNCREATE(CThreadPrintLabel, CWinThread)

CThreadPrintLabel::CThreadPrintLabel()
{
}

CThreadPrintLabel::CThreadPrintLabel(STPrintParams stPrintParams, HANDLE hEndPrintThread, HANDLE hPrintLabel, HANDLE hPrintTestLabel)
{
	m_stPrintParams = stPrintParams;
	m_hEndPrintLabelThread = hEndPrintThread;
	m_hPrintLabel = hPrintLabel;
	m_hPrintTestLabel = hPrintTestLabel;
	m_bPrinterPortOpened = m_bLogoUploadedToPrinter = FALSE;
	m_pApp = (CPMSHJApp*)AfxGetApp();
}

CThreadPrintLabel::~CThreadPrintLabel()
{
}

BOOL CThreadPrintLabel::InitInstance()
{
	// Open Printer port
	CString strMsg;
	strMsg.Format(L"PrintLabel Thread Printer: %s, Print Logo: %d, Logo File: %s", 
		m_stPrintParams.strPrinterName, m_stPrintParams.bPrintLogo, m_stPrintParams.strLogoFile);
	IOutputLogString(strMsg);

	while (WaitForSingleObject(m_hEndPrintLabelThread, 5) == WAIT_TIMEOUT)
	{
		while (WaitForSingleObject(m_hPrintLabel, 5) != WAIT_TIMEOUT)
		{
			m_InspResult = m_pApp->m_stInspResultSaved;
			PrintLabelCLExpress(m_InspResult);
		}
		while (WaitForSingleObject(m_hPrintTestLabel, 5) != WAIT_TIMEOUT)
		{
			PrintTestLabel();
		}
		MSG msg;
		while (::PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
		{
			if (!m_pApp->PumpMessage()) //if WM_QUIT i.e User exited the application
			{
				closeport();
				::PostQuitMessage(0);
				ResetEvent(m_hEndPrintLabelThread);
				IOutputLog(L"Quit Application Triggered - Aborting PrintLabel Thread!");
				return FALSE;// FT_APPLN_QUIT;
			}
		}
	}

	if (m_bPrinterPortOpened)
		int nRetVal = closeport();

	IOutputLog(L"End Thread Triggered - Aborting PrintLabel Thread!");

	ResetEvent(m_hEndPrintLabelThread);

	return TRUE;
}

int CThreadPrintLabel::ExitInstance()
{
	// TODO:  perform any per-thread cleanup here
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CThreadPrintLabel, CWinThread)
END_MESSAGE_MAP()


// CThreadPrintLabel message handlers
BOOL CThreadPrintLabel::PrintLabelCLExpress(STInspResult InspResult)
{
	BOOL bResult = TRUE;
	CPMSHJApp* pApp = (CPMSHJApp*)AfxGetApp();
	CStringA str, str1, str2;
	CString strUni;
	std::string strPrinter = CW2A(m_pApp->m_stPrintParams.strPrinterName);
	int nRetVal = 0;
	//IOutputLog(L"Opening Printer Port!");
	if (!m_bPrinterPortOpened)
	{
		int nRetVal = openport((char*)strPrinter.c_str());// This Works!
		if (nRetVal == 0)
			return FALSE;
		if (!pApp->m_pFrame->m_pImageView->CheckPrinterStatus())
		{
			IOutputLog(L"CheckPrinterStatus() returned FALSE!");
			nRetVal = closeport();
			return FALSE;
		}
		m_bPrinterPortOpened = TRUE;
	}
	IOutputLog(L"In Thread PrintLabelCLExpress() Port Opened, Status OK!");
	// label width mm, label height, speed, printing density (0-15), type of sensor (0-gap sensor), Vert spacing, offset
	setup("70 mm", "100 mm", "3", "10", "0", "3", "0");
	sendcommand("SET CUTTER OFF");
	sendcommand("DIRECTION 0");// was 1, 0
	sendcommand("GAP 5 mm");
	sendcommand("REFERENCE -32,12");// For DIRECTION 1, Referece is 64,12
	nobackfeed();
	clearbuffer();
	// If we draw boxes and bars before drawing Logo they don't get printed!
	// Print Logo
	if (m_pApp->m_stPrintParams.bPrintLogo)
	{
		std::string strFileName(CW2A(m_pApp->m_stPrintParams.strLogoFile.GetString()));
		clearbuffer(); //Clear image buffer
		if (strFileName.find(".PCX") != string::npos)// Found ".PCX"
		{
			if (!m_bLogoUploadedToPrinter)
			{
				nRetVal = sendcommand("KILL \"*\"");
				nRetVal = downloadpcx((char*)(strFileName.c_str()), "LOGOCLEXP.PCX");  //Download PCX file into printer
				if (nRetVal)
				{
					nRetVal = sendcommand("MOVE");
					if (nRetVal)
						m_bLogoUploadedToPrinter = TRUE;
					else
					{
						IOutputLog(L"PrintLabel() Couldn't MOVE logo in printer!");
						nRetVal = closeport();
						m_bLogoUploadedToPrinter = FALSE;
						return FALSE;
					}
				}
				else
				{
					IOutputLog(L"Thread PrintLabel() Couldn't upload logo to printer!");
					nRetVal = closeport();
					m_bLogoUploadedToPrinter = FALSE;
					return FALSE;
				}
			}
			if (m_bLogoUploadedToPrinter)
				nRetVal = sendcommand("PUTPCX 208,24,\"LOGOCLEXP.PCX\"");
			else
			{
				IOutputLog(L"Thread PrintLabel() Couldn't upload / MOVE logo to printer!");
				AfxMessageBox(L"PrintLabel() Couldn't upload / MOVE logo to printer!");
				nRetVal = closeport();
				return FALSE;
			}
		}
		else if (strFileName.find(".BMP") != string::npos)// Found ".BMP"
		{
			nRetVal = sendcommand("PUTBMP 208,12,\"LOGOCLEXPBMP.BMP\"");
			if (!nRetVal)
				IOutputLog(L"Thread PrintLabel() - Printing BMP logo failed");
		}
	}
	else
	{
		IOutputLog(L"Thread PrintLabelCLExpress() Not printing logo");
		// Write Corporate Name - whether Chinese or English
		if (pApp->m_pFrame->m_pImageView->IsStringChinese(m_pApp->m_stPrintParams.strCorporateName))
			nRetVal = printerfont("288", "36", "TSS24.BF2", "0", "2", "2", CW2A(m_pApp->m_stPrintParams.strCorporateName.GetBuffer()));
		else
			nRetVal = printerfont("288", "36", "4", "0", "1.2", "1.2", CW2A(m_pApp->m_stPrintParams.strCorporateName.GetBuffer()));
	}
	if (nRetVal)
		IOutputLog(L"Thread PrintLabelCLExpress() after printing logo");
	else
	{
		IOutputLog(L"Thread PrintLabelCLExpress() printing logo failed!");
		AfxMessageBox(L"Printing Logo Failed!");
		nRetVal = closeport();
		return FALSE;
	}
	sendcommand("BOX 48,104,560,760,5");
	//sendcommand("BOX 56,120,450,550,8");
	// Draw 5 horiz lines
	sendcommand("BAR 50,178,512,4");// StartX, StartY, Line Width, Line Thk
	sendcommand("BAR 50,464,512,4");// StartX, StartY, Line Width, Line Thk
	sendcommand("BAR 50,560,512,4");// StartX, StartY, Line Width, Line Thk
	sendcommand("BAR 50,652,512,4");// StartX, StartY, Line Width, Line Thk
									// Draw a box for the lone vertical line as it seems we cannot set orientation of line and max thk is 12 mm
	sendcommand("BOX 48,104,216,760,4");// StartX, StartY, EndX, EndY, Line Thk

										// Write Product Name Legend - First Chinese, then English
	strUni.Format(L"产品名称");
	// StartX, StartY, Font (1: 8*12, 2:12*20, 3:16*24, 4:24*32, 5:32*48), Rot Angle, ScaleX, SclaeY, Text
	nRetVal = printerfont("84", "128", "TSS24.BF2", "0", "1", "1", CW2A(strUni.GetBuffer()));
	// Write Product Name - whether Chinese or English
	if (IsStringChinese(m_pApp->m_stPrintParams.strProductName))
		nRetVal = printerfont("280", "128", "TSS24.BF2", "0", "1.5", "1.5", CW2A(m_pApp->m_stPrintParams.strProductName.GetBuffer()));
	else
		nRetVal = printerfont("280", "128", "3", "0", "1.5", "1.5", CW2A(m_pApp->m_stPrintParams.strProductName.GetBuffer()));
	// Write Volume Data Legend - First Chinese, then English
	strUni.Format(L"体积数据");
	nRetVal = printerfont("80", "288", "TSS24.BF2", "0", "1", "1", CW2A(strUni.GetBuffer()));
	nRetVal = printerfont("76", "324", "3", "0", "1", "1", "Volume ");
	// Write Weight Legend- First Chinese, then English
	strUni.Format(L"重量");
	nRetVal = printerfont("104", "484", "TSS24.BF2", "0", "1", "1", CW2A(strUni.GetBuffer()));
	nRetVal = printerfont("88", "520", "2", "0", "1", "1", "Weight");
	// Write Freight Legend- First Chinese, then English
	strUni.Format(L"运费");
	nRetVal = printerfont("104", "578", "TSS24.BF2", "0", "1", "1", CW2A(strUni.GetBuffer()));
	nRetVal = printerfont("88", "612", "2", "0", "1", "1", "Freight");
	// Write Date Legend- First Chinese, then English
	strUni.Format(L"日 期");
	nRetVal = printerfont("96", "678", "TSS24.BF2", "0", "1", "1", CW2A(strUni.GetBuffer()));
	nRetVal = printerfont("96", "712", "3", "0", "1", "1", "Date");
	// Write Measurements - Line 1 (Length)
	strUni.Format(L"长");
	nRetVal = printerfont("280", "192", "TSS24.BF2", "0", "1", "1", CW2A(strUni.GetBuffer()));
	str.Format("(L) = %dmm", (int)(InspResult.dLength));
	nRetVal = printerfont("308", "192", "3", "0", "1", "1", str.GetBuffer(str.GetLength()));
	// Write Measurements - Line 2 (Width)
	strUni.Format(L"宽");
	nRetVal = printerfont("280", "264", "TSS24.BF2", "0", "1", "1", CW2A(strUni.GetBuffer()));
	str.Format("(W) = %dmm", (int)(InspResult.dWidth));
	nRetVal = printerfont("308", "264", "3", "0", "1", "1", str.GetBuffer(str.GetLength()));
	// Write Measurements - Line 3 (Height)
	strUni.Format(L"高");
	nRetVal = printerfont("280", "336", "TSS24.BF2", "0", "1", "1", CW2A(strUni.GetBuffer()));
	str.Format("(H) = %dmm", (int)(InspResult.dHeight));
	nRetVal = printerfont("308", "336", "3", "0", "1", "1", str.GetBuffer(str.GetLength()));
	// Write Measurements - Line 4 (Volume)
	strUni.Format(L"体积");
	nRetVal = printerfont("256", "408", "TSS24.BF2", "0", "1", "1", CW2A(strUni.GetBuffer()));
	str.Format("(V) = %2.2fm3", InspResult.dVolume);
	nRetVal = printerfont("308", "408", "3", "0", "1", "1", str.GetBuffer(str.GetLength()));
	// Write Weight
	str.Format("%3.2f Kg", InspResult.dWeightNet);
	nRetVal = printerfont("332", "500", "3", "0", "1", "1", str.GetBuffer(str.GetLength()));
	// Write Freight
	double dFreight = InspResult.dFreight;
	str.Format("$%3.3f", dFreight);
	nRetVal = printerfont("332", "600", "3", "0", "1", "1", str.GetBuffer(str.GetLength()));
	// Write Date
	CTime time = CTime::GetCurrentTime();
	CString strTotal, strYear, strMonth, strDay;
	strYear.Format(L"%d年-", time.GetYear());
	strMonth.Format(L"%d月-", time.GetMonth());
	strDay.Format(L"%d日", time.GetDay());
	strTotal = strYear + strMonth + strDay;
	nRetVal = printerfont("272", "692", "TSS24.BF2", "0", "1.2", "1.2", CW2A(strTotal.GetBuffer()));
	//windowsfont(100, 300, 72, 0, 2, 0, "ariel", "Arial Font 72 pt");

	printlabel("1", "1");
	//nRetVal = closeport();
	//if (nRetVal)
	IOutputLog(L"Thread Label Printed Successfully!");

	return bResult;
} // PrintLabelCLExpress()

BOOL CThreadPrintLabel::IsStringChinese(CString str)
{
	BOOL bResult = FALSE;
	for (int i = 0; i < str.GetLength(); i++)
	{
		TCHAR c = str[i];
		if ((unsigned int)c >= 0x4E00u && ((unsigned int)c <= 0x9FFFu))
		{
			return TRUE;
		}
	}
	return bResult;
} // IsStringChinese()

BOOL CThreadPrintLabel::PrintTestLabel()
{
	BOOL bResult = TRUE;
	CPMSHJApp* pApp = (CPMSHJApp*)AfxGetApp();
	CStringA str, str1, str2;
	CString strUni;
	std::string strPrinter = CW2A(m_pApp->m_stPrintParams.strPrinterName);
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
	if (m_pApp->m_stPrintParams.bPrintLogo)
	{
		if (!m_bLogoUploadedToPrinter)
		{
			clearbuffer(); //Clear image buffer
			std::string strFileName(CW2A(m_pApp->m_stPrintParams.strLogoFile.GetString()));
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
		if (pApp->m_pFrame->m_pImageView->IsStringChinese(m_pApp->m_stPrintParams.strCorporateName))
			nRetVal = printerfont("320", "76", "TSS24.BF2", "0", "2", "2", CW2A(m_pApp->m_stPrintParams.strCorporateName.GetBuffer()));
		else
			nRetVal = printerfont("320", "76", "4", "0", "1.2", "1.2", CW2A(m_pApp->m_stPrintParams.strCorporateName.GetBuffer()));
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
	if (pApp->m_pFrame->m_pImageView->IsStringChinese(m_pApp->m_stPrintParams.strProductName))
		nRetVal = printerfont("456", "174", "TSS24.BF2", "0", "1.5", "1.5", CW2A(m_pApp->m_stPrintParams.strProductName.GetBuffer()));//384
	else
		nRetVal = printerfont("456", "174", "3", "0", "1.5", "1.5", CW2A(m_pApp->m_stPrintParams.strProductName.GetBuffer()));//384
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
	//nRetVal = closeport();
	if (nRetVal)
		AfxMessageBox(L"Test Label Printed Successfully!");

	return bResult;
} // PrintTestLabel()