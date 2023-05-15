/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdafx.h>
#include <drmkeyfile.h>
#include <oemkeyfile.h>
#include <drmblackbox.h>
#include <KeyFileTestAPI.h>
#include <drmmathsafe.h>

ENTER_PKTEST_NAMESPACE_CODE;

static TST_DWORD_VALUE_MAP_ENTRY g_rgoKFCertTypeMap[] = {
    DECLARE_DWORD_MAP_ENTRY( eKF_CERT_TYPE_INVALID ),
    DECLARE_DWORD_MAP_ENTRY( eKF_CERT_TYPE_NDT ),
    DECLARE_DWORD_MAP_ENTRY( eKF_CERT_TYPE_WMDRM ),
    DECLARE_DWORD_MAP_ENTRY( eKF_CERT_TYPE_PLAYREADY ),
    { CREATE_DRM_ANSI_STRING( "CERT_TYPE_ABOVE_MAX" ), 4 },
    EMPTY_DWORD_MAP_ENTRY
};

static TST_DWORD_VALUE_MAP_ENTRY g_rgoKFKeyTypeMap[] = {
    DECLARE_DWORD_MAP_ENTRY( eKF_KEY_TYPE_INVALID ),
    DECLARE_DWORD_MAP_ENTRY( eKF_KEY_TYPE_ECC_256 ),
    DECLARE_DWORD_MAP_ENTRY( eKF_KEY_TYPE_RSA ),
    DECLARE_DWORD_MAP_ENTRY( eKF_KEY_TYPE_ECC_160 ),
    DECLARE_DWORD_MAP_ENTRY( eKF_KEY_SECURE_STORE ),
    { CREATE_DRM_ANSI_STRING( "KEY_TYPE_ABOVE_MAX" ), 5 },
    EMPTY_DWORD_MAP_ENTRY
};

static TST_DWORD_VALUE_MAP_ENTRY g_rgoKFKeyEncryptionTypeMap[] = {
    DECLARE_DWORD_MAP_ENTRY( KF_DECRYPTION_TYPE_CLEAR ),
    DECLARE_DWORD_MAP_ENTRY( KF_DECRYPTION_TYPE_AES_CBC ),
    DECLARE_DWORD_MAP_ENTRY( KF_DECRYPTION_TYPE_AES_CTR_WM7_SST ),
    DECLARE_DWORD_MAP_ENTRY( KF_DECRYPTION_TYPE_AES_CTR_WM7 ),
    DECLARE_DWORD_MAP_ENTRY( KF_DECRYPTION_TYPE_AES_CTR_WM7_PLAIN ),
    DECLARE_DWORD_MAP_ENTRY( KF_DECRYPTION_TYPE_ACTIVATION_SST ),
    { CREATE_DRM_ANSI_STRING( "KEY_DECRYPTION_TYPE_ABOVE_MAX" ), KF_DECRYPTION_TYPE_MAX_VALUE + 1 },
    EMPTY_DWORD_MAP_ENTRY
};


/*
**  If you add/change an entry here, you must also add/change the corresponding Free call
**  in this module's post-test-case function.
*/
const DRM_CHAR g_KF_Context_1[] = {"KF_Context_1"};
const DRM_CHAR g_KF_Context_2[] = {"KF_Context_2"};
const DRM_CHAR g_KF_Context_3[] = {"KF_Context_3"};
const DRM_CHAR g_KF_Context_4[] = {"KF_Context_4"};


/*
** Callback "destructor" for DRM_KEYFILE_CONTEXT test variables:
** it needs to close the file to release it for the next test.
*/
DRM_VOID DRM_CALL _CleanKFContextVar( __in const DRM_VOID *f_pvTestVar )
{
    DRM_KEYFILE_CONTEXT     *pKFContext  = NULL;
    const DRM_TEST_VARIABLE *f_poTestVar = (const DRM_TEST_VARIABLE*)f_pvTestVar;
    
    if ( f_pvTestVar == NULL )
    {
        return;
    }

    DRM_TST_GetTestVar( f_poTestVar->szName, (DRM_VOID**)&pKFContext, NULL );

    if ( pKFContext != NULL )
    {
        (void)DRM_KF_Close( pKFContext );
    }
}


/********************************************************************************************
** Function:    TST_MAP_GetDWORD
**
** Synopsis:    Searches the array in f_pMap for the Key f_pdastrSearch and Returns the value in f_pdwValue.
**
** Arguments:   f_pMap          -- Array of key-value pairs to search
**              f_pdastrSearch  -- Key string into the array
**              f_pdwValue      -- Output value for the search
**
** Returns:     DRM_SUCCESS
**              DRM_E_TEST_INVALIDARG
**              DRM_E_TEST_NOMORE
********************************************************************************************/
DRM_RESULT DRM_CALL TST_MAP_GetDWORD( const TST_DWORD_VALUE_MAP_ENTRY *f_pMap,
                                      const DRM_ANSI_CONST_STRING     *f_pdastrSearch,
                                            DRM_DWORD                 *f_pdwValue )
{
    DRM_RESULT dr     = DRM_SUCCESS;
    DRM_DWORD  iEntry = 0;
    DRM_BOOL   fFound = FALSE;

    ChkArg( f_pMap         != NULL );
    ChkArg( f_pdastrSearch != NULL );
    ChkArg( f_pdwValue     != NULL );

    while ( f_pMap[ iEntry ].dastrName.pszString != NULL
         && !fFound )
    {
        if ( f_pMap[ iEntry ].dastrName.cchString == f_pdastrSearch->cchString
          && 0 == DRMCRT_strncmp( f_pMap[ iEntry ].dastrName.pszString,
                                  f_pdastrSearch->pszString,
                                  f_pdastrSearch->cchString ) )
        {
            *f_pdwValue = f_pMap[ iEntry ].dwValue;
            fFound = TRUE;
        }
        iEntry++;
    }
    ChkBOOL( fFound, DRM_E_TEST_NOMORE );

ErrorExit:
    return dr;
}

