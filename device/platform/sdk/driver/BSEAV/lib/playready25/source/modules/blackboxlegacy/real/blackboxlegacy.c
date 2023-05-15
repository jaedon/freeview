/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_BLACKBOXLEGACYREAL_C
#include <blackboxlegacy.h>
#include <blackboxinternal.h>
#include <oemeccp160.h>
#include <oemkeyfile.h>
#include <drmconstants.h>
#include <drmprofile.h>
#include <drmprofileconstants.h>
#include <drmwmdrm.h>
#include <drmlastinclude.h>

#if DRM_SUPPORT_SECUREOEMHAL
#error DRM_SUPPORT_SECUREOEMHAL is set.  You can not implement a secure HAL that supports blackbox legacy crypto.  You must link to the stub version of the blackboxlegacy module.
#endif /* DRM_SUPPORT_SECUREOEMHAL */

ENTER_PK_NAMESPACE_CODE;

/*
** Functions defining whether legacy (WMDRM) crypto keys and algorithms are supported
*/
DRM_API DRM_NO_INLINE DRM_BOOL DRM_CALL DRM_BBX_IsLegacyCryptoSupported(DRM_VOID)
{
    return TRUE;
}
DRM_API DRM_NO_INLINE DRM_BOOL DRM_CALL DRM_BBX_IsLegacyCryptoUnsupported(DRM_VOID)
{
    return FALSE;
}

#if DBG
/*********************************************************************
**
**  Function:  _TestDeviceKeyPair
**
**  Synopsis:  Test the current BlackBox context device key pair by    
**             encrypting and then decrypting a block of memory
**
**  Arguments:
**     [f_pcontextBBX] : Pointer to an initialized blackbox context
**
*********************************************************************/
DRM_RESULT DRM_CALL _TestDeviceKeyPair( 
    __in_ecount( 1 ) DRM_BB_CONTEXT *f_pcontextBBX  )
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

    if( MEMCMP(pbIn, pbOut, cbClear) != 0 )
    {
        ChkDR( DRM_E_KEY_MISMATCH );
    }

ErrorExit:
    OEM_ECC_ZeroPrivateKey_P160( &privkey );
    return dr;
}
#endif /* DBG */


/*  Verify the checksum on a content key. */
static DRM_RESULT _Legacy_VerifyChecksum(
    __in_bcount( f_cbKey ) const DRM_BYTE                *f_pbKey,
    __in                         DRM_DWORD                f_cbKey,
    __in_ecount( 1 )       const DRM_BB_CHECKSUM_CONTEXT *f_pChecksum,
    __inout_ecount( 1 )          DRM_CRYPTO_CONTEXT      *f_pCryptoContext )
{
    DRM_RESULT dr     = DRM_SUCCESS;
    DRM_BYTE   rgbTemp[__CB_DECL( max( LEGACY_KID_B64ENC_LEN, DRM_SHA1_DIGEST_LEN + 1 ) )] = { 0 };
    DRM_AES_KEY aesKey; /* Do not zero init key material */

    ChkArg( f_pChecksum      != NULL
         && f_pCryptoContext != NULL );

    ChkBOOL( f_pbKey != NULL && f_cbKey != 0, DRM_E_CH_BAD_KEY );

    ZEROMEM( rgbTemp, SIZEOF( rgbTemp ) );

    if( f_pChecksum->m_fIgnoreChecksum )
    {
        goto ErrorExit;
    }

    if( f_pChecksum->m_pbV1KID != NULL )
    {
        DRM_CIPHER_CONTEXT *pCipherContext = ( DRM_CIPHER_CONTEXT * )( f_pCryptoContext->rgbCryptoContext );

        ChkDR( DRM_CPHR_Init( pCipherContext, eDRM_RC4_CIPHER, f_cbKey, f_pbKey ) );

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
        DRM_SHA_CONTEXT *pShaVal = ( DRM_SHA_CONTEXT * )( f_pCryptoContext->rgbCryptoContext );

        /* Run SHA on key SHA_ITERATIONS times. */
        /* First copy key into rgbTemp. */
        if( f_cbKey > SIZEOF( rgbTemp ) )
        {
            f_cbKey = SIZEOF( rgbTemp ); /* Make sure we don't exceed buffer capacity in rgbTemp. */
        }

        MEMCPY( rgbTemp, f_pbKey, f_cbKey );

        for( lCount = 0; lCount < SHA_ITERATIONS; lCount++ )
        {
            ChkDR( DRM_SHA_Init( pShaVal, eDRM_SHA_1 ) );
            ChkDR( DRM_SHA_Update( rgbTemp, DRM_SHA1_DIGEST_LEN + 1, pShaVal ) );
            ChkDR( DRM_SHA_Finalize( pShaVal, DRM_SHA1_DIGEST_LEN, rgbTemp ) );
        }

        ChkBOOL( MEMCMP( rgbTemp, f_pChecksum->m_rgbChecksumData, CHECKSUM_LENGTH ) == 0,
                 DRM_E_CH_BAD_KEY );
    }

ErrorExit:
    OEM_SECURE_ZERO_MEMORY( &aesKey, SIZEOF( aesKey ) );

    return dr;
}


/*********************************************************************
**
**  Function:  _Legacy_GetClientId
**
**  Synopsis:  Return the WMDRM client ID.
**
**  Arguments:
**     [f_pClientId]   : returns the client ID
**     [f_pcontextBBX] : Pointer to an initialized blackbox context
**
*********************************************************************/
DRM_API DRM_RESULT DRM_CALL _Legacy_GetClientId(
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


/*********************************************************************
**
** Function: DRM_BBX_Legacy_SymmetricCipher
**
** Synopsis: Function that does symmetric ciphering (encrypt or decrypt)
**           of the passed in data using a symmetric key, which
**           is a key derived from a private key.
**
**           The private key is hashed using SHA-1,
**           and the hash is used as an RC4 key to cipher the
**           data.
**
** Arguments:
** [f_fEncrypt]         : Specifies that encryption should be done instead of decryption
**                        (ignored for RC4)
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
DRM_API DRM_RESULT DRM_CALL DRM_BBX_Legacy_SymmetricCipher(
    __in                                DRM_BOOL  f_fEncrypt,
    __in_bcount_opt( f_cbKey )   const  DRM_BYTE *f_pbKey,
    __in                                DRM_DWORD f_cbKey,
    __in_bcount( f_cbData )             DRM_BYTE *f_pbDataIn,
    __out_bcount_opt( f_cbData )        DRM_BYTE *f_pbDataOut,
    __in                                DRM_DWORD f_cbData )
{
    DRM_RESULT       dr                         = DRM_SUCCESS;
    DRM_SHA_CONTEXT  shaContext                 = DRM_SHA_CONTEXT_DEFAULT;
    DRM_BYTE         rgbDigest[__CB_DECL( DRM_SHA_DIGEST_LEN_MAX )] = { 0 };
    RC4_KEYSTRUCT    rc4Key                     = { 0 };

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_BLACKBOX, PERF_FUNC_DRM_BBX_Legacy_SymmetricCipher );

    ChkArg( f_pbDataIn  != NULL );
    ChkArg( f_pbKey     != NULL );
    ChkArg( f_cbData > 0 );
    ChkArg( f_cbKey > 0 );

    if( f_pbDataOut != NULL )
    {
        MEMCPY( f_pbDataOut, f_pbDataIn, f_cbData );
    }
    else
    {
        f_pbDataOut = f_pbDataIn;
    }

    /*
    **  1. Get a hash of the privkey
    **  2. Generate a RC4 key using this hash
    **  3. Decrypt ciphertext using this RC4 key
    */

    ChkDR( DRM_SHA_Init( &shaContext, eDRM_SHA_1 ) );
    ChkDR( DRM_SHA_Update( f_pbKey, f_cbKey, &shaContext ) );
    ChkDR( DRM_SHA_Finalize( &shaContext, DRM_SHA1_DIGEST_LEN, rgbDigest ) );

    DRM_RC4_KeySetup( &rc4Key, DRM_SHA1_DIGEST_LEN, rgbDigest );
    DRM_RC4_Cipher( &rc4Key, f_cbData, f_pbDataOut );

    DRM_RC4_ZeroKey( &rc4Key );

ErrorExit:
    OEM_SECURE_ZERO_MEMORY( &shaContext, SIZEOF( DRM_SHA_CONTEXT ) );
    OEM_SECURE_ZERO_MEMORY( rgbDigest, DRM_SHA_DIGEST_LEN_MAX );
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}


/*********************************************************************
**
**  Function:  _Legacy_SymmetricSign
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
DRM_API DRM_RESULT DRM_CALL _Legacy_SymmetricSign(
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
    if( f_pbExtraData != NULL )
    {
        ChkDR( DRM_HMAC_Update( &hmac, f_pbExtraData, f_cbExtraData ) );
    }
    ChkDR( DRM_HMAC_Finalize( &hmac, f_rgbSymSig, DRM_SHA1_DIGEST_LEN ) );

ErrorExit:
    OEM_ECC_ZeroPrivateKey_P160( &privkey );
    return dr;
}


/******************************************************************************
** Function :   _Legacy_SignData
** Synopsis :   Sign data with the machine private key
** Arguments :  f_pcontextBBX       - Blackbox context
**              f_pbData            - Data to be signed
**              f_cbData            - Size of data to be signed
**              f_rgbSignature      - Signature
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL _Legacy_SignData(
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
**  Function:  _Legacy_SymmetricVerify
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
DRM_API DRM_RESULT DRM_CALL _Legacy_SymmetricVerify(
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

    if( f_pslk == NULL )
    {
        ChkDR( DRM_HMAC_Init( &hmac, eDRM_SHA_1, ( DRM_BYTE * )&privkey, SIZEOF( privkey ) ) );
    }
    else
    {
        DRM_SLK slkClear = { 0 };

        ChkDR( DRM_BBX_Legacy_SymmetricCipher( SYMMETRIC_DECRYPT,
                                               ( DRM_BYTE * )&privkey,
                                               SIZEOF( privkey ),
                                               ( DRM_BYTE * ) f_pslk,
                                               ( DRM_BYTE * )&slkClear,
                                               DES_KEYSIZE ) );  /* Only decrypt the first 8 bytes */
        ChkDR( DRM_HMAC_Init( &hmac, eDRM_SHA_1, ( DRM_BYTE * )&slkClear, DES_KEYSIZE ) );
    }

    ChkDR( DRM_HMAC_Update( &hmac, f_pbData, f_cbData ) );
    if( f_pbExtraData != NULL )
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
**  Function:  _Legacy_DecryptLicense
**
**  Synopsis:  Decrypt a license that was encryted for privacy reasons (not
**             security reasons) while traveling on the net during license
**             acquisition.
**
**  Arguments:
**     [f_pbEncryptedLicense] -- Pointer to an encrypted license buffer
**     [f_cbEncryptedLicense] -- Size, in bytes, of license buffer
**     [f_pbLicense]          -- returns the decrypted license
**     [f_pcontextBBX]        -- Pointer to an initialized blackbox context
**     [f_pDevPubKey]         -- Returns public key of decyrpting private key.
**
*********************************************************************/
DRM_API DRM_BOOL DRM_CALL _Legacy_DecryptLicense(
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

    if( GET_BYTE(rgbKey, 1) != PKSYMM_ALG_TYPE_RC4 )
    {
        ChkDR( DRM_E_UNSUPPORTED_ALGORITHM );
    }

    f_cbEncryptedLicense -= DRM_ECC160_CIPHERTEXT_LEN;

    if( f_pbLicense == NULL )
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


/*********************************************************************
**
**  Function:  _AsymmetricToSymmetricKeyXML
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
DRM_API DRM_RESULT DRM_CALL _AsymmetricToSymmetricKeyXML(
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

    ChkDR( DRM_BBX_Legacy_SymmetricCipher( SYMMETRIC_ENCRYPT,
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
**  Function:  _DecryptContentKeyXML
**
**  Synopsis:  Decrypt a content key and place it in the given key buffer.
**
**  Arguments:
**     [f_pXmlBindInfo]   : Pointer to XML Bind info structure(s) defining
**                          the license chain to extract the key from
**     [f_dwChainLen]     : length of the license chain
**     [f_pcontextBBX]    : Pointer to an initialized blackbox context
**     [f_IgnoreChecksum] : Specifies whether to ignore checksum over content key.
**     [f_IgnoreCKSig]    : Specifies whether to ignore signature over content key.
**     [f_cbKeyBuff]      : the size of the key buffer.
**     [f_pbKeyBuff]      : the key buffer.
**     [f_pcbKey]         : the size of the key.
**     [f_pPrivKey]       : return the private key that was used to decrypt the content key.
**
*********************************************************************/
DRM_RESULT _DecryptContentKeyXML(
    __in_ecount( f_dwChainLen ) const DRM_BINDING_INFO    *f_pBindingInfo,
    __in                              DRM_DWORD            f_dwChainLen,
    __inout_ecount( 1 )               DRM_BB_CONTEXT      *f_pcontextBBX,
    __in                              DRM_BOOL             f_IgnoreChecksum,
    __in                              DRM_BOOL             f_IgnoreCKSig,
    __in                              DRM_DWORD            f_cbKeyBuff,
    __out_bcount( f_cbKeyBuff )       DRM_BYTE            *f_pbKeyBuff,
    __out_ecount( 1 )                 DRM_DWORD           *f_pcbKey,
    __out_opt                         PRIVKEY             *f_pPrivKey )
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
          || pXmlBindingInfo->m_dwAlgorithm == eMSDRM_SYM, DRM_E_UNSUPPORTED_ALGORITHM );

    dr = _GetDevicePrivkey( f_pcontextBBX, &( pXmlBindingInfo->m_oPublKey ), &privkey );
    if( dr == DRM_E_KEYFILE_PRIVATE_KEY_NOT_FOUND )
    {
        ChkDR( DRM_E_UNKNOWN_BINDING_KEY );
    }
    ChkDR( dr );


    if( f_pBindingInfo[0].m_BindingInfo.m_XML.m_dwAlgorithm == eMSDRM_SYM )
    {
        /* Even if this is a license chain, if the leaf license is symmetrically optimized
           we can directly decrypt it */
        f_dwChainLen = 1;
        pXmlBindingInfo = &( f_pBindingInfo[0].m_BindingInfo.m_XML );
    }

    /*
    ** Decrypt the content key with that private key.
    */
    if( pXmlBindingInfo->m_dwAlgorithm == eMSDRM_PK )
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
        ChkDR( DRM_BBX_Legacy_SymmetricCipher( SYMMETRIC_DECRYPT,
                                               ( DRM_BYTE * )&privkey,
                                               SIZEOF( privkey ),
                                               ( DRM_BYTE * )pXmlBindingInfo->m_rgbContentKey,
                                               rgbDecContKey,
                                               SIZEOF( rgbDecContKey ) ) );
    }

    bSize = GET_BYTE( rgbDecContKey, 0 );
    /*
    ** The size byte should have a value of at most 15.
    */
    ChkBOOL( bSize < SIZEOF( rgbDecContKey ), DRM_E_CH_BAD_KEY );
    ChkBOOL( bSize != 0, DRM_E_CH_BAD_KEY );

    DRM_BYT_MoveBytes( rgbDecContKey, 0, rgbDecContKey, 1, SIZEOF( rgbDecContKey ) - 1);

    if( !f_IgnoreChecksum )
    {
        ChkDR( _Legacy_VerifyChecksum( rgbDecContKey,
                                       bSize,
                                       &f_pBindingInfo[f_dwChainLen-1].m_oChecksum,
                                       &f_pcontextBBX->CryptoContext ) );
    }

    /*
    ** Verify the key signature.
    */
    if( pXmlBindingInfo->m_dwAlgorithm == eMSDRM_PK
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

    for( f_dwChainLen--; f_dwChainLen > 0; f_dwChainLen-- )
    {
        pXmlBindingInfo = &f_pBindingInfo[f_dwChainLen-1].m_BindingInfo.m_XML;

        /*
        ** We are now in chained licenses enabling bits.  The algorithm *must* be eMSDRM_CK
        */
        ChkBOOL( pXmlBindingInfo->m_dwAlgorithm == eMSDRM_CK
              && bSize == DES_KEYSIZE,
                 DRM_E_UNSUPPORTED_ALGORITHM );

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
        ChkBOOL( bSize < SIZEOF( rgbDecContKey ), DRM_E_CH_BAD_KEY );

        DRM_BYT_MoveBytes( rgbDecContKey, 0, rgbDecContKey, 1, SIZEOF( rgbDecContKey ) - 1 );

        if( !f_IgnoreChecksum )
        {
            ChkDR( _Legacy_VerifyChecksum( rgbDecContKey,
                                           bSize,
                                           &f_pBindingInfo[f_dwChainLen-1].m_oChecksum,
                                           &f_pcontextBBX->CryptoContext ) );
        }

        /*
        ** Verify the key signature.
        */
        if( !f_IgnoreCKSig )
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
    if( f_pPrivKey != NULL )
    {
        MEMCPY( f_pPrivKey, &privkey, SIZEOF( privkey ) );
    }

ErrorExit:
    OEM_ECC_ZeroPrivateKey_P160( &privkey );
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}


/*********************************************************************
**
**  Function:  _GetDevicePrivkey
**
**  Synopsis:  Return the private key of the current key pair, or for
**             the specified public key.
**
**  Arguments:
**     [f_pcontextBBX] : Pointer to an initialized blackbox context
**     [f_pDevPubKey]  : Specifies the public key of the key pair.
**     [f_pDevPrivKey] : returns the private key.
**
*********************************************************************/
DRM_RESULT _GetDevicePrivkey(
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

    if( f_pDevPubKey == NULL )
    {
        /* Use the current device public key. */
        f_pDevPubKey = &f_pcontextBBX->cachedCertValues.pubkeyCurrent;
    }

    ChkDR( DRM_KF_GetPrivateKey( f_pcontextBBX->pKeyFileContext,
                                 eKF_KEY_TYPE_ECC_160,
                                 ( const DRM_BYTE * )f_pDevPubKey,
                                 SIZEOF( PUBKEY ),
                                 &oEncKey ) );

    ChkDR( DRM_BBX_KF_Legacy_DecryptKey( f_pcontextBBX->pOEMContext,
                                         &oEncKey,
                                         DRM_DKT_WMDRMPD_DEVICE,  /*Decrypting device key*/
                                         ( DRM_BYTE * )f_pDevPrivKey,
                                         &cbDecKey ) );
ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}


/*****************************************************************************
** Function:    _KF_Legacy_DecryptKey
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
DRM_RESULT DRM_API DRM_CALL _KF_Legacy_DecryptKey(
    __in_opt                                       DRM_VOID             *f_pOEMContext,
    __in                                     const DRM_ENCRYPTED_KEY    *f_pEncryptedKey,
    __in                                           DRM_DEVICE_KEY_TYPE   f_eKeyType,
    __inout_bcount_opt( *f_pcbDecryptedKey )       DRM_BYTE             *f_pbDecryptedKey,
    __inout                                        DRM_DWORD            *f_pcbDecryptedKey )
{
    DRM_RESULT   dr                                           = DRM_SUCCESS;
    DRM_AES_KEY  aesKey                                       = { 0 };
    DRM_BYTE     rgbDecryptBuffer[ KF_MAX_ENCRYPTION_BUFFER ] = { 0 };

    ChkArg(  f_pEncryptedKey   != NULL );
    ChkArg(  f_pcbDecryptedKey != NULL );

    if( f_pbDecryptedKey   == NULL
     || *f_pcbDecryptedKey <  ( DRM_DWORD )( f_pEncryptedKey->wKeySize / BITS_PER_BYTE ) )
    {
        *f_pcbDecryptedKey = ( DRM_DWORD )( f_pEncryptedKey->wKeySize / BITS_PER_BYTE );
        ChkDR( DRM_E_BUFFERTOOSMALL );
    }

    /*
    ** If the Key is encrypted, get the encryption KeyID and then decrypt the key.
    */
    switch( f_pEncryptedKey->wKeypairDecryptionMethod )
    {
        case KF_DECRYPTION_TYPE_AES_CBC:
            ChkDR( Oem_Keyfile_GetKFAesKey( &f_pEncryptedKey->guidKeypairDecryptionKeyID, &aesKey ) );

            /*
            ** Copy the data into the temp output buffer. then Decrypt it and return the correct portion.
            */

            if( f_pEncryptedKey->cbEncryptedKey > KF_MAX_ENCRYPTION_BUFFER )
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
                    ( DRM_SIZE_T )( f_pEncryptedKey->wKeySize / BITS_PER_BYTE ) );
            *f_pcbDecryptedKey = ( DRM_DWORD )( f_pEncryptedKey->wKeySize / BITS_PER_BYTE );
            break;

        case KF_DECRYPTION_TYPE_CLEAR:
            if( *f_pcbDecryptedKey < f_pEncryptedKey->cbEncryptedKey )
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
** Function:    _KF_Legacy_EncryptKey
**
** Synopsis:    Encrypts data (a key) to be stored in the keyfile.
**
** Arguments:   [f_pOEMContext]   : A pointer to the OEM context; may be NULL
**              [f_pbClearKey]    : Buffer containing clear key to be encrypted
**              [f_cbClearKey]    : Size in bytes of the clear key to be encrypted
**              [f_eKeyType]      : See description for DRM_BBX_KF_Legacy_DecryptKey
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
DRM_RESULT DRM_API DRM_CALL _KF_Legacy_EncryptKey(
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
    ChkArg( f_cbClearKey < ( MAX_UNSIGNED_TYPE(DRM_WORD) / BITS_PER_BYTE ) );

    /*
    ** The private key needs to be a multiple of 16 bytes. We will add padding if needed.
    */
    cbBufferedKeySize = f_cbClearKey;
    if( cbBufferedKeySize % DRM_AES_BLOCKLEN != 0 )
    {
        cbBufferedKeySize += DRM_AES_BLOCKLEN - ( cbBufferedKeySize % DRM_AES_BLOCKLEN );
    }

    if( f_pEncryptedKey->pbEncryptedKey == NULL
     || f_pEncryptedKey->cbEncryptedKey <  cbBufferedKeySize )
    {
        f_pEncryptedKey->cbEncryptedKey = cbBufferedKeySize;
        ChkDR( DRM_E_BUFFERTOOSMALL );
    }

    /*
    ** Fill-in the struct
    */
    f_pEncryptedKey->wKeypairDecryptionMethod = KF_DECRYPTION_TYPE_AES_CBC;
    f_pEncryptedKey->wKeySize = ( DRM_WORD )( f_cbClearKey * BITS_PER_BYTE ); /* size when decrypted - overflow checked by ChkArg( f_cbClearKey < ( MAX_UNSIGNED_TYPE(DRM_WORD) / BITS_PER_BYTE ) ); */

    ChkDR( Oem_Keyfile_GetCurrentKFAesKeyId( &f_pEncryptedKey->guidKeypairDecryptionKeyID ) );

    ChkDR( Oem_Keyfile_GetKFAesKey(&f_pEncryptedKey->guidKeypairDecryptionKeyID, &aesKey ) );
    /*
    ** Generate random IV
    */
    ChkDR( Oem_Random_GetBytes( f_pOEMContext, f_pEncryptedKey->rgbInitializationVector, __CB_DECL( DRM_AES_BLOCKLEN ) ) );

    /*
    ** Generate random data for the encrypted key structure if it needs it.
    */
    if( cbBufferedKeySize != f_cbClearKey )
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


/*********************************************************************
**
** Function: _Legacy_GenerateECCKeyPair
**
** Synopsis:
**           Generation of keypairs is an important operation from
**           robustness perspective - private key of is one
**           of the most important robustness assets. Blackbox.c file
**           and oem files are the source files that should be the
**           only ones to be edited to increase the protection of
**           robustness assets.
**           In the given implementation this function just calls
**           a plain one to generate a P160 ECC key pair.
**           It should be called whenever a P160 key pair for
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
DRM_API DRM_RESULT DRM_CALL _Legacy_GenerateECCKeyPair(
    __inout                             DRM_CRYPTO_CONTEXT                 *f_pCryptoCtx,
    __in                                DRM_PKCRYPTO_SUPPORTED_ALGORITHMS   f_eAlgorithm,
    __inout_ecount( 1 )                 DRM_DWORD                          *f_pcbPublicKey,
    __out_bcount( *f_pcbPublicKey )     DRM_BYTE                           *f_pbPublicKey,
    __inout_ecount( 1 )                 DRM_DWORD                          *f_pcbPrivateKey,
    __out_bcount( *f_pcbPrivateKey )    DRM_BYTE                           *f_pbPrivateKey)
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pCryptoCtx!= NULL );
    ChkArg( eDRM_ECC_P160 == f_eAlgorithm );

    ChkDR( DRM_PK_GenKeyPair(f_pCryptoCtx->rgbCryptoContext,
            f_eAlgorithm,
            eDRM_RSA_KEY_LENGTH_UNKNOWN_2048BIT,
            f_pcbPublicKey,
            f_pbPublicKey,
            f_pcbPrivateKey,
            f_pbPrivateKey));

ErrorExit:
    return dr;
}


EXIT_PK_NAMESPACE_CODE;

