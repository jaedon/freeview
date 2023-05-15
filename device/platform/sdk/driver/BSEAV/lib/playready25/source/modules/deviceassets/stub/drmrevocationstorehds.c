/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/


#define DRM_BUILDING_DRMREVOCATIONSTOREINTERNALS_C
#include <drmrevocationstore.h>
#include <drmprofile.h>
#include <drmlastinclude.h>

ENTER_PK_NAMESPACE_CODE;

static const DRM_DST_NAMESPACE RVS_STORE_NAMESPACE = { 'i', 'n', 's', 'e', 'c', 'u', 'r', 'e', 's', 't', 'o', 'r', 'e', '\0' };

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
    DRM_RESULT                   dr             = DRM_SUCCESS;
    const DRM_ID                *pLID           = NULL;
    _RevocationStoreContext     *pContext       = (_RevocationStoreContext*)f_pContextRev;
    DRM_DST_NAMESPACE_CONTEXT    oNsContext     = {0};
    DRM_DST_SLOT_CONTEXT         oSlotContext   = {0};
    DRM_DWORD                    cbMinSlot      = f_cbRevocationData;
    DRM_DWORD                    cbSlot         = cbMinSlot;
    DRM_DWORD                    cbWritten      = 0;
    DRM_BOOL                     fSlotOpen      = FALSE;
    DRM_BOOL                     fNameSpaceOpen = FALSE;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_REVOCATION, PERF_FUNC_DRM_RVS_StoreRevocationData );

    ChkDR( DRM_RVS_LoopkupRevocationLIDFromGUID( f_pRevGUID, &pLID ) );

    ChkDR( DRM_DST_OpenNamespace(
        pContext->pDatastore,
        &RVS_STORE_NAMESPACE,
        (DRM_DST_OPENMODE)(eDRM_DST_OPEN_ALWAYS | eDRM_DST_LOCKWAIT),
        DRM_SECURE_STORE_NUM_CHILD_NODES,
        DRM_SST_PREALLOCATE_SIZE,
        &oNsContext ) );
    fNameSpaceOpen = TRUE;

    ChkDR( DRM_DST_OpenSlot(
        pContext->pDatastore,
        &oNsContext,
        eDRM_DST_OPEN_ALWAYS | eDRM_DST_LOCKEXCLUSIVE | eDRM_DST_LOCKWAIT,
        pLID,
        pLID,
        NULL,
        &cbSlot,
        &oSlotContext ) );
    fSlotOpen = TRUE;

    if( cbSlot < cbMinSlot )
    {
        ChkDR( DRM_DST_SlotResize(
            pContext->pDatastore,
            &oSlotContext,
            cbMinSlot ) );
        cbSlot = cbMinSlot;
    }

    ChkDR( DRM_DST_SlotSeek(
        pContext->pDatastore,
        &oSlotContext,
        0,
        eDRM_DST_SEEKSET,
        NULL ) );

    ChkDR( DRM_DST_SlotWrite(
        pContext->pDatastore,
        &oSlotContext,
        f_cbRevocationData,
        f_pbRevocationData,
        &cbWritten ) );
    ChkBOOL( cbWritten == f_cbRevocationData, DRM_E_FILE_WRITE_ERROR );

#if !_DATASTORE_WRITE_THRU
    ChkDR( DRM_DST_CommitNamespace(
        pContext->pDatastore,
        &oNsContext ) );
#endif

ErrorExit:
    if( fSlotOpen )
    {
        DRM_RESULT dr2 = DRM_DST_CloseSlot( pContext->pDatastore, &oSlotContext );
        if( DRM_SUCCEEDED( dr ) && DRM_FAILED( dr2 ) )
        {
            dr = dr2;
        }
    }
    if( fNameSpaceOpen )
    {
        DRM_RESULT dr2 = DRM_DST_CloseNamespace( pContext->pDatastore, &oNsContext );
        if( DRM_SUCCEEDED( dr ) && DRM_FAILED( dr2 ) )
        {
            dr = dr2;
        }
    }

    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}


DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_RVS_GetRevocationData(
    __inout                                      DRM_REVOCATIONSTORE_CONTEXT *f_pContextRev,
    __in                                   const DRM_GUID                    *f_pRevGUID,
    __out_bcount_opt(*f_pcbRevocationData)       DRM_BYTE                    *f_pbRevocationData,
    __inout                                      DRM_DWORD                   *f_pcbRevocationData )
{
    DRM_RESULT                   dr             = DRM_SUCCESS;
    const DRM_ID                *pLID           = NULL;
    _RevocationStoreContext     *pContext       = (_RevocationStoreContext*)f_pContextRev;
    DRM_DST_NAMESPACE_CONTEXT    oNsContext     = {0};
    DRM_DST_SLOT_CONTEXT         oSlotContext   = {0};
    DRM_BOOL                     fSlotOpen      = FALSE;
    DRM_BOOL                     fNameSpaceOpen = FALSE;
    DRM_DWORD                    cbSlot         = 0;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_REVOCATION, PERF_FUNC_DRM_RVS_GetRevocationData );

    ChkDR( DRM_RVS_LoopkupRevocationLIDFromGUID( f_pRevGUID, &pLID ) );

    ChkDRMap( DRM_DST_OpenNamespace(
        pContext->pDatastore,
        &RVS_STORE_NAMESPACE,
        (DRM_DST_OPENMODE)(eDRM_DST_OPEN_EXISTING | eDRM_DST_LOCKWAIT),
        DRM_SECURE_STORE_NUM_CHILD_NODES,
        DRM_SST_PREALLOCATE_SIZE,
        &oNsContext ), DRM_E_DST_NAMESPACE_NOT_FOUND, DRM_E_DST_SLOT_NOT_FOUND );
    fNameSpaceOpen = TRUE;

    ChkDR( DRM_DST_OpenSlot(
        pContext->pDatastore,
        &oNsContext,
        eDRM_DST_OPEN_EXISTING | eDRM_DST_LOCKEXCLUSIVE | eDRM_DST_LOCKWAIT,
        pLID,
        pLID,
        NULL,
        &cbSlot,
        &oSlotContext ) );
    fSlotOpen = TRUE;

    if( f_pbRevocationData == NULL || *f_pcbRevocationData < cbSlot )
    {
        *f_pcbRevocationData = cbSlot;
        ChkDR( DRM_E_BUFFERTOOSMALL );
    }

    ChkDR( DRM_DST_SlotRead(
        pContext->pDatastore,
        &oSlotContext,
        cbSlot,
        f_pbRevocationData,
        f_pcbRevocationData ) );
    ChkBOOL( *f_pcbRevocationData == cbSlot, DRM_E_FILE_READ_ERROR );

ErrorExit:
    if( fSlotOpen )
    {
        DRM_RESULT dr2 = DRM_DST_CloseSlot( pContext->pDatastore, &oSlotContext );
        if( DRM_SUCCEEDED( dr ) && DRM_FAILED( dr2 ) )
        {
            dr = dr2;
        }
    }
    if( fNameSpaceOpen )
    {
        DRM_RESULT dr2 = DRM_DST_CloseNamespace( pContext->pDatastore, &oNsContext );
        if( DRM_SUCCEEDED( dr ) && DRM_FAILED( dr2 ) )
        {
            dr = dr2;
        }
    }

    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}

EXIT_PK_NAMESPACE_CODE;


