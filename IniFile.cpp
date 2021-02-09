// IniFile.cpp: implementation of the CIniFile class.
// Written by: Adam Clauss
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "IniFile.h"
#include <fstream>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////
// Construction/Destruction
/////////////////////////////////////////////////////////////////////

//default constructor
CIniFile::CIniFile()
{
	m_bModified = FALSE;
	m_bFileReadSuccessful = FALSE;
}

//constructor, can specify pathname here instead of using SetPath later
CIniFile::CIniFile(CString strIniPath)
{
	path = strIniPath;
	m_bModified = FALSE;
	m_bFileReadSuccessful = FALSE;
	ReadFile(false);
}

//default destructor
CIniFile::~CIniFile()
{
}

/////////////////////////////////////////////////////////////////////
// Public Functions
/////////////////////////////////////////////////////////////////////

//sets path of ini file to read and write from
void CIniFile::SetPath(CString strNewPath)
{
	path = strNewPath;
	ReadFile(false);
}

//reads ini file specified using CIniFile::SetPath()
//returns true if successful, false otherwise
BOOL CIniFile::ReadFile(bool bUseOfStream)
{
	CStdioFile file;
	CFileStatus status;
	CString strReadInfo;
	CString strKeyName,
			strValueName,
			strValue;

	m_bFileReadSuccessful = FALSE;
	if (!file.GetStatus(path,status))
	{
		error = "Unable to find file.";
		return 0;
	}
	if (bUseOfStream)
	{
		std::ifstream inifile;
		inifile.open(path);
		if (inifile.fail()) {
			error = "Unable to open ini file.";
			return 0;
		}
		while (getline(inifile, strReadInfo))
		{
			if (strReadInfo != "")
			{
				//if a section heading
				if (strReadInfo[0] == '[' && strReadInfo[strReadInfo.GetLength() - 1] == ']')
				{
					strKeyName = strReadInfo;
					strKeyName.TrimLeft('[');
					strKeyName.TrimRight(']');
				}
				else
				{
					//if a value
					strValueName = strReadInfo.Left(strReadInfo.Find('='));
					strValue = strReadInfo.Right(strReadInfo.GetLength() - strValueName.GetLength() - 1);
					SetCString(strKeyName, strValueName, strValue);
				}
			}
		}
		m_bModified = FALSE;
		m_bFileReadSuccessful = TRUE;
		inifile.close();
	}
	else
	{
		if (file.Open(path, CFile::modeRead | CStdioFile::typeUnicode))
		{
			while (file.ReadString(strReadInfo))
			{
				if (!strReadInfo.IsEmpty())
				{
					strReadInfo.Remove('\r');
					//if a section heading
					if ((strReadInfo.Find(_T('[')) != -1) && (strReadInfo.Find(_T(']')) != -1))
					{
						strKeyName = strReadInfo;
						strKeyName.Remove(_T('['));
						strKeyName.Remove(_T(']'));
					}
					else
					{
						//if a value
						strValueName = strReadInfo.Left(strReadInfo.Find('='));
						strValue = strReadInfo.Right(strReadInfo.GetLength() - strValueName.GetLength() - 1);
						SetCString(strKeyName, strValueName, strValue);
					}
				}
			}
			m_bModified = FALSE;
			m_bFileReadSuccessful = TRUE;
			file.Close();
		}

	}
	for (int nKeyNum = 0; nKeyNum <= names.GetUpperBound(); nKeyNum++)
	{
		if (keys[nKeyNum].names.GetSize() != 0)
		{
			TRACE(names[nKeyNum]);
			TRACE(_T("\n"));
			for (int nValueNum = 0; nValueNum <= keys[nKeyNum].names.GetUpperBound(); nValueNum++)
			{
				CString str1; str1.Format(_T("%s"), keys[nKeyNum].names[nValueNum]);
				CString str2; str2.Format(_T("%s"), keys[nKeyNum].values[nValueNum]);
				TRACE(_T("%s=%s"), keys[nKeyNum].names[nValueNum], keys[nKeyNum].values[nValueNum]);
				if (nValueNum != keys[nKeyNum].names.GetUpperBound())
					TRACE(_T("\n"));
				else
					if (nKeyNum < names.GetSize())
						TRACE(_T("\n"));
			}
			if (nKeyNum < names.GetSize())
				TRACE(_T("\n"));
		}
	}
	return 1;
}

