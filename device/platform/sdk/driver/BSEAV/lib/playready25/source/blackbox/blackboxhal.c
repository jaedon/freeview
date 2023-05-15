/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_BLACKBOXHAL_C

#include <drmtypes.h>
#include <drmcompiler.h>
#include <drmbytemanip.h>
#include <drmsal.h>
#include <drmerr.h>
#include <drmresults.h>
#include <drmblackbox.h>
#include <drmsymopt.h>
#include <oemcommon.h>
#include <oemhal.h>
#include <blackboxhal.h>
#include <drmmathsafe.h>

#include <drmlastinclude.h>

#if DRM_SUPPORT_SECUREOEMHAL
/*
** When using the secure HAL, we require the passed in crypto context to be NULL.
*/
#define USE_SECURE_HAL_CRYPTO_CONTEXT  NULL
#else   /* DRM_SUPPORT_SECUREOEMHAL */
#define USE_SECURE_HAL_CRYPTO_CONTEXT  &f_pBbxContext->CryptoContext
#endif  /* DRM_SUPPORT_SECUREOEMHAL */

ENTER_PK_NAMESPACE_CODE;

/*********************************************************************
**
** Function: _ValidateKeyHandle
**
** Synopsis: Validate that a key handle resolves to a valid block of
**      memory for its type.
**
** Arguments:
**
** [f_pBbxContext]  : The blackbox context.
** [f_hKey]         : Handle to a key.
**
** Returns:    DRM_SUCCESS on success.
**             DRM_E_HANDLE if the handle is invalid.
**
** Notes:      None.
**
**********************************************************************/
static DRM_FRE_INLINE DRM_RESULT _ValidateKeyHandle(
    __inout_ecount( 1 ) const DRM_BB_CONTEXT    *f_pBbxContext,
    __in                      OEM_CRYPTO_HANDLE  f_hKey ) DRM_FRE_INLINE_ATTRIBUTE;
static DRM_FRE_INLINE DRM_RESULT _ValidateKeyHandle(
    __inout_ecount( 1 ) const DRM_BB_CONTEXT    *f_pBbxContext,
    __in                      OEM_CRYPTO_HANDLE  f_hKey )
{
    DRM_RESULT dr = DRM_E_HANDLE;

    ChkPtr( f_pBbxContext );

    if( f_hKey != OEM_CRYPTO_HANDLE_INVALID )
    {
              DRM_BB_KEY_REGISTER_ENTRY *pUnsafeEntry = ( DRM_BB_KEY_REGISTER_ENTRY* )f_hKey;
        const DRM_BB_KEY_REGISTER_ENTRY *pMin         = NULL;
        const DRM_BB_KEY_REGISTER_ENTRY *pMax         = NULL;
              OEM_HAL_KEY_TYPE           apparentType = pUnsafeEntry->eType;

        /*
        ** Find the expected range, given the reported type
        */
        if( apparentType > OEM_HAL_KEY_TYPE_INVALID
         && apparentType < OEM_HAL_KEY_TYPE_NUM_VALID_TYPES )
        {
            ChkBOOL( DRM_BBX_IsSlkCryptoSupported()    || apparentType != OEM_HAL_KEY_TYPE_AES_SLK, DRM_E_NOTIMPL );

            pMin = &( f_pBbxContext->keyHandleState.rgKeyMapTables[ apparentType ].pEntries[ 0 ] );
            pMax = &( f_pBbxContext->keyHandleState.rgKeyMapTables[ apparentType ].pEntries[ f_pBbxContext->keyHandleState.rgKeyMapTables[ apparentType ].cEntries - 1 ] );

            /*
            ** If the type looks ok, check the location is within range
            */
            if( pUnsafeEntry >= pMin && pUnsafeEntry <= pMax )
            {
                /*
                ** If the location is in range, check it aligns with a valid entry
                */
                DRM_SIZE_T offset = ( DRM_SIZE_T )( ( DRM_BYTE* )pUnsafeEntry - ( DRM_BYTE* )pMin );

                if( ( offset % SIZEOF( DRM_BB_KEY_REGISTER_ENTRY ) ) == 0 )
                {
                    dr = DRM_SUCCESS;
                }
            }
        }
    }

ErrorExit:
    return dr;
}


/*********************************************************************
**
** Function:   _ResolveKeyHandle
**
** Synopsis:   Validate and resolve a key handle to a key entry.
**
** Arguments:
**
** [f_pBbxContext]  : The blackbox context.
** [f_hKey]         : Handle to a key.
** [f_ppEntry]      : Pointer to a DRM_BB_KEY_REGISTER_ENTRY pointer to populate.
**
** Returns:    DRM_SUCCESS on success.
**             DRM_E_HANDLE if the handle is invalid.
**             DRM_E_POINTER if f_ppEntry is NULL.
**
** Notes:      None.
**
**********************************************************************/
PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_NONCONST_PARAM_25004, "Prefast Noise: DRM_BB_CONTEXT should never be const")
static DRM_FRE_INLINE DRM_RESULT _ResolveKeyHandle(
    __inout_ecount( 1 ) DRM_BB_CONTEXT             *f_pBbxContext,
    __in                OEM_CRYPTO_HANDLE           f_hKey,
    __out_ecount( 1 )   DRM_BB_KEY_REGISTER_ENTRY **f_ppEntry ) DRM_FRE_INLINE_ATTRIBUTE;
static DRM_FRE_INLINE DRM_RESULT _ResolveKeyHandle(
    __inout_ecount( 1 ) DRM_BB_CONTEXT             *f_pBbxContext,
    __in                OEM_CRYPTO_HANDLE           f_hKey,
    __out_ecount( 1 )   DRM_BB_KEY_REGISTER_ENTRY **f_ppEntry )
PREFAST_POP /* __WARNING_NONCONST_PARAM_25004 */
{
    DRM_RESULT                 dr         = DRM_SUCCESS;
    DRM_BB_KEY_REGISTER_ENTRY *pTempEntry = NULL;

    ChkPtr( f_pBbxContext );
    ChkPtr( f_ppEntry );
    ChkDR( _ValidateKeyHandle( f_pBbxContext, f_hKey ) );

    pTempEntry = ( DRM_BB_KEY_REGISTER_ENTRY* )f_hKey;

    if( pTempEntry->eState != eDRM_BB_KEY_REGISTER_STATE_IN_USE
     && pTempEntry->eState != eDRM_BB_KEY_REGISTER_STATE_PRELOADED )
    {
        /*
        ** Something has gone wrong - the key handle is not allocated
        */
        DRMASSERT( FALSE );

        /*
        ** Although the handle resolves, externally it is not valid
        ** so return DRM_E_HANDLE
        */
        ChkDR( DRM_E_HANDLE );
    }

    *f_ppEntry = pTempEntry;

ErrorExit:
    return dr;
}

static DRM_FRE_INLINE DRM_RESULT _AllocateKeyHandleWithCaching(
    __inout_ecount( 1 )               DRM_BB_CONTEXT             *f_pBbxContext,
    __in                              OEM_HAL_KEY_TYPE            f_eType,
    __in_bcount( f_cbCacheKey ) const DRM_BYTE                   *f_pbCacheKey,
    __in                        const DRM_DWORD                   f_cbCacheKey,
    __out_ecount( 1 )                 OEM_CRYPTO_HANDLE          *f_phKey,
    __out_ecount( 1 )                 DRM_BB_KEY_REGISTER_ENTRY **f_ppEntry,
    __out_ecount( 1 )                 DRM_BOOL                   *f_pCachedHandle ) DRM_FRE_INLINE_ATTRIBUTE;
static DRM_FRE_INLINE DRM_RESULT _AllocateKeyHandleWithCaching(
    __inout_ecount( 1 )               DRM_BB_CONTEXT             *f_pBbxContext,
    __in                              OEM_HAL_KEY_TYPE            f_eType,
    __in_bcount( f_cbCacheKey ) const DRM_BYTE                   *f_pbCacheKey,
    __in                        const DRM_DWORD                   f_cbCacheKey,
    __out_ecount( 1 )                 OEM_CRYPTO_HANDLE          *f_phKey,
    __out_ecount( 1 )                 DRM_BB_KEY_REGISTER_ENTRY **f_ppEntry,
    __out_ecount( 1 )                 DRM_BOOL                   *f_pCachedHandle )
{
    DRM_RESULT                 dr                   = DRM_SUCCESS;
    DRM_BB_KEY_REGISTER_TABLE* pTable               = NULL;
    DRM_BB_KEY_REGISTER_ENTRY* pEntryToUse          = NULL;
    DRM_LONG                   iEntry               = 0;
    DRM_LONG                   iUnallocatedEntry    = DRM_BB_KEY_REGISTER_ENTRY_INDEX_INVALID;
    DRM_LONG                   iPurgeEntry          = DRM_BB_KEY_REGISTER_ENTRY_INDEX_INVALID;
    DRM_DWORD                  dwMinUnwrapTimestamp = ULONG_MAX;

    ChkPtr( f_pBbxContext );
    ChkPtr( f_phKey );
    ChkPtr( f_ppEntry );
    ChkBOOL( DRM_BBX_IsSlkCryptoSupported()    || f_eType != OEM_HAL_KEY_TYPE_AES_SLK, DRM_E_NOTIMPL );
    ChkArg( f_eType > OEM_HAL_KEY_TYPE_INVALID );
    ChkArg( f_eType < OEM_HAL_KEY_TYPE_NUM_VALID_TYPES );
    ChkArg( f_cbCacheKey <= DRM_BB_MAX_CACHE_KEY_SIZE_IN_BYTES );
    if( f_cbCacheKey > 0)
    {
        ChkPtr( f_pbCacheKey );
    }

    *f_phKey   = OEM_CRYPTO_HANDLE_INVALID;
    *f_ppEntry = NULL;

    DRMASSERT( f_eType >= 0 && f_eType < NO_OF( f_pBbxContext->keyHandleState.rgKeyMapTables ) );
    pTable = &f_pBbxContext->keyHandleState.rgKeyMapTables[ f_eType ];

    *f_pCachedHandle = FALSE;

    for( iEntry = 0; ( iEntry < pTable->cEntries ) && !( *f_pCachedHandle ); iEntry++ )
    {
        if( ( iUnallocatedEntry                 == DRM_BB_KEY_REGISTER_ENTRY_INDEX_INVALID ) &&
            ( pTable->pEntries[ iEntry ].eState == eDRM_BB_KEY_REGISTER_STATE_UNALLOCATED  ) )
        {
            iUnallocatedEntry = iEntry;
        }

        if( pTable->pEntries[ iEntry ].eState == eDRM_BB_KEY_REGISTER_STATE_UNASSIGNED )
        {
            if( pTable->pEntries[ iEntry ].dwUnwrapTimestamp < dwMinUnwrapTimestamp )
            {
                dwMinUnwrapTimestamp = pTable->pEntries[ iEntry ].dwUnwrapTimestamp;
                iPurgeEntry = iEntry;
            }

            if( ( f_cbCacheKey > 0 ) &&
                ( pTable->pEntries[ iEntry ].cbCacheKey == f_cbCacheKey ) &&
                ( DRM_BYT_CompareBytes( pTable->pEntries[ iEntry ].pbCacheKey,
                                        0,
                                        f_pbCacheKey,
                                        0,
                                        f_cbCacheKey ) == 0 ) )
            {
                /* Cached entry is found. */

                pEntryToUse = &pTable->pEntries[ iEntry ];
                *f_pCachedHandle = TRUE;
            }
        }
    }

    if( pEntryToUse == NULL )
    {
        /* Cached entry is not found. */
        if( iUnallocatedEntry != DRM_BB_KEY_REGISTER_ENTRY_INDEX_INVALID )
        {
            /* Unallocated entry is available. */
            ChkDR( Oem_Hal_AllocateRegister( f_eType, &pTable->pEntries[ iUnallocatedEntry ].indexKeyRegister ) );

            pEntryToUse = &pTable->pEntries[ iUnallocatedEntry ];
        }
        else if( iPurgeEntry != DRM_BB_KEY_REGISTER_ENTRY_INDEX_INVALID )
        {
            /* Unallocated entry is not available but there is a cached entry that can be purged. */
            pEntryToUse = &pTable->pEntries[ iPurgeEntry ];
        }
    }

    if( pEntryToUse == NULL )
    {
        ChkDR( DRM_E_OEMHAL_OUT_OF_KEY_REGISTERS );
    }

    DRMASSERT( pEntryToUse->eType      == f_eType );
    DRMASSERT( pEntryToUse->dwRefCount == 0 );

    pEntryToUse->dwRefCount = 1;
    pEntryToUse->eState     = eDRM_BB_KEY_REGISTER_STATE_IN_USE;
    *f_phKey                = ( OEM_CRYPTO_HANDLE )pEntryToUse;
    *f_ppEntry              = pEntryToUse;
    dr                      = DRM_SUCCESS;

    DRMASSERT( _ValidateKeyHandle( f_pBbxContext, *f_phKey ) == DRM_SUCCESS );

ErrorExit:
    return dr;
}

