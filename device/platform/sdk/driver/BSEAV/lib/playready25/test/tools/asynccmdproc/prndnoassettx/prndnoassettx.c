/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifdef WINCE_TEST

#include <drmerr.h>

DRM_RESULT TST_DRM_AsyncCmdProc_ProcessMessage(
    __in                           DRM_DWORD    f_dwMessageType,
    __in_ecount(f_cbMessage) const DRM_BYTE    *f_pbMessage,
    __in                           DRM_DWORD    f_cbMessage )
{
    UNREFERENCED_PARAMETER(f_dwMessageType);
    UNREFERENCED_PARAMETER(f_pbMessage);
    UNREFERENCED_PARAMETER(f_cbMessage);
    return DRM_E_NOTIMPL;
}

#else   /* WINCE_TEST */

#include <stdio.h>

#include <drmmanager.h>
#include <drmconstants.h>
#include <drmlicgen.h>
#include <tOEMImp.h>
#include <PKTestNameSpace.h>
#include <tstutils.h>

#define SKIP_DECLARE_MODULE_DEFS 1
#include <prndnoassettxtest.h>

ENTER_PKTEST_NAMESPACE_CODE;

extern DRM_RESULT TST_DRM_AsyncCmdProc_WriteMessage(
    __in                           DRM_DWORD    f_dwMessageType,
    __in_ecount(f_cbMessage) const DRM_BYTE    *f_pbMessage,
    __in                           DRM_DWORD    f_cbMessage );

DRM_APP_CONTEXT                 g_AppContext                                                = {0};
DRM_BYTE                        g_rgbOpaqueBuffer[MINIMUM_APPCONTEXT_OPAQUE_BUFFER_SIZE]    = {0};
DRM_BYTE                        g_rgbRevocationBuffer[REVOCATION_BUFFER_SIZE]               = {0};
DRM_BOOL                        g_fDrmInitialized                                           = FALSE;

DRM_PRND_TRANSMITTER_CONTEXT    g_TxCtx                                                     = {0};
DRM_ID                          g_SessionID                                                 = {0};
DRM_BOOL                        g_fSessionStarted                                           = FALSE;

/* Note: Must match file name in prndnoassettxtest.c */
DRM_WCHAR                       g_rgwchHds[]                                                = { WCHAR_CAST('t'), WCHAR_CAST('m'), WCHAR_CAST('p'), WCHAR_CAST('\0')} ;
DRM_CONST_STRING                g_dstrHds                                                   = CREATE_DRM_STRING( g_rgwchHds );

static const DRM_DWORD          g_dwCRLTime                                                 = 1206921600;  /* crl issue time of revinfo in revpackage.xml */

PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_NONCONST_PARAM_25004, "Prefast Noise: Callback contexts should never be const");
static DRM_RESULT DRM_CALL _CertificateCallback(
    __inout_ecount_opt( 1 )                    DRM_VOID                                        *f_pvCertificateCallbackContext,
    __in_opt                                   DRM_PRND_CERTIFICATE_DATA                       *f_pCertificateData,
    __in_opt                             const DRM_ID                                          *f_pCustomDataTypeID,
    __in_bcount_opt( f_cbCustomData )    const DRM_BYTE                                        *f_pbCustomData,
    __in                                       DRM_DWORD                                        f_cbCustomData )
PREFAST_POP /* __WARNING_NONCONST_PARAM_25004 */
{
    return DRM_SUCCESS;
}

PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_NONCONST_PARAM_25004, "Prefast Noise: Callback contexts should never be const");
static DRM_RESULT DRM_CALL _Content_Identifier_Callback(
    __inout_ecount_opt( 1 )                        DRM_VOID                                    *f_pvContentIdentifierCallbackContext,
    __in                                           DRM_PRND_CONTENT_IDENTIFIER_TYPE             f_eContentIdentifierType,
    __in_bcount_opt( f_cbContentIdentifier ) const DRM_BYTE                                    *f_pbContentIdentifier,
    __in                                           DRM_DWORD                                    f_cbContentIdentifier,
    __in_ecount_opt( 1 )                           DRM_KID                                     *f_pkidContent )
