// AutoSaveImage.cpp: implementation of the CAutoSaveImage class.
// V1.30 - This file is added in this version.
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PMSHJ.h"
#include "AutoSaveImage.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// Default Constructor
// 
CAutoSaveImage::CAutoSaveImage(CString strFileFormat)
{
	CPMSHJApp* pApp = (CPMSHJApp*) AfxGetApp();
	CString str("");
	str.Format(L"Image Log");
	m_strImageFormat				= strFileFormat;
	m_strFolderName					= str;
	m_bSaveReportImage				=  pApp->GetProfileInt(L"Options", L"SaveReportImage",1);//v1.0.0.1
	m_nMaxCountPass					= 1;
	m_nMaxCountFail					= 1;
	m_nMaxCountAll					= 1;
	m_nMaxCountReport				= 1;
	m_nImageCountPass				= 1;
	m_nImageCountFail				= 1;
	m_nImageCountAll				= 1;
	m_nScanCount					= 1;

	m_bRetVal						= FALSE;
	m_strFileName.Empty();

	m_nTempCount = 0;

	int nOption = pApp->GetProfileInt(L"Options", L"SaveFailImage", 1); // 1 indicates true
	if (nOption)
		m_bSaveFail = true;
	else
		m_bSaveFail = false;

	m_nMaxCountFail = pApp->GetProfileInt(L"Options", L"MaxFailImageCount", DEFAULT_IMAGE_COUNT);


	if (m_nMaxCountFail < MIN_IMAGE_COUNT || m_nMaxCountFail > MAX_IMAGE_COUNT)
 		m_nMaxCountFail = DEFAULT_IMAGE_COUNT;
	
	nOption = pApp->GetProfileInt(L"Options", L"SavePassImage", 1); // 1 indicates true
	if (nOption)
		m_bSavePass = true;
	else
		m_bSavePass = false;

	m_nMaxCountPass = pApp->GetProfileInt(L"Options", L"MaxPassImageCount", DEFAULT_IMAGE_COUNT);

	if (m_nMaxCountPass < MIN_IMAGE_COUNT || m_nMaxCountPass > MAX_IMAGE_COUNT)
 		m_nMaxCountPass = DEFAULT_IMAGE_COUNT;

	nOption = pApp->GetProfileInt(L"Options", L"SaveAllImage", 0); // 1 indicates true
	if (nOption)
		m_bSaveAll = true;
	else
		m_bSaveAll = false;

	m_nMaxCountAll = pApp->GetProfileInt(L"Options", L"MaxAllImageCount", DEFAULT_IMAGE_COUNT);

	if (m_nMaxCountAll < MIN_IMAGE_COUNT || m_nMaxCountAll > MAX_IMAGE_COUNT)
 		m_nMaxCountAll = DEFAULT_IMAGE_COUNT;

	m_nMaxCountReport = pApp->GetProfileInt(L"Options", L"MaxReportImageCount", DEFAULT_IMAGE_REPORT_COUNT);

	if (m_nMaxCountReport < MIN_IMAGE_COUNT || m_nMaxCountReport > MAX_IMAGE_COUNT)
 		m_nMaxCountReport = DEFAULT_IMAGE_COUNT;

	m_strAllImgDir = pApp->IGetProfileString(L"Options", L"SaveAllImagePath", L"");
	if (m_strAllImgDir == "")
		m_strAllImgDir = pApp->m_strProjectDir;

	CreateImageFolders();

} // Constructor

CAutoSaveImage::~CAutoSaveImage()
{
	CPMSHJApp* pApp = (CPMSHJApp*) AfxGetApp();
	pApp->WriteProfileInt(L"Options", L"SaveFailImage", m_bSaveFail);
	pApp->WriteProfileInt(L"Options", L"SavePassImage", m_bSavePass);

} // Destructor


//****************************************************************************
// Function		:	SaveFailImage
//
// Description	:	This function saves the images for the max count.
//					saves the image in the desired format. When max count
//					is achieved, the images are overwriten.
//					The images are stored in "Image Log\\Fail Images" folder under project dir
// Inputs		:   SourceBuffer,camera number
//             
// Returns		:	void
//****************************************************************************
void CAutoSaveImage::SaveFailImage(IplImage* InImage)
{
	CPMSHJApp *m_pApp = (CPMSHJApp*) AfxGetApp();	

} // SaveFailImage

//****************************************************************************
// Function		:	SavePassImage
//
// Description	:	This function saves the images for the max count.
//					saves the image in the desired format. When max count
//					is achieved, the images are overwriten.
//					The images are stored in "Image Log\\Pass Images" folder under project dir
//					the images among the camera one,two, three and four				
// Inputs		:   SourceBuffer,camera number
//             
// Returns		:	void
//****************************************************************************
void CAutoSaveImage::SavePassImage(IplImage* InImage)
{
	CPMSHJApp* m_pApp=(CPMSHJApp*)AfxGetApp();
}

void CAutoSaveImage::SaveAllImages(IplImage* InImage)
{	

} // SaveAllImage


// Creates all image folders 'Failed Images', 'Pass Images' under 'Images' in the pApp->m_strProjectDir folder
// 
void CAutoSaveImage::CreateImageFolders()
{
}

CString CAutoSaveImage::GetFailImagesFolder()
{
 	CPMSHJApp* pApp = (CPMSHJApp*) AfxGetApp(); 
	CString str = pApp->m_strProjectDir;
	str += "\\Image Log\\Failed Images";

	return str;
}

