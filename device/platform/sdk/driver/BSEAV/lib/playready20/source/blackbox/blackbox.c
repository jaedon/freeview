/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmcommon.h>
#include <drmutilities.h>
#include <drmcrt.h>
#include <drmcontextsizes.h>
#include <oempkcrypto.h>
#include <oemcipher.h>
#include <oemsha1.h>
#include <drmmanager.h>
#include <drmblackbox.h>
#include <oemaes.h>
#include <oemshahmac.h>
#include <drmxmrbuilder.h>
#include <drmdomainapi.h>
#include <drmdomainkeyxmrparser.h>
#include <drmdomainstore.h>
#include <oem.h>
#include <drmkeyfile.h>
#include <oemkeyfile.h>
#include <drmbcertbuilder.h>
#include <drmbcertparser.h>
#include <drmderivedkey.h>
#include <oemkeyfile.h>
#include <drmwmdrm.h>
#include <oemeccp160.h>

ENTER_PK_NAMESPACE_CODE;

#define HASH_RESIDUAL_LEN       6


DRM_VOID compile_time_asserts( DRM_VOID )
{
    DRMCASSERT( SIZEOF( DRM_DECRYPT_CONTEXT ) == SIZEOF( DRM_CIPHER_CONTEXT ) );
}

static DRM_RESULT _GetDevicePrivkey(
    __inout_ecount( 1 )    DRM_BB_CONTEXT *f_pcontextBBX,
    __in_opt         const PUBKEY         *f_pDevPubKey,
    __out_ecount( 1 )      PRIVKEY        *f_pDevPrivKey );

static DRM_RESULT _GetDeviceECC256KeyPair(
    __in            DRM_BB_CONTEXT  *f_poBBXContext,
    __in_opt  const PUBKEY_P256     *f_poDevPubKeyIn,
    __out_opt       PUBKEY_P256     *f_poDevPubKey,
    __out           PRIVKEY_P256    *f_poDevPrivKey,
    __in            eDRM_BINARY_DEVICE_CERT_KEYUSAGE f_eDevKeyUsage );

static DRM_RESULT _DecryptContentKeyXML(
    __in_ecount( f_dwChainLen ) const DRM_BINDING_INFO    *f_pBindingInfo,
    __in                              DRM_DWORD            f_dwChainLen,
    __inout_ecount( 1 )               DRM_BB_CONTEXT      *f_pcontextBBX,
    __in                              DRM_BOOL             f_IgnoreChecksum,  /* Specifies whether to ignore checksum over content key */
    __in                              DRM_BOOL             f_IgnoreCKSig,     /* Specifies whether to ignore signature over content key */
    __in                              DRM_DWORD            f_cbKeyBuff,       /* the size of the key buffer */
    __out_bcount( f_cbKeyBuff )       DRM_BYTE            *f_pbKeyBuff,       /* the key buffer */
    __out_ecount( 1 )                 DRM_DWORD           *f_pcbKey,          /* the size of the key */
    __out_opt                         PRIVKEY             *f_pPrivKey );      /* return the private key that was used to decrypt the content key */

static DRM_RESULT _DecryptContentKeyXMR(
    __in_ecount( f_dwChainLen ) const DRM_BINDING_INFO    *f_pBindingInfo,
    __in                              DRM_DWORD            f_dwChainLen,
    __inout_ecount( 1 )               DRM_BB_CONTEXT      *f_pContextBBX,
    __in                              DRM_BOOL             f_fIgnoreChecksum,
    __in                              DRM_BOOL             f_fIncludeCIKey,   /* Specifies to include content key in the returned key data */
    __in                              DRM_DWORD            f_cbKeyBuff,       /* the size of the key buffer */
    __out_bcount( f_cbKeyBuff )       DRM_BYTE            *f_pbKeyBuff,       /* the key buffer */
    __out_ecount( 1 )                 DRM_DWORD           *f_pcbKey,          /* the size of the key */
    __out_ecount_opt( 1 )             PRIVKEY_P256        *f_pPrivKey );      /* returns the privkey used to decrypt */

static DRM_RESULT _VerifyChecksum(
    __in_bcount( f_cbKey ) const DRM_BYTE                *f_pbKey,
    __in                         DRM_DWORD                f_cbKey,
    __in_ecount( 1 )       const DRM_BB_CHECKSUM_CONTEXT *f_pChecksum,
    __inout_ecount( 1 )          DRM_BB_CONTEXT          *f_pcontextBBX );

static DRM_RESULT _RebindSymmetricKey(
    __inout                                DRM_BB_CONTEXT       *f_pcontextBBX,
    __in                             const DRM_SLK              *f_pslk,
    __inout_bcount( f_cbKeyBuffer )        DRM_BYTE             *f_pbEncryptedKeys,
    __in                                   DRM_DWORD             f_cbEncryptedKeys,
    __in                                   DRM_DWORD             f_cbKeyBuffer,
    __out_bcount( f_cbKeyBuffer )          DRM_BYTE             *f_pbDecryptedKeys );

static DRM_BYTE g_rgbMagicConstantLeft[ DRM_AES_KEYSIZE_128 ] = {
    0xf0, 0x61, 0x4d, 0xb6, 0x0f, 0xca, 0x3f, 0xbb,
    0x0f, 0x0d, 0x20, 0xf5, 0x61, 0xad, 0xec, 0x81 };

static DRM_BYTE g_rgbMagicConstantRight[ DRM_AES_KEYSIZE_128 ] = {
    0x3e, 0xf4, 0x88, 0xf3, 0x37, 0xe2, 0xda, 0xc6,
    0x62, 0x14, 0x9a, 0xdb, 0x82, 0x7e, 0x97, 0x6d };

static DRM_BYTE g_rgbMagicConstantZero[ DRM_AES_KEYSIZE_128 ] = {
    0x7e, 0xe9, 0xed, 0x4a, 0xf7, 0x73, 0x22, 0x4f,
    0x00, 0xb8, 0xea, 0x7e, 0xfb, 0x02, 0x7c, 0xbb };

static DRM_RESULT _CalculateKeys_Derived(
    __in_bcount( DRM_AES_KEYSIZE_128 )
         const DRM_BYTE              *f_pbRootContentKey,
    __in       DRM_XMR_AUX_KEY       *f_poAuxKey,
    __inout    DRM_XMR_UPLINKX       *f_poUplinkX );

static DRM_RESULT _DecryptContentKey_Derived(
    __inout_ecount( 1 )
                   DRM_BB_CONTEXT          *f_poBBXContext,
    __in_bcount( DRM_AES_KEYSIZE_128 )
             const DRM_BYTE                *f_pbRootContentKey,
    __in_bcount( DRM_AES_KEYSIZE_128 )
             const DRM_BYTE                *f_pbSecondaryKey,
    __in           DRM_XMR_UPLINKX         *f_poUplinkX,
    __in_opt const DRM_BB_CHECKSUM_CONTEXT *f_poChecksum,
    __out_bcount( DRM_AES_KEYSIZE_128 * 2 )
                   DRM_BYTE                *f_pbKey );

static DRM_RESULT _ValidateBindingInfo_Derived(
    __in_ecount( f_dwChainLen )
          const DRM_BINDING_INFO *f_poBindingInfo,
    __in        DRM_DWORD         f_dwChainLen,
    __out       DRM_BOOL         *f_pfDerived );

static DRM_RESULT _UnshuffleContentKey(
    __inout_bcount( DRM_AES_KEYSIZE_128 * 2 )
        DRM_BYTE *f_pbContentKey );

#if DBG
static DRM_RESULT DRM_CALL _TestDeviceKeyPair( DRM_BB_CONTEXT *f_pcontextBBX  )
{
    DRM_RESULT  dr          = DRM_SUCCESS;
    PRIVKEY     privkey     = { 0 };
    DRM_BYTE   *pbIn        = NULL;
    DRM_BYTE   *pbOut       = NULL;
    DRM_BYTE   *pbEncrypted = NULL;
    DRM_DWORD   cbEncrypted = 0;
    DRM_DWORD   cbClear     = 0;

    /* Some basic assumptions about the environment */
    DRMCASSERT( DRM_ECC160_PLAINTEXT_LEN  <= SIZEOF( f_pcontextBBX->CryptoContext.pubKey2 ) );
    DRMCASSERT( DRM_ECC160_CIPHERTEXT_LEN <= SIZEOF( f_pcontextBBX->CryptoContext.union_cert.cert ) );

    ChkDR(_GetDevicePrivkey(f_pcontextBBX, NULL, &privkey));

    pbIn = ( DRM_BYTE * )&( f_pcontextBBX->CryptoContext.pubKey2 );
    MEMSET(pbIn, 'a', SIZEOF( PUBKEY ) );

    pbEncrypted = (DRM_BYTE *)&( f_pcontextBBX->CryptoContext.union_cert.cert );
    cbEncrypted = SIZEOF( f_pcontextBBX->CryptoContext.union_cert.cert );
    pbOut       = (DRM_BYTE *)&( f_pcontextBBX->CryptoContext.signature );
    cbClear     = min ( SIZEOF( f_pcontextBBX->CryptoContext.pubKey2 ),
                        SIZEOF( f_pcontextBBX->CryptoContext.signature ) );

    ChkDR( DRM_PK_Encrypt( NULL,
                           f_pcontextBBX->CryptoContext.rgbCryptoContext,
                           eDRM_ECC_P160,
                           DRM_ECC160_PUBLIC_KEY_LEN,
                           ( DRM_BYTE * )&f_pcontextBBX->cachedCertValues.pubkeyCurrent,
                           cbClear,
                           pbIn,
                           &cbEncrypted,
                           pbEncrypted ) );

    ChkDR( DRM_PK_Decrypt( f_pcontextBBX->CryptoContext.rgbCryptoContext,
                           eDRM_ECC_P160,
                           DRM_ECC160_PRIVATE_KEY_LEN,
                           ( const DRM_BYTE * )&privkey,
                           cbEncrypted,
                           pbEncrypted,
                           &cbClear,
                           pbOut ) );

    if (MEMCMP(pbIn, pbOut, cbClear) !=0)
    {
        ChkDR ( DRM_E_KEY_MISMATCH );
    }

ErrorExit:
    OEM_ECC_ZeroPrivateKey_P160( &privkey );
    return dr;
}
#endif /* DBG */

DRM_API DRM_RESULT DRM_CALL DRM_BBX_Initialize(
    __in_opt            DRM_VOID           *f_pOEMContext,
    __inout_ecount( 1 ) DRM_BB_CONTEXT     *f_pcontextBBX )
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRM_PROFILING_ENTER_SCOPE(PERF_MOD_BLACKBOX, PERF_FUNC_DRM_BBX_Initialize);

    ChkArg( f_pcontextBBX != NULL );

    ZEROMEM( &f_pcontextBBX->CryptoContext, SIZEOF( DRM_CRYPTO_CONTEXT ) );

    f_pcontextBBX->pOEMContext = f_pOEMContext;

    f_pcontextBBX->pDomainStoreContext = NULL;

#if DBG
    if( DRM_WMDRM_IsWMDRMSupported() )
    {
        f_pcontextBBX->fInited = FALSE;
        ChkDR(_TestDeviceKeyPair(f_pcontextBBX));
    }
#endif

    f_pcontextBBX->fInited = TRUE;

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

DRM_API DRM_RESULT DRM_CALL DRM_BBX_Shutdown( __inout_ecount( 1 ) DRM_BB_CONTEXT     *f_pcontextBBX )
{
    f_pcontextBBX->fInited = TRUE;
    return DRM_SUCCESS;
}

DRM_API DRM_RESULT DRM_CALL DRM_BBX_GetClientId(
    __out_ecount( 1 )    CLIENTID          *f_pClientId,
    __inout_ecount( 1 )  DRM_BB_CONTEXT    *f_pcontextBBX )
{
    DRM_RESULT     dr          = DRM_SUCCESS;
    RC4_KEYSTRUCT *prc4KS      = NULL;
    DRM_DWORD      cbEncrypted = DRM_ECC160_CIPHERTEXT_LEN;
    DRM_BYTE       rgbRandBuf[__CB_DECL(DRM_ECC160_PLAINTEXT_LEN)] = {0};

    ChkArg( f_pClientId   != NULL
         && f_pcontextBBX != NULL
         && f_pcontextBBX->fInited );

    ChkBOOL( DRM_WMDRM_IsWMDRMSupported(), DRM_E_NOTIMPL );

    MEMCPY( f_pClientId->version, CLIENT_ID_VER, VERSION_LEN );
    MEMCPY( &(f_pClientId->pk), &(f_pcontextBBX->cachedCertValues.m_BBCompatibilityCert), SIZEOF( PKCERT ) );

    ChkDR( Oem_Random_GetBytes( f_pcontextBBX->pOEMContext, rgbRandBuf, SIZEOF( rgbRandBuf ) ) );
    ChkDR( DRM_PK_Encrypt( NULL,
                           f_pcontextBBX->CryptoContext.rgbCryptoContext,
                           eDRM_ECC_P160,
                           DRM_ECC160_PUBLIC_KEY_LEN,
                           ( DRM_BYTE * )&g_pubkeyLicenseServer,
                           DRM_ECC160_PLAINTEXT_LEN,
                           rgbRandBuf,
                           &cbEncrypted,
                           f_pClientId->randNum ) );

    prc4KS = (RC4_KEYSTRUCT*)f_pcontextBBX->CryptoContext.rgbCryptoContext;
    /* Encrypt the other data with RC4 using the random number as the key     */
    DRM_RC4_KeySetup( prc4KS, DRMCIPHERKEYLEN_RC4,    rgbRandBuf );
    DRM_RC4_Cipher(   prc4KS, SIZEOF(f_pClientId->pk), (DRM_BYTE *)&(f_pClientId->pk) );

ErrorExit:
    return dr;
}


/*
    Test whether or not a key is valid and can be used to decrypt
    content.
*/
DRM_API DRM_BOOL DRM_CALL DRM_BBX_CanBind(
    __in_ecount( f_dwChainLength ) const DRM_BINDING_INFO *f_pBindingInfo,
    __in                                 DRM_DWORD         f_dwChainLength,
    __inout_ecount( 1 )                  DRM_BB_CONTEXT   *f_pcontextBBX,
    __out_opt                            DRM_RESULT       *f_pDr )
{
    DRM_BYTE    rgbKey [__CB_DECL( DRM_AES_KEYSIZE_128 )] = { 0 };   /* 16 BYTES */
    DRM_DWORD   f_cbKey = 0;
    DRM_RESULT  dr      = DRM_SUCCESS;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_BLACKBOX, PERF_FUNC_DRM_BBX_CanBind );

    ChkArg( f_pBindingInfo != NULL );

    if ( f_pBindingInfo[0].m_fXMRLicense )
    {
        dr = _DecryptContentKeyXMR( f_pBindingInfo,
                                    f_dwChainLength,
                                    f_pcontextBBX,
                                    TRUE,
                                    FALSE,
                                    DRM_AES_KEYSIZE_128,
                                    rgbKey,
                                   &f_cbKey,
                                    NULL );

    }
    else
    {
        dr = _DecryptContentKeyXML( f_pBindingInfo,
                                    f_dwChainLength,
                                    f_pcontextBBX,
                                    TRUE, /* Ignore CK checksum on CanBind */
                                    TRUE, /* Ignore CK signature on CanBind */
                                    DRM_AES_KEYSIZE_128,
                                    rgbKey,
                                   &f_cbKey,
                                    NULL );
    }
    if ( f_pDr != NULL )
    {
        *f_pDr = dr;
    }

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return DRM_SUCCEEDED( dr );
}

/*********************************************************************
**
** Function: DRM_BBX_SymmetricBind
**
** Synopsis: Function that does symmetric binding of the passed in data
**           to the current blackbox - using a symmetric key derived from
**           the device private key.
**           If f_fUseLegacyCrypto is TRUE, will use the ECC-160 priv key,
**           otherwise will use the ECC-256 priv key.
**
** Arguments:
** [f_poBBXContext]     : Specifies a pre-initialized blackbox context
** [f_fEncrypt]         : Specifies that encryption should be done instead of decryption
**                        (ignored for RC4)
** [f_fUseLegacyCrypto] : Specifies whether to use legacy crypto algorithms and keys
**                        (SHA-1, RC4), or new ones (SHA-256, AES-ECB)
** [f_pbDataIn]         : Pointer to a buffer of data to be
**                        ciphered.
** [f_pbDataOut]        : Pointer to a buffer that the result of
**                        the ciphering is stored. If NULL, ciphering
**                        will be done in place.
** [f_cbData]           : Size (number of bytes) of both the input
**                        and output buffers. The size of the input
**                        buffer and output buffer is the same
**                        since symmetric ciphering is used.
**
** Returns:
**      DRM_SUCCESS
**          Success.
**      DRM_E_INVALIDARG
**          One of the arguments is invalid or improperly initialized
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_BBX_SymmetricBind(
    __inout_ecount( 1 )          DRM_BB_CONTEXT *f_poBBXContext,
    __in                         DRM_BOOL        f_fEncrypt,
    __in                         DRM_BOOL        f_fUseLegacyCrypto,
    __in_bcount( f_cbData )      DRM_BYTE       *f_pbDataIn,
    __out_bcount_opt( f_cbData ) DRM_BYTE       *f_pbDataOut,
    __in                         DRM_DWORD       f_cbData )
{
    DRM_RESULT dr    = DRM_SUCCESS;
    DRM_DWORD  cbKey = SIZEOF( PRIVKEY_P256 );
    DRM_BYTE   rgbKey[SIZEOF( PRIVKEY_P256 )] = { 0 };

    ChkArg( f_poBBXContext != NULL );
    ChkArg( f_poBBXContext->fInited );

    if ( f_fUseLegacyCrypto )
    {
        ChkDR( _GetDevicePrivkey( f_poBBXContext, NULL, ( PRIVKEY * )rgbKey ) );
        cbKey = SIZEOF( PRIVKEY );
    }
    else
    {
        ChkDR( _GetDeviceECC256KeyPair( f_poBBXContext,
                                        NULL,
                                        NULL,
                                        ( PRIVKEY_P256 * )rgbKey,
                                        DRM_BINARY_DEVICE_CERT_KEY_ENCRYPT ) );
    }

    ChkDR( DRM_BBX_SymmetricCipher( f_fEncrypt,
                                    f_fUseLegacyCrypto,
                                    rgbKey,
                                    cbKey,
                                    f_pbDataIn,
                                    f_pbDataOut,
                                    f_cbData ) );

ErrorExit:
    OEM_ECC_ZeroPrivateKey( rgbKey, cbKey );
    return dr;
}

/*********************************************************************
**
** Function: DRM_BBX_SymmetricCipher
**
** Synopsis: Function that does symmetric ciphering (encrypt or decrypt)
**           of the passed in data using a symmetric key, which
**           is a key derived from a private key.
**
**           The private key is hashed using either SHA-1 or SHA-256,
**           and the hash is used as an RC4 or AES key to cipher the
**           data.
**
** Arguments:
** [f_fEncrypt]         : Specifies that encryption should be done instead of decryption
**                        (ignored for RC4)
** [f_fUseLegacyCrypto] : Specifies whether to use legacy crypto algorithms and keys
**                        (SHA-1, RC4), or new ones (SHA-256, AES-ECB)
** [f_pbKey]            : Specifies a key to hash to the symmetric key.
** [f_cbKey]            : The length of f_pbKey
** [f_pbDataIn]         : Pointer to a buffer of data to be
**                        ciphered.
** [f_pbDataOut]        : Pointer to a buffer that the result of
**                        the ciphering is stored. If NULL, ciphering
**                        will be done in place.
** [f_cbData]           : Size (number of bytes) of both the input
**                        and output buffers. The size of the input
**                        buffer and output buffer is the same
**                        since symmetric ciphering is used.
**
** Returns:
**      DRM_SUCCESS
**          Success.
**      DRM_E_INVALIDARG
**          One of the arguments is invalid or improperly initialized
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_BBX_SymmetricCipher(
    __in                                DRM_BOOL  f_fEncrypt,
    __in                                DRM_BOOL  f_fUseLegacyCrypto,
    __in_bcount_opt( f_cbKey )   const  DRM_BYTE *f_pbKey,
    __in                                DRM_DWORD f_cbKey,
    __in_bcount( f_cbData )             DRM_BYTE *f_pbDataIn,
    __out_bcount_opt( f_cbData )        DRM_BYTE *f_pbDataOut,
    __in                                DRM_DWORD f_cbData )
{
    DRM_RESULT                  dr                         = DRM_SUCCESS;
    DRM_SHA_CONTEXT             shaContext                 = DRM_SHA_CONTEXT_DEFAULT;
    DRM_BYTE                    rgbDigest[__CB_DECL( DRM_SHA_DIGEST_LEN_MAX )] = { 0 };
    DRM_BYTE                    rgbOutputKeyUnaligned[ DRM_AES_BLOCKLEN + SIZEOF(DRM_DWORD) ] = {0};
    DRM_BYTE                   *rgbOutputKeyAligned        = NULL;
    DRM_STACK_ALLOCATOR_CONTEXT stkContext                 = {0};
    DRM_AES_KEY                 aesKey                     = {0};

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_BLACKBOX, PERF_FUNC_DRM_BBX_SymmetricCipher );

    ChkArg( f_pbDataIn  != NULL );
    ChkArg( f_pbKey     != NULL );
    ChkArg( f_cbData > 0 );
    ChkArg( f_cbKey > 0 );

    if ( f_pbDataOut != NULL )
    {
        MEMCPY( f_pbDataOut, f_pbDataIn, f_cbData );
    }
    else
    {
        f_pbDataOut = f_pbDataIn;
    }

    if ( f_fUseLegacyCrypto )
    {
        /*
        **  1. Get a hash of the privkey
        **  2. Generate a RC4 key using this hash
        **  3. Decrypt ciphertext using this RC4 key
        */
        RC4_KEYSTRUCT rc4Key = { 0 };

        ChkDR( DRM_SHA_Init( &shaContext, eDRM_SHA_1 ) );
        ChkDR( DRM_SHA_Update( f_pbKey, f_cbKey, &shaContext ) );
        ChkDR( DRM_SHA_Finalize( &shaContext, DRM_SHA1_DIGEST_LEN, rgbDigest ) );

        DRM_RC4_KeySetup( &rc4Key, DRM_SHA1_DIGEST_LEN, rgbDigest );
        DRM_RC4_Cipher( &rc4Key, f_cbData, f_pbDataOut );

        DRM_RC4_ZeroKey( &rc4Key );
    }
    else
    {

        /*
        **  1. Get an OMAC1 of the privkey
        **  2. Generate an AES key using this hash
        **  3. Decrypt or encrypt ciphertext using this AES key
        */
        DRM_BYTE    rgbConstantBlock[DRM_AES_BLOCKLEN] = {0};

        /*
        ** rgbOutputKeyAligned will be cast to a DWORD aligned
        ** structure in Oem_Aes_SetKey so make sure rgbOutputKeyAligned
        ** is DWORD aligned.
        */
        ChkDR( DRM_STK_Init( &stkContext,
                             rgbOutputKeyUnaligned,
                             SIZEOF( rgbOutputKeyUnaligned ) ) );

        ChkDR( DRM_STK_Alloc_Aligned( &stkContext,
                                       DRM_AES_BLOCKLEN,
                                       SIZEOF( DRM_DWORD ),
                                       NULL,
                                       ( DRM_VOID **) &rgbOutputKeyAligned ) );

        ChkBOOL( f_cbKey >= DRM_AES_KEYSIZE_128, DRM_E_CH_BAD_KEY );
        ChkDR( Oem_Aes_SetKey( f_pbKey, &aesKey ) );
        ChkDR( Oem_Omac1_Sign(&aesKey, rgbConstantBlock, 0, SIZEOF(rgbConstantBlock), rgbOutputKeyAligned ) );
        ChkDR( Oem_Aes_ZeroKey(&aesKey) );

        ChkDR( Oem_Aes_SetKey( rgbOutputKeyAligned, &aesKey ) );

        if ( f_fEncrypt )
        {
            ChkDR( Oem_Aes_EcbEncryptData( &aesKey, f_pbDataOut, f_cbData ) );
        }
        else
        {
            ChkDR( Oem_Aes_EcbDecryptData( &aesKey, f_pbDataOut, f_cbData ) );
        }
    }

ErrorExit:
    OEM_SECURE_ZERO_MEMORY( &shaContext, SIZEOF( DRM_SHA_CONTEXT ) );
    OEM_SECURE_ZERO_MEMORY( rgbDigest, DRM_SHA_DIGEST_LEN_MAX );
    (DRM_VOID)Oem_Aes_ZeroKey( &aesKey );
    OEM_SECURE_ZERO_MEMORY( rgbOutputKeyUnaligned, SIZEOF( rgbOutputKeyUnaligned ) );
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}

/*********************************************************************
**
** Function: DRM_BBX_EncryptDomainPrivateKey
**
** Synopsis:
**           Encrypt the passed in clear domain private key using the
**           device's symmetric key.
**
** Arguments:
**
** [f_poBBXContext]         -- Pointer to a DRM_BB_CONTEXT context.
** [f_pszBase]              -- Pointer to a DRM_CHAR buffer. Part of it contains
**                             the domain private key specified by a substring
**                             named f_pdasstrInputPrivKey.
** [f_pdasstrInputPrivKey]  -- Pointer to a DRM substring that defines the
**                             domain private key in the buffer pointed by f_pszBase.
**                             The data defined by f_pdasstrInputPrivKey will
**                             be B64 decoded in place so the content defined by
**                             f_pdasstrInputPrivKey will change after the function
**                             call.
** [f_poOutputPrivKey]      -- Pointer to a buffer that receives an encrypted
**                             domain private key using device's symmetric key.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_BBX_EncryptDomainPrivateKey(
    __inout_ecount( 1 )         DRM_BB_CONTEXT  *f_poBBXContext,
    __inout_bcount( f_pdasstrInputPrivKey->m_ich + f_pdasstrInputPrivKey->m_cch ) DRM_CHAR *f_pszBase,
    __in                const   DRM_SUBSTRING   *f_pdasstrInputPrivKey,
    __out_ecount( 1 )           PRIVKEY_P256    *f_poOutputPrivKey )
{
    DRM_RESULT   dr     = DRM_SUCCESS;
    DRM_DWORD    cbSize = 0;
    PRIVKEY_P256 oClearDomainPrivKey = { 0 };

    ChkBOOL( DRM_DOMAIN_IsDomainSupported(), DRM_E_NOTIMPL );
    ChkArg( f_poBBXContext != NULL );
    ChkArg( f_pszBase != NULL );
    ChkArg( f_pdasstrInputPrivKey != NULL );
    ChkArg( f_poOutputPrivKey != NULL );

    cbSize = SIZEOF( PRIVKEY_P256 );

    PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_USE_WIDE_API, "Ignore prefast warning about ANSI functions.")

    ChkDR( DRM_B64_DecodeA( f_pszBase,
                            f_pdasstrInputPrivKey,
                            &cbSize,
                            NULL,
                            DRM_BASE64_DECODE_IN_PLACE ) );
    PREFAST_POP

  /*
    ** The size of the B64 decoded data should be the size of an
    ** ECC-256 private key.
    */
    DRMASSERT( cbSize == SIZEOF( PRIVKEY_P256 ) );

    DRM_BYT_MoveBytes( ( DRM_BYTE * )&oClearDomainPrivKey,
                       0,
                       f_pszBase,
                       f_pdasstrInputPrivKey->m_ich,
                       SIZEOF( PRIVKEY_P256 ) );

    ChkDR( DRM_BBX_SymmetricBind( f_poBBXContext,
                                  TRUE,
                                  FALSE,
                                  ( DRM_BYTE * )&oClearDomainPrivKey,
                                  ( DRM_BYTE * )f_poOutputPrivKey,
                                   SIZEOF( PRIVKEY_P256 ) ) );

    /*
    ** Fill the passed in domain private key with valid XML characters
    ** to prevent XML parsing error.
    */
    DRM_BYT_SetBytes( f_pszBase,
                      f_pdasstrInputPrivKey->m_ich,
                      f_pdasstrInputPrivKey->m_cch,
                      ' ' );

ErrorExit:
    OEM_ECC_ZeroPrivateKey_P256( &oClearDomainPrivKey );
    return dr;
}

/*  Decrypt a content key and place it in the given key buffer. */
static DRM_RESULT _DecryptContentKeyXML(
    __in_ecount( f_dwChainLen ) const DRM_BINDING_INFO    *f_pBindingInfo,
    __in                              DRM_DWORD            f_dwChainLen,
    __inout_ecount( 1 )               DRM_BB_CONTEXT      *f_pcontextBBX,
    __in                              DRM_BOOL             f_IgnoreChecksum,  /* Specifies whether to ignore checksum over content key */
    __in                              DRM_BOOL             f_IgnoreCKSig,     /* Specifies whether to ignore signature over content key */
    __in                              DRM_DWORD            f_cbKeyBuff,       /* the size of the key buffer */
    __out_bcount( f_cbKeyBuff )       DRM_BYTE            *f_pbKeyBuff,       /* the key buffer */
    __out_ecount( 1 )                 DRM_DWORD           *f_pcbKey,          /* the size of the key */
    __out_opt                         PRIVKEY             *f_pPrivKey )       /* return the private key that was used to decrypt the content key */
{
    DRM_RESULT dr       = DRM_SUCCESS;
    PRIVKEY    privkey  = { 0 };
    DRM_BYTE   rgbDecContKey [__CB_DECL( DRM_ECC160_PLAINTEXT_LEN )] = { 0 };
    DRM_BYTE   bSize    = 0;
    const DRM_XML_BINDING_INFO *pXmlBindingInfo = NULL;

    DRM_PROFILING_ENTER_SCOPE(PERF_MOD_BLACKBOX, PERF_FUNC__DecryptContentKeyXML);

    ChkArg( f_pcbKey != NULL );

    *f_pcbKey = 0;

    ChkArg( f_pcontextBBX  != NULL
         && f_pBindingInfo != NULL
         && f_pbKeyBuff    != NULL
         && f_dwChainLen    > 0
         && f_pcontextBBX->fInited
         && !f_pBindingInfo[f_dwChainLen-1].m_fXMRLicense );

    pXmlBindingInfo = &( f_pBindingInfo[f_dwChainLen-1].m_BindingInfo.m_XML );

    /*
    **  Find the private key that corresponds to
    **  the public key used to encrypt the content key.
    **  The last value in the chain *must* be the key encrypted to the machine key.  It *must*
    **  be of type eMSDRM_PK or eMSDRM_SYM
    */
    ChkBOOL( pXmlBindingInfo->m_dwAlgorithm == eMSDRM_PK
          || pXmlBindingInfo->m_dwAlgorithm == eMSDRM_SYM, DRM_E_UNSUPPORTEDALGORITHM );

    dr = _GetDevicePrivkey( f_pcontextBBX, &( pXmlBindingInfo->m_oPublKey ), &privkey );
    if ( dr == DRM_E_KEYFILE_PRIVATE_KEY_NOT_FOUND )
    {
        ChkDR( DRM_E_UNKNOWN_BINDING_KEY );
    }
    ChkDR( dr );


    if ( f_pBindingInfo[0].m_BindingInfo.m_XML.m_dwAlgorithm == eMSDRM_SYM )
    {
        /* Even if this is a license chain, if the leaf license is symmetrically optimized
           we can directly decrypt it */
        f_dwChainLen = 1;
        pXmlBindingInfo = &( f_pBindingInfo[0].m_BindingInfo.m_XML );
    }

    /*
    ** Decrypt the content key with that private key.
    */
    if ( pXmlBindingInfo->m_dwAlgorithm == eMSDRM_PK )
    {
        *f_pcbKey = DRM_ECC160_PLAINTEXT_LEN;
        ChkDR( DRM_PK_Decrypt( f_pcontextBBX->CryptoContext.rgbCryptoContext,
                               eDRM_ECC_P160,
                               DRM_ECC160_PRIVATE_KEY_LEN,
                               ( const DRM_BYTE * )&privkey,
                               DRM_ECC160_CIPHERTEXT_LEN,
                               pXmlBindingInfo->m_rgbContentKey,
                               f_pcbKey,
                               rgbDecContKey ) );
    }
    else
    {
        /*
        ** Also, just decrypt the entire buffer, even if we only want a small portion of it.
        ** We won't know how large the key is until we finish the call and can look at the
        ** first byte.
        */
        DRMCASSERT( SIZEOF( pXmlBindingInfo->m_rgbContentKey ) >= SIZEOF( rgbDecContKey ) );
        ChkDR( DRM_BBX_SymmetricCipher( SYMMETRIC_DECRYPT,
                                        TRUE,
                                       ( DRM_BYTE * )&privkey,
                                       SIZEOF( privkey ),
                                       ( DRM_BYTE * )pXmlBindingInfo->m_rgbContentKey,
                                       rgbDecContKey,
                                       SIZEOF( rgbDecContKey ) ) );
    }

    bSize = GET_BYTE( rgbDecContKey, 0 );
    /* the size byte should have a value of at most 15, 16 is also ok in this source - we don't run over buffers that way as well, failure should come from some signature mismatch later */
    ChkBOOL( bSize <= SIZEOF( rgbDecContKey ), DRM_E_CH_BAD_KEY );
    ChkBOOL( bSize != 0, DRM_E_CH_BAD_KEY );

    DRM_BYT_MoveBytes( rgbDecContKey, 0, rgbDecContKey, 1, SIZEOF( rgbDecContKey ) - 1);

    if ( !f_IgnoreChecksum )
    {
        ChkDR( _VerifyChecksum( rgbDecContKey,
                                bSize,
                               &f_pBindingInfo[f_dwChainLen-1].m_oChecksum,
                                f_pcontextBBX ) );
    }

    /*
    ** Verify the key signature.
    */
    if ( pXmlBindingInfo->m_dwAlgorithm == eMSDRM_PK
      && !f_IgnoreCKSig )
    {
        ChkDR( DRM_PK_Verify( f_pcontextBBX->CryptoContext.rgbCryptoContext,
                              eDRM_ECC_P160,
                              DRM_ECC160_PUBLIC_KEY_LEN,
                              ( const DRM_BYTE * )&pXmlBindingInfo->m_oLsPublKey,
                              ( DRM_DWORD ) bSize,
                              rgbDecContKey,
                              DRM_ECC160_SIGNATURE_LEN,
                              pXmlBindingInfo->m_rgbSignature ) );
    }

    for ( f_dwChainLen--; f_dwChainLen > 0; f_dwChainLen-- )
    {
        pXmlBindingInfo = &f_pBindingInfo[f_dwChainLen-1].m_BindingInfo.m_XML;

        /*
        ** We are now in chained licenses enabling bits.  The algorithm *must* be eMSDRM_CK
        */
        ChkBOOL( pXmlBindingInfo->m_dwAlgorithm == eMSDRM_CK
              && bSize == DES_KEYSIZE,
                 DRM_E_UNSUPPORTEDALGORITHM );

        /*
        ** Use the current content key we extracted as a DES key and decrypt the next section
        */
        DRM_DES_KeySetup( ( DESTable * )( f_pcontextBBX->CryptoContext.rgbCryptoContext ), rgbDecContKey );

        DRM_DES_Decrypt( pXmlBindingInfo->m_rgbContentKey,
                         rgbDecContKey, /* output buffer */
                        ( DESTable * )( f_pcontextBBX->CryptoContext.rgbCryptoContext ) );
        DRM_DES_Decrypt( ( pXmlBindingInfo->m_rgbContentKey ) + __CB_DECL( DES_BLOCKLEN ),
                        rgbDecContKey + __CB_DECL( DES_BLOCKLEN ),
                        ( DESTable * )( f_pcontextBBX->CryptoContext.rgbCryptoContext ) );

        bSize = GET_BYTE( rgbDecContKey, 0 );
        ChkBOOL( bSize <= SIZEOF( rgbDecContKey ), DRM_E_CH_BAD_KEY );

        DRM_BYT_MoveBytes( rgbDecContKey, 0, rgbDecContKey, 1, SIZEOF( rgbDecContKey ) - 1 );

        if ( !f_IgnoreChecksum )
        {
            ChkDR( _VerifyChecksum( rgbDecContKey,
                                    bSize,
                                   &f_pBindingInfo[f_dwChainLen-1].m_oChecksum,
                                    f_pcontextBBX ) );
        }

        /*
        ** Verify the key signature.
        */
        if ( !f_IgnoreCKSig )
        {
            ChkDR( DRM_PK_Verify( f_pcontextBBX->CryptoContext.rgbCryptoContext,
                                                   eDRM_ECC_P160,
                                                   DRM_ECC160_PUBLIC_KEY_LEN,
                                                   ( const DRM_BYTE * )&pXmlBindingInfo->m_oLsPublKey,
                                                   ( DRM_DWORD ) bSize,
                                                   rgbDecContKey,
                                                   DRM_ECC160_SIGNATURE_LEN,
                                                   pXmlBindingInfo->m_rgbSignature ) );
        }
   } /* end-for chainlen */

    *f_pcbKey = ( DRM_DWORD )bSize;

    ChkBOOL( f_cbKeyBuff >= *f_pcbKey, DRM_E_BUFFERTOOSMALL );

    /*
    ** Everything passed.  Return the key.
    */
    MEMCPY( f_pbKeyBuff, rgbDecContKey, *f_pcbKey );
    if ( f_pPrivKey != NULL )
    {
        MEMCPY( f_pPrivKey, &privkey, SIZEOF( privkey ) );
    }

ErrorExit:
    OEM_ECC_ZeroPrivateKey_P160( &privkey );
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}

/*  Verify the checksum on a content key. */
static DRM_RESULT _VerifyChecksum(
    __in_bcount( f_cbKey ) const DRM_BYTE                *f_pbKey,
    __in                         DRM_DWORD                f_cbKey,
    __in_ecount( 1 )       const DRM_BB_CHECKSUM_CONTEXT *f_pChecksum,
    __inout_ecount( 1 )          DRM_BB_CONTEXT          *f_pcontextBBX )
{
    DRM_RESULT dr     = DRM_SUCCESS;
    DRM_BYTE   rgbTemp[__CB_DECL( max( LEGACY_KID_B64ENC_LEN, DRM_SHA1_DIGEST_LEN + 1 ) )] = { 0 };
    DRM_AES_KEY aesKey = {0};

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_BLACKBOX, PERF_FUNC_VerifyChecksum );

    ChkArg( f_pChecksum   != NULL
         && f_pcontextBBX != NULL );

    ChkBOOL( f_pbKey != NULL && f_cbKey != 0, DRM_E_CH_BAD_KEY );

    ZEROMEM( rgbTemp, SIZEOF( rgbTemp ) );

    if ( f_pChecksum->m_fIgnoreChecksum )
    {
        goto ErrorExit;
    }

    if ( f_pChecksum->m_pbV1KID != NULL )
    {
        DRM_CIPHER_CONTEXT *pCipherContext = ( DRM_CIPHER_CONTEXT * )( f_pcontextBBX->CryptoContext.rgbCryptoContext );

        ChkDR( DRM_BBX_CPHR_Init( pCipherContext, eDRM_RC4_CIPHER, f_cbKey, f_pbKey ) );

        /*
        ** Verify the secret data:
        ** The secret is the encrypted blob of the base64 encoded KID with the content key
        */
        ChkBOOL( f_pChecksum->m_cbChecksum >= LEGACY_KID_B64ENC_LEN, DRM_E_CH_BAD_KEY );

        DRM_BYT_CopyBytes( rgbTemp,
                           0,
                           f_pChecksum->m_rgbChecksumData,
                           LEGACY_KID_B64ENC_LEN - DRM_CPHR_CB_FOR_INITDECRYPT,
                           DRM_CPHR_CB_FOR_INITDECRYPT );

        ChkDR( DRM_CPHR_InitDecrypt( pCipherContext,
                                     rgbTemp,
                                     LEGACY_KID_B64ENC_LEN ) );

        MEMCPY( rgbTemp, f_pChecksum->m_rgbChecksumData, LEGACY_KID_B64ENC_LEN );
        ChkDR( DRM_CPHR_Decrypt( pCipherContext, NULL, LEGACY_KID_B64ENC_LEN, rgbTemp ) );

        ChkBOOL( MEMCMP( f_pChecksum->m_pbV1KID, rgbTemp, LEGACY_KID_B64ENC_LEN ) == 0, DRM_E_CH_BAD_KEY );
    }
    else if( f_pChecksum->m_cbChecksum == DRM_AES_CK_CHECKSUM_LENGTH )
    {
        DRM_BYTE rgbBlock[SIZEOF( DRM_KID )] = {0};

        ChkArg( f_cbKey == DRM_AES_KEYSIZE_128 );
        ChkDR( Oem_Aes_SetKey( f_pbKey, &aesKey ) );
        MEMCPY( rgbBlock, f_pChecksum->m_KID.rgb, SIZEOF( DRM_KID ) );
        dr = Oem_Aes_EcbEncryptData(&aesKey, rgbBlock, SIZEOF( DRM_KID ) );
        Oem_Aes_ZeroKey( &aesKey );
        ChkDR( dr );
        ChkBOOL( MEMCMP( f_pChecksum->m_rgbChecksumData, rgbBlock, DRM_AES_CK_CHECKSUM_LENGTH ) == 0,
                 DRM_E_CH_BAD_KEY );
    }
    else
    {
        DRM_LONG         lCount  = 0;
        DRM_SHA_CONTEXT *pShaVal = ( DRM_SHA_CONTEXT * )( f_pcontextBBX->CryptoContext.rgbCryptoContext );

        /* Run SHA on key SHA_ITERATIONS times. */
        /* First copy key into rgbTemp. */
        if ( f_cbKey > SIZEOF( rgbTemp ) )
        {
            f_cbKey = SIZEOF( rgbTemp ); /* Make sure we don't exceed buffer capacity in rgbTemp. */
        }

        MEMCPY( rgbTemp, f_pbKey, f_cbKey );

        for ( lCount = 0; lCount < SHA_ITERATIONS; lCount++ )
        {
            ChkDR( DRM_SHA_Init( pShaVal, eDRM_SHA_1 ) );
            ChkDR( DRM_SHA_Update( rgbTemp, DRM_SHA1_DIGEST_LEN + 1, pShaVal ) );
            ChkDR( DRM_SHA_Finalize( pShaVal, DRM_SHA1_DIGEST_LEN, rgbTemp ) );
        }

        ChkBOOL( MEMCMP( rgbTemp, f_pChecksum->m_rgbChecksumData, CHECKSUM_LENGTH ) == 0,
                 DRM_E_CH_BAD_KEY );
    }

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}


/*
    Decrypt a license that was encryted for privacy reasons (not
    security reasons) while traveling on the net during license
    acquisition.
*/
DRM_API DRM_BOOL DRM_CALL DRM_BBX_DecryptLicense(
    __inout_bcount( f_cbEncryptedLicense )   DRM_BYTE       *f_pbEncryptedLicense,
    __in                                     DRM_DWORD       f_cbEncryptedLicense,
    __out_bcount_opt( f_cbEncryptedLicense - DRM_ECC160_CIPHERTEXT_LEN )
                                             DRM_BYTE       *f_pbLicense,
    __inout_ecount( 1 )                      DRM_BB_CONTEXT *f_pcontextBBX,
    __in_opt                           const PUBKEY         *f_pDevPubKey )

{
    DRM_RESULT     dr      = DRM_SUCCESS;
    DRM_DWORD      cbKey   = DRM_ECC160_PLAINTEXT_LEN;
    PRIVKEY        privkey = { 0 };
    RC4_KEYSTRUCT *prc4KS  = NULL;
    DRM_BYTE       rgbKey[__CB_DECL( DRM_ECC160_PLAINTEXT_LEN )] = { 0 };

    DRM_PROFILING_ENTER_SCOPE(PERF_MOD_BLACKBOX, PERF_FUNC_DRM_BBX_DecryptLicense);

    ChkArg( f_pcontextBBX        != NULL
         && f_cbEncryptedLicense >= DRM_ECC160_CIPHERTEXT_LEN
         && f_pcontextBBX->fInited );

    /* use device privkey to decrypt the license blob */
    ChkDR(_GetDevicePrivkey(f_pcontextBBX, f_pDevPubKey, &privkey));
    ChkDR( DRM_PK_Decrypt( f_pcontextBBX->CryptoContext.rgbCryptoContext,
                           eDRM_ECC_P160,
                           DRM_ECC160_PRIVATE_KEY_LEN,
                           ( const DRM_BYTE * )&privkey,
                           DRM_ECC160_CIPHERTEXT_LEN,
                           f_pbEncryptedLicense,
                           &cbKey,
                           rgbKey ) );

    if ( GET_BYTE(rgbKey, 1) != PKSYMM_ALG_TYPE_RC4 )
    {
        ChkDR( DRM_E_UNSUPPORTEDALGORITHM );
    }

    f_cbEncryptedLicense -= DRM_ECC160_CIPHERTEXT_LEN;

    if ( f_pbLicense == NULL )
    {
        /* This means decrypt in place.  Caller must know that decrpyted content starts at pELicense + DRM_ECC160_CIPHERTEXT_LEN */
        f_pbLicense = f_pbEncryptedLicense + __CB_DECL(DRM_ECC160_CIPHERTEXT_LEN);
    }
    else
    {
        DRM_BYT_CopyBytes( f_pbLicense, 0, f_pbEncryptedLicense, DRM_ECC160_CIPHERTEXT_LEN, f_cbEncryptedLicense );
    }

    prc4KS = (RC4_KEYSTRUCT*)(f_pcontextBBX->CryptoContext.rgbCryptoContext);
    DRM_RC4_KeySetup(prc4KS, GET_BYTE(rgbKey, 0), rgbKey + __CB_DECL(2));
    DRM_RC4_Cipher  (prc4KS, f_cbEncryptedLicense, f_pbLicense );

ErrorExit:
    OEM_ECC_ZeroPrivateKey_P160( &privkey );
    DRM_PROFILING_LEAVE_SCOPE;
    return (DRM_SUCCEEDED(dr) ? TRUE : FALSE);
}

/*
** Note that this API must be called in the context of the keyfile context already open
** and locked for writing.
*/

DRM_API DRM_RESULT DRM_CALL DRM_BBX_SetupSecureStoreKey(
    DRM_KEYFILE_CONTEXT                 *f_pKeyFileContext,
    DRM_VOID                            *f_pOEMContext )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_ENCRYPTED_KEY oEncKey    = { 0 };

    ChkArg( f_pKeyFileContext != NULL );

    /* First check if we have the secure store key loaded in the keyfile cache already to avoid having to lock
    ** the keyfile with a call to DRM_KF_Open
    */
    dr = DRM_KF_GetPrivateKey( f_pKeyFileContext,
                               eKF_KEY_SECURE_STORE,
                               NULL,
                               0,
                               &oEncKey );

    if ( dr == DRM_E_KEYFILE_PRIVATE_KEY_NOT_FOUND )
    {
        /* No secure store key yet, need to create and store one. The DRM_KF_Open function will
        ** lock the keyfile exclusively so that nobody else should be trying to add one at the same time
        */
        DRM_ENCRYPTED_KEY oEncryptedKey = { 0 };
        DRM_BYTE rgbScratch[ __CB_DECL(DRM_AES_KEYSIZE_128
                             + (DRM_AES_BLOCKLEN
                                 - DRM_AES_KEYSIZE_128 % DRM_AES_BLOCKLEN) % DRM_AES_BLOCKLEN) ] = {0};

        ChkDR( Oem_Random_GetBytes(
            f_pOEMContext,
            rgbScratch,
            SIZEOF(rgbScratch) ) );

        oEncryptedKey.pbEncryptedKey = rgbScratch;
        oEncryptedKey.cbEncryptedKey = SIZEOF(rgbScratch);

        ChkDR( DRM_BBX_KF_EncryptKey( f_pOEMContext,
                                        rgbScratch,
                                        DRM_AES_KEYSIZE_128,
                                        DRM_DKT_SECURE_STORE,
                                        &oEncryptedKey ) );

        oEncryptedKey.wKeySize = DRM_AES_KEYSIZE_128 * BITS_PER_BYTE;

        ChkDR( DRM_KF_SetPrivateKey ( f_pKeyFileContext,
                                        eKF_KEY_SECURE_STORE,
                                        NULL,
                                        0,
                                        &oEncryptedKey ) );
    }
    else
    {
        ChkDR( dr );
    }

ErrorExit:
    return dr;
}

/*  Used for computing the secure store password. */
DRM_API DRM_RESULT DRM_CALL DRM_BBX_HashValue(
    __in_bcount( f_cbValue )  const     DRM_BYTE       *f_pbValue,
    __in                                DRM_DWORD       f_cbValue,
    __in_bcount( DRM_SHA1_DIGEST_LEN )  DRM_BYTE        f_rgbHashValue[__CB_DECL(DRM_SHA1_DIGEST_LEN)],
    __inout_ecount( 1 )                 DRM_BB_CONTEXT *f_pcontextBBX )
{
    DRM_RESULT       dr          = DRM_SUCCESS;
    DRM_BYTE rgbAESKey[ __CB_DECL( DRM_AES_KEYSIZE_128 ) ] = { 0 };
    DRM_ENCRYPTED_KEY oEncKey    = { 0 };
    DRM_DWORD        cbDecKey    = DRM_AES_KEYSIZE_128;
    DRM_AES_KEY      oAESKey     = { 0 };

    DRM_PROFILING_ENTER_SCOPE(PERF_MOD_BLACKBOX, PERF_FUNC_DRM_BBX_HashValue);

    ChkArg( f_pbValue      != NULL
         && f_cbValue       > 0
         && f_rgbHashValue != NULL
         && f_pcontextBBX  != NULL
         && f_pcontextBBX->fInited );

    ChkDR( DRM_KF_GetPrivateKey( f_pcontextBBX->pKeyFileContext,
                                 eKF_KEY_SECURE_STORE,
                                 NULL,
                                 0,
                                &oEncKey ) );

    ChkDR( DRM_BBX_KF_DecryptKey( f_pcontextBBX->pOEMContext,
                                   &oEncKey,
                                   DRM_DKT_SECURE_STORE,
                                   rgbAESKey,
                                   &cbDecKey ) );

    ChkDR( Oem_Aes_SetKey( rgbAESKey, &oAESKey ) );

    ZEROMEM( f_rgbHashValue, DRM_SHA1_DIGEST_LEN );

    /*
    ** AES 128 only generates 16 bytes of signature, the 20 bytes hash will
    ** be padded with trailing 0s.
    */
    DRMCASSERT( DRM_AES_BLOCKLEN <= DRM_SHA1_DIGEST_LEN );

    ChkDR( Oem_Omac1_Sign( &oAESKey,
                           f_pbValue,
                           0,
                           f_cbValue,
                           f_rgbHashValue ) );

ErrorExit:
    ZEROMEM( rgbAESKey, DRM_AES_KEYSIZE_128 );
    (DRM_VOID)Oem_Aes_ZeroKey( &oAESKey );

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/******************************************************************************
**
** Function :   DRM_BBX_CipherKeySetup
**
** Synopsis :   Set up the content key data ( decryption or encryption )
**
** Arguments :
**          [f_pBindingInfo]     : Binding information
**          [f_dwChainLength]    : Chain length
**          [f_pCipherContext]   : Cipher context to set up
**          [f_pcontextBBX]      : Blackbox content
**
** Returns :
**          DRM_SUCCESS
**              Success
**          DRM_E_INVALIDARG
**              A required parameter was NULL or improperly initialized
**          DRM_E_CH_BAD_KEY
**              The checksum over the content key failed
**          DRM_E_UNSUPPORTEDALGORITHM
**              The symmetric encryption type specified in f_pBindingInfo is
**              unsupported or unrecognized
**
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_BBX_CipherKeySetup(
    __in_ecount( f_dwChainLength ) const DRM_BINDING_INFO    *f_pBindingInfo,
    __in                                 DRM_DWORD            f_dwChainLength,
    __out_ecount( 1 )                    DRM_CIPHER_CONTEXT  *f_pCipherContext,
    __inout_ecount( 1 )                  DRM_BB_CONTEXT      *f_pcontextBBX )
{
    DRM_RESULT dr      = DRM_SUCCESS;
    DRM_DWORD  f_cbKey = 0;
    DRM_BYTE   rgbKey[__CB_DECL( DRM_AES_KEYSIZE_128 )] = { 0 };
    DRM_SUPPORTED_CIPHERS eCipherType = eDRM_RC4_CIPHER;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_BLACKBOX, PERF_FUNC_DRM_BBX_CipherKeySetup );

    ChkArg( f_pBindingInfo   != NULL
         && f_pCipherContext != NULL
         && f_pcontextBBX    != NULL
         && f_pcontextBBX->fInited );

    if ( f_pBindingInfo[0].m_fXMRLicense )
    {
        ChkDR( _DecryptContentKeyXMR( f_pBindingInfo,
                                      f_dwChainLength,
                                      f_pcontextBBX,
                                      FALSE,
                                      FALSE,
                                      SIZEOF( rgbKey ),
                                      rgbKey,
                                     &f_cbKey,
                                      NULL ) );

        if ( f_pBindingInfo[0].m_BindingInfo.m_XMR.m_pxmrContentKey->wSymmetricCipherType
             == XMR_SYMMETRIC_ENCRYPTION_TYPE_AES_128_CTR )
        {
            eCipherType = eDRM_AES_COUNTER_CIPHER;
        }
        else if ( f_pBindingInfo[0].m_BindingInfo.m_XMR.m_pxmrContentKey->wSymmetricCipherType
                  != XMR_SYMMETRIC_ENCRYPTION_TYPE_RC4_CIPHER &&
                  f_pBindingInfo[0].m_BindingInfo.m_XMR.m_pxmrContentKey->wSymmetricCipherType
                  != XMR_SYMMETRIC_ENCRYPTION_TYPE_COCKTAIL )
        {
            ChkDR( DRM_E_UNSUPPORTEDALGORITHM );
        }
    }
    else
    {
        ChkDR( _DecryptContentKeyXML( f_pBindingInfo,
                                      f_dwChainLength,
                                      f_pcontextBBX,
                                      FALSE, /* Enforce CK checksum on Bind */
                                      f_pBindingInfo->m_BindingInfo.m_XML.m_fCKSigVerified,
                                      SIZEOF( rgbKey ),
                                      rgbKey,
                                      &f_cbKey,
                                      NULL ) );
    }
    ChkDR( DRM_BBX_CPHR_Init( f_pCipherContext, eCipherType, f_cbKey, rgbKey ) );

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/******************************************************************************
**
** Function :   DRM_BBX_CPHR_InitDecrypt
**
** Synopsis :   Initializes state for phased decryption, calls into drmcipher.c
**
** Arguments :
**      f_pCipherContext : Cipher context initialized with DRM_CPHR_Init
**      f_pbLast15 : Pointer to the last 15 bytes of the ciphertext. If the
**                   ciphertext is less than 15 bytes, it is a pointer to the
**                   beginning of the ciphertext.
**      f_cbData   : # of bytes in ENTIRE ciphertext
**
** Returns :
**      DRM_SUCCESS
**          success
**      DRM_E_INVALIDARG
**          f_pContext is null, or f_pbLast15 is null when cipher type is RC4
**      DRM_E_CIPHER_NOTINITIALIZED
**          f_pContext was not appropriately initialized
**
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_BBX_CPHR_InitDecrypt(
    __inout_ecount( 1 )                                                DRM_CIPHER_CONTEXT *f_pCipherContext,
    __in_bcount_opt( min(DRM_CPHR_CB_FOR_INITDECRYPT,f_cbData) ) const DRM_BYTE           *f_pbLast15,
    __in                                                               DRM_DWORD           f_cbData )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pCipherContext != NULL );

    ChkDR( DRM_CPHR_InitDecrypt( f_pCipherContext,
                                 f_pbLast15,
                                 f_cbData ) );

ErrorExit:

    return dr;

}

/******************************************************************************
**
** Function :   DRM_BBX_CPHR_Decrypt
**
** Synopsis :   Decrypts part of the ciphertext. This function must be called
**              after DRM_BBX_CPHR_InitDecrypt has been successful, calls into drmcipher.c
**
** Arguments :
**     f_pCipherContext     : Cipher context initialized with DRM_CPHR_Init
**                      followed by DRM_CPHR_InitDecrypt
**     f_pCtrContext  : Sets and maintains the
**                      initialization vector and counter
**                      for AES CTR mode. Must be
**                      specified for AES decryption, and
**                      ignored for Cocktail.
**     f_cbData       :  Length of pbData in bytes
**     f_pbData       :  Array of bytes to decrypt
**
**  Returns:
**      DRM_SUCCESS
**          success
**      DRM_E_INVALIDARG
**          pbData or pContextCipher is null
**      DRM_E_CIPHER_NOTINITIALIZED
**          pContextCipher was not appropriately initialized
**      DRM_E_DECRYPT_NOTINITIALIZED
**          pContextCipher was not appropriately initialized for decryption (Cocktail only)
**      DRM_E_CRYPTO_FAILED
**          The encrypt/decrypt operation failed
**      DRM_E_ARITHMETIC_OVERFLOW
**          An overflow/underflow occurred while processing the data
**
** Notes :      IMPORTANT: (COCKTAIL ONLY)
**              Phased decrypt should be called on segments of
**              encrypted data sequentially, i.e. if encrypted data is divided
**              four segments, the order of decryption should be segment 1,
**              followed by 2, 3, 4.
**              To remove complexity we do not handle the case where
**              a packet is split somewhere in the middle of the last 15 bytes.
**              The caller is responsible to ensure that the last 15 bytes never
**              straddle multiple calls to Decrypt.
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_BBX_CPHR_Decrypt(
    __in_ecount( 1 )            DRM_CIPHER_CONTEXT           *f_pCipherContext,
    __inout_opt                 DRM_AES_COUNTER_MODE_CONTEXT *f_pCtrContext,
    __in                        DRM_DWORD                     f_cbData,
    __inout_bcount( f_cbData )  DRM_BYTE                     *f_pbData )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pCipherContext != NULL );

    dr = DRM_CPHR_Decrypt( f_pCipherContext,
                           f_pCtrContext,
                           f_cbData,
                           f_pbData );

ErrorExit:

    return dr;

}

/*********************************************************************
**
**  Function:  DRM_BBX_CPHR_Init
**
**  Synopsis:  Initializes a content cipher context.  This must be done prior to using
**             DRM_CPHR_Encrypt or DRM_BBX_CPHR_Decrypt, calls into drmcipher.c
**
**  Arguments:
**     f_pContext     :  The cipher context to be initialized
**     f_eCipherType  :  The type of cipher to be used
**     f_cbContKey    :  The length of f_pbContKey in bytes
**     f_pbContKey    :  The content key
**
**  Returns:
**      DRM_SUCCESS
**          success
**      DRM_E_INVALIDARG
**          one of the arguments was NULL or out of range
**
*********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_BBX_CPHR_Init(
    __out_ecount(1)                     DRM_CIPHER_CONTEXT    *f_pContext,
    __in                                DRM_SUPPORTED_CIPHERS  f_eCipherType,
    __in                                DRM_DWORD              f_cbContKey,
    __in_bcount(f_cbContKey)    const   DRM_BYTE              *f_pbContKey )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( NULL != f_pContext );

    ChkDR( DRM_CPHR_Init( f_pContext,
                          f_eCipherType,
                          f_cbContKey,
                          f_pbContKey ) );

ErrorExit:

    return dr;
}

static DRM_RESULT _GetDevicePrivkey(
    __inout_ecount( 1 )    DRM_BB_CONTEXT *f_pcontextBBX,
    __in_opt         const PUBKEY         *f_pDevPubKey,
    __out_ecount( 1 )      PRIVKEY        *f_pDevPrivKey )
{
    DRM_RESULT dr          = DRM_SUCCESS;
    DRM_ENCRYPTED_KEY oEncKey = { 0 };
    DRM_DWORD  cbDecKey    = SIZEOF( PRIVKEY );

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_BLACKBOX, PERF_FUNC__GetDevicePrivkey );

    ChkArg( f_pcontextBBX != NULL);
    ChkArg( f_pDevPrivKey != NULL);

    ChkBOOL( DRM_WMDRM_IsWMDRMSupported(), DRM_E_NOTIMPL );

    if ( f_pDevPubKey == NULL )
    {
        /* Use the current device public key. */
        f_pDevPubKey = &f_pcontextBBX->cachedCertValues.pubkeyCurrent;
    }

    ChkDR( DRM_KF_GetPrivateKey( f_pcontextBBX->pKeyFileContext,
                                 eKF_KEY_TYPE_ECC_160,
                                 ( const DRM_BYTE * )f_pDevPubKey,
                                 SIZEOF( PUBKEY ),
                                 &oEncKey ) );

    ChkDR( DRM_BBX_KF_DecryptKey( f_pcontextBBX->pOEMContext,
                                   &oEncKey,
                                   DRM_DKT_WMDRMPD_DEVICE,  /*Decrypting device key*/
                                   ( DRM_BYTE * )f_pDevPrivKey,
                                   &cbDecKey ) );
ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}

/******************************************************************************
** Function :   DRM_BBX_SignData
** Synopsis :   Sign data with the machine private key
** Arguments :  f_pcontextBBX       - Blackbox context
**              f_pbData            - Data to be signed
**              f_cbData            - Size of data to be signed
**              f_rgbSignature      - Signature
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_BBX_SignData(
    __inout_ecount( 1 )                      DRM_BB_CONTEXT *f_pcontextBBX,
    __in_bcount( f_cbData )  const           DRM_BYTE       *f_pbData,
    __in                                     DRM_DWORD       f_cbData,
    __in                                     DRM_DWORD       f_ePrivKeyType,
    __out_bcount( DRM_ECC160_SIGNATURE_LEN ) DRM_BYTE        f_rgbSignature[__CB_DECL( DRM_ECC160_SIGNATURE_LEN )] )
{
    PRIVKEY     privkey     = { 0 };
    DRM_RESULT  dr          = DRM_SUCCESS;
    DRM_DWORD   cbSignature = DRM_ECC160_SIGNATURE_LEN;

    ChkArg( f_pcontextBBX  != NULL
         && f_ePrivKeyType == eBBX_PRIVKEY_MACHINE
         && f_pcontextBBX->fInited );

    ChkDR( _GetDevicePrivkey(f_pcontextBBX, NULL, &privkey) );

    ChkDR( DRM_PK_Sign( f_pcontextBBX->CryptoContext.rgbCryptoContext,
                        eDRM_ECC_P160,
                        DRM_ECC160_PRIVATE_KEY_LEN,
                        ( const DRM_BYTE * )&privkey,
                        f_cbData,
                        f_pbData,
                        &cbSignature,
                        f_rgbSignature) );

ErrorExit:
    OEM_ECC_ZeroPrivateKey_P160( &privkey );
    return dr;
}

/*********************************************************************
**
**  Function:  DRM_BBX_SymmetricSign
**
**  Synopsis:  Creates a symmetric signature (that only the current blackbox can verify) over the data provided
**             by the caller
**
**  Arguments:
**     [f_pcontextBBX] -- Pointer to an initialized blackbox context
**     [f_pbData]      -- Pointer to the data to sign
**     [f_cbData]      -- Length of f_pbData in DRM_BYTEs
**     [f_pbExtraData] -- Additional data added to signed data.
**     [f_cbExtraData] -- Length of f_pbExtraData in bytes.
**     [f_rgbSymSig]   -- Buffer to place the resultant 'symmetric signature'
**
*********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_BBX_SymmetricSign(
    __inout_ecount( 1 )                    DRM_BB_CONTEXT *f_pcontextBBX,
    __in_bcount( f_cbData )  const         DRM_BYTE       *f_pbData,
    __in                                   DRM_DWORD       f_cbData,
    __in_bcount_opt( f_cbExtraData ) const DRM_BYTE       *f_pbExtraData,
    __in                                   DRM_DWORD       f_cbExtraData,
    __out_bcount( DRM_SHA1_DIGEST_LEN )    DRM_BYTE        f_rgbSymSig[__CB_DECL( DRM_SHA1_DIGEST_LEN )] )
{
    PRIVKEY          privkey = { 0 };
    DRM_HMAC_CONTEXT hmac    = { 0 };
    DRM_RESULT       dr      = DRM_SUCCESS;

    ChkArg( f_pcontextBBX  != NULL
         && f_pbData       != NULL
         && f_cbData       != 0
         && f_pcontextBBX->fInited );

    ChkDR( _GetDevicePrivkey(f_pcontextBBX, NULL, &privkey) );

    ChkDR( DRM_HMAC_Init( &hmac, eDRM_SHA_1, (DRM_BYTE*)&privkey, SIZEOF( privkey ) ) );
    ChkDR( DRM_HMAC_Update( &hmac, f_pbData, f_cbData ) );
    if ( f_pbExtraData != NULL )
    {
        ChkDR( DRM_HMAC_Update( &hmac, f_pbExtraData, f_cbExtraData ) );
    }
    ChkDR( DRM_HMAC_Finalize( &hmac, f_rgbSymSig, DRM_SHA1_DIGEST_LEN ) );

ErrorExit:
    OEM_ECC_ZeroPrivateKey_P160( &privkey );
    return dr;
}

/*********************************************************************
**
**  Function:  DRM_BBX_SymmetricVerify
**
**  Synopsis:  Verifies a symmetric signtaure that was created by a call to DRM_BBX_SymmetricSign
**
**  Arguments:
**     [f_pcontextBBX] -- Pointer to an initialized blackbox context
**     [f_pPubKey]     -- Pointer to an ECC 160 public key whose matching private key will be used
**                        to do the verification. If it is NULL, the current private key will be used.
**     [f_pbData]      -- Pointer to the data to sign
**     [f_cbData]      -- Length of f_pbData in DRM_BYTEs
**     [f_pbExtraData] -- Additional data added to signed data.
**     [f_cbExtraData] -- Length of f_pbExtraData in bytes.
**     [f_pslk]        -- Optional.  If this parameter is provided the caller is asking to verify
**                        the signature with a BBX protected SLK key.
**     [f_rgbSymSig]   -- Buffer containing the signtature to compare against.
**
*********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_BBX_SymmetricVerify(
    __inout_ecount( 1 )                      DRM_BB_CONTEXT *f_pcontextBBX,
    __in_opt                           const PUBKEY         *f_pPubKey,
    __in_bcount( f_cbData )            const DRM_BYTE       *f_pbData,
    __in                                     DRM_DWORD       f_cbData,
    __in_bcount_opt( f_cbExtraData )   const DRM_BYTE       *f_pbExtraData,
    __in                                     DRM_DWORD       f_cbExtraData,
    __in_opt                           const DRM_SLK        *f_pslk,
    __in_bcount( DRM_SHA1_DIGEST_LEN ) const DRM_BYTE        f_rgbSymSig[__CB_DECL( DRM_SHA1_DIGEST_LEN )] )
{
    PRIVKEY          privkey = { 0 };
    DRM_RESULT       dr      = DRM_SUCCESS;
    DRM_HMAC_CONTEXT hmac    = { 0 };
    DRM_BYTE         rgbMAC[__CB_DECL( DRM_SHA1_DIGEST_LEN )] = { 0 };

    DRM_PROFILING_ENTER_SCOPE(PERF_MOD_BLACKBOX, PERF_FUNC_DRM_BBX_SymmetricVerify);

    ChkArg( f_pcontextBBX  != NULL
         && f_pbData       != NULL
         && f_cbData       != 0
         && f_pcontextBBX->fInited );

    ChkDR( _GetDevicePrivkey(f_pcontextBBX, f_pPubKey, &privkey) );

    if ( f_pslk == NULL )
    {
        ChkDR( DRM_HMAC_Init( &hmac, eDRM_SHA_1, ( DRM_BYTE * )&privkey, SIZEOF( privkey ) ) );
    }
    else
    {
        DRM_SLK slkClear = { 0 };

        ChkDR( DRM_BBX_SymmetricCipher( SYMMETRIC_DECRYPT,
                                        TRUE,
                                        ( DRM_BYTE * )&privkey,
                                        SIZEOF( privkey ),
                                        ( DRM_BYTE * ) f_pslk,
                                        ( DRM_BYTE * )&slkClear,
                                        DES_KEYSIZE ) );  /* Only decrypt the first 8 bytes */
        ChkDR( DRM_HMAC_Init( &hmac, eDRM_SHA_1, ( DRM_BYTE * )&slkClear, DES_KEYSIZE ) );
    }

    ChkDR( DRM_HMAC_Update( &hmac, f_pbData, f_cbData ) );
    if ( f_pbExtraData != NULL )
    {
        ChkDR( DRM_HMAC_Update( &hmac, f_pbExtraData, f_cbExtraData ) );
    }
    ChkDR( DRM_HMAC_Finalize( &hmac, rgbMAC, DRM_SHA1_DIGEST_LEN ) );
    ChkBOOL( MEMCMP( rgbMAC, f_rgbSymSig, DRM_SHA1_DIGEST_LEN ) == 0, DRM_E_INVALID_SIGNATURE );

ErrorExit:
    OEM_ECC_ZeroPrivateKey_P160( &privkey );
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}

/*********************************************************************
**
**  Function:  _UpdateXMRWithSymmKey
**
**  Synopsis:  Takes an XMR license (bound to an asymmetric key or symmetric SLK),
**             a content integrity key (clear), and a new content and integrity key (re-encrypted symmetrically),
**             and updates the license with the symmetric key and new signature.
**             This function expects that the XMR license already contains an optimized content key object
**
**  Arguments:
**     [f_rgbCIKey]        : Buffer containing the clear CI key
**     [f_rgbEncryptedKeys]: Buffer containing the re-encrypted content and CI keys
**     [f_pXmrLicense]     : Pointer to the license object
**
*********************************************************************/
static DRM_RESULT DRM_CALL _UpdateXMRWithSymmKey(
    __in_bcount( DRM_AES_KEYSIZE_128 )     const DRM_BYTE         f_rgbCIKey[__CB_DECL( DRM_AES_KEYSIZE_128 )],
    __in_bcount( DRM_AES_KEYSIZE_128 * 2 ) const DRM_BYTE         f_rgbEncryptedKeys[__CB_DECL( DRM_AES_KEYSIZE_128 * 2 )],
    __inout_ecount( 1 )                          DRM_XMR_LICENSE *f_pXmrLicense )
{
    DRM_RESULT  dr     = DRM_SUCCESS;
    DRM_AES_KEY aesKey = {0};
    DRM_BYTE    rgbSignature[__CB_DECL(XMR_AES_OMAC_SIGNATURE_LENGTH)];

    ChkArg( f_pXmrLicense != NULL );

    ChkArg( f_pXmrLicense->containerOuter.containerKeys.OptimizedContentKey.fValid );

    ChkBOOL( f_pXmrLicense->containerOuter.containerKeys.OptimizedContentKey.cbEncryptedKey == DRM_AES_BLOCKLEN * 2,
             DRM_E_INVALIDLICENSE );
    DRM_BYT_CopyBytes( f_pXmrLicense->containerOuter.containerKeys.OptimizedContentKey.pbEncryptedKeyBuffer,
                       f_pXmrLicense->containerOuter.containerKeys.OptimizedContentKey.iEncryptedKey,
                       f_rgbEncryptedKeys,
                       0,
                       DRM_AES_BLOCKLEN * 2 );

    f_pXmrLicense->containerOuter.containerKeys.OptimizedContentKey.wKeyEncryptionCipherType = XMR_SYMMETRIC_KEY_ENCRYPTION_TYPE_AES_128_ECB;

    /*
    ** Copy the new encryption type to the license buffer
    */
    ChkBOOL( f_pXmrLicense->containerOuter.containerKeys.OptimizedContentKey.iEncryptedKey > 2 * SIZEOF( DRM_WORD ),
             DRM_E_INVALIDLICENSE );
    WORD_TO_NETWORKBYTES( f_pXmrLicense->containerOuter.containerKeys.OptimizedContentKey.pbEncryptedKeyBuffer,
                          f_pXmrLicense->containerOuter.containerKeys.OptimizedContentKey.iEncryptedKey - SIZEOF( DRM_WORD ) * 2,
                          XMR_SYMMETRIC_KEY_ENCRYPTION_TYPE_AES_128_ECB );


    ChkDR( Oem_Aes_SetKey(f_rgbCIKey, &aesKey) );
    dr = Oem_Omac1_Sign(&aesKey, f_pXmrLicense->pbSignedDataBuffer, f_pXmrLicense->iSignedData, f_pXmrLicense->cbSignedData, rgbSignature );
    Oem_Aes_ZeroKey(&aesKey);
    DRM_BYT_CopyBytes(f_pXmrLicense->containerOuter.signature.pbSignatureBuffer, f_pXmrLicense->containerOuter.signature.iSignature, rgbSignature, 0, XMR_AES_OMAC_SIGNATURE_LENGTH);
    ChkDR( dr );


ErrorExit:
    return dr;
}


/*********************************************************************
**
**  Function:  DRM_BBX_AsymmetricToSymmetricKeyXML
**
**  Synopsis:  Takes an asymmetrically encrypted XML license and symmetrically
**             rebinds it to the current blackbox.
**
**  Arguments:
**     [f_pcontextBBX] : Pointer to an initialized blackbox context
**     [f_pbindinfo]   : Pointer to the binding info of the license that is being optmized
**     [f_pcbSymmKey]  : Length of the resultant symmetric content key
**
**  Notes:  The content key will be reencrypted and returned in the same buffer.
**          The new length of the symmetric key is returned in f_pcbSymmKey.
**
*********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_BBX_AsymmetricToSymmetricKeyXML(
    __inout_ecount( 1 )      DRM_BB_CONTEXT    *f_pcontextBBX,
    __inout_ecount_opt( 1 )  DRM_BINDING_INFO  *f_pbindinfo,
    __out_ecount( 1 )        DRM_DWORD         *f_pcbSymmKey )
{
    DRM_RESULT dr      = DRM_SUCCESS;
    PRIVKEY    privkey = { 0 };
    DRM_BYTE   rgbKey[__CB_DECL( DRM_ECC160_PLAINTEXT_LEN )] = { 0 };

    ChkArg( f_pcontextBBX != NULL );
    ChkArg( f_pcbSymmKey  != NULL );
    ChkArg( f_pbindinfo   != NULL );
    ChkArg( f_pbindinfo->m_BindingInfo.m_XML.m_dwAlgorithm == eMSDRM_PK ); /* Must be an asymmetric encryption */
    ChkArg( f_pcontextBBX->fInited );

    ChkDR( _DecryptContentKeyXML( f_pbindinfo,
                                  1,
                                  f_pcontextBBX,
                                  TRUE,  /* Ignore CK checksum during symm opt */
                                  FALSE, /* Always verify ck signature during symm opt */
                                  SIZEOF( rgbKey ),
                                  rgbKey,
                                  f_pcbSymmKey,
                                 &privkey ) );

    DRM_BYT_CopyBytes( f_pbindinfo->m_BindingInfo.m_XML.m_rgbContentKey, 1, rgbKey, 0, *f_pcbSymmKey );
    PUT_BYTE( f_pbindinfo->m_BindingInfo.m_XML.m_rgbContentKey, 0, ( DRM_BYTE )*f_pcbSymmKey );

    *f_pcbSymmKey += 1;

    ChkDR( DRM_BBX_SymmetricCipher( SYMMETRIC_ENCRYPT,
                                    TRUE,
                                    ( DRM_BYTE * )&privkey,
                                    SIZEOF( PRIVKEY ),
                                    f_pbindinfo->m_BindingInfo.m_XML.m_rgbContentKey,
                                    NULL,
                                    *f_pcbSymmKey ) );

ErrorExit:
    OEM_SECURE_ZERO_MEMORY( rgbKey, SIZEOF( rgbKey ) );
    OEM_ECC_ZeroPrivateKey_P160( &privkey );
    return dr;
}

/*********************************************************************
**
**  Function:  DRM_BBX_AsymmetricToSymmetricKeyXMR
**
**  Synopsis:  Takes an asymmetrically encrypted license and symmetrically
**             rebinds it to the current blackbox. Adds the optimized content key object
**             if it doesn't already exist, then updates the signature object in-place.
**
**  Arguments:
**     [f_pcontextBBX]      : Pointer to an initialized blackbox context
**     [f_pbindinfoEx]      : Pointer to the binding info of the license that is being optmized
**     [f_pbLicense]        : Pointer to the license bytes
**     [f_cbLicense]        : Length of the entire license buffer
**     [f_pcbLicenseUsed]   : Pointer to the length of the license buffer used.
**     [f_pStack]           : Pointer to stack allocator context used to unpack license
**     [f_pcbSymmKey]       : Length of the resultant symmetric content key
**
**  Notes:  The content key will be reencrypted and returned in the same buffer.
**          The new length of the symmetric key is returned in f_pcbSymmKey.
**
*********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_BBX_AsymmetricToSymmetricKeyXMR(
    __inout_ecount( 1 )               DRM_BB_CONTEXT                *f_pcontextBBX,
    __inout_ecount_opt( 1 )           DRM_BINDING_INFO              *f_pbindinfo,
    __inout_bcount( f_cbLicense )     DRM_BYTE                      *f_pbLicense,
    __in                              DRM_DWORD                      f_cbLicense,
    __inout_ecount( 1 )               DRM_DWORD                     *f_pcbLicenseUsed,
    __inout_ecount( 1 )               DRM_STACK_ALLOCATOR_CONTEXT   *f_pStack,
    __out_ecount( 1 )                 DRM_DWORD                     *f_pcbSymmKey )
{
    DRM_RESULT   dr      = DRM_SUCCESS;
    PRIVKEY_P256 privkey = { 0 };
    DRM_BYTE     rgbKey[__CB_DECL( ECC_P256_PLAINTEXT_SIZE_IN_BYTES )] = { 0 };
    DRM_BYTE     rgbCIKey[__CB_DECL( DRM_AES_KEYSIZE_128 )] = { 0 }; /* Save the clear CI key */
    DRM_XMR_BINDING_INFO *pXmrBindInfo = NULL;

    ChkArg( f_pcontextBBX    != NULL );
    ChkArg( f_pcbSymmKey     != NULL );
    ChkArg( f_pcbLicenseUsed != NULL );
    ChkArg( f_pbindinfo      != NULL );
    ChkArg( f_pStack         != NULL );

    pXmrBindInfo = &f_pbindinfo->m_BindingInfo.m_XMR;

    ChkArg( f_pbindinfo->m_BindingInfo.m_XMR.m_pxmrContentKey != NULL );
    ChkArg( f_pcontextBBX->fInited );
    ChkArg( pXmrBindInfo->m_pxmrContentKey->wKeyEncryptionCipherType
         == XMR_ASYMMETRIC_ENCRYPTION_TYPE_ECC_256
         || pXmrBindInfo->m_pxmrContentKey->wKeyEncryptionCipherType
         == XMR_ASYMMETRIC_ENCRYPTION_TYPE_ECC_256_WITH_KZ
          ); /* Must be an asymmetric encryption */

    ChkDR( _DecryptContentKeyXMR( f_pbindinfo,
                                  1,
                                  f_pcontextBBX,
                                  TRUE,
                                  TRUE,
                                  SIZEOF( rgbKey ),
                                  rgbKey,
                                  f_pcbSymmKey,
                                  &privkey ) );
    MEMCPY( rgbCIKey, rgbKey, DRM_AES_KEYSIZE_128 );
    *f_pcbSymmKey = DRM_AES_BLOCKLEN * 2;
    ChkDR( DRM_BBX_SymmetricCipher( SYMMETRIC_ENCRYPT,
                                    FALSE,
                                    ( DRM_BYTE * )&privkey,
                                    SIZEOF( PRIVKEY_P256 ),
                                    rgbKey,
                                    NULL,
                                    *f_pcbSymmKey ) );

    if( pXmrBindInfo->m_pxmrOptimizedContentKey == NULL
     || !pXmrBindInfo->m_pxmrOptimizedContentKey->fValid )
    {
        /*
        ** Insert Optimized content key directly after regular content key
        */
        DRM_DWORD   dwInsertionPoint    = pXmrBindInfo->m_pxmrLicense->containerOuter.containerKeys.ContentKey.iguidKeyID
                                              - XMR_BASE_OBJECT_LENGTH;

        ChkOverflow( pXmrBindInfo->m_pxmrLicense->containerOuter.containerKeys.ContentKey.iguidKeyID, dwInsertionPoint );

        /*
        ** Expand license to accommodate symmetric key and update size
        */
        ChkDR( DRM_XMR_UpdateLicenseSizeForSymmKey(  f_pbLicense,
                                                     f_cbLicense,
                                                     f_pcbLicenseUsed,
                                                    &dwInsertionPoint ) );

        /*
        ** Unpack license structure again in order to readjust pointers which have changed
        */
        f_pStack->nStackTop = 0; /* Reset top of stack */
        ZEROMEM( pXmrBindInfo->m_pxmrLicense, SIZEOF( *pXmrBindInfo->m_pxmrLicense ) );
        ChkDR( DRM_XMR_UnpackLicense( f_pbLicense,
                                     *f_pcbLicenseUsed,
                                      f_pStack,
                                      pXmrBindInfo->m_pxmrLicense ) );
        /*
        ** Reset pointers in binding info structure
        */
        pXmrBindInfo->m_pxmrContentKey          = &pXmrBindInfo->m_pxmrLicense->containerOuter.containerKeys.ContentKey;
        pXmrBindInfo->m_pxmrOptimizedContentKey = &pXmrBindInfo->m_pxmrLicense->containerOuter.containerKeys.OptimizedContentKey;
        pXmrBindInfo->m_pxmrSignature           = &pXmrBindInfo->m_pxmrLicense->containerOuter.signature;
        pXmrBindInfo->m_pxmrDomainID            = &pXmrBindInfo->m_pxmrLicense->containerOuter.containerGlobalPolicies.DomainID;
    }

    ChkDR( _UpdateXMRWithSymmKey( rgbCIKey, rgbKey, pXmrBindInfo->m_pxmrLicense ) );

ErrorExit:
    OEM_SECURE_ZERO_MEMORY( rgbKey, SIZEOF( rgbKey ) );
    OEM_ECC_ZeroPrivateKey_P256( &privkey );
    return dr;
}

/*********************************************************************
**
**  Function:  DRM_BBX_RebindSymmetricKeyXML
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
DRM_API DRM_RESULT DRM_CALL DRM_BBX_RebindSymmetricKeyXML(
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

    ChkDR( _RebindSymmetricKey( f_pcontextBBX,
                                f_pslk,
                                f_pXmlBindInfo->m_rgbContentKey,
                                f_cbSymmKey,
                                SIZEOF( f_pXmlBindInfo->m_rgbContentKey ),
                                rgbDecryptedKeys ) );

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
**  Function:  DRM_BBX_RebindSymmetricKeyXMR
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
DRM_API DRM_RESULT DRM_CALL DRM_BBX_RebindSymmetricKeyXMR(
    __inout         DRM_BB_CONTEXT       *f_pcontextBBX,
    __in      const DRM_SLK              *f_pslk,
    __inout         DRM_XMR_LICENSE      *f_pXmrLicense )
{
    DRM_RESULT dr              = DRM_SUCCESS;
    DRM_DWORD  cbEncryptedKeys = 0;
    DRM_BYTE   rgbEncryptedKeys[__CB_DECL( DRM_AES_BLOCKLEN * 2 )] = { 0 };
    DRM_BYTE   rgbDecryptedKeys[__CB_DECL( DRM_AES_BLOCKLEN * 2 )] = { 0 };

    ChkArg( f_pcontextBBX != NULL );
    ChkArg( f_pslk        != NULL );
    ChkArg( f_pXmrLicense != NULL );

    cbEncryptedKeys = f_pXmrLicense->containerOuter.containerKeys.OptimizedContentKey.cbEncryptedKey;
    ChkBOOL( f_pXmrLicense->containerOuter.containerKeys.OptimizedContentKey.cbEncryptedKey <= SIZEOF( rgbEncryptedKeys ),
             DRM_E_BUFFERTOOSMALL );

    DRM_BYT_CopyBytes( rgbEncryptedKeys,
                       0,
                       f_pXmrLicense->containerOuter.containerKeys.OptimizedContentKey.pbEncryptedKeyBuffer,
                       f_pXmrLicense->containerOuter.containerKeys.OptimizedContentKey.iEncryptedKey,
                       f_pXmrLicense->containerOuter.containerKeys.OptimizedContentKey.cbEncryptedKey );

    ChkDR( _RebindSymmetricKey( f_pcontextBBX,
                                f_pslk,
                                rgbEncryptedKeys,
                                cbEncryptedKeys,
                                SIZEOF( rgbEncryptedKeys ),
                                rgbDecryptedKeys ) );
    ChkDR( _UpdateXMRWithSymmKey( rgbDecryptedKeys, rgbEncryptedKeys, f_pXmrLicense ) );

ErrorExit:
    OEM_SECURE_ZERO_MEMORY( &rgbEncryptedKeys,  SIZEOF( rgbEncryptedKeys ) );
    OEM_SECURE_ZERO_MEMORY( &rgbDecryptedKeys, SIZEOF( rgbDecryptedKeys) );

    return dr;
}


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
**     [f_cbKeyBuffer]       : Length of f_pbSymmKey buffer (must be larger than
**                              f_cbEncryptedKeys for the cases where the symmetric
**                              key becomes longer in this function).
**     [f_pbDecryptedKeys]   : Returns the decrypted key data (including CI key for XMR license)
**
*********************************************************************/
static DRM_RESULT _RebindSymmetricKey(
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
    DRM_BOOL     fUseLegacyCrypto = FALSE;

    ChkArg( f_pcontextBBX      != NULL );
    ChkArg( f_pslk             != NULL );
    ChkArg( f_pbEncryptedKeys  != NULL );
    ChkArg( f_cbEncryptedKeys  != 0    );
    ChkArg( f_cbKeyBuffer      != 0    );
    ChkArg( f_pcontextBBX->fInited     );

    if ( f_pslk->cbKey == DRM_AES_KEYSIZE_128 )
    {
        ChkDR( _GetDeviceECC256KeyPair( f_pcontextBBX,
                                        NULL,
                                        NULL,
                                        &privkey,
                                        DRM_BINARY_DEVICE_CERT_KEY_ENCRYPT ) );
    }
    else
    {
        ChkArg( f_pslk->cbKey == DES_KEYSIZE );
        ChkDR( _GetDevicePrivkey( f_pcontextBBX, NULL, ( PRIVKEY * )&privkey ) );

        fUseLegacyCrypto = TRUE;
        cbPrivkey        = SIZEOF( PRIVKEY );
    }

    /*
    ** First get the SLK key in the clear
    */
    ChkDR( DRM_BBX_SymmetricCipher( SYMMETRIC_DECRYPT,
                                    fUseLegacyCrypto,
                                    ( DRM_BYTE * )&privkey,
                                    cbPrivkey,
                                    ( DRM_BYTE * )f_pslk,
                                    ( DRM_BYTE * )&slkClear,
                                    f_pslk->cbKey ) );


    /*
    ** Use the clear SLK key to decrypt the content key
    */
    ChkBOOL( f_cbEncryptedKeys <= f_cbKeyBuffer, DRM_E_BUFFERTOOSMALL );

    if ( fUseLegacyCrypto )
    {
        ChkDR( DRM_BBX_SymmetricCipher( SYMMETRIC_DECRYPT,
                                        TRUE,
                                        ( DRM_BYTE * )&slkClear,
                                        DES_KEYSIZE,
                                        f_pbEncryptedKeys,
                                        f_pbDecryptedKeys,
                                        f_cbEncryptedKeys ) );
    }
    else
    {
        MEMCPY( f_pbDecryptedKeys, f_pbEncryptedKeys, f_cbEncryptedKeys );
        ChkDR( Oem_Aes_SetKey( ( DRM_BYTE * )&slkClear, ( DRM_AES_KEY * )( f_pcontextBBX->CryptoContext.rgbCryptoContext ) ) );
        ChkDR( Oem_Aes_EcbDecryptData( ( DRM_AES_KEY * )( f_pcontextBBX->CryptoContext.rgbCryptoContext ), f_pbDecryptedKeys, f_cbEncryptedKeys ) );
    }

    /*
    ** Use the device private key to symmetrically encrypt the clear content key
    */
    ChkDR( DRM_BBX_SymmetricCipher( SYMMETRIC_ENCRYPT,
                                    fUseLegacyCrypto,
                                    ( DRM_BYTE * )&privkey,
                                    cbPrivkey,
                                    f_pbDecryptedKeys,
                                    f_pbEncryptedKeys,
                                    f_cbEncryptedKeys ) );

ErrorExit:
    (DRM_VOID)Oem_Aes_ZeroKey( ( DRM_AES_KEY * )( f_pcontextBBX->CryptoContext.rgbCryptoContext ) );
    OEM_ECC_ZeroPrivateKey( &privkey, cbPrivkey );
    OEM_SECURE_ZERO_MEMORY( &slkClear, SIZEOF( slkClear) );
    return dr;
}

/*********************************************************************
**
**  Function:  DRM_BBX_RebindSLK
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
DRM_API DRM_RESULT DRM_CALL DRM_BBX_RebindSLK(
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

    DRM_PROFILING_ENTER_SCOPE(PERF_MOD_BLACKBOX, PERF_FUNC_DRM_BBX_RebindSLK);

    ChkArg( f_pcontextBBX  != NULL );
    ChkArg( f_pbEncSLK     != NULL );
    ChkArg( f_pslk         != NULL );
    ChkArg( f_pcontextBBX->fInited );

    if ( f_cbEncSLK == ECC_P256_CIPHERTEXT_SIZE_IN_BYTES )
    {
        ChkDR( _GetDeviceECC256KeyPair( f_pcontextBBX,
                                        NULL,
                                        NULL,
                                        &privkey,
                                        DRM_BINARY_DEVICE_CERT_KEY_ENCRYPT ) );

        ChkDR( DRM_PK_Decrypt( f_pcontextBBX->CryptoContext.rgbCryptoContext,
                               eDRM_ECC_P256,
                               SIZEOF( privkey ),
                               ( const DRM_BYTE * )&privkey,
                               f_cbEncSLK,
                               f_pbEncSLK,
                               &cbslk,
                               ( DRM_BYTE * )rgbslk ) );
        ChkDR( DRM_BBX_SymmetricCipher( SYMMETRIC_ENCRYPT,
                                        FALSE,
                                        ( DRM_BYTE * )&privkey,
                                        SIZEOF( privkey ),
                                        &rgbslk[DRM_AES_KEYSIZE_128],  /* First half is just padding */
                                        ( DRM_BYTE * )&f_pslk->rgbKey,
                                        DRM_AES_KEYSIZE_128 ) );
        f_pslk->cbKey = DRM_AES_KEYSIZE_128;
    }
    else
    {
        DRMCASSERT( DRM_ECC160_PLAINTEXT_LEN / 2 == DES_KEYSIZE );
        ChkArg( f_cbEncSLK == DRM_ECC160_CIPHERTEXT_LEN );

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
        ChkDR( DRM_BBX_SymmetricCipher( SYMMETRIC_ENCRYPT,
                                        TRUE,
                                        ( DRM_BYTE * )&privkey,
                                        SIZEOF( PRIVKEY ),
                                        rgbslk,
                                        ( DRM_BYTE * )&f_pslk->rgbKey,
                                        DES_KEYSIZE ) );
        f_pslk->cbKey = DES_KEYSIZE;
    }

ErrorExit:
    OEM_ECC_ZeroPrivateKey( &privkey, cbPrivKey );
    OEM_SECURE_ZERO_MEMORY( rgbslk, SIZEOF( rgbslk ) );
    DRM_PROFILING_LEAVE_SCOPE;
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
**          [f_fIncludeCIKey]   : Specifies to include the content integrity key in the returned
**                                key data. If TRUE, the buffer must be large enough to accomodate
**                                all of the key data.
**          [f_cbKeyBuff]       : length of the key buffer
**          [f_pbKeyBuff]       : Pointer to the key buffer
**          [f_pcbKey]          : Returns the length of the key
**          [f_pPrivKey]        : Returns the private key used to decrypt the content key (optional)
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
**          DRM_E_UNSUPPORTEDALGORITHM
**              An unsupported crypto algorithm was specified in the license
**          DRM_E_BUFFERTOOSMALL
**              The content key was too large for the buffer space
**          DRM_E_DOMAIN_BIND_LICENSE
**              No matching domain private key can be located (domain license only).
**
******************************************************************************/
static DRM_RESULT _DecryptContentKeyXMR(
    __in_ecount( f_dwChainLen ) const DRM_BINDING_INFO    *f_pBindingInfo,
    __in                              DRM_DWORD            f_dwChainLen,
    __inout_ecount( 1 )               DRM_BB_CONTEXT      *f_pContextBBX,
    __in                              DRM_BOOL             f_fIgnoreChecksum,
    __in                              DRM_BOOL             f_fIncludeCIKey,   /* Specifies to include content key in the returned key data */
    __in                              DRM_DWORD            f_cbKeyBuff,       /* the size of the key buffer */
    __out_bcount( f_cbKeyBuff )       DRM_BYTE            *f_pbKeyBuff,       /* the key buffer */
    __out_ecount( 1 )                 DRM_DWORD           *f_pcbKey,          /* the size of the key */
    __out_ecount_opt( 1 )             PRIVKEY_P256        *f_pPrivKey )       /* returns the privkey used to decrypt */
{
    DRM_RESULT   dr           = DRM_SUCCESS;
    PRIVKEY_P256 privKey      = { 0 };
    DRM_DWORD    cbDecKeys    = ECC_P256_PLAINTEXT_SIZE_IN_BYTES;
    DRM_DWORD    cbTemp       = SHA256_DIGEST_SIZE_IN_BYTES;
    DRM_BB_DOMAIN_KEY_RECORD oDomainKeyRecord = {0};
    DRM_BOOL     fSymmBound   = FALSE;
    const DRM_XMR_BINDING_INFO *pXmrBindInfo = NULL;

    DRM_BOOL     fDerived     = FALSE;
    DRM_BYTE    *pbScrub      = NULL;
    DRM_DWORD    cbScrub      = 0;

    /*
    ** Temp buffer is SHA256_DIGEST_SIZE_IN_BYTES (32 bytes), but may be used for SHA256 HMAC or to
    ** copy the DRM_AES_KEYSIZE_128 signature buffer (16 bytes) for OMAC verification, or
    ** to copy the Domain Account ID (128 bits) to get the domain key
    */
    DRM_BYTE     rgbTemp[__CB_DECL( SHA256_DIGEST_SIZE_IN_BYTES )]          = { 0 };
    DRM_BYTE     rgbDecKeys[__CB_DECL( ECC_P256_PLAINTEXT_SIZE_IN_BYTES )]  = { 0 };
    DRM_BYTE     rgbEncKeys[__CB_DECL( ECC_P256_CIPHERTEXT_SIZE_IN_BYTES )] = { 0 };

    ChkArg( f_pcbKey       != NULL );
    ChkArg( f_pContextBBX  != NULL );
    ChkArg( f_pBindingInfo != NULL );
    ChkArg( f_pbKeyBuff    != NULL );
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

    *f_pcbKey = 0;

    ChkBOOL( pXmrBindInfo->m_pxmrContentKey->wKeyEncryptionCipherType
          == XMR_ASYMMETRIC_ENCRYPTION_TYPE_ECC_256
          || pXmrBindInfo->m_pxmrContentKey->wKeyEncryptionCipherType
          == XMR_ASYMMETRIC_ENCRYPTION_TYPE_ECC_256_WITH_KZ,
             DRM_E_UNSUPPORTEDALGORITHM );
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
        DRM_BYT_CopyBytes( rgbDecKeys,
                           0,
                           pXmrBindInfo->m_pxmrDomainID->pbAccountID,
                           pXmrBindInfo->m_pxmrDomainID->ibAccountID,
                           pXmrBindInfo->m_pxmrDomainID->cbAccountID );
        cbTemp = SIZEOF( DRM_BB_DOMAIN_KEY_RECORD );
        if ( DRM_FAILED( DRM_DOMST_GetKey( f_pContextBBX->pDomainStoreContext,
                                           ( DRM_GUID * )rgbDecKeys,
                                           pXmrBindInfo->m_pxmrDomainID->dwRevision,
                                           (DRM_BYTE*)&oDomainKeyRecord,
                                           &cbTemp ) ) )
        {
            ChkDR( DRM_E_DOMAIN_BIND_LICENSE );
        }

        ChkBOOL( oDomainKeyRecord.dwVersion == DOMAIN_KEY_RECORD_VERSION, DRM_E_DOMAIN_STORE_INVALID_KEY_RECORD );
        ChkBOOL( cbTemp == SIZEOF( DRM_BB_DOMAIN_KEY_RECORD ), DRM_E_DOMAIN_STORE_INVALID_KEY_RECORD );

        ChkDR( _GetDeviceECC256KeyPair( f_pContextBBX,
                                        ( const PUBKEY_P256 * )& oDomainKeyRecord.oDevicePubKey,
                                        NULL,
                                        &privKey,
                                        DRM_BINARY_DEVICE_CERT_KEY_ENCRYPT ) );

        cbTemp = SIZEOF(PRIVKEY_P256);
        ChkDR( DRM_BBX_SymmetricBind( f_pContextBBX,
                                      FALSE,
                                      FALSE,
                                      (DRM_BYTE*)&oDomainKeyRecord.oDomainPrivKeyEncrypted,
                                      ( DRM_BYTE * )&privKey,
                                      cbTemp ) );
    }
    else
    {
        DRM_XMR_ECC_DEVICE_KEY *poECCKey = pXmrBindInfo->m_pxmrDeviceEccKey;

        ChkDR( _GetDeviceECC256KeyPair( f_pContextBBX,
                                        ( const PUBKEY_P256 * )( poECCKey->pbKeyData +
                                        __CB_DECL( poECCKey->iKeyData ) ),
                                        NULL,
                                        &privKey,
                                        DRM_BINARY_DEVICE_CERT_KEY_ENCRYPT ) );
    }

    /*
    **  Get the encrypted content key & integrity key
    */
    if ( pXmrBindInfo->m_pxmrOptimizedContentKey != NULL
      && pXmrBindInfo->m_pxmrOptimizedContentKey->fValid
      && pXmrBindInfo->m_pxmrOptimizedContentKey->wKeyEncryptionCipherType
      == XMR_SYMMETRIC_KEY_ENCRYPTION_TYPE_AES_128_ECB
      && pXmrBindInfo->m_pxmrOptimizedContentKey->cbEncryptedKey <= SIZEOF( rgbEncKeys ) )
    {
        DRM_BYT_CopyBytes( rgbEncKeys,
                           0,
                           pXmrBindInfo->m_pxmrOptimizedContentKey->pbEncryptedKeyBuffer,
                           pXmrBindInfo->m_pxmrOptimizedContentKey->iEncryptedKey,
                           pXmrBindInfo->m_pxmrOptimizedContentKey->cbEncryptedKey );

        cbDecKeys = pXmrBindInfo->m_pxmrOptimizedContentKey->cbEncryptedKey;

        if ( DRM_SUCCEEDED( DRM_BBX_SymmetricCipher( SYMMETRIC_DECRYPT,
                                                     FALSE,
                                                     ( DRM_BYTE * )&privKey,
                                                     SIZEOF( privKey ),
                                                     rgbEncKeys,
                                                     rgbDecKeys,
                                                     cbDecKeys ) ) )
        {
            fSymmBound = TRUE;
        }
    }

    if ( !fSymmBound ) /* Fall back to asymmetric binding if failed */
    {
        /*
        ** Need to make sure the encrypted content key buffer has at least
        ** ECC_P256_CIPHERTEXT_SIZE_IN_BYTES bytes (the size of rgbEncKeys)
        ** that can be decrypted via ECC-256.
        */
        ChkBOOL( pXmrBindInfo->m_pxmrContentKey->cbEncryptedKey >=
                 SIZEOF( rgbEncKeys ),
                 DRM_E_CH_BAD_KEY );

        DRM_BYT_CopyBytes( rgbEncKeys,
                           0,
                           pXmrBindInfo->m_pxmrContentKey->pbEncryptedKeyBuffer,
                           pXmrBindInfo->m_pxmrContentKey->iEncryptedKey,
                           ECC_P256_CIPHERTEXT_SIZE_IN_BYTES );

        /*
        ** Decrypt the content key with that private key
        */
        ChkDR( DRM_PK_Decrypt( f_pContextBBX,
                               eDRM_ECC_P256,
                               SIZEOF( PRIVKEY_P256 ),
                               ( const DRM_BYTE * )&privKey,
                               ECC_P256_CIPHERTEXT_SIZE_IN_BYTES,
                               rgbEncKeys,
                               &cbDecKeys,
                               rgbDecKeys ) );

        if ( fDerived )
        {
            ChkDR( _UnshuffleContentKey( rgbDecKeys ) );
        }
    }

    /*
    ** Verify the signature of the license buffer
    */
    if( pXmrBindInfo->m_pxmrSignature->wType == XMR_SIGNATURE_TYPE_AES_128_OMAC )
    {
        DRM_AES_KEY aesKey = {0};

        ChkBOOL( pXmrBindInfo->m_pxmrSignature->cbSignature == DRM_AES_BLOCKLEN,
                 DRM_E_INVALID_SIGNATURE );

        ChkDR( Oem_Aes_SetKey(rgbDecKeys, &aesKey) );
        dr = Oem_Omac1_Verify(&aesKey,
                                 pXmrBindInfo->m_pxmrLicense->pbSignedDataBuffer,
                                 pXmrBindInfo->m_pxmrLicense->iSignedData,
                                 pXmrBindInfo->m_pxmrLicense->cbSignedData,
                                 pXmrBindInfo->m_pxmrSignature->pbSignatureBuffer,
                                 pXmrBindInfo->m_pxmrSignature->iSignature );
        Oem_Aes_ZeroKey(&aesKey);
        ChkDR( dr );
    }
    else
    {
        ChkDR( DRM_E_UNSUPPORTEDALGORITHM );
    }

    /*
    ** ECB mode is allowed if and only if the license is a root license.
    ** However, we can tell a root license only when license chain length is > 1
    ** because the format of a root license and a simple license is the same.
    */
    if ( f_dwChainLen > 1 )
    {
        ChkBOOL( pXmrBindInfo->m_pxmrContentKey->wSymmetricCipherType
            == XMR_SYMMETRIC_ENCRYPTION_TYPE_AES_128_ECB, DRM_E_UNSUPPORTEDALGORITHM );
    }

    /*
    ** Get the size of the key
    */
    if ( pXmrBindInfo->m_pxmrContentKey->wSymmetricCipherType
      == XMR_SYMMETRIC_ENCRYPTION_TYPE_AES_128_CTR
      || pXmrBindInfo->m_pxmrContentKey->wSymmetricCipherType
      == XMR_SYMMETRIC_ENCRYPTION_TYPE_AES_128_ECB )
    {
        *f_pcbKey = DRM_AES_KEYSIZE_128;
    }
    else if ( pXmrBindInfo->m_pxmrContentKey->wSymmetricCipherType
           == XMR_SYMMETRIC_ENCRYPTION_TYPE_RC4_CIPHER )
    {
        *f_pcbKey = GET_BYTE( rgbDecKeys, DRM_AES_KEYSIZE_128 );
        ChkBOOL( *f_pcbKey == DRMCIPHERKEYLEN_RC4
              || *f_pcbKey == DRMCIPHERKEYLEN_RC4_PDRM, DRM_E_LIC_KEY_DECODE_FAILURE );
        /*
        ** Remove the size byte
        */
        DRM_BYT_MoveBytes( rgbDecKeys, DRM_AES_KEYSIZE_128, rgbDecKeys, DRM_AES_KEYSIZE_128 + 1, *f_pcbKey );
    }
    else if ( pXmrBindInfo->m_pxmrContentKey->wSymmetricCipherType
           == XMR_SYMMETRIC_ENCRYPTION_TYPE_COCKTAIL )
    {
        *f_pcbKey = DRMCIPHERKEYLEN_RC4;
    }
    else
    {
        ChkDR( DRM_E_UNSUPPORTEDALGORITHM );
    }

    /*
    ** Verify the checksum (over content key only).
    ** Only do so for a non chained license.
    ** For a chained license verifying root's checksum is not necessary
    ** because if it fails at root it would fail at leaf also. So it can be skipped.
    ** However, we can tell a non chained license only when license chain length is 1.
    */
    if ( f_dwChainLen == 1
     && !f_fIgnoreChecksum
     && !fDerived )
    {
        ChkDR( _VerifyChecksum( &rgbDecKeys[DRM_AES_KEYSIZE_128],
                                *f_pcbKey,
                                &f_pBindingInfo[f_dwChainLen-1].m_oChecksum,
                                f_pContextBBX ) );
    }

    /*
    ** Work back through the chain and decrypt key
    */
    for ( f_dwChainLen--; f_dwChainLen > 0; f_dwChainLen-- )
    {
        ZEROMEM( rgbTemp, SIZEOF( rgbTemp ) );
        pXmrBindInfo = &f_pBindingInfo[f_dwChainLen-1].m_BindingInfo.m_XMR;

        /*
        ** We are now in chained licenses enabling bits.  The algorithm *must* be chained
        ** And the decrypted root key *must* be an AES-128 key
        */
        ChkBOOL( pXmrBindInfo->m_pxmrContentKey->wKeyEncryptionCipherType
              == XMR_ASYMMETRIC_ENCRYPTION_TYPE_CHAINED_LICENSE
              && *f_pcbKey == DRM_AES_KEYSIZE_128,
                 DRM_E_UNSUPPORTEDALGORITHM );

        if ( fDerived )
        {
            DRM_XMR_KEY_MATERIAL *pXmrContainerKeyLeaf =
                &pXmrBindInfo->m_pxmrLicense->containerOuter.containerKeys;
            DRM_XMR_OUTER_CONTAINER *pXmrContainerOuterRoot =
                &f_pBindingInfo[f_dwChainLen].m_BindingInfo.m_XMR.m_pxmrLicense->containerOuter;

            /*
            ** Use rgbEncKeys to temporarily store the clear root CK and the encrypted secondary
            ** key (with the total size of 2 * DRM_AES_KEYSIZE_128 bytes).
            */
            DRMCASSERT( SIZEOF( rgbEncKeys ) >= 2 * DRM_AES_KEYSIZE_128 );

            MEMCPY( rgbEncKeys,
                    rgbDecKeys + DRM_AES_KEYSIZE_128,
                    DRM_AES_KEYSIZE_128 );

            /*
            ** The Content Key XMR object has the encrypted secondary key at offset
            ** ECC_P256_CIPHERTEXT_SIZE_IN_BYTES of the EncryptedKeyBuffer field.
            */
            DRMASSERT( pXmrContainerOuterRoot->containerKeys.ContentKey.wKeyEncryptionCipherType ==
                       XMR_ASYMMETRIC_ENCRYPTION_TYPE_ECC_256_WITH_KZ );

            MEMCPY( rgbEncKeys + DRM_AES_KEYSIZE_128,
                    pXmrContainerOuterRoot->containerKeys.ContentKey.pbEncryptedKeyBuffer +
                    pXmrContainerOuterRoot->containerKeys.ContentKey.iEncryptedKey +
                    ECC_P256_CIPHERTEXT_SIZE_IN_BYTES,
                    DRM_AES_KEYSIZE_128 );

            pbScrub = pXmrContainerKeyLeaf->UplinkX.pbKey;
            cbScrub = pXmrContainerKeyLeaf->UplinkX.cEntries * DRM_AES_KEYSIZE_128;

            ChkDR( _CalculateKeys_Derived( rgbEncKeys,
                                                  &pXmrContainerOuterRoot->containerKeys.AuxKey,
                                                  &pXmrContainerKeyLeaf->UplinkX ) );

            DRM_BYT_CopyBytes( rgbDecKeys,
                               0,
                               pXmrBindInfo->m_pxmrContentKey->pbEncryptedKeyBuffer,
                               pXmrBindInfo->m_pxmrContentKey->iEncryptedKey,
                               DRM_AES_KEYSIZE_128 * 2 );

            ChkDR( _DecryptContentKey_Derived( f_pContextBBX,
                                                      rgbEncKeys,
                                                      rgbEncKeys + DRM_AES_KEYSIZE_128,
                                                      &pXmrContainerKeyLeaf->UplinkX,
                                                      f_fIgnoreChecksum ?
                                                      NULL : &f_pBindingInfo[ f_dwChainLen ].m_oChecksum,
                                                      rgbDecKeys ) );
        }
        else
        {
            /*
            ** Use the current content key we extracted as an AES key in ECB mode
            ** to decrypt the next section
            */
            ChkDR( Oem_Aes_SetKey( &rgbDecKeys[DRM_AES_KEYSIZE_128],
                                   ( DRM_AES_KEY * )( f_pContextBBX->CryptoContext.rgbCryptoContext ) ) );
            DRM_BYT_CopyBytes( rgbDecKeys,
                               0,
                               pXmrBindInfo->m_pxmrContentKey->pbEncryptedKeyBuffer,
                               pXmrBindInfo->m_pxmrContentKey->iEncryptedKey,
                               DRM_AES_KEYSIZE_128 * 2 );
            ChkDR( Oem_Aes_EcbDecryptData( ( DRM_AES_KEY * )( f_pContextBBX->CryptoContext.rgbCryptoContext ),
                                           rgbDecKeys,
                                           DRM_AES_KEYSIZE_128 * 2 ) );
        }

        /*
        ** Get the size of the key
        */
        if ( pXmrBindInfo->m_pxmrContentKey->wSymmetricCipherType
          == XMR_SYMMETRIC_ENCRYPTION_TYPE_AES_128_CTR )
        {
            *f_pcbKey = DRM_AES_KEYSIZE_128;
        }
        else if ( pXmrBindInfo->m_pxmrContentKey->wSymmetricCipherType
               == XMR_SYMMETRIC_ENCRYPTION_TYPE_RC4_CIPHER )
        {
            *f_pcbKey = GET_BYTE( rgbDecKeys, DRM_AES_KEYSIZE_128 );
            ChkBOOL( *f_pcbKey == DRMCIPHERKEYLEN_RC4
                  || *f_pcbKey == DRMCIPHERKEYLEN_RC4_PDRM, DRM_E_LIC_KEY_DECODE_FAILURE );
            /*
            ** Remove the size byte
            */
            DRM_BYT_MoveBytes( rgbDecKeys, DRM_AES_KEYSIZE_128, rgbDecKeys, DRM_AES_KEYSIZE_128 + 1, *f_pcbKey );
        }
        else if ( pXmrBindInfo->m_pxmrContentKey->wSymmetricCipherType
               == XMR_SYMMETRIC_ENCRYPTION_TYPE_COCKTAIL )
        {
            *f_pcbKey = DRMCIPHERKEYLEN_RC4;
        }
        else
        {
            ChkDR( DRM_E_UNSUPPORTEDALGORITHM );
        }

        /*
        ** Verify the checksum (over content key only).
        ** Only do so for non derived key based leaf license
        ** since derived key based leaf license verifies checksum
        ** inside of _DecryptContentKey_Derived().
        */
        if ( !f_fIgnoreChecksum && !fDerived )
        {
            ChkDR( _VerifyChecksum( &rgbDecKeys[DRM_AES_KEYSIZE_128],
                                    *f_pcbKey,
                                    &f_pBindingInfo[f_dwChainLen-1].m_oChecksum,
                                    f_pContextBBX ) );
        }

        /*
        ** Verify the signature of the license buffer
        */
        if( pXmrBindInfo->m_pxmrSignature->wType == XMR_SIGNATURE_TYPE_AES_128_OMAC )
        {
            DRM_AES_KEY aesKey = {0};

            ChkBOOL( pXmrBindInfo->m_pxmrSignature->cbSignature == DRM_AES_BLOCKLEN,
                     DRM_E_INVALID_SIGNATURE );

            ChkDR( Oem_Aes_SetKey(rgbDecKeys, &aesKey) );
            dr = Oem_Omac1_Verify(&aesKey,
                                     pXmrBindInfo->m_pxmrLicense->pbSignedDataBuffer,
                                     pXmrBindInfo->m_pxmrLicense->iSignedData,
                                     pXmrBindInfo->m_pxmrLicense->cbSignedData,
                                     pXmrBindInfo->m_pxmrSignature->pbSignatureBuffer,
                                     pXmrBindInfo->m_pxmrSignature->iSignature );
            Oem_Aes_ZeroKey(&aesKey);
            ChkDR( dr );
        }
        else
        {
            ChkDR( DRM_E_UNSUPPORTEDALGORITHM );
        }
   } /* end-for chainlen */

    /*
    ** Everything looks good, copy the decrypted content key
    */
    if ( f_fIncludeCIKey )
    {
        if ( f_pBindingInfo[0].m_BindingInfo.m_XMR.m_pxmrContentKey->wSymmetricCipherType
          == XMR_SYMMETRIC_ENCRYPTION_TYPE_RC4_CIPHER )
        {
            /*
            ** Replace the size byte if we are returning the entire key buffer
            */
            ChkBOOL( *f_pcbKey <= DRMCIPHERKEYLEN_RC4, DRM_E_LIC_KEY_DECODE_FAILURE );
            DRM_BYT_MoveBytes( rgbDecKeys, DRM_AES_KEYSIZE_128 + 1, rgbDecKeys, DRM_AES_KEYSIZE_128, *f_pcbKey );
            PUT_BYTE( rgbDecKeys, DRM_AES_KEYSIZE_128, ( DRM_BYTE )*f_pcbKey );
            *f_pcbKey += 1;
        }
        *f_pcbKey += DRM_AES_KEYSIZE_128;
        ChkBOOL( *f_pcbKey <= f_cbKeyBuff, DRM_E_BUFFERTOOSMALL );
        DRM_BYT_CopyBytes( f_pbKeyBuff,
                           0,
                           rgbDecKeys,
                           0,
                           *f_pcbKey );
    }
    else
    {
        ChkBOOL( *f_pcbKey <= f_cbKeyBuff, DRM_E_BUFFERTOOSMALL );

        DRM_BYT_CopyBytes( f_pbKeyBuff,
                           0,
                           rgbDecKeys,
                           DRM_AES_KEYSIZE_128,
                           *f_pcbKey );
    }

    if ( f_pPrivKey != NULL )
    {
        MEMCPY( f_pPrivKey, &privKey, SIZEOF( PRIVKEY_P256 ) );
    }

