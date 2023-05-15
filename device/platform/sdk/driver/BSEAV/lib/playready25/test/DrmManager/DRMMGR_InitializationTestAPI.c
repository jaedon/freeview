/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdafx.h>
#include <DRMManagerTestAPI.h>
#include <drmutf.h>
#include <drmmathsafe.h>

ENTER_PKTEST_NAMESPACE_CODE;

/*****************************************************************************
** Function:    _ResetOpaqueBuffer
**
** Synopsis:    Reset the opaque buffer in the AppContext to the
**              default size, if it has been previously resized larger.
**              If resetting, current buffer contents are not maintained;
**              the smaller buffer will be empty.
**
** Arguments:   f_poAppContext: a pointer to the App Context
**
** Returns:     DRM_SUCCESS - on success
**
** Note:
**
******************************************************************************/
static DRM_RESULT DRM_CALL _ResetOpaqueBuffer(
__in       DRM_APP_CONTEXT     *f_poAppContext
)
{
    DRM_RESULT                dr                   = DRM_SUCCESS;
    DRM_APP_CONTEXT_INTERNAL *poAppContextInternal = ( DRM_APP_CONTEXT_INTERNAL * )f_poAppContext;
    DRM_BYTE                 *pbNewBuffer          = NULL;
    const DRM_DWORD           cbDefaultBufferSize  = TEST_DRM_BUFFER_SIZE;


    ChkArg( f_poAppContext != NULL );

    if ( ( poAppContextInternal->pbOpaqueBuffer != NULL ) &&
         ( poAppContextInternal->cbOpaqueBuffer > cbDefaultBufferSize ) )
    {
        /*
        **  The test has resized the opaque buffer to a larger size, reset it.
        */
        ChkDR( DRM_TST_ReallocTestVar( DRMBUFFER_VAR, cbDefaultBufferSize, (DRM_VOID **)&pbNewBuffer, NULL ) );

        poAppContextInternal->pbOpaqueBuffer = pbNewBuffer;
        poAppContextInternal->cbOpaqueBuffer = cbDefaultBufferSize;
        poAppContextInternal->pbDRMHeaderData = pbNewBuffer;
        poAppContextInternal->cbDRMHeaderData = cbDefaultBufferSize - DWORD_ALIGNED_SIZE( DRM_MAX_LICENSESIZE );
        poAppContextInternal->pbDRMLicense = pbNewBuffer + cbDefaultBufferSize - DWORD_ALIGNED_SIZE( DRM_MAX_LICENSESIZE );
        poAppContextInternal->cbDRMLicense = DWORD_ALIGNED_SIZE( DRM_MAX_LICENSESIZE );
    }

ErrorExit:
    return dr;
}


/*****************************************************************************
** Function:    TestManagerReinitialize
**
** Synopsis:    Can call Drm_Reinitialize or can zero out shared data buffers
**
** Arguments:   argv[0]: (optional) Indicates what should be reinitialized:
**                       "Drm_Reinitialize" to make the function call Drm_Reinitialize
**                       "Reset" to uninitialize shared test app context, decrypt
**                       context, response buffer, optionally the HDS, and optionally
**                       calls initialize again on the shared app context
**                       "AppContext" to call uninitialize on the shared app context and
**                       optionally can delete the HDS and/or call initialize on the
**                       shared app context.
**                       "DecryptContext" to zero out the decrypt context memory
**                       "HDS" to delete the hds
**                       "SharedResponse" to clear the shared test response buffer
**                       Default value is "Reset"
**              argv[1]: (optional) Flag indicating that the HDS should be deleted
**                       during "Reset" or "AppContext" calls.  Default is FALSE.
**              argv[2]: (optional) Flag indicating that initialize should be called after
**                       uninitializing the app context and optionally deleting the HDS
**                       during "Reset" or "AppContext" calls.  Default is TRUE.
**
** Returns:     DRM_SUCCESS                  - on success
**
** Note:
**
******************************************************************************/
DRM_RESULT DRM_CALL TestManagerReinitialize( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT           dr                 = DRM_SUCCESS;
    DRM_BOOL             fDoReset           = TRUE;
    DRM_BOOL             fKnownArg          = FALSE;
    DRM_APP_CONTEXT     *poManagerContext   = NULL;
    DRM_BYTE            *pbOpaqueBuffer     = NULL;
    DRM_DWORD            cbOpaqueBuffer     = 0;
    DRM_CONST_STRING    *pwszDeviceStore    = NULL;
    DRM_DECRYPT_CONTEXT *poDecryptContext   = NULL;
    DRM_BOOL             fDeleteHDS         = FALSE;
    DRM_BOOL             fInitAppContext    = TRUE;
    DRM_BYTE            *pbRevocationBuffer = NULL;

    GET_SHARED_APP_CONTEXT( poManagerContext );
    GET_SHARED_DEVICE_STORE( pwszDeviceStore );
    MAP_TO_GLOBAL_TEST_VAR( poDecryptContext, DECRYPT_CONTEXT_VAR );

    if( argc >= 1 && argv[0] != NULL )
    {
        if ( 0 != strncmp( argv[0], "Reset", 5 ) )
        {
            fDoReset = FALSE;
        }
        else
        {
            fKnownArg = TRUE;
        }
    }
    if( argc >= 2 && argv[1] != NULL )
    {
        ChkDR( StringToBool( argv[1], &fDeleteHDS ) );
    }
    if( argc >= 3 && argv[2] != NULL )
    {
        ChkDR( StringToBool( argv[2], &fInitAppContext ) );
    }

    if( fDoReset || 0 == strncmp( argv[0], "AppContext", 10 ) )
    {
        fKnownArg = TRUE;

        /*
        ** Reset the shared App Context
        */
        ChkDR( TestScanDevicePrivateKey( poManagerContext ) );

        Drm_Uninitialize( poManagerContext );

        ChkDR( _ResetOpaqueBuffer( poManagerContext ) );

        ChkDR( Drm_GetOpaqueBuffer( poManagerContext, &pbOpaqueBuffer, &cbOpaqueBuffer ) );
        if( pbOpaqueBuffer == NULL )
        {
            GET_SHARED_DRMBUFFER( pbOpaqueBuffer );
        }
        MEMSET( poManagerContext, 0, SIZEOF( DRM_APP_CONTEXT ) );
        MEMSET( pbOpaqueBuffer, 0, TEST_DRM_BUFFER_SIZE );

        if( fDeleteHDS )
        {
            /*
            ** Delete the app context
            */
            RemoveDRMFile( RMFILE_STORE );
        }
        if( fInitAppContext )
        {
            /*
            ** Call initialize using the shared device store name.
            ** For multiple store case, the previous store should be saved to a different location or the shared
            ** store name should be changed before calling this function
            */
            ChkDR( Drm_Initialize( poManagerContext, NULL, pbOpaqueBuffer, TEST_DRM_BUFFER_SIZE, pwszDeviceStore ) );
            if( DRM_REVOCATION_IsRevocationSupported() )
            {
                MAP_TO_GLOBAL_TEST_VAR( pbRevocationBuffer, REVOCATION_BUFFER_VAR );
                ChkDR( Drm_Revocation_SetBuffer( poManagerContext, pbRevocationBuffer, REVOCATION_BUFFER_SIZE ) );
            }
        }
    }

    if( argc >= 1 && 0 == strncmp( argv[0], "HDS", 3 ) )
    {
        /*
        ** Delete saved HDS
        ** For full reset, this is deleted before initializing a new app context
        */
        fKnownArg = TRUE;
        RemoveDRMFile( RMFILE_STORE );
    }

    if( fDoReset || 0 == strncmp( argv[0], "DecryptContext", 14 )  )
    {
        fKnownArg = TRUE;

        /*
        ** Close and zero out shared decrypt context
        */
        Drm_Reader_Close( poDecryptContext );
    }

    if( fDoReset || 0 == strncmp( argv[0], "SharedResponse", 14 ) )
    {
        fKnownArg = TRUE;

        /*
        ** Free RESPONSE_VAR
        */
        dr = DRM_TST_FreeTestVar( RESPONSE_VAR );
        if ( dr == DRM_E_TEST_VARIABLE_NOTFOUND )
        {
            dr = DRM_SUCCESS;
        }
        else
        {
            ChkDR( dr );
        }
    }

    /*
    ** Sometimes we really want to call Drm_Reinitialize
    */
    if ( !fDoReset && 0 == strncmp( argv[0], "Drm_Reinitialize", 16 ) )
    {
        fKnownArg = TRUE;
        ChkDR( Drm_Reinitialize( poManagerContext ) );
    }

    /*
    ** Make sure to return an error if the arguments are unknown
    */
    ChkArg( fKnownArg );
ErrorExit:
    return dr;
}

