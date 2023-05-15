/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdafx.h>
#include <FileLogger.h>
#include <TransmitterConfig.h>
#include <HelperFunctions.h>

#define TRANSMITTER_PATH_SIZE   1024

ENTER_PK_NAMESPACE_CODE;
extern  DRM_DWORD g_dwPRNDCRLExpiration; //defined in oeminfo.c
EXIT_PK_NAMESPACE_CODE;

using namespace std;

CTransmitterConfig::CTransmitterConfig()
    : m_fUPnPIsEnabled( false )
{
    m_dwTransmitterPort     = 0;
    m_dwMediaStreamerPort   = 0;
    g_dwPRNDCRLExpiration   = ( 20 * 365 * 24 * 60 * 60 ); /* 20 years */
}

CTransmitterConfig::~CTransmitterConfig()
{
    g_dstrDrmPath.pwszString = nullptr;
    g_dstrDrmPath.cchString  = 0;
}

HRESULT CTransmitterConfig::GetTransmitterPath( string &strPath )
{
    PRNDLogFunctionEntry();

    HRESULT     dr                              = DRM_SUCCESS;
    char        szPath[ TRANSMITTER_PATH_SIZE ] = {0};
    string      strFileName;

    if( !GetModuleFileNameA( NULL, szPath, TRANSMITTER_PATH_SIZE ) )
    {
        dr = HRESULT_FROM_WIN32( GetLastError() );
        goto ErrorExit;
    }

    strFileName = szPath;
    string::size_type index = strFileName.find_last_of( "\\" );
    strPath = strFileName.substr( 0, index );

ErrorExit:
    PRNDLogFunctionExitWithResult( dr );
    return dr;
}


HRESULT CTransmitterConfig::GetTransmitterLogFileFullPath( string &strFilePath )
{
    PRNDLogFunctionEntry();

    HRESULT     dr  = DRM_SUCCESS;
    string      strPath;

    ChkDR( GetTransmitterPath( strPath ) );
    strPath = append_backslash_if_not_exist( strPath );
    strFilePath = strPath + "Transmitter.log";

ErrorExit:
    PRNDLogFunctionExitWithResult( dr );
    return dr;
}

HRESULT CTransmitterConfig::GetPlayReadyHDSFileFullPath( string &strFilePath )
{
    PRNDLogFunctionEntry();

    HRESULT     dr  = DRM_SUCCESS;
    string      strPath;

    ChkDR( GetTransmitterPath( strPath ) );
    strPath = append_backslash_if_not_exist( strPath );
    strFilePath = strPath + "playready.hds";

ErrorExit:
    PRNDLogFunctionExitWithResult( dr );
    return dr;
}

HRESULT CTransmitterConfig::GetTransmitterConfigFileFullPath( string &strFilePath )
{
    PRNDLogFunctionEntry();

    HRESULT     dr  = DRM_SUCCESS;
    string      strPath;

    ChkDR( GetTransmitterPath( strPath ) );
    strPath = append_backslash_if_not_exist( strPath );
    strFilePath = strPath + "PRNDTransmitter.xml";

ErrorExit:
    PRNDLogFunctionExitWithResult( dr );
    return dr;
}

HRESULT CTransmitterConfig::SetupDrmPathGlobalVariable()
{
    PRNDLogFunctionEntry();

    HRESULT     dr  = DRM_SUCCESS;
    string      strPath;

    ChkDR( GetTransmitterPath( strPath ) );

    m_wstrDrmPath =  string_to_wstring( strPath );
    g_dstrDrmPath.pwszString = ( const DRM_WCHAR * ) m_wstrDrmPath.c_str();
    g_dstrDrmPath.cchString  = ( DRM_DWORD ) m_wstrDrmPath.length();

ErrorExit:
    PRNDLogFunctionExitWithResult( dr );
    return dr;
}

bool CTransmitterConfig::FileExists( string strFile )
{
    FILE* pFile     = nullptr;
    bool  bExists   = false;

    if( (pFile = ( FILE* )fopen ( strFile.c_str(), "r" )) != NULL )
    {
        bExists = TRUE;
        fclose ( pFile );
    }

    return bExists;
}

