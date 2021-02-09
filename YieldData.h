// YieldData.h: interface for the CYieldData class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(QG_LM_YIELDDATA_H__9826A8E1_B466_11D2_9E4F_AC4B71160C4D__INCLUDED_)
#define QG_LM_YIELDDATA_H__9826A8E1_B466_11D2_9E4F_AC4B71160C4D__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

//#include "PromptResetDlg.h"		
#define MANUAL_RESET_SNOOZE_COUNT 1000

class CYieldData  
{
public:
//Constructor / Destructor
	CYieldData();
	~CYieldData();
public:

	CPMSHJApp* m_pApp;
	CFileStatus m_FileStatus;
	CString strYieldFolderName;
	CString strMonthFolderName;
//reset condition data
	UINT	m_ResetDays;	// 1 to 365 days
	long	m_ResetCount;	// 3000 to 1 billion
	UINT	m_ResetOption;	// by days (0), count (1) or Manual (2)
	UINT	m_ResultLogType;//Summary or Detail
	float	m_YieldThreshold;
public:
	long	m_PassOverall;
	long	m_CountOverall;
	long	m_FailOverall;
	float	m_YieldOverall;
	CString	m_strYieldDataFileName;//Used for yield data Excel Sheet
	CString	m_strYieldSessionDataFName;//used for updating trace or session data total pass count etc.
	BOOL	m_bLogAtStop;
	int		m_CurrentMonth;
	CStringArray m_arstrTransporter;// Records for a transporter

public:
	void		ResetYieldCount();
	void		ResetFileAttributes(CString strFileName);
	void		SetHiddenAttributeToFile(CString strFileName);
	BOOL		GenerateExcelFile(CString m_strYieldFileName);
	void		CheckYieldForReset();
	int			m_nManualResetCountSetting;
	void		SaveRegistrySettingsForYield();
	UINT		GetRecordCount(CString strFileName);
	void		UpdateYieldDataOverall(long Total, long Pass, long fail, float Yield);
	CTime		m_StartDay;
	UINT		GetDaysElapsed();
	void		LoadRegistrySettingsForYield();
	void		LogYieldData();
	UINT		LogToExcelFile(CString strFileName);
	void		LogSessisonData();
	void		LogAtStop(BOOL);
	void		Reset();	
	void		SetStartDay();//Start counting the days	
	UINT		GetYieldUpdateCount() const;// Data accessor functions	
	BOOL		IsLogAtStop();//Log at Stop
	CString		GetLogFileName(int CurrentMonth);
	CString		GetDimLogFileName(int CurrentMonth);
	void		LogData(CString m_strLogMeasurements);
	void		LogDataTrans(CString strTranID);
	void		AddDataTransporter(CString m_strLogMeasurements);// Adds data to string array for a particular transporter

};

#endif // !defined(QG_LM_YIELDDATA_H__9826A8E1_B466_11D2_9E4F_AC4B71160C4D__INCLUDED_)
