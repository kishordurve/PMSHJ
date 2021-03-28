// ThreadReadWeight.cpp : implementation file
//

#include "stdafx.h"
#include "ThreadReadWeight.h"
#include "PMSHJ.h"


// CThreadReadWeight
USE_DEBUGDUMP;
IMPLEMENT_DYNCREATE(CThreadReadWeight, CWinThread)

CThreadReadWeight::CThreadReadWeight()
{
	m_dWeight = m_dWeightOld = 0.0f;
	//m_eWtUnit = Kg;
}

CThreadReadWeight::CThreadReadWeight(std::string strCOMPort, eWeighScaleType WeighScaleType, eWeighScaleMode WeighScaleMode)
{
	m_dWeight = m_dWeightOld = 0.0f;
	m_strCOMPort = strCOMPort;
	m_WeighScaleType = WeighScaleType;
	m_eWeighScaleMode = WeighScaleMode;
	CPMSHJApp* pApp = (CPMSHJApp*)AfxGetApp();
	m_hReadWeight = pApp->m_hReadWeight;
	m_hEndWtReadThread = pApp->m_hEndWtReadThread;
	//m_eWtUnit = Kg;
	m_strWtUnitMarker = "";
	nTimesWtUnitSet = 0;
	//IOutputLog(L"In ReadWeight Thread");
}

