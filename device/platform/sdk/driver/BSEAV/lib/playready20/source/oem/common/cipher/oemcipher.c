/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmcommon.h>
#include <oemcipher.h>
#include <oemaes.h>
#include <byteorder.h>
#include <oem.h>

ENTER_PK_NAMESPACE_CODE;

/*********************************************************************
**
**  Function:  DRM_CPHR_Init
**
**  Synopsis:  Initializes a content cipher context.  This must be done prior to using
**             DRM_CPHR_Encrypt or DRM_CHPR_Decrypt.
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
DRM_API DRM_RESULT DRM_CALL DRM_CPHR_Init(
    __out_ecount( 1 )                 DRM_CIPHER_CONTEXT     *f_pContext,
    __in                              DRM_SUPPORTED_CIPHERS   f_eCipherType,
    __in                              DRM_DWORD               f_cbContKey,
    __in_bcount( f_cbContKey )  const DRM_BYTE               *f_pbContKey )
{
    DRM_RESULT  dr             = DRM_SUCCESS;
    DRM_DWORD   rnd_bytes[16]  = { 0 };
    DRM_DWORD   rnd_bytes2[16] = { 0 };
    DRM_LONG    i              = 0;

    DRM_PROFILING_ENTER_SCOPE(PERF_MOD_DRMCIPHER, PERF_FUNC_DRM_CPHR_Init);

    DRMCASSERT( SIZEOF( DRM_SHA_CONTEXT ) < SIZEOF( RC4_KEYSTRUCT ) );

    ChkArg( f_pbContKey != NULL );
    ChkArg( f_cbContKey > 0 );
    ChkArg( f_cbContKey <= DRMCIPHERKEYLEN_MAX );
    ChkArg( f_pContext  != NULL );

    ZEROMEM( f_pContext, SIZEOF( DRM_CIPHER_CONTEXT ) );

    f_pContext->cbContentKey = f_cbContKey;

    /*
    ** Initialize an RC4 cipher
    */
    if( f_eCipherType == eDRM_RC4_CIPHER )
    {
        /*
        ** hash to song key to 20 bytes length. first 12 bytes used as a MAC key, the following
        ** 8 ( 7 ) bytes as a DES key
        */
        ChkDR( DRM_SHA_Init( ( DRM_SHA_CONTEXT * )&f_pContext->cipher.cipherRC4.rc4ks, eDRM_SHA_1 ) );
        ChkDR( DRM_SHA_Update( f_pbContKey,
                               f_cbContKey,
                              ( DRM_SHA_CONTEXT* )&f_pContext->cipher.cipherRC4.rc4ks ) );
        ChkDR( DRM_SHA_Finalize( ( DRM_SHA_CONTEXT* )&f_pContext->cipher.cipherRC4.rc4ks,
                                 DRM_SHA1_DIGEST_LEN,
                                 f_pContext->cipher.cipherRC4.shaOut ) );

        ZEROMEM( rnd_bytes2, SIZEOF( rnd_bytes ) );

        DRM_RC4_KeySetup( &f_pContext->cipher.cipherRC4.rc4ks,
                           12,
                           f_pContext->cipher.cipherRC4.shaOut );
        DRM_RC4_Cipher( &f_pContext->cipher.cipherRC4.rc4ks,
                         SIZEOF( rnd_bytes2 ),
                         ( DRM_BYTE * )rnd_bytes2 );

        for( i = 0; i < (DRM_LONG)NO_OF( rnd_bytes2 ); i++ )
        {
            BYTES_TO_DWORD( rnd_bytes[i], ( ( DRM_BYTE* )( rnd_bytes2 + i ) ) );
        }

        f_pContext->cipher.cipherRC4.mackey.a1 = rnd_bytes[0]  | 0x00000001;
        f_pContext->cipher.cipherRC4.mackey.b1 = rnd_bytes[1]  | 0x00000001;
        f_pContext->cipher.cipherRC4.mackey.c1 = rnd_bytes[2]  | 0x00000001;
        f_pContext->cipher.cipherRC4.mackey.d1 = rnd_bytes[3]  | 0x00000001;
        f_pContext->cipher.cipherRC4.mackey.e1 = rnd_bytes[4]  | 0x00000001;
        f_pContext->cipher.cipherRC4.mackey.f1 = rnd_bytes[5]  | 0x00000001;
        f_pContext->cipher.cipherRC4.mackey.a2 = rnd_bytes[6]  | 0x00000001;
        f_pContext->cipher.cipherRC4.mackey.b2 = rnd_bytes[7]  | 0x00000001;
        f_pContext->cipher.cipherRC4.mackey.c2 = rnd_bytes[8]  | 0x00000001;
        f_pContext->cipher.cipherRC4.mackey.d2 = rnd_bytes[9]  | 0x00000001;
        f_pContext->cipher.cipherRC4.mackey.e2 = rnd_bytes[10] | 0x00000001;
        f_pContext->cipher.cipherRC4.mackey.f2 = rnd_bytes[11] | 0x00000001;

        f_pContext->cipher.cipherRC4.invmackey.a1 =
            DRM_MAC_inv32( f_pContext->cipher.cipherRC4.mackey.a1 );
        f_pContext->cipher.cipherRC4.invmackey.a2 =
            DRM_MAC_inv32( f_pContext->cipher.cipherRC4.mackey.a2 );
        f_pContext->cipher.cipherRC4.invmackey.b1 =
            DRM_MAC_inv32( f_pContext->cipher.cipherRC4.mackey.b1 );
        f_pContext->cipher.cipherRC4.invmackey.b2 =
            DRM_MAC_inv32( f_pContext->cipher.cipherRC4.mackey.b2 );
        f_pContext->cipher.cipherRC4.invmackey.c1 =
            DRM_MAC_inv32( f_pContext->cipher.cipherRC4.mackey.c1 );
        f_pContext->cipher.cipherRC4.invmackey.c2 =
            DRM_MAC_inv32( f_pContext->cipher.cipherRC4.mackey.c2 );
        f_pContext->cipher.cipherRC4.invmackey.d1 =
            DRM_MAC_inv32( f_pContext->cipher.cipherRC4.mackey.d1 );
        f_pContext->cipher.cipherRC4.invmackey.d2 =
            DRM_MAC_inv32( f_pContext->cipher.cipherRC4.mackey.d2 );
        f_pContext->cipher.cipherRC4.invmackey.e1 =
            DRM_MAC_inv32( f_pContext->cipher.cipherRC4.mackey.e1 );
        f_pContext->cipher.cipherRC4.invmackey.e2 =
            DRM_MAC_inv32( f_pContext->cipher.cipherRC4.mackey.e2 );

        f_pContext->cipher.cipherRC4.desS1[0] = rnd_bytes[12];
        f_pContext->cipher.cipherRC4.desS1[1] = rnd_bytes[13];
        f_pContext->cipher.cipherRC4.desS2[0] = rnd_bytes[14];
        f_pContext->cipher.cipherRC4.desS2[1] = rnd_bytes[15];

        DRM_DES_KeySetup( &f_pContext->cipher.cipherRC4.destable,
                           f_pContext->cipher.cipherRC4.shaOut + __CB_DECL( 12 ) );
    }
    /*
    ** Initialize an AES CTR-mode cipher
    */
    else
    {
        ChkArg( f_eCipherType == eDRM_AES_COUNTER_CIPHER );
        ChkArg( f_cbContKey   == DRM_AES_KEYSIZE_128 );

        MEMCPY( f_pContext->cipher.cipherAES.rgbContentKey, f_pbContKey, f_cbContKey );

        ChkDR( Oem_Aes_SetKey( f_pbContKey, &f_pContext->cipher.cipherAES.aesKey ) );
    }
    f_pContext->eCipherType = f_eCipherType;
    f_pContext->fInited     = TRUE;

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}


