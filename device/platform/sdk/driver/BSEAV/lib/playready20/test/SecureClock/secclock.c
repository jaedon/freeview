/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdafx.h>
#include <SecClockTestAPI.h>
#include <drmsecureclock.h>
#include <drmsecureclockconstants.h>
#include <drmwmdrm.h>

ENTER_PKTEST_NAMESPACE_CODE

static DRM_APP_CONTEXT *g_pManagerContext = NULL;

static const DRM_WCHAR  g_rgwchTIDOpen  [] = { ONE_WCHAR('<', '\0'), ONE_WCHAR('T', '\0'), ONE_WCHAR('I', '\0'), ONE_WCHAR('D', '\0'), ONE_WCHAR('>', '\0'), ONE_WCHAR('\0', '\0') }; /* "<TID>" */
static const DRM_WCHAR  g_rgwchTIDClose [] = { ONE_WCHAR('<', '\0'), ONE_WCHAR('/', '\0'), ONE_WCHAR('T', '\0'), ONE_WCHAR('I', '\0'), ONE_WCHAR('D', '\0'), ONE_WCHAR('>', '\0'), ONE_WCHAR('\0', '\0') }; /* "</TID>" */

DRM_RESULT DRM_CALL Test_API_MGR_Initialize(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT dr;

    ChkArg(!g_pManagerContext);

    GET_SHARED_APP_CONTEXT( g_pManagerContext );
ErrorExit:
    return dr;
}

static DRM_RESULT iGetSecStorePassword(DRM_BYTE *rgbPassword)
{
    DRM_RESULT dr;
    DRM_BB_CONTEXT *pBlackboxContext = NULL;
    DRM_BYTE rgbBuffer[MAX_DEVICE_CERT_SIZE];
    DRM_CONST_STRING dstrDevCert;

    DSTR_FROM_PB( &dstrDevCert, rgbBuffer, SIZEOF(rgbBuffer) );

    /* Get devcert to open the blackbox */
    ChkMem(pBlackboxContext = (DRM_BB_CONTEXT*)Oem_MemAlloc(SIZEOF(DRM_BB_CONTEXT)));
    ZEROMEM( pBlackboxContext, SIZEOF(DRM_BB_CONTEXT) );

    ChkMem( pBlackboxContext->pKeyFileContext = ( DRM_KEYFILE_CONTEXT * ) Oem_MemAlloc( SIZEOF( DRM_KEYFILE_CONTEXT ) ) );
    ChkDR( DRM_KF_Initialize( NULL, pBlackboxContext->pKeyFileContext ) );
    ChkDR( DRM_KF_Open( pBlackboxContext->pKeyFileContext, TRUE, OEM_GENERIC_READ ) );
    ChkDR( DRM_KF_LoadFromFile( pBlackboxContext->pKeyFileContext, TRUE ) );
    ChkDR( DRM_BBX_SetupSecureStoreKey( pBlackboxContext->pKeyFileContext, NULL ) );
    ChkDR( DRM_KF_Close( pBlackboxContext->pKeyFileContext ) );

    if( DRM_WMDRM_IsWMDRMSupported() )
    {
        ChkDR( TST_UTL_SetWMDRMCertCachedValues( pBlackboxContext ) );
    }
    ChkDR(DRM_BBX_Initialize(NULL, pBlackboxContext));

    ChkDR(DRM_SST_CreateGlobalStorePassword(rgbPassword, (DRM_BYTE*)pBlackboxContext));
ErrorExit:
    if (pBlackboxContext != NULL)
    {
        DRM_BBX_Shutdown(pBlackboxContext);

        DRM_KF_Uninitialize(pBlackboxContext->pKeyFileContext);
        SAFE_OEM_FREE( pBlackboxContext->pKeyFileContext );

    }
    SAFE_OEM_FREE(pBlackboxContext);
    return dr;
}


static DRM_RESULT _SetSecureClockData(DRM_SECSTORE_CLOCK_DATA *pClock, DRM_DST *pDST)
{
    DRM_RESULT dr;
    DRM_SECSTORE_CONTEXT *pSecStoreContext = NULL;
    DRM_BYTE rgbSecStoreInPswd[DRM_SHA1_DIGEST_LEN];

    if( DRM_CLK_IsSecureClockSupported() )
    {
        if (!pDST)
        {
            ChkArg(g_pManagerContext);
            pDST = &((DRM_APP_CONTEXT_INTERNAL*)g_pManagerContext)->oDatastoreHDS;
        }

        ChkMem(pSecStoreContext = (DRM_SECSTORE_CONTEXT*)Oem_MemAlloc(sizeof(DRM_SECSTORE_CONTEXT)));
        ChkDR(iGetSecStorePassword(rgbSecStoreInPswd));
        ChkDR( DRM_SST_SetData( pSecStoreContext,
                                &g_idSData,
                                &g_idSDataPrev,
                                rgbSecStoreInPswd,
                                SECURE_STORE_GLOBAL_DATA,
                                pDST,
                                (DRM_BYTE*)pClock,
                                sizeof(DRM_SECSTORE_CLOCK_DATA) ) );
    }
    else
    {
        dr = DRM_E_NOTIMPL;
    }

ErrorExit:
    SAFE_OEM_FREE(pSecStoreContext);
    return dr;
}

/*    Sets the secure clock data directly
    argv[0]: validity flag
*/
DRM_RESULT DRM_CALL Test_SetSecureClock(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT dr;
    DRM_SECSTORE_CLOCK_DATA oClock;
    DRMSYSTEMTIME oSystemTime;
    DRM_DST *pDST = NULL;
    DRM_CONST_STRING dstrHdsPath;
    DRM_WCHAR wszPathName[GET_DRM_FILE_NAME_PATH_LENGTH] = {'\0'};

    ChkArg(argc > 0 && argv[0]);

    oClock.flag = (DRM_WORD)atol(argv[0]);

    Oem_Clock_GetSystemTime(NULL, &oSystemTime);
    ChkArg(Oem_Clock_SystemTimeToFileTime(&oSystemTime, (DRMFILETIME*)&(oClock.LastKnownRealtimeSecureClock)));

    ChkDR( tGetDeviceStorePathname(wszPathName, &dstrHdsPath) );
    ChkDR(OpenHDS(&pDST, dstrHdsPath.pwszString, FALSE));

    ChkDR(_SetSecureClockData(&oClock, pDST));

ErrorExit:
    CloseHDS(pDST);
    return dr;
}

static DRM_RESULT _GetSecureClockData(DRM_SECSTORE_CLOCK_DATA *pClock)
{
    DRM_RESULT dr;
    DRM_SECSTORE_CONTEXT *pSecStoreContext = NULL;
    DRM_BYTE rgbSecStoreInPswd[DRM_SHA1_DIGEST_LEN];
    DRM_DWORD cbClock;

    if( DRM_CLK_IsSecureClockSupported() )
    {
        ChkArg(pClock && g_pManagerContext);
        ChkMem(pSecStoreContext = (DRM_SECSTORE_CONTEXT*)Oem_MemAlloc(sizeof(DRM_SECSTORE_CONTEXT)));
        ChkDR(iGetSecStorePassword(rgbSecStoreInPswd));

        cbClock = sizeof(DRM_SECSTORE_CLOCK_DATA);
        ChkDR(DRM_SST_GetData(pSecStoreContext,
                            &g_idSData,
                            &g_idSDataPrev,
                            rgbSecStoreInPswd,
                            SECURE_STORE_GLOBAL_DATA,
                            &((DRM_APP_CONTEXT_INTERNAL*)g_pManagerContext)->oDatastoreHDS,
                            (DRM_BYTE*)pClock,
                            &cbClock));
    }
    else
    {
        dr = DRM_E_NOTIMPL;
    }

ErrorExit:
    SAFE_OEM_FREE(pSecStoreContext);
    return dr;
}

