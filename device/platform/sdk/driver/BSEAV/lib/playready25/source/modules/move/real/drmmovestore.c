/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_DRMMOVESTORE_C
#include <drmmovestore.h>
#include <drmbytemanip.h>
#include <drmerr.h>
#include <drmlastinclude.h>

ENTER_PK_NAMESPACE_CODE;

#define DRM_MOVE_STORE_MAX_NUMBER_OF_CHILD_NODES   16
#define DRM_MOVE_STORE_EXPECTED_SLOT_SIZE          1000

/* "movestore\0\0\0" */
static const DRM_DST_NAMESPACE g_rgbMoveStoreNameSpace =
{
    'm', 'o', 'v', 'e', 's', 't',
    'o', 'r', 'e', '\0', '\0', '\0',
    '\0', '\0'
};

/* Empy (NULL) ID */
static const DRM_ID g_oPageKey =
{
    'M', 'o', 'v', 'e', 'P', 'a',
    'g', 'e', '\0', '\0', '\0', '\0',
    '\0', '\0', '\0', '\0'
};

/* Empy (NULL) ID */
static const DRM_ID g_oPageEmptyID =
{
    '\0', '\0', '\0', '\0', '\0', '\0',
    '\0', '\0', '\0', '\0', '\0', '\0',
    '\0', '\0', '\0', '\0'
};

/* Active ID */
static const DRM_ID g_oActiveID =
{
    'A', 'c', 't', 'i', 'v', 'e',
    '\0', '\0', '\0', '\0', '\0', '\0',
    '\0', '\0', '\0', '\0'
};

/*
** When a move transaction slot is created we write 'ActiveN' into the slot where N is either 1 or 2 to represent move/copy
** The constant g_ibTxType is the index into g_oActiveID to write the 'N' value (or into a copy of g_oActiveID)
*/
static const DRM_DWORD g_ibTxType = 7;

DRMFORCEINLINE static void _GetIdsForPage( __in DRM_DWORD f_dwPage, __out DRM_ID *f_pid1, __out DRM_ID *f_pid2 )
{
    /*
    ** 2 keys for move page/index entries is
    ** HashKey   = 'MovePage00000000'
    ** UniqueKey = 'XXXXXXXX00000000'  where XXXXXXXX is a DWORD (not string literal) 0 = Index 1..N = Move Page X
    */
    MEMCPY( f_pid1, &g_oPageKey,     SIZEOF( DRM_ID ) );
    MEMCPY( f_pid2, &g_oPageEmptyID, SIZEOF( DRM_ID ) );
    DWORD_TO_NETWORKBYTES(  f_pid2, 0, f_dwPage );
}

DRM_NO_INLINE static DRM_RESULT _MoveStore_AddData(
    __in                          DRM_MOVESTORE_CONTEXT *f_poMoveStoreContext,
    __in                    const DRM_ID                *f_pid1,
    __in                    const DRM_ID                *f_pid2,
    __in_bcount( f_cbData ) const DRM_BYTE              *f_pbData,
    __in                          DRM_DWORD              f_cbData,
    __in_opt                      DRM_DST_SLOT_CONTEXT  *f_poDSTSlotContext ) DRM_NO_INLINE_ATTRIBUTE;
DRM_NO_INLINE static DRM_RESULT _MoveStore_AddData(
    __in                          DRM_MOVESTORE_CONTEXT *f_poMoveStoreContext,
    __in                    const DRM_ID                *f_pid1,
    __in                    const DRM_ID                *f_pid2,
    __in_bcount( f_cbData ) const DRM_BYTE              *f_pbData,
    __in                          DRM_DWORD              f_cbData,
    __in_opt                      DRM_DST_SLOT_CONTEXT  *f_poDSTSlotContext )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_MOVESTORE_CONTEXT_PRIVATE *poMoveStoreContextPrivate =
        ( DRM_MOVESTORE_CONTEXT_PRIVATE * )f_poMoveStoreContext;
    DRM_DWORD cbWritten  = 0;
    DRM_DWORD cbSlotSize = 0;
    DRM_BOOL fOpened = FALSE;
    DRM_DST_SLOT_CONTEXT  *poDSTSlotContext = f_poDSTSlotContext;

    ChkArg( f_poMoveStoreContext != NULL );
    ChkArg( poMoveStoreContextPrivate->m_fInited );

    if( f_poDSTSlotContext == NULL )
    {
        poDSTSlotContext = &poMoveStoreContextPrivate->m_oDSTSlotContext;
    }

    if( f_cbData > 0 )
    {
        ChkArg( f_pbData != NULL );
    }

    cbSlotSize = f_cbData;

    ChkDR( DRM_DST_OpenSlot( poMoveStoreContextPrivate->m_poDST,
                             &poMoveStoreContextPrivate->m_oDSTNameSpaceContext,
                             eDRM_DST_OPEN_ALWAYS | eDRM_DST_LOCKEXCLUSIVE | eDRM_DST_LOCKWAIT,
                             f_pid1,
                             f_pid2,
                             NULL,
                             &cbSlotSize,
                             poDSTSlotContext ) );

    fOpened = TRUE;

    if ( cbSlotSize != f_cbData )
    {
        cbSlotSize = f_cbData;
        ChkDR( DRM_DST_SlotResize( poMoveStoreContextPrivate->m_poDST,
                                   poDSTSlotContext,
                                   cbSlotSize ) );
    }

    /* Write the data . */
    ChkDR( DRM_DST_SlotWrite( poMoveStoreContextPrivate->m_poDST,
                              poDSTSlotContext,
                              f_cbData,
                              f_pbData,
                              &cbWritten ) );

    ChkBOOL( cbWritten == f_cbData, DRM_E_MOVE_STORE_ADD_DATA );

