/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdafx.h>
#include <PRNDSession.h>
#include <FileLogger.h>
#include <PRNDRegistrationRequestProcessor.h>
#include <PRNDProximityDetectionProcessor.h>
#include <PRNDLicenseRequestProcessor.h>
#include <TransmitterConfig.h>
#include <HelperFunctions.h>

#define TRANSMITTER_PORT            27015
#define PRND_eMessageType_SIZE      8

using namespace std;

/*static*/ HRESULT CPRNDSession::CreateInstance( __out CPRNDSession **ppSession )
{
    PRNDLogFunctionEntry();
    DRM_RESULT          dr                 = DRM_SUCCESS;
    CPRNDSession       *pNewSession        = NULL;

    AssertChkArg( ppSession != NULL );

    *ppSession = NULL;

    ChkMem( pNewSession = new CPRNDSession() );
    ChkDR( pNewSession->Initialize() );

    // Hand off to caller

    *ppSession = pNewSession;
    pNewSession = NULL;

ErrorExit:

    DELETE_OBJECT( pNewSession );

    PRNDLogFunctionExitWithResult( dr );
    return dr;
}

CPRNDSession::CPRNDSession() :
    m_poAppContext( NULL ),
    m_pbOpaqueBuffer( NULL ),
    m_pbRevocationBuffer( NULL ),
    m_pMediaStreamer( NULL )
{
    ZEROMEM( &m_oTransmitterContext, sizeof( m_oTransmitterContext ));
    memset( &m_sessionID.rgb, 0, sizeof( m_sessionID.rgb ) );

    m_spLicenseGenerator            = nullptr;
    m_spProximityDetectionConfig    = nullptr;
}

CPRNDSession::~CPRNDSession()
{
    PRNDLogFunctionEntry();

    Shutdown();

    PRNDLogFunctionExit();
}

HRESULT CPRNDSession::Initialize()
{
    PRNDLogFunctionEntry();
    DRM_RESULT          dr                  = DRM_SUCCESS;
    DRM_CONST_STRING    dstrDataStoreFile   = {0};
    string              strPath;
    wstring             wstrPath;
    string              strCRLPackage;

    ChkDR( CTransmitterConfig::Instance().GetPlayReadyHDSFileFullPath( strPath ) );

    wstrPath =  string_to_wstring( strPath );
    dstrDataStoreFile.pwszString = reinterpret_cast< const DRM_WCHAR *> ( wstrPath.c_str() );
    dstrDataStoreFile.cchString  = ( DRM_DWORD ) wstrPath.length();

    ChkDR( DRMTOOLS_DrmInitializeWithOpaqueBuffer(
                NULL,
                &dstrDataStoreFile,
                TOOLS_DRM_BUFFER_SIZE,
                &m_pbOpaqueBuffer,
                &m_poAppContext ) );

    ChkDR( UpdateRevocationPackage() );

    ChkDR( Drm_Prnd_Transmitter_BeginSession( m_poAppContext, &m_oTransmitterContext ) );

ErrorExit:
    PRNDLogFunctionExitWithResult( dr );
    return dr;
}

HRESULT CPRNDSession::UpdateRevocationPackage()
{
    PRNDLogFunctionEntry();
    DRM_RESULT          dr                  = DRM_SUCCESS;
    string              strPath;
    string              strCRLPackage;

    ChkDR( CTransmitterConfig::Instance().GetPlayReadyHDSFileFullPath( strPath ) );
    ChkDR( CTransmitterConfig::Instance().GetCRLPackage( strCRLPackage ) );

    SAFE_OEM_FREE( m_pbRevocationBuffer );
    ChkMem( m_pbRevocationBuffer = ( DRM_BYTE * )Oem_MemAlloc( REVOCATION_BUFFER_SIZE ) );

    ChkDR( Drm_Revocation_SetBuffer( m_poAppContext, m_pbRevocationBuffer, REVOCATION_BUFFER_SIZE ) );
    ChkDR( Drm_Revocation_StorePackage( m_poAppContext, strCRLPackage.c_str(), ( DRM_DWORD ) strCRLPackage.size() ) );

ErrorExit:
    PRNDLogFunctionExitWithResult( dr );
    return dr;
}

