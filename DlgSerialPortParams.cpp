// DlgSerialPortParams.cpp : implementation file
//

#include "stdafx.h"
#include "PMSHJ.h"
#include "DlgSerialPortParams.h"
#include "afxdialogex.h"


// CDlgSerialPortParams dialog

IMPLEMENT_DYNAMIC(CDlgSerialPortParams, CDialogEx)

CDlgSerialPortParams::CDlgSerialPortParams(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DLG_SERIAL_COMM, pParent)
	, m_nWeighScaleType(0)
	, m_nDataTransMode(0)
	, m_nWeighScalePort(1)// Selecting Output port to start with
{

}

CDlgSerialPortParams::~CDlgSerialPortParams()
{
}

void CDlgSerialPortParams::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_COM_PORTS, m_listPorts);
	DDX_Control(pDX, IDC_CMB_BAUD_RATE, m_CmbBaudRate);
	DDX_Radio(pDX, IDC_RADIO_WEIGH_SCALE_TYPR, m_nWeighScaleType);
	DDV_MinMaxInt(pDX, m_nWeighScaleType, 0, 4);
	DDX_Radio(pDX, IDC_RADIO_DATA_TRANS_TYPE, m_nDataTransMode);
	DDV_MinMaxInt(pDX, m_nDataTransMode, 0, 1);
	DDX_Control(pDX, IDC_CMB_BYTE_SIZE, m_CmbByteSize);
	DDX_Control(pDX, IDC_CMB_PARITY, m_CmbParity);
	DDX_Control(pDX, IDC_CMB_STOP_BIT, m_CmbStopBit);
	DDX_Radio(pDX, IDC_RADIO_WEIGH_CALE_INPUT, m_nWeighScalePort);
	DDV_MinMaxInt(pDX, m_nWeighScalePort, 0, 4);
	DDX_Control(pDX, IDOK, m_BnOK);
	DDX_Control(pDX, IDCANCEL, m_BnCancel);
}


BEGIN_MESSAGE_MAP(CDlgSerialPortParams, CDialogEx)
	ON_BN_CLICKED(IDC_BN_SET_COM_PARAMS, &CDlgSerialPortParams::OnBnClickedBnSetWeighScaleComPortParams)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_RADIO_WEIGH_CALE_INPUT, &CDlgSerialPortParams::OnBnClickedRadioWeighScaleInput)
	ON_BN_CLICKED(IDC_RADIO3, &CDlgSerialPortParams::OnBnClickedRadio3)
	ON_BN_CLICKED(IDC_RADIO_PRINTER, &CDlgSerialPortParams::OnBnClickedRadioPrinter)
	ON_BN_CLICKED(IDC_RADIO6, &CDlgSerialPortParams::OnBnClickedScannerPort)
	ON_BN_CLICKED(IDC_RADIO7, &CDlgSerialPortParams::OnBnClickedFootSwitchPort)
END_MESSAGE_MAP()


// CDlgSerialPortParams message handlers


