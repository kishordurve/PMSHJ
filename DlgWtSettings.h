#pragma once
#include "afxwin.h"


// CDlgWtSettings dialog

class CDlgWtSettings : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgWtSettings)

public:
	CDlgWtSettings(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgWtSettings();
	unsigned char hexdigit2int(unsigned char xd);
	int getHexVal(char c);
	const char* hexToAscii(const char * hexStr);
	CString DecToHexString(const int nVal);

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_WT_SETTINGS };
#endif
	CSerialCommHelper*	m_pSerialComm;
	BOOL m_bCOMPortOpened;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBnSetZeroWt();
	afx_msg void OnBnClickedBnSetTare();
	afx_msg void OnBnClickedBnSetPreTare();
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual void OnOK();
	CxShadeButton m_BnZeroWt;
	CxShadeButton m_BnTareWt;
	CxShadeButton m_BnPreTareWt;
	CxShadeButton m_BnOK;
	CxShadeButton m_BnCancel;
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	CString m_strPreTareWt;
	int m_nWtAccuracy;
};