CThreadReadWeight::~CThreadReadWeight()
{

}

 
BOOL CThreadReadWeight::InitInstance()
{
	CPMSHJApp* pApp = (CPMSHJApp*)AfxGetApp();
	m_pSerialComm = new CSerialCommHelper;
	//HRESULT hResult = m_pSerialComm->Init(m_strCOMPort, 9600, 0, 1, 8);// Parity = 0, Stop Bit = 1, Byte size = 8
	HRESULT hResult = m_pSerialComm->Init(m_strCOMPort, pApp->m_stWeighScaleOPParams.nBaudRate, pApp->m_stWeighScaleOPParams.nParity, 
		pApp->m_stWeighScaleOPParams.nStopBit, pApp->m_stWeighScaleOPParams.nByteSize);// Parity = 0, Stop Bit = 1, Byte size = 8
	CString strMsg;
	strMsg.Format(L"Port: COM%d, Baud Rate: %d, Parity: %d, StopBit: %d, ByteSize: %d", pApp->m_stWeighScaleOPParams.nComPortNum, pApp->m_stWeighScaleOPParams.nBaudRate, pApp->m_stWeighScaleOPParams.nParity,
		pApp->m_stWeighScaleOPParams.nStopBit, pApp->m_stWeighScaleOPParams.nByteSize);
	IOutputLogString(strMsg);
	static char result[256];
	int nTimeOut = 0;	
	if (hResult != E_FAIL)
	{
		hResult = m_pSerialComm->Start();
		//IOutputLog(L"Weighing thread After m_pSerialComm->Start()");
		if (hResult != E_FAIL)
		{
			//IOutputLog(L"Weighing thread did not Fail in m_pSerialComm->Start()");
			pApp->m_pFrame->m_pImageView->m_bCOMPortStarted = TRUE;
			// Flag to set Weight accuracy
			BOOL bSetAccuracyLevel = TRUE;
			m_HighResTimer.StartHighResTimer();
			int nSendStable = 0;// We will send stable readings 5 times and not just once as other wise
			int nSendUnStable = 0;// We will send stable readings 5 times and not just once as other wise
			int nErrorCount = 0;
			// sometimes updating stable wt is a problem
			while (WaitForSingleObject(m_hEndWtReadThread, 5) == WAIT_TIMEOUT)
			{
				if (WaitForSingleObject(m_pSerialComm->GetWaitForEvent(), 5) != WAIT_TIMEOUT)
				{
					BOOL bIsUnstable = FALSE, bIsOverWt = FALSE, bIsGrossWt = TRUE, bIsNetWt = FALSE, bIsTimeout = FALSE;
					BOOL bIsError = FALSE;
					if (m_WeighScaleType == EXCELL)
					{
						bIsError = ReadWeightExcell(m_strWeight, m_strWtUnit, bIsUnstable);
						//IOutputLogString(m_strWeight);
					}
					else if (m_WeighScaleType == STANDARD)
					{
						bIsError = ReadWeightStandard(m_strWeight, m_strWtUnit, bIsUnstable);
						//IOutputLogString(m_strWeight);
					}
					else if (m_WeighScaleType == CH340)
					{
						bIsError = !ReadWeightCH340(m_strWeight, m_strWtUnit, bIsUnstable, bIsGrossWt, bIsTimeout);
						if (bIsError && bIsTimeout)
						{
							nErrorCount++;
							if (nErrorCount >= 3)
							{
								AfxGetApp()->m_pMainWnd->PostMessage(WM_WEIGHT_ERROR, 0, 0);
								IOutputLog(L"Error reading weight!");
							}
							break;
						}
						else
							nErrorCount = 0;
						//IOutputLogString(m_strWeight);
					}
					else if (m_WeighScaleType == P5)// KPD - 240320
					{
						bIsError = !ReadWeightP5(m_strWeight, m_strWtUnit, bIsUnstable, bIsOverWt, bIsGrossWt, bIsNetWt);
						//IOutputLogString(m_strWeight);
					}
					else if (m_WeighScaleType == GRAM)// KPD - 120520
					{
						bIsError = !ReadWeightGRAM(m_strWeight, m_strWtUnit, bIsUnstable, bIsOverWt);
						strMsg = "Weight String comprises GR WT+TARE WT+TARE WT UNIT: ";
						strMsg += m_strWeight;
						if (bIsUnstable || bIsOverWt)
							nSendStable = 0;
						//IOutputLogString(strMsg);
					}
					if (m_strWeight.IsEmpty())
						continue;
					EnterCriticalSection(&(pApp->m_csWeightString));
					pApp->m_strWeightString = m_strWeight;
					pApp->m_strWtUnit = m_strWtUnit;
					pApp->m_bIsGross = bIsGrossWt;// P5 specific - also added for CH340 (HJ)
					LeaveCriticalSection(&(pApp->m_csWeightString));
					//strMsg = "App Weight String: ";
					//strMsg += pApp->m_strWeightString;
					//IOutputLogString(strMsg);
					m_dWeight = _wtof(m_strWeight);
					if (bIsError || bIsOverWt)// KPD - 240320
					{
						AfxGetApp()->m_pMainWnd->PostMessage(WM_UPDATE_WEIGHT, 0, 1);
						nSendStable = 0;
					}
					else if (bIsUnstable)
					{
						nSendUnStable++;
						if (nSendUnStable > 3)
							AfxGetApp()->m_pMainWnd->PostMessage(WM_UPDATE_WEIGHT, 0, 2);
						nSendStable = 0;
					}
					else if ((nSendStable < 5) || (m_WeighScaleType == GRAM))
					{
						if (m_strWeight.IsEmpty())
							continue;
						strcpy(result, CT2A(m_strWeight.GetBuffer()));
						AfxGetApp()->m_pMainWnd->PostMessage(WM_UPDATE_WEIGHT, (WPARAM)((LPCTSTR)result), (LPARAM)(0));
						nSendStable++;
						nSendUnStable = 0;
						if (bSetAccuracyLevel && (m_WeighScaleType != GRAM))// Need to do it only once - when starting to measure weight
						{
							bSetAccuracyLevel = FALSE;
							// Get accuracy level
							int nIndex1 = m_strWeight.Find('.');
							if (nIndex1 != -1)
							{
								int nIndex2 = nIndex1 + 1;
								while (isdigit(m_strWeight[nIndex2]))
									nIndex2++;
								pApp->m_stOptions.nWtAccuracy = nIndex2 - nIndex1 - 1;
							}
							else
								pApp->m_stOptions.nWtAccuracy = 0;
						}// if (necessary to set Accuracy Level)
					}// else if (weight stable and message being sent to MainFrame)
					nTimeOut = 0;
				}// if (Serial Port timeout)
				else
				{
					nTimeOut++;
					if (nTimeOut > 500)
					{
						IOutputLog(L"Weigh Scale timed out!");
						AfxGetApp()->m_pMainWnd->PostMessage(WM_WEIGHT_ERROR, 0, 0);
						break;
					}
				}
			} // while (thread end event)
		}// if (Init())
		else
			IOutputLog(L"Could Not Start COM port for reading weight. Reading of weight will be disabled.");
	}
	else
		IOutputLog(L"Could Not Open COM port for reading weight. Reading of weight will be disabled.");

	if (m_pSerialComm && m_pSerialComm->IsConnection())
		m_pSerialComm->Stop();
	m_pSerialComm->UnInit();// To avoid crash while stopping CSerialPortHelper Thread
	m_pSerialComm->DelLock();
	ResetEvent(m_hEndWtReadThread);
	IOutputLog(L"End Thread Triggered - Aborting ReadWeight Thread!");
	if (pApp)// App may have died by the time it comes here
	{
		pApp->m_bReadWtThreadUp = FALSE;
		pApp->m_pMainWnd->PostMessage(WM_UPDATE_WEIGHT, (WPARAM)(3), (LPARAM)(3));
	}

	AfxEndThread(0);
	return TRUE;
}

int CThreadReadWeight::ExitInstance()
{
	// TODO:  perform any per-thread cleanup here
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CThreadReadWeight, CWinThread)
END_MESSAGE_MAP()


