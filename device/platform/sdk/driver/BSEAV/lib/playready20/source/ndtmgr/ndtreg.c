/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmcommon.h>
#include <oemndt.h>
#include <drmcrt.h>
#include <drmcert.h>
#include <drmutilities.h>
#include <oemrsaoaeppss.h>
#include <oemaes.h>
#include <drmcertparser.h>
#include <drmbase64.h>
#include <ndtconstants.h>
#include <ndtnetmsg.h>
#include <ndtreg.h>
#include <ndtmanager_internal.h>

ENTER_PK_NAMESPACE_CODE;

/**********************************************************************
**
** Function:    _GetCertificateAttributes
**
** Synopsis:    Returns key values from the certificate to be cached during the lifetime
**              of this registration session.
**
** Arguments:   [f_rgbDeviceCertificiate]  - Specifies the device certificate
**              [f_pdasstrLeafCertificate] - Specifies the offset and size of the device
**                                           certification, in bytes.
**              [f_ppDevicePublicKey]      - Returns the Device RSA Public Key
**              [f_pcbPubModulus]          - Returns the size of the public key modulus
**              [f_pdwSecurityLevel]       - Returns the Security Level for the device
**              [f_pdwFeatures]            - Returns the Features bit field
**              [f_rgbDigestValue]         - Returns the Digest Value
**
** Returns:     DRM_SUCCESS on success or
**              DRM_E_INVALIDARG if any of the arguments are invalid
**
**********************************************************************/
static DRM_RESULT _GetCertificateAttributes(
    __in_ecount(f_pdasstrLeafCertificate->m_ich + f_pdasstrLeafCertificate->m_cch) const DRM_CHAR * const f_rgchDeviceCertificate,
    __in const DRM_SUBSTRING       * const f_pdasstrLeafCertificate,
    __out      DRM_RSA_PUBLIC_KEY        **f_ppDevicePublicKey,
    __out      DRM_DWORD           * const f_pcbPubModulus,
    __out      DRM_DWORD           * const f_pdwSecurityLevel,
    __out      DRM_DWORD           * const f_pdwFeatures,
    __out_bcount(DRM_SHA1_DIGEST_LEN) DRM_BYTE f_rgbDigestValue[ __CB_DECL( DRM_SHA1_DIGEST_LEN ) ]
)
{
    DRM_RESULT    dr                  = DRM_SUCCESS;
    DRM_SUBSTRING rgdasstrAttrs [3]   = { 0 };
    DRM_DWORD     cAttrEntries        = NO_OF(rgdasstrAttrs);
    DRM_DWORD     iFeature;
    DRM_DWORD     cbDestination;
    DRM_BYTE      rgbExponent   [__CB_DECL(DRM_RSA_CB_PUBEXP)] = {0};
    DRM_BYTE      rgbModulus    [__CB_DECL(DRM_RSA_CB_MODULUS_MAX)] = { 0 };
    DRM_RSA_SUPPORTED_KEY_LENGTHS eKeyLength = eDRM_RSA_UNKNOWN;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT, PERF_FUNC__GetCertificateAttributes );

    ChkArg( f_rgchDeviceCertificate && f_pdasstrLeafCertificate && f_pdwSecurityLevel
         && f_pdwFeatures  && f_ppDevicePublicKey && f_pcbPubModulus );

    /*
     * Get Security Level
     */
    ChkDR( DRM_WCP_GetAttribute( f_rgchDeviceCertificate,
                                 f_pdasstrLeafCertificate,
                                 DRM_WMDRM_CERT_ATTR_SECURITY_LEVEL,
                                 rgdasstrAttrs,
                                &cAttrEntries ) );

    ChkDR( DRMCRT_AtoDWORD( f_rgchDeviceCertificate + rgdasstrAttrs[0].m_ich, rgdasstrAttrs[0].m_ich + rgdasstrAttrs[0].m_cch, 10, f_pdwSecurityLevel ) );

    /*
     * Get Features
     */
    cAttrEntries = NO_OF(rgdasstrAttrs);

    ChkDR( DRM_WCP_GetAttribute( f_rgchDeviceCertificate,
                                 f_pdasstrLeafCertificate,
                                 DRM_WMDRM_CERT_ATTR_FEATURES,
                                 rgdasstrAttrs,
                                &cAttrEntries ) );

    *f_pdwFeatures = 0;
    for(iFeature = 0; iFeature < cAttrEntries; iFeature++ )
    {
        if( DRM_SUCCEEDED( DRM_UTL_DASSTRStringsEqual(
            f_rgchDeviceCertificate, rgdasstrAttrs + iFeature, &g_dastrTagWMDRMReceiver) ) )
        {
            *f_pdwFeatures |= NDT_FEATURE_RECEIVER;
        }
        else if( DRM_SUCCEEDED( DRM_UTL_DASSTRStringsEqual(
            f_rgchDeviceCertificate, rgdasstrAttrs + iFeature, &g_dastrTagWMDRMTransmitter) ) )
        {
            *f_pdwFeatures |= NDT_FEATURE_TRANSMITTER;
        }
        else if( DRM_SUCCEEDED( DRM_UTL_DASSTRStringsEqual(
            f_rgchDeviceCertificate,
            rgdasstrAttrs + iFeature,
           &g_dastrTagWMDRMSharedCertificate) ) )
        {
            *f_pdwFeatures |= NDT_FEATURE_SHARED_CERT;
        }
        else ChkDR( DRM_E_INVALIDDEVICECERTIFICATE );
    }

    /*
     *  Get Digest Value
     */
    cAttrEntries = NO_OF(rgdasstrAttrs);

    ChkDR( DRM_WCP_GetAttribute( f_rgchDeviceCertificate,
                                 f_pdasstrLeafCertificate,
                                 DRM_WMDRM_CERT_ATTR_DIGEST_VALUE,
                                 rgdasstrAttrs,
                                &cAttrEntries ) );

    cbDestination = DRM_SHA1_DIGEST_LEN;
    ChkDR( DRM_B64_DecodeA( f_rgchDeviceCertificate,
                            rgdasstrAttrs,
                           &cbDestination,
                           f_rgbDigestValue,
                            0 ) );

    /*
     * Get Public Key Exponent and Modulus from certificate
     */
    cAttrEntries = NO_OF(rgdasstrAttrs);

    ChkDR( DRM_WCP_GetAttribute( f_rgchDeviceCertificate,
                                 f_pdasstrLeafCertificate,
                                 DRM_WMDRM_CERT_ATTR_PUBLIC_KEY_SELF_RSA,
                                 rgdasstrAttrs,
                                &cAttrEntries ) );

    /*
     *  Decode Exponent
     */
    cbDestination = DRM_RSA_CB_PUBEXP;
    ChkDR( DRM_B64_DecodeA( f_rgchDeviceCertificate,
                            rgdasstrAttrs,
                           &cbDestination,
                            rgbExponent,
                            0 ) );
    /*
    ** Put the Exponent in Big-Endian format (the way the RSA code needs it)
    ** I.e., 0x01 0x00 0x01 0x00 --> 0x00 0x01 0x00 0x01 (65537)
    */
    PUT_BYTE( rgbExponent, 3, GET_BYTE( rgbExponent, 2 ) );
    PUT_BYTE( rgbExponent, 2, GET_BYTE( rgbExponent, 1 ) );
    PUT_BYTE( rgbExponent, 1, GET_BYTE( rgbExponent, 0 ) );
    PUT_BYTE( rgbExponent, 0, 0 );

    /*
     *  Decode Modulus
     */
    cbDestination = SIZEOF(rgbModulus);
    ChkDR( DRM_B64_DecodeA( f_rgchDeviceCertificate,
                            rgdasstrAttrs + 1,
                           &cbDestination,
                            rgbModulus,
                            0 ) );

    ChkDR( OEM_RSA_GetKeyLengthFromModulusLength( cbDestination, &eKeyLength ) );

    ChkMem( *f_ppDevicePublicKey = (DRM_RSA_PUBLIC_KEY*)Oem_MemAlloc( SIZEOF( DRM_RSA_PUBLIC_KEY ) ) );
    ChkDR( OEM_RSA_SetPublicKey( eKeyLength,
                                 DRM_RSA_CB_PUBEXP,
                                 rgbExponent,
                                 cbDestination,
                                 rgbModulus,
                                 *f_ppDevicePublicKey ) );

    /*
     * Return Public Key Modulus Size
     */
    *f_pcbPubModulus = cbDestination;

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}


