/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdafx.h>
#include <drmblackbox.h>
#include <oempkcrypto.h>
#include <drmheaderparser.h>
#include <drmlicacq.h>
#include <oemcipher.h>
#include <drmviewrights.h>
#include <drmrevocation.h>
#include <drmchain.h>
#include <drmsecureclock.h>
#include <drmsyncstore.h>
#include <drmsecurestore.h>
#include <drmlicreason.h>
#include <devicedevcert.h>
#include <drmantirollbackclock.h>
#include <drmwmdrm.h>
#include <drmliceval.h>

#include <ViewRightsTestAPI.h>
#include <drmantirollbackclock.h>

PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_USE_WIDE_API, "Ignore prefast warning about ANSI functions for this entire file.")

ENTER_PKTEST_NAMESPACE_CODE

/*global variables*/
DRM_BB_CONTEXT g_oBBContext = {0};
DRM_SECSTORE_CONTEXT g_oGlobalSecStoreContext, g_oLIDSecStoreContext, g_oContentRevSecStoreContext;
DRM_LICSTORE_CONTEXT g_oLicStoreContext;
DRM_LICSTORE_CONTEXT g_oXMRLicStoreContext;

#if DRM_MAX_LICENSE_CHAIN_DEPTH != 2
#err "This only supports max chain depth of 2"
#endif

DRM_LICSTOREENUM_CONTEXT g_oLicEnumContext;
DRM_LICSTOREENUM_CONTEXT g_oLicEnumContext2;
#pragma prefast(suppress:22112) /* ignore warning about predictable address with function pointer */
DRM_LICEVAL_CONTEXT g_oLicEvalContext;
DRM_VIEW_RIGHTS_CONTEXT g_oViewRightsContext;
DRM_REVOCATIONSTORE_CONTEXT g_oRevContext;
static DRM_DST *g_pDatastore = NULL;

DRM_LICENSE_STATE_DATA    *g_pStateData = NULL;
DRM_DWORD g_dwStateData = 0;

#define MAX_SANDBOX_SIZE 10000

const DRM_WCHAR g_dxcvstrDRM_VERSION_STRINGCREATEDRMSTRING [] = {  ONE_WCHAR('2', '\0'), ONE_WCHAR('.', '\0'), ONE_WCHAR('0', '\0'), ONE_WCHAR('.', '\0'), ONE_WCHAR('0', '\0'), ONE_WCHAR('.', '\0'), ONE_WCHAR('0', '\0'), ONE_WCHAR('\0', '\0') };
const DRM_CONST_STRING g_dxcvstrDRM_VERSION_STRING = CREATE_DRM_STRING( g_dxcvstrDRM_VERSION_STRINGCREATEDRMSTRING );

static void CleanGlobalStateData()
{
    SAFE_OEM_FREE(g_pStateData);
    g_pStateData = NULL;
    g_dwStateData = 0;
}

