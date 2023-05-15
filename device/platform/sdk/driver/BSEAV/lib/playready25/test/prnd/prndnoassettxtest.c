/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdafx.h>
#include <prndnoassettxtest.h>
#include <prndprotocoltest.h>

ENTER_PKTEST_NAMESPACE_CODE;

#define TEST_VAR_PRND_NOASSETTX_CMDDATA         "PrndNoAssetTxCmdData"
#define TEST_VAR_PRND_NOASSETTX_CUSTOMDATA      "PrndNoAssetTxCustomData"

/* Note: Must match file name in prndnoassettx.c */
DRM_WCHAR   g_rgwchHds[]    =  { WCHAR_CAST('t'), WCHAR_CAST('m'), WCHAR_CAST('p'), WCHAR_CAST('\0') };

/* L"bgroupcert.dat.bak" */
DRM_WCHAR   g_rgwchGroupCertBak[]   =  { WCHAR_CAST('b'), WCHAR_CAST('g'), WCHAR_CAST('r'), WCHAR_CAST('o'), WCHAR_CAST('u'), WCHAR_CAST('p'), WCHAR_CAST('c'), WCHAR_CAST('e'), WCHAR_CAST('r'), WCHAR_CAST('t'), WCHAR_CAST('.'), WCHAR_CAST('d'), WCHAR_CAST('a'), WCHAR_CAST('t'), WCHAR_CAST('.'), WCHAR_CAST('b'), WCHAR_CAST('a'), WCHAR_CAST('k'), WCHAR_CAST('\0') };
/* L"bgroupcert.dat" */
DRM_WCHAR   g_rgwchGroupCert[]      =  { WCHAR_CAST('b'), WCHAR_CAST('g'), WCHAR_CAST('r'), WCHAR_CAST('o'), WCHAR_CAST('u'), WCHAR_CAST('p'), WCHAR_CAST('c'), WCHAR_CAST('e'), WCHAR_CAST('r'), WCHAR_CAST('t'), WCHAR_CAST('.'), WCHAR_CAST('d'), WCHAR_CAST('a'), WCHAR_CAST('t'), WCHAR_CAST('\0') };
/* L"zgpriv.dat.bak" */
DRM_WCHAR   g_rgwchZgprivBak[]      =  { WCHAR_CAST('z'), WCHAR_CAST('g'), WCHAR_CAST('p'), WCHAR_CAST('r'), WCHAR_CAST('i'), WCHAR_CAST('v'), WCHAR_CAST('.'), WCHAR_CAST('d'), WCHAR_CAST('a'), WCHAR_CAST('t'), WCHAR_CAST('.'), WCHAR_CAST('b'), WCHAR_CAST('a'), WCHAR_CAST('k'), WCHAR_CAST('\0') };
/* L"zgpriv.dat" */
DRM_WCHAR   g_rgwchZgpriv[]         =  { WCHAR_CAST('z'), WCHAR_CAST('g'), WCHAR_CAST('p'), WCHAR_CAST('r'), WCHAR_CAST('i'), WCHAR_CAST('v'), WCHAR_CAST('.'), WCHAR_CAST('d'), WCHAR_CAST('a'), WCHAR_CAST('t'), WCHAR_CAST('\0') };
/* L"devcerttemplate.dat.bak" */
DRM_WCHAR   g_rgwchDevCertTemBak[]  =  { WCHAR_CAST('d'), WCHAR_CAST('e'), WCHAR_CAST('v'), WCHAR_CAST('c'), WCHAR_CAST('e'), WCHAR_CAST('r'), WCHAR_CAST('t'), WCHAR_CAST('t'), WCHAR_CAST('e'), WCHAR_CAST('m'), WCHAR_CAST('p'), WCHAR_CAST('l'), WCHAR_CAST('a'), WCHAR_CAST('t'), WCHAR_CAST('e'), WCHAR_CAST('.'), WCHAR_CAST('d'), WCHAR_CAST('a'), WCHAR_CAST('t'), WCHAR_CAST('.'), WCHAR_CAST('b'), WCHAR_CAST('a'), WCHAR_CAST('k'), WCHAR_CAST('\0') };
/* L"devcerttemplate.dat" */
DRM_WCHAR   g_rgwchDevCertTem[]     =  { WCHAR_CAST('d'), WCHAR_CAST('e'), WCHAR_CAST('v'), WCHAR_CAST('c'), WCHAR_CAST('e'), WCHAR_CAST('r'), WCHAR_CAST('t'), WCHAR_CAST('t'), WCHAR_CAST('e'), WCHAR_CAST('m'), WCHAR_CAST('p'), WCHAR_CAST('l'), WCHAR_CAST('a'), WCHAR_CAST('t'), WCHAR_CAST('e'), WCHAR_CAST('.'), WCHAR_CAST('d'), WCHAR_CAST('a'), WCHAR_CAST('t'), WCHAR_CAST('\0') };
/* L"priv.dat.bak" */
DRM_WCHAR   g_rgwchPrivBak[]        =  { WCHAR_CAST('p'), WCHAR_CAST('r'), WCHAR_CAST('i'), WCHAR_CAST('v'), WCHAR_CAST('.'), WCHAR_CAST('d'), WCHAR_CAST('a'), WCHAR_CAST('t'), WCHAR_CAST('.'), WCHAR_CAST('b'), WCHAR_CAST('a'), WCHAR_CAST('k'), WCHAR_CAST('\0') };
/* L"priv.dat" */
DRM_WCHAR   g_rgwchPriv[]           =  { WCHAR_CAST('p'), WCHAR_CAST('r'), WCHAR_CAST('i'), WCHAR_CAST('v'), WCHAR_CAST('.'), WCHAR_CAST('d'), WCHAR_CAST('a'), WCHAR_CAST('t'), WCHAR_CAST('\0') };

