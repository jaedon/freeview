/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_BLACKBOXSLK_C
#include <blackboxlegacy.h>
#include <blackboxinternal.h>
#include <blackboxslk.h>
#include <blackboxhal.h>
#include <oemeccp160.h>
#include <oemkeyfile.h>
#include <drmconstants.h>
#include <drmprofile.h>
#include <drmprofileconstants.h>
#include <drmwmdrm.h>
#include <drmlastinclude.h>

#if DRM_SUPPORT_SECUREOEMHAL
#error DRM_SUPPORT_SECUREOEMHAL is set.  You can not implement a secure HAL that supports blackbox SLK crypto.  You must link to the stub version of the blackboxslk module.
#endif /* DRM_SUPPORT_SECUREOEMHAL */

ENTER_PK_NAMESPACE_CODE;

/*
** Functions defining whether Symmetric License Keys (SLKs) are supported
*/
DRM_API DRM_NO_INLINE DRM_BOOL DRM_CALL DRM_BBX_IsSlkCryptoSupported(DRM_VOID)
{
    return TRUE;
}
DRM_API DRM_NO_INLINE DRM_BOOL DRM_CALL DRM_BBX_IsSlkCryptoUnsupported(DRM_VOID)
{
    return FALSE;
}

static DRM_RESULT _Legacy_RebindSymmetricKey(
    __inout                                DRM_BB_CONTEXT       *f_pcontextBBX,
    __in                             const DRM_SLK              *f_pslk,
    __inout_bcount( f_cbKeyBuffer )        DRM_BYTE             *f_pbEncryptedKeys,
    __in                                   DRM_DWORD             f_cbEncryptedKeys,
    __in                                   DRM_DWORD             f_cbKeyBuffer,
    __out_bcount( f_cbKeyBuffer )          DRM_BYTE             *f_pbDecryptedKeys );

static DRM_RESULT _Legacy_RebindSLK(
    __inout_ecount( 1 )             DRM_BB_CONTEXT *f_pcontextBBX,
    __in_bcount( f_cbEncSLK ) const DRM_BYTE       *f_pbEncSLK,
    __in                            DRM_DWORD       f_cbEncSLK,
    __out_ecount( 1 )               DRM_SLK        *f_pslk );

