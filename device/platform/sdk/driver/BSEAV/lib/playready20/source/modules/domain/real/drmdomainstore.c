/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmcommon.h>
#include <drmtypes.h>
#include <drmutilities.h>
#include <drmcrt.h>
#include <drmcontextsizes.h>
#include <drmdatastore.h>
#include <drmdomainstore.h>
#include <oem.h>
#include <drmbcertparser.h>
#include <drmantirollbackclock.h>

ENTER_PK_NAMESPACE_CODE;

#define DRM_DOMAIN_STORE_MAX_NUMBER_OF_CHILD_NODES   16
#define DRM_DOMAIN_STORE_EXPECTED_SLOT_SIZE          1000

/* "domainstore\0\0\0" */
static const DRM_DST_NAMESPACE g_rgbDomainStoreNameSpace =
{
    'd', 'o', 'm', 'a', 'i', 'n',
    's', 't', 'o', 'r', 'e', '\0',
    '\0', '\0'
};

/* Type ID of the domain certificate */
static const DRM_ID g_oCertTypeID =
{
    '\0', '\0', '\0', '\0', '\0', '\0',
    '\0', '\0', '\0', '\0', '\0', '\0',
    '\0', '\0', '\0', '\0'
};

/*********************************************************************
**
** Function: _DomainStore_AddData
**
** Synopsis: Function that adds a piece of data into the domain store
**           using the passed in primary key and secondary key.
**
** Arguments:
**
** [f_poDomainStoreContext] -- Pointer to an initialized domain store
**                             context.
** [f_poAccountID]          -- Pointer to a primary key (account ID)
**                             that is used to add the data.
** [f_poTypeID]             -- Pointer to a secondary key (type ID) that
**                             is used to add the data.
** [f_poServiceID]          -- Pointer to service ID (GUID). It can be NULL
**                             if *f_poTypeID is NOT the same as g_oCertTypeID.
** [f_pbData]               -- Pointer to a buffer that contains the data
**                             to be added.
** [f_cbData]               -- Size (number of bytes) of the data to be added.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_DOMAIN_STORE_ADD_DATA for all other errors.
**
**********************************************************************/
static DRM_NO_INLINE DRM_RESULT _DomainStore_AddData(
    IN       DRM_DOMAINSTORE_CONTEXT *f_poDomainStoreContext,
    IN const DRM_GUID                *f_poAccountID,
    IN const DRM_GUID                *f_poTypeID,
    IN const DRM_GUID                *f_poServiceID,
    IN const DRM_BYTE                *f_pbData,
    IN       DRM_DWORD                f_cbData )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DOMAINSTORE_CONTEXT_PRIVATE *poDomainStoreContextPrivate =
        ( DRM_DOMAINSTORE_CONTEXT_PRIVATE * )f_poDomainStoreContext;
    DRM_DWORD cbWritten = 0;
    DRM_DWORD cbSlotSize1 = 0;
    DRM_DWORD cbSlotSize2 = 0;
    DRM_BOOL fOpened = FALSE;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DOMAINSTORE, PERF_FUNC__DomainStore_AddData );

    ChkArg( f_poDomainStoreContext != NULL );
    ChkArg( poDomainStoreContextPrivate->m_fInited );
    ChkArg( f_poAccountID != NULL );
    ChkArg( f_poTypeID != NULL );
    ChkArg( MEMCMP( f_poTypeID, &g_oCertTypeID, SIZEOF( DRM_GUID ) ) != 0 ||
            f_poServiceID != NULL );
    ChkArg( f_pbData != NULL );
    ChkArg( f_cbData > 0 );

    cbSlotSize1 = f_cbData;

    if ( MEMCMP( f_poTypeID, &g_oCertTypeID, SIZEOF( DRM_GUID ) ) == 0 )
    {
        /*
        ** If a domain certificate is going to be stored in the slot, its service ID
        ** is stored together at the beginning of the slot. This saves parsing of the
        ** domain certificate when the domain certificate is enumerated.
        ** The size of the slot to be allocated should take into account the size of
        ** the service ID.
        */
        cbSlotSize1 += SIZEOF( DRM_GUID );
    }

    cbSlotSize2 = cbSlotSize1;

    ChkDR( DRM_DST_OpenSlot( poDomainStoreContextPrivate->m_poDST,
                             &poDomainStoreContextPrivate->m_oDSTNameSpaceContext,
                             eDRM_DST_OPEN_ALWAYS | eDRM_DST_LOCKEXCLUSIVE | eDRM_DST_LOCKWAIT,
                             ( DRM_ID * )f_poAccountID,
                             ( DRM_ID * )f_poTypeID,
                             NULL,
                             &cbSlotSize2,
                             &poDomainStoreContextPrivate->m_oDSTSlotContext ) );

    fOpened = TRUE;

    if ( cbSlotSize1 != cbSlotSize2 )
    {
        ChkDR( DRM_DST_SlotResize( poDomainStoreContextPrivate->m_poDST,
                                   &poDomainStoreContextPrivate->m_oDSTSlotContext,
                                   cbSlotSize1 ) );
    }

    if ( MEMCMP( f_poTypeID, &g_oCertTypeID, SIZEOF( DRM_GUID ) ) == 0 )
    {
        /* Write the service ID first. */
        ChkDR( DRM_DST_SlotWrite( poDomainStoreContextPrivate->m_poDST,
                                  &poDomainStoreContextPrivate->m_oDSTSlotContext,
                                  SIZEOF( DRM_GUID ),
                                  ( DRM_BYTE * )f_poServiceID,
                                  &cbWritten ) );

        ChkBOOL( cbWritten == SIZEOF( DRM_GUID ), DRM_E_DOMAIN_STORE_ADD_DATA );

        ChkDR( DRM_DST_SlotSeek( poDomainStoreContextPrivate->m_poDST,
                                 &poDomainStoreContextPrivate->m_oDSTSlotContext,
                                 SIZEOF( DRM_GUID ),
                                 eDRM_DST_SEEKSET,
                                 NULL ) );
    }

    /* Write the data (domain certificate or domain private key). */
    ChkDR( DRM_DST_SlotWrite( poDomainStoreContextPrivate->m_poDST,
                              &poDomainStoreContextPrivate->m_oDSTSlotContext,
                              f_cbData,
                              f_pbData,
                              &cbWritten ) );

    ChkBOOL( cbWritten == f_cbData, DRM_E_DOMAIN_STORE_ADD_DATA );

