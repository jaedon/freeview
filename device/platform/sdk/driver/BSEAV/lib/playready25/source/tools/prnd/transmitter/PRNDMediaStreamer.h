/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#pragma once

#include <HttpServer.h>
#include <TSFile.h>
#include <LicenseGenerator.h>
#include <HttpClient.h>

class CPRNDMediaStreamer
{
public:

    CPRNDMediaStreamer();
    virtual ~CPRNDMediaStreamer();
    static void DRM_CALL CPRNDMediaStreamer::Encryption_Log_Callback( __inout DRM_VOID *pCallbackContext, __in const DRM_M2TS_LOG *f_poLog );
    static HRESULT CPRNDMediaStreamer::CreatePRNDMediaStreamer( CHttpServer *pHttpServer, CPRNDMediaStreamer  **ppMediaStreamer );
    HRESULT CPRNDMediaStreamer::LogEncryptionTrace( const DRM_M2TS_LOG *poLog );    

    virtual HRESULT CPRNDMediaStreamer::ProcessMediaStreamingRequest( 
                                        CHttpServer                        *pHttpServer, 
                                        bool                                fEncrypted,
                                        std::shared_ptr<CLicenseGenerator>  spLicenseGenerator ) = 0;
    HRESULT CPRNDMediaStreamer::Shutdown();
    
protected:    
    HRESULT CPRNDMediaStreamer::InitializeEncryption( std::shared_ptr<CLicenseGenerator>  spLicenseGenerator );
    HRESULT CPRNDMediaStreamer::ProcessMediaStreamingRequest( CHttpServer *pHttpServer, bool fEncrypted );
    virtual std::vector<BYTE> CPRNDMediaStreamer::GetNextPacket() = 0;
    
    HRESULT CPRNDMediaStreamer::StartMediaStreamUsingChunkedTransfer( bool fEncrypted );
    HRESULT CPRNDMediaStreamer::SendMediaStreamUsingChunkedTransfer( bool fEncrypted );
    HRESULT CPRNDMediaStreamer::EndMediaStreamUsingChunkedTransfer();

private:    
    HRESULT CPRNDMediaStreamer::SetEncryptionKeyData();
    HRESULT CPRNDMediaStreamer::Encrypt( std::vector<BYTE> &vbDataIn, std::vector<BYTE> &vbDataOut, bool bLastPacket );
    HRESULT CPRNDMediaStreamer::GeneratePlayReadyHeader( std::string &strHexPlayReadyHeader );
    HRESULT CPRNDMediaStreamer::SendChunk( std::vector<BYTE> &vb );
    std::vector<BYTE> CPRNDMediaStreamer::GetPacket();
    HRESULT CPRNDMediaStreamer::GetArchiveFileNameForClearContent( std::string &strFileName );
    HRESULT CPRNDMediaStreamer::GetArchiveFileNameForEncryptedContent( std::string &strFileName );
    
protected:
    CHttpServer    *m_pHttpServer;
    bool            m_bLiveStreaming;
    BOOL            m_bShutdownStreaming;
    HANDLE          m_hStreamingFinished;
    
private:    
    DRM_M2TS_ENCRYPTOR_HANDLE   m_hEncryptor;
    DRM_GUID                    m_KeyId;
    
    std::shared_ptr<CLicenseGenerator>  m_spLicenseGenerator;
    std::shared_ptr<CTSFileWriter>      m_spTSFileWriter;
    std::shared_ptr<CTSFileWriter>      m_spClearTSFileWriter;

    FILE   *m_fpEncryptionLog;
    DWORD   m_dwLastTimeFuzzed;

    CDRMLock    m_lock;
    

};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class CHttpMediaStreamer : public CPRNDMediaStreamer
{
public:
    CHttpMediaStreamer();
    virtual ~CHttpMediaStreamer();

    virtual HRESULT CHttpMediaStreamer::ProcessMediaStreamingRequest( 
                                        CHttpServer                        *pHttpServer, 
                                        bool                                fEncrypted,
                                        std::shared_ptr<CLicenseGenerator>  spLicenseGenerator );
    
protected:    
    virtual std::vector<BYTE> CHttpMediaStreamer::GetNextPacket();

private:
    std::shared_ptr<CHttpClient>        m_spHttpClient;
    
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class CTSFileMediaStreamer : public CPRNDMediaStreamer
{
public:
    CTSFileMediaStreamer();
    virtual ~CTSFileMediaStreamer();

    virtual HRESULT CTSFileMediaStreamer::ProcessMediaStreamingRequest( 
                                        CHttpServer                        *pHttpServer, 
                                        bool                                fEncrypted,
                                        std::shared_ptr<CLicenseGenerator>  spLicenseGenerator );
    
protected:    
    virtual std::vector<BYTE> CTSFileMediaStreamer::GetNextPacket();

private:
    CTSFile                  *m_pTSFile;
    std::deque<std::string>   m_qTSFiles;

    LONGLONG m_llPacketIndex;
    LONGLONG m_llPacketCount;
    
};
