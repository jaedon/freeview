/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#pragma once

enum CHANNEL_TYPE
{
    CHANNEL_TYPE_UNKNOWN = 0,
    CHANNEL_TYPE_TSFILE,
    CHANNEL_TYPE_HTTP
};

class CChannel
{

public:
    CChannel( std::string strName, CHANNEL_TYPE channelType )
    {
        m_strName       = strName;
        m_channelType   = channelType;
    }

    std::string GetName()
    {
        return m_strName;
    }

    virtual CHANNEL_TYPE GetChannelType()
    {
        return m_channelType;
    }


private:
    std::string    m_strName;
    CHANNEL_TYPE   m_channelType;
};

class CTSFileChannel : public CChannel
{
public:
    CTSFileChannel( std::string strName, CHANNEL_TYPE channelType ) :
        CChannel( strName, channelType )
    {

    }
    void AddTSFile( std::string strFile )
    {
        m_vTSFiles.push_back( strFile );
    }

    const std::vector<std::string>& GetTSFiles()
    {
        return m_vTSFiles;
    }

private:
    std::vector<std::string>    m_vTSFiles;

};

class CHttpChannel : public CChannel
{
public:
    CHttpChannel( std::string strName, CHANNEL_TYPE channelType ) :
        CChannel( strName, channelType )
    {

    }
    void SetHttpPath( std::string strHttpPath )
    {
        m_strHttpPath = strHttpPath;
    }

    std::string GetHttpPath()
    {
        return m_strHttpPath;
    }

private:
    std::string  m_strHttpPath;

};

class CTransmitterConfig
{

public:
    static CTransmitterConfig& CTransmitterConfig::Instance()
    {
        static CTransmitterConfig config;
        return config;
    }

    CTransmitterConfig();
    virtual ~CTransmitterConfig();

    HRESULT CTransmitterConfig::GetTransmitterPath( std::string &strPath );
    HRESULT CTransmitterConfig::GetTransmitterLogFileFullPath( std::string &strPath );
    HRESULT CTransmitterConfig::GetPlayReadyHDSFileFullPath( std::string &strFilePath );
    HRESULT CTransmitterConfig::GetTransmitterConfigFileFullPath( std::string &strFilePath );

    HRESULT CTransmitterConfig::SetupDrmPathGlobalVariable();
    HRESULT CTransmitterConfig::GetFilePathFromHttpPath( const std::string &strHttpPath, std::string &strFilePath );

    bool CTransmitterConfig::FileExists( std::string strFile );
    HRESULT CTransmitterConfig::ReadConfigFile( std::string &strXml );
    HRESULT CTransmitterConfig::ParseConfigFile();

    HRESULT CTransmitterConfig::GetTransmitterPort();
    HRESULT CTransmitterConfig::GetMediaStreamerPort();
    std::shared_ptr<CChannel> CTransmitterConfig::GetChannel( const std::string &strName );

    std::string CTransmitterConfig::GetClearContentArchivePath();
    std::string CTransmitterConfig::GetEncryptedContentArchivePath();
    std::string CTransmitterConfig::GetEncryptorTraceFilePath();

    HRESULT CTransmitterConfig::GetCRLPackage( std::string &strCRLPackage );

    bool            CTransmitterConfig::UPnPIsEnabled() const;
    std::string     CTransmitterConfig::GetUPnPDeviceName() const;

private:

    HRESULT CTransmitterConfig::ParseTransmitterPort( std::string &strXml, DRM_SUBSTRING *pdasstrXML );
    HRESULT CTransmitterConfig::ParseMediaStreamerPort( std::string &strXml, DRM_SUBSTRING *pdasstrXML );
    HRESULT CTransmitterConfig::ParseUPnPDeviceName( std::string &strXml, DRM_SUBSTRING *pdasstrXML );
    HRESULT CTransmitterConfig::ParseEncryptorTrace( const std::string &strXml, DRM_SUBSTRING *pdasstrXML );

    HRESULT CTransmitterConfig::ParseChannels( std::string &strXml, DRM_SUBSTRING *pdasstrXML );
    HRESULT CTransmitterConfig::ParseVRoots( std::string &strXml, DRM_SUBSTRING *pdasstrXML );

    std::string CTransmitterConfig::ParseAttribute( const std::string &strXml, DRM_SUBSTRING *pdasstrXML, std::string strAttributeName );
    HRESULT CTransmitterConfig::ParseClearContent( const std::string &strXml, DRM_SUBSTRING *pdasstrXML );
    HRESULT CTransmitterConfig::ParseEncryptedContent( const std::string &strXml, DRM_SUBSTRING *pdasstrXML );


    std::wstring                m_wstrDrmPath;
    std::vector<std::string>    m_vstrVRoots;

    DWORD   m_dwTransmitterPort;
    DWORD   m_dwMediaStreamerPort;

    std::vector< std::shared_ptr<CChannel> >    m_vspChannels;
    std::string                                 m_strClearContentArchivePath;
    std::string                                 m_strEncryptedContentArchivePath;
    std::string                                 m_strEncryptorTraceFilePath;
    std::string                                 m_strUPnPDeviceName;

    bool    m_fUPnPIsEnabled;

    std::string  m_strCRLPackage;
};


