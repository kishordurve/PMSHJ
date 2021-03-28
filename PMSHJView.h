
// PMSHJView.h : interface of the CPMSHJView class
//

#pragma once
#include "afxwin.h"
#include <Opencv.hpp>
#include "PMSHJDoc.h"
#include "xShadeButton.h"
#include <OpenNI.h>
#include "afxbutton.h"
#include "labelcontrol.h"
#include "BlobResult.h"
#include "CImage.h"
#include "label_gross_weight3.h"
#include "Structures.h"
#include "msflexgrid.h"

class CPMSHJSet;
using namespace std;
//using namespace cv;
using namespace openni;

class CPMSHJDoc;
class CPMSHJApp;
class CMainFrame;

class CPMSHJView : public CRecordView
{
protected: // create from serialization only
	CPMSHJView();
	DECLARE_DYNCREATE(CPMSHJView)

public:
#ifdef AFX_DESIGN_TIME
	enum{ IDD = IDD_PMSHJ_FORM };
#endif
	CPMSHJSet* m_pSet;

// Attributes
public:
	CPMSHJDoc*	GetDocument() const;
	CPMSHJApp*	m_pApp;
	CMainFrame*	m_pFrame;
	HBITMAP		m_img;

	BOOL		m_b3DLive;
	BOOL		m_bCOMPortStarted;
	BOOL		m_bBoxResultReady;
	BOOL		m_bScanDone;
	BOOL		m_bImageLoaded;
	BOOL		m_bNewImage;
	BOOL		m_bLogoUploadedToPrinter;
	BOOL		m_bPkgNumGreen;
	BOOL		m_bAllFramesGrabbed;
	BOOL		m_bDepthScanDone;
	BOOL		m_bWeightScanDone;
	BOOL		m_bReadWeightOnce;
	BOOL		m_bDecimalSeparatorDot;// It is either dot or comma
	BOOL		m_bFootSwithTrigRcd;
	BOOL		m_NewDepthFrame;
	//BOOL		m_bPlatformDataReadOK;
	BOOL		m_bPlatformCalibON;
	BOOL		m_bWtStatus;
	BOOL		m_bScanStatus;
	BOOL		m_bPrinterPortOpened;

	CIHighResTimer	m_HighResTimer;
	eWtUnits		m_eWtUnits;

	int			m_nThreshold;
	size_t		m_nImagesGrabbed;
	size_t		m_nImagesGrabbedContinuously;
	double		m_dCurWt;

	CRect		m_rectPlatform;
	CRect		m_rectROI;
	CRect		m_rectCentralTarget;
	CRect		m_rectImage;
	CRect		m_rectBnPanel;
	CRect		m_rectFreightType;
	CvBox2D		m_Box2D;
	CvBox2D		m_Box2DTransToPlat;
	cv::Mat		m_ZDistanceMap;
	//cv::Mat		m_XDistanceMap;
	//cv::Mat		m_YDistanceMap;
	//cv::Mat		m_XPlatform;
	//cv::Mat		m_YPlatform;
	//cv::Mat		m_ZPlatform;
	CvFont*		m_font;
	eDataLoggingStatus m_DataLoggingStatus;
	eBarCodeReadStatus m_BarCodeReadStatus;

	CString		m_strBoxWidth;
	CString		m_strBoxLength;
	CString		m_strBoxHeight;
	CString		m_strBoxVolume;
	CString		m_strBarcode;
	double		m_dBoxLength;
	double		m_dBoxWidth;
	double		m_dBoxHeight;
	double		m_dBoxVolume;
	double		m_dBoxWeightNet;
	double		m_dBoxWeightGr;
	double		m_dBoxWeightTare;
	double		m_dBoxWeightPTare;
	double		m_dAvgZ;
	BOOL		m_bWtRead;
	CString		m_strWeight;
	CString		m_strAccuracy;
	CString		m_strMsg;
	CPoint		m_ptHighest;
	std::vector<cv::Point2f>	points2D;
	CArray<CPoint, CPoint>		m_arMeasurementPoints;
	CArray<double, double>		m_arZReadings;
	CArray<double, double>		m_arLength;
	CArray<double, double>		m_arWidth;
	CArray<double, double>		m_arHeight;
	CArray<CPoint, CPoint>		m_arPeripheryPoints;
	CArray<CPoint, CPoint>		m_arRayTracingBoundaryPoints;
	CArray<CPoint, CPoint>		m_arRayTracingBoundaryPointsTransToPlat;
	double						m_dOldLength;
	double						m_dOldWidth;
	double						m_dOldHeight;
	HBITMAP m_hBitmapDataOK, m_hBitmapDataNotDone, m_hBitmapDataFail;
	// Declaring CImage creates clash with ATL::CImage
	IplImage*		m_imgDepthGray;
	IplImage*		m_imgMask;
	IplImage*		img;
	IplImage*		dst;
	IplImage*		m_PlatformMask;
	IplImage*		m_PlatformLevelGray;// This image will hold the image transformed to platform level
	HBITMAP			m_BitmapDisplay;
	HBITMAP			m_hBitmapFreightSliderON;
	HBITMAP			m_hBitmapFreightSliderOFF;
	//Used to display User Guide Messages
	CString			m_strMessage;
	BOOL			m_bDisplayMsg;

