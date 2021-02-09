
// PMSHJView.cpp : implementation of the CPMSHJView class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "PMSHJ.h"
#endif

#include "PMSHJSet.h"
#include "PMSHJDoc.h"
#include "PMSHJView.h"
#include "DlgSerialPortParams.h"
#include "DlgSettings.h"
#include "DlgReport.h"
#include "DlgCalib.h"
#include "tsclib.h"
#include "afxctl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CPMSHJView

IMPLEMENT_DYNCREATE(CPMSHJView, CRecordView)

BEGIN_MESSAGE_MAP(CPMSHJView, CRecordView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CRecordView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CRecordView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CPMSHJView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BN_EXIT, &CPMSHJView::OnBnClickedBnExit)
	ON_BN_CLICKED(IDC_BN_ZERO_WT, &CPMSHJView::OnBnClickedBnZeroWt)
	ON_BN_CLICKED(IDC_BN_SETTINGS, &CPMSHJView::OnBnClickedBnSettings)
	ON_BN_CLICKED(IDC_BN_SCAN, &CPMSHJView::OnBnClickedBnScan)
	ON_BN_CLICKED(IDC_BN_REPORT, &CPMSHJView::OnBnClickedBnReport)
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDC_BN_STOP, &CPMSHJView::OnBnClickedBnStop)
	ON_WM_VSCROLL()
	ON_BN_CLICKED(IDC_PRINT, &CPMSHJView::OnBnClickedPrint)
	ON_BN_CLICKED(IDC_BN_AIR_FREIGHT, &CPMSHJView::OnBnClickedBnAirFreight)
	ON_BN_CLICKED(IDC_BN_SEA_FREIGHT, &CPMSHJView::OnBnClickedBnSeaFreight)
END_MESSAGE_MAP()

// CPMSHJView construction/destruction

CPMSHJView::CPMSHJView()
	: CRecordView(IDD_PMSHJ_FORM)
{
	m_pSet = NULL;
	m_img = NULL;

	// Fill m_arPeripheryPoints
	for (int i = 10; i <= 630; i += 10) // Was i += 80
		m_arPeripheryPoints.Add(CPoint(i, 10));
	for (int j = 10; j <= 470; j += 10)
		m_arPeripheryPoints.Add(CPoint(630, j));
	for (int i = 10; i <= 630; i += 10)
		m_arPeripheryPoints.Add(CPoint(i, 470));
	for (int j = 10; j <= 470; j += 10)
		m_arPeripheryPoints.Add(CPoint(10, j));

	m_b3DLive = m_bPlatformCalibON = m_bFootSwithTrigRcd = m_bWeightScanDone = FALSE;
	m_bWtStatus = m_bScanStatus = m_bPkgNumGreen = TRUE;
	m_dBoxLength = m_dBoxWidth = m_dBoxHeight = m_dBoxVolume = m_dBoxWeightNet = m_dBoxWeightGr = m_dBoxWeightTare = m_dBoxWeightPTare = 0.0f;
	m_eWtUnits = INVALID_WT;

	m_rectImage = CRect(292, 148, 812, 536);
	m_rectBnPanel = CRect(5, 5, 10, 10);
	m_dOldLength = m_dOldWidth = m_dOldHeight = 0.0f;

	SetDecimalSeparator();// Sets value of m_bDecimalSeparatorDot
	m_dAvgZ = 0.0f;
	m_NewDepthFrame = FALSE;
	m_bLogoUploadedToPrinter = FALSE;

	m_rectFreightType = CRect(0,570,240,720);
	m_hBitmapFreightSliderON = (HBITMAP)LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP_SLIDER_ON));
	m_hBitmapFreightSliderOFF = (HBITMAP)LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP_SLIDER_OFF));
	m_hBitmapDataOK = (HBITMAP)LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP_DATA_OK));
	m_hBitmapDataNotDone = (HBITMAP)LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAPDATA_NOT_DONE));
	m_hBitmapDataFail = (HBITMAP)LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP_DATA_FAIL));
}

CPMSHJView::~CPMSHJView()
{
	if (m_img != NULL)
		DeleteObject(m_img);
	delete m_font;
}

void CPMSHJView::DoDataExchange(CDataExchange* pDX)
{
	CRecordView::DoDataExchange(pDX);
	// you can insert DDX_Field* functions here to 'connect' your controls to the database fields, ex.
	// you can insert DDX_Field* functions here to 'connect' your controls to the database fields, ex.
	DDX_FieldText(pDX, IDC_EDIT_STA_NAME, m_pSet->m_sta_name, m_pSet);
	DDX_FieldText(pDX, IDC_EDIT_data_stamp, m_pSet->m_data_stamp, m_pSet);
	DDX_FieldText(pDX, IDC_EDIT_wk_date, m_pSet->m_wk_date, m_pSet);
	DDX_FieldText(pDX, IDC_EDIT_tran_no, m_pSet->m_tran_no, m_pSet);
	DDX_FieldText(pDX, IDC_EDIT_pack_no, m_pSet->m_pack_no, m_pSet);
	DDX_FieldText(pDX, IDC_EDIT_pack_l, m_pSet->m_pack_l, m_pSet);
	DDX_FieldText(pDX, IDC_EDIT_pack_w, m_pSet->m_pack_w, m_pSet);
	DDX_FieldText(pDX, IDC_EDIT_pack_h, m_pSet->m_pack_h, m_pSet);
	DDX_FieldText(pDX, IDC_EDIT_pack_v, m_pSet->m_pack_v, m_pSet);
	DDX_FieldText(pDX, IDC_EDIT_v_unit, m_pSet->m_v_unit, m_pSet);
	DDX_FieldText(pDX, IDC_EDIT_pack_net, m_pSet->m_pack_net, m_pSet);
	DDX_FieldText(pDX, IDC_EDIT_pack_gross, m_pSet->m_pack_gross, m_pSet);
	DDX_FieldText(pDX, IDC_EDIT_pack_tare, m_pSet->m_pack_tare, m_pSet);
	DDX_FieldText(pDX, IDC_EDIT_pack_ptare, m_pSet->m_pack_ptare, m_pSet);
	DDX_FieldText(pDX, IDC_EDIT_wt_unit, m_pSet->m_wt_unit, m_pSet);
	DDX_FieldText(pDX, IDC_EDIT_upd_flag, m_pSet->m_upd_flag, m_pSet);
	DDX_FieldText(pDX, IDC_EDIT_del_mark, m_pSet->m_del_mark, m_pSet);// 17 Fields
	// See MSDN and ODBC samples for more information
	DDX_Control(pDX, IDC_BN_SETTINGS, m_BnSettings);
	DDX_Control(pDX, IDC_BN_REPORT, m_BnReport);
	DDX_Control(pDX, IDC_BN_EXIT, m_BnExit);
	DDX_Control(pDX, IDC_BN_SCAN, m_BnScan);
	DDX_Control(pDX, IDC_BN_STOP, m_BnStop);
	DDX_Control(pDX, IDC_MSFLEXGRID, m_FlexGrid);
	DDX_Control(pDX, IDC_STATIC_TEXT_LEN, m_StaticTextLength);
	DDX_Control(pDX, IDC_STATIC_TEXT_WIDTH, m_StaticTextWidth);
	DDX_Control(pDX, IDC_STATIC_TEXT_HT, m_StaticTextHeight);
	DDX_Control(pDX, IDC_STATIC_TEXT_VOL, m_StaticTextVolume);
	DDX_Control(pDX, IDC_STATIC_TEXT_WT, m_StaticTextWeight);
	DDX_Control(pDX, IDC_PRINT, m_BnPrint);
	DDX_Control(pDX, IDC_BN_AIR_FREIGHT, m_BnAirFreight);
	DDX_Control(pDX, IDC_BN_SEA_FREIGHT, m_BnSeaFreight);
	DDX_Control(pDX, IDC_PIC_DATA_LIGHT, m_PicDataLoggingStatus);
}

BOOL CPMSHJView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CRecordView::PreCreateWindow(cs);
}

void CPMSHJView::OnInitialUpdate()
{
	m_pSet = &GetDocument()->m_PMSHJSet;

	m_pApp = (CPMSHJApp*)AfxGetApp();
	m_pFrame = (CMainFrame*)AfxGetMainWnd();
	m_pFrame->m_pImageView = this;

	CvSize sz;
	sz.width = IMAGE_WIDTH;
	sz.height = IMAGE_HEIGHT;
	m_MatColor = cv::Mat(sz, CV_8UC3);
	m_MatColor.setTo(0);
	m_MatColorSmall = cv::Mat(cv::Size(m_rectImage.Width(), m_rectImage.Height()), CV_8UC3);
	m_MatColorSmall.setTo(0);
	m_MatDepth = cv::Mat(sz, CV_16UC1);
	m_imgMask = cvCreateImage(sz, IPL_DEPTH_8U, 1);
	m_imgDepthGray = cvCreateImage(sz, IPL_DEPTH_8U, 1);
	m_PlatformLevelGray = cvCreateImage(sz, IPL_DEPTH_8U, 1);

	//m_rectPlatform = CRect(192, 32, 452, 380);
	m_rectPlatform = CRect(200, 120, 440, 360);
	m_rectROI = CRect(5, 5, 635, 475);
	int nXSpotLeft = m_rectPlatform.CenterPoint().x - m_pApp->m_stInspParams.nSpotRectSize / 2;
	int nXSpotRight = m_rectPlatform.CenterPoint().x + m_pApp->m_stInspParams.nSpotRectSize / 2;
	int nXSpotTop = m_rectPlatform.CenterPoint().y - m_pApp->m_stInspParams.nSpotRectSize / 2;
	int nXSpotBtm = m_rectPlatform.CenterPoint().y + m_pApp->m_stInspParams.nSpotRectSize / 2;
	m_rectCentralTarget = CRect(nXSpotLeft, nXSpotTop, nXSpotRight, nXSpotBtm);

	m_font = new CvFont;
	cvInitFont(m_font, CV_FONT_VECTOR0, 1.0f, 1.0f, 0, 2, 8);
	EnableScrollBarCtrl(SB_BOTH, FALSE);// Disable scroll bars
	m_fontStatic.CreateFont(
		64,                        // nHeight
		0,                         // nWidth
		0,                         // nEscapement
		0,                         // nOrientation
		FW_EXTRALIGHT,                 // nWeight
		FALSE,                     // bItalic
		FALSE,                     // bUnderline
		0,                         // cStrikeOut
		ANSI_CHARSET,              // nCharSet
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		ANTIALIASED_QUALITY,           // nQuality
		DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
		_T("Calibri Light"));                 // lpszFacename
	CRecordView::OnInitialUpdate();

}

void  CPMSHJView::SetButtonsState(BOOL bScanON)
{
	if (bScanON)
	{
		m_BnScan.SetIcon(IDI_START_CLKD, BS_CENTER);
		m_BnStop.SetIcon(IDI_STOP, BS_CENTER);
		m_BnPrint.SetIcon(IDI_PRINT);
	}
	else
	{
		m_BnReport.SetIcon(IDI_REPORT, BS_CENTER);
		m_BnPrint.SetIcon(IDI_PRINT);
		m_BnSettings.SetIcon(IDI_SETTINGS, BS_CENTER);
		m_BnScan.SetIcon(IDI_START, BS_CENTER);
		m_BnStop.SetIcon(IDI_STOP_CLKD, BS_CENTER);
	}
	if (!m_pApp->m_bReadWtThreadUp)
	{
		SetWeightReadStatus(FALSE);
		m_bWtStatus = FALSE;
	}
	// Invalidate button rect
	CRect rect(0, 50, 240, 720);
	InvalidateRect(&rect, FALSE);
}

void  CPMSHJView::SetButtonsNLabels()
{
	CString strBnCaption, strToolTip;
	std::wstring str = m_pApp->m_pLanguage->GetString(IDS_STR_SETTINGS);
	strBnCaption = str.c_str();

	m_BnSettings.SetFont(L"Aril", 22, 1000, 0, 0);
	m_BnSettings.SetTextColor(RGB(0, 0, 255));
	strToolTip = str.c_str();
	m_BnSettings.SetToolTipText(strToolTip);
	m_BnSettings.SetShade(SHS_METAL, 10, 30, 20, BTN_BACK_COLOR);
	m_BnSettings.SetFlat(FALSE);
	m_BnSettings.SetWindowText(strBnCaption);
	m_BnSettings.SetWindowText(L"");
	m_BnSettings.SetIcon(IDI_SETTINGS);

	str = m_pApp->m_pLanguage->GetString(IDS_STR_REPORT);
	strBnCaption = str.c_str();
	m_BnReport.SetFont(L"Aril", 22, 1000, 0, 0);
	m_BnReport.SetTextColor(RGB(0, 0, 255));
	str = m_pApp->m_pLanguage->GetString(IDS_STR_REPORT);
	strToolTip = str.c_str();
	m_BnReport.SetToolTipText(strToolTip);
	m_BnReport.SetShade(SHS_METAL, 10, 30, 20, BTN_BACK_COLOR);
	m_BnReport.SetFlat(FALSE);
	m_BnReport.SetWindowText(L"");
	m_BnReport.SetIcon(IDI_REPORT, BS_CENTER);

	str = m_pApp->m_pLanguage->GetString(IDS_STR_PRINT);
	strToolTip = str.c_str();
	m_BnPrint.SetToolTipText(strToolTip);
	m_BnPrint.SetIcon(IDI_PRINT);

	str = m_pApp->m_pLanguage->GetString(IDS_STR_SCAN);
	strBnCaption = str.c_str();
	m_BnScan.SetFont(L"Aril", 22, 1000, 0, 0);
	m_BnScan.SetTextColor(RGB(0, 0, 255));
	str = m_pApp->m_pLanguage->GetString(IDS_STR_SCAN);
	strToolTip = str.c_str();
	m_BnScan.SetToolTipText(strToolTip);
	m_BnScan.SetShade(SHS_METAL, 10, 30, 20, BTN_BACK_COLOR);
	m_BnScan.SetFlat(FALSE);
	m_BnScan.SetWindowText(L"");
	m_BnScan.SetIcon(IDI_START);

	m_BnStop.SetWindowText(L"");
	m_BnStop.SetIcon(IDI_STOP);
	str = m_pApp->m_pLanguage->GetString(IDS_STR_STOP_SCAN);
	strToolTip = str.c_str();
	m_BnStop.SetToolTipText(strToolTip);

	m_BnAirFreight.SetWindowText(L"");
	str = m_pApp->m_pLanguage->GetString(IDS_STR_AIR_FREIGHT);
	strToolTip = str.c_str();
	m_BnAirFreight.SetToolTipText(strToolTip);

	m_BnSeaFreight.SetWindowText(L"");
	str = m_pApp->m_pLanguage->GetString(IDS_STR_SEA_FREIGHT);
	strToolTip = str.c_str();
	m_BnSeaFreight.SetToolTipText(strToolTip);
	// Set freight mode slider bitmaps
	if (m_pApp->m_stFreightParams.FreightMode == FREIGHT_AIR)
	{
		m_BnSeaFreight.SetIcon(IDI_SEA_FREIGHT);
		m_BnAirFreight.SetIcon(IDI_AIR_FREIGHT_CLKD);
	}
	else
	{
		m_BnSeaFreight.SetIcon(IDI_SEA_FREIGHT_CLKD);
		m_BnAirFreight.SetIcon(IDI_AIR_FREIGHT);
	}

	str = m_pApp->m_pLanguage->GetString(IDS_STR_EXIT);
	strToolTip = str.c_str();
	m_BnExit.SetFont(L"Aril", 18, 1600, 0, 0);
	m_BnExit.SetTextColor(RGB(255, 0, 30));
	m_BnExit.SetToolTipText(strToolTip);
	m_BnExit.SetShade(SHS_METAL, 10, 30, 20, RED_COLOR);
	m_BnExit.SetFlat(FALSE);
	m_BnExit.SetWindowText(L"");
	m_BnExit.SetIcon(IDI_EXIT);

	m_StaticTextLength.ModifyStyle(SS_CENTER, SS_LEFT);
	m_StaticTextLength.SetFont(&m_fontStatic);
	m_StaticTextLength.SetWindowTextW(L"0000");
	m_StaticTextWidth.ModifyStyle(SS_CENTER, SS_LEFT);
	m_StaticTextWidth.SetFont(&m_fontStatic);
	m_StaticTextWidth.SetWindowTextW(L"000");
	m_StaticTextHeight.ModifyStyle(SS_CENTER, SS_LEFT);
	m_StaticTextHeight.SetFont(&m_fontStatic);
	m_StaticTextHeight.SetWindowTextW(L"00");
	m_StaticTextVolume.ModifyStyle(SS_CENTER, SS_LEFT);
	m_StaticTextVolume.SetFont(&m_fontStatic);
	m_StaticTextVolume.SetWindowTextW(L"00.00");
	m_StaticTextWeight.ModifyStyle(SS_CENTER, SS_LEFT);
	m_StaticTextWeight.SetFont(&m_fontStatic);
	m_StaticTextWeight.SetWindowTextW(L"000.0");

	SetFocusToHorzBar();
	CRect rect(0, 2, 252, 768);

	InvalidateRect(&rect, FALSE);
} // SetButtons()

// To ensure focus is away from any buttons
void CPMSHJView::SetFocusToHorzBar()
{
	GetDlgItem(IDC_PIC_HEIGHT)->SetFocus();
}

// CPMSHJView printing
void CPMSHJView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CPMSHJView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CPMSHJView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CPMSHJView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

void CPMSHJView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CPMSHJView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CPMSHJView diagnostics

#ifdef _DEBUG
void CPMSHJView::AssertValid() const
{
	CRecordView::AssertValid();
}

void CPMSHJView::Dump(CDumpContext& dc) const
{
	CRecordView::Dump(dc);
}

CPMSHJDoc* CPMSHJView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CPMSHJDoc)));
	return (CPMSHJDoc*)m_pDocument;
}
#endif //_DEBUG


// CPMSHJView database support
CRecordset* CPMSHJView::OnGetRecordset()
{
	return m_pSet;
}

// CPMSHJView message handlers

HBRUSH CPMSHJView::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	CPMSHJApp* pApp = (CPMSHJApp*)AfxGetApp();
	pDC->SetBkMode(TRANSPARENT);
	if ((pWnd->GetDlgCtrlID() == IDC_EDIT_del_mark) || (pWnd->GetDlgCtrlID() == IDC_EDIT_STA_NAME) ||
		(pWnd->GetDlgCtrlID() == IDC_EDIT_data_stamp) || (pWnd->GetDlgCtrlID() == IDC_EDIT_tran_no) ||
		(pWnd->GetDlgCtrlID() == IDC_EDIT_wk_date) || (pWnd->GetDlgCtrlID() == IDC_EDIT_pack_no) ||
		(pWnd->GetDlgCtrlID() == IDC_EDIT_pack_l) || (pWnd->GetDlgCtrlID() == IDC_EDIT_pack_w) ||
		(pWnd->GetDlgCtrlID() == IDC_EDIT_pack_h) || (pWnd->GetDlgCtrlID() == IDC_EDIT_pack_v) ||
		(pWnd->GetDlgCtrlID() == IDC_EDIT_pack_net) || (pWnd->GetDlgCtrlID() == IDC_EDIT_pack_gross) ||
		(pWnd->GetDlgCtrlID() == IDC_EDIT_pack_tare) || (pWnd->GetDlgCtrlID() == IDC_EDIT_pack_ptare) ||
		(pWnd->GetDlgCtrlID() == IDC_EDIT_upd_flag) || (pWnd->GetDlgCtrlID() == IDC_EDIT_pack_gross) ||
		(pWnd->GetDlgCtrlID() == IDC_EDIT_wt_unit) || (pWnd->GetDlgCtrlID() == IDC_EDIT_v_unit))
	{
		int nID = pWnd->GetDlgCtrlID();// To check
		pDC->SetTextColor(RGB(70, 140, 180));
		return pApp->m_DBSetBkgndBrush;
	}
	else if ((pWnd->GetDlgCtrlID() == IDC_STATIC_TEXT_LEN) || (pWnd->GetDlgCtrlID() == IDC_STATIC_TEXT_WIDTH) || (pWnd->GetDlgCtrlID() == IDC_STATIC_TEXT_HT) ||
		(pWnd->GetDlgCtrlID() == IDC_STATIC_TEXT_VOL) || (pWnd->GetDlgCtrlID() == IDC_STATIC_TEXT_WT))
	{
		pDC->SetTextColor(RGB(255, 255, 255));
		pWnd->SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);// Has no effect
		return pApp->m_StaticTextBkgndBrush;
	}
	return pApp->m_BkgndBrush;
}

void CPMSHJView::FillBitmapInfo(BITMAPINFO* bmi, int width, int height, int bpp, int origin)
{
	assert(bmi && width >= 0 && height >= 0 && (bpp == 8 || bpp == 24 || bpp == 32));

	BITMAPINFOHEADER* bmih = &(bmi->bmiHeader);

	memset(bmih, 0, sizeof(*bmih));
	bmih->biSize = sizeof(BITMAPINFOHEADER);
	bmih->biWidth = width;
	bmih->biHeight = origin ? abs(height) : -abs(height);
	bmih->biPlanes = 1;
	bmih->biBitCount = (unsigned short)bpp;
	bmih->biCompression = BI_RGB;

	if (bpp == 8)
	{
		RGBQUAD* palette = bmi->bmiColors;

		for (int i = 0; i < 256; i++)
		{
			palette[i].rgbBlue = palette[i].rgbGreen = palette[i].rgbRed = (BYTE)i;
			palette[i].rgbReserved = 0;
		}
	}
}


void  CPMSHJView::Show(cv::Mat img, HDC dc, int x, int y, int w, int h, int from_x, int from_y)
{
	uchar buffer[sizeof(BITMAPINFOHEADER) + 1024];
	BITMAPINFO* bmi = (BITMAPINFO*)buffer;
	int bmp_w = img.cols, bmp_h = img.rows;

	FillBitmapInfo(bmi, bmp_w, bmp_h, img.channels() * 8, 0);

	from_x = MIN(MAX(from_x, 0), bmp_w - 1);
	from_y = MIN(MAX(from_y, 0), bmp_h - 1);

	int sw = MAX(MIN(bmp_w - from_x, w), 0);
	int sh = MAX(MIN(bmp_h - from_y, h), 0);

	CRect rectClient;
	GetClientRect(&rectClient);
	SetDIBitsToDevice(dc, x, y, sw, sh, from_x, from_y, from_y, sh, img.data + from_y*img.rows, bmi, DIB_RGB_COLORS);
}

void CPMSHJView::OnBnClickedBnExit()
{
	SetFocusToHorzBar();
	m_BnExit.SetIcon(IDI_EXIT_CLKD);
	m_BnStop.SetIcon(IDI_STOP);
	m_HighResTimer.HighResSleep(200);
	if (m_b3DLive)
	{
		OnBnClickedBnStop();
		m_BnExit.SetIcon(IDI_EXIT);
		m_pApp->m_pFrame->m_bAbortScan = TRUE;
		return;
	}
	Invalidate();
	m_pApp->m_pFrame->m_bAbortScan = TRUE;

	m_pApp->m_pFrame->SendMessage(WM_CLOSE, NULL, NULL);
}