BOOL CDlgSerialPortParams::OnInitDialog()
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

	m_listPorts.SetOnlyPresent(0);
	m_listPorts.InitList(0);// COM port list is updated here!

	m_CmbBaudRate.AddString(L"4800");
	m_CmbBaudRate.AddString(L"9600");
	m_CmbBaudRate.AddString(L"2400");
	m_CmbBaudRate.AddString(L"115200");
	m_CmbBaudRate.SetCurSel(0);

	m_CmbByteSize.AddString(L"5");
	m_CmbByteSize.AddString(L"6");
	m_CmbByteSize.AddString(L"7");
	m_CmbByteSize.AddString(L"8");
	m_CmbByteSize.AddString(L"9");
	m_CmbByteSize.SetCurSel(3);

	m_CmbParity.AddString(L"None");
	m_CmbParity.AddString(L"Odd");
	m_CmbParity.AddString(L"Even");
	m_CmbParity.AddString(L"Mark");
	m_CmbParity.AddString(L"Space");
	m_CmbParity.SetCurSel(0);

	m_CmbStopBit.AddString(L"0");
	m_CmbStopBit.AddString(L"1");
	m_CmbStopBit.AddString(L"2");
	m_CmbStopBit.SetCurSel(0);

	m_strCOMPort.Format(L"COM%d",pApp->m_stWeighScaleOPParams.nComPortNum);
	m_strBaudRate.Format(L"%d", pApp->m_stWeighScaleOPParams.nBaudRate);
	m_strByteSize.Format(L"%d", pApp->m_stWeighScaleOPParams.nByteSize);
	m_strParity.Format(L"%d", pApp->m_stWeighScaleOPParams.nParity);
	m_strStopBit.Format(L"%d", pApp->m_stWeighScaleOPParams.nStopBit);

	for (int i = 0; i < m_listPorts.GetCount(); i++)
	{
		CString str;
		m_listPorts.GetFileName(str);
		m_listPorts.GetText(i, str);
		if (str.Find(m_strCOMPort) != -1)
		{ 
			m_listPorts.SetCurSel(i);
			break;
		}
	}
	SetCmbSelections(m_strBaudRate, m_strByteSize, m_strParity, m_strStopBit);
	/*for (int i = 0; i < m_CmbBaudRate.GetCount(); i++)
	{
		CString str;
		m_CmbBaudRate.GetLBText(i, str);
		if (m_strBaudRate == str)
		{
			m_CmbBaudRate.SetCurSel(i);
			break;
		}
	}
	for (int i = 0; i < m_CmbByteSize.GetCount(); i++)
	{
		CString str;
		m_CmbByteSize.GetLBText(i, str);
		if (m_strByteSize == str)
		{
			m_CmbByteSize.SetCurSel(i);
			break;
		}
	}
	for (int i = 0; i < m_CmbParity.GetCount(); i++)
	{
		CString str;
		m_CmbParity.GetLBText(i, str);
		if (m_strParity == str)
		{
			m_CmbParity.SetCurSel(i);
			break;
		}
	}
	for (int i = 0; i < m_CmbStopBit.GetCount(); i++)
	{
		CString str;
		m_CmbStopBit.GetLBText(i, str);
		if (m_strStopBit == str)
		{
			m_CmbStopBit.SetCurSel(i);
			break;
		}
	}*/

	m_nWeighScaleType = pApp->m_stWeighScaleParams.WeighScaleType;
	m_nDataTransMode = pApp->m_stWeighScaleParams.WeighScaleMode;

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgSerialPortParams::SetCmbSelections(CString strBaudRate, CString strByteSize, CString strParity, CString strStopBit)
{
	for (int i = 0; i < m_CmbBaudRate.GetCount(); i++)
	{
		CString str;
		m_CmbBaudRate.GetLBText(i, str);
		if (strBaudRate == str)
		{
			m_CmbBaudRate.SetCurSel(i);
			break;
		}
	}
	for (int i = 0; i < m_CmbByteSize.GetCount(); i++)
	{
		CString str;
		m_CmbByteSize.GetLBText(i, str);
		if (strByteSize == str)
		{
			m_CmbByteSize.SetCurSel(i);
			break;
		}
	}
	for (int i = 0; i < m_CmbParity.GetCount(); i++)
	{
		CString str;
		m_CmbParity.GetLBText(i, str);
		if (strParity == str)
		{
			m_CmbParity.SetCurSel(i);
			break;
		}
	}
	for (int i = 0; i < m_CmbStopBit.GetCount(); i++)
	{
		CString str;
		m_CmbStopBit.GetLBText(i, str);
		if (strStopBit == str)
		{
			m_CmbStopBit.SetCurSel(i);
			break;
		}
	}
} // SetCmbSelections()

void CDlgSerialPortParams::OnOK()
{
	UpdateData(TRUE); 
	CPMSHJApp* pApp = (CPMSHJApp*)AfxGetApp();
	pApp->m_stWeighScaleParams.WeighScaleType = (eWeighScaleType)m_nWeighScaleType;
	pApp->m_stWeighScaleOPParams.nByteSize = 8;
	pApp->WriteCOMPortParamsToRegistry();
	
	CDialogEx::OnOK();
}


