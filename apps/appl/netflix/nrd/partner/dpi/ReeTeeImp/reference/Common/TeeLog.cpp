 /*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "TeeLog.h"
#include <stdarg.h>
#include <nrd/AppLog.h>

using namespace netflix;

extern "C" {
void teeVLog(TeeLogSeverity severity, const char *fmt, va_list args)
{
    Log::LogLevel level = Log::Trace;
    switch (severity) {
    case Tee_Trace:
#ifndef NRDP_HAS_TRACING
        return;
#endif
        level = Log::Trace;
        break;
    case Tee_Error:
        level = Log::Error;
        break;
    case Tee_Warning:
        level = Log::Warn;
        break;
    }
    va_list args2;
    va_copy(args2, args);

    Log::log(TRACE_TEE, level, Log::Message::DefaultFlags, fmt, args2);
    va_end(args2);
}

void teeLog(TeeLogSeverity severity, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    teeVLog(severity, fmt, args);
    va_end(args);
}
}