static DRM_RESULT SetupViewRightsContext(DRM_VIEW_RIGHTS_CONTEXT *pVRContext)
{
    DRM_RESULT dr;
    DRM_SUBSTRING dasstrKID = { 0 };
    DRM_DWORD cbKID = DRM_ID_SIZE;
    DRM_BYTE szSecStorePassword[DRM_SHA1_DIGEST_LEN];
    DRM_STRING dstrDevCert;
    DRM_BYTE *pbBuffer = NULL;

    ZEROMEM( pVRContext, SIZEOF( DRM_VIEW_RIGHTS_CONTEXT ) );

    ChkMem( pbBuffer = (DRM_BYTE*) Oem_MemAlloc( MAX_DEVICE_CERT_SIZE ) );
    ZEROMEM( pbBuffer, SIZEOF(MAX_DEVICE_CERT_SIZE) );

    DSTR_FROM_PB( &dstrDevCert, pbBuffer, MAX_DEVICE_CERT_SIZE );
    DRMASSERT( g_oBBContext.pKeyFileContext != NULL && g_oBBContext.pKeyFileContext->fLoaded ); /* keyfile initialized in pretestcase */
    if( DRM_WMDRM_IsWMDRMSupported() )
    {
        ChkDR( DRM_DDC_GetXMLDeviceCertificate(
            NULL,
            g_oBBContext.pKeyFileContext,
            &dstrDevCert,
            0,
            &g_oBBContext.CryptoContext ) );
    }

    /* Use the default KID from our test licgen */
    dasstrKID.m_ich = 0;
    dasstrKID.m_cch = ( DRM_DWORD ) strlen(szLicGenDefaultKID);

    ChkDR(DRM_B64_DecodeA(szLicGenDefaultKID, &dasstrKID, &cbKID, pVRContext->KID.rgb, 0));

    /* Initialize a blackbox context */
    ChkDR( DRM_BBX_Initialize( NULL, &g_oBBContext ) );

    if( DRM_WMDRM_IsWMDRMSupported() )
    {
        ChkDR( DRM_DCP_LoadPropertiesCache(
            NULL,
            (DRM_CONST_STRING*) &dstrDevCert,
            NULL,
            &g_oBBContext.CryptoContext,
            &g_oBBContext.cachedCertValues ) );
    }

    ChkDR( DRM_BBX_TestDeviceKeyPair( &g_oBBContext ) );

    pVRContext->pBBContext = &g_oBBContext;

    /* Talk to blackbox to get the global secure store password */
    ChkDR(DRM_SST_CreateGlobalStorePassword(NULL, szSecStorePassword, (DRM_BYTE*)&g_oBBContext));

    /* Open the global key */
    ChkDR(DRM_SST_OpenKeyTokens(&g_oGlobalSecStoreContext, &g_rgbSecStoreGlobalName, NULL,
        szSecStorePassword, DRM_SECURE_STORE_OPEN_ALWAYS, SECURE_STORE_GLOBAL_DATA,
        g_pDatastore));
    pVRContext->pbGlobalSecStore = &g_oGlobalSecStoreContext;

    /* Set the LID secure store context */
    pVRContext->pbLIDSecStore = &g_oLIDSecStoreContext;

    /* Set the licstore (XML) and licstore enum contexts */
    ChkDR(DRM_LST_Open(&g_oLicStoreContext, g_pDatastore, eDRM_LICENSE_STORE_XML));
    ChkDR(DRM_LST_Open(&g_oXMRLicStoreContext, g_pDatastore, eDRM_LICENSE_STORE_XMR));

    pVRContext->pbLicenseStoreXML = &g_oLicStoreContext;
    pVRContext->pbLicenseStoreXMR = &g_oXMRLicStoreContext;
    pVRContext->rgpLicQueryContext[0] = &g_oLicEnumContext;
    pVRContext->rgpLicQueryContext[1] = &g_oLicEnumContext2;

    if( DRM_REVOCATION_IsRevocationSupported() )
    {
        /* Setup the revocation store context */
        ChkDR( DRM_RVS_InitRevocationStore(  g_pDatastore,
                                                &g_oContentRevSecStoreContext,
                                                &g_oBBContext,
                                                &g_oRevContext ) );
    }

    /* Set the liceval context */
    memset(&g_oLicEvalContext, 0, sizeof(DRM_LICEVAL_CONTEXT));
    if( DRM_REVOCATION_IsRevocationSupported() )
    {
        g_oLicEvalContext.pContextRev = &g_oRevContext;
    }
    g_oLicEvalContext.pcontextBBX = &g_oBBContext;
    g_oLicEvalContext.pcontextSSTRevocation = &g_oContentRevSecStoreContext;
    g_oLicEvalContext.pcontextSSTLicense = &g_oLIDSecStoreContext;
    g_oLicEvalContext.pcontextSSTGlobal = &g_oGlobalSecStoreContext;
    g_oLicEvalContext.certinfoSDK.appSec = 2000; /* hardcode here */
    g_oLicEvalContext.fAppInfoValid = FALSE;
    g_oLicEvalContext.fLegacyAppInfoValid = FALSE;
    if( DRM_REVOCATION_IsRevocationSupported() )
    {
        g_oLicEvalContext.cbRevocationBuffer = REVOCATION_BUFFER_SIZE;
        ChkMem(g_oLicEvalContext.pbRevocationBuffer = (DRM_BYTE*)Oem_MemAlloc(g_oLicEvalContext.cbRevocationBuffer));
    }
    else
    {
        g_oLicEvalContext.cbRevocationBuffer = 0;
        g_oLicEvalContext.pbRevocationBuffer = NULL;
    }

    if( DRM_ARCLK_IsAntirollbackClockSupported() )
    {
        /* Set machine clock to the secure store. Otherwise CheckClockRollback will fail. */
        ChkDR( DRM_ARCLK_LIC_ResetClockRollback( &g_oLicEvalContext ) );
    }

    pVRContext->pLicEval = &g_oLicEvalContext;

    /* Set the sandbox buffer */
    pVRContext->cbBuffer = MAX_SANDBOX_SIZE;
    ChkMem(pVRContext->pbBuffer = (DRM_BYTE*)Oem_MemAlloc(MAX_SANDBOX_SIZE));

ErrorExit:
    SAFE_OEM_FREE( pbBuffer );
    return dr;
}

