/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmcommon.h>

#include <drmutilities.h>
#include <drmcontextsizes.h>
#include <drmsecurestore.h>
#include <drmrevocationstore.h>
#include <drmapprevocation.h>
#include <drmdevicerevocation.h>
#include <drmwmdrmnet.h>
#include <drmrevocationconstants.h>

ENTER_PK_NAMESPACE_CODE;

/*****************************************************************************
** Function: _CreateRevocationStorePassword
**
** Synopsis: create the one-byte-zero hash to open the revocations secure store
**
** Arguments:
**            [f_pcontextBBX] -- blackbox context
**            [f_rgbPassword] -- output buffer
*****************************************************************************/

static DRM_NO_INLINE DRM_RESULT DRM_CALL _CreateRevocationStorePassword(
    __inout                           _RevocationStoreContext *f_pContextRev,
    __out_bcount(DRM_SHA1_DIGEST_LEN) DRM_BYTE                 f_rgbPassword [__CB_DECL(DRM_SHA1_DIGEST_LEN)])
{
    DRM_RESULT dr                                           = DRM_SUCCESS;
    const DRM_BYTE bZero                                    = 0x00;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_REVOCATION, PERF_FUNC__CreateRevocationStorePassword );

    DRMASSERT( f_pContextRev != NULL );

    if( !f_pContextRev->fInitedPassword )
    {
        /* If we've never calculated the password before, do it now */
        ChkDR( DRM_BBX_HashValue(&bZero, SIZEOF (bZero), f_pContextRev->rgbPassword, f_pContextRev->pContextBB ) );
        f_pContextRev->fInitedPassword = TRUE;
    }

    /* Copy the cached password, it will never change. */
    MEMCPY( f_rgbPassword, f_pContextRev->rgbPassword, DRM_SHA1_DIGEST_LEN );

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

