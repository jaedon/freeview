/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/
#ifndef __HEADERTEST_H
#define __HEADERTEST_H

#include "PKTestNameSpace.h"

ENTER_PKTEST_NAMESPACE

extern DRM_CONST_STRING g_strContentHeader, g_strContentHeaderBackup, *g_pStrContentHeader;
extern DRM_BYTE  *g_pbHBContext;
extern DRM_DWORD g_cbHBContext;

DRM_RESULT DRM_CALL TestLoadHeader(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestLoadWideCharHeader(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestContentHeaderVerify(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestContentHeaderGetAttribute(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestContentHeaderGetUplink(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestGetHeaderVersion(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL Test_HB_CreateHeader(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL Test_HB_CloseHeader(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL Test_HB_ReallocHeader(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL Test_HB_GetContextSize(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL Test_HB_OpenNode(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL Test_HB_CloseCurrNode(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL Test_HB_AddData(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL Test_HB_SetKeyID(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL Test_HB_SetLicAcqUrl(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL Test_HB_SetChecksum(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL Test_HB_SetAttribute(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestPlayReadyObjectParser(long argc, __in_ecount(argc) char **argv );

DECLARE_MODULE_DEFS( Header )

EXIT_PKTEST_NAMESPACE

#endif /* __HEADERTEST_H */