static DRM_RESULT _VerifyTIDInChallenge(DRM_ANSI_STRING *pdastrChallenge)
{
    DRM_RESULT dr;
    DRM_SECSTORE_CLOCK_DATA oClock;
    DRM_DWORD cb = pdastrChallenge->cchString;
    DRM_CONST_STRING dstr   = EMPTY_DRM_STRING;
    DRM_SUBSTRING    dasstr = { 0 };
    DRM_WCHAR       *pStart = NULL,
                    *pStop  = NULL;

    dasstr.m_ich = 0;
    dasstr.m_cch = pdastrChallenge->cchString;

    /* Decode the challenge string*/
    ChkDR(DRM_B64_DecodeA(pdastrChallenge->pszString, &dasstr, &cb, NULL, DRM_BASE64_DECODE_IN_PLACE));

    dstr.pwszString = (DRM_WCHAR *) (pdastrChallenge->pszString + dasstr.m_ich);
    ((DRM_WCHAR *)(dstr.pwszString))[cb/SIZEOF(DRM_WCHAR)] = 0;

    /* Get the TID within <TID></TID> */

    ChkArg( ( pStart = (DRM_WCHAR*)DRMCRT_wcsnstr(dstr.pwszString, DRMCRT_wcslen( dstr.pwszString ), g_rgwchTIDOpen, DRMCRT_wcslen( g_rgwchTIDOpen ) ) ) != NULL );
    pStart += 5;
    dstr.pwszString = pStart;
    ChkArg( ( pStop = (DRM_WCHAR*)DRMCRT_wcsnstr(dstr.pwszString, DRMCRT_wcslen( dstr.pwszString ), g_rgwchTIDClose, DRMCRT_wcslen( g_rgwchTIDClose ) ) ) != NULL );
    dstr.cchString = (DRM_DWORD)(pStop - pStart);

    /* Decode the TID string */
    cb = dstr.cchString;
    ChkDR(DRM_B64_DecodeW(&dstr, &cb, NULL, DRM_BASE64_DECODE_IN_PLACE));
    ChkDR(_GetSecureClockData(&oClock));

    ChkArg(cb == sizeof(DRM_TID) && !memcmp(&(oClock.tid), dstr.pwszString, cb));
ErrorExit:
    return dr;
}

/*    Test API MGR_ClkGenerateChallenge
    argv[0]: status of DRM manager context: NULL or NORMAL
    argv[1]: Check returned URL? TRUE or FALSE
    argv[2]: status of the return challenge: NORMAL or NULL
*/
DRM_RESULT DRM_CALL Test_API_MGR_ClkGenerateChallenge(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT dr;
    DRM_CONST_STRING wszURL = {0};
    DRM_CONST_STRING wszURLFromDevcert;
    DRM_CONST_STRING *pwszURLToVerify = NULL;
    DRM_ANSI_STRING szChallenge = {0};
    DRM_CONST_STRING dstrDevCert = EMPTY_DRM_STRING;

    if( DRM_CLK_IsSecureClockSupported() )
    {
        ChkArg(argc == 3 && argv[1]);
        dr = Drm_SecureClock_GenerateChallenge(argv[0]? g_pManagerContext: NULL,
            NULL, argv[1]? &wszURL.cchString: NULL,
            NULL, argv[2]? &szChallenge.cchString: NULL);
        if (dr != DRM_E_BUFFERTOOSMALL)
        {
            ChkDR(dr);
            ChkDR( DRM_E_TEST_UNEXPECTED_OUTPUT );
        }

        if (argv[1])
        {
            ChkMem(wszURL.pwszString = (DRM_WCHAR*)Oem_MemAlloc(wszURL.cchString * sizeof(DRM_WCHAR)));
        }
        if (argv[2])
        {
            ChkMem(szChallenge.pszString = (DRM_CHAR*)Oem_MemAlloc(szChallenge.cchString * sizeof(DRM_CHAR)));
        }
        ChkDR(Drm_SecureClock_GenerateChallenge(argv[0]? g_pManagerContext: NULL,
            (DRM_WCHAR*)wszURL.pwszString, argv[1]? &wszURL.cchString: NULL,
            (DRM_BYTE*)szChallenge.pszString, argv[2]? &szChallenge.cchString: NULL));

        if (!strcmp(argv[1], "TRUE"))
        {
            /* Verify the returned secure clock service URL */
            ChkArg(wszURL.pwszString);

            if( DRM_WMDRM_IsWMDRMSupported() )
            {
                ChkDR( TST_UTL_GetWMDRMCert(
                    (DRM_STRING*)&dstrDevCert,
                    &((DRM_APP_CONTEXT_INTERNAL*)g_pManagerContext)->oBlackBoxContext.CryptoContext ) );

                ChkDR(DRM_DCP_GetAttribute(NULL, &dstrDevCert, DRM_DEVCERT_SECURECLOCKURL, NULL, &wszURLFromDevcert));
                pwszURLToVerify = &wszURLFromDevcert;
            }
            else
            {
                pwszURLToVerify = (DRM_CONST_STRING*)&g_dstrDefaultPlayReadySecureClockServerUrl;
            }

            /* The returned URL includes a NULL terminating char. */
            if (wszURL.cchString > pwszURLToVerify->cchString)
            {
                wszURL.cchString = pwszURLToVerify->cchString;
            }

            if (!DRM_UTL_DSTRStringsEqual(&wszURL, pwszURLToVerify))
            {
                Log("Trace", "The returned SecureClock URL doesn't match the expected one (in devcert or fixed)");
                ChkDR(DRM_E_FAIL);
            }
        }

        /* Verify TID in the challenge string matches the one in the secstore */
        ChkDR(_VerifyTIDInChallenge(&szChallenge));
    }
    else
    {
        dr = DRM_E_NOTIMPL;
    }

ErrorExit:
    FREEDRMSTRING( wszURL );
    SAFE_OEM_FREE(szChallenge.pszString);
    FREEDRMSTRING( dstrDevCert );
    return dr;
}

