/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdafx.h>

#include <UPnPUtils.h>
#include <UPnPDeviceEnum.h>

#include <PRND_UPnP_TxService.h>
#include <PRND_UPnP_TxControl.h>

#include <strsafe.h>
#include <Utilities.h>

#include <PRNDMemHolder.h>
#include <PRNDReceiverSession.h>
#include <Socket.h>

////////////////////////////////////////////////////////////////////////////////
//
// Constants
//
////////////////////////////////////////////////////////////////////////////////

const WCHAR PRND_TRANSMITTER_UPNP_DEVICE_TYPE[] = L"urn:microsoft_com:device:PRNDTestTransmitter:1";

////////////////////////////////////////////////////////////////////////////////
//
// Classes
//
////////////////////////////////////////////////////////////////////////////////


/// <summary>
/// Retrieves the path to the PlayReady Hashed Data Storage
/// </summary>
HRESULT GetPlayReadyStorePaths(
    __inout std::wstring &strDataStorePath,
    __inout std::wstring &strRevocationStorePath,
    __inout std::wstring &strLastTransmitterFile )
{
    HRESULT dr      = S_OK;
    ULONG   cchUsed = 0;
    WCHAR   szPath[MAX_PATH];
    LPWSTR  pchLastDirSeparator = nullptr;

    cchUsed = ::GetModuleFileNameW( NULL, szPath, ARRAYSIZE( szPath ) );

    ChkDR( VerifyWin32( 0 < cchUsed ) );
    ChkBOOL( cchUsed < ARRAYSIZE( szPath ), STRSAFE_E_INSUFFICIENT_BUFFER );
    
    pchLastDirSeparator = wcsrchr( szPath, L'\\' );

    ChkBOOL( pchLastDirSeparator != nullptr, HRESULT_FROM_WIN32( ERROR_PATH_NOT_FOUND ) );

    ++pchLastDirSeparator;
    *pchLastDirSeparator = L'\0';

    ChkDR( StringCchCatW( szPath, ARRAYSIZE( szPath ), L"Playready.hds" ) );
    strDataStorePath = szPath;
    *pchLastDirSeparator = L'\0';

    ChkDR( StringCchCatW( szPath, ARRAYSIZE( szPath ), L"Revpackage.xml" ) );
    strRevocationStorePath = szPath;
    *pchLastDirSeparator = L'\0';

    ChkDR( StringCchCatW( szPath, ARRAYSIZE( szPath ), L"LastTransmitter.txt" ) );
    strLastTransmitterFile = szPath;

ErrorExit:

    return dr;
}

/// <summary>
/// Holds the application's state and executes its commands
/// </summary>
class PRNDReceiverApp
{
public:

    PRNDReceiverApp()
        : m_fErrorReportedForCommand( false )
        , m_pTransmitterControl( nullptr )
        , m_pReceiverSession( nullptr )
    {
        m_itCurrentTransmitter = m_knownTransmitters.end();
    }

    ~PRNDReceiverApp()
    {
        _OnTransmitterChange();
    }

    HRESULT Initialize();
    HRESULT SetTransmitterUDN( __in LPCWSTR pszUDN );
    void ReportStatus();
    void ExecuteRegistration();
    void ExecuteProximityDetection();
    void ExecuteFetchLicense();
    void ResetPrndSession();
    void SetProximityChannel( __in LPCWSTR pszHost, __in int nPort );
    void ListPrndTransmitters();

private:
    std::wstring m_strPlayReadyStorePath;
    std::wstring m_strPlayReadyRevocationStorePackage;
    std::wstring m_strLastTxFile;
    bool m_fErrorReportedForCommand;

    struct TRANSMITTER_INFO
    {
        PRNDSessionInfo sessionInfo;
        std::wstring strProximityHostName;
        int proximityPort;
        bool fProximityChannelsSet;
        bool fProximitySucceeded;

        TRANSMITTER_INFO()
            : fProximityChannelsSet( false )
            , fProximitySucceeded( false )
        {
        }
    };

    typedef std::map< std::wstring, TRANSMITTER_INFO > TransmittersMap;

    TransmittersMap m_knownTransmitters;
    TransmittersMap::iterator m_itCurrentTransmitter;

    PRNDUPnPTransmitterControl *m_pTransmitterControl;
    PRNDReceiverSession *m_pReceiverSession;

