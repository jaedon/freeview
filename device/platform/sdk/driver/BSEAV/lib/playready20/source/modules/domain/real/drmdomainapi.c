/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmcommon.h>
#include <drmtypes.h>
#include <drmhds.h>
#include <drmutilities.h>
#include <drmdomainstore.h>
#include <drmmanager.h>
#include <drmdomainapi.h>
#include <drmcontract.h>
#include <drmsoapxmlutility.h>
#include <drmrevocation.h>
#include <drmxmlparser.h>
#include <drmdomainconstants.h>
#include <drmxmlsigconstants.h>

ENTER_PK_NAMESPACE_CODE;

DRM_NO_INLINE DRM_API DRM_BOOL DRM_CALL DRM_DOMAIN_IsDomainSupported(DRM_VOID)
{
    CLAW_AUTO_RANDOM_CIPHER
    return TRUE;
}

DRM_API DRM_BOOL DRM_CALL DRM_DOMAIN_IsDomainUnsupported(DRM_VOID)
{
    return FALSE;
}

/*********************************************************************
**
** Function: Drm_JoinDomain_GenerateChallenge
**
** Synopsis: API that generates a domain join challenge to be sent to the
**           domain server to register a device to a domain. The challenge
**           includes the  identity of the device to be registered and
**           optionally the domain ID to which the device wants to be
**           registered.
**
** Arguments:
**
** [f_poAppContext]         -- Pointer to a DRM App context initialized via
**                             Drm_Initialize().
** [f_dwFlags]              -- Flag that indicates the type of custom data.
** [f_poDomainID]           -- Pointer to a domain ID to be registered with
**                             the server. Some of its fields (service ID,
**                             account ID or revision) may be blank.
** [f_pchFriendlyName]      -- Pointer to a character buffer that contains
**                             the friendly name. It can be NULL.
** [f_cchFriendlyName]      -- Size (number of characters) of the friendly
**                             name string.
** [f_pchData]              -- Pointer to a buffer that contains the custom data
**                             sent to the server. The format of the custom data
**                             is based on the value of f_dwFlags. It may be NULL.
**                             If the Query data is passed in with f_dwFlags == DRM_REGISTER_INDIRECT_QUERY_DATA
**                             then it should have the following XML Format:
**                             <JoinDomain type = \"challenge\">
**                              <Data>
**                               <ServiceID>Optional Base 64 encoded Service Account ID string in ANSI</ServiceID>
**                               <AccountID>Optional Base 64 encoded Domain Account ID string in ANSI</AccountID>
**                               <Revision>Optional Domain Revision</Revision>
**                               <FriendlyName>Optional Friendly Name in ANSI </FriendlyName>
**                               <CustomData>Optional Custom Data in ANSI</CustomData>
**                              </Data>
**                             </JoinDomain>
** [f_cchData]              -- Size (in characters) of the custom data buffer.
**                             It must be 0 if f_pchCustomData is NULL.
** [f_pbChallenge]          -- Pointer to a buffer to receive a generated challenge
**                             byte stream. If it is NULL, the function returns
**                             an error and the size of the required buffer is
**                             stored in *f_pcbChallenge.
** [f_pcbChallenge]         -- Pointer to a variable that contains the size (in bytes
**                             of the challenge buffer during input and receives the
**                             size (in bytes) of the challenge buffer during output.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_BUFFERTOOSMALL if the buffer to retrieve
**                          the data is not big enough.
**                          DRM_E_DEVCERTEXCEEDSIZELIMIT if the device certificate
**                          is too big.
**                          DRM_E_DOMAIN_INVALID_CUSTOM_DATA_TYPE if the custom data
**                          type is incorrect.
**                          DRM_E_DOMAIN_INVALID_CUSTOM_DATA if the custom data cannot
**                          be parsed.
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL Drm_JoinDomain_GenerateChallenge(
    IN                                         DRM_APP_CONTEXT *f_poAppContext,
    IN                                         DRM_DWORD        f_dwFlags,
    IN                                         DRM_DOMAIN_ID   *f_poDomainID,
    __in_ecount_opt( f_cchFriendlyName ) const DRM_CHAR        *f_pchFriendlyName,
    IN                                         DRM_DWORD        f_cchFriendlyName,
    __in_ecount_opt( f_cchData )         const DRM_CHAR        *f_pchData,
    IN                                         DRM_DWORD        f_cchData,
    __out_bcount_opt( *f_pcbChallenge )        DRM_BYTE        *f_pbChallenge,
    IN OUT                                     DRM_DWORD       *f_pcbChallenge )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_APP_CONTEXT_INTERNAL *poAppContextInternal =
        ( DRM_APP_CONTEXT_INTERNAL * )f_poAppContext;
    DRM_DWORD cbDeviceCert = 0;

    ChkArg( f_poAppContext != NULL );
    ChkArg( poAppContextInternal->fStoreOpened );

    /*
    ** Clear the previous response fields in the app context.
    */
    poAppContextInternal->fResponseSignatureChecked = FALSE;
    poAppContextInternal->dwLastResponseProtocol    = DRM_SOAPXML_PROTOCOL_UNKNOWN;

    /*
    ** Get the size (number of characters) of the device certificate.
    */
    ChkDR( DRM_SOAPXML_GetDeviceCert( poAppContextInternal->oBlackBoxContext.pKeyFileContext,
                                      FALSE,
                                      NULL,
                                      &cbDeviceCert ) );

    /*
    ** Use license buffer to store the device certificate.
    */
    if ( cbDeviceCert > poAppContextInternal->cbDRMLicense )
    {
        /*
        ** If size of the license buffer is smaller than the size of the
        ** device certificate, return with error.
        */
        ChkDR( DRM_E_DEVCERTEXCEEDSIZELIMIT );
    }

    if( f_pbChallenge != NULL )
    {
        /* Don't bother if we're not actually generating the challenge */
        ChkDR( DRM_SOAPXML_GetDeviceCert( poAppContextInternal->oBlackBoxContext.pKeyFileContext,
                                          FALSE,
                                          ( DRM_CHAR * )poAppContextInternal->pbDRMLicense,
                                          &cbDeviceCert ) );
    }

    ChkDR( DRM_DOM_GenerateJoinChallenge( poAppContextInternal->pOEMContext,
                                          &poAppContextInternal->oBlackBoxContext,
                                          &poAppContextInternal->oDomainStoreContext,
                                          f_dwFlags,
                                          f_poDomainID,
                                          f_pchFriendlyName,
                                          f_cchFriendlyName,
                                          f_pbChallenge == NULL ? NULL : poAppContextInternal->pbDRMLicense,
                                          cbDeviceCert,
                                          f_pchData,
                                          f_cchData,
                                          f_pbChallenge,
                                          f_pcbChallenge ) );

