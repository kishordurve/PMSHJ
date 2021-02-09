// MySocket.cpp : implementation file
//

#include "stdafx.h"
#include "PMSHJ.h"
#include "MySocket.h"
#include "ThreadTCP.h"


// CMySocket

CMySocket::CMySocket()
{
}

CMySocket::~CMySocket()
{
}


// CMySocket member functions
void CMySocket::OnAccept(int nErrorCode)
{
	if (nErrorCode == 0)
		((CThreadTCP*)m_pWnd)->OnAccept();
}
void CMySocket::OnConnect(int nErrorCode)
{
	if (nErrorCode == 0)
		((CThreadTCP*)m_pWnd)->OnConnect();
}
void CMySocket::OnClose(int nErrorCode)
{
	if (nErrorCode == 0)
		((CThreadTCP*)m_pWnd)->OnClose();
}
void CMySocket::OnReceive(int nErrorCode)
{
	if (nErrorCode == 0)
		((CThreadTCP*)m_pWnd)->OnReceive();
}
