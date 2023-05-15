/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdafx.h>
#include <PRND_UPnP_TxServiceBaseImpl.h>
#include <UPnPUtils.h>
#include <AutomationUtils.h>

IFACEMETHODIMP PRNDUPnPTransmitterServiceBase::get_A_ARG_TYPE_SessionID(
    __out SAFEARRAY * *pA_ARG_TYPE_SessionID )
{
    // Return an empty SessionID

    HRESULT dr = S_OK;
    SafeArrayAccessor<BYTE> data;

    static const DRM_ID emptySid = { 0 };

    AssertChkArg( pA_ARG_TYPE_SessionID != nullptr );

    *pA_ARG_TYPE_SessionID = nullptr;

    ChkDR( data.CopyFrom( emptySid.rgb, sizeof( emptySid.rgb ) ) );

    *pA_ARG_TYPE_SessionID = data.Detach();

ErrorExit:

    return dr;
}

IFACEMETHODIMP PRNDUPnPTransmitterServiceBase::get_A_ARG_TYPE_LicensePushType( 
    __out long *pA_ARG_TYPE_LicensePushType )
{
    HRESULT dr = S_OK;

    AssertChkArg( pA_ARG_TYPE_LicensePushType != nullptr );

    *pA_ARG_TYPE_LicensePushType = 0;

ErrorExit:

    return dr;
}

IFACEMETHODIMP PRNDUPnPTransmitterServiceBase::get_A_ARG_TYPE_LicensePushAddress(
    __out BSTR *pA_ARG_TYPE_LicensePushAddress )
{
    // Return the default push license address
    // ( an empty string )

    HRESULT dr = S_OK;

    AssertChkArg( pA_ARG_TYPE_LicensePushAddress != nullptr );

    *pA_ARG_TYPE_LicensePushAddress = CComBSTR( L"" ).Detach();

ErrorExit:

    return dr;
}


IFACEMETHODIMP PRNDUPnPTransmitterServiceBase::get_A_ARG_TYPE_GenericData(
    __out SAFEARRAY * *pA_ARG_TYPE_GenericData )
{
    // Return the default generic data array.
    // Unfortunately this must be a non-emtpy array because empty arrays
    // are converted in the UPnP Framework to null arrays and that triggers
    // an error at the Control Point's end, so we'll use a single element
    // array. The actual value in the element doesn't matter since this
    // property exists only for the sake of defining an argument type
    // used in the actions.

    HRESULT                 dr = S_OK;
    SafeArrayAccessor<BYTE> data;

    static const BYTE rgTemplate[] = { 0x00 };

    AssertChkArg( pA_ARG_TYPE_GenericData != nullptr );

    *pA_ARG_TYPE_GenericData = nullptr;

    ChkDR( data.CopyFrom( rgTemplate, ARRAYSIZE( rgTemplate ) ) );

    *pA_ARG_TYPE_GenericData = data.Detach();

ErrorExit:

    return dr;
}

