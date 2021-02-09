
// MainFrm.h : interface of the CMainFrame class
//

#pragma once
#include "PMSHJView.h"

typedef BOOL(_cdecl *inshook)(HWND);

class CMainFrame : public CFrameWndEx
{
	
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:
	CPMSHJView*		m_pImageView;
	BOOL				m_bAbortScan;
	BOOL				m_bWeighScaleError;

// Operations
public:
	// Required if you want to hide the window.
	BOOL m_bForceHidden;
	virtual void ActivateFrame(int nCmdShow = -1);
// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	void CreateUDFFile(STUDFFileText& stUDFFileText);
	BOOL CreatePrintFile(STUDFFileText& stUDFFileText, STInspResult stInspResult);
	BOOL ReplaceSystVars(STUDFFileText& stUDFFileText, STInspResult stInspResult);
	BOOL ReplaceHexVars(STUDFFileText& stUDFFileText, STInspResult stInspResult);
	int HexToDecimal(char* hexCharArray, int nNumDigits = 2);
	CString GetSystemDateString();
	CString GetSystemTimeString();
	CString get_date_in_user_format(CTime& time);
	CString get_time_in_user_format(CTime& time);
	BOOL CreatePrintFile();
	CStringA GetDateStringForPrinter();

	// KB Caputure
	void StartKBCapture();
	void StopKBCapture();
	void ReplaceString(CString& csString_io, int& nPos_io);
	inline void LockBuffer() { ::EnterCriticalSection(&m_csLockScanner); }
	inline void UnLockBuffer() { ::LeaveCriticalSection(&m_csLockScanner); }
	inshook				instkbhook;
	inshook				uninstkbhook;
	HINSTANCE			m_hinstDLL;
	BOOL				m_bKBCapture;
	BOOL				m_bBarcodeReceived;
	CStringArray		m_csArray;
	CRITICAL_SECTION	m_csLockScanner;// For reading from barcode scanner
	LPWSTR				m_csKey;
	//
	int					m_nTimeStamp;
	int					m_nTimeStampOld;
	int					m_nStartTimeSecs;
	CString				m_strTotal;
	CStringArray		m_arUDFFileStrings;

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CMFCMenuBar       m_wndMenuBar;
	CMFCToolBar       m_wndToolBar;

// Generated message map functions
protected:
	afx_msg LRESULT OnKeyStroke(WPARAM wParam, LPARAM lParam);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnViewCustomize();
	afx_msg LRESULT OnToolbarCreateNew(WPARAM wp, LPARAM lp);
	afx_msg void OnApplicationLook(UINT id);
	afx_msg void OnUpdateApplicationLook(CCmdUI* pCmdUI);
	afx_msg LRESULT OnUpdateWeight(WPARAM wParam, LPARAM lParam);
	//afx_msg LRESULT OnUpdateCode(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnRcdFootSwitchTrigger(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnRcdFootSwitchReadFail(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnRcdDataLogStatus(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnRcdWeighScaleError(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnClose();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};


