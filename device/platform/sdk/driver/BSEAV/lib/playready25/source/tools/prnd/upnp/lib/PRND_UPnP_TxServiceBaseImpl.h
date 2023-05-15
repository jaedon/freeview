/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#pragma once

#include <PRND_UPnP_TxService.h>
#include <AutomationUtils.h>

/// <summary>
/// Base class implementing the IUPnPService_PlayReadyTransmitterDual interface.
/// Basically this calls takes the IDispatch-like semantics of the interface
/// and adapts that to semantics more suitable to the actual implementation.
/// </summary>
class PRNDUPnPTransmitterServiceBase
    : public CComObjectRootEx<CComMultiThreadModelNoCS>
    , public IUPnPService_PlayReadyTransmitterDual
{
public:

    HRESULT LoadTypeInfo( __in_opt HMODULE hModule )
    {
        return m_typeInfoHolder.LoadFromModule( hModule, __uuidof( IUPnPService_PlayReadyTransmitterDual ) );
    }

protected:

    BEGIN_COM_MAP( PRNDUPnPTransmitterServiceBase )
        COM_INTERFACE_ENTRY( IDispatch )
        COM_INTERFACE_ENTRY( IUPnPService_PlayReadyTransmitterDual )
    END_COM_MAP()

    //
    // IDispatch
    //

    IFACEMETHOD( GetTypeInfoCount )( 
        __out UINT *pctinfo )
    {
        return m_typeInfoHolder.GetTypeInfoCount( pctinfo );
    }

    IFACEMETHOD( GetTypeInfo )( 
        __in            UINT        iTInfo,
        __in            LCID        lcid,
        __deref_out_opt ITypeInfo **ppTInfo )
    {
        return m_typeInfoHolder.GetTypeInfo( iTInfo, lcid, ppTInfo );
    }

    IFACEMETHOD( GetIDsOfNames )( 
        __in                        REFIID    riid,
        __in_ecount( cNames )       LPOLESTR *rgszNames,
        __in_range( 0,16384 )       UINT      cNames,
        __in                        LCID      lcid,
        __out_ecount_full( cNames ) DISPID   *rgDispId )
    {
        return m_typeInfoHolder.GetIDsOfNames( riid, rgszNames, cNames, lcid, rgDispId );
    }

    IFACEMETHOD( Invoke )( 
        __in       DISPID      dispIdMember,
        __in       REFIID      riid,
        __in       LCID        lcid,
        __in       WORD        wFlags,
        __in       DISPPARAMS *pDispParams,
        __out_opt  VARIANT    *pVarResult,
        __out_opt  EXCEPINFO  *pExcepInfo,
        __out_opt  UINT       *puArgErr )
    {
        return m_typeInfoHolder.Invoke( this, dispIdMember, riid, lcid, wFlags, pDispParams, pVarResult, pExcepInfo, puArgErr );
    }

    //
    // IUPnPService_PlayReadyTransmitterDual implementation
    //

    IFACEMETHOD( get_A_ARG_TYPE_SessionID )(
        __out SAFEARRAY * *pA_ARG_TYPE_SessionID );

    IFACEMETHOD( get_A_ARG_TYPE_LicensePushType )(
        __out long *pA_ARG_TYPE_LicensePushType );

    IFACEMETHOD( get_A_ARG_TYPE_LicensePushAddress )(
        __out BSTR *pA_ARG_TYPE_LicensePushAddress );

    IFACEMETHOD( get_A_ARG_TYPE_GenericData )(
        __out SAFEARRAY * *pA_ARG_TYPE_GenericData );

    IFACEMETHOD( RegisterDevice )(
        __in_opt IUnknown *punkRemoteEndpointInfo,
        __in     VARIANT  *pOldSessionID,
        __in     long      LicensePushType,
        __in     BSTR      LicensePushAddress,
        __in     VARIANT  *pRegistrationRequestMessage,
        __inout  VARIANT  *pResponseSessionID,
        __inout  VARIANT  *pRegistrationResponseMessage );

    IFACEMETHOD( FetchLicense )(
        __in    IUnknown *punkRemoteEndpointInfo,
        __in    VARIANT  *pSessionID,
        __in    VARIANT  *pLicenseRequestMessage,
        __inout VARIANT  *pLicenseResponseMessage );

    //
    // Overrideables in derived class to implement the service
    //

    virtual HRESULT ExecuteRegisterDevice(
        __in_opt   IUnknown                 *punkRemoteEndpointInfo,
        __in const DRM_ID                   &drmOldSessionId,
        __in       long                      pushLicenseType,
        __in       LPCWSTR                   pszLicensePushAddress,
        __in const SafeArrayAccessor<BYTE>  &registrationRequestMessage,
        __out      DRM_ID                   *pdrmResponseSessionId,
        __inout    SafeArrayAccessor<BYTE>  *pRegistrationReponseMessage );

    virtual HRESULT ExecuteFetchLicense(
        __in_opt   IUnknown                 *punkRemoteEndpointInfo,
        __in const DRM_ID                   &uuidSessionId,
        __in const SafeArrayAccessor<BYTE>  &licenseRequestMessage,
        __inout    SafeArrayAccessor<BYTE>  *pLicenseResponseMessage );

private:

    TypeInfoHolder m_typeInfoHolder;
};

