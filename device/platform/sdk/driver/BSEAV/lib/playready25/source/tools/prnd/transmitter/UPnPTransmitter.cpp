/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdafx.h>

#include <UPnPUtils.h>
#include <PRND_UPnP_TxServiceBaseImpl.h>

#include <UPnPTransmitter.h>
#include <HelperFunctions.h>
#include <FileLogger.h>
#include <PRNDSessionManager.h>


////////////////////////////////////////////////////////////////////////////////
//
// CUPnPComContext implementation
//
////////////////////////////////////////////////////////////////////////////////

CUPnPComContext::~CUPnPComContext()
{
    if( m_fComInitialized )
    {
        ::CoUninitialize();
        m_fComInitialized = false;
    }
}

HRESULT CUPnPComContext::Init()
{
    HRESULT dr = S_OK;

    ChkBOOL( !m_fComInitialized, E_NOT_VALID_STATE );

    ChkDR( ::CoInitializeEx( nullptr, COINIT_MULTITHREADED ) );

    m_fComInitialized = true;

    ChkDR( ::CoInitializeSecurity(
                nullptr,
                -1,
                nullptr,
                nullptr,
                RPC_C_AUTHN_LEVEL_DEFAULT,
                RPC_C_IMP_LEVEL_IMPERSONATE,
                nullptr,
                EOAC_SECURE_REFS,
                nullptr
                ) );

ErrorExit:

    return dr;
}

////////////////////////////////////////////////////////////////////////////////
//
// Local constants
//
////////////////////////////////////////////////////////////////////////////////

//
// UPnP Device Description
//

static const char UPNP_DEVICE_NAME_PLACEHOLDER[] = "@@NAME_PLACEHOLDER";

static const char UPNP_DEVICE_DESCRIPTION_TEMPLATE[] =
    "<?xml version=\"1.0\"?>\r\n"
    "<root xmlns=\"urn:schemas-upnp-org:device-1-0\">\r\n"
    "\r\n"
    "    <specVersion>\r\n"
    "        <major>1</major>\r\n"
    "        <minor>0</minor>\r\n"
    "    </specVersion>\r\n"
    "\r\n"
    "    <device>\r\n"
    "        <deviceType>urn:microsoft_com:device:PRNDTestTransmitter:1</deviceType>\r\n"
    "        <friendlyName>@@NAME_PLACEHOLDER</friendlyName>\r\n"
    "        <manufacturer>Microsoft</manufacturer>\r\n"
    "        <manufacturerURL>http://www.microsoft.com/</manufacturerURL>\r\n"
    "        <modelDescription>UPnP PRND Test Transmitter Device 1.0</modelDescription>\r\n"
    "        <modelName>PRNDTestTransmitter1.0</modelName>\r\n"
    "        <modelNumber>1-00-00</modelNumber>\r\n"
    "        <modelURL>http://www.microsoft.com/</modelURL>\r\n"
    "        <serialNumber>0000001</serialNumber>\r\n"
    "        <UDN>uuid:RootDevice</UDN>\r\n"
    "        <UPC>00000-00001</UPC>\r\n"
    "\r\n"
    "        <serviceList>\r\n"
    "            <service>\r\n"
    "                <serviceType>urn:microsoft_com:service:PRNDTransmitter:1</serviceType>\r\n"
    "                <serviceId>urn:microsoft_com:serviceId:PRNDTransmitter</serviceId>\r\n"
    "                <controlURL></controlURL>\r\n"
    "                <eventSubURL></eventSubURL>\r\n"
    "                <SCPDURL>PlayReady_UPnP_TransmitterServiceDescription.xml</SCPDURL>\r\n"
    "            </service>\r\n"
    "        </serviceList>\r\n"
    "\r\n"
    "        <presentationURL></presentationURL>\r\n"
    "    </device>\r\n"
    "\r\n"
    "</root>";