#if !_DATASTORE_WRITE_THRU
    ChkDR( DRM_DST_CommitNamespace( poDomainStoreContextPrivate->m_poDST,
                                    &poDomainStoreContextPrivate->m_oDSTNameSpaceContext ) );
#endif

ErrorExit:
    if ( fOpened )
    {
        DRM_DST_CloseSlot( poDomainStoreContextPrivate->m_poDST,
                           &poDomainStoreContextPrivate->m_oDSTSlotContext );
    }

    if ( DRM_FAILED( dr ) &&
         dr != DRM_E_INVALIDARG )
    {
        dr = DRM_E_DOMAIN_STORE_ADD_DATA;
    }

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/*********************************************************************
**
** Function: _DomainStore_GetData
**
** Synopsis: Function that gets a piece of data from the domain store using
**           the passed in primary key and secondary key.
**
** Arguments:
**
** [f_poDomainStoreContext] -- Pointer to an initialized domain store
**                             context.
** [f_poAccountID]          -- Pointer to a primary key (account ID)
**                             that is used to retrieve the data.
** [f_poTypeID]             -- Pointer to a secondary key (type ID) that
**                             is used to retrieve the data.
** [f_poServiceID]          -- Pointer to a buffer to receive the service
**                             ID. It can be NULL if *f_poTypeID is NOT
**                             the same as g_oCertTypeID.
** [f_pbData]               -- Pointer to a buffer is used to receive the
**                             data retrieved.
** [f_cbData]               -- Size (number of bytes) of the data retrieved.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_BUFFERTOOSMALL if the buffer is not big enough.
**                          DRM_E_DOMAIN_STORE_GET_DATA for all other errors.
**
**********************************************************************/
static DRM_NO_INLINE DRM_RESULT _DomainStore_GetData(
    IN DRM_DOMAINSTORE_CONTEXT *f_poDomainStoreContext,
    IN const DRM_GUID *f_poAccountID,
    IN const DRM_GUID *f_poTypeID,
    OUT DRM_GUID *f_poServiceID,
    OUT DRM_BYTE *f_pbData,
    IN OUT DRM_DWORD *f_pcbData )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DOMAINSTORE_CONTEXT_PRIVATE *poDomainStoreContextPrivate =
        ( DRM_DOMAINSTORE_CONTEXT_PRIVATE * )f_poDomainStoreContext;
    DRM_DWORD cbSlotSize = 0;
    DRM_BOOL fOpened = FALSE;
    DRM_DWORD cbRead = 0;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DOMAINSTORE, PERF_FUNC__DomainStore_GetData );

    ChkArg( f_poDomainStoreContext != NULL );
    ChkArg( poDomainStoreContextPrivate->m_fInited );
    ChkArg( f_poAccountID != NULL );
    ChkArg( f_poTypeID != NULL );
    ChkArg( MEMCMP( f_poTypeID, &g_oCertTypeID, SIZEOF( DRM_GUID ) ) != 0 ||
            f_poServiceID != NULL );
    ChkArg( f_pcbData != NULL );
    ChkArg( f_pbData == NULL ||
            *f_pcbData > 0 );

    ChkDR( DRM_DST_OpenSlot( poDomainStoreContextPrivate->m_poDST,
                             &poDomainStoreContextPrivate->m_oDSTNameSpaceContext,
                             eDRM_DST_OPEN_EXISTING | eDRM_DST_LOCKWAIT,
                             ( DRM_ID * )f_poAccountID,
                             ( DRM_ID * )f_poTypeID,
                             NULL,
                             &cbSlotSize,
                             &poDomainStoreContextPrivate->m_oDSTSlotContext ) );

    fOpened = TRUE;

    if ( MEMCMP( f_poTypeID, &g_oCertTypeID, SIZEOF( DRM_GUID ) ) == 0 )
    {
        /* Make sure the size of the slot is at least big enough to fit a service ID. */
        ChkBOOL( cbSlotSize >= SIZEOF( DRM_GUID ), DRM_E_DOMAIN_STORE_GET_DATA );

        ChkDR( DRM_DST_SlotRead( poDomainStoreContextPrivate->m_poDST,
                                 &poDomainStoreContextPrivate->m_oDSTSlotContext,
                                 SIZEOF( DRM_GUID ),
                                 ( DRM_BYTE * )f_poServiceID,
                                 &cbRead ) );

        ChkBOOL( cbRead == SIZEOF( DRM_GUID ), DRM_E_DOMAIN_STORE_GET_DATA );

        ChkDR( DRM_DST_SlotSeek( poDomainStoreContextPrivate->m_poDST,
                                 &poDomainStoreContextPrivate->m_oDSTSlotContext,
                                 SIZEOF( DRM_GUID ),
                                 eDRM_DST_SEEKSET,
                                 NULL ) );

        /*
        ** The size of buffer provided by user is only used to store the domain
        ** certificate and not the service ID.
        **/
        cbSlotSize -= SIZEOF( DRM_GUID );
    }

    if ( *f_pcbData < cbSlotSize )
    {
        *f_pcbData = cbSlotSize;

        ChkDR( DRM_E_BUFFERTOOSMALL );
    }

    ChkArg( f_pbData != NULL );

    ChkDR( DRM_DST_SlotRead( poDomainStoreContextPrivate->m_poDST,
                             &poDomainStoreContextPrivate->m_oDSTSlotContext,
                             cbSlotSize,
                             f_pbData,
                             f_pcbData ) );

