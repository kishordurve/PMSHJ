#pragma once



// CThreadPrintLabel

class CThreadPrintLabel : public CWinThread
{
	DECLARE_DYNCREATE(CThreadPrintLabel)

public:
	CThreadPrintLabel();           // protected constructor used by dynamic creation
	CThreadPrintLabel(STPrintParams stPrintParams, HANDLE hEndPrintThread, HANDLE hPrintLabel, HANDLE hPrintTestLabel);
	virtual ~CThreadPrintLabel();

public:
	STPrintParams m_stPrintParams;
	virtual BOOL InitInstance();
	virtual int ExitInstance();

	CPMSHJApp* m_pApp;
	HANDLE	m_hEndPrintLabelThread;
	HANDLE	m_hPrintLabel;// Initiates label pinting
	HANDLE	m_hPrintTestLabel;// Initiates label pinting

	BOOL PrintLabelCLExpress(STInspResult InspResult);
	BOOL PrintTestLabel();
	BOOL IsStringChinese(CString str);
	BOOL m_bPrinterPortOpened;
	BOOL m_bLogoUploadedToPrinter;
	STInspResult m_InspResult;
protected:
	DECLARE_MESSAGE_MAP()
};


