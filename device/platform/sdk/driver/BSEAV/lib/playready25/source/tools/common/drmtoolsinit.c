/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmtoolsinit.h>

ENTER_PK_NAMESPACE_CODE;

/**********************************************************************
**
** Function:    DRMTOOLS_DrmInitializeWithOpaqueBuffer
**
** Synopsis:    Allocates and initializes DRM_APP_CONTEXT and its corresponding OpaqueBuffer
**
** Arguments:
**
** [f_pOEMContext]          -- OEMContext to pass to Drm_Initialize
** [f_pdstrDeviceStoreName] -- DeviceStoreName to pass to Drm_Initialize
** [f_cbOpaqueBuffer]       -- Size of OpaqueBuffer to allocate and pass to Drm_Initialize
** [f_ppbOpaqueBuffer]      -- OpaqueBuffer allocated
** [f_ppAppContext]         -- DRM_APP_CONTEXT allocated and initialized
**
** Returns:                 DRM_SUCCESS on success or failed with.
**                          DRM_E_INVALIDARG if any parameter is incorrect.
**                          Other errors found in drmresults.h.
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRMTOOLS_DrmInitializeWithOpaqueBuffer(
    __in_opt                               DRM_VOID         *f_pOEMContext,
    __in                             const DRM_CONST_STRING *f_pdstrDeviceStoreName,
    __in                                   DRM_DWORD         f_cbOpaqueBuffer,
    __deref_out_bcount( f_cbOpaqueBuffer ) DRM_BYTE        **f_ppbOpaqueBuffer,
    __out                                  DRM_APP_CONTEXT **f_ppAppContext )
{
    DRM_RESULT       dr             = DRM_SUCCESS;
    DRM_BYTE        *pbOpaqueBuffer = NULL;
    DRM_APP_CONTEXT *pAppContext    = NULL;

    ChkArg( f_ppbOpaqueBuffer  != NULL );
    ChkArg( f_ppAppContext     != NULL );

    ChkMem( pAppContext = (DRM_APP_CONTEXT*)Oem_MemAlloc( SIZEOF( DRM_APP_CONTEXT ) ) );
    ZEROMEM( pAppContext, SIZEOF( DRM_APP_CONTEXT ) );
    ChkMem( pbOpaqueBuffer = (DRM_BYTE*)Oem_MemAlloc( f_cbOpaqueBuffer ) );
    ChkDR( Drm_Initialize( pAppContext, f_pOEMContext, pbOpaqueBuffer, f_cbOpaqueBuffer, f_pdstrDeviceStoreName ) );

    *f_ppbOpaqueBuffer = pbOpaqueBuffer;
    pbOpaqueBuffer     = NULL;
    *f_ppAppContext    = pAppContext;
    pAppContext        = NULL;

ErrorExit:
    SAFE_OEM_FREE( pbOpaqueBuffer );
    SAFE_OEM_FREE( pAppContext );
    return dr;
}

/**********************************************************************
**
** Function:    DRMTOOLS_DrmUninitializeWithOpaqueBuffer
**
** Synopsis:    Uninitializes after DRMTOOLS_DrmInitializeWithOpaqueBuffer
**
** Arguments:
**
** [f_ppbOpaqueBuffer]      -- OpaqueBuffer returned from DRMTOOLS_DrmInitializeWithOpaqueBuffer
** [f_ppAppContext]         -- DRM_APP_CONTEXT returned from DRMTOOLS_DrmInitializeWithOpaqueBuffer
**
** Returns:                 DRM_VOID
**
**********************************************************************/
DRM_API DRM_VOID DRM_CALL DRMTOOLS_DrmUninitializeWithOpaqueBuffer(
    __out   DRM_BYTE        **f_ppbOpaqueBuffer,
    __out   DRM_APP_CONTEXT **f_ppAppContext )
{
    if( f_ppbOpaqueBuffer != NULL )
    {
        SAFE_OEM_FREE( *f_ppbOpaqueBuffer );
    }

    if( f_ppAppContext != NULL && *f_ppAppContext != NULL )
    {
        Drm_Uninitialize( *f_ppAppContext );
        SAFE_OEM_FREE( *f_ppAppContext );
    }
}

EXIT_PK_NAMESPACE_CODE;
