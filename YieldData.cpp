// YieldData.cpp: implementation of the CYieldData class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PMSHJ.h"
#include "YieldData.h"

#include "MainFrm.h" 


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define DEFAULT_RESET_DAYS		1
#define DEFAULT_RESET_COUNT		1000
#define DEFAULT_RESET_OPTION	1
#define DEFAULT_RESULT_LOG_TYPE 1

// V3.4
//#define DEFAULT_YIELD_THRESHOLD	99.7
#define DEFAULT_YIELD_THRESHOLD	99.4

//v3.1 (E)
#define DEFAULT_CURRENT_MONTH	0

#define RESULT_UPDATE_COUNT	1
#define DEFAULT_LOG_FILE_SUMMARY "L&M_LogSummary.csv"
#define DEFAULT_LOG_FILE_DETAILS "L&M_LogDetails.csv"
#define REG_KEY			L"Yield"
#define DEFAULT_LOG_AT_STOP			FALSE
#include "MainFrm.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CYieldData::CYieldData()
{
	m_CountOverall = 0L;
	m_PassOverall = 0L;
	m_FailOverall = 0L;
	m_YieldOverall = 0.0f;

	m_pApp = (CPMSHJApp*)AfxGetApp();

	CFileStatus fileStatus;
 	SetCurrentDirectory(m_pApp->m_strProjectDir);
	BOOL m_bDirFound = CFile::GetStatus(g_strYieldLogFolderName, fileStatus);
	if (!m_bDirFound)
		CreateDirectory(g_strYieldLogFolderName, NULL);

	LoadRegistrySettingsForYield();
	CString str;
	str.Format(L"%u",DEFAULT_RESET_COUNT);
//	m_ResetCount = atoi(AfxGetApp()->GetProfileString(REG_KEY, "Reset Count", str));
	str.Format(L"%d", DEFAULT_LOG_AT_STOP);
//	m_bLogAtStop = atoi(AfxGetApp()->GetProfileString(REG_KEY, "Log At Stop", str));
	SetStartDay();
	CTime time = CTime::GetCurrentTime();
	m_CurrentMonth = time.GetMonth();
	m_strYieldDataFileName = "";
	
	CString m_strMonth;
	m_strMonth.Format(L"%d",m_CurrentMonth);
	m_strYieldSessionDataFName = m_pApp->m_strProjectDir + L"\\" + g_strYieldLogFolderName + L"\\"+m_strMonth + L"Session Data.csv";
	
	GetLogFileName(m_CurrentMonth);
} // CYieldData()

CYieldData::~CYieldData()
{	
	CPMSHJApp* m_pApp=(CPMSHJApp*)AfxGetApp();
	//m_pApp->m_tStopTime= CTime::GetCurrentTime();
	SaveRegistrySettingsForYield();
	LogSessisonData();
}

void CYieldData::SetStartDay()
{
	m_StartDay = CTime::GetCurrentTime();
}//SetStartDay

UINT CYieldData::GetDaysElapsed()
{
	CTimeSpan days_elapsed = CTime::GetCurrentTime() - m_StartDay;
	
	return (UINT)days_elapsed.GetDays();
}//GetDaysElapsed

void CYieldData::Reset()
{
	CMainFrame *pFrame = (CMainFrame*)AfxGetMainWnd();
	//LogYieldData();// update log file before reseting data
//TBIS

	SetStartDay();
	m_CountOverall = 0L;
	m_PassOverall = 0L;
	m_FailOverall = 0L;
	m_YieldOverall = 0.0f;

}//Reset