DRM_RESULT DRM_CALL MODULE_PRETESTCASE(prndnoassettxtest)(long lTCID, const char *strTCName)
{
          DRM_RESULT   dr                         = DRM_SUCCESS;
          DRM_VOID   **ppvAsyncShellCommandData   = NULL;
          DRM_BYTE    *pbCustomData               = NULL;
          DRM_BOOL     fNoAssetTxAvailable        = TRUE;
    const DRM_CHAR    *pszPRNDNoAssetTxAvailable  = "PRNDNoAssetTxAvailable";

    ChkDR( TST_UTL_IsConditionTrue( &fNoAssetTxAvailable, 1, &pszPRNDNoAssetTxAvailable ) );
    if ( fNoAssetTxAvailable )
    {
        (DRM_VOID)tRemoveFileW( g_rgwchHds );

        MAP_TO_GLOBAL_TEST_VAR_Alloc_And_Zero( ppvAsyncShellCommandData, TEST_VAR_PRND_NOASSETTX_CMDDATA,       SIZEOF( DRM_VOID* ) );
        MAP_TO_GLOBAL_TEST_VAR_Alloc_And_Zero( pbCustomData,             TEST_VAR_PRND_NOASSETTX_CUSTOMDATA,    DRM_AES_BLOCKLEN*2+SIZEOF(DRM_UINT64) );
    }
    
ErrorExit:
    return dr;
}

DRM_RESULT DRM_CALL MODULE_POSTTESTCASE(prndnoassettxtest)(long lTCID, const char *strTCName)
{
          DRM_RESULT   dr                         = DRM_SUCCESS;
          DRM_VOID   **ppvAsyncShellCommandData   = NULL;
          DRM_BOOL     fNoAssetTxAvailable        = TRUE;
    const DRM_CHAR    *pszPRNDNoAssetTxAvailable  = "PRNDNoAssetTxAvailable";

    POSTTESTCASE_FUNCTION_CALL( TST_UTL_IsConditionTrue( &fNoAssetTxAvailable, 1, &pszPRNDNoAssetTxAvailable ) );
    if ( fNoAssetTxAvailable )
    {
        if( DRM_SUCCEEDED( DRM_TST_GetTestVar( TEST_VAR_PRND_NOASSETTX_CMDDATA, (DRM_VOID**)&ppvAsyncShellCommandData, NULL ) ) )
        {
            if( ppvAsyncShellCommandData != NULL && *ppvAsyncShellCommandData != NULL )
            {
                POSTTESTCASE_FUNCTION_CALL( TST_OEM_AsyncShellCommandWriteMessage( *ppvAsyncShellCommandData, ASYNC_SHELL_COMMAND_MESSAGE_TYPE_HALT_PROCESS, NULL, 0 ) );
                POSTTESTCASE_FUNCTION_CALL( TST_OEM_AsyncShellCommandEnd( ppvAsyncShellCommandData ) );
            }
        }

        /* Restore the certificate files */
        (DRM_VOID)tMoveFile( g_rgwchGroupCertBak,  g_rgwchGroupCert  );
        (DRM_VOID)tMoveFile( g_rgwchZgprivBak,     g_rgwchZgpriv     );
        (DRM_VOID)tMoveFile( g_rgwchDevCertTemBak, g_rgwchDevCertTem );
        (DRM_VOID)tMoveFile( g_rgwchPrivBak,       g_rgwchPriv       );
    }

    /* Do NOT define ErrorExit label here.  Do NOT use ChkDR (etc) above.  Doing so could skip cleanup operations. */
    return dr;
}

PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_NONCONST_PARAM_25004, "Prefast Noise: Callback contexts should never be const");
static DRM_RESULT DRM_CALL _CertificateCallbackEmpty(
    __inout_ecount_opt( 1 )                    DRM_VOID                                        *f_pvCertificateCallbackContext,
    __in_opt                                   DRM_PRND_CERTIFICATE_DATA                       *f_pCertificateData,
    __in_opt                             const DRM_ID                                          *f_pCustomDataTypeID,
    __in_bcount_opt( f_cbCustomData )    const DRM_BYTE                                        *f_pbCustomData,
    __in                                       DRM_DWORD                                        f_cbCustomData )
PREFAST_POP /* __WARNING_NONCONST_PARAM_25004 */
{
    return DRM_SUCCESS;
}

static DRM_RESULT DRM_CALL _TransactMessage(
    __inout                                     DRM_VOID        *f_pvAsyncShellCommandData,
    __in                                        DRM_ID          *f_pSessionID,
    __in_ecount(f_cbMsgRxToTx)            const DRM_BYTE        *f_pbMsgRxToTx,
    __in                                        DRM_DWORD        f_cbMsgRxToTx,
    __deref_inout_ecount(*f_pcbDataWrite)       DRM_BYTE       **f_ppbDataWrite,
    __inout                                     DRM_DWORD       *f_pcbDataWrite,
    __in                                        DRM_DWORD        f_dwMessageWrite,
    __in                                        DRM_DWORD        f_dwMessageReadExpected,
    __deref_out_ecount(*f_pcbDataRead)          DRM_BYTE       **f_ppbDataRead,
    __inout                                     DRM_DWORD       *f_pcbDataRead )
{
    DRM_RESULT dr               = DRM_SUCCESS;
    DRM_DWORD  dwMessageRead    = 0;

    /* Insert session ID at start of message */
    *f_pcbDataWrite = f_cbMsgRxToTx + SIZEOF(DRM_ID);
    DRM_PRND_SAFEMEMFREE( *f_ppbDataWrite );
    ChkMem( *f_ppbDataWrite = (DRM_BYTE*)Drm_Prnd_MemAlloc( *f_pcbDataWrite ) );
    MEMCPY( *f_ppbDataWrite, f_pSessionID, SIZEOF(DRM_ID) );
    MEMCPY( *f_ppbDataWrite + SIZEOF(DRM_ID), f_pbMsgRxToTx, f_cbMsgRxToTx );
    ChkDR( TST_OEM_AsyncShellCommandWriteMessage( f_pvAsyncShellCommandData, f_dwMessageWrite, *f_ppbDataWrite, *f_pcbDataWrite ) );
    ChkDR( TST_OEM_AsyncShellCommandReadMessage( f_pvAsyncShellCommandData, &dwMessageRead, f_ppbDataRead, f_pcbDataRead ) );

    /* Confirm session ID at start of message */
    ChkBOOL( dwMessageRead == f_dwMessageReadExpected, DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkBOOL( *f_pcbDataWrite > SIZEOF(DRM_ID), DRM_E_TEST_UNEXPECTED_OUTPUT );
    if( 0 != MEMCMP( f_pSessionID, &g_guidNull, SIZEOF(DRM_ID) )
     || f_dwMessageReadExpected == PRND_NO_ASSET_TX_MESSAGE_TYPE_REGISTRATION_RESPONSE_FAILURE )
    {
        ChkBOOL( 0 == MEMCMP( *f_ppbDataRead, f_pSessionID, SIZEOF(DRM_ID) ), DRM_E_TEST_UNEXPECTED_OUTPUT );
    }
    else
    {
        ChkBOOL( 0 != MEMCMP( *f_ppbDataRead, f_pSessionID, SIZEOF(DRM_ID) ), DRM_E_TEST_UNEXPECTED_OUTPUT );
    }

ErrorExit:
    return dr;
}

/*****************************************************************************
** Arguments: None
******************************************************************************/
DRM_RESULT DRM_CALL TestPRNDNoAssetTx_StartTx( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT    dr                        = DRM_SUCCESS;
    DRM_VOID    **ppvAsyncShellCommandData  = NULL;

    MAP_TO_GLOBAL_TEST_VAR( ppvAsyncShellCommandData, TEST_VAR_PRND_NOASSETTX_CMDDATA );

    /* Rename the certificate files to prevent them from being picked up from the asset-less Tx */
    ChkBOOL( tMoveFile( g_rgwchGroupCert,   g_rgwchGroupCertBak ),   DRM_E_FILE_WRITE_ERROR );
    ChkBOOL( tMoveFile( g_rgwchZgpriv,      g_rgwchZgprivBak ),      DRM_E_FILE_WRITE_ERROR );
    ChkBOOL( tMoveFile( g_rgwchDevCertTem,  g_rgwchDevCertTemBak ),  DRM_E_FILE_WRITE_ERROR );
    ChkBOOL( tMoveFile( g_rgwchPriv,        g_rgwchPrivBak ),        DRM_E_FILE_WRITE_ERROR );

    ChkDR( TST_OEM_AsyncShellCommandStart( "prndnoassettx.exe", ppvAsyncShellCommandData ) );

ErrorExit:
    return dr;
}

/*****************************************************************************
** Arguments: None
******************************************************************************/
DRM_RESULT DRM_CALL TestPRNDNoAssetTx_EndTx( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT    dr                        = DRM_SUCCESS;
    DRM_VOID    **ppvAsyncShellCommandData  = NULL;

    MAP_TO_GLOBAL_TEST_VAR( ppvAsyncShellCommandData, TEST_VAR_PRND_NOASSETTX_CMDDATA );

    ChkDR( TST_OEM_AsyncShellCommandWriteMessage( *ppvAsyncShellCommandData, ASYNC_SHELL_COMMAND_MESSAGE_TYPE_HALT_PROCESS, NULL, 0 ) );
    ChkDR( TST_OEM_AsyncShellCommandEnd( ppvAsyncShellCommandData ) );

    /* Restore the certificate files */
    ChkBOOL( tMoveFile( g_rgwchGroupCertBak,  g_rgwchGroupCert ),     DRM_E_FILE_WRITE_ERROR );
    ChkBOOL( tMoveFile( g_rgwchZgprivBak,     g_rgwchZgpriv ),        DRM_E_FILE_WRITE_ERROR );
    ChkBOOL( tMoveFile( g_rgwchDevCertTemBak, g_rgwchDevCertTem ),    DRM_E_FILE_WRITE_ERROR );
    ChkBOOL( tMoveFile( g_rgwchPrivBak,       g_rgwchPriv ),          DRM_E_FILE_WRITE_ERROR );

ErrorExit:
    return dr;
}

/*****************************************************************************
** Arguments: argc    - count of the number of arguments
**            argv[0] - OPTIONAL - when present, indicates that we want the test Tx
**                      to believe that its revocation information has expired
******************************************************************************/
DRM_RESULT DRM_CALL TestPRNDNoAssetTx_PerformRegistration( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT                            dr                                        = DRM_SUCCESS;
    DRM_VOID                            **ppvAsyncShellCommandData                  = NULL;
    DRM_APP_CONTEXT                      *pAppContext                               = NULL;
    DRM_PRND_RECEIVER_CONTEXT            *pRxCtx                                    = NULL;
    DRM_BYTE                             *pbMsgRxToTx                               = NULL;
    DRM_DWORD                             cbMsgRxToTx                               = 0;
    DRM_BYTE                             *pbDataWrite                               = NULL;
    DRM_DWORD                             cbDataWrite                               = 0;
    DRM_BYTE                             *pbDataRead                                = NULL;
    DRM_DWORD                             cbDataRead                                = 0;
    DRM_ID                                idSession                                 = {0};
    DRM_PRND_PROXIMITY_DETECTION_TYPE     eProximityDetectionType                   = DRM_PRND_PROXIMITY_DETECTION_TYPE_TRANSPORT_AGNOSTIC;
    DRM_BYTE                             *pbTransmitterProximityDetectionChannel    = NULL;
    DRM_DWORD                             cbTransmitterProximityDetectionChannel    = 0;
    DRM_DWORD                             dwFlagsOut                                = 0;
    DRM_DWORD                             dwMessageWrite                            = PRND_NO_ASSET_TX_MESSAGE_TYPE_REGISTRATION_REQUEST;
    DRM_DWORD                             dwMessageReadExpected                     = PRND_NO_ASSET_TX_MESSAGE_TYPE_REGISTRATION_RESPONSE;

    MAP_TO_GLOBAL_TEST_VAR( ppvAsyncShellCommandData, TEST_VAR_PRND_NOASSETTX_CMDDATA );
    GET_SHARED_APP_CONTEXT( pAppContext );
    MAP_TO_GLOBAL_TEST_VAR( pRxCtx, TEST_VAR_PRND_RXCTX );

    /* Rx: Begin Session */
    ChkDR( Drm_Prnd_Receiver_BeginSession( pAppContext, pRxCtx ) );

    /* Rx: Generate Registration Request */
    DRM_PRND_SAFEMEMFREE( pbMsgRxToTx );
    cbMsgRxToTx = 0;
    ChkDR( Drm_Prnd_Receiver_RegistrationRequest_Generate(
        pAppContext,
        NULL,
        &g_customDataTypeID,
        g_rgbCustomData,
        g_cbCustomData,
        DRM_PRND_FLAG_NO_FLAGS,
        &pbMsgRxToTx,
        &cbMsgRxToTx ) );

    if ( HASARG( 0 ) )
    {
        dwMessageWrite        = PRND_NO_ASSET_TX_MESSAGE_TYPE_REGISTRATION_REQUEST_EXPIRED_REVINFO;
        dwMessageReadExpected = PRND_NO_ASSET_TX_MESSAGE_TYPE_REGISTRATION_RESPONSE_FAILURE;
    }
    
    /* Rx: Send Registration Request, Receive Registration Response */
    DRM_PRND_SAFEMEMFREE( pbDataWrite );
    cbDataWrite = 0;
    SAFE_OEM_FREE( pbDataRead );
    cbDataRead = 0;
    ChkDR( _TransactMessage(
        *ppvAsyncShellCommandData,
        (DRM_ID*)&g_guidNull,
        pbMsgRxToTx,
        cbMsgRxToTx,
        &pbDataWrite,
        &cbDataWrite,
        dwMessageWrite,
        dwMessageReadExpected,
        &pbDataRead,
        &cbDataRead ) );

    if ( dwMessageReadExpected == PRND_NO_ASSET_TX_MESSAGE_TYPE_REGISTRATION_RESPONSE )
    {
        /* Rx: Process Registration Response */
        ChkDR( Drm_Prnd_Receiver_RegistrationResponse_Process(
            pAppContext,
            pbDataRead + SIZEOF(DRM_ID),
            cbDataRead - SIZEOF(DRM_ID),
            _CertificateCallbackEmpty,
            NULL,
            &idSession,
            &eProximityDetectionType,
            &pbTransmitterProximityDetectionChannel,
            &cbTransmitterProximityDetectionChannel,
            &dwFlagsOut ) );
        ChkBOOL( 0 == MEMCMP( pbDataRead, &idSession, SIZEOF(DRM_ID) ), DRM_E_TEST_UNEXPECTED_OUTPUT );
    }
    else if ( dwMessageReadExpected == PRND_NO_ASSET_TX_MESSAGE_TYPE_REGISTRATION_RESPONSE_FAILURE )
    {
        DRM_RESULT drReturned = DRM_SUCCESS;
        
        ChkDR( Drm_Prnd_Receiver_RegistrationError_Process(
            pAppContext,
            pbDataRead + SIZEOF(DRM_ID),
            cbDataRead - SIZEOF(DRM_ID),
            &drReturned,
            &dwFlagsOut ) );
        ChkBOOL( drReturned == DRM_E_OUTDATED_REVOCATION_LIST, DRM_E_TEST_UNEXPECTED_OUTPUT );
        ChkBOOL( dwFlagsOut == DRM_PRND_FLAG_NO_FLAGS,         DRM_E_TEST_UNEXPECTED_OUTPUT );
    }
    else
    {
        ChkDR( DRM_E_TEST_UNEXPECTED_OUTPUT );
    }
    
ErrorExit:
    DRM_PRND_SAFEMEMFREE( pbTransmitterProximityDetectionChannel );
    DRM_PRND_SAFEMEMFREE( pbMsgRxToTx );
    DRM_PRND_SAFEMEMFREE( pbDataWrite );
    SAFE_OEM_FREE( pbDataRead );
    return dr;
}

/*****************************************************************************
** Arguments: None
******************************************************************************/
DRM_RESULT DRM_CALL TestPRNDNoAssetTx_PerformProximityDetection( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT                            dr                                        = DRM_SUCCESS;
    DRM_VOID                            **ppvAsyncShellCommandData                  = NULL;
    DRM_APP_CONTEXT                      *pAppContext                               = NULL;
    DRM_PRND_RECEIVER_CONTEXT            *pRxCtx                                    = NULL;
    DRM_BYTE                             *pbMsgRxToTx                               = NULL;
    DRM_DWORD                             cbMsgRxToTx                               = 0;
    DRM_BYTE                             *pbDataWrite                               = NULL;
    DRM_DWORD                             cbDataWrite                               = 0;
    DRM_BYTE                             *pbDataRead                                = NULL;
    DRM_DWORD                             cbDataRead                                = 0;
    DRM_DWORD                             cTries                                    = ASYNC_SHELL_COMMAND_WAIT_PINGS;
    DRM_RESULT                            drPD                                      = DRM_SUCCESS;
    DRM_DWORD                             dwFlagsOut                                = 0;

    MAP_TO_GLOBAL_TEST_VAR( ppvAsyncShellCommandData, TEST_VAR_PRND_NOASSETTX_CMDDATA );
    GET_SHARED_APP_CONTEXT( pAppContext );
    MAP_TO_GLOBAL_TEST_VAR( pRxCtx, TEST_VAR_PRND_RXCTX );

    do
    {
        /* Rx: Start proximity detection */
        DRM_PRND_SAFEMEMFREE( pbMsgRxToTx );
        cbMsgRxToTx = 0;
        ChkDR( Drm_Prnd_Receiver_ProximityDetectionStart_Generate(
            pAppContext,
            DRM_PRND_FLAG_NO_FLAGS,
            &pbMsgRxToTx,
            &cbMsgRxToTx ) );

        /* Rx: Send proximity start, Receive proximity challenge */
        DRM_PRND_SAFEMEMFREE( pbDataWrite );
        cbDataWrite = 0;
        SAFE_OEM_FREE( pbDataRead );
        cbDataRead = 0;
        ChkDR( _TransactMessage(
            *ppvAsyncShellCommandData,
            (DRM_ID*)&pRxCtx->oPrndReceiverSessionContext.SessionID,
            pbMsgRxToTx,
            cbMsgRxToTx,
            &pbDataWrite,
            &cbDataWrite,
            PRND_NO_ASSET_TX_MESSAGE_TYPE_PD_START,
            PRND_NO_ASSET_TX_MESSAGE_TYPE_PD_CHLG,
            &pbDataRead,
            &cbDataRead ) );

        /* Rx: Process proximity challenge, Generate proximity response */
        /* Note: In the real world, this method will return DRM_E_PRND_MESSAGE_WRONG_TYPE if the message is a result message instead. */
        DRM_PRND_SAFEMEMFREE( pbMsgRxToTx );
        cbMsgRxToTx = 0;
        ChkDR( Drm_Prnd_Receiver_ProximityDetectionChallenge_Process(
            pAppContext,
            pbDataRead + SIZEOF(DRM_ID),
            cbDataRead - SIZEOF(DRM_ID),
            DRM_PRND_FLAG_NO_FLAGS,
            &pbMsgRxToTx,
            &cbMsgRxToTx,
            &dwFlagsOut ) );

        /* Rx: Send proximity response, Receive proximity result */
        DRM_PRND_SAFEMEMFREE( pbDataWrite );
        cbDataWrite = 0;
        SAFE_OEM_FREE( pbDataRead );
        cbDataRead = 0;
        ChkDR( _TransactMessage(
            *ppvAsyncShellCommandData,
            (DRM_ID*)&pRxCtx->oPrndReceiverSessionContext.SessionID,
            pbMsgRxToTx,
            cbMsgRxToTx,
            &pbDataWrite,
            &cbDataWrite,
            PRND_NO_ASSET_TX_MESSAGE_TYPE_PD_RESP,
            PRND_NO_ASSET_TX_MESSAGE_TYPE_PD_RSLT,
            &pbDataRead,
            &cbDataRead ) );

        /* Rx: Process proximity result */
        ChkDR( Drm_Prnd_Receiver_ProximityDetectionResult_Process(
            pAppContext,
            pbDataRead + SIZEOF(DRM_ID),
            cbDataRead - SIZEOF(DRM_ID),
            &drPD,
            &dwFlagsOut ) );

        cTries--;
        if( drPD != DRM_E_PRND_PROXIMITY_DETECTION_RESPONSE_TIMEOUT )
        {
            /* Only retry on timeout error */
            cTries = 0;
        }
        else if( cTries > 0 )
        {
            Log( "Trace", "RTT was unexpectedly more than 7 ms.  This may be due to heavy load on the system.  Retrying... (%d remaining)", cTries );
            ChkDR( TST_OEM_AsyncShellCommandPing( *ppvAsyncShellCommandData, ASYNC_SHELL_COMMAND_DEFAULT_PING_TIME ) );     /* Wait 50 ms, try again */
        }
        else
        {
            Log( "Trace", "RTT was unexpectedly more than 7 ms too many times.  Halting retry loop." );
        }

    } while( cTries > 0 );

    ChkDR( drPD );

ErrorExit:
    DRM_PRND_SAFEMEMFREE( pbMsgRxToTx );
    DRM_PRND_SAFEMEMFREE( pbDataWrite );
    SAFE_OEM_FREE( pbDataRead );
    return dr;
}

PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_NONCONST_PARAM_25004, "Prefast Noise: Callback contexts should never be const");
static DRM_RESULT DRM_CALL _CertificateCallbackSaveCustomData(
    __inout_ecount_opt( 1 )                    DRM_VOID                                        *f_pvCertificateCallbackContext,
    __in_opt                                   DRM_PRND_CERTIFICATE_DATA                       *f_pCertificateData,
    __in_opt                             const DRM_ID                                          *f_pCustomDataTypeID,
    __in_bcount_opt( f_cbCustomData )    const DRM_BYTE                                        *f_pbCustomData,
    __in                                       DRM_DWORD                                        f_cbCustomData )
