/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#pragma once

#include <UPnP.h>

#include <AutomationUtils.h>

/// <summary>
/// Auxiliary class to invoke actions in a UPnP PRNDTransmitter service
/// through the Windows UPnP Framework using stronger argument types
/// </summary>
class PRNDUPnPTransmitterControl
{
public:

    PRNDUPnPTransmitterControl();

    ~PRNDUPnPTransmitterControl();

    HRESULT Bind( __in IUPnPService *pService );

    void Clear();

    HRESULT RegisterDevice(
                __in                                        const DRM_ID                  &drmOldSessionID,
                __in                                              long                     pushLicenseType,
                __in_opt                                          LPCWSTR                  pszPushLicenseAddress,
                __in_bcount( cbRegistrationRequestMessage ) const void                    *pRegistrationRequestMessage,
                __in                                              ULONG                    cbRegistrationRequestMessage,
                __out                                             DRM_ID                  *pdrmResponseSessionID,
                __inout                                           SafeArrayAccessor<BYTE> *pRegistrationResponseMessage );

    HRESULT FetchLicense(
                __in                                   const DRM_ID                  &sessionId,
                __in_bcount( cbLicenseRequestMessage ) const void                    *pLicenseRequestMessage,
                __in                                         ULONG                    cbLicenseRequestMessage,
                __inout                                      SafeArrayAccessor<BYTE> *pLicenseResponseMessage );

private:

    PRNDUPnPTransmitterControl( const PRNDUPnPTransmitterControl& );
    void operator = ( const PRNDUPnPTransmitterControl& );

    static HRESULT _MoveArgumentsToVariant(
                __inout SafeArrayAccessor<VARIANT> *pArgumentsArray,
                __out   VARIANT                    *pVarOut );

    // Attributes
    CComPtr<IUPnPService> m_spService;
};
