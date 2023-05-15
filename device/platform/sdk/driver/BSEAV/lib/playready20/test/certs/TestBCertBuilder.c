/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/


#include <string.h>
#include <stdafx.h>
#include <drmbcert.h>
#include <drmbcertbuilder.h>
#include <drmbcertparser.h>
#include <TestBCert.h>

ENTER_PKTEST_NAMESPACE_CODE

static DRM_CRYPTO_CONTEXT     g_CryptoCtx         = {0};
/*****************************************************************************
** Function:    _tGetSetCurCertBuildData
**
** Synopsis:    Either writes over or outputs the static cert builder data
**
** Arguments:   [pData]   : Either the cert builder data output buffer (fGet ==TRUE)
**                          or the input cert builder data which should replace the
**                          current data (fGet == FALSE)
**              [fGet]    : Flag indicating if current cert builder data should
**                          be output or overwritten
**
** Returns:     DRM_SUCCESS                                 - on success
**              DRM_E_INVALIDARG                            - if any parameter is invalid
**
** Notes:       NOT THREAD SAFE!
**
******************************************************************************/
DRM_RESULT DRM_CALL _tGetSetCurCertBuildData( DRM_BCERT_BUILDER_DATA *pData, DRM_BOOL fGet )
{
    DRM_RESULT       dr                          = DRM_E_TEST_INCOMPLETE;
    static DRM_BCERT_BUILDER_DATA  StaticCertData = {0};

    ChkArg( pData != NULL );

    if ( fGet )
    {
        Log("Trace", "Getting current bin cert builder data");
        /* Copy the current cert to the output buffer */
        MEMCPY( (DRM_BYTE*)pData, (DRM_BYTE*)&StaticCertData, SIZEOF(DRM_BCERT_BUILDER_DATA) );
    }
    else
    {
        Log("Trace", "Resetting current bin cert builder data");
        /* Copy the input cert */
        MEMCPY( (DRM_BYTE*)&StaticCertData, (DRM_BYTE*)pData, SIZEOF(DRM_BCERT_BUILDER_DATA) );
    }

    dr = DRM_SUCCESS;

ErrorExit:
    return dr;
}

/*****************************************************************************
** Function:    Test_SetDefaultBuilderData
**
** Synopsis:    Test wrapper for the function BCert_SetDefaultBuilderData.
**              This will overwrite any previously stored builder data
**
** Arguments:   argv[0]   : The Certificate ID to use
**              argv[1]   : The type of certificate.  PC (1) Device (2)
**                          Domain (3) Issuer (4), CRLSigner (5), Service (6),
**                          Silverlight (7), Application (8), Metering(9),
**                          KeyFile Signer (10), Server (11)
**              argv[2]   : File name which holds the public key to use
**              argv[3]   : The number of key usages
**              argv[4]   : The key usage (read as decimal DWORD, for multiple usages, add values)
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
**                          4096     -> issuer keyfile signer
**                          8192     -> sign keyfile
**                          16384    -> issuer server
**              argv[5]   : Name of file which holds the private key
**              argv[6]   : Name of the file which holds the Issuer public key
**              argv[7]   : Optional, Security Version DWORD
**              argv[8]   : Optional, Platform Identifier. Should be given if argv[7] is given
**
** Returns:     DRM_SUCCESS                                 - on success
**              DRM_E_INVALIDARG                            - if any parameter is invalid
**              or other return code from a function called within
**
** Notes:       This will overwrite any previously stored builder data
**              Test files are required to be in a directory called "files" in
**              the same folder in which the executable is being run.
**
******************************************************************************/
DRM_RESULT DRM_CALL Test_SetDefaultBuilderData(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT              dr                   = DRM_E_TEST_INCOMPLETE;
    DRM_BCERT_CERT_ID       *pCertificateID      = NULL;
    DRM_DWORD               dwType               = 0;
    DRM_BCERT_PUBKEY        keyIssuerKey         = {0};
    DRM_BCERT_BUILDER_DATA  Data                 = {0};
    DRM_BYTE   *pbTestPubKey   = NULL;
    DRM_BYTE   *pbTestPrivKey  = NULL;
    DRM_BYTE   *pbTestIssuerKey = NULL;
    DRM_DWORD  cbTestPubKey   = 0;
    DRM_DWORD  cbTestPrivKey  = 0;
    DRM_DWORD  cbTestIssuerKey  = 0;
    DRM_DWORD  fKeyUsages = 0;
    DRM_DWORD  dwCurKeyUsage = 0;
    DRM_DWORD  dwCurKeyUseType = 1;
    DRM_DWORD  dwCurKeyUseFlag = 1; /* should equal 2^(dwCurKeyUseType-1) */

    ChkArg( argc == 7 || argc >= 9 ); /* argv[8] is required if argv[7] is present */

    /* Free any previously allocated memory from current cert data */
    Log("Trace", "Free any previously allocated memory from current cert data");
    ChkDR( _tGetSetCurCertBuildData( &Data, TRUE ) );
    SAFE_OEM_FREE( Data.pCertificateID );
    SAFE_OEM_FREE( Data.prgdwFeatureSet );
    SAFE_OEM_FREE( Data.pCertificateKeys );
    SAFE_OEM_FREE( Data.pManufacturerStrings );
    SAFE_OEM_FREE( Data.pPrivateKey );

    SAFE_OEM_FREE( Data.ExtendedData.pExtDataRecords );

    if( Data.dwType == 3 )
    {
        /* Domain type */
        SAFE_OEM_FREE( Data.Type.DomainData.pServiceID );
        SAFE_OEM_FREE( Data.Type.DomainData.pAccountID );
        SAFE_OEM_FREE( Data.Type.DomainData.pDomainURL );
    }

    /* Get Certificate ID from arguments */
    Log("Trace", "Reading arguments");
    if ( argv[0] != NULL )
    {
        ChkMem( pCertificateID = (DRM_BCERT_CERT_ID*)Oem_MemAlloc( SIZEOF( DRM_BCERT_CERT_ID ) ) );
            DRM_BYT_SetBytes( pCertificateID, 0, SIZEOF(DRM_BCERT_CERT_ID), 0 );
            MEMCPY( pCertificateID->rgb, argv[0], min( DRM_BCERT_CERT_ID_LENGTH, strlen( argv[0] ) ) );
    }
    /* Get Certificate Type from arguments */
    ChkDR( DRMCRT_AtoDWORD( argv[1], (DRM_DWORD)strlen( argv[1] ), 10, &dwType ) );

    /* Load  public key from file */
    ChkBOOL( LoadTestFile( "files", argv[2], &pbTestPubKey, &cbTestPubKey ), DRM_E_TEST_INVALIDARG );
    ChkArg( cbTestPubKey <= DRM_BCERT_MAX_PUBKEY_VALUE_LENGTH );

    /* Make space for up to ( DRM_BCERT_MAX_KEY_USAGES + 5 ) keys in the builder context */
    ChkOverflow( SIZEOF(DRM_BCERT_BUILDER_CERTKEY) * ( DRM_BCERT_MAX_KEY_USAGES + 5 ), SIZEOF(DRM_BCERT_BUILDER_CERTKEY) );
    ChkMem( Data.pCertificateKeys = (DRM_BCERT_BUILDER_CERTKEY*) Oem_MemAlloc( SIZEOF(DRM_BCERT_BUILDER_CERTKEY)
                                                                           * ( DRM_BCERT_MAX_KEY_USAGES + 5 ) ) );
    ZEROMEM( Data.pCertificateKeys, SIZEOF(DRM_BCERT_BUILDER_CERTKEY) * ( DRM_BCERT_MAX_KEY_USAGES + 5 ) );

    MEMCPY( Data.pCertificateKeys[0].KeyValue.m_rgbPubkey, pbTestPubKey, cbTestPubKey );

    /* Load private key from file */
    ChkBOOL( LoadTestFile( "files", argv[5], &pbTestPrivKey, &cbTestPrivKey ), DRM_E_TEST_INVALIDARG );
    ChkArg( cbTestPrivKey == SIZEOF( PRIVKEY_P256 ) );

    /* Load Issuer key from a file */
    ChkBOOL( LoadTestFile( "files", argv[6], &pbTestIssuerKey, &cbTestIssuerKey ), DRM_E_TEST_INVALIDARG );
    ChkArg( cbTestIssuerKey <= DRM_BCERT_MAX_PUBKEY_VALUE_LENGTH );
    MEMCPY( &keyIssuerKey, pbTestIssuerKey, cbTestIssuerKey );

    Log("Trace", "Call BCert_SetDefaultBuilderData");
    BCert_SetDefaultBuilderData( pCertificateID,
                                 dwType,
                                 (PRIVKEY_P256*)pbTestPrivKey,
                                 (PUBKEY_P256*)&keyIssuerKey,
                                 &Data );

    /* Get Number of key usages, and key usage values from arguments */
    ChkDR( DRMCRT_AtoDWORD( argv[3], (DRM_DWORD)strlen( argv[3] ), 10, &Data.pCertificateKeys[0].dwNumKeyUsages ) );

    ChkDR( DRMCRT_AtoDWORD( argv[4], (DRM_DWORD)strlen( argv[4] ), 10, &fKeyUsages ) );

    if ( Data.pCertificateKeys[0].dwNumKeyUsages > 0 )
    {
        ChkOverflow( SIZEOF( DRM_DWORD ) * Data.pCertificateKeys[0].dwNumKeyUsages, Data.pCertificateKeys[0].dwNumKeyUsages );
        ChkMem( Data.pCertificateKeys[0].pKeyUsageSet = (DRM_DWORD*)Oem_MemAlloc( SIZEOF( DRM_DWORD ) * Data.pCertificateKeys[0].dwNumKeyUsages ) );
        ZEROMEM( Data.pCertificateKeys[0].pKeyUsageSet, SIZEOF( DRM_DWORD ) * Data.pCertificateKeys[0].dwNumKeyUsages );
        while ( dwCurKeyUsage < Data.pCertificateKeys[0].dwNumKeyUsages && dwCurKeyUseType < 31 )
        {
            if ( fKeyUsages & dwCurKeyUseFlag )
            {
                Data.pCertificateKeys[0].pKeyUsageSet[dwCurKeyUsage] = dwCurKeyUseType;
                dwCurKeyUsage++;
            }
            dwCurKeyUseType++;
            dwCurKeyUseFlag = dwCurKeyUseFlag * 2;
        }
    }
    else
    {
        /*
        ** Special testcase: number key usages is set into zero while pointer to key usages array is not NULL.
        */
        ChkMem( Data.pCertificateKeys[0].pKeyUsageSet = (DRM_DWORD*)Oem_MemAlloc(SIZEOF(DRM_DWORD)) );
    }

    Data.SecurityVersion.fValid = FALSE;
    if ( argc == 9 )
    {
        /*
        ** set the security version
        */
        if( argv[7] != NULL )
        {
            ChkDR( DRMCRT_AtoDWORD( argv[7], (DRM_DWORD)strlen( argv[7] ), 10, &Data.SecurityVersion.dwSecurityVersion ) );
        }
        /*
        ** set the PlatForm ID
        */
        if( argv[8] != NULL )
        {
            ChkDR( DRMCRT_AtoDWORD( argv[8], (DRM_DWORD)strlen( argv[8] ), 10, &Data.SecurityVersion.dwPlatformIdentifier ) );
        }
        Data.SecurityVersion.fValid = TRUE;
    }

    /* Fill in default cert data */
    Data.pCertificateKeys[0].dwKeyFlags = 0;
    Data.pCertificateKeys[0].wKeyLength = ECC_P256_PUBKEY_SIZE_IN_BITS;
    Data.pCertificateKeys[0].wKeyType = DRM_BCERT_KEYTYPE_ECC256;

    Data.dwNumCertificateKeys = 1;

    /* make sure to use test security level */
    Data.dwSecurityLevel = DRM_BCERT_SECURITYLEVEL_150;
    /* Store the filled in data */
    ChkDR( _tGetSetCurCertBuildData( &Data, FALSE ) );

    dr = DRM_SUCCESS;

ErrorExit:
    SAFE_OEM_FREE( pbTestPubKey );
    /* Do not free pbTestPrivKey.  This will be handled by next call to Test_SetDefaultBuilderData, or PostTestCase */
    SAFE_OEM_FREE( pbTestIssuerKey );
    return dr;
}

