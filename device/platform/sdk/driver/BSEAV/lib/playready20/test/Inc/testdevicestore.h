/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef __DEVICESTORETEST_H
#define __DEVICESTORETEST_H

#include "PKTestNameSpace.h"

ENTER_PKTEST_NAMESPACE

extern DRM_APP_CONTEXT *g_pManagerContext;

extern DRM_DWORD g_cchChallenge;
extern DRM_CHAR *g_pszChallenge;

extern DRM_DWORD g_cbData; 
extern DRM_BYTE *g_pbData;

extern DRM_BYTE *g_pbDLRB;

DRM_RESULT DRM_CALL TestDeviceStoreRegisterDevice(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestDeviceStoreUnRegisterDevice(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestDeviceStoreCommitDevice(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestDeviceStoreFreeDevice(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestDeviceStoreGetDeviceByID(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestDeviceStoreGetFirstDevice(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestDeviceStoreGetNextDevice(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestDeviceStoreGetDeviceID(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestDeviceStoreSetDeviceAttr(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestDeviceStoreGetDeviceAttr(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestDeviceStoreGetDeviceAttrByIndex(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestDeviceStoreGetDeviceAttrCount(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestDeviceStoreChangeTime(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestDeviceStoreInitialize(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestDeviceStoreUninitialize(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestDeviceStoreRegisterManyDevices(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestDeviceStoreUnRegisterManyDevices(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestDeviceStoreSetManyAttr(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestDeviceStoreGetManyAttr(long argc, __in_ecount(argc) char **argv);

DECLARE_MODULE_DEFS( DeviceStore )

EXIT_PKTEST_NAMESPACE

#endif /* __DEVICESTORETEST_H */
