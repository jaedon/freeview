/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmcommon.h>
#include <ndtconstants.h>
#include <oemdevstore.h>
#include <drmdevstore.h>

ENTER_PK_NAMESPACE_CODE;

/**********************************************************************
**
** Function:    DRM_DeviceStore_HasTimedOut
**
** Synopsis:    Compares the current time against a previous time and an timeout period
**              to see if the timeout has expired.
**
** Arguments:   [f_pOEMContext]      -- Pointer to the OEM context; may be NULL
**              [f_pftStartTime]     -- Starting time of measurement
**              [f_dwTimeoutSeconds] -- Timeout period in seconds
**
** Returns:     TRUE if the timeout has expired,
**              FALSE otherwise
**
**********************************************************************/
DRM_API DRM_BOOL DRM_CALL DRM_DeviceStore_HasTimedOut(
    __in_opt    DRM_VOID    *f_pOEMContext,
    __in        DRMFILETIME *f_pftStartTime,
    __in const  DRM_DWORD    f_dwTimeoutSeconds
)
{
    return FALSE;
}


/**********************************************************************
**
** Function:    DRM_DeviceStore_MarkDeviceAsValid
**
** Synopsis:    Marks the given device as valid.  Sets a device property
**              with the current time, which is queried by
**              DRM_DeviceStore_IsDeviceValid
**
** Arguments:   [f_pOEMContext]    -- Pointer to the OEM context; may be NULL
**              [f_hDeviceHandle]  -- The device handle
**
** Returns:     DRM_SUCCESS on success or
**              DRM_E_INVALIDARG if any of the arguments are invalid
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_DeviceStore_MarkDeviceAsValid(
    __in_opt    DRM_VOID            *f_pOEMContext,
    __in const  OEM_DEVICE_HANDLE    f_hDeviceHandle
)
{
    return DRM_E_NOTIMPL;
}



/**********************************************************************
**
** Function:    DRM_DeviceStore_IsDeviceValid
**
** Synopsis:    Determines if the given device is valid.  This uses
**              the time set at the time of DRM_DeviceStore_MarkDeviceAsValid
**              and ensures the validation timeout has not occured.
**
** Arguments:   [f_pOEMContext]    -- Pointer to the OEM context; may be NULL
**              [f_hDeviceHandle]  -- The device handle
**
** Returns:     TRUE if the device is valid; FALSE otherwise
**
**********************************************************************/
DRM_API DRM_BOOL DRM_CALL DRM_DeviceStore_IsDeviceValid(
    __in_opt   DRM_VOID           *f_pOEMContext,
    __in const OEM_DEVICE_HANDLE   f_hDeviceHandle
)
{
    return DRM_E_NOTIMPL;
}



/**********************************************************************
**
** Function:    DRM_DeviceStore_FreeDevice
**
** Synopsis:    A wrapper for Oem_DeviceStore_FreeDevice.
**
** Arguments:   [f_hDeviceHandle]     -- The device handle
**
** Returns:     N/A
**
**********************************************************************/
DRM_API DRM_VOID DRM_CALL DRM_DeviceStore_FreeDevice(
    __in const   OEM_DEVICE_HANDLE            f_hDeviceHandle
)
{
    /* Do nothing */
}


