/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmcommon.h>
#include <drmcontextsizes.h>
#include <oemdevstore.h>

ENTER_PK_NAMESPACE_CODE;


/**********************************************************************
** Function:    Oem_DeviceStore_Initialize
** Synopsis:    A wrapper for DRM_DVS_Initialize
** Arguments:   [f_pDeviceStoreContext]  -- Pointer to allocated context 
**              [f_pDeviceStoreInitData] -- Pointer to initialization data
***********************************************************************/
DRM_API DRM_RESULT DRM_CALL Oem_DeviceStore_Initialize(
    __inout_ecount(1)  OEM_DEVICESTORE_CONTEXT      *f_pDeviceStoreContext,
    __in_ecount(1)     OEM_DEVICESTORE_INITDATA     *f_pDeviceStoreInitData
)
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkDR( DRM_DVS_Initialize( (DRM_DEVICESTORE_CONTEXT *)f_pDeviceStoreContext, 
                                (DRM_APP_CONTEXT *)f_pDeviceStoreInitData ) );
ErrorExit:
    return dr;
}

/**********************************************************************
** Function:    Oem_DeviceStore_Uninitialize
** Synopsis:    A wrapper for DRM_DVS_Uninitialize
** Arguments:   [f_pDeviceStoreContext] -- Pointer to allocated and intialized context
***********************************************************************/
DRM_API DRM_VOID DRM_CALL Oem_DeviceStore_Uninitialize(
    __inout_ecount(1)   OEM_DEVICESTORE_CONTEXT    *f_pDeviceStoreContext
)
{
    DRM_DVS_Uninitialize( (DRM_DEVICESTORE_CONTEXT *)f_pDeviceStoreContext );
    return;
}
    

/**********************************************************************
** Function:    Oem_DeviceStore_FreeDevice
** Synopsis:    A wrapper for DRM_DVS_FreeDevice
** Arguments:   [f_hDeviceHandle] -- Device handle
***********************************************************************/
DRM_API DRM_VOID DRM_CALL Oem_DeviceStore_FreeDevice (
    __in     OEM_DEVICE_HANDLE            f_hDeviceHandle
)
{
    DRM_DVS_FreeDevice( (DRM_DEVICESTORE_HANDLE)f_hDeviceHandle );
    return;
}


/**********************************************************************
** Function:    Oem_DeviceStore_RegisterDevice
** Synopsis:    A wrapper for DRM_DVS_RegisterDevice
** Arguments:   [f_pDeviceStoreContext] -- Pointer to allocated context
                [f_DeviceID]            -- Device ID
                [f_phDeviceHandle]      -- New device handle
***********************************************************************/
DRM_API DRM_RESULT DRM_CALL Oem_DeviceStore_RegisterDevice(
    __inout_ecount(1) OEM_DEVICESTORE_CONTEXT      *f_pDeviceStoreContext,
    __in              DRM_BYTEBLOB                  f_DeviceID,
    __out_ecount(1)   OEM_DEVICE_HANDLE            *f_phDeviceHandle
)
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkDR( DRM_DVS_RegisterDevice( (DRM_DEVICESTORE_CONTEXT *)f_pDeviceStoreContext,
                                   f_DeviceID,
                                   (DRM_DEVICESTORE_HANDLE *)f_phDeviceHandle ) );

ErrorExit:
    return dr;
}


/**********************************************************************
** Function:    Oem_DeviceStore_UnRegisterDevice
** Synopsis:    A wrapper for DRM_DVS_UnRegisterDevice
** Arguments:   [f_pDeviceStoreContext] -- Pointer to context
**              [f_DeviceID]            -- Device ID to unregister
***********************************************************************/
DRM_API DRM_RESULT DRM_CALL Oem_DeviceStore_UnRegisterDevice(
    __inout_ecount(1) OEM_DEVICESTORE_CONTEXT      *f_pDeviceStoreContext,
    __in              DRM_BYTEBLOB                  f_DeviceID
)
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkDR( DRM_DVS_UnRegisterDevice( (DRM_DEVICESTORE_CONTEXT *)f_pDeviceStoreContext,
                                     f_DeviceID ) );

ErrorExit:
    return dr;
}