void CPRNDSession::Shutdown()
{
    DELETE_OBJECT( m_pMediaStreamer );
    Drm_Prnd_Transmitter_EndSession( m_poAppContext );

    SAFE_OEM_FREE( m_pbRevocationBuffer );

    DRMTOOLS_DrmUninitializeWithOpaqueBuffer(
                &m_pbOpaqueBuffer,
                &m_poAppContext );
}

HRESULT CPRNDSession::GetMessageProcessor(
                                        __in  DRM_PRND_MESSAGE_TYPE     eMessageType,
                                        __out CPRNDMessageProcessor   **ppMessageProcessor
                                        )
{
    PRNDLogFunctionEntry();
    HRESULT dr = DRM_SUCCESS;
    CPRNDMessageProcessor* pMessageProcessor = NULL;

    switch( eMessageType )
    {
        case DRM_PRND_MESSAGE_TYPE_REGISTRATION_REQUEST:

            PRNDLogMessage( "******************      Creating CPRNDRegistrationRequestProcessor         ********************* " );
            ChkDR( UpdateRevocationPackage());
            ChkMem( pMessageProcessor = new CPRNDRegistrationRequestProcessor );
            break;

        case DRM_PRND_MESSAGE_TYPE_PROXIMITY_DETECTION_START:

            PRNDLogMessage( "******************      Creating CPRNDProximityDetectionStartProcessor         ********************* " );
            ChkMem( pMessageProcessor = new CPRNDProximityDetectionStartProcessor );
            break;

        case DRM_PRND_MESSAGE_TYPE_PROXIMITY_DETECTION_RESPONSE:

            PRNDLogMessage( "******************      Creating CPRNDProximityDetectionResponseProcessor         ********************* " );
            ChkMem( pMessageProcessor = new CPRNDProximityDetectionResponseProcessor );
            break;

        case DRM_PRND_MESSAGE_TYPE_LICENSE_REQUEST:

            PRNDLogMessage( "******************      Creating CPRNDLicenseRequestProcessor         ********************* " );
            ChkDR( UpdateRevocationPackage());
            ChkMem( pMessageProcessor = new CPRNDLicenseRequestProcessor );
            break;

        default:

            if( eMessageType == DRM_PRND_MESSAGE_TYPE_INVALID )
            {
                PRNDLogMessage( "!!!!!!!!Invalid Message Type!!!!!!!!!", eMessageType );
            }
            else
            {
                PRNDLogMessage( "!!!!!!!!Unsupported Message Type [%d] !!!!!!!!!", eMessageType );
            }

            dr = DRM_E_FAIL;
            break;
    }

    *ppMessageProcessor = pMessageProcessor;
ErrorExit:
    PRNDLogFunctionExitWithResult( dr );
    return dr;
}

HRESULT CPRNDSession::GetSessionID( __out DRM_ID& sessionID )
{
    PRNDLogFunctionEntry();
    HRESULT dr = DRM_SUCCESS;

    ChkDR( CPRNDSession::IsPRNDSessionIDValid( m_sessionID ));

    sessionID = m_sessionID;

ErrorExit:
    PRNDLogFunctionExitWithResult( dr );
    return dr;
}

HRESULT CPRNDSession::IsPRNDSessionIDValid( __in const DRM_ID& sessionID )
{
    HRESULT dr                      = DRM_SUCCESS;
    DRM_ID  invalidSessionID        = {0};

    if( memcmp( sessionID.rgb, invalidSessionID.rgb, sizeof( invalidSessionID.rgb ) ) == 0 )
    {
        dr = DRM_E_FAIL;
    }

    return dr;
}

