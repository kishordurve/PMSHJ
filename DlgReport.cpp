// DlgReport.cpp : implementation file
//

#include "stdafx.h"
#include "PMSHJ.h"
#include "DlgReport.h"
#include "afxdialogex.h"


// CDlgReport dialog

IMPLEMENT_DYNAMIC(CDlgReport, CDialogEx)

CDlgReport::CDlgReport(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DLG_REPORT, pParent)
	, m_strSelTransporter(_T(""))
	, m_nEnterTransNo(1)
{
	m_nCurSelIndex = 0;
	m_bCodeTestingON = FALSE;
}

CDlgReport::~CDlgReport()
{
}

void CDlgReport::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_DATABASE, m_ListControl);
	DDX_Control(pDX, IDC_BN_EXPORT_TO_XLS, m_BnExport);
	DDX_Control(pDX, IDC_BN_SCAN_BARCODE, m_BnScanBarcode);
	DDX_Control(pDX, IDC_BN_SHOW_RECORDS, m_BnShowRecords);
	DDX_Control(pDX, IDCANCEL, m_BnCancel);
	DDX_Control(pDX, IDC_BN_DELETE_RECORD, m_BnDeleteRecord);
	DDX_Control(pDX, IDC_BN_FETCH_RECORDS, m_BnFetchRecords);
	DDX_Text(pDX, IDC_EDIT_TRANS_NO, m_strSelTransporter);
	DDX_Radio(pDX, IDC_RADIO_TRAN_NUM, m_nEnterTransNo);
	DDV_MinMaxInt(pDX, m_nEnterTransNo, 0, 1);
	DDX_Control(pDX, IDC_STATIC_SEL_PKG, m_StaticPkgNumber);
}


BEGIN_MESSAGE_MAP(CDlgReport, CDialogEx)
	ON_BN_CLICKED(IDC_BN_SHOW_RECORDS, &CDlgReport::OnBnClickedBnShowRecords)
	ON_BN_CLICKED(IDC_BN_EXPORT_TO_XLS, &CDlgReport::OnBnClickedBnExportToXls)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BN_SCAN_BARCODE, &CDlgReport::OnBnClickedBnScanBarcode)
	ON_BN_CLICKED(IDC_BN_DELETE_RECORD, &CDlgReport::OnBnClickedBnDeleteRecord)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_DATABASE, &CDlgReport::OnLvnItemchangedListDatabase)
	ON_BN_CLICKED(IDC_BN_FETCH_RECORDS, &CDlgReport::OnBnClickedBnFetchRecords)
	ON_BN_CLICKED(IDC_RADIO_TRAN_NUM, &CDlgReport::OnBnClickedRadioTranNum)
	ON_BN_CLICKED(IDC_RADIO2, &CDlgReport::OnBnClickedTranIDScanBarcode)
END_MESSAGE_MAP()

