/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_OEMDEVSTORE_C
#include <drmcontextsizes.h>
#include <oemdevstore.h>
#include <drmlastinclude.h>

ENTER_PK_NAMESPACE_CODE;

PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_NONCONST_PARAM_25004, "Changing parameters to const to satisfy warning would make them not match the standard OEM interface.")

struct __tagINMEMORY_OEMDEVICEATTRINTERNAL;
struct __tagINMEMORY_OEM_DEVICESTORE_CONTEXTINTERNAL;
struct __tagINMEMORY_OEMDEVICEINTERNAL;

typedef struct __tagINMEMORY_OEMDEVICEATTRINTERNAL
{
    struct __tagINMEMORY_OEMDEVICEATTRINTERNAL  *nextAttr;
    DRM_BYTEBLOB                                 attrName;
    DRM_BYTEBLOB                                 attrValue;
} INMEMORY_OEMDEVICEATTRINTERNAL;



typedef struct __tagINMEMORY_OEM_DEVICESTORE_CONTEXTINTERNAL
{
    struct __tagINMEMORY_OEMDEVICEINTERNAL      *firstDevice;
} INMEMORY_OEM_DEVICESTORE_CONTEXTINTERNAL;



typedef struct __tagINMEMORY_OEMDEVICEINTERNAL
{
    INMEMORY_OEM_DEVICESTORE_CONTEXTINTERNAL    *pContext;
    INMEMORY_OEMDEVICEATTRINTERNAL              *firstAttr;
    struct __tagINMEMORY_OEMDEVICEINTERNAL      *nextDevice;
    DRM_BYTEBLOB                                 deviceID;
    DRM_DWORD                                    dwRefCount;
    DRM_BOOL                                     fRegistered;
} INMEMORY_OEMDEVICEINTERNAL;



#define ChkDeviceHandle(h) ChkArg(h != NULL)
#define DEVICE_FROM_HANDLE(h) ((INMEMORY_OEMDEVICEINTERNAL *)h)
#define CONST_DEVICE_FROM_HANDLE(h) ((const INMEMORY_OEMDEVICEINTERNAL *)h)

/* Reference counting macros */
#define ADD_REFERENCE(h) (DEVICE_FROM_HANDLE(h))->dwRefCount++;

#define DELETE_REFERENCE(h) \
    DRMASSERT((DEVICE_FROM_HANDLE(h))->dwRefCount > 0); \
    (DEVICE_FROM_HANDLE(h))->dwRefCount--;


static DRM_RESULT _DestroyDevice(
    __in_ecount(1) INMEMORY_OEMDEVICEINTERNAL *f_pDeviceHandle
)
{
    DRM_RESULT                      dr        = DRM_SUCCESS;
    INMEMORY_OEMDEVICEATTRINTERNAL* pAttr     = NULL;
    INMEMORY_OEMDEVICEATTRINTERNAL* pNextAttr = NULL;
    INMEMORY_OEMDEVICEINTERNAL*     pDevice   = f_pDeviceHandle;
    INMEMORY_OEMDEVICEINTERNAL**    ppDevice  = NULL;

    /* Find reference to the device */
    for(ppDevice = &pDevice->pContext->firstDevice;
        ppDevice != NULL && *ppDevice != pDevice;
        ppDevice = &((*ppDevice)->nextDevice));

    /* Ensure the device is in the list */
    if(!*ppDevice)
    {
        ChkDR(DRM_E_INVALIDARG);
    }

    /* Iterate and free all attributes */
    for(pAttr = f_pDeviceHandle->firstAttr; pAttr; )
    {
        pNextAttr = pAttr->nextAttr;
        Oem_MemFree(pAttr);
        pAttr = pNextAttr;
    }
    Oem_MemFree( f_pDeviceHandle->deviceID.pbBlob );
    f_pDeviceHandle->deviceID.pbBlob = NULL;

    /* Replace reference to device with next in list */
    *ppDevice = pDevice->nextDevice;

    /* Release memory */
    Oem_MemFree(pDevice);

ErrorExit:
    return dr;
}


