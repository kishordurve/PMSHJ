// DlgUDFFileEdit.cpp : implementation file
//

#include "stdafx.h"
#include "PMSHJ.h"
#include "DlgUDFFileEdit.h"
#include "afxdialogex.h"


// CDlgUDFFileEdit dialog

IMPLEMENT_DYNAMIC(CDlgUDFFileEdit, CDialogEx)

CDlgUDFFileEdit::CDlgUDFFileEdit(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DLG_UDF_FILE, pParent)
{

}

CDlgUDFFileEdit::~CDlgUDFFileEdit()
{
}

void CDlgUDFFileEdit::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RICHEDIT2_TXT, m_CtrlUDFFileText);
	DDX_Control(pDX, IDOK, m_BnOK);
	DDX_Control(pDX, IDCANCEL, m_BnCancel);
}


BEGIN_MESSAGE_MAP(CDlgUDFFileEdit, CDialogEx)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CDlgUDFFileEdit message handlers


void CDlgUDFFileEdit::OnOK()
{
	UpdateData(TRUE);
	int nTextLength = m_CtrlUDFFileText.GetTextLength();
	if (nTextLength == 0)
	{
		if (::MessageBoxA(this->m_hWnd, "No Text Entered. Do You Want to Quit?", "Quit?", MB_YESNO | MB_ICONEXCLAMATION) == IDYES)
			CDialogEx::OnOK();
	}
	else
	{
		CPMSHJApp* pApp = (CPMSHJApp*)AfxGetApp();
		// Save to UDF File here
		CStringArray arStrings;
		//TCHAR arChar[256];
		pApp->m_stUDFFileText.arStrings.RemoveAll();
		int nSize = m_CtrlUDFFileText.GetLineCount();
		for (int i = 0; i < m_CtrlUDFFileText.GetLineCount(); i++)
		{
			int nActualLength = 0;
			const int nLineIndex = m_CtrlUDFFileText.LineIndex(i);
			int nLineLength = m_CtrlUDFFileText.LineLength(nLineIndex);
			CString str;			
			wchar_t* buffer = str.GetBuffer(nLineLength);
			if (nLineLength > 1)
				nActualLength = m_CtrlUDFFileText.GetLine(i, buffer, nLineLength);
			// If you do not do the below calisthenics it crashes for lines with just a single character!
			else if (nLineLength <= 1)
			{
				TCHAR c[2];
				m_CtrlUDFFileText.GetLine(i, c, 2);
				str = c[0];
			}
			pApp->m_stUDFFileText.arStrings.Add(str);
			str.ReleaseBuffer();
		}
		// Get File Name from user
		static char BASED_CODE szFilter[] = "UDF Files (*.udf)| *.udf; ||";
		CString strFilter(szFilter);
		CString strFileName;
		CFileDialog FileDlg(FALSE,
			NULL,
			NULL,
			OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST,
			strFilter,
			NULL);

		if (FileDlg.DoModal() == IDOK)
		{
			strFileName = FileDlg.GetPathName();
			if (strFileName.Find(L".udf") == -1)
				strFileName += ".udf";
			pApp->m_stUDFFileText.strUDFFileNameWithPath = strFileName;
			pApp->m_pFrame->CreateUDFFile(pApp->m_stUDFFileText);
		}
		CDialogEx::OnOK();
	}
}


HBRUSH CDlgUDFFileEdit::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	CPMSHJApp* pApp = (CPMSHJApp*)AfxGetApp();
	pDC->SetBkMode(TRANSPARENT);
	if (pWnd->GetDlgCtrlID() == IDC_RICHEDIT2_TXT)
	{
		return pApp->m_WhiteBrush;
	}
	else
		return pApp->m_DlgBkgndBrush;
}


BOOL CDlgUDFFileEdit::OnInitDialog()
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

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}
