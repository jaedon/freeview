/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmcontentrevocation.h>
#include <drmheaderparser.h>
#include <drmutilitieslite.h>
#include <drmwmdrm.h>
#include <drmwmdrmconstants.h>

ENTER_PK_NAMESPACE_CODE;

/******************************************************************************
**
** Function: _GetContentRevocationInStore
**
** Synopsis: Get the content revocation data from the data store
**
** Arguments:
**      [f_pcontextLicEval]        : Specifies the liceval context to check
**      [f_pdstrContentOwnerPubKey]: Specifies the public key of the content owner
**      [f_pdstrContentRevocation] : Returns the content revocation data
**      [f_pDatastore]             : Specifies the DST context
**
** Returns :
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          One of the parameters is NULL or f_pdstrContentOwnerPubKey is
**          an invalid string
**
******************************************************************************/
static DRM_RESULT DRM_CALL _GetContentRevocationInStore(
    IN  const DRM_LICEVAL_CONTEXT *f_pcontextLicEval,
    IN  const DRM_CONST_STRING    *f_pdstrContentOwnerPubKey,
    OUT       DRM_CONST_STRING    *f_pdstrContentRevocation,
    IN  const DRM_DST             *f_pDatastore )
{
    DRM_RESULT       dr         = DRM_SUCCESS;
    DRM_DWORD        cbBuffer   = 0;
    DRM_LID          lid        = { 0 };
    DRM_SHA_CONTEXT  contextSHA = DRM_SHA_CONTEXT_DEFAULT;
    DRM_BYTE         rgbPassword[__CB_DECL( DRM_SHA1_DIGEST_LEN )] = { 0 };

    ChkArg( f_pcontextLicEval        != NULL );
    ChkArg( f_pDatastore             != NULL );
    ChkArg( f_pdstrContentRevocation != NULL );

    ChkDRMString( f_pdstrContentOwnerPubKey );

    ChkDR( DRM_SHA_Init( &contextSHA, eDRM_SHA_1 ) );
    ChkDR( DRM_SHA_Update( PB_DSTR ( f_pdstrContentOwnerPubKey ),
                           CB_DSTR ( f_pdstrContentOwnerPubKey ),
                          &contextSHA ) );

    ChkDR( DRM_SHA_Finalize( &contextSHA, DRM_SHA1_DIGEST_LEN, rgbPassword ) );

    MEMCPY( lid.rgb, rgbPassword, SIZEOF( lid.rgb ) );

    ChkDR( DRM_SST_CreateLicenseStatePassword( &lid,
                                                rgbPassword,
                                                f_pcontextLicEval->pcontextBBX ) );

    cbBuffer = f_pcontextLicEval->cbRevocationBuffer;

    ChkDR( DRM_SST_GetData( f_pcontextLicEval->pcontextSSTRevocation,
                           &lid,
                            NULL,
                            rgbPassword,
                            SECURE_STORE_REVOCATION_DATA,
                            ( DRM_DST * )f_pDatastore,
                            f_pcontextLicEval->pbRevocationBuffer,
                           &cbBuffer ) );

    DSTR_FROM_PB( f_pdstrContentRevocation, f_pcontextLicEval->pbRevocationBuffer, cbBuffer );

ErrorExit:
    return( dr );
}

/******************************************************************************
**
** Function: _SetContentRevocationInStore
**
** Synopsis: Set the content revocation data in the data store
**
** Arguments:
**      [f_pcontextLicEval]        : Specifies the liceval context to check
**      [f_pdstrContentOwnerPubKey]: Specifies the public key of the content owner
**      [f_pdstrContentRevocation] : Specifies the content revocation data
**      [f_pDatastore]             : Specifies the DST context
**
** Returns :
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          One of the parameters is NULL or an invalid DRM string
**      LICEVAL_UPDATE_FAILURE
**          Unable to update the license eval context for revocation
**
******************************************************************************/
static DRM_RESULT DRM_CALL _SetContentRevocationInStore(
    IN  const DRM_LICEVAL_CONTEXT *f_pcontextLicEval,
    IN  const DRM_CONST_STRING    *f_pdstrContentOwnerPubKey,
    IN  const DRM_CONST_STRING    *f_pdstrContentRevocation,
    IN OUT    DRM_DST             *f_pDatastore )
{
    DRM_RESULT       dr         = DRM_SUCCESS;
    DRM_LID          lid        = { 0 };
    DRM_SHA_CONTEXT  contextSHA = DRM_SHA_CONTEXT_DEFAULT;
    DRM_BYTE         rgbPassword [__CB_DECL( DRM_SHA1_DIGEST_LEN )] = { 0 };

    ChkArg( f_pcontextLicEval != NULL );
    ChkArg( f_pDatastore      != NULL );

    ChkDRMString( f_pdstrContentOwnerPubKey );
    ChkDRMString( f_pdstrContentRevocation );

    ChkDR( DRM_SHA_Init( &contextSHA, eDRM_SHA_1 ) );
    ChkDR( DRM_SHA_Update( PB_DSTR ( f_pdstrContentOwnerPubKey ),
                           CB_DSTR ( f_pdstrContentOwnerPubKey ),
                          &contextSHA ) );

    ChkDR( DRM_SHA_Finalize( &contextSHA,  DRM_SHA1_DIGEST_LEN, rgbPassword ) );

    MEMCPY( lid.rgb, rgbPassword, SIZEOF( lid.rgb ) );

    ChkDR( DRM_SST_CreateLicenseStatePassword( &lid,
                                                rgbPassword,
                                                f_pcontextLicEval->pcontextBBX ) );
    ChkDR( DRM_SST_SetData( f_pcontextLicEval->pcontextSSTRevocation,
                           &lid,
                            NULL,
                            rgbPassword,
                            SECURE_STORE_REVOCATION_DATA,
                            f_pDatastore,
                            PB_DSTR( f_pdstrContentRevocation ),
                            CB_DSTR( f_pdstrContentRevocation ) ) );

ErrorExit:
    if ( DRM_FAILED( dr ) )
    {
        dr = DRM_E_LICEVAL_UPDATE_FAILURE;
    }
    return dr;
}