    void _StartCommand();
    void _EndCommand( __in HRESULT drIn, __in_opt LPCWSTR pszLastContext );
    HRESULT _ExpectTransmitterIsSelected();
    HRESULT _EnsureTransmitterControlIsSet();
    HRESULT _EnsurePrndReceiverSessionIsSet();
    HRESULT _SendPrndMessage( __in CSocket *pSocket, __in const CPRNDMemHolder &message );
    HRESULT _ReceivePrndMessage( __in CSocket *pSocket, __inout CPRNDMemHolder &message );
    void _OnTransmitterChange();
};

HRESULT PRNDReceiverApp::Initialize()
{
    _StartCommand();
    
    HRESULT     dr                 = S_OK;
    DRM_WCHAR   szLastTx[MAX_PATH] = { 0 };
    DRM_DWORD   cbMaxPathRead      = 0;
    LPCWSTR     pszLastContext     = nullptr;
    OEM_FILEHDL hLastTxFile        = OEM_INVALID_HANDLE_VALUE;
    
    pszLastContext = L"Getting the paths to the PlayReady stores";
    ChkDR( GetPlayReadyStorePaths(
                m_strPlayReadyStorePath,
                m_strPlayReadyRevocationStorePackage,
                m_strLastTxFile
                ) );

    pszLastContext = L"Loading the current Transmitter from the config file";
    hLastTxFile = Oem_File_Open( NULL, ( DRM_WCHAR * ) m_strLastTxFile.c_str(), OEM_GENERIC_READ, OEM_FILE_SHARE_READ, OEM_OPEN_EXISTING, OEM_ATTRIBUTE_NORMAL );

    if( hLastTxFile != OEM_INVALID_HANDLE_VALUE )
    {
        ChkBOOL( Oem_File_Read( hLastTxFile, ( DRM_VOID * ) szLastTx, MAX_PATH * SIZEOF( DRM_WCHAR ), &cbMaxPathRead ), DRM_E_FAIL );
        ChkBOOL( Oem_File_Close( hLastTxFile ), DRM_E_FAIL );

        hLastTxFile = OEM_INVALID_HANDLE_VALUE;
        SetTransmitterUDN( ( LPCWSTR ) szLastTx );
    }

ErrorExit:

    if( OEM_INVALID_HANDLE_VALUE != hLastTxFile )
    {
        Oem_File_Close( hLastTxFile );
    }

    _EndCommand( dr, pszLastContext );

    return dr;
}

HRESULT PRNDReceiverApp::SetTransmitterUDN( __in LPCWSTR pszUDN )
{
    _StartCommand();

    HRESULT     dr          = S_OK;
    OEM_FILEHDL hLastTxFile = OEM_INVALID_HANDLE_VALUE;

    AssertChkArg( pszUDN != nullptr );

    m_itCurrentTransmitter = m_knownTransmitters.find( pszUDN );
    if( m_itCurrentTransmitter == m_knownTransmitters.end() )
    {
        m_itCurrentTransmitter =
                m_knownTransmitters.insert(
                                        std::pair<std::wstring,TRANSMITTER_INFO>( pszUDN, TRANSMITTER_INFO() )
                                        ).first;
    }

    hLastTxFile = Oem_File_Open( NULL, ( DRM_WCHAR * ) m_strLastTxFile.c_str(), OEM_GENERIC_WRITE, OEM_FILE_SHARE_NONE, OEM_CREATE_ALWAYS, OEM_ATTRIBUTE_NORMAL );
    if( hLastTxFile != OEM_INVALID_HANDLE_VALUE )
    {
        DRM_DWORD cbWritten = 0;
        ChkBOOL( Oem_File_Write( hLastTxFile, ( DRM_VOID * ) m_itCurrentTransmitter->first.c_str(), ( DRM_DWORD ) m_itCurrentTransmitter->first.length() * SIZEOF( wchar_t ), &cbWritten ), DRM_E_FAIL );
        ChkBOOL( Oem_File_Close( hLastTxFile ), DRM_E_FAIL );

        hLastTxFile = OEM_INVALID_HANDLE_VALUE;
    }
    _OnTransmitterChange();

ErrorExit:

    if( OEM_INVALID_HANDLE_VALUE != hLastTxFile )
    {
        Oem_File_Close( hLastTxFile );
    }
    _EndCommand( dr, L"Setting the current Transmitter UDN" );
    return dr;
}