// Reads weight string from Excell scale and removes spaces. String of at least 40 chars is read and procesed.
// Inserts markers G,N,T and P for Gross, Net, Tare and PreTare weights.
// From the incoming string composes string of type -GXXX.XXNXXX.XXTXXX.XXPXXX.XX. 
// The - sign is included only if it exists in the incoming string
// This string is later copied to the string in App.
// 
BOOL CThreadReadWeight::ReadWeightExcell(CString& strWeight, CString& strWtUnit, BOOL& bIsUnstable)
{
	double t1 = m_HighResTimer.GetHighResElapsedTimeMsec();
	std::string data, strData, strData1;
	HRESULT hResult;
	long lCount = 0;
	do
	{
		hResult = m_pSerialComm->Read_N(strData1, 1, 50);
		strData += strData1;
	} while (strData1.length() > 0);	
	
	//if (m_strWtUnitMarker.length() == 0)// First time
	if (nTimesWtUnitSet < 3)// Doing it thrice - Just to be safe
	{
		nTimesWtUnitSet++;
		if (strData.length() > 40)
		{
			CString strtemp(strData.c_str());
			//IOutputLogString(strtemp);
			if (strData.find('k') != std::string::npos)
			{
				int nIndex = strData.find('k');
				CString str(strData.c_str());
				nIndex = str.Find('k');
				if (str.GetLength() > (nIndex + 1))
				{
					strWtUnit = str.Right(str.GetLength() - nIndex);
					str = strWtUnit.Left(2);
					strWtUnit = str;// "kg";
				}
				m_strWtUnitMarker = "k";
				//IOutputLogString(strtemp);
				//IOutputLogString(str);
			}
			else if (strData.find('K') != std::string::npos)
			{
				int nIndex = strData.find('K');
				CString str(strData.c_str());
				if (str.GetLength() > (nIndex + 1))
				{
					strWtUnit = str.Right(str.GetLength() - nIndex);
					str = strWtUnit.Left(2);
					strWtUnit = str;// "Kg";// str;
				}
				m_strWtUnitMarker = "K";
			}
			else if (strData.find('g') != std::string::npos)
			{
				m_strWtUnitMarker = "g";
				strWtUnit = "g";
				IOutputLog(L"Found g");
			}
			else if (strData.find('G') != std::string::npos)
			{
				m_strWtUnitMarker = "G";
				strWtUnit = "G";
				IOutputLog(L"Found G");
			}
			else if (strData.find('L') != std::string::npos)
			{
				m_strWtUnitMarker = "L";
				strWtUnit = "L";
			}
			else if (strData.find('l') != std::string::npos)
			{
				int nIndex = strData.find('l');
				CString str(strData.c_str());
				if (str.GetLength() > (nIndex + 1))
				{
					strWtUnit = str.Right(str.GetLength() - nIndex);
					str = strWtUnit.Left(2);
					strWtUnit = str;// "lb";// str;
				}
				m_strWtUnitMarker = "l";
			}
			else if (strData.find('T') != std::string::npos)
			{
				m_strWtUnitMarker = "T";
				strWtUnit = "T";
			}
			else if (strData.find('t') != std::string::npos)
			{
				m_strWtUnitMarker = "t";
				strWtUnit = "t";
			}
			// Finding wt unit assuming fixed format
			CString strDataRead(strData.c_str());
			int nIndex = strDataRead.Find('+');
			if (nIndex == -1)
			{
				nIndex = strDataRead.Find('-');
				if (nIndex == -1)
				{
					//IOutputLog(L"Found Neither + nor -");
					strWeight.Empty();
					bIsUnstable = TRUE;
					return FALSE;
				}
			}
			CString strSub = strDataRead.Right(strDataRead.GetLength() - nIndex - 1);
			if (strSub.GetLength() < 36)// If string is too small
			{
				strWeight.Empty();
				bIsUnstable = TRUE;
				return FALSE;
			}
			strWtUnit = strSub.Left(36);
			strWtUnit = strWtUnit.Right(4);
			strWtUnit.Remove(' ');// Remove spaces
		}
	}
	
	int nLength = strData.length();
	if (nLength < 38)
	{
		do//while (m_pSerialComm->IsInputAvailable())
		{
			hResult = m_pSerialComm->Read_N(strData1, 1, 50);
			strData += strData1;
		} while (strData1.length() > 0);
	}
	nLength = strData.length();
	if (nLength >= 40)// Applicable only for EXCELL
	{
		long alCount = NULL;
		
		CStringA strDataRead(strData.c_str()), strKg;
		// Check for the character indicating unstable reading
		if (strDataRead.GetLength() > 34)// If data stable
		{
			bIsUnstable = FALSE;
			BOOL bNegativeSign = FALSE;
			int nIndex = strDataRead.Find('+');
			if (nIndex == -1)
			{
				nIndex = strDataRead.Find('-');
				if (nIndex != -1)
					bNegativeSign = TRUE;
			}
			if (nIndex == -1)// If neither + not - is present return;
			{
				//IOutputLog(L"Found Neither + nor -");
				strWeight.Empty();
				bIsUnstable = TRUE;
				return FALSE;
			}

			CStringA strTemp = strDataRead.Right(strDataRead.GetLength() - nIndex - 1);
			/*int nWtUnitIndex = strTemp.Find(m_strWtUnitMarker.c_str());
			if (nWtUnitIndex == -1)// invalid - could not find Wt unit marker string
			{
				strWeight = "0.0";
				return FALSE;
			}
			CStringA strTrimmed;
			int nLength = strTemp.GetLength();
			if (nWtUnitIndex != -1)
				strTrimmed = strTemp.Left(nWtUnitIndex);
			// At this point we have all chars from +/- to just before 'k/g/l/t'
			//TRACE(_T("Read Data: %s\n\n"), strTrimmed.GetBuffer());
			// Need to break up into 4 strings by replacing space (tab) with N/G/T/P (Net/Gross/Tare/PreTare). For ex - N34.56G35.84T1.5P1.
			int nNumTab = 0;
			for (int i = 0; i < strTrimmed.GetLength(); i++)
			{
				if (isspace(strTrimmed[i]))
				{
					if (nNumTab == 0)
					{
						while (isspace(strTrimmed[i + 1]))
							strTrimmed.Delete(i, 1);
						strTrimmed.SetAt(i, 'G');
					}
					else if (nNumTab == 1)
					{
						while (isspace(strTrimmed[i + 1]))
							strTrimmed.Delete(i, 1);
						strTrimmed.SetAt(i, 'N');
					}
					else if (nNumTab == 2)
					{
						while (isspace(strTrimmed[i + 1]))
							strTrimmed.Delete(i, 1);
						strTrimmed.SetAt(i, 'T');
					}
					else if (nNumTab == 3)
					{
						while (isspace(strTrimmed[i + 1]))
							strTrimmed.Delete(i, 1);
						strTrimmed.SetAt(i, 'P');
					}
					nNumTab++;
				}
			}
			//TRACE(_T("Modified Data: %s\n\n"), strTrimmed.GetBuffer());
			CStringA strNeg = "-";
			if (bNegativeSign)
				strTrimmed = strNeg + strTrimmed;
			strWeight = strTrimmed;*/
			CStringA strGross = strTemp.Left(8);
			strGross = strGross;
			strTemp = strDataRead.Right(strDataRead.GetLength() - nIndex - 1 - 8);
			CStringA strNet = strTemp.Left(8);
			strTemp = strDataRead.Right(strDataRead.GetLength() - nIndex - 1 - 16);
			CStringA strTare = strTemp.Left(8);
			strTemp = strDataRead.Right(strDataRead.GetLength() - nIndex - 1 - 24);
			CStringA strPTare = strTemp.Left(8);
			CStringA strNeg = "-";
			if (bNegativeSign)
				strWeight = strNeg + strGross + strNet + strTare + strPTare;
			else
				strWeight = strGross + strNet + strTare + strPTare;
		}
		else
			bIsUnstable = TRUE;
	}
	return FALSE;// No Error
} // ReadWeightExcell()