static DRM_FRE_INLINE DRM_RESULT _AllocateKeyHandleWithoutCaching(
    __inout_ecount( 1 )               DRM_BB_CONTEXT             *f_pBbxContext,
    __in                              OEM_HAL_KEY_TYPE            f_eType,
    __out_ecount( 1 )                 OEM_CRYPTO_HANDLE          *f_phKey,
    __out_ecount( 1 )                 DRM_BB_KEY_REGISTER_ENTRY **f_ppEntry ) DRM_FRE_INLINE_ATTRIBUTE;
static DRM_FRE_INLINE DRM_RESULT _AllocateKeyHandleWithoutCaching(
    __inout_ecount( 1 )               DRM_BB_CONTEXT             *f_pBbxContext,
    __in                              OEM_HAL_KEY_TYPE            f_eType,
    __out_ecount( 1 )                 OEM_CRYPTO_HANDLE          *f_phKey,
    __out_ecount( 1 )                 DRM_BB_KEY_REGISTER_ENTRY **f_ppEntry )
{
    DRM_RESULT                 dr          = DRM_SUCCESS;
    DRM_BB_KEY_REGISTER_TABLE* pTable      = NULL;
    DRM_BB_KEY_REGISTER_ENTRY* pEntryToUse = NULL;
    DRM_LONG                   i           = 0;

    ChkPtr( f_pBbxContext );
    ChkPtr( f_phKey );
    ChkPtr( f_ppEntry );
    ChkBOOL( DRM_BBX_IsSlkCryptoSupported()    || f_eType != OEM_HAL_KEY_TYPE_AES_SLK, DRM_E_NOTIMPL );
    ChkArg( f_eType > OEM_HAL_KEY_TYPE_INVALID );
    ChkArg( f_eType < OEM_HAL_KEY_TYPE_NUM_VALID_TYPES );

    *f_phKey   = OEM_CRYPTO_HANDLE_INVALID;
    *f_ppEntry = NULL;

    DRMASSERT( f_eType >= 0 && f_eType < NO_OF( f_pBbxContext->keyHandleState.rgKeyMapTables ) );
    pTable = &f_pBbxContext->keyHandleState.rgKeyMapTables[ f_eType ];

    for( i = 0; i < pTable->cEntries; i++ )
    {
        switch( pTable->pEntries[ i ].eState )
        {
        case eDRM_BB_KEY_REGISTER_STATE_UNALLOCATED:
            ChkDR( Oem_Hal_AllocateRegister( f_eType, &pTable->pEntries[ i ].indexKeyRegister ) );

            pEntryToUse = &pTable->pEntries[ i ];
            break;

        case eDRM_BB_KEY_REGISTER_STATE_UNASSIGNED:
            pEntryToUse = &pTable->pEntries[ i ];
            break;

        default:
            break;
        }

        /*
        ** Break here, rather than in the for() statement so that when pEntryToUse is set (i) is left at the
        ** last used index, rather than being incremented past.
        */
        if( pEntryToUse != NULL )
        {
            break;
        }
    }

    if( pEntryToUse == NULL )
    {
        ChkDR( DRM_E_OEMHAL_OUT_OF_KEY_REGISTERS );
    }
    else
    {
        DRMASSERT( pEntryToUse->eType      == f_eType );

        DRMASSERT( pEntryToUse->dwRefCount == 0 );
        pEntryToUse->dwRefCount = 1;

        pEntryToUse->eState = eDRM_BB_KEY_REGISTER_STATE_IN_USE;
        *f_phKey            = ( OEM_CRYPTO_HANDLE )pEntryToUse;
        *f_ppEntry          = pEntryToUse;
        dr                  = DRM_SUCCESS;

        DRMASSERT( _ValidateKeyHandle( f_pBbxContext, *f_phKey ) == DRM_SUCCESS );
    }

ErrorExit:
    return dr;
}

/*********************************************************************
**
** Function: _Hal_AllocateKeyHandle
**
** Synopsis: Allocate a key handle for the requested type.
**
** Arguments:
**
** [f_pBbxContext]  : The blackbox context.
** [f_eType]        : Type of key to allocate.
** [f_pbCacheKey]   : Cache key associated with the key handle.
** [f_cbCacheKey]   : Size of f_pbCacheKey in bytes.
** [f_phKey]        : Handle to the DRM_BB_KEY_REGISTER_ENTRY.
** [f_ppEntry]      : The allocated DRM_BB_KEY_REGISTER_ENTRY.
** [f_pCacheHandle] : Indicates whether f_phKey is a previously cached handle.
**
** Returns:    DRM_SUCCESS on success.
**             DRM_E_INVALIDARG if the handle is invalid.
**
** Notes:      None.
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL _Hal_AllocateKeyHandle(
    __inout_ecount( 1 )               DRM_BB_CONTEXT             *f_pBbxContext,
    __in                              OEM_HAL_KEY_TYPE            f_eType,
    __in_bcount( f_cbCacheKey ) const DRM_BYTE                   *f_pbCacheKey,
    __in                        const DRM_DWORD                   f_cbCacheKey,
    __out_ecount( 1 )                 OEM_CRYPTO_HANDLE          *f_phKey,
    __out_ecount( 1 )                 DRM_BB_KEY_REGISTER_ENTRY **f_ppEntry,
    __out_ecount( 1 )                 DRM_BOOL                   *f_pCachedHandle )
{
    DRM_RESULT dr = DRM_SUCCESS;

    if( _Hal_IsHandleCachingSupported() )
    {
        ChkDR( _AllocateKeyHandleWithCaching( f_pBbxContext,
                                              f_eType,
                                              f_pbCacheKey,
                                              f_cbCacheKey,
                                              f_phKey,
                                              f_ppEntry,
                                              f_pCachedHandle ) );
    }
    else
    {
        ChkDR( _AllocateKeyHandleWithoutCaching( f_pBbxContext,
                                                 f_eType,
                                                 f_phKey,
                                                 f_ppEntry ) );
    }

ErrorExit:
    return dr;
}


/*********************************************************************
**
** Function: _AllocatePreloadedKeyHandle
**
** Synopsis: Allocate a key handle for a preloaded key ofthe requested type.
**
** Arguments:
**
** [f_pBbxContext]    : The blackbox context.
** [f_eType]          : Type of key to allocate.
** [f_phKey]          : Handle to the DRM_BB_KEY_REGISTER_ENTRY.
** [f_ppEntry]        : The allocated DRM_BB_KEY_REGISTER_ENTRY.
** [f_pnPreloadIndex] : The next available PreloadedIds table slot.
**
** Returns:    DRM_SUCCESS on success.
**             DRM_E_INVALIDARG if the handle is invalid.
**
** Notes:      None.
**
**********************************************************************/
static DRM_RESULT _AllocatePreloadedKeyHandle(
    __inout_ecount( 1 )   DRM_BB_CONTEXT             *f_pBbxContext,
    __in                  OEM_HAL_KEY_TYPE            f_eType,
    __out_ecount( 1 )     OEM_CRYPTO_HANDLE          *f_phKey,
    __out_ecount( 1 )     DRM_BB_KEY_REGISTER_ENTRY **f_ppEntry,
    __out_ecount_opt( 1 ) DRM_LONG                   *f_pnPreloadIndex )
{
    DRM_RESULT                 dr                    = DRM_SUCCESS;
    DRM_BB_KEY_REGISTER_TABLE* pTable                = NULL;
    DRM_BB_KEY_REGISTER_ENTRY* pEntryToUse           = NULL;
    DRM_LONG                   i                     = 0;
    DRM_LONG                   currentMaxLookupIndex = -1;

    ChkPtr( f_pBbxContext );
    ChkPtr( f_phKey );
    ChkPtr( f_ppEntry );
    ChkBOOL( DRM_BBX_IsSlkCryptoSupported()    || f_eType != OEM_HAL_KEY_TYPE_AES_SLK, DRM_E_NOTIMPL );
    ChkArg( f_eType > OEM_HAL_KEY_TYPE_INVALID );
    ChkArg( f_eType < OEM_HAL_KEY_TYPE_NUM_VALID_TYPES );

    *f_phKey   = OEM_CRYPTO_HANDLE_INVALID;
    *f_ppEntry = NULL;

    DRMASSERT( f_eType >= 0 && f_eType < NO_OF( f_pBbxContext->keyHandleState.rgKeyMapTables ) );
    pTable = &f_pBbxContext->keyHandleState.rgKeyMapTables[ f_eType ];

    while( i < pTable->cEntries && pEntryToUse == NULL )
    {
        switch( pTable->pEntries[ i ].eState )
        {
        case eDRM_BB_KEY_REGISTER_STATE_UNALLOCATED:
            pEntryToUse = &pTable->pEntries[ i ];
            break;

        case eDRM_BB_KEY_REGISTER_STATE_PRELOADED:
            if( pTable->pEntries[ i ].indexPreloadedTable > currentMaxLookupIndex )
            {
                currentMaxLookupIndex = pTable->pEntries[ i ].indexPreloadedTable;
            }

            break;

        default:
            break;
        }

        i++;
    }

    /*
    ** Increment the lookup index to point to the next available slot
    */
    currentMaxLookupIndex++;

    if( pEntryToUse == NULL || currentMaxLookupIndex >= pTable->cPreloadedIds )
    {
        ChkDR( DRM_E_OEMHAL_OUT_OF_KEY_REGISTERS );
    }
    else
    {
        DRMASSERT( pEntryToUse->eType == f_eType );

        if( f_pnPreloadIndex != NULL )
        {
            *f_pnPreloadIndex = currentMaxLookupIndex;
        }

        pEntryToUse->eState = eDRM_BB_KEY_REGISTER_STATE_PRELOADED;
        *f_phKey            = ( OEM_CRYPTO_HANDLE )pEntryToUse;
        *f_ppEntry          = pEntryToUse;
        dr                  = DRM_SUCCESS;

        DRMASSERT( _ValidateKeyHandle( f_pBbxContext, *f_phKey ) == DRM_SUCCESS );
    }

ErrorExit:
    return dr;
}


static DRM_FRE_INLINE DRM_VOID _InitKeyMapTable(
    __in                                 OEM_HAL_KEY_TYPE           f_eType,
    __inout_ecount( 1 )                  DRM_BB_KEY_HANDLE_STATE   *f_pHandleState,
    __inout_ecount( f_cKeyEntries )      DRM_BB_KEY_REGISTER_ENTRY *f_pKeyEntries,
    __in                                 DRM_LONG                   f_cKeyEntries,
    __inout_ecount_opt( f_cPreloadIds )  DRM_GUID                  *f_pPreloadIds,
    __in                                 DRM_LONG                   f_cPreloadIds ) DRM_FRE_INLINE_ATTRIBUTE;
