/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include "stdafx.h"
#include "stdio.h"
#include <DeviceDevCertTestAPI.h>
#include <drmbcert.h>
#include <drmbcertparser.h>
#include <drmtoolsutils.h>
#include <oemkeyfile.h>
#include <devicedevcert.h>
#include <oemcommon.h>
#include <PKCommonTestAPI.h>
#include <drmsecureclock.h>
#include <drmantirollbackclock.h>
#include <drmmodelrevocation.h>
#include <drmdevcertkeygen.h>

ENTER_PKTEST_NAMESPACE_CODE

DRM_STRING g_dstrData_DDC[GDATA_TOTAL_COUNT];
DRM_BYTE   *g_pbData_DDC[GDATA_TOTAL_COUNT];
DRM_DWORD  g_cbData_DDC[GDATA_TOTAL_COUNT];


/*****************************************************************************
** Function:    Test_SetRobustnessVersion
**
** Synopsis:    Sets the firmware's reported robustness version
**
** Arguments:   argv[0] : Robustness version to set
**              argv[1] : Platform ID to set in decimal
**              argv[2] : DRM flavor, optional: 1 for WMDRM, 2 for PlayReady.
**                        If this param is omitted then the same robustness version value
**                        is set for both DRM flavors. Otherwise only one robustness version
**                        will be set.
**
** Returns:     DRM_SUCCESS                  - on success
**
** Note: 
**
**
******************************************************************************/
DRM_RESULT DRM_CALL Test_SetRobustnessVersion( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_CONST_STRING dstrSecurityVersion  = EMPTY_DRM_STRING;
    DRM_DWORD dwDRMFlavor = 0; /* default value: same robustness version for both DRM flavors */

    ChkArg( argc == 2 || argc == 3 );
    ChkArg( argv[0] != NULL );
    ChkArg( argv[1] != NULL );

    ChkDR( MakeDRMString( &dstrSecurityVersion, argv[0] ) );

    ChkDR( DRMCRT_AtoDWORD( argv[1], DRMCRT_strlen( argv[1] ), 10, &g_dwPlatformID ) );

    if ( argc == 3 )
    {
        ChkDR( DRMCRT_AtoDWORD( argv[2], DRMCRT_strlen( argv[2] ), 10, &dwDRMFlavor ) );
    }
    
    switch( dwDRMFlavor )
        {
        case 0:
            ChkDR( DRM_UTL_GetVersionFromStringAsDWORD( dstrSecurityVersion.pwszString, 
                                                        dstrSecurityVersion.cchString, 
                                                        &g_dwWMRobustnessVersion ) );
            ChkDR( DRM_UTL_GetVersionFromStringAsDWORD( dstrSecurityVersion.pwszString, 
                                                        dstrSecurityVersion.cchString, 
                                                        &g_dwPRRobustnessVersion ) );
            break;
        case 1:
            ChkDR( DRM_UTL_GetVersionFromStringAsDWORD( dstrSecurityVersion.pwszString, 
                                                        dstrSecurityVersion.cchString, 
                                                        &g_dwWMRobustnessVersion ) );
            break;
        case 2:
            ChkDR( DRM_UTL_GetVersionFromStringAsDWORD( dstrSecurityVersion.pwszString, 
                                                        dstrSecurityVersion.cchString, 
                                                        &g_dwPRRobustnessVersion ) );
            break;
        default:
            ChkDR(DRM_E_TEST_INVALIDARG);
        }

ErrorExit:
    return dr;
}


/*****************************************************************************
** Function:    Test_RemoveDeviceData
**
** Synopsis:    Remove devcert, devcert template and private key file
**
** Arguments:   none
**
** Returns:     DRM_SUCCESS          - on success
**
**
******************************************************************************/
DRM_RESULT DRM_CALL Test_RemoveDeviceData(long argc, __in_ecount_opt( argc ) char **argv)
{
    DRM_RESULT dr = DRM_E_TEST_INCOMPLETE;
    DRM_LONG   i  = 0;

    for( i = 0; i < argc; i++ )
    {
        Log("Trace", "\t\tFrom Test_RemoveDeviceData: argv[%d]=%s", i, argv[i]);
    }
    
    ChkDR( RemoveDRMFile( RMFILE_ALL ) );
    
    dr = DRM_SUCCESS;

ErrorExit:
    return dr;
}


/*****************************************************************************
** Function:    Test_SetDeviceEnv
**
** Synopsis:    Copy devcert template and private key file and create the devcert optionally
**
** Arguments:   argv[0]          : Device Cert Template Filename (legacy XML cert)
**              argv[1]          : Private Key filename (for device cert template)
**              argv[2]          : Group Cert filename (Binary certs)
**              argv[3]          : Group Cert Private Key file name
**              argv[4]          : Flag indicating if devcerts should be created
**
** Returns:     DRM_SUCCESS          - on success
**
**
******************************************************************************/
DRM_RESULT DRM_CALL Test_SetDeviceEnv(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT       dr                  = DRM_E_TEST_INCOMPLETE;
    DRM_CONST_STRING dstrDevCertTemplate = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrDTPrivKey       = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrGroupCert       = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrGCPrivKey       = EMPTY_DRM_STRING;
    DRM_BOOL         fCreateDevCerts     = FALSE;
    DRM_LONG         i                   = 0;

    for( i = 0; i < argc; i++ )
    {
        Log("Trace", "\t\tFrom Test_SetDeviceEnv: argv[%d]=%s", i, argv[i]);
    }

    ChkArg( argc >= 5 );

    if ( argv[0] != NULL )
    {
        ChkDR( MakeDRMString( &dstrDevCertTemplate, argv[0] ) );
    }
    if ( argv[1] != NULL )
    {
        ChkDR( MakeDRMString( &dstrDTPrivKey,       argv[1] ) );
    }
    if ( argv[2] != NULL )
    {
        ChkDR( MakeDRMString( &dstrGroupCert,       argv[2] ) );
    }
    if ( argv[3] != NULL )
    {
        ChkDR( MakeDRMString( &dstrGCPrivKey,       argv[3] ) );
    }
    if ( argv[4] != NULL )
    {
        ChkDR( StringToBool( argv[4], &fCreateDevCerts ) );
    }
    ChkDR( SetDeviceEnv( dstrDevCertTemplate.pwszString,
                         dstrDTPrivKey.pwszString,
                         dstrGroupCert.pwszString,
                         dstrGCPrivKey.pwszString,
                         fCreateDevCerts ) );
    
    dr = DRM_SUCCESS;
    
ErrorExit:
    FREEDRMSTRING( dstrDevCertTemplate );
    FREEDRMSTRING( dstrDTPrivKey );
    FREEDRMSTRING( dstrGroupCert );
    FREEDRMSTRING( dstrGCPrivKey );
    return dr;
}