void CPRNDSession::LogSessionID( __in const DRM_ID& sessionID )
{
    PRNDLogMessage( "SessionID = " );
    PRNDLogHexDump( ( BYTE* )( &sessionID ), sizeof( sessionID ), false );
}

// Tries to convert an error code into a PRND error response.
// If the conversion is possible, the method returns success and
// puts the error response in the response argument. If the
// conversion is not possible or an error occurrs, it returns
// the error code
/*static*/ HRESULT CPRNDSession::TranslateErrorToResponse(
                                        __in        HRESULT                 drIn,
                                        __in_opt    DRM_APP_CONTEXT        *poAppContext,
                                        __in        DRM_ID                 &sessionID,
                                        __in        DRM_PRND_MESSAGE_TYPE   eMessageType,
                                        __inout     CPRNDMemHolder&         response
                                        )
{
    PRNDLogFunctionEntry();

    HRESULT     dr          = DRM_SUCCESS;
    DRM_BYTE*   pbResponse  = NULL;
    DRM_DWORD   cbResponse  = 0;

    AssertChkArg( FAILED( drIn ) );

    switch( eMessageType )
    {
        case DRM_PRND_MESSAGE_TYPE_REGISTRATION_REQUEST:

            ChkDR( Drm_Prnd_Transmitter_RegistrationError_Generate( poAppContext, drIn, DRM_PRND_FLAG_NO_FLAGS, &pbResponse, &cbResponse ) );
            break;

        case DRM_PRND_MESSAGE_TYPE_PROXIMITY_DETECTION_START:
        case DRM_PRND_MESSAGE_TYPE_PROXIMITY_DETECTION_RESPONSE:

            ChkDR( Drm_Prnd_Transmitter_ProximityDetectionResult_Generate( poAppContext, &sessionID, drIn, DRM_PRND_FLAG_NO_FLAGS, &pbResponse, &cbResponse ) );
            break;

        case DRM_PRND_MESSAGE_TYPE_LICENSE_REQUEST:

            ChkDR( Drm_Prnd_Transmitter_LicenseError_Generate( poAppContext, NULL, drIn, DRM_PRND_FLAG_NO_FLAGS, &pbResponse, &cbResponse ) );
            break;

        default:

            // Message type not translatable to code.
            // Return the original error.
            ChkDR( drIn );
            break;
    }

    // Replace error with response

    DRMASSERT( pbResponse != NULL );
    DRMASSERT( cbResponse != 0 );

    response.Attach( pbResponse, cbResponse );
    pbResponse = NULL;

ErrorExit:

    DRM_PRND_SAFEMEMFREE( pbResponse );

    PRNDLogFunctionExitWithResult( dr );
    return dr;
}


