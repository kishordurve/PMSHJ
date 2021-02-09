#pragma once
#include "afxwin.h"


// CDlgOptions dialog

class CDlgOptions : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgOptions)

public:
	CDlgOptions(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgOptions();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_OPTIONS };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	int m_nLanguage;
	int m_nDimUnit;
	CxShadeButton m_BnOK;
	CxShadeButton m_BnCancel;
	//BOOL m_bSerialPortScanner;
	int m_nSuffixChar;
	BOOL m_bShowNetWt;
	BOOL m_bShowGrossWt;
	BOOL m_bShowTareWt;
	BOOL m_bShowPreTareWt;
	BOOL m_bPrintLabel;
	int m_nTriggerSource;
	CxShadeButton m_BnTestFootSwitch;
	afx_msg void OnBnClickedBnTestFootSwitch();
	afx_msg void OnBnClickedRadioFootSwitch();
	afx_msg void OnBnClickedRadioTrgSource();
	int m_nScannerType;
	BOOL m_bEnableWt;
};
