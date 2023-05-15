/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdafx.h>
#include <PRNDProximityDetectionProcessor.h>
#include <FileLogger.h>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////

CPRNDProximityDetectionProcessor::CPRNDProximityDetectionProcessor()
{

}

CPRNDProximityDetectionProcessor::~CPRNDProximityDetectionProcessor()
{

}

void CPRNDProximityDetectionProcessor::SetProximityDetectionConfig( std::shared_ptr<CProximityDetectionConfig> spPDConfig )
{
    m_spProximityDetectionConfig = spPDConfig;
    m_spProximityDetectionConfig->SetProximityDetectionFailureStartTime();
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////

CPRNDProximityDetectionStartProcessor::CPRNDProximityDetectionStartProcessor()
{

}

CPRNDProximityDetectionStartProcessor::~CPRNDProximityDetectionStartProcessor()
{

}

HRESULT CPRNDProximityDetectionStartProcessor::PrepareErrorMessage( DRM_APP_CONTEXT *poAppContext, HRESULT  drResult )
{
    PRNDLogFunctionEntry();

    HRESULT     dr   = DRM_SUCCESS;

    ChkArg( FAILED( drResult ) );
    ChkArg( m_pbMessageResponse == NULL );
    ChkArg( m_cbMessageResponseSize == 0 );

    ChkDR( Drm_Prnd_Transmitter_ProximityDetectionResult_Generate(  poAppContext,
                                                                    NULL,
                                                                    drResult,
                                                                    DRM_PRND_FLAG_NO_FLAGS,
                                                                   &m_pbMessageResponse,
                                                                   &m_cbMessageResponseSize ) );

ErrorExit:
    PRNDLogFunctionExitWithResult( dr );
    return dr;
}

HRESULT CPRNDProximityDetectionStartProcessor::ProcessMessage(
                                        __in    DRM_APP_CONTEXT *poAppContext,
                                        __inout CPRNDMemHolder  &message,
                                        __inout CPRNDMemHolder  &response   )
{
    PRNDLogFunctionEntry();
    HRESULT   dr         = DRM_SUCCESS;
    DRM_DWORD dwFlagsOut = 0;

    DRMASSERT( m_pbMessageRequest == NULL );
    DRMASSERT( m_cbMessageRequestSize == 0 );

    AdoptMessage( message );

    DRMASSERT( m_pbMessageRequest != NULL );
    DRMASSERT( m_cbMessageRequestSize != 0 );

    DRMASSERT( m_pbMessageResponse == NULL );
    DRMASSERT( m_cbMessageResponseSize == 0 );

    PRNDLogMessage( "Generating response from PK.." );
    dr = Drm_Prnd_Transmitter_ProximityDetectionStart_Process(
        poAppContext,
        m_pbMessageRequest,
        m_cbMessageRequestSize,
        DRM_PRND_FLAG_NO_FLAGS,
        &m_pbMessageResponse,
        &m_cbMessageResponseSize,
        &dwFlagsOut );

    if( FAILED( dr ) )
    {
        ChkDR( PrepareErrorMessage( poAppContext, dr ) );
    }

    if( m_spProximityDetectionConfig &&
        !m_spProximityDetectionConfig->m_bNoPDStartResponse )
    {
        ExtractResponse( response );
    }

ErrorExit:
    PRNDLogFunctionExitWithResult( dr );
    return dr;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
CPRNDProximityDetectionResponseProcessor::CPRNDProximityDetectionResponseProcessor()
{

}

CPRNDProximityDetectionResponseProcessor::~CPRNDProximityDetectionResponseProcessor()
{

}

HRESULT CPRNDProximityDetectionResponseProcessor::PrepareErrorMessage( DRM_APP_CONTEXT *poAppContext, HRESULT  drResult )
{
    PRNDLogFunctionEntry();

    HRESULT     dr   = DRM_SUCCESS;

    ChkArg( FAILED( drResult ) );
    ChkArg( m_pbMessageResponse == NULL );
    ChkArg( m_cbMessageResponseSize == 0 );

    ChkDR( Drm_Prnd_Transmitter_ProximityDetectionResult_Generate(  poAppContext,
                                                                    NULL,
                                                                    drResult,
                                                                    DRM_PRND_FLAG_NO_FLAGS,
                                                                   &m_pbMessageResponse,
                                                                   &m_cbMessageResponseSize ) );

ErrorExit:
    PRNDLogFunctionExitWithResult( dr );
    return dr;
}

HRESULT CPRNDProximityDetectionResponseProcessor::ProcessMessage(
                                        __in    DRM_APP_CONTEXT *poAppContext,
                                        __inout CPRNDMemHolder  &message,
                                        __inout CPRNDMemHolder  &response     )
{
    PRNDLogFunctionEntry();
    HRESULT   dr              = DRM_SUCCESS;
    bool      bTimeOutNeeded  = false;
    DRM_DWORD dwFlagsOut      = 0;

    DRMASSERT( m_pbMessageRequest == NULL );
    DRMASSERT( m_cbMessageRequestSize == 0 );

    AdoptMessage( message );

    DRMASSERT( m_pbMessageRequest != NULL );
    DRMASSERT( m_cbMessageRequestSize != 0 );

    DRMASSERT( m_pbMessageResponse == NULL );
    DRMASSERT( m_cbMessageResponseSize == 0 );

    if( m_spProximityDetectionConfig &&
        !m_spProximityDetectionConfig->ShouldProximityDetectionSucceed() )
    {
        bTimeOutNeeded = true;
    }

    if( bTimeOutNeeded )
    {
        PRNDLogMessage( "Waiting for 1 sec to force proximity detection failure.." );
        Sleep( 1000 );
    }

    PRNDLogMessage( "Generating response from PK.." );
    dr = Drm_Prnd_Transmitter_ProximityDetectionResponse_Process(   poAppContext,
                                                                    m_pbMessageRequest,
                                                                    m_cbMessageRequestSize,
                                                                    DRM_PRND_FLAG_NO_FLAGS,
                                                                   &m_pbMessageResponse,
                                                                   &m_cbMessageResponseSize,
                                                                   &dwFlagsOut );
    if( FAILED( dr ) )
    {
        ChkDR( PrepareErrorMessage( poAppContext, dr ) );
    }

    if( m_spProximityDetectionConfig &&
        !m_spProximityDetectionConfig->m_bNoPDResponseResponse )
    {
        ExtractResponse( response );
    }

ErrorExit:
    PRNDLogFunctionExitWithResult( dr );
    return dr;
}