/**********************************************************************
** Function:    Oem_DeviceStore_GetFirstDevice
** Synopsis:    A wrapper for DRM_DVS_GetFirstDevice
** Arguments:   [f_pDeviceStoreContext] -- Pointer to allocated context
**              [f_phDeviceHandle]      -- Retreived device handle
***********************************************************************/
DRM_API DRM_RESULT DRM_CALL Oem_DeviceStore_GetFirstDevice(
    __inout_ecount(1) OEM_DEVICESTORE_CONTEXT      *f_pDeviceStoreContext,
    __out_ecount(1)   OEM_DEVICE_HANDLE            *f_phDeviceHandle
)
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkDR( DRM_DVS_GetFirstDevice( (DRM_DEVICESTORE_CONTEXT *)f_pDeviceStoreContext,
                                    (DRM_DEVICESTORE_HANDLE *)f_phDeviceHandle ) );

ErrorExit:
    return dr;
}


/**********************************************************************
** Function:    Oem_DeviceStore_GetNextDevice
** Synopsis:    A wrapper for DRM_DVS_GetNextDevice
** Arguments:   [f_pDeviceStoreContext] -- Pointer to allocated context
**              [f_phDeviceHandle]      -- Current (in) and next (out) Device handle
***********************************************************************/
DRM_API DRM_RESULT DRM_CALL Oem_DeviceStore_GetNextDevice(
    __in_ecount(1)    OEM_DEVICESTORE_CONTEXT      *f_pDeviceStoreContext,
    __inout_ecount(1) OEM_DEVICE_HANDLE            *f_phDeviceHandle
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    
    ChkDR( DRM_DVS_GetNextDevice( (DRM_DEVICESTORE_CONTEXT *)f_pDeviceStoreContext,
                                   (DRM_DEVICESTORE_HANDLE *)f_phDeviceHandle ) );

ErrorExit:
    return dr;
}


/**********************************************************************
** Function:    Oem_DeviceStore_GetDeviceByID
** Synopsis:    A wrapper for DRM_DVS_GetDeviceByID
** Arguments:   [f_pDeviceStoreContext] -- Pointer to context
**              [f_DeviceID]            -- Device ID to retreive
**              [f_phDeviceHandle]      -- Returned device handle
************************************************************************/
DRM_API DRM_RESULT DRM_CALL Oem_DeviceStore_GetDeviceByID(
    __inout_ecount(1) OEM_DEVICESTORE_CONTEXT      *f_pDeviceStoreContext,
    __in              DRM_BYTEBLOB                  f_DeviceID,
    __out_ecount(1)   OEM_DEVICE_HANDLE            *f_phDeviceHandle
)
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkDR( DRM_DVS_GetDeviceByID( (DRM_DEVICESTORE_CONTEXT *)f_pDeviceStoreContext,
                                  f_DeviceID,
                                  (DRM_DEVICESTORE_HANDLE *)f_phDeviceHandle ) );
ErrorExit:
    return dr;
}


/**********************************************************************
** Function:    Oem_DeviceStore_GetDeviceID
** Synopsis:    A wrapper for DRM_DVS_GetDeviceID
** Arguments:   [f_hDeviceHandle] -- device handle
**              [f_pbDeviceID]    -- Buffer to hold device ID
**              [f_pcbDeviceID]   -- Size of input buffer and resulting size of device ID
***********************************************************************/
DRM_API DRM_RESULT DRM_CALL Oem_DeviceStore_GetDeviceID(
    __in                           OEM_DEVICE_HANDLE   f_hDeviceHandle,
    __out_bcount(*f_pcbDeviceID)   DRM_BYTE           *f_pbDeviceID,
    __inout_ecount(1)              DRM_DWORD          *f_pcbDeviceID
)
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkDR( DRM_DVS_GetDeviceID( (DRM_DEVICESTORE_HANDLE)f_hDeviceHandle,
                                f_pbDeviceID,
                                f_pcbDeviceID ) );
    
ErrorExit:
    return dr;
}


/**********************************************************************
** Function:    Oem_DeviceStore_GetDeviceAttr
** Synopsis:    A wrapper for DRM_DVS_GetDeviceAttr
** Arguments:   [f_hDeviceHandle] -- device handle
**              [f_AttrName]      -- Attribute name
**              [f_pbAttrValue]   -- Buffer to hold attribute values
**              [f_pcbAttrValue]  -- Size of input buffer and resulting length of output
***********************************************************************/
DRM_API DRM_RESULT DRM_CALL Oem_DeviceStore_GetDeviceAttr(
    __in                           OEM_DEVICE_HANDLE   f_hDeviceHandle,
    __in                           DRM_BYTEBLOB        f_AttrName,
    __out_bcount(*f_pcbAttrValue)  DRM_BYTE           *f_pbAttrValue,
    __inout_ecount(1)              DRM_DWORD          *f_pcbAttrValue
)
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkDR( DRM_DVS_GetDeviceAttr( (DRM_DEVICESTORE_HANDLE)f_hDeviceHandle,
                                   f_AttrName,
                                   f_pbAttrValue,
                                   f_pcbAttrValue ) );
    
 ErrorExit:    
    return dr;
}