HRESULT CTransmitterConfig::ReadConfigFile( string &strXml )
{
    PRNDLogFunctionEntry();

    HRESULT     dr  = DRM_SUCCESS;

    string      strConfigFilePath;
    string      strTotalXml;
    string      strLine;
    ifstream    configStream;

    ChkDR( GetTransmitterConfigFileFullPath( strConfigFilePath ) );
    configStream.open( strConfigFilePath );
    while( getline( configStream, strLine ) )
    {
        strTotalXml += strLine;
    }

    strXml = strTotalXml;
ErrorExit:
    PRNDLogFunctionExitWithResult( dr );
    return dr;
}


HRESULT CTransmitterConfig::ParseVRoots( string &strXml, DRM_SUBSTRING *pdasstrXML )
{
    PRNDLogFunctionEntry();

    HRESULT             dr              = DRM_SUCCESS;
    DRM_SUBSTRING       dasstrVRoot     = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING       dasstrOffSet    = EMPTY_DRM_SUBSTRING;

    DRM_ANSI_CONST_STRING       dastrTag         = EMPTY_DRM_STRING;
    DWORD                       cNodes           = 0;

    string      strTagVRoots        = "VRoots";

    //Traverse to VRoots node data
    dastrTag.pszString = strTagVRoots.c_str();
    dastrTag.cchString = ( DRM_DWORD ) strTagVRoots.size();
    ChkDR( DRM_XML_GetNodeA( strXml.c_str(), pdasstrXML, &dastrTag, NULL, NULL, 0, NULL, &dasstrVRoot ) );
    ChkDR( DRM_XML_CountMatchingNodesA( strXml.c_str(), &dasstrVRoot,  NULL, NULL, NULL, &cNodes ));

    //Traverse to VRoot nodes
    for( DWORD iNode = 0; iNode < cNodes; iNode++ )
    {
        DRM_SUBSTRING dasstrTag = EMPTY_DRM_SUBSTRING;

        ChkDR( DRM_XML_EnumNextNodeA( strXml.c_str(),
                                   &dasstrVRoot,
                                    iNode,
                                   &dasstrTag,
                                    NULL,
                                    &dasstrOffSet,
                                    NULL,
                                    NULL ));

        string strVRoot( strXml, dasstrOffSet.m_ich, dasstrOffSet.m_cch );
        PRNDLogMessage( "VRoot = %s", strVRoot.c_str() );
        m_vstrVRoots.push_back( strVRoot );
    }


ErrorExit:
    PRNDLogFunctionExitWithResult( dr );
    return dr;
}


