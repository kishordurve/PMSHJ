#pragma once


// CDlgCalib dialog

class CDlgCalib : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgCalib)

public:
	CDlgCalib(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgCalib();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_CALIB };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	int m_nCalibType;
};