static DRM_FRE_INLINE DRM_VOID _InitKeyMapTable(
    __in                                 OEM_HAL_KEY_TYPE           f_eType,
    __inout_ecount( 1 )                  DRM_BB_KEY_HANDLE_STATE   *f_pHandleState,
    __inout_ecount( f_cKeyEntries )      DRM_BB_KEY_REGISTER_ENTRY *f_pKeyEntries,
    __in                                 DRM_LONG                   f_cKeyEntries,
    __inout_ecount_opt( f_cPreloadIds )  DRM_GUID                  *f_pPreloadIds,
    __in                                 DRM_LONG                   f_cPreloadIds )
{
    DRM_BB_KEY_REGISTER_TABLE *pTable = &f_pHandleState->rgKeyMapTables[ f_eType ];
    DRM_LONG                   i      = 0;

    /*
    ** Initialize the overall table
    */
    pTable->cEntries      = f_cKeyEntries;
    pTable->pEntries      = f_pKeyEntries;

    if( f_pPreloadIds != NULL )
    {
        pTable->cPreloadedIds = f_cPreloadIds;
        pTable->pPreloadedIds = f_pPreloadIds;

        /*
        ** Zero the preload table
        */
        OEM_SECURE_ZERO_MEMORY( f_pPreloadIds, ( SIZEOF( DRM_GUID ) * f_cPreloadIds ) );
    }
    else
    {
        pTable->cPreloadedIds = 0;
        pTable->pPreloadedIds = NULL;
    }

    /*
    ** Initialize the individual key entries
    */
    if( f_cKeyEntries > 0 )
    {
        f_pKeyEntries[ 0 ].eType                 = f_eType;
        f_pKeyEntries[ 0 ].eState                = eDRM_BB_KEY_REGISTER_STATE_UNALLOCATED;
        f_pKeyEntries[ 0 ].indexKeyRegister      = OEM_HAL_KEY_REGISTER_INDEX_INVALID;
        f_pKeyEntries[ 0 ].indexPreloadedTable   = -1;
        f_pKeyEntries[ 0 ].dwRefCount            = 0;

        if( _Hal_IsHandleCachingSupported() )
        {
            OEM_SECURE_ZERO_MEMORY( &f_pKeyEntries[ 0 ].pbCacheKey, DRM_BB_MAX_CACHE_KEY_SIZE_IN_BYTES );
            f_pKeyEntries[ 0 ].cbCacheKey        = 0;
            f_pKeyEntries[ 0 ].dwUnwrapTimestamp = 0;
        }

        for( i = 1; i < f_cKeyEntries; i++ )
        {
            f_pKeyEntries[i] = f_pKeyEntries[ 0 ];
        }
    }
}

/*********************************************************************
**
** Function:  _Hal_IsHandleCachingSupported
**
** Synopsis:  Defines whether crypto handle caching in black box HAL is supported.
**
** Returns:   TRUE if handle caching is supported and FALSE otherwise.
**
** Notes:     None.
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_BOOL DRM_CALL _Hal_IsHandleCachingSupported(DRM_VOID)
{
    return TRUE;
}

/*********************************************************************
**
** Function:  _Hal_Initialize
**
** Synopsis:  Initializes the crypto core. This operation should be called only once per
**            boot: subsequent calls must result in a no-op. When this function is
**            called, the crypto core must load its private keys and verify their
**            integrity. If there is a failure in the integrity check then the core must
**            return an error, and not carry out any further operations.
**
** Arguments:
**
**            [f_pBbxContext]:   The blackbox context.
**
** Returns:   DRM_SUCCESS on success.
**            DRM_E_OUTOFMEMORY
**
** Notes:     None.
**
**********************************************************************/
#define INIT_KEY_MAP_TABLE( keyType, KeyTable, PreloadedTable )                 \
    _InitKeyMapTable( keyType,                                                  \
                      &f_pBbxContext->keyHandleState,                           \
                      f_pBbxContext->keyHandleState.KeyTable,                   \
                      NO_OF( f_pBbxContext->keyHandleState.KeyTable ),          \
                      f_pBbxContext->keyHandleState.PreloadedTable,             \
                      NO_OF( f_pBbxContext->keyHandleState.PreloadedTable ) );  \

#define INIT_KEY_MAP_TABLE_NO_PRELOAD( keyType, KeyTable )                      \
    _InitKeyMapTable( keyType,                                                  \
                      &f_pBbxContext->keyHandleState,                           \
                      f_pBbxContext->keyHandleState.KeyTable,                   \
                      NO_OF( f_pBbxContext->keyHandleState.KeyTable ),          \
                      NULL,                                                     \
                      0 );                                                      \

DRM_API DRM_RESULT DRM_CALL _Hal_Initialize(
    __inout_ecount( 1 ) DRM_BB_CONTEXT *f_pBbxContext )
{
    DRM_RESULT dr = DRM_SUCCESS;

    AssertChkArg( !f_pBbxContext->fInited );

    INIT_KEY_MAP_TABLE_NO_PRELOAD( OEM_HAL_KEY_TYPE_ECC_P256_CONTENT_ENCRYPT,       rgKeyMapEccP256ContentEncrypt);
    INIT_KEY_MAP_TABLE_NO_PRELOAD( OEM_HAL_KEY_TYPE_ECC_P256_DOMAIN_ENCRYPT,        rgKeyMapEccP256DomainEncrypt);
    INIT_KEY_MAP_TABLE_NO_PRELOAD( OEM_HAL_KEY_TYPE_ECC_P256_MESSAGE_ENCRYPT,       rgKeyMapEccP256MessageEncrypt);
    INIT_KEY_MAP_TABLE_NO_PRELOAD( OEM_HAL_KEY_TYPE_ECC_P256_SIGN,                  rgKeyMapEccP256Sign);
    INIT_KEY_MAP_TABLE_NO_PRELOAD( OEM_HAL_KEY_TYPE_AES_ECB_ROOT,                   rgKeyMapAesEcbRoot);
    INIT_KEY_MAP_TABLE_NO_PRELOAD( OEM_HAL_KEY_TYPE_AES_CTR_CONTENT,                rgKeyMapAesCtrContent);
    INIT_KEY_MAP_TABLE_NO_PRELOAD( OEM_HAL_KEY_TYPE_AES_ECB_MESSAGE_ENCRYPT,        rgKeyMapAesEcbMessageEncrypt);
    INIT_KEY_MAP_TABLE_NO_PRELOAD( OEM_HAL_KEY_TYPE_AES_ECB_DOMAIN_PROTECT,         rgKeyMapAesEcbDomainProtect);
    INIT_KEY_MAP_TABLE_NO_PRELOAD( OEM_HAL_KEY_TYPE_AES_OMAC1_SECURE_STORE,         rgKeyMapAesOmac1SecureStore);
    INIT_KEY_MAP_TABLE_NO_PRELOAD( OEM_HAL_KEY_TYPE_AES_ECB_NEW_LICENSE_ROOT,       rgKeyMapAesEcbNewLicenseRoot);
    INIT_KEY_MAP_TABLE_NO_PRELOAD( OEM_HAL_KEY_TYPE_AES_CTR_NEW_LICENSE_CONTENT,    rgKeyMapAesCtrNewLicenseContent);
    INIT_KEY_MAP_TABLE_NO_PRELOAD( OEM_HAL_KEY_TYPE_AES_GENERIC_KEY_CONTENT,        rgKeyMapAesGenericKeyContent);
    INIT_KEY_MAP_TABLE_NO_PRELOAD( OEM_HAL_KEY_TYPE_AES_GENERIC_KEY_ROOT,           rgKeyMapAesGenericKeyRoot);
    INIT_KEY_MAP_TABLE_NO_PRELOAD( OEM_HAL_KEY_TYPE_LEGACY,                         rgKeyMapLegacy);

    if( DRM_BBX_IsSlkCryptoSupported() )
    {
        INIT_KEY_MAP_TABLE_NO_PRELOAD( OEM_HAL_KEY_TYPE_AES_SLK,              rgKeyMapSlk);
    }

    INIT_KEY_MAP_TABLE( OEM_HAL_KEY_TYPE_ECC_P256_PUBLIC,     rgKeyMapEccP256Public,    rgPreloadLookupMapEccP256Public );
    INIT_KEY_MAP_TABLE( OEM_HAL_KEY_TYPE_ECC_P256_MODEL_SIGN, rgKeyMapEccP256ModelSign, rgPreloadLookupMapEccP256ModelSign );
    INIT_KEY_MAP_TABLE( OEM_HAL_KEY_TYPE_AES_KEYFILE,         rgKeyMapAesKeyFile,       rgPreloadLookupMapAesKeyFile );

    ChkDR( Oem_Hal_Initialize() );

ErrorExit:
    return dr;
}


/*********************************************************************
**
** Function:  _Hal_Deinitialize
**
** Synopsis:  This function deinitializes the cryptocore, leaving it unable
**            to used further. This should be doneonly as part of an orderly
**            shutdown of the platform.
**
** Arguments:
**
**            [f_pBbxContext]:   The blackbox context.
**
** Returns:   DRM_SUCCESS on success.
**            DRM_E_OEMHAL_CRYPTO_NOT_INITIALIZED
**
** Notes:     None.
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL _Hal_Deinitialize(
    __inout_ecount( 1 ) DRM_BB_CONTEXT *f_pBbxContext )
{
    DRM_RESULT dr               = DRM_SUCCESS;
    DRM_BOOL   fKeysStillActive = FALSE;
    DRM_LONG   iType            = 0;

    /*
    ** Clean up any allocated keys
    */
    for( iType = 0; iType < ( DRM_LONG )OEM_HAL_KEY_TYPE_NUM_VALID_TYPES; iType++ )
    {
        DRM_BB_KEY_REGISTER_TABLE* pTable = &f_pBbxContext->keyHandleState.rgKeyMapTables[ iType ];
        DRM_LONG                   iEntry = 0;

        DRMASSERT( iType >= 0 && iType < NO_OF( f_pBbxContext->keyHandleState.rgKeyMapTables ) );

        for( iEntry = 0; iEntry < pTable->cEntries; iEntry++ )
        {
            DRM_BB_KEY_REGISTER_ENTRY *pEntry = &pTable->pEntries[ iEntry ];

            /*
            ** If this key register has been allocated, release it
            */
            if( pEntry->eState != eDRM_BB_KEY_REGISTER_STATE_UNALLOCATED )
            {
                /*
                ** Something has gone wrong: a key is still locked for use by the client code.
                ** Continue deallocating, but return an error
                */
                if( pEntry->eState == eDRM_BB_KEY_REGISTER_STATE_IN_USE )
                {
                    DRMASSERT( FALSE );
                    fKeysStillActive = TRUE;
                }

                ChkDR( Oem_Hal_FreeRegister( ( OEM_HAL_KEY_TYPE )iType, pEntry->indexKeyRegister ) );

                pEntry->eState           = eDRM_BB_KEY_REGISTER_STATE_UNALLOCATED;
                pEntry->indexKeyRegister = OEM_HAL_KEY_REGISTER_INDEX_INVALID;

                if( _Hal_IsHandleCachingSupported() )
                {
                    pEntry->cbCacheKey                   = 0;
                    pEntry->dwUnwrapTimestamp            = 0;
                    OEM_SECURE_ZERO_MEMORY( pEntry->pbCacheKey, DRM_BB_MAX_CACHE_KEY_SIZE_IN_BYTES );
                }
            }
        }
    }

    ChkDR( Oem_Hal_Deinitialize() );

    if( fKeysStillActive )
    {
        ChkDR( DRM_E_OEMHAL_KEYS_IN_USE );
    }

ErrorExit:
    return dr;
}


/*********************************************************************
**
** Function:   _Hal_ReleaseKey
**
** Synopsis:   Release a previously allocated key
**
** Arguments:
**
** [f_pBbxContext]: The blackbox context.
** [f_hKey]:        The handle of the key to release.
**
** Returns:    DRM_SUCCESS on success.
**             DRM_E_HANDLE if the handle is invalid.
**
** Notes:      None.
**
**********************************************************************/
PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_NONCONST_PARAM_25004, "Prefast Noise: DRM_BB_CONTEXT should never be const")
DRM_API DRM_RESULT DRM_CALL _Hal_ReleaseKey(
    __inout_ecount( 1 ) DRM_BB_CONTEXT    *f_pBbxContext,
    __in                OEM_CRYPTO_HANDLE  f_hKey )