/*****************************************************************************
** Function:    Test_ClearBuilderData
**
** Synopsis:    Test function to free the builder data.
**              This will overwrite any previously stored, type-specific builder data
**
** Arguments:   none
**
** Returns:     DRM_SUCCESS                                 - on success
**              DRM_E_INVALIDARG                            - if any parameter is invalid
**              or other return code from a function called within
**
** Notes:       This will overwrite any previously stored builder data for PC
**              This should only be called after calling Test_SetDefaultBuilderData
**              when the type is set to 1 (PC-type).
**
******************************************************************************/
DRM_RESULT DRM_CALL Test_ClearBuilderData(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT              dr   = DRM_E_TEST_INCOMPLETE;
    DRM_BCERT_BUILDER_DATA  Data = {0};
    DRM_DWORD               dwCount = 0;

    /* Free any memory still allocated to the cert builder data */
    ChkDR( _tGetSetCurCertBuildData( &Data, TRUE ) );

    for ( dwCount = 0; dwCount < Data.dwNumCertificateKeys; dwCount++)
    {
        SAFE_OEM_FREE( Data.pCertificateKeys[dwCount].pKeyUsageSet );
    }
    Data.dwNumCertificateKeys = 0;
    SAFE_OEM_FREE( Data.pCertificateID );
    SAFE_OEM_FREE( Data.prgdwFeatureSet );
    SAFE_OEM_FREE( Data.pCertificateKeys );
    SAFE_OEM_FREE( Data.pManufacturerStrings );
    SAFE_OEM_FREE( Data.pPrivateKey );
    if( Data.dwType == DRM_BCERT_CERTTYPE_DOMAIN )
    {
        /* Domain type */
        SAFE_OEM_FREE( Data.Type.DomainData.pServiceID );
        SAFE_OEM_FREE( Data.Type.DomainData.pAccountID );
        SAFE_OEM_FREE( Data.Type.DomainData.pDomainURL );
    }
    else if( Data.dwType == DRM_BCERT_CERTTYPE_METERING )
    {
        SAFE_OEM_FREE( Data.Type.MeteringData.pMeteringID );
        SAFE_OEM_FREE( Data.Type.MeteringData.pMeteringURL );
    }
    ZEROMEM( &Data, SIZEOF( DRM_BCERT_BUILDER_DATA ) );
    ChkDR( _tGetSetCurCertBuildData( &Data, FALSE ) );
    dr = DRM_SUCCESS;

ErrorExit:
    return dr;
}

/*****************************************************************************
** Function:    Test_AddKeyToBuilderData
**
** Synopsis:    Test function to add another key to the certificate builder data.
**              This will overwrite any previously stored, type-specific builder data
**
** Arguments:   argv[0]   : File name which holds the public key to use
**              argv[1]   : The number of key usages
**              argv[2]   : The key usage (read as decimal DWORD, for multiple usages, add values)
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
**              argv[3]    : Optional parameter telling how many times to add the key to a cert,
**                           possible values are either an integer or MAX, or MAX+-integer
**                           i.e. MAX+1, MAX-1, MAX are all valid inputs, MAX stands for DRM_BCERT_MAX_KEY_USAGES.
**
** Returns:     DRM_SUCCESS                                 - on success
**              DRM_E_INVALIDARG                            - if any parameter is invalid
**              or other return code from a function called within
**
** Notes:       This will overwrite any previously stored builder data for PC
**              This should only be called after calling Test_SetDefaultBuilderData
**              when the type is set to 1 (PC-type).
**
******************************************************************************/
DRM_RESULT DRM_CALL Test_AddKeyToBuilderData(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT dr                 = DRM_E_TEST_INCOMPLETE;
    DRM_BCERT_BUILDER_DATA Data   = {0};
    DRM_BYTE  *pbTestPubKey       = NULL;
    DRM_DWORD cbTestPubKey        = 0;
    DRM_DWORD fKeyUsages          = 0;
    DRM_DWORD dwCurKeyUsage       = 0;
    DRM_DWORD dwCurKeyUseType     = 1;
    DRM_DWORD dwCurKeyUseFlag     = 1; /* should equal 2^(dwCurKeyUseType-1) */
    DRM_DWORD dwNumberOfKeysToAdd = 1; /* default value: add a key to a cert once */
    DRM_DWORD dwCount             = 0;

    Log("Trace", "Parsing test arguments");
    ChkArg( argc >= 3 );
    ChkArg( argv[0] != NULL );
    ChkArg( argv[1] != NULL );
    ChkArg( argv[2] != NULL );

    if ( argc >= 4 ) /* how many times to add a key */
    {
       ChkArg( argv[3] != NULL );

       if ( DRM_FAILED( DRMCRT_AtoDWORD( argv[3], (DRM_DWORD)strlen( argv[3] ), 10, &dwNumberOfKeysToAdd ) )
            || dwNumberOfKeysToAdd == 0 )
        {
            if ( (DRM_DWORD)strlen( argv[3] ) < 3 || DRMCRT_strncmp ( argv[3], "MAX", 3 ) != 0 )
            {
                ChkDR(DRM_E_TEST_INVALIDARG);
            }
            if ( (DRM_DWORD)strlen( argv[3] ) == 3 ) /* MAX */
            {
                dwNumberOfKeysToAdd = DRM_BCERT_MAX_KEY_USAGES;
            }
            else /* MAX+-number */
            {
                DRM_CHAR  *pchStr = argv[3] + 4;
                if ( DRM_FAILED( DRMCRT_AtoDWORD( pchStr, (DRM_DWORD)strlen(pchStr), 10, &dwNumberOfKeysToAdd ) ) )
                {
                    ChkDR(DRM_E_TEST_INVALIDARG);
                }
                if ( DRMCRT_strncmp ( argv[3], "MAX-", 4 ) == 0 )
                {
                    dwNumberOfKeysToAdd = DRM_BCERT_MAX_KEY_USAGES - dwNumberOfKeysToAdd;
                }
                else if ( DRMCRT_strncmp ( argv[3], "MAX+", 4 ) == 0 )
                {
                    dwNumberOfKeysToAdd = DRM_BCERT_MAX_KEY_USAGES + dwNumberOfKeysToAdd;
                }
                else
                {
                    ChkDR(DRM_E_TEST_INVALIDARG);
                }
            }
        }
    }

    Log( "Trace", "\tAdd a key to a cert %d time(s)", dwNumberOfKeysToAdd );

    ChkDR( _tGetSetCurCertBuildData( &Data, TRUE ) );
    /* Add some room for test case with too many keys */
    ChkArg( Data.dwNumCertificateKeys < ( DRM_BCERT_MAX_KEY_USAGES + 5 ) );

    /* Load  public key from file */
    ChkBOOL( LoadTestFile( "files", argv[0], &pbTestPubKey, &cbTestPubKey ), DRM_E_TEST_INVALIDARG );
    ChkArg( cbTestPubKey <= DRM_BCERT_MAX_PUBKEY_VALUE_LENGTH );

    ChkDR( DRMCRT_AtoDWORD( argv[2], (DRM_DWORD)strlen( argv[2] ), 10, &fKeyUsages ) );

    for ( dwCount = 0; dwCount < dwNumberOfKeysToAdd; dwCount++ )
    {
        /* Set public key in the builder data */
        MEMCPY( Data.pCertificateKeys[Data.dwNumCertificateKeys].KeyValue.m_rgbPubkey, pbTestPubKey, cbTestPubKey );

        /* Get Number of key usages, and key usage values from arguments */
        ChkDR( DRMCRT_AtoDWORD( argv[1], (DRM_DWORD)strlen( argv[1] ), 10, &Data.pCertificateKeys[Data.dwNumCertificateKeys].dwNumKeyUsages ) );

        ChkMem( Data.pCertificateKeys[Data.dwNumCertificateKeys].pKeyUsageSet = (DRM_DWORD*)Oem_MemAlloc( SIZEOF( DRM_DWORD ) * Data.pCertificateKeys[Data.dwNumCertificateKeys].dwNumKeyUsages ) );
        ZEROMEM( Data.pCertificateKeys[Data.dwNumCertificateKeys].pKeyUsageSet,
                 SIZEOF( DRM_DWORD ) * Data.pCertificateKeys[Data.dwNumCertificateKeys].dwNumKeyUsages );

        dwCurKeyUsage = 0;
        dwCurKeyUseFlag = 1;
        dwCurKeyUseType = 1;
        while ( dwCurKeyUsage < Data.pCertificateKeys[Data.dwNumCertificateKeys].dwNumKeyUsages
             && dwCurKeyUseType < 31 )
        {
            if ( fKeyUsages & dwCurKeyUseFlag )
            {
                Data.pCertificateKeys[Data.dwNumCertificateKeys].pKeyUsageSet[dwCurKeyUsage] = dwCurKeyUseType;
                dwCurKeyUsage++;
            }
            dwCurKeyUseType++;
            dwCurKeyUseFlag = dwCurKeyUseFlag * 2;
        }

        /* Fill in default key data */
        Data.pCertificateKeys[Data.dwNumCertificateKeys].dwKeyFlags = 0;
        Data.pCertificateKeys[Data.dwNumCertificateKeys].wKeyLength = ECC_P256_PUBKEY_SIZE_IN_BITS;
        Data.pCertificateKeys[Data.dwNumCertificateKeys].wKeyType = DRM_BCERT_KEYTYPE_ECC256;
        Data.dwNumCertificateKeys++;

    }

    /* Store the filled in data */
    ChkDR( _tGetSetCurCertBuildData( &Data, FALSE ) );

    dr = DRM_SUCCESS;

ErrorExit:
    SAFE_OEM_FREE( pbTestPubKey );
    return dr;
}


