#pragma once
#include "afxwin.h"
#include "ComPortList.h"


// CDlgSerialPortParams dialog

class CDlgSerialPortParams : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgSerialPortParams)

public:
	CDlgSerialPortParams(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgSerialPortParams();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_SERIAL_COMM };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_strCOMPort;
	CString m_strBaudRate;
	CString m_strByteSize;
	CString m_strParity;
	CString m_strStopBit;
	CComPortList m_listPorts;
	virtual BOOL OnInitDialog();
	virtual void OnOK();

	void SetCmbSelections(CString strBaudRate, CString strByteSize, CString strParity, CString strStopBit);

	CComboBox m_CmbBaudRate;
	CComboBox m_CmbByteSize;
	CComboBox m_CmbParity;
	CComboBox m_CmbStopBit;
	// Denotes weigh scale type - EXCELL or STANDARD as of now
	int m_nWeighScaleType;
	// Data (Weight) can be transmitted either continuously or when changed
	int m_nDataTransMode;
	// 0 is Input port and 1 is Output port
	int m_nWeighScalePort;
	afx_msg void OnBnClickedBnSetWeighScaleComPortParams();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBnClickedRadioWeighScaleInput();
	afx_msg void OnBnClickedRadio3();
	afx_msg void OnBnClickedRadioPrinter();
	CxShadeButton m_BnOK;
	CxShadeButton m_BnCancel;
	afx_msg void OnBnClickedScannerPort();
	afx_msg void OnBnClickedFootSwitchPort();
};
