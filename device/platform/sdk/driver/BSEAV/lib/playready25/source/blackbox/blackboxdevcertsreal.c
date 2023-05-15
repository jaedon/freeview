/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_BLACKBOXDEVCERTSREAL_C

#include <blackboxdevcerts.h>
#include <drmrevocation.h>
#include <oemhal.h>
#include <blackboxhal.h>
#include <drmbcertparser.h>
#include <drmbcrl.h>
#include <drmconstants.h>

#include <drmlastinclude.h>

ENTER_PK_NAMESPACE_CODE;

/*
** Functions defining whether OEM HAL device certificate validation is supported
*/
DRM_API DRM_NO_INLINE DRM_BOOL DRM_CALL DRM_BBX_IsHalDevCertValidationSupported(DRM_VOID)
{
    return OEM_HAL_IsHalDevCertValidationSupported();
}


DRM_API DRM_NO_INLINE DRM_BOOL DRM_CALL DRM_BBX_IsHalDevCertValidationUnsupported(DRM_VOID)
{
    return OEM_HAL_IsHalDevCertValidationUnsupported();
}


static DRM_NO_INLINE DRM_RESULT _UpdateRevInfo(
    __inout_ecount( 1 )                               DRM_BB_CONTEXT                  *f_poBBXContext,
    __in_ecount_opt( f_cbRevInfo )              const DRM_BYTE                        *f_pbRevInfo,
    __in                                              DRM_DWORD                        f_cbRevInfo ) DRM_NO_INLINE_ATTRIBUTE;


static DRM_NO_INLINE DRM_RESULT _UpdateCRL(
    __inout_ecount( 1 )                               DRM_BB_CONTEXT                  *f_poBBXContext,
    __in_ecount_opt( f_cbCertificateTargetCRL ) const DRM_BYTE                        *f_pbCertificateTargetCRL,
    __in                                              DRM_DWORD                        f_cbCertificateTargetCRL ) DRM_NO_INLINE_ATTRIBUTE;


static DRM_NO_INLINE DRM_RESULT _ResolveSigningKey(
    __inout_ecount( 1 )                           DRM_BB_CONTEXT    *f_poBBXContext,
    __in_ecount_opt( f_cbCertificateChain ) const DRM_BYTE          *f_pbCertificateChain,
    __in                                          DRM_DWORD          f_cbCertificateChain,
    __out_ecount(1)                               OEM_CRYPTO_HANDLE *f_phSigningKey ) DRM_NO_INLINE_ATTRIBUTE;


DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL _ValidatePublicKey(
    __inout_ecount( 1 )                               DRM_BB_CONTEXT                  *f_poBBXContext,
    __in                                        const DRM_PUBLIC_KEY_CONTEXT          *f_poECC256PubKeyTarget,
    __in_ecount_opt( f_cbCertificateTarget )    const DRM_BYTE                        *f_pbCertificateTarget,
    __in                                              DRM_DWORD                        f_cbCertificateTarget,
    __in_ecount_opt( f_cbCertificateTargetCRL ) const DRM_BYTE                        *f_pbCertificateTargetCRL,
    __in                                              DRM_DWORD                        f_cbCertificateTargetCRL,
    __in_ecount_opt( f_cbRevInfo )              const DRM_BYTE                        *f_pbRevInfo,
    __in                                              DRM_DWORD                        f_cbRevInfo )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_poBBXContext           != NULL );
    ChkArg( f_poECC256PubKeyTarget   != NULL );

    /*
    ** Only try to update revocation info if the HAL supports it
    */
    if( DRM_BBX_IsHalDevCertValidationSupported() )
    {
        if( f_pbCertificateTargetCRL != NULL )
        {
            /*
            ** Update the CRL first, so that there isn't a gap where
            ** all keys are invalidated and revalidated in the case
            ** the RIV and CRL are updated together
            */
            ChkDR( _UpdateCRL( f_poBBXContext,
                               f_pbCertificateTargetCRL,
                               f_cbCertificateTargetCRL ) );
        }

        if( f_pbRevInfo != NULL )
        {
            /*
            ** Second, update the revocation info, if necessary
            */
            ChkDR( _UpdateRevInfo( f_poBBXContext,
                                   f_pbRevInfo,
                                   f_cbRevInfo ) );
        }
    }

    ChkArg( f_poECC256PubKeyTarget->hPublicKey != OEM_CRYPTO_HANDLE_INVALID );