/********************************************************************************************
** Function:    PreTestCase
**
** Synopsis:    Pretest case for the KeyFile test module
**
** Arguments:   Both arguments are ignored.
**              lTCID     -- the test case in operation incase there is special code for that test case.
**              strTCName -- The name of the test case
**
** Returns:     DRM_SUCCESS
********************************************************************************************/
DRM_RESULT DRM_CALL MODULE_PRETESTCASE(KeyFile)(long lTCID, const char *strTCName)
{
    DRM_RESULT dr = DRM_SUCCESS;
    ChkDR( Oem_Platform_Init() );
    ChkDR( RemoveDRMFile( RMFILE_ALL ) );
ErrorExit:
    return dr;
}

/********************************************************************************************
** Function:    PostTestCase
**
** Synopsis:    Posttest case for the KeyFile test module
**
** Arguments:   Both arguments are ignored.
**              lTCID     -- the test case in operation incase there is special code for that test case.
**              strTCName -- The name of the test case
**
** Returns:     DRM_SUCCESS
********************************************************************************************/
DRM_RESULT DRM_CALL MODULE_POSTTESTCASE(KeyFile)(long lTCID, const char *strTCName)
{
    DRM_RESULT    dr         = DRM_SUCCESS;
    DRM_VOID     *pKFContext = NULL;
    
    if ( DRM_SUCCEEDED( DRM_TST_GetTestVar( g_KF_Context_1, &pKFContext, NULL ) ) )
    {
        POSTTESTCASE_FUNCTION_CALL( Test_KF_FreeContext( 1, (char**)&g_KF_Context_1 ) );
    }
    if ( DRM_SUCCEEDED( DRM_TST_GetTestVar( g_KF_Context_2, &pKFContext, NULL ) ) )
    {
        POSTTESTCASE_FUNCTION_CALL( Test_KF_FreeContext( 1, (char**)&g_KF_Context_2 ) );
    }
    if ( DRM_SUCCEEDED( DRM_TST_GetTestVar( g_KF_Context_3, &pKFContext, NULL ) ) )
    {
        POSTTESTCASE_FUNCTION_CALL( Test_KF_FreeContext( 1, (char**)&g_KF_Context_3 ) );
    }
    if ( DRM_SUCCEEDED( DRM_TST_GetTestVar( g_KF_Context_4, &pKFContext, NULL ) ) )
    {
        POSTTESTCASE_FUNCTION_CALL( Test_KF_FreeContext( 1, (char**)&g_KF_Context_4 ) );
    }
    
    POSTTESTCASE_FUNCTION_CALL( RemoveDRMFile( RMFILE_ALL ) );
    POSTTESTCASE_FUNCTION_CALL( Oem_Platform_UnInit() );

/* Do NOT define ErrorExit label here.  Do NOT use ChkDR (etc) above.  Doing so could skip cleanup operations. */
    return dr;
}

/********************************************************************************************
** Function:    Test_KF_ReplaceKeyFile
**
** Synopsis:    Copies a file from one location to another.
**
** Arguments:   argv[0] : Path to replacement Keyfile
**
********************************************************************************************/
DRM_RESULT DRM_CALL Test_KF_ReplaceKeyFile( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT       dr          = DRM_SUCCESS;
    DRM_CONST_STRING dstrKeyFile = EMPTY_DRM_STRING;

    ChkArg( argc == 1 && argv[0] != NULL );

    ChkDR( MakeDRMString( &dstrKeyFile, argv[0] ) );

    ChkDR( _CopyFileToDRMFolder( dstrKeyFile.pwszString , KEYFILE_FILE_NAME ) );


ErrorExit:

    FREEDRMSTRING( dstrKeyFile );
    return dr;
}

/********************************************************************************************
** Function:    Test_KF_CreateContext
**
** Synopsis:    Creates the KeyFile context for testing.
**
** Arguments:   argv[0] -- Shared Variable name in which to store the context
**
** Returns:     DRM_SUCCESS
********************************************************************************************/
DRM_RESULT DRM_CALL Test_KF_CreateContext( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT           dr           = DRM_SUCCESS;
    DRM_KEYFILE_CONTEXT *pKFContext   = NULL;
    DRM_DWORD            dwArgv0Len   = 0;
    DRM_DWORD            dwContextLen = 0;
    DRM_DWORD            dwCompareLen = 0;

    ChkArg( argc    >  0 );
    ChkArg( argv[0] != NULL );

    dwArgv0Len   = (DRM_DWORD)DRMCRT_strlen( argv[0] );
    dwContextLen = (DRM_DWORD)DRMCRT_strlen( g_KF_Context_1 );   /* they are all the same length */
    dwCompareLen = MAX( dwArgv0Len, dwContextLen );
    
    ChkArg( DRMCRT_strncmp( argv[0], g_KF_Context_1, dwCompareLen ) == 0
         || DRMCRT_strncmp( argv[0], g_KF_Context_2, dwCompareLen ) == 0
         || DRMCRT_strncmp( argv[0], g_KF_Context_3, dwCompareLen ) == 0
         || DRMCRT_strncmp( argv[0], g_KF_Context_4, dwCompareLen ) == 0 );

    MAP_TO_GLOBAL_TEST_VAR_Alloc_WithCallback( pKFContext,
                                               argv[0],
                                               SIZEOF( DRM_KEYFILE_CONTEXT ),
                                               _CleanKFContextVar );
ErrorExit:
    return dr;
}

/********************************************************************************************
** Function:    Test_KF_FreeContext
**
** Synopsis:    Frees the KeyFile context
**
** Arguments:   argv[0] -- Shared Variable name holding the context
**
** Returns:     DRM_SUCCESS
********************************************************************************************/
DRM_RESULT DRM_CALL Test_KF_FreeContext( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT           dr         = DRM_SUCCESS;
    DRM_KEYFILE_CONTEXT *pKFContext = NULL;

    ChkArg( argc    >  0 );
    ChkArg( argv[0] != NULL );

    MAP_TO_GLOBAL_TEST_VAR( pKFContext, argv[0] );

    if ( pKFContext->fInited
      && pKFContext->hKeyfile != OEM_INVALID_HANDLE_VALUE )
    {
        ChkDR( DRM_KF_Close( pKFContext ) );
    }

    ChkDR( DRM_TST_FreeTestVar( argv[0] ) );

ErrorExit:
    return dr;
}