/******************************************************************************
**
** Function: DRM_CONTENTREVOCATION_LEVL_SetOrUpdateContentRevocation
**
** Synopsis:
**
** Arguments:
**      [f_pcontextLicEval] : Specifies the liceval context to check
**      [f_pDatastore]      : Specifies the DST context
**
** Returns :
**      DRM_SUCCESS
**          Success
**      LICEVAL_UPDATE_FAILURE
**          Unable to update the license eval context for revocation
**
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_CONTENTREVOCATION_LEVL_SetOrUpdateContentRevocation(
          DRM_LICEVAL_CONTEXT *f_pContextLicEval,
    const DRM_DST             *f_pDatastore )
{
    DRM_RESULT       dr                            = DRM_SUCCESS;
    DRM_CONST_STRING dstrContentRev                = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrContentOwnerPubkey        = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrSecStateContentRevocation = EMPTY_DRM_STRING;
    DRM_DWORD        dwLicSequenceNumber           = 0;
    DRM_DWORD        dwSecStateSequenceNumber      = 0;
    DRM_DWORD        dwIndex                       = 0;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRMLICEVAL, PERF_FUNC_DRM_CONTENTREVOCATION_LEVL_SetOrUpdateContentRevocation );

    /*
    ** Retrieve content revocation from the license one by one and process.
    */
    while( DRM_SUCCESS == dr )
    {
        dr = DRM_CONTENTREVOCATION_LIC_GetContentRevocation( &( f_pContextLicEval->dstrContentLicense ),
                                              dwIndex,
                                             &dwLicSequenceNumber,
                                             &dstrContentRev,
                                             &dstrContentOwnerPubkey,
                                              NULL,
                                             &f_pContextLicEval->pcontextBBX->CryptoContext );
        if( DRM_SUCCESS == dr )
        {
            if( DRM_SUCCESS != _GetContentRevocationInStore( f_pContextLicEval, &dstrContentOwnerPubkey, &dstrSecStateContentRevocation, f_pDatastore )
             || DRM_SUCCESS != DRM_CONTENTREVOCATION_LIC_VerifyContentRevocation( &dstrSecStateContentRevocation,
                                                                 NULL,
                                                                &dwSecStateSequenceNumber,
                                                                 NULL,
                                                                 NULL,
                                                                &f_pContextLicEval->pcontextBBX->CryptoContext )
             || dwSecStateSequenceNumber < dwLicSequenceNumber )
            {
                /*
                ** The one in secure state seems older. Write the new one
                */
                ChkDR( _SetContentRevocationInStore( f_pContextLicEval,
                                                    &dstrContentOwnerPubkey,
                                                    &dstrContentRev,
                                          (DRM_DST*)f_pDatastore ) );
            }
        }
        dwIndex++;
    }

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/******************************************************************************
**
** Function: DRM_CONTENTREVOCATION_LEVL_IsLicenseRevoked
**
** Synopsis: Check whether a license is revoked.
**
** Arguments:
**      [f_pcontextLicEval] : Specifies the liceval context to check
**      [f_pDatastore]      : Specifies the DST context
**
** Returns :
**      TRUE
**          The license is revoked
**      FALSE
**          The license is not revoked
**
******************************************************************************/
DRM_API DRM_BOOL DRM_CALL DRM_CONTENTREVOCATION_LEVL_IsLicenseRevoked(
    IN const DRM_LICEVAL_CONTEXT *f_pContextLicEval,
    IN const DRM_DST             *f_pDatastore )
{
    DRM_RESULT       dr                             = DRM_SUCCESS;
    DRM_CONST_STRING dstrContentPubKey              = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrSecStateContentRevocation  = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrCondition                  = EMPTY_DRM_STRING;
    DRM_BOOL         fResult                        = FALSE;

    /*
    ** Check if the license has public key. If not, it cannot be revoked.
    */
    ChkDR( DRM_WMDRM_LIC_GetAttribute( &( f_pContextLicEval->dstrContentLicense ),
                                        NULL,
                                        DRM_LICENSE_ATTRIB_CONTENTPUBKEY,
                                        NULL,
                                       &dstrContentPubKey,
                                        0 ) ); /* optional. Don't check for errors. */

    if ( dstrContentPubKey.pwszString != NULL )
    {
        /*
        ** License contains content owner public key. Retrieve content revocation section
        ** for this public key, if any.
        */
        ChkDR( _GetContentRevocationInStore( f_pContextLicEval,
                                           &dstrContentPubKey,
                                           &dstrSecStateContentRevocation,
                                            f_pDatastore ) );

        ChkDR( DRM_CONTENTREVOCATION_LIC_VerifyContentRevocation( &dstrSecStateContentRevocation,
                                                 NULL,
                                                 NULL,
                                                 NULL,
                                                &dstrCondition,
                                                &f_pContextLicEval->pcontextBBX->CryptoContext ) );

        /*
        ** Evaluate the condition to determine if it allows the use of license.
        */

        /*
        ** missing condition is treated as the one that allows use of license.
        */
        if ( dstrCondition.pwszString != NULL && dstrCondition.cchString != 0 )
        {

            /*
            ** DRM_WMDRM_EXPR_LEVL_EvaluateExpression will return FALSE if the expression evaluates
            ** correctly and the license is revoked. So we can set fResult to TRUE intially
            ** and then invert it ( if fResult is TRUE on the way out of
            ** DRM_WMDRM_EXPR_LEVL_EvaluateExpression then the license IS NOT revoked.
            ** If it is false then the license IS revoked
            */
            fResult = TRUE;
            ChkDR( DRM_WMDRM_EXPR_LEVL_EvaluateExpression( (DRM_LICEVAL_CONTEXT*)f_pContextLicEval, &dstrCondition, &fResult ) );
            fResult = !fResult;
        }
    }

ErrorExit:
    return fResult;
}

