/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/


#include "stdafx.h"
#include "oemcipher.h"
#include "drmblackbox.h"
#include "drmlicense.h"
#include "blackboxtest.h"
#include "drmkeyfile.h"
#include <drmwmdrm.h>

ENTER_PKTEST_NAMESPACE_CODE

DRM_BB_CONTEXT g_BBContext;
CLIENTID g_clientID;

/* #define MAX_GLOBAL_BINDING_BLOB 2 */
DRM_BINDING_INFO g_binding[MAX_GLOBAL_BINDING_BLOB];

#define MAX_BLOB 5
static DRM_BYTE *g_blob[MAX_BLOB];

#define MAX_CIPHER 1
DRM_CIPHER_CONTEXT g_cipherContext[MAX_CIPHER];

static void CleanKeyfileAndDevStore()
{
    #define KEYFILE_NAME "v2ks.bla"
    #define SECFILE_NAME "v2ks.sec"

    /*    Delete key file and secret file    */
    ( void ) remove(KEYFILE_NAME);
    ( void ) remove(SECFILE_NAME);

    /* delete the secure store and keyfile*/
    RemoveDRMFile(RMFILE_STORE);
    RemoveDRMFile(RMFILE_KEYFILE);
}

DRM_RESULT DRM_CALL BBXInitWithCerts( DRM_BB_CONTEXT *pcontextBBX )
{
    DRM_RESULT dr                                  = DRM_SUCCESS;
    DRM_BYTE   rgbBuffer[MAX_DEVICE_CERT_SIZE]     = { 0 };
    DRM_BYTE  *pbCertChain                         = NULL;
    DRM_DWORD  dwBCertChainSize                    = DRM_MAX_LICENSESIZE;
    DRM_CONST_STRING dstrDevCert;

    DSTR_FROM_PB( &dstrDevCert, rgbBuffer, SIZEOF( rgbBuffer ) );
    ChkMem( pbCertChain = ( DRM_BYTE * ) Oem_MemAlloc(  dwBCertChainSize ) );

    /*blackbox initialized */
    if( DRM_WMDRM_IsWMDRMSupported() )
    {
        ChkDR( TST_UTL_SetWMDRMCertCachedValues( pcontextBBX ) );
    }

    ChkDR( TST_UTL_SetPlayReadyCertCachedValues( pcontextBBX ) );


ErrorExit:
    return dr;
}

DRM_RESULT DRM_CALL MODULE_PRETESTCASE(BlackBox)(long lTCID, const char *strTCName)
{
    int i;
    DRM_RESULT dr;
    DRM_KEYFILE_CONTEXT *poKeyfileContext = NULL;

    memset(g_binding, 0, sizeof(DRM_BINDING_INFO) * MAX_GLOBAL_BINDING_BLOB);

    /* Init random number blobs */
    for (i = 0; i < MAX_BLOB; i++)
        g_blob[i] = NULL;

    CleanKeyfileAndDevStore();
    ChkDR(SetDeviceEnv(DEVCERT_TEMPLATE_FILE_NAME,
                       PRIVATE_KEY_FILE_NAME,
                       GROUP_CERT_FILE_NAME,
                       GC_PRIVATE_KEY_FILE_NAME,
                       TRUE));

    /*
    ** Get a devcert
    */
    ChkDR(BBXInitWithCerts(&g_BBContext));

    /*
    ** Set pointer to keyfile context in the blackbox
    */
    ChkMem( poKeyfileContext = (DRM_KEYFILE_CONTEXT*) Oem_MemAlloc( SIZEOF(DRM_KEYFILE_CONTEXT) ) );

    ChkDR( DRM_KF_Initialize( NULL, poKeyfileContext ) );
    g_BBContext.pKeyFileContext = poKeyfileContext;
    ChkDR( DRM_KF_Open( poKeyfileContext, TRUE, OEM_GENERIC_READ ) );
    ChkDR( DRM_KF_LoadFromFile( poKeyfileContext, TRUE ) );
    ChkDR( DRM_BBX_SetupSecureStoreKey( poKeyfileContext, NULL ) );

    ChkDR(DRM_BBX_Initialize(NULL, &g_BBContext));
ErrorExit:
    return dr;
}