PREFAST_POP /* __WARNING_NONCONST_PARAM_25004 */
{
    DRM_RESULT                 dr            = DRM_SUCCESS;
    DRM_BB_KEY_REGISTER_ENTRY *pEntry        = NULL;
    DRM_BOOL                   fReleaseEntry = FALSE;

    ChkDR( _ResolveKeyHandle( f_pBbxContext, f_hKey, &pEntry ) );

    if( pEntry->eState == eDRM_BB_KEY_REGISTER_STATE_UNASSIGNED )
    {
        DRMASSERT( pEntry->dwRefCount == 0 );

        fReleaseEntry = TRUE;
    }
    else if( pEntry->eState == eDRM_BB_KEY_REGISTER_STATE_IN_USE )
    {
        DRMASSERT( pEntry->dwRefCount > 0 );

        pEntry->dwRefCount--;

        if( pEntry->dwRefCount == 0 )
        {
            if( pEntry->cbCacheKey > 0 )
            {
                pEntry->eState = eDRM_BB_KEY_REGISTER_STATE_UNASSIGNED;
            }
            else
            {
                fReleaseEntry  = TRUE;
            }
#ifdef TEE
            if (pEntry->eType == OEM_HAL_KEY_TYPE_AES_CTR_CONTENT) {
                DRM_BOOL secureStopOn = FALSE;
                ChkDR(TEE_IsSecureStopOn(&secureStopOn));
                if (secureStopOn) {
                    ChkDR(TEE_StopSession(pEntry->indexKeyRegister));
                }  //
            }  //
#endif
		}
    }

    if( fReleaseEntry )
    {
        ChkDR( Oem_Hal_FreeRegister( pEntry->eType, pEntry->indexKeyRegister ) );

        pEntry->eState           = eDRM_BB_KEY_REGISTER_STATE_UNALLOCATED;
        pEntry->indexKeyRegister = OEM_HAL_KEY_REGISTER_INDEX_INVALID;

        if( _Hal_IsHandleCachingSupported() )
        {
            OEM_SECURE_ZERO_MEMORY( pEntry->pbCacheKey, DRM_BB_MAX_CACHE_KEY_SIZE_IN_BYTES );
            pEntry->cbCacheKey        = 0;
            pEntry->dwUnwrapTimestamp = 0;
        }
    }

ErrorExit:
    return dr;
}


/*********************************************************************
**
** Function:   _Hal_DuplicateKey
**
** Synopsis:   Copy a crypto handle, incrementing the reference count
**
** Arguments:
**
** [f_pBbxContext]: The blackbox context.
** [f_hKey]:        The handle of the key to duplicate.
** [f_phDuplicate]: The duplicate handle.
**
** Returns:    DRM_SUCCESS on success.
**             DRM_E_HANDLE if the handle is invalid.
**
** Notes:      None.
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL _Hal_DuplicateKey(
    __inout_ecount( 1 ) DRM_BB_CONTEXT    *f_pBbxContext,
    __in                OEM_CRYPTO_HANDLE  f_hKey,
    __out_ecount( 1 )   OEM_CRYPTO_HANDLE *f_phDuplicate )
{
    DRM_RESULT                 dr     = DRM_SUCCESS;
    DRM_BB_KEY_REGISTER_ENTRY *pEntry = NULL;

    ChkDR( _ResolveKeyHandle( f_pBbxContext, f_hKey, &pEntry ) );

    pEntry->dwRefCount++;

    /*
    ** Simply return the same handle with an additional reference count
    */
    *f_phDuplicate = f_hKey;

ErrorExit:
    return dr;
}


/*********************************************************************
**
** Function:   TEST_Hal_GetKeyRegisterIndex
**
** Synopsis:   Return the register index of a key handle
**
** Arguments:
**
** [f_pBbxContext]: The blackbox context.
** [f_hKey]:        The handle of the key.
**
** Returns:    register index on success.
**             OEM_HAL_KEY_REGISTER_INDEX_INVALID if the handle is invalid.
**
** Notes:      Test function - resolving handles to registers and register
**             types in this way is inefficient and should not be used by
**             production code.
**
**********************************************************************/
DRM_API OEM_HAL_KEY_REGISTER_INDEX DRM_CALL TEST_Hal_GetKeyRegisterIndex(
    __inout_ecount( 1 ) DRM_BB_CONTEXT    *f_pBbxContext,
    __in                OEM_CRYPTO_HANDLE  f_hKey )
{
    DRM_RESULT                 dr     = DRM_SUCCESS;
    OEM_HAL_KEY_REGISTER_INDEX index  = OEM_HAL_KEY_REGISTER_INDEX_INVALID;
    DRM_BB_KEY_REGISTER_ENTRY *pEntry = NULL;

    ChkDR( _ResolveKeyHandle( f_pBbxContext, f_hKey, &pEntry ) );

    index = pEntry->indexKeyRegister;

ErrorExit:
    return index;
}

/*********************************************************************
**
** Function:   TEST_Hal_PurgeKeyHandleCache
**
** Synopsis:   Release cached key register entries of specified type.
**
** Arguments:
**
** [f_pBbxContext]: The blackbox context.
** [f_eType]:       The type of the cached key entries to be released.
**
** Returns:    DRM_SUCCESS on success.
**             DRM_E_INVALIDARG if f_pBbxContext or f_eType is invalid.
**
** Notes:      None.
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL TEST_Hal_PurgeKeyHandleCache(
    __inout_ecount( 1 )     DRM_BB_CONTEXT    *f_pBbxContext,
    __in                    OEM_HAL_KEY_TYPE   f_eType )
{
    DRM_RESULT                 dr     = DRM_SUCCESS;
    DRM_BB_KEY_REGISTER_TABLE *pTable = NULL;
    DRM_BB_KEY_REGISTER_ENTRY *pEntry = NULL;
    DRM_LONG                   iEntry = 0;

    ChkPtr( f_pBbxContext );
    ChkBOOL( DRM_BBX_IsSlkCryptoSupported()    || f_eType != OEM_HAL_KEY_TYPE_AES_SLK, DRM_E_NOTIMPL );
    ChkArg( f_eType > OEM_HAL_KEY_TYPE_INVALID );
    ChkArg( f_eType < OEM_HAL_KEY_TYPE_NUM_VALID_TYPES );

    DRMASSERT( f_eType >= 0 && f_eType < NO_OF( f_pBbxContext->keyHandleState.rgKeyMapTables ) );
    pTable = &f_pBbxContext->keyHandleState.rgKeyMapTables[ f_eType ];

    for( iEntry = 0; iEntry < pTable->cEntries; iEntry++ )
    {
        pEntry = &pTable->pEntries[ iEntry ];

        if( pEntry->eState == eDRM_BB_KEY_REGISTER_STATE_UNASSIGNED )
        {
            ChkDR( Oem_Hal_FreeRegister( f_eType, pEntry->indexKeyRegister ) );

            pEntry->indexKeyRegister  = OEM_HAL_KEY_REGISTER_INDEX_INVALID;
            pEntry->eState            = eDRM_BB_KEY_REGISTER_STATE_UNALLOCATED;
            pEntry->cbCacheKey        = 0;
            pEntry->dwUnwrapTimestamp = 0;
            OEM_SECURE_ZERO_MEMORY( pEntry->pbCacheKey, DRM_BB_MAX_CACHE_KEY_SIZE_IN_BYTES );
        }
    }

ErrorExit:
    return dr;
}

/*********************************************************************
**
** Function:   _Hal_GetKeyType
**
** Synopsis:   Return the register type of a key handle
**
** Arguments:
**
** [f_pBbxContext]: The blackbox context.
** [f_hKey]:        The handle of the key.
**
** Returns:    register type on success.
**             OEM_HAL_KEY_TYPE_INVALID if the handle is invalid.
**
** Notes:
**
**********************************************************************/
DRM_API OEM_HAL_KEY_TYPE DRM_CALL _Hal_GetKeyType(
    __inout_ecount( 1 ) DRM_BB_CONTEXT    *f_pBbxContext,
    __in                OEM_CRYPTO_HANDLE  f_hKey )
{
    DRM_RESULT                 dr     = DRM_SUCCESS;
    OEM_HAL_KEY_TYPE           eType  = OEM_HAL_KEY_TYPE_INVALID;
    DRM_BB_KEY_REGISTER_ENTRY *pEntry = NULL;

    ChkDR( _ResolveKeyHandle( f_pBbxContext, f_hKey, &pEntry ) );

    eType = pEntry->eType;

ErrorExit:
    return eType;
}


/*********************************************************************
**
** Function:    _Hal_GetPreloadedKey
**
** Synopsis:    Return the handle to a preloaded key with the specified key ID.
**
** Arguments:
**
** [f_pBbxContext]: The blackbox context.
** [f_eType]:       The type of the key to return.
** [f_pguidKeyId]:  The unique ID of the preloaded key.
** [f_phKey]:       The handle of the preloaded key.
**
** Returns:     DRM_SUCCESS on success.
**              DRM_E_INVALIDARG if f_eType is invalid or keys of this type cannot be preloaded.
**              DRM_E_POINTER if f_phKey are NULL.
**              IPTV_HAL_ERROR_CRYPTO_NO_KEY if the requested key doesn't exist.
**
** Notes:      None.
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL _Hal_GetPreloadedKey(
    __inout_ecount( 1 )     DRM_BB_CONTEXT    *f_pBbxContext,
    __in                    OEM_HAL_KEY_TYPE   f_eType,
    __in_ecount( 1 )  const DRM_GUID          *f_pguidKeyId,
    __out_ecount( 1 )       OEM_CRYPTO_HANDLE *f_phKey )
{
    DRM_RESULT                 dr     = DRM_SUCCESS;
    DRM_BB_KEY_REGISTER_TABLE *pTable = NULL;
    DRM_LONG                   i      = 0;

    ChkPtr( f_pBbxContext );
    ChkBOOL( DRM_BBX_IsSlkCryptoSupported()    || f_eType != OEM_HAL_KEY_TYPE_AES_SLK, DRM_E_NOTIMPL );
    ChkArg( f_eType > OEM_HAL_KEY_TYPE_INVALID );
    ChkArg( f_eType < OEM_HAL_KEY_TYPE_NUM_VALID_TYPES );
    ChkPtr( f_pguidKeyId );
    ChkPtr( f_phKey );

    *f_phKey = NULL;

    /*
    ** First, see if the key already exists
    */
    DRMASSERT( f_eType >= 0 && f_eType < NO_OF( f_pBbxContext->keyHandleState.rgKeyMapTables ) );
    pTable = &f_pBbxContext->keyHandleState.rgKeyMapTables[ f_eType ];

    /*
    ** Check whether preloading of keys of this type is supported.
    */
    ChkArg( pTable->pPreloadedIds != NULL );

    for( i = 0; i < pTable->cEntries && *f_phKey == NULL; i++ )
    {
        DRM_LONG nPreloadedIndex = pTable->pEntries[ i ].indexPreloadedTable;

        if( pTable->pEntries[ i ].eState == eDRM_BB_KEY_REGISTER_STATE_PRELOADED &&
            nPreloadedIndex              != -1 )
        {
            if( MEMCMP( f_pguidKeyId, &pTable->pPreloadedIds[nPreloadedIndex], SIZEOF( DRM_GUID ) ) == 0 )
            {
                /*
                ** This key has already been requested, return the handle again
                */
                *f_phKey = ( OEM_CRYPTO_HANDLE )&pTable->pEntries[ i ];
            }
        }
    }

    /*
    ** If the key hasn't already been requested, do so now
    */
    if( *f_phKey == NULL )
    {
        OEM_HAL_KEY_REGISTER_INDEX  indexKey      = OEM_HAL_KEY_REGISTER_INDEX_INVALID;
        OEM_CRYPTO_HANDLE           hKey          = OEM_CRYPTO_HANDLE_INVALID;
        DRM_BB_KEY_REGISTER_ENTRY  *pEntry        = NULL;
        DRM_LONG                    nPreloadIndex = -1;

        ChkDR( Oem_Hal_GetPreloadedIndex( f_pBbxContext->pOEMContext,
                                          f_eType,
                                          ( DRM_BYTE* )f_pguidKeyId,
                                          SIZEOF( DRM_GUID ),
                                          &indexKey ) );

        ChkDR( _AllocatePreloadedKeyHandle( f_pBbxContext,
                                            f_eType,
                                            &hKey,
                                            &pEntry,
                                            &nPreloadIndex ) );

        /*
        ** Store the key ID in the preload lookup table
        */
        MEMCPY( &pTable->pPreloadedIds[nPreloadIndex], f_pguidKeyId, SIZEOF( DRM_GUID ) );

        pEntry->indexKeyRegister    = indexKey;
        pEntry->indexPreloadedTable = nPreloadIndex;

        *f_phKey = hKey;
    }

