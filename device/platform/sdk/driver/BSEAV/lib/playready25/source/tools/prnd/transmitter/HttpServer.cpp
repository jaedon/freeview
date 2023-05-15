/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdafx.h>
#include <HttpServer.h>
#include <FileLogger.h>
#include <HelperFunctions.h>

#define  HTTP_REQUEST_MAX_SIZE          4096
#define  HTTP_BURST_WAIT_TIME           1000  /* milliseconds */
#define  CHANNEL_PREFIX                 "Channel="

using namespace std;

CHttpServer::CHttpServer( shared_ptr<CSocket> spSocket )
{
    m_spSocket = spSocket;
}

CHttpServer::~CHttpServer()
{
    PRNDLogFunctionEntry();
    
    PRNDLogFunctionExit();
}

HRESULT CHttpServer::ReadRequest()
{
    PRNDLogFunctionEntry();
    HRESULT dr                      = DRM_SUCCESS;
    bool    bRequestHeadersFound    = false;
    char   *pbRequest               = NULL;
    int     cbRequestSize           = 0;

    ChkMem( pbRequest = new char[ HTTP_REQUEST_MAX_SIZE ] );

    while( cbRequestSize < HTTP_REQUEST_MAX_SIZE )
    {
        char   *pbBuffer     = pbRequest + cbRequestSize;
        int     cbBufferSize = HTTP_REQUEST_MAX_SIZE - cbRequestSize;
        
        ChkDR( m_spSocket->RecvBurst( pbBuffer, &cbBufferSize ) );
        cbRequestSize += cbBufferSize;

        pbBuffer[ cbBufferSize ] = 0;
        char* pbEnd = strstr( pbBuffer, "\r\n\r\n" );
        if( NULL != pbEnd )
        {
            bRequestHeadersFound = true;
            break;
        }

        ChkDR( m_spSocket->WaitForOperation( CSocket::OP_RECV, HTTP_BURST_WAIT_TIME, NULL ) );
        
    }
    
    PRNDLogHexDump( ( BYTE* )pbRequest, ( DWORD )cbRequestSize, true );
    if( bRequestHeadersFound )
    {
        m_strRequestHeaders.assign( pbRequest, ( unsigned int )cbRequestSize );
    }
    
ErrorExit:
    DELETE_OBJECT_ARRAY( pbRequest );
    
    PRNDLogFunctionExitWithResult( dr );
    return dr;
}

HRESULT CHttpServer::ParseRequest()
{
    PRNDLogFunctionEntry();

    HRESULT         dr          = DRM_SUCCESS;
    vector<string>  allHeaders;
    int             cHeaders    = 0;
    int             cParts      = 0;
    vector<string>  initialHeader;
    string          str;

    cHeaders = string_split( m_strRequestHeaders, "\r\n", allHeaders );
    PRNDLogMessage( "Http Header count = %d ", cHeaders );
    if( cHeaders == 0 )
    {
        dr = DRM_E_FAIL;
        goto ErrorExit;
    }

    cParts = string_split( allHeaders[0], " ", initialHeader );
    PRNDLogMessage( "Initial Header Parts count = %d ", cParts );
    if( cParts != 3 )
    {
        dr = DRM_E_FAIL;
        goto ErrorExit;
    }

    if( !string_compare_case_insensitive( initialHeader[0], "GET" ) )
    {
        PRNDLogMessage( "Header %s is not supported", initialHeader[0].c_str() );
        dr = DRM_E_FAIL;
        goto ErrorExit;
        
    }

    m_strRelativeUrl = string_trim( initialHeader[1], " " );
    
    PRNDLogMessage( "RelativeUrl = %s", m_strRelativeUrl.c_str() );
    string_URLDecode( m_strRelativeUrl );
    PRNDLogMessage( "RelativeUrl decoded = %s", m_strRelativeUrl.c_str() );
    
    ParseRelativeUrl();

    if( !string_compare_case_insensitive( initialHeader[2], "HTTP/1.1" ) )
    {
        PRNDLogMessage( "Header %s is not supported", initialHeader[2].c_str() );
        dr = DRM_E_FAIL;
        goto ErrorExit;
        
    }


ErrorExit:
    PRNDLogFunctionExitWithResult( dr );
    return dr;
}

