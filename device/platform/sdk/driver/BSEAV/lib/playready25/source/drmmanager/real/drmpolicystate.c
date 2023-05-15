/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_DRMPOLICYSTATE_C
#include <drmmanager.h>
#include <drmsecurestore.h>
#include <drmlastinclude.h>

ENTER_PK_NAMESPACE_CODE;

/*********************************************************************
**
** Function: Drm_Policy_GetStateData
**
** Synopsis: API that retrieves the saves state data for a specific KID.
**
** Arguments:
**
** [f_poAppContext]         -- Pointer to an initialized DRM_APP_CONTEXT.
** [f_pdstrKID]             -- Pointer to a DRM string that contains the
**                             B64 encoded KID that owns the state data.
** [f_pdstrStateName]       -- Pointer to DRM string that is the name of
**                             the state being retrieved.
** [f_pbStateData]          -- Pointer to a byte buffer to receive the
**                             state data. If it is NULL, the required
**                             size is stored in *f_pcbStateData.
** [f_pcbStateData]         -- Pointer to a variable that indicates the
**                             size of the buffer to receive the state
**                             data. It will be set to the required or
**                             actual size of the data when the API
**                             returns.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_POLICYSTATE_NOT_FOUND if the state is
**                          not found.
**                          DRM_E_POLICYSTATE_CORRUPTED if the internal type
**                          of the state is not of byte BLOB.
**                          DRM_E_BUFFERTOOSMALL if the buffer to receive
**                          the state data is not big enough.
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL Drm_Policy_GetStateData(
    __in    DRM_APP_CONTEXT         *f_poAppContext,
    __in    const DRM_CONST_STRING  *f_pdstrKID,
    __in    const DRM_CONST_STRING  *f_pdstrStateName,
    __out_bcount_opt( *f_pcbStateData )
            DRM_BYTE                *f_pbStateData,
    __inout DRM_DWORD               *f_pcbStateData )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_APP_CONTEXT_INTERNAL *poAppContextInternal = ( DRM_APP_CONTEXT_INTERNAL* )f_poAppContext;
    DRM_ID oKID = { 0 };
    DRM_BYTE rgbPassword[ __CB_DECL( DRM_SHA1_DIGEST_LEN ) ] = { 0 };
    DRM_SECSTORE_CONTEXT oSecStoreContext = { 0 };
    TOKEN oToken = { 0 };
    DRM_DWORD cbRequiredSize = 0;

    ChkArg( f_poAppContext != NULL );
    ChkDRMString( f_pdstrKID );
    ChkDRMString( f_pdstrStateName );
    ChkArg( f_pcbStateData != NULL );
    ChkArg( f_pbStateData != NULL || *f_pcbStateData == 0 );

    ChkDR( DRM_UTL_DecodeKID( f_pdstrKID, &oKID ) );

    ChkDR( DRM_SST_CreateGlobalStorePassword( poAppContextInternal->pOEMContext, rgbPassword,
                                              ( DRM_BYTE * )&poAppContextInternal->oBlackBoxContext ) );

    ChkDR( DRM_SST_OpenKeyTokens( &oSecStoreContext,
                                  &oKID,
                                  NULL,
                                  rgbPassword,
                                  DRM_SECURE_STORE_OPEN_ALWAYS,
                                  SECURE_STORE_LICENSE_DATA,
                                  &poAppContextInternal->oDatastoreHDS ) );

    ChkDR( DRM_SST_GetTokenValue( &oSecStoreContext,
                                  f_pdstrStateName,
                                  &oToken ) );

    if ( dr == DRM_S_FALSE )
    {
        ChkDR( DRM_E_POLICYSTATE_NOT_FOUND );
    }

    switch ( oToken.TokenType )
    {
        /* Only byte BLOB type is supported. */
        case TOKEN_BYTEBLOB:
                    cbRequiredSize = oToken.val.byteValue.cbBlob;
                    break;

        default:
                    ChkDR( DRM_E_POLICYSTATE_CORRUPTED );
                    break;
    }

    if ( f_pbStateData == NULL || *f_pcbStateData < cbRequiredSize )
    {
        *f_pcbStateData = cbRequiredSize;

        ChkDR( DRM_E_BUFFERTOOSMALL );
    }

    MEMCPY( f_pbStateData, oToken.val.byteValue.pbBlob, cbRequiredSize );
    *f_pcbStateData = cbRequiredSize;

