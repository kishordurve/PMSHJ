#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CDlgReport dialog

class CDlgReport : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgReport)

public:
	int m_nCurSelIndex;

	CDlgReport(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgReport();

	CDatabase	m_PMSDatabase;
	BOOL		m_bCodeTestingON;
	CString		m_strBarcode;

	void		ShowRecords();
	void		ResetListControl();
	BOOL		LogDataToCSVFile();
	CString		GetReportFileName();
	UINT		LogToExcelFile(CString strFileName);
	void		ResetFileAttributes(CString strFileName);
	void		SetHiddenAttributeToFile(CString strFileName);
	void		ReadCode();
	void		SetButtonStateForCodeScan();
	BOOL		ShowSelectedRecord(CString strPK);

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_REPORT };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_ListControl;
	afx_msg void OnBnClickedBnShowRecords();
	afx_msg void OnBnClickedBnExportToXls();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	CxShadeButton m_BnExport;
	CxShadeButton m_BnScanBarcode;
	CxShadeButton m_BnShowRecords;
	CxShadeButton m_BnCancel;
	CxShadeButton m_BnDeleteRecord;
	CxShadeButton m_BnFetchRecords;
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	afx_msg void OnBnClickedBnScanBarcode();
	afx_msg void OnBnClickedBnDeleteRecord();
	afx_msg void OnLvnItemchangedListDatabase(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedBnFetchRecords();
	CString m_strSelTransporter;
	afx_msg void OnBnClickedRadioTranNum();
	afx_msg void OnBnClickedTranIDScanBarcode();
	// Transport number to be entered manually or by scanning barcode
	int m_nEnterTransNo;
	CStatic m_StaticPkgNumber;
};
