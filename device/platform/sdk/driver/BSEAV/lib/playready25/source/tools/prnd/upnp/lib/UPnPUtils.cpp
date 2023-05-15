/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdafx.h>

#include <UPnPUtils.h>

#include <strsafe.h>


////////////////////////////////////////////////////////////////////////////////
//
// UPnPDeviceRegistrar implementation
//
////////////////////////////////////////////////////////////////////////////////

HRESULT UPnPDeviceRegistrar::RegisterRunningDevice(
    __in LPCWSTR             pszDeviceDescriptionXml,
    __in LPCWSTR             pszInitString,
    __in LPCWSTR             pszResourcePath,
    __in IUPnPDeviceControl *pDevice )
{
    HRESULT  dr = S_OK;
    CComBSTR bstrXml( pszDeviceDescriptionXml );
    CComBSTR bstrInitString( pszInitString );
    CComBSTR bstrResourcePath( pszResourcePath );

    AssertChkArg( pszDeviceDescriptionXml != nullptr );

    ChkBOOL( m_spRegistrar == nullptr, E_NOT_VALID_STATE );

    ChkDR( m_spRegistrar.CoCreateInstance( __uuidof( UPnPRegistrar ), nullptr, CLSCTX_LOCAL_SERVER ) );

    ChkDR( m_spRegistrar->RegisterRunningDevice( bstrXml, pDevice, bstrInitString, bstrResourcePath, 0, &m_deviceIdentifier ) );

    m_spRegisteredDeviceControl = pDevice;

    dr = m_spRegistrar->GetUniqueDeviceName( m_deviceIdentifier, CComBSTR( L"uuid:RootDevice" ), &m_uniqueDeviceName );
    ChkDR( dr );

ErrorExit:

    return dr;
}

void UPnPDeviceRegistrar::Clear()
{
    HRESULT dr = S_OK;

    if( m_spRegisteredDeviceControl != nullptr )
    {
        DRMASSERT( m_spRegistrar != nullptr );
        DRMASSERT( m_deviceIdentifier != nullptr );

        dr = m_spRegistrar->UnregisterDevice( m_deviceIdentifier, TRUE );
        DRMASSERT( SUCCEEDED( dr ) );
    }

    m_spRegisteredDeviceControl.Release();
    m_spRegistrar.Release();
    m_deviceIdentifier.Empty();
    m_uniqueDeviceName.Empty();
}

////////////////////////////////////////////////////////////////////////////////
//
// UPnPServiceEnum implementation
//
////////////////////////////////////////////////////////////////////////////////

HRESULT UPnPServiceEnum::StartFor( __in IUPnPDevice *pDevice )
{
    HRESULT                dr = S_OK;
    CComPtr<IUPnPServices> spServices;
    CComPtr<IUnknown>      spUnknownEnum;

    AssertChkArg( pDevice != nullptr );

    m_spServiceEnumUnknown.Release();
    m_spCurrentService.Release();

    ChkDR( pDevice->get_Services( &spServices ) );

    ChkDR( spServices->get__NewEnum( &spUnknownEnum ) );

    ChkDR( spUnknownEnum.QueryInterface( &m_spServiceEnumUnknown ) );

ErrorExit:

    return dr;
}

HRESULT UPnPServiceEnum::MoveNext()
{
    HRESULT dr = S_OK;

    CComPtr<IUnknown> spUnknwonService;
    ULONG             fetchCount = 0;

    ChkBOOL( m_spServiceEnumUnknown != nullptr, E_NOT_VALID_STATE );

    m_spCurrentService.Release();

    ChkDR( m_spServiceEnumUnknown->Next( 1, &spUnknwonService, &fetchCount ) );

    if( fetchCount == 0 )
    {
        // End of list
        DRMASSERT( spUnknwonService == nullptr );
        dr = S_FALSE;
        goto ErrorExit;
    }

    DRMASSERT( fetchCount == 1 );

    if( spUnknwonService != nullptr )
    {
        ChkDR( spUnknwonService.QueryInterface( &m_spCurrentService ) );
    }

ErrorExit:

    return dr;
}

