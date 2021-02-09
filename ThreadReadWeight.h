#pragma once

using namespace std;
#include <string>

#include "SerialCommHelper.h"
// CThreadReadWeight

class CThreadReadWeight : public CWinThread
{
	DECLARE_DYNCREATE(CThreadReadWeight)

public:
	CThreadReadWeight();
	CThreadReadWeight(std::string strCOMPort, eWeighScaleType WeighScaleType, eWeighScaleMode WeighScaleMode);           // protected constructor used by dynamic creation
	virtual ~CThreadReadWeight();

	virtual BOOL InitInstance();
	virtual int ExitInstance();

	CSerialCommHelper*	m_pSerialComm;
	std::string m_strCOMPort;
	HANDLE m_hReadWeight;
	HANDLE m_hEndWtReadThread;
	std::string m_strWtUnitMarker;
	int nTimesWtUnitSet;

	CIHighResTimer		m_HighResTimer;
	double m_dWeight, m_dWeightOld;
	CString m_strWeight, m_strNewWeight, m_strWtUnit;
	eWeighScaleType m_WeighScaleType;
	eWeighScaleMode m_eWeighScaleMode;
	//eWtUnits m_eWtUnit;

	BOOL ReadWeightStandard(CString& strWeight, CString& strWtUnit, BOOL& bIsUnstable);
	BOOL ReadWeightExcell(CString& strWeight, CString& strWtUnit, BOOL& bIsUnstable);
	BOOL ReadWeightCH340(CString& strWeight, CString& strWtUnit, BOOL& bIsUnstable, BOOL& bIsGrossWt, BOOL& bIsTimeOut);
	BOOL ReadWeightP5(CString& strWeight, CString& strWtUnit, BOOL& bIsUnstable, BOOL& bIsOverWt, BOOL& bIsGrossWt, BOOL& bIsNetWt);
	BOOL ReadWeightGRAM(CString& strWeight, CString& strWtUnit, BOOL& bIsUnstable, BOOL& bIsOverWt);
	BOOL SendReadWtRqstGRAM();
	BOOL InitGRAMScale();
	BOOL VerifyLRCGRAMScale(std::string strIn, std::string strLRC);
	std::string string2hexString(char* input);
	std::string hexToASCII(string hex);
	unsigned char LRC(unsigned char* s);
	int HexStringToDecimal(std::string strInHex);
protected:
	DECLARE_MESSAGE_MAP()
};


