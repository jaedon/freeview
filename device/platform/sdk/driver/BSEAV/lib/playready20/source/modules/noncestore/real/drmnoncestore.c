/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmcommon.h>
#include <drmnoncestore.h>

ENTER_PK_NAMESPACE_CODE;

/*********************************************************************
**
** Function: DRM_NST_Open
**
** Synopsis: API that opens a nonce store and returns an initialized
**           nonce store context and an associated license store context.
**
** Arguments:
**
** [f_pvOEMContext]         -- Pointer to an optional OEM context.
** [f_pbNonceStore]         -- Pointer to a buffer that is used to construct
**                             the internal embedded memory store to store
**                             licenses.
** [f_cbNonceStore]         -- Size of the buffer that is used to construct
**                             the internal embedded memory store to store
**                             licenses.
** [f_poLicStoreContext]    -- Pointer to a license store context that is
**                             associated with the nonce store being opened.
** [f_poNonceStore]         -- Pointer to a nonce store context to be
**                             initialized by the API.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_NONCE_STORE_OPEN_STORE if the nonce store
**                          is failed to be opened.
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_NST_Open(
    __in_opt DRM_VOID *f_pvOEMContext,
    __in_bcount( f_cbNonceStore ) DRM_BYTE *f_pbNonceStore,
    __in DRM_DWORD f_cbNonceStore,
    __out DRM_LICSTORE_CONTEXT *f_poLicStoreContext,
    __out DRM_NONCESTORE_CONTEXT *f_poNonceStore )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_NONCESTORE_CONTEXT_INTERNAL *poNonceStore =
        ( DRM_NONCESTORE_CONTEXT_INTERNAL * )f_poNonceStore;

    ChkArg( f_pbNonceStore != NULL && f_cbNonceStore > 0 );
    ChkArg( f_poLicStoreContext != NULL );
    ChkArg( f_poNonceStore != NULL );
    ChkArg( !poNonceStore->m_fInited );

    ZEROMEM( poNonceStore, SIZEOF( DRM_NONCESTORE_CONTEXT_INTERNAL ) );

    ChkDR( DRM_DST_Init( eDRM_DST_EMBEDDED,
                         ( DRM_DST_CONTEXT * )&poNonceStore->m_oESTContext,
                         SIZEOF( DRM_EST_CONTEXT ),
                         &poNonceStore->m_oDataStore ) );

    ChkDR( DRM_DST_CreateStore( f_pvOEMContext,
                                f_pbNonceStore,
                                f_cbNonceStore,
                                0,
                                eDRM_DSTBLKNUM_DWORD,
                                FALSE,
                                0,
                                &poNonceStore->m_oDataStore ) );

    ChkDR( DRM_LST_Open( f_poLicStoreContext,
                         &poNonceStore->m_oDataStore,
                         eDRM_LICENSE_STORE_XMR ) );

    poNonceStore->m_pvOEMContext = f_pvOEMContext;
    poNonceStore->m_poLicStoreContext = f_poLicStoreContext;
    poNonceStore->m_pbBuffer = f_pbNonceStore;
    poNonceStore->m_cbBuffer = f_cbNonceStore;
    poNonceStore->m_fInited = TRUE;

ErrorExit:

    if ( DRM_FAILED( dr ) &&
         dr != DRM_E_INVALIDARG )
    {
        dr = DRM_E_NONCE_STORE_OPEN_STORE;
    }

    return dr;
}

/*********************************************************************
**
** Function: DRM_NST_Close
**
** Synopsis: API that closes an opened nonce store.
**
** Arguments:
**
** [f_poNonceStore]         -- Pointer to an initialized nonce store context.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_NONCE_STORE_CLOSE_STORE if the nonce store
**                          is failed to be closed.
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_NST_Close(
    __in DRM_NONCESTORE_CONTEXT *f_poNonceStore )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_NONCESTORE_CONTEXT_INTERNAL *poNonceStore =
        ( DRM_NONCESTORE_CONTEXT_INTERNAL * )f_poNonceStore;

    ChkArg( poNonceStore != NULL );

    /*
    ** If a nonce store context has not yet been initialized,
    ** just exit the function without returning any error code.
    */
    if( !poNonceStore->m_fInited )
    {
        goto ErrorExit;
    }

    ChkArg( poNonceStore->m_poLicStoreContext != NULL );

    ChkDR( DRM_LST_Close( poNonceStore->m_poLicStoreContext ) );

    ChkDR( DRM_DST_CloseStore( &poNonceStore->m_oDataStore ) );

    ZEROMEM( poNonceStore, SIZEOF( DRM_NONCESTORE_CONTEXT_INTERNAL ) );

ErrorExit:

    if ( DRM_FAILED( dr ) &&
         dr != DRM_E_INVALIDARG )
    {
        dr = DRM_E_NONCE_STORE_CLOSE_STORE;
    }

    return dr;
}

