/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include "PKTestNameSpace.h"

ENTER_PKTEST_NAMESPACE


DRM_RESULT DRM_CALL PreTestCase_OEM_DeviceStore(long lTCID, char *strTCName);
DRM_RESULT DRM_CALL PostTestCase_OEM_DeviceStore(long lTCID, char *strTCName);

DRM_RESULT DRM_CALL Test_SetctxOemDeviceStore(long argc, char *argv[]);

DRM_RESULT DRM_CALL Test_ChkVar_OEM_DeviceStore_DeviceHandle(long argc, char *argv[]);
DRM_RESULT DRM_CALL Test_ChkVar_OEM_DeviceStore_AttrValue(long argc, char *argv[]);

DRM_RESULT DRM_CALL Test_Oem_DeviceStore_Initialize(long argc, char *argv[]);
DRM_RESULT DRM_CALL Test_Oem_DeviceStore_Uninitialize(long argc, char *argv[]);
DRM_RESULT DRM_CALL Test_Oem_DeviceStore_FreeDevice(long argc, char *argv[]);
DRM_RESULT DRM_CALL Test_Oem_DeviceStore_RegisterDevice(long argc, char *argv[]);
DRM_RESULT DRM_CALL Test_Oem_DeviceStore_UnRegisterDevice(long argc, char *argv[]);
DRM_RESULT DRM_CALL Test_Oem_DeviceStore_GetFirstDevice(long argc, char *argv[]);
DRM_RESULT DRM_CALL Test_Oem_DeviceStore_GetNextDevice(long argc, char *argv[]);
DRM_RESULT DRM_CALL Test_Oem_DeviceStore_GetDeviceByID(long argc, char *argv[]);
DRM_RESULT DRM_CALL Test_Oem_DeviceStore_GetDeviceID(long argc, char *argv[]);
DRM_RESULT DRM_CALL Test_Oem_DeviceStore_GetDeviceAttr(long argc, char *argv[]);
DRM_RESULT DRM_CALL Test_Oem_DeviceStore_GetDeviceAttrCount(long argc, char *argv[]);
DRM_RESULT DRM_CALL Test_Oem_DeviceStore_GetDeviceAttrByIndex(long argc, char *argv[]);
DRM_RESULT DRM_CALL Test_Oem_DeviceStore_SetDeviceAttr(long argc, char *argv[]);
DRM_RESULT DRM_CALL Test_Oem_DeviceStore_CommitDevice(long argc, char *argv[]);

EXIT_PKTEST_NAMESPACE
