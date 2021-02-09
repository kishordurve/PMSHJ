#pragma once
#include "afxwin.h"


// CDlgInspParams dialog

class CDlgInspParams : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgInspParams)

public:
	CDlgInspParams(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgInspParams();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_INSP_PARAMS };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	double m_dMinWt;
	int m_nMinJobHeight;
	int m_nMaxJobHeight;
	int m_nSpotRectSize;
	double m_dCameraHeight;
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	BOOL m_bFlipImageVert;
	BOOL m_bFlipImageHorz;
	int m_nBarcodeLengthMin;
	int m_nBarcodeLengthMax;
	CxShadeButton m_BnOK;
	CxShadeButton m_BnCancel;
	BOOL m_bIsCuboid;
	virtual void OnCancel();
	// This the display accuracy introduced to show minimum variation in displayed readings
	int m_nDsplAccuracy;
	int m_nCamAlignRectSize;
};