HRESULT CTransmitterConfig::ParseChannels( string &strXml, DRM_SUBSTRING *pdasstrXML )
{
    PRNDLogFunctionEntry();

    HRESULT             dr              = DRM_SUCCESS;
    DRM_SUBSTRING       dasstrChannel   = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING       dasstrNode      = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING       dasstrNodeData  = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING       dasstrTSFile    = EMPTY_DRM_SUBSTRING;

    DWORD                   cNodes           = 0;
    string                  strTagChannels   = "Channels";
    DRM_ANSI_CONST_STRING   dastrTag         = { strTagChannels.c_str(), ( DRM_DWORD ) strTagChannels.size() };

    DRM_SUBSTRING           dasstrTag       = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING           dasstrAttribute = EMPTY_DRM_SUBSTRING;

    string strAttributeName      = "Name";
    string strAttributeType      = "Type";
    DRM_ANSI_CONST_STRING   dastrAttributeName    = { strAttributeName.c_str(),  ( DRM_DWORD ) strAttributeName.size() };
    DRM_ANSI_CONST_STRING   dastrAttributeType    = { strAttributeType.c_str(),  ( DRM_DWORD ) strAttributeType.size() };


    ChkDR( DRM_XML_GetNodeA( strXml.c_str(), pdasstrXML, &dastrTag, NULL, NULL, 0, NULL, &dasstrChannel ) );
    ChkDR( DRM_XML_CountMatchingNodesA( strXml.c_str(), &dasstrChannel,  NULL, NULL, NULL, &cNodes ) );

    for( DWORD iNode = 0; iNode < cNodes; iNode++ )
    {
        ChkDR( DRM_XML_EnumNextNodeA( strXml.c_str(),
                                   &dasstrChannel,
                                    iNode,
                                   &dasstrTag,
                                   &dasstrNode,
                                   &dasstrNodeData,
                                    NULL,
                                    NULL ));

        ChkDR( DRM_XML_GetNodeAttributeA( strXml.c_str(),
                                        &dasstrNode,
                                        &dastrAttributeName,
                                        &dasstrAttribute ));

        string strName( strXml, dasstrAttribute.m_ich, dasstrAttribute.m_cch );
        PRNDLogMessage( "Channel name = %s", strName.c_str() );

        ChkDR( DRM_XML_GetNodeAttributeA( strXml.c_str(),
                                        &dasstrNode,
                                        &dastrAttributeType,
                                        &dasstrAttribute ));

        string strType( strXml, dasstrAttribute.m_ich, dasstrAttribute.m_cch );
        PRNDLogMessage( "Channel type = %s", strType.c_str() );

        CHANNEL_TYPE channelType = CHANNEL_TYPE_UNKNOWN;
        if( string_compare_case_insensitive( strType, "Http" ) )
        {
            channelType = CHANNEL_TYPE_HTTP;
        }
        else if( string_compare_case_insensitive( strType, "TSFile" ) )
        {
            channelType = CHANNEL_TYPE_TSFILE ;
        }
        else
        {
            PRNDLogMessage( "Unknown Channel type " );
            goto ErrorExit;
        }

        if( channelType == CHANNEL_TYPE_TSFILE )
        {
            auto spChannel = make_shared< CTSFileChannel >( strName, channelType );

            DWORD cTSFileNodes = 0;
            ChkDR( DRM_XML_CountMatchingNodesA( strXml.c_str(), &dasstrNodeData,  NULL, NULL, NULL, &cTSFileNodes ) );
            for( DWORD iTSFile = 0; iTSFile < cTSFileNodes; ++iTSFile )
            {
                ChkDR( DRM_XML_EnumNextNodeA( strXml.c_str(),
                                           &dasstrNodeData,
                                            iTSFile,
                                           &dasstrTag,
                                           &dasstrNode,
                                           &dasstrTSFile,
                                            NULL,
                                            NULL ));

                string strTSFile( strXml, dasstrTSFile.m_ich, dasstrTSFile.m_cch );
                PRNDLogMessage( "TS File = %s", strTSFile.c_str() );
                spChannel->AddTSFile( strTSFile );
            }

            m_vspChannels.push_back( spChannel );
        }
        else if( channelType == CHANNEL_TYPE_HTTP )
        {
            string strHttpPath( strXml, dasstrNodeData.m_ich, dasstrNodeData.m_cch );
            PRNDLogMessage( "Http Path = %s", strHttpPath.c_str() );

            auto spChannel = make_shared< CHttpChannel >( strName, channelType );
            spChannel->SetHttpPath( strHttpPath );

            m_vspChannels.push_back( spChannel );
        }
    }


ErrorExit:
    PRNDLogFunctionExitWithResult( dr );
    return dr;
}

shared_ptr<CChannel> CTransmitterConfig::GetChannel( const string &strName )
{
    shared_ptr<CChannel> spChannel = nullptr;
    for( vector<shared_ptr<CChannel>>::iterator iterChannel = m_vspChannels.begin();
         iterChannel != m_vspChannels.end();
         iterChannel++ )
    {
        shared_ptr<CChannel> channel = *iterChannel;
        if( string_compare_case_insensitive( channel->GetName(), strName ) )
        {
            spChannel = channel;
            break;
        }
    }

    return spChannel;
}

HRESULT CTransmitterConfig::GetTransmitterPort()
{
    return ( HRESULT )m_dwTransmitterPort;
}

HRESULT CTransmitterConfig::ParseTransmitterPort( string &strXml, DRM_SUBSTRING *pdasstrXML )
{
    PRNDLogFunctionEntry();

    HRESULT                 dr              = DRM_SUCCESS;
    DRM_SUBSTRING           dasstrData      = EMPTY_DRM_SUBSTRING;
    DRM_ANSI_CONST_STRING   dastrTag        = EMPTY_DRM_STRING;

    string strTagTransmitterPort  = "TransmitterPort";
    string strPort;

    dastrTag.pszString = strTagTransmitterPort.c_str();
    dastrTag.cchString = ( DRM_DWORD ) strTagTransmitterPort.size();
    ChkDR( DRM_XML_GetNodeA( strXml.c_str(), pdasstrXML, &dastrTag, NULL, NULL, 0, NULL, &dasstrData ) );

    strPort = string( strXml, dasstrData.m_ich, dasstrData.m_cch );
    PRNDLogMessage( "TransmitterPort = %s", strPort.c_str() );

    m_dwTransmitterPort = ( DWORD )std::stoi( strPort );

ErrorExit:
    PRNDLogFunctionExitWithResult( dr );
    return dr;
}

