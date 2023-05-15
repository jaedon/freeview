/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdafx.h>
#include <PRNDSessionManager.h>
#include <FileLogger.h>
#include <HttpServer.h>
#include <HelperFunctions.h>

using namespace std;

CPRNDSessionManager *CPRNDSessionManager::s_pSessionManager = NULL;
CDRMLock             CPRNDSessionManager::s_lock;

HRESULT CPRNDSessionManager::GetInstance( __out CPRNDSessionManager **ppSessionManager )
{
    DRM_RESULT    dr = DRM_SUCCESS;
    CDRMAutoLock  lock( &s_lock );

    *ppSessionManager  = NULL;
    if( CPRNDSessionManager::s_pSessionManager == NULL )
    {
        ChkMem( CPRNDSessionManager::s_pSessionManager = new CPRNDSessionManager());
    }

    *ppSessionManager = s_pSessionManager;

ErrorExit:
    return dr;
}

CPRNDSessionManager::CPRNDSessionManager()
{
    DRM_PtrList_Initialize( &m_ptrlistSessions, PRND_SESSION_COUNT_MAX );
}

CPRNDSessionManager::~CPRNDSessionManager()
{

}

HRESULT CPRNDSessionManager::GetPRNDSession( __out CPRNDSession **ppSession  )
{
    PRNDLogFunctionEntry();
    CDRMAutoLock  lock( &s_lock );

    HRESULT             dr              = DRM_E_FAIL;
    PRND_SESSION_ENTRY *pSessionEntry   = NULL;
    CPRNDSession       *pSession        = NULL;
    PTR_LIST           *ptrlistSessions = &m_ptrlistSessions;

    if( ptrlistSessions->dwCurrentSize > 0 )
    {
        ChkDR( DRM_PtrList_GetByIndex( ptrlistSessions, 0, ( DRM_VOID ** ) &pSessionEntry ));
        pSession = pSessionEntry->pSession;
    }

    *ppSession = pSession;
ErrorExit:
    PRNDLogFunctionExitWithResult( dr );
    return dr;
}


HRESULT CPRNDSessionManager::GetPRNDSessionFromSessionID(
                                    __in const DRM_ID         &sessionID,
                                    __out      CPRNDSession  **ppSession
                                    )
{
    PRNDLogFunctionEntry();
    CDRMAutoLock  lock( &s_lock );

    HRESULT             dr              = DRM_SUCCESS;
    PRND_SESSION_ENTRY *pSessionEntry   = NULL;
    CPRNDSession       *pSession        = NULL;
    PTR_LIST           *ptrlistSessions = &m_ptrlistSessions;

    for( DWORD i = 0; i < ptrlistSessions->dwCurrentSize; i++ )
    {
        ChkDR( DRM_PtrList_GetByIndex( ptrlistSessions, i, ( DRM_VOID ** ) &pSessionEntry ));
        if( memcmp( &pSessionEntry->sessionID.rgb, &sessionID.rgb, sizeof( sessionID.rgb ) ) == 0 )
        {
            pSession = pSessionEntry->pSession;
            break;
        }
    }

    if( pSession == NULL )
    {
        dr = DRM_E_FAIL;
    }

    *ppSession = pSession;
ErrorExit:
    PRNDLogFunctionExitWithResult( dr );
    return dr;
}


HRESULT CPRNDSessionManager::AdoptSession( __in CPRNDSession *pSession )
{
    PRNDLogFunctionEntry();

    CDRMAutoLock  lock( &s_lock );

    HRESULT             dr              = S_OK;
    PRND_SESSION_ENTRY *pSessionEntry   = NULL;
    PTR_LIST           *ptrlistSessions = &m_ptrlistSessions;
    DRM_ID              sessionID;

    AssertChkArg( pSession != NULL );

    ChkDR( pSession->GetSessionID( sessionID ) );

    ChkMem( pSessionEntry = new PRND_SESSION_ENTRY());
    pSessionEntry->pSession = pSession;

    pSessionEntry->sessionID = sessionID;

    ChkDR( DRM_PtrList_AddTail( ptrlistSessions, ( DRM_VOID * ) pSessionEntry ));
    PRNDLogMessage( "SessionCount = %d", ptrlistSessions->dwCurrentSize );

ErrorExit:
    if( FAILED( dr ) )
    {
        DELETE_OBJECT( pSessionEntry );
    }

    PRNDLogFunctionExitWithResult( dr );
    return dr;
}