static DRM_RESULT _GenerateOffsetSystemTime( __inout_ecount(18) char* szSystemTimeString, long loffset)
{
    DRM_RESULT    dr = DRM_SUCCESS;
    DRMSYSTEMTIME tSysTime   = { 0 };
    DRMFILETIME   mcFileTime = { 0 };
    DRM_UINT64    u64        = { 0 };
    DRM_WORD      wTemp      = 0;

    ChkArg(szSystemTimeString != NULL);
    ChkArg(loffset != 0);

    Oem_Clock_GetSystemTimeAsFileTime(NULL, &mcFileTime);

    /*
    ** offset the file time
    */
    FILETIME_TO_UI64(mcFileTime, u64);
    if(loffset > 0)
    {
        u64 = DRM_UI64Add(u64, DRM_UI64Mul(DRM_UI64(loffset), DRM_UI64(FILETIME_TO_SECONDS)));
    }
    else
    {
        u64 = DRM_UI64Sub(u64, DRM_UI64Mul(DRM_UI64(-loffset), DRM_UI64(FILETIME_TO_SECONDS)));
    }
    UI64_TO_FILETIME(u64, mcFileTime);

    /*
    ** convert back to system time from file time
    */
    ChkArg(Oem_Clock_FileTimeToSystemTime(&mcFileTime, &tSysTime));

    /*
    ** e.g. 20090101
    */
    wTemp = tSysTime.wYear;
    PUT_CHAR( szSystemTimeString, 3, (DRM_BYTE)TST_UTL_ByteValueToASCII( (DRM_BYTE)(wTemp % 10) ) );
    wTemp /= 10;
    PUT_CHAR( szSystemTimeString, 2, (DRM_BYTE)TST_UTL_ByteValueToASCII( (DRM_BYTE)(wTemp % 10) ) );
    wTemp /= 10;
    PUT_CHAR( szSystemTimeString, 1, (DRM_BYTE)TST_UTL_ByteValueToASCII( (DRM_BYTE)(wTemp % 10) ) );
    wTemp /= 10;
    PUT_CHAR( szSystemTimeString, 0, (DRM_BYTE)TST_UTL_ByteValueToASCII( (DRM_BYTE)(wTemp % 10) ) );

    wTemp = tSysTime.wMonth;
    PUT_CHAR( szSystemTimeString, 5, (DRM_BYTE)TST_UTL_ByteValueToASCII( (DRM_BYTE)(wTemp % 10) ) );
    wTemp /= 10;
    PUT_CHAR( szSystemTimeString, 4, (DRM_BYTE)TST_UTL_ByteValueToASCII( (DRM_BYTE)(wTemp % 10) ) );

    wTemp = tSysTime.wDay;
    PUT_CHAR( szSystemTimeString, 7, (DRM_BYTE)TST_UTL_ByteValueToASCII( (DRM_BYTE)(wTemp % 10) ) );
    wTemp /= 10;
    PUT_CHAR( szSystemTimeString, 6, (DRM_BYTE)TST_UTL_ByteValueToASCII( (DRM_BYTE)(wTemp % 10) ) );

    PUT_CHAR( szSystemTimeString, 8, '\0' );

ErrorExit:
    return dr;
}


