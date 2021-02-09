#pragma once



// CThreadFootSwitch

class CThreadFootSwitch : public CWinThread
{
	DECLARE_DYNCREATE(CThreadFootSwitch)

protected:
	CThreadFootSwitch();           // protected constructor used by dynamic creation

public:
	CThreadFootSwitch(std::string strCOMPort, int nBaudRate);
	virtual ~CThreadFootSwitch();
	virtual BOOL InitInstance();
	virtual int ExitInstance();

	BOOL SendResultOverSerialPort(CString strResult);
	BOOL IsTriggerON();

	int m_nBaudRate;
	CSerialCommHelper*	m_pSerialComm;
	HANDLE				m_hEndFootSwitchThread;
	HANDLE				m_hCheckFootSwitch;
	std::string			m_strCOMPortFootSwitch;
	CIHighResTimer		m_HighResTimer;

protected:
	DECLARE_MESSAGE_MAP()
};


