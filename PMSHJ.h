
// PMSHJ.h : main header file for the PMSHJ application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols
#include "MainFrm.h"
#include "Structures.h"
#include "AutoSaveImage.h"
#include "SerialCommHelper.h"
#include "ThreadReadWeight.h"
#include "YieldData.h"
#include "Language.h"
#include "ThreadScanCode.h"
#include "ThreadTCP.h"
#include "ThreadFootSwitch.h"
#include "ThreadHikVCode.h"
#include "ThreadPrintLabel.h"
#include "ThreadSendDataToServer.h"

// CPMSHJApp:
// See PMSHJ.cpp for the implementation of this class
//

class CPMSHJApp : public CWinAppEx
{
public:
	CPMSHJApp();


// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	// Required to hide the window
	virtual BOOL LoadWindowPlacement(CRect& rectNormalPosition, int& nFflags, int& nShowCmd);

	BOOL	StartWeighScaleReadThread();
	BOOL	StopWeighScaleReadThread();

	BOOL	StartCodeScannerThread();
	BOOL	StopCodeScannerThread();

	BOOL	StartTCPThread();
	BOOL	StopTCPThread();

	BOOL	StartFootSwitchThread();
	BOOL	StopFootSwitchThread();

	BOOL	StartPrintLabelThread();
	BOOL	StopPrintLabelThread();

	BOOL	StartSendDataThread();
	BOOL	StopSendDataThread();

	int		IGetProfileInt(LPCTSTR lpszSection, LPCTSTR lpszEntry, DWORD dwVal);
	CString	IGetProfileString(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCTSTR strValue);

	BOOL	WriteInspParamsToRegistry();
	BOOL	GetInspParamsFromRegistry();
	
	BOOL	WriteCurMachineNameToRegistry();
	BOOL	GetCurMachineNameFromRegistry();

	BOOL	WriteOptionsToRegistry();
	BOOL	GetOptionsFromRegistry();

	BOOL	WriteFootSwitchBoxSlNumToRegistry();
	BOOL	GetFootSwitchBoxSlNumFromRegistry();

	BOOL	WriteCOMPortParamsToRegistry();
	BOOL	GetCOMPortParamsFromRegistry();

	BOOL	WriteWeighScaleParamsToRegistry();
	BOOL	GetWeighScaleParamsFromRegistry();

	BOOL	WriteUDFFileNameToRegistry();
	BOOL	GetUDFFileNameFromRegistry();

	BOOL	WritePrintParamsToRegistry();
	BOOL	GetPrintParamsFromRegistry();

	BOOL	WriteFreightParamsToRegistry();
	BOOL	GetFreightParamsFromRegistry();

	BOOL	WriteSendDataParamsToRegistry();
	BOOL	GetSendDataParamsFromRegistry();

	BOOL	GetLicenseStatus();// The default password for this copy!
	BOOL	SetLicenseStatus(BOOL bOK);// The default password for this copy!

	void	WriteLog(CString logstr);
	CString	RespondToTCPRequest(CString strRequest);

	BOOL	CreateImageDir();
	CString GetImagesDirectory();

	// ORBBEC Camera
	BOOL InitCameraOrbbec();
	openni::Device m_device;
	openni::VideoStream m_depth, m_color;
	CString	GetCameraSlNumber();

	// Implementation
	UINT  m_nAppLook;
	BOOL  m_bHiColorIcons;
	BOOL  m_bSameScanID;
	BOOL  m_bStartingNewMultiItemScan;
	BOOL  m_bCam3DAvailable;
	CString m_strProjectDir;

