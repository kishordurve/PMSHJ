// ThreadHikVCode.cpp : implementation file
//

#include "stdafx.h"
#include "PMSHJ.h"
#include "ThreadHikVCode.h"


// CThreadHikVCode
bool IsStrUTF8(const char* pBuffer, int size)
{
	if (size < 0)
	{
		return false;
	}
	bool IsUTF8 = true;
	unsigned char* start = (unsigned char*)pBuffer;
	unsigned char* end = (unsigned char*)pBuffer + size;
	if (NULL == start ||
		NULL == end)
	{
		return false;
	}
	while (start < end)
	{
		if (*start < 0x80) // ch:(10000000): 值小于0x80的为ASCII字符 | en:(10000000): if the value is smaller than 0x80, it is the ASCII character
		{
			start++;
		}
		else if (*start < (0xC0)) // ch:(11000000): 值介于0x80与0xC0之间的为无效UTF-8字符 | en:(11000000): if the value is between 0x80 and 0xC0, it is the invalid UTF-8 character
		{
			IsUTF8 = false;
			break;
		}
		else if (*start < (0xE0)) // ch:(11100000): 此范围内为2字节UTF-8字符  | en: (11100000): if the value is between 0xc0 and 0xE0, it is the 2-byte UTF-8 character
		{
			if (start >= end - 1)
			{
				break;
			}

			if ((start[1] & (0xC0)) != 0x80)
			{
				IsUTF8 = false;
				break;
			}

			start += 2;
		}
		else if (*start < (0xF0)) // ch:(11110000): 此范围内为3字节UTF-8字符 | en: (11110000): if the value is between 0xE0 and 0xF0, it is the 3-byte UTF-8 character 
		{
			if (start >= end - 2)
			{
				break;
			}

			if ((start[1] & (0xC0)) != 0x80 || (start[2] & (0xC0)) != 0x80)
			{
				IsUTF8 = false;
				break;
			}

			start += 3;
		}
		else
		{
			IsUTF8 = false;
			break;
		}
	}

	return IsUTF8;
}

bool Char2Wchar(const char *pStr, wchar_t *pOutWStr, int nOutStrSize)
{
	if (!pStr || !pOutWStr)
	{
		return false;
	}

	bool bIsUTF = IsStrUTF8(pStr, strlen(pStr));
	UINT nChgType = bIsUTF ? CP_UTF8 : CP_ACP;

	int iLen = MultiByteToWideChar(nChgType, 0, (LPCSTR)pStr, -1, NULL, 0);

	memset(pOutWStr, 0, sizeof(wchar_t) * nOutStrSize);

	if (iLen >= nOutStrSize)
	{
		iLen = nOutStrSize - 1;
	}

	MultiByteToWideChar(nChgType, 0, (LPCSTR)pStr, -1, pOutWStr, iLen);

	pOutWStr[iLen] = 0;

	return true;
}

void __stdcall VirtualImageCallBack(MVID_CAM_OUTPUT_INFO* pFrameInfo, void* pUser)
{
	CThreadHikVCode * pThis = (CThreadHikVCode *)pUser;
	if (pThis)
	{
		pThis->ImageCallBack(pFrameInfo);
	}
}

IMPLEMENT_DYNCREATE(CThreadHikVCode, CWinThread)

CThreadHikVCode::CThreadHikVCode():
m_bProcess(FALSE)
{
	m_pstOutput = (MVID_CAM_OUTPUT_INFO *)malloc(sizeof(MVID_CAM_OUTPUT_INFO));
	if (NULL == m_pstOutput)
	{
		//return FALSE;
	}
	else
		memset(m_pstOutput, 0, sizeof(MVID_CAM_OUTPUT_INFO));

	// Device Information List Initialization
	m_pstDevList = new MVID_CAMERA_INFO_LIST();
	if (NULL == m_pstDevList)
	{
		//return;
	}
	else
		memset(m_pstDevList, 0, sizeof(MVID_CAMERA_INFO_LIST));

	m_bIsNewCode = TRUE;
	CPMSHJApp* pApp = (CPMSHJApp*)AfxGetApp();
	m_hEndScannerThread = pApp->m_hEndScannerThread;
	m_nBarcodeLengthMin = pApp->m_stInspParams.nBarcodeLengthMin;
	m_nBarcodeLengthMax = pApp->m_stInspParams.nBarcodeLengthMax;
	m_CodeTypeRqd = pApp->m_stPrintParams.CodeType;

	m_bNeedToCheckCodeRange = FALSE;
}