void PRNDReceiverApp::ReportStatus()
{
    if( m_itCurrentTransmitter == m_knownTransmitters.end() )
    {
        wprintf_s(
                L"Transmitter: <none>\r\n"
                );
    }
    else
    {
        wprintf_s(
                L"Transmitter: %ls\r\n", m_itCurrentTransmitter->first.c_str()
                );

        bool fSessionIsValid = !m_itCurrentTransmitter->second.sessionInfo.IsEmpty();

        if( fSessionIsValid )
        {
            DRMFILETIME currentTime;

            Oem_Clock_GetSystemTimeAsFileTime( NULL, &currentTime );

            DRM_INT64 secondsUntilExpiration = m_itCurrentTransmitter->second.sessionInfo.GetSecondsUntilExpiration( currentTime );

            bool fSessionIsCurrent = !( DRM_I64Les( secondsUntilExpiration, DRM_I64( 0 ) ) || DRM_I64Eql( secondsUntilExpiration, DRM_I64( 0 ) ) );

            DRMIDString strId( m_itCurrentTransmitter->second.sessionInfo.SessionID() );

            if( !fSessionIsCurrent )
            {
                secondsUntilExpiration = DRM_I64Mul( secondsUntilExpiration, DRM_I64( -1 ) );
            }

            wprintf_s(
                    L"Session ID : %ls\r\n"
                    L"Status     : %ls %lld:%02lld:%02lld %ls\r\n"
                    L"Proximity  : %ls\r\n",
                    strId.Value(),
                    fSessionIsCurrent ? L"Current, expiring in" : L"Stale, expired",
                    DRM_I64Div( secondsUntilExpiration, DRM_I64( 3600 ) ),
                    DRM_I64Mod( DRM_I64Div( secondsUntilExpiration, DRM_I64( 60 ) ), DRM_I64( 60 ) ),
                    DRM_I64Mod( secondsUntilExpiration, DRM_I64( 60 ) ),
                    fSessionIsCurrent ? L"hours" : L"hours ago",
                    m_itCurrentTransmitter->second.fProximityChannelsSet ? L"Done" : L"Required"
                    );
        }
        else
        {
            wprintf_s(
                    L"Session ID : <none>\r\n" );
        }
    }
}


void PRNDReceiverApp::ExecuteRegistration()
{
    _StartCommand();

    HRESULT                 dr             = S_OK;
    LPCWSTR                 pszLastContext = nullptr;
    CPRNDMemHolder          registrationRequest;
    DRM_ID                  drmSessionID;
    SafeArrayAccessor<BYTE> registrationResponse;
    DRMFILETIME             currentTime;
    bool                    fSessionIsValid = false;

    ChkDR( _ExpectTransmitterIsSelected() );

    //
    // Check the current PRND Session state
    //

    fSessionIsValid = !m_itCurrentTransmitter->second.sessionInfo.IsEmpty();

    if( fSessionIsValid )
    {
        wprintf_s(
            L"Trying renew of Session %ls\r\n",
            DRMIDString( m_itCurrentTransmitter->second.sessionInfo.SessionID() ).Value()
            );
    }
    else
    {
        wprintf_s(
            L"New registration\r\n"
            );
    }

    //
    // Execute registration
    //

    ChkDR( _EnsureTransmitterControlIsSet() );
    ChkDR( _EnsurePrndReceiverSessionIsSet() );

    pszLastContext = L"Generating PRND registration request";
    ChkDR( m_pReceiverSession->GenerateRegistrationRequest( registrationRequest ) );

    pszLastContext = L"Invoking UPnP RegisterDevice action";
    ChkDR( m_pTransmitterControl->RegisterDevice(
                                    m_pReceiverSession->SessionID(),
                                    0,
                                    L"",
                                    registrationRequest.Data(),
                                    registrationRequest.Size(),
                                    &drmSessionID,
                                    &registrationResponse ) );

    pszLastContext = L"Processing RegisterDevice response";
    ChkDR( m_pReceiverSession->ProcessRegistrationResponse(
                                    registrationResponse.Elements(),
                                    registrationResponse.Size() ) );

    wprintf_s(
        L"Registration successful\r\n"
        L"Transmitter:  %ls\r\n"
        L"Session:      %ls\r\n",
        m_itCurrentTransmitter->first.c_str(),
        DRMIDString( m_pReceiverSession->SessionID() ).Value()
        );

    Oem_Clock_GetSystemTimeAsFileTime( NULL, &currentTime );
    m_itCurrentTransmitter->second.sessionInfo.SetSessionID( m_pReceiverSession->SessionID(), currentTime );

ErrorExit:

    _EndCommand( dr, pszLastContext );
}