// When user selects a COM port after seleccting either Input or Output
void CDlgSerialPortParams::OnBnClickedBnSetWeighScaleComPortParams()
{
	UpdateData(TRUE);
	CPMSHJApp* pApp = (CPMSHJApp*)AfxGetApp();
	if (m_nWeighScalePort == 0)// Input to weigh scale
	{
		m_CmbBaudRate.GetLBText(m_CmbBaudRate.GetCurSel(), m_strBaudRate);
		m_listPorts.GetText(m_listPorts.GetCurSel(), m_strCOMPort);
		CString str;
		for (int i = 1; i < 10; i++)
		{
			str.Format(L"COM%d", i);
			if (m_strCOMPort.Find(str) != -1)
			{
				pApp->m_stWeighScaleIPParams.nComPortNum = i;
				break;
			}
		}
		char dst[256];
		wcstombs(dst, m_strBaudRate.GetBuffer(), m_strBaudRate.GetLength() + 1);
		pApp->m_stWeighScaleIPParams.nBaudRate = atoi(dst);
		wcstombs(dst, m_strByteSize.GetBuffer(), m_strByteSize.GetLength() + 1);
		pApp->m_stWeighScaleIPParams.nByteSize = atoi(dst);
		wcstombs(dst, m_strParity.GetBuffer(), m_strParity.GetLength() + 1);
		pApp->m_stWeighScaleIPParams.nParity = 0;// atoi(dst);
		wcstombs(dst, m_strStopBit.GetBuffer(), m_strStopBit.GetLength() + 1);
		pApp->m_stWeighScaleIPParams.nStopBit = atoi(dst);
	}
	else if (m_nWeighScalePort == 1)// Output from weigh scale
	{
		m_CmbBaudRate.GetLBText(m_CmbBaudRate.GetCurSel(), m_strBaudRate);
		m_listPorts.GetText(m_listPorts.GetCurSel(), m_strCOMPort);
		CString str;
		for (int i = 1; i < 10; i++)
		{
			str.Format(L"COM%d", i);
			if (m_strCOMPort.Find(str) != -1)
			{
				pApp->m_stWeighScaleOPParams.nComPortNum = i;
				break;
			}
		}
		char dst[256];
		wcstombs(dst, m_strBaudRate.GetBuffer(), m_strBaudRate.GetLength() + 1);
		pApp->m_stWeighScaleOPParams.nBaudRate = atoi(dst);
		wcstombs(dst, m_strByteSize.GetBuffer(), m_strByteSize.GetLength() + 1);
		pApp->m_stWeighScaleOPParams.nByteSize = atoi(dst);
		wcstombs(dst, m_strParity.GetBuffer(), m_strParity.GetLength() + 1);
		pApp->m_stWeighScaleOPParams.nParity = 0;// atoi(dst);
		wcstombs(dst, m_strStopBit.GetBuffer(), m_strStopBit.GetLength() + 1);
		pApp->m_stWeighScaleOPParams.nStopBit = atoi(dst);
	}
	else if (m_nWeighScalePort == 2)// Printer Port
	{
		m_CmbBaudRate.GetLBText(m_CmbBaudRate.GetCurSel(), m_strBaudRate);
		m_listPorts.GetText(m_listPorts.GetCurSel(), m_strCOMPort);
		CString str;
		for (int i = 1; i < 10; i++)
		{
			str.Format(L"COM%d", i);
			if (m_strCOMPort.Find(str) != -1)
			{
				pApp->m_stPrinterPortParams.nComPortNum = i;
				break;
			}
		}
		char dst[256];
		wcstombs(dst, m_strBaudRate.GetBuffer(), m_strBaudRate.GetLength() + 1);
		pApp->m_stPrinterPortParams.nBaudRate = atoi(dst);
		wcstombs(dst, m_strByteSize.GetBuffer(), m_strByteSize.GetLength() + 1);
		pApp->m_stPrinterPortParams.nByteSize = atoi(dst);
		wcstombs(dst, m_strParity.GetBuffer(), m_strParity.GetLength() + 1);
		pApp->m_stPrinterPortParams.nParity = 0;// atoi(dst);
		wcstombs(dst, m_strStopBit.GetBuffer(), m_strStopBit.GetLength() + 1);
		pApp->m_stPrinterPortParams.nStopBit = atoi(dst);
	}
	else if (m_nWeighScalePort == 3)// Barcode Scanner Port
	{
		m_CmbBaudRate.GetLBText(m_CmbBaudRate.GetCurSel(), m_strBaudRate);
		m_listPorts.GetText(m_listPorts.GetCurSel(), m_strCOMPort);
		CString str;
		for (int i = 1; i < 10; i++)
		{
			str.Format(L"COM%d", i);
			if (m_strCOMPort.Find(str) != -1)
			{
				pApp->m_stScannerPortParams.nComPortNum = i;
				break;
			}
		}
		char dst[256];
		wcstombs(dst, m_strBaudRate.GetBuffer(), m_strBaudRate.GetLength() + 1);
		pApp->m_stScannerPortParams.nBaudRate = atoi(dst);
		wcstombs(dst, m_strByteSize.GetBuffer(), m_strByteSize.GetLength() + 1);
		pApp->m_stScannerPortParams.nByteSize = atoi(dst);
		wcstombs(dst, m_strParity.GetBuffer(), m_strParity.GetLength() + 1);
		pApp->m_stScannerPortParams.nParity = 0;// atoi(dst);
		wcstombs(dst, m_strStopBit.GetBuffer(), m_strStopBit.GetLength() + 1);
		pApp->m_stScannerPortParams.nStopBit = atoi(dst);
	}
	else if (m_nWeighScalePort == 4)// Foot Switch Port
	{
		m_CmbBaudRate.GetLBText(m_CmbBaudRate.GetCurSel(), m_strBaudRate);
		m_listPorts.GetText(m_listPorts.GetCurSel(), m_strCOMPort);
		CString str;
		for (int i = 1; i < 10; i++)
		{
			str.Format(L"COM%d", i);
			if (m_strCOMPort.Find(str) != -1)
			{
				pApp->m_stFootSwitchPortParams.nComPortNum = i;
				break;
			}
		}
		char dst[256];
		wcstombs(dst, m_strBaudRate.GetBuffer(), m_strBaudRate.GetLength() + 1);
		pApp->m_stFootSwitchPortParams.nBaudRate = atoi(dst);
		wcstombs(dst, m_strByteSize.GetBuffer(), m_strByteSize.GetLength() + 1);
		pApp->m_stFootSwitchPortParams.nByteSize = atoi(dst);
		wcstombs(dst, m_strParity.GetBuffer(), m_strParity.GetLength() + 1);
		pApp->m_stFootSwitchPortParams.nParity = 0;// atoi(dst);
		wcstombs(dst, m_strStopBit.GetBuffer(), m_strStopBit.GetLength() + 1);
		pApp->m_stFootSwitchPortParams.nStopBit = atoi(dst);
	}

	pApp->m_stWeighScaleParams.WeighScaleType = (eWeighScaleType)m_nWeighScaleType;
	pApp->m_stWeighScaleParams.WeighScaleMode = (eWeighScaleMode)m_nDataTransMode;
} // OnBnClickedBnSetWeighScaleComPortParams()