// Fetches all records from database and displays them.
// Need to to show only those for which the function was called.
// pack_no is the primary key.
// 
void CDlgReport::ShowRecords()
{
	CPMSHJApp *pApp = (CPMSHJApp*)AfxGetApp();
	CString SqlString;
	// Build the SQL statement
	SqlString = "SELECT sta_name, data_stamp, wk_date, tran_no, pack_no, pack_l, pack_w, pack_h, pack_v, v_unit, pack_net, pack_gross, pack_tare, pack_ptare, wt_unit, upd_flag, del_mark " "FROM mlog";// FROM Table
	CString str_sta_name, str_data_stamp, str_wk_date, str_tran_no, str_pack_no, str_v_unit, str_pack_l, str_pack_w,
		str_pack_h, str_dim_unit, str_pack_v, str_pack_net, str_pack_gross, str_pack_tare, str_pack_ptare, str_wt_unit, str_upd_flag, str_del_mark;
	double dpack_l, dpack_w, dpack_h, dpack_v, dpack_net, dpack_gross, dpack_tare, dpack_ptare;
	BOOL bupd_flag = FALSE, bdel_mark = FALSE;
	CString sDriver = L"MICROSOFT ACCESS DRIVER (*.mdb)";
	CString sDsn;
	// You must change path if it's different
	CString strDBFilePath = pApp->GetProjectDirectory();
	CString sFile = strDBFilePath + L"\\PMS.mdb";
	//CString sFile = L"D:\\PMS.mdb";

	// Build ODBC connection string
	sDsn.Format(L"ODBC;DRIVER={%s};DSN='';DBQ=%s", sDriver, sFile);
	TRY{
		// Open the database
		BOOL bResult = m_PMSDatabase.Open(NULL,false,false,sDsn);
		CRecordset recset(&m_PMSDatabase);
		recset.Open(CRecordset::forwardOnly, SqlString, CRecordset::readOnly);
		ResetListControl();
		// Column width and heading
		// Create/insert a column. RetVal is index of column added
		int nColNum = m_ListControl.InsertColumn(
			0,              // Rank/order of item 
			L"sta_name",    // Caption for this header 
			LVCFMT_LEFT,    // Alignment of items 
			80);           // Pixel Width of column

	nColNum = m_ListControl.InsertColumn(1, L"date_stamp", LVCFMT_LEFT, -1, 1);
	nColNum = m_ListControl.InsertColumn(2, L"wk_date", LVCFMT_CENTER, -1, 1);
	nColNum = m_ListControl.InsertColumn(3, L"tran_no", LVCFMT_CENTER, -1, 1);
	nColNum = m_ListControl.InsertColumn(4, L"pack_no", LVCFMT_CENTER, -1, 1);// PK
	nColNum = m_ListControl.InsertColumn(5, L"pack_l", LVCFMT_CENTER, -1, 1);
	nColNum = m_ListControl.InsertColumn(6, L"pack_w", LVCFMT_CENTER, -1, 1);
	nColNum = m_ListControl.InsertColumn(7, L"pack_h", LVCFMT_CENTER, -1, 1);
	nColNum = m_ListControl.InsertColumn(8, L"pack_v", LVCFMT_CENTER, -1, 1);
	nColNum = m_ListControl.InsertColumn(9, L"v_unit", LVCFMT_CENTER, -1, 1);
	nColNum = m_ListControl.InsertColumn(10, L"pack_net", LVCFMT_CENTER, -1, 1);
	nColNum = m_ListControl.InsertColumn(11, L"pack_gross", LVCFMT_CENTER, -1, 1);
	nColNum = m_ListControl.InsertColumn(12, L"pack_tare", LVCFMT_CENTER, -1, 1);
	nColNum = m_ListControl.InsertColumn(13, L"Freight (USD)", LVCFMT_CENTER, -1, 1);// was pack_ptare
	nColNum = m_ListControl.InsertColumn(14, L"wt_unit", LVCFMT_CENTER, -1, 1);
	nColNum = m_ListControl.InsertColumn(15, L"Air/Sea", LVCFMT_RIGHT, -1, 1);// upd_flag
	nColNum = m_ListControl.InsertColumn(16, L"del_mark", LVCFMT_RIGHT, -1, 1);
	m_ListControl.SetColumnWidth(0, 65);
	m_ListControl.SetColumnWidth(1, 120);
	m_ListControl.SetColumnWidth(2, 140);
	m_ListControl.SetColumnWidth(3, 90);
	m_ListControl.SetColumnWidth(4, 115);
	m_ListControl.SetColumnWidth(5, 50);
	m_ListControl.SetColumnWidth(6, 50);
	m_ListControl.SetColumnWidth(7, 50);
	m_ListControl.SetColumnWidth(8, 50);
	m_ListControl.SetColumnWidth(9, 45);
	m_ListControl.SetColumnWidth(10, 60);
	m_ListControl.SetColumnWidth(11, 65);
	m_ListControl.SetColumnWidth(12, 65);
	m_ListControl.SetColumnWidth(13, 80);//55
	m_ListControl.SetColumnWidth(14, 50);
	m_ListControl.SetColumnWidth(15, 50);
	m_ListControl.SetColumnWidth(16, 60);
	short nFields = recset.GetODBCFieldCount();
	// Loop through each record
	while (!recset.IsEOF())
	{
		// Copy each column into a variable
		CDBVariant varValue;
		if (recset.IsDeleted())
			continue;
		recset.GetFieldValue(L"sta_name", str_sta_name);
		recset.GetFieldValue(L"data_stamp", str_data_stamp);
		recset.GetFieldValue(L"wk_date", str_wk_date);
		recset.GetFieldValue(L"tran_no", str_tran_no);
		recset.GetFieldValue(L"pack_no", str_pack_no);// PK
		recset.GetFieldValue(L"pack_l", varValue);
		dpack_l = varValue.m_dblVal;
		recset.GetFieldValue(L"pack_w", varValue);
		dpack_w = varValue.m_dblVal;
		recset.GetFieldValue(L"pack_h", varValue);
		dpack_h = varValue.m_dblVal;
		recset.GetFieldValue(L"pack_v", varValue);
		dpack_v = varValue.m_dblVal;
		recset.GetFieldValue(L"v_unit", str_v_unit);
		recset.GetFieldValue(L"pack_net", varValue);
		dpack_net = varValue.m_dblVal;
		recset.GetFieldValue(L"pack_gross", varValue);
		dpack_gross = varValue.m_dblVal;
		recset.GetFieldValue(L"pack_tare", varValue);
		dpack_tare = varValue.m_dblVal;
		recset.GetFieldValue(L"pack_ptare", varValue);
		dpack_ptare = varValue.m_dblVal;
		recset.GetFieldValue(L"wt_unit", str_wt_unit);
		recset.GetFieldValue(L"upd_flag", varValue);// For HJ being used for Air / Sea freight mode
		unsigned char c = varValue.m_chVal;
		if (c == 0)
			bupd_flag = 0;
		else
			bupd_flag = 1;
		recset.GetFieldValue(L"del_mark", varValue);
		c = varValue.m_chVal;
		if (c == 0)
			bdel_mark = 0;
		else
			bdel_mark = 1;
		// Insert values into the list control
		int iRec = 0;
		iRec = m_ListControl.InsertItem(0, str_sta_name, 0);
		m_ListControl.SetItemText(0, 1, str_data_stamp);
		m_ListControl.SetItemText(0, 2, str_wk_date);
		m_ListControl.SetItemText(0, 3, str_tran_no);
		m_ListControl.SetItemText(0, 4, str_pack_no);
		// Forcing to mm
		str_dim_unit = "mm";
		if (str_dim_unit == "mm")
		{
			str_pack_l.Format(L"%4.0f", dpack_l);
			str_pack_w.Format(L"%4.0f", dpack_w);
			str_pack_h.Format(L"%4.0f", dpack_h);
		}
		else if ((str_dim_unit == "cm") || (str_dim_unit == "Cm"))
		{
			str_pack_l.Format(L"%4.1f", dpack_l);
			str_pack_w.Format(L"%4.1f", dpack_w);
			str_pack_h.Format(L"%4.1f", dpack_h);
		}
		else if (str_dim_unit == "M")
		{
			str_pack_l.Format(L"%1.3f", dpack_l);
			str_pack_w.Format(L"%1.3f", dpack_w);
			str_pack_h.Format(L"%1.3f", dpack_h);
		}
		//str_pack_l.Format(L"%4.2f", dpack_l);
		m_ListControl.SetItemText(0, 5, str_pack_l);
		//str_pack_w.Format(L"%4.2f", dpack_w);
		m_ListControl.SetItemText(0, 6, str_pack_w);
		//str_pack_h.Format(L"%4.2f", dpack_h);
		m_ListControl.SetItemText(0, 7, str_pack_h);
		str_pack_v.Format(L"%4.3f", dpack_v);
		m_ListControl.SetItemText(0, 8, str_pack_v);
		m_ListControl.SetItemText(0, 9, str_v_unit);
		str_pack_net.Format(L"%4.2f", dpack_net);
		m_ListControl.SetItemText(0, 10, str_pack_net);
		str_pack_gross.Format(L"%4.2f", dpack_gross);
		m_ListControl.SetItemText(0, 11, str_pack_gross);
		str_pack_tare.Format(L"%4.2f", dpack_tare);
		m_ListControl.SetItemText(0, 12, str_pack_tare);
		str_pack_ptare.Format(L"%4.2f", dpack_ptare);
		m_ListControl.SetItemText(0, 13, str_pack_ptare);
		m_ListControl.SetItemText(0, 14, str_wt_unit);
		// For HJ - being used for freight mode (Air / Sea)
		if (bupd_flag)
			str_upd_flag.Format(L"Air");
		else
			str_upd_flag.Format(L"Sea");
		m_ListControl.SetItemText(0, 15, str_upd_flag);
		str_del_mark.Format(L"%d", bdel_mark);
		m_ListControl.SetItemText(0, 16, str_del_mark);
		// goto next record
		recset.MoveNext();
	}
	recset.Close();
	m_PMSDatabase.Close();

	}CATCH(CDBException, e) {
		// If a database exception occured, show error msg
		AfxMessageBox(L"Database error: " + e->m_strError);
	}
	END_CATCH;

}// CDlgReport message handlers

 // Reset List control