/*
    Add a license to the store
    argv[0]: dwOption value to testLicGen
    argv[1]: extra xml string to add to the license, can be NULL
    argv[2]: optional LID value
    argv[3]: optional KID value
    argv[4]: optional License Type
    argv[5]: optional Uplink KID value
    argv[6]: optional offset time in hours
*/
DRM_RESULT DRM_CALL TestAddLicense(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT dr;
    DRM_CONST_STRING szLic;

    DRM_CHAR szExtraXML[2000]={0};
    DRM_DWORD cbResponseContext = 20000, cbResponse;
    _XMBContext *pbResponseContext = NULL;
    DRM_BYTE *pbResponse = NULL;
    DRM_BYTE *pbLicenseBuffer = NULL;
    DRM_SYNC_CONTEXT *pcontextSYN = NULL;
    ELicenseType eLicType = MAKE_SIMPLE_LICENSE;
    DRM_SYNC_CONTEXT contextSYN;

    if( DRM_SNC_IsLicenseSyncSupported() )
    {
        /*
        ** Ignore errors from DRM_SNC_OpenStore
        */
        pcontextSYN = &contextSYN;
        (DRM_VOID)DRM_SNC_OpenStore( g_pDatastore, pcontextSYN );
    }

    ChkArg(argc >= 2 && argv[0]);

    /* Generate the test license */

    ChkDR(TestLicGenInit(20000));
    if (argc > 2 && argv[2] != NULL )
    {
        ChkDR(TestLicGenSetKeys("LICENSE_LID", argv[2]));
    }

    if (argc>3 && argv[3] != NULL )
    {
        ChkDR(TestLicGenSetKeys("LICENSE_KID",argv[3]));
    }

    if(argc>4 && argv[4] != NULL )
    {
        ChkArg(argv[4]);
        eLicType = (ELicenseType)atol(argv[4]);
    }

    if (argc > 5 && argv[5] != NULL )
    {
        ChkDR(TestLicGenSetKeys("LICENSE_UPLINK_KID",argv[5]));
    }

    if (argc > 6 && argv[6] != NULL )
    {
        char        pSystemTime[20];
        const char *pszXMLEnding  = "Z#:0]]></ACTION></ONSTORE>";
        long        lOffset       = 0;
        DRM_DWORD   cchSystemTime = 0;
        DRM_DWORD   cchXMLEnding  = ( DRM_DWORD ) strlen( pszXMLEnding );
        DRM_DWORD   cchArg        = 0;

        ChkArg(argv[6]);
        lOffset=atol(argv[6])*60*60;
        ChkDR( _GenerateOffsetSystemTime( pSystemTime , lOffset) );

        cchSystemTime = ( DRM_DWORD ) strlen( pSystemTime );
        cchArg        = ( DRM_DWORD ) strlen( argv[1] );
        ChkBOOL( SIZEOF( szExtraXML ) >= ( cchSystemTime + cchArg + cchXMLEnding + 1 ), DRM_E_FAIL );

        DRM_BYT_CopyBytes( szExtraXML, 0, argv[1], 0, cchArg );
        DRM_BYT_CopyBytes( szExtraXML, cchArg, pSystemTime, 0, cchSystemTime );
        DRM_BYT_CopyBytes( szExtraXML, cchArg + cchSystemTime, pszXMLEnding, 0, cchXMLEnding);
        TRACE((szExtraXML));
    }
    else if( argv[1] != NULL ) {
        MEMCPY(szExtraXML, argv[1], min( SIZEOF( szExtraXML ), ( DRM_DWORD ) strlen( argv[1] ) + 1 ) );
    }

    ChkDR( TestLicGenMakeLicense( (DRM_DWORD)atoi( argv[0] ),
                                  argv[1]?szExtraXML:NULL,
                                  NULL,
                                  eLicType,
                                  &szLic,
                                  NULL,
                                  FALSE) );

    LogBlob( "TestAddLicense", "XML License:", (const DRM_BYTE*)(szLic.pwszString), szLic.cchString * SIZEOF(DRM_WCHAR) );

    /* Build a response string */
    ChkMem(pbResponseContext = (_XMBContext*)Oem_MemAlloc(cbResponseContext));
    ChkMem(pbLicenseBuffer   = (DRM_BYTE*)Oem_MemAlloc(DRM_MAX_LICENSESIZE) );
    ChkDR(TestResponseInit(pbResponseContext, cbResponseContext));
    ChkDR(TestResponseAddLicense(pbResponseContext, &szLic));
    ChkDR(TestResponseGetResponse(pbResponseContext, &pbResponse, &cbResponse));

    g_oLicEvalContext.fLicenseIsXMR = FALSE;
    /* Process the response string. This adds the license to the licstore and secstore */
    ChkDR( DRM_WMDRM_LA_ProcessResponse( pbResponse,
                                         cbResponse,
                                         &g_oLicEvalContext,
                                         &g_oLicStoreContext,
                                         &g_oXMRLicStoreContext,
                                         &g_oLicEnumContext,
                                         NULL,
                                         NULL,
                                         NULL,
                                         g_pDatastore,
                                         &g_oViewRightsContext,
                                         pbLicenseBuffer,
                                         pcontextSYN,
                                         NULL ) );

ErrorExit:
    if( DRM_SNC_IsLicenseSyncSupported() && pcontextSYN != NULL )
    {
        /*
        ** Ignore errors from DRM_SNC_CloseStore
        */
        (DRM_VOID)DRM_SNC_CloseStore(pcontextSYN);
    }
    TestLicGenShutDown();
    SAFE_OEM_FREE(pbResponseContext);
    SAFE_OEM_FREE(pbLicenseBuffer);
    SAFE_OEM_FREE(pbResponse);
    return dr;
}