/*****************************************************************************
** Function:    Test_InitWithOemData
**
** Synopsis:    Call Drm_Initialize with OemContext data to create a devcert if it's not there
**
** Arguments:   argv[0]          : String, type of data to be added to the OemContext. "DeviceModel" for device model data
**              argv[1]          : String, info to be added. If argv[0]=="DeviceModel", argv[1] is the device model data to be added
**
** Returns:     DRM_SUCCESS      - on success
**
**
******************************************************************************/
DRM_RESULT DRM_CALL Test_InitWithOemData(long argc, __in_ecount(argc) char **argv)
{
#define MAX_TEST_DEVICE_MODEL_NAME  256   /* 256 WCHAR, 512 bytes */

    DRM_RESULT                    dr                         = DRM_E_TEST_INCOMPLETE;

#if DRM_BUILD_PROFILE == DRM_BUILD_PROFILE_WM7

    DRM_CONST_STRING              dstrDeviceInfoType         = EMPTY_DRM_STRING;
    DRM_CONST_STRING              dstrDeviceModel            = EMPTY_DRM_STRING;
    DRM_WCHAR                     rgwchDeviceModel[ MAX_TEST_DEVICE_MODEL_NAME + 1 ] = {0};
    DRM_LONG                      i                          = 0;
    HKEY                          hkWM7Model                 = NULL;
    WCHAR                         pwszParameters[200]        = {0};
    BOOL                          bSetRegSuccess             = FALSE;


    DRMASSERT(DRM_MODELREVOCATION_IsModelRevocationSupported());

    for( i = 0; i < argc; i++ )
    {
        Log("Trace", "\t\tFrom Test_InitWithOemData: argv[%d]=%s", i, argv[i]);
    }

    // One type-info pair as input
    ChkArg( argc == 2 );

    ChkArg(argv[0]);
    ChkArg(argv[1]);

    ChkDR( MakeDRMString( &dstrDeviceInfoType, argv[0] ) );

    if( 0 == MEMCMP(dstrDeviceInfoType.pwszString, L"DeviceModel", dstrDeviceInfoType.cchString  * SIZEOF(DRM_WCHAR)) )
    {
        ChkDR( MakeDRMString(&dstrDeviceModel, argv[1]) );

        // Check if the registry entry for model name exist. If not, print a warning and create it:
        if(RegOpenKeyExW( HKEY_LOCAL_MACHINE, REG_SUBKEY_WM7_MODEL_STRING, 0, 0, &hkWM7Model ) != ERROR_SUCCESS || hkWM7Model == NULL)
        {
            Log("Warning:", "\t\tFrom Test_InitWithOemData: reg entry for model name not exist.");

            wcscat(pwszParameters, L"HKLM\\");
            wcscat(pwszParameters, REG_SUBKEY_WM7_MODEL_STRING);
            wcscat(pwszParameters, L" ");
            wcscat(pwszParameters, REG_VALUENAME_WM7_MODEL_STRING);
            wcscat(pwszParameters, L"=\"\"");

            bSetRegSuccess = LaunchDRMHelperRoutine(L"regcreatekey", pwszParameters);
            ChkBOOL(bSetRegSuccess, DRM_E_FAIL);

            _wcsset(pwszParameters, L'\0');
            bSetRegSuccess = FALSE;
        }

        wcscat(pwszParameters, L"-r HKEY_LOCAL_MACHINE -p ");
        wcscat(pwszParameters, REG_SUBKEY_WM7_MODEL_STRING);
        wcscat(pwszParameters, L" -n ");
        wcscat(pwszParameters, REG_VALUENAME_WM7_MODEL_STRING);
        wcscat(pwszParameters, L" -v \"");

        if( 0 == MEMCMP(dstrDeviceModel.pwszString, L"RealModelInfo", dstrDeviceModel.cchString * SIZEOF(DRM_WCHAR)) )
        {
            // Delete the registry override so we'll use the "real" model info. Delete by setting its value to be ""
            wcscat(pwszParameters, L"\"\"");
            bSetRegSuccess = LaunchDRMHelperRoutine(L"SetRegValue", pwszParameters);

            ChkBOOL(bSetRegSuccess, DRM_E_FAIL);
        }
        else if( 0 != MEMCMP(dstrDeviceModel.pwszString, L"RegModelInfo", dstrDeviceModel.cchString * SIZEOF(DRM_WCHAR)) )
        {
            ChkBOOL( dstrDeviceModel.cchString < MAX_TEST_DEVICE_MODEL_NAME + 1, DRM_E_BCERT_MANUFACTURER_STRING_TOO_LONG);

            wcscat(pwszParameters, dstrDeviceModel.pwszString);
            wcscat(pwszParameters, L"\"");
            bSetRegSuccess = LaunchDRMHelperRoutine(L"SetRegValue", pwszParameters);

            ChkBOOL(bSetRegSuccess, DRM_E_FAIL);
        }
        // If they specified "RegModelInfo" it means we should do nothing
    }
    else
    {
        dr = DRM_E_NOTIMPL;
        ChkDR(dr);
    }

    ChkDR( InitWithOemData( NULL ) );
   
ErrorExit:
    FREEDRMSTRING( dstrDeviceInfoType );
    return dr;
#else //else: DRM_BUILD_PROFILE == DRM_BUILD_PROFILE_WM7
    ChkDR( InitWithOemData( NULL ) );
ErrorExit:
    return dr;
#endif //endif: DRM_BUILD_PROFILE == DRM_BUILD_PROFILE_WM7
}