/* Sets up the secure clock data after a challenge.
    argv[0]: TID in base64 string
    argv[1]: Challenge time in base64 string
*/
DRM_RESULT DRM_CALL Test_SetChallengeState(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT dr;
    DRM_SECSTORE_CLOCK_DATA oClock;
    DRM_SUBSTRING           dasstrData;
    DRM_DWORD cb;
    DRMFILETIME ft;
    DRMSYSTEMTIME st,UpdatedTime;

    ChkArg(argc == 2 && argv[0] && argv[1]);

    memset(&oClock, 0, sizeof(DRM_SECSTORE_CLOCK_DATA));

    /* Set TID */
    cb = sizeof(DRM_TID);
    dasstrData.m_ich = 0;
    dasstrData.m_cch = ( DRM_DWORD ) strlen(argv [0]);

    ChkDR(DRM_B64_DecodeA(argv [0], &dasstrData, &cb, (DRM_BYTE *) &oClock.tid, 0));

    /* Set Challenge time */
    cb = sizeof(DRM_UINT64);
    dasstrData.m_ich = 0;
    dasstrData.m_cch = ( DRM_DWORD ) strlen(argv [1]);

    ChkDR(DRM_B64_DecodeA(argv [1], &dasstrData, &cb, (DRM_BYTE *) &oClock.ChallengeTime, 0));

    oClock.flag = CLK_TID_VALID | CLK_CHALLENGETIME_VALID;

    ChkDR(_SetSecureClockData(&oClock, NULL));

    /* Set the machine clock to the challenge time */
    UI64_TO_FILETIME(oClock.ChallengeTime, ft);
    ChkArg(Oem_Clock_FileTimeToSystemTime(&ft, &st));
    Oem_Clock_SetSystemTime(NULL, &st);
    Oem_Clock_GetSystemTime(NULL, &UpdatedTime);
    /*The following if statement is to make sure the system time is updated correctly
    even when we cross between the DST boundaries (when the current date is between
    October and April)*/
    if(UpdatedTime.wHour != st.wHour)
    {
        Oem_Clock_SetSystemTime(NULL, &st);
    }

ErrorExit:
    return dr;
}

/*    Test API MGR_ClkProcessResponse
    argv[0]: response file name
    argv[1]: status of drm manager context, NORMAL or NULL
    argv[2]: status of response string, NORMAL or NULL
    argv[3]: expected return code from the response or NULL
    argv[4]: optional flag to modify the response: BREAK_SIGNATURE
*/
DRM_RESULT DRM_CALL Test_API_MGR_ClkProcessResponse(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD cbData = 0;
    DRM_BYTE *pbData = NULL;
    DRM_RESULT drResult = DRM_E_FAIL;

    if( DRM_CLK_IsSecureClockSupported() )
    {
        ChkArg(argc >= 4);
        ChkArg(LoadTestFile(NULL, argv[0], &pbData, &cbData));

        if (argc > 4 && argv[4])
        {
            if (!strcmp(argv[4], "BREAK_CERT"))
            {
                pbData[cbData / 2]++;
            }
            else if (!strcmp(argv[4], "BREAK_SIGNATURE"))
            {
                ChkArg(cbData > 248);
                pbData[248]++;
            }
        }

        ChkDR(Drm_SecureClock_ProcessResponse(argv[1]? g_pManagerContext: NULL,
            argv[2]? pbData: NULL, cbData,
            argv[3]? &drResult: NULL));

        if (argv[3] && (drResult != (DRM_RESULT)atol(argv[3])))
        {
            Log("Trace", "Secure time response code is %d. Expecting %s", drResult, argv[2]);
            dr = DRM_S_FALSE;
        }
    }
    else
    {
        dr = DRM_E_NOTIMPL;
    }

ErrorExit:
    SAFE_OEM_FREE(pbData);
    return dr;
}