// For the standard (FCNA) scale the format is:
// Total:+      0pcs                 
// G.W. :+0.990kg
// U.W. : +0.00000 g / pcs
// 
BOOL CThreadReadWeight::ReadWeightStandard(CString& strWeight, CString& strWtUnit, BOOL& bIsUnstable)
{
	double t1 = m_HighResTimer.GetHighResElapsedTimeMsec(), t2;
	std::string data, strData, strData1;
	HRESULT hResult;
	long lCount = 0;
	do
	{
		hResult = m_pSerialComm->Read_N(strData1, 1, 50);
		strData += strData1;
		t2 = m_HighResTimer.GetHighResElapsedTimeMsec();
		if ((t2 - t1) > 500)// The fuse - ensures escape if no input
		{
			IOutputLog(L"Time out waiting for scale output");
			return FALSE;
		}
	} while (strData.length() < 60);

	int nLength = strData.length();

	if (nLength > 10)
	{
		long alCount = NULL;
		CString strKg(strData.c_str());
		int nIndex = strKg.ReverseFind('k');
		if (nIndex == -1)
			nIndex = strKg.ReverseFind('k');
		if (nIndex > 7)
			strKg = strKg.Left(nIndex);
		else
			return FALSE;
		strKg = strKg.Right(6);
		strKg.Remove(' ');

		double t2 = m_HighResTimer.GetHighResElapsedTimeMsec();
		strWeight = strKg;
		//TRACE("\nWt: %3.2f, As Rcd: %s\n", dWt, strKg);
	}

	return TRUE;
} // ReadWeightStandard()