/*****************************************************************************
** Function:    Test_API_GetDeviceCertificate
**
** Synopsis:    Test the api DRM_DDC_GetDeviceCertificate 
**
** Arguments:   argv[0]: size of the output buffer or NULL, EMPTY
**              argv[1]: dwFlag: 0 or 1 (DRM_DCP_CREATE_DEVCERT_IF_NOT_EXISTING)
**              argv[2]: crypto context: NORMAL or NULL
**              argv[3]: output buffer index: 0 or 1
**
** Returns:     DRM_SUCCESS          - on success
**
**
******************************************************************************/
DRM_RESULT DRM_CALL Test_API_GetDeviceCertificate(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT         dr         = DRM_E_TEST_INCOMPLETE;
    DRM_DWORD          cbDataStr  = 0;
    DRM_DWORD          dwFlags    = 0;
    DRM_CRYPTO_CONTEXT *pCrypto   = NULL;
    DRM_DWORD          dwIndex    = 0;
    DRM_LONG           i          = 0;
    DRM_KEYFILE_CONTEXT *poKeyfileContext = NULL;

    for( i = 0; i < argc; i++ )
    {
        Log("Trace", "\t\tFrom Test_API_GetDeviceCertificate: argv[%d]=%s", i, argv[i]);
    }
    
    ChkArg( argc >= 4 );
    ChkArg( argv[1] != NULL );

    if ( argv[0] != NULL ) 
    {
        ChkArg( DRM_SUCCESS == DRMCRT_AtoDWORD( argv[3], DRMCRT_strlen( argv[3] ), 10, &dwIndex ) );
        ChkArg( dwIndex < GDATA_COUNT );

        FREEDRMSTRING( g_dstrData_DDC[dwIndex] );

        ChkArg( DRM_SUCCESS == DRMCRT_AtoDWORD( argv[0], DRMCRT_strlen( argv[0] ), 10, &cbDataStr ) );
        g_dstrData_DDC[dwIndex].cchString = cbDataStr / SIZEOF( DRM_WCHAR );

        if ( g_dstrData_DDC[dwIndex].cchString != 0 )
        {
            ChkMem( g_dstrData_DDC[dwIndex].pwszString = (DRM_WCHAR*)Oem_MemAlloc( g_dstrData_DDC[dwIndex].cchString * SIZEOF( DRM_WCHAR ) ) );
        }
        else 
        {
            g_dstrData_DDC[dwIndex].pwszString = NULL;
        }
    }

    ChkArg( DRM_SUCCESS == DRMCRT_AtoDWORD( argv[1], DRMCRT_strlen( argv[1] ), 10, &dwFlags ) );

    if ( argv[2] != NULL )
    {   
        ChkMem(  pCrypto = (DRM_CRYPTO_CONTEXT*)Oem_MemAlloc( SIZEOF( DRM_CRYPTO_CONTEXT ) ) );
        ZEROMEM( pCrypto, SIZEOF( DRM_CRYPTO_CONTEXT ) );
    }
    
    ChkMem( poKeyfileContext = (DRM_KEYFILE_CONTEXT*) Oem_MemAlloc( SIZEOF(DRM_KEYFILE_CONTEXT) ) );

    ChkDR( DRM_KF_Initialize( NULL, poKeyfileContext ) );
    ChkDR( DRM_KF_Open( poKeyfileContext, TRUE, OEM_GENERIC_READ | OEM_GENERIC_WRITE ) );
    ChkDR( DRM_KF_LoadFromFile( poKeyfileContext, TRUE ) );
    ChkDR( DRM_BBX_SetupSecureStoreKey( poKeyfileContext, NULL ) );
    ChkDR( DRM_KF_Close( poKeyfileContext ) );

    dr = DRM_DDC_GetXMLDeviceCertificate( NULL, 
                                          poKeyfileContext, 
                                          &g_dstrData_DDC[dwIndex], 
                                          dwFlags,
                                          pCrypto );
    if ( DRM_FAILED(dr) )
    {
        if ( ( dwFlags & DRM_DCP_CREATE_DEVCERT_IF_NOT_EXISTING ) == DRM_DCP_CREATE_DEVCERT_IF_NOT_EXISTING )
        {
            ChkDR( DRM_KF_Open( poKeyfileContext, TRUE, OEM_GENERIC_READ | OEM_GENERIC_WRITE ) );
            ChkDR( DRM_KF_LoadFromFile( poKeyfileContext, TRUE ) );

            ChkDR( SetDeviceEnv( DEVCERT_TEMPLATE_FILE_NAME, 
                                 PRIVATE_KEY_FILE_NAME, 
                                 GROUP_CERT_FILE_NAME, 
                                 GC_PRIVATE_KEY_FILE_NAME, 
                                 FALSE ) );

            ChkDR( DRM_DDC_SetXMLDeviceCertificateAndKey( NULL,
                                                          poKeyfileContext,
                                                          &g_dstrData_DDC[dwIndex],
                                                          pCrypto ) );
            ChkDR( DRM_KF_StoreToFile (poKeyfileContext) );
            ChkDR( DRM_KF_Close( poKeyfileContext ) );
            
            ChkDR( DRM_DDC_GetXMLDeviceCertificate( NULL, 
                                                    poKeyfileContext, 
                                                    &g_dstrData_DDC[dwIndex], 
                                                    dwFlags,
                                                    pCrypto ) );
        }
        else
        {
            ChkDR( dr );
        }
    }
    
ErrorExit:
    SAFE_OEM_FREE( pCrypto );
    if( poKeyfileContext != NULL )
    {
        if ( poKeyfileContext->hKeyfile != OEM_INVALID_HANDLE_VALUE )
        {
            DRM_KF_Close( poKeyfileContext );
        }
        SAFE_OEM_FREE( poKeyfileContext );
    }
    return dr;
}

