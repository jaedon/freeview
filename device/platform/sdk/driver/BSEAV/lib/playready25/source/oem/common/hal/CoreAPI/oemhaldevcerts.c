/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

/******************************************************************************
**
** This file defines a minimal set of revocation functions for inside the secure crypto core.
** OEMs should not need to change this file in any way (except to get it to compile properly).
** If the device supports a secure crypto core, this file should compile ONLY for that core.
**
*******************************************************************************
*/

#define DRM_BUILDING_OEMHALDEVCERTS_C

#include <oemhaldevcerts.h>
#include <oemhaloemimpl.h>
#include <oemhal.h>
#include <oemparsers.h>

#include <drmconstants.h>

#include <drmlastinclude.h>

ENTER_PK_NAMESPACE_CODE;

/*****************************************************************************
** Function:    _ProcessRevocationInfo
**
** Synopsis:    Process a revocation info buffer.
**
** Arguments:   [f_pCryptoContext]       : A crypto context
**              [f_pbRevocationInfo]     : The revocation info data
**              [f_cbRevocationInfo]     : The length of the revocation info data
**              [f_indexSigningKey]      : The data signing key
**
** Returns:     DRM_SUCCESS                   - on success
**              DRM_E_INVALIDARG              - if invalid arguments.
**              DRM_E_INVALID_REVOCATION_LIST - the revocation info data is invalid.
**
** Notes:
**
******************************************************************************/
DRM_RESULT _ProcessRevocationInfo(
    __inout_ecount( 1 )                     DRM_CRYPTO_CONTEXT         *f_pCryptoContext,
    __in_bcount( f_cbRevocationInfo ) const DRM_BYTE                   *f_pbRevocationInfo,
    __in                                    DRM_DWORD                   f_cbRevocationInfo,
    __in                                    OEM_HAL_KEY_REGISTER_INDEX  f_indexSigningKey )
{
    DRM_RESULT dr            = DRM_SUCCESS;
    DRM_RLVI   rlvi          = { 0 };
    DRM_DWORD  cbSignedBytes = 0;
    DRM_DWORD  dwStoredRIV   = 0;

    ChkPtr( f_pCryptoContext );
    ChkPtr( f_pbRevocationInfo );
    ChkArg( f_cbRevocationInfo >  0 );

    ChkDR( DRM_RVK_ParseRevocationInfo( f_pbRevocationInfo,
                                        f_cbRevocationInfo,
                                        &rlvi,
                                        &cbSignedBytes ) );

    /*
    ** Check that this RevInfo structure is as expected
    */
    ChkBOOL( rlvi.head.bFormatVersion      == RLVI_FORMAT_VERSION_V2, DRM_E_INVALID_REVOCATION_LIST );
    ChkBOOL( rlvi.signature.bSignatureType == RLVI_SIGNATURE_TYPE_2,  DRM_E_INVALID_REVOCATION_LIST );

    ChkDR( Oem_HalOemImpl_ReadStoredRIV( &dwStoredRIV ) );
    if( rlvi.head.dwRIV > dwStoredRIV )
    {
        DRM_DWORD dwStoredRequiredCRLVersionFromRevInfo = 0;
        DRM_DWORD dwStoredCRLVersion                    = 0;

        /*
        ** Validate the signature
        */
        ChkBOOL( SIZEOF(SIGNATURE_P256) == rlvi.signature.cbSignature, DRM_E_INVALID_REVOCATION_LIST );

        ChkDR( Oem_HalOemImpl_ECDSA_Verify_P256( f_pCryptoContext,
                                                 f_indexSigningKey,
                                                 f_pbRevocationInfo,
                                                 cbSignedBytes,
                                                 ( SIGNATURE_P256* )&f_pbRevocationInfo[rlvi.signature.ibSignature] ) );

        ChkDR( Oem_HalOemImpl_WriteStoredRIV( 0 ) );
        ChkDR( Oem_HalOemImpl_WriteStoredRequiredCRLVersionFromRevInfo( 0 ) );

        /*
        ** Now extract the CRL version
        */
        ChkDR( DRM_RVK_FindEntryInRevInfo( &rlvi,
                                           f_pbRevocationInfo,
                                           f_cbRevocationInfo,
                                           &g_guidRevocationTypePlayReadySilverLightRuntime,
                                           &dwStoredRequiredCRLVersionFromRevInfo ) );

        ChkDR( Oem_HalOemImpl_WriteStoredRequiredCRLVersionFromRevInfo( dwStoredRequiredCRLVersionFromRevInfo ) );
        ChkDR( Oem_HalOemImpl_ReadStoredCRLVersion( &dwStoredCRLVersion ) );

        /*
        ** If the CRL version is greater than the current CRL version, then the old CRL is invalid
        */
        if( dwStoredRequiredCRLVersionFromRevInfo > dwStoredCRLVersion )
        {
            ChkDR( Oem_HalOemImpl_WriteStoredCRLVersion( 0 ) );
            ChkDR( Oem_HalOemImpl_WriteStoredCRLEntries( 0, NULL ) );
        }

        ChkDR( Oem_HalOemImpl_WriteStoredRIV( rlvi.head.dwRIV ) );
    }

ErrorExit:
    return dr;
}