/*****************************************************************************
** Function:    Test_SetSecurityLevel
**
** Synopsis:    Test function to set the security level in the builder data.
**              This will overwrite any previously stored, type-specific builder data
**
** Arguments:   argv[0]   : The new security level
**
** Returns:     DRM_SUCCESS                                 - on success
**              DRM_E_INVALIDARG                            - if any parameter is invalid
**              or other return code from a function called within
**
** Notes:       This will overwrite any previously stored builder data for PC
**              This should only be called after calling Test_SetDefaultBuilderData
**              when the type is set to 1 (PC-type).
**
******************************************************************************/
DRM_RESULT DRM_CALL Test_SetSecurityLevel(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT              dr                = DRM_E_TEST_INCOMPLETE;
    DRM_BCERT_BUILDER_DATA  Data              = {0};
    DRM_DWORD               dwSecLevel        = 0;


    Log("Trace", "Parsing test arguments");
    ChkArg( argc >= 1 );
    ChkArg( argv[0] != NULL );

    ChkDR( _tGetSetCurCertBuildData( &Data, TRUE ) );

    /* Get security level */
    ChkDR( DRMCRT_AtoDWORD( argv[0], (DRM_DWORD)strlen( argv[0] ), 10, &dwSecLevel ) );
    Data.dwSecurityLevel = dwSecLevel;

    /* Store the filled in data */
    ChkDR( _tGetSetCurCertBuildData( &Data, FALSE ) );

    dr = DRM_SUCCESS;

ErrorExit:
    return dr;
}


/*****************************************************************************
** Function:    Test_SetExpiration
**
** Synopsis:    Test function to set the expiration date in the builder data.
**              This will overwrite any previously stored, type-specific builder data
**
** Arguments:   argv[0]   : The new expiration date in hex
**
** Returns:     DRM_SUCCESS                                 - on success
**              DRM_E_INVALIDARG                            - if any parameter is invalid
**              or other return code from a function called within
**
** Notes:       This will overwrite any previously stored builder data for PC
**              This should only be called after calling Test_SetDefaultBuilderData
**              when the type is set to 1 (PC-type).
**
******************************************************************************/
DRM_RESULT DRM_CALL Test_SetExpiration(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT              dr                = DRM_E_TEST_INCOMPLETE;
    DRM_BCERT_BUILDER_DATA  Data              = {0};
    DRM_DWORD               dwExpDate         = 0;


    Log("Trace", "Parsing test arguments");
    ChkArg( argc >= 1 );
    ChkArg( argv[0] != NULL );

    ChkDR( _tGetSetCurCertBuildData( &Data, TRUE ) );

    /* Get security level */
    ChkDR( DRMCRT_AtoDWORD( argv[0], (DRM_DWORD)strlen( argv[0] ), 16, &dwExpDate ) );
    Data.dwExpirationDate = dwExpDate;

    /* Store the filled in data */
    ChkDR( _tGetSetCurCertBuildData( &Data, FALSE ) );

    dr = DRM_SUCCESS;

ErrorExit:
    return dr;
}


/*****************************************************************************
** Function:    Test_SetPCBuilderData
**
** Synopsis:    Test wrapper for the function BCert_SetPCBuilderData.
**              This will overwrite any previously stored, type-specific builder data
**
** Arguments:   argv[0]   : The Client (Hardware) ID to use
**              argv[1]   : The Security (Indiv) version to use
**              argv[2]   : File with a public key for extended data signature - "NULL" if set into NULL
**              argv[3]   : File with a private key to sign extended data, can be "NULL"
**              argv[4]   : Length of extended data
**              argv[5]   : Type of extended data
**              argv[6]   : File with extended data, optional.
**
** Returns:     DRM_SUCCESS                                 - on success
**              DRM_E_INVALIDARG                            - if any parameter is invalid
**              or other return code from a function called within
**
** Notes:       This will overwrite any previously stored builder data for PC
**              This should only be called after calling Test_SetDefaultBuilderData
**              when the type is set to 1 (PC-type).
**
******************************************************************************/
DRM_RESULT DRM_CALL Test_SetPCBuilderData(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT              dr                = DRM_E_TEST_INCOMPLETE;
    DRM_BCERT_BUILDER_DATA  Data              = {0};
    DRM_BCERT_CLIENT_ID     *pClientID        = NULL;
    DRM_DWORD               dwSecurityVersion = 0;

    DRM_BYTE         *pbTestPubKey   = NULL;
    DRM_BYTE         *pbTestPrivKey  = NULL;
    DRM_DWORD        cbTestPubKey    = 0;
    DRM_DWORD        cbTestPrivKey   = 0;
    DRM_BCERT_PUBKEY oPubKey         = {0};
    DRM_BCERT_PRIVKEY oPrivKey       = {0};
    DRM_BCERT_EXTDATA_RECORD *poExtData = NULL;
    DRM_DWORD        cbExtDataLen    = 0;

    Log("Trace", "Parsing test arguments");
    ChkArg( argc >= 7 );

    ChkDR( _tGetSetCurCertBuildData( &Data, TRUE ) );

    /* Get hardware ID from arguments */
    if ( argv[0] != NULL )
    {
        ChkMem( pClientID = (DRM_BCERT_CLIENT_ID*)Oem_MemAlloc( SIZEOF( DRM_BCERT_CLIENT_ID ) ) );
        DRM_BYT_SetBytes( pClientID, 0, SIZEOF(DRM_BCERT_CLIENT_ID), 0 );
        MEMCPY( pClientID->rgb, argv[0], min( strlen( argv[0] ), DRM_BCERT_CLIENT_ID_LENGTH ) );
    }

    /* Get security level from arguments */
    if ( argv[1] != NULL )
    {
        ChkDR( DRMCRT_AtoDWORD( argv[1], (DRM_DWORD)strlen( argv[1] ), 16, &dwSecurityVersion ) );
    }

    /* Read public key for ext data signature */
    ChkBOOL( LoadTestFile( "files", argv[2], &pbTestPubKey, &cbTestPubKey ), DRM_E_TEST_INVALIDARG );
    ChkArg( cbTestPubKey <= DRM_BCERT_MAX_PUBKEY_VALUE_LENGTH );
    MEMCPY( &oPubKey, pbTestPubKey, cbTestPubKey );

    /* Read private key to sign ext data with */
    ChkBOOL( LoadTestFile( "files", argv[3], &pbTestPrivKey, &cbTestPrivKey ), DRM_E_TEST_INVALIDARG );
    ChkArg( cbTestPrivKey == SIZEOF( PRIVKEY_P256 ) );
    MEMCPY( &oPrivKey, pbTestPrivKey, cbTestPrivKey );

    /* Allocate memory for extended data structure, it should persist till the test is done */
    ChkMem( poExtData = (DRM_BCERT_EXTDATA_RECORD*)Oem_MemAlloc( SIZEOF( DRM_BCERT_EXTDATA_RECORD ) ) );
    ZEROMEM( poExtData, SIZEOF( DRM_BCERT_EXTDATA_RECORD ) );

    /* Read length of ext data */
    poExtData->dwDataLength = (DRM_DWORD)atoi(argv[4]);

    /* Read type of ext data */
    poExtData->Header.wType = (DRM_DWORD)atoi(argv[5]);

    poExtData->Header.wFlags = DRM_BCERT_OBJFLAG_EMPTY;

    /* Optionally ext data can be provided. Otherwise, we will do it in two stages */
    if ( argv[6] != NULL )
    {
        ChkBOOL( LoadTestFile( "files", argv[6], &(poExtData->pbData), &cbExtDataLen ), DRM_E_TEST_INVALIDARG );
        ChkBOOL( poExtData->dwDataLength == cbExtDataLen, DRM_E_TEST_INVALIDARG );
    }
    else
    {
        poExtData->pbData = NULL;
    }

    /* Update the current cert builder data */
    Log("Trace", "Call BCert_SetPCBuilderData");
    BCert_SetPCBuilderData( pClientID,
                            dwSecurityVersion,
                            (PRIVKEY_P256*)&oPrivKey,
                            (PUBKEY_P256*)&oPubKey,
                            poExtData,
                            &Data );
    ChkDR( _tGetSetCurCertBuildData( &Data, FALSE ) );

    dr = DRM_SUCCESS;

ErrorExit:
    SAFE_OEM_FREE( pClientID );
    SAFE_OEM_FREE( pbTestPubKey );
    SAFE_OEM_FREE( pbTestPrivKey );

    return dr;
}

