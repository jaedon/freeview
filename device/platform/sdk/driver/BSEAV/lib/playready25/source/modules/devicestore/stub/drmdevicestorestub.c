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
    __in     DRM_DEVICESTORE_CONTEXT   *f_pDeviceStoreContext,
    __in     DRM_APP_CONTEXT           *f_pDeviceStoreInitData )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_VOID DRM_CALL DRM_DVS_Uninitialize(
    __inout DRM_DEVICESTORE_CONTEXT      *f_pDeviceStoreContext )
{
    return;
}

DRM_API DRM_RESULT DRM_CALL DRM_DVS_RegisterDevice(
    __inout  DRM_DEVICESTORE_CONTEXT      *f_pDeviceStoreContext,
    __in     DRM_BYTEBLOB                  f_DeviceID,
    __out    DRM_DEVICESTORE_HANDLE       *f_pDeviceHandle )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_DVS_UnRegisterDevice(
    __inout DRM_DEVICESTORE_CONTEXT      *f_pDeviceStoreContext,
    __in    DRM_BYTEBLOB                  f_DeviceID )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_VOID DRM_CALL DRM_DVS_FreeDevice(
    __in_opt    DRM_DEVICESTORE_HANDLE            f_hDeviceHandle )
{
    return;
}

DRM_API DRM_RESULT DRM_CALL DRM_DVS_GetFirstDevice(
    __inout DRM_DEVICESTORE_CONTEXT      *f_pDeviceStoreContext,
    __out   DRM_DEVICESTORE_HANDLE       *f_pDeviceHandle )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_DVS_GetNextDevice(
    __inout DRM_DEVICESTORE_CONTEXT      *f_pDeviceStoreContext,
    __out   DRM_DEVICESTORE_HANDLE       *f_pDeviceHandle )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_DVS_GetDeviceByID(
    __inout DRM_DEVICESTORE_CONTEXT           *f_pDeviceStoreContext,
    __in    DRM_BYTEBLOB                       f_DeviceID,
    __out   DRM_DEVICESTORE_HANDLE            *f_pDeviceHandle )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_DVS_GetDeviceID(
    __in                             const DRM_DEVICESTORE_HANDLE  f_hDeviceHandle,
    __out_ecount_opt(*f_pcbDeviceID)       DRM_BYTE               *f_pbDeviceID,
    __inout                                DRM_DWORD              *f_pcbDeviceID )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_DVS_GetDeviceAttr(
    __in                              const DRM_DEVICESTORE_HANDLE  f_hDeviceHandle,
    __in                                    DRM_BYTEBLOB            f_AttrName,
    __out_ecount_opt(*f_pcbAttrValue)       DRM_BYTE               *f_pbAttrValue,
    __inout                                 DRM_DWORD              *f_pcbAttrValue )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_DVS_GetDeviceAttrCount(
    __in     DRM_DEVICESTORE_HANDLE               f_hDeviceHandle,
    __out    DRM_DWORD                           *f_pdwAttrCount )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_DVS_GetDeviceAttrByIndex(
    __in                              const DRM_DEVICESTORE_HANDLE  f_hDeviceHandle,
    __in                                    DRM_DWORD               f_dwAttrIndex,
    __out_ecount_opt(*f_pcbAttrName)        DRM_BYTE               *f_pbAttrName,
    __inout                                 DRM_DWORD              *f_pcbAttrName,
    __out_ecount_opt(*f_pcbAttrValue)       DRM_BYTE               *f_pbAttrValue,
    __inout                                 DRM_DWORD              *f_pcbAttrValue )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_DVS_SetDeviceAttr(
    __in     DRM_DEVICESTORE_HANDLE           f_hDeviceHandle,
    __in     DRM_BYTEBLOB                     f_AttrName,
    __in     DRM_BYTEBLOB                     f_AttrValue )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_DVS_CommitDevice(
    __in     DRM_DEVICESTORE_HANDLE             f_hDeviceHandle )
{
    return DRM_E_NOTIMPL;
}

EXIT_PK_NAMESPACE_CODE;

