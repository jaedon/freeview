/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#pragma once
#include <PRNDMessageProcessor.h>
#include <LicenseGenerator.h>

class CPRNDLicenseRequestProcessor : public CPRNDMessageProcessor
{
public:
    CPRNDLicenseRequestProcessor::CPRNDLicenseRequestProcessor();
    virtual CPRNDLicenseRequestProcessor::~CPRNDLicenseRequestProcessor();

    HRESULT virtual ProcessMessage(
                            __in    DRM_APP_CONTEXT *poAppContext,
                            __inout CPRNDMemHolder  &message,
                            __inout CPRNDMemHolder  &response
                            );

    HRESULT CPRNDLicenseRequestProcessor::SetKeyId(  DRM_GUID  *pKeyId );
    std::shared_ptr<CLicenseGenerator> CPRNDLicenseRequestProcessor::GetLicenseGenerator();

protected:
    HRESULT CPRNDLicenseRequestProcessor::PrepareErrorMessage(
                                                DRM_APP_CONTEXT *poAppContext,
                                                HRESULT          drResult );

    static HRESULT  DRM_CALL CPRNDLicenseRequestProcessor::Drm_Prnd_Data_Callback(
    __inout_ecount_opt( 1 )                    DRM_VOID                                        *pvDataCallbackContext,
    __in_opt                                   DRM_PRND_CERTIFICATE_DATA                       *pCertificateData,
    __in_opt                             const DRM_ID                                          *f_pCustomDataTypeID,
    __in_bcount_opt( f_cbCustomData )    const DRM_BYTE                                        *pbCustomData,
    __in                                       DRM_DWORD                                        cbCustomData );

    static DRM_RESULT DRM_CALL CPRNDLicenseRequestProcessor::Drm_Prnd_Content_Identifier_Callback(
    __inout_ecount_opt( 1 )                    DRM_VOID                                        *pvContentIdentifierCallbackContext,
    __in                                       DRM_PRND_CONTENT_IDENTIFIER_TYPE                 f_eContentIdentifierType,
    __in_bcount( f_cbContentIdentifier ) const DRM_BYTE                                        *pbContentIdentifier,
    __in                                       DRM_DWORD                                        cbContentIdentifier,
    __out_ecount( 1 )                          DRM_KID                                         *pkidContent );

    HRESULT CPRNDLicenseRequestProcessor::ProcessRequestCustomData();

    HRESULT CPRNDLicenseRequestProcessor::ParseTransmitterAction( std::string &strXml, DRM_SUBSTRING *pdasstrXML );

private:

    bool                                m_bNoLicenseResponse;
    std::shared_ptr<CLicenseGenerator>  m_spLicenseGenerator;

};


