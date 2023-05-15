/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_BLACKBOXINTERNAL_C

#include <blackboxinternal.h>
#include <drmdomainapi.h>
#include <drmprofile.h>
#include <drmprnd.h>
#include <drmderivedkey.h>
#include <blackboxhal.h>

#include <drmlastinclude.h>


ENTER_PK_NAMESPACE_CODE;

/*********************************************************************
**
** Function: _GetDeviceECC256KeyPair
**
** Synopsis: Function that retrieves a device's ECC-256 key pair. One of
**           the output parameters may be NULL.
**
** Arguments:
**
** [f_poBBXContext]         -- Pointer to a DRM_BB_CONTEXT context.
** [f_poDevPubKeyIn]        -- Pointer to a PUBKEY_P256 that is used to
**                             specify which device ECC-256 key pair to use.
**                             This parameter is optional and if it is NULL,
**                             The current key pair is used.
** [f_poDevPubKey]          -- Pointer to a PUBKEY_P256 that is used to
**                             receive the device's ECC-256 public key.
**                             This parameter is optional.
** [f_phDevPrivKey]         -- Pointer to an OEM_CRYPTO_HANDLE that is used to
**                             receive the device's ECC-256 private key.
** [f_eDevKeyUsage]         -- Key usage that defines which key pair to return.
**                             Device cert has two keys with Sign and Encrypt key usages
**                             (don't confuse this enum with key usages defined in drmbcert.h)
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
** Notes:                   None.
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL _GetDeviceECC256KeyPair(
    __in              DRM_BB_CONTEXT                      *f_poBBXContext,
    __in_opt    const PUBKEY_P256                         *f_poDevPubKeyIn,
    __out_opt         PUBKEY_P256                         *f_poDevPubKey,
    __out_ecount( 1 ) OEM_CRYPTO_HANDLE                   *f_phDevPrivKey,
    __in              eDRM_BINARY_DEVICE_CERT_KEYUSAGE     f_eDevKeyUsage )
{
    DRM_RESULT          dr          = DRM_SUCCESS;
    DRM_ENCRYPTED_KEY   oEncKey     = { 0 };
    OEM_CRYPTO_HANDLE   hKeyFileKey = OEM_CRYPTO_HANDLE_INVALID;
    OEM_CRYPTO_HANDLE   hPrivKey    = OEM_CRYPTO_HANDLE_INVALID;
    OEM_HAL_KEY_TYPE    keyType     = OEM_HAL_KEY_TYPE_INVALID;

    ChkArg( f_poBBXContext != NULL );
    ChkArg( f_phDevPrivKey != NULL );

    *f_phDevPrivKey = OEM_CRYPTO_HANDLE_INVALID;

    switch ( f_eDevKeyUsage )
    {
        case DRM_BINARY_DEVICE_CERT_KEY_SIGN:
            if ( f_poDevPubKeyIn == NULL )
            {
                f_poDevPubKeyIn = &(f_poBBXContext->cachedBCertValues.DeviceKeySign.PublicKeyContext.PublicKey);
            }

            keyType = OEM_HAL_KEY_TYPE_ECC_P256_SIGN;

            break;
        case DRM_BINARY_DEVICE_CERT_KEY_ENCRYPT:
            if ( f_poDevPubKeyIn == NULL )
            {
                f_poDevPubKeyIn = &(f_poBBXContext->cachedBCertValues.DeviceKeyEncrypt.PublicKeyContext.PublicKey);
            }

            keyType = OEM_HAL_KEY_TYPE_ECC_P256_CONTENT_ENCRYPT;

            break;

        case DRM_BINARY_DEVICE_CERT_KEY_PRND_ENCRYPT:
            ChkBOOL( DRM_PRND_IsPRNDReceiverSupported(), DRM_E_NOTIMPL );

            if ( f_poDevPubKeyIn == NULL )
            {
                DRMASSERT( f_poBBXContext->cachedBCertValues.fDeviceKeyPrndEncryptSet );
                f_poDevPubKeyIn = &(f_poBBXContext->cachedBCertValues.DeviceKeyPrndEncrypt.PublicKeyContext.PublicKey);
            }

            keyType = OEM_HAL_KEY_TYPE_ECC_P256_MESSAGE_ENCRYPT;

            break;

        default:
            ChkDR( DRM_E_INVALIDARG );
            break;
    }

    if ( f_poDevPubKey != NULL )
    {
        MEMCPY( f_poDevPubKey, f_poDevPubKeyIn, SIZEOF( PUBKEY_P256 ) );
    }

    ChkDR( DRM_KF_GetPrivateKey( f_poBBXContext->pKeyFileContext,
                                 eKF_KEY_TYPE_ECC_256,
                                 ( const DRM_BYTE * )f_poDevPubKeyIn,
                                 SIZEOF( PUBKEY_P256 ),
                                 &oEncKey ) );

    ChkDR( _Hal_GetPreloadedKey( f_poBBXContext,
                                 OEM_HAL_KEY_TYPE_AES_KEYFILE,
                                 &oEncKey.guidKeypairDecryptionKeyID,
                                 &hKeyFileKey ) );

    ChkDR( _Hal_UnwrapKey( f_poBBXContext,
                           keyType,
                           hKeyFileKey,
                           ( DRM_BYTE* )&oEncKey,
                           SIZEOF( oEncKey ),
                           NULL,
                           0,
                           oEncKey.pbEncryptedKey,
                           oEncKey.cbEncryptedKey,
                           &hPrivKey ) );

    *f_phDevPrivKey = hPrivKey;
    hPrivKey        = OEM_CRYPTO_HANDLE_INVALID;

ErrorExit:
    DRM_BBX_SafeReleaseKey( f_poBBXContext, &hKeyFileKey );
    DRM_BBX_SafeReleaseKey( f_poBBXContext, &hPrivKey );

    return dr;
}

/*********************************************************************
**
** Function: _ValidateBindingInfo_Derived
**
** Synopsis: Function that validates whether a chain of binding info is
**           from a derived key based license chain.
**
** Arguments:
**
** [f_poBindingInfo]        -- Pointer to the binding info chain.
** [f_dwChainLen]           -- Number of items in thje binding info chain.
** [f_pfDerived]            -- Pointer to a flag to receive the result of
**                             whether the binding info chain is from a
**                             derived key based license chain.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
static DRM_RESULT _ValidateBindingInfo_Derived(
    __in_ecount( f_dwChainLen )
          const DRM_BINDING_INFO *f_poBindingInfo,
    __in        DRM_DWORD         f_dwChainLen,
    __out       DRM_BOOL         *f_pfDerived )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD  i  = 0;
    DRM_BOOL   fAuxKey = FALSE, fUplinkX = FALSE;

    ChkArg( f_poBindingInfo != NULL && f_dwChainLen > 0 );
    ChkArg( f_pfDerived != NULL );

    *f_pfDerived = FALSE;

    if ( f_dwChainLen > 2 )
    {
        goto ErrorExit;
    }

    for ( i = 0; i < f_dwChainLen; i++ )
    {
        if ( XMR_IS_AUX_KEY_VALID( f_poBindingInfo[i].m_BindingInfo.m_XMR.m_pxmrLicense ) )
        {
            if ( !fAuxKey )
            {
                fAuxKey = TRUE;
            }
            else
            {
                ChkDR( DRM_E_INVALIDARG );
            }
        }
        else if ( XMR_IS_UPLINKX_VALID( f_poBindingInfo[i].m_BindingInfo.m_XMR.m_pxmrLicense ) )
        {
            if ( !fUplinkX )
            {
                fUplinkX = TRUE;
            }
            else
            {
                ChkDR( DRM_E_INVALIDARG );
            }
        }
    }

    if ( f_dwChainLen > 1 &&
         fAuxKey != fUplinkX )
    {
        ChkDR( DRM_E_INVALIDARG );
    }

    if ( fAuxKey || fUplinkX )
    {
        *f_pfDerived = TRUE;
    }

ErrorExit:
    return dr;
}


static DRMFORCEINLINE OEM_HAL_KEY_TYPE _TranslateKeyType(
    DRM_WORD wXmrKeyType ) DRM_ALWAYS_INLINE_ATTRIBUTE;

static DRMFORCEINLINE OEM_HAL_KEY_TYPE _TranslateKeyType(
    DRM_WORD wXmrKeyType )
{
    OEM_HAL_KEY_TYPE eKeyType = OEM_HAL_KEY_TYPE_INVALID;

    switch( wXmrKeyType )
    {
    case XMR_SYMMETRIC_ENCRYPTION_TYPE_AES_128_CTR:
        eKeyType = OEM_HAL_KEY_TYPE_AES_CTR_CONTENT;
        break;

    case XMR_SYMMETRIC_ENCRYPTION_TYPE_AES_128_ECB:
        eKeyType = OEM_HAL_KEY_TYPE_AES_ECB_ROOT;
        break;

    case XMR_SYMMETRIC_ENCRYPTION_TYPE_RC4_CIPHER:
    case XMR_SYMMETRIC_ENCRYPTION_TYPE_COCKTAIL:
        eKeyType = OEM_HAL_KEY_TYPE_LEGACY;
        break;

    default:
        break;
    }

    return eKeyType;
}


/*********************************************************************
**
** Function: _InitUnwrapInfo
**
** Synopsis: Initialize an DRM_XMR_UNWRAP_INFO structure from a
**           DRM_BINDING_INFO structure
**
** Arguments:
**
** [f_pUnwrapInfo]          -- Pointer to the unwrap info struct.
** [f_poBindingInfo]        -- Pointer to a single binding info struct
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
static DRMFORCEINLINE DRM_RESULT _InitUnwrapInfo(
    __out_ecount(1)       DRM_XMR_UNWRAP_INFO *f_pUnwrapInfo,
    __in_ecount(1)  const DRM_BINDING_INFO    *f_pBindingInfo ) DRM_ALWAYS_INLINE_ATTRIBUTE;
static DRMFORCEINLINE DRM_RESULT _InitUnwrapInfo(
    __out_ecount(1)       DRM_XMR_UNWRAP_INFO *f_pUnwrapInfo,
    __in_ecount(1)  const DRM_BINDING_INFO    *f_pBindingInfo )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pUnwrapInfo                 != NULL );
    ChkArg( f_pBindingInfo                != NULL );
    ChkArg( f_pBindingInfo->m_fXMRLicense == TRUE );

    f_pUnwrapInfo->pbXMRLic   = f_pBindingInfo->m_BindingInfo.m_XMR.m_pxmrLicense->pbXMRLic;
    f_pUnwrapInfo->cbXMRLic   = f_pBindingInfo->m_BindingInfo.m_XMR.m_pxmrLicense->cbXMRLic;
    f_pUnwrapInfo->poChecksum = &f_pBindingInfo->m_oChecksum;

ErrorExit:

    return dr;
}

/*********************************************************************
**
** Function: _GetCacheKey
**
** Synopsis: Create cache key based on DRM_XMR_UNWRAP_INFO and DRM_BINDING_INFO structures.
**
** Arguments:
**
** [f_pUnwrapInfo]          -- Pointer to the unwrap info struct.
** [f_pBindingInfo]         -- Pointer to the binding info struct.
** [f_pbCacheKeyBuffer]     -- Pointer to a buffer that will contain resulting cache key.
** [f_cbCacheKeyBuffer]     -- Size of f_pbCacheKeyBuffer in bytes.
** [f_cbCacheKey]           -- Pointer to a variable that will contain actual cache key size.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
static DRMFORCEINLINE DRM_RESULT _GetCacheKey(
    __in_ecount(1)                     const DRM_XMR_UNWRAP_INFO  *f_pUnwrapInfo,
    __in_ecount(1)                     const DRM_BINDING_INFO     *f_pBindingInfo,
    __out_bcount( f_cbWrappedKeyData )       DRM_BYTE             *f_pbCacheKeyBuffer,
    __in                               const DRM_DWORD             f_cbCacheKeyBuffer,
    __out                                    DRM_DWORD            *f_cbCacheKey)
{
    DRM_RESULT                     dr                   = DRM_SUCCESS;
    const DRM_BB_CHECKSUM_CONTEXT *poChecksum           = NULL;
    const DRM_XMR_SIGNATURE       *poXmrSignature       = NULL;
    DRM_DWORD                      dwRequiredBufferSize = 0;

    ChkPtr( f_pUnwrapInfo );
    ChkPtr( f_pUnwrapInfo->poChecksum );
    ChkPtr( f_pBindingInfo );
    ChkPtr( f_pBindingInfo->m_BindingInfo.m_XMR.m_pxmrSignature );

    poXmrSignature = f_pBindingInfo->m_BindingInfo.m_XMR.m_pxmrSignature;
    poChecksum     = f_pUnwrapInfo->poChecksum;

    if( poChecksum->m_fIgnoreChecksum == FALSE)
    {
        dwRequiredBufferSize = poXmrSignature->cbSignature + poChecksum->m_cbChecksum;

        DRMASSERT( dwRequiredBufferSize <= f_cbCacheKeyBuffer );

        // Copy license signature
        DRM_BYT_CopyBytes( f_pbCacheKeyBuffer,
                           0,
                           &poXmrSignature->pbSignatureBuffer[poXmrSignature->iSignature],
                           0,
                           poXmrSignature->cbSignature );

        // Copy checksum
        DRM_BYT_CopyBytes( f_pbCacheKeyBuffer,
                            poXmrSignature->cbSignature,
                            poChecksum->m_rgbChecksumData,
                            0,
                            poChecksum->m_cbChecksum );
    }

    *f_cbCacheKey = dwRequiredBufferSize;

ErrorExit:

    return dr;
}

/******************************************************************************
**
** Function :   _DecryptContentKeyXMR
**
** Synopsis :   Decrypt a content key and place it in the given key buffer (XMR)
**
** Arguments :
**          [f_pBindingInfo]    : XMR Binding information
**          [f_dwChainLen]      : Chain length
**          [f_pcontextBBX]     : Blackbox content
**          [f_fIgnoreChecksum] : Specifies to ignore the checksum over the content key
**          [f_cbKeyBuff]       : length of the key buffer
**          [f_pbKeyBuff]       : Pointer to the key buffer
**          [f_phKey]           : A handle to the unwrapped key.
**                                This must be released using DRM_BBX_SafeReleaseKey()
**          [f_phPrivKey]       : Optionally returns the private key used to
**                                decrypt the content key. If obtained
**                                this must be released using DRM_BBX_SafeReleaseKey()
**
** Returns :
**          DRM_SUCCESS
**              Success
**          DRM_E_INVALIDARG
**              A required parameter was NULL or improperly initialized
**          DRM_E_LIC_KEY_DECODE_FAILURE
**              Unable to decode the content key
**          DRM_E_INVALID_SIGNATURE
**              Failed to verify the signature in the license
**          DRM_E_UNSUPPORTED_ALGORITHM
**              An unsupported crypto algorithm was specified in the license
**          DRM_E_BUFFERTOOSMALL
**              The content key was too large for the buffer space
**          DRM_E_DOMAIN_BIND_LICENSE
**              No matching domain private key can be located (domain license only).
**
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL _DecryptContentKeyXMR(
    __in_ecount( f_dwChainLen ) const DRM_BINDING_INFO    *f_pBindingInfo,
    __in                              DRM_DWORD            f_dwChainLen,
    __inout_ecount( 1 )               DRM_BB_CONTEXT      *f_pContextBBX,
    __in                              DRM_BOOL             f_fIgnoreChecksum,
    __out_ecount( 1 )                 OEM_CRYPTO_HANDLE   *f_phKey,            /* the created key handle */
    __out_ecount_opt( 1 )             OEM_CRYPTO_HANDLE   *f_phPrivKey )       /* returns the privkey used to decrypt */
{
    DRM_RESULT                  dr                                                    = DRM_SUCCESS;
    DRM_DWORD                   cbTemp                                                = SHA256_DIGEST_SIZE_IN_BYTES;
    DRM_BB_DOMAIN_KEY_RECORD    oDomainKeyRecord                                      = {0};
    const DRM_XMR_BINDING_INFO *pXmrBindInfo                                          = NULL;
    OEM_CRYPTO_HANDLE           hPrivKey                                              = OEM_CRYPTO_HANDLE_INVALID;
    OEM_CRYPTO_HANDLE           hRootKey                                              = OEM_CRYPTO_HANDLE_INVALID;
    OEM_CRYPTO_HANDLE           hOutputKey                                            = OEM_CRYPTO_HANDLE_INVALID;
    DRM_BOOL                    fDerived                                              = FALSE;
    DRM_XMR_UNWRAP_INFO         unwrapInfo                                            = { 0 };
    DRM_DWORD                   cbCacheKey                                            = 0;
    DRM_BYTE                    rgbCacheKeyBuffer[DRM_BB_MAX_CACHE_KEY_SIZE_IN_BYTES] = { 0 };

    /*
    ** Temp buffer is SHA256_DIGEST_SIZE_IN_BYTES (32 bytes),
    ** to copy the Domain Account ID (128 bits) to get the domain key
    */
    DRM_BYTE rgbAccountID[__CB_DECL( ECC_P256_PLAINTEXT_SIZE_IN_BYTES )]  = { 0 };

    ChkArg( f_phKey        != NULL );
    ChkArg( f_pContextBBX  != NULL );
    ChkArg( f_pBindingInfo != NULL );
    ChkArg( f_dwChainLen   >  0 );
    ChkArg( f_pContextBBX->fInited );
    ChkArg( f_pBindingInfo[f_dwChainLen-1].m_fXMRLicense );

    ChkDR( _ValidateBindingInfo_Derived( f_pBindingInfo,
                                         f_dwChainLen,
                                         &fDerived ) );

    /*
    ** If chain length > 1 pXmrBindInfo points to root license info,
    ** otherwise it points to simple license.
    */

    pXmrBindInfo = &f_pBindingInfo[f_dwChainLen-1].m_BindingInfo.m_XMR;

    ChkArg( pXmrBindInfo->m_pxmrContentKey != NULL );

    ChkBOOL( pXmrBindInfo->m_pxmrContentKey->wKeyEncryptionCipherType
          == XMR_ASYMMETRIC_ENCRYPTION_TYPE_ECC_256
          || pXmrBindInfo->m_pxmrContentKey->wKeyEncryptionCipherType
          == XMR_ASYMMETRIC_ENCRYPTION_TYPE_ECC_256_WITH_KZ,
             DRM_E_UNSUPPORTED_ALGORITHM );
    /*
    **  Find the private key that corresponds to
    **  the public key used to encrypt the content key.
    **  If it is a domain-bound license, use appropriate domain key
    **  Otherwise use the device private key
    */
    if ( pXmrBindInfo->m_pxmrDomainID->fValid )
    {
        ChkBOOL( DRM_DOMAIN_IsDomainSupported(), DRM_E_NOTIMPL );
        ChkArg( f_pContextBBX->pDomainStoreContext != NULL );

        /*
        ** Using DecKeys temporarily to hold the Account ID for the domain
        */
        ChkBOOL( pXmrBindInfo->m_pxmrDomainID->cbAccountID <= ECC_P256_PLAINTEXT_SIZE_IN_BYTES,
                 DRM_E_BUFFERTOOSMALL );
        DRM_BYT_CopyBytes( rgbAccountID,
                           0,
                           pXmrBindInfo->m_pxmrDomainID->pbAccountID,
                           pXmrBindInfo->m_pxmrDomainID->ibAccountID,
                           pXmrBindInfo->m_pxmrDomainID->cbAccountID );
        cbTemp = SIZEOF( DRM_BB_DOMAIN_KEY_RECORD );
        if ( DRM_FAILED( DRM_DOMST_GetKey( f_pContextBBX->pDomainStoreContext,
                                           ( DRM_GUID * )rgbAccountID,
                                           pXmrBindInfo->m_pxmrDomainID->dwRevision,
                                           (DRM_BYTE*)&oDomainKeyRecord,
                                           &cbTemp ) ) )
        {
            ChkDR( DRM_E_DOMAIN_BIND_LICENSE );
        }

        ChkBOOL( oDomainKeyRecord.dwVersion == DOMAIN_KEY_RECORD_VERSION, DRM_E_DOMAIN_STORE_INVALID_KEY_RECORD );
        ChkBOOL( cbTemp == SIZEOF( DRM_BB_DOMAIN_KEY_RECORD ), DRM_E_DOMAIN_STORE_INVALID_KEY_RECORD );

        ChkDR( _SymmetricUnwrapKey( f_pContextBBX,
                                    (DRM_BYTE*)&oDomainKeyRecord.oDomainPrivKeyEncrypted,
                                    SIZEOF(oDomainKeyRecord.oDomainPrivKeyEncrypted),
                                    &hPrivKey ) );
    }
    else
    {
        const DRM_XMR_ECC_DEVICE_KEY *poECCKey = pXmrBindInfo->m_pxmrDeviceEccKey;

        ChkDR( _GetDeviceECC256KeyPair( f_pContextBBX,
                                        ( const PUBKEY_P256 * )( poECCKey->pbKeyData +
                                        __CB_DECL( poECCKey->iKeyData ) ),
                                        NULL,
                                        &hPrivKey,
                                        DRM_BINARY_DEVICE_CERT_KEY_ENCRYPT ) );

    }


    /*
    ** Unwrap the last key in the chain
    */
    if( f_dwChainLen == 1 )
    {
        OEM_HAL_KEY_TYPE eKeyType = _TranslateKeyType( pXmrBindInfo->m_pxmrContentKey->wSymmetricCipherType );

        ChkBOOL( eKeyType != OEM_HAL_KEY_TYPE_INVALID, DRM_E_UNSUPPORTED_ALGORITHM );

        ChkDR( _InitUnwrapInfo( &unwrapInfo,
                                &f_pBindingInfo[0] ) );

        if( _Hal_IsHandleCachingSupported() )
        {
            ChkDR( _GetCacheKey( &unwrapInfo,
                                 &f_pBindingInfo[0],
                                 rgbCacheKeyBuffer,
                                 DRM_BB_MAX_CACHE_KEY_SIZE_IN_BYTES,
                                 &cbCacheKey ) );
        }
        ChkDR( _Hal_UnwrapKey( f_pContextBBX,
                               eKeyType,
                               hPrivKey,
                               ( const DRM_BYTE* )&unwrapInfo,
                               SIZEOF( unwrapInfo ),
                               ( DRM_BYTE* )&f_fIgnoreChecksum,
                               SIZEOF( f_fIgnoreChecksum ),
                               rgbCacheKeyBuffer,
                               cbCacheKey,
                               &hOutputKey ) );
    }
    else
    {
        OEM_HAL_KEY_TYPE    eKeyType   = OEM_HAL_KEY_TYPE_INVALID;

        /*
        ** ECB mode is allowed if and only if the license is a root license.
        ** However, we can tell a root license only when license chain length is > 1
        ** because the format of a root license and a simple license is the same.
        */
        ChkBOOL( pXmrBindInfo->m_pxmrContentKey->wSymmetricCipherType
            == XMR_SYMMETRIC_ENCRYPTION_TYPE_AES_128_ECB, DRM_E_UNSUPPORTED_ALGORITHM );

        ChkDR( _InitUnwrapInfo( &unwrapInfo,
                                &f_pBindingInfo[1] ) );

        if( _Hal_IsHandleCachingSupported() )
        {
            ChkDR( _GetCacheKey( &unwrapInfo,
                                 &f_pBindingInfo[1],
                                 rgbCacheKeyBuffer,
                                 DRM_BB_MAX_CACHE_KEY_SIZE_IN_BYTES,
                                 &cbCacheKey ) );
        }

        ChkDR( _Hal_UnwrapKey( f_pContextBBX,
                               OEM_HAL_KEY_TYPE_AES_ECB_ROOT,
                               hPrivKey,
                               ( const DRM_BYTE* )&unwrapInfo,
                               SIZEOF( unwrapInfo ),
                               ( DRM_BYTE* )&f_fIgnoreChecksum,
                               SIZEOF( f_fIgnoreChecksum ),
                               rgbCacheKeyBuffer,
                               cbCacheKey,
                               &hRootKey ) );

        pXmrBindInfo = &f_pBindingInfo[0].m_BindingInfo.m_XMR;
        eKeyType     = _TranslateKeyType( pXmrBindInfo->m_pxmrContentKey->wSymmetricCipherType );

        ChkBOOL( eKeyType != OEM_HAL_KEY_TYPE_INVALID, DRM_E_UNSUPPORTED_ALGORITHM );

        /*
        ** We are now in chained licenses enabling bits.  The algorithm *must* be chained
        */
        ChkBOOL( pXmrBindInfo->m_pxmrContentKey->wKeyEncryptionCipherType
              == XMR_ASYMMETRIC_ENCRYPTION_TYPE_CHAINED_LICENSE,
                 DRM_E_UNSUPPORTED_ALGORITHM );

        ChkDR( _InitUnwrapInfo( &unwrapInfo,
                                &f_pBindingInfo[0] ) );

        if( _Hal_IsHandleCachingSupported() )
        {
            ChkDR( _GetCacheKey( &unwrapInfo,
                                 &f_pBindingInfo[0],
                                 rgbCacheKeyBuffer,
                                 DRM_BB_MAX_CACHE_KEY_SIZE_IN_BYTES,
                                 &cbCacheKey ) );
        }

        ChkDR( _Hal_UnwrapKey( f_pContextBBX,
                               eKeyType,
                               hRootKey,
                               ( const DRM_BYTE* )&unwrapInfo,
                               SIZEOF( unwrapInfo ),
                               ( DRM_BYTE* )&f_fIgnoreChecksum,
                               SIZEOF( f_fIgnoreChecksum ),
                               rgbCacheKeyBuffer,
                               cbCacheKey,
                               &hOutputKey ) );
    }

    if( f_phPrivKey != NULL )
    {
        *f_phPrivKey = hPrivKey;
        hPrivKey     = OEM_CRYPTO_HANDLE_INVALID;
    }

    *f_phKey   = hOutputKey;
    hOutputKey = OEM_CRYPTO_HANDLE_INVALID;

ErrorExit:

    OEM_SECURE_ZERO_MEMORY( rgbAccountID, ECC_P256_PLAINTEXT_SIZE_IN_BYTES );

    DRM_BBX_SafeReleaseKey( f_pContextBBX, &hPrivKey );
    DRM_BBX_SafeReleaseKey( f_pContextBBX, &hRootKey );
    DRM_BBX_SafeReleaseKey( f_pContextBBX, &hOutputKey );

    return dr;
}