//writes data stored in class to ini file
void CIniFile::WriteFile(bool bUseOfStream)
{
	if (bUseOfStream)
	{
		std::ofstream inifile;
		inifile.open(path);
		for (int nKeyNum = 0; nKeyNum <= names.GetUpperBound(); nKeyNum++) 
		{
			if (keys[nKeyNum].names.GetSize() != 0) 
			{
				inifile << '[' << names[nKeyNum] << ']' << std::endl;
				for (int nValueNum = 0; nValueNum <= keys[nKeyNum].names.GetUpperBound(); nValueNum++)
				{
					CString str1 = keys[nKeyNum].names[nValueNum];
					CString str2 = keys[nKeyNum].values[nValueNum];
					inifile << keys[nKeyNum].names[nValueNum] << "=" << keys[nKeyNum].values[nValueNum];
					if (nValueNum != keys[nKeyNum].names.GetUpperBound())
						inifile << std::endl;
					else
						if (nKeyNum < names.GetSize())
							inifile << std::endl;
				}
				if (nKeyNum < names.GetSize())
					inifile << std::endl;
			}
		}
		m_bModified = FALSE;
		inifile.close();
	}
	else // To be called for Unicode
	{
		CStdioFile fileWriteUnicode;
		CString strWrite, strLineFeed = _T("\r\n");

		if (fileWriteUnicode.Open(path, CFile::modeCreate | CFile::modeWrite | CStdioFile::typeUnicode))
		{
			for (int nKeyNum = 0; nKeyNum <= names.GetUpperBound(); nKeyNum++) 
			{
				if (keys[nKeyNum].names.GetSize() != 0) 
				{
					if (!names[nKeyNum].IsEmpty())
					{
						strWrite = _T("[") + names[nKeyNum] + _T("]");
						fileWriteUnicode.WriteString(strWrite);
						fileWriteUnicode.WriteString(strLineFeed);
						for (int nValueNum = 0; nValueNum <= keys[nKeyNum].names.GetUpperBound(); nValueNum++)
						{
							CString str1 = keys[nKeyNum].names[nValueNum];
							if (!keys[nKeyNum].names[nValueNum].IsEmpty())
							{
								CString str2 = keys[nKeyNum].values[nValueNum];
								strWrite.Format(_T("%s=%s"), keys[nKeyNum].names[nValueNum], keys[nKeyNum].values[nValueNum]);
								fileWriteUnicode.WriteString(strWrite);
								if (nValueNum != keys[nKeyNum].names.GetUpperBound())
									fileWriteUnicode.WriteString(strLineFeed);
								else
									if (nKeyNum < names.GetSize())
										fileWriteUnicode.WriteString(strLineFeed);
							}
						}
						if (nKeyNum < names.GetSize())
							fileWriteUnicode.WriteString(strLineFeed);
					}
				}
			}
		}
	}
}

//deletes all stored ini data
void CIniFile::Reset()
{
	keys.SetSize(0);
	names.SetSize(0);
}

//returns number of keys currently in the ini
int CIniFile::GetNumKeys()
{
	return keys.GetSize();
}

//returns number of values stored for specified key, or -1 if key found
int CIniFile::GetNumValues(CString strKeyName)
{
	int nKeyNum = FindKey(strKeyName);
	if (nKeyNum == -1)
		return -1;
	else
		return keys[nKeyNum].names.GetSize();
}