CThreadHikVCode::CThreadHikVCode(int nBarcodeLengthMin, int nBarcodeLengthMax, char cEndChar, CString strBarcodeValMin, CString strBarcodeValMax)
{
	m_nBarcodeLengthMin = nBarcodeLengthMin;
	m_nBarcodeLengthMax = nBarcodeLengthMax;
	m_cEndChar = cEndChar;
	CPMSHJApp* pApp = (CPMSHJApp*)AfxGetApp();
	m_hEndScannerThread = pApp->m_hEndScannerThread;
	m_bIsNewCode = TRUE;

	m_strBarcodeValMin = strBarcodeValMin;
	m_strBarcodeValMax = strBarcodeValMax;
	IOutputLog(L"Code String Lower, Upper: ");
	IOutputLogString(m_strBarcodeValMin);
	IOutputLogString(m_strBarcodeValMax);

	CString str = m_strBarcodeValMax;
	for (int i = 0; i < str.GetLength(); i++)
		if (!isdigit(str[i]))
			str.Remove(str[i--]);
	m_dUpper = _wtof(str.GetString());
	str.Format(L"m_dUpper: %16.0f", m_dUpper);
	IOutputLogString(str);
	str = m_strBarcodeValMin;
	for (int i = 0; i < str.GetLength(); i++)
		if (!isdigit(str[i]))
			str.Remove(str[i--]);
	m_dLower = _wtof(str.GetString());
	str.Format(L"Code Value Lower / Upper: %16f, %16f", m_dLower, m_dUpper);
	IOutputLogString(str);
	// Need to check only if upper limit > 0
	if (m_dUpper > EPSILON)
	{
		IOutputLog(L"Code Range Check Effective");
		m_bNeedToCheckCodeRange = TRUE;
	}
	else
		m_bNeedToCheckCodeRange = FALSE;
}

CThreadHikVCode::~CThreadHikVCode()
{
}