HBRUSH CDlgSerialPortParams::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	CPMSHJApp* pApp = (CPMSHJApp*)AfxGetApp();
	pDC->SetBkMode(TRANSPARENT);
	if ((pWnd->GetDlgCtrlID() == IDC_LIST_COM_PORTS) ||  
		(pWnd->GetDlgCtrlID() == IDC_CMB_BYTE_SIZE) || (pWnd->GetDlgCtrlID() == IDC_CMB_PARITY) || (pWnd->GetDlgCtrlID() == IDC_CMB_STOP_BIT))
	{
		return pApp->m_WhiteBrush;
	}
	else if (pWnd->GetDlgCtrlID() == IDC_CMB_BAUD_RATE)
		return pApp->m_WhiteBrush;
	else
		return pApp->m_DlgBkgndBrush;
}

//
// This function forces weigh scale input port number on pApp->m_stWeighScaleIPParams.nComPortNum 
//
void CDlgSerialPortParams::OnBnClickedRadioWeighScaleInput()
{
	CPMSHJApp* pApp = (CPMSHJApp*)AfxGetApp();
	int nCurPortNum = pApp->m_stWeighScaleIPParams.nComPortNum;
	BOOL bPortNumFound = FALSE;
	CString str, str1, str2;
	str.Format(L"Silicon Labs Dual");
	int nNumAvailablePorts = m_listPorts.GetCount();
	for (int i = 1; i < nNumAvailablePorts; i++)
	{
		m_listPorts.GetText(i, m_strCOMPort);
		if (m_strCOMPort.Find(str) != -1)
		{
			str1.Format(L"Standard COM Port");
			if (m_strCOMPort.Find(str1) != -1)
			{
				str2.Format(L"COM%d", nCurPortNum);
				if (m_strCOMPort.Find(str2) != -1)
				{
					bPortNumFound = TRUE;
					m_listPorts.SetCurSel(i);
					break;
				}
			}
		}
	}
	if (!bPortNumFound)// If port number not found set to first port
	{
		AfxMessageBox(L"Current Weigh Scale Input Port number not found!");
		if (nNumAvailablePorts > 0)
			m_listPorts.SetCurSel(0);
	}
	CString strBaudRate, strByteSize, strParity, strStopBit;
	strBaudRate.Format(L"%d", pApp->m_stWeighScaleIPParams.nBaudRate);
	strByteSize.Format(L"%d", pApp->m_stWeighScaleIPParams.nByteSize);
	strParity.Format(L"%d", pApp->m_stWeighScaleIPParams.nParity);
	strStopBit.Format(L"%d", pApp->m_stWeighScaleIPParams.nStopBit);
	SetCmbSelections(strBaudRate, strByteSize, strParity, strStopBit);
} // OnBnClickedRadioWeighScaleInput()

