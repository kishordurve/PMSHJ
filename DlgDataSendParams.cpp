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
	, m_bSendDataAddr1(FALSE)
	, m_strURL2(_T(""))
	, m_strKey2(_T(""))
	, m_bSendDataAddr2(FALSE)
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
	DDX_Check(pDX, IDC_CHK_SEND_DATA, m_bSendDataAddr1);
	DDX_Text(pDX, IDC_EDIT_URL2, m_strURL2);
	DDX_Text(pDX, IDC_EDIT_KEY2, m_strKey2);
	DDX_Check(pDX, IDC_CHK_ENABLE_ADDR2, m_bSendDataAddr2);
}


BEGIN_MESSAGE_MAP(CDlgDataSendParams, CDialogEx)
	ON_BN_CLICKED(IDC_CHK_SEND_DATA, &CDlgDataSendParams::OnBnClickedChkSendData)
	ON_BN_CLICKED(IDC_CHK_ENABLE_ADDR2, &CDlgDataSendParams::OnBnClickedChkEnableAddr2)
END_MESSAGE_MAP()


// CDlgDataSendParams message handlers


BOOL CDlgDataSendParams::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_strURL = m_pApp->m_stServerCommParams.strURL1;
	m_strKey = m_pApp->m_stServerCommParams.strKey1;
	m_bSendDataAddr1 = m_pApp->m_stServerCommParams.bSendDataToServer;
	SetEditControlsState(m_bSendDataAddr1);

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void CDlgDataSendParams::OnOK()
{
	UpdateData(TRUE);
	m_pApp->m_stServerCommParams.strURL1 = m_strURL;
	m_pApp->m_stServerCommParams.strKey1 = m_strKey;
	m_pApp->m_stServerCommParams.bSendDataToServer = m_bSendDataAddr1;

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
	if (m_bSendDataAddr1)
	{
		// Enable Edit Controls
		GetDlgItem(IDC_EDIT_URL)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_KEY)->EnableWindow(TRUE);
	}
	else
	{
		// Disable Edit Controls
		GetDlgItem(IDC_EDIT_URL)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_KEY)->EnableWindow(FALSE);
	}
}

void CDlgDataSendParams::SetEditControlsState(BOOL bEnable)
{
	if (m_bSendDataAddr1)
	{
		// Enable Edit Controls
		GetDlgItem(IDC_EDIT_URL)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_KEY)->EnableWindow(TRUE);
	}
	else
	{
		// Disable Edit Controls
		GetDlgItem(IDC_EDIT_URL)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_KEY)->EnableWindow(FALSE);
	}

}


void CDlgDataSendParams::OnBnClickedChkEnableAddr2()
{
	UpdateData(TRUE);
	if (m_bSendDataAddr2)
	{
		// Enable Edit Controls
		GetDlgItem(IDC_EDIT_URL2)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_KEY2)->EnableWindow(TRUE);
	}
	else
	{
		// Disable Edit Controls
		GetDlgItem(IDC_EDIT_URL2)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_KEY2)->EnableWindow(FALSE);
	}
}