/********************************************************************************************
** Function:    Test_KF_Initialize
**
** Synopsis:    Initializes the KeyFile context
**
** Arguments:   argv[0] -- Shared Variable name holding the context. Can be "NULL"
**              argv[1] -- Optional OEM Context Variable
**
** Returns:     DRM_SUCCESS
********************************************************************************************/
DRM_RESULT DRM_CALL Test_KF_Initialize( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT           dr         = DRM_SUCCESS;
    DRM_KEYFILE_CONTEXT *pKFContext = NULL;
    DRM_VOID *pOEMContext = NULL;

    if ( argc    >  0
      && argv[0] != NULL )
    {
        MAP_TO_GLOBAL_TEST_VAR( pKFContext, argv[0] );
    }

    if ( argc    >  1
      && argv[1] != NULL )
    {
        MAP_TO_GLOBAL_TEST_VAR( pOEMContext, argv[1] );
    }

PREFAST_PUSH_DISABLE_EXPLAINED( __WARNING_INVALID_PARAM_VALUE_1, "Test code is potentially passing invalid parameters intentionally." );
    ChkDR( DRM_KF_Initialize( pOEMContext , pKFContext ) );
PREFAST_POP

ErrorExit:
    return dr;
}

/********************************************************************************************
** Function:    Test_KF_Uninitialize
**
** Synopsis:    Uninitializes the KeyFile context
**
** Arguments:   argv[0] -- Shared Variable name holding the context. Can be "NULL"
**
** Returns:     DRM_SUCCESS
********************************************************************************************/
DRM_RESULT DRM_CALL Test_KF_Uninitialize( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT           dr         = DRM_SUCCESS;
    DRM_KEYFILE_CONTEXT *pKFContext = NULL;

    if ( argc    >  0
      && argv[0] != NULL )
    {
        MAP_TO_GLOBAL_TEST_VAR( pKFContext, argv[0] );
    }

PREFAST_PUSH_DISABLE_EXPLAINED( __WARNING_INVALID_PARAM_VALUE_1, "Test code is potentially passing invalid parameters intentionally." );
    ChkDR( DRM_KF_Uninitialize( pKFContext ) );
PREFAST_POP

ErrorExit:
    return dr;
}

/********************************************************************************************
** Function:    Test_KF_OpenKeyFile
**
** Synopsis:    Opens the KeyFile into the given KeyFile context
**
** Arguments:   argv[0] -- Shared Variable name holding the context. Can be "NULL"
**              argv[1] -- Access mode to use when opening the file.  Can be "READ" or "WRITE".
**                         Default value is READ | WRITE
**
** Returns:     DRM_SUCCESS
********************************************************************************************/
DRM_RESULT DRM_CALL Test_KF_OpenKeyFile( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT           dr           = DRM_SUCCESS;
    DRM_KEYFILE_CONTEXT *pKFContext   = NULL;
    DRM_DWORD            dwAccessMode = OEM_GENERIC_READ | OEM_GENERIC_WRITE;

    if ( argc    >  0
      && argv[0] != NULL )
    {
        MAP_TO_GLOBAL_TEST_VAR( pKFContext, argv[0] );
    }

    if ( argc >  1 && argv[1] != NULL )
    {
        if (   DRMCRT_strlen( argv[1] ) >= 4
            && DRMCRT_strncmp( argv[1], "READ", 5) == 0 )
        {
            dwAccessMode = OEM_GENERIC_READ;
        }
        else if (   DRMCRT_strlen( argv[1] ) >= 5
                 && DRMCRT_strncmp( argv[1], "WRITE", 5) == 0 )
        {
            dwAccessMode = OEM_GENERIC_READ | OEM_GENERIC_WRITE;
        }
        else
        {
            ChkDR( DRM_E_INVALIDARG );
        }
    }

    ChkDR( DRM_KF_Open( pKFContext, TRUE, dwAccessMode ) );

ErrorExit:
    return dr;
}

/********************************************************************************************
** Function:    Test_KF_CloseKeyFile
**
** Synopsis:    Closes the opened KeyFile
**
** Arguments:   argv[0] -- Shared Variable name holding the context. Can be "NULL"
**
** Returns:     DRM_SUCCESS
********************************************************************************************/
DRM_RESULT DRM_CALL Test_KF_CloseKeyFile( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT           dr         = DRM_SUCCESS;
    DRM_KEYFILE_CONTEXT *pKFContext = NULL;

    if ( argc    >  0
      && argv[0] != NULL )
    {
        MAP_TO_GLOBAL_TEST_VAR( pKFContext, argv[0] );
    }

PREFAST_PUSH_DISABLE_EXPLAINED( __WARNING_INVALID_PARAM_VALUE_1, "Test code is potentially passing invalid parameters intentionally." );
    ChkDR( DRM_KF_Close( pKFContext ) );
PREFAST_POP

ErrorExit:
    return dr;
}


/********************************************************************************************
** Function:    Test_KF_LoadKeyFile
**
** Synopsis:    calls DRM_KF_LoadFromFile on the specified context.
**
** Arguments:   argv[0] -- Shared Variable name holding the context. Can be "NULL"
**
** Returns:     DRM_SUCCESS
********************************************************************************************/
DRM_RESULT DRM_CALL Test_KF_LoadKeyFile( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT           dr         = DRM_SUCCESS;
    DRM_KEYFILE_CONTEXT *pKFContext = NULL;

    if ( argc    >  0
      && argv[0] != NULL )
    {
        MAP_TO_GLOBAL_TEST_VAR( pKFContext, argv[0] );
    }

PREFAST_PUSH_DISABLE_EXPLAINED( __WARNING_INVALID_PARAM_VALUE_1, "Test code is potentially passing invalid parameters intentionally." );
    ChkDR( DRM_KF_LoadFromFile( pKFContext, TRUE ) );
PREFAST_POP

ErrorExit:
    return dr;
}