// Logs yield data to .csv file
void CYieldData::LogYieldData()
{
	CStdioFile fileLog;
	CString LogFileName;//.csv file
	CString TitleLine;//Title of each bin item
	//m_stDeviceInfo.strDeviceID
	CTime time = CTime::GetCurrentTime();
	LogFileName = GetLogFileName(m_CurrentMonth);
	TitleLine = "Sl No.,Job,Date,Time    ,	Count,	Pass,	Fail,	Yield\n\n    ";
	UINT mode1 = CFile::modeWrite | CFile::typeText;
	UINT mode2 = CFile::modeCreate | CFile::modeWrite | CFile::typeText | CFile::shareDenyNone | 
		CFile::modeNoTruncate   ;
	m_CurrentMonth = time.GetMonth();
	CString curr_month;
	curr_month.Format(L"%d",m_CurrentMonth);	
	m_pApp->WriteProfileString(REG_KEY, L"Current Month", curr_month);
	int nNumRecords = GetRecordCount(LogFileName);
	ResetFileAttributes(LogFileName);
	TRY
	{
		if (!fileLog.Open(LogFileName, mode1))//if File already not created throw exception
			AfxThrowFileException(CFileException::fileNotFound);
		
		fileLog.SeekToEnd();//Set file pointer to the End of the file
	}
	CATCH(CFileException, e)//File does not exist
	{
		if (e->m_cause == CFileException::fileNotFound)
		{
			TRY
			{
				if (fileLog.Open(LogFileName, mode2))//New File created succesfully!
					fileLog.WriteString(TitleLine);//Write title as the first line
				else
					return;
			}
			CATCH(CFileException, e)
			{
				return;
			}
			END_CATCH
		}
	}
	END_CATCH
	CString LogString;

//	int nNumRecords = GetRecordCount(LogFileName);
	m_CountOverall++;
	if(!(m_CountOverall == 0))
	{
						//nNumRecords,	strDeviceID,	time.GetDay(),	   time.GetMonth(), time.GetYear(), time.GetHour(), time.GetMinute(), time.GetSecond(), m_CountOverall, m_PassOverall, m_CountOverall - m_PassOverall, m_YieldOverall
		LogString.Format(L"%d,%d/%d/%d,%d:%d:%d,%d,%d,%lu,%0.2f\n"
						,nNumRecords,					
						time.GetDay(),
						time.GetMonth(),
						time.GetYear(),
						time.GetHour(),
						time.GetMinute(), 
						time.GetSecond(),
						m_CountOverall,
						m_PassOverall,
						m_CountOverall - m_PassOverall,
						m_YieldOverall);
		
	}
	fileLog.WriteString(LogString);//writing csv string into .csv file.
	fileLog.Close();
	SetHiddenAttributeToFile(LogFileName);
	
}//LogYieldData

BOOL CYieldData::IsLogAtStop()
{
	return m_bLogAtStop;
}//IsLogAtStop

void CYieldData::LogAtStop(BOOL bLog)
{
	m_bLogAtStop = bLog;
}//LogAtStop

UINT CYieldData::GetYieldUpdateCount() const
{
	// V3.4 const UINT YieldUpdateCount = 100;
	const UINT YieldUpdateCount = 1000;
	return YieldUpdateCount;
}//GetYieldUpdateCount

// Generates and returns .csv file name
CString CYieldData::GetLogFileName(int CurrentMonth)
{
	CString logfilename;
	CTime time;
	CString strTemp ("");
	strTemp = m_pApp->m_strProjectDir + "\\" + g_strYieldLogFolderName ;

	int month;
	switch(CurrentMonth)
	{
	case 0:
		time  = CTime::GetCurrentTime();
		month = time.GetMonth();
		logfilename = GetLogFileName(month);
		break;
	case 1:
		logfilename = strTemp + L"\\Jan Yield Summary.csv";
		break;
	case 2:
		logfilename = strTemp + L"\\Feb Yield Summary.csv";
		break;
	case 3:
		logfilename = strTemp + L"\\Insp Mar Yield Summary.csv";
		break;
	case 4:
		logfilename = strTemp + L"\\Apr Yield Summary.csv";
		break;
	case 5:
		logfilename = strTemp + L"\\May Yield Summary.csv";
		break;
	case 6:
		logfilename = strTemp + L"\\Jun Yield Summary.csv";
		break;
	case 7:
		logfilename = strTemp + L"\\Jul Yield Summary.csv";
		break;
	case 8:
		logfilename = strTemp + L"\\Aug Yield Summary.csv";
		break;
	case 9:
		logfilename = strTemp + L"\\Sep Yield Summary.csv";
		break;
	case 10:
		logfilename = strTemp + L"\\Oct Yield Summary.csv";
		break;
	case 11:
		logfilename = strTemp + L"\\Nov Yield Summary.csv";
		break;
	case 12:
		logfilename = strTemp + L"\\Dec Yield Summary.csv";
		break;

	default:
		time  = CTime::GetCurrentTime();
		month = time.GetMonth();
		logfilename = GetLogFileName(month);
		break;
	}
	m_strYieldDataFileName = logfilename;
	return logfilename;
}//GetLogFileName

