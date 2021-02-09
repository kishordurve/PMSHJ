// DlgFreightParams.cpp : implementation file
//

#include "stdafx.h"
#include "PMSHJ.h"
#include "DlgFreightParams.h"
#include "afxdialogex.h"


// CDlgFreightParams dialog

IMPLEMENT_DYNAMIC(CDlgFreightParams, CDialogEx)

CDlgFreightParams::CDlgFreightParams(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DLG_FREIGHT_PARAMS, pParent)
	, m_strSeaFreightRate(_T(""))
	, m_strAirFreightRate(_T(""))
	, m_strWeightFreightRate(_T(""))
{

}

CDlgFreightParams::~CDlgFreightParams()
{
}

void CDlgFreightParams::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_SEAFREIGHT_RATE, m_strSeaFreightRate);
	DDV_MaxChars(pDX, m_strSeaFreightRate, 10);
	DDX_Text(pDX, IDC_EDIT_AIRFREIGHT_RATE, m_strAirFreightRate);
	DDV_MaxChars(pDX, m_strAirFreightRate, 10);
	DDX_Text(pDX, IDC_EDIT_WEIGHTFREIGHT_RATE, m_strWeightFreightRate);
	DDV_MaxChars(pDX, m_strWeightFreightRate, 10);
}


BEGIN_MESSAGE_MAP(CDlgFreightParams, CDialogEx)
END_MESSAGE_MAP()


// CDlgFreightParams message handlers


BOOL CDlgFreightParams::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CPMSHJApp* pApp = (CPMSHJApp*)AfxGetApp();
	m_strSeaFreightRate.Format(L"%2.3f", pApp->m_stFreightParams.dSeaFreightRate);
	m_strAirFreightRate.Format(L"%2.3f", pApp->m_stFreightParams.dAirFreightRate);
	m_strWeightFreightRate.Format(L"%2.3f", pApp->m_stFreightParams.dWeightFreightRate);
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void CDlgFreightParams::OnOK()
{
	UpdateData(TRUE);
	CPMSHJApp* pApp = (CPMSHJApp*)AfxGetApp();
	pApp->m_stFreightParams.dSeaFreightRate = _wtof(m_strSeaFreightRate);
	pApp->m_stFreightParams.dAirFreightRate = _wtof(m_strAirFreightRate);
	pApp->m_stFreightParams.dWeightFreightRate = _wtof(m_strWeightFreightRate);
	pApp->WriteFreightParamsToRegistry();

	CDialogEx::OnOK();
}