/********************************************************************************************
** Function:    Test_KF_StoreKeyFile
**
** Synopsis:    calls DRM_KF_StoreToFile on the specified context.
**
** Arguments:   argv[0] -- Shared Variable name holding the context. Can be "NULL"
**
** Returns:     DRM_SUCCESS
********************************************************************************************/
DRM_RESULT DRM_CALL Test_KF_StoreKeyFile( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT           dr         = DRM_SUCCESS;
    DRM_KEYFILE_CONTEXT *pKFContext = NULL;

    if ( argc    >  0
      && argv[0] != NULL )
    {
        MAP_TO_GLOBAL_TEST_VAR( pKFContext, argv[0] );
    }

PREFAST_PUSH_DISABLE_EXPLAINED( __WARNING_INVALID_PARAM_VALUE_1, "Test code is potentially passing invalid parameters intentionally." );
    ChkDR( DRM_KF_StoreToFile( pKFContext ) );
PREFAST_POP

ErrorExit:
    return dr;
}
/********************************************************************************************
** Function:    Test_KF_SetCertificate
**
** Synopsis:    calls DRM_KF_SetCertificate on the specified context.
**
** Arguments:   argv[0] -- Shared Variable name holding the context. Can be "NULL"
**              argv[1] -- Cert Type to pass. See the declaraion of g_rgoKFCertTypeMap for values.
**              argv[2] -- Shared Variable holding the bytes for the cert to store.
**              argv[3] -- Optional, if "INVALID" will set cbCert passed to DRM_KF_SetCertificate to 0
**              argv[4] -- Optional, if "INVALID" will set pbCert passed to DRM_KF_SetCertificate to NULL
**
** Returns:     DRM_SUCCESS
********************************************************************************************/
DRM_RESULT DRM_CALL Test_KF_SetCertificate( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT            dr            = DRM_SUCCESS;
    DRM_KEYFILE_CONTEXT  *pKFContext    = NULL;
    DRM_DWORD             dwCertType    = 0;
    DRM_BYTE             *pbCert        = NULL;
    DRM_DWORD             cbCert        = 0;
    DRM_DWORD            *pcbCert       = NULL;
    DRM_ANSI_CONST_STRING dastrCertType = EMPTY_DRM_STRING;


    if (( argc > 0) && (argv[0] != NULL ))
    {
        MAP_TO_GLOBAL_TEST_VAR( pKFContext, argv[0] );
    }

    if (( argc > 1) && (argv[1] != NULL ))
    {
        dastrCertType.pszString = argv[1];
        ChkDR( DRM_SizeTToDWord( DRMCRT_strlen( argv[1] ), &dastrCertType.cchString ) );
        ChkDR( TST_MAP_GetDWORD( g_rgoKFCertTypeMap, &dastrCertType, &dwCertType ) );
    }

    if ( ( argc > 2) && (argv[2] != NULL ))
    {
        MAP_TO_GLOBAL_TEST_VAR_EX( pbCert, pcbCert, argv[2] );
    }
    else
    {
        pcbCert = &cbCert;
    }

    if ( argc > 3
     && argv[3] != NULL
     && DRMCRT_strlen( argv[3] ) >= 6
     && DRMCRT_strncmp( argv[3], "INVALID", 6) == 0 )
    {
        pbCert = NULL;
    }

    if ( argc > 4
      && argv[4] != NULL
      && DRMCRT_strlen( argv[4] ) >= 6
      && DRMCRT_strncmp( argv[4], "INVALID", 6) == 0 )
    {
        pcbCert = &cbCert;
    }

PREFAST_PUSH_DISABLE_EXPLAINED( __WARNING_INVALID_PARAM_VALUE_1, "Test code is potentially passing invalid parameters intentionally." );
    ChkDR( DRM_KF_SetCertificate( pKFContext, ( DRM_KF_CERT_TYPE ) dwCertType, pbCert, *pcbCert ) );
PREFAST_POP

ErrorExit:
    return dr;
}

/********************************************************************************************
** Function:    Test_KF_GetCertificate
**
** Synopsis:    calls DRM_KF_GetCertificate on the specified context.
**
** Arguments:   argv[0] -- Shared Variable name holding the context. Can be "NULL"
**              argv[1] -- Cert Type to pass. See the declaraion of g_rgoKFCertTypeMap for values.
**              argv[2] -- Shared variable holding the cert for comparison to the retrieved cert.
**
** Returns:     DRM_SUCCESS
**              DRM_E_TEST_UNEXPECTED_OUTPUT -- if the retrieved cert doesn't match teh stored cert.
********************************************************************************************/
DRM_RESULT DRM_CALL Test_KF_GetCertificate( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT            dr            = DRM_SUCCESS;
    DRM_KEYFILE_CONTEXT  *pKFContext    = NULL;
    DRM_DWORD             dwCertType    = 0;
    DRM_BYTE             *pbCert        = NULL;
    DRM_DWORD             cbCert        = 0;
    DRM_ANSI_CONST_STRING dastrCertType = EMPTY_DRM_STRING;


    ChkArg( argc > 2 );
    ChkArg( argv[1] != NULL ); /* Cert Type must be given. */

    if ( argv[0] != NULL )
    {
        MAP_TO_GLOBAL_TEST_VAR( pKFContext, argv[0] );
    }

    dastrCertType.pszString = argv[1];
    ChkDR( DRM_SizeTToDWord( DRMCRT_strlen( argv[1] ), &dastrCertType.cchString ) );

    ChkDR( TST_MAP_GetDWORD( g_rgoKFCertTypeMap, &dastrCertType, &dwCertType ) );

    ChkDR( DRM_KF_GetCertificate( pKFContext, ( DRM_KF_CERT_TYPE ) dwCertType, &pbCert, &cbCert ) );

    if( argv[2] != NULL )
    {
        DRM_BYTE  *pbCompareCert  = NULL;
        DRM_DWORD *pcbCompareCert = NULL;
        MAP_TO_GLOBAL_TEST_VAR_EX( pbCompareCert, pcbCompareCert, argv[2] );

        ChkBOOL ( cbCert == *pcbCompareCert
               && DRMCRT_memcmp( pbCompareCert, pbCert, cbCert ) == 0,
                  DRM_E_TEST_KEYFILE_VERIFICATION_FAILURE );
    }

ErrorExit:
    return dr;
}

