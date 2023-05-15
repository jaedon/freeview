/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

/*
** Module Name: aes128.c
**
** Abstract:
**    This module contains the low-level AES encryption routines
**    for 128-bit AES
**    Derived from public domain sources 
*/

#include "aes128.h"
#include "aesbox.h"

ENTER_PK_NAMESPACE_CODE;

/*********************************************************************************************
** Function:  Aes_CreateKey128
**
** Synopsis:  Set up an AES Key Table for a 128-bit AES key
**
** Arguments: [f_pKeyTable ]: The key table to set up ( output )
**            [f_rgbKey]    : The key data
**
** Returns:   None
**********************************************************************************************/
DRM_API DRM_NO_INLINE DRM_VOID Aes_CreateKey128(
    __out_ecount( 1 )                        DRM_AESTable *f_pKeyTable,
    __in_bcount( DRM_AES_KEYSIZE_128 ) const DRM_BYTE      f_rgbKey[__CB_DECL( DRM_AES_KEYSIZE_128 )] )
{   
    CLAW_AUTO_RANDOM_CIPHER
    /*
    ** Calculate the necessary round keys
    ** The number of calculations depends on keyBits and blockBits
    */
    DRM_DWORD  j           = 0; 
    DRM_DWORD  r           = 0; 
    DRM_DWORD  t           = 0;     
    DRM_DWORD  rconpointer = 0;    
    DRM_BYTE   bTmp        = 0;
    DRM_DWORD *w           = NULL;
    DRM_DWORD  tk[DRM_AES_ROUNDS_128 - 6] = { 0 };
    const DRM_DWORD  KC    = DRM_AES_ROUNDS_128 - 6;

    /*
    ** These assertions check that DWORD access of T and U arrays is valid
    */
    DRMASSERT( DRM_IS_DWORD_ALIGNED( AES_T1[0] ) );
    DRMASSERT( DRM_IS_DWORD_ALIGNED( AES_T2[0] ) );
    DRMASSERT( DRM_IS_DWORD_ALIGNED( AES_T3[0] ) );
    DRMASSERT( DRM_IS_DWORD_ALIGNED( AES_T4[0] ) );
    DRMASSERT( DRM_IS_DWORD_ALIGNED( AES_T5[0] ) );
    DRMASSERT( DRM_IS_DWORD_ALIGNED( AES_T6[0] ) );
    DRMASSERT( DRM_IS_DWORD_ALIGNED( AES_T7[0] ) );
    DRMASSERT( DRM_IS_DWORD_ALIGNED( AES_T8[0] ) );
    DRMASSERT( DRM_IS_DWORD_ALIGNED( AES_U1[0] ) );
    DRMASSERT( DRM_IS_DWORD_ALIGNED( AES_U2[0] ) );
    DRMASSERT( DRM_IS_DWORD_ALIGNED( AES_U3[0] ) );
    DRMASSERT( DRM_IS_DWORD_ALIGNED( AES_U4[0] ) );


    /*
    ** Compute keytabenc
    */

    for( j = KC; j > 0; j-- )
    {
        MEMCPY( ( DRM_BYTE * )&tk[j-1], &f_rgbKey[4 * ( j - 1 )], SIZEOF( DRM_DWORD ) );
    }

    /*
    ** copy values into round key array 
    */
    for( j = 0; ( j < KC ) && ( r < DRM_AES_ROUNDS_128 + 1 ); ) 
    {
        for ( ; ( j < KC ) && ( t < 4 ); j++, t++ ) 
        {
            f_pKeyTable->keytabenc[r][t] = tk[j];
        }
        if ( t == 4 )
        {
            r++;
            t = 0;
        }
    }
    /*
    ** while not enough round key material calculated
    ** calculate new values 
    */
    while ( r < DRM_AES_ROUNDS_128 + 1 ) 
    { 
        bTmp =  GET_BYTE( ( DRM_BYTE * )( tk + ( KC-1 ) ), 1 );
        bTmp =  GET_BYTE( AES_S, bTmp );
        bTmp ^= GET_BYTE( ( DRM_BYTE * )( tk ), 0 );
        PUT_BYTE( ( DRM_BYTE * )( tk ), 0, bTmp );

        bTmp =  GET_BYTE( ( DRM_BYTE * )( tk + ( KC-1 ) ), 2 );
        bTmp =  GET_BYTE( AES_S, bTmp );
        bTmp ^= GET_BYTE( ( DRM_BYTE * )( tk ), 1 );
        PUT_BYTE( ( DRM_BYTE * )( tk ), 1, bTmp );

        bTmp =  GET_BYTE( ( DRM_BYTE * )( tk + ( KC-1 ) ), 3 );
        bTmp =  GET_BYTE( AES_S, bTmp );
        bTmp ^= GET_BYTE( ( DRM_BYTE * )( tk ), 2 );
        PUT_BYTE( ( DRM_BYTE * )( tk ), 2, bTmp );

        bTmp =  GET_BYTE( ( DRM_BYTE * )( tk + ( KC-1 ) ), 0 );
        bTmp =  GET_BYTE( AES_S, bTmp );
        bTmp ^= GET_BYTE( ( DRM_BYTE * )( tk ), 3 );
        PUT_BYTE( ( DRM_BYTE * )( tk ), 3, bTmp );

        bTmp =  ( DRM_BYTE ) rcon[rconpointer++];
        bTmp ^= GET_BYTE( ( DRM_BYTE * )( tk ), 0 );
        PUT_BYTE( ( DRM_BYTE * )( tk ), 0, bTmp );

        for ( j = 1; j < KC; j++ ) 
        {
            tk[j] ^= tk[j-1];
        }
        
        /*
        ** Copy values into round key array 
        */
        for ( j = 0; ( j < KC ) && ( r < DRM_AES_ROUNDS_128 + 1 ); ) 
        {
            for ( ; ( j < KC ) && ( t < 4 ); j++, t++ ) 
            {
                f_pKeyTable->keytabenc[r][t] = tk[j];
            }
            if ( t == 4 ) 
            {
                r++;
                t = 0;
            }
        }
    }

    MEMCPY( ( DRM_BYTE * )( f_pKeyTable->keytabdec[0] ), 
            ( DRM_BYTE * )( f_pKeyTable->keytabenc[0] ),
            SIZEOF( f_pKeyTable->keytabenc ) );

    /*
    ** Compute keytabdec
    */
    for ( r = 1; r < DRM_AES_ROUNDS_128; r++ ) 
    {
        w = &f_pKeyTable->keytabdec[r][0];
        *w =
              *( ( DRM_DWORD* )AES_U1[GET_BYTE( ( DRM_BYTE * )w, 0 )] )
            ^ *( ( DRM_DWORD* )AES_U2[GET_BYTE( ( DRM_BYTE * )w, 1 )] )
            ^ *( ( DRM_DWORD* )AES_U3[GET_BYTE( ( DRM_BYTE * )w, 2 )] )
            ^ *( ( DRM_DWORD* )AES_U4[GET_BYTE( ( DRM_BYTE * )w, 3 )] );

        w = &f_pKeyTable->keytabdec[r][1];
        *w =
              *( ( DRM_DWORD* )AES_U1[GET_BYTE( ( DRM_BYTE * )w, 0 )] )
            ^ *( ( DRM_DWORD* )AES_U2[GET_BYTE( ( DRM_BYTE * )w, 1 )] )
            ^ *( ( DRM_DWORD* )AES_U3[GET_BYTE( ( DRM_BYTE * )w, 2 )] )
            ^ *( ( DRM_DWORD* )AES_U4[GET_BYTE( ( DRM_BYTE * )w, 3 )] );

        w = &f_pKeyTable->keytabdec[r][2];
        *w =
              *( ( DRM_DWORD* )AES_U1[GET_BYTE( ( DRM_BYTE * )w, 0 )] )
            ^ *( ( DRM_DWORD* )AES_U2[GET_BYTE( ( DRM_BYTE * )w, 1 )] )
            ^ *( ( DRM_DWORD* )AES_U3[GET_BYTE( ( DRM_BYTE * )w, 2 )] )
            ^ *( ( DRM_DWORD* )AES_U4[GET_BYTE( ( DRM_BYTE * )w, 3 )] );

        w = &f_pKeyTable->keytabdec[r][3];
        *w =
              *( ( DRM_DWORD* )AES_U1[GET_BYTE( ( DRM_BYTE * )w, 0 )] )
            ^ *( ( DRM_DWORD* )AES_U2[GET_BYTE( ( DRM_BYTE * )w, 1 )] )
            ^ *( ( DRM_DWORD* )AES_U3[GET_BYTE( ( DRM_BYTE * )w, 2 )] )
            ^ *( ( DRM_DWORD* )AES_U4[GET_BYTE( ( DRM_BYTE * )w, 3 )] );
    }   
}