BOOL CIniFile::SetCString(CString strKeyName, CString strValueName, CString strValue, BOOL bCreate)
{
	int nKeyNum = FindKey(strKeyName);
	int nValueNum = 0;

	//find key
	if (nKeyNum == -1) {
		//if key doesn't exist
		if (!bCreate) //and user does not want to create it,
			return 0; //stop entering this key
		names.SetSize(names.GetSize()+1);
		keys.SetSize(keys.GetSize()+1);
		nKeyNum = names.GetSize()-1;
		names[nKeyNum] = strKeyName;
		CString str1 = names[nKeyNum];
		CString str2 = str1;
	}

	//find value
	nValueNum = FindValue(nKeyNum,strValueName);
	if (nValueNum == -1) {
		if (!bCreate)
			return 0;
		keys[nKeyNum].names.SetSize(keys[nKeyNum].names.GetSize()+1);
		keys[nKeyNum].values.SetSize(keys[nKeyNum].names.GetSize()+1);
		nValueNum = keys[nKeyNum].names.GetSize()-1;
		keys[nKeyNum].names[nValueNum] = strValueName;
	}
	keys[nKeyNum].values[nValueNum] = strValue;
	CString str1 = keys[nKeyNum].values[nValueNum];
	m_bModified = TRUE;
	return 1;
}

BOOL CIniFile::SetInteger(CString strKeyName, CString strValueName, int nValue, BOOL bCreate)
{
	CString strTemp;
	strTemp.Format(_T("%d"),nValue);
	return SetCString(strKeyName, strValueName, strTemp, bCreate);
}

BOOL CIniFile::SetLong(CString strKeyName, CString strValueName, long lValue, BOOL bCreate)
{
	CString strTemp;
	strTemp.Format(_T("%ld"),lValue);
	return SetCString(strKeyName, strValueName, strTemp, bCreate);
}

BOOL CIniFile::SetDouble(CString strKeyName, CString strValueName, double dValue, BOOL bCreate)
{
	CString strTemp;
	strTemp.Format(_T("%e"),dValue);
	return SetCString(strKeyName, strValueName, strTemp, bCreate);
}

BOOL CIniFile::SetFloat(CString strKeyName, CString strValueName, float fValue, BOOL bCreate)
{
	CString strTemp;
	strTemp.Format(_T("%f"),fValue);
	return SetCString(strKeyName, strValueName, strTemp, bCreate);
}

BOOL CIniFile::SetBoolean(CString strKeyName, CString strValueName, BOOL bValue, BOOL bCreate)
{
	CString strTemp;
	if (bValue)
		strTemp.Format(_T("%d"),1);
	else
		strTemp.Format(_T("%d"),0);
	return SetCString(strKeyName, strValueName, strTemp, bCreate);
}

BOOL CIniFile::SetCPoint(CString strKeyName, CString strValueName, CPoint pt, BOOL bCreate)
{
	CString strPtX = strValueName + _T("X");
	CString	strPtY = strValueName + _T("Y");

	CString strTemp;
	strTemp.Format(_T("%d"),pt.x);
	if (!SetCString(strKeyName, strPtX, strTemp, bCreate))
		return FALSE;

	strTemp.Format(_T("%d"),pt.y);
	if (!SetCString(strKeyName, strPtY, strTemp, bCreate))
		return FALSE;

	return TRUE;
}

BOOL CIniFile::SetCRect(CString strKeyName, CString strValueName, CRect rect, BOOL bCreate)
{
	CString strRectTop = strValueName + _T("Top");
	CString strRectBot = strValueName + _T("Bot");
	CString strRectLft = strValueName + _T("Lft");
	CString strRectRht = strValueName + _T("Rht");

	CString strTemp;
	strTemp.Format(_T("%d"),rect.top);
	if (!SetCString(strKeyName, strRectTop, strTemp, bCreate))
		return FALSE;

	strTemp.Format(_T("%d"),rect.bottom);
	if (!SetCString(strKeyName, strRectBot, strTemp, bCreate))
		return FALSE;

	strTemp.Format(_T("%d"),rect.left);
	if (!SetCString(strKeyName, strRectLft, strTemp, bCreate))
		return FALSE;

	strTemp.Format(_T("%d"),rect.right);
	if (!SetCString(strKeyName, strRectRht, strTemp, bCreate))
		return FALSE;

	return TRUE;
}