/// <summary>
/// Specializes PRNDUPnPTransmitterServiceBase for the PRNDTransmitter.
/// Basically the class takes the services offered by CPRNDSessionManager,
/// CPRNDSession and the message processors and exposes them as a UPnP
/// Service.
/// </summary>
class CPRNDUPnPTransmitterService
    : public PRNDUPnPTransmitterServiceBase
{
protected:

    virtual HRESULT ExecuteRegisterDevice(
        __in_opt   IUnknown                *punkRemoteEndpointInfo,
        __in const DRM_ID                  &drmOldSessionId,
        __in       long                     pushLicenseType,
        __in       LPCWSTR                  pszLicensePushAddress,
        __in const SafeArrayAccessor<BYTE> &registrationRequestMessage,
        __out      DRM_ID                  *pdrmResponseSessionId,
        __inout    SafeArrayAccessor<BYTE> *pRegistrationReponseMessage );

    virtual HRESULT ExecuteFetchLicense(
        __in_opt   IUnknown                *punkRemoteEndpointInfo,
        __in const DRM_ID                  &sessionId,
        __in const SafeArrayAccessor<BYTE> &licenseRequestMessage,
        __inout    SafeArrayAccessor<BYTE> *pLicenseResponseMessage );
};


HRESULT CPRNDUPnPTransmitterService::ExecuteRegisterDevice(
    __in_opt   IUnknown                *punkRemoteEndpointInfo,
    __in const DRM_ID                  &drmOldSessionId,
    __in       long                     pushLicenseType,
    __in       LPCWSTR                  pszLicensePushAddress,
    __in const SafeArrayAccessor<BYTE> &registrationRequestMessage,
    __out      DRM_ID                  *pdrmResponseSessionId,
    __inout    SafeArrayAccessor<BYTE> *pRegistrationReponseMessage )
{
    PRNDLogFunctionEntry();

    HRESULT dr = S_OK;

    CPRNDSessionManager *pSessionManager = nullptr;
    CPRNDSession        *pSession        = nullptr;
    CPRNDSession        *pNewSession     = nullptr;
    CPRNDMemHolder       message;
    CPRNDMemHolder       response;

    AssertChkArg( pdrmResponseSessionId != nullptr );
    AssertChkArg( pRegistrationReponseMessage != nullptr );

    CPRNDSession::LogSessionID( drmOldSessionId );

    ChkDR( message.CopyFrom( registrationRequestMessage.Elements(), registrationRequestMessage.Size() ) );

    ChkDR( CPRNDSessionManager::GetInstance( &pSessionManager ) );

    if( CPRNDSession::IsPRNDSessionIDValid( drmOldSessionId ) == S_OK )
    {
        dr = pSessionManager->GetPRNDSessionFromSessionID( drmOldSessionId, &pSession );
        if( SUCCEEDED( dr ) )
        {
            //
            // Known session ID. Use existing session object to process the message.
            //

            ChkDR( pSession->ProcessMessage( DRM_PRND_MESSAGE_TYPE_REGISTRATION_REQUEST, message, response ) );

            pSession->GetSessionID( *pdrmResponseSessionId );
        }
        else
        {
            //
            // Unknown session ID. Generate error.
            //

            ChkDR( CPRNDSession::TranslateErrorToResponse(
                                        DRM_E_PRND_SESSION_ID_INVALID,
                                        NULL,
                                        const_cast<DRM_ID&>( drmOldSessionId ),
                                        DRM_PRND_MESSAGE_TYPE_REGISTRATION_REQUEST,
                                        response ) );
        }
    }
    else
    {
        //
        // Empty session ID. Create a new session object to process the message
        // and once it gets a session ID assigned move its owership to the manager
        //

        ChkDR( CPRNDSession::CreateInstance( &pNewSession ) );

        ChkDR( pNewSession->ProcessMessage( DRM_PRND_MESSAGE_TYPE_REGISTRATION_REQUEST, message, response ) );

        pNewSession->GetSessionID( *pdrmResponseSessionId );

        ChkDR( pSessionManager->AdoptSession( pNewSession ) );

        pNewSession = NULL;  // adopted by manager
    }

    //
    // If there's a response, return it to the caller
    //

    if( response.Data() != nullptr )
    {
        ChkDR( pRegistrationReponseMessage->CopyFrom( response.Data(), response.Size() ) );
    }

ErrorExit:

    DELETE_OBJECT( pNewSession );   // delete if not adopted

    PRNDLogFunctionExitWithResult( dr );

    return dr;
}

