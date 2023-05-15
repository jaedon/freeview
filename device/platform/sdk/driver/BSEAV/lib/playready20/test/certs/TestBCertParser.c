/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdafx.h>
#include <drmbcert.h>
#include <drmbcertbuilder.h>
#include <drmbcertparser.h>
#include <TestBCert.h>

ENTER_PKTEST_NAMESPACE_CODE

static DRM_CRYPTO_CONTEXT     g_CryptoCtx         = {0};
/*****************************************************************************
** Function:    _tGetSetBinCertList
**
** Synopsis:    Either writes over or outputs the static cert chain used in
**              the cert building test cases.
**
** Arguments:   [pBinCerts] : Either the cert chain output buffer (fGet ==TRUE)
**                                 or the input cert chain which should replace the
**                                 current chain (fGet == FALSE)
**              [cBinCerts] : The size of the cert chain (in certs) on input and output.
**              [fGet]           : Flag indicating if current cert chain should be
**                                 output or overwritten
**
** Returns:     DRM_SUCCESS          - on success
**              DRM_E_BUFFERTOOSMALL - if the output buffer is too small.
**                                     cbBinCertChain will hold the required buffer
**                                     size if this is returned
**
** Notes:       NOT THREAD SAFE!
**              Input chains of too long a size currently get truncated without
**              any error messages
**
******************************************************************************/
DRM_RESULT DRM_CALL _tGetSetBinCertList( DRM_BCERT_CERTIFICATE *pBinCerts, DRM_DWORD *cBinCerts, DRM_BOOL fGet)
{
    DRM_RESULT       dr                          = DRM_E_TEST_INCOMPLETE;
    static DRM_BCERT_CERTIFICATE  pStaticBinCerts[10] = {0}; /* Leave room for more than 6 certs of max size to be in the chain */
    static DRM_DWORD cStaticBinCerts        = 0;

    ChkArg( pBinCerts  != NULL );

    if ( fGet )
    {
        /* Make sure that output buffers can hold current chain */
        if ( *cBinCerts < cStaticBinCerts )
        {
            /* if output buffer is too small, output the required size and return the proper error code */
            *cBinCerts = cStaticBinCerts;
            ChkDR( DRM_E_BUFFERTOOSMALL );
        }

        /* Copy the current chain to the output buffer */
        MEMCPY( pBinCerts, pStaticBinCerts, cStaticBinCerts * SIZEOF( DRM_BCERT_CERTIFICATE ) );
        *cBinCerts = cStaticBinCerts;
    }
    else
    {
        /* Copy the input chain upto 10 certs */
        MEMCPY( pStaticBinCerts,  pBinCerts,  min( *cBinCerts * SIZEOF( DRM_BCERT_CERTIFICATE ), 10 * SIZEOF( DRM_BCERT_CERTIFICATE ) ) );
        cStaticBinCerts = min( *cBinCerts, 10 );
    }

    dr = DRM_SUCCESS;

ErrorExit:
    return dr;
}

