/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdafx.h>
#include <oemkeyfile.h>
#include <drmactivation.h>
#include <tOEMIMP_PD.h>

#if DRM_BUILD_PROFILE == DRM_BUILD_PROFILE_WM7
#include <windows.h>
#include <shellapi.h>
#endif /*: DRM_BUILD_PROFILE == DRM_BUILD_PROFILE_WM7 */

ENTER_PKTEST_NAMESPACE_CODE;

/*****************************************************************************
** Function:    TestScanDevicePrivateKey
**
** Synopsis:    Scan the context memory for the device private key. This verifies the context doesn't contain the key by accident.
**
** Arguments:   pManagerContext
**
** Returns:     DRM_SUCCESS on success or
**              Or any return code as defined in drmresults.h
*****************************************************************************/
DRM_RESULT DRM_CALL TestScanDevicePrivateKey( DRM_APP_CONTEXT *pManagerContext )
{
    /* This function is a no-op now that we use keyfiles. */
    return DRM_SUCCESS;
}

/*****************************************************************************
** Function:    RestoreDeviceStore
**
** Synopsis:    Call this to replace the test devcert and devicestore with the
**              ones specified in the arguments
**
** Arguments:   argv[0] - Name of the device cert to be restored
**              argv[1] - Name of the device HDS to be restored
**
** Returns:     DRM_SUCCESS on success or
**              Or any return code as defined in drmresults.h
*****************************************************************************/
DRM_RESULT DRM_CALL RestoreDeviceStore( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT        dr           = DRM_SUCCESS;
    DRM_CONST_STRING  dstrDevCert  = {0};
    DRM_CONST_STRING  dstrDevStore = {0};
    CLIENTID          clientID     = {0};
    DRM_APP_CONTEXT  *poAppContext = NULL;
    DRM_CONST_STRING *pdstrDevStore = NULL;

    ChkBOOL( !DRM_ACTIVATION_IsActivationSupported(), DRM_E_NOTIMPL );

    ChkArg(argc == 2);
    ChkArg(argv[0] != NULL || argv[1] != NULL);

    if( argv[0] != NULL )
    {
        ChkDR( MakeDRMString( &dstrDevCert, argv[0] ) );
    }

    if( argv[1] != NULL )
    {
        ChkDR( MakeDRMString( &dstrDevStore, argv[1] ) );
    }

    FREE_SHARED_APP_CONTEXT( poAppContext, pdstrDevStore );

    ChkDR( TST_UTL_RestoreDeviceStore( dstrDevStore, dstrDevCert ) );

    ChkDR( _CopyFileToDRMFolder( BDEVCERT_FILE_NAME, BDEVCERT_FILE_NAME ) );
    ChkDR( _CopyFileToDRMFolder( Z_PRIVATE_KEY_FILE_NAME_SIGN, Z_PRIVATE_KEY_FILE_NAME_SIGN ) );
    ChkDR( _CopyFileToDRMFolder( Z_PRIVATE_KEY_FILE_NAME_ENCRYPT, Z_PRIVATE_KEY_FILE_NAME_ENCRYPT ) );

    Drm_ClearAllCaches();
    INIT_SHARED_APP_CONTEXT( poAppContext, pdstrDevStore );
    ChkDR( TestGetClientID( &clientID ) );
    ChkDR( TestLicResponseSetClientID( &clientID ) );

ErrorExit:
    FREEDRMSTRING( dstrDevCert );
    FREEDRMSTRING( dstrDevStore );
    return dr;
}

