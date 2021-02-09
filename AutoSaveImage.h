// AutoSaveImage.h: interface for the CAutoSaveImage class.
// V1.30 - This file is added in this version.
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AUTOSAVEIMAGE_H__99D9AC71_ABEB_11D5_9ACF_0050BA8D7822__INCLUDED_)
#define AFX_AUTOSAVEIMAGE_H__99D9AC71_ABEB_11D5_9ACF_0050BA8D7822__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define DEFAULT_IMAGE_COUNT		100000
#define DEFAULT_IMAGE_REPORT_COUNT		100000
#define MIN_IMAGE_COUNT			1
#define MAX_IMAGE_COUNT			100000
#include <opencv/highgui.h>
#include <opencv/cv.h>

class CAutoSaveImage  
{
	CFileStatus m_FileStatus;
	BOOL m_bRetVal;
	CString m_strFileName;	

// Attributes
public:
	BOOL	m_bSaveFail,
			m_bSavePass,
			m_bSaveAll,
			m_bSaveReportImage;

	int		m_nMaxCountPass,
			m_nMaxCountFail,
			m_nMaxCountAll,
			m_nMaxCountReport;

	int		m_nImageCountPass,
			m_nImageCountFail,
			m_nImageCountAll,
			m_nImageCountReport,
			m_nScanCount;

	int     m_nTempCount;

	CString m_strFolderName,
			m_strImageFormat;
	CString	m_strAllImgDir;
	CString m_strCurScanDataFolder;
	CString m_strCurScanDataItemFolder;

// Operations
public:
	CString GetPassImagesFolder();
	CString GetFailImagesFolder();
	CString GetReportImagesFolder();
	void CreateImageFolders();
	void SavePassImage(IplImage* InImage);
	void SaveFailImage(IplImage* InImage);
	void SaveAllImages(IplImage* InImage);
	void SaveReportImage(IplImage* InImage);
	void SaveColorImageToScanFolder(IplImage* InImageColor);
	CString GetNewScanDataFolder();
	CString GetCurScanDataFolder();

	CAutoSaveImage(CString strFileFormat = L"jpg");
	virtual ~CAutoSaveImage();
};

#endif // !defined(AFX_AUTOSAVEIMAGE_H__99D9AC71_ABEB_11D5_9ACF_0050BA8D7822__INCLUDED_)
