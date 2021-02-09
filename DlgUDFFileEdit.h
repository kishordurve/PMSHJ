#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CDlgUDFFileEdit dialog

class CDlgUDFFileEdit : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgUDFFileEdit)

public:
	CDlgUDFFileEdit(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgUDFFileEdit();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_UDF_FILE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CRichEditCtrl m_CtrlUDFFileText;
	virtual void OnOK();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	CxShadeButton m_BnOK;
	CxShadeButton m_BnCancel;
	virtual BOOL OnInitDialog();
};