ErrorExit:

    return dr;
}


static DRM_NO_INLINE DRM_RESULT _UpdateRevInfo(
    __inout_ecount( 1 )                               DRM_BB_CONTEXT                  *f_pBbxContext,
    __in_ecount_opt( f_cbRevInfo )              const DRM_BYTE                        *f_pbRevInfo,
    __in                                              DRM_DWORD                        f_cbRevInfo )
{
    DRM_RESULT        dr          = DRM_E_LOGICERR;
    DRM_RLVI          rlvi        = { 0 };
    OEM_CRYPTO_HANDLE hSigningKey = OEM_CRYPTO_HANDLE_INVALID;

    ChkArg( f_pBbxContext != NULL );
    ChkArg( f_pbRevInfo   != NULL );
    ChkArg( f_cbRevInfo   >  0 );

    ChkDR( DRM_RVK_ParseRevocationInfo( f_pbRevInfo,
                                        f_cbRevInfo,
                                        &rlvi,
                                        NULL ) );

    /*
    ** Check that this RevInfo structure is as expected
    */
    ChkBOOL( rlvi.head.bFormatVersion      == RLVI_FORMAT_VERSION_V2, DRM_E_INVALID_REVOCATION_LIST );
    ChkBOOL( rlvi.signature.bSignatureType == RLVI_SIGNATURE_TYPE_2,  DRM_E_INVALID_REVOCATION_LIST );

    if( rlvi.head.dwRIV > f_pBbxContext->seenRevInfoVersions.dwRevInfoSequenceNumber )
    {
        /*
        ** Find the signing key for the RIV data
        */
        ChkDR( _ResolveSigningKey( f_pBbxContext,
                                   f_pbRevInfo + rlvi.certchain.ibCertChain,
                                   rlvi.certchain.cbCertChain,
                                   &hSigningKey ) );

        /*
        ** Send the RIV data to the HAL
        */
        ChkDR( _Hal_LoadPlayReadyRevocationInfo( f_pBbxContext,
                                                 f_pbRevInfo,
                                                 f_cbRevInfo,
                                                 hSigningKey ) );

        f_pBbxContext->seenRevInfoVersions.dwRevInfoSequenceNumber = rlvi.head.dwRIV;
    }

ErrorExit:

    DRM_BBX_SafeReleaseKey( f_pBbxContext, &hSigningKey );

    return dr;
}


static DRM_NO_INLINE DRM_RESULT _UpdateCRL(
    __inout_ecount( 1 )                               DRM_BB_CONTEXT                  *f_pBbxContext,
    __in_ecount_opt( f_cbCertificateTargetCRL ) const DRM_BYTE                        *f_pbCertificateTargetCRL,
    __in                                              DRM_DWORD                        f_cbCertificateTargetCRL )
{
    DRM_RESULT        dr          = DRM_E_LOGICERR;
    DRM_BCRL_Signed   bcrl        = { 0 };
    OEM_CRYPTO_HANDLE hSigningKey = OEM_CRYPTO_HANDLE_INVALID;

    ChkArg( f_pBbxContext            != NULL );
    ChkArg( f_pbCertificateTargetCRL != NULL );
    ChkArg( f_cbCertificateTargetCRL >  0 );

    ChkDR( DRM_BCrl_ParseSignedCrl( f_pbCertificateTargetCRL,
                                    f_cbCertificateTargetCRL,
                                    NULL,
                                    &bcrl ) );

    /*
    ** Make sure we have been passed the right CRL
    */
    ChkBOOL( 0 == MEMCMP( bcrl.Crl.Identifier, &g_guidRevocationTypePlayReadySilverLightRuntime, SIZEOF(DRM_GUID) ),
             DRM_E_INVALID_REVOCATION_LIST );

    /*
    ** See whether this CRL needs to be submitted
    */
    if( bcrl.Crl.dwVersion > f_pBbxContext->seenRevInfoVersions.dwCRLVersion )
    {
        /*
        ** Find the signing key for the RIV data
        */
        ChkDR( _ResolveSigningKey( f_pBbxContext,
                                   bcrl.pbCertificateChain,
                                   bcrl.cbCertificateChain,
                                   &hSigningKey ) );

        /*
        ** Send the RIV data to the HAL
        */
        ChkDR( _Hal_LoadPlayReadyCrl( f_pBbxContext,
                                      f_pbCertificateTargetCRL,
                                      f_cbCertificateTargetCRL,
                                      hSigningKey ) );

        f_pBbxContext->seenRevInfoVersions.dwCRLVersion = bcrl.Crl.dwVersion;
    }

ErrorExit:

    DRM_BBX_SafeReleaseKey( f_pBbxContext, &hSigningKey );

    return dr;
}