ErrorExit:

    if ( fOpened )
    {
        DRM_DST_CloseSlot( poDomainStoreContextPrivate->m_poDST,
                          &poDomainStoreContextPrivate->m_oDSTSlotContext );
    }

    if ( DRM_FAILED( dr ) &&
         dr != DRM_E_INVALIDARG &&
         dr != DRM_E_BUFFERTOOSMALL )
    {
        dr = DRM_E_DOMAIN_STORE_GET_DATA;
    }

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/*********************************************************************
**
** Function: _DomainStore_DeleteData
**
** Synopsis: Function that deletes a piece of data from the domain store
**           using the passed in primary key and secondary key.
**
** Arguments:
**
** [f_poDomainStoreContext] -- Pointer to an initialized domain store
**                             context.
** [f_poAccountID]          -- Pointer to a primary key (account ID) that
**                             is used to delete the data.
** [f_poTypeID]             -- Pointer to a secondary key (type ID) that
**                             is used to delete the data.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_DOMAIN_STORE_DELETE_DATA for all other errors.
**
**********************************************************************/
static DRM_RESULT _DomainStore_DeleteData(
    IN DRM_DOMAINSTORE_CONTEXT *f_poDomainStoreContext,
    IN const DRM_GUID *f_poAccountID,
    IN const DRM_GUID *f_poTypeID )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DOMAINSTORE_CONTEXT_PRIVATE *poDomainStoreContextPrivate =
        ( DRM_DOMAINSTORE_CONTEXT_PRIVATE * )f_poDomainStoreContext;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DOMAINSTORE, PERF_FUNC__DomainStore_DeleteData );

    ChkArg( f_poDomainStoreContext != NULL );
    ChkArg( poDomainStoreContextPrivate->m_fInited );
    ChkArg( f_poAccountID != NULL );
    ChkArg( f_poTypeID != NULL );

    ChkDR( DRM_DST_DeleteSlot( poDomainStoreContextPrivate->m_poDST,
                               &poDomainStoreContextPrivate->m_oDSTNameSpaceContext,
                               ( DRM_ID * )f_poAccountID,
                               ( DRM_ID * )f_poTypeID,
                               NULL,
                               TRUE ) );