void PRNDReceiverApp::ExecuteProximityDetection()
{
    HRESULT        dr             = S_OK;
    LPCWSTR        pszLastContext = nullptr;
    CPRNDMemHolder proximityStart;
    CPRNDMemHolder proximityResponse;
    CPRNDMemHolder proximityResult;
    CSocket       *pSocket        = nullptr;
    std::string    hostName;

    ChkDR( _ExpectTransmitterIsSelected() );

    pszLastContext = L"Expecting proximity params for current Transmitter";
    ChkBOOL( m_itCurrentTransmitter->second.fProximityChannelsSet, E_NOT_VALID_STATE );

    //
    // Connect the socket to do proximity
    //
    pszLastContext = L"Connecting to the Transmitter's socket";
    ChkMem( pSocket = new ( std::nothrow ) CSocket( SOCK_STREAM ) );
    hostName.assign(
                m_itCurrentTransmitter->second.strProximityHostName.begin(),
                m_itCurrentTransmitter->second.strProximityHostName.end() );

    ChkDR( pSocket->Connect(
                        hostName.c_str(), 
                        m_itCurrentTransmitter->second.proximityPort ) );

    //
    // Generate the proximity start
    //
    pszLastContext = L"Generating proximity message";
    ChkDR( _EnsurePrndReceiverSessionIsSet() );
    ChkDR( m_pReceiverSession->GenerateProximityStart( proximityStart ) );

    //
    // Send to Transmitter
    //
    pszLastContext = L"Sending proximity start";
    ChkDR( _SendPrndMessage( pSocket, proximityStart ) );

    pszLastContext = L"Receiving proximity start reponse";
    ChkDR( _ReceivePrndMessage( pSocket, proximityResult ) );

    //
    // Process challenge
    //

    pszLastContext = L"Processing the proximity challenge";
    dr = m_pReceiverSession->ProcessProximityChallenge(
                                 proximityResult.Data(),
                                 proximityResult.Size(),
                                 proximityResponse );

    if( dr != DRM_E_PRND_MESSAGE_WRONG_TYPE )
    {
        //
        // The result is a challenge.
        // Sent the reponse to the Transmitter
        //
        ChkDR( dr );

        pszLastContext = L"Sending proximity challenge reponse";
        ChkDR( _SendPrndMessage( pSocket, proximityResponse ) );

        pszLastContext = L"Receiving proximity result";
        ChkDR( _ReceivePrndMessage( pSocket, proximityResult ) );
    }

    //
    // Process the result
    //
    ChkDR( m_pReceiverSession->ProcessProximityResult(
                                   proximityResult.Data(),
                                   proximityResult.Size() ) );

    wprintf_s( L"Proximity succeeded\r\n" );

ErrorExit:

    if( m_itCurrentTransmitter != m_knownTransmitters.end() )
    {
        m_itCurrentTransmitter->second.fProximitySucceeded = SUCCEEDED( dr );
    }

    if( pSocket != nullptr )
    {
        pSocket->Close();
        delete pSocket;
    }

    _EndCommand( dr, pszLastContext );
}