/*********************************************************************************************
** Function:  Aes_Encrypt128
**
** Synopsis:  Encrypt a single block
**
** Arguments: 
**            [f_rgbEncrypted] : Returns the encrypted data
**            [f_rgbClear]     : Specifies the data to encrypt
**            [f_rgbKeys]      : The key table to use for decryption
**
** Returns:   None
**********************************************************************************************/
DRM_API DRM_NO_INLINE DRM_VOID Aes_Encrypt128( 
    __out_bcount( DRM_AES_KEYSIZE_128 )       DRM_BYTE   f_rgbEncrypted[__CB_DECL( DRM_AES_KEYSIZE_128 )],
    __in_bcount( DRM_AES_KEYSIZE_128 )  const DRM_BYTE   f_rgbClear[__CB_DECL( DRM_AES_KEYSIZE_128 )],
    __in_ecount( DRM_AES_ROUNDS_128+1 ) const DRM_DWORD  f_rgbKeys[DRM_AES_ROUNDS_128+1][4] )
#if 1
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_DWORD r       = 0;
    DRM_DWORD c[4]    = { 0 };
    DRM_BYTE *d       = ( DRM_BYTE * )c;
    DRM_DWORD temp[4] = { 0 };
    DRM_BYTE  bTmp    = 0;

    MEMCPY( ( DRM_BYTE * )temp, f_rgbClear, DRM_AES_KEYSIZE_128 );

    temp[0] ^= f_rgbKeys[0][0];
    temp[1] ^= f_rgbKeys[0][1];
    temp[2] ^= f_rgbKeys[0][2];
    temp[3] ^= f_rgbKeys[0][3];
    
    c[0] = *( ( DRM_DWORD* )AES_T1[GET_BYTE( ( ( DRM_BYTE * )temp ), 0 )] )
         ^ *( ( DRM_DWORD* )AES_T2[GET_BYTE( ( ( DRM_BYTE * )( temp + 1 ) ), 1 )] )
         ^ *( ( DRM_DWORD* )AES_T3[GET_BYTE( ( ( DRM_BYTE * )( temp + 2 ) ), 2 )] )
         ^ *( ( DRM_DWORD* )AES_T4[GET_BYTE( ( ( DRM_BYTE * )( temp + 3 ) ), 3 )] );

    c[1] = *( ( DRM_DWORD* )AES_T1[GET_BYTE( ( ( DRM_BYTE * )( temp + 1 ) ), 0 )] )
         ^ *( ( DRM_DWORD* )AES_T2[GET_BYTE( ( ( DRM_BYTE * )( temp + 2 ) ), 1 )] )
         ^ *( ( DRM_DWORD* )AES_T3[GET_BYTE( ( ( DRM_BYTE * )( temp + 3 ) ), 2 )] )
         ^ *( ( DRM_DWORD* )AES_T4[GET_BYTE( ( ( DRM_BYTE * )temp ), 3 )] );

    c[2] = *( ( DRM_DWORD* )AES_T1[GET_BYTE( ( ( DRM_BYTE * )( temp + 2 ) ), 0 )] )
         ^ *( ( DRM_DWORD* )AES_T2[GET_BYTE( ( ( DRM_BYTE * )( temp + 3 ) ), 1 )] )
         ^ *( ( DRM_DWORD* )AES_T3[GET_BYTE( ( ( DRM_BYTE * )temp ), 2 )] )
         ^ *( ( DRM_DWORD* )AES_T4[GET_BYTE( ( ( DRM_BYTE * )( temp + 1 ) ), 3 )] );

    c[3] = *( ( DRM_DWORD* )AES_T1[GET_BYTE( ( ( DRM_BYTE * )( temp + 3 ) ), 0 )] )
         ^ *( ( DRM_DWORD* )AES_T2[GET_BYTE( ( ( DRM_BYTE * )temp ), 1 )] )
         ^ *( ( DRM_DWORD* )AES_T3[GET_BYTE( ( ( DRM_BYTE * )( temp + 1 ) ), 2 )] )
         ^ *( ( DRM_DWORD* )AES_T4[GET_BYTE( ( ( DRM_BYTE * )( temp + 2 ) ), 3 )] );

    for ( r = 1; r < DRM_AES_ROUNDS_128 - 1; r++ )
    {
        temp[0] = c[0] ^ f_rgbKeys[r][0];
        temp[1] = c[1] ^ f_rgbKeys[r][1];
        temp[2] = c[2] ^ f_rgbKeys[r][2];
        temp[3] = c[3] ^ f_rgbKeys[r][3];

        c[0] = *( ( DRM_DWORD* )AES_T1[GET_BYTE( ( ( DRM_BYTE * )temp ), 0 )] )
             ^ *( ( DRM_DWORD* )AES_T2[GET_BYTE( ( ( DRM_BYTE * )( temp + 1 ) ), 1 )] )
             ^ *( ( DRM_DWORD* )AES_T3[GET_BYTE( ( ( DRM_BYTE * )( temp + 2 ) ), 2 )] )
             ^ *( ( DRM_DWORD* )AES_T4[GET_BYTE( ( ( DRM_BYTE * )( temp + 3 ) ), 3 )] );

        c[1] = *( ( DRM_DWORD* )AES_T1[GET_BYTE( ( ( DRM_BYTE * )( temp + 1 ) ), 0 )] )
             ^ *( ( DRM_DWORD* )AES_T2[GET_BYTE( ( ( DRM_BYTE * )( temp + 2 ) ), 1 )] )
             ^ *( ( DRM_DWORD* )AES_T3[GET_BYTE( ( ( DRM_BYTE * )( temp + 3 ) ), 2 )] )
             ^ *( ( DRM_DWORD* )AES_T4[GET_BYTE( ( ( DRM_BYTE * )temp ), 3 )] );

        c[2] = *( ( DRM_DWORD* )AES_T1[GET_BYTE( ( ( DRM_BYTE * )( temp + 2 ) ), 0 )] )
             ^ *( ( DRM_DWORD* )AES_T2[GET_BYTE( ( ( DRM_BYTE * )( temp + 3 ) ), 1 )] )
             ^ *( ( DRM_DWORD* )AES_T3[GET_BYTE( ( ( DRM_BYTE * )temp ), 2 )] )
             ^ *( ( DRM_DWORD* )AES_T4[GET_BYTE( ( ( DRM_BYTE * )( temp + 1 ) ), 3 )] );

        c[3] = *( ( DRM_DWORD* )AES_T1[GET_BYTE( ( ( DRM_BYTE * )( temp + 3 ) ), 0 )] )
             ^ *( ( DRM_DWORD* )AES_T2[GET_BYTE( ( ( DRM_BYTE * )temp ), 1 )] )
             ^ *( ( DRM_DWORD* )AES_T3[GET_BYTE( ( ( DRM_BYTE * )( temp + 1 ) ), 2 )] )
             ^ *( ( DRM_DWORD* )AES_T4[GET_BYTE( ( ( DRM_BYTE * )( temp + 2 ) ), 3 )] );
    }

    /*
    ** last round is special 
    */
    temp[0] = c[0] ^ f_rgbKeys[DRM_AES_ROUNDS_128-1][0];
    temp[1] = c[1] ^ f_rgbKeys[DRM_AES_ROUNDS_128-1][1];
    temp[2] = c[2] ^ f_rgbKeys[DRM_AES_ROUNDS_128-1][2];
    temp[3] = c[3] ^ f_rgbKeys[DRM_AES_ROUNDS_128-1][3];

    bTmp = GET_BYTE( ( ( DRM_BYTE * )temp ), 0 );
    bTmp = GET_BYTE( AES_T1[bTmp], 1 );
    PUT_BYTE( d, 0, bTmp );

    bTmp = GET_BYTE( ( ( DRM_BYTE * )( temp + 1 ) ), 1 );
    bTmp = GET_BYTE( AES_T1[bTmp], 1 );
    PUT_BYTE( d, 1, bTmp );

    bTmp = GET_BYTE( ( ( DRM_BYTE * )( temp + 2 ) ), 2 );
    bTmp = GET_BYTE( AES_T1[bTmp], 1 );
    PUT_BYTE( d, 2, bTmp );

    bTmp = GET_BYTE( ( ( DRM_BYTE * )( temp + 3 ) ), 3 );
    bTmp = GET_BYTE( AES_T1[bTmp], 1 );
    PUT_BYTE( d, 3, bTmp );

    bTmp = GET_BYTE( ( ( DRM_BYTE * )( temp + 1 ) ), 0 );
    bTmp = GET_BYTE( AES_T1[bTmp], 1 );
    PUT_BYTE( d, 4, bTmp );

    bTmp = GET_BYTE( ( ( DRM_BYTE * )( temp + 2 ) ), 1 );
    bTmp = GET_BYTE( AES_T1[bTmp], 1 );
    PUT_BYTE( d, 5, bTmp );

    bTmp = GET_BYTE( ( ( DRM_BYTE * )( temp + 3 ) ), 2 );
    bTmp = GET_BYTE( AES_T1[bTmp], 1 );
    PUT_BYTE( d, 6, bTmp );

    bTmp = GET_BYTE( ( ( DRM_BYTE * )temp ), 3 );
    bTmp = GET_BYTE( AES_T1[bTmp], 1 );
    PUT_BYTE( d, 7, bTmp );

    bTmp = GET_BYTE( ( ( DRM_BYTE * )( temp + 2 ) ), 0 );
    bTmp = GET_BYTE( AES_T1[bTmp], 1 );
    PUT_BYTE( d, 8, bTmp );

    bTmp = GET_BYTE( ( ( DRM_BYTE * )( temp + 3 ) ), 1 );
    bTmp = GET_BYTE( AES_T1[bTmp], 1 );
    PUT_BYTE( d, 9, bTmp );

    bTmp = GET_BYTE( ( ( DRM_BYTE * )temp ), 2 );
    bTmp = GET_BYTE( AES_T1[bTmp], 1 );
    PUT_BYTE( d, 10, bTmp );

    bTmp = GET_BYTE( ( ( DRM_BYTE * )( temp + 1 ) ), 3 );
    bTmp = GET_BYTE( AES_T1[bTmp], 1 );
    PUT_BYTE( d, 11, bTmp );

    bTmp = GET_BYTE( ( ( DRM_BYTE * )( temp + 3 ) ), 0 );
    bTmp = GET_BYTE( AES_T1[bTmp], 1 );
    PUT_BYTE( d, 12, bTmp );

    bTmp = GET_BYTE( ( ( DRM_BYTE * )temp ), 1 );
    bTmp = GET_BYTE( AES_T1[bTmp], 1 );
    PUT_BYTE( d, 13, bTmp );

    bTmp = GET_BYTE( ( ( DRM_BYTE * )( temp + 1 ) ), 2 );
    bTmp = GET_BYTE( AES_T1[bTmp], 1 );
    PUT_BYTE( d, 14, bTmp );

    bTmp = GET_BYTE( ( ( DRM_BYTE * )( temp + 2 ) ), 3 );
    bTmp = GET_BYTE( AES_T1[bTmp], 1 );
    PUT_BYTE( d, 15, bTmp );

    c[0] ^= f_rgbKeys[DRM_AES_ROUNDS_128][0];
    c[1] ^= f_rgbKeys[DRM_AES_ROUNDS_128][1];
    c[2] ^= f_rgbKeys[DRM_AES_ROUNDS_128][2];
    c[3] ^= f_rgbKeys[DRM_AES_ROUNDS_128][3];
    MEMCPY( f_rgbEncrypted, c, SIZEOF( c ) );

    return;    
}
#else
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_DWORD r       = 0;
    DRM_DWORD c[4]    = { 0 };
    DRM_BYTE *d       = ( DRM_BYTE * )c;
    DRM_DWORD temp[4] = { 0 };
    DRM_BYTE  bTmp    = 0;

    MEMCPY( ( DRM_BYTE * )temp, f_rgbClear, DRM_AES_KEYSIZE_128 );

    temp[0] ^= f_rgbKeys[0][0];
    temp[1] ^= f_rgbKeys[0][1];
    temp[2] ^= f_rgbKeys[0][2];
    temp[3] ^= f_rgbKeys[0][3];
    
    c[0] = *( ( DRM_DWORD* )T1[GET_BYTE( ( ( DRM_BYTE * )temp ), 0 )] )
         ^ *( ( DRM_DWORD* )T2[GET_BYTE( ( ( DRM_BYTE * )( temp + 1 ) ), 1 )] )
         ^ *( ( DRM_DWORD* )T3[GET_BYTE( ( ( DRM_BYTE * )( temp + 2 ) ), 2 )] )
         ^ *( ( DRM_DWORD* )T4[GET_BYTE( ( ( DRM_BYTE * )( temp + 3 ) ), 3 )] );

    c[1] = *( ( DRM_DWORD* )T1[GET_BYTE( ( ( DRM_BYTE * )( temp + 1 ) ), 0 )] )
         ^ *( ( DRM_DWORD* )T2[GET_BYTE( ( ( DRM_BYTE * )( temp + 2 ) ), 1 )] )
         ^ *( ( DRM_DWORD* )T3[GET_BYTE( ( ( DRM_BYTE * )( temp + 3 ) ), 2 )] )
         ^ *( ( DRM_DWORD* )T4[GET_BYTE( ( ( DRM_BYTE * )temp ), 3 )] );

    c[2] = *( ( DRM_DWORD* )T1[GET_BYTE( ( ( DRM_BYTE * )( temp + 2 ) ), 0 )] )
         ^ *( ( DRM_DWORD* )T2[GET_BYTE( ( ( DRM_BYTE * )( temp + 3 ) ), 1 )] )
         ^ *( ( DRM_DWORD* )T3[GET_BYTE( ( ( DRM_BYTE * )temp ), 2 )] )
         ^ *( ( DRM_DWORD* )T4[GET_BYTE( ( ( DRM_BYTE * )( temp + 1 ) ), 3 )] );

    c[3] = *( ( DRM_DWORD* )T1[GET_BYTE( ( ( DRM_BYTE * )( temp + 3 ) ), 0 )] )
         ^ *( ( DRM_DWORD* )T2[GET_BYTE( ( ( DRM_BYTE * )temp ), 1 )] )
         ^ *( ( DRM_DWORD* )T3[GET_BYTE( ( ( DRM_BYTE * )( temp + 1 ) ), 2 )] )
         ^ *( ( DRM_DWORD* )T4[GET_BYTE( ( ( DRM_BYTE * )( temp + 2 ) ), 3 )] );

    for ( r = 1; r < DRM_AES_ROUNDS_128 - 1; r++ )
    {
        temp[0] = c[0] ^ f_rgbKeys[r][0];
        temp[1] = c[1] ^ f_rgbKeys[r][1];
        temp[2] = c[2] ^ f_rgbKeys[r][2];
        temp[3] = c[3] ^ f_rgbKeys[r][3];

        c[0] = *( ( DRM_DWORD* )T1[GET_BYTE( ( ( DRM_BYTE * )temp ), 0 )] )
             ^ *( ( DRM_DWORD* )T2[GET_BYTE( ( ( DRM_BYTE * )( temp + 1 ) ), 1 )] )
             ^ *( ( DRM_DWORD* )T3[GET_BYTE( ( ( DRM_BYTE * )( temp + 2 ) ), 2 )] )
             ^ *( ( DRM_DWORD* )T4[GET_BYTE( ( ( DRM_BYTE * )( temp + 3 ) ), 3 )] );

        c[1] = *( ( DRM_DWORD* )T1[GET_BYTE( ( ( DRM_BYTE * )( temp + 1 ) ), 0 )] )
             ^ *( ( DRM_DWORD* )T2[GET_BYTE( ( ( DRM_BYTE * )( temp + 2 ) ), 1 )] )
             ^ *( ( DRM_DWORD* )T3[GET_BYTE( ( ( DRM_BYTE * )( temp + 3 ) ), 2 )] )
             ^ *( ( DRM_DWORD* )T4[GET_BYTE( ( ( DRM_BYTE * )temp ), 3 )] );

        c[2] = *( ( DRM_DWORD* )T1[GET_BYTE( ( ( DRM_BYTE * )( temp + 2 ) ), 0 )] )
             ^ *( ( DRM_DWORD* )T2[GET_BYTE( ( ( DRM_BYTE * )( temp + 3 ) ), 1 )] )
             ^ *( ( DRM_DWORD* )T3[GET_BYTE( ( ( DRM_BYTE * )temp ), 2 )] )
             ^ *( ( DRM_DWORD* )T4[GET_BYTE( ( ( DRM_BYTE * )( temp + 1 ) ), 3 )] );

        c[3] = *( ( DRM_DWORD* )T1[GET_BYTE( ( ( DRM_BYTE * )( temp + 3 ) ), 0 )] )
             ^ *( ( DRM_DWORD* )T2[GET_BYTE( ( ( DRM_BYTE * )temp ), 1 )] )
             ^ *( ( DRM_DWORD* )T3[GET_BYTE( ( ( DRM_BYTE * )( temp + 1 ) ), 2 )] )
             ^ *( ( DRM_DWORD* )T4[GET_BYTE( ( ( DRM_BYTE * )( temp + 2 ) ), 3 )] );
    }

    /*
    ** last round is special 
    */
    temp[0] = c[0] ^ f_rgbKeys[DRM_AES_ROUNDS_128-1][0];
    temp[1] = c[1] ^ f_rgbKeys[DRM_AES_ROUNDS_128-1][1];
    temp[2] = c[2] ^ f_rgbKeys[DRM_AES_ROUNDS_128-1][2];
    temp[3] = c[3] ^ f_rgbKeys[DRM_AES_ROUNDS_128-1][3];

    bTmp = GET_BYTE( ( ( DRM_BYTE * )temp ), 0 );
    bTmp = GET_BYTE( T1[bTmp], 1 );
    PUT_BYTE( d, 0, bTmp );

    bTmp = GET_BYTE( ( ( DRM_BYTE * )( temp + 1 ) ), 1 );
    bTmp = GET_BYTE( T1[bTmp], 1 );
    PUT_BYTE( d, 1, bTmp );

    bTmp = GET_BYTE( ( ( DRM_BYTE * )( temp + 2 ) ), 2 );
    bTmp = GET_BYTE( T1[bTmp], 1 );
    PUT_BYTE( d, 2, bTmp );

    bTmp = GET_BYTE( ( ( DRM_BYTE * )( temp + 3 ) ), 3 );
    bTmp = GET_BYTE( T1[bTmp], 1 );
    PUT_BYTE( d, 3, bTmp );

    bTmp = GET_BYTE( ( ( DRM_BYTE * )( temp + 1 ) ), 0 );
    bTmp = GET_BYTE( T1[bTmp], 1 );
    PUT_BYTE( d, 4, bTmp );

    bTmp = GET_BYTE( ( ( DRM_BYTE * )( temp + 2 ) ), 1 );
    bTmp = GET_BYTE( T1[bTmp], 1 );
    PUT_BYTE( d, 5, bTmp );

    bTmp = GET_BYTE( ( ( DRM_BYTE * )( temp + 3 ) ), 2 );
    bTmp = GET_BYTE( T1[bTmp], 1 );
    PUT_BYTE( d, 6, bTmp );

    bTmp = GET_BYTE( ( ( DRM_BYTE * )temp ), 3 );
    bTmp = GET_BYTE( T1[bTmp], 1 );
    PUT_BYTE( d, 7, bTmp );

    bTmp = GET_BYTE( ( ( DRM_BYTE * )( temp + 2 ) ), 0 );
    bTmp = GET_BYTE( T1[bTmp], 1 );
    PUT_BYTE( d, 8, bTmp );

    bTmp = GET_BYTE( ( ( DRM_BYTE * )( temp + 3 ) ), 1 );
    bTmp = GET_BYTE( T1[bTmp], 1 );
    PUT_BYTE( d, 9, bTmp );

    bTmp = GET_BYTE( ( ( DRM_BYTE * )temp ), 2 );
    bTmp = GET_BYTE( T1[bTmp], 1 );
    PUT_BYTE( d, 10, bTmp );

    bTmp = GET_BYTE( ( ( DRM_BYTE * )( temp + 1 ) ), 3 );
    bTmp = GET_BYTE( T1[bTmp], 1 );
    PUT_BYTE( d, 11, bTmp );

    bTmp = GET_BYTE( ( ( DRM_BYTE * )( temp + 3 ) ), 0 );
    bTmp = GET_BYTE( T1[bTmp], 1 );
    PUT_BYTE( d, 12, bTmp );

    bTmp = GET_BYTE( ( ( DRM_BYTE * )temp ), 1 );
    bTmp = GET_BYTE( T1[bTmp], 1 );
    PUT_BYTE( d, 13, bTmp );

    bTmp = GET_BYTE( ( ( DRM_BYTE * )( temp + 1 ) ), 2 );
    bTmp = GET_BYTE( T1[bTmp], 1 );
    PUT_BYTE( d, 14, bTmp );

    bTmp = GET_BYTE( ( ( DRM_BYTE * )( temp + 2 ) ), 3 );
    bTmp = GET_BYTE( T1[bTmp], 1 );
    PUT_BYTE( d, 15, bTmp );

    c[0] ^= f_rgbKeys[DRM_AES_ROUNDS_128][0];
    c[1] ^= f_rgbKeys[DRM_AES_ROUNDS_128][1];
    c[2] ^= f_rgbKeys[DRM_AES_ROUNDS_128][2];
    c[3] ^= f_rgbKeys[DRM_AES_ROUNDS_128][3];
    MEMCPY( f_rgbEncrypted, c, SIZEOF( c ) );

    return;    
}
#endif