/*****************************************************************************
** Function:    Test_SetDomainBuilderData
**
** Synopsis:    Test wrapper for the function BCert_SetDomainBuilderData.
**              This will overwrite any previously stored, type-specific builder data
**
** Arguments:   argv[0]   : The Service ID
**              argv[1]   : The Domain ID
**              argv[2]   : The Revision value
**              argv[3]   : The Client ID
**              argv[4]   : The Domain URL
**              argv[5]   : The Domain URL length to use
**
** Returns:     DRM_SUCCESS                                 - on success
**              DRM_E_INVALIDARG                            - if any parameter is invalid
**              or other return code from a function called within
**
** Notes:       This will overwrite any previously stored builder data for domain
**              This should only be called after calling Test_SetDefaultBuilderData
**              when the type is set to 3 (Domain-type).
**
******************************************************************************/
DRM_RESULT DRM_CALL Test_SetDomainBuilderData(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT              dr          = DRM_E_TEST_INCOMPLETE;
    DRM_BCERT_BUILDER_DATA  Data        = {0};
    DRM_BCERT_SERVICE_ID    *pServiceID = NULL;
    DRM_BCERT_ACCOUNT_ID    *pAccountID = NULL;
    DRM_DWORD               dwRevision  = 0;
    DRM_BCERT_CLIENT_ID     *pClientID  = NULL;
    DRM_BCERT_DOMAIN_URL    *pDomainURL = NULL;

    Log("Trace", "Parsing test arguments");
    ChkArg( argc >= 6 );
    ChkArg( argv[5] != NULL ); /* Domain URL length cannot be NULL */


    ChkDR( _tGetSetCurCertBuildData( &Data, TRUE ) );
    if ( Data.Type.DomainData.pServiceID != NULL )
    {
        SAFE_OEM_FREE( Data.Type.DomainData.pServiceID );
    }
    if ( Data.Type.DomainData.pAccountID != NULL )
    {
        SAFE_OEM_FREE( Data.Type.DomainData.pAccountID );
    }
    if ( Data.Type.DomainData.pDomainURL != NULL )
    {
        SAFE_OEM_FREE( Data.Type.DomainData.pDomainURL );
    }

    /* Get Service ID from arguments */
    if ( argv[0] != NULL )
    {
        ChkMem( pServiceID = (DRM_BCERT_SERVICE_ID*)Oem_MemAlloc( SIZEOF(DRM_BCERT_SERVICE_ID) ) );
        DRM_BYT_SetBytes( pServiceID, 0, SIZEOF(DRM_BCERT_SERVICE_ID), 0 );
        MEMCPY( pServiceID->rgb, argv[0], min( strlen( argv[0] ), DRM_BCERT_SERVICE_ID_LENGTH ) );
    }

    /* Get Domain ID from arguments */
    if ( argv[1] != NULL )
    {
        ChkMem( pAccountID = (DRM_BCERT_ACCOUNT_ID*)Oem_MemAlloc( SIZEOF(DRM_BCERT_ACCOUNT_ID) ) );
        DRM_BYT_SetBytes( pAccountID, 0, SIZEOF(DRM_BCERT_ACCOUNT_ID), 0 );
        MEMCPY( pAccountID->rgb, argv[1], min( strlen( argv[1] ), DRM_BCERT_ACCOUNT_ID_LENGTH ) );
    }

    /* Get revision from arguments */
    ChkDR( DRMCRT_AtoDWORD( argv[2], (DRM_DWORD)strlen( argv[2] ), 10, &dwRevision ) );

    /* Get Client ID from arguments */
    if ( argv[3] != NULL )
    {
        ChkMem( pClientID = (DRM_BCERT_CLIENT_ID*)Oem_MemAlloc( SIZEOF(DRM_BCERT_CLIENT_ID) ) );
        DRM_BYT_SetBytes( pClientID, 0, SIZEOF(DRM_BCERT_CLIENT_ID), 0 );
        MEMCPY( pClientID->rgb, argv[3], min( strlen( argv[3] ), DRM_BCERT_CLIENT_ID_LENGTH ) );
    }

    /* Get Domain URL from arguments */
    if ( argv[4] != NULL )
    {
        ChkMem( pDomainURL = (DRM_BCERT_DOMAIN_URL*)Oem_MemAlloc( SIZEOF(DRM_BCERT_DOMAIN_URL) ) );
        DRM_BYT_SetBytes( pDomainURL, 0, SIZEOF(DRM_BCERT_DOMAIN_URL), 0 );
        MEMCPY( pDomainURL->rgb, argv[4], min( strlen( argv[4] ), DRM_BCERT_MAX_DOMAIN_URL_LENGTH ) );

        /* Get Domain URL length from arguments */
        ChkDR( DRMCRT_AtoDWORD( argv[5], (DRM_DWORD)strlen( argv[5] ), 10, &(pDomainURL->cb) ) );
    }
    /* Update the current cert builder data */
    Log("Trace", "Call BCert_SetDomainBuilderData");
    BCert_SetDomainBuilderData( pServiceID, pAccountID, dwRevision, pClientID, pDomainURL, &Data);
    ChkDR( _tGetSetCurCertBuildData( &Data, FALSE ) );

    dr = DRM_SUCCESS;

ErrorExit:
    SAFE_OEM_FREE( pClientID );
    return dr;
}

/*****************************************************************************
** Function:    Test_SetDeviceBuilderData
**
** Synopsis:    Test wrapper for the function BCert_SetDeviceBuilderData.
**              This will overwrite any previously stored, type-specific builder data
**
** Arguments:   argv[0]   : Serial Number (Client ID)
**
** Returns:     DRM_SUCCESS                                 - on success
**              DRM_E_INVALIDARG                            - if any parameter is invalid
**              or other return code from a function called within
**
** Notes:       This will overwrite any previously stored builder data for Device
**              This should only be called after calling Test_SetDefaultBuilderData
**              when the type is set to 2 (Device-type).
**
******************************************************************************/
DRM_RESULT DRM_CALL Test_SetDeviceBuilderData(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT              dr          = DRM_E_TEST_INCOMPLETE;
    DRM_BCERT_BUILDER_DATA  Data        = {0};
    DRM_BCERT_CLIENT_ID     *pClientID = NULL;

    Log("Trace", "Parsing test arguments");
    ChkArg( argc >= 1 );
    ChkDR( _tGetSetCurCertBuildData( &Data, TRUE ) );

    /* Get serial number from arguments */
    if ( argv[0] != NULL )
    {
        ChkMem( pClientID = (DRM_BCERT_CLIENT_ID*)Oem_MemAlloc( SIZEOF(DRM_BCERT_CLIENT_ID) ) );
        DRM_BYT_SetBytes( pClientID, 0, SIZEOF(DRM_BCERT_CLIENT_ID), 0 );
        MEMCPY( pClientID->rgb, argv[0], min( strlen( argv[0] ), DRM_BCERT_CLIENT_ID_LENGTH ) );
    }

    /* Update the current cert builder data */
    Log("Trace", "Call BCert_SetDeviceBuilderData");
    BCert_SetDeviceBuilderData( pClientID, &Data);
    ChkDR( _tGetSetCurCertBuildData( &Data, FALSE ) );

    dr = DRM_SUCCESS;

ErrorExit:
    SAFE_OEM_FREE( pClientID );
    return dr;
}

/*****************************************************************************
** Function:    Test_SetSilverLightBuilderData
**
** Synopsis:    Test wrapper for the function BCert_SetSilverLightBuilderData.
**              This will overwrite any previously stored, type-specific builder data
**
** Arguments:   argv[0]   : The Hardware ID to use
**              argv[1]   : The Security version to use
**              argv[2]   : The Platform Identifier to use
**
** Returns:     DRM_SUCCESS                                 - on success
**              DRM_E_INVALIDARG                            - if any parameter is invalid
**              or other return code from a function called within
**
** Notes:       This will overwrite any previously stored builder data for Silverlight
**              This should only be called after calling Test_SetDefaultBuilderData
**              when the type is set to 7 (Silverlight-type).
**
******************************************************************************/
DRM_RESULT DRM_CALL Test_SetSilverLightBuilderData(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT              dr                = DRM_E_TEST_INCOMPLETE;
    DRM_BCERT_BUILDER_DATA  Data              = {0};
    DRM_BCERT_CLIENT_ID     *pClientID        = NULL;
    DRM_DWORD               dwSecurityVersion = 0;
    DRM_DWORD               dwPlatformId      = 0;

    Log("Trace", "Parsing test arguments");
    ChkArg( argc >= 3 );

    ChkDR( _tGetSetCurCertBuildData( &Data, TRUE ) );

    /* Get hardware ID from arguments */
    if ( argv[0] != NULL )
    {
        ChkMem( pClientID = (DRM_BCERT_CLIENT_ID*)Oem_MemAlloc( SIZEOF( DRM_BCERT_CLIENT_ID ) ) );
        DRM_BYT_SetBytes( pClientID, 0, SIZEOF(DRM_BCERT_CLIENT_ID), 0 );
        MEMCPY( pClientID->rgb, argv[0], min( strlen( argv[0] ), DRM_BCERT_CLIENT_ID_LENGTH ) );
    }

    /* Get security level from arguments */
    if ( argv[1] != NULL )
    {
        ChkDR( DRMCRT_AtoDWORD( argv[1], DRMCRT_strlen( argv[1] ), 16, &dwSecurityVersion ) );
    }

    /* Get platform identifier from arguments */
    ChkDR( DRMCRT_AtoDWORD( argv[2], DRMCRT_strlen( argv[2] ), 16, &dwPlatformId ) );

    /* Update the current cert builder data */
    Log("Trace", "Call BCert_SetSilverLightBuilderData");
    BCert_SetSilverLightBuilderData( pClientID, dwSecurityVersion, dwPlatformId, &Data);
    ChkDR( _tGetSetCurCertBuildData( &Data, FALSE ) );

    dr = DRM_SUCCESS;

ErrorExit:
    SAFE_OEM_FREE( pClientID );
    return dr;
}