/******************************************************************************
**
** Function :   DRM_CPHR_InitDecrypt
**
** Synopsis :   Initializes state for phased decryption
**
** Arguments :
**      f_pContext : Cipher context initialized with DRM_CPHR_Init
**      f_pbLast15 : Pointer to the last 15 bytes of the ciphertext. If the
**                   ciphertext is less than 15 bytes, it is a pointer to the
**                   beginning of the ciphertext.
**      f_cbData   : # of bytes in ENTIRE ciphertext
**
** Returns :
**      DRM_E_INVALIDARG
**          f_pContext is null, or f_pbLast15 is null when cipher type is RC4
**      DRM_E_CIPHER_NOTINITIALIZED
**          f_pContext was not appropriately initialized
**
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_CPHR_InitDecrypt(
    __inout_ecount( 1 )                                                DRM_CIPHER_CONTEXT *f_pContext,
    __in_bcount_opt( min(DRM_CPHR_CB_FOR_INITDECRYPT,f_cbData) ) const DRM_BYTE           *f_pbLast15,
    __in                                                               DRM_DWORD           f_cbData )
{
    DRM_DWORD  cbClear = 0;
    DRM_RESULT dr      = DRM_SUCCESS;

    DRM_PROFILING_ENTER_SCOPE(PERF_MOD_DRMCIPHER, PERF_FUNC_DRM_CPHR_InitDecrypt);

    ChkArg( f_pContext != NULL );
    ChkArg( f_cbData   >  0 );

    ChkBOOL( f_pContext->fInited, DRM_E_CIPHER_NOTINITIALIZED );

    /*
    ** Only RC4 cipher requires this extra initialization
    */
    if ( f_pContext->eCipherType == eDRM_RC4_CIPHER )
    {
        f_pContext->cipher.cipherRC4.fDecryptInited = FALSE;

        ChkArg( f_pbLast15 != NULL );

        /*
        **  Initialize data required for phased decryption
        */
        f_pContext->cipher.cipherRC4.cbPacket = f_cbData;
        f_pContext->cipher.cipherRC4.iPacket  = 0;

        if ( f_cbData >= 16 )
        {
            /*
            **  Find Number of clear bytes
            */
            cbClear = f_cbData % 8;

            /*
            **  Extract the last 8 bytes before the clear content
            ** ( To find these bytes start at the end of the last 15 bytes,
            **  subtract the number of clear bytes, then move 8 more bytes back,
            **  i.e. 15 - ( cbClear + 8 ) ).
            */
            DRM_BYT_CopyBytes( ( DRM_BYTE* )f_pContext->cipher.cipherRC4.rguiLast8,
                               0,
                               f_pbLast15,
                               15 - ( cbClear + 8 ),
                               2 * SIZEOF( DRM_DWORD ) );

            FIX_ENDIAN_DWORD( f_pContext->cipher.cipherRC4.rguiLast8[0] );
            FIX_ENDIAN_DWORD( f_pContext->cipher.cipherRC4.rguiLast8[1] );

            /*
            **  XOR the last 8 bytes( right before the clear content ) with the DES key
            */
            f_pContext->cipher.cipherRC4.rguiLast8[0] ^= f_pContext->cipher.cipherRC4.desS2[0];
            f_pContext->cipher.cipherRC4.rguiLast8[1] ^= f_pContext->cipher.cipherRC4.desS2[1];

            FIX_ENDIAN_DWORD( f_pContext->cipher.cipherRC4.rguiLast8[0] );
            FIX_ENDIAN_DWORD( f_pContext->cipher.cipherRC4.rguiLast8[1] );

            /*
            **  Use the DES key to decrypt the 8 bytes. The result is the XOR'd RC4
            **  content encryption key for the payload.
            */
            DRM_DES_Decrypt( ( DRM_BYTE * )f_pContext->cipher.cipherRC4.rguiLast8,
                             ( DRM_BYTE * )f_pContext->cipher.cipherRC4.rc4key,
                             &f_pContext->cipher.cipherRC4.destable );

            FIX_ENDIAN_DWORD( f_pContext->cipher.cipherRC4.rguiLast8[0] );
            FIX_ENDIAN_DWORD( f_pContext->cipher.cipherRC4.rguiLast8[1] );
            FIX_ENDIAN_DWORD( f_pContext->cipher.cipherRC4.rc4key[0] );
            FIX_ENDIAN_DWORD( f_pContext->cipher.cipherRC4.rc4key[1] );

            /*
            **  XOR the 8 bytes resulting from the DES decryption with the DES key.
            **  The result is the RC4 content key for the packet
            */
            f_pContext->cipher.cipherRC4.rc4key[0] ^= f_pContext->cipher.cipherRC4.desS1[0];
            f_pContext->cipher.cipherRC4.rc4key[1] ^= f_pContext->cipher.cipherRC4.desS1[1];

            FIX_ENDIAN_DWORD( f_pContext->cipher.cipherRC4.rc4key[0] );
            FIX_ENDIAN_DWORD( f_pContext->cipher.cipherRC4.rc4key[1] );

            /*
            **  Setup the RC4 key to decrypt content
            */
            DRM_RC4_KeySetup( &f_pContext->cipher.cipherRC4.rc4ks,
                              8,
                              ( DRM_BYTE * )f_pContext->cipher.cipherRC4.rc4key );

            FIX_ENDIAN_DWORD( f_pContext->cipher.cipherRC4.rc4key[0] );
            FIX_ENDIAN_DWORD( f_pContext->cipher.cipherRC4.rc4key[1] );

            /*
            **  Initialize cbc state
            */
            DRM_CBC64InitState( &( f_pContext->cipher.cipherRC4.cbcstate ) );
        }
        f_pContext->cipher.cipherRC4.fDecryptInited = TRUE;
    }
    else
    {
        ChkArg( f_pContext->eCipherType == eDRM_AES_COUNTER_CIPHER );
    }

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}