void CDlgReport::ResetListControl()
{
	m_ListControl.DeleteAllItems();
	int iNbrOfColumns = 0;
	CHeaderCtrl* pHeader = (CHeaderCtrl*)m_ListControl.GetDlgItem(0);
	if (pHeader) {
		iNbrOfColumns = pHeader->GetItemCount();
	}
	for (int i = iNbrOfColumns; i >= 0; i--)
	{
		m_ListControl.DeleteColumn(i);
	}
}


void CDlgReport::OnBnClickedBnShowRecords()
{
	ShowRecords();
}

// This func saves all records for a particular transtporter ID
// 
void CDlgReport::OnBnClickedBnExportToXls()
{
	UpdateData(TRUE);
	CPMSHJApp *pApp = (CPMSHJApp*)AfxGetApp();
	CString strCaption, strToolTip;
	std::wstring str = pApp->m_pLanguage->GetString(IDS_STR_ENTER_TRAN_ID);
	strCaption = str.c_str();
	if (m_strSelTransporter.IsEmpty())
	{
		AfxMessageBox(strCaption);
		return;
	}

	CString str_sta_name, str_data_stamp, str_wk_date, str_tran_no, str_pack_no, str_v_unit, str_pack_l, str_pack_w,
		str_pack_h, str_pack_v, str_pack_net, str_pack_gross, str_pack_tare, str_pack_ptare, str_wt_unit, str_upd_flag, str_del_mark;
	double dpack_l, dpack_w, dpack_h, dpack_v, dpack_net, dpack_gross, dpack_tare, dpack_ptare;
	BOOL bupd_flag = FALSE, bdel_mark = FALSE;
	int nNumRecords = 0;
	TRY
	{
		if (!pApp->m_pFrame->m_pImageView->m_pSet->IsOpen())
			pApp->m_pFrame->m_pImageView->m_pSet->Open();
		pApp->m_pFrame->m_pImageView->m_pSet->MoveFirst();
		while (!pApp->m_pFrame->m_pImageView->m_pSet->IsEOF())
		{
			if (pApp->m_pFrame->m_pImageView->m_pSet->m_tran_no == m_strSelTransporter)
			{
				// Fetch values of fields and enter into vars
				// Copy each column into a variable
				CDBVariant varValue;
				pApp->m_pFrame->m_pImageView->m_pSet->GetFieldValue(L"sta_name", str_sta_name);
				pApp->m_pFrame->m_pImageView->m_pSet->GetFieldValue(L"data_stamp", str_data_stamp);
				pApp->m_pFrame->m_pImageView->m_pSet->GetFieldValue(L"wk_date", str_wk_date);
				pApp->m_pFrame->m_pImageView->m_pSet->GetFieldValue(L"tran_no", str_tran_no);
				pApp->m_pFrame->m_pImageView->m_pSet->GetFieldValue(L"pack_no", str_pack_no);// PK
				pApp->m_pFrame->m_pImageView->m_pSet->GetFieldValue(L"pack_l", varValue);
				dpack_l = varValue.m_dblVal;
				pApp->m_pFrame->m_pImageView->m_pSet->GetFieldValue(L"pack_w", varValue);
				dpack_w = varValue.m_dblVal;
				pApp->m_pFrame->m_pImageView->m_pSet->GetFieldValue(L"pack_h", varValue);
				dpack_h = varValue.m_dblVal;
				pApp->m_pFrame->m_pImageView->m_pSet->GetFieldValue(L"pack_v", varValue);
				dpack_v = varValue.m_dblVal;
				pApp->m_pFrame->m_pImageView->m_pSet->GetFieldValue(L"v_unit", str_v_unit);
				pApp->m_pFrame->m_pImageView->m_pSet->GetFieldValue(L"pack_net", varValue);
				dpack_net = varValue.m_dblVal;
				pApp->m_pFrame->m_pImageView->m_pSet->GetFieldValue(L"pack_gross", varValue);
				dpack_gross = varValue.m_dblVal;
				pApp->m_pFrame->m_pImageView->m_pSet->GetFieldValue(L"pack_tare", varValue);// For HJ this is Fright
				dpack_tare = varValue.m_dblVal;
				pApp->m_pFrame->m_pImageView->m_pSet->GetFieldValue(L"pack_ptare", varValue);
				dpack_ptare = varValue.m_dblVal;
				pApp->m_pFrame->m_pImageView->m_pSet->GetFieldValue(L"wt_unit", str_wt_unit);
				pApp->m_pFrame->m_pImageView->m_pSet->GetFieldValue(L"upd_flag", varValue);
				unsigned char c = varValue.m_chVal;
				// For HJ this is flag for Freight mode - Air / Sea
				if (c == 0)
					str_upd_flag.Format(L"Sea");
				else
					str_upd_flag.Format(L"Air");
				pApp->m_pFrame->m_pImageView->m_pSet->GetFieldValue(L"del_mark", varValue);
				c = varValue.m_chVal;
				if (c == 0)
					bdel_mark = 0;
				else
					bdel_mark = 1;

				// Form string - order is different than in recordset fields (str_wt_unit) but is maintained in title
				CString strLogCSV;
				strLogCSV.Format(L"%s,%s,%s,%s,%s,%s,%s,%3.2f,%3.2f,%3.2f,%3.2f,%2.2f,%2.2f,%2.2f,%2.2f,%s,%d\n",
					str_sta_name, str_data_stamp, str_wk_date, str_tran_no, str_pack_no, str_v_unit, str_wt_unit, dpack_l, dpack_w, dpack_h, dpack_v,
					dpack_net, dpack_gross, dpack_tare, dpack_ptare, str_upd_flag, bdel_mark);
				// Add record to array for later entry in file
				pApp->m_pYield->AddDataTransporter(strLogCSV);
				nNumRecords++;
			}
			// go to next record
			pApp->m_pFrame->m_pImageView->m_pSet->MoveNext();
		}
	}CATCH(CDBException, e)
	{
		// If a database exception occured, show error msg
		AfxMessageBox(L"Database error: " + e->m_strError);
	}
	END_CATCH;
	//pApp->m_pFrame->m_pImageView->m_pSet->Close();
	// Log Data to csv and then xls file
	pApp->m_pYield->LogDataTrans(m_strSelTransporter);
}