void PRNDReceiverApp::ExecuteFetchLicense()
{
    _StartCommand();

    HRESULT                 dr             = S_OK;
    LPCWSTR                 pszLastContext = nullptr;
    DRM_KID                 contentID;
    CPRNDMemHolder          fetchLicenseMessage;
    SafeArrayAccessor<BYTE> licenseResponse;
    DRM_ID                  customDataTypeID;
    CPRNDMemHolder          customData;

    ChkDR( _ExpectTransmitterIsSelected() );

    //
    // Execute registration
    //
    ChkDR( _EnsureTransmitterControlIsSet() );
    ChkDR( _EnsurePrndReceiverSessionIsSet() );

    ChkDR( Oem_Random_GetBytes( nullptr, contentID.rgb, sizeof( contentID.rgb ) ) );

    pszLastContext = L"Generating fetch license request";
    ChkDR( m_pReceiverSession->GenerateFetchLicense( DRM_ACTION_PLAY, contentID, fetchLicenseMessage ) );

    pszLastContext = L"Invoking UPnP FetchLicense action";
    ChkDR( m_pTransmitterControl->FetchLicense(
                                    m_pReceiverSession->SessionID(),
                                    fetchLicenseMessage.Data(),
                                    fetchLicenseMessage.Size(),
                                    &licenseResponse ) );

    pszLastContext = L"Processing FetchLicense response";
    ChkDR( m_pReceiverSession->ProcessLicenseResponse(
                                    licenseResponse.Elements(),
                                    licenseResponse.Size(),
                                    customDataTypeID,
                                    customData ) );

    wprintf_s(
            L"License Information Received\r\n"
            L"Custom Data Type: %ls\r\n"
            L"-----------------------\r\n"
            L"%ls\r\n"
            L"-----------------------\r\n",
            DRMIDString( customDataTypeID ).Value(),
            std::wstring( customData.Data(), customData.Data() + customData.Size() ).c_str()
            );

ErrorExit:

    _EndCommand( dr, pszLastContext );
}

void PRNDReceiverApp::ResetPrndSession()
{
    delete m_pReceiverSession;
    m_pReceiverSession = nullptr;

    if( m_itCurrentTransmitter != m_knownTransmitters.end() )
    {
        m_itCurrentTransmitter->second.sessionInfo.Clear();
    }
}

void PRNDReceiverApp::SetProximityChannel( __in LPCWSTR pszHost, __in int nPort )
{
    _StartCommand();

    HRESULT dr             = S_OK;
    LPCWSTR pszLastContext = nullptr;

    ChkDR( _ExpectTransmitterIsSelected() );

    m_itCurrentTransmitter->second.proximityPort = nPort;
    m_itCurrentTransmitter->second.strProximityHostName = pszHost;
    m_itCurrentTransmitter->second.fProximityChannelsSet = true;

ErrorExit:

    _EndCommand( dr, pszLastContext );
}


void PRNDReceiverApp::ListPrndTransmitters()
{
    _StartCommand();

    HRESULT                    dr             = S_OK;
    LPCWSTR                    pszTaskContext = nullptr;
    CComPtr<IUPnPDeviceFinder> spFinder;
    UPnPDeviceEnum             deviceEnum;

    pszTaskContext = L"Instantiating a UPnPDeviceFinder";
    ChkDR( spFinder.CoCreateInstance( __uuidof( UPnPDeviceFinder ), NULL, CLSCTX_SERVER ) );

    pszTaskContext = L"Enumerating for PRND Test Transmitters";
    ChkDR( deviceEnum.StartFor( PRND_TRANSMITTER_UPNP_DEVICE_TYPE, spFinder ) );

    while( ( dr = deviceEnum.MoveNext() ) == S_OK )
    {
        CComBSTR bstrUdn;
        CComBSTR bstrDeviceFriendlyName;

        ChkDR( deviceEnum.PCurrent()->get_UniqueDeviceName( &bstrUdn ) );
        ChkDR( deviceEnum.PCurrent()->get_FriendlyName( &bstrDeviceFriendlyName ) );

        wprintf_s(
                L"Name :   %ls\r\n"
                L"UDN  :   %ls\r\n"
                L"\r\n",
                bstrDeviceFriendlyName,
                bstrUdn
                );
    }

    ChkDR( dr );

ErrorExit:

    _EndCommand( dr, pszTaskContext );
}


void PRNDReceiverApp::_StartCommand()
{
    m_fErrorReportedForCommand = false;
}

void PRNDReceiverApp::_EndCommand( __in HRESULT drIn, __in_opt LPCWSTR pszLastContext )
{
    if( FAILED( drIn ) && !m_fErrorReportedForCommand && ( pszLastContext != nullptr ) )
    {
        wprintf_s( L"Error 0x%X: %ls\r\n", drIn, pszLastContext );
        m_fErrorReportedForCommand = true;
    }
}

HRESULT PRNDReceiverApp::_ExpectTransmitterIsSelected()
{
    _StartCommand();

    HRESULT dr = S_OK;

    ChkBOOL( m_itCurrentTransmitter != m_knownTransmitters.end(), E_NOT_VALID_STATE );

ErrorExit:

    _EndCommand( dr, L"Expecting a selected Transmitter for the operation" );

    return dr;
}

