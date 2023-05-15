/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#pragma once
#include <Socket.h>

enum CONTENT_REQUEST_TYPE
{
    CONTENT_REQUEST_TYPE_FILE = 0,
    CONTENT_REQUEST_TYPE_CHANNEL_NAME,
    CONTENT_REQUEST_TYPE_CHANNEL_HTTP_URL
};

class CHttpServer
{
public:

    CHttpServer( std::shared_ptr<CSocket> spSocket );
    virtual ~CHttpServer();
    
    HRESULT CHttpServer::ReadRequest();
    HRESULT CHttpServer::ParseRequest();
    void CHttpServer::ParseRelativeUrl();
    
    HRESULT CHttpServer::SendResponseHeaders( LONGLONG llContentLength );
    HRESULT CHttpServer::SendResponseHeaders( const std::string &strPlayReadyHeader );
        
    HRESULT CHttpServer::SendResponseChunk( std::vector<BYTE> &vbData  );
    HRESULT CHttpServer::SendResponseChunkEnd();
    HRESULT CHttpServer::SendResponseFileNotFound();
    HRESULT CHttpServer::SendResponseChunkString( const std::string str );

    CONTENT_REQUEST_TYPE CHttpServer::GetContentRequestType() { return m_contentRequestType; }
    std::string CHttpServer::GetContentSource();
    std::string CHttpServer::GetPRNDSessionID();

    std::string CHttpServer::GetArchiveFileKey() const;
    
private:
    std::string m_strRequestHeaders;
    std::string m_strRelativeUrl;
    std::string m_strContentSource;
    std::string m_strPRNDSessionID;
    std::string m_archiveFileName;

    std::shared_ptr<CSocket> m_spSocket;
    CONTENT_REQUEST_TYPE     m_contentRequestType;
    
};