// Generates and returns .csv file name for Dimension Log File
// 
CString CYieldData::GetDimLogFileName(int CurrentMonth)
{
	CString logfilename;
	CTime time;
	CString strTemp("");
	strTemp = m_pApp->m_strProjectDir + "\\" + g_strYieldLogFolderName;

	time = CTime::GetCurrentTime();
	int month, year;
	year = time.GetYear();
	switch (CurrentMonth)
	{
	case 0:
		month = time.GetMonth();
		logfilename = GetLogFileName(month);
		break;
	case 1:
		logfilename.Format(L"%s\\Jan%d.csv", strTemp, year);
		break;
	case 2:
		logfilename.Format(L"%s\\feb%d.csv", strTemp, year);
		break;
	case 3:
		logfilename.Format(L"%s\\Mar%d.csv", strTemp, year);
		break;
	case 4:
		logfilename.Format(L"%s\\Apr%d.csv", strTemp, year);
		break;
	case 5:
		logfilename.Format(L"%s\\May%d.csv", strTemp, year);
		break;
	case 6:
		logfilename.Format(L"%s\\Jun%d.csv", strTemp, year);
		break;
	case 7:
		logfilename.Format(L"%s\\Jul%d.csv", strTemp, year);
		break;
	case 8:
		logfilename.Format(L"%s\\Aug%d.csv", strTemp, year);
		break;
	case 9:
		logfilename.Format(L"%s\\Sep%d.csv", strTemp, year);
		break;
	case 10:
		logfilename.Format(L"%s\\Oct%d.csv", strTemp, year);
		break;
	case 11:
		logfilename.Format(L"%s\\Nov%d.csv", strTemp, year);
		break;
	case 12:
		logfilename.Format(L"%s\\Dec%d.csv", strTemp, year);
		break;

	default:
		time = CTime::GetCurrentTime();
		month = time.GetMonth();
		logfilename = GetLogFileName(month);
		break;
	}
	m_strYieldDataFileName = logfilename;
	return logfilename;
} //GetDimLogFileName

void CYieldData::UpdateYieldDataOverall(long Total, long Pass, long fail, float Yield)
{
//	int camera = cam;
	m_CountOverall = Total;
	m_PassOverall = Pass;
	m_FailOverall = fail;
	m_YieldOverall = Yield;
}

UINT CYieldData::GetRecordCount(CString strFileName)
{
	CString strTemp("");
	int nCount = 0;
	CFileStatus fileStatus;
	BOOL bFileFound = CFile::GetStatus(strFileName, fileStatus);
	if (bFileFound == FALSE)
		return 1;
	CFile *pFile = NULL;
	pFile = new CFile(strFileName, CFile::modeRead | CFile::shareDenyNone);
	if (pFile != NULL)
	{
		CArchive *pArchive = NULL;
		pArchive = new CArchive(pFile, CArchive::load);
		
		if (pArchive != NULL)
		{			
			while(pArchive->ReadString(strTemp))
			{
				strTemp.TrimLeft();
				strTemp.TrimRight();
				if (!strTemp.IsEmpty())
					nCount++;
				//else// KPD tried to avoid exception when file empty!
				//	break;
			}
		}
		delete pArchive;
		delete pFile;
	}
	return nCount;
}//GetRecordCount()

