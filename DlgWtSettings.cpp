// DlgWtSettings.cpp : implementation file
//

#include "stdafx.h"
#include "PMSHJ.h"
#include "DlgWtSettings.h"
#include "afxdialogex.h"


// CDlgWtSettings dialog

IMPLEMENT_DYNAMIC(CDlgWtSettings, CDialogEx)

CDlgWtSettings::CDlgWtSettings(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DLG_WT_SETTINGS, pParent)
	, m_strPreTareWt(_T("00.00"))
{
	m_bCOMPortOpened = FALSE;
}

CDlgWtSettings::~CDlgWtSettings()
{
}

void CDlgWtSettings::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BN_SET_ZERO_WT, m_BnZeroWt);
	DDX_Control(pDX, IDC_BN_SET_TARE, m_BnTareWt);
	DDX_Control(pDX, IDC_BN_SET_PRETARE, m_BnPreTareWt);
	DDX_Control(pDX, IDOK, m_BnOK);
	DDX_Control(pDX, IDCANCEL, m_BnCancel);
	DDX_Text(pDX, IDC_EDIT_PRETARE_WT, m_strPreTareWt);
	DDV_MaxChars(pDX, m_strPreTareWt, 5);
	DDX_Text(pDX, IDC_EDIT_WT_ACC, m_nWtAccuracy);
	DDV_MinMaxInt(pDX, m_nWtAccuracy, 0, 3);
}


BEGIN_MESSAGE_MAP(CDlgWtSettings, CDialogEx)
	ON_BN_CLICKED(IDC_BN_SET_ZERO_WT, &CDlgWtSettings::OnBnClickedBnSetZeroWt)
	ON_BN_CLICKED(IDC_BN_SET_TARE, &CDlgWtSettings::OnBnClickedBnSetTare)
	ON_BN_CLICKED(IDC_BN_SET_PRETARE, &CDlgWtSettings::OnBnClickedBnSetPreTare)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CDlgWtSettings message handlers

int CDlgWtSettings::getHexVal(char c)
{
	if (c >= '0' && c <= '9')
		return c - '0';
	else if (c >= 'a' && c <= 'f')
		return c - 'a' + 10;
	else if (c >= 'A' && c <= 'F')
		return c - 'A' + 10;
	else
		return -1;//error
}

const char * CDlgWtSettings::hexToAscii(const char * hexStr)
{
	int hexLen = strlen(hexStr);

	char* asciiStr = new char(hexLen / 2 + 1);// Memory Leak!

	for (int i = 0; i < hexLen; i += 2)
	{
		asciiStr[i / 2] = (getHexVal(hexStr[i]) * 16 + getHexVal(hexStr[i + 1]));
	}
	asciiStr[hexLen / 2] = '\0';
	return asciiStr;
}

// 
CString CDlgWtSettings::DecToHexString(const int nVal)
{
	std::string str;
	int rem = 0, dec = nVal;
	while (dec > 0)   // Do this whilst the quotient is greater than 0.
	{
		rem = dec % 16; // Get the remainder.
		if (rem > 9)
		{
			// Map the character given that the remainder is greater than 9.
			switch (rem)
			{
			case 10: str = "A" + str; break;
			case 11: str = "B" + str; break;
			case 12: str = "C" + str; break;
			case 13: str = "D" + str; break;
			case 14: str = "E" + str; break;
			case 15: str = "F" + str; break;
			}
		}
		else
		{
			str = char(rem + 48) + str; // Converts integer (0-9) to ASCII code.
									// x + 48 is the ASCII code for x digit (if 0 <= x <= 9)
		}
		dec = dec / 16;
	}
	CString strHex(str.c_str());
	// Ensure expected length of string as otherwise substitution will fail in calling func
	while (strHex.GetLength() % 8 != 0)
		strHex = '0' + strHex;
	return strHex;
}

void CDlgWtSettings::OnBnClickedBnSetZeroWt()
{
	if (m_bCOMPortOpened)
	{
		HRESULT hResult;
		CStringA str = "FF31000000770D0A";
		const char data[] = "FF31000000770D0A";
		const char* m_pccharSendAscii = hexToAscii(data);
		hResult = m_pSerialComm->Write(m_pccharSendAscii, (str.GetLength()) /2);
		if (hResult == S_OK)
			AfxMessageBox(L"Zero Setting Succesful!");
	}
}

void CDlgWtSettings::OnBnClickedBnSetTare()
{
	if (m_bCOMPortOpened)
	{
		HRESULT hResult;
		CStringA str = "FF32000000770D0A";
		const char data[] = "FF32000000770D0A";
		const char* m_pccharSendAscii = hexToAscii(data);
		hResult = m_pSerialComm->Write(m_pccharSendAscii, (str.GetLength()) / 2);
		if (hResult == S_OK)
			AfxMessageBox(L"Tare Setting Succesful!");
	}
}