PREFAST_POP /* __WARNING_NONCONST_PARAM_25004 */
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pvContentIdentifierCallbackContext != NULL );
    ChkArg( f_pkidContent != NULL );

    /* Just copy the KID into the context address which we know is the addess of a KID (see _HandleLicenseRequest below) */
    MEMCPY( f_pvContentIdentifierCallbackContext, f_pkidContent, SIZEOF( DRM_KID ) );

ErrorExit:
    return dr;
}


static DRM_RESULT DRM_CALL _WriteMessage(
    __in_ecount(f_cbMsgRxToTx)            const DRM_BYTE        *f_pbMsgTxToRx,
    __in                                        DRM_DWORD        f_cbMsgTxToRx,
    __deref_inout_ecount(*f_pcbDataWrite)       DRM_BYTE       **f_ppbDataWrite,
    __inout                                     DRM_DWORD       *f_pcbDataWrite,
    __in                                        DRM_DWORD        f_dwMessageWrite )
{
    DRM_RESULT dr = DRM_SUCCESS;

    /* Insert session ID at start of message */
    *f_pcbDataWrite = f_cbMsgTxToRx + SIZEOF(DRM_ID);
    ChkMem( *f_ppbDataWrite = (DRM_BYTE*)Drm_Prnd_MemAlloc( *f_pcbDataWrite ) );
    MEMCPY( *f_ppbDataWrite, &g_SessionID, SIZEOF(DRM_ID) );
    MEMCPY( *f_ppbDataWrite + SIZEOF(DRM_ID), f_pbMsgTxToRx, f_cbMsgTxToRx );
    ChkDR( TST_DRM_AsyncCmdProc_WriteMessage( f_dwMessageWrite, *f_ppbDataWrite, *f_pcbDataWrite ) );

ErrorExit:
    return dr;
}

static DRM_VOID _Cleanup()
{
    if( g_fSessionStarted )
    {
        (DRM_VOID)Drm_Prnd_Transmitter_EndSession( &g_AppContext );
    }

    if( g_fDrmInitialized )
    {
        (DRM_VOID)Drm_Uninitialize( &g_AppContext );
        (DRM_VOID)tRemoveFileW( g_rgwchHds );
    }
}