void CYieldData::LoadRegistrySettingsForYield()
{
	CString reset_days, reset_count, reset_option;
	CString str;
	str.Format(L"%u", DEFAULT_RESET_DAYS);
	reset_days		= m_pApp->IGetProfileString(REG_KEY, L"Reset Days", str);
	str.Format(L"%u", DEFAULT_RESET_COUNT);
	reset_count		= m_pApp->IGetProfileString(REG_KEY, L"Reset Count", str);
	str.Format(L"%u", DEFAULT_RESET_OPTION);
	reset_option	= m_pApp->IGetProfileString(REG_KEY, L"Reset Option", str);
	m_ResetDays		= StrToInt(reset_days);
	m_ResetCount	= StrToInt(reset_count);
	m_ResetOption	= StrToInt(reset_option);
	m_nManualResetCountSetting = m_pApp->GetProfileInt(REG_KEY, L"Manual Reset Count", DEFAULT_MANUAL_RESET_COUNT); 	
}//LoadRegistrySettingsForYield()

void CYieldData::SaveRegistrySettingsForYield()
{
	CString reset_days, reset_count, reset_option,curr_month; 
	reset_days.Format(L"%u",m_ResetDays);
	reset_count.Format(L"%lu",m_ResetCount);
	reset_option.Format(L"%u",m_ResetOption);
	curr_month.Format(L"%d",m_CurrentMonth);
	m_pApp->WriteProfileString(REG_KEY, L"Reset Days", reset_days);
	m_pApp->WriteProfileString(REG_KEY, L"Reset Count", reset_count);
	m_pApp->WriteProfileString(REG_KEY, L"Reset Option", reset_option);
	m_pApp->WriteProfileString(REG_KEY, L"Current Month", curr_month);
	m_pApp->WriteProfileInt(REG_KEY, L"Manual Reset Count", m_nManualResetCountSetting);

}//SaveRegistrySettingsForYield()

void CYieldData::CheckYieldForReset()
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	if (m_ResetOption == 0) // Days
	{
		;
	}

	else if (m_ResetOption == 1) // Counter 
	{
		
		;
	}
}//CheckYieldForReset()

BOOL CYieldData::GenerateExcelFile(CString m_strYieldFileName)
{
	int nRetVal = LogToExcelFile(m_strYieldFileName);

	if ((nRetVal == 0) || (nRetVal ==  ERROR_BAD_FORMAT) || (nRetVal ==  ERROR_PATH_NOT_FOUND) || (nRetVal ==  ERROR_FILE_NOT_FOUND))
		return FALSE;
	else
		return TRUE;
}

// This function uses Dataload exe to convert a csv file to xls file.
//
//
UINT CYieldData::LogToExcelFile(CString strFileName)
{
	CString strSourceFileName("");
	strSourceFileName.Format(L"\"%s\"",strFileName);

	ResetFileAttributes(strFileName);

	CString strExeName;
	strExeName.Format(L"\"%s\\%s\"", m_pApp->m_strProjectDir, L"DataLoad");

	CString strTemp("");
	strTemp = strFileName;

	strTemp.Replace(L".csv", L".xls");
	CString strTargetFileName;
	strTargetFileName.Format(L"\"%s\"",  strTemp);

	CString strCmdLine("");
	strCmdLine.Format(L"%s %s %s", strExeName, strSourceFileName, strTargetFileName);

	UINT nRetVal = WinExec(CT2CA(strCmdLine), SW_HIDE);

	#ifdef _DEBUG
	if (nRetVal == 0)
		AfxMessageBox(L"The system is out of memory or resources.");
	else if (nRetVal ==  ERROR_BAD_FORMAT)
		AfxMessageBox(L"The .EXE file is invalid (non-Win32 .EXE or error in .EXE image).");
	else if (nRetVal ==  ERROR_FILE_NOT_FOUND)
		AfxMessageBox(L"Dataload file was not found.");
	else if (nRetVal ==  ERROR_PATH_NOT_FOUND)
		AfxMessageBox(L"Dataload Path Not Found");
	#endif
	//for release versions
	if (nRetVal == 0)
		AfxMessageBox(L"The system is out of memory or resources.");
	else if (nRetVal ==  ERROR_BAD_FORMAT)
		AfxMessageBox(L"The .EXE file is invalid (non-Win32 .EXE or error in .EXE image).");
	else if (nRetVal ==  ERROR_FILE_NOT_FOUND)
		AfxMessageBox(L"Dataload file was not found.");
	else if (nRetVal ==  ERROR_PATH_NOT_FOUND)
		AfxMessageBox(L"Dataload Path Not Found");

	SetHiddenAttributeToFile(strFileName);
//	SetFileAttributes(strTargetFileName);//V1.0.11 earler it was strFileName thats why after converting in .xls file it was making .csv as read only;

	return nRetVal;
}