/*****************************************************************************
** Function:    Test_API_GetBinaryDeviceCertificate
**
** Synopsis:    Test the api DRM_DDC_GetBinaryDeviceCertificate 
**
** Arguments:   argv[0]: size of the output buffer or NULL, EMPTY
**              argv[1]: dwFlag: 0, 1, 2 or 3 ( DRM_DCP_CREATE_DEVCERT_IF_NOT_EXISTING | DRM_DCP_VERIFY_DEVCERT )
**              argv[2]: crypto context: NORMAL or NULL
**              argv[3]: output buffer index: 0 or 1
**
** Returns:     DRM_SUCCESS          - on success
**
**
******************************************************************************/
DRM_RESULT DRM_CALL Test_API_GetBinaryDeviceCertificate(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT         dr         = DRM_E_TEST_INCOMPLETE;
    DRM_DWORD          dwFlags    = 0;
    DRM_CRYPTO_CONTEXT *pCrypto   = NULL;
    DRM_DWORD          dwIndex    = 0;
    DRM_LONG           i          = 0;
    DRM_KEYFILE_CONTEXT *poKeyfileContext = NULL;

    for( i = 0; i < argc; i++ )
    {
        Log("Trace", "\t\tFrom Test_API_GetBinaryDeviceCertificate: argv[%d]=%s", i, argv[i]);
    }
    
    ChkArg( argc >= 4 );
    ChkArg( argv[1] != NULL );

    if ( argv[0] != NULL ) 
    {
        ChkArg( DRM_SUCCESS == DRMCRT_AtoDWORD( argv[3], DRMCRT_strlen( argv[3] ), 10, &dwIndex ) );
        ChkArg( dwIndex < GDATA_COUNT );

        SAFE_OEM_FREE( g_pbData_DDC[dwIndex] );

        ChkArg( DRM_SUCCESS == DRMCRT_AtoDWORD( argv[0], DRMCRT_strlen( argv[0] ), 10, &g_cbData_DDC[dwIndex] ) );

        if ( g_cbData_DDC[dwIndex] != 0 )
        {
            ChkMem( g_pbData_DDC[dwIndex] = (DRM_BYTE*)Oem_MemAlloc( g_cbData_DDC[dwIndex] * SIZEOF( DRM_WCHAR ) ) );
        }
        else 
        {
            g_pbData_DDC[dwIndex] = NULL;
        }

    }

    ChkArg( DRM_SUCCESS == DRMCRT_AtoDWORD( argv[1], DRMCRT_strlen( argv[1] ), 10, &dwFlags ) );

    if ( argv[2] != NULL )
    {   
        ChkMem(  pCrypto = (DRM_CRYPTO_CONTEXT*)Oem_MemAlloc( SIZEOF( DRM_CRYPTO_CONTEXT ) ) );
        ZEROMEM( pCrypto, SIZEOF( DRM_CRYPTO_CONTEXT ) );
    }
    ChkMem( poKeyfileContext = (DRM_KEYFILE_CONTEXT*) Oem_MemAlloc( SIZEOF(DRM_KEYFILE_CONTEXT) ) );

    ChkDR( DRM_KF_Initialize( NULL, poKeyfileContext ) );
    ChkDR( DRM_KF_Open( poKeyfileContext, TRUE, OEM_GENERIC_READ | OEM_GENERIC_WRITE ) );
    ChkDR( DRM_KF_LoadFromFile( poKeyfileContext, TRUE ) );
    ChkDR( DRM_KF_Close( poKeyfileContext ) );

    dr = DRM_DDC_GetBinaryDeviceCertificate( NULL,
                                             poKeyfileContext,
                                             (argv[0] == NULL )? NULL : g_pbData_DDC[dwIndex], 
                                             ( argv[0] == NULL )? NULL : &g_cbData_DDC[dwIndex], 
                                             dwFlags,
                                             pCrypto );
    if ( DRM_FAILED(dr) )
    {
        if ( ( dwFlags & DRM_DCP_CREATE_DEVCERT_IF_NOT_EXISTING ) == DRM_DCP_CREATE_DEVCERT_IF_NOT_EXISTING )
        {
            /*
            ** In case of small buffer this API overwrites g_cbData_DDC[dwIndex] and returns "buffer too small".
            ** In order to test DRM_DDC_Set...() API with correct buffer size we should reset the value back.
            */
            if ( argv[0] != NULL )
            {
                ChkArg( DRM_SUCCESS == DRMCRT_AtoDWORD( argv[0], DRMCRT_strlen( argv[0] ), 10, &g_cbData_DDC[dwIndex] ) );
            }

            ChkDR( SetDeviceEnv( DEVCERT_TEMPLATE_FILE_NAME, 
                                 PRIVATE_KEY_FILE_NAME, 
                                 GROUP_CERT_FILE_NAME, 
                                 GC_PRIVATE_KEY_FILE_NAME, 
                                 FALSE ) );

             ChkDR( DRM_KF_Open( poKeyfileContext, TRUE,
                                 OEM_GENERIC_READ | OEM_GENERIC_WRITE ) );
             ChkDR( DRM_KF_LoadFromFile( poKeyfileContext, TRUE ) );
             ChkDR( DRM_DDC_SetBinaryDeviceCertificateAndKey( NULL,
                                                              poKeyfileContext,
                                                              g_pbData_DDC[dwIndex],
                                                              &g_cbData_DDC[dwIndex],
                                                              pCrypto ) );      
            ChkDR( DRM_KF_StoreToFile (poKeyfileContext) );
            ChkDR( DRM_KF_Close( poKeyfileContext ) );

            ChkDR( DRM_DDC_GetBinaryDeviceCertificate( NULL,
                                                      poKeyfileContext,
                                                      (argv[0] == NULL )? NULL : g_pbData_DDC[dwIndex], 
                                                      ( argv[0] == NULL )? NULL : &g_cbData_DDC[dwIndex], 
                                                      dwFlags,
                                                      pCrypto ) );

        }
        else
        {
            ChkDR( dr );
        }
    }
    
ErrorExit:
    SAFE_OEM_FREE( pCrypto );
    if( poKeyfileContext != NULL )
    {
        if ( poKeyfileContext->hKeyfile != OEM_INVALID_HANDLE_VALUE )
        {
            DRM_KF_Close( poKeyfileContext );
        }
        SAFE_OEM_FREE( poKeyfileContext );
    }
    return dr;
}


static DRM_STRING* _GetXMLDataBuffer(const char *szArg)
{
    DRM_RESULT  dr      = DRM_E_TEST_INCOMPLETE;
    DRM_STRING  *pOut   = NULL;
    DRM_DWORD   dwIndex = 0;
    DRM_KEYFILE_CONTEXT *poKeyfileContext = NULL;

    if ( szArg != NULL ) 
    {
        if ( DRMCRT_strlen( szArg ) == 7 && !DRMCRT_strncmp( szArg, "Devcert", 7 ) ) 
        { 
            DRM_CRYPTO_CONTEXT cryptoCtx = {0};
            
            /* Read the real devcert */
            FREEDRMSTRING( g_dstrData_DDC[GDATA_DEVCERT] );

            ChkMem(g_dstrData_DDC[GDATA_DEVCERT].pwszString = (DRM_WCHAR*)Oem_MemAlloc( MAX_DEVICE_CERT_SIZE ));
            g_dstrData_DDC[GDATA_DEVCERT].cchString = MAX_DEVICE_CERT_SIZE;

            ChkMem( poKeyfileContext = (DRM_KEYFILE_CONTEXT*) Oem_MemAlloc( SIZEOF(DRM_KEYFILE_CONTEXT) ) );

            ChkDR( DRM_KF_Initialize( NULL, poKeyfileContext ) );
            ChkDR( DRM_KF_Open( poKeyfileContext, TRUE, OEM_GENERIC_READ | OEM_GENERIC_WRITE ) );
            ChkDR( DRM_KF_LoadFromFile( poKeyfileContext, TRUE ) );
            ChkDR( DRM_KF_Close( poKeyfileContext ) );

            ChkDR( DRM_DDC_GetXMLDeviceCertificate( NULL, 
                                                   poKeyfileContext, 
                                                   &g_dstrData_DDC[GDATA_DEVCERT], 
                                                   0,
                                                   &cryptoCtx  ) );
           
            pOut = &g_dstrData_DDC[GDATA_DEVCERT];
        } 
        else 
        {
            ChkArg( DRM_SUCCESS == DRMCRT_AtoDWORD( szArg, DRMCRT_strlen( szArg ), 10, &dwIndex ) );
            ChkArg( dwIndex < GDATA_COUNT );
            pOut = &g_dstrData_DDC[dwIndex];
        }
    }
ErrorExit:
    if( poKeyfileContext != NULL )
    {
        DRM_KF_Uninitialize( poKeyfileContext );
        SAFE_OEM_FREE( poKeyfileContext );
    }
    return pOut;
}
static DRM_RESULT _GetBinaryDataBuffer(const char *szArg, OUT DRM_BYTE **pbOut, OUT DRM_DWORD *cbOut)
{
    DRM_RESULT  dr      = DRM_E_TEST_INCOMPLETE;
    DRM_DWORD   dwIndex = 0;
    DRM_KEYFILE_CONTEXT *poKeyfileContext = NULL;

    ChkArg( cbOut != NULL );
    ChkArg( *pbOut == NULL );

    if ( szArg != NULL ) 
    {
        if ( DRMCRT_strlen( szArg ) == 7 && !DRMCRT_strncmp( szArg, "Devcert", 7 ) ) 
        { 
            DRM_CRYPTO_CONTEXT cryptoCtx = {0};
            /* Read the real devcert */
            SAFE_OEM_FREE( g_pbData_DDC[GDATA_DEVCERT] );
            g_pbData_DDC[GDATA_DEVCERT] = NULL;
            g_cbData_DDC[GDATA_DEVCERT] = 0;

            ChkMem(g_pbData_DDC[GDATA_DEVCERT] = (DRM_BYTE*)Oem_MemAlloc( MAX_DEVICE_CERT_SIZE ) );

            g_cbData_DDC[GDATA_DEVCERT] = MAX_DEVICE_CERT_SIZE;
            ChkMem( poKeyfileContext = (DRM_KEYFILE_CONTEXT*) Oem_MemAlloc( SIZEOF(DRM_KEYFILE_CONTEXT) ) );

            ChkDR( DRM_KF_Initialize( NULL, poKeyfileContext ) );
            ChkDR( DRM_KF_Open( poKeyfileContext, TRUE, OEM_GENERIC_READ | OEM_GENERIC_WRITE ) );
            ChkDR( DRM_KF_LoadFromFile( poKeyfileContext, TRUE ) );
            ChkDR( DRM_KF_Close( poKeyfileContext ) );

            ChkDR( DRM_DDC_GetBinaryDeviceCertificate( NULL,
                                                      poKeyfileContext,
                                                      g_pbData_DDC[GDATA_DEVCERT],
                                                      &g_cbData_DDC[GDATA_DEVCERT], 
                                                      0,
                                                      &cryptoCtx ) );
            
            *pbOut = g_pbData_DDC[GDATA_DEVCERT];
            *cbOut = g_cbData_DDC[GDATA_DEVCERT];
        } 
        else 
        {
            ChkArg( DRM_SUCCESS == DRMCRT_AtoDWORD( szArg, DRMCRT_strlen( szArg ), 10, &dwIndex ) );
            ChkArg( dwIndex < GDATA_COUNT );
            *pbOut = g_pbData_DDC[dwIndex];
            *cbOut = g_cbData_DDC[dwIndex];
        }
    }
    
    dr = DRM_SUCCESS;
    
ErrorExit:
    if( poKeyfileContext != NULL )
    {
        DRM_KF_Uninitialize( poKeyfileContext );
        SAFE_OEM_FREE( poKeyfileContext );
    }
    return dr;
}

