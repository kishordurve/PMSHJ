// ThreadTCP.cpp : implementation file
//

#include "stdafx.h"
#include "PMSHJ.h"
#include "ThreadTCP.h"


// CThreadTCP

IMPLEMENT_DYNCREATE(CThreadTCP, CWinThread)

CThreadTCP::CThreadTCP()
{
	CPMSHJApp* pApp = (CPMSHJApp*)AfxGetApp();
	m_hEndTCPThread = pApp->m_hEndTCPThread;

	m_strName = "loopback";
	m_iPort = 4000;

	//WORD wVersionRequested = MAKEWORD(2, 2);
	//WSADATA wsaData;
	//int err = WSAStartup(wVersionRequested, &wsaData);
	if (!AfxSocketInit())
	{
		//AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		//return FALSE;
		IOutputLog(L"AfxSocketInit() Failed");
	}
	m_bClientConnected = FALSE;
}

CThreadTCP::~CThreadTCP()
{
	OnClose();
}

BOOL CThreadTCP::InitInstance()
{
	CPMSHJApp* pApp = (CPMSHJApp*)AfxGetApp();
	m_sConnectSocket.SetParent(this);
	m_sListenSocket.SetParent(this);

	int nErrorCode = m_sListenSocket.Create(m_iPort);
	if (!m_sListenSocket.Listen())
	{
		nErrorCode = CAsyncSocket::GetLastError();
		if (nErrorCode == WSAENOTSOCK)
			int nTemp = 0;
	}
	else
	{
		while (WaitForSingleObject(m_hEndTCPThread, 50) == WAIT_TIMEOUT)
		{
			// Do nothing!
			MSG msg;
			while (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				::TranslateMessage(&msg);
				::DispatchMessage(&msg);
			}
		}
	}
	if (m_bClientConnected)
	{
		m_strToSend = L"Server App Exiting!";
		OnSend();
	}
	if (pApp)// App may have died by the time it comes here
	{
		IOutputLog(L"End TCP Thread Triggered - Aborting TCP Thread!");
		pApp->m_bTCPThreadUp = FALSE;
	}

	return TRUE;
}

int CThreadTCP::ExitInstance()
{
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CThreadTCP, CWinThread)
END_MESSAGE_MAP()


// CThreadTCP message handlers
void CThreadTCP::OnAccept()
{
	if (!m_sListenSocket.Accept(m_sConnectSocket))
	{
		// Handle Error;
		IOutputLog(L"Connect request from client socket not accepted!");
	}
	else
	{
		m_bClientConnected = TRUE;
		IOutputLog(L"Connect request from client socket accepted!");
	}
}

void CThreadTCP::OnConnect()
{

}

void CThreadTCP::OnSend()
{
	int iLen = 0, iSent = 0;
	if (!m_strToSend.IsEmpty())
	{
		// convert from UTF-16 (UCS-2) to UTF-8
		CStringA sMessageA = CW2A(m_strToSend, CP_UTF8);
		const size_t nBytes = sizeof(CStringA::XCHAR) * sMessageA.GetLength();
		CByteArray Message;
		Message.SetSize(nBytes);
		memcpy(Message.GetData(), (BYTE*)(LPCSTR)sMessageA, nBytes);
		iSent = m_sConnectSocket.Send(Message.GetData(), Message.GetSize());
		if (iSent == SOCKET_ERROR)
		{
			// Handle error here!
		}
		else
		{
			// All is well!
		}
	}

}

void CThreadTCP::OnReceive()
{
	char* pBuf = new char[1025];
	int iBufSize = 1024;
	int iRcvd;
	CString strRcvd;
	iRcvd = m_sConnectSocket.Receive(pBuf, iBufSize);
	if (iRcvd == SOCKET_ERROR)
	{

	}
	else
	{
		pBuf[iRcvd] = _T('\0');
		m_strRceived = pBuf;
		TRACE(m_strRceived);
		RespondToRequest(m_strRceived);
		//m_strMessage = _T("Received:") + m_strMessage;
		//OnSend();
	}
}

void CThreadTCP::OnClose()
{
	m_sConnectSocket.Close();
}

// This function takes appropriate action on the request received
void CThreadTCP::RespondToRequest(CString strRequest)
{
	CPMSHJApp* pApp = (CPMSHJApp*)AfxGetApp();
	m_strToSend = pApp->RespondToTCPRequest(strRequest);
	OnSend();
}