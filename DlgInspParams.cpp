// DlgInspParams.cpp : implementation file
//

#include "stdafx.h"
#include "PMSHJ.h"
#include "DlgInspParams.h"
#include "afxdialogex.h"


// CDlgInspParams dialog

IMPLEMENT_DYNAMIC(CDlgInspParams, CDialogEx)

CDlgInspParams::CDlgInspParams(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DLG_INSP_PARAMS, pParent)
	, m_dMinWt(0)
	, m_nMinJobHeight(0)
	, m_nMaxJobHeight(0)
	, m_nSpotRectSize(0)
	, m_dCameraHeight(1200.0)
	, m_bFlipImageVert(FALSE)
	, m_bFlipImageHorz(FALSE)
	, m_nBarcodeLengthMin(0)
	, m_bIsCuboid(FALSE)
	, m_nDsplAccuracy(0)
	, m_nCamAlignRectSize(180)
	, m_nBarcodeLengthMax(0)
{

}

CDlgInspParams::~CDlgInspParams()
{
}

void CDlgInspParams::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_MIN_WT, m_dMinWt);
	DDV_MinMaxDouble(pDX, m_dMinWt, 0.05, 200);
	DDX_Text(pDX, IDC_EDIT_MIN_JOB_HT, m_nMinJobHeight);
	DDV_MinMaxInt(pDX, m_nMinJobHeight, 15, 500);
	DDX_Text(pDX, IDC_EDIT_MAX_JOB_HT, m_nMaxJobHeight);
	DDV_MinMaxInt(pDX, m_nMaxJobHeight, 50, 2000);
	DDX_Text(pDX, IDC_EDIT_SPOT_RECT_SIZE, m_nSpotRectSize);
	DDV_MinMaxInt(pDX, m_nSpotRectSize, 5, 100);
	DDX_Text(pDX, IDC_EDIT_CAM_HT, m_dCameraHeight);
	DDV_MinMaxDouble(pDX, m_dCameraHeight, 800.0, 6000.0);
	DDX_Check(pDX, IDC_CHK_FLIP_VERT, m_bFlipImageVert);
	DDX_Check(pDX, IDC_CHK_FLIP_HORZ, m_bFlipImageHorz);
	DDX_Text(pDX, IDC_EDIT_CODE_LEN, m_nBarcodeLengthMin);
	DDV_MinMaxInt(pDX, m_nBarcodeLengthMin, 3, 64);
	DDX_Control(pDX, IDOK, m_BnOK);
	DDX_Control(pDX, IDCANCEL, m_BnCancel);
	DDX_Check(pDX, IDC_CHK_ISCUBOID, m_bIsCuboid);
	DDX_Radio(pDX, IDC_RADIO_DSPL_ACC, m_nDsplAccuracy);
	DDV_MinMaxInt(pDX, m_nDsplAccuracy, 0, 3);
	DDX_Text(pDX, IDC_EDIT_CAM_ALIGN_RECT_SIZE, m_nCamAlignRectSize);
	DDV_MinMaxInt(pDX, m_nCamAlignRectSize, 120, 360);
	DDX_Text(pDX, IDC_EDIT_CODE_LEN_MAX, m_nBarcodeLengthMax);
	DDV_MinMaxInt(pDX, m_nBarcodeLengthMax, 3, 64);
}


BEGIN_MESSAGE_MAP(CDlgInspParams, CDialogEx)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CDlgInspParams message handlers


