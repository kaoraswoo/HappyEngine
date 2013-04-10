#ifndef _C_LOG_WINDOW_H_
#define _C_LOG_WINDOW_H_

#include "ZFLog.h"

//----------------------------------------------------------------------------
// class CLogWindow
//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
class CLogWindow : public ZFLog
{
	public:
		static CLogWindow&	GetInstance();
	
	public:
		CLogWindow();
		virtual ~CLogWindow();
};

//----------------------------------------------------------------------------
// class CLogFile
//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
class CLogFile : public ZFLog
{
	public:
		static CLogFile&	GetInstance();
	
	public:
		CLogFile();
		virtual ~CLogFile();
};


#define CLOG() CLogWindow::GetInstance()



#endif //_C_LOG_WINDOW_H_