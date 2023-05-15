/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include "cryptotest.h"
#include "tstcryptodata.h"
#include <oemeccp160.h>

/*
Alignment tests are enabled only in Win32 builds on 64bit and Windows CE platforms.

if ALIGNMENT_TESTING_ENABLED is defined, the code checks whether byte buffers are
DWORD-aligned and, if so, shifts the buffer to make it misaligned, hence testing
the PK code ability, or lack of it, to deal with misaligned data.
*/

#if ( defined(_WIN32_WCE) || ( defined(_WIN32) && ( defined(_M_X64) || defined(_AMD64_) ) ) )
#include <windows.h>
#define ALIGNMENT_TESTING_ENABLED
#endif

/*
On 64bit Windows (Intel and AMD), these functions enable
and disable alignment exceptions for the current process.
The default processor and OS behavior is "Disabled," so functions
have to call setacx64 to be able to catch alignment exceptions.
On ARM and Windows CE, alignment exceptions
are enabled by default, hence the #else
*/
#if (defined(_WIN32) && (defined(_M_X64) || defined(_AMD64_)))
    extern "C" unsigned __int64 setacx64();
    extern "C" unsigned __int64 clearacx64();
#else
#define setacx64()
#define clearacx64()
#endif

ENTER_PKTEST_NAMESPACE_CODE

/*
** Static key variables
*/
DRM_BYTE  g_pszPlainKey[DRM_ECC160_PLAINTEXT_LEN] = { 7, 'P', 'l', 'a', 'i', 'n', 'K', 'y' }; /*max of 7 - export limit */
DRM_BYTE *g_pszKey                                = NULL;
DRM_BYTE *g_pbCryptoContext                       = NULL;
DRM_CRYPTO_CONTEXT g_Crypto                       = { 0 };
DRM_BYTE *g_pbRandBufIn                           = NULL;
DRM_BYTE *g_pbCipher                              = NULL;
DRM_BYTE *g_pbDecryptBufOut                       = NULL;
PUBKEY   *g_pPubKey                               = NULL;
PRIVKEY  *g_pPrivKey                              = NULL;

DRM_CIPHER_CONTEXT  g_Context     = { (enum _DRM_SUPPORTED_CIPHERS)0, 0 };
DRM_CIPHER_CONTEXT *g_pContext    = NULL;
DRM_HMAC_CONTEXT    g_HmacContext = DRM_HMAC_CONTEXT_DEFAULT;
RC4_KEYSTRUCT       g_rc4ks       = { 0 };
DESTable            g_desTable    = { 0 };
DRM_BYTE           *g_pbSymmetricEncryptedData = NULL;
DRM_BYTE           *g_pbSymmetricClearData     = NULL;
DRM_UINT64          g_qwSymmetricDataID        = DRM_UI64LITERAL( 0, 0 );
/*
** CBC Hashing
*/
DRM_BYTE   *g_pbOrgDataCBC   = NULL;
DRM_BYTE   *g_pbEncDataCBC   = NULL;
DRM_CBCKey  g_mackey         = { 0 };
DRM_CBCKey  g_invmackey      = { 0 };

DRM_AES_KEY  g_aesKey                         = { 0 };
DRM_AES_COUNTER_MODE_CONTEXT g_AesCtrContext  = { 0 };

#ifdef ALIGNMENT_TESTING_ENABLED
/*
** global alignment-specific flags
*/
DRM_BOOL g_fCipherTextAlignmentChanged = FALSE;
DRM_BOOL g_fPlainTextAlignmentChanged = FALSE;

static int _AlignmentExceptionHandler( DWORD dwExceptionCode )
{
    if ( dwExceptionCode == STATUS_DATATYPE_MISALIGNMENT )
    {
        return(EXCEPTION_EXECUTE_HANDLER);
    }
    return(EXCEPTION_CONTINUE_SEARCH);
}
#endif

/*
** _ParseSizeString
**
** Parses szSize for Sentinal sizes or converts it to a dword.
** it will also look for add a number found after the sentinel to the sentinel value (1 off tests)
**
** EX: "P160_PRIVKEY1", "P160_PRIVKEY+1", "P160_PRIVKEY 1", and "P160_PRIVKEY +1" all set f_pdwSize
**          to the ECCP160 Privkey length + 1
**     "P160_PRIVKEY-1" and "P160_PRIVKEY -1" set it to the ECC P160 Privkey length -1
**
** NB: This works by calling strtol on whatever is after the Sentinel and adding them, so
**     "P160_PUBKEY - 1" (note the space after '-') won't work
**
** Sentinel Sizes are:
**  P256_PUBKEY
**  P256_PRIVKEY
**  P256_CIPHERTEXT
**  P256_PLAINTEXT
**  P256_SIGNATURE
**  P160_PUBKEY
**  P160_PRIVKEY
**  P160_CIPHERTEXT
**  P160_PLAINTEXT
**  P160_SIGNATURE
**  RSA_1024_MAX_PLAINTEXT
**  RSA_2048_MAX_PLAINTEXT
**  RSA_PUBKEY
**  RSA_PRIVKEY
**  RSA_1024_MODULUS
**  RSA_2048_MODULUS
**  SHA_1_DIGEST
**  SHA_1_BLOCK_SIZE
**  SHA_256_DIGEST
**  SHA_256_BLOCK_SIZE
**
** Arguments:
**      [f_szSize]  - Null Terminated string to parse
**      [f_pdwSize] - Output DWORD
**
** Returns:
**      DRM_SUCCESS - on Success
**      DRM_E_TEST_INVALID_ARG  - Invalid argument
*/
static DRM_RESULT DRM_CALL _ParseSizeString(
    __in_z char* f_szSize,
    __out_ecount( 1 ) DRM_DWORD *f_pdwSize )
{
    DRM_RESULT dr      = DRM_SUCCESS;
    char      *pchEnd  = NULL;

    ChkArg( f_szSize  != NULL );
    ChkArg( f_pdwSize != NULL );

    *f_pdwSize = (DRM_DWORD) strtoul( f_szSize , &pchEnd, 0);

    if( *f_pdwSize == 0 && f_szSize == pchEnd )
    {
        if( DRMCRT_strncmp(f_szSize, "P256", 4 ) == 0 )
        {
            if( DRMCRT_strncmp( f_szSize, "P256_PUBKEY", 11 ) == 0 )
            {
                *f_pdwSize = SIZEOF( PUBKEY_P256 );
                pchEnd = f_szSize + 11;
            }
            else if( DRMCRT_strncmp( f_szSize, "P256_PRIVKEY", 12 ) == 0 )
            {
                *f_pdwSize = SIZEOF( PRIVKEY_P256 );
                pchEnd = f_szSize + 12;
            }
            else if( DRMCRT_strncmp( f_szSize, "P256_CIPHERTEXT", 15 ) == 0 )
            {
                *f_pdwSize = SIZEOF( CIPHERTEXT_P256 );
                pchEnd = f_szSize + 15;
            }
            else if( DRMCRT_strncmp( f_szSize, "P256_PLAINTEXT", 14 ) == 0 )
            {
                *f_pdwSize = SIZEOF( PLAINTEXT_P256 );
                pchEnd = f_szSize + 14;
            }
            else if( DRMCRT_strncmp( f_szSize, "P256_SIGNATURE", 14 ) == 0 )
            {
                *f_pdwSize = SIZEOF( SIGNATURE_P256 );
                pchEnd = f_szSize + 14;
            }
        }
        else if( DRMCRT_strncmp( f_szSize, "P160", 4 ) == 0 )
        {
            if( DRMCRT_strncmp( f_szSize, "P160_PUBKEY", 11 ) == 0 )
            {
                *f_pdwSize = __CB_DECL( DRM_ECC160_PUBLIC_KEY_LEN );
                pchEnd = f_szSize + 11;
            }
            else if( DRMCRT_strncmp( f_szSize, "P160_PRIVKEY", 12 ) == 0 )
            {
                *f_pdwSize = __CB_DECL( DRM_ECC160_PRIVATE_KEY_LEN );
                pchEnd = f_szSize + 12;
            }
            else if( DRMCRT_strncmp( f_szSize, "P160_CIPHERTEXT", 15 ) == 0 )
            {
                *f_pdwSize = __CB_DECL( DRM_ECC160_CIPHERTEXT_LEN );
                pchEnd = f_szSize + 15;
            }
            else if( DRMCRT_strncmp( f_szSize, "P160_PLAINTEXT", 14 ) == 0 )
            {
                *f_pdwSize = __CB_DECL( DRM_ECC160_PLAINTEXT_LEN );
                pchEnd = f_szSize + 14;
            }
            else if( DRMCRT_strncmp( f_szSize, "P160_SIGNATURE", 14 ) == 0 )
            {
                *f_pdwSize = __CB_DECL( DRM_ECC160_SIGNATURE_LEN );
                pchEnd = f_szSize + 14;
            }
        }
        else if( DRMCRT_strncmp( f_szSize, "RSA", 3 ) == 0 )
        {
            if( DRMCRT_strncmp( f_szSize, "RSA_1024_MAX_PLAINTEXT", 22 ) == 0 )
            {
                *f_pdwSize = DRM_RSA_CB_MAX_PLAINTEXT( eDRM_RSA_1024 );
                pchEnd = f_szSize + 22;
            }
            else if( DRMCRT_strncmp( f_szSize, "RSA_2048_MAX_PLAINTEXT", 22 ) == 0 )
            {
                *f_pdwSize = DRM_RSA_CB_MAX_PLAINTEXT( eDRM_RSA_2048 );
                pchEnd = f_szSize + 22;
            }
            else if( DRMCRT_strncmp( f_szSize, "RSA_PUBKEY", 10 ) == 0 )
            {
                *f_pdwSize = DRM_RSA_CB_PUBKEY;
                pchEnd = f_szSize + 10;
            }
            else if( DRMCRT_strncmp( f_szSize, "RSA_PRIVKEY", 11 ) == 0 )
            {
                *f_pdwSize = DRM_RSA_CB_PRIVKEY;
                pchEnd = f_szSize + 11;
            }
            else if( DRMCRT_strncmp( f_szSize, "RSA_1024_MODULUS", 16 ) == 0 )
            {
                *f_pdwSize = DRM_RSA_CB_MODULUS( eDRM_RSA_1024 );
                pchEnd = f_szSize + 16;
            }
            else if( DRMCRT_strncmp( f_szSize, "RSA_2048_MODULUS", 16 ) == 0 )
            {
                *f_pdwSize = DRM_RSA_CB_MODULUS( eDRM_RSA_2048 );
                pchEnd = f_szSize + 16;
            }
        }
        else if( DRMCRT_strncmp( f_szSize, "SHA", 3 ) == 0 )
        {
            if( DRMCRT_strncmp( f_szSize, "SHA_1_DIGEST", 12 ) == 0 )
            {
                *f_pdwSize = DRM_SHA1_DIGEST_LEN;
                pchEnd = f_szSize + 12;
            }
            else if( DRMCRT_strncmp( f_szSize, "SHA_1_BLOCK_SIZE", 16 ) == 0 )
            {
                *f_pdwSize = DRM_SHA1_BLOCK_SIZE;
                pchEnd = f_szSize + 16;
            }
            else if( DRMCRT_strncmp( f_szSize, "SHA_256_DIGEST", 14 ) == 0 )
            {
                *f_pdwSize = SHA256_DIGEST_SIZE_IN_BYTES;
                pchEnd = f_szSize + 14;
            }
            else if( DRMCRT_strncmp( f_szSize, "SHA_256_BLOCK_SIZE", 18 ) == 0 )
            {
                *f_pdwSize = SHA256_BLOCK_SIZE_IN_BYTES;
                pchEnd = f_szSize + 18;
            }
        }
        else
        {
            ChkDR( DRM_E_TEST_INVALIDARG );
        }
        *f_pdwSize += atol( pchEnd );
    }
ErrorExit:
    return dr;
}

#define GET_SIZE( get_iIndex, get_dw_size )                    \
do {                                                           \
    ChkArg( argc > get_iIndex );                               \
    ChkArg( argv[ get_iIndex ] != NULL );                      \
    ChkDR( _ParseSizeString( argv[get_iIndex],&get_dw_size ) );\
} while( FALSE )


/*
** _ParsePKAlgorithmType
**
** Parses f_szAlgorithm for the encryption algorithm type
** if the string doesn't match a defined algorithm type it will cast the numerical value to
** DRM_PKCRYPTO_SUPPORTED_ALGORITHMS
**
** Recognized types are: ECCP256, ECCP160
**
** Arguments:
**      [f_szAlgorithm] - Null Terminated string to parse
**      [f_peAlgorithm] - pointer to the algorithm type variable.
**
** Returns:
**      DRM_SUCCESS - on Success
**      DRM_E_TEST_INVALID_ARG  - Invalid argument
*/
static DRM_RESULT DRM_CALL _ParsePKAlgorithmType(
    __in_z char* f_szAlgorithm,
    __out_ecount( 1 ) DRM_PKCRYPTO_SUPPORTED_ALGORITHMS *f_peAlgorithm )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_szAlgorithm != NULL );
    ChkArg( f_peAlgorithm != NULL );

    if( 0 == DRMCRT_strncmp( f_szAlgorithm, "ECCP256", 7 ) )
    {
        *f_peAlgorithm = eDRM_ECC_P256;
    }
    else if( 0 == DRMCRT_strncmp( f_szAlgorithm, "ECCP160", 7 ) )
    {
        *f_peAlgorithm = eDRM_ECC_P160;
    }
    else
    {
        char *pszEnd = NULL;
        *f_peAlgorithm = ( DRM_PKCRYPTO_SUPPORTED_ALGORITHMS ) strtol( f_szAlgorithm, &pszEnd, 0 );
        ChkBOOL( pszEnd != f_szAlgorithm, DRM_E_TEST_INVALIDARG );
    }
ErrorExit:
    return dr;
}

#define GET_PK_ALGORITHM( get_iIndex, get_eAlgorithm )                   \
do {                                                                  \
    ChkArg( argv[ get_iIndex ] != NULL );                             \
    ChkDR( _ParsePKAlgorithmType( argv[get_iIndex],&get_eAlgorithm ) ); \
} while( FALSE )

/*
** _ParseShaAlgorithmType
**
** Parses f_szAlgorithm for the SHA algorithm type
** if the string doesn't match a defined algorithm type it will cast the numerical value to
** DRM_PKCRYPTO_SUPPORTED_ALGORITHMS
**
** Recognized types are: SHA1, SHA256
**
** Arguments:
**      [f_szAlgorithm] - Null Terminated string to parse
**      [f_peAlgorithm] - pointer to the algorithm type variable.
**
** Returns:
**      DRM_SUCCESS - on Success
**      DRM_E_TEST_INVALID_ARG  - Invalid argument
*/
static DRM_RESULT DRM_CALL _ParseShaAlgorithmType(
    __in_z char* f_szAlgorithm,
    __out_ecount( 1 ) eDRM_SHA_SUPPORTED_ALGORITHMS *f_peAlgorithm )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_szAlgorithm != NULL );
    ChkArg( f_peAlgorithm != NULL );

    if( 0 == DRMCRT_strncmp( f_szAlgorithm, "SHA1", 4 ) )
    {
        *f_peAlgorithm = eDRM_SHA_1;
    }
    else if( 0 == DRMCRT_strncmp( f_szAlgorithm, "SHA256", 6 ) )
    {
        *f_peAlgorithm = eDRM_SHA_256;
    }
    else
    {
        char *pszEnd = NULL;
        *f_peAlgorithm = ( eDRM_SHA_SUPPORTED_ALGORITHMS) strtol( f_szAlgorithm, &pszEnd, 0 );
        ChkBOOL( pszEnd != f_szAlgorithm, DRM_E_TEST_INVALIDARG );
    }
ErrorExit:
    return dr;
}

#define GET_SHA_ALGORITHM( get_iIndex, get_eAlgorithm )                  \
do {                                                                     \
    ChkArg( argv[ get_iIndex ] != NULL );                                \
    ChkDR( _ParseShaAlgorithmType( argv[get_iIndex],&get_eAlgorithm ) ); \
} while( FALSE )


/*
**  Test_Oem_Random_GetBytes
**  argv[0]: Index into random data array to start generating
**  argv[1]: Number of bytes to generate, calls _ParseSizeString
**  argv[2]: Optional flag: if "Misalign_Data", the returned memory is not DWORD-algined.
*/
DRM_RESULT DRM_CALL Test_Oem_Random_GetBytes( long argc, __in_ecount(argc) char **argv )
{
    DRM_RESULT dr     = DRM_SUCCESS;
    DRM_BYTE  *pbData = 0;
    DRM_DWORD  cbData = 0;

    ChkArg( argc >= 2 );
    ChkArg( argv[0] != NULL);

    GET_SIZE( 1, cbData );

    /*
    ** Allocate one more than requested, because we may have been told to misalign the data
    */
    MAP_TO_GLOBAL_TEST_VAR_Alloc( pbData, argv[0], ( cbData + 1 ) );

    if( argc > 2 && strncmp( argv[2], "Misalign_Data", 13 ) == 0 )
    {
        if( DRM_IS_DWORD_ALIGNED( pbData ) ) /*if the address is already DWORD-aligned*/
        {
            pbData++;

#ifdef ALIGNMENT_TESTING_ENABLED
            g_fPlainTextAlignmentChanged = TRUE;
#endif
        }
    }

    ChkDR( Oem_Random_GetBytes( NULL, pbData, cbData ) );

ErrorExit:
    return dr;
}

/*
**  Test_MEMCMP
**  argv[0]: First index into data array to compare
**  argv[1]: Second index into data array to compare
**  argv[2]: Number of bytes to compare, calls _ParseSizeString
*/
DRM_RESULT DRM_CALL Test_MEMCMP( long argc, __in_ecount(argc) char **argv )
{
    DRM_RESULT dr      = DRM_SUCCESS;
    DRM_BYTE  *pbLeft  = 0;
    DRM_BYTE  *pbRight = 0;
    DRM_DWORD  cbMax   = 0;

    ChkArg( argc    == 3
         && argv[0] != NULL
         && argv[1] != NULL);

    MAP_TO_GLOBAL_TEST_VAR( pbLeft, argv[0] );
    MAP_TO_GLOBAL_TEST_VAR( pbRight, argv[1] );

    GET_SIZE( 2, cbMax );

    ChkBOOL( MEMCMP( pbLeft, pbRight, cbMax) == 0,
             DRM_S_FALSE );

ErrorExit:
    return dr;
}

DRM_RESULT DRM_CALL Test_ReallocBuffer( long argc, __in_ecount(argc) char **argv )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_BYTE  *pbBuffer = NULL;
    DRM_DWORD  dwSize = 0;

    GET_SIZE( 1, dwSize);

    MAP_TO_GLOBAL_TEST_VAR_Realloc(pbBuffer, argv[0], dwSize);

ErrorExit:
    return dr;
}