HRESULT PRNDReceiverApp::_EnsureTransmitterControlIsSet()
{
    _StartCommand();

    HRESULT                    dr             = S_OK;
    LPCWSTR                    pszLastContext = nullptr;
    CComPtr<IUPnPDeviceFinder> spFinder;
    CComPtr<IUPnPDevice>       spTxDevice;
    UPnPServiceEnum            enumServices;

    if( m_pTransmitterControl != nullptr )
    {
        dr = S_OK;
        goto ErrorExit;
    }

    ChkDR( _ExpectTransmitterIsSelected() );

    ChkMem( m_pTransmitterControl = new ( std::nothrow ) PRNDUPnPTransmitterControl() );

    //
    // Find the device
    //
    pszLastContext = L"Creating UPnPDeviceFinder";
    ChkDR( spFinder.CoCreateInstance( __uuidof( UPnPDeviceFinder ), NULL, CLSCTX_SERVER ) );

    pszLastContext = L"Looking for Transmitter by UPnP UDN";
    ChkDR( spFinder->FindByUDN( CComBSTR( m_itCurrentTransmitter->first.c_str() ), &spTxDevice ) );

    ChkBOOL( spTxDevice != nullptr, HRESULT_FROM_WIN32( ERROR_NOT_FOUND ) );

    //
    // Find the service
    //
    pszLastContext = L"Looking for PRND UPnP Service in Transmitter";
    ChkDR( enumServices.StartFor( spTxDevice ) );

    while( ( dr = enumServices.MoveNext() ) == S_OK )
    {
        CComBSTR bstrServiceTypeID;

        ChkDR( enumServices.PCurrent()->get_ServiceTypeIdentifier( &bstrServiceTypeID ) );

        if( wcscmp( bstrServiceTypeID, PRND_TRANSMITTER_UPNP_SERVICE_TYPE_ID ) == 0 )
        {
            break;
        }
    }

    ChkDR( dr );
    ChkBOOL( dr == S_OK, HRESULT_FROM_WIN32( ERROR_NOT_FOUND ) );

    //
    // Bind the device
    //
    pszLastContext = L"Binding Transmitter's PRND Service";
    ChkDR( m_pTransmitterControl->Bind( enumServices.PCurrent() ) );

ErrorExit:

    _EndCommand( dr, pszLastContext );

    return dr;
}

HRESULT PRNDReceiverApp::_EnsurePrndReceiverSessionIsSet()
{
    _StartCommand();

    HRESULT dr = S_OK;
    LPCWSTR pszLastContext = nullptr;

    if( m_pReceiverSession != nullptr )
    {
        dr = S_OK;
        goto ErrorExit;
    }

    ChkDR( _ExpectTransmitterIsSelected() );

    pszLastContext = L"Initializing a PRNDReceiverSession";
    ChkMem( m_pReceiverSession = new ( std::nothrow ) PRNDReceiverSession() );
    ChkDR( m_pReceiverSession->Open(
                                    m_itCurrentTransmitter->second.sessionInfo.SessionID(),
                                    m_strPlayReadyStorePath.c_str(),
                                    m_strPlayReadyRevocationStorePackage.c_str() ) );

ErrorExit:
    if( DRM_FAILED( dr ) 
     && nullptr != m_pReceiverSession )
    {
        delete m_pReceiverSession;
        m_pReceiverSession = nullptr;
    }
    _EndCommand( dr, pszLastContext );

    return dr;
}

#pragma pack( push, 1 )
    struct PRND_MESSAGE_HEADER
    {
        BYTE rgbFullMessageSize[4];
        BYTE rgbSessionID[sizeof( DRM_ID )];
    };
#pragma pack( pop )

HRESULT PRNDReceiverApp::_SendPrndMessage(
    __in       CSocket        *pSocket,
    __in const CPRNDMemHolder &message )
{
    HRESULT             dr    = S_OK;
    PRND_MESSAGE_HEADER hdr;
    ULONG               cbMsg = 0;
    DRM_ID              sessionID;

    AssertChkArg( pSocket != nullptr );

    ChkDR( _EnsureTransmitterControlIsSet() );

    sessionID = m_itCurrentTransmitter->second.sessionInfo.SessionID();
    cbMsg = sizeof( hdr.rgbSessionID ) + message.Size();
    memcpy_s( hdr.rgbSessionID, sizeof( hdr.rgbSessionID ), &sessionID, sizeof( sessionID ) );
    DWORD_TO_NETWORKBYTES( hdr.rgbFullMessageSize, 0, cbMsg );

    ChkDR( pSocket->Send( reinterpret_cast<const char*>( &hdr ), sizeof( hdr ) ) );
    ChkDR( pSocket->Send(
                    reinterpret_cast<const char*>( message.Data() ),
                    static_cast<int>( message.Size() )
                    ) );

ErrorExit:

    return dr;
}

