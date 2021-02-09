/*
	Developed by Geert van Horrik (http://geert.yoki.org)
*/

//*********************************************************************
// INCLUDES
//*********************************************************************

#include "StdAfx.h"
#include "Language.h"
#include <direct.h>				// For getting path to current folder
#include <shlwapi.h>			// Needed for PathFileExists function

//*********************************************************************
// INITIALISATION
//*********************************************************************

std::auto_ptr<CLanguage> CLanguage::sm_inst;

//*********************************************************************
// CONSTRUCTOR & DESTRUCTOR
//*********************************************************************

CLanguage::CLanguage()
{
	// Set language to NULL
	m_hDLLfile = NULL;

	// Set default language name
	m_sLanguageName = _T("");
	m_sDLLPath = _T("lng");
	m_sDefaultLanguage = _T("english");

	// Init available languages
	InitLanguagesAvailable();
}

//*********************************************************************
// PUBLIC FUNCTIONS
//*********************************************************************

CLanguage * CLanguage::Instance()
{
	if(sm_inst.get() == 0)
		sm_inst = std::auto_ptr<CLanguage>(new CLanguage);
	return sm_inst.get();
}

//=====================================================================

void CLanguage::Init(std::wstring sDLLPath /* = "lng" */, std::wstring sDefaultLanguage /* = "english" */)
{
	// Set options
	m_sDefaultLanguage = sDefaultLanguage;
	m_sDLLPath = sDLLPath;
}

//=====================================================================

bool CLanguage::LoadLanguage(LANGID language)
{
	// Declare variables
	std::wstring sTemp, sWorkingPath;
	TCHAR map[_MAX_PATH]; 

	// Get working path
	_wgetcwd(map, _MAX_PATH);
	sWorkingPath = map;

	// Automatic load right language
	for (int i = 0; i < m_iLanguageCount; i++)
	{
		// Check all sublanguages
		for (int j = 0; j < m_arrLanguages[i].iLanguageCount; j++)
		{
			// Check if we found the right language
			if (m_arrLanguages[i].arrLanguages[j] == language)
			{
				// We found it!
				sTemp = sWorkingPath + _T("\\") + m_sDLLPath + _T("\\FastScanNMeasure") + m_arrLanguages[i].sDll + _T(".dll");
				LoadLanguage(sTemp, m_arrLanguages[i].sDll);
				return true;
			}
		}
	}

	// If we get here, no local language is found
	return false;
}

//=====================================================================

bool CLanguage::LoadLanguage(std::wstring dllLocation, std::wstring sLanguageName)
{
	// Declare variables
	std::string sLanguageVersion, sError, sTemp;

	// If we already loaded a dll, release it first
	ReleaseLanguage();

	// Set language
	m_sLanguageName = sLanguageName;

	// Check if dll we want to load exists
    if (!FileExists(dllLocation))
	{
		m_sLanguageName = m_sDefaultLanguage;
		return false;
	}
	
	// Load dll
	CT2CA temp = dllLocation.c_str();
	//mbtowc(temp, dllLocation.c_str(), dllLocation.length());

	LPVOID lpMsgBuf;
	LPVOID lpDisplayBuf;
	m_hDLLfile = LoadLibrary(dllLocation.c_str());
	if (m_hDLLfile != NULL)
		return true;
	else
	{
		DWORD dw = GetLastError();

		FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			dw,
			MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_UK),
			(LPTSTR)&lpMsgBuf,
			0, NULL);
		CString strError((LPTSTR)&lpMsgBuf);
		// Display the error message and exit the process
		lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT,
			(lstrlen((LPCTSTR)lpMsgBuf) +  40) * sizeof(TCHAR));
		//StringCchPrintf((LPTSTR)lpDisplayBuf,
		//	LocalSize(lpDisplayBuf) / sizeof(TCHAR),
		//	TEXT("%s failed with error %d: %s"),
		//	lpszFunction, dw, lpMsgBuf);
		return false;
	}
}

//=====================================================================

void CLanguage::ReleaseLanguage()
{
	// Release the dll
	if (m_hDLLfile != NULL)
	{
		FreeLibrary(m_hDLLfile);
		m_hDLLfile = NULL;
	}
}

//=====================================================================

std::wstring CLanguage::GetString(UINT ResourceNumber)
{
	// Declare variables
	std::wstring sResult;
	TCHAR szBuffer[1024];

	// First try to read from DLL
	sResult = ReadFromDll(ResourceNumber);

	// If that doesn't works, read from default string table
	if (sResult.length() == 0)
	{
		// Should load from dll
		::LoadStringW(NULL, ResourceNumber, szBuffer, 1024);
		sResult = szBuffer;
	}

	return sResult;
}

//=====================================================================

std::wstring CLanguage::GetLanguageName()
{
	// Return value
	return m_sLanguageName;
}

//*********************************************************************
// PRIVATE FUNCTIONS
//*********************************************************************

bool CLanguage::FileExists(std::wstring sFile)
{
	// Check if file exists
	if ((DWORD)-1 == GetFileAttributes(sFile.c_str()))
		return false;

	return true;
}

//=====================================================================

std::wstring CLanguage::ReadFromDll(UINT ResourceNumber)
{
	// Declare variables
	std::wstring sResult = _T("");
	TCHAR szBuffer[1024];

	// Should we load from dll?
	if (m_hDLLfile != NULL)
	{
		// Should load from dll
		::LoadString(m_hDLLfile, ResourceNumber, szBuffer, 1024);
		sResult = szBuffer;
	}

	return sResult;
}

//=====================================================================

void CLanguage::InitLanguagesAvailable()
{
	// Set language count to zero
	m_iLanguageCount = 0;

	// Add English
	m_arrLanguages[m_iLanguageCount].iLanguageCount = 0;
	m_arrLanguages[m_iLanguageCount].arrLanguages[m_arrLanguages[m_iLanguageCount].iLanguageCount++] = 0x0809; // English
	m_arrLanguages[m_iLanguageCount++].sDll = _T("EN");

	// Add Chinese
	m_arrLanguages[m_iLanguageCount].iLanguageCount = 0;
	m_arrLanguages[m_iLanguageCount].arrLanguages[m_arrLanguages[m_iLanguageCount].iLanguageCount++] = 0x0804; // Chinese
	m_arrLanguages[m_iLanguageCount++].sDll = _T("CN");

}