//deletes specified value
//returns true if value existed and deleted, false otherwise
BOOL CIniFile::DeleteValue(CString strKeyName, CString strValueName)
{
	int nKeyNum = FindKey(strKeyName);
	int nValueNum = FindValue(nKeyNum,strValueName);
	if (nKeyNum == -1 || nValueNum == -1)
		return 0;

	keys[nKeyNum].names.RemoveAt(nValueNum);
	keys[nKeyNum].values.RemoveAt(nValueNum);
	return 1;
}

//deletes specified key and all values contained within
//returns true if key existed and deleted, false otherwise
BOOL CIniFile::DeleteKey(CString strKeyName)
{
	int nKeyNum = FindKey(strKeyName);
	if (nKeyNum == -1)
		return 0;
	keys.RemoveAt(nKeyNum);
	names.RemoveAt(nKeyNum);
	return 1;
}

/////////////////////////////////////////////////////////////////////
// Private Functions
/////////////////////////////////////////////////////////////////////

//returns index of specified key, or -1 if not found
int CIniFile::FindKey(CString strKeyName)
{
	int nKeyNum = 0;
	while ( nKeyNum < keys.GetSize() && names[nKeyNum] != strKeyName)
		nKeyNum++;
	if (nKeyNum == keys.GetSize())
		return -1;
	return nKeyNum;
}

//returns index of specified value, in the specified key, or -1 if not found
int CIniFile::FindValue(int nKeyNum, CString strValueName)
{
	if (nKeyNum == -1)
		return -1;
	int nValueNum = 0;
	while (nValueNum < keys[nKeyNum].names.GetSize() && keys[nKeyNum].names[nValueNum] != strValueName)
		nValueNum++;
	if (nValueNum == keys[nKeyNum].names.GetSize())
		return -1;
	return nValueNum;
}

//overloaded from original getline to take CString
std::istream & CIniFile:: getline(std::istream & is, CString & str)
{
    char buf[2048];
    is.getline(buf,2048);
    str = buf;
    return is;
}

CString CIniFile::GetString(CString strKeyName, CString strValueName, CString defaultvalue)
{
	int nKeyNum = FindKey(strKeyName), nValueNum = FindValue(nKeyNum,strValueName);

	if (nKeyNum == -1)
	{
		error = "Unable to locate specified key.";
		SetCString(strKeyName, strValueName, defaultvalue);
		return defaultvalue;
	}

	if (nValueNum == -1)
	{
		error = "Unable to locate specified value.";
		SetCString(strKeyName, strValueName, defaultvalue);
		return defaultvalue;
	}
	return keys[nKeyNum].values[nValueNum];
}

int CIniFile::GetInteger(CString strKeyName, CString strValueName, int defaultvalue)
{
	int nKeyNum = FindKey(strKeyName), nValueNum = FindValue(nKeyNum,strValueName);

	if (nKeyNum == -1)
	{
		error = "Unable to locate specified key.";
		SetInteger(strKeyName, strValueName, defaultvalue);
		return defaultvalue;
	}

	if (nValueNum == -1)
	{
		error = "Unable to locate specified value.";
		SetInteger(strKeyName, strValueName, defaultvalue);
		return defaultvalue;
	}
	return _wtoi(keys[nKeyNum].values[nValueNum]);
}