static DRM_NO_INLINE DRM_RESULT _ResolveSigningKey(
    __inout_ecount( 1 )                           DRM_BB_CONTEXT    *f_poBBXContext,
    __in_ecount_opt( f_cbCertificateChain ) const DRM_BYTE          *f_pbCertificateChain,
    __in                                          DRM_DWORD          f_cbCertificateChain,
    __out_ecount(1)                               OEM_CRYPTO_HANDLE *f_phSigningKey )
{
    DRM_RESULT              dr              = DRM_E_LOGICERR;
    DRM_BCERT_CHAIN_HEADER  oChainHeader    = { 0 };
    DRM_DWORD               bLeafCertStart  = 0;

    /*
    ** Parse a cert chain header and make sure
    ** that there are at least two certs in the chain.
    */
    ChkDR( DRM_BCert_GetChainHeader( f_pbCertificateChain,
                                     f_cbCertificateChain,
                                     &bLeafCertStart,
                                     &oChainHeader ) );

    ChkDR( _ResolveSigningChainKey( f_poBBXContext,
                                    f_pbCertificateChain,
                                    f_cbCertificateChain,
                                    oChainHeader.cCerts,
                                    0,
                                    DRM_BCERT_KEYUSAGE_SIGN_CRL,
                                    bLeafCertStart,
                                    f_phSigningKey ) );

ErrorExit:

    return dr;
}