HRESULT CPRNDUPnPTransmitterService::ExecuteFetchLicense(
    __in_opt   IUnknown                *punkRemoteEndpointInfo,
    __in const DRM_ID                  &sessionId,
    __in const SafeArrayAccessor<BYTE> &licenseRequestMessage,
    __inout    SafeArrayAccessor<BYTE> *pLicenseResponseMessage )
{
    PRNDLogFunctionEntry();

    HRESULT dr = S_OK;

    CPRNDSessionManager *pSessionManager = nullptr;
    CPRNDSession        *pSession        = nullptr;
    CPRNDMemHolder       message;
    CPRNDMemHolder       response;

    AssertChkArg( pLicenseResponseMessage != nullptr );

    CPRNDSession::LogSessionID( sessionId );

    ChkDR( message.CopyFrom( licenseRequestMessage.Elements(), licenseRequestMessage.Size() ) );

    ChkDR( CPRNDSessionManager::GetInstance( &pSessionManager ) );

    //
    // Route the message to a session
    //

    dr = pSessionManager->GetPRNDSessionFromSessionID( sessionId, &pSession );
    if( FAILED( dr ) )
    {
        ChkDR( CPRNDSession::TranslateErrorToResponse(
                                    DRM_E_PRND_SESSION_ID_INVALID,
                                    NULL,
                                    const_cast<DRM_ID&>( sessionId ),
                                    DRM_PRND_MESSAGE_TYPE_LICENSE_REQUEST,
                                    response ) );
    }
    else
    {
        ChkDR( pSession->ProcessMessage( DRM_PRND_MESSAGE_TYPE_LICENSE_REQUEST, message, response ) );
    }

    //
    // If there's a response, return it to the caller
    //

    if( response.Data() != nullptr )
    {
        ChkDR( pLicenseResponseMessage->CopyFrom( response.Data(), response.Size() ) );
    }

ErrorExit:

    PRNDLogFunctionExitWithResult( dr );

    return dr;
}


/// <summary>
/// Exposes the PRNDTransmitter as a UPnP Device that supports the
/// urn:microsoft_com:service:PRNDTransmitter:1 service type.
/// </summary>
class CPRNDUPnPTransmitterDevice
    : public CComObjectRootEx<CComMultiThreadModelNoCS>
    , public IUPnPDeviceControl
{
protected:

    BEGIN_COM_MAP( CPRNDUPnPTransmitterDevice )
        COM_INTERFACE_ENTRY( IUPnPDeviceControl )
    END_COM_MAP()

    IFACEMETHOD( Initialize )(
        __in BSTR bstrXMLDesc,
        __in BSTR bstrDeviceIdentifier,
        __in BSTR bstrInitString );

    IFACEMETHOD( GetServiceObject )(
        __in            BSTR        bstrUDN,
        __in            BSTR        bstrId,
        __deref_out_opt IDispatch **ppdispService );

private:

    CComPtr<CPRNDUPnPTransmitterService> m_spService;
};

IFACEMETHODIMP CPRNDUPnPTransmitterDevice::Initialize(
    __in BSTR bstrXMLDesc,
    __in BSTR bstrDeviceIdentifier,
    __in BSTR bstrInitString )
{
    UNREFERENCED_PARAMETER( bstrXMLDesc );
    UNREFERENCED_PARAMETER( bstrInitString );

    PRNDLogMessage( "UPnP Initialize Device, ID [%ls]", bstrDeviceIdentifier );
    return S_OK;
}