ErrorExit:

    if ( poAppContextInternal != NULL )
    {
        DRM_SST_CloseKey( &oSecStoreContext,
                          &poAppContextInternal->oDatastoreHDS,
                          NULL,
                          NULL  );
    }

    return dr;
}

/*********************************************************************
**
** Function: Drm_Policy_SetStateData
**
** Synopsis: API that stores the state data for a specific KID.
**
** Arguments:
**
** [f_poAppContext]         -- Pointer to an initialized DRM_APP_CONTEXT.
** [f_pdstrKID]             -- Pointer to a DRM string that contains the
**                             B64 encoded KID that owns the state data.
** [f_pdstrStateName]       -- Pointer to DRM string that is the name of
**                             the state to be stored.
** [f_pbStateData]          -- Pointer to a byte buffer that contains the
**                             state data.
** [f_cbStateData]          -- The size of the buffer that contains the
**                             state data.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL Drm_Policy_SetStateData(
    __in    DRM_APP_CONTEXT         *f_poAppContext,
    __in    const DRM_CONST_STRING  *f_pdstrKID,
    __in    const DRM_CONST_STRING  *f_pdstrStateName,
    __in_bcount( f_cbStateData )
            DRM_BYTE                *f_pbStateData,
    __inout DRM_DWORD                f_cbStateData )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_APP_CONTEXT_INTERNAL *poAppContextInternal = ( DRM_APP_CONTEXT_INTERNAL* )f_poAppContext;
    DRM_ID oKID = { 0 };
    DRM_BYTE rgbPassword[ __CB_DECL( DRM_SHA1_DIGEST_LEN ) ] = { 0 };
    DRM_SECSTORE_CONTEXT oSecStoreContext = { 0 };
    TOKEN oToken = { 0 };

    ChkArg( f_poAppContext != NULL );
    ChkDRMString( f_pdstrKID );
    ChkDRMString( f_pdstrStateName );
    ChkArg( f_pbStateData != NULL && f_cbStateData > 0 );

    ChkDR( DRM_UTL_DecodeKID( f_pdstrKID, &oKID ) );

    ChkDR( DRM_SST_CreateGlobalStorePassword( poAppContextInternal->pOEMContext, rgbPassword,
                                              ( DRM_BYTE * )&poAppContextInternal->oBlackBoxContext ) );

    ChkDR( DRM_SST_OpenKeyTokens( &oSecStoreContext,
                                  &oKID,
                                  NULL,
                                  rgbPassword,
                                  DRM_SECURE_STORE_OPEN_ALWAYS,
                                  SECURE_STORE_LICENSE_DATA,
                                  &poAppContextInternal->oDatastoreHDS ) );

    oToken.TokenType = TOKEN_BYTEBLOB;
    oToken.val.byteValue.pbBlob = f_pbStateData;
    oToken.val.byteValue.cbBlob = f_cbStateData;

    ChkDR( DRM_SST_SetExplicitResolutionTokenValue( &oSecStoreContext,
                                                    f_pdstrStateName,
                                                    &oToken,
                                                    DRM_TOKEN_RESOLVE_LAST_WINS ) );

ErrorExit:

    if ( poAppContextInternal != NULL )
    {
        DRM_SST_CloseKey( &oSecStoreContext,
                          &poAppContextInternal->oDatastoreHDS,
                          NULL,
                          NULL  );
    }

    return dr;
}

EXIT_PK_NAMESPACE_CODE;