DRM_RESULT DRM_CALL MODULE_POSTTESTCASE(BlackBox)(long lTCID, const char *strTCName)
{
    int i;

    for (i = 0; i < MAX_BLOB; i++) {
        SAFE_OEM_FREE(g_blob[i]);
        g_blob[i] = NULL;
    }
    if ( g_BBContext.pKeyFileContext != NULL )
    {
        if ( g_BBContext.pKeyFileContext->fLoaded )
        {
            DRM_KF_Close( g_BBContext.pKeyFileContext );
        }
    }
    SAFE_OEM_FREE( g_BBContext.pKeyFileContext );

    return DRM_BBX_Shutdown(&g_BBContext);
}

DRM_RESULT DRM_CALL Test_CreateBindingInfo(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL Test_API_CanBind(long argc, __in_ecount(argc) char **argv);

/*
    Test API DRM_BBX_GetClientId
    argv[0]: client id pointer: NULL, NORMAL
    argv[1]: blackbox context: NULL, NORMAL
*/
DRM_RESULT DRM_CALL Test_API_GetClientID(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT dr;
    DRM_BB_CONTEXT *pBBContext;
    CLIENTID *pClientID;

    ChkArg(argc == 2);
    pClientID = argv[0]? &g_clientID: NULL;
    pBBContext = argv[1]? &g_BBContext: NULL;

    ChkDR(DRM_BBX_GetClientId(pClientID, pBBContext));
ErrorExit:
    return dr;
}

/* Verify the global clientid is correctly encrypted */
DRM_RESULT DRM_CALL Test_VerifyClientID(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT dr;
    CLIENTID *pClientID = &g_clientID;

    /* Verify the version is correct */
    if (memcmp(pClientID->version, CLIENT_ID_VER, VERSION_LEN))
        ChkDR(DRM_E_FAIL);

    /* Decrypt the client id and verify its cert */
    ChkDR(tDecryptClientID(g_BBContext.CryptoContext.rgbCryptoContext, pClientID));
    if (memcmp(&pClientID->pk, &g_BBContext.cachedCertValues.m_BBCompatibilityCert, sizeof(PKCERT)))
        dr = DRM_S_FALSE;
ErrorExit:
    return dr;
}




/* Generate random blob for other test functions
    argv[0]: index to the global random blob to hold the generated number
    argv[1]: the length of the random bytes.
*/
DRM_RESULT DRM_CALL Test_GenRandomBlob(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT dr;
    int iRand;
    DRM_DWORD dwLen;

    ChkArg(argc == 2 && argv[0] && argv[1]);

    iRand = atoi(argv[0]);
    ChkArg(iRand >= 0 && iRand < MAX_BLOB);

    dwLen = atol(argv[1]);

    SAFE_OEM_FREE(g_blob[iRand]);
    ChkMem(g_blob[iRand] = (DRM_BYTE*)Oem_MemAlloc(dwLen * sizeof(DRM_BYTE)));

    ChkDR(Oem_Random_GetBytes(NULL, g_blob[iRand], dwLen));
ErrorExit:
    return dr;
}

/* Test API function DRM_BBX_HashValue
    argv[0]: index to the global random blob to hold the data to be hashed.
    argv[1]: the length of the input data.
    argv[2]: index to the global memory to save the hash value
    argv[3]: blackbox context: NULL, NORMAL
*/
DRM_RESULT DRM_CALL Test_API_HashValue(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT dr;
    int iRand, iHash;
    DRM_DWORD dwLen;
    DRM_BB_CONTEXT *pBBContext;

    ChkArg(argc == 4 && argv[0] && argv[1] && argv[2]);

    iRand = atoi(argv[0]);
    dwLen = atol(argv[1]);
    iHash = atoi(argv[2]);
    pBBContext = argv[3]? &g_BBContext: NULL;

    ChkArg(iRand >= 0 && iRand < MAX_BLOB && iHash >= 0 && iHash < MAX_BLOB && iRand != iHash);

    SAFE_OEM_FREE(g_blob[iHash]);
    ChkMem(g_blob[iHash] = (DRM_BYTE*)Oem_MemAlloc(DRM_SHA1_DIGEST_LEN));
    ChkDR(DRM_BBX_HashValue(g_blob[iRand], dwLen, g_blob[iHash], pBBContext));
ErrorExit:
    return dr;
}

/* Perform memory operations on two blobs
    argv[0]: do what: memcmp, memcpy
    argv[1]: index to the first blob
    argv[2]: index to the second blob
    argv[3]: blob size
*/
DRM_RESULT DRM_CALL Test_BlobOps(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT dr = DRM_SUCCESS;
    int i1, i2;
    DRM_DWORD dwLen;

    ChkArg(argc == 4 && argv[0] && argv[1] && argv[2] && argv[3]);

    i1 = atoi(argv[1]);
    i2 = atoi(argv[2]);
    dwLen = atol(argv[3]);

    ChkArg(i1 >= 0 && i1 < MAX_BLOB && i2 >= 0 && i2 < MAX_BLOB);

    if (!strcmp(argv[0], "memcmp")) {
        if (memcmp(g_blob[i1], g_blob[i2], dwLen))
            dr = DRM_S_FALSE;
    } else if (!strcmp(argv[0], "memcpy"))
        memcpy(g_blob[i1], g_blob[i2], dwLen);
    else
        dr = DRM_E_INVALIDARG;

ErrorExit:
    return dr;
}

/* Encrypt a random blob with the blackbox public key.
    argv[0]: index to the random block to encrypt
    argv[1]: the size of the random block encrypt
    argv[2]: index to the output encrypted random block.
    argv[3]: symmetric key size in bytes
*/
DRM_RESULT DRM_CALL Test_PKEncryptLarge(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT dr;
    int iIn, iOut;
    DRM_DWORD cbIn, dwSymKeySize;

    ChkArg(argc == 4);

    iIn = atoi(argv[0]);
    cbIn = atol(argv[1]);
    iOut = atoi(argv[2]);
    dwSymKeySize = atol(argv[3]);

    ChkArg(iIn != iOut && iIn >= 0 && iIn < MAX_BLOB && iOut >= 0 && iOut < MAX_BLOB);

    SAFE_OEM_FREE(g_blob[iOut]);
    ChkMem(g_blob[iOut] = (DRM_BYTE*)Oem_MemAlloc(DRM_ECC160_CIPHERTEXT_LEN + cbIn));

    ChkDR(DRM_PK_EncryptLarge( NULL,
                              &g_BBContext.cachedCertValues.m_BBCompatibilityCert.pk.pk,
                               g_blob[iIn],
                               cbIn,
                               g_blob[iOut],
                               dwSymKeySize,
                              &g_BBContext.CryptoContext));
ErrorExit:
    return dr;
}

/* Test API DRM_BBX_DecryptLicense
    argv[0]: index to the global random blob as the input encrypted license.
    argv[1]: the input encrypted license length
    argv[2]: index to the global random blob to save the decrypted license. Or NULL to decrypt in-place.
    argv[3]: blackbox context: NULL, NORMAL
*/
DRM_RESULT DRM_CALL Test_API_DecryptLicense(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT dr;
    int iIn, iOut;
    DRM_DWORD cbIn;
    DRM_BYTE *pLicense = NULL;
    DRM_BB_CONTEXT *pBBContext;

    ChkArg(argc == 4);

    /*
    ** DRM_BBX_DecryptLicense always returns FALSE if WMDRM is not supported because there's no ECC160 bbx privkey available
    ** If it returned a DRM_RESULT, it would be DRM_E_NOTIMPL, so that's what we'll fail with here.
    */
    ChkBOOL( DRM_WMDRM_IsWMDRMSupported(), DRM_E_NOTIMPL );

    iIn = atoi(argv[0]);
    cbIn = atol(argv[1]);
    ChkArg(iIn >= 0 && iIn < MAX_BLOB);

    if (argv[2]) {
        iOut = atoi(argv[2]);
        ChkArg(iIn != iOut && iOut >= 0 && iOut < MAX_BLOB);

        SAFE_OEM_FREE(g_blob[iOut]);
        ChkMem(pLicense = g_blob[iOut] = (DRM_BYTE*)Oem_MemAlloc(cbIn));
    }

    pBBContext = argv[3]? &g_BBContext: NULL;

    ChkOverflow( cbIn + DRM_ECC160_CIPHERTEXT_LEN, DRM_ECC160_CIPHERTEXT_LEN );

    if (DRM_BBX_DecryptLicense(g_blob[iIn], cbIn + DRM_ECC160_CIPHERTEXT_LEN, pLicense, pBBContext, NULL)) {
        dr = DRM_SUCCESS;
        if (!pLicense) /* in-place decryption, move the decrypted license up. */
            memmove(g_blob[iIn], g_blob[iIn] + DRM_ECC160_CIPHERTEXT_LEN, cbIn);
    } else
        dr = DRM_S_FALSE;
ErrorExit:
    return dr;
}

/* Test API DRM_BBX_CipherKeySetup
    argv[0]: index to the binding info
    argv[1]: index to the cipher context to setup
    argv[2]: blackbox context: NULL, NORMAL
*/
DRM_RESULT DRM_CALL Test_API_CipherKeySetup(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT dr;
    int i;
    DRM_BINDING_INFO *pBinding = NULL;
    DRM_CIPHER_CONTEXT *pCipher = NULL;
    DRM_BB_CONTEXT *pBBContext;

    ChkArg(argc == 3);

    /* set up binding info parameter */
    if (argv[0]) {
        i = atoi(argv[0]);
        ChkArg(i >= 0 && i < MAX_GLOBAL_BINDING_BLOB);
        pBinding = &g_binding[i];
    }

    /* set up cipher parameter */
    if (argv[1]) {
        i = atoi(argv[1]);
        ChkArg(i >= 0 && i < MAX_CIPHER);
        pCipher = &g_cipherContext[i];
    }

    /* set up blackbox context */
    pBBContext = argv[2]? &g_BBContext: NULL;

    ChkDR(DRM_BBX_CipherKeySetup(pBinding, 1, pCipher, pBBContext));
ErrorExit:
    return dr;
}

DRM_RESULT DRM_CALL Test_RenewBlackboxContext(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT dr;

    ChkDR(DRM_BBX_Shutdown(&g_BBContext));
    ChkDR(BBXInitWithCerts(&g_BBContext));
    ChkDR(DRM_BBX_Initialize(NULL, &g_BBContext));
ErrorExit:
    return dr;
}

/*************************************************************************************
** TestBBXSymmetricCipher
**
** Test the DRM_BBX_SymmetricCipher function
**
** Arguments
**              argv[0] -- Use Legacy Crypto, TRUE or FALSE
**              argv[1] -- Size of the Key to use. Accepts "AES128KEY" or an integer value
**              argv[2] -- Size of the data to encrypt and decrypt. Accepts an integer
**              argv[3] -- NULL pointer for pbDataOut. Tests in place cipher if NULL.
**              argv[4] -- Use a NULL pointer for the key buffer if NULL.
**              argv[5] -- Use a NULL pointer for the data in buffer if NULL.
**
**************************************************************************************/
DRM_RESULT DRM_CALL TestBBXSymmetricCipher( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT  dr                 = DRM_SUCCESS;
    DRM_BOOL    fLegacyCrypto      = FALSE;
    DRM_BOOL    fInPlace           = FALSE;
    DRM_BYTE   *pbKey              = NULL;
    DRM_BYTE   *pbClearData        = NULL;
    DRM_BYTE   *pbControlClearData = NULL;
    DRM_BYTE   *pbCipherData       = NULL;
    DRM_DWORD   cbKey              = 0;
    DRM_DWORD   cbData             = 0;
    ChkArg( argc >= 3 );
    ChkArg( argv[0] != NULL );
    ChkArg( argv[1] != NULL );
    ChkArg( argv[2] != NULL );

    ChkDR( StringToBool(argv[0], &fLegacyCrypto ) );

    if ( DRMCRT_strncmp( argv[1], "AES128KEY", 9 ) == 0 )
    {
        cbKey = DRM_AES_KEYSIZE_128;
    }
    else
    {
        ChkDR( DRMCRT_AtoDWORD( argv[1], DRMCRT_strlen( argv[1] ), 10, &cbKey ) );
    }
    ChkDR( DRMCRT_AtoDWORD( argv[2], DRMCRT_strlen( argv[2] ), 10, &cbData ) );

    if( argc >= 4 && argv[3] == NULL )
    {
        fInPlace = TRUE;
    }

    ChkMem( pbKey = ( DRM_BYTE * ) Oem_MemAlloc( cbKey ) );
    ChkMem( pbClearData = ( DRM_BYTE * ) Oem_MemAlloc( cbData ) );
    ChkMem( pbControlClearData = ( DRM_BYTE * ) Oem_MemAlloc( cbData ) );
    if( !fInPlace )
    {
        ChkMem( pbCipherData = ( DRM_BYTE * ) Oem_MemAlloc( cbData ) );
    }
    ChkDR( Oem_Random_GetBytes( NULL, pbKey, cbKey ) );
    ChkDR( Oem_Random_GetBytes( NULL, pbClearData, cbData ) );
    MEMCPY( pbControlClearData, pbClearData, cbData );


    ChkDR( DRM_BBX_SymmetricCipher( TRUE,
                                    fLegacyCrypto,
                                    ( argc >= 5 && argv[4] == NULL ) ? NULL : pbKey,
                                    cbKey,
                                    ( argc >= 6 && argv[5] == NULL ) ? NULL : pbClearData,
                                    fInPlace ? NULL : pbCipherData,
                                    cbData ) );
    ChkBOOL( MEMCMP( pbControlClearData,
                     fInPlace ? pbClearData : pbCipherData,
                     cbData ) != 0,
             DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkDR( DRM_BBX_SymmetricCipher( FALSE,
                                    fLegacyCrypto,
                                    ( argc >= 5 && argv[4] == NULL ) ? NULL : pbKey,
                                    cbKey,
                                    ( argc >= 6 && argv[5] == NULL ) ? NULL : ( fInPlace ? pbClearData : pbCipherData ),
                                    fInPlace ? NULL : pbClearData,
                                    cbData ) );

    ChkBOOL( MEMCMP( pbControlClearData,
                     pbClearData,
                     cbData ) == 0,
             DRM_E_TEST_UNEXPECTED_OUTPUT );

ErrorExit:
    SAFE_OEM_FREE( pbKey );
    SAFE_OEM_FREE( pbClearData );
    SAFE_OEM_FREE( pbControlClearData );
    SAFE_OEM_FREE( pbCipherData );
    return dr;
}

/*********************************************************************
**
** Function: _GetDeviceECC256KeyPair -- Copied From Blackbox.c
**
** Synopsis: Function that retrieves a device's ECC-256 key pair. One of
**           the output parameters may be NULL.
**
** Arguments:
**
** [f_poBBXContext]         -- Pointer to a DRM_BB_CONTEXT context.
** [f_poDevPubKey]          -- Pointer to a PUBKEY_P256 that is used to
**                             receive the device's ECC-256 public key.
**                             This parameter is optional.
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
static DRM_RESULT _GetDeviceECC256PubKey(
    __in      DRM_BB_CONTEXT  *f_poBBXContext,
    __out_opt PUBKEY_P256     *f_poDevPubKey,
    __in      eDRM_BINARY_DEVICE_CERT_KEYUSAGE f_eDevKeyUsage
)
{
    DRM_RESULT    dr          = DRM_SUCCESS;


    ChkArg( f_poBBXContext != NULL );
    ChkArg( f_poDevPubKey != NULL );

    switch ( f_eDevKeyUsage )
    {
        case DRM_BINARY_DEVICE_CERT_KEY_SIGN:
            MEMCPY( f_poDevPubKey,
                    &(f_poBBXContext->cachedBCertValues.DeviceKeySign.PublicKey),
                    SIZEOF( PUBKEY_P256 ) );
            break;
        case DRM_BINARY_DEVICE_CERT_KEY_ENCRYPT:
            MEMCPY( f_poDevPubKey,
                    &(f_poBBXContext->cachedBCertValues.DeviceKeyEncrypt.PublicKey),
                    SIZEOF( PUBKEY_P256 ) );
            break;
        default:
            break;
    }

ErrorExit:
    return dr;
}


#define OPT_NULL_PUBKEY      1
#define OPT_ZERO_PUBKEY      128
#define OPT_MISMATCH_PUBKEY  256
#define OPT_NULL_BBCONTEXT   2
#define OPT_NULL_DATAPOINTER 4
#define OPT_NULL_SIZEPOINTER 8
#define OPT_NULL_SIGNATURE   16

#define OPT_TEST_SIGN   32
#define OPT_TEST_VERIFY 64


DRM_RESULT DRM_CALL Test_ECCP256_Decrypt( long argc, __in_ecount( argc ) char **argv )
{

    DRM_RESULT       dr              = DRM_SUCCESS;
    PLAINTEXT_P256   oClearData      = { 0 };
    CIPHERTEXT_P256  oCipherData     = { 0 };
    DRM_BYTE        *pbContextCrypto = NULL;
    PUBKEY_P256      oPubKey         = { 0 };
    DRM_DWORD        cbClearData     = SIZEOF( oClearData.m_rgbPlaintext );
    DRM_DWORD        cbCipherData    = SIZEOF( oCipherData.m_rgbCiphertext );
    DRM_DWORD        dwOption        = 0;

    ChkMem( pbContextCrypto = (DRM_BYTE*)Oem_MemAlloc( __CB_DECL(DRM_PKCRYPTO_CONTEXT_BUFFER_SIZE) ) );
    ZEROMEM( pbContextCrypto, __CB_DECL(DRM_PKCRYPTO_CONTEXT_BUFFER_SIZE) );

    if ( argc >= 1 && argv[0] != NULL )
    {
        ChkDR( DRMCRT_AtoDWORD( argv[ 0 ], DRMCRT_strlen( argv[ 0 ] ), 10, &dwOption ) );
    }

    ChkDR( Oem_Random_GetBytes( NULL, oClearData.m_rgbPlaintext, SIZEOF( PLAINTEXT_P256 ) ) );
    ChkDR( OEM_ECC_GenerateHMACKey_P256( &oClearData, (struct bigctx_t *)pbContextCrypto ) );

    ChkDR( _GetDeviceECC256PubKey( &g_BBContext, &oPubKey, DRM_BINARY_DEVICE_CERT_KEY_ENCRYPT ) );

    ChkDR( OEM_ECC_Encrypt_P256(&oPubKey, &oClearData, &oCipherData, (struct bigctx_t *)pbContextCrypto ) );

    if ( ( dwOption &  OPT_ZERO_PUBKEY ) != 0 )
    {
        ZEROMEM( oPubKey.m_rgbPubkey, SIZEOF( oPubKey ) );
    }

    if ( ( dwOption &  OPT_MISMATCH_PUBKEY ) != 0 )
    {
        ChkDR( Oem_Random_GetBytes( NULL,  oPubKey.m_rgbPubkey, SIZEOF( oPubKey ) ) );
    }

#pragma prefast(push)
#pragma prefast(disable:26015) /* prefast insists that this buffer size may not be correct, but it is */
    ChkDR( DRM_BBX_ECC256_Decrypt( ( ( dwOption & OPT_NULL_BBCONTEXT )   != 0 ) ? NULL : &g_BBContext,
                                   ( ( dwOption & OPT_NULL_DATAPOINTER ) != 0 ) ? NULL : oCipherData.m_rgbCiphertext,
                                   ( ( dwOption & OPT_NULL_SIZEPOINTER ) != 0 ) ? NULL : &cbCipherData,
                                   ( ( dwOption & OPT_NULL_PUBKEY )      != 0 ) ? NULL : &oPubKey ) );
#pragma prefast(pop)
    ChkBOOL( MEMCMP( oClearData.m_rgbPlaintext, oCipherData.m_rgbCiphertext, cbClearData ) == 0,
        DRM_E_TEST_UNEXPECTED_OUTPUT );

ErrorExit:
    SAFE_OEM_FREE( pbContextCrypto );
    return dr;
}

/*************************************************************************************
** Test_SymmetricSign
**
** Tests the DRM_BBX_SymmetricSign and DRM_BBX_SymmetricVerify functions with random data
**
** Arguments
**              argv[0] -- Value for public key:
**                         "RANDOM" to call verify with a random key,
**                         "ZEROS" to call verify with a zero'ed out key,
**                         "NULL" to use device key (default)
**              argv[1] -- length of data to sign
**
**************************************************************************************/
DRM_RESULT DRM_CALL Test_SymmetricSign( long argc, __in_ecount( argc ) char **argv )
{

    DRM_RESULT       dr              = DRM_SUCCESS;
    PUBKEY           oPubKey         = { 0 };
    PUBKEY          *poPubKey        = NULL;
    DRM_DWORD        cbData          = 0;
    DRM_BYTE        *pbData          = NULL;
    DRM_BYTE         rgbSymSig[__CB_DECL( DRM_SHA1_DIGEST_LEN )] = { 0 };

    if ( argc >= 1 && argv[0] != NULL )
    {
        if ( strncmp( argv[0], "RANDOM", 6 ) == 0 )
        {
            ChkDR( Oem_Random_GetBytes( NULL,  oPubKey.y, SIZEOF( oPubKey ) ) );
        }
        else if ( strncmp( argv[0], "ZEROS", 5 ) != 0 )
        {
            ChkArg( FALSE );
        }
        poPubKey = &oPubKey;
    }
    if ( argc >= 2 && argv[1] != NULL )
    {
        ChkDR( DRMCRT_AtoDWORD( argv[1], (DRM_DWORD)DRMCRT_strlen( argv[1] ), 10, &cbData ) );
        if ( cbData > 0 )
        {
            ChkMem( pbData = (DRM_BYTE*) Oem_MemAlloc( cbData ) );
            ChkDR( Oem_Random_GetBytes( NULL,  pbData, cbData ) );
        }
    }

    ChkDR( DRM_BBX_SymmetricSign( &g_BBContext, pbData, cbData, NULL, 0, rgbSymSig ) );
    ChkDR( DRM_BBX_SymmetricVerify( &g_BBContext, poPubKey, pbData, cbData, NULL, 0, NULL, rgbSymSig ) );

ErrorExit:
    SAFE_OEM_FREE( pbData );
    return dr;
}

DRM_RESULT DRM_CALL Test_ECCP256_Sign_Verify( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT       dr            = DRM_SUCCESS;
    PUBKEY_P256      oPubKey       = { 0 };
    SIGNATURE_P256   oSig          = { 0 };
    DRM_BYTE        *pMessage      = NULL;
    DRM_DWORD        cbMessage     = 0;
    DRM_DWORD        dwOption      = 0;
    DRM_DWORD        dwFuncOption  = 0;

    ChkArg( argc >= 1 );
    ChkArg( argv[0] != NULL );

    if ( argc >= 3 && argv[1] != NULL )
    {
        ChkDR( DRMCRT_AtoDWORD( argv[ 1 ], DRMCRT_strlen( argv[ 1 ] ), 10, &dwOption ) );
    }
    if ( argc >= 3 && argv[2] != NULL )
    {
        ChkDR( DRMCRT_AtoDWORD( argv[ 2 ], DRMCRT_strlen( argv[ 2 ] ), 10, &dwFuncOption ) );
    }
    dwOption |= dwFuncOption;

    ChkDR( DRMCRT_AtoDWORD( argv[0] , DRMCRT_strlen( argv[0] ), 10, &cbMessage ) );

    ChkMem( pMessage = ( DRM_BYTE * ) Oem_MemAlloc( cbMessage ) );

    ChkDR( Oem_Random_GetBytes( NULL, pMessage, cbMessage ) );

    if ( dwOption == 0 )
    {
        dwOption = OPT_TEST_SIGN | OPT_TEST_VERIFY;
    }

    if ( ( dwOption & OPT_TEST_SIGN ) != 0 )
    {
        ChkDR( DRM_BBX_ECC256_Sign( ( ( dwOption & OPT_NULL_BBCONTEXT )   != 0 ) ? NULL : &g_BBContext,
                                    ( ( dwOption & OPT_NULL_DATAPOINTER ) != 0 ) ? NULL : pMessage,
                                    cbMessage,
                                    ( ( dwOption & OPT_NULL_PUBKEY )      != 0 ) ? NULL : &oPubKey,
                                    ( ( dwOption & OPT_NULL_SIGNATURE )   != 0 ) ? NULL : oSig.m_rgbSignature ) );
    }

    if ( ( dwOption & OPT_TEST_VERIFY ) != 0 )
    {
        ChkDR( OEM_ECDSA_Verify_P256( ( ( dwOption & OPT_NULL_DATAPOINTER ) != 0 ) ? NULL : pMessage,
                                      cbMessage,
                                      ( ( dwOption & OPT_NULL_PUBKEY )      != 0 ) ? NULL : &oPubKey,
                                      ( ( dwOption & OPT_NULL_SIGNATURE )   != 0 ) ? NULL : (SIGNATURE_P256*)oSig.m_rgbSignature,
                                      ( ( dwOption & OPT_NULL_BBCONTEXT )   != 0 ) ? NULL : (struct bigctx_t *)&g_BBContext.CryptoContext ) );
    }

ErrorExit:
    return dr;
}


BEGIN_TEST_API_LIST( BlackBox )
    API_ENTRY(TestBBXSymmetricCipher)
    API_ENTRY(Test_ECCP256_Decrypt)
    API_ENTRY(Test_ECCP256_Sign_Verify)
    API_ENTRY(Test_SetRandomSeed)
    API_ENTRY(Test_API_GetClientID)
    API_ENTRY(Test_VerifyClientID)
    API_ENTRY(Test_CreateBindingInfo)
    API_ENTRY(Test_API_CanBind)
    API_ENTRY(Test_GenRandomBlob)
    API_ENTRY(Test_API_HashValue)
    API_ENTRY(Test_PKEncryptLarge)
    API_ENTRY(Test_API_DecryptLicense)
    API_ENTRY(Test_API_CipherKeySetup)
    API_ENTRY(Test_BlobOps)
    API_ENTRY(Test_RenewBlackboxContext)
    API_ENTRY(Test_SymmetricSign)
END_TEST_API_LIST

EXIT_PKTEST_NAMESPACE_CODE