DRM_API DRM_RESULT DRM_CALL DRM_CONTENTREVOCATION_LEVL_VerifyHeaderSignature(
    __inout   DRM_LICEVAL_CONTEXT          *f_pContextLicEval,
    __in      eDRM_LICEVAL_OPERATION_STATE  f_eOperationState,
    __in      DRM_CONST_STRING             *f_pdstrKID,
    __in      DRM_CONST_STRING             *f_pdstrLIData )
{
    DRM_RESULT        dr            = DRM_SUCCESS;
    DRM_LONG          lResult       = 0;
    DRM_CONST_STRING  dstrHeaderKID = EMPTY_DRM_STRING;

    /*
    ** Fetch KID, ISSUEDATE, and CONTENTPUBKEY from the license.
    */
    if ( f_eOperationState == DRM_LICENSE_EVAL_CAN_DO_OPERATION
        && f_pContextLicEval->dstrContentHeader.pwszString != NULL
        && f_pContextLicEval->dstrContentHeader.cchString  > 0
        && f_pContextLicEval->dwChainDepth                 == 0 )
    {
        dr = DRM_HDR_GetAttribute( &( f_pContextLicEval->dstrContentHeader ),
                                    NULL,
                                    DRM_HEADER_ATTRIB_KID,
                                    &dstrHeaderKID,
                                    0 );
        if ( dr == DRM_SUCCESS )
        {
            /*
            ** Match KID only if it can be retrieved from header.
            ** Thus, any license can be selected for an empty content header as long
            ** as the condition allows. If the condition refers to content data,
            ** then it will fail.
            ** Compare the KID
            */
            if ( !f_pContextLicEval->fUseCachedAttribs )
            {
                ChkBOOL( DRM_UTL_DSTRStringsEqual( f_pdstrKID, &dstrHeaderKID ),
                            DRM_E_LICEVAL_KID_MISMATCH );

                DRM_WMDRM_LIC_GetAttribute( &( f_pContextLicEval->dstrContentLicense ),
                                              NULL,
                                              DRM_LICENSE_ATTRIB_CONTENTPUBKEY,
                                              f_pdstrLIData,
                                              &f_pContextLicEval->rgdstrCachedAttribs[eDRM_LIC_CACHED_ATTRIB_CONTENTPUBKEY],
                                              0 );
            }
            else
            {
                ChkBOOL( DRM_UTL_DSTRStringsEqual( &f_pContextLicEval->rgdstrCachedAttribs[eDRM_LIC_CACHED_ATTRIB_KID],
                                                    &dstrHeaderKID ), DRM_E_LICEVAL_KID_MISMATCH );
            }

            if ( f_pContextLicEval->rgdstrCachedAttribs[eDRM_LIC_CACHED_ATTRIB_CONTENTPUBKEY].cchString != 0 )
            {
                ChkDR( DRM_HDR_Verify( &( f_pContextLicEval->dstrContentHeader ),
                                        &( f_pContextLicEval->rgdstrCachedAttribs[eDRM_LIC_CACHED_ATTRIB_CONTENTPUBKEY] ),
                                        &f_pContextLicEval->pcontextBBX->CryptoContext,
                                        &lResult ) );
                ChkBOOL( lResult != 0, DRM_E_CH_UNABLE_TO_VERIFY );
            }
        }
    }

ErrorExit:
    return dr;
}

EXIT_PK_NAMESPACE_CODE;