/******************************************************************************
**
** Function :   DRM_CPHR_Decrypt
**
** Synopsis :   Decrypts part of the ciphertext. This function must be called
**              after DRM_CPHR_InitDecrypt has been successful
**
** Arguments :
**     f_pContext     : Cipher context initialized with DRM_CPHR_Init
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
DRM_API DRM_RESULT DRM_CALL DRM_CPHR_Decrypt(
    __inout_ecount( 1 )          DRM_CIPHER_CONTEXT           *f_pContext,
    __inout_opt                  DRM_AES_COUNTER_MODE_CONTEXT *f_pCtrContext,
    __in                         DRM_DWORD                     f_cbData,
    __inout_bcount( f_cbData )   DRM_BYTE                     *f_pbData )
{
    DRM_RESULT  dr                = DRM_SUCCESS;
    DRM_DWORD   imac_start        = 0;
    DRM_DWORD   imac_end          = 0;
    DRM_DWORD   segment_end       = 0;
    DRM_DWORD   imac_in_seg_start = 0;
    DRM_DWORD   imac_in_seg_end   = 0;

    DRM_PROFILING_ENTER_SCOPE(PERF_MOD_DRMCIPHER, PERF_FUNC_DRM_CPHR_Decrypt);

    ChkArg( f_pbData   != NULL );
    ChkArg( f_pContext != NULL );
    ChkArg( f_cbData   >  0 );

    ChkBOOL( f_pContext->fInited, DRM_E_CIPHER_NOTINITIALIZED );

    /*
    ** RC4 Decryption:
    */
    if ( f_pContext->eCipherType == eDRM_RC4_CIPHER )
    {
        ChkBOOL( f_pContext->cipher.cipherRC4.fDecryptInited, DRM_E_DECRYPT_NOTINITIALIZED );

        ChkArg( ( f_pContext->cipher.cipherRC4.iPacket + f_cbData )
             <= f_pContext->cipher.cipherRC4.cbPacket );
        ChkOverflow ( ( f_pContext->cipher.cipherRC4.iPacket + f_cbData ),
                      f_pContext->cipher.cipherRC4.iPacket );

        if ( f_cbData == 0 )
        {
            goto ErrorExit;
        }

        /*
        **  small packet case: MAC does not handle it
        */
        if ( f_pContext->cipher.cipherRC4.cbPacket < 16 )
        {
            DRM_DWORD iData = 0;
            for ( iData = 0; iData < f_cbData; iData++ )
            {
                DRM_BYTE bSHA = GET_BYTE( f_pContext->cipher.cipherRC4.shaOut,
                                          iData + f_pContext->cipher.cipherRC4.iPacket );
                DRM_BYTE bData = GET_BYTE( f_pbData, iData );

                PUT_BYTE( f_pbData, iData, bData ^ bSHA );
            }

            f_pContext->cipher.cipherRC4.iPacket += f_cbData;
            goto ErrorExit;
        }

        imac_end    = ( f_pContext->cipher.cipherRC4.cbPacket / 8 ) * 8;
        imac_start  = imac_end - 8;
        segment_end = f_pContext->cipher.cipherRC4.iPacket + f_cbData;

        if ( segment_end > imac_start )
        {
            /*
            ** NOTE:  To remove complexity we do not handle the case where
            ** a packet is split somewhere in the middle of the last 15 bytes
            */
            DRMASSERT( segment_end == f_pContext->cipher.cipherRC4.cbPacket );

            /*
            ** Set the last 8 bytes correctly
            */
            DRM_BYT_CopyBytes( f_pbData,
                               imac_start - f_pContext->cipher.cipherRC4.iPacket,
                               ( DRM_BYTE* )f_pContext->cipher.cipherRC4.rguiLast8,
                               0,
                               2 * SIZEOF( DRM_DWORD ) );
        }

        /*
        **  RC4 decrypt the content
        */
        DRM_RC4_Cipher( &f_pContext->cipher.cipherRC4.rc4ks, f_cbData, f_pbData );

        if ( f_pContext->cipher.cipherRC4.iPacket < imac_start )
        {
            ChkOverflow( f_pContext->cipher.cipherRC4.iPacket + f_cbData,
                         f_pContext->cipher.cipherRC4.iPacket );

            if ( f_pContext->cipher.cipherRC4.iPacket + f_cbData >= imac_start )
            {
                DRM_DWORD mac1 = 0;
                DRM_DWORD mac2 = 0;
                DRM_DWORD macInverse1 = 0;
                DRM_DWORD macInverse2 = 0;

                /* Try to make Prefix happy. */
                ChkOverflow( ( imac_start - f_pContext->cipher.cipherRC4.iPacket ) * 8,
                             imac_start - f_pContext->cipher.cipherRC4.iPacket );

                /*
                **  First update MAC with data from this segment
                */
                DRM_CBC64Update( &f_pContext->cipher.cipherRC4.mackey,
                             &f_pContext->cipher.cipherRC4.cbcstate,
                             imac_start - f_pContext->cipher.cipherRC4.iPacket,
                             f_pbData );

                /*
                **  Finalize MAC to decipher last 8 bytes of encrypted data
                */
                mac1 = DRM_CBC64Finalize( &f_pContext->cipher.cipherRC4.mackey,
                                      &f_pContext->cipher.cipherRC4.cbcstate,
                                      &mac2 );
                macInverse2 = DRM_CBC64Invert( &f_pContext->cipher.cipherRC4.mackey,
                                           &f_pContext->cipher.cipherRC4.invmackey,
                                           mac1,
                                           mac2,
                                           f_pContext->cipher.cipherRC4.rc4key[0],
                                           f_pContext->cipher.cipherRC4.rc4key[1],
                                           &macInverse1 );
                f_pContext->cipher.cipherRC4.rc4key[0] = macInverse1;
                f_pContext->cipher.cipherRC4.rc4key[1] = macInverse2;
            }
            else
            {
                /*
                **  Update MAC with data from this segment
                */
                DRM_CBC64Update( &f_pContext->cipher.cipherRC4.mackey,
                             &f_pContext->cipher.cipherRC4.cbcstate,
                             f_cbData,
                             f_pbData );
            }
        }

        if ( f_pContext->cipher.cipherRC4.iPacket < imac_end
          && segment_end > imac_start )
        {
            /*
            **  Insert last 8 bytes of data deciphered
            */
            DRM_BYTE  rgbMac[__CB_DECL( 8 )];
            DRM_DWORD iData = 0;

            DWORD_TO_BYTES( rgbMac,                f_pContext->cipher.cipherRC4.rc4key[0] );
            DWORD_TO_BYTES( rgbMac + __CB_DECL( 4 ), f_pContext->cipher.cipherRC4.rc4key[1] );

            imac_in_seg_start = ( imac_start >= f_pContext->cipher.cipherRC4.iPacket ) ?
                                                imac_start : f_pContext->cipher.cipherRC4.iPacket;
            imac_in_seg_end = ( imac_end <= segment_end ) ? imac_end:segment_end;

            for ( iData = imac_in_seg_start; iData < imac_in_seg_end; iData++ )
            {
                PUT_BYTE( f_pbData,
                          iData - f_pContext->cipher.cipherRC4.iPacket,
                          GET_BYTE( rgbMac, iData - imac_start ) );
            }
        }

        f_pContext->cipher.cipherRC4.iPacket += f_cbData;
        if ( f_pContext->cipher.cipherRC4.iPacket >= f_pContext->cipher.cipherRC4.cbPacket )
        {
            f_pContext->cipher.cipherRC4.fDecryptInited = FALSE;
        }
    }
    /*
    ** AES Counter-mode decryption:
    */
    else
    {
        ChkArg( f_pContext->eCipherType == eDRM_AES_COUNTER_CIPHER );
        ChkArg( f_pCtrContext           != NULL );

        ChkDR( Oem_Aes_CtrProcessData( &f_pContext->cipher.cipherAES.aesKey,
                                       f_pbData,
                                       f_cbData,
                                       f_pCtrContext ) );
    }

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}