/********************************************************************************************
** Function:    Test_Oem_KeyFile_EncryptKey
**
** Synopsis:    calls DRM_KF_SetCertificate on the specified context.
**
** Arguments:   argv[0] -- Shared Variable name for the Encrypted Key Struct.
**              argv[1] -- Shared Variable holding the clear key
**              argv[2] -- name of the Encryption Type to use. see g_rgoKFKeyEncryptionTypeMap for values.
**              argv[3] -- Shared Variable to hold the buffer for the encrypted key bytes.
**                         This maps to poEncKey->pbEncryptedKey.
**
** Returns:     DRM_SUCCESS
********************************************************************************************/
DRM_RESULT DRM_CALL Test_Oem_KeyFile_EncryptKey( long argc, __in_ecount( argc ) char ** argv )
{
    DRM_RESULT            dr           = DRM_SUCCESS;
    DRM_ENCRYPTED_KEY    *poEncKey     = NULL;
    DRM_DWORD             dwEncType    = 0;
    DRM_BYTE             *pbClearKey   = NULL;
    DRM_DWORD            *pcbClearKey  = NULL;
    DRM_ANSI_CONST_STRING dastrEncType = EMPTY_DRM_STRING;
    /* In PK the encryption type does not matter, in WM7 we check that corresponding encryption happens with corresponding key */
    DRM_DEVICE_KEY_TYPE   eKeyType     = DRM_DKT_WMDRMPD_DEVICE;


    ChkArg( argc >= 3 );
    ChkArg( argv[0] != NULL );
    ChkArg( argv[1] != NULL );
    ChkArg( argv[2] != NULL );

    MAP_TO_GLOBAL_TEST_VAR_Alloc( poEncKey, argv[0], SIZEOF( DRM_ENCRYPTED_KEY ) );

    dastrEncType.pszString = argv[2];
    ChkDR( DRM_SizeTToDWord( DRMCRT_strlen( argv[2] ), &dastrEncType.cchString ) );

    ChkDR( TST_MAP_GetDWORD( g_rgoKFKeyEncryptionTypeMap, &dastrEncType, &dwEncType ) );


    MAP_TO_GLOBAL_TEST_VAR_EX( pbClearKey, pcbClearKey, argv[1] );

    if( dwEncType == KF_DECRYPTION_TYPE_CLEAR )
    {
        DRM_WORD wTemp = 0;

        poEncKey->pbEncryptedKey = pbClearKey;
        poEncKey->cbEncryptedKey = *pcbClearKey;
        poEncKey->wKeypairDecryptionMethod = KF_DECRYPTION_TYPE_CLEAR;
        ChkDR( DRM_DWordToWord( *pcbClearKey, &wTemp ) );
        ChkDR( DRM_WordMult( wTemp, BITS_PER_BYTE, &poEncKey->wKeySize ) );
    }
    else
    {
        dr = DRM_BBX_KF_Legacy_EncryptKey( NULL, pbClearKey, *pcbClearKey, eKeyType, poEncKey );
        if ( dr      == DRM_E_BUFFERTOOSMALL
          && argc    >  3
          && argv[3] != NULL )
        {
            MAP_TO_GLOBAL_TEST_VAR_Alloc( poEncKey->pbEncryptedKey, argv[3], poEncKey->cbEncryptedKey );
            ChkDR( DRM_BBX_KF_Legacy_EncryptKey( NULL, pbClearKey, *pcbClearKey, eKeyType, poEncKey ) );
        }
        ChkDR( dr );
    }
ErrorExit:
    return dr;
}

