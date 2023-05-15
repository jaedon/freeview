/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_DRMBCRLPARSER_C
#include <byteorder.h>
#include <drmlicense.h>
#include <drmbcert.h>
#include <drmbcertparser.h>
#include <drmbcrl.h>
#include <drmblobcache.h>
#include <drmprofile.h>
#include <drmconstants.h>
#include <drmlastinclude.h>

ENTER_PK_NAMESPACE_CODE;

/**********************************************************************
**
** Function:    DRM_BCrl_VerifySignature
**
** Synopsis:    Verifies the PlayReady revinfov2 or binary crl signature.
**
** Arguments:
**              [f_pbSignedBytes]        - Specifies the bytes that are signed
**              [f_cbSignedBytes]        - Specifies the size of the signed bytes
**              [f_pbSignature]          - Specifies the signature
**              [f_cbSignature]          - Specifies the size signature
**              [f_pbCertificate]        - Specifies the certificate chain used to find signing key
**              [f_cbCertificate]        - Specifies the size certificate chain used to find signing key
**              [f_pRootPubkey]          - A pointer to public key to verify root certificate.
**              [f_pCryptoCtx]           - Pointer to DRM_CRYPTO_CONTEXT used for temporary data in crypto code.
**
** Returns:     DRM_SUCCESS if the signature is valid
**              DRM_E_INVALIDARG if the data in the passed arguments is invalid
**              passes return codes from other failures up to caller
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_BCrl_VerifySignature(
    __in_bcount(f_cbSignedBytes)      const DRM_BYTE               *f_pbSignedBytes,
    __in                                    DRM_DWORD               f_cbSignedBytes,
    __in_bcount(f_cbSignature)              DRM_BYTE               *f_pbSignature,
    __in                                    DRM_DWORD               f_cbSignature,
    __in_bcount(f_cbCertificateChain) const DRM_BYTE               *f_pbCertificateChain,
    __in                                    DRM_DWORD               f_cbCertificateChain,
    __in_ecount(1)                    const PUBKEY_P256            *f_pRootPubkey,
    __in                                    DRM_CRYPTO_CONTEXT     *f_pCryptoCtx,
    __in_opt                                DRM_BB_CONTEXT         *f_pcontextBBX,
    __in_opt                                DRM_SECSTORE_CONTEXT   *f_pcontextSST,
    __in_opt                                DRM_DST                *f_pDatastore ) DRM_NO_INLINE_ATTRIBUTE;
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_BCrl_VerifySignature(
    __in_bcount(f_cbSignedBytes)      const DRM_BYTE               *f_pbSignedBytes,
    __in                                    DRM_DWORD               f_cbSignedBytes,
    __in_bcount(f_cbSignature)              DRM_BYTE               *f_pbSignature,
    __in                                    DRM_DWORD               f_cbSignature,
    __in_bcount(f_cbCertificateChain) const DRM_BYTE               *f_pbCertificateChain,
    __in                                    DRM_DWORD               f_cbCertificateChain,
    __in_ecount(1)                    const PUBKEY_P256            *f_pRootPubkey,
    __in                                    DRM_CRYPTO_CONTEXT     *f_pCryptoCtx,
    __in_opt                                DRM_BB_CONTEXT         *f_pcontextBBX,
    __in_opt                                DRM_SECSTORE_CONTEXT   *f_pcontextSST,
    __in_opt                                DRM_DST                *f_pDatastore )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT dr = DRM_SUCCESS;
    PUBKEY_P256 pub_key = { 0 };
    DRM_BCERT_CHAIN_HEADER chain_header = { 0 };
    DRM_DWORD offset_for_header = 0;
    DRM_DWORD cert_index = 0; /* Cert[0] is the leaf certificate. */
    DRM_BCERT_VERIFICATIONCONTEXT oVerificationContext = {0};
    DRM_DWORD rgdwKeyUsageSet[] = { DRM_BCERT_KEYUSAGE_SIGN_CRL }; /* verify that this certificate has the Sign Crl key usage value */
    DRMFILETIME ftTime = {0};
    DRM_BOOL fVerified = FALSE;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_REVOCATION, PERF_FUNC_DRM_BCrl_VerifySignature );

    ChkDR( DRM_BLOBCACHE_Verify( f_pcontextBBX,
                                 f_pcontextSST,
                                 f_pDatastore,
                                 f_pbSignedBytes,
                                 f_cbSignedBytes,
                                 f_pbSignature,
                                 f_cbSignature,
                                 &fVerified ) );

    if ( fVerified )
    {
        goto ErrorExit;
    }

    /* verifying length of chain */
    ChkDR( DRM_BCert_GetChainHeader( f_pbCertificateChain, f_cbCertificateChain, &offset_for_header, &chain_header ) );
    ChkArg( f_cbCertificateChain == chain_header.cbChain );

    /* verifying certificate chain if it's trusted */
    Oem_Clock_GetSystemTimeAsFileTime( NULL, &ftTime );

    ChkDR( DRM_BCert_InitVerificationContext( &ftTime,
                                              f_pRootPubkey,
                                              DRM_BCERT_CERTTYPE_CRL_SIGNER,
                                              f_pCryptoCtx,
                                              TRUE,
                                              FALSE,
                                              rgdwKeyUsageSet,
                                              NO_OF(rgdwKeyUsageSet),
                                              FALSE,
                                              f_pcontextBBX,
                                              f_pcontextSST,
                                              f_pDatastore,
                                              &oVerificationContext ) );

    ChkDR( DRM_BCert_ParseCertificateChain( f_pbCertificateChain, f_cbCertificateChain, &oVerificationContext ) );

    /* retrieving key from certificate chain */
    ChkDR( DRM_BCert_GetPublicKey( f_pbCertificateChain, f_cbCertificateChain, cert_index, &pub_key ) );

    /* verifying signature */
    ChkArg(sizeof(SIGNATURE_P256) == f_cbSignature);
    ChkDR( OEM_ECDSA_Verify_P256( f_pbSignedBytes,
                                  f_cbSignedBytes,
                                  &pub_key,
                                  (SIGNATURE_P256*)f_pbSignature,
                                  (struct bigctx_t *)f_pCryptoCtx ) );

    if( f_pcontextBBX != NULL
     && f_pcontextSST != NULL
     && f_pDatastore  != NULL )
    {
        ChkDR( DRM_BLOBCACHE_Update( f_pcontextBBX,
                                     f_pcontextSST,
                                     f_pDatastore,
                                     f_pbSignedBytes,
                                     f_cbSignedBytes,
                                     f_pbSignature,
                                     f_cbSignature ) );
    }

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}