BOOL CDlgInspParams::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	CPMSHJApp* pApp = (CPMSHJApp*)AfxGetApp();
	CString strCaption, strToolTip;
	std::wstring str;

	str = pApp->m_pLanguage->GetString(IDS_STR_OK);
	strCaption = str.c_str();
	m_BnOK.SetFont(L"Aril", 16, 1000, 0, 0);
	m_BnOK.SetTextColor(RGB(0, 0, 255));
	m_BnOK.SetToolTipText(strCaption);
	m_BnOK.SetShade(SHS_METAL, 10, 30, 20, BTN_BACK_COLOR);
	m_BnOK.SetFlat(FALSE);
	m_BnOK.SetWindowText(strCaption);

	str = pApp->m_pLanguage->GetString(IDS_STR_CANCEL);
	strCaption = str.c_str();
	m_BnCancel.SetFont(L"Aril", 16, 1000, 0, 0);
	m_BnCancel.SetTextColor(RGB(0, 0, 255));
	m_BnCancel.SetToolTipText(strCaption);
	m_BnCancel.SetShade(SHS_METAL, 10, 30, 20, BTN_BACK_COLOR);
	m_BnCancel.SetFlat(FALSE);
	m_BnCancel.SetWindowText(strCaption);

	m_dMinWt			= pApp->m_stInspParams.dMinWt;
	m_bFlipImageVert	= pApp->m_stInspParams.bFlipImageVert;
	m_bFlipImageHorz	= pApp->m_stInspParams.bFlipImageHorz;
	m_nMinJobHeight		= pApp->m_stInspParams.nMinObjectHeight;
	m_nMaxJobHeight		= pApp->m_stInspParams.nMaxObjectHeight;
	m_dCameraHeight		= pApp->m_stInspParams.d3DCameraHeight;
	m_nBarcodeLengthMin	= pApp->m_stInspParams.nBarcodeLengthMin;
	m_nBarcodeLengthMax	= pApp->m_stInspParams.nBarcodeLengthMax;
	m_nSpotRectSize		= pApp->m_stInspParams.nSpotRectSize;
	m_bIsCuboid			= pApp->m_stInspParams.bIsCuboid;
	m_nCamAlignRectSize = pApp->m_stInspParams.nCamAlignRectSize;

	// KPD - 140520
	if (pApp->m_stInspParams.nDsplAccuracy == 1)
		m_nDsplAccuracy = 0;
	else if (pApp->m_stInspParams.nDsplAccuracy == 2)
		m_nDsplAccuracy = 1;
	else if (pApp->m_stInspParams.nDsplAccuracy == 5)
		m_nDsplAccuracy = 2;
	else if (pApp->m_stInspParams.nDsplAccuracy == 10)
		m_nDsplAccuracy = 3;
	//
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void CDlgInspParams::OnOK()
{
	if (UpdateData(TRUE))
	{
		CPMSHJApp* pApp = (CPMSHJApp*)AfxGetApp();
		if (pApp->m_stOptions.bSerialPortScanner)
		{
			if (pApp->m_pThreadCodeScanner)
				pApp->m_pThreadCodeScanner->m_nBarcodeLengthMin = m_nBarcodeLengthMin;
		}
		else if ((pApp->m_stInspParams.nBarcodeLengthMin != m_nBarcodeLengthMin) && pApp->m_stOptions.bHikVisionScanner)
		{
			if (pApp->m_pThreadHikVCode)
			{
				pApp->m_pThreadHikVCode->m_nBarcodeLengthMin = m_nBarcodeLengthMin;
			}
		}
		pApp->m_stInspParams.dMinWt				= m_dMinWt;
		pApp->m_stInspParams.bFlipImageVert		= m_bFlipImageVert;
		pApp->m_stInspParams.bFlipImageHorz		= m_bFlipImageHorz;
		pApp->m_stInspParams.nMinObjectHeight	= m_nMinJobHeight;
		pApp->m_stInspParams.nMaxObjectHeight	= m_nMaxJobHeight;
		pApp->m_stInspParams.d3DCameraHeight	= m_dCameraHeight;
		if (m_nBarcodeLengthMax < m_nBarcodeLengthMin)
			m_nBarcodeLengthMax = m_nBarcodeLengthMin;
		pApp->m_stInspParams.nBarcodeLengthMin	= m_nBarcodeLengthMin;
		pApp->m_stInspParams.nBarcodeLengthMax	= m_nBarcodeLengthMax;
		pApp->m_stInspParams.nSpotRectSize		= m_nSpotRectSize;
		pApp->m_stInspParams.bIsCuboid			= m_bIsCuboid;
		pApp->m_stInspParams.nCamAlignRectSize = m_nCamAlignRectSize;
		// KPD - 140520
		if (m_nDsplAccuracy == 0)
			pApp->m_stInspParams.nDsplAccuracy = 1;
		else if (m_nDsplAccuracy == 1)
			pApp->m_stInspParams.nDsplAccuracy = 2;
		else if (m_nDsplAccuracy == 2)
			pApp->m_stInspParams.nDsplAccuracy = 5;
		else if (m_nDsplAccuracy == 3)
			pApp->m_stInspParams.nDsplAccuracy = 10;

		pApp->WriteInspParamsToRegistry();
		CDialogEx::OnOK();
	}
}


HBRUSH CDlgInspParams::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	CPMSHJApp* pApp = (CPMSHJApp*)AfxGetApp();
	pDC->SetBkMode(TRANSPARENT);
	if ((pWnd->GetDlgCtrlID() == IDC_EDIT_CAM_HT) || (pWnd->GetDlgCtrlID() == IDC_EDIT_MIN_WT) || 
		(pWnd->GetDlgCtrlID() == IDC_EDIT_MIN_JOB_HT) || (pWnd->GetDlgCtrlID() == IDC_EDIT_MAX_JOB_HT) || 
		(pWnd->GetDlgCtrlID() == IDC_EDIT_SPOT_RECT_SIZE) || (pWnd->GetDlgCtrlID() == IDC_EDIT_CODE_LEN) || 
		(pWnd->GetDlgCtrlID() == IDC_EDIT_CODE_LEN_MAX) || (pWnd->GetDlgCtrlID() == IDC_EDIT_CAM_ALIGN_RECT_SIZE))
	{
		return pApp->m_WhiteBrush;
	}
	else
		return pApp->m_DlgBkgndBrush;
}


void CDlgInspParams::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class

	CDialogEx::OnCancel();
}