/********************************************************************************************
** Function:    Test_Oem_KeyFile_DecryptKey
**
** Synopsis:    Decrypts the key in the DRM_ENCRYPTED_KEY Struct in argv[0]
**
** Arguments:   argv[0] -- Shared Variable name for the Encrypted Key Struct.
**              argv[1] -- Shared Variable holding the original
**              argv[2] -- Expected Key Encryption Type. see g_rgoKFKeyEncryptionTypeMap for values.
**
** Returns:     DRM_SUCCESS
**              DRM_E_TEST_UNEXPECTED_OUTPUT
**                  -- the decrypted key is not the same as the expected
**                  -- the decrypted key Type is not the same as the expected
**                  -- the decrypted key size is not the same as the expected
********************************************************************************************/
DRM_RESULT DRM_CALL Test_Oem_KeyFile_DecryptKey( long argc, __in_ecount( argc ) char ** argv )
{
    DRM_RESULT            dr           = DRM_SUCCESS;
    DRM_ENCRYPTED_KEY    *poEncKey     = NULL;
    DRM_DWORD             dwDecType    = 0;
    DRM_BYTE             *pbClearKeyExpected   = NULL;
    DRM_DWORD            *pcbClearKeyExpected  = NULL;
    DRM_BYTE             *pbClearKey   = NULL;
    DRM_DWORD             cbClearKey   = 0;
    DRM_ANSI_CONST_STRING dastrDecType = EMPTY_DRM_STRING;


    ChkArg( argc >= 3 );
    ChkArg( argv[0] != NULL );
    ChkArg( argv[1] != NULL );
    ChkArg( argv[2] != NULL );

    MAP_TO_GLOBAL_TEST_VAR( poEncKey, argv[0] );

    dastrDecType.pszString = argv[2];
    ChkDR( DRM_SizeTToDWord( DRMCRT_strlen( argv[2] ), &dastrDecType.cchString ) );

    ChkDR( TST_MAP_GetDWORD( g_rgoKFKeyEncryptionTypeMap, &dastrDecType, &dwDecType ) );

    MAP_TO_GLOBAL_TEST_VAR_EX( pbClearKeyExpected, pcbClearKeyExpected, argv[1] );

    ChkBOOL( poEncKey->wKeypairDecryptionMethod == (DRM_WORD) dwDecType, DRM_E_TEST_UNEXPECTED_OUTPUT );


    /* keytype parameter is ignored here */
    dr = DRM_BBX_KF_Legacy_DecryptKey( NULL, poEncKey, DRM_DKT_WMDRMPD_DEVICE, NULL, &cbClearKey );

    if ( dr == DRM_E_BUFFERTOOSMALL )
    {
        if ( cbClearKey != *pcbClearKeyExpected )
        {
            Log( "", "Retrieved key is %d bytes. Expected %d bytes.", cbClearKey, *pcbClearKeyExpected );
            ChkDR( DRM_E_TEST_UNEXPECTED_OUTPUT );
        }
        ChkMem( pbClearKey = ( DRM_BYTE * ) Oem_MemAlloc( cbClearKey ) );
        ChkDR( DRM_BBX_KF_Legacy_DecryptKey( NULL, poEncKey, DRM_DKT_WMDRMPD_DEVICE, pbClearKey, &cbClearKey ) );
        ChkBOOL( MEMCMP( pbClearKey, pbClearKeyExpected, cbClearKey ) == 0, DRM_E_TEST_UNEXPECTED_OUTPUT );
    }
    else
    {
        ChkDR( dr );
    }
ErrorExit:
    SAFE_OEM_FREE( pbClearKey );
    return dr;
}

/********************************************************************************************
** Function:    Test_KF_SetPrivateKey
**
** Synopsis:    Calls DRM_KF_SetPrivateKey with the DRM_ENCRYPTED_KEY struct in argv[3]
**
** Arguments:   argv[0] -- Key Type. See g_rgoKFKeyTypeMap for values.
**              argv[1] -- Shared Variable holding the KF Context
**              argv[2] -- Shared Variable holding the pub key.
**              argv[3] -- Shared Variable name for the Encrypted Key Struct.
**              argv[4] -- Optional, if "INVALID" will set pbPubKey passed to DRM_KF_SetPrivateKey to NULL
**              argv[5] -- Optional, if "INVALID" will set cbPubKey passed to DRM_KF_SetPrivateKey to 0
**              argv[6] -- Optional, if "INVALID" will set pEncKey passed to DRM_KF_SetPrivateKey to NULL
**
** Returns:     DRM_SUCCESS
********************************************************************************************/
DRM_RESULT DRM_CALL Test_KF_SetPrivateKey( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT            dr           = DRM_SUCCESS;
    DRM_KEYFILE_CONTEXT  *pKFContext   = NULL;
    DRM_ENCRYPTED_KEY    *pEncKey      = NULL;
    DRM_BYTE             *pbPubKey     = NULL;
    DRM_DWORD             cbPubKey     = 0;
    DRM_DWORD            *pcbPubKey    = NULL;
    DRM_ANSI_CONST_STRING dastrKeyType = EMPTY_DRM_STRING;
    DRM_DWORD             dwKeyType    = 0;

    ChkArg( argc    >  0 );
    ChkArg( argv[0] != NULL ); /* must specify the keytype */

    pcbPubKey = &cbPubKey;

    dastrKeyType.pszString = argv[0];
    ChkDR( DRM_SizeTToDWord( DRMCRT_strlen( argv[0] ), &dastrKeyType.cchString ) );

    ChkDR( TST_MAP_GetDWORD( g_rgoKFKeyTypeMap, &dastrKeyType, &dwKeyType ) );

    if ( argc    >  1
      && argv[1] != NULL )
    {
        MAP_TO_GLOBAL_TEST_VAR( pKFContext, argv[1] );
    }

    if ( argc    >  2
      && argv[2] != NULL )
    {
        MAP_TO_GLOBAL_TEST_VAR_EX( pbPubKey, pcbPubKey, argv[2] );
    }

    if ( argc    >  3
      && argv[3] != NULL )
    {
        MAP_TO_GLOBAL_TEST_VAR( pEncKey, argv[3] );
    }

    if ( argc > 4
      && argv[4] != NULL
      && DRMCRT_strlen( argv[4] ) >= 6
      && DRMCRT_strncmp( argv[4], "INVALID", 6) == 0 )
    {
        pbPubKey = NULL;
    }

    if ( argc > 5
      && argv[5] != NULL
      && DRMCRT_strlen( argv[5] ) >= 6
      && DRMCRT_strncmp( argv[5], "INVALID", 6) == 0 )
    {
        pcbPubKey = &cbPubKey;
    }

    if ( argc > 6
      && argv[6] != NULL
      && DRMCRT_strlen( argv[6] ) >= 6
      && DRMCRT_strncmp( argv[6], "INVALID", 6) == 0 )
    {
        pEncKey = NULL;
    }

PREFAST_PUSH_DISABLE_EXPLAINED( __WARNING_INVALID_PARAM_VALUE_1, "Test code is potentially passing invalid parameters intentionally." );
    ChkDR( DRM_KF_SetPrivateKey( pKFContext, ( DRM_KF_KEY_TYPE ) dwKeyType, pbPubKey, *pcbPubKey, pEncKey ) );
PREFAST_POP

ErrorExit:
    return dr;
}