/*****************************************************************************
** Function:    DRM_BCrl_ParseCrl
**
** Synopsis:    Parses Playready CRL from binary to data structure.
**              Verifies the CRL signature and associated certificate chain.
**
** Arguments:   [f_pbCrlData]   : A pointer to the raw binary CRL data
**              [f_cbCrlData]   : Number of bytes in the raw binary CRL data
**              [f_poCrl]       : A pointer to the structure to hold the parsed CRL data
**              [f_pRootPubkey] : A pointer to public key to verify root certificate.
**              [f_pCryptoCtx]  : Pointer to DRM_CRYPTO_CONTEXT used for temporary data in crypto code.
**              [f_pcontextBBX] : Pointer to the blackbox context.
**              [f_pcontextSST] : Pointer to the secure store context.
**              [f_pDatastore]  : Pointer to a Data Store structure.
**
** Returns:     DRM_SUCCESS      - on success
**              DRM_E_INVALIDARG - if the output parm or the CRL data is NULL
**
** Notes:       The parser does not make copies of the DRM_RevocationEntry
**              data, it just points to them in the f_pbCrlData buffer.  Thus
**              the caller cannot free the f_pbCrlData buffer and still have a
**              valid f_poCrl data structure.
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_BCrl_ParseCrl(
    __in_bcount(f_cbCrlData) const DRM_BYTE       *f_pbCrlData,
    __in                     const DRM_DWORD      f_cbCrlData,
    __out                    DRM_BCRL_Signed      *f_poCrl,
    __in_ecount(1)           const PUBKEY_P256    *f_pRootPubkey,
    __inout                  DRM_CRYPTO_CONTEXT   *f_pCryptoCtx,
    __in_opt                 DRM_BB_CONTEXT       *f_pcontextBBX,
    __in_opt                 DRM_SECSTORE_CONTEXT *f_pcontextSST,
    __in_opt                 DRM_DST              *f_pDatastore ) DRM_NO_INLINE_ATTRIBUTE;
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_BCrl_ParseCrl(
    __in_bcount(f_cbCrlData) const DRM_BYTE       *f_pbCrlData,
    __in                     const DRM_DWORD      f_cbCrlData,
    __out                    DRM_BCRL_Signed      *f_poCrl,
    __in_ecount(1)           const PUBKEY_P256    *f_pRootPubkey,
    __inout                  DRM_CRYPTO_CONTEXT   *f_pCryptoCtx,
    __in_opt                 DRM_BB_CONTEXT       *f_pcontextBBX,
    __in_opt                 DRM_SECSTORE_CONTEXT *f_pcontextSST,
    __in_opt                 DRM_DST              *f_pDatastore )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD dwSignedMessageLength = 0;

    ChkArg( f_poCrl != NULL );
    ChkArg( f_pbCrlData != NULL);

    ChkDR( DRM_BCrl_ParseSignedCrl( f_pbCrlData,
                                      f_cbCrlData,
                                      &dwSignedMessageLength,
                                      f_poCrl ) );

    /* verifying signature */
    ChkDR( DRM_BCrl_VerifySignature( f_pbCrlData,
                                     dwSignedMessageLength,
                                     &(f_poCrl->Signature.rgb[0]),
                                     f_poCrl->Signature.cb,
                                     f_poCrl->pbCertificateChain,
                                     f_poCrl->cbCertificateChain,
                                     f_pRootPubkey,
                                     f_pCryptoCtx,
                                     f_pcontextBBX,
                                     f_pcontextSST,
                                     f_pDatastore ) );