ErrorExit:
    {
        DRM_BOOL fCloseSlot = FALSE;
        if( DRM_FAILED( dr ) )
        {
            if( fOpened )
            {
                fCloseSlot = TRUE;
            }

            if( dr != DRM_E_INVALIDARG
                && dr != DRM_E_DST_SLOT_NOT_FOUND )
            {
                dr = DRM_E_MOVE_STORE_ADD_DATA;
            }
        }

        if( fOpened
            && f_poDSTSlotContext == NULL )
        {
            /*
            **  If the caller did NOT pass in a context to use this means that the slot should be closed
            */
            fCloseSlot = TRUE;
        }

        if( fCloseSlot )
        {
            DRM_DST_CloseSlot( poMoveStoreContextPrivate->m_poDST,
                poDSTSlotContext );
        }
    }
    return dr;
}

DRM_NO_INLINE static DRM_RESULT _MoveStore_GetData(
    __in DRM_MOVESTORE_CONTEXT *f_poMoveStoreContext,
    __in const DRM_ID *f_pid1,
    __in const DRM_ID *f_pid2,
    __out_bcount_opt( *f_pcbData ) DRM_BYTE *f_pbData,
    __inout DRM_DWORD *f_pcbData ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE static DRM_RESULT _MoveStore_GetData(
    __in DRM_MOVESTORE_CONTEXT *f_poMoveStoreContext,
    __in const DRM_ID *f_pid1,
    __in const DRM_ID *f_pid2,
    __out_bcount_opt( *f_pcbData ) DRM_BYTE *f_pbData,
    __inout DRM_DWORD *f_pcbData )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_MOVESTORE_CONTEXT_PRIVATE *poMoveStoreContextPrivate =
        ( DRM_MOVESTORE_CONTEXT_PRIVATE * )f_poMoveStoreContext;
    DRM_DWORD cbSlotSize = 0;
    DRM_BOOL fOpened = FALSE;

    ChkArg( f_poMoveStoreContext != NULL );
    ChkArg( poMoveStoreContextPrivate->m_fInited );
    ChkArg( f_pcbData != NULL );
    ChkArg( f_pbData == NULL ||
           *f_pcbData > 0 );

    ChkDR( DRM_DST_OpenSlot( poMoveStoreContextPrivate->m_poDST,
                             &poMoveStoreContextPrivate->m_oDSTNameSpaceContext,
                             eDRM_DST_OPEN_EXISTING | eDRM_DST_LOCKWAIT,
                             f_pid1,
                             f_pid2,
                             NULL,
                             &cbSlotSize,
                             &poMoveStoreContextPrivate->m_oDSTSlotContext ) );

    fOpened = TRUE;

    if ( *f_pcbData < cbSlotSize )
    {
        *f_pcbData = cbSlotSize;

        ChkDR( DRM_E_BUFFERTOOSMALL );
    }

    if( cbSlotSize > 0 )
    {
        ChkArg( f_pbData != NULL );

        ChkDR( DRM_DST_SlotRead( poMoveStoreContextPrivate->m_poDST,
                                 &poMoveStoreContextPrivate->m_oDSTSlotContext,
                                 cbSlotSize,
                                 f_pbData,
                                 f_pcbData ) );
    }

ErrorExit:

    if ( fOpened )
    {
        DRM_DST_CloseSlot( poMoveStoreContextPrivate->m_poDST,
                          &poMoveStoreContextPrivate->m_oDSTSlotContext );
    }

    if ( DRM_FAILED( dr ) &&
         dr != DRM_E_INVALIDARG &&
         dr != DRM_E_BUFFERTOOSMALL &&
         dr != DRM_E_DST_SLOT_NOT_FOUND )
    {
        dr = DRM_E_MOVE_STORE_GET_DATA;
    }

    return dr;
}