ErrorExit:

    if ( pbScrub != NULL )
    {
        OEM_SECURE_ZERO_MEMORY( pbScrub, cbScrub );
    }

    OEM_ECC_ZeroPrivateKey_P256( &privKey );
    OEM_SECURE_ZERO_MEMORY( rgbEncKeys, ECC_P256_PLAINTEXT_SIZE_IN_BYTES );
    OEM_SECURE_ZERO_MEMORY( rgbDecKeys, ECC_P256_PLAINTEXT_SIZE_IN_BYTES );

    return dr;
}

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
** [f_poDevPrivKey]         -- Pointer to a PRIVKEY_P256 that is used to
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
static DRM_RESULT _GetDeviceECC256KeyPair(

    __in            DRM_BB_CONTEXT  *f_poBBXContext,
    __in_opt  const PUBKEY_P256     *f_poDevPubKeyIn,
    __out_opt       PUBKEY_P256     *f_poDevPubKey,
    __out           PRIVKEY_P256    *f_poDevPrivKey,
    __in            eDRM_BINARY_DEVICE_CERT_KEYUSAGE f_eDevKeyUsage )
{
    DRM_RESULT    dr          = DRM_SUCCESS;
    DRM_ENCRYPTED_KEY oEncKey = { 0 };
    DRM_DWORD     cbDecKey    = SIZEOF( PRIVKEY_P256 );
    DRM_DEVICE_KEY_TYPE eDecryptKeyType = DRM_DKT_INVALID;

    ChkArg( f_poBBXContext != NULL );
    ChkArg( f_poDevPrivKey != NULL );

    switch ( f_eDevKeyUsage )
    {
        case DRM_BINARY_DEVICE_CERT_KEY_SIGN:
                if ( f_poDevPubKeyIn == NULL )
                {
                    f_poDevPubKeyIn = &(f_poBBXContext->cachedBCertValues.DeviceKeySign.PublicKey);
                }
                eDecryptKeyType = DRM_DKT_PLAYREADY_DEVICE_SIGN;
                break;

        case DRM_BINARY_DEVICE_CERT_KEY_ENCRYPT:
                if ( f_poDevPubKeyIn == NULL )
                {
                    f_poDevPubKeyIn = &(f_poBBXContext->cachedBCertValues.DeviceKeyEncrypt.PublicKey);
                }
                eDecryptKeyType = DRM_DKT_PLAYREADY_DEVICE_ENCRYPT;
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

    ChkDR( DRM_BBX_KF_DecryptKey( f_poBBXContext->pOEMContext,
                                   &oEncKey,
                                   eDecryptKeyType,
                                   ( DRM_BYTE * )f_poDevPrivKey,
                                   &cbDecKey ) );
ErrorExit:
    return dr;
}


/*********************************************************************
**
** Function: DRM_BBX_ECC256_Decrypt
**
** Synopsis: Function that does ECC-256 decryption of the passed in data
**           in place using the device's ECC-256 private key, whose matching
**           ECC-256 public key is the passed in ECC-256 public key.
**
** Arguments:
**
** [f_poBBXContext]         -- Pointer to a DRM_BB_CONTEXT context.
** [f_pbData]               -- Pointer to a buffer that contains the
**                             data to be decrypted. After the decryption,
**                             it contains the decrypted data.
** [f_pcbData]              -- Pointer to a variable that contains the size
**                             (number of bytes) of the buffer to be
**                             decrypted. After the decryption, it receives
**                             the size (number of bytes) of the decrypted
**                             data.
** [f_poECC256PubKey]       -- Pointer to a device ECC-256 public key whose
**                             matching device ECC-256 private key should be
**                             used to decrypt the passed in data.
**                             If its content is all 0, caller assumes to
**                             use the current device private key to decrypt
**                             the data without checking whether
**                             *f_poECC256PubKey is the same as the current
**                             device public key.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_CRYPTO_PUBLIC_KEY_NOT_MATCH if the
**                          passed in ECC-256 public key does not match any
**                          device public key.
**
** Note:                    The caller ensures the passed in buffer is big
**                          enough to receive the decrypted data.
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_BBX_ECC256_Decrypt(
    __inout_ecount( 1 )          DRM_BB_CONTEXT *f_poBBXContext,
    __inout_bcount( *f_pcbData ) DRM_BYTE       *f_pbData,
    __inout_ecount( 1 )          DRM_DWORD      *f_pcbData,
    __inout_ecount( 1 )          PUBKEY_P256    *f_poECC256PubKey )
{
    DRM_RESULT      dr              = DRM_SUCCESS;
    PUBKEY_P256     oECC256PubKey   = { 0 };
    PRIVKEY_P256    oECC256PrivKey  = { 0 };
    PLAINTEXT_P256  oPlainText      = { 0 };
    CIPHERTEXT_P256 oCipherText     = { 0 };

    ChkArg( f_poBBXContext   != NULL );
    ChkArg( f_pbData         != NULL );
    ChkArg( f_pcbData        != NULL );
    ChkArg( *f_pcbData       == ECC_P256_CIPHERTEXT_SIZE_IN_BYTES );
    ChkArg( f_poECC256PubKey != NULL );

    if(DRM_UTL_IsZeros( (const DRM_BYTE*)f_poECC256PubKey, SIZEOF( PUBKEY_P256 ) ) )
    {
        f_poECC256PubKey = NULL;
    }

    ChkDR( _GetDeviceECC256KeyPair( f_poBBXContext,
                                    f_poECC256PubKey,
                                    &oECC256PubKey,
                                    &oECC256PrivKey,
                                    DRM_BINARY_DEVICE_CERT_KEY_ENCRYPT ) );

    ZEROMEM( &oPlainText, SIZEOF( PLAINTEXT_P256 ) );
    ZEROMEM( &oCipherText, SIZEOF( CIPHERTEXT_P256 ) );

    MEMCPY( oCipherText.m_rgbCiphertext, f_pbData, *f_pcbData );

    ChkDR( OEM_ECC_Decrypt_P256( &oECC256PrivKey,
                                 &oCipherText,
                                 &oPlainText,
                                 (struct bigctx_t *)f_poBBXContext->CryptoContext.rgbCryptoContext ) );

    MEMCPY( f_pbData, oPlainText.m_rgbPlaintext, ECC_P256_PLAINTEXT_SIZE_IN_BYTES );

    *f_pcbData = ECC_P256_PLAINTEXT_SIZE_IN_BYTES;

    /* Zero the temoprary decoded buffer to avoid leaking secrets in the heap. */
    OEM_SECURE_ZERO_MEMORY( &oPlainText, SIZEOF( PLAINTEXT_P256 ) );

ErrorExit:
    OEM_ECC_ZeroPrivateKey_P256( &oECC256PrivKey );
    OEM_ECC_ZeroPublicKey_P256( &oECC256PubKey );
    return dr;
}

/*********************************************************************
**
** Function: DRM_BBX_ECC256_Sign
**
** Synopsis: Function that generates an ECC-256 signature of the passed in
**           data. It also returns an ECC-256 public key used to verify
**           the generated ECC-256 signature.
**
** Arguments:
**
** [f_poBBXContext]         -- Pointer to a DRM_BB_CONTEXT context.
** [f_pbData]               -- Pointer to a data buffer for which
**                             ECC-256 signature is generated.
** [f_cbData]               -- Size (number of bytes) of a data buffer
**                             for which ECC-256 signature is generated.
** [f_poECC256PubKey]       -- Pointer to receive an ECC-256 public key
**                             whose matching ECC-256 private key is used
**                             to create the signature. The public key
**                             can be used to verify the signature
**                             generated in this function.
** [f_rgbECC256Signature]   -- Pointer to a buffer that receives a generated
**                             ECC-256 signature.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_BBX_ECC256_Sign(
    __inout_ecount( 1 )                                DRM_BB_CONTEXT *f_poBBXContext,
    __inout_bcount( f_cbData )                         const DRM_BYTE *f_pbData,
    __in                                               DRM_DWORD       f_cbData,
    __inout_ecount( 1 )                                PUBKEY_P256    *f_poECC256PubKey,
    __out_bcount( ECDSA_P256_SIGNATURE_SIZE_IN_BYTES ) DRM_BYTE        f_rgbECC256Signature[ __CB_DECL( ECDSA_P256_SIGNATURE_SIZE_IN_BYTES ) ] )
{
    DRM_RESULT     dr               = DRM_SUCCESS;
    PRIVKEY_P256   oECC256PrivKey   = { 0 };
    SIGNATURE_P256 oECC256Signature = { 0 };

    ChkArg( f_poBBXContext != NULL );
    ChkArg( f_poBBXContext->fInited );
    ChkArg( f_pbData != NULL );
    ChkArg( f_cbData > 0 );
    ChkArg( f_poECC256PubKey != NULL );
    ChkArg( f_rgbECC256Signature != NULL );

    OEM_ECC_ZeroPublicKey_P256( f_poECC256PubKey );

    ZEROMEM( f_rgbECC256Signature, ECDSA_P256_SIGNATURE_SIZE_IN_BYTES );

    ChkDR( _GetDeviceECC256KeyPair( f_poBBXContext,
                                    NULL,
                                    f_poECC256PubKey,
                                    &oECC256PrivKey,
                                    DRM_BINARY_DEVICE_CERT_KEY_SIGN ) );

    ChkDR( OEM_ECDSA_Sign_P256( f_pbData,
                                f_cbData,
                                &oECC256PrivKey,
                                &oECC256Signature,
                                (struct bigctx_t *)f_poBBXContext->CryptoContext.rgbCryptoContext ) );

    MEMCPY( f_rgbECC256Signature,
            oECC256Signature.m_rgbSignature,
            ECDSA_P256_SIGNATURE_SIZE_IN_BYTES );

ErrorExit:
    OEM_ECC_ZeroPrivateKey_P256( &oECC256PrivKey );
    return dr;
}

/*********************************************************************
 *
** Function: DRM_BBX_StoreDomainPrivateKeys
**
** Synopsis:
**           Parse the domain key XMR data and store domain private
**           keys in the store
**
** Arguments:
**  [f_poBBXContext]            -- Pointer to a DRM_BB_CONTEXT context.
**  [f_poDomainStoreContext]    -- Pointer to a DRM_DOMAINSTORE_CONTEXT
**                                 context.
**  [f_pbData]                  -- Pointer to a DRM_BYTE buffer that
**                                 contains the domain key XMR data
**  [f_cbData]                  -- The size of the XMR data
**  [f_poDomainID]              -- Pointer to a DRM_DOMAIN_ID struct
**  [f_poECC256PubKey]          -- Pointer to the ECC256 Public Key that
**                                 is used to encrypt the AES session
**                                 key
**  [f_pdastrCertificate]       -- Pointer a DRM_ANSI_STRING that holds
**                                 the domain certificate
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**
**********************************************************************/

DRM_API DRM_RESULT DRM_CALL DRM_BBX_StoreDomainPrivateKeys(
    __inout_ecount( 1 )                 DRM_BB_CONTEXT *f_poBBXContext,
    __inout_ecount( 1 )                 DRM_DOMAINSTORE_CONTEXT *f_poDomainStoreContext,
    __inout_bcount( f_cbData )          DRM_BYTE *f_pbData,
    __in                                DRM_DWORD f_cbData,
    __inout_ecount( 1 )                 DRM_DOMAIN_ID *f_poDomainID,
    __in_ecount( 1 )                    PUBKEY_P256 *f_poECC256PubKey,
    __in_ecount( 1 )                    DRM_ANSI_STRING *f_pdastrCertificate )
{
    DRM_RESULT                          dr                      = DRM_SUCCESS;
    DRM_DOMKEYXMR_HEADER                oXMRHeader              = {0};
    DRM_DOMKEYXMR_SESSIONKEY            oXMRSessionKey          = {0};
    DRM_DOMKEYXMR_PRIVKEYCONTAINER      oXMRPrivKeyContainer    = {0};
    DRM_DOMKEYXMR_PRIVKEY               oXMRDomainPrivKey       = {0};
    PRIVKEY_P256                        oDomainPrivKeyDecrypted = {0};
    DRM_BB_DOMAIN_KEY_RECORD            oDomainKeyRecord        = {0};
    DRM_BCERT_CHAIN_HEADER              oChainHeader            = {0};
    DRM_BCERT_CERTIFICATE               oCert                   = {0};
    DRM_BYTE                            rgbAESSessionKeys[ECC_P256_CIPHERTEXT_SIZE_IN_BYTES]  = {0};
    DRM_AES_KEY                         oAESKeyCI               = {0};
    DRM_AES_KEY                         oAESKeyCK               = {0};
    DRM_DWORD                           bCurrOffset             = 0;
    DRM_DWORD                           bCurrOffsetSaved        = 0;
    DRM_DWORD                           dwOffset                = 0;
    DRM_DWORD                           dwDomainRevisionPrev    = 0;
    DRM_BOOL                            fFirstRevision          = TRUE;
    DRM_BYTE                            rgbPasswordSST[__CB_DECL(DRM_SHA1_DIGEST_LEN)] = {0};

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_BLACKBOX, PERF_FUNC_DRM_BBX_StoreDomainPrivateKeys );

    ChkBOOL( DRM_DOMAIN_IsDomainSupported(), DRM_E_NOTIMPL );
    ChkArg( f_poBBXContext != NULL );
    ChkArg( f_poDomainStoreContext != NULL );
    ChkArg( f_pbData != NULL );
    ChkArg( f_cbData > 0 );  // should this be the minumu data length
    ChkArg( f_poDomainID != NULL );
    ChkArg( f_poECC256PubKey != NULL );
    ChkDRMANSIString( f_pdastrCertificate );

    /* get domain key xmr header */
    ChkDR( DRM_DOMKEYXMR_GetHeader(f_pbData, f_cbData, &bCurrOffset, &oXMRHeader) );
    ChkBOOL( oXMRHeader.dwConstant == DRM_DOMKEYXMR_HEADER_CONSTANT, DRM_E_DOMAIN_INVALID_DOMKEYXMR_DATA );
    ChkBOOL( oXMRHeader.dwVersion  == DRM_DOMKEYXMR_HEADER_VERSION,  DRM_E_DOMAIN_INVALID_DOMKEYXMR_DATA );
    ChkBOOL( bCurrOffset <= f_cbData, DRM_E_DOMAIN_INVALID_DOMKEYXMR_DATA );

    /* get domain key xmr session key */
    ChkDR( DRM_DOMKEYXMR_GetSessionKey( f_pbData, f_cbData, &bCurrOffset, &oXMRSessionKey ) );
    ChkBOOL( oXMRSessionKey.wType           == DRM_DOMKEYXMR_OBJTYPE_SESSIOINKEY,   DRM_E_DOMAIN_INVALID_DOMKEYXMR_DATA );
    ChkBOOL( oXMRSessionKey.wEncryptionType == DRM_DOMKEYXMR_SESSIONKEY_ENCTYPE_ECCP256, DRM_E_DOMAIN_INVALID_DOMKEYXMR_DATA );
    ChkBOOL( oXMRSessionKey.dwKeyLength     == ECC_P256_CIPHERTEXT_SIZE_IN_BYTES, DRM_E_DOMAIN_INVALID_DOMKEYXMR_DATA );
    ChkBOOL( bCurrOffset <= f_cbData, DRM_E_DOMAIN_INVALID_DOMKEYXMR_DATA );

    /* get domain key xmr privkey container */
    ChkDR( DRM_DOMKEYXMR_GetPrivKeyContainer(f_pbData, f_cbData, &bCurrOffset, &oXMRPrivKeyContainer ) );
    ChkBOOL( oXMRPrivKeyContainer.wType     == DRM_DOMKEYXMR_OBJTYPE_PRIVKEYCONTAINER, DRM_E_DOMAIN_INVALID_DOMKEYXMR_DATA );
    ChkBOOL( bCurrOffset <= f_cbData, DRM_E_DOMAIN_INVALID_DOMKEYXMR_DATA );

    /* get domain cert */
    ChkDR( DRM_BCert_GetChainHeader( PB_DASTR( f_pdastrCertificate ),
                                     CB_DASTR( f_pdastrCertificate ),
                                     &dwOffset,
                                     &oChainHeader ) );

    ChkDR( DRM_BCert_GetCertificate( PB_DASTR( f_pdastrCertificate ),
                                     CB_DASTR( f_pdastrCertificate ),
                                     &dwOffset,
                                     &oCert,
                                     DRM_BCERT_CERTTYPE_DOMAIN ) );

    /*
    ** Verify the revision numbers to ensure that:
    ** 1) The revision numbers are in descending order.
    ** 2) The first revision number on the list should be same as the revision number of the domain cert.
    */
    bCurrOffsetSaved = bCurrOffset;
    while( bCurrOffset + DRM_DOMKEYXMR_PRIVKEYOBJ_MIN_LENGTH < f_cbData )
    {
        ChkDR( DRM_DOMKEYXMR_GetPrivkey( f_pbData, f_cbData, &bCurrOffset, &oXMRDomainPrivKey ) );
        ChkBOOL( bCurrOffset <= f_cbData, DRM_E_DOMAIN_INVALID_DOMKEYXMR_DATA );
        if( fFirstRevision )
        {
            ChkBOOL( oXMRDomainPrivKey.dwRevision == oCert.Type.DomainInfo.dwRevision, DRM_E_DOMAIN_INVALID_DOMKEYXMR_DATA );
            fFirstRevision = FALSE;
        }
        else
        {
            ChkBOOL( dwDomainRevisionPrev > oXMRDomainPrivKey.dwRevision, DRM_E_DOMAIN_INVALID_DOMKEYXMR_DATA );
        }
        dwDomainRevisionPrev = oXMRDomainPrivKey.dwRevision;
    }


    MEMCPY(rgbAESSessionKeys, oXMRSessionKey.pbData, ECC_P256_CIPHERTEXT_SIZE_IN_BYTES );

    /* Decrypt AES session keys to get 2 AES session keys { CI|CK } */
    ChkDR(DRM_BBX_ECC256_Decrypt( f_poBBXContext,
                                 (DRM_BYTE*) rgbAESSessionKeys,
                                  &oXMRSessionKey.dwKeyLength,
                                  f_poECC256PubKey ) );

    ChkDR( Oem_Aes_SetKey((DRM_BYTE*) rgbAESSessionKeys, &oAESKeyCI) );
    ChkDR( Oem_Aes_SetKey((DRM_BYTE*) rgbAESSessionKeys+DRM_AES_KEYSIZE_128, &oAESKeyCK) );

    dr = DRM_DOMST_DeleteCert( f_poDomainStoreContext, &f_poDomainID->m_oAccountID );
    if (DRM_E_DOMAIN_STORE_DELETE_DATA == dr)
    {
        /* It is okay if we couldn't find the cert to delete, this may be the first time we
           have joined this domain.
           */
        dr = DRM_SUCCESS;
    }
    ChkDR( dr );

    ChkDR( DRM_DOMST_CreateDomainStorePassword( &f_poDomainID->m_oAccountID,
                                                f_poBBXContext,
                                                rgbPasswordSST ) );

    ChkDR( DRM_DOMST_AddCert( rgbPasswordSST,
                              f_poBBXContext->pOEMContext,
                              f_poDomainStoreContext,
                             &f_poDomainID->m_oAccountID,
                             &f_poDomainID->m_oServiceID,
                              PB_DASTR( f_pdastrCertificate ),
                              CB_DASTR( f_pdastrCertificate ) ) );

    bCurrOffset = bCurrOffsetSaved;
    while( bCurrOffset + DRM_DOMKEYXMR_PRIVKEYOBJ_MIN_LENGTH < f_cbData )
    {
        ChkDR( DRM_DOMKEYXMR_GetPrivkey( f_pbData, f_cbData, &bCurrOffset, &oXMRDomainPrivKey ) );
        ChkBOOL( oXMRDomainPrivKey.wType == DRM_DOMKEYXMR_OBJTYPE_PRIVKEY,  DRM_E_DOMAIN_INVALID_DOMKEYXMR_DATA );
        ChkBOOL( oXMRDomainPrivKey.wKeyType == DRM_DOMKEYXMR_PRIVKEY_TYPE_ECCP256, DRM_E_DOMAIN_INVALID_DOMKEYXMR_DATA );
        ChkBOOL( oXMRDomainPrivKey.dwKeyLength == ECC_P256_PRIVKEY_SIZE_IN_BYTES , DRM_E_DOMAIN_INVALID_DOMKEYXMR_DATA );
        ChkBOOL( oXMRDomainPrivKey.wEncryptionType == DRM_DOMKEYXMR_PRIVKEY_ENCTYPE_MIXED_AESECB, DRM_E_DOMAIN_INVALID_DOMKEYXMR_DATA );
        ChkBOOL( oXMRDomainPrivKey.dwKeyLength == ECC_P256_PRIVKEY_SIZE_IN_BYTES, DRM_E_DOMAIN_INVALID_DOMKEYXMR_DATA );
        ChkBOOL( bCurrOffset <= f_cbData, DRM_E_DOMAIN_INVALID_DOMKEYXMR_DATA );

        MEMCPY(&oDomainPrivKeyDecrypted, oXMRDomainPrivKey.pbData, ECC_P256_PRIVKEY_SIZE_IN_BYTES);

        f_poDomainID->m_dwRevision = oXMRDomainPrivKey.dwRevision;

        ChkDR(Oem_Aes_EcbDecryptData(&oAESKeyCI,
                                     (DRM_BYTE*) &oDomainPrivKeyDecrypted,
                                     DRM_AES_KEYSIZE_128) );

        ChkDR(Oem_Aes_EcbDecryptData(&oAESKeyCK,
                                     (DRM_BYTE*)&oDomainPrivKeyDecrypted + __CB_DECL(DRM_AES_KEYSIZE_128),
                                     DRM_AES_KEYSIZE_128) );

        ChkDR( DRM_BBX_SymmetricBind( f_poBBXContext,
                                      TRUE,
                                      FALSE,
                                     (DRM_BYTE*) &oDomainPrivKeyDecrypted,
                                     (DRM_BYTE*) &oDomainKeyRecord.oDomainPrivKeyEncrypted,
                                     SIZEOF( PRIVKEY_P256 ) ) );

        dr =   DRM_DOMST_DeleteKey( f_poDomainStoreContext,
                                   &f_poDomainID->m_oAccountID,
                                   f_poDomainID->m_dwRevision );
        if (DRM_E_DOMAIN_STORE_DELETE_DATA == dr)
        {
            /* It is okay if we couldn't find the key to delete, this may be the first time we
               have seen this revision value.
               */
            dr = DRM_SUCCESS;
        }
        ChkDR( dr );

        DRM_BYT_CopyBytes( &oDomainKeyRecord.oDevicePubKey,
                            0,
                            &f_poBBXContext->cachedBCertValues.DeviceKeyEncrypt.PublicKey,
                            0,
                            SIZEOF( PUBKEY_P256 ) );

        oDomainKeyRecord.dwVersion = DOMAIN_KEY_RECORD_VERSION;

        ChkDR( DRM_DOMST_AddKey( f_poDomainStoreContext,
                                &f_poDomainID->m_oAccountID,
                                 f_poDomainID->m_dwRevision,
                                 (DRM_BYTE *) &oDomainKeyRecord,
                                 SIZEOF( DRM_BB_DOMAIN_KEY_RECORD ) ) );

    }

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    OEM_ECC_ZeroPrivateKey_P256( &oDomainPrivKeyDecrypted );
    OEM_SECURE_ZERO_MEMORY( &oDomainKeyRecord, SIZEOF(DRM_BB_DOMAIN_KEY_RECORD));
    OEM_SECURE_ZERO_MEMORY( rgbAESSessionKeys, SIZEOF( rgbAESSessionKeys ) ) ;
    (DRM_VOID)Oem_Aes_ZeroKey( &oAESKeyCI );
    (DRM_VOID)Oem_Aes_ZeroKey( &oAESKeyCK ) ;
    return dr;
}


