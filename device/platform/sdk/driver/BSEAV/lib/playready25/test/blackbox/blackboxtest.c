/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdafx.h>
#include <oemcipher.h>
#include <drmblackbox.h>
#include <drmlicense.h>
#include <blackboxtest.h>
#include <blackboxhal.h>
#include <blackboxinternal.h>
#include <drmkeyfile.h>
#include <drmprnd.h>
#include <drmwmdrm.h>
#include <drmmathsafe.h>
#include <licgenxmr.h>
#include <oemhaltest.h>
#include <drmbcert.h>
#include <DRMManagerTestAPI.h>
#include <drmbcertparser.h>
#include <oemkeyfile.h>
#include <tstkeyfile.h>
#include <blackboxdevcerts.h>
#include <drmdeviceassets.h>
#include <drmerror.h>

#if DRM_SUPPORT_SECUREOEMHAL
/*
** When using the secure HAL, we require the passed in crypto context to be NULL.
*/
#define USE_SECURE_HAL_CRYPTO_CONTEXT  NULL
#else   /* DRM_SUPPORT_SECUREOEMHAL */
#define USE_SECURE_HAL_CRYPTO_CONTEXT  &f_poBBXContext->CryptoContext
#endif  /* DRM_SUPPORT_SECUREOEMHAL */

ENTER_PKTEST_NAMESPACE_CODE;

DRM_BB_CONTEXT g_BBContext = {0};
CLIENTID g_clientID;

/* #define MAX_GLOBAL_BINDING_BLOB 2 */
DRM_BINDING_INFO g_binding[MAX_GLOBAL_BINDING_BLOB];

#define NA       0xFFFFFFFF

#define MAX_BLOB 5
static DRM_BYTE *g_blob[MAX_BLOB];

#define MAX_CIPHER 1
DRM_CIPHER_CONTEXT g_cipherContext[MAX_CIPHER];

#define CRL_ISSUE_TIME_IN_TEST_REVPACKAGE_XML 1206921600 /* crl issue time of revinfo in revpackage.xml */
static const DRM_DWORD g_dwCRLTime = CRL_ISSUE_TIME_IN_TEST_REVPACKAGE_XML;

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
    DRM_RESULT  dr  = DRM_SUCCESS;

    /*blackbox initialized */
    if( DRM_WMDRM_IsWMDRMSupported() )
    {
        ChkDR( TST_UTL_SetWMDRMCertCachedValues( pcontextBBX ) );
    }

    ChkDR( TST_UTL_SetPlayReadyCertCachedValues( pcontextBBX ) );


ErrorExit:
    return dr;
}


static DRM_RESULT _SetTimeOffset()
{
    DRM_RESULT  dr            = DRM_E_LOGICERR;
    DRMFILETIME ftCurrentTime = {0};
    DRM_DWORD   dwCurrentTime = 0;
    DRM_LONG    lOffsetTime   = 0;

    tResetSystemTimeOffset();

    Oem_Clock_GetSystemTimeAsFileTime( NULL, &ftCurrentTime );
    FILE_TIME_TO_DWORD( ftCurrentTime, dwCurrentTime );

    lOffsetTime = (DRM_LONG)( g_dwCRLTime - dwCurrentTime );
    ChkDR( tChangeSystemTime( lOffsetTime ) );

ErrorExit:

    return dr;
}


DRM_RESULT DRM_CALL MODULE_PRETESTCASE(BlackBox)(long lTCID, const char *strTCName)
{
    DRM_DWORD            i                   = 0;
    DRM_RESULT           dr                  = DRM_SUCCESS;
    DRM_RESULT           dr2                 = DRM_SUCCESS;
    DRM_KEYFILE_CONTEXT *poKeyfileContext    = NULL;
    DRM_APP_CONTEXT     *poAppContext        = NULL;
    DRM_CONST_STRING    *pwszDeviceStoreName = NULL;
    DRM_BYTE            *pbRevocationBuffer  = NULL;
    DRM_BOOL             fCloseKeyFile       = FALSE;


    DRM_InitAllStrings();

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
    ** Create an App context and initialize the filetime to match the test revocation package
    */
    ChkDR( _SetTimeOffset() );

    INIT_SHARED_APP_CONTEXT( poAppContext, pwszDeviceStoreName );
    if ( DRM_REVOCATION_IsRevocationSupported() )
    {
        Log( "", "Setting REVOCATION_BUFFER_VAR" );
        MAP_TO_GLOBAL_TEST_VAR_Alloc( pbRevocationBuffer, REVOCATION_BUFFER_VAR, REVOCATION_BUFFER_SIZE );
        ZEROMEM( pbRevocationBuffer, REVOCATION_BUFFER_SIZE );
        ChkDR( Drm_Revocation_SetBuffer( poAppContext, pbRevocationBuffer, REVOCATION_BUFFER_SIZE ) );
    }

    /*
    ** Set pointer to keyfile context in the blackbox
    */
    ChkMem( poKeyfileContext = (DRM_KEYFILE_CONTEXT*) Oem_MemAlloc( SIZEOF(DRM_KEYFILE_CONTEXT) ) );
    ChkDR( DRM_KF_Initialize( NULL, poKeyfileContext ) );
    g_BBContext.pKeyFileContext = poKeyfileContext;

    if( !DRM_DEVICEASSETS_IsDeviceAssetsSupported() )
    {
        ChkDR( _CopyFileToDRMFolder( KEYFILE_FILE_NAME, KEYFILE_FILE_NAME ) );
    }
    ChkDR( TST_KF_Open( poKeyfileContext, TRUE, OEM_GENERIC_READ ) );
    fCloseKeyFile = TRUE;
    ChkDR( TST_KF_LoadFromFile( poKeyfileContext, TRUE ) );
    ChkDR( DRM_KF_Close( poKeyfileContext ) );
    fCloseKeyFile = FALSE;

    if( DRM_DEVICEASSETS_IsDeviceAssetsSupported() )
    {
        ChkDR( DRM_BBX_SetupSecureStoreKey( &g_BBContext, NULL ) );
    }

    ChkDR( DRM_BBX_Initialize( NULL, &g_BBContext ) );

    /*
    ** Get a devcert
    */
    ChkDR(BBXInitWithCerts(&g_BBContext));

    ChkDR( DRM_BBX_TestDeviceKeyPair( &g_BBContext ) );

ErrorExit:
    if( fCloseKeyFile )
    {
        dr2 = DRM_KF_Close( poKeyfileContext );
        if( DRM_SUCCEEDED( dr ) )
        {
            dr = dr2;
        }
    }
    return dr;
}

DRM_RESULT DRM_CALL MODULE_POSTTESTCASE(BlackBox)(long lTCID, const char *strTCName)
{
    DRM_DWORD            i                   = 0;
    DRM_APP_CONTEXT     *poAppContext        = NULL;
    DRM_RESULT           dr                  = DRM_SUCCESS;
    DRM_CONST_STRING    *pwszDeviceStoreName = NULL;

    GET_SHARED_APP_CONTEXT( poAppContext );

    tResetSystemTimeOffset();

    for (i = 0; i < MAX_BLOB; i++) {
        SAFE_OEM_FREE(g_blob[i]);
        g_blob[i] = NULL;
    }
    if ( g_BBContext.pKeyFileContext != NULL )
    {
        if ( g_BBContext.pKeyFileContext->fInited && g_BBContext.pKeyFileContext->hKeyfile != OEM_INVALID_HANDLE_VALUE )
        {
            DRM_KF_Close( g_BBContext.pKeyFileContext );
        }
    }
    SAFE_OEM_FREE( g_BBContext.pKeyFileContext );

    FREE_SHARED_APP_CONTEXT( poAppContext, pwszDeviceStoreName );

    ChkDR( DRM_BBX_Shutdown( &g_BBContext ) );

ErrorExit:
    return dr;
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

    dwLen = (DRM_DWORD)atol(argv[1]);

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
    dwLen = (DRM_DWORD)atol(argv[1]);
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
    dwLen = (DRM_DWORD)atol(argv[3]);

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
    cbIn = (DRM_DWORD)atol(argv[1]);
    iOut = atoi(argv[2]);
    dwSymKeySize = (DRM_DWORD)atol(argv[3]);

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
    cbIn = (DRM_DWORD)atol(argv[1]);
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
    const DRM_BINDING_INFO *pBinding = NULL;
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
    ChkDR( DRM_BBX_TestDeviceKeyPair( &g_BBContext ) );

ErrorExit:
    return dr;
}

/*************************************************************************************
** TestBBXLegacySymmetricCipher
**
** Test the DRM_BBX_Legacy_SymmetricCipher function
**
** Arguments
**              argv[0] -- Size of the Key to use. Accepts "AES128KEY" or an integer value
**              argv[1] -- Size of the data to encrypt and decrypt. Accepts an integer
**              argv[2] -- NULL pointer for pbDataOut. Tests in place cipher if NULL.
**              argv[3] -- Use a NULL pointer for the key buffer if NULL.
**              argv[4] -- Use a NULL pointer for the data in buffer if NULL.
**
**************************************************************************************/
DRM_RESULT DRM_CALL TestBBXLegacySymmetricCipher( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT  dr                 = DRM_SUCCESS;
    DRM_BOOL    fInPlace           = FALSE;
    DRM_BYTE   *pbKey              = NULL;
    DRM_BYTE   *pbClearData        = NULL;
    DRM_BYTE   *pbControlClearData = NULL;
    DRM_BYTE   *pbCipherData       = NULL;
    DRM_DWORD   cbKey              = 0;
    DRM_DWORD   cbData             = 0;
    DRM_DWORD   cch                = 0;
    ChkArg( argc >= 2 );
    ChkArg( argv[0] != NULL );
    ChkArg( argv[1] != NULL );

    ChkDR( DRM_SizeTToDWord( DRMCRT_strlen(argv[0]), &cch ) );
    ChkDR( DRMCRT_AtoDWORD( argv[0], cch, 10, &cbKey ) );

    ChkDR( DRM_SizeTToDWord( DRMCRT_strlen(argv[1]), &cch ) );
    ChkDR( DRMCRT_AtoDWORD( argv[1], cch, 10, &cbData ) );

    if( argc >= 3 && argv[2] == NULL )
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

    ChkDR( DRM_BBX_Legacy_SymmetricCipher( TRUE,
                                            ( argc >= 4 && argv[3] == NULL ) ? NULL : pbKey,
                                            cbKey,
                                            ( argc >= 5 && argv[4] == NULL ) ? NULL : pbClearData,
                                            fInPlace ? NULL : pbCipherData,
                                            cbData ) );

    ChkBOOL( MEMCMP( pbControlClearData,
                     fInPlace ? pbClearData : pbCipherData,
                     cbData ) != 0,
             DRM_E_TEST_UNEXPECTED_OUTPUT );

    ChkDR( DRM_BBX_Legacy_SymmetricCipher( FALSE,
                                            ( argc >= 4 && argv[3] == NULL ) ? NULL : pbKey,
                                            cbKey,
                                            ( argc >= 5 && argv[4] == NULL ) ? NULL : ( fInPlace ? pbClearData : pbCipherData ),
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

/*************************************************************************************
** TestBBXSymmetricCipher
**
** Test the symetric cipher functionality for keys within the blackbox
**
** Arguments
**              argv[0] -- Use an invalid key handle for the wrapping key if NULL.
**              argv[1] -- Use an invalid key handle for the wrapped key if NULL.
**
**************************************************************************************/
DRM_RESULT DRM_CALL TestBBXSymmetricCipher( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT        dr                                           = DRM_SUCCESS;
    OEM_CRYPTO_HANDLE hWrappingKey                                 = OEM_CRYPTO_HANDLE_INVALID;
    OEM_CRYPTO_HANDLE hKeyToWrap                                   = OEM_CRYPTO_HANDLE_INVALID;
    OEM_CRYPTO_HANDLE hUnwrappedKey                                = OEM_CRYPTO_HANDLE_INVALID;
    DRM_BYTE          rgbEncryptedBuffer[ SIZEOF( PRIVKEY_P256 ) ] = { 0 };
    DRM_DWORD         cbEncryptedBuffer                            = 0;

    DRM_BOOL    fValidWrappingKey = TRUE;
    DRM_BOOL    fValidWrappedKey  = TRUE;

    if( argc >= 1 && argv[0] == NULL )
    {
        fValidWrappingKey = FALSE;
    }

    if( argc >= 2 && argv[1] == NULL)
    {
        fValidWrappedKey = FALSE;
    }

    if( fValidWrappingKey )
    {
        ChkDR( _Hal_GenerateKey( &g_BBContext,
                                 OEM_HAL_KEY_TYPE_ECC_P256_CONTENT_ENCRYPT,
                                 0,
                                 &hWrappingKey ) );
    }

    if( fValidWrappedKey )
    {
        if( fValidWrappingKey )
        {
            DRM_BB_KEY_REGISTER_ENTRY *pKeyEntry      = NULL;
            DRM_BOOL                   fCachedHandle  = FALSE;
            ChkDR( _Hal_AllocateKeyHandle( &g_BBContext, OEM_HAL_KEY_TYPE_ECC_P256_DOMAIN_ENCRYPT, NULL, 0, &hKeyToWrap, &pKeyEntry, &fCachedHandle ) );

            ChkDR( OEM_HAL_TEST_GenerateEcc256KeyPair( &g_BBContext.CryptoContext,
                                                       OEM_HAL_KEY_TYPE_ECC_P256_DOMAIN_ENCRYPT,
                                                       pKeyEntry->indexKeyRegister ) );
        }
        else
        {
            ChkDR( _Hal_GenerateKey( &g_BBContext,
                                     OEM_HAL_KEY_TYPE_ECC_P256_CONTENT_ENCRYPT,
                                     0,
                                     &hKeyToWrap ) );
        }
    }

    /*
    ** Now try to wrap one with the other - this performs a symmetric encrypt
    */
    dr = _Hal_WrapKey( &g_BBContext,
                       hKeyToWrap,
                       hWrappingKey,
                       NULL,
                       &cbEncryptedBuffer );

    if( fValidWrappedKey )
    {
        DRM_TEST_EXPECT_ERROR_AND_MAP_TO_SUCCESS( dr, DRM_E_BUFFERTOOSMALL );
    }
    else
    {
        ChkDR( dr );
    }

    /*
    ** If an invalid wrapping key is supplied, then the OEM HAL exports the public portion of the
    ** ECC Key pair, so this is allowed. Check the size is as expected, though
    */
    if( fValidWrappingKey )
    {
        ChkBOOL( cbEncryptedBuffer == SIZEOF( PRIVKEY_P256 ), DRM_E_TEST_UNEXPECTED_OUTPUT );
    }
    else
    {
        ChkBOOL( cbEncryptedBuffer == SIZEOF( PUBKEY_P256 ), DRM_E_TEST_UNEXPECTED_OUTPUT );
        dr = DRM_SUCCESS;
        goto ErrorExit;
    }

    ChkDR( _Hal_WrapKey( &g_BBContext,
                         hKeyToWrap,
                         hWrappingKey,
                         rgbEncryptedBuffer,
                         &cbEncryptedBuffer ) );

    ChkBOOL( cbEncryptedBuffer == SIZEOF( rgbEncryptedBuffer ), DRM_E_TEST_UNEXPECTED_OUTPUT );

    ChkBOOL( OEM_HAL_TEST_CompareKeyMaterial( OEM_HAL_KEY_TYPE_ECC_P256_CONTENT_ENCRYPT,
                                              TEST_Hal_GetKeyRegisterIndex( &g_BBContext, hKeyToWrap ),
                                              rgbEncryptedBuffer,
                                              cbEncryptedBuffer ) == FALSE,
             DRM_E_TEST_UNEXPECTED_OUTPUT );

    /*
    ** Now unwrap the key again
    */
    ChkDR( _Hal_UnwrapKey( &g_BBContext,
                           OEM_HAL_KEY_TYPE_ECC_P256_DOMAIN_ENCRYPT,
                           hWrappingKey,
                           rgbEncryptedBuffer,
                           cbEncryptedBuffer,
                           NULL,
                           0,
                           NULL,
                           0,
                           &hUnwrappedKey ) );

    ChkBOOL( OEM_HAL_TEST_CompareKeys( OEM_HAL_KEY_TYPE_ECC_P256_DOMAIN_ENCRYPT,
                                       TEST_Hal_GetKeyRegisterIndex( &g_BBContext, hKeyToWrap ),
                                       TEST_Hal_GetKeyRegisterIndex( &g_BBContext, hUnwrappedKey ) ) == TRUE,
             DRM_E_TEST_UNEXPECTED_OUTPUT );

ErrorExit:
    DRM_BBX_SafeReleaseKey( &g_BBContext, &hWrappingKey );
    DRM_BBX_SafeReleaseKey( &g_BBContext, &hKeyToWrap );
    DRM_BBX_SafeReleaseKey( &g_BBContext, &hUnwrappedKey );

    return dr;
}

/*********************************************************************
**
** Function: _GetDeviceECC256KeyPair  -- Copied From Blackbox.c
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
** [f_phDevPrivKey]         -- Pointer to an OEM_CRYPTO_HANDLE that is used to
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
static DRM_API DRM_RESULT DRM_CALL _tGetDeviceECC256KeyPair(
    __in              DRM_BB_CONTEXT                      *f_poBBXContext,
    __in_opt    const PUBKEY_P256                         *f_poDevPubKeyIn,
    __out_opt         PUBKEY_P256                         *f_poDevPubKey,
    __out_ecount( 1 ) OEM_CRYPTO_HANDLE                   *f_phDevPrivKey,
    __in              eDRM_BINARY_DEVICE_CERT_KEYUSAGE     f_eDevKeyUsage )
{
    DRM_RESULT          dr          = DRM_SUCCESS;
    DRM_ENCRYPTED_KEY   oEncKey     = { 0 };
    OEM_CRYPTO_HANDLE   hKeyFileKey = OEM_CRYPTO_HANDLE_INVALID;
    OEM_CRYPTO_HANDLE   hPrivKey    = OEM_CRYPTO_HANDLE_INVALID;
    OEM_HAL_KEY_TYPE    keyType     = OEM_HAL_KEY_TYPE_INVALID;

    ChkArg( f_poBBXContext != NULL );
    ChkArg( f_phDevPrivKey != NULL );

    *f_phDevPrivKey = OEM_CRYPTO_HANDLE_INVALID;

    switch ( f_eDevKeyUsage )
    {
        case DRM_BINARY_DEVICE_CERT_KEY_SIGN:
            if ( f_poDevPubKeyIn == NULL )
            {
                f_poDevPubKeyIn = &(f_poBBXContext->cachedBCertValues.DeviceKeySign.PublicKeyContext.PublicKey);
            }

            keyType = OEM_HAL_KEY_TYPE_ECC_P256_SIGN;

            break;
        case DRM_BINARY_DEVICE_CERT_KEY_ENCRYPT:
            if ( f_poDevPubKeyIn == NULL )
            {
                f_poDevPubKeyIn = &(f_poBBXContext->cachedBCertValues.DeviceKeyEncrypt.PublicKeyContext.PublicKey);
            }

            keyType = OEM_HAL_KEY_TYPE_ECC_P256_CONTENT_ENCRYPT;

            break;

        case DRM_BINARY_DEVICE_CERT_KEY_PRND_ENCRYPT:

            if ( f_poDevPubKeyIn == NULL )
            {
                DRMASSERT( f_poBBXContext->cachedBCertValues.fDeviceKeyPrndEncryptSet );
                f_poDevPubKeyIn = &(f_poBBXContext->cachedBCertValues.DeviceKeyPrndEncrypt.PublicKeyContext.PublicKey);
            }

            keyType = OEM_HAL_KEY_TYPE_ECC_P256_MESSAGE_ENCRYPT;

            break;

        default:
            ChkDR( DRM_E_INVALIDARG );
            break;
    }

    if ( f_poDevPubKey != NULL )
    {
        MEMCPY( f_poDevPubKey, f_poDevPubKeyIn, SIZEOF( PUBKEY_P256 ) );
    }

    ChkDR( TST_KF_GetPrivateKey( f_poBBXContext->pKeyFileContext,
                                 eKF_KEY_TYPE_ECC_256,
                                 ( const DRM_BYTE * )f_poDevPubKeyIn,
                                 SIZEOF( PUBKEY_P256 ),
                                 &oEncKey ) );

    ChkDR( _Hal_GetPreloadedKey( f_poBBXContext,
                                 OEM_HAL_KEY_TYPE_AES_KEYFILE,
                                 &oEncKey.guidKeypairDecryptionKeyID,
                                 &hKeyFileKey ) );

    ChkDR( _Hal_UnwrapKey( f_poBBXContext,
                           keyType,
                           hKeyFileKey,
                           ( DRM_BYTE* )&oEncKey,
                           SIZEOF( oEncKey ),
                           NULL,
                           0,
                           oEncKey.pbEncryptedKey,
                           oEncKey.cbEncryptedKey,
                           &hPrivKey ) );

    *f_phDevPrivKey = hPrivKey;
    hPrivKey        = OEM_CRYPTO_HANDLE_INVALID;

ErrorExit:
    DRM_BBX_SafeReleaseKey( f_poBBXContext, &hKeyFileKey );
    DRM_BBX_SafeReleaseKey( f_poBBXContext, &hPrivKey );

    return dr;
}

/*********************************************************************
**
** Function: _GetDeviceECC256PubKey -- Copied From Blackbox.c
**
** Synopsis: Function that retrieves a device's ECC-256 key pair. One of
**           the output parameters may be NULL.
**
** Arguments:
**
** [f_poBBXContext]         -- Pointer to a DRM_BB_CONTEXT context.
** [f_poDevPubKey]          -- Pointer to a DRM_PUBLIC_KEY_CONTEXT that is used to
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
    __in      DRM_BB_CONTEXT                   *f_poBBXContext,
    __out_opt DRM_PUBLIC_KEY_CONTEXT           *f_poDevPubKey,
    __in      eDRM_BINARY_DEVICE_CERT_KEYUSAGE  f_eDevKeyUsage
)
{
    DRM_RESULT    dr          = DRM_SUCCESS;


    ChkArg( f_poBBXContext != NULL );
    ChkArg( f_poDevPubKey != NULL );

    switch ( f_eDevKeyUsage )
    {
        case DRM_BINARY_DEVICE_CERT_KEY_SIGN:
            ChkDR( DRM_BBX_DuplicatePublicKeyContext( f_poBBXContext,
                                                      f_poDevPubKey,
                                                      &(f_poBBXContext->cachedBCertValues.DeviceKeySign.PublicKeyContext) ) );
            break;
        case DRM_BINARY_DEVICE_CERT_KEY_ENCRYPT:
            ChkDR( DRM_BBX_DuplicatePublicKeyContext( f_poBBXContext,
                                                      f_poDevPubKey,
                                                      &(f_poBBXContext->cachedBCertValues.DeviceKeyEncrypt.PublicKeyContext) ) );
            break;
        case DRM_BINARY_DEVICE_CERT_KEY_PRND_ENCRYPT:
            ChkDR( DRM_BBX_DuplicatePublicKeyContext( f_poBBXContext,
                                                      f_poDevPubKey,
                                                      &(f_poBBXContext->cachedBCertValues.DeviceKeyPrndEncrypt.PublicKeyContext ) ) );
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
    DRM_RESULT              dr                = DRM_SUCCESS;
    PLAINTEXT_P256          oClearData        = { 0 };
    CIPHERTEXT_P256         oCipherData       = { 0 };
    DRM_BYTE               *pbContextCrypto   = NULL;
    DRM_PUBLIC_KEY_CONTEXT  oPubKey           = { 0 };
    DRM_DWORD               cbClearData       = SIZEOF( oClearData.m_rgbPlaintext );
    DRM_DWORD               cbCipherData      = SIZEOF( oCipherData.m_rgbCiphertext );
    DRM_DWORD               dwOption          = 0;
    DRM_DWORD               cch             = 0;
    OEM_CRYPTO_HANDLE       hDomainProtectKey = OEM_CRYPTO_HANDLE_INVALID;

    ChkMem( pbContextCrypto = (DRM_BYTE*)Oem_MemAlloc( __CB_DECL(DRM_PKCRYPTO_CONTEXT_BUFFER_SIZE) ) );
    ZEROMEM( pbContextCrypto, __CB_DECL(DRM_PKCRYPTO_CONTEXT_BUFFER_SIZE) );

    if ( argc >= 1 && argv[0] != NULL )
    {
        ChkDR( DRM_SizeTToDWord( DRMCRT_strlen(argv[0]), &cch ) );
        ChkDR( DRMCRT_AtoDWORD( argv[ 0 ], cch, 10, &dwOption ) );
    }

    ChkDR( Oem_Random_GetBytes( NULL, oClearData.m_rgbPlaintext, SIZEOF( PLAINTEXT_P256 ) ) );
    ChkDR( OEM_ECC_GenerateHMACKey_P256( &oClearData, (struct bigctx_t *)pbContextCrypto ) );

    ChkDR( _GetDeviceECC256PubKey( &g_BBContext, &oPubKey, DRM_BINARY_DEVICE_CERT_KEY_ENCRYPT ) );

    ChkDR( OEM_ECC_Encrypt_P256(&oPubKey.PublicKey, &oClearData, &oCipherData, (struct bigctx_t *)pbContextCrypto ) );

    if ( ( dwOption &  OPT_ZERO_PUBKEY ) != 0 )
    {
        ZEROMEM( oPubKey.PublicKey.m_rgbPubkey, SIZEOF( oPubKey.PublicKey ) );
    }

    if ( ( dwOption &  OPT_MISMATCH_PUBKEY ) != 0 )
    {
        ChkDR( Oem_Random_GetBytes( NULL,  oPubKey.PublicKey.m_rgbPubkey, SIZEOF( oPubKey.PublicKey ) ) );
    }

#pragma prefast(push)
#pragma prefast(disable:26015) /* prefast insists that this buffer size may not be correct, but it is */
    ChkDR( DRM_BBX_ECC256_Decrypt( ( ( dwOption & OPT_NULL_BBCONTEXT )   != 0 ) ? NULL : &g_BBContext,
                                   ( ( dwOption & OPT_NULL_DATAPOINTER ) != 0 ) ? NULL : oCipherData.m_rgbCiphertext,
                                   ( ( dwOption & OPT_NULL_SIZEPOINTER ) != 0 ) ? 0 : cbCipherData,
                                   ( ( dwOption & OPT_NULL_PUBKEY )      != 0 ) ? NULL : &oPubKey.PublicKey,
                                   &hDomainProtectKey ) );
#pragma prefast(pop)

    ChkBOOL( OEM_HAL_TEST_CompareKeyMaterial( _Hal_GetKeyType( &g_BBContext, hDomainProtectKey ),
                                              TEST_Hal_GetKeyRegisterIndex( &g_BBContext, hDomainProtectKey ),
                                              oClearData.m_rgbPlaintext,
                                              cbClearData ) == TRUE,
                                              DRM_E_TEST_UNEXPECTED_OUTPUT );

ErrorExit:
    SAFE_OEM_FREE( pbContextCrypto );

    DRM_BBX_CleanupPublicKeyContext( &g_BBContext, &oPubKey );
    DRM_BBX_SafeReleaseKey( &g_BBContext, &hDomainProtectKey );
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
    const PUBKEY    *poPubKey        = NULL;
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
    DRM_DWORD        cch           = 0;

    ChkArg( argc >= 1 );
    ChkArg( argv[0] != NULL );

    if ( argc >= 3 && argv[1] != NULL )
    {
        ChkDR( DRM_SizeTToDWord( DRMCRT_strlen(argv[1]), &cch ) );
        ChkDR( DRMCRT_AtoDWORD( argv[ 1 ], cch, 10, &dwOption ) );
    }
    if ( argc >= 3 && argv[2] != NULL )
    {
        ChkDR( DRM_SizeTToDWord( DRMCRT_strlen(argv[2]), &cch ) );
        ChkDR( DRMCRT_AtoDWORD( argv[ 2 ], cch, 10, &dwFuncOption ) );
    }
    dwOption |= dwFuncOption;

    ChkDR( DRM_SizeTToDWord( DRMCRT_strlen(argv[0]), &cch ) );
    ChkDR( DRMCRT_AtoDWORD( argv[0] , cch, 10, &cbMessage ) );

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
    SAFE_OEM_FREE( pMessage );
    return dr;
}

static DRM_RESULT _Test_DecryptContent(
    __inout_ecount( 1 )         DRM_BB_CONTEXT    *f_pBbxContext,
    __in                        OEM_CRYPTO_HANDLE  f_hKey,
    __in                        DRM_UINT64         f_qwIv,
    __in                        DRM_DWORD          f_cbData,
    __inout_bcount( f_cbData )  DRM_BYTE          *f_pbData )
{
    DRM_RESULT                   dr             = DRM_E_LOGICERR;
    DRM_AES_COUNTER_MODE_CONTEXT counterContext = { 0 };
    DRM_CIPHER_CONTEXT           cipherContext;

    OEM_SECURE_ZERO_MEMORY( &cipherContext, SIZEOF( cipherContext ) );

    ChkDR( DRM_BBX_CPHR_InitWithKeyHandle( f_pBbxContext,
                                           &cipherContext,
                                           eDRM_AES_COUNTER_CIPHER,
                                           f_hKey ) );

    counterContext.qwInitializationVector = f_qwIv;

    ChkDR( DRM_BBX_CPHR_Decrypt( &cipherContext,
                                 &counterContext,
                                 f_cbData,
                                 f_pbData ) );

ErrorExit:
    DRM_BBX_CPHR_Uninitialize( &cipherContext );
    return dr;
}

static DRM_RESULT _GetRivAndCrl(
    __inout_ecount(1)   DRM_APP_CONTEXT_INTERNAL *f_pAppContext,
    __out               DRM_BYTE                **f_ppRIV,
    __out_ecount(1)     DRM_DWORD                *f_pcRIV,
    __out               DRM_BYTE                 *f_pCRL,
    __out_ecount(1)     DRM_DWORD                 f_cCRL )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_ppRIV  != NULL );
    ChkArg( *f_ppRIV == NULL );
    ChkArg( f_pcRIV  != NULL );
    ChkArg( f_pCRL  != NULL );

    *f_pcRIV = 0;

    dr = DRM_RVK_CheckRevInfoForExpiration(
        &f_pAppContext->oBlackBoxContext.CryptoContext,
        &f_pAppContext->oRevContext,
        &g_guidRevocationTypeRevInfo2,
        NULL,
        NULL,
        f_pcRIV );

    DRM_REQUIRE_BUFFER_TOO_SMALL( dr );
    DRMASSERT( *f_pcRIV > 0 );
    ChkMem( *f_ppRIV = (DRM_BYTE*)Oem_MemAlloc( *f_pcRIV ) );

    ChkDR( DRM_RVK_CheckRevInfoForExpiration(
        &f_pAppContext->oBlackBoxContext.CryptoContext,
        &f_pAppContext->oRevContext,
        &g_guidRevocationTypeRevInfo2,
        NULL,
        *f_ppRIV,
        f_pcRIV ) );

    ChkDR( DRM_RVS_GetRevocationData(
        &f_pAppContext->oRevContext,
        &g_guidRevocationTypePlayReadySilverLightRuntime,
        f_pCRL,
        &f_cCRL ) );

ErrorExit:

    return dr;
}

#define UNWRAP_KEY_FOR_CACHING_TEST( cacheKey, hKey ) do {                          \
        ChkDR( _Hal_UnwrapKey( &g_BBContext,                                        \
                               OEM_HAL_KEY_TYPE_AES_CTR_NEW_LICENSE_CONTENT,        \
                               hGenericContentKey,                                  \
                               rgbKey,                                              \
                               cbKey,                                               \
                               NULL,                                                \
                               0,                                                   \
                               ( const DRM_BYTE* )&cacheKey,                        \
                               sizeof(cacheKey),                                    \
                               &hKey ) );                                           \
        ChkBOOL( hKey != OEM_CRYPTO_HANDLE_INVALID, DRM_E_TEST_UNEXPECTED_OUTPUT ); \
    } while(FALSE)                                                                  \

#define RELEASE_KEY_AND_CHECK_STATE_FOR_CACHING_TEST( hKey, registerState ) do {    \
        keyRegisterIndex = TEST_Hal_GetKeyRegisterIndex( &g_BBContext, hKey );      \
        ChkBOOL( keyRegisterIndex != -1, DRM_E_TEST_UNEXPECTED_OUTPUT );            \
        DRM_BBX_SafeReleaseKey( &g_BBContext, &hKey );                              \
        ChkBOOL( pTable->pEntries[keyRegisterIndex].eState == (registerState),      \
                 DRM_E_TEST_UNEXPECTED_OUTPUT );                                    \
    } while(FALSE)                                                                  \

DRM_RESULT DRM_CALL Test_BBXCryptoHandleCaching( long argc, __in_ecount( argc ) char **argv )
{
    const DRM_LONG                   cacheKey1 = 10001;
    const DRM_LONG                   cacheKey2 = 10002;
    const DRM_LONG                   cacheKey3 = 10003;
    const DRM_LONG                   cacheKey4 = 10004;
    const DRM_LONG                   cacheKey5 = 10005;
    const DRM_LONG                   cacheKey6 = 10006;
    const DRM_BB_KEY_REGISTER_TABLE *pTable    = &g_BBContext.keyHandleState.rgKeyMapTables[ OEM_HAL_KEY_TYPE_AES_CTR_NEW_LICENSE_CONTENT ];

    DRM_RESULT                 dr                                              = DRM_SUCCESS;
    OEM_CRYPTO_HANDLE          hK1                                             = OEM_CRYPTO_HANDLE_INVALID;
    OEM_CRYPTO_HANDLE          hK2                                             = OEM_CRYPTO_HANDLE_INVALID;
    OEM_CRYPTO_HANDLE          hK3                                             = OEM_CRYPTO_HANDLE_INVALID;
PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_NONCONST_LOCAL_VARIABLE_25003, "Prefast Noise: Enclosed local variables cannot be const")
    OEM_CRYPTO_HANDLE          hK1Cached                                       = OEM_CRYPTO_HANDLE_INVALID;
    OEM_CRYPTO_HANDLE          hK2Cached                                       = OEM_CRYPTO_HANDLE_INVALID;
    OEM_CRYPTO_HANDLE          hK3Cached                                       = OEM_CRYPTO_HANDLE_INVALID;
