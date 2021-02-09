// ThreadScanCode.cpp : implementation file
//

#include "stdafx.h"
#include "PMSHJ.h"
#include "ThreadScanCode.h"


// CThreadScanCode

IMPLEMENT_DYNCREATE(CThreadScanCode, CWinThread)

CThreadScanCode::CThreadScanCode()
{
}

CThreadScanCode::CThreadScanCode(std::string strCOMPort, int nBarcodeLengthMin, int nBarcodeLengthMax, char cEndChar)
{
	m_strCOMPortScanner = strCOMPort;
	m_nBarcodeLengthMin = nBarcodeLengthMin;
	m_nBarcodeLengthMax = nBarcodeLengthMax;
	m_cEndChar = cEndChar;
	CPMSHJApp* pApp = (CPMSHJApp*)AfxGetApp();
	m_hEndScannerThread = pApp->m_hEndScannerThread;
}

CThreadScanCode::~CThreadScanCode()
{
}

BOOL CThreadScanCode::InitInstance()
{
	CPMSHJApp* pApp = (CPMSHJApp*)AfxGetApp();
	m_pSerialComm = new CSerialCommHelper;
	HRESULT hResult = m_pSerialComm->Init(m_strCOMPortScanner, pApp->m_stScannerPortParams.nBaudRate, pApp->m_stScannerPortParams.nParity,
		pApp->m_stWeighScaleOPParams.nStopBit, pApp->m_stWeighScaleOPParams.nByteSize);// Parity = 0, Stop Bit = 1, Byte size = 8
	CString strMsg;
	strMsg.Format(L"Scnner Port: COM%d, Baud Rate: %d, Parity: %d, StopBit: %d, ByteSize: %d", pApp->m_stWeighScaleOPParams.nComPortNum, pApp->m_stWeighScaleOPParams.nBaudRate, pApp->m_stWeighScaleOPParams.nParity,
		pApp->m_stWeighScaleOPParams.nStopBit, pApp->m_stWeighScaleOPParams.nByteSize);
	IOutputLogString(strMsg);
	static char result[256];
	int nTimeOut = 0;
	if (hResult != E_FAIL)
	{
		hResult = m_pSerialComm->Start();
		if (hResult != E_FAIL)
		{
			std::string strData;
			while (WaitForSingleObject(m_hEndScannerThread, 5) == WAIT_TIMEOUT)
			{
				if (WaitForSingleObject(m_pSerialComm->GetWaitForEvent(), 5) != WAIT_TIMEOUT)
				{
					// Gather all chars available at the port
					hResult = m_pSerialComm->ReadAvailable(strData);
					if (SUCCEEDED(hResult))
					{
						int nIndex = strData.find(m_cEndChar);
						if (nIndex >= m_nBarcodeLengthMin)// if m_cEndChar is not found nIndex will be -1 (value of std::string::npos)
						{
							std::string subStr = strData.substr(0, nIndex);
							if (subStr.length() > m_nBarcodeLengthMax)
								subStr = subStr.substr(subStr.length() - m_nBarcodeLengthMax);
							EnterCriticalSection(&(pApp->m_csScannerString));
							pApp->m_strScannerString = CString(subStr.c_str());
							LeaveCriticalSection(&(pApp->m_csScannerString));
							AfxGetApp()->m_pMainWnd->PostMessage(WM_UPDATE_CODE, (WPARAM)((LPCTSTR)strData.c_str()), (LPARAM)(0));
						}
					} // Succeded in scanning barcode
				}// Data available at port
			}// End thread event not yet set
		}
		else
			IOutputLog(L"Could Not Start COM port for scanning code. Scanning of barcode will be disabled.");
	}
	else
		IOutputLog(L"Could Not Open COM port for scanning code. Scanning of barcode will be disabled.");

	if (m_pSerialComm && m_pSerialComm->IsConnection())
		m_pSerialComm->Stop();
	m_pSerialComm->UnInit();// To avoid crash while stopping CSerialPortHelper Thread
	m_pSerialComm->DelLock();
	ResetEvent(m_hEndScannerThread);
	IOutputLog(L"End Serial Port Scanner Thread Triggered - Aborting Scanning!");
	if (pApp)// App may have died by the time it comes here
	{
		pApp->m_bScannerThreadUp = FALSE;
	}

	AfxEndThread(0);
	return TRUE;
}

int CThreadScanCode::ExitInstance()
{
	// TODO:  perform any per-thread cleanup here
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CThreadScanCode, CWinThread)
END_MESSAGE_MAP()


// CThreadScanCode message handlers