/* Test API IsAllowed
    argv[0]: action string
    argv[1]: ViewRightsContext: Normal or NULL
*/
DRM_RESULT DRM_CALL TestIsAllowed(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_CONST_STRING wszAction = EMPTY_DRM_STRING;

    ChkArg(argc == 2);
    ChkDR( MakeDRMString(&wszAction, argv[0]) );
    ChkDR(DRM_VIEWRIGHTS_IsAllowed(&wszAction, argv[1]? &g_oViewRightsContext: NULL, g_pDatastore));
ErrorExit:
    FREEDRMSTRING( wszAction );
    return dr;
}

/*
** Test API GetLicenseAggregateData
**  argv[0]: Status of pwszAction: Normal or NULL
**  argv[1]: Status of pStateData: Normal or NULL
**  argv[2]: number of actions in pwszAction
**  argv[3]: Status of viewrights context:: Normal or NULL
**  argv[4]: Aggregation type: DRM_ASD_AGGREGATE_ROOT_LICENSES or DRM_ASD_AGGREGATE_SIMPLE_AND_LEAF_LICENSES
**  argv[5]...: Action strings to fill in pwszAction
*/
DRM_RESULT DRM_CALL TestGetLicenseAggregateData( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT         dr          = DRM_SUCCESS;
    DRM_CONST_STRING **ppwszAction = NULL;
    DRM_DWORD          i           = 0;
    DRM_DWORD          dwAggType   = DRM_ASD_AGGREGATE_ROOT_LICENSES;

    ChkArg( argc >= 5 ); /* At least 5 args are required */
    ChkArg( argv[2] != NULL );
    ChkArg( argv[4] != NULL );

    CleanGlobalStateData();

    /*
    ** Set up the actions
    */
    g_dwStateData = strtoul( argv[2], NULL, 0 );

    if ( argv[0] != NULL )
    {
        ChkMem( ppwszAction = ( DRM_CONST_STRING** )Oem_MemAlloc( SIZEOF( DRM_CONST_STRING * ) * g_dwStateData ) );
        ZEROMEM( ppwszAction, sizeof(DRM_CONST_STRING*) * g_dwStateData );

        for ( i = 0; i < g_dwStateData; i++ )
        {
            ChkArg( argc > 5 + ( DRM_LONG ) i );
            ChkMem( ppwszAction[i] = ( DRM_CONST_STRING * )Oem_MemAlloc( SIZEOF( DRM_CONST_STRING ) ) );
            ChkDR( MakeDRMString( ppwszAction[i], argv[5 + i] ) );
        }
    }

    if ( argv[1] != NULL )
    {
        ChkMem( g_pStateData = ( DRM_LICENSE_STATE_DATA * )Oem_MemAlloc( SIZEOF( DRM_LICENSE_STATE_DATA ) * g_dwStateData ) );
    }

    /*
    ** Parse aggregation type
    */
    if ( strncmp( argv[4], "DRM_ASD_AGGREGATE_ROOT_LICENSES", 31 ) != 0 )
    {
        if ( strncmp( argv[4], "DRM_ASD_AGGREGATE_SIMPLE_AND_LEAF_LICENSES", 42 ) == 0 )
        {
            dwAggType = DRM_ASD_AGGREGATE_SIMPLE_AND_LEAF_LICENSES;
        }
        else
        {
            ChkArg( FALSE );
        }
    }

    ChkDR( DRM_VIEWRIGHTS_GetLicenseAggregateData( ppwszAction,
                                                   argv[1] != NULL? g_pStateData: NULL,
                                                   g_dwStateData,
                                                   argv[3] != NULL? &g_oViewRightsContext: NULL,
                                                   g_pDatastore,
                                                   FALSE,
                                                   dwAggType,
                                                   NULL,
                                                   NULL ) );

ErrorExit:
    if ( ppwszAction != NULL )
    {
        for ( i = 0; i < g_dwStateData; i++ )
        {
            if ( ppwszAction[i] != NULL )
            {
                FREEDRMSTRINGPOINTER( ppwszAction[i] );
                SAFE_OEM_FREE( ppwszAction[i] );
            }
        }
    }
    SAFE_OEM_FREE( ppwszAction );
    return dr;
}

