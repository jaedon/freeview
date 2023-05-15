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
#include <oemsha1.h>
#include <drmkeygenerator.h>

ENTER_PK_NAMESPACE_CODE;

DRM_API DRM_RESULT DRM_CALL DRM_KG_GenerateKeyseed(
    IN                                 DRM_VOID  *f_pOEMContext,
    __out_ecount_opt( *f_pcchKeyseed ) DRM_WCHAR *f_pwszKeyseed,
    IN OUT                             DRM_DWORD *f_pcchKeyseed )
{
    DRM_RESULT dr=DRM_SUCCESS;
    DRM_BYTE rgbKeyseed [__CB_DECL(DRM_KEYSEED_BASE64_LEN)];
    DRM_LONG i=0;

    ChkArg(f_pcchKeyseed);
    if ( *f_pcchKeyseed<(DRM_KEYSEED_BASE64_LEN+1) || f_pwszKeyseed==NULL )
    {
        dr = DRM_E_BUFFERTOOSMALL;
        *f_pcchKeyseed = DRM_KEYSEED_BASE64_LEN+1;
        goto ErrorExit;
    }
    *f_pcchKeyseed = DRM_KEYSEED_BASE64_LEN+1;
    ChkDR(Oem_Random_GetBytes(f_pOEMContext, rgbKeyseed, DRM_KEYSEED_BASE64_LEN));

    ZEROMEM((DRM_BYTE*)f_pwszKeyseed, *f_pcchKeyseed * SIZEOF(DRM_WCHAR));
    for (i = 0; i < DRM_KEYSEED_BASE64_LEN; i++)
    {
        /* convert each byte into an alpha numberic character (62 available) */
        DRM_BYTE val = (GET_BYTE(rgbKeyseed, i)) % 62;

        if (val < 10)
        {
            val += NATIVE_WCHAR(g_wch0);
        }
        else if (val < 36)
        {
            val += NATIVE_WCHAR(g_wcha) - 10;
        }
        else
        {
            val += NATIVE_WCHAR(g_wchA) - 36;
        }
        f_pwszKeyseed[i] = WCHAR_CAST(val);
    }

ErrorExit:

    return dr;
}

DRM_API DRM_RESULT DRM_CALL DRM_KG_GenerateKeyID(
    IN                               DRM_VOID  *f_pOEMContext,
    __out_ecount_opt( *f_pcchKeyID ) DRM_WCHAR *f_pwszKeyID,           /* base64 encoded string */
    IN OUT                           DRM_DWORD *f_pcchKeyID )
{
    DRM_RESULT dr=DRM_SUCCESS;
    DRM_GUID   keyGuid;
    DRM_DWORD  cchEncoded = CCH_BASE64_EQUIV(SIZEOF(DRM_GUID));

    ChkArg( f_pcchKeyID != 0 );

    if ( *f_pcchKeyID<( cchEncoded+1)
      || f_pwszKeyID==NULL )
    {
        *f_pcchKeyID=cchEncoded+1;
        dr =DRM_E_BUFFERTOOSMALL;
        goto ErrorExit;
    }

    /*
    ** generate random bytes for KeyID (used to generate a GUID)
    */
    ChkDR(Oem_Random_GetBytes(f_pOEMContext, (DRM_BYTE*)&keyGuid, SIZEOF(DRM_GUID)));

    /* base64 encode */
    ZEROMEM((DRM_BYTE*)f_pwszKeyID, *f_pcchKeyID * SIZEOF(DRM_WCHAR));
    ChkDR(DRM_B64_EncodeW((DRM_BYTE *)&keyGuid, SIZEOF(DRM_GUID), f_pwszKeyID, &cchEncoded, 0));
    *f_pcchKeyID=cchEncoded;

ErrorExit:
    return dr;
}