HRESULT PRNDReceiverApp::_ReceivePrndMessage(
    __in    CSocket        *pSocket,
    __inout CPRNDMemHolder &message )
{
    HRESULT             dr         = S_OK;
    PRND_MESSAGE_HEADER hdr;
    int                 cbTransfer = 0;
    DWORD               cbMsg      = 0;

    AssertChkArg( pSocket != nullptr );

    //
    // Receive the header
    //
    cbTransfer = sizeof( hdr );

    ChkDR( pSocket->Recv( reinterpret_cast<char*>( &hdr ), &cbTransfer, 0 ) );
    ChkBOOL( cbTransfer == sizeof( hdr ), DRM_E_PRND_MESSAGE_INVALID );

    NETWORKBYTES_TO_DWORD( cbMsg, hdr.rgbFullMessageSize, 0 );
    ChkBOOL( sizeof( hdr.rgbSessionID ) <= cbMsg, DRM_E_PRND_MESSAGE_INVALID );

    cbMsg -= sizeof( hdr.rgbSessionID );
    ChkDR( message.Alloc( cbMsg ) );
    cbTransfer = ( int )message.Size();

    ChkDR( pSocket->Recv( reinterpret_cast<char*>( message.Data() ), &cbTransfer, 0 ) );
    ChkBOOL( cbTransfer == ( int )cbMsg, DRM_E_PRND_MESSAGE_INVALID );

ErrorExit:

    return dr;
}


void PRNDReceiverApp::_OnTransmitterChange()
{
    delete m_pTransmitterControl;
    m_pTransmitterControl = nullptr;

    delete m_pReceiverSession;
    m_pReceiverSession = nullptr;
}

////////////////////////////////////////////////////////////////////////////////
//
// Main
//
////////////////////////////////////////////////////////////////////////////////

#define COMMAND_EMPTY                   L""
#define COMMAND_HELP                    L"?"
#define COMMAND_EXIT                    L"exit"
#define COMMAND_FIND_TRANSMITTERS       L"findTransmitters"
#define COMMAND_STATUS                  L"status"
#define COMMAND_SELECT_TRANSMITTER      L"tx"
#define COMMAND_REGISTER                L"register"
#define COMMAND_PROXIMITY               L"proximity"
#define COMMAND_FETCH_LICENSE           L"fetchLicense"
#define COMMAND_RESET                   L"reset"
#define COMMAND_SET_PROXIMITY_CHANNEL   L"setProximity"

#define CMD_HELP_STR( _X_ )  L"\r\n  " _X_ L"\r\n"

//   |--------------------------------------------------------------------------------//
static const WCHAR HELP_TEXT[] =
    L"PRNDReceiver commands:\r\n"

    CMD_HELP_STR( COMMAND_HELP )
    L"      Shows this help information.\r\n"

    CMD_HELP_STR( COMMAND_EXIT )
    L"      Exits the PRNDReceiver app.\r\n"

    CMD_HELP_STR( COMMAND_FIND_TRANSMITTERS )
    L"      Lists UPnP devices in the network and flags the PRND Transmitters.\r\n"

    CMD_HELP_STR( COMMAND_STATUS )
    L"      Gives the current status of the PRNDReceiver.\r\n"

    CMD_HELP_STR( COMMAND_SELECT_TRANSMITTER L" <UniqueDeviceName>" )
    L"      Selects the current Transmitter.\r\n"
    L"      The selection is persistent until modified.\r\n"

    CMD_HELP_STR( COMMAND_REGISTER )
    L"      Registers the receiver with the current Transmitter.\r\n"
    L"      If the registration status is absent, attempts a full registration.\r\n"
    L"      If the registration is present, attempts a renewal.\r\n"

    CMD_HELP_STR( COMMAND_PROXIMITY )
    L"      Executes Proximity Detection with the current Transmitter.\r\n"

    CMD_HELP_STR( COMMAND_FETCH_LICENSE )
    L"      Fetches a Play License.\r\n"

    CMD_HELP_STR( COMMAND_RESET )
    L"      Resets the registration with the current Transmitter.\r\n"

    CMD_HELP_STR( COMMAND_SET_PROXIMITY_CHANNEL L" <host> <port>" )
    L"      Sets the proximity channel for the current transmitter.\r\n"

    L"\r\n";