/*
**  Test_PK_GenKeyPair_Encrypt_Decrypt
**  argv[0]: Operation to perform (GENKEYPAIR, ENCRYPT or DECRYPT)
**  argv[1]: Use existing crypto context or NULL
**  argv[2]: Use existing pubkey or NULL
**  argv[3]: Use existing privkey or NULL
**  argv[4]: Use valid comparison buffer or NULL, not needed when argv[0] = GENKEYPAIR
*/
DRM_RESULT DRM_CALL Test_PK_GenKeyPair_Encrypt_Decrypt( long argc, __in_ecount(argc) char **argv )
{
    DRM_RESULT dr          = DRM_SUCCESS;
    PUBKEY    *pubKey2     = NULL;
    PRIVKEY   *privKey2    = NULL;
    PUBKEY    *tmpPubKey   = NULL;
    PRIVKEY   *tmpPrivKey  = NULL;
    DRM_DWORD  cbPubKey    = DRM_ECC160_PUBLIC_KEY_LEN;
    DRM_DWORD  cbPrivKey   = DRM_ECC160_PRIVATE_KEY_LEN;
    DRM_DWORD  cbEncrypted = DRM_ECC160_CIPHERTEXT_LEN;
    DRM_DWORD  cbClear     = DRM_ECC160_PLAINTEXT_LEN;
    DRM_CHAR   tmp[__CB_DECL( DRM_ECC160_CIPHERTEXT_LEN + 1 )] = { 0 };

    ChkArg( argc > 3 );

    if( argv[0] != NULL
     && strncmp( argv[0], "GENKEYPAIR", 10 ) == 0 )
    {
        if( argv[1] == NULL )
        {
            g_pbCryptoContext = NULL;
        }
        if( argv[2] == NULL )
        {
            g_pPubKey = NULL;
        }
        if( argv[3] == NULL )
        {
            g_pPrivKey = NULL;
        }
    }

    ChkDR( DRM_PK_GenKeyPair( g_pbCryptoContext,
                              eDRM_ECC_P160,
                              eDRM_RSA_UNKNOWN,
                              &cbPubKey,
                              ( DRM_BYTE * )g_pPubKey,
                              &cbPrivKey,
                              ( DRM_BYTE * )g_pPrivKey ) );

    tmpPubKey  = g_pPubKey;
    tmpPrivKey = g_pPrivKey;

    ChkMem( pubKey2  = ( PUBKEY * )Oem_MemAlloc( cbPubKey  ) );
    ChkMem( privKey2 = ( PRIVKEY * )Oem_MemAlloc( cbPrivKey  ) );
    ChkDR( DRM_PK_GenKeyPair( g_Crypto.rgbCryptoContext,
                              eDRM_ECC_P160,
                              eDRM_RSA_UNKNOWN,
                              &cbPubKey,
                              ( DRM_BYTE * )pubKey2,
                              &cbPrivKey,
                              ( DRM_BYTE * )privKey2 ) );

    if( argv[0] != NULL
     && strncmp( argv[0], "ENCRYPT", 7 ) == 0 )
    {
        ChkArg( argc > 4 );
        if( argv[1] == NULL )
        {
            g_pbCryptoContext = NULL;
        }

        if( argv[2] != NULL
         && strncmp( argv[2], "PubKey2", 7 ) == 0 )
        {
            tmpPubKey = pubKey2;
        }
        else if( argv[2] == NULL )
        {
            tmpPubKey = NULL;
        }
        if( argv[3] == NULL )
        {
            g_pbRandBufIn = NULL;
        }
        if( argv[4] == NULL )
        {
            g_pbCipher = NULL;
        }
    }

    ChkDR( DRM_PK_Encrypt( NULL,
                           g_pbCryptoContext,
                           eDRM_ECC_P160,
                           DRM_ECC160_PUBLIC_KEY_LEN,
                           ( DRM_BYTE * )tmpPubKey,
                           DRM_ECC160_PLAINTEXT_LEN,
                           g_pbRandBufIn,
                           &cbEncrypted,
                           g_pbCipher ) );

    if( argv[0] != NULL
     && strncmp( argv[0], "DECRYPT", 7 ) == 0 )
    {
        ChkArg( argc > 4 );
        if( argv[1] == NULL )
        {
            g_pbCryptoContext = NULL;
        }

        if( argv[2] != NULL
         && strncmp( argv[2], "PrivKey2", 8 ) == 0 )
        {
            tmpPrivKey = privKey2;
        }
        else if ( argv[2] == NULL )
        {
            tmpPrivKey = NULL;
        }
        if( argv[3] == NULL )
        {
            g_pbCipher = NULL;
        }
        if( argv[4] == NULL )
        {
            g_pbDecryptBufOut = NULL;
        }
    }

    ChkDR( DRM_PK_Decrypt( g_pbCryptoContext,
                           eDRM_ECC_P160,
                           DRM_ECC160_PRIVATE_KEY_LEN,
                           ( const DRM_BYTE * )tmpPrivKey,
                           DRM_ECC160_CIPHERTEXT_LEN,
                           g_pbCipher,
                           &cbClear,
                           g_pbDecryptBufOut ) );

    ChkBOOL( MEMCMP( g_pbRandBufIn, g_pbDecryptBufOut, DRM_ECC160_PLAINTEXT_LEN ) == 0,
             DRM_E_TEST_PKCRYPTO_FAILURE );

    MEMCPY( tmp, g_pbDecryptBufOut, DRM_ECC160_PLAINTEXT_LEN );
    Log( "Trace", "Decrypted Text = %s", tmp );

ErrorExit:
    SAFE_OEM_FREE( pubKey2 );
    SAFE_OEM_FREE( privKey2 );
    return dr;
}


/*
**  Test_PK_Sign_Verify
**  argv[0]: Operation to perform (PKSIGN or PKVERIFY)
**  argv[1]: Use existing crypto context or NULL
**  argv[2]: Use existing privkey or NULL
**  argv[3]: Use existing plainkey or NULL
**  argv[4]: Plain key length or NULL
**  argv[5]: USe valid signature or NULL
*/
DRM_RESULT DRM_CALL Test_PK_Sign_Verify( long argc, __in_ecount(argc) char **argv )
{
    DRM_RESULT  dr          = DRM_SUCCESS;
    DRM_DWORD   cbKeyLength = 0;
    DRM_BYTE   *bSignature  = NULL;
    PUBKEY     *pubKey2     = NULL;
    PRIVKEY    *privKey2    = NULL;
    PUBKEY     *tmpPubKey   = NULL;
    PRIVKEY    *tmpPrivKey  = NULL;
    DRM_DWORD   cbPubKey    = DRM_ECC160_PUBLIC_KEY_LEN;
    DRM_DWORD   cbPrivKey   = DRM_ECC160_PRIVATE_KEY_LEN;
    DRM_DWORD   cbSignature = DRM_ECC160_SIGNATURE_LEN;
    DRM_BYTE    bSign[__CB_DECL( DRM_ECC160_SIGNATURE_LEN )] = { 0 };

    bSignature = bSign;
    ChkDR( DRM_PK_GenKeyPair( g_pbCryptoContext,
                              eDRM_ECC_P160,
                              eDRM_RSA_UNKNOWN,
                              &cbPubKey,
                              ( DRM_BYTE * )g_pPubKey,
                              &cbPrivKey,
                              ( DRM_BYTE * )g_pPrivKey ) );

    tmpPubKey = g_pPubKey;
    tmpPrivKey = g_pPrivKey;

    ChkMem( pubKey2 = ( PUBKEY * )Oem_MemAlloc( DRM_ECC160_PUBLIC_KEY_LEN  ) );
    ChkMem( privKey2 = ( PRIVKEY * )Oem_MemAlloc( DRM_ECC160_PRIVATE_KEY_LEN  ) );
    ChkDR( DRM_PK_GenKeyPair( g_Crypto.rgbCryptoContext,
                              eDRM_ECC_P160,
                              eDRM_RSA_UNKNOWN,
                              &cbPubKey,
                              ( DRM_BYTE * )pubKey2,
                              &cbPrivKey,
                              ( DRM_BYTE * )privKey2 ) );

    if( argc > 0
     && argv[0] != NULL
     && strncmp( argv[0], "PKSIGN", 6 ) == 0 )
    {
        ChkArg( argc > 5 );
        if( argv[1] == NULL )
        {
            g_pbCryptoContext = NULL;
        }
        if( argv[2] != NULL
         && strncmp( argv[2], "PrivKey2", 8 ) == 0 )
        {
            tmpPrivKey = privKey2;
        }
        else if ( argv[2] == NULL )
        {
            tmpPrivKey = NULL;
        }
        if( argv[3] == NULL )
        {
            g_pszKey = NULL;
        }
        if( argv[3] != NULL
         && strncmp( argv[4], "PlainKeyLength", 14 ) == 0 )
        {
            cbKeyLength = ( DRM_DWORD )strlen( (const char *)g_pszKey );
        }
        else    /*if( argv[4]&&!strcmp( argv[4], "PlainKeyZeroLength" ) )*/
        {
            cbKeyLength = 0;
        }
        if( argv[5] == NULL )
        {
            bSignature = NULL;
        }
    }
    else
    {
        cbKeyLength = ( DRM_DWORD )strlen( (const char *)g_pszKey );
    }

    ChkDR( DRM_PK_Sign( g_pbCryptoContext,
                        eDRM_ECC_P160,
                        DRM_ECC160_PRIVATE_KEY_LEN,
                        ( const DRM_BYTE * )tmpPrivKey,
                        cbKeyLength,
                        g_pszKey,
                        &cbSignature,
                        bSignature ) );

    if( argc > 0
     && argv[0] != NULL
     && strncmp( argv[0], "PKVERIFY", 8 ) == 0 )
    {
        ChkArg( argc > 5 );
        if( argv[1] == NULL )
        {
            g_pbCryptoContext = NULL;
        }
        if( argv[2] != NULL
         && strncmp( argv[2], "PubKey2", 7 ) == 0 )
        {
            tmpPubKey = pubKey2;
        }
        else if( argv[2] == NULL )
        {
            tmpPubKey = NULL;
        }
        if( argv[3] == NULL )
        {
            g_pszKey = NULL;
        }
        if( argv[3] != NULL
         && strncmp( argv[4], "PlainKeyLength", 14 ) == 0 )
        {
            cbKeyLength = ( DRM_DWORD )strlen( (const char *)g_pszKey );
        }
        else    /*if( argv[4]&&!strcmp( argv[4], "PlainKeyZeroLength" ) )*/
        {
            cbKeyLength = 0;
        }
        if( argv[5] == NULL )
        {
            bSignature = NULL;
        }
    }
    ChkDR( DRM_PK_Verify( g_pbCryptoContext,
                          eDRM_ECC_P160,
                          DRM_ECC160_PUBLIC_KEY_LEN,
                          ( const DRM_BYTE * )tmpPubKey,
                          cbKeyLength,
                          g_pszKey,
                          DRM_ECC160_SIGNATURE_LEN,
                          bSignature ) );

ErrorExit:
    SAFE_OEM_FREE( pubKey2 );
    SAFE_OEM_FREE( privKey2 );
    return dr;
}

#ifdef ALIGNMENT_TESTING_ENABLED
/*
**  Test_PK_Sign_Verify_Alignment
** Same as Test_PK_Sign_Verify, in addition to making sure that
** signature is DWORD-aligned
**  argv[0]: Operation to perform (PKSIGN or PKVERIFY)
**  argv[1]: Use existing crypto context or NULL
**  argv[2]: Use existing privkey or NULL
**  argv[3]: Use existing plainkey or NULL
**  argv[4]: Plain key length or NULL
**  argv[5]: USe valid signature
*/
DRM_RESULT DRM_CALL Test_PK_Sign_Verify_Alignment( long argc, __in_ecount(argc) char **argv )
{
    DRM_RESULT  dr          = DRM_SUCCESS;
    DRM_DWORD   cbKeyLength = 0;
    DRM_BYTE   *bSignature  = NULL;
    PUBKEY     *pubKey2     = NULL;
    PRIVKEY    *privKey2    = NULL;
    PUBKEY     *tmpPubKey   = NULL;
    PRIVKEY    *tmpPrivKey  = NULL;
    DRM_DWORD   cbPubKey    = DRM_ECC160_PUBLIC_KEY_LEN;
    DRM_DWORD   cbPrivKey   = DRM_ECC160_PRIVATE_KEY_LEN;
    DRM_DWORD   cbSignature = DRM_ECC160_SIGNATURE_LEN;
    DRM_BYTE    bSign[__CB_DECL( DRM_ECC160_SIGNATURE_LEN ) + 1] = { 0 };

    bSignature = bSign;

    if( DRM_IS_DWORD_ALIGNED( bSignature ) )
    {
        bSignature++;
    }
    ChkDR( DRM_PK_GenKeyPair( g_pbCryptoContext,
                              eDRM_ECC_P160,
                              eDRM_RSA_UNKNOWN,
                              &cbPubKey,
                              ( DRM_BYTE * )g_pPubKey,
                              &cbPrivKey,
                              ( DRM_BYTE * )g_pPrivKey ) );

    tmpPubKey = g_pPubKey;
    tmpPrivKey = g_pPrivKey;

    ChkMem( pubKey2 = ( PUBKEY * )Oem_MemAlloc( DRM_ECC160_PUBLIC_KEY_LEN  ) );
    ChkMem( privKey2 = ( PRIVKEY * )Oem_MemAlloc( DRM_ECC160_PRIVATE_KEY_LEN  ) );
    ChkDR( DRM_PK_GenKeyPair( g_Crypto.rgbCryptoContext,
                              eDRM_ECC_P160,
                              eDRM_RSA_UNKNOWN,
                              &cbPubKey,
                              ( DRM_BYTE * )pubKey2,
                              &cbPrivKey,
                              ( DRM_BYTE * )privKey2 ) );

    if( argc > 0
     && argv[0] != NULL
     && strncmp( argv[0], "PKSIGN", 6 ) == 0 )
    {
        ChkArg( argc > 5 );
        if( argv[1] == NULL )
        {
            g_pbCryptoContext = NULL;
        }
        if( argv[2] != NULL
         && strncmp( argv[2], "PrivKey2", 8 ) == 0 )
        {
            tmpPrivKey = privKey2;
        }
        else if ( argv[2] == NULL )
        {
            tmpPrivKey = NULL;
        }
        if( argv[3] == NULL )
        {
            g_pszKey = NULL;
        }
        if( argv[3] != NULL
         && strncmp( argv[4], "PlainKeyLength", 14 ) == 0 )
        {
            cbKeyLength = ( DRM_DWORD )strlen( (const char *)g_pszKey );
        }
        else    /*if( argv[4]&&!strcmp( argv[4], "PlainKeyZeroLength" ) )*/
        {
            cbKeyLength = 0;
        }
    }
    else
    {
        cbKeyLength = ( DRM_DWORD )strlen( (const char *)g_pszKey );
    }

    setacx64();
    __try
    {
        ChkDR( DRM_PK_Sign( g_pbCryptoContext,
                            eDRM_ECC_P160,
                            DRM_ECC160_PRIVATE_KEY_LEN,
                            ( const DRM_BYTE * )tmpPrivKey,
                            cbKeyLength,
                            g_pszKey,
                            &cbSignature,
                            bSignature ) );
    }
     __except( _AlignmentExceptionHandler( GetExceptionCode() ) )
    {
        ChkDR( STATUS_DATATYPE_MISALIGNMENT );
    }

    if( argc > 0
     && argv[0] != NULL
     && strncmp( argv[0], "PKVERIFY", 8 ) == 0 )
    {
        ChkArg( argc > 5 );
        if( argv[1] == NULL )
        {
            g_pbCryptoContext = NULL;
        }
        if( argv[2] != NULL
         && strncmp( argv[2], "PubKey2", 7 ) == 0 )
        {
            tmpPubKey = pubKey2;
        }
        else if( argv[2] == NULL )
        {
            tmpPubKey = NULL;
        }
        if( argv[3] == NULL )
        {
            g_pszKey = NULL;
        }
        if( argv[3] != NULL
         && strncmp( argv[4], "PlainKeyLength", 14 ) == 0 )
        {
            cbKeyLength = ( DRM_DWORD )strlen( (const char *)g_pszKey );
        }
        else    /*if( argv[4]&&!strcmp( argv[4], "PlainKeyZeroLength" ) )*/
        {
            cbKeyLength = 0;
        }
    }
    __try
    {
        ChkDR( DRM_PK_Verify( g_pbCryptoContext,
                              eDRM_ECC_P160,
                              DRM_ECC160_PUBLIC_KEY_LEN,
                              ( const DRM_BYTE * )tmpPubKey,
                              cbKeyLength,
                              g_pszKey,
                              DRM_ECC160_SIGNATURE_LEN,
                              bSignature ) );
    }
     __except( _AlignmentExceptionHandler( GetExceptionCode() ) )
    {
        ChkDR( STATUS_DATATYPE_MISALIGNMENT );
    }

ErrorExit:
    clearacx64();
    SAFE_OEM_FREE( pubKey2 );
    SAFE_OEM_FREE( privKey2 );
    return dr;
}
#endif /* ALIGNMENT_TESTING_ENABLED */

/*
** Test_DRM_PK_GenKeyPair
** Tests private-public key pair generation
** argv[0] - Algorithm ( ECCP256, ECCP160, a number )
** argv[1] - Crypto Context, NULL context if NULL for argument.
** argv[2] - Output public key. Allocates mem to this.
** argv[3] - Size of public key buffer. parsed by _ParseKeySizeString
** argv[4] - Output private key. Allocates mem to this.
** argv[5] - Size of private key buffer. parsed by _ParseKeySizeString
*/
DRM_RESULT DRM_CALL Test_DRM_PK_GenKeyPair( long argc, __in_ecount(argc) char **argv )
{
    /*
    ** Decls
    */
    DRM_RESULT  dr        = DRM_SUCCESS;
    DRM_BYTE   *pCrypto   = NULL;
    DRM_BYTE   *pPrivkey  = NULL;
    DRM_BYTE   *pPubkey   = NULL;
    DRM_DWORD   cbPubKey  = 0;
    DRM_DWORD   cbPrivKey = 0;
    DRM_PKCRYPTO_SUPPORTED_ALGORITHMS eAlgorithm = ( DRM_PKCRYPTO_SUPPORTED_ALGORITHMS ) 0;

    /*
    ** Arg Checks
    */
    ChkArg( argc      == 6 );
    GET_PK_ALGORITHM( 0, eAlgorithm );

    GET_SIZE( 3, cbPubKey);
    GET_SIZE( 5, cbPrivKey);

    MAP_TO_GLOBAL_TEST_VAR_Alloc( pPubkey, argv[2], cbPubKey );
    MAP_TO_GLOBAL_TEST_VAR_Alloc( pPrivkey, argv[4], cbPrivKey );


    if( argv[ 1 ] != NULL )
    {
        pCrypto = g_Crypto.rgbCryptoContext;
    }
    /*
    ** Generate the key pair
    */
    ChkDR( DRM_PK_GenKeyPair( pCrypto,
                              eAlgorithm,
                              eDRM_RSA_UNKNOWN,
                              &cbPubKey,
                              ( DRM_BYTE * )pPubkey,
                              &cbPrivKey,
                              ( DRM_BYTE * )pPrivkey ) );

ErrorExit:
    return dr;
} /* end Test_DRM_PK_GenKeyPair */

/*
** Test_DRM_PK_Encrypt
** Tests Public Key Encryption
** argv[0] - Algorithm ( ECCP256, ECCP160 )
** argv[1] - Crypto Context, NULL context if NULL for argument.
** argv[2] - public key for encryption.
** argv[3] - Size of public key buffer. Parsed by _ParseSizeString
** argv[4] - Plaintext block to be encrypted
** argv[5] - Size of Plaintext block to be encrypted.  Parsed by _ParseSizeString
** argv[6] - Output ciphertext. Allocates mem to this.
** argv[7] - Size of ciphertext.  Parsed by _ParseSizeString
*/
DRM_RESULT DRM_CALL Test_DRM_PK_Encrypt( long argc, __in_ecount(argc) char **argv )
{
    /*
    ** Decls
    */
    DRM_RESULT  dr           = DRM_SUCCESS;
    DRM_BYTE   *pCrypto      = NULL;
    DRM_BYTE   *pPubkey      = NULL;
    DRM_BYTE   *pPlainText   = NULL;
    DRM_BYTE   *pCipherText  = NULL;
    DRM_DWORD   cbPubKey     = 0;
    DRM_DWORD   cbPlainText  = 0;
    DRM_DWORD   cbCipherText = 0;
    DRM_PKCRYPTO_SUPPORTED_ALGORITHMS eAlgorithm = ( DRM_PKCRYPTO_SUPPORTED_ALGORITHMS ) 0;

    /*
    ** Arg Checks
    */
    ChkArg( argc      == 8 );

    GET_PK_ALGORITHM( 0, eAlgorithm );

    GET_SIZE( 3, cbPubKey);
    GET_SIZE( 5, cbPlainText);
    GET_SIZE( 7, cbCipherText);

    MAP_TO_GLOBAL_TEST_VAR( pPubkey, argv[2] );
    MAP_TO_GLOBAL_TEST_VAR( pPlainText, argv[4] );
    MAP_TO_GLOBAL_TEST_VAR_Alloc( pCipherText, argv[6], cbCipherText );

    if( argv[ 1 ] != NULL )
    {
        pCrypto = g_Crypto.rgbCryptoContext;
    }
    /*
    ** Do the encryption
    */
    ChkDR( DRM_PK_Encrypt( NULL,
                           pCrypto,
                           eAlgorithm,
                           cbPubKey,
                           pPubkey,
                           cbPlainText,
                           pPlainText,
                           &cbCipherText,
                           pCipherText ) );

ErrorExit:
    return dr;
} /* end Test_DRM_PK_Encrypt */

