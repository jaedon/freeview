/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_OEMNDT_C
#include <drmcrt.h>
#include <drmtypes.h>
#include <ndttypes.h>
#include <oemndt.h>
#include <drmerr.h>
#include <drmprofile.h>
#include <drmbytemanip.h>
#include <drmlastinclude.h>

ENTER_PK_NAMESPACE_CODE;


/* "MTP" */
#define PROXIMITYDETECTIONADDRESS_LEN 4 /* include NULL terminator */
static DRM_BYTE s_szProximityDetectionAddress[__CB_DECL(PROXIMITYDETECTIONADDRESS_LEN)] = {
    'M', 'T', 'P', '\0'
};

/* "TA:" */
static DRM_BYTE s_szProximityDetectionChannel[] = {
    'T', 'A', ':', '\0'
};


/**********************************************************************
**
** Function:    Oem_Ndt_AllowNewDeviceRegistration
**
** Synopsis:    This OEM function is called by the NDT PK if it
**              receives a registration request from a device that is
**              not already registered in the OEM device store.  If
**              this function returns success, then the device will be
**              added to the device store.  Else it will not be added
**              and the NDT PK will fail the original call that
**              required the device to be registered.
**
**              This function can be used by OEMs who want to pop a
**              UI or take some decision about whether to add a new
**              device.
**
**              For this sample/default implementation, this function just always
**              succeeds.
**
** Arguments:   [f_DeviceCertificate]  -- device certificate
**              [f_DeviceSerialNumber] -- device serial number
**
** Returns:     DRM_SUCCESS -- Always
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL Oem_Ndt_AllowNewDeviceRegistration(
    __in     DRM_BYTEBLOB   f_DeviceCertificate,
    __in     DRM_BYTEBLOB   f_DeviceSerialNumber
)
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT, PERF_FUNC_OEM_NDT_AllowNewDeviceRegistration );

    ChkDR( dr );

    UNREFERENCED_PARAMETER( f_DeviceCertificate );
    UNREFERENCED_PARAMETER( f_DeviceSerialNumber );

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/**********************************************************************
**
** Function:    Oem_Ndt_GetProximityDetectionAddress
**
** Synopsis:    Gets the proximity detection address for a device.
**              The caller (viz the NDT PK) must allocate f_pbAddress
**              and supply the size in *f_pcbAddress.  If the buffer
**              size is sufficient and the function succeeds, it returns
**              the number of bytes copied into the buffer via
**              *f_pcbAddress.
**              If the buffer size is not sufficent, the function will
**              fail with DRM_E_BUFFERTOOSMALL and return the required
**              size in *f_pcbAddress.  This feature can be used by the
**              caller to determine the size of buffer needed to hold
**              the address.  If all the caller wants to do is find out
**              the size of buffer needed, f_pbAddress can be NULL.
**
** Arguments:   [f_pbAddress]             -- caller-allocated pointer to hold address
**              [f_pcbAddress]            -- input  - size of buffer available
**                                           output - size of buffer used
**
** Returns:     DRM_E_BUFFERTOOSMALL - if *f_pcbAddress is too small
**              DRM_SUCCESS          - ON SUCCESS
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL Oem_Ndt_GetProximityDetectionAddress(
    __out_bcount_opt(*f_pcbAddress) DRM_BYTE       *f_pbAddress,
    __inout                         DRM_WORD       *f_pcbAddress
)
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT, PERF_FUNC_OEM_NDT_GetProximityDetectionAddress );

    ChkArg( f_pcbAddress != NULL );

    if ( *f_pcbAddress < PROXIMITYDETECTIONADDRESS_LEN )
    {
        *f_pcbAddress = PROXIMITYDETECTIONADDRESS_LEN;
        ChkDR( DRM_E_BUFFERTOOSMALL );
    }

    ChkArg( f_pbAddress != NULL );

    DRM_BYT_CopyBytes( f_pbAddress,
                       0,
                       s_szProximityDetectionAddress,
                       0,
                       PROXIMITYDETECTIONADDRESS_LEN );

    *f_pcbAddress = PROXIMITYDETECTIONADDRESS_LEN;

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}

DRM_API DRM_RESULT DRM_CALL Oem_Ndt_GetProximityDetectionType(
    __out_ecount(1) DRM_PRND_PROXIMITY_DETECTION_TYPE   *f_peProximityDetectionType )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_peProximityDetectionType != NULL );

    *f_peProximityDetectionType = DRM_PRND_PROXIMITY_DETECTION_TYPE_TRANSPORT_AGNOSTIC;

ErrorExit:
    return dr;
}

DRM_API DRM_RESULT DRM_CALL Oem_Ndt_GetProximityDetectionChannel(
    __out_bcount_opt(*f_pcbChannel) DRM_BYTE       *f_pbChannel,
    __inout                         DRM_WORD       *f_pcbChannel )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pcbChannel != NULL );

    if ( *f_pcbChannel < SIZEOF( s_szProximityDetectionChannel ) )
    {
        *f_pcbChannel = SIZEOF( s_szProximityDetectionChannel );
        ChkDR( DRM_E_BUFFERTOOSMALL );
    }

    ChkArg( f_pbChannel != NULL );

    DRM_BYT_CopyBytes( f_pbChannel,
                       0,
                       s_szProximityDetectionChannel,
                       0,
                       SIZEOF( s_szProximityDetectionChannel ) );

    *f_pcbChannel = SIZEOF( s_szProximityDetectionChannel );

ErrorExit:
    return dr;
}


/******************************************************************************
**
** Function:  OEM_NDT_GetLicenseOPLs
**
** Synopsis:  Sets the OPL (Output Protection Level) values, in the content sessions's ASF Context,
**            to be used in the generation of an XMR license for clear content.
**
** Arguments: [f_MinPlayOpls] -- Supplies a pointer to a variable that
**                               optionally receives the minimum OPL set to use
**                               for clear content.
**            [f_fOplSet] -- Supplies a pointer to a variable that will be
**                           updated with TRUE if f_MinPlayOpls was set, FALSE
**                           otherwise. In the latter case, the content will
**                           have a default set of minimal OPLs corresponding
**                           to test content (in other words, analog copying
**                           allowed.) On error, this value is undefined.
**
** Returns:   DRM_E_INVALIDARG - if the parameter is NULL
**            DRM_SUCCESS      - ON SUCCESS
**
******************************************************************************/
DRM_BOOL   g_fOverride_OPL_Defaults    = FALSE;
DRM_WORD   g_wAnalogVideo              = 200;
DRM_WORD   g_wCompressedDigitalAudio   = 200;
DRM_WORD   g_wCompressedDigitalVideo   = 200;
DRM_WORD   g_wUncompressedDigitalAudio = 200;
DRM_WORD   g_wUncompressedDigitalVideo = 200;