/**********************************************************************
** Function:    Oem_DeviceStore_Initialize
** Synopsis:    Initializes the OEM device store
** Arguments:   [f_pDeviceStoreContext]  -- Pointer to allocated context
**              [f_pDeviceStoreInitData] -- Pointer to initialization data
***********************************************************************/
DRM_API DRM_RESULT DRM_CALL Oem_DeviceStore_Initialize(
    __inout_ecount(1)  OEM_DEVICESTORE_CONTEXT    *f_pDeviceStoreContext,
    __inout_ecount(1)  OEM_DEVICESTORE_INITDATA   *f_pDeviceStoreInitData
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    INMEMORY_OEM_DEVICESTORE_CONTEXTINTERNAL *pInMemoryDevStoreContext = (INMEMORY_OEM_DEVICESTORE_CONTEXTINTERNAL *)f_pDeviceStoreContext;

    ChkArg( f_pDeviceStoreContext  != NULL );
    ChkArg( f_pDeviceStoreInitData != NULL );

    ZEROMEM( f_pDeviceStoreContext, SIZEOF( INMEMORY_OEM_DEVICESTORE_CONTEXTINTERNAL ) );
    pInMemoryDevStoreContext->firstDevice = NULL;
ErrorExit:
    return dr;
}


/**********************************************************************
** Function:    Oem_DeviceStore_Uninitialize
** Synopsis:    Uninitializes the OEM device store
** Arguments:   [f_pDeviceStoreContext] -- Pointer to allocated and intialized context
***********************************************************************/
DRM_API void DRM_CALL Oem_DeviceStore_Uninitialize(
    __inout_ecount(1)   OEM_DEVICESTORE_CONTEXT    *f_pDeviceStoreContext
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    INMEMORY_OEM_DEVICESTORE_CONTEXTINTERNAL *pInMemoryDevStoreContext = (INMEMORY_OEM_DEVICESTORE_CONTEXTINTERNAL *)f_pDeviceStoreContext;
    INMEMORY_OEMDEVICEINTERNAL *pDevice, *pNextDevice;

    ChkArg( f_pDeviceStoreContext != NULL );

    /* Iterate and free all devices */
    for(pDevice = pInMemoryDevStoreContext->firstDevice; pDevice; )
    {
        pNextDevice = pDevice->nextDevice;
        _DestroyDevice( pDevice );  /* this frees pDevice */
        pDevice = pNextDevice;
    }

ErrorExit:
    return;
}


/**********************************************************************
** Function:    Oem_DeviceStore_FreeDevice
** Synopsis:    Commits any "dirty" attributes and frees the device handle
** Arguments:   [f_hDeviceHandle] -- Device handle
***********************************************************************/
DRM_API DRM_VOID DRM_CALL Oem_DeviceStore_FreeDevice (
    __in     OEM_DEVICE_HANDLE            f_hDeviceHandle
)
{
    INMEMORY_OEMDEVICEINTERNAL *pDevice;

    if (f_hDeviceHandle != NULL)
    {
        pDevice = DEVICE_FROM_HANDLE(f_hDeviceHandle);

        DELETE_REFERENCE( f_hDeviceHandle );
        if(pDevice->fRegistered == FALSE && pDevice->dwRefCount == 0)
        {
            /* We've been unregistered and our reference count is zero, free it */
            _DestroyDevice( pDevice );
        }
    }
}


