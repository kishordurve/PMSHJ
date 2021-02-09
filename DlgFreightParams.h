#pragma once


// CDlgFreightParams dialog

class CDlgFreightParams : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgFreightParams)

public:
	CDlgFreightParams(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgFreightParams();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_FREIGHT_PARAMS	};
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_strSeaFreightRate;
	CString m_strAirFreightRate;
	CString m_strWeightFreightRate;
	virtual BOOL OnInitDialog();
	virtual void OnOK();
};