static DRM_NO_INLINE DRM_RESULT DRM_CALL _LoopkupRevocationLIDFromGUID(
    __in  const DRM_GUID  *f_pRevGUID,
    __out const DRM_ID   **f_pLID )
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_REVOCATION, PERF_FUNC__LoopkupRevocationLIDFromGUID );

    DRMASSERT( f_pRevGUID != NULL );
    DRMASSERT( f_pLID     != NULL );

    *f_pLID = NULL;

    if( MEMCMP( f_pRevGUID, &g_guidRevocationTypeRevInfo, SIZEOF( DRM_GUID ) ) == 0 )
    {
        *f_pLID = &g_lidRevocationInfo;
    }
    else if( MEMCMP( f_pRevGUID, &g_guidRevocationTypeRevInfo2, SIZEOF( DRM_GUID ) ) == 0 )
    {
        *f_pLID = &g_lidRevocationInfo2;
    }
    else if( MEMCMP( f_pRevGUID, &g_guidRevocationTypePlayReadySilverLightRuntime, SIZEOF( DRM_GUID ) ) == 0 )
    {
        *f_pLID = &g_lidPRRTRevocation;
    }
    else if( MEMCMP( f_pRevGUID, &g_guidRevocationTypePlayReadySilverLightApplication, SIZEOF( DRM_GUID ) ) == 0 )
    {
        *f_pLID = &g_lidPRAppRevocation;
    }
    else if( DRM_DEVICEREVOCATION_IsDeviceRevocationSupported()
          && ( MEMCMP( f_pRevGUID, &g_guidRevocationTypeDevice, SIZEOF( DRM_GUID ) ) == 0 ) )
    {
        *f_pLID = &g_lidDeviceRevocation;
    }
    else if( DRM_APPREVOCATION_IsAppRevocationSupported()
          && ( MEMCMP( f_pRevGUID, &g_guidRevocationTypeApp, SIZEOF( DRM_GUID ) ) == 0 ) )
    {
        *f_pLID = &g_lidAppRevocation;
    }
    else if( DRM_WMDRMNET_IsWmdrmnetSupported()
          && ( MEMCMP( f_pRevGUID, &g_guidRevocationTypeWMDRMNET, SIZEOF( DRM_GUID ) ) == 0 ) )
    {
        *f_pLID = &g_lidWMDRMNET_Revocation;
    }
    else
    {
        ChkDR( DRM_E_REVOCATION_GUID_NOT_RECOGNIZED );
    }

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_RVS_InitRevocationStore(
    __in  const DRM_DST                      *f_pDatastore,
    __in  const DRM_SECSTORE_CONTEXT         *f_pContextSST,
    __in  const DRM_BB_CONTEXT               *f_pContextBB,
    __out       DRM_REVOCATIONSTORE_CONTEXT  *f_pContextRev )
{
    DRM_RESULT               dr       = DRM_SUCCESS;
    _RevocationStoreContext *pContext = (_RevocationStoreContext*)f_pContextRev;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_REVOCATION, PERF_FUNC_DRM_RVS_InitRevocationStore );

    ChkArg( f_pDatastore  != NULL );
    ChkArg( f_pContextSST != NULL );
    ChkArg( f_pContextBB  != NULL );
    ChkArg( f_pContextRev != NULL );

    ZEROMEM( f_pContextRev, SIZEOF( DRM_REVOCATIONSTORE_CONTEXT ) );

    pContext->pContextBB  = (DRM_BB_CONTEXT*)       f_pContextBB;
    pContext->pDatastore  = (DRM_DST*)              f_pDatastore;
    pContext->pContextSST = (DRM_SECSTORE_CONTEXT*) f_pContextSST;

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_RVS_StoreRevocationData(
    __inout                               DRM_REVOCATIONSTORE_CONTEXT *f_pContextRev,
    __in                            const DRM_GUID                    *f_pRevGUID,
    __in_bcount(f_cbRevocationData) const DRM_BYTE                    *f_pbRevocationData,
    __in                            const DRM_DWORD                    f_cbRevocationData )
{
    DRM_RESULT    dr                               = DRM_SUCCESS;
    DRM_BYTE      rgbPassword[DRM_SHA1_DIGEST_LEN] = {0};
    const DRM_ID *pLID                             = NULL;
    _RevocationStoreContext *pContext              = (_RevocationStoreContext*)f_pContextRev;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_REVOCATION, PERF_FUNC_DRM_RVS_StoreRevocationData );

    ChkDR( _LoopkupRevocationLIDFromGUID( f_pRevGUID, &pLID ) );

    ChkDR( _CreateRevocationStorePassword( pContext, rgbPassword ) );

    ChkDR( DRM_SST_SetData( pContext->pContextSST,
                            pLID,
                            NULL,
                            rgbPassword,
                            SECURE_STORE_REVOCATION_DATA,
                            pContext->pDatastore,
                            f_pbRevocationData,
                            f_cbRevocationData ) );
ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}


DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_RVS_GetRevocationData(
    __inout                                      DRM_REVOCATIONSTORE_CONTEXT *f_pContextRev,
    __in                                   const DRM_GUID                    *f_pRevGUID,
    __out_bcount_opt(*f_pcbRevocationData)       DRM_BYTE                    *f_pbRevocationData,
    __inout                                      DRM_DWORD                   *f_pcbRevocationData )
{
    DRM_RESULT    dr                               = DRM_SUCCESS;
    DRM_BYTE      rgbPassword[DRM_SHA1_DIGEST_LEN] = {0};
    const DRM_ID *pLID                             = NULL;
    _RevocationStoreContext *pContext              = (_RevocationStoreContext*)f_pContextRev;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_REVOCATION, PERF_FUNC_DRM_RVS_GetRevocationData );

    ChkDR( _LoopkupRevocationLIDFromGUID( f_pRevGUID, &pLID ) );

    ChkDR( _CreateRevocationStorePassword( pContext, rgbPassword ) );

    ChkDR( DRM_SST_GetData( pContext->pContextSST,
                            pLID,
                            NULL,
                            rgbPassword,
                            SECURE_STORE_REVOCATION_DATA,
                            pContext->pDatastore,
                            f_pbRevocationData,
                            f_pcbRevocationData));
ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

EXIT_PK_NAMESPACE_CODE;


