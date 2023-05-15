/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef MDXLOG_H_
#define MDXLOG_H_

#include <nrdbase/Log.h>

DECLARE_TRACEAREA(MDX);
DECLARE_TRACEAREA(MDXLISTENER);
DECLARE_TRACEAREA(MDX_MONGOOSE);
DECLARE_TRACEAREA(MDX_MONGOOSE_REQUEST);

#if defined(BUILD_DEBUG)
#define MDXTRACE(...) MdxLog(netflix::Log::Trace, __VA_ARGS__)
#else
#define MDXTRACE(...)
#endif

#define MDXINFO(...) MdxLog(netflix::Log::Info, __VA_ARGS__)
#define MDXWARN(...) MdxLog(netflix::Log::Warn, __VA_ARGS__)
#define MDXERROR(...) MdxLog(netflix::Log::Error, __VA_ARGS__)

void MdxLog(netflix::Log::LogLevel logLevel, ...);

#endif /* MDXLOG_H_ */
