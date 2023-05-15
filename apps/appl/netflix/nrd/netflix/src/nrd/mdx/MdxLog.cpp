/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "MdxLog.h"
#include <nrdbase/StringFormatter.h>

using namespace netflix;

void MdxLog(Log::LogLevel logLevel, ...)
{
    va_list args;
    va_start(args, logLevel);
    const char *fmt = va_arg(args, const char *);
    // NOTE: we only specify type for logLevel's higher than trace, in order to not send trace messages in logblobs
    Log::Message msg(TRACE_MDX, logLevel, (logLevel == Log::Trace) ? "" : "mdx");
    msg.m_message = StringFormatter::vsformat(fmt, args);
    va_end(args);
    msg.send();
}