/***************************************************************************************************
** Function:    TestVerifyStateData
**
** Synopsis:    Verify values in g_pStateData returned from GetLicenseAggregateData in a previous
**              test API
**
** Arguments:   argv[0]: the index in g_pStateData to verify
**              argv[1]: the expected value of dwCategory
**              argv[2]: (optional) expected value of dwVague, could be NULL
**              argv[3]: (optional) expected number of counts
**              argv[4]: (optional) expected value of the first count
**
** Returns:     DRM_SUCCESS                  - on success
**              DRM_E_TEST_UNEXPECTED_OUTPUT - if the state data does not match
**                                             the expected value
**
** Notes:       Must be called after TestGetLicenseAggregateData, which sets the state data values
**              which this test API verifies.
**
***************************************************************************************************/
DRM_RESULT DRM_CALL TestVerifyStateData( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT dr      = DRM_E_TEST_INCOMPLETE;
    DRM_DWORD  dwIndex = 0;
    DRM_LONG   i       = 0;

    ChkArg( argc    > 1 );
    ChkArg( argv[0] != NULL );
    ChkArg( argv[1] != NULL );

    ChkBOOL( g_pStateData != NULL, DRM_E_TEST_UNEXPECTED_OUTPUT );

    dwIndex = strtoul( argv[0], NULL, 0 );

    ChkArg( dwIndex < g_dwStateData );

    /*
    ** Verify DRM_LICENSE_STATE_CATEGORY
    */
    ChkBOOL( strtoul( argv[1], NULL, 0 ) == g_pStateData[dwIndex].dwCategory,
             DRM_E_TEST_UNEXPECTED_OUTPUT );

    /*
    ** Verify the dwVague
    */
    if ( argc > 2 && argv[2] != NULL )
    {
        ChkBOOL( strtoul( argv[2], NULL, 0 ) == g_pStateData[dwIndex].dwVague,
                 DRM_E_TEST_UNEXPECTED_OUTPUT  );
    }

    /*
    ** Verify the number of counts
    */
    if ( argc > 3 && argv[3] != NULL )
    {
        ChkBOOL( strtoul( argv[3], NULL, 0 ) == g_pStateData[dwIndex].dwNumCounts,
                 DRM_E_TEST_UNEXPECTED_OUTPUT  );
    }

    /*
    ** Verify the four count values
    */
    for ( i = 4; i < argc && i < 8; i++ )
    {
        ChkBOOL( strtoul( argv[i], NULL, 0 ) ==  g_pStateData[dwIndex].dwCount[i - 4],
                 DRM_E_TEST_UNEXPECTED_OUTPUT  );
    }

    dr = DRM_SUCCESS;

ErrorExit:
    return dr;
}