/*****************************************************************************
** Function:    SaveDeviceStore
**
** Synopsis:    Call this to save the test devcert and devicestore for future use
**
** Arguments:   argv[0] - DeviceCert will be saved with this new name in curr. working dir
**              argv[1] - DeviceStore will be saved with this new name in curr. working dir
**
** Returns:     DRM_SUCCESS on success or
**              Or any return code as defined in drmresults.h
*****************************************************************************/
DRM_RESULT DRM_CALL SaveDeviceStore( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT        dr            = DRM_SUCCESS;
    DRM_CONST_STRING  dstrDevCert   = {0};
    DRM_CONST_STRING  dstrDevStore  = {0};
    DRM_APP_CONTEXT  *poAppContext  = NULL;
    DRM_CONST_STRING *pdstrDevStore = NULL;

    ChkBOOL( !DRM_ACTIVATION_IsActivationSupported(), DRM_E_NOTIMPL );

    ChkArg( argc == 2 );
    ChkArg( argv[0] != NULL || argv[1] != NULL );

    if( argv[0] != NULL )
    {
        ChkDR( MakeDRMString( &dstrDevCert, argv[0] ) );
    }

    if( argv[1] != NULL )
    {
        ChkDR( MakeDRMString( &dstrDevStore, argv[1] ) );
    }

    FREE_SHARED_APP_CONTEXT( poAppContext, pdstrDevStore );

    ChkDR( TST_UTL_SaveDeviceStore( dstrDevStore, dstrDevCert ) );

ErrorExit:
    FREEDRMSTRING( dstrDevCert );
    FREEDRMSTRING( dstrDevStore );
    return dr;
}

DRM_RESULT DRM_CALL TestDrmInitializeWithActivation(
    __in                                    DRM_APP_CONTEXT     *f_poAppContext,
    __in                                    DRM_VOID            *f_pOEMContext,
    __in_bcount( f_cbOpaqueBuffer )         DRM_BYTE            *f_pbOpaqueBuffer,
    __in                                    DRM_DWORD            f_cbOpaqueBuffer,
    __in                              const DRM_CONST_STRING    *f_pdstrDeviceStoreName )
{
    DRM_RESULT       dr                                         = DRM_SUCCESS;
    DRM_BYTE         rgbActivationCtx[ACTIVATION_CONTEXT_SIZE]  = {0};
    DRM_BYTE        *pbChallenge                                = NULL;
    DRM_DWORD        cbChallenge                                = 0;
    DRM_BYTE        *pbResponse                                 = NULL;
    DRM_DWORD        cbResponse                                 = 0;
    char            *pchUrl                                     = NULL;
    DRM_DWORD        cchUrl                                     = 0;
    char            *pchUrlAlloc                                = NULL;
    DRM_RESULT       drSoap                                     = DRM_SUCCESS;
#if 0
    /* Real URL Do not use because XNET cannot redirect to corpnet */
    const char       c_rchUrlDefault[]                          = "http://go.microsoft.com/fwlink/?LinkID=215731";
    /* Internal unsupported test URL which confirms that redirection works if we get around the above issue */
    const char       c_rchUrlDefault[]                          = "http://go.microsoft.com/fwlink/?LinkID=72125";
#endif /* 0 */
    /* Direct link to the test server */
    const char       c_rchUrlDefault[]                          = "http://prindiv-05.redmond.corp.microsoft.com/PlayReady/ACT/Activation.asmx?WSDL";

    dr = Drm_Initialize( f_poAppContext, f_pOEMContext, f_pbOpaqueBuffer, f_cbOpaqueBuffer, f_pdstrDeviceStoreName );
    if( DRM_ACTIVATION_IsActivationSupported() && dr == DRM_E_ACTIVATION_REQUIRED )
    {
        dr = DRM_SUCCESS;
        pchUrl = (char*)c_rchUrlDefault;

        do
        {
            ChkDR( tOEM_SendNetData(
                pchUrl,
                pbChallenge == NULL ? eDRM_TEST_NET_ACTIVATIONGET : eDRM_TEST_NET_ACTIVATIONPOST,
                pbChallenge,
                cbChallenge,
                &pbResponse,
                &cbResponse ) );

            pchUrl = NULL;
            cchUrl = 0;
            SAFE_OEM_FREE( pchUrlAlloc );

            if( pbChallenge != NULL )
            {
                ZEROMEM( pbChallenge, cbChallenge );
            }

            dr = Drm_Activation_ProcessResponseGenerateChallenge(
                rgbActivationCtx,
                pbResponse,
                cbResponse,
                pbChallenge,
                &cbChallenge,
                pchUrl,
                &cchUrl,
                &drSoap );

            if( dr == DRM_E_BUFFERTOOSMALL )
            {
                dr = DRM_SUCCESS;
                ChkBOOL( cbChallenge > 0, DRM_E_FAIL );
                SAFE_OEM_FREE( pbChallenge );
                ChkMem( pbChallenge = ( DRM_BYTE * )Oem_MemAlloc( cbChallenge ) );
                ZEROMEM( pbChallenge, cbChallenge );

                if( cchUrl > 0 )
                {
                    ChkMem( pchUrlAlloc = ( DRM_CHAR * )Oem_MemAlloc( cchUrl ) );
                    ZEROMEM( pchUrlAlloc, cchUrl );
                    pchUrl = pchUrlAlloc;
                }

                dr = Drm_Activation_ProcessResponseGenerateChallenge(
                    rgbActivationCtx,
                    pbResponse,
                    cbResponse,
                    pbChallenge,
                    &cbChallenge,
                    pchUrl,
                    &cchUrl,
                    &drSoap );
            }

            /*
            ** Just propagate any SOAP error which takes precendence over
            ** the result of Drm_Activation_ProcessResponseGenerateChallenge
            ** Don't overwrite dr with drSoap if drSoap succeeded
            */
            if( DRM_FAILED( drSoap ) )
            {
                ChkDR( drSoap );
            }

            /*
            ** Just propagate any error from Drm_Activation_ProcessResponseGenerateChallenge
            */
            ChkDR( dr );

        } while( cbChallenge > 0 );

        ChkDR( Drm_Initialize( f_poAppContext, f_pOEMContext, f_pbOpaqueBuffer, f_cbOpaqueBuffer, f_pdstrDeviceStoreName ) );
    }
    else
    {
        ChkDR( dr );
    }

ErrorExit:
    SAFE_OEM_FREE( pbChallenge );
    SAFE_OEM_FREE( pbResponse  );
    SAFE_OEM_FREE( pchUrlAlloc );
    return dr;
}