int CIniFile::GetLong(CString strKeyName, CString strValueName, long defaultvalue)
{
	int nKeyNum = FindKey(strKeyName), nValueNum = FindValue(nKeyNum,strValueName);

	if (nKeyNum == -1)
	{
		error = "Unable to locate specified key.";
		SetLong(strKeyName, strValueName, defaultvalue);
		return defaultvalue;
	}

	if (nValueNum == -1)
	{
		error = "Unable to locate specified value.";
		SetLong(strKeyName, strValueName, defaultvalue);
		return defaultvalue;
	}
	return _wtol(keys[nKeyNum].values[nValueNum]);
}

double CIniFile::GetDouble(CString strKeyName, CString strValueName, double defaultvalue)
{
	int nKeyNum = FindKey(strKeyName), nValueNum = FindValue(nKeyNum,strValueName);

	if (nKeyNum == -1)
	{
		error = "Unable to locate specified key.";
		SetDouble(strKeyName, strValueName, defaultvalue);
		return defaultvalue;
	}

	if (nValueNum == -1)
	{
		error = "Unable to locate specified value.";
		SetDouble(strKeyName, strValueName, defaultvalue);
		return defaultvalue;
	}
	return _wtof(keys[nKeyNum].values[nValueNum]);
}

float CIniFile::GetFloat(CString strKeyName, CString strValueName, float defaultvalue)
{
	int nKeyNum = FindKey(strKeyName), nValueNum = FindValue(nKeyNum,strValueName);

	if (nKeyNum == -1)
	{
		error = "Unable to locate specified key.";
		SetFloat(strKeyName, strValueName, defaultvalue);
		return defaultvalue;
	}

	if (nValueNum == -1)
	{
		error = "Unable to locate specified value.";
		SetFloat(strKeyName, strValueName, defaultvalue);
		return defaultvalue;
	}
	return (float)(_wtof(keys[nKeyNum].values[nValueNum]));
}

BOOL CIniFile::GetBoolean(CString strKeyName, CString strValueName, BOOL boolValue)
{
	int nVal;

	if(boolValue)
		nVal = 1;
	else
		nVal = 0;

	int nKeyNum = FindKey(strKeyName);
	int nBoolNum = FindValue(nKeyNum,strValueName);

	if (nKeyNum == -1)
	{
		error = "Unable to locate specified key.";
		SetInteger(strKeyName, strValueName, nVal);
		return boolValue;
	}

	if (nBoolNum == -1)
	{
		error = "Unable to locate specified value.";
		SetInteger(strKeyName, strValueName, nVal);
		return boolValue;
	}

	nVal = _wtoi(keys[nKeyNum].values[nBoolNum]);
	if (nVal)
		return TRUE;
	else 
		return FALSE;
}

CPoint CIniFile::GetCPoint(CString strKeyName, CString strValueName, CPoint pt)
{
	CString strPtX = strValueName + _T("X");
	CString	strPtY = strValueName + _T("Y");
	int nKeyNum = FindKey(strKeyName);
	int nPtXNum = FindValue(nKeyNum,strPtX);
	int nPtYNum = FindValue(nKeyNum,strPtY);

	if (nKeyNum == -1) {
		error = "Unable to locate specified key.";
		SetInteger(strKeyName, strPtX, pt.x);
		SetInteger(strKeyName, strPtY, pt.y);
		return pt;
	}
	if (nPtXNum == -1) {
		error = "Unable to locate specified value.";
		SetInteger(strKeyName, strPtX, pt.x);
		SetInteger(strKeyName, strPtY, pt.y);
		return pt;
	}
	if (nPtYNum == -1) {
		error = "Unable to locate specified value.";
		SetInteger(strKeyName, strPtX, pt.x);
		SetInteger(strKeyName, strPtY, pt.y);
		return pt;
	}

	pt.x = _wtoi(keys[nKeyNum].values[nPtXNum]);
	pt.y = _wtoi(keys[nKeyNum].values[nPtYNum]);
	
	return pt;
}