/*****************************************************************************
** Function:    TestManagerInitialize
**
** Synopsis:
**
** Arguments:   argv[0]: AppContext, NULL or other
**              argv[1]: (optional) WMDRM-PD devcert file name
**              argv[2]: (optional) PR Group Cert
**              argv[3]: (optional) PR Group Private Key file. Must be included with argv[2].
**              argv[4]: (optional) WMDRM-PD Cert Template
**              argv[5]: (optional) WMDRM-PD Private Key file. Must be included with argv[4].
**              argv[6]: (optional) TRUE/non-NULL to avoid deleting the HDS/keyfile when changing model certs, or NULL
**                                  to delete HDS/keyfile (default).
**              argv[7]: (optional) Pregenerated keyfile
**
** Returns:     DRM_SUCCESS                  - on success
**
** Note: PostTestCase will delete the newly created certs. The default test certs
**       are set up in PreTestCase and test cases other test cases are not affected by
**       changing the group cert and template here.
**
******************************************************************************/
DRM_RESULT DRM_CALL TestManagerInitialize( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT        dr                          = DRM_SUCCESS;
    DRM_WCHAR         rgwcDevCert[DRM_MAX_PATH]   = { 0 };
    DRM_CONST_STRING  dstrDevCert                 = EMPTY_DRM_STRING;
    DRM_WCHAR         rgwcGroupCert[DRM_MAX_PATH] = { 0 };
    DRM_CONST_STRING  dstrGroupCert               = EMPTY_DRM_STRING;
    DRM_WCHAR         rgwcZGPriv[DRM_MAX_PATH]    = { 0 };
    DRM_CONST_STRING  dstrZGPriv                  = EMPTY_DRM_STRING;
    DRM_WCHAR         rgwcKeyfile[DRM_MAX_PATH]   = { 0 };
    DRM_CONST_STRING  dstrKeyfile                 = EMPTY_DRM_STRING;
    DRM_APP_CONTEXT  *poManagerContext            = NULL;
    DRM_BYTE         *pbOpaqueBuffer              = NULL;
    DRM_DWORD         cbOpaqueBuffer              = 0;
    DRM_CONST_STRING *pdstrDeviceStore            = NULL;

    ChkArg( argc >= 1 );

    GET_SHARED_APP_CONTEXT( poManagerContext );

    if ( argv[0] == NULL )
    {
        /*
        ** Initialize with NULL context
        */
        poManagerContext = NULL;
    }
    else if ( argc    >  1
           && argv[1] != NULL )
    {
        DRM_DWORD cch = 0;
        ChkDR( DRM_SizeTToDWord( DRMCRT_strlen(argv[1]), &cch ) );
        ChkBOOL( cch < NO_OF( rgwcDevCert ), DRM_E_BUFFERTOOSMALL );

        dstrDevCert.pwszString = rgwcDevCert;
        dstrDevCert.cchString  = NO_OF( rgwcDevCert );

        /*
        ** Convert to UTF-16 (DRM string)
        */
        ChkDR( DRM_STR_UTF8toUTF16( ( DRM_CHAR* )( argv[1] ), 0, cch, ( DRM_WCHAR * )dstrDevCert.pwszString, &dstrDevCert.cchString ) );
        dstrDevCert.cchString--; /* remove NULL terminator */

        /*
        ** Replace the devcert and delete the old store
        */
        ChkDR( _CopyFileToDRMFolder( dstrDevCert.pwszString, DEVCERT_FILE_NAME ) );
        ChkDR( RemoveDRMFile( RMFILE_STORE ) );
    }
    if ( argc    >  2
      && argv[2] != NULL )
    {
        /*
        ** Group Priv key must be supplied with the group cert.
        */
        DRM_DWORD cch = 0;
        ChkArg( argc    >  3 );
        ChkArg( argv[3] != NULL );

        ChkBOOL( DRMCRT_strlen( argv[2] ) < NO_OF( rgwcGroupCert ), DRM_E_BUFFERTOOSMALL );

        dstrGroupCert.pwszString = rgwcGroupCert;
        dstrGroupCert.cchString  = NO_OF( rgwcGroupCert );

        /*
        ** Convert to UTF-16 (DRM string)
        */
        ChkDR( DRM_SizeTToDWord( DRMCRT_strlen(argv[2]), &cch ) );
        ChkDR( DRM_STR_UTF8toUTF16( ( DRM_CHAR* ) argv[2], 0, cch, ( DRM_WCHAR * )dstrGroupCert.pwszString, &dstrGroupCert.cchString ) );
        dstrGroupCert.cchString--; /* remove NULL terminator */

        /*
        ** Replace the group cert and delete the old ones
        */
        ChkDR( _CopyFileToDRMFolder( dstrGroupCert.pwszString, GROUP_CERT_FILE_NAME ) );

        ChkBOOL( DRMCRT_strlen( argv[3] ) < NO_OF( rgwcZGPriv ), DRM_E_BUFFERTOOSMALL );

        dstrZGPriv.pwszString = rgwcZGPriv;
        dstrZGPriv.cchString  = NO_OF( rgwcZGPriv );

        /*
        ** Convert to UTF-16 (DRM string)
        */
        ChkDR( DRM_SizeTToDWord( DRMCRT_strlen(argv[3]), &cch ) );
        ChkDR( DRM_STR_UTF8toUTF16( ( DRM_CHAR* ) argv[3], 0, cch, ( DRM_WCHAR * )dstrZGPriv.pwszString, &dstrZGPriv.cchString ) );
        dstrZGPriv.cchString--; /* remove NULL terminator */

        /*
        ** Replace the Model Priv Key
        */
        ChkDR( _CopyFileToDRMFolder( dstrZGPriv.pwszString, GC_PRIVATE_KEY_FILE_NAME ) );

        /*
        ** Delete the HDS and the PlayReady Devcert and priv keys
        */
        if ( argc    <=  6
          || argv[6] == NULL )
        {
            ChkDR( RemoveDRMFile( RMFILE_STORE | RMFILE_KEYFILE ) );
        }
    }
    if ( argc    >  4
      && argv[4] != NULL )
    {
        DRM_DWORD cch = 0;
        /*
        ** Priv key must be supplied with the cert template.
        */
        ChkArg( argc    >  5 );
        ChkArg( argv[5] != NULL );

        ChkBOOL( DRMCRT_strlen( argv[4] ) < NO_OF( rgwcGroupCert ), DRM_E_BUFFERTOOSMALL );

        dstrGroupCert.pwszString = rgwcGroupCert;
        dstrGroupCert.cchString  = NO_OF( rgwcGroupCert );

        /*
        ** Convert to UTF-16 (DRM string)
        */
        ChkDR( DRM_SizeTToDWord( DRMCRT_strlen(argv[4]), &cch ) );
        ChkDR( DRM_STR_UTF8toUTF16( ( DRM_CHAR* ) argv[4], 0, cch, ( DRM_WCHAR * )dstrGroupCert.pwszString, &dstrGroupCert.cchString ) );
        dstrGroupCert.cchString--; /* remove NULL terminator */

        /*
        ** Replace the group cert and delete the old ones
        */
        ChkDR( _CopyFileToDRMFolder( dstrGroupCert.pwszString, DEVCERT_TEMPLATE_FILE_NAME ) );

        ChkBOOL( DRMCRT_strlen( argv[5] ) < NO_OF( rgwcZGPriv ), DRM_E_BUFFERTOOSMALL );

        dstrZGPriv.pwszString = rgwcZGPriv;
        dstrZGPriv.cchString  = NO_OF( rgwcZGPriv );

        /*
        ** Convert to UTF-16 (DRM string)
        */
        ChkDR( DRM_SizeTToDWord( DRMCRT_strlen(argv[5]), &cch ) );
        ChkDR( DRM_STR_UTF8toUTF16( ( DRM_CHAR* ) argv[5], 0, cch, ( DRM_WCHAR * )dstrZGPriv.pwszString, &dstrZGPriv.cchString ) );
        dstrZGPriv.cchString--; /* remove NULL terminator */

        /*
        ** Replace the Priv Key
        */
        ChkDR( _CopyFileToDRMFolder( dstrZGPriv.pwszString, PRIVATE_KEY_FILE_NAME ) );

        /*
        ** Delete the HDS and the WMDRM DevCert
        */
        if ( argc    <=  6
          || argv[6] == NULL )
        {
            ChkDR( RemoveDRMFile( RMFILE_STORE | RMFILE_KEYFILE ) );
        }

    }
    if ( argc > 7
        && argv[7] != NULL )
    {
        /*
        ** Load the whole keyfile bypassing
        */
        DRM_DWORD cch = 0;

        ChkBOOL( cch < NO_OF( rgwcKeyfile ), DRM_E_BUFFERTOOSMALL );

        dstrKeyfile.pwszString = rgwcKeyfile;
        dstrKeyfile.cchString  = NO_OF( rgwcKeyfile );

        /*
        ** Convert to UTF-16 (DRM string)
        */
        ChkDR( DRM_SizeTToDWord( DRMCRT_strlen(argv[7]), &cch ) );
        ChkDR( DRM_STR_UTF8toUTF16( ( DRM_CHAR* ) argv[7], 0, cch, ( DRM_WCHAR * )dstrKeyfile.pwszString, &dstrKeyfile.cchString ) );
        dstrKeyfile.cchString--; /* remove NULL terminator */

        ChkDR( _CopyFileToDRMFolder( dstrKeyfile.pwszString, KEYFILE_FILE_NAME ) );

        /*
        ** Delete HDS
        */
        if ( argc    <=  6
          || argv[6] == NULL )
        {
            ChkDR( RemoveDRMFile( RMFILE_STORE ) );
        }
    }

    GET_SHARED_DEVICE_STORE( pdstrDeviceStore );

    if ( poManagerContext != NULL )
    {
        ChkDR( _ResetOpaqueBuffer( poManagerContext ) );
        ChkDR( Drm_GetOpaqueBuffer( poManagerContext, &pbOpaqueBuffer, &cbOpaqueBuffer ) );
    }

    if( pbOpaqueBuffer == NULL )
    {
        GET_SHARED_DRMBUFFER( pbOpaqueBuffer );
    }

    ChkDR( Drm_Initialize( poManagerContext, NULL, pbOpaqueBuffer, TEST_DRM_BUFFER_SIZE, pdstrDeviceStore ) );

    /*
    ** If we replaced anything we need to reset the ClientID global variable for license generation
    */
    if ( argc    >  2 )
    {
        CLIENTID oClientID = { 0 };

        ChkDR( TestGetClientID( &oClientID ) );
        ChkDR( TestLicResponseSetClientID( &oClientID ) );
    }

ErrorExit:
    return dr;
}

