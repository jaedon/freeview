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

class CLicenseDescriptor
{
public:
    CLicenseDescriptor::CLicenseDescriptor()
    {
        memset( &m_descriptor, 0, sizeof( DRM_LOCAL_LICENSE_POLICY_DESCRIPTOR ) );
        m_licenseType = eDRM_LOCAL_LICENSE_REMOTE_BOUND_SIMPLE;
        
        memset( &m_KeyId, 0, sizeof( DRM_KID ) );
        memset( &m_RootKeyId, 0, sizeof( DRM_KID ) );

        m_PolicyRotationTime = GetCurrentTimeInSeconds();
    }
    
    DRM_LOCAL_LICENSE_POLICY_DESCRIPTOR     m_descriptor;
    DRM_LOCAL_LICENSE_TYPE                  m_licenseType;
    
    DRM_KID  m_KeyId;
    DRM_KID  m_RootKeyId;
    DWORD    m_PolicyRotationTime;
};

class CLicenseGenerator
{
public:
    CLicenseGenerator::CLicenseGenerator();
    virtual CLicenseGenerator::~CLicenseGenerator();

    bool CLicenseGenerator::EncryptAudio();
    HRESULT CLicenseGenerator::GetKeyId(  DRM_GUID  *pKeyId );
    HRESULT CLicenseGenerator::SetKeyId(  DRM_GUID  *pKeyId );
    HRESULT CLicenseGenerator::IsKeyRotationRequired(){ return m_bKeyRotationRequired; }
    DWORD CLicenseGenerator::GetKeyRotationDuration(){ return m_dwKeyRotationDuration; }
    
    HRESULT CLicenseGenerator::ProcessRequestCustomData( std::string strXml, DRM_SUBSTRING *pdasstrXML );
    HRESULT CLicenseGenerator::CreateLicensesForDelivery( DRM_APP_CONTEXT *poAppContext );
    HRESULT CLicenseGenerator::CreateLicenseForEncryption( DRM_LICENSE_HANDLE *phLicense, DRM_ID *pKeyId );
    
protected:

    HRESULT CLicenseGenerator::B64Decode( const std::string &str, std::vector<BYTE> &vb );
    std::string CLicenseGenerator::ParseAttribute( const std::string &strXml, DRM_SUBSTRING *pdasstrXML, std::string strAttributeName );
    
    HRESULT CLicenseGenerator::InitializeLicensePolicyDescriptor( std::shared_ptr<CLicenseDescriptor> spLicenseDescriptor );
    HRESULT CLicenseGenerator::ParseKeyRotation( std::string &strXml, DRM_SUBSTRING *pdasstrXML );
    HRESULT CLicenseGenerator::ParseEncryptAudio( std::string &strXml, DRM_SUBSTRING *pdasstrXML );
    
    HRESULT CLicenseGenerator::ParseRightsPool( std::string &strXml, DRM_SUBSTRING *pdasstrXML );
    HRESULT CLicenseGenerator::ParseLicenseRights( std::string &strXml, DRM_SUBSTRING *pdasstrXML );
    HRESULT CLicenseGenerator::ParsePlay( std::string &strXml, DRM_SUBSTRING *pdasstrXML, std::shared_ptr<CLicenseDescriptor> spLicenseDescriptor );
    
    HRESULT CLicenseGenerator::ParseExplicitOutputProtection( std::string &strXml, DRM_SUBSTRING *pdasstrXML, std::vector<BYTE> &vbId, std::vector<BYTE> &vbData );
    HRESULT CLicenseGenerator::ParseAnalogVideoExplicitOutputProtection( std::string &strXml, DRM_SUBSTRING *pdasstrXML, std::shared_ptr<CLicenseDescriptor> spLicenseDescriptor );
    HRESULT CLicenseGenerator::ParseDigitalAudioExplicitOutputProtection( std::string &strXml, DRM_SUBSTRING *pdasstrXML, std::shared_ptr<CLicenseDescriptor> spLicenseDescriptor );
    HRESULT CLicenseGenerator::ParsePlayEnabler( std::string &strXml, DRM_SUBSTRING *pdasstrXML, std::shared_ptr<CLicenseDescriptor> spLicenseDescriptor );

    HRESULT CLicenseGenerator::SetKIDProperty( DRM_KID *pKid );
    HRESULT CLicenseGenerator::CreateLocalBoundRootLicense( std::shared_ptr<CLicenseDescriptor> spLicenseDescriptor );
    HRESULT CLicenseGenerator::CreateRootLicense( std::shared_ptr<CLicenseDescriptor> spLicenseDescriptor );
    HRESULT CLicenseGenerator::CreateLeafLicense( std::shared_ptr<CLicenseDescriptor> spLicenseDescriptor );
    
    HRESULT CLicenseGenerator::CreateLocalBoundSimpleLicense( std::shared_ptr<CLicenseDescriptor> spLicenseDescriptor );
    HRESULT CLicenseGenerator::CreateSimpleLicense( std::shared_ptr<CLicenseDescriptor> spLicenseDescriptor );
    HRESULT CLicenseGenerator::GetLeafLicenseDescriptor( std::shared_ptr<CLicenseDescriptor> &spLeafLicenseDescryptor );
    
private:
    DRM_KID             m_KeyId;
    DRM_LICENSE_HANDLE  m_hLicense;
    DRM_LICENSE_HANDLE  m_hRootLicense;
    DRM_APP_CONTEXT    *m_poAppContext;
        
    std::vector<std::shared_ptr<CLicenseDescriptor>>    m_vspLicenseDescriptors;

    bool    m_bKeyRotationRequired;
    DWORD   m_dwKeyRotationDuration;
    bool    m_bEncryptAudio;
};