ErrorExit:

    ChkECC( ECC_Drm_JoinDomain_GenerateChallenge, dr );

    return dr;
}

/*********************************************************************
**
** Function: _ValidateDomJoinResponse
**
** Synopsis: Internal API that velidates a domain join response sent by
**           the domain registration server to ensure that the response is
**           properly signed when <signature> node is present or the caller
**           opt-in signature verification.
**
** Arguments:
**
** [f_poAppContext]         -- Pointer to a DRM App context initialized via
**                             Drm_Initialize().
** [f_dwFlags]              -- Flags of DRM_PROCESS_DJ_RESPONSE_FLAG
** [f_pdastrResponse]       -- Pointer to a DRM UTF8/ANSI string that
**                             contains a domain join response XML
**                             received from the server.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALID_DOMAIN_JOIN_RESPONSE_SIGNATURE
**                          DRM_E_DOMAIN_JOIN_RESPONSE_SIGNATURE_MISSING
**
**********************************************************************/
static DRM_NO_INLINE DRM_RESULT _ValidateDomJoinResponse(
    IN DRM_APP_CONTEXT              *f_poAppContext,
    IN DRM_PROCESS_DJ_RESPONSE_FLAG  f_dwFlags,
    IN DRM_ANSI_CONST_STRING        *f_pdastrResponse )
{
    DRM_RESULT                   dr                     = DRM_SUCCESS;
    DRM_SUBSTRING                dasstrResponse         = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING                dasstrSignature        = EMPTY_DRM_SUBSTRING;
    DRM_APP_CONTEXT_INTERNAL    *poAppContextInternal   = ( DRM_APP_CONTEXT_INTERNAL * )f_poAppContext;
    DRM_DWORD                    dwProtocolType         = DRM_SOAPXML_PROTOCOL_DOMAIN_JOIN_RESPONSE;

    ChkDRMANSIString( f_pdastrResponse );

    dasstrResponse.m_ich = 0;
    dasstrResponse.m_cch = f_pdastrResponse->cchString;

#if DRM_USE_SOAP

    ChkDR( DRM_SOAPXML_SkipSOAPHeaders( ( DRM_CHAR * )f_pdastrResponse->pszString,
                                        &dasstrResponse ) );

#endif /* DRM_USE_SOAP */

    /*
    ** Extract <Signature> node from
    ** <JoinDomainResult>/<Response> node.
    */
    dr = DRM_XML_GetSubNodeByPathA( f_pdastrResponse->pszString,
                                    &dasstrResponse,
                                    &g_dastrDomJoinSignaturePath,
                                    NULL,
                                    NULL,
                                    &dasstrSignature,
                                    NULL,
                                    g_chForwardSlash );
    if( dr == DRM_E_XMLNOTFOUND )
    {
        if( 0 != ( f_dwFlags & DRM_PROCESS_LIC_RESPONSE_SIGNATURE_NOT_REQUIRED ) )
        {
            /*
            ** Caller doesn't requests signature verification and there is no signature node in response.
            ** Continue response process.
            */
            dr = DRM_SUCCESS;
            goto ErrorExit;
        }

        ChkDR( DRM_E_DOMAIN_JOIN_RESPONSE_SIGNATURE_MISSING );
    }

    ChkDR( dr );

    /*
    ** Validate server's signing cert chain.
    */
    dr = DRM_RVK_VerifySigningCert( f_pdastrResponse,
                                    &dasstrResponse,
                                    &g_dastrDomJoinSigningCertPath,
                                    poAppContextInternal->pbDRMLicense,
                                    poAppContextInternal->cbDRMLicense,
                                    poAppContextInternal->pbRevocationBuffer,
                                    poAppContextInternal->cbRevocationBuffer,
                                    &poAppContextInternal->oBlackBoxContext,
                                    &poAppContextInternal->oSecStoreGlobalContext,
                                    &poAppContextInternal->oDatastoreHDS,
                                    &poAppContextInternal->oRevContext );

    if( dr == DRM_E_XMLNOTFOUND )
    {
        ChkDR( DRM_E_DOMAIN_JOIN_RESPONSE_SIGNATURE_MISSING );
    }

    if( dr == DRM_E_CERTIFICATE_REVOKED
     || dr == DRM_E_BCERT_BASICINFO_CERT_EXPIRED )
    {
        ChkDR( DRM_E_INVALID_DOMAIN_JOIN_RESPONSE_SIGNATURE );
    }
    ChkDR( dr );

    /*
    ** Validate signature.
    */
    dr = DRM_SOAPXML_ValidateProtocolSignature( (DRM_BYTE *)f_pdastrResponse->pszString,
                                                f_pdastrResponse->cchString,
                                                poAppContextInternal->pbDRMLicense,  /* use as scratch buffer for BCert.
                                                                                                             pbDRMLicense has minimum size of DRM_MAX_LICENSESIZE (10K) that
                                                                                                             should be enough for BCert. */
                                                poAppContextInternal->cbDRMLicense,
                                                &poAppContextInternal->oBlackBoxContext.CryptoContext,
                                                &dwProtocolType );
    if( dr == DRM_E_XMLSIG_ECDSA_VERIFY_FAILURE
     || dr == DRM_E_XMLSIG_SHA_VERIFY_FAILURE )
    {
        ChkDR( DRM_E_INVALID_DOMAIN_JOIN_RESPONSE_SIGNATURE );
    }

    ChkDR( dr );

ErrorExit:

    return dr;
}

