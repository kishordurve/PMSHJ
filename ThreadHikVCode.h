#pragma once
#include "MVIDCodeReader.h"

// CThreadHikVCode

class CThreadHikVCode : public CWinThread
{
	DECLARE_DYNCREATE(CThreadHikVCode)

public:
	CThreadHikVCode();           // protected constructor used by dynamic creation
	CThreadHikVCode(int nBarcodeLengthMin, int nBarcodeLengthMax, char cEndChar);
	virtual ~CThreadHikVCode();

public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

	HANDLE m_hEndScannerThread;

	MVID_CAMERA_INFO_LIST*  m_pstDevList;
	MVID_CAM_OUTPUT_INFO*   m_pstOutput;
	void*                   m_handle;
	BOOL                    m_bProcess;
	BOOL					m_bIsNewCode;
	CString					m_strCode;
	CString					m_strOldCode;
	int						m_nBarcodeLengthMin;
	int						m_nBarcodeLengthMax;
	char					m_cEndChar;
	eCodeType				m_CodeTypeRqd;
	// Code points
	CPoint m_pt1;
	CPoint m_pt2;
	CPoint m_pt3;
	CPoint m_pt4;

	void ImageCallBack(MVID_CAM_OUTPUT_INFO* pFrameInfo);

protected:
	DECLARE_MESSAGE_MAP()
};