// For the CH340 Scale the format is:
// ST, NT, +0.59kg CR LF
// where -
//	ST: Stable, US: Unstable, OL: Overload / underload
//	TR: Tare Mode, NT: Net Wt Mode, GS: Gross Mode
//	+/- sign, Data (has decimal point), Unit, CR LF
// 
BOOL CThreadReadWeight::ReadWeightCH340(CString& strWeight, CString& strWtUnit, BOOL& bIsUnstable, BOOL& bIsGrossWt, BOOL& bIsTimeOut)
{
	BOOL bResult = TRUE;
	double t1 = m_HighResTimer.GetHighResElapsedTimeMsec(), t2;
	std::string data, strData, strData1;
	HRESULT hResult;
	long lCount = 0;
	do
	{
		hResult = m_pSerialComm->Read_N(strData1, 1, 50);
		strData += strData1;
		t2 = m_HighResTimer.GetHighResElapsedTimeMsec();
		if ((t2 - t1) > 500)// The fuse - ensures escape if no input
		{
			IOutputLog(L"Time out waiting for scale output");
			return FALSE;
		}
	} while (strData.length() < 40);

	CString strKg(strData.c_str());
	if (strKg.GetLength() < 36)
		return FALSE;
	if (strKg.Find(L"NT") != -1)
		bIsGrossWt = FALSE;
	else
		bIsGrossWt = TRUE;
	int nIndex = 0;
	if (strKg.Find(L"ST") != -1)
	{
		nIndex = strKg.Find(L"ST");
		strKg = strKg.Right(strKg.GetLength() - nIndex - 1);
		bIsUnstable = FALSE;
	}
	else if (strKg.Find(L"US") != -1)
	{
		nIndex = strKg.Find(L"US");
		strKg = strKg.Right(strKg.GetLength() - nIndex - 1);
		bIsUnstable = TRUE;
	}
	else if (strKg.Find(L"OL") != -1)
	{
		nIndex = strKg.Find(L"OL");
		bIsUnstable = TRUE;
		strKg = strKg.Right(strKg.GetLength() - nIndex - 1);
	}
	else
	{
		IOutputLog(L"Could not find ST, US or OL");
		return FALSE;
	}

	BOOL bNegativeSign = FALSE;
	CStringA strNeg = "-";
	nIndex = strKg.Find(L"-");
	if (nIndex != -1)
		bNegativeSign = TRUE;
	else
	{
		nIndex = strKg.Find(L"+");
		if (nIndex == -1)// Neither + nor - sign found. Something wrong
		{
			bIsUnstable = TRUE;
			IOutputLog(L"Weight Unstable");
			return FALSE;
		}
	}
	// First comma
	nIndex = strKg.Find(L",");
	CString strTrimmed = strKg.Right(strKg.GetLength() - nIndex - 1);
	// Second comma
	nIndex = strTrimmed.Find(L",");
	strTrimmed = strTrimmed.Right(strTrimmed.GetLength() - nIndex - 1);
	if (bNegativeSign)
		nIndex = strTrimmed.Find(L"-");
	else
		nIndex = strTrimmed.Find(L"+");
	strTrimmed = strTrimmed.Right(strTrimmed.GetLength() - nIndex - 1);// No need to check for +/- as it has already been done earlier
	// At this point we should have a string xxx.xxxkgCRLF or xxxx.xxxkgCRLF
	int nStrLength = strTrimmed.GetLength();
	if (nStrLength >= 9)
	{
		CString strWtCur;
		if (isalpha(strTrimmed[7]))
		{
			if ((strTrimmed[7] == 'k') || (strTrimmed[7] == 'K'))
				m_strWtUnit = "kg";
			strWtCur = strTrimmed.Left(7);
		}
		else if (isalpha(strTrimmed[7]))
		{
			if ((strTrimmed[8] == 'k') || (strTrimmed[8] == 'K'))
				m_strWtUnit = "kg";
			strWtCur = strTrimmed.Left(8);
		}
		if (strWtCur != m_strWeight)// The new weigh scale is not sending 'US' for unstable weight!
			bIsUnstable = TRUE;
		strWeight = strWtCur;
	}
	else
		return FALSE;
	return bResult;
} // ReadWeightCH340()


// For the P5 scale the format is:
// Status (UN/ST/OV)	,	Gross/Net	,	Sign	Data Unit CRLF    
//	2					1	  2			1	 1		 7	  2		2	Total 18 bytes
// 
BOOL CThreadReadWeight::ReadWeightP5(CString& strWeight, CString& strWtUnit, BOOL& bIsUnstable, BOOL& bIsOverWt, BOOL& bIsGrossWt, BOOL& bIsNetWt)
{
	double t1 = m_HighResTimer.GetHighResElapsedTimeMsec(), t2;
	std::string data, strData, strData1;
	HRESULT hResult;
	long lCount = 0;
	do
	{
		hResult = m_pSerialComm->Read_N(strData1, 1, 50);
		strData += strData1;
		t2 = m_HighResTimer.GetHighResElapsedTimeMsec();
		if ((t2 - t1) > 1500)// The fuse - ensures escape if no input
		{
			CString str;
			str.Format(L"Time out - Took %d msec to collect 50 chars", (t2 - t1));
			IOutputLogString(str);
			return FALSE;
		}
	} while (strData.length() < 36);// We collect 36 bytes so as to ensure that we get at least one full valid string of 18 bytes

	int nLength = strData.length();
	CString strTotal, strStatus, strRemainder, str;
	bool bIsNegative = false;
	if (nLength > 35)// To check if it has come here after escaping the (t2 - t1) < 500 condition
	{
		strTotal = CString(strData.c_str());
		int nIndex = strTotal.Find(_T("ST"));// First 'ST'
		strRemainder = strTotal.Right(strTotal.GetLength() - nIndex - 3);// Removes first comma and chars up to it
		if (nIndex == -1)// Weight not stable. May be it is overweight?
		{
			nIndex = strTotal.Find(_T("OV"));// First 'ST'
			if (nIndex == -1)
				bIsUnstable = TRUE;
			else
				bIsOverWt = TRUE;
			IOutputLog(L"Weight Unstable");
			return FALSE;
		}
		// Gross / Net Weight (2 bytes)
		nIndex = strRemainder.Find(',');
		strStatus = strRemainder.Left(nIndex);
		str = strStatus.Right(2);
		if (str == "GS")
			bIsGrossWt = TRUE;
		else if (str == "NT")
			bIsNetWt = TRUE;
		// Sign (1 byte)
		strRemainder = strRemainder.Right(strRemainder.GetLength() - nIndex - 1);
		str = strRemainder.Left(1);
		if (str == "-")
			bIsNegative = true;
		strRemainder = strRemainder.Right(strRemainder.GetLength() - 1);
		// Data (7 bytes)
		if (strRemainder.GetLength() > 7)
			strWeight = strRemainder.Left(7);
		else
			return FALSE;
		// Wt Unit
		strRemainder = strRemainder.Right(strRemainder.GetLength() - 7);
		if (strRemainder.GetLength() >= 2)
			strWtUnit = strRemainder.Left(2);
		else
		{
			IOutputLog(L"Could not find weight units");
			return FALSE;
		}
	}

	return TRUE;
} // ReadWeightP5()

