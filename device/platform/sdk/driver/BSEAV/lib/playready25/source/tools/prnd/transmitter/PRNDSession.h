/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/


#pragma once
#include <PRNDMessageProcessor.h>
#include <PRNDRegistrationRequestProcessor.h>
#include <PRNDMediaStreamer.h>


class CPRNDSession
{
public:

    static HRESULT CPRNDSession::CreateInstance( __out CPRNDSession **ppSession );

    static HRESULT CPRNDSession::IsPRNDSessionIDValid( __in const DRM_ID &sessionID );

    static void CPRNDSession::LogSessionID( __in const DRM_ID &sessionID );

    static HRESULT CPRNDSession::TranslateErrorToResponse(
                                        __in        HRESULT                 drIn,
                                        __in_opt    DRM_APP_CONTEXT        *poAppContext,
                                        __in        DRM_ID                 &sessionID,
                                        __in        DRM_PRND_MESSAGE_TYPE   eMessageType,
                                        __inout     CPRNDMemHolder         &response       );

    virtual ~CPRNDSession();

    HRESULT GetSessionID( __out DRM_ID& sessionID );

    HRESULT CPRNDSession::ProcessMessage(
                                        __in    DRM_PRND_MESSAGE_TYPE   eMessageType,
                                        __inout CPRNDMemHolder         &message,
                                        __inout CPRNDMemHolder         &response           );

    HRESULT CPRNDSession::ProcessEncryptedMediaStreamingRequest( __in CHttpServer *pHttpServer );


private:

    CPRNDSession();

    HRESULT Initialize();

    HRESULT UpdateRevocationPackage();


    HRESULT CPRNDSession::GetMessageProcessor(
                                        __in  DRM_PRND_MESSAGE_TYPE     eMessageType,
                                        __out CPRNDMessageProcessor   **ppMessageProcessor    );

    HRESULT CPRNDSession::StartEncryptedMediaStreaming( __in CHttpServer *pHttpServer );

    void Shutdown();

private:

    DRM_APP_CONTEXT                *m_poAppContext;
    DRM_BYTE                       *m_pbOpaqueBuffer;
    DRM_BYTE                       *m_pbRevocationBuffer;
    DRM_PRND_TRANSMITTER_CONTEXT    m_oTransmitterContext;
    DRM_ID                          m_sessionID;

    CPRNDMediaStreamer                 *m_pMediaStreamer;
    std::shared_ptr<CLicenseGenerator>  m_spLicenseGenerator;

    std::shared_ptr< CProximityDetectionConfig > m_spProximityDetectionConfig;

    CDRMLock    m_lock;
};