/*********************************************************************
**
** Function: Drm_JoinDomain_ProcessResponse
**
** Synopsis: API that processes a domain join response sent by the domain
**           registration server. The domain response contains a domain
**           certificate and one or more domain private key that will be
**           added to the domain store by the device that calls the API.
**
** Arguments:
**
** [f_poAppContext]         -- Pointer to a DRM App context initialized via
**                             Drm_Initialize().
** [f_dwFlags]              -- Flags of DRM_PROCESS_DJ_RESPONSE_FLAG
** [f_pbResponse]           -- Pointer to a buffer that contains the domain
**                             join response from the domain registration
**                             server.
** [f_cbResponse]           -- Size of the domain join response from the domain
**                             registration server. It must be bigger than 0.
** [f_pResult]              -- Optional pointer to a variable that receives the
**                             status code.
** [f_poDomainID]           -- Optional pointer to a variable that receives the
**                             domain id contained in the domain join response.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_S_MORE_DATA if custom data is available from
**                          the server.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_SOAPXML_XML_FORMAT if SOAP format error happens.
**                          DRM_E_XMLNOTFOUND if expected XML node is not found.
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL Drm_JoinDomain_ProcessResponse(
    IN DRM_APP_CONTEXT *f_poAppContext,
    IN DRM_PROCESS_DJ_RESPONSE_FLAG f_dwFlags,
    __in_bcount( f_cbResponse ) DRM_BYTE *f_pbResponse,
    IN DRM_DWORD f_cbResponse,
    OUT DRM_RESULT *f_pResult,
    OUT DRM_DOMAIN_ID *f_poDomainID )
{
    DRM_RESULT                   dr                     = DRM_SUCCESS;
    DRM_ANSI_STRING              dastrResponse          = EMPTY_DRM_STRING;
    DRM_APP_CONTEXT_INTERNAL    *poAppContextInternal   = ( DRM_APP_CONTEXT_INTERNAL * )f_poAppContext;

    ChkArg( f_poAppContext != NULL );
    ChkArg( poAppContextInternal->fStoreOpened );
    ChkArg( f_dwFlags == DRM_PROCESS_DJ_RESPONSE_NO_FLAGS
         || 0 != ( f_dwFlags & DRM_PROCESS_DJ_RESPONSE_SIGNATURE_NOT_REQUIRED ) );

    DASTR_FROM_PB( &dastrResponse, f_pbResponse, f_cbResponse );
    ChkDRMANSIString( &dastrResponse );

    ChkDR( _ValidateDomJoinResponse( f_poAppContext,
                                     f_dwFlags,
                                     (DRM_ANSI_CONST_STRING *)&dastrResponse ) );

    ChkDR( DRM_DOM_ProcessJoinResponse( &poAppContextInternal->oBlackBoxContext,
                                        &poAppContextInternal->oDomainStoreContext,
                                        f_pbResponse,
                                        f_cbResponse,
                                        f_pResult,
                                        f_poDomainID ) );

ErrorExit:

    ChkECC( ECC_Drm_JoinDomain_ProcessResponse, dr );

    return dr;
}

/*********************************************************************
**
** Function: Drm_LeaveDomain_GenerateChallenge
**
** Synopsis: API that generates a domain leave challenge to be sent to the
**           domain registration server to unregister a device from a domain. The
**           challenge includes the identity of the device to be unregistered and
**           the domain ID to which the device wants to be unregistered.
**           The device will remove the domain certificate and the domain
**           private keys from its domain store.
**
** Arguments:
**
** [f_poAppContext]         -- Pointer to a DRM App context initialized via
**                             Drm_Initialize().
** [f_dwFlags]              -- Flag that indicates the type of custom data.
** [f_poDomainID]           -- Domain ID to be unregistered with the server. Some of
**                             its fields (service ID, account ID or revision)
**                             may be blank.
** [f_pchData]              -- Pointer to a buffer that contains the custom data
**                             sent to the server. The format of the custom data
**                             is based on the value of f_dwFlags. It may be NULL.
**                             If the Query data is passed in with f_dwFlags == DRM_REGISTER_INDIRECT_QUERY_DATA
**                             then it should have the following XML Format:
**                             <LeaveDomain type = \"challenge\">
**                              <Data>
**                               <ServiceID>Optional Base 64 encoded Service Account ID string in ANSI</ServiceID>
**                               <AccountID>Mandatory Base 64 encoded Domain Account ID string in ANSI</AccountID>
**                               <Revision>Optional Domain Revision</Revision>
**                               <CustomData>Optional Custom Data in ANSI</CustomData>
**                              </Data>
**                             </LeaveDomain>
** [f_cchData]              -- Size (in characters) of the custom data buffer.
**                             It must be 0 if f_pchCustomData is NULL.
** [f_pbChallenge]          -- Pointer to a buffer to receive a generated challenge
**                             byte stream. If it is NULL, the function returns an
**                             error and the size of the required buffer is stored
**                             in *f_pcbChallenge.
** [f_pcbChallenge]         -- Pointer to a variable that contains the size (in bytes
**                             of the challenge buffer during input and receives the
**                             size (in bytes) of the challenge buffer during output.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_BUFFERTOOSMALL if the buffer to retrieve
**                          the data is not big enough.
**                          DRM_E_DEVCERTEXCEEDSIZELIMIT if the device certificate
**                          is too big.
**                          DRM_E_DOMAIN_INVALID_CUSTOM_DATA_TYPE if the custom data
**                          type is incorrect.
**                          DRM_E_DOMAIN_INVALID_CUSTOM_DATA if the custom data cannot
**                          be parsed.
**                          DRM_E_DOMAIN_STORE_DELETE_DATA if domain data cannot be
**                          deleted.
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL Drm_LeaveDomain_GenerateChallenge(
    IN                                        DRM_APP_CONTEXT *f_poAppContext,
    IN                                        DRM_DWORD        f_dwFlags,
    IN                                        DRM_DOMAIN_ID   *f_poDomainID,
    __in_ecount_opt( f_cchData )        const DRM_CHAR        *f_pchData,
    IN                                        DRM_DWORD        f_cchData,
    __out_bcount_opt( *f_pcbChallenge )       DRM_BYTE        *f_pbChallenge,
    IN OUT                                    DRM_DWORD       *f_pcbChallenge )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_APP_CONTEXT_INTERNAL *poAppContextInternal =
        ( DRM_APP_CONTEXT_INTERNAL * )f_poAppContext;
    DRM_DWORD cbDeviceCert = 0;

    ChkArg( f_poAppContext != NULL );
    ChkArg( poAppContextInternal->fStoreOpened );

    /*
    ** Clear the previous response fields in the app context.
    */
    poAppContextInternal->fResponseSignatureChecked = FALSE;
    poAppContextInternal->dwLastResponseProtocol    = DRM_SOAPXML_PROTOCOL_UNKNOWN;

    /*
    ** Get the size (number of characters) of the device certificate.
    */
    ChkDR( DRM_SOAPXML_GetDeviceCert( poAppContextInternal->oBlackBoxContext.pKeyFileContext,
                                     FALSE,
                                     NULL,
                                     &cbDeviceCert ) );

    /*
    ** Use license buffer to store the device certificate.
    */
    if ( cbDeviceCert > poAppContextInternal->cbDRMLicense )
    {
        /*
        ** If size of the license buffer is smaller than the size of the
        ** device certificate, return with error.
        */
        ChkDR( DRM_E_DEVCERTEXCEEDSIZELIMIT );
    }


    if( f_pbChallenge != NULL )
    {
        /* Don't bother if we're not actually generating the challenge */
        ChkDR( DRM_SOAPXML_GetDeviceCert( poAppContextInternal->oBlackBoxContext.pKeyFileContext,
                                          FALSE,
                                          ( DRM_CHAR * )poAppContextInternal->pbDRMLicense,
                                          &cbDeviceCert ) );
    }

    ChkDR( DRM_DOM_GenerateLeaveChallenge( poAppContextInternal->pOEMContext,
                                           &poAppContextInternal->oBlackBoxContext,
                                           &poAppContextInternal->oDomainStoreContext,
                                           f_dwFlags,
                                           f_poDomainID,
                                           f_pbChallenge == NULL ? NULL : poAppContextInternal->pbDRMLicense,
                                           cbDeviceCert,
                                           f_pchData,
                                           f_cchData,
                                           f_pbChallenge,
                                           f_pcbChallenge ) );

