/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/
#include "stdafx.h"
#include <xmrlicgen.h>

ENTER_PKTEST_NAMESPACE_CODE

PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_USE_WIDE_API, "Ignore prefast warning about ANSI functions for this entire file.")

/* No existing client-side constant: client ignores this value */
#define SESSION_KEY_TYPE                        1

/* Fixed: 0x90 bytes */
#define SESSION_KEY_OBJECT_SIZE                 ( ECC_P256_CIPHERTEXT_SIZE_IN_BYTES                             + 2 * SIZEOF(DRM_DWORD) + 4 * SIZEOF(DRM_WORD) )

/* Fixed: 0x34 bytes */
#define DOMAIN_KEY_OBJECT_SIZE                  ( ECC_P256_PRIVKEY_SIZE_IN_BYTES                                + 3 * SIZEOF(DRM_DWORD) + 4 * SIZEOF(DRM_WORD) )

/* Varies: e.g. 5 keys == 5*0x34+8 == 0x10c bytes */
#define DOMAIN_KEY_CONTAINER_SIZE(cKeys)        ( (cKeys) * DOMAIN_KEY_OBJECT_SIZE                              + 1 * SIZEOF(DRM_DWORD) + 2 * SIZEOF(DRM_WORD) )

/* Varies: e.g. 5 keys == 0x10c + 0x90 + 0x10 == 0x1ac bytes */
#define DOMAIN_KEY_GLOBAL_SiZE(cKeys)           ( DOMAIN_KEY_CONTAINER_SIZE(cKeys) + SESSION_KEY_OBJECT_SIZE    + 4 * SIZEOF(DRM_DWORD) + 0 * SIZEOF(DRM_WORD) )

