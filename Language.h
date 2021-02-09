/*
	Developed by Geert van Horrik (http://geert.yoki.org)
*/

// Redefinition protection
#ifndef LANGUAGE_H
#define LANGUAGE_H

//*********************************************************************
// INCLUDES
//*********************************************************************

#include <windows.h>
#include <memory>					// Needed for singleton class
#include <string>

//using namespace std;

//*********************************************************************
// STRUCTS
//*********************************************************************

struct LanguageInfo
{
	std::wstring	sDll;				// Dll name
	LANGID	arrLanguages[10];	// Languages that should use this dll
	int		iLanguageCount;		// Number of languages
};

//*********************************************************************
// CLanguage
//*********************************************************************

class CLanguage  
{
public:
	// Functions
	static  CLanguage * Instance();
	void	Init(std::wstring sDLLPath = _T("lng"), std::wstring sDefaultLanguage = _T("english"));
	bool	LoadLanguage(LANGID language);
	bool	LoadLanguage(std::wstring dllLocation, std::wstring sLanguageName);
	void	ReleaseLanguage();
	std::wstring  GetString(UINT ResourceNumber);
	std::wstring  GetLanguageName();

	// Variables

private:
	// Constructor & destructor
	CLanguage();			// Private constructor because singleton!

	// Functions
	bool	FileExists(std::wstring sFile);
	std::wstring  ReadFromDll(UINT ResourceNumber);
	void	InitLanguagesAvailable();

	// Variables
	static std::auto_ptr<CLanguage> sm_inst;
	std::wstring 	m_sLanguageName;
	std::wstring	m_sDefaultLanguage;
	std::wstring	m_sDLLPath;
	HINSTANCE		m_hDLLfile;
	LanguageInfo	m_arrLanguages[100];
	int				m_iLanguageCount;
};

// End of redefinition protection
#endif // LANGUAGE_H