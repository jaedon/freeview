/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include "PKTestNameSpace.h"

ENTER_PKTEST_NAMESPACE

DRM_RESULT DRM_CALL PreTestCase_OEM_DeviceStore(long lTCID, const char *strTCName);
DRM_RESULT DRM_CALL PostTestCase_OEM_DeviceStore(long lTCID, const char *strTCName);

DRM_RESULT DRM_CALL Test_SetctxOemDeviceStore(long argc, __in_ecount(argc) char *argv[]);

DRM_RESULT DRM_CALL Test_ChkVar_OEM_DeviceStore_DeviceHandle(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_ChkVar_OEM_DeviceStore_AttrValue(long argc, __in_ecount(argc) char *argv[]);

DRM_RESULT DRM_CALL Test_Oem_DeviceStore_Initialize(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_Oem_DeviceStore_Uninitialize(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_Oem_DeviceStore_FreeDevice(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_Oem_DeviceStore_RegisterDevice(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_Oem_DeviceStore_UnRegisterDevice(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_Oem_DeviceStore_GetFirstDevice(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_Oem_DeviceStore_GetNextDevice(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_Oem_DeviceStore_GetDeviceByID(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_Oem_DeviceStore_GetDeviceID(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_Oem_DeviceStore_GetDeviceAttr(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_Oem_DeviceStore_GetDeviceAttrCount(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_Oem_DeviceStore_GetDeviceAttrByIndex(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_Oem_DeviceStore_SetDeviceAttr(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_Oem_DeviceStore_CommitDevice(long argc, __in_ecount(argc) char *argv[]);

DRM_RESULT DRM_CALL Test_DRM_DeviceStore_FreeDevice(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_DRM_DeviceStore_UnRegisterDevice(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_DRM_DeviceStore_GetFirstDevice(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_DRM_DeviceStore_GetNextDevice(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_DRM_DeviceStore_GetDeviceByID(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_DRM_DeviceStore_GetDeviceInfo(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_DRM_DeviceStore_GetDeviceAttr(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_DRM_DeviceStore_GetDeviceAttrCount(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_DRM_DeviceStore_GetDeviceAttrByIndex(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_DRM_DeviceStore_SetDeviceAttr(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_DRM_DeviceStore_IsDeviceValid(long argc, __in_ecount(argc) char *argv[]);

EXIT_PKTEST_NAMESPACE

