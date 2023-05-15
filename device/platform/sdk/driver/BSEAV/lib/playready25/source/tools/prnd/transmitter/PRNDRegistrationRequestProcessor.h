/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#pragma once
#include <PRNDMessageProcessor.h>
#include <PRNDProximityDetectionProcessor.h>

class CPRNDRegistrationRequestProcessor : public CPRNDMessageProcessor
{
public:
    CPRNDRegistrationRequestProcessor();
    virtual ~CPRNDRegistrationRequestProcessor();

    HRESULT virtual ProcessMessage(
                            __in    DRM_APP_CONTEXT *poAppContext,
                            __inout CPRNDMemHolder  &message,
                            __inout CPRNDMemHolder  &response
                            );

    std::shared_ptr<CProximityDetectionConfig> CPRNDRegistrationRequestProcessor::GetProximityDetectionConfig();

protected:
    HRESULT CPRNDRegistrationRequestProcessor::PrepareErrorMessage(
                                                DRM_APP_CONTEXT *poAppContext,
                                                HRESULT          drResult );

    static HRESULT  DRM_CALL CPRNDRegistrationRequestProcessor::Drm_Prnd_Data_Callback(
        __inout_ecount_opt( 1 )                    DRM_VOID                                        *pvDataCallbackContext,
        __in_opt                                   DRM_PRND_CERTIFICATE_DATA                       *pCertificateData,
        __in_opt                             const DRM_ID                                          *pdrmidCustomDataTypeID,
        __in_bcount_opt( f_cbCustomData )    const DRM_BYTE                                        *pbCustomData,
        __in                                       DRM_DWORD                                        cbCustomData );


private:

    HRESULT CPRNDRegistrationRequestProcessor::ParseRegistration( std::string &strXml, DRM_SUBSTRING *pdasstrXML );
    HRESULT CPRNDRegistrationRequestProcessor::ParseTransmitterAction( std::string &strXml, DRM_SUBSTRING *pdasstrXML );
    HRESULT CPRNDRegistrationRequestProcessor::ParseRequestCustomData();
    HRESULT CPRNDRegistrationRequestProcessor::ProcessRequestCustomData();


    bool    m_bFailRegistration;
    bool    m_bNoRegistrationResponse;

    std::shared_ptr< CProximityDetectionConfig > m_spProximityDetectionConfig;
};