#define TESTDRM_PLATFORM_INITIALIZE_CALLED "TestDrm_Platform_Initialize_Called"

DRM_RESULT DRM_CALL TestDrm_Platform_Initialize()
{
    DRM_RESULT dr                    = DRM_SUCCESS;
    DRM_BOOL  *pfPlatformInitialized = NULL;

    MAP_TO_GLOBAL_TEST_VAR_AllocIfNotExists( pfPlatformInitialized, TESTDRM_PLATFORM_INITIALIZE_CALLED, SIZEOF( DRM_BOOL ) );

    if( !*pfPlatformInitialized )
    {
        ChkDR( Drm_Platform_Initialize() );
        *pfPlatformInitialized = TRUE;
    }

ErrorExit:
    return dr;
}

DRM_RESULT DRM_CALL TestDrm_Platform_Uninitialize()
{
    DRM_RESULT dr                    = DRM_SUCCESS;
    DRM_BOOL  *pfPlatformInitialized = NULL;

    MAP_TO_GLOBAL_TEST_VAR_AllocIfNotExists( pfPlatformInitialized, TESTDRM_PLATFORM_INITIALIZE_CALLED, SIZEOF( DRM_BOOL ) );

    if( *pfPlatformInitialized )
    {
        ChkDR( Drm_Platform_Uninitialize() );
        *pfPlatformInitialized = FALSE;
    }

ErrorExit:
    return dr;
}

BEGIN_TEST_API_LIST(PKCommon)
    API_ENTRY(SaveDeviceStore)
    API_ENTRY(RestoreDeviceStore)
END_TEST_API_LIST