BOOL CDlgReport::LogDataToCSVFile()
{
	BOOL bResult = TRUE;
	CStdioFile fileLog;
	CString csvFileName;
	CString TitleLine;//Title line of report
	CTime time = CTime::GetCurrentTime();
	csvFileName = GetReportFileName();
	TitleLine = "Serial No.,Job Name,Date,Time    ,	CountOverall,	PassOverall,	FailOverall,	YieldOverall, CountCam1,PassCam1    ,FailCam1    ,YieldCam1 %    ,CountCam2    ,PassCam2    ,FailCam2    ,YieldCam2 %    ,CountCam3    ,PassCam3     ,FailCam3    ,YieldCam3 %, CountCam4    ,PassCam4     ,FailCam4    ,YieldCam4%, CountCam5    ,PassCam5     ,FailCam5    ,YieldCam5%, CountCam6    ,PassCam6     ,FailCam6    ,YieldCam6%\n\n    ";
	UINT mode1 = CFile::modeWrite | CFile::typeText;
	UINT mode2 = CFile::modeCreate | CFile::modeWrite | CFile::typeText | CFile::shareDenyNone |
		CFile::modeNoTruncate;
	TRY
	{
		if (!fileLog.Open(csvFileName, mode1))//if File already not created throw exception
		AfxThrowFileException(CFileException::fileNotFound);

		fileLog.SeekToEnd();//Set file pointer to the End of the file
	}
	CATCH(CFileException, e)//File does not exist
	{
		if (e->m_cause == CFileException::fileNotFound)
		{
			if (!fileLog.Open(csvFileName, mode2))//New File created
			{
				AfxMessageBox(L"Could Not Create Report File");
				return FALSE;
			}
			fileLog.WriteString(TitleLine);//Write title as the first line
		}
	}
	END_CATCH
		CString LogString;

	//int nNumRecords = GetRecordCount(csvFileName);

	BOOL bNeedToLog = FALSE;
	CHeaderCtrl* pHeader = (CHeaderCtrl*)m_ListControl.GetDlgItem(0);
	int nColumnCount = pHeader->GetItemCount();
	int nNumRows = m_ListControl.GetItemCount() / nColumnCount;
	for (int i = 0; i < nNumRows; i++)
	{
		//nNumRecords,strDeviceID,time.GetDay(), time.GetMonth(), time.GetYear(), time.GetHour(), time.GetMinute(), time.GetSecond(), m_CountCam1, m_PassCam1, m_CountCam1 - m_PassCam1, m_YieldCam1, m_CountCam2, m_PassCam2, m_CountCam2 - m_PassCam2, m_YieldCam2,m_CountCam3, m_PassCam3, m_CountCam3 - m_PassCam3, m_YieldCam3,m_CountCam4, m_PassCam4, m_CountCam4 - m_PassCam4, m_YieldCam4, m_CountCam5 - m_PassCam5, m_YieldCam5,m_CountCam6, m_PassCam6, m_CountCam6 - m_PassCam6, m_YieldCam6
		//LogString.Format(L"%d,%s,%d/%d/%d,%d:%d:%d,%lu ,%lu,%lu,%0.2f,%lu ,%lu,%lu,%0.2f,%lu ,%lu,%lu,%0.2f,%lu, %lu,%lu,%0.2f,%lu, %lu,%lu,%0.2f, %lu,%lu,%lu,%0.2f,%lu, %lu,%lu,%0.2f\n",

	}
	fileLog.WriteString(LogString);//writing csv string into .csv file.
	fileLog.Close();
	SetHiddenAttributeToFile(csvFileName);

	// Convert csv file to xls
	LogToExcelFile(LogString);

	return bResult;

}//LogYieldData

// Gets report file name from user
// 
CString CDlgReport::GetReportFileName()
{
	CString strFileName;
	static char BASED_CODE szFilter[] = "Image Files (*.xls;)| *.xls; ||";
	CFileDialog FileDlg(FALSE,// FALSE for 'SaveAs'
		NULL,
		NULL,
		OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST,
		NULL,//szFilter,
		NULL);
	if (FileDlg.DoModal() != IDCANCEL)
	{
		strFileName = FileDlg.GetPathName();
	}
	return strFileName;
}

// This function uses Dataload exe to convert a csv file to xls file.
// 
UINT CDlgReport::LogToExcelFile(CString strFileName)
{
	CPMSHJApp *pApp = (CPMSHJApp*)AfxGetApp();
	CString strSourceFileName("");
	strSourceFileName.Format(L"\"%s\"", strFileName);

	ResetFileAttributes(strFileName);

	CString strExeName;
	strExeName.Format(L"\"%s\\%s\"", pApp->m_strProjectDir, "DataLoad");

	CString strTemp("");
	strTemp = strFileName;

	strTemp.Replace(L".csv", L".xls");
	CString strTargetFileName;
	strTargetFileName.Format(L"\"%s\"", strTemp);

	CStringA strCmdLine("");
	strCmdLine.Format("%s %s %s", strExeName, strSourceFileName, strTargetFileName);

	UINT nRetVal = WinExec(strCmdLine, SW_HIDE);

	//	#ifdef _DEBUG
	if (nRetVal == 0)
		AfxMessageBox(L"The system is out of memory or resources.");
	else if (nRetVal == ERROR_BAD_FORMAT)
		AfxMessageBox(L"The .EXE file is invalid (non-Win32 .EXE or error in .EXE image).");
	else if (nRetVal == ERROR_FILE_NOT_FOUND)
		AfxMessageBox(L"Dataload file was not found.");
	else if (nRetVal == ERROR_PATH_NOT_FOUND)
		AfxMessageBox(L"Dataload Path Not Found");
	//	#endif

	SetHiddenAttributeToFile(strFileName);
	//	SetFileAttributes(strTargetFileName);//V1.0.11 earler it was strFileName thats why after converting in .xls file it was making .csv as read only;

	return nRetVal;
}

void CDlgReport::ResetFileAttributes(CString strFileName)
{
	CFileStatus fileStatus;
	BOOL bFileFound = CFile::GetStatus(strFileName, fileStatus);
	if (!bFileFound)
		return;

	BYTE newAttribute = 0x00;
	CFile::GetStatus(strFileName, fileStatus);
	fileStatus.m_mtime = 0;
	fileStatus.m_attribute = newAttribute;
	CFile::SetStatus(strFileName, fileStatus);
}

void CDlgReport::SetHiddenAttributeToFile(CString strFileName)
{
	CFileStatus fileStatus;
	BOOL bFileFound = CFile::GetStatus(strFileName, fileStatus);
	if (!bFileFound)
		return;

	BYTE newAttribute = 0x03;
	CFile::GetStatus(strFileName, fileStatus);
	fileStatus.m_mtime = 0;
	fileStatus.m_attribute = newAttribute;
	CFile::SetStatus(strFileName, fileStatus);
}


HBRUSH CDlgReport::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	CPMSHJApp* pApp = (CPMSHJApp*)AfxGetApp();
	pDC->SetBkMode(TRANSPARENT);
	if (pWnd->GetDlgCtrlID() == IDC_EDIT_TRANS_NO)
	{
		return pApp->m_WhiteBrush;
	}
	else if ((pWnd->GetDlgCtrlID() == IDC_LIST_DATABASE) || (pWnd->GetDlgCtrlID() == IDC_STATIC_SEL_PKG))
		return pApp->m_WhiteBrush;
	else
		return pApp->m_DlgBkgndBrush;
}