static DRM_RESULT DRM_CALL _GeneratePlayReadyContentKey(
    __in_bcount(f_cbKeySeed) const      DRM_BYTE  *f_pbKeySeed,
    __in const                          DRM_DWORD  f_cbKeySeed,
    IN                                  DRM_KID   *f_pKID,
    IN                                  DRM_SUPPORTED_CIPHERS f_eCipherType,
    __out_bcount(f_cbContentKey)        DRM_BYTE  *f_pbContentKey,
    IN                                  DRM_DWORD  f_cbContentKey )
{
    DRM_SHA_CONTEXT  sha1 = DRM_SHA_CONTEXT_DEFAULT;
    DRM_SHA_CONTEXT  sha2 = DRM_SHA_CONTEXT_DEFAULT;
    DRM_RESULT       dr   = DRM_SUCCESS;
    DRM_DWORD        i    = 0;
    DRM_BYTE         rgbHashA[SHA256_DIGEST_SIZE_IN_BYTES] = {0};
    DRM_BYTE         rgbHashB[SHA256_DIGEST_SIZE_IN_BYTES] = {0};
    DRM_BYTE         rgbHashC[SHA256_DIGEST_SIZE_IN_BYTES] = {0};

    ChkArg( f_pKID != NULL );
    ChkArg( f_pbKeySeed != NULL );
    ChkArg( f_pbContentKey != NULL );
    ChkArg( f_eCipherType == eDRM_AES_COUNTER_CIPHER );
    ChkArg( f_cbContentKey == DRM_AES_KEYSIZE_128 ); /* Currently only a 128bit AES content key is valid */

    /* Perform first hash (A) */
    ChkDR( DRM_SHA_Init( &sha1, eDRM_SHA_256 ) );
    ChkDR( DRM_SHA_Update( f_pbKeySeed, f_cbKeySeed, &sha1 ) ); /* Explicitly use key seed length (f_cbKeySeed) */
    ChkDR( DRM_SHA_Update( f_pKID->rgb, SIZEOF( f_pKID->rgb ), &sha1 ) );
    MEMCPY( &sha2, &sha1, SIZEOF( DRM_SHA_CONTEXT ) ); /* Optimization: Save off current SHA state for use in hash B */
    ChkDR( DRM_SHA_Finalize( &sha1, SHA256_DIGEST_SIZE_IN_BYTES, rgbHashA ) );

    /* Perform second hash (B) */
    ChkDR( DRM_SHA_Update( f_pbKeySeed, f_cbKeySeed, &sha2 ) ); /* Explicitly use key seed length (f_cbKeySeed) */
    MEMCPY( &sha1, &sha2, SIZEOF( DRM_SHA_CONTEXT ) ); /* Optimization: Save off current SHA state for use in hash C */
    ChkDR( DRM_SHA_Finalize( &sha2, SHA256_DIGEST_SIZE_IN_BYTES, rgbHashB ) );

    /* Perform third hash (C) */
    ChkDR( DRM_SHA_Update( f_pKID->rgb, SIZEOF( f_pKID->rgb ), &sha1 ) );
    ChkDR( DRM_SHA_Finalize( &sha1, SHA256_DIGEST_SIZE_IN_BYTES, rgbHashC ) );

    /* Final step is to XOR resulting hashes to get the content key
    **
    ** Note that this loop may be optimized if we XOR 32bits or 64bits
    ** at a time instead of just 1 byte at a time, but if the output
    ** content key buffer isn't aligned we could have alignment issues
    ** on some platforms so for now this optimization is not done.
    */
    for( i = 0; i < DRM_AES_KEYSIZE_128; i++ )
    {
        f_pbContentKey[i] = rgbHashA[i] ^ rgbHashA[i + DRM_AES_KEYSIZE_128]
                        ^ rgbHashB[i] ^ rgbHashB[i + DRM_AES_KEYSIZE_128]
                        ^ rgbHashC[i] ^ rgbHashC[i + DRM_AES_KEYSIZE_128];
    }

ErrorExit:
    return dr;
}



static DRM_RESULT DRM_CALL _GenerateLegacyContentKey(
    __in_ecount( f_cchKeySeed ) const     DRM_WCHAR *f_pwszKeySeed,          /* base64 encoded string */
    __in                                  DRM_DWORD  f_cchKeySeed,
    __in                                  DRM_DWORD  f_cbContentKey,
    __in_ecount( f_cchKeyID ) const       DRM_WCHAR *f_pwszKeyID,            /* base64 encoded string */
    __in                                  DRM_DWORD  f_cchKeyID,
    __out_ecount_opt( *f_pcchContentKey ) DRM_WCHAR *f_pwszContentKey,       /* base64 encoded string */
    __inout                               DRM_DWORD *f_pcchContentKey)
{
    DRM_RESULT       dr     = DRM_SUCCESS;
    DRM_SHA_CONTEXT  shaVal = DRM_SHA_CONTEXT_DEFAULT;
    DRM_BYTE res       [__CB_DECL(DRM_SHA1_DIGEST_LEN + 1)];
    DRM_BYTE res1      [__CB_DECL(DRM_SHA1_DIGEST_LEN + 1)];
    DRM_BYTE res2      [__CB_DECL(DRM_SHA1_DIGEST_LEN + 1)];
    DRM_DWORD cchb64 = CCH_BASE64_EQUIV(f_cbContentKey);
    DRM_DWORD i      = 0;
    /*
    ** The size of the following scratch buffer has been doubled to accomodate
    ** longer Key Seeds. However, it is possible that the key seed is so long
    ** that this buffer might not be big enough to accomodate the entire seed.
    */
    DRM_BYTE rgbKeyseed[__CB_DECL(DRM_KEYSEED_BASE64_LEN*SIZEOF(DRM_WCHAR))];

    ChkArg( f_pwszKeySeed    != NULL
         && f_pwszKeyID      != NULL
         && f_cchKeyID       != 0
         && f_pcchContentKey != NULL );
    ChkArg( f_cbContentKey < DRM_SHA1_DIGEST_LEN );
    ChkArg( f_cchKeySeed <= SIZEOF(rgbKeyseed) );

    /*
    **  We should actually limit this to just 24 bytes; but for historical
    **  reasons, we have some providers using slightly longer KIDs. Reusing the
    **  keyseed buffer allows for slightly longer KIDs while still checking for
    **  KIDs that are too large
    */
    ChkArg( f_cchKeyID < NO_OF(rgbKeyseed) );

    /* check buffer length */
    if ( *f_pcchContentKey < cchb64
      || f_pwszContentKey == NULL )
    {
        *f_pcchContentKey=cchb64;
        dr=DRM_E_BUFFERTOOSMALL;
        goto ErrorExit;
    }

    for( i=0; i<f_cchKeySeed; i++ )
    {
        PUT_BYTE(rgbKeyseed, i,  (DRM_BYTE) NATIVE_WCHAR (f_pwszKeySeed [i]));
    }

    /* init buffer */
    ZEROMEM((DRM_BYTE*)f_pwszContentKey, *f_pcchContentKey*SIZEOF(DRM_WCHAR));

    /* generate key */
    ChkDR( DRM_SHA_Init( &shaVal, eDRM_SHA_1 ) );
    ChkDR( DRM_SHA_Update(rgbKeyseed, f_cchKeySeed, &shaVal) );
    ChkDR( DRM_SHA_Finalize(&shaVal, DRM_SHA1_DIGEST_LEN, res) );

    for( i=0; i<f_cchKeyID; i++ )
    {
        PUT_BYTE(rgbKeyseed, i, (DRM_BYTE) NATIVE_WCHAR (f_pwszKeyID [i]));
    }

    ChkDR( DRM_SHA_Init( &shaVal, eDRM_SHA_1 ) );
    ChkDR( DRM_SHA_Update( res, 6, &shaVal ) );  /* Copy first 48 bits of res */
    ChkDR( DRM_SHA_Update( rgbKeyseed, f_cchKeyID, &shaVal ) );
    ChkDR( DRM_SHA_Finalize( &shaVal, DRM_SHA1_DIGEST_LEN, res1 ) );

    ChkDR( DRM_SHA_Init( &shaVal, eDRM_SHA_1 ) );
    ChkDR( DRM_SHA_UpdateOffset( res, 6, 6, &shaVal ) );  /* Copy 2nd set of 48 bits of res */
    ChkDR( DRM_SHA_Update( rgbKeyseed, f_cchKeyID, &shaVal ) );
    ChkDR( DRM_SHA_Finalize( &shaVal, DRM_SHA1_DIGEST_LEN, res2 ) );

    ChkDR( DRM_SHA_Init( &shaVal, eDRM_SHA_1 ) );
    DRM_SHA_UpdateOffset(res, 12, 6, &shaVal);  /* Copy third set of 48 bits of res */
    ChkDR( DRM_SHA_Update( rgbKeyseed, f_cchKeyID, &shaVal ) );
    ChkDR( DRM_SHA_Finalize( &shaVal, DRM_SHA1_DIGEST_LEN, res ) );

    /* Reuse res to hold the final result. */
    for (i = 0; i < DRM_SHA1_DIGEST_LEN; i++)
    {
        DRM_BYTE b = GET_BYTE(res, i);
        b ^= GET_BYTE(res1, i) ^ GET_BYTE(res2, i);
        PUT_BYTE(res, i, b);
    }

    ChkDR(DRM_B64_EncodeW(res, f_cbContentKey, f_pwszContentKey, f_pcchContentKey, 0));

ErrorExit:

    return dr;
}

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_KG_GenerateContentKeyBinary(
    __in_bcount( f_cbKeySeed )             const DRM_BYTE  *f_pbKeySeed,
    __in                                         DRM_DWORD  f_cbKeySeed,
    __in                                         DRM_KID   *f_pKID,
    __in                                         DRM_SUPPORTED_CIPHERS f_eCipherType,
    __out_bcount(f_cbContentKey)                 DRM_BYTE  *f_pbContentKey,
    __in                                         DRM_DWORD  f_cbContentKey,
    __out_bcount_opt( f_cbScratchKeySeed )       DRM_BYTE  *f_pbScratchKeySeed,
    __in                                         DRM_DWORD  f_cbScratchKeySeed )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT       dr   = DRM_SUCCESS;

    ChkArg( f_pKID != NULL );
    ChkArg( f_pbContentKey != NULL );

    if( f_eCipherType == eDRM_RC4_CIPHER || f_eCipherType == eDRM_DES_CIPHER )
    {
        DRM_CONST_STRING dstr = EMPTY_DRM_STRING;
        DRM_WCHAR rgwcKeyID[DRM_KEYSEED_BASE64_LEN] = {0};
        DRM_WCHAR rgwcContentKey[CCH_BASE64_EQUIV(max(DES_KEYSIZE, DRMCIPHERKEYLEN_RC4))] = {0};
        DRM_DWORD cchKID = 0;
        DRM_DWORD cchSeed = 0;
        DRM_DWORD cchContentKey = 0;

        ChkArg( (f_eCipherType == eDRM_RC4_CIPHER && f_cbContentKey == DRMCIPHERKEYLEN_RC4)
             || (f_eCipherType == eDRM_DES_CIPHER && f_cbContentKey == DES_KEYSIZE) );
        ChkArg( f_pbScratchKeySeed != NULL );

        /* Size of the scratch buffer passed in should be big enough to accomodate the
        ** Base-64 encoded KeySeed in widechar format.
        */
        ChkArg(CCH_BASE64_EQUIV(f_cbKeySeed) * SIZEOF( DRM_WCHAR ) <= f_cbScratchKeySeed );

        cchSeed = ( f_cbScratchKeySeed / SIZEOF(DRM_WCHAR) );
        ChkDR( DRM_B64_EncodeW( f_pbKeySeed, f_cbKeySeed, (DRM_WCHAR *)f_pbScratchKeySeed, &cchSeed, 0 ) );

        cchKID = NO_OF( rgwcKeyID );
        ChkDR( DRM_B64_EncodeW( f_pKID->rgb, SIZEOF( f_pKID->rgb ), rgwcKeyID, &cchKID, 0 ) );
        ChkBOOL( cchKID <= NO_OF( rgwcKeyID ), DRM_E_BUFFERTOOSMALL );

        cchContentKey = NO_OF( rgwcContentKey );
        ChkDR( _GenerateLegacyContentKey( (DRM_WCHAR *)f_pbScratchKeySeed, cchSeed, f_cbContentKey, rgwcKeyID, cchKID, rgwcContentKey, &cchContentKey ) );

        dstr.pwszString = rgwcContentKey;
        dstr.cchString = cchContentKey;
        ChkDR( DRM_B64_DecodeW( &dstr, &f_cbContentKey, f_pbContentKey, 0 ) );
        ChkFAIL( f_cbContentKey == DRMCIPHERKEYLEN_RC4 );
    }
    else if( f_eCipherType == eDRM_AES_COUNTER_CIPHER )
    {
        ChkDR( _GeneratePlayReadyContentKey(f_pbKeySeed, f_cbKeySeed, f_pKID, f_eCipherType, f_pbContentKey, f_cbContentKey) );
    }
    else
    {
        /* Unsupported cipher type */
        ChkArg( FALSE );
    }

ErrorExit:
    return dr;

}

DRM_API DRM_RESULT DRM_CALL DRM_KG_GenerateContentKeyB64(
    __in_ecount( f_cchKeySeed ) const      DRM_WCHAR *f_pwszKeySeed,          /* base64 encoded string */
    IN                                     DRM_DWORD  f_cchKeySeed,
    __in_ecount( f_cchKeyID ) const        DRM_WCHAR *f_pwszKeyID,            /* base64 encoded string */
    IN                                     DRM_DWORD  f_cchKeyID,
    IN                                     DRM_SUPPORTED_CIPHERS f_eCipherType,
    IN                                     DRM_DWORD  f_cbContentKey,
    __out_ecount_opt(*f_pcchContentKey)    DRM_WCHAR *f_pwszContentKey,
    IN                                     DRM_DWORD *f_pcchContentKey,
    __out_bcount_opt( f_cbScratchKeySeed ) DRM_BYTE  *f_pbScratchKeySeed,
    IN                                     DRM_DWORD  f_cbScratchKeySeed)
{
    DRM_RESULT       dr   = DRM_SUCCESS;

    ChkArg( f_pwszKeySeed != NULL );
    ChkArg( f_pwszKeyID != NULL );
    ChkArg( f_pcchContentKey != NULL );

    if( f_eCipherType == eDRM_RC4_CIPHER || f_eCipherType == eDRM_DES_CIPHER )
    {
        /* Real KIDs should be smaller than this but according to source code comments there are legacy KIDs that
        ** can be this large.
        */
        ChkArg( f_cchKeyID <= DRM_KEYSEED_BASE64_LEN );

        ChkArg( (f_eCipherType == eDRM_RC4_CIPHER && f_cbContentKey == DRMCIPHERKEYLEN_RC4)
             || (f_eCipherType == eDRM_DES_CIPHER && f_cbContentKey == DES_KEYSIZE) );

        ChkDR( _GenerateLegacyContentKey( f_pwszKeySeed,
                                        f_cchKeySeed,
                                        f_cbContentKey,
                                        f_pwszKeyID,
                                        f_cchKeyID,
                                        f_pwszContentKey,
                                        f_pcchContentKey) );
    }
    else if( f_eCipherType == eDRM_AES_COUNTER_CIPHER )
    {
        DRM_CONST_STRING dstr = EMPTY_DRM_STRING;
        DRM_KID  kid = {0};
        DRM_BYTE rgbContentKey[DRMCIPHERKEYLEN_MAX] = {0};
        DRM_DWORD cbDecode = 0;

        ChkArg( f_cbContentKey <= DRMCIPHERKEYLEN_MAX );
        ChkArg( f_pbScratchKeySeed != NULL );

        /* Size of the scratch buffer passed in should be big enough to accomodate the
        ** Base-64 decoded KeySeed.
        */
        ChkArg( CB_BASE64_DECODE (f_cchKeySeed) <= f_cbScratchKeySeed );

        /* For PlayReady content keys, force that the base64 KID is the correct length */
        ChkArg( f_cchKeyID == CCH_BASE64_EQUIV( SIZEOF( DRM_KID ) ) );

        /* Check that the buffer is big enough. This would happen later in the B64 encode,
        ** but doing it now as well means we skip having to generate the content key twice
        */
        if( *f_pcchContentKey < CCH_BASE64_EQUIV( f_cbContentKey ) )
        {
            *f_pcchContentKey = CCH_BASE64_EQUIV( f_cbContentKey );
            ChkDR( DRM_E_BUFFERTOOSMALL );
        }

        dstr.pwszString = f_pwszKeySeed;
        dstr.cchString = f_cchKeySeed;
        cbDecode = f_cbScratchKeySeed;

        ChkDR( DRM_B64_DecodeW( &dstr, &cbDecode, f_pbScratchKeySeed, 0) );
        ChkArg( cbDecode <= f_cbScratchKeySeed );

        dstr.pwszString = f_pwszKeyID;
        dstr.cchString = f_cchKeyID;
        ChkDR( DRM_UTL_DecodeKID( &dstr, &kid ) );

        ChkDR( _GeneratePlayReadyContentKey(f_pbScratchKeySeed, cbDecode, &kid, f_eCipherType, rgbContentKey, f_cbContentKey) );

        ChkDR( DRM_B64_EncodeW( rgbContentKey, f_cbContentKey, f_pwszContentKey, f_pcchContentKey, 0 ) );
    }
    else
    {
        /* Unsupported cipher type */
        ChkArg( FALSE );
    }

ErrorExit:
    return dr;
}



DRM_API DRM_RESULT DRM_CALL DRM_KG_GenerateSigningKeys(
    IN                                 DRM_CRYPTO_CONTEXT *f_pCrypto,
    __out_ecount_opt( *f_pcchPrivKey ) DRM_WCHAR          *f_pwszPrivKey,  /* base64 encoded string */
    IN OUT                             DRM_DWORD          *f_pcchPrivKey,
    __out_ecount_opt( *f_pcchPubKey )  DRM_WCHAR          *f_pwszPubKey,   /* base64 encoded string */
    IN OUT                             DRM_DWORD          *f_pcchPubKey)
{
    DRM_RESULT dr        = DRM_SUCCESS;
    PUBKEY     pub       = { 0 };
    PRIVKEY    priv      = { 0 };
    DRM_DWORD  cbPubKey  = DRM_ECC160_PUBLIC_KEY_LEN;
    DRM_DWORD  cbPrivKey = DRM_ECC160_PRIVATE_KEY_LEN;
    DRM_DWORD  cchPubEncoded  = CCH_BASE64_EQUIV( DRM_ECC160_PUBLIC_KEY_LEN );
    DRM_DWORD  cchPrivEncoded = CCH_BASE64_EQUIV( DRM_ECC160_PRIVATE_KEY_LEN );

    if ( *f_pcchPrivKey <  cchPrivEncoded + 1
      || *f_pcchPubKey  <  cchPubEncoded  + 1
      ||  f_pwszPrivKey == NULL
      ||  f_pwszPubKey  == NULL )
    {
        *f_pcchPrivKey = cchPrivEncoded + 1;
        *f_pcchPubKey  = cchPubEncoded  + 1;
        dr = DRM_E_BUFFERTOOSMALL;
        goto ErrorExit;
    }

    ZEROMEM((DRM_BYTE*) f_pwszPrivKey, *f_pcchPrivKey * SIZEOF(DRM_WCHAR));
    ZEROMEM(( DRM_BYTE*) f_pwszPubKey,  *f_pcchPubKey  * SIZEOF(DRM_WCHAR));

    /* generate the key pair */
    ChkDR(DRM_PK_GenKeyPair( f_pCrypto->rgbCryptoContext,
                             eDRM_ECC_P160,
                             eDRM_RSA_UNKNOWN,
                             &cbPubKey,
                             ( DRM_BYTE * )&pub,
                             &cbPrivKey,
                             ( DRM_BYTE * )&priv ) );

    /* base64 encode */
    ChkDR(DRM_B64_EncodeW((DRM_BYTE *)&pub,  cbPubKey,  f_pwszPubKey,  f_pcchPubKey,  0));
    ChkDR(DRM_B64_EncodeW((DRM_BYTE *)&priv, cbPrivKey, f_pwszPrivKey, f_pcchPrivKey, 0));
    *f_pcchPubKey  += 1;
    *f_pcchPrivKey += 1;

ErrorExit:
    return dr;
}




EXIT_PK_NAMESPACE_CODE;
