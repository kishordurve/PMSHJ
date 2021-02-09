// ThreadFootSwitch.cpp : implementation file
//

#include "stdafx.h"
#include "PMSHJ.h"
#include "ThreadFootSwitch.h"


// CThreadFootSwitch

IMPLEMENT_DYNCREATE(CThreadFootSwitch, CWinThread)

CThreadFootSwitch::CThreadFootSwitch()
{
	m_nBaudRate = 9600; // Default
}

CThreadFootSwitch::CThreadFootSwitch(std::string strCOMPort, int nBaudRate)
{
	m_strCOMPortFootSwitch = strCOMPort;
	m_nBaudRate = nBaudRate;
	CPMSHJApp* pApp = (CPMSHJApp*)AfxGetApp();
	m_hEndFootSwitchThread = pApp->m_hEndFootSwitchThread;
	m_hCheckFootSwitch = pApp->m_hCheckFootSwitch;
}

CThreadFootSwitch::~CThreadFootSwitch()
{
}

BOOL CThreadFootSwitch::InitInstance()
{
	CPMSHJApp* pApp = (CPMSHJApp*)AfxGetApp();
	m_pSerialComm = new CSerialCommHelper;
	HRESULT hResult = m_pSerialComm->Init(m_strCOMPortFootSwitch, m_nBaudRate, pApp->m_stFootSwitchPortParams.nParity,
		pApp->m_stFootSwitchPortParams.nStopBit, pApp->m_stFootSwitchPortParams.nByteSize);// Parity = 0, Stop Bit = 1, Byte size = 8
	CString strMsg;
	strMsg.Format(L"Foot Switch Arduino Port: COM%d, Baud Rate: %d, Parity: %d, StopBit: %d, ByteSize: %d", pApp->m_stFootSwitchPortParams.nComPortNum, pApp->m_stFootSwitchPortParams.nBaudRate, pApp->m_stFootSwitchPortParams.nParity,
		pApp->m_stFootSwitchPortParams.nStopBit, pApp->m_stFootSwitchPortParams.nByteSize);
	IOutputLogString(strMsg);
	static char result[256];
	int nTimeOut = 0;
	std::string data;
	if (hResult != E_FAIL)
	{
		hResult = m_pSerialComm->Start();
		if (hResult != E_FAIL)
		{
			std::string strData;
			m_HighResTimer.StartHighResTimer();
			while (WaitForSingleObject(m_hEndFootSwitchThread, 5) == WAIT_TIMEOUT)
			{
				while (WaitForSingleObject(m_hCheckFootSwitch, 5) != WAIT_TIMEOUT)
				{
					if (WaitForSingleObject(m_hEndFootSwitchThread, 5) != WAIT_TIMEOUT) // Just in case user ends scan / quits program
					{
						ResetEvent(m_hCheckFootSwitch);
						break;
					}
					try
					{
						if (SendResultOverSerialPort(L"A"))//Enquire if foot pedal pressed bit ON from Arduino
						{
							//IOutputLog(L"Sending string to Arduino Succeeded!");
							HRESULT hResult = m_pSerialComm->ReadAvailable(data);//Get Status
							if (hResult == S_OK)
							{
								CString str(data.c_str());
								MSG msg;
								while (::PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
								{
									if (!pApp->PumpMessage()) //if WM_QUIT i.e User exited the application
									{
										::PostQuitMessage(0);
										return false;// FT_APPLN_QUIT;
									}
								}
								if (str.IsEmpty())
								{
									Sleep(10);
									continue;
								}

								if (str.Find(L"Y") != -1)// The character sent is received back from Arduino
								{
									// Received foot switch trigger - send message to MainFrame. It will decide whether to print label, save to DB etc.
									AfxGetApp()->m_pMainWnd->PostMessage(WM_FOOT_SW_TRG_RCD, 0, 1);
									ResetEvent(m_hCheckFootSwitch);
									m_pSerialComm->ReadAvailable(data);// To clear second string at serial port
								}
							}
							else
							{
								AfxGetApp()->m_pMainWnd->PostMessage(WM_FOOT_SW_READ_FAIL, 0, 1);
								CString strMsg("Read from Serial Comm Port Failed!");
							}
						}
						Sleep(100);
					}
					catch (std::exception e)
					{
						CString strMsg;
						strMsg.Format(L"Exception in Send/Read at COM port");
						AfxMessageBox(strMsg);
					}
				}
			} // while (thread end event)
		}
		else
			IOutputLog(L"Could Not Start COM port for Foot Switch. Foot Switch check will be disabled.");
	}
	else
		IOutputLog(L"Could Not Open COM port for Foot Switch. Foot Switch check will be disabled.");

	if (m_pSerialComm && m_pSerialComm->IsConnection())
		m_pSerialComm->Stop();
	m_pSerialComm->UnInit();// To avoid crash while stopping CSerialPortHelper Thread
	m_pSerialComm->DelLock();
	
	ResetEvent(m_hEndFootSwitchThread);
	IOutputLog(L"End Thread Triggered - Aborting Foot Switch Thread!");
	if (pApp)// App may have died by the time it comes here
	{
		pApp->m_bFootSwitchThreadUp = FALSE;
	}

	AfxEndThread(0);
	return TRUE;
}

int CThreadFootSwitch::ExitInstance()
{
	// TODO:  perform any per-thread cleanup here
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CThreadFootSwitch, CWinThread)
END_MESSAGE_MAP()


// CThreadFootSwitch message handlers
// Function to send output to Arduino
BOOL CThreadFootSwitch::SendResultOverSerialPort(CString strResult)
{
	//IOutputLog(L"Sending string to Arduino.");
	CPMSHJApp* pApp = (CPMSHJApp*)AfxGetApp();
	HRESULT hResult = m_pSerialComm->Write(CW2A(strResult), strResult.GetLength());
	if (hResult == S_OK)
		return TRUE;
	else
		return FALSE;
}

// Checks out if foot switch pressed. Not used
BOOL CThreadFootSwitch::IsTriggerON()
{
	BOOL bRetVal = FALSE;
	CPMSHJApp* pApp = (CPMSHJApp*)AfxGetApp();
	//Wait for the Trigger
	while (!bRetVal)
	{
		try
		{
			SendResultOverSerialPort(L"J");//Enquire if foot pedal pressed bit ON from Arduino
			std::string data;
			HRESULT hResult = m_pSerialComm->ReadAvailable(data);//Get Status
			if (hResult == S_OK)
			{
				CString str(data.c_str());
				MSG msg;
				while (::PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
				{
					if (!pApp->PumpMessage()) //if WM_QUIT i.e User exited the application
					{
						::PostQuitMessage(0);
						return false;// FT_APPLN_QUIT;
					}
				}
				if (str.IsEmpty())
					continue;

				str = str.Right(str[str.GetLength() - 1]);// This is because we get an ack string while using Arduino IOs
				if (str == "1")
				{
					bRetVal = TRUE;
					m_pSerialComm->ReadAvailable(data);// To clear the second string
					break;
				}
			}
			else
			{
				CString strMsg("Read from Serial Comm Port Failed!");
				bRetVal = FALSE;
			}
			Sleep(100);
		}
		catch (std::exception e)
		{
			CString strMsg;
			strMsg.Format(L"Exception in Send/Read at COM port");
			AfxMessageBox(strMsg);
		}
		if (WaitForSingleObject(m_hEndFootSwitchThread, 5) != WAIT_TIMEOUT) // Just in case user ends scan / quits program
		{
			ResetEvent(m_hCheckFootSwitch);
			bRetVal = FALSE;
			break;
		}
	}
	return bRetVal;
}