// For the GRAM scale is it not necessary to send the read request if the scale is in continuous streaming mode
// Output from the scale in streaming mode is in the following format (40 bytes after STX):
// 0	1-2			3-4			5		6		7		8		9		10		11		12		13		14
// STX	ID_O		ID_I		'r'		'O'		'1'		'0'		'7'		'1'		'6'		'W'		W1		W2
//
// 15	16		17		18		19		20		21		22		23		24		25		26		27		28	
// W3	W4		W5		W6		W7		W8		U1		U2		'T'		T1		T2		T3		T4		T5
//
// 29	30		31		32		33		34		35		36		37		38-39		40	
// T6	T7		T8		U1		U2		'S'		S1		S2		S3		LRC			ETX
// 
// Assumption: Scale is in coninuous steaming mode
// This func forms a composite string: wwwwwwwwttttttttUU (both wwwwwwww and tttttttt can contain '-' sign and decimal)
// 
BOOL CThreadReadWeight::ReadWeightGRAM(CString& strWeight, CString& strWtUnit, BOOL& bIsUnstable, BOOL& bIsOverWt)
{
	double t1 = m_HighResTimer.GetHighResElapsedTimeMsec(), t2;
	std::string data, strData, strData1;
	CString strTareWeight, strTareWtUnit;
	HRESULT hResult;
	long lCount = 0;
	int nAttempts = 0;
	char c = 0;
	// Wait till STX is received
	do
	{
		hResult = m_pSerialComm->Read_N(strData, 1, 200);
		if (SUCCEEDED(hResult))
			c = strData[0];
		t2 = m_HighResTimer.GetHighResElapsedTimeMsec();
		if ((t2 - t1) > 1000)// The fuse - ensures escape if no input
		{
			IOutputLog(L"Time out waiting for 1st char from scale");
			return FALSE;
		}
	} while (c != 2);
	// STX is received - now grab the next 40 chars
	while (nAttempts < 5)
	{
		// SendReadWtRqstGRAM(); No need if in coninuous streaming mode!
		hResult = m_pSerialComm->Read_N(strData, 40, 200);
		nAttempts++;
		if (SUCCEEDED(hResult))
		{
			// Verify LRC to ensure integrity of data
			// STX has already been removed. Now remove LRC (char indices 37 and 38) and ETX (char index 40)
			std::string stringToCheckLRC = strData.substr(0, 37);// start pos 0, length 37
			std::string strLRC = strData.substr(37, 2);// start pos 37, length 2
			if (VerifyLRCGRAMScale(stringToCheckLRC, strLRC))// Currently returning TRUE by default
				break;
		}
	}
	if (nAttempts >= 5)
		return FALSE;
	CString strMsg;
	strMsg.Format(_T("GW and TR String Rcd: %s %s"), CString(strData.substr(12, 10).c_str()), CString(strData.substr(23, 10).c_str()));
	//IOutputLogString(strMsg);
	int nLength = strData.length();
	CString strTotal, strStatus, strRemainder, str;
	bool bIsNegative = false;
	if (nLength >= 40)
	{
		strMsg.Format(_T("Total String Rcd: %s"), CString(strData.substr(0, 39).c_str()));
		IOutputLogString(strMsg);
		strTotal = CString(strData.c_str());
		// Get Gross Weight
		int nIndex = strTotal.Find(_T("W"));// Weight marker
		if (nIndex < 0)
			return FALSE;
		strRemainder = strTotal.Right(strTotal.GetLength() - nIndex - 1);// Removes up to and including W
		if (strRemainder.GetLength() >= 25)
			strWeight = strRemainder.Left(8);
		else // Looks like partial string is received
			return FALSE;
		strWeight = CString(strData.substr(12, 8).c_str());
		//strRemainder = strRemainder.Right(strRemainder.GetLength() - 8);
		// Get Weight Unit
		//strWtUnit = strRemainder.Left(2);
		strWtUnit = CString(strData.substr(20, 2).c_str());
		strMsg.Format(_T("Gross Wt string: %s, Unit: %s"), strWeight, strWtUnit);
		//IOutputLogString(strMsg);
		// Get Tare Weight
		nIndex = strTotal.Find(_T("T"));// Tare marker
		if (nIndex < 0)
			return FALSE;
		strTareWeight = CString(strData.substr(23, 8).c_str());
		// Get Tare Weight Unit
		strTareWtUnit = CString(strData.substr(31, 2).c_str());
		strMsg.Format(_T("Tare Wt string: %s, Unit: %s"), strTareWeight, strTareWtUnit);
		//IOutputLogString(strMsg);
		// Get if Weight stable
		nIndex = strTotal.Find(_T("S"));// Status marker
		if (nIndex < 0)
			return FALSE;
		// Get Status - it is a hex value encoded as 3 ASCII chars
		std::string strStatusASCII = strData.substr(34, 3);
		int nStatusVal = HexStringToDecimal(strStatusASCII);
		strMsg.Format(_T("Status string value: %d"), nStatusVal);
		IOutputLogString(strMsg);
		if (!(nStatusVal & 4)) // Checking if 3rd bit is NOT ON
		{
			bIsUnstable = TRUE;
			IOutputLog(L"Wt Unstable");
		}
		else
			bIsUnstable = FALSE;
		// Get if overweight
		if (nStatusVal & 128) // Checking if 8th bit is ON
		{
			bIsOverWt = TRUE;
			IOutputLog(L"Over Wt");
		}
		else
			bIsOverWt = FALSE;
		// Get if Tare mode ON
		if (!(nStatusVal & 16)) // Checking if 5th bit is NOT ON
		{
			strTareWeight = "00000000";
			//IOutputLog(L"Tare Wt OFF");
		}

		// Form composite string
		strWeight += strTareWeight + strTareWtUnit;
	}
	else
	{
		IOutputLog(L"String Length less than 40");
		return FALSE;
	}

	return TRUE;
} // ReadWeightGRAM()


