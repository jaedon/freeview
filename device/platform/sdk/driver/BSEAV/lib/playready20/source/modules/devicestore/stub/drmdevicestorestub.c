/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmdevicestore.h>

ENTER_PK_NAMESPACE_CODE;

DRM_API DRM_RESULT DRM_CALL DRM_DVS_Initialize(
    IN     DRM_DEVICESTORE_CONTEXT   *f_pDeviceStoreContext,
    IN     DRM_APP_CONTEXT       *f_pDeviceStoreInitData )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_VOID DRM_CALL DRM_DVS_Uninitialize(
    IN OUT DRM_DEVICESTORE_CONTEXT      *f_pDeviceStoreContext )
{
    return;
}

DRM_API DRM_RESULT DRM_CALL DRM_DVS_RegisterDevice(
    IN OUT DRM_DEVICESTORE_CONTEXT      *f_pDeviceStoreContext,
    IN     DRM_BYTEBLOB                  f_DeviceID,
       OUT DRM_DEVICESTORE_HANDLE       *f_pDeviceHandle )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_DVS_UnRegisterDevice(
    IN OUT DRM_DEVICESTORE_CONTEXT      *f_pDeviceStoreContext,
    IN     DRM_BYTEBLOB                  f_DeviceID )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_VOID DRM_CALL DRM_DVS_FreeDevice(
    IN    DRM_DEVICESTORE_HANDLE            f_hDeviceHandle )
{
    return;
}

DRM_API DRM_RESULT DRM_CALL DRM_DVS_GetFirstDevice(
    IN OUT DRM_DEVICESTORE_CONTEXT      *f_pDeviceStoreContext,
       OUT DRM_DEVICESTORE_HANDLE       *f_pDeviceHandle )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_DVS_GetNextDevice(
    IN OUT DRM_DEVICESTORE_CONTEXT      *f_pDeviceStoreContext,
       OUT DRM_DEVICESTORE_HANDLE       *f_pDeviceHandle )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_DVS_GetDeviceByID(
    IN OUT DRM_DEVICESTORE_CONTEXT      *f_pDeviceStoreContext,
    IN     DRM_BYTEBLOB                     f_DeviceID,
       OUT DRM_DEVICESTORE_HANDLE            *f_pDeviceHandle )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_DVS_GetDeviceID(
    IN     DRM_DEVICESTORE_HANDLE             f_hDeviceHandle,
       OUT DRM_BYTE                        *f_pbDeviceID,
    IN OUT DRM_DWORD                       *f_pcbDeviceID )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_DVS_GetDeviceAttr(
    IN     DRM_DEVICESTORE_HANDLE             f_hDeviceHandle,
    IN     DRM_BYTEBLOB                     f_AttrName,
       OUT DRM_BYTE                        *f_pbAttrValue,
    IN OUT DRM_DWORD                       *f_pcbAttrValue )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_DVS_GetDeviceAttrCount(
    IN     DRM_DEVICESTORE_HANDLE               f_hDeviceHandle,
       OUT DRM_DWORD                           *f_pdwAttrCount )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_DVS_GetDeviceAttrByIndex(
    IN     DRM_DEVICESTORE_HANDLE             f_hDeviceHandle,
    IN     DRM_DWORD                        f_dwAttrIndex,
       OUT DRM_BYTE                        *f_pbAttrName,
    IN OUT DRM_DWORD                       *f_pcbAttrName,
       OUT DRM_BYTE                        *f_pbAttrValue,
    IN OUT DRM_DWORD                       *f_pcbAttrValue )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_DVS_SetDeviceAttr(
    IN     DRM_DEVICESTORE_HANDLE           f_hDeviceHandle,
    IN     DRM_BYTEBLOB                     f_AttrName,
    IN     DRM_BYTEBLOB                     f_AttrValue )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_DVS_CommitDevice(
    IN     DRM_DEVICESTORE_HANDLE             f_hDeviceHandle )
{
    return DRM_E_NOTIMPL;
}

EXIT_PK_NAMESPACE_CODE;

