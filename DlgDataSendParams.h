#pragma once


// CDlgDataSendParams dialog

class CDlgDataSendParams : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgDataSendParams)

public:
	CDlgDataSendParams(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgDataSendParams();
	CPMSHJApp* m_pApp;
	void SetEditControlsState(BOOL bEnable);
// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_DATA_SEND	};
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_strURL;
	CString m_strKey;
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnBnClickedChkSendData();
	BOOL m_bSendData;
};