	// Orbbec camera specific variables
	cv::Mat			m_MatDepth;
	cv::Mat			m_MatColor;
	cv::Mat			m_MatColorSmall;// 532 x 376
	double			m_dDepthMin;
	double			m_dDepthMax;
	cv::Mat			adjustDepth(const cv::Mat& inImage);
	CFont			m_fontStatic;
	//
	// Operations
public:
	void	Show(cv::Mat img, HDC dc, int x, int y, int w, int h, int from_x, int from_y);
	void	FillBitmapInfo(BITMAPINFO* bmi, int width, int height, int bpp, int origin);
	void	SetFocusToHorzBar();
	void	SetMessage(CString str, COLORREF color = RGB(0,255,0));
	void	ShowScanID(CString str);
	void	SetButtonsState(BOOL bScanON);
	void	SetScanBnText(BOOL bLive);
	BOOL	AreDimReadingsStable(int nMaxAllDev = 10);
	void	Scan();
	void	SetWeightReadStatus(BOOL bOK);

	BOOL	ImageLiveStart3DOrbbec(int nNoOfFrames = 10000, BOOL bFindBoxDims = TRUE);
	BOOL	Process3DDataOrbbec(openni::VideoStream& m_depthStream, cv::Mat MatDepth, int ImagesGrabbed, BOOL bCuboidBox);
	double	GetMinDropForHeight(double dAvg);
	double	GetRefinedZVal(double dRawAvg, CArray<double, double>& arZDepths, int nAllowableError);
	void	ShowDimensions(CString strBoxLength, CString strBoxWidth, CString strBoxHeight, CString strWeight, CString strVolume);
	void	UpdateWeight(double dWtNet, double dWtGross, double dWtTare, double dWtPreTare, CString strWtUnits, int nErrorFlag);
	void	UpdateWeightGRAM(double dWtNet, CString strWtUnits, int nErrorFlag);// GRAM Scale
	double	ApplyXYorrection(double& dX, double& dY, double dZ);
	HBITMAP Mat2DIB(cv::Mat *Image);
	void	SetMessage(int nStringResourceNum, COLORREF color = RGB(0, 255, 0));
	void	AddMessage(CString strMsgAdd, COLORREF color = RGB(0, 255, 0));
	void	SetDimUnits(eDimUnits DimUnits);
	void	ChangeLaguage(eLanguage NewLanguage);
	void	SetDecimalSeparator();
	void	ModifyDisplyDimsForNewUnits(eDimUnits NewUnits);
	double	GetPlatformHeight(openni::VideoStream& depthStream, cv::Mat MatDepth, CRect rect);
	BOOL	CalibForPlatformHeight();
	BOOL	CalibForCamAlignment();
	void	DisplayWtLabels();// Hides weight labels which are not rqd
	BOOL	PrintLabelHJ(STInspResult InspResult);
	BOOL	PrintLabelCLExpress(STInspResult InspResult);
	BOOL	CheckPrinterStatus();
	void	InitFlexGrid();
	void	UpdateGrid(STInspResult InspResult);
	void	DrawCentralTargetRect(cv::Mat& matIn, cv::Scalar color);
	double	GetFreight(STInspResult InspResult, STFreightParams stFreightParams, eFreightMode &ActualFreightMode);
	CString	GetStringToSendToServer(CString strData, CString strSeed);
	void	SetDataLoggingStatus(eDataLoggingStatus DataLoggingStatus);
	void	SetCodeReadStatus(eBarCodeReadStatus BarCodeReadStatus);// Sets bitmap in the same picture control as data logging

	CvRect	CRectToCVRect(CRect rect)
	{
		CvRect cvRect;
		cvRect.x = rect.left; cvRect.y = rect.top; cvRect.width = rect.Width() + 1; cvRect.height = rect.Height() + 1;
		return cvRect;
	}
	CRect	CvRectToCRect(CvRect rect)
	{
		CRect cRect;
		cRect.left = rect.x; cRect.top = rect.y; cRect.right = rect.x + rect.width; cRect.bottom = rect.y + rect.height;
		return cRect;
	}
	char* hexToAscii(char* hexStr);
	unsigned char hexdigit2int(unsigned char xd);
	BOOL IsStringChinese(CString str);
	std::string CStringTostdstring(CString str)
	{
		std::string stdString(CW2A(str.GetString()));
		return stdString;
	}
	CString stdstringToCString(std::string str)
	{
		CString MFCString(str.c_str());
		return MFCString;
	}

	// Overrides
public:
	virtual CRecordset* OnGetRecordset();
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

	void SetButtonsNLabels();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnInitialUpdate(); // called first time after construct
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Implementation
public:
	virtual ~CPMSHJView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	// Zeroes the current weight
	CxShadeButton m_BnSettings;
	CxShadeButton m_BnReport;
	CxShadeButton m_BnExit;
	CxShadeButton m_BnScan;
	CxShadeButton m_BnPrint;
	CxShadeButton m_BnStop;
	CxShadeButton m_BnAirFreight;
	CxShadeButton m_BnSeaFreight;
	afx_msg void OnBnClickedBnExit();
	afx_msg void OnBnClickedBnZeroWt();
	afx_msg void OnBnClickedBnSettings();
	virtual BOOL OnMove(UINT nIDMoveCommand);
	afx_msg void OnBnClickedBnScan();
	afx_msg void OnBnClickedBnReport();
	virtual void OnDraw(CDC* /*pDC*/);
	// Length Caption
	CLabelControl m_LabelCapLength;
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnBnClickedBnStop();
	CMSFlexGrid m_FlexGrid;
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	CStatic m_StaticTextLength;
	CStatic m_StaticTextWidth;
	CStatic m_StaticTextHeight;
	CStatic m_StaticTextVolume;
	CStatic m_StaticTextWeight;
	afx_msg void OnBnClickedPrint();
	afx_msg void OnBnClickedBnAirFreight();
	afx_msg void OnBnClickedBnSeaFreight();
	CStatic m_PicDataLoggingStatus;
};

#ifndef _DEBUG  // debug version in PMSHJView.cpp
inline CPMSHJDoc* CPMSHJView::GetDocument() const
   { return reinterpret_cast<CPMSHJDoc*>(m_pDocument); }
#endif