/*********************************************************************
**
**  Function:  _RebindSymmetricKey
**
**  Synopsis:  Takes an license which is symmetrically optimized to an SLK
**             and re-encrypts it to the device (or domain) symmetric key.
**             Legacy or current crypto is inferred from SLK size.
**
**  Arguments:
**     [f_pcontextBBX]       : Pointer to an initialized blackbox context
**     [f_pslk]              : Pointer to current SLK data
**     [f_pbEncryptedKeys]   : Pointer to the symmetrically encrypted key data.
**                             Will be updated in place
**     [f_cbEncryptedKeys]   : Length of the symmetrically encrypted key data.
**     [f_pbDecryptedKeys]   : Returns the decrypted key data (including CI key for XMR license)
**
*********************************************************************/
static DRM_RESULT _RebindSymmetricKey(
    __inout                                DRM_BB_CONTEXT       *f_pcontextBBX,
    __in                             const DRM_SLK              *f_pslk,
    __inout_bcount( f_cbKeyBuffer )        DRM_BYTE             *f_pbEncryptedKeys,
    __in                                   DRM_DWORD             f_cbEncryptedKeys,
    __out_ecount( 1 )                      OEM_CRYPTO_HANDLE    *f_hDecryptedKeys )
{
    DRM_RESULT                  dr             = DRM_SUCCESS;
    OEM_CRYPTO_HANDLE           hPrivKey       = OEM_CRYPTO_HANDLE_INVALID;
    OEM_CRYPTO_HANDLE           hSlkKey        = OEM_CRYPTO_HANDLE_INVALID;
    OEM_CRYPTO_HANDLE           hContentKey    = OEM_CRYPTO_HANDLE_INVALID;
    OEM_HAL_SLK_ENCRYPTION_TYPE eSlkCryptoType = OEM_HAL_SLK_ENCRYPTION_TYPE_DERIVED_SYMMETRIC;

    ChkArg( f_pcontextBBX      != NULL );
    ChkArg( f_pslk             != NULL );
    ChkArg( f_pbEncryptedKeys  != NULL );
    ChkArg( f_pcontextBBX->fInited     );
    ChkArg( f_cbEncryptedKeys  == 2 * DRM_AES_KEYSIZE_128 );
    ChkArg( f_pslk->cbKey      == DRM_AES_KEYSIZE_128 );

    ChkBOOL( DRM_BBX_IsSlkCryptoSupported(), DRM_E_NOTIMPL );

    ChkDR( _GetDeviceECC256KeyPair( f_pcontextBBX,
                                    NULL,
                                    NULL,
                                    &hPrivKey,
                                    DRM_BINARY_DEVICE_CERT_KEY_ENCRYPT ) );

    /*
    ** Unwrap the key using a symmetric key derived from the private key
    */
    ChkDR( _Hal_UnwrapKey( f_pcontextBBX,
                           OEM_HAL_KEY_TYPE_AES_SLK,
                           hPrivKey,
                           f_pslk->rgbKey,
                           f_pslk->cbKey,
                           ( DRM_BYTE* )&eSlkCryptoType,
                           SIZEOF( eSlkCryptoType ),
                           f_pslk->rgbKey,
                           f_pslk->cbKey,
                           &hSlkKey ) );

    /*
    ** Unwrap the content key using the SLK key
    */
    ChkDR( _Hal_UnwrapKey( f_pcontextBBX,
                           OEM_HAL_KEY_TYPE_AES_CTR_CONTENT,
                           hSlkKey,
                           f_pbEncryptedKeys,
                           f_cbEncryptedKeys,
                           NULL,
                           0,
                           f_pbEncryptedKeys,
                           f_cbEncryptedKeys,
                           &hContentKey ) );

    /*
    ** Wrap the content key using the symmetric key derived from the private key
    */
    ChkDR( _Hal_WrapKey( f_pcontextBBX,
                         hContentKey,
                         hPrivKey,
                         f_pbEncryptedKeys,
                         &f_cbEncryptedKeys ) );

    *f_hDecryptedKeys = hContentKey;
    hContentKey       = OEM_CRYPTO_HANDLE_INVALID;

ErrorExit:

    DRM_BBX_SafeReleaseKey( f_pcontextBBX, &hPrivKey );
    DRM_BBX_SafeReleaseKey( f_pcontextBBX, &hSlkKey );
    DRM_BBX_SafeReleaseKey( f_pcontextBBX, &hContentKey );

    return dr;
}

/*********************************************************************
**
**  Function:  _SLK_RebindSymmetricKeyXMR
**
**  Synopsis:  Takes an license which is symmetrically optimized to an SLK
**             and re-encrypts it to the device (or domain) symmetric key.
**             Legacy or current crypto is inferred from SLK size.
**             Reencrypts and updates the specified XMR license.
**
**  Arguments:
**     [f_pcontextBBX] : Pointer to an initialized blackbox context
**     [f_pslk]        : Pointer to current SLK data
**     [f_pXmrLicense] : Pointer to an XMR license object to update with the new key and signature
**
*********************************************************************/
DRM_API DRM_RESULT DRM_CALL _SLK_RebindSymmetricKeyXMR(
    __inout         DRM_BB_CONTEXT       *f_pcontextBBX,
    __in      const DRM_SLK              *f_pslk,
    __inout         DRM_XMR_LICENSE      *f_pXmrLicense )
{
          DRM_RESULT         dr              = DRM_SUCCESS;
          DRM_DWORD          cbEncryptedKeys = 0;
          DRM_BYTE           rgbEncryptedKeys[__CB_DECL( DRM_AES_BLOCKLEN * 2 )] = { 0 };
          DRM_BYTE           rgbDecryptedKeys[__CB_DECL( DRM_AES_BLOCKLEN * 2 )] = { 0 };
    const DRM_BYTE          *pbSigningKey    = NULL;
          OEM_CRYPTO_HANDLE  hSigningKey     = OEM_CRYPTO_HANDLE_INVALID;

    ChkArg( f_pcontextBBX != NULL );
    ChkArg( f_pslk        != NULL );
    ChkArg( f_pXmrLicense != NULL );

    ChkBOOL( DRM_BBX_IsSlkCryptoSupported(), DRM_E_NOTIMPL );

    cbEncryptedKeys = f_pXmrLicense->containerOuter.containerKeys.OptimizedContentKey.cbEncryptedKey;
    ChkBOOL( f_pXmrLicense->containerOuter.containerKeys.OptimizedContentKey.cbEncryptedKey <= SIZEOF( rgbEncryptedKeys ),
             DRM_E_BUFFERTOOSMALL );

    DRM_BYT_CopyBytes( rgbEncryptedKeys,
                       0,
                       f_pXmrLicense->containerOuter.containerKeys.OptimizedContentKey.pbEncryptedKeyBuffer,
                       f_pXmrLicense->containerOuter.containerKeys.OptimizedContentKey.iEncryptedKey,
                       f_pXmrLicense->containerOuter.containerKeys.OptimizedContentKey.cbEncryptedKey );

    if( f_pslk->cbKey == DRM_AES_KEYSIZE_128 )
    {
        ChkDR( _RebindSymmetricKey( f_pcontextBBX,
                                    f_pslk,
                                    rgbEncryptedKeys,
                                    cbEncryptedKeys,
                                    &hSigningKey ) );
    }
    else
    {
        ChkDR( _Legacy_RebindSymmetricKey( f_pcontextBBX,
                                           f_pslk,
                                           rgbEncryptedKeys,
                                           cbEncryptedKeys,
                                           SIZEOF( rgbEncryptedKeys ),
                                           rgbDecryptedKeys ) );

        pbSigningKey = rgbDecryptedKeys;
    }

    ChkDR( _UpdateXMRWithSymmKey( f_pcontextBBX,
                                  hSigningKey, 
                                  pbSigningKey, 
                                  rgbEncryptedKeys, 
                                  f_pXmrLicense ) );

ErrorExit:
    OEM_SECURE_ZERO_MEMORY( &rgbEncryptedKeys,  SIZEOF( rgbEncryptedKeys ) );
    OEM_SECURE_ZERO_MEMORY( &rgbDecryptedKeys, SIZEOF( rgbDecryptedKeys) );

    DRM_BBX_SafeReleaseKey( f_pcontextBBX, &hSigningKey );

    return dr;
}