/*****************************************************************************
** Function:    _ProcessCRL
**
** Synopsis:    Process a CRL buffer.
**
** Arguments:   [f_pCryptoContext]  : A crypto context
**              [f_pbCRL]           : The CRL data
**              [f_cbCRL]           : The length of the CRL data
**              [f_indexSigningKey] : The data signing key
**
** Returns:     DRM_SUCCESS                   - on success
**              DRM_E_INVALIDARG              - if invalid arguments.
**              DRM_E_INVALID_REVOCATION_LIST - the revocation info data is invalid.
**
** Notes:
**
******************************************************************************/
DRM_RESULT _ProcessCRL(
    __inout_ecount( 1 )                     DRM_CRYPTO_CONTEXT         *f_pCryptoContext,
    __in_bcount( f_cbCRL )            const DRM_BYTE                   *f_pbCRL,
    __in                                    DRM_DWORD                   f_cbCRL,
    __in                                    OEM_HAL_KEY_REGISTER_INDEX  f_indexSigningKey )
{
    DRM_RESULT      dr                    = DRM_SUCCESS;
    DRM_BCRL_Signed bcrl                  = { 0 };
    DRM_DWORD       dwSignedMessageLength = 0;
    DRM_DWORD       dwStoredCRLVersion    = 0;
    DRM_DWORD       dwStoredRequiredCRLVersionFromRevInfo   = 0;

    ChkPtr( f_pCryptoContext );
    ChkPtr( f_pbCRL );
    ChkArg( f_cbCRL > 0 );

    ChkDR( DRM_BCrl_ParseSignedCrl( f_pbCRL,
                                    f_cbCRL,
                                    &dwSignedMessageLength,
                                    &bcrl ) );

    /*
    ** Make sure we have been passed the right CRL
    */
    ChkBOOL( OEM_SECURE_ARE_EQUAL( bcrl.Crl.Identifier, &g_guidRevocationTypePlayReadySilverLightRuntime, SIZEOF(DRM_GUID) ),
             DRM_E_INVALID_REVOCATION_LIST );

    /*
    ** Now see if this list supersedes the previous list and is current. Allow the CRL version to
    ** be greater than that required by the RevInfo in case the CRL is updated first
    */
    ChkDR( Oem_HalOemImpl_ReadStoredCRLVersion( &dwStoredCRLVersion ) );
    ChkDR( Oem_HalOemImpl_ReadStoredRequiredCRLVersionFromRevInfo( &dwStoredRequiredCRLVersionFromRevInfo ) );
    if( bcrl.Crl.dwVersion >  dwStoredCRLVersion
     && bcrl.Crl.dwVersion >= dwStoredRequiredCRLVersionFromRevInfo )
    {
        /*
        ** Validate the signature
        */
        ChkBOOL( SIZEOF(SIGNATURE_P256) == bcrl.Signature.cb, DRM_E_INVALID_REVOCATION_LIST );

        ChkDR( Oem_HalOemImpl_ECDSA_Verify_P256( f_pCryptoContext,
                                                 f_indexSigningKey,
                                                 f_pbCRL,
                                                 dwSignedMessageLength,
                                                 ( SIGNATURE_P256* )bcrl.Signature.rgb ) );

        /*
        ** Extract the CRL data
        */
        ChkDR( Oem_HalOemImpl_WriteStoredCRLVersion( 0 ) );    /* Ensure that the version isn't set if storing the actual entries fails. */
        ChkDR( Oem_HalOemImpl_WriteStoredCRLEntries( bcrl.Crl.cRevocationEntries, bcrl.Crl.Entries ) );
        ChkDR( Oem_HalOemImpl_WriteStoredCRLVersion( bcrl.Crl.dwVersion ) );
    }

ErrorExit:
    return dr;
}