/*****************************************************************************
** Function:    Test_SetBuilderFlagData
**
** Synopsis:    This forces the current binary cert builder data to use non-default flags.
**              These flags are currently ignored, but are reserved for future use and so
**              the builder must be able to properly build specified flags
**
** Arguments:   argv[0]   : Chain header flags
**              argv[1]   : Basic flags
**              argv[2]   : Key flags
**              argv[3]   : Manufacturer flags
**
** Returns:     DRM_SUCCESS                                 - on success
**              DRM_E_INVALIDARG                            - if any parameter is invalid
**              or other return code from a function called within
**
** Notes:       This will overwrite any previously stored builder data for flags
**
******************************************************************************/
DRM_RESULT DRM_CALL Test_SetBuilderFlagData(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT   dr                   = DRM_E_TEST_INCOMPLETE;
    DRM_BCERT_BUILDER_DATA  Data      = {0};
    DRM_DWORD    dwChainHeaderFlags   = 0;
    DRM_DWORD    dwBasicFlags         = 0;
    DRM_DWORD    dwKeyFlags           = 0;
    DRM_DWORD    dwManufacturerFlags  = 0;

    ChkArg( argc >= 4 );
    ChkArg( argv[0] != NULL );
    ChkArg( argv[0] != NULL );
    ChkArg( argv[0] != NULL );
    ChkArg( argv[0] != NULL );

    ChkDR( DRMCRT_AtoDWORD( argv[0], (DRM_DWORD)strlen( argv[0] ), 16, &dwChainHeaderFlags ) );
    ChkDR( DRMCRT_AtoDWORD( argv[1], (DRM_DWORD)strlen( argv[1] ), 16, &dwBasicFlags ) );
    ChkDR( DRMCRT_AtoDWORD( argv[2], (DRM_DWORD)strlen( argv[2] ), 16, &dwKeyFlags ) );
    ChkDR( DRMCRT_AtoDWORD( argv[3], (DRM_DWORD)strlen( argv[3] ), 16, &dwManufacturerFlags ) );

    /* Update the current cert builder data */
    ChkDR( _tGetSetCurCertBuildData( &Data, TRUE ) );
    Data.dwChainHeaderFlags  = dwChainHeaderFlags;
    Data.dwBasicFlags        = dwBasicFlags;
    /*
    ** Was: Data.dwKeyFlags          = dwKeyFlags;
    ** Need to update this test function to support multiple public keys
    */
    Data.dwManufacturerFlags = dwManufacturerFlags;
    ChkDR( _tGetSetCurCertBuildData( &Data, FALSE ) );

    dr = DRM_SUCCESS;

ErrorExit:
    return dr;
}

/*****************************************************************************
** Function:    Test_SetManufacturerInfo
**
** Synopsis:    This forces the current binary cert builder data to use specified
**              manufacturer name, model name, and model number
**
** Arguments:   argv[0]   : Manufacturer name, if NULL other arguments are ignored
**              argv[1]   : Model Name
**              argv[2]   : Model Number
**
** Returns:     DRM_SUCCESS                                 - on success
**              DRM_E_INVALIDARG                            - if any parameter is invalid
**              or other return code from a function called within
**
** Notes:       This will overwrite any previously stored builder data for manufacturer info
**
******************************************************************************/
DRM_RESULT DRM_CALL Test_SetManufacturerInfo(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT                      dr                  = DRM_E_TEST_INCOMPLETE;
    DRM_BCERT_BUILDER_DATA          Data                = {0};

    ChkDR( _tGetSetCurCertBuildData( &Data, TRUE ) );

    if ( Data.pManufacturerStrings != NULL )
    {
        SAFE_OEM_FREE( Data.pManufacturerStrings );
    }

    /* Check for NULL manufacturer strings */
    if (argc == 0 || argv[0] == NULL )
    {
        Data.pManufacturerStrings  = NULL;
    }

    else
    {
        ChkArg( argc >= 3 );
        ChkMem( Data.pManufacturerStrings = (DRM_BCERT_MANUFACTURER_STRINGS*)Oem_MemAlloc( SIZEOF(DRM_BCERT_MANUFACTURER_STRINGS) ) );
        ZEROMEM( Data.pManufacturerStrings, SIZEOF(DRM_BCERT_MANUFACTURER_STRINGS) );

        /* Be sure to pass in NULL-terminated strings and include NULL terminator in cb value */
        DRM_BYT_SetBytes( Data.pManufacturerStrings->ManufacturerName.rgb, 0, DRM_BCERT_MAX_MANUFACTURER_STRING_LENGTH, 0 );
        Data.pManufacturerStrings->ManufacturerName.cb  = 0;
        DRM_BYT_SetBytes( Data.pManufacturerStrings->ModelName.rgb, 0, DRM_BCERT_MAX_MANUFACTURER_STRING_LENGTH, 0 );
        Data.pManufacturerStrings->ModelName.cb         = 0;
        DRM_BYT_SetBytes( Data.pManufacturerStrings->ModelNumber.rgb, 0, DRM_BCERT_MAX_MANUFACTURER_STRING_LENGTH, 0 );
        Data.pManufacturerStrings->ModelNumber.cb       = 0;

        MEMCPY( Data.pManufacturerStrings->ManufacturerName.rgb, argv[0], min( DRM_BCERT_MAX_MANUFACTURER_STRING_LENGTH-1, strlen( argv[0] ) ) );
        Data.pManufacturerStrings->ManufacturerName.cb = (DRM_DWORD)min( DRM_BCERT_MAX_MANUFACTURER_STRING_LENGTH, strlen( argv[0] ) + 1 );
        ChkArg( argv[1] != NULL );
        MEMCPY( Data.pManufacturerStrings->ModelName.rgb, argv[1], min( DRM_BCERT_MAX_MANUFACTURER_STRING_LENGTH-1, strlen( argv[1] ) ) );
        Data.pManufacturerStrings->ModelName.cb = (DRM_DWORD)min( DRM_BCERT_MAX_MANUFACTURER_STRING_LENGTH, strlen( argv[1] ) + 1 );
        ChkArg( argv[2] != NULL );
        MEMCPY( Data.pManufacturerStrings->ModelNumber.rgb, argv[2], min( DRM_BCERT_MAX_MANUFACTURER_STRING_LENGTH-1, strlen( argv[2] ) ) );
        Data.pManufacturerStrings->ModelNumber.cb = (DRM_DWORD)min( DRM_BCERT_MAX_MANUFACTURER_STRING_LENGTH, strlen( argv[2] ) + 1 );
    }

    ChkDR( _tGetSetCurCertBuildData( &Data, FALSE ) );

    dr = DRM_SUCCESS;

ErrorExit:
    return dr;
}

/*****************************************************************************
** Function:    Test_SetFeatureInfo
**
** Synopsis:    This forces the current binary cert builder data to use specified
**              features
**
** Arguments:   argv[0]   : The supported features (read as decimal DWORD)
**                          1         -> transmitter
**                          2         -> receiver
**                          4         -> shared cert
**                          8         -> secure clock
**                          16        -> antirollback clock
**                          32        -> metering               (deprecated)
**                          64        -> license sync           (deprecated)
**                          128       -> symmetric optimization (deprecated)
**                          256       -> CRL support
**                          (to indicate multiple features, just add these,.
**                          ie 34 -> metering and receiver )
**              argv[1]   : The number of features (should equal the number of features indicated in argv[0])
**
** Returns:     DRM_SUCCESS                                 - on success
**              DRM_E_INVALIDARG                            - if any parameter is invalid
**              or other return code from a function called within
**
** Notes:       This will overwrite any previously stored builder data for feature info
**
******************************************************************************/
DRM_RESULT DRM_CALL Test_SetFeatureInfo(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT                      dr                  = DRM_E_TEST_INCOMPLETE;
    DRM_DWORD                       dwNumFeatures       = 0;
    DRM_DWORD                       dwFeatureSet        = 0;
    DRM_DWORD                       dwCurFeatureType    = 1;
    DRM_DWORD                       dwCurFeatureFlag    = 1;
    DRM_DWORD                       dwCurBuilderFeature = 0;
    DRM_BCERT_BUILDER_DATA          Data                = {0};

    ChkDR( _tGetSetCurCertBuildData( &Data, TRUE ) );

    if ( Data.prgdwFeatureSet != NULL )
    {
        SAFE_OEM_FREE( Data.prgdwFeatureSet );
    }

    /* Check for NULL feature set */
    if (argc == 0 || argv[0] == NULL )
    {
        Data.prgdwFeatureSet  = NULL;
    }

    else
    {
        ChkArg( argc >= 2 );
        ChkDR( DRMCRT_AtoDWORD( argv[0], (DRM_DWORD)strlen( argv[0] ), 10, &dwFeatureSet ) );
        ChkDR( DRMCRT_AtoDWORD( argv[1], (DRM_DWORD)strlen( argv[1] ), 10, &dwNumFeatures ) );

        ChkMem( Data.prgdwFeatureSet = (DRM_DWORD*)Oem_MemAlloc( dwNumFeatures * SIZEOF(DRM_DWORD) ) );
        ZEROMEM( Data.prgdwFeatureSet, dwNumFeatures * SIZEOF(DRM_DWORD) );

        Data.dwNumFeatureEntries = dwNumFeatures;

        while (dwCurBuilderFeature < dwNumFeatures && dwCurFeatureFlag <= 4096 )
        {
            Log("Trace", "Checking for feature flag %#X in %#X.  Finding %#X.", dwCurFeatureFlag, dwFeatureSet, (dwCurFeatureFlag & dwFeatureSet));
            if ( (dwCurFeatureFlag & dwFeatureSet) != 0 )
            {
                Log("Trace", "Adding feature %#X.", dwCurFeatureType);
                Data.prgdwFeatureSet[dwCurBuilderFeature] = dwCurFeatureType;
                dwCurBuilderFeature++;
            }
            dwCurFeatureFlag = dwCurFeatureFlag * 2;
            dwCurFeatureType++;
        }

    }

    ChkDR( _tGetSetCurCertBuildData( &Data, FALSE ) );

    dr = DRM_SUCCESS;

ErrorExit:
    return dr;
}