/**********************************************************************
** Function:    Oem_DeviceStore_RegisterDevice
** Synopsis:    Registers a device
** Arguments:   [f_pDeviceStoreContext] -- Pointer to allocated context
**              [f_DeviceID]            -- Device ID
**              [f_pDeviceHandle]       -- New device handle
***********************************************************************/
DRM_API DRM_RESULT DRM_CALL Oem_DeviceStore_RegisterDevice(
    __inout_ecount(1) OEM_DEVICESTORE_CONTEXT      *f_pDeviceStoreContext,
    __in              DRM_BYTEBLOB                  f_DeviceID,
    __out_ecount  (1) OEM_DEVICE_HANDLE            *f_pDeviceHandle
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    INMEMORY_OEM_DEVICESTORE_CONTEXTINTERNAL *pInMemoryDevStoreContext;
    INMEMORY_OEMDEVICEINTERNAL **ppNextDevice;
    INMEMORY_OEMDEVICEINTERNAL *pDevice = NULL;

    ChkArg( f_pDeviceStoreContext != NULL );
    ChkArg( f_DeviceID.cbBlob     != 0    );
    ChkArg( f_DeviceID.pbBlob     != NULL );
    ChkArg( f_pDeviceHandle       != NULL );

    pInMemoryDevStoreContext = (INMEMORY_OEM_DEVICESTORE_CONTEXTINTERNAL *)f_pDeviceStoreContext;

    for(ppNextDevice   = &pInMemoryDevStoreContext->firstDevice;
        *ppNextDevice != NULL;
        ppNextDevice   = &(*ppNextDevice)->nextDevice);

    ChkMem( pDevice = (INMEMORY_OEMDEVICEINTERNAL *)Oem_MemAlloc( SIZEOF( INMEMORY_OEMDEVICEINTERNAL ) ) );
    ZEROMEM( pDevice, SIZEOF( INMEMORY_OEMDEVICEINTERNAL ) );

    pDevice->pContext = pInMemoryDevStoreContext;
    /* Copy the device ID */
    ChkMem( pDevice->deviceID.pbBlob = (DRM_BYTE *)Oem_MemAlloc( f_DeviceID.cbBlob ) );
    MEMCPY( pDevice->deviceID.pbBlob, f_DeviceID.pbBlob, f_DeviceID.cbBlob );
    pDevice->deviceID.cbBlob = f_DeviceID.cbBlob;

    /* Initialize the other members */
    pDevice->fRegistered = TRUE;

        /* Reference count is incremented because we're returning the handle. */
    ADD_REFERENCE( pDevice );

    *ppNextDevice    = pDevice;
    *f_pDeviceHandle = pDevice;

ErrorExit:

    if( DRM_FAILED( dr ) )
    {
        if(pDevice)
        {
            if(pDevice->deviceID.pbBlob)
            {
                Oem_MemFree(pDevice->deviceID.pbBlob);
            }
            Oem_MemFree(pDevice);
        }
    }
    return dr;
}


/**********************************************************************
** Function:    Oem_DeviceStore_UnRegisterDevice
** Synopsis:    Unregisters a device
** Arguments:   [f_pDeviceStoreContext] -- Pointer to context
**              [f_DeviceID]            -- Device ID to unregister
***********************************************************************/
DRM_API DRM_RESULT DRM_CALL Oem_DeviceStore_UnRegisterDevice(
    __inout_ecount(1) OEM_DEVICESTORE_CONTEXT      *f_pDeviceStoreContext,
    __in              DRM_BYTEBLOB                  f_DeviceID
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    INMEMORY_OEMDEVICEINTERNAL *pDevice;

    ChkDR( Oem_DeviceStore_GetDeviceByID( f_pDeviceStoreContext,
                                          f_DeviceID,
                                          (OEM_DEVICE_HANDLE *)&pDevice ) );

    DRMASSERT( pDevice != NULL );
    __analysis_assume( pDevice != NULL );

    pDevice->fRegistered = FALSE;

    Oem_DeviceStore_FreeDevice( (OEM_DEVICE_HANDLE) pDevice );

ErrorExit:
    return dr;
}


