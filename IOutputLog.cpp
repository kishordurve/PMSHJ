/************************************************************************
Implementation of IOutputLogString

Use IOutputLogString to "send" a string to the DebugLog process.  
DebugLog uses a memory-mapped file as the data buffer.  IOutputLogString
writes a string to the file and sets a named event that the DebugLog
process waits on.  When the DebugLog process wakes up, it reads the
new string(s) added to the memory-mapped file and displays it in
an edit control.


************************************************************************/

#include "stdafx.h"
#include "IOutputLog.h"
#include <process.h>
#include <strsafe.h>


void IOutputLogString(CString lpOutputString)
{
    /* before we do anything else, make sure DebugLog process is open and waiting */
    HANDLE heventDebugLog = OpenEvent(EVENT_ALL_ACCESS, FALSE, _T("IOUTPUTLOG_777_BUFFER_777"));
    if ( !heventDebugLog )
    {
        //MessageBox(NULL, "IOUTPUTLOG_777_BUFFER_777 nonexistent", NULL, MB_OK);
        return;            
    }

	/* add time stamp to front of lpOutputString */
 /*   SYSTEMTIME st;
    GetLocalTime(&st);
    sprintf(achBuffer, "%02u:%02u:%02u.%03u  %s\n", 
                            st.wHour, 
                            st.wMinute, 
                            st.wSecond, 
                            st.wMilliseconds,
                            lpOutputString);

*/

	int const arraysize = 1024;
	//TCHAR pszDest[arraysize];
	size_t cbDest = arraysize * sizeof(TCHAR);
	LPCTSTR pszFormat = TEXT(" %s\n");

	HRESULT hr;
	unsigned int nSize = wcslen(lpOutputString);

	TCHAR achBuffer[MAX_MEMORY_BLOCK_SIZE-1];
	hr = StringCbPrintf(achBuffer, cbDest, pszFormat, lpOutputString);
    //sprintf_s(achBuffer, "%s\n", lpOutputString);

    /* get a handle to the data synch object */
    HANDLE heventData = OpenEvent(EVENT_MODIFY_STATE, FALSE, _T("DEBUGLOG_DATA_READY"));
    if ( !heventData )
    {
        // MessageBox(NULL, "DEBUGLOG_DATA_READY nonexistent", NULL, MB_OK);
        CloseHandle(heventDebugLog);
        return;            
    }
    
    HANDLE hSharedFile = CreateFileMapping((HANDLE)-1, NULL, PAGE_READWRITE, 0, MAX_MEMORY_BLOCK_SIZE, _T("DEBUGLOG_BUFFER"));
    if (!hSharedFile) 
    {
        //MessageBox(NULL, "IOutputLogString: Unable to create file mapping object DEBUGLOG_BUFFER", "Error", MB_OK);
        CloseHandle(heventDebugLog);
        CloseHandle(heventData);
        return;
    }

    LPWSTR lpszSharedMem = (LPWSTR)MapViewOfFile(hSharedFile, FILE_MAP_WRITE, 0, 0, 0);
    if (!lpszSharedMem) 
    {
        //MessageBox(NULL, "IOutputLogString: Unable to map shared memory", "Error", MB_OK);
        CloseHandle(hSharedFile);
        CloseHandle(heventDebugLog);
        CloseHandle(heventData);
        return;
    }

    /* wait for buffer event */
    WaitForSingleObject(heventDebugLog, INFINITE);

    /* write it to the shared memory */
    *((LPDWORD)lpszSharedMem) = _getpid();
	// This has done the trick. Must write 'chars' to LogOutput window as it cannot accept 'wchars' - no other issue
	char chString[256];
	wcstombs(chString, achBuffer, 255);
    wsprintfA((LPSTR)lpszSharedMem + sizeof(DWORD), "%s", chString);

    /* signal data ready event */
    SetEvent(heventData);

    /* clean up handles */
    UnmapViewOfFile(lpszSharedMem);
    CloseHandle(hSharedFile);
    CloseHandle(heventData);
    CloseHandle(heventDebugLog);

    return;
}
/************************************************************************
Implementation of IOutputLogRect

The IOutputLogRect converts the CRect values to a string before
calling IOutputLogString to "send" a string to the DebugLog process.  

************************************************************************/
void IOutputLogRect(LPCTSTR lpOutputString, CRect &rect)
{
	int const arraysize = 1024;
	TCHAR pszDest[arraysize];
	size_t cbDest = arraysize * sizeof(TCHAR);
	LPCTSTR pszFormat = TEXT(" %s  SX=%d SY=%d EX=%d EY=%d Width=%d Height=%d");

	HRESULT hr = StringCbPrintf(pszDest, cbDest, pszFormat, lpOutputString, rect.TopLeft().x,
		rect.TopLeft().y,
		rect.BottomRight().x,
		rect.BottomRight().y,
		rect.Width(),
		rect.Height());

	IOutputLogString(pszDest);
}

/************************************************************************
Implementation of IOutputLogPoint

The IOutputLogPoint converts the CPoint values to a string before
calling IOutputLogString to "send" a string to the DebugLog process.  

************************************************************************/
void IOutputLogPoint(LPCTSTR lpOutputString, CPoint &point)
{
	int const arraysize = 1024;
	TCHAR pszDest[arraysize];
	size_t cbDest = arraysize * sizeof(TCHAR);
	LPCTSTR pszFormat = TEXT(" %s  X=%d Y=%d");

	HRESULT hr = StringCbPrintf(pszDest, cbDest, pszFormat, lpOutputString, point.x, point.y);

	IOutputLogString(pszDest);

	/*char buf[MAX_MEMORY_BLOCK_SIZE -1];
	wsprintf(buf," %s  X=%d Y=%d",
					lpOutputString,
					point.x,
					point.y);

	IOutputLogString(buf);*/
}

/************************************************************************
Implementation of IOutputLog

The IOutputLog converts the values using format string to a string before
calling IOutputLogString to "send" a string to the DebugLog process.  

************************************************************************/
void IOutputLog(LPTSTR lpFormat, ...) 
{
	int const arraysize = 1024;
	TCHAR pszDest[arraysize];
	size_t cbDest = arraysize * sizeof(TCHAR);
	LPCTSTR pszFormat = TEXT(" %s");

	HRESULT hr;
	unsigned int nSize = wcslen(lpFormat);

	if (nSize > MAX_MEMORY_BLOCK_SIZE - 1)
	{
		LPTSTR strError = _T(" The Data lenth %u is exceeding LOG Memory Size");
		hr = StringCbPrintf(pszDest, cbDest, pszFormat, strError);
	}
	else
	{
		hr = StringCbPrintf(pszDest, cbDest, pszFormat, lpFormat);
	}
	IOutputLogString(pszDest);

	/*char buf[MAX_MEMORY_BLOCK_SIZE-1];

	if ( nSize > MAX_MEMORY_BLOCK_SIZE-1 )
	{
		sprintf(buf, " The Data lenth %u is exceeding LOG Memory Size : %u", nSize, MAX_MEMORY_BLOCK_SIZE );
	}
	else
		// format the string
		vsprintf(buf, lpFormat, (LPSTR)(&lpFormat+1));

	IOutputLogString(buf);*/
}

