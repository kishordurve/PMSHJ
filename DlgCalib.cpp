// DlgCalib.cpp : implementation file
//

#include "stdafx.h"
#include "PMSHJ.h"
#include "DlgCalib.h"
#include "afxdialogex.h"


// CDlgCalib dialog

IMPLEMENT_DYNAMIC(CDlgCalib, CDialogEx)

CDlgCalib::CDlgCalib(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DLG_CALIB, pParent)
	, m_nCalibType(0)
{

}

CDlgCalib::~CDlgCalib()
{
}

void CDlgCalib::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_RADIO_CALIB_CAMHEIGHT, m_nCalibType);
	DDV_MinMaxInt(pDX, m_nCalibType, 0, 1);
}


BEGIN_MESSAGE_MAP(CDlgCalib, CDialogEx)
END_MESSAGE_MAP()


// CDlgCalib message handlers