#ifdef ALIGNMENT_TESTING_ENABLED
/*
** Test_DRM_PK_Encrypt_P160_Alignment
** Same as Test_DRM_PK_Encrypt_P160 except it passes unaligned pointers
** to the encryption function
** Tests Public Key Encryption
** argv[0] - Algorithm ( ECCP256, ECCP160 )
** argv[1] - Crypto Context, NULL context if NULL for argument.
** argv[2] - public key for encryption.
** argv[3] - Size of public key buffer. Parsed by _ParseSizeString
** argv[4] - Plaintext block to be encrypted
** argv[5] - Size of Plaintext block to be encrypted.  Parsed by _ParseSizeString
** argv[6] - Output ciphertext. Allocates mem to this.
** argv[7] - Size of ciphertext.  Parsed by _ParseSizeString
*/
DRM_RESULT DRM_CALL Test_DRM_PK_Encrypt_P160_Alignment( long argc, __in_ecount(argc) char **argv )
{
    /*
    ** Decls
    */
    DRM_RESULT  dr           = DRM_SUCCESS;
    DRM_BYTE   *pCrypto      = NULL;
    DRM_BYTE   *pPubkey      = NULL;
    DRM_BYTE   *pPlainText   = NULL;
    DRM_BYTE   *pCipherText  = NULL;
    DRM_BYTE   *pAlignCipherText   = NULL;
    DRM_DWORD   cbPubKey     = 0;
    DRM_DWORD   cbPlainText  = 0;
    DRM_DWORD   cbCipherText = 0;
    DRM_PKCRYPTO_SUPPORTED_ALGORITHMS eAlgorithm = ( DRM_PKCRYPTO_SUPPORTED_ALGORITHMS ) 0;

    ChkArg( argc == 8 );

    GET_PK_ALGORITHM( 0, eAlgorithm );

    GET_SIZE( 3, cbPubKey);
    GET_SIZE( 5, cbPlainText);
    GET_SIZE( 7, cbCipherText);

    MAP_TO_GLOBAL_TEST_VAR( pPubkey, argv[2] );
    MAP_TO_GLOBAL_TEST_VAR( pPlainText, argv[4] );
    MAP_TO_GLOBAL_TEST_VAR_Alloc( pCipherText, argv[6], cbCipherText+1 ); //allocate an extra byte so that we can shift, if necessary

    if( argv[ 1 ] != NULL )
    {
        pCrypto = g_Crypto.rgbCryptoContext;
    }

    ChkBOOL( cbCipherText >= DRM_ECC160_CIPHERTEXT_LEN, DRM_E_BUFFERTOOSMALL );
    ChkBOOL( cbPlainText   >= DRM_ECC160_PLAINTEXT_LEN, DRM_E_BUFFERTOOSMALL );

    pAlignCipherText = pCipherText;

    setacx64();

    __try
    {
        if( DRM_IS_DWORD_ALIGNED( pAlignCipherText ) ) /*if the address is already DWORD-aligned*/
        {
            pAlignCipherText++; /*no longer DWORD-aligned*/
            g_fCipherTextAlignmentChanged = TRUE;
        }

        if( g_fPlainTextAlignmentChanged == TRUE )
        {
            pPlainText++;
        }

        ChkDR( OEM_ECC_Encrypt_P160( pCrypto,
                                     ( const PUBKEY * ) pPubkey,
                                     pPlainText,
                                     pAlignCipherText ) );
    }
    __except( _AlignmentExceptionHandler( GetExceptionCode() ) )
    {
        ChkDR( STATUS_DATATYPE_MISALIGNMENT );
    }
ErrorExit:
    clearacx64();

    return dr;
}

/*
** Test_DRM_PK_Decrypt_P160_Alignment
** Tests Public Key Decryption
** argv[0] - Algorithm ( ECCP256, ECCP160, a number )
** argv[1] - Crypto Context, NULL context if NULL for argument.
** argv[2] - Private key for decryption.
** argv[3] - Size of private key buffer. Parsed by _ParseSizeString
** argv[4] - Ciphertext block to be decrypted
** argv[5] - Size of ciphertext block to be decrypted.  Parsed by _ParseSizeString
** argv[6] - Output plaintexr. Allocates mem to this.
** argv[7] - Size of plaintext.  Parsed by _ParseSizeString
*/
DRM_RESULT DRM_CALL Test_DRM_PK_Decrypt_P160_Alignment( long argc, __in_ecount(argc) char **argv )
{
    /*
    ** Decls
    */
    DRM_RESULT  dr           = DRM_SUCCESS;
    DRM_BYTE   *pCrypto      = NULL;
    DRM_BYTE   *pPrivkey     = NULL;
    DRM_BYTE   *pPlainText   = NULL;
    DRM_BYTE   *pCipherText  = NULL;
    DRM_BYTE   *pAlignPlainText   = NULL;
    DRM_DWORD   cbPrivKey    = 0;
    DRM_DWORD   cbPlainText  = 0;
    DRM_DWORD   cbCipherText = 0;
    DRM_PKCRYPTO_SUPPORTED_ALGORITHMS eAlgorithm = ( DRM_PKCRYPTO_SUPPORTED_ALGORITHMS ) 0;

    ChkArg( argc == 8 );
    GET_PK_ALGORITHM( 0, eAlgorithm );

    GET_SIZE( 3, cbPrivKey);
    GET_SIZE( 5, cbCipherText);
    GET_SIZE( 7, cbPlainText);

    MAP_TO_GLOBAL_TEST_VAR( pPrivkey, argv[2] );
    MAP_TO_GLOBAL_TEST_VAR( pCipherText, argv[4] );
    MAP_TO_GLOBAL_TEST_VAR_Alloc( pPlainText, argv[6], cbPlainText+1 );


    if( argv[ 1 ] != NULL )
    {
        pCrypto = g_Crypto.rgbCryptoContext;
    }

    setacx64();

    __try
    {
        pAlignPlainText = pPlainText;

        if( DRM_IS_DWORD_ALIGNED( pAlignPlainText ) ) /*if the address is already DWORD-aligned*/
        {
            pAlignPlainText++; /*no longer DWORD-aligned*/
        }
        if( g_fCipherTextAlignmentChanged == TRUE )
        {
            pCipherText++;
        }

        ChkDR( OEM_ECC_Decrypt_P160( pCrypto,
                                     ( const PRIVKEY * ) pPrivkey,
                                     pCipherText,
                                     pAlignPlainText ) );
    }
    __except( _AlignmentExceptionHandler( GetExceptionCode() ) )
    {
        ChkDR( STATUS_DATATYPE_MISALIGNMENT );
    }

ErrorExit:
    clearacx64();
    return dr;
}
#endif /* ALIGNMENT_TESTING_ENABLED */

/*
** Test_DRM_PK_Decrypt
** Tests Public Key Decryption
** argv[0] - Algorithm ( ECCP256, ECCP160, a number )
** argv[1] - Crypto Context, NULL context if NULL for argument.
** argv[2] - Private key for decryption.
** argv[3] - Size of private key buffer. Parsed by _ParseSizeString
** argv[4] - Ciphertext block to be decrypted
** argv[5] - Size of ciphertext block to be decrypted.  Parsed by _ParseSizeString
** argv[6] - Output plaintexr. Allocates mem to this.
** argv[7] - Size of plaintext.  Parsed by _ParseSizeString
*/
DRM_RESULT DRM_CALL Test_DRM_PK_Decrypt( long argc, __in_ecount(argc) char **argv )
{
    /*
    ** Decls
    */
    DRM_RESULT  dr           = DRM_SUCCESS;
    DRM_BYTE   *pCrypto      = NULL;
    DRM_BYTE   *pPrivkey     = NULL;
    DRM_BYTE   *pPlainText   = NULL;
    DRM_BYTE   *pCipherText  = NULL;
    DRM_DWORD   cbPrivKey    = 0;
    DRM_DWORD   cbPlainText  = 0;
    DRM_DWORD   cbCipherText = 0;
    DRM_PKCRYPTO_SUPPORTED_ALGORITHMS eAlgorithm = ( DRM_PKCRYPTO_SUPPORTED_ALGORITHMS ) 0;

    /*
    ** Arg Checks
    */
    ChkArg( argc      == 8 );
    GET_PK_ALGORITHM( 0, eAlgorithm );

    GET_SIZE( 3, cbPrivKey);
    GET_SIZE( 5, cbCipherText);
    GET_SIZE( 7, cbPlainText);

    MAP_TO_GLOBAL_TEST_VAR( pPrivkey, argv[2] );
    MAP_TO_GLOBAL_TEST_VAR( pCipherText, argv[4] );
    MAP_TO_GLOBAL_TEST_VAR_Alloc( pPlainText, argv[6], cbPlainText );


    if( argv[ 1 ] != NULL )
    {
        pCrypto = g_Crypto.rgbCryptoContext;
    }
    /*
    ** Do the encryption
    */
    ChkDR( DRM_PK_Decrypt( pCrypto,
                           eAlgorithm,
                           cbPrivKey,
                           pPrivkey,
                           cbCipherText,
                           pCipherText,
                           &cbPlainText,
                           pPlainText ) );
ErrorExit:
    return dr;
} /* end Test_DRM_PK_Encrypt */

/*
** Test_DRM_PK_Sign
** Tests basic Public Key signing
** argv[0] - Algorithm ( ECCP256, ECCP160, a number )
** argv[1] - Crypto Context, NULL context if NULL for argument.
** argv[2] - Message to be signed
** argv[3] - Message len in bytes  Parsed by _ParseSizeString
** argv[4] - Private key for signing.
** argv[5] - Size of private key buffer. Parsed by _ParseSizeString
** argv[6] - The output of the signing operation - this is the signature. Allocates mem to this.
** argv[7] - Size of the output buffer.  Parsed by _ParseSizeString
*/
DRM_RESULT DRM_CALL Test_DRM_PK_Sign( long argc, __in_ecount(argc) char **argv )
{
    /*
    ** Decls
    */
    DRM_RESULT      dr        = DRM_SUCCESS;
    DRM_BYTE       *pCrypto   = NULL;
    DRM_DWORD       cbMessage = 0;
    DRM_DWORD       cbSig     = 0;
    DRM_DWORD       cbPrivKey = 0;
    DRM_BYTE       *pPrivkey  = NULL;
    DRM_BYTE       *pbMessage = NULL;
    DRM_BYTE       *pSig      = NULL;
    DRM_PKCRYPTO_SUPPORTED_ALGORITHMS eAlgorithm = ( DRM_PKCRYPTO_SUPPORTED_ALGORITHMS ) 0;


    /*
    ** Chk Args
    */
    ChkArg( argc == 8 );

    GET_PK_ALGORITHM( 0, eAlgorithm );

    GET_SIZE( 3, cbMessage );
    GET_SIZE( 5, cbPrivKey );
    GET_SIZE( 7, cbSig );


    MAP_TO_GLOBAL_TEST_VAR( pPrivkey, argv[4] );
    MAP_TO_GLOBAL_TEST_VAR( pbMessage, argv[2] );
    MAP_TO_GLOBAL_TEST_VAR_Alloc( pSig, argv[6], cbSig );


    if( argv[ 1 ] != NULL )
    {
        pCrypto = g_Crypto.rgbCryptoContext;
    }
    /*
    ** Do the signing
    */
    ChkDR( DRM_PK_Sign( pCrypto,
                        eAlgorithm,
                        cbPrivKey,
                        pPrivkey,
                        cbMessage,
                        pbMessage,
                        &cbSig,
                        pSig ) );

ErrorExit:
    return dr;
} /* end Test_DRM_PK_Sign */

/*
** Test_DRM_PK_Verify
** Tests basic Public Key verification
** argv[0] - Algorithm ( ECCP256, ECCP160, a number )
** argv[1] - Crypto Context, NULL context if NULL for argument.
** argv[2] - Message to be verified
** argv[3] - Message len in bytes  Parsed by _ParseSizeString
** argv[4] - Public key for verify.
** argv[5] - Size of public key buffer. Parsed by _ParseSizeString
** argv[6] - Signature index
** argv[7] - Size of the output buffer.  Parsed by _ParseSizeString
*/
DRM_RESULT DRM_CALL Test_DRM_PK_Verify( long argc, __in_ecount(argc) char **argv )
{
    /*
    ** Decls
    */
    DRM_RESULT      dr        = DRM_SUCCESS;
    DRM_BYTE       *pCrypto   = NULL;
    DRM_DWORD       cbMessage = 0;
    DRM_DWORD       cbSig     = 0;
    DRM_DWORD       cbPubKey  = 0;
    DRM_BYTE       *pPubkey   = NULL;
    DRM_BYTE       *pbMessage = NULL;
    DRM_BYTE       *pSig      = NULL;
    DRM_PKCRYPTO_SUPPORTED_ALGORITHMS eAlgorithm = ( DRM_PKCRYPTO_SUPPORTED_ALGORITHMS ) 0;


    /*
    ** Chk Args
    */
    ChkArg( argc == 8 );

    GET_PK_ALGORITHM( 0, eAlgorithm );

    GET_SIZE( 3, cbMessage );
    GET_SIZE( 5, cbPubKey );
    GET_SIZE( 7, cbSig );


    MAP_TO_GLOBAL_TEST_VAR( pPubkey, argv[4] );
    MAP_TO_GLOBAL_TEST_VAR( pbMessage, argv[2] );
    MAP_TO_GLOBAL_TEST_VAR( pSig, argv[6] );


    if( argv[ 1 ] != NULL )
    {
        pCrypto = g_Crypto.rgbCryptoContext;
    }
    /*
    ** Do the signing
    */
    ChkDR( DRM_PK_Verify( pCrypto,
                          eAlgorithm,
                          cbPubKey,
                          pPubkey,
                          cbMessage,
                          pbMessage,
                          cbSig,
                          pSig ) );

ErrorExit:
    return dr;
} /* end Test_DRM_PK_Verify */

/*
**  Test_DRM_CPHR_Init
**  argv[0]: Length in bytes of the content key, can be zero.
**  argv[1]: Status of the content key (RANDOM, CLEAR, NULL, or KNOWN_RC4)
**  argv[2]: Pointer to a DRM_CIPHER_CONTEXT structure
**  argv[3]: Type of cipher to initialize ( RC4, AES_COUNTER )
*/
DRM_RESULT DRM_CALL Test_DRM_CPHR_Init( long argc, __in_ecount(argc) char **argv )
{
    DRM_RESULT dr           = DRM_SUCCESS;
    DRM_DWORD  cbContentKey = 0;
    DRM_BYTE  *pbContKey    = NULL;
    DRM_SUPPORTED_CIPHERS eCipher = eDRM_RC4_CIPHER;

    ChkArg( argc    == 4 );
    ChkArg( argv[0] != NULL );
    ChkArg( argv[3] != NULL );

    /*
    ** set cipher type
    */
    if( strncmp( argv[3], "RC4", 3 ) != 0 )
    {
        ChkArg( strncmp( argv[3], "AES_COUNTER", 11 ) == 0 );
        eCipher = eDRM_AES_COUNTER_CIPHER;
    }

    /*
    ** set content key length
    */
    cbContentKey = atoi( argv[0] );

    /*
    ** set up content key parameter
    */
    if ( argv[1] != NULL )
    {
        ChkMem( pbContKey = ( DRM_BYTE * )Oem_MemAlloc( max( 1, cbContentKey ) ) );
        MEMSET( pbContKey, 0, cbContentKey );

        if ( strncmp( argv[1], "RANDOM", 6 ) == 0 )
        {
            ChkDR( Oem_Random_GetBytes( NULL, pbContKey, cbContentKey ) );
        }
        else if ( strncmp( argv[1], "KNOWN_RC4", 9 ) == 0 )
        {
            MEMCPY( pbContKey, g_rgbRC4KnownContentKey, DRMCIPHERKEYLEN_RC4 );
        }
        else
        {
            ChkArg( strncmp( argv[1], "CLEAR", 4 ) == 0 );
        }
    }

    /*
    ** set up cipher context
    */
    g_pContext = argv[2] != NULL ? &g_Context : NULL;

    ChkDR( DRM_CPHR_Init( g_pContext, eCipher, cbContentKey, pbContKey ) );

ErrorExit:
    SAFE_OEM_FREE( pbContKey );
    return dr;
}

/*
**  Test_DRM_CPHR_Encrypt
** argv[0]: status of crypto context: NULL or NORMAL
** argv[1]: Number of bytes to be encrypted by the cipher
** argv[2]: Status of pbData: NULL, RANDOM, KNOWN_FILE or KNOWN_RC4
** argv[3]: Data ID to encrypt (optional, DWORD or "KNOWN_FILE" only)
** argv[4]: Block offset to encrypt (optional, DWORD only, required if argv[3] is present and not "KNOWN_FILE")
*/
DRM_RESULT DRM_CALL Test_DRM_CPHR_Encrypt( long argc, __in_ecount(argc) char **argv )
{
    DRM_RESULT dr         = DRM_SUCCESS;
    DRM_DWORD  cbData     = 0;
    DRM_BYTE  *pbData     = NULL;
    DRM_BOOL   fKnownRC4  = FALSE;
    DRM_BOOL   fKnownFile = FALSE;
    DRM_AES_COUNTER_MODE_CONTEXT *pAesCtrContext = NULL;

    ChkArg( argc     >= 3 );
    ChkArg( argv[1] !=  NULL );

    /*
    ** set content key length
    */
    cbData = atol( argv[1] );

    /*
    ** set up content to be encrypted
    */
    if ( argv[2] != NULL )
    {
        ChkMem( pbData = ( DRM_BYTE * )Oem_MemAlloc( cbData ) );

        if( strncmp( argv[2], "RANDOM", 6 ) == 0 )
        {
            ChkDR( Oem_Random_GetBytes( NULL, pbData, cbData ) );
        }
        else if( strncmp( argv[2], "KNOWN_FILE", 10 ) == 0 )
        {
            fKnownFile = TRUE;
            MEMCPY( pbData, g_pbSymmetricClearData, cbData );
        }
        else
        {
            ChkArg( strncmp( argv[2], "KNOWN_RC4", 9 ) == 0 );
            fKnownRC4 = TRUE;
            MEMCPY( pbData, g_pszRC4KnownClearText, cbData );
        }
        MEMCPY( g_pbSymmetricClearData, pbData, cbData );
    }


    /*
    ** Set up AES counter/offset data
    */
    if ( argc    >= 4
      && argv[3] != NULL)
    {
        if( strncmp( argv[3], "KNOWN_FILE", 10 ) == 0 )
        {
            g_AesCtrContext.qwInitializationVector = g_qwSymmetricDataID;
            g_AesCtrContext.qwBlockOffset = DRM_UI64( 0 );
            g_AesCtrContext.bByteOffset   = 0x00;
        }
        else if ( argc == 5 && argv[4] != NULL )
        {
            g_AesCtrContext.qwInitializationVector = DRM_UI64( strtoul( argv[3], NULL, 0 ) );
            g_AesCtrContext.qwBlockOffset = DRM_UI64( strtoul( argv[4], NULL, 0 ) / DRM_AES_BLOCKLEN );
            g_AesCtrContext.bByteOffset = ( DRM_BYTE )( strtoul( argv[4], NULL, 0 ) % DRM_AES_BLOCKLEN );
        }
    }

    if ( g_Context.eCipherType == eDRM_AES_COUNTER_CIPHER )
    {
        pAesCtrContext = &g_AesCtrContext;
    }

    ChkDR( DRM_CPHR_Encrypt( argv[0] == NULL? NULL : &g_Context, pAesCtrContext, cbData, pbData ) );

    if ( pbData != NULL )
    {
        if ( fKnownRC4 )
        {
            ChkBOOL( MEMCMP( pbData, g_rgbRC4KnownCipherText, cbData ) == 0, DRM_E_TEST_ENCRYPT_ERROR )
        }
        else if ( fKnownFile )
        {
            ChkBOOL( MEMCMP( pbData, g_pbSymmetricEncryptedData, cbData ) == 0, DRM_E_TEST_ENCRYPT_ERROR )
        }
        else
        {
            ChkBOOL( MEMCMP( pbData, g_pbSymmetricClearData, cbData ) != 0, DRM_E_TEST_ENCRYPT_ERROR );
        }
        MEMCPY( g_pbSymmetricEncryptedData, pbData, cbData );
    }

ErrorExit:
    SAFE_OEM_FREE( pbData );
    return dr;
}


/*
** Test_DRM_CPHR_InitDecrypt
**
** Arguments:
**              argv[0] -- the total length of the encrypted data
**              argv[1] -- byte count offset to the last 15 bytes of the total encrypted data. can be NULL, can not be empty.
**              argv[2] -- cipher context parameter, NULL or NORMAL
*/
DRM_RESULT DRM_CALL Test_DRM_CPHR_InitDecrypt( long argc, __in_ecount(argc) char **argv )
{
    DRM_RESULT dr            = DRM_SUCCESS;
    DRM_DWORD  cbData        = 0;
    DRM_DWORD  ibLast15      = 0;
    DRM_BYTE   rgbLast15[15] = {0};

    ChkArg( argc >= 3 );
    ChkArg( argv[0] != NULL );

    if ( argv[0] != NULL )
    {
        cbData = atoi( argv[0] );
    }

    if ( argv[1] != NULL )
    {
        DRM_DWORD dwCopyCount = 15;
        if ( cbData < 15 && cbData != 0 )
        {
            dwCopyCount = cbData;
        }
        if ( strstr( argv[1], "NORMAL" ) != NULL )
        {
            if ( cbData > 15 )
            {
                ibLast15 = cbData - 15;
            }
        }
        else
        {
            ibLast15 = atoi( argv[1] );
        }
        DRM_BYT_CopyBytes( rgbLast15, 0, g_pbSymmetricEncryptedData, ibLast15, dwCopyCount );
    }

    ChkDR( DRM_CPHR_InitDecrypt( ( argv[2] == NULL ) ? NULL : &g_Context,
                                 ( argv[1] == NULL ) ? NULL : rgbLast15,
                                  cbData) );
ErrorExit:
    return dr;
}