/*********************************************************************
**
**  Function:  DRM_CPHR_Encrypt
**
**  Synopsis:  Encrypts an array of bytes using a DRM content cipher algorithm
**
**  Arguments:
**     f_pContext     : Cipher context initialized with DRM_CPHR_Init
**     f_pCtrContext  : Sets and maintains the
**                      initialization vector and counter
**                      for AES CTR mode. Must be
**                      specified for AES decryption, and
**                      ignored for Cocktail.
**     f_cbData       : Length of f_pbData in bytes
**     f_pbData       : Array of bytes to encrypt
**
**  Returns:
**      DRM_SUCCESS
**          success
**      DRM_E_INVALIDARG
**          f_pbData or f_pContext is null or the data has an invalid byte count
**          for AES ECB-mode cipher
**      DRM_E_CIPHER_NOTINITIALIZED
**          f_pContext was not appropriately initialized
**      DRM_E_CRYPTO_FAILED
**          The encrypt/decrypt operation failed
**      DRM_E_ARITHMETIC_OVERFLOW
**          An overflow/underflow occurred while processing the data
**
**  Notes:  Encryption is done in place.
**
*********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_CPHR_Encrypt(
    __inout_ecount( 1 )         DRM_CIPHER_CONTEXT           *f_pContext,
    __inout_opt                 DRM_AES_COUNTER_MODE_CONTEXT *f_pCtrContext,
    __in                        DRM_DWORD                     f_cbData,
    __inout_bcount( f_cbData )  DRM_BYTE                     *f_pbData )
{
    DRM_RESULT dr            = DRM_SUCCESS;
    DRM_DWORD  macLength4    = 0;       /* mac length in four byte blocks */
    DRM_DWORD  rc4key    [2] = { 0 };
    DRM_DWORD  rguiLast8 [2] = { 0 };

    DRM_PROFILING_ENTER_SCOPE(PERF_MOD_DRMCIPHER, PERF_FUNC_DRM_CPHR_Encrypt);

    ChkArg( f_pbData   != NULL );
    ChkArg( f_pContext != NULL );

    ChkBOOL( f_pContext->fInited, DRM_E_CIPHER_NOTINITIALIZED );

    /*
    ** RC4/Cocktail Encrypt:
    */
    if ( f_pContext->eCipherType == eDRM_RC4_CIPHER )
    {
        /*
        ** small packet case: MAC does not handle it
        */
        if ( f_cbData < 16 )
        {
            for ( rc4key[0] = 0; rc4key[0] < f_cbData; rc4key[0]++ )
            {
                DRM_BYTE bTemp = GET_BYTE( f_pbData, rc4key[0] );
                bTemp ^= GET_BYTE( f_pContext->cipher.cipherRC4.shaOut, rc4key[0] );
                PUT_BYTE( f_pbData, rc4key[0], bTemp );
            }
        }
        else
        {
            /*
            ** making sure block number is even
            */
            macLength4 = ( f_cbData / 8 ) * 2;
            MEMCPY( rguiLast8,
                   ( f_pbData + __CB_DECL( 4 * macLength4 - 8 )),
                   NO_OF( rguiLast8 ) * SIZEOF( DRM_DWORD ) );

            /*
            ** run MAC over data
            */
            DRM_CBC_Mac( f_pbData, macLength4, rc4key, &( f_pContext->cipher.cipherRC4.mackey ) );

            /*
            ** RC4 encrypt content
            */
            FIX_ENDIAN_DWORD( rc4key[0] );
            FIX_ENDIAN_DWORD( rc4key[1] );

            DRM_RC4_KeySetup( &f_pContext->cipher.cipherRC4.rc4ks, 8, ( DRM_BYTE * )rc4key );
            DRM_RC4_Cipher(   &f_pContext->cipher.cipherRC4.rc4ks, f_cbData, f_pbData );
            FIX_ENDIAN_DWORD( rc4key[0] );
            FIX_ENDIAN_DWORD( rc4key[1] );

            /*
            ** DES encrypt MAC and put it in the right place
            */
            rc4key[0] ^= f_pContext->cipher.cipherRC4.desS1[0];
            rc4key[1] ^= f_pContext->cipher.cipherRC4.desS1[1];

            FIX_ENDIAN_DWORD( rc4key[0] );
            FIX_ENDIAN_DWORD( rc4key[1] );
            DRM_DES_Encrypt( ( DRM_BYTE * )rc4key,
                             ( DRM_BYTE* )rguiLast8,
                              &f_pContext->cipher.cipherRC4.destable );

            FIX_ENDIAN_DWORD( rguiLast8[0] );
            FIX_ENDIAN_DWORD( rguiLast8[1] );
            rguiLast8[0] ^= f_pContext->cipher.cipherRC4.desS2[0];
            rguiLast8[1] ^= f_pContext->cipher.cipherRC4.desS2[1];
            DWORD_TO_BYTES( ( f_pbData + __CB_DECL( 4 * macLength4 - 8 ) ), rguiLast8[0] );
            DWORD_TO_BYTES( ( f_pbData + __CB_DECL( 4 * macLength4 - 4 ) ), rguiLast8[1] );
        }
    }
    /*
    ** Counter-Mode AES Encrypt:
    */
    else
    {
        ChkArg( f_pContext->eCipherType == eDRM_AES_COUNTER_CIPHER );
        ChkArg( f_pCtrContext          != NULL );

        ChkDR( Oem_Aes_CtrProcessData( &f_pContext->cipher.cipherAES.aesKey,
                                       f_pbData,
                                       f_cbData,
                                       f_pCtrContext ) );
    }

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}

EXIT_PK_NAMESPACE_CODE;