/*****************************************************************************
** Function:    Test_AddCert
**
** Synopsis:    Test wrapper for the function BCert_AddCert.
**              This will add a cert to the current cert chain based upon the
**              current cert builder data.
**
** Arguments:   argv[0] : Optional - size of a "new chain" on the first call,
**                        otherwise call with default zero.
**
** Returns:     DRM_SUCCESS                                 - on success
**              DRM_E_INVALIDARG                            - if any parameter is invalid
**              or other return code from a function called within
**
** Notes:       Currently, cert chains which get too long will be silently trunctated.
**
******************************************************************************/
DRM_RESULT DRM_CALL Test_AddCert(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT             dr                 = DRM_E_TEST_INCOMPLETE;
    DRM_BCERT_BUILDER_DATA Data               = {0};
    DRM_BYTE               *pbParentChain     = NULL;
    DRM_BYTE               *pbNewChain        = NULL;
    DRM_DWORD              cbParentChain      = 0;
    DRM_DWORD              cbNewChain         = 0;

    ChkDR( _tGetSetCurCertBuildData( &Data, TRUE ) );
    dr = _tGetSetBCChainBuffer( pbParentChain, &cbParentChain, TRUE );
    if( dr == DRM_E_BUFFERTOOSMALL )
    {
        ChkMem( pbParentChain = (DRM_BYTE*)Oem_MemAlloc( cbParentChain * SIZEOF(DRM_BYTE) ) );
        ZEROMEM( pbParentChain, cbParentChain * SIZEOF(DRM_BYTE) );
        ChkDR( _tGetSetBCChainBuffer( pbParentChain, &cbParentChain, TRUE ) );
    }
    else
    {
        ChkDR( dr );
    }

    Log("Trace", "Calling BCert_AddCert");
    dr = BCert_AddCert( pbParentChain, pbNewChain, &cbNewChain, &Data, &g_CryptoCtx, NULL );
    if( dr == DRM_E_BUFFERTOOSMALL )
    {
        ChkMem( pbNewChain = (DRM_BYTE*)Oem_MemAlloc( cbNewChain * SIZEOF(DRM_BYTE) ) );
        ZEROMEM( pbNewChain, cbNewChain * SIZEOF(DRM_BYTE) );
        ChkDR( BCert_AddCert( pbParentChain, pbNewChain, &cbNewChain, &Data, &g_CryptoCtx, NULL ) );
    }
    else
    {
        ChkDR( dr );
    }

    ChkDR( _tGetSetBCChainBuffer( pbNewChain, &cbNewChain, FALSE ) );

    dr = DRM_SUCCESS;

ErrorExit:
    SAFE_OEM_FREE( pbParentChain );
    SAFE_OEM_FREE( pbNewChain );
    return dr;
}


/*****************************************************************************
** Function:    Test_AddExtendedDataToCert
**
** Synopsis:    Adds extended data to the certificate, extended data is already set.
**
**
** Arguments:   argv[0] : Optional, if "TRUE" then call API with NULL pointer, otherwise not
**
** Returns:     DRM_SUCCESS                                 - on success
**              DRM_E_INVALIDARG                            - if any parameter is invalid
**              or other return code from a function called within
******************************************************************************/
DRM_RESULT DRM_CALL Test_AddExtendedDataToCert(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT dr = DRM_E_TEST_INCOMPLETE;
    DRM_BCERT_BUILDER_DATA Data         = {0};
    DRM_BYTE               *pbChain     = NULL;
    DRM_DWORD              cbChain      = 0;
    DRM_BOOL               fNullExtData = FALSE;

    ChkDR( _tGetSetCurCertBuildData( &Data, TRUE ) );
    dr = _tGetSetBCChainBuffer( pbChain, &cbChain, TRUE );
    if( dr == DRM_E_BUFFERTOOSMALL )
    {
        ChkMem( pbChain = (DRM_BYTE*)Oem_MemAlloc( cbChain * SIZEOF(DRM_BYTE) ) );
        ZEROMEM( pbChain, cbChain * SIZEOF(DRM_BYTE) );
        ChkDR( _tGetSetBCChainBuffer( pbChain, &cbChain, TRUE ) );
    }
    else
    {
        ChkDR( dr );
    }

    if ( argc > 0 && argv[0] != NULL )
    {
        ChkDR( StringToBool( argv[0], &fNullExtData ) );
    }

    Log("Trace", "Calling BCert_AddExtendedDataToCert");
    ChkDR( BCert_AddExtendedDataToCert( pbChain, cbChain, fNullExtData ? NULL : &(Data.ExtendedData), &g_CryptoCtx ) );

    ChkDR( _tGetSetBCChainBuffer( pbChain, &cbChain, FALSE ) );

    dr = DRM_SUCCESS;

ErrorExit:
    SAFE_OEM_FREE( pbChain );
    return dr;
}

/*****************************************************************************
** Function:    Test_CompareCertChains
**
** Synopsis:    This compares the current cert chain to the cert chain stored
**              in a specified file.
**
** Arguments:   argv[0] : Name of the file containing the binary cert chain
**                        which is to be compared to the current cert chain
**              argv[1] : starting location for comparison
**              argv[2] : number of bytes to compare (so you don't have
**                        to compare signatures)
**
** Returns:     DRM_SUCCESS                                 - on success
**              DRM_E_INVALIDARG                            - if any parameter is invalid
**              or other return code from a function called within
**
** Notes:       Test files are required to be in a directory called "files" in
**              the same folder in which the executable is being run.
**
******************************************************************************/