void CYieldData::ResetFileAttributes(CString strFileName)
{
	CFileStatus fileStatus;
	BOOL bFileFound = CFile::GetStatus(strFileName, fileStatus);
	if (!bFileFound)
		return;

	BYTE newAttribute = 0x00;
	CFile::GetStatus( strFileName, fileStatus );
	fileStatus.m_mtime = 0;
	fileStatus.m_attribute = newAttribute;
	CFile::SetStatus( strFileName, fileStatus );
}

void CYieldData::SetHiddenAttributeToFile(CString strFileName)
{
	CFileStatus fileStatus;
	BOOL bFileFound = CFile::GetStatus(strFileName, fileStatus);
	if (!bFileFound)
		return;

	BYTE newAttribute = 0x03;
	CFile::GetStatus( strFileName, fileStatus );
	fileStatus.m_mtime = 0;
	fileStatus.m_attribute = newAttribute;
	CFile::SetStatus( strFileName, fileStatus );
}

void CYieldData::ResetYieldCount()
{
	m_CountOverall		= 0L;
	m_PassOverall		= 0L;
	m_FailOverall		= 0L;
	m_YieldOverall		= 0.0f;
}

void CYieldData::LogSessisonData()
{
} // LogSessisonData()

void CYieldData::AddDataTransporter(CString m_strLogMeasurements)
{
	m_arstrTransporter.Add(m_strLogMeasurements);
}

// Logs data to csv file for a given transporter ID
// Deletes old file if found
void CYieldData::LogDataTrans(CString strTransporterID)
{
	CStdioFile fileLog;
	CString LogFileName;//.csv file
	CString TitleLine;//Title of each inspected item - contains min / max values
	//LogFileName = strTransporterID;
	// Get File Name from user
	static char BASED_CODE szFilter[] = "XLS Files (*.xls)| *.xls; ||";
	CString strFilter(szFilter);
	CString strFileName;
	CFileDialog FileDlg(FALSE,
		NULL,
		NULL,
		OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST,
		strFilter,
		NULL);

	if (FileDlg.DoModal() == IDOK)
	{
		strFileName = FileDlg.GetPathName();
		if (strFileName.Find(L".xls") != -1)
		{
			strFileName = strFileName.Left(strFileName.GetLength() - 4);
		}
		strFileName += ".csv";
		LogFileName = strFileName;
	}
	else
		return;
	//

	//CString strTemp = m_pApp->m_strProjectDir + "\\" + g_strYieldLogFolderName + "\\" + LogFileName + ".csv";
	//LogFileName = strTemp;
	TitleLine = "sta_name, data_stamp, wk_date, tran_no, pack_no, v_unit, wt_unit, pack_l, pack_w, pack_h, pack_v, pack_net, pack_gross, pack_tare, Freight (USD), Air / Sea, del_mark\n\n";// pack_ptare, upd_flag
	UINT mode1 = CFile::modeWrite | CFile::typeText;
	UINT mode2 = CFile::modeCreate | CFile::modeWrite | CFile::typeText | CFile::shareDenyNone | CFile::modeNoTruncate;
	ResetFileAttributes(LogFileName);
	CString strMsg;
	CFileFind fileFind;
	if (fileFind.FindFile(LogFileName))
	{
		if (m_pApp->m_pFrame->MessageBox(L"Do You Want to Overwrite Existing File?", L"Overwrite File?", MB_YESNO) == IDNO)
			return;
	}
	TRY
	{
		if (fileLog.Open(LogFileName, mode1))// if the file is already existing, delete it!
		{
			fileLog.Close();
			CFile::Remove(LogFileName);
		}
		if (fileLog.Open(LogFileName, mode2))//New File created succesfully!
			fileLog.WriteString(TitleLine);//Write title as the first line
		else
		{
			CString strMsg;
			strMsg.Format(L"Could not create csv file - %s! Make sure you have write permission.", LogFileName);
			AfxMessageBox(strMsg);
		}

		fileLog.SeekToBegin();//Set file pointer to the End of the file
		fileLog.WriteString(TitleLine);
	}
	CATCH(CFileException, e)//File does not exist
	{
		if (e->m_cause == CFileException::badPath)
		{
			strMsg.Format(L"Bad File Path %s of csv file! Make sure you have write permission.", LogFileName);
			AfxMessageBox(strMsg);
			return;
		}
		else if (e->m_cause == CFileException::diskFull)
		{
			strMsg.Format(L"Disk Full!");
			AfxMessageBox(strMsg);
			return;
		}
		else if (e->m_cause == CFileException::tooManyOpenFiles)
		{
			strMsg.Format(L"Too Many Open Files!");
			AfxMessageBox(strMsg);
			return;
		}
		else if (e->m_cause == CFileException::genericException)
		{
			strMsg.Format(L"Generic Exception!");
			AfxMessageBox(strMsg);
			return;
		}
		else if (e->m_cause == CFileException::sharingViolation)
		{
			strMsg.Format(L"Could Not Delete Old File. Please Delete Manually!");
			AfxMessageBox(strMsg);
			return;
		}
	}
	END_CATCH
	for (int i = 0; i < m_arstrTransporter.GetSize(); i++)
	{
		fileLog.WriteString(m_arstrTransporter[i]);//writing csv string into .csv file.
	}
	fileLog.Close();
	SetHiddenAttributeToFile(LogFileName);
	if (LogToExcelFile(LogFileName) > 31)
	{
		CString strMsg = LogFileName;
		strMsg.Replace(L"csv", L"xls");
		strMsg.Format(L"File saved: %s!", strMsg);
		AfxMessageBox(strMsg);
		m_arstrTransporter.RemoveAll();
	}
}