/**********************************************************************
**
** Function:    NDT_ValidateCertificate
**
** Synopsis:    Validates the WMDRM-ND device certificate.
**
** Arguments:   [f_pNdtMgrContext]            - NDT Manager Context
**              [f_rgchDeviceCertificate]     - Specifies the device certificate
**              [f_cbDeviceCertificate]       - Specifies the size (in bytes) of the Device Certificate
**              [f_pdasstrWMDRMNDCertificate] - Optional, can be NULL.  If not NULL, returns the
**                                              DRM_SUBSTRING pointing to the WMDRM-ND certificate
**              [f_pCertificateInfo]          - Optional, can be NULL.  If not NULL, returns the
**                                              certificate info.
**              [f_ppbCRL]                    - Optional, can be NULL.  If not NULL, returns the
**                                              revocation list buffer
**              [f_pcbCRL]                    - Optional, can be NULL.  If not NULL, returns the
**                                              size of the revocation list buffer
**              [f_pdwCRLVersion]             - Optional, can be NULL.  If not NULL, returns the
**                                              version of the revocation list
**
** Returns:     DRM_SUCCESS on success or
**              DRM_E_INVALIDARG if any of the arguments are invalid
**
**********************************************************************/
DRM_RESULT NDT_ValidateCertificate(
    __in        NDTMGR_CONTEXT_INTERNAL                        *f_pNdtMgrContext,
    __in_ecount(f_cbDeviceCertificate) const DRM_CHAR   * const f_rgchDeviceCertificate,
    __in const  DRM_DWORD                                       f_cbDeviceCertificate,
    __inout     DRM_SUBSTRING                                  *f_pdasstrWMDRMNDCertificate,
    __out       NDT_CERTIFICATE_INFO                    * const f_pCertificateInfo,
    __deref_opt_out_bcount(*f_pcbCRL) DRM_BYTE                **f_ppbCRL,
    __out_opt   DRM_DWORD                               * const f_pcbCRL,
    __out       DRM_DWORD                               * const f_pdwCRLVersion
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_SUBSTRING subString;
    DRM_SUBSTRING dasstrWMDRMNDCertificate;
    NDT_CERTIFICATE_INFO certificateInfo = CERTIFICATE_INFO_INITIALIZER;
    DRM_BYTE *pbCRL = NULL;
    DRM_DWORD cbCRL = 0;
    DRM_DWORD dwCRLVersion;
    DRM_SUBSTRING dasstrPubkeyLeafmost;
    DRM_DWORD cRevocationEntries = 0;
    WMDRMNET_CRL_ENTRY *pRevocationEntries = NULL;
    DRM_CRYPTO_CONTEXT *poCrypto = NULL;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT, PERF_FUNC_NDT_ValidateCertificate );

    ChkArg(f_pNdtMgrContext && f_rgchDeviceCertificate );

    ChkMem( poCrypto = (DRM_CRYPTO_CONTEXT*)Oem_MemAlloc( SIZEOF( DRM_CRYPTO_CONTEXT ) ) );
    ZEROMEM( poCrypto, SIZEOF( DRM_CRYPTO_CONTEXT ) );

    subString.m_ich = 0;
    subString.m_cch = f_cbDeviceCertificate;

    ChkDR( GetWMDRMNDRevocationList( f_pNdtMgrContext,
                                     &pbCRL,
                                     &cbCRL,
                                     &dwCRLVersion ) );

    if(cbCRL != 0)
    {
        ChkDR( GetWMDRMNETRevocationEntries( pbCRL,
                                             cbCRL,
                                             &cRevocationEntries,
                                             &pRevocationEntries ) );
    }

    dr = DRM_WCP_VerifyCertificateCollection(f_rgchDeviceCertificate,
                                             &subString,
                                             &g_dastrMSNDRootPubKeyB64,
                                             pRevocationEntries,
                                             cRevocationEntries,
                                             &dasstrWMDRMNDCertificate,
                                             &dasstrPubkeyLeafmost,
                                             poCrypto);

    if(dr == DRM_E_CERTIFICATE_REVOKED)
    {
        ChkDR( DRM_E_CERTIFICATE_REVOKED );
    }
    if(DRM_FAILED( dr ))
    {
        ChkDR( DRM_E_INVALIDDEVICECERTIFICATE );
    }

    if ( NULL != f_pdasstrWMDRMNDCertificate )
    {
        MEMCPY(f_pdasstrWMDRMNDCertificate, &dasstrWMDRMNDCertificate, SIZEOF(dasstrWMDRMNDCertificate));
    }

    ChkDR(_GetCertificateAttributes(f_rgchDeviceCertificate,
                                    &dasstrWMDRMNDCertificate,
                                    &certificateInfo.pDevicePublicKey,
                                    &certificateInfo.dwPubKeyModulusSize,
                                    &certificateInfo.dwSecurityLevel,
                                    &certificateInfo.dwWMDRMNDFeatures,
                                    certificateInfo.rgbDigestValue));

    if ( NULL != f_pCertificateInfo )
    {
        MEMCPY(f_pCertificateInfo, &certificateInfo, SIZEOF(certificateInfo));
        certificateInfo.pDevicePublicKey = NULL;
    }

    if( 0 != cbCRL )
    {
        if ( NULL != f_ppbCRL )
        {
            *f_ppbCRL = pbCRL;
            pbCRL = NULL;
        }

        if ( NULL != f_pcbCRL )
        {
            *f_pcbCRL = cbCRL;
        }

        if ( NULL != f_pdwCRLVersion )
        {
            *f_pdwCRLVersion = dwCRLVersion;
        }
    }
    else if ( 0 == cbCRL )
    {
        if ( NULL != f_pcbCRL )
        {
            *f_pcbCRL = 0;
        }
    }
    dr = DRM_SUCCESS;