/*********************************************************************
**
**  Function:  _RebindSLK
**
**  Synopsis:  Takes an SLK (ILA Session Key) encrypted with the device
**             private key and re-encrypts it to the corresponding symmetric key,
**             updating the appropriate members of the SLK structure.
**             f_cbEncSLK must be a comlplete ECC256 cipher block.
**
**  Arguments:
**     [f_pcontextBBX] : Pointer to an initialized blackbox context
**     [f_pbEncSLK]    : Pointer to the encrypted SLK data
**     [f_cbEncSLK]    : Length of f_pbEncSLK
**     [f_pslk]        : Returns the re-encrypted SLK and it's length
**
*********************************************************************/
static DRM_RESULT _RebindSLK(
    __inout_ecount( 1 )             DRM_BB_CONTEXT *f_pcontextBBX,
    __in_bcount( f_cbEncSLK ) const DRM_BYTE       *f_pbEncSLK,
    __in                            DRM_DWORD       f_cbEncSLK,
    __out_ecount( 1 )               DRM_SLK        *f_pslk )
{
    DRM_RESULT                  dr             = DRM_SUCCESS;
    OEM_CRYPTO_HANDLE           hPrivKey       = OEM_CRYPTO_HANDLE_INVALID;
    OEM_CRYPTO_HANDLE           hSlkKey        = OEM_CRYPTO_HANDLE_INVALID;
    DRM_DWORD                   cbWrappedKey   = DRM_AES_KEYSIZE_128;
    OEM_HAL_SLK_ENCRYPTION_TYPE eSlkCryptoType = OEM_HAL_SLK_ENCRYPTION_TYPE_ASYMMETRIC;

    ChkArg( f_pcontextBBX  != NULL );
    ChkArg( f_pbEncSLK     != NULL );
    ChkArg( f_pslk         != NULL );
    ChkArg( f_pcontextBBX->fInited );
    ChkArg( f_cbEncSLK     == ECC_P256_CIPHERTEXT_SIZE_IN_BYTES );

    ChkDR( _GetDeviceECC256KeyPair( f_pcontextBBX,
                                    NULL,
                                    NULL,
                                    &hPrivKey,
                                    DRM_BINARY_DEVICE_CERT_KEY_ENCRYPT ) );

    /*
    ** Unwrap the key using the asymmetric private key
    */
    ChkDR( _Hal_UnwrapKey( f_pcontextBBX,
                           OEM_HAL_KEY_TYPE_AES_SLK,
                           hPrivKey,
                           f_pbEncSLK,
                           f_cbEncSLK,
                           ( DRM_BYTE* )&eSlkCryptoType,
                           SIZEOF( eSlkCryptoType ),
                           f_pbEncSLK,
                           f_cbEncSLK,
                           &hSlkKey ) );

    /*
    ** Wrap it using a derived symmetric key
    */
    ChkDR( _Hal_WrapKey( f_pcontextBBX,
                         hSlkKey,
                         hPrivKey,
                         ( DRM_BYTE * )&f_pslk->rgbKey,
                         &cbWrappedKey ) );

    f_pslk->cbKey = cbWrappedKey;

ErrorExit:

    DRM_BBX_SafeReleaseKey( f_pcontextBBX, &hPrivKey );
    DRM_BBX_SafeReleaseKey( f_pcontextBBX, &hSlkKey );

    return dr;
}