/*****************************************************************************
** Function:    TestManagerInitializeForNewStore
**
** Synopsis:
**
** Arguments:   argv[0] : file name
**              argv[1] : indicates the file should be deleted before initialization
**
** Returns:     DRM_SUCCESS                  - on success
**
** Note:        This test function is not currently in use and may require
**              updates to work properly
**
******************************************************************************/
DRM_RESULT DRM_CALL TestManagerInitializeForNewStore( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT        dr                  = DRM_SUCCESS;
    DRM_CONST_STRING  dstrFileName        = {0};
    DRM_CONST_STRING  dstrDeviceStorePath = {0};
    DRM_APP_CONTEXT  *poManagerContext    = NULL;
    DRM_BYTE         *pbOpaqueBuffer      = NULL;
    DRM_DWORD         cbOpaqueBuffer      = 0;
    DRM_BYTE         *pbRevocationBuffer  = NULL;
    DRM_WCHAR         wszPathName[GET_DRM_FILE_NAME_PATH_LENGTH] = {'\0'};

    ChkArg( argc > 0 );
    ChkArg( argv[0] != NULL );

    GET_SHARED_APP_CONTEXT( poManagerContext );

    ChkDR( MakeDRMString( &dstrFileName, argv[0] ) );
    ChkDR( tGetDRMFileName( wszPathName, &dstrDeviceStorePath, dstrFileName.pwszString ) );

    Log("", "Trying to load %s", dstrDeviceStorePath.pwszString );

    if ( argc          >  1
      && argv[1]       != NULL
      && atoi(argv[1]) != 0 )
    {
        ChkDR( tRemoveFileW( dstrDeviceStorePath.pwszString ) );
    }

    ChkDR( _ResetOpaqueBuffer( poManagerContext ) );

    ChkDR( Drm_GetOpaqueBuffer( poManagerContext, &pbOpaqueBuffer, &cbOpaqueBuffer ) );
    if( pbOpaqueBuffer == NULL )
    {
        GET_SHARED_DRMBUFFER( pbOpaqueBuffer );
    }
    ChkDR( Drm_Initialize( poManagerContext, NULL, pbOpaqueBuffer, TEST_DRM_BUFFER_SIZE, &dstrDeviceStorePath ) );

    if( DRM_REVOCATION_IsRevocationSupported() )
    {
        /*
        ** Set REVOCATION_BUFFER_VAR
        */
        MAP_TO_GLOBAL_TEST_VAR( pbRevocationBuffer, REVOCATION_BUFFER_VAR );
        ChkDR( Drm_Revocation_SetBuffer( poManagerContext, pbRevocationBuffer, REVOCATION_BUFFER_SIZE ) );
    }

ErrorExit:
    FREEDRMSTRING( dstrFileName );
    return dr;
}