ErrorExit:
    return dr;
}


/*********************************************************************
**
** Function:    _Hal_VerifyMessageSignature
**
** Synopsis:    This function verifies the asymmetric signature of the supplied
**              message using the validated public key contained in f_hIntegrityKey.
**
** Arguments:
**
** [f_pBbxContext]:      The blackbox context.
** [f_rgbMessage]:       The message whose signature is to be verified.
** [f_cbMessage]:        The length (in bytes) of the message.
** [f_rgbSignature]:     The signature to be verified.
** [f_cbSignature]:      The length (in  bytes) of the signature.
** [f_eHashType]:        The hash algorithm to be used.
** [f_eSignatureScheme]: The signature scheme (including padding type) to be used.
** [f_hIntegrityKey]:    The handle of the key to be used for verification of the message.
**
** Returns:     DRM_SUCCESS on success.
**              DRM_E_INVALIDARG if f_eHashType or f_eSignatureScheme is invalid.
**              DRM_E_HANDLE if f_hIntegrityKey is invalid.
**              DRM_E_POINTER if f_rgbMessage or f_rgbSignature are NULL.
**              DRM_E_OEMHAL_CRYPTO_BAD_MESSAGE_LENGTH
**              DRM_E_OEMHAL_CRYPTO_BUFFER_TOO_LARGE
**              DRM_E_BUFFERTOOSMALL
**              DRM_E_OEMHAL_CRYPTO_NOT_INITIALIZED
**              DRM_E_OUTOFMEMORY
**              DRM_E_OEMHAL_CRYPTO_SIGNATURE_CHECK_FAILED
**              DRM_E_OEMHAL_CRYPTO_UNSUPPORTED_HASH_TYPE
**              DRM_E_OEMHAL_CRYPTO_UNSUPPORTED_SIGNATURE_SCHEME
**
** Notes:      None.
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL _Hal_VerifyMessageSignature(
    __inout_ecount( 1 )                DRM_BB_CONTEXT            *f_pBbxContext,
    __in_bcount( f_cbMessage )   const DRM_BYTE                  *f_rgbMessage,
    __in                               DRM_DWORD                  f_cbMessage,
    __in_bcount( f_cbSignature ) const DRM_BYTE                  *f_rgbSignature,
    __in                               DRM_DWORD                  f_cbSignature,
    __in                               OEM_HAL_HASH_TYPE          f_eHashType,
    __in                               OEM_HAL_SIGNATURE_SCHEME   f_eSignatureScheme,
    __in                               OEM_CRYPTO_HANDLE          f_hIntegrityKey )
{
    DRM_RESULT                 dr     = DRM_SUCCESS;
    DRM_BB_KEY_REGISTER_ENTRY *pEntry = NULL;

    ChkDR( _ResolveKeyHandle( f_pBbxContext, f_hIntegrityKey, &pEntry ) );

    ChkDR( Oem_Hal_VerifyMessageSignature( f_rgbMessage,
                                           f_cbMessage,
                                           f_rgbSignature,
                                           f_cbSignature,
                                           f_eHashType,
                                           f_eSignatureScheme,
                                           pEntry->indexKeyRegister ) );

ErrorExit:
    return dr;
}


/*********************************************************************
**
** Function:    _Hal_SignWithPrivateKey
**
** Synopsis:    This function creates a digital signature for a message/cert.
**              If the signature scheme is OEM_HAL_SIGNATURE_SCHEME_RSASSA_PKCS1_v1_5
**              the signature is created using NonAV_Pk. If the scheme is
**              OEM_HAL_SIGNATURE_SCHEME_ECDSA_P256 the signature is created
**              using the device's PlayReady signing key OR the device's
**              model certificate private signing key (for cert generation).
**              If the signature buffer passed in is null or if the buffer
**              is too small to store the signature, the actual buffer
**              length needed is returned in signatureLength.
**              Note that OEM_HAL_SIGNATURE_SCHEME_RSASSA_PKCS1_v1_5 is not
**              supported in the default PK implementation.
**
** Arguments:
**
** [f_pBbxContext]:         The blackbox context.
** [f_rgbMessage]:          The message to be signed.
**                          If the model certificate private key is being used,
**                          this must contain an unsigned certificate where the
**                          public keys in that certificate were generated inside
**                          the HAL with appropriate key usages.
** [f_cbMessage]:           The length (in bytes) of the message.
** [f_rgbSignature]:        The buffer that is to hold the signature.
** [f_pcbSignature]:        A pointer to a location holding the length (in bytes)
**                          of the signature buffer. If the buffer is too small to contain
**                          the signature, the function returns an error indicating that,
**                          and updates the location with the buffer size required.
** [f_eHashType]:           The hash algorithm to be used.
** [f_eSignatureScheme]:    The signature scheme (including padding type) to be used.
** [f_hIntegrityKey]:       The handle of the key to be used for signing the message.
**
** Returns:     DRM_SUCCESS on success.
**              DRM_E_HANDLE if f_hKey is invalid.
**              DRM_E_POINTER if f_rgbData or f_rgbSignature are NULL.
**              DRM_E_OEMHAL_CRYPTO_BUFFER_TOO_LARGE
**              DRM_E_BUFFERTOOSMALL
**              DRM_E_OEMHAL_CRYPTO_NOT_INITIALIZED
**              DRM_E_OUTOFMEMORY
**              DRM_E_OEMHAL_UNSUPPORTED_HASH_TYPE
**              DRM_E_OEMHAL_UNSUPPORTED_SIGNATURE_SCHEME
**
** Notes:      None.
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL _Hal_SignWithPrivateKey(
    __inout_ecount( 1 )                 DRM_BB_CONTEXT            *f_pBbxContext,
    __in_bcount( f_cbMessage )    const DRM_BYTE                  *f_rgbMessage,
    __in                                DRM_DWORD                  f_cbMessage,
    __out_bcount_opt( *f_pcbSignature ) DRM_BYTE                  *f_rgbSignature,
    __inout                             DRM_DWORD                 *f_pcbSignature,
    __in                                OEM_HAL_HASH_TYPE          f_eHashType,
    __in                                OEM_HAL_SIGNATURE_SCHEME   f_eSignatureScheme,
    __in                                OEM_CRYPTO_HANDLE          f_hIntegrityKey )
{
    DRM_RESULT                 dr     = DRM_SUCCESS;
    DRM_BB_KEY_REGISTER_ENTRY *pEntry = NULL;


    ChkDR( _ResolveKeyHandle( f_pBbxContext, f_hIntegrityKey, &pEntry ) );


    ChkDR( Oem_Hal_SignWithPrivateKey( &f_pBbxContext->pOEMContext,
                                       USE_SECURE_HAL_CRYPTO_CONTEXT,
                                       f_rgbMessage,
                                       f_cbMessage,
                                       f_rgbSignature,
                                       f_pcbSignature,
                                       f_eHashType,
                                       f_eSignatureScheme,
                                       pEntry->eType,
                                       pEntry->indexKeyRegister ) );


ErrorExit:

    return dr;
}


/*********************************************************************
**
** Function:    _Hal_VerifyOMAC1Signature
**
** Synopsis:    This function verifies the signature of the supplied data
**              blob using the AES-OMAC1 algorithm with the indicated key.
**
** Arguments:
**
** [f_pBbxContext]:      The blackbox context.
** [f_rgbData]:          The data blob whose signature is to be verified.
** [f_cbData]:           The length (in bytes) of the data blob.
** [f_rgbSignature]:     The signature to be verified.
** [f_cbSignature]:      The length (in  bytes) of the signature.
** [f_hKey]:             The handle of the key to be used for the AES OMAC1 signature
**
** Returns:     DRM_SUCCESS on success.
**              DRM_E_HANDLE if f_hKey is invalid.
**              DRM_E_POINTER if f_rgbData or f_rgbSignature are NULL.
**              DRM_E_OEMHAL_CRYPTO_BUFFER_TOO_LARGE
**              DRM_E_BUFFERTOOSMALL
**              DRM_E_OEMHAL_CRYPTO_NOT_INITIALIZED
**              DRM_E_OUTOFMEMORY
**              DRM_E_OEMHAL_CRYPTO_SIGNATURE_CHECK_FAILED
**              DRM_E_OEMHAL_CRYPTO_UNSUPPORTED_KEY_TYPE
**
** Notes:      None.
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL _Hal_VerifyOMAC1Signature(
    __inout_ecount( 1 )                DRM_BB_CONTEXT    *f_pBbxContext,
    __in_bcount( f_cbData )      const DRM_BYTE          *f_rgbData,
    __in                               DRM_DWORD          f_cbData,
    __in_bcount( f_cbSignature ) const DRM_BYTE          *f_rgbSignature,
    __in                               DRM_DWORD          f_cbSignature,
    __in                               OEM_CRYPTO_HANDLE  f_hKey )
{
    DRM_RESULT                 dr     = DRM_SUCCESS;
    DRM_BB_KEY_REGISTER_ENTRY *pEntry = NULL;

    ChkDR( _ResolveKeyHandle( f_pBbxContext, f_hKey, &pEntry ) );

    ChkDR( Oem_Hal_VerifyOMAC1Signature( f_rgbData,
                                         f_cbData,
                                         f_rgbSignature,
                                         f_cbSignature,
                                         pEntry->eType,
                                         pEntry->indexKeyRegister ) );

ErrorExit:
    return dr;
}


/*********************************************************************
**
** Function:    _Hal_CreateOMAC1Signature
**
** Synopsis:    This function creates the signature of the supplied data
**              blob using the AES-OMAC1 algorithm with the indicated key.
**              This is only  permitted for preloaded keys. If the signature
**              buffer passed in is null or if the buffer is too small to
**              store the signature, the actual buffer  length needed is
**              returned in signatureLength.
**
** Arguments:
**
** [f_pBbxContext]:  The blackbox context.
** [f_rgbData]:      The data blob whose signature is to be verified.
** [f_cbData]:       The length (in bytes) of the data blob.
** [f_rgbSignature]: The signature to be verified.
** [f_pcbSignature]: The length (in  bytes) of the signature.
** [f_hKey]:         The handle of the key to be used for the AES OMAC1 signature
**
** Returns:     DRM_SUCCESS on success.
**              DRM_E_HANDLE if f_hKey is invalid.
**              DRM_E_POINTER if f_rgbData, f_rgbSignature or f_pcbSignature are NULL.
**              DRM_E_OEMHAL_CRYPTO_BUFFER_TOO_LARGE
**              DRM_E_BUFFERTOOSMALL
**              DRM_E_OEMHAL_CRYPTO_NOT_INITIALIZED
**              DRM_E_OUTOFMEMORY
**              DRM_E_OEMHAL_CRYPTO_UNSUPPORTED_KEY_TYPE
**
** Notes:      None.
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL _Hal_CreateOMAC1Signature(
    __inout_ecount( 1 )                 DRM_BB_CONTEXT    *f_pBbxContext,
    __in_bcount( f_cbData )       const DRM_BYTE          *f_rgbData,
    __in                                DRM_DWORD          f_cbData,
    __out_bcount_opt( *f_pcbSignature ) DRM_BYTE          *f_rgbSignature,
    __inout                             DRM_DWORD         *f_pcbSignature,
    __in                                OEM_CRYPTO_HANDLE  f_hKey )
{
    DRM_RESULT                 dr     = DRM_SUCCESS;
    DRM_BB_KEY_REGISTER_ENTRY *pEntry = NULL;

    ChkDR( _ResolveKeyHandle( f_pBbxContext, f_hKey, &pEntry ) );

    ChkDR( Oem_Hal_CreateOMAC1Signature( f_rgbData,
                                         f_cbData,
                                         f_rgbSignature,
                                         f_pcbSignature,
                                         pEntry->eType,
                                         pEntry->indexKeyRegister ) );

ErrorExit:
    return dr;
}


/*********************************************************************
**
** Function:    _Hal_UnwrapKey
**
** Synopsis:    This function unwraps a key, wrapped using another key,
**              and loads it into a key register.
**
** Arguments:
**
** [f_pBbxContext]:      The blackbox context.
** [f_eKeyType]:         The type of key to be unwrapped.
** [f_hWrappingKey]:     The handle of the wrapping key.
** [f_pbWrappedKeyData]: A pointer to the buffer containing the data required
**                       to unpack the wrapped key (this is key dependent).
** [f_cbWrappedKeyData]: The length (in bytes) of the wrappedKey data.
** [f_pbParameterData]:  Parameter data associated with the particular unwrap type.
** [f_cbParameterData]:  Size of parameter data.
** [f_pbCacheKey]:       Cache key associated with the key handle.
** [f_cbCacheKey]:       Size of f_pbCacheKey in bytes.
** [f_hKey]:             The handle of the unwrapped key.
**
** Returns:     DRM_SUCCESS on success.
**              DRM_E_HANDLE if f_hKey is invalid.
**              DRM_E_INVALIDARG if f_eKeyType is invalid.
**              DRM_E_POINTER if f_pbWrappedKeyData are NULL.
**              DRM_E_OEMHAL_CRYPTO_NOT_INITIALIZED
**              DRM_E_OEMHAL_CRYPTO_UNSUPPORTED_KEY_TYPE
**              DRM_E_OEMHAL_CRYPTO_UNSUPPORTED_KEY_LENGTH
**              DRM_E_OEMHAL_CRYPTO_UNSUPPORTED_KEY_WRAPPING_FORMAT
**
** Notes:      after this function is successfully called, a call must be made to
**             _Hal_ReleaseKey to free resources associated with the key.
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL _Hal_UnwrapKey(
    __inout                                 DRM_BB_CONTEXT     *f_pBbxContext,
    __in                                    OEM_HAL_KEY_TYPE    f_eKeyType,
    __in                                    OEM_CRYPTO_HANDLE   f_hWrappingKey,
    __in_bcount( f_cbWrappedKeyData ) const DRM_BYTE           *f_pbWrappedKeyData,
    __in                                    DRM_DWORD           f_cbWrappedKeyData,
    __in_bcount( f_cbParameterData )  const DRM_BYTE           *f_pbParameterData,
    __in                                    DRM_DWORD           f_cbParameterData,
    __in_bcount( f_cbCacheKey )       const DRM_BYTE           *f_pbCacheKey,
    __in                              const DRM_DWORD           f_cbCacheKey,
    __out_ecount( 1 )                       OEM_CRYPTO_HANDLE  *f_phKey )
{
    DRM_RESULT                  dr               = DRM_SUCCESS;
    OEM_CRYPTO_HANDLE           hKey             = OEM_CRYPTO_HANDLE_INVALID;
    DRM_BB_KEY_REGISTER_ENTRY  *pKeyEntry        = NULL;
    OEM_HAL_KEY_REGISTER_INDEX  indexWrappingKey = OEM_HAL_KEY_REGISTER_INDEX_INVALID;
    OEM_HAL_KEY_TYPE            eWrappingKeyType = OEM_HAL_KEY_TYPE_INVALID;
    DRM_BOOL                    fCachedHandle     = FALSE;

    ChkPtr( f_pBbxContext );

    ChkBOOL( f_eKeyType != OEM_HAL_KEY_TYPE_INVALID, DRM_E_OEMHAL_UNSUPPORTED_KEY_TYPE );

    /*
    ** Allocate a new key slot for the unwrapped key
    */
    ChkDR( _Hal_AllocateKeyHandle( f_pBbxContext, f_eKeyType, f_pbCacheKey, f_cbCacheKey, &hKey, &pKeyEntry, &fCachedHandle ) );

    ChkPtr( pKeyEntry );

    if( !fCachedHandle )
    {
        /*
        ** Resolve the wrapping key
        */
        /*
        ** Allow NULL wrapping handles - their validity will be checked
        ** before they are used
        */
        if( f_hWrappingKey != OEM_CRYPTO_HANDLE_INVALID )
        {
            DRM_BB_KEY_REGISTER_ENTRY *pWrappingEntry = NULL;

            /*
            ** Resolve the wrapping key
            */
            ChkDR( _ResolveKeyHandle( f_pBbxContext, f_hWrappingKey, &pWrappingEntry ) );

            indexWrappingKey = pWrappingEntry->indexKeyRegister;
            eWrappingKeyType = pWrappingEntry->eType;
        }

        ChkDR( Oem_Hal_UnwrapKey( USE_SECURE_HAL_CRYPTO_CONTEXT,
                                  f_eKeyType,
                                  pKeyEntry->indexKeyRegister,
                                  eWrappingKeyType,
                                  indexWrappingKey,
                                  f_pbWrappedKeyData,
                                  f_cbWrappedKeyData,
                                  f_pbParameterData,
                                  f_cbParameterData ) );

        if( f_cbCacheKey > 0)
        {
            DRM_BYT_CopyBytes( pKeyEntry->pbCacheKey, 0, f_pbCacheKey, 0, f_cbCacheKey );
            pKeyEntry->cbCacheKey = f_cbCacheKey;
        }
        else
        {
            OEM_SECURE_ZERO_MEMORY( pKeyEntry->pbCacheKey, DRM_BB_MAX_CACHE_KEY_SIZE_IN_BYTES );
            pKeyEntry->cbCacheKey = 0;
        }
    }

    if( _Hal_IsHandleCachingSupported() )
    {
        pKeyEntry->dwUnwrapTimestamp = Oem_Clock_GetTickCount( NULL );
    }

    *f_phKey = hKey;
    hKey     = OEM_CRYPTO_HANDLE_INVALID;

