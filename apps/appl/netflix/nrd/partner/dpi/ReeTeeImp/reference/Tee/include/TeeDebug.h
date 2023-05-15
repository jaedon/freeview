/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef _TEEDEBUG_H_
#define _TEEDEBUG_H_

#include "DebugTypes.h"

/*
 * various debug MACROS are enabled with a command line arg, like this:
 *
 * --debug "<comma-separated list of flags>"
 *      or
 * --debug="<comma-separated list of flags>"
 *      or
 * -d "<comma-separated list of flags>"
 *      or
 * -d="<comma-separated list of flags>"
 *
 * Examples:
 *
 * --debug "init,comm"
 * --debug "all"
 * --debug "secstore"
 * --debug "init,comm,msgs"
 * (and so on)
 *
 * If you give "--debug" with no args, this implies "all"
 */

// global debug flags, initialized in tee.c
extern nfDebugFlags_t nfDebugFlags;

// redefine teeDebugPrint to put it north of the TPI
//#define teeDebugPrint printf
void teeDebugPrint(const char *format, ...);

/*
 * debug macro definitions -- note that the fprintf call can easily be replaced
 * with a different function, as long as it takes varargs...
 */

// #define NRDP_TEEDEBUG
#ifdef NRDP_TEEDEBUG
#define INITDBG(...) if ( nfDebugFlags & nfDEBUG_INIT ) teeDebugPrint( __VA_ARGS__)
#define COMMDBG(...) if ( nfDebugFlags & nfDEBUG_COMM_AGENT ) teeDebugPrint(__VA_ARGS__)
#define MGMTDBG(...) if ( nfDebugFlags & nfDEBUG_MGMT_AGENT ) teeDebugPrint(__VA_ARGS__)
#define MSGDBG(...) if ( nfDebugFlags & nfDEBUG_MSG ) teeDebugPrint(__VA_ARGS__)
#define STORAGEDBG(...) if ( nfDebugFlags & nfDEBUG_STORAGE ) teeDebugPrint(__VA_ARGS__)
#define CRYPTODBG(...) do { if ( nfDebugFlags & nfDEBUG_CRYPTO ) teeDebugPrint( __VA_ARGS__); } while (0)
#define MEMDBG(...) if ( nfDebugFlags & nfDEBUG_MEMORY ) teeDebugPrint( __VA_ARGS__)
#define RSADBG(...) if ( nfDebugFlags & nfDEBUG_RSA ) teeDebugPrint( __VA_ARGS__)
#define AESDBG(...) if ( nfDebugFlags & nfDEBUG_AES ) teeDebugPrint( __VA_ARGS__)
#define DRMDBG(...) if ( nfDebugFlags & nfDEBUG_DRM ) teeDebugPrint( __VA_ARGS__)
#define DRMERR(...) teeDebugPrint( __VA_ARGS__)
#else
#define INITDBG(...)
#define COMMDBG(...)
#define MGMTDBG(...)
#define MSGDBG(...)
#define STORAGEDBG(...)
#define CRYPTODBG(...)
#define MEMDBG(...)
#define RSADBG(...)
#define AESDBG(...)
#define DRMDBG(...)
#define DRMERR(...)
#endif
#endif /* _TEEDEBUG_H_ */
