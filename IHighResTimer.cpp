// IHighResTimer.cpp: implementation of the CElapsed class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "IHighResTimer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
///////// ***** Description about CIHighResTimer Class **** /////////
//	 
//	The CIHighResTimer class provides 
//	1. A High resolution time measurement in the order of 0.1 msec 
//  2. A High resolution delay ( theoretically 0.1 micro second )
//	
//  Usage:
//		
//		Create a local object of this class where ever these
//		functions need to be used.
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//****************************************************************************
// Function		:	CIHighResTimer (Constructor)
//
// Description	:	This function calculates the frequency and starts the High 
//					Resolution Timer 
//
// Inputs		:   None
//
// Returns		:	None
//****************************************************************************

CIHighResTimer::CIHighResTimer()
{
	m_liEndTime		= 0; 
	m_liStartTime	= 0;

	// get the frequency of the counter
	m_bInitialized  = QueryPerformanceFrequency( (LARGE_INTEGER *)&m_liFrequency );

	// Start High Resolution Timer.
	StartHighResTimer();

} // CIHighResTimer

CIHighResTimer::~CIHighResTimer()
{
	
} // ~CIHighResTimer

//****************************************************************************
// Function		:	StartHighResTimer
//
// Description	:	This function starts the High Resolution Timer 
//
// Inputs		:   None
//
//             
// Returns		:	BOOL Value, TRUE indicates that the High Resolution Timer
//					has got initiated successfully. FALSE indicates that the
//					operation has failed.
//****************************************************************************

BOOL CIHighResTimer::StartHighResTimer()
{
	if( ! m_bInitialized )
		return FALSE; // error - couldn't get frequency

	// get the starting counter value
	return QueryPerformanceCounter( (LARGE_INTEGER *)&m_liStartTime );

} // StartHighResTimer


//****************************************************************************
// Function		:	EndHighResTimer
//
// Description	:	This function Ends the High Resolution Timer 
//
// Inputs		:   None
//
//             
// Returns		:	double value indicating the elapsed time in Seconds.
//****************************************************************************
double CIHighResTimer::EndHighResTimer()
{
	if( ! m_bInitialized )
		return 0.0; // error - couldn't get frequency
	
	QueryPerformanceCounter( (LARGE_INTEGER *)&m_liEndTime );

	// determine the elapsed counts
	__int64 elapsed = m_liEndTime - m_liStartTime;
	
	// convert counts to time in seconds and return it
	return (double)elapsed / (double)m_liFrequency;

} // EndHighResTimer

//****************************************************************************
// Function		:	GetHighResStartCount
//
// Description	:	This function Returns the start count of the High Resolution 
//					timer
//
// Inputs		:   None
//             
// Returns		:	int64 value indicating the start count.
//****************************************************************************
__int64	CIHighResTimer::GetHighResStartCount()
{
	// return the start count
	return m_liStartTime;

} // GetHighResStartCount

//****************************************************************************
// Function		:	GetHighResEndCount
//
// Description	:	This function Returns the end count of the High Resolution 
//					timer
//
// Inputs		:   None
//             
// Returns		:	int64 value indicating the end count.
//****************************************************************************

 __int64 CIHighResTimer::GetHighResEndCount()
{
	// return the end count
	return m_liEndTime;

 } // GetHighResEndCount

//****************************************************************************
// Function		:	GetPerformanceFrequency
//
// Description	:	This function returns the frequency of the High Resolution
//					timer. 
//
// Inputs		:   None
//             
// Returns		:	int64 value indicating the elapsed count.
//****************************************************************************
__int64 CIHighResTimer::GetPerformanceFrequency()
{

	// return the frequency 
	return m_liFrequency;

} // GetPerformanceFrequency


//****************************************************************************
// Function		:	GetHighResElapsedCount
//
// Description	:	This function Returns the elapsed count from the begining of 
//					timer.
//
// Inputs		:   None
//             
// Returns		:	int64 value indicating the elapsed count.
//****************************************************************************
__int64 CIHighResTimer::GetHighResElapsedCount()
{
	if( ! m_bInitialized )
		return 0; // error - couldn't get frequency

	__int64 endtime;
	
	QueryPerformanceCounter( (LARGE_INTEGER *)&endtime );

	// determine the elapsed count.
	__int64 elapsed = endtime - m_liStartTime;

	// return the elapsed count.
	return elapsed;

} // GetElapsedCount


// V1.1: Added
//****************************************************************************
// Function		:	GetHighResElapsedTimeMsec
//
// Description	:	This function Returns the elapsed time in milliseconds 
//					from the begining of the timer.
//
// Inputs		:   None
//             
// Returns		:	double value indicating the elapsed time in milliseconds.
//****************************************************************************
double CIHighResTimer::GetHighResElapsedTimeMsec()
{
	if( ! m_bInitialized )
		return 0; // error - couldn't get frequency

	__int64 end_count;
	
	QueryPerformanceCounter( (LARGE_INTEGER *)&end_count );

	// determine the elapsed count.
	__int64 elapsed_count = end_count - m_liStartTime;

	// convert to elapsed time in milliseconds
	double elapsed_time_msec = 1000 * elapsed_count / (double)m_liFrequency;

	// return the elapsed time.
	return elapsed_time_msec;

} // GetHighResElapsedTimeMsec


//***************************************************************************************
// Function		:	HighResSleep
//
// Description	:	This function halts the execution for the specified period in 
//					microsecond(s).
//
// Inputs		:   micro-second 
//             
// Returns		:	None.
//***************************************************************************************

void CIHighResTimer::HighResSleep( double msec )
{
	__int64 elapsed = 0;

	if( ! m_bInitialized )
		return ; // error - couldn't get frequency.

	// Calculate the performance count. 
	__int64 perfCount = (__int64)(msec * m_liFrequency / 1000);

	// Start the High Resolution Timer.
	StartHighResTimer();

	// Loop until the elapsed count is less then Performance Counter
	while ( elapsed < perfCount )
		elapsed = GetHighResElapsedCount();

} // HighResSleep

