/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/
#include <stdio.h>

#include <drmcommon.h>
#include <drmcrt.h>
#include <drmstkalloc.h>
#include <oempkcrypto.h>
#include <oemcipher.h>
#include <drmcmdlnpars.h>
#define RUN_TEST( Function )                                                    \
    do {                                                                        \
            printf( "Testing " #Function ":" );                                 \
            fflush( stdout );                                                   \
            fResult = Function;                                                 \
            fTestFailed = !fResult || fTestFailed;                              \
            if ( fResult )                                                      \
            {                                                                   \
                printf( "PASS\n" );                                             \
            }                                                                   \
            else                                                                \
            {                                                                   \
                printf( "FAIL\n" );                                             \
            }                                                                   \
            fflush( stdout );                                                   \
        } while ( FALSE )

#define AES_TEST_CONTENT_SIZE DRM_AES_BLOCKLEN * 10

/**********************************************************************
**
** Function:    RC4EncryptDecryptTest
**
** Synopsis:    Tests encrypting and decrypting data symmetrically
**              using RC4/Cocktail
**
** Returns:     TRUE  if the test passed.
**              FALSE if the test failed.
***********************************************************************/
DRM_BOOL DRM_CALL RC4EncryptDecryptTest()
{
    DRM_RESULT         dr    = DRM_SUCCESS;
    DRM_BOOL           fRet  = FALSE;
    DRM_DWORD          cbLen = 0;
    DRM_BYTE           rgbLast15[__CB_DECL(15)] = { 0 };
    DRM_CIPHER_CONTEXT oContext = { (DRM_SUPPORTED_CIPHERS) 0, 0 };
    DRM_BYTE           rgbClearData[] =
    {
    'T', 'h', 'i', 's', ' ', 'i', 's', ' ',
    't', 'h', 'e', ' ', 'c', 'l', 'e', 'a',
    'r', ' ', 't', 'e', 'x', 't'
    };
    DRM_BYTE           rgbEncData[__CB_DECL(SIZEOF(rgbClearData))];
    DRM_BYTE           rgbContentKey[__CB_DECL(8)] = {
        1, 2, 3, 4, 5, 6, 7, 0
    };

    /*
    ** Test that data -> Encrypt -> Decrypt = data
    */
    ChkDR( DRM_CPHR_Init( &oContext, eDRM_RC4_CIPHER, DRMCIPHERKEYLEN_RC4, rgbContentKey) );
    MEMCPY( rgbEncData, rgbClearData, SIZEOF( rgbEncData ) );
    ChkDR( DRM_CPHR_Encrypt( &oContext, NULL, SIZEOF( rgbEncData ), rgbEncData ) );
    ChkDR( DRM_CPHR_Init( &oContext, eDRM_RC4_CIPHER, DRMCIPHERKEYLEN_RC4, rgbContentKey) );

    cbLen = SIZEOF( rgbEncData );
    if ( cbLen < 15 )
    {
        ChkDR( DRM_CPHR_InitDecrypt( &oContext, rgbEncData, cbLen ) );
    }
    else
    {
        DRM_BYT_CopyBytes( rgbLast15, 0, rgbEncData, cbLen - 15, 15 );
        ChkDR( DRM_CPHR_InitDecrypt( &oContext, rgbLast15, cbLen ) );
    }

    ChkDR( DRM_CPHR_Decrypt ( &oContext, NULL, SIZEOF( rgbEncData ), rgbEncData ) );

    fRet = ( MEMCMP(rgbClearData, rgbEncData, SIZEOF( rgbClearData ) ) == 0 );

ErrorExit:
    if ( DRM_FAILED( dr ) )
    {
        printf( "\tRC4EncryptDecryptTest failed with error code: 0x%u", dr );
        fRet = FALSE;
    }
    return fRet;
}

/**********************************************************************
**
** Function:    RC4KnownDataTest
**
** Synopsis:    Tests decrypting knwon data using RC4/Cocktail
**
** Returns:     TRUE  if the test passed.
**              FALSE if the test failed.
***********************************************************************/
DRM_BOOL DRM_CALL RC4KnownDataTest()
{
    DRM_RESULT         dr    = DRM_SUCCESS;
    DRM_BOOL           fRet  = FALSE;
    DRM_DWORD          cbLen = 0;
    DRM_BYTE           rgbLast15[__CB_DECL(15)] = { 0 };
    DRM_CIPHER_CONTEXT oContext = { (DRM_SUPPORTED_CIPHERS) 0, 0 };

    /* "Sample clear text.  DRM content encryption will be applied." */
    DRM_CHAR szClearText[] = "Sample clear text.  DRM content encryption will be applied.";

    DRM_BYTE rgbContentKey[__CB_DECL(8)] = {
        1, 2, 3, 4, 5, 6, 7, 0
    };

    DRM_BYTE rgbCipherText[] =
    {
        0x77, 0xd7, 0x89, 0xf2, 0x1a, 0xf5, 0x24, 0x30,
        0x2d, 0xe6, 0x9c, 0x28, 0x04, 0x36, 0x70, 0x28,
        0xd9, 0xa4, 0xd0, 0xe3, 0x6e, 0x46, 0x65, 0x3f,
        0x03, 0xee, 0xf6, 0x95, 0xfb, 0xe3, 0x58, 0xef,
        0xed, 0xa5, 0x01, 0xfa, 0x21, 0x32, 0x06, 0x6b,
        0xa3, 0x73, 0x86, 0xc8, 0xb2, 0x78, 0xc2, 0x20,
        0xab, 0x6a, 0xf1, 0x63, 0x64, 0x81, 0x13, 0x4d,
        0x28, 0xc2, 0x7d, 0x1c
    };

    /*
    ** Test that Decrypt of known CipherText and Key produces known ClearText.
    */
    ChkDR( DRM_CPHR_Init    (&oContext, eDRM_RC4_CIPHER, DRMCIPHERKEYLEN_RC4, rgbContentKey) );

    cbLen = SIZEOF(rgbCipherText);
    if ( cbLen < 15 )
    {
        ChkDR( DRM_CPHR_InitDecrypt( &oContext, rgbCipherText, cbLen ) );
    }
    else
    {
        DRM_BYT_CopyBytes( rgbLast15, 0, rgbCipherText, cbLen - 15, 15 );
        ChkDR( DRM_CPHR_InitDecrypt( &oContext, rgbLast15, cbLen ) );
    }

    ChkDR(DRM_CPHR_Decrypt( &oContext, NULL, cbLen/2, rgbCipherText ));
    ChkDR(DRM_CPHR_Decrypt( &oContext, NULL, SIZEOF(rgbCipherText) - (cbLen/2), rgbCipherText + __CB_DECL((cbLen/2)) ));
    fRet = (MEMCMP(rgbCipherText, szClearText, SIZEOF(rgbCipherText)) == 0 );


ErrorExit:
    if ( DRM_FAILED( dr ) )
    {
        printf( "\tRC4EncryptDecryptTest failed with error code: 0x%u", dr );
        fRet = FALSE;
    }
    return fRet;
}


/**********************************************************************
**
** Function:    AesCtr128EncryptDecryptTest
**
** Synopsis:    Tests encrypting and decrypting data symmetrically
**              using AES128 in Counter Mode
**
** Returns:     TRUE  if the test passed.
**              FALSE if the test failed.
***********************************************************************/
DRM_BOOL DRM_CALL AesCtr128EncryptDecryptTest()
{
    DRM_RESULT         dr       = DRM_SUCCESS;
    DRM_BOOL           fRet     = FALSE;
    DRM_CIPHER_CONTEXT oContext = { (DRM_SUPPORTED_CIPHERS) 0, 0 };
    DRM_AES_COUNTER_MODE_CONTEXT AesCtrContext                = { 0 };
    DRM_BYTE           rgbLast15[__CB_DECL(15)]               = { 0 };
    DRM_BYTE           rgbKey[ DRM_AES_KEYSIZE_128 ]          = { 0 };
    DRM_BYTE           rgbPlainText[ AES_TEST_CONTENT_SIZE ]  = { 0 };
    DRM_BYTE           rgbCipherText[ AES_TEST_CONTENT_SIZE ] = { 0 };
    DRM_DWORD          cbText                                 = AES_TEST_CONTENT_SIZE;
    DRM_BYTE          *pData                                  = rgbCipherText;

    Oem_Random_GetBytes( NULL, rgbKey,DRM_AES_KEYSIZE_128 );
    Oem_Random_GetBytes( NULL, rgbPlainText,AES_TEST_CONTENT_SIZE );
    MEMCPY( rgbCipherText, rgbPlainText, cbText );
    AesCtrContext.qwInitializationVector = DRM_UI64( 1 );
    AesCtrContext.qwBlockOffset          = DRM_UI64( 0 );
    AesCtrContext.bByteOffset            = 0x00;

    ChkDR( DRM_CPHR_Init( &oContext, eDRM_AES_COUNTER_CIPHER, DRM_AES_KEYSIZE_128, rgbKey ) );
    ChkDR( DRM_CPHR_Encrypt( &oContext, &AesCtrContext, AES_TEST_CONTENT_SIZE, rgbCipherText ) );
    ChkDR( DRM_CPHR_InitDecrypt( &oContext, rgbLast15, 15 ) );
    AesCtrContext.qwInitializationVector = DRM_UI64( 1 );
    AesCtrContext.qwBlockOffset          = DRM_UI64( 0 );
    AesCtrContext.bByteOffset            = 0x00;
    ChkDR( DRM_CPHR_Decrypt( &oContext, &AesCtrContext, DRM_AES_BLOCKLEN, pData ) );
    ChkDR( DRM_CPHR_Decrypt( &oContext, &AesCtrContext, DRM_AES_BLOCKLEN*3, pData + __CB_DECL( DRM_AES_BLOCKLEN ) ) );
    ChkDR( DRM_CPHR_Decrypt( &oContext, &AesCtrContext, DRM_AES_BLOCKLEN, pData + __CB_DECL( DRM_AES_BLOCKLEN * 4 ) ) );
    ChkDR( DRM_CPHR_Decrypt( &oContext, &AesCtrContext, DRM_AES_BLOCKLEN*2, pData + __CB_DECL( DRM_AES_BLOCKLEN * 5 ) ) );
    ChkDR( DRM_CPHR_Decrypt( &oContext, &AesCtrContext, DRM_AES_BLOCKLEN, pData + __CB_DECL( DRM_AES_BLOCKLEN * 7 ) ) );
    ChkDR( DRM_CPHR_Decrypt( &oContext, &AesCtrContext, DRM_AES_BLOCKLEN*2, pData + __CB_DECL( DRM_AES_BLOCKLEN * 8 ) ) );

    fRet = ( MEMCMP( rgbCipherText, rgbPlainText, AES_TEST_CONTENT_SIZE ) == 0 );

ErrorExit:
    if ( DRM_FAILED( dr ) )
    {
        printf( "\tAES Counter Mode Symmetric Encrypt Decrypt failed with error code: 0x%u", dr );
        fRet = FALSE;
    }
    return fRet;
}

/**********************************************************************
**
** Function:    AesCtr128KnownDataTest
**
** Synopsis:    Tests decrypting knwon data using AES128 in Counter Mode
**
** Returns:     TRUE  if the test passed.
**              FALSE if the test failed.
***********************************************************************/
DRM_BOOL DRM_CALL AesCtr128KnownDataTest()
{
    DRM_RESULT         dr       = DRM_SUCCESS;
    DRM_BOOL           fRet     = FALSE;
    DRM_BYTE           rgbLast15[__CB_DECL(15)] = { 0 };
    DRM_CIPHER_CONTEXT oContext = { (DRM_SUPPORTED_CIPHERS) 0, 0 };
    DRM_AES_COUNTER_MODE_CONTEXT AesCtrContext = {0};

    DRM_BYTE rgbAESCtrKnownPlaintext[] =
    {
        0x95, 0xdd, 0xad, 0xbf, 0x43, 0x03, 0xa6, 0x05,
        0xe7, 0x95, 0xbf, 0x06, 0x18, 0x46, 0xeb, 0xaf,
        0xff, 0x2a, 0xfe, 0xfa, 0xa7, 0x64, 0x4e, 0x19,
        0x94, 0x1d, 0xf4, 0x52, 0x43, 0x0a, 0xf7, 0x03,
        0x87, 0x9c, 0xd5, 0x35, 0xc3, 0x77, 0x09, 0x1a,
        0x2e, 0x7c, 0xfe, 0x8e, 0x86, 0x6e, 0x94, 0x45
    };
    DRM_DWORD cbAESCtrKnownPlaintext = SIZEOF( rgbAESCtrKnownPlaintext );

    /*
    ** 128-bit key and corresponding ciphertext
    */
    DRM_BYTE rgbAESCtr128Key[] =
    {
        0x60, 0x09, 0x5e, 0xcd, 0xf6, 0x1e, 0xa4, 0x6b,
        0xac, 0x69, 0xc4, 0xab, 0x32, 0x5d, 0x72, 0x08
    };
    DRM_BYTE rgbAESCtr128KnownCiphertext[] =
    {
        0xaa, 0x9b, 0x36, 0x83, 0xeb, 0x9d, 0xee, 0xab,
        0x06, 0xbe, 0xb3, 0x63, 0xa3, 0x90, 0x84, 0xee,
        0x29, 0x3c, 0xf9, 0x91, 0xf3, 0x7b, 0x59, 0xd5,
        0x63, 0x9e, 0x50, 0xeb, 0x1f, 0x70, 0x07, 0x3a,
        0xeb, 0x38, 0x50, 0x9d, 0xa4, 0x30, 0x6a, 0x8c,
        0x94, 0x04, 0x13, 0x96, 0x53, 0x7d, 0x4b, 0xb7
    };
    DRM_DWORD cbAESCtr128KnownCiphertext = SIZEOF( rgbAESCtr128KnownCiphertext );


    /*
    ** Test that Decrypt of known CipherText and Key produces known ClearText.
    */
    AesCtrContext.qwInitializationVector = DRM_UI64( 1 );
    AesCtrContext.qwBlockOffset          = DRM_UI64( 0 );
    AesCtrContext.bByteOffset            = 0x00;
    ChkDR( DRM_CPHR_Init( &oContext, eDRM_AES_COUNTER_CIPHER, DRM_AES_KEYSIZE_128, rgbAESCtr128Key) );
    ChkDR( DRM_CPHR_InitDecrypt( &oContext, rgbLast15, 15 ) );
    ChkDR( DRM_CPHR_Decrypt( &oContext, &AesCtrContext, cbAESCtr128KnownCiphertext, rgbAESCtr128KnownCiphertext ) );
    fRet = ( MEMCMP(rgbAESCtr128KnownCiphertext, rgbAESCtrKnownPlaintext, cbAESCtrKnownPlaintext ) == 0 );


ErrorExit:
    if ( DRM_FAILED( dr ) )
    {
        printf( "\tAES Counter Mode Known Data failed with error code: 0x%u", dr );
        fRet = FALSE;
    }
    return fRet;
}

/**********************************************************************
**
** Function:    OemAesEncryptDecryptAlignmentTest
**
** Synopsis:    Tests using Oem_AesEncryptOne directly. It optionally
**              misaligns the data used for encrypt and decrypt.
**
**              [f_fMisAlign]   If TRUE the data is intentionally
**                              taken out of DWORD alignment
**
** Returns:     TRUE  if the test passed.
**              FALSE if the test failed.
***********************************************************************/
DRM_BOOL DRM_CALL OemAesEncryptDecryptAlignmentTest( __in DRM_BOOL f_fMisAlign )
{
    DRM_BOOL     fRet          = FALSE;
    DRM_RESULT   dr            = DRM_SUCCESS;
    DRM_BYTE    *pbDataClr     = NULL;
    DRM_BYTE    *pbData        = NULL;
    DRM_BYTE    *pbDataOffset  = NULL;
    DRM_DWORD    dwOffset      = 0;
    DRM_BYTE     cbData        = DRM_AES_BLOCKLEN;
    DRM_BYTE     rgbKey[__CB_DECL( DRM_AES_KEYSIZE_128 )] = { 0 };
    DRM_AES_KEY  oAesKey        = { 0 };

    /*
    ** Test set key
    */
    ChkDR( Oem_Random_GetBytes( NULL, rgbKey, DRM_AES_KEYSIZE_128 ) );

    ChkDR( Oem_Aes_SetKey( rgbKey, &oAesKey ) );

    if ( f_fMisAlign )
    {
        while( DRM_IS_DWORD_ALIGNED( cbData ) )
        {
            cbData += 1;
            dwOffset++;
        }
    }
    /*
    ** Test encrypt
    */
    ChkMem( pbDataClr = ( DRM_BYTE * )Oem_MemAlloc( cbData ) );
    ChkMem( pbData    = ( DRM_BYTE * )Oem_MemAlloc( cbData ) );
    ChkDR( Oem_Random_GetBytes( NULL, pbDataClr, cbData ) );

    MEMCPY( pbData, pbDataClr, cbData );


    pbDataOffset = pbData;
    if ( f_fMisAlign )
    {
        pbDataOffset += dwOffset;
    }

    ChkDR( Oem_Aes_EncryptOne( &oAesKey, pbDataOffset ) );
    ChkBOOL( MEMCMP( pbData, pbDataClr, DRM_AES_BLOCKLEN ) != 0,
             DRM_E_FAIL );

    /*
    ** Test decrypt
    */
    ChkDR( Oem_Aes_DecryptOne( &oAesKey, pbDataOffset ) );
    fRet = ( MEMCMP( pbData, pbDataClr, DRM_AES_BLOCKLEN ) == 0);

    /*
    ** Test zero key
    */
    ChkDR( Oem_Aes_ZeroKey( &oAesKey ) );

ErrorExit:
    SAFE_OEM_FREE( pbData );
    SAFE_OEM_FREE( pbDataClr );

    if ( DRM_FAILED( dr ) )
    {
        printf( "\tOEM_AES failed with error code: 0x%u", dr );
        fRet = FALSE;
    }
    return fRet;
}


DRM_LONG DRM_CALL DRM_Main(
    IN                  DRM_LONG argc,
    __in_ecount( argc ) DRM_WCHAR** argv )
{
    DRM_BOOL           fResult        = FALSE;
    DRM_BOOL           fTestFailed    = FALSE;

    printf( "Microsoft (R) %s\nCopyright (c) Microsoft Corporation. All rights reserved.\n\n", "Test Cipher application" );

    RUN_TEST( RC4KnownDataTest() );
    RUN_TEST( RC4EncryptDecryptTest() );
    RUN_TEST( OemAesEncryptDecryptAlignmentTest( FALSE ) );
    RUN_TEST( OemAesEncryptDecryptAlignmentTest( TRUE ) );
    RUN_TEST( AesCtr128KnownDataTest() );
    RUN_TEST( AesCtr128EncryptDecryptTest() );

    if ( fTestFailed )
    {
        printf("failed\n");
        return 1;
    }
    return 0;
}
