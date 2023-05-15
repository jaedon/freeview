/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef __LOGGER_H
#define __LOGGER_H

#include <tclib.h>

#include <PKTestNameSpace.h>

ENTER_PKTEST_NAMESPACE;


typedef enum _EDISPOSITION
{
    EDISPOSITION_PASSED  = 0,
    EDISPOSITION_FAILED  = 1,
    EDISPOSITION_SKIPPED = 2
} EDISPOSITION;


/**************
*    You can optionally call this function in your test implementation to log your messages.
*    This function can be called multiple times.
*    szTag is the tag name in the xml log file. The usage of szMessageFmt and the variable arglist
*    is the same as sprintf.
***************/
void DRM_CALL Log( __in const char *szTag, __in const char *szMessage, ...);

#ifndef __MACINTOSH__

#include <stdarg.h>
#include <apiParams.h>

/*
** call this to initialize the log engine
*/
void DRM_CALL InitLogger(DRM_BOOL fLoggerEnabled);

/*
** if using PK scripts to parse the result xml logs, call this second to create a list of test
** files which are expected to be used
*/
void DRM_CALL LogTestInfo( const TestScriptListEntry *f_rgoTestScriptList, const char** f_szSupportedPlatforms, DRM_TEST_RUN_TYPE f_eRunWhat );

/*
** call this first for each test case
*/
void DRM_CALL LogStart(long lTC, ENUM_TESTCASE_TYPE f_eType, __in const char *szTitle);

void DRM_CALL vLog(const char *szTag, __in_z __format_string const char *szMessage, va_list argptr);

void DRM_CALL LogBlob(const char* f_szTag, const char* f_szMessage, const DRM_BYTE* f_pbBlob, DRM_DWORD f_cbBlob );

void DRM_CALL LogBlobB64(
    __out_ecount( f_cchLogBuffer )            DRM_CHAR  *f_pchLogBuffer,
    __in                                      DRM_DWORD  f_cchLogBuffer,
    __in_ecount( f_cbBlob )             const DRM_BYTE  *f_pbBlob,
    __in                                      DRM_DWORD  f_cbBlob );

/*
** call this to log result. should be the last call for the test case
*/
void DRM_CALL LogEnd(
    __in       const enum _EDISPOSITION  f_eDisposition,
    __in       const DRM_UINT64          f_qwTicks,
    __in       const DRM_DWORD           f_dwTCID,
    __in_z_opt const DRM_CHAR           *f_szTitle,
    __in_z_opt const DRM_CHAR           *f_szTrace );

/*
** call this to uninitialize the log engine
*/
void DRM_CALL UninitLogger();

/*
** Set this to TRUE to silence test logging of Chk* macros (ie ChkDR, ChkArg etc)
** This can be useful to reduce printfs, and will speed up Chess testing due to
** the locking that vararg printf does.
*/
extern DRM_BOOL g_fSilenceTestChkLogger;

#endif /* __MACINTOSH__ */

EXIT_PKTEST_NAMESPACE;

#endif