HRESULT CTransmitterConfig::GetMediaStreamerPort()
{
    return ( HRESULT )m_dwMediaStreamerPort;
}

HRESULT CTransmitterConfig::ParseMediaStreamerPort( string &strXml, DRM_SUBSTRING *pdasstrXML )
{
    PRNDLogFunctionEntry();

    HRESULT                 dr              = DRM_SUCCESS;
    DRM_SUBSTRING           dasstrData      = EMPTY_DRM_SUBSTRING;
    DRM_ANSI_CONST_STRING   dastrTag        = EMPTY_DRM_STRING;

    string strTagMediaStreamerPort  = "MediaStreamerPort";
    string strPort;

    dastrTag.pszString = strTagMediaStreamerPort.c_str();
    dastrTag.cchString = ( DRM_DWORD ) strTagMediaStreamerPort.size();
    ChkDR( DRM_XML_GetNodeA( strXml.c_str(), pdasstrXML, &dastrTag, NULL, NULL, 0, NULL, &dasstrData ) );

    strPort = string( strXml, dasstrData.m_ich, dasstrData.m_cch );
    PRNDLogMessage( "MediaStreamerPort = %s", strPort.c_str() );

    m_dwMediaStreamerPort = ( DWORD )std::stoi( strPort );

ErrorExit:
    PRNDLogFunctionExitWithResult( dr );
    return dr;
}

HRESULT CTransmitterConfig::ParseUPnPDeviceName( string &strXml, DRM_SUBSTRING *pdasstrXML )
{
    PRNDLogFunctionEntry();

    HRESULT                 dr              = DRM_SUCCESS;
    DRM_SUBSTRING           dasstrNode      = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING           dasstrData      = EMPTY_DRM_SUBSTRING;
    DRM_ANSI_CONST_STRING   dastrTag        = EMPTY_DRM_STRING;

    const char TAG_UPNP_DEVICE_NAME[] = "TransmitterUPnPDeviceName";

    string strAttributeValue;

    m_fUPnPIsEnabled = false;

    dastrTag.pszString = TAG_UPNP_DEVICE_NAME;
    dastrTag.cchString = ARRAYSIZE( TAG_UPNP_DEVICE_NAME ) - 1;

    dr = DRM_XML_GetNodeA( strXml.c_str(), pdasstrXML, &dastrTag, NULL, NULL, 0, &dasstrNode, &dasstrData );

    if( DRM_E_XMLNOTFOUND == dr )
    {
        dr = DRM_SUCCESS;
    }
    else
    {
        ChkDR( dr );

        m_strUPnPDeviceName = string( strXml, dasstrData.m_ich, dasstrData.m_cch );

        strAttributeValue = ParseAttribute( strXml, &dasstrNode, "Enabled" );

        m_fUPnPIsEnabled = ( strAttributeValue.empty() || string_compare_case_insensitive( strAttributeValue, "true" ) );
    }

    if( m_fUPnPIsEnabled )
    {
        PRNDLogMessage( "UPnP Enabled, DeviceName [%s]", m_strUPnPDeviceName.c_str() );
    }
    else
    {
        PRNDLogMessage( "UPnP Disabled" );
    }

ErrorExit:

    PRNDLogFunctionExitWithResult( dr );
    return dr;
}


std::string CTransmitterConfig::GetEncryptedContentArchivePath()
{
    return m_strEncryptedContentArchivePath;
}

std::string CTransmitterConfig::ParseAttribute( const string &strXml, DRM_SUBSTRING *pdasstrXML, string strAttributeName )
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

