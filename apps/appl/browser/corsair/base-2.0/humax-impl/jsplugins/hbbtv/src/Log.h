// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifndef LOG_H_
#define LOG_H_

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <iostream>

namespace Ooif
{
// Debug macros
// enable/disable flags
#if defined(CONFIG_CORSAIR_LOG)
#ifndef OPERA_LOG_ENALBLED
# define OOIF_LOG_PRINT(fmt, ...) \
	do { \
        std::string buf;\
        buf.append("hbbtv plugin: ");\
        buf.append(fmt);\
        HxLOG_Print(buf.c_str(),##__VA_ARGS__);\
	} while (0)
# define OOIF_LOG_FPRINT(fmt, ...) \
    do { \
        std::string buf;\
        buf.append("hbbtv plugin: ");\
        buf.append(fmt);\
        HxLOG_Print(buf.c_str(),##__VA_ARGS__);\
    } while (0)
# define OOIF_LOG_DEBUG(fmt, ...) \
    do { \
        std::string buf;\
        buf.append("hbbtv plugin: ");\
        buf.append(fmt);\
        HxLOG_Debug(buf.c_str(),##__VA_ARGS__);\
    } while (0)
# define OOIF_LOG_INFO(fmt, ...) \
    do { \
        std::string buf;\
        buf.append("hbbtv plugin: ");\
        buf.append(fmt);\
        HxLOG_Info(buf.c_str(),##__VA_ARGS__);\
    } while (0)
# define OOIF_LOG_WARNING(fmt, ...) \
    do { \
        std::string buf;\
        buf.append("hbbtv plugin: ");\
        buf.append(fmt);\
        HxLOG_Warning(buf.c_str(),##__VA_ARGS__);\
    } while (0)
# define OOIF_LOG_ERROR(fmt, ...) \
    do { \
        std::string buf;\
        buf.append("hbbtv plugin: ");\
        buf.append(fmt);\
        HxLOG_Error(buf.c_str(),##__VA_ARGS__);\
    } while (0)
#else
# define OOIF_LOG_PRINT(fmt, ...)	printf(fmt,##__VA_ARGS__)
# define OOIF_LOG_FPRINT(fmt, ...)	fprintf(stderr,fmt,##__VA_ARGS__)
# define OOIF_LOG_DEBUG(fmt, ...) Ooif::OoifLog::logDebug(__FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__)
# define OOIF_LOG_INFO(fmt, ...) Ooif::OoifLog::logInfo(__FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__)
# define OOIF_LOG_WARNING(fmt, ...) Ooif::OoifLog::logWarning(__FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__)
# define OOIF_LOG_ERROR(fmt, ...) Ooif::OoifLog::logError(__FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__)
#endif // OPERA_LOG_ENALBLED
#else
# define OOIF_LOG_PRINT(fmt, ...)
# define OOIF_LOG_FPRINT(fmt, ...)
# define OOIF_LOG_DEBUG(fmt, ...) 
# define OOIF_LOG_INFO(fmt, ...) 
# define OOIF_LOG_WARNING(fmt, ...)
# define OOIF_LOG_ERROR(fmt, ...) 
#endif //CONFIG_DEBUG


class OoifLog
{
public:
	static void logDebug(const char *fileName, unsigned int lineNo, const char *functionName, const char *fmt, ...);
	static void logInfo(const char *fileName, unsigned int lineNo, const char *functionName, const char *fmt, ...);
	static void logWarning(const char *fileName, unsigned int lineNo, const char *functionName, const char *fmt, ...);
	static void logError(const char *fileName, unsigned int lineNo, const char *functionName, const char *fmt, ...);

private:
	static void log(const char *fileName, unsigned int lineNo, const char *functionName, const char *level, const char *fmt, va_list argList);

	static bool isDebugLogEnabled();
	static bool isInfoLogEnabled();
	static bool isWarningLogEnabled();
	static bool isErrorLogEnabled();
	static bool isLogFileEnabled(const char *fileName);

	static const char *s_debugEnvName;
	static const char *s_infoEnvName;
	static const char *s_warningEnvName;
	static const char *s_errorEnvName;

	static const char *s_debugLevelName;
	static const char *s_infoLevelName;
	static const char *s_warningLevelName;
	static const char *s_errorLevelName;
	static const std::string s_enableLogFiles[];
};

} // namespace Ooif

#endif // LOG_H_