//   |--------------------------------------------------------------------------------|



void __cdecl wmain(
                        int    argc,
    __in_ecount( argc ) WCHAR* argv[]
    )
{
    HRESULT         dr         = S_OK;
    ComContext      comContext;
    WCHAR           szInputCommandBuffer[1024];
    LPCWSTR         pszCommand = nullptr;
    LPCWSTR         pszParams  = nullptr;

    PRNDReceiverApp driver;

    ChkDR( driver.Initialize() );

    wprintf_s( L"PRNDReceiver - for a list of commands, enter '?'.\r\n" );

    while( true )
    {
        wprintf_s( L">" );

        ChkBOOL( _getws_s( szInputCommandBuffer ) != nullptr, E_UNEXPECTED );
        pszCommand = SkipSpaces( szInputCommandBuffer );

        if( StringIsCommand( pszCommand, COMMAND_HELP ) )
        {
            wprintf_s( HELP_TEXT );
            continue;
        }

        if( StringIsCommand( pszCommand, COMMAND_EXIT ) )
        {
            break;
        }

        if( StringIsCommand( pszCommand, COMMAND_FIND_TRANSMITTERS ) )
        {
            driver.ListPrndTransmitters();
            continue;
        }

        if( StringIsCommand( pszCommand, COMMAND_STATUS ) )
        {
            driver.ReportStatus();
            continue;
        }

        pszParams = SkipPrefix( pszCommand, COMMAND_SELECT_TRANSMITTER L" " );
        pszParams = SkipSpaces( pszParams );
        if( pszParams != nullptr )
        {
            std::wstring strTransmitterUDN;

            pszParams = SkipNonSpaces( pszParams, &strTransmitterUDN );
            if( !StringIsEmpty( pszParams ) || strTransmitterUDN.empty() )
            {
                wprintf_s( L"Error 0x%x: Expecting <UniqueDeviceName> after " COMMAND_SELECT_TRANSMITTER L"\r\n", E_INVALIDARG );
                continue;
            }

            driver.SetTransmitterUDN( strTransmitterUDN.c_str() );
            continue;
        }

        if( StringIsCommand( pszCommand, COMMAND_RESET ) )
        {
            driver.ResetPrndSession();
            continue;
        }

        if( StringIsCommand( pszCommand, COMMAND_REGISTER ) )
        {
            driver.ExecuteRegistration();
            continue;
        }

        if( StringIsCommand( pszCommand, COMMAND_PROXIMITY ) )
        {
            driver.ExecuteProximityDetection();
            continue;
        }

        if( StringIsCommand( pszCommand, COMMAND_FETCH_LICENSE ) )
        {
            driver.ExecuteFetchLicense();
            continue;
        }

        pszParams = SkipPrefix( pszCommand, COMMAND_SET_PROXIMITY_CHANNEL L" " );
        pszParams = SkipSpaces( pszParams );
        if( pszParams != nullptr )
        {
            std::wstring strHost;
            unsigned int nPort = 0;

            pszParams = SkipNonSpaces( pszParams, &strHost );
            pszParams = SkipSpaces( pszParams );
            pszParams = SkipUnsignedDecimalInt( pszParams, &nPort );

            if( !StringIsEmpty( pszParams ) || strHost.empty() )
            {
                wprintf_s( L"Error 0x%x: Expecting <host> and <port> after " COMMAND_SET_PROXIMITY_CHANNEL L"\r\n", E_INVALIDARG );
                continue;
            }

            driver.SetProximityChannel( strHost.c_str(), static_cast<int>( nPort ) );
            continue;
        }

        if( StringIsEmpty( pszCommand ) )
        {
            continue;
        }

        wprintf_s( L"Unknown command. Use '?' for a list of supported commands.\r\n" );
    }

ErrorExit:

    if( FAILED( dr ) )
    {
        wprintf_s( L"Exit with error: 0x%x\r\n", dr );
    }
}