//
// This function forces weigh scale output port number on pApp->m_stWeighScaleOPParams.nComPortNum 
//
void CDlgSerialPortParams::OnBnClickedRadio3()
{
	CPMSHJApp* pApp = (CPMSHJApp*)AfxGetApp();
	int nCurPortNum = pApp->m_stWeighScaleOPParams.nComPortNum;
	BOOL bPortNumFound = FALSE;
	CString str, str1, str2;
	str.Format(L"Silicon Labs Dual");
	int nNumAvailablePorts = m_listPorts.GetCount();
	for (int i = 1; i < nNumAvailablePorts; i++)
	{
		m_listPorts.GetText(i, m_strCOMPort);
		if (m_strCOMPort.Find(str) != -1)
		{
			str1.Format(L"Enhanced COM Port");
			if (m_strCOMPort.Find(str1) != -1)
			{
				str2.Format(L"COM%d", nCurPortNum);
				if (m_strCOMPort.Find(str2) != -1)
				{
					bPortNumFound = TRUE;
					m_listPorts.SetCurSel(i);
					break;
				}
			}
		}
	}
	if (!bPortNumFound)// If port number not found set to first port
	{
		AfxMessageBox(L"Current Weigh Scale Input Port number not found!");
		if (nNumAvailablePorts > 0)
			m_listPorts.SetCurSel(0);
	}
	CString strBaudRate, strByteSize, strParity, strStopBit;
	strBaudRate.Format(L"%d", pApp->m_stWeighScaleOPParams.nBaudRate);
	strByteSize.Format(L"%d", pApp->m_stWeighScaleOPParams.nByteSize);
	strParity.Format(L"%d", pApp->m_stWeighScaleOPParams.nParity);
	strStopBit.Format(L"%d", pApp->m_stWeighScaleOPParams.nStopBit);
	SetCmbSelections(strBaudRate, strByteSize, strParity, strStopBit);
} // OnBnClickedRadio3()