DRM_RESULT DRM_CALL Test_CompareXMLDataBuffer(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT dr      = DRM_E_TEST_INCOMPLETE;
    DRM_STRING *pdstr1 = NULL;
    DRM_STRING *pdstr2 = NULL;
    DRM_LONG   i       = 0;

    for( i = 0; i < argc; i++ )
    {
        Log("Trace", "\t\tFrom Test_CompareXMLDataBuffer: argv[%d]=%s", i, argv[i]);
    }
    
    ChkArg(argc == 2);
    pdstr1 = _GetXMLDataBuffer(argv[0]);
    pdstr2 = _GetXMLDataBuffer(argv[1]);
    if ( pdstr1 == NULL ||
         pdstr2 == NULL ||
         pdstr1->cchString != pdstr2->cchString ||
         MEMCMP(pdstr1->pwszString, pdstr2->pwszString, pdstr2->cchString * SIZEOF( DRM_WCHAR ) ) )
    {
        dr = DRM_S_FALSE;
    }
    else
    {
        dr = DRM_SUCCESS;
    }
ErrorExit:
    return dr;
}

DRM_RESULT DRM_CALL Test_CompareBinaryDataBuffer(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT dr       = DRM_E_TEST_INCOMPLETE;
    DRM_BYTE   *pbData1 = NULL;
    DRM_DWORD  cbData1  = 0;
    DRM_BYTE   *pbData2 = NULL;
    DRM_DWORD  cbData2  = 0;
    DRM_LONG   i        = 0;

    for( i = 0; i < argc; i++ )
    {
        Log("Trace", "\t\tFrom Test_CompareBinaryDataBuffer: argv[%d]=%s", i, argv[i]);
    }
    
    ChkArg(argc == 2);
    ChkDR( _GetBinaryDataBuffer( argv[0], &pbData1, &cbData1 ) );
    ChkDR( _GetBinaryDataBuffer( argv[1], &pbData2, &cbData2 ) );

    if ( cbData1 != cbData2 ||
        MEMCMP( pbData1, pbData2, cbData1 ) )
    {
        dr = DRM_S_FALSE;
    }
    else
    {
        dr = DRM_SUCCESS;
    }
ErrorExit:
    return dr;
}


/*****************************************************************************
** Function:    Test_VerifyXMLDevcert
**
** Synopsis:    Verify the devcert in the data buffer 
**
** Arguments:   argv[0]: data buffer index
**
** Returns:     DRM_SUCCESS          - on success
**
**
******************************************************************************/
DRM_RESULT DRM_CALL Test_VerifyXMLDevcert(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT         dr            = DRM_E_TEST_INCOMPLETE;
    DRM_CONST_STRING   dstrDevcert   = EMPTY_DRM_STRING;
    DRM_STRING         *pdstrDevcert = NULL;
    DRM_CRYPTO_CONTEXT *pCrypto      = NULL;
    DRM_LONG           i             = 0;

    for( i = 0; i < argc; i++ )
    {
        Log("Trace", "\t\tFrom Test_VerifyXMLDevcert: argv[%d]=%s", i, argv[i]);
    }

    ChkArg( argc >= 1 );
    ChkArg( argv[0] != NULL );
    
    ChkArg( ( pdstrDevcert = _GetXMLDataBuffer( argv[0] ) ) != NULL );

    /* Verify it*/
    dstrDevcert.pwszString = (const DRM_WCHAR*)pdstrDevcert->pwszString;
    dstrDevcert.cchString = pdstrDevcert->cchString;

    ChkMem( pCrypto = (DRM_CRYPTO_CONTEXT*)Oem_MemAlloc( SIZEOF( DRM_CRYPTO_CONTEXT ) ) );
    ZEROMEM( pCrypto, SIZEOF( DRM_CRYPTO_CONTEXT ) );
    
    ChkDR( DRM_DCP_VerifyDeviceCert( NULL, &dstrDevcert, DRM_DCP_VERIFY_ENTIRE_DEVCERT, NULL, pCrypto ) );
    
    dr = DRM_SUCCESS;
ErrorExit:
    SAFE_OEM_FREE(pCrypto);
    return dr;
}