/*
** Test_DRM_CPHR_Decrypt
** argv[0]: Number of bytes to be decrypted by the cipher
** argv[1]: DRM_CIPHER_CONTEXT: NULL, NORMAL
** argv[2]: opt, byte offset into the total encrypted data tp mark the start location to decrypt. assumed 0 if missing.
** argv[3]: Data ID to decrypt (optional, DWORD or "KNOWN_FILE" only)
** argv[4]: Block offset to decrypt (optional, DWORD only, required if argv[4] is present and not "KNOWN_FILE")
*/
DRM_RESULT DRM_CALL Test_DRM_CPHR_Decrypt( long argc, __in_ecount(argc) char **argv )
{
    DRM_RESULT dr           = DRM_SUCCESS;
    DRM_DWORD  ibOffset     = 0;
    DRM_DWORD  cbDecryptLen = 0;
    DRM_BYTE  *pbData       = NULL;
    DRM_AES_COUNTER_MODE_CONTEXT *pAesCtrContext = NULL;

    ChkArg( argc >= 2 );

    /*
    ** get the data offset
    */
    if ( argc    >  2
      && argv[2] != NULL )
    {
        ibOffset = strtoul( argv[2], NULL, 0 );
    }

    /*
    ** Get the encrypted data
    */
    if ( argv[0] != NULL )
    {
        cbDecryptLen = strtoul( argv[0], NULL, 0 );
        ChkMem( pbData = ( DRM_BYTE * )Oem_MemAlloc( cbDecryptLen ) );
        DRM_BYT_CopyBytes( pbData, 0, g_pbSymmetricEncryptedData, ibOffset, cbDecryptLen );
    }

    /*
    ** Set up AES counter/offset data
    */
    if ( argc    >= 4
      && argv[3] != NULL)
    {
        if( strncmp( argv[3], "KNOWN_FILE", 10 ) == 0 )
        {
            g_AesCtrContext.qwInitializationVector = g_qwSymmetricDataID;
        }
        else
        {
            g_AesCtrContext.qwInitializationVector = DRM_UI64( strtoul( argv[3], NULL, 0 ) );
        }
        if ( argc == 5 && argv[4] != NULL )
        {
            g_AesCtrContext.qwBlockOffset = DRM_UI64( strtoul( argv[4], NULL, 0 ) / DRM_AES_BLOCKLEN );
            g_AesCtrContext.bByteOffset = ( DRM_BYTE )( strtoul( argv[4], NULL, 0 ) % DRM_AES_BLOCKLEN );
        }
        else
        {
            g_AesCtrContext.qwBlockOffset = DRM_UI64( 0 );
            g_AesCtrContext.bByteOffset = 0x00;
        }
        pAesCtrContext = &g_AesCtrContext;
    }

    ChkDR( DRM_CPHR_Decrypt( ( argv[1] != NULL ) ? &g_Context : NULL,
                             pAesCtrContext,
                             cbDecryptLen,
                             pbData ) );

    /*
    ** compare the decrypted data with the clear content before encrypted
    */
    ChkBOOL( DRM_BYT_CompareBytes( g_pbSymmetricClearData, ibOffset, pbData, 0, cbDecryptLen - 1 ) == 0,
             DRM_E_TEST_DECRYPT_ERROR );

ErrorExit:
    SAFE_OEM_FREE( pbData );
    return dr;
}

/*
**  Test_DRM_SHA_Init - will initialize internal SHA data.  This must be called each time before a new
**  SHA hash is computed.
**  argv[0]: index to the SHA Context in the test data array
**  argv[1]: Specifies algorithm to use, SHA1 or SHA256
*/
DRM_RESULT DRM_CALL Test_DRM_SHA_Init( long argc, __in_ecount(argc) char **argv )
{
    DRM_RESULT dr = DRM_SUCCESS;
    eDRM_SHA_SUPPORTED_ALGORITHMS eAlgorithm = ( eDRM_SHA_SUPPORTED_ALGORITHMS ) 2; /* default to a bad value */
    DRM_SHA_CONTEXT *pShaContext = NULL;

    ChkArg( argc == 2 );

    MAP_TO_GLOBAL_TEST_VAR_Alloc( pShaContext, argv[0], SIZEOF( DRM_SHA_CONTEXT ) );
    GET_SHA_ALGORITHM( 1, eAlgorithm );

    ChkDR( DRM_SHA_Init( pShaContext, eAlgorithm ) );

ErrorExit:
    return dr;
}

/*
** Test_DRM_SHA_Update- updates the running hash context with new data to be hashed.
** argv[0]:index to the context pointer.
** argv[1]:index to the internal test buffer.
** argv[2]:test buffer size
*/
DRM_RESULT DRM_CALL Test_DRM_SHA_Update( long argc, __in_ecount(argc) char **argv )
{
    DRM_RESULT dr      = DRM_SUCCESS;
    DRM_BYTE  *pbBuffer = NULL;
    DRM_DWORD  cbBuffer = 0;
    DRM_SHA_CONTEXT *pShaContext = NULL;

    ChkArg( argc == 3 );

    MAP_TO_GLOBAL_TEST_VAR( pShaContext, argv[0] );
    MAP_TO_GLOBAL_TEST_VAR( pbBuffer, argv[1] );
    GET_SIZE( 2, cbBuffer );

    ChkDR( DRM_SHA_Update( pbBuffer, cbBuffer, pShaContext) );

 ErrorExit:
    return dr;
}

/*
** Test_DRM_SHA_Finalize - signals that all data has been added to the hash
** argv[0]: context index in the global data array
** argv[1]: index to the internal buffer to store the hash result, use NULL pointer if NULL
** argv[2]: Digest Size, Parsed through GET_SIZE.
*/
DRM_RESULT DRM_CALL Test_DRM_SHA_Finalize( long argc, __in_ecount(argc) char **argv )
{
    DRM_RESULT dr     = DRM_SUCCESS;
    DRM_DWORD  cbDigest = 0;
    DRM_BYTE  *pbDigest;
    DRM_SHA_CONTEXT *pShaContext = NULL;

    ChkArg( argc == 3 );

    MAP_TO_GLOBAL_TEST_VAR( pShaContext, argv[0] );
    MAP_TO_GLOBAL_TEST_VAR_Alloc( pbDigest, argv[1], DRM_SHA_DIGEST_LEN_MAX );
    GET_SIZE( 2, cbDigest );

    ChkDR( DRM_SHA_Finalize( pShaContext,
                             cbDigest,
                             pbDigest) );

ErrorExit:
    return dr;
}






/*
** Test_DRM_SHA256_Init
** Tests basic SHA256 sturcture initialization
** argv[0] - index to global pointer to init the struct to. Allocates mem to this.
*/
DRM_RESULT DRM_CALL Test_DRM_SHA256_Init( long argc, __in_ecount(argc) char **argv )
{
    /*
    ** Decls
    */
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_SHA256_Context *pSha256Context = NULL;

    /*
    ** Arg Checks
    */
    ChkArg( 1   == argc );

    MAP_TO_GLOBAL_TEST_VAR_Alloc( pSha256Context,
                                  argv[0],
                                  SIZEOF( DRM_SHA256_Context ) );

    /*
    ** Make sure we can init ok
    */
    ChkDR(DRM_SHA256_Init( pSha256Context ) );

ErrorExit:
    return dr;
} /* end proc Test_DRM_SHA256_Init */

/*
** Test_DRM_SHA256_Update
** Tests basic SHA256 functionality for adding byte to be hashed
** argv[0] - index to global pointer to initilized sha256 sturct
** argv[1] - index to global pointer to the message to be sha'ed
** argv[2] - Len of the message to sign
*/
DRM_RESULT DRM_CALL Test_DRM_SHA256_Update( long argc, __in_ecount(argc) char **argv )
{
    /*
    ** Decls
    */
    DRM_RESULT dr      = DRM_SUCCESS;
    DRM_SHA256_Context *pSha256Context = NULL;
    DRM_BYTE           *pBuffer        = NULL;

    /*
    ** Arg Checks
    */
    ChkArg( 3    == argc );
    ChkArg( NULL != argv[ 2 ] );

    MAP_TO_GLOBAL_TEST_VAR( pSha256Context, argv[0] );
    MAP_TO_GLOBAL_TEST_VAR( pBuffer, argv[1] );

    /*
    ** Make sure that we can do sha update ok
    */
    ChkDR( DRM_SHA256_Update( pSha256Context,
                              pBuffer,
                              atol( argv[ 2 ] ) ) ); /* len of buffer */

 ErrorExit:
    return dr;
} /* end proc Test_DRM_SHA256_Update */


/*
** Test_DRM_SHA256_UpdateOffset
** Tests basic SHA256 functionality for adding byte to be hashed
** argv[0] - index to global pointer to initilized sha256 sturct
** argv[1] - index to global pointer to the message to be sha'ed
** argv[2] - Len of the message to sign
** argv[3] - offset into argv[1] where the message begins
*/
DRM_RESULT DRM_CALL Test_DRM_SHA256_UpdateOffset( long argc, __in_ecount(argc) char **argv )
{
    /*
    ** Decls
    */
    DRM_RESULT dr      = DRM_SUCCESS;
    DRM_SHA256_Context *pSha256Context = NULL;
    DRM_BYTE           *pBuffer        = NULL;

    /*
    ** Arg Checks
    */
    ChkArg( 4    == argc );
    ChkArg( NULL != argv[ 2 ] );
    ChkArg( NULL != argv[ 3 ] );

    MAP_TO_GLOBAL_TEST_VAR( pSha256Context, argv[0] );
    MAP_TO_GLOBAL_TEST_VAR( pBuffer, argv[1] );

    /*
    ** Make sure that we can do sha update ok
    */
    ChkDR( DRM_SHA256_UpdateOffset( pSha256Context,
                                    pBuffer,
                                    atol( argv[2] ),
                                    atol( argv[3] ) ) ); /* len of buffer */

 ErrorExit:
    return dr;
} /* end proc Test_DRM_SHA256_Update */

/*
** Test_DRM_SHA256_Finalize
** Tests basic SHA256 finilization
** argv[0] - index to global pointer to initilized sha256 sturct
** argv[1] - index to global pointer to the result (digest). Allocates mem to this.
*/
DRM_RESULT DRM_CALL Test_DRM_SHA256_Finalize( long argc, __in_ecount(argc) char **argv )
{
    /*
    ** Decls
    */
    DRM_RESULT dr     = DRM_SUCCESS;
    DRM_SHA256_Context *pContext = NULL;
    DRM_SHA256_Digest  *pDigest  = NULL;

    /*
    ** Arg Checks
    */
    ChkArg( 2    == argc );

    MAP_TO_GLOBAL_TEST_VAR( pContext, argv[0] );
    MAP_TO_GLOBAL_TEST_VAR_Alloc( pDigest, argv[1], SIZEOF( DRM_SHA256_Digest ) );

    ChkDR(DRM_SHA256_Finalize( pContext,
                               pDigest ) );

ErrorExit:
    return dr;
} /* end proc Test_DRM_SHA256_Finalize */

/*
** No input Params. This function will test all the FIP test vectors for SHA256
*/
DRM_RESULT DRM_CALL Test_DRM_SHA256_E2E( long argc, __in_ecount(argc) char **argv )
{
    /*
    ** Decls
    */
    DRM_RESULT         dr         = DRM_SUCCESS;
    DRM_SHA256_Context shaContext = {0};
    DRM_SHA256_Digest  shaDigest  = {0};

    /*
    ** Init the SHA struct for its first use.
    */
    ChkDR( DRM_SHA256_Init( &shaContext ) );

    /*
    ** Test Vector #1: This is a functional test a small message
    */
    {
        ChkDR( DRM_SHA256_Update( &shaContext,
                                   g_szSHA256TestVector1,
                                   TEST_VECTOR1_SIZE_IN_BYTES ) );
        ChkDR( DRM_SHA256_Finalize( &shaContext, &shaDigest ) );

        if ( 0 != MEMCMP( shaDigest.m_rgbDigest,
                          g_szSHA256TestVector1Result,
                          SHA256_DIGEST_SIZE_IN_BYTES ) )
        {
            dr = DRM_S_FALSE;
            goto ErrorExit;
        } /* end if */
    } /* end local block for running SHA256 test vector #1 */

    /*
    ** Test Vector #2: This is a functional test for a block
    */
    {
        ChkDR( DRM_SHA256_Update( &shaContext,
                                   g_szSHA256TestVector2,
                                   TEST_VECTOR2_SIZE_IN_BYTES ) );
        ChkDR( DRM_SHA256_Finalize( &shaContext, &shaDigest ) );

        if ( 0 != MEMCMP( shaDigest.m_rgbDigest,
                          g_szSHA256TestVector2Result,
                          SHA256_DIGEST_SIZE_IN_BYTES ) )
        {
            dr = DRM_S_FALSE;
            goto ErrorExit;
        } /* end if */
    } /* end local block for running SHA256 test vector #2 */

    /*
    ** Test Vector #3: This is a functional test for a block
    */
    {
        ChkDR( DRM_SHA256_Update( &shaContext,
                                   g_szSHA256TestVector3,
                                   TEST_VECTOR3_SIZE_IN_BYTES ) );
        ChkDR( DRM_SHA256_Finalize( &shaContext, &shaDigest ) );

        if ( 0 != MEMCMP( shaDigest.m_rgbDigest,
                          g_szSHA256TestVector3Result,
                          SHA256_DIGEST_SIZE_IN_BYTES ) )
        {
            dr = DRM_S_FALSE;
            goto ErrorExit;;
        } /* end if */
    } /* end local block for running SHA256 test vector #3 */

    /*
    ** Test Vector #4: This tests to make sure that SHA256 handles the case were message + pladding
    ** overflow onto an additional block.
    */
    {
        ChkDR( DRM_SHA256_Update( &shaContext,
                                   g_szSHA256TestVector4,
                                   TEST_VECTOR4_SIZE_IN_BYTES ) );
        ChkDR( DRM_SHA256_Finalize( &shaContext, &shaDigest ) );

        if ( 0 != MEMCMP( shaDigest.m_rgbDigest,
                          g_szSHA256TestVector4Result,
                          SHA256_DIGEST_SIZE_IN_BYTES ) )
        {
            dr = DRM_S_FALSE;
            goto ErrorExit;
        } /* end if */
    } /* end local block for running SHA256 test vector #4 */

    /*
    ** Test Vector #5: This tests zero sized message
    */
    {
        ChkDR( DRM_SHA256_Update( &shaContext,
                                   g_szSHA256TestVector5,
                                   TEST_VECTOR5_SIZE_IN_BYTES ) );
        ChkDR( DRM_SHA256_Finalize( &shaContext, &shaDigest ) );

        if ( 0 != MEMCMP( shaDigest.m_rgbDigest,
                          g_szSHA256TestVector5Result,
                          SHA256_DIGEST_SIZE_IN_BYTES ) )
        {
            dr = DRM_S_FALSE;
            goto ErrorExit;
        } /* end if */
    } /* end local block for running SHA256 test vector #5 */

    /*
    ** Test Vector #6: This tests large messages sizes
    */
    {
        DRM_DWORD i = 0;

        /*
        ** 1,000,000 is perfectly divisible by 64, so we don't need to worry about any
        ** excess 'a's that did not get covered.
        */
        for ( i = 0; i < TEST_VECTOR6_SIZE_IN_BYTES; i += SHA256_BLOCK_SIZE_IN_BYTES )
        {
            ChkDR( DRM_SHA256_Update( &shaContext,
                                       g_szSHA256TestVector6,
                                       SHA256_BLOCK_SIZE_IN_BYTES ) );
        } /* end for */
        ChkDR( DRM_SHA256_Finalize( &shaContext, &shaDigest ) );

        /*
        ** Make sure everything came out as expected
        */
        if ( 0 != MEMCMP( shaDigest.m_rgbDigest,
                          g_szSHA256TestVector6Result,
                          SHA256_DIGEST_SIZE_IN_BYTES ) )
        {
            dr = DRM_S_FALSE;
            goto ErrorExit;
        } /* end if */
    } /* end local block for running SHA256 test vector #6 */

    /*
    ** Test Vector #7: This tests if we can deal with non-dword aligned buffers
    */
    {
        ChkDR( DRM_SHA256_Update( &shaContext,
                                  g_szSHA256TestVector7 + __CB_DECL( 2 ),
                                  TEST_VECTOR7_SIZE_IN_BYTES ) );
        ChkDR( DRM_SHA256_Finalize( &shaContext, &shaDigest ) );

        if ( 0 != MEMCMP( shaDigest.m_rgbDigest,
                          g_szSHA256TestVector7Result,
                          SHA256_DIGEST_SIZE_IN_BYTES ) )
        {
            dr = DRM_S_FALSE;
            goto ErrorExit;
        } /* end if */
    } /* end local block for running SHA256 test vector #7 */

ErrorExit:
    return dr;
    UNREFERENCED_PARAMETER( argc );
    UNREFERENCED_PARAMETER( argv );
} /* end proc Test_DRM_SHA256_E2E */