/*********************************************************************
**
**  Function:  _SLK_RebindSLK
**
**  Synopsis:  Takes an SLK (ILA Session Key) encrypted with the device
**             private key and re-encrypts it to the corresponding symmetric key,
**             updating the appropriate members of the SLK structure.
**             Legacy or current crypto is inferred from encrypted key size.
**
**  Arguments:
**     [f_pcontextBBX] : Pointer to an initialized blackbox context
**     [f_pbEncSLK]    : Pointer to the encrypted SLK data
**     [f_cbEncSLK]    : Length of f_pbEncSLK
**     [f_pslk]        : Returns the re-encrypted SLK and it's length
**
*********************************************************************/
DRM_API DRM_RESULT DRM_CALL _SLK_RebindSLK(
    __inout_ecount( 1 )             DRM_BB_CONTEXT *f_pcontextBBX,
    __in_bcount( f_cbEncSLK ) const DRM_BYTE       *f_pbEncSLK,
    __in                            DRM_DWORD       f_cbEncSLK,
    __out_ecount( 1 )               DRM_SLK        *f_pslk )
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRM_PROFILING_ENTER_SCOPE(PERF_MOD_BLACKBOX, PERF_FUNC_DRM_BBX_RebindSLK);

    ChkBOOL( DRM_BBX_IsSlkCryptoSupported(), DRM_E_NOTIMPL );

    if( f_cbEncSLK == ECC_P256_CIPHERTEXT_SIZE_IN_BYTES )
    {
        ChkDR( _RebindSLK( f_pcontextBBX,
                           f_pbEncSLK,
                           f_cbEncSLK,
                           f_pslk ) );
    }
    else
    {
        ChkDR( _Legacy_RebindSLK( f_pcontextBBX,
                                  f_pbEncSLK,
                                  f_cbEncSLK,
                                  f_pslk ) );
    }

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}


/*********************************************************************
**
**  Function:  _SLK_RebindSymmetricKeyXML
**
**  Synopsis:  Takes an license which is symmetrically optimized to an SLK
**             and re-encrypts it to the device (or domain) symmetric key.
**             Legacy or current crypto is inferred from SLK size.
**             Reencrypts in place and verifies the content key signature,
**             using the specified Binding Info struct
**
**  Arguments:
**     [f_pcontextBBX] : Pointer to an initialized blackbox context
**     [f_pslk]        : Pointer to current SLK data
**     [f_pXmlBindInfo]: Pointer to XML Bind info structure containing the key to reencrypt,
**                       and signature data to verify.
**     [f_cbSymmKey]   : Length of the symmetrically encrypted key data.
**
*********************************************************************/
DRM_API DRM_RESULT DRM_CALL _SLK_RebindSymmetricKeyXML(
    __inout         DRM_BB_CONTEXT       *f_pcontextBBX,
    __in      const DRM_SLK              *f_pslk,
    __inout         DRM_XML_BINDING_INFO *f_pXmlBindInfo,
    __in            DRM_DWORD             f_cbSymmKey )
{
    DRM_RESULT dr    = DRM_SUCCESS;
    DRM_BYTE   bSize = 0;
    DRM_BYTE   rgbDecryptedKeys[__CB_DECL( DRM_ECC160_CIPHERTEXT_LEN )] = { 0 };

    ChkArg( f_pcontextBBX  != NULL );
    ChkArg( f_pXmlBindInfo != NULL );
    ChkArg( f_pslk         != NULL );
    ChkArg( f_cbSymmKey  <= SIZEOF( f_pXmlBindInfo->m_rgbContentKey ) );

    if( f_pslk->cbKey == DRM_AES_KEYSIZE_128 )
    {
        /*
        ** This codepath should never be encountered.
        */
        DRMASSERT( FALSE );

        ChkDR( DRM_E_NOTIMPL );
    }
    else
    {
        ChkDR( _Legacy_RebindSymmetricKey( f_pcontextBBX,
                                           f_pslk,
                                           f_pXmlBindInfo->m_rgbContentKey,
                                           f_cbSymmKey,
                                           SIZEOF( f_pXmlBindInfo->m_rgbContentKey ),
                                           rgbDecryptedKeys ) );
    }

    bSize = GET_BYTE( rgbDecryptedKeys, 0 );
    ChkBOOL( bSize <= SIZEOF( rgbDecryptedKeys ), DRM_E_CH_BAD_KEY );

    DRM_BYT_MoveBytes( rgbDecryptedKeys, 0, rgbDecryptedKeys, 1, SIZEOF( rgbDecryptedKeys ) - 1 );

    ChkDR( DRM_PK_Verify( f_pcontextBBX->CryptoContext.rgbCryptoContext,
                          eDRM_ECC_P160,
                          DRM_ECC160_PUBLIC_KEY_LEN,
                          ( const DRM_BYTE * )&f_pXmlBindInfo->m_oLsPublKey,
                          ( DRM_DWORD )bSize,
                          rgbDecryptedKeys,
                          DRM_ECC160_SIGNATURE_LEN,
                          f_pXmlBindInfo->m_rgbSignature ) );

ErrorExit:
    OEM_SECURE_ZERO_MEMORY( &rgbDecryptedKeys, SIZEOF( rgbDecryptedKeys) );
    return dr;
}


