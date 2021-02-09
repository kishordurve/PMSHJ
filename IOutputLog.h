#ifndef IIOutputLog_H
#define IIOutputLog_H
#include <atlstr.h>
#include <atltypes.h>

// V 1.1
const int MAX_MEMORY_BLOCK_SIZE = 50 * 1024;

// function declarations

void  IOutputLogString(CString lpOutputString);
/************************************************************************
   Declaration for IOutputLogString function used to send a string to
	the DebugLog process.
   
************************************************************************/

void  IOutputLogRect(LPCTSTR lpOutputString, CRect &rect);
/************************************************************************
   Declaration for IOutputLogRect function used to convert CRect
   values to a string before calling IOutputLogString function to send
   the string to the DebugLog process.
   
************************************************************************/

void  IOutputLogPoint(LPCTSTR lpOutputString, CPoint &point);
/************************************************************************
   Declaration for IOutputLogPoint function used to convert CPoint 
   values to a string before calling IOutputLogString function to send
   the string to the DebugLog process.
   
************************************************************************/

void  IOutputLog(LPTSTR lpFormat, ...);
/************************************************************************
   Declaration for IOutputLog function used to format the values 
   to a string before calling IOutputLogString function to send
   the string to the DebugLog process.
   
************************************************************************/

#endif  //IOUTDBLOG_H


