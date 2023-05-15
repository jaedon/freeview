/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdafx.h>
#include <PRND_UPnP_TxControl.h>
#include <PRND_UPnP_TxService.h>
#include <UPnPUtils.h>


PRNDUPnPTransmitterControl::PRNDUPnPTransmitterControl()
{
}

PRNDUPnPTransmitterControl::~PRNDUPnPTransmitterControl()
{
    Clear();
}

HRESULT PRNDUPnPTransmitterControl::Bind( __in IUPnPService *pService )
{
    HRESULT  dr = S_OK;
    CComBSTR serviceId;

    AssertChkArg( pService != nullptr );

    ChkBOOL( m_spService == nullptr, E_NOT_VALID_STATE );

    ChkDR( pService->get_ServiceTypeIdentifier( &serviceId ) );

    ChkBOOL( wcscmp( serviceId, PRND_TRANSMITTER_UPNP_SERVICE_TYPE_ID ) == 0, E_INVALIDARG );

    m_spService = pService;

ErrorExit:

    return dr;
}

void PRNDUPnPTransmitterControl::Clear()
{
    m_spService.Release();
}

HRESULT PRNDUPnPTransmitterControl::RegisterDevice(
                __in                                        const DRM_ID                  &drmOldSessionID,
                __in                                              long                     pushLicenseType,
                __in_opt                                          LPCWSTR                  pszPushLicenseAddress,
                __in_bcount( cbRegistrationRequestMessage ) const void                    *pRegistrationRequestMessage,
                __in                                              ULONG                    cbRegistrationRequestMessage,
                __out                                             DRM_ID                  *pdrmResponseSessionID,
                __inout                                           SafeArrayAccessor<BYTE> *pRegistrationResponseMessage )
{
    HRESULT     dr = S_OK;
                
    CComBSTR    actionId( L"RegisterDevice" );

    CComVariant varArguments;
    CComVariant varOutputs;
    CComVariant varRetVal;
    SafeArrayAccessor<VARIANT> argumentArray;
    SafeArrayAccessor<VARIANT> outputArray;

    AssertChkArg( pszPushLicenseAddress        != nullptr );
    AssertChkArg( pRegistrationRequestMessage  != nullptr );
    AssertChkArg( pdrmResponseSessionID        != nullptr );
    AssertChkArg( pRegistrationResponseMessage != nullptr );

    ChkBOOL( m_spService != nullptr, E_NOT_VALID_STATE );

    //
    // Convert the arguments to the IUPnPService invocation format
    //

    ChkDR( argumentArray.Resize( 4 ) );

    ChkDR( VariantConverter::FromDrmID( drmOldSessionID, &argumentArray.Elements()[0] ) );

    ChkDR( CComVariant( pushLicenseType, VT_I4 ).Detach( &argumentArray.Elements()[1] ) );

    ChkDR( CComVariant( pszPushLicenseAddress ).Detach( &argumentArray.Elements()[2] ) );

    ChkDR( VariantConverter::FromGenericData( pRegistrationRequestMessage, cbRegistrationRequestMessage, &argumentArray.Elements()[3] ) );

    ChkDR( _MoveArgumentsToVariant( &argumentArray, &varArguments ) );

    //
    // Invoke action
    //

    ChkDR( m_spService->InvokeAction( actionId, varArguments, &varOutputs, &varRetVal ) );

    //
    // Convert output to caller's friendly format
    //

    ChkBOOL( varRetVal.vt == VT_EMPTY, UPNP_E_ERROR_PROCESSING_RESPONSE );

    ChkBOOL( varOutputs.vt == ( VT_VARIANT | VT_ARRAY ), UPNP_E_ERROR_PROCESSING_RESPONSE );

    ChkDR( outputArray.AccessByReference( varOutputs.parray ) );

    ChkBOOL( outputArray.Size() == 2, UPNP_E_ERROR_PROCESSING_RESPONSE );

    ChkDR( VariantConverter::ToDrmID( &outputArray.Elements()[0], pdrmResponseSessionID ) );
    ChkDR( VariantConverter::ToGenericData( &outputArray.Elements()[1], pRegistrationResponseMessage ) );

ErrorExit:

    return dr;
}

HRESULT PRNDUPnPTransmitterControl::FetchLicense(
                __in                                   const DRM_ID                  &sessionId,
                __in_bcount( cbLicenseRequestMessage ) const void                    *pLicenseRequestMessage,
                __in                                         ULONG                    cbLicenseRequestMessage,
                __inout                                      SafeArrayAccessor<BYTE> *pLicenseResponseMessage )
{
    HRESULT  dr = S_OK;

    CComBSTR actionId( L"FetchLicense" );

    CComVariant varArguments;
    CComVariant varOutputs;
    CComVariant varRetVal;
    SafeArrayAccessor<VARIANT> argumentArray;
    SafeArrayAccessor<VARIANT> outputArray;

    AssertChkArg( pLicenseRequestMessage  != nullptr );
    AssertChkArg( pLicenseResponseMessage != nullptr );

    ChkBOOL( m_spService != nullptr, E_NOT_VALID_STATE );

    //
    // Convert the arguments to the IUPnPService invocation format
    //

    ChkDR( argumentArray.Resize( 2 ) );

    ChkDR( VariantConverter::FromDrmID( sessionId, &argumentArray.Elements()[0] ) );

    ChkDR( VariantConverter::FromGenericData( pLicenseRequestMessage, cbLicenseRequestMessage, &argumentArray.Elements()[1] ) );

    ChkDR( _MoveArgumentsToVariant( &argumentArray, &varArguments ) );

    //
    // Invoke action
    //

    ChkDR( m_spService->InvokeAction( actionId, varArguments, &varOutputs, &varRetVal ) );

    //
    // Convert output to caller's friendly format
    //

    ChkBOOL( varRetVal.vt == VT_EMPTY, UPNP_E_ERROR_PROCESSING_RESPONSE );

    ChkBOOL( varOutputs.vt == ( VT_VARIANT | VT_ARRAY ), UPNP_E_ERROR_PROCESSING_RESPONSE );

    ChkDR( outputArray.AccessByReference( varOutputs.parray ) );

    ChkBOOL( outputArray.Size() == 1, UPNP_E_ERROR_PROCESSING_RESPONSE );

    ChkDR( VariantConverter::ToGenericData( &outputArray.Elements()[0], pLicenseResponseMessage ) );

ErrorExit:

    return dr;

}

/*static*/ HRESULT PRNDUPnPTransmitterControl::_MoveArgumentsToVariant(
    __inout SafeArrayAccessor<VARIANT> *pArgumentsArray,
    __out   VARIANT                    *pVarOut )
{
    HRESULT dr = S_OK;

    AssertChkArg( pArgumentsArray != nullptr );
    AssertChkArg( pVarOut         != nullptr );
    AssertChkArg( pVarOut->vt     == VT_EMPTY );

    pVarOut->vt = ( VT_VARIANT | VT_ARRAY );
    pVarOut->parray = pArgumentsArray->Detach();

ErrorExit:

    return dr;
}