/*********************************************************************
**
** Function: DRM_MOVST_OpenStore
**
** Synopsis: API that opens a move store and returns an initialized
**           move store context.
**
** Arguments:
**
** [f_pDatastore]           -- Pointer to an initialized data store.
** [f_poMoveStoreContext]   -- Pointer to a move store context to be
**                             initialized by the API.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_MOVE_STORE_OPEN_STORE for all other
**                          errors.
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_MOVST_OpenStore(
    __in  DRM_DST               *f_pDatastore,
    __out DRM_MOVESTORE_CONTEXT *f_poMoveStoreContext )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_MOVESTORE_CONTEXT_PRIVATE *poMoveStoreContextPrivate =
        ( DRM_MOVESTORE_CONTEXT_PRIVATE * )f_poMoveStoreContext;

    ChkArg( f_pDatastore != NULL );
    ChkArg( f_poMoveStoreContext != NULL );
    ChkArg( !poMoveStoreContextPrivate->m_fInited );

    DRMSIZEASSERT( SIZEOF( DRM_MOVESTORE_CONTEXT_PRIVATE ), SIZEOF ( DRM_MOVESTORE_CONTEXT ) );

    /* Open the namespace of the Move Store */
    ChkDR( DRM_DST_OpenNamespace( f_pDatastore,
                                 &g_rgbMoveStoreNameSpace,
                                  (DRM_DST_OPENMODE)(eDRM_DST_OPEN_ALWAYS | eDRM_DST_LOCKWAIT),
                                  DRM_MOVE_STORE_MAX_NUMBER_OF_CHILD_NODES,
                                  DRM_MOVE_STORE_EXPECTED_SLOT_SIZE,
                                  &poMoveStoreContextPrivate->m_oDSTNameSpaceContext ) );

    poMoveStoreContextPrivate->m_fInited = TRUE;

    poMoveStoreContextPrivate->m_poDST = f_pDatastore;

ErrorExit:

    if ( DRM_FAILED( dr ) &&
         dr != DRM_E_INVALIDARG )
    {
        dr = DRM_E_MOVE_STORE_OPEN_STORE;
    }

    return dr;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_MOVST_CloseStore(
    __in DRM_MOVESTORE_CONTEXT *f_poMoveStoreContext )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_MOVESTORE_CONTEXT_PRIVATE *poMoveStoreContextPrivate =
        ( DRM_MOVESTORE_CONTEXT_PRIVATE * )f_poMoveStoreContext;

    ChkArg( f_poMoveStoreContext != NULL );

    /*
    ** If a move store context has not yet been initialized,
    ** just exit the function without returning any error code.
    */
    if ( !poMoveStoreContextPrivate->m_fInited )
    {
        goto ErrorExit;
    }

    /*
    ** If the move store context has been initialized, then these are errors
    */
    ChkDR( DRM_DST_CloseNamespace( poMoveStoreContextPrivate->m_poDST,
                                   &poMoveStoreContextPrivate->m_oDSTNameSpaceContext ) );

    /* Implicitly set poMoveStoreContextPrivate->m_fInited to FALSE. */
    ZEROMEM( f_poMoveStoreContext, SIZEOF( DRM_MOVESTORE_CONTEXT ) );

ErrorExit:

    if ( DRM_FAILED( dr ) &&
         dr != DRM_E_INVALIDARG )
    {
        dr = DRM_E_MOVE_STORE_CLOSE_STORE;
    }

    return dr;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_MOVST_UpdateMovePage(
    __in                          DRM_MOVESTORE_CONTEXT *f_poMoveStoreContext,
    __in                          DRM_DWORD              f_iPage,
    __in_bcount( f_cbPage ) const DRM_BYTE              *f_pbPage,
    __in                          DRM_DWORD              f_cbPage )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_ID id1;
    DRM_ID id2;

    _GetIdsForPage( f_iPage, &id1, &id2 );

    return _MoveStore_AddData( f_poMoveStoreContext,
                              &id1,
                              &id2,
                               f_pbPage,
                               f_cbPage,
                               NULL );
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_MOVST_GetMovePage(
    __in                           DRM_MOVESTORE_CONTEXT *f_poMoveStoreContext,
    __in                           DRM_DWORD              f_iPage,
    __out_bcount_opt( *f_pcbPage ) DRM_BYTE              *f_pbPage,
    __inout                        DRM_DWORD             *f_pcbPage )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_ID id1;
    DRM_ID id2;

    _GetIdsForPage( f_iPage, &id1, &id2 );

    return _MoveStore_GetData( f_poMoveStoreContext,
                              &id1,
                              &id2,
                               f_pbPage,
                               f_pcbPage );
}

EXIT_PK_NAMESPACE_CODE;