// Example: 
// Weight Spec: 60.00kg / e=0.01kg
// PreTare 5.00kg - Need to input 500 -		FF 30 00 00 01 F4 0D 0A - (00 00 01 F4 is 500)
// PreTare 0.05kg - Need to input 5 -		FF 30 00 00 00 05 0D 0A - (00 00 00 05 is 5)
// PreTare 12.25kg - Need to input 1225 -	FF 30 00 00 04 C9 0D 0A - (00 00 04 C9 is 1225)
// 
void CDlgWtSettings::OnBnClickedBnSetPreTare()
{
	if (UpdateData(TRUE))
	{
		CPMSHJApp* pApp = (CPMSHJApp*)AfxGetApp();
		// The multiplying factor should be dependent on weight format. 100 for xx.xx, 1000 for xx.xxx
		double dMultFactor = pow(10.0, pApp->m_stOptions.nWtAccuracy);
		int nVal = _wtof(m_strPreTareWt.GetBuffer()) * dMultFactor;
		// Get the number down to 4 digits
		while (nVal > 10000)
			nVal /= 10;
		CString hexadecimalNumber = DecToHexString(nVal);
		// Ensure it is only a 8-character number (00000000)
		if (hexadecimalNumber.GetLength() > 8)
		{
			AfxMessageBox(L"PreTare Weight incorrect!");
			return;
		}
		// Convert decimal to hex
		if (m_bCOMPortOpened)
		{
			HRESULT hResult;
			CStringA str("FF30000000000D0A");
			// Replace the central 8 digits with hexadecimalNumber
			for (int i = 0; i < hexadecimalNumber.GetLength(); i++)
				str.SetAt(4 + i, hexadecimalNumber[i]);
			const char* m_pccharSendAscii = hexToAscii(str.GetBuffer());
			hResult = m_pSerialComm->Write(m_pccharSendAscii, str.GetLength() / 2);
			if (hResult == S_OK)
				AfxMessageBox(L"PreTare Setting Succesful!");
		}
		CDialogEx::OnOK();
	}
}

BOOL CDlgWtSettings::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CPMSHJApp* pApp = (CPMSHJApp*)AfxGetApp();
	m_BnZeroWt.SetFont(L"Aril", 16, 1000, 0, 0);
	m_BnZeroWt.SetTextColor(RGB(0, 0, 255));
	m_BnZeroWt.SetToolTipText(L"Set Zero Wt After Emptying Platform.");
	m_BnZeroWt.SetShade(SHS_METAL, 10, 30, 20, BTN_BACK_COLOR);
	m_BnZeroWt.SetFlat(FALSE);
	m_BnZeroWt.SetWindowText(L"Set Zero Wt");

	m_BnTareWt.SetFont(L"Aril", 16, 1000, 0, 0);
	m_BnTareWt.SetTextColor(RGB(0, 0, 255));
	m_BnTareWt.SetToolTipText(L"Set Tare Wt.");
	m_BnTareWt.SetShade(SHS_METAL, 10, 30, 20, BTN_BACK_COLOR);
	m_BnTareWt.SetFlat(FALSE);
	m_BnTareWt.SetWindowText(L"Set Tare Wt");

	m_BnPreTareWt.SetFont(L"Aril", 16, 1000, 0, 0);
	m_BnPreTareWt.SetTextColor(RGB(0, 0, 255));
	m_BnPreTareWt.SetToolTipText(L"Set PreTare Wt.");
	m_BnPreTareWt.SetShade(SHS_METAL, 10, 30, 20, BTN_BACK_COLOR);
	m_BnPreTareWt.SetFlat(FALSE);
	m_BnPreTareWt.SetWindowText(L"Set PreTare Wt");

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

	// Open Weigh Scale Input COM port here
	m_pSerialComm = new CSerialCommHelper;
	CString strIPCOMPort;
	strIPCOMPort.Format(L"COM%d", pApp->m_stWeighScaleIPParams.nComPortNum);
	std::string strCOMPort(CT2A(strIPCOMPort.GetBuffer()));
	strIPCOMPort.Format(L"COM%d", pApp->m_stWeighScaleIPParams.nComPortNum);
	HRESULT hResult = m_pSerialComm->Init(strCOMPort, 9600, 0, 1, 8);// Parity = 0, Stop Bit = 1, Byte size = 8
	if (hResult != E_FAIL)
	{
		hResult = m_pSerialComm->Start();
		if (hResult != E_FAIL)
		{
			m_bCOMPortOpened = TRUE;
		}
	}
	m_nWtAccuracy = pApp->m_stOptions.nWtAccuracy;
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgWtSettings::OnOK()
{
	if (m_bCOMPortOpened)
	{
		HRESULT hResult;
		if (m_pSerialComm && m_pSerialComm->IsConnection())
			hResult = m_pSerialComm->Stop();
		if (hResult == S_OK)
			hResult = m_pSerialComm->UnInit();
		m_pSerialComm->DelLock();
	}
	UpdateData(TRUE);
	CPMSHJApp* pApp = (CPMSHJApp*)AfxGetApp();
	pApp->m_stOptions.nWtAccuracy = m_nWtAccuracy;
	pApp->WriteOptionsToRegistry();

	CDialogEx::OnOK();
}

void CDlgWtSettings::OnCancel()
{
	if (m_bCOMPortOpened)
	{
		HRESULT hResult;
		if (m_pSerialComm && m_pSerialComm->IsConnection())
			hResult = m_pSerialComm->Stop();
		if (hResult == S_OK)
			hResult = m_pSerialComm->UnInit();
		m_pSerialComm->DelLock();
	}

	CDialogEx::OnCancel();
}


HBRUSH CDlgWtSettings::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	CPMSHJApp* pApp = (CPMSHJApp*)AfxGetApp();
	pDC->SetBkMode(TRANSPARENT);
	if ((pWnd->GetDlgCtrlID() == IDC_EDIT_PRETARE_WT) || (pWnd->GetDlgCtrlID() == IDC_EDIT_WT_ACC))
	{
		return pApp->m_WhiteBrush;
	}
	else
		return pApp->m_DlgBkgndBrush;
}

