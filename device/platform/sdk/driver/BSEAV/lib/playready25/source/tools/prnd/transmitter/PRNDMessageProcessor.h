/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#pragma once
#include <PRNDMemHolder.h>

class CPRNDMessageProcessor
{
public:
    CPRNDMessageProcessor();
    virtual ~CPRNDMessageProcessor();

    void    GetSessionID( __out      DRM_ID &sessionID );
    void    SetSessionID( __in const DRM_ID &sessionID );

    HRESULT virtual ProcessMessage(
                            __in    DRM_APP_CONTEXT *poAppContext,
                            __inout CPRNDMemHolder  &message,
                            __inout CPRNDMemHolder  &response                            );

protected:

    void AdoptMessage( __inout CPRNDMemHolder& message );
    void ExtractResponse( __inout CPRNDMemHolder& response );

    HRESULT CPRNDMessageProcessor::IsPRNDSessionIDValid();
    HRESULT CPRNDMessageProcessor::SetRequestCustomData(
                __in                            const DRM_ID *pdrmidCustomDataTypeID,
                __in_bcount( cbCustomDataSize )       BYTE   *pbCustomData,
                __in                                  DWORD   cbCustomDataSize          );
    HRESULT CPRNDMessageProcessor::CopyRequestCustomDataToResponseCustomData();

    DRM_BYTE   *m_pbMessageRequest;
    DWORD       m_cbMessageRequestSize;

    DRM_BYTE   *m_pbMessageResponse;
    DWORD       m_cbMessageResponseSize;

    DRM_ID      m_drmidSessionID;

    DRM_ID      m_drmidRequestCustomDataTypeID;
    DRM_BYTE   *m_pbRequestCustomData;
    DWORD       m_cbRequestCustomDataSize;

    DRM_ID      m_drmidResponseCustomDataTypeID;
    DRM_BYTE   *m_pbResponseCustomData;
    DWORD       m_cbResponseCustomDataSize;

    bool        m_bValidateSessionID;
};