// Sends Read request to GRAM scale (Page 30 of Xtreme manual)
// The string to be sent is (15 bytes):
// 0	1-2			3-4			5		6		7		8		9		10		11		12-13		14
// STX	ID_O		ID_I		'R'		'O'		'1'		'0'		'7'		'0'		'0'		LRC			ETX
// Assumption ID_O is 02 and ID_I is 01
BOOL CThreadReadWeight::SendReadWtRqstGRAM()
{
	BOOL bResult = TRUE;
	HRESULT hResult;
	// String without STX (1 byte), LRC (2 bytes) and ETX (1 byte);
	std::string str("\u000200FFR01070000\u0003\r\n");
	unsigned char* inputToLRC = (unsigned char*)str.c_str();
	// Get LRC
	unsigned char cLRC = LRC(inputToLRC);
	// Convert LRC to Hex (2 bytes)
	char buf[2 + 1];
	sprintf(buf, "%02X", cLRC);
	buf[2] = '\0';
	// Add STX, LRC and ETX to string
	//str = "02" + str + buf + "03"; // No need as the string already has STX, LRC and ETX
	std::string strWithLRC(str);
	int nAttempts = 0;
	while (nAttempts < 5)
	{
		hResult = m_pSerialComm->Write(strWithLRC.c_str(), strWithLRC.length());
		if (SUCCEEDED(hResult))
		{
			bResult = TRUE;
			break;
		}
		nAttempts++;
	}

	return bResult;
}

