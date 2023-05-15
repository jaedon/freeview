// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#include "Log.h"
#include <string.h>

namespace Ooif
{

const char *OoifLog::s_debugEnvName   = "ENABLE_OOIF_DEBUG_LOGS";
const char *OoifLog::s_infoEnvName    = "ENABLE_OOIF_INFO_LOGS";
const char *OoifLog::s_warningEnvName = "ENABLE_OOIF_WARNING_LOGS";
const char *OoifLog::s_errorEnvName   = "ENABLE_OOIF_ERROR_LOGS";

const char *OoifLog::s_debugLevelName   = "debug";
const char *OoifLog::s_infoLevelName    = "info";
const char *OoifLog::s_warningLevelName = "warning";
const char *OoifLog::s_errorLevelName   = "error";


//#define LOG_FILE_ENABLE	1

#ifdef LOG_FILE_ENABLE
const std::string OoifLog::s_enableLogFiles[] = {"src/application.cpp",\
												"src/oipf.cpp"};
#endif

void OoifLog::logDebug(const char *fileName, unsigned int lineNo, const char *functionName, const char *fmt, ...)
{
	if(isDebugLogEnabled())
	{
		va_list argList; va_start(argList, fmt);
		log(fileName, lineNo, functionName, s_debugLevelName, fmt, argList);
		va_end(argList);
	}
	else if(isLogFileEnabled(fileName))
	{
		va_list argList; va_start(argList, fmt);
        log(fileName, lineNo, functionName, s_debugLevelName, fmt, argList);
        va_end(argList);
	}
}

void OoifLog::logInfo(const char *fileName, unsigned int lineNo, const char *functionName, const char *fmt, ...)
{
	if(isInfoLogEnabled())
	{
		va_list argList; va_start(argList, fmt);
		log(fileName, lineNo, functionName, s_infoLevelName, fmt, argList);
		va_end(argList);
	}	
	else if(isLogFileEnabled(fileName))
	{
		va_list argList; va_start(argList, fmt);
        log(fileName, lineNo, functionName, s_debugLevelName, fmt, argList);
        va_end(argList);
	}
}

void OoifLog::logWarning(const char *fileName, unsigned int lineNo, const char *functionName, const char *fmt, ...)
{
	if(isWarningLogEnabled())
	{
		va_list argList; va_start(argList, fmt);
		log(fileName, lineNo, functionName, s_warningLevelName, fmt, argList);
		va_end(argList);
	}
	else if(isLogFileEnabled(fileName))
	{
		va_list argList; va_start(argList, fmt);
        log(fileName, lineNo, functionName, s_debugLevelName, fmt, argList);
        va_end(argList);
	}
}

void OoifLog::logError(const char *fileName, unsigned int lineNo, const char *functionName, const char *fmt, ...)
{
	if(isErrorLogEnabled())
	{
		va_list argList; va_start(argList, fmt);
		log(fileName, lineNo, functionName, s_errorLevelName, fmt, argList);
		va_end(argList);
	}	
	else if(isLogFileEnabled(fileName))
	{
		va_list argList; va_start(argList, fmt);
        log(fileName, lineNo, functionName, s_debugLevelName, fmt, argList);
        va_end(argList);
	}
}

void OoifLog::log(const char *fileName, unsigned int lineNo, const char *functionName, const char *level, const char *fmt, va_list argList)
{
	fprintf (stderr, "HBBTV %s :", level);
	fprintf (stderr, "(%32s:%4d::%20s) : ", fileName, lineNo, functionName);
	vfprintf(stderr, fmt, argList);
	fprintf (stderr, "\n");
}

bool OoifLog::isDebugLogEnabled()
{
	static bool s_isInitialized = getenv(s_debugEnvName) && strcasecmp(getenv(s_debugEnvName), "YES") == 0;
	return s_isInitialized;
}

bool OoifLog::isInfoLogEnabled()
{
	static bool s_isInitialized = getenv(s_infoEnvName) && strcasecmp(getenv(s_infoEnvName), "YES") == 0;
	return s_isInitialized;
}

bool OoifLog::isWarningLogEnabled()
{
	static bool s_isInitialized = getenv(s_warningEnvName) && strcasecmp(getenv(s_warningEnvName), "YES") == 0;
	return s_isInitialized;
}

bool OoifLog::isErrorLogEnabled()
{
	static bool s_isInitialized = getenv(s_errorEnvName) && strcasecmp(getenv(s_errorEnvName), "YES") == 0;
	return s_isInitialized;
}

bool OoifLog::isLogFileEnabled(const char *fileName)
{
#ifdef LOG_FILE_ENABLE
	int iCnt = sizeof(s_enableLogFiles)/sizeof(std::string);
	for(int i=0;i<iCnt;i++)
	{
		if(strcmp(s_enableLogFiles[i].c_str(),fileName)==0)
		{
			return true;
		}
	}
#endif	
	return false;
}

} // namespace Ooif
