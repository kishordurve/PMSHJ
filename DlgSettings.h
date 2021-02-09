#pragma once
#include "afxwin.h"


// CDlgSettings dialog

class CDlgSettings : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgSettings)

public:
	CDlgSettings(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgSettings();
	BOOL m_bCodeTestingON;
	void ReadCode();
	void SetButtonCaptions();


// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_SETTINGS };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBnCOMPortSettings();
	afx_msg void OnBnClickedBnWtSettings();
	afx_msg void OnBnClickedBnReadUDFFile();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	CxShadeButton m_BnWtSettings;
	CxShadeButton m_BnCOMPortSettings;
	CxShadeButton m_BnReadUDFFile;
	CxShadeButton m_BnSaveUDFFile;
	CxShadeButton m_BnInspParams;
	CxShadeButton m_BnOK;
	CxShadeButton m_BnBarcodeTest;
	CxShadeButton m_BnPrintParams;
	CxShadeButton m_BnCalib;
	CxShadeButton m_BnFreightParams;
	CxShadeButton m_BnDataSendParams;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedBnSaveUDFFile();
	CString m_strMachineName;
	virtual void OnOK();
	afx_msg void OnBnClickedBnInspParams();
	afx_msg void OnBnClickedBnCodeTest();
	CString m_strBarcode;
	CxShadeButton m_BnOptions;
	afx_msg void OnBnClickedBnOptions();
	afx_msg void OnMoving(UINT fwSide, LPRECT pRect);
	afx_msg void OnBnClickedBnPrintParams();
	afx_msg void OnBnClickedBnCalibration();
	afx_msg void OnBnClickedBnFreightParams();
	afx_msg void OnBnClickedBnDataSendParams();
};