void CPMSHJView::OnBnClickedBnZeroWt()
{
	SetFocusToHorzBar();
	AfxMessageBox(L"TBD");
}

void CPMSHJView::OnBnClickedBnSettings()
{
	// For trials
	//int nRoundedOffLengthCM = 27;
	//int nRoundedOffWidthCM = 19;
	//int nRoundedOffHeightCM = 17;
	//int nRoundedVolumeInt = nRoundedOffLengthCM * nRoundedOffWidthCM * nRoundedOffHeightCM * 100;
	//double dRoundedVolumeCM = nRoundedVolumeInt / 100.0;
	//CString str;
	//str.Format(L"%3.6f", dRoundedVolumeCM / (1000 * 1000));
	//double dRefined = _wtof(str);
	//STInspResult InspResult;
	//InspResult.dLength = 340;
	//InspResult.dWidth = 180;
	//InspResult.dHeight = 130;
	//InspResult.dWeightNet = 2.08;
	//InspResult.FreightMode = FREIGHT_AIR;
	//eFreightMode ActualFreightMode;
	//InspResult.dFreight = GetFreight(InspResult, m_pApp->m_stFreightParams, ActualFreightMode);
	//m_pApp->m_stInspResultSaved = InspResult;
	//SetEvent(m_pApp->m_hSendDataToServer);

	SetDataLoggingStatus(DATA_OK);
	if (m_b3DLive)
		return;
	SetFocusToHorzBar();
	m_BnSettings.SetIcon(IDI_SETTINGS_CLKD);
	m_BnStop.SetIcon(IDI_STOP);
	Invalidate();
	CDlgSettings dlg;

	//CDlgSerialPortParams dlg;
	dlg.DoModal();
	m_BnStop.SetIcon(IDI_STOP_CLKD, BS_CENTER);
}

void CPMSHJView::OnBnClickedBnReport()
{
	if (m_b3DLive)
		return;
	m_BnReport.SetIcon(IDI_REPORT_CLKD, BS_CENTER);
	m_BnStop.SetIcon(IDI_STOP);
	Invalidate();
	SetFocusToHorzBar();
	CDlgReport dlg;
	dlg.DoModal();
	m_BnStop.SetIcon(IDI_STOP_CLKD, BS_CENTER);
}

void CPMSHJView::OnBnClickedBnScan()
{
	if (m_b3DLive)
	{
		SetFocusToHorzBar();
		return;
	}
	if (!m_pApp->m_bCam3DAvailable)
	{
		IOutputLog(L"3D camera NA");
		m_BnStop.SetIcon(IDI_STOP_CLKD);
		return;
	}
	SetButtonsNLabels();
	CString strBnCaption, strToolTip;
	std::wstring str;
	if (!m_b3DLive)
	{
		SetScanBnText(TRUE);
		//IOutputLog(L"Preparing to start Live");
		//SetMessage(IDS_STR_SCAN_UP);
		////IOutputLog(L"Before GetString(IDS_STR_STOP)");
		//str = m_pApp->m_pLanguage->GetString(IDS_STR_STOP);
		////IOutputLog(L"After GetString(IDS_STR_STOP)");
		//strBnCaption = str.c_str();
		//m_pApp->m_pFrame->m_bAbortScan = FALSE;
		//m_BnScan.SetTextColor(RGB(255, 0, 0));
		//m_BnScan.SetWindowText(strBnCaption);
		//str = m_pApp->m_pLanguage->GetString(IDS_STR_STOP_SCAN);
		//strToolTip = str.c_str();
		//m_BnScan.SetToolTipText(strToolTip);
		SetButtonsState(TRUE);
		if (!m_pApp->m_bReadWtThreadUp && m_pApp->m_stOptions.bWtRqd)
		{
			//IOutputLog(L"Before call to StartWeighScaleReadThread()");
			m_pApp->StartWeighScaleReadThread();
			//IOutputLog(L"After call to StartWeighScaleReadThread()");

			m_HighResTimer.HighResSleep(200);// Giving time to thread to come up and set m_pApp->m_bReadWtThreadUp flag!
		}
		m_bWtStatus = m_pApp->m_bReadWtThreadUp;

		SetWeightReadStatus(m_bWtStatus); 
		// Start Barcode scanner thread if not already up if using serial port scanner
		if (m_pApp->m_stOptions.bUseCodeAsTrigger)
		{
			if ((m_pApp->m_stOptions.bSerialPortScanner) || (m_pApp->m_stOptions.bHikVisionScanner))
			{
				if (!m_pApp->m_bScannerThreadUp)
				{
					m_pApp->m_bScannerThreadUp = m_pApp->StartCodeScannerThread();
					if (m_pApp->m_bScannerThreadUp)
						IOutputLog(L"Started Code Scanner Thread()");
					else
						IOutputLog(L"Could not start Code Scanner Thread()");
				}
			}
			else
			{
				m_pFrame->StartKBCapture();
				IOutputLog(L"Using KB for Barcode scanning");
			}
		}
		else
			IOutputLog(L"Barcode scanning is OFF");
		//
		SetFocusToHorzBar();
		Scan();
	}
	else
	{
		SetScanBnText(FALSE);
		//str = m_pApp->m_pLanguage->GetString(IDS_STR_SCAN);
		//strBnCaption = str.c_str();
		//SetMessage(IDS_STR_SCAN_STOPPED);
		//m_pApp->m_pFrame->m_bAbortScan = TRUE;// This causes scan to be aborted
		//m_BnScan.SetTextColor(RGB(0, 0, 255));
		//m_BnScan.SetWindowText(strBnCaption);
		//m_BnScan.SetToolTipText(strBnCaption);
		m_b3DLive = FALSE;
		m_bPlatformCalibON = FALSE;
		SetButtonsState(FALSE);
		if (m_pApp->m_bReadWtThreadUp)
			m_pApp->StopWeighScaleReadThread();
		
		if (m_pFrame->m_bKBCapture)
			m_pFrame->StopKBCapture();

		SetFocusToHorzBar();

		Sleep(200);// Waiting for weight read thread to end
		SetWeightReadStatus(FALSE);
	}
}

void CPMSHJView::SetScanBnText(BOOL bLive)
{
	CString strBnCaption, strToolTip;
	std::wstring str;
	if (bLive)
	{
		SetMessage(IDS_STR_SCAN_UP);
		str = m_pApp->m_pLanguage->GetString(IDS_STR_STOP);
		strBnCaption = str.c_str();
		m_pApp->m_pFrame->m_bAbortScan = FALSE;
		m_BnScan.SetTextColor(RGB(255, 0, 0));
		//m_BnScan.SetWindowText(strBnCaption);
		str = m_pApp->m_pLanguage->GetString(IDS_STR_STOP_SCAN);
		strToolTip = str.c_str();
		//m_BnScan.SetToolTipText(strToolTip);
	}
	else
	{
		SetMessage(IDS_STR_SCAN_STOPPED);
		str = m_pApp->m_pLanguage->GetString(IDS_STR_SCAN);
		strBnCaption = str.c_str();
		m_pApp->m_pFrame->m_bAbortScan = TRUE;// This causes scan to be aborted
		m_BnScan.SetTextColor(RGB(0, 0, 255));
		//m_BnScan.SetWindowText(strBnCaption);
		m_BnScan.SetToolTipText(strBnCaption);
	}
}

void CPMSHJView::SetMessage(CString str, COLORREF color)
{
	// Put new text only if the text has changed
	CString strOld;
	if (m_strMessage != str)
	{
		m_strMessage = str;
	}
}

void CPMSHJView::Scan()
{
	CString strBnCaption, strToolTip;
	std::wstring str;
	str = m_pApp->m_pLanguage->GetString(IDS_STR_STOP);
	strBnCaption = str.c_str();
	//m_BnScan.SetWindowText(strBnCaption);
	ImageLiveStart3DOrbbec(100000);
}

// This func checks variation in length and width readings and returns true if variation is within set limit
BOOL CPMSHJView::AreDimReadingsStable(int nMaxAllDev)
{
	BOOL bResult = TRUE;
	double dLengthMin = LONG_MAX;
	double dLengthMax = LONG_MIN;
	double dWidthMin = LONG_MAX;
	double dWidthMax = LONG_MIN;
	for (int i = 0; i < m_arLength.GetSize(); i++)
	{
		if (m_arLength[i] < dLengthMin)
			dLengthMin = m_arLength[i];
		if (m_arLength[i] > dLengthMax)
			dLengthMax = m_arLength[i];
	}
	for (int i = 0; i < m_arWidth.GetSize(); i++)
	{
		if (m_arWidth[i] < dWidthMin)
			dWidthMin = m_arWidth[i];
		if (m_arWidth[i] > dWidthMax)
			dWidthMax = m_arWidth[i];
	}

	if (((dLengthMax - dLengthMin) > 10) || ((dWidthMax - dWidthMin) > 10))
		bResult = FALSE;

	return bResult;
} // AreDimReadingsStable()

// Not used anymore!
void CPMSHJView::ShowScanID(CString str)
{
	m_bPkgNumGreen = !m_bPkgNumGreen;
	int nIndex = str.Find(L"Num");
	if (nIndex != -1)
		str = str.Right(str.GetLength() - 3);
	CString strPkg = L"Pkg No: ";
	strPkg += str;
}

// Not used!
cv::Mat CPMSHJView::adjustDepth(const cv::Mat& inImage)
{
	// from https://orbbec3d.com/product-astra/
	// Astra S has a depth in the range 0.35m to 2.5m
	int maxDepth = 2500;
	int minDepth = 350; // in mm

	cv::Mat retImage = inImage;

	for (int j = 0; j < retImage.rows; j++)
		for (int i = 0; i < retImage.cols; i++)
		{
			if (retImage.at<ushort>(j, i))
				retImage.at<ushort>(j, i) = maxDepth - (retImage.at<ushort>(j, i) - minDepth);
		}

	return retImage;
}