/*
** No input Params. This function will test all the FIP test vectors for SHA256
** when they are stored in a random offset in an array.
*/
DRM_RESULT DRM_CALL Test_DRM_SHA256_E2EOffset( long argc, __in_ecount(argc) char **argv )
{
    /*
    ** Decls
    */
    DRM_RESULT         dr              = DRM_SUCCESS;
    DRM_SHA256_Context shaContext      = {0};
    DRM_SHA256_Digest  shaDigest       = {0};
    DRM_DWORD          dwRandomOffset  = 0;
    DRM_BYTE           rgbOffset[1000] = {0}; /* more than enough space to hold any FIPS test vector */


    /*
    ** Init the SHA struct for its first use.
    */
    ChkDR(DRM_SHA256_Init( &shaContext ) );

    /*
    ** Test Vector #1: This is a functional test a small message
    */
    {
        /*
        ** Generate the random offset, mod it by the max then copy the message into that offset.
        */
        ChkDR( Oem_Random_GetBytes( NULL, ( DRM_BYTE* )&dwRandomOffset, 4 ) );
        dwRandomOffset %= MAX_RANDOM_OFFSET;
        DRM_BYT_CopyBytes( rgbOffset,
                           dwRandomOffset,
                           g_szSHA256TestVector1,
                           0,
                           TEST_VECTOR1_SIZE_IN_BYTES );

        /*
        ** Do the Hashing then check to see if the result is correct.
        */
        ChkDR(DRM_SHA256_UpdateOffset( &shaContext,
                                        rgbOffset,
                                        TEST_VECTOR1_SIZE_IN_BYTES,
                                        dwRandomOffset ) );
        ChkDR(DRM_SHA256_Finalize( &shaContext, &shaDigest ) );

        if ( 0 != MEMCMP( shaDigest.m_rgbDigest,
                          g_szSHA256TestVector1Result,
                          SHA256_DIGEST_SIZE_IN_BYTES ) )
        {
            dr = DRM_S_FALSE;
            goto ErrorExit;
        } /* end if */
    } /* end local block for running SHA256 test vector #1 */

    /*
    ** Test Vector #2: This is a functional test for a block
    */
    {
        /*
        ** Generate the random offset, mod it by the max then copy the message into that offset.
        */
        ChkDR( Oem_Random_GetBytes( NULL, ( DRM_BYTE* )&dwRandomOffset, 4 ) );
        dwRandomOffset %= MAX_RANDOM_OFFSET;
        DRM_BYT_CopyBytes( rgbOffset,
                           dwRandomOffset,
                           g_szSHA256TestVector2,
                           0,
                           TEST_VECTOR2_SIZE_IN_BYTES );

        /*
        ** Do the Hashing then check to see if the result is correct.
        */
        ChkDR(DRM_SHA256_UpdateOffset( &shaContext,
                                        rgbOffset,
                                        TEST_VECTOR2_SIZE_IN_BYTES,
                                        dwRandomOffset ) );
        ChkDR(DRM_SHA256_Finalize( &shaContext, &shaDigest ) );

        if ( 0 != MEMCMP( shaDigest.m_rgbDigest,
                          g_szSHA256TestVector2Result,
                          SHA256_DIGEST_SIZE_IN_BYTES ) )
        {
            dr = DRM_S_FALSE;
            goto ErrorExit;
        } /* end if */
    } /* end local block for running SHA256 test vector #2 */

    /*
    ** Test Vector #3: This is a functional test for a block
    */
    {
        /*
        ** Generate the random offset, mod it by the max then copy the message into that offset.
        */
        ChkDR( Oem_Random_GetBytes( NULL, ( DRM_BYTE* )&dwRandomOffset, 4 ) );
        dwRandomOffset %= MAX_RANDOM_OFFSET;
        DRM_BYT_CopyBytes( rgbOffset,
                           dwRandomOffset,
                           g_szSHA256TestVector3,
                           0,
                           TEST_VECTOR3_SIZE_IN_BYTES );

        /*
        ** Do the Hashing then check to see if the result is correct.
        */
        ChkDR(DRM_SHA256_UpdateOffset( &shaContext,
                                        rgbOffset,
                                        TEST_VECTOR3_SIZE_IN_BYTES,
                                        dwRandomOffset ) );
        ChkDR(DRM_SHA256_Finalize( &shaContext, &shaDigest ) );

        if ( 0 != MEMCMP( shaDigest.m_rgbDigest,
                          g_szSHA256TestVector3Result,
                          SHA256_DIGEST_SIZE_IN_BYTES ) )
        {
            dr = DRM_S_FALSE;
            goto ErrorExit;;
        } /* end if */
    } /* end local block for running SHA256 test vector #3 */

    /*
    ** Test Vector #4: This tests to make sure that SHA256 handles the case were message + pladding
    ** overflow onto an additional block.
    */
    {
        /*
        ** Generate the random offset, mod it by the max then copy the message into that offset.
        */
        ChkDR( Oem_Random_GetBytes( NULL, ( DRM_BYTE* )&dwRandomOffset, 4 ) );
        dwRandomOffset %= MAX_RANDOM_OFFSET;
        DRM_BYT_CopyBytes( rgbOffset,
                           dwRandomOffset,
                           g_szSHA256TestVector4,
                           0,
                           TEST_VECTOR4_SIZE_IN_BYTES );

        /*
        ** Do the Hashing then check to see if the result is correct.
        */
        ChkDR(DRM_SHA256_UpdateOffset( &shaContext,
                                        rgbOffset,
                                        TEST_VECTOR4_SIZE_IN_BYTES,
                                        dwRandomOffset ) );
        ChkDR(DRM_SHA256_Finalize( &shaContext, &shaDigest ) );

        if ( 0 != MEMCMP( shaDigest.m_rgbDigest,
                          g_szSHA256TestVector4Result,
                          SHA256_DIGEST_SIZE_IN_BYTES ) )
        {
            dr = DRM_S_FALSE;
            goto ErrorExit;
        } /* end if */
    } /* end local block for running SHA256 test vector #4 */

    /*
    ** Test Vector #5: This tests zero sized message
    */
    {
        /*
        ** Generate the random offset, mod it by the max then copy the message into that offset.
        */
        ChkDR( Oem_Random_GetBytes( NULL, ( DRM_BYTE* )&dwRandomOffset, 4 ) );
        dwRandomOffset %= MAX_RANDOM_OFFSET;
        DRM_BYT_CopyBytes( rgbOffset,
                           dwRandomOffset,
                           g_szSHA256TestVector5,
                           0,
                           TEST_VECTOR5_SIZE_IN_BYTES );

        /*
        ** Do the Hashing then check to see if the result is correct.
        */
        ChkDR(DRM_SHA256_UpdateOffset( &shaContext,
                                        rgbOffset,
                                        TEST_VECTOR5_SIZE_IN_BYTES,
                                        dwRandomOffset ) );
        ChkDR(DRM_SHA256_Finalize( &shaContext, &shaDigest ) );

        if ( 0 != MEMCMP( shaDigest.m_rgbDigest,
                          g_szSHA256TestVector5Result,
                          SHA256_DIGEST_SIZE_IN_BYTES ) )
        {
            dr = DRM_S_FALSE;
            goto ErrorExit;
        } /* end if */
    } /* end local block for running SHA256 test vector #5 */

    /*
    ** Test Vector #6: This tests large messages sizes
    */
    {
        DRM_DWORD i = 0;

        /*
        ** 1,000,000 is perfectly divisible by 64, so we don't need to worry about any
        ** excess 'a's that did not get covered.
        */
        for (i = 0; i < TEST_VECTOR6_SIZE_IN_BYTES; i += SHA256_BLOCK_SIZE_IN_BYTES)
        {
            /*
            ** Generate the random offset, mod it by the max then copy the message into that offset.
            */
            ChkDR( Oem_Random_GetBytes( NULL, ( DRM_BYTE* )&dwRandomOffset, 4 ) );
            dwRandomOffset %= MAX_RANDOM_OFFSET;
            DRM_BYT_CopyBytes( rgbOffset,
                               dwRandomOffset,
                               g_szSHA256TestVector6,
                               0,
                               SHA256_BLOCK_SIZE_IN_BYTES );

            /*
            ** Do the Hashing then check to see if the result is correct.
            */
            ChkDR(DRM_SHA256_UpdateOffset( &shaContext,
                                            rgbOffset,
                                            SHA256_BLOCK_SIZE_IN_BYTES,
                                            dwRandomOffset ) );
        } /* end for */
        ChkDR( DRM_SHA256_Finalize( &shaContext, &shaDigest ) );

        /*
        ** Make sure everything came out as expected
        */
        if ( 0 != MEMCMP( shaDigest.m_rgbDigest,
                          g_szSHA256TestVector6Result,
                          SHA256_DIGEST_SIZE_IN_BYTES ) )
        {
            dr = DRM_S_FALSE;
            goto ErrorExit;
        } /* end if */
    } /* end local block for running SHA256 test vector #6 */

ErrorExit:
    return dr;
    UNREFERENCED_PARAMETER(argc);
    UNREFERENCED_PARAMETER(argv);
} /* end proc Test_DRM_SHA256_E2E */

/*
**  Test_DRM_HMAC_Init - will initialize internal HMAC data.
**   argv[0]:DRM_HMAC_CONTEXT index
**   argv[1]:Length to allocate for the key
**   argv[2]:Length of key to pass to Init
**   argv[3]:Hash algorithm to use: SHA1, SHA256
**   argv[4]:Known-data test file to use (optional)
*/
DRM_RESULT DRM_CALL Test_DRM_HMAC_Init( long argc, __in_ecount(argc) char **argv )
{
    DRM_RESULT        dr           = DRM_SUCCESS;
    DRM_BYTE         *pbTestKey    = NULL;
    DRM_DWORD         cbTestKey    = 0;
    DRM_DWORD         cbDatalength = 0;
    DRM_HMAC_CONTEXT *pHmacContext = NULL;
    DRM_BYTE          rgbTempBuffer[__CB_DECL( DRM_SHA_BLOCK_SIZE_MAX )] = { 0 };
    eDRM_SHA_SUPPORTED_ALGORITHMS eAlgorithm = ( eDRM_SHA_SUPPORTED_ALGORITHMS ) 2; /* start invalid */

    ChkArg( argc    >= 4 );
    ChkArg( argv[3] != NULL )

    MAP_TO_GLOBAL_TEST_VAR_Alloc( pHmacContext, argv[0], SIZEOF( DRM_HMAC_CONTEXT ) );

    if ( argc == 5  && argv[4] != NULL )
    {
        ChkBOOL( LoadTestFile( "files", argv[4], &pbTestKey, &cbDatalength), DRM_E_FILEREADERROR );
    }
    else if ( argv[1] != NULL )
    {
        GET_SIZE( 1, cbDatalength );
        ChkMem( pbTestKey = ( DRM_BYTE * )Oem_MemAlloc( cbDatalength ) );
        ChkDR( Oem_Random_GetBytes( NULL, pbTestKey, cbDatalength ) );
    }

    /*
    ** set size of data to be hashed
    */
    GET_SIZE( 2, cbTestKey );

    /*
    ** zero the memory for compare
    */
    MEMSET( rgbTempBuffer, 0, DRM_SHA_BLOCK_SIZE_MAX );

    GET_SHA_ALGORITHM( 3, eAlgorithm );
    ChkDR( DRM_HMAC_Init( pHmacContext, eAlgorithm, pbTestKey, cbTestKey ) );

    if (eAlgorithm == eDRM_SHA_1 )
    {
        /*
        ** verify the HMAC context after HMACInit
        */
        ChkBOOL( pHmacContext->shaContext.context.sha1Context.dwHighByteCount == 0
              && pHmacContext->shaContext.context.sha1Context.dwLowByteCount  != 0
              && pHmacContext->shaContext.context.sha1Context.ABCDE[0]        != 0
              && pHmacContext->shaContext.context.sha1Context.ABCDE[1]        != 0
              && pHmacContext->shaContext.context.sha1Context.ABCDE[2]        != 0
              && pHmacContext->shaContext.context.sha1Context.ABCDE[3]        != 0
              && pHmacContext->shaContext.context.sha1Context.ABCDE[4]        != 0,
                 DRM_E_TEST_HMAC_FAILURE );
    }


    ChkBOOL( MEMCMP( rgbTempBuffer, pHmacContext->rgbBuffer, DRM_SHA_BLOCK_SIZE_MAX ) != 0,
             DRM_E_TEST_HMAC_FAILURE );

ErrorExit:
    SAFE_OEM_FREE( pbTestKey );
    return dr;
}

/*
**  Test_DRM_HMAC_Update - updates the running hash context with new data to be hashed
**   argv[0]:DRM_HMAC_CONTEXT: NULL, NORMAL
**   argv[1]:Length of data to allocate
**   argv[2]:Length in bytes to actually add to the hash
**   argv[3]:Known-data test file to use (optional)
*/
DRM_RESULT DRM_CALL Test_DRM_HMAC_Update( long argc, __in_ecount(argc) char **argv )
{
    DRM_RESULT        dr           = DRM_SUCCESS;
    DRM_BYTE         *pbData       = NULL;
    DRM_DWORD         cbData       = 0;
    DRM_DWORD         cbDatalength = 0;
    DRM_HMAC_CONTEXT  oHmacContext = DRM_HMAC_CONTEXT_DEFAULT;
    DRM_HMAC_CONTEXT *pHmacContext = NULL;
    DRM_BYTE          rgbTempBuffer[__CB_DECL( DRM_SHA1_BLOCK_SIZE )] = { 0 };    /* This is 64 bytes.  */

    ChkArg( argc >= 3 );

    MAP_TO_GLOBAL_TEST_VAR( pHmacContext, argv[0] );

    if ( argc == 4 && argv[3] != NULL  )
    {
        ChkBOOL( LoadTestFile( "files", argv[3], &pbData, &cbDatalength), DRM_E_FILEREADERROR );
    }
    else if ( argv[1] != NULL )
    {
        GET_SIZE( 1, cbDatalength );
        ChkMem( pbData = ( DRM_BYTE * )Oem_MemAlloc( cbDatalength ) );
        ChkDR( Oem_Random_GetBytes( NULL, pbData, cbDatalength ) );
    }

    /*
    ** set size of data to be hashed
    */
    GET_SIZE( 2, cbData );
    /*
    ** zero the memory for compare
    */
    if( pHmacContext != NULL )
    {
        MEMCPY( &oHmacContext, pHmacContext, SIZEOF( DRM_HMAC_CONTEXT ) );
    }

    ChkDR( DRM_HMAC_Update( pHmacContext, pbData, cbData ) );

    /*
    ** verify the HMAC context after HMACUpdate
    */
    if ( pHmacContext->shaContext.eAlgorithm == eDRM_SHA_1 )
    {
        ChkBOOL( pHmacContext->shaContext.context.sha1Context.dwHighByteCount == 0
              && pHmacContext->shaContext.context.sha1Context.dwLowByteCount  == oHmacContext.shaContext.context.sha1Context.dwLowByteCount + cbData,
                 DRM_E_TEST_HMAC_FAILURE );
    }
    ChkBOOL( MEMCMP( rgbTempBuffer, pHmacContext->rgbBuffer, DRM_SHA_BLOCK_SIZE_MAX ) != 0, DRM_E_TEST_HMAC_FAILURE );


ErrorExit:
    SAFE_OEM_FREE( pbData );
    return dr;
}


/*
** Test_DRM_HMAC_Finalize
**   argv[0]:DRM_HMAC_CONTEXT index
**   argv[1]:Length of bytes to be filled with the resultant hash.
**   argv[2]:Length in bytes of cbKeyedHash Only a maximum of 20 bytes can be returned.
**   argv[3]:Known-data test file to use (optional)
*/
DRM_RESULT DRM_CALL Test_DRM_HMAC_Finalize( long argc, __in_ecount(argc) char **argv )
{
    DRM_RESULT        dr           = DRM_SUCCESS;
    DRM_DWORD         cbDatalength = 0;
    DRM_BYTE         *pbKnownHash  = NULL;
    DRM_BYTE         *pbKeyedHash  = NULL;
    DRM_DWORD         cbKeyedHash  = 0;
    DRM_HMAC_CONTEXT *pHmacContext = NULL;

    ChkArg( argc >= 3 );

    MAP_TO_GLOBAL_TEST_VAR( pHmacContext, argv[0] );

    if ( argv[1] != NULL )
    {
        GET_SIZE( 1, cbDatalength );

        ChkMem( pbKeyedHash = ( DRM_BYTE * )Oem_MemAlloc( cbDatalength ) );
        ChkDR( Oem_Random_GetBytes( NULL, pbKeyedHash, cbDatalength ) );
    }

    if ( argc == 4 && argv[3] != NULL )
    {
        ChkBOOL( LoadTestFile( "files", argv[3], &pbKnownHash, &cbDatalength), DRM_E_FILEREADERROR );
    }

    /*
    ** set size of data to be hashed
    */
    if ( argv[2] != NULL )
    {
        GET_SIZE( 2, cbKeyedHash );
    }
    ChkDR( DRM_HMAC_Finalize( pHmacContext, pbKeyedHash, cbKeyedHash ) );
    ChkBOOL( pbKeyedHash != NULL, DRM_E_TEST_HMAC_FAILURE );

    if ( pbKnownHash != NULL )
    {
        ChkBOOL( MEMCMP( pbKeyedHash, pbKnownHash, cbKeyedHash ) == 0,
                 DRM_E_TEST_HMAC_FAILURE );
    }

ErrorExit:
    SAFE_OEM_FREE( pbKeyedHash );
    SAFE_OEM_FREE( pbKnownHash );
    return dr;
}

/* Test_RC4_Cipher
** argv[0]: RC4_KEYSTRUCT: NORMAL, NULL
** argv[1]: size of pRC4key in bytes
** argv[2]: size of pbData being encrypt/decrypt in bytes
*/

DRM_RESULT DRM_CALL Test_RC4_Cipher( long argc, __in_ecount(argc) char **argv )
{
    DRM_RESULT     dr         = DRM_SUCCESS;
    RC4_KEYSTRUCT *pRC4ks     = NULL;
    RC4_KEYSTRUCT  pTmpRC4ks  = { 0 };
    DRM_BYTE      *pRC4key    = NULL;
    DRM_BYTE      *pbData     = NULL;
    DRM_BYTE      *pbTmpData  = NULL;
    DRM_DWORD      cbKeyLen   = 0;
    DRM_DWORD      cbDataSize = 0;

    ChkArg( argc == 3 );

    if( argv[0] != NULL )
    {
        pRC4ks = &g_rc4ks;
    }

    if ( argv[1] != NULL )
    {
        cbKeyLen = atoi( argv[1] );
        ChkMem( pRC4key = ( DRM_BYTE * )Oem_MemAlloc( cbKeyLen ) );
        ChkDR( Oem_Random_GetBytes( NULL, pRC4key, cbKeyLen ) );
    }

    if ( argv[2] != NULL )
    {
        cbDataSize = atoi( argv[2] );
        ChkMem( pbData = ( DRM_BYTE * )Oem_MemAlloc( cbDataSize ) );
        ChkDR( Oem_Random_GetBytes( NULL, pbData, cbDataSize ) );
        ChkMem( pbTmpData = ( DRM_BYTE * )Oem_MemAlloc( cbDataSize ) );
    }

    /*
    ** save the pRC4Key before calling the DRM_rc4_key
    */
    MEMCPY( &pTmpRC4ks, pRC4ks, SIZEOF( RC4_KEYSTRUCT ) );
    DRM_RC4_KeySetup( pRC4ks, cbKeyLen, pRC4key );

    /*
    ** compare the pRC4ks after calling the DRM_rc4_key, pRC4ks should not be the same
    */
    ChkBOOL( MEMCMP( &pTmpRC4ks, pRC4ks, SIZEOF( RC4_KEYSTRUCT ) ) != 0,
             DRM_E_TEST_RC4KEY_FAILED );

    MEMCPY( pbTmpData, pbData, cbDataSize );

    /*
    ** The first call to DRM_rc4 encrypts pbData
    */
    DRM_RC4_Cipher( pRC4ks, cbDataSize, pbData );

    /*
    ** compare the pbData after calling the DRM_rc4, pbData should not be the same
    */
    ChkBOOL( MEMCMP( pbTmpData, pbData, cbDataSize ) != 0,
             DRM_E_TEST_RC4KEY_FAILED );

    /*
    ** save the pRC4ks before calling the DRM_rc4_key
    */
    MEMCPY( &pTmpRC4ks, pRC4ks, SIZEOF( RC4_KEYSTRUCT ) );
    DRM_RC4_KeySetup( pRC4ks, cbKeyLen, pRC4key );

    /*
    ** compare the pRC4ks after calling the DRM_rc4_key, pRC4ks should not be the same
    */
    ChkBOOL( MEMCMP( &pTmpRC4ks, pRC4ks, SIZEOF( RC4_KEYSTRUCT ) ) != 0,
             DRM_E_TEST_RC4KEY_FAILED );

    DRM_RC4_Cipher( pRC4ks, cbDataSize, pbData );

    /*
    ** compare the pbData with the one before being encrypted, it should be the same
    */
    ChkBOOL( MEMCMP( pbTmpData, pbData, cbDataSize ) == 0,
             DRM_E_TEST_RC4KEY_FAILED );

ErrorExit:
    SAFE_OEM_FREE( pRC4key );
    SAFE_OEM_FREE( pbData );
    SAFE_OEM_FREE( pbTmpData );
    return dr;
}

/* Test_DES_Cipher
** argv[0]: DESTable: NORMAL, NULL
** argv[1]: size of pkey in bytes
** argv[2]: size of pbIn in bytes to be encrypted
** argv[3]: size of pbOut in bytes to hold encrypted data
*/

DRM_RESULT DRM_CALL Test_DES_Cipher( long argc, __in_ecount(argc) char **argv )
{
    DRM_RESULT dr            = DRM_SUCCESS;
    DESTable  *pTable        = NULL;
    DESTable   tmpTable      = { 0 };
    DRM_BYTE  *pKey          = NULL;
    DRM_BYTE  *pbIn          = NULL;
    DRM_BYTE  *pbOut         = NULL;
    DRM_BYTE  *pbOrgIn       = NULL;
    DRM_DWORD  cbKeyLen      = 0;
    DRM_DWORD  cbInDataSize  = 0;
    DRM_DWORD  cbOutDataSize = 0;

    ChkArg( argc == 4 );

    if( argv[0] != NULL )
    {
        pTable = &g_desTable;
    }

    if ( argv[1] != NULL )
    {
        cbKeyLen = atoi( argv[1] );
        ChkBOOL( cbKeyLen >= DES_KEYSIZE, DRM_E_TEST_DESKEY_FAILED );
        ChkMem( pKey = ( DRM_BYTE * )Oem_MemAlloc( cbKeyLen ) );
        ChkDR( Oem_Random_GetBytes( NULL, pKey, cbKeyLen ) );
    }
    if ( argv[2] != NULL )
    {
        cbInDataSize = atoi( argv[2] );
        ChkMem( pbIn = ( DRM_BYTE * )Oem_MemAlloc( cbInDataSize ) );
        ChkDR( Oem_Random_GetBytes( NULL, pbIn, cbInDataSize ) );

        ChkMem( pbOrgIn = ( DRM_BYTE * )Oem_MemAlloc( cbInDataSize ) );
        MEMCPY( pbOrgIn, pbIn, cbInDataSize );
    }
    if ( argv[3] != NULL )
    {
        cbOutDataSize = atoi( argv[3] );
        ChkMem( pbOut = ( DRM_BYTE * )Oem_MemAlloc( cbOutDataSize ) );
        ChkDR( Oem_Random_GetBytes( NULL, pbOut, cbOutDataSize ) );
    }

    /*
    ** save the pTable before calling the deskey
    */
    MEMCPY( &tmpTable, pTable, SIZEOF( DESTable ) );
    DRM_DES_KeySetup( pTable, pKey );

    /*
    ** compare the pTable after calling the deskey, pTable should not be the same
    */
    ChkBOOL( MEMCMP( &tmpTable, pTable, SIZEOF( DESTable ) ) != 0,
             DRM_E_TEST_DESKEY_FAILED );

    /*
    ** encrypt the pbIn, pbOut will hold the encrypted data
    */
    DRM_DES_Cipher( pbIn, pbOut, pTable, DES_ENCRYPT );

    /*
    ** compare the data after encrypted, pbOut should not be the same as pbOrgIn
    */
    ChkBOOL( MEMCMP( pbOrgIn, pbOut, cbOutDataSize ) != 0,
             DRM_E_TEST_DESKEY_FAILED );

    /*
    ** Will decrypt the encrypted data
    */
    DRM_DES_KeySetup( pTable, pKey );

    /*
    ** save the last pbOut as a new pbIn for next des call
    */
    MEMCPY( pbIn, pbOut, cbInDataSize );
    DRM_DES_Cipher( pbIn, pbOut, pTable, DES_DECRYPT );

    /*
    ** compare the decrypted data with the original, pbOut should be the same as pbOrgIn
    */
    ChkBOOL( MEMCMP( pbOrgIn, pbOut, DES_BLOCKLEN ) == 0,
             DRM_E_TEST_DESKEY_FAILED );

ErrorExit:
    SAFE_OEM_FREE( pKey );
    SAFE_OEM_FREE( pbIn );
    SAFE_OEM_FREE( pbOrgIn );
    SAFE_OEM_FREE( pbOut );

    return dr;
}