IFACEMETHODIMP PRNDUPnPTransmitterServiceBase::RegisterDevice(
    __in_opt IUnknown *punkRemoteEndpointInfo,
    __in     VARIANT  *pOldSessionID,
    __in     long      LicensePushType,
    __in     BSTR      LicensePushAddress,
    __in     VARIANT  *pRegistrationRequestMessage,
    __inout  VARIANT  *pResponseSessionID,
    __inout  VARIANT  *pRegistrationResponseMessage )
{
    HRESULT                 dr = S_OK;
    DRM_ID                  drmOldSessionID;
    DRM_ID                  drmResponseSessionID;
    SafeArrayAccessor<BYTE> registrationRequestArray;
    SafeArrayAccessor<BYTE> registrationResponseArray;

    if( pResponseSessionID != nullptr )
    {
        ::VariantClear( pResponseSessionID );
    }

    if( pRegistrationResponseMessage != nullptr )
    {
        ::VariantClear( pRegistrationResponseMessage );
    }

    AssertChkArg( pOldSessionID                != nullptr );
    AssertChkArg( LicensePushAddress           != nullptr );
    AssertChkArg( pRegistrationRequestMessage  != nullptr );
    AssertChkArg( pResponseSessionID           != nullptr );
    AssertChkArg( pRegistrationResponseMessage != nullptr );

    //
    // Convert input to friendly format
    //

    ChkDR( VariantConverter::ToDrmID( pOldSessionID, &drmOldSessionID ) );

    ChkDR( VariantConverter::ToGenericData( pRegistrationRequestMessage, &registrationRequestArray ) );

    //
    // Invoke derived class
    //

    ChkDR( ExecuteRegisterDevice(
                punkRemoteEndpointInfo,
                drmOldSessionID,
                LicensePushType,
                LicensePushAddress,
                registrationRequestArray,
                &drmResponseSessionID,
                &registrationResponseArray ) );

    //
    // Convert result to interface format
    //

    ChkDR( VariantConverter::FromDrmID( drmResponseSessionID, pResponseSessionID ) );

    ChkDR( VariantConverter::MoveArrayToVariant( &registrationResponseArray, pRegistrationResponseMessage ) );

ErrorExit:

    return dr;
}

IFACEMETHODIMP PRNDUPnPTransmitterServiceBase::FetchLicense(
    __in_opt IUnknown *punkRemoteEndpointInfo,
    __in     VARIANT  *pSessionID,
    __in     VARIANT  *pLicenseRequestMessage,
    __inout  VARIANT  *pLicenseResponseMessage )
{
    HRESULT                 dr                        = S_OK;
    DRM_ID                  drmSessionID              = { 0 };
    SafeArrayAccessor<BYTE> licenseFetchRequestArray;
    SafeArrayAccessor<BYTE> licenseFetchResponseArray;

    if( pLicenseResponseMessage != nullptr )
    {
        ::VariantClear( pLicenseResponseMessage );
    }

    AssertChkArg( pSessionID != nullptr );
    AssertChkArg( pLicenseRequestMessage != nullptr );
    AssertChkArg( pLicenseResponseMessage != nullptr );

    //
    // Convert input to friendly format
    //

    ChkDR( VariantConverter::ToDrmID( pSessionID, &drmSessionID ) );

    ChkDR( VariantConverter::ToGenericData( pLicenseRequestMessage, &licenseFetchRequestArray ) );

    //
    // Invoke derived class
    //

    ChkDR( ExecuteFetchLicense(
                punkRemoteEndpointInfo,
                drmSessionID,
                licenseFetchRequestArray,
                &licenseFetchResponseArray ) );

    //
    // Convert result to interface format
    //

    ChkDR( VariantConverter::MoveArrayToVariant( &licenseFetchResponseArray, pLicenseResponseMessage ) );

ErrorExit:

    return dr;
}

HRESULT PRNDUPnPTransmitterServiceBase::ExecuteRegisterDevice(
    __in_opt       IUnknown                *punkRemoteEndpointInfo,
    __in     const DRM_ID                  &uuidOldSessionId,
    __in           long                     licensePushType,
    __in           LPCWSTR                  pszLicensePushAddress,
    __in     const SafeArrayAccessor<BYTE> &registrationRequestMessage,
    __out          DRM_ID                  *pResponseSessionId,
    __inout        SafeArrayAccessor<BYTE> *pRegistrationReponseMessage )
{
    // Implemented in the derived class
    return E_NOTIMPL;
}

HRESULT PRNDUPnPTransmitterServiceBase::ExecuteFetchLicense(
    __in_opt       IUnknown                *punkRemoteEndpointInfo,
    __in     const DRM_ID                  &drmSessionID,
    __in     const SafeArrayAccessor<BYTE> &licenseRequestMessage,
    __inout        SafeArrayAccessor<BYTE> *pLicenseResponseMessage )
{
    // Implemented in the derived class
    return E_NOTIMPL;
}
