
#ifndef NativeLog_h
#define NativeLog_h

#include <string>
#include <stdio.h>

/*******************************************************************************************
 * Commented NTRACE or Log::trace message in humax_dpi
 *    because netflix::base::Log() cause the NTS failure (NCCP Logblob error). 
 * We dont know the exact reason for the failure, 
 *    but some special characters used in log message cause NRD internal XML parsing error.
 *
 * [NCCP Logblob error]
 *   [GIBBON_MAIN:0x777f6000] NCCP(warn): NCCP client time mismatch
 *   [GIBBON_MAIN:0x777f6000] DISK_CACHE(trace): Couldn't find https://... in DiskCache.
 *   [GIBBON_MAIN:0x777f6000] DISK_CACHE(trace): https://partnertools... Miss
 *******************************************************************************************/

#if 0 /* defined(NRDLIB_HAVE_TRACING) */
# define H_TRACE(a, ...) netflix::base::Log::trace(a, __VA_ARGS__)
# define H_ERROR(a, ...) netflix::base::Log::error(a, __VA_ARGS__)
# define H_TRACES(a,s) netflix::base::Log::stream(a,netflix::base::Log::Trace) << s
#else
# define H_TRACE(a, ...)
# define H_ERROR(a, ...)
# define H_TRACES(a,s)
#endif

#endif /* NativeLog_h */