ErrorExit:
    DRM_BBX_SafeReleaseKey( f_pBbxContext, &hKey );
    return dr;
}


/*********************************************************************
**
** Function:    _Hal_WrapKey
**
** Synopsis:    This function wraps a key (in a key register) with a wrapping
**              key (typically also in a key  register) and places the wrapped
**              key in a buffer.
**
** Arguments:
**
** [f_pBbxContext]:    The blackbox context.
** [f_hKey]:           The handle of the key to be wrapped.
** [f_hWrappingKey]:   The handle of the wrapping key.
** [f_pbWrappedKey]:   A pointer to the buffer to contain the wrapped key.
**                     If the buffer is too small to contain the wrapped key, the
**                     function returns an error indicating that, and updates the
**                     location with the buffer size required.
** [f_pcbWrappedKey]:  A pointer to a location holding the length (in bytes)
**                     of the wrappedKey buffer. On input, this parameter holds the
**                     length of the wrappedKey buffer. On output, this parameter
**                     indicates the number of bytes written to the wrappedKey buffer.
**
** Returns:     DRM_SUCCESS on success.
**              DRM_E_HANDLE if f_hKey is invalid.
**              DRM_E_POINTER if f_pbWrappedKeyData or f_pcbWrappedKey are NULL.
**              DRM_E_OEMHAL_NOT_INITIALIZED
**              DRM_E_OEMHAL_UNSUPPORTED_KEY_TYPE
**              DRM_E_OEMHAL_UNSUPPORTED_KEY_WRAPPING_FORMAT
**              DRM_E_BUFFERTOOSMALL
**
** Notes:      None.
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL _Hal_WrapKey(
    __inout_ecount( 1 )                  DRM_BB_CONTEXT     *f_pBbxContext,
    __in                                 OEM_CRYPTO_HANDLE   f_hKey,
    __in                                 OEM_CRYPTO_HANDLE   f_hWrappingKey,
    __out_bcount_opt( *f_pcbWrappedKey ) DRM_BYTE           *f_pbWrappedKey,
    __inout_ecount(1)                    DRM_DWORD          *f_pcbWrappedKey )
{
    DRM_RESULT                  dr               = DRM_SUCCESS;
    DRM_BB_KEY_REGISTER_ENTRY  *pKeyEntry        = NULL;
    OEM_HAL_KEY_REGISTER_INDEX  indexWrappingKey = OEM_HAL_KEY_REGISTER_INDEX_INVALID;
    OEM_HAL_KEY_TYPE            eWrappingKeyType = OEM_HAL_KEY_TYPE_INVALID;

    ChkPtr( f_pBbxContext );

    /*
    ** Resolve the key to wrap
    */
    ChkBOOL( f_hKey != OEM_CRYPTO_HANDLE_INVALID, DRM_E_OEMHAL_UNSUPPORTED_KEY_TYPE );
    ChkDR( _ResolveKeyHandle( f_pBbxContext, f_hKey, &pKeyEntry ) );

    /*
    ** Allow NULL wrapping handles - their validity will be checked
    ** before they are used
    */
    if( f_hWrappingKey != OEM_CRYPTO_HANDLE_INVALID )
    {
        DRM_BB_KEY_REGISTER_ENTRY *pWrappingEntry = NULL;

        /*
        ** Resolve the wrapping key
        */
        ChkDR( _ResolveKeyHandle( f_pBbxContext, f_hWrappingKey, &pWrappingEntry ) );

        indexWrappingKey = pWrappingEntry->indexKeyRegister;
        eWrappingKeyType = pWrappingEntry->eType;
    }

#if !DRM_SUPPORT_SECUREOEMHAL /* If we have an insecure hal we need to check the SymOpt feature and key pairings here */
    if( !DRM_SYMOPT_IsSymOptSupported() )
    {
        ChkBOOL( !( eWrappingKeyType == OEM_HAL_KEY_TYPE_ECC_P256_CONTENT_ENCRYPT && pKeyEntry->eType == OEM_HAL_KEY_TYPE_AES_ECB_ROOT    ), DRM_E_OEMHAL_UNSUPPORTED_KEY_WRAPPING_FORMAT );
        ChkBOOL( !( eWrappingKeyType == OEM_HAL_KEY_TYPE_ECC_P256_CONTENT_ENCRYPT && pKeyEntry->eType == OEM_HAL_KEY_TYPE_AES_CTR_CONTENT ), DRM_E_OEMHAL_UNSUPPORTED_KEY_WRAPPING_FORMAT );
        ChkBOOL( !( eWrappingKeyType == OEM_HAL_KEY_TYPE_ECC_P256_DOMAIN_ENCRYPT  && pKeyEntry->eType == OEM_HAL_KEY_TYPE_AES_ECB_ROOT    ), DRM_E_OEMHAL_UNSUPPORTED_KEY_WRAPPING_FORMAT );
        ChkBOOL( !( eWrappingKeyType == OEM_HAL_KEY_TYPE_ECC_P256_DOMAIN_ENCRYPT  && pKeyEntry->eType == OEM_HAL_KEY_TYPE_AES_CTR_CONTENT ), DRM_E_OEMHAL_UNSUPPORTED_KEY_WRAPPING_FORMAT );
    }
#endif /* !DRM_SUPPORT_SECUREOEMHAL */

    ChkDR( Oem_Hal_WrapKey( f_pBbxContext->pOEMContext,
                            USE_SECURE_HAL_CRYPTO_CONTEXT,
                            pKeyEntry->eType,
                            pKeyEntry->indexKeyRegister,
                            eWrappingKeyType,
                            indexWrappingKey,
                            f_pbWrappedKey,
                            f_pcbWrappedKey ) );

ErrorExit:
    return dr;
}