/*********************************************************************************************
** Function:  Aes_Decrypt128
**
** Synopsis:  Decrypt a single block
**
** Arguments: 
**            [f_rgbClear]     : Returns the decrypted data
**            [f_rgbEncrypted] : Specifies the data to decrypt
**            [f_rgbKeys]      : The table to use for encryption
**
** Returns:   None
**********************************************************************************************/
DRM_API DRM_NO_INLINE DRM_VOID Aes_Decrypt128( 
    __out_bcount( DRM_AES_KEYSIZE_128 )       DRM_BYTE   f_rgbClear[__CB_DECL( DRM_AES_KEYSIZE_128 )],
    __in_bcount( DRM_AES_KEYSIZE_128 )  const DRM_BYTE   f_rgbEncrypted[__CB_DECL( DRM_AES_KEYSIZE_128 )],
    __in_ecount( DRM_AES_ROUNDS_128+1 ) const DRM_DWORD  f_rgbKeys[DRM_AES_ROUNDS_128+1][4] )
#if 1
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_DWORD r       = 0;
    DRM_DWORD temp[4] = { 0 };
    DRM_DWORD c[4]    = { 0 };
    DRM_BYTE  bTmp    = 0;

    MEMCPY( ( DRM_BYTE * )temp, f_rgbEncrypted, DRM_AES_KEYSIZE_128 );
    temp[0] ^= f_rgbKeys[DRM_AES_ROUNDS_128][0];
    temp[1] ^= f_rgbKeys[DRM_AES_ROUNDS_128][1];
    temp[2] ^= f_rgbKeys[DRM_AES_ROUNDS_128][2];
    temp[3] ^= f_rgbKeys[DRM_AES_ROUNDS_128][3];

    c[0] =  *( ( DRM_DWORD* )AES_T5[GET_BYTE( ( ( DRM_BYTE * )temp ), 0 )] )
          ^ *( ( DRM_DWORD* )AES_T6[GET_BYTE( ( ( DRM_BYTE * )( temp + 3 ) ), 1 )] )
          ^ *( ( DRM_DWORD* )AES_T7[GET_BYTE( ( ( DRM_BYTE * )( temp + 2 ) ), 2 )] )
          ^ *( ( DRM_DWORD* )AES_T8[GET_BYTE( ( ( DRM_BYTE * )( temp + 1 ) ), 3 )] );
    c[1] = *( ( DRM_DWORD* )AES_T5[GET_BYTE( ( ( DRM_BYTE * )( temp + 1 ) ), 0 )] )
         ^ *( ( DRM_DWORD* )AES_T6[GET_BYTE( ( ( DRM_BYTE * )temp ), 1 )] )
         ^ *( ( DRM_DWORD* )AES_T7[GET_BYTE( ( ( DRM_BYTE * )( temp + 3 ) ), 2 )] )
         ^ *( ( DRM_DWORD* )AES_T8[GET_BYTE( ( ( DRM_BYTE * )( temp + 2 ) ), 3 )] );
    c[2] = *( ( DRM_DWORD* )AES_T5[GET_BYTE( ( ( DRM_BYTE * )( temp + 2 ) ), 0 )] )
         ^ *( ( DRM_DWORD* )AES_T6[GET_BYTE( ( ( DRM_BYTE * )( temp + 1 ) ), 1 )] )
         ^ *( ( DRM_DWORD* )AES_T7[GET_BYTE( ( ( DRM_BYTE * )temp ), 2 )] )
         ^ *( ( DRM_DWORD* )AES_T8[GET_BYTE( ( ( DRM_BYTE * )( temp + 3 ) ), 3 )] );
    c[3] = *( ( DRM_DWORD* )AES_T5[GET_BYTE( ( ( DRM_BYTE * )( temp + 3 ) ), 0 )] )
         ^ *( ( DRM_DWORD* )AES_T6[GET_BYTE( ( ( DRM_BYTE * )( temp + 2 ) ), 1 )] )
         ^ *( ( DRM_DWORD* )AES_T7[GET_BYTE( ( ( DRM_BYTE * )( temp + 1 ) ), 2 )] )
         ^ *( ( DRM_DWORD* )AES_T8[GET_BYTE( ( ( DRM_BYTE * )temp ), 3 )] );

    for ( r = DRM_AES_ROUNDS_128 - 1; r > 1; r-- ) 
    {
        temp[0] = c[0] ^ f_rgbKeys[r][0];
        temp[1] = c[1] ^ f_rgbKeys[r][1];
        temp[2] = c[2] ^ f_rgbKeys[r][2];
        temp[3] = c[3] ^ f_rgbKeys[r][3];

        c[0] = *( ( DRM_DWORD* )AES_T5[GET_BYTE( ( ( DRM_BYTE * )temp ), 0 )] )
             ^ *( ( DRM_DWORD* )AES_T6[GET_BYTE( ( ( DRM_BYTE * )( temp + 3 ) ), 1 )] )
             ^ *( ( DRM_DWORD* )AES_T7[GET_BYTE( ( ( DRM_BYTE * )( temp + 2 ) ), 2 )] )
             ^ *( ( DRM_DWORD* )AES_T8[GET_BYTE( ( ( DRM_BYTE * )( temp + 1 ) ), 3 )] );

        c[1] = *( ( DRM_DWORD* )AES_T5[GET_BYTE( ( ( DRM_BYTE * )( temp + 1 ) ), 0 )] )
             ^ *( ( DRM_DWORD* )AES_T6[GET_BYTE( ( ( DRM_BYTE * )temp ), 1 )] )
             ^ *( ( DRM_DWORD* )AES_T7[GET_BYTE( ( ( DRM_BYTE * )( temp + 3 ) ), 2 )] )
             ^ *( ( DRM_DWORD* )AES_T8[GET_BYTE( ( ( DRM_BYTE * )( temp + 2 ) ), 3 )] );

        c[2] = *( ( DRM_DWORD* )AES_T5[GET_BYTE( ( ( DRM_BYTE * )( temp + 2 ) ), 0 )] )
             ^ *( ( DRM_DWORD* )AES_T6[GET_BYTE( ( ( DRM_BYTE * )( temp + 1 ) ), 1 )] )
             ^ *( ( DRM_DWORD* )AES_T7[GET_BYTE( ( ( DRM_BYTE * )temp ), 2 )] )
             ^ *( ( DRM_DWORD* )AES_T8[GET_BYTE( ( ( DRM_BYTE * )( temp + 3 ) ), 3 )] );

        c[3] = *( ( DRM_DWORD* )AES_T5[GET_BYTE( ( ( DRM_BYTE * )( temp + 3 ) ), 0 )] )
             ^ *( ( DRM_DWORD* )AES_T6[GET_BYTE( ( ( DRM_BYTE * )( temp + 2 ) ), 1 )] )
             ^ *( ( DRM_DWORD* )AES_T7[GET_BYTE( ( ( DRM_BYTE * )( temp + 1 ) ), 2 )] )
             ^ *( ( DRM_DWORD* )AES_T8[GET_BYTE( ( ( DRM_BYTE * )temp ), 3 )] );
    }

    /*
    ** last round is special 
    */
    MEMCPY( ( DRM_BYTE * )temp, ( DRM_BYTE * )c, DRM_AES_KEYSIZE_128 );
    temp[0] ^= f_rgbKeys[1][0];
    temp[1] ^= f_rgbKeys[1][1];
    temp[2] ^= f_rgbKeys[1][2];
    temp[3] ^= f_rgbKeys[1][3];

    bTmp = GET_BYTE( ( ( DRM_BYTE * )temp ), 0 );
    bTmp = GET_BYTE( AES_S5, bTmp );
    PUT_BYTE( ( DRM_BYTE * )c, 0, bTmp );

    bTmp = GET_BYTE( ( ( DRM_BYTE * )( temp + 3 ) ), 1 );
    bTmp = GET_BYTE( AES_S5, bTmp );
    PUT_BYTE( ( DRM_BYTE * )c, 1, bTmp );

    bTmp = GET_BYTE( ( ( DRM_BYTE * )( temp + 2 ) ), 2 );
    bTmp = GET_BYTE( AES_S5, bTmp );
    PUT_BYTE( ( DRM_BYTE * )c, 2, bTmp );

    bTmp = GET_BYTE( ( ( DRM_BYTE * )( temp + 1 ) ), 3 );
    bTmp = GET_BYTE( AES_S5, bTmp );
    PUT_BYTE( ( DRM_BYTE * )c, 3, bTmp );

    bTmp = GET_BYTE( ( ( DRM_BYTE * )( temp + 1 ) ), 0 );
    bTmp = GET_BYTE( AES_S5, bTmp );
    PUT_BYTE( ( DRM_BYTE * )c, 4, bTmp );

    bTmp = GET_BYTE( ( ( DRM_BYTE * )temp ), 1 );
    bTmp = GET_BYTE( AES_S5, bTmp );
    PUT_BYTE( ( DRM_BYTE * )c, 5, bTmp );

    bTmp = GET_BYTE( ( ( DRM_BYTE * )( temp + 3 ) ), 2 );
    bTmp = GET_BYTE( AES_S5, bTmp );
    PUT_BYTE( ( DRM_BYTE * )c, 6, bTmp );

    bTmp = GET_BYTE( ( ( DRM_BYTE * )( temp + 2 ) ), 3 );
    bTmp = GET_BYTE( AES_S5, bTmp );
    PUT_BYTE( ( DRM_BYTE * )c, 7, bTmp );

    bTmp = GET_BYTE( ( ( DRM_BYTE * )( temp + 2 ) ), 0 );
    bTmp = GET_BYTE( AES_S5, bTmp );
    PUT_BYTE( ( DRM_BYTE * )c, 8, bTmp );

    bTmp = GET_BYTE( ( ( DRM_BYTE * )( temp + 1 ) ), 1 );
    bTmp = GET_BYTE( AES_S5, bTmp );
    PUT_BYTE( ( DRM_BYTE * )c, 9, bTmp );

    bTmp = GET_BYTE( ( ( DRM_BYTE * )temp ), 2 );
    bTmp = GET_BYTE( AES_S5, bTmp );
    PUT_BYTE( ( DRM_BYTE * )c, 10, bTmp );

    bTmp = GET_BYTE( ( ( DRM_BYTE * )( temp + 3 ) ), 3 );
    bTmp = GET_BYTE( AES_S5, bTmp );
    PUT_BYTE( ( DRM_BYTE * )c, 11, bTmp );

    bTmp = GET_BYTE( ( ( DRM_BYTE * )( temp + 3 ) ), 0 );
    bTmp = GET_BYTE( AES_S5, bTmp );
    PUT_BYTE( ( DRM_BYTE * )c, 12, bTmp );

    bTmp = GET_BYTE( ( ( DRM_BYTE * )( temp + 2 ) ), 1 );
    bTmp = GET_BYTE( AES_S5, bTmp );
    PUT_BYTE( ( DRM_BYTE * )c, 13, bTmp );

    bTmp = GET_BYTE( ( ( DRM_BYTE * )( temp + 1 ) ), 2 );
    bTmp = GET_BYTE( AES_S5, bTmp );
    PUT_BYTE( ( DRM_BYTE * )c, 14, bTmp );

    bTmp = GET_BYTE( ( ( DRM_BYTE * )temp ), 3 );
    bTmp = GET_BYTE( AES_S5, bTmp );
    PUT_BYTE( ( DRM_BYTE * )c, 15, bTmp );

    c[0] ^= f_rgbKeys[0][0];
    c[1] ^= f_rgbKeys[0][1];
    c[2] ^= f_rgbKeys[0][2];
    c[3] ^= f_rgbKeys[0][3];

    MEMCPY( f_rgbClear, ( DRM_BYTE * )c, DRM_AES_KEYSIZE_128 );

    return;
}
#else
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_DWORD r       = 0;
    DRM_DWORD temp[4] = { 0 };
    DRM_DWORD c[4]    = { 0 };
    DRM_BYTE  bTmp    = 0;

    MEMCPY( ( DRM_BYTE * )temp, f_rgbEncrypted, DRM_AES_KEYSIZE_128 );
    temp[0] ^= f_rgbKeys[DRM_AES_ROUNDS_128][0];
    temp[1] ^= f_rgbKeys[DRM_AES_ROUNDS_128][1];
    temp[2] ^= f_rgbKeys[DRM_AES_ROUNDS_128][2];
    temp[3] ^= f_rgbKeys[DRM_AES_ROUNDS_128][3];

    c[0] =  *( ( DRM_DWORD* )T5[GET_BYTE( ( ( DRM_BYTE * )temp ), 0 )] )
          ^ *( ( DRM_DWORD* )T6[GET_BYTE( ( ( DRM_BYTE * )( temp + 3 ) ), 1 )] )
          ^ *( ( DRM_DWORD* )T7[GET_BYTE( ( ( DRM_BYTE * )( temp + 2 ) ), 2 )] )
          ^ *( ( DRM_DWORD* )T8[GET_BYTE( ( ( DRM_BYTE * )( temp + 1 ) ), 3 )] );
    c[1] = *( ( DRM_DWORD* )T5[GET_BYTE( ( ( DRM_BYTE * )( temp + 1 ) ), 0 )] )
         ^ *( ( DRM_DWORD* )T6[GET_BYTE( ( ( DRM_BYTE * )temp ), 1 )] )
         ^ *( ( DRM_DWORD* )T7[GET_BYTE( ( ( DRM_BYTE * )( temp + 3 ) ), 2 )] )
         ^ *( ( DRM_DWORD* )T8[GET_BYTE( ( ( DRM_BYTE * )( temp + 2 ) ), 3 )] );
    c[2] = *( ( DRM_DWORD* )T5[GET_BYTE( ( ( DRM_BYTE * )( temp + 2 ) ), 0 )] )
         ^ *( ( DRM_DWORD* )T6[GET_BYTE( ( ( DRM_BYTE * )( temp + 1 ) ), 1 )] )
         ^ *( ( DRM_DWORD* )T7[GET_BYTE( ( ( DRM_BYTE * )temp ), 2 )] )
         ^ *( ( DRM_DWORD* )T8[GET_BYTE( ( ( DRM_BYTE * )( temp + 3 ) ), 3 )] );
    c[3] = *( ( DRM_DWORD* )T5[GET_BYTE( ( ( DRM_BYTE * )( temp + 3 ) ), 0 )] )
         ^ *( ( DRM_DWORD* )T6[GET_BYTE( ( ( DRM_BYTE * )( temp + 2 ) ), 1 )] )
         ^ *( ( DRM_DWORD* )T7[GET_BYTE( ( ( DRM_BYTE * )( temp + 1 ) ), 2 )] )
         ^ *( ( DRM_DWORD* )T8[GET_BYTE( ( ( DRM_BYTE * )temp ), 3 )] );

    for ( r = DRM_AES_ROUNDS_128 - 1; r > 1; r-- ) 
    {
        temp[0] = c[0] ^ f_rgbKeys[r][0];
        temp[1] = c[1] ^ f_rgbKeys[r][1];
        temp[2] = c[2] ^ f_rgbKeys[r][2];
        temp[3] = c[3] ^ f_rgbKeys[r][3];

        c[0] = *( ( DRM_DWORD* )T5[GET_BYTE( ( ( DRM_BYTE * )temp ), 0 )] )
             ^ *( ( DRM_DWORD* )T6[GET_BYTE( ( ( DRM_BYTE * )( temp + 3 ) ), 1 )] )
             ^ *( ( DRM_DWORD* )T7[GET_BYTE( ( ( DRM_BYTE * )( temp + 2 ) ), 2 )] )
             ^ *( ( DRM_DWORD* )T8[GET_BYTE( ( ( DRM_BYTE * )( temp + 1 ) ), 3 )] );

        c[1] = *( ( DRM_DWORD* )T5[GET_BYTE( ( ( DRM_BYTE * )( temp + 1 ) ), 0 )] )
             ^ *( ( DRM_DWORD* )T6[GET_BYTE( ( ( DRM_BYTE * )temp ), 1 )] )
             ^ *( ( DRM_DWORD* )T7[GET_BYTE( ( ( DRM_BYTE * )( temp + 3 ) ), 2 )] )
             ^ *( ( DRM_DWORD* )T8[GET_BYTE( ( ( DRM_BYTE * )( temp + 2 ) ), 3 )] );

        c[2] = *( ( DRM_DWORD* )T5[GET_BYTE( ( ( DRM_BYTE * )( temp + 2 ) ), 0 )] )
             ^ *( ( DRM_DWORD* )T6[GET_BYTE( ( ( DRM_BYTE * )( temp + 1 ) ), 1 )] )
             ^ *( ( DRM_DWORD* )T7[GET_BYTE( ( ( DRM_BYTE * )temp ), 2 )] )
             ^ *( ( DRM_DWORD* )T8[GET_BYTE( ( ( DRM_BYTE * )( temp + 3 ) ), 3 )] );

        c[3] = *( ( DRM_DWORD* )T5[GET_BYTE( ( ( DRM_BYTE * )( temp + 3 ) ), 0 )] )
             ^ *( ( DRM_DWORD* )T6[GET_BYTE( ( ( DRM_BYTE * )( temp + 2 ) ), 1 )] )
             ^ *( ( DRM_DWORD* )T7[GET_BYTE( ( ( DRM_BYTE * )( temp + 1 ) ), 2 )] )
             ^ *( ( DRM_DWORD* )T8[GET_BYTE( ( ( DRM_BYTE * )temp ), 3 )] );
    }

    /*
    ** last round is special 
    */
    MEMCPY( ( DRM_BYTE * )temp, ( DRM_BYTE * )c, DRM_AES_KEYSIZE_128 );
    temp[0] ^= f_rgbKeys[1][0];
    temp[1] ^= f_rgbKeys[1][1];
    temp[2] ^= f_rgbKeys[1][2];
    temp[3] ^= f_rgbKeys[1][3];

    bTmp = GET_BYTE( ( ( DRM_BYTE * )temp ), 0 );
    bTmp = GET_BYTE( S5, bTmp );
    PUT_BYTE( ( DRM_BYTE * )c, 0, bTmp );

    bTmp = GET_BYTE( ( ( DRM_BYTE * )( temp + 3 ) ), 1 );
    bTmp = GET_BYTE( S5, bTmp );
    PUT_BYTE( ( DRM_BYTE * )c, 1, bTmp );

    bTmp = GET_BYTE( ( ( DRM_BYTE * )( temp + 2 ) ), 2 );
    bTmp = GET_BYTE( S5, bTmp );
    PUT_BYTE( ( DRM_BYTE * )c, 2, bTmp );

    bTmp = GET_BYTE( ( ( DRM_BYTE * )( temp + 1 ) ), 3 );
    bTmp = GET_BYTE( S5, bTmp );
    PUT_BYTE( ( DRM_BYTE * )c, 3, bTmp );

    bTmp = GET_BYTE( ( ( DRM_BYTE * )( temp + 1 ) ), 0 );
    bTmp = GET_BYTE( S5, bTmp );
    PUT_BYTE( ( DRM_BYTE * )c, 4, bTmp );

    bTmp = GET_BYTE( ( ( DRM_BYTE * )temp ), 1 );
    bTmp = GET_BYTE( S5, bTmp );
    PUT_BYTE( ( DRM_BYTE * )c, 5, bTmp );

    bTmp = GET_BYTE( ( ( DRM_BYTE * )( temp + 3 ) ), 2 );
    bTmp = GET_BYTE( S5, bTmp );
    PUT_BYTE( ( DRM_BYTE * )c, 6, bTmp );

    bTmp = GET_BYTE( ( ( DRM_BYTE * )( temp + 2 ) ), 3 );
    bTmp = GET_BYTE( S5, bTmp );
    PUT_BYTE( ( DRM_BYTE * )c, 7, bTmp );

    bTmp = GET_BYTE( ( ( DRM_BYTE * )( temp + 2 ) ), 0 );
    bTmp = GET_BYTE( S5, bTmp );
    PUT_BYTE( ( DRM_BYTE * )c, 8, bTmp );

    bTmp = GET_BYTE( ( ( DRM_BYTE * )( temp + 1 ) ), 1 );
    bTmp = GET_BYTE( S5, bTmp );
    PUT_BYTE( ( DRM_BYTE * )c, 9, bTmp );

    bTmp = GET_BYTE( ( ( DRM_BYTE * )temp ), 2 );
    bTmp = GET_BYTE( S5, bTmp );
    PUT_BYTE( ( DRM_BYTE * )c, 10, bTmp );

    bTmp = GET_BYTE( ( ( DRM_BYTE * )( temp + 3 ) ), 3 );
    bTmp = GET_BYTE( S5, bTmp );
    PUT_BYTE( ( DRM_BYTE * )c, 11, bTmp );

    bTmp = GET_BYTE( ( ( DRM_BYTE * )( temp + 3 ) ), 0 );
    bTmp = GET_BYTE( S5, bTmp );
    PUT_BYTE( ( DRM_BYTE * )c, 12, bTmp );

    bTmp = GET_BYTE( ( ( DRM_BYTE * )( temp + 2 ) ), 1 );
    bTmp = GET_BYTE( S5, bTmp );
    PUT_BYTE( ( DRM_BYTE * )c, 13, bTmp );

    bTmp = GET_BYTE( ( ( DRM_BYTE * )( temp + 1 ) ), 2 );
    bTmp = GET_BYTE( S5, bTmp );
    PUT_BYTE( ( DRM_BYTE * )c, 14, bTmp );

    bTmp = GET_BYTE( ( ( DRM_BYTE * )temp ), 3 );
    bTmp = GET_BYTE( S5, bTmp );
    PUT_BYTE( ( DRM_BYTE * )c, 15, bTmp );

    c[0] ^= f_rgbKeys[0][0];
    c[1] ^= f_rgbKeys[0][1];
    c[2] ^= f_rgbKeys[0][2];
    c[3] ^= f_rgbKeys[0][3];

    MEMCPY( f_rgbClear, ( DRM_BYTE * )c, DRM_AES_KEYSIZE_128 );

    return;
}

#endif
EXIT_PK_NAMESPACE_CODE;