	CMainFrame	*m_pFrame;
	CAutoSaveImage*		m_pAutoSaveImage;
	CBrush	m_BkgndBrush;//used for changing the background color of view
	CBrush	m_DlgBkgndBrush;//used for changing the background color of dialogs
	CBrush	m_WhiteBrush;//used for changing the background color of edit controls
	CBrush	m_DBSetBkgndBrush;//used for changing the background color of edit controls
	CBrush	m_StaticTextBkgndBrush;// Used for static text (L, W, H, V, W) bkgnd
	STComPortParams		m_stPrinterPortParams;
	STComPortParams		m_stWeighScaleOPParams;
	STComPortParams		m_stWeighScaleIPParams;
	STComPortParams		m_stScannerPortParams;
	STComPortParams		m_stFootSwitchPortParams;
	STWeighScaleParams	m_stWeighScaleParams;
	STInspParams		m_stInspParams;
	STPrintParams		m_stPrintParams;
	STOptions			m_stOptions;
	STInspResult		m_stInspResult;// Scan Result for single item
	STInspResult		m_stInspResultSaved;// Scan Result saved to DB. Will be overwritten by next saved scan result.
	STScanData			m_stScanData;// Scan Data contains InspResult data for all items for current scan
	STUDFFileText		m_stUDFFileText;// Holds contents of udf file
	STUDFFileText		m_stUDFFileTextPrint;// Holds modified udf file - ready for printing
	STDBRecordFileds	m_stDBRecordFields;// Holds scan data in Database fields format (17 nos.)
	STFreightParams		m_stFreightParams;
	STServerCommParams	m_stServerCommParams;
	STBarcodeParts		m_stBarcodeParts;// For HJ using only the code rectangle corner points
	
	CYieldData*			m_pYield;
	CLanguage*			m_pLanguage;
	CArray<STInspResult, STInspResult> m_arInspResults;

	// Serial Comm
	CSerialCommHelper*	m_pSerialComm;
	CSerialCommHelper*	m_pSerialCommArduino;
	//CSerialCommHelper*	m_pSerialCommPrinter;

	HANDLE				m_hReadWeight;// Handle to Event
	HANDLE				m_hPrintLabel;// Initiates label pinting
	HANDLE				m_hPrintTestLabel;// Initiates Test Label pinting
	HANDLE				m_hSendDataToServer;// Initiates data transmission to server

	HANDLE				m_hEndWtReadThread;
	HANDLE				m_hEndScannerThread;
	HANDLE				m_hEndTCPThread;
	HANDLE				m_hCheckFootSwitch;
	HANDLE				m_hEndFootSwitchThread;
	HANDLE				m_hEndPrintLabelThread;
	HANDLE				m_hEndSendDataThread;

	CThreadReadWeight*	m_pThreadReadWeight;
	CThreadScanCode*	m_pThreadCodeScanner;
	CThreadHikVCode*	m_pThreadHikVCode;
	CThreadTCP*			m_pThreadTCP;
	CThreadFootSwitch*	m_pThreadFootSwitch;
	CThreadPrintLabel*	m_pThreadPrintLabel;
	CThreadSendDataToServer*	m_pThreadSendData;

	BOOL				m_bReadWtThreadUp;
	BOOL				m_bPrinterPortOpened;
	BOOL				m_bScannerThreadUp;
	BOOL				m_bTCPThreadUp;
	BOOL				m_bFootSwitchThreadUp;
	BOOL				m_bPrintLabelThreadUp;
	BOOL				m_bSendDataThreadUp;
	//
	int					m_nFootSwitchTrigBoxSlNum;
	CRITICAL_SECTION m_csWeightString;
	CString m_strWeightString;// Composite string of all 4 weights (Excell), composite string of 3 - GRWT, TAREWT, UNIT (GRAM)
	CString m_strWtUnit;
	// P5 specific
	BOOL m_bIsGross;
	// Scanner variables
	CRITICAL_SECTION m_csScannerString;
	CRITICAL_SECTION m_csFootSwitchTrigger;
	CString m_strScannerString;
	char	m_cEndChar;
	//

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	CString	GetProjectDirectory();
	BOOL	SaveScanData();// Saves results to ini file and also pictures
	BOOL	SaveScanDataToDatabase(STScanData ScanData);// Saves results to Database
	BOOL	DoesRecordExist(CString strPrKey, BOOL bDelete = FALSE);
	BOOL	WriteScanDataToIniFile(CString strFilePath, STScanData ScanData);
	BOOL	GetScanDataFromIniFile(CString strFilePath, STScanData& ScanData);
	void	GetDataToSave(STScanData &ScanData);
	BOOL	DeleteLastScanData();// Deletes most recent scan data
	BOOL	SplitBarcode(CString strBarcode, STBarcodeParts&);
	CString GetSysDateTimeMS(BOOL bAddPuncMarks = FALSE);
	BOOL	VerifyDongle();
	BOOL	ReadUDFFile(STUDFFileText& stUDFFileText);
	void	CenterWindow();
	void	RaiseWeightReadError();
	void	CreateDefaultRegistry();

	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CPMSHJApp theApp;