/**********************************************************************
**
** Function:    DRM_DeviceStore_UnRegisterDevice
**
** Synopsis:    A wrapper for Oem_DeviceStore_UnRegisterDevice
**
** Arguments:   [f_pOemDevStoreContext] -- OEM device store context
**              [f_DeviceCertificate]   -- Certificate
**              [f_DeviceSerialNumber]  -- Serial Number
**
** Returns:     return from underlying call is bubbled up
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_DeviceStore_UnRegisterDevice(
    __inout       OEM_DEVICESTORE_CONTEXT *f_pOemDevStoreContext,
    __in const    DRM_BYTEBLOB             f_DeviceCertificate,
    __in const    DRM_ID                   f_DeviceSerialNumber
)
{
    return DRM_E_NOTIMPL;
}


/**********************************************************************
**
** Function:    DRM_DeviceStore_GetFirstDevice
**
** Synopsis:    A wrapper for Oem_DeviceStore_GetFirstDevice
**
** Arguments:   [f_pOemDevStoreContext] -- OEM device store context
**              [f_phDeviceHandle]      -- Device Handle
**
** Returns:     return from underlying call is bubbled up
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_DeviceStore_GetFirstDevice(
    __inout OEM_DEVICESTORE_CONTEXT *       f_pOemDevStoreContext,
    __out   OEM_DEVICE_HANDLE       * const f_phDeviceHandle
)
{
    return DRM_E_NOTIMPL;
}


/**********************************************************************
**
** Function:    DRM_DeviceStore_GetNextDevice
**
** Synopsis:    A wrapper for Oem_DeviceStore_GetNextDevice
**
** Arguments:   [f_pOemDevStoreContext] -- OEM device store context
**              [f_phDeviceHandle]      -- Device Handle
**
** Returns:     return from underlying call is bubbled up
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_DeviceStore_GetNextDevice(
    __inout OEM_DEVICESTORE_CONTEXT *       f_pOemDevStoreContext,
    __out   OEM_DEVICE_HANDLE       * const f_phDeviceHandle
)
{
    return DRM_E_NOTIMPL;
}


/**********************************************************************
**
** Function:    DRM_DeviceStore_GetDeviceByID
**
** Synopsis:    A wrapper for Oem_DeviceStore_GetDeviceByID
**
** Arguments:   [f_pOemDevStoreContext] -- OEM device store context
**              [f_DeviceCertificate]   -- Device Certificate
**              [f_DeviceSerialNumber]  -- Device Serial Number
**              [f_phDeviceHandle]      -- Device Handle
**
** Returns:     return from underlying call is bubbled up
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_DeviceStore_GetDeviceByID(
    __inout       OEM_DEVICESTORE_CONTEXT *       f_pOemDevStoreContext,
    __in const    DRM_BYTEBLOB                    f_DeviceCertificate,
    __in const    DRM_ID                          f_DeviceSerialNumber,
    __out         OEM_DEVICE_HANDLE       * const f_phDeviceHandle
)
{
    return DRM_E_NOTIMPL;
}


/**********************************************************************
**
** Function:    DRM_DeviceStore_GetDeviceInfo
**
** Synopsis:    The function extracts device certificate and serial number
**
** Arguments:   [f_hDeviceHandle]        -- Device Handle
**              [f_pbDeviceCertificate]  -- Pointer to Device Certificate; may be NULL
**              [f_pcbDeviceCertificate] -- Device Certificate Size
**              [f_pDeviceSerialNumber]  -- Device Serial Number
**
** Returns:     return from underlying call is bubbled up
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_DeviceStore_GetDeviceInfo(
    __in const                                OEM_DEVICE_HANDLE         f_hDeviceHandle,
    __out_bcount_opt(*f_pcbDeviceCertificate) DRM_BYTE          * const f_pbDeviceCertificate,
    __inout                                   DRM_DWORD         * const f_pcbDeviceCertificate,
    __out                                     DRM_ID            * const f_pDeviceSerialNumber
)
{
    return DRM_E_NOTIMPL;
}


/**********************************************************************
**
** Function:    DRM_DeviceStore_GetDeviceAttr
**
** Synopsis:    A wrapper for Oem_DeviceStore_GetDeviceAttr
**
** Arguments:   [f_hDeviceHandle] -- Device Handle
**              [f_AttrName]      -- Attribute Name
**              [f_pbAttrValue]   -- Pointer to Attribute Value; may be NULL
**              [f_pcbAttrValue]  -- Attribute Value Size
**
** Returns:     return from underlying call is bubbled up
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_DeviceStore_GetDeviceAttr(
    __in const                        OEM_DEVICE_HANDLE         f_hDeviceHandle,
    __in const                        DRM_BYTEBLOB              f_AttrName,
    __out_bcount_opt(*f_pcbAttrValue) DRM_BYTE          * const f_pbAttrValue,
    __inout                           DRM_DWORD         * const f_pcbAttrValue
)
{
    return DRM_E_NOTIMPL;
}


/**********************************************************************
**
** Function:    DRM_DeviceStore_GetDeviceAttrCount
**
** Synopsis:    A wrapper for Oem_DeviceStore_GetDeviceAttrCount
**
** Arguments:   [f_hDeviceHandle] -- Device Handle
**              [f_pdwAttrCount]  -- Number of Attributes
**
** Returns:     return from underlying call is bubbled up
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_DeviceStore_GetDeviceAttrCount(
    __in const  OEM_DEVICE_HANDLE            f_hDeviceHandle,
    __out       DRM_DWORD            * const f_pdwAttrCount
)
{
    return DRM_E_NOTIMPL;
}


/**********************************************************************
**
** Function:    DRM_DeviceStore_GetDeviceAttrByIndex
**
** Synopsis:    A wrapper for Oem_DeviceStore_GetDeviceAttrByIndex
**
** Arguments:   [f_hDeviceHandle] -- Device Handle
**              [f_dwAttrIndex]   -- Attribute Index
**              [f_pbAttrName]    -- Pointer to Attribute Name; may be NULL
**              [f_pcbAttrName]   -- Attribute Name Size
**              [f_pbAttrValue]   -- Pointer to Attribute Value; may be NULL
**              [f_pcbAttrValue]  -- Attribute Value Size
**
** Returns:     return from underlying call is bubbled up
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_DeviceStore_GetDeviceAttrByIndex(
    __in const                        OEM_DEVICE_HANDLE         f_hDeviceHandle,
    __in const                        DRM_DWORD                 f_dwAttrIndex,
    __out_bcount_opt(*f_pcbAttrName)  DRM_BYTE          * const f_pbAttrName,
    __inout                           DRM_DWORD         * const f_pcbAttrName,
    __out_bcount_opt(*f_pcbAttrValue) DRM_BYTE          * const f_pbAttrValue,
    __inout                           DRM_DWORD         * const f_pcbAttrValue
)
{
    return DRM_E_NOTIMPL;
}


/**********************************************************************
**
** Function:    DRM_DeviceStore_SetDeviceAttr
**
** Synopsis:    A wrapper for Oem_DeviceStore_SetDeviceAttr
**
** Arguments:   [f_hDeviceHandle] -- Device Handle
**              [f_dwAttrIndex]   -- Attribute Index
**              [f_AttrName]      -- Attribute Name
**              [f_AttrValue]     -- Attribute Value
**
** Returns:     return from underlying call is bubbled up
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_DeviceStore_SetDeviceAttr(
    __in const   OEM_DEVICE_HANDLE f_hDeviceHandle,
    __in const   DRM_BYTEBLOB      f_AttrName,
    __in const   DRM_BYTEBLOB      f_AttrValue
)
{
    return DRM_E_NOTIMPL;
}

EXIT_PK_NAMESPACE_CODE;