ErrorExit:

    if ( DRM_FAILED( dr ) &&
         dr != DRM_E_INVALIDARG )
    {
        dr = DRM_E_DOMAIN_STORE_DELETE_DATA;
    }

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/*********************************************************************
**
** Function: _DomainStore_InitEnumData
**
** Synopsis: Function that initializes an enumerator of a particular type
**           of data in an opened domain store.
**
** Arguments:
**
** [f_poDomainStoreContext] -- Pointer to a domain store context initialized
**                             via DRM_DOMST_OpenStore().
** [f_fCert]                -- Flag indicating whether domain certificates
**                             or domain private keys should be enumerated.
** [f_fExclusive]           -- Flag indicating whether the domain store
**                             enumerator context should be opened exclusively.
** [f_poDomainStoreEnumContext]
**                          -- Pointer to a domain store enumerator context
**                             to be initialized by the API.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_DOMAIN_STORE_GET_DATA for all other errors.
**
**********************************************************************/
static DRM_RESULT _DomainStore_InitEnumData(
    IN DRM_DOMAINSTORE_CONTEXT *f_poDomainStoreContext,
    IN const DRM_BOOL f_fCert,
    IN DRM_BOOL f_fExclusive,
    OUT DRM_DOMAINSTORE_ENUM_CONTEXT *f_poDomainStoreEnumContext )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DOMAINSTORE_CONTEXT_PRIVATE *poDomainStoreContextPrivate =
        ( DRM_DOMAINSTORE_CONTEXT_PRIVATE * )f_poDomainStoreContext;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DOMAINSTORE, PERF_FUNC__DomainStore_InitEnumData );

    ChkArg( f_poDomainStoreContext != NULL );
    ChkArg( poDomainStoreContextPrivate->m_fInited );
    ChkArg( f_poDomainStoreEnumContext != NULL );

    ZEROMEM( f_poDomainStoreEnumContext, SIZEOF( DRM_DOMAINSTORE_ENUM_CONTEXT ) );

    dr = DRM_DST_InitSlotEnum( poDomainStoreContextPrivate->m_poDST,
                               &poDomainStoreContextPrivate->m_oDSTNameSpaceContext,
                               NULL,
                               ( DRM_DST_LOCKMODE )( ( f_fExclusive ? eDRM_DST_LOCKEXCLUSIVE : eDRM_DST_LOCKSHARED ) | eDRM_DST_LOCKWAIT ),
                               &f_poDomainStoreEnumContext->m_oDSTEnumContext );

    if ( dr == DRM_E_DSTSLOTNOTFOUND )
    {
        dr = DRM_SUCCESS;
    }
    else if ( DRM_FAILED( dr ) )
    {
        ChkDR( DRM_E_DOMAIN_STORE_GET_DATA );
    }

    f_poDomainStoreEnumContext->m_poDomainStore = poDomainStoreContextPrivate->m_poDST;

    f_poDomainStoreEnumContext->m_fCert = f_fCert;

    f_poDomainStoreEnumContext->m_fInited = TRUE;

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/*********************************************************************
**
** Function: _DomainStore_EnumNextData
**
** Synopsis: Function that enumerates the next data of a particular type in
**           an opened domain store.
**
** Arguments:
**
** [f_poDomainStoreEnumContext]
**                          -- Pointer to an initialized domain store enumerator
**                             context by DRM_DOMST_InitEnumData().
** [f_poAccountID]          -- Pointer to a variable that receives the account ID
**                             of the currently enumerated domain store slot.
**                             This parameter is optional.
** [f_poTypeID]             -- Pointer to a variable that receives the type ID
**                             of the currently enumerated domain store slot.
**                             This parameter is optional.
** [f_poServiceID]          -- Pointer to a buffer to receive the service ID.
**                             This parameter is optional and is only used
**                             when the value of the secondary key is the same
**                             as g_oCertTypeID, which means the slot contains
**                             a domain certificate.
** [f_poSlotHint]           -- Pointer to a variable that receives the DST
**                             slot hint of the currently enumerated domain
**                             store slot. This parameter is optional.
** [f_pcbData]              -- Pointer to a variable that receives the size
**                             (number of bytes) of the currently enumerated
**                             domain store slot. This parameter is optional.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_NOMORE if there is no more data to be enumerated.
**                          DRM_E_DOMAIN_STORE_GET_DATA for all other errors.
**
**********************************************************************/
static DRM_NO_INLINE DRM_RESULT _DomainStore_EnumNextData(
    IN DRM_DOMAINSTORE_ENUM_CONTEXT *f_poDomainStoreEnumContext,
    OUT DRM_GUID *f_poAccountID,
    OUT DRM_GUID *f_poTypeID,
    OUT DRM_GUID *f_poServiceID,
    OUT DRM_DST_SLOT_HINT *f_poSlotHint,
    OUT DRM_DWORD *f_pcbData )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_GUID oCurrAccountID = EMPTY_DRM_GUID;
    DRM_GUID oCurrTypeID = EMPTY_DRM_GUID;
    DRM_DWORD cbSlotSize = 0;
    DRM_DWORD cbRead = 0;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DOMAINSTORE, PERF_FUNC__DomainStore_EnumNextData );

    ChkArg( f_poDomainStoreEnumContext != NULL );
    ChkArg( f_poDomainStoreEnumContext->m_fInited );

    for ( ; ; )
    {
        dr = DRM_DST_SlotEnumNext( f_poDomainStoreEnumContext->m_poDomainStore,
                                   &f_poDomainStoreEnumContext->m_oDSTEnumContext,
                                   &f_poDomainStoreEnumContext->m_oDSTSlotContext,
                                   ( DRM_ID * )&oCurrAccountID,
                                   ( DRM_ID * )&oCurrTypeID,
                                   &cbSlotSize );

        if ( dr == DRM_E_NOMORE )
        {
            break;
        }
        else if ( dr == DRM_E_DSTBLOCKMISMATCH ||
                  dr == DRM_E_DSTSLOTNOTFOUND )
        {
            ChkDR( DRM_E_DOMAIN_STORE_GET_DATA );
        }

        if ( f_poSlotHint != NULL )
        {
            DRM_DST_MakeSlotHint( f_poDomainStoreEnumContext->m_poDomainStore,
                                  &f_poDomainStoreEnumContext->m_oDSTSlotContext,
                                  f_poSlotHint );
        }

        if ( ( f_poDomainStoreEnumContext->m_fCert &&
               MEMCMP( &oCurrTypeID,
                       ( DRM_GUID * )&g_oCertTypeID,
                       SIZEOF( DRM_GUID ) ) != 0 ) ||
             ( !f_poDomainStoreEnumContext->m_fCert &&
               MEMCMP( &oCurrTypeID,
                       ( DRM_GUID * )&g_oCertTypeID,
                       SIZEOF( DRM_GUID ) ) == 0 ) )
        {
            ChkDR( DRM_DST_CloseSlot( f_poDomainStoreEnumContext->m_poDomainStore,
                                      &f_poDomainStoreEnumContext->m_oDSTSlotContext ) );

            continue;
        }

        if ( f_poDomainStoreEnumContext->m_fCert &&
             MEMCMP( &oCurrTypeID,
                     ( DRM_GUID * )&g_oCertTypeID,
                     SIZEOF( DRM_GUID ) ) == 0 )
        {
            /* Make sure the size of the current slot is at least big enough to fit a service ID. */
            ChkBOOL( cbSlotSize >= SIZEOF( DRM_GUID ), DRM_E_DOMAIN_STORE_GET_DATA );

            /*
            ** The size of buffer provided by user is only used to store the domain
            ** certificate and not the service ID.
            **/
            cbSlotSize -= SIZEOF( DRM_GUID );

            if ( f_poServiceID != NULL )
            {
                ChkDR( DRM_DST_SlotRead( f_poDomainStoreEnumContext->m_poDomainStore,
                                         &f_poDomainStoreEnumContext->m_oDSTSlotContext,
                                         SIZEOF( DRM_GUID ),
                                         ( DRM_BYTE * )f_poServiceID,
                                         &cbRead ) );

                ChkBOOL( cbRead == SIZEOF( DRM_GUID ), DRM_E_DOMAIN_STORE_GET_DATA );
            }
        }

        if ( f_poAccountID != NULL )
        {
            MEMCPY( f_poAccountID, &oCurrAccountID, SIZEOF( DRM_GUID ) );
        }

        if ( f_poTypeID != NULL )
        {
            MEMCPY( f_poTypeID, &oCurrTypeID, SIZEOF( DRM_GUID ) );
        }

        if ( f_pcbData != NULL )
        {
            *f_pcbData = cbSlotSize;
        }

        ChkDR( DRM_DST_CloseSlot( f_poDomainStoreEnumContext->m_poDomainStore,
                                  &f_poDomainStoreEnumContext->m_oDSTSlotContext ) );

        break;
    }

ErrorExit:

    if ( DRM_FAILED( dr ) &&
         dr != DRM_E_INVALIDARG &&
         dr != DRM_E_NOMORE )
    {
        dr = DRM_E_DOMAIN_STORE_GET_DATA;
    }

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/*********************************************************************
**
** Function: _InitEnumKeyExclusive
**
** Synopsis: Function that initializes an enumerator of all domain private
**           keys in an opened domain store. The enumerator is opened
**           in the exclusive mode.
**
** Arguments:
**
** [f_poDomainStoreContext] -- Pointer to a domain store context initialized
**                             via DRM_DOMST_OpenStore().
** [f_poDomainStoreEnumContext]
**                          -- Pointer to a domain store enumerator context
**                             to be initialized by the API.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_DOMAIN_STORE_GET_DATA for all other
**                          errors.
**
**********************************************************************/
static DRM_NO_INLINE DRM_RESULT _InitEnumKeyExclusive(
    IN DRM_DOMAINSTORE_CONTEXT *f_poDomainStoreContext,
    OUT DRM_DOMAINSTORE_ENUM_CONTEXT *f_poDomainStoreEnumContext )
{
    return _DomainStore_InitEnumData( f_poDomainStoreContext,
                                      FALSE,
                                      TRUE,
                                      f_poDomainStoreEnumContext );
}

