/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdafx.h>
#include <LicenseGenerator.h>
#include <FileLogger.h>
#include <HelperFunctions.h>

ENTER_PK_NAMESPACE_CODE;
extern  DRM_LONG g_lOEMTimeOffset;
EXIT_PK_NAMESPACE_CODE;

using namespace std;

bool IsGUIDEqualToNULL( BYTE *pGuid )
{
    bool    bResult                     = false;
    BYTE    rgbTemp[ sizeof( DRM_GUID ) ] = {0};

    if( memcmp( pGuid, rgbTemp, sizeof( DRM_GUID ) ) == 0 )
    {
        bResult = true;
    }

    return bResult;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
CLicenseGenerator::CLicenseGenerator()
{
    memset( &m_KeyId, 0, sizeof( DRM_KID ) );
    m_hLicense      = DRM_LICENSE_HANDLE_INVALID;
    m_hRootLicense  = DRM_LICENSE_HANDLE_INVALID;

    m_bKeyRotationRequired  = true;
    m_bEncryptAudio         = true;
    m_dwKeyRotationDuration = 60; //seconds
    m_poAppContext          = nullptr;

}

CLicenseGenerator::~CLicenseGenerator()
{
    PRNDLogFunctionEntry();

    Drm_LocalLicense_Release( &m_hLicense );
    Drm_LocalLicense_Release( &m_hRootLicense );

    PRNDLogFunctionExit();
}

HRESULT CLicenseGenerator::B64Decode( const string &str, vector<BYTE> &vb )
{
    PRNDLogFunctionEntry();

    HRESULT         dr                  = DRM_SUCCESS;
    DRM_SUBSTRING   dasstrOffSet        = { 0, ( DRM_DWORD ) str.length() };
    DWORD           dwDestinationSize   = 0;

    vb.resize( str.size() );
    dwDestinationSize = ( DWORD ) vb.size();
    ChkDR( DRM_B64_DecodeA( str.c_str(), &dasstrOffSet, &dwDestinationSize, &vb[0], 0 ) );
    vb.resize( dwDestinationSize );

ErrorExit:
    PRNDLogFunctionExitWithResult( dr );
    return dr;
}

HRESULT CLicenseGenerator::ParseExplicitOutputProtection( string &strXml, DRM_SUBSTRING *pdasstrXML, vector<BYTE> &vbId, vector<BYTE> &vbData )
{
    PRNDLogFunctionEntry();

    HRESULT                 dr                      = DRM_SUCCESS;

    string                  strAttributeValueId;
    string                  strAttributeValueData;

    //Parse Id attribute
    strAttributeValueId = ParseAttribute( strXml, pdasstrXML, "Id" );
    if( strAttributeValueId.empty() )
    {
        goto ErrorExit;
    }

    //Parse Data attribute
    strAttributeValueData = ParseAttribute( strXml, pdasstrXML, "Data" );
    if( strAttributeValueData.empty() )
    {
        goto ErrorExit;
    }

    ChkDR( B64Decode( strAttributeValueId, vbId ) );
    ChkDR( B64Decode( strAttributeValueData, vbData ) );

    PRNDLogMessage( "Dump of OP Guid and Data:" );
    PRNDLogHexDump( ( BYTE* )&vbId[0], ( DWORD ) vbId.size(), false );
    PRNDLogHexDump( ( BYTE* )&vbData[0], ( DWORD ) vbData.size(), false );

    ChkBOOL( vbId.size() == sizeof( DRM_GUID ), DRM_E_FAIL );
    ChkBOOL( vbData.size() <= DRM_MAX_LOCAL_LICENSE_EXPLICIT_OUTPUT_PROTECTION_CONFIG_DATA, DRM_E_FAIL );

ErrorExit:
    PRNDLogFunctionExitWithResult( dr );
    return dr;
}

HRESULT CLicenseGenerator::ParseAnalogVideoExplicitOutputProtection( string &strXml, DRM_SUBSTRING *pdasstrXML, shared_ptr<CLicenseDescriptor> spLicenseDescriptor )
{
    PRNDLogFunctionEntry();

    HRESULT     dr    = DRM_SUCCESS;

    vector<BYTE>    vbId;
    vector<BYTE>    vbData;
    int             index   = 0;

    ChkDR( ParseExplicitOutputProtection( strXml, pdasstrXML, vbId, vbData ) );

    index = spLicenseDescriptor->m_descriptor.cExplicitAnalogVideoOutputProtections;
    memcpy_s( &spLicenseDescriptor->m_descriptor.rgoExplicitAnalogVideoOutputProtections[ index ].oProtectionId, sizeof( DRM_GUID ), &vbId[0], vbId.size() );

    spLicenseDescriptor->m_descriptor.rgoExplicitAnalogVideoOutputProtections[ index ].cbConfigData = ( DRM_WORD ) vbData.size();
    memcpy_s( &spLicenseDescriptor->m_descriptor.rgoExplicitAnalogVideoOutputProtections[ index ].rgbConfigData, vbData.size(), &vbData[0], vbData.size() );
    spLicenseDescriptor->m_descriptor.cExplicitAnalogVideoOutputProtections++;

ErrorExit:
    PRNDLogFunctionExitWithResult( dr );
    return dr;
}


HRESULT CLicenseGenerator::ParseDigitalAudioExplicitOutputProtection( string &strXml, DRM_SUBSTRING *pdasstrXML, shared_ptr<CLicenseDescriptor> spLicenseDescriptor )
{
    PRNDLogFunctionEntry();
    HRESULT     dr    = DRM_SUCCESS;

    vector<BYTE>    vbId;
    vector<BYTE>    vbData;
    int             index   = 0;

    ChkDR( ParseExplicitOutputProtection( strXml, pdasstrXML, vbId, vbData ) );

    index = spLicenseDescriptor->m_descriptor.cExplicitDigitalAudioOoutputProtections;
    memcpy_s( &spLicenseDescriptor->m_descriptor.rgoExplicitDigitalAudioOutputProtections[ index ].oProtectionId, sizeof( DRM_GUID ), &vbId[0], vbId.size() );

    spLicenseDescriptor->m_descriptor.rgoExplicitDigitalAudioOutputProtections[ index ].cbConfigData = ( DRM_WORD ) vbData.size();
    memcpy_s( &spLicenseDescriptor->m_descriptor.rgoExplicitDigitalAudioOutputProtections[ index ].rgbConfigData, vbData.size(), &vbData[0], vbData.size() );
    spLicenseDescriptor->m_descriptor.cExplicitDigitalAudioOoutputProtections++;


ErrorExit:
    PRNDLogFunctionExitWithResult( dr );
    return dr;
}

HRESULT CLicenseGenerator::ParsePlayEnabler( string &strXml, DRM_SUBSTRING *pdasstrXML, shared_ptr<CLicenseDescriptor> spLicenseDescriptor )
{
    PRNDLogFunctionEntry();
    HRESULT         dr      = DRM_SUCCESS;
    int             index   = 0;

    string          strAttributeValueId;
    vector<BYTE>    vbId;

    //Parse Id attribute
    strAttributeValueId = ParseAttribute( strXml, pdasstrXML, "Id" );
    if( strAttributeValueId.empty() )
    {
        goto ErrorExit;
    }

    ChkDR( B64Decode( strAttributeValueId, vbId ) );

    PRNDLogMessage( "Dump of PlayEnabler Guid:" );
    PRNDLogHexDump( ( BYTE* )&vbId[0], ( DWORD ) vbId.size(), false );

    ChkBOOL( vbId.size() == sizeof( DRM_GUID ), DRM_E_FAIL );

    index = spLicenseDescriptor->m_descriptor.cPlayEnablers;
    memcpy_s( &spLicenseDescriptor->m_descriptor.rgoPlayEnablers[ index ], sizeof( DRM_GUID ), &vbId[0], vbId.size() );
    spLicenseDescriptor->m_descriptor.cPlayEnablers++;

ErrorExit:
    PRNDLogFunctionExitWithResult( dr );
    return dr;
}

HRESULT CLicenseGenerator::ParsePlay( string &strXml, DRM_SUBSTRING *pdasstrXML, shared_ptr<CLicenseDescriptor> spLicenseDescriptor )
{
    PRNDLogFunctionEntry();

    HRESULT                 dr                      = DRM_SUCCESS;
    DRM_SUBSTRING           dasstrData              = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING           dasstrNode              = EMPTY_DRM_SUBSTRING;
    DWORD                   cNodes                  = 0;
    string                  strAttributeValue;

    string                  strPlay    = "Play";
    DRM_ANSI_CONST_STRING   dastrTag   = { strPlay.c_str(), ( DRM_DWORD ) strPlay.size() };

    //Parse AnalogVideoOPL attribute
    strAttributeValue = ParseAttribute( strXml, pdasstrXML, "AnalogVideoOPL" );
    if( !strAttributeValue.empty() )
    {
        spLicenseDescriptor->m_descriptor.wAnalogVideo = ( DRM_WORD )std::stoi( strAttributeValue );
    }

    //Parse CompressedDigitalAudioOPL attribute
    strAttributeValue = ParseAttribute( strXml, pdasstrXML, "CompressedDigitalAudioOPL" );
    if( !strAttributeValue.empty() )
    {
        spLicenseDescriptor->m_descriptor.wCompressedDigitalAudio = ( DRM_WORD )std::stoi( strAttributeValue );
    }

    //Parse CompressedDigitalVideoOPL attribute
    strAttributeValue = ParseAttribute( strXml, pdasstrXML, "CompressedDigitalVideoOPL" );
    if( !strAttributeValue.empty() )
    {
        spLicenseDescriptor->m_descriptor.wCompressedDigitalVideo = ( DRM_WORD )std::stoi( strAttributeValue );
    }

    //Parse UncompressedDigitalAudioOPL attribute
    strAttributeValue = ParseAttribute( strXml, pdasstrXML, "UncompressedDigitalAudioOPL" );
    if( !strAttributeValue.empty() )
    {
        spLicenseDescriptor->m_descriptor.wUncompressedDigitalAudio = ( DRM_WORD )std::stoi( strAttributeValue );
    }

    //Parse UncompressedDigitalVideoOPL attribute
    strAttributeValue = ParseAttribute( strXml, pdasstrXML, "UncompressedDigitalVideoOPL" );
    if( !strAttributeValue.empty() )
    {
        spLicenseDescriptor->m_descriptor.wUncompressedDigitalVideo = ( DRM_WORD )std::stoi( strAttributeValue );
    }

    ChkDR( DRM_XML_GetNodeA( strXml.c_str(), pdasstrXML, &dastrTag, NULL, NULL, 0, NULL, &dasstrData ) );
    ChkDR( DRM_XML_CountMatchingNodesA( strXml.c_str(), &dasstrData,  NULL, NULL, NULL, &cNodes ));
    PRNDLogMessage( "Node count = %d", cNodes );

    //Traverse  all child nodes
    for( DWORD iNode = 0; iNode < cNodes; iNode++ )
    {
        DRM_SUBSTRING dasstrTag = EMPTY_DRM_SUBSTRING;

        ChkDR( DRM_XML_EnumNextNodeA( strXml.c_str(),
                                     &dasstrData,
                                      iNode,
                                     &dasstrTag,
                                     &dasstrNode,
                                      NULL,
                                      NULL,
                                      NULL ) );
        string strTag( strXml, dasstrTag.m_ich, dasstrTag.m_cch );
        PRNDLogMessage( "Tag = %s", strTag.c_str() );

        string strNode( strXml, dasstrNode.m_ich, dasstrNode.m_cch );
        PRNDLogMessage( "Node = %s", strNode.c_str() );

        if( string_compare_case_insensitive( strTag, "AnalogVideoExplicitOutputProtection" ) )
        {
            ChkDR( ParseAnalogVideoExplicitOutputProtection( strXml, &dasstrNode, spLicenseDescriptor ) );
        }

        if( string_compare_case_insensitive( strTag, "DigitalAudioExplicitOutputProtection" ) )
        {
            ChkDR( ParseDigitalAudioExplicitOutputProtection( strXml, &dasstrNode, spLicenseDescriptor ) );
        }

        if( string_compare_case_insensitive( strTag, "PlayEnabler" ) )
        {
            ChkDR( ParsePlayEnabler( strXml, &dasstrNode, spLicenseDescriptor ) );
        }

    }

ErrorExit:
    PRNDLogFunctionExitWithResult( dr );
    return dr;
}

std::string CLicenseGenerator::ParseAttribute( const string &strXml, DRM_SUBSTRING *pdasstrXML, string strAttributeName )
{
    DRM_ANSI_CONST_STRING   dastrAttribute      = { strAttributeName.c_str(), ( DRM_DWORD ) strAttributeName.size() };
    DRM_SUBSTRING           dastrAttributeValue = EMPTY_DRM_SUBSTRING;
    string                  strAttributeValue;

    if( SUCCEEDED( DRM_XML_GetNodeAttributeA( strXml.c_str(), pdasstrXML, &dastrAttribute, &dastrAttributeValue ) ) )
    {
        strAttributeValue = string( strXml, dastrAttributeValue.m_ich, dastrAttributeValue.m_cch );
        PRNDLogMessage( "%s attribute value = %s", strAttributeName.c_str(), strAttributeValue.c_str() );
    }

    return strAttributeValue;
}

HRESULT CLicenseGenerator::ParseLicenseRights( string &strXml, DRM_SUBSTRING *pdasstrXML )
{
    PRNDLogFunctionEntry();

    HRESULT                 dr                  = DRM_SUCCESS;
    DRM_SUBSTRING           dasstrData          = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING           dasstrNode          = EMPTY_DRM_SUBSTRING;
    DWORD                   cNodes              = 0;
    string                  strAttributeValue;
    vector<BYTE>            vb;

    string                  strLicenseRights    = "LicenseRights";
    DRM_ANSI_CONST_STRING   dastrTag            = { strLicenseRights.c_str(), ( DRM_DWORD ) strLicenseRights.size() };

    shared_ptr<CLicenseDescriptor> spLicenseDescriptor = make_shared<CLicenseDescriptor>();

    ChkDR( InitializeLicensePolicyDescriptor( spLicenseDescriptor ) );

    //Parse Type attribute
    strAttributeValue = ParseAttribute( strXml, pdasstrXML, "Type" );
    if( !strAttributeValue.empty() )
    {
        if( string_compare_case_insensitive( strAttributeValue, "RemoteBoundRoot" ) )
        {
            spLicenseDescriptor->m_licenseType = eDRM_LOCAL_LICENSE_REMOTE_BOUND_ROOT;
        }
        else if( string_compare_case_insensitive( strAttributeValue, "Leaf" ) )
        {
            spLicenseDescriptor->m_licenseType = eDRM_LOCAL_LICENSE_LEAF;
        }
        else if( string_compare_case_insensitive( strAttributeValue, "RemoteBoundSimple" ) )
        {
            spLicenseDescriptor->m_licenseType = eDRM_LOCAL_LICENSE_REMOTE_BOUND_SIMPLE;
        }
        else if( string_compare_case_insensitive( strAttributeValue, "LocalBoundRoot" ) )
        {
            spLicenseDescriptor->m_licenseType = eDRM_LOCAL_LICENSE_LOCAL_BOUND_ROOT;
        }
        else if( string_compare_case_insensitive( strAttributeValue, "LocalBoundSimple" ) )
        {
            spLicenseDescriptor->m_licenseType = eDRM_LOCAL_LICENSE_LOCAL_BOUND_SIMPLE;
        }

    }

    //Parse RotatePolicyAtDuration attribute
    strAttributeValue = ParseAttribute( strXml, pdasstrXML, "RotatePolicyAtDuration" );
    if( !strAttributeValue.empty() )
    {
        DWORD dwNow = GetCurrentTimeInSeconds();
        spLicenseDescriptor->m_PolicyRotationTime = dwNow + ( DRM_WORD )std::stoi( strAttributeValue );
    }

    //Parse KeyId attribute
    strAttributeValue = ParseAttribute( strXml, pdasstrXML, "KeyId" );
    if( !strAttributeValue.empty() )
    {
        ChkDR( B64Decode( strAttributeValue, vb ) );
        ChkBOOL( vb.size() == sizeof( DRM_GUID ), DRM_E_FAIL );

        memcpy_s( &spLicenseDescriptor->m_KeyId, sizeof( DRM_KID ), &vb[0], vb.size() );
    }

    //Parse ParentKeyId attribute
    vb.resize( 0 );
    strAttributeValue = ParseAttribute( strXml, pdasstrXML, "ParentKeyId" );
    if( !strAttributeValue.empty() )
    {
        ChkDR( B64Decode( strAttributeValue, vb ) );
        ChkBOOL( vb.size() == sizeof( DRM_GUID ), DRM_E_FAIL );

        memcpy_s( &spLicenseDescriptor->m_RootKeyId, sizeof( DRM_KID ), &vb[0], vb.size() );
    }


    //Parse CannotPersist attribute
    strAttributeValue = ParseAttribute( strXml, pdasstrXML, "CannotPersist" );
    if( !strAttributeValue.empty() )
    {
        spLicenseDescriptor->m_descriptor.fCannotPersist = FALSE;
        if( string_compare_case_insensitive( strAttributeValue, "TRUE" ) )
        {
            spLicenseDescriptor->m_descriptor.fCannotPersist = TRUE;
        }
    }

    //Parse MinimumSecurityLevel attribute
    strAttributeValue = ParseAttribute( strXml, pdasstrXML, "MinimumSecurityLevel" );
    if( !strAttributeValue.empty() )
    {
        spLicenseDescriptor->m_descriptor.wSecurityLevel = ( DRM_WORD )std::stoi( strAttributeValue );
    }

    //Parse ExpirationDate attribute
    strAttributeValue = ParseAttribute( strXml, pdasstrXML, "ExpirationDate" );
    if( !strAttributeValue.empty() )
    {
        spLicenseDescriptor->m_descriptor.oExpiration.fValid        = TRUE;
        spLicenseDescriptor->m_descriptor.oExpiration.dwBeginDate   = DRM_LOCAL_LICENSE_EXPIRATION_MIN_BEGIN_DATE;
        spLicenseDescriptor->m_descriptor.oExpiration.dwEndDate     = ( DRM_DWORD )std::stoi( strAttributeValue );
    }

    //Parse ExpirationAfterFirstPlay attribute
    strAttributeValue = ParseAttribute( strXml, pdasstrXML, "ExpirationAfterFirstPlay" );
    if( !strAttributeValue.empty() )
    {
        spLicenseDescriptor->m_descriptor.oExpirationAfterFirstPlay.fValid  = TRUE;
        spLicenseDescriptor->m_descriptor.oExpirationAfterFirstPlay.dwValue = ( DRM_DWORD )std::stoi( strAttributeValue );
    }

    //Parse RealTimeExpiration attribute
    strAttributeValue = ParseAttribute( strXml, pdasstrXML, "RealTimeExpiration" );
    if( !strAttributeValue.empty() )
    {
        spLicenseDescriptor->m_descriptor.fRealTimeExpiration = FALSE;
        if( string_compare_case_insensitive( strAttributeValue, "TRUE" ) )
        {
            spLicenseDescriptor->m_descriptor.fRealTimeExpiration = TRUE;
        }
    }

    //Parse SourceId attribute
    strAttributeValue = ParseAttribute( strXml, pdasstrXML, "SourceId" );
    if( !strAttributeValue.empty() )
    {
        //Parse SourceId attribute
        string strAttributeValue2 = ParseAttribute( strXml, pdasstrXML, "RestrictedSourceId" );
        if( strAttributeValue.empty() )
        {
            PRNDLogMessage( "When source id specified, restricted source id must be specified !!!!!!!" );
            ChkDR( DRM_E_FAIL );
        }

        spLicenseDescriptor->m_descriptor.oSourceId.fValid       = true;
        spLicenseDescriptor->m_descriptor.oSourceId.dwSourceId   = ( DRM_DWORD )std::stoi( strAttributeValue );

        spLicenseDescriptor->m_descriptor.oSourceId.fRestrictedSourceId = false;
        if( string_compare_case_insensitive( strAttributeValue2, "TRUE" ) )
        {
            spLicenseDescriptor->m_descriptor.oSourceId.fRestrictedSourceId = TRUE;
        }
    }

    //Parse child nodes
    ChkDR( DRM_XML_GetNodeA( strXml.c_str(), pdasstrXML, &dastrTag, NULL, NULL, 0, NULL, &dasstrData ) );
    ChkDR( DRM_XML_CountMatchingNodesA( strXml.c_str(), &dasstrData,  NULL, NULL, NULL, &cNodes ) );
    PRNDLogMessage( "Node count = %d", cNodes );

    for( DWORD iNode = 0; iNode < cNodes; iNode++ )
    {
        DRM_SUBSTRING dasstrTag = EMPTY_DRM_SUBSTRING;

        ChkDR( DRM_XML_EnumNextNodeA( strXml.c_str(),
                                     &dasstrData,
                                      iNode,
                                     &dasstrTag,
                                     &dasstrNode,
                                      NULL,
                                      NULL,
                                      NULL ) );

        string strTag( strXml, dasstrTag.m_ich, dasstrTag.m_cch );
        PRNDLogMessage( "Tag = %s", strTag.c_str() );

        string strNode( strXml, dasstrNode.m_ich, dasstrNode.m_cch );
        PRNDLogMessage( "Node = %s", strNode.c_str() );

        if( string_compare_case_insensitive( strTag, "Play" ) )
        {
            ChkDR( ParsePlay( strXml, &dasstrNode, spLicenseDescriptor ) );
        }
    }

    m_vspLicenseDescriptors.push_back( spLicenseDescriptor );

ErrorExit:
    PRNDLogFunctionExitWithResult( dr );
    return dr;
}

/*
** The XML format used for license request:
**
** <RightsPool>
**     <KeyRotation Required="true/false" Duration="60"></KeyRotation>        //duration in seconds
**     <EncryptAudio Required="true/false"></EncryptAudio>
**     <TxRxClockDrift>-60</TxRxClockDrift>                                   //n or -n seconds
**     <LicenseRights
**       CannotPersist=true/false
**       Type=RemoteBoundRoot/Leaf/RemoteBoundSimple
**       RotatePolicyAtDuration="##"                                          //duration in seconds
**       KeyId=[Base64Guid]
**       ParentKeyId=[Base64Guid]
**       MinimumSecurityLevel=##
**       ExpirationDate=## ( number of seconds )
**       RealTimeExpiration="true/false"
**       SourceId=[UInt32]
**       RestrictedSourceId="true/false"
**         <Play
**           AnalogVideoOPL=##
**           CompressedDigitalAudioOPL=##
**           CompressedDigitalVideoOPL=##
**           UncompressedDigitalAudioOPL=##
**           UncompressedDigitalVideoOPL=##
**           ExpirationAfterFirstPlay=## >
**             <AnalogVideoExplicitOutputProtection Id="B64Guid" Data="B64ConfigData" ></AnalogVideoExplicitOutputProtection>
**             ... more Analog Video Explicit Output Protections ...
**             <DigitalAudioExplicitOutputProtection Id="B64Guid" Data="B64ConfigData" /></DigitalAudioExplicitOutputProtection>
**             ... more Digital Audio Explicit Output Protections ...
**             <PlayEnabler Id=[Base64Guid]> </PlayEnabler>
**             ... more PlayEnablers ...
**         </Play>
**     </LicenseRights>
**     ... more LicenseRights elements if requested ...
** </RightsPool>
**
*/

HRESULT CLicenseGenerator::ParseRightsPool( string &strXml, DRM_SUBSTRING *pdasstrXML )
{
    PRNDLogFunctionEntry();

    HRESULT                 dr              = DRM_SUCCESS;
    DRM_SUBSTRING           dasstrNode      = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING           dasstrNodeData  = EMPTY_DRM_SUBSTRING;
    DWORD                   cNodes          = 0;

    ChkDR( DRM_XML_CountMatchingNodesA( strXml.c_str(), pdasstrXML,  NULL, NULL, NULL, &cNodes ));
    PRNDLogMessage( "Node count = %d", cNodes );

    //Traverse all child nodes
    for( DWORD iNode = 0; iNode < cNodes; iNode++ )
    {
        DRM_SUBSTRING dasstrTag = EMPTY_DRM_SUBSTRING;

        ChkDR( DRM_XML_EnumNextNodeA( strXml.c_str(),
                                    pdasstrXML,
                                    iNode,
                                   &dasstrTag,
                                   &dasstrNode,
                                   &dasstrNodeData,
                                    NULL,
                                    NULL ));

        string strTag( strXml, dasstrTag.m_ich, dasstrTag.m_cch );
        PRNDLogMessage( "Tag = %s", strTag.c_str() );

        string strNode( strXml, dasstrNode.m_ich, dasstrNode.m_cch );
        PRNDLogMessage( "Node = %s", strNode.c_str() );

        string strNodeData( strXml, dasstrNodeData.m_ich, dasstrNodeData.m_cch );
        PRNDLogMessage( "Node data = %s", strNodeData.c_str() );

        if( string_compare_case_insensitive( strTag, "KeyRotation" ) )
        {
            ChkDR( ParseKeyRotation( strXml, &dasstrNode ) );
        }

        if( string_compare_case_insensitive( strTag, "LicenseRights" ) )
        {
            ChkDR( ParseLicenseRights( strXml, &dasstrNode ) );
        }

        if( string_compare_case_insensitive( strTag, "EncryptAudio" ) )
        {
            ChkDR( ParseEncryptAudio( strXml, &dasstrNode ) );
        }

        if( string_compare_case_insensitive( strTag, "TxRxClockDrift" ) )
        {
            g_lOEMTimeOffset = ( DRM_LONG )std::stoi( strNodeData );
        }

    }

ErrorExit:
    PRNDLogFunctionExitWithResult( dr );
    return dr;
}

HRESULT CLicenseGenerator::SetKIDProperty( DRM_KID *pKid )
{
    PRNDLogFunctionEntry();
    HRESULT     dr      = DRM_SUCCESS;
    DRM_DWORD   cchKid  = CCH_BASE64_EQUIV( SIZEOF( DRM_KID ) );

    DRM_WCHAR   rgwchKid[ CCH_BASE64_EQUIV( SIZEOF( DRM_GUID ) ) + 1 ] = {0};

    ChkDR( DRM_B64_EncodeW(
        ( const DRM_BYTE* )pKid,
        SIZEOF( DRM_KID ),
        rgwchKid,
        &cchKid,
        0 ) );
    rgwchKid[ CCH_BASE64_EQUIV( SIZEOF( DRM_KID ) ) ] = g_wchNull;

    ChkDR( Drm_Content_SetProperty(
        m_poAppContext,
        DRM_CSP_KID,
        ( const DRM_BYTE* )rgwchKid,
        SIZEOF( rgwchKid ) - SIZEOF( DRM_WCHAR ) ) );     /* Function *requires* the KID to NOT include the null terminator */

ErrorExit:
    PRNDLogFunctionExitWithResult( dr );
    return dr;
}

HRESULT CLicenseGenerator::CreateLocalBoundRootLicense( shared_ptr<CLicenseDescriptor> spLicenseDescriptor )
{
    PRNDLogFunctionEntry();
    HRESULT  dr      = DRM_SUCCESS;
    DRM_KID  kidRoot = {0};

    const DRM_CONST_STRING      *pdstrPlay      = &g_dstrWMDRM_RIGHT_PLAYBACK;

    ChkArg( m_hRootLicense == DRM_LICENSE_HANDLE_INVALID );

    PRNDLogMessage( "Creating local bound root license..." );
    if( IsGUIDEqualToNULL( ( BYTE* ) &spLicenseDescriptor->m_KeyId ) )
    {
        PRNDLogMessage( "CustomData didn't specify KeyId" );
        memcpy_s( &kidRoot, sizeof( DRM_GUID ), &m_KeyId, sizeof( DRM_GUID ) );
    }
    else
    {
        memcpy_s( &kidRoot, sizeof( DRM_GUID ), &spLicenseDescriptor->m_KeyId, sizeof( DRM_GUID ) );
    }

    ChkDR( Drm_LocalLicense_CreateLicense(
                                           m_poAppContext,
                                          &spLicenseDescriptor->m_descriptor,
                                           spLicenseDescriptor->m_licenseType,
                              ( DRM_KID * ) &kidRoot,
                                           NULL,
                                           NULL,
                                           NULL,
                                          &m_hRootLicense ) );

    PRNDLogMessage( "KID = " );
    PRNDLogHexDump( ( BYTE* ) &kidRoot, sizeof( DRM_ID ), false );

    PRNDLogMessage( "Calling Drm_LocalLicense_StoreLicense()" );
    ChkDR( Drm_LocalLicense_StoreLicense( m_hRootLicense, eDRM_LOCAL_LICENSE_XMR_STORE ) );
    ChkDR( SetKIDProperty( &kidRoot ) );

    PRNDLogMessage( "Calling Drm_Reader_Bind()" );
    ChkDR( Drm_Reader_Bind(
            m_poAppContext,
            &pdstrPlay,
            1,
            NULL,
            NULL,
            NULL ) );

    PRNDLogMessage( "Calling PrepareLicensesForTransmit()" );
    ChkDR( Drm_Prnd_Transmitter_PrepareLicensesForTransmit( m_poAppContext ) );

ErrorExit:
    PRNDLogFunctionExitWithResult( dr );
    return dr;
}

HRESULT CLicenseGenerator::CreateRootLicense( shared_ptr<CLicenseDescriptor> spLicenseDescriptor )
{
    PRNDLogFunctionEntry();
    HRESULT  dr      = DRM_SUCCESS;
    DRM_KID  kidRoot = {0};

    ChkArg( m_hRootLicense == DRM_LICENSE_HANDLE_INVALID );

    PRNDLogMessage( "Creating remote bound root license..." );
    if( IsGUIDEqualToNULL( ( BYTE* ) &spLicenseDescriptor->m_KeyId ) )
    {
        PRNDLogMessage( "CustomData didn't specify KeyId" );
        memcpy_s( &kidRoot, sizeof( DRM_GUID ), &m_KeyId, sizeof( DRM_GUID ) );
    }
    else
    {
        memcpy_s( &kidRoot, sizeof( DRM_GUID ), &spLicenseDescriptor->m_KeyId, sizeof( DRM_GUID ) );
    }

    ChkDR( Drm_LocalLicense_CreateLicense(
                                           m_poAppContext,
                                          &spLicenseDescriptor->m_descriptor,
                                           spLicenseDescriptor->m_licenseType,
                              ( DRM_KID * ) &kidRoot,
                                           NULL,
                                           NULL,
                                           NULL,
                                          &m_hRootLicense ) );

    PRNDLogMessage( "KID = " );
    PRNDLogHexDump( ( BYTE* ) &kidRoot, sizeof( DRM_ID ), false );

    PRNDLogMessage( "Calling Drm_LocalLicense_StoreLicense()" );
    ChkDR( Drm_LocalLicense_StoreLicense( m_hRootLicense, eDRM_LOCAL_LICENSE_PRND_SESSION_STORE ) );

ErrorExit:
    PRNDLogFunctionExitWithResult( dr );
    return dr;
}

HRESULT CLicenseGenerator::CreateLocalBoundSimpleLicense( shared_ptr<CLicenseDescriptor> spLicenseDescriptor )
{
    PRNDLogFunctionEntry();
    HRESULT  dr  = DRM_SUCCESS;
    DRM_KID  kid = {0};

    const DRM_CONST_STRING *pdstrPlay      = &g_dstrWMDRM_RIGHT_PLAYBACK;
    DRM_DECRYPT_CONTEXT     oTxEncryptCtx  = {0};

    PRNDLogMessage( "Creating local bound simple license..." );
    if( IsGUIDEqualToNULL( ( BYTE* ) &spLicenseDescriptor->m_KeyId ) )
    {
        PRNDLogMessage( "CustomData didn't specify KeyId" );
        memcpy_s( &kid, sizeof( DRM_GUID ), &m_KeyId, sizeof( DRM_GUID ) );
    }
    else
    {
        memcpy_s( &kid, sizeof( DRM_GUID ), &spLicenseDescriptor->m_KeyId, sizeof( DRM_GUID ) );
    }

    ChkDR( Drm_LocalLicense_CreateLicense(
                                          m_poAppContext,
                                         &spLicenseDescriptor->m_descriptor,
                                          spLicenseDescriptor->m_licenseType,
                             ( DRM_KID * ) &kid,
                                          NULL,
                                          NULL,
                                          NULL,
                                         &m_hLicense ));
    PRNDLogMessage( "KID = " );
    PRNDLogHexDump( ( BYTE* ) &kid, sizeof( DRM_ID ), false );

    PRNDLogMessage( "Calling Drm_LocalLicense_StoreLicense()" );
    ChkDR( Drm_LocalLicense_StoreLicense( m_hLicense, eDRM_LOCAL_LICENSE_XMR_STORE ) );
    ChkDR( SetKIDProperty( &kid ) );

    PRNDLogMessage( "Calling Drm_Reader_Bind()" );
    ChkDR( Drm_Reader_Bind(
            m_poAppContext,
            &pdstrPlay,
            1,
            NULL,
            NULL,
            &oTxEncryptCtx ) );

    PRNDLogMessage( "Calling PrepareLicensesForTransmit()" );
    ChkDR( Drm_Prnd_Transmitter_PrepareLicensesForTransmit( m_poAppContext ) );

ErrorExit:
    PRNDLogFunctionExitWithResult( dr );
    return dr;
}


HRESULT CLicenseGenerator::CreateSimpleLicense( shared_ptr<CLicenseDescriptor> spLicenseDescriptor )
{
    PRNDLogFunctionEntry();
    HRESULT                 dr          = DRM_SUCCESS;
    DRM_KID  kid = {0};

    PRNDLogMessage( "Creating remote bound simple license..." );
    if( IsGUIDEqualToNULL( ( BYTE* ) &spLicenseDescriptor->m_KeyId ) )
    {
        PRNDLogMessage( "CustomData didn't specify KeyId" );
        memcpy_s( &kid, sizeof( DRM_GUID ), &m_KeyId, sizeof( DRM_GUID ) );
    }
    else
    {
        memcpy_s( &kid, sizeof( DRM_GUID ), &spLicenseDescriptor->m_KeyId, sizeof( DRM_GUID ) );
    }

    ChkDR( Drm_LocalLicense_CreateLicense(
                                          m_poAppContext,
                                         &spLicenseDescriptor->m_descriptor,
                                          spLicenseDescriptor->m_licenseType,
                             ( DRM_KID * ) &kid,
                                          NULL,
                                          NULL,
                                          NULL,
                                         &m_hLicense ));
    PRNDLogMessage( "KID = " );
    PRNDLogHexDump( ( BYTE* ) &kid, sizeof( DRM_ID ), false );

    PRNDLogMessage( "Calling Drm_LocalLicense_StoreLicense()" );
    ChkDR( Drm_LocalLicense_StoreLicense( m_hLicense, eDRM_LOCAL_LICENSE_PRND_SESSION_STORE ) );

ErrorExit:
    PRNDLogFunctionExitWithResult( dr );
    return dr;
}

HRESULT CLicenseGenerator::CreateLeafLicense( shared_ptr<CLicenseDescriptor> spLicenseDescriptor )
{
    PRNDLogFunctionEntry();
    HRESULT                 dr          = DRM_SUCCESS;
    DRM_LICENSE_HANDLE      hLicense    = DRM_LICENSE_HANDLE_INVALID;
    DRM_KID                 kid         = {0};

    if( m_hRootLicense == DRM_LICENSE_HANDLE_INVALID )
    {
        PRNDLogMessage( "Root license doesn't exist !!!!!!" );
        ChkDR( DRM_E_FAIL );
    }

    PRNDLogMessage( "Creating leaf license..." );
    if( IsGUIDEqualToNULL( ( BYTE* )&spLicenseDescriptor->m_KeyId ) )
    {
        PRNDLogMessage( "CustomData didn't specify KeyId" );
        ChkDR( Oem_Random_GetBytes( NULL, ( DRM_BYTE * )&kid, SIZEOF( DRM_KID )) );
    }
    else
    {
        memcpy_s( &kid, sizeof( DRM_GUID ), &spLicenseDescriptor->m_KeyId, sizeof( DRM_GUID ) );
    }

    ChkDR( Drm_LocalLicense_CreateLicense(
                                          m_poAppContext,
                                         &spLicenseDescriptor->m_descriptor,
                                          spLicenseDescriptor->m_licenseType,
                             ( DRM_KID * ) &kid,
                                          NULL,
                                          NULL,
                                          m_hRootLicense,
                                         &hLicense ));
    PRNDLogMessage( "KID = " );
    PRNDLogHexDump( ( BYTE* ) &kid, sizeof( DRM_ID ), false );

    PRNDLogMessage( "Calling Drm_LocalLicense_StoreLicense()" );
    ChkDR( Drm_LocalLicense_StoreLicense( hLicense, eDRM_LOCAL_LICENSE_PRND_SESSION_STORE ) );

ErrorExit:
    if( hLicense != DRM_LICENSE_HANDLE_INVALID )
    {
        Drm_LocalLicense_Release( &hLicense );
    }
    PRNDLogFunctionExitWithResult( dr );
    return dr;
}

HRESULT CLicenseGenerator::CreateLicensesForDelivery( DRM_APP_CONTEXT *poAppContext )
{
    PRNDLogFunctionEntry();
    HRESULT dr = DRM_SUCCESS;

    m_poAppContext = poAppContext;

    for( vector<shared_ptr<CLicenseDescriptor>>::iterator licenseIterator = m_vspLicenseDescriptors.begin();
         licenseIterator != m_vspLicenseDescriptors.end();
         licenseIterator++ )
    {
        shared_ptr<CLicenseDescriptor> spLicenseDescriptor = *licenseIterator;

        switch( spLicenseDescriptor->m_licenseType )
        {
            case eDRM_LOCAL_LICENSE_REMOTE_BOUND_ROOT:
                ChkDR( CreateRootLicense( spLicenseDescriptor ) );
                break;
            case eDRM_LOCAL_LICENSE_REMOTE_BOUND_SIMPLE:
                ChkDR( CreateSimpleLicense( spLicenseDescriptor ) );
                break;
            case eDRM_LOCAL_LICENSE_LEAF:
                if( m_bKeyRotationRequired )
                {
                    continue;
                }
                ChkDR( CreateLeafLicense( spLicenseDescriptor ) );
                break;
            case eDRM_LOCAL_LICENSE_LOCAL_BOUND_ROOT:
                ChkDR( CreateLocalBoundRootLicense( spLicenseDescriptor ) );
                break;
            case eDRM_LOCAL_LICENSE_LOCAL_BOUND_SIMPLE:
                ChkDR( CreateLocalBoundSimpleLicense( spLicenseDescriptor ) );
                break;
        }

    }

    if( m_bKeyRotationRequired )
    {
        if( m_hRootLicense == DRM_LICENSE_HANDLE_INVALID )
        {
            PRNDLogMessage( "Key Rotation specified. However, NO root license provided" );
            ChkDR( DRM_E_FAIL );
        }

        shared_ptr<CLicenseDescriptor> spLeafLicenseDescriptor;
        ChkDR( GetLeafLicenseDescriptor( spLeafLicenseDescriptor ) );
    }

ErrorExit:
    PRNDLogFunctionExitWithResult( dr );
    return dr;
}

HRESULT CLicenseGenerator::GetLeafLicenseDescriptor( shared_ptr<CLicenseDescriptor>& spLeafLicenseDescriptor )
{
    HRESULT dr      = DRM_E_FAIL;
    DWORD   dwNow   = GetCurrentTimeInSeconds();

    for( vector<shared_ptr<CLicenseDescriptor>>::iterator licenseIterator = m_vspLicenseDescriptors.begin();
         licenseIterator != m_vspLicenseDescriptors.end();
         licenseIterator++ )
    {
        shared_ptr<CLicenseDescriptor> spLicenseDescriptor = *licenseIterator;

        if( spLicenseDescriptor->m_licenseType == eDRM_LOCAL_LICENSE_LEAF &&
            spLicenseDescriptor->m_PolicyRotationTime <= dwNow )
        {
            if( spLeafLicenseDescriptor )
            {
                if( spLeafLicenseDescriptor->m_PolicyRotationTime < spLicenseDescriptor->m_PolicyRotationTime )
                {
                    spLeafLicenseDescriptor = spLicenseDescriptor;
                }
            }
            else
            {
                spLeafLicenseDescriptor = spLicenseDescriptor;
            }
            dr = DRM_SUCCESS;
        }
    }

    if( FAILED( dr ) )
    {
        PRNDLogFunctionExitWithResult( dr );
    }
    return dr;
}

HRESULT CLicenseGenerator::CreateLicenseForEncryption( DRM_LICENSE_HANDLE *phLicense, DRM_ID *pKeyId )
{
    HRESULT            dr       = DRM_SUCCESS;
    DRM_KID            oKeyId   = {0};
    DRM_LICENSE_HANDLE hLicense = DRM_LICENSE_HANDLE_INVALID;

    if( m_bKeyRotationRequired )
    {
        shared_ptr<CLicenseDescriptor> spLeafLicenseDescryptor;
        ChkDR( GetLeafLicenseDescriptor( spLeafLicenseDescryptor ) );

        ChkDR( Oem_Random_GetBytes( NULL, ( DRM_BYTE * )&oKeyId, SIZEOF( DRM_KID )) );
        PRNDLogMessage( "Creating leaf license..." );
        ChkDR( Drm_LocalLicense_CreateLicense(
              m_poAppContext,
             &spLeafLicenseDescryptor->m_descriptor,
              eDRM_LOCAL_LICENSE_LEAF,
             &oKeyId,
              NULL,
              NULL,
              m_hRootLicense,
             &hLicense ));
        memcpy( pKeyId, &oKeyId, sizeof( DRM_GUID ));
    }
    else
    {
        Drm_LocalLicense_AddRef( m_hLicense );
        hLicense = m_hLicense;
        memcpy( pKeyId, &m_KeyId, sizeof( DRM_GUID ));
    }

    *phLicense = hLicense;

ErrorExit:
    if( FAILED( dr ) )
    {
        PRNDLogFunctionExitWithResult( dr );
    }
    return dr;
}

HRESULT CLicenseGenerator::InitializeLicensePolicyDescriptor( shared_ptr<CLicenseDescriptor> spLicenseDescriptor )
{
    PRNDLogFunctionEntry();
    HRESULT    dr  = DRM_SUCCESS;

    PRNDLogMessage( "Calling Drm_LocalLicense_InitializePolicyDescriptor()" );
    ChkDR( Drm_LocalLicense_InitializePolicyDescriptor( &spLicenseDescriptor->m_descriptor ) );

    spLicenseDescriptor->m_descriptor.wSecurityLevel     = 150;
    spLicenseDescriptor->m_descriptor.fCannotPersist     = TRUE;

ErrorExit:
    PRNDLogFunctionExitWithResult( dr );
    return dr;
}

HRESULT CLicenseGenerator::ParseKeyRotation( string &strXml, DRM_SUBSTRING *pdasstrXML )
{
    PRNDLogFunctionEntry();

    HRESULT dr = DRM_SUCCESS;
    string  strAttributeValue;

    strAttributeValue = ParseAttribute( strXml, pdasstrXML, "Required" );
    if( !strAttributeValue.empty() )
    {
        if( string_compare_case_insensitive( strAttributeValue, "TRUE" ) )
        {
            m_bKeyRotationRequired = true;
        }
        else
        {
            m_bKeyRotationRequired = false;
        }
    }

    strAttributeValue = ParseAttribute( strXml, pdasstrXML, "Duration" );
    if( !strAttributeValue.empty() )
    {
        m_dwKeyRotationDuration = ( DRM_DWORD )std::stoi( strAttributeValue );
    }

    PRNDLogFunctionExitWithResult( dr );
    return dr;
}

bool CLicenseGenerator::EncryptAudio()
{
    return m_bEncryptAudio;
}

HRESULT CLicenseGenerator::ParseEncryptAudio( string &strXml, DRM_SUBSTRING *pdasstrXML )
{
    PRNDLogFunctionEntry();

    HRESULT                 dr                      = DRM_SUCCESS;
    string                  strAttributeValue;

    strAttributeValue = ParseAttribute( strXml, pdasstrXML, "Required" );
    if( !strAttributeValue.empty() )
    {
        if( string_compare_case_insensitive( strAttributeValue, "TRUE" ) )
        {
            m_bEncryptAudio = true;
        }
        else
        {
            m_bEncryptAudio = false;
        }
    }

    PRNDLogFunctionExitWithResult( dr );
    return dr;
}

HRESULT CLicenseGenerator::ProcessRequestCustomData( string strXml, DRM_SUBSTRING *pdasstrXML )
{
    PRNDLogFunctionEntry();

    HRESULT                 dr              = DRM_SUCCESS;
    DRM_SUBSTRING           dasstrData      = EMPTY_DRM_SUBSTRING;

    string                  strTagRightsPool    = "RightsPool";
    DRM_ANSI_CONST_STRING   dastrTag            = { strTagRightsPool.c_str(), ( DRM_DWORD ) strTagRightsPool.size() };

    //Traverse to RightsPool node data
    ChkDR( DRM_XML_GetNodeA( strXml.c_str(), pdasstrXML, &dastrTag, NULL, NULL, 0, NULL, &dasstrData ) );

    ChkDR( ParseRightsPool( strXml, &dasstrData ) );

ErrorExit:
    PRNDLogFunctionExitWithResult( dr );
    return dr;
}


HRESULT CLicenseGenerator::GetKeyId(  DRM_GUID  *pKeyId )
{
    PRNDLogFunctionEntry();
    HRESULT     dr   = DRM_SUCCESS;

    ChkArg( pKeyId != NULL );
    memcpy( pKeyId, &m_KeyId, sizeof( DRM_GUID ));

ErrorExit:
    PRNDLogFunctionExitWithResult( dr );
    return dr;
}

HRESULT CLicenseGenerator::SetKeyId(  DRM_GUID  *pKeyId )
{
    PRNDLogFunctionEntry();
    HRESULT     dr   = DRM_SUCCESS;

    ChkArg( pKeyId != NULL );
    memcpy( &m_KeyId, pKeyId, sizeof( DRM_GUID ));

ErrorExit:
    PRNDLogFunctionExitWithResult( dr );
    return dr;
}