/*********************************************************************
**
** Function: DRM_NST_GenerateNonce
**
** Synopsis: API that generates a nonce token with a random nonce value,
**           and caches it at the internal nonce token.
**
** Arguments:
**
** [f_poNonceStore]         -- Pointer to an initialized nonce store context.
** [f_poNonce]              -- Pointer to a variable that is used to receive
**                             the generated nonce value.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_NST_GenerateNonce(
    __in DRM_NONCESTORE_CONTEXT *f_poNonceStore,
    __out DRM_ID *f_poNonce )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_NONCESTORE_CONTEXT_INTERNAL *poNonceStore =
        ( DRM_NONCESTORE_CONTEXT_INTERNAL * )f_poNonceStore;

    ChkArg( poNonceStore != NULL &&
            poNonceStore->m_fInited );
    ChkArg( f_poNonce != NULL );

    /*
    ** If the internal nonce token is allocated (associated with a license)
    ** previously, need to delete its associated license.
    */
    if ( poNonceStore->m_oToken.m_fAllocated )
    {
        ChkDR( DRM_LST_DeleteLicense( poNonceStore->m_poLicStoreContext,
                                      &poNonceStore->m_oToken.m_oKID,
                                      &poNonceStore->m_oToken.m_oNonce,
                                      NULL ) );
    }

    ZEROMEM( &poNonceStore->m_oToken, SIZEOF( DRM_NONCE_TOKEN ) );

    /* Generate a random nonce value */
    ChkDR( Oem_Random_GetBytes( poNonceStore->m_pvOEMContext,
                                ( DRM_BYTE * )&poNonceStore->m_oToken.m_oNonce,
                                SIZEOF( DRM_ID ) ) );

    /* Return the generated nonce value to the caller. */
    MEMCPY( f_poNonce, &poNonceStore->m_oToken.m_oNonce, SIZEOF( DRM_ID ) );

ErrorExit:

    return dr;
}

/*********************************************************************
**
** Function: DRM_NST_AddLicense
**
** Synopsis: API that adds a XMR license to the internal license store.
**
** Arguments:
**
** [f_poNonceStore]         -- Pointer to an initialized nonce store context.
** [f_cbLicense]            -- Size of the license being added.
** [f_pbLicense]            -- Pointer to a buffer that contains the license
**                             being added. The first DWORD of the buffer
**                             should be reserved for the priority value.
** [f_poKID]                -- Pointer to the KID of the license being added.
** [f_poLID]                -- Pointer to the LID of the license being added.
** [f_dwPriority]           -- The priority of the license being added.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_NONCE_STORE_TOKEN_NOT_FOUND if the matching
**                          nonce store token is not found.
**                          DRM_E_NONCE_STORE_ADD_LICENSE if there is already
**                          a license associated with the nonce token.
**
** Note:                    The API first checks whether the passed in LID matches
**                          the nonce value of the internal nonce token. If matches,
**                          the license will be added to the internal license store,
**                          otherwise DRM_E_NONCE_STORE_TOKEN_NOT_FOUND will be
**                          returned to the caller.
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_NST_AddLicense(
    __in DRM_NONCESTORE_CONTEXT *f_poNonceStore,
    __in DRM_DWORD f_cbLicense,
    __in_bcount( f_cbLicense ) DRM_BYTE *f_pbLicense,
    __in DRM_KID *f_poKID,
    __in DRM_LID *f_poLID,
    __in DRM_DWORD f_dwPriority )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_NONCESTORE_CONTEXT_INTERNAL *poNonceStore =
        ( DRM_NONCESTORE_CONTEXT_INTERNAL * )f_poNonceStore;

    ChkArg( poNonceStore != NULL &&
            poNonceStore->m_fInited );
    ChkArg( f_cbLicense > 0 && f_pbLicense != NULL );
    ChkArg( f_poKID != NULL && f_poLID != NULL );

    /* Check whether the passed in LID matches the internal nonce. */
    if ( MEMCMP( f_poLID,
                 &poNonceStore->m_oToken.m_oNonce,
                 SIZEOF( DRM_ID ) ) != 0 )
    {
        ChkDR( DRM_E_NONCE_STORE_TOKEN_NOT_FOUND );
    }

    /*
    ** If there is already a license associated with the nonce token,
    ** return with an error.
    */
    ChkBOOL( !poNonceStore->m_oToken.m_fAllocated,
             DRM_E_NONCE_STORE_ADD_LICENSE );

    /*
    ** Remember the KID associated with the nonce token since
    ** it is used to delete the license.
    */
    MEMCPY( &poNonceStore->m_oToken.m_oKID,
            f_poKID,
            SIZEOF( DRM_ID ) );

    ChkDR( DRM_LST_AddLicense( poNonceStore->m_poLicStoreContext,
                               f_cbLicense,
                               f_pbLicense,
                               f_poKID,
                               f_poLID,
                               f_dwPriority ) );

    /*
    ** Set a flag indicating a license has been associated with
    ** the internal nonce token.
    */
    poNonceStore->m_oToken.m_fAllocated = TRUE;

ErrorExit:

    return dr;
}

EXIT_PK_NAMESPACE_CODE;