/*****************************************************************************
** Function:    _CheckCertificateRevocation
**
** Synopsis:    Check a key certificate hash against the current revocation list
**              if one is loaded.
**
** Arguments:   [f_pCertDigests]        : The hashes of the certificates to check
**              [f_cDigests]            : The number of hashes
**              [f_peVerificationState] : returns the verification state, based
**                                        on currently available CRL info
**
** Returns:     DRM_SUCCESS               - on success
**              DRM_E_INVALIDARG          - if invalid arguments.
**
** Notes:
**
******************************************************************************/
DRM_NO_INLINE DRM_RESULT _CheckCertificateRevocation(
    __in_ecount(f_cDigests)           const DRM_SHA256_Digest                   *f_pCertDigests,
    __in                                    DRM_DWORD                            f_cDigests,
    __out_ecount( 1 )                       OEM_HAL_PUBKEY_VERIFICATION_STATE   *f_peVerificationState )
{
    DRM_RESULT dr                    = DRM_SUCCESS;
    DRM_DWORD  dwStoredCRLVersion    = 0;
    DRM_DWORD  dwStoredCRLEntryCount = 0;

    ChkPtr( f_pCertDigests );
    ChkArg( f_cDigests > 0 );
    ChkPtr( f_peVerificationState );

    *f_peVerificationState = eOEM_HAL_PUBKEY_NOT_VERIFIED;

    /*
    ** If there is no CRL, then the key is not verified
    */
    ChkDR( Oem_HalOemImpl_ReadStoredCRLVersion( &dwStoredCRLVersion ) );
    if( dwStoredCRLVersion > 0 )
    {
        DRM_DWORD iEntries  = 0;
        DRM_DWORD iHash     = 0;

        ChkDR( Oem_HalOemImpl_ReadStoredCRLEntryCount( &dwStoredCRLEntryCount ) );

        /*
        ** Check to see whether the key is on the revoked list
        */
        for( iHash = 0; iHash < f_cDigests; iHash++ )
        {
            for( iEntries = 0; iEntries < dwStoredCRLEntryCount; iEntries++ )
            {
                DRM_RevocationEntry oEntry = {0};
                ChkDR( Oem_HalOemImpl_ReadStoredCRLEntry( iEntries, &oEntry ) );
                if( OEM_SECURE_ARE_EQUAL( &oEntry, &f_pCertDigests[iHash], SIZEOF( DRM_SHA256_Digest ) ) )
                {
                    *f_peVerificationState = eOEM_HAL_PUBKEY_REVOKED;
                    /* As soon as we're revoked, we're done. */
                    goto ErrorExit;
                }
            }
        }

        /*
        ** No matches found, the key is valid
        */
        *f_peVerificationState = eOEM_HAL_PUBKEY_VERIFIED;
    }

ErrorExit:
    return dr;
}

/*****************************************************************************
** Function:    _CheckValidPublicKey
**
** Synopsis:    Check whether a key has been validated against a CRL.
**
** Arguments:   [f_indexPublicKey]  : The key
**
** Returns:     DRM_SUCCESS               - the key is valid against the current CRL
**              DRM_E_INVALIDARG          - the key has not been validated.
**              DRM_E_CERTIFICATE_REVOKED - the key certificate has been revoken.
**
** Notes:
**
******************************************************************************/
DRM_RESULT _CheckValidPublicKey(
    __in                                    OEM_HAL_KEY_REGISTER_INDEX  f_indexPublicKey )
{
    DRM_RESULT                          dr      = DRM_SUCCESS;
    OEM_HAL_PUBKEY_VERIFICATION_STATE   state   = eOEM_HAL_PUBKEY_NOT_VERIFIED;

    ChkDR( Oem_HalOemImpl_GetEcc256PublicKeyVerificationState( f_indexPublicKey, &state ) );

    switch( state )
    {
    case eOEM_HAL_PUBKEY_NOT_VERIFIED:
        dr = DRM_E_INVALIDARG;
        break;

    case eOEM_HAL_PUBKEY_VERIFIED:
        dr = DRM_SUCCESS;
        break;

    case eOEM_HAL_PUBKEY_REVOKED:
        dr = DRM_E_CERTIFICATE_REVOKED;
        break;

    default:
        dr = DRM_E_LOGICERR;
        break;
    }

ErrorExit:
    return dr;
}

EXIT_PK_NAMESPACE_CODE;