/********************************************************************************************
** Function:    Test_KF_GetPrivateKey
**
** Synopsis:    Calls DRM_KF_GetPrivateKey with the Pub Key struct in argv[2]
**
** Arguments:   argv[0] -- Key Type. See g_rgoKFKeyTypeMap for values.
**              argv[1] -- Shared Variable holding the KF Context
**              argv[2] -- Shared Variable holding the pub key to find.
**              argv[3] -- Shared Variable name for the Encrypted Key Struct.
**              argv[4] -- Shared Variable name for an Encrypted Key to compare against.
**
** Returns:     DRM_SUCCESS
**              DRM_E_TEST_KEYFILE_VERIFICATION_FAILURE
**                  -- the retrieved encrypted key does not match the compared encrypted.
********************************************************************************************/
DRM_RESULT DRM_CALL Test_KF_GetPrivateKey( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT            dr           = DRM_SUCCESS;
    DRM_KEYFILE_CONTEXT  *pKFContext   = NULL;
    DRM_ENCRYPTED_KEY    *pEncKey      = NULL;
    DRM_BYTE             *pbPubKey     = NULL;
    DRM_DWORD             cbPubKey     = 0;
    DRM_DWORD            *pcbPubKey    = NULL;
    DRM_ANSI_CONST_STRING dastrKeyType = EMPTY_DRM_STRING;
    DRM_DWORD             dwKeyType    = 0;

    ChkArg( argc    >  0 );
    ChkArg( argv[0] != NULL ); /* must specify the keytype */

    pcbPubKey = &cbPubKey;

    dastrKeyType.pszString = argv[0];
    ChkDR( DRM_SizeTToDWord( DRMCRT_strlen( argv[0] ), &dastrKeyType.cchString ) );

    ChkDR( TST_MAP_GetDWORD( g_rgoKFKeyTypeMap, &dastrKeyType, &dwKeyType ) );

    if ( argc    >  1
      && argv[1] != NULL )
    {
        MAP_TO_GLOBAL_TEST_VAR( pKFContext, argv[1] );
    }

    if ( argc    >  2
      && argv[2] != NULL )
    {
        MAP_TO_GLOBAL_TEST_VAR_EX( pbPubKey, pcbPubKey, argv[2] );
    }

    if ( argc    >  3
      && argv[3] != NULL )
    {
        MAP_TO_GLOBAL_TEST_VAR_Alloc( pEncKey, argv[3], SIZEOF( DRM_ENCRYPTED_KEY ) );
    }

PREFAST_PUSH_DISABLE_EXPLAINED( __WARNING_INVALID_PARAM_VALUE_1, "Test code is potentially passing invalid parameters intentionally." );
    ChkDR( DRM_KF_GetPrivateKey( pKFContext, ( DRM_KF_KEY_TYPE ) dwKeyType, pbPubKey, *pcbPubKey, pEncKey ) );
PREFAST_POP

    if( argc > 4 && argv[4] != NULL )
    {
        DRM_ENCRYPTED_KEY    *pVerificationEncKey      = NULL;
        MAP_TO_GLOBAL_TEST_VAR( pVerificationEncKey, argv[3] );

        ChkBOOL( pVerificationEncKey->wKeypairDecryptionMethod == pEncKey->wKeypairDecryptionMethod, DRM_E_TEST_KEYFILE_VERIFICATION_FAILURE  );
        ChkBOOL( pVerificationEncKey->wKeySize == pEncKey->wKeySize, DRM_E_TEST_KEYFILE_VERIFICATION_FAILURE  );
        ChkBOOL( pVerificationEncKey->wReserved == pEncKey->wReserved, DRM_E_TEST_KEYFILE_VERIFICATION_FAILURE  );
        ChkBOOL ( ( pVerificationEncKey->cbEncryptedKey == pEncKey->cbEncryptedKey
                 && DRMCRT_memcmp( pVerificationEncKey->pbEncryptedKey, pEncKey->pbEncryptedKey, pEncKey->cbEncryptedKey ) == 0 ),
                DRM_E_TEST_KEYFILE_VERIFICATION_FAILURE );
    }


ErrorExit:
    return dr;
}

