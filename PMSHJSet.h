
// PMSHJSet.h: interface of the CPMSHJSet class
//


#pragma once

// code generated on Wednesday, August 7, 2019, 2:16 PM

class CPMSHJSet : public CRecordset
{
public:
	CPMSHJSet(CDatabase* pDatabase = NULL);
	DECLARE_DYNAMIC(CPMSHJSet)

// Field/Param Data

// The string types below (if present) reflect the actual data type of the
// database field - CStringA for ANSI datatypes and CStringW for Unicode
// datatypes. This is to prevent the ODBC driver from performing potentially
// unnecessary conversions.  If you wish, you may change these members to
// CString types and the ODBC driver will perform all necessary conversions.
// (Note: You must use an ODBC driver version that is version 3.5 or greater
// to support both Unicode and these conversions).

	CStringW	m_sta_name;
	CStringW	m_data_stamp;
	CStringW	m_wk_date;
	CStringW	m_tran_no;
	CStringW	m_pack_no;
	double		m_pack_l;
	double		m_pack_w;
	double		m_pack_h;
	double		m_pack_v;
	CStringW	m_v_unit;
	double		m_pack_net;
	double		m_pack_gross;
	double		m_pack_tare;
	double		m_pack_ptare;
	CStringW	m_wt_unit;
	BYTE		m_upd_flag;
	BYTE		m_del_mark;

// Overrides
	// Wizard generated virtual function overrides
	public:
	virtual CString GetDefaultConnect();	// Default connection string

	virtual CString GetDefaultSQL(); 	// default SQL for Recordset
	virtual void DoFieldExchange(CFieldExchange* pFX);	// RFX support

// Implementation
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

};