/**********************************************************************************************************
** Function:    _ResolveSigningChainKey
**
** Synopsis:    Resolve a signing key handle from a certificate chain.
**
** Arguments:   [f_pBlackboxContext]       : the blackbox context
**              [f_pbCertData]             : data buffer that is being parsed
**              [f_cbCertData]             : size of the data buffer (in bytes)
**              [f_dwChainLength]          : the number of certificates in the chain
**              [f_idxStopResolutionAt]    : the index to stop resolving at. If 0, resolution continues all the
**                                           way to the leaf. If a positive integer then the signing key obtained
**                                           from the certificate at index f_idxStopResolutionAt is returned
**              [f_dwRequiredKeyUsageType] : If resolution is up to the leaf, the type of usage type of the key to obtain
**              [f_bOffset]                : the offset in bytes from f_pbCertData to the start of the leaf cert
**              [f_phSigningKey]           : pointer to handle location to fill with the validated signing key
**
** Returns:     DRM_SUCCESS          - on success
**              DRM_E_INVALIDARG     - if arguments are invalid
**              DRM_E_BUFFERTOOSMALL - buffer is shorter than expected
**              DRM_E_OEMHAL_CRYPTO_NOT_INITIALIZED
**              DRM_E_ARITHMETIC_OVERFLOW
**              DRM_E_BCERT_INVALID_CERT_LENGTH
**              DRM_E_BCERT_INVALID_CERT_TYPE
**
** Notes:
**
**********************************************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL _ResolveSigningChainKey(
    __in_ecount(1)                  DRM_BB_CONTEXT    *f_pBlackboxContext,
    __in_bcount(f_cbCertData) const DRM_BYTE          *f_pbCertData,
    __in                      const DRM_DWORD          f_cbCertData,
    __in                            DRM_DWORD          f_dwChainLength,
    __in                            DRM_DWORD          f_idxResolveUpTo,
    __in                            DRM_DWORD          f_dwRequiredKeyUsageType,
    __in                            DRM_DWORD          f_bOffset,
    __out_ecount(1)                 OEM_CRYPTO_HANDLE *f_phSigningKey )
{
    DRM_RESULT        dr                                             = DRM_E_LOGICERR;
    DRM_DWORD         bOffset                                        = f_bOffset;
    DRM_DWORD         rgdwCertOffsets[DRM_BCERT_MAX_CERTS_PER_CHAIN] = { 0 };
    DRM_DWORD         rgdwKeyIndices[DRM_BCERT_MAX_CERTS_PER_CHAIN]  = { 0 };
    DRM_BCERT_PUBKEY *f_rgpPublicKeys[DRM_BCERT_MAX_CERTS_PER_CHAIN] = { 0 };
    DRM_GUID          guidEmpty                                      = EMPTY_DRM_GUID;
    OEM_CRYPTO_HANDLE hSigningKey                                    = OEM_CRYPTO_HANDLE_INVALID;
    OEM_CRYPTO_HANDLE hChildKey                                      = OEM_CRYPTO_HANDLE_INVALID;
    DRM_DWORD         iCert                                          = 0;

    ChkArg( f_pBlackboxContext != NULL );
    ChkArg( f_pbCertData       != NULL );
    ChkArg( f_cbCertData       >  0 );
    ChkArg( f_idxResolveUpTo   <  f_dwChainLength );
    ChkArg( f_bOffset          <  f_cbCertData );
    ChkArg( f_phSigningKey     != NULL );
    ChkArg( *f_phSigningKey    == OEM_CRYPTO_HANDLE_INVALID );

    /*
    ** If we are resolving up to a key in the leaf certificate, that key type must be specified
    */
    ChkArg( ( f_idxResolveUpTo == 0 ) != ( f_dwRequiredKeyUsageType == DRM_BCERT_KEYUSAGE_UNKNOWN ) );

    /*
    ** Locate the starting point of each certificate, and the index of each parent key
    */
    ChkDR( DRM_BCert_GetCertChainInfo( f_pbCertData,
                                       f_cbCertData,
                                       bOffset,
                                       f_dwChainLength,
                                       f_idxResolveUpTo,
                                       f_dwRequiredKeyUsageType,
                                       rgdwCertOffsets,
                                       rgdwKeyIndices,
                                       f_rgpPublicKeys ) );

    /*
    ** Try to get the root public signing key
    */
    ChkDR( DRM_BBX_GetPreloadedKey( f_pBlackboxContext,
                                    OEM_HAL_KEY_TYPE_ECC_P256_PUBLIC,
                                    &guidEmpty,
                                    &hSigningKey ) );

    iCert = f_dwChainLength - 1;

    do
    {
        ChkDR( DRM_BBX_UnwrapKey( f_pBlackboxContext,
                                  OEM_HAL_KEY_TYPE_ECC_P256_PUBLIC,
                                  hSigningKey,
                                  f_pbCertData + rgdwCertOffsets[iCert],
                                  f_cbCertData - rgdwCertOffsets[iCert],
                                  ( const DRM_BYTE* )&rgdwKeyIndices[iCert],
                                  SIZEOF( rgdwKeyIndices[iCert] ),
                                  f_rgpPublicKeys[iCert]->m_rgbPubkey,
                                  SIZEOF(PUBKEY_P256),
                                  &hChildKey ) );

        if( iCert > f_idxResolveUpTo )
        {
            DRM_BBX_SafeReleaseKey( f_pBlackboxContext, &hSigningKey );

            hSigningKey = hChildKey;
            hChildKey   = OEM_CRYPTO_HANDLE_INVALID;
            iCert--;
        }
        else
        {
            *f_phSigningKey = hChildKey;
            hChildKey       = OEM_CRYPTO_HANDLE_INVALID;
        }
    }
    while( *f_phSigningKey == OEM_CRYPTO_HANDLE_INVALID );

ErrorExit:

    DRM_BBX_SafeReleaseKey( f_pBlackboxContext, &hSigningKey );
    DRM_BBX_SafeReleaseKey( f_pBlackboxContext, &hChildKey );

    return dr;
}


EXIT_PK_NAMESPACE_CODE;

