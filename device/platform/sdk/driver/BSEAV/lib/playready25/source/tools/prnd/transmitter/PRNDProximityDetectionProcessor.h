/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#pragma once
#include <PRNDMessageProcessor.h>
#include <HelperFunctions.h>

class CProximityDetectionConfig
{
public:
    CProximityDetectionConfig()
    {
        m_dwProximityFailureStartTime       = 0;
        m_dwProximityFailureDuration        = 0;
        m_bNoPDStartResponse                = false;
        m_bNoPDResponseResponse             = false;
    }

    void CProximityDetectionConfig::SetProximityDetectionFailureStartTime()
    {
        if( m_dwProximityFailureStartTime == 0 )
        {
            m_dwProximityFailureStartTime = GetCurrentTimeInSeconds();
        }
    }

    bool CProximityDetectionConfig::ShouldProximityDetectionSucceed()
    {
        bool bSucceed = false;
        if( ( m_dwProximityFailureStartTime + m_dwProximityFailureDuration ) < GetCurrentTimeInSeconds() )
        {
            bSucceed = true;
        }

        return bSucceed;
    }

    DWORD   m_dwProximityFailureStartTime;
    DWORD   m_dwProximityFailureDuration;

    bool    m_bNoPDStartResponse;
    bool    m_bNoPDResponseResponse;
};

////////////////////////////////////////////////////////////////////////////////////////////////
class CPRNDProximityDetectionProcessor : public CPRNDMessageProcessor
{
public:
    CPRNDProximityDetectionProcessor();
    virtual ~CPRNDProximityDetectionProcessor();

    void CPRNDProximityDetectionProcessor::SetProximityDetectionConfig( std::shared_ptr<CProximityDetectionConfig> spPDConfig );

protected:

    std::shared_ptr< CProximityDetectionConfig > m_spProximityDetectionConfig;

};

//////////////////////////////////////////////////////////////////////////////////////////////
class CPRNDProximityDetectionStartProcessor : public CPRNDProximityDetectionProcessor
{
public:
    CPRNDProximityDetectionStartProcessor();
    virtual ~CPRNDProximityDetectionStartProcessor();

    HRESULT virtual ProcessMessage(
                            __in DRM_APP_CONTEXT   *poAppContext,
                            __inout CPRNDMemHolder &message,
                            __inout CPRNDMemHolder &response       );

private:

    HRESULT CPRNDProximityDetectionStartProcessor::PrepareErrorMessage( DRM_APP_CONTEXT *poAppContext, HRESULT  drResult );

};


/////////////////////////////////////////////////////////////////////////////////////////////////
class CPRNDProximityDetectionResponseProcessor : public CPRNDProximityDetectionProcessor
{
public:
    CPRNDProximityDetectionResponseProcessor();
    virtual ~CPRNDProximityDetectionResponseProcessor();

    HRESULT virtual ProcessMessage(
                            __in    DRM_APP_CONTEXT *poAppContext,
                            __inout CPRNDMemHolder  &message,
                            __inout CPRNDMemHolder  &response      );

private:

    HRESULT CPRNDProximityDetectionResponseProcessor::PrepareErrorMessage( DRM_APP_CONTEXT *poAppContext, HRESULT  drResult );

};


