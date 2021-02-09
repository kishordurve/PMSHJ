// DlgDataSendParams.cpp : implementation file
//

#include "stdafx.h"
#include "PMSHJ.h"
#include "DlgDataSendParams.h"
#include "afxdialogex.h"


// CDlgDataSendParams dialog

IMPLEMENT_DYNAMIC(CDlgDataSendParams, CDialogEx)

CDlgDataSendParams::CDlgDataSendParams(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DLG_DATA_SEND, pParent)
	, m_strURL(_T(""))
	, m_strKey(_T(""))
	, m_bSendData(FALSE)
{
	m_pApp = (CPMSHJApp*)AfxGetApp();
}

CDlgDataSendParams::~CDlgDataSendParams()
{
}

void CDlgDataSendParams::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_URL, m_strURL);
	DDX_Text(pDX, IDC_EDIT_KEY, m_strKey);
	DDV_MaxChars(pDX, m_strKey, 32);
	DDX_Check(pDX, IDC_CHK_SEND_DATA, m_bSendData);
}


BEGIN_MESSAGE_MAP(CDlgDataSendParams, CDialogEx)
	ON_BN_CLICKED(IDC_CHK_SEND_DATA, &CDlgDataSendParams::OnBnClickedChkSendData)
END_MESSAGE_MAP()


// CDlgDataSendParams message handlers


BOOL CDlgDataSendParams::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_strURL = m_pApp->m_stServerCommParams.strURL;
	m_strKey = m_pApp->m_stServerCommParams.strKey;
	m_bSendData = m_pApp->m_stServerCommParams.bSendDataToServer;
	SetEditControlsState(m_bSendData);

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void CDlgDataSendParams::OnOK()
{
	UpdateData(TRUE);
	m_pApp->m_stServerCommParams.strURL = m_strURL;
	m_pApp->m_stServerCommParams.strKey = m_strKey;
	m_pApp->m_stServerCommParams.bSendDataToServer = m_bSendData;

	m_pApp->WriteSendDataParamsToRegistry();

	CDialogEx::OnOK();
}


void CDlgDataSendParams::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class

	CDialogEx::OnCancel();
}


void CDlgDataSendParams::OnBnClickedChkSendData()
{
	UpdateData(TRUE);
	if (m_bSendData)
	{
		// Enable Edit Controls
		SetEditControlsState(TRUE);
	}
	else
	{
		// Disable Edit Controls
		SetEditControlsState(FALSE);
	}
}

void CDlgDataSendParams::SetEditControlsState(BOOL bEnable)
{
	if (m_bSendData)
	{
		// Enable Edit Controls
		GetDlgItem(IDC_EDIT_URL)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_KEY)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_OPERATING_MODE)->EnableWindow(TRUE);
	}
	else
	{
		// Disable Edit Controls
		GetDlgItem(IDC_EDIT_URL)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_KEY)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_OPERATING_MODE)->EnableWindow(FALSE);
	}

}