PREFAST_POP /* __WARNING_NONCONST_LOCAL_VARIABLE_25003 */
    DRM_LONG                   i                                               = 0;
    OEM_HAL_KEY_REGISTER_INDEX keyRegisterIndex                                = -1;
    OEM_CRYPTO_HANDLE          testHandles[OEM_BB_KEYMAP_SIZE_AES_CTR_CONTENT];
    OEM_CRYPTO_HANDLE          hGenericContentKey                              = OEM_CRYPTO_HANDLE_INVALID;
    OEM_CRYPTO_HANDLE          hCtrContentKey                                  = OEM_CRYPTO_HANDLE_INVALID;
    DRM_BYTE                   rgbKey[ 2 * DRM_AES_KEYSIZE_128 ]               = { 0 };
    DRM_DWORD                  cbKey                                           = SIZEOF( rgbKey );
    DRM_DWORD                  nOemHalUnwrapsAfterFirstPass                    = 0;
    DRM_DWORD                  nOemHalUnwrapsAfterSecondPass                   = 0;

    /* Setup preconditions */
    for( i = 0; i < OEM_BB_KEYMAP_SIZE_AES_CTR_CONTENT; i++ )
    {
        testHandles[ i ] = OEM_CRYPTO_HANDLE_INVALID;
    }

    ChkDR( _Hal_GenerateKey( &g_BBContext,
                             OEM_HAL_KEY_TYPE_AES_GENERIC_KEY_CONTENT,
                             0,
                             &hGenericContentKey ) );

    ChkDR( _Hal_GenerateKey( &g_BBContext,
                             OEM_HAL_KEY_TYPE_AES_CTR_NEW_LICENSE_CONTENT,
                             150,    /* Security level 150 for testing */
                             &hCtrContentKey ) );

    ChkDR( _Hal_WrapKey( &g_BBContext,
                         hCtrContentKey,
                         hGenericContentKey,
                         rgbKey,
                         &cbKey ) );

    ChkDR( TEST_Hal_PurgeKeyHandleCache( &g_BBContext, OEM_HAL_KEY_TYPE_AES_CTR_NEW_LICENSE_CONTENT ) );

    /* Unwrap should not cache handle if cache key is not specified ( f_pbCacheKey==NULL and f_cbCacheKey==0 ) */
    ChkDR( _Hal_UnwrapKey( &g_BBContext,
                           OEM_HAL_KEY_TYPE_AES_CTR_NEW_LICENSE_CONTENT,
                           hGenericContentKey,
                           rgbKey,
                           cbKey,
                           NULL,
                           0,
                           NULL,
                           0,
                           &hK1 ) );
    RELEASE_KEY_AND_CHECK_STATE_FOR_CACHING_TEST( hK1, eDRM_BB_KEY_REGISTER_STATE_UNALLOCATED );

    /* Unwrap should not return an in-use handle as cached */
    UNWRAP_KEY_FOR_CACHING_TEST( cacheKey1, hK1 );
    UNWRAP_KEY_FOR_CACHING_TEST( cacheKey1, hK2 );
    ChkBOOL( hK2 != hK1, DRM_E_TEST_UNEXPECTED_OUTPUT );

    DRM_BBX_SafeReleaseKey( &g_BBContext, &hK1 );
    DRM_BBX_SafeReleaseKey( &g_BBContext, &hK2 );
    ChkDR( TEST_Hal_PurgeKeyHandleCache( &g_BBContext, OEM_HAL_KEY_TYPE_AES_CTR_NEW_LICENSE_CONTENT ) );

    /* Unwrap should cache if cache key is specified ( f_pbCacheKey!=NULL and f_cbCacheKey>0 ) */
    UNWRAP_KEY_FOR_CACHING_TEST( cacheKey1, hK1 );
    hK1Cached = hK1;
    RELEASE_KEY_AND_CHECK_STATE_FOR_CACHING_TEST( hK1, eDRM_BB_KEY_REGISTER_STATE_UNASSIGNED );

    UNWRAP_KEY_FOR_CACHING_TEST( cacheKey1, hK2 );
    ChkBOOL( hK2 == hK1Cached, DRM_E_TEST_UNEXPECTED_OUTPUT );
    RELEASE_KEY_AND_CHECK_STATE_FOR_CACHING_TEST( hK2, eDRM_BB_KEY_REGISTER_STATE_UNASSIGNED );
    ChkDR( TEST_Hal_PurgeKeyHandleCache( &g_BBContext, OEM_HAL_KEY_TYPE_AES_CTR_NEW_LICENSE_CONTENT ) );

    /* Unwrap should purge cache entries in the correct order. Entry with the smallest dwUnwrapTimestamp should be purged first.*/
    /* Create 3 cached keys K1, K2, K3. Sleep to ensure timestamps are different. */
    UNWRAP_KEY_FOR_CACHING_TEST( cacheKey1, hK1 );
    hK1Cached = hK1;
    RELEASE_KEY_AND_CHECK_STATE_FOR_CACHING_TEST( hK1, eDRM_BB_KEY_REGISTER_STATE_UNASSIGNED );
    tDRMSleep( 100 );

    UNWRAP_KEY_FOR_CACHING_TEST( cacheKey2, hK2 );
    hK2Cached = hK2;
    RELEASE_KEY_AND_CHECK_STATE_FOR_CACHING_TEST( hK2, eDRM_BB_KEY_REGISTER_STATE_UNASSIGNED );
    tDRMSleep( 100 );

    UNWRAP_KEY_FOR_CACHING_TEST( cacheKey3, hK3 );
    hK3Cached = hK3;
    RELEASE_KEY_AND_CHECK_STATE_FOR_CACHING_TEST( hK3, eDRM_BB_KEY_REGISTER_STATE_UNASSIGNED );
    tDRMSleep( 100 );

    /* Retrieve K1 from cache. Timestamp is expected to be updated so that it will not be first key to purge anymore. */
    UNWRAP_KEY_FOR_CACHING_TEST( cacheKey1, hK1 );
    RELEASE_KEY_AND_CHECK_STATE_FOR_CACHING_TEST( hK1, eDRM_BB_KEY_REGISTER_STATE_UNASSIGNED );

    /* Fill up all unallocated entries */
    for( i = 0; i < (DRM_LONG)pTable->cEntries ; i++ )
    {
        if( pTable->pEntries[i].eState == eDRM_BB_KEY_REGISTER_STATE_UNALLOCATED )
        {
            UNWRAP_KEY_FOR_CACHING_TEST( i, testHandles[i] );
        }
    }

    /* Unwrap 3 new keys. This should purge previously created cache entries in the correct order - K2, K3, K1. */
    UNWRAP_KEY_FOR_CACHING_TEST( cacheKey4, hK1 );
    UNWRAP_KEY_FOR_CACHING_TEST( cacheKey5, hK2 );
    UNWRAP_KEY_FOR_CACHING_TEST( cacheKey6, hK3 );

    /* Validate that newly allocated handles are indeed former cached handles */
    ChkBOOL( hK1 == hK2Cached , DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkBOOL( hK2 == hK3Cached , DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkBOOL( hK3 == hK1Cached , DRM_E_TEST_UNEXPECTED_OUTPUT );

    for( i = 0; i < OEM_BB_KEYMAP_SIZE_AES_CTR_CONTENT; i++ )
    {
        if( testHandles[i] != OEM_CRYPTO_HANDLE_INVALID )
        {
            DRM_BBX_SafeReleaseKey( &g_BBContext, &testHandles[i] );
        }
    }

    /* All handles should be cached when performing certificate chain validation second time */
    BBXInitWithCerts( &g_BBContext );
    nOemHalUnwrapsAfterFirstPass = OEM_HAL_TEST_GetUnwrapCount();

    BBXInitWithCerts( &g_BBContext );
    nOemHalUnwrapsAfterSecondPass  = OEM_HAL_TEST_GetUnwrapCount();

    ChkBOOL( nOemHalUnwrapsAfterFirstPass == nOemHalUnwrapsAfterSecondPass, DRM_E_TEST_UNEXPECTED_OUTPUT );

ErrorExit:

    if( hK1 != OEM_CRYPTO_HANDLE_INVALID )
    {
        DRM_BBX_SafeReleaseKey( &g_BBContext, &hK1 );
    }
    if( hK2 != OEM_CRYPTO_HANDLE_INVALID )
    {
        DRM_BBX_SafeReleaseKey( &g_BBContext, &hK2 );
    }
    if( hK3 != OEM_CRYPTO_HANDLE_INVALID )
    {
        DRM_BBX_SafeReleaseKey( &g_BBContext, &hK3 );
    }
    for( i = 0; i < OEM_BB_KEYMAP_SIZE_AES_CTR_CONTENT; i++ )
    {
        if( testHandles[i] != OEM_CRYPTO_HANDLE_INVALID )
        {
            DRM_BBX_SafeReleaseKey( &g_BBContext, &testHandles[i] );
        }
    }

    DRM_BBX_SafeReleaseKey( &g_BBContext, &hCtrContentKey );
    DRM_BBX_SafeReleaseKey( &g_BBContext, &hGenericContentKey );

    return dr;
}

DRM_RESULT DRM_CALL Test_PRNDFunctions( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT                      dr                                      = DRM_SUCCESS;
    DRM_PUBLIC_KEY_CONTEXT          oPubKeyEncrypt                          = {0};
    DRM_PUBLIC_KEY_CONTEXT          oPubKeyPrndEncrypt                      = {0};
    CIPHERTEXT_P256                 oCipherTextMIMK                         = {0};
    CIPHERTEXT_P256                 oCipherTextCICK                         = {0};
    OEM_CRYPTO_HANDLE               hM1                                     = OEM_CRYPTO_HANDLE_INVALID;
    OEM_CRYPTO_HANDLE               hC1                                     = OEM_CRYPTO_HANDLE_INVALID;
    OEM_CRYPTO_HANDLE               hM2                                     = OEM_CRYPTO_HANDLE_INVALID;
    OEM_CRYPTO_HANDLE               hC2                                     = OEM_CRYPTO_HANDLE_INVALID;
    OEM_CRYPTO_HANDLE               hC3                                     = OEM_CRYPTO_HANDLE_INVALID;
    OEM_CRYPTO_HANDLE               hGKC1                                   = OEM_CRYPTO_HANDLE_INVALID;
    OEM_CRYPTO_HANDLE               hR1                                     = OEM_CRYPTO_HANDLE_INVALID;
    OEM_CRYPTO_HANDLE               hGKR1                                   = OEM_CRYPTO_HANDLE_INVALID;
    OEM_ENCRYPTION_HANDLE           handleEncryption                        = OEM_ENCRYPTION_HANDLE_INVALID;
    DRM_BYTE                        rgbClearOrig[DRM_AES_BLOCKLEN]          = {0};
    DRM_BYTE                        rgbClear[DRM_AES_BLOCKLEN]              = {0};
    DRM_BYTE                        rgbClear2[DRM_AES_BLOCKLEN]             = {0};
    DRM_BYTE                        rgbSig1[DRM_AES_BLOCKLEN]               = {0};
    DRM_BYTE                        rgbSig2[DRM_AES_BLOCKLEN]               = {0};
    DRM_UINT64                      qwIv                                    = DRM_UI64LITERAL( 0, 0 );
    DRM_UINT64                      qwIv2                                   = DRM_UI64LITERAL( 0, 0 );
    DRM_BYTE                        rgbKeyPair[DRM_AES_BLOCKLEN * 2]        = {0};
    DRM_KID                         kid                                     = { 0xa3, 0x22, 0xd4, 0xbe, 0x1d, 0xf7, 0x44, 0xed, 0xb5, 0x5a, 0xf2, 0xe0, 0xa8, 0x17, 0x5f, 0xb3 };
    DRM_BYTE                        rgbChecksum[DRM_AES_CK_CHECKSUM_LENGTH] = {0};
    DRM_BYTE                       *pbDeviceCert                            = NULL;
    DRM_DWORD                       cbDeviceCert                            = 0;
    DRMFILETIME                     ftCurrentTime                           = {0};
    DRM_BOOL                        fIvCollision                            = FALSE;
    DRM_BYTE                       *pRIV                                    = NULL;
    DRM_DWORD                       cbRIV                                   = 0;
    DRM_APP_CONTEXT_INTERNAL       *poAppContext                            = NULL;
    DRM_BYTE                        DummyOpaqueInputBuffer[10]              = {0};
    DRM_BYTE                        DummyOpaqueOutputBuffer[10]             = {0};

    GET_SHARED_APP_CONTEXT( poAppContext );

    ChkDR( _GetDeviceECC256PubKey( &g_BBContext, &oPubKeyEncrypt,     DRM_BINARY_DEVICE_CERT_KEY_ENCRYPT ) );
    ChkDR( _GetDeviceECC256PubKey( &g_BBContext, &oPubKeyPrndEncrypt, DRM_BINARY_DEVICE_CERT_KEY_PRND_ENCRYPT ) );

    ChkDR( _GetRivAndCrl( poAppContext,
                          &pRIV,
                          &cbRIV,
                          poAppContext->pbRevocationBuffer,
                          poAppContext->cbRevocationBuffer ) );

    cbDeviceCert = MAX_DEVICE_CERT_SIZE;
    ChkMem( pbDeviceCert = (DRM_BYTE*)Oem_MemAlloc( cbDeviceCert ) );
    ChkDR( TST_UTL_GetPlayReadyCert( pbDeviceCert, &cbDeviceCert, &g_BBContext.CryptoContext ) );

    Oem_Clock_GetSystemTimeAsFileTime( NULL, &ftCurrentTime );

    /* Generate some random clear content */
    ChkDR( Oem_Random_GetBytes(
        g_BBContext.pOEMContext,
        rgbClearOrig,
        SIZEOF( rgbClearOrig ) ) );
    DRMCRT_memcpy( rgbClear, rgbClearOrig, SIZEOF( rgbClear ) );
    DRMCRT_memcpy( rgbClear2, rgbClearOrig, SIZEOF( rgbClear2 ) );

    /* Generate MIMK */
    dr = DRM_BBX_PRND_Tx_GenerateAESKeyPairAndECCEncrypt(
            &g_BBContext,
            DRM_BBX_KEY_PAIR_TYPE_MIMK,
            &oPubKeyPrndEncrypt,
            pbDeviceCert,
            cbDeviceCert,
            NULL,   /* No revocation data */
            0,      /* No revocation data */
            NULL,   /* No revocation info */
            0,      /* No revocation info */
            0,      /* Security level is ignored for MIMK */
            &oCipherTextMIMK,
            &hM1 );

    /* There was no revocation list set, check that DRM_E_INVALIDARG was returned */
    DRM_TEST_EXPECT_ERROR_AND_MAP_TO_SUCCESS( dr, DRM_E_INVALIDARG );

    /* This time, call should succeed */
    ChkDR( DRM_BBX_PRND_Tx_GenerateAESKeyPairAndECCEncrypt(
        &g_BBContext,
        DRM_BBX_KEY_PAIR_TYPE_MIMK,
        &oPubKeyPrndEncrypt,
        pbDeviceCert,
        cbDeviceCert,
        poAppContext->pbRevocationBuffer,
        poAppContext->cbRevocationBuffer,
        pRIV,
        cbRIV,
        0,      /* Security level is ignored for MIMK */
        &oCipherTextMIMK,
        &hM1 ) );

    if( DRM_PRND_IsPRNDReceiverSupported() )
    {
        /* Save MIMK as if receiver */
        ChkDR( DRM_BBX_PRND_Rx_ECCDecryptAESKeyPair(
            &g_BBContext,
            DRM_BBX_KEY_PAIR_TYPE_MIMK,
            &oCipherTextMIMK,
            &hM2 ) );
    }

    /* Generate CICK. RevInfo has been seen, so no need to supply again */
    ChkDR( DRM_BBX_PRND_Tx_GenerateAESKeyPairAndECCEncrypt(
        &g_BBContext,
        DRM_BBX_KEY_PAIR_TYPE_CICK,
        &oPubKeyEncrypt,
        pbDeviceCert,
        cbDeviceCert,
        NULL,   /* No revocation data */
        0,      /* No revocation data */
        NULL,   /* No revocation info */
        0,      /* No revocation info */
        150,    /* Security level 150 for testing */
        &oCipherTextCICK,
        &hC1 ) );

    /* Verify unable to generate MIMK without a specified pubkey */
    dr = DRM_BBX_PRND_Tx_GenerateAESKeyPairAndECCEncrypt(
        &g_BBContext,
        DRM_BBX_KEY_PAIR_TYPE_MIMK,
        NULL,
        NULL,
        0,
        NULL,
        0,
        NULL,
        0,
        0,      /* Security level is ignored for MIMK */
        &oCipherTextCICK,
        &hC2 );
    DRM_TEST_EXPECT_ERROR_AND_MAP_TO_SUCCESS( dr, DRM_E_NOT_FOUND );
    ChkBOOL( hC2 == OEM_CRYPTO_HANDLE_INVALID, DRM_E_TEST_UNEXPECTED_OUTPUT );

    /* Verify: Can't generate CICK with security level too low */
    dr = DRM_BBX_PRND_Tx_GenerateAESKeyPairAndECCEncrypt(
        &g_BBContext,
        DRM_BBX_KEY_PAIR_TYPE_CICK,
        NULL,
        NULL,
        0,
        NULL,
        0,
        NULL,
        0,
        100,    /* 100 < OEM_KEY_GENERATION_SECURITY_LEVEL from oemhaloemimpl.c */
        &oCipherTextCICK,
        &hC2 );
    DRM_TEST_EXPECT_ERROR_AND_MAP_TO_SUCCESS( dr, DRM_E_DEVICE_SECURITY_LEVEL_TOO_LOW );

    /* Verify: Can't generate CICK with security level higher than certificate */
    dr = DRM_BBX_PRND_Tx_GenerateAESKeyPairAndECCEncrypt(
        &g_BBContext,
        DRM_BBX_KEY_PAIR_TYPE_CICK,
        NULL,
        NULL,
        0,
        NULL,
        0,
        NULL,
        0,
        3500,    /* 3500 > certificate's security level */
        &oCipherTextCICK,
        &hC2 );
    DRM_TEST_EXPECT_ERROR_AND_MAP_TO_SUCCESS( dr, DRM_E_DEVICE_SECURITY_LEVEL_TOO_LOW );

    /* Generate CICK without specifying a pubkey */
    ChkDR( DRM_BBX_PRND_Tx_GenerateAESKeyPairAndECCEncrypt(
        &g_BBContext,
        DRM_BBX_KEY_PAIR_TYPE_CICK,
        NULL,
        NULL,
        0,
        NULL,
        0,
        NULL,
        0,
        150,    /* Security level 150 for testing */
        &oCipherTextCICK,
        &hC2 ) );

    /* Verify: Can't generate RIRK with security level too low */
    dr = DRM_BBX_PRND_Tx_GenerateAESKeyPairAndECCEncrypt(
        &g_BBContext,
        DRM_BBX_KEY_PAIR_TYPE_RIRK,
        NULL,
        NULL,
        0,
        NULL,
        0,
        NULL,
        0,
        100,    /* 100 < OEM_KEY_GENERATION_SECURITY_LEVEL from oemhaloemimpl.c */
        &oCipherTextCICK,
        &hR1 );
    DRM_TEST_EXPECT_ERROR_AND_MAP_TO_SUCCESS( dr, DRM_E_DEVICE_SECURITY_LEVEL_TOO_LOW );

    /* Verify: Can't generate RIRK with security level higher than certificate */
    dr = DRM_BBX_PRND_Tx_GenerateAESKeyPairAndECCEncrypt(
        &g_BBContext,
        DRM_BBX_KEY_PAIR_TYPE_RIRK,
        NULL,
        NULL,
        0,
        NULL,
        0,
        NULL,
        0,
        3500,    /* 3500 > certificate's security level */
        &oCipherTextCICK,
        &hR1 );
    DRM_TEST_EXPECT_ERROR_AND_MAP_TO_SUCCESS( dr, DRM_E_DEVICE_SECURITY_LEVEL_TOO_LOW );

    /* Generate RIRK without specifying a pubkey */
    ChkDR( DRM_BBX_PRND_Tx_GenerateAESKeyPairAndECCEncrypt(
        &g_BBContext,
        DRM_BBX_KEY_PAIR_TYPE_RIRK,
        NULL,
        NULL,
        0,
        NULL,
        0,
        NULL,
        0,
        150,    /* Security level 150 for testing */
        &oCipherTextCICK,
        &hR1 ) );

    /* Verify handle uniqueness (not a complete pairwise check, just a sanity check) */
    ChkBOOL( hM1 != hM2, DRM_E_TEST_UNEXPECTED_OUTPUT );

    /* Verify: Can't encrypt content with wrong key types */
    dr = DRM_BBX_PRND_Tx_EncryptClearContentWithAESCTR(
        &g_BBContext,
        hM2,
        &handleEncryption,
        &qwIv,
        NULL,
        OEM_OPAQUE_BUFFER_HANDLE_INVALID,
        OEM_OPAQUE_BUFFER_HANDLE_INVALID,
        rgbClear,
        SIZEOF( rgbClear ) );
    DRM_TEST_EXPECT_ERROR_AND_MAP_TO_SUCCESS( dr, DRM_E_INVALIDARG );
    ChkBOOL( handleEncryption == OEM_ENCRYPTION_HANDLE_INVALID, DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkBOOL( DRM_UI64Eql( qwIv, DRM_UI64HL( 0, 0 ) ), DRM_E_TEST_UNEXPECTED_OUTPUT );
    dr = DRM_BBX_PRND_Tx_EncryptClearContentWithAESCTR(
        &g_BBContext,
        hM1,
        &handleEncryption,
        &qwIv,
        NULL,
        OEM_OPAQUE_BUFFER_HANDLE_INVALID,
        OEM_OPAQUE_BUFFER_HANDLE_INVALID,
        rgbClear,
        SIZEOF( rgbClear ) );
    DRM_TEST_EXPECT_ERROR_AND_MAP_TO_SUCCESS( dr, DRM_E_INVALIDARG );
    ChkBOOL( handleEncryption == OEM_ENCRYPTION_HANDLE_INVALID, DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkBOOL( DRM_UI64Eql( qwIv, DRM_UI64HL( 0, 0 ) ), DRM_E_TEST_UNEXPECTED_OUTPUT );
    dr = DRM_BBX_PRND_Tx_EncryptClearContentWithAESCTR(
        &g_BBContext,
        hR1,
        &handleEncryption,
        &qwIv,
        NULL,
        OEM_OPAQUE_BUFFER_HANDLE_INVALID,
        OEM_OPAQUE_BUFFER_HANDLE_INVALID,
        rgbClear,
        SIZEOF( rgbClear ) );
    DRM_TEST_EXPECT_ERROR_AND_MAP_TO_SUCCESS( dr, DRM_E_INVALIDARG );
    ChkBOOL( handleEncryption == OEM_ENCRYPTION_HANDLE_INVALID, DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkBOOL( DRM_UI64Eql( qwIv, DRM_UI64HL( 0, 0 ) ), DRM_E_TEST_UNEXPECTED_OUTPUT );
    dr = DRM_BBX_PRND_Tx_EncryptClearContentWithAESCTR(
        &g_BBContext,
        hR1,
        &handleEncryption,
        &qwIv,
        NULL,
        OEM_OPAQUE_BUFFER_HANDLE_INVALID,
        OEM_OPAQUE_BUFFER_HANDLE_INVALID,
        rgbClear,
        SIZEOF( rgbClear ) );
    DRM_TEST_EXPECT_ERROR_AND_MAP_TO_SUCCESS( dr, DRM_E_INVALIDARG );
    ChkBOOL( handleEncryption == OEM_ENCRYPTION_HANDLE_INVALID, DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkBOOL( DRM_UI64Eql( qwIv, DRM_UI64HL( 0, 0 ) ), DRM_E_TEST_UNEXPECTED_OUTPUT );

    /* Verify can't encrypt with wrong argument combinations */
    dr = DRM_BBX_PRND_Tx_EncryptClearContentWithAESCTR(
        &g_BBContext,
        hC1,
        &handleEncryption,
        &qwIv,
        NULL,
        ( OEM_OPAQUE_BUFFER_HANDLE )DummyOpaqueInputBuffer,
        ( OEM_OPAQUE_BUFFER_HANDLE )DummyOpaqueOutputBuffer,
        rgbClear,
        SIZEOF( rgbClear ) );
    DRM_TEST_EXPECT_ERROR_AND_MAP_TO_SUCCESS( dr, DRM_E_INVALIDARG );
    ChkBOOL( handleEncryption == OEM_ENCRYPTION_HANDLE_INVALID, DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkBOOL( DRM_UI64Eql( qwIv, DRM_UI64HL( 0, 0 ) ), DRM_E_TEST_UNEXPECTED_OUTPUT );

    /* Verify: Can encrypt content with right key type */
    ChkDR( DRM_BBX_PRND_Tx_EncryptClearContentWithAESCTR(
        &g_BBContext,
        hC1,
        &handleEncryption,
        &qwIv,
        NULL,
        OEM_OPAQUE_BUFFER_HANDLE_INVALID,
        OEM_OPAQUE_BUFFER_HANDLE_INVALID,
        rgbClear,
        SIZEOF( rgbClear ) ) );
    ChkBOOL( 0 != DRMCRT_memcmp( rgbClear, rgbClearOrig, SIZEOF( rgbClear ) ), DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkBOOL( handleEncryption != OEM_ENCRYPTION_HANDLE_INVALID, DRM_E_TEST_UNEXPECTED_OUTPUT );

    /* Verify: repeated encryptions result in different ciphertext */
    do
    {
        ChkDR( DRM_BBX_PRND_Tx_EncryptClearContentWithAESCTR(
            &g_BBContext,
            hC1,
            &handleEncryption,
            &qwIv2,
            NULL,
            OEM_OPAQUE_BUFFER_HANDLE_INVALID,
            OEM_OPAQUE_BUFFER_HANDLE_INVALID,
            rgbClear2,
            SIZEOF( rgbClear2 ) ) );
        ChkBOOL( 0 != DRMCRT_memcmp( rgbClear2, rgbClearOrig, SIZEOF( rgbClear2 ) ), DRM_E_TEST_UNEXPECTED_OUTPUT );

        /* Allow one (very unlikely) IV collision, but bail on the second one */
        if( DRM_UI64Eql( qwIv, qwIv2 ) )
        {
            if( fIvCollision == FALSE )
            {
                fIvCollision = TRUE;
            }
            else
            {
                ChkDR( DRM_E_TEST_UNEXPECTED_OUTPUT );
            }
        }
        else
        {
            ChkBOOL( 0 != DRMCRT_memcmp( rgbClear, rgbClear2, SIZEOF( rgbClear ) ), DRM_E_TEST_UNEXPECTED_OUTPUT );
        }
    }
    while( DRM_UI64Eql( qwIv, qwIv2 ) );

    /* Verify: Perform decrypt, to get back what we started with */
    ChkDR( _Test_DecryptContent(
            &g_BBContext,
            hC1,
            qwIv,
            SIZEOF( rgbClear ),
            rgbClear ) );
    ChkBOOL( 0 == DRMCRT_memcmp( rgbClear, rgbClearOrig, SIZEOF( rgbClear ) ), DRM_E_TEST_UNEXPECTED_OUTPUT );

    DRM_BBX_SafeDestroySampleEncryptionContext( &handleEncryption );    /* returns void */

    /* Verify: Can't calc checksum with wrong key types */
    dr = DRM_BBX_PRND_Tx_CalculateChecksum(
        &g_BBContext,
        hM2,
        &kid,
        rgbChecksum );
    DRM_TEST_EXPECT_ERROR_AND_MAP_TO_SUCCESS( dr, DRM_E_INVALIDARG );
    dr = DRM_BBX_PRND_Tx_CalculateChecksum(
        &g_BBContext,
        hM1,
        &kid,
        rgbChecksum );
    DRM_TEST_EXPECT_ERROR_AND_MAP_TO_SUCCESS( dr, DRM_E_INVALIDARG );
    dr = DRM_BBX_PRND_Tx_CalculateChecksum(
        &g_BBContext,
        hC1,
        &kid,
        rgbChecksum );
    DRM_TEST_EXPECT_ERROR_AND_MAP_TO_SUCCESS( dr, DRM_E_INVALIDARG );

    /* Verify: Can calc checksum with right key type */
    ChkDR( DRM_BBX_PRND_Tx_CalculateChecksum(
        &g_BBContext,
        hR1,
        &kid,
        rgbChecksum ) );

    /* Verify: Can sign/verify with right key types */
    /* Verify: Signatures match when using same keys */
    /* Verify: Signatures can be verified when using same keys */
    ChkDR( DRM_BBX_PRND_Tx_SignWithAESOMAC(
        &g_BBContext,
        hM1,
        rgbClear,
        SIZEOF( rgbClear ),
        rgbSig1 ) );

    if( DRM_PRND_IsPRNDReceiverSupported() )
    {
        ChkDR( DRM_BBX_PRND_Rx_VerifySignatureWithAESOMAC(
            &g_BBContext,
            hM2,
            rgbClear,
            SIZEOF( rgbClear ),
            rgbSig1 ) );
        /* Note: Do not attempt to sign here with hC1 because our clear data would have to be a valid license! */
        ChkBOOL( 0 != DRMCRT_memcmp( rgbSig1, rgbSig2, SIZEOF( rgbSig1 ) ), DRM_E_TEST_UNEXPECTED_OUTPUT );
    }

    dr = DRM_BBX_PRND_Tx_VerifySignatureWithAESOMAC(
        &g_BBContext,
        hM1,
        rgbClear,
        SIZEOF( rgbClear ),
        rgbSig2 );
    DRM_TEST_EXPECT_ERROR_AND_MAP_TO_SUCCESS( dr, DRM_E_INVALID_SIGNATURE );
    ZEROMEM( &rgbSig2, SIZEOF( rgbSig2 ) );
    /* Note: Do not attempt to sign here with hR1 because our clear data would have to be a valid license! */

    /* Verify: Can't generate and ECB encrypt with wrong key types */
    dr = DRM_BBX_PRND_Tx_GenerateAESKeyPairAndAESECBEncrypt(
        &g_BBContext,
        DRM_BBX_KEY_PAIR_TYPE_CICK,
        hM2,
        150,    /* Security level 150 for testing */
        rgbKeyPair,
        &hC3 );
    DRM_TEST_EXPECT_ERROR_AND_MAP_TO_SUCCESS( dr, DRM_E_OEMHAL_UNSUPPORTED_KEY_WRAPPING_FORMAT );
    dr = DRM_BBX_PRND_Tx_GenerateAESKeyPairAndAESECBEncrypt(
        &g_BBContext,
        DRM_BBX_KEY_PAIR_TYPE_CICK,
        hM1,
        150,    /* Security level 150 for testing */
        rgbKeyPair,
        &hC3 );
    DRM_TEST_EXPECT_ERROR_AND_MAP_TO_SUCCESS( dr, DRM_E_OEMHAL_UNSUPPORTED_KEY_WRAPPING_FORMAT );
    dr = DRM_BBX_PRND_Tx_GenerateAESKeyPairAndAESECBEncrypt(
        &g_BBContext,
        DRM_BBX_KEY_PAIR_TYPE_CICK,
        hC1,
        150,    /* Security level 150 for testing */
        rgbKeyPair,
        &hC3 );
    DRM_TEST_EXPECT_ERROR_AND_MAP_TO_SUCCESS( dr, DRM_E_OEMHAL_UNSUPPORTED_KEY_WRAPPING_FORMAT );

    /* Verify: Can't generate CICK and ECB encrypt with security level too low */
    dr = DRM_BBX_PRND_Tx_GenerateAESKeyPairAndAESECBEncrypt(
        &g_BBContext,
        DRM_BBX_KEY_PAIR_TYPE_CICK,
        hR1,
        100,    /* 100 < OEM_KEY_GENERATION_SECURITY_LEVEL from oemhaloemimpl.c */
        rgbKeyPair,
        &hC3 );
    DRM_TEST_EXPECT_ERROR_AND_MAP_TO_SUCCESS( dr, DRM_E_DEVICE_SECURITY_LEVEL_TOO_LOW );

    /* Verify: Can't generate CICK and ECB encrypt with security level higher than RIRK */
    dr = DRM_BBX_PRND_Tx_GenerateAESKeyPairAndAESECBEncrypt(
        &g_BBContext,
        DRM_BBX_KEY_PAIR_TYPE_CICK,
        hR1,
        151,    /* 151 > security level we used to generate hR1 */
        rgbKeyPair,
        &hC3 );
    DRM_TEST_EXPECT_ERROR_AND_MAP_TO_SUCCESS( dr, DRM_E_DEVICE_SECURITY_LEVEL_TOO_LOW );

    /* Verify: Can generate and ECB encrypt with right key type */
    ChkDR( DRM_BBX_PRND_Tx_GenerateAESKeyPairAndAESECBEncrypt(
        &g_BBContext,
        DRM_BBX_KEY_PAIR_TYPE_CICK,
        hR1,
        150,    /* Security level 150 for testing */
        rgbKeyPair,
        &hC3 ) );

    /* Verify: Can release keys independently but still use non-released keys */
    DRM_BBX_SafeReleaseKey( &g_BBContext, &hC1 );
    if( DRM_PRND_IsPRNDReceiverSupported() )
    {
        ChkDR( DRM_BBX_PRND_Rx_SignWithAESOMAC(
            &g_BBContext,
            hM2,
            rgbClear,
            SIZEOF( rgbClear ),
            rgbSig2 ) );
        ChkDR( DRM_BBX_PRND_Tx_VerifySignatureWithAESOMAC(
            &g_BBContext,
            hM1,
            rgbClear,
            SIZEOF( rgbClear ),
            rgbSig2 ) );

        /* Verify: Can cache CI/CK keypairs (only) outside the blackbox and then use them later - encrypt/decrypt using cached CK */
        DRMCRT_memcpy( rgbClear, rgbClearOrig, SIZEOF( rgbClear ) );
    }

    qwIv = DRM_UI64HL( 0, 0 );
    ChkDR( DRM_BBX_PRND_Tx_EncryptClearContentWithAESCTR(
        &g_BBContext,
        hC2,
        &handleEncryption,
        &qwIv,
        NULL,
        OEM_OPAQUE_BUFFER_HANDLE_INVALID,
        OEM_OPAQUE_BUFFER_HANDLE_INVALID,
        rgbClear,
        SIZEOF( rgbClear ) ) );
    ChkBOOL( handleEncryption != OEM_ENCRYPTION_HANDLE_INVALID, DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkBOOL( 0 != DRMCRT_memcmp( rgbClear, rgbClearOrig, SIZEOF( rgbClear ) ), DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkDR( DRM_BBX_PRND_Tx_GenerateAESGenericKey( &g_BBContext, OEM_HAL_KEY_TYPE_AES_GENERIC_KEY_CONTENT, &hGKC1 ) );
    dr = DRM_BBX_PRND_Tx_EncryptAESKeyPairWithAESECBUsingGenericKey(    /* First param must be GKC, not any other key type */
        &g_BBContext,
        hC2,
        hC2,
        rgbKeyPair );
    DRM_TEST_EXPECT_ERROR_AND_MAP_TO_SUCCESS( dr, DRM_E_OEMHAL_UNSUPPORTED_KEY_WRAPPING_FORMAT );
    dr = DRM_BBX_PRND_Tx_EncryptAESKeyPairWithAESECBUsingGenericKey(    /* Can't use RI/RK with GKC (must use with GKR) */
        &g_BBContext,
        hGKC1,
        hR1,
        rgbKeyPair );
    DRM_TEST_EXPECT_ERROR_AND_MAP_TO_SUCCESS( dr, DRM_E_OEMHAL_UNSUPPORTED_KEY_WRAPPING_FORMAT );
    ChkDR( DRM_BBX_PRND_Tx_EncryptAESKeyPairWithAESECBUsingGenericKey(
        &g_BBContext,
        hGKC1,
        hC2,
        rgbKeyPair ) );
    DRM_BBX_SafeReleaseKey( &g_BBContext, &hC2 );
    ChkDR( DRM_BBX_PRND_Tx_DecryptAESKeyPairWithAESECBUsingGenericKey(
        &g_BBContext,
        hGKC1,
        rgbKeyPair,
        &hC2 ) );
    ChkDR( _Test_DecryptContent(
        &g_BBContext,
        hC2,
        qwIv,
        SIZEOF( rgbClear ),
        rgbClear ) );
    ChkBOOL( 0 == DRMCRT_memcmp( rgbClear, rgbClearOrig, SIZEOF( rgbClear ) ), DRM_E_TEST_UNEXPECTED_OUTPUT );

    /* Verify: Can cache RI/RK keypairs (only) outside the blackbox and then use them later - encrypt/decrypt using cached RI */
    DRMCRT_memcpy( rgbClear,  rgbClearOrig, SIZEOF( rgbClear ) );
    DRMCRT_memcpy( rgbClear2, rgbClearOrig, SIZEOF( rgbClear2 ) );
    DRM_BBX_SafeDestroySampleEncryptionContext( &handleEncryption );    /* returns void */
    ChkDR( DRM_BBX_PRND_Tx_EncryptClearContentWithAESCTR(
        &g_BBContext,
        hC2,
        &handleEncryption,
        &qwIv,
        NULL,
        OEM_OPAQUE_BUFFER_HANDLE_INVALID,
        OEM_OPAQUE_BUFFER_HANDLE_INVALID,
        rgbClear,
        SIZEOF( rgbClear ) ) );
    DRM_BBX_SafeDestroySampleEncryptionContext( &handleEncryption );    /* returns void */
    ChkDR( DRM_BBX_PRND_Tx_GenerateAESGenericKey( &g_BBContext, OEM_HAL_KEY_TYPE_AES_GENERIC_KEY_ROOT, &hGKR1 ) );
    dr = DRM_BBX_PRND_Tx_EncryptAESKeyPairWithAESECBUsingGenericKey(    /* First param must be GKR, not any other key type */
        &g_BBContext,
        hR1,
        hR1,
        rgbKeyPair );
    DRM_TEST_EXPECT_ERROR_AND_MAP_TO_SUCCESS( dr, DRM_E_OEMHAL_UNSUPPORTED_KEY_WRAPPING_FORMAT );
    dr = DRM_BBX_PRND_Tx_EncryptAESKeyPairWithAESECBUsingGenericKey(    /* Can't use CI/CK with GKR (must use with GKC) */
        &g_BBContext,
        hGKR1,
        hC2,
        rgbKeyPair );
    DRM_TEST_EXPECT_ERROR_AND_MAP_TO_SUCCESS( dr, DRM_E_OEMHAL_UNSUPPORTED_KEY_WRAPPING_FORMAT );
    ChkDR( DRM_BBX_PRND_Tx_EncryptAESKeyPairWithAESECBUsingGenericKey(
        &g_BBContext,
        hGKC1,
        hC2,
        rgbKeyPair ) );
    DRM_BBX_SafeReleaseKey( &g_BBContext, &hC1 );
    ChkDR( DRM_BBX_PRND_Tx_DecryptAESKeyPairWithAESECBUsingGenericKey(
        &g_BBContext,
        hGKC1,
        rgbKeyPair,
        &hC1 ) );
    ChkDR( _Test_DecryptContent(
            &g_BBContext,
            hC1,
            qwIv,
            SIZEOF( rgbClear ),
            rgbClear ) );
    ChkBOOL( 0 == DRMCRT_memcmp( rgbClear, rgbClearOrig, SIZEOF( rgbClear ) ), DRM_E_TEST_UNEXPECTED_OUTPUT );
    DRM_BBX_SafeDestroySampleEncryptionContext( &handleEncryption );    /* returns void */
    ChkDR( DRM_BBX_PRND_Tx_EncryptClearContentWithAESCTR(
        &g_BBContext,
        hC1,
        &handleEncryption,
        &qwIv,
        NULL,
        OEM_OPAQUE_BUFFER_HANDLE_INVALID,
        OEM_OPAQUE_BUFFER_HANDLE_INVALID,
        rgbClear2,
        SIZEOF( rgbClear2 ) ) );
    ChkBOOL( 0 != DRMCRT_memcmp( rgbClearOrig, rgbClear2, SIZEOF( rgbClear2 ) ), DRM_E_TEST_UNEXPECTED_OUTPUT );

ErrorExit:
    SAFE_OEM_FREE( pbDeviceCert );

    DRM_BBX_CleanupPublicKeyContext( &g_BBContext, &oPubKeyEncrypt );
    DRM_BBX_CleanupPublicKeyContext( &g_BBContext, &oPubKeyPrndEncrypt );

    DRM_BBX_SafeDestroySampleEncryptionContext( &handleEncryption );    /* returns void */
    DRM_BBX_SafeReleaseKey( &g_BBContext, &hM1 );
    DRM_BBX_SafeReleaseKey( &g_BBContext, &hC1 );
    DRM_BBX_SafeReleaseKey( &g_BBContext, &hM2 );
    DRM_BBX_SafeReleaseKey( &g_BBContext, &hC2 );
    DRM_BBX_SafeReleaseKey( &g_BBContext, &hC3 );
    DRM_BBX_SafeReleaseKey( &g_BBContext, &hGKC1 );
    DRM_BBX_SafeReleaseKey( &g_BBContext, &hR1 );
    DRM_BBX_SafeReleaseKey( &g_BBContext, &hGKR1 );
    SAFE_OEM_FREE( pRIV );

    return dr;
}


static DRM_RESULT _Test_ECC256_DecryptKey(
    __inout_ecount( 1 )                DRM_BB_CONTEXT                       *f_poBBXContext,
    __in_bcount( f_cbData )      const DRM_BYTE                             *f_pbData,
    __in                               DRM_DWORD                             f_cbData,
    __inout_ecount( 1 )          const DRM_PUBLIC_KEY_CONTEXT               *f_poECC256PubKey,
    __in                               eDRM_BINARY_DEVICE_CERT_KEYUSAGE      f_eDevKeyUsage,
    __in                               OEM_HAL_KEY_TYPE                      f_eKeyType,
    __out_ecount( 1 )                  OEM_CRYPTO_HANDLE                    *f_phKeyPair )
{
    DRM_RESULT                   dr             = DRM_SUCCESS;
    OEM_CRYPTO_HANDLE            hPrivKey       = OEM_CRYPTO_HANDLE_INVALID;
    const PUBKEY_P256           *pPubKey        = NULL;
    OEM_CRYPTO_HANDLE            hKeyPair       = OEM_CRYPTO_HANDLE_INVALID;
    DRM_BB_KEY_REGISTER_ENTRY   *pKeyEntry      = NULL;
    DRM_BOOL                     fCachedHandle  = FALSE;

    ChkArg( f_poBBXContext   != NULL );
    ChkArg( f_pbData         != NULL );
    ChkArg( f_cbData         == ECC_P256_CIPHERTEXT_SIZE_IN_BYTES );
    ChkArg( f_poECC256PubKey != NULL );
    ChkArg( f_phKeyPair      != NULL );

    *f_phKeyPair = OEM_CRYPTO_HANDLE_INVALID;
    pPubKey      = &f_poECC256PubKey->PublicKey;

    if( DRM_UTL_IsZeros( (const DRM_BYTE*)pPubKey, SIZEOF( PUBKEY_P256 ) ) )
    {
        pPubKey = NULL;
    }

    ChkDR( _tGetDeviceECC256KeyPair( f_poBBXContext,
                                     pPubKey,
                                     NULL,
                                     &hPrivKey,
                                     f_eDevKeyUsage ) );

    /*
    ** Normally, we would call _Hal_UnwrapKey directly.
    ** However, we're not allowed to directly decrypt an ECC256 encrypted content key without a license.
    ** So, we'll mimic _Hal_UnwrapKey's behavior with fewer correctness checks,
    ** and we'll use OEM_HAL_TEST_InitTestKeysUnwrap/Oem_HalOemImpl_UnwrapAesKeyPairWithEcc256PrivKey
    ** (where appropriate) instead of calling Oem_Hal_UnwrapKey.
    ** This will allow us to bypass the multitude of checks that would block us from
    ** decrypting an ECC256 encrypted content key without a license.
    ** If we're NOT trying to decrypt an ECC256 encrypted content key in this function call,
    ** then the call to the standard Oem_Hal_UnwrapKey can service the request.
    */
    ChkDR( _Hal_AllocateKeyHandle( f_poBBXContext, f_eKeyType, f_pbData, f_cbData, &hKeyPair, &pKeyEntry, &fCachedHandle ) );
    ChkPtr( pKeyEntry );
    if( !fCachedHandle )
    {
        OEM_HAL_UNWRAP_CONTEXT      unwrapContext;
        OEM_HAL_KEY_REGISTER_INDEX  indexWrappingKey = ((DRM_BB_KEY_REGISTER_ENTRY*)hPrivKey)->indexKeyRegister;
        OEM_HAL_KEY_TYPE            eWrappingKeyType = ((DRM_BB_KEY_REGISTER_ENTRY*)hPrivKey)->eType;

        ChkDR( OEM_HAL_TEST_InitTestKeysUnwrap( f_eKeyType,
                                                pKeyEntry->indexKeyRegister,
                                                eWrappingKeyType,
                                                indexWrappingKey,
                                                f_cbData,
                                                &unwrapContext ) );
        if( dr == DRM_S_FALSE )
        {
            ChkDR( Oem_Hal_UnwrapKey( USE_SECURE_HAL_CRYPTO_CONTEXT,
                                      f_eKeyType,
                                      pKeyEntry->indexKeyRegister,
                                      eWrappingKeyType,
                                      indexWrappingKey,
                                      f_pbData,
                                      f_cbData,
                                      NULL,
                                      0 ) );
        }
        else
        {
            DRMASSERT( unwrapContext.eKeyDataType    == eOEM_HAL_KEYDATA_TYPE_ENCRYPTED_BYTES );
            DRMASSERT( unwrapContext.eDecryptionType == eOEM_HAL_CRYPTO_TYPE_AES_KEY_PAIR );
            ChkDR( Oem_HalOemImpl_UnwrapAesKeyPairWithEcc256PrivKey(
                &f_poBBXContext->CryptoContext,
                &unwrapContext,
                f_pbData,
                f_cbData ) );
        }

        if( f_cbData > 0 )
        {
            DRM_BYT_CopyBytes( pKeyEntry->pbCacheKey, 0, f_pbData, 0, f_cbData );
            pKeyEntry->cbCacheKey = f_cbData;
        }
        else
        {
            OEM_SECURE_ZERO_MEMORY( pKeyEntry->pbCacheKey, DRM_BB_MAX_CACHE_KEY_SIZE_IN_BYTES );
            pKeyEntry->cbCacheKey = 0;
        }
    }

    *f_phKeyPair = hKeyPair;
    hKeyPair     = OEM_CRYPTO_HANDLE_INVALID;

ErrorExit:
    DRM_BBX_SafeReleaseKey( f_poBBXContext, &hPrivKey );
    DRM_BBX_SafeReleaseKey( f_poBBXContext, &hKeyPair );
    return dr;
}


PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_USE_WIDE_API_25068, "Ignore prefast warning about ANSI functions for this function.");
#define Test_PRNDRebindLicense_Arg0_FLAGS_LowerSecurityLevel     0x00000001
#define Test_PRNDRebindLicense_Arg0_FLAGS_LowerRIV               0x00000002
#define Test_CreateXMRLicense_MAX_ARGC_INDEX                     39
DRM_RESULT DRM_CALL Test_PRNDRebindLicense( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT                           dr                                                         = DRM_SUCCESS;
    DRM_PUBLIC_KEY_CONTEXT               oPubKeyEncrypt                                             = {0};
    DRM_PUBLIC_KEY_CONTEXT               oPubKeyPrndEncrypt                                         = {0};
    CIPHERTEXT_P256                      oCipherTextCICK;                                           /* Do not zero-init key material */
    OEM_CRYPTO_HANDLE                    hCI                                                        = OEM_CRYPTO_HANDLE_INVALID;
    DRM_BYTE                             rgbClearOrig[DRM_AES_BLOCKLEN]                             = {0};
    DRM_BYTE                             rgbClear[DRM_AES_BLOCKLEN]                                 = {0};
    DRM_AES_COUNTER_MODE_CONTEXT         oCtrCtx                                                    = {0};
    DRM_BYTE                            *pbDeviceCert                                               = NULL;
    DRM_DWORD                            cbDeviceCert                                               = 0;
    DRM_BINDING_INFO                     rgoBindingInfo[DRM_MAX_LICENSE_CHAIN_DEPTH]                = {0};
    DRM_KID                              oKid                                                       = {0};
    DRM_LID                              oLid                                                       = {0};
    DRM_CHAR                             rgchKid[ CCH_BASE64_EQUIV( SIZEOF( DRM_KID ) ) + 1 ]       = {0};
    DRM_CHAR                             rgchLid[ CCH_BASE64_EQUIV( SIZEOF( DRM_LID ) ) + 1 ]       = {0};
    DRM_DWORD                            cchKid                                                     = CCH_BASE64_EQUIV( SIZEOF( DRM_KID ) );
    DRM_DWORD                            cchLid                                                     = CCH_BASE64_EQUIV( SIZEOF( DRM_LID ) );
    const DRM_LONG                       argcInternal                                               = Test_CreateXMRLicense_MAX_ARGC_INDEX;
    DRM_CHAR                            *argvInternal[ Test_CreateXMRLicense_MAX_ARGC_INDEX + 1 ]   = {0};
    DRM_BYTE                            *pbLicense                                                  = NULL;
    DRM_DWORD                            cbLicense                                                  = 0;
    DRM_BYTE                            *pbContentKeyPair                                           = NULL;
    DRM_DWORD                            cbContentKeyPair                                           = 0;
    const DRM_CHAR                       szKeySeed[]                                                = "XVBovsmzhP9gRIZxWfFta3VVRPzVEWmJsazEJ46I";
    const DRM_CHAR                       szOmit[]                                                   = "OMIT";
    DRM_CHAR                             rgchKeySeed[SIZEOF(szKeySeed)]                             = {0};
    DRM_CHAR                             rgchOmit[SIZEOF(szOmit)]                                   = {0};
    DRM_XMR_LICENSE                      oXMRLicense                                                = {0};
    DRM_STACK_ALLOCATOR_CONTEXT          oStack                                                     = {0};
    DRM_BYTE                             rgbStack[4096]                                             = {0};
    DRM_DWORD                            cbStack                                                    = 4096;
    OEM_CRYPTO_HANDLE                    hContentKey1                                               = OEM_CRYPTO_HANDLE_INVALID;
    OEM_CRYPTO_HANDLE                    hContentKey2                                               = OEM_CRYPTO_HANDLE_INVALID;
    OEM_ENCRYPTION_HANDLE                hEncryptionContext                                         = OEM_ENCRYPTION_HANDLE_INVALID;
    DRM_UINT64                           qwIV                                                       = DRM_UI64LITERAL( 0, 0 );
    DRM_CIPHER_CONTEXT                   cipherContext                                              = { (enum _DRM_SUPPORTED_CIPHERS)0, 0 };
    DRM_BYTE                            *pRIV                                                       = NULL;
    DRM_DWORD                            cbRIV                                                      = 0;
    DRM_APP_CONTEXT_INTERNAL            *poAppContext                                               = NULL;
    const DRM_CHAR                       szTrue[]                                                   = "TRUE";
    DRM_CHAR                             rgchTrue[SIZEOF(szTrue)]                                   = {0};
    const DRM_CHAR                       szSL100[]                                                  = "100";
    DRM_CHAR                             rgchSL100[SIZEOF(szSL100)]                                 = {0};
    const DRM_CHAR                       szRIV1[]                                                   = "1";
    DRM_CHAR                             rgchRIV1[SIZEOF(szRIV1)]                                   = {0};
    DRM_BOOL                             fArg0LowerSecurityLevel                                    = FALSE;
    DRM_BOOL                             fArg0LowerRIV                                              = FALSE;

#if !DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY
    DRM_BYTE                             rgbWorking[DRM_AES_BLOCKLEN]                               = {0};
    DRM_BYTE                            *pbLicense2                                                 = NULL;
    DRM_DWORD                            cbLicense2                                                 = 0;
    DRM_DWORD                            idx                                                        = 0;
    DRM_DWORD                            cbCipherTextCICK                                           = SIZEOF( oCipherTextCICK );
    DRM_BYTE                             rgbSignature[DRM_AES_BLOCKLEN]                             = {0};
    const DRM_DWORD                      cbSignatureObject                                          = XMR_AES_OMAC_SIGNATURE_LENGTH + XMR_BASE_OBJECT_LENGTH + 2 * SIZEOF( DRM_WORD );
#endif  /* !DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY */

    if( HASARG(0) )
    {
        DRM_DWORD   dwArg0Flags = 0;
        ChkDR( DRMCRT_AtoDWORD( argv[0], (DRM_DWORD)DRMCRT_strlen( argv[0] ), 10, &dwArg0Flags ) );
        fArg0LowerSecurityLevel = 0 != ( dwArg0Flags & Test_PRNDRebindLicense_Arg0_FLAGS_LowerSecurityLevel );
        fArg0LowerRIV           = 0 != ( dwArg0Flags & Test_PRNDRebindLicense_Arg0_FLAGS_LowerRIV );
    }

    GET_SHARED_APP_CONTEXT( poAppContext );

    OEM_SECURE_ZERO_MEMORY( &cipherContext, SIZEOF( cipherContext ) );

    ChkDR( _GetDeviceECC256PubKey( &g_BBContext, &oPubKeyEncrypt,     DRM_BINARY_DEVICE_CERT_KEY_ENCRYPT ) );
    ChkDR( _GetDeviceECC256PubKey( &g_BBContext, &oPubKeyPrndEncrypt, DRM_BINARY_DEVICE_CERT_KEY_PRND_ENCRYPT ) );

    ChkDR( _GetRivAndCrl( poAppContext,
                          &pRIV,
                          &cbRIV,
                          poAppContext->pbRevocationBuffer,
                          poAppContext->cbRevocationBuffer ) );

    /* Allow the PrndEncrypt public be used as a content encryption public key for this test */
    ChkDR( OEM_HAL_TEST_AddPublicKeyUsage( _Hal_GetKeyType( &g_BBContext, oPubKeyPrndEncrypt.hPublicKey ),
                                           TEST_Hal_GetKeyRegisterIndex( &g_BBContext, oPubKeyPrndEncrypt.hPublicKey ),
                                           BCERT_KEYUSAGE_BIT( DRM_BCERT_KEYUSAGE_ENCRYPT_KEY ) ) );

    cbDeviceCert = MAX_DEVICE_CERT_SIZE;
    ChkMem( pbDeviceCert = (DRM_BYTE*)Oem_MemAlloc( cbDeviceCert ) );
    ChkDR( TST_UTL_GetPlayReadyCert( pbDeviceCert, &cbDeviceCert, &g_BBContext.CryptoContext ) );

    /* Generate random KID and LID */
    ChkDR( Oem_Random_GetBytes( NULL, (DRM_BYTE*)&oKid, SIZEOF( oKid ) ) );
    ChkDR( Oem_Random_GetBytes( NULL, (DRM_BYTE*)&oLid, SIZEOF( oLid ) ) );

    ChkDR( DRM_B64_EncodeA(
        (const DRM_BYTE*)&oKid,
        SIZEOF( oKid ),
        rgchKid,
       &cchKid,
        0 ) );
    rgchKid[ CCH_BASE64_EQUIV( SIZEOF( DRM_KID ) ) ] = g_chNull;

    ChkDR( DRM_B64_EncodeA(
        (const DRM_BYTE*)&oLid,
        SIZEOF( oLid ),
        rgchLid,
       &cchLid,
        0 ) );
    rgchLid[ CCH_BASE64_EQUIV( SIZEOF( DRM_LID ) ) ] = g_chNull;

    DRMCRT_memcpy( rgchKeySeed, szKeySeed, SIZEOF( rgchKeySeed) );
    DRMCRT_memcpy( rgchOmit, szOmit, SIZEOF( rgchOmit ) );
    DRMCRT_memcpy( rgchTrue, szTrue, SIZEOF( rgchTrue ) );
    DRMCRT_memcpy( rgchSL100, szSL100, SIZEOF( rgchSL100 ) );
    DRMCRT_memcpy( rgchRIV1, szRIV1, SIZEOF( rgchRIV1 ) );

    /* Generate random content */
    ChkDR( Oem_Random_GetBytes(
        g_BBContext.pOEMContext,
        rgbClearOrig,
        SIZEOF( rgbClearOrig ) ) );
    DRMCRT_memcpy( rgbClear, rgbClearOrig, SIZEOF( rgbClear ) );

    /* Create XMR License */
    argvInternal[0]  = rgchKid;     /* KID */
    argvInternal[1]  = rgchLid;     /* LID */
    argvInternal[2]  = rgchKeySeed; /* Key Seed */
    argvInternal[13] = rgchOmit;    /* Omit the copy right */
    argvInternal[37] = rgchRIV1;    /* RIV = 1 */

#if DRM_SUPPORT_SECUREOEMHAL
    /*
    ** Only cannot persist licenses can be used when SecureHal is enabled.
    */
    argvInternal[32] = rgchOmit;   /* Cannot Persist - Any string is OK because it's set to TRUE if non-NULL. */
#endif  /* DRM_SUPPORT_SECUREOEMHAL */

    ChkDR( Test_InitLicGenXMR() );
    ChkDR( Test_CreateXMRLicense( &pbLicense, &cbLicense, TRUE, argcInternal, argvInternal ) );

    oStack.cbStack = cbStack;
    oStack.pbStack = rgbStack;
    ChkDR( DRM_XMR_UnpackLicense( pbLicense, cbLicense, &oStack, &oXMRLicense ) );

    rgoBindingInfo[0].m_fXMRLicense                                 = TRUE;
    rgoBindingInfo[0].m_oChecksum.m_fIgnoreChecksum                 = TRUE;
    rgoBindingInfo[0].m_oChecksum.m_pbV1KID                         = NULL;
    rgoBindingInfo[0].m_oChecksum.m_cbChecksum                      = 0;
    rgoBindingInfo[0].m_BindingInfo.m_XMR.m_pxmrContentKey          = &oXMRLicense.containerOuter.containerKeys.ContentKey;
    rgoBindingInfo[0].m_BindingInfo.m_XMR.m_pxmrOptimizedContentKey = &oXMRLicense.containerOuter.containerKeys.OptimizedContentKey;
    rgoBindingInfo[0].m_BindingInfo.m_XMR.m_pxmrDeviceKey           = &oXMRLicense.containerOuter.containerKeys.DeviceKey;
    rgoBindingInfo[0].m_BindingInfo.m_XMR.m_pxmrDeviceEccKey        = &oXMRLicense.containerOuter.containerKeys.ECCKey;
    rgoBindingInfo[0].m_BindingInfo.m_XMR.m_pxmrDomainID            = &oXMRLicense.containerOuter.containerGlobalPolicies.DomainID;
    rgoBindingInfo[0].m_BindingInfo.m_XMR.m_pxmrSignature           = &oXMRLicense.containerOuter.signature;
    rgoBindingInfo[0].m_BindingInfo.m_XMR.m_pxmrLicense             = &oXMRLicense;

    /* Encrypt content using the key from the license */
    cbContentKeyPair = oXMRLicense.containerOuter.containerKeys.ContentKey.cbEncryptedKey;
    ChkMem( pbContentKeyPair = (DRM_BYTE*)Oem_MemAlloc( cbContentKeyPair ) );
    DRMCRT_memcpy( pbContentKeyPair, oXMRLicense.containerOuter.containerKeys.ContentKey.pbEncryptedKeyBuffer + oXMRLicense.containerOuter.containerKeys.ContentKey.iEncryptedKey, cbContentKeyPair );
    ChkDR( _Test_ECC256_DecryptKey(
        &g_BBContext,
        pbContentKeyPair,
        cbContentKeyPair,
        &oPubKeyEncrypt,
        DRM_BINARY_DEVICE_CERT_KEY_ENCRYPT,
        OEM_HAL_KEY_TYPE_AES_CTR_CONTENT,
        &hContentKey1 ) );

    /*
    ** We're not actually allowed to encrypt using an existing license key.
    ** Fortunately, AES CTR Encrypt and Decrypt are the same operation,
    ** so we'll just Decrypt with the existing license key to simulate encrypt.
    */
    ChkDR( DRM_BBX_CPHR_InitWithKeyHandle( &g_BBContext, &cipherContext, eDRM_AES_COUNTER_CIPHER, hContentKey1 ) );
    ZEROMEM( &oCtrCtx, SIZEOF( oCtrCtx ) );
    oCtrCtx.qwInitializationVector = qwIV;
    ChkDR( DRM_BBX_CPHR_Decrypt( &cipherContext, &oCtrCtx, SIZEOF( rgbClear ), rgbClear ) );
    DRM_BBX_CPHR_Uninitialize( &cipherContext );

#if DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY

    /* Rebinding the key isn't supported when SecureHalPlayOnly is enabled. */
    dr = DRM_BBX_PRND_Tx_RebindLicenseKeysToNewECCKey(
        &g_BBContext,
        rgoBindingInfo,
        &oPubKeyPrndEncrypt,
        pbDeviceCert,
        cbDeviceCert,
        poAppContext->pbRevocationBuffer,
        poAppContext->cbRevocationBuffer,
        pRIV,
        cbRIV,
        &hCI,
        &oCipherTextCICK );
    DRM_TEST_EXPECT_ERROR_AND_MAP_TO_SUCCESS( dr, DRM_E_NOTIMPL );

    /* Make return code from this function match that which occurs for !DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY */
    if( fArg0LowerSecurityLevel )
    {
        ChkDR( DRM_E_DEVICE_SECURITY_LEVEL_TOO_LOW );
    }
    else if( fArg0LowerRIV )
    {
        ChkDR( DRM_E_RIV_TOO_SMALL );
    }

#else   /* DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY */

    /* Rebind the key */
    ChkDR( DRM_BBX_PRND_Tx_RebindLicenseKeysToNewECCKey(
        &g_BBContext,
        rgoBindingInfo,
        &oPubKeyPrndEncrypt,
        pbDeviceCert,
        cbDeviceCert,
        poAppContext->pbRevocationBuffer,
        poAppContext->cbRevocationBuffer,
        pRIV,
        cbRIV,
        &hCI,
        &oCipherTextCICK ) );

    /*
    ** Ensure that we can/cannot sign using the key.
    ** When signing a license, the license must be valid.
    ** At this point, in order to perform a sign operation, we have to:
    **    1. construct a new, valid license with the newly encrypted key material oCipherTextCICK
    **       (i.e. the original content key encrypted with a different pubkey, i.e. rebound)
    **    2. sign that license
    */

    /*
    ** Create an unsigned 2nd XMR license (as described above),
    ** potentially with a lower security level and/or lower RIV (error-path tests).
    */
    argvInternal[39] = rgchTrue; /* Do not sign the license in the create function */
    if( fArg0LowerSecurityLevel )
    {
        argvInternal[12] = rgchSL100; /* 100 is less than the default value of 150 */
    }
    if( fArg0LowerRIV )
    {
        argvInternal[37] = NULL; /* use the default RIV of 0, which is less than the first license above */
    }
    ChkDR( Test_InitLicGenXMR() );
    ChkDR( Test_CreateXMRLicense( &pbLicense2, &cbLicense2, TRUE, argcInternal, argvInternal ) );

    /*
    ** Sign this 2nd license using the handle (hCI) to the AES integrity key (CICK) obtained during
    ** the rebind above.  It should fail if the SL and/or RIV is smaller than that in the 1st license.
    */
    ChkDR( DRM_BBX_PRND_Tx_SignWithAESOMAC( &g_BBContext,
                                             hCI,
                                             pbLicense2,
                                             ( cbLicense2 - cbSignatureObject ),
                                             rgbSignature ) );

    /*
    ** Decrypt the AESKeyPair
    ** We can't use DRM_BINARY_DEVICE_CERT_KEY_ENCRYPT / OEM_HAL_KEY_TYPE_AES_CTR_CONTENT
    ** (which would be ideal) because oPubKeyPrndEncrypt maps to a keyfile entry
    ** which is signed and has type OEM_HAL_KEY_TYPE_ECC_P256_MESSAGE_ENCRYPT.
    ** So, we have to decrypt into an ECB message encryption key...
    */
    ChkDR( _Test_ECC256_DecryptKey(
        &g_BBContext,
        (DRM_BYTE*)&oCipherTextCICK,
        cbCipherTextCICK,
        &oPubKeyPrndEncrypt,
        DRM_BINARY_DEVICE_CERT_KEY_PRND_ENCRYPT,
        OEM_HAL_KEY_TYPE_AES_ECB_MESSAGE_ENCRYPT,
        &hContentKey2 ) );

    /*
    ** Verify that the contentkey can be used for decryption.
    ** Now, we would normally call DRM_BBX_CPHR_InitWithKeyHandle + DRM_BBX_CPHR_Decrypt to decrypt and verify the content.
    ** However, hContentKey2 is of type OEM_HAL_KEY_TYPE_AES_ECB_MESSAGE_ENCRYPT which can't be used for "content" decryption.
    ** (Well, at least not directly using the functions above.)  Fortunately, we can work around that.
    ** We repurpose DRM_BBX_PRND_Tx_EncryptNonceWithAESECB to encrypt the IV with AES ECB and then XOR that result with
    ** the encrypted data.  This matches the AES CTR algorithm and thus gives us back the clear data which we can then verify.
    ** We can only do this because we hacked up the PRND message encryptioon key with OEM_HAL_TEST_AddPublicKeyUsage which
    ** allowed us to rebind to it in the first place - otherwise, DRM_BBX_PRND_Tx_RebindLicenseKeysToNewECCKey would have failed.
    */
    QWORD_TO_NETWORKBYTES( rgbWorking, 0, qwIV );
    ChkDR( DRM_BBX_PRND_Tx_EncryptNonceWithAESECB(
        &g_BBContext,
        hContentKey2,
        (DRM_ID*)rgbWorking ) );

    for( idx = 0; idx < DRM_AES_BLOCKLEN; idx++ )
    {
        rgbClear[idx] = (DRM_BYTE)( rgbClear[idx] ^ rgbWorking[idx] );
    }

    ChkBOOL( 0 == DRMCRT_memcmp( rgbClear, rgbClearOrig, SIZEOF( rgbClear ) ), DRM_E_TEST_UNEXPECTED_OUTPUT );

    /* Now let's verify that we CAN'T use DRM_BBX_CPHR_InitWithKeyHandle + DRM_BBX_CPHR_Decrypt because hContentKey2 is the wrong type. */
    ChkDR( DRM_BBX_CPHR_InitWithKeyHandle( &g_BBContext, &cipherContext, eDRM_AES_COUNTER_CIPHER, hContentKey2 ) );
    ZEROMEM( &oCtrCtx, SIZEOF( oCtrCtx ) );
    oCtrCtx.qwInitializationVector = qwIV;
    dr = DRM_BBX_CPHR_Decrypt( &cipherContext, &oCtrCtx, SIZEOF( rgbClear ), rgbClear );
    DRM_TEST_EXPECT_ERROR_AND_MAP_TO_SUCCESS( dr, DRM_E_OEMHAL_UNSUPPORTED_KEY_TYPE );
#endif  /* DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY */

ErrorExit:
    DRM_BBX_CleanupPublicKeyContext( &g_BBContext, &oPubKeyEncrypt );
    DRM_BBX_CleanupPublicKeyContext( &g_BBContext, &oPubKeyPrndEncrypt );

    DRM_BBX_SafeDestroySampleEncryptionContext( &hEncryptionContext );    /* returns void */
    DRM_BBX_CPHR_Uninitialize( &cipherContext );
    DRM_BBX_SafeReleaseKey( &g_BBContext, &hContentKey1 );
    DRM_BBX_SafeReleaseKey( &g_BBContext, &hContentKey2 );
    DRM_BBX_SafeReleaseKey( &g_BBContext, &hCI );

    SAFE_OEM_FREE( pbLicense );
#if !DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY
    SAFE_OEM_FREE( pbLicense2 );
#endif  /* !DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY */
    SAFE_OEM_FREE( pbDeviceCert );
    SAFE_OEM_FREE( pbContentKeyPair );
    SAFE_OEM_FREE( pRIV );

    return dr;
}
PREFAST_POP; /* __WARNING_USE_WIDE_API_25068 */

/*
** Structure to hold all the required keys and data for the Wrap and Unwrap tests
*/
typedef struct __tagWrapUnwrapData
{
    DRM_BB_CHECKSUM_CONTEXT DummyChecksum;

    DRM_BOOL                    fIgnoreChecksum;
    OEM_HAL_SLK_ENCRYPTION_TYPE slkEncryptionSymmetric;
    OEM_HAL_SLK_ENCRYPTION_TYPE slkEncryptionAsymmetric;
    DRM_DWORD                   dwCertKeyIndex;

    DRM_BYTE*           pbRootXmrLicense;
    DRM_DWORD           cbRootXmrLicense;
    DRM_XMR_UNWRAP_INFO RootUnwrapInfo;

    DRM_BYTE*           pbSimpleXmrLicenseAes;
    DRM_DWORD           cbSimpleXmrLicenseAes;
    DRM_XMR_UNWRAP_INFO SimpleUnwrapInfo;

    DRM_BYTE*           pbSimpleXmrLicenseLegacy;
    DRM_DWORD           cbSimpleXmrLicenseLegacy;
    DRM_XMR_UNWRAP_INFO SimpleLegacyUnwrapInfo;

    DRM_BYTE*           pbLeafXmrLicenseAes;
    DRM_DWORD           cbLeafXmrLicenseAes;
    DRM_XMR_UNWRAP_INFO LeafUnwrapInfo;

    DRM_BYTE*           pbLeafXmrLicenseLegacy;
    DRM_DWORD           cbLeafXmrLicenseLegacy;
    DRM_XMR_UNWRAP_INFO LegacyLeafUnwrapInfo;

    DRM_BYTE*           pbRootDomainXmrLicense;
    DRM_DWORD           cbRootDomainXmrLicense;
    DRM_XMR_UNWRAP_INFO RootDomainUnwrapInfo;

    DRM_BYTE*           pbSimpleDomainXmrLicense;
    DRM_DWORD           cbSimpleDomainXmrLicense;
    DRM_XMR_UNWRAP_INFO SimpleDomainUnwrapInfo;

    DRM_BYTE*           pbLegacyDomainXmrLicense;
    DRM_DWORD           cbLegacyDomainXmrLicense;
    DRM_XMR_UNWRAP_INFO LegacyDomainUnwrapInfo;

    /*
    ** good test certificate
    */
    DRM_BYTE*           pbDeviceCertChain;
    DRM_DWORD           cbDeviceCertChain;
    DRM_DWORD           iDeviceCertStart;
    DRM_DWORD           iDeviceCertUnsignedEnd;

    /*
    ** bad test certificate 1
    */
    DRM_BYTE*           pbDeviceCertChain_WithBadPubkeyInLeaf;
    DRM_DWORD           cbDeviceCertChain_WithBadPubkeyInLeaf;
    DRM_DWORD           iDeviceCertWithBadPubkeyStart;
    DRM_DWORD           iDeviceCertWithBadPubkeyUnsignedEnd;

    /*
    ** bad test certificate 2
    */
    DRM_BYTE*           pbDeviceCertChain_WithSwappedPubkeysInLeaf;
    DRM_DWORD           cbDeviceCertChain_WithSwappedPubkeysInLeaf;
    DRM_DWORD           iDeviceCertWithSwappedPubkeysStart;
    DRM_DWORD           iDeviceCertWithSwappedPubkeysUnsignedEnd;

    /*
    ** bad test certificate 3
    */
    DRM_BYTE*           pbDeviceCertChain_WithIncreasedSLInLeaf;
    DRM_DWORD           cbDeviceCertChain_WithIncreasedSLInLeaf;
    DRM_DWORD           iDeviceCertWithIncreasedSLStart;
    DRM_DWORD           iDeviceCertWithIncreasedSLUnsignedEnd;

    OEM_CRYPTO_HANDLE   rghWrappingKeys[ OEM_HAL_KEY_TYPE_NUM_VALID_TYPES ];

    /*
    ** Buffer to use as random 'encrypted key material' data
    */
    DRM_BYTE            RandomKeyData[ ECC_P256_CIPHERTEXT_SIZE_IN_BYTES ];

    DRM_ENCRYPTED_KEY   EncryptedEccKey;
    DRM_ENCRYPTED_KEY   EncryptedAesKey;

} WrapUnwrapData;

typedef enum
{
    eTEST_DATA_DUMMY                            = 0,
    eTEST_DATA_DUMMY_32                         = 1,
    eTEST_DATA_DRM_ENCRYPTED_KEY_16             = 2,
    eTEST_DATA_DRM_ENCRYPTED_KEY_32             = 3,
    eTEST_DATA_ENCRYPTED_BYTES_16               = 4,
    eTEST_DATA_ENCRYPTED_BYTES_32               = 5,
    eTEST_DATA_ENCRYPTED_BYTES_128              = 6,
    eTEST_DATA_XMR_LICENSE_ROOT                 = 7,
    eTEST_DATA_XMR_LICENSE_SIMPLE               = 8,
    eTEST_DATA_XMR_LICENSE_LEAF                 = 9,
    eTEST_DATA_XMR_LICENSE_LEGACY               = 10,
    eTEST_DATA_XMR_LICENSE_LEGACY_LEAF          = 11,
    eTEST_DATA_XMR_LICENSE_DOMAIN_ROOT          = 12,
    eTEST_DATA_XMR_LICENSE_DOMAIN_SIMPLE        = 13,
    eTEST_DATA_XMR_LICENSE_DOMAIN_LEGACY        = 14,
    eTEST_DATA_CERTIFICATE                      = 15,    /* good */
    eTEST_DATA_CERTIFICATE_WITH_BAD_PUBKEY      = 16,    /* bad  */
    eTEST_DATA_CERTIFICATE_WITH_SWAPPED_PUBKEYS = 17,    /* bad  */
    eTEST_DATA_CERTIFICATE_WITH_INCREASED_SL    = 18,    /* bad  */
    eTEST_DATA_SLK_SYMMETRIC                    = 19,
    eTEST_DATA_SLK_ASYMMETRIC                   = 20,
} eOemTestDataType;


PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_USE_WIDE_API_25068, "Ignore prefast warning about ANSI functions for this function.");
static DRM_RESULT _InitWrapUnwrapData(
    __inout_ecount( 1 ) DRM_BB_CONTEXT *f_poBBXContext,
    __inout_ecount( 1 ) WrapUnwrapData *f_pTestData )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_KID                              oKid                                                       = {0};
    DRM_LID                              oLid                                                       = {0};
    DRM_CHAR                             rgchKid[ CCH_BASE64_EQUIV( SIZEOF( DRM_KID ) ) + 1 ]       = {0};
    DRM_CHAR                             rgchRootKid[ CCH_BASE64_EQUIV( SIZEOF( DRM_KID ) ) + 1 ]   = {0};
    DRM_CHAR                             rgchLid[ CCH_BASE64_EQUIV( SIZEOF( DRM_LID ) ) + 1 ]       = {0};
    DRM_DWORD                            cchKid                                                     = CCH_BASE64_EQUIV( SIZEOF( DRM_KID ) );
    DRM_DWORD                            cchLid                                                     = CCH_BASE64_EQUIV( SIZEOF( DRM_LID ) );
    const DRM_LONG                       argcInternal                                               = 33;
    DRM_CHAR                            *argvInternal[33]                                           = { 0 };
    const DRM_CHAR                       szKeySeed[]                                                = "XVBovsmzhP9gRIZxWfFta3VVRPzVEWmJsazEJ46I";
    const DRM_CHAR                       szOmit[]                                                   = "OMIT";
    DRM_CHAR                             rgchKeySeed[SIZEOF(szKeySeed)]                             = {0};
    DRM_CHAR                             rgchOmit[SIZEOF(szOmit)]                                   = {0};
    DRM_BCERT_CHAIN_HEADER               oChainHeader                                               = {0};
    DRM_DWORD                            bLeafCertStart                                             = 0;
    DRM_DWORD                            iXmrCreateIteration                                        = 0;
    DRM_GUID                             guidKeyfile                                                = EMPTY_DRM_GUID;
    DRM_GUID                             guidModelCert                                              = EMPTY_DRM_GUID;
    DRM_BYTE                            *pRIV                                                       = NULL;
    DRM_DWORD                            cbRIV                                                      = 0;
    DRM_APP_CONTEXT_INTERNAL            *poAppContext                                               = NULL;
    DRM_PUBLIC_KEY_CONTEXT               dummyKeyContext                                            = { 0 };
    const DRM_CHAR                       szAlgorithmCocktail[]                                      = "COCKTAIL";
    const DRM_CHAR                       szAlgorithmAesEcb[]                                        = "AES_128_ECB";
    const DRM_CHAR                       szAlgorithmAesCtr[]                                        = "AES_128_CTR";
    DRM_CHAR                             rgchAlgorithmCocktail[SIZEOF(szAlgorithmCocktail)]         = {0};
    DRM_CHAR                             rgchAlgorithmAesEcb[SIZEOF(szAlgorithmAesEcb)]             = {0};
    DRM_CHAR                             rgchAlgorithmAesCtr[SIZEOF(szAlgorithmAesCtr)]             = {0};

    DRM_CHAR                             rgchDomainAccountID[ CCH_BASE64_EQUIV( SIZEOF( DRM_ID ) ) + 1 ]    = {0};
    DRM_DWORD                            cchDomainAccountID                                                 = CCH_BASE64_EQUIV( SIZEOF( DRM_ID ) );
    DRM_ID                               oDomainAccountID                                                   = {0};
    PUBKEY_P256                          oDomainPubKey                                                      = {0};
    DRM_DWORD                            cbDomainPubKey                                                     = SIZEOF( PUBKEY_P256 );
    DRM_CHAR                             rgchDomainPubKey[ CCH_BASE64_EQUIV( SIZEOF( PUBKEY_P256 ) ) + 1 ]  = {0};
    DRM_DWORD                            cchDomainPubKey                                                    = CCH_BASE64_EQUIV( SIZEOF( PUBKEY_P256 ) );

    GET_SHARED_APP_CONTEXT( poAppContext );

    /*
    ** Initialize everything to zero
    */
    OEM_SECURE_ZERO_MEMORY( f_pTestData, SIZEOF( WrapUnwrapData ) );

    if( !DRM_DEVICEASSETS_IsDeviceAssetsSupported() )
    {
        ChkDR( TST_UTL_SetPlayReadyCertCachedValues( &poAppContext->oBlackBoxContext ) );
    }
    
    /*
    ** Obtain a good cert chain with which to test
    */
    f_pTestData->cbDeviceCertChain = MAX_DEVICE_CERT_SIZE;
    ChkMem( f_pTestData->pbDeviceCertChain = (DRM_BYTE*)Oem_MemAlloc( f_pTestData->cbDeviceCertChain ) );
    ChkDR( TST_UTL_GetPlayReadyCert( f_pTestData->pbDeviceCertChain, &f_pTestData->cbDeviceCertChain, &f_poBBXContext->CryptoContext ) );
    ChkDR( DRM_BCert_GetChainHeader( f_pTestData->pbDeviceCertChain,
                                     f_pTestData->cbDeviceCertChain,
                                     &bLeafCertStart,
                                     &oChainHeader ) );
    f_pTestData->iDeviceCertStart = bLeafCertStart;
    f_pTestData->iDeviceCertUnsignedEnd = f_pTestData->cbDeviceCertChain - DRM_BCERT_SIGNATURE_OBJECT_LEN;
    
    /*
    ** Create a signing public key by parsing the good cert chain
    */
    ChkDR( _ResolveSigningChainKey( f_poBBXContext,
                                    f_pTestData->pbDeviceCertChain,
                                    f_pTestData->cbDeviceCertChain,
                                    oChainHeader.cCerts,
                                    1, /* resolve the leaf certificate parent signing key */
                                    DRM_BCERT_KEYUSAGE_UNKNOWN,
                                    bLeafCertStart,
                                    &f_pTestData->rghWrappingKeys[ OEM_HAL_KEY_TYPE_ECC_P256_PUBLIC ] ) );

    /*
    ** Create a model-sign negative test case "bad pubkey" cert chain from the good one
    */
    /* First, make a copy of the good cert chain */
    f_pTestData->cbDeviceCertChain_WithBadPubkeyInLeaf = f_pTestData->cbDeviceCertChain;
    ChkMem( f_pTestData->pbDeviceCertChain_WithBadPubkeyInLeaf = (DRM_BYTE*)Oem_MemAlloc( f_pTestData->cbDeviceCertChain_WithBadPubkeyInLeaf ) );
    MEMCPY( f_pTestData->pbDeviceCertChain_WithBadPubkeyInLeaf, f_pTestData->pbDeviceCertChain, f_pTestData->cbDeviceCertChain_WithBadPubkeyInLeaf );
    f_pTestData->iDeviceCertWithBadPubkeyStart       = f_pTestData->iDeviceCertStart;
    f_pTestData->iDeviceCertWithBadPubkeyUnsignedEnd = f_pTestData->iDeviceCertUnsignedEnd;   
    {
        /* Next, alter one of the pubkeys in the leaf */
        DRM_DWORD                       dwOffset                = f_pTestData->iDeviceCertWithBadPubkeyStart;
        DRM_BCERT_MINIMALDATA           oMinData                = {0};
        DRM_BCERT_VERIFICATIONCONTEXT   oVerificationContext    = {0};
     
        ChkDR( DRM_BCert_InitVerificationContext(
            NULL,
            NULL,
            0,
            NULL,
            FALSE,
            TRUE,
            NULL,
            0,
            FALSE,
            NULL,
            NULL,
            NULL,
            &oVerificationContext ) );
            
        ChkDR( DRM_BCert_ParseRawCertificate(
            f_pTestData->pbDeviceCertChain_WithBadPubkeyInLeaf,
            f_pTestData->cbDeviceCertChain_WithBadPubkeyInLeaf,
            &dwOffset,
            NULL,
            &oMinData,
            &oVerificationContext ) );

        /* pValue points to the key value in the cert buffer, it is not a local copy */
        oMinData.oKeyInfo.rgoKeys[0].pValue->m_rgbPubkey[0]++;
    }
    
    /*
    ** Create a model-sign negative test case "swapped pubkeys" cert chain from the good one
    */
    /* First, make a copy of the good cert chain */
    f_pTestData->cbDeviceCertChain_WithSwappedPubkeysInLeaf = f_pTestData->cbDeviceCertChain;
    ChkMem( f_pTestData->pbDeviceCertChain_WithSwappedPubkeysInLeaf = (DRM_BYTE*)Oem_MemAlloc( f_pTestData->cbDeviceCertChain_WithSwappedPubkeysInLeaf ) );
    MEMCPY( f_pTestData->pbDeviceCertChain_WithSwappedPubkeysInLeaf, f_pTestData->pbDeviceCertChain, f_pTestData->cbDeviceCertChain_WithSwappedPubkeysInLeaf );
    f_pTestData->iDeviceCertWithSwappedPubkeysStart       = f_pTestData->iDeviceCertStart;
    f_pTestData->iDeviceCertWithSwappedPubkeysUnsignedEnd = f_pTestData->iDeviceCertUnsignedEnd;    
    {
        /* Next, swap the data of two of the pubkeys in the leaf */
        DRM_DWORD                       dwOffset                = f_pTestData->iDeviceCertWithSwappedPubkeysStart;
        DRM_BCERT_MINIMALDATA           oMinData                = {0};
        DRM_BCERT_VERIFICATIONCONTEXT   oVerificationContext    = {0};
        DRM_BCERT_PUBKEY                oPubKeySwap             = {0};
     
        ChkDR( DRM_BCert_InitVerificationContext(
            NULL,
            NULL,
            0,
            NULL,
            FALSE,
            TRUE,
            NULL,
            0,
            FALSE,
            NULL,
            NULL,
            NULL,
            &oVerificationContext ) );
            
        ChkDR( DRM_BCert_ParseRawCertificate(
            f_pTestData->pbDeviceCertChain_WithSwappedPubkeysInLeaf,
            f_pTestData->cbDeviceCertChain_WithSwappedPubkeysInLeaf,
            &dwOffset,
            NULL,
            &oMinData,
            &oVerificationContext ) );

        /* pValue points to the key value in the cert buffer, it is not a local copy */
        MEMCPY( (DRM_BYTE *)&oPubKeySwap,                        (DRM_BYTE *)oMinData.oKeyInfo.rgoKeys[0].pValue, SIZEOF( DRM_BCERT_PUBKEY ) );
        MEMCPY( (DRM_BYTE *)oMinData.oKeyInfo.rgoKeys[0].pValue, (DRM_BYTE *)oMinData.oKeyInfo.rgoKeys[1].pValue, SIZEOF( DRM_BCERT_PUBKEY ) );
        MEMCPY( (DRM_BYTE *)oMinData.oKeyInfo.rgoKeys[1].pValue, (DRM_BYTE *)&oPubKeySwap,                        SIZEOF( DRM_BCERT_PUBKEY ) );
    }
    
    /*
    ** Create a model-sign negative test case "increased security level" cert chain from the good one
    */
    /* First, make a copy of the good cert chain */
    f_pTestData->cbDeviceCertChain_WithIncreasedSLInLeaf = f_pTestData->cbDeviceCertChain;
    ChkMem( f_pTestData->pbDeviceCertChain_WithIncreasedSLInLeaf = (DRM_BYTE*)Oem_MemAlloc( f_pTestData->cbDeviceCertChain_WithIncreasedSLInLeaf ) );
    MEMCPY( f_pTestData->pbDeviceCertChain_WithIncreasedSLInLeaf, f_pTestData->pbDeviceCertChain, f_pTestData->cbDeviceCertChain_WithIncreasedSLInLeaf );
    f_pTestData->iDeviceCertWithIncreasedSLStart       = f_pTestData->iDeviceCertStart;
    f_pTestData->iDeviceCertWithIncreasedSLUnsignedEnd = f_pTestData->iDeviceCertUnsignedEnd;    
    {
        /* Next, increase the security level of the leaf cert */
        DRM_DWORD                       dwOffset                = f_pTestData->iDeviceCertWithIncreasedSLStart;
        DRM_BCERT_HEADER                oCertHeader             = {0};
        DRM_BCERT_VERIFICATIONCONTEXT   oVerificationContext    = {0};
        DRM_BCERT_OBJECT_HEADER         oObjHeader              = {0};
        
        ChkDR( DRM_BCert_InitVerificationContext(
            NULL,
            NULL,
            0,
            NULL,
            FALSE,
            TRUE,
            NULL,
            0,
            FALSE,
            NULL,
            NULL,
            NULL,
            &oVerificationContext ) );
            
        ChkDR( DRM_BCert_parseCertHeader( f_pTestData->pbDeviceCertChain_WithIncreasedSLInLeaf,
                                          f_pTestData->cbDeviceCertChain_WithIncreasedSLInLeaf,
                                          &dwOffset,
                                          &oCertHeader,
                                          &oVerificationContext ) );
                                          
        /* Basic info always follows certificate header */
        ChkDR( DRM_BCert_getObjectHeader( f_pTestData->pbDeviceCertChain_WithIncreasedSLInLeaf,
                                           f_pTestData->cbDeviceCertChain_WithIncreasedSLInLeaf,
                                           &dwOffset,
                                           &oObjHeader ) );

       /* Skip over the certificate ID */
       dwOffset += DRM_BCERT_CERT_ID_LENGTH;

       /* dwOffset should now be pointing to the security level DWORD; increase its value */
       *((DRM_DWORD *)(f_pTestData->pbDeviceCertChain_WithIncreasedSLInLeaf + dwOffset)) += 100;
    }
    
    /*
    ** Generate key types where this is available
    */
    {
        DRM_BB_KEY_REGISTER_ENTRY *pKeyEntry      = NULL;
        DRM_BOOL                   fCachedHandle  = FALSE;

        ChkDR( _Hal_AllocateKeyHandle( &g_BBContext, OEM_HAL_KEY_TYPE_AES_ECB_ROOT, NULL, 0, &f_pTestData->rghWrappingKeys[OEM_HAL_KEY_TYPE_AES_ECB_ROOT], &pKeyEntry, &fCachedHandle ) );
        ChkDR( OEM_HAL_TEST_GenerateAes128KeyPair( f_poBBXContext->pOEMContext,
                                                   &f_poBBXContext->CryptoContext,
                                                   OEM_HAL_KEY_TYPE_AES_ECB_ROOT,
                                                   pKeyEntry->indexKeyRegister ) );

        ChkDR( _Hal_AllocateKeyHandle( &g_BBContext, OEM_HAL_KEY_TYPE_AES_CTR_CONTENT, NULL, 0, &f_pTestData->rghWrappingKeys[OEM_HAL_KEY_TYPE_AES_CTR_CONTENT], &pKeyEntry, &fCachedHandle ) );
        ChkDR( OEM_HAL_TEST_GenerateAes128KeyPair( f_poBBXContext->pOEMContext,
                                                   &f_poBBXContext->CryptoContext,
                                                   OEM_HAL_KEY_TYPE_AES_CTR_CONTENT,
                                                   pKeyEntry->indexKeyRegister ) );

        ChkDR( _Hal_AllocateKeyHandle( &g_BBContext, OEM_HAL_KEY_TYPE_AES_ECB_DOMAIN_PROTECT, NULL, 0, &f_pTestData->rghWrappingKeys[OEM_HAL_KEY_TYPE_AES_ECB_DOMAIN_PROTECT], &pKeyEntry, &fCachedHandle ) );
        ChkDR( OEM_HAL_TEST_GenerateAes128KeyPair( g_BBContext.pOEMContext,
                                                   &g_BBContext.CryptoContext,
                                                   OEM_HAL_KEY_TYPE_AES_ECB_DOMAIN_PROTECT,
                                                   pKeyEntry->indexKeyRegister ) );

#if !DRM_SUPPORT_SECUREOEMHAL
            ChkDR( _Hal_AllocateKeyHandle( &g_BBContext, OEM_HAL_KEY_TYPE_LEGACY, NULL, 0, &f_pTestData->rghWrappingKeys[OEM_HAL_KEY_TYPE_LEGACY], &pKeyEntry, &fCachedHandle ) );
            ChkDR( OEM_HAL_TEST_GenerateAes128Key( g_BBContext.pOEMContext,
                                                   &g_BBContext.CryptoContext,
                                                   OEM_HAL_KEY_TYPE_LEGACY,
                                                   pKeyEntry->indexKeyRegister ) );
#else /* !DRM_SUPPORT_SECUREOEMHAL */
            dr = _Hal_AllocateKeyHandle( &g_BBContext, OEM_HAL_KEY_TYPE_LEGACY, NULL, 0, &f_pTestData->rghWrappingKeys[OEM_HAL_KEY_TYPE_LEGACY], &pKeyEntry, &fCachedHandle );

            DRM_TEST_EXPECT_ERROR_AND_MAP_TO_SUCCESS( dr, DRM_E_OEMHAL_UNSUPPORTED_KEY_TYPE );

#endif /* !DRM_SUPPORT_SECUREOEMHAL */

        if( DRM_BBX_IsSlkCryptoSupported() )
        {
            ChkDR( _Hal_AllocateKeyHandle( &g_BBContext, OEM_HAL_KEY_TYPE_AES_SLK, NULL, 0, &f_pTestData->rghWrappingKeys[OEM_HAL_KEY_TYPE_AES_SLK], &pKeyEntry, &fCachedHandle ) );
            ChkDR( OEM_HAL_TEST_GenerateAes128Key( g_BBContext.pOEMContext,
                                                   &g_BBContext.CryptoContext,
                                                   OEM_HAL_KEY_TYPE_AES_SLK,
                                                   pKeyEntry->indexKeyRegister ) );
        }
        else
        {
            dr = _Hal_AllocateKeyHandle( &g_BBContext, OEM_HAL_KEY_TYPE_AES_SLK, NULL, 0, &f_pTestData->rghWrappingKeys[OEM_HAL_KEY_TYPE_AES_SLK], &pKeyEntry, &fCachedHandle );
            DRM_TEST_EXPECT_ERROR_AND_MAP_TO_SUCCESS( dr, DRM_E_NOTIMPL );
        }
    }

#if !DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY
    ChkDR( _Hal_GenerateKey( f_poBBXContext,
                             OEM_HAL_KEY_TYPE_AES_ECB_NEW_LICENSE_ROOT,
                             150,    /* Security level 150 for testing */
                             &f_pTestData->rghWrappingKeys[OEM_HAL_KEY_TYPE_AES_ECB_NEW_LICENSE_ROOT] ) );
    ChkDR( _Hal_GenerateKey( f_poBBXContext,
                             OEM_HAL_KEY_TYPE_AES_CTR_NEW_LICENSE_CONTENT,
                             150,    /* Security level 150 for testing */
                             &f_pTestData->rghWrappingKeys[OEM_HAL_KEY_TYPE_AES_CTR_NEW_LICENSE_CONTENT] ) );
    ChkDR( _Hal_GenerateKey( f_poBBXContext,
                             OEM_HAL_KEY_TYPE_AES_ECB_MESSAGE_ENCRYPT,
                             0,
                             &f_pTestData->rghWrappingKeys[OEM_HAL_KEY_TYPE_AES_ECB_MESSAGE_ENCRYPT] ) );
    ChkDR( _Hal_GenerateKey( f_poBBXContext,
                             OEM_HAL_KEY_TYPE_AES_GENERIC_KEY_CONTENT,
                             0,
                             &f_pTestData->rghWrappingKeys[OEM_HAL_KEY_TYPE_AES_GENERIC_KEY_CONTENT] ) );
    ChkDR( _Hal_GenerateKey( f_poBBXContext,
                             OEM_HAL_KEY_TYPE_AES_GENERIC_KEY_ROOT,
                             0,
                             &f_pTestData->rghWrappingKeys[OEM_HAL_KEY_TYPE_AES_GENERIC_KEY_ROOT] ) );
#else   /* !DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY */
    dr = _Hal_GenerateKey( f_poBBXContext,
                           OEM_HAL_KEY_TYPE_AES_ECB_NEW_LICENSE_ROOT,
                           150,    /* Security level 150 for testing */
                           &f_pTestData->rghWrappingKeys[OEM_HAL_KEY_TYPE_AES_ECB_NEW_LICENSE_ROOT] );
    DRM_TEST_EXPECT_ERROR_AND_MAP_TO_SUCCESS( dr, DRM_E_OEMHAL_UNSUPPORTED_KEY_TYPE );
    dr = _Hal_GenerateKey( f_poBBXContext,
                           OEM_HAL_KEY_TYPE_AES_CTR_NEW_LICENSE_CONTENT,
                           150,    /* Security level 150 for testing */
                           &f_pTestData->rghWrappingKeys[OEM_HAL_KEY_TYPE_AES_CTR_NEW_LICENSE_CONTENT] );
    DRM_TEST_EXPECT_ERROR_AND_MAP_TO_SUCCESS( dr, DRM_E_OEMHAL_UNSUPPORTED_KEY_TYPE );
    dr = _Hal_GenerateKey( f_poBBXContext,
                           OEM_HAL_KEY_TYPE_AES_ECB_MESSAGE_ENCRYPT,
                           0,
                           &f_pTestData->rghWrappingKeys[OEM_HAL_KEY_TYPE_AES_ECB_MESSAGE_ENCRYPT] );
    DRM_TEST_EXPECT_ERROR_AND_MAP_TO_SUCCESS( dr, DRM_E_OEMHAL_UNSUPPORTED_KEY_TYPE );
    dr = _Hal_GenerateKey( f_poBBXContext,
                           OEM_HAL_KEY_TYPE_AES_GENERIC_KEY_CONTENT,
                           0,
                           &f_pTestData->rghWrappingKeys[OEM_HAL_KEY_TYPE_AES_GENERIC_KEY_CONTENT] );
    DRM_TEST_EXPECT_ERROR_AND_MAP_TO_SUCCESS( dr, DRM_E_OEMHAL_UNSUPPORTED_KEY_TYPE );
    dr = _Hal_GenerateKey( f_poBBXContext,
                           OEM_HAL_KEY_TYPE_AES_GENERIC_KEY_ROOT,
                           0,
                           &f_pTestData->rghWrappingKeys[OEM_HAL_KEY_TYPE_AES_GENERIC_KEY_ROOT] );
    DRM_TEST_EXPECT_ERROR_AND_MAP_TO_SUCCESS( dr, DRM_E_OEMHAL_UNSUPPORTED_KEY_TYPE );
#endif  /* !DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY */

    ChkDR( _Hal_GenerateKey( f_poBBXContext,
                             OEM_HAL_KEY_TYPE_AES_OMAC1_SECURE_STORE,
                             0,
                             &f_pTestData->rghWrappingKeys[OEM_HAL_KEY_TYPE_AES_OMAC1_SECURE_STORE] ) );
    {
        DRM_BB_KEY_REGISTER_ENTRY *pKeyEntry      = NULL;
        DRM_BOOL                   fCachedHandle  = FALSE;
        ChkDR( _Hal_AllocateKeyHandle( &g_BBContext, OEM_HAL_KEY_TYPE_ECC_P256_DOMAIN_ENCRYPT, NULL, 0, &f_pTestData->rghWrappingKeys[OEM_HAL_KEY_TYPE_ECC_P256_DOMAIN_ENCRYPT], &pKeyEntry, &fCachedHandle ) );
        ChkDR( OEM_HAL_TEST_GenerateEcc256KeyPair( &f_poBBXContext->CryptoContext,
                                                   OEM_HAL_KEY_TYPE_ECC_P256_DOMAIN_ENCRYPT,
                                                   pKeyEntry->indexKeyRegister ) );
    }

    /*
    ** Grab the domain pubkey so we can bind licenses to it
    */
    ChkDR( _Hal_WrapKey( f_poBBXContext,
                         f_pTestData->rghWrappingKeys[OEM_HAL_KEY_TYPE_ECC_P256_DOMAIN_ENCRYPT],
                         OEM_CRYPTO_HANDLE_INVALID,
                         (DRM_BYTE*)&oDomainPubKey,
                         &cbDomainPubKey ) );
    ChkDR( DRM_B64_EncodeA(
        (const DRM_BYTE*)&oDomainPubKey,
        SIZEOF( PUBKEY_P256 ),
        rgchDomainPubKey,
       &cchDomainPubKey,
        0 ) );
    rgchDomainPubKey[ CCH_BASE64_EQUIV( SIZEOF( PUBKEY_P256 ) ) ] = g_chNull;

    /*
    ** Create XMR licenses for the various Content, Root and Legacy keys
    */
    ChkDR( Test_InitLicGenXMR() );

    DRMCRT_memcpy( rgchKeySeed, szKeySeed, SIZEOF( rgchKeySeed) );
    DRMCRT_memcpy( rgchOmit, szOmit, SIZEOF( rgchOmit ) );
    DRMCRT_memcpy( rgchAlgorithmCocktail, szAlgorithmCocktail, SIZEOF( rgchAlgorithmCocktail ) );
    DRMCRT_memcpy( rgchAlgorithmAesEcb, szAlgorithmAesEcb, SIZEOF( rgchAlgorithmAesEcb ) );
    DRMCRT_memcpy( rgchAlgorithmAesCtr, szAlgorithmAesCtr, SIZEOF( rgchAlgorithmAesCtr ) );

    /* Base XMR License fields */
    argvInternal[0] = rgchKid;                                                  /* KID */
    argvInternal[1] = rgchLid;                                                  /* LID */
    argvInternal[2] = rgchKeySeed;                                              /* Key Seed */
    argvInternal[13] = rgchOmit;                                                /* Omit the copy right */

    for( iXmrCreateIteration = 0; iXmrCreateIteration < 8; iXmrCreateIteration++ )
    {
        argvInternal[4]  = NULL;
        argvInternal[17] = NULL;
        argvInternal[24] = NULL;

        /* Generate random KID, LID, DOMID */
        ChkDR( Oem_Random_GetBytes( NULL, (DRM_BYTE*)&oKid, SIZEOF( oKid ) ) );
        ChkDR( Oem_Random_GetBytes( NULL, (DRM_BYTE*)&oLid, SIZEOF( oLid ) ) );
        ChkDR( Oem_Random_GetBytes( NULL, (DRM_BYTE*)&oDomainAccountID, SIZEOF( oDomainAccountID ) ) );

        ChkDR( DRM_B64_EncodeA(
            (const DRM_BYTE*)&oKid,
            SIZEOF( oKid ),
            rgchKid,
           &cchKid,
            0 ) );
        rgchKid[ CCH_BASE64_EQUIV( SIZEOF( DRM_KID ) ) ] = g_chNull;

        ChkDR( DRM_B64_EncodeA(
            (const DRM_BYTE*)&oLid,
            SIZEOF( oLid ),
            rgchLid,
           &cchLid,
            0 ) );
        rgchLid[ CCH_BASE64_EQUIV( SIZEOF( DRM_LID ) ) ] = g_chNull;

        ChkDR( DRM_B64_EncodeA(
            (const DRM_BYTE*)&oDomainAccountID,
            SIZEOF( oDomainAccountID ),
            rgchDomainAccountID,
           &cchDomainAccountID,
            0 ) );
        rgchLid[ CCH_BASE64_EQUIV( SIZEOF( DRM_ID ) ) ] = g_chNull;

#if DRM_SUPPORT_SECUREOEMHAL
        /*
        ** Only cannot persist licenses can be used when SecureHal is enabled.
        */
        argvInternal[32]          = rgchOmit;   /* Cannot Persist - Any string is OK because it's set to TRUE if non-NULL. */
#endif  /* DRM_SUPPORT_SECUREOEMHAL */

        switch( iXmrCreateIteration )
        {
        case 0:
            argvInternal[15]          = rgchAlgorithmAesEcb;

            ChkDR( Test_CreateXMRLicense( &f_pTestData->pbRootXmrLicense, &f_pTestData->cbRootXmrLicense, TRUE, argcInternal, argvInternal ) );

            f_pTestData->RootUnwrapInfo.pbXMRLic   = f_pTestData->pbRootXmrLicense;
            f_pTestData->RootUnwrapInfo.cbXMRLic   = f_pTestData->cbRootXmrLicense;
            f_pTestData->RootUnwrapInfo.poChecksum = &f_pTestData->DummyChecksum;

            OEM_SECURE_MEMCPY( rgchRootKid, rgchKid, cchKid );

            break;

        case 1:
            argvInternal[15]          = rgchAlgorithmAesCtr;

            ChkDR( Test_CreateXMRLicense( &f_pTestData->pbSimpleXmrLicenseAes, &f_pTestData->cbSimpleXmrLicenseAes, TRUE, argcInternal, argvInternal ) );

            f_pTestData->SimpleUnwrapInfo.pbXMRLic   = f_pTestData->pbSimpleXmrLicenseAes;
            f_pTestData->SimpleUnwrapInfo.cbXMRLic   = f_pTestData->cbSimpleXmrLicenseAes;
            f_pTestData->SimpleUnwrapInfo.poChecksum = &f_pTestData->DummyChecksum;
            break;

        case 2:
            argvInternal[15]            = rgchAlgorithmCocktail;

            ChkDR( Test_CreateXMRLicense( &f_pTestData->pbSimpleXmrLicenseLegacy, &f_pTestData->cbSimpleXmrLicenseLegacy, TRUE, argcInternal, argvInternal ) );

            f_pTestData->SimpleLegacyUnwrapInfo.pbXMRLic   = f_pTestData->pbSimpleXmrLicenseLegacy;
            f_pTestData->SimpleLegacyUnwrapInfo.cbXMRLic   = f_pTestData->cbSimpleXmrLicenseLegacy;
            f_pTestData->SimpleLegacyUnwrapInfo.poChecksum = &f_pTestData->DummyChecksum;
            break;

        case 3:
            argvInternal[4]           = rgchRootKid;
            argvInternal[15]          = rgchAlgorithmAesCtr;

            ChkDR( Test_CreateXMRLicense( &f_pTestData->pbLeafXmrLicenseAes, &f_pTestData->cbLeafXmrLicenseAes, TRUE, argcInternal, argvInternal ) );

            f_pTestData->LeafUnwrapInfo.pbXMRLic   = f_pTestData->pbLeafXmrLicenseAes;
            f_pTestData->LeafUnwrapInfo.cbXMRLic   = f_pTestData->cbLeafXmrLicenseAes;
            f_pTestData->LeafUnwrapInfo.poChecksum = &f_pTestData->DummyChecksum;
            break;

        case 4:
            argvInternal[4]           = rgchRootKid;
            argvInternal[15]          = rgchAlgorithmCocktail;

            ChkDR( Test_CreateXMRLicense( &f_pTestData->pbLeafXmrLicenseLegacy, &f_pTestData->cbLeafXmrLicenseLegacy, TRUE, argcInternal, argvInternal ) );

            f_pTestData->LegacyLeafUnwrapInfo.pbXMRLic   = f_pTestData->pbLeafXmrLicenseLegacy;
            f_pTestData->LegacyLeafUnwrapInfo.cbXMRLic   = f_pTestData->cbLeafXmrLicenseLegacy;
            f_pTestData->LegacyLeafUnwrapInfo.poChecksum = &f_pTestData->DummyChecksum;
            break;

        case 5:
            if( DRM_DOMAIN_IsDomainSupported() )
            {
                argvInternal[15]          = rgchAlgorithmAesEcb;
                argvInternal[17]          = rgchDomainAccountID;
                argvInternal[24]          = rgchDomainPubKey;

                ChkDR( Test_CreateXMRLicense( &f_pTestData->pbRootDomainXmrLicense, &f_pTestData->cbRootDomainXmrLicense, TRUE, argcInternal, argvInternal ) );

                f_pTestData->RootDomainUnwrapInfo.pbXMRLic   = f_pTestData->pbRootDomainXmrLicense;
                f_pTestData->RootDomainUnwrapInfo.cbXMRLic   = f_pTestData->cbRootDomainXmrLicense;
                f_pTestData->RootDomainUnwrapInfo.poChecksum = &f_pTestData->DummyChecksum;
            }
            break;

        case 6:
            if( DRM_DOMAIN_IsDomainSupported() )
            {
                argvInternal[15]          = rgchAlgorithmAesCtr;
                argvInternal[17]          = rgchDomainAccountID;
                argvInternal[24]          = rgchDomainPubKey;

                ChkDR( Test_CreateXMRLicense( &f_pTestData->pbSimpleDomainXmrLicense, &f_pTestData->cbSimpleDomainXmrLicense, TRUE, argcInternal, argvInternal ) );

                f_pTestData->SimpleDomainUnwrapInfo.pbXMRLic   = f_pTestData->pbSimpleDomainXmrLicense;
                f_pTestData->SimpleDomainUnwrapInfo.cbXMRLic   = f_pTestData->cbSimpleDomainXmrLicense;
                f_pTestData->SimpleDomainUnwrapInfo.poChecksum = &f_pTestData->DummyChecksum;
            }
            break;

        case 7:
            if( DRM_DOMAIN_IsDomainSupported() )
            {
                argvInternal[15]          = rgchAlgorithmCocktail;
                argvInternal[17]          = rgchDomainAccountID;
                argvInternal[24]          = rgchDomainPubKey;

                ChkDR( Test_CreateXMRLicense( &f_pTestData->pbLegacyDomainXmrLicense, &f_pTestData->cbLegacyDomainXmrLicense, TRUE, argcInternal, argvInternal ) );

                f_pTestData->LegacyDomainUnwrapInfo.pbXMRLic   = f_pTestData->pbLegacyDomainXmrLicense;
                f_pTestData->LegacyDomainUnwrapInfo.cbXMRLic   = f_pTestData->cbLegacyDomainXmrLicense;
                f_pTestData->LegacyDomainUnwrapInfo.poChecksum = &f_pTestData->DummyChecksum;
            }
            break;

        default:
            ChkDR( DRM_E_LOGICERR );
            break;
        }
    }

    f_pTestData->fIgnoreChecksum         = TRUE;
    f_pTestData->slkEncryptionSymmetric  = OEM_HAL_SLK_ENCRYPTION_TYPE_DERIVED_SYMMETRIC;
    f_pTestData->slkEncryptionAsymmetric = OEM_HAL_SLK_ENCRYPTION_TYPE_ASYMMETRIC;
    f_pTestData->dwCertKeyIndex          = 0;

    /*
    ** Use default keys where they are available
    */
    ChkDR( _Hal_GetPreloadedKey( f_poBBXContext,
                                 OEM_HAL_KEY_TYPE_AES_KEYFILE,
                                 &guidKeyfile,
                                 &f_pTestData->rghWrappingKeys[OEM_HAL_KEY_TYPE_AES_KEYFILE] ) );

    ChkDR( _Hal_GetPreloadedKey( f_poBBXContext,
                                 OEM_HAL_KEY_TYPE_ECC_P256_MODEL_SIGN,
                                 &guidModelCert,
                                 &f_pTestData->rghWrappingKeys[OEM_HAL_KEY_TYPE_ECC_P256_MODEL_SIGN] ) );

    /*
    ** Get device fixed keypairs
    */
    ChkDR( _tGetDeviceECC256KeyPair( f_poBBXContext,
                                    NULL,
                                    NULL,
                                    &f_pTestData->rghWrappingKeys[OEM_HAL_KEY_TYPE_ECC_P256_CONTENT_ENCRYPT],
                                    DRM_BINARY_DEVICE_CERT_KEY_ENCRYPT ) );

    ChkDR( _tGetDeviceECC256KeyPair( f_poBBXContext,
                                    NULL,
                                    NULL,
                                    &f_pTestData->rghWrappingKeys[OEM_HAL_KEY_TYPE_ECC_P256_MESSAGE_ENCRYPT],
                                    DRM_BINARY_DEVICE_CERT_KEY_PRND_ENCRYPT ) );

    ChkDR( _tGetDeviceECC256KeyPair( f_poBBXContext,
                                    NULL,
                                    NULL,
                                    &f_pTestData->rghWrappingKeys[OEM_HAL_KEY_TYPE_ECC_P256_SIGN],
                                    DRM_BINARY_DEVICE_CERT_KEY_SIGN ) );

    /*
    ** Fill a random buffer for our key data
    */
    ChkDR( Oem_Random_GetBytes( NULL, f_pTestData->RandomKeyData, SIZEOF( f_pTestData->RandomKeyData ) ) );

    /*
    ** Populate the DRM_ENCRYPTED_KEY objects
    */
    f_pTestData->EncryptedEccKey.wKeypairDecryptionMethod    = KF_DECRYPTION_TYPE_AES_CBC;
    f_pTestData->EncryptedEccKey.wKeySize                    = SIZEOF( PRIVKEY_P256 ) * BITS_PER_BYTE;
    ChkDR( Oem_Random_GetBytes( NULL, f_pTestData->EncryptedEccKey.rgbInitializationVector, SIZEOF( f_pTestData->EncryptedEccKey.rgbInitializationVector ) ) );
    f_pTestData->EncryptedEccKey.pbEncryptedKey              = f_pTestData->RandomKeyData;
    f_pTestData->EncryptedEccKey.cbEncryptedKey              = SIZEOF( PRIVKEY_P256 );
    ChkDR( Oem_Random_GetBytes( NULL, ( DRM_BYTE* )&f_pTestData->EncryptedEccKey.guidKeypairDecryptionKeyID, SIZEOF( f_pTestData->EncryptedEccKey.guidKeypairDecryptionKeyID ) ) );
    f_pTestData->EncryptedEccKey.wReserved                   = 0;

    f_pTestData->EncryptedAesKey.wKeypairDecryptionMethod    = KF_DECRYPTION_TYPE_AES_CBC;
    f_pTestData->EncryptedAesKey.wKeySize                    = DRM_AES_KEYSIZE_128 * BITS_PER_BYTE;
    ChkDR( Oem_Random_GetBytes( NULL, f_pTestData->EncryptedAesKey.rgbInitializationVector, SIZEOF( f_pTestData->EncryptedAesKey.rgbInitializationVector ) ) );
    f_pTestData->EncryptedAesKey.pbEncryptedKey              = f_pTestData->RandomKeyData;
    f_pTestData->EncryptedAesKey.cbEncryptedKey              = DRM_AES_KEYSIZE_128;
    ChkDR( Oem_Random_GetBytes( NULL, ( DRM_BYTE* )&f_pTestData->EncryptedAesKey.guidKeypairDecryptionKeyID, SIZEOF( f_pTestData->EncryptedAesKey.guidKeypairDecryptionKeyID ) ) );
    f_pTestData->EncryptedAesKey.wReserved                   = 0;

    /*
    ** Now, make sure there is valid revocation info available.
    */

    /*
    ** Set dummyKeyContext.hPublicKey to a valid key handle to pass the cursory check that it is assigned.
    */
    dummyKeyContext.hPublicKey = f_pTestData->rghWrappingKeys[OEM_HAL_KEY_TYPE_ECC_P256_MODEL_SIGN];

    ChkDR( _GetRivAndCrl( poAppContext,
                          &pRIV,
                          &cbRIV,
                          poAppContext->pbRevocationBuffer,
                          poAppContext->cbRevocationBuffer ) );

    ChkDR( _ValidatePublicKey( &g_BBContext,
                               &dummyKeyContext,
                               NULL,
                               0,
                               poAppContext->pbRevocationBuffer,
                               poAppContext->cbRevocationBuffer,
                               pRIV,
                               cbRIV ) );


ErrorExit:

    SAFE_OEM_FREE( pRIV );

    return dr;
}
PREFAST_POP; /* __WARNING_USE_WIDE_API_25068 */


static DRM_RESULT _RegenerateEccKeyPairs(
    __inout_ecount( 1 ) DRM_BB_CONTEXT *f_poBBXContext,
    __inout_ecount( 1 ) WrapUnwrapData *f_pTestData )
{
    DRM_RESULT dr = DRM_SUCCESS;

    /*
    ** Release the device cert private keys and generate some new ones, which will have a public key component
    */
    DRM_BBX_SafeReleaseKey( f_poBBXContext, &f_pTestData->rghWrappingKeys[OEM_HAL_KEY_TYPE_ECC_P256_CONTENT_ENCRYPT] );
    DRM_BBX_SafeReleaseKey( f_poBBXContext, &f_pTestData->rghWrappingKeys[OEM_HAL_KEY_TYPE_ECC_P256_MESSAGE_ENCRYPT] );
    DRM_BBX_SafeReleaseKey( f_poBBXContext, &f_pTestData->rghWrappingKeys[OEM_HAL_KEY_TYPE_ECC_P256_SIGN] );

    ChkDR( _Hal_GenerateKey( f_poBBXContext,
                             OEM_HAL_KEY_TYPE_ECC_P256_CONTENT_ENCRYPT,
                             0,
                             &f_pTestData->rghWrappingKeys[OEM_HAL_KEY_TYPE_ECC_P256_CONTENT_ENCRYPT] ) );
    ChkDR( _Hal_GenerateKey( f_poBBXContext,
                             OEM_HAL_KEY_TYPE_ECC_P256_MESSAGE_ENCRYPT,
                             0,
                             &f_pTestData->rghWrappingKeys[OEM_HAL_KEY_TYPE_ECC_P256_MESSAGE_ENCRYPT] ) );
    ChkDR( _Hal_GenerateKey( f_poBBXContext,
                             OEM_HAL_KEY_TYPE_ECC_P256_SIGN,
                             0,
                             &f_pTestData->rghWrappingKeys[OEM_HAL_KEY_TYPE_ECC_P256_SIGN] ) );

ErrorExit:

    return dr;
}


static DRM_RESULT _GetUnwrapData(
    __inout_ecount( 1 )   WrapUnwrapData    *f_pTestData,
    __in                  eOemTestDataType   f_eTestDataType,
    __out_ecount(1) const DRM_BYTE         **f_ppbUnwrapData,
    __out_ecount(1)       DRM_DWORD         *f_pcbUnwrapData,
    __out_ecount(1) const DRM_BYTE         **f_ppbParamData,
    __out_ecount(1)       DRM_DWORD         *f_pcbParamData )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pTestData     != NULL );
    ChkArg( f_ppbUnwrapData != NULL );
    ChkArg( f_pcbUnwrapData != NULL );
    ChkArg( f_ppbParamData  != NULL );
    ChkArg( f_pcbParamData  != NULL );

    switch( f_eTestDataType )
    {
    case eTEST_DATA_DUMMY:
        *f_ppbUnwrapData = f_pTestData->RandomKeyData;
        *f_pcbUnwrapData = 1;
        *f_ppbParamData  = NULL;
        *f_pcbParamData  = 0;
        break;

    case eTEST_DATA_DUMMY_32:
        *f_ppbUnwrapData = f_pTestData->RandomKeyData;
        *f_pcbUnwrapData = 32;
        *f_ppbParamData  = NULL;
        *f_pcbParamData  = 0;
        break;

    case eTEST_DATA_DRM_ENCRYPTED_KEY_16:
        *f_ppbUnwrapData = ( const DRM_BYTE* )&f_pTestData->EncryptedAesKey;
        *f_pcbUnwrapData = SIZEOF( f_pTestData->EncryptedAesKey );
        *f_ppbParamData  = NULL;
        *f_pcbParamData  = 0;
        break;

    case eTEST_DATA_DRM_ENCRYPTED_KEY_32:
        *f_ppbUnwrapData = ( const DRM_BYTE* )&f_pTestData->EncryptedEccKey;
        *f_pcbUnwrapData = SIZEOF( f_pTestData->EncryptedEccKey );
        *f_ppbParamData  = NULL;
        *f_pcbParamData  = 0;
        break;
    case eTEST_DATA_ENCRYPTED_BYTES_16:
        *f_ppbUnwrapData = f_pTestData->RandomKeyData;
        *f_pcbUnwrapData = 16;
        *f_ppbParamData  = NULL;
        *f_pcbParamData  = 0;
        break;
    case eTEST_DATA_ENCRYPTED_BYTES_32:
        *f_ppbUnwrapData = f_pTestData->RandomKeyData;
        *f_pcbUnwrapData = 32;
        *f_ppbParamData  = NULL;
        *f_pcbParamData  = 0;
        break;

    case eTEST_DATA_ENCRYPTED_BYTES_128:
        *f_ppbUnwrapData = f_pTestData->RandomKeyData;
        *f_pcbUnwrapData = 128;
        *f_ppbParamData  = NULL;
        *f_pcbParamData  = 0;
        break;

    case eTEST_DATA_XMR_LICENSE_ROOT:
        *f_ppbUnwrapData = ( const DRM_BYTE* )&f_pTestData->RootUnwrapInfo;
        *f_pcbUnwrapData = SIZEOF( DRM_XMR_UNWRAP_INFO ) ;
        *f_ppbParamData  = ( const DRM_BYTE* )&f_pTestData->fIgnoreChecksum;
        *f_pcbParamData  = SIZEOF( DRM_BOOL );
        break;

    case eTEST_DATA_XMR_LICENSE_SIMPLE:
        *f_ppbUnwrapData = ( const DRM_BYTE* )&f_pTestData->SimpleUnwrapInfo;
        *f_pcbUnwrapData = SIZEOF( DRM_XMR_UNWRAP_INFO ) ;
        *f_ppbParamData  = ( const DRM_BYTE* )&f_pTestData->fIgnoreChecksum;
        *f_pcbParamData  = SIZEOF( DRM_BOOL );
        break;

    case eTEST_DATA_XMR_LICENSE_LEAF:
        *f_ppbUnwrapData = ( const DRM_BYTE* )&f_pTestData->LeafUnwrapInfo;
        *f_pcbUnwrapData = SIZEOF( DRM_XMR_UNWRAP_INFO ) ;
        *f_ppbParamData  = ( const DRM_BYTE* )&f_pTestData->fIgnoreChecksum;
        *f_pcbParamData  = SIZEOF( DRM_BOOL );
        break;

    case eTEST_DATA_XMR_LICENSE_LEGACY:
        *f_ppbUnwrapData = ( const DRM_BYTE* )&f_pTestData->SimpleLegacyUnwrapInfo;
        *f_pcbUnwrapData = SIZEOF( DRM_XMR_UNWRAP_INFO ) ;
        *f_ppbParamData  = ( const DRM_BYTE* )&f_pTestData->fIgnoreChecksum;
        *f_pcbParamData  = SIZEOF( DRM_BOOL );
        break;

    case eTEST_DATA_XMR_LICENSE_LEGACY_LEAF:
        *f_ppbUnwrapData = ( const DRM_BYTE* )&f_pTestData->LegacyLeafUnwrapInfo;
        *f_pcbUnwrapData = SIZEOF( DRM_XMR_UNWRAP_INFO ) ;
        *f_ppbParamData  = ( const DRM_BYTE* )&f_pTestData->fIgnoreChecksum;
        *f_pcbParamData  = SIZEOF( DRM_BOOL );
        break;

    case eTEST_DATA_CERTIFICATE:
        *f_ppbUnwrapData = f_pTestData->pbDeviceCertChain + f_pTestData->iDeviceCertStart;
        *f_pcbUnwrapData = f_pTestData->cbDeviceCertChain - f_pTestData->iDeviceCertStart;
        *f_ppbParamData  = ( const DRM_BYTE* )&f_pTestData->dwCertKeyIndex ;
        *f_pcbParamData  = SIZEOF( DRM_DWORD );
        break;

    case eTEST_DATA_CERTIFICATE_WITH_BAD_PUBKEY:
        *f_ppbUnwrapData = f_pTestData->pbDeviceCertChain_WithBadPubkeyInLeaf + f_pTestData->iDeviceCertWithBadPubkeyStart;
        *f_pcbUnwrapData = f_pTestData->iDeviceCertWithBadPubkeyUnsignedEnd - f_pTestData->iDeviceCertWithBadPubkeyStart;
        *f_ppbParamData  = ( const DRM_BYTE* )&f_pTestData->dwCertKeyIndex ;
        *f_pcbParamData  = SIZEOF( DRM_DWORD );
        break;

    case eTEST_DATA_CERTIFICATE_WITH_SWAPPED_PUBKEYS:
        *f_ppbUnwrapData = f_pTestData->pbDeviceCertChain_WithSwappedPubkeysInLeaf + f_pTestData->iDeviceCertWithSwappedPubkeysStart;
        *f_pcbUnwrapData = f_pTestData->iDeviceCertWithSwappedPubkeysUnsignedEnd - f_pTestData->iDeviceCertWithSwappedPubkeysStart;
        *f_ppbParamData  = ( const DRM_BYTE* )&f_pTestData->dwCertKeyIndex ;
        *f_pcbParamData  = SIZEOF( DRM_DWORD );
        break;

    case eTEST_DATA_CERTIFICATE_WITH_INCREASED_SL:
        *f_ppbUnwrapData = f_pTestData->pbDeviceCertChain_WithIncreasedSLInLeaf + f_pTestData->iDeviceCertWithIncreasedSLStart;
        *f_pcbUnwrapData = f_pTestData->iDeviceCertWithIncreasedSLUnsignedEnd - f_pTestData->iDeviceCertWithIncreasedSLStart;
        *f_ppbParamData  = ( const DRM_BYTE* )&f_pTestData->dwCertKeyIndex ;
        *f_pcbParamData  = SIZEOF( DRM_DWORD );
        break;

    case eTEST_DATA_SLK_SYMMETRIC:
        *f_ppbUnwrapData = f_pTestData->RandomKeyData;
        *f_pcbUnwrapData = 16;
        *f_ppbParamData  = ( const DRM_BYTE* )&f_pTestData->slkEncryptionSymmetric;
        *f_pcbParamData  = SIZEOF( OEM_HAL_SLK_ENCRYPTION_TYPE );
        break;

    case eTEST_DATA_SLK_ASYMMETRIC:
        *f_ppbUnwrapData = f_pTestData->RandomKeyData;
        *f_pcbUnwrapData = 128;
        *f_ppbParamData  = ( const DRM_BYTE* )&f_pTestData->slkEncryptionAsymmetric;
        *f_pcbParamData  = SIZEOF( OEM_HAL_SLK_ENCRYPTION_TYPE );
        break;

    case eTEST_DATA_XMR_LICENSE_DOMAIN_ROOT:
        *f_ppbUnwrapData = ( const DRM_BYTE* )&f_pTestData->RootDomainUnwrapInfo;
        *f_pcbUnwrapData = SIZEOF( DRM_XMR_UNWRAP_INFO ) ;
        *f_ppbParamData  = ( const DRM_BYTE* )&f_pTestData->fIgnoreChecksum;
        *f_pcbParamData  = SIZEOF( DRM_BOOL );
        break;

    case eTEST_DATA_XMR_LICENSE_DOMAIN_SIMPLE:
        *f_ppbUnwrapData = ( const DRM_BYTE* )&f_pTestData->SimpleDomainUnwrapInfo;
        *f_pcbUnwrapData = SIZEOF( DRM_XMR_UNWRAP_INFO ) ;
        *f_ppbParamData  = ( const DRM_BYTE* )&f_pTestData->fIgnoreChecksum;
        *f_pcbParamData  = SIZEOF( DRM_BOOL );
        break;
    case eTEST_DATA_XMR_LICENSE_DOMAIN_LEGACY:
        *f_ppbUnwrapData = ( const DRM_BYTE* )&f_pTestData->LegacyDomainUnwrapInfo;
        *f_pcbUnwrapData = SIZEOF( DRM_XMR_UNWRAP_INFO ) ;
        *f_ppbParamData  = ( const DRM_BYTE* )&f_pTestData->fIgnoreChecksum;
        *f_pcbParamData  = SIZEOF( DRM_BOOL );
        break;

    default:
        ChkDR( DRM_E_LOGICERR );
        break;
    }

ErrorExit:

    return dr;
}


static DRM_RESULT _TestUnwrap(
    __inout_ecount( 1 )   DRM_BB_CONTEXT    *f_poBBXContext,
    __inout_ecount( 1 )   WrapUnwrapData    *f_pTestData,
    __in                  OEM_HAL_KEY_TYPE   f_eTargetKey,
    __in                  OEM_HAL_KEY_TYPE   f_eWrappingKey,
    __in                  eOemTestDataType   f_eTestDataType,
    __in                  DRM_RESULT         f_drExpected,
    __in                  DRM_BOOL           f_fKeepResult )
{
          DRM_RESULT         dr           = DRM_SUCCESS;
    const DRM_BYTE          *pbUnwrapData = NULL;
          DRM_DWORD          cbUnwrapData = 0;
    const DRM_BYTE          *pbParamData  = NULL;
          DRM_DWORD          cbParamData  = 0;
          OEM_CRYPTO_HANDLE  hWrappingKey = OEM_CRYPTO_HANDLE_INVALID;
          OEM_CRYPTO_HANDLE  hNewKey      = OEM_CRYPTO_HANDLE_INVALID;

    if( f_eWrappingKey != OEM_HAL_KEY_TYPE_INVALID )
    {
        hWrappingKey = f_pTestData->rghWrappingKeys[ f_eWrappingKey ];
    }

    /* Load the unwrap parameter data */
    ChkDR( _GetUnwrapData( f_pTestData,
                           f_eTestDataType,
                           &pbUnwrapData,
                           &cbUnwrapData,
                           &pbParamData,
                           &cbParamData ) );

    /* Try to unwrap */
    dr = _Hal_UnwrapKey( f_poBBXContext,
                         f_eTargetKey,
                         hWrappingKey,
                         pbUnwrapData,
                         cbUnwrapData,
                         pbParamData,
                         cbParamData,
                         NULL,
                         0,
                         &hNewKey );

    if( dr != f_drExpected )
    {
        Log( "", "_TestUnwrap failure: dr=0x%x(%s)  f_drExpected=0x%x(%s)  f_eTargetKey=%d  f_eWrappingKey=%d", dr, DRM_ERR_GetErrorNameFromCode( dr, NULL ), f_drExpected, DRM_ERR_GetErrorNameFromCode( f_drExpected, NULL ), f_eTargetKey, f_eWrappingKey );
        ChkBOOL( dr == f_drExpected, DRM_E_TEST_UNEXPECTED_OUTPUT );
    }

    if( dr == DRM_SUCCESS )
    {
        ChkBOOL( hNewKey != OEM_CRYPTO_HANDLE_INVALID, DRM_E_TEST_UNEXPECTED_OUTPUT );
    }

    /* See if we need to keep the key for later use */
    if( f_fKeepResult )
    {
        DRMASSERT( dr == DRM_SUCCESS );

        DRM_BBX_SafeReleaseKey( f_poBBXContext, &f_pTestData->rghWrappingKeys[ f_eTargetKey ] );

        f_pTestData->rghWrappingKeys[ f_eTargetKey ] = hNewKey;
        hNewKey                                      = OEM_CRYPTO_HANDLE_INVALID;
    }

    dr = DRM_SUCCESS;

ErrorExit:

    DRM_BBX_SafeReleaseKey( f_poBBXContext, &hNewKey );

    return dr;
}


static DRM_RESULT _TestWrap(
    __inout_ecount( 1 )   DRM_BB_CONTEXT    *f_poBBXContext,
    __inout_ecount( 1 )   WrapUnwrapData    *f_pTestData,
    __in                  OEM_HAL_KEY_TYPE   f_eTargetKey,
    __in                  OEM_HAL_KEY_TYPE   f_eWrappingKey,
    __in                  DRM_RESULT         f_drExpected )
{
    DRM_RESULT         dr           = DRM_SUCCESS;
    OEM_CRYPTO_HANDLE  hWrappingKey = OEM_CRYPTO_HANDLE_INVALID;
    OEM_CRYPTO_HANDLE  hTargetKey   = OEM_CRYPTO_HANDLE_INVALID;
    DRM_BYTE           rgbKeyBuffer[ ECC_P256_CIPHERTEXT_SIZE_IN_BYTES ] = { 0 };
    DRM_DWORD          cbKeyBuffer  = SIZEOF( rgbKeyBuffer );
    DRM_ENCRYPTED_KEY  encryptedKey = { 0 };
    DRM_BYTE          *pKeyBuffer = rgbKeyBuffer;

    if( f_eTargetKey != OEM_HAL_KEY_TYPE_INVALID )
    {
        hTargetKey = f_pTestData->rghWrappingKeys[ f_eTargetKey ];
    }

    if( f_eWrappingKey != OEM_HAL_KEY_TYPE_INVALID )
    {
        hWrappingKey = f_pTestData->rghWrappingKeys[ f_eWrappingKey ];

        if( f_eWrappingKey == OEM_HAL_KEY_TYPE_AES_KEYFILE )
        {
            encryptedKey.pbEncryptedKey = rgbKeyBuffer;
            encryptedKey.cbEncryptedKey = cbKeyBuffer;

            cbKeyBuffer = SIZEOF( encryptedKey );
            pKeyBuffer  = ( DRM_BYTE* )&encryptedKey;
        }
    }


    /* Try to wrap */
    dr = _Hal_WrapKey( f_poBBXContext,
                       hTargetKey,
                       hWrappingKey,
                       pKeyBuffer,
                       &cbKeyBuffer );

    if( dr != f_drExpected )
    {
        Log( "", "_TestWrap failure: dr=0x%x(%s)  f_drExpected=0x%x(%s)  f_eTargetKey=%d  f_eWrappingKey=%d", dr, DRM_ERR_GetErrorNameFromCode( dr, NULL ), f_drExpected, DRM_ERR_GetErrorNameFromCode( f_drExpected, NULL ), f_eTargetKey, f_eWrappingKey );
        ChkBOOL( dr == f_drExpected, DRM_E_TEST_UNEXPECTED_OUTPUT );
    }

    dr = DRM_SUCCESS;

ErrorExit:
    return dr;
}


static DRM_VOID _CleanupWrapUnwrapData(
    __inout_ecount( 1 ) DRM_BB_CONTEXT *f_poBBXContext,
    __inout_ecount( 1 ) WrapUnwrapData *f_pTestData )
{
    DRMASSERT( f_pTestData != NULL );

    if( f_pTestData != NULL )
    {
        DRM_LONG iKeyType = 0;

        SAFE_OEM_FREE( f_pTestData->pbRootXmrLicense );
        SAFE_OEM_FREE( f_pTestData->pbSimpleXmrLicenseAes );
        SAFE_OEM_FREE( f_pTestData->pbSimpleXmrLicenseLegacy );
        SAFE_OEM_FREE( f_pTestData->pbLeafXmrLicenseAes );
        SAFE_OEM_FREE( f_pTestData->pbLeafXmrLicenseLegacy );
        SAFE_OEM_FREE( f_pTestData->pbRootDomainXmrLicense );
        SAFE_OEM_FREE( f_pTestData->pbSimpleDomainXmrLicense );
        SAFE_OEM_FREE( f_pTestData->pbLegacyDomainXmrLicense );

        SAFE_OEM_FREE( f_pTestData->pbDeviceCertChain );
        SAFE_OEM_FREE( f_pTestData->pbDeviceCertChain_WithBadPubkeyInLeaf );
        SAFE_OEM_FREE( f_pTestData->pbDeviceCertChain_WithSwappedPubkeysInLeaf );
        SAFE_OEM_FREE( f_pTestData->pbDeviceCertChain_WithIncreasedSLInLeaf );

        for( iKeyType = 0; iKeyType < OEM_HAL_KEY_TYPE_NUM_VALID_TYPES; iKeyType++ )
        {
            DRM_BBX_SafeReleaseKey( f_poBBXContext, &f_pTestData->rghWrappingKeys[iKeyType] );
        }

        OEM_SECURE_ZERO_MEMORY( f_pTestData, SIZEOF( WrapUnwrapData ) );
    }
}

#define TestUnwrap( TargetKey, WrappingKey, DataType, ExpectedResult, KeepKey )                                               \
    localError = _TestUnwrap( f_poBBXContext, f_pTestData, TargetKey, WrappingKey, DataType, ExpectedResult, KeepKey );       \
    if( DRM_FAILED( localError ) && dr == DRM_SUCCESS )                                                                       \
    {                                                                                                                         \
        dr = localError;                                                                                                      \
    }                                                                                                                         \


typedef struct __tagValidWrapOrUnwrapPair
{
    OEM_HAL_KEY_TYPE eWrappedKeyType;
    OEM_HAL_KEY_TYPE eWrappingKeyType;
} ValidWrapOrUnwrapPair;

static ValidWrapOrUnwrapPair g_ValidUnwrapPairs[] = {
    { OEM_HAL_KEY_TYPE_AES_ECB_ROOT,                OEM_HAL_KEY_TYPE_ECC_P256_CONTENT_ENCRYPT },
    { OEM_HAL_KEY_TYPE_AES_ECB_DOMAIN_PROTECT,      OEM_HAL_KEY_TYPE_ECC_P256_CONTENT_ENCRYPT },
    { OEM_HAL_KEY_TYPE_ECC_P256_PUBLIC,             OEM_HAL_KEY_TYPE_ECC_P256_PUBLIC          },
    { OEM_HAL_KEY_TYPE_ECC_P256_CONTENT_ENCRYPT,    OEM_HAL_KEY_TYPE_AES_KEYFILE              },
    { OEM_HAL_KEY_TYPE_ECC_P256_DOMAIN_ENCRYPT,     OEM_HAL_KEY_TYPE_ECC_P256_CONTENT_ENCRYPT },
    { OEM_HAL_KEY_TYPE_ECC_P256_DOMAIN_ENCRYPT,     OEM_HAL_KEY_TYPE_AES_ECB_DOMAIN_PROTECT   },
    { OEM_HAL_KEY_TYPE_ECC_P256_MESSAGE_ENCRYPT,    OEM_HAL_KEY_TYPE_AES_KEYFILE              },
    { OEM_HAL_KEY_TYPE_ECC_P256_SIGN,               OEM_HAL_KEY_TYPE_AES_KEYFILE              },
#if !DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY
    { OEM_HAL_KEY_TYPE_AES_ECB_NEW_LICENSE_ROOT,    OEM_HAL_KEY_TYPE_AES_GENERIC_KEY_ROOT     },
#endif  /* !DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY */
    { OEM_HAL_KEY_TYPE_AES_CTR_CONTENT,             OEM_HAL_KEY_TYPE_ECC_P256_CONTENT_ENCRYPT },
    { OEM_HAL_KEY_TYPE_AES_CTR_CONTENT,             OEM_HAL_KEY_TYPE_AES_ECB_ROOT             },
#if !DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY
    { OEM_HAL_KEY_TYPE_AES_CTR_NEW_LICENSE_CONTENT, OEM_HAL_KEY_TYPE_AES_GENERIC_KEY_CONTENT  },
#endif  /* !DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY */
    { OEM_HAL_KEY_TYPE_AES_ECB_MESSAGE_ENCRYPT,     OEM_HAL_KEY_TYPE_ECC_P256_MESSAGE_ENCRYPT },
    { OEM_HAL_KEY_TYPE_AES_OMAC1_SECURE_STORE,      OEM_HAL_KEY_TYPE_AES_KEYFILE              }
};

static ValidWrapOrUnwrapPair g_ValidUnwrapPairsSlkCrypto[] = {
    { OEM_HAL_KEY_TYPE_AES_SLK,                     OEM_HAL_KEY_TYPE_ECC_P256_CONTENT_ENCRYPT },
    { OEM_HAL_KEY_TYPE_AES_SLK,                     OEM_HAL_KEY_TYPE_ECC_P256_CONTENT_ENCRYPT },
    { OEM_HAL_KEY_TYPE_AES_CTR_CONTENT,             OEM_HAL_KEY_TYPE_AES_SLK                  }
};

static ValidWrapOrUnwrapPair g_ValidUnwrapPairsDomainSupported[] = {
    { OEM_HAL_KEY_TYPE_AES_ECB_ROOT,                OEM_HAL_KEY_TYPE_ECC_P256_DOMAIN_ENCRYPT  },
#if !DRM_SUPPORT_SECUREOEMHAL
    { OEM_HAL_KEY_TYPE_LEGACY,                      OEM_HAL_KEY_TYPE_ECC_P256_DOMAIN_ENCRYPT  },
#endif /* !DRM_SUPPORT_SECUREOEMHAL */
    { OEM_HAL_KEY_TYPE_AES_CTR_CONTENT,             OEM_HAL_KEY_TYPE_ECC_P256_DOMAIN_ENCRYPT  }
};

static ValidWrapOrUnwrapPair g_ValidUnwrapPairsLegacyCrypto[] = {
    { OEM_HAL_KEY_TYPE_LEGACY,                      OEM_HAL_KEY_TYPE_ECC_P256_CONTENT_ENCRYPT },
    { OEM_HAL_KEY_TYPE_LEGACY,                      OEM_HAL_KEY_TYPE_AES_ECB_ROOT             }
};

/*
** The following function tests (and illustrates) all of the valid and invalid key unwrap combinations
*/
static DRM_RESULT _TestKeyUnwrap(
    __inout_ecount( 1 ) DRM_BB_CONTEXT *f_poBBXContext,
    __inout_ecount( 1 ) WrapUnwrapData *f_pTestData )
{
    DRM_RESULT dr         = DRM_SUCCESS;
    DRM_RESULT localError = DRM_SUCCESS;

    /*
    ** First, try the valid combinations
    */

    /*         |  Key Type To Unwrap                         |  Wrapping Key Type                         |  Unwrap Data                        |  Expected Result */
    TestUnwrap( OEM_HAL_KEY_TYPE_AES_ECB_ROOT,                OEM_HAL_KEY_TYPE_ECC_P256_CONTENT_ENCRYPT,   eTEST_DATA_XMR_LICENSE_ROOT,          DRM_SUCCESS,  TRUE );     /* Root license content key decrypt with my ecc priv content key. */
    TestUnwrap( OEM_HAL_KEY_TYPE_AES_ECB_DOMAIN_PROTECT,      OEM_HAL_KEY_TYPE_ECC_P256_CONTENT_ENCRYPT,   eTEST_DATA_ENCRYPTED_BYTES_128,       DRM_SUCCESS,  TRUE );     /* Load domain-key-protecting transient key from domain join response.  The protecting key is encrypted with the device content key. */

#if !DRM_SUPPORT_SECUREOEMHAL
    TestUnwrap( OEM_HAL_KEY_TYPE_LEGACY,                      OEM_HAL_KEY_TYPE_ECC_P256_CONTENT_ENCRYPT,   eTEST_DATA_XMR_LICENSE_LEGACY,        DRM_SUCCESS,  TRUE );     /* Cocktail content key decrypt with my ecc priv content key. */
    TestUnwrap( OEM_HAL_KEY_TYPE_LEGACY,                      OEM_HAL_KEY_TYPE_AES_ECB_ROOT,               eTEST_DATA_XMR_LICENSE_LEGACY_LEAF,   DRM_SUCCESS,  FALSE );    /* Cocktail content key decrypt with root license key. */
#endif /* !DRM_SUPPORT_SECUREOEMHAL */

    TestUnwrap( OEM_HAL_KEY_TYPE_ECC_P256_PUBLIC,             OEM_HAL_KEY_TYPE_ECC_P256_PUBLIC,            eTEST_DATA_CERTIFICATE,               DRM_SUCCESS,  TRUE );     /* Load a public key into a register. */

    if( DRM_BBX_IsSlkCryptoSupported() )
    {
        TestUnwrap( OEM_HAL_KEY_TYPE_AES_SLK,                 OEM_HAL_KEY_TYPE_ECC_P256_CONTENT_ENCRYPT,   eTEST_DATA_SLK_SYMMETRIC,             DRM_SUCCESS,  TRUE );     /* SLK decrypt with a symmetric key derived from my ecc priv content key. */
        TestUnwrap( OEM_HAL_KEY_TYPE_AES_SLK,                 OEM_HAL_KEY_TYPE_ECC_P256_CONTENT_ENCRYPT,   eTEST_DATA_SLK_ASYMMETRIC,            DRM_SUCCESS,  FALSE );    /* SLK decrypt with my ecc priv content key */
        TestUnwrap( OEM_HAL_KEY_TYPE_AES_CTR_CONTENT,         OEM_HAL_KEY_TYPE_AES_SLK,                    eTEST_DATA_ENCRYPTED_BYTES_32,        DRM_SUCCESS,  FALSE );    /* Local simple license key decrypted with SLK. */
    }
    TestUnwrap( OEM_HAL_KEY_TYPE_ECC_P256_CONTENT_ENCRYPT,    OEM_HAL_KEY_TYPE_AES_KEYFILE,                eTEST_DATA_DRM_ENCRYPTED_KEY_32,      DRM_SUCCESS,  FALSE );    /* Load device content key from key file.  Device private content encryption key decrypted from keyfile using aes keyfile key. */
    TestUnwrap( OEM_HAL_KEY_TYPE_ECC_P256_DOMAIN_ENCRYPT,     OEM_HAL_KEY_TYPE_ECC_P256_CONTENT_ENCRYPT,   eTEST_DATA_ENCRYPTED_BYTES_32,        DRM_SUCCESS,  FALSE );    /* Load domain key from hash data store. Key is encrypted with a symmetric key derived from device ECC key. */
    TestUnwrap( OEM_HAL_KEY_TYPE_ECC_P256_DOMAIN_ENCRYPT,     OEM_HAL_KEY_TYPE_AES_ECB_DOMAIN_PROTECT,     eTEST_DATA_ENCRYPTED_BYTES_32,        DRM_SUCCESS,  FALSE );    /* Unwrap an ECC256 private domain key in two halves using the {CI|CK} Domain Protect key pair to do an AES-ECB decrypt on one half each.  Scenario: Process Domain Join Response. */
    TestUnwrap( OEM_HAL_KEY_TYPE_ECC_P256_MESSAGE_ENCRYPT,    OEM_HAL_KEY_TYPE_AES_KEYFILE,                eTEST_DATA_DRM_ENCRYPTED_KEY_32,      DRM_SUCCESS,  FALSE );    /* Load device message key from key file.  Device private message encryption key decrypted from keyfile using aes keyfile key. */
    TestUnwrap( OEM_HAL_KEY_TYPE_ECC_P256_SIGN,               OEM_HAL_KEY_TYPE_AES_KEYFILE,                eTEST_DATA_DRM_ENCRYPTED_KEY_32,      DRM_SUCCESS,  FALSE );    /* Load device signing key from key file.  Device private signing key decrypted from keyfile using aes keyfile key. */
#if !DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY
    TestUnwrap( OEM_HAL_KEY_TYPE_AES_ECB_NEW_LICENSE_ROOT,    OEM_HAL_KEY_TYPE_AES_GENERIC_KEY_ROOT,       eTEST_DATA_ENCRYPTED_BYTES_32,        DRM_SUCCESS,  FALSE );    /* Local root license key caching to avoid running out of registers. */
#endif /* !DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY */
    TestUnwrap( OEM_HAL_KEY_TYPE_AES_CTR_CONTENT,             OEM_HAL_KEY_TYPE_ECC_P256_CONTENT_ENCRYPT,   eTEST_DATA_XMR_LICENSE_SIMPLE,        DRM_SUCCESS,  FALSE );    /* Simple license content key decrypted with my ecc privkey. */
    TestUnwrap( OEM_HAL_KEY_TYPE_AES_CTR_CONTENT,             OEM_HAL_KEY_TYPE_AES_ECB_ROOT,               eTEST_DATA_XMR_LICENSE_LEAF,          DRM_SUCCESS,  FALSE );    /* Leaf license content key decrypted with root license content key. */
#if !DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY
    TestUnwrap( OEM_HAL_KEY_TYPE_AES_CTR_NEW_LICENSE_CONTENT, OEM_HAL_KEY_TYPE_AES_GENERIC_KEY_CONTENT,    eTEST_DATA_ENCRYPTED_BYTES_32,        DRM_SUCCESS,  FALSE );    /* Local leaf/simple license key caching to avoid running out of registers. */
#endif /* !DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY */
    TestUnwrap( OEM_HAL_KEY_TYPE_AES_ECB_MESSAGE_ENCRYPT,     OEM_HAL_KEY_TYPE_ECC_P256_MESSAGE_ENCRYPT,   eTEST_DATA_ENCRYPTED_BYTES_128,       DRM_SUCCESS,  FALSE );    /* MI/MK decrypted with my ecc private message encrypt key. */
    TestUnwrap( OEM_HAL_KEY_TYPE_AES_OMAC1_SECURE_STORE,      OEM_HAL_KEY_TYPE_AES_KEYFILE,                eTEST_DATA_DRM_ENCRYPTED_KEY_16,      DRM_SUCCESS,  FALSE );    /* Secure store key read from key file decrypted with aes keyfile key. */

    if( DRM_DOMAIN_IsDomainSupported() )
    {
        TestUnwrap( OEM_HAL_KEY_TYPE_AES_ECB_ROOT,            OEM_HAL_KEY_TYPE_ECC_P256_DOMAIN_ENCRYPT,    eTEST_DATA_XMR_LICENSE_DOMAIN_ROOT,   DRM_SUCCESS,  FALSE );    /* Root license content key decrypt with domain ecc priv key. */

#if !DRM_SUPPORT_SECUREOEMHAL
        TestUnwrap( OEM_HAL_KEY_TYPE_LEGACY,                  OEM_HAL_KEY_TYPE_ECC_P256_DOMAIN_ENCRYPT,    eTEST_DATA_XMR_LICENSE_DOMAIN_LEGACY, DRM_SUCCESS,  FALSE );    /* Cocktail content key decrypt with domain ecc priv key. */
#endif /* !DRM_SUPPORT_SECUREOEMHAL */

        TestUnwrap( OEM_HAL_KEY_TYPE_AES_CTR_CONTENT,         OEM_HAL_KEY_TYPE_ECC_P256_DOMAIN_ENCRYPT,    eTEST_DATA_XMR_LICENSE_DOMAIN_SIMPLE, DRM_SUCCESS,  FALSE );    /* Simple license content key decrypted with domain ecc privkey. */
    }

    /*
    ** Now all the invalid ones
    */
    {
        DRM_LONG         idx1               = 0;
        DRM_LONG         idx2               = 0;
        DRM_DWORD        idx3               = 0;
        OEM_HAL_KEY_TYPE eWrappedKeyType    = OEM_HAL_KEY_TYPE_INVALID;
        OEM_HAL_KEY_TYPE eWrappingKeyType   = OEM_HAL_KEY_TYPE_INVALID;

        const DRM_LONG   idx1Min            = OEM_HAL_KEY_TYPE_INVALID;
        const DRM_LONG   idx1Max            = OEM_HAL_KEY_TYPE_NUM_VALID_TYPES;
        const DRM_LONG   idx2Min            = OEM_HAL_KEY_TYPE_INVALID;
        const DRM_LONG   idx2Max            = OEM_HAL_KEY_TYPE_NUM_VALID_TYPES;

        for( idx1 = idx1Min; idx1 < idx1Max; idx1++ )
        {
            eWrappedKeyType = (OEM_HAL_KEY_TYPE)idx1;
            for( idx2 = idx2Min; idx2 < idx2Max; idx2++ )
            {
                DRM_RESULT       drExpected = DRM_SUCCESS;

                eWrappingKeyType = (OEM_HAL_KEY_TYPE)idx2;

                switch( eWrappedKeyType )
                {
                case OEM_HAL_KEY_TYPE_INVALID:                      __fallthrough;
                case OEM_HAL_KEY_TYPE_ECC_P256_MODEL_SIGN:          __fallthrough;
                case OEM_HAL_KEY_TYPE_AES_GENERIC_KEY_CONTENT:      __fallthrough;
                case OEM_HAL_KEY_TYPE_AES_GENERIC_KEY_ROOT:         __fallthrough;
#if DRM_SUPPORT_SECUREOEMHAL
                case OEM_HAL_KEY_TYPE_LEGACY:                       __fallthrough;
#endif  /* DRM_SUPPORT_SECUREOEMHAL */
#if DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY
                case OEM_HAL_KEY_TYPE_AES_ECB_NEW_LICENSE_ROOT:     __fallthrough;
                case OEM_HAL_KEY_TYPE_AES_CTR_NEW_LICENSE_CONTENT:  __fallthrough;
#endif  /* DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY */
                case OEM_HAL_KEY_TYPE_AES_KEYFILE:
                    drExpected = DRM_E_OEMHAL_UNSUPPORTED_KEY_TYPE;
                    break;
                default:
                    drExpected = DRM_E_OEMHAL_UNSUPPORTED_KEY_WRAPPING_FORMAT;
                    break;
                }

                for( idx3 = 0; idx3 < NO_OF( g_ValidUnwrapPairs ); idx3++ )
                {
                    if( g_ValidUnwrapPairs[idx3].eWrappedKeyType  == eWrappedKeyType
                     && g_ValidUnwrapPairs[idx3].eWrappingKeyType == eWrappingKeyType )
                    {
                        drExpected = DRM_SUCCESS;
                        break;
                    }
                }

                if( DRM_DOMAIN_IsDomainSupported() )
                {
                    for( idx3 = 0; idx3 < NO_OF( g_ValidUnwrapPairsDomainSupported ); idx3++ )
                    {
                        if( g_ValidUnwrapPairsDomainSupported[idx3].eWrappedKeyType  == eWrappedKeyType
                         && g_ValidUnwrapPairsDomainSupported[idx3].eWrappingKeyType == eWrappingKeyType )
                        {
                            drExpected = DRM_SUCCESS;
                            break;
                        }
                    }
                }
                else
                {
                    if( OEM_HAL_KEY_TYPE_ECC_P256_DOMAIN_ENCRYPT == eWrappingKeyType )
                    {
                        /*
                        ** We expect invalid arg from the hal because we have incorrect license data
                        ** in these combinations.
                        ** At this point we have already rejected creating a key handle for the Domain
                        ** so it is okay to not go looking for that.
                        */
                        switch( eWrappedKeyType )
                        {
#if !DRM_SUPPORT_SECUREOEMHAL
                        case OEM_HAL_KEY_TYPE_LEGACY:           __fallthrough;
#endif /* !DRM_SUPPORT_SECUREOEMHAL */
                        case OEM_HAL_KEY_TYPE_AES_ECB_ROOT:     __fallthrough;
                        case OEM_HAL_KEY_TYPE_AES_CTR_CONTENT:
                            drExpected = DRM_E_INVALIDARG;
                            break;
                        default:
                            break;
                        }
                    }
                }

                if( DRM_BBX_IsSlkCryptoSupported() )
                {
                    for( idx3 = 0; idx3 < NO_OF( g_ValidUnwrapPairsSlkCrypto ); idx3++ )
                    {
                        if( g_ValidUnwrapPairsSlkCrypto[idx3].eWrappedKeyType  == eWrappedKeyType
                         && g_ValidUnwrapPairsSlkCrypto[idx3].eWrappingKeyType == eWrappingKeyType )
                        {
                            drExpected = DRM_SUCCESS;
                            break;
                        }
                    }
                }
                else
                {
                    if( OEM_HAL_KEY_TYPE_AES_SLK == eWrappedKeyType )
                    {
                        drExpected = DRM_E_NOTIMPL;
                    }
                }
#if !DRM_SUPPORT_SECUREOEMHAL
                for( idx3 = 0; idx3 < NO_OF( g_ValidUnwrapPairsLegacyCrypto ); idx3++ )
                {
                    if( g_ValidUnwrapPairsLegacyCrypto[idx3].eWrappedKeyType  == eWrappedKeyType
                     && g_ValidUnwrapPairsLegacyCrypto[idx3].eWrappingKeyType == eWrappingKeyType )
                    {
                        drExpected = DRM_SUCCESS;
                        break;
                    }
                }
#endif /* !DRM_SUPPORT_SECUREOEMHAL */

                if( drExpected != DRM_SUCCESS )
                {
                    TestUnwrap( eWrappedKeyType, eWrappingKeyType, eTEST_DATA_DUMMY, drExpected, FALSE );
                }
            }
        }
    }

    /*
    ** ChkDR here to trap any error from TestUnwrap
    */
    ChkDR( dr );

ErrorExit:
    return dr;
}


#define TestWrap( TargetKey, WrappingKey, ExpectedResult )                                               \
    localError = _TestWrap( f_poBBXContext, f_pTestData, TargetKey, WrappingKey, ExpectedResult );       \
    if( DRM_FAILED( localError ) && DRM_SUCCEEDED( dr ) )                                                \
    {                                                                                                    \
        dr = localError;                                                                                 \
    }                                                                                                    \

static ValidWrapOrUnwrapPair g_ValidWrapPairs[] = {
    { OEM_HAL_KEY_TYPE_ECC_P256_DOMAIN_ENCRYPT,     OEM_HAL_KEY_TYPE_ECC_P256_CONTENT_ENCRYPT  },    /* Write domain key to hash data store.  Key is encrypted with a symmetric key derived from device ECC key. */
    { OEM_HAL_KEY_TYPE_ECC_P256_DOMAIN_ENCRYPT,     OEM_HAL_KEY_TYPE_INVALID                   },    /* Load domain public key (pubkey). */
    { OEM_HAL_KEY_TYPE_ECC_P256_CONTENT_ENCRYPT,    OEM_HAL_KEY_TYPE_AES_KEYFILE,              },    /* Current or historical ECC device content privkey written to keyfile. */
    { OEM_HAL_KEY_TYPE_ECC_P256_MESSAGE_ENCRYPT,    OEM_HAL_KEY_TYPE_AES_KEYFILE,              },    /* Current ECC device message privkey written to keyfile. */
    { OEM_HAL_KEY_TYPE_ECC_P256_SIGN,               OEM_HAL_KEY_TYPE_AES_KEYFILE,              },    /* Current ECC device sign privkey written to keyfile. */
#if !DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY
    { OEM_HAL_KEY_TYPE_AES_ECB_ROOT,                OEM_HAL_KEY_TYPE_ECC_P256_PUBLIC,          },    /* Rebind root license to remote public key. */
    { OEM_HAL_KEY_TYPE_AES_ECB_NEW_LICENSE_ROOT,    OEM_HAL_KEY_TYPE_ECC_P256_PUBLIC,          },    /* Generate local-bound or remote-bound root license content key. */
    { OEM_HAL_KEY_TYPE_AES_ECB_NEW_LICENSE_ROOT,    OEM_HAL_KEY_TYPE_AES_GENERIC_KEY_ROOT,     },    /* Local root license key caching to avoid running out of registers. */
    { OEM_HAL_KEY_TYPE_AES_CTR_CONTENT,             OEM_HAL_KEY_TYPE_ECC_P256_PUBLIC,          },    /* Rebind simple license to remote public key. */
    { OEM_HAL_KEY_TYPE_AES_CTR_NEW_LICENSE_CONTENT, OEM_HAL_KEY_TYPE_ECC_P256_PUBLIC,          },    /* Generate local-bound or remote-bound simple license content key. */
    { OEM_HAL_KEY_TYPE_AES_CTR_NEW_LICENSE_CONTENT, OEM_HAL_KEY_TYPE_AES_ECB_NEW_LICENSE_ROOT, },    /* Bind generated leaf license content key to generated root license content key. */
    { OEM_HAL_KEY_TYPE_AES_CTR_NEW_LICENSE_CONTENT, OEM_HAL_KEY_TYPE_AES_GENERIC_KEY_CONTENT,  },    /* Local leaf/simple license key caching to avoid running out of registers. */
    { OEM_HAL_KEY_TYPE_AES_ECB_MESSAGE_ENCRYPT,     OEM_HAL_KEY_TYPE_ECC_P256_PUBLIC,          },    /* Generate and encrypt MI/MK with target device pubkey. */
#endif /* !DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY */
    { OEM_HAL_KEY_TYPE_AES_OMAC1_SECURE_STORE,      OEM_HAL_KEY_TYPE_AES_KEYFILE,              }     /* Secure store key written to keyfile. */
};

static ValidWrapOrUnwrapPair g_ValidWrapPairsSymOpt[] = {
    { OEM_HAL_KEY_TYPE_AES_CTR_CONTENT,             OEM_HAL_KEY_TYPE_ECC_P256_CONTENT_ENCRYPT  },    /* Symmetrically optimize simple license during license acquisition. */
    { OEM_HAL_KEY_TYPE_AES_CTR_CONTENT,             OEM_HAL_KEY_TYPE_ECC_P256_DOMAIN_ENCRYPT   },    /* Symmetrically optimize simple domain license during license acquisition. */
    { OEM_HAL_KEY_TYPE_AES_ECB_ROOT,                OEM_HAL_KEY_TYPE_ECC_P256_CONTENT_ENCRYPT  },    /* Symmetrically optimize root license during license acquisition. */
    { OEM_HAL_KEY_TYPE_AES_ECB_ROOT,                OEM_HAL_KEY_TYPE_ECC_P256_DOMAIN_ENCRYPT   }     /* Symmetrically optimize root domain license during license acquisition. */
};

static ValidWrapOrUnwrapPair g_ValidWrapPairsSlkCrypto[] = {
    { OEM_HAL_KEY_TYPE_AES_SLK,                     OEM_HAL_KEY_TYPE_ECC_P256_CONTENT_ENCRYPT  }     /* SLK creation encrypted with my ecc priv content key. */
};

static ValidWrapOrUnwrapPair g_ValidWrapPairsLegacyCrypto[] = {
    { OEM_HAL_KEY_TYPE_LEGACY,                      OEM_HAL_KEY_TYPE_INVALID,                  },    /* Cocktail key leaves HAL in the clear. */
    { OEM_HAL_KEY_TYPE_LEGACY,                      OEM_HAL_KEY_TYPE_ECC_P256_CONTENT_ENCRYPT  },    /* Cocktail key re-encrypted with a symmetric key derived from my ecc priv content key in client-side symmetric optimizations during lacq. */
    { OEM_HAL_KEY_TYPE_LEGACY,                      OEM_HAL_KEY_TYPE_ECC_P256_DOMAIN_ENCRYPT   }     /* Cocktail domain-bound key re-encrypted with a symmetric key derived from my ecc priv content key in client-side symmetric optimizations during lacq. */
};

/*
**
** The following function tests (and illustrates) all of the valid and invalid key wrap combinations
**
*/
static DRM_RESULT _TestKeyWrap(
    __inout_ecount( 1 ) DRM_BB_CONTEXT *f_poBBXContext,
    __inout_ecount( 1 ) WrapUnwrapData *f_pTestData )
{
    DRM_RESULT        dr            = DRM_SUCCESS;
    DRM_RESULT        localError    = DRM_SUCCESS;
    OEM_CRYPTO_HANDLE hPublicKey    = f_pTestData->rghWrappingKeys[ OEM_HAL_KEY_TYPE_ECC_P256_PUBLIC ];
    OEM_CRYPTO_HANDLE hRoot200      = OEM_CRYPTO_HANDLE_INVALID;
    OEM_CRYPTO_HANDLE hCont250      = OEM_CRYPTO_HANDLE_INVALID;


    /* Allow the public to be used as a content encryption and message signing key for this test */
    ChkDR( OEM_HAL_TEST_AddPublicKeyUsage( _Hal_GetKeyType( f_poBBXContext, hPublicKey ),
                                           TEST_Hal_GetKeyRegisterIndex( f_poBBXContext, hPublicKey ),
                                           BCERT_KEYUSAGE_BIT( DRM_BCERT_KEYUSAGE_ENCRYPT_KEY ) ) );
    ChkDR( OEM_HAL_TEST_AddPublicKeyUsage( _Hal_GetKeyType( f_poBBXContext, hPublicKey ),
                                           TEST_Hal_GetKeyRegisterIndex( f_poBBXContext, hPublicKey ),
                                           BCERT_KEYUSAGE_BIT( DRM_BCERT_KEYUSAGE_PRND_ENCRYPT_KEY ) ) );

    {
        DRM_LONG         idx1               = 0;
        DRM_LONG         idx2               = 0;
        DRM_DWORD        idx3               = 0;
        OEM_HAL_KEY_TYPE eWrappedKeyType    = OEM_HAL_KEY_TYPE_INVALID;
        OEM_HAL_KEY_TYPE eWrappingKeyType   = OEM_HAL_KEY_TYPE_INVALID;

        const DRM_LONG   idx1Min            = OEM_HAL_KEY_TYPE_INVALID;
        const DRM_LONG   idx1Max            = OEM_HAL_KEY_TYPE_NUM_VALID_TYPES;
        const DRM_LONG   idx2Min            = OEM_HAL_KEY_TYPE_INVALID;
        const DRM_LONG   idx2Max            = OEM_HAL_KEY_TYPE_NUM_VALID_TYPES;

        for( idx1 = idx1Min; idx1 < idx1Max; idx1++ )
        {
            eWrappedKeyType = (OEM_HAL_KEY_TYPE)idx1;
            for( idx2 = idx2Min; idx2 < idx2Max; idx2++ )
            {
                DRM_RESULT drExpected = DRM_SUCCESS;
                eWrappingKeyType = (OEM_HAL_KEY_TYPE)idx2;

                switch( eWrappedKeyType )
                {
                case OEM_HAL_KEY_TYPE_INVALID:                      __fallthrough;
                case OEM_HAL_KEY_TYPE_ECC_P256_PUBLIC:              __fallthrough;
                case OEM_HAL_KEY_TYPE_ECC_P256_MODEL_SIGN:          __fallthrough;
                case OEM_HAL_KEY_TYPE_AES_ECB_DOMAIN_PROTECT:       __fallthrough;
                case OEM_HAL_KEY_TYPE_AES_GENERIC_KEY_CONTENT:      __fallthrough;
                case OEM_HAL_KEY_TYPE_AES_GENERIC_KEY_ROOT:         __fallthrough;
#if DRM_SUPPORT_SECUREOEMHAL
                case OEM_HAL_KEY_TYPE_LEGACY:                       __fallthrough;
#endif /* DRM_SUPPORT_SECUREOEMHAL */
#if DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY
                case OEM_HAL_KEY_TYPE_AES_ECB_NEW_LICENSE_ROOT:     __fallthrough;
                case OEM_HAL_KEY_TYPE_AES_CTR_NEW_LICENSE_CONTENT:  __fallthrough;
#endif  /* DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY */
                case OEM_HAL_KEY_TYPE_AES_KEYFILE:
                    drExpected = DRM_E_OEMHAL_UNSUPPORTED_KEY_TYPE;
                    break;
                default:
                    drExpected = DRM_E_OEMHAL_UNSUPPORTED_KEY_WRAPPING_FORMAT;
                    break;
                }

                for( idx3 = 0; idx3 < NO_OF( g_ValidWrapPairs ); idx3++ )
                {
                    if( g_ValidWrapPairs[idx3].eWrappedKeyType  == eWrappedKeyType
                     && g_ValidWrapPairs[idx3].eWrappingKeyType == eWrappingKeyType )
                    {
                        drExpected = DRM_SUCCESS;
                        break;
                    }
                }
                if( DRM_SYMOPT_IsSymOptSupported() )
                {
                    for( idx3 = 0; idx3 < NO_OF( g_ValidWrapPairsSymOpt ); idx3++ )
                    {
                        if( g_ValidWrapPairsSymOpt[idx3].eWrappedKeyType  == eWrappedKeyType
                         && g_ValidWrapPairsSymOpt[idx3].eWrappingKeyType == eWrappingKeyType )
                        {
                            drExpected = DRM_SUCCESS;
                            break;
                        }
                    }
                }
                if( DRM_BBX_IsSlkCryptoSupported() )
                {
                    for( idx3 = 0; idx3 < NO_OF( g_ValidWrapPairsSlkCrypto ); idx3++ )
                    {
                        if( g_ValidWrapPairsSlkCrypto[idx3].eWrappedKeyType  == eWrappedKeyType
                         && g_ValidWrapPairsSlkCrypto[idx3].eWrappingKeyType == eWrappingKeyType )
                        {
                            drExpected = DRM_SUCCESS;
                            break;
                        }
                    }
                }
                else
                {
                    if( OEM_HAL_KEY_TYPE_AES_SLK == eWrappedKeyType )
                    {
                        drExpected = DRM_E_OEMHAL_UNSUPPORTED_KEY_TYPE;
                    }
                    else if ( OEM_HAL_KEY_TYPE_AES_SLK                 == eWrappingKeyType
                           && OEM_HAL_KEY_TYPE_ECC_P256_DOMAIN_ENCRYPT == eWrappedKeyType)
                    {
                        /*
                        ** In this situation the key handle for the wrapping key is invalid
                        ** and thus _Hal_WrapKey will return the domain public key.
                        */
                        drExpected = DRM_SUCCESS;
                    }
                }
#if !DRM_SUPPORT_SECUREOEMHAL
                for( idx3 = 0; idx3 < NO_OF( g_ValidWrapPairsLegacyCrypto ); idx3++ )
                {
                    if( g_ValidWrapPairsLegacyCrypto[idx3].eWrappedKeyType  == eWrappedKeyType
                     && g_ValidWrapPairsLegacyCrypto[idx3].eWrappingKeyType == eWrappingKeyType )
                    {
                        drExpected = DRM_SUCCESS;
                        break;
                    }
                }
#else /* !DRM_SUPPORT_SECUREOEMHAL */
                if ( OEM_HAL_KEY_TYPE_LEGACY                  == eWrappingKeyType
                  && OEM_HAL_KEY_TYPE_ECC_P256_DOMAIN_ENCRYPT == eWrappedKeyType)
                {
                    /*
                    ** In this situation the key handle for the wrapping key is invalid
                    ** and thus _Hal_WrapKey will return the domain public key.
                    */
                    drExpected = DRM_SUCCESS;
                }
#endif /* !DRM_SUPPORT_SECUREOEMHAL */
#if DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY
                if( OEM_HAL_KEY_TYPE_ECC_P256_DOMAIN_ENCRYPT == eWrappedKeyType )
                {
                    if( OEM_HAL_KEY_TYPE_AES_GENERIC_KEY_CONTENT        == eWrappingKeyType
                     || OEM_HAL_KEY_TYPE_AES_GENERIC_KEY_ROOT           == eWrappingKeyType
                     || OEM_HAL_KEY_TYPE_AES_ECB_NEW_LICENSE_ROOT       == eWrappingKeyType
                     || OEM_HAL_KEY_TYPE_AES_CTR_NEW_LICENSE_CONTENT    == eWrappingKeyType
                     || OEM_HAL_KEY_TYPE_AES_ECB_MESSAGE_ENCRYPT        == eWrappingKeyType )
                    {
                        /*
                        ** In this situation the key handle for the wrapping key is invalid
                        ** and thus _Hal_WrapKey will return the domain public key.
                        */
                        drExpected = DRM_SUCCESS;
                    }
                }
                if( OEM_HAL_KEY_TYPE_AES_ECB_MESSAGE_ENCRYPT == eWrappedKeyType )
                {
                    /*
                    ** Message Encrypt Keys can not be generated/wrapped SecureHalPlayOnly is enabled.
                    */
                    drExpected = DRM_E_OEMHAL_UNSUPPORTED_KEY_TYPE;
                }
#endif /* DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY */
                TestWrap( eWrappedKeyType, eWrappingKeyType, drExpected );
            }
        }
    }

    /*
    ** ChkDR here to trap any error from TestWrap
    */
    ChkDR( dr );

    /*
    ** Now generate some new asymmetric key pairs. When the key pair is generated locally it contains a public component
    ** that can be extracted by 'wrapping' it with an invalid key handle.
    ** These are not listed in the table above because they are only available when the keys are first generated.
    ** After that, they're only available by parsing them out from a binary object,
    ** e.g. from the leaf certificate (current key), from a license (current or historical key), from the domain store (current or historical key), etc.
    */
    ChkDR( _RegenerateEccKeyPairs( f_poBBXContext, f_pTestData ) );

    TestWrap( OEM_HAL_KEY_TYPE_ECC_P256_CONTENT_ENCRYPT,  OEM_HAL_KEY_TYPE_INVALID,                   DRM_SUCCESS );    /* Current ECC device content pubkey available in clear for writing to the certificate during certificate generation. */
    TestWrap( OEM_HAL_KEY_TYPE_ECC_P256_MESSAGE_ENCRYPT,  OEM_HAL_KEY_TYPE_INVALID,                   DRM_SUCCESS );    /* Current ECC device message pubkey available in clear for writing to the certificate during certificate generation. */
    TestWrap( OEM_HAL_KEY_TYPE_ECC_P256_SIGN,             OEM_HAL_KEY_TYPE_INVALID,                   DRM_SUCCESS );    /* Current ECC device signing pubkey available in clear for writing to the certificate during certificate generation. */

    /*
    ** ChkDR here to trap any error from TestWrap
    */
    ChkDR( dr );

    /* Can't even generate new license keys when SecureHalPlayOnly is enabled, let alone wrap them. */
#if !DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY
    {
        DRM_BYTE          rgbKeyBuffer[ ECC_P256_CIPHERTEXT_SIZE_IN_BYTES ];
        DRM_DWORD         cbKeyBuffer   = SIZEOF( rgbKeyBuffer );

        /* Generate a couple keys for some security level testing */
        ChkDR( _Hal_GenerateKey( f_poBBXContext,
                                 OEM_HAL_KEY_TYPE_AES_ECB_NEW_LICENSE_ROOT,
                                 200,
                                 &hRoot200 ) );
        ChkDR( _Hal_GenerateKey( f_poBBXContext,
                                 OEM_HAL_KEY_TYPE_AES_CTR_NEW_LICENSE_CONTENT,
                                 250,
                                 &hCont250 ) );

        /* Verify: Can't wrap a content key with a lower level root key */
        dr = _Hal_WrapKey( f_poBBXContext,
                           hCont250,
                           hRoot200,
                           rgbKeyBuffer,
                           &cbKeyBuffer );
        DRM_TEST_EXPECT_ERROR_AND_MAP_TO_SUCCESS( dr, DRM_E_DEVICE_SECURITY_LEVEL_TOO_LOW );

        /* Verify: Can't wrap a content key with a lower level ecc public key */
        dr = _Hal_WrapKey( f_poBBXContext,
                           hCont250,
                           f_pTestData->rghWrappingKeys[OEM_HAL_KEY_TYPE_ECC_P256_PUBLIC],
                           rgbKeyBuffer,
                           &cbKeyBuffer );
        DRM_TEST_EXPECT_ERROR_AND_MAP_TO_SUCCESS( dr, DRM_E_DEVICE_SECURITY_LEVEL_TOO_LOW );

        /* Verify: Can't wrap a root key with a lower level ecc public key */
        dr = _Hal_WrapKey( f_poBBXContext,
                           hRoot200,
                           f_pTestData->rghWrappingKeys[OEM_HAL_KEY_TYPE_ECC_P256_PUBLIC],
                           rgbKeyBuffer,
                           &cbKeyBuffer );
        DRM_TEST_EXPECT_ERROR_AND_MAP_TO_SUCCESS( dr, DRM_E_DEVICE_SECURITY_LEVEL_TOO_LOW );
    }
#endif  /* !DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY */

ErrorExit:
    DRM_BBX_SafeReleaseKey( f_poBBXContext, &hCont250 );
    DRM_BBX_SafeReleaseKey( f_poBBXContext, &hRoot200 );
    return dr;
}


static DRM_RESULT _TestEncryptWithKeys(
    __inout_ecount( 1 ) DRM_BB_CONTEXT *f_poBBXContext,
    __inout_ecount( 1 ) WrapUnwrapData *f_pTestData )
{
    DRM_RESULT              dr                  = DRM_SUCCESS;
    DRM_LONG                iKeyType            = 0;
    OEM_ENCRYPTION_HANDLE   handleEncryption    = OEM_ENCRYPTION_HANDLE_INVALID;

    /* Data blocks */
    /* _Hal_EncryptDataBlock should fail on all keys except for OEM_HAL_KEY_TYPE_AES_ECB_MESSAGE_ENCRYPT and OEM_HAL_KEY_TYPE_AES_ECB_NEW_LICENSE_ROOT. */
    Log( "", "_TestEncryptWithKeys() - _Hal_EncryptDataBlock() Testing..." );
    for( iKeyType = 0; iKeyType < OEM_HAL_KEY_TYPE_NUM_VALID_TYPES; iKeyType++ )
    {
        DRM_ID     oDrmId                       = { 0 };
        DRM_BYTE   rgbBlock[ SIZEOF( DRM_ID ) ] = { 0 };
        DRM_DWORD  cbBlock                      = SIZEOF( DRM_ID );

        dr = _Hal_EncryptDataBlock( f_poBBXContext,
                                    f_pTestData->rghWrappingKeys[ iKeyType ],
                                    oDrmId.rgb,
                                    SIZEOF( DRM_ID ),
                                    rgbBlock,
                                    &cbBlock );

        Log( "", "\tOEM HAL key type=%d", iKeyType );

        if( !DRM_BBX_IsSlkCryptoSupported() && iKeyType == OEM_HAL_KEY_TYPE_AES_SLK )
        {
            DRM_TEST_EXPECT_ERROR_AND_MAP_TO_SUCCESS( dr, DRM_E_HANDLE );
        }
#if DRM_SUPPORT_SECUREOEMHAL
        else if ( iKeyType == OEM_HAL_KEY_TYPE_LEGACY )
        {
            DRM_TEST_EXPECT_ERROR_AND_MAP_TO_SUCCESS( dr, DRM_E_HANDLE );
        }
#endif  /* DRM_SUPPORT_SECUREOEMHAL */
#if DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY
        else if ( iKeyType == OEM_HAL_KEY_TYPE_AES_ECB_MESSAGE_ENCRYPT
               || iKeyType == OEM_HAL_KEY_TYPE_AES_ECB_NEW_LICENSE_ROOT
               || iKeyType == OEM_HAL_KEY_TYPE_AES_CTR_NEW_LICENSE_CONTENT
               || iKeyType == OEM_HAL_KEY_TYPE_AES_GENERIC_KEY_ROOT
               || iKeyType == OEM_HAL_KEY_TYPE_AES_GENERIC_KEY_CONTENT )
        {
            DRM_TEST_EXPECT_ERROR_AND_MAP_TO_SUCCESS( dr, DRM_E_HANDLE );
        }
#endif  /* DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY */
        else if( ( iKeyType != OEM_HAL_KEY_TYPE_AES_ECB_MESSAGE_ENCRYPT  )
              && ( iKeyType != OEM_HAL_KEY_TYPE_AES_ECB_NEW_LICENSE_ROOT ) )
        {
            DRM_TEST_EXPECT_ERROR_AND_MAP_TO_SUCCESS( dr, DRM_E_OEMHAL_UNSUPPORTED_KEY_TYPE );
        }
        else
        {
            ChkDR( dr );
        }
    }

#if DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY
    {
        dr = _Hal_CreateSampleEncryptionContext( f_poBBXContext,
                                                 f_pTestData->rghWrappingKeys[ OEM_HAL_KEY_TYPE_AES_CTR_NEW_LICENSE_CONTENT ],
                                                 OEM_HAL_SAMPLE_ENCRYPTION_MODE_FULL,
                                                 &handleEncryption );
        DRM_TEST_EXPECT_ERROR_AND_MAP_TO_SUCCESS( dr, DRM_E_HANDLE );
    }
#else   /* DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY */

    /* Sample Data */
    /* _Hal_EncryptSampleData should fail on all keys except for OEM_HAL_KEY_TYPE_AES_CTR_NEW_LICENSE_CONTENT. */
    Log( "", "_TestEncryptWithKeys() - _Hal_EncryptSampleData() Testing..." );
    for( iKeyType = ( OEM_HAL_KEY_TYPE_INVALID - 1 ); iKeyType <= ( OEM_HAL_KEY_TYPE_NUM_VALID_TYPES + 1 ); iKeyType++)
    {
        DRM_UINT64             qwIV              = DRM_UI64LITERAL( 0, 0 );
        DRM_BYTE               rgbData[50]       = { 0 };
        DRM_DWORD              cbData            = SIZEOF( rgbData );

        ChkDR( _Hal_CreateSampleEncryptionContext( f_poBBXContext,
                                                   f_pTestData->rghWrappingKeys[ OEM_HAL_KEY_TYPE_AES_CTR_NEW_LICENSE_CONTENT ],
                                                   OEM_HAL_SAMPLE_ENCRYPTION_MODE_FULL,
                                                   &handleEncryption ) );

        ((OEM_HAL_ENCRYPTION_CONTEXT_INTERNAL *)handleEncryption)->contentKeyType = (OEM_HAL_KEY_TYPE)iKeyType;

        dr = _Hal_EncryptSampleData( f_poBBXContext,
                                     handleEncryption,
                                     &qwIV,
                                     NULL,
                                     OEM_OPAQUE_BUFFER_HANDLE_INVALID,
                                     OEM_OPAQUE_BUFFER_HANDLE_INVALID,
                                     cbData,
                                     rgbData );

        Log( "", "\tOEM HAL key type=%d", iKeyType );

        if( iKeyType == OEM_HAL_KEY_TYPE_AES_CTR_NEW_LICENSE_CONTENT )
        {
            ChkDR( dr );
        }
        else if( ( iKeyType <= OEM_HAL_KEY_TYPE_INVALID ) || ( iKeyType >= OEM_HAL_KEY_TYPE_NUM_VALID_TYPES ) )
        {
            DRM_TEST_EXPECT_ERROR_AND_MAP_TO_SUCCESS( dr, DRM_E_OEMHAL_UNSUPPORTED_KEY_TYPE );
        }
#if DRM_SUPPORT_SECUREOEMHAL
        else if( ( iKeyType == OEM_HAL_KEY_TYPE_LEGACY ) || ( iKeyType == OEM_HAL_KEY_TYPE_AES_SLK ) )
        {
            DRM_TEST_EXPECT_ERROR_AND_MAP_TO_SUCCESS( dr, DRM_E_OEMHAL_UNSUPPORTED_KEY_TYPE );
        }
#endif /* DRM_SUPPORT_SECUREOEMHAL */
        else
        {
            DRM_TEST_EXPECT_ERROR_AND_MAP_TO_SUCCESS( dr, DRM_E_INVALIDARG );
        }

        DRM_BBX_SafeDestroySampleEncryptionContext( &handleEncryption );    /* returns void */
    }
#endif  /* DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY */

    /* Encryption Context */
    /* _Hal_CreateSampleEncryptionContext should fail on all keys except for OEM_HAL_KEY_TYPE_AES_CTR_NEW_LICENSE_CONTENT. */
    Log( "", "_TestEncryptWithKeys() - _Hal_CreateSampleEncryptionContext() Testing..." );
    for( iKeyType = 0; iKeyType < OEM_HAL_KEY_TYPE_NUM_VALID_TYPES; iKeyType++)
    {
        dr = _Hal_CreateSampleEncryptionContext( f_poBBXContext,
                                                 f_pTestData->rghWrappingKeys[ iKeyType ],
                                                 OEM_HAL_SAMPLE_ENCRYPTION_MODE_FULL,
                                                 &handleEncryption );

        Log( "", "\tOEM HAL key type=%d", iKeyType );

        if( !DRM_BBX_IsSlkCryptoSupported() && iKeyType == OEM_HAL_KEY_TYPE_AES_SLK )
        {
            DRM_TEST_EXPECT_ERROR_AND_MAP_TO_SUCCESS( dr, DRM_E_HANDLE );
        }
#if DRM_SUPPORT_SECUREOEMHAL
        else if ( iKeyType == OEM_HAL_KEY_TYPE_LEGACY )
        {
            DRM_TEST_EXPECT_ERROR_AND_MAP_TO_SUCCESS( dr, DRM_E_HANDLE );
        }
#endif  /* DRM_SUPPORT_SECUREOEMHAL */
#if DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY
        else if ( iKeyType == OEM_HAL_KEY_TYPE_AES_ECB_MESSAGE_ENCRYPT
               || iKeyType == OEM_HAL_KEY_TYPE_AES_GENERIC_KEY_CONTENT
               || iKeyType == OEM_HAL_KEY_TYPE_AES_GENERIC_KEY_ROOT
               || iKeyType == OEM_HAL_KEY_TYPE_AES_ECB_NEW_LICENSE_ROOT
               || iKeyType == OEM_HAL_KEY_TYPE_AES_CTR_NEW_LICENSE_CONTENT )
        {
            DRM_TEST_EXPECT_ERROR_AND_MAP_TO_SUCCESS( dr, DRM_E_HANDLE );
        }
#endif  /* DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY */
        else if( iKeyType != OEM_HAL_KEY_TYPE_AES_CTR_NEW_LICENSE_CONTENT )
        {
            DRM_TEST_EXPECT_ERROR_AND_MAP_TO_SUCCESS( dr, DRM_E_INVALIDARG );
        }
        else
        {
            ChkDR( dr );
        }
        DRM_BBX_SafeDestroySampleEncryptionContext( &handleEncryption );    /* returns void */
    }

ErrorExit:
    DRM_BBX_SafeDestroySampleEncryptionContext( &handleEncryption );    /* returns void */
    return dr;
}


DRM_RESULT DRM_CALL TestKeyUnwrapAndWrap( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT       dr           = DRM_SUCCESS;
    WrapUnwrapData   data         = { 0 };
    DRM_BYTE         rgbFake[1]   = { 0 };

    OEM_HAL_TEST_EnableTestWrapUnwrap( TRUE );

    ChkDR( _InitWrapUnwrapData( &g_BBContext, &data ) );

    /*
    ** Disable special test casing, to test the production wrap and unwrap combinations
    */
    OEM_HAL_TEST_EnableTestWrapUnwrap( FALSE );

    ChkDR( _TestKeyUnwrap( &g_BBContext, &data ) );

    ChkDR( _TestEncryptWithKeys( &g_BBContext, &data ) );

    ChkDR( _TestKeyWrap( &g_BBContext, &data ) );

    /*
    ** Verify that public keys from certificates without DRM_BCERT_KEYUSAGE_SIGN_CRL can't be used to verify revocation info or CRLs.
    ** Verifying that public keys from certificates WITH the key usage CAN be used already occurred when _InitWrapUnwrapData called _ValidatePublicKey.
    ** Using fake data (rgbFake) ensures that the key usage is checked before the Rev Info or CRL is even parsed in any way.
    ** (Note: data.rghWrappingKeys[OEM_HAL_KEY_TYPE_ECC_P256_PUBLIC] is pointing to the pubkey from a regular device certificate.)
    ** We do this here instead of in a separate test function or test case in order to avoid having to redo the setup that _InitWrapUnwrapData does.
    */
    dr = _Hal_LoadPlayReadyRevocationInfo( &g_BBContext, rgbFake, SIZEOF( rgbFake ), data.rghWrappingKeys[OEM_HAL_KEY_TYPE_ECC_P256_PUBLIC] );
    DRM_TEST_EXPECT_ERROR_AND_MAP_TO_SUCCESS( dr, DRM_E_OEMHAL_UNSUPPORTED_KEY_TYPE );
    dr = _Hal_LoadPlayReadyCrl( &g_BBContext, rgbFake, SIZEOF( rgbFake ), data.rghWrappingKeys[OEM_HAL_KEY_TYPE_ECC_P256_PUBLIC] );
    DRM_TEST_EXPECT_ERROR_AND_MAP_TO_SUCCESS( dr, DRM_E_OEMHAL_UNSUPPORTED_KEY_TYPE );

    _CleanupWrapUnwrapData( &g_BBContext, &data );

    OEM_HAL_TEST_EnableTestWrapUnwrap( TRUE );

ErrorExit:

    return dr;
}

static DRM_RESULT _TestCreateAndVerifyOMAC1Signature(
    __inout_ecount( 1 )   DRM_BB_CONTEXT    *f_poBBXContext,
    __inout_ecount( 1 )   WrapUnwrapData    *f_pTestData,
    __in                  OEM_HAL_KEY_TYPE   f_eKeyType,
    __in                  eOemTestDataType   f_eTestDataType,
    __in                  DRM_RESULT         f_drExpectedCreateResult,
    __in                  DRM_RESULT         f_drExpectedVerifyResult )
{
          DRM_RESULT         dr                                 = DRM_SUCCESS;
    const DRM_BYTE          *pbUnwrapData                       = NULL;
          DRM_DWORD          cbUnwrapData                       = 0;
    const DRM_BYTE          *pbParamData                        = NULL;
          DRM_DWORD          cbParamData                        = 0;
          DRM_BYTE           rgbHashValue[DRM_SHA1_DIGEST_LEN]  = { 0 };
          DRM_DWORD          cbHashValue                        = DRM_SHA1_DIGEST_LEN;
          OEM_CRYPTO_HANDLE  hSigningKey                        = OEM_CRYPTO_HANDLE_INVALID;
    const DRM_DWORD          cbSig                              = XMR_BASE_OBJECT_LENGTH + XMR_AES_OMAC_SIGNATURE_LENGTH + 2 * SIZEOF( DRM_WORD );

    if( f_eKeyType != OEM_HAL_KEY_TYPE_INVALID )
    {
        hSigningKey = f_pTestData->rghWrappingKeys[ f_eKeyType ];
    }

    switch( f_eTestDataType )
    {
    case eTEST_DATA_XMR_LICENSE_ROOT:
        pbUnwrapData = f_pTestData->RootUnwrapInfo.pbXMRLic;
        cbUnwrapData = f_pTestData->RootUnwrapInfo.cbXMRLic - cbSig;
        break;

    case eTEST_DATA_XMR_LICENSE_SIMPLE:
        pbUnwrapData = f_pTestData->SimpleUnwrapInfo.pbXMRLic;
        cbUnwrapData = f_pTestData->SimpleUnwrapInfo.cbXMRLic - cbSig;
        break;

    case eTEST_DATA_XMR_LICENSE_LEAF:
        pbUnwrapData = f_pTestData->LeafUnwrapInfo.pbXMRLic;
        cbUnwrapData = f_pTestData->LeafUnwrapInfo.cbXMRLic - cbSig;
        break;

    case eTEST_DATA_XMR_LICENSE_DOMAIN_ROOT:
        pbUnwrapData = f_pTestData->RootDomainUnwrapInfo.pbXMRLic;
        cbUnwrapData = f_pTestData->RootDomainUnwrapInfo.cbXMRLic - cbSig;
        break;

    case eTEST_DATA_XMR_LICENSE_DOMAIN_SIMPLE:
        pbUnwrapData = f_pTestData->SimpleDomainUnwrapInfo.pbXMRLic;
        cbUnwrapData = f_pTestData->SimpleDomainUnwrapInfo.cbXMRLic - cbSig;
        break;

    default:
        /* Load the unwrap parameter data */
        ChkDR( _GetUnwrapData( f_pTestData,
                               f_eTestDataType,
                               &pbUnwrapData,
                               &cbUnwrapData,
                               &pbParamData,
                               &cbParamData ) );
        break;
    }

    dr = _Hal_CreateOMAC1Signature( f_poBBXContext,
                                    pbUnwrapData,
                                    cbUnwrapData,
                                    rgbHashValue,
                                    &cbHashValue,
                                    hSigningKey );
    if( dr != f_drExpectedCreateResult )
    {
        Log( "", "_TestCreateAndVerifyOMAC1Signature failure: dr=0x%x(%s)  f_drExpectedCreateResult=0x%x(%s)  f_eKeyType=%d  f_eTestData=%d", dr, DRM_ERR_GetErrorNameFromCode( dr, NULL ), f_drExpectedCreateResult, DRM_ERR_GetErrorNameFromCode( f_drExpectedCreateResult, NULL ), f_eKeyType, f_eTestDataType );
        ChkDR( DRM_E_TEST_UNEXPECTED_OUTPUT );
    }

    if( DRM_SUCCEEDED( dr ) )
    {
        dr = _Hal_VerifyOMAC1Signature( f_poBBXContext,
                                        pbUnwrapData,
                                        cbUnwrapData,
                                        rgbHashValue,
                                        cbHashValue,
                                        hSigningKey );
        if( dr != f_drExpectedVerifyResult )
        {
            Log( "", "_TestCreateAndVerifyOMAC1Signature failure: dr=0x%x(%s)  f_drExpectedVerifyResult=0x%x(%s)  f_eKeyType=%d  f_eTestData=%d", dr, DRM_ERR_GetErrorNameFromCode( dr, NULL ), f_drExpectedVerifyResult, DRM_ERR_GetErrorNameFromCode( f_drExpectedCreateResult, NULL ), f_eKeyType, f_eTestDataType );
            ChkDR( DRM_E_TEST_UNEXPECTED_OUTPUT );
        }
    }

    dr = DRM_SUCCESS;

ErrorExit:
    return dr;
}

#define TestCreateAndVerifyOMAC1Signature( KeyType, TestDataType, ExpectedCreateResult, ExpectedVerifyResult )                                      \
    do {                                                                                                                                            \
        DRM_RESULT drLocal = DRM_SUCCESS;                                                                                                           \
        drLocal = _TestCreateAndVerifyOMAC1Signature( &g_BBContext, &data, KeyType, TestDataType, ExpectedCreateResult, ExpectedVerifyResult );     \
        if( DRM_FAILED( drLocal ) && DRM_SUCCEEDED( dr ) )                                                                                          \
        {                                                                                                                                           \
            dr = drLocal;                                                                                                                           \
        }                                                                                                                                           \
    } while( FALSE )

DRM_RESULT DRM_CALL Test_CreateAndVerifyOMAC1Signature( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT          dr              = DRM_SUCCESS;
    WrapUnwrapData      data            = { 0 };
    DRM_LONG            iKeyType        = 0;
    OEM_HAL_KEY_TYPE    eKeyType        = OEM_HAL_KEY_TYPE_INVALID;

    ChkDR( _InitWrapUnwrapData( &g_BBContext, &data ) );

    /* Test each key type */
    for( iKeyType = OEM_HAL_KEY_TYPE_INVALID; iKeyType < OEM_HAL_KEY_TYPE_NUM_VALID_TYPES; iKeyType++ )
    {
        eKeyType = (OEM_HAL_KEY_TYPE)iKeyType;

        if( !DRM_DOMAIN_IsDomainSupported() )
        {
            if( eKeyType == OEM_HAL_KEY_TYPE_ECC_P256_DOMAIN_ENCRYPT
             || eKeyType == OEM_HAL_KEY_TYPE_AES_ECB_DOMAIN_PROTECT )
            {
                TestCreateAndVerifyOMAC1Signature( eKeyType, eTEST_DATA_DUMMY_32, DRM_E_OEMHAL_UNSUPPORTED_KEY_TYPE, NA );
                continue;
            }
        }

        if( !DRM_BBX_IsSlkCryptoSupported()
         && eKeyType == OEM_HAL_KEY_TYPE_AES_SLK )
        {
            TestCreateAndVerifyOMAC1Signature( eKeyType, eTEST_DATA_DUMMY_32, DRM_E_HANDLE, NA );
            continue;
        }

        switch( eKeyType )
        {
#if DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY
        case OEM_HAL_KEY_TYPE_AES_GENERIC_KEY_CONTENT:      __fallthrough;
        case OEM_HAL_KEY_TYPE_AES_GENERIC_KEY_ROOT:         __fallthrough;
            /*
            ** If SecureHalPlayOnly is on, then Generic Keys can't be generated.
            ** In this case _InitWrapUnwrapData initializes the handle with a bad handle
            ** so we should expect that here.
            */
            TestCreateAndVerifyOMAC1Signature( eKeyType, eTEST_DATA_DUMMY_32, DRM_E_HANDLE, NA );
            break;
#else   /* DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY */
        case OEM_HAL_KEY_TYPE_AES_GENERIC_KEY_CONTENT:      __fallthrough;
        case OEM_HAL_KEY_TYPE_AES_GENERIC_KEY_ROOT:         __fallthrough;
#endif  /* DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY */
        case OEM_HAL_KEY_TYPE_ECC_P256_MESSAGE_ENCRYPT:     __fallthrough;
        case OEM_HAL_KEY_TYPE_ECC_P256_CONTENT_ENCRYPT:     __fallthrough;
        case OEM_HAL_KEY_TYPE_ECC_P256_DOMAIN_ENCRYPT:      __fallthrough;
        case OEM_HAL_KEY_TYPE_ECC_P256_SIGN:                __fallthrough;
        case OEM_HAL_KEY_TYPE_ECC_P256_PUBLIC:              __fallthrough;
        case OEM_HAL_KEY_TYPE_ECC_P256_MODEL_SIGN:          __fallthrough;
        case OEM_HAL_KEY_TYPE_AES_ECB_DOMAIN_PROTECT:       __fallthrough;
        case OEM_HAL_KEY_TYPE_AES_SLK:
            /* Failed cases */
            TestCreateAndVerifyOMAC1Signature( eKeyType, eTEST_DATA_DUMMY_32, DRM_E_OEMHAL_UNSUPPORTED_KEY_TYPE, NA );
            break;

#if DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY
        case OEM_HAL_KEY_TYPE_AES_ECB_MESSAGE_ENCRYPT:
            /*
            ** If SecureHalPlayOnly is on, then ECB Message Encrypt keys (PRND) can't be *generated*,
            ** so we don't have one available at this point.
            ** In this case _InitWrapUnwrapData initializes the handle with a bad handle
            ** so we should expect that here.
            */
            TestCreateAndVerifyOMAC1Signature( eKeyType, eTEST_DATA_DUMMY_32, DRM_E_HANDLE, NA );
            break;
#else   /* DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY */
        case OEM_HAL_KEY_TYPE_AES_ECB_MESSAGE_ENCRYPT:      __fallthrough;
#endif  /* DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY */
        case OEM_HAL_KEY_TYPE_AES_OMAC1_SECURE_STORE:       __fallthrough;
        case OEM_HAL_KEY_TYPE_AES_KEYFILE:
            /* Succeeded cases but the data to be signed aren't checked */
            TestCreateAndVerifyOMAC1Signature( eKeyType, eTEST_DATA_DUMMY_32, DRM_SUCCESS, DRM_SUCCESS );
            break;

        case OEM_HAL_KEY_TYPE_LEGACY:
#if DRM_SUPPORT_SECUREOEMHAL
            /*
            ** If SecureHal is on, then Legacy keys (Cocktail) aren't supported.
            ** In this case _InitWrapUnwrapData initializes the handle with a bad handle
            ** so we should expect that here.
            */
            TestCreateAndVerifyOMAC1Signature( eKeyType, eTEST_DATA_DUMMY_32, DRM_E_HANDLE, NA );
            break;

#else /* DRM_SUPPORT_SECUREOEMHAL */
            /* Succeeded cases but the data to be signed aren't checked */
            TestCreateAndVerifyOMAC1Signature( eKeyType, eTEST_DATA_DUMMY_32, DRM_SUCCESS, DRM_SUCCESS );
            break;

#endif /* DRM_SUPPORT_SECUREOEMHAL */

        case OEM_HAL_KEY_TYPE_AES_ECB_ROOT:
            /*                               | Key Type   | Test Data To Be Signed                | Expected Create Result            | Expected Verify Result*/
#if DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY
            TestCreateAndVerifyOMAC1Signature( eKeyType,    eTEST_DATA_XMR_LICENSE_ROOT,            DRM_E_OEMHAL_UNSUPPORTED_KEY_TYPE,  NA );
#else   /* DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY */
            TestCreateAndVerifyOMAC1Signature( eKeyType,    eTEST_DATA_XMR_LICENSE_ROOT,            DRM_SUCCESS,                        DRM_SUCCESS );
#endif  /* DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY */
            TestCreateAndVerifyOMAC1Signature( eKeyType,    eTEST_DATA_XMR_LICENSE_LEAF,            DRM_E_UNSUPPORTED_ALGORITHM,        NA );
            TestCreateAndVerifyOMAC1Signature( eKeyType,    eTEST_DATA_XMR_LICENSE_SIMPLE,          DRM_E_UNSUPPORTED_ALGORITHM,        NA );
            TestCreateAndVerifyOMAC1Signature( eKeyType,    eTEST_DATA_DUMMY_32,                    DRM_E_INVALID_LICENSE,              NA );
            if( DRM_DOMAIN_IsDomainSupported() )
            {
#if DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY
                TestCreateAndVerifyOMAC1Signature( eKeyType,    eTEST_DATA_XMR_LICENSE_DOMAIN_ROOT,     DRM_E_OEMHAL_UNSUPPORTED_KEY_TYPE,  NA );
#else   /* DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY */
                TestCreateAndVerifyOMAC1Signature( eKeyType,    eTEST_DATA_XMR_LICENSE_DOMAIN_ROOT,     DRM_SUCCESS,                        DRM_SUCCESS );
#endif  /* DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY */
                TestCreateAndVerifyOMAC1Signature( eKeyType,    eTEST_DATA_XMR_LICENSE_DOMAIN_SIMPLE,   DRM_E_UNSUPPORTED_ALGORITHM,        NA );
            }
            break;

        case OEM_HAL_KEY_TYPE_AES_CTR_CONTENT:
            TestCreateAndVerifyOMAC1Signature( eKeyType,    eTEST_DATA_XMR_LICENSE_ROOT,            DRM_E_UNSUPPORTED_ALGORITHM,        NA );
#if DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY
            TestCreateAndVerifyOMAC1Signature( eKeyType,    eTEST_DATA_XMR_LICENSE_LEAF,            DRM_E_OEMHAL_UNSUPPORTED_KEY_TYPE,  NA );
            TestCreateAndVerifyOMAC1Signature( eKeyType,    eTEST_DATA_XMR_LICENSE_SIMPLE,          DRM_E_OEMHAL_UNSUPPORTED_KEY_TYPE,  NA );
#else   /* DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY */
            TestCreateAndVerifyOMAC1Signature( eKeyType,    eTEST_DATA_XMR_LICENSE_LEAF,            DRM_SUCCESS,                        DRM_SUCCESS );
            TestCreateAndVerifyOMAC1Signature( eKeyType,    eTEST_DATA_XMR_LICENSE_SIMPLE,          DRM_SUCCESS,                        DRM_SUCCESS );
#endif  /* DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY */
            TestCreateAndVerifyOMAC1Signature( eKeyType,    eTEST_DATA_DUMMY_32,                    DRM_E_INVALID_LICENSE,              NA );
            if( DRM_DOMAIN_IsDomainSupported() )
            {
                TestCreateAndVerifyOMAC1Signature( eKeyType,    eTEST_DATA_XMR_LICENSE_DOMAIN_ROOT,     DRM_E_UNSUPPORTED_ALGORITHM,        NA );
#if DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY
                TestCreateAndVerifyOMAC1Signature( eKeyType,    eTEST_DATA_XMR_LICENSE_DOMAIN_SIMPLE,   DRM_E_OEMHAL_UNSUPPORTED_KEY_TYPE,  NA );
#else   /* DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY */
                TestCreateAndVerifyOMAC1Signature( eKeyType,    eTEST_DATA_XMR_LICENSE_DOMAIN_SIMPLE,   DRM_SUCCESS,                        DRM_SUCCESS );
#endif  /* DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY */
            }
            break;

        case OEM_HAL_KEY_TYPE_AES_ECB_NEW_LICENSE_ROOT:
#if DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY
            /*
            ** If SecureHalPlayOnly is on, then New License Keys aren't supported.
            ** In this case _InitWrapUnwrapData initializes the handle with a bad handle
            ** so we should expect that here.
            */
            TestCreateAndVerifyOMAC1Signature( eKeyType, eTEST_DATA_DUMMY_32, DRM_E_HANDLE, NA );
            break;
#else   /* DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY */
            TestCreateAndVerifyOMAC1Signature( eKeyType,    eTEST_DATA_XMR_LICENSE_ROOT,            DRM_SUCCESS,                 DRM_E_OEMHAL_UNSUPPORTED_KEY_TYPE );
            TestCreateAndVerifyOMAC1Signature( eKeyType,    eTEST_DATA_XMR_LICENSE_LEAF,            DRM_E_UNSUPPORTED_ALGORITHM, NA );
            TestCreateAndVerifyOMAC1Signature( eKeyType,    eTEST_DATA_XMR_LICENSE_SIMPLE,          DRM_E_UNSUPPORTED_ALGORITHM, NA );
            TestCreateAndVerifyOMAC1Signature( eKeyType,    eTEST_DATA_DUMMY_32,                    DRM_E_INVALID_LICENSE,       NA );
            if( DRM_DOMAIN_IsDomainSupported() )
            {
                TestCreateAndVerifyOMAC1Signature( eKeyType,    eTEST_DATA_XMR_LICENSE_DOMAIN_ROOT,     DRM_SUCCESS,                 DRM_E_OEMHAL_UNSUPPORTED_KEY_TYPE );
                TestCreateAndVerifyOMAC1Signature( eKeyType,    eTEST_DATA_XMR_LICENSE_DOMAIN_SIMPLE,   DRM_E_UNSUPPORTED_ALGORITHM, NA );
            }
            break;
#endif  /* DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY */

        case OEM_HAL_KEY_TYPE_AES_CTR_NEW_LICENSE_CONTENT:
#if DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY
            /*
            ** If SecureHalPlayOnly is on, then New License Keys aren't supported.
            ** In this case _InitWrapUnwrapData initializes the handle with a bad handle
            ** so we should expect that here.
            */
            TestCreateAndVerifyOMAC1Signature( eKeyType, eTEST_DATA_DUMMY_32, DRM_E_HANDLE, NA );
            break;
#else   /* DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY */
            TestCreateAndVerifyOMAC1Signature( eKeyType,    eTEST_DATA_XMR_LICENSE_ROOT,            DRM_E_UNSUPPORTED_ALGORITHM, NA );
            TestCreateAndVerifyOMAC1Signature( eKeyType,    eTEST_DATA_XMR_LICENSE_LEAF,            DRM_SUCCESS,                 DRM_E_OEMHAL_UNSUPPORTED_KEY_TYPE );
            TestCreateAndVerifyOMAC1Signature( eKeyType,    eTEST_DATA_XMR_LICENSE_SIMPLE,          DRM_SUCCESS,                 DRM_E_OEMHAL_UNSUPPORTED_KEY_TYPE );
            TestCreateAndVerifyOMAC1Signature( eKeyType,    eTEST_DATA_DUMMY_32,                    DRM_E_INVALID_LICENSE,       NA );
            if( DRM_DOMAIN_IsDomainSupported() )
            {
                TestCreateAndVerifyOMAC1Signature( eKeyType,    eTEST_DATA_XMR_LICENSE_DOMAIN_ROOT,     DRM_E_UNSUPPORTED_ALGORITHM, NA );
                TestCreateAndVerifyOMAC1Signature( eKeyType,    eTEST_DATA_XMR_LICENSE_DOMAIN_SIMPLE,   DRM_SUCCESS,                 DRM_E_OEMHAL_UNSUPPORTED_KEY_TYPE );
            }
            break;
#endif  /* DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY */

        case OEM_HAL_KEY_TYPE_INVALID:
            TestCreateAndVerifyOMAC1Signature( eKeyType,    eTEST_DATA_DUMMY_32,                    DRM_E_HANDLE,       NA );
            break;

        default:
            /* New key types, need to update this test. */
            ChkDR( DRM_E_TEST_UNEXPECTED_OUTPUT );
            break;
        }
    }

    _CleanupWrapUnwrapData( &g_BBContext, &data );

ErrorExit:
    return dr;
}

static DRM_RESULT _TestCreateAndVerifyMessageSignature(
    __inout_ecount( 1 )   DRM_BB_CONTEXT    *f_poBBXContext,
    __inout_ecount( 1 )   WrapUnwrapData    *f_pTestData,
    __in                  OEM_HAL_KEY_TYPE   f_eKeyType,
    __in                  eOemTestDataType   f_eTestDataType,
    __in                  DRM_RESULT         f_drExpectedCreateResult,
    __in                  DRM_RESULT         f_drExpectedVerifyResult )
{
          DRM_RESULT         dr                                                 = DRM_SUCCESS;
    const DRM_BYTE          *pbUnwrapData                                       = NULL;
          DRM_DWORD          cbUnwrapData                                       = 0;
    const DRM_BYTE          *pbParamData                                        = NULL;
          DRM_DWORD          cbParamData                                        = 0;
          DRM_BYTE           rgbHashValue[ECDSA_P256_SIGNATURE_SIZE_IN_BYTES]   = { 0 };
          DRM_DWORD          cbHashValue                                        = ECDSA_P256_SIGNATURE_SIZE_IN_BYTES;
          OEM_CRYPTO_HANDLE  hSigningKey                                        = OEM_CRYPTO_HANDLE_INVALID;

    if( f_eKeyType != OEM_HAL_KEY_TYPE_INVALID )
    {
        hSigningKey = f_pTestData->rghWrappingKeys[ f_eKeyType ];
    }

    /* Load the unwrap parameter data */
    ChkDR( _GetUnwrapData( f_pTestData,
                           f_eTestDataType,
                           &pbUnwrapData,
                           &cbUnwrapData,
                           &pbParamData,
                           &cbParamData ) );

    dr = _Hal_SignWithPrivateKey( f_poBBXContext,
                                  pbUnwrapData,
                                  cbUnwrapData,
                                  rgbHashValue,
                                  &cbHashValue,
                                  OEM_HAL_HASH_TYPE_SHA256,
                                  OEM_HAL_SIGNATURE_SCHEME_ECDSA_P256,
                                  hSigningKey );
    if( dr != f_drExpectedCreateResult )
    {
        Log( "", "_TestCreateAndVerifyMessageSignature failure: dr=0x%x(%s)  f_drExpectedCreateResult=0x%x(%s)  f_eKeyType=%d  f_eTestData=%d", dr, DRM_ERR_GetErrorNameFromCode( dr, NULL ), f_drExpectedCreateResult, DRM_ERR_GetErrorNameFromCode( f_drExpectedCreateResult, NULL ), f_eKeyType, f_eTestDataType );
        ChkDR( DRM_E_TEST_UNEXPECTED_OUTPUT );
    }

    if( DRM_SUCCEEDED( dr ) )
    {
        dr = _Hal_VerifyMessageSignature( f_poBBXContext,
                                          pbUnwrapData,
                                          cbUnwrapData,
                                          rgbHashValue,
                                          cbHashValue,
                                          OEM_HAL_HASH_TYPE_SHA256,
                                          OEM_HAL_SIGNATURE_SCHEME_ECDSA_P256,
                                          hSigningKey );
        if( dr != f_drExpectedVerifyResult )
        {
            Log( "", "_TestCreateAndVerifyMessageSignature failure: dr=0x%x(%s)  f_drExpectedVerifyResult=0x%x(%s)  f_eKeyType=%d  f_eTestData=%d", dr, DRM_ERR_GetErrorNameFromCode( dr, NULL ), f_drExpectedVerifyResult, DRM_ERR_GetErrorNameFromCode( f_drExpectedVerifyResult, NULL ), f_eKeyType, f_eTestDataType );
            ChkDR( DRM_E_TEST_UNEXPECTED_OUTPUT );
        }
    }

    dr = DRM_SUCCESS;

ErrorExit:
    return dr;
}

#define TestCreateAndVerifyMessageSignature( KeyType, TestDataType, ExpectedCreateResult, ExpectedVerifyResult )                                        \
    do {                                                                                                                                                \
        DRM_RESULT drLocal = DRM_SUCCESS;                                                                                                               \
        drLocal = _TestCreateAndVerifyMessageSignature( &g_BBContext, &data, KeyType, TestDataType, ExpectedCreateResult, ExpectedVerifyResult );       \
        if( DRM_FAILED( drLocal ) && DRM_SUCCEEDED( dr ) )                                                                                              \
        {                                                                                                                                               \
            dr = drLocal;                                                                                                                               \
        }                                                                                                                                               \
    } while( FALSE )

DRM_RESULT DRM_CALL Test_CreateAndVerifyMessageSignature( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT          dr          = DRM_SUCCESS;
    WrapUnwrapData      data        = { 0 };
    DRM_LONG            iKeyType    = 0;
    OEM_HAL_KEY_TYPE    eKeyType    = OEM_HAL_KEY_TYPE_INVALID;

    ChkDR( _InitWrapUnwrapData( &g_BBContext, &data ) );

    /*
    ** First try the valid key types
    */
    
    /*                                 | Key Type To Sign                     | Test Data To Be Signed                      | Expected Create Result              | Expected Verify Result*/
    TestCreateAndVerifyMessageSignature( OEM_HAL_KEY_TYPE_ECC_P256_SIGN,        eTEST_DATA_DUMMY_32,                          DRM_SUCCESS,                          DRM_E_NOTIMPL );    
    TestCreateAndVerifyMessageSignature( OEM_HAL_KEY_TYPE_ECC_P256_MODEL_SIGN,  eTEST_DATA_DUMMY_32,                          DRM_E_BCERT_INVALID_CERT_HEADER_TAG,  DRM_E_NOTIMPL );
    TestCreateAndVerifyMessageSignature( OEM_HAL_KEY_TYPE_ECC_P256_MODEL_SIGN,  eTEST_DATA_CERTIFICATE_WITH_BAD_PUBKEY,       DRM_E_INVALID_DEVICE_CERTIFICATE,     DRM_E_NOTIMPL );
    TestCreateAndVerifyMessageSignature( OEM_HAL_KEY_TYPE_ECC_P256_MODEL_SIGN,  eTEST_DATA_CERTIFICATE_WITH_SWAPPED_PUBKEYS,  DRM_E_INVALID_DEVICE_CERTIFICATE,     DRM_E_NOTIMPL );

    if ( DRM_FAILED( OEM_HAL_TEST_EnableSkipLeafCertificateKeysValidityCheckDuringSignOperation() ) )
    {
        ChkDR( DRM_E_TEST_INCOMPLETE );
    }
    TestCreateAndVerifyMessageSignature( OEM_HAL_KEY_TYPE_ECC_P256_MODEL_SIGN,  eTEST_DATA_CERTIFICATE_WITH_INCREASED_SL,     DRM_E_DEVICE_SECURITY_LEVEL_TOO_LOW,  DRM_E_NOTIMPL );
    if ( DRM_FAILED( OEM_HAL_TEST_DisableSkipLeafCertificateKeysValidityCheckDuringSignOperation() ) )
    {
        ChkDR( DRM_E_TEST_INCOMPLETE );
    }

    /* Note: the Model-Sign success case is covered by every Drm_Initialize() call. */

    /*
    ** Now the invalid key types
    */
    for( iKeyType = OEM_HAL_KEY_TYPE_INVALID; iKeyType < OEM_HAL_KEY_TYPE_NUM_VALID_TYPES; iKeyType++ )
    {
        eKeyType = (OEM_HAL_KEY_TYPE)iKeyType;
        if( !DRM_DOMAIN_IsDomainSupported()
         && ( eKeyType == OEM_HAL_KEY_TYPE_ECC_P256_DOMAIN_ENCRYPT
           || eKeyType == OEM_HAL_KEY_TYPE_AES_ECB_DOMAIN_PROTECT ) )
        {
            TestCreateAndVerifyMessageSignature( eKeyType,
                                                 eTEST_DATA_DUMMY_32,
                                                 DRM_E_OEMHAL_UNSUPPORTED_SIGNATURE_SCHEME,
                                                 NA );
            continue;
        }
        if( !DRM_BBX_IsSlkCryptoSupported()
         && eKeyType == OEM_HAL_KEY_TYPE_AES_SLK )
        {
            /*
            ** In this case _InitWrapUnwrapData initializes the handle with a bad handle
            ** so we should expect that here.
            */
            TestCreateAndVerifyMessageSignature( eKeyType,
                                                 eTEST_DATA_DUMMY_32,
                                                 DRM_E_HANDLE,
                                                 NA );
            continue;
        }

#if DRM_SUPPORT_SECUREOEMHAL
        if( eKeyType == OEM_HAL_KEY_TYPE_LEGACY )
        {
            /*
            ** If SecureHal is on, then Legacy keys (Cocktail) aren't supported.
            ** In this case _InitWrapUnwrapData initializes the handle with a bad handle
            ** so we should expect that here.
            */
            TestCreateAndVerifyMessageSignature( eKeyType,
                                                 eTEST_DATA_DUMMY_32,
                                                 DRM_E_HANDLE,
                                                 NA );
            continue;
        }
#endif /* DRM_SUPPORT_SECUREOEMHAL */

#if DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY
        if( eKeyType == OEM_HAL_KEY_TYPE_AES_ECB_MESSAGE_ENCRYPT
         || eKeyType == OEM_HAL_KEY_TYPE_AES_ECB_NEW_LICENSE_ROOT
         || eKeyType == OEM_HAL_KEY_TYPE_AES_CTR_NEW_LICENSE_CONTENT
         || eKeyType == OEM_HAL_KEY_TYPE_AES_GENERIC_KEY_CONTENT
         || eKeyType == OEM_HAL_KEY_TYPE_AES_GENERIC_KEY_ROOT )
        {
            /*
            ** If SecureHalPlayOnly is on, then several key types aren't supported.
            ** In this case _InitWrapUnwrapData initializes the handle with a bad handle
            ** so we should expect that here.
            ** Even though ECB Message Encrypt keys (PRND) are supported, they can't be *generated*,
            ** so we end up with the same result.
            */
            TestCreateAndVerifyMessageSignature( eKeyType, eTEST_DATA_DUMMY_32, DRM_E_HANDLE, NA );
            continue;
        }
#endif  /* DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY */

        if( eKeyType != OEM_HAL_KEY_TYPE_ECC_P256_SIGN
         && eKeyType != OEM_HAL_KEY_TYPE_ECC_P256_MODEL_SIGN )
        {
            TestCreateAndVerifyMessageSignature( eKeyType,
                                                 eTEST_DATA_DUMMY_32,
                                                 eKeyType != OEM_HAL_KEY_TYPE_INVALID ? DRM_E_OEMHAL_UNSUPPORTED_SIGNATURE_SCHEME : DRM_E_HANDLE,
                                                 NA );
        }
    }

ErrorExit:
    _CleanupWrapUnwrapData( &g_BBContext, &data );
    return dr;
}

static DRM_RESULT _TestDecryptContent(
    __inout_ecount( 1 )   DRM_BB_CONTEXT    *f_poBBXContext,
    __inout_ecount( 1 )   WrapUnwrapData    *f_pTestData,
    __in                  OEM_HAL_KEY_TYPE   f_eKeyType,
    __in                  DRM_RESULT         f_drExpectedResult )
{
    DRM_RESULT                     dr                     = DRM_SUCCESS;
    OEM_CRYPTO_HANDLE              hDecryptKey            = OEM_CRYPTO_HANDLE_INVALID;
    DRM_CIPHER_CONTEXT             oCipherContext;
    DRM_AES_COUNTER_MODE_CONTEXT   oCtrContext            = { 0 };
    DRM_BYTE                       rgbData[32]            = { 0 };

    OEM_SECURE_ZERO_MEMORY( &oCipherContext, SIZEOF( oCipherContext ) );

    if( f_eKeyType != OEM_HAL_KEY_TYPE_INVALID )
    {
        hDecryptKey = f_pTestData->rghWrappingKeys[ f_eKeyType ];
    }

    dr = DRM_BBX_CPHR_InitWithKeyHandle( f_poBBXContext,
                                           &oCipherContext,
                                           eDRM_AES_COUNTER_CIPHER,
                                           hDecryptKey );
    if( DRM_SUCCEEDED( dr ) )
    {
        dr = _Hal_DecryptContent( f_poBBXContext,
                                  &oCipherContext,
                                  &oCtrContext,
                                  SIZEOF( rgbData ),
                                  rgbData );
    }

    if( dr != f_drExpectedResult )
    {
        Log( "", "_TestDecryptContent failure: dr=0x%x(%s)  f_drExpectedResult=0x%x(%s)  f_eKeyType=%d", dr, DRM_ERR_GetErrorNameFromCode( dr, NULL ), f_drExpectedResult, DRM_ERR_GetErrorNameFromCode( f_drExpectedResult, NULL ), f_eKeyType );
        ChkDR( DRM_E_TEST_UNEXPECTED_OUTPUT );
    }

    dr = DRM_SUCCESS;

ErrorExit:
    DRM_BBX_CPHR_Uninitialize( &oCipherContext );
    return dr;
}

#define TestDecryptContent( KeyType, ExpectedDecryptResult )                                        \
    do {                                                                                            \
        DRM_RESULT drLocal = DRM_SUCCESS;                                                           \
        drLocal = _TestDecryptContent( &g_BBContext, &data, KeyType, ExpectedDecryptResult );       \
        if( DRM_FAILED( drLocal ) && DRM_SUCCEEDED( dr ) )                                          \
        {                                                                                           \
            dr = drLocal;                                                                           \
        }                                                                                           \
    } while( FALSE )

DRM_RESULT DRM_CALL Test_DecryptContent( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT          dr          = DRM_SUCCESS;
    WrapUnwrapData      data        = { 0 };
    DRM_LONG            iKeyType    = 0;
    OEM_HAL_KEY_TYPE    eKeyType    = OEM_HAL_KEY_TYPE_INVALID;

    ChkDR( _InitWrapUnwrapData( &g_BBContext, &data ) );

    /* First try the valid key types */
    TestDecryptContent( OEM_HAL_KEY_TYPE_AES_CTR_CONTENT,             DRM_SUCCESS );

#if !DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY
    TestDecryptContent( OEM_HAL_KEY_TYPE_AES_CTR_NEW_LICENSE_CONTENT, DRM_SUCCESS );
#endif  /* !DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY */

    /* Now the invalid key types */
    for( iKeyType = OEM_HAL_KEY_TYPE_INVALID; iKeyType < OEM_HAL_KEY_TYPE_NUM_VALID_TYPES; iKeyType++ )
    {
        eKeyType = (OEM_HAL_KEY_TYPE)iKeyType;

        if( !DRM_DOMAIN_IsDomainSupported()
          && ( eKeyType == OEM_HAL_KEY_TYPE_ECC_P256_DOMAIN_ENCRYPT
                || eKeyType == OEM_HAL_KEY_TYPE_AES_ECB_DOMAIN_PROTECT ) )
        {
            TestDecryptContent( eKeyType, DRM_E_OEMHAL_UNSUPPORTED_KEY_TYPE );
            continue;
        }
        if( !DRM_BBX_IsSlkCryptoSupported()
          && eKeyType == OEM_HAL_KEY_TYPE_AES_SLK )
        {
            /*
            ** In this case _InitWrapUnwrapData initializes the handle with a bad handle
            ** so we should expect that here.
            */
            TestDecryptContent( eKeyType, DRM_E_HANDLE );
            continue;
        }

#if DRM_SUPPORT_SECUREOEMHAL
        if( eKeyType == OEM_HAL_KEY_TYPE_LEGACY )
        {
            /*
            ** If SecureHal is on, then Legacy keys (Cocktail) aren't supported.
            ** In this case _InitWrapUnwrapData initializes the handle with a bad handle
            ** so we should expect that here.
            */
            TestDecryptContent( eKeyType, DRM_E_HANDLE );
            continue;
        }
#endif /* DRM_SUPPORT_SECUREOEMHAL */

#if DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY
        if( eKeyType == OEM_HAL_KEY_TYPE_AES_ECB_MESSAGE_ENCRYPT
         || eKeyType == OEM_HAL_KEY_TYPE_AES_ECB_NEW_LICENSE_ROOT
         || eKeyType == OEM_HAL_KEY_TYPE_AES_CTR_NEW_LICENSE_CONTENT
         || eKeyType == OEM_HAL_KEY_TYPE_AES_GENERIC_KEY_CONTENT
         || eKeyType == OEM_HAL_KEY_TYPE_AES_GENERIC_KEY_ROOT )
        {
            /*
            ** If SecureHalPlayOnly is on, then several key types aren't supported.
            ** In this case _InitWrapUnwrapData initializes the handle with a bad handle
            ** so we should expect that here.
            ** Even though ECB Message Encrypt keys (PRND) are supported, they can't be *generated*,
            ** so we end up with the same result.
            */
            TestDecryptContent( eKeyType, DRM_E_HANDLE );
            continue;
        }
#endif  /* DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY */

        if( eKeyType != OEM_HAL_KEY_TYPE_AES_CTR_CONTENT
         && eKeyType != OEM_HAL_KEY_TYPE_AES_CTR_NEW_LICENSE_CONTENT )
        {
            TestDecryptContent( eKeyType,
                                eKeyType != OEM_HAL_KEY_TYPE_INVALID ? DRM_E_OEMHAL_UNSUPPORTED_KEY_TYPE : DRM_E_HANDLE );
        }
    }

    _CleanupWrapUnwrapData( &g_BBContext, &data );

ErrorExit:
    return dr;
}

DRM_RESULT DRM_CALL Test_HalRevocation( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT                      dr                                      = DRM_SUCCESS;
    DRM_PUBLIC_KEY_CONTEXT          oPubKeyEncrypt                          = {0};
    DRM_PUBLIC_KEY_CONTEXT          oPubKeyRev                              = {0};
    CIPHERTEXT_P256                 oCipherTextMIMK                         = {0};
    OEM_CRYPTO_HANDLE               hAesKeyPair                             = OEM_CRYPTO_HANDLE_INVALID;
    OEM_CRYPTO_HANDLE               hRevKey                                 = OEM_CRYPTO_HANDLE_INVALID;
    DRM_APP_CONTEXT_INTERNAL       *poAppContext                            = NULL;
    DRM_DWORD                       cbWrappedKey                            = SIZEOF( CIPHERTEXT_P256 );
    DRM_BYTE                       *pbRevCert                               = NULL;
    DRM_DWORD                       cbRevCert                               = 0;
    DRM_BYTE                       *pbRevCrl                                = NULL;
    DRM_DWORD                       cbRevCrl                                = 0;
    DRM_BCERT_CHAIN_HEADER          oChainHeader                            = {0};
    DRM_DWORD                       bLeafCertStart                          = 0;
    DRM_DWORD                       dwKeyUsageSet                           = 0;
    DRM_DWORD                       dwCertStartOffset                       = 0;
    DRM_DWORD                       iFoundKey                               = 0;

    ChkArg( argc == 2 );

    GET_SHARED_APP_CONTEXT( poAppContext );

    ChkDR( _GetDeviceECC256PubKey( &g_BBContext, &oPubKeyEncrypt, DRM_BINARY_DEVICE_CERT_KEY_PRND_ENCRYPT ) );

    /* Load revoked cert and crl pair */
    ChkDR( LoadTestFile( NULL, argv[0], &pbRevCert, &cbRevCert ) );
    ChkDR( LoadTestFile( NULL, argv[1], &pbRevCrl,  &cbRevCrl ) );

    /* Reset crl version */
    ChkDR( Oem_HalOemImpl_WriteStoredCRLVersion( 0 ) );
    ChkDR( Oem_HalOemImpl_WriteStoredCRLEntries( 0, NULL ) );

    /* Load the revoked cert and get the key */
    ChkDR( DRM_BCert_GetChainHeader( pbRevCert,
                                     cbRevCert,
                                     &bLeafCertStart,
                                     &oChainHeader ) );


    ChkDR( _ResolveSigningChainKey( &g_BBContext,
                                    pbRevCert,
                                    cbRevCert,
                                    oChainHeader.cCerts,
                                    1,
                                    DRM_BCERT_KEYUSAGE_UNKNOWN,
                                    bLeafCertStart,
                                    &hRevKey ) );

    ChkDR( DRM_BCert_GetPublicKeyByUsage( pbRevCert,
                                          cbRevCert,
                                          0,
                                          DRM_BCERT_KEYUSAGE_SIGN,
                                          &oPubKeyRev.PublicKey,
                                          &dwKeyUsageSet,
                                          &dwCertStartOffset,
                                          &iFoundKey ) );

    ChkDR( DRM_BBX_UnwrapKey( &g_BBContext,
                              OEM_HAL_KEY_TYPE_ECC_P256_PUBLIC,
                              hRevKey,
                              pbRevCert + dwCertStartOffset,
                              cbRevCert - dwCertStartOffset,
                              (const DRM_BYTE*)&iFoundKey,
                              SIZEOF( iFoundKey ),
                              oPubKeyRev.PublicKey.m_rgbPubkey,
                              SIZEOF( oPubKeyRev.PublicKey.m_rgbPubkey ),
                              &oPubKeyRev.hPublicKey ) );

    /* Add ENCRYPT KEY usage to the RevKey */
    ChkDR( OEM_HAL_TEST_AddPublicKeyUsage( _Hal_GetKeyType( &g_BBContext, oPubKeyRev.hPublicKey ),
                                           TEST_Hal_GetKeyRegisterIndex( &g_BBContext, oPubKeyRev.hPublicKey ),
                                           BCERT_KEYUSAGE_BIT( DRM_BCERT_KEYUSAGE_ENCRYPT_KEY ) ) );

    /* Add ENCRYPT KEY usage to the RevKey */
    ChkDR( OEM_HAL_TEST_AddPublicKeyUsage( _Hal_GetKeyType( &g_BBContext, oPubKeyRev.hPublicKey ),
                                           TEST_Hal_GetKeyRegisterIndex( &g_BBContext, oPubKeyRev.hPublicKey ),
                                           BCERT_KEYUSAGE_BIT( DRM_BCERT_KEYUSAGE_PRND_ENCRYPT_KEY ) ) );

#if !DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY
    /* WrapKey return DRM_E_INVALIDARG because the key has not been verified */
    ChkDR( _Hal_GenerateKey( &g_BBContext,
                             OEM_HAL_KEY_TYPE_AES_CTR_NEW_LICENSE_CONTENT,
                             150,
                             &hAesKeyPair ) );

    dr = _Hal_WrapKey( &g_BBContext,
                       hAesKeyPair,
                       oPubKeyRev.hPublicKey,
                       (DRM_BYTE *)&oCipherTextMIMK,
                       &cbWrappedKey );

    DRM_TEST_EXPECT_ERROR_AND_MAP_TO_SUCCESS( dr, DRM_E_INVALIDARG );
#endif  /* !DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY */
    cbWrappedKey = SIZEOF( CIPHERTEXT_P256 );

    /* Validate the key and expect the key to be revoked */
    ChkDR( _ValidatePublicKey( &g_BBContext,
                               &oPubKeyRev,
                               pbRevCert,
                               cbRevCert,
                               pbRevCrl,
                               cbRevCrl,
                               NULL,
                               0 ) );

    dr = _Hal_WrapKey( &g_BBContext,
                         hAesKeyPair,
                         oPubKeyRev.hPublicKey,
                         (DRM_BYTE *)&oCipherTextMIMK,
                         &cbWrappedKey );
#if DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY
    DRM_TEST_EXPECT_ERROR_AND_MAP_TO_SUCCESS( dr, DRM_E_OEMHAL_UNSUPPORTED_KEY_TYPE );
#else   /* DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY */
    DRM_TEST_EXPECT_ERROR_AND_MAP_TO_SUCCESS( dr, DRM_E_CERTIFICATE_REVOKED );
#endif  /* DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY */
    DRM_BBX_SafeReleaseKey( &g_BBContext, &hAesKeyPair );
    cbWrappedKey = SIZEOF( CIPHERTEXT_P256 );

#if !DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY
    /* Make sure non-revoked key can still be used */
    ChkDR( _Hal_GenerateKey( &g_BBContext,
                             OEM_HAL_KEY_TYPE_AES_ECB_MESSAGE_ENCRYPT,
                             150,
                             &hAesKeyPair ) );

    ChkDR( _Hal_WrapKey( &g_BBContext,
                         hAesKeyPair,
                         oPubKeyEncrypt.hPublicKey,
                         (DRM_BYTE *)&oCipherTextMIMK,
                         &cbWrappedKey ) );
    DRM_BBX_SafeReleaseKey( &g_BBContext, &hAesKeyPair );
#endif  /* !DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY */

ErrorExit:
    SAFE_OEM_FREE( pbRevCert );
    SAFE_OEM_FREE( pbRevCrl );

    DRM_BBX_CleanupPublicKeyContext( &g_BBContext, &oPubKeyRev );
    DRM_BBX_CleanupPublicKeyContext( &g_BBContext, &oPubKeyEncrypt );

    DRM_BBX_SafeReleaseKey( &g_BBContext, &hAesKeyPair );
    DRM_BBX_SafeReleaseKey( &g_BBContext, &hRevKey );

    return dr;
}


DRM_RESULT DRM_CALL Test_InvalidPubKeyWrapUsage( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT                      dr                                      = DRM_SUCCESS;
    DRM_PUBLIC_KEY_CONTEXT          oPubKeySign                             = {0};
    DRM_PUBLIC_KEY_CONTEXT          oPubKeyEncrypt                          = {0};
    DRM_PUBLIC_KEY_CONTEXT          oPubKeyPrndEncrypt                      = {0};
    CIPHERTEXT_P256                 oCipherText                             = {0};
    OEM_CRYPTO_HANDLE               hKeyPair                                = OEM_CRYPTO_HANDLE_INVALID;
    OEM_CRYPTO_HANDLE               hKeyFileKey                             = OEM_CRYPTO_HANDLE_INVALID;
    OEM_CRYPTO_HANDLE               hPrivKey                                = OEM_CRYPTO_HANDLE_INVALID;
    OEM_CRYPTO_HANDLE               hSignCrlKey                             = OEM_CRYPTO_HANDLE_INVALID;
    DRM_BYTE                       *pRIV                                    = NULL;
    DRM_DWORD                       cbRIV                                   = 0;
    DRM_BYTE                       *pbDeviceCert                            = NULL;
    DRM_DWORD                       cbDeviceCert                            = 0;
    DRM_APP_CONTEXT_INTERNAL       *poAppContext                            = NULL;
    DRM_DWORD                       cbWrappedKey                            = SIZEOF( CIPHERTEXT_P256 );
    DRM_LONG                        iKeyType                                = 0;
    DRM_DWORD                       iPubKey                                 = 0;
    OEM_CRYPTO_HANDLE               rghPubKey[4]                            = {0};
    DRM_BCRL_Signed                 bcrl                                    = {0};
    DRM_BCERT_CHAIN_HEADER          oChainHeader                            = {0};
    DRM_DWORD                       bLeafCertStart                          = 0;

    GET_SHARED_APP_CONTEXT( poAppContext );

    ChkDR( _GetDeviceECC256PubKey( &g_BBContext, &oPubKeySign,        DRM_BINARY_DEVICE_CERT_KEY_SIGN ) );
    ChkDR( _GetDeviceECC256PubKey( &g_BBContext, &oPubKeyEncrypt,     DRM_BINARY_DEVICE_CERT_KEY_ENCRYPT ) );
    ChkDR( _GetDeviceECC256PubKey( &g_BBContext, &oPubKeyPrndEncrypt, DRM_BINARY_DEVICE_CERT_KEY_PRND_ENCRYPT ) );

    rghPubKey[0] = oPubKeySign.hPublicKey;
    rghPubKey[1] = oPubKeyEncrypt.hPublicKey;
    rghPubKey[2] = oPubKeyPrndEncrypt.hPublicKey;

    ChkDR( _GetRivAndCrl( poAppContext,
                          &pRIV,
                          &cbRIV,
                          poAppContext->pbRevocationBuffer,
                          poAppContext->cbRevocationBuffer ) );

    cbDeviceCert = MAX_DEVICE_CERT_SIZE;
    ChkMem( pbDeviceCert = (DRM_BYTE*)Oem_MemAlloc( cbDeviceCert ) );
    ChkDR( TST_UTL_GetPlayReadyCert( pbDeviceCert, &cbDeviceCert, &g_BBContext.CryptoContext ) );

    /* Set the CRL and RevInfo */
    ChkDR( _ValidatePublicKey( &g_BBContext,
                               &oPubKeySign,
                               pbDeviceCert,
                               cbDeviceCert,
                               poAppContext->pbRevocationBuffer,
                               poAppContext->cbRevocationBuffer,
                               pRIV,
                               cbRIV ) );

    /* Get SIGN_CRL Pub Key */
    ChkDR( DRM_BCrl_ParseSignedCrl( poAppContext->pbRevocationBuffer,
                                    poAppContext->cbRevocationBuffer,
                                    NULL,
                                    &bcrl ) );

    ChkDR( DRM_BCert_GetChainHeader( bcrl.pbCertificateChain,
                                     bcrl.cbCertificateChain,
                                     &bLeafCertStart,
                                     &oChainHeader ) );

    ChkDR( _ResolveSigningChainKey( &g_BBContext,
                                    bcrl.pbCertificateChain,
                                    bcrl.cbCertificateChain,
                                    oChainHeader.cCerts,
                                    0,
                                    DRM_BCERT_KEYUSAGE_SIGN_CRL,
                                    bLeafCertStart,
                                    &hSignCrlKey ) );
    rghPubKey[3] = hSignCrlKey;

    /* test for invalid key usage */
    for( iPubKey = 0; iPubKey < NO_OF( rghPubKey ); iPubKey++ )
    {
        for( iKeyType = OEM_HAL_KEY_TYPE_INVALID + 1; iKeyType < OEM_HAL_KEY_TYPE_NUM_VALID_TYPES; iKeyType++ )
        {
            DRM_BOOL                        fCachedHandle                           = FALSE;
            DRM_BB_KEY_REGISTER_ENTRY      *pKeyEntry                               = NULL;
            DRM_RESULT                      drExpected                              = DRM_SUCCESS;

            Log( "", "Test_InvalidPubKeyWrapUsage: iPubKey=%d  iKeyType=%d", iPubKey, iKeyType );

#if DRM_SUPPORT_SECUREOEMHAL
            /* We can't even allocate key handles of certain types when SecureHal is enabled. */
            switch( iKeyType )
            {
#if DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY
            /* We can allocate even fewer types when SecureHalPlayOnly is enabled. */
            case OEM_HAL_KEY_TYPE_AES_ECB_NEW_LICENSE_ROOT:     __fallthrough;
            case OEM_HAL_KEY_TYPE_AES_CTR_NEW_LICENSE_CONTENT:  __fallthrough;
            case OEM_HAL_KEY_TYPE_AES_GENERIC_KEY_CONTENT:      __fallthrough;
            case OEM_HAL_KEY_TYPE_AES_GENERIC_KEY_ROOT:         __fallthrough;
#endif  /* DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY */
            case OEM_HAL_KEY_TYPE_LEGACY:                       __fallthrough;
            case OEM_HAL_KEY_TYPE_AES_SLK:
                dr = _Hal_AllocateKeyHandle(
                    &g_BBContext,
                    ( OEM_HAL_KEY_TYPE ) iKeyType,
                    NULL,
                    0,
                    &hKeyPair,
                    &pKeyEntry,
                    &fCachedHandle );
                if( iKeyType == OEM_HAL_KEY_TYPE_AES_SLK )
                {
                    DRM_TEST_EXPECT_ERROR_AND_MAP_TO_SUCCESS( dr, DRM_E_NOTIMPL );
                }
                else
                {
                    DRM_TEST_EXPECT_ERROR_AND_MAP_TO_SUCCESS( dr, DRM_E_OEMHAL_UNSUPPORTED_KEY_TYPE );
                }
                continue; /* move on to the next key type - nothing more to test if we couldn't allocate it at all. */
            default:
                /* no-op: continue with test */
                break;
            }
#endif  /* DRM_SUPPORT_SECUREOEMHAL */

            ChkDR( _Hal_AllocateKeyHandle(
                &g_BBContext,
                ( OEM_HAL_KEY_TYPE ) iKeyType,
                NULL,
                0,
                &hKeyPair,
                &pKeyEntry,
                &fCachedHandle ) );

            dr = _Hal_WrapKey(
                &g_BBContext,
                hKeyPair,
                rghPubKey[iPubKey],
                (DRM_BYTE *)&oCipherText,
                &cbWrappedKey );

            if(    rghPubKey[iPubKey] == oPubKeySign.hPublicKey
                || rghPubKey[iPubKey] == hSignCrlKey )
            {
                switch( iKeyType )
                {
                case OEM_HAL_KEY_TYPE_INVALID:                      __fallthrough;
                case OEM_HAL_KEY_TYPE_ECC_P256_CONTENT_ENCRYPT:     __fallthrough;
                case OEM_HAL_KEY_TYPE_ECC_P256_DOMAIN_ENCRYPT:      __fallthrough;
                case OEM_HAL_KEY_TYPE_ECC_P256_MESSAGE_ENCRYPT:     __fallthrough;
                case OEM_HAL_KEY_TYPE_ECC_P256_SIGN:                __fallthrough;
                case OEM_HAL_KEY_TYPE_AES_OMAC1_SECURE_STORE:       __fallthrough;
                case OEM_HAL_KEY_TYPE_LEGACY:                       __fallthrough;
                case OEM_HAL_KEY_TYPE_AES_SLK:
                    drExpected = DRM_E_OEMHAL_UNSUPPORTED_KEY_WRAPPING_FORMAT;
                    break;
                default:
                    drExpected = DRM_E_OEMHAL_UNSUPPORTED_KEY_TYPE;
                    break;
                }
            }
            else if( rghPubKey[iPubKey] == oPubKeyEncrypt.hPublicKey )
            {
                switch( iKeyType )
                {
                case OEM_HAL_KEY_TYPE_AES_ECB_ROOT:                 __fallthrough;
                case OEM_HAL_KEY_TYPE_AES_CTR_CONTENT:              __fallthrough;
                case OEM_HAL_KEY_TYPE_AES_ECB_NEW_LICENSE_ROOT:     __fallthrough;
                case OEM_HAL_KEY_TYPE_AES_CTR_NEW_LICENSE_CONTENT:
#if DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY
                    /*
                    ** If SecureHalPlayOnly is on, then none of these keys can
                    ** be encrypted by a public encryption key.
                    */
                    __fallthrough;
#else   /* DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY */
                    drExpected = DRM_SUCCESS;
                    break;
#endif  /* DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY */
                case OEM_HAL_KEY_TYPE_INVALID:                      __fallthrough;
                case OEM_HAL_KEY_TYPE_ECC_P256_CONTENT_ENCRYPT:     __fallthrough;
                case OEM_HAL_KEY_TYPE_ECC_P256_DOMAIN_ENCRYPT:      __fallthrough;
                case OEM_HAL_KEY_TYPE_ECC_P256_MESSAGE_ENCRYPT:     __fallthrough;
                case OEM_HAL_KEY_TYPE_ECC_P256_SIGN:                __fallthrough;
                case OEM_HAL_KEY_TYPE_AES_OMAC1_SECURE_STORE:       __fallthrough;
                case OEM_HAL_KEY_TYPE_LEGACY:                       __fallthrough;
                case OEM_HAL_KEY_TYPE_AES_SLK:
                    drExpected = DRM_E_OEMHAL_UNSUPPORTED_KEY_WRAPPING_FORMAT;
                    break;
                default:
                    drExpected = DRM_E_OEMHAL_UNSUPPORTED_KEY_TYPE;
                    break;
                }
            }
            else if( rghPubKey[iPubKey] == oPubKeyPrndEncrypt.hPublicKey )
            {
                switch( iKeyType )
                {
                case OEM_HAL_KEY_TYPE_AES_ECB_MESSAGE_ENCRYPT:
#if DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY
                    /*
                    ** If SecureHalPlayOnly is on, then this key can't
                    ** be encrypted by a prnd encryption key.
                    */
                    __fallthrough;
#else   /* DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY */
                    drExpected = DRM_SUCCESS;
                    break;
#endif  /* DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY */
                case OEM_HAL_KEY_TYPE_INVALID:                      __fallthrough;
                case OEM_HAL_KEY_TYPE_ECC_P256_CONTENT_ENCRYPT:     __fallthrough;
                case OEM_HAL_KEY_TYPE_ECC_P256_DOMAIN_ENCRYPT:      __fallthrough;
                case OEM_HAL_KEY_TYPE_ECC_P256_MESSAGE_ENCRYPT:     __fallthrough;
                case OEM_HAL_KEY_TYPE_ECC_P256_SIGN:                __fallthrough;
                case OEM_HAL_KEY_TYPE_AES_OMAC1_SECURE_STORE:       __fallthrough;
                case OEM_HAL_KEY_TYPE_LEGACY:                       __fallthrough;
                case OEM_HAL_KEY_TYPE_AES_SLK:
                    drExpected = DRM_E_OEMHAL_UNSUPPORTED_KEY_WRAPPING_FORMAT;
                    break;
                default:
                    drExpected = DRM_E_OEMHAL_UNSUPPORTED_KEY_TYPE;
                    break;
                }
            }

            if( dr != drExpected )
            {
                Log( "", "Test_PubKeyWrapUsage failure: dr=0x%x(%s)  drExpected=0x%x(%s)  iPubKey=%x  KeyType=%d", dr, DRM_ERR_GetErrorNameFromCode( dr, NULL ), drExpected, DRM_ERR_GetErrorNameFromCode( drExpected, NULL ), iPubKey, iKeyType );
                ChkBOOL( dr == drExpected, DRM_E_TEST_UNEXPECTED_OUTPUT );
            }

            dr = DRM_SUCCESS;

            DRM_BBX_SafeReleaseKey( &g_BBContext, &hKeyPair );
            cbWrappedKey = SIZEOF( CIPHERTEXT_P256 );
        }
    }

ErrorExit:
    SAFE_OEM_FREE( pbDeviceCert );

    DRM_BBX_CleanupPublicKeyContext( &g_BBContext, &oPubKeySign );
    DRM_BBX_CleanupPublicKeyContext( &g_BBContext, &oPubKeyEncrypt );
    DRM_BBX_CleanupPublicKeyContext( &g_BBContext, &oPubKeyPrndEncrypt );

    DRM_BBX_SafeReleaseKey( &g_BBContext, &hKeyPair );
    DRM_BBX_SafeReleaseKey( &g_BBContext, &hPrivKey );
    DRM_BBX_SafeReleaseKey( &g_BBContext, &hKeyFileKey );
    DRM_BBX_SafeReleaseKey( &g_BBContext, &hSignCrlKey );

    return dr;
}

DRM_RESULT DRM_CALL Test_InvalidPubKeySignUsage( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT                      dr                                      = DRM_SUCCESS;
    DRM_PUBLIC_KEY_CONTEXT          oPubKeySign                             = {0};
    DRM_PUBLIC_KEY_CONTEXT          oPubKeyEncrypt                          = {0};
    DRM_PUBLIC_KEY_CONTEXT          oPubKeyPrndEncrypt                      = {0};
    DRM_BYTE                       *pRIV                                    = NULL;
    DRM_DWORD                       cbRIV                                   = 0;
    DRM_BYTE                       *pbDeviceCert                            = NULL;
    DRM_DWORD                       cbDeviceCert                            = 0;
    DRM_DWORD                       iPubKey                                 = 0;
    DRM_APP_CONTEXT_INTERNAL       *poAppContext                            = NULL;
    WrapUnwrapData                  data                                    = {0};
    OEM_HAL_KEY_TYPE                rgKeyType[]                             = { OEM_HAL_KEY_TYPE_ECC_P256_SIGN,
                                                                                OEM_HAL_KEY_TYPE_ECC_P256_CONTENT_ENCRYPT,
                                                                                OEM_HAL_KEY_TYPE_ECC_P256_MESSAGE_ENCRYPT };
    DRM_PUBLIC_KEY_CONTEXT         *rgpPubKey[3]                            = {0};

    rgpPubKey[0] = &oPubKeySign;
    rgpPubKey[1] = &oPubKeyEncrypt;
    rgpPubKey[2] = &oPubKeyPrndEncrypt;

    GET_SHARED_APP_CONTEXT( poAppContext );


    ChkDR( _GetDeviceECC256PubKey( &g_BBContext, &oPubKeySign,        DRM_BINARY_DEVICE_CERT_KEY_SIGN ) );
    ChkDR( _GetDeviceECC256PubKey( &g_BBContext, &oPubKeyEncrypt,     DRM_BINARY_DEVICE_CERT_KEY_ENCRYPT ) );
    ChkDR( _GetDeviceECC256PubKey( &g_BBContext, &oPubKeyPrndEncrypt, DRM_BINARY_DEVICE_CERT_KEY_PRND_ENCRYPT ) );

    ChkDR( _GetRivAndCrl( poAppContext,
                          &pRIV,
                          &cbRIV,
                          poAppContext->pbRevocationBuffer,
                          poAppContext->cbRevocationBuffer ) );

    cbDeviceCert = MAX_DEVICE_CERT_SIZE;
    ChkMem( pbDeviceCert = (DRM_BYTE*)Oem_MemAlloc( cbDeviceCert ) );
    ChkDR( TST_UTL_GetPlayReadyCert( pbDeviceCert, &cbDeviceCert, &g_BBContext.CryptoContext ) );

    /* Set the CRL and RevInfo */
    ChkDR( _ValidatePublicKey( &g_BBContext,
                               &oPubKeySign,
                               pbDeviceCert,
                               cbDeviceCert,
                               poAppContext->pbRevocationBuffer,
                               poAppContext->cbRevocationBuffer,
                               pRIV,
                               cbRIV ) );

    ChkDR( _InitWrapUnwrapData( &g_BBContext, &data ) );

    for( iPubKey = 0; iPubKey < NO_OF( rgpPubKey ); iPubKey++ )
    {
        const DRM_BYTE          *pbUnwrapData                                       = NULL;
              DRM_DWORD          cbUnwrapData                                       = 0;
        const DRM_BYTE          *pbParamData                                        = NULL;
              DRM_DWORD          cbParamData                                        = 0;
              DRM_BYTE           rgbHashValue[ECDSA_P256_SIGNATURE_SIZE_IN_BYTES]   = {0};
              DRM_DWORD          cbHashValue                                        = ECDSA_P256_SIGNATURE_SIZE_IN_BYTES;
              DRM_ENCRYPTED_KEY  oEncKey                                            = {0};
              OEM_CRYPTO_HANDLE  hKeyFileKey                                        = OEM_CRYPTO_HANDLE_INVALID;
              OEM_CRYPTO_HANDLE  hPrivKey                                           = OEM_CRYPTO_HANDLE_INVALID;

        /* Load the unwrap parameter data */
        ChkDR( _GetUnwrapData( &data,
                    eTEST_DATA_DUMMY_32,
                    &pbUnwrapData,
                    &cbUnwrapData,
                    &pbParamData,
                    &cbParamData ) );

        ChkDR( TST_KF_GetPrivateKey( g_BBContext.pKeyFileContext,
                                     eKF_KEY_TYPE_ECC_256,
                                     (const DRM_BYTE *)&rgpPubKey[ iPubKey ]->PublicKey,
                                     SIZEOF( PUBKEY_P256 ),
                                     &oEncKey ) );

        ChkDR( _Hal_GetPreloadedKey( &g_BBContext,
                                     OEM_HAL_KEY_TYPE_AES_KEYFILE,
                                     &oEncKey.guidKeypairDecryptionKeyID,
                                     &hKeyFileKey ) );

        ChkDR( _Hal_UnwrapKey( &g_BBContext,
                               rgKeyType[ iPubKey ],
                               hKeyFileKey,
                               ( DRM_BYTE *)&oEncKey,
                               SIZEOF( oEncKey ),
                               NULL,
                               0,
                               oEncKey.pbEncryptedKey,
                               oEncKey.cbEncryptedKey,
                               &hPrivKey ) );


        dr = _Hal_SignWithPrivateKey( &g_BBContext,
                                      pbUnwrapData,
                                      cbUnwrapData,
                                      rgbHashValue,
                                      &cbHashValue,
                                      OEM_HAL_HASH_TYPE_SHA256,
                                      OEM_HAL_SIGNATURE_SCHEME_ECDSA_P256,
                                      hPrivKey );

        if( rgpPubKey[ iPubKey ] != &oPubKeySign )
        {
            DRM_TEST_EXPECT_ERROR_AND_MAP_TO_SUCCESS( dr, DRM_E_OEMHAL_UNSUPPORTED_SIGNATURE_SCHEME );
        }

        ChkDR( dr );
        DRM_BBX_SafeReleaseKey( &g_BBContext, &hPrivKey );
        DRM_BBX_SafeReleaseKey( &g_BBContext, &hKeyFileKey );
    }

ErrorExit:
    SAFE_OEM_FREE( pbDeviceCert );

    _CleanupWrapUnwrapData( &g_BBContext, &data );

    DRM_BBX_CleanupPublicKeyContext( &g_BBContext, &oPubKeySign );
    DRM_BBX_CleanupPublicKeyContext( &g_BBContext, &oPubKeyEncrypt );
    DRM_BBX_CleanupPublicKeyContext( &g_BBContext, &oPubKeyPrndEncrypt );

    return dr;
}


DRM_RESULT DRM_CALL Test_InvalidKeyFileUsage( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT                      dr                                      = DRM_SUCCESS;
    DRM_PUBLIC_KEY_CONTEXT          oPubKeySign                             = {0};
    DRM_PUBLIC_KEY_CONTEXT          oPubKeyEncrypt                          = {0};
    DRM_PUBLIC_KEY_CONTEXT          oPubKeyPrndEncrypt                      = {0};
    OEM_CRYPTO_HANDLE               hKeyFileKey                             = OEM_CRYPTO_HANDLE_INVALID;
    OEM_CRYPTO_HANDLE               hPrivKey                                = OEM_CRYPTO_HANDLE_INVALID;
    DRM_BYTE                       *pRIV                                    = NULL;
    DRM_DWORD                       cbRIV                                   = 0;
    DRM_BYTE                       *pbDeviceCert                            = NULL;
    DRM_DWORD                       cbDeviceCert                            = 0;
    DRM_APP_CONTEXT_INTERNAL       *poAppContext                            = NULL;
    DRM_ENCRYPTED_KEY               oEncKey                                 = {0};
    DRM_KF_KEY_TYPE                 rgKFKeyType[]                           = { eKF_KEY_TYPE_ECC_256, eKF_KEY_SECURE_STORE };
    DRM_LONG                        iKeyFileType                            = 0;
    DRM_LONG                        iKeyType                                = 0;
    DRM_DWORD                       iPubKey                                 = 0;
    DRM_PUBLIC_KEY_CONTEXT         *rgpPubKey[3]                            = {0};

    rgpPubKey[0] = &oPubKeySign;
    rgpPubKey[1] = &oPubKeyEncrypt;
    rgpPubKey[2] = &oPubKeyPrndEncrypt;

    GET_SHARED_APP_CONTEXT( poAppContext );
    ChkDR( _GetDeviceECC256PubKey( &g_BBContext, &oPubKeySign,        DRM_BINARY_DEVICE_CERT_KEY_SIGN ) );
    ChkDR( _GetDeviceECC256PubKey( &g_BBContext, &oPubKeyEncrypt,     DRM_BINARY_DEVICE_CERT_KEY_ENCRYPT ) );
    ChkDR( _GetDeviceECC256PubKey( &g_BBContext, &oPubKeyPrndEncrypt, DRM_BINARY_DEVICE_CERT_KEY_PRND_ENCRYPT ) );

    ChkDR( _GetRivAndCrl( poAppContext,
                          &pRIV,
                          &cbRIV,
                          poAppContext->pbRevocationBuffer,
                          poAppContext->cbRevocationBuffer ) );

    cbDeviceCert = MAX_DEVICE_CERT_SIZE;
    ChkMem( pbDeviceCert = (DRM_BYTE*)Oem_MemAlloc( cbDeviceCert ) );
    ChkDR( TST_UTL_GetPlayReadyCert( pbDeviceCert, &cbDeviceCert, &g_BBContext.CryptoContext ) );

    /* Set the CRL and RevInfo */
    ChkDR( _ValidatePublicKey( &g_BBContext,
                               &oPubKeySign,
                               pbDeviceCert,
                               cbDeviceCert,
                               poAppContext->pbRevocationBuffer,
                               poAppContext->cbRevocationBuffer,
                               pRIV,
                               cbRIV ) );

    for( iKeyFileType = 0; iKeyFileType < NO_OF( rgKFKeyType ); iKeyFileType++)
    {
        DRM_DWORD  dwMaxPubKey = 1;

        if( rgKFKeyType[ iKeyFileType ] == eKF_KEY_TYPE_ECC_256 )
        {
            dwMaxPubKey = NO_OF( rgpPubKey );
        }

        for( iPubKey = 0; iPubKey < dwMaxPubKey; iPubKey++ )
        {
            OEM_SECURE_ZERO_MEMORY( &oEncKey, SIZEOF( oEncKey ) );

            ChkDR( TST_KF_GetPrivateKey( g_BBContext.pKeyFileContext,
                        rgKFKeyType[ iKeyFileType ],
                        rgKFKeyType[ iKeyFileType ] == eKF_KEY_TYPE_ECC_256 ? (const DRM_BYTE *)&rgpPubKey[iPubKey]->PublicKey : NULL,
                        rgKFKeyType[ iKeyFileType ] == eKF_KEY_TYPE_ECC_256 ? SIZEOF( rgpPubKey[iPubKey]->PublicKey ) : 0,
                        &oEncKey ) );
            ChkDR( _Hal_GetPreloadedKey( &g_BBContext,
                        OEM_HAL_KEY_TYPE_AES_KEYFILE,
                        &oEncKey.guidKeypairDecryptionKeyID,
                        &hKeyFileKey ) );

            for( iKeyType = OEM_HAL_KEY_TYPE_INVALID; iKeyType < OEM_HAL_KEY_TYPE_NUM_VALID_TYPES; iKeyType++ )
            {
                DRM_RESULT drExpected  = DRM_SUCCESS;

                dr = _Hal_UnwrapKey( &g_BBContext,
                        (OEM_HAL_KEY_TYPE) iKeyType,
                        hKeyFileKey,
                        ( const DRM_BYTE *)&oEncKey,
                        SIZEOF( DRM_ENCRYPTED_KEY ),
                        NULL,
                        0,
                        oEncKey.pbEncryptedKey,
                        oEncKey.cbEncryptedKey,
                        &hPrivKey );

                if(    rgKFKeyType[ iKeyFileType ] == eKF_KEY_TYPE_ECC_256
                    && rgpPubKey[iPubKey]          == &oPubKeyEncrypt
                    && iKeyType                    == OEM_HAL_KEY_TYPE_ECC_P256_CONTENT_ENCRYPT )
                {
                    drExpected = DRM_SUCCESS;
                }
                else if(    rgKFKeyType[ iKeyFileType ] == eKF_KEY_TYPE_ECC_256
                         && rgpPubKey[iPubKey]          == &oPubKeySign
                         && iKeyType                    == OEM_HAL_KEY_TYPE_ECC_P256_SIGN )
                {
                    drExpected = DRM_SUCCESS;
                }
                else if(    rgKFKeyType[ iKeyFileType ] == eKF_KEY_TYPE_ECC_256
                         && rgpPubKey[iPubKey]          == &oPubKeyPrndEncrypt
                         && iKeyType                    == OEM_HAL_KEY_TYPE_ECC_P256_MESSAGE_ENCRYPT )
                {
                    drExpected = DRM_SUCCESS;
                }
                else if(    rgKFKeyType[ iKeyFileType ] == eKF_KEY_SECURE_STORE
                         && iKeyType                    == OEM_HAL_KEY_TYPE_AES_OMAC1_SECURE_STORE )
                {
                    drExpected = DRM_SUCCESS;
                }
                else
                {
                    switch( iKeyType )
                    {
                    case OEM_HAL_KEY_TYPE_ECC_P256_DOMAIN_ENCRYPT:      __fallthrough;
                    case OEM_HAL_KEY_TYPE_AES_ECB_ROOT:                 __fallthrough;
                    case OEM_HAL_KEY_TYPE_AES_CTR_CONTENT:              __fallthrough;
                    case OEM_HAL_KEY_TYPE_AES_ECB_MESSAGE_ENCRYPT:      __fallthrough;
                    case OEM_HAL_KEY_TYPE_AES_ECB_DOMAIN_PROTECT:       __fallthrough;
#if !DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY
                    /* These key types aren't supported in general when SecureHalPlayOnly is enabled. */
                    case OEM_HAL_KEY_TYPE_AES_ECB_NEW_LICENSE_ROOT:     __fallthrough;
                    case OEM_HAL_KEY_TYPE_AES_CTR_NEW_LICENSE_CONTENT:  __fallthrough;
#endif  /* !DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY */
#if !DRM_SUPPORT_SECUREOEMHAL
                    /* These key types aren't supported in general when SecureHal is enabled. */
                    case OEM_HAL_KEY_TYPE_LEGACY:                       __fallthrough;
                    case OEM_HAL_KEY_TYPE_AES_SLK:                      __fallthrough;
#endif  /* !DRM_SUPPORT_SECUREOEMHAL */
                    case OEM_HAL_KEY_TYPE_ECC_P256_PUBLIC:
                        drExpected = DRM_E_OEMHAL_UNSUPPORTED_KEY_WRAPPING_FORMAT;
                        break;
                    case OEM_HAL_KEY_TYPE_AES_OMAC1_SECURE_STORE:
                        if( rgKFKeyType[ iKeyFileType ] == eKF_KEY_TYPE_ECC_256 )
                        {
                            drExpected = DRM_E_OEMHAL_UNSUPPORTED_KEY_LENGTH;
                        }
                        break;
                    case OEM_HAL_KEY_TYPE_ECC_P256_CONTENT_ENCRYPT:
                        if( rgKFKeyType[ iKeyFileType ] == eKF_KEY_TYPE_ECC_256 )
                        {
                            drExpected = DRM_E_OEMHAL_UNSUPPORTED_KEY_TYPE;
                        }
                        else
                        {
                            drExpected = DRM_E_OEMHAL_UNSUPPORTED_KEY_LENGTH;
                        }
                        break;
                    case OEM_HAL_KEY_TYPE_ECC_P256_SIGN:
                        if( rgKFKeyType[ iKeyFileType ] == eKF_KEY_TYPE_ECC_256 )
                        {
                            drExpected = DRM_E_OEMHAL_UNSUPPORTED_KEY_TYPE;
                        }
                        else
                        {
                            drExpected = DRM_E_OEMHAL_UNSUPPORTED_KEY_LENGTH;
                        }
                        break;
                    case OEM_HAL_KEY_TYPE_ECC_P256_MESSAGE_ENCRYPT:
                        if( rgKFKeyType[ iKeyFileType ] == eKF_KEY_TYPE_ECC_256 )
                        {
                            drExpected = DRM_E_OEMHAL_UNSUPPORTED_KEY_TYPE;
                        }
                        else
                        {
                            drExpected = DRM_E_OEMHAL_UNSUPPORTED_KEY_LENGTH;
                        }
                        break;
#if DRM_SUPPORT_SECUREOEMHAL
                    /* These key types aren't supported in general when SecureHal is enabled. */
                    case OEM_HAL_KEY_TYPE_AES_SLK:
                        drExpected = DRM_E_NOTIMPL;
                        break;
                    case OEM_HAL_KEY_TYPE_LEGACY:                       __fallthrough;
#endif  /* DRM_SUPPORT_SECUREOEMHAL */
#if DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY
                    /* These key types aren't supported in general when SecureHalPlayOnly is enabled. */
                    case OEM_HAL_KEY_TYPE_AES_ECB_NEW_LICENSE_ROOT:     __fallthrough;
                    case OEM_HAL_KEY_TYPE_AES_CTR_NEW_LICENSE_CONTENT:  __fallthrough;
#endif  /* DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY */
                    default:
                        drExpected = DRM_E_OEMHAL_UNSUPPORTED_KEY_TYPE;
                        break;
                    }
                }

                if( dr != drExpected )
                {
                    Log( "", "Test_InvalidKeyFileUsage failure: dr=0x%x(%s)  drExpected=0x%x(%s)  KeyFileType=%d  KeyType=%d", dr, DRM_ERR_GetErrorNameFromCode( dr, NULL ), drExpected, DRM_ERR_GetErrorNameFromCode( drExpected, NULL ), iKeyFileType, iKeyType );
                    ChkBOOL( dr == drExpected, DRM_E_TEST_UNEXPECTED_OUTPUT );
                }

                dr = DRM_SUCCESS;
                DRM_BBX_SafeReleaseKey( &g_BBContext, &hPrivKey );
            }

            DRM_BBX_SafeReleaseKey( &g_BBContext, &hKeyFileKey );
        }
    }

ErrorExit:
    SAFE_OEM_FREE( pbDeviceCert );

    DRM_BBX_CleanupPublicKeyContext( &g_BBContext, &oPubKeySign );
    DRM_BBX_CleanupPublicKeyContext( &g_BBContext, &oPubKeyEncrypt );
    DRM_BBX_CleanupPublicKeyContext( &g_BBContext, &oPubKeyPrndEncrypt );

    DRM_BBX_SafeReleaseKey( &g_BBContext, &hPrivKey );
    DRM_BBX_SafeReleaseKey( &g_BBContext, &hKeyFileKey );

    return dr;
}

BEGIN_TEST_API_LIST( BlackBox )
    API_ENTRY(TestBBXSymmetricCipher)
    API_ENTRY(TestBBXLegacySymmetricCipher)
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
    API_ENTRY(Test_PRNDFunctions)
    API_ENTRY(Test_PRNDRebindLicense)
    API_ENTRY(TestKeyUnwrapAndWrap)
    API_ENTRY(TestManagerStoreRevocationPackage)
    API_ENTRY(Test_BBXCryptoHandleCaching)
    API_ENTRY(Test_CreateAndVerifyOMAC1Signature)
    API_ENTRY(Test_CreateAndVerifyMessageSignature)
    API_ENTRY(Test_DecryptContent)
    API_ENTRY(Test_HalRevocation)
    API_ENTRY(Test_InvalidPubKeyWrapUsage)
    API_ENTRY(Test_InvalidPubKeySignUsage)
    API_ENTRY(Test_InvalidKeyFileUsage)
END_TEST_API_LIST

EXIT_PKTEST_NAMESPACE_CODE;