void CDlgSerialPortParams::OnBnClickedRadioPrinter()
{
	CPMSHJApp* pApp = (CPMSHJApp*)AfxGetApp();
	CString str, strPrinterPort;
	strPrinterPort.Format(L"COM%d", pApp->m_stPrinterPortParams.nComPortNum);
	BOOL bPortNumFound = FALSE;
	int nNumAvailablePorts = m_listPorts.GetCount();
	for (int i = 1; i < nNumAvailablePorts; i++)
	{
		m_listPorts.GetText(i, m_strCOMPort);
		if (m_strCOMPort.Find(strPrinterPort) != -1)
		{
			m_listPorts.SetCurSel(i);
			bPortNumFound = TRUE;
			break;
		}
	}
	if (!bPortNumFound)// Current printer port number not found! Try to set current list box sel as printer port
	{
		m_listPorts.GetText(m_listPorts.GetCurSel(), str);
		int nCurIndex = m_listPorts.GetCurSel();
		if (::MessageBox(NULL, L"Current printer port not found! Do you want to set selected COM port as printer port?", L"Printer Port Not Found", MB_YESNO) == IDYES)
		{
			if (str.Find(L"Silicon Labs") == -1)
			{
				// Get currently selected COM port number
				for (int i = 1; i < 100; i++)
				{
					strPrinterPort.Format(L"COM%d", i);
					if (str.Find(strPrinterPort) != -1)
					{
						pApp->m_stPrinterPortParams.nComPortNum = i;
						bPortNumFound = TRUE;
						break;
					}
				}
			}
			else
			{
				AfxMessageBox(L"Cannot select this port for printer. Select another port and retry.");
				m_nWeighScalePort = 0;
				UpdateData(FALSE);
				OnBnClickedRadioWeighScaleInput();
				return;
			}
		}
		else
		{
			m_nWeighScalePort = 0;
			UpdateData(FALSE);
			OnBnClickedRadioWeighScaleInput();
			return;
		}
	}
	if (bPortNumFound)
	{
		CString strBaudRate, strByteSize, strParity, strStopBit;
		strBaudRate.Format(L"%d", pApp->m_stPrinterPortParams.nBaudRate);
		strByteSize.Format(L"%d", pApp->m_stPrinterPortParams.nByteSize);
		strParity.Format(L"%d", pApp->m_stPrinterPortParams.nParity);
		strStopBit.Format(L"%d", pApp->m_stPrinterPortParams.nStopBit);
		SetCmbSelections(strBaudRate, strByteSize, strParity, strStopBit);
	}
	else
	{
		AfxMessageBox(L"Current Printer Port Number Not Found.\nSelect another from list of available ports.");
		m_nWeighScalePort = 0;
		UpdateData(FALSE);
		OnBnClickedRadioWeighScaleInput();
	}
} // OnBnClickedRadioPrinter()


void CDlgSerialPortParams::OnBnClickedScannerPort()
{
	CPMSHJApp* pApp = (CPMSHJApp*)AfxGetApp();
	CString str, strScannerPort;
	strScannerPort.Format(L"COM%d", pApp->m_stScannerPortParams.nComPortNum);
	BOOL bPortNumFound = FALSE;
	int nNumAvailablePorts = m_listPorts.GetCount();
	for (int i = 1; i < nNumAvailablePorts; i++)
	{
		m_listPorts.GetText(i, m_strCOMPort);
		if (m_strCOMPort.Find(strScannerPort) != -1)
		{
			m_listPorts.SetCurSel(i);
			bPortNumFound = TRUE;
			break;
		}
	}
	if (!bPortNumFound)// Current scanner port number not found! Try to set current list box sel as scanner port
	{
		m_listPorts.GetText(m_listPorts.GetCurSel(), str);
		int nCurIndex = m_listPorts.GetCurSel();
		if (::MessageBox(NULL, L"Current Scanner port not found! Do you want to set selected COM port as printer port?", L"Scanner Port Not Found", MB_YESNO) == IDYES)
		{
			if (str.Find(L"Silicon Labs") == -1)
			{
				// Get currently selected COM port number
				for (int i = 1; i < 100; i++)
				{
					strScannerPort.Format(L"COM%d", i);
					if (str.Find(strScannerPort) != -1)
					{
						pApp->m_stScannerPortParams.nComPortNum = i;
						bPortNumFound = TRUE;
						break;
					}
				}
			}
			else
			{
				AfxMessageBox(L"Cannot select this port for Scanner. Select another port and retry.");
				m_nWeighScalePort = 0;
				UpdateData(FALSE);
				OnBnClickedRadioWeighScaleInput();
				return;
			}
		}
		else
		{
			m_nWeighScalePort = 0;
			UpdateData(FALSE);
			OnBnClickedRadioWeighScaleInput();
			return;
		}
	}
	if (bPortNumFound)
	{
		CString strBaudRate, strByteSize, strParity, strStopBit;
		strBaudRate.Format(L"%d", pApp->m_stScannerPortParams.nBaudRate);
		strByteSize.Format(L"%d", pApp->m_stScannerPortParams.nByteSize);
		strParity.Format(L"%d", pApp->m_stScannerPortParams.nParity);
		strStopBit.Format(L"%d", pApp->m_stScannerPortParams.nStopBit);
		SetCmbSelections(strBaudRate, strByteSize, strParity, strStopBit);
	}
	else
	{
		AfxMessageBox(L"Current Scanner Port Number Not Found.\nSelect another from list of available ports.");
		m_nWeighScalePort = 0;
		UpdateData(FALSE);
		OnBnClickedRadioWeighScaleInput();
	}
} // OnBnClickedScannerPort()


