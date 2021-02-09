#pragma once



#include <string>

#include "SerialCommHelper.h"

// CThreadScanCode

class CThreadScanCode : public CWinThread
{
	DECLARE_DYNCREATE(CThreadScanCode)

public:
	CThreadScanCode();           // protected constructor used by dynamic creation
	CThreadScanCode(std::string strCOMPort, int nBarcodeLengthMin, int nBarcodeLengthMax, char cEndChar);
	virtual ~CThreadScanCode();

	CSerialCommHelper*	m_pSerialComm;
	HANDLE				m_hEndScannerThread;
	std::string			m_strCOMPortScanner;
	int					m_nBarcodeLengthMin;
	int					m_nBarcodeLengthMax;
	char				m_cEndChar;
	CIHighResTimer		m_HighResTimer;

	virtual BOOL InitInstance();
	virtual int ExitInstance();

protected:
	DECLARE_MESSAGE_MAP()
};