ErrorExit:
    return dr;
}


/*****************************************************************************
** Function:    DRM_BCrl_CheckIfPlayReadyCertRevoked
**
** Synopsis:    Checks if PlayReady certificate is revoked by the CRL.
**              Iterates over hashes in the device CRL, check if it matches hashes of public keys in
**              certificate chain
**
** Arguments:   [f_pbPlayReadyCert]     : A pointer to the raw PlayReady certificate data.
**              [f_cbPlayReadyCert]     : A Number of bytes in the raw PlayReady certificate data.
**              [f_pbCrlData]           : A pointer to the raw binary CRL data
**              [f_cbCrlData]           : Number of bytes in the raw binary CRL data
**              [f_pRevokedCertDigest]  : Optional pointer to receive the digest of the specific cert within the chain that was revoked.
**              [f_pdwPlayReadyCRLVer]  : Optional pointer to DRM_DWORD. Filled with version of PlayReady CRL.
**              [f_pCryptoCtx]          : Pointer to DRM_CRYPTO_CONTEXT used for temporary data in crypto code.
**
** Returns:     DRM_SUCCESS                 - on success
**              DRM_E_INVALIDARG            - if the output parm or the CRL data is NULL
**              DRM_E_CERTIFICATE_REVOKED   - if the certificate is revoked by the CRL
**
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_BCrl_CheckIfPlayReadyCertRevoked(
    __in_bcount(f_cbPlayReadyCert)const DRM_BYTE              *f_pbPlayReadyCert,
    __in                                DRM_DWORD             f_cbPlayReadyCert,
    __in_bcount(f_cbPlayReadyCRL) const DRM_BYTE             *f_pbPlayReadyCRL,
    __in                                DRM_DWORD             f_cbPlayReadyCRL,
    __out_opt                           DRM_SHA256_Digest    *f_pRevokedCertDigest,
    __out_opt                           DRM_DWORD            *f_pdwPlayReadyCRLVer,
    __inout                             DRM_CRYPTO_CONTEXT   *f_pCryptoCtx,
    __in_opt                            DRM_BB_CONTEXT       *f_pcontextBBX,
    __in_opt                            DRM_SECSTORE_CONTEXT *f_pcontextSST,
    __in_opt                            DRM_DST              *f_pDatastore ) DRM_NO_INLINE_ATTRIBUTE;
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_BCrl_CheckIfPlayReadyCertRevoked(
    __in_bcount(f_cbPlayReadyCert)const DRM_BYTE              *f_pbPlayReadyCert,
    __in                                DRM_DWORD             f_cbPlayReadyCert,
    __in_bcount(f_cbPlayReadyCRL) const DRM_BYTE             *f_pbPlayReadyCRL,
    __in                                DRM_DWORD             f_cbPlayReadyCRL,
    __out_opt                           DRM_SHA256_Digest    *f_pRevokedCertDigest,
    __out_opt                           DRM_DWORD            *f_pdwPlayReadyCRLVer,
    __inout                             DRM_CRYPTO_CONTEXT   *f_pCryptoCtx,
    __in_opt                            DRM_BB_CONTEXT       *f_pcontextBBX,
    __in_opt                            DRM_SECSTORE_CONTEXT *f_pcontextSST,
    __in_opt                            DRM_DST              *f_pDatastore )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT dr                       = DRM_SUCCESS;
    DRM_BCRL_Signed obCRLDevice         = {0};
    DRM_BCERT_CHAIN_HEADER oChainHeader = {0};
    DRM_BCERT_CERTIFICATE  oDeviceCert  = {0};
    DRM_DWORD              dwCertIndex  = 0;
    DRM_DWORD              bCurrOffset  = 0;
    DRM_DWORD              dwHashIndex  = 0;


    ChkArg( f_pbPlayReadyCert != NULL );
    ChkArg( f_cbPlayReadyCert > 0 );
    ChkArg( f_pbPlayReadyCRL != NULL );
    ChkArg( f_cbPlayReadyCRL > 0 );
    ChkArg( f_pCryptoCtx != NULL );

    if ( NULL != f_pRevokedCertDigest )
    {
        MEMSET( f_pRevokedCertDigest, 0, SIZEOF( *f_pRevokedCertDigest ) );
    }

    /*
    **  Parse PlayReady CRL. This operation brings list of hashes of public keys.
    **  Uses production public key to test signature on CRL files.
    */
    ChkDR( DRM_BCrl_ParseCrl( f_pbPlayReadyCRL, f_cbPlayReadyCRL, &obCRLDevice, (const PUBKEY_P256*)g_rgbMSPlayReadyRootIssuerPubKey, f_pCryptoCtx, f_pcontextBBX, f_pcontextSST, f_pDatastore ) );

    /* Optionally ouputs the version of the CRL. On PC they check that CRL has minimal version */
    if ( f_pdwPlayReadyCRLVer != NULL )
    {
        *f_pdwPlayReadyCRLVer = obCRLDevice.Crl.dwVersion;
    }

    /*
    ** Loop over all certificates in the chain of f_pbPlayReadyCert and compare hash of the public key in each
    ** certificate with hashes in the binary CRL.
    */

    /*
    ** Skip the chain header to get to device certificate
    */
    ChkDR( DRM_BCert_GetChainHeader( f_pbPlayReadyCert,
                                     f_cbPlayReadyCert,
                                     &bCurrOffset,
                                     &oChainHeader ) );

    /* Itearation over all certificates in the chain */
    for ( dwCertIndex = 0; dwCertIndex < oChainHeader.cCerts ; dwCertIndex++ )
    {
        /*
        ** Load device certificate. bCurrOffset is updated on each call and thus we iterate over certificates.
        */
        ChkDRMap( DRM_BCert_GetCertificate( f_pbPlayReadyCert,
                                            f_cbPlayReadyCert,
                                            &bCurrOffset,
                                            &oDeviceCert,
                                            DRM_BCERT_CERTTYPE_UNKNOWN ), DRM_S_FALSE, DRM_SUCCESS );

        /* Iteration over all hashes in binary CRL. */
        for ( dwHashIndex = 0; dwHashIndex < obCRLDevice.Crl.cRevocationEntries; dwHashIndex++ )
        {
            /* Compare hashes in the device cert. and in the CRL list. */

            if ( 0 == MEMCMP( obCRLDevice.Crl.Entries + dwHashIndex, &oDeviceCert.BasicInfo.DigestValue, SIZEOF( obCRLDevice.Crl.Entries ) ) )
            {
                /*
                ** OOPs, we found that one of hashes in CRL correspond to hash in device cert.
                ** So it is revoked!
                */
                if ( NULL != f_pRevokedCertDigest )
                {
                    MEMCPY( f_pRevokedCertDigest, &oDeviceCert.BasicInfo.DigestValue, SIZEOF( *f_pRevokedCertDigest ) );
                }
                ChkDR( DRM_E_CERTIFICATE_REVOKED );
            }
        }
    }

ErrorExit :

    /*
    **  DRM_BCert_GetCertificate return DRM_S_FALSE on last certificate.
    **  Thus if we do not change return code, it would be DRM_S_FALSE in case of not revoked certificate.
    **  While it is acceptable, it is strange return code for DRM_BCrl_CheckIfPlayReadyCertRevoked, so I change it to DRM_SUCCESS
    */
    if ( DRM_S_FALSE == dr  )
    {
        dr = DRM_SUCCESS;
    }
    return dr;
}


EXIT_PK_NAMESPACE_CODE;