/*********************************************************************
**
** Function:    _Hal_GenerateKey
**
** Synopsis:    This function generates a random key of a given type and
**              places it into a key register.
**
** Arguments:
**
** [f_pBbxContext]              : The blackbox context
** [f_eKeyType]                 : The type of key to be generated.
** [f_dwRequestedSecurityLevel] : The requested security level of the key.
**                                Ignored unless f_eKeyType == OEM_HAL_KEY_TYPE_AES_ECB_NEW_LICENSE_ROOT
**                                            || f_eKeyType == OEM_HAL_KEY_TYPE_AES_CTR_NEW_LICENSE_CONTENT
** [f_phKey]                    : The handle of the generated key.
**
** Returns:     DRM_SUCCESS on success.
**              DRM_E_INVALIDARG if f_eKeyType is invalid.
**              DRM_E_POINTER if f_phKey is NULL.
**              DRM_E_OEMHAL_CRYPTO_NOT_INITIALIZED
**              DRM_E_OEMHAL_CRYPTO_UNSUPPORTED_KEY_TYPE
**
** Notes:      after this function is successfully called, a call must be made to
**             _Hal_ReleaseKey to free resources associated with the key.
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL _Hal_GenerateKey(
    __inout_ecount( 1 ) DRM_BB_CONTEXT     *f_pBbxContext,
    __in                OEM_HAL_KEY_TYPE    f_eKeyType,
    __in                DRM_DWORD           f_dwRequestedSecurityLevel,
    __out_ecount(1)     OEM_CRYPTO_HANDLE  *f_phKey )
{
    DRM_RESULT                 dr             = DRM_SUCCESS;
    OEM_CRYPTO_HANDLE          hKey           = OEM_CRYPTO_HANDLE_INVALID;
    DRM_BB_KEY_REGISTER_ENTRY *pKeyEntry      = NULL;
    DRM_BOOL                   fCachedHandle  = FALSE;
    ChkPtr( f_pBbxContext );

    /*
    ** Allocate a new key slot for the generated key
    */
    ChkDR( _Hal_AllocateKeyHandle( f_pBbxContext, f_eKeyType, NULL, 0, &hKey, &pKeyEntry, &fCachedHandle ) );

    ChkDR( Oem_Hal_GenerateKey( f_pBbxContext->pOEMContext,
                                USE_SECURE_HAL_CRYPTO_CONTEXT,
                                f_eKeyType,
                                pKeyEntry->indexKeyRegister,
                                f_dwRequestedSecurityLevel ) );

    *f_phKey = hKey;
    hKey     = OEM_CRYPTO_HANDLE_INVALID;

ErrorExit:
    DRM_BBX_SafeReleaseKey( f_pBbxContext, &hKey );
    return dr;
}


/*********************************************************************
**
** Function:    _Hal_CreateSampleEncryptionContext
**
** Synopsis:    Creates a sample encryption context that can, after it is
**              has been populated appropriately, designate the input for
**              a series of encryption operations covering one or several samples.
**              The sample encryption context itself is held in secure memory,
**              and cannot be directly manipulated by application code. Once
**              created, the context should be destroyed using the OEM_HAL
**              Oem_Hal_DestroySampleEncryptionContext function
**
** Arguments:
**
** [f_pBbxContext]:             The blackbox context.
** [f_hKey]:                    The handle of the key to be used for encryption
**                              of the source data..
** [f_eSampleEncryptionMode]:   The encryption mode requested for this context.
**                              This mode may not be available according to policy,
**                              in which case DRM_E_OEMHAL_CRYPTO_SAMPLE_ENCRYPTION_MODE_NOT_PERMITTED
**                              will be returned (see Remarks).
** [f_phEncryptionContext]:     A pointer to a handle which will be filled with
**                              a reference to the newly created sample encryption context.
**
** Returns:     DRM_SUCCESS on success.
**              DRM_E_HANDLE if f_hKey is invalid.
**              DRM_E_POINTER if f_pqwInitializationVector or f_phEncryptionContext are NULL.
**              DRM_E_OEMHAL_CRYPTO_NOT_INITIALIZED
**              DRM_E_OUTOFMEMORY
**              DRM_E_OEMHAL_CRYPTO_UNSUPPORTED_KEY_TYPE
**              DRM_E_OEMHAL_CRYPTO_SAMPLE_ENCRYPTION_MODE_NOT_PERMITTED
**
** Notes:       Following its creation, a sample encryption context can be populated with
** references to the data to be encrypted (and with certain other metadata governing the
** encryption process) by a secure data source, such as a hardware transcoder. This
** maintains the secure data path. The crypto HAL implementation will generate or derive
** a suitable 64-bit sample id for each sample. For the first sample this will be random.
** For subsequent samples, this will be derived by incrementing the previous one. At most
** 2**20 samples can be encrypted using a single context. The detailed layout of the context
** in secure memory is platform-dependent, and is a matter for bilateral agreement between
** the implementors of the crypto HAL and the secure data source. However it is expected
** that the context will contain at least the information in the OEM_HAL_ENCRYPTION_CONTEXT_INTERNAL
** structure. The encryption mode requested in the requestedMode parameter may not be
** available according to the policy enforced by the crypto HAL implementation for a
** particular platform. It is expected that such restrictions will be made known to the
** application developers affected.
** Here are some examples of policy restricting the use of certain modes:
**    OEM_HAL_SAMPLE_ENCRYPTION_MODE_PASSTHRU is only permitted if the source data
**       (e.g. before transcoding) was unencrypted;
**    OEM_HAL_SAMPLE_ENCRYPTION_MODE_FULL is permitted unless adherence to one of the
**       subsample encryption standards is mandated;
**    OEM_HAL_SAMPLE_ENCRYPTION_MODE_SUBSAMPLE_CENC is permitted only for PlayReady
**       encryption, and only if CFF is not required;
**    OEM_HAL_SAMPLE_ENCRYPTION_MODE_SUBSAMPLE_CFF is permitted only for PlayReady encryption.
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL _Hal_CreateSampleEncryptionContext(
    __inout_ecount( 1 ) DRM_BB_CONTEXT                 *f_pBbxContext,
    __in                OEM_CRYPTO_HANDLE               f_hKey,
    __in                OEM_HAL_SAMPLE_ENCRYPTION_MODE  f_eSampleEncryptionMode,
    __out_ecount(1)     OEM_ENCRYPTION_HANDLE          *f_phEncryptionContext )
{
    DRM_RESULT                 dr     = DRM_SUCCESS;
    DRM_BB_KEY_REGISTER_ENTRY *pEntry = NULL;

    ChkDR( _ResolveKeyHandle( f_pBbxContext, f_hKey, &pEntry ) );

    ChkDR( Oem_Hal_CreateSampleEncryptionContext( pEntry->eType,
                                                  pEntry->indexKeyRegister,
                                                  f_eSampleEncryptionMode,
                                                  f_phEncryptionContext ) );

ErrorExit:
    return dr;
}


/*********************************************************************
**
** Function:    _Hal_EncryptDataBlock
**
** Synopsis:    This function encrypts a single 16-byte block with a
**              message encryption or root key.
**
** Arguments:
**
** [f_pBbxContext]:            The blackbox context.
** [f_hKey]                    The key used for encrypting.
** [f_rgbBlock]:               The data block to be encrypted.
** [f_cbBlock]:                The length (in bytes) of the data block. This must be 16 bytes long.
** [f_rgbEncryptedBlock]:      Buffer to output the encrypted block.
** [f_pcbEncryptedBlock]:      A pointer to a location holding the length (in bytes)
**                             of the encrypted block buffer. If the buffer is too small the
**                             function returns an error indicating that, and updates the
**                             location with the buffer size required.
**
** Returns:     DRM_SUCCESS: No Errors
**              DRM_E_OEMHAL_BAD_KEY_REGISTER_INDEX
**              DRM_E_OEMHAL_BAD_POINTER
**              DRM_E_OEMHAL_BUFFER_TOO_LARGE
**              DRM_E_BUFFERTOOSMALL
**              DRM_E_OEMHAL_NO_KEY_IN_REGISTER
**              DRM_E_OEMHAL_NOT_INITIALIZED
**              DRM_E_OEMHAL_UNSUPPORTED_KEY_TYPE
**
** Notes:       None.
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL _Hal_EncryptDataBlock(
    __inout_ecount( 1 )                       DRM_BB_CONTEXT             *f_pBbxContext,
    __in                                      OEM_CRYPTO_HANDLE           f_hKey,
    __in_bcount( f_cbData )             const DRM_BYTE                   *f_rgbBlock,
    __in                                      DRM_DWORD                   f_cbBlock,
    __out_bcount_opt( *f_pcbEncryptedBlock )  DRM_BYTE                   *f_rgbEncryptedBlock,
    __inout                                   DRM_DWORD                  *f_pcbEncryptedBlock )
{
    DRM_RESULT                 dr     = DRM_SUCCESS;
    DRM_BB_KEY_REGISTER_ENTRY *pEntry = NULL;

    ChkDR( _ResolveKeyHandle( f_pBbxContext, f_hKey, &pEntry ) );

    ChkDR( Oem_Hal_EncryptDataBlock( f_rgbBlock,
                                     f_cbBlock,
                                     f_rgbEncryptedBlock,
                                     f_pcbEncryptedBlock,
                                     pEntry->eType,
                                     pEntry->indexKeyRegister ) );

ErrorExit:
    return dr;
}

#if 0
//Added For Get Content Key
DRM_API DRM_RESULT DRM_CALL _Hal_GetContentKey(
    __inout_ecount( 1 )         DRM_BB_CONTEXT               *f_pBbxContext,
    __in_ecount( 1 )            DRM_CIPHER_CONTEXT           *f_pCipherContext)
{
    DRM_RESULT                dr                 = DRM_SUCCESS;
    DRM_BB_KEY_REGISTER_ENTRY *pEntry            = NULL;

	ChkDR( _ResolveKeyHandle( f_pBbxContext,
							  f_pCipherContext->cipher.cipherAES.hAesKey,
							  &pEntry ) );
    ChkBOOL( pEntry->eType == OEM_HAL_KEY_TYPE_AES_CTR_CONTENT
          || pEntry->eType == OEM_HAL_KEY_TYPE_AES_CTR_NEW_LICENSE_CONTENT,
             DRM_E_OEMHAL_UNSUPPORTED_KEY_TYPE );
    Oem_Hal_GetContentKey(f_pCipherContext, pEntry->eType, pEntry->indexKeyRegister);

ErrorExit:
    return dr;

}
#endif
/*********************************************************************
**
** Function: _Hal_DecryptContent
**
** Synopsis: Decrypt content using AES counter mode via the OEM HAL API.
**
** Arguments:
** [f_pBbxContext]      : The black box context
** [f_pCipherContext]   : Specifies a pre-initialized cipher context
** [f_pCtrContext]      : Sets and maintains the
**                        initialization vector and counter
**                        for AES CTR mode. Must be
**                        specified for AES decryption, and
**                        ignored for Cocktail.
** [f_cbData]           : Length of f_pbData in bytes
** [f_pbData]           : Array of bytes to decrypt
**
** Returns:
**      DRM_SUCCESS
**          success
**      DRM_E_INVALIDARG
**          pbData or pContextCipher is null
**      DRM_E_CIPHER_NOT_INITIALIZED
**          pContextCipher was not appropriately initialized
**      DRM_E_CRYPTO_FAILED
**          The encrypt/decrypt operation failed
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL _Hal_DecryptContent(
    __inout_ecount( 1 )         DRM_BB_CONTEXT               *f_pBbxContext,
    __in_ecount( 1 )            DRM_CIPHER_CONTEXT           *f_pCipherContext,
    __inout_opt                 DRM_AES_COUNTER_MODE_CONTEXT *f_pCtrContext,
    __in                        DRM_DWORD                     f_cbData,
    __inout_bcount( f_cbData )  DRM_BYTE                     *f_pbData )
{
    DRM_RESULT                dr                 = DRM_SUCCESS;
    DRM_UINT64                qwSampleByteOffset = DRM_UI64LITERAL( 0, 0 );
    DRM_BB_KEY_REGISTER_ENTRY *pEntry            = NULL;
    DRM_UINT64		      debugingCode = 0xF000000000000000;
    ChkArg( f_pCipherContext != NULL );
    ChkArg( f_pCtrContext    != NULL );
    ChkArg( f_pbData         != NULL );

    ChkDR( _ResolveKeyHandle( f_pBbxContext,
                              f_pCipherContext->cipher.cipherAES.hAesKey,
                              &pEntry ) );

    ChkBOOL( pEntry->eType == OEM_HAL_KEY_TYPE_AES_CTR_CONTENT
          || pEntry->eType == OEM_HAL_KEY_TYPE_AES_CTR_NEW_LICENSE_CONTENT,
             DRM_E_OEMHAL_UNSUPPORTED_KEY_TYPE );

    /*
    ** Convert the counter context values into a sample byte offset by adding
    ** the byte offset to a 4-byte leftshifted block offset
    */

	if((f_pCtrContext->qwBlockOffset & debugingCode) != 0x0000000000000000)
	{
		printf("[Func = %s, Line = %d] [blockOffset = %x] CAN BE ABNORMAL , First 4bit can miss\n",__FUNCTION__, __LINE__, f_pCtrContext->qwBlockOffset);
	}

    qwSampleByteOffset = DRM_UI64ShL( f_pCtrContext->qwBlockOffset, 4 );
    qwSampleByteOffset = DRM_UI64Add( qwSampleByteOffset, DRM_UI64( f_pCtrContext->bByteOffset ) );

    ChkDR( Oem_Hal_DecryptContent( f_pbData,
                                   f_cbData,
                                   pEntry->eType,
                                   pEntry->indexKeyRegister,
                                   f_pCtrContext->qwInitializationVector,
                                   qwSampleByteOffset ) );