// Logs individual part data to csv file
// One file is created for each month
void CYieldData::LogData(CString m_strLogMeasurements)
{
	CStdioFile fileLog;
	CString LogFileName;//.csv file
	CString TitleLine;//Title of each inspected item - contains min / max values
	CTime time = CTime::GetCurrentTime();
	LogFileName = GetDimLogFileName(m_CurrentMonth);
	TitleLine = "sta_name, data_stamp, wk_date, tran_no, pack_no, v_unit, wt_unit, pack_l, pack_w, pack_h, pack_v, pack_net, pack_gross, pack_tare, pack_ptare, upd_flag, del_mark\n\n";
	UINT mode1 = CFile::modeWrite | CFile::typeText;
	UINT mode2 = CFile::modeCreate | CFile::modeWrite | CFile::typeText | CFile::shareDenyNone |
		CFile::modeNoTruncate;
	m_CurrentMonth = time.GetMonth();
	CString curr_month;
	curr_month.Format(L"%d", m_CurrentMonth);
	m_pApp->WriteProfileString(REG_KEY, L"Current Month", curr_month);

	ResetFileAttributes(LogFileName);
	TRY
	{
		if (!fileLog.Open(LogFileName, mode1))//if File already not created throw exception
			AfxThrowFileException(CFileException::fileNotFound);

	fileLog.SeekToEnd();//Set file pointer to the End of the file
	}
	CATCH(CFileException, e)//File does not exist
	{
		if (e->m_cause == CFileException::fileNotFound)
		{
			TRY
			{
				if (fileLog.Open(LogFileName, mode2))//New File created succesfully!
					fileLog.WriteString(TitleLine);//Write title as the first line
				else
					return;
			}
				CATCH(CFileException, e)
			{
				return;
			}
			END_CATCH
		}
	}
	END_CATCH

	fileLog.WriteString(m_strLogMeasurements);//writing csv string into .csv file.
	fileLog.Close();
	SetHiddenAttributeToFile(LogFileName);

} // LogData()