/*********************************************************************
**
** Function: DRM_DOMST_OpenStore
**
** Synopsis: API that opens a domain store and returns an initialized
**           domain store context.
**
** Arguments:
**
** [f_pDatastore]           -- Pointer to an initialized data store.
** [f_poDomainStoreContext] -- Pointer to a domain store context to be
**                             initialized by the API.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_DOMAIN_STORE_OPEN_STORE for all other
**                          errors.
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_DOMST_OpenStore(
    IN  DRM_DST                 *f_pDatastore,
    OUT DRM_DOMAINSTORE_CONTEXT *f_poDomainStoreContext )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DOMAINSTORE_CONTEXT_PRIVATE *poDomainStoreContextPrivate =
        ( DRM_DOMAINSTORE_CONTEXT_PRIVATE * )f_poDomainStoreContext;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DOMAINSTORE, PERF_FUNC_DRM_DOMST_OpenStore );

    ChkArg( f_pDatastore != NULL );
    ChkArg( f_poDomainStoreContext != NULL );
    ChkArg( !poDomainStoreContextPrivate->m_fInited );

    DRMSIZEASSERT( SIZEOF( DRM_DOMAINSTORE_CONTEXT_PRIVATE ), SIZEOF ( DRM_DOMAINSTORE_CONTEXT ) );

    /* Open the namespace of the Domain Store */
    ChkDR( DRM_DST_OpenNamespace( f_pDatastore,
                                  &g_rgbDomainStoreNameSpace,
                                  (DRM_DST_OPENMODE)(eDRM_DST_OPEN_ALWAYS | eDRM_DST_LOCKWAIT),
                                  DRM_DOMAIN_STORE_MAX_NUMBER_OF_CHILD_NODES,
                                  DRM_DOMAIN_STORE_EXPECTED_SLOT_SIZE,
                                  &poDomainStoreContextPrivate->m_oDSTNameSpaceContext ) );

    poDomainStoreContextPrivate->m_fInited = TRUE;

    poDomainStoreContextPrivate->m_poDST = f_pDatastore;

ErrorExit:

    if ( DRM_FAILED( dr ) &&
         dr != DRM_E_INVALIDARG )
    {
        dr = DRM_E_DOMAIN_STORE_OPEN_STORE;
    }

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/*********************************************************************
**
** Function: DRM_DOMST_CloseStore
**
** Synopsis: API that closes an opened domain store.
**
** Arguments:
**
** [f_poDomainStoreContext] -- Pointer to a domain store context initialized
**                             via DRM_DOMST_OpenStore(). If the passed
**                             in domain store context is not initialized,
**                             the function just exit without returning any
**                             error code.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_DOMAIN_STORE_CLOSE_STORE for all other
**                          errors.
**
** Note:                    If caller passes in an uninitialized domain store
**                          context. It must be zeroed by the caller before
**                            being passed in.
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_DOMST_CloseStore(
    IN DRM_DOMAINSTORE_CONTEXT *f_poDomainStoreContext )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DOMAINSTORE_CONTEXT_PRIVATE *poDomainStoreContextPrivate =
        ( DRM_DOMAINSTORE_CONTEXT_PRIVATE * )f_poDomainStoreContext;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DOMAINSTORE, PERF_FUNC_DRM_DOMST_CloseStore );

    ChkArg( f_poDomainStoreContext != NULL );

    /*
    ** If a domain store context has not yet been initialized,
    ** just exit the function without returning any error code.
    */
    if ( !poDomainStoreContextPrivate->m_fInited )
    {
        goto ErrorExit;
    }

    /*
    ** If the domain store context has been initialized, then these are errors
    */
    ChkDR( DRM_DST_CloseNamespace( poDomainStoreContextPrivate->m_poDST,
                                   &poDomainStoreContextPrivate->m_oDSTNameSpaceContext ) );

    /* Implicitly set poDomainStoreContextPrivate->m_fInited to FALSE. */
    ZEROMEM( f_poDomainStoreContext, SIZEOF( DRM_DOMAINSTORE_CONTEXT ) );

ErrorExit:

    if ( DRM_FAILED( dr ) &&
         dr != DRM_E_INVALIDARG )
    {
        dr = DRM_E_DOMAIN_STORE_CLOSE_STORE;
    }

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/*********************************************************************
**
** Function: DRM_DOMST_AddCert
**
** Synopsis: API that adds a domain certificate into an opened domain store.
**
** Arguments:
**
** [f_rgbPasswordSST]       -- Domain Store Secure Store Password generated by
**                             DRM_DOMST_CreateDomainStorePassword.
** [f_pOEMContext]          -- Pointer to an optional OEM context structure.
** [f_poDomainStoreContext] -- Pointer to a domain store context initialized
**                             via DRM_DOMST_OpenStore().
** [f_poAccountID]          -- Pointer to an account ID of the domain
**                             certificate to be added into the domain store.
**                             It cannot be NULL.
** [f_poServiceID]          -- Pointer to a service ID of the domain
**                             certificate to be added into the domain store.
**                             It cannot be NULL.
** [f_pbCert]               -- Pointer to a buffer that contains the domain
**                             certificate to be added into the domain store.
** [f_cbCert]               -- Size (in bytes) of the buffer that contains
**                             the domain certificate to be added into the
**                             domain store.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_DOMAIN_STORE_ADD_DATA for all other
**                          errors.
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_DOMST_AddCert(
    __in_bcount( DRM_SHA1_DIGEST_LEN ) const DRM_BYTE                 f_rgbPasswordSST[__CB_DECL(DRM_SHA1_DIGEST_LEN)],
    __inout_opt                              DRM_VOID                *f_pOEMContext,
    IN                                       DRM_DOMAINSTORE_CONTEXT *f_poDomainStoreContext,
    IN                                       DRM_GUID                *f_poAccountID,
    IN                                       DRM_GUID                *f_poServiceID,
    __in_bcount( f_cbCert )            const DRM_BYTE                *f_pbCert,
    IN                                       DRM_DWORD                f_cbCert )
{
    DRM_RESULT                       dr                                                 = DRM_SUCCESS;
    DRM_BCERT_VERIFICATIONCONTEXT    oVerificationContext                               = {0};

    ChkDR( DRM_BCert_InitVerificationContext( NULL, /* expiration time */
                                              NULL, /* root key */
                                              DRM_BCERT_CERTTYPE_DOMAIN,
                                              NULL, /* crypto ctx */
                                              FALSE,
                                              TRUE,
                                              NULL, /* required key usages */
                                              0,
                                              FALSE,
                                              NULL, /* BBX ctx */
                                              NULL, /* SST ctx */
                                              NULL, /* data store */
                                              &oVerificationContext ) );

    ChkDR( DRM_BCert_ParseCertificateChain( f_pbCert, f_cbCert, &oVerificationContext ) );

    ChkDR( _DomainStore_AddData( f_poDomainStoreContext,
                                 f_poAccountID,
                                 ( DRM_GUID * )&g_oCertTypeID,
                                 f_poServiceID,
                                 f_pbCert,
                                 f_cbCert ) );

    ChkDR( DRM_ARCLK_DOMST_GetDomainSavedDateTime( f_rgbPasswordSST,
                                                   f_pOEMContext,
                                                   f_poDomainStoreContext,
                                                   f_poAccountID,
                                                   NULL ) );

