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

static DRM_RESULT _GenerateDeviceID(
    __out      DRM_BYTEBLOB * const f_pDeviceID,
    __in const DRM_BYTEBLOB         f_DeviceCertificate,
    __in const DRM_ID               f_DeviceSerialNumber
)
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DEVICESTORE, PERF_FUNC__GenerateDeviceID );

    ChkArg(f_pDeviceID
           && f_DeviceCertificate.pbBlob);

    /* DeviceID is the serial number concatenated with the certificate */
    ChkOverflow( SIZEOF(DRM_ID) + f_DeviceCertificate.cbBlob, SIZEOF(DRM_ID) );
    f_pDeviceID->cbBlob = SIZEOF(DRM_ID) + f_DeviceCertificate.cbBlob;
    f_pDeviceID->pbBlob = (DRM_BYTE *) Oem_MemAlloc(f_pDeviceID->cbBlob);
    ChkMem(f_pDeviceID->pbBlob);
    DRM_BYT_CopyBytes(f_pDeviceID->pbBlob, 0, &f_DeviceSerialNumber, 0, SIZEOF(DRM_ID));
    DRM_BYT_CopyBytes(f_pDeviceID->pbBlob, SIZEOF(DRM_ID), f_DeviceCertificate.pbBlob, 0, f_DeviceCertificate.cbBlob);

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}


static DRM_RESULT _ExtractDeviceInfo(
    __in const                            DRM_BYTEBLOB         f_DeviceID,
    __out_bcount(*f_pcbDeviceCertificate) DRM_BYTE     * const f_pbDeviceCertificate,
    __out                                 DRM_DWORD    * const f_pcbDeviceCertificate,
    __out                                 DRM_ID       * const f_pDeviceSerialNumber
)
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DEVICESTORE, PERF_FUNC__ExtractDeviceInfo );

    ChkArg( f_DeviceID.pbBlob
            && f_pcbDeviceCertificate
            && (0 == *f_pcbDeviceCertificate || NULL != f_pbDeviceCertificate)
            && f_pDeviceSerialNumber );

    if ( *f_pcbDeviceCertificate < f_DeviceID.cbBlob - SIZEOF(DRM_ID) )
    {
        dr = DRM_E_BUFFERTOOSMALL;
    }
    *f_pcbDeviceCertificate = f_DeviceID.cbBlob - SIZEOF(DRM_ID);
    ChkDR(dr);

    DRM_BYT_CopyBytes(f_pDeviceSerialNumber, 0, f_DeviceID.pbBlob, 0, SIZEOF(DRM_ID));
    DRM_BYT_CopyBytes(f_pbDeviceCertificate, 0, f_DeviceID.pbBlob, SIZEOF(DRM_ID), *f_pcbDeviceCertificate);

 ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}

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
    DRMFILETIME ft;
    DRM_UINT64  ui64Start;
    DRM_UINT64  ui64Now;


    DRMASSERT( f_pftStartTime != NULL );

    Oem_Clock_GetSystemTimeAsFileTime( f_pOEMContext, &ft );

    /*
    ** Convert start time to quadword seconds
    */
    FILETIME_TO_UI64( *f_pftStartTime, ui64Start );
    ui64Start = DRM_UI64Div( ui64Start, DRM_UI64( C_TICS_PER_SECOND ) );

    /*
    ** Convert current time to quadword seconds
    */
    FILETIME_TO_UI64( ft, ui64Now );
    ui64Now = DRM_UI64Div( ui64Now, DRM_UI64( C_TICS_PER_SECOND ) );

    /*
    ** Return TRUE if the timeout value is less than the elapsed time (now - start)
    */
    if ( DRM_UI64Les( DRM_UI64( f_dwTimeoutSeconds ), DRM_UI64Sub( ui64Now, ui64Start ) ) )
    {
        return TRUE;
    }
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
    DRM_RESULT   dr = DRM_SUCCESS;
    DRM_BYTEBLOB attrName;
    DRM_BYTEBLOB attrValue;
    DRMFILETIME  ftNow;
    DRM_BYTE     rgbValue[__CB_DECL(SIZEOF(DRM_DWORD) * 2)];


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DEVICESTORE, PERF_FUNC_DRM_DeviceStore_MarkDeviceAsValid );

    ChkArg( f_hDeviceHandle != NULL );

    attrName.pbBlob = (DRM_BYTE *) g_dstrDeviceAttrLastValidationTime.pwszString;
    attrName.cbBlob = g_dstrDeviceAttrLastValidationTime.cchString * SIZEOF(DRM_WCHAR);

    Oem_Clock_GetSystemTimeAsFileTime(f_pOEMContext, &ftNow);
    DWORD_TO_NETWORKBYTES(rgbValue, 0, ftNow.dwHighDateTime);
    DWORD_TO_NETWORKBYTES(rgbValue, SIZEOF(DRM_DWORD), ftNow.dwLowDateTime);
    attrValue.pbBlob = rgbValue;
    attrValue.cbBlob = SIZEOF(DRM_DWORD) * 2;

    ChkDR( Oem_DeviceStore_SetDeviceAttr( f_hDeviceHandle, attrName, attrValue ) );
    ChkDR( Oem_DeviceStore_CommitDevice( f_hDeviceHandle ) );

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
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
    DRM_RESULT   dr          = DRM_SUCCESS;
    DRMFILETIME  ftValidated = { 0 };
    DRM_BYTE     rgbValidated[ __CB_DECL( SIZEOF(DRM_DWORD) * 2 )] = { 0 };
    DRM_BYTEBLOB attrName    = { NULL, 0 };
    DRM_DWORD    cbValue     = SIZEOF( rgbValidated );
    DRM_BOOL     fResult     = FALSE;

    /* Refresh attributes */
    ChkDR( Oem_DeviceStore_CommitDevice( f_hDeviceHandle ) );

    attrName.pbBlob = (DRM_BYTE *) g_dstrDeviceAttrLastValidationTime.pwszString;
    attrName.cbBlob = g_dstrDeviceAttrLastValidationTime.cchString * SIZEOF(DRM_WCHAR);

    ChkDR( Oem_DeviceStore_GetDeviceAttr( f_hDeviceHandle, attrName, rgbValidated, &cbValue ) );
    ChkBOOL( cbValue == SIZEOF( rgbValidated), DRM_E_FAIL );

    NETWORKBYTES_TO_DWORD( ftValidated.dwHighDateTime, rgbValidated, 0 );
    NETWORKBYTES_TO_DWORD( ftValidated.dwLowDateTime, rgbValidated, SIZEOF(DRM_DWORD) );

    fResult = !DRM_DeviceStore_HasTimedOut( f_pOEMContext,
                                            &ftValidated,
                                            DRM_DEVICESTORE_VALIDATION_TIMEOUT );