DRM_API DRM_RESULT DRM_CALL OEM_NDT_GetLicenseOPLs(
    __out_opt DRM_MINIMUM_OUTPUT_PROTECTION_LEVELS *f_MinPlayOpls,
    __out     DRM_BOOL                             *f_fOplSet
)
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT, PERF_FUNC_OEM_NDT_GetLicenseOPLs );

    if ( g_fOverride_OPL_Defaults )
    {
        ChkArg( f_MinPlayOpls );
        f_MinPlayOpls->wAnalogVideo              = g_wAnalogVideo;
        f_MinPlayOpls->wCompressedDigitalAudio   = g_wCompressedDigitalAudio;
        f_MinPlayOpls->wCompressedDigitalVideo   = g_wCompressedDigitalVideo;
        f_MinPlayOpls->wUncompressedDigitalAudio = g_wUncompressedDigitalAudio;
        f_MinPlayOpls->wUncompressedDigitalVideo = g_wUncompressedDigitalVideo;

        *f_fOplSet = TRUE;
    }
    else
    {
        *f_fOplSet = FALSE;
    }

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}



/******************************************************************************
**
** Function:  OEM_NDT_GetLicenseInclusionList
**
** Synopsis:  Sets the Inclusion List GUIDs, in the content sessions's ASF Context,
**            to be used in the generation of an XMR license for clear content.
**
** Arguments: [f_InclusionList] -- Supplies a pointer to a variable that
**                                 optionally receives an inclusion list whose
**                                 array was allocated with Oem_MemAlloc.
**            [f_fInclusionListSet] -- Supplies a variable that receives TRUE
**                                     if the list was populated, FALSE
**                                     otherwise. On error, this value is
**                                     undefined.
**
** Returns:   DRM_E_INVALIDARG  - if the parameter is NULL
**            DRM_E_OUTOFMEMORY - if the memory allocation fails
**            DRM_SUCCESS       - ON SUCCESS
**
******************************************************************************/
DRM_BOOL   g_fOverride_InclusionList_Defaults = FALSE;
DRM_DWORD  g_cInclusionListGUIDs              = 2;
DRM_GUID   g_TestGUIDs[3] =
{
    { 0xE0FB055E, 0x4F34, 0x4d46, { 0x8C, 0xBE, 0x41, 0xC6, 0xCB, 0x0A, 0xF5, 0x46 }},
    { 0x3A6390E8, 0x9467, 0x4f08, { 0xA2, 0xA4, 0x29, 0xB9, 0x51, 0xBE, 0xE8, 0x28 }},
    { 0xB0D58B22, 0x64F6, 0x4931, { 0xA7, 0xA1, 0xD3, 0xAB, 0x56, 0xED, 0xAC, 0x6C }},
};
/*  {E0FB055E-4F34-4d46-8CBE-41C6CB0AF546}
**  {3A6390E8-9467-4f08-A2A4-29B951BEE828}
**  {B0D58B22-64F6-4931-A7A1-D3AB56EDAC6C}
*/

