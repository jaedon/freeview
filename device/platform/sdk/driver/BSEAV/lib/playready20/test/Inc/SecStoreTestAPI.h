/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef _DRM_SECSTORETEST_H_
#define _DRM_SECSTORETEST_H_

#include "PKTestNameSpace.h"

ENTER_PKTEST_NAMESPACE

/*
** ---------------------------------------
** SecStore Test Global Variable Names
** ---------------------------------------
*/

/*
** ---------------------------------------
** SecStore Non-APIMAP Functions 
** ---------------------------------------
*/

/*
** ---------------------------------------
** SecStore APIMAP Functions
** ---------------------------------------
*/
DRM_RESULT DRM_CALL TestDRM_SST_GetData(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestDRM_SST_SetData(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestDRM_SST_OpenEnumerator(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestDRM_SST_EnumNext(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestDRM_SST_OpenKeyTokens(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestDRM_SST_GetTokenValue(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestDRM_SST_SetTokenValue(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestDRM_SST_GetAllData(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestDRM_SST_CloseKey(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestDRM_SST_DeleteKey(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestDRM_SST_Initialize(long iIndex, __in_opt char **argv);
DRM_RESULT DRM_CALL TestDRM_SST_CreateGlobalStorePassword(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestDRM_SST_CreateLicenseStatePassword(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestDRM_SST_OpenAndLockSlot(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestDRM_SST_SetLockedData(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestDRM_SST_GetLockedData(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestDRM_SST_CloseLockedSlot(long argc, __in_ecount(argc) char **argv);

DECLARE_MODULE_DEFS( SecStore )

EXIT_PKTEST_NAMESPACE

#endif /* _DRM_SECSTORETEST_H_ */