/**********************************************************************
** Function:    Oem_DeviceStore_GetFirstDevice
** Synopsis:    Retreives the first device from the store
** Arguments:   [f_pDeviceStoreContext] -- Pointer to allocated context
**              [f_pDeviceHandle]       -- Retreived device handle
***********************************************************************/
DRM_API DRM_RESULT DRM_CALL Oem_DeviceStore_GetFirstDevice(
    __inout_ecount(1) OEM_DEVICESTORE_CONTEXT      *f_pDeviceStoreContext,
    __out_ecount  (1) OEM_DEVICE_HANDLE            *f_pDeviceHandle
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    INMEMORY_OEM_DEVICESTORE_CONTEXTINTERNAL *pPFDDevStoreContext;

    ChkArg( f_pDeviceStoreContext != NULL );
    ChkArg( f_pDeviceHandle       != NULL );

    pPFDDevStoreContext = (INMEMORY_OEM_DEVICESTORE_CONTEXTINTERNAL *)f_pDeviceStoreContext;
    *f_pDeviceHandle = pPFDDevStoreContext->firstDevice;
    ChkBOOL( *f_pDeviceHandle != NULL, DRM_E_NOMORE );
    ADD_REFERENCE( *f_pDeviceHandle );

ErrorExit:
    return dr;
}


/**********************************************************************
** Function:    Oem_DeviceStore_GetNextDevice
** Synopsis:    Retreives the next device from the store
** Arguments:   [f_pDeviceStoreContext] -- Pointer to allocated context
**              [f_pDeviceHandle]       -- Current (in) and next (out) Device handle
***********************************************************************/
DRM_API DRM_RESULT DRM_CALL Oem_DeviceStore_GetNextDevice(
    __in_ecount(1)    OEM_DEVICESTORE_CONTEXT    *f_pDeviceStoreContext,
    __inout_ecount(1) OEM_DEVICE_HANDLE          *f_pDeviceHandle
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    INMEMORY_OEMDEVICEINTERNAL *pDevice;

    ChkArg( f_pDeviceStoreContext != NULL );
    ChkArg( f_pDeviceHandle       != NULL );

    pDevice = *((INMEMORY_OEMDEVICEINTERNAL **)f_pDeviceHandle);
    ChkArg( pDevice != NULL );

    *f_pDeviceHandle = (OEM_DEVICE_HANDLE)pDevice->nextDevice;
    ChkBOOL( *f_pDeviceHandle != NULL, DRM_E_NOMORE );
    ADD_REFERENCE( *f_pDeviceHandle );

ErrorExit:
    return dr;
}


/**********************************************************************
** Function:    Oem_DeviceStore_GetDeviceByID
** Synopsis:    Retreives a device through the device ID
** Arguments:   [f_pDeviceStoreContext] -- Pointer to context
**              [f_DeviceID]            -- Device ID to retreive
**              [f_pDeviceHandle]       -- Returned device handle
***********************************************************************/
DRM_API DRM_RESULT DRM_CALL Oem_DeviceStore_GetDeviceByID(
    __inout_ecount(1) OEM_DEVICESTORE_CONTEXT      *f_pDeviceStoreContext,
    __in              DRM_BYTEBLOB                  f_DeviceID,
    __out_ecount  (1) OEM_DEVICE_HANDLE            *f_pDeviceHandle
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    INMEMORY_OEM_DEVICESTORE_CONTEXTINTERNAL *pPFDDevStoreContext;
    INMEMORY_OEMDEVICEINTERNAL *pDevice;

    ChkArg( f_pDeviceStoreContext != NULL );
    ChkArg( f_DeviceID.pbBlob     != NULL );
    ChkArg( f_DeviceID.cbBlob     != 0    );
    ChkArg( f_pDeviceHandle       != NULL );

    pPFDDevStoreContext = (INMEMORY_OEM_DEVICESTORE_CONTEXTINTERNAL *)f_pDeviceStoreContext;

    for(pDevice = pPFDDevStoreContext->firstDevice; pDevice; pDevice = pDevice->nextDevice)
    {
        if(pDevice->fRegistered == FALSE)
        {
            continue;
        }
        if(f_DeviceID.cbBlob == pDevice->deviceID.cbBlob &&
           (MEMCMP (f_DeviceID.pbBlob, pDevice->deviceID.pbBlob, f_DeviceID.cbBlob) == 0))
        {
            break;
        }
    }
    ChkBOOL( pDevice != NULL, DRM_E_DEVICE_NOT_REGISTERED );

    /* We found it; set our return value. */
    *f_pDeviceHandle = (OEM_DEVICE_HANDLE)pDevice;
    ADD_REFERENCE( *f_pDeviceHandle );

ErrorExit:
    return dr;
}