static DRM_RESULT _HandleRegistrationRequest(
    __in_ecount(f_cbMessage) const DRM_BYTE    *f_pbMessage,
    __in                           DRM_DWORD    f_cbMessage,
    __in                     const DRM_BOOL     f_fNonExpiredRevInfo)
{
    DRM_RESULT                           dr                                     = DRM_SUCCESS;
    DRM_BYTE                            *pbMsgTxToRx                            = NULL;
    DRM_DWORD                            cbMsgTxToRx                            = 0;
    DRM_BYTE                            *pbDataWrite                            = NULL;
    DRM_DWORD                            cbDataWrite                            = 0;
    DRM_PRND_PROXIMITY_DETECTION_TYPE    eProximityDetectionType                = DRM_PRND_PROXIMITY_DETECTION_TYPE_TRANSPORT_AGNOSTIC;
    DRM_BYTE                            *pbTransmitterProximityDetectionChannel = NULL;
    DRM_DWORD                            cbTransmitterProximityDetectionChannel = 0;
    DRM_DWORD                            dwFlagsOut                             = 0;
    DRM_BYTE                            *pbRevPackage                           = NULL;
    DRM_DWORD                            cbRevPackage                           = 0;
    DRM_RESULT                           drRequest                              = DRM_SUCCESS;
    DRM_DWORD                            dwMessageWrite                         = PRND_NO_ASSET_TX_MESSAGE_TYPE_REGISTRATION_RESPONSE;

    if( !g_fDrmInitialized )
    {
        DRM_DWORD   dwCurrentTime  = 0;
        DRM_LONG    lOffsetTime    = 0;

        ChkDR( Drm_Initialize(
            &g_AppContext,
            NULL,
            g_rgbOpaqueBuffer,
            SIZEOF(g_rgbOpaqueBuffer),
            &g_dstrHds ) );
        g_fDrmInitialized = TRUE;

        ChkDR( Drm_Revocation_SetBuffer(
            &g_AppContext,
            g_rgbRevocationBuffer,
            SIZEOF(g_rgbRevocationBuffer) ) );

        ChkBOOL( LoadTestFile( NULL, "revpackage.xml", &pbRevPackage, &cbRevPackage ) == TRUE, DRM_E_FAIL );
        ChkDR( Drm_Revocation_StorePackage( &g_AppContext, ( DRM_CHAR * )pbRevPackage, cbRevPackage ) );

        if ( f_fNonExpiredRevInfo )
        {
            DRMFILETIME ftCurrentTime  = {0};
            
            Oem_Clock_GetSystemTimeAsFileTime( NULL, &ftCurrentTime );
            FILE_TIME_TO_DWORD( ftCurrentTime, dwCurrentTime );

            lOffsetTime = (DRM_LONG)g_dwCRLTime - (DRM_LONG)dwCurrentTime;
            ChkDR( tChangeSystemTime( lOffsetTime ) );
        }
    }

    if( !g_fSessionStarted )
    {
        /* Tx: Begin Session */
        ChkDR( Drm_Prnd_Transmitter_BeginSession( &g_AppContext, &g_TxCtx ) );
        g_fSessionStarted = TRUE;
    }

    ChkArg( f_cbMessage > SIZEOF(DRM_ID) );
    ChkArg( 0 == MEMCMP( f_pbMessage, &g_guidNull, SIZEOF(DRM_ID) ) );

    DRM_PRND_SAFEMEMFREE( pbMsgTxToRx );
    cbMsgTxToRx = 0;
    
    drRequest = Drm_Prnd_Transmitter_RegistrationRequest_Process(
        &g_AppContext,
        f_pbMessage + SIZEOF(DRM_ID),
        f_cbMessage - SIZEOF(DRM_ID),
        _CertificateCallback,
        NULL,   /* No callback context */
        &g_SessionID,
        &eProximityDetectionType,
        &pbTransmitterProximityDetectionChannel,
        &cbTransmitterProximityDetectionChannel,
        &dwFlagsOut );

    if ( DRM_SUCCEEDED( drRequest ) )
    {
        ChkBOOL( pbTransmitterProximityDetectionChannel != NULL, DRM_E_TEST_UNEXPECTED_OUTPUT );
        ChkBOOL( cbTransmitterProximityDetectionChannel  > 0,    DRM_E_TEST_UNEXPECTED_OUTPUT );
        DRM_PRND_SAFEMEMFREE( pbTransmitterProximityDetectionChannel );

        ChkDR( Drm_Prnd_Transmitter_RegistrationResponse_Generate(
            &g_AppContext,
            NULL,   /* No custom data */
            NULL,   /* No custom data */
            0,      /* No custom data */
            DRM_PRND_FLAG_NO_FLAGS,
            &pbMsgTxToRx,
            &cbMsgTxToRx ) );

        dwMessageWrite = PRND_NO_ASSET_TX_MESSAGE_TYPE_REGISTRATION_RESPONSE;
    }
    else
    {
        ChkDR( Drm_Prnd_Transmitter_RegistrationError_Generate(
            &g_AppContext,
            drRequest,
            DRM_PRND_FLAG_NO_FLAGS,
            &pbMsgTxToRx,
            &cbMsgTxToRx ) );
            
        dwMessageWrite = PRND_NO_ASSET_TX_MESSAGE_TYPE_REGISTRATION_RESPONSE_FAILURE;
    }
    
    DRM_PRND_SAFEMEMFREE( pbDataWrite );
    cbDataWrite = 0;
    ChkDR( _WriteMessage(
        pbMsgTxToRx,
        cbMsgTxToRx,
        &pbDataWrite,
        &cbDataWrite,
        dwMessageWrite ) );

ErrorExit:
    DRM_PRND_SAFEMEMFREE( pbMsgTxToRx );
    DRM_PRND_SAFEMEMFREE( pbTransmitterProximityDetectionChannel );
    DRM_PRND_SAFEMEMFREE( pbDataWrite );
    SAFE_OEM_FREE( pbRevPackage );
    printf( "_HandleRegistrationRequest returns 0x%x\n", dr );
    return dr;
}

