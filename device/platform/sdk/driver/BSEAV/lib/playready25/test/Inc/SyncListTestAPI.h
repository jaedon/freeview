/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef _DRM_SYNCLISTTEST_H_
#define _DRM_SYNCLISTTEST_H_

#include "PKTestNameSpace.h"

ENTER_PKTEST_NAMESPACE

/*
** ---------------------------------------
** SyncList Test Global Variable Names
** ---------------------------------------
*/

/*
** ---------------------------------------
** SyncList Non-APIMAP Functions
** ---------------------------------------
*/

/*
** ---------------------------------------
** SyncList APIMAP Functions
** ---------------------------------------
*/
DRM_RESULT DRM_CALL TestSyncListReset(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestGenerateSyncListChallenge(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestAddLicenseToStore(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestSetEnhancedData(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestSetHeader(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestPlayback(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestSynclistChangeTime(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestEnumLicense2(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestGetContentHeaderByKID(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestGetSyncStoreEntry(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestSyncUpdateKID(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestSetManagerReinitialize(long argc, __in_ecount(argc) char **argv);

DECLARE_MODULE_DEFS( SyncList )

EXIT_PKTEST_NAMESPACE

#endif /* _DRM_SYNCLISTTEST_H_ */