ErrorExit:

    ChkECC( ECC_Drm_LeaveDomain_GenerateChallenge, dr );

    return dr;
}

/*********************************************************************
**
** Function: Drm_LeaveDomain_ProcessResponse
**
** Synopsis: API that processes a domain leave response received from
**           the domain server.
**
** Arguments:
**
** [f_poAppContext]         -- Pointer to a DRM App context initialized via
**                             Drm_Initialize().
** [f_pbResponse]           -- Pointer to a buffer that contains the domain
**                             leave response from the domain registration
**                             server.
** [f_cbResponse]           -- Size of the domain leave response from the domain
**                             registration server. It must be bigger than 0.
** [f_pResult]              -- Optional pointer to a variable that receives the
**                             status code.
**
** Returns:                 DRM_SUCCESS on success.
                            DRM_S_MORE_DATA if custom data is available from
**                          the server.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_SOAPXML_XML_FORMAT if SOAP format error happens.
**                          DRM_E_XMLNOTFOUND if expected XML node is not found.
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL Drm_LeaveDomain_ProcessResponse(
    IN DRM_APP_CONTEXT *f_poAppContext,
    __in_bcount( f_cbResponse ) DRM_BYTE *f_pbResponse,
    IN DRM_DWORD f_cbResponse,
    OUT DRM_RESULT *f_pResult )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_APP_CONTEXT_INTERNAL *poAppContextInternal =
        ( DRM_APP_CONTEXT_INTERNAL * )f_poAppContext;

    ChkArg( f_poAppContext != NULL );
    ChkArg( poAppContextInternal->fStoreOpened );

    ChkDR( DRM_DOM_ProcessLeaveResponse( f_pbResponse,
                                         f_cbResponse,
                                         f_pResult,
                                         &poAppContextInternal->oBlackBoxContext) );

ErrorExit:

    ChkECC( ECC_Drm_LeaveDomain_ProcessResponse, dr );

    return dr;
}

/*********************************************************************
**
** Function: Drm_DomainCert_Find
**
** Synopsis: API that retrieves a domain certificate from the domain
**           store for a specific domain.
**
** Arguments:
**
** [f_poAppContext]         -- Pointer to a DRM App context initialized via
**                             Drm_Initialize().
** [f_pDomainID]            -- Domain ID of the domain whose domain certificate will
**                             be retrieved. Only the account ID field is used.
** [f_pbDomainCert]         -- Pointer to a buffer to receive a domain certificate
**                             byte stream. If it is NULL, the function returns an
**                             error and the size of the required buffer is stored
**                             in *f_pcbDomainCert.
** [f_pcbDomainCert]        -- Pointer to a variable that contains the size (in bytes)
**                             of the domain certificate buffer during input and
**                             receives the size (in bytes) of the domain certificate
**                             buffer during output.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_BUFFERTOOSMALL if the buffer to retrieve
**                          the data is not big enough.
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL Drm_DomainCert_Find(
    IN DRM_APP_CONTEXT *f_poAppContext,
    IN DRM_DOMAIN_ID *f_poDomainID,
    __out_bcount_opt( *f_pcbDomainCert ) DRM_BYTE *f_pbDomainCert,
    IN OUT DRM_DWORD *f_pcbDomainCert )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_APP_CONTEXT_INTERNAL *poAppContextInternal =
        ( DRM_APP_CONTEXT_INTERNAL * )f_poAppContext;

    ChkArg( f_poAppContext != NULL );
    ChkArg( poAppContextInternal->fStoreOpened );

    /*
    ** DRM App context should have an already opened domain store during
    ** its initialization.
    */
    ChkDR( DRM_DOM_FindCert( &poAppContextInternal->oDomainStoreContext,
                             f_poDomainID,
                             f_pbDomainCert,
                             f_pcbDomainCert ) );