static DRM_RESULT _HandleProximityStart(
    __in_ecount(f_cbMessage) const DRM_BYTE    *f_pbMessage,
    __in                           DRM_DWORD    f_cbMessage )
{
    DRM_RESULT                           dr                                     = DRM_SUCCESS;
    DRM_BYTE                            *pbMsgTxToRx                            = NULL;
    DRM_DWORD                            cbMsgTxToRx                            = 0;
    DRM_BYTE                            *pbDataWrite                            = NULL;
    DRM_DWORD                            cbDataWrite                            = 0;
    DRM_DWORD                            dwFlagsOut                             = 0;

    ChkArg( g_fSessionStarted );

    ChkArg( f_cbMessage > SIZEOF(DRM_ID) );
    ChkArg( 0 == MEMCMP( f_pbMessage, &g_SessionID, SIZEOF(DRM_ID) ) );

    DRM_PRND_SAFEMEMFREE( pbMsgTxToRx );
    cbMsgTxToRx = 0;
    ChkDR( Drm_Prnd_Transmitter_ProximityDetectionStart_Process(
        &g_AppContext,
        f_pbMessage + SIZEOF(DRM_ID),
        f_cbMessage - SIZEOF(DRM_ID),
        DRM_PRND_FLAG_NO_FLAGS,
        &pbMsgTxToRx,
        &cbMsgTxToRx,
        &dwFlagsOut ) );

    DRM_PRND_SAFEMEMFREE( pbDataWrite );
    cbDataWrite = 0;
    ChkDR( _WriteMessage(
        pbMsgTxToRx,
        cbMsgTxToRx,
        &pbDataWrite,
        &cbDataWrite,
        PRND_NO_ASSET_TX_MESSAGE_TYPE_PD_CHLG ) );

ErrorExit:
    DRM_PRND_SAFEMEMFREE( pbMsgTxToRx );
    DRM_PRND_SAFEMEMFREE( pbDataWrite );
    return dr;
}

static DRM_RESULT _HandleProximityResponse(
    __in_ecount(f_cbMessage) const DRM_BYTE    *f_pbMessage,
    __in                           DRM_DWORD    f_cbMessage )
{
    DRM_RESULT                           dr                                     = DRM_SUCCESS;
    DRM_BYTE                            *pbMsgTxToRx                            = NULL;
    DRM_DWORD                            cbMsgTxToRx                            = 0;
    DRM_BYTE                            *pbDataWrite                            = NULL;
    DRM_DWORD                            cbDataWrite                            = 0;
    DRM_DWORD                            dwFlagsOut                             = 0;

    ChkArg( g_fSessionStarted );

    ChkArg( f_cbMessage > SIZEOF(DRM_ID) );
    ChkArg( 0 == MEMCMP( f_pbMessage, &g_SessionID, SIZEOF(DRM_ID) ) );

    DRM_PRND_SAFEMEMFREE( pbMsgTxToRx );
    cbMsgTxToRx = 0;
    ChkDR( Drm_Prnd_Transmitter_ProximityDetectionResponse_Process(
        &g_AppContext,
        f_pbMessage + SIZEOF(DRM_ID),
        f_cbMessage - SIZEOF(DRM_ID),
        DRM_PRND_FLAG_NO_FLAGS,
        &pbMsgTxToRx,
        &cbMsgTxToRx,
        &dwFlagsOut ) );

    DRM_PRND_SAFEMEMFREE( pbDataWrite );
    cbDataWrite = 0;
    ChkDR( _WriteMessage(
        pbMsgTxToRx,
        cbMsgTxToRx,
        &pbDataWrite,
        &cbDataWrite,
        PRND_NO_ASSET_TX_MESSAGE_TYPE_PD_RSLT ) );

ErrorExit:
    DRM_PRND_SAFEMEMFREE( pbMsgTxToRx );
    DRM_PRND_SAFEMEMFREE( pbDataWrite );
    return dr;
}