HRESULT CTransmitterConfig::ParseEncryptedContent( const string &strXml, DRM_SUBSTRING *pdasstrXML )
{
    PRNDLogFunctionEntry();

    HRESULT                 dr              = DRM_SUCCESS;
    DRM_SUBSTRING           dasstrNode      = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING           dasstrData      = EMPTY_DRM_SUBSTRING;
    DRM_ANSI_CONST_STRING   dastrTag        = EMPTY_DRM_STRING;

    string strTagEncryptedContent  = "EncryptedContent";
    string strAttributeValue;

    dastrTag.pszString = strTagEncryptedContent.c_str();
    dastrTag.cchString = ( DRM_DWORD ) strTagEncryptedContent.size();
    ChkDR( DRM_XML_GetNodeA( strXml.c_str(), pdasstrXML, &dastrTag, NULL, NULL, 0, &dasstrNode, &dasstrData ) );

    strAttributeValue = ParseAttribute( strXml, &dasstrNode, "Archive" );
    if( !strAttributeValue.empty() )
    {
        if( string_compare_case_insensitive( strAttributeValue, "true" ) )
        {
            m_strEncryptedContentArchivePath = string( strXml, dasstrData.m_ich, dasstrData.m_cch );
            PRNDLogMessage( "Archive Path = %s", m_strEncryptedContentArchivePath.c_str() );
        }
    }

ErrorExit:
    PRNDLogFunctionExitWithResult( dr );
    return dr;
}

std::string CTransmitterConfig::GetClearContentArchivePath()
{
    return m_strClearContentArchivePath;
}

HRESULT CTransmitterConfig::ParseClearContent( const string &strXml, DRM_SUBSTRING *pdasstrXML )
{
    PRNDLogFunctionEntry();

    HRESULT                 dr              = DRM_SUCCESS;
    DRM_SUBSTRING           dasstrNode      = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING           dasstrData      = EMPTY_DRM_SUBSTRING;
    DRM_ANSI_CONST_STRING   dastrTag        = EMPTY_DRM_STRING;

    string strTagClearContent  = "ClearContent";
    string strAttributeValue;

    dastrTag.pszString = strTagClearContent.c_str();
    dastrTag.cchString = ( DRM_DWORD ) strTagClearContent.size();
    ChkDR( DRM_XML_GetNodeA( strXml.c_str(), pdasstrXML, &dastrTag, NULL, NULL, 0, &dasstrNode, &dasstrData ) );

    strAttributeValue = ParseAttribute( strXml, &dasstrNode, "Archive" );
    if( !strAttributeValue.empty() )
    {
        if( string_compare_case_insensitive( strAttributeValue, "true" ) )
        {
            m_strClearContentArchivePath = string( strXml, dasstrData.m_ich, dasstrData.m_cch );
            PRNDLogMessage( "Archive Path = %s", m_strClearContentArchivePath.c_str() );
        }
    }

ErrorExit:
    PRNDLogFunctionExitWithResult( dr );
    return dr;
}

std::string CTransmitterConfig::GetEncryptorTraceFilePath()
{
    return m_strEncryptorTraceFilePath;
}

HRESULT CTransmitterConfig::ParseEncryptorTrace( const string &strXml, DRM_SUBSTRING *pdasstrXML )
{
    PRNDLogFunctionEntry();

    HRESULT                 dr              = DRM_SUCCESS;
    DRM_SUBSTRING           dasstrNode      = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING           dasstrData      = EMPTY_DRM_SUBSTRING;
    DRM_ANSI_CONST_STRING   dastrTag        = EMPTY_DRM_STRING;

    string strTagEncryptorTrace  = "EncryptorTrace";
    string strAttributeValue;

    dastrTag.pszString = strTagEncryptorTrace.c_str();
    dastrTag.cchString = ( DRM_DWORD ) strTagEncryptorTrace.size();
    ChkDR( DRM_XML_GetNodeA( strXml.c_str(), pdasstrXML, &dastrTag, NULL, NULL, 0, &dasstrNode, &dasstrData ) );

    strAttributeValue = ParseAttribute( strXml, &dasstrNode, "Enabled" );
    if( !strAttributeValue.empty() )
    {
        if( string_compare_case_insensitive( strAttributeValue, "true" ) )
        {
            m_strEncryptorTraceFilePath = string( strXml, dasstrData.m_ich, dasstrData.m_cch );
            PRNDLogMessage( "EncryptorTrace Path = %s", m_strEncryptorTraceFilePath.c_str() );
        }
    }

ErrorExit:
    PRNDLogFunctionExitWithResult( dr );
    return dr;
}