DRM_API DRM_RESULT DRM_CALL OEM_NDT_GetLicenseInclusionList(
    __inout_opt DRM_GUIDLIST *f_InclusionList,
    __out       DRM_BOOL     *f_fInclusionListSet
)
{
    DRM_RESULT dr       = DRM_SUCCESS;
    DRM_DWORD  i        = 0;
    DRM_DWORD  dwOffset = 0;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT, PERF_FUNC_OEM_NDT_GetLicenseInclusionList );

    if (   ( g_fOverride_InclusionList_Defaults )
        && ( g_cInclusionListGUIDs >  0 )
        && ( g_cInclusionListGUIDs <= 3 ) )
    {
        f_InclusionList->cGUIDs = g_cInclusionListGUIDs;

        /*
        ** The following memory is freed by routine NDTMGR_ASF_Internal_Uninitialize()
        */
        DRMASSERT( f_InclusionList->pguidBuffer == NULL );
        ChkMem( f_InclusionList->pguidBuffer =
                (DRM_BYTE *)Oem_MemAlloc( SIZEOF(DRM_GUID) * f_InclusionList->cGUIDs ));

        for (i = 0; i < g_cInclusionListGUIDs; i++)
        {
            DRM_BYT_CopyBytes( f_InclusionList->pguidBuffer,
                               dwOffset,
                               (DRM_BYTE *)&g_TestGUIDs[i],
                               0,
                               SIZEOF( DRM_GUID ) );
            dwOffset += SIZEOF( DRM_GUID );
        }

        *f_fInclusionListSet = TRUE;
    }
    else
    {
        *f_fInclusionListSet = FALSE;
    }

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}


/******************************************************************************
**
** Function:  OEM_NDT_GetSourceID
**
** Synopsis:  Gets the Source ID to be used in the XMR license.
**
** Arguments: [f_pdwSourceID]      -- pointer to value to update
**
** Returns:   DRM_E_INVALIDARG  - if the parameter is NULL
**            DRM_SUCCESS       - ON SUCCESS
**
******************************************************************************/
DRM_DWORD g_dwSourceID = 0;
DRM_API DRM_RESULT DRM_CALL OEM_NDT_GetSourceID (
    __out_ecount(1)    DRM_DWORD                               *f_pdwSourceID )
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT, PERF_FUNC_OEM_NDT_GetSourceID );

    ChkArg( f_pdwSourceID      != NULL );

    *f_pdwSourceID = g_dwSourceID;

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}


/******************************************************************************
**
** Function:  OEM_NDT_GetLicenseRevInfoVersion
**
** Synopsis:  Gets the Revocation Information Version to be used in the XMR license.
**
** Arguments: [f_pdwRevInfo]       -- pointer to value to update
**
** Returns:   DRM_E_INVALIDARG  - if the parameter is NULL
**            DRM_SUCCESS       - ON SUCCESS
**
******************************************************************************/
DRM_DWORD g_dwRevInfo = 0;
DRM_API DRM_RESULT DRM_CALL OEM_NDT_GetLicenseRevInfoVersion (
    __out_ecount(1)     DRM_DWORD                               *f_pdwRevInfo )
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT, PERF_FUNC_OEM_NDT_GetLicenseRevInfoVersion );

    ChkArg( f_pdwRevInfo       != NULL );

    *f_pdwRevInfo = g_dwRevInfo;

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}


/******************************************************************************
**
** Function:  OEM_NDT_GetWMDRMNETMinAppSec
**
** Synopsis:  Gets the WMDRMNET Minimum Application Security Level to be used in the XMR license.
**
** Arguments: [f_pdwMinAppSec]     -- pointer to value to update
**
** Returns:   DRM_E_INVALIDARG  - if the parameter is NULL
**            DRM_SUCCESS       - ON SUCCESS
**
******************************************************************************/
DRM_DWORD g_dwMinAppSec = 0;
DRM_API DRM_RESULT DRM_CALL OEM_NDT_GetWMDRMNETMinAppSec(
    __out_ecount(1) DRM_DWORD                               *f_pdwMinAppSec )
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT, PERF_FUNC_OEM_NDT_GetWMDRMNETMinAppSec );

    ChkArg( f_pdwMinAppSec      != NULL );

    *f_pdwMinAppSec = g_dwMinAppSec;

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}


EXIT_PK_NAMESPACE_CODE;