/*********************************************************************
**
** Function: DRM_BBX_SignWithCertKeys
**
** Synopsis:
**           Sign device cert with the appropriate keys for either P160 or P256 case.
**
** Arguments:
**  [f_pOEMContext]             -- A pointer to the OEM Context; may be NULL
**  [f_pCryptoCtx]              -- A pointer to the crypto context
**  [f_eKeyType]                -- Enum describing what key to
**                                           use for signing.
**  [f_pbPrivKey]                    TEMPORARY: Key to be used in P256 signing
**                                   can be NULL for P160
**  [f_cbData]                  -- Size of data to be signed
**  [f_pbData]                  -- Pointer data to be signed
**  [f_pcbSignature]            -- Returned size of signature
**  [f_pbSignature]             -- Signature created.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_BBX_SignWithCertKeys(
    __in_opt                                        DRM_VOID                           *f_pOEMContext,
    __inout                                         DRM_CRYPTO_CONTEXT                 *f_pCryptoCtx,
    __in                                            DRM_DEVICE_KEY_TYPE                 f_eKeyType,
    __in_bcount_opt(ECC_P256_INTEGER_SIZE_IN_BYTES) DRM_BYTE                           *f_pbPrivKey,  /* Remove once embedded group keys are used */
    __in                                            DRM_DWORD                           f_cbData,
    __in_bcount( f_cbData ) const                   DRM_BYTE                           *f_pbData,
    __inout_ecount( 1 )                             DRM_DWORD                          *f_pcbSignature,
    __out_bcount( *f_pcbSignature )                 DRM_BYTE                           *f_pbSignature
)
{
    DRM_RESULT                          dr                 = DRM_SUCCESS;
    DRM_PKCRYPTO_SUPPORTED_ALGORITHMS   eECCAlgorithm      = eDRM_ECC_P160;
    DRM_DWORD                           dwPrivKeyLength    = DRM_ECC160_PRIVATE_KEY_LEN;

    DRM_BYTE                           *pPrivateKey        = NULL;
    PRIVKEY                             oSigningKeyP160    = {0};

    ChkArg( NULL != f_pbData );
    ChkArg( NULL != f_pcbSignature );
    ChkArg( NULL != f_pbSignature );
    ChkArg( NULL != f_pCryptoCtx );

    switch (f_eKeyType)
    {
        case DRM_DKT_WMDRMPD_FALLBACK:
        case DRM_DKT_WMDRMPD_GROUP:
            {
                DRM_DWORD        cbSigningKey             = SIZEOF( PRIVKEY );

                ChkDR( Oem_Device_GetKey( f_pOEMContext,
                                          f_eKeyType,
                                          (DRM_BYTE *)&oSigningKeyP160,
                                         &cbSigningKey ) );

                pPrivateKey = ( DRM_BYTE * ) &oSigningKeyP160;
            }
            break;
        case DRM_DKT_PLAYREADY_MODEL:
            {
                /* Remove once embedded group keys are used */
                ChkArg( NULL != f_pbPrivKey );

                eECCAlgorithm  = eDRM_ECC_P256;
                dwPrivKeyLength = ECC_P256_PRIVKEY_SIZE_IN_BYTES;
                pPrivateKey = f_pbPrivKey;
            }
            break;
        default:
            ChkDR(DRM_E_INVALIDARG);
    }


    ChkDR( DRM_PK_Sign( f_pCryptoCtx->rgbCryptoContext,
                    eECCAlgorithm,
                    dwPrivKeyLength,
                    pPrivateKey,
                    f_cbData,
                    f_pbData,
                    f_pcbSignature,
                    f_pbSignature ) );

ErrorExit:
    if( pPrivateKey != NULL )
    {
        OEM_ECC_ZeroPrivateKey( pPrivateKey, dwPrivKeyLength );
    }
    return dr;
}


/*********************************************************************
**
** Function: DRM_BBX_GenerateECCKeyPair
**
** Synopsis:
**           Generation of keypairs is an important operation from
**           robustness perspective - private key of is one
**           of the most important robustness assets. Blackbox.c file
**           and oem files are the source files that should be the
**           only ones to be edited to increase the protection of
**           robustness assets.
**           In the given implementation this function just calls
**           a plain one to generate a P160 or P256 ECC key pair.
**           It should be called whenever a P160/P256 key pair for
**           encryption/decryption is generated.
**
** Arguments:
**  [f_pCryptoCtx]              -- A pointer to the crypto context
**  [f_eAlgorithm]              -- Enum describing what algorithm to
**                                 use for key generation.
**  [f_pcbPublicKey]            -- Size of pubkey buffer
**  [f_pbPublicKey]             -- Pubkey buffer
**  [f_pcbPrivateKey]           -- Size of privkey buffer
**  [f_pbPrivateKey]            -- Privkey buffer
**
**********************************************************************/

DRM_API DRM_RESULT DRM_CALL DRM_BBX_GenerateECCKeyPair(
    __inout                             DRM_CRYPTO_CONTEXT                 *f_pCryptoCtx,
    __in                                DRM_PKCRYPTO_SUPPORTED_ALGORITHMS   f_eAlgorithm,
    __inout_ecount( 1 )                 DRM_DWORD                          *f_pcbPublicKey,
    __out_bcount( *f_pcbPublicKey )     DRM_BYTE                           *f_pbPublicKey,
    __inout_ecount( 1 )                 DRM_DWORD                          *f_pcbPrivateKey,
    __out_bcount( *f_pcbPrivateKey )    DRM_BYTE                           *f_pbPrivateKey)
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pCryptoCtx!= NULL );
    ChkArg( ( eDRM_ECC_P160 == f_eAlgorithm )
         || ( eDRM_ECC_P256 == f_eAlgorithm ) );

    ChkDR( DRM_PK_GenKeyPair(f_pCryptoCtx->rgbCryptoContext,
            f_eAlgorithm,
            eDRM_RSA_UNKNOWN,
            f_pcbPublicKey,
            f_pbPublicKey,
            f_pcbPrivateKey,
            f_pbPrivateKey));

ErrorExit:
    return dr;
}


/*********************************************************************
**
** Function: _CalcContent_Key_Prime
**
** Synopsis: Function that calculates the AES key that is used to encrypt
**           the derived tree keys. The AES key is derived from the root
**           content key using the following formula:
**           K' = AES128_ENCRYPT_BLOCK(K, K^MCZero)
**
** Arguments:
**
** [f_pbContentKey]         -- Pointer to the root content key.
**                             child of the current key.
** [f_pbContentKeyPrime]    -- Pointer to a buffer to receive the calculated
**                             AES key.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
static DRM_RESULT _CalcContent_Key_Prime(
    __in_bcount( DRM_AES_KEYSIZE_128 )  const DRM_BYTE *f_pbContentKey,
    __out_bcount( DRM_AES_KEYSIZE_128 )       DRM_BYTE *f_pbContentKeyPrime )
{
    DRM_RESULT  dr = DRM_SUCCESS;
    DRM_DWORD   i  = 0;
    DRM_AES_KEY oAesKey = { 0 };

    MEMCPY( f_pbContentKeyPrime, f_pbContentKey, DRM_AES_KEYSIZE_128 );

    for ( i = 0; i < DRM_AES_KEYSIZE_128; i++ )
    {
        f_pbContentKeyPrime[ i ] ^= g_rgbMagicConstantZero[ i ];
    }

    ChkDR( Oem_Aes_SetKey( f_pbContentKey, &oAesKey ) );

    ChkDR( Oem_Aes_EcbEncryptData( &oAesKey, f_pbContentKeyPrime, DRM_AES_KEYSIZE_128 ) );

ErrorExit:
    Oem_Aes_ZeroKey( &oAesKey );
    return dr;
}

/*********************************************************************
**
** Function: _CalcNextKey
**
** Synopsis: Function that calculates the next key value based on the
**           current key value and whether the new key is the left or the
**           right child of the current key.
**
** Arguments:
**
** [f_fDeriveRight]         -- Flag indicating whether the new key is the right
**                             child of the current key.
** [f_pbKey]                -- Pointer to a buffer that contains the current key
**                             value during input and the new key value during output.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
static DRM_RESULT _CalcNextKey(
    __in DRM_BOOL  f_fDeriveRight,
    __inout_bcount( DRM_AES_KEYSIZE_128 )
         DRM_BYTE *f_pbKey )
{
    DRM_RESULT  dr      = DRM_SUCCESS;
    DRM_AES_KEY oAesKey = { 0 };
    DRM_BYTE    rgbResult[ DRM_AES_KEYSIZE_128 ];

    ChkDR( Oem_Aes_SetKey( f_pbKey, &oAesKey ) );

    if ( f_fDeriveRight )
    {
        MEMCPY( rgbResult, g_rgbMagicConstantRight, DRM_AES_KEYSIZE_128 );
    }
    else
    {
        MEMCPY( rgbResult, g_rgbMagicConstantLeft, DRM_AES_KEYSIZE_128 );
    }

    ChkDR( Oem_Aes_EcbEncryptData( &oAesKey, rgbResult, DRM_AES_KEYSIZE_128 ) );

    MEMCPY( f_pbKey, rgbResult, DRM_AES_KEYSIZE_128 );

ErrorExit:
    Oem_Aes_ZeroKey( &oAesKey );
    OEM_SECURE_ZERO_MEMORY( rgbResult, SIZEOF( rgbResult ) );
    return dr;
}

/*********************************************************************
**
** Function: _TryDeriveKey
**
** Synopsis: Function that tries to derive the key of a given location
**           using Aux Key entries from the stack that is used to store
**           the intermediate keys from pervious derivations. This
**           function may generate new stack entries during its derivation
**           process.
**
** Arguments:
**
** [f_dwLocation]           -- The location whose key is to be derived.
** [f_poKeyStack]           -- A stack of Aux Key entries that is used to stored the
**                             intermediate keys during key derivation.
** [f_pdwStackTop]          -- Pointer to a value that indicates the current top of
**                             the stack mentioned above.
** [f_pbKey]                -- Pointer to a buffer to store the derived key of the location.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_S_FALSE if the location cannot be derived from stack entries.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
static DRM_RESULT _TryDeriveKey(
    __in    DRM_DWORD              f_dwLocation,
    __inout DRM_XMR_AUX_KEY_ENTRY *f_poKeyStack,
    __inout DRM_DWORD             *f_pdwStackTop,
    __out_bcount( DRM_AES_KEYSIZE_128 )
            DRM_BYTE              *f_pbKey )
{
    DRM_RESULT dr           = DRM_SUCCESS;
    DRM_DWORD  dwSBMParent  = 0;
    DRM_BOOL   fDeriveRight = FALSE;

    if ( !IsAncestor( f_poKeyStack[ *f_pdwStackTop - 1 ].dwLocation,
                      f_dwLocation ) )
    {
        dr = DRM_S_FALSE;
        goto ErrorExit;
    }

    while ( f_poKeyStack[ *f_pdwStackTop - 1 ].dwLocation != f_dwLocation )
    {
        dwSBMParent = SBM( f_poKeyStack[ *f_pdwStackTop - 1 ].dwLocation );

        fDeriveRight = ( f_dwLocation & dwSBMParent ) != 0;
        MEMCPY( f_poKeyStack[ *f_pdwStackTop ].rgbKey,
                f_poKeyStack[ *f_pdwStackTop - 1 ].rgbKey,
                DRM_AES_KEYSIZE_128 );

        ChkDR( _CalcNextKey( fDeriveRight,  f_poKeyStack[ *f_pdwStackTop ].rgbKey ) );

        f_poKeyStack[ *f_pdwStackTop ].dwLocation =
                    ( f_poKeyStack[ *f_pdwStackTop - 1 ].dwLocation & ( ~dwSBMParent ) |
                    ( f_dwLocation & dwSBMParent ) |
                    ( dwSBMParent >> 1 ) );

        ( *f_pdwStackTop )++;

        ChkBOOL( *f_pdwStackTop < MAX_KEY_STACK, DRM_E_UNABLE_TO_RESOLVE_LOCATION_TREE );
    }

    MEMCPY( f_pbKey,
            f_poKeyStack[ *f_pdwStackTop - 1 ].rgbKey,
            DRM_AES_KEYSIZE_128 );

ErrorExit:
    return dr;
}

/*********************************************************************
**
** Function: _FindNextAuxEntry
**
** Synopsis: Function that bisectionally searches the Aux Key entry
**           array to find an Aux Key entry that can be used to derive
**           a given location.
**
** Arguments:
**
** [f_dwLocation]           -- The location that is used to search for an
**                             Aux Key entry to derive from.
** [f_poAuxKey]             -- Pointer to the Aux Key entry array in the root license.
** [f_pdwAuxKeyIndex]       -- Pointer to the beginning index of Aux Key entry to
**                          -- use in the Aux Key entry array. It may be updated
**                             when the function is returned.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_S_FALSE if no matching is found.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
static DRM_RESULT _FindNextAuxEntry(
    __in    DRM_DWORD        f_dwLocation,
    __in    DRM_XMR_AUX_KEY *f_poAuxKey,
    __inout DRM_DWORD       *f_pdwAuxKeyIndex )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_LONG   nLeft, nRight, nMiddle, nStart;
    DRM_DWORD  dwLocationToTry;

    if ( f_poAuxKey->cEntries == 0 ||
         *f_pdwAuxKeyIndex >= f_poAuxKey->cEntries )
    {
        dr = DRM_S_FALSE;
        goto ErrorExit;
    }

    nStart = nLeft = ( DRM_LONG )( *f_pdwAuxKeyIndex );
    nRight = f_poAuxKey->cEntries - 1;

    for ( ; ; )
    {
        nMiddle = ( nLeft + nRight ) / 2;

        NETWORKBYTES_TO_DWORD( dwLocationToTry, &f_poAuxKey->pEntries[ nMiddle ].dwLocation, 0 );
        if ( IsAncestor( dwLocationToTry, f_dwLocation ) )
        {
            *f_pdwAuxKeyIndex = nMiddle;
            break;
        }
        else if ( dwLocationToTry > f_dwLocation )
        {
            nRight = nMiddle - 1;
        }
        else
        {
            nLeft = nMiddle + 1;
        }

        if ( nLeft >= f_poAuxKey->cEntries ||
             nRight < nStart ||
             nLeft > nRight )
        {
            dr = DRM_S_FALSE;
            break;
        }
    }

ErrorExit:
    return dr;
}

/*********************************************************************
**
** Function: _CalcOneDerivedKey
**
** Synopsis: Function that calculates the key of an UplinkX entry from
**           the Aux Key entries in the root license.
**
** Arguments:
**
** [f_pbContentKeyPrime]    -- Pointer to the clear AES 128 content key prime
**                             used to decrypt the key in the Aux Key entry.
** [f_poAuxKey]             -- Pointer to the Aux Key entry array in the root license.
** [f_poUplinkX]            -- Pointer the UplinkX entry array in the leaf license.
** [f_dwUplinkXIdx]         -- The index of the UplinkX entry whose key is to
**                             be calculated.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_UNABLE_TO_RESOLVE_LOCATION_TREE if a key cannot be derived.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
** Note:                    The Aux entries in the root license are sorted in their
**                          location values (ascending).
**
**********************************************************************/
DRM_RESULT _CalcOneDerivedKey(
    __in_bcount( DRM_AES_KEYSIZE_128 )
         const DRM_BYTE              *f_pbContentKeyPrime,
    __in       DRM_XMR_AUX_KEY       *f_poAuxKey,
    __inout    DRM_XMR_UPLINKX       *f_poUplinkX,
    __in       DRM_DWORD              f_dwUplinkXIdx )
{
    DRM_RESULT dr           = DRM_SUCCESS;
    DRM_XMR_AUX_KEY_ENTRY rgoKeyStack[ MAX_KEY_STACK ];
    DRM_DWORD dwStackTop    = 0;
    DRM_DWORD dwAuxKeyIndex = 0;
    DRM_BOOL  fFound        = FALSE;
    DRM_DWORD dwLocation    = 0;

    /* Need to covert the location to machine byte order before being used. */
    NETWORKBYTES_TO_DWORD( dwLocation, &f_poUplinkX->pdwLocation[ f_dwUplinkXIdx ], 0 );

    for ( ; ; )
    {
        while ( dwStackTop > 0 )
        {
            ChkDR( _TryDeriveKey( dwLocation,
                                  rgoKeyStack,
                                  &dwStackTop,
                                  &f_poUplinkX->pbKey[ f_dwUplinkXIdx * DRM_AES_KEYSIZE_128 ] ) );

            if ( dr == DRM_SUCCESS )
            {
                fFound = TRUE;
                break;
            }
            else
            {
                dwStackTop--;
            }
        }

        if ( fFound )
        {
            goto ErrorExit;
        }

        ChkBOOL( dwStackTop == 0, DRM_E_UNABLE_TO_RESOLVE_LOCATION_TREE );

        ChkDR( _FindNextAuxEntry( dwLocation, f_poAuxKey, &dwAuxKeyIndex ) );

        if ( dr == DRM_S_FALSE )
        {
            /* No match. */
            TRACE( ( "Fail to derive key for location: %d", dwLocation ) );
            ChkDR( DRM_E_UNABLE_TO_RESOLVE_LOCATION_TREE );
        }
        else
        {
            DRM_AES_KEY oAesKey = { 0 };
            DRM_DWORD dwLocation = 0;

            MEMCPY( &rgoKeyStack[ dwStackTop ],
                    &f_poAuxKey->pEntries[ dwAuxKeyIndex ],
                    SIZEOF( DRM_XMR_AUX_KEY_ENTRY ) );

            NETWORKBYTES_TO_DWORD( dwLocation, &rgoKeyStack[ dwStackTop ].dwLocation, 0 );
            rgoKeyStack[ dwStackTop ].dwLocation = dwLocation;

            ChkDR( Oem_Aes_SetKey( f_pbContentKeyPrime, &oAesKey ) );

            ChkDR( Oem_Aes_EcbEncryptData( &oAesKey,
                                           rgoKeyStack[ dwStackTop ].rgbKey,
                                           DRM_AES_KEYSIZE_128 ) );

            dwAuxKeyIndex++;
            dwStackTop++;
        }
    }

ErrorExit:
    return dr;
}