HRESULT CTransmitterConfig::ParseConfigFile()
{
    PRNDLogFunctionEntry();

    HRESULT                 dr              = DRM_SUCCESS;
    DRM_SUBSTRING           dasstrData      = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING           dasstrOffSet    = EMPTY_DRM_SUBSTRING;
    DRM_ANSI_CONST_STRING   dastrTag        = EMPTY_DRM_STRING;

    string      strXml;
    string      strTagTransmitter   = "Transmitter";

    ChkDR( ReadConfigFile( strXml ) );

    //Traverse to Transmitter node data
    dasstrOffSet.m_ich = 0;
    dasstrOffSet.m_cch = ( DRM_DWORD ) strXml.length();

    dastrTag.pszString = strTagTransmitter.c_str();
    dastrTag.cchString = ( DRM_DWORD ) strTagTransmitter.size();
    ChkDR( DRM_XML_GetNodeA( strXml.c_str(),&dasstrOffSet, &dastrTag, NULL, NULL, 0, NULL, &dasstrData ) );

    ChkDR( ParseTransmitterPort( strXml, &dasstrData ) );
    ChkDR( ParseMediaStreamerPort( strXml, &dasstrData ) );
    ChkDR( ParseUPnPDeviceName( strXml,&dasstrData ) );
    ChkDR( ParseClearContent( strXml, &dasstrData ) );
    ChkDR( ParseEncryptedContent( strXml, &dasstrData ) );
    ChkDR( ParseEncryptorTrace( strXml, &dasstrData ) );
    ChkDR( ParseVRoots( strXml, &dasstrData ) );
    ChkDR( ParseChannels( strXml, &dasstrData ) );

ErrorExit:
    PRNDLogFunctionExitWithResult( dr );
    return dr;
}

HRESULT CTransmitterConfig::GetCRLPackage( std::string &strCRLPackage )
{
    PRNDLogFunctionEntry();

    HRESULT     dr              = DRM_SUCCESS;
    string      strPath;
    string      strXml;
    string      strLine;
    ifstream    fileStream;

    ChkDR( GetTransmitterPath( strPath ) );
    strPath = append_backslash_if_not_exist( strPath );
    strPath = strPath + "revpackage.xml";

    PRNDLogMessage( "Reading package %s", strPath.c_str() );
    fileStream.open( strPath );
    if( !fileStream.is_open() )
    {
        PRNDLogMessage( "File couldn't be opened !!!!!!!!" );
        ChkDR( DRM_E_FAIL );
    }
    while( getline( fileStream, strLine ) )
    {
        strXml += strLine;
    }

    strCRLPackage = std::move( strXml );
    PRNDLogMessage( "package %s", strCRLPackage.c_str() );

ErrorExit:
    PRNDLogFunctionExitWithResult( dr );
    return dr;
}

bool CTransmitterConfig::UPnPIsEnabled() const
{
    return m_fUPnPIsEnabled;
}

std::string CTransmitterConfig::GetUPnPDeviceName() const
{
    return m_strUPnPDeviceName;
}

HRESULT CTransmitterConfig::GetFilePathFromHttpPath( const string &strHttpPath, string &strFilePath )
{
    PRNDLogFunctionEntry();

    HRESULT     dr  = DRM_SUCCESS;
    string      strPath;

    strFilePath.clear();
    ChkArg( !strHttpPath.empty() );
    
    for( vector<string>::iterator iterVRoots = m_vstrVRoots.begin();
         iterVRoots != m_vstrVRoots.end();
         iterVRoots++ )
    {
        strPath = *iterVRoots;
        string strRelativePath = strHttpPath;
        replace( strRelativePath.begin(), strRelativePath.end(), '/','\\' );

        if( !string_ends_with( strPath, "\\" ) )
        {
            strPath += "\\";
        }
        strPath += strRelativePath;

        PRNDLogMessage( "Verifying  existance of file %s", strPath.c_str() );
        if( FileExists( strPath ) )
        {
            strFilePath = strPath;
            break;
        }
    }

    if( strFilePath.empty() )
    {
        dr = E_FAIL;
    }

ErrorExit:
    PRNDLogFunctionExitWithResult( dr );
    return dr;
}