IFACEMETHODIMP CPRNDUPnPTransmitterDevice::GetServiceObject(
    __in            BSTR        bstrUDN,
    __in            BSTR        bstrId,
    __deref_out_opt IDispatch **ppdispService )
{
    PRNDLogFunctionEntry();

    HRESULT dr = S_OK;

    AssertChkArg( ppdispService != nullptr );

    *ppdispService = nullptr;

    PRNDLogMessage( "UPnP Get Service, UDN [%ls], ID [%ls]", bstrUDN, bstrId );

    if( wcscmp( bstrId, PRND_TRANSMITTER_UPNP_DEFAULT_SERVICE_ID ) == 0 )
    {
        if( m_spService == nullptr )
        {
            PRNDLogMessage( "UPnP Instantiate PRND Transmitter Service Object" );

            ChkMem( m_spService = new ( std::nothrow ) CComObjectNoLock<CPRNDUPnPTransmitterService>() );

            ChkDR( m_spService->LoadTypeInfo( nullptr ) );
        }

        ChkDR( m_spService.QueryInterface( ppdispService ) );
    }
    else
    {
        dr = E_FAIL;
        goto ErrorExit;
    }

ErrorExit:

    PRNDLogFunctionExitWithResult( dr );

    return dr;
}


/// <summary>
/// Implements the abstract class CUPnPTransmitter.
/// The split in two classes is to decouple consumers of
/// CUPnPTransmitter from the implementation details.
/// </summary>
class CUPnPTransmitterImpl
    : public CUPnPTransmitter
{
public:

    ~CUPnPTransmitterImpl()
    {
    }

    __override HRESULT Start(
                            __in const char *pszDeviceName,
                            __in const char *pszResourcePath );

private:

    CComPtr<CPRNDUPnPTransmitterDevice> m_spDeviceControl;
    UPnPDeviceRegistrar                 m_registrar;
};


HRESULT CUPnPTransmitterImpl::Start(
    __in const char *pszDeviceName,
    __in const char *pszResourcePath )

{
    PRNDLogFunctionEntry();

    HRESULT dr = S_OK;

    CComBSTR    bstrResourcePath;
    std::string strDeviceDescription = UPNP_DEVICE_DESCRIPTION_TEMPLATE;

    AssertChkArg( pszDeviceName != nullptr );

    ChkBOOL( m_spDeviceControl == nullptr, E_UNEXPECTED );

    //
    // Create the device control
    //

    m_spDeviceControl = new ( std::nothrow ) CComObjectNoLock<CPRNDUPnPTransmitterDevice>();
    ChkMem( m_spDeviceControl );

    //
    // Generate a device description with the device name
    //

    find_and_replace_substring(
                strDeviceDescription,
                UPNP_DEVICE_NAME_PLACEHOLDER,
                pszDeviceName );

    //
    // Compose resource path
    //

    ChkDR( bstrResourcePath.Append( pszResourcePath ) );
    ChkDR( bstrResourcePath.Append( L"\\" ) );

    //
    // Register the device
    //

    ChkDR( m_registrar.RegisterRunningDevice(
                            CComBSTR( strDeviceDescription.c_str() ),
                            CComBSTR( L"" ),
                            bstrResourcePath,
                            m_spDeviceControl ) );

ErrorExit:

    PRNDLogFunctionExitWithResult( dr );

    return dr;
}


CUPnPTransmitter::CUPnPTransmitter()
{
}

CUPnPTransmitter::~CUPnPTransmitter()
{
}

/*static*/ CUPnPTransmitter* CUPnPTransmitter::MakeInstance()
{
    return new ( std::nothrow ) CUPnPTransmitterImpl();
}