static DRM_RESULT DRM_CALL _SerializeDomainKeys(
    __in                                          DRM_BB_CONTEXT     *f_poBBXContext,
    __in                                          DRM_DWORD           f_cDomainRevisions,
    __in_ecount(f_cDomainRevisions)         const DRM_DWORD          *f_rgdwDomainRevisions,
    __in_ecount(f_cDomainRevisions)               DRM_BYTE          **f_prgbClearDomainKeys,
    __inout_ecount_opt( *f_pcbDomainKeys )        DRM_BYTE           *f_pbDomainKeys,
    __inout                                       DRM_DWORD          *f_pcbDomainKeys )
{
    DRM_RESULT          dr                      = DRM_SUCCESS;
    PLAINTEXT_P256      oClearSessionKeys       = {0};
    CIPHERTEXT_P256     oEncryptedSessionKeys   = {0};
    DRM_DWORD           icb                     = 0;
    DRM_DWORD           idx                     = 0;
    DRM_AES_KEY         oAESKeyCI               = {0};
    DRM_AES_KEY         oAESKeyCK               = {0};

    DRMCASSERT( SIZEOF( oClearSessionKeys )     == DRM_AES_KEYSIZE_128 * 2 );
    DRMCASSERT( SIZEOF( oClearSessionKeys )     == ECC_P256_PRIVKEY_SIZE_IN_BYTES );
    DRMCASSERT( SIZEOF( oEncryptedSessionKeys ) == ECC_P256_CIPHERTEXT_SIZE_IN_BYTES );

    AssertChkArg( f_poBBXContext        != NULL );
    AssertChkArg( f_cDomainRevisions     > 0    );
    AssertChkArg( f_rgdwDomainRevisions != NULL );
    AssertChkArg( f_prgbClearDomainKeys != NULL );
    AssertChkArg( f_pcbDomainKeys       != NULL );
    AssertChkArg( ( f_pbDomainKeys == NULL ) == ( *f_pcbDomainKeys == 0 ) );

    if( *f_pcbDomainKeys < DOMAIN_KEY_GLOBAL_SiZE(f_cDomainRevisions) )
    {
        *f_pcbDomainKeys = DOMAIN_KEY_GLOBAL_SiZE(f_cDomainRevisions);
        ChkDR( DRM_E_BUFFERTOOSMALL );
    }

    /* Randomly generate session keys */
    do
    {
        ChkDR( Oem_Random_GetBytes( NULL, (DRM_BYTE*)&oClearSessionKeys, SIZEOF( oClearSessionKeys ) ) );

        dr = OEM_ECC_GenerateHMACKey_P256( &oClearSessionKeys, (struct bigctx_t *)f_poBBXContext->CryptoContext.rgbCryptoContext );

    } while( dr == DRM_E_P256_HMAC_KEYGEN_FAILURE );

    /* Set up session keys for later encryption */
    ChkDR( Oem_Aes_SetKey(((DRM_BYTE*)&oClearSessionKeys), &oAESKeyCI) );
    ChkDR( Oem_Aes_SetKey(((DRM_BYTE*)&oClearSessionKeys) + DRM_AES_KEYSIZE_128, &oAESKeyCK) );

    /* Encrypt session keys with BBX pubkey */
    ChkDR( OEM_ECC_Encrypt_P256(
        &(f_poBBXContext->cachedBCertValues.DeviceKeyEncrypt.PublicKeyContext.PublicKey),
        &oClearSessionKeys,
        &oEncryptedSessionKeys,
        (struct bigctx_t *)f_poBBXContext->CryptoContext.rgbCryptoContext ) );

    /* Build domain key header */
    DWORD_TO_NETWORKBYTES( f_pbDomainKeys, icb, DRM_DOMKEYXMR_HEADER_CONSTANT );
    icb += SIZEOF(DRM_DWORD);
    DWORD_TO_NETWORKBYTES( f_pbDomainKeys, icb, DRM_DOMKEYXMR_HEADER_VERSION );
    icb += SIZEOF(DRM_DWORD);
    DWORD_TO_NETWORKBYTES( f_pbDomainKeys, icb, DOMAIN_KEY_GLOBAL_SiZE(f_cDomainRevisions) );
    icb += SIZEOF(DRM_DWORD);
    DWORD_TO_NETWORKBYTES( f_pbDomainKeys, icb, 0 ); /* Signed length value is not used */
    icb += SIZEOF(DRM_DWORD);

    /* Build domain key session key */
    WORD_TO_NETWORKBYTES( f_pbDomainKeys, icb, XMR_FLAGS_MUST_UNDERSTAND );
    icb += SIZEOF(DRM_WORD);
    WORD_TO_NETWORKBYTES( f_pbDomainKeys, icb, DRM_DOMKEYXMR_OBJTYPE_SESSIOINKEY );
    icb += SIZEOF(DRM_WORD);
    DWORD_TO_NETWORKBYTES( f_pbDomainKeys, icb, SESSION_KEY_OBJECT_SIZE );
    icb += SIZEOF(DRM_DWORD);
    WORD_TO_NETWORKBYTES( f_pbDomainKeys, icb, SESSION_KEY_TYPE );
    icb += SIZEOF(DRM_WORD);
    WORD_TO_NETWORKBYTES( f_pbDomainKeys, icb, DRM_DOMKEYXMR_SESSIONKEY_ENCTYPE_ECCP256 );
    icb += SIZEOF(DRM_WORD);
    DWORD_TO_NETWORKBYTES( f_pbDomainKeys, icb, ECC_P256_CIPHERTEXT_SIZE_IN_BYTES );
    icb += SIZEOF(DRM_DWORD);
    DRMCRT_memcpy( &f_pbDomainKeys[icb], &oEncryptedSessionKeys, ECC_P256_CIPHERTEXT_SIZE_IN_BYTES );
    icb += ECC_P256_CIPHERTEXT_SIZE_IN_BYTES;

    /* Build domain private key container */
    WORD_TO_NETWORKBYTES( f_pbDomainKeys, icb, XMR_FLAGS_MUST_UNDERSTAND | XMR_FLAGS_CONTAINER );
    icb += SIZEOF(DRM_WORD);
    WORD_TO_NETWORKBYTES( f_pbDomainKeys, icb, DRM_DOMKEYXMR_OBJTYPE_PRIVKEYCONTAINER );
    icb += SIZEOF(DRM_WORD);
    DWORD_TO_NETWORKBYTES( f_pbDomainKeys, icb, DOMAIN_KEY_CONTAINER_SIZE(f_cDomainRevisions) );
    icb += SIZEOF(DRM_DWORD);

    /* Build domain private keys */
    for( idx = 0; idx < f_cDomainRevisions; idx++ )
    {
        AssertChkArg( f_prgbClearDomainKeys[idx] != NULL );

        WORD_TO_NETWORKBYTES( f_pbDomainKeys, icb, XMR_FLAGS_MUST_UNDERSTAND );
        icb += SIZEOF(DRM_WORD);
        WORD_TO_NETWORKBYTES( f_pbDomainKeys, icb, DRM_DOMKEYXMR_OBJTYPE_PRIVKEY );
        icb += SIZEOF(DRM_WORD);
        DWORD_TO_NETWORKBYTES( f_pbDomainKeys, icb, DOMAIN_KEY_OBJECT_SIZE );
        icb += SIZEOF(DRM_DWORD);
        DWORD_TO_NETWORKBYTES( f_pbDomainKeys, icb, f_rgdwDomainRevisions[idx] );
        icb += SIZEOF(DRM_DWORD);
        WORD_TO_NETWORKBYTES( f_pbDomainKeys, icb, DRM_DOMKEYXMR_PRIVKEY_TYPE_ECCP256 );
        icb += SIZEOF(DRM_WORD);
        WORD_TO_NETWORKBYTES( f_pbDomainKeys, icb, DRM_DOMKEYXMR_PRIVKEY_ENCTYPE_MIXED_AESECB );
        icb += SIZEOF(DRM_WORD);
        DWORD_TO_NETWORKBYTES( f_pbDomainKeys, icb, ECC_P256_PRIVKEY_SIZE_IN_BYTES );
        icb += SIZEOF(DRM_DWORD);
        DRMCRT_memcpy( &f_pbDomainKeys[icb], f_prgbClearDomainKeys[idx], DRM_AES_KEYSIZE_128 * 2 );  /* Domain keys are encrypted with session keys */
        ChkDR( Oem_Aes_EcbEncryptData( &oAESKeyCI, &f_pbDomainKeys[icb], DRM_AES_KEYSIZE_128 ) );
        icb += DRM_AES_KEYSIZE_128;
        ChkDR( Oem_Aes_EcbEncryptData( &oAESKeyCK, &f_pbDomainKeys[icb], DRM_AES_KEYSIZE_128 ) );
        icb += DRM_AES_KEYSIZE_128;
    }

    DRMASSERT( icb == DOMAIN_KEY_GLOBAL_SiZE(f_cDomainRevisions) );
    *f_pcbDomainKeys = DOMAIN_KEY_GLOBAL_SiZE(f_cDomainRevisions);

ErrorExit:
    return dr;
}

const DRM_CHAR rgchDomainJoinStart[] = "<?xml version=\"1.0\" encoding=\"utf-8\"?><soap:Envelope xmlns:soap=\"http://schemas.xmlsoap.org/soap/envelope/\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\"><soap:Body><JoinDomainResponse xmlns=\"http://schemas.microsoft.com/DRM/2007/03/protocols\"><JoinDomainResult><Response xmlns=\"http://schemas.microsoft.com/DRM/2007/03/protocols/messages\"><Domain xmlns=\"http://schemas.microsoft.com/DRM/2007/03/protocols\">";
const DRM_CHAR rgchCustomDataOpen[] = "<CustomData>";
const DRM_CHAR rgchCustomDataClose[] = "</CustomData>";
const DRM_CHAR rgchVersionOpen[] = "<Version>";
const DRM_CHAR rgchVersionClose[] = "</Version>";
const DRM_CHAR rgchAccountIDOpen[] = "<AccountID>";
const DRM_CHAR rgchAccountIDClose[] = "</AccountID>";
const DRM_CHAR rgchServiceIDOpen[] = "<ServiceID>";
const DRM_CHAR rgchServiceIDClose[] = "</ServiceID>";
const DRM_CHAR rgchCertificateChainOpen[] = "<CertificateChain>";
const DRM_CHAR rgchCertificateChainClose[] = "</CertificateChain>";
const DRM_CHAR rgchDomainKeysOpen[] = "<DomainKeys>";
const DRM_CHAR rgchDomainKeysClose[] = "</DomainKeys>";
const DRM_CHAR rgchDomainJoinEnd[] = "</Domain></Response></JoinDomainResult></JoinDomainResponse></soap:Body></soap:Envelope>";

#define COPY_CONST_STRING( pb, icb, rgch )                      \
    do {                                                        \
        DRMCRT_memcpy( &pb[icb], rgch, SIZEOF( rgch ) - 1 );    \
        icb += SIZEOF( rgch ) - 1;                              \
    } while(FALSE)

#define COPY_SZ_STRING( pb, icb, psz )                          \
    do {                                                        \
        DRM_DWORD cch = (DRM_DWORD)DRMCRT_strlen(psz);          \
        DRMCRT_memcpy( &pb[icb], psz, cch );                    \
        icb += cch;                                             \
    } while(FALSE)

DRM_RESULT DRM_CALL Test_CreateNewDomainJoinResponse(
    __in                                          DRM_BB_CONTEXT     *f_poBBXContext,
    __in_z                                  const DRM_CHAR           *f_pszDomainJoinResponseVariableName,
    __in_z_opt                              const DRM_CHAR           *f_pszCustomData,
    __in_z                                  const DRM_CHAR           *f_pszVersion,
    __in_z                                  const DRM_CHAR           *f_pszAccountID,
    __in_z                                  const DRM_CHAR           *f_pszServiceID,
    __in_z                                  const DRM_CHAR           *f_pszCertificateChain,
    __in                                          DRM_DWORD           f_cDomainRevisions,
    __in_ecount(f_cDomainRevisions)         const DRM_DWORD          *f_rgdwDomainRevisions,
    __in_ecount(f_cDomainRevisions)               DRM_BYTE          **f_prgbClearDomainKeys )
{
    DRM_RESULT   dr             = DRM_SUCCESS;
    DRM_DWORD    cchResponse    = 0;
    DRM_BYTE    *pbResponse     = NULL;
    DRM_DWORD    icbResponse    = 0;
    DRM_DWORD    cbKeys         = DOMAIN_KEY_GLOBAL_SiZE( f_cDomainRevisions );
    DRM_BYTE    *pbKeys         = NULL;
    DRM_DWORD    cchKeys        = CCH_BASE64_EQUIV( cbKeys );
    DRM_DWORD    cchLeft        = 0;

    ChkMem( pbKeys = (DRM_BYTE*)Oem_MemAlloc( cbKeys ) );

    ChkDR( _SerializeDomainKeys(
        f_poBBXContext,
        f_cDomainRevisions,
        f_rgdwDomainRevisions,
        f_prgbClearDomainKeys,
        pbKeys,
        &cbKeys ) );

    cchResponse += 0
      + SIZEOF( rgchDomainJoinStart ) - 1;

    if( f_pszCustomData != NULL )
    {
        cchResponse += 0
          + SIZEOF( rgchCustomDataOpen ) - 1
          + (DRM_DWORD)DRMCRT_strlen(f_pszCustomData)
          + SIZEOF( rgchCustomDataClose ) - 1;
    }

    cchResponse += 0
      + SIZEOF( rgchVersionOpen ) - 1
      + (DRM_DWORD)DRMCRT_strlen(f_pszVersion)
      + SIZEOF( rgchVersionClose ) - 1;

    cchResponse += 0
      + SIZEOF( rgchAccountIDOpen ) - 1
      + (DRM_DWORD)DRMCRT_strlen(f_pszAccountID)
      + SIZEOF( rgchAccountIDClose ) - 1;

    cchResponse += 0
      + SIZEOF( rgchServiceIDOpen ) - 1
      + (DRM_DWORD)DRMCRT_strlen(f_pszServiceID)
      + SIZEOF( rgchServiceIDClose ) - 1;

    cchResponse += 0
      + SIZEOF( rgchCertificateChainOpen ) - 1
      + (DRM_DWORD)DRMCRT_strlen(f_pszCertificateChain)
      + SIZEOF( rgchCertificateChainClose ) - 1;

    cchResponse += 0
      + SIZEOF( rgchDomainKeysOpen ) - 1
      + cchKeys
      + SIZEOF( rgchDomainKeysClose ) - 1;

    cchResponse += 0
      + SIZEOF( rgchDomainJoinEnd ) - 1;

    MAP_TO_GLOBAL_TEST_VAR_Alloc( pbResponse, f_pszDomainJoinResponseVariableName, cchResponse );

    COPY_CONST_STRING( pbResponse, icbResponse, rgchDomainJoinStart );

    if( f_pszCustomData != NULL )
    {
        COPY_CONST_STRING( pbResponse, icbResponse, rgchCustomDataOpen );
        COPY_SZ_STRING(    pbResponse, icbResponse, f_pszCustomData );
        COPY_CONST_STRING( pbResponse, icbResponse, rgchCustomDataClose );
    }

    COPY_CONST_STRING( pbResponse, icbResponse, rgchVersionOpen );
    COPY_SZ_STRING(    pbResponse, icbResponse, f_pszVersion );
    COPY_CONST_STRING( pbResponse, icbResponse, rgchVersionClose );

    COPY_CONST_STRING( pbResponse, icbResponse, rgchAccountIDOpen );
    COPY_SZ_STRING(    pbResponse, icbResponse, f_pszAccountID );
    COPY_CONST_STRING( pbResponse, icbResponse, rgchAccountIDClose );

    COPY_CONST_STRING( pbResponse, icbResponse, rgchServiceIDOpen );
    COPY_SZ_STRING(    pbResponse, icbResponse, f_pszServiceID );
    COPY_CONST_STRING( pbResponse, icbResponse, rgchServiceIDClose );

    COPY_CONST_STRING( pbResponse, icbResponse, rgchCertificateChainOpen );
    COPY_SZ_STRING(    pbResponse, icbResponse, f_pszCertificateChain );
    COPY_CONST_STRING( pbResponse, icbResponse, rgchCertificateChainClose );

    COPY_CONST_STRING( pbResponse, icbResponse, rgchDomainKeysOpen );
    cchLeft = cchResponse - icbResponse;
    ChkDR( DRM_B64_EncodeA( pbKeys, cbKeys, (DRM_CHAR*)&pbResponse[icbResponse], &cchLeft, 0 ) );
    icbResponse += cchLeft;
    COPY_CONST_STRING( pbResponse, icbResponse, rgchDomainKeysClose );

    COPY_CONST_STRING( pbResponse, icbResponse, rgchDomainJoinEnd );

ErrorExit:
    SAFE_OEM_FREE( pbKeys );
    return dr;
}

PREFAST_POP /* __WARNING_USE_WIDE_API */

EXIT_PKTEST_NAMESPACE_CODE