/*********************************************************************
**
** Function: _CalculateKeys_Derived
**
** Synopsis: Function that calculates all UplinkX keys from the Aux Key
**           entries in the root license.
**
** Arguments:
**
** [f_pbRootContentKey]     -- Pointer to the clear AES 128 root content key
**                             used to decrypt the key in the Aux Key entry.
** [f_poAuxKey]             -- Pointer to the Aux Key entry array in the root license.
** [f_poUplinkX]            -- Pointer the UplinkX entry array in the leaf license.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
** Note:                    The Aux entries in the root license are sorted in their
**                          location values (ascending).
**
**********************************************************************/
static DRM_RESULT _CalculateKeys_Derived(
    __in_bcount( DRM_AES_KEYSIZE_128 )
         const DRM_BYTE              *f_pbRootContentKey,
    __in       DRM_XMR_AUX_KEY       *f_poAuxKey,
    __inout    DRM_XMR_UPLINKX       *f_poUplinkX )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD  i  = 0;
    DRM_BYTE   rgbContentKeyPrime[ DRM_AES_KEYSIZE_128 ];

    ChkDR( f_pbRootContentKey != NULL );
    ChkDR( f_poAuxKey != NULL && f_poUplinkX != NULL );

    ChkDR( _CalcContent_Key_Prime( ( DRM_BYTE * )f_pbRootContentKey,
                                   rgbContentKeyPrime ) );

    for ( i = 0; i < f_poUplinkX->cEntries; i++ )
    {
        ChkDR( _CalcOneDerivedKey( rgbContentKeyPrime,
                                   f_poAuxKey,
                                   f_poUplinkX,
                                   i ) );
    }

ErrorExit:
    return dr;
}

/*********************************************************************
**
** Function: _VerifyChecksum
**
** Synopsis: Function that verifies the checksum of the key that is used
**           to decrypt the leaf content key.
**
** Arguments:
**
** [f_pbKey]                -- Pointer to the AES 128 key to be verified.
** [f_poChecksum]           -- Pointer to a checksum to be verified.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_CH_BAD_KEY if checksum verification fails.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
static DRM_RESULT _VerifyChecksum2(
    __in_bcount( DRM_AES_KEYSIZE_128 ) const DRM_BYTE                *f_pbKey,
    __in                               const DRM_BB_CHECKSUM_CONTEXT *f_poChecksum )
{
    DRM_RESULT  dr      = DRM_SUCCESS;
    DRM_AES_KEY oAesKey = { 0 };
    DRM_BYTE    rgbBlock[ SIZEOF( DRM_KID ) ] = { 0 };

    ChkDR( Oem_Aes_SetKey( f_pbKey, &oAesKey ) );

    MEMCPY( rgbBlock, f_poChecksum->m_KID.rgb, SIZEOF( DRM_KID ) );

    ChkDR( Oem_Aes_EcbEncryptData( &oAesKey, rgbBlock, SIZEOF( DRM_KID ) ) );

    ChkBOOL( MEMCMP( f_poChecksum->m_rgbChecksumData, rgbBlock, DRM_AES_CK_CHECKSUM_LENGTH ) == 0,
             DRM_E_CH_BAD_KEY );

ErrorExit:
    Oem_Aes_ZeroKey( &oAesKey );
    return dr;
}

/*********************************************************************
**
** Function: _DecryptContentKey_Derived
**
** Synopsis: Function that decryptes a content key using all derived keys
**           of locations in the UplinkX plus the seconary key in the Aux Key.
**
** Arguments:
**
** [f_poBBXContext]         -- Pointer to a black box context.
** [f_pbRootContentKey]     -- Pointer to the clear AES 128 root content key
**                             used to decrypt the secondary key in the Aux Key
**                             object.
** [f_pbSecondaryKey]       -- Pointer to an encrypted secondary key.
** [f_poUplinkX]            -- Pointer the UplinkX entry array in the leaf license.
** [f_poChecksum]           -- Pointer to a checksum that is used to make sure the
**                             key used to decrypt the content key is valid. If this
**                             parameter is NULL, checksum will not be verified.
** [f_pbKey]                -- Pointer to a buffer that contains the encrypted
**                             content key during input, and the decrypted content
**                             key during output. The size of the key is
**                             DRM_AES_KEYSIZE_128 * 2 since it includes both CI and CK.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
static DRM_RESULT _DecryptContentKey_Derived(
    __inout_ecount( 1 )
                   DRM_BB_CONTEXT          *f_poBBXContext,
    __in_bcount( DRM_AES_KEYSIZE_128 )
             const DRM_BYTE                *f_pbRootContentKey,
    __in_bcount( DRM_AES_KEYSIZE_128 )
             const DRM_BYTE                *f_pbSecondaryKey,
    __in           DRM_XMR_UPLINKX         *f_poUplinkX,
    __in_opt const DRM_BB_CHECKSUM_CONTEXT *f_poChecksum,
    __out_bcount( DRM_AES_KEYSIZE_128 * 2 )
                   DRM_BYTE                *f_pbKey )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_BYTE   rgbMixedKey[ DRM_AES_KEYSIZE_128 ] = { 0 };
    DRM_DWORD  i = 0, j = 0;

    ChkArg( f_poBBXContext != NULL );
    ChkArg( f_pbRootContentKey != NULL );
    ChkArg( f_pbSecondaryKey != NULL );
    ChkArg( f_poUplinkX != NULL );
    ChkArg( f_pbKey != NULL );

    ZEROMEM( rgbMixedKey, SIZEOF( rgbMixedKey ) );

    for ( i = 0; i < f_poUplinkX->cEntries; i++ )
    {
        for ( j = 0; j < DRM_AES_KEYSIZE_128; j++ )
        {
            rgbMixedKey[ j ] ^= f_poUplinkX->pbKey[ i * DRM_AES_KEYSIZE_128 + j ];
        }
    }

    /* Semi-decrypt the leaf content key using the combined middle (key tree) key. */
    ChkDR( Oem_Aes_SetKey( rgbMixedKey,
                           ( DRM_AES_KEY * )( f_poBBXContext->CryptoContext.rgbCryptoContext ) ) );
    ChkDR( Oem_Aes_EcbEncryptData( ( DRM_AES_KEY * )( f_poBBXContext->CryptoContext.rgbCryptoContext ),
                                   f_pbKey,
                                   DRM_AES_KEYSIZE_128 * 2 ) );

    /* Decrypt the secondary key using the root content key. */
    ChkDR( Oem_Aes_SetKey( f_pbRootContentKey,
                           ( DRM_AES_KEY * )( f_poBBXContext->CryptoContext.rgbCryptoContext ) ) );
    MEMCPY( rgbMixedKey, f_pbSecondaryKey, DRM_AES_KEYSIZE_128 );
    ChkDR( Oem_Aes_EcbEncryptData( ( DRM_AES_KEY * )( f_poBBXContext->CryptoContext.rgbCryptoContext ),
                                   rgbMixedKey,
                                   DRM_AES_KEYSIZE_128 ) );

    /* Decrypt the semi decrypted leaf content key using the clear secondary key. */
    ChkDR( Oem_Aes_SetKey( rgbMixedKey,
                           ( DRM_AES_KEY * )( f_poBBXContext->CryptoContext.rgbCryptoContext ) ) );
    ChkDR( Oem_Aes_EcbEncryptData( ( DRM_AES_KEY * )( f_poBBXContext->CryptoContext.rgbCryptoContext ),
                                   f_pbKey,
                                   DRM_AES_KEYSIZE_128 * 2 ) );

    if ( f_poChecksum != NULL )
    {
        ChkDR( _VerifyChecksum2( f_pbKey + DRM_AES_KEYSIZE_128, f_poChecksum ) );
    }

ErrorExit:
    OEM_SECURE_ZERO_MEMORY( rgbMixedKey, SIZEOF( rgbMixedKey ) );
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

/*********************************************************************
**
** Function: _UnshuffleContentKey
**
** Synopsis: Function that unshuffles a content key in the form of CI||CK
**           to the original value.
**           Note: X||Y = x0y0x1y1...xnyn where xi and yi are ith byte
**           value of X and Y.
**
** Arguments:
**
** [f_pbContentKey]         -- Pointer to a content key that is shuffled.
**                             The same buffer receives the content key unshuffled.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
static DRM_RESULT  _UnshuffleContentKey(
    __inout_bcount( DRM_AES_KEYSIZE_128 * 2 )
        DRM_BYTE *f_pbContentKey )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_BYTE   rgbTmp[ DRM_AES_KEYSIZE_128 * 2 ];
    DRM_DWORD  i, j = 0;

    ChkArg( f_pbContentKey != NULL );

    for ( i = 0; i < DRM_AES_KEYSIZE_128; i++ )
    {
        rgbTmp[ i ] = f_pbContentKey[ j++ ];
        rgbTmp[ i + DRM_AES_KEYSIZE_128 ] = f_pbContentKey[ j++ ];
    }

    MEMCPY( f_pbContentKey, rgbTmp, DRM_AES_KEYSIZE_128 * 2 );

ErrorExit:
    OEM_SECURE_ZERO_MEMORY( rgbTmp, SIZEOF( rgbTmp ) );
    return dr;
}

/*****************************************************************************
** Function:    DRM_BBX_KF_DecryptKey
**
** Synopsis:    Decrypts data (a key) using a key stored in keyfile.
**
** Arguments:   [f_pOEMContext]     : A pointer to the OEM context; may be NULL
**              [f_pEncryptedKey]   : A structure containing encrypted key data
**              [f_eKeyType]        : Describes the key type being decrypted.
**                                    Different key types may need different
**                                    operations done on them. In this implementation
**                                    the type is ignored.
**              [f_pbDecryptedKey]  : A pointer to the buffer to receive the unique ID
**              [f_pcbDecryptedKey] : Buffer to receive the decrypted key of the size
**                                    indicated in the f_pcbEncryptedKey struct
**
** Returns:     DRM_SUCCESS          - on success
**              DRM_E_BUFFERTOOSMALL - if the buffer is NULL or too small
**              DRM_E_KEYFILE_UNKNOWN_DECRYPTION_METHOD - if invalid decryption method
**              if specified in the f_pcbEncryptedKey struct.
**
** Notes:       None.
**
******************************************************************************/

DRM_RESULT DRM_API DRM_CALL DRM_BBX_KF_DecryptKey (
    __in_opt                                 DRM_VOID          *f_pOEMContext,
    __in                                     DRM_ENCRYPTED_KEY *f_pEncryptedKey,
    __in                                     DRM_DEVICE_KEY_TYPE   f_eKeyType,
    __inout_bcount_opt( *f_pcbDecryptedKey ) DRM_BYTE          *f_pbDecryptedKey,
    __inout                                  DRM_DWORD         *f_pcbDecryptedKey
)
{
    DRM_RESULT   dr                                           = DRM_SUCCESS;
    DRM_AES_KEY  aesKey                                       = { 0 };
    DRM_BYTE     rgbDecryptBuffer[ KF_MAX_ENCRYPTION_BUFFER ] = { 0 };

    ChkArg(  f_pEncryptedKey   != NULL );
    ChkArg(  f_pcbDecryptedKey != NULL );

    if ( f_pbDecryptedKey   == NULL
      || *f_pcbDecryptedKey <  ( DRM_DWORD ) ( f_pEncryptedKey->wKeySize / BITS_PER_BYTE ) )
    {
        *f_pcbDecryptedKey = f_pEncryptedKey->wKeySize / BITS_PER_BYTE;
        ChkDR( DRM_E_BUFFERTOOSMALL );
    }

    /*
    ** If the Key is encrypted, get the encryption KeyID and then decrypt the key.
    */
    switch ( f_pEncryptedKey->wKeypairDecryptionMethod )
    {
        case KF_DECRYPTION_TYPE_AES_CBC:
            ChkDR( Oem_Keyfile_GetKFAesKey( &f_pEncryptedKey->guidKeypairDecryptionKeyID, &aesKey ) );

            /*
            ** Copy the data into the temp output buffer. then Decrypt it and return the correct portion.
            */

            if ( f_pEncryptedKey->cbEncryptedKey > KF_MAX_ENCRYPTION_BUFFER )
            {
                TRACE(("The key in the keyfile is too large to load into the decryption buffer."));
                ChkDR( DRM_E_KEYFILE_INTERNAL_DECRYPTION_BUFFER_TOO_SMALL );
            }

            MEMCPY( rgbDecryptBuffer,
                    f_pEncryptedKey->pbEncryptedKey,
                    f_pEncryptedKey->cbEncryptedKey );

            ChkDR( Oem_Aes_CbcDecryptData( &aesKey,
                                           rgbDecryptBuffer,
                                           f_pEncryptedKey->cbEncryptedKey,
                                           f_pEncryptedKey->rgbInitializationVector ) );

            MEMCPY( f_pbDecryptedKey,
                    rgbDecryptBuffer,
                    f_pEncryptedKey->wKeySize / BITS_PER_BYTE );
            *f_pcbDecryptedKey = f_pEncryptedKey->wKeySize / BITS_PER_BYTE;
            break;

        case KF_DECRYPTION_TYPE_CLEAR:
            if ( *f_pcbDecryptedKey < f_pEncryptedKey->cbEncryptedKey )
            {
                *f_pcbDecryptedKey = f_pEncryptedKey->cbEncryptedKey;
                ChkDR( DRM_E_BUFFERTOOSMALL );
            }
            else
            {
                MEMCPY( f_pbDecryptedKey,
                        f_pEncryptedKey->pbEncryptedKey,
                        f_pEncryptedKey->cbEncryptedKey );
                *f_pcbDecryptedKey = f_pEncryptedKey->cbEncryptedKey;
            }
            break;
        default:
            ChkDR( DRM_E_KEYFILE_UNKNOWN_DECRYPTION_METHOD );
    }


ErrorExit:
    Oem_Aes_ZeroKey( &aesKey );

    return dr;
}


/***********************************************************************************
** Function:    DRM_BBX_KF_EncryptKey
**
** Synopsis:    Encrypts data (a key) to be stored in the keyfile.
**
** Arguments:   [f_pOEMContext]   : A pointer to the OEM context; may be NULL
**              [f_pbClearKey]    : Buffer containing clear key to be encrypted
**              [f_cbClearKey]    : Size in bytes of the clear key to be encrypted
**              [f_eKeyType]      : See description for DRM_BBX_KF_DecryptKey
**              [f_pEncryptedKey] : Structure to be populated with encrypted key
**                                  and metadata about encryption.
**
** Returns:     DRM_SUCCESS          - on success
**              DRM_E_BUFFERTOOSMALL - if the buffer is NULL or too small
**
**
** Notes:       f_pEncryptedKey->pbEncryptedKey buffer to store encrypted key value
**              is already allocated by a caller.
***********************************************************************************/
DRM_RESULT DRM_API DRM_CALL DRM_BBX_KF_EncryptKey (
    __in_opt                        DRM_VOID                  *f_pOEMContext,
    __in_bcount(f_cbClearKey) const DRM_BYTE                  *f_pbClearKey,
    __in                            DRM_DWORD                  f_cbClearKey,
    __in                            DRM_DEVICE_KEY_TYPE        f_eKeyType,
    __out                           DRM_ENCRYPTED_KEY         *f_pEncryptedKey )
{
    DRM_RESULT   dr                = DRM_SUCCESS;
    DRM_AES_KEY  aesKey            = { 0 };
    DRM_DWORD    cbBufferedKeySize = 0;

    ChkArg( f_pbClearKey != NULL );
    ChkArg( f_cbClearKey > 0 );
    ChkArg( f_pEncryptedKey != NULL );

    /*
    ** The private key needs to be a multiple of 16 bytes. We will add padding if needed.
    */
    cbBufferedKeySize = f_cbClearKey;
    if ( cbBufferedKeySize % DRM_AES_BLOCKLEN != 0 )
    {
        cbBufferedKeySize += DRM_AES_BLOCKLEN - ( cbBufferedKeySize % DRM_AES_BLOCKLEN );
    }

    if ( f_pEncryptedKey->pbEncryptedKey == NULL
      || f_pEncryptedKey->cbEncryptedKey <  cbBufferedKeySize )
    {
        f_pEncryptedKey->cbEncryptedKey = cbBufferedKeySize;
        ChkDR( DRM_E_BUFFERTOOSMALL );
    }

    /*
    ** Fill-in the struct
    */
    f_pEncryptedKey->wKeypairDecryptionMethod = KF_DECRYPTION_TYPE_AES_CBC;
    f_pEncryptedKey->wKeySize = f_cbClearKey * BITS_PER_BYTE; /* size when decrypted */

    ChkDR( Oem_Keyfile_GetCurrentKFAesKeyId( &f_pEncryptedKey->guidKeypairDecryptionKeyID ) );

    ChkDR( Oem_Keyfile_GetKFAesKey(&f_pEncryptedKey->guidKeypairDecryptionKeyID, &aesKey ) );
    /*
    ** Generate random IV
    */
    ChkDR( Oem_Random_GetBytes( f_pOEMContext, f_pEncryptedKey->rgbInitializationVector, __CB_DECL( DRM_AES_BLOCKLEN ) ) );

    /*
    ** Generate random data for the encrypted key structure if it needs it.
    */
    if ( cbBufferedKeySize != f_cbClearKey )
    {
        ChkDR( Oem_Random_GetBytes( f_pOEMContext, f_pEncryptedKey->pbEncryptedKey, cbBufferedKeySize ) );
    }

    /*
    ** Copy the Clear key into the encryption buffer
    */
    MEMCPY( f_pEncryptedKey->pbEncryptedKey,
            f_pbClearKey,
            f_cbClearKey );

    f_pEncryptedKey->cbEncryptedKey = cbBufferedKeySize;

    ChkDR( Oem_Aes_CbcEncryptData( &aesKey,
                                   f_pEncryptedKey->pbEncryptedKey,
                                   f_pEncryptedKey->cbEncryptedKey,
                                   f_pEncryptedKey->rgbInitializationVector ) );

ErrorExit:
    Oem_Aes_ZeroKey(&aesKey);
    return dr;
}


/*****************************************************************************************************************
** Function:    DRM_BBX_KF_CalculateOmac
**
** Synopsis:    Creates an OMAC signature of the specified keyfile data
**              using the AES keyfile decryption key specified or the current AES keyfile decryption key.
**
** Arguments:   [f_pOEMContext]             : A pointer to the OEM context; may be NULL
**              [f_pguidAESDecryptionKeyID] : Specifies on input the AES decryption key ID to use
**                                            for calculating the OMAC if f_fUseCurrentKey is FALSE,
**                                            otherwise is undefined on input and contains
**                                            the current AES decryption key ID used in the OMAC on return.
**              [f_fUseCurrentKey]          : If FALSE, the AES key to use for the OMAC calculation is specified
**                                            by the f_pguidAESDecryptionKeyID parameter. If TRUE then
**                                            the current AES keyfile key is used instead
**                                            and returned in f_ pguidAESDecryptionKeyID.
**              [f_pbData]                  : Pointer to the keyfile data to be signed
**              [f_cbData]                  : Size in bytes of the keyfile data to be signed
**              [f_rgbSignatureBuffer]      : Buffer to receive the OMAC signature
*****************************************************************************************************************/
DRM_RESULT DRM_API DRM_CALL DRM_BBX_KF_CalculateOmac (
    __in_opt    DRM_VOID    *f_pOEMContext,
    __inout     DRM_GUID    *f_pguidAESDecryptionKeyID,
    __in        DRM_BOOL     f_fUseCurrentKey,
    __in_bcount(f_cbData) const DRM_BYTE *f_pbData,
    __in        DRM_DWORD    f_cbData,
    __inout_bcount( DRM_AES_BLOCKLEN ) DRM_BYTE f_rgbSignatureBuffer[DRM_AES_BLOCKLEN]
)
{
    DRM_RESULT  dr     = DRM_SUCCESS;
    DRM_AES_KEY aesKey = { 0 };

    ChkArg( f_pguidAESDecryptionKeyID != NULL ); /* even if undefined it should not be NULL */
    ChkArg( f_pbData != NULL );
    ChkArg( f_cbData > 0 );

    if ( f_fUseCurrentKey )
    {
        ChkDR( Oem_Keyfile_GetCurrentKFAesKeyId( f_pguidAESDecryptionKeyID ) );
    }
    ChkDR( Oem_Keyfile_GetKFAesKey( f_pguidAESDecryptionKeyID, &aesKey ) );

    ChkDR( Oem_Omac1_Sign( &aesKey, f_pbData, 0, f_cbData, f_rgbSignatureBuffer ) );

ErrorExit:
    Oem_Aes_ZeroKey( &aesKey );
    return dr;
}
EXIT_PK_NAMESPACE_CODE;