PREFAST_POP /* __WARNING_NONCONST_PARAM_25004 */
{
    DRM_RESULT   dr                         = DRM_SUCCESS;
    DRM_BYTE    *pbCustomData               = NULL;
    DRM_ID       idCustomData               = {1};      /* Note: Must match value in prndnoassettx.c */

    MAP_TO_GLOBAL_TEST_VAR( pbCustomData, TEST_VAR_PRND_NOASSETTX_CUSTOMDATA );

    ChkBOOL( f_pCustomDataTypeID != NULL,                                                                   DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkBOOL( 0                   == DRMCRT_memcmp( f_pCustomDataTypeID, &idCustomData, SIZEOF(DRM_ID)),     DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkBOOL( f_cbCustomData      == DRM_AES_BLOCKLEN*2+SIZEOF(DRM_UINT64),                                  DRM_E_TEST_UNEXPECTED_OUTPUT );

    MEMCPY( pbCustomData, f_pbCustomData, f_cbCustomData );

ErrorExit:
    return dr;
}

/*****************************************************************************
** Arguments: None
******************************************************************************/
DRM_RESULT DRM_CALL TestPRNDNoAssetTx_PerformLicenseFetch( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT                            dr                                                    = DRM_SUCCESS;
    DRM_VOID                            **ppvAsyncShellCommandData                              = NULL;
    DRM_APP_CONTEXT                      *pAppContext                                           = NULL;
    DRM_PRND_RECEIVER_CONTEXT            *pRxCtx                                                = NULL;
    DRM_BYTE                             *pbMsgRxToTx                                           = NULL;
    DRM_DWORD                             cbMsgRxToTx                                           = 0;
    DRM_BYTE                             *pbDataWrite                                           = NULL;
    DRM_DWORD                             cbDataWrite                                           = 0;
    DRM_BYTE                             *pbDataRead                                            = NULL;
    DRM_DWORD                             cbDataRead                                            = 0;
    DRM_KID                               oKid                                                  = {0};
    DRM_WCHAR                             rgwchKid[ CCH_BASE64_EQUIV( SIZEOF( DRM_KID ) ) + 1 ] = {0};
    DRM_DWORD                             cchKid                                                = CCH_BASE64_EQUIV( SIZEOF( DRM_KID ) );
    DRM_BYTE                             *pbCustomData                                          = NULL;
    const DRM_CONST_STRING               *pdstrPlay                                             = &g_dstrWMDRM_RIGHT_PLAYBACK;
    DRM_DECRYPT_CONTEXT                   oDecryptCtx                                           = {0};
    DRM_AES_COUNTER_MODE_CONTEXT          oAesCtrCtx                                            = {0};
    DRM_BYTE                              rgbClearData[DRM_AES_BLOCKLEN]                        = PRND_NO_ASSET_TX_CLEAR_CONTENT_STRING;
    DRM_BYTE                              rgbEncryptedData[DRM_AES_BLOCKLEN]                    = {0};
    DRM_DWORD                             dwFlagsOut                                            = 0;

    MAP_TO_GLOBAL_TEST_VAR( ppvAsyncShellCommandData, TEST_VAR_PRND_NOASSETTX_CMDDATA );
    GET_SHARED_APP_CONTEXT( pAppContext );
    MAP_TO_GLOBAL_TEST_VAR( pRxCtx, TEST_VAR_PRND_RXCTX );
    MAP_TO_GLOBAL_TEST_VAR( pbCustomData, TEST_VAR_PRND_NOASSETTX_CUSTOMDATA );

    ChkDR( Oem_Random_GetBytes( NULL, (DRM_BYTE*)&oKid, SIZEOF( oKid ) ) );

    /* Rx: Generate License Request */
    DRM_PRND_SAFEMEMFREE( pbMsgRxToTx );
    cbMsgRxToTx = 0;
    ChkDR( Drm_Prnd_Receiver_LicenseRequest_Generate(
        pAppContext,
        &DRM_ACTION_PLAY,
        NULL,
        DRM_PRND_CONTENT_IDENTIFIER_TYPE_KID,
        (const DRM_BYTE*)&oKid,
        SIZEOF( oKid ),
        &g_customDataTypeID,
        g_rgbCustomData,
        g_cbCustomData,
        DRM_PRND_FLAG_NO_FLAGS,
        &pbMsgRxToTx,
        &cbMsgRxToTx ) );

    /* Rx: Send License Request, Receive License Response */
    DRM_PRND_SAFEMEMFREE( pbDataWrite );
    cbDataWrite = 0;
    SAFE_OEM_FREE( pbDataRead );
    cbDataRead = 0;
    ChkDR( _TransactMessage(
        *ppvAsyncShellCommandData,
        (DRM_ID*)&pRxCtx->oPrndReceiverSessionContext.SessionID,
        pbMsgRxToTx,
        cbMsgRxToTx,
        &pbDataWrite,
        &cbDataWrite,
        PRND_NO_ASSET_TX_MESSAGE_TYPE_LICENSE_REQUEST,
        PRND_NO_ASSET_TX_MESSAGE_TYPE_LICENSE_RESPONSE,
        &pbDataRead,
        &cbDataRead ) );

    /* Rx: Process License Response */
    ChkDR( Drm_Prnd_Receiver_LicenseTransmit_Process(
        pAppContext,
        pbDataRead + SIZEOF(DRM_ID),
        cbDataRead - SIZEOF(DRM_ID),
        _CertificateCallbackSaveCustomData,
        NULL,
        &dwFlagsOut ) );

    /* Rx: Receive the IV, clear data, and encrypted data as the Custom Data for the License Response! */
    MEMCPY( &oAesCtrCtx.qwInitializationVector, pbCustomData,                                           SIZEOF(DRM_UINT64) );
    MEMCPY( rgbClearData,                       &pbCustomData[SIZEOF(DRM_UINT64)],                      SIZEOF(rgbClearData) );
    MEMCPY( rgbEncryptedData,                   &pbCustomData[SIZEOF(DRM_UINT64)+SIZEOF(rgbClearData)], SIZEOF(rgbEncryptedData) );
    ChkBOOL( 0 != DRMCRT_memcmp( rgbClearData, rgbEncryptedData, SIZEOF( rgbClearData ) ), DRM_E_TEST_UNEXPECTED_OUTPUT );

    /* Rx: Bind to license */
    ChkDR( DRM_B64_EncodeW(
        (const DRM_BYTE*)&oKid,
        SIZEOF( oKid ),
        rgwchKid,
        &cchKid,
        0 ) );
    rgwchKid[ CCH_BASE64_EQUIV( SIZEOF( DRM_KID ) ) ] = g_wchNull;
    ChkDR( Drm_Content_SetProperty(
        pAppContext,
        DRM_CSP_KID,
        (const DRM_BYTE*)rgwchKid,
        SIZEOF( rgwchKid ) - SIZEOF( DRM_WCHAR ) ) );     /* Function *requires* the KID to NOT include the null terminator */
    ChkDR( Drm_Reader_Bind(
        pAppContext,
        &pdstrPlay,
        1,
        NULL,
        NULL,
        &oDecryptCtx ) );

    /* Rx: Decrypt and verify content */
    ChkDR( Drm_Reader_Decrypt(
        &oDecryptCtx,
        &oAesCtrCtx,
        rgbEncryptedData,
        SIZEOF( rgbEncryptedData ) ) );
    ChkBOOL( 0 == DRMCRT_memcmp( rgbClearData, rgbEncryptedData, SIZEOF( rgbClearData ) ), DRM_E_TEST_UNEXPECTED_OUTPUT );
    Log( "Trace", "Decrypted Data: \"%s\"", (DRM_CHAR*)rgbEncryptedData );
    ChkDR( Drm_Reader_Commit(
        pAppContext,
        NULL,
        NULL ) );

ErrorExit:
    Drm_Reader_Close( &oDecryptCtx );
    DRM_PRND_SAFEMEMFREE( pbMsgRxToTx );
    DRM_PRND_SAFEMEMFREE( pbDataWrite );
    SAFE_OEM_FREE( pbDataRead );
    return dr;
}

BEGIN_TEST_API_LIST(prndnoassettxtest)
    API_ENTRY(TestPRNDNoAssetTx_StartTx)
    API_ENTRY(TestPRNDNoAssetTx_EndTx)
    API_ENTRY(TestPRNDNoAssetTx_PerformRegistration)
    API_ENTRY(TestPRNDNoAssetTx_PerformProximityDetection)
    API_ENTRY(TestPRNDNoAssetTx_PerformLicenseFetch)
END_TEST_API_LIST

EXIT_PKTEST_NAMESPACE_CODE;

