/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdafx.h>
#include <PRNDMessageProcessor.h>
#include <FileLogger.h>

CPRNDMessageProcessor::CPRNDMessageProcessor()
{
    m_pbMessageRequest      = NULL;
    m_cbMessageRequestSize  = 0;

    m_pbMessageResponse     = NULL;
    m_cbMessageResponseSize = 0;

    memset( &m_drmidSessionID, 0, sizeof( DRM_ID ) );

    memset( &m_drmidRequestCustomDataTypeID, 0, sizeof( DRM_ID ) );
    m_pbRequestCustomData       = NULL;
    m_cbRequestCustomDataSize   = 0;

    memset( &m_drmidResponseCustomDataTypeID, 0, sizeof( DRM_ID ) );
    m_pbResponseCustomData       = NULL;
    m_cbResponseCustomDataSize   = 0;

    m_bValidateSessionID = true;

}

CPRNDMessageProcessor::~CPRNDMessageProcessor()
{
    DRM_PRND_SAFEMEMFREE( m_pbMessageRequest );
    DRM_PRND_SAFEMEMFREE( m_pbMessageResponse );
    DRM_PRND_SAFEMEMFREE( m_pbRequestCustomData );
    DRM_PRND_SAFEMEMFREE( m_pbResponseCustomData );
}

HRESULT CPRNDMessageProcessor::IsPRNDSessionIDValid()
{
    HRESULT dr                                  = DRM_SUCCESS;
    BYTE    rgbTemp[ sizeof( m_drmidSessionID ) ] = {0};

    if( memcmp( &m_drmidSessionID, rgbTemp, sizeof( m_drmidSessionID ) ) == 0 )
    {
        dr = DRM_E_FAIL;
    }

    return dr;
}

void CPRNDMessageProcessor::GetSessionID( __out DRM_ID &sessionID )
{
    sessionID = m_drmidSessionID;
}

void CPRNDMessageProcessor::SetSessionID( __in const DRM_ID &sessionID )
{
    m_drmidSessionID = sessionID;
}

HRESULT CPRNDMessageProcessor::SetRequestCustomData(
                __in                            const DRM_ID *pdrmidCustomDataTypeID,
                __in_bcount( cbCustomDataSize )       BYTE   *pbCustomData,
                __in                                  DWORD   cbCustomDataSize )
{
    PRNDLogFunctionEntry();
    HRESULT dr = DRM_SUCCESS;

    ChkArg( pdrmidCustomDataTypeID != NULL );
    ChkArg( pbCustomData != NULL );
    ChkArg( cbCustomDataSize != 0 );

    ChkArg( m_pbRequestCustomData       == NULL );
    ChkArg( m_cbRequestCustomDataSize   == 0 );

    memcpy_s( &m_drmidRequestCustomDataTypeID, sizeof( DRM_ID ), pdrmidCustomDataTypeID, sizeof( DRM_ID ) );

    ChkMem( m_pbRequestCustomData = Drm_Prnd_MemAlloc( cbCustomDataSize ) );
    memcpy_s( m_pbRequestCustomData, cbCustomDataSize, pbCustomData, cbCustomDataSize );

    m_cbRequestCustomDataSize = cbCustomDataSize;

ErrorExit:
    PRNDLogFunctionExitWithResult( dr );
    return dr;
}

HRESULT CPRNDMessageProcessor::CopyRequestCustomDataToResponseCustomData()
{
    PRNDLogFunctionEntry();
    HRESULT dr = DRM_SUCCESS;

    if( m_pbRequestCustomData == NULL )
    {
        PRNDLogMessage( "There is no request custom data to copy to response !!!!!" );
        goto ErrorExit;
    }

    ChkArg( m_pbResponseCustomData       == NULL );
    ChkArg( m_cbResponseCustomDataSize   == 0 );

    memcpy_s( &m_drmidResponseCustomDataTypeID, sizeof( DRM_ID ), &m_drmidRequestCustomDataTypeID, sizeof( DRM_ID ) );

    ChkMem( m_pbResponseCustomData = Drm_Prnd_MemAlloc( m_cbRequestCustomDataSize ) );
    memcpy_s( m_pbResponseCustomData, m_cbRequestCustomDataSize, m_pbRequestCustomData, m_cbRequestCustomDataSize );

    m_cbResponseCustomDataSize = m_cbRequestCustomDataSize;

ErrorExit:
    PRNDLogFunctionExitWithResult( dr );
    return dr;
}


void CPRNDMessageProcessor::AdoptMessage( __inout CPRNDMemHolder& message )
{
    DRMASSERT( m_pbMessageRequest == NULL );
    DRMASSERT( m_cbMessageRequestSize == 0 );

    m_cbMessageRequestSize = message.Size();
    m_pbMessageRequest = message.Detach();
}

void CPRNDMessageProcessor::ExtractResponse( __inout CPRNDMemHolder& response )
{
    DRMASSERT( m_cbMessageResponseSize != 0 );
    DRMASSERT( m_pbMessageResponse != NULL );

    response.Attach( m_pbMessageResponse, m_cbMessageResponseSize );

    m_pbMessageResponse = NULL;
    m_cbMessageRequestSize = 0;
}

HRESULT CPRNDMessageProcessor::ProcessMessage(
                                        __in DRM_APP_CONTEXT   *poAppContext,
                                        __inout CPRNDMemHolder &message,
                                        __inout CPRNDMemHolder &response         )
{
    return DRM_E_NOTIMPL;
}