ErrorExit:
    TRACE_IF_FAILED(( "DRM_DeviceStore_IsDeviceValid failed; dr = 0x%x", dr ));
    return fResult;
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
    Oem_DeviceStore_FreeDevice((OEM_DEVICE_HANDLE) f_hDeviceHandle);
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
    DRM_RESULT               dr = DRM_SUCCESS;
    DRM_BYTEBLOB             Tmp = {NULL, 0};

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DEVICESTORE, PERF_FUNC_DRM_DeviceStore_UnRegisterDevice );

    ChkArg( f_pOemDevStoreContext != NULL );
    ChkArg( f_DeviceCertificate.pbBlob != NULL );

    ChkDR( _GenerateDeviceID(&Tmp, f_DeviceCertificate, f_DeviceSerialNumber) );
    ChkDR( Oem_DeviceStore_UnRegisterDevice( f_pOemDevStoreContext, Tmp) );

 ErrorExit:
    SAFE_OEM_FREE(Tmp.pbBlob);

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
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
    DRM_RESULT               dr = DRM_SUCCESS;

    ChkArg( f_pOemDevStoreContext != NULL );
    ChkArg( f_phDeviceHandle != NULL );

    ChkDR( Oem_DeviceStore_GetFirstDevice( f_pOemDevStoreContext,
                                           (OEM_DEVICE_HANDLE *) f_phDeviceHandle ) );

 ErrorExit:
    return dr;
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
    DRM_RESULT               dr = DRM_SUCCESS;

    ChkArg( f_pOemDevStoreContext != NULL );
    ChkArg( f_phDeviceHandle != NULL );

    ChkDR( Oem_DeviceStore_GetNextDevice( f_pOemDevStoreContext,
                                          (OEM_DEVICE_HANDLE *) f_phDeviceHandle ) );

 ErrorExit:
    return dr;
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
    DRM_RESULT               dr = DRM_SUCCESS;
    DRM_BYTEBLOB             Tmp = {NULL, 0};

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DEVICESTORE, PERF_FUNC_DRM_DeviceStore_GetDeviceByID );

    ChkArg( f_pOemDevStoreContext != NULL );
    ChkArg( f_DeviceCertificate.pbBlob!= NULL );
    ChkArg( f_phDeviceHandle != NULL );

    ChkDR( _GenerateDeviceID(&Tmp, f_DeviceCertificate, f_DeviceSerialNumber) );
    ChkDR( Oem_DeviceStore_GetDeviceByID( f_pOemDevStoreContext,
                                          Tmp,
                                          (OEM_DEVICE_HANDLE *) f_phDeviceHandle ) );

 ErrorExit:
    SAFE_OEM_FREE(Tmp.pbBlob);

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
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
    DRM_RESULT   dr = DRM_SUCCESS;
    DRM_BYTEBLOB Tmp = {NULL, 0};

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DEVICESTORE, PERF_FUNC_DRM_DeviceStore_GetDeviceInfo );

    ChkArg( f_pcbDeviceCertificate
            && (0 == *f_pcbDeviceCertificate || NULL != f_pbDeviceCertificate)
            && f_pDeviceSerialNumber );

    dr = Oem_DeviceStore_GetDeviceID((OEM_DEVICE_HANDLE) f_hDeviceHandle,
                                     Tmp.pbBlob,
                                     &Tmp.cbBlob);
    if ( DRM_E_BUFFERTOOSMALL == dr )
    {
        Tmp.pbBlob = (DRM_BYTE*)Oem_MemAlloc(Tmp.cbBlob);
        ChkMem(Tmp.pbBlob);
        dr = Oem_DeviceStore_GetDeviceID((OEM_DEVICE_HANDLE) f_hDeviceHandle,
                                         Tmp.pbBlob,
                                         &Tmp.cbBlob);
    }
    ChkDR(dr);

    ChkDR(_ExtractDeviceInfo(Tmp, f_pbDeviceCertificate, f_pcbDeviceCertificate, f_pDeviceSerialNumber));

 ErrorExit:
    SAFE_OEM_FREE(Tmp.pbBlob);

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
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
    DRM_RESULT dr = DRM_SUCCESS;

    ChkDR( Oem_DeviceStore_GetDeviceAttr((OEM_DEVICE_HANDLE) f_hDeviceHandle,
                                          f_AttrName,
                                          f_pbAttrValue,
                                          f_pcbAttrValue) );

 ErrorExit:
    return dr;
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
    DRM_RESULT dr = DRM_SUCCESS;

    ChkDR( Oem_DeviceStore_GetDeviceAttrCount((OEM_DEVICE_HANDLE) f_hDeviceHandle,
                                               f_pdwAttrCount) );

 ErrorExit:
    return dr;
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
    DRM_RESULT dr = DRM_SUCCESS;

    ChkDR( Oem_DeviceStore_GetDeviceAttrByIndex((OEM_DEVICE_HANDLE) f_hDeviceHandle,
                                                f_dwAttrIndex,
                                                f_pbAttrName,
                                                f_pcbAttrName,
                                                f_pbAttrValue,
                                                f_pcbAttrValue) );

 ErrorExit:
    return dr;
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
    DRM_RESULT dr = DRM_SUCCESS;

    ChkDR( Oem_DeviceStore_SetDeviceAttr((OEM_DEVICE_HANDLE) f_hDeviceHandle,
                                         f_AttrName,
                                         f_AttrValue) );

 ErrorExit:
    return dr;
}

EXIT_PK_NAMESPACE_CODE;