// Live grab code for Orbbec camera.
// 
BOOL CPMSHJView::ImageLiveStart3DOrbbec(int nNoOfFrames, BOOL bFindBoxDims)
{
	IOutputLog(L"Entering ImageLiveStart3DOrbbec()");
	CString str;
	m_b3DLive = TRUE;
	m_bBoxResultReady = m_bFootSwithTrigRcd = FALSE;
	int nImagesToGrab = nNoOfFrames;
	m_nImagesGrabbed = 0;
	m_arLength.RemoveAll();
	m_arWidth.RemoveAll();
	m_arHeight.RemoveAll();
	m_arRayTracingBoundaryPoints.RemoveAll();
	bool TimoutOccurred = false;// As long as a valid buffer is returned, i.e., no timeout has occurred, the grab loop can continue.
	//m_pFrame->StartKBCapture();
	BOOL bBarcodeRcd = FALSE, bFootSwitchTrigRcd = FALSE;
	int nNumImagesAfterCodeRcpt = 0;
	int nNumImagesAfterDataSave = 100;
	if (m_pApp->m_stOptions.DimUnits == DIM_MM)
		m_strAccuracy = "%4.0f";
	else if (m_pApp->m_stOptions.DimUnits == DIM_CM)
		m_strAccuracy = "%3.1f";
	else if (m_pApp->m_stOptions.DimUnits == DIM_M)
		m_strAccuracy = "%1.3f";
	else if (m_pApp->m_stOptions.DimUnits == DIM_INCH)
		m_strAccuracy = "%3.1f";
	//m_pFrame->m_csArray.RemoveAll();
	//IOutputLog(L"Entering measurement loop");
	double t1 = m_HighResTimer.GetHighResElapsedTimeMsec();
	BOOL bBarcodeScanStart = FALSE;
	BOOL bAbortingToPrint = FALSE;
	double t2 = t1;
	BOOL bWFServerResponse = TRUE;
	// Open the printer port
	//if (m_pApp->m_stOptions.bPrintLabel && !m_bPrinterPortOpened)
	//{
	//	std::string strPrinter = CW2A(m_pApp->m_stPrintParams.strPrinterName);
	//	IOutputLog(L"Opening Printer Port!");
	//	int nRetVal = openport((char*)strPrinter.c_str());// This Works!
	//	if (nRetVal == 0)
	//	{
	//		AfxMessageBox(L"Could not open printer port. Label cannot be printed.");
	//		m_bPrinterPortOpened = FALSE;
	//	}
	//	else
	//	{
	//		if (!m_pApp->m_pFrame->m_pImageView->CheckPrinterStatus())
	//		{
	//			IOutputLog(L"CheckPrinterStatus() returned FALSE!");
	//			nRetVal = closeport();
	//			m_bPrinterPortOpened = FALSE;
	//			AfxMessageBox(L"Printer Problem!");
	//		}
	//		else
	//			m_bPrinterPortOpened = TRUE;
	//	}
	//	m_bLogoUploadedToPrinter = FALSE;
	//}
	MSG msg;
	try
	{
		int nNumFrames = 0;
		int changedIndex;
		openni::VideoFrameRef depthFrame, colorFrame;
		openni::VideoStream* stream[] = { &m_pApp->m_depth, &m_pApp->m_color };

		int nChannels = 1;
		BOOL bDepthFrameValid = FALSE;
		int rowSize = 640;// Default 
		int nFrameNumAtLastBarcode = 0;// Used to refresh message
		IOutputLog(L"Entering Grab Loop");
		do
		{
			openni::Status rc = OpenNI::waitForAnyStream(stream, 2, &changedIndex, TIMEOUT_FOREVER);
			if (rc != openni::STATUS_OK)
			{
				str.Format(L"Error in Image Grabbing - waitForAnyStream() failed:\n%s\n", openni::OpenNI::getExtendedError());
				SetMessage(str);
				IOutputLog(L"Error in Image Grabbing - waitForAnyStream() failed");
				//openni::OpenNI::shutdown();
				//return FALSE;
			}
			switch (changedIndex)
			{
			case 0:
				rc = m_pApp->m_depth.readFrame(&depthFrame);
				if (rc != openni::STATUS_OK)
				{
					str.Format(L"Error in Image Grabbing - depth.readFrame() failed:\n%s\n", openni::OpenNI::getExtendedError());
					IOutputLog(L"Error in Image Grabbing - depth.readFrame() failed");
					SetMessage(IDS_STR_GRAB_ERR);
					openni::OpenNI::shutdown();
					m_b3DLive = FALSE;
					SetButtonsState(FALSE);
					return m_bBoxResultReady;
				}
				if (depthFrame.isValid())
				{
					m_MatDepth.data = (uchar*)depthFrame.getData();
				}
				m_NewDepthFrame = TRUE;
				break;

			case 1:
				rc = m_pApp->m_color.readFrame(&colorFrame);
				if (rc != openni::STATUS_OK)
				{
					str.Format(L"Error in Image Grabbing - color.readFrame() failed:\n%s\n", openni::OpenNI::getExtendedError());
					IOutputLog(L"Error in Image Grabbing - color.readFrame() failed");
					SetMessage(IDS_STR_GRAB_ERR);
					openni::OpenNI::shutdown();
					m_b3DLive = FALSE;
					SetButtonsState(FALSE);
					return m_bBoxResultReady;
				}
				if (colorFrame.isValid())
				{
					m_MatColor.data = (uchar*)colorFrame.getData();
					cv::cvtColor(m_MatColor, m_MatColor, CV_BGR2RGB);
					m_bImageLoaded = TRUE;
					if (m_NewDepthFrame)//  If new depth image was obtained
					{
						m_NewDepthFrame = FALSE;
						CString strMsg;
						//InvalidateRect(m_rectImage, FALSE);
						nNumImagesAfterDataSave++;
						if (!Process3DDataOrbbec(m_pApp->m_depth, m_MatDepth, m_nImagesGrabbed, m_pApp->m_stInspParams.bIsCuboid))
						{
							//IOutputLog(L"Process3DDataOrbbec() not OK");
							strMsg = "Scan Data Invalid!";
							SetMessage(IDS_STR_SCANDATA_INVALID, RED_COLOR);
							//IOutputLog(L"Scan Data Invalid!");

							if (m_bScanStatus)// SetBitmap() only if previously it was a different one
							{
								m_strBoxLength = m_strBoxWidth = m_strBoxHeight = m_strBoxVolume = L"0";// KPD = 150520
								CString strNull = L"0";
								ShowDimensions(m_strBoxLength, m_strBoxWidth, m_strBoxHeight, m_strWeight, m_strBoxVolume);
							}
							m_bScanStatus = FALSE;
							if (nNumImagesAfterDataSave > 100)
								SetMessage(IDS_STR_SCANDATA_INVALID, RED_COLOR);

							// Draw central target box. This serves as indication that the box placement is ok
							DrawCentralTargetRect(m_MatColor, cv::Scalar(255, 0, 255));

							if (m_pApp->m_stInspParams.bFlipImageVert)
								flip(m_MatColor, m_MatColor, 0);
							if (m_pApp->m_stInspParams.bFlipImageHorz)
								flip(m_MatColor, m_MatColor, +1);
							cv::resize(m_MatColor, m_MatColorSmall, m_MatColorSmall.size(), 0, 0, cv::INTER_AREA);
							InvalidateRect(m_rectImage, FALSE);
							//SetDataLoggingStatus(DATA_NOT_DONE);
						} // Process3DDataOrbbec() returned FALSE
						else // Processing of 3D data was fine 
						{
							if (nNumImagesAfterDataSave > 100)
							{
								SetMessage(IDS_STR_SCADATA_DSPLD, GREEN_COLOR);
							}
							m_nImagesGrabbed++;
							InvalidateRect(m_rectImage, FALSE);
							MSG msg;
							while (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
							{
								::TranslateMessage(&msg);
								::DispatchMessage(&msg);
							}
							if (m_pApp->m_stOptions.bUseCodeAsTrigger) // look for barcode if required!
							{
								//m_bNeedToRefresh = FALSE;
								m_bScanStatus = TRUE;
								strMsg = "Scan Data Displayed! W/F Barcode";
								if (bBarcodeRcd)
								{
									nNumImagesAfterCodeRcpt++;
									nFrameNumAtLastBarcode = m_nImagesGrabbed;
								}
								else if ((m_pApp->m_stOptions.bSerialPortScanner) || (m_pApp->m_stOptions.bHikVisionScanner))
								{
									// It is necessary to put this here as otherwise due to rapid code scanning in cont mode screen doesn't refresh.
									MSG msg;
									while (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
									{
										::TranslateMessage(&msg);
										::DispatchMessage(&msg);
									}
									CString strCode;
									EnterCriticalSection(&(m_pApp->m_csScannerString));
									strCode = m_pApp->m_strScannerString;
									m_pApp->m_strScannerString.Empty();
									LeaveCriticalSection(&(m_pApp->m_csScannerString));
									if ((strCode.IsEmpty()))// Code not yet received!
										continue;
									if (strCode == m_pApp->m_stInspResultSaved.strBarcode)// Same barcode is read again!
									{
										int nLen = abs(m_pApp->m_stInspResultSaved.dLength - m_pApp->m_stInspResult.dLength);
										int nWid = abs(m_pApp->m_stInspResultSaved.dWidth - m_pApp->m_stInspResult.dWidth);
										int nHt = abs(m_pApp->m_stInspResultSaved.dHeight - m_pApp->m_stInspResult.dHeight);
										// If dims are unchanged wait for next box - continue 3D scanning
										if ((nLen < 2) && (nWid < 2) && (nHt < 2))
											continue;
									}
									if (strCode.GetLength() >= m_pApp->m_stInspParams.nBarcodeLengthMin)
									{
										m_pApp->m_stInspResult.strBarcode = strCode;
										//ShowScanID(strCode);
										bBarcodeRcd = TRUE;
										nNumImagesAfterCodeRcpt = 0;
									}
									else if (strCode.GetLength() >= 3)// Some code was received
									{
										bBarcodeRcd = FALSE;
										strMsg = L"Length of code received short.";
										SetMessage(strMsg, RED_COLOR);
										IOutputLog(L"Length of barcode less than param");
										continue;
									}
									else // W/F Barcode
										continue;
								}
							}// if m_pApp->m_stOptions.bUseCodeAsTrigger is TRUE
							else // Foot switch signal used as trigger
							{
								m_pApp->m_stInspResult.strBarcode.Empty();// No code is required so string must be empty
								if (m_pApp->m_stOptions.bUseFootSwitch)
								{
									// Wait here to get foot switch trigger
									if (!bFootSwitchTrigRcd && ((m_nImagesGrabbed - nFrameNumAtLastBarcode) > 60))
									{
										SetEvent(m_pApp->m_hCheckFootSwitch);
										EnterCriticalSection(&(m_pApp->m_csFootSwitchTrigger));
										if (m_bFootSwithTrigRcd == TRUE)
										{
											nFrameNumAtLastBarcode = m_nImagesGrabbed;
											bFootSwitchTrigRcd = TRUE;
											m_bFootSwithTrigRcd = FALSE;
											nNumImagesAfterCodeRcpt++;// Will happen only once after foot switch trigg rcd
										}
										LeaveCriticalSection(&(m_pApp->m_csFootSwitchTrigger));
									}
									else // Trigger already received. Now we have to collect 10 more images
										nNumImagesAfterCodeRcpt++;
								}
								else // May be used for weight stable trigger later. As of now it will just keep on saving data!
									bFootSwitchTrigRcd = TRUE;
							} // Foot switch is used as trigger
							if (nNumImagesAfterCodeRcpt == 1)// Log just once
								IOutputLog(L"Full barcode / foot switch trigger received");
							
							SetDataLoggingStatus(DATA_NOT_DONE);
							// Barcode received. Now wait for dimensions to stabilize and then save scan data
							if ((bBarcodeRcd || bFootSwitchTrigRcd) && (nNumImagesAfterCodeRcpt >= 10))
							{
								if (AreDimReadingsStable(m_pApp->m_stInspParams.nDsplAccuracy))
								{
									IOutputLog(L"Full barcode / foot switch trigger received, 10 images grabbed and stable");
									nNumImagesAfterCodeRcpt = 0;
									m_pApp->m_stInspResult.dLength = m_dBoxLength;
									m_pApp->m_stInspResult.dWidth = m_dBoxWidth;
									m_pApp->m_stInspResult.dHeight = m_dBoxHeight;
									m_pApp->m_stInspResult.dVolume = m_dBoxVolume;
									m_pApp->m_stInspResult.strDimensionUnit = "M";

									m_pApp->m_stInspResult.dWeightNet = m_dBoxWeightNet;
									m_pApp->m_stInspResult.dWeightGr = m_dBoxWeightGr;
									m_pApp->m_stInspResult.dWeightTare = m_dBoxWeightTare;
									m_pApp->m_stInspResult.dWeightPT = m_dBoxWeightPTare;
									m_pApp->m_stInspResult.strWeightunit = m_pApp->m_strWtUnit;
									eFreightMode FreightMode = m_pApp->m_stFreightParams.FreightMode;
									m_pApp->m_stInspResult.dFreight = GetFreight(m_pApp->m_stInspResult, m_pApp->m_stFreightParams, FreightMode);
									m_pApp->m_stInspResult.FreightMode = FreightMode;
									
									m_pApp->m_stInspResult.strWeightunit = m_pApp->m_strWtUnit; 

									m_pApp->m_stScanData.AddItem(m_pApp->m_stInspResult);
									bBarcodeRcd = FALSE;
									bFootSwitchTrigRcd = FALSE;
									if (m_pApp->SaveScanData())
									{
										SetMessage(IDS_STR_DATA_SAVED, CYAN_COLOR);
										UpdateGrid(m_pApp->m_stInspResultSaved);
									}
									else
										SetMessage(IDS_STR_DATA_NOT_SAVED);
									// Save Image
									if (m_pApp->m_stOptions.bSaveImage)
									{
										std::string str = CW2A(m_pApp->GetImagesDirectory().GetString());
										str += "\\Box.jpg";
										cv::imwrite(str.c_str(), m_MatColor);
									}
									if (m_pApp->m_stServerCommParams.bSendDataToServer)
									{
										IOutputLog(L"Setting event to log to server");
										SetEvent(m_pApp->m_hSendDataToServer);
										bWFServerResponse = TRUE;
										m_nImagesGrabbed = nNoOfFrames;
										break;
									}
									// Once data is sent to server we need to wait till a server response is received.
									//if (m_pApp->m_stOptions.bPrintLabel)// && m_bPrinterPortOpened)
									//{
									//	if (m_pApp->m_stPrintParams.bGetUserOKForPrinting)
									//	{
									//		if (MessageBox(L"Print Label?", L"Print Label?", MB_YESNO) == IDYES)
									//		{
									//			if (m_pApp->m_bPrintLabelThreadUp)
									//				SetEvent(m_pApp->m_hPrintLabel);
									//			else
									//			{
									//				PrintLabelCLExpress(m_pApp->m_stInspResultSaved);
									//				m_nImagesGrabbed = 100001;// breakout as otherwise it does not get printed.
									//			}
									//		}
									//	}
									//	else
									//	{
									//		if (m_pApp->m_bPrintLabelThreadUp)
									//			SetEvent(m_pApp->m_hPrintLabel);
									//		else
									//		{
									//			PrintLabelCLExpress(m_pApp->m_stInspResultSaved);
									//			m_nImagesGrabbed = 100001;// breakout as otherwise it does not get printed.
									//		}
									//		IOutputLog(L"ImageLiveStartOrbbec() - Label Printed");
									//	}
									//}
									// Clean up and make ready for next box!
									//m_pApp->m_stScanData.RemoveAllItems();
									//// Empty the code string as otherwise it will be mistaken for the next valid code scan trigger
									//m_pApp->m_strScannerString.Empty();
									//nNumImagesAfterDataSave = 0;
									//m_pApp->m_pFrame->m_strTotal.Empty();
								} // if (AreDimReadingsStable())
								else // Readings not stable. Grab another set of 10 images.
								{
									nNumImagesAfterCodeRcpt = 0;
									IOutputLog(L"Full barcode received, but dims not stable. Doing 10 more scans.");
								}
							} // if (nNumImagesAfterCodeRcpt >= 10)
						} // if Process3DDataOrbbec() returned TRUE and using m_pApp->m_stOptions.bUseCodeAsTrigger
					} // if (1)
				} // if (colorFrame.isValid())
				break;
			default:
				break;
			}
			//IOutputLog(L"At the end of loop in ImageLiveStartOrbbec()");
			while (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				::TranslateMessage(&msg);
				::DispatchMessage(&msg);
			}
			m_arRayTracingBoundaryPoints.RemoveAll();
			m_arRayTracingBoundaryPointsTransToPlat.RemoveAll();
		} while (m_pApp->m_device.isValid() && ((nNoOfFrames == 0) || (m_nImagesGrabbed < nNoOfFrames)) && !TimoutOccurred && m_b3DLive && !m_pFrame->m_bAbortScan);
	}
	catch (...)
	{
		IOutputLog(L"Error in measurement loop!");
		if ((m_pApp->m_stOptions.bSerialPortScanner) || (m_pApp->m_stOptions.bHikVisionScanner))
		{
			if (m_pApp->m_bScannerThreadUp)
				m_pApp->StopCodeScannerThread();
		}
		else
			m_pFrame->StopKBCapture();

		str.Format(L"%s", openni::OpenNI::getExtendedError());
		SetMessage(str);
	}
	if ((!m_pFrame->m_bAbortScan) && bWFServerResponse && m_pApp->m_stOptions.bPrintLabel)
	{
		t1 = m_HighResTimer.GetHighResElapsedTimeMsec();
		IOutputLog(L"Entering while loop for Server response");
		while (m_DataLoggingStatus == DATA_NOT_DONE)
		{
			while (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				::TranslateMessage(&msg);
				::DispatchMessage(&msg);
			}
			t2 = m_HighResTimer.GetHighResElapsedTimeMsec();
			if ((t2 - t1) > 5000)
			{
				m_DataLoggingStatus = DATA_FAIL;
				IOutputLog(L"Server response not recived in 5 secs. Breaking out!");
				break;
			}
			//Sleep(50);
		}		
		if (m_DataLoggingStatus == DATA_OK)
		{
			// If it comes here server response has been received within the max time limit
			if (m_pApp->m_stPrintParams.bGetUserOKForPrinting)
			{
				if (MessageBox(L"Print Label?", L"Print Label?", MB_YESNO) == IDYES)
				{
					if (m_pApp->m_bPrintLabelThreadUp)
						SetEvent(m_pApp->m_hPrintLabel);
					else
					{
						PrintLabelCLExpress(m_pApp->m_stInspResultSaved);
						m_nImagesGrabbed = 100001;// breakout as otherwise it does not get printed.
					}
				}
			}
			else
			{
				if (m_pApp->m_bPrintLabelThreadUp)
					SetEvent(m_pApp->m_hPrintLabel);
				else
				{
					PrintLabelCLExpress(m_pApp->m_stInspResultSaved);
					m_nImagesGrabbed = 100001;
				}
				IOutputLog(L"Label Printed");
			}
			// Reset data logging status
			m_DataLoggingStatus = DATA_NOT_DONE;
		}
		// Clean up and make ready for next box!
		m_pApp->m_stScanData.RemoveAllItems();
		// Empty the code string as otherwise it will be mistaken for the next valid code scan trigger
		m_pApp->m_strScannerString.Empty();
		nNumImagesAfterDataSave = 0;
		m_pApp->m_pFrame->m_strTotal.Empty();
	}
	m_bScanDone = TRUE;
	m_b3DLive = FALSE;
	if (m_nImagesGrabbed >= nNoOfFrames)
	{
		m_bAllFramesGrabbed = TRUE;
		SetMessage(IDS_STR_SCAN_OVER);
		m_pApp->m_pFrame->m_pImageView->m_bDepthScanDone = FALSE;// Ensure that data is saved just once for a successful scan
		IOutputLog(L"Requisite number of framses grabbed!");
	}
	if (!m_pFrame->m_bAbortScan)
	{
		Scan(); // To continue looping since it has come here only because (m_nImagesGrabbed >= nNoOfFrames)
	}
	return TRUE;
} // ImageLiveStart3DOrbbec()

  // Main function to process 3D Data Received, gets point cloud
  // KPD added parameter ImagesGrabbed to display on screen. can be commented out if not reuired
BOOL CPMSHJView::Process3DDataOrbbec(openni::VideoStream& depthStream, cv::Mat MatDepth, int ImagesGrabbed, BOOL bCuboidBox)
{
	//IOutputLog(L"nNew Scan");
	//return TRUE;
	openni::VideoFrameRef depthFrame;
	depthStream.readFrame(&depthFrame);
	MatDepth.data = (uchar*)depthFrame.getData();
	cv::Mat MatColor = m_MatColor;
	int nDrawingOffsetX = 0;
	int nDrawingOffsetY = 0;
	CString strMsg;

	m_bWeightScanDone = FALSE;
	double t1 = m_HighResTimer.GetHighResElapsedTimeMsec();
	double t2 = t1;
	double t3 = t1;
	m_ptHighest.x = 0;
	m_ptHighest.y = 0;

	BOOL bResult = TRUE;

	// Create OpenCV images from grabbed buffer
	const int width = MatDepth.cols;
	const int height = MatDepth.rows;
	const int count = width * height;

	cv::Mat OrigGrey;
	//cvConvert(MatColor, OrigGrey, );
	CvRect rectROI;
	rectROI.x = m_rectROI.left;
	rectROI.y = m_rectROI.top;
	rectROI.width = m_rectROI.Width();
	rectROI.height = m_rectROI.Height();
	// Modify mask image
	cvSetZero(m_imgMask);
	cvSetImageROI(m_imgMask, rectROI);
	cvSet(m_imgMask, 255);
	cvResetImageROI(m_imgMask);
	//cvSaveImage("d:\\MaskImage.bmp", m_imgMask);

	cv::Point ptMinVal, ptMaxVal;
	double dMinDist = FLT_MAX;
	double dMaxDist = 0;
	//double dSecondMin = FLT_MAX;
	// Get average depth at the center of m_rectCentralTarget
	double dMaxDistZ = m_pApp->m_stInspParams.d3DCameraHeight - m_pApp->m_stInspParams.nMinObjectHeight;
	double dMinDistZ = m_pApp->m_stInspParams.d3DCameraHeight - m_pApp->m_stInspParams.nMaxObjectHeight;
	dMinDistZ = 500;
	int nStartRow = m_rectCentralTarget.CenterPoint().y - m_pApp->m_stInspParams.nSpotRectSize;
	int nEndRow = m_rectCentralTarget.CenterPoint().y + m_pApp->m_stInspParams.nSpotRectSize;
	int nStartCol = m_rectCentralTarget.CenterPoint().x - m_pApp->m_stInspParams.nSpotRectSize;
	int nEndCol = m_rectCentralTarget.CenterPoint().x + m_pApp->m_stInspParams.nSpotRectSize;
	double dAvg = 0.0f;
	int nCount = 0;
	int nCountInvalidZ = 0;
	int rowSize = depthFrame.getStrideInBytes() / sizeof(openni::DepthPixel);
	if (depthFrame.isValid())
	{
		const openni::DepthPixel* pDepthRow = (const openni::DepthPixel*)depthFrame.getData();
		for (int row = nStartRow; row < nEndRow; ++row)
		{
			const openni::DepthPixel* pDepth = pDepthRow;
			for (int col = nStartCol; col < nEndCol; ++pDepth, col++)
			{
				int nOtherZ = *pDepth;
				uint16_t z = MatDepth.at<uint16_t>(row, col); // 
				if ((z > dMinDistZ) && (z < dMaxDistZ))// Check z validity
				{
					// Finding Min distance as minMaxLoc returns this as 0 for nan
					if (dMinDist > nOtherZ)
						dMinDist = nOtherZ;
					if (dMaxDist < nOtherZ)
						dMaxDist = nOtherZ;
					dAvg += z;
					nCount++;
				}
				else // Too many invalid readings
					nCountInvalidZ++;
			}
			pDepthRow += rowSize;
		}
		if (nCount > (m_rectCentralTarget.Width() * m_rectCentralTarget.Height() / 2))
			dAvg /= nCount;
		else
		{
			return FALSE;
		}
		if (nCountInvalidZ > (m_rectCentralTarget.Width() * m_rectCentralTarget.Height() / 4))
		{
			//IOutputLog(L"Too many invalid z readings");
			return FALSE;
		}
		if (bCuboidBox)// 090620 returning if too much variation in depth. Mostly happens if box is held at angle.
		{
			if ((dMaxDist - dMinDist) > 30)
			{
				//IOutputLog(L"Too much z variation");
				return FALSE;
			}
			else
			{
				strMsg.Format(L"z variation = %4.1f", dMaxDist - dMinDist);
				//IOutputLogString(strMsg);
			}
		}
		if (fabs(m_dAvgZ - dAvg) > 30)// If too much variation in consecutive readings the box must be moving (getting placed on platform)
		{
			m_dAvgZ = dAvg;
			IOutputLog(L"Too much variation bet old and new z");
			return FALSE;
		}
	}
	else // Depth frame not valid
		return FALSE;
	//strMsg.Format(L"dAvg = %4.1f", dAvg);
	//IOutputLogString(strMsg);
	cvResetImageROI(m_imgDepthGray);
	cvSetZero(m_imgDepthGray);
	cvResetImageROI(m_PlatformLevelGray);// This image will hold the image transformed to platform level
	cvSetZero(m_PlatformLevelGray);
	double dTransFactor = dAvg / m_pApp->m_stInspParams.d3DCameraHeight;
	// Binarize image within the rectROI - Set all bits which belong to the object to 255 and the rest to zero
	int nBoxPixelCount = 0;
	const openni::DepthPixel* pDepthRow = (const openni::DepthPixel*)depthFrame.getData();
	for (int row = 0; row < height; ++row)
	{
		const openni::DepthPixel* pDepth = pDepthRow;
		int idx = row * width;
		for (int col = 0; col < width; idx++, ++pDepth, col++)
		{
			double z = MatDepth.at<uint16_t>(row, col);
			z = *pDepth;
			dTransFactor = z / m_pApp->m_stInspParams.d3DCameraHeight;
			// This needs to be done thru parameter
			// we need to take x and y distance from the center of the camera
			double nColDelta = (320 - col) - (IMAGE_WIDTH / 2 - col) * dTransFactor;
			double nRowDelta = (240 - row) - (IMAGE_HEIGHT / 2 - row) * dTransFactor;
			int nColTrans = (int)(col + nColDelta + 0.5);
			int nRowTrans = (int)(row + nRowDelta + 0.5);
			if (bCuboidBox)
			{
				if ((z < (dAvg + 20)) && (z >(dAvg - 20)))//30 / 25
				{
					m_imgDepthGray->imageData[row * width + col] = 255;
					m_PlatformLevelGray->imageData[(int)(nRowTrans*width + nColTrans)] = 255;
					nBoxPixelCount++;
				}
			}
			else // Non-Cuboid (irregular object) 
			{
				// Collect all points which are less than a certain nMinObjectHeight above the platform
				if ((z < (m_pApp->m_stInspParams.d3DCameraHeight - m_pApp->m_stInspParams.nMinObjectHeight)) && (z > 400))
				{
					m_imgDepthGray->imageData[(int)(row*width + col)] = 255;
					m_PlatformLevelGray->imageData[(int)nRowTrans*width + nColTrans] = 255;
					nBoxPixelCount++;
				}
				else
					m_imgDepthGray->imageData[row * width + col] = 0;
			}
		}
		pDepthRow += rowSize;
	}
	//cvSaveImage("d:\\BeforeMaskingImage.bmp", m_imgDepthGray);
	//cvSaveImage("d:\\BeforeMaskingPlatformLevelImage.bmp", m_PlatformLevelGray);
	cvAnd(m_imgDepthGray, m_imgMask, m_imgDepthGray);
	//return TRUE;
	cvErode(m_imgDepthGray, m_imgDepthGray, NULL, 1);
	cvDilate(m_imgDepthGray, m_imgDepthGray, NULL, 1);// KPD - 151118
	//cvSaveImage("d:\\MaskedImage.bmp", m_imgDepthGray);
	// Using ray tracing to find points on part boundary
	BOOL bTraceMethodOK = TRUE;
	if (m_pApp->m_stInspParams.bUseTracingMethod)
	{
		double dMinDrop = GetMinDropForHeight(dAvg);
		dMinDrop /= 2;
		CvLineIterator itrTrace;
		int nSpotRectSize = m_pApp->m_stInspParams.nSpotRectSize;
		CvPoint Trpoint1, Trpoint2, Trpoint3, Trpoint4;
		Trpoint1.y = nStartRow + nSpotRectSize / 2;
		Trpoint1.x = nStartCol + nSpotRectSize / 2;
		int TraceDir = 0;
		int j = 0;
		int nInvalidEdgePoints = 0;
		for (int i = 0; i < m_arPeripheryPoints.GetSize(); i++)
		{
			Trpoint2.x = m_arPeripheryPoints[i].x;
			Trpoint2.y = m_arPeripheryPoints[i].y;
			nCount = cvInitLineIterator(m_imgDepthGray, Trpoint1, Trpoint2, &itrTrace, 8, TraceDir);
			int nOldZ = dAvg + 0.5;
			CvPoint ptNew, ptPrev;
			ptNew = ptPrev = Trpoint1;// To avoid crash if too many nan points in center
			BOOL bFoundZDrop = FALSE;
			for (int j = 0; j < nCount; j++)
			{
				int offset = itrTrace.ptr - (uchar*)(m_imgDepthGray->imageData);
				ptNew.y = offset / m_imgDepthGray->widthStep;
				ptNew.x = (offset - ptNew.y * m_imgDepthGray->widthStep) / (sizeof(uchar));
				double dNewZ = MatDepth.at<uint16_t>(ptNew.y, ptNew.x);
				if (dNewZ > 0)
				{
					if (bCuboidBox)
					{
						if (((dNewZ - nOldZ) > dMinDrop))
						{
							dTransFactor = nOldZ / m_pApp->m_stInspParams.d3DCameraHeight;
							bFoundZDrop = TRUE;
							break;
						}
					}
					else// non-cuboid - here the edge is when z exceeds min object height
					{
						if ((dNewZ > (m_pApp->m_stInspParams.d3DCameraHeight - m_pApp->m_stInspParams.nMinObjectHeight)) && (dNewZ > 400))
						{
							bFoundZDrop = TRUE;
							dTransFactor = nOldZ / m_pApp->m_stInspParams.d3DCameraHeight;
							break;
						}
					}
					ptPrev = ptNew;
					nOldZ = (int)(dNewZ + 0.5);
				}
				else // We are likely in the shadow region around the job. But it may also be a patch or hole on job which has invalid points.
				{
					ptNew = ptPrev;// Restore last valid point
					//bFoundZDrop = TRUE;
					//break;
				}
				CV_NEXT_LINE_POINT(itrTrace);
			}
			//if (bFoundZDrop)
			{
				m_arRayTracingBoundaryPoints.Add(CPoint(ptPrev.x, ptPrev.y));
				// we need to take x and y distance from the center of the camera	
				double dXDelta	= (320 - ptPrev.x) - (IMAGE_WIDTH / 2 - ptPrev.x) * dTransFactor;
				double dYDelta	= (240 - ptPrev.y) - (IMAGE_HEIGHT / 2 - ptPrev.y) * dTransFactor;
				int nXTrans		= ptPrev.x + dXDelta + 0.5;
				int nYwTrans	= ptPrev.y + dYDelta + 0.5;
				m_arRayTracingBoundaryPointsTransToPlat.Add(CPoint(nXTrans, nYwTrans));
			}
			//else // Whether or not we find the drop it is better to add the ptPrev as last valid point rather than the first point! 
			//{		// Especialy for big parts where all points after the edge of part and up to the end may be invalid
			//	m_arRayTracingBoundaryPoints.Add(CPoint(Trpoint1.x, Trpoint1.y));
			//	m_arRayTracingBoundaryPointsTransToPlat.Add(CPoint(Trpoint1.x, Trpoint1.y));
			//}
			if (j >= nCount)
				nInvalidEdgePoints++;
		}
		int nArraySize = m_arRayTracingBoundaryPoints.GetSize();
		if (nInvalidEdgePoints > (nArraySize / 8))
		{
			IOutputLog(L"Too many invalid edge points!");
			return FALSE;
		}
		//ValidateEdgePoints(m_arRayTracingBoundaryPoints, CPoint(Trpoint1.x, Trpoint1.y));
		//nArraySize = m_arRayTracingBoundaryPoints.GetSize();
		// Fill CvSeq of edge points
		CvMemStorage* storage = cvCreateMemStorage(0);
		CvMemStorage* storageTrans = cvCreateMemStorage(0);
		CvSeq* SeqEdgePoints = cvCreateSeq(CV_32SC2, sizeof(CvSeq), sizeof(CvPoint), storage);
		CvSeq* SeqEdgePointsTrans = cvCreateSeq(CV_32SC2, sizeof(CvSeq), sizeof(CvPoint), storage);
		for (int i = 0; i < m_arRayTracingBoundaryPoints.GetSize(); i++)
		{
			CvPoint pt;
			pt.x = m_arRayTracingBoundaryPoints[i].x;
			pt.y = m_arRayTracingBoundaryPoints[i].y;
			cvSeqPush(SeqEdgePoints, &pt);
			CvPoint ptT;
			if (i < m_arRayTracingBoundaryPointsTransToPlat.GetSize())
			{
				ptT.x = m_arRayTracingBoundaryPointsTransToPlat[i].x;
				ptT.y = m_arRayTracingBoundaryPointsTransToPlat[i].y;
				cvSeqPush(SeqEdgePointsTrans, &ptT);
			}
		}
		if (bCuboidBox)
		{
			m_Box2D = cvMinAreaRect2(SeqEdgePoints);
			if ((m_Box2D.size.width < m_pApp->m_stInspParams.nSpotRectSize) || (m_Box2D.size.height < m_pApp->m_stInspParams.nSpotRectSize))
			{
				bTraceMethodOK = FALSE;
				return FALSE;
			}
		}
		else
		{
			m_Box2DTransToPlat = cvMinAreaRect2(SeqEdgePointsTrans);
			if (!bCuboidBox && ((m_Box2DTransToPlat.size.width < m_pApp->m_stInspParams.nSpotRectSize) || 
				(m_Box2DTransToPlat.size.height < m_pApp->m_stInspParams.nSpotRectSize)))
			{
				bTraceMethodOK = FALSE;
				return FALSE;
			}
		}
		cvClearSeq(SeqEdgePoints);
		cvClearSeq(SeqEdgePointsTrans);
		cvClearMemStorage(storage);
		cvClearMemStorage(storageTrans);
		cvReleaseMemStorage(&storage);
		cvReleaseMemStorage(&storageTrans);
		//IOutputLog("Orig Box: %3.1f x %3.1f", m_Box2D.size.width, m_Box2D.size.height);
		//IOutputLog("Trans Box: %3.1f x %3.1f", m_Box2DTransToPlat.size.width, m_Box2DTransToPlat.size.height);
	}
	// if box is smaller than m_rectCentralTarget - return FALSE from here
	if (cvCountNonZero(m_imgDepthGray) < (m_rectCentralTarget.Width() * m_rectCentralTarget.Height()))
		return FALSE;
	// Do blob analysis if Trace method not selected or has failed
	//IOutputLog("Ray Tracing Done");
	if (!m_pApp->m_stInspParams.bUseTracingMethod || !bTraceMethodOK)
	{
		CBlobResult blobResult(m_imgDepthGray, NULL, 0);
		int nNumBlobs = blobResult.GetNumBlobs();
		if (nNumBlobs == 0)
		{
			//IOutputLog("No blobs found");
			return FALSE;
		}
		//cvSaveImage("d:\\BinImage.bmp", m_imgDepthGray);
		CBlob blobLargest;
		CvRect rectCvBox;
		CRect rectBox;
		CRect rectIntersect;
		CvScalar scalar(0.0);
		BOOL bIsIntersection = FALSE;

		// Get the largest blob which contains the m_rectCentralTarget
		if (nNumBlobs > 0)
		{
			blobResult.GetNthBlob(CBlobGetArea(), 0, blobLargest);
		}
		t_PointList BoundaryPoints = blobLargest.GetConvexHull();
		m_Box2D = cvMinAreaRect2((CvArr*)BoundaryPoints);
	}
	//IOutputLog("After BlobAnalyssis");
	CvPoint2D32f pt[4];
	cvBoxPoints(m_Box2D, pt);
	CvPoint2D32f ptTrans[4];
	cvBoxPoints(m_Box2DTransToPlat, ptTrans);
	float dBoxAngle = m_Box2D.angle;
	CvPoint point1, point2, point3, point4, OldPoint1, OldPoint2, OldPoint3, OldPoint4;
	// point1,2,3 and 4 will be modified subsequently so they will be  points at which valid X, Y and Z readings in mm are found
	// Store points in OldPoints as these will be used later to calculate box dims as well as for drawing
	// Also, DrawingPoints which will be used to draw box boundary on image.
	CvPoint DrawingPoint1, DrawingPoint2, DrawingPoint3, DrawingPoint4;
	DrawingPoint1.x = OldPoint1.x = point1.x = pt[0].x;
	DrawingPoint1.y = OldPoint1.y = point1.y = pt[0].y;
	DrawingPoint2.x = OldPoint2.x = point2.x = pt[1].x;
	DrawingPoint2.y = OldPoint2.y = point2.y = pt[1].y;
	DrawingPoint3.x = OldPoint3.x = point3.x = pt[2].x;
	DrawingPoint3.y = OldPoint3.y = point3.y = pt[2].y;
	DrawingPoint4.x = OldPoint4.x = point4.x = pt[3].x;
	DrawingPoint4.y = OldPoint4.y = point4.y = pt[3].y;

	// If box is going out of image area return FALSE
	if ((DrawingPoint1.x < 0) || (DrawingPoint1.x > 640) ||
		(DrawingPoint1.y < 0) || (DrawingPoint1.y > 480) ||
		(DrawingPoint2.x < 0) || (DrawingPoint2.x > 640) ||
		(DrawingPoint2.y < 0) || (DrawingPoint2.y > 480) ||
		(DrawingPoint3.x < 0) || (DrawingPoint3.x > 640) ||
		(DrawingPoint3.y < 0) || (DrawingPoint3.y > 480) ||
		(DrawingPoint4.x < 0) || (DrawingPoint4.x > 640) ||
		(DrawingPoint4.y < 0) || (DrawingPoint4.y > 480))
		return FALSE;
	//TRACE("Point1X: %d,Y: %d\n", point1.x, point1.y);
	//TRACE("Point2X: %d,Y: %d\n", point2.x, point2.y);
	//TRACE("Point3X: %d,Y: %d\n", point3.x, point3.y);
	//TRACE("Point4X: %d,Y: %d\n", point4.x, point4.y);
	m_bBoxResultReady = TRUE;

	CvPoint point1Trans, point2Trans, point3Trans, point4Trans;
	if (!bCuboidBox && (m_pApp->m_stInspParams.bUseTracingMethod && bTraceMethodOK))
	{
		OldPoint1.x = point1Trans.x = point1.x = cvRound(ptTrans[0].x);
		OldPoint1.y = point1Trans.y = point1.y = cvRound(ptTrans[0].y);
		OldPoint2.x = point2Trans.x = point2.x = cvRound(ptTrans[1].x);
		OldPoint2.y = point2Trans.y = point2.y = cvRound(ptTrans[1].y);
		OldPoint3.x = point3Trans.x = point3.x = cvRound(ptTrans[2].x);
		OldPoint3.y = point3Trans.y = point3.y = cvRound(ptTrans[2].y);
		OldPoint4.x = point4Trans.x = point4.x = cvRound(ptTrans[3].x);
		OldPoint4.y = point4Trans.y = point4.y = cvRound(ptTrans[3].y);

		// we need to take x and y distance from the center of the camera
		double dXDelta = (320 - point1.x) - (IMAGE_WIDTH / 2 - point1.x) / dTransFactor;
		double dYDelta = (240 - point1.y) - (IMAGE_HEIGHT / 2 - point1.y) / dTransFactor;
		DrawingPoint1.x = point1.x + dXDelta + 0.5;
		DrawingPoint1.y = point1.y + dYDelta + 0.5;

		dXDelta = (320 - point2.x) - (IMAGE_WIDTH / 2 - point2.x) / dTransFactor;
		dYDelta = (240 - point2.y) - (IMAGE_HEIGHT / 2 - point2.y) / dTransFactor;
		DrawingPoint2.x = point2.x + dXDelta + 0.5;
		DrawingPoint2.y = point2.y + dYDelta + 0.5;

		dXDelta = (320 - point3.x) - (IMAGE_WIDTH / 2 - point3.x) / dTransFactor;
		dYDelta = (240 - point3.y) - (IMAGE_HEIGHT / 2 - point3.y) / dTransFactor;
		DrawingPoint3.x = point3.x + dXDelta + 0.5;
		DrawingPoint3.y = point3.y + dYDelta + 0.5;

		dXDelta = (320 - point4.x) - (IMAGE_WIDTH / 2 - point4.x) / dTransFactor;
		dYDelta = (240 - point4.y) - (IMAGE_HEIGHT / 2 - point4.y) / dTransFactor;
		DrawingPoint4.x = point4.x + dXDelta + 0.5;
		DrawingPoint4.y = point4.y + dYDelta + 0.5;
	}
	else // box was not formed for transformed points
	{
		m_arRayTracingBoundaryPointsTransToPlat.RemoveAll();
		double dXDelta	= (320 - point1.x) - (IMAGE_WIDTH / 2 - point1.x) * dTransFactor;
		double dYDelta	= (240 - point1.y) - (IMAGE_HEIGHT / 2 - point1.y) * dTransFactor;
		int nXTrans		= point1.x + dXDelta + 0.5;
		int nYwTrans	= point1.y + dYDelta + 0.5;
		m_arRayTracingBoundaryPointsTransToPlat.Add(CPoint(nXTrans, nYwTrans));

		dXDelta		= (320 - point2.x) - (IMAGE_WIDTH / 2 - point2.x) * dTransFactor;
		dYDelta		= (240 - point2.y) - (IMAGE_HEIGHT / 2 - point2.y) * dTransFactor;
		nXTrans		= point2.x + dXDelta + 0.5;
		nYwTrans	= point2.y + dYDelta + 0.5;
		m_arRayTracingBoundaryPointsTransToPlat.Add(CPoint(nXTrans, nYwTrans));

		dXDelta		= (320 - point3.x) - (IMAGE_WIDTH / 2 - point3.x) * dTransFactor;
		dYDelta		= (240 - point3.y) - (IMAGE_HEIGHT / 2 - point3.y) * dTransFactor;
		nXTrans		= point3.x + dXDelta + 0.5;
		nYwTrans	= point3.y + dYDelta + 0.5;
		m_arRayTracingBoundaryPointsTransToPlat.Add(CPoint(nXTrans, nYwTrans));

		dXDelta		= (320 - point4.x) - (IMAGE_WIDTH / 2 - point4.x) * dTransFactor;
		dYDelta		= (240 - point4.y) - (IMAGE_HEIGHT / 2 - point4.y) * dTransFactor;
		nXTrans		= point4.x + dXDelta + 0.5;
		nYwTrans	= point4.y + dYDelta + 0.5;
		m_arRayTracingBoundaryPointsTransToPlat.Add(CPoint(nXTrans, nYwTrans));
		//IOutputLog("Trace method NOT OK");
	}

	//return TRUE;
	// Box corners may not have valid 3D coordinates. Move corner points inward along the diagonals
	CvLineIterator iterator;
	int connectivity = 8;
	int left_to_right = 0;// Setting it true (1) will force left to right point collection
	nCount = cvInitLineIterator(m_imgDepthGray, point1, point3, &iterator, connectivity, left_to_right);
	// point1
	CvPoint ptNew = point1;
	int nValidPoints = 0;
	// Get box diagonal length
	double dDiag = sqrt((point1.x - point3.x) * (point1.x - point3.x) + (point1.y - point3.y) * (point1.y - point3.y));
	int nMaxPossibleValidPoints = dDiag / 20;
	int nMaxValidPoints = min(nMaxPossibleValidPoints, 2);//2
	int offset;
	for (int i = 0; i < nCount; i++)
	{
		offset = iterator.ptr - (uchar*)(m_imgDepthGray->imageData);
		ptNew.y = offset / m_imgDepthGray->widthStep;
		ptNew.x = (offset - ptNew.y * m_imgDepthGray->widthStep) / (sizeof(uchar));
		double dTemp = MatDepth.at<uint16_t>(ptNew.y, ptNew.x);
		unsigned char PixVal = m_imgDepthGray->imageData[offset];
		if (!isnan(dTemp) && (PixVal != 0))
		{
			point1 = ptNew;
			nValidPoints++;
			if (nValidPoints > nMaxValidPoints)
				break;
		}
		CV_NEXT_LINE_POINT(iterator);
	}
	// point3
	ptNew = point3;
	nCount = cvInitLineIterator(m_imgDepthGray, point3, point1, &iterator, connectivity, left_to_right);
	nValidPoints = 0;
	for (int i = 0; i < nCount; i++)
	{
		offset = iterator.ptr - (uchar*)(m_imgDepthGray->imageData);
		ptNew.y = offset / m_imgDepthGray->widthStep;
		ptNew.x = (offset - ptNew.y * m_imgDepthGray->widthStep) / (sizeof(uchar));
		double dTemp = MatDepth.at<uint16_t>(ptNew.y, ptNew.x);
		unsigned char PixVal = m_imgDepthGray->imageData[offset];
		if (!isnan(dTemp) && (PixVal != 0))
		{
			point3 = ptNew;
			nValidPoints++;
			if (nValidPoints > nMaxValidPoints)
				break;
		}
		CV_NEXT_LINE_POINT(iterator);
	}
	// point2
	ptNew = point2;
	nCount = cvInitLineIterator(m_imgDepthGray, point2, point4, &iterator, connectivity, left_to_right);
	nValidPoints = 0;
	for (int i = 0; i < nCount; i++)
	{
		offset = iterator.ptr - (uchar*)(m_imgDepthGray->imageData);
		ptNew.y = offset / m_imgDepthGray->widthStep;
		ptNew.x = (offset - ptNew.y * m_imgDepthGray->widthStep) / (sizeof(uchar));
		double dTemp = MatDepth.at<uint16_t>(ptNew.y, ptNew.x);
		unsigned char PixVal = m_imgDepthGray->imageData[offset];
		if (!isnan(dTemp) && (PixVal != 0))
		{
			point2 = ptNew;
			nValidPoints++;
			if (nValidPoints > nMaxValidPoints)
				break;
		}
		CV_NEXT_LINE_POINT(iterator);
	}
	// point4
	ptNew = point4;
	nCount = cvInitLineIterator(m_imgDepthGray, point4, point2, &iterator, connectivity, left_to_right);
	nValidPoints = 0;
	for (int i = 0; i < nCount; i++)
	{
		offset = iterator.ptr - (uchar*)(m_imgDepthGray->imageData);
		ptNew.y = offset / m_imgDepthGray->widthStep;
		ptNew.x = (offset - ptNew.y * m_imgDepthGray->widthStep) / (sizeof(uchar));
		double dTemp = MatDepth.at<uint16_t>(ptNew.y, ptNew.x);
		unsigned char PixVal = m_imgDepthGray->imageData[offset];
		if (!isnan(dTemp) && (PixVal != 0))
		{
			point4 = ptNew;
			nValidPoints++;
			if (nValidPoints > nMaxValidPoints)
				break;
		}
		CV_NEXT_LINE_POINT(iterator);
	}
	m_arMeasurementPoints.RemoveAll();
	if (bCuboidBox)
	{
		m_arMeasurementPoints.Add(CPoint(point1.x, point1.y));
		m_arMeasurementPoints.Add(CPoint(point2.x, point2.y));
		m_arMeasurementPoints.Add(CPoint(point3.x, point3.y));
		m_arMeasurementPoints.Add(CPoint(point4.x, point4.y));
	}
	else
	{
		m_arMeasurementPoints.Add(CPoint(point1Trans.x, point1Trans.y));
		m_arMeasurementPoints.Add(CPoint(point2Trans.x, point2Trans.y));
		m_arMeasurementPoints.Add(CPoint(point3Trans.x, point3Trans.y));
		m_arMeasurementPoints.Add(CPoint(point4Trans.x, point4Trans.y));
	}
	t2 = m_HighResTimer.GetHighResElapsedTimeMsec();
#ifndef _DEBUG
	//if ((t2 - t1) > 20)// Time for total process is ususally around 5 msec (P5, 8 GB)
	//	return FALSE;
#endif
	//IOutputLog("Time for Shifting corner points in - %2.3f", (t2 - t1));
	//t1 = t2;

	//IOutputLog("After Digonal tracing");
	// Get real world distance coordinate of this point
	// X, Y distances are stored inr col, row format
	float x1, y1, x2, y2, x3, y3, x4, y4;
	cv::Point3f ptMVS;
	uint16_t z = 0;
	if (bCuboidBox)
	{
		z = MatDepth.at<uint16_t>(point1.y, point1.x);
		openni::CoordinateConverter::convertDepthToWorld(depthStream, point1.x, point1.y, z, &ptMVS.x, &ptMVS.y, &ptMVS.z);
		x1 = ptMVS.x;
		y1 = ptMVS.y;
		z = MatDepth.at<uint16_t>(point2.y, point2.x);
		openni::CoordinateConverter::convertDepthToWorld(depthStream, point2.x, point2.y, z, &ptMVS.x, &ptMVS.y, &ptMVS.z);
		x2 = ptMVS.x;
		y2 = ptMVS.y;
		z = MatDepth.at<uint16_t>(point3.y, point3.x);
		openni::CoordinateConverter::convertDepthToWorld(depthStream, point3.x, point3.y, z, &ptMVS.x, &ptMVS.y, &ptMVS.z);
		x3 = ptMVS.x;
		y3 = ptMVS.y;
		z = MatDepth.at<uint16_t>(point4.y, point4.x);
		openni::CoordinateConverter::convertDepthToWorld(depthStream, point4.x, point4.y, z, &ptMVS.x, &ptMVS.y, &ptMVS.z);
		x4 = ptMVS.x;
		y4 = ptMVS.y;
	}
	else
	{
		z = m_pApp->m_stInspParams.d3DCameraHeight;
		openni::CoordinateConverter::convertDepthToWorld(depthStream, point1Trans.x, point1Trans.y, z, &ptMVS.x, &ptMVS.y, &ptMVS.z);
		x1 = ptMVS.x;
		y1 = ptMVS.y;
		openni::CoordinateConverter::convertDepthToWorld(depthStream, point2Trans.x, point2Trans.y, z, &ptMVS.x, &ptMVS.y, &ptMVS.z);
		x2 = ptMVS.x;
		y2 = ptMVS.y;
		openni::CoordinateConverter::convertDepthToWorld(depthStream, point3Trans.x, point3Trans.y, z, &ptMVS.x, &ptMVS.y, &ptMVS.z);
		x3 = ptMVS.x;
		y3 = ptMVS.y;
		openni::CoordinateConverter::convertDepthToWorld(depthStream, point4Trans.x, point4Trans.y, z, &ptMVS.x, &ptMVS.y, &ptMVS.z);
		x4 = ptMVS.x;
		y4 = ptMVS.y;
	}
	//TRACE("MM1X: %d,Y: %d\n", x1, y1);
	//TRACE("MM2X: %d,Y: %d\n", x2, y2);
	//TRACE("MM3X: %d,Y: %d\n", x3, y3);
	//TRACE("MM4X: %d,Y: %d\n", x4, y4);

	double distance_12 = sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
	double distance_23 = sqrt((x2 - x3) * (x2 - x3) + (y2 - y3) * (y2 - y3));
	double distance_34 = sqrt((x3 - x4) * (x3 - x4) + (y3 - y4) * (y3 - y4));
	double distance_41 = sqrt((x4 - x1) * (x4 - x1) + (y4 - y1) * (y4 - y1));
	double distance_13 = sqrt((x1 - x3) * (x1 - x3) + (y1 - y3) * (y1 - y3));
	double distance_24 = sqrt((x2 - x4) * (x2 - x4) + (y2 - y4) * (y2 - y4));

	double distance_12Trans = 0.0f; double distance_23Trans = 0.0f; double distance_34Trans = 0.0f; double distance_41Trans = 0.0f;
	double distance_12TransOld = 0.0f; double distance_23TransOld = 0.0f; double distance_34TransOld = 0.0f; double distance_41TransOld = 0.0f;

	if (bCuboidBox)
	{
		// Need to apply correction if corner points have been pulled in to get valid readings
		double mmPerPixel12 = distance_12 / (sqrt((point1.x - point2.x) * (point1.x - point2.x) + (point1.y - point2.y) * (point1.y - point2.y)));
		double mmPerPixel23 = distance_23 / (sqrt((point2.x - point3.x) * (point2.x - point3.x) + (point2.y - point3.y) * (point2.y - point3.y)));
		double mmPerPixel34 = distance_34 / (sqrt((point3.x - point4.x) * (point3.x - point4.x) + (point3.y - point4.y) * (point3.y - point4.y)));
		double mmPerPixel41 = distance_41 / (sqrt((point4.x - point1.x) * (point4.x - point1.x) + (point4.y - point1.y) * (point4.y - point1.y)));
		double mmPerPixelAvg = (mmPerPixel12 + mmPerPixel23 + mmPerPixel34 + mmPerPixel41) / 4.0;

		double dDist12Pix = sqrt((OldPoint1.x - OldPoint2.x) * (OldPoint1.x - OldPoint2.x) + (OldPoint1.y - OldPoint2.y) * (OldPoint1.y - OldPoint2.y));
		distance_12 = dDist12Pix * mmPerPixelAvg;// mmPerPixel12;

		double dDist23Pix = sqrt((OldPoint2.x - OldPoint3.x) * (OldPoint2.x - OldPoint3.x) + (OldPoint2.y - OldPoint3.y) * (OldPoint2.y - OldPoint3.y));
		distance_23 = dDist23Pix * mmPerPixelAvg;// mmPerPixel23;

		double dDist34Pix = sqrt((OldPoint3.x - OldPoint4.x) * (OldPoint3.x - OldPoint4.x) + (OldPoint3.y - OldPoint4.y) * (OldPoint3.y - OldPoint4.y));
		distance_34 = dDist34Pix * mmPerPixelAvg;// mmPerPixel34;

		double dDist41Pix = sqrt((OldPoint4.x - OldPoint1.x) * (OldPoint4.x - OldPoint1.x) + (OldPoint4.y - OldPoint1.y) * (OldPoint4.y - OldPoint1.y));
		distance_41 = dDist41Pix * mmPerPixelAvg;// mmPerPixel41;
	}

	//TRACE("\n dDist12Pix: %d, dDist23Pix: %d, dDist34Pix: %d , dDist41Pix: %d \n", dDist12Pix, dDist23Pix, dDist34Pix, dDist41Pix);
	//TRACE("\n mmPerPixel12: %d, mmPerPixel23: %d, mmPerPixel34: %d , mmPerPixel41: %d \n", mmPerPixel12, mmPerPixel23, mmPerPixel34, mmPerPixel41);
	//
	m_arMeasurementPoints.RemoveAll();
	m_arMeasurementPoints.Add(CPoint(DrawingPoint1.x, DrawingPoint1.y));
	m_arMeasurementPoints.Add(CPoint(DrawingPoint2.x, DrawingPoint2.y));
	m_arMeasurementPoints.Add(CPoint(DrawingPoint3.x, DrawingPoint3.y));
	m_arMeasurementPoints.Add(CPoint(DrawingPoint4.x, DrawingPoint4.y));
	
	double Big1 = 0;
	// Take care of invalid results!
	if ((x1 > 9000) || (y1 > 9000) || (x2 > 9000) || (y2 > 9000))
	{
		distance_12 = 0;
		distance_12Trans = 0;
	}
	if ((x3 > 9000) || (y3 > 9000) || (x4 > 9000) || (y4 > 9000))
	{
		distance_34 = 0;
		distance_34Trans = 0;
	}
	//
	if ((distance_12 > EPSILON) && (distance_34 > EPSILON))
	{
		if (distance_12 > distance_34)
			Big1 = distance_12;
		else
			Big1 = distance_34;
	}
	else if (distance_12 > EPSILON)
		Big1 = distance_12;
	else if (distance_34 > EPSILON)
		Big1 = distance_34;

	double Big2 = 0;
	if ((x2 > 9000) || (y2 > 9000) || (x3 > 9000) || (y3 > 9000))
		distance_23 = 0;
	if ((x4 > 9000) || (y4 > 9000) || (x1 > 9000) || (y1 > 9000))
		distance_41 = 0;

	if ((distance_23 > EPSILON) && (distance_41 > EPSILON))
	{
		if (distance_23 > distance_41)
			Big2 = distance_23;
		else
			Big2 = distance_41;
	}
	else if (distance_23 > EPSILON)
		Big2 = distance_23;
	else if (distance_41 > EPSILON)
		Big2 = distance_41;
	else// both long distances are below EPSILON
		return FALSE;

	if (bCuboidBox)
	{
		Big1 -= 2;// Was showing 5 mm OS
		Big2 -= 2;
	}
	else
	{
		Big1 -= 5;// Was showing 5 mm OS
		Big2 -= 5;
	}
	//

	// Get Avg Height
	// Mask out background leaving only the blob at 255
	CvScalar color = cvScalarAll(255.0);
	cvSet(m_imgMask, color);
	// Draw box border
	color = cvScalarAll(0.0);
	cvLine(m_imgMask, OldPoint1, OldPoint2, color, 2);
	cvLine(m_imgMask, OldPoint2, OldPoint3, color, 2);
	cvLine(m_imgMask, OldPoint3, OldPoint4, color, 2);
	cvLine(m_imgMask, OldPoint4, OldPoint1, color, 2);
	cvFloodFill(m_imgMask, CvPoint(5, 5), color);
	cvErode(m_imgMask, m_imgMask, NULL, 2);
	// AND with image used to compute X, Y dimensions
	double dTotal = 0.0, dMinDepth = FLT_MAX;
	nCount = 0;
	//cvSaveImage("d:\\ObjectMaskImage.bmp", m_imgMask);
	cvAnd(m_imgDepthGray, m_imgMask, m_imgDepthGray);
	cvErode(m_imgDepthGray, m_imgDepthGray, NULL, 2);
	//cvSaveImage("d:\\ObjectMaskedImage.bmp", m_imgDepthGray);
	m_arZReadings.RemoveAll();
	// Find average height. Fill an array of heights of all points belonging to the object and then find average and later refine it.
	for (int i = rectROI.y; i < (rectROI.y + rectROI.height); i++)
	{
		int idx = i * width;
		idx += rectROI.x;
		for (int j = rectROI.x; j < (rectROI.x + rectROI.width); ++idx, j++)
		{
			int nIndex = i * m_imgDepthGray->widthStep + j;
			unsigned char temp = m_imgDepthGray->imageData[nIndex];
			if (temp != 0)
			{
				double z = MatDepth.at<uint16_t>(i, j); // ZDistanceMap.at<double>(i, j);
				//if (bCuboidBox)
				//{
					if ((z > dMinDistZ) && (z < dMaxDistZ))
					{
						m_arZReadings.Add(z);
						dTotal += z;
						nCount++;
					}
				//}
				if (!bCuboidBox && (dMinDepth > z))
				{
					dMinDepth = z;
					m_ptHighest.x = j;
					m_ptHighest.y = i;
				}
			}
		}
	}
	t3 = m_HighResTimer.GetHighResElapsedTimeMsec();
#ifndef _DEBUG
	//if ((t3 - t1) > 30)// Time for total process is ususally around 5 msec (P5, 8 GB)
	//	return FALSE;
#endif
	//IOutputLog("Time for getting average box height for blob pixels - %2.3f", (t2 - t1));
	//t1 = t2;
	double dCorrectedHeight = 0.0f;
	float fCompValZ = 0;
	BOOL bDimsStable = TRUE;// Will be used to decide if necessary to draw box boundary
	int nLowerHt = 0;
	int nUpperHt = 0;
	double dReportedHtMM = 0.0f;
	double dRawAvg = 0.0f;
	if (nCount > 0)
	{
		dRawAvg = dTotal / nCount;
		//IOutputLog("dRawAvg = %4.1f", dRawAvg);
	}
	else
		return FALSE;
	if (bCuboidBox)
	{
		double dAvgDepth = GetRefinedZVal(dRawAvg, m_arZReadings, 5);// Refines Z value by removing outliers
		dCorrectedHeight = m_pApp->m_stInspParams.d3DCameraHeight - dAvgDepth;
		// Temp Correction - KPD 161118
		dCorrectedHeight += fCompValZ;
		m_arHeight.Add(dCorrectedHeight);
		double dMinHeight = FLT_MAX;
		double dMaxHeight = FLT_MIN;
		if (m_arHeight.GetSize() > 15)
		{
			m_arHeight.RemoveAt(0);
			double dTotal = 0.0f;
			for (int i = 0; i < m_arHeight.GetSize(); i++)
				dTotal += m_arHeight[i];
			dCorrectedHeight = dTotal / m_arHeight.GetSize();
		}

		if (m_pApp->m_stOptions.DimUnits == DIM_CM)
			dCorrectedHeight /= 10;
		else if (m_pApp->m_stOptions.DimUnits == DIM_M)
			dCorrectedHeight /= 1000;
		else if (m_pApp->m_stOptions.DimUnits == DIM_INCH)
			dCorrectedHeight /= 25.4;
		if (nCount > 0)
		{
			if (fabs(dCorrectedHeight - m_dOldHeight) > m_pApp->m_stInspParams.nDsplAccuracy)
			{
				m_dOldHeight = dCorrectedHeight;
			}
			else
				dCorrectedHeight = m_dOldHeight;
			//
			m_strBoxHeight.Format(m_strAccuracy, dCorrectedHeight);
		}
		else
			m_strBoxHeight.Format(L"Invalid Ht Readings!");
	}
	else // Non cuboid irregular package
	{
		double dMaxHeight = m_pApp->m_stInspParams.d3DCameraHeight - dMinDepth;
		double dAvgHt = m_pApp->m_stInspParams.d3DCameraHeight - dRawAvg;
		if (fabs(dAvgHt - dMaxHeight) > 30) // If not too much variation looks to be a cubical box, Otherwise an irregular object.
			dCorrectedHeight = dMaxHeight;
		else
		{
			//IOutputLog(L"Box found cuboid");	
			dCorrectedHeight = dAvgHt;
		}
		//KPD - 140520
		if (fabs(dCorrectedHeight - m_dOldHeight) > m_pApp->m_stInspParams.nDsplAccuracy)
			m_dOldHeight = dCorrectedHeight;
		else
			dCorrectedHeight = dMaxHeight = m_dOldHeight;
		//
		if (m_pApp->m_stOptions.DimUnits == DIM_CM)
			dMaxHeight /= 10;
		else if (m_pApp->m_stOptions.DimUnits == DIM_M)
			dMaxHeight /= 1000;
		else if (m_pApp->m_stOptions.DimUnits == DIM_INCH)
			dMaxHeight /= 25.4;
		else if (m_pApp->m_stOptions.DimUnits == DIM_FT)
			dMaxHeight /= (25.4 * 12);

		m_strBoxHeight.Format(m_strAccuracy, dMaxHeight);
	}
	ApplyXYorrection(Big1, Big2, dCorrectedHeight);
	// Swap if Big1 is smaller
	if (Big1 < Big2)
	{
		double dTemp = Big1;
		Big1 = Big2;
		Big2 = dTemp;
	}
	// Take average of last 10 readings for  length and width
	m_arLength.Add(Big1);
	if (m_arLength.GetSize() > 10)
		m_arLength.RemoveAt(0);
	m_arWidth.Add(Big2);
	if (m_arWidth.GetSize() > 10)
		m_arWidth.RemoveAt(0);
	dTotal = 0.0f;
	for (int i = 0; i < m_arLength.GetSize(); i++)
	{
		double dTemp = m_arLength[i];
		dTotal += m_arLength[i];
	}
	dTotal /= m_arLength.GetSize();
	if (fabs(dTotal - m_dOldLength) > m_pApp->m_stInspParams.nDsplAccuracy)
	{
		Big1 = m_dOldLength = dTotal;
	}
	else
		Big1 = m_dOldLength;
	dTotal = 0.0f;
	for (int i = 0; i < m_arWidth.GetSize(); i++)
		dTotal += m_arWidth[i];
	dTotal /= m_arWidth.GetSize();
	//
	if (fabs(dTotal - m_dOldWidth) > m_pApp->m_stInspParams.nDsplAccuracy)
	{
		Big2 = m_dOldWidth = dTotal;
	}
	else
		Big2 = m_dOldWidth;
	m_strBoxLength.Format(L"%3.0f", Big1);
	m_strBoxWidth.Format(L"%3.0f", Big2);
	if (m_pApp->m_stOptions.DimUnits == DIM_CM)
	{
		double Big1Cm, Big2Cm;
		Big1Cm = Big1 / 10;
		Big2Cm = Big2 / 10;
		m_strBoxLength.Format(m_strAccuracy, Big1Cm);
		m_strBoxWidth.Format(m_strAccuracy, Big2Cm);
	}
	else if (m_pApp->m_stOptions.DimUnits == DIM_M)
	{
		double Big1m, Big2m;
		Big1m = Big1 / 1000;
		Big2m = Big2 / 1000;
		m_strBoxLength.Format(m_strAccuracy, Big1m);
		m_strBoxWidth.Format(m_strAccuracy, Big2m);
	}
	else if (m_pApp->m_stOptions.DimUnits == DIM_INCH)
	{
		double Big1Inch, Big2Inch;
		Big1Inch = Big1 / 25.4;
		Big2Inch = Big2 / 25.4;
		m_strBoxLength.Format(m_strAccuracy, Big1Inch);
		m_strBoxWidth.Format(m_strAccuracy, Big2Inch);
	}

	//TRACE("Length: %s, Width: %s, Height: %s\n", m_strBoxLength, m_strBoxWidth, m_strBoxHeight);
	CString strWeight, strVolume;

	// Convert height (which may be in mm, cm or M) to mm
	double dHtMM = dCorrectedHeight;
	if (m_pApp->m_stOptions.DimUnits == DIM_CM)
		dHtMM = dCorrectedHeight * 10;
	else if (m_pApp->m_stOptions.DimUnits == DIM_M)
		dHtMM = dCorrectedHeight * 1000;
	m_dBoxVolume = m_pApp->m_stInspResult.dVolume = (dHtMM*Big1*Big2) / (1000 * 1000 * 1000);
	if (m_pApp->m_stInspResult.dVolume < 1.0)
		strVolume.Format(L"%1.3f", m_pApp->m_stInspResult.dVolume);
	else
		strVolume.Format(L"%2.2f", m_pApp->m_stInspResult.dVolume);
	m_strBoxVolume = strVolume;
	ShowDimensions(m_strBoxLength, m_strBoxWidth, m_strBoxHeight, strWeight, m_strBoxVolume);
	m_dBoxLength	= m_pApp->m_stInspResult.dLength	= Big1;
	m_dBoxWidth		= m_pApp->m_stInspResult.dWidth		= Big2;
	m_dBoxHeight	= m_pApp->m_stInspResult.dHeight	= dCorrectedHeight;
	m_pApp->m_stInspResult.bDimensionReadSuccess = TRUE;
	m_pApp->m_stInspResult.bResultReady = TRUE;
	//return TRUE;

	ASSERT(m_arMeasurementPoints.GetSize() == 4);
	// Draw job boundary
	for (int i = 0; i < (m_arMeasurementPoints.GetSize() - 1); i++)
	{
		cv::Point pt(m_arMeasurementPoints[i].x, m_arMeasurementPoints[i].y);
		line(MatColor, cv::Point(m_arMeasurementPoints[i].x - nDrawingOffsetX , m_arMeasurementPoints[i].y), cv::Point(m_arMeasurementPoints[i + 1].x - nDrawingOffsetX, m_arMeasurementPoints[i + 1].y), cv::Scalar(0, 255, 0), 2, 8);
	}
	//strMsg.Format(L"L-%3.0f, W-%3.0f", m_dBoxLength, m_dBoxWidth);
	//IOutputLogString(strMsg);
	// Close job box
	line(MatColor, cv::Point(m_arMeasurementPoints[0].x - nDrawingOffsetX , m_arMeasurementPoints[0].y), cv::Point(m_arMeasurementPoints[m_arMeasurementPoints.GetSize() - 1].x - nDrawingOffsetX, m_arMeasurementPoints[m_arMeasurementPoints.GetSize() - 1].y), cv::Scalar(0, 255, 0), 2, 8);
	// Draw central target box. This serves as indication that the box placement is ok. If it is not user needs to adjust it.
	DrawCentralTargetRect(MatColor, cv::Scalar(255, 0, 0));
	//int TopLeftX = m_rectCentralTarget.CenterPoint().x - m_pApp->m_stInspParams.nSpotRectSize;
	//int TopLeftY = m_rectCentralTarget.CenterPoint().y - m_pApp->m_stInspParams.nSpotRectSize;
	//int BtmRtX = m_rectCentralTarget.CenterPoint().x + m_pApp->m_stInspParams.nSpotRectSize;
	//int BtmRtY = m_rectCentralTarget.CenterPoint().y + m_pApp->m_stInspParams.nSpotRectSize;

	// Draw box for the barcode - possible only for HikVision scanner
	if ((m_pApp->m_stBarcodeParts.pt1.x > 0) && (m_pApp->m_stBarcodeParts.pt1.x < 640) &&
		(m_pApp->m_stBarcodeParts.pt1.y > 0) && (m_pApp->m_stBarcodeParts.pt1.y < 480))
	{
		cv::Point pt1 = cv::Point(m_pApp->m_stBarcodeParts.pt1.x, m_pApp->m_stBarcodeParts.pt1.y);
		cv::Point pt2 = cv::Point(m_pApp->m_stBarcodeParts.pt2.x, m_pApp->m_stBarcodeParts.pt2.y);
		cv::Point pt3 = cv::Point(m_pApp->m_stBarcodeParts.pt3.x, m_pApp->m_stBarcodeParts.pt3.y);
		cv::Point pt4 = cv::Point(m_pApp->m_stBarcodeParts.pt4.x, m_pApp->m_stBarcodeParts.pt4.y);
		// Draw cross at pt1 as oftern drawing the box around the code does not make sense
		cv::Point ptLeft = pt1, ptRight = pt1, ptTop = pt1, ptBtm = pt1;
		ptLeft.x -= 5; ptRight.x += 5;
		ptTop.y -= 5; ptBtm.y += 5;
		cv::line(MatColor, ptLeft, ptRight, cv::Scalar(0, 255, 0), 2, 8);
		cv::line(MatColor, ptTop, ptBtm, cv::Scalar(0, 255, 0), 2, 8);
		//cv::line(MatColor, pt3, pt4, cv::Scalar(0, 255, 0), 2, 8);
		//cv::line(MatColor, pt4, pt1, cv::Scalar(0, 255, 0), 2, 8);
	}
	// Draw Platform Rect
	// Write Corner Numbers
	color = BLUE_COLOR;
	float fDeltaX = m_arMeasurementPoints[2].x - m_arMeasurementPoints[0].x;
	float fDeltaY = m_arMeasurementPoints[2].y - m_arMeasurementPoints[0].y;
	// Draw crosses at four corners where X and Y readings were taken (modified corner points)
	CArray<CPoint, CPoint> arModCornerPoint;
	arModCornerPoint.Add(CPoint(point1.x, point1.y));
	arModCornerPoint.Add(CPoint(point2.x, point2.y));
	arModCornerPoint.Add(CPoint(point3.x, point3.y));
	arModCornerPoint.Add(CPoint(point4.x, point4.y));
	double dDistPix12 = sqrt((point1.x - point2.x) * (point1.x - point2.x) + (point1.y - point2.y) * (point1.y - point2.y));
	double dDistPix23 = sqrt((point2.x - point3.x) * (point2.x - point3.x) + (point2.y - point3.y) * (point2.y - point3.y));
	double dDistPix34 = sqrt((point3.x - point4.x) * (point3.x - point4.x) + (point3.y - point4.y) * (point3.y - point4.y));
	double dDistPix41 = sqrt((point4.x - point1.x) * (point4.x - point1.x) + (point4.y - point1.y) * (point4.y - point1.y));
	
	// Draw cross marks at corner points
	// Draw cross marks at edge Points For Trace Method
	// Flip image to align user viewpoint with display on screen
	if (m_pApp->m_stInspParams.bFlipImageVert)
		flip(MatColor, MatColor, 0);
	if (m_pApp->m_stInspParams.bFlipImageHorz)
		flip(MatColor, MatColor, +1);

	// Draw corner numbers
	// Draw barcode on image
	// Write frame number in left hand top corner
	CString strFrameNumber;
	strFrameNumber.Format(L"%d", ImagesGrabbed);
	//pt1.x = 40;
	//pt1.y = 30;
	//cvPutText(&(IplImage)MatColor, CT2A(strFrameNumber.GetBuffer()), pt1, m_font, CV_RGB(0, 255, 0));	

	//return TRUE;
	MatColor.copyTo(m_MatColor);
	cv::resize(MatColor, m_MatColorSmall, m_MatColorSmall.size(), 0, 0, cv::INTER_AREA);
	//cvSaveImage("d:\\SmallImage.bmp", &(IplImage)m_MatColorSmall);

	//return TRUE;
	m_bDepthScanDone = TRUE;
	//t2 = m_HighResTimer.GetHighResElapsedTimeMsec();
	//strMsg.Format(L"Time for entire Process3DData() func - %3.3f", (t2 - t1));
	//IOutputLogString(strMsg);
	//t1 = t2;
	//InvalidateRect(NULL, FALSE);
	OrigGrey.release();
//#ifndef _DEBUG
//	Sleep(50);
//#endif
	return bResult;

}

// Get min drop for edge (used for ray tracing method)
// 
double CPMSHJView::GetMinDropForHeight(double dAvg)
{
	double dDrop = 15;// Default
	if ((m_pApp->m_stInspParams.d3DCameraHeight - dAvg) < 100)
		dDrop = 15;
	else if ((m_pApp->m_stInspParams.d3DCameraHeight - dAvg) < 150)
		dDrop = 15;
	else
		dDrop = 20;	

	return dDrop;
}

// Assumption arZDepths is filled with Z vals
// dRawAvg has been computed in the calling func
// Finds and removes outliers defined by the parameter nAllowableError
// Finds new average after removal of outliers
// Does this in a loop for max 3 iterations
// TBD: If outliers are found even after 3 iterations it should return error.
double CPMSHJView::GetRefinedZVal(double dRawAvg, CArray<double, double>& arZDepths, int nAllowableError)
{
	//return dRawAvg;

	double dOldAvg = 0.0f, dNewAvg = 0.0f;
	if (arZDepths.GetSize() <= 0)
		return dNewAvg;

	dOldAvg = dRawAvg;
	int nCount = 0;
	BOOL bOutlierFound = FALSE;
	for (int i = 0; i < arZDepths.GetSize(); i++)
	{
		double dTemp = arZDepths[i];// To check
		if (abs(arZDepths[i] - dOldAvg) > nAllowableError)
		{
			arZDepths[i] = 0;
			bOutlierFound = TRUE;
		}
		else
		{
			dNewAvg += arZDepths[i];
			nCount++;
		}
	}
	if (nCount > 0)
		dNewAvg /= nCount;
	// If the outliers did not make too much difference - return the new found avg
	if (fabs(dNewAvg - dOldAvg) < nAllowableError)
		return dNewAvg;

	dOldAvg = dNewAvg;
	int nNumIterations = 0;
	while (bOutlierFound)
	{
		nNumIterations++;
		nCount = 0;
		dNewAvg = 0.0f;
		bOutlierFound = FALSE;
		for (int i = 0; i < arZDepths.GetSize(); i++)
		{
			double dTemp = arZDepths[i];
			if (fabs(arZDepths[i]) > EPSILON)
			{
				if (fabs(arZDepths[i] - dOldAvg) > nAllowableError)
				{
					arZDepths[i] = 0;
					bOutlierFound = TRUE;
				}
				else
				{
					dNewAvg += arZDepths[i];
					nCount++;
				}
			}
		}
		if (nCount > 0)
			dNewAvg /= nCount;
		else
		{
			dNewAvg = dOldAvg;
			break;
		}
		if (fabs(dNewAvg - dOldAvg) < nAllowableError)
			return dNewAvg;

		if (nNumIterations > 3)
			break;
	}
	// It's a screw up if to few valid points or large error from initial value
	if ((nCount > arZDepths.GetSize() * 0.75) && (fabs(dNewAvg - dRawAvg) < nAllowableError * 2))
		return dNewAvg;
	else
		return dRawAvg;
} // GetRefinedZVal()

void CPMSHJView::ShowDimensions(CString strBoxLength, CString strBoxWidth, CString strBoxHeight, CString strWeight, CString strVolume)
{
	// Set appropriate decimal point
	if (m_bDecimalSeparatorDot)
	{
		strBoxLength.Replace(L",", L".");
		strBoxWidth.Replace(L",", L".");
		strBoxHeight.Replace(L",", L".");
		strVolume.Replace(L",", L".");
	}
	else
	{
		strBoxLength.Replace(L".", L",");
		strBoxWidth.Replace(L".", L",");
		strBoxHeight.Replace(L".", L",");
		strVolume.Replace(L".", L",");
	}
	m_StaticTextLength.SetWindowTextW(strBoxLength);
	m_StaticTextWidth.SetWindowTextW(strBoxWidth);
	m_StaticTextHeight.SetWindowTextW(strBoxHeight);
	if (strVolume.GetLength() > 5)
		strVolume = strVolume.Left(5);
	m_StaticTextVolume.SetWindowTextW(strVolume);
	// Weight is updated separately in UpdateWeight()
	//m_StaticTextWeight.SetWindowTextW(strWeight);
}

// Displayed dimensions need to be modified if units are modified in Options dialog
// Comes here only from the Options dialog and only if units have been changed.
// 
void CPMSHJView::ModifyDisplyDimsForNewUnits(eDimUnits NewUnits)
{
	if (NewUnits == DIM_MM)
	{
		m_strAccuracy = "%4.0f";
		m_strBoxLength.Format(m_strAccuracy, m_dBoxLength);
		m_strBoxWidth.Format(m_strAccuracy, m_dBoxWidth);
		m_strBoxHeight.Format(m_strAccuracy, m_dBoxHeight);
	}
	else if (NewUnits == DIM_CM)
	{
		m_strAccuracy = "%3.1f";
		m_strBoxLength.Format(m_strAccuracy, m_dBoxLength / 10.0);
		m_strBoxWidth.Format(m_strAccuracy, m_dBoxWidth / 10.0);
		m_strBoxHeight.Format(m_strAccuracy, m_dBoxHeight / 10.0);
	}
	if (NewUnits == DIM_M)
	{
		m_strAccuracy = "%1.3f";
		m_strBoxLength.Format(m_strAccuracy, m_dBoxLength / 1000.0);
		m_strBoxWidth.Format(m_strAccuracy, m_dBoxWidth / 1000.0);
		m_strBoxHeight.Format(m_strAccuracy, m_dBoxHeight / 1000.0);
	}
	else if (NewUnits == DIM_INCH)
	{
		m_strAccuracy = "%3.1f";
		m_strBoxLength.Format(m_strAccuracy, m_dBoxLength / 25.4);
		m_strBoxWidth.Format(m_strAccuracy, m_dBoxWidth / 25.4);
		m_strBoxHeight.Format(m_strAccuracy, m_dBoxHeight / 25.4);
	}
	ShowDimensions(m_strBoxLength, m_strBoxWidth, m_strBoxHeight, m_strWeight, m_strBoxVolume);
}

// Overriding the default behaviour of CRecordView class
// 
BOOL CPMSHJView::OnMove(UINT nIDMoveCommand)
{
	switch (nIDMoveCommand) 
	{
	case ID_RECORD_PREV:
		m_pSet->MovePrev();         
		if (!m_pSet->IsBOF())             
			break;     
	case ID_RECORD_FIRST:         
		m_pSet->MoveFirst();         
		break;     
	case ID_RECORD_NEXT:         
		m_pSet->MoveNext();         
		if (!m_pSet->IsEOF())            
			break;         
		if (!m_pSet->CanScroll()) 
		{             
			// Clear screen since we're sitting on EOF             
			m_pSet->SetFieldNull(NULL);             
			break;         
		}     
	case ID_RECORD_LAST:         
		m_pSet->MoveLast();         
		break;     
	default:         
		// unexpected case value         
		ASSERT(FALSE);     
	}     
	// Show results of Move operation     
	UpdateData(FALSE);     
	return TRUE;

	return CRecordView::OnMove(nIDMoveCommand);
}

// Used for EXCELL
// nErrorFlag: 1-Error, 1-Unstable, 3-OK
void CPMSHJView::UpdateWeight(double dWtNet, double dWtGross, double dWtTare, double dWtPreTare, CString strWtUnits, int nErrorFlag)
{
	CString strMsg;
	strMsg.Format(L"In UpdateWeight of View dwNet: %2.2f, dwGr: %2.2f", dWtNet, dWtGross);
	//IOutputLogString(strMsg);
	BOOL bIsNetWtNegative = FALSE;
	if (dWtNet < 0)
		bIsNetWtNegative = TRUE;
	BOOL bIsGrWtNegative = FALSE;
	if (dWtGross < 0)
		bIsGrWtNegative = TRUE;
	BOOL bIsTareWtNegative = FALSE;
	if (dWtTare < 0)
		bIsTareWtNegative = TRUE;
	BOOL bIsPreTareWtNegative = FALSE;
	if (dWtPreTare < 0)
		bIsPreTareWtNegative = TRUE;
	if (fabs(dWtGross) > EPSILON)
		dWtNet = dWtGross - dWtTare - dWtPreTare;// Ensure no visible mismatch
	else// dWtNet may have some legitimate value but dWtGross is 0. Can happen for CH340
		dWtGross = dWtNet;
	//if (bIsNetWtNegative)
	//	dWtNet *= -1;
	m_pApp->m_stInspResult.dWeightNet		= m_dBoxWeightNet = dWtNet;
	m_pApp->m_stInspResult.dWeightGr		= m_dBoxWeightGr = dWtGross;
	m_pApp->m_stInspResult.dWeightTare		= m_dBoxWeightTare = dWtTare;
	m_pApp->m_stInspResult.dWeightPT		= m_dBoxWeightPTare = dWtPreTare;
	CString strWeightNet, strWeightGross	= L"-", strWeightTare = L"-", strWeightPTare = L"-";
	if (nErrorFlag == 1)
	{
		strWeightNet = "-";
		strWeightGross = strWeightTare = strWeightPTare = L"   -";
		SetWeightReadStatus(FALSE);
	}
	else if (nErrorFlag == 2)
	{
		// Change weight icon obly if it was a different one previously
		if (m_bWtStatus)
			SetWeightReadStatus(FALSE);
		strWeightNet = "UST";
		m_bWtStatus = FALSE;
	}
	else
	{
		// Change weight icon only if it was a different one previously
		//if (!m_bWtStatus)
		if (nErrorFlag != 3)
			SetWeightReadStatus(TRUE);
		else
			SetWeightReadStatus(FALSE);
		// Change weight icon
		m_bWtStatus = TRUE;
		CString strWtFormat;
		if (m_pApp->m_stOptions.nWtAccuracy == 0)
			strWtFormat = "%2.0f";
		else if (m_pApp->m_stOptions.nWtAccuracy == 1)
			strWtFormat = "%2.1f";
		else if (m_pApp->m_stOptions.nWtAccuracy == 2)
			strWtFormat = "%2.2f";
		else if (m_pApp->m_stOptions.nWtAccuracy == 3)
			strWtFormat = "%2.3f";
		else if (m_pApp->m_stOptions.nWtAccuracy == 4)
			strWtFormat = "%2.4f";
		// Overriding for CL Express
		if (m_pApp->m_stOptions.WtUnits == Kg)
			strWtFormat = "%2.2f";
		//
		strWeightNet.Format(strWtFormat, dWtNet);
		if (bIsNetWtNegative && (strWeightNet.Find(L"-") == -1))
			strWeightNet = L"-" + strWeightNet;
		strWeightGross.Format(strWtFormat, dWtGross);
		if (bIsGrWtNegative && (strWeightGross.Find(L"-") == -1))
			strWeightGross = L"-" + strWeightGross;
		strWeightTare.Format(strWtFormat, dWtTare);
		//IOutputLogString(strWeightTare);
		strWeightPTare.Format(strWtFormat, dWtPreTare);
	}
	if (nErrorFlag == 3)
	{
		// Change weight icon
		SetWeightReadStatus(FALSE);
	}
	// Set appropriate decimal point
	if (m_bDecimalSeparatorDot)
	{
		strWeightNet.Replace(L",", L".");
		strWeightGross.Replace(L",", L".");
		strWeightTare.Replace(L",", L".");
		strWeightPTare.Replace(L",", L".");
	}
	else
	{
		strWeightNet.Replace(L".", L",");
		strWeightGross.Replace(L".", L",");
		strWeightTare.Replace(L".", L",");
		strWeightPTare.Replace(L".", L",");
	}
	//

	m_strWeight = strWeightNet;
	if (m_pApp->m_stOptions.bShowNetWt)
	{
		if (strWeightNet.GetLength() > 5)
			strWeightNet = strWeightNet.Left(5);
		m_StaticTextWeight.SetWindowTextW(strWeightNet);
		//m_LabelWeight.SetWindowText(strWeightNet);
	}
}

void CPMSHJView::UpdateWeightGRAM(double dWtNet, CString strWtUnits, int nErrorFlag)
{
	CString strMsg;
	strMsg.Format(L"In UpdateWeight of View dwNet: %2.2f", dWtNet);
	//IOutputLogString(strMsg);
	BOOL bIsNetWtNegative = FALSE;
	if (dWtNet < 0)
		bIsNetWtNegative = TRUE;
	BOOL bIsGrWtNegative = FALSE;
	if (bIsNetWtNegative)
		dWtNet *= -1;
	m_pApp->m_stInspResult.dWeightNet = m_dBoxWeightNet = dWtNet;
	CString strWeightNet;
	if (nErrorFlag == 1)// Error or over weight
	{
		strWeightNet = "-";
		SetWeightReadStatus(FALSE);
	}
	else if (nErrorFlag == 2)// Unstable
	{
		// Change weight icon obly if it was a different one previously
		if (m_bWtStatus)
			SetWeightReadStatus(FALSE);
		strWeightNet = "UST";
		m_bWtStatus = FALSE;
	}
	else if (nErrorFlag == 3)// Thread exiting
	{
		// Change weight icon
		SetWeightReadStatus(FALSE);
	}
	else // All is well!
	{
		m_bWtStatus = TRUE;
		CString strWtFormat;
		if (m_pApp->m_stOptions.nWtAccuracy == 0)
			strWtFormat = "%2.0f";
		else if (m_pApp->m_stOptions.nWtAccuracy == 1)
			strWtFormat = "%2.1f";
		else if (m_pApp->m_stOptions.nWtAccuracy == 2)
			strWtFormat = "%2.2f";
		else if (m_pApp->m_stOptions.nWtAccuracy == 3)
			strWtFormat = "%2.3f";
		else if (m_pApp->m_stOptions.nWtAccuracy == 4)
			strWtFormat = "%2.4f";
		CString strTemp(L"Weight String Format: ");
		strWeightNet.Format(strWtFormat, dWtNet);
		if (bIsNetWtNegative && (strWeightNet.Find(L"-") == -1))
			strWeightNet = L"-" + strWeightNet;
	}
	// Set appropriate decimal point
	if (m_bDecimalSeparatorDot)
		strWeightNet.Replace(L",", L".");
	else
		strWeightNet.Replace(L".", L",");
	//

	m_strWeight = strWeightNet;
	//m_LabelWeight.SetWindowText(strWeightNet);
} // UpdateWeightGRAM()

// This func applies corrections to XY dimensions depending on height of object (dJobHeight)
// 
double CPMSHJView::ApplyXYorrection(double& X, double& Y, double dJobHeight)
{
	double dDivFactor = 0.91;// 0.89;
	if (dJobHeight > 900)
		dDivFactor = 0.91;
	else if (dJobHeight > 800)
		dDivFactor = 0.91;
	else if (dJobHeight > 700)
		dDivFactor = 0.91;
	else if (dJobHeight > 600)
		dDivFactor = 0.91;
	else if (dJobHeight > 500)
		dDivFactor = 0.91;
	else
		dDivFactor = 0.91;// Was 0.9 and was showing u/s

	X /= dDivFactor;
	Y /= dDivFactor;

	return dDivFactor;
}

// Not used currently!
HBITMAP CPMSHJView::Mat2DIB(cv::Mat *Image)
{
	int bpp = Image->channels() * 8;

	cv::Mat tmp = *Image;
	//cvtColor(*Image, tmp, CV_BGRA2BGR);

	BITMAPINFO bmpInfo = { 0 };
	LONG lBmpSize = Image->rows * Image->cols * 3;
	bmpInfo.bmiHeader.biBitCount = bpp;
	bmpInfo.bmiHeader.biHeight = Image->rows;
	bmpInfo.bmiHeader.biWidth = Image->cols;
	bmpInfo.bmiHeader.biPlanes = 1;
	bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	// Pointer to access the pixels of bitmap
	BYTE * pPixels = 0, *pP;
	if (m_img != NULL)// Need to do this to avoid memory leak!
		DeleteObject(m_img);

	m_img = CreateDIBSection(NULL, (BITMAPINFO *)&bmpInfo, DIB_RGB_COLORS, (void **)&pPixels, NULL, 0);

	if (!m_img)
		return m_img; // return if invalid bitmaps

	//(m_img, lBmpSize, ((IplImage*)Image)->imageData);
	// Directly Write
	int left_width = ((bpp*Image->cols + 31) / 32) * 4;
	pP = pPixels;
	for (int y = Image->rows - 1, row = 0; row<Image->rows; row++, y--) 
	{
		for (int x = 0, col = 0; col<Image->cols; x += 3, col++) 
		{
			pP[x] = tmp.at<cv::Vec3b>(y, col).val[0];
			pP[x + 1] = tmp.at<cv::Vec3b>(y, col).val[1];
			pP[x + 2] = tmp.at<cv::Vec3b>(y, col).val[2];
		}
		pP += left_width;
	}
	return m_img;
}


void CPMSHJView::OnDraw(CDC* pDC)
{
	//IOutputLog(L"Refreshing view");
	//CRect rectClient;
	//GetClientRect(&rectClient);
	int w = m_rectImage.Width();
	int h = m_rectImage.Height();
	//Create a memory dc.
	CDC dcMem;
	int ret = dcMem.CreateCompatibleDC(pDC);

	if (ret == 0)
	{
		return;
	}
	CBitmap bmpMem;
	ret = bmpMem.CreateCompatibleBitmap(pDC, w, h);

	if (ret == 0)
	{
		return;
	}
	//Select the bitmap to memory dc
	CBitmap *pOldBmp = (CBitmap *)(dcMem.SelectObject(&bmpMem));

	Show(m_MatColorSmall, dcMem, 0, 0, m_MatColorSmall.cols, m_MatColorSmall.rows, 0, 0);

	//Display memory dc.
	pDC->BitBlt
	(
		m_rectImage.left,
		m_rectImage.top,
		w,
		h,
		&dcMem,
		0,
		0,
		SRCCOPY
	);
	DeleteDC(dcMem);
	DeleteObject(bmpMem);
	//RECT windowRect;
	//GetClientRect(&windowRect);// Bounds of the current window

	CFormView::OnDraw(pDC);
}

void CPMSHJView::SetMessage(int nStringResourceNum, COLORREF color)
{
	std::wstring str = m_pApp->m_pLanguage->GetString(nStringResourceNum);
	SetMessage(str.c_str(), color);
	m_strMsg = str.c_str();
}

void CPMSHJView::AddMessage(CString strMsgAdd, COLORREF color)
{
	m_strMsg += L", ";
	m_strMsg == strMsgAdd;
	SetMessage(m_strMsg, color);
}

void CPMSHJView::SetWeightReadStatus(BOOL bOK)
{
}


void CPMSHJView::SetDimUnits(eDimUnits DimUnits)
{
}

void CPMSHJView::ChangeLaguage(eLanguage NewLanguage)
{
	wstring sDLLPath(m_pApp->m_strProjectDir.GetBuffer());
	bool bSuccess = true;
	if (NewLanguage == CHINESE)
	{
		sDLLPath += _T("\\Lng\\PMSEXOCN.dll");// For Chinese - need to make dll. Look at D:\\Dimensioner\\Multiligual folder
		bSuccess = m_pApp->m_pLanguage->LoadLanguage(sDLLPath, _T("Chinese"));
	}
	else if (NewLanguage == ENGLISH)
	{
		bSuccess = m_pApp->m_pLanguage->LoadLanguage(2057);
	}
	SetButtonsNLabels();
}

void CPMSHJView::SetDecimalSeparator()
{
	//
	int   ret;
	DWORD value;

	ret = GetLocaleInfoEx(LOCALE_NAME_USER_DEFAULT,
		LOCALE_SDECIMAL,
		(LPWSTR)&value,
		sizeof(value) / sizeof(WCHAR));
	if (value == 46)// Dot
		m_bDecimalSeparatorDot = TRUE;
	else if (value == 44)// Comma
		m_bDecimalSeparatorDot = FALSE;

}

void CPMSHJView::OnLButtonDown(UINT nFlags, CPoint point)
{
	CRecordView::OnLButtonDown(nFlags, point);
}


void CPMSHJView::OnSize(UINT nType, int cx, int cy)
{
	CRecordView::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
}

// This func returns average distance of pixels in the rect passed to it
double	CPMSHJView::GetPlatformHeight(openni::VideoStream& depthStream, cv::Mat MatDepth, CRect rect)
{
	openni::VideoFrameRef depthFrame;
	depthStream.readFrame(&depthFrame);
	MatDepth.data = (uchar*)depthFrame.getData();

	cv::Point ptMinVal, ptMaxVal;
	double dMinDist = FLT_MAX;
	double dMaxDist = 0;
	double dSecondMin = FLT_MAX;
	double dMaxDistZ = 3500;
	double dMinDistZ = 500;
	int nStartRow = rect.CenterPoint().y - m_pApp->m_stInspParams.nSpotRectSize;
	int nEndRow = rect.CenterPoint().y + m_pApp->m_stInspParams.nSpotRectSize;
	int nStartCol = rect.CenterPoint().x - m_pApp->m_stInspParams.nSpotRectSize;
	int nEndCol = rect.CenterPoint().x + m_pApp->m_stInspParams.nSpotRectSize;
	double dAvg = 0.0f;
	int nCount = 0;
	int rowSize = depthFrame.getStrideInBytes() / sizeof(openni::DepthPixel);
	if (depthFrame.isValid())
	{
		const openni::DepthPixel* pDepthRow = (const openni::DepthPixel*)depthFrame.getData();
		for (int row = nStartRow; row < nEndRow; ++row)
		{
			const openni::DepthPixel* pDepth = pDepthRow;
			for (int col = nStartCol; col < nEndCol; ++pDepth, col++)
			{
				int nOtherZ = *pDepth;
				uint16_t z = MatDepth.at<uint16_t>(row, col); // 
				if ((z > dMinDistZ) && (z < dMaxDistZ))// Check z validity
				{
					// Finding Min distance as minMaxLoc returns this as 0 for nan
					if (dMinDist > nOtherZ)
						dMinDist = nOtherZ;
					dAvg += z;
					nCount++;
				}
			}
			pDepthRow += rowSize;
		}
		if (nCount > (rect.Width() * rect.Height() / 2))
			dAvg /= nCount;
		else
		{
			return 0.0;
		}
	}

	return dAvg;
} // GetPlatformHeight()

// This function starts a live stream at the camera and calls GetPlatformHeight() to get platform height. 
// It averages this height over 10 frames and updates m_pApp->m_stInspParams.d3DCameraHeight parameter
//
BOOL CPMSHJView::CalibForPlatformHeight()
{
	BOOL bResult = TRUE;
	IOutputLog(L"Entering CalibForPlatformHeight()");
	CString str;
	m_b3DLive = TRUE;
	int nImagesToGrab = 10;
	m_nImagesGrabbed = 0;
	double dSum = 0;
	bool TimoutOccurred = false;// As long as a valid buffer is returned, i.e., no timeout has occurred, the grab loop can continue.
	double t1 = m_HighResTimer.GetHighResElapsedTimeMsec();
	double t2 = t1;
	int nNumFrames = 0;
	int nNumInvalidFrames = 0;
	try
	{
		int changedIndex;
		openni::VideoFrameRef depthFrame, colorFrame;
		openni::VideoStream* stream[] = { &m_pApp->m_depth, &m_pApp->m_color };

		int nChannels = 1;
		BOOL bDepthFrameValid = FALSE;
		int nFrameNumAtLastBarcode = 0;// Used to refresh message
		IOutputLog(L"Entering Grab Loop");
		do
		{
			openni::Status rc = OpenNI::waitForAnyStream(stream, 2, &changedIndex, TIMEOUT_FOREVER);
			if (rc != openni::STATUS_OK)
			{
				str.Format(L"Error in Image Grabbing - waitForAnyStream() failed:\n%s\n", openni::OpenNI::getExtendedError());
				SetMessage(str);
				IOutputLog(L"Error in Image Grabbing - waitForAnyStream() failed");
			}
			switch (changedIndex)
			{
			case 0:
				rc = m_pApp->m_depth.readFrame(&depthFrame);
				if (rc != openni::STATUS_OK)
				{
					str.Format(L"Error in Image Grabbing - depth.readFrame() failed:\n%s\n", openni::OpenNI::getExtendedError());
					IOutputLog(L"Error in Image Grabbing - depth.readFrame() failed");
					SetMessage(IDS_STR_GRAB_ERR);
					openni::OpenNI::shutdown();
					m_b3DLive = FALSE;
					SetButtonsState(FALSE);
					return m_bBoxResultReady;
				}
				if (depthFrame.isValid())
				{
					m_MatDepth.data = (uchar*)depthFrame.getData();
				}
				break;

			case 1:
				//IOutputLog(L"Color frame switch statement");
				rc = m_pApp->m_color.readFrame(&colorFrame);
				if (rc != openni::STATUS_OK)
				{
					str.Format(L"Error in Image Grabbing - color.readFrame() failed:\n%s\n", openni::OpenNI::getExtendedError());
					IOutputLog(L"Error in Image Grabbing - color.readFrame() failed");
					SetMessage(IDS_STR_GRAB_ERR);
					openni::OpenNI::shutdown();
					m_b3DLive = FALSE;
					SetButtonsState(FALSE);
					return m_bBoxResultReady;
				}
				if (colorFrame.isValid())
				{
					m_MatColor.data = (uchar*)colorFrame.getData();
					cv::cvtColor(m_MatColor, m_MatColor, CV_BGR2RGB);
					m_bImageLoaded = TRUE;
					CString strMsg;
					double dHeight = GetPlatformHeight(m_pApp->m_depth, m_MatDepth, m_rectCentralTarget);
					if (dHeight > EPSILON)
					{
						nNumFrames++;
						dSum += dHeight;
					}
					else
						nNumInvalidFrames++;
					if (m_pApp->m_stInspParams.bFlipImageVert)
						flip(m_MatColor, m_MatColor, 0);
					if (m_pApp->m_stInspParams.bFlipImageHorz)
						flip(m_MatColor, m_MatColor, +1);
					InvalidateRect(m_rectImage, FALSE);
				}
			}
		} while ((nNumFrames < nImagesToGrab) && (nNumInvalidFrames < 50));
	}
	catch (...)
	{
		IOutputLog(L"Error in grabbing for calibration loop!");

		str.Format(L"%s", openni::OpenNI::getExtendedError());
		SetMessage(str);
	}
	m_b3DLive = FALSE;
	SetButtonsState(FALSE);// Actually no need since the time taken is very short
	if (nNumInvalidFrames >= 50)
	{
		AfxMessageBox(L"Too many invalid frames. Could not set height!");
		return FALSE;
	}
	double dAvgHeight = dSum / nImagesToGrab;
	int nAvgHt = dAvgHeight + 0.5;
	dAvgHeight = nAvgHt;
	// Validate result
	str.Format(L"Camera Height is: %4.0f. Do you want to use it?", dAvgHeight);
	if (::MessageBox(NULL, str, L"New Camera Ht", MB_YESNO) == IDYES)
		m_pApp->m_stInspParams.d3DCameraHeight = dAvgHeight;

	m_pApp->WriteInspParamsToRegistry();

	return bResult;
} // CalibForPlatformHeight()

// This function starts a live stream at the camera and calls GetPlatformHeight() to get platform height at the four corner rects. 
// It displays difference in the height of four corners in the image on screen so that 
// user can adjust the camera to make its axis perpendicular to the platform
//
BOOL CPMSHJView::CalibForCamAlignment()
{
	BOOL bResult = TRUE;
	IOutputLog(L"Entering CalibForCamAlignment()");
	CString str;
	m_b3DLive = m_bPlatformCalibON = TRUE;
	SetButtonsState(TRUE);
	int nImagesToGrab = 10;
	m_nImagesGrabbed = 0;
	double dSum = 0;
	bool TimoutOccurred = false;// As long as a valid buffer is returned, i.e., no timeout has occurred, the grab loop can continue.
	double t1 = m_HighResTimer.GetHighResElapsedTimeMsec();
	double t2 = t1;
	double dAvg = 0.0f, dHeightLHTop = 0.0f, dHeightLHBtm = 0.0f, dHeightRHTop = 0.0f, dHeightRHBtm = 0.0f;
	int nNumFrames = 0;
	int nNumInvalidFrames = 0;
	CRect rectCamAlign;
	rectCamAlign.left = IMAGE_WIDTH / 2 - m_pApp->m_stInspParams.nCamAlignRectSize / 2;
	rectCamAlign.top = IMAGE_HEIGHT / 2 - m_pApp->m_stInspParams.nCamAlignRectSize / 2;
	rectCamAlign.right = rectCamAlign.left + m_pApp->m_stInspParams.nCamAlignRectSize;
	rectCamAlign.bottom = rectCamAlign.top + m_pApp->m_stInspParams.nCamAlignRectSize;
	CRect rectLHTop, rectLHBtm, rectRHTop, rectRHBtm;
	rectLHTop = CRect(rectCamAlign.left, rectCamAlign.top, rectCamAlign.left + 20, rectCamAlign.top + 20);
	rectLHBtm = CRect(rectCamAlign.left, rectCamAlign.bottom - 20, rectCamAlign.left + 20, rectCamAlign.bottom);
	rectRHTop = CRect(rectCamAlign.right - 20, rectCamAlign.top, rectCamAlign.right, rectCamAlign.top + 20);
	rectRHBtm = CRect(rectCamAlign.right - 20, rectCamAlign.bottom - 20, rectCamAlign.right, rectCamAlign.bottom);
	SetScanBnText(TRUE);
	SetMessage(IDS_STR_CALIB_MSG);
	try
	{
		int changedIndex;
		openni::VideoFrameRef depthFrame, colorFrame;
		openni::VideoStream* stream[] = { &m_pApp->m_depth, &m_pApp->m_color };

		int nChannels = 1;
		CString strMsg;
		BOOL bDepthFrameValid = FALSE;
		int nFrameNumAtLastBarcode = 0;// Used to refresh message
		IOutputLog(L"Entering Grab Loop");
		do
		{
			openni::Status rc = OpenNI::waitForAnyStream(stream, 2, &changedIndex, TIMEOUT_FOREVER);
			if (rc != openni::STATUS_OK)
			{
				str.Format(L"Error in Image Grabbing - waitForAnyStream() failed:\n%s\n", openni::OpenNI::getExtendedError());
				SetMessage(str);
				IOutputLog(L"Error in Image Grabbing - waitForAnyStream() failed");
			}
			switch (changedIndex)
			{
			case 0:
				rc = m_pApp->m_depth.readFrame(&depthFrame);
				if (rc != openni::STATUS_OK)
				{
					str.Format(L"Error in Image Grabbing - depth.readFrame() failed:\n%s\n", openni::OpenNI::getExtendedError());
					IOutputLog(L"Error in Image Grabbing - depth.readFrame() failed");
					SetMessage(IDS_STR_GRAB_ERR);
					openni::OpenNI::shutdown();
					m_b3DLive = m_bPlatformCalibON = FALSE;
					SetButtonsState(FALSE);
					SetScanBnText(FALSE);
					return m_bBoxResultReady;
				}
				if (depthFrame.isValid())
				{
					m_MatDepth.data = (uchar*)depthFrame.getData();
				}
				break;

			case 1:
				//IOutputLog(L"Color frame switch statement");
				rc = m_pApp->m_color.readFrame(&colorFrame);
				if (rc != openni::STATUS_OK)
				{
					str.Format(L"Error in Image Grabbing - color.readFrame() failed:\n%s\n", openni::OpenNI::getExtendedError());
					IOutputLog(L"Error in Image Grabbing - color.readFrame() failed");
					SetMessage(IDS_STR_GRAB_ERR);
					openni::OpenNI::shutdown();
					m_b3DLive = m_bPlatformCalibON = FALSE;
					SetButtonsState(FALSE);
					SetScanBnText(FALSE);
					return m_bBoxResultReady;
				}
				if (colorFrame.isValid())
				{
					m_MatColor.data = (uchar*)colorFrame.getData();
					cv::cvtColor(m_MatColor, m_MatColor, CV_BGR2RGB);
					if (m_pApp->m_stInspParams.bFlipImageVert)
						flip(m_MatColor, m_MatColor, 0);
					if (m_pApp->m_stInspParams.bFlipImageHorz)
						flip(m_MatColor, m_MatColor, +1);
					dHeightLHTop = GetPlatformHeight(m_pApp->m_depth, m_MatDepth, rectLHTop);
					dHeightLHBtm = GetPlatformHeight(m_pApp->m_depth, m_MatDepth, rectLHBtm);
					dHeightRHTop = GetPlatformHeight(m_pApp->m_depth, m_MatDepth, rectRHTop);
					dHeightRHBtm = GetPlatformHeight(m_pApp->m_depth, m_MatDepth, rectRHBtm);
					if ((dHeightLHTop > EPSILON) && (dHeightLHBtm > EPSILON) & (dHeightRHTop > EPSILON) && (dHeightRHBtm > EPSILON))
					{
						nNumFrames++;
						// Get avg of the four corner heights
						dAvg = (dHeightLHTop + dHeightLHBtm + dHeightRHTop + dHeightRHBtm) / 4;
						// Draw corner heights on image
						CString str;
						CvPoint pt;
						// Left Top
						str.Format(L"%d", (int)(dHeightLHTop - dAvg + 0.5));
						pt.x = rectLHTop.CenterPoint().x - 10;
						pt.y = rectLHTop.CenterPoint().y + 40;
						cvPutText(&(IplImage)m_MatColor, CT2A(str.GetBuffer()), pt, m_font, CV_RGB(0, 255, 0));
						// Left Btm
						str.Format(L"%d", (int)(dHeightLHBtm - dAvg + 0.5));
						pt.x = rectLHBtm.CenterPoint().x - 10;
						pt.y = rectLHBtm.CenterPoint().y - 20;
						cvPutText(&(IplImage)m_MatColor, CT2A(str.GetBuffer()), pt, m_font, CV_RGB(0, 255, 0));
						// Right Top
						str.Format(L"%d", (int)(dHeightRHTop - dAvg + 0.5));
						pt.x = rectRHTop.CenterPoint().x - 10;
						pt.y = rectRHTop.CenterPoint().y + 40;
						cvPutText(&(IplImage)m_MatColor, CT2A(str.GetBuffer()), pt, m_font, CV_RGB(0, 255, 0));
						// Right Btm
						str.Format(L"%d", (int)(dHeightRHBtm - dAvg + 0.5));
						pt.x = rectRHBtm.CenterPoint().x - 10;
						pt.y = rectRHBtm.CenterPoint().y - 20;
						cvPutText(&(IplImage)m_MatColor, CT2A(str.GetBuffer()), pt, m_font, CV_RGB(0, 255, 0));
						// Draw Rects in different colors (green / red)
						CvRect rect = CRectToCVRect(rectLHTop);
						if (fabs(dHeightLHTop - dAvg) < 5)
							cv::rectangle(m_MatColor, rect, CV_RGB(0, 255, 0), 2);
						else if ((dAvg - dHeightLHTop) > 5) // Point is lower than avg
							cv::rectangle(m_MatColor, rect, CV_RGB(0, 0, 255), 2);
						else
							cv::rectangle(m_MatColor, rect, CV_RGB(255, 0, 0), 2);
						rect = CRectToCVRect(rectLHBtm);
						if (fabs(dHeightLHBtm - dAvg) < 5)
							cv::rectangle(m_MatColor, rect, CV_RGB(0, 255, 0), 2);
						else if ((dAvg - dHeightLHBtm) > 5) // Point is lower than avg
							cv::rectangle(m_MatColor, rect, CV_RGB(0, 0, 255), 2);
						else
							cv::rectangle(m_MatColor, rect, CV_RGB(255, 0, 0), 2);
						rect = CRectToCVRect(rectRHTop);
						if (fabs(dHeightRHTop - dAvg) < 5)
							cv::rectangle(m_MatColor, rect, CV_RGB(0, 255, 0), 2);
						else if ((dAvg - dHeightRHTop) > 5) // Point is lower than avg
							cv::rectangle(m_MatColor, rect, CV_RGB(0, 0, 255), 2);
						else
							cv::rectangle(m_MatColor, rect, CV_RGB(255, 0, 0), 2);
						rect = CRectToCVRect(rectRHBtm);
						if (fabs(dHeightRHBtm - dAvg) < 5)
							cv::rectangle(m_MatColor, rect, CV_RGB(0, 255, 0), 2);
						else if ((dAvg - dHeightRHBtm) > 5) // Point is lower than avg
							cv::rectangle(m_MatColor, rect, CV_RGB(0, 0, 255), 2);
						else
							cv::rectangle(m_MatColor, rect, CV_RGB(255, 0, 0), 2);
					}
					else
						nNumInvalidFrames++;
					m_bImageLoaded = TRUE;
					InvalidateRect(m_rectImage, FALSE);
				}
			}
			// Ensure refresh and handling of UI messages
			MSG msg;
			while (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				::TranslateMessage(&msg);
				::DispatchMessage(&msg);
			}
		} while (m_bPlatformCalibON);
	}
	catch (...)
	{
		IOutputLog(L"Error in grabbing for calibration loop!");

		str.Format(L"%s", openni::OpenNI::getExtendedError());
		SetMessage(str);
	}
	SetScanBnText(FALSE);
	m_b3DLive = m_bPlatformCalibON = FALSE;
	SetButtonsState(FALSE);
	SetMessage(IDS_STR_SCAN);

	if (nNumInvalidFrames >= 50)
	{
		AfxMessageBox(L"Too many invalid frames. Could not run camera alignment!");
		return FALSE;
	}
	str.Format(L"Ht LHTop: %4.0f, Ht LHBtm: %4.0f, Ht RHTop: %4.0f, Ht RHBtm: %4.0f, Avg Ht: %4.0f", dHeightLHTop, dHeightLHBtm, dHeightRHTop, dHeightRHBtm, dAvg);
	IOutputLogString(str);

	return bResult;
} // CalibForPlatformHeight()

// Hides weight labels which are not rqd
void CPMSHJView::DisplayWtLabels()
{
	std::wstring str;
	CString strEmpty, strBnCaption;
	if (!m_pApp->m_stOptions.bShowNetWt)
	{
		//m_LabelWeight.SetWindowTextW(strEmpty);
		GetDlgItem(IDC_PIC_WEIGHT)->ShowWindow(SW_HIDE);
	}
	else
	{
		strBnCaption.Format(L"0,00");
		//m_LabelWeight.SetWindowTextW(strBnCaption);
		str = m_pApp->m_pLanguage->GetString(IDS_STR_NET_WT);
		strBnCaption = str.c_str();
		GetDlgItem(IDC_PIC_WEIGHT)->ShowWindow(SW_SHOW);
	}
	Invalidate();
}

//void CPMSHJView::OnBnClickedBnCalib()
//{
//	CDlgCalib dlg;
//	if (dlg.DoModal() == IDOK)
//	{
//		if (dlg.m_nCalibType == 0) // Camera height calibration
//		{
//			CalibForPlatformHeight();
//		}
//		else if (dlg.m_nCalibType == 1) // Camera alignment calibration
//		{
//			CalibForCamAlignment();
//		}
//	}
//}

// Prints HJ label of size 100 (W) x 80 (H) mm
// Fields (Rows) are: 1) Corporate Name 2) Product Name (Name can be set) 3) Barcode 4) Specifications 5) Weight 6) Date
BOOL CPMSHJView::PrintLabelHJ(STInspResult InspResult)
{
	BOOL bResult = TRUE;
	UpdateData(TRUE);
	CPMSHJApp* pApp = (CPMSHJApp*)AfxGetApp();
	CStringA str, str1, str2;
	CString strUni;
	std::string strPrinter = CW2A(m_pApp->m_stPrintParams.strPrinterName);
	//IOutputLog(L"Opening Printer Port!");
	int nRetVal = openport((char*)strPrinter.c_str());// This Works!
	if (nRetVal == 0)
		return FALSE;
	IOutputLog(L"In PrintLabelHJ() Printer Port Opened!");
	// label width mm, label height, speed, printing density (0-15), type of sensor (0-gap sensor), Vert spacing, offset
	setup("100 mm", "80 mm", "3", "10", "0", "3", "0");
	sendcommand("SET CUTTER OFF");
	sendcommand("DIRECTION 1,0");
	sendcommand("GAP 5 mm");
	sendcommand("REFERENCE -170,0");
	//formfeed();
	nobackfeed();
	clearbuffer();
	// Draw outer box. BOX command params (all in dots): StartX, StartY, EndX, EndY, Line Thk
	sendcommand("BOX 16,48,764,592,5");//624
	// Draw 5 horiz lines
	sendcommand("BAR 20,128,742,4");// StartX, StartY, Line Width, Line Thk//602
	sendcommand("BAR 20,234,742,4");// StartX, StartY, Line Width, Line Thk
	sendcommand("BAR 20,320,742,4");// StartX, StartY, Line Width, Line Thk
	sendcommand("BAR 20,416,742,4");// StartX, StartY, Line Width, Line Thk
	sendcommand("BAR 20,512,742,4");// StartX, StartY, Line Width, Line Thk
	// Draw a box for the lone vertical line as it seems we cannot set orientation of line and max thk is 12 mm
	sendcommand("BOX 16,128,228,592,5");// StartX, StartY, EndX, EndY, Line Thk
	// StartX, StartY, Font, Rot Angle, ScaleX, ScaleY, Text 
	if (m_pApp->m_stPrintParams.bPrintLogo)
	{
		// Need to 
		nRetVal = sendcommand("PUTPCX 320,76,\"LOGOCLEXP.PCX\"");
		if (nRetVal)
			IOutputLog(L"Printing Logo");
		else
			IOutputLog(L"Print Logo Failed!");
	}
	else
	{
		// Write Corporate Name - whether Chinese or English
		if (IsStringChinese(m_pApp->m_stPrintParams.strCorporateName))
			nRetVal = printerfont("320", "76", "TSS24.BF2", "0", "2", "2", CW2A(m_pApp->m_stPrintParams.strCorporateName.GetBuffer()));//248
		else
			nRetVal = printerfont("320", "76", "4", "0", "1.2", "1.2", CW2A(m_pApp->m_stPrintParams.strCorporateName.GetBuffer()));//248
	}
	// Write Product Name Legend - First Chinese, then English
	strUni.Format(L"产品名称");
	nRetVal = printerfont("72", "154", "TSS24.BF2", "0", "1", "1", CW2A(strUni.GetBuffer()));//72
	nRetVal = printerfont("28", "184", "3", "0", "1", "1", "Product Name");//28
	// Write Product Name - whether Chinese or English
	if (IsStringChinese(m_pApp->m_stPrintParams.strProductName))
		nRetVal = printerfont("456", "174", "TSS24.BF2", "0", "1.5", "1.5", CW2A(m_pApp->m_stPrintParams.strProductName.GetBuffer()));//384
	else
		nRetVal = printerfont("456", "174", "3", "0", "1.5", "1.5", CW2A(m_pApp->m_stPrintParams.strProductName.GetBuffer()));//384
	// Write Bar code Legend - First Chinese, then English
	strUni.Format(L"条  码");
	nRetVal = printerfont("64", "248", "TSS24.BF2", "0", "1", "1", CW2A(strUni.GetBuffer()));//64
	nRetVal = printerfont("56", "278", "3", "0", "1", "1", "Bar Code");//56
	// Write Bar Code - Start point X, Y, Code type, height (dots), print text, Rot Angle, Scale X, Scale Y, Text
	if (InspResult.strBarcode.GetLength() > m_pApp->m_stInspParams.nBarcodeLengthMax)
		InspResult.strBarcode = InspResult.strBarcode.Left(m_pApp->m_stInspParams.nBarcodeLengthMax);
	// If chars are Chinese can't encode in 1D barcode!
	if (!IsStringChinese(InspResult.strBarcode))
	{
		str = CW2A(InspResult.strBarcode);
		barcode("416", "240", "128", "56", "1", "0", "1", "1", str.GetBuffer(str.GetLength()));// strUni does not work!//344
	}
	// Write Specifications Legend- First Chinese, then English
	strUni.Format(L" 测   量");
	nRetVal = printerfont("64", "342", "TSS24.BF2", "0", "1", "1", CW2A(strUni.GetBuffer()));//64
	nRetVal = printerfont("24", "372", "2", "0", "1", "1", "Specifications");
	// Write Measurements - Line 1
	str.Format("L=%dmm W=%dmm H=%dmm", (int)(InspResult.dLength), (int)(InspResult.dWidth), (int)(InspResult.dHeight));
	//windowsfont(248, 344, 40, 0, 0, 0, "Arial", str.GetBuffer(str.GetLength()));
	nRetVal = printerfont("312", "344", "3", "0", "1", "1", str.GetBuffer(str.GetLength()));//240
	// Write Specifications - Line 2 (Volume)
	strUni.Format(L"V=%2.3fm³", InspResult.dVolume);
	nRetVal = printerfont("428", "376", "3", "0", "1", "1", CW2A(strUni.GetBuffer()));//356
	// Write Weight Legend- First Chinese, then English
	strUni.Format(L"重  量");
	nRetVal = printerfont("88", "432", "TSS24.BF2", "0", "1", "1", CW2A(strUni.GetBuffer()));//88
	nRetVal = printerfont("72", "468", "3", "0", "1", "1", "Weight");//72
	// Write Weight
	str.Format("%3.2f kg", InspResult.dWeightGr);
	//str.Format("%3.2f kg", pApp->m_stInspResultSaved.dWeightGr);
	nRetVal = printerfont("416", "458", "3", "0", "1", "1", str.GetBuffer(str.GetLength()));// X is OK for this!/344
	// Write Date Legend- First Chinese, then English
	strUni.Format(L"日 期");
	nRetVal = printerfont("96", "520", "TSS24.BF2", "0", "1", "1", CW2A(strUni.GetBuffer()));//96
	nRetVal = printerfont("96", "552", "3", "0", "1", "1", "Date");//96
	// Write Date
	CTime time = CTime::GetCurrentTime();
	CString strTotal, strYear, strMonth, strDay;
	strYear.Format(L"%d年", time.GetYear());
	strMonth.Format(L"%d月", time.GetMonth());
	strDay.Format(L"%d日", time.GetDay());
	strTotal = strYear + strMonth + strDay;
	nRetVal = printerfont("392", "544", "TSS24.BF2", "0", "1", "1", CW2A(strTotal.GetBuffer()));//320

	//windowsfont(100, 300, 72, 0, 2, 0, "ariel", "Arial Font 72 pt");
	IOutputLog(L"In PrintLabelHJ() just before Printing Label");
	printlabel("1", "1");
	closeport();
	IOutputLog(L"In PrintLabelHJ() closed Port");

	return bResult;
}// PrintLabelHJ()

// Prints CL Express label of size 70 (W) x 100 (H) mm
// Fields (Rows) are: 1) LOGO 2) Product Name (Name can be set) 3) Volume (L, W, H, V) 4) Weight 5) Freight 6) Date
// Learning: Logo file has to be LOGO.PCX (All CAPS). File size should be around 2 KB. 
// If we have png or bmp file use IrfanView to convert to PCX. 
// Use monochrome pallet to reduce file size with 'Decrease Color Depth' to use black/white pallet.
// 
BOOL CPMSHJView::PrintLabelCLExpress(STInspResult InspResult)
{
	BOOL bResult = TRUE;
	UpdateData(TRUE);
	CStringA str, str1, str2;
	CString strUni;
	std::string strPrinter = CW2A(m_pApp->m_stPrintParams.strPrinterName);
	int nRetVal = 0;
	CString strMsg;
	strMsg.Format(L"Printer Name: %s", strPrinter.c_str());
	//IOutputLogString(strMsg);
	//if (!m_bPrinterPortOpened)
	{
		int nRetVal = openport((char*)strPrinter.c_str());// This Works!
		if (nRetVal == 0)
		{
			IOutputLog(L"openport() failed!");
			return FALSE;
		}
		if (!CheckPrinterStatus())
		{
			IOutputLog(L"CheckPrinterStatus() returned FALSE!");
			nRetVal = closeport();
			return FALSE;
		}
		m_bPrinterPortOpened = TRUE;
	}
	//IOutputLog(L"In PrintLabelCLExpress() Port Opened, Status OK!");
	// label width mm, label height, speed, printing density (0-15), type of sensor (0-gap sensor), Vert spacing, offset
	setup("70 mm", "100 mm", "3", "10", "0", "3", "0");
	sendcommand("SET CUTTER OFF");
	sendcommand("DIRECTION 0");
	sendcommand("GAP 5 mm");
	sendcommand("REFERENCE -24,12");// For DIRECTION 1, Referece is 64,12 // was -32
	nobackfeed();
	clearbuffer();
	// If we draw boxes and bars before drawing Logo they don't get printed!
	// Print Logo
	if (m_pApp->m_stPrintParams.bPrintLogo)
	{
		std::string strFileName(CW2A(m_pApp->m_stPrintParams.strLogoFile.GetString()));
		strMsg.Format(L"Logo File: %s", strFileName.c_str());
		//IOutputLogString(strMsg);
		clearbuffer(); //Clear image buffer
		if (strFileName.find(".PCX") != string::npos)// Found ".PCX"
		{
			//if (!m_bLogoUploadedToPrinter)
			{
				//nRetVal = sendcommand("KILL \"*\"");
				nRetVal = downloadpcx((char*)(strFileName.c_str()), "LOGOCLEXP.PCX");  //Download PCX file into printer
				if (nRetVal)
				{
					nRetVal = sendcommand("MOVE");
					if (nRetVal)
						m_bLogoUploadedToPrinter = TRUE;
					else
					{
						IOutputLog(L"PrintLabelCLExpress() Couldn't MOVE logo in printer!");
						nRetVal = closeport();
						m_bLogoUploadedToPrinter = FALSE;
						return FALSE;
					}
				}
				else
				{
					IOutputLog(L"PrintLabelCLExpress() Couldn't upload logo to printer!");
					nRetVal = closeport();
					m_bLogoUploadedToPrinter = FALSE;
					return FALSE;
				}
			}
			if (m_bLogoUploadedToPrinter)
				nRetVal = sendcommand("PUTPCX 208,24,\"LOGOCLEXP.PCX\"");
			else
			{
				IOutputLog(L"PrintLabelCLExpress() Couldn't upload / MOVE logo to printer!");
				AfxMessageBox(L"PrintLabelCLExpress() Couldn't upload / MOVE logo to printer!");
				nRetVal = closeport();
				return FALSE;
			}
		}
	}
	else
	{
		IOutputLog(L"PrintLabelCLExpress() Not printing logo");
		// Write Corporate Name - whether Chinese or English
		if (IsStringChinese(m_pApp->m_stPrintParams.strCorporateName))
			nRetVal = printerfont("288", "36", "TSS24.BF2", "0", "2", "2", CW2A(m_pApp->m_stPrintParams.strCorporateName.GetBuffer()));
		else
			nRetVal = printerfont("288", "36", "4", "0", "1.2", "1.2", CW2A(m_pApp->m_stPrintParams.strCorporateName.GetBuffer()));
	}
	if (nRetVal)
		IOutputLog(L"PrintLabelCLExpress() after printing logo");
	else
	{
		IOutputLog(L"PrintLabelCLExpress() printing logo failed!");
		AfxMessageBox(L"Printing Logo Failed!");
		nRetVal = closeport();
		return FALSE;
	}
	sendcommand("BOX 48,104,560,760,5");//760
	// Draw 3 horiz lines
	sendcommand("BAR 50,258,512,4");// StartX, StartY, Line Width, Line Thk //194
	sendcommand("BAR 50,560,512,4");// 464
	sendcommand("BAR 50,656,512,4");// 560
	//sendcommand("BAR 50,652,512,4");// StartX, StartY, Line Width, Line Thk
	// Draw a box for the lone vertical line as it seems we cannot set orientation of line and max thk is 12 mm
	sendcommand("BOX 48,258,216,760,4");// StartX, StartY, EndX, EndY, Line Thk // 760
										
	// Write Volume Data Legend - First Chinese, then English
	strUni.Format(L"体积数据");
	nRetVal = printerfont("80", "392", "TSS24.BF2", "0", "1", "1", CW2A(strUni.GetBuffer()));//286
	nRetVal = printerfont("76", "428", "3", "0", "1", "1", "Volume ");//314
	// Write Weight Legend- First Chinese, then English
	strUni.Format(L"实际重量");
	nRetVal = printerfont("88", "580", "TSS24.BF2", "0", "1", "1", CW2A(strUni.GetBuffer()));//484
	nRetVal = printerfont("88", "616", "2", "0", "1", "1", "Weight");//520
	// Write Freight Legend- First Chinese, then English
	//strUni.Format(L"运费");
	//nRetVal = printerfont("104", "578", "TSS24.BF2", "0", "1", "1", CW2A(strUni.GetBuffer()));
	//nRetVal = printerfont("88", "612", "2", "0", "1", "1", "Freight");
	// Write Date Legend- First Chinese, then English
	strUni.Format(L"日 期");
	nRetVal = printerfont("96", "676", "TSS24.BF2", "0", "1", "1", CW2A(strUni.GetBuffer()));//582
	nRetVal = printerfont("96", "714", "3", "0", "1", "1", "Date");//618
	// Write Bar Code - Start point X, Y, Code type, height (dots), print text, Rot Angle, Scale X, Scale Y, Text
	if (InspResult.strBarcode.GetLength() > m_pApp->m_stInspParams.nBarcodeLengthMax)
		InspResult.strBarcode = InspResult.strBarcode.Left(m_pApp->m_stInspParams.nBarcodeLengthMax);
	// If chars are Chinese can't encode in 1D barcode!
	if (!IsStringChinese(InspResult.strBarcode))
	{
		str = CW2A(InspResult.strBarcode);
		//	StartX, StartY, Code Type, Code Ht, Print text, RotDeg, scaleX, scaleY, code text
		barcode("144", "120", "128", "76",		"0",		"0",		"2.3",	"1", str.GetBuffer(str.GetLength()));// strUni does not work!
		nRetVal = printerfont("176", "212", "3", "0", "1", "1", str.GetBuffer(str.GetLength()));
	}
	int nRoundedOffLength = InspResult.dLength / 10 + 0.5;
	int nRoundedOffWidth = InspResult.dWidth / 10 + 0.5;
	int nRoundedOffHeight = InspResult.dHeight / 10 + 0.5;
	// Write Measurements - Line 1 (Length)
	strUni.Format(L"长");
	nRetVal = printerfont("280", "282", "TSS24.BF2", "0", "1", "1", CW2A(strUni.GetBuffer()));//218
	str.Format("(L) = %dcm", nRoundedOffLength);
	nRetVal = printerfont("308", "282", "3", "0", "1", "1", str.GetBuffer(str.GetLength()));//242
	// Write Measurements - Line 2 (Width)
	strUni.Format(L"宽");
	nRetVal = printerfont("280", "360", "TSS24.BF2", "0", "1", "1", CW2A(strUni.GetBuffer()));//288
	str.Format("(W) = %dcm", nRoundedOffWidth);
	nRetVal = printerfont("308", "360", "3", "0", "1", "1", str.GetBuffer(str.GetLength()));//288
	// Write Measurements - Line 3 (Height)
	strUni.Format(L"高");
	nRetVal = printerfont("280", "440", "TSS24.BF2", "0", "1", "1", CW2A(strUni.GetBuffer()));//336
	str.Format("(H) = %dcm", nRoundedOffHeight);
	nRetVal = printerfont("308", "440", "3", "0", "1", "1", str.GetBuffer(str.GetLength()));//336
	// Write Measurements - Line 4 (Volume)
	// If air freight write volume weight
	if (InspResult.FreightMode == FREIGHT_AIR)
	{
		strUni.Format(L"体积重");
		nRetVal = printerfont("276", "520", "TSS24.BF2", "0", "1", "1", CW2A(strUni.GetBuffer()));//408
		str.Format("   = %3.2fkg", 1.0 * nRoundedOffLength * nRoundedOffWidth * nRoundedOffHeight / 6000);
		nRetVal = printerfont("328", "520", "3", "0", "1", "1", str.GetBuffer(str.GetLength()));//408
	}
	else // Marine - write normal volume
	{
		strUni.Format(L"体积");
		nRetVal = printerfont("272", "520", "TSS24.BF2", "0", "1", "1", CW2A(strUni.GetBuffer()));//408//448
		//str.Format("(V) = %2.3fm3", InspResult.dVolume);
		str.Format("   = %3.2fm3", 1.0 * nRoundedOffLength * nRoundedOffWidth * nRoundedOffHeight / (1000 * 1000));
		nRetVal = printerfont("316", "520", "3", "0", "1", "1", str.GetBuffer(str.GetLength()));//408
	}
	// Write Weight
	str.Format("%3.2f Kg", InspResult.dWeightNet);
	nRetVal = printerfont("332", "600", "3", "0", "1", "1", str.GetBuffer(str.GetLength()));//500
	
	// Write Date
	CTime time = CTime::GetCurrentTime();
	CString strTotal, strYear, strMonth, strDay;
	strYear.Format(L"%d年-", time.GetYear());
	strMonth.Format(L"%d月-", time.GetMonth());
	strDay.Format(L"%d日", time.GetDay());
	strTotal = strYear + strMonth + strDay;
	nRetVal = printerfont("272", "696", "TSS24.BF2", "0", "1.2", "1.2", CW2A(strTotal.GetBuffer()));//652
	//windowsfont(100, 300, 72, 0, 2, 0, "ariel", "Arial Font 72 pt");

	printlabel("1", "1");
	nRetVal = closeport();
	if (nRetVal)
		IOutputLog(L"Label Printed Successfully!");

	return bResult;
}// PrintLabelCLExpress()

// Assumption: Printer port is already opened and will be closed by the calling function.
BOOL CPMSHJView::CheckPrinterStatus()
{
	BOOL bResult = TRUE;
	int nRetVal = sendcommand("0x1B 0x21 0x3F");
	CString str;
	str.Format(L"Checking Printer Status RetVal: %d", nRetVal);
	IOutputLogString(str);
	if (nRetVal > 1)// 1 is top cover opened! It is returning this number so temporarily bypassing this condition. Ideally should be > 0
		bResult = FALSE;
	//return TRUE;
	return bResult;
}

char* CPMSHJView::hexToAscii(char* hexStr)
{
	char text[sizeof(hexStr) + 1], *dst = text;
	strcpy(text, hexStr);
	char *src = text;
	while (*src != '\0')
	{
		const unsigned char high = hexdigit2int(*src++);
		const unsigned char low = hexdigit2int(*src++);
		*dst++ = (high << 4) | low;
	}
	*dst = '\0';

	return text;
}

unsigned char CPMSHJView::hexdigit2int(unsigned char xd)
{
	if (xd <= '9')
		return xd - '0';
	xd = tolower(xd);
	if (xd == 'a')
		return 10;
	if (xd == 'b')
		return 11;
	if (xd == 'c')
		return 12;
	if (xd == 'd')
		return 13;
	if (xd == 'e')
		return 14;
	if (xd == 'f')
		return 15;
	return 0;
}

BOOL CPMSHJView::IsStringChinese(CString str)
{
	BOOL bResult = FALSE;
	for (int i = 0; i < str.GetLength(); i++)
	{
		TCHAR c = str[i];
		if ((unsigned int)c >= 0x4E00u && ((unsigned int)c <= 0x9FFFu))
		{
			return TRUE;
		}
	}
	return bResult;
}

BOOL CPMSHJView::OnEraseBkgnd(CDC* pDC)
{
	CRect rectRightPane, rectLeftPane, rectLabel;// rectLeftPane is for the left side buttons, rectLabel is for RH (Measurements) Label bkgnd
	GetClientRect(&rectRightPane);
	rectLeftPane.right = 246;
	rectLeftPane.bottom = rectRightPane.bottom;
	rectRightPane.left = rectLeftPane.right;
	rectLabel.left = 856;
	rectLabel.top = 180;
	rectLabel.right = 1320;
	rectLabel.bottom = m_rectImage.bottom;
	int r2 = 31, g2 = 118, b2 = 160; // For Main Panel
	int r1 = 35, g1 = 190, b1 = 200; // For Main Panel
	int r4 = 70, g4 = 140, b4 = 180; // For LHS Panel
	int r3 = 72, g3 = 200, b3 = 210; // For LHS Panel
	int r5 = 67, g5 = 195, b5 = 205; // For RH Label Panel
	int r6 = 66, g6 = 170, b6 = 195; // For RH Label Panel

	for (int i = 0; i < rectRightPane.Height(); i++)
	{
		// Main Panel
		int rR, gR, bR;
		rR = r1 + (i * (r2 - r1) / rectRightPane.Height());
		gR = g1 + (i * (g2 - g1) / rectRightPane.Height());
		bR = b1 + (i * (b2 - b1) / rectRightPane.Height());
		pDC->FillSolidRect(rectRightPane.left, i, rectRightPane.Width(), 1, RGB(rR, gR, bR));
		// LHS Panel
		int rL, gL, bL;
		rL = r3 + (i * (r4 - r3) / rectLeftPane.Height());
		gL = g3 + (i * (g4 - g3) / rectLeftPane.Height());
		bL = b3 + (i * (b4 - b3) / rectLeftPane.Height());
		pDC->FillSolidRect(0, i, rectLeftPane.Width(), 1, RGB(rL, gL, bL));
	}
	// RH (Measurements display) Label Panel
	for (int i = rectLabel.top; i < rectLabel.bottom; i++)
	{
		int rLabel, gLabel, bLabel;
		rLabel = r5 + ((i - rectLabel.top) * (r6 - r5) / rectLabel.Height());
		gLabel = g5 + ((i - rectLabel.top) * (g6 - g5) / rectLabel.Height());
		bLabel = b5 + ((i - rectLabel.top) * (b6- b5) / rectLabel.Height());
		pDC->FillSolidRect(rectLabel.left, i, rectLabel.Width(), 1, RGB(rLabel, gLabel, bLabel));
	}
	// Draw decorative lines at btm of image area and mesurements display panel
	CPen pen(PS_SOLID, 2, RGB(105, 216, 240));
	CPen* pOldPen = pDC->SelectObject(&pen);
	pDC->MoveTo(CPoint(m_rectImage.left-1, m_rectImage.bottom - 16));
	pDC->LineTo(CPoint(m_rectImage.left - 1, m_rectImage.bottom + 1));
	pDC->LineTo(CPoint(m_rectImage.left + 16, m_rectImage.bottom + 1));
	pDC->MoveTo(CPoint(m_rectImage.right+1, m_rectImage.bottom - 16));
	pDC->LineTo(CPoint(m_rectImage.right+1, m_rectImage.bottom + 1));
	pDC->LineTo(CPoint(m_rectImage.right - 16, m_rectImage.bottom + 1));
	// Measurement panel on RH side
	pDC->MoveTo(CPoint(rectLabel.left, rectLabel.bottom - 16));
	pDC->LineTo(CPoint(rectLabel.left, rectLabel.bottom + 1));
	pDC->LineTo(CPoint(rectLabel.left + 16, rectLabel.bottom + 1));
	pDC->MoveTo(CPoint(rectLabel.right, rectLabel.bottom - 16));
	pDC->LineTo(CPoint(rectLabel.right, rectLabel.bottom + 1));
	pDC->LineTo(CPoint(rectLabel.right - 16, rectLabel.bottom + 1));
	pDC->SelectObject(&pOldPen);

	m_StaticTextLength.SetFont(&m_fontStatic);
	m_StaticTextWidth.SetFont(&m_fontStatic);
	m_StaticTextHeight.SetFont(&m_fontStatic);
	return TRUE;

	//return CRecordView::OnEraseBkgnd(pDC);
}

 
void CPMSHJView::OnBnClickedBnStop()
{
	if (!m_b3DLive)
		return;
	
	SetScanBnText(FALSE);
	m_b3DLive = FALSE;
	m_bPlatformCalibON = FALSE;
	SetButtonsState(FALSE);
	if (m_pApp->m_bReadWtThreadUp)
		m_pApp->StopWeighScaleReadThread();

	if (m_pFrame->m_bKBCapture)
		m_pFrame->StopKBCapture();

	SetFocusToHorzBar();

	Sleep(200);// Waiting for weight read thread to end
	SetWeightReadStatus(FALSE);
}

void CPMSHJView::InitFlexGrid()
{
	// Could not find the enums in .h file. So......
	enum {
		flexAlignLeftTop = 0,
		flexAlignLeftCenter = 1,
		flexAlignLeftBottom = 2,
		flexAlignCenterTop = 3,
		flexAlignCenterCenter = 4,
		flexAlignCenterBottom = 5,
		flexAlignRightTop = 6,
		flexAlignRightCenter = 7,
		flexAlignRightBottom = 8,
		flexAlignGeneral = 9
	};

	m_FlexGrid.Clear();

	m_FlexGrid.SetCols(GRID_COLS);
	m_FlexGrid.SetRows(GRID_ROWS);
	m_FlexGrid.SetColWidth(0, 800);
	m_FlexGrid.SetColWidth(1, 3100);
	m_FlexGrid.SetColWidth(2, 5600);
	m_FlexGrid.SetColWidth(3, 2100);
	m_FlexGrid.SetColWidth(4, 4100);

	CPictureHolder picHdr;
	m_FlexGrid.SetRow(0);
	picHdr.CreateFromBitmap(IDB_BITMAP_GRID_COL1);
	m_FlexGrid.SetCol(0);
	m_FlexGrid.SetRefCellPicture(picHdr.GetPictureDispatch());
	picHdr.CreateFromBitmap(IDB_BITMAP_GRID_COL2);
	m_FlexGrid.SetCol(1);
	m_FlexGrid.SetRefCellPicture(picHdr.GetPictureDispatch());
	picHdr.CreateFromBitmap(IDB_BITMAP_GRID_COL3);
	m_FlexGrid.SetCol(2);
	m_FlexGrid.SetRefCellPicture(picHdr.GetPictureDispatch());
	picHdr.CreateFromBitmap(IDB_BITMAP_GRID_COL4);
	m_FlexGrid.SetCol(3);
	m_FlexGrid.SetRefCellPicture(picHdr.GetPictureDispatch());
	picHdr.CreateFromBitmap(IDB_BITMAP_GRID_COL5);
	m_FlexGrid.SetCol(4);
	m_FlexGrid.SetRefCellPicture(picHdr.GetPictureDispatch());

	for (int i = 0; i < GRID_ROWS; i++)
	{
		m_FlexGrid.SetRow(i);
		if (i == 0)
			m_FlexGrid.SetRowHeight(i, 700);
		else
			m_FlexGrid.SetRowHeight(i, 625);
		CString str;
		if (i > 0)
		{
			str.Format(L"%d", i);
			m_FlexGrid.SetTextArray(i * GRID_COLS, str);
		}
		for (int j = 0; j < GRID_COLS; j++)
		{
			m_FlexGrid.SetCol(j);
			if (j == 2)
				m_FlexGrid.SetCellFontSize(14.00);
			else
				m_FlexGrid.SetCellFontSize(14.00);
			m_FlexGrid.SetCellBackColor(RGB(46, 140, 160));
			m_FlexGrid.SetCellAlignment(flexAlignCenterCenter);// flexAlignCenterCenter = 4
		}
	}
	int cols = m_FlexGrid.GetCols();
	//m_FlexGrid.SetTextArray(0, L"Number");
	//m_FlexGrid.SetTextArray(1, L"Barcode");
	//m_FlexGrid.SetTextArray(2, L"Survey Data");
	//m_FlexGrid.SetTextArray(3, L"Weight");
	//m_FlexGrid.SetTextArray(4, L"Time");
} // InitFlexGrid()

void CPMSHJView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	//m_FlexGrid.sc

	CRecordView::OnVScroll(nSBCode, nPos, pScrollBar);
}

// Updates grid with latest saved result
void CPMSHJView::UpdateGrid(STInspResult InspResult)
{
	m_pApp->m_arInspResults.Add(InspResult);
	if (m_pApp->m_arInspResults.GetSize() > (GRID_ROWS - 1))
		m_pApp->m_arInspResults.RemoveAt(0);

	for (int i = 0; i < m_pApp->m_arInspResults.GetSize(); i++)
	{
		CString str;
		int nRowIndex = (m_pApp->m_arInspResults.GetSize() - i);
		// Column 2 - Barcode
		if (m_pApp->m_arInspResults[i].strBarcode.IsEmpty())
			str.Format(L"-");
		else
			str.Format(L"%s", m_pApp->m_arInspResults[i].strBarcode);
		m_FlexGrid.SetTextArray(nRowIndex * GRID_COLS + 1, str);
		// Column 2 - Barcode
		str.Format(L"L:%d-W:%d-H:%d-V:%1.3f", (int)m_pApp->m_arInspResults[i].dLength, (int)m_pApp->m_arInspResults[i].dWidth, (int)m_pApp->m_arInspResults[i].dHeight, m_pApp->m_arInspResults[i].dVolume);
		m_FlexGrid.SetTextArray(nRowIndex * GRID_COLS + 2, str);
		// Column 3 - Weight
		str.Format(L"W%1.3f", m_pApp->m_arInspResults[i].dWeightNet);
		m_FlexGrid.SetTextArray(nRowIndex * GRID_COLS + 3, str);
		// Column 3 - Time
		str = m_pApp->m_arInspResults[i].strTime;
		CString space(" ");
		str = str.Left(19);
		//str.Format(L"%s", m_pSet->m_wk_date);
		m_FlexGrid.SetTextArray(nRowIndex * GRID_COLS + 4, str);
	}

}

void CPMSHJView::DrawCentralTargetRect(cv::Mat& matIn, cv::Scalar)
{
	int TopLeftX = m_rectCentralTarget.CenterPoint().x - m_pApp->m_stInspParams.nSpotRectSize;
	int TopLeftY = m_rectCentralTarget.CenterPoint().y - m_pApp->m_stInspParams.nSpotRectSize;
	int BtmRtX = m_rectCentralTarget.CenterPoint().x + m_pApp->m_stInspParams.nSpotRectSize;
	int BtmRtY = m_rectCentralTarget.CenterPoint().y + m_pApp->m_stInspParams.nSpotRectSize;
	cv::Point pt1 = cv::Point(TopLeftX, TopLeftY);
	cv::Point pt2 = cv::Point(BtmRtX, BtmRtY);
	cv::rectangle(matIn, pt1, pt2, cv::Scalar(255, 0, 0), 2, 8);// if you want to draw a filled  rect use CV_FILLED as line thk
}

void CPMSHJView::OnBnClickedPrint()
{
	m_BnPrint.SetIcon(IDI_PRINT_CLKD);
	PrintLabelCLExpress(m_pApp->m_stInspResultSaved);
	Sleep(200);
	m_BnPrint.SetIcon(IDI_PRINT);
	Invalidate();
}

double CPMSHJView::GetFreight(STInspResult InspResult, STFreightParams stFreightParams, eFreightMode &ActualFreightMode)
{
	CString str;
	str.Format(L"L: %d, W: %d, H: %d, Vol: %3.2f, Wt: %3.2f", (int)InspResult.dLength, (int)InspResult.dWidth, (int)InspResult.dHeight, InspResult.dVolume, InspResult.dWeightNet);
	IOutputLogString(str);
	str.Format(L"FreightMode: %d, Freight (Air): %2.3f, Freight (Sea): %2.3f, , Freight (Wt): %2.3f: %3.2f", stFreightParams.FreightMode, stFreightParams.dAirFreightRate, stFreightParams.dSeaFreightRate, stFreightParams.dWeightFreightRate);
	IOutputLogString(str);
	double dVolFreight = 0.0f, dWtFreight = 0.0;
	int nRoundedOffLengthCM = InspResult.dLength / 10 + 0.5;
	int nRoundedOffWidthCM = InspResult.dWidth / 10 + 0.5;
	int nRoundedOffHeightCM = InspResult.dHeight / 10 + 0.5;
	int nRoundedVolumeInt = nRoundedOffLengthCM * nRoundedOffWidthCM * nRoundedOffHeightCM * 100;
	double dRoundedVolumeCM = nRoundedVolumeInt / 100.0;
	str.Format(L"%3.2f", dRoundedVolumeCM);// Converting to M3 and Rounding off 3rd digit after decimal
	//double dRoundedVolumeCM = 1.0 * nRoundedOffLengthCM * nRoundedOffWidthCM * nRoundedOffHeightCM;
	dRoundedVolumeCM = _wtof(str);
	str.Format(L"%3.2f", dRoundedVolumeCM / (1000 * 1000));// Converting to M3 and Rounding off 3rd digit after decimal
	double dRoundedVolumeM = _wtof(str);
	// Calculate Volume Weight depending on whether it is Air or Sea shipping
	if (stFreightParams.FreightMode == FREIGHT_AIR)
	{
		// If air freight the formula is: Freight  = cm * cm * cm * Rate / 6000 
		double dVolWeight = dRoundedVolumeCM / 6000;
		str.Format(L"%3.2f", dVolWeight);
		dVolWeight = _wtof(str);
		if (dVolWeight > InspResult.dWeightNet)
			dVolFreight = dVolWeight * stFreightParams.dAirFreightRate;
		else
			dVolFreight = InspResult.dWeightNet * stFreightParams.dAirFreightRate;
	}
	else // Sea Freight. Formula is: Freight  = m x m x m x Rate
	{
		dVolFreight = dRoundedVolumeM * stFreightParams.dSeaFreightRate;
		m_pApp->m_stInspResult.dVolume = dRoundedVolumeM;
	}

	str.Format(L"%3.2f Kg", InspResult.dWeightNet);
	InspResult.dWeightNet = _wtof(str);
	dWtFreight = InspResult.dWeightNet * stFreightParams.dWeightFreightRate;

	str.Format(L"Volume: %2.3f, Vol Freight: %3.2f, Wt Freight: %3.2f", dRoundedVolumeM, dVolFreight, dWtFreight);
	IOutputLogString(str);

	if (dWtFreight > dVolFreight)
	{
		ActualFreightMode = FREIGHT_WT;//Freight Mode changes only when freight by weight is more than volume freight
		return dWtFreight;
	}
	else
		return dVolFreight;
} //GetFreight()

void CPMSHJView::OnBnClickedBnAirFreight()
{
	m_pApp->m_stFreightParams.FreightMode = FREIGHT_AIR;
	m_BnSeaFreight.SetIcon(IDI_SEA_FREIGHT);
	m_BnAirFreight.SetIcon(IDI_AIR_FREIGHT_CLKD);
	m_pApp->WriteFreightParamsToRegistry();
	InvalidateRect(&m_rectFreightType, FALSE);
}


void CPMSHJView::OnBnClickedBnSeaFreight()
{
	m_pApp->m_stFreightParams.FreightMode = FREIGHT_SEA;
	m_BnSeaFreight.SetIcon(IDI_SEA_FREIGHT_CLKD);
	m_BnAirFreight.SetIcon(IDI_AIR_FREIGHT);
	m_pApp->WriteFreightParamsToRegistry();
	InvalidateRect(&m_rectFreightType, FALSE);
}

#define BUFSIZE 1024
#define MD5LEN  16
// This function is not used as of now
CString CPMSHJView::GetStringToSendToServer(CString strData, CString strSeed)
{
	CString strToSend;
	if (strData.IsEmpty() || strSeed.IsEmpty())
		return strToSend;
	CString strMsg;
	DWORD dwStatus = 0;
	BOOL bResult = FALSE;
	HCRYPTPROV hProv = 0;
	HCRYPTHASH hHash = 0;
	HANDLE hFile = NULL;
	BYTE rgbFile[BUFSIZE];
	DWORD cbRead = 0;
	BYTE rgbHash[MD5LEN];
	DWORD cbHash = 0;	// Get handle to the crypto provider
	if (!CryptAcquireContext(&hProv,
		NULL,
		NULL,
		PROV_RSA_FULL,
		CRYPT_VERIFYCONTEXT))
	{
		dwStatus = GetLastError();
		strMsg.Format(L"CryptAcquireContext failed: %d\n", dwStatus);
		IOutputLogString(strMsg);
		CloseHandle(hFile);
		return strToSend;
	}

	// Param 3 is the 'key'
	if (!CryptCreateHash(hProv, CALG_MD5, 0, 0, &hHash))
	{
		dwStatus = GetLastError();
		strMsg.Format(L"CryptCreateHash failed: %d\n", dwStatus);
		IOutputLogString(strMsg);
		CloseHandle(hFile);
		CryptReleaseContext(hProv, 0);
		return strToSend;
	}

	if (!CryptHashData(hHash, rgbFile, cbRead, 0))
	{
		dwStatus = GetLastError();
		strMsg.Format(L"CryptCreateHash failed: %d\n", dwStatus);
		IOutputLogString(strMsg);
		CryptReleaseContext(hProv, 0);
		CryptDestroyHash(hHash);
		CloseHandle(hFile);
		return strToSend;
	}
	cbHash = MD5LEN;
	if (CryptGetHashParam(hHash, HP_HASHVAL, rgbHash, &cbHash, 0))
	{
		for (DWORD i = 0; i < cbHash; i++)
		{
			//strMsg.Format(L"%c%c", rgbDigits[rgbHash[i] >> 4],
			//	rgbDigits[rgbHash[i] & 0xf]);
		}
	}
	else
	{

	}
	{
		dwStatus = GetLastError();
		strMsg.Format(L"CryptCreateHash failed: %d\n", dwStatus);
		IOutputLogString(strMsg);
	}

	CryptDestroyHash(hHash);
	CryptReleaseContext(hProv, 0);
	CloseHandle(hFile);

	return strToSend;
}

// Sets a bitmap in the circle just above the grid on right hand side
void CPMSHJView::SetDataLoggingStatus(eDataLoggingStatus DataLoggingStatus)
{
	m_DataLoggingStatus = DataLoggingStatus;
	if (DataLoggingStatus == DATA_OK)
	{
		m_PicDataLoggingStatus.SetBitmap(m_hBitmapDataOK);
		IOutputLog(L"View Logging Data status - OK");
	}
	else if (DataLoggingStatus == DATA_NOT_DONE)
		m_PicDataLoggingStatus.SetBitmap(m_hBitmapDataNotDone);
	else
		m_PicDataLoggingStatus.SetBitmap(m_hBitmapDataFail);
}