/********************************************************************************************
** Function:    Test_KF_VerifyContext
**
** Synopsis:    Verifies the KeyFile context.
**
** Arguments:   argv[0] -- Shared Variable name holding the context
**              argv[1] -- Context State to check for can be INIT, UNINIT, OPEN, CLOSED, KEY or CERT
**              argv[2] -- Either a KeyCipher or Cert Type to check for if KEY or CERT is used
**                      for argv[1]. If the context contains a cert or key that matches the type
**                      will return DRM_SUCCESS, otherwise DRM_E_TEST_KEYFILE_VERIFICATION_FAILURE
**
** Returns:     DRM_SUCCESS
**              DRM_E_TEST_KEYFILE_VERIFICATION_FAILURE
********************************************************************************************/
DRM_RESULT DRM_CALL Test_KF_VerifyContext( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT           dr         = DRM_SUCCESS;
    DRM_KEYFILE_CONTEXT *pKFContext = NULL;

    ChkArg( argc    >=  2 );
    ChkArg( argv[0] != NULL );
    ChkArg( argv[1] != NULL );

    MAP_TO_GLOBAL_TEST_VAR( pKFContext, argv[0] );

    if ( DRMCRT_strlen( argv[1] ) >= 4
      && DRMCRT_strncmp( argv[1], "INIT", 4) == 0 )
    {
        ChkBOOL( pKFContext->fLoaded == FALSE , DRM_E_TEST_KEYFILE_VERIFICATION_FAILURE );
        ChkBOOL( pKFContext->fInited == TRUE , DRM_E_TEST_KEYFILE_VERIFICATION_FAILURE );
        ChkBOOL( pKFContext->hKeyfile == OEM_INVALID_HANDLE_VALUE , DRM_E_TEST_KEYFILE_VERIFICATION_FAILURE );
    }
    else if ( DRMCRT_strlen( argv[1] ) >= 6
           && DRMCRT_strncmp( argv[1], "UNINIT", 6) == 0 )
    {
        ChkBOOL( pKFContext->fLoaded == 0 , DRM_E_TEST_KEYFILE_VERIFICATION_FAILURE );
        ChkBOOL( pKFContext->fInited == 0 , DRM_E_TEST_KEYFILE_VERIFICATION_FAILURE );
        ChkBOOL( pKFContext->hKeyfile == 0 , DRM_E_TEST_KEYFILE_VERIFICATION_FAILURE );
    }
    else if ( DRMCRT_strlen( argv[1] ) >= 4
           && DRMCRT_strncmp( argv[1], "OPEN", 4) == 0 )
    {
        ChkBOOL( pKFContext->hKeyfile != OEM_INVALID_HANDLE_VALUE , DRM_E_TEST_KEYFILE_VERIFICATION_FAILURE );
    }
    else if ( DRMCRT_strlen( argv[1] ) >= 6
           && DRMCRT_strncmp( argv[1], "CLOSED", 6) == 0 )
    {
        ChkBOOL( pKFContext->hKeyfile == OEM_INVALID_HANDLE_VALUE , DRM_E_TEST_KEYFILE_VERIFICATION_FAILURE );
    }
    else if ( DRMCRT_strlen( argv[1] ) >= 6
           && DRMCRT_strncmp( argv[1], "LOADED", 6) == 0 )
    {
        ChkBOOL( pKFContext->fLoaded == TRUE , DRM_E_TEST_KEYFILE_VERIFICATION_FAILURE );
    }
    else if ( DRMCRT_strlen( argv[1] ) >= 4
           && DRMCRT_strncmp( argv[1], "CERT", 4) == 0 )
    {
        const KF_CERTIFICATE_CHAIN_OBJECT_V2 *pCertChain = NULL;
        pCertChain = pKFContext->keyfile.pCertChainV2;

        ChkBOOL( pCertChain != NULL, DRM_E_TEST_KEYFILE_VERIFICATION_FAILURE );
        ChkBOOL( pCertChain->fValid , DRM_E_TEST_KEYFILE_VERIFICATION_FAILURE );

        if( argc > 2 && argv[2] != NULL )
        {
            DRM_DWORD             dwCertType    = 0;
            DRM_ANSI_CONST_STRING dastrCertType = EMPTY_DRM_STRING;
            DRM_KEYFILE_CERT_TYPE CertType = KF_CERT_TYPE_INVALID;

            dastrCertType.pszString = argv[2];
            ChkDR( DRM_SizeTToDWord( DRMCRT_strlen( argv[2] ), &dastrCertType.cchString ) );
            ChkDR( TST_MAP_GetDWORD( g_rgoKFCertTypeMap, &dastrCertType, &dwCertType ) );
            CertType = MapApiCertTypeToFormatCertType( (DRM_KF_CERT_TYPE)dwCertType );
            dr = DRM_E_TEST_KEYFILE_VERIFICATION_FAILURE;
            while( pCertChain != NULL && pCertChain->fValid )
            {
                if( pCertChain->dwCertificateType == (DRM_DWORD)CertType )
                {
                    dr = DRM_SUCCESS;
                    goto ErrorExit;
                }
                pCertChain = pCertChain->pNext;
            }

            ChkDR(dr);
        }

    }
    else if ( DRMCRT_strlen( argv[1] ) >= 3
           && DRMCRT_strncmp( argv[1], "KEY", 3) == 0 )
    {
        const KF_KEYPAIR_V2_OBJECT *pKeyPair = NULL;
        pKeyPair = pKFContext->keyfile.KeyHistoryV2.pKeyPairHeadv2;

        ChkBOOL( pKeyPair != NULL, DRM_E_TEST_KEYFILE_VERIFICATION_FAILURE );
        ChkBOOL( pKeyPair->fValid, DRM_E_TEST_KEYFILE_VERIFICATION_FAILURE );

        if( ( argc > 2 ) && ( argv[2] != NULL ))
        {
            DRM_DWORD             dwKeyType    = 0;
            DRM_ANSI_CONST_STRING dastrKeyType = EMPTY_DRM_STRING;
            DRM_KEYFILE_ASYMMETRIC_KEY_TYPE KeyType = KF_ASYMM_KEY_TYPE_INVALID;

            dastrKeyType.pszString = argv[0];
            ChkDR( DRM_SizeTToDWord( DRMCRT_strlen( argv[0] ), &dastrKeyType.cchString ) );
            ChkDR( TST_MAP_GetDWORD( g_rgoKFKeyTypeMap, &dastrKeyType, &dwKeyType ) );

            KeyType = MapApiKeyTypeToFormatKeyType((DRM_KF_KEY_TYPE)dwKeyType);
            dr = DRM_E_TEST_KEYFILE_VERIFICATION_FAILURE;
            while( pKeyPair != NULL && pKeyPair ->fValid )
            {
                if( pKeyPair->wCipherType == (DRM_DWORD)KeyType )
                {
                    dr = DRM_SUCCESS;
                    goto ErrorExit;
                }
                pKeyPair = pKeyPair->pNext;
            }

            ChkDR(dr);
        }

    }
ErrorExit:
    return dr;
}

BEGIN_TEST_API_LIST( KeyFile )
    API_ENTRY(Test_KF_CreateContext)
    API_ENTRY(Test_KF_FreeContext)
    API_ENTRY(Test_KF_Initialize)
    API_ENTRY(Test_KF_Uninitialize)
    API_ENTRY(Test_KF_OpenKeyFile)
    API_ENTRY(Test_KF_CloseKeyFile)
    API_ENTRY(Test_KF_LoadKeyFile)
    API_ENTRY(Test_KF_StoreKeyFile)
    API_ENTRY(Test_KF_SetCertificate)
    API_ENTRY(Test_KF_GetCertificate)
    API_ENTRY(Test_KF_SetPrivateKey)
    API_ENTRY(Test_KF_GetPrivateKey)
    API_ENTRY(Test_Oem_KeyFile_EncryptKey)
    API_ENTRY(Test_Oem_KeyFile_DecryptKey)
    API_ENTRY(Test_KF_VerifyContext)
    API_ENTRY(Test_KF_ReplaceKeyFile)
END_TEST_API_LIST

EXIT_PKTEST_NAMESPACE_CODE