CRect CIniFile::GetCRect(CString strKeyName, CString strValueName, CRect rect)
{
	CString strRectTop = strValueName + _T("Top");
	CString strRectBot = strValueName + _T("Bot");
	CString strRectLft = strValueName + _T("Lft");
	CString strRectRht = strValueName + _T("Rht");
	int nKeyNum = FindKey(strKeyName);
	int nRectTopNum = FindValue(nKeyNum,strRectTop);
	int nRectBotNum = FindValue(nKeyNum,strRectBot);
	int nRectLftNum = FindValue(nKeyNum,strRectLft);
	int nRectRhtNum = FindValue(nKeyNum,strRectRht);

	if (nKeyNum == -1) {
		error = "Unable to locate specified key.";
		SetInteger(strKeyName, strRectTop, rect.top);
		SetInteger(strKeyName, strRectBot, rect.bottom);
		SetInteger(strKeyName, strRectLft, rect.left);
		SetInteger(strKeyName, strRectRht, rect.right);
		return rect;
	}
	if (nRectTopNum == -1) {
		error = "Unable to locate specified value.";
		SetInteger(strKeyName, strRectTop, rect.top);
		SetInteger(strKeyName, strRectBot, rect.bottom);
		SetInteger(strKeyName, strRectLft, rect.left);
		SetInteger(strKeyName, strRectRht, rect.right);
		return rect;
	}
	if (nRectBotNum == -1) {
		error = "Unable to locate specified value.";
		SetInteger(strKeyName, strRectTop, rect.top);
		SetInteger(strKeyName, strRectBot, rect.bottom);
		SetInteger(strKeyName, strRectLft, rect.left);
		SetInteger(strKeyName, strRectRht, rect.right);
		return rect;
	}
	if (nRectLftNum == -1) {
		error = "Unable to locate specified value.";
		SetInteger(strKeyName, strRectTop, rect.top);
		SetInteger(strKeyName, strRectBot, rect.bottom);
		SetInteger(strKeyName, strRectLft, rect.left);
		SetInteger(strKeyName, strRectRht, rect.right);
		return rect;
	}
	if (nRectRhtNum == -1) {
		error = "Unable to locate specified value.";
		SetInteger(strKeyName, strRectTop, rect.top);
		SetInteger(strKeyName, strRectBot, rect.bottom);
		SetInteger(strKeyName, strRectLft, rect.left);
		SetInteger(strKeyName, strRectRht, rect.right);
		return rect;
	}

	rect.top = _wtoi(keys[nKeyNum].values[nRectTopNum]);
	rect.bottom = _wtoi(keys[nKeyNum].values[nRectBotNum]);
	rect.left = _wtoi(keys[nKeyNum].values[nRectLftNum]);
	rect.right = _wtoi(keys[nKeyNum].values[nRectRhtNum]);
	
	return rect;
}

BOOL CIniFile::IsModified()
{
	return m_bModified;
}


BOOL CIniFile::IsInputOK()
{
	return m_bFileReadSuccessful;
}


unsigned long CIniFile::GetUnsignedLong(CString strKeyName, CString strValueName, unsigned long defaultvalue)
{
	int nKeyNum = FindKey(strKeyName), nValueNum = FindValue(nKeyNum,strValueName);

	if (nKeyNum == -1)
	{
		error = "Unable to locate specified key.";
		SetUnsignedLong(strKeyName, strValueName, defaultvalue);
		return defaultvalue;
	}

	if (nValueNum == -1)
	{
		error = "Unable to locate specified value.";
		SetUnsignedLong(strKeyName, strValueName, defaultvalue);
		return defaultvalue;
	}

	TCHAR* ptr;

	return wcstoul(keys[nKeyNum].values[nValueNum], &ptr, 16);
}

BOOL CIniFile::SetUnsignedLong(CString strKeyName, CString strValueName, unsigned long lValue, BOOL bCreate)
{
	CString strTemp;
	strTemp.Format(_T("0X%04X"),lValue);
	return SetCString(strKeyName, strValueName, strTemp, bCreate);
}
