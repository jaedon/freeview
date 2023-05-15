/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#pragma once

/// <summary>
/// Utility classes to use Windows UPnP framework
/// <summary>

#include <upnphost.h>
#include <UPnP.h>

/// <summary>
/// Handles the registration of a object that exposes the IUPnPDeviceControl
/// interface. The object is unregistered automatically on destruction.
/// </summary>
class UPnPDeviceRegistrar
{
public:

    UPnPDeviceRegistrar()
    {
    }

    ~UPnPDeviceRegistrar()
    {
        Clear();
    }

    BSTR DeviceIdentifier() const
    {
        return m_deviceIdentifier;
    }

    BSTR UniqueDeviceName() const
    {
        return m_uniqueDeviceName;
    }

    /// <summary>
    /// Registers a running UPnP Device as a object exposing the
    /// IUPnPDeviceControl interface. The instance automatically
    /// unregister the device on destruction or if Clear is called.
    /// </summary>
    HRESULT RegisterRunningDevice(
            __in LPCWSTR             pszDeviceDescriptionXml,
            __in LPCWSTR             pszInitString,
            __in LPCWSTR             pszResourcePath,
            __in IUPnPDeviceControl *pDevice );

    /// <summary>
    /// Clears the instance. Unregisters the currently registered device
    /// if any
    /// </summary>
    void Clear();

private:

    CComPtr<IUPnPRegistrar>     m_spRegistrar;
    CComPtr<IUPnPDeviceControl> m_spRegisteredDeviceControl;
    CComBSTR                    m_deviceIdentifier;
    CComBSTR                    m_uniqueDeviceName;
};


/// <summary>
/// Enumerates the services of a UPnP device
/// </summary>
class UPnPServiceEnum
{
public:

    HRESULT StartFor( __in IUPnPDevice *pDevice );

    HRESULT MoveNext();

    IUPnPService* PCurrent()
    {
        return m_spCurrentService;
    }

private:

    CComPtr<IEnumUnknown> m_spServiceEnumUnknown;
    CComPtr<IUPnPService> m_spCurrentService;
};