DRM_RESULT DRM_CALL Test_CompareCertChains(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT dr               = DRM_E_TEST_INCOMPLETE;
    DRM_BYTE   *pbExpectedChain = NULL;
    DRM_BYTE   *pbCurrentChain  = NULL;
    DRM_DWORD  cbExpectedChain  = 0;
    DRM_DWORD  cbCurrentChain   = 0;
    DRM_DWORD  dwStartOffset    = 0;
    DRM_DWORD  dwCompareLength  = 0;


    ChkArg( argc >= 3 );
    ChkArg( argv[0] != NULL );
    ChkArg( argv[1] != NULL );
    ChkArg( argv[2] != NULL );

    ChkBOOL( LoadTestFile( "files", argv[0], &pbExpectedChain, &cbExpectedChain ), DRM_E_TEST_INVALIDARG );
    ChkDR( DRMCRT_AtoDWORD( argv[1], (DRM_DWORD)strlen( argv[1] ), 10, &dwStartOffset ) );
    ChkDR( DRMCRT_AtoDWORD( argv[2], (DRM_DWORD)strlen( argv[2] ), 10, &dwCompareLength ) );

    ChkBOOL( _tGetSetBCChainBuffer( pbCurrentChain, &cbCurrentChain, TRUE ) == DRM_E_BUFFERTOOSMALL, DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkMem( pbCurrentChain = (DRM_BYTE*)Oem_MemAlloc( cbCurrentChain ) );
    ZEROMEM( pbCurrentChain, cbCurrentChain );
    ChkDR( _tGetSetBCChainBuffer( pbCurrentChain, &cbCurrentChain, TRUE ) );
    LogBlob( "CertBuilder", "Binary Cert:", pbCurrentChain, cbCurrentChain );

    ChkBOOL( cbCurrentChain == cbExpectedChain, DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkBOOL( 0 == DRM_BYT_CompareBytes( pbCurrentChain,
                                        dwStartOffset,
                                        pbExpectedChain,
                                        dwStartOffset,
                                        min( cbExpectedChain-dwStartOffset, dwCompareLength ) ),
                  DRM_E_TEST_UNEXPECTED_OUTPUT );

    dr = DRM_SUCCESS;

ErrorExit:
    SAFE_OEM_FREE( pbExpectedChain );
    return dr;
}


/*****************************************************************************
** Function:    Test_SetMeteringBuilderData
**
** Synopsis:    Test wrapper for the function BCert_SetMeteringBuilderData.
**              This will overwrite any previously stored, type-specific builder data
**
** Arguments:   argv[0]   : The Metering ID
**              argv[1]   : The Metering URL
**              argv[2]   : The Metering URL length to use
**
** Returns:     DRM_SUCCESS                                 - on success
**              DRM_E_INVALIDARG                            - if any parameter is invalid
**              or other return code from a function called within
**
** Notes:       This will overwrite any previously stored builder data for metering
**              This should only be called after calling Test_SetDefaultBuilderData
**              when the type is set to 9 (Metering-type).
**
******************************************************************************/
DRM_RESULT DRM_CALL Test_SetMeteringBuilderData(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT              dr            = DRM_E_TEST_INCOMPLETE;
    DRM_BCERT_BUILDER_DATA  Data          = {0};
    DRM_BCERT_METERING_ID   *pMeteringID  = NULL;
    DRM_BCERT_METERING_URL  *pMeteringURL = NULL;

    Log("Trace", "Parsing test arguments");
    ChkArg( argc >= 3 );
    ChkArg( argv[2] != NULL ); /* Metering URL length cannot be NULL */


    ChkDR( _tGetSetCurCertBuildData( &Data, TRUE ) );
    SAFE_OEM_FREE( Data.Type.MeteringData.pMeteringID );
    SAFE_OEM_FREE( Data.Type.MeteringData.pMeteringURL );

    /* Get Metering ID from arguments */
    if ( argv[0] != NULL )
    {
        ChkMem( pMeteringID = (DRM_BCERT_METERING_ID*)Oem_MemAlloc( SIZEOF(DRM_BCERT_METERING_ID) ) );
        DRM_BYT_SetBytes( pMeteringID, 0, SIZEOF(DRM_BCERT_METERING_ID), 0 );
        MEMCPY( pMeteringID->rgb, argv[0], min( strlen( argv[0] ), DRM_BCERT_METERING_ID_LENGTH ) );
    }

    /* Get Metering URL from arguments */
    if ( argv[1] != NULL )
    {
        ChkMem( pMeteringURL = (DRM_BCERT_METERING_URL*)Oem_MemAlloc( SIZEOF(DRM_BCERT_METERING_URL) ) );
        DRM_BYT_SetBytes( pMeteringURL, 0, SIZEOF(DRM_BCERT_METERING_URL), 0 );
        MEMCPY( pMeteringURL->rgb, argv[1], min( strlen( argv[1] ), DRM_BCERT_MAX_METERING_URL_LENGTH ) );

        /* Get Metering URL length from arguments */
        ChkDR( DRMCRT_AtoDWORD( argv[2], (DRM_DWORD)strlen( argv[2] ), 10, &(pMeteringURL->cb) ) );
    }

    /* Update the current cert builder data */
    Log("Trace", "Call BCert_SetMeteringBuilderData");
    BCert_SetMeteringBuilderData( pMeteringID, pMeteringURL, &Data);
    ChkDR( _tGetSetCurCertBuildData( &Data, FALSE ) );

    dr = DRM_SUCCESS;

ErrorExit:
    return dr;
}

/*****************************************************************************
** Function:    Test_SetServerBuilderData
**
** Synopsis:    Test wrapper for the function BCert_SetServerBuilderData.
**              This will overwrite any previously stored, type-specific builder data
**
** Arguments:   argv[0]   : Server certificate expiration warning start time (in sec from Jan 1, 1970)
**
** Returns:     DRM_SUCCESS                                 - on success
**              DRM_E_INVALIDARG                            - if any parameter is invalid
**              or other return code from a function called within
**
** Notes:       This will overwrite any previously stored builder data for Server
**              This should only be called after calling Test_SetDefaultBuilderData
**              when the type is set to 11 (Server-type).
**
******************************************************************************/
DRM_RESULT DRM_CALL Test_SetServerBuilderData(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT              dr          = DRM_E_TEST_INCOMPLETE;
    DRM_BCERT_BUILDER_DATA  Data        = {0};
    DRM_DWORD               dwWarningStartDate = 0;

    Log("Trace", "Parsing test arguments");
    ChkArg( argc >= 1 );
    ChkDR( _tGetSetCurCertBuildData( &Data, TRUE ) );

    /* Get warning start date from arguments */
    if ( argv[0] != NULL )
    {
        ChkDR( DRMCRT_AtoDWORD( argv[0], (DRM_DWORD)strlen( argv[0] ), 16, &dwWarningStartDate ) );
    }

    /* Update the current cert builder data */
    Log("Trace", "Call BCert_SetServerBuilderData");
    BCert_SetServerBuilderData( dwWarningStartDate, &Data);
    ChkDR( _tGetSetCurCertBuildData( &Data, FALSE ) );

    dr = DRM_SUCCESS;

ErrorExit:
    return dr;
}

/*****************************************************************************
** Function:    _addAlignedData
**
** Synopsis:    Adds a sequence of bytes to a binary certificate, with padding
**              characters inserted at the end to ensure alignment on a 32-bit boundary.
**
** Arguments:   [f_pbDestination] : A pointer to a buffer to receive the bytes
**              [f_pdwDestOffset] : A pointer to the offset within the destination buffer; updated
**              [f_pbSource]      : Address of where to get the bytes from
**              [f_cbAmount]      : How many bytes to copy
**
** Returns:     DRM_SUCCESS      - on success
**              DRM_E_INVALIDARG - if any parameter is invalid
**
** Notes:       None.
**
******************************************************************************/
static DRM_RESULT _addAlignedData(
  __out_bcount(f_cbAmount + *f_pdwDestOffset + 3) DRM_BYTE   *f_pbDestination,
  __inout                                         DRM_DWORD  *f_pdwDestOffset,
  __in_bcount(f_cbAmount)                         DRM_BYTE   *f_pbSource,
  __in                                     const  DRM_DWORD   f_cbAmount
)
{
    DRM_RESULT  dr         = DRM_SUCCESS;
    DRM_DWORD   dwPadCount = 0;
    DRM_BYTE    bPadValue  = '\0';

    ChkArg( f_pbDestination != NULL );
    ChkArg( f_pdwDestOffset != NULL );
    ChkArg( f_pbSource      != NULL );
    ChkArg( f_cbAmount       > 0    );

    DRM_BYT_CopyBytes( f_pbDestination, *f_pdwDestOffset, f_pbSource, 0, f_cbAmount );
    *f_pdwDestOffset += f_cbAmount;

    /* Padding, if needed to align on 32-bit boundary */
    for ( dwPadCount = PAD_AMOUNT( f_cbAmount ); dwPadCount > 0; dwPadCount-- )
    {
        PUT_BYTE( f_pbDestination, (*f_pdwDestOffset)++, bPadValue );
    }

 ErrorExit:
    return dr;
}

/*****************************************************************************
** Function:    _addCertChainHeader
**
** Synopsis:    Adds the certificate chain header to the certificate chain.
**
** Arguments:   [f_pbBuffer] : A pointer to an empty bufffer to contain the new certificate chain
**              [f_dwFlags]  : The flags (a bit-field) to set in the certificate chain header
**
** Returns:     DRM_SUCCESS      - on success
**              DRM_E_INVALIDARG - if the buffer pointer is NULL
**
** Notes:       None.
**
******************************************************************************/
static DRM_RESULT _addCertChainHeader (
               DRM_BYTE   *f_pbBuffer,
  __in  const  DRM_DWORD   f_dwFlags
)
{
    DRM_RESULT dr                = DRM_SUCCESS;
    DRM_DWORD  iBuffer           = 0;

    /*
    ** The following variables are necessary because the big-endian version of
    ** the DWORD_TO_NETWORKBYTES macro cannot take a constant as the last parameter.
    */
    DRM_DWORD  dwChainHeaderTag  = DRM_BCERT_CHAIN_HEADER_TAG;
    DRM_DWORD  dwChainVersion    = DRM_BCERT_CHAIN_VERSION;
    DRM_DWORD  dwChainHeaderSize = SIZEOF( DRM_BCERT_CHAIN_HEADER );
    DRM_DWORD  dwZero            = 0;



    ChkArg( f_pbBuffer != NULL );

    /*
    ** Add the Chain Header Tag & Chain Version
    */
    DWORD_TO_NETWORKBYTES( f_pbBuffer, iBuffer, dwChainHeaderTag );
    iBuffer += SIZEOF( DRM_DWORD );

    DWORD_TO_NETWORKBYTES( f_pbBuffer, iBuffer, dwChainVersion );
    iBuffer += SIZEOF( DRM_DWORD );

    /*
    ** Add the length in bytes of binary certificate chain structure, including this structure.
    ** This will be updated after each certificate is added.
    */
    DWORD_TO_NETWORKBYTES( f_pbBuffer, iBuffer, dwChainHeaderSize );
    iBuffer += SIZEOF( DRM_DWORD );

    /*
    ** Add any flags
    */
    DWORD_TO_NETWORKBYTES( f_pbBuffer, iBuffer, f_dwFlags );
    iBuffer += SIZEOF( DRM_DWORD );

    /*
    ** Number of entries in this certificate chain.
    ** This will be updated after each certificate is added.
    */
    DWORD_TO_NETWORKBYTES( f_pbBuffer, iBuffer, dwZero );

 ErrorExit:
    return dr;
}

/*****************************************************************************
** Function:    _addCertObjectHeader
**
** Synopsis:    Adds the header of a certificate object.
**
** Arguments:   [f_pbBuffer] : The address at which to start writing these data
**              [f_dwFlags]  : The flags (a bit-field) for this object header
**              [f_dwType]   : The type of certificate object that this is for
**              [f_cbLength] : The length of this certificate object
**
** Returns:     DRM_SUCCESS      - on success
**              DRM_E_INVALIDARG - if any parameter is invalid
**
** Notes:       For future usability, this routine does not check the validity
**              of the certificate object type, just that it is non-zero.
**
******************************************************************************/
static DRM_RESULT _addCertObjectHeader (
                 DRM_BYTE    *f_pbBuffer,
  __in     const DRM_WORD     f_dwFlags,
  __in     const DRM_WORD     f_dwType,
  __in     const DRM_DWORD    f_cbLength)
{
    DRM_RESULT  dr      = DRM_SUCCESS;
    DRM_DWORD   iBuffer = 0;


    ChkArg( f_pbBuffer != NULL );
    ChkArg( f_dwType   != 0 );
    ChkArg( f_cbLength != 0 );

    /*
    ** Add the Flags, Object Type, then Object Length.
    */
    WORD_TO_NETWORKBYTES( f_pbBuffer, iBuffer, f_dwFlags );
    iBuffer += SIZEOF( DRM_WORD );

    WORD_TO_NETWORKBYTES( f_pbBuffer, iBuffer, f_dwType );
    iBuffer += SIZEOF( DRM_WORD );

    DWORD_TO_NETWORKBYTES( f_pbBuffer, iBuffer, f_cbLength );

 ErrorExit:
    return dr;
}

/*****************************************************************************
** Function:    _addCertSignatureInfo
**
** Synopsis:    Adds the Signature Information object to a certificate.
**
** Arguments:   [f_pbBuffer]       : The address at which to start writing these data
**              [f_pData]          : A pointer to all the input data
**              [f_cbAmountToSign] : The portion of this certificate to be signed (in bytes)
**              [f_cbSize]         : The previously calculated size of this object (in bytes)
**              [f_pCryptoCtx]     : Pointer to DRM_CRYPTO_CONTEXT used for temporary data in crypto code.
**
** Returns:     DRM_SUCCESS                           - on success
**              DRM_E_INVALIDARG                      - if any parameter is invalid
**              DRM_E_BCERT_PRIVATE_KEY_NOT_SPECIFIED - if the private key is NULL
**              DRM_E_BCERT_ISSUER_KEY_NOT_SPECIFIED  - if the issuer key is NULL
**              DRM_E_BCERT_INVALID_KEY_LENGTH        - if the issuer key length is unsupported
**              DRM_E_BCERT_INVALID_SIGNATURE_TYPE    - if the signature type is unsupported
**              or other return code from a function called within
**
** Notes:       None.
**
******************************************************************************/
static DRM_RESULT _addCertSignatureInfo(
               DRM_BYTE                *f_pbBuffer,
  __in         DRM_BCERT_BUILDER_DATA  *f_pData,
  __in  const  DRM_DWORD                f_cbAmountToSign,
  __in  const  DRM_DWORD                f_cbSize,
  __inout      DRM_CRYPTO_CONTEXT      *f_pCryptoCtx
)
{
    DRM_RESULT       dr               = DRM_SUCCESS;
    DRM_DWORD        iBuffer          = SIZEOF( DRM_BCERT_OBJECT_HEADER );
    SIGNATURE_P256   Signature        = {0};
    DRM_WORD         wSignatureLength = 0;


    ChkArg( f_pbBuffer       != NULL );
    ChkArg( f_pData          != NULL );
    ChkArg( f_cbAmountToSign  > 0    );
    ChkArg( f_cbSize          > 0    );
    ChkArg( f_pCryptoCtx     != NULL );
    /*
    ** Check existance of required keys
    */
    ChkBOOL( f_pData->pPrivateKey       != NULL, DRM_E_BCERT_PRIVATE_KEY_NOT_SPECIFIED );
    ChkBOOL( f_pData->dwIssuerKeyLength != 0,    DRM_E_BCERT_ISSUER_KEY_NOT_SPECIFIED  );

    /*
    ** Check supported key lengths
    */
    ChkBOOL( f_pData->dwIssuerKeyLength == ECC_P256_PUBKEY_SIZE_IN_BITS, DRM_E_BCERT_INVALID_KEY_LENGTH );

    /*
    ** Add the Object Header
    */
    ChkDR( _addCertObjectHeader( f_pbBuffer,
                                 DRM_BCERT_OBJFLAG_MUST_UNDERSTAND,
                                 DRM_BCERT_OBJTYPE_SIGNATURE,
                                 f_cbSize ) );

    if ( f_pData->wSignatureType == DRM_BCERT_SIGNATURE_TYPE_P256 )
    {
        /*
        ** Add the Signature Type
        */
        WORD_TO_NETWORKBYTES( f_pbBuffer, iBuffer, f_pData->wSignatureType );
        iBuffer += SIZEOF( DRM_WORD );

        /*
        ** Sign the data in this certificate
        */
        ChkDR( OEM_ECDSA_Sign_P256( (f_pbBuffer - f_cbAmountToSign),
                                     f_cbAmountToSign,
                                     f_pData->pPrivateKey,
                                     &Signature,
                                     (struct bigctx_t *)f_pCryptoCtx->rgbCryptoContext ) );
        wSignatureLength = SIZEOF( SIGNATURE_P256 );
    }
    else
    {
        ChkDR( DRM_E_BCERT_INVALID_SIGNATURE_TYPE );
    }

    /*
    ** Add the signature length and signature data
    */
    WORD_TO_NETWORKBYTES( f_pbBuffer, iBuffer, wSignatureLength );
    iBuffer += SIZEOF( DRM_WORD );

    ChkDR( _addAlignedData(f_pbBuffer, &iBuffer, Signature.m_rgbSignature, wSignatureLength ) );

    /*
    ** Add the Issuer Key Length (in bits)
    */
    DWORD_TO_NETWORKBYTES( f_pbBuffer, iBuffer, f_pData->dwIssuerKeyLength );
    iBuffer += SIZEOF( DRM_DWORD );

    /*
    ** Add the Issuer Key (should be the public key from the next certificate in this chain)
    */
    ChkDR( _addAlignedData( f_pbBuffer,
                             &iBuffer,
                             f_pData->IssuerKey.m_rgbPubkey,
                             ( f_pData->dwIssuerKeyLength / CHAR_BIT ) ) );

 ErrorExit:
    return dr;
}

/*****************************************************************************
** Function:    Test_SignAndAddCert
**
** Synopsis:    Test function which takes a bcert with no signature object, adds
**              a signature, and adds it to the shared cert.
**
** Arguments:   argv[0]   : File name of the unsigned cert
**
** Returns:     DRM_SUCCESS                                 - on success
**              DRM_E_INVALIDARG                            - if any parameter is invalid
**              or other return code from a function called within
**
** Notes:       This uses the issuer keys set in the shared builder data
**
******************************************************************************/
DRM_RESULT DRM_CALL Test_SignAndAddCert(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT              dr                  = DRM_E_TEST_INCOMPLETE;
    DRM_BCERT_BUILDER_DATA  Data                = {0};
    DRM_BYTE               *pbTestUnsignedCert  = NULL;
    DRM_DWORD              cbTestUnsignedCert   = 0;
    DRM_BYTE               *pbParentChain       = NULL;
    DRM_BYTE               *pbNewChain          = NULL;
    DRM_DWORD              cbParentChain        = 0;
    DRM_DWORD              cbNewChain           = 0;
    DRM_DWORD              cbSignatureInfo      = 0;
    DRM_DWORD              cCerts               = 0;
    DRM_DWORD              iOffset              = 0;

    Log("Trace", "Parsing test arguments");
    ChkArg( argc >= 1 );
    ChkArg( argv[0] != NULL );

    /* Get builder data for the issuer keys */
    ChkDR( _tGetSetCurCertBuildData( &Data, TRUE ) );

    /* Load signature-less cert from file */
    ChkBOOL( LoadTestFile( "files", argv[0], &pbTestUnsignedCert, &cbTestUnsignedCert ), DRM_E_TEST_INVALIDARG );

    /* Get Parent Chain */
    dr = _tGetSetBCChainBuffer( pbParentChain, &cbParentChain, TRUE );
    if( dr == DRM_E_BUFFERTOOSMALL )
    {
        ChkMem( pbParentChain = (DRM_BYTE*)Oem_MemAlloc( cbParentChain * SIZEOF(DRM_BYTE) ) );
        ZEROMEM( pbParentChain, cbParentChain * SIZEOF(DRM_BYTE) );
        ChkDR( _tGetSetBCChainBuffer( pbParentChain, &cbParentChain, TRUE ) );
    }
    else
    {
        ChkDR( dr );
    }

    /* Calc new chain size */
    cbSignatureInfo = SIZEOF( DRM_BCERT_OBJECT_HEADER )                              /* object header     */
                        + SIZEOF( DRM_WORD )                             /* signature type    */
                        + SIZEOF( DRM_WORD )                             /* signature length  */
                        + SIZEOF( SIGNATURE_P256 )                       /* signature data    */
                        + PAD_AMOUNT( SIZEOF( SIGNATURE_P256 ) )                 /* padding           */
                        + SIZEOF( DRM_DWORD )                            /* issuer key length */
                        + ( ECC_P256_PUBKEY_SIZE_IN_BITS / CHAR_BIT )             /* issuer key value  */
                        + PAD_AMOUNT( ECC_P256_PUBKEY_SIZE_IN_BITS / CHAR_BIT );  /* padding           */
    cbNewChain = cbParentChain + cbTestUnsignedCert + cbSignatureInfo;

    ChkMem( pbNewChain = (DRM_BYTE*)Oem_MemAlloc( cbNewChain * SIZEOF(DRM_BYTE) ) );
    ZEROMEM( pbNewChain, cbNewChain * SIZEOF(DRM_BYTE) );

    /*
    ** If this is the first certificate in the chain, add a chain header,
    ** otherwise, copy the existing chain header.
    */
    if ( pbParentChain == NULL )
    {
        ChkDR( _addCertChainHeader( pbNewChain, 0 ) );
    }
    else
    {
        DRM_BYT_CopyBytes( pbNewChain, 0, pbParentChain, 0, SIZEOF( DRM_BCERT_CHAIN_HEADER ) );
    }
    iOffset += SIZEOF( DRM_BCERT_CHAIN_HEADER );

    /*
    ** Add unsigned cert
    */
    DRM_BYT_CopyBytes( pbNewChain, iOffset, pbTestUnsignedCert, 0, cbTestUnsignedCert );
    iOffset += cbTestUnsignedCert;

    /*
    ** Add the Signature Information
    */
    ChkDR( _addCertSignatureInfo( pbNewChain + iOffset,
                                  &Data,
                                  cbTestUnsignedCert,  /* # of bytes to sign */
                                  cbSignatureInfo,
                                  &g_CryptoCtx ) );
    iOffset += cbSignatureInfo;

    /*
    ** If this is not the first certificate in the chain, then we need to copy the parent chain
    ** after the certificate that we just added
    */
    if ( pbParentChain != NULL )
    {
        DRM_BYT_CopyBytes( pbNewChain, iOffset, pbParentChain, SIZEOF( DRM_BCERT_CHAIN_HEADER ), cbParentChain - SIZEOF( DRM_BCERT_CHAIN_HEADER ) );
    }

    /*
    ** Update the length in bytes of binary certificate chain structure, including this structure.
    ** Get the current value, increment it, then set the new value.
    */
    DWORD_TO_NETWORKBYTES( pbNewChain, DRM_BCERT_CHAIN_SIZE_OFFSET, cbNewChain);

    /*
    ** Update the number of entries in this certificate chain.
    ** Get the current value, increment it, then set the new value.
    */
    NETWORKBYTES_TO_DWORD( cCerts, pbNewChain, DRM_BCERT_CHAIN_COUNT_OFFSET);
    cCerts += 1;
    DWORD_TO_NETWORKBYTES( pbNewChain, DRM_BCERT_CHAIN_COUNT_OFFSET, cCerts );

    /* Save new chain */
    ChkDR( _tGetSetBCChainBuffer( pbNewChain, &cbNewChain, FALSE ) );

    dr = DRM_SUCCESS;

ErrorExit:
    SAFE_OEM_FREE( pbTestUnsignedCert );
    SAFE_OEM_FREE( pbParentChain );
    SAFE_OEM_FREE( pbNewChain );
    return dr;
}

EXIT_PKTEST_NAMESPACE_CODE