HRESULT CPRNDSession::ProcessMessage(
                                __in const DRM_PRND_MESSAGE_TYPE  eMessageType,
                                __inout    CPRNDMemHolder        &message,
                                __inout    CPRNDMemHolder        &response
                                )
{
    PRNDLogFunctionEntry();

    HRESULT                 dr                                  = DRM_SUCCESS;
    CPRNDMessageProcessor  *pMessageProcessor                   = NULL;

    CPRNDRegistrationRequestProcessor           *pRegistrationProcessor = NULL;
    CPRNDProximityDetectionProcessor            *pPDProcessor           = NULL;
    CPRNDLicenseRequestProcessor                *pLicenseProcessor      = NULL;

    ChkDR( GetMessageProcessor( eMessageType, &pMessageProcessor ) );

    pRegistrationProcessor = dynamic_cast< CPRNDRegistrationRequestProcessor *>( pMessageProcessor );
    if( pRegistrationProcessor )
    {
        ChkDR( pRegistrationProcessor->ProcessMessage( m_poAppContext, message, response ) );

        pRegistrationProcessor->GetSessionID( m_sessionID );

        CPRNDSession::LogSessionID( m_sessionID );

        m_spProximityDetectionConfig = pRegistrationProcessor->GetProximityDetectionConfig();
    }
    else
    {
        // Must register before attempting proximity

        ChkBOOL( m_spProximityDetectionConfig != NULL, E_NOT_VALID_STATE );

        pPDProcessor = dynamic_cast< CPRNDProximityDetectionProcessor *>( pMessageProcessor );
        if( pPDProcessor )
        {
            pPDProcessor->SetProximityDetectionConfig( m_spProximityDetectionConfig );
        }

        ChkDR( CPRNDSession::IsPRNDSessionIDValid( m_sessionID ));

        pMessageProcessor->SetSessionID( m_sessionID );

        ChkDR( pMessageProcessor->ProcessMessage( m_poAppContext, message, response ) );
    }

    pLicenseProcessor = dynamic_cast< CPRNDLicenseRequestProcessor *>( pMessageProcessor );
    if( pLicenseProcessor )
    {
        m_spLicenseGenerator = pLicenseProcessor->GetLicenseGenerator();
    }

ErrorExit:

    if( FAILED( dr ) )
    {
        dr = TranslateErrorToResponse( dr, m_poAppContext, m_sessionID, eMessageType, response );
    }

    DELETE_OBJECT( pMessageProcessor );

    PRNDLogFunctionExitWithResult( dr );

    return dr;
}

HRESULT CPRNDSession::StartEncryptedMediaStreaming( CHttpServer *pHttpServer )
{
    PRNDLogFunctionEntry();
    CDRMAutoLock lock( &m_lock );

    HRESULT             dr              = DRM_SUCCESS;
    CPRNDMediaStreamer *pMediaStreamer  = NULL;

    void *pvMediaStreamer = InterlockedCompareExchangePointer( ( volatile PVOID * )&m_pMediaStreamer, NULL, NULL );
    ChkArg( pvMediaStreamer == NULL );

    ChkDR( CPRNDMediaStreamer::CreatePRNDMediaStreamer( pHttpServer, &pMediaStreamer ) );
    InterlockedExchangePointer( reinterpret_cast<void**>( &m_pMediaStreamer ), pMediaStreamer );

    ChkDR( pMediaStreamer->ProcessMediaStreamingRequest( pHttpServer, true, m_spLicenseGenerator ) );

ErrorExit:
    InterlockedExchangePointer( reinterpret_cast<void**>( &m_pMediaStreamer ), NULL );
    DELETE_OBJECT( pMediaStreamer );
    PRNDLogFunctionExitWithResult( dr );
    return dr;
}

HRESULT CPRNDSession::ProcessEncryptedMediaStreamingRequest( CHttpServer *pHttpServer )
{
    PRNDLogFunctionEntry();

    HRESULT   dr            = DRM_SUCCESS;

    if( m_spLicenseGenerator )
    {
        void *pvMediaStreamer = InterlockedCompareExchangePointer( ( volatile PVOID * )&m_pMediaStreamer, NULL, NULL );
        if( pvMediaStreamer != NULL )
        {
            PRNDLogMessage( "Shutdown existing MediaStreamer!!!" );
            m_pMediaStreamer->Shutdown();

            PRNDLogMessage( "Waiting for other thread to cleanup..." );
            while( pvMediaStreamer != NULL )
            {
                Sleep( 1000 );
                pvMediaStreamer = InterlockedCompareExchangePointer( ( volatile PVOID * )&m_pMediaStreamer, NULL, NULL );
            }
        }

        ChkDR( StartEncryptedMediaStreaming( pHttpServer ) );
    }
    else
    {
        PRNDLogMessage( "No Licenses available for encryption. Perform license acquisition before request for content streaming  !!!!!!!!!" );
        ChkDR( pHttpServer->SendResponseFileNotFound() );
    }

ErrorExit:
    PRNDLogFunctionExitWithResult( dr );
    return dr;
}

