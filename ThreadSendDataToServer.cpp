// ThreadSendDataToServer.cpp : implementation file
//

#include "stdafx.h"
#include "PMSHJ.h"
#include "ThreadSendDataToServer.h"
#include <bcrypt.h>

#include <iostream>
#include <iomanip>

// CThreadSendDataToServer

IMPLEMENT_DYNCREATE(CThreadSendDataToServer, CWinThread)

CThreadSendDataToServer::CThreadSendDataToServer()
{
	m_pApp = (CPMSHJApp*)AfxGetApp();
	m_hEndSendDataThread = m_pApp->m_hEndSendDataThread;
	m_hSendDataToServer = m_pApp->m_hSendDataToServer;
	m_strKey = m_pApp->m_stServerCommParams.strKey1;// "vbD#&!f&P&24$9x8XC*Q%Z^VJPiKNNg@";
	m_strURL = m_pApp->m_stServerCommParams.strURL1;// "http://47.114.126.132:9983/system/packing/data";
}

CThreadSendDataToServer::CThreadSendDataToServer(HANDLE hEndSendDataThread, HANDLE hSendData)
{
	m_pApp = (CPMSHJApp*)AfxGetApp();
	m_hEndSendDataThread = m_pApp->m_hEndSendDataThread;
	m_hSendDataToServer = m_pApp->m_hSendDataToServer;
	m_strKey = m_pApp->m_stServerCommParams.strKey1;// "vbD#&!f&P&24$9x8XC*Q%Z^VJPiKNNg@";
	m_strURL = m_pApp->m_stServerCommParams.strURL1;// "http://47.114.126.132:9983/system/packing/data";
}

CThreadSendDataToServer::~CThreadSendDataToServer()
{
}

