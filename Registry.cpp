// Registry.cpp: implementation of the CRegistry class.
// Gurupada Bera : Dated 04.12.2005
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PMSHJ.h"
#include "Registry.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRegistry::CRegistry()
{
	m_hSoftKey = NULL;
	m_hCompanyKey = NULL;
	m_hAppKey = NULL;
	m_bRegistryClosed = TRUE;
}

CRegistry::~CRegistry()
{
	CloseRegistry();
}

LONG CRegistry::OpenRegistry()
{
	CPMSHJApp *pApp = (CPMSHJApp*)AfxGetApp();

	LONG lRet = 0;

	ASSERT(pApp->m_pszRegistryKey != NULL);
	ASSERT(pApp->m_pszProfileName != NULL);

	const TCHAR szSoftware[] = _T("Software");

	if (RegOpenKeyEx(HKEY_CURRENT_USER, szSoftware, 0, KEY_WRITE|KEY_READ, &m_hSoftKey) == ERROR_SUCCESS)
	{
		if (RegOpenKeyEx(m_hSoftKey, pApp->m_pszRegistryKey, 0, KEY_WRITE|KEY_READ, &m_hCompanyKey) == ERROR_SUCCESS)
				lRet = RegOpenKeyEx(m_hCompanyKey, pApp->m_pszProfileName, 0, KEY_WRITE|KEY_READ, &m_hAppKey);
		
	}

	m_bRegistryClosed = FALSE;

	return lRet;
}//OpenRegistry

void CRegistry::CloseRegistry()
{
	if (!m_bRegistryClosed)
		return;

	if (m_hSoftKey != NULL)
		::RegCloseKey(m_hSoftKey);
	if (m_hCompanyKey != NULL)
		::RegCloseKey(m_hCompanyKey);

	if (m_hAppKey != NULL)
		::RegCloseKey(m_hAppKey);

	m_bRegistryClosed = TRUE;

}//CloseRegistry

eRegDataType CRegistry::GetRegistryDataType(LPCTSTR strSectionName, LPCTSTR strKeyName)
{
	eRegDataType regDataType = REG_DT_NONE;

	HKEY hSectionKey = GetSectionKey(strSectionName);
	
	DWORD dwType, dwCount;
	
	//Get the data
	long result = RegQueryValueEx( hSectionKey, strKeyName, NULL, &dwType, NULL, &dwCount);
	if (result == ERROR_SUCCESS) 
	{
		if (dwType == REG_SZ) 
			regDataType = REG_DT_STRING;
		
		if (dwType == REG_DWORD)
			regDataType = REG_DT_INT;
	}

	return regDataType;

}//GetRegistryDataType

HKEY CRegistry::GetSectionKey(LPCTSTR lpszSection)
{
	ASSERT(lpszSection != NULL);

	HKEY hSectionKey = NULL;
	if (m_hAppKey == NULL)
		return NULL;

	DWORD dw;

	RegCreateKeyEx(m_hAppKey, lpszSection, 0, REG_NONE, REG_OPTION_NON_VOLATILE, KEY_WRITE|KEY_READ, NULL, &hSectionKey, &dw);

	return hSectionKey;

}//GetSectionKey

DWORD CRegistry::ReadIntegerValue(LPCTSTR strSectionName, LPCTSTR strKeyName, DWORD dwValue)
{
	ASSERT(m_hAppKey);

	HKEY hSectionKey = GetSectionKey(strSectionName);
	if (hSectionKey == NULL)
		return -1;

	DWORD dwType, dwDest; 
	DWORD dwSize = sizeof (DWORD);

	LONG lRet = RegQueryValueEx (hSectionKey, (LPWSTR) strKeyName, NULL,  &dwType, (BYTE *) &dwDest, &dwSize);
	if (lRet == ERROR_SUCCESS)
		dwValue = dwDest;

	return dwValue;

}//ReadIntegerValue

CString CRegistry::ReadStringValue(LPCTSTR szSectionName, LPCTSTR szKeyName, LPCTSTR &sVal)
{
	ASSERT(m_hAppKey);
	
	HKEY hSectionKey = GetSectionKey(szSectionName);
	if (hSectionKey == NULL)
		return sVal;


	DWORD dwType;
	DWORD dwSize = 200;
	TCHAR  string[200];

	LONG lReturn = RegQueryValueEx (hSectionKey, (LPWSTR) szKeyName, NULL, &dwType, (BYTE *) string, &dwSize);

	if (lReturn == ERROR_SUCCESS)
		sVal = string;
	
	return sVal;

}//ReadStringValue

LONG CRegistry::DeleteKey(LPCTSTR lpszSection, LPCTSTR lpszKeyName)
{
	LONG lRetVal;

	HKEY hSectionKey = GetSectionKey(lpszSection);
	if (hSectionKey == NULL)
		return -1;

	lRetVal = RegDeleteKey(hSectionKey, lpszKeyName);

	return lRetVal;

}//DeleteKey