/*********************************************************************
**
**  Function:  _Legacy_RebindSymmetricKey
**
**  Synopsis:  Takes an license which is symmetrically optimized to an SLK
**             and re-encrypts it to the device (or domain) symmetric key.
**             Legacy or current crypto is inferred from SLK size.
**
**  Arguments:
**     [f_pcontextBBX]       : Pointer to an initialized blackbox context
**     [f_pslk]              : Pointer to current SLK data
**     [f_pbEncryptedKeys]   : Pointer to the symmetrically encrypted key data.
**                             Will be updated in place
**     [f_cbEncryptedKeys]   : Length of the symmetrically encrypted key data.
**     [f_cbKeyBuffer]       : Length of f_pbSymmKey buffer (must be larger than
**                              f_cbEncryptedKeys for the cases where the symmetric
**                              key becomes longer in this function).
**     [f_pbDecryptedKeys]   : Returns the decrypted key data (including CI key for XMR license)
**
*********************************************************************/
static DRM_RESULT _Legacy_RebindSymmetricKey(
    __inout                                DRM_BB_CONTEXT       *f_pcontextBBX,
    __in                             const DRM_SLK              *f_pslk,
    __inout_bcount( f_cbKeyBuffer )        DRM_BYTE             *f_pbEncryptedKeys,
    __in                                   DRM_DWORD             f_cbEncryptedKeys,
    __in                                   DRM_DWORD             f_cbKeyBuffer,
    __out_bcount( f_cbKeyBuffer )          DRM_BYTE             *f_pbDecryptedKeys )
{
    DRM_RESULT   dr               = DRM_SUCCESS;
    PRIVKEY_P256 privkey          = { 0 };
    DRM_DWORD    cbPrivkey        = SIZEOF( privkey );
    DRM_SLK      slkClear         = { 0 };

    ChkArg( f_pcontextBBX      != NULL );
    ChkArg( f_pslk             != NULL );
    ChkArg( f_pbEncryptedKeys  != NULL );
    ChkArg( f_cbEncryptedKeys  != 0    );
    ChkArg( f_cbKeyBuffer      != 0    );
    ChkArg( f_pcontextBBX->fInited     );
    ChkArg( f_pslk->cbKey      == DES_KEYSIZE );

    ChkDR( _GetDevicePrivkey( f_pcontextBBX, NULL, ( PRIVKEY * )&privkey ) );

    cbPrivkey = SIZEOF( PRIVKEY );

    /*
    ** First get the SLK key in the clear
    */
    ChkDR( DRM_BBX_Legacy_SymmetricCipher( SYMMETRIC_DECRYPT,
                                           ( DRM_BYTE * )&privkey,
                                           cbPrivkey,
                                           ( DRM_BYTE * )f_pslk,
                                           ( DRM_BYTE * )&slkClear,
                                           f_pslk->cbKey ) );

    /*
    ** Use the clear SLK key to decrypt the content key
    */
    ChkBOOL( f_cbEncryptedKeys <= f_cbKeyBuffer, DRM_E_BUFFERTOOSMALL );

    ChkDR( DRM_BBX_Legacy_SymmetricCipher( SYMMETRIC_DECRYPT,
                                           ( DRM_BYTE * )&slkClear,
                                           DES_KEYSIZE,
                                           f_pbEncryptedKeys,
                                           f_pbDecryptedKeys,
                                           f_cbEncryptedKeys ) );

    /*
    ** Use the device private key to symmetrically encrypt the clear content key
    */
    ChkDR( DRM_BBX_Legacy_SymmetricCipher( SYMMETRIC_ENCRYPT,
                                           ( DRM_BYTE * )&privkey,
                                           cbPrivkey,
                                           f_pbDecryptedKeys,
                                           f_pbEncryptedKeys,
                                           f_cbEncryptedKeys ) );

ErrorExit:
    OEM_ECC_ZeroPrivateKey( &privkey, cbPrivkey );
    OEM_SECURE_ZERO_MEMORY( &slkClear, SIZEOF( slkClear) );
    return dr;
}