#ifdef TEE
	DRM_BOOL secureStopOn = FALSE;
	ChkDR(TEE_IsSecureStopOn(&secureStopOn));
	if (secureStopOn) {
		ChkDR( TEE_SecureStopUpdated(pEntry->indexKeyRegister, &f_pCipherContext->cipher.cipherAES.secureStopUpdated));
	} else {
		f_pCipherContext->cipher.cipherAES.secureStopUpdated = FALSE;
	}
#endif

    /*
    ** Update the counter context by adding the bytes decoded and reversing
    ** conversion carried out above
    */
    qwSampleByteOffset = DRM_UI64Add( qwSampleByteOffset, DRM_UI64( f_cbData ) );

    f_pCtrContext->qwBlockOffset = DRM_UI64ShR( qwSampleByteOffset, 4 );
    ChkDR( DRM_UInt64ToByte( DRM_UI64Mod( qwSampleByteOffset, DRM_UI64HL( 0, DRM_AES_BLOCKLEN ) ), &f_pCtrContext->bByteOffset ) );

ErrorExit:
    return dr;
}

DRM_API DRM_RESULT DRM_CALL _Hal_DecryptContentOpaque(
    __inout_ecount( 1 )         DRM_BB_CONTEXT               *f_pBbxContext,
    __in_ecount( 1 )            DRM_CIPHER_CONTEXT           *f_pCipherContext,
    __inout                     DRM_AES_COUNTER_MODE_CONTEXT *f_pCtrContext,
    __in                        OEM_OPAQUE_BUFFER_HANDLE      f_hInBuf,
    __in                        OEM_OPAQUE_BUFFER_HANDLE      f_hOutBuf,
    __in                        DRM_DWORD                     f_cbData )
{
    DRM_RESULT                dr                 = DRM_SUCCESS;
    DRM_UINT64                qwSampleByteOffset = DRM_UI64LITERAL( 0, 0 );
    DRM_BB_KEY_REGISTER_ENTRY *pEntry            = NULL;

    ChkPtr( f_pCipherContext );
    ChkPtr( f_pCtrContext    );
    ChkArg( f_hInBuf         != OEM_OPAQUE_BUFFER_HANDLE_INVALID );
    ChkArg( f_hOutBuf        != OEM_OPAQUE_BUFFER_HANDLE_INVALID );

    ChkDR( _ResolveKeyHandle( f_pBbxContext,
                              f_pCipherContext->cipher.cipherAES.hAesKey,
                              &pEntry ) );

    ChkBOOL( pEntry->eType == OEM_HAL_KEY_TYPE_AES_CTR_CONTENT
          || pEntry->eType == OEM_HAL_KEY_TYPE_AES_CTR_NEW_LICENSE_CONTENT,
             DRM_E_OEMHAL_UNSUPPORTED_KEY_TYPE );

    /*
    ** Convert the counter context values into a sample byte offset by adding
    ** the byte offset to a 4-byte leftshifted block offset
    */
    qwSampleByteOffset = DRM_UI64ShL( f_pCtrContext->qwBlockOffset, 4 );
    qwSampleByteOffset = DRM_UI64Add( qwSampleByteOffset, DRM_UI64( f_pCtrContext->bByteOffset ) );

    ChkDR( Oem_Hal_DecryptContentOpaque( f_hInBuf,
                                         f_hOutBuf,
                                         f_cbData,
                                         pEntry->eType,
                                         pEntry->indexKeyRegister,
                                         f_pCtrContext->qwInitializationVector,
                                         qwSampleByteOffset ) );

    /*
    ** Update the counter context by adding the bytes decoded and reversing
    ** conversion carried out above
    */
    qwSampleByteOffset = DRM_UI64Add( qwSampleByteOffset, DRM_UI64( f_cbData ) );

    f_pCtrContext->qwBlockOffset = DRM_UI64ShR( qwSampleByteOffset, 4 );
    ChkDR( DRM_UInt64ToByte( DRM_UI64Mod( qwSampleByteOffset, DRM_UI64HL( 0, DRM_AES_BLOCKLEN ) ), &f_pCtrContext->bByteOffset ) );

ErrorExit:
    return dr;
}

/*********************************************************************
**
** Function: _Hal_EncryptSampleData
**
** Synopsis: Encrypt content using AES counter mode via the OEM HAL API.
**
** Arguments:
** [f_pOEMContext]             : The OEM context
** [f_hEncryptionContext]      : Handle to an initialized encryption context
** [f_pqwInitializationVector] : Pointer to a variable to populate with the
**                               initialization vector. If this is NULL the old
**                               IV will be used
** [f_pMetadata]               : Optional pointer to a structure to populate
**                               with metadata about the current sample
** [f_hInBuf]                  : Handle to an OEM-defined buffer structure
** [f_hOutBuf]                 : Handle to an OEM-defined buffer structure
** [f_cbData]                  : Length of f_pbData in bytes
** [f_pbData]                  : Array of bytes to decrypt
**
** Returns:
**      DRM_SUCCESS
**          success
**      DRM_E_INVALIDARG
**          pbData or pContextCipher is null
**      DRM_E_CIPHER_NOT_INITIALIZED
**          pContextCipher was not appropriately initialized
**      DRM_E_CRYPTO_FAILED
**          The encrypt/decrypt operation failed
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL _Hal_EncryptSampleData(
    __in                           DRM_VOID                 *f_pOEMContext,
    __in                           OEM_ENCRYPTION_HANDLE     f_hEncryptionContext,
    __out_ecount_opt(1)            DRM_UINT64               *f_pqwInitializationVector,
    __out_ecount_opt(1)            OEM_HAL_SAMPLE_METADATA  *f_pMetadata,
    __in_opt                       OEM_OPAQUE_BUFFER_HANDLE  f_hInBuf,
    __in_opt                       OEM_OPAQUE_BUFFER_HANDLE  f_hOutBuf,
    __in                           DRM_DWORD                 f_cbData,
    __inout_bcount_opt( f_cbData ) DRM_BYTE                 *f_pbData )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_hEncryptionContext != OEM_ENCRYPTION_HANDLE_INVALID );

    /*
    ** Only one of the input methods should be valid, and supply of f_hInBuf must be the same as f_hOutBuf
    */
    ChkArg( ( f_hInBuf == OEM_OPAQUE_BUFFER_HANDLE_INVALID ) != ( f_pbData == NULL ) );
    ChkArg( ( f_hInBuf == OEM_OPAQUE_BUFFER_HANDLE_INVALID ) == ( f_hOutBuf == OEM_OPAQUE_BUFFER_HANDLE_INVALID ) );

    ChkDR( Oem_Hal_EncryptSampleData( f_pOEMContext,
                                      f_hEncryptionContext,
                                      f_pqwInitializationVector,
                                      f_pMetadata,
                                      f_hInBuf,
                                      f_hOutBuf,
                                      f_cbData,
                                      f_pbData ) );

ErrorExit:
    return dr;
}


/*********************************************************************
**
** Function: _Hal_LoadPlayReadyRevocationInfo
**
** Synopsis: This function loads a signed revocation info blob. Any public keys currently in use
**           are invalidated pending re-validation against a CRL submitted with Oem_Hal_LoadPlayReadyCRL
**
** Arguments:
** [f_pBbxContext]      : The blackbox context
** [f_pbRevocationInfo] : Pointer to the Revocation Info BLOB
** [f_cbRevocationInfo] : Length in bytes of the Revocation Info BLOB
** [f_hSigningKey]      : Handle to the public key used to sign the revocation info
**
** Returns:
**      DRM_SUCCESS
**          success
**      DRM_E_INVALIDARG
**          f_pBbxContext or f_pbRevocationInfo is null, f_hSigningKey is invalid
**      DRM_E_OEMHAL_NOT_INITIALIZED
**          The OEM HAL has not been initialized
**      DRM_E_INVALID_REVOCATION_LIST
**          The revocation list is invalid
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL _Hal_LoadPlayReadyRevocationInfo(
    __inout_ecount( 1 )                     DRM_BB_CONTEXT    *f_pBbxContext,
    __in_bcount( f_cbRevocationInfo ) const DRM_BYTE          *f_pbRevocationInfo,
    __in                                    DRM_DWORD          f_cbRevocationInfo,
    __in                                    OEM_CRYPTO_HANDLE  f_hSigningKey )
{
    DRM_RESULT                 dr     = DRM_SUCCESS;
    DRM_BB_KEY_REGISTER_ENTRY *pEntry = NULL;

    ChkDR( _ResolveKeyHandle( f_pBbxContext, f_hSigningKey, &pEntry ) );

    ChkArg( pEntry->eType == OEM_HAL_KEY_TYPE_ECC_P256_PUBLIC );

    ChkDR( Oem_Hal_LoadPlayReadyRevocationInfo( USE_SECURE_HAL_CRYPTO_CONTEXT,
                                                f_pbRevocationInfo,
                                                f_cbRevocationInfo,
                                                pEntry->indexKeyRegister ) );

ErrorExit:
    return dr;
}


/*********************************************************************
**
** Function: _Hal_LoadPlayReadyCrl
**
** Synopsis: This function loads a signed CRL (cert revocation list) blob. Any public keys
**           are re-validated against the CRL
**
** Arguments:
** [f_pBbxContext]      : The blackbox context
** [f_pbCRL]            : Pointer to the CRL BLOB
** [f_cbCRL]            : Length in bytes of the CRL BLOB
** [f_hSigningKey]      : Handle to the public key used to sign the CRL
**
** Returns:
**      DRM_SUCCESS
**          success
**      DRM_E_INVALIDARG
**          f_pBbxContext or f_pbRevocationInfo is null, f_hSigningKey is invalid
**      DRM_E_OEMHAL_NOT_INITIALIZED
**          The OEM HAL has not been initialized
**      DRM_E_INVALID_REVOCATION_LIST
**          The revocation list is invalid
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL _Hal_LoadPlayReadyCrl(
    __inout_ecount( 1 )          DRM_BB_CONTEXT    *f_pBbxContext,
    __in_bcount( f_cbCRL ) const DRM_BYTE          *f_pbCRL,
    __in                         DRM_DWORD          f_cbCRL,
    __in                         OEM_CRYPTO_HANDLE  f_hSigningKey )
{
    DRM_RESULT                 dr     = DRM_SUCCESS;
    DRM_BB_KEY_REGISTER_ENTRY *pEntry = NULL;

    ChkDR( _ResolveKeyHandle( f_pBbxContext, f_hSigningKey, &pEntry ) );

    ChkArg( pEntry->eType == OEM_HAL_KEY_TYPE_ECC_P256_PUBLIC );

    ChkDR( Oem_Hal_LoadPlayReadyCrl( USE_SECURE_HAL_CRYPTO_CONTEXT,
                                     f_pbCRL,
                                     f_cbCRL,
                                     pEntry->indexKeyRegister ) );

ErrorExit:
    return dr;
}

EXIT_PK_NAMESPACE_CODE;