ErrorExit:
    SAFE_OEM_FREE( poCrypto );
    SAFE_OEM_FREE( pbCRL );
    SAFE_OEM_FREE( pRevocationEntries );
    if ( NULL != certificateInfo.pDevicePublicKey )
    {
        OEM_SECURE_ZERO_MEMORY( certificateInfo.pDevicePublicKey, SIZEOF( DRM_RSA_PUBLIC_KEY ) );
        SAFE_OEM_FREE( certificateInfo.pDevicePublicKey );
    }

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}


/**********************************************************************
**
** Function:    NDT_EncryptSeed
**
** Synopsis:    Encrypts the Seed for the WMDRM-ND Registration Response message
**              as defined in WMDRM-ND Protocol Specification Section 14.2.6.
**
** Arguments:   [f_pDevicePublicKey] - Specifies the RSA Public Key.
**              [f_pSeed]            - Specifies the Seed to derive the content encryption
**                                     and integrity keys.
**              [f_wBufferSize]      - Specifies the size of the f_pbEncryptedSeed provided
**                                     for returning the encrypted seed to the caller.
**              [f_pbEncryptedSeed]  - Buffer to return the encrypted seed.
**              [f_pdwBytesWritten]  - Returns the size of buffer written.  It should be
**                                     equal to BufferSize.
**
** Returns:     DRM_SUCCESS on success
**              DRM_E_INVALIDARG - Pointer Parameter NULL
**              DRM_E_PARAMETERS_MISMATCHED - Seed Encryption failed to match buffer size
**
**********************************************************************/
DRM_RESULT NDT_EncryptSeed(
    __in                        const DRM_RSA_PUBLIC_KEY  * const f_pDevicePublicKey,
    __in                        const DRM_ID              * const f_pSeed,
    __in                        const DRM_DWORD                   f_wBufferSize,
    __out_bcount(f_wBufferSize) const DRM_BYTE            * const f_pbEncryptedSeed,
    __out                             DRM_DWORD           * const f_pdwBytesWritten )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_CRYPTO_CONTEXT *poCrypto = NULL;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT, PERF_FUNC_NDT_EncryptSeed );

    ChkArg( f_pDevicePublicKey != NULL );
    ChkArg( f_pSeed            != NULL );
    ChkArg( f_pbEncryptedSeed  != NULL );
    ChkArg( f_pdwBytesWritten  != NULL );

    ChkMem( poCrypto = (DRM_CRYPTO_CONTEXT*)Oem_MemAlloc( SIZEOF( DRM_CRYPTO_CONTEXT ) ) );
    ZEROMEM( poCrypto, SIZEOF( DRM_CRYPTO_CONTEXT ) );

    *f_pdwBytesWritten = f_wBufferSize;
    ChkDR( OEM_RSA_OaepEncrypt(              NULL,
                                             f_pDevicePublicKey,
                                             SIZEOF( *f_pSeed ),
                                ( DRM_BYTE* )f_pSeed,
                                             f_pdwBytesWritten,
                                             f_pbEncryptedSeed,
                                             poCrypto ) );

    ChkBOOL( f_wBufferSize == *f_pdwBytesWritten, DRM_E_PARAMETERS_MISMATCHED );

