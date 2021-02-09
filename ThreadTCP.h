#pragma once

#include "MySocket.h"


// CThreadTCP

class CThreadTCP : public CWinThread
{
	DECLARE_DYNCREATE(CThreadTCP)

protected:

public:
	CThreadTCP();           // protected constructor used by dynamic creation
	virtual ~CThreadTCP();

	virtual BOOL InitInstance();
	virtual int ExitInstance();

	CMySocket m_sListenSocket;
	CMySocket m_sConnectSocket;

	HANDLE m_hEndTCPThread;

	void OnAccept();
	void OnConnect();
	void OnSend();
	void OnReceive();
	void OnClose();
	void RespondToRequest(CString strRequest);

	CString m_strRceived;
	CString m_strToSend;
	CString m_strName;
	int m_iPort;
	BOOL m_bClientConnected;
protected:
	DECLARE_MESSAGE_MAP()
};