// Inits the GRAM scale
// Sets the scale in continuous streaming mode (Page 20 of Xtreme manual)
// Format: 15 bytes (set Data Address 1011h):
// 0	1-2			3-4			5		6		7		8		9		10		11		12-13		14
// STX	ID_O		ID_D		'E'		'1'		'0'		'1'		'1'		'0'		'0'		LRC			ETX
//
BOOL CThreadReadWeight::InitGRAMScale()
{
	BOOL bResult = TRUE;
	HRESULT hResult;
	// String without STX (1 byte), LRC (2 bytes) and ETX (1 byte);
	std::string str("\u000200FFE10110000\u0003\r\n");// \u0002 is start of packet				was ("0201E101100");
	unsigned char* inputToLRC = (unsigned char*)str.c_str();
	// Get LRC
	unsigned char cLRC = LRC(inputToLRC);
	// Convert LRC to Hex (2 bytes)
	char buf[2 + 1];
	sprintf(buf, "%02X", cLRC);
	buf[2] = '\0';
	// Add STX, LRC and ETX to string
	//str = "02" + str + buf + "03"; // No need as the new string has these additions already done
	std::string strWithLRC(str);
	hResult = m_pSerialComm->Write(strWithLRC.c_str(), strWithLRC.length());
	if (SUCCEEDED(hResult))
	{
		// Now check response to this request from scale to see if the operation has been successful.
		// Format of response is: 
		// STX ID_O ID_D 'e' '1' '0' '1' '2' '0' '1' Result LRC ETX // Result is byte 13 of a 16 byte string
		int nAttempts = 0;
		std::string strData;
		/*while (nAttempts < 5)
		{
			// SendReadWtRqstGRAM(); No need if in coninuous streaming mode!
			hResult = m_pSerialComm->Read_N(strData, 16, 500);
			//hResult = m_pSerialComm->ReadAvailable(strData);
			if (SUCCEEDED(hResult))
			{
				CString strTemp("Response to Init() from GRAM scale: ");
				CString strMsg(strData.substr(5, 8).c_str());
				IOutputLogString(strTemp + strMsg);
				char c = strData[0];
				if ('c' == 2)// Verify the first char is STX
				{
					// Verify LRC to ensure integrity of data
					// Remove STX (char 1), LRC (char 14 and 15) and ETX (char 16)
					std::string stringToSend = strData.substr(1, 12);
					std::string strLRC = strData.substr(13, 14);
					if (VerifyLRCGRAMScale(stringToSend, strLRC))
						break;
					else
						return FALSE;
				}
			}
			else
				IOutputLog(L"Reading response from scale after write() in Init() failed!");
			nAttempts++;
		}
		if (nAttempts < 5)
		{
			IOutputLog(L"GRAM Scale successfully set in continuous streaming mode!");
		}
		else
		{
			IOutputLog(L"GRAM Scale - could not set in continuous streaming mode! Weight reading disabled!");
			bResult = FALSE;
		}*/
		IOutputLog(L"Write() to GRAM scale put in continuous streaming mode succeeded!");
	}
	else
	{
		IOutputLog(L"Write() to GRAM scale in Init() failed! Failed to set scale in continuous streaming mode!");
		bResult = FALSE;
	}
	// Set weight send trigger interval to 100 ms (default is 50 ms)
	std::string str2("\u000200FF00130320000\u0003\r\n");// 200 ms (000200FF00130350000\u0003) - 500 ms
	hResult = m_pSerialComm->Write(str2.c_str(), str2.length());
	if (SUCCEEDED(hResult))
	{
		IOutputLog(L"Trigger interval set to 100 ms in streaming mode!");
	}
	else
	{
		IOutputLog(L"Failed to change trigger interval in streaming mode!");
		bResult = FALSE;
	}

	return bResult;
}

// Calculates LRC in the given strIn and matches it with the one in strLRC
// Assumption: 1. strIn is without STX, LRC and ETX
//			   2. LRC is as extracted from string received from scale. LRC is hex.
BOOL CThreadReadWeight::VerifyLRCGRAMScale(std::string strIn, std::string strLRC)
{
	return TRUE;
	unsigned char lrc = 0;
	const char* s = strIn.c_str();
	lrc = LRC((unsigned char*)strIn.c_str());
	std::string strUChar = hexToASCII(strLRC);

	if (lrc == *(strUChar.c_str()))
		return TRUE;
	else
		return FALSE;
}

// function to convert ascii char[] to hex-string (char[])
// Assumption: input is null terminated and output is previously allocated. size of output is strlen(input)*2 + 1
// 
std::string CThreadReadWeight::string2hexString(char* input)
{
	int len = strlen(input);
	char* output = new char[(len * 2) + 1];

	int loop;
	int i;

	i = 0;
	loop = 0;

	while (input[loop] != '\0')
	{
		sprintf((char*)(output + i), "%02X", input[loop]);
		loop += 1;
		i += 2;
	}
	
	output[i++] = '\0';
	std::string strOut(output);

	delete output;

	return strOut;
} //string2hexString()

// Convert hexadecimal string to ASCII format string 
std::string CThreadReadWeight::hexToASCII(std::string hex)
{
	// initialize the ASCII code string as empty. 
	string ascii = "";
	for (size_t i = 0; i < hex.length(); i += 2)
	{
		// extract two characters from hex string 
		string part = hex.substr(i, 2);

		// change it into base 16 and typecast as  character 
		char ch = stoul(part, nullptr, 16);

		// add this char to final ASCII string 
		ascii += ch;
	}
	return ascii;
} // hexToASCII()

unsigned char CThreadReadWeight::LRC(unsigned char* s)
{
	unsigned char lrc = 0;
	while (*s)
	{
		lrc ^= *(s++);
	}
	return lrc;
}

int CThreadReadWeight::HexStringToDecimal(std::string hex)
{
	int val = 0, decimal = 0, len = hex.length();
	for (int i = 0; hex[i] != '\0'; i++)
	{
		/* Find the decimal representation of hex[i] */
		if (hex[i] >= '0' && hex[i] <= '9')
		{
			val = hex[i] - 48;
		}
		else if (hex[i] >= 'a' && hex[i] <= 'f')
		{
			val = hex[i] - 97 + 10;
		}
		else if (hex[i] >= 'A' && hex[i] <= 'F')
		{
			val = hex[i] - 65 + 10;
		}

		decimal += val * pow(16, len - 1);
		len--;
	}

	return decimal;
}