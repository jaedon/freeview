#pragma once
#ifndef TRL_Write2

#include <stdio.h>



#if defined(_WIN32)
//#define TRANSCODE_LOG
#endif
#if defined(TRANSCODE_LOG)
#define TRANCODE_LOG_INIT()		cTransLog::InitLog()
#define TRANCODE_LOG_CLOSE()	cTransLog::DeinitLog()
#define TRL_Write(msg)			cTransLog::write(msg)
#define TRL_Write2(...)			cTransLog::write3 (__FUNCTION__,__VA_ARGS__)
#else
#define TRANCODE_LOG_INIT()		
#define TRANCODE_LOG_CLOSE()	
#define TRL_Write(msg)			
#define TRL_Write2(...)			
#endif

#if defined(TRANSCODE_LOG)
class cTransLog
{
public:
	cTransLog(void);
	~cTransLog(void);
private:
	FILE * m_pf;
private:
	int OpenLogFile();
	int CloseLogfile();
	int WriteLog(const char * pMsg);
	int WriteLogVL(const char * pMsg, va_list ap);
	int WriteLog2(const char * pMsg, ...);
	int WriteLog3(const char * func, const char * pMsg, va_list ap);
	int WriteTimeToLog();
	
public:
	static cTransLog *s_Translog;
public:
	static int InitLog();
	static int DeinitLog();
	static int write(const char * pMsg);
	static int write2(const char * pMsg,...);
	static int write3(const char * func, const char * pMsg, ...);
};
#endif

#endif