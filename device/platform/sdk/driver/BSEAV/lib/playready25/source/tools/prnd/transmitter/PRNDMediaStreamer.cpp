/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdafx.h>
#include <PRNDMediaStreamer.h>
#include <FileLogger.h>
#include <TransmitterConfig.h>
#include <HelperFunctions.h>
#include <FuzzFunctions.h>

using namespace std;

CPRNDMediaStreamer::CPRNDMediaStreamer()
{
    m_pHttpServer   = nullptr;

    m_hEncryptor    = DRM_M2TS_ENCRYPTOR_HANDLE_INVALID;
    memset( &m_KeyId, 0, sizeof( DRM_GUID ) );

    m_spLicenseGenerator    = nullptr;
    m_spTSFileWriter        = nullptr;
    m_spClearTSFileWriter   = nullptr;
    
    m_fpEncryptionLog   = nullptr;
    m_bLiveStreaming    = false;
    m_dwLastTimeFuzzed  = 0;

    m_bShutdownStreaming = FALSE;
    m_hStreamingFinished = CreateEvent( NULL, TRUE, TRUE, NULL ); //signaled manual-reset event
}

CPRNDMediaStreamer::~CPRNDMediaStreamer()
{
    PRNDLogFunctionEntry();

    Shutdown();
    CLOSE_HANDLE( m_hStreamingFinished );

    PRNDLogFunctionExit();
}

HRESULT CPRNDMediaStreamer::Shutdown()
{
    PRNDLogFunctionEntry();
    CDRMAutoLock lock( &m_lock );

    HRESULT    dr  = DRM_SUCCESS;

    InterlockedExchange( ( LONG* )&m_bShutdownStreaming, TRUE );
    WaitForSingleObject( m_hStreamingFinished, INFINITE );
    
    if( m_hEncryptor != DRM_M2TS_ENCRYPTOR_HANDLE_INVALID )
    {
        ChkDR( Drm_M2ts_Uninitialize( m_hEncryptor ) );
        m_hEncryptor = DRM_M2TS_ENCRYPTOR_HANDLE_INVALID;
    }

    if( m_fpEncryptionLog )
    {
        fclose( m_fpEncryptionLog );
        m_fpEncryptionLog = nullptr;
    }

    m_spLicenseGenerator    = nullptr;
    m_spTSFileWriter        = nullptr;
    m_spClearTSFileWriter   = nullptr;

ErrorExit:
    PRNDLogFunctionExitWithResult( dr );
    return dr;
}

HRESULT CPRNDMediaStreamer::LogEncryptionTrace( const DRM_M2TS_LOG *poLog )
{
    HRESULT dr    = DRM_SUCCESS;

    if( m_fpEncryptionLog == NULL )
    {
        return dr;
    }

    switch ( poLog->eDetailsType )
    {
        case eDRM_M2TS_LOG_DETAILS_TYPE_PACKET:
            fprintf( m_fpEncryptionLog, "0x%8.8x occured in TS packet, PID=%d, Packet#=%d, Dropped=%s\n", 
                    poLog->drResult, 
                    poLog->Details.Packet.wPID, 
                    poLog->Details.Packet.dwPacketNumber,
                    poLog->Details.Packet.fDropped ? "Yes" : "No" );
            break;

        case eDRM_M2TS_LOG_DETAILS_TYPE_PES:
            fprintf( m_fpEncryptionLog, "0x%8.8x occured in PES packet, PID=%d, StreamType=%d, PTS=%u, FirstPacket#=%d, LastPacket#=%d, Dropped=%s\n", 
                    poLog->drResult, 
                    poLog->Details.PES.wPID, 
                    poLog->Details.PES.bStreamType,
                    DRM_UI64Low32( poLog->Details.PES.qwPTS ),
                    poLog->Details.PES.dwFirstPacketNumber,
                    poLog->Details.PES.dwLastPacketNumber,
                    poLog->Details.PES.fDropped ? "Yes" : "No" );
            break;

    case eDRM_M2TS_LOG_DETAILS_TYPE_KEY_ROTATION:
            // Convert qwAggregatedDuration from hns to seconds
            fprintf( m_fpEncryptionLog, "Key rotated after %u seconds\n",
                    DRM_UI64Div( poLog->Details.KeyRotation.qwAggregatedDuration, DRM_UI64HL( 0, 10000000 ) ) );
            break;
    }
    
    return dr;
}

void DRM_CALL CPRNDMediaStreamer::Encryption_Log_Callback( __inout DRM_VOID *pCallbackContext, __in const DRM_M2TS_LOG *poLog )
{
    if( poLog == NULL || pCallbackContext == NULL )
    {
        return;
    }
    
    CPRNDMediaStreamer *pMediaStreamer = reinterpret_cast<CPRNDMediaStreamer *>( pCallbackContext );
    if( pMediaStreamer != NULL )
    {
        pMediaStreamer->LogEncryptionTrace( poLog );
    }
}

HRESULT CPRNDMediaStreamer::GetArchiveFileNameForClearContent( std::string &strFileName )
{
    PRNDLogFunctionEntry();

    HRESULT     dr              = DRM_SUCCESS;
    string      strChannelName;
    std::string fileNameKey;

    ChkArg( m_pHttpServer != nullptr );

    fileNameKey = m_pHttpServer->GetArchiveFileKey();
    if( !string_compare_case_insensitive( fileNameKey, "" ) )
    {
        strChannelName = fileNameKey;
    }    
    else if( m_pHttpServer->GetContentRequestType() == CONTENT_REQUEST_TYPE_CHANNEL_NAME )
    {
        strChannelName = m_pHttpServer->GetContentSource();
    }
    else if( m_pHttpServer->GetContentRequestType() == CONTENT_REQUEST_TYPE_CHANNEL_HTTP_URL )
    {
        string str = m_pHttpServer->GetContentSource();
        strChannelName = str.substr( str.find_last_of( "/" ) + 1 );
    }

    strChannelName += "_ClearContent_";
    strChannelName += std::to_string( ( _Longlong ) GetCurrentTimeInSeconds() );
    strChannelName += ".ts";

    strFileName = strChannelName;
ErrorExit:
    PRNDLogFunctionExitWithResult( dr );
    return dr;
}

HRESULT CPRNDMediaStreamer::GetArchiveFileNameForEncryptedContent( std::string &strFileName )
{
    PRNDLogFunctionEntry();

    HRESULT     dr              = DRM_SUCCESS;
    string      strChannelName;
    std::string fileNameKey;

    ChkArg( m_pHttpServer != nullptr );

    fileNameKey = m_pHttpServer->GetArchiveFileKey();
    if( !string_compare_case_insensitive( fileNameKey, "" ) )
    {
        strChannelName = fileNameKey;
    }    
    else if( m_pHttpServer->GetContentRequestType() == CONTENT_REQUEST_TYPE_CHANNEL_NAME )
    {
        strChannelName = m_pHttpServer->GetContentSource();
    }
    else if( m_pHttpServer->GetContentRequestType() == CONTENT_REQUEST_TYPE_CHANNEL_HTTP_URL )
    {
        string str = m_pHttpServer->GetContentSource();
        strChannelName = str.substr( str.find_last_of( "/" ) + 1 );
    }

    strChannelName += "_EncryptedContent_";
    strChannelName += std::to_string( ( _Longlong ) GetCurrentTimeInSeconds() );
    strChannelName += ".ts";

    strFileName = strChannelName;
ErrorExit:
    PRNDLogFunctionExitWithResult( dr );
    return dr;
}

HRESULT CPRNDMediaStreamer::InitializeEncryption( std::shared_ptr<CLicenseGenerator>  spLicenseGenerator )
{
    PRNDLogFunctionEntry();

    HRESULT                 dr      = DRM_SUCCESS;
    string                  strArchivePath;
    string                  strTracePath;
    
    DRM_M2TS_ENCRYPTION_PROPERTY oEncryptorProperty;

    memset( &oEncryptorProperty, 0, sizeof( DRM_M2TS_ENCRYPTION_PROPERTY ) );
    m_spLicenseGenerator = spLicenseGenerator;
    ChkDR( spLicenseGenerator->GetKeyId( &m_KeyId ) );
    
    ChkDR( Drm_M2ts_Initialize( 0, &m_hEncryptor ) );
    oEncryptorProperty.ePropertyType = eDRM_M2TS_PROPERTY_TYPE_ENCYRPT_AUDIO;
    oEncryptorProperty.Value.fValue = false;
    if( spLicenseGenerator->EncryptAudio() )
    {
        oEncryptorProperty.Value.fValue = true;
    }
    ChkDR( Drm_M2ts_SetEncryptionProperty( m_hEncryptor, &oEncryptorProperty ) );
    
    strTracePath = CTransmitterConfig::Instance().GetEncryptorTraceFilePath();
    if( !strTracePath.empty() )
    {
        oEncryptorProperty.ePropertyType = eDRM_M2TS_PROPERTY_TYPE_ENABLE_LOGGING;
        oEncryptorProperty.Value.LoggingCallback.pCallbackContext = this;
        oEncryptorProperty.Value.LoggingCallback.pfnLoggingCallback = CPRNDMediaStreamer::Encryption_Log_Callback;

        ChkDR( Drm_M2ts_SetEncryptionProperty( m_hEncryptor, &oEncryptorProperty ) );
        m_fpEncryptionLog = fopen( strTracePath.c_str(), "w+" );
    }

    strArchivePath = CTransmitterConfig::Instance().GetEncryptedContentArchivePath();

    if( !strArchivePath.empty() )
    {
        string strFileName;
        ChkDR( GetArchiveFileNameForEncryptedContent( strFileName ) );

        if( !string_ends_with( strArchivePath, "\\" ) )
        {
            strArchivePath += "\\";
        }
        strArchivePath += strFileName;  

        PRNDLogMessage( "Creating archive file for encrypted content = %s", strArchivePath.c_str() );
        m_spTSFileWriter = make_shared<CTSFileWriter>( string_to_wstring( strArchivePath ) );
    }

    strArchivePath = CTransmitterConfig::Instance().GetClearContentArchivePath();

    if( !strArchivePath.empty() )
    {
        string strFileName;
        ChkDR( GetArchiveFileNameForClearContent( strFileName ) );

        if( !string_ends_with( strArchivePath, "\\" ) )
        {
            strArchivePath += "\\";
        }
        strArchivePath += strFileName;

        PRNDLogMessage( "Creating archive file for clear content = %s", strArchivePath.c_str() );
        m_spClearTSFileWriter = make_shared<CTSFileWriter>( string_to_wstring( strArchivePath ) );
    }
    
ErrorExit:
    PRNDLogFunctionExitWithResult( dr );
    return dr;
}


HRESULT CPRNDMediaStreamer::SetEncryptionKeyData()
{
    HRESULT                 dr          = DRM_SUCCESS;
    DRM_LICENSE_HANDLE      hLicense    = DRM_LICENSE_HANDLE_INVALID;
    DRM_M2TS_PRO_TYPE       proType     = eDRM_M2TS_PRO_TYPE_NO_PRO;
    DWORD                   dwDuration  = 0;
    DRM_KID                 oKeyId      = {0};
        
    ChkDR( m_spLicenseGenerator->CreateLicenseForEncryption( &hLicense, &oKeyId ) );
    if( m_spLicenseGenerator->IsKeyRotationRequired() )
    {
        proType     = eDRM_M2TS_PRO_TYPE_KEY_ROTATION_WITH_LICENSE_IN_PRO;
        dwDuration  = m_spLicenseGenerator->GetKeyRotationDuration();
    }
    
    ChkDR( Drm_M2ts_SetKeyData(
                m_hEncryptor,
                hLicense,
   ( DRM_GUID* ) &oKeyId,
                proType,
                dwDuration ) );
    PRNDLogMessage( "Duration = %d sec, KID = ", dwDuration );
    PRNDLogHexDump( ( BYTE* )&oKeyId, sizeof( DRM_ID ), false );
    
ErrorExit:
    if( hLicense != DRM_LICENSE_HANDLE_INVALID )
    {
        Drm_LocalLicense_Release( &hLicense );
    }
    if( FAILED( dr ) )
    {
        PRNDLogFunctionExitWithResult( dr );
    }
    return dr;
}


HRESULT CPRNDMediaStreamer::Encrypt( vector<BYTE>& vbDataIn, vector<BYTE>& vbDataOut, bool bLastPacket  )
{
    HRESULT    dr     = DRM_SUCCESS;
    DRM_DWORD  dwSize = ( DRM_DWORD ) vbDataOut.size();

    ChkArg( m_hEncryptor != DRM_M2TS_ENCRYPTOR_HANDLE_INVALID );

/*
** The constructions &vbDataIn[0] and  &vbDataOut[0] to access the data in a vector works with the MS version of STL but
** is not guaranteed to work. You should check if this works on your implementation.
*/
#if !defined( _MSC_VER )
#error Double-Check the way your STL accesses Vector Data
#endif
    dr = Drm_M2ts_Encrypt(
                            m_hEncryptor,
                            vbDataIn.empty() ? NULL: &vbDataIn[0],
                            ( DRM_DWORD ) vbDataIn.size(),
                           &vbDataOut[0],
                           &dwSize,
                            bLastPacket );
    vbDataOut.resize( dwSize );

    if( dr == DRM_E_M2TS_NEED_KEY_DATA )
    {
        dr = SetEncryptionKeyData();
    }    

ErrorExit:
    if( FAILED( dr ) )
    {
        PRNDLogFunctionExitWithResult( dr );
    }
    return dr;
}


HRESULT CPRNDMediaStreamer::GeneratePlayReadyHeader( string &strHexPlayReadyHeader )
{
    DRM_RESULT dr           = DRM_SUCCESS;
    DRM_DWORD  cbHeaderSize = 0;
    DRM_BYTE  *pbHeader     = NULL;
    char      *pEncodedData = NULL;

    if( m_spLicenseGenerator->IsKeyRotationRequired() )
    {
        ChkDR( Drm_LocalLicense_CreateRMHeader(
                                NULL,
                                &cbHeaderSize,
                                &pbHeader ) );
    }
    else
    {
        ChkDR( Drm_LocalLicense_CreateRMHeader(
                    ( DRM_KID * ) &m_KeyId,
                                &cbHeaderSize,
                                &pbHeader ) );
        PRNDLogMessage( "KID = " );
        PRNDLogHexDump( ( BYTE* )&m_KeyId, sizeof( DRM_ID ), false );
    }
    PRNDLogHexDump( ( BYTE* )pbHeader, cbHeaderSize, true );

    ChkDR( Base16Encode( pbHeader, cbHeaderSize, &pEncodedData ) );
    strHexPlayReadyHeader.resize( cbHeaderSize*2 );
    memcpy_s( &strHexPlayReadyHeader[0], strHexPlayReadyHeader.size(), pEncodedData, strHexPlayReadyHeader.size() );

ErrorExit:

    DRM_PRND_SAFEMEMFREE( pbHeader );
    return dr;
}

HRESULT CPRNDMediaStreamer::StartMediaStreamUsingChunkedTransfer( bool fEncrypted )
{
    PRNDLogFunctionEntry();

    HRESULT  dr  = DRM_SUCCESS;
    string   strHexPlayReadyHeader;

    ChkArg( m_pHttpServer   != nullptr );
    if( fEncrypted )
    {
        PRNDLogMessage( "Starting Protected Stream..." );
        ChkArg( m_hEncryptor    != DRM_M2TS_ENCRYPTOR_HANDLE_INVALID );

        PRNDLogMessage( "Generating Playready header..." );
        ChkDR( GeneratePlayReadyHeader( strHexPlayReadyHeader ) );
        ChkDR( m_pHttpServer->SendResponseHeaders( strHexPlayReadyHeader ) );
    }
    else
    {
        PRNDLogMessage( "Starting Clear Stream..." );
        ChkDR( m_pHttpServer->SendResponseHeaders( "" ) );
    }
ErrorExit:
    PRNDLogFunctionExitWithResult( dr );
    return dr;
}

HRESULT CPRNDMediaStreamer::SendMediaStreamUsingChunkedTransfer( bool fEncrypted )
{
    PRNDLogFunctionEntry();

    HRESULT         dr                  = DRM_SUCCESS;
    HRESULT         drCached            = DRM_SUCCESS;

    vector<BYTE>    vbPacketIn;
    vector<BYTE>    vbPacketOut;

    ChkArg( m_pHttpServer   != nullptr );

    if( fEncrypted )
    {
        PRNDLogMessage( "Encryption enabled." );
    }
    else
    {
        PRNDLogMessage( "Encryption not enabled. Clear content streaming" );
    }

    while( true )
    {
        BOOL bShutdownStreaming = InterlockedExchange( ( LONG* )&m_bShutdownStreaming, FALSE );
        if( bShutdownStreaming )
        {
            PRNDLogMessage( "Shutdown streaming..." );
            break;
        }

        if( fEncrypted )
        {
            if( drCached == DRM_S_MORE_DATA )
            {
                vbPacketIn.resize( 0 );
            }
            else
            {
                vbPacketIn = GetPacket();
            }

            vbPacketOut.resize( TS_PACKET_SIZE, 0 );
            drCached = Encrypt( vbPacketIn, vbPacketOut, false );
            ChkDR( drCached );
        }
        else
        {
            vbPacketOut = GetPacket();
        }
        
        if( !vbPacketOut.empty() )
        {
            ChkDR( SendChunk( vbPacketOut ) );
        }

        if( vbPacketIn.empty() && vbPacketOut.empty() && !m_bLiveStreaming )
        {
            PRNDLogMessage( "No packets to stream, quitting..." );            
            break;
        }
    }
    
ErrorExit:
    PRNDLogFunctionExitWithResult( dr );
    return dr;
}

std::vector<BYTE> CPRNDMediaStreamer::GetPacket()
{
    std::vector<BYTE> vbPacket = GetNextPacket();

    if( m_spClearTSFileWriter )
    {
        m_spClearTSFileWriter->WritePacket( vbPacket, false );
    }

    return vbPacket;
}

HRESULT CPRNDMediaStreamer::SendChunk( vector<BYTE>& vb )
{
    HRESULT     dr      = DRM_SUCCESS;
    
    ChkArg( vb.size() != 0 );
    
    if( m_spTSFileWriter )
    {
        m_spTSFileWriter->WritePacket( vb, false );
    }
    
#if 0    
    DWORD       dwNow   = GetCurrentTimeInSeconds();

    if( dwNow - m_dwLastTimeFuzzed > 10 )
    {
        PRNDLogMessage( "CPRNDMediaStreamer::SendChunk() -- Fuzzing encrypted packet" );
        m_dwLastTimeFuzzed = dwNow;
        DropTSPacket( vb, 1 );
    }
#endif
    
    ChkDR( m_pHttpServer->SendResponseChunk( vb ) );

ErrorExit:
    if( FAILED( dr ) )
    {
        PRNDLogMessage( "CPRNDMediaStreamer::SendChunk() FAILED !!!!" );
    }
    return dr;
    
}

HRESULT CPRNDMediaStreamer::EndMediaStreamUsingChunkedTransfer()
{
    PRNDLogFunctionEntry();

    HRESULT  dr  = DRM_SUCCESS;

    ChkArg( m_pHttpServer   != nullptr );
    PRNDLogMessage( "Sending chunk end marker..." );
    ChkDR( m_pHttpServer->SendResponseChunkEnd() );

ErrorExit:
    PRNDLogFunctionExitWithResult( dr );
    return dr;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CHttpMediaStreamer::CHttpMediaStreamer()
{
    m_spHttpClient = nullptr;
}

CHttpMediaStreamer::~CHttpMediaStreamer()
{
    PRNDLogFunctionEntry();

    PRNDLogFunctionExit();
}

HRESULT CHttpMediaStreamer::ProcessMediaStreamingRequest( 
                                        CHttpServer                        *pHttpServer, 
                                        bool                                fEncrypted,
                                        std::shared_ptr<CLicenseGenerator>  spLicenseGenerator )
{
    PRNDLogFunctionEntry();
    HRESULT                 dr          = DRM_SUCCESS;
    
    string                      strHttpPath;
    string                      strChannel;
    shared_ptr<CHttpChannel>    spChannel;
    string                      header;
    string::size_type           pos = 0;

    if( !ResetEvent( m_hStreamingFinished ) )
    {
        dr = HRESULT_FROM_WIN32( GetLastError() );
        goto ErrorExit;
    }
    
    m_pHttpServer       = pHttpServer;
    if( fEncrypted )
    {
        ChkDR( InitializeEncryption( spLicenseGenerator ) );
    }
    m_bLiveStreaming    = true;

    if( pHttpServer->GetContentRequestType() == CONTENT_REQUEST_TYPE_CHANNEL_HTTP_URL )
    {
        strHttpPath = pHttpServer->GetContentSource();
    }
    else
    {
        strChannel = pHttpServer->GetContentSource();
        spChannel   = std::dynamic_pointer_cast< CHttpChannel >( CTransmitterConfig::Instance().GetChannel( strChannel ) );
        strHttpPath = spChannel->GetHttpPath();
    }
    
    PRNDLogMessage( "Creating Http channel for %s", strHttpPath );
    m_spHttpClient = make_shared<CHttpClient>( strHttpPath );

    ChkDR( m_spHttpClient->ReadHttpStream() );
    
    // check for 404 not found and if that's what we received, return the same...
    header = m_spHttpClient->GetResponseHeader();
    if( header.find( "404", pos ) != std::string::npos )
    {
        ChkDR( m_pHttpServer->SendResponseFileNotFound() );
    }
    else // start streaming
    {
        ChkDR( m_spHttpClient->ReadResponseBodyAsync() );        
        ChkDR( StartMediaStreamUsingChunkedTransfer( fEncrypted ) );
        ChkDR( SendMediaStreamUsingChunkedTransfer( fEncrypted ) );
        ChkDR( EndMediaStreamUsingChunkedTransfer() );
    }
    
ErrorExit:
    m_spHttpClient = nullptr;
    
    SetEvent( m_hStreamingFinished );
    PRNDLogFunctionExitWithResult( dr );
    return dr;
    
}

std::vector<BYTE> CHttpMediaStreamer::GetNextPacket()
{
    if( m_spHttpClient->ShutdownInitiated() )
    {
        m_bLiveStreaming = false;
    }

    return m_spHttpClient->GetPacket();
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CTSFileMediaStreamer::CTSFileMediaStreamer()
{
    m_pTSFile       = nullptr;
}

CTSFileMediaStreamer::~CTSFileMediaStreamer()
{
    PRNDLogFunctionEntry();
    DELETE_OBJECT( m_pTSFile );

    PRNDLogFunctionExit();
}

HRESULT CTSFileMediaStreamer::ProcessMediaStreamingRequest( 
                                        CHttpServer                        *pHttpServer, 
                                        bool                                fEncrypted,
                                        std::shared_ptr<CLicenseGenerator>  spLicenseGenerator )
{
    PRNDLogFunctionEntry();
    HRESULT                 dr          = DRM_SUCCESS;
    string                  strFilePath;

    if( !ResetEvent( m_hStreamingFinished ) )
    {
        dr = HRESULT_FROM_WIN32( GetLastError() );
        goto ErrorExit;
    }

    m_pHttpServer = pHttpServer;
    if( fEncrypted )
    {
        ChkDR( InitializeEncryption( spLicenseGenerator ) );
    }
    
    if( pHttpServer->GetContentRequestType() == CONTENT_REQUEST_TYPE_FILE )
    {
        string  strHttpPath = m_pHttpServer->GetContentSource();
        ChkDR( CTransmitterConfig::Instance().GetFilePathFromHttpPath( strHttpPath, strFilePath ) );
        m_qTSFiles.push_back( strFilePath );
    }
    else
    {
        string strChannel = m_pHttpServer->GetContentSource();
        shared_ptr<CTSFileChannel> spChannel = std::dynamic_pointer_cast<CTSFileChannel>( CTransmitterConfig::Instance().GetChannel( strChannel.c_str() ) );
        PRNDLogMessage( "Opening Channel %s", strChannel.c_str() );

        if( spChannel )
        {
            const std::vector<std::string>& vTSFiles = spChannel->GetTSFiles();
            m_qTSFiles.insert( m_qTSFiles.end(), vTSFiles.cbegin(), vTSFiles.cend() );
        }
    }

    ChkDR( StartMediaStreamUsingChunkedTransfer( fEncrypted ) );
    ChkDR( SendMediaStreamUsingChunkedTransfer( fEncrypted ) );
    ChkDR( EndMediaStreamUsingChunkedTransfer() );
ErrorExit:
    SetEvent( m_hStreamingFinished );
    PRNDLogFunctionExitWithResult( dr );
    return dr;
    
}


std::vector<BYTE> CTSFileMediaStreamer::GetNextPacket()
{
    HRESULT         dr  = DRM_SUCCESS;
    vector<BYTE>    vbPacket;
    string          strFile;

    m_llPacketIndex++;
    if( m_pTSFile == nullptr || m_llPacketIndex >= m_llPacketCount )
    {
        DELETE_OBJECT( m_pTSFile );
        if( m_qTSFiles.size() > 0 )
        {
            strFile = m_qTSFiles.front();
            m_qTSFiles.pop_front();

            PRNDLogMessage( "Opening file %s", strFile.c_str() );
            ChkMem( m_pTSFile = new CTSFile( string_to_wstring ( strFile ) ) );
            ChkDR( m_pTSFile->GetPacketCount( &m_llPacketCount ) );
            m_llPacketIndex = 0;
        }
    }

    if( m_pTSFile )
    {
        m_pTSFile->GetPacket( m_llPacketIndex, vbPacket );
        
#if 0            
        if( m_llPacketIndex % 10 == 0 )
        {
            DuplicateTSPacket( vbPacket );
        }
#endif
        
    }

ErrorExit:    
    return vbPacket;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

HRESULT CPRNDMediaStreamer::CreatePRNDMediaStreamer( CHttpServer *pHttpServer, CPRNDMediaStreamer  **ppMediaStreamer )
{
    PRNDLogFunctionEntry();

    HRESULT                 dr              = DRM_SUCCESS;
    CPRNDMediaStreamer     *pMediaStreamer  = nullptr;

    *ppMediaStreamer = nullptr;

    if( pHttpServer->GetContentRequestType() == CONTENT_REQUEST_TYPE_FILE )
    {
        ChkMem( pMediaStreamer = new CTSFileMediaStreamer );
    }
    else
    {
        if( pHttpServer->GetContentRequestType() == CONTENT_REQUEST_TYPE_CHANNEL_HTTP_URL )
        {
            // channel URL is passed in the HTTP request
            ChkMem( pMediaStreamer = new CHttpMediaStreamer );
        }
        else
        {
            //channel URL is configured in the PRNDTransmitter.xml
            string strChannel = pHttpServer->GetContentSource();
            shared_ptr<CChannel> spChannel = CTransmitterConfig::Instance().GetChannel( strChannel );
            if( spChannel )
            {
                if( spChannel->GetChannelType() == CHANNEL_TYPE_HTTP )
                {
                    ChkMem( pMediaStreamer = new CHttpMediaStreamer );
                }
                else
                {
                    ChkMem( pMediaStreamer = new CTSFileMediaStreamer );
                }
            }
            else
            {
                PRNDLogMessage( "Channel not found!!!" );
                ChkDR( E_FAIL );
            }
        }
    }

    *ppMediaStreamer = pMediaStreamer;

ErrorExit:
    if( FAILED( dr ) )
    {
        DELETE_OBJECT( pMediaStreamer );
    }
    PRNDLogFunctionExitWithResult( dr );
    return dr;
}