/*****************************************************************************
** Function:    Test_VerifyXMLDevcert_RobustnessVersion
**
** Synopsis:    Verify the robustness version info of the devcert in the data buffer 
**
** Arguments:   argv[0]: data buffer index
**              argv[1]: expected robustness version
**
** Returns:     DRM_SUCCESS          - on success
**              DRM_S_FALSE          - not matched
**
******************************************************************************/
DRM_RESULT DRM_CALL Test_VerifyXMLDevcert_RobustnessVersion(long argc, __in_ecount(argc) char **argv)
{
#if DRM_BUILD_PROFILE == DRM_BUILD_PROFILE_WM7
    DRM_RESULT        dr                            = DRM_E_TEST_INCOMPLETE;
    DRM_STRING       *pdstrDevcert                  = NULL;
    DRM_CONST_STRING dstrExpectedRobustnessVersion  = EMPTY_DRM_STRING;
    DRM_DWORD        dwExpectedRobustnessVersion    = 0;
    DRM_DWORD        dwCertRobustnessVersion        = 0;
    
    DRM_CONST_STRING    dstrRetVal              = EMPTY_DRM_STRING;

    ChkArg( ( pdstrDevcert = _GetXMLDataBuffer( argv[0] ) ) != NULL );

    ChkDR( MakeDRMString( &dstrExpectedRobustnessVersion, argv[1] ) );

    /* Retrieve the robustness version info */
    ChkDR( DRM_DCP_GetAttribute(
        NULL,
        (DRM_CONST_STRING*) pdstrDevcert,
        DRM_DEVCERT_GROUPSECURITYVERSIONNUM,
        NULL,
        &dstrRetVal ) );

    ChkDR( DRM_UTL_GetVersionFromStringAsDWORD( dstrRetVal.pwszString,
                                                dstrRetVal.cchString,
                                                &dwCertRobustnessVersion ) );

    ChkDR( DRM_UTL_GetVersionFromStringAsDWORD( dstrExpectedRobustnessVersion.pwszString,
                                                dstrExpectedRobustnessVersion.cchString,
                                                &dwExpectedRobustnessVersion ) );
    
    ChkBOOL( dwCertRobustnessVersion == dwExpectedRobustnessVersion, DRM_E_TEST_UNEXPECTED_OUTPUT);
    
ErrorExit:
    return dr;
#else
    return DRM_SUCCESS;
#endif  //endif DRM_BUILD_PROFILE == DRM_BUILD_PROFILE_WM7
    
}


/*****************************************************************************
** Function:    Test_VerifyXMLDevcert_ModelInfo
**
** Synopsis:    Verify the device model info of the devcert in the data buffer 
**
** Arguments:   argv[0]: data buffer index
**              argv[1]: expected model info
**
** Returns:     DRM_SUCCESS          - on success
**              DRM_S_FALSE          - not matched
**
******************************************************************************/
DRM_RESULT DRM_CALL Test_VerifyXMLDevcert_ModelInfo(long argc, __in_ecount(argc) char **argv)
{
#if DRM_BUILD_PROFILE == DRM_BUILD_PROFILE_WM7
    DRM_RESULT          dr                      = DRM_E_TEST_INCOMPLETE;
    DRM_STRING         *pdstrDevcert            = NULL;
    DRM_CONST_STRING    dstrExpectedModelValue  = EMPTY_DRM_STRING;
    DRM_CONST_STRING    dstrRetVal              = EMPTY_DRM_STRING;
    DRM_WCHAR           rgwchDecodedVal[DRM_BCERT_MAX_MANUFACTURER_STRING_LENGTH + 1] = {0};
    DWORD               dwcchDecodedVal         = DRM_BCERT_MAX_MANUFACTURER_STRING_LENGTH;
    DRM_WCHAR           rgwchRealModelInfo[(MAX_PATH*2) + 2] = {0};
    DRM_LONG            i                       = 0;


    DRMASSERT(DRM_MODELREVOCATION_IsModelRevocationSupported());

    for( i = 0; i < argc; i++ )
    {
        Log("Trace", "\t\tFrom Test_VerifyXMLDevcert_ModelInfo: argv[%d]=%s", i, argv[i]);
    }

    ChkArg( argc >= 2 );
    ChkArg( argv[0] != NULL );
    ChkArg( argv[1] != NULL );

    ChkArg( ( pdstrDevcert = _GetXMLDataBuffer( argv[0] ) ) != NULL );

    ChkDR( MakeDRMString( &dstrExpectedModelValue, argv[1] ) );
    if( 0 == MEMCMP(dstrExpectedModelValue.pwszString, L"RealModelInfo", dstrExpectedModelValue.cchString * SIZEOF(DRM_WCHAR)) )
    {
        DRM_WCHAR   rgwchOemModel       [ MAX_PATH + 1 ] = {0};
        DRM_WCHAR   rgwchOemManufacturer[ MAX_PATH + 1 ] = {0};
        
        FREEDRMSTRING( dstrExpectedModelValue );
        
        ChkWR( SystemParametersInfoW( SPI_GETPLATFORMNAME, 
                                      SIZEOF(rgwchOemModel), 
                                      rgwchOemModel, 
                                      0 ) );
                                      
        ChkWR( SystemParametersInfoW( SPI_GETPLATFORMMANUFACTURER, 
                                      SIZEOF( rgwchOemManufacturer ), 
                                      rgwchOemManufacturer, 
                                      0 ) );
        /*
        **  First the model name, followed by a comma, then the manufacturer name
        */
        MEMCPY( rgwchRealModelInfo, 
                rgwchOemModel,
                DRMCRT_wcslen( rgwchOemModel ) * SIZEOF(DRM_WCHAR) );

        rgwchRealModelInfo[ DRMCRT_wcslen( rgwchOemModel ) ] = g_wchComma;
        
        MEMCPY( rgwchRealModelInfo + DRMCRT_wcslen( rgwchOemModel ) + 1, 
                rgwchOemManufacturer, 
                DRMCRT_wcslen( rgwchOemManufacturer ) * SIZEOF(DRM_WCHAR) );

        dstrExpectedModelValue.pwszString = rgwchRealModelInfo;
        dstrExpectedModelValue.cchString = min( DRMCRT_wcslen( dstrExpectedModelValue.pwszString ),
                                                 DRM_BCERT_MAX_MANUFACTURER_STRING_LENGTH / SIZEOF(DRM_WCHAR) );    
    }

    // Retrieve the model info
    ChkDR(DRM_DCP_GetAttribute(
        NULL,
        (DRM_CONST_STRING*) pdstrDevcert,
        DRM_DEVCERT_DEVICEMODELNAME,
        NULL,
        &dstrRetVal));   

    // Model info retrieved is XML encoded, need to decode:
    ChkDR(DRM_UTL_XMLDecode(dstrRetVal.pwszString, dstrRetVal.cchString, rgwchDecodedVal, &dwcchDecodedVal));

    /* Compare the decoded return value to the expected value */
    if ( dstrExpectedModelValue.cchString != dwcchDecodedVal 
        || 0 != MEMCMP(rgwchDecodedVal, dstrExpectedModelValue.pwszString, dwcchDecodedVal * SIZEOF( DRM_WCHAR )) ) 
    {
        Log("UNEXPECTEDVALUE", "Retrieved=%S,Expecting=%S", rgwchDecodedVal, dstrExpectedModelValue.pwszString);
        dr = DRM_S_FALSE; /* Not matched! */
        goto ErrorExit;
    }

    dr = DRM_SUCCESS;
ErrorExit:
    return dr;

#else
    return DRM_SUCCESS;
#endif  //endif DRM_BUILD_PROFILE == DRM_BUILD_PROFILE_WM7

}

/*****************************************************************************
** Function:    Test_VerifyBinaryDevcert
**
** Synopsis:    Verify the binary devcert in the data buffer 
**
** Arguments:   argv[0]: data buffer index
**              argv[1]: Optional, Security Version
**              argv[2]: Optional, Platform ID
**              argv[3]: Optional, Expected model info
**
** Returns:     DRM_SUCCESS          - on success
**
**
******************************************************************************/