/**********************************************************************
** Function:    Oem_DeviceStore_GetDeviceID
** Synopsis:    Retreives the device ID given a device handle
** Arguments:   [f_pDeviceHandle] -- device handle
**              [f_pbDeviceID]    -- Buffer to hold device ID
**              [f_pcbDeviceID]   -- Size of input buffer and resulting size of device ID
***********************************************************************/
DRM_API DRM_RESULT DRM_CALL Oem_DeviceStore_GetDeviceID(
    __in                             OEM_DEVICE_HANDLE    f_pDeviceHandle,
    __out_ecount_opt(*f_pcbDeviceID) DRM_BYTE            *f_pbDeviceID,
    __inout_ecount(1)                DRM_DWORD           *f_pcbDeviceID
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    const INMEMORY_OEMDEVICEINTERNAL *pDevice;

    ChkDeviceHandle(f_pDeviceHandle);
    ChkArg( f_pcbDeviceID   != NULL &&
            (*f_pcbDeviceID == 0 || f_pbDeviceID != NULL) );

    pDevice = CONST_DEVICE_FROM_HANDLE( f_pDeviceHandle );
    if(*f_pcbDeviceID < pDevice->deviceID.cbBlob)
    {
        *f_pcbDeviceID = pDevice->deviceID.cbBlob;
        ChkDR( DRM_E_BUFFERTOOSMALL );
    }

    MEMCPY( f_pbDeviceID, pDevice->deviceID.pbBlob, pDevice->deviceID.cbBlob );
    *f_pcbDeviceID = pDevice->deviceID.cbBlob;

ErrorExit:
    return dr;
}


/**********************************************************************
** Function:    Oem_DeviceStore_GetDeviceAttr
** Synopsis:    Retreives device attribute values
** Arguments:   [f_pDeviceHandle] -- device handle
**              [f_AttrName]      -- Attribute name
**              [f_pbAttrValue]   -- Buffer to hold attribute values
**              [f_pcbAttrValue]  -- Size of input buffer and resulting length of output
***********************************************************************/
DRM_API DRM_RESULT DRM_CALL Oem_DeviceStore_GetDeviceAttr(
    __in                              OEM_DEVICE_HANDLE  f_pDeviceHandle,
    __in                              DRM_BYTEBLOB       f_AttrName,
    __out_bcount_opt(*f_pcbAttrValue) DRM_BYTE          *f_pbAttrValue,
    __inout_ecount(1)                 DRM_DWORD         *f_pcbAttrValue
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    const INMEMORY_OEMDEVICEINTERNAL *pDevice;
    const INMEMORY_OEMDEVICEATTRINTERNAL *pAttr;

    ChkDeviceHandle(f_pDeviceHandle);
    ChkArg( f_AttrName.pbBlob &&
            f_AttrName.cbBlob &&
            f_pcbAttrValue &&
            (*f_pcbAttrValue == 0 || f_pbAttrValue != NULL ) );

    pDevice = CONST_DEVICE_FROM_HANDLE( f_pDeviceHandle );
    for(pAttr = pDevice->firstAttr; pAttr; pAttr = pAttr->nextAttr)
    {
        if(pAttr->attrName.cbBlob == f_AttrName.cbBlob &&
           (MEMCMP(pAttr->attrName.pbBlob, f_AttrName.pbBlob, f_AttrName.cbBlob) == 0))
        {
            break;
        }
    }
    ChkBOOL( pAttr != NULL, DRM_E_NOMORE );

    if(*f_pcbAttrValue < pAttr->attrValue.cbBlob)
    {
        *f_pcbAttrValue = pAttr->attrValue.cbBlob;
        ChkDR( DRM_E_BUFFERTOOSMALL );
    }
    ChkArg( f_pbAttrValue != NULL );
    MEMCPY( f_pbAttrValue, pAttr->attrValue.pbBlob, pAttr->attrValue.cbBlob );
    *f_pcbAttrValue = pAttr->attrValue.cbBlob;

ErrorExit:
    return dr;
}