BOOL CThreadSendDataToServer::InitInstance()
{
	CString strMsg;
	strMsg.Format(L"CThreadSendDataToServer: %s", m_strURL);
	IOutputLogString(strMsg);
	while (WaitForSingleObject(m_hEndSendDataThread, 5) == WAIT_TIMEOUT)
	{
		while (WaitForSingleObject(m_hSendDataToServer, 5) != WAIT_TIMEOUT)
		{
			IOutputLog(L"Received trigger to send data to server");
			// Prepare data here, create HMAC and send it to server
			CString strData;
			m_stInspResult = m_pApp->m_stInspResultSaved;
			// Ensure .xx for 'fee'
			m_strFee.Format(L"%2.2f", m_stInspResult.dFreight);
			if (m_strFee.Find('.') == -1)
				m_strFee += L".00";
			if (m_strFee.Find('.') < (m_strFee.GetLength() - 3))
				m_strFee = m_strFee.Left(m_strFee.Find('.') + 3);
			while (m_strFee.Find('.') > (m_strFee.GetLength() - 3))
				m_strFee += L"0";
			// Ensure .xx for 'weight'
			m_strWeight.Format(L"%2.2f", m_stInspResult.dWeightNet);
			if (m_strWeight.Find('.') == -1)
				m_strWeight += L".00";
			if (m_strWeight.Find('.') < (m_strWeight.GetLength() - 3))
				m_strWeight = m_strWeight.Left(m_strWeight.Find('.') + 3);
			while (m_strWeight.Find('.') > (m_strWeight.GetLength() - 3))
				m_strWeight += L"0";
			int nRoundedOffLength = m_stInspResult.dLength / 10 + 0.5;
			int nRoundedOffWidth = m_stInspResult.dWidth / 10 + 0.5;
			int nRoundedOffHeight = m_stInspResult.dHeight / 10 + 0.5;
			// Ensure .xxx for 'volume'
			m_strVolume.Format(L"%3.2f", 1.0 * nRoundedOffLength * nRoundedOffWidth * nRoundedOffHeight / (1000 * 1000.0));// Volume is in M3
			if (m_strVolume.Find('.') == -1)
				m_strVolume += L".000";
			if (m_strVolume.Find('.') < (m_strVolume.GetLength() - 4))
				m_strVolume = m_strVolume.Left(m_strVolume.Find('.') + 4);
			while (m_strVolume.Find('.') > (m_strVolume.GetLength() - 4))
				m_strVolume += L"0";

			if (m_stInspResult.FreightMode == FREIGHT_AIR)
			{
				//m_strVolume.Format(L"%3.2f", 1.0 * nRoundedOffLength * nRoundedOffWidth * nRoundedOffHeight / 5000.0);
				m_strVolumeWeight.Format(L"%3.2f", 1.0 * nRoundedOffLength * nRoundedOffWidth * nRoundedOffHeight / 5000.0);
			}
			else
			{
				m_strVolumeWeight.Format(L"0.01");
				// Ensure .xxx for 'volume'
				//m_strVolume.Format(L"%3.2f", 1.0 * nRoundedOffLength * nRoundedOffWidth * nRoundedOffHeight / (1000 * 1000.0));// Volume is in M3
				//if (m_strVolume.Find('.') == -1)
				//	m_strVolume += L".000";
				//if (m_strVolume.Find('.') < (m_strVolume.GetLength() - 4))
				//	m_strVolume = m_strVolume.Left(m_strVolume.Find('.') + 4);
				//while (m_strVolume.Find('.') > (m_strVolume.GetLength() - 4))
				//	m_strVolume += L"0";
			}
			// Dimensions are needed in cm
			m_strHeight.Format(L"%d", (int)(m_stInspResult.dHeight / 10 + 0.5));
			m_strLength.Format(L"%d", (int)(m_stInspResult.dLength / 10 + 0.5));
			m_strWidth.Format(L"%d", (int)(m_stInspResult.dWidth / 10 + 0.5));
			m_strOperateMode.Format(L"%d", (int)(m_stInspResult.FreightMode));
			m_strPackingNo = m_stInspResult.strBarcode;
			// String for creating HMAC
			if (m_stInspResult.FreightMode == FREIGHT_AIR)
			{
				//strData.Format(L"fee=%s&height=%s&length=%s&operateMode=%s&packingNo=%s&volumeWeight=%s&weight=%s&width=%s",
				//	m_strFee, m_strHeight, m_strLength, m_strOperateMode, m_strPackingNo, m_strVolume, m_strWeight, m_strWidth);
				strData.Format(L"fee=%s&height=%s&length=%s&operateMode=%s&packingNo=%s&volume=%s&volumeWeight=%s&weight=%s&width=%s",
					m_strFee, m_strHeight, m_strLength, m_strOperateMode, m_strPackingNo, m_strVolume, m_strVolumeWeight, m_strWeight, m_strWidth);
			}
			else
			{
				//strData.Format(L"fee=%s&height=%s&length=%s&operateMode=%s&packingNo=%s&volume=%s&weight=%s&width=%s",
				//	m_strFee, m_strHeight, m_strLength, m_strOperateMode, m_strPackingNo, m_strVolume, m_strWeight, m_strWidth);
				strData.Format(L"fee=%s&height=%s&length=%s&operateMode=%s&packingNo=%s&volume=%s&volumeWeight=%s&weight=%s&width=%s",
					m_strFee, m_strHeight, m_strLength, m_strOperateMode, m_strPackingNo, m_strVolume, m_strVolumeWeight, m_strWeight, m_strWidth);
			}

			CreateHash(strData, m_strKey);
			strMsg.Format(L"Data: %s, Key: %s, HMAC: %s", strData, m_strKey, m_strSign);
			IOutputLogString(strMsg);
			// Fill structure
			m_stSendData.strFee = (CW2A)m_strFee;
			m_stSendData.strHeight = (CW2A)m_strHeight;
			m_stSendData.strLength = (CW2A)m_strLength;
			m_stSendData.strOperateMode = (CW2A)m_strOperateMode;
			m_stSendData.strPackingNo = (CW2A)m_strPackingNo;
			m_stSendData.strSign = (CW2A)m_strSign;
			m_stSendData.strVolume = (CW2A)m_strVolume;
			m_stSendData.strVolumeWeight = (CW2A)m_strVolumeWeight;
			m_stSendData.strWeight = (CW2A)m_strWeight;
			m_stSendData.strWidth = (CW2A)m_strWidth;

			// Build json data structure
			eDataLoggingStatus DataLoggingStatus;
			if (m_stInspResult.FreightMode == FREIGHT_AIR)
			{
				JsonDataAir["fee"] = m_stSendData.strFee;
				JsonDataAir["height"] = stoi(m_stSendData.strHeight);
				JsonDataAir["length"] = stoi(m_stSendData.strLength);
				JsonDataAir["operateMode"] = stoi(m_stSendData.strOperateMode);
				JsonDataAir["packingNo"] = m_stSendData.strPackingNo;
				JsonDataAir["sign"] = m_stSendData.strSign;
				JsonDataAir["volume"] = m_stSendData.strVolume;
				JsonDataAir["volumeWeight"] = m_stSendData.strVolumeWeight;
				JsonDataAir["weight"] = m_stSendData.strWeight;
				JsonDataAir["width"] = stoi(m_stSendData.strWidth);

				IOutputLog(L"Sending Air Freight Data to Server. W/F Response:");
				CString strVolumeMsg(CA2W(m_stSendData.strVolume.c_str()));
				strVolumeMsg.Format(L"Air Freight Volume: %s", CA2W(m_stSendData.strVolume.c_str()));
				IOutputLogString(strVolumeMsg);
				DataLoggingStatus = SendDataToServer(m_strURL, JsonDataAir);
			}
			else
			{
				JsonDataSea["fee"] = m_stSendData.strFee;
				JsonDataSea["height"] = stoi(m_stSendData.strHeight);
				JsonDataSea["length"] = stoi(m_stSendData.strLength);
				JsonDataSea["operateMode"] = stoi(m_stSendData.strOperateMode);
				JsonDataSea["packingNo"] = m_stSendData.strPackingNo;
				JsonDataSea["sign"] = m_stSendData.strSign;
				JsonDataSea["volume"] = m_stSendData.strVolume;
				JsonDataSea["volumeWeight"] = m_stSendData.strVolumeWeight;
				JsonDataSea["weight"] = m_stSendData.strWeight;
				JsonDataSea["width"] = stoi(m_stSendData.strWidth);

				IOutputLog(L"Sending Ocean Freight Data to Server. W/F Response:");
				CString strVolumeMsg(CA2W(m_stSendData.strVolumeWeight.c_str()));
				strVolumeMsg.Format(L"Sea Freight VolumeWeight: %s", CA2W(m_stSendData.strVolumeWeight.c_str()));
				IOutputLogString(strVolumeMsg);
				DataLoggingStatus = SendDataToServer(m_strURL, JsonDataSea);
			}

			AfxGetApp()->m_pMainWnd->PostMessage(WM_DATA_LOG_STATUS, (WPARAM)(DataLoggingStatus), (LPARAM)(0));
			IOutputLogString(m_strError);
		}
		MSG msg;
		while (::PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
		{
			if (!m_pApp->PumpMessage()) //if WM_QUIT i.e User exited the application
			{
				IOutputLog(L"Quit Application Triggered - Aborting ThreadSendDataToServer !");
				return FALSE;// FT_APPLN_QUIT;
			}
		}
	}
	IOutputLog(L"End Thread Triggered - Aborting ThreadSendDataToServer!");
	ResetEvent(m_hEndSendDataThread);
	if (m_pApp)
		m_pApp->m_bSendDataThreadUp = FALSE;

	return TRUE;
}

int CThreadSendDataToServer::ExitInstance()
{
	// TODO:  perform any per-thread cleanup here
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CThreadSendDataToServer, CWinThread)
END_MESSAGE_MAP()

#define NT_SUCCESS(Status)          (((NTSTATUS)(Status)) >= 0)
#define STATUS_UNSUCCESSFUL         ((NTSTATUS)0xC0000001L)
//#pragma comment(lib, "bcrypt.lib")
// This function creates and returns HASH for strData with strKey1 using MD5 algorithm
// 
CString CThreadSendDataToServer::CreateHash(CString strData, CString strKey1)
{
	BOOL bResult = TRUE;
	CString strToSend;
	std::vector<unsigned char> hexval;
	std::string strstdResult;
	BCRYPT_ALG_HANDLE	hAlg = NULL;
	BCRYPT_HASH_HANDLE  hHash = NULL;
	NTSTATUS            status = STATUS_UNSUCCESSFUL;
	DWORD               cbData = 0,
		cbHash = 0,
		cbHashObject = 0;
	PBYTE               pbHashObject = NULL;
	PBYTE               pbHash = NULL;
	std::string			strstdKey = CW2A(strKey1);
	BYTE* key = new BYTE[strstdKey.length()];//BYTE key[32];// = { "vbD#&!f&P&24$9x8XC*Q%Z^VJPiKNNg@" };
	std::string			strstdData = CW2A(strData);
	int nSize = strstdData.length();
	BYTE* message = new BYTE[strstdData.length()];//BYTE message[100];	
	for (int i = 0; i < 32; i++)
		key[i] = strstdKey[i];	
	for (int i = 0; i < strstdData.length(); i++)
		message[i] = strstdData[i];
	// Open an algorithm handle. We should be using the algo handle hAlg as class variable 
	// as this func is time intensive (MSDN) https://docs.microsoft.com/en-us/windows/win32/seccng/creating-a-hash-with-cng
	if (!NT_SUCCESS(status = BCryptOpenAlgorithmProvider(
		&hAlg,
		BCRYPT_MD5_ALGORITHM,
		NULL,
		BCRYPT_ALG_HANDLE_HMAC_FLAG)))// BCRYPT_HASH_REUSABLE_FLAG 
	{
		wprintf(L"**** Error 0x%x returned by BCryptOpenAlgorithmProvider\n", status);
		goto Cleanup;
	}
	//calculate the size of the buffer to hold the hash object
	if (!NT_SUCCESS(status = BCryptGetProperty(
		hAlg,
		BCRYPT_OBJECT_LENGTH,
		(PBYTE)&cbHashObject,
		sizeof(DWORD),
		&cbData,
		0)))
	{
		wprintf(L"**** Error 0x%x returned by BCryptGetProperty\n", status);
		goto Cleanup;
	}

	//allocate the hash object on the heap
	pbHashObject = (PBYTE)HeapAlloc(GetProcessHeap(), 0, cbHashObject);
	if (NULL == pbHashObject)
	{
		wprintf(L"**** memory allocation failed\n");
		goto Cleanup;
	}
	//calculate the length of the hash
	if (!NT_SUCCESS(status = BCryptGetProperty(
		hAlg,
		BCRYPT_HASH_LENGTH,
		(PBYTE)&cbHash,
		sizeof(DWORD),
		&cbData,
		0)))
	{
		wprintf(L"**** Error 0x%x returned by BCryptGetProperty\n", status);
		goto Cleanup;
	}

	//allocate the hash buffer on the heap
	pbHash = (PBYTE)HeapAlloc(GetProcessHeap(), 0, cbHash);
	if (NULL == pbHash)
	{
		wprintf(L"**** memory allocation failed\n");
		goto Cleanup;
	}

	//create a hash
	nSize = sizeof(key);
	if (!NT_SUCCESS(status = BCryptCreateHash(
		hAlg,
		&hHash,
		pbHashObject,
		cbHashObject,
		(PBYTE)(key),
		strstdKey.length(),
		0)))
	{
		wprintf(L"**** Error 0x%x returned by BCryptCreateHash\n", status);
		goto Cleanup;
	}

	//hash some data
	nSize = sizeof(message);
	if (!NT_SUCCESS(status = BCryptHashData(
		hHash,
		(PBYTE)(message),
		strstdData.length(),
		0)))
	{
		AfxMessageBox(L"**** Error 0x%x returned by BCryptHashData\n", status);
		wprintf(L"**** Error 0x%x returned by BCryptHashData\n", status);
		goto Cleanup;
	}

	//close the hash
	if (!NT_SUCCESS(status = BCryptFinishHash(
		hHash,
		pbHash,
		cbHash,
		0)))
	{
		AfxMessageBox(L"**** Error 0x%x returned by BCryptFinishHash\n", status);
		goto Cleanup;
	}

	TRACE("The hash is:  ");
	//;
	for (DWORD i = 0; i < cbHash; i++)
	{
		unsigned char c = pbHash[i];
		hexval.push_back(c);
	}
	strstdResult = hextostr(hexval);

	m_strSign = CString(strstdResult.c_str());
	IOutputLogString(m_strSign);
	nSize = m_strSign.GetLength();
	delete[] key;
	delete[] message;
Cleanup:

	if (hAlg)
	{
		BCryptCloseAlgorithmProvider(hAlg, 0);
	}

	if (hHash)
	{
		BCryptDestroyHash(hHash);
	}

	if (pbHashObject)
	{
		HeapFree(GetProcessHeap(), 0, pbHashObject);
	}

	if (pbHash)
	{
		HeapFree(GetProcessHeap(), 0, pbHash);
	}

	return m_strSign;
}

// CThreadSendDataToServer message handlers
std::string CThreadSendDataToServer::hextostr(std::vector<unsigned char> const & hexval, bool uppercase)
{
	std::stringstream sstr;

	if (!hexval.empty())
	{
		sstr.setf(std::ios_base::hex, std::ios::basefield);
		if (uppercase)
			sstr.setf(std::ios_base::uppercase);
		sstr.fill('0');
		for (size_t i = 0; i<hexval.size(); i++)
		{
			sstr << std::setw(2) << (unsigned int)(unsigned char)(hexval[i]);
		}
	}

	return sstr.str();
}

// RetVal 0 - Success, otherwise Fail
// 
eDataLoggingStatus CThreadSendDataToServer::SendDataToServer(CString strURL1, json JsonData)
{
	m_strError.Empty();
	std::string ResponseText;
	IOutputLogString(strURL1);
	CString strMsg;
	cpr::Response ServerResponse = cpr::Put(cpr::Url{ (CW2A)strURL1 },
		cpr::Header{ { "Content-Type", "application/json" } },
		cpr::Body{ JsonData.dump() }
	);
	//Checking the response object from server
	ResponseText = ServerResponse.text;
	if (strstr(ResponseText.c_str(), "00000") != NULL) {
		//success
		IOutputLog(L"Data Logging Success!");
		return DATA_OK;
	}
	else
	{
		//failure
		CString strFail(ResponseText.c_str());
		if (strFail.Find(L"A0322"))
			m_strError.Format(L"Data Logging Error A0322 - Packing freight has been calculated.");
		else if (strFail.Find(L"A0309"))
			m_strError.Format(L"Data Logging Error A0309 - Packing does not exist.");
		else if (strFail.Find(L"A0323"))
			m_strError.Format(L"Data Logging Error A0323 - Packing data verification failed.");
		else if (strFail.Find(L"B0104"))
			m_strError.Format(L"Data Logging Error B0104 - Failed to edit (write).");

		//AfxMessageBox(m_strError);
		IOutputLogString(m_strError);
		return DATA_FAIL;
	}
}
