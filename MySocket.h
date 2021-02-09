#pragma once

// CMySocket command target
#include "ThreadTCP.h"

class CMySocket : public CAsyncSocket
{
public:
	CMySocket();
	virtual ~CMySocket();
	CWinThread* m_pWnd;
	void SetParent(CWinThread* pWnd) { m_pWnd = pWnd; };
	void OnAccept(int nErrorCode);
	void OnConnect(int nErrorCode);
	void OnClose(int nErrorCode);
	void OnReceive(int nErrorCode);
};