/* Test_CBC64WS4_asm
** argv[0]: cbData: length in bytes of input data
*/
DRM_RESULT DRM_CALL Test_CBC64WS4_asm( long argc, __in_ecount(argc) char **argv )
{
    DRM_RESULT  dr              = DRM_SUCCESS;
    DRM_BYTE   *pbData          = NULL;
    DRM_BYTE   *pbContKey       = NULL;
    DRM_DWORD   cbData          = 0;
    DRM_DWORD   cbContentKey    = DRMCIPHERKEYLEN_RC4;
    DRM_DWORD   dwMacLength4    = 0; /* mac length in four byte blocks */
    DRM_DWORD   rgdwRC4Key[ 2 ] = { 0 };
    DRM_DWORD  *pdwLast8        = NULL;

    ChkArg( argc == 1
         && argv[0] );

    ChkMem( pbContKey = ( DRM_BYTE * )Oem_MemAlloc( cbContentKey ) );
    ChkDR( Oem_Random_GetBytes( NULL, pbContKey, cbContentKey ) );

    /*
    ** set up cipher context
    */
    g_pContext = &g_Context;
    ChkDR( DRM_CPHR_Init( g_pContext, eDRM_RC4_CIPHER, cbContentKey, pbContKey ) ); /* Uses RC4 cipher */

    cbData = atoi( argv[0] );

    ChkMem( pbData = ( DRM_BYTE * )Oem_MemAlloc( cbData ) );
    ChkDR( Oem_Random_GetBytes( NULL, pbData, cbData ) );

    ChkMem( g_pbOrgDataCBC = ( DRM_BYTE * )Oem_MemAlloc( cbData ) );
    MEMCPY( g_pbOrgDataCBC, pbData, cbData );
    ChkMem( g_pbEncDataCBC = ( DRM_BYTE * )Oem_MemAlloc( cbData ) );

    /*
    ** small packet case: MAC does not handle it
    */
    if ( cbData < 16 )
    {
        for ( rgdwRC4Key[0] = 0; rgdwRC4Key[0] < cbData; rgdwRC4Key[0]++ )
        {
            pbData[rgdwRC4Key[0]] ^= g_pContext->cipher.cipherRC4.shaOut[rgdwRC4Key[0]];
        }
    }
    else
    {
        /*
        ** standard case: cbData >= 16
        */
        dwMacLength4 = ( cbData / 8 ) * 2; /* making sure block number is even */
        pdwLast8 = ( DRM_DWORD * )( pbData + 4 * dwMacLength4 - 8 );

        /*
        ** run MAC over data
        */
        DRM_CBC_Mac( pbData, dwMacLength4, rgdwRC4Key, &g_pContext->cipher.cipherRC4.mackey );

        /*
        ** RC4 encrypt content
        */
        DRM_RC4_KeySetup( &g_pContext->cipher.cipherRC4.rc4ks, 8, ( DRM_BYTE * )rgdwRC4Key );
        DRM_RC4_Cipher( &g_pContext->cipher.cipherRC4.rc4ks, cbData, pbData );

        /*
        ** DES encrypt MAC and put it in the right place
        */
        rgdwRC4Key[0] ^= g_pContext->cipher.cipherRC4.desS1[0];
        rgdwRC4Key[1] ^= g_pContext->cipher.cipherRC4.desS1[1];
        DRM_DES_Cipher( ( DRM_BYTE * )rgdwRC4Key,
                        ( DRM_BYTE * )pdwLast8,
                        &g_pContext->cipher.cipherRC4.destable, DES_ENCRYPT );
        pdwLast8[0]   ^= g_pContext->cipher.cipherRC4.desS2[0];
        pdwLast8[1]   ^= g_pContext->cipher.cipherRC4.desS2[1];
    }
    /*
    ** save the pbData to g_pbEncDataCBC
    */
    MEMCPY( g_pbEncDataCBC, pbData, cbData );
    MEMCPY( &g_Context, g_pContext, SIZEOF( DRM_CIPHER_CONTEXT ) );

ErrorExit:
    SAFE_OEM_FREE( pbContKey );
    SAFE_OEM_FREE( pbData );

    return dr;
}

/* Test_InvCBC64WS4_asm
** argv[0]: cbData: length in bytes of input data
*/
DRM_RESULT DRM_CALL Test_InvCBC64WS4_asm( long argc, __in_ecount(argc) char **argv )
{
    DRM_RESULT dr              = DRM_SUCCESS;
    DRM_BYTE  *pbData          = NULL;
    DRM_DWORD  cbData          = 0;
    DRM_DWORD  dwMacLength4    = 0; /* mac length in four byte blocks */
    DRM_DWORD  rgdwRC4Key[ 2 ] = { 0 };
    DRM_DWORD *pdwLast8        = NULL;

    ChkArg( argc == 1
         && argv[0] != NULL );

    /*
    ** set up cipher context
    */
    g_pContext = &g_Context;
    g_pContext->eCipherType = eDRM_RC4_CIPHER;

    pbData = g_pbEncDataCBC;

    cbData = atoi( argv[0] );

    /*
    ** small packet case: MAC does not handle it
    */
    if ( cbData < 16 )
    {
        for ( rgdwRC4Key[0]=0; rgdwRC4Key[0] < cbData; rgdwRC4Key[0]++ )
        {
            pbData[rgdwRC4Key[0]] ^= g_pContext->cipher.cipherRC4.shaOut[rgdwRC4Key[0]];
        }
    }
    else
    {
        /*
        ** making sure block number is even
        */
        dwMacLength4 = ( cbData / 8 ) * 2;
        pdwLast8 = ( DRM_DWORD * )( pbData + 4 * dwMacLength4 - 8 );

        /*
        ** DesS decrypt RC4 key
        */
        pdwLast8[0] ^= g_pContext->cipher.cipherRC4.desS2[0];
        pdwLast8[1] ^= g_pContext->cipher.cipherRC4.desS2[1];
        DRM_DES_Decrypt( ( DRM_BYTE * )pdwLast8,
                         ( DRM_BYTE * )rgdwRC4Key,
                         &g_pContext->cipher.cipherRC4.destable );
        rgdwRC4Key[0] ^= g_pContext->cipher.cipherRC4.desS1[0];
        rgdwRC4Key[1] ^= g_pContext->cipher.cipherRC4.desS1[1];

        /*
        ** RC4 decrypt content
        */
        DRM_RC4_KeySetup( &g_pContext->cipher.cipherRC4.rc4ks, 8, ( DRM_BYTE * )rgdwRC4Key );
        DRM_RC4_Cipher( &g_pContext->cipher.cipherRC4.rc4ks, cbData, pbData );

        /*
        ** take content of rgdwRC4Key, reverse the mac and put it into the right position
        */
        pdwLast8[0] = rgdwRC4Key[0];
        pdwLast8[1] = rgdwRC4Key[1];
        DRM_CBC_InverseMac( pbData,
                            dwMacLength4,
                            &( g_pContext->cipher.cipherRC4.mackey ),
                            &( g_pContext->cipher.cipherRC4.invmackey ) );
    }

    /*
    ** compare the decrypted data with the original, pbOut should be the same as pbOrgIn
    */
    ChkBOOL( MEMCMP( g_pbOrgDataCBC, pbData, SIZEOF( g_pbEncDataCBC ) ) == 0,
             DRM_E_TEST_CBC_INVERSEMAC_FAILURE );

ErrorExit:
    return dr;
}

/*
** Test_OEM_AES_Cipher
** Tests OEM aes encrypt/decrypt of one block using random data/key
**
** if argv[0] & 1 then misalign the data.
*/
#define Test_OEM_AES_Cipher_MisAlign_Data 1
DRM_RESULT DRM_CALL Test_OEM_AES_Cipher( long argc, __in_ecount(argc) char **argv )
{
    DRM_RESULT   dr            = DRM_SUCCESS;
    DRM_BYTE    *pbDataClr     = NULL;
    DRM_BYTE    *pbData        = NULL;
    DRM_BYTE    *pbDataOffset  = NULL;
    DRM_BYTE     cbData        = DRM_AES_BLOCKLEN;
    DRM_BYTE     rgbKey[__CB_DECL( DRM_AES_KEYSIZE_128 )] = { 0 };
    DRM_DWORD    dwOption      = 0;

    /*
    ** Test set key
    */
    ChkDR( Oem_Random_GetBytes( NULL, rgbKey, DRM_AES_KEYSIZE_128 ) );

    ChkDR( Oem_Aes_SetKey( rgbKey, &g_aesKey ) );

    if ( argc > 0 && argv[0] != NULL )
    {
        dwOption = ( DRM_DWORD ) strtoul( argv[0], NULL, 0 );
    }
    if ( ( dwOption & 1 ) == 1 )
    {
        cbData += 1;
    }
    /*
    ** Test encrypt
    */
    ChkMem( pbDataClr = ( DRM_BYTE * )Oem_MemAlloc( cbData ) );
    ChkMem( pbData    = ( DRM_BYTE * )Oem_MemAlloc( cbData ) );
    ChkDR( Oem_Random_GetBytes( NULL, pbDataClr, cbData ) );

    MEMCPY( pbData, pbDataClr, cbData );


    pbDataOffset = pbData;
    if ( ( dwOption & Test_OEM_AES_Cipher_MisAlign_Data ) == 1 )
    {
        pbDataOffset += 1;
    }

    ChkDR( Oem_Aes_EncryptOne( &g_aesKey, pbDataOffset ) );
    ChkBOOL( MEMCMP( pbData, pbDataClr, DRM_AES_BLOCKLEN ) != 0,
             DRM_E_TEST_ENCRYPT_ERROR );

    /*
    ** Test decrypt
    */
    ChkDR( Oem_Aes_DecryptOne( &g_aesKey, pbDataOffset ) );
    ChkBOOL( MEMCMP( pbData, pbDataClr, DRM_AES_BLOCKLEN ) == 0,
             DRM_E_TEST_DECRYPT_ERROR );

    /*
    ** Test zero key
    */
    ChkDR( Oem_Aes_ZeroKey( &g_aesKey ) );

ErrorExit:
    SAFE_OEM_FREE( pbData );
    SAFE_OEM_FREE( pbDataClr );

    return dr;
}
#undef Test_OEM_AES_Cipher_MisAlign_Data


/*
** Loads data from files into the global key, clear data, and ecrypted data variables for symmetric tests
** Pair with data generated by a tool using another AES Implementation
**
** Arguments:
**              argv[0] -- Cipher Type. Allowed values:  AES_COUNTER, RC4
**              argv[1] -- key file
**              argv[2] -- clear file
**              argv[3] -- cipher file
**              argv[4] -- opt, DataID file. Required if Cipher Type is AES_COUNTER, otherwise ignored.
*/
DRM_RESULT DRM_CALL LoadSymmetricTestData( long argc, __inout_ecount( argc ) char **argv )
{
    DRM_RESULT  dr             = DRM_SUCCESS;
    DRM_BYTE   *pbKey          = NULL;
    DRM_DWORD   cbKeySize      = 0;
    DRM_BYTE   *pbClearData    = NULL;
    DRM_DWORD   cbClearData    = 0;
    DRM_BYTE   *pbCipherData   = NULL;
    DRM_DWORD   cbCipherData   = 0;
    DRM_BYTE   *pbDataID     = NULL;
    DRM_DWORD   cbDataID       = 0;
    DRM_SUPPORTED_CIPHERS eCipher = eDRM_RC4_CIPHER;

    ChkArg( cbClearData <= MAX_CB_SYMMETRIC_BUFFER  );

    ChkArg( argc    >= 4 );
    ChkArg( argv[0] != NULL );
    ChkArg( argv[1] != NULL );
    ChkArg( argv[2] != NULL );
    ChkArg( argv[3] != NULL );

    cbCipherData = cbClearData;



    if( strncmp( argv[0], "RC4", 3 ) != 0 )
    {
        ChkArg( strncmp( argv[0], "AES_COUNTER", 11 ) == 0 );
        ChkArg( argc    >  4 );
        ChkArg( argv[4] != NULL );
        eCipher = eDRM_AES_COUNTER_CIPHER;
        ChkBOOL( LoadTestFile( "files", argv[4], &pbDataID, &cbDataID), DRM_E_FILEREADERROR );
        NETWORKBYTES_TO_QWORD(g_qwSymmetricDataID, pbDataID, 0);
    }

    ChkBOOL( LoadTestFile( "files", argv[1], &pbKey, &cbKeySize ), DRM_E_FILEREADERROR );
    ChkBOOL( LoadTestFile( "files", argv[2], &pbClearData, &cbClearData ), DRM_E_FILEREADERROR );
    ChkBOOL( LoadTestFile( "files", argv[3], &pbCipherData, &cbCipherData ), DRM_E_FILEREADERROR );

    MEMCPY( g_pbSymmetricClearData, pbClearData, cbClearData );
    MEMCPY( g_pbSymmetricEncryptedData, pbCipherData, cbCipherData );

    ChkDR( DRM_CPHR_Init( &g_Context, eCipher, cbKeySize, pbKey ) );

ErrorExit:
    SAFE_OEM_FREE( pbKey );
    SAFE_OEM_FREE( pbDataID );
    SAFE_OEM_FREE( pbClearData );
    SAFE_OEM_FREE( pbCipherData );
    return dr;
}



/*
** Test_DRM_AES_Cipher
** Tests basic drmaes functionality for encrypting or decrypting
** argv[0] - AES mode ( ECB, CTR, or CBC )
** argv[1] - Key/Data type ( KNOWN or RANDOM )
*/
DRM_RESULT DRM_CALL Test_DRM_AES_Cipher( long argc, __in_ecount(argc) char **argv )
{
    DRM_RESULT   dr            = DRM_SUCCESS;
    DRM_BOOL     fRandomTest   = TRUE;
    DRM_BYTE    *pbDataClr     = NULL;
    DRM_BYTE    *pbData        = NULL;
    DRM_UINT64   qwDataId      = DRM_UI64(1);
    DRM_UINT64   qwZero        = DRM_UI64LITERAL( 0, 0 );
    DRM_DWORD    cbDataLen     = 0;
    DRM_BYTE     rgbKey[__CB_DECL( DRM_AES_KEYSIZE_128 )] = { 0 };
    DRM_BYTE     rgbIV[__CB_DECL( DRM_AES_BLOCKLEN )]     = { 0 };
    DRM_AES_SUPPORTED_MODES       eMode                         = eDRM_AES_CBC_MODE;
    DRM_AES_COUNTER_MODE_CONTEXT  ctrContext                    = { 0 };
    const DRM_BYTE               *pKnownCipher                  = NULL;

    ChkArg( argc    == 2
         && argv[0] != NULL
         && argv[1] != NULL );

    if ( strncmp( argv[0], "CBC", 3 ) != 0 )
    {
        if ( strncmp( argv[0], "ECB", 3 ) != 0  )
        {
            ChkArg( strncmp( argv[0], "CTR", 3 ) == 0  );
            eMode = eDRM_AES_CTR_MODE;
        }
        else
        {
            eMode = eDRM_AES_ECB_MODE;
        }
    }
    if ( strncmp( argv[1], "RANDOM", 6 ) != 0 )
    {
        ChkArg( strncmp( argv[1], "KNOWN", 5 ) == 0 );
        fRandomTest = FALSE;
    }

    /*
    ** Initialize the key and plaintext
    */

    /*
    ** Random key and cleartext
    */
    if ( fRandomTest )
    {
        cbDataLen = DRM_AES_BLOCKLEN;
        ChkDR( Oem_Random_GetBytes( NULL, rgbKey, DRM_AES_KEYSIZE_128 ) );

        ChkMem( pbDataClr = ( DRM_BYTE * )Oem_MemAlloc( cbDataLen ) );
        ChkDR( Oem_Random_GetBytes( NULL, pbDataClr, cbDataLen ) );
        ChkDR( Oem_Random_GetBytes( NULL, rgbIV, DRM_AES_BLOCKLEN ) );
    }
    /*
    ** Known key and cleartest: will compare encrypted text to known value
    */
    else if ( eMode == eDRM_AES_ECB_MODE )
    {
        cbDataLen = g_cbAESECBKnownPlaintext;
        ChkMem( pbDataClr = ( DRM_BYTE * )Oem_MemAlloc( cbDataLen ) );

        MEMCPY( pbDataClr, g_rgbAESECBKnownPlaintext, cbDataLen );

        MEMCPY( rgbKey, g_rgbAESECB128Key, DRM_AES_KEYSIZE_128 );
        pKnownCipher = g_rgbAESECB128KnownCiphertext;
    }
    else if ( eMode == eDRM_AES_CTR_MODE )
    {
        cbDataLen = g_cbAESCtrKnownPlaintext;
        ChkMem( pbDataClr = ( DRM_BYTE * )Oem_MemAlloc( cbDataLen ) );
        MEMCPY( pbDataClr, g_rgbAESCtrKnownPlaintext, cbDataLen );

        MEMCPY( rgbKey, g_rgbAESCtr128Key, DRM_AES_KEYSIZE_128 );
        pKnownCipher = g_rgbAESCtr128KnownCiphertext;
    }
    else /* if ( eMode == eDRM_AES_CBC_MODE ) */
    {
        cbDataLen = g_cbAESCbcKnownPlaintext;
        ChkMem( pbDataClr = ( DRM_BYTE * )Oem_MemAlloc( cbDataLen ) );
        MEMCPY( pbDataClr, g_rgbAESCbcKnownPlaintext, cbDataLen );

        MEMCPY( rgbIV, g_rgbAESCbc128IV, DRM_AES_BLOCKLEN );
        MEMCPY( rgbKey, g_rgbAESCbc128Key, DRM_AES_KEYSIZE_128 );
        pKnownCipher = g_rgbAESCbc128KnownCiphertext;
    }

    ChkMem( pbData = ( DRM_BYTE * )Oem_MemAlloc( cbDataLen ) );
    MEMCPY( pbData, pbDataClr, cbDataLen );
    ChkDR( Oem_Aes_SetKey( rgbKey, &g_aesKey ) );

    /*
    ** Test encrypt
    */
    if ( eMode == eDRM_AES_ECB_MODE )
    {
        ChkDR( Oem_Aes_EcbEncryptData( &g_aesKey, pbData, cbDataLen ) );
    }
    else if ( eMode == eDRM_AES_CTR_MODE )
    {
        ctrContext.qwInitializationVector = qwDataId;
        ctrContext.qwBlockOffset = qwZero;
        ctrContext.bByteOffset   = 0x00;
        ChkDR( Oem_Aes_CtrProcessData( &g_aesKey, pbData, cbDataLen, &ctrContext ) );
    }
    else /* if ( eMode == eDRM_AES_CBC_MODE ) */
    {
        ChkDR( Oem_Aes_CbcEncryptData( &g_aesKey, pbData, cbDataLen, rgbIV ) );
    }

    ChkBOOL( MEMCMP( pbData, pbDataClr, cbDataLen ) != 0
         && ( fRandomTest || MEMCMP( pbData, pKnownCipher, cbDataLen ) == 0 ),
            DRM_E_TEST_ENCRYPT_ERROR );

    /*
    ** Test decrypt
    */
    if ( eMode == eDRM_AES_ECB_MODE )
    {
        ChkDR( Oem_Aes_EcbDecryptData( &g_aesKey, pbData, cbDataLen ) );
    }
    else if ( eMode == eDRM_AES_CTR_MODE )
    {
        ctrContext.qwInitializationVector = qwDataId;
        ctrContext.qwBlockOffset = qwZero;
        ctrContext.bByteOffset   = 0x00;
        ChkDR( Oem_Aes_CtrProcessData( &g_aesKey, pbData, cbDataLen, &ctrContext ) );
    }
    else /* if ( eMode == eDRM_AES_CBC_MODE ) */
    {
        ChkDR( Oem_Aes_CbcDecryptData( &g_aesKey, pbData, cbDataLen, rgbIV ) );
    }
    ChkBOOL( MEMCMP( pbData, pbDataClr, cbDataLen ) == 0, DRM_E_TEST_DECRYPT_ERROR );

ErrorExit:
    SAFE_OEM_FREE( pbData );
    SAFE_OEM_FREE( pbDataClr );

    return dr;
}


