// DlgPassword.cpp : implementation file
//

#include "stdafx.h"
#include "PMSHJ.h"
#include "DlgPassword.h"
#include "afxdialogex.h"


// CDlgPassword dialog

IMPLEMENT_DYNAMIC(CDlgPassword, CDialogEx)

CDlgPassword::CDlgPassword(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG1, pParent)
	, m_strPWD(_T(""))
{

}

CDlgPassword::~CDlgPassword()
{
}

void CDlgPassword::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_PWD, m_strPWD);
	DDV_MaxChars(pDX, m_strPWD, 16);
}


BEGIN_MESSAGE_MAP(CDlgPassword, CDialogEx)
END_MESSAGE_MAP()


// CDlgPassword message handlers
