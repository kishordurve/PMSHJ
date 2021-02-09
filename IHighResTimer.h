// IHighResTimer.h: interface for the CIHighResTimer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IHIGHRESTIMER_H__BE9FFCB7_79A9_11D5_8FE1_0050BA8D3A99__INCLUDED_)
#define AFX_IHIGHRESTIMER_H__BE9FFCB7_79A9_11D5_8FE1_0050BA8D3A99__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
  
class CIHighResTimer  
{
// Attributes

	BOOL	m_bInitialized;

	__int64 m_liFrequency,					 // Variable to hold the frequency of the High Resolution Timer.
		 	m_liStartTime,					 // Varibale to hold the Start Time of the High Resolution Timer.
			m_liEndTime;					 // Variable to hold the End Time of the High Resolu

// Operations
public :
	
	 BOOL		StartHighResTimer();		 // Start High Resolution Timer.

	 double		EndHighResTimer();			 // End the High Resolution Timer and get Elapsed time in seconds.
	
	 void		HighResSleep( double msec ); // High Resolution Sleep in milli seconds.	 
	
	 __int64	GetHighResStartCount();		 // Get the Start Count of the High Resolution Timer.	
	
	 __int64	GetHighResEndCount();		 // Get the End Count of the High Resolution Timer.

	 __int64    GetHighResElapsedCount();	 // Get the Elapsed Count of the High Resolution Timer.

	 double		GetHighResElapsedTimeMsec(); // V1.1: Get elapsed time in milliseconds

	 __int64	GetPerformanceFrequency();	 // Get the Performance Frequency of the High Resolution Timer.
	
	CIHighResTimer();
	virtual ~CIHighResTimer();

};

#endif // !defined(AFX_IHIGHRESTIMER_H__BE9FFCB7_79A9_11D5_8FE1_0050BA8D3A99__INCLUDED_)