void CDlgSerialPortParams::OnBnClickedFootSwitchPort()
{
	CPMSHJApp* pApp = (CPMSHJApp*)AfxGetApp();
	CString str, strFootSwitchPort;
	strFootSwitchPort.Format(L"COM%d", pApp->m_stFootSwitchPortParams.nComPortNum);
	BOOL bPortNumFound = FALSE;
	int nNumAvailablePorts = m_listPorts.GetCount();
	for (int i = 1; i < nNumAvailablePorts; i++)
	{
		m_listPorts.GetText(i, m_strCOMPort);
		if (m_strCOMPort.Find(strFootSwitchPort) != -1)
		{
			m_listPorts.SetCurSel(i);
			bPortNumFound = TRUE;
			break;
		}
	}
	if (!bPortNumFound)// Current scanner port number not found! Try to set current list box sel as scanner port
	{
		m_listPorts.GetText(m_listPorts.GetCurSel(), str);
		int nCurIndex = m_listPorts.GetCurSel();
		if (::MessageBox(NULL, L"Current Foot Switch port not found! Do you want to set selected COM port as printer port?", L"Port Not Found", MB_YESNO) == IDYES)
		{
			if (str.Find(L"Silicon Labs") == -1)// If it is not a Excell weigh scale port
			{
				// Get currently selected COM port number
				for (int i = 1; i < 100; i++)
				{
					strFootSwitchPort.Format(L"COM%d", i);
					if (str.Find(strFootSwitchPort) != -1)
					{
						pApp->m_stFootSwitchPortParams.nComPortNum = i;
						bPortNumFound = TRUE;
						break;
					}
				}
			}
			else
			{
				AfxMessageBox(L"Cannot select this port for Foot Switch. Select another port and retry.");
				m_nWeighScalePort = 0;
				UpdateData(FALSE);
				OnBnClickedRadioWeighScaleInput();
				return;
			}
		}
		else
		{
			m_nWeighScalePort = 0;
			UpdateData(FALSE);
			OnBnClickedRadioWeighScaleInput();
			return;
		}
	}
	if (bPortNumFound)
	{
		CString strBaudRate, strByteSize, strParity, strStopBit;
		strBaudRate.Format(L"%d", pApp->m_stFootSwitchPortParams.nBaudRate);
		strByteSize.Format(L"%d", pApp->m_stFootSwitchPortParams.nByteSize);
		strParity.Format(L"%d", pApp->m_stFootSwitchPortParams.nParity);
		strStopBit.Format(L"%d", pApp->m_stFootSwitchPortParams.nStopBit);
		SetCmbSelections(strBaudRate, strByteSize, strParity, strStopBit);
	}
	else
	{
		AfxMessageBox(L"Current Foot Switch Port Number Not Found.\nSelect another from list of available ports.");
		m_nWeighScalePort = 0;
		UpdateData(FALSE);
		OnBnClickedRadioWeighScaleInput();
	}
} // OnBnClickedFootSwitchPort()