HRESULT GeneratePRNDErrorResponse(
    __in    DRM_ID         *pdrmidSessionID,
    __in    HRESULT         hrResult,
    __in    CPRNDMemHolder &message,
    __inout CPRNDMemHolder &response )
{
    PRNDLogFunctionEntry();
    DRM_RESULT              dr      = DRM_SUCCESS;
    DRM_PRND_MESSAGE_TYPE   eType   = DRM_PRND_MESSAGE_TYPE_INVALID;

    DRM_BYTE    *pbResponse     = NULL;
    DRM_DWORD    dwResponseSize = 0;

    ChkArg( pdrmidSessionID != NULL );

    ChkDR( Drm_Prnd_GetMessageType( message.Data(), message.Size(), &eType ) );

    switch ( eType )
    {
        case DRM_PRND_MESSAGE_TYPE_PROXIMITY_DETECTION_START:
        case DRM_PRND_MESSAGE_TYPE_PROXIMITY_DETECTION_RESPONSE:
            ChkDR( Drm_Prnd_Transmitter_ProximityDetectionResult_Generate(
                                                                NULL,
                                                                pdrmidSessionID,
                                                                hrResult,
                                                                DRM_PRND_FLAG_NO_FLAGS,
                                                                &pbResponse,
                                                                &dwResponseSize ));
            break;

        case DRM_PRND_MESSAGE_TYPE_LICENSE_REQUEST:
            ChkDR( Drm_Prnd_Transmitter_LicenseError_Generate(
                                                                NULL,
                                                                pdrmidSessionID,
                                                                hrResult,
                                                                DRM_PRND_FLAG_NO_FLAGS,
                                                                &pbResponse,
                                                                &dwResponseSize ));
            break;

        default:
            ChkDR( DRM_E_FAIL );
            break;
    }

    response.Attach( pbResponse, dwResponseSize );

    pbResponse = NULL;
    dwResponseSize = 0;

ErrorExit:
    DRM_PRND_SAFEMEMFREE( pbResponse );

    PRNDLogFunctionExitWithResult( dr );
    return dr;
}



DWORD WINAPI CPRNDSessionManager::ProcessPRNDSessionMessagesCallback( __in LPVOID lpParam )
{
    PRNDLogFunctionEntry();

#pragma pack( push, 1 )
    struct PRND_MESSAGE_HEADER
    {
        BYTE rgbFullMessageSize[4];
        BYTE rgbSessionID[sizeof( DRM_ID )];
    };
#pragma pack( pop )


    HRESULT         dr              = S_OK;
    CPRNDSession   *pSession        = NULL;
    CPRNDSession   *pNewSession     = NULL;
    CSocket        *pClientSocket   = ( CSocket * )lpParam;

    int                     cbRecv              = 0;
    DWORD                   dwMessageSize       = 0;
    PRND_MESSAGE_HEADER     messageHeader       = {0};
    DRM_ID                  sessionID           = {0};
    DRM_PRND_MESSAGE_TYPE   eMessageType        = DRM_PRND_MESSAGE_TYPE_INVALID;

    CPRNDMemHolder          message;
    CPRNDMemHolder          response;

    CPRNDSessionManager     *pSessionManager    = NULL;


    ChkDR( CPRNDSessionManager::GetInstance( &pSessionManager ) );
    for( ;; )
    {
        PRNDLogMessage( " " );
        PRNDLogMessage( "Waiting for message" );

        //
        // Receive the header of the next message
        //

        cbRecv = sizeof( messageHeader );

        ChkDR( pClientSocket->Recv( reinterpret_cast<char*>( &messageHeader ), &cbRecv, 0 ) );

        ChkBOOL( cbRecv == sizeof( messageHeader ), DRM_E_PRND_MESSAGE_INVALID );

        NETWORKBYTES_TO_DWORD( dwMessageSize, messageHeader.rgbFullMessageSize, 0 );

        memcpy_s( sessionID.rgb, sizeof( sessionID.rgb ), messageHeader.rgbSessionID, sizeof( messageHeader.rgbSessionID ) );

        CPRNDSession::LogSessionID( sessionID );

        //
        // The size in the header includes the session ID that's in the header.
        // Subtract that to obtain the size of the PRND message.
        //

        ChkBOOL( sizeof( DRM_ID ) <= dwMessageSize, DRM_E_PRND_MESSAGE_INVALID );

        PRNDLogMessage( "Message size = %d ( 0x%X )", dwMessageSize, dwMessageSize );

        dwMessageSize -= sizeof( DRM_ID );

        //
        // Receive the PRND message
        //

        ChkDR( message.Alloc( dwMessageSize ) );

        cbRecv = ( int ) dwMessageSize;

        ChkDR( pClientSocket->Recv( ( char* ) message.Data(), &cbRecv, 0 ) );

        ChkBOOL( cbRecv == ( int ) dwMessageSize, DRM_E_PRND_MESSAGE_INVALID );


        ChkDR( Drm_Prnd_GetMessageType( message.Data(), message.Size(), &eMessageType ) );

        if( CPRNDSession::IsPRNDSessionIDValid( sessionID ) == DRM_SUCCESS )
        {
            dr = pSessionManager->GetPRNDSessionFromSessionID( sessionID, &pSession );
            if( SUCCEEDED( dr ) )
            {
                //
                // Known session ID. Use existing session object to process the message.
                //

                ChkDR( pSession->ProcessMessage( eMessageType, message, response ) );
            }
            else
            {
                //
                // Unknown session ID. Generate error.
                //

                ChkDR( CPRNDSession::TranslateErrorToResponse( DRM_E_PRND_SESSION_ID_INVALID, NULL, sessionID, eMessageType, response ) );
            }
        }
        else
        {
            //
            // Empty session ID. Create a new session object to process the message.
            //

            ChkDR( CPRNDSession::CreateInstance( &pNewSession ) );

            ChkDR( pNewSession->ProcessMessage( eMessageType, message, response ) );

            //
            // If the new session object got a valid ID after processing the message,
            // move its owership to the manager. Otherwise it doesn't represent a
            // valid session, and in that case discard it.
            //

            dr = pNewSession->GetSessionID( sessionID );
            if( SUCCEEDED( dr ) )
            {

                ChkDR( pSessionManager->AdoptSession( pNewSession ) );

                pNewSession = NULL; // adopted by manager
            }
            else
            {
                DELETE_OBJECT( pNewSession );

                dr = S_OK;
            }
        }

        //
        // If there's a reponse, transmit it to the client
        //

        if( response.Data() != NULL )
        {
            DWORD       cbSizeInNetworkByteOrder            = 0;
            DWORD       cbMessageSize                       = 0;
            int         cbMessageLengthStoreSize            = 4;

            cbMessageSize = response.Size() + sizeof( DRM_ID );
            PRNDLogMessage( "Sending error message response( including sessionID ) byte count = %d ( 0x%X )", cbMessageSize, cbMessageSize );
            DWORD_TO_NETWORKBYTES( &cbSizeInNetworkByteOrder, 0, cbMessageSize );

            cbRecv = sizeof( cbMessageSize );

            ChkDR( pClientSocket->Send( ( char* )&cbSizeInNetworkByteOrder, cbMessageLengthStoreSize ) );
            ChkDR( pClientSocket->Send( ( char* )sessionID.rgb, sizeof( sessionID.rgb )) );
            ChkDR( pClientSocket->Send( ( char* )response.Data(), ( int )response.Size() ) );
        }
    }

ErrorExit:

    pClientSocket->Close();
    DELETE_OBJECT( pClientSocket );

    DELETE_OBJECT( pNewSession );   // delete if not adopted

    PRNDLogFunctionExitWithResult( dr );
    return ( DWORD ) dr;
}