ErrorExit:
    return dr;
}

/*********************************************************************
**
** Function: DRM_DOMST_GetCert
**
** Synopsis: API that retrieves a domain certificate from an opened domain
**           store.
**
** Arguments:
**
** [f_poDomainStoreContext] -- Pointer to a domain store context initialized
**                             via DRM_DOMST_OpenStore().
** [f_poAccountID]          -- Pointer to an account ID of the domain
**                             certificate to be retrieved from the domain store.
**                             It cannot be NULL.
** [f_poServiceID]          -- Pointer to buffer to receive the server ID of the
**                             domain  certificate retrieved from the domain store.
**                             It cannot be NULL.
** [f_pbCert]               -- Pointer to a buffer to receive the domain certificate
**                             retrieved  from the domain store. If it is NULL,
**                             the function returns an error and the size of the
**                             required buffer is stored in *pcbCert.
** [f_pcbCert]              -- Pointer to a variable that contains the size
**                             (in bytes) of the domain certificate buffer
**                             during input and receives the size (in bytes) of
**                             the domain certificate buffer during output.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_BUFFERTOOSMALL if the buffer to retrieve
**                          the data is not big enough.
**                          DRM_E_DOMAIN_STORE_GET_DATA for all other
**                          errors.
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_DOMST_GetCert(
    IN DRM_DOMAINSTORE_CONTEXT *f_poDomainStoreContext,
    IN DRM_GUID *f_poAccountID,
    IN DRM_GUID *f_poServiceID,
    __out_bcount_opt( *f_pcbCert ) DRM_BYTE *f_pbCert,
    IN OUT DRM_DWORD *f_pcbCert )
{
    return _DomainStore_GetData( f_poDomainStoreContext,
                                 f_poAccountID,
                                 ( DRM_GUID * )&g_oCertTypeID,
                                 f_poServiceID,
                                 f_pbCert,
                                 f_pcbCert );
}

/*********************************************************************
**
** Function: DRM_DOMST_DeleteCert
**
** Synopsis: API that deletes a domain certificate from an opened domain
**           store.
**
** Arguments:
**
** [f_poDomainStoreContext] -- Pointer to a domain store context initialized
**                             via DRM_DOMST_OpenStore().
** [f_poAccountID]          -- Pointer to an accountID of the domain
**                             certificate to be deleted from the domain store.
**                             It cannot be NULL.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_DOMAIN_STORE_DELETE_DATA for all other
**                          errors.
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_DOMST_DeleteCert(
    IN DRM_DOMAINSTORE_CONTEXT *f_poDomainStoreContext,
    IN DRM_GUID *f_poAccountID )
{
    return _DomainStore_DeleteData( f_poDomainStoreContext,
                                    f_poAccountID,
                                    ( DRM_GUID * )&g_oCertTypeID );
}

/*********************************************************************
**
** Function: DRM_DOMST_InitEnumCert
**
** Synopsis: API that initializes an enumerator of all domain certificates
**           in an opened domain store.
**
** Arguments:
**
** [f_poDomainStoreContext] -- Pointer to a domain store context initialized
**                             via DRM_DOMST_OpenStore().
** [f_poDomainStoreEnumContext]
**                          -- Pointer to a domain store enumerator context
**                             to be initialized by the API.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_DOMAIN_STORE_GET_DATA for all other
**                          errors.
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_DOMST_InitEnumCert(
    IN DRM_DOMAINSTORE_CONTEXT *f_poDomainStoreContext,
    OUT DRM_DOMAINSTORE_ENUM_CONTEXT *f_poDomainStoreEnumContext )
{
    return _DomainStore_InitEnumData( f_poDomainStoreContext,
                                      TRUE,
                                      FALSE,
                                      f_poDomainStoreEnumContext );
}

/*********************************************************************
**
** Function: DRM_DOMST_EnumNextCert
**
** Synopsis: API that enumerates the next domain certificate in an opened
**           domain store.
**
** Arguments:
**
** [f_poDomainStoreEnumContext]
**                          -- Pointer to an initialized domain store enumerator
**                             context by DRM_DOMST_InitEnumCert().
** [f_poAccountID]          -- Pointer to a variable that receives the account ID
**                             of the currently enumerated domain certificate.
**                             This parameter is optional.
** [f_poServiceID]          -- Pointer to a variable that receives the service ID
**                             of the currently enumerated domain certificate.
**                             This parameter is optional.
** [f_poSlotHint]           -- Pointer to a variable that receives the DST
**                             slot hint of the currently enumerated domain
**                             certificate. This parameter is optional.
** [f_pcbData]              -- Pointer to a variable that receives the size
**                             (number of bytes) of the currently enumerated
**                             domain certificate. This parameter is optional.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_NOMORE if there is no more data to be enumerated.
**                          DRM_E_DOMAIN_STORE_GET_DATA for all other
**                          errors.
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_DOMST_EnumNextCert(
    IN DRM_DOMAINSTORE_ENUM_CONTEXT *f_poDomainStoreEnumContext,
    OUT DRM_GUID *f_poAccountID,
    OUT DRM_GUID *f_poServiceID,
    OUT DRM_DST_SLOT_HINT *f_poSlotHint,
    OUT DRM_DWORD *f_pcbData )
{
    return _DomainStore_EnumNextData( f_poDomainStoreEnumContext,
                                      f_poAccountID,
                                      NULL,
                                      f_poServiceID,
                                      f_poSlotHint,
                                      f_pcbData );
}

/*********************************************************************
**
** Function: DRM_DOMST_AddKey
**
** Synopsis: API that adds a domain private key into an opened domain store.
**
** Arguments:
**
** [f_poDomainStoreContext] -- Pointer to a domain store context initialized
**                             via DRM_DOMST_OpenStore().
** [f_poAccountID]          -- Pointer to a account ID of the domain
**                             private key to be added into the domain store.
**                             It cannot be NULL.
** [f_dwRevision]           -- Revision of the domain private key to be added
**                             into the domain store.
** [f_pbKey]                -- Pointer to a buffer that contains the domain
**                             private key to be added into the domain store.
** [f_cbKey]                -- Size (in bytes) of the buffer that contains
**                             the domain private key to be added into the
**                             domain store.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_DOMAIN_STORE_ADD_DATA for all other
**                          errors.
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_DOMST_AddKey(
    IN                           DRM_DOMAINSTORE_CONTEXT *f_poDomainStoreContext,
    IN                           DRM_GUID                *f_poAccountID,
    IN                           DRM_DWORD                f_dwRevision,
    __in_bcount( f_cbKey ) const DRM_BYTE                *f_pbKey,
    IN                           DRM_DWORD                f_cbKey )
{
    DRM_GUID oTypeID = EMPTY_DRM_GUID;

    DWORD_TO_NETWORKBYTES( &oTypeID, 0, f_dwRevision );

    return _DomainStore_AddData( f_poDomainStoreContext,
                                 f_poAccountID,
                                 &oTypeID,
                                 NULL,
                                 f_pbKey,
                                 f_cbKey );
}

/*********************************************************************
**
** Function: DRM_DOMST_GetKey
**
** Synopsis: API that retrieves a domain private key from an opened domain
**           store.
**
** Arguments:
**
** [f_poDomainStoreContext] -- Pointer to a domain store context initialized
**                             via DRM_DOMST_OpenStore().
** [f_poAccountID]          -- Pointer to an account ID of the domain
**                             private key to be retrieved from the domain store.
**                             It cannot be NULL.
** [f_dwRevision]           -- Revision of the domain private key to be retrieved
**                             from the domain store. It cannot be NULL.
** [f_pbKey]                -- Pointer to a buffer to receive the domain private
**                             key retrieved from the domain store. If it is
**                             NULL, the function returns an error and the size
**                             of the required buffer is stored in *pcbCert.
** [f_pcbKey]               -- Pointer to a variable that contains the size
**                             (in bytes) of the domain private key buffer
**                             during input and receives the size (in bytes) of
**                             the domain private key buffer during output.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_BUFFERTOOSMALL if the buffer to retrieve
**                          the data is not big enough.
**                          DRM_E_DOMAIN_STORE_GET_DATA for all other
**                          errors.
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_DOMST_GetKey(
    IN DRM_DOMAINSTORE_CONTEXT *f_poDomainStoreContext,
    IN DRM_GUID *f_poAccountID,
    IN DRM_DWORD f_dwRevision,
    __out_bcount_opt( *f_pcbKey ) DRM_BYTE *f_pbKey,
    IN OUT DRM_DWORD *f_pcbKey )
{
    DRM_GUID oTypeID = EMPTY_DRM_GUID;

    DWORD_TO_NETWORKBYTES( &oTypeID, 0, f_dwRevision );

    return _DomainStore_GetData( f_poDomainStoreContext,
                                 f_poAccountID,
                                 &oTypeID,
                                 NULL,
                                 f_pbKey,
                                 f_pcbKey );
}

/*********************************************************************
**
** Function: DRM_DOMST_DeleteKey
**
** Synopsis: API that deletes a domain private key from an opened domain
**           store.
**
** Arguments:
**
** [f_poDomainStoreContext] -- Pointer to a domain store context initialized
**                             via DRM_DOMST_OpenStore().
** [f_poAccountID]          -- Pointer to an account ID of the domain
**                             private key to be deleted from the domain store.
**                             It cannot be NULL.
** [f_dwRevision]           -- Revision of the domain private key to be deleted
**                             from the domain store. It cannot be NULL.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_DOMAIN_STORE_DELETE_DATA for all other
**                          errors.
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_DOMST_DeleteKey(
    IN DRM_DOMAINSTORE_CONTEXT *f_poDomainStoreContext,
    IN DRM_GUID *f_poAccountID,
    IN DRM_DWORD f_dwRevision )
{
    DRM_GUID oTypeID = EMPTY_DRM_GUID;

    DWORD_TO_NETWORKBYTES( ( DRM_BYTE * )&oTypeID, 0, f_dwRevision );

    return _DomainStore_DeleteData( f_poDomainStoreContext,
                                    f_poAccountID,
                                    &oTypeID );
}

/*********************************************************************
**
** Function: DRM_DOMST_DeleteKeys
**
** Synopsis: API that deletes all domain private keys that have the same
**           account ID from an opened domain store.
**
** Arguments:
**
** [f_poDomainStoreContext] -- Pointer to a domain store context initialized
**                             via DRM_DOMST_OpenStore().
** [f_poAccountID]          -- Pointer to an account ID of the domain
**                             private keys to be deleted from the domain store.
**                             It cannot be NULL.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_DOMAIN_STORE_DELETE_DATA for all other
**                          errors.
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_DOMST_DeleteKeys(
    IN DRM_DOMAINSTORE_CONTEXT *f_poDomainStoreContext,
    IN DRM_GUID *f_poAccountID )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DOMAINSTORE_ENUM_CONTEXT oDomainStoreEnumContext = { 0 };
    DRM_GUID oAccountID = EMPTY_DRM_GUID;
    DRM_DOMAINSTORE_CONTEXT_PRIVATE *poDomainStoreContextPrivate =
        ( DRM_DOMAINSTORE_CONTEXT_PRIVATE * )f_poDomainStoreContext;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DOMAINSTORE, PERF_FUNC_DRM_DOMST_DeleteKeys );

    ChkArg( f_poDomainStoreContext != NULL );
    ChkArg( f_poAccountID != NULL );

    ChkDR( _InitEnumKeyExclusive( f_poDomainStoreContext,
                                  &oDomainStoreEnumContext ) );

    for ( ; ; )
    {
        dr = DRM_DOMST_EnumNextKey( &oDomainStoreEnumContext,
                                    &oAccountID,
                                    NULL,
                                    NULL,
                                    NULL );

        if ( dr == DRM_E_NOMORE )
        {
            dr = DRM_SUCCESS;

            break;
        }
        else
        {
            ChkDR( dr );
        }

        if ( MEMCMP( &oAccountID, f_poAccountID, SIZEOF( DRM_GUID ) ) != 0 )
        {
            continue;
        }

        ChkDR( DRM_DST_SlotEnumReloadCurrent( poDomainStoreContextPrivate->m_poDST,
                                              &oDomainStoreEnumContext.m_oDSTEnumContext,
                                              &oDomainStoreEnumContext.m_oDSTSlotContext,
                                              NULL,
                                              NULL,
                                              NULL ) );

        ChkDR( DRM_DST_SlotEnumDeleteCurrent( poDomainStoreContextPrivate->m_poDST,
                                              &oDomainStoreEnumContext.m_oDSTEnumContext,
                                              &oDomainStoreEnumContext.m_oDSTSlotContext ) );
    }

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/*********************************************************************
**
** Function: DRM_DOMST_InitEnumKey
**
** Synopsis: API that initializes an enumerator of all domain private
**           keys in an opened domain store.
**
** Arguments:
**
** [f_poDomainStoreContext] -- Pointer to a domain store context initialized
**                             via DRM_DOMST_OpenStore().
** [f_poDomainStoreEnumContext]
**                          -- Pointer to a domain store enumerator context
**                             to be initialized by the API.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_DOMAIN_STORE_GET_DATA for all other
**                          errors.
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_DOMST_InitEnumKey(
    IN DRM_DOMAINSTORE_CONTEXT *f_poDomainStoreContext,
    OUT DRM_DOMAINSTORE_ENUM_CONTEXT *f_poDomainStoreEnumContext )
{
    return _DomainStore_InitEnumData( f_poDomainStoreContext,
                                      FALSE,
                                      FALSE,
                                      f_poDomainStoreEnumContext );
}

/*********************************************************************
**
** Function: DRM_DOMST_EnumNextKey
**
** Synopsis: API that enumerates the next domain private key in an opened
**           domain store.
**
** Arguments:
**
** [f_poDomainStoreEnumContext]
**                          -- Pointer to an initialized domain store enumerator
**                             context by DRM_DOMST_InitEnumKey().
** [f_poAccountID]          -- Pointer to a variable that receives the account ID
**                             of the currently enumerated domain private key.
**                             This parameter is optional.
** [f_pdwRevision]          -- Pointer to a variable that receives the revision
**                             of the currently enumerated domain private key.
**                             This parameter is optional.
** [f_poSlotHint]           -- Pointer to a variable that receives the DST
**                             slot hint of the currently enumerated domain
**                             private key. This parameter is optional.
** [f_pcbData]              -- Pointer to a variable that receives the size
**                             (number of bytes) of the currently enumerated
**                             domain private key. This parameter is optional.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_NOMORE if there is no more data to be enumerated.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_DOMAIN_STORE_GET_DATA for all other
**                          errors.
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_DOMST_EnumNextKey(
    IN DRM_DOMAINSTORE_ENUM_CONTEXT *f_poDomainStoreEnumContext,
    OUT DRM_GUID *f_poAccountID,
    OUT DRM_DWORD *f_pdwRevision,
    OUT DRM_DST_SLOT_HINT *f_poSlotHint,
    OUT DRM_DWORD *f_pcbData )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_GUID oTypeID = EMPTY_DRM_GUID;

    ChkDR( _DomainStore_EnumNextData( f_poDomainStoreEnumContext,
                                      f_poAccountID,
                                      &oTypeID,
                                      NULL,
                                      f_poSlotHint,
                                      f_pcbData ) );

    if ( f_pdwRevision != NULL )
    {
        NETWORKBYTES_TO_DWORD( *f_pdwRevision, ( DRM_BYTE * )&oTypeID, 0 );
    }

ErrorExit:

    return dr;
}

/*********************************************************************
**
** Function: DRM_DOMST_CreateDomainStorePassword
**
** Synopsis: Generates a secure store password for the domain saved
**           datetime for the given account ID.
**
** Arguments:
**
** [f_poAccountID]          -- Pointer to an account ID of the domain.
**                             It cannot be NULL.
** [f_poBBXContext]         -- Pointer to a DRM_BB_CONTEXT context.
** [f_rgbPasswordSST]       -- Generated Domain Store Secure Store Password.
**
** Returns:                 DRM_SUCCESS on success.
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_DOMST_CreateDomainStorePassword(
    IN  const DRM_GUID          *f_poAccountID,
    IN  DRM_BB_CONTEXT          *f_poBBXContext,
    __out_bcount( DRM_SHA1_DIGEST_LEN ) DRM_BYTE f_rgbPasswordSST[__CB_DECL(DRM_SHA1_DIGEST_LEN)] )
{
    DRM_RESULT  dr  = DRM_SUCCESS;

    ChkDR( DRM_ARCLK_DOMST_CreateDomainStorePassword( f_poAccountID,
                                                      f_poBBXContext,
                                                      f_rgbPasswordSST ) );

ErrorExit:
    return dr;
}


EXIT_PK_NAMESPACE_CODE;