/*********************************************************************
**
** Function: _SymmetricWrapKey
**
** Synopsis: Function that wraps the passed in private key with a
**           symmetric key derived from the device's private key.
**
** Arguments:
** [f_poBBXContext]     : Specifies a pre-initialized blackbox context
** [f_hKeyToWrap]       : the handle of the key to encrypt
** [f_cbData]           : Size (number of bytes) of the output buffer.
**                        This must be the size of an ECC-256
**                        private key
** [f_pbDataOut]        : Pointer to a buffer that the result of
**                        the ciphering is stored.
**
** Returns:
**      DRM_SUCCESS
**          Success.
**      DRM_E_INVALIDARG
**          One of the arguments is invalid or improperly initialized
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL _SymmetricWrapKey(
    __inout_ecount( 1 )          DRM_BB_CONTEXT    *f_pBbxContext,
    __in_ecount( 1 )             OEM_CRYPTO_HANDLE  f_hKeyToWrap,
    __in                         DRM_DWORD          f_cbData,
    __out_bcount_opt( f_cbData ) DRM_BYTE          *f_pbDataOut )
{
    DRM_RESULT        dr       = DRM_E_LOGICERR;
    OEM_CRYPTO_HANDLE hPrivKey = OEM_CRYPTO_HANDLE_INVALID;

    ChkPtr( f_pBbxContext );
    ChkArg( f_pBbxContext->fInited );
    ChkPtr( f_pbDataOut );
    ChkArg( f_cbData == SIZEOF( PRIVKEY_P256 ) );

    ChkDR( _GetDeviceECC256KeyPair( f_pBbxContext,
                                    NULL,
                                    NULL,
                                    &hPrivKey,
                                    DRM_BINARY_DEVICE_CERT_KEY_ENCRYPT ) );

    ChkDR( _Hal_WrapKey( f_pBbxContext,
                         f_hKeyToWrap,
                         hPrivKey,
                         f_pbDataOut,
                         &f_cbData ) );

ErrorExit:

    DRM_BBX_SafeReleaseKey( f_pBbxContext, &hPrivKey );

    return dr;
}


/*********************************************************************
**
** Function: _SymmetricUnwrapKey
**
** Synopsis: Function that unwraps the passed in private ECC256 key data
**           with a symmetric key derived from the device's private key.
**
** Arguments:
** [f_poBBXContext]     : Specifies a pre-initialized blackbox context
** [f_pbDataIn]         : Buffer containing encrypted private key data
** [f_cbData]           : Size (number of bytes) of the input buffer.
**                        This must be the size of an ECC-256
**                        private key
** [f_phKey]            : Pointer to variable that will be assigned the
**                        handle of the unwrapped key. This must be
**                        released with _Hal_ReleaseKey after use.
**
** Returns:
**      DRM_SUCCESS
**          Success.
**      DRM_E_INVALIDARG
**          One of the arguments is invalid or improperly initialized
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL _SymmetricUnwrapKey(
    __inout_ecount( 1 )           DRM_BB_CONTEXT    *f_poBBXContext,
    __in_bcount( f_cbData ) const DRM_BYTE          *f_pbDataIn,
    __in                          DRM_DWORD          f_cbData,
    __out_ecount( 1 )             OEM_CRYPTO_HANDLE *f_phKey )
{
    DRM_RESULT        dr       = DRM_E_LOGICERR;
    OEM_CRYPTO_HANDLE hPrivKey = OEM_CRYPTO_HANDLE_INVALID;

    ChkPtr( f_poBBXContext );
    ChkArg( f_poBBXContext->fInited );
    ChkPtr( f_pbDataIn );
    ChkArg( f_cbData == SIZEOF( PRIVKEY_P256 ) );
    ChkPtr( f_phKey );

    ChkDR( _GetDeviceECC256KeyPair( f_poBBXContext,
                                    NULL,
                                    NULL,
                                    &hPrivKey,
                                    DRM_BINARY_DEVICE_CERT_KEY_ENCRYPT ) );

    ChkDR( _Hal_UnwrapKey( f_poBBXContext,
                           OEM_HAL_KEY_TYPE_ECC_P256_DOMAIN_ENCRYPT,
                           hPrivKey,
                           f_pbDataIn,
                           f_cbData,
                           NULL,
                           0,
                           f_pbDataIn,
                           f_cbData,
                           f_phKey ) );

ErrorExit:

    DRM_BBX_SafeReleaseKey( f_poBBXContext, &hPrivKey );

    return dr;
}


EXIT_PK_NAMESPACE_CODE;