DRM_RESULT DRM_CALL MODULE_PRETESTCASE(ViewRights)(long lTCID, const char *strTCName)
{
    DRM_RESULT dr;
    CLIENTID clientID;
    DRM_CONST_STRING dstrHdsPath;
    DRM_KEYFILE_CONTEXT *poKeyfileContext = NULL;
    DRM_WCHAR wszPathName[GET_DRM_FILE_NAME_PATH_LENGTH] = {'\0'};

    ChkDR(SetDeviceEnv(DEVCERT_TEMPLATE_FILE_NAME,
                       PRIVATE_KEY_FILE_NAME,
                       GROUP_CERT_FILE_NAME,
                       GC_PRIVATE_KEY_FILE_NAME,
                       TRUE));
    if( DRM_WMDRM_IsWMDRMSupported() )
    {
        ChkDR(TestGetClientID(&clientID));
        ChkDR(TestLicResponseSetClientID(&clientID));
    }
    RemoveDRMFile(RMFILE_STORE);
    ChkDR( tGetDeviceStorePathname(wszPathName, &dstrHdsPath) );
    ChkDR(OpenHDS(&g_pDatastore, dstrHdsPath.pwszString, TRUE));

    /*
    ** Set pointer to keyfile context in the blackbox
    */
    ChkMem( poKeyfileContext = (DRM_KEYFILE_CONTEXT*) Oem_MemAlloc( SIZEOF(DRM_KEYFILE_CONTEXT) ) );

    ChkDR( DRM_KF_Initialize( NULL, poKeyfileContext ) );
    g_oBBContext.pKeyFileContext = poKeyfileContext;
    ChkDR( DRM_KF_Open( poKeyfileContext, TRUE, OEM_GENERIC_READ ) );
    ChkDR( DRM_KF_LoadFromFile( poKeyfileContext, TRUE ) );
    ChkDR( DRM_BBX_SetupSecureStoreKey( &g_oBBContext, NULL ) );
    ChkDR( DRM_KF_Close( poKeyfileContext ) );

    ChkDR(SetupViewRightsContext(&g_oViewRightsContext));
ErrorExit:
    return dr;
}

DRM_RESULT DRM_CALL MODULE_POSTTESTCASE(ViewRights)(long lTCID, const char *strTCName)
{
    DRM_SST_CloseKey(&g_oGlobalSecStoreContext, g_pDatastore, NULL, NULL);
    DRM_LST_Close(&g_oLicStoreContext);
    DRM_LST_Close(&g_oXMRLicStoreContext);
    SAFE_OEM_FREE(g_oLicEvalContext.pbRevocationBuffer);
    SAFE_OEM_FREE(g_oViewRightsContext.pbBuffer);
    CleanGlobalStateData();

    DRM_KF_Uninitialize( g_oBBContext.pKeyFileContext );
    SAFE_OEM_FREE( g_oBBContext.pKeyFileContext );

    DRM_BBX_Shutdown( &g_oBBContext );

    CloseHDS(g_pDatastore);
    RemoveDRMFile(RMFILE_STORE);
    return DRM_SUCCESS;
}

BEGIN_TEST_API_LIST(ViewRights)
    API_ENTRY(TestGetLicenseAggregateData)
    API_ENTRY(TestIsAllowed)
    API_ENTRY(TestAddLicense)
    API_ENTRY(TestVerifyStateData)
END_TEST_API_LIST

EXIT_PKTEST_NAMESPACE_CODE

PREFAST_POP /* ignore prefast warning about ANSI functions for this entire file  */