string CHttpServer::GetContentSource()
{
    return m_strContentSource;
}

string CHttpServer::GetPRNDSessionID()
{
    return m_strPRNDSessionID;
}

void CHttpServer::ParseRelativeUrl()
{
    vector<string> vstrUrlParts;
    
    //Examples: 
    //     /folder/file.ts?key=value&key2=value2
    // Improper format still supported for backwards compat with existing tests:
    //     /folder/file.ts&CONTENTPROTECTIONTYPE=PRND&PRNDSESSION=2844426F6C7927E3F578F3ACA3C7D8AD
    //     /StreamingChannel1&CONTENTPROTECTIONTYPE=PRND&PRNDSESSION=2844426F6C7927E3F578F3ACA3C7D8AD
    //     /Channel=http://bar-server/TSStreaming/1&CONTENTPROTECTIONTYPE=PRND&PRNDSESSION=2844426F6C7927E3F578F3ACA3C7D8AD
    
    //Replace query string delimiter '?' with '&' so we can easily get the key=value pairs
    //by just splitting the URL along '&' characters
    find_and_replace_substring( m_strRelativeUrl, "?", "&" );
    
    string_split( m_strRelativeUrl, "&", vstrUrlParts );
    
    string strRelativePath = string_trim( vstrUrlParts[0], " " );
    PRNDLogMessage( "RelativePath = %s", strRelativePath.c_str() );

    strRelativePath = string_trim( strRelativePath, "/" );
    if( string_ends_with( strRelativePath, ".ts" ) )
    {
        m_contentRequestType = CONTENT_REQUEST_TYPE_FILE;
        m_strContentSource   = strRelativePath;
    }
    else
    {
        string strChannelPrefix( CHANNEL_PREFIX );
        if( string_starts_with( strRelativePath, strChannelPrefix ) )
        {
            m_contentRequestType = CONTENT_REQUEST_TYPE_CHANNEL_HTTP_URL;
            m_strContentSource   = strRelativePath.substr( strChannelPrefix.length() );
        }
        else
        {
            m_contentRequestType = CONTENT_REQUEST_TYPE_CHANNEL_NAME;
            m_strContentSource   = strRelativePath;
        }
    }
    PRNDLogMessage( "Content source = %s", m_strContentSource.c_str() );
    
    for( DWORD i = 1; i < vstrUrlParts.size(); ++i )
    {
        vector<string> vstrNameValue;
        string_split( vstrUrlParts[i], "=", vstrNameValue );

        if( vstrNameValue.size() == 2 )
        {
            if( string_compare_case_insensitive( vstrNameValue[0], "PRNDSESSION" ) )
            {
                m_strPRNDSessionID = vstrNameValue[1];
                PRNDLogMessage( "SessionID = %s", m_strPRNDSessionID.c_str() );
            }

            if( string_compare_case_insensitive( vstrNameValue[0], "ArchiveFile" ) )
            {
                m_archiveFileName = vstrNameValue[1];
            }
        }
    }
}

std::string CHttpServer::GetArchiveFileKey() const
{
    return m_archiveFileName;
}


HRESULT CHttpServer::SendResponseHeaders( const string &strPlayReadyHeader )
{
    PRNDLogFunctionEntry();

    HRESULT     dr                          = DRM_SUCCESS;
    string      strResponseHeaders;

    strResponseHeaders  =  "HTTP/1.1 200 OK\r\n";
    strResponseHeaders  += "Content-Type: video/MP2T\r\n";
    strResponseHeaders  += "Transfer-Encoding: chunked\r\n";
    if( !strPlayReadyHeader.empty() )
    {
        strResponseHeaders  += "PRND-PRO.microsoft.com: ";
        strResponseHeaders  += strPlayReadyHeader;
        strResponseHeaders  += "\r\n";
    }
    strResponseHeaders  += "\r\n";
    
    PRNDLogHexDump( ( BYTE* )strResponseHeaders.c_str(), ( DWORD ) strResponseHeaders.length(), true );
    ChkDR( m_spSocket->Send( ( char* )strResponseHeaders.c_str(), ( int )strResponseHeaders.length() ) );

ErrorExit:
    PRNDLogFunctionExitWithResult( dr );
    return dr;
}