/*********************************************************************
**
**  Function:  _Legacy_RebindSLK
**
**  Synopsis:  Takes an SLK (ILA Session Key) encrypted with the device
**             private key and re-encrypts it to the corresponding symmetric key,
**             updating the appropriate members of the SLK structure.
**             Legacy or current crypto is inferred from encrypted key size.
**
**  Arguments:
**     [f_pcontextBBX] : Pointer to an initialized blackbox context
**     [f_pbEncSLK]    : Pointer to the encrypted SLK data
**     [f_cbEncSLK]    : Length of f_pbEncSLK
**     [f_pslk]        : Returns the re-encrypted SLK and it's length
**
*********************************************************************/
static DRM_RESULT _Legacy_RebindSLK(
    __inout_ecount( 1 )             DRM_BB_CONTEXT *f_pcontextBBX,
    __in_bcount( f_cbEncSLK ) const DRM_BYTE       *f_pbEncSLK,
    __in                            DRM_DWORD       f_cbEncSLK,
    __out_ecount( 1 )               DRM_SLK        *f_pslk )
{
    DRM_RESULT   dr        = DRM_SUCCESS;
    PRIVKEY_P256 privkey   = { 0 };
    DRM_DWORD    cbPrivKey = SIZEOF( PRIVKEY_P256 );
    DRM_DWORD    cbslk     = ECC_P256_PLAINTEXT_SIZE_IN_BYTES;
    DRM_BYTE     rgbslk[ECC_P256_PLAINTEXT_SIZE_IN_BYTES] = { 0 }; /* Maximum size */

    ChkArg( f_pcontextBBX  != NULL );
    ChkArg( f_pbEncSLK     != NULL );
    ChkArg( f_pslk         != NULL );
    ChkArg( f_pcontextBBX->fInited );
    ChkArg( f_cbEncSLK     == DRM_ECC160_CIPHERTEXT_LEN );

    DRMCASSERT( DRM_ECC160_PLAINTEXT_LEN / 2 == DES_KEYSIZE );

    ChkDR( _GetDevicePrivkey( f_pcontextBBX, NULL, ( PRIVKEY * )&privkey ) );
    cbPrivKey = SIZEOF( PRIVKEY );

    ChkDR( DRM_PK_Decrypt( f_pcontextBBX->CryptoContext.rgbCryptoContext,
                            eDRM_ECC_P160,
                            SIZEOF( PRIVKEY ),
                            ( const DRM_BYTE * )&privkey,
                            f_cbEncSLK,
                            f_pbEncSLK,
                            &cbslk,
                            ( DRM_BYTE * )rgbslk ) );
    ChkDR( DRM_BBX_Legacy_SymmetricCipher( SYMMETRIC_ENCRYPT,
                                            ( DRM_BYTE * )&privkey,
                                            SIZEOF( PRIVKEY ),
                                            rgbslk,
                                            ( DRM_BYTE * )&f_pslk->rgbKey,
                                            DES_KEYSIZE ) );
    f_pslk->cbKey = DES_KEYSIZE;

ErrorExit:
    OEM_ECC_ZeroPrivateKey( &privkey, cbPrivKey );
    OEM_SECURE_ZERO_MEMORY( rgbslk, SIZEOF( rgbslk ) );
    return dr;
}


EXIT_PK_NAMESPACE_CODE;

