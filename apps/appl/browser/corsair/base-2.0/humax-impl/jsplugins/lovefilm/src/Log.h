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

namespace Lovefilm
{
// Debug macros
// enable/disable flags
#if defined(CONFIG_CORSAIR_LOG)
#define HMX_LOG_ENABLED 1
//#define NJSE_DEBUG_ENABLED 1
//#define NJSE_INFO_ENABLED 1
//#define NJSE_WARNING_ENABLED 1
//#define NJSE_ERROR_ENABLED 1
#endif

#ifdef HMX_LOG_ENABLED
# define NJSE_LOG_PRINT(fmt, ...) \
	do { \
        std::string buf;\
        buf.append("lovefilm plugin: ");\
        buf.append(fmt);\
        HxLOG_Print(buf.c_str(),##__VA_ARGS__);\
	} while (0)
# define NJSE_LOG_FPRINT(fmt, ...) \
    do { \
        std::string buf;\
        buf.append("lovefilm plugin: ");\
        buf.append(fmt);\
        HxLOG_Print(buf.c_str(),##__VA_ARGS__);\
    } while (0)
# define NJSE_LOG_DEBUG(fmt, ...) \
    do { \
        std::string buf;\
        buf.append("lovefilm plugin: ");\
        buf.append(fmt);\
        HxLOG_Debug(buf.c_str(),##__VA_ARGS__);\
    } while (0)
# define NJSE_LOG_INFO(fmt, ...) \
    do { \
        std::string buf;\
        buf.append("lovefilm plugin: ");\
        buf.append(fmt);\
        HxLOG_Info(buf.c_str(),##__VA_ARGS__);\
    } while (0)
# define NJSE_LOG_WARNING(fmt, ...) \
    do { \
        std::string buf;\
        buf.append("lovefilm plugin: ");\
        buf.append(fmt);\
        HxLOG_Warning(buf.c_str(),##__VA_ARGS__);\
    } while (0)
# define NJSE_LOG_ERROR(fmt, ...) \
    do { \
        std::string buf;\
        buf.append("lovefilm plugin: ");\
        buf.append(fmt);\
        HxLOG_Error(buf.c_str(),##__VA_ARGS__);\
    } while (0)
#else
// macros for different debug levels
#ifdef NJSE_DEBUG_ENABLED
# define NJSE_LOG_DEBUG(fmt, ...) Lovefilm::NjseLog::logDebug(__FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__)
#else
# define NJSE_LOG_DEBUG(fmt, ...)
#endif

#ifdef NJSE_INFO_ENABLED
# define NJSE_LOG_INFO(fmt, ...) Lovefilm::NjseLog::logInfo(__FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__)
#else
# define NJSE_LOG_INFO(fmt, ...)
#endif

#ifdef NJSE_WARNING_ENABLED
# define NJSE_LOG_WARNING(fmt, ...) Lovefilm::NjseLog::logWarning(__FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__)
#else
# define NJSE_LOG_WARNING(fmt, ...)
#endif

#ifdef NJSE_ERROR_ENABLED
# define NJSE_LOG_ERROR(fmt, ...) Lovefilm::NjseLog::logError(__FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__)
#else
# define NJSE_LOG_ERROR(fmt, ...)
#endif
#endif // HMX_LOG_ENABLED


class NjseLog
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

    static const char *s_debugEnvName;
    static const char *s_infoEnvName;
    static const char *s_warningEnvName;
    static const char *s_errorEnvName;

    static const char *s_debugLevelName;
    static const char *s_infoLevelName;
    static const char *s_warningLevelName;
    static const char *s_errorLevelName;
};

} // namespace Lovefilm

#endif // LOG_H_
