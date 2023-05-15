// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2012 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#include "Log.h"
#include <string.h>

namespace Lovefilm
{

const char *NjseLog::s_debugEnvName   = "ENABLE_NJSE_DEBUG_LOGS";
const char *NjseLog::s_infoEnvName    = "ENABLE_NJSE_INFO_LOGS";
const char *NjseLog::s_warningEnvName = "ENABLE_NJSE_WARNING_LOGS";
const char *NjseLog::s_errorEnvName   = "ENABLE_NJSE_ERROR_LOGS";

const char *NjseLog::s_debugLevelName   = "debug";
const char *NjseLog::s_infoLevelName    = "info";
const char *NjseLog::s_warningLevelName = "warning";
const char *NjseLog::s_errorLevelName   = "error";



void NjseLog::logDebug(const char *fileName, unsigned int lineNo, const char *functionName, const char *fmt, ...)
{
    if(isDebugLogEnabled()) {
        va_list argList; va_start(argList, fmt);
        log(fileName, lineNo, functionName, s_debugLevelName, fmt, argList);
        va_end(argList);
    }
}

void NjseLog::logInfo(const char *fileName, unsigned int lineNo, const char *functionName, const char *fmt, ...)
{
    if(isInfoLogEnabled()) {
        va_list argList; va_start(argList, fmt);
        log(fileName, lineNo, functionName, s_infoLevelName, fmt, argList);
        va_end(argList);
    }
}

void NjseLog::logWarning(const char *fileName, unsigned int lineNo, const char *functionName, const char *fmt, ...)
{
	if(isWarningLogEnabled())
	{
        va_list argList; va_start(argList, fmt);
        log(fileName, lineNo, functionName, s_warningLevelName, fmt, argList);
        va_end(argList);
    }
}

void NjseLog::logError(const char *fileName, unsigned int lineNo, const char *functionName, const char *fmt, ...)
{
	if(isErrorLogEnabled())
	{
        va_list argList; va_start(argList, fmt);
        log(fileName, lineNo, functionName, s_errorLevelName, fmt, argList);
        va_end(argList);
    }
}

void NjseLog::log(const char *fileName, unsigned int lineNo, const char *functionName, const char *level, const char *fmt, va_list argList)
{
    fprintf (stderr, "Lovefilm %s   : ", level);
    vfprintf(stderr, fmt, argList);
    fprintf (stderr, " (%s:%d::%s)\n", fileName, lineNo, functionName);
}

bool NjseLog::isDebugLogEnabled()
{
	static bool s_isInitialized = getenv(s_debugEnvName) && strcasecmp(getenv(s_debugEnvName), "YES") == 0;
	return s_isInitialized;
}

bool NjseLog::isInfoLogEnabled()
{
	static bool s_isInitialized = getenv(s_infoEnvName) && strcasecmp(getenv(s_infoEnvName), "YES") == 0;
	return s_isInitialized;
}

bool NjseLog::isWarningLogEnabled()
{
	static bool s_isInitialized = getenv(s_warningEnvName) && strcasecmp(getenv(s_warningEnvName), "YES") == 0;
	return s_isInitialized;
}

bool NjseLog::isErrorLogEnabled()
{
	static bool s_isInitialized = getenv(s_errorEnvName) && strcasecmp(getenv(s_errorEnvName), "YES") == 0;
	return s_isInitialized;
}

} // namespace Lovefilm