HRESULT CHttpServer::SendResponseHeaders( LONGLONG llContentLength )
{
    PRNDLogFunctionEntry();

    HRESULT     dr                          = DRM_SUCCESS;
    string      strResponseHeaders;

    strResponseHeaders  =  "HTTP/1.1 200 OK \r\n";
    strResponseHeaders  += "Content-Type: video/MP2T \r\n";
    strResponseHeaders  += "Content-Length: " + std::to_string( llContentLength ) + " \r\n";
    strResponseHeaders  += "\r\n";
    
    PRNDLogHexDump( ( BYTE* )strResponseHeaders.c_str(), ( DWORD ) strResponseHeaders.length(), true );
    ChkDR( m_spSocket->Send( ( char* )strResponseHeaders.c_str(), ( int )strResponseHeaders.length() ) );

ErrorExit:
    PRNDLogFunctionExitWithResult( dr );
    return dr;
}

HRESULT CHttpServer::SendResponseChunk( vector<BYTE>& vbData  )
{
    
    HRESULT         dr              = DRM_SUCCESS;
    string          strChunkSize;
    string          strChunkEnd     = "\r\n";
    
    ChkArg( vbData.size() > 0 );
    strChunkSize  =  integer_to_hexstring( vbData.size() ) + "\r\n";
    ChkDR( m_spSocket->Send( ( char* )strChunkSize.c_str(), ( int )strChunkSize.length() ) );
    
    ChkDR( m_spSocket->Send( ( char* )&vbData[0], ( int )vbData.size() ) );
    ChkDR( m_spSocket->Send( ( char* )strChunkEnd.c_str(), ( int )strChunkEnd.length() ) );

ErrorExit:
    return dr;
}

HRESULT CHttpServer::SendResponseChunkEnd()
{
    PRNDLogFunctionEntry();
    
    HRESULT     dr = DRM_SUCCESS;
    string      strChunkSize;

    strChunkSize  =  "0\r\n\r\n";
    ChkDR( m_spSocket->Send( ( char* )strChunkSize.c_str(), ( int )strChunkSize.length() ) );

ErrorExit:
    PRNDLogFunctionExitWithResult( dr );
    return dr;
}

HRESULT CHttpServer::SendResponseFileNotFound()
{
    PRNDLogFunctionEntry();

    HRESULT     dr = DRM_SUCCESS;
    string      strResponseHeaders;

    strResponseHeaders  =   "HTTP/1.1 404 Not Found\r\n";
    strResponseHeaders  +=  "Connection: close\r\n";
    strResponseHeaders  +=  "\r\n";
    
    PRNDLogHexDump( ( BYTE* )strResponseHeaders.c_str(), ( DWORD ) strResponseHeaders.length(), true );
    ChkDR( m_spSocket->Send( ( char* )strResponseHeaders.c_str(), ( int )strResponseHeaders.length() ) );

ErrorExit:
    PRNDLogFunctionExitWithResult( dr );
    return dr;
}

HRESULT CHttpServer::SendResponseChunkString( const std::string str )
{
    PRNDLogFunctionEntry();

    HRESULT dr           = DRM_SUCCESS;
    string  strChunkSize;
    string  strChunkEnd  = "\r\n";

    strChunkSize  =  integer_to_hexstring( str.size() ) + "\r\n";
    ChkDR( m_spSocket->Send( ( char* )strChunkSize.c_str(), ( int )strChunkSize.length() ) );
    
    PRNDLogHexDump( ( BYTE* )str.c_str(), ( DWORD ) str.length(), true );
    ChkDR( m_spSocket->Send( ( char* )str.c_str(), ( int )str.length() ) );
    ChkDR( m_spSocket->Send( ( char* )strChunkEnd.c_str(), ( int )strChunkEnd.length() ) );

ErrorExit:
    PRNDLogFunctionExitWithResult( dr );
    return dr;
}