CString CAutoSaveImage::GetPassImagesFolder()
{
 	CPMSHJApp* pApp = (CPMSHJApp*) AfxGetApp(); 
	CString str = pApp->m_strProjectDir;
	str += "\\Image Log\\Pass Images";

	return str;
}
CString CAutoSaveImage::GetReportImagesFolder()
{
 	CPMSHJApp* pApp = (CPMSHJApp*) AfxGetApp(); 
	CString str = pApp->m_strProjectDir;
	str += "\\Image Log\\Report Images";

	return str;
}

//****************************************************************************
// Function		:	SaveReportImage
//
// Description	:	This function saves the images for the max count.
//					saves the image in the desired format. When max count
//					is achieved, the images are overwriten.
//					The images are stored in "Image Log\\Report Images" folder under project dir
// Inputs		:   SourceBuffer
//             
// Returns		:	void
//****************************************************************************
void CAutoSaveImage::SaveReportImage(IplImage* InImage)
{
} // SaveFailImage

void CAutoSaveImage::SaveColorImageToScanFolder(IplImage* InImageColor)
{
	CPMSHJApp *pApp = (CPMSHJApp*)AfxGetApp();
	CString strScanDataFolder;// = GetNewScanDataFolder();

	if ((pApp->m_bSameScanID) && !pApp->m_bStartingNewMultiItemScan)
		strScanDataFolder = GetCurScanDataFolder(); // Current Scan ID folder only without new order folder being created
	else
		strScanDataFolder = GetNewScanDataFolder(); // Create new folder using timestamp

	CString strItemFolder;
	//Create item folder if necessary - necessary when same scan ID but new item
	if (pApp->m_bSameScanID)
	{
		SetCurrentDirectory(strScanDataFolder);
		strItemFolder.Format(L"Item%d", pApp->m_stScanData.nNumItems);
		m_bRetVal = CFile::GetStatus(strItemFolder, m_FileStatus);
		if (!m_bRetVal)
			m_bRetVal = CreateDirectory(strItemFolder, NULL);
		if (m_bRetVal)
		{
			m_strCurScanDataItemFolder = strScanDataFolder + "\\" + strItemFolder;
		}
	}
	else
	{
		m_strCurScanDataItemFolder = pApp->m_strProjectDir + "\\" + strScanDataFolder;
	}

	if (m_strCurScanDataItemFolder.IsEmpty())
		return;

	// Save 3D Image
	if (InImageColor)
	{
		m_strFileName.Format(L"%s\\Camera2D.%s", m_strCurScanDataItemFolder, m_strImageFormat);
		std::string str(CT2A(m_strFileName.GetBuffer()));
		cvSaveImage(str.c_str(), InImageColor);
	}
}

// This func creates Scan data folder (if not existing) with time stamp and returns its name.
// 
CString CAutoSaveImage::GetNewScanDataFolder()
{
	CPMSHJApp *pApp = (CPMSHJApp*)AfxGetApp();
	SetCurrentDirectory(pApp->m_strProjectDir);
	//current time in label as the timer is set to 60mins
	CTime time = CTime::GetCurrentTime();
	CString strFolder, strYear, strMonth, strDay,strTimeHr,strTimeMin,strTimeSec,strEmpty;
	// Year
	strYear.Format(L"%d", time.GetYear());
	strYear = strYear.Right(2);
	// Month
	strMonth.Format(L"%d", time.GetMonth());
	int nMonthChar = strMonth.GetLength();
	if (nMonthChar == 1)
		strMonth.Format(L"0%d", time.GetMonth());
	//Day
	strDay.Format(L"%d", time.GetDay());
	int nDayChar = strDay.GetLength();
	if (nDayChar == 1)
		strDay.Format(L"0%d", time.GetDay());
	//TimeHour
	strTimeHr.Format(L"%d", time.GetHour());
	int nHourChar = strTimeHr.GetLength();
	if (nHourChar == 1)
		strTimeHr.Format(L"0%d", time.GetHour());
	//TimeMin
	strTimeMin.Format(L"%d", time.GetMinute());
	int nMinChar = strTimeMin.GetLength();
	if(nMinChar == 1)
		strTimeMin.Format(L"0%d", time.GetMinute());
	//TimeSec
	strTimeSec.Format(L"%d", time.GetSecond());
	int nSecChar = strTimeSec.GetLength();
	if (nSecChar == 1)
		strTimeSec.Format(L"0%d", time.GetSecond());
	strTimeSec.Left(2);
	
	CFileStatus fileStatus;	//strFolder.Format("%s\\%d_%s%s%s_%d%d%d", g_strScanData, m_nScanCount, strYear, strMonth, strDay, time.GetHour(), time.GetMinute(), time.GetSecond());
	strFolder.Format(L"%s\\%s%s%s_%s%s%s", g_strScanData, strYear, strMonth, strDay, strTimeHr, strTimeMin, strTimeSec);
	m_bRetVal = CFile::GetStatus(strFolder, fileStatus);
	if (!m_bRetVal)
		m_bRetVal = CreateDirectory(strFolder, NULL);
	if (m_bRetVal)
	{
		m_strCurScanDataFolder = strFolder;
		return strFolder;
	}
	else
		return strEmpty;

} // GetNewScanDataFolder()

CString CAutoSaveImage::GetCurScanDataFolder()
{
	CPMSHJApp *pApp = (CPMSHJApp*)AfxGetApp();
	if (m_strCurScanDataFolder.IsEmpty())
		m_strCurScanDataFolder = GetNewScanDataFolder();
	CString strFolder = pApp->m_strProjectDir + "\\" + m_strCurScanDataFolder;
	
	return strFolder;
}