ErrorExit:
    SAFE_OEM_FREE( poCrypto );

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}



/**********************************************************************
**
** Function:    NDT_SignRegistrationMessage
**
** Synopsis:    Signs the WMDRM-ND Registration Response message
**              as defined in WMDRM-ND Protocol Specification Section 14.2.7.
**
**              The OMAC must be computed using the Content Integrity key (KI)
**              and a zero initialization vector.
**
** Arguments:   [f_pbMessage]   - Specifies the Registration Response Message
**              [f_cbMessage]   - Specifies the size (in bytes) of the Registration Response Message
**              [f_pSeed]       - Specifies the Seed to derive the integrity key.
**              [f_cbSignature] - Specifies the size of the Signature Buffer provided for returning
**                                the signature to the caller.
**              [f_pbSignature] - Pre-allocated buffer to return the message signature.
**              [f_pdwBytesWritten] - Returns the size of buffer written.  It should be
**                                    equal to SignatureSize.
**
** Returns:     DRM_SUCCESS on success or
**              DRM_E_INVALIDARG - Parameter NULL Pointer or
**              DRM_E_OUTOFMEMORY - Failed to allocate AES Key or
**              DRM_E_FAIL - Signature operation failed.
**
**********************************************************************/
DRM_RESULT NDT_SignRegistrationMessage(
    __in_bcount(f_cbMessage) const  DRM_BYTE    * const f_pbMessage,
    __in const                      DRM_DWORD           f_cbMessage,
    __in const                      DRM_ID      * const f_pSeed,
    __in const                      DRM_DWORD           f_cbSignature,
    __inout_bcount(f_cbSignature)   DRM_BYTE    * const f_pbSignature,
    __out                           DRM_DWORD   * const f_pdwBytesWritten
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_AES_KEY *pContentIntegrityKey = NULL;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT, PERF_FUNC_NDT_SignRegistrationMessage );

    ChkArg( f_pbMessage && f_pSeed && f_pbSignature && f_pdwBytesWritten );
    ChkBOOL( f_cbSignature >= DRM_AES_BLOCKLEN, DRM_E_BUFFERTOOSMALL );
    *f_pdwBytesWritten = 0;

    /*
     * Get Content Integrity Key From Seed
     */
    pContentIntegrityKey = WmdrmNetGetRegistrationKey( f_pSeed, TRUE );
    ChkMem(pContentIntegrityKey);

    /*
     * Sign Message using Content Integrity Key
     */
    dr = Oem_Omac1_Sign( pContentIntegrityKey, f_pbMessage, 0, f_cbMessage, f_pbSignature );
    *f_pdwBytesWritten = DRM_AES_BLOCKLEN;

    if(DRM_FAILED(dr)) {
        dr = DRM_E_FAIL;
        goto ErrorExit;
    }