/*
    argv[0]: amount to change the machine clock in minutes
*/
DRM_RESULT DRM_CALL Test_ChangeClock(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg(argc == 1 && argv[0]);
    ChkDR(tChangeSystemTime(atol(argv[0]) * 60));
ErrorExit:
    return dr;
}

/*    Test API MGR_GetSecureClock
    argv[0]: status of drm manager context, NORMAL or NULL
    argv[1]: status of the return string data, NORMAL or NULL
    argv[2]: status of the return xml data, NORMAL or NULL
*/
DRM_RESULT DRM_CALL Test_API_MGR_GetSecureClock(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT dr;
    DRM_WCHAR *wszString = NULL;
    DRM_BYTE *pbXMLData = NULL;
    DRM_DWORD cchString = 0;
    DRM_DWORD cbXMLData = 0;
    DRM_DWORD dwFlag = 0;

    if( DRM_CLK_IsSecureClockSupported() )
    {
        ChkArg(argc == 3);

        dr = Drm_SecureClock_GetValue(argv[0]? g_pManagerContext: NULL,
            NULL, argv[1]? &cchString: NULL, argv[1]? &dwFlag: NULL,
            NULL, argv[2]? &cbXMLData: NULL);
        if (dr != DRM_E_BUFFERTOOSMALL)
        {
            ChkDR(dr);
            ChkDR( DRM_E_TEST_UNEXPECTED_OUTPUT );
        }
        if (argv[1])
        {
            PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_ALLOC_SIZE_OVERFLOW, "ChkOverflow checks for arithmetic but Prefast still warns.")
            ChkOverflow( cchString * sizeof(DRM_WCHAR), cchString );
            ChkMem(wszString = (DRM_WCHAR*)Oem_MemAlloc(cchString * sizeof(DRM_WCHAR)));
            PREFAST_POP
        }
        if (argv[2])
        {
            PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_ALLOC_SIZE_OVERFLOW, "ChkOverflow checks for arithmetic but Prefast still warns.")
            ChkOverflow( cbXMLData * sizeof(DRM_BYTE), cbXMLData );
            ChkMem(pbXMLData = (DRM_BYTE*)Oem_MemAlloc(cbXMLData * sizeof(DRM_BYTE)));
            PREFAST_POP
        }
        ChkDR(Drm_SecureClock_GetValue(argv[0]? g_pManagerContext: NULL,
            argv[1]? wszString: NULL, argv[1]? &cchString: NULL, argv[1]? &dwFlag: NULL,
            argv[2]? pbXMLData: NULL, argv[2]? &cbXMLData: NULL));
    }
    else
    {
        dr = DRM_E_NOTIMPL;
    }

ErrorExit:
    SAFE_OEM_FREE(wszString);
    SAFE_OEM_FREE(pbXMLData);
    return dr;
}

DRM_RESULT DRM_CALL MODULE_PRETESTCASE(SecClock)(long lTCID, const char *strTCName)
{
    g_pManagerContext = NULL;
    return DRM_SUCCESS;
}

DRM_RESULT DRM_CALL MODULE_POSTTESTCASE(SecClock)(long lTCID, const char *strTCName)
{
    tResetSystemTimeOffset();
    return DRM_SUCCESS;
}

BEGIN_TEST_API_LIST(SecClock)
    API_ENTRY(Test_API_MGR_Initialize)
    API_ENTRY(Test_API_MGR_ClkGenerateChallenge)
    API_ENTRY(Test_API_MGR_ClkProcessResponse)
    API_ENTRY(Test_API_MGR_GetSecureClock)
    API_ENTRY(Test_SetChallengeState)
    API_ENTRY(Test_SetSecureClock)
    API_ENTRY(Test_ChangeClock)
END_TEST_API_LIST

EXIT_PKTEST_NAMESPACE_CODE
