/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdio.h>
#include <time.h>
#include <drmcommon.h>
#include <drmcrt.h>
#include <drmstkalloc.h>
#include <oempkcrypto.h>
#include <oemeccp160.h>
#include <oemeccp256.h>
#include <drmcmdlnpars.h>

#define PRINT_ECCP256_BUFFERS 0

#define RUN_TEST( Function )                                                    \
    do {                                                                        \
            printf( "Testing " #Function ":" );                                 \
            fflush( stdout );                                                   \
            fResult = Function;                                                 \
            fTestFailed = !fResult || fTestFailed;                              \
            printf( fResult ? "PASS\n" : "FAIL\n" );                            \
            fflush( stdout );                                                   \
        } while ( FALSE )


/*******************************************************************************
** Function:    ECC_P160_Verify_Interop_test
**
** Synopsis:    Performs Basic testing to ensure that the specific implementation
**              of ECCP160 PK algorithms works with the MSDRM implementation
*******************************************************************************/
DRM_BOOL DRM_CALL ECC_P160_Verify_Interop_test(DRM_VOID *f_pContext)
{
    DRM_BOOL  fRet = FALSE;
    PUBKEY pubkey =
    {
        0xB1, 0xC7, 0x62, 0xFE, 0x99, 0x53, 0x08, 0x69,
        0x02, 0x08, 0xB4, 0xCC, 0x6D, 0x14, 0xAB, 0x98,
        0x5C, 0x74, 0x51, 0x52, 0x8F, 0x6C, 0x3D, 0x54,
        0x02, 0x12, 0xFA, 0x04, 0xE7, 0x9A, 0x46, 0x6E,
        0x2B, 0xFE, 0x71, 0xDF, 0x8F, 0x28, 0x58, 0x03
    };
    DRM_BYTE rgbMessage [__CB_DECL(DRM_ECC160_PLAINTEXT_LEN)] =
    {
        0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
        0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10
    };
    DRM_BYTE rgbSignature [__CB_DECL(DRM_ECC160_SIGNATURE_LEN)] =
    {
        0x07, 0xf7, 0x99, 0x1d, 0x40, 0xc9, 0x5e, 0xb2,
        0x56, 0x98, 0x95, 0x0e, 0xac, 0xce, 0x2d, 0xc9,
        0x32, 0x3c, 0xa2, 0x69, 0x3f, 0xf9, 0x29, 0x0e,
        0x78, 0x6b, 0xca, 0x3a, 0x26, 0x00, 0x33, 0xa3,
        0x08, 0x8f, 0xaa, 0x87, 0x31, 0x5b, 0x78, 0x63
    };

    fRet = DRM_SUCCEEDED( DRM_PK_Verify (f_pContext,
                                         eDRM_ECC_P160,
                                         DRM_ECC160_PUBLIC_KEY_LEN,
                                         ( const DRM_BYTE * )&pubkey,
                                         SIZEOF (rgbMessage),
                                         rgbMessage,
                                         DRM_ECC160_SIGNATURE_LEN,
                                         rgbSignature ) );
    return fRet;
}
/*******************************************************************************
** Function:    ECC_P160_Decrypt_Interop_test
**
** Synopsis:    Performs Basic testing to ensure that the specific implementation
**              of ECCP160 PK algorithms works with the MSDRM implementation
*******************************************************************************/
DRM_BOOL DRM_CALL ECC_P160_Decrypt_Interop_test(DRM_VOID *f_pContext)
{
    DRM_BOOL  fRet = FALSE;
    DRM_BYTE  rgbPlainText [__CB_DECL(DRM_ECC160_CIPHERTEXT_LEN)] = { 0 };
    DRM_DWORD cbPlainText = DRM_ECC160_CIPHERTEXT_LEN;

    PRIVKEY privkey =
    {
        0xD2, 0x2D, 0xA5, 0xC6, 0xBF, 0xB6, 0x2C, 0x18,
        0x1A, 0x52, 0xC8, 0x93, 0xD0, 0xEB, 0x2C, 0x70,
        0xD5, 0x10, 0x21, 0x41
    };
    DRM_BYTE rgbMessage [__CB_DECL(DRM_ECC160_PLAINTEXT_LEN)] =
    {
        0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
        0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10
    };
    DRM_BYTE rgbCipherText [__CB_DECL(DRM_ECC160_CIPHERTEXT_LEN)] =
    {
        0x03, 0x12, 0xF3, 0x84, 0xEA, 0xFA, 0xC5, 0xFA,
        0x89, 0x52, 0x9F, 0x03, 0x9D, 0x05, 0xC5, 0x6C,
        0x74, 0x10, 0xE6, 0x39, 0xAD, 0xF9, 0xD0, 0x4F,
        0x20, 0x4D, 0xED, 0x33, 0xF0, 0xFE, 0xDA, 0xEE,
        0x8F, 0x48, 0x8C, 0x3B, 0x8C, 0xF2, 0x83, 0x78,
        0x03, 0x03, 0xB6, 0x3E, 0x9F, 0x4B, 0x40, 0x64,
        0x2B, 0xD1, 0x2D, 0xA4, 0xF4, 0x37, 0xB5, 0x40,
        0xCE, 0x69, 0x9F, 0x16, 0x38, 0x2A, 0xD2, 0xF2,
        0x72, 0xBB, 0x79, 0xCE, 0xD4, 0x54, 0x2F, 0x1B,
        0xEC, 0x5E, 0xDD, 0x95, 0x11, 0x09, 0xDF, 0x4F
    };

    if (DRM_SUCCEEDED (DRM_PK_Decrypt (f_pContext,
                                       eDRM_ECC_P160,
                                       DRM_ECC160_PRIVATE_KEY_LEN,
                                       ( const DRM_BYTE * )&privkey,
                                       DRM_ECC160_CIPHERTEXT_LEN,
                                       rgbCipherText,
                                       &cbPlainText,
                                       rgbPlainText)))
    {
        if (MEMCMP(rgbPlainText, rgbMessage, DRM_ECC160_PLAINTEXT_LEN) == 0)
        {
            fRet = TRUE;
        }
    }
    return fRet;
}
/*******************************************************************************
** Function:    ECC_P160_EncryptDecrypt
**
** Synopsis:    Tests ECCP160 PK_Encrypt/Decrypt  mulitple times
**              and optionally reports timing information.
**
**              [f_fTiming]    Boolean value stating to gather perf information.
**
** Notes:       DRM_SUPPORT_PROFILING needs to be enabled in the build to actually
**              gather timing information, otherwise timing is reported as 0.0
*******************************************************************************/
DRM_BOOL DRM_CALL ECC_P160_EncryptDecrypt(const DRM_BOOL f_fTiming, DRM_VOID *f_pContext)
{
    DRM_RESULT dr   = DRM_SUCCESS;
    DRM_BOOL   fRet = TRUE;
    PUBKEY     pubkey      = { 0 };
    PRIVKEY    privkey     = { 0 };
    DRM_BYTE   rgbIn        [__CB_DECL(DRM_ECC160_PLAINTEXT_LEN)]  = { 0 },
               rgbCipher    [__CB_DECL(DRM_ECC160_CIPHERTEXT_LEN)] = { 0 },
               rgbOut       [__CB_DECL(DRM_ECC160_PLAINTEXT_LEN)]  = { 0 };
    DRM_LONG   iRepeat     = 0,
               cRepeats    = 0;
    DRM_DWORD  cbPubKey    = DRM_ECC160_PUBLIC_KEY_LEN;
    DRM_DWORD  cbPrivKey   = DRM_ECC160_PRIVATE_KEY_LEN;
    DRM_DWORD  ticks1      = 0,
               ticks2      = 0,
               ticks3      = 0;

    if (f_fTiming)
    {
        cRepeats = 100;
    }
    else
    {
        cRepeats = 1;
    }

    ZEROMEM(&pubkey,  SIZEOF (pubkey));
    ZEROMEM(&privkey, SIZEOF (privkey));
    ZEROMEM(rgbIn,    SIZEOF (rgbIn));
    ZEROMEM(rgbCipher,SIZEOF (rgbCipher));

    ChkDR(DRM_PK_GenKeyPair (f_pContext,
                                       eDRM_ECC_P160,
                                       eDRM_RSA_UNKNOWN,
                                       &cbPubKey,
                                       ( DRM_BYTE * )&pubkey,
                                       &cbPrivKey,
                                       ( DRM_BYTE * )&privkey));

    Oem_Random_GetBytes(NULL, rgbIn, SIZEOF (rgbIn));

    ticks1 = Oem_Clock_GetTickCount( NULL );
    for (iRepeat = 0; iRepeat < cRepeats; iRepeat++)
    {
        DRM_DWORD cbEncrypted = SIZEOF( rgbCipher );
        ChkDR(DRM_PK_Encrypt (NULL,
                                        f_pContext,
                                        eDRM_ECC_P160,
                                        DRM_ECC160_PUBLIC_KEY_LEN,
                                        ( DRM_BYTE * )&pubkey,
                                        DRM_ECC160_PLAINTEXT_LEN,
                                        rgbIn,
                                        &cbEncrypted,
                                        rgbCipher));

    } /* for iRepeat */

    ticks2 = Oem_Clock_GetTickCount( NULL );

    for (iRepeat = 0; iRepeat < cRepeats; iRepeat++)
    {
        DRM_DWORD cbClear = SIZEOF( rgbOut );
        ChkDR(DRM_PK_Decrypt (f_pContext,
                                        eDRM_ECC_P160,
                                        DRM_ECC160_PRIVATE_KEY_LEN,
                                        ( const DRM_BYTE * )&privkey,
                                        DRM_ECC160_CIPHERTEXT_LEN,
                                        rgbCipher,
                                        &cbClear,
                                        rgbOut));
    } /* for iRepeat */

    ticks3 = Oem_Clock_GetTickCount( NULL );

    if (MEMCMP (rgbIn, rgbOut, DRM_ECC160_PLAINTEXT_LEN) != 0)
    {
        fRet = FALSE;
    }
    if (f_fTiming)
    {
        printf ("\n\tEncrypt: %8.3f mSecs\n\tDecrypt: %8.3f mSecs\n\tResult: ",
                (double) (ticks2 - ticks1) / (double) (cRepeats),
                (double) (ticks3 - ticks2) / (double) (cRepeats));
        fflush(stdout);
    }
ErrorExit:
    if ( DRM_FAILED( dr ) )
    {
        printf( "\tECC_P160_EncryptDecrypt failed with error code: 0x%u", dr );
        fRet = FALSE;
    }
    return fRet;
}
/*******************************************************************************
** Function:    ECC_P160_SignVerify
**
** Synopsis:    Tests ECCP160 PK_Sign/Verify mulitple times
**              and opitonally reports timing information.
**
**              [f_fTiming]    Boolean value stating to gather perf information.
**
** Notes:       DRM_SUPPORT_PROFILING needs to be enabled in the build to actually
**              gather timing information, otherwise timing is reported as 0.0
*******************************************************************************/
DRM_BOOL DRM_CALL ECC_P160_SignVerify(const DRM_BOOL f_fTiming, DRM_VOID *f_pContext)
{
    DRM_RESULT dr          = DRM_SUCCESS;
    DRM_BOOL   fRet        = TRUE;
    PUBKEY     pubkey      = { 0 };
    PRIVKEY    privkey     = { 0 };
    DRM_BYTE   rgbMessage   [__CB_DECL(24)] = { 0 },
               rgbBadMessage   [__CB_DECL(24)] = { 0 },
               rgbSignature [__CB_DECL(DRM_ECC160_SIGNATURE_LEN)] = { 0 };
    DRM_LONG   iRepeat     = 0,
               cRepeats    = 0;
    DRM_DWORD  ticks1      = 0,
               ticks2      = 0,
               start       = 0,
               end         = 0;

    rgbBadMessage[0] = 1;

    if (f_fTiming)
    {
        cRepeats = 100;
    }
    else
    {
        cRepeats = 10000;
    }


    for (iRepeat = 0; iRepeat != cRepeats; iRepeat++)
    {
        DRM_DWORD cbSignature = SIZEOF( rgbSignature );
        DRM_DWORD  cbPubKey    = DRM_ECC160_PUBLIC_KEY_LEN;
        DRM_DWORD  cbPrivKey   = DRM_ECC160_PRIVATE_KEY_LEN;

        ZEROMEM(&pubkey,  SIZEOF (pubkey));
        ZEROMEM(&privkey, SIZEOF (privkey));

        ChkDR(DRM_PK_GenKeyPair ( f_pContext,
                              eDRM_ECC_P160,
                              eDRM_RSA_UNKNOWN,
                              &cbPubKey,
                              ( DRM_BYTE * )&pubkey,
                              &cbPrivKey,
                              ( DRM_BYTE * )&privkey));

        Oem_Random_GetBytes(NULL, rgbMessage, SIZEOF (rgbMessage));

        start = Oem_Clock_GetTickCount( NULL );
        ChkDR(DRM_PK_Sign(f_pContext,
                                    eDRM_ECC_P160,
                                    DRM_ECC160_PRIVATE_KEY_LEN,
                                    ( const DRM_BYTE * )&privkey,
                                    SIZEOF (rgbMessage),
                                    (DRM_BYTE *)rgbMessage,
                                    &cbSignature,
                                    (DRM_BYTE *)rgbSignature));
        end = Oem_Clock_GetTickCount( NULL );
        ticks1 += end - start;

        start = Oem_Clock_GetTickCount( NULL );

        dr = DRM_PK_Verify( f_pContext,
                            eDRM_ECC_P160,
                            DRM_ECC160_PUBLIC_KEY_LEN,
                            ( const DRM_BYTE * )&pubkey,
                            SIZEOF (rgbMessage),
                            (const DRM_BYTE *)rgbMessage,
                            DRM_ECC160_SIGNATURE_LEN,
                            (DRM_BYTE *)rgbSignature);
        end = Oem_Clock_GetTickCount( NULL );

        ticks2 += end - start;

        ChkDR( dr );

    }

    dr = DRM_PK_Verify( f_pContext,
        eDRM_ECC_P160,
        DRM_ECC160_PUBLIC_KEY_LEN,
        ( const DRM_BYTE * )&pubkey,
        SIZEOF (rgbMessage),
        (const DRM_BYTE *)rgbBadMessage,
        DRM_ECC160_SIGNATURE_LEN,
        (DRM_BYTE *)rgbSignature);

    if ( dr == DRM_SUCCESS )
    {
        dr = DRM_E_FAIL;
    }
    else if( dr == DRM_E_INVALID_SIGNATURE )
    {
        dr = DRM_SUCCESS;
    }

    ChkDR( dr );


    if (f_fTiming)
    {
        printf ("\n\tSign: %8.3f mSecs\n\tVerify: %8.3f mSecs\n\tResult: ",
             (double)(ticks1)/(double)(cRepeats),
             (double)(ticks2)/(double)(cRepeats));
        fflush(stdout);
    }
ErrorExit:
    if ( DRM_FAILED( dr ) )
    {
        printf( "\tECC_P160_SignVerify failed with error code: 0x%u", dr );
        fRet = FALSE;
    }
    return fRet;
}

/*******************************************************************************
** Function:    _PrintBytes
**
** Synopsis:    Prints an array of bytes in a form that it can be included in
**              DRM source code.
**
**              [f_szBName]  The name to geve the array variable in the printout.
**              [f_szCName]  The name to give the count variable in the printout.
**              [f_pbData]   Array to print
**              [f_cbData]   number of bytes in the array to print.
**
**
** Notes:       Prints the data in the following format:
**              DRM_DWORD   f_szCName = f_cbData;
**              DRM_BYTE    f_szBName[  __CB_DECL( f_cbData) ] =
**              {
**                  0x00, 0x01, 0x02, 0x03, 0x04, 0x05 ...
**              };
*******************************************************************************/
#if PRINT_ECCP256_BUFFERS
static DRM_VOID DRM_CALL _PrintBytes(
    __in_z                  DRM_CHAR *f_szBName,
    __in_z                  DRM_CHAR *f_szCName,
    __in_bcount( f_cbData ) DRM_BYTE *f_pbData,
    __in                    DRM_DWORD f_cbData )
{
    DRM_DWORD dwIndex  = 0;
    DRM_DWORD cbEven   = 0;
    DRM_DWORD dwColumn = 0;

    if( f_szBName == NULL )
    {
        printf( "_PrintBytes, f_szBName cannot be NULL");
    }
    if( f_szCName == NULL )
    {
        printf( "_PrintBytes, f_szCName cannot be NULL");
    }
    if( f_pbData == NULL )
    {
        printf( "_PrintBytes, f_pbData cannot be NULL");
    }

    cbEven = f_cbData - ( f_cbData % 2 );
    printf( "\tDRM_DWORD %s = %d;\n", f_szCName, f_cbData );
    printf( "\tDRM_BYTE  %s[ __CB_DECL(%d) ] = \n\t{\n\t\t", f_szBName, f_cbData );
    while( dwIndex < cbEven )
    {
        printf( "TWO_BYTES( " );
        if ( f_pbData[ dwIndex ] > 0xF )
        {
            printf( "0x%2X", f_pbData[ dwIndex ] );
        }
        else
        {
            printf("0x0%1X", f_pbData[ dwIndex ] );
        }
        printf( ", " );
        dwIndex++;
        if ( f_pbData[ dwIndex ] > 0xF )
        {
            printf( "0x%2X", f_pbData[ dwIndex ] );
        }
        else
        {
            printf("0x0%1X", f_pbData[ dwIndex ] );
        }
        dwIndex++;
        dwColumn++;
        if ( dwIndex == cbEven )
        {
            printf( ")" );
        }
        else
        {
            printf( "), " );
            if ( dwColumn % 4 == 0 )
            {
                printf( "\n\t\t" );
            }
        }
    }
    if ( dwIndex < f_cbData )
    {
        printf( ", TWO_BYTES( 0x%2X, 0x00 )", f_pbData[dwIndex]);
    }
    printf( "\n\t};\n" );
    fflush( stdout );
}
#endif

/*******************************************************************************
** Function:    ECC_P256_Verify_Interop_test
**
** Synopsis:    Performs Basic testing to ensure that the specific implementation
**              of ECCP256 PK algorithms works with the MSDRM implementation
**
** Returns:     TRUE  if the test passed.
**              FASLE if the test failed
*******************************************************************************/
DRM_BOOL DRM_CALL ECC_P256_Verify_Interop_test(DRM_VOID *f_pContext)
{
    DRM_BOOL    fRet = FALSE;
    DRM_DWORD   cbKnownMessage    = 24;
    DRM_BYTE    rgbPubKey[ __CB_DECL( ECC_P256_POINT_SIZE_IN_BYTES ) ]  =
    {
        0x28, 0xE2, 0x3E, 0xD2, 0x72, 0xBA, 0xC2, 0xD5,
        0xCD, 0x06, 0x04, 0x84, 0xE8, 0x12, 0x52, 0xA8,
        0x08, 0x3E, 0xEE, 0xC0, 0x9E, 0x09, 0x60, 0xED,
        0xE0, 0x42, 0x08, 0x55, 0xC1, 0xB5, 0x69, 0x22,
        0x87, 0x15, 0x12, 0x04, 0x1A, 0xE3, 0x6E, 0xC0,
        0xF6, 0x33, 0x99, 0x18, 0x78, 0x38, 0x9A, 0x29,
        0xB2, 0x89, 0xB8, 0xEA, 0x52, 0x42, 0x32, 0xF9,
        0xB8, 0xA8, 0xE2, 0x92, 0x4E, 0xCB, 0x1D, 0x5C
    };
    DRM_BYTE     rgbKnownMessage[ __CB_DECL( 24 ) ] =
    {
        0x80, 0x44, 0x45, 0xAF, 0xD4, 0x3A, 0x4D, 0x87,
        0x0C, 0xA3, 0x0E, 0x2A, 0xB1, 0xF0, 0xA7, 0xD7,
        0x16, 0xD2, 0x20, 0x25, 0x54, 0x59, 0x1A, 0xCE
    };
    DRM_BYTE     rgbKnownSigntaure[ __CB_DECL( ECC_P256_POINT_SIZE_IN_BYTES ) ] =
    {
        0xE8, 0x15, 0xEC, 0xDC, 0xF2, 0x71, 0xDC, 0x6D,
        0xC1, 0x1B, 0x92, 0x4B, 0x17, 0x4E, 0x13, 0x6B,
        0x34, 0x40, 0x6A, 0xCB, 0x3A, 0x13, 0xBD, 0x90,
        0xBC, 0x67, 0xDE, 0xB1, 0x0F, 0x54, 0x3F, 0xD5,
        0x6D, 0xB5, 0xBB, 0xEC, 0x9A, 0x47, 0x1D, 0xB1,
        0x61, 0x7F, 0x25, 0xFB, 0x0A, 0x70, 0x1C, 0x63,
        0xD5, 0x9F, 0x15, 0xFB, 0xC1, 0xCD, 0x7A, 0xD7,
        0xF4, 0x3F, 0xA2, 0x76, 0xB6, 0x0D, 0x7E, 0x9A
    };


    /*
    ** Test interop of DRM_PK_Verify
    */

    fRet = DRM_SUCCEEDED( DRM_PK_Verify (f_pContext,
                                         eDRM_ECC_P256,
                                         ECC_P256_POINT_SIZE_IN_BYTES,
                                         rgbPubKey,
                                         cbKnownMessage,
                                         rgbKnownMessage,
                                         ECC_P256_POINT_SIZE_IN_BYTES,
                                         rgbKnownSigntaure ) );

    return fRet;
}

/*******************************************************************************
** Function:    ECC_P256_Verify_Interop_test
**
** Synopsis:    Performs Basic testing to ensure that the specific implementation
**              of ECCP256 PK algorithms works with the MSDRM implementation
**
** Returns:     TRUE  if the test passed.
**              FASLE if the test failed
*******************************************************************************/
DRM_BOOL DRM_CALL ECC_P256_Decrypt_Interop_test(DRM_VOID *f_pContext)
{
    DRM_BOOL     fRet = FALSE;
    DRM_DWORD    cbNewPlainText    = ECC_P256_PLAINTEXT_SIZE_IN_BYTES;
    DRM_BYTE     rgbNewPlainText[ __CB_DECL( ECC_P256_PLAINTEXT_SIZE_IN_BYTES ) ] = { 0 };
    DRM_BYTE     rgbPrivKey[  __CB_DECL( ECC_P256_INTEGER_SIZE_IN_BYTES ) ] =
    {
        0xEA, 0xA0, 0x63, 0xE6, 0x6C, 0x44, 0x75, 0x71,
        0xB1, 0x92, 0xD8, 0x78, 0x19, 0xA8, 0x60, 0x7E,
        0xA9, 0xB0, 0x4F, 0xFC, 0x96, 0x09, 0x11, 0xA2,
        0x4C, 0xBB, 0xFE, 0x78, 0x15, 0x02, 0x0E, 0xA3
    };

    DRM_BYTE     rgbKnownPlainText[ __CB_DECL( ECC_P256_PLAINTEXT_SIZE_IN_BYTES ) ] =
    {
        0x51, 0xCD, 0xC5, 0x7E, 0x88, 0x9B, 0x00, 0x55,
        0xE9, 0x63, 0x41, 0xD9, 0x02, 0x89, 0xFA, 0xC1,
        0x88, 0x3A, 0x00, 0x57, 0xC6, 0x32, 0xB7, 0x5B,
        0xA0, 0xB5, 0xA7, 0x34, 0xD1, 0x56, 0x97, 0x99
    };
    DRM_BYTE     rgbKnownCipherText[ __CB_DECL( ECC_P256_CIPHERTEXT_SIZE_IN_BYTES ) ] =
    {
        0x07, 0x03, 0x69, 0xCC, 0xEC, 0x8C, 0x9F, 0x2A,
        0xDA, 0x1B, 0x58, 0x98, 0xE4, 0x03, 0xCF, 0x3A,
        0x51, 0x6C, 0x78, 0x67, 0xB3, 0x8D, 0x18, 0x0D,
        0x25, 0xBB, 0x76, 0x9F, 0x31, 0x3F, 0x0A, 0x47,
        0x58, 0xD0, 0x40, 0x48, 0x6E, 0x3F, 0x54, 0xE0,
        0x15, 0xD3, 0x3A, 0x6F, 0xEE, 0x0B, 0x3F, 0x7C,
        0xCB, 0xC5, 0x18, 0x76, 0xFE, 0x0E, 0x38, 0x37,
        0x51, 0xD1, 0x05, 0x84, 0xD1, 0x2B, 0x4F, 0x8E,
        0x42, 0xB5, 0xB0, 0xDB, 0xAD, 0xC7, 0x38, 0x7F,
        0x0E, 0xB8, 0xC7, 0xEF, 0x92, 0x51, 0x23, 0xC5,
        0xBE, 0x23, 0x1A, 0xEE, 0x1F, 0xF5, 0x94, 0x3A,
        0x87, 0x76, 0xFE, 0x8C, 0x54, 0xDD, 0x2E, 0x0A,
        0x82, 0xAB, 0x21, 0x9A, 0x42, 0x40, 0x42, 0x35,
        0xAD, 0x72, 0x77, 0x17, 0xD4, 0x2C, 0x60, 0xE4,
        0xD3, 0x89, 0xDF, 0x48, 0xEA, 0x82, 0xBF, 0x88,
        0xD5, 0x4F, 0xF0, 0xC2, 0xDD, 0xB5, 0x50, 0xE0
    };

    fRet = FALSE;

    if (DRM_SUCCEEDED (DRM_PK_Decrypt (f_pContext,
                                       eDRM_ECC_P256,
                                       ECC_P256_INTEGER_SIZE_IN_BYTES,
                                       rgbPrivKey,
                                       ECC_P256_CIPHERTEXT_SIZE_IN_BYTES,
                                       rgbKnownCipherText,
                                       &cbNewPlainText,
                                       rgbNewPlainText)))
    {
        if (MEMCMP(rgbNewPlainText, rgbKnownPlainText, ECC_P256_PLAINTEXT_SIZE_IN_BYTES) == 0)
        {
            fRet = TRUE;
        }
    }

    return fRet;
}

/*******************************************************************************
** Function:    ECC_P256_E2E_test
**
** Synopsis:    Tests ECCP256 PK_Encrypt/Decrypt mulitple times
**              and opitonally reports timing information.
**
**              [f_fTiming]    Boolean value stating to gather perf information.
**
** Notes:       DRM_SUPPORT_PROFILING needs to be enabled in the build to actually
**              gather timing information, otherwise timing is reported as 0.0
*******************************************************************************/
DRM_BOOL DRM_CALL ECC_P256_EncryptDecrypt(const DRM_BOOL f_fTiming, DRM_VOID *f_pContext)
{
    DRM_RESULT     dr        = DRM_SUCCESS;
    PUBKEY_P256    pubkey    = { 0 };
    PRIVKEY_P256   privkey   = { 0 };
    PLAINTEXT_P256 plaintext = { 0 };
    DRM_BYTE       rgbCipher    [__CB_DECL(ECC_P256_CIPHERTEXT_SIZE_IN_BYTES)] = { 0 },
                   rgbOut       [__CB_DECL(ECC_P256_PLAINTEXT_SIZE_IN_BYTES)]  = { 0 };
    DRM_LONG       iRepeat     = 0,
                   cRepeats    = 0;
    DRM_DWORD      cbPubKey    = ECC_P256_POINT_SIZE_IN_BYTES;
    DRM_DWORD      cbPrivKey   = ECC_P256_INTEGER_SIZE_IN_BYTES;
    DRM_BOOL       fRet        = TRUE;
    DRM_DWORD      ticks1      = 0,
                   ticks2      = 0,
                   ticks3      = 0;

    if (f_fTiming)
    {
        cRepeats = 100;
    }
    else
    {
        cRepeats = 1;
    }

    ZEROMEM(&pubkey,  SIZEOF (pubkey));
    ZEROMEM(&privkey, SIZEOF (privkey));
    ZEROMEM(plaintext.m_rgbPlaintext,    SIZEOF (plaintext));
    ZEROMEM(rgbCipher,SIZEOF (rgbCipher));

    /*
    ** Generate a keypair
    */

    ChkDR( DRM_PK_GenKeyPair ( f_pContext,
                               eDRM_ECC_P256,
                               eDRM_RSA_UNKNOWN,
                               &cbPubKey,
                               ( DRM_BYTE * )&pubkey,
                               &cbPrivKey,
                               ( DRM_BYTE * )&privkey ) );

    #if PRINT_ECCP256_BUFFERS
    _PrintBytes("rgbECCP256PubKey", "cbECCP256PubKey", ( DRM_BYTE * )&pubkey,cbPubKey );
    _PrintBytes("rgbECCP256PrivKey", "cbECCP256PrivKey", ( DRM_BYTE * )&privkey,cbPrivKey );
    #endif

    Oem_Random_GetBytes(NULL, plaintext.m_rgbPlaintext, SIZEOF (plaintext.m_rgbPlaintext));
    ChkDR( OEM_ECC_GenerateHMACKey_P256( &plaintext, (struct bigctx_t *)f_pContext ) );

    ticks1 = Oem_Clock_GetTickCount( NULL );
    for (iRepeat = 0; iRepeat != cRepeats; iRepeat++)
    {
        DRM_DWORD cbEncrypted = SIZEOF( rgbCipher );
        ChkDR( DRM_PK_Encrypt ( NULL,
                                f_pContext,
                                eDRM_ECC_P256,
                                ECC_P256_POINT_SIZE_IN_BYTES,
                                ( DRM_BYTE * )&pubkey,
                                ECC_P256_PLAINTEXT_SIZE_IN_BYTES,
                                plaintext.m_rgbPlaintext,
                               &cbEncrypted,
                                rgbCipher));

        #if PRINT_ECCP256_BUFFERS
        _PrintBytes("rgbECCP256Clear", "cbECCP256Clear", plaintext.m_rgbPlaintext,ECC_P256_PLAINTEXT_SIZE_IN_BYTES );
        _PrintBytes("rgbECCP256Cipher", "cbECCP256Cipher", rgbCipher,cbEncrypted );
        #endif

    } /* for iRepeat */

    ticks2 = Oem_Clock_GetTickCount( NULL );

    for (iRepeat = 0; iRepeat != cRepeats; iRepeat++)
    {
        DRM_DWORD cbClear = SIZEOF( rgbOut );
        ChkDR( DRM_PK_Decrypt ( f_pContext,
                                eDRM_ECC_P256,
                                ECC_P256_INTEGER_SIZE_IN_BYTES,
                                ( const DRM_BYTE * )&privkey,
                                ECC_P256_CIPHERTEXT_SIZE_IN_BYTES,
                                rgbCipher,
                               &cbClear,
                                rgbOut) );

    } /* for iRepeat */

    ticks3 = Oem_Clock_GetTickCount( NULL );

    if (MEMCMP (plaintext.m_rgbPlaintext, rgbOut, ECC_P256_PLAINTEXT_SIZE_IN_BYTES) != 0)
    {
        fRet = FALSE;
    }
    if (f_fTiming)
    {
        printf ("\n\tEncrypt: %8.3f mSecs\n\tDecrypt: %8.3f mSecs\n\tResult: ",
                (double) (ticks2 - ticks1) / (double) (cRepeats),
                (double) (ticks3 - ticks2) / (double) (cRepeats));
        fflush(stdout);
    }
ErrorExit:
    if ( DRM_FAILED( dr ) )
    {
        printf( "\tECC_P256_EncryptDecrypt failed with error code: 0x%u", dr );
        fRet = FALSE;
    }
    return fRet;
}
/*******************************************************************************
** Function:    ECC_P256_SignVerify
**
** Synopsis:    Tests ECCP256 PK_Sign/Verify mulitple times
**              and opitonally reports timing information.
**
**              [f_fTiming]    Boolean value stating to gather perf information.
**
** Notes:       DRM_SUPPORT_PROFILING needs to be enabled in the build to actually
**              gather timing information, otherwise timing is reported as 0.0
*******************************************************************************/
DRM_BOOL DRM_CALL ECC_P256_SignVerify(const DRM_BOOL f_fTiming, DRM_VOID *f_pContext)
{
    DRM_RESULT     dr        = DRM_SUCCESS;
    PUBKEY_P256    pubkey    = { 0 };
    PRIVKEY_P256   privkey   = { 0 };
    DRM_BYTE       rgbMessage   [__CB_DECL(24)] = { 0 },
                   rgbSignature [__CB_DECL(ECC_P256_POINT_SIZE_IN_BYTES)] = { 0 };
    DRM_LONG       iRepeat     = 0,
                   cRepeats    = 0;
    DRM_DWORD      cbPubKey    = ECC_P256_POINT_SIZE_IN_BYTES;
    DRM_DWORD      cbPrivKey   = ECC_P256_INTEGER_SIZE_IN_BYTES;
    DRM_BOOL       fRet        = TRUE;
    DRM_DWORD      ticks1      = 0,
                   ticks2      = 0,
                   ticks3      = 0;

    if (f_fTiming)
    {
        cRepeats = 100;
    }
    else
    {
        cRepeats = 1;
    }

    ZEROMEM(&pubkey,  SIZEOF (pubkey));
    ZEROMEM(&privkey, SIZEOF (privkey));

    /*
    ** Generate a keypair
    */

    ChkDR(DRM_PK_GenKeyPair ( f_pContext,
                              eDRM_ECC_P256,
                              eDRM_RSA_UNKNOWN,
                              &cbPubKey,
                              ( DRM_BYTE * )&pubkey,
                              &cbPrivKey,
                              ( DRM_BYTE * )&privkey));

    #if PRINT_ECCP256_BUFFERS
    _PrintBytes("rgbECCP256PubKey", "cbECCP256PubKey", ( DRM_BYTE * )&pubkey,cbPubKey );
    _PrintBytes("rgbECCP256PrivKey", "cbECCP256PrivKey", ( DRM_BYTE * )&privkey,cbPrivKey );
    #endif

   Oem_Random_GetBytes(NULL, rgbMessage, SIZEOF (rgbMessage));

   ticks1 = Oem_Clock_GetTickCount( NULL );
   for (iRepeat = 0; iRepeat != cRepeats; iRepeat++)
   {
       DRM_DWORD cbSignature = SIZEOF( rgbSignature );
       ChkDR( DRM_PK_Sign( f_pContext,
                           eDRM_ECC_P256,
                           ECC_P256_INTEGER_SIZE_IN_BYTES,
                           ( const DRM_BYTE * )&privkey,
                           SIZEOF ( rgbMessage ),
                           (DRM_BYTE *)rgbMessage,
                           &cbSignature,
                           (DRM_BYTE *)rgbSignature ) );
       #if PRINT_ECCP256_BUFFERS
       _PrintBytes("rgbECCP256Message", "cbECCP256Message", (DRM_BYTE *)rgbMessage,SIZEOF (rgbMessage) );
       _PrintBytes("rgbECCP256Signtaure", "cbECCP256Signature", rgbSignature,cbSignature );
       #endif
   } /* for iRepeat */

   ticks2 = Oem_Clock_GetTickCount( NULL );
   for (iRepeat = 0; iRepeat != cRepeats; iRepeat++)
   {
       dr = DRM_PK_Verify( f_pContext,
                           eDRM_ECC_P256,
                           ECC_P256_POINT_SIZE_IN_BYTES,
                           ( const DRM_BYTE * )&pubkey,
                           SIZEOF (rgbMessage),
                           (const DRM_BYTE *)rgbMessage,
                           ECC_P256_POINT_SIZE_IN_BYTES,
                           (DRM_BYTE *)rgbSignature);
       if( dr == DRM_E_INVALID_SIGNATURE )
       {
           fRet = FALSE;
           ChkDR( DRM_SUCCESS );
       }
   } /* for iRepeat */
   ticks3 = Oem_Clock_GetTickCount( NULL );
   if (f_fTiming)
    {
        printf ("\n\tSign: %8.3f mSecs\n\tVerify: %8.3f mSecs\n\tResult: ",
             (double)(ticks2 - ticks1)/(double)(cRepeats),
             (double)(ticks3 - ticks2)/(double)(cRepeats));
        fflush(stdout);
    }
ErrorExit:
    if ( DRM_FAILED( dr ) )
    {
        printf( "\tECC_P256_SignVerify failed with error code: 0x%u", dr );
        fRet = FALSE;
    }
    return fRet;
}


DRM_LONG DRM_CALL DRM_Main(
    IN                  DRM_LONG argc,
    __in_ecount( argc ) DRM_WCHAR** argv )
{
    DRM_BYTE          *pbContextCrypto = NULL;
    DRM_BOOL           fResult         = FALSE;
    DRM_BOOL           fTestFailed     = FALSE;
    printf( "Microsoft (R) %s\nCopyright (c) Microsoft Corporation. All rights reserved.\n\n", "Test Crypto Application" );

    pbContextCrypto = (DRM_BYTE*)Oem_MemAlloc( __CB_DECL(DRM_PKCRYPTO_CONTEXT_BUFFER_SIZE) );
    if( pbContextCrypto == NULL )
    {
        return 1;
    }

    #if !PRINT_ECCP256_BUFFERS
    ZEROMEM(pbContextCrypto, __CB_DECL(DRM_PKCRYPTO_CONTEXT_BUFFER_SIZE));
    RUN_TEST( ECC_P160_Verify_Interop_test(pbContextCrypto) );
    ZEROMEM(pbContextCrypto, __CB_DECL(DRM_PKCRYPTO_CONTEXT_BUFFER_SIZE));
    RUN_TEST( ECC_P160_Decrypt_Interop_test(pbContextCrypto) );
    ZEROMEM(pbContextCrypto, __CB_DECL(DRM_PKCRYPTO_CONTEXT_BUFFER_SIZE));
    RUN_TEST( ECC_P256_Verify_Interop_test(pbContextCrypto) );
    ZEROMEM(pbContextCrypto, __CB_DECL(DRM_PKCRYPTO_CONTEXT_BUFFER_SIZE));
    RUN_TEST( ECC_P256_Decrypt_Interop_test(pbContextCrypto) );
    ZEROMEM(pbContextCrypto, __CB_DECL(DRM_PKCRYPTO_CONTEXT_BUFFER_SIZE));
    RUN_TEST( ECC_P160_EncryptDecrypt(TRUE,pbContextCrypto) );
    ZEROMEM(pbContextCrypto, __CB_DECL(DRM_PKCRYPTO_CONTEXT_BUFFER_SIZE));
    RUN_TEST( ECC_P160_SignVerify(TRUE,pbContextCrypto) );
    ZEROMEM(pbContextCrypto, __CB_DECL(DRM_PKCRYPTO_CONTEXT_BUFFER_SIZE));
    RUN_TEST( ECC_P160_SignVerify(FALSE,pbContextCrypto) );
    ZEROMEM(pbContextCrypto, __CB_DECL(DRM_PKCRYPTO_CONTEXT_BUFFER_SIZE));
    RUN_TEST( ECC_P256_EncryptDecrypt(TRUE,pbContextCrypto) );
    ZEROMEM(pbContextCrypto, __CB_DECL(DRM_PKCRYPTO_CONTEXT_BUFFER_SIZE));
    RUN_TEST( ECC_P256_SignVerify(TRUE,pbContextCrypto) );
    #else
    ZEROMEM(pbContextCrypto, __CB_DECL(DRM_PKCRYPTO_CONTEXT_BUFFER_SIZE));
    RUN_TEST( ECC_P256_EncryptDecrypt(FALSE,pbContextCrypto) );
    ZEROMEM(pbContextCrypto, __CB_DECL(DRM_PKCRYPTO_CONTEXT_BUFFER_SIZE));
    RUN_TEST( ECC_P256_SignVerify(FALSE,pbContextCrypto) );
    #endif

    SAFE_OEM_FREE( pbContextCrypto );

    if ( fTestFailed )
    {
        return 1;
    }

    return 0;
}