/*****************************************************************************
** Function:    TestManagerUnInitialize
**
** Synopsis:
**
** Arguments:
**
** Returns:     DRM_SUCCESS                  - on success
**
** Note:
**
******************************************************************************/
DRM_RESULT DRM_CALL TestManagerUninitialize( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT       dr               = DRM_SUCCESS;
    DRM_APP_CONTEXT *poManagerContext = NULL;

    GET_SHARED_APP_CONTEXT( poManagerContext );

    Drm_Uninitialize( poManagerContext );

    MEMSET( poManagerContext, 0, SIZEOF( DRM_APP_CONTEXT ) );

ErrorExit:
    return dr;
}


/*****************************************************************************
** Function:    TestManagerResizeOpaqueBuffer
**
** Synopsis:    Facilitate testing of the functionality of Drm_ResizeOpaqueBuffer().
**
** Arguments:   argc:    parameter count
**              argv[0]: an integer - the absolute size of the new buffer
**
** Returns:     DRM_SUCCESS - on success
**
** Note:
**
******************************************************************************/
DRM_RESULT DRM_CALL TestManagerResizeOpaqueBuffer( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT        dr               = DRM_SUCCESS;
    DRM_APP_CONTEXT  *poManagerContext = NULL;
    DRM_BYTE         *pbNewBuffer      = NULL;
    DRM_DWORD         cbNewSize        = 0;


    ChkArg( ( argc == 1 ) && ( argv[0] != NULL ) );
    cbNewSize = (DRM_DWORD)atoi( argv[0] );

    /*
    ** 1) Allocate a new buffer.
    ** 2) Replace the opaque buffer in the app context.
    ** 3) Swap the test variables so that DRMBUFFER_VAR points to the new buffer
    */
    INIT_VARIABLE_BY_NAME( pbNewBuffer, "NEW_DRMBUFFER_VAR", cbNewSize );

    GET_SHARED_APP_CONTEXT( poManagerContext );
    ChkDR( Drm_ResizeOpaqueBuffer( poManagerContext, pbNewBuffer, cbNewSize ) );

    ChkDR( DRM_TST_SwapTestVars( "NEW_DRMBUFFER_VAR", DRMBUFFER_VAR ) );

ErrorExit:
    /* Free the 'new buffer' test variable;
    ** it either points to the new buffer that didnt get used properly (error path)
    ** or it was swapped with the default 'old buffer' (success path).
    */
    DRM_TST_FreeTestVar( "NEW_DRMBUFFER_VAR" );
    return dr;
}


EXIT_PKTEST_NAMESPACE_CODE