/*
** Test_DRM_OMAC
** Tests basic drmaes omac sign/verify functionality with random data
** argv[0] - Data to use ( KNOWN or RANDOM )
** argv[1] - Type of test verification to do ( GOOD or BAD )
*/
DRM_RESULT DRM_CALL Test_DRM_OMAC( long argc, __in_ecount(argc) char **argv )
{
    DRM_RESULT   dr            = DRM_SUCCESS;
    DRM_BOOL     fRandomTest   = TRUE;
    DRM_BYTE    *pbData        = NULL;
    DRM_BYTE     rgbKey[__CB_DECL( DRM_AES_KEYSIZE_128 )] = { 0 };
    DRM_BYTE     rgbSignTag[DRM_AES_BLOCKLEN]             = { 0 };
    DRM_DWORD    cbDataSize    = DRM_AES_BLOCKLEN;

    ChkArg( argc    == 2
         && argv[0] != NULL
         && argv[1] != NULL );

    /*
    ** Initialize the key & data
    */
    if ( strncmp( argv[0], "KNOWN", 5 ) == 0 )
    {
        fRandomTest = FALSE;
        cbDataSize  = SIZEOF_OMACMSG;
    }
    else
    {
        ChkArg( strncmp( argv[0], "RANDOM", 6 ) == 0 );
    }

    ChkMem( pbData  = ( DRM_BYTE * )Oem_MemAlloc( cbDataSize ) );

    if ( fRandomTest )
    {
        ChkDR( Oem_Random_GetBytes( NULL, rgbKey, DRM_AES_KEYSIZE_128 ) );
        ChkDR( Oem_Random_GetBytes( NULL, pbData, cbDataSize ) );
    }
    else
    {
        MEMCPY( rgbKey, g_omacKey, DRM_AES_KEYSIZE_128 );
        MEMCPY( pbData, g_omacMsg, cbDataSize );
    }
    ChkDR( Oem_Aes_SetKey( rgbKey, &g_aesKey ) );

    /*
    ** Test OMAC sign
    */
    ChkDR( Oem_Omac1_Sign( &g_aesKey, pbData, 0, cbDataSize, rgbSignTag ) );

    /*
    ** Check the signature for a known-data test
    */
    ChkBOOL( fRandomTest || MEMCMP( rgbSignTag, g_omacTag, SIZEOF( rgbSignTag ) ) == 0,
             DRM_E_TEST_UNEXPECTED_OUTPUT );

    /*
    ** Alter the data for a bad verify test
    */
    if ( strncmp( argv[1], "BAD", 3 ) == 0  )
    {
        rgbSignTag[0] = ~rgbSignTag[0];
    }

    /*
    ** Test OMAC verify
    */
    ChkDR( Oem_Omac1_Verify( &g_aesKey, pbData, 0, cbDataSize, rgbSignTag, 0 ) );

ErrorExit:
    SAFE_OEM_FREE( pbData );

    return dr;
}

/*
** Test_OEM_RSA_KeySetup
** Sets up a private key and corresponding public key
** argv[0] - key length in bits
*/
DRM_RESULT DRM_CALL Test_OEM_RSA_KeySetup(
    __in const        long   argc,
    __in_ecount(argc) char **argv
    )
{
    DRM_RESULT  dr = DRM_SUCCESS;

    DRM_RSA_SUPPORTED_KEY_LENGTHS eKeyLength = eDRM_RSA_UNKNOWN;
    const DRM_BYTE *pbModulus                = NULL;
    DRM_DWORD       cbModulus                = 0;
    const DRM_BYTE *pbPrivExp                = NULL;
    DRM_DWORD       cbPrivExp                = 0;
    const DRM_BYTE *pbPrimes[ 2 ]            = { NULL, NULL };
    DRM_DWORD       cbPrime                  = 0;

    DRM_RSA_SUPPORTED_KEY_LENGTHS eReturnedKeyLength                    = eDRM_RSA_UNKNOWN;
    DRM_BYTE    rgbReturnedModulus[__CB_DECL( DRM_RSA_CB_MODULUS_MAX )] = { 0 };
    DRM_DWORD   cbReturnedModulus                                       = SIZEOF( rgbReturnedModulus );
    DRM_BYTE    rgbReturnedPubExp[__CB_DECL( DRM_RSA_CB_PUBEXP )]       = { 0 };
    DRM_DWORD   cbReturnedPubExp                                        = SIZEOF( rgbReturnedPubExp );

    ChkArg( ( 1    == argc    )
         && ( NULL != argv[0] ) );

    eKeyLength = ( DRM_RSA_SUPPORTED_KEY_LENGTHS )atol( argv[ 0 ] );

    ChkArg( IS_VALID_DRM_RSA_KEY_LENGTH( eKeyLength ) );

    cbModulus = DRM_RSA_CB_MODULUS( eKeyLength );
    cbPrivExp = DRM_RSA_CB_PRIVEXP( eKeyLength );
    cbPrime   = DRM_RSA_CB_PRIME( eKeyLength );

    switch ( eKeyLength )
    {
    case eDRM_RSA_1024:
        pbModulus     = g_rgbRSAModulus1024;
        pbPrivExp     = g_rgbRSAPrivExp1024;
        pbPrimes[ 0 ] = g_rgbRSAPrimes1024[ 0 ];
        pbPrimes[ 1 ] = g_rgbRSAPrimes1024[ 1 ];
        break;

    case eDRM_RSA_2048:
        pbModulus     = g_rgbRSAModulus2048;
        pbPrivExp     = g_rgbRSAPrivExp2048;
        pbPrimes[ 0 ] = g_rgbRSAPrimes2048[ 0 ];
        pbPrimes[ 1 ] = g_rgbRSAPrimes2048[ 1 ];
        break;
    }

    ChkDR( OEM_RSA_SetPublicKey(  eKeyLength,
                                  SIZEOF( g_rgbRSAPubExp ),
                                  g_rgbRSAPubExp,
                                  cbModulus,
                                  pbModulus,
                                 &g_RsaPubKey ) );

    ChkDR( OEM_RSA_ParsePublicKey( &g_RsaPubKey,
                                   &eReturnedKeyLength,
                                   &cbReturnedPubExp,
                                    rgbReturnedPubExp,
                                   &cbReturnedModulus,
                                    rgbReturnedModulus ) );

    ChkBOOL( eKeyLength == eReturnedKeyLength,
             DRM_E_TEST_UNEXPECTED_OUTPUT );

    ChkBOOL( SIZEOF( g_rgbRSAPubExp ) == cbReturnedPubExp,
             DRM_E_TEST_UNEXPECTED_OUTPUT );

    ChkBOOL( MEMCMP( rgbReturnedPubExp, g_rgbRSAPubExp, cbReturnedPubExp ) == 0,
             DRM_E_TEST_UNEXPECTED_OUTPUT );

    ChkBOOL( cbModulus == cbReturnedModulus,
             DRM_E_TEST_UNEXPECTED_OUTPUT );

    ChkBOOL( DRM_RSA_CB_MODULUS( (DRM_DWORD)eKeyLength ) == cbReturnedModulus,
             DRM_E_TEST_UNEXPECTED_OUTPUT );

    ChkBOOL( MEMCMP( rgbReturnedModulus, pbModulus, cbModulus ) == 0,
             DRM_E_TEST_UNEXPECTED_OUTPUT );

    ChkDR( OEM_RSA_SetPrivateKey(  eKeyLength,
                                   cbPrivExp,
                                   pbPrivExp,
                                   cbPrime,
                                   pbPrimes[ 0 ],
                                   pbPrimes[ 1 ],
                                  &g_RsaPrivKey,
                                  &g_Crypto ) );

ErrorExit:
    return dr;
}

/*
** Test_DRM_RSA_ReleaseKeys
** Free the public and private keys
*/
DRM_RESULT DRM_CALL Test_OEM_RSA_ZeroKeys(
    __in const        long   argc,
    __in_ecount(argc) char **argv
    )
{
    DRM_RESULT  dr    = DRM_SUCCESS;
    DRM_DWORD   i     = 0;
    DRM_BYTE   *pbKey = NULL;

    ChkDR( OEM_RSA_ZeroPublicKey( &g_RsaPubKey ) );

    pbKey = ( DRM_BYTE* )&g_RsaPubKey;
    for ( i = 0; i < SIZEOF( g_RsaPubKey ); i++, pbKey++ )
    {
        if ( 0 != *pbKey )
        {
            ChkDR( DRM_E_TEST_UNEXPECTED_OUTPUT );
        }
    }

    ChkDR( OEM_RSA_ZeroPrivateKey( &g_RsaPrivKey ) );

    pbKey = ( DRM_BYTE* )&g_RsaPrivKey;
    for ( i = 0; i < SIZEOF( g_RsaPrivKey ); i++, pbKey++ )
    {
        if ( 0 != *pbKey )
        {
            ChkDR( DRM_E_TEST_UNEXPECTED_OUTPUT );
        }
    }

ErrorExit:
    return dr;
}

/*
** Test_OEM_RSA_Encrypt
** Tests basic oemrsa encrypt functionality
** argv[0] - key length in bits
** argv[1] - RANDOM or KNOWN1 or KNOWN2 PlainText
*/
DRM_RESULT DRM_CALL Test_OEM_RSA_Encrypt(
    __in const        long   argc,
    __in_ecount(argc) char **argv
    )
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRM_RSA_SUPPORTED_KEY_LENGTHS eKeyLength   = eDRM_RSA_UNKNOWN;
    DRM_DWORD        cbRSAText                 = 0;
    const DRM_BYTE  *pbRSAKnownPlaintext[ 2 ]  = { NULL, NULL };
    const DRM_BYTE  *pbRSAKnownCiphertext[ 2 ] = { NULL, NULL };

    ChkArg( ( 2    == argc    )
         && ( NULL != argv[0] )
         && ( NULL != argv[1] ) );

    eKeyLength = ( DRM_RSA_SUPPORTED_KEY_LENGTHS )atol( argv[ 0 ] );

    ChkArg( IS_VALID_DRM_RSA_KEY_LENGTH( eKeyLength ) );

    cbRSAText = DRM_RSA_CB_MODULUS( eKeyLength );

    switch ( eKeyLength )
    {
    case eDRM_RSA_1024:
        pbRSAKnownPlaintext[ 0 ] = g_rgbRSAKnownPlaintext1024[ 0 ];
        pbRSAKnownPlaintext[ 1 ] = g_rgbRSAKnownPlaintext1024[ 1 ];
        pbRSAKnownCiphertext[ 0 ] = g_rgbRSAKnownCiphertext1024[ 0 ];
        pbRSAKnownCiphertext[ 1 ] = g_rgbRSAKnownCiphertext1024[ 1 ];
        break;

    case eDRM_RSA_2048:
        pbRSAKnownPlaintext[ 0 ] = g_rgbRSAKnownPlaintext2048[ 0 ];
        pbRSAKnownPlaintext[ 1 ] = g_rgbRSAKnownPlaintext2048[ 1 ];
        pbRSAKnownCiphertext[ 0 ] = g_rgbRSAKnownCiphertext2048[ 0 ];
        pbRSAKnownCiphertext[ 1 ] = g_rgbRSAKnownCiphertext2048[ 1 ];
        break;
    }

    if( strncmp( argv[1], "RANDOM", 6 ) == 0 )
    {
        /*
        ** Don't use the full plaintext to make sure it's slightly smaller than the modulus
        */
        ChkDR( Oem_Random_GetBytes(  NULL,
                                    &( g_rgbRSAPlaintext[__CB_DECL( 4 )] ),
                                     DRM_RSA_CB_MODULUS( eKeyLength ) - 4 ) );

        ChkDR( OEM_RSA_Encrypt( &g_RsaPubKey,
                                 DRM_RSA_CB_MODULUS( eKeyLength ),
                                 g_rgbRSAPlaintext,
                                 g_rgbRSACiphertext,
                                &g_Crypto ) );
    }
    else if ( strncmp( argv[1], "KNOWN1", 6 ) == 0 )
    {
        MEMCPY( g_rgbRSAPlaintext, pbRSAKnownPlaintext[ 0 ], cbRSAText );

        ChkDR( OEM_RSA_Encrypt( &g_RsaPubKey,
                                 cbRSAText,
                                 g_rgbRSAPlaintext,
                                 g_rgbRSACiphertext,
                                &g_Crypto ) );

        ChkBOOL( MEMCMP( g_rgbRSACiphertext, pbRSAKnownCiphertext[ 0 ], cbRSAText ) == 0,
                 DRM_E_TEST_ENCRYPT_ERROR );
    }
    else if ( strncmp( argv[1], "KNOWN2", 6 ) == 0 )
    {
        MEMCPY( g_rgbRSAPlaintext, pbRSAKnownPlaintext[ 1 ], cbRSAText );

        ChkDR( OEM_RSA_Encrypt( &g_RsaPubKey,
                                 cbRSAText,
                                 g_rgbRSAPlaintext,
                                 g_rgbRSACiphertext,
                                &g_Crypto ) );

        ChkBOOL( MEMCMP( g_rgbRSACiphertext, pbRSAKnownCiphertext[ 1 ], cbRSAText ) == 0,
                 DRM_E_TEST_ENCRYPT_ERROR );
    }
    else
    {
        ChkArg( FALSE );
    }

ErrorExit:
    return dr;
}

/*
** Test_OEM_RSA_Decrypt
** Tests basic oemrsa decrypt functionality
** argv[0] - key length in bits
*/
DRM_RESULT DRM_CALL Test_OEM_RSA_Decrypt(
    __in const        long   argc,
    __in_ecount(argc) char **argv
    )
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRM_RSA_SUPPORTED_KEY_LENGTHS eKeyLength                     = eDRM_RSA_UNKNOWN;
    DRM_BYTE rgbPlaintext[ __CB_DECL( DRM_RSA_CB_MODULUS_MAX ) ] = { 0 };

    ChkArg( ( 1    == argc    )
         && ( NULL != argv[0] ) );

    eKeyLength = ( DRM_RSA_SUPPORTED_KEY_LENGTHS )atol( argv[ 0 ] );

    ChkArg( IS_VALID_DRM_RSA_KEY_LENGTH( eKeyLength ) );

    ChkArg( DRM_RSA_CB_MODULUS( eKeyLength ) <= SIZEOF( rgbPlaintext ) );

    ChkDR( OEM_RSA_Decrypt( &g_RsaPrivKey,
                             DRM_RSA_CB_MODULUS( eKeyLength ),
                             g_rgbRSACiphertext,
                             rgbPlaintext,
                            &g_Crypto ) );

    ChkBOOL( MEMCMP( rgbPlaintext, g_rgbRSAPlaintext, DRM_RSA_CB_MODULUS( eKeyLength ) ) == 0,
             DRM_E_TEST_DECRYPT_ERROR );

ErrorExit:
    return dr;
}

/*
** Test_DRM_RSA_Encrypt
** Tests basic drmrsa encrypt functionality
** argv[0] - key length in bits
** argv[1] - Byte count of plain text to use
*/
DRM_RESULT DRM_CALL Test_DRM_RSA_Encrypt(
    __in const        long   argc,
    __in_ecount(argc) char **argv
    )
{
    DRM_RESULT  dr           = DRM_SUCCESS;

    DRM_RSA_SUPPORTED_KEY_LENGTHS eKeyLength = eDRM_RSA_UNKNOWN;
    DRM_DWORD cbPlaintext = 0;

    ChkArg( ( 2    == argc    )
         && ( NULL != argv[0] )
         && ( NULL != argv[1] ) );

    eKeyLength = ( DRM_RSA_SUPPORTED_KEY_LENGTHS )atol( argv[ 0 ] );
    cbPlaintext = atol( argv[ 1 ] );

    ChkArg( IS_VALID_DRM_RSA_KEY_LENGTH( eKeyLength ) );
    ChkArg( cbPlaintext <= DRM_RSA_CB_MAX_PLAINTEXT( (DRM_DWORD)eKeyLength ) );

    /*
    ** Set up the plaintext and encrypt
    */

    ChkDR( Oem_Random_GetBytes( NULL, g_rgbRSAPlaintext, cbPlaintext ) );

    g_cbRsaCiphertext = SIZEOF( g_rgbRSACiphertext );

    ChkDR( OEM_RSA_OaepEncrypt(  NULL,
                                &g_RsaPubKey,
                                 cbPlaintext,
                                 g_rgbRSAPlaintext,
                                &g_cbRsaCiphertext,
                                 g_rgbRSACiphertext,
                                &g_Crypto ) );

ErrorExit:
    return dr;
}

/*
** Test_DRM_RSA_Decrypt
** Tests basic drmrsa decrypt functionality
** argv[0] - key length in bits
** argv[1] - Byte count of plain text used to encrypt
*/
DRM_RESULT DRM_CALL Test_DRM_RSA_Decrypt(
    __in const        long   argc,
    __in_ecount(argc) char **argv
    )
{
    DRM_RESULT  dr           = DRM_SUCCESS;

    DRM_RSA_SUPPORTED_KEY_LENGTHS eKeyLength = eDRM_RSA_UNKNOWN;
    DRM_DWORD   cbPlaintext  = 0;
    DRM_DWORD   cbPlaintext2 = 0;
    DRM_DWORD   cbCiphertext = 0;
    DRM_BYTE   *pbCiphertext = NULL;
    DRM_BYTE    rgbPlaintext[DRM_RSA_CB_MAX_PLAINTEXT_MAX] = { 0 };

    ChkArg( ( 2    == argc    )
         && ( NULL != argv[0] )
         && ( NULL != argv[1] ) );

    eKeyLength = ( DRM_RSA_SUPPORTED_KEY_LENGTHS )atol( argv[ 0 ] );
    cbPlaintext = atol( argv[ 1 ] );

    ChkArg( IS_VALID_DRM_RSA_KEY_LENGTH( eKeyLength ) );
    ChkArg( cbPlaintext <= DRM_RSA_CB_MAX_PLAINTEXT( (DRM_DWORD)eKeyLength ) );

    cbCiphertext = g_cbRsaCiphertext;
    pbCiphertext = g_rgbRSACiphertext;

    ChkArg( cbCiphertext <= DRM_RSA_CB_MODULUS_MAX );

    /*
    ** Decrypt the plaintext and compare buffers
    */
    cbPlaintext2 = SIZEOF( rgbPlaintext );
    ChkDR( OEM_RSA_OaepDecrypt( &g_RsaPrivKey,
                                 cbCiphertext,
                                 pbCiphertext,
                                &cbPlaintext2,
                                 rgbPlaintext,
                                &g_Crypto ) );

    ChkBOOL( cbPlaintext == cbPlaintext2, DRM_E_TEST_DECRYPT_ERROR );
    ChkBOOL( MEMCMP( rgbPlaintext, g_rgbRSAPlaintext, cbPlaintext ) == 0, DRM_E_TEST_DECRYPT_ERROR );

ErrorExit:
    return dr;
}

/*
** Test_DRM_RSA_Sign
** Tests basic drmrsa sign functionality
** argv[0] - key length in bits
** argv[1] - Byte count of plain text to sign
*/
DRM_RESULT DRM_CALL Test_DRM_RSA_Sign(
    __in const        long   argc,
    __in_ecount(argc) char **argv
    )
{
    DRM_RESULT    dr           = DRM_SUCCESS;

    DRM_RSA_SUPPORTED_KEY_LENGTHS eKeyLength = eDRM_RSA_UNKNOWN;
    DRM_DWORD     cbPlaintext  = 0;

    ChkArg( ( 2    == argc    )
         && ( NULL != argv[0] )
         && ( NULL != argv[1] ) );

    eKeyLength = ( DRM_RSA_SUPPORTED_KEY_LENGTHS )atol( argv[ 0 ] );
    cbPlaintext = atol( argv[ 1 ] );

    ChkArg( IS_VALID_DRM_RSA_KEY_LENGTH( eKeyLength ) );
    ChkArg( cbPlaintext <= DRM_RSA_CB_MAX_PLAINTEXT( (DRM_DWORD)eKeyLength ) );

    g_cbRsaCiphertext = SIZEOF( g_rgbRSACiphertext );

    /*
    ** Set up the random plaintext and sign
    */
    ChkDR( Oem_Random_GetBytes( NULL, g_rgbRSAPlaintext, cbPlaintext ) );

    ChkDR( OEM_RSA_PssSign( &g_RsaPrivKey,
                             cbPlaintext,
                             g_rgbRSAPlaintext,
                             0,
                            &g_cbRsaCiphertext,
                             g_rgbRSACiphertext,
                            &g_Crypto ) );

ErrorExit:
    return dr;
}