DRM_RESULT DRM_CALL Test_VerifyBinaryDevcert(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT              dr                   = DRM_E_TEST_INCOMPLETE;
    DRM_BYTE                *pbData              = NULL;
    DRM_DWORD               cbData               = 0;
    DRM_DWORD               bCurrOffset          = 0;
    DRM_LONG                i                    = 0;
    DRM_DWORD               dw                   = 0;
    DRM_DWORD               iUsage               = 0;
    DRM_BCERT_CERTIFICATE   oDeviceCert          = {0};
    DRM_BCERT_CHAIN_HEADER  ChainHeader          = {0};
    DRM_CRYPTO_CONTEXT     *poCryptoContext      = NULL;
    DRM_CONST_STRING        dstrSecurityVersion  = EMPTY_DRM_STRING;
    DRM_DWORD               dwExpectedSecVersion = 0;
    DRM_DWORD               dwExpectedPlatformID = 0;
    DRM_BOOL                f_isChkSecVersion    = FALSE;
    DRM_CONST_STRING        dstrExpModelInfo     = EMPTY_DRM_STRING;
    DRM_BOOL                f_isChkModelInfo     = FALSE;
    DRM_BOOL                f_fSecClockFlag     = FALSE;
    DRM_BOOL                f_fAntirollFlag     = FALSE;
    DRM_BOOL                f_fSignFlag         = FALSE;
    DRM_BOOL                f_fEncryptFlag      = FALSE;
    DRM_BOOL                fRevocationFlag     = FALSE;
    DRM_BCERT_VERIFICATIONCONTEXT oVerificationContext = {0};
    DRMFILETIME             ftCurrentTime       = {0};
    /*
    ** If both secure clock and antirollback clock are
    ** enabled, we expect only secure clock to be in the cert
    ** since certificates can only support one clock type
    */
    if( DRM_ARCLK_IsAntirollbackClockSupported() && DRM_CLK_IsSecureClockSupported() )
    {
        f_fAntirollFlag     = TRUE;
    }

    for( i = 0; i < argc; i++ )
    {
        Log("Trace", "\t\tFrom Test_VerifyBinaryDevcert: argv[%d]=%s", i, argv[i]);
    }

    ChkArg( argc == 1 || argc >= 3 );
    ChkArg( argv[0] != NULL );

    // Check if we need to verify security version:
    if( argc >= 3 && argv[1] != NULL && argv[2] != NULL)
    {
        f_isChkSecVersion = TRUE;

        ChkDR( MakeDRMString( &dstrSecurityVersion, argv[1] ) );
        ChkDR( DRMCRT_AtoDWORD( argv[2], DRMCRT_strlen( argv[2] ), 0, &dwExpectedPlatformID ) );
        ChkDR( DRM_UTL_GetVersionFromStringAsDWORD( dstrSecurityVersion.pwszString,
                                                    dstrSecurityVersion.cchString,
                                                    &dwExpectedSecVersion ) );
    }

    // Check if we need to verify the device model info:
    if( argc >= 4 && argv[3] != NULL)
    {
#if DRM_BUILD_PROFILE == DRM_BUILD_PROFILE_WM7
        DRM_WCHAR rgwchRealModelInfo[(MAX_PATH*2) + 2] = {0};
        f_isChkModelInfo = TRUE;

        DRMASSERT(DRM_MODELREVOCATION_IsModelRevocationSupported());
        
        ChkDR( MakeDRMString( &dstrExpModelInfo, argv[3] ) );
        if( 0 == MEMCMP(dstrExpModelInfo.pwszString, L"RealModelInfo", dstrExpModelInfo.cchString * SIZEOF(DRM_WCHAR)) )
        {
            DRM_WCHAR   rgwchOemModel       [ MAX_PATH + 1 ] = {0};
            DRM_WCHAR   rgwchOemManufacturer[ MAX_PATH + 1 ] = {0};
            
            FREEDRMSTRING( dstrExpModelInfo );
            
            ChkWR( SystemParametersInfoW( SPI_GETPLATFORMNAME, 
                                          SIZEOF(rgwchOemModel), 
                                          rgwchOemModel, 
                                          0 ) );
                                          
            ChkWR( SystemParametersInfoW( SPI_GETPLATFORMMANUFACTURER, 
                                          SIZEOF( rgwchOemManufacturer ), 
                                          rgwchOemManufacturer, 
                                          0 ) );

            /*
            **  First the model name, followed by a comma, then the manufacturer name
            */
            MEMCPY( rgwchRealModelInfo, 
                    rgwchOemModel,
                    DRMCRT_wcslen( rgwchOemModel ) * SIZEOF(DRM_WCHAR) );
            
            rgwchRealModelInfo[ DRMCRT_wcslen( rgwchOemModel ) ] = g_wchComma;
            
            MEMCPY( rgwchRealModelInfo + DRMCRT_wcslen( rgwchOemModel ) + 1, 
                    rgwchOemManufacturer, 
                    DRMCRT_wcslen( rgwchOemManufacturer ) * SIZEOF(DRM_WCHAR) );
                    
            dstrExpModelInfo.pwszString = rgwchRealModelInfo;
            dstrExpModelInfo.cchString = DRMCRT_wcslen( dstrExpModelInfo.pwszString );    
        }
#endif  //endif DRM_BUILD_PROFILE == DRM_BUILD_PROFILE_WM7
    }

    ChkMem( poCryptoContext = (DRM_CRYPTO_CONTEXT*)Oem_MemAlloc( SIZEOF( DRM_CRYPTO_CONTEXT ) ) );
    ZEROMEM( poCryptoContext, SIZEOF( DRM_CRYPTO_CONTEXT ) );
    
    ChkDR( _GetBinaryDataBuffer( argv[0], &pbData, &cbData ) );

    /* 
    ** Validate the cert chain
    */
    Oem_Clock_GetSystemTimeAsFileTime( NULL, &ftCurrentTime );

    ChkDR( DRM_BCert_InitVerificationContext( &ftCurrentTime,
                                              (const PUBKEY_P256*) g_rgbMSPlayReadyRootIssuerPubKey,
                                              DRM_BCERT_CERTTYPE_DEVICE,
                                              poCryptoContext,
                                              TRUE,
                                              FALSE,
                                              NULL,
                                              0,
                                              FALSE,
                                              NULL,
                                              NULL,
                                              NULL,
                                              &oVerificationContext ) );

    ChkDR( DRM_BCert_ParseCertificateChain( pbData, 
                                            cbData, 
                                            &oVerificationContext ) );
    /* 
    ** Skip the chain header to get to the individual certs 
    */
    ChkDR( DRM_BCert_GetChainHeader( pbData,
                                     cbData,
                                     &bCurrOffset,
                                     &ChainHeader ) );
    

    /* 
    ** Load leaf cert into a cert sturcture 
    */
    ChkDR( DRM_BCert_GetCertificate( pbData,
                                     cbData,
                                     &bCurrOffset,
                                     &oDeviceCert,
                                     DRM_BCERT_CERTTYPE_DEVICE ) );

    /*
    ** Verify that the leaf cert contains the expected features
    */    
    for ( dw = 0; dw < oDeviceCert.FeatureInfo.dwNumFeatureEntries; dw++ )
    {
        switch ( oDeviceCert.FeatureInfo.rgdwFeatureSet[dw] )
        {
        case DRM_BCERT_FEATURE_SECURE_CLOCK:
            /* 
            ** Check that this build of the device supports secure clock
            ** and that the secure clock feature flag has not already
            ** been found
            */
            if( DRM_CLK_IsSecureClockSupported() )
            {
                ChkBOOL( !f_fSecClockFlag, DRM_E_TEST_UNEXPECTED_OUTPUT );
                f_fSecClockFlag     = TRUE;
            }
            else
            {
                ChkDR(DRM_E_TEST_UNEXPECTED_OUTPUT);
            }
            break;
        case DRM_BCERT_FEATURE_ANTIROLLBACK_CLOCK:
            /* 
            ** Check that this build of the device supports antirollback clock
            ** and that the antirollback clock feature flag has not already
            ** been found
            */
            if( DRM_ARCLK_IsAntirollbackClockSupported() )
            {
                ChkBOOL( !f_fAntirollFlag, DRM_E_TEST_UNEXPECTED_OUTPUT );
                f_fAntirollFlag     = TRUE;
            }
            else
            {
                ChkDR(DRM_E_TEST_UNEXPECTED_OUTPUT);
            }
            break;
        case DRM_BCERT_FEATURE_SUPPORTS_CRLS:
            ChkBOOL( DRM_REVOCATION_IsRevocationSupported(), DRM_E_TEST_UNEXPECTED_OUTPUT );
            fRevocationFlag = TRUE;
            break;
        default:
            /* 
            ** Other values are invalid
            */
            ChkDR(DRM_E_TEST_UNEXPECTED_OUTPUT);
            break;
        }
    }
    /* 
    ** Verify that all expected features were found 
    */
    if( DRM_CLK_IsSecureClockSupported() )
    {
        ChkBOOL( f_fSecClockFlag, DRM_E_TEST_UNEXPECTED_OUTPUT );
    }
    if( DRM_ARCLK_IsAntirollbackClockSupported() )
    {
        ChkBOOL( f_fAntirollFlag, DRM_E_TEST_UNEXPECTED_OUTPUT );
    }
    ChkBOOL( !DRM_REVOCATION_IsRevocationSupported() || fRevocationFlag, DRM_E_TEST_UNEXPECTED_OUTPUT );

    /*
    ** Verify that the leaf cert contains the expected key usages
    */    
    for ( dw = 0; dw < oDeviceCert.KeyInfo.dwNumKeys; dw++ )
    {
        for ( iUsage = 1; iUsage <= DRM_BCERT_KEYUSAGE_MAX_VALUE; iUsage++ )
        {
            if ( ( ( oDeviceCert.KeyInfo.rgoKeys[dw].dwUsageSet ) & BCERT_KEYUSAGE_BIT( iUsage ) ) > 0 )
            {
                switch ( iUsage )
                {
                    case DRM_BCERT_KEYUSAGE_SIGN:
                        /* 
                        ** Check that the sign usage flag has not already
                        ** been found
                        */
                        ChkBOOL( !f_fSignFlag, DRM_E_TEST_UNEXPECTED_OUTPUT );
                        f_fSignFlag     = TRUE;
                        break;
                    case DRM_BCERT_KEYUSAGE_ENCRYPT_KEY:
                        /* 
                        ** Check that the encrypt usage flag has not already
                        ** been found
                        */
                        ChkBOOL( !f_fEncryptFlag, DRM_E_TEST_UNEXPECTED_OUTPUT );
                        f_fEncryptFlag     = TRUE;
                        break;
                    default:
                        /* 
                        ** Other values are invalid for devices
                        */
                    ChkDR(DRM_E_TEST_UNEXPECTED_OUTPUT);
                    break;
                }
            }
        }
    }
    /* 
    ** Verify that all expected usages were found 
    */
    ChkBOOL( f_fSignFlag, DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkBOOL( f_fEncryptFlag, DRM_E_TEST_UNEXPECTED_OUTPUT );

    if( f_isChkSecVersion )
    {
        ChkBOOL( oDeviceCert.SecurityVersion.dwPlatformIdentifier == dwExpectedPlatformID, 
                 DRM_E_TEST_UNEXPECTED_OUTPUT );
        ChkBOOL( oDeviceCert.SecurityVersion.dwSecurityVersion    == dwExpectedSecVersion, 
                 DRM_E_TEST_UNEXPECTED_OUTPUT );
    }

    if( f_isChkModelInfo )
    {
        ChkBOOL( MEMCMP(oDeviceCert.ManufacturerInfo.ManufacturerStrings.ModelName.rgb, dstrExpModelInfo.pwszString, dstrExpModelInfo.cchString * SIZEOF( DRM_WCHAR )) == 0,
                 DRM_E_TEST_UNEXPECTED_OUTPUT );
    }
    
    dr = DRM_SUCCESS;
ErrorExit:
    SAFE_OEM_FREE(poCryptoContext);
    return dr;
}

DRM_RESULT DRM_CALL MODULE_PRETESTCASE(DeviceDevCert)(long lTCID, const char *strTCName)
{
    DRM_LONG i;
    for (i = 0; i < GDATA_TOTAL_COUNT; i++)
    {
        g_dstrData_DDC[i].pwszString = NULL;
        g_dstrData_DDC[i].cchString = 0;
        g_pbData_DDC[i] = NULL;
        g_cbData_DDC[i] = 0;
    }

    return Test_RemoveDeviceData( 0, NULL );
}

DRM_RESULT DRM_CALL MODULE_POSTTESTCASE(DeviceDevCert)(long lTCID, const char *strTCName)
{
    DRM_LONG i;
    for (i = 0; i < GDATA_TOTAL_COUNT; i++)
    {
        FREEDRMSTRING( g_dstrData_DDC[i] );
        SAFE_OEM_FREE( g_pbData_DDC[i] );
    }
    g_dwPlatformID = 0;
    g_dwWMRobustnessVersion = 0;
    g_dwPRRobustnessVersion = 0;
    
    return DRM_SUCCESS;
}

BEGIN_TEST_API_LIST( DeviceDevCert )
    API_ENTRY(Test_RemoveDeviceData)
    API_ENTRY(Test_SetDeviceEnv)
    API_ENTRY(Test_InitWithOemData)
    API_ENTRY(Test_CompareXMLDataBuffer)
    API_ENTRY(Test_CompareBinaryDataBuffer)
    API_ENTRY(Test_VerifyXMLDevcert)
    API_ENTRY(Test_VerifyXMLDevcert_RobustnessVersion)
    API_ENTRY(Test_VerifyXMLDevcert_ModelInfo)
    API_ENTRY(Test_VerifyBinaryDevcert)
    API_ENTRY(Test_API_GetDeviceCertificate)
    API_ENTRY(Test_API_GetBinaryDeviceCertificate)
    API_ENTRY(Test_SetRobustnessVersion)
END_TEST_API_LIST

EXIT_PKTEST_NAMESPACE_CODE

