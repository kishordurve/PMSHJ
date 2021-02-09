
// PMSHJSet.cpp : implementation of the CPMSHJSet class
//

#include "stdafx.h"
#include "PMSHJ.h"
#include "PMSHJSet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CPMSHJSet implementation

// code generated on Wednesday, August 7, 2019, 2:16 PM

IMPLEMENT_DYNAMIC(CPMSHJSet, CRecordset)

CPMSHJSet::CPMSHJSet(CDatabase* pdb)
	: CRecordset(pdb)
{
	m_sta_name = L"";
	m_data_stamp = L"";
	m_wk_date = L"";
	m_tran_no = L"";
	m_pack_no = L"";
	m_pack_l = 0.0;
	m_pack_w = 0.0;
	m_pack_h = 0.0;
	m_pack_v = 0.0;
	m_v_unit = L"";
	m_pack_net = 0.0;
	m_pack_gross = 0.0;
	m_pack_tare = 0.0;
	m_pack_ptare = 0.0;
	m_wt_unit = L"";
	m_upd_flag = 0;
	m_del_mark = 0;
	m_nFields = 17;
	m_nDefaultType = dynaset;
}
//#error Security Issue: The connection string may contain a password
// The connection string below may contain plain text passwords and/or
// other sensitive information. Please remove the #error after reviewing
// the connection string for any security related issues. You may want to
// store the password in some other form or use a different user authentication.
CString CPMSHJSet::GetDefaultConnect()
{
	CString strDBFilePath;
	CPMSHJApp* pApp = (CPMSHJApp*)AfxGetApp();
	strDBFilePath = pApp->GetProjectDirectory();
	CString strTarget;
	strTarget.Format(L"DSN=PMS;DBQ=%s\\PMS.mdb;DriverId=25;FIL=MS Access;MaxBufferSize=2048;PageTimeout=5;UID=admin;", strDBFilePath);
	return strTarget;
	//return _T("DSN=PMS;DBQ=D:\\PMS.mdb;DriverId=25;FIL=MS Access;MaxBufferSize=2048;PageTimeout=5;UID=admin;");
}

CString CPMSHJSet::GetDefaultSQL()
{
	return _T("[mlog]");
}

void CPMSHJSet::DoFieldExchange(CFieldExchange* pFX)
{
	pFX->SetFieldType(CFieldExchange::outputColumn);
// Macros such as RFX_Text() and RFX_Int() are dependent on the
// type of the member variable, not the type of the field in the database.
// ODBC will try to automatically convert the column value to the requested type
	RFX_Text(pFX, _T("[sta_name]"), m_sta_name);
	RFX_Text(pFX, _T("[data_stamp]"), m_data_stamp);
	RFX_Text(pFX, _T("[wk_date]"), m_wk_date);
	RFX_Text(pFX, _T("[tran_no]"), m_tran_no);
	RFX_Text(pFX, _T("[pack_no]"), m_pack_no);
	RFX_Double(pFX, _T("[pack_l]"), m_pack_l);
	RFX_Double(pFX, _T("[pack_w]"), m_pack_w);
	RFX_Double(pFX, _T("[pack_h]"), m_pack_h);
	RFX_Double(pFX, _T("[pack_v]"), m_pack_v);
	RFX_Text(pFX, _T("[v_unit]"), m_v_unit);
	RFX_Double(pFX, _T("[pack_net]"), m_pack_net);
	RFX_Double(pFX, _T("[pack_gross]"), m_pack_gross);
	RFX_Double(pFX, _T("[pack_tare]"), m_pack_tare);
	RFX_Double(pFX, _T("[pack_ptare]"), m_pack_ptare);
	RFX_Text(pFX, _T("[wt_unit]"), m_wt_unit);
	RFX_Byte(pFX, _T("[upd_flag]"), m_upd_flag);
	RFX_Byte(pFX, _T("[del_mark]"), m_del_mark);

}
/////////////////////////////////////////////////////////////////////////////
// CPMSHJSet diagnostics

#ifdef _DEBUG
void CPMSHJSet::AssertValid() const
{
	CRecordset::AssertValid();
}

void CPMSHJSet::Dump(CDumpContext& dc) const
{
	CRecordset::Dump(dc);
}
#endif //_DEBUG