BOOL CThreadHikVCode::InitInstance()
{
	CString strMsg;
	CPMSHJApp* pApp = (CPMSHJApp*)AfxGetApp();
	m_bProcess = FALSE;
	// Device Information List Initialization
	m_pstDevList = new MVID_CAMERA_INFO_LIST();
	if (NULL != m_pstDevList)
	{
		memset(m_pstDevList, 0, sizeof(MVID_CAMERA_INFO_LIST));
		// Enumerate all devices within subnet  MVID_CR_CAM_EnumDevicesByCfg
		int nRet = MVID_CR_CAM_EnumDevices(m_pstDevList);
		if ((MVID_CR_OK == nRet) && (m_pstDevList->nCamNum > 0))
		{
			MVID_CAMERA_INFO* pDeviceInfo = m_pstDevList->pstCamInfo[0];

			if ((pDeviceInfo->nCamType == MVID_GIGE_CAM) || (pDeviceInfo->nCamType == MVID_USB_CAM))
			{
				int nRet = MVID_CR_CreateHandle(&m_handle, MVID_BCR | MVID_TDCR);
				if (MVID_CR_OK == nRet)
				{
					nRet = MVID_CR_CAM_BindDevice(m_handle, m_pstDevList->pstCamInfo[0]);
					if (MVID_CR_OK == nRet)
					{
						nRet = MVID_CR_CAM_RegisterImageCallBack(m_handle, VirtualImageCallBack, this);
						if (MVID_CR_OK == nRet)
						{
							nRet = MVID_CR_CAM_StartGrabbing(m_handle);
							m_bProcess = TRUE;
							IOutputLog(L"Hikvision scanner started grabbing!");
						}
						else
							IOutputLog(L"Hikvision scanner failed in MVID_CR_CAM_RegisterImageCallBack()!");
					}
					else
						IOutputLog(L"Hikvision scanner failed in MVID_CR_CAM_BindDevice()!");
				}
				else
					IOutputLog(L"Hikvision scanner failed in MVID_CR_CreateHandle()!");
			} // if (pDeviceInfo->nCamType == MVID_GIGE_CAM)
			else
			{
				// Should exit the thread here!
				AfxMessageBox(L"Unknown Scanner!");
				m_bProcess = FALSE;
			}
			while ((WaitForSingleObject(m_hEndScannerThread, 50) == WAIT_TIMEOUT) && m_bProcess)
			{
				if (m_bIsNewCode && !m_strCode.IsEmpty() && (m_strCode != m_strOldCode))
				{
					// To ensure that only one update is done once a valid code is read. This flag gets reset at the next invalid read
					m_bIsNewCode = FALSE;
					m_strOldCode = m_strCode;
					EnterCriticalSection(&(pApp->m_csScannerString));
					pApp->m_stBarcodeParts.pt1 = m_pt1;
					pApp->m_stBarcodeParts.pt2 = m_pt2;
					pApp->m_stBarcodeParts.pt3 = m_pt3;
					pApp->m_stBarcodeParts.pt4 = m_pt4;
					pApp->m_strScannerString = m_strCode;
					if (m_bNeedToCheckCodeRange)
					{
						//IOutputLog(L"Thread InitInstance: Need to check code status");
						if (m_bCodeInRange)
						{
							IOutputLog(L"Thread InitInstance: Need to check code status, status OK.");
							pApp->m_pFrame->m_pImageView->m_BarCodeReadStatus = CODE_OK;
						}
						else
							pApp->m_pFrame->m_pImageView->m_BarCodeReadStatus = CODE_OUT_OF_RANGE;
					}
					else
						pApp->m_pFrame->m_pImageView->m_BarCodeReadStatus = CODE_OK;
					LeaveCriticalSection(&(pApp->m_csScannerString));
					//IOutputLog(L"Code Rcd:");
					//IOutputLogString(m_strCode);
					//AfxGetApp()->m_pMainWnd->PostMessage(WM_UPDATE_CODE, (WPARAM)((CW2A(m_strCode)), (LPARAM)(0)));
				}
				else if (!m_strCode.IsEmpty()) // To make sure that the code corner points are updated if box has moved
				{
					if ((abs(pApp->m_stBarcodeParts.pt1.x - m_pt1.x) > 5) || (abs(pApp->m_stBarcodeParts.pt1.y - m_pt1.y) > 5) ||
						(abs(pApp->m_stBarcodeParts.pt2.x - m_pt2.x) > 5) || (abs(pApp->m_stBarcodeParts.pt2.y - m_pt2.y) > 5) ||
						(abs(pApp->m_stBarcodeParts.pt3.x - m_pt3.x) > 5) || (abs(pApp->m_stBarcodeParts.pt3.y - m_pt3.y) > 5) ||
						(abs(pApp->m_stBarcodeParts.pt4.x - m_pt4.x) > 5) || (abs(pApp->m_stBarcodeParts.pt4.y - m_pt4.y) > 5))
					{
						EnterCriticalSection(&(pApp->m_csScannerString));
						pApp->m_stBarcodeParts.pt1 = m_pt1;
						pApp->m_stBarcodeParts.pt2 = m_pt2;
						pApp->m_stBarcodeParts.pt3 = m_pt3;
						pApp->m_stBarcodeParts.pt4 = m_pt4;
						LeaveCriticalSection(&(pApp->m_csScannerString));
					}
				}
				MSG msg;
				while (::PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
				{
					if (!pApp->PumpMessage()) //if WM_QUIT i.e User exited the application
					{
						::PostQuitMessage(0);
						m_bProcess = FALSE;// FT_APPLN_QUIT;
					}
				}
			}
		}
		else if (0 == m_pstDevList->nCamNum)
		{
			m_bProcess = FALSE;
			CString errorMsg;
			errorMsg.Format(_T("HikVision Scanner Not Found!"));
			AfxMessageBox(errorMsg);
		}
	}
	else
		IOutputLog(L"HikVision Scanner Not Found - Aborting Scanning!");
	
	IOutputLog(L"End HikVision Scanner Thread Triggered - Aborting Scanning!");
	if (m_handle)
	{
		MVID_CR_DestroyHandle(m_handle);
		m_handle = NULL;
	}
	if (pApp)// App may have died by the time it comes here
	{
		pApp->m_bScannerThreadUp = FALSE;
	}
	pApp->m_pThreadHikVCode = NULL;
	AfxEndThread(0);

	return TRUE;
}

int CThreadHikVCode::ExitInstance()
{
	// TODO:  perform any per-thread cleanup here
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CThreadHikVCode, CWinThread)
END_MESSAGE_MAP()


// CThreadHikVCode message handlers
void CThreadHikVCode::ImageCallBack(MVID_CAM_OUTPUT_INFO* pstOutput)
{
	int nRet = MVID_CR_OK;
	CPMSHJApp* pApp = (CPMSHJApp*)AfxGetApp();
	CString strMsg;
	//m_strCode.Empty();
	if (m_bProcess && NULL != pstOutput)
	{
		if (0 != pstOutput->stCodeList.nCodeNum)
		{
			// Output result - this effectively takes only the last code
			for (int i = 0; i < pstOutput->stCodeList.nCodeNum; ++i)
			{
				if (m_CodeTypeRqd == MVID_CODE_TDCR_QR)// Only for QRcode our enum matches with HikVision #define
				{
					if (pstOutput->stCodeList.stCodeInfo[i].enBarType != m_CodeTypeRqd)
					{
						m_bCodeInRange = FALSE;
						continue;
					}
				}
				else // Expected is barcode
				{
					if (pstOutput->stCodeList.stCodeInfo[i].enBarType == MVID_CODE_TDCR_QR)// Wrong code
					{
						m_bCodeInRange = FALSE;
						continue;
					}
				}
				wchar_t strWchar[MVID_MAX_CODECHARATERLEN] = { 0 };
				std::string str((char*)pstOutput->stCodeList.stCodeInfo[i].strCode);
				if ((str.length() < m_nBarcodeLengthMin) || (str.length() > m_nBarcodeLengthMax))
				{
					IOutputLog(L"Barcode type correct but length different.");
					m_bCodeInRange = FALSE;
					continue;
				}
				// Check for code validity - code value must lie between min and max as specified in insp params
				if (m_bNeedToCheckCodeRange)
				{
					//IOutputLog(L"Entering Code Range Check");

					CString strRcd(str.c_str());
					for (int i = 0; i < strRcd.GetLength(); i++)
						if (!isdigit(strRcd[i]))
							strRcd.Remove(strRcd[i--]);
					double dRcd = _wtof(strRcd.GetString());
					//strRcd.Format(L"Code Rcd: %s, dRcd: %16.0f, dUpper: %16.0f, dLower: %16.0f", str.c_str(), dRcd, m_dUpper, m_dLower);
					//IOutputLogString(strRcd);
					if ((dRcd > (m_dUpper + EPSILON)) || (dRcd < (m_dLower - EPSILON)))
					{
						if (m_bCodeInRange)// Ensures message is sent only once per code
						{
							AfxGetApp()->m_pMainWnd->PostMessage(WM_CODE_READ_STATUS, (WPARAM)(CODE_OUT_OF_RANGE), (LPARAM)(0));
							m_bCodeInRange = FALSE;
						}
						//continue;
					}
					else
					{
						if (!m_bCodeInRange)// Ensures message is sent only once per code
						{
							AfxGetApp()->m_pMainWnd->PostMessage(WM_CODE_READ_STATUS, (WPARAM)(CODE_OK), (LPARAM)(0));
							m_bCodeInRange = TRUE;
						}
					}
				}
				//else
				//	IOutputLog(L"Doesn't need code range check");
				Char2Wchar((char*)pstOutput->stCodeList.stCodeInfo[i].strCode, strWchar, MVID_MAX_CODECHARATERLEN);
				m_pt1.x = pstOutput->stCodeList.stCodeInfo[i].stCornerPt[0].nX;
				m_pt1.y = pstOutput->stCodeList.stCodeInfo[i].stCornerPt[0].nY;
				m_pt2.x = pstOutput->stCodeList.stCodeInfo[i].stCornerPt[1].nX;
				m_pt2.y = pstOutput->stCodeList.stCodeInfo[i].stCornerPt[1].nY;
				m_pt3.x = pstOutput->stCodeList.stCodeInfo[i].stCornerPt[2].nX;
				m_pt3.y = pstOutput->stCodeList.stCodeInfo[i].stCornerPt[2].nY;
				m_pt4.x = pstOutput->stCodeList.stCodeInfo[i].stCornerPt[3].nX;
				m_pt4.y = pstOutput->stCodeList.stCodeInfo[i].stCornerPt[3].nY;
				m_strCode.Format(_T("%s"), strWchar);
			}
			CString strMsg("Code Received: ");
			//strMsg += m_strCode;
			//IOutputLogString(strMsg);
			Sleep(100);
		}
		else
		{
			m_strCode.Empty();
			m_bIsNewCode = TRUE;// Setting flag for next valid read. This will be reset after it is written to App in InitInstance()
			strMsg.Format(_T("No object is recognized, frame No.[%d]"), pstOutput->stImage.nFrameNum);
			//IOutputLogString(strMsg);
			if ((m_pt1.x <= 0) && (m_pt1.y <= 0))
			{
				EnterCriticalSection(&(pApp->m_csScannerString));
				pApp->m_stBarcodeParts.pt1 = m_pt1 = CPoint(0,0);
				pApp->m_stBarcodeParts.pt2 = m_pt2 = CPoint(0, 0);
				pApp->m_stBarcodeParts.pt3 = m_pt3 = CPoint(0, 0);
				pApp->m_stBarcodeParts.pt4 = m_pt4 = CPoint(0, 0);
				LeaveCriticalSection(&(pApp->m_csScannerString));
			}

			Sleep(200);
		}
	}
}
