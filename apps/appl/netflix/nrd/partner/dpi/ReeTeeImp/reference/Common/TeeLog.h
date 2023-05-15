/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef TeeLog_h
#define TeeLog_h

#include <nrdbase/config.h>
#include <stdarg.h>

typedef enum {
    Tee_Error,
    Tee_Warning,
    Tee_Trace
} TeeLogSeverity;

#ifdef __cplusplus
extern "C" {
#endif
void teeLog(TeeLogSeverity severity, const char *fmt, ...);
void teeVLog(TeeLogSeverity severity, const char *fmt, va_list list);
#ifdef __cplusplus
}
#endif

#ifdef NRDP_HAS_TRACING
#define TEE_TRACE(fmt, ...) teeLog(Tee_Trace, fmt, __VA_ARGS__)
#else
#define TEE_TRACE(fmt, ...)
#endif

#endif
