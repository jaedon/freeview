/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef __UTILTEST_H
#define __UTILTEST_H

#include "PKTestNameSpace.h"

ENTER_PKTEST_NAMESPACE

/* globals */
#define MAX_BUFF 3
extern DRM_BYTE* g_buf[MAX_BUFF];
extern DRM_BYTE g_bufUsed;

#define COOKIECHAR 'k'

DRM_RESULT DRM_CALL TestAllocBuffer(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestCompBuffer(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestSetBuffer(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestVerifyBuffer(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL Test_API_DRMCRT_memmove(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL Test_API_DRMCRT_wcsntol(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL Test_API_DRMCRT_strncmp(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL Test_API_DRMCRT_wcsncmp(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL Test_API_DRMCRT_wcslen(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL Test_API_StringCchCopyW(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL Test_API_DRMCRT_abs(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL Test_API_DRMCRT_isxxx(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL Test_API_DRM_Base64Encode(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL Test_API_DRM_Base64Decode(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL Test_API_DecodeKID(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestUtilAreGUIDEqual(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestUtilGUIDToString(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestUtilReadGUID(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestCRTStringCchLengthW(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestCRTwcstr(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestCRTCchSPrintFSingleParam(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestNumberToString(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestECCExpectedErrorCode(long argc, __in_ecount(argc) char **argv);


DECLARE_MODULE_DEFS( UtilTest )

EXIT_PKTEST_NAMESPACE

#endif /* __UTILTEST_H */