DRM_RESULT DRM_CALL MODULE_PRETESTCASE(PKCommon)(long lTCID, const char *strTCName)
{
    DRM_RESULT        dr                  = DRM_SUCCESS;
    DRM_APP_CONTEXT  *poAppContext        = NULL;
    DRM_CONST_STRING *pwszDeviceStoreName = NULL;
    DRM_BYTE         *pbRevocationBuffer  = NULL;

    tResetSystemTimeOffset();
    ChkDR( TestDrm_Platform_Initialize() );

    RemoveDRMFile( RMFILE_ALL );

    if( !DRM_ACTIVATION_IsActivationSupported() )
    {
        dr = SetDeviceEnv( DEVCERT_TEMPLATE_FILE_NAME,
                           PRIVATE_KEY_FILE_NAME,
                           GROUP_CERT_FILE_NAME,
                           GC_PRIVATE_KEY_FILE_NAME,
                           FALSE );
        if ( dr != DRM_SUCCESS )
        {
            Log( "Warning", "PKCommon Module could not set the device environment.  Shared App context is not set." );
            dr = DRM_SUCCESS;
            goto ErrorExit;
        }

        dr = _CopyFileToDRMFolder( BDEVCERT_FILE_NAME, BDEVCERT_FILE_NAME );
        if ( dr != DRM_SUCCESS )
        {
            Log( "Warning", "PKCommon Module could not set the binary device cert.   This may be an issue if the test needs a static device cert. You can usually ignore this." );
            dr = DRM_SUCCESS;
        }
        dr = _CopyFileToDRMFolder( Z_PRIVATE_KEY_FILE_NAME_SIGN, Z_PRIVATE_KEY_FILE_NAME_SIGN );
        if ( dr != DRM_SUCCESS )
        {
            Log( "Warning", "PKCommon Module could not set the signing binary device cert's key. This may be an issue if the test needs a static signing key. You can usually ignore this." );
            dr = DRM_SUCCESS;
        }
        dr = _CopyFileToDRMFolder( Z_PRIVATE_KEY_FILE_NAME_ENCRYPT, Z_PRIVATE_KEY_FILE_NAME_ENCRYPT );
        if ( dr != DRM_SUCCESS )
        {
            Log( "Warning", "PKCommon Module could not set the encrypting binary device cert's key. This may be an issue if the test needs a static encryption key. You can usually ignore this." );
            dr = DRM_SUCCESS;
        }
    }

    INIT_SHARED_APP_CONTEXT( poAppContext, pwszDeviceStoreName );
    if ( DRM_REVOCATION_IsRevocationSupported() )
    {
        Log( "", "Setting REVOCATION_BUFFER_VAR" );
        MAP_TO_GLOBAL_TEST_VAR_Alloc( pbRevocationBuffer, REVOCATION_BUFFER_VAR, REVOCATION_BUFFER_SIZE );
        ZEROMEM( pbRevocationBuffer, REVOCATION_BUFFER_SIZE );
        ChkDR( Drm_Revocation_SetBuffer( poAppContext, pbRevocationBuffer, REVOCATION_BUFFER_SIZE ) );
    }
ErrorExit:
    return dr;
}

DRM_RESULT DRM_CALL MODULE_POSTTESTCASE(PKCommon)(long lTCID, const char *strTCName)
{
    DRM_RESULT        dr                  = DRM_SUCCESS;
    DRM_APP_CONTEXT  *poAppContext        = NULL;
    DRM_CONST_STRING *pwszDeviceStoreName = NULL;

    FREE_SHARED_APP_CONTEXT( poAppContext, pwszDeviceStoreName );

    RemoveDRMFile( RMFILE_ALL );

    ChkDR( TestDrm_Platform_Uninitialize() );

ErrorExit:
    return dr;
}

#if DRM_BUILD_PROFILE == DRM_BUILD_PROFILE_WM7
DRM_BOOL DRM_CALL LaunchDRMHelperRoutine(LPCTSTR pwszProgrm, LPCTSTR pwszParams)
{
    BOOL bRtn = FALSE;
    SHELLEXECUTEINFO ShellInfo = {0};

    ShellInfo.cbSize = sizeof(SHELLEXECUTEINFO);
    ShellInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
    ShellInfo.lpFile = pwszProgrm;
    ShellInfo.lpVerb = L"Open";
    ShellInfo.lpParameters = pwszParams;

    bRtn = ShellExecuteEx(&ShellInfo);

    if (FALSE == bRtn)
    {
        Log( "Warning", "LaunchDRMHelperRoutine: ShellExecuteEx failed 0x%x", GetLastError() );
        goto ErrorExit;
    }

    WaitForSingleObject(ShellInfo.hProcess,INFINITE);

ErrorExit:
    if( ShellInfo.hProcess != NULL)
    {
        CloseHandle(ShellInfo.hProcess);
    }
    return bRtn;
}
#endif //endif: DRM_BUILD_PROFILE == DRM_BUILD_PROFILE_WM7


EXIT_PKTEST_NAMESPACE_CODE