/*****************************************************************************
** Function:    Test_ParseChainHeader
**
** Synopsis:    Parses a binary certificate chain header from a specified file.
**
** Arguments:   argv[0]   : The name of the test file to load
**              argv[1]   : The expected version number
**              argv[2]   : The expected size (in bytes) of the cert chain
**              argv[3]   : The expected value of the chain header flags
**              argv[4]   : The expected number of certs in the chain
**              argv[5]   : The expected offset value
**
** Returns:     DRM_SUCCESS                                 - on success
**              DRM_E_INVALIDARG                            - if any parameter is invalid
**              or other return code from a function called within
**
** Notes:       Test files are required to be in a directory called "files" in
**              the same folder in which the executable is being run.
**
******************************************************************************/
DRM_RESULT DRM_CALL Test_ParseChainHeader(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT dr                  = DRM_E_TEST_INCOMPLETE;
    DRM_BYTE   *pbTestBinCertChain = NULL;
    DRM_DWORD  cbTestBinCertChain  = 0;
    DRM_DWORD  dwVersion           = 0;
    DRM_DWORD  cbExpectedChainSize = 0;
    DRM_DWORD  dwFlags             = 0;
    DRM_DWORD  cCerts              = 0;
    DRM_DWORD  dwExpectedOffset    = 0;
    DRM_DWORD  dwOffset            = 0;
    DRM_BCERT_CHAIN_HEADER ChainHeader   = {0};

    ChkArg( argc >= 6 );
    ChkArg( argv[1] != NULL );
    ChkArg( argv[2] != NULL );
    ChkArg( argv[3] != NULL );
    ChkArg( argv[4] != NULL );
    ChkArg( argv[5] != NULL );

    if ( argv[0] != NULL )
    {
        /* Load cert chain from a file if specified */
        ChkBOOL( LoadTestFile( "files", argv[0], &pbTestBinCertChain, &cbTestBinCertChain ), DRM_E_TEST_INVALIDARG );
    }
    else
    {
        /* Load cert chain from the static buffer if no file is specifed */
        ChkBOOL( _tGetSetBCChainBuffer( pbTestBinCertChain, &cbTestBinCertChain, TRUE) == DRM_E_BUFFERTOOSMALL, DRM_E_TEST_UNEXPECTED_OUTPUT );
        ChkMem( pbTestBinCertChain = (DRM_BYTE*)Oem_MemAlloc( cbTestBinCertChain * SIZEOF(DRM_BYTE) ) );
        ZEROMEM( pbTestBinCertChain, cbTestBinCertChain * SIZEOF(DRM_BYTE) );
        ChkDR( _tGetSetBCChainBuffer( pbTestBinCertChain, &cbTestBinCertChain, TRUE) );
    }

    ChkDR( DRMCRT_AtoDWORD( argv[1], (DRM_DWORD)strlen( argv[1] ), 10, &dwVersion ) );
    ChkDR( DRMCRT_AtoDWORD( argv[2], (DRM_DWORD)strlen( argv[2] ), 10, &cbExpectedChainSize ) );
    ChkDR( DRMCRT_AtoDWORD( argv[3], (DRM_DWORD)strlen( argv[3] ), 10, &dwFlags ) );
    ChkDR( DRMCRT_AtoDWORD( argv[4], (DRM_DWORD)strlen( argv[4] ), 10, &cCerts ) );
    ChkDR( DRMCRT_AtoDWORD( argv[5], (DRM_DWORD)strlen( argv[5] ), 10, &dwExpectedOffset ) );

    /* Parse the chain header */
    ChkDR( DRM_BCert_GetChainHeader( pbTestBinCertChain, cbTestBinCertChain, &dwOffset, &ChainHeader ) );

    /* Check parsed values against expected values */
    ChkBOOL( DRM_BCERT_CHAIN_HEADER_TAG == ChainHeader.dwHeaderTag, DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkBOOL( dwVersion                  == ChainHeader.dwVersion,   DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkBOOL( cbExpectedChainSize        == ChainHeader.cbChain,     DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkBOOL( dwFlags                    == ChainHeader.dwFlags,     DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkBOOL( cCerts                     == ChainHeader.cCerts,      DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkBOOL( dwExpectedOffset           == dwOffset,                DRM_E_TEST_UNEXPECTED_OUTPUT );

    dr = DRM_SUCCESS;

ErrorExit:
    SAFE_OEM_FREE( pbTestBinCertChain );
    return dr;
}



/*****************************************************************************
** Function:    Test_FindObjectInCertByType
**
** Synopsis:    Finds obects in a binary cert loaded from a specified file
**              using API that iterates thru objects in a cert.
**
** Arguments:   argv[0]   : The filename to parse the cert from
**              argv[1]   : Flag indicating if there is a cert chain header in the test file
**              argv[2]   : The number of bytes to skip from the start of the file
**                          or (if argv[1] is TRUE), the number of bytes to skip starting from
**                          after the cert header
**              argv[3]   : Type of object to look for.
**              argv[4]   : Cert size to use
**              argv[5]   : Expected output byte offset
**
** Returns:     DRM_SUCCESS                                 - on success
**              DRM_E_INVALIDARG                            - if any parameter is invalid
**              or other return code from a function called within
**
** Notes:
**
******************************************************************************/
DRM_RESULT DRM_CALL Test_FindObjectInCertByType(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT  dr                   = DRM_E_TEST_INCOMPLETE;
    DRM_BYTE    *pbTestBinCertChain  = NULL;
    DRM_DWORD   cbTestBinCertChain   = 0;
    DRM_BOOL    fSkipHeader          = TRUE;
    DRM_DWORD   cbBytesToSkip        = 0;
    DRM_DWORD   bCurrOffset          = 0;
    DRM_DWORD   dwExpectedObjType    = 0;
    DRM_DWORD   dwExpectedOffset     = 0;

    ChkArg( argc >= 4 );
    ChkArg( argv[1] != NULL );
    ChkArg( argv[2] != NULL );
    ChkArg( argv[3] != NULL );

    if ( argv[0] != NULL )
    {
        /* Load cert chain from a file if specified */
        ChkBOOL( LoadTestFile( "files", argv[0], &pbTestBinCertChain, &cbTestBinCertChain ), DRM_E_TEST_INVALIDARG );
    }
    else
    {
        /* Load cert chain from the static buffer if no file is specifed */
        ChkBOOL( _tGetSetBCChainBuffer( pbTestBinCertChain, &cbTestBinCertChain, TRUE) == DRM_E_BUFFERTOOSMALL, DRM_E_TEST_UNEXPECTED_OUTPUT );
        ChkMem( pbTestBinCertChain = (DRM_BYTE*)Oem_MemAlloc( cbTestBinCertChain * SIZEOF(DRM_BYTE) ) );
        ZEROMEM( pbTestBinCertChain, cbTestBinCertChain * SIZEOF(DRM_BYTE) );
        ChkDR( _tGetSetBCChainBuffer( pbTestBinCertChain, &cbTestBinCertChain, TRUE) );
    }

    ChkDR( StringToBool( argv[1], &fSkipHeader ) );
    ChkDR( DRMCRT_AtoDWORD( argv[2], DRMCRT_strlen( argv[2] ), 10, &cbBytesToSkip ) );
    ChkDR( DRMCRT_AtoDWORD( argv[3], DRMCRT_strlen( argv[3] ), 10, &dwExpectedObjType ) );

    if ( ( argc > 4 ) && ( argv[4]!= NULL ) )
    {
        ChkDR( DRMCRT_AtoDWORD( argv[4], DRMCRT_strlen( argv[4] ), 10, &cbTestBinCertChain ) );
    }

    Log( "Trace", "looking for an object type of: %s", argv[3]  );

    if (fSkipHeader)
    {
        cbBytesToSkip = cbBytesToSkip + SIZEOF( DRM_BCERT_CHAIN_HEADER );
    }
    /*
    ** Find object in a cert by its type:
    */
    bCurrOffset = cbBytesToSkip; /* set offset to the beginning of the first cert */

    /*
    ** Find object
    */
    ChkDR( DRM_BCert_FindObjectInCertByType( pbTestBinCertChain,
                                             cbTestBinCertChain,
                                             &bCurrOffset,
                                             dwExpectedObjType ) );

    if ( ( argc > 5 ) && ( argv[5] != NULL ) )
    {
        ChkDR( DRMCRT_AtoDWORD( argv[5], DRMCRT_strlen( argv[5] ), 10, &dwExpectedOffset ) );
        ChkBOOL( dwExpectedOffset == bCurrOffset, DRM_E_TEST_UNEXPECTED_OUTPUT );
    }

    dr = DRM_SUCCESS;

ErrorExit:
    SAFE_OEM_FREE( pbTestBinCertChain );
    return dr;
}



/*****************************************************************************
** Function:    Test_ParseCertificate
**
** Synopsis:    Parses a binary cert from a specified file.
**
** Arguments:   argv[0]   : The filename to parse the cert from
**              argv[1]   : Flag indicating if there is a cert chain header in the test file
**              argv[2]   : The number of bytes to skip from the start of the file
**                          or (if argv[1] is TRUE), the number of bytes to skip starting from
**                          after the cert header
**              argv[3]   : Type of cert
**              argv[4]   : Certificate ID
**              argv[5]   : Client ID
**              argv[6]   : (optional) expected expiration date.
**
** Returns:     DRM_SUCCESS                                 - on success
**              DRM_E_INVALIDARG                            - if any parameter is invalid
**              or other return code from a function called within
**
** Notes:
**
******************************************************************************/
DRM_RESULT DRM_CALL Test_ParseCertificate(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT  dr                  = DRM_E_TEST_INCOMPLETE;
    DRM_BYTE    *pbTestBinCertChain = NULL;
    DRM_DWORD   cbTestBinCertChain  = 0;
    DRM_BOOL    fSkipHeader         = TRUE;
    DRM_DWORD   cbBytesToSkip       = 0;
    DRM_DWORD   dwExpectedType      = 0;
    DRM_DWORD   dwExpectedExpire    = DRM_BCERT_DEFAULT_EXPIRATION_DATE;
    DRM_BCERT_CLIENT_ID    ExpectedClientID = {0};
    DRM_BCERT_CERT_ID      ExpectedCertID   = {0};
    DRM_BCERT_CERTIFICATE  Cert             = {0};


    ChkArg( argc >= 6 );
    ChkArg( argv[1] != NULL );
    ChkArg( argv[2] != NULL );
    ChkArg( argv[3] != NULL );
    ChkArg( argv[4] != NULL );
    ChkArg( argv[5] != NULL );

    if ( argv[0] != NULL )
    {
        /* Load cert chain from a file if specified */
        ChkBOOL( LoadTestFile( "files", argv[0], &pbTestBinCertChain, &cbTestBinCertChain ), DRM_E_TEST_INVALIDARG );
    }
    else
    {
        /* Load cert chain from the static buffer if no file is specifed */
        ChkBOOL( _tGetSetBCChainBuffer( pbTestBinCertChain, &cbTestBinCertChain, TRUE) == DRM_E_BUFFERTOOSMALL, DRM_E_TEST_UNEXPECTED_OUTPUT );
        ChkMem( pbTestBinCertChain = (DRM_BYTE*)Oem_MemAlloc( cbTestBinCertChain * SIZEOF(DRM_BYTE) ) );
        ZEROMEM( pbTestBinCertChain, cbTestBinCertChain * SIZEOF(DRM_BYTE) );
        ChkDR( _tGetSetBCChainBuffer( pbTestBinCertChain, &cbTestBinCertChain, TRUE) );
    }

    ChkDR( StringToBool( argv[1], &fSkipHeader ) );
    ChkDR( DRMCRT_AtoDWORD( argv[2], (DRM_DWORD)strlen( argv[2] ), 10, &cbBytesToSkip ) );
    ChkDR( DRMCRT_AtoDWORD( argv[3], (DRM_DWORD)strlen( argv[3] ), 10, &dwExpectedType ) );
    MEMCPY( ExpectedCertID.rgb,   argv[4], min( DRM_BCERT_CERT_ID_LENGTH,   strlen( argv[4] ) ) );
    MEMCPY( ExpectedClientID.rgb, argv[5], min( DRM_BCERT_CLIENT_ID_LENGTH, strlen( argv[5] ) ) );
    if ( argc >= 7 && argv[6] != NULL )
    {
        ChkDR( DRMCRT_AtoDWORD( argv[6], (DRM_DWORD)strlen( argv[6] ), 16, &dwExpectedExpire ) );
    }

    if (fSkipHeader)
    {
        cbBytesToSkip = cbBytesToSkip + SIZEOF( DRM_BCERT_CHAIN_HEADER );
    }

    /* Parse the cert */
    ChkDR( DRM_BCert_GetCertificate( pbTestBinCertChain, cbTestBinCertChain, &cbBytesToSkip, &Cert, DRM_BCERT_CERTTYPE_UNKNOWN ) );

    /* Verify basic fields within cert */
    ChkBOOL( Cert.BasicInfo.dwType == dwExpectedType, DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkBOOL( MEMCMP( &Cert.BasicInfo.CertificateID, &ExpectedCertID, SIZEOF(DRM_BCERT_CERT_ID) ) == 0, DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkBOOL( MEMCMP( &Cert.BasicInfo.ClientID, &ExpectedClientID, SIZEOF(DRM_BCERT_CLIENT_ID) ) == 0, DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkBOOL( Cert.BasicInfo.dwType == dwExpectedType, DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkBOOL( Cert.BasicInfo.dwExpirationDate == dwExpectedExpire, DRM_E_TEST_UNEXPECTED_OUTPUT );

    dr = DRM_SUCCESS;

ErrorExit:
    SAFE_OEM_FREE( pbTestBinCertChain );
    return dr;
}

/*****************************************************************************
** Function:    Test_ParseCertificateChain
**
** Synopsis:    Parses binary certificate chain from a specified file.
**
** Arguments:   argv[0]   : The name of the test file to load
**              argv[1]   : File name containing the "root public key" used to
**                          create the chain (for testing, this may not be the
**                          official root public key)
**              argv[2]   : The expected number of certs in the chain
**              argv[3]   : Optional: expected leaf certificate type. PC (1) Device (2)
**                          Domain (3) Issuer (4), CRLSigner (5), Service (6),
**                          Silverlight (7), Application (8), Metering(9),
**                          Keyfilesigner (10), Server (11).
**              argv[4]   : Optional: flag indicating that NULL time should be used
**              argv[5]   : Optional: flag indicating that NULL crypto ctx should be used
**              argv[6]   : Optional: value of fVerifyCertSignatures (TRUE or FALSE)
**              argv[7]   : Optional: value of fDontFailOnMissingExtData (TRUE or FALSE)
**              argv[8]   : Optional: number of rgdwRequiredKeyUsages
**              argv[9]   : Optional: The key usage (read as decimal DWORD, for multiple usages, add values)
**                          1        -> sign
**                          2        -> encrypt
**                          4        -> sign CRL
**                          8        -> issuer all
**                          16       -> issuer indiv
**                          32       -> issuer device
**                          64       -> issuer link
**                          128      -> issuer domain
**                          256      -> issuer silverlight
**                          512      -> issuer application
**                          1024     -> issuer crl
**                          2048     -> issuer metering
**                          4096     -> issuer sign keyfile
**                          8192     -> sign keyfile
**                          16384    -> issuer server
**              argv[10]  : Optional: if present then set fCollectErrorsInArrayAndContinue into TRUE,
**                          this is number of elements in expected errors array
**              argv[11]  : Optional: value of the first error code in errors arrays
**
**
** Returns:     DRM_SUCCESS                                 - on success
**              DRM_E_INVALIDARG                            - if any parameter is invalid
**              or other return code from a function called within
**
** Notes:       Test files are required to be in a directory called "files" in
**              the same folder in which the executable is being run.
**
******************************************************************************/
DRM_RESULT DRM_CALL Test_ParseCertificateChain(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT  dr                          = DRM_E_TEST_INCOMPLETE;
    DRM_BYTE    *pbTestBinCertChain         = NULL;
    DRM_DWORD   cbTestBinCertChain          = 0;
    DRM_BYTE    *pbRootPublicKey            = NULL;
    DRM_DWORD   cbRootPublicKey             = 0;
    DRM_DWORD   cExpectedCerts              = 0;
    DRM_DWORD   dwIndex                     = 0;
    DRM_DWORD   bCurrOffset                 = 0;
    DRM_BCERT_CHAIN_HEADER ChainHeader      = {0};
    DRM_BCERT_CERTIFICATE  *pChainedCerts   = NULL;
    DRM_BOOL    fUseNullTime                = FALSE;
    DRMFILETIME fTime                       = {0};
    DRM_CRYPTO_CONTEXT *pCryptoContext      = NULL;
    DRM_BOOL    fUseNullCryptoCtx           = FALSE;
    DRM_DWORD   dwType                      = DRM_BCERT_CERTTYPE_UNKNOWN;
    DRM_BOOL    fVerifyCertSignatures       = TRUE; /* test default is TRUE */
    DRM_BOOL    fDontFailOnMissingExtData   = FALSE;
    DRM_DWORD   dwNumKeyUsages              = 0;
    DRM_DWORD   *prgdwKeyUsageSet           = NULL;
    DRM_DWORD   dwKeyUsages                 = 0;
    DRM_DWORD   dwCurKeyUsage = 0;
    DRM_DWORD   dwCurKeyUseType = 1;
    DRM_DWORD   dwCurKeyUseFlag = 1; /* should equal 2^(dwCurKeyUseType-1) */
    DRM_DWORD   dwExpectedNumOfCollectedErrs = 0;
    DRM_BCERT_VERIFICATIONCONTEXT oVerificationContext = {0};
    DRM_RESULT  dwExpectedErrorCode = DRM_SUCCESS;

    ChkArg( argc >= 3 );
    ChkArg( argv[2] != NULL );

    ChkMem( pCryptoContext = (DRM_CRYPTO_CONTEXT*)Oem_MemAlloc( SIZEOF( DRM_CRYPTO_CONTEXT ) ) );
    ZEROMEM( pCryptoContext, SIZEOF( DRM_CRYPTO_CONTEXT ) );

    Oem_Clock_GetSystemTimeAsFileTime( NULL, &fTime );

    if ( argv[0] != NULL )
    {
        /*
        ** Load cert chain from a file if specified
        */
        ChkBOOL( LoadTestFile( "files", argv[0], &pbTestBinCertChain, &cbTestBinCertChain ), DRM_E_TEST_INVALIDARG );
    }
    else
    {
        /*
        ** Load cert chain from the static buffer if no file is specifed
        */
        ChkBOOL( _tGetSetBCChainBuffer( pbTestBinCertChain, &cbTestBinCertChain, TRUE) == DRM_E_BUFFERTOOSMALL, DRM_E_TEST_UNEXPECTED_OUTPUT );
        ChkMem( pbTestBinCertChain = (DRM_BYTE*)Oem_MemAlloc( cbTestBinCertChain * SIZEOF(DRM_BYTE) ) );
        ZEROMEM( pbTestBinCertChain, cbTestBinCertChain * SIZEOF(DRM_BYTE) );
        ChkDR( _tGetSetBCChainBuffer( pbTestBinCertChain, &cbTestBinCertChain, TRUE) );
    }
    /*
    ** Load Keys from a file if specified
    */
    if ( argv[1] != NULL )
    {
        ChkBOOL( LoadTestFile( "files", argv[1], &pbRootPublicKey, &cbRootPublicKey ), DRM_E_TEST_INVALIDARG );
        ChkBOOL( cbRootPublicKey == SIZEOF(PUBKEY_P256), DRM_E_TEST_INVALIDARG );
    }
    /*
    ** Check for certificate type
    */
    if ( ( argc > 3 ) && ( argv[3] != NULL ) )
    {
        ChkDR( DRMCRT_AtoDWORD( argv[3], (DRM_DWORD)strlen( argv[3] ), 10, &dwType ) );
    }
    /*
    ** Check for null filetime flag
    */
    if ( ( argc > 4 ) && ( argv[4] != NULL ) )
    {
        ChkDR( StringToBool( argv[4], &fUseNullTime ) );
    }
    /*
    ** Check for null crypto ctx
    */
    if ( argc > 5 && argv[5] != NULL )
    {
        ChkDR( StringToBool( argv[5], &fUseNullCryptoCtx ) );
    }
    /*
    ** Check for fVerifyCertSignatures
    */
    if ( argc> 6 && argv[6] != NULL )
    {
        ChkDR( StringToBool( argv[6], &fVerifyCertSignatures ) );
    }
    /*
    ** Check for fDontFailOnMissingExtData
    */
    if ( argc > 7 && argv[7] != NULL )
    {
        ChkDR( StringToBool( argv[7], &fDontFailOnMissingExtData ) );
    }
    /*
    **  Read number of required key usages and key usage values
    */
    if ( argc > 8 && argv[8] != NULL )
    {
        ChkBOOL( argc > 9 && argv[9] != NULL, DRM_E_TEST_INVALIDARG );

        ChkDR( DRMCRT_AtoDWORD( argv[8], (DRM_DWORD)strlen( argv[8] ), 10, &dwNumKeyUsages ) );
        ChkDR( DRMCRT_AtoDWORD( argv[9], (DRM_DWORD)strlen( argv[9] ), 10, &dwKeyUsages ) );
        ChkMem( prgdwKeyUsageSet = (DRM_DWORD*)Oem_MemAlloc( SIZEOF( DRM_DWORD ) * dwNumKeyUsages ) );
        while ( dwCurKeyUsage < dwNumKeyUsages && dwCurKeyUseType < 31 )
        {
            if ( dwKeyUsages & dwCurKeyUseFlag )
            {
                prgdwKeyUsageSet[dwCurKeyUsage] = dwCurKeyUseType;
                dwCurKeyUsage++;
            }
            dwCurKeyUseType++;
            dwCurKeyUseFlag = dwCurKeyUseFlag * 2;
        }
    }
    /*
    ** Check for number of expected non-fatal parsing errors
    */
    if ( argc > 10 && argv[10] != NULL )
    {
        ChkDR( DRMCRT_AtoDWORD( argv[10], (DRM_DWORD)strlen( argv[10] ), 10, &dwExpectedNumOfCollectedErrs ) );
    }
    if ( argc > 11 && argv[11] != NULL )
    {
        ChkBOOL( dwExpectedNumOfCollectedErrs > 0, DRM_E_TEST_INVALIDARG );
        ChkDR( DRMCRT_AtoDWORD( argv[11], (DRM_DWORD)strlen( argv[11] ), 16, (DRM_DWORD*)&dwExpectedErrorCode ) );
    }

    /*
    ** Verify the cert chain
    */
    ChkDR( DRM_BCert_InitVerificationContext( fUseNullTime? NULL : &fTime,
                                              (const PUBKEY_P256*) pbRootPublicKey,
                                              dwType,
                                              fUseNullCryptoCtx ? NULL : pCryptoContext,
                                              fVerifyCertSignatures,
                                              fDontFailOnMissingExtData,
                                              prgdwKeyUsageSet,
                                              dwNumKeyUsages,
                                              dwExpectedNumOfCollectedErrs > 0 ? TRUE : FALSE,
                                              NULL,
                                              NULL,
                                              NULL,
                                              &oVerificationContext ) );

    dr = DRM_BCert_ParseCertificateChain( pbTestBinCertChain,
                                          cbTestBinCertChain,
                                          &oVerificationContext );
    /* if expecting non-fatal errors then there will be special error code and, of course, errors */
    if ( dwExpectedNumOfCollectedErrs > 0 )
    {
        /* if verification errors are found there should be errors in the list */
        DRM_DWORD dwCount = 0;
        for ( dwCount = 0; dwCount < oVerificationContext.cResults; dwCount++ )
        {
            Log( "Trace",
                 "Verification error found: %#X in cert %d.\n",
                 oVerificationContext.rgoResults[dwCount].dwResult,
                 oVerificationContext.rgoResults[dwCount].cCertNumber );
        }
        if ( dr != DRM_E_BCERT_VERIFICATION_ERRORS )
        {
            Log( "Trace",
                 "Error: %#X instead of DRM_E_BCERT_VERIFICATION_ERRORS",
                 dr );
        }
        ChkBOOL( dr == DRM_E_BCERT_VERIFICATION_ERRORS, DRM_E_TEST_UNEXPECTED_OUTPUT );

        ChkBOOL( oVerificationContext.cResults == dwExpectedNumOfCollectedErrs, DRM_E_TEST_UNEXPECTED_OUTPUT );

        if ( dwExpectedErrorCode != DRM_SUCCESS )
        {
            ChkBOOL( oVerificationContext.rgoResults[0].dwResult == dwExpectedErrorCode, DRM_E_TEST_UNEXPECTED_OUTPUT );
        }
        dr = DRM_SUCCESS;
    }

    ChkDR(dr);

    /*
    ** If non-fatal verification mode is off then parse each cert in the chain and store
    */
    if ( dwExpectedNumOfCollectedErrs == 0 )
    {
        /* Allocate enough memory to hold all expected cert structures */
        ChkDR( DRMCRT_AtoDWORD( argv[2], (DRM_DWORD)strlen( argv[2] ), 10, &cExpectedCerts ) );
        ChkMem( pChainedCerts = (DRM_BCERT_CERTIFICATE*)Oem_MemAlloc(cExpectedCerts * SIZEOF(DRM_BCERT_CERTIFICATE) ) );
        ZEROMEM( pChainedCerts, cExpectedCerts * SIZEOF(DRM_BCERT_CERTIFICATE) );
        /* Skip the chain header to get to the individual certs */
        ChkDR( DRM_BCert_GetChainHeader(
                                        pbTestBinCertChain,
                                        cbTestBinCertChain,
                                        &bCurrOffset,
                                        &ChainHeader ) );

        for ( dwIndex = 0; dwIndex < cExpectedCerts; dwIndex++ )
        {
            /* Load current cert into a cert sturcture */
            ChkDR( DRM_BCert_GetCertificate(
                                    pbTestBinCertChain,
                                    cbTestBinCertChain,
                                    &bCurrOffset,
                                    &pChainedCerts[dwIndex],
                                    DRM_BCERT_CERTTYPE_UNKNOWN ) );
        }

        /* Store the chained certs */
        ChkDR( _tGetSetBinCertList( pChainedCerts, &cExpectedCerts, FALSE ) );
    }


    dr = DRM_SUCCESS;

ErrorExit:
    SAFE_OEM_FREE( pCryptoContext );
    SAFE_OEM_FREE( pbTestBinCertChain );
    SAFE_OEM_FREE( pChainedCerts );
    if ( pbRootPublicKey != NULL )
    {
        SAFE_OEM_FREE( pbRootPublicKey );
    }
    return dr;
}


/*****************************************************************************
** Function:    Test_VerifyCertFromChain
**
** Synopsis:    Compares cert values from a previously parsed in a cert chain to specified values
**              This currently verifies cert type, features supported, key usage, expiration date
**
** Arguments:   argv[0]   : The cert to verify from the chain (valid values are 0-5)
**              argv[1]   : Expected cert type (1 (PC), 2 (device), 3 (domain),
**                          4 (issuer), 5 (signer), 6 (service), 7 (Silverlight), 8 (Application),
**                          9 (Metering), 10 (Keyfilesigner), 11 (Server) )
**              argv[2]   : The expected features (read as decimal DWORD)
**                          1         -> transmitter
**                          2         -> receiver
**                          4         -> shared cert
**                          8         -> secure clock
**                          16        -> antirollaback clock
**                          32        -> metering
**                          64        -> license sync
**                          128       -> symmtric optimization
**                          256       -> domain support
**                          (to indicate multiple features, just add these,.
**                          ie 34 -> metering and receiver )
**              argv[3]   : The expected expiration date (read as hex DWORD, cert default is 0xFFFFFFFF)
**              argv[4]   : (Optional) manufacturer name
**              argv[5]   : (Optional) model name
**              argv[6]   : (Optional) model number
**              argv[7]   : The expected number of keys
**              argv[7+n] : The expected key usages (read as decimal DWORD) for key n
**                          1        -> sign
**                          2        -> encrypt
**                          4        -> sign CRL
**                          8        -> issuer all
**                          16       -> issuer indiv
**                          32       -> issuer device
**                          64       -> issuer link
**                          128      -> issuer domain
**                          256      -> issuer silverlight
**                          512      -> issuer application
**                          1024     -> issuer crl
**                          2048     -> issuer metering
**                          4096     -> issuer sign keyfile
**                          8192     -> sign keyfile
**                          16384    -> issuer server
**
** Returns:     DRM_SUCCESS                                 - on success
**              DRM_E_INVALIDARG                            - if any parameter is invalid
**              or other return code from a function called within
**
** Notes:       Must call Test_ParseCertificateChain before calling this test function.
**
******************************************************************************/
DRM_RESULT DRM_CALL Test_VerifyCertFromChain(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT             dr         = DRM_E_TEST_INCOMPLETE;
    DRM_BCERT_CERTIFICATE  *pCerts     = NULL;
    DRM_DWORD              cCerts     = 0;
    DRM_DWORD              dwCertNum  = 0;
    DRM_DWORD              dwType     = 0;
    DRM_DWORD              dwFeatures = 0;
    DRM_DWORD              dwUsage    = 0;
    DRM_DWORD              dwNumKeys  = 0;
    DRM_DWORD              dwExpire   = 0;
    DRM_DWORD              i          = 0;
    DRM_DWORD              j          = 0;

    ChkArg( argc >= 8 );
    ChkArg( argv[0] != NULL );
    ChkArg( argv[1] != NULL );
    ChkArg( argv[2] != NULL );
    ChkArg( argv[3] != NULL );
    ChkArg( argv[7] != NULL );

    ChkDR( DRMCRT_AtoDWORD( argv[0], (DRM_DWORD)strlen( argv[0] ), 10, &dwCertNum ) );
    ChkDR( DRMCRT_AtoDWORD( argv[1], (DRM_DWORD)strlen( argv[1] ), 10, &dwType ) );
    ChkDR( DRMCRT_AtoDWORD( argv[2], (DRM_DWORD)strlen( argv[2] ), 10, &dwFeatures ) );
    ChkDR( DRMCRT_AtoDWORD( argv[3], (DRM_DWORD)strlen( argv[3] ), 16, &dwExpire ) );
    ChkDR( DRMCRT_AtoDWORD( argv[7], (DRM_DWORD)strlen( argv[7] ), 10, &dwNumKeys ) );

    /* Get the previously parsed cert chain */
    cCerts = 6;
    ChkMem( pCerts = (DRM_BCERT_CERTIFICATE*)Oem_MemAlloc( cCerts * SIZEOF(DRM_BCERT_CERTIFICATE) ) );
    ZEROMEM( pCerts, cCerts * SIZEOF(DRM_BCERT_CERTIFICATE) );
    ChkDR( _tGetSetBinCertList( pCerts, &cCerts, TRUE ) );

    ChkBOOL( dwCertNum < cCerts , DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkBOOL( dwType == pCerts[dwCertNum].BasicInfo.dwType, DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkBOOL( dwExpire == pCerts[dwCertNum].BasicInfo.dwExpirationDate, DRM_E_TEST_UNEXPECTED_OUTPUT );

    for ( i = 0; i < pCerts[dwCertNum].FeatureInfo.dwNumFeatureEntries; i++ )
    {
        switch ( pCerts[dwCertNum].FeatureInfo.rgdwFeatureSet[i] )
        {
        case 0:
            /* 0 is invalid */
            ChkDR(DRM_E_TEST_UNEXPECTED_OUTPUT);
            break;
        case 1:
            /* check that current feature in cert is included in expected features */
            ChkBOOL( (dwFeatures & 1) == 1, DRM_E_TEST_UNEXPECTED_OUTPUT );
            /* remove current feature from expected features to make sure no features are duplicates */
            dwFeatures = dwFeatures ^ 1;
            break;
        case 2:
            /* check that current feature in cert is included in expected features */
            ChkBOOL( (dwFeatures & 2) == 2, DRM_E_TEST_UNEXPECTED_OUTPUT );
            /* remove current feature from expected features to make sure no features are duplicates */
            dwFeatures = dwFeatures ^ 2;
            break;
        case 3:
            /* check that current feature in cert is included in expected features */
            ChkBOOL( (dwFeatures & 4) == 4, DRM_E_TEST_UNEXPECTED_OUTPUT );
            /* remove current feature from expected features to make sure no features are duplicates */
            dwFeatures = dwFeatures ^ 4;
            break;
        case 4:
            /* check that current feature in cert is included in expected features */
            ChkBOOL( (dwFeatures & 8) == 8, DRM_E_TEST_UNEXPECTED_OUTPUT );
            /* remove current feature from expected features to make sure no features are duplicates */
            dwFeatures = dwFeatures ^ 8;
            break;
        case 5:
            /* check that current feature in cert is included in expected features */
            ChkBOOL( (dwFeatures & 16) == 16, DRM_E_TEST_UNEXPECTED_OUTPUT );
            /* remove current feature from expected features to make sure no features are duplicates */
            dwFeatures = dwFeatures ^ 16;
            break;
        case 6:
            /* check that current feature in cert is included in expected features */
            ChkBOOL( (dwFeatures & 32) == 32, DRM_E_TEST_UNEXPECTED_OUTPUT );
            /* remove current feature from expected features to make sure no features are duplicates */
            dwFeatures = dwFeatures ^ 32;
            break;
        case 7:
            /* check that current feature in cert is included in expected features */
            ChkBOOL( (dwFeatures & 64) == 64, DRM_E_TEST_UNEXPECTED_OUTPUT );
            /* remove current feature from expected features to make sure no features are duplicates */
            dwFeatures = dwFeatures ^ 64;
            break;
        case 8:
            /* check that current feature in cert is included in expected features */
            ChkBOOL( (dwFeatures & 128) == 128, DRM_E_TEST_UNEXPECTED_OUTPUT );
            /* remove current feature from expected features to make sure no features are duplicates */
            dwFeatures = dwFeatures ^ 128;
            break;
        case 9:
            /* check that current feature in cert is included in expected features */
            ChkBOOL( (dwFeatures & 256) == 256, DRM_E_TEST_UNEXPECTED_OUTPUT );
            /* remove current feature from expected features to make sure no features are duplicates */
            dwFeatures = dwFeatures ^ 256;
            break;
        default:
            /* higher values are invalid */
            ChkDR(DRM_E_TEST_UNEXPECTED_OUTPUT);
            break;
        }
    }
    /* verify that all expected features were found */
    ChkBOOL( dwFeatures == 0, DRM_E_TEST_UNEXPECTED_OUTPUT );

    /* verify that all expected keys were found */
    ChkBOOL( dwNumKeys == pCerts[dwCertNum].KeyInfo.dwNumKeys, DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkArg( (DRM_DWORD)argc >= 8 + dwNumKeys );
    for ( i = 0; i < dwNumKeys; i++)
    {
        /* Get expected usage for this key */
        ChkDR( DRMCRT_AtoDWORD( argv[8+i], (DRM_DWORD)strlen( argv[8+i] ), 10, &dwUsage ) );

        /* Check usages */
        for ( j = 1; j <= DRM_BCERT_KEYUSAGE_MAX_VALUE; j++ )
        {
            if ( ( ( pCerts[dwCertNum].KeyInfo.rgoKeys[i].dwUsageSet ) & BCERT_KEYUSAGE_BIT( j ) ) > 0 )
            {
                switch ( j )
                {
                    case 0:
                        /* 0 is invalid */
                        ChkDR(DRM_E_TEST_UNEXPECTED_OUTPUT);
                        break;
                    case 1:
                        /* check that current usages in cert is included in expected usages */
                        ChkBOOL( (dwUsage & 1) == 1, DRM_E_TEST_UNEXPECTED_OUTPUT );
                        /* remove current usage from expected usages to make sure no usages are duplicates */
                        dwUsage = dwUsage ^ 1;
                        break;
                    case 2:
                        /* check that current usage in cert is included in expected usages */
                        ChkBOOL( (dwUsage & 2) == 2, DRM_E_TEST_UNEXPECTED_OUTPUT );
                        /* remove current usage from expected usages to make sure no usages are duplicates */
                        dwUsage = dwUsage ^ 2;
                        break;
                    case 3:
                        /* check that current usage in cert is included in expected usages */
                        ChkBOOL( (dwUsage & 4) == 4, DRM_E_TEST_UNEXPECTED_OUTPUT );
                        /* remove current usage from expected usages to make sure no usages are duplicates */
                        dwUsage = dwUsage ^ 4;
                        break;
                    case 4:
                        /* check that current usage in cert is included in expected usages */
                        ChkBOOL( (dwUsage & 8) == 8, DRM_E_TEST_UNEXPECTED_OUTPUT );
                        /* remove current usage from expected usages to make sure no usages are duplicates */
                        dwUsage = dwUsage ^ 8;
                        break;
                    case 5:
                        /* check that current usage in cert is included in expected usages */
                        ChkBOOL( (dwUsage & 16) == 16, DRM_E_TEST_UNEXPECTED_OUTPUT );
                        /* remove current usage from expected usages to make sure no usages are duplicates */
                        dwUsage = dwUsage ^ 16;
                        break;
                    case 6:
                        /* check that current usage in cert is included in expected usages */
                        ChkBOOL( (dwUsage & 32) == 32, DRM_E_TEST_UNEXPECTED_OUTPUT );
                        /* remove current usage from expected usages to make sure no usages are duplicates */
                        dwUsage = dwUsage ^ 32;
                        break;
                    case 7:
                        /* check that current usage in cert is included in expected usages */
                        ChkBOOL( (dwUsage & 64) == 64, DRM_E_TEST_UNEXPECTED_OUTPUT );
                        /* remove current usage from expected usages to make sure no usages are duplicates */
                        dwUsage = dwUsage ^ 64;
                        break;
                    case 8:
                        /* check that current usage in cert is included in expected usages */
                        ChkBOOL( (dwUsage & 128) == 128, DRM_E_TEST_UNEXPECTED_OUTPUT );
                        /* remove current usage from expected usages to make sure no usages are duplicates */
                        dwUsage = dwUsage ^ 128;
                        break;
                    case 9:
                        /* check that current usage in cert is included in expected usages */
                        ChkBOOL( (dwUsage & 256) == 256, DRM_E_TEST_UNEXPECTED_OUTPUT );
                        /* remove current usage from expected usages to make sure no usages are duplicates */
                        dwUsage = dwUsage ^ 256;
                        break;
                    case 12:
                        /* check that current usage in cert is included in expected usages */
                        ChkBOOL( (dwUsage & 2048) == 2048, DRM_E_TEST_UNEXPECTED_OUTPUT );
                        /* remove current usage from expected usages to make sure no usages are duplicates */
                        dwUsage = dwUsage ^ 2048;
                        break;
                    case 13:
                        /* check that current usage in cert is included in expected usages */
                        ChkBOOL( (dwUsage & 4096) == 4096, DRM_E_TEST_UNEXPECTED_OUTPUT );
                        /* remove current usage from expected usages to make sure no usages are duplicates */
                        dwUsage = dwUsage ^ 4096;
                        break;
                    case 14:
                        /* check that current usage in cert is included in expected usages */
                        ChkBOOL( (dwUsage & 8192) == 8192, DRM_E_TEST_UNEXPECTED_OUTPUT );
                        /* remove current usage from expected usages to make sure no usages are duplicates */
                        dwUsage = dwUsage ^ 8192;
                        break;
                    case 15:
                        /* check that current usage in cert is included in expected usages */
                        ChkBOOL( (dwUsage & 16384) == 16384, DRM_E_TEST_UNEXPECTED_OUTPUT );
                        /* remove current usage from expected usages to make sure no usages are duplicates */
                        dwUsage = dwUsage ^ 16384;
                        break;
                    default:
                        /* higher values are invalid */
                        ChkDR(DRM_E_TEST_UNEXPECTED_OUTPUT);
                        break;
                }
            }
        }

        /* verify that all expected key usages were found */
        ChkBOOL( dwUsage == 0, DRM_E_TEST_UNEXPECTED_OUTPUT );
    }


    /* Check manufacturer strings */
    if ( argv[4] != NULL )
    {
        ChkBOOL( pCerts[dwCertNum].ManufacturerInfo.ManufacturerStrings.ManufacturerName.cb == DRMCRT_strlen( argv[4] ) + 1,
                 DRM_E_TEST_UNEXPECTED_OUTPUT );
        ChkBOOL( MEMCMP( (DRM_BYTE*)pCerts[dwCertNum].ManufacturerInfo.ManufacturerStrings.ManufacturerName.rgb,
                          argv[4],
                          pCerts[dwCertNum].ManufacturerInfo.ManufacturerStrings.ManufacturerName.cb-1 ) == 0,
                 DRM_E_TEST_UNEXPECTED_OUTPUT );
    }
    else
    {
       ChkBOOL( pCerts[dwCertNum].ManufacturerInfo.ManufacturerStrings.ManufacturerName.cb == 0, DRM_E_TEST_UNEXPECTED_OUTPUT );
    }
    if ( argv[5] != NULL )
    {
        ChkBOOL( pCerts[dwCertNum].ManufacturerInfo.ManufacturerStrings.ModelName.cb == DRMCRT_strlen( argv[5] ) + 1, DRM_E_TEST_UNEXPECTED_OUTPUT );
        ChkBOOL( MEMCMP( (DRM_BYTE*)pCerts[dwCertNum].ManufacturerInfo.ManufacturerStrings.ModelName.rgb,
                          argv[5],
                          pCerts[dwCertNum].ManufacturerInfo.ManufacturerStrings.ModelName.cb-1 ) == 0,
                 DRM_E_TEST_UNEXPECTED_OUTPUT );
    }
    else
    {
        ChkBOOL( pCerts[dwCertNum].ManufacturerInfo.ManufacturerStrings.ModelName.cb == 0, DRM_E_TEST_UNEXPECTED_OUTPUT );
    }
    if ( argv[6] != NULL )
    {
        ChkBOOL( pCerts[dwCertNum].ManufacturerInfo.ManufacturerStrings.ModelNumber.cb == DRMCRT_strlen( argv[6] ) + 1, DRM_E_TEST_UNEXPECTED_OUTPUT );
        ChkBOOL( MEMCMP( (DRM_BYTE*)pCerts[dwCertNum].ManufacturerInfo.ManufacturerStrings.ModelNumber.rgb,
                          argv[6],
                          pCerts[dwCertNum].ManufacturerInfo.ManufacturerStrings.ModelNumber.cb-1 ) == 0,
                 DRM_E_TEST_UNEXPECTED_OUTPUT );
    }
    else
    {
        ChkBOOL( pCerts[dwCertNum].ManufacturerInfo.ManufacturerStrings.ModelNumber.cb == 0, DRM_E_TEST_UNEXPECTED_OUTPUT );
    }

    dr = DRM_SUCCESS;

ErrorExit:
    SAFE_OEM_FREE(pCerts);
    return dr;
}

DRM_RESULT DRM_CALL Test_VerifySecurityVersionForCert( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT dr          = DRM_SUCCESS;
    DRM_DWORD  cCerts      = 6;
    DRM_DWORD  dwCertIndex = 0;
    DRM_DWORD  dwSecVer    = 0;
    DRM_DWORD  dwPlatID    = 0;
    DRM_BCERT_CERTIFICATE  *pCerts = NULL;
    DRM_BOOL   fContainsSecurityVersion = TRUE;
    DRM_BOOL   fContainsSecurityVersion2 = TRUE;

    ChkArg( argc >= 5 );
    ChkArg( argv[0] != NULL );
    ChkArg( argv[1] != NULL );
    ChkArg( argv[2] != NULL );
    ChkArg( argv[3] != NULL );
    ChkArg( argv[4] != NULL ); 


    ChkDR( DRMCRT_AtoDWORD( argv[0], DRMCRT_strlen( argv[0] ), 0, &dwCertIndex ) );
    ChkDR( DRMCRT_AtoDWORD( argv[1], DRMCRT_strlen( argv[1] ), 0, &dwSecVer ) );
    ChkDR( DRMCRT_AtoDWORD( argv[2], DRMCRT_strlen( argv[2] ), 0, &dwPlatID ) );
    ChkDR( StringToBool( argv[3], &fContainsSecurityVersion ) );
    ChkDR( StringToBool( argv[4], &fContainsSecurityVersion2 ) );

    ChkArg( dwCertIndex < cCerts );

    /*
    ** Get the parsed cert chain
    */
    ChkMem( pCerts = (DRM_BCERT_CERTIFICATE*)Oem_MemAlloc( cCerts * SIZEOF(DRM_BCERT_CERTIFICATE) ) );
    ZEROMEM( pCerts, cCerts * SIZEOF(DRM_BCERT_CERTIFICATE) );
    ChkDR( _tGetSetBinCertList( pCerts, &cCerts, TRUE ) );

    if (fContainsSecurityVersion)
    {
        ChkBOOL( pCerts[dwCertIndex].SecurityVersion.dwSecurityVersion    == dwSecVer, DRM_E_TEST_UNEXPECTED_OUTPUT );
        ChkBOOL( pCerts[dwCertIndex].SecurityVersion.dwPlatformIdentifier == dwPlatID, DRM_E_TEST_UNEXPECTED_OUTPUT );
    }
    else
    {
       ChkBOOL(pCerts[dwCertIndex].SecurityVersion.Header.wType == DRM_BCERT_OBJFLAG_EMPTY, DRM_E_TEST_UNEXPECTED_OUTPUT);
    }
   
    /*
    ** If this is a new PC, Device, or Issuer cert, then SecurityVersion2 must be present
    */
    if (fContainsSecurityVersion2)
    {
        ChkBOOL( pCerts[dwCertIndex].SecurityVersion2.dwSecurityVersion    == dwSecVer, DRM_E_TEST_UNEXPECTED_OUTPUT );
        ChkBOOL( pCerts[dwCertIndex].SecurityVersion2.dwPlatformIdentifier == dwPlatID, DRM_E_TEST_UNEXPECTED_OUTPUT );
    }
    else
    {
       ChkBOOL(pCerts[dwCertIndex].SecurityVersion2.Header.wType == DRM_BCERT_OBJFLAG_EMPTY, DRM_E_TEST_UNEXPECTED_OUTPUT);
    }

ErrorExit:
    SAFE_OEM_FREE( pCerts );
    return dr;
}

EXIT_PKTEST_NAMESPACE_CODE