static DRM_RESULT _HandleLicenseRequest(
    __in_ecount(f_cbMessage) const DRM_BYTE    *f_pbMessage,
    __in                           DRM_DWORD    f_cbMessage )
{
    DRM_RESULT                           dr                                                     = DRM_SUCCESS;
    DRM_BYTE                            *pbMsgTxToRx                                            = NULL;
    DRM_DWORD                            cbMsgTxToRx                                            = 0;
    DRM_BYTE                            *pbDataWrite                                            = NULL;
    DRM_DWORD                            cbDataWrite                                            = 0;
    DRM_LOCAL_LICENSE_POLICY_DESCRIPTOR  oLicensePolicyDecriptor                                = {0};
    DRM_LICENSE_HANDLE                   hLicenseHandle                                         = NULL;
    DRM_BYTE                             rgbClearData[DRM_AES_BLOCKLEN]                         = PRND_NO_ASSET_TX_CLEAR_CONTENT_STRING;
    DRM_BYTE                             rgbEncryptedData[DRM_AES_BLOCKLEN]                     = {0};
    DRM_UINT64                           qwIV                                                   = DRM_UI64HL( 0, 0 );
    DRM_ID                               idCustomData                                           = {1};      /* Note: Must match value in prndnoassettx.c */
    DRM_BYTE                             rgbCustomData[DRM_AES_BLOCKLEN*2+SIZEOF(DRM_UINT64)]   = {0};
    DRM_DWORD                            dwFlagsOut                                             = 0;
    DRM_KID                              oKid                                                   = {0};

    ChkArg( f_cbMessage > SIZEOF(DRM_ID) );
    ChkArg( 0 == MEMCMP( f_pbMessage, &g_SessionID, SIZEOF(DRM_ID) ) );

    ChkDR( Drm_Prnd_Transmitter_LicenseRequest_Process(
        &g_AppContext,
        f_pbMessage + SIZEOF(DRM_ID),
        f_cbMessage - SIZEOF(DRM_ID),
        _CertificateCallback,
        NULL,                   /* Test purposes only - real world caller would pass their own context */
        _Content_Identifier_Callback,
        &oKid,                  /* Pass the address of the KID as the callback context - in the callback we'll just copy the KID into this address */
        &dwFlagsOut ) );

    /* Tx: Use the local license generation to create REMOTE_BOUND_SIMPLE license */
    ChkDR( Drm_LocalLicense_InitializePolicyDescriptor( &oLicensePolicyDecriptor ) );
    oLicensePolicyDecriptor.wSecurityLevel   = 150;
    oLicensePolicyDecriptor.fCannotPersist   = TRUE;

    ChkDR( Drm_LocalLicense_CreateLicense(
        &g_AppContext,
        &oLicensePolicyDecriptor,
        eDRM_LOCAL_LICENSE_REMOTE_BOUND_SIMPLE,
        &oKid,
        0,
        NULL,
        DRM_LICENSE_HANDLE_INVALID,
        &hLicenseHandle ) );
    ChkDR( Drm_LocalLicense_StoreLicense(
        hLicenseHandle,
        eDRM_LOCAL_LICENSE_PRND_SESSION_STORE ) );

    /* Tx: Encrypt some content */
    DRMCRT_memcpy( rgbEncryptedData, rgbClearData, SIZEOF( rgbEncryptedData ) );
    {
        /* Tx: Call the licgen encryptor function to encrypt */
        DRM_DWORD  rgbSubsampleCount[1]   = {0};
        DRM_BYTE  *rgbSubsamplePointer[1] = {0};

        rgbSubsampleCount[0]   = SIZEOF( rgbEncryptedData );
        rgbSubsamplePointer[0] = (DRM_BYTE*)rgbEncryptedData;

        ChkDR( Drm_LocalLicense_EncryptSample(
            hLicenseHandle,
            1,
            rgbSubsampleCount,
            rgbSubsamplePointer,
           &qwIV ) );
    }

    /* Tx: Send the IV, clear data, and encrypted data as the Custom Data for the message! */
    MEMCPY( rgbCustomData,                                     &qwIV,            SIZEOF(qwIV) );
    MEMCPY( &rgbCustomData[SIZEOF(qwIV)],                      rgbClearData,     SIZEOF(rgbClearData) );
    MEMCPY( &rgbCustomData[SIZEOF(qwIV)+SIZEOF(rgbClearData)], rgbEncryptedData, SIZEOF(rgbEncryptedData) );

    DRM_PRND_SAFEMEMFREE( pbMsgTxToRx );
    cbMsgTxToRx = 0;
    ChkDR( Drm_Prnd_Transmitter_LicenseTransmit_Generate(
        &g_AppContext,
        &idCustomData,
        rgbCustomData,
        SIZEOF(rgbCustomData),
        DRM_PRND_FLAG_NO_FLAGS,
        &pbMsgTxToRx,
        &cbMsgTxToRx ) );

    DRM_PRND_SAFEMEMFREE( pbDataWrite );
    cbDataWrite = 0;
    ChkDR( _WriteMessage(
        pbMsgTxToRx,
        cbMsgTxToRx,
        &pbDataWrite,
        &cbDataWrite,
        PRND_NO_ASSET_TX_MESSAGE_TYPE_LICENSE_RESPONSE ) );

ErrorExit:
    if( hLicenseHandle != DRM_LICENSE_HANDLE_INVALID )
    {
        DRM_RESULT dr2 = DRM_SUCCESS;
        dr2 = Drm_LocalLicense_Release( &hLicenseHandle );
        if( DRM_SUCCEEDED( dr ) )
        {
            dr = dr2;
        }
    }
    DRM_PRND_SAFEMEMFREE( pbMsgTxToRx );
    DRM_PRND_SAFEMEMFREE( pbDataWrite );
    printf( "_HandleLicenseRequest returns 0x%x\n", dr );
    return dr;
}