/**********************************************************************
** Function:    Oem_DeviceStore_GetDeviceAttrCount
** Synopsis:    a wrapper for DRM_DVS_GetAttrCount
** Arguments:   [f_hDeviceHandle]       --  Device handle
**              [f_pdwAttrCount]        --  Number of attributes
***********************************************************************/
DRM_API DRM_RESULT DRM_CALL Oem_DeviceStore_GetDeviceAttrCount(
    __in            OEM_DEVICE_HANDLE            f_hDeviceHandle,
    __out_ecount(1) DRM_DWORD                   *f_pdwAttrCount
)
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkDR( DRM_DVS_GetDeviceAttrCount( (DRM_DEVICESTORE_HANDLE)f_hDeviceHandle, 
                                        f_pdwAttrCount ) );

ErrorExit:
    return dr;
}


/**********************************************************************
** Function:    Oem_DeviceStore_GetDeviceAttrByIndex
** Synopsis:    A wrapper for DRM_DVS_GetDeviceAttrByIndex
** Arguments:   [f_hDeviceHandle]      --  Device handle
**              [f_dwAttrIndex]        --  Attribute index
**              [f_pbAttrName]         --  Buffer to attribute name
**              [f_pcbAttrName]        --  Size of input buffer (in)
**                                         Length of attribute name (out)
**              [f_pbAttrValue]        --  Buffer for attribute value
**              [f_pcbAttrValue]       --  Size of input buffer (in)
**                                         Length of attribute value (out)
***********************************************************************/
DRM_API DRM_RESULT DRM_CALL Oem_DeviceStore_GetDeviceAttrByIndex(
    __in                            OEM_DEVICE_HANDLE  f_hDeviceHandle,
    __in                            DRM_DWORD          f_dwAttrIndex,
    __inout_bcount(*f_pcbAttrName)  DRM_BYTE          *f_pbAttrName,
    __inout_ecount(1)               DRM_DWORD         *f_pcbAttrName,
    __inout_bcount(*f_pcbAttrValue) DRM_BYTE          *f_pbAttrValue,
    __inout_ecount(1)               DRM_DWORD         *f_pcbAttrValue
)
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkDR( DRM_DVS_GetDeviceAttrByIndex( (DRM_DEVICESTORE_HANDLE)f_hDeviceHandle,
                                          f_dwAttrIndex,
                                          f_pbAttrName,
                                          f_pcbAttrName,
                                          f_pbAttrValue,
                                          f_pcbAttrValue ) );

ErrorExit:
    return dr;
}


/**********************************************************************
** Function:    Oem_DeviceStore_SetDeviceAttr
** Synopsis:    A wrapper for DRM_DVS_SetDeviceAttr
** Arguments:   [f_hDeviceHandle]       --  Device handle
**              [f_AttrName]            --  Attribute name
**              [f_AttrValue]           --  Attribute value
***********************************************************************/
DRM_API DRM_RESULT DRM_CALL Oem_DeviceStore_SetDeviceAttr(
    __in     OEM_DEVICE_HANDLE             f_hDeviceHandle,
    __in     DRM_BYTEBLOB                  f_AttrName,
    __in     DRM_BYTEBLOB                  f_AttrValue
)
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkDR( DRM_DVS_SetDeviceAttr( (DRM_DEVICESTORE_HANDLE)f_hDeviceHandle,
                                   f_AttrName,
                                   f_AttrValue ) );

ErrorExit:
    return dr;
}


/**********************************************************************
** Function:    Oem_DeviceStore_CommitDevice
** Synopsis:    A wrapper for DRM_DVS_CommitDevice
** Arguments:   [f_hDeviceHandle]      --  Device handle
***********************************************************************/
DRM_API DRM_RESULT DRM_CALL Oem_DeviceStore_CommitDevice(
    __in     OEM_DEVICE_HANDLE            f_hDeviceHandle
)
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkDR( DRM_DVS_CommitDevice( (DRM_DEVICESTORE_HANDLE)f_hDeviceHandle ) );

 ErrorExit:   
    return dr;
}


EXIT_PK_NAMESPACE_CODE;