/*
** Test_DRM_RSA_Verify
** Tests basic drmrsa verify functionality
** argv[0] - key length in bits
** argv[1] - Byte count of plain text signed
** argv[2] - Type of test (BAD/GOOD)
*/
DRM_RESULT DRM_CALL Test_DRM_RSA_Verify(
    __in const        long   argc,
    __in_ecount(argc) char **argv
    )
{
    DRM_RESULT    dr           = DRM_SUCCESS;

    DRM_RSA_SUPPORTED_KEY_LENGTHS eKeyLength = eDRM_RSA_UNKNOWN;
    DRM_DWORD     cbPlaintext  = 0;

    ChkArg( ( 3    == argc    )
         && ( NULL != argv[0] )
         && ( NULL != argv[1] )
         && ( NULL != argv[2] ) );

    eKeyLength = ( DRM_RSA_SUPPORTED_KEY_LENGTHS )atol( argv[ 0 ] );
    cbPlaintext = atol( argv[ 1 ] );

    ChkArg( IS_VALID_DRM_RSA_KEY_LENGTH( eKeyLength ) );
    ChkArg( cbPlaintext <= DRM_RSA_CB_MAX_PLAINTEXT( (DRM_DWORD)eKeyLength ) );

    /*
    ** Alter the data for a bad verify test
    */
    if ( strncmp( argv[2], "BAD", 3 ) == 0  )
    {
        g_rgbRSACiphertext[cbPlaintext - 1] = ~g_rgbRSACiphertext[cbPlaintext - 1];
    }
    /*
    ** Verify the signature
    */
    ChkDR( OEM_RSA_PssVerify( &g_RsaPubKey,
                               cbPlaintext,
                               g_rgbRSAPlaintext,
                               0,
                               g_cbRsaCiphertext,
                               g_rgbRSACiphertext,
                              &g_Crypto ) );

ErrorExit:
    return dr;
}

/*
** Test_DRM_AES_GenerateHMACKey
** Tests basic hmac generation sign functionality. Also use to create a valid plaintext
**      point for Encrypt
** argv[0] - index to global pointer to generate an HMAC/CK keypair to .
*/
DRM_RESULT DRM_CALL Test_DRM_ECC_GenerateHMACKey( long argc, __in_ecount(argc) char **argv )
{
    /*
    ** Decls
    */
    DRM_RESULT      dr      = DRM_SUCCESS;
    PLAINTEXT_P256  keyCopy = {0};
    PLAINTEXT_P256 *pKey    = NULL;

    /*
    ** Arg Checks
    */
    ChkArg( argc    == 1 );
    MAP_TO_GLOBAL_TEST_VAR( pKey, argv[0] );/* should be initially of the form [0^16][{0123456789abcdef}^16] bytes */

    /*
    ** Make a copy of the input
    */
    if( pKey != NULL )
    {
        MEMCPY( keyCopy.m_rgbPlaintext, pKey->m_rgbPlaintext, SIZEOF( PLAINTEXT_P256 ) );
    }

    /*
    ** Do the Generation
    */
    ChkDR( OEM_ECC_GenerateHMACKey_P256( pKey, (struct bigctx_t *)&g_Crypto.rgbCryptoContext ) );

    /*
    ** Make sure the the first half has changed and the 2nd half has not!
    */
    if( pKey != NULL )
    {
        ChkBOOL( 0 != MEMCMP( keyCopy.m_rgbPlaintext, pKey->m_rgbPlaintext, SIZEOF( PLAINTEXT_P256 ) / 2 )
            && ( 0 == MEMCMP( &keyCopy.m_rgbPlaintext[ SIZEOF( PLAINTEXT_P256 ) / 2 ],
                              &pKey->m_rgbPlaintext[ SIZEOF( PLAINTEXT_P256 ) / 2 ],
                               SIZEOF( PLAINTEXT_P256 ) / 2 )),
            DRM_E_P256_PLAINTEXT_MAPPING_FAILURE );
    }

ErrorExit:
    return dr;
} /* end Test_DRM_AES_GenerateHMACKey */


/*
** Test_DRM_ECC_Decrypt_P256
** Tests basic ECC P256 (El-Gamal) Decryption
** argv[0] - Private key to decrypt with
** argv[1] - Ciphertext block to be decrypted
** argv[2] - Output plaintext. Allocates mem to this.
*/
DRM_RESULT DRM_CALL Test_DRM_ECC_Decrypt_P256( long argc, __in_ecount(argc) char **argv )
{
    /*
    ** Decls
    */
    DRM_RESULT       dr          = DRM_SUCCESS;
    PRIVKEY_P256    *pPrivkey    = NULL;
    PLAINTEXT_P256  *pPlaintext  = NULL;
    CIPHERTEXT_P256 *pCiphertext = NULL;

    /*
    ** Arg Checks
    */
    ChkArg( argc    == 3 );

    MAP_TO_GLOBAL_TEST_VAR( pPrivkey, argv[0] );
    MAP_TO_GLOBAL_TEST_VAR( pCiphertext, argv[1] );
    MAP_TO_GLOBAL_TEST_VAR_Alloc( pPlaintext, argv[2], SIZEOF( PLAINTEXT_P256 ) );

    /*
    ** Do the decryption
    */
    ChkDR( OEM_ECC_Decrypt_P256( ( const PRIVKEY_P256 * ) pPrivkey,
                                 ( const CIPHERTEXT_P256 *) pCiphertext,
                                 pPlaintext,
                                 (struct bigctx_t *)g_Crypto.rgbCryptoContext ) );

ErrorExit:
    return dr;
} /* end Test_DRM_ECC_Decrypt_P256 */

/*
** Test_DRM_ECC_Encrypt_P256
** Tests basic ECC P256 (El-Gamal) encryption
** argv[0] - Public key to encrypt with
** argv[1] - Plaintext block to be encrypted
** argv[2] - Output ciphertext. Allocates mem to this.
*/
DRM_RESULT DRM_CALL Test_DRM_ECC_Encrypt_P256( long argc, __in_ecount(argc) char **argv )
{
    /*
    ** Decls
    */
    DRM_RESULT          dr                 = DRM_SUCCESS;
    PUBKEY_P256        *pPubkey      = NULL;
    PLAINTEXT_P256     *pPlaintext   = NULL;
    CIPHERTEXT_P256    *pCiphertext  = NULL;
    DRM_CRYPTO_CONTEXT *pcryptoCtx   = NULL;

    /*
    ** Arg Checks
    */
    ChkArg( argc    == 3 );

    ChkMem( pcryptoCtx = (DRM_CRYPTO_CONTEXT*)Oem_MemAlloc( SIZEOF( DRM_CRYPTO_CONTEXT ) ) );
    ZEROMEM( pcryptoCtx, SIZEOF( DRM_CRYPTO_CONTEXT ) );

    MAP_TO_GLOBAL_TEST_VAR( pPubkey, argv[0] );
    MAP_TO_GLOBAL_TEST_VAR( pPlaintext, argv[1] );
    MAP_TO_GLOBAL_TEST_VAR_Alloc( pCiphertext, argv[2], SIZEOF( CIPHERTEXT_P256 ) );

    /*
    ** Do the encryption
    */
    ChkDR( OEM_ECC_Encrypt_P256(pPubkey,
                                pPlaintext,
                                pCiphertext,
                                (struct bigctx_t *)pcryptoCtx->rgbCryptoContext ) );
ErrorExit:
    SAFE_OEM_FREE( pcryptoCtx );
    return dr;
} /* end Test_DRM_ECC_Encrypt_P256 */

/*
** Test_DRM_ECC_GenKeyPair_P256
** Tests private-public key pair generation for ECC P256
** argv[0] - Output public key. Allocates mem to this.
** argv[1] - Output private key. Allocates mem to this.
*/
DRM_RESULT DRM_CALL Test_DRM_ECC_GenKeyPair_P256( long argc, __in_ecount(argc) char **argv )
{
    /*
    ** Decls
    */
    DRM_RESULT dr          = DRM_SUCCESS;
    PRIVKEY_P256 *pPrivkey = NULL;
    PUBKEY_P256  *pPubkey  = NULL;

    /*
    ** Arg Checks
    */
    ChkArg( argc      == 2 );

    MAP_TO_GLOBAL_TEST_VAR_Alloc( pPubkey, argv[0], SIZEOF( PUBKEY_P256 ) );
    MAP_TO_GLOBAL_TEST_VAR_Alloc( pPrivkey, argv[1], SIZEOF( PRIVKEY_P256 ) );

    /*
    ** Do the signing
    */
    ChkDR(OEM_ECC_GenKeyPair_P256( pPubkey,
                                   pPrivkey,
                                   (struct bigctx_t *)&g_Crypto.rgbCryptoContext ) );

ErrorExit:
    return dr;
} /* end Test_DRM_ECC_GenKeyPair_P256 */

/*
** Test_DRM_ECC_GenKeyPairRestrictedPriv_P256
** Tests private-public key pair generation for ECC P256
** argv[0] - Output public key. Allocates mem to this.
** argv[1] - Output private key. Allocates mem to this.
*/
DRM_RESULT DRM_CALL Test_DRM_ECC_GenKeyPairRestrictedPriv_P256( long argc, __in_ecount(argc) char **argv )
{
    /*
    ** Decls
    */
    DRM_RESULT dr           = DRM_SUCCESS;
    PRIVKEY_P256 *pPrivkey  = NULL;
    PUBKEY_P256  *pPubkey   = NULL;
    DRM_DWORD     cbPubKey  = SIZEOF( PUBKEY_P256 );
    DRM_DWORD     cbPrivKey = SIZEOF( PRIVKEY_P256 );

    /*
    ** Arg Checks
    */
    ChkArg( argc      == 2 );

    MAP_TO_GLOBAL_TEST_VAR_Alloc( pPubkey, argv[0], cbPubKey );
    MAP_TO_GLOBAL_TEST_VAR_Alloc( pPrivkey, argv[1], cbPrivKey );

    /*
    ** Generate the key pair
    */
    ChkDR( OEM_ECC_GenKeyPairRestrictedPriv_P256( pPubkey, pPrivkey, (struct bigctx_t *)&g_Crypto.rgbCryptoContext ) );

ErrorExit:
    return dr;
} /* end Test_DRM_ECC_GenKeyPairRestrictedPriv_P256 */

/*
** Test_DRM_ECDSA_Sign_P256
** Tests basic ECDSA signing
** argv[0] - Message to be signed
** argv[1] - Message len in bytes
** argv[2] - Private key to sign the message with
** argv[4] - The output of the signing operation - this is the signature. Allocates mem to this.
*/
DRM_RESULT DRM_CALL Test_DRM_ECDSA_Sign_P256( long argc, __in_ecount(argc) char **argv )
{
    /*
    ** Decls
    */
    DRM_RESULT      dr        = DRM_SUCCESS;
    DRM_DWORD       cbMessage = 0;
    PRIVKEY_P256   *pPrivkey  = NULL;
    DRM_BYTE       *pbMessage = NULL;
    SIGNATURE_P256 *pSig      = NULL;


    /*
    ** Chk Args
    */
    ChkArg( argc      == 4 );
    ChkArg( argv[ 1 ] != NULL ); /* should be the message len in bytes */

    MAP_TO_GLOBAL_TEST_VAR( pPrivkey, argv[2] );
    MAP_TO_GLOBAL_TEST_VAR( pbMessage, argv[0] );
    MAP_TO_GLOBAL_TEST_VAR_Alloc( pSig, argv[3], SIZEOF( SIGNATURE_P256 ) );

    /*
    ** Get the len of the message to verify
    */
    cbMessage = atol( argv[ 1 ] );

    /*
    ** Do the signing
    */
    ChkDR( OEM_ECDSA_Sign_P256( ( const DRM_BYTE * )pbMessage,
                                  cbMessage,
                                  pPrivkey,
                                  pSig,
                                  (struct bigctx_t *)g_Crypto.rgbCryptoContext ) );

ErrorExit:
    return dr;
} /* end Test_DRM_ECDSA_Sign_P256 */

/*
** Test_DRM_ECDSA_Verify_P256
** Tests basic ECDSA P256 Verification
** argv[0] - Message to verify
** argv[1] - Message len in bytes
** argv[2] - Public key struct to use in verification
** argv[3] - Signature for the message that is to be verified
*/
DRM_RESULT DRM_CALL Test_DRM_ECDSA_Verify_P256( long argc, __in_ecount(argc) char **argv )
{
    /*
    ** Decls
    */
    DRM_RESULT dr        = DRM_SUCCESS;
    DRM_DWORD  cbMessage = 0;
    PUBKEY_P256     *pPubkey   = NULL;
    DRM_BYTE        *pbMessage = NULL;
    SIGNATURE_P256  *pSig      = NULL;

    /*
    ** Chk Args
    */
    ChkArg( argc    == 4 );
    ChkArg( argv[1] != NULL ); /* should be the message len in bytes */

    MAP_TO_GLOBAL_TEST_VAR( pbMessage, argv[0] );
    MAP_TO_GLOBAL_TEST_VAR( pPubkey, argv[2] );
    MAP_TO_GLOBAL_TEST_VAR( pSig, argv[3] );

    /*
    ** Get the len of the message to verify
    */
    cbMessage = atol( argv[ 1 ] );

    /*
    ** Do the verification
    */
    ChkDR( OEM_ECDSA_Verify_P256( ( const DRM_BYTE * )pbMessage,
                                  cbMessage,
                                  pPubkey,
                                  pSig,
                                  (struct bigctx_t *)g_Crypto.rgbCryptoContext) );


ErrorExit:
    return dr;
} /* end Test_DRM_ECDSA_Verify_P256 */


DRM_RESULT DRM_CALL MODULE_PRETESTCASE(Crypto)( long lTCID, const char *strTCName )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkMem( g_pbRandBufIn = ( DRM_BYTE * )Oem_MemAlloc( DRM_ECC160_PLAINTEXT_LEN ) );
    MEMSET( g_pbRandBufIn, 0, DRM_ECC160_PLAINTEXT_LEN  );

    ChkMem( g_pbCipher = ( DRM_BYTE * )Oem_MemAlloc( DRM_ECC160_CIPHERTEXT_LEN ) );
    MEMSET( g_pbCipher, 0, DRM_ECC160_CIPHERTEXT_LEN  );

    ChkMem( g_pbDecryptBufOut = ( DRM_BYTE * )Oem_MemAlloc( DRM_ECC160_PLAINTEXT_LEN ) );
    MEMSET( g_pbDecryptBufOut, 0, DRM_ECC160_PLAINTEXT_LEN  );

    g_pszKey = g_pszPlainKey;
    ChkMem( g_pbCryptoContext = ( DRM_BYTE * )Oem_MemAlloc( DRM_PKCRYPTO_CONTEXT_BUFFER_SIZE ) );
    MEMSET( g_pbCryptoContext, 0, DRM_PKCRYPTO_CONTEXT_BUFFER_SIZE );

    ChkMem( g_pPubKey = ( PUBKEY * )Oem_MemAlloc( DRM_ECC160_PUBLIC_KEY_LEN ) );
    MEMSET( g_pPubKey, 0, DRM_ECC160_PUBLIC_KEY_LEN );

    ChkMem( g_pPrivKey = ( PRIVKEY * )Oem_MemAlloc( DRM_ECC160_PRIVATE_KEY_LEN ) );
    MEMSET( g_pPrivKey, 0, DRM_ECC160_PRIVATE_KEY_LEN );

    ChkMem( g_pbSymmetricEncryptedData = ( DRM_BYTE * )Oem_MemAlloc( MAX_CB_SYMMETRIC_BUFFER) );
    ChkMem( g_pbSymmetricClearData = ( DRM_BYTE * )Oem_MemAlloc( MAX_CB_SYMMETRIC_BUFFER ) );

    MEMSET( &g_mackey, 0, SIZEOF( DRM_CBCKey ) );
    MEMSET( &g_invmackey, 0, SIZEOF( DRM_CBCKey ) );
    MEMSET( &g_desTable, 0, SIZEOF( DESTable ) );
    MEMSET( &g_AesCtrContext, 0, SIZEOF( DRM_AES_COUNTER_MODE_CONTEXT ) );

    MEMSET( g_rgbRSAPlaintext, 0, SIZEOF( g_rgbRSAPlaintext ) );
    MEMSET( g_rgbRSACiphertext, 0, SIZEOF( g_rgbRSACiphertext ) );

    MEMSET( &g_Context, 0, SIZEOF( DRM_CIPHER_CONTEXT ) );

ErrorExit:
    return dr;
}

DRM_RESULT DRM_CALL MODULE_POSTTESTCASE(Crypto)( long lTCID, const char *strTCName )
{

    SAFE_OEM_FREE( g_pbRandBufIn );
    SAFE_OEM_FREE( g_pbCipher );
    SAFE_OEM_FREE( g_pbDecryptBufOut );

    MEMSET( &g_Crypto, 0, SIZEOF( DRM_CRYPTO_CONTEXT ) );

    SAFE_OEM_FREE( g_pbCryptoContext );
    SAFE_OEM_FREE( g_pPubKey );
    SAFE_OEM_FREE( g_pPrivKey );
    SAFE_OEM_FREE( g_pbEncDataCBC );
    SAFE_OEM_FREE( g_pbOrgDataCBC );
    SAFE_OEM_FREE( g_pbSymmetricEncryptedData );
    SAFE_OEM_FREE( g_pbSymmetricClearData );

    return DRM_SUCCESS;
}

BEGIN_TEST_API_LIST( Crypto )
    API_ENTRY( Test_ReallocBuffer )
    API_ENTRY( Test_PK_GenKeyPair_Encrypt_Decrypt )
    API_ENTRY( Test_PK_Sign_Verify )
    API_ENTRY( Test_DRM_CPHR_Init )
    API_ENTRY( Test_DRM_CPHR_InitDecrypt )
    API_ENTRY( Test_DRM_CPHR_Encrypt )
    API_ENTRY( Test_DRM_CPHR_Decrypt )
    API_ENTRY( Test_DRM_SHA_Init )
    API_ENTRY( Test_DRM_SHA_Update )
    API_ENTRY( Test_DRM_SHA_Finalize )
    API_ENTRY( Test_DRM_HMAC_Init )
    API_ENTRY( Test_DRM_HMAC_Update )
    API_ENTRY( Test_DRM_HMAC_Finalize )
    API_ENTRY( Test_RC4_Cipher )
    API_ENTRY( Test_DES_Cipher )
    API_ENTRY( Test_CBC64WS4_asm )
    API_ENTRY( Test_InvCBC64WS4_asm )
    API_ENTRY( Test_OEM_AES_Cipher )
    API_ENTRY( Test_DRM_AES_Cipher )
    API_ENTRY( Test_DRM_OMAC )
    API_ENTRY( Test_Oem_Random_GetBytes )
    API_ENTRY( Test_MEMCMP )
    API_ENTRY( Test_OEM_RSA_ZeroKeys )
    API_ENTRY( Test_OEM_RSA_KeySetup )
    API_ENTRY( Test_OEM_RSA_Encrypt )
    API_ENTRY( Test_OEM_RSA_Decrypt )
    API_ENTRY( Test_DRM_RSA_Encrypt )
    API_ENTRY( Test_DRM_RSA_Decrypt )
    API_ENTRY( Test_DRM_RSA_Sign )
    API_ENTRY( Test_DRM_RSA_Verify )
    API_ENTRY( LoadSymmetricTestData )
    API_ENTRY( Test_DRM_SHA256_Init )
    API_ENTRY( Test_DRM_SHA256_Update )
    API_ENTRY( Test_DRM_SHA256_UpdateOffset )
    API_ENTRY( Test_DRM_SHA256_Finalize )
    API_ENTRY( Test_DRM_SHA256_E2E )
    API_ENTRY( Test_DRM_SHA256_E2EOffset )
    API_ENTRY( Test_DRM_ECC_GenerateHMACKey )
    API_ENTRY( Test_DRM_ECC_Decrypt_P256 )
    API_ENTRY( Test_DRM_ECC_Encrypt_P256 )
    API_ENTRY( Test_DRM_ECC_GenKeyPair_P256 )
    API_ENTRY( Test_DRM_ECDSA_Sign_P256 )
    API_ENTRY( Test_DRM_ECDSA_Verify_P256 )
    API_ENTRY( Test_DRM_ECC_GenKeyPairRestrictedPriv_P256 )
    API_ENTRY( Test_DRM_PK_Decrypt )
    API_ENTRY( Test_DRM_PK_Encrypt )
    API_ENTRY( Test_DRM_PK_GenKeyPair )
    API_ENTRY( Test_DRM_PK_Sign )
    API_ENTRY( Test_DRM_PK_Verify )
#ifdef ALIGNMENT_TESTING_ENABLED
    API_ENTRY( Test_DRM_PK_Encrypt_P160_Alignment )
    API_ENTRY( Test_DRM_PK_Decrypt_P160_Alignment )
    API_ENTRY( Test_PK_Sign_Verify_Alignment )
#endif
END_TEST_API_LIST

EXIT_PKTEST_NAMESPACE_CODE