DRM_RESULT TST_DRM_AsyncCmdProc_ProcessMessage(
    __in                           DRM_DWORD    f_dwMessageType,
    __in_ecount(f_cbMessage) const DRM_BYTE    *f_pbMessage,
    __in                           DRM_DWORD    f_cbMessage )
{
    DRM_RESULT  dr  = DRM_SUCCESS;

    switch( f_dwMessageType )
    {
        case 0:
            _Cleanup();
            break;
        case PRND_NO_ASSET_TX_MESSAGE_TYPE_REGISTRATION_REQUEST:
            ChkDR( _HandleRegistrationRequest( f_pbMessage, f_cbMessage, TRUE ) );
            break;
        case PRND_NO_ASSET_TX_MESSAGE_TYPE_PD_START:
            ChkDR( _HandleProximityStart( f_pbMessage, f_cbMessage ) );
            break;
        case PRND_NO_ASSET_TX_MESSAGE_TYPE_PD_RESP:
            ChkDR( _HandleProximityResponse( f_pbMessage, f_cbMessage ) );
            break;
        case PRND_NO_ASSET_TX_MESSAGE_TYPE_LICENSE_REQUEST:
            ChkDR( _HandleLicenseRequest( f_pbMessage, f_cbMessage ) );
            break;
        case PRND_NO_ASSET_TX_MESSAGE_TYPE_REGISTRATION_REQUEST_EXPIRED_REVINFO:
            ChkDR( _HandleRegistrationRequest( f_pbMessage, f_cbMessage, FALSE ) );
            break;
        default:
            ChkArg( FALSE );
    }

ErrorExit:
    return dr;
}

EXIT_PKTEST_NAMESPACE_CODE;

#endif   /* WINCE_TEST */