ErrorExit:

    ChkECC( ECC_Drm_DomainCert_Find, dr );

    return dr;
}

/*********************************************************************
**
** Function: Drm_DomainCert_InitEnum
**
** Synopsis: API that initializes an domain store enumerator context
**           for all the domain certificates.
**
** Arguments:
**
** [f_poAppContext]         -- Pointer to a DRM App context initialized via
**                             Drm_Initialize().
** [f_poDomainCertEnumContext]
**                          -- Pointer to a domain cert enumerator context
**                             for all the domain certificates. This API
**                             initialized the domain cert context.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL Drm_DomainCert_InitEnum(
    IN DRM_APP_CONTEXT *f_poAppContext,
    OUT DRM_DOMAIN_CERT_ENUM_CONTEXT *f_poDomainCertEnumContext )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_APP_CONTEXT_INTERNAL *poAppContextInternal =
        ( DRM_APP_CONTEXT_INTERNAL * )f_poAppContext;

    ChkArg( f_poAppContext != NULL );
    ChkArg( poAppContextInternal->fStoreOpened );

    ChkDR( DRM_DOM_InitCertEnum( &poAppContextInternal->oDomainStoreContext,
                                 f_poDomainCertEnumContext ) );

ErrorExit:

    ChkECC( ECC_Drm_DomainCert_InitEnum, dr );

    return dr;
}

/*********************************************************************
**
** Function: Drm_DomainCert_EnumNext
**
** Synopsis: API that enumerates the next domain certificate using an
**           initialized domain cert enumerator context.
**
** Arguments:
**
** [f_poDomainCertEnumContext]
**                          -- Pointer to an initialized domain cert
**                             enumerator context.
** [f_pcchDomainCert]       -- Pointer to a variable that receives the size
**                             (in characters) of the domain cert being
**                             enumerated. Caller should allocate buffer using
**                             the returned size and then call Drm_DomainCert_Find()
**                             to actually read the domain certificate.
** [f_poDomainCertInfo]     -- Pointer to a structure that receives additional
**                             information of the domain certificates retrieved.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_NOMORE when there is no more element to be
**                          enumerated next.
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL Drm_DomainCert_EnumNext(
    IN DRM_DOMAIN_CERT_ENUM_CONTEXT *f_poDomainCertEnumContext,
    OUT DRM_DWORD *f_pcchDomainCert,
    OUT DRM_DOMAINCERT_INFO *f_poDomainCertInfo )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkDR( DRM_DOM_EnumNextCert( f_poDomainCertEnumContext,
                                 f_pcchDomainCert,
                                 f_poDomainCertInfo ) );

ErrorExit:

    ChkECC( ECC_Drm_DomainCert_EnumNext, dr );

    return dr;
}

EXIT_PK_NAMESPACE_CODE;

