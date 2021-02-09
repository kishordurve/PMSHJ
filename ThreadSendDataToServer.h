#pragma once


#include <cpr/cpr.h>
#include <iostream>
using namespace std;

//json lib header
#include <nlohmann/json.hpp>
using json = nlohmann::json;

// CThreadSendDataToServer

class CThreadSendDataToServer : public CWinThread
{
	DECLARE_DYNCREATE(CThreadSendDataToServer)

public:
	CThreadSendDataToServer();           // protected constructor used by dynamic creation
	CThreadSendDataToServer(HANDLE hEndSendDataThread, HANDLE hSendData);
	virtual ~CThreadSendDataToServer();
	CPMSHJApp* m_pApp;
	HANDLE	m_hEndSendDataThread;
	HANDLE	m_hSendDataToServer;// Initiates label pinting
	CString m_strURL, m_strKey, m_strFee, m_strHeight, m_strLength, m_strOperateMode, 
				m_strPackingNo, m_strSign, m_strVolume, m_strVolumeWeight, m_strWeight, m_strWidth, m_strError;
	STInspResult m_stInspResult;

	std::string hextostr(std::vector<unsigned char> const & hexval, bool uppercase = false);
	CString CreateHash(CString strData, CString strKey);
	eDataLoggingStatus SendDataToServer(CString strURL, json JsonDataSea);
	//Struct for the data to be send
	struct SendData
	{
		std::string strFee;
		std::string strHeight;
		std::string strLength;
		std::string strOperateMode;
		std::string strPackingNo;
		std::string strSign;
		std::string strVolume;
		std::string strVolumeWeight;
		std::string strWeight;
		std::string strWidth ;

		SendData()
		{
			strFee = strHeight = strLength = strOperateMode = strPackingNo = strSign = strVolume = strVolumeWeight = strWeight = strWidth = "";
		}

		SendData(SendData &other)
		{
			strFee = other.strFee;
			strHeight = other.strHeight;
			strLength = other.strLength;
			strOperateMode = other.strOperateMode;
			strPackingNo = other.strPackingNo;
			strSign = other.strSign;
			strVolume = other.strVolume;
			strVolumeWeight = other.strVolumeWeight;
			strWeight = other.strWeight;
			strWidth = other.strWidth;
		}
		SendData& operator = (SendData& other)
		{
			strFee = other.strFee;
			strHeight = other.strHeight;
			strLength = other.strLength;
			strOperateMode = other.strOperateMode;
			strPackingNo = other.strPackingNo;
			strSign = other.strSign;
			strVolume = other.strVolume;
			strVolumeWeight = other.strVolumeWeight;
			strWeight = other.strWeight;
			strWidth = other.strWidth;

			return *this;
		}
	}m_stSendData;

	json JsonDataSea;
	json JsonDataAir;

public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

protected:
	DECLARE_MESSAGE_MAP()
};


