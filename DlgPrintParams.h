#pragma once
#include "afxwin.h"


// CDlgPrintParams dialog

class CDlgPrintParams : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgPrintParams)

public:
	CDlgPrintParams(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgPrintParams();
	BOOL PrintTestLabel();
	BOOL PrintTestLabelCLExpress();
	BOOL PrintTestLabelByFileTransfer();
	CStringA GetDateStringForPrinter();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_PRINT_PARAMS };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	int m_nLabelLength;
	int m_nLabelWidth;
	int m_nCodeHeight;
	int m_nNumPrints;
	CString m_strLogoFile;
	CxShadeButton m_BnTestPrint;
	CxShadeButton m_BnOK;
	CxShadeButton m_BnCancel;
	CxShadeButton m_BnSelectLogoFile;
	afx_msg void OnBnClickedBnTestPrint();
	CString m_strCorporateName;
	CString m_strProductName;
	CComboBox m_CmbFont;
	CComboBox m_CmbCodeType;
	CString m_strPrinterName;
	BOOL m_bGetUserOKForPrinting;
	BOOL m_bPrintLogo;
	BOOL m_bLogoUploadedToPrinter;
	afx_msg void OnBnClickedChkLogo();
	afx_msg void OnBnClickedBnLogoFile();
	CStatic m_staticLogoFile;
};