/**********************************************************************
** Function:    Oem_DeviceStore_GetDeviceAttrCount
** Synopsis:    Retreives the number of attributes for a given device
** Arguments:   [f_pDeviceHandle]      --  Device handle
**              [f_pdwAttrCount]       --  Number of attributes
***********************************************************************/
DRM_API DRM_RESULT DRM_CALL Oem_DeviceStore_GetDeviceAttrCount(
    __in             OEM_DEVICE_HANDLE  f_pDeviceHandle,
    __out_ecount(1)  DRM_DWORD         *f_pdwAttrCount
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    const INMEMORY_OEMDEVICEINTERNAL *pDevice;
    const INMEMORY_OEMDEVICEATTRINTERNAL *pAttr;
    DRM_DWORD dwCount = 0;

    ChkDeviceHandle( f_pDeviceHandle );
    ChkArg( f_pdwAttrCount );

    pDevice = CONST_DEVICE_FROM_HANDLE(f_pDeviceHandle);
    for(pAttr = pDevice->firstAttr; pAttr; pAttr = pAttr->nextAttr)
    {
        dwCount++;
    }
    *f_pdwAttrCount = dwCount;

ErrorExit:
    return dr;
}


/**********************************************************************
** Function:    Oem_DeviceStore_GetDeviceAttrByIndex
** Synopsis:    Retreives the device attribute and values given an index
** Arguments:   [f_pDeviceHandle]      --  Device handle
**              [f_dwAttrIndex]        --  Attribute index
**              [f_pbAttrName]         --  Buffer to attribute name
**              [f_pcbAttrName]        --  Size of input buffer (in)
**                                         Length of attribute name (out)
**              [f_pbAttrValue]        --  Buffer for attribute value
**              [f_pcbAttrValue]       --  Size of input buffer (in)
**                                         Length of attribute value (out)
***********************************************************************/
DRM_API DRM_RESULT DRM_CALL Oem_DeviceStore_GetDeviceAttrByIndex(
    __in                                OEM_DEVICE_HANDLE    f_pDeviceHandle,
    __in                                DRM_DWORD            f_dwAttrIndex,
    __inout_bcount_opt(*f_pcbAttrName)  DRM_BYTE            *f_pbAttrName,
    __inout_ecount(1)                   DRM_DWORD           *f_pcbAttrName,
    __inout_bcount_opt(*f_pcbAttrValue) DRM_BYTE            *f_pbAttrValue,
    __inout_ecount(1)                   DRM_DWORD           *f_pcbAttrValue
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    const INMEMORY_OEMDEVICEINTERNAL *pDevice;
    const INMEMORY_OEMDEVICEATTRINTERNAL *pAttr;
    DRM_DWORD dwCount = 0;

    ChkDeviceHandle( f_pDeviceHandle );
    ChkArg( f_pcbAttrName != NULL );
    ChkArg( *f_pcbAttrName == 0 || f_pbAttrName != NULL);
    ChkArg( f_pcbAttrValue != NULL );
    ChkArg( *f_pcbAttrValue == 0 || f_pbAttrValue != NULL);

    pDevice = CONST_DEVICE_FROM_HANDLE(f_pDeviceHandle);
    for(pAttr = pDevice->firstAttr; pAttr; pAttr = pAttr->nextAttr)
    {
        if(dwCount == f_dwAttrIndex)
        {
            break;
        }
        dwCount++;
    }
    ChkBOOL( pAttr != NULL, DRM_E_NOMORE );

    if(*f_pcbAttrName < pAttr->attrName.cbBlob)
    {
        dr = DRM_E_BUFFERTOOSMALL;
    }
    *f_pcbAttrName = pAttr->attrName.cbBlob;
    if(*f_pcbAttrValue < pAttr->attrValue.cbBlob)
    {
        dr = DRM_E_BUFFERTOOSMALL;
    }
    *f_pcbAttrValue = pAttr->attrValue.cbBlob;
    if(DRM_FAILED(dr))
    {
        goto ErrorExit;
    }
    MEMCPY( f_pbAttrName, pAttr->attrName.pbBlob, pAttr->attrName.cbBlob );
    MEMCPY( f_pbAttrValue, pAttr->attrValue.pbBlob, pAttr->attrValue.cbBlob );

ErrorExit:
    return dr;
}