ErrorExit:

    if( pContentIntegrityKey != NULL ) {
        Oem_Aes_ZeroKey( pContentIntegrityKey );
    }

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}



/**********************************************************************
**
** Function:    WmdrmNetGetRegistrationKey
**
** Synopsis:    Computes the content integrity key
**              or the content encryption key from the registration seed.
**
** Arguments:   [f_pSeed] - Specifies the seed to compute the key from
**              [f_bComputeContentIntegrityKey] - Specify TRUE if the content integrity key
**                                                is to be computed.  Specify FALSE if the
**                                                content encryption key is to be computed
**
** Returns:
**      The requested AES secret key
**      The returned key must be freed using DrmAesDeleteKey().
**      NULL not enough memory
**
**********************************************************************/
DRM_API DRM_AES_KEY * DRM_CALL WmdrmNetGetRegistrationKey(
    __in const DRM_ID    * const f_pSeed,
    __in const DRM_BOOL          f_bComputeContentIntegrityKey
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_ID Constant;
    DRM_SHA_CONTEXT shaCtx;
    DRM_BYTE shaCalculatedDigest[ DRM_SHA1_DIGEST_LEN ];
    DRM_AES_KEY *pAesKey = NULL;

    /*
     * Compute the constant to hash in with the seed
     *
     * The content integrity key uses a 128-bit big endian constant of 2.
     * The content encryption key uses a 128-bit big endian constant of 1.
     */
    ZEROMEM( &Constant, SIZEOF(Constant) );
    if ( f_bComputeContentIntegrityKey )
    {
        PUT_BYTE(Constant.rgb, SIZEOF(Constant) - 1, 2);
    }
    else
    {
        PUT_BYTE(Constant.rgb, SIZEOF(Constant) - 1, 1);
    }

    /*
     * Compute the SHA1 digest of the seed concatenated with the constant
     */
    ChkDR( DRM_SHA_Init( &shaCtx, eDRM_SHA_1)  );
    ChkDR( DRM_SHA_Update( (DRM_BYTE *)f_pSeed, SIZEOF(*f_pSeed), &shaCtx ) );
    ChkDR( DRM_SHA_Update( (DRM_BYTE *)&Constant, SIZEOF(Constant), &shaCtx ) );
    ChkDR( DRM_SHA_Finalize( &shaCtx, DRM_SHA1_DIGEST_LEN, shaCalculatedDigest ) );

    /*
     * Use the first 128 bits as the AES key
     */

    DRMCASSERT( DRM_AES_KEYSIZE_128 <= DRM_SHA1_DIGEST_LEN );

    ChkMem( pAesKey = (DRM_AES_KEY*)Oem_MemAlloc( SIZEOF( DRM_AES_KEY ) ) );

    ChkDR( Oem_Aes_SetKey( shaCalculatedDigest, pAesKey ) );

ErrorExit:

    if( DRM_FAILED( dr ) )
    {
        SAFE_OEM_FREE( pAesKey );
    }

    return pAesKey;
}

EXIT_PK_NAMESPACE_CODE;

