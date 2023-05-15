/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#pragma once
#include <Socket.h>
#include <Locks.h>

class CHttpClient
{
public:

    CHttpClient( std::string strHttpPath );
    virtual ~CHttpClient();
    
    std::vector<BYTE> CHttpClient::GetPacket();
    HRESULT CHttpClient::ReadHttpStreamAsync();
    bool CHttpClient::ShutdownInitiated();

    HRESULT CHttpClient::ReadHttpStream();    
    std::string CHttpClient::GetResponseHeader();
    HRESULT CHttpClient::ReadResponseBodyAsync();

private:
    static DWORD WINAPI CHttpClient::ReadHttpStreamCallback( __in LPVOID lpParam );
    static DWORD WINAPI CHttpClient::ReadResponseBodyCallback( __in LPVOID lpParam );
    HRESULT CHttpClient::ParseHostNameAndPort();
    
    HRESULT CHttpClient::MakeRequest();
    HRESULT CHttpClient::ReadResponse();
    HRESULT CHttpClient::ReadResponseBody();

    void CHttpClient::AddPacket( std::vector<BYTE> &vbPacket );

    void CHttpClient::InitiateShutdown();
        
    CSocket     *m_pSocket;

    std::string                     m_strResponseHeaders;
    std::deque<std::vector<BYTE>>   m_qPackets;
    
    CDRMLock    m_lock;
    HANDLE      m_hWorkerThread;
    bool        m_bShutdown;

    std::string m_strHttpPath;
    std::string m_strHostName;
    std::string m_strPort;

    DWORD   m_dwLastTimeDroppedPacketLogged;
    DWORD   m_dwLastTimeCacheEmptyLogged;
};