/**********************************************************************
** Function:    Oem_DeviceStore_SetDeviceAttr
** Synopsis:    Sets a device attribute value
** Arguments:   [f_pDeviceHandle]      --  Device handle
**              [f_AttrName]           --  Attribute name
**              [f_AttrValue]          --  Attribute value
***********************************************************************/
DRM_API DRM_RESULT DRM_CALL Oem_DeviceStore_SetDeviceAttr(
    __in     OEM_DEVICE_HANDLE f_pDeviceHandle,
    __in     DRM_BYTEBLOB      f_AttrName,
    __in     DRM_BYTEBLOB      f_AttrValue
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    INMEMORY_OEMDEVICEATTRINTERNAL *pAttr = NULL, **ppAttr;
    INMEMORY_OEMDEVICEINTERNAL *pDevice;
    DRM_BYTE *pbValue = NULL;

    ChkDeviceHandle( f_pDeviceHandle );
    pDevice = DEVICE_FROM_HANDLE( f_pDeviceHandle );
    ChkMem( pbValue = (DRM_BYTE *)Oem_MemAlloc( f_AttrValue.cbBlob ) );

    for(ppAttr = &(pDevice->firstAttr); *ppAttr; ppAttr = &((*ppAttr)->nextAttr))
    {
        if((*ppAttr)->attrName.cbBlob == f_AttrName.cbBlob &&
           MEMCMP( (*ppAttr)->attrName.pbBlob, f_AttrName.pbBlob, f_AttrName.cbBlob) == 0)
        {
            break;
        }
    }
    if(*ppAttr)
    {
        Oem_MemFree( (*ppAttr)->attrValue.pbBlob );
        (*ppAttr)->attrValue.pbBlob = NULL;
    }
    else
    {
        ChkMem( pAttr = (INMEMORY_OEMDEVICEATTRINTERNAL *)Oem_MemAlloc( SIZEOF( INMEMORY_OEMDEVICEATTRINTERNAL ) ) );
        pAttr->nextAttr = NULL;
        pAttr->attrName.pbBlob = NULL;
        pAttr->attrValue.pbBlob = NULL;
        ChkMem( pAttr->attrName.pbBlob = (DRM_BYTE *)Oem_MemAlloc( f_AttrName.cbBlob ) );
        MEMCPY( pAttr->attrName.pbBlob, f_AttrName.pbBlob, f_AttrName.cbBlob );
        pAttr->attrName.cbBlob = f_AttrName.cbBlob;
        *ppAttr = pAttr;
        pAttr = NULL;
    }
    MEMCPY(pbValue, f_AttrValue.pbBlob, f_AttrValue.cbBlob);
    (*ppAttr)->attrValue.pbBlob = pbValue;
    (*ppAttr)->attrValue.cbBlob = f_AttrValue.cbBlob;

ErrorExit:
    if ( DRM_FAILED( dr ) )
    {
        if ( pAttr )
        {
            if ( pAttr->attrName.pbBlob )
            {
                Oem_MemFree( pAttr->attrName.pbBlob );
            }
            Oem_MemFree( pAttr );
        }
    }
    return dr;
}


/**********************************************************************
** Function:    Oem_DeviceStore_CommitDevice
** Synopsis:    Commits a device to persisent storage
** Arguments:   [f_pDeviceHandle]      --  Device handle
***********************************************************************/
DRM_API DRM_RESULT DRM_CALL Oem_DeviceStore_CommitDevice(
    __in     OEM_DEVICE_HANDLE            f_pDeviceHandle
)
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkDeviceHandle( f_pDeviceHandle );

ErrorExit:
    return dr;
}

PREFAST_POP /* Changing parameters to const to satisfy warning would make them not match the standard OEM interface. */

EXIT_PK_NAMESPACE_CODE;
