// Registry.h: interface for the CRegistry class.
// Gurupada Bera : Dated 04.12.2005
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REGISTRY_H__44AC5691_A9C5_11D6_A647_0050BA8EB165__INCLUDED_)
#define AFX_REGISTRY_H__44AC5691_A9C5_11D6_A647_0050BA8EB165__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CRegistry  
{
	HKEY m_hSoftKey,
		 m_hCompanyKey,
		 m_hAppKey;

	BOOL m_bRegistryClosed;

public:
	LONG DeleteKey(LPCTSTR lpszSection, LPCTSTR lpszKeyName);
	CString ReadStringValue (LPCTSTR szSectionName, LPCTSTR szKeyName, LPCTSTR& sVal);
	DWORD ReadIntegerValue(LPCTSTR strSectionName, LPCTSTR strKeyName, DWORD dwValue);
	HKEY GetSectionKey(LPCTSTR lpszSection);
	eRegDataType GetRegistryDataType(LPCTSTR strSectionName, LPCTSTR strKeyName);
	void CloseRegistry();
	LONG OpenRegistry();
	CRegistry();
	virtual ~CRegistry();
};

#endif // !defined(AFX_REGISTRY_H__44AC5691_A9C5_11D6_A647_0050BA8EB165__INCLUDED_)