HRESULT ProcessClearMediaStreamingRequest( __in CHttpServer *pHttpServer )
{
    PRNDLogFunctionEntry();

    HRESULT             dr               = DRM_SUCCESS;
    CPRNDMediaStreamer *pMediaStreamer   = NULL;

    ChkDR( CPRNDMediaStreamer::CreatePRNDMediaStreamer( pHttpServer, &pMediaStreamer ) );
    ChkDR( pMediaStreamer->ProcessMediaStreamingRequest( pHttpServer, false, nullptr ) );

ErrorExit:
    PRNDLogFunctionExitWithResult( dr );
    DELETE_OBJECT( pMediaStreamer );

    return dr;
}


DWORD WINAPI CPRNDSessionManager::ProcessMediaStreamingRequestsCallback( __in LPVOID lpParam )
{
    PRNDLogFunctionEntry();

    HRESULT              dr              = S_OK;
    CPRNDSession        *pSession        = NULL;
    shared_ptr<CSocket>  spSocket( (CSocket * )lpParam );

    CPRNDSessionManager *pSessionManager    = NULL;
    CHttpServer         *pHttpServer        = NULL;
    DRM_ID               sessionID;
    string               strSessionID;
    vector<BYTE>         vbSessionID;

    ChkDR( CPRNDSessionManager::GetInstance( &pSessionManager ) );

    for( ;; )
    {
        PRNDLogMessage( " " );

        ChkDR( spSocket->WaitForOperation( CSocket::OP_RECV, INFINITE, NULL ) );
        DELETE_OBJECT( pHttpServer );

        ChkMem( pHttpServer = new CHttpServer( spSocket ) );
        ChkDR( pHttpServer->ReadRequest() );
        ChkDR( pHttpServer->ParseRequest() );
        strSessionID = pHttpServer->GetPRNDSessionID();
        if( strSessionID.empty() )
        {
            //ignore any message processing failures
            ProcessClearMediaStreamingRequest( pHttpServer );
        }
        else
        {
            vbSessionID = hexstring_to_bytes( strSessionID );
            ChkArg( vbSessionID.size() == sizeof( DRM_GUID ) );

            std::copy( vbSessionID.begin(), vbSessionID.end(), sessionID.rgb );

            ChkDR( pSessionManager->GetPRNDSessionFromSessionID( sessionID, &pSession ) );

            //ignore any message processing failures
            pSession->ProcessEncryptedMediaStreamingRequest( pHttpServer );
        }
        if( !spSocket->IsConnected() )
        {
            PRNDLogMessage( "!!!!Client Socket no longer connected!!!!" );
            break;
        }
    }

ErrorExit:
    spSocket->Close();
    DELETE_OBJECT( pHttpServer );

    PRNDLogFunctionExitWithResult( dr );
    return ( DWORD )dr;
}