BOOL CDlgReport::OnInitDialog()
{
	CDialogEx::OnInitDialog(); 
	CPMSHJApp *pApp = (CPMSHJApp*)AfxGetApp();
	CString strCaption, strToolTip;

	std::wstring str = pApp->m_pLanguage->GetString(IDS_STR_SCAN_CODE);
	strCaption = str.c_str();
	str = pApp->m_pLanguage->GetString(IDS_STR_SCAN_FOR_TRNSPORTER);
	strToolTip = str.c_str();
	m_BnScanBarcode.SetFont(L"Aril", 16, 1000, 0, 0);
	m_BnScanBarcode.SetTextColor(RGB(0, 0, 255));
	m_BnScanBarcode.SetToolTipText(strToolTip);
	m_BnScanBarcode.SetShade(SHS_METAL, 10, 30, 20, BTN_BACK_COLOR);
	m_BnScanBarcode.SetFlat(FALSE);
	m_BnScanBarcode.SetWindowText(strCaption);

	str = pApp->m_pLanguage->GetString(IDS_STR_SHOW_RECS);
	strCaption = str.c_str();
	str = pApp->m_pLanguage->GetString(IDS_STR_SHOW_RECS);
	strToolTip = str.c_str();	m_BnShowRecords.SetFont(L"Aril", 16, 1000, 0, 0);
	m_BnShowRecords.SetTextColor(RGB(0, 0, 255));
	m_BnShowRecords.SetToolTipText(strCaption);
	m_BnShowRecords.SetShade(SHS_METAL, 10, 30, 20, BTN_BACK_COLOR);
	m_BnShowRecords.SetFlat(FALSE);
	m_BnShowRecords.SetWindowText(strCaption);

	str = pApp->m_pLanguage->GetString(IDS_STR_EXPORT);
	strCaption = str.c_str();
	str = pApp->m_pLanguage->GetString(IDS_STR_SCAN_FOR_TRNSPORTER);
	strToolTip = str.c_str();	m_BnShowRecords.SetFont(L"Aril", 16, 1000, 0, 0);
	m_BnExport.SetFont(L"Aril", 16, 1000, 0, 0);
	m_BnExport.SetTextColor(RGB(0, 0, 255));
	m_BnExport.SetToolTipText(L"Export Records for a Transporter to XLS File!");
	m_BnExport.SetShade(SHS_METAL, 10, 30, 20, BTN_BACK_COLOR);
	m_BnExport.SetFlat(FALSE);
	m_BnExport.SetWindowText(strCaption);

	str = pApp->m_pLanguage->GetString(IDS_STR_FETCH_RECS);
	strCaption = str.c_str();
	str = pApp->m_pLanguage->GetString(IDS_STR_FETCH_RECS);
	strToolTip = str.c_str();	m_BnShowRecords.SetFont(L"Aril", 16, 1000, 0, 0);
	m_BnFetchRecords.SetFont(L"Aril", 16, 1000, 0, 0);
	m_BnFetchRecords.SetTextColor(RGB(0, 0, 255));
	m_BnFetchRecords.SetToolTipText(strToolTip);
	m_BnFetchRecords.SetShade(SHS_METAL, 10, 30, 20, BTN_BACK_COLOR);
	m_BnFetchRecords.SetFlat(FALSE);
	m_BnFetchRecords.SetWindowText(strCaption);

	str = pApp->m_pLanguage->GetString(IDS_STR_DEL_REC);
	strCaption = str.c_str();
	str = pApp->m_pLanguage->GetString(IDS_STR_DONE);
	strToolTip = str.c_str();	m_BnShowRecords.SetFont(L"Aril", 16, 1000, 0, 0);
	m_BnDeleteRecord.SetFont(L"Aril", 16, 1000, 0, 0);
	m_BnDeleteRecord.SetTextColor(RGB(0, 0, 255));
	m_BnDeleteRecord.SetToolTipText(strCaption);
	m_BnDeleteRecord.SetShade(SHS_METAL, 10, 30, 20, BTN_BACK_COLOR);
	m_BnDeleteRecord.SetFlat(FALSE);
	m_BnDeleteRecord.SetWindowText(strCaption);

	str = pApp->m_pLanguage->GetString(IDS_STR_DONE);
	strCaption = str.c_str();
	str = pApp->m_pLanguage->GetString(IDS_STR_DONE);
	strToolTip = str.c_str();	m_BnShowRecords.SetFont(L"Aril", 16, 1000, 0, 0);
	m_BnCancel.SetFont(L"Aril", 16, 1000, 0, 0);
	m_BnCancel.SetTextColor(RGB(0, 0, 255));
	m_BnCancel.SetToolTipText(strToolTip);
	m_BnCancel.SetShade(SHS_METAL, 10, 30, 20, BTN_BACK_COLOR);
	m_BnCancel.SetFlat(FALSE);
	m_BnCancel.SetWindowText(strCaption);

	m_BnDeleteRecord.EnableWindow(FALSE);

	ListView_SetExtendedListViewStyle(m_ListControl, LVS_EX_GRIDLINES);
	ListView_SetExtendedListViewStyle(m_ListControl, LVS_EX_FULLROWSELECT);
	m_ListControl.SetBkColor(RGB(176, 185, 218));
	m_ListControl.SetTextColor(RGB(0, 0, 222));
	ShowRecords();
	BOOL bResult = m_ListControl.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
	m_nCurSelIndex = 0;
	//m_BnScanBarcode.EnableWindow(FALSE);// Default is manual entry of tran_no

	pApp->m_pFrame->StopKBCapture();
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgReport::OnCancel()
{
	CPMSHJApp* pApp = (CPMSHJApp*)AfxGetApp();
	pApp->m_pFrame->StopKBCapture();
	// Restore 'Report'button icon
	pApp->m_pFrame->m_pImageView->m_BnReport.SetIcon(IDI_REPORT, BS_CENTER);
	pApp->m_pFrame->m_pImageView->Invalidate();
	CDialogEx::OnCancel();
}

void CDlgReport::SetButtonStateForCodeScan()
{
	if (m_bCodeTestingON)
	{
		m_BnShowRecords.EnableWindow(FALSE);
		m_BnExport.EnableWindow(FALSE);
		m_BnFetchRecords.EnableWindow(FALSE);
		m_BnDeleteRecord.EnableWindow(FALSE);
		m_BnCancel.EnableWindow(FALSE);
	}
	else
	{
		m_BnShowRecords.EnableWindow(TRUE);
		m_BnExport.EnableWindow(TRUE);
		m_BnFetchRecords.EnableWindow(TRUE);
		m_BnDeleteRecord.EnableWindow(TRUE);
		m_BnCancel.EnableWindow(TRUE);
	}
}

void CDlgReport::OnBnClickedBnScanBarcode()
{
	CPMSHJApp* pApp = (CPMSHJApp*)AfxGetApp();
	BOOL bAbort = FALSE;
	if (m_bCodeTestingON)
		bAbort = TRUE;
	m_bCodeTestingON = !m_bCodeTestingON;
	SetButtonStateForCodeScan();
	if (m_bCodeTestingON)
	{
		AfxMessageBox(L"Scan a barcode to delete a record or \ndisplay all records for its transporter.");
		m_BnScanBarcode.SetWindowText(L"Scan Done");

		ReadCode();

		m_bCodeTestingON = FALSE;
		SetButtonStateForCodeScan();
	}
	//if (!bAbort)
	//	OnBnClickedBnFetchRecords();

	m_BnScanBarcode.SetWindowText(L"Scan Barcode");
}

// We get the user to scan a package and delete that particular entry from DB
// We don't actually delete a record but just turn ON a delete_flag
// This button gets enabled only when a package has been scanned successfully.
// 
void CDlgReport::OnBnClickedBnDeleteRecord()
{
	//AfxMessageBox(L"TBI");
	CPMSHJApp *pApp = (CPMSHJApp*)AfxGetApp();
	//CString strPK;
	// Get PK for the cur selected item
	//strPK = m_strBarcode.Right(4);
	ShowSelectedRecord(m_strBarcode);
	if (MessageBox(L"Do You Want to Delete This Record?", L"Delete Record?", MB_YESNO) == IDNO)
		return;
	BOOL bExists = pApp->DoesRecordExist(m_strBarcode, FALSE);// FALSE says don't delete reocord if it exists 
	int nCount = 0;
	BOOL bDelMarkSetOK = FALSE;
	TRY
	{
		if (!pApp->m_pFrame->m_pImageView->m_pSet->IsOpen())
			pApp->m_pFrame->m_pImageView->m_pSet->Open();
		pApp->m_pFrame->m_pImageView->m_pSet->MoveFirst();
		while (!pApp->m_pFrame->m_pImageView->m_pSet->IsEOF())
		{
			if (pApp->m_pFrame->m_pImageView->m_pSet->m_pack_no == m_strBarcode)
			{
				// Prepare for edit
				pApp->m_pFrame->m_pImageView->m_pSet->Edit();
				// Set flag
				pApp->m_pFrame->m_pImageView->m_pSet->m_del_mark = 1;
				if (pApp->m_pFrame->m_pImageView->m_pSet->CanUpdate())
				{
					pApp->m_pFrame->m_pImageView->m_pSet->Update();
					bDelMarkSetOK = TRUE;
				}
				break;
			}
			pApp->m_pFrame->m_pImageView->m_pSet->MoveNext();
		}
	}CATCH(CDBException, e) 
	{
		// If a database exception occured, show error msg
		AfxMessageBox(L"Database error: " + e->m_strError);
	}
	END_CATCH;
	//pApp->m_pFrame->m_pImageView->m_pSet->Close();
	if (bDelMarkSetOK == TRUE)
		AfxMessageBox(L"Record Delete Mark Set!");

	//pApp->m_pFrame->m_pImageView->m_pSet->Close();

	ShowRecords();
}


void CDlgReport::OnLvnItemchangedListDatabase(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	m_nCurSelIndex = pNMLV->iItem;
	*pResult = 0;
}

// Use case: User enters a transporter ID in the edit box (or scans a barcode) and clicks this button
void CDlgReport::OnBnClickedBnFetchRecords()
{
	UpdateData(TRUE);
	CPMSHJApp *pApp = (CPMSHJApp*)AfxGetApp();
	CString strCaption, strToolTip;
	std::wstring str = pApp->m_pLanguage->GetString(IDS_STR_TRANS_ID_EMPTY);
	strCaption = str.c_str();
	if (m_strSelTransporter.IsEmpty())
	{
		AfxMessageBox(strCaption);
		return;
	}
	m_ListControl.DeleteAllItems();
	CString str_sta_name, str_data_stamp, str_wk_date, str_tran_no, str_pack_no, str_v_unit, str_pack_l, str_pack_w,
		str_pack_h, str_pack_v, str_pack_net, str_pack_gross, str_pack_tare, str_pack_ptare, str_wt_unit, str_upd_flag, str_del_mark;
	double dpack_l, dpack_w, dpack_h, dpack_v, dpack_net, dpack_gross, dpack_tare, dpack_ptare;
	BOOL bupd_flag = FALSE, bdel_mark = FALSE;
	BOOL bFoundRecord = FALSE;
	TRY
	{
		// Get transporter ID for the cur selected item
		// Fetch Records for the transporter number (string) entered by user
		if (!pApp->m_pFrame->m_pImageView->m_pSet->IsOpen())
			pApp->m_pFrame->m_pImageView->m_pSet->Open();
		if (!pApp->m_pFrame->m_pImageView->m_pSet->IsBOF())
			pApp->m_pFrame->m_pImageView->m_pSet->MoveFirst();
		while (!pApp->m_pFrame->m_pImageView->m_pSet->IsEOF())
		{
			if (pApp->m_pFrame->m_pImageView->m_pSet->m_tran_no == m_strSelTransporter)
			{
				bFoundRecord = TRUE;// Found at least one record!
				// Fetch values of fields and enter into vars
				// Copy each column into a variable
				CDBVariant varValue;
				pApp->m_pFrame->m_pImageView->m_pSet->GetFieldValue(L"sta_name", str_sta_name);
				pApp->m_pFrame->m_pImageView->m_pSet->GetFieldValue(L"data_stamp", str_data_stamp);
				pApp->m_pFrame->m_pImageView->m_pSet->GetFieldValue(L"wk_date", str_wk_date);
				pApp->m_pFrame->m_pImageView->m_pSet->GetFieldValue(L"tran_no", str_tran_no);
				pApp->m_pFrame->m_pImageView->m_pSet->GetFieldValue(L"pack_no", str_pack_no);// PK
				pApp->m_pFrame->m_pImageView->m_pSet->GetFieldValue(L"pack_l", varValue);
				dpack_l = varValue.m_dblVal;
				pApp->m_pFrame->m_pImageView->m_pSet->GetFieldValue(L"pack_w", varValue);
				dpack_w = varValue.m_dblVal;
				pApp->m_pFrame->m_pImageView->m_pSet->GetFieldValue(L"pack_h", varValue);
				dpack_h = varValue.m_dblVal;
				pApp->m_pFrame->m_pImageView->m_pSet->GetFieldValue(L"pack_v", varValue);
				dpack_v = varValue.m_dblVal;
				pApp->m_pFrame->m_pImageView->m_pSet->GetFieldValue(L"v_unit", str_v_unit);
				pApp->m_pFrame->m_pImageView->m_pSet->GetFieldValue(L"pack_net", varValue);
				dpack_net = varValue.m_dblVal;
				pApp->m_pFrame->m_pImageView->m_pSet->GetFieldValue(L"pack_gross", varValue);
				dpack_gross = varValue.m_dblVal;
				pApp->m_pFrame->m_pImageView->m_pSet->GetFieldValue(L"pack_tare", varValue);
				dpack_tare = varValue.m_dblVal;
				pApp->m_pFrame->m_pImageView->m_pSet->GetFieldValue(L"pack_ptare", varValue);
				dpack_ptare = varValue.m_dblVal;
				pApp->m_pFrame->m_pImageView->m_pSet->GetFieldValue(L"wt_unit", str_wt_unit);
				pApp->m_pFrame->m_pImageView->m_pSet->GetFieldValue(L"upd_flag", varValue);
				unsigned char c = varValue.m_chVal;
				if (c == 0)
					bupd_flag = 0;
				else
					bupd_flag = 1;
				pApp->m_pFrame->m_pImageView->m_pSet->GetFieldValue(L"del_mark", varValue);
				c = varValue.m_chVal;
				if (c == 0)
					bdel_mark = 0;
				else
					bdel_mark = 1;
				// Insert values into the list control
				int iRec = 0;
				iRec = m_ListControl.InsertItem(0, str_sta_name, 0);
				m_ListControl.SetItemText(0, 1, str_data_stamp);
				m_ListControl.SetItemText(0, 2, str_wk_date);
				m_ListControl.SetItemText(0, 3, str_tran_no);
				m_ListControl.SetItemText(0, 4, str_pack_no);
				str_pack_l.Format(L"%4.2f", dpack_l);
				m_ListControl.SetItemText(0, 5, str_pack_l);
				str_pack_w.Format(L"%4.2f", dpack_w);
				m_ListControl.SetItemText(0, 6, str_pack_w);
				str_pack_h.Format(L"%4.2f", dpack_h);
				m_ListControl.SetItemText(0, 7, str_pack_h);
				str_pack_v.Format(L"%4.2f", dpack_v);
				m_ListControl.SetItemText(0, 8, str_pack_v);
				m_ListControl.SetItemText(0, 9, str_v_unit);
				str_pack_net.Format(L"%4.2f", dpack_net);
				m_ListControl.SetItemText(0, 10, str_pack_net);
				str_pack_gross.Format(L"%4.2f", dpack_gross);
				m_ListControl.SetItemText(0, 11, str_pack_gross);
				str_pack_tare.Format(L"%4.2f", dpack_tare);
				m_ListControl.SetItemText(0, 12, str_pack_tare);
				str_pack_ptare.Format(L"%4.2f", dpack_ptare);
				m_ListControl.SetItemText(0, 13, str_pack_ptare);
				m_ListControl.SetItemText(0, 14, str_wt_unit);
				str_upd_flag.Format(L"%d", bupd_flag);
				m_ListControl.SetItemText(0, 15, str_upd_flag);
				str_del_mark.Format(L"%d", bdel_mark);
				m_ListControl.SetItemText(0, 16, str_del_mark);
			}
			// goto next record
			pApp->m_pFrame->m_pImageView->m_pSet->MoveNext();
		}
		//pApp->m_pFrame->m_pImageView->m_pSet->Close();
	}CATCH(CDBException, e)
	{
		// If a database exception occured, show error msg
		AfxMessageBox(L"Database error: " + e->m_strError);
		//pApp->m_pFrame->m_pImageView->m_pSet->Close();
	}
	END_CATCH;
	if (!bFoundRecord)
	{
		AfxMessageBox(L"Did not find any records to match the barcode!");
		OnBnClickedBnShowRecords();
	}
} // OnBnClickedBnFetchRecords()


void CDlgReport::OnBnClickedRadioTranNum()
{
	UpdateData(TRUE);
	//m_BnScanBarcode.EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_TRANS_NO)->EnableWindow(TRUE);
}


void CDlgReport::OnBnClickedTranIDScanBarcode()
{
	UpdateData(TRUE);
	m_BnScanBarcode.EnableWindow(TRUE);
	m_strBarcode.Empty();
	m_strSelTransporter.Empty();
	UpdateData(FALSE);
	GetDlgItem(IDC_EDIT_TRANS_NO)->EnableWindow(FALSE);
}


void CDlgReport::ReadCode()
{
	CPMSHJApp* pApp = (CPMSHJApp*)AfxGetApp();
	MSG msg;
	CIHighResTimer	HighResTimer;
	double dTime1 = HighResTimer.GetHighResElapsedTimeMsec();
	m_strBarcode.Empty();
	m_strSelTransporter.Empty();
	pApp->m_pFrame->m_bBarcodeReceived = FALSE;
	pApp->m_pFrame->m_csArray.RemoveAll();
	pApp->m_pFrame->m_strTotal.Empty();
	//pApp->m_pFrame->StartKBCapture();
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
	UpdateData(FALSE);
	while (m_bCodeTestingON)// m_bReadWtThreadUp is set to FALSE if thread has stopped due to whatever reason
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
		if (m_strBarcode.Find('*') == -1)
			continue;
		else
		{
			m_strBarcode = m_strBarcode.Left(m_strBarcode.Find('*'));
			// For some reason "Num" is getting prepended to the string!
			int nIndex = m_strBarcode.Find(L"Num");
			if (nIndex != -1)
			{
				m_strBarcode = m_strBarcode.Right(m_strBarcode.GetLength() - 3);
			}
			GetDlgItem(IDC_EDIT_TRANS_NO)->EnableWindow(TRUE);
			GetDlgItem(IDC_EDIT_TRANS_NO)->SetWindowTextW(m_strBarcode);
			GetDlgItem(IDC_EDIT_TRANS_NO)->EnableWindow(FALSE);
			//UpdateData(FALSE);
			break;
		}

		while (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
	}
	int nSize = m_strBarcode.GetLength();
	if (nSize > 12)
	{
		pApp->m_pFrame->StopKBCapture();
		m_strSelTransporter = m_strBarcode.Left(12);
		pApp->m_pFrame->m_csArray.RemoveAll();
		m_BnDeleteRecord.EnableWindow(TRUE);
		AfxMessageBox(L"Barcode Scanned Successfully! You can now delete reocord \nor view all packages for this transporter.");
	}
	UpdateData(FALSE);
	m_StaticPkgNumber.SetWindowTextW(m_strBarcode);
}

// Shows selected single record
BOOL CDlgReport::ShowSelectedRecord(CString strPK)
{
	BOOL bResult = TRUE;

	CPMSHJApp *pApp = (CPMSHJApp*)AfxGetApp();
	m_ListControl.DeleteAllItems();
	CString str_sta_name, str_data_stamp, str_wk_date, str_tran_no, str_pack_no, str_v_unit, str_pack_l, str_pack_w,
		str_pack_h, str_pack_v, str_pack_net, str_pack_gross, str_pack_tare, str_pack_ptare, str_wt_unit, str_upd_flag, str_del_mark;
	double dpack_l, dpack_w, dpack_h, dpack_v, dpack_net, dpack_gross, dpack_tare, dpack_ptare;
	BOOL bupd_flag = FALSE, bdel_mark = FALSE;

	// Get transporter ID for the cur selected item
	// Fetch Records for the transporter number (string) entered by user
	TRY
	{
		if (!pApp->m_pFrame->m_pImageView->m_pSet->IsOpen())
			pApp->m_pFrame->m_pImageView->m_pSet->Open();
		if (!pApp->m_pFrame->m_pImageView->m_pSet->IsBOF())
			pApp->m_pFrame->m_pImageView->m_pSet->MoveFirst();
		while (!pApp->m_pFrame->m_pImageView->m_pSet->IsEOF())
		{
			if (pApp->m_pFrame->m_pImageView->m_pSet->m_pack_no == strPK)
			{
				// Fetch values of fields and enter into vars
				// Copy each column into a variable
				CDBVariant varValue;
				pApp->m_pFrame->m_pImageView->m_pSet->GetFieldValue(L"sta_name", str_sta_name);
				pApp->m_pFrame->m_pImageView->m_pSet->GetFieldValue(L"data_stamp", str_data_stamp);
				pApp->m_pFrame->m_pImageView->m_pSet->GetFieldValue(L"wk_date", str_wk_date);
				pApp->m_pFrame->m_pImageView->m_pSet->GetFieldValue(L"tran_no", str_tran_no);
				pApp->m_pFrame->m_pImageView->m_pSet->GetFieldValue(L"pack_no", str_pack_no);// PK
				pApp->m_pFrame->m_pImageView->m_pSet->GetFieldValue(L"pack_l", varValue);
				dpack_l = varValue.m_dblVal;
				pApp->m_pFrame->m_pImageView->m_pSet->GetFieldValue(L"pack_w", varValue);
				dpack_w = varValue.m_dblVal;
				pApp->m_pFrame->m_pImageView->m_pSet->GetFieldValue(L"pack_h", varValue);
				dpack_h = varValue.m_dblVal;
				pApp->m_pFrame->m_pImageView->m_pSet->GetFieldValue(L"pack_v", varValue);
				dpack_v = varValue.m_dblVal;
				pApp->m_pFrame->m_pImageView->m_pSet->GetFieldValue(L"v_unit", str_v_unit);
				pApp->m_pFrame->m_pImageView->m_pSet->GetFieldValue(L"pack_net", varValue);
				dpack_net = varValue.m_dblVal;
				pApp->m_pFrame->m_pImageView->m_pSet->GetFieldValue(L"pack_gross", varValue);
				dpack_gross = varValue.m_dblVal;
				pApp->m_pFrame->m_pImageView->m_pSet->GetFieldValue(L"pack_tare", varValue);
				dpack_tare = varValue.m_dblVal;
				pApp->m_pFrame->m_pImageView->m_pSet->GetFieldValue(L"pack_ptare", varValue);
				dpack_ptare = varValue.m_dblVal;
				pApp->m_pFrame->m_pImageView->m_pSet->GetFieldValue(L"wt_unit", str_wt_unit);
				pApp->m_pFrame->m_pImageView->m_pSet->GetFieldValue(L"upd_flag", varValue);
				unsigned char c = varValue.m_chVal;
				if (c == 0)
					bupd_flag = 0;
				else
					bupd_flag = 1;
				pApp->m_pFrame->m_pImageView->m_pSet->GetFieldValue(L"del_mark", varValue);
				c = varValue.m_chVal;
				if (c == 0)
					bdel_mark = 0;
				else
					bdel_mark = 1;
				// Insert values into the list control
				int iRec = 0;
				iRec = m_ListControl.InsertItem(0, str_sta_name, 0);
				m_ListControl.SetItemText(0, 1, str_data_stamp);
				m_ListControl.SetItemText(0, 2, str_wk_date);
				m_ListControl.SetItemText(0, 3, str_tran_no);
				m_ListControl.SetItemText(0, 4, str_pack_no);
				str_pack_l.Format(L"%4.2f", dpack_l);
				m_ListControl.SetItemText(0, 5, str_pack_l);
				str_pack_w.Format(L"%4.2f", dpack_w);
				m_ListControl.SetItemText(0, 6, str_pack_w);
				str_pack_h.Format(L"%4.2f", dpack_h);
				m_ListControl.SetItemText(0, 7, str_pack_h);
				str_pack_v.Format(L"%4.2f", dpack_v);
				m_ListControl.SetItemText(0, 8, str_pack_v);
				m_ListControl.SetItemText(0, 9, str_v_unit);
				str_pack_net.Format(L"%4.2f", dpack_net);
				m_ListControl.SetItemText(0, 10, str_pack_net);
				str_pack_gross.Format(L"%4.2f", dpack_gross);
				m_ListControl.SetItemText(0, 11, str_pack_gross);
				str_pack_tare.Format(L"%4.2f", dpack_tare);
				m_ListControl.SetItemText(0, 12, str_pack_tare);
				str_pack_ptare.Format(L"%4.2f", dpack_ptare);
				m_ListControl.SetItemText(0, 13, str_pack_ptare);
				m_ListControl.SetItemText(0, 14, str_wt_unit);
				str_upd_flag.Format(L"%d", bupd_flag);
				m_ListControl.SetItemText(0, 15, str_upd_flag);
				str_del_mark.Format(L"%d", bdel_mark);
				m_ListControl.SetItemText(0, 16, str_del_mark);

				break;
			}
			// goto next record
			pApp->m_pFrame->m_pImageView->m_pSet->MoveNext();
		}
	}CATCH(CDBException, e)
	{
		// If a database exception occured, show error msg
		AfxMessageBox(L"Database error: " + e->m_strError);
	}
	END_CATCH;

	//pApp->m_pFrame->m_pImageView->m_pSet->Close();

	return bResult;
}