/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#pragma once
/// <summary>
/// Enumerator of UPnP Devices
/// </summary>
class UPnPDeviceEnum
{
public:

    UPnPDeviceEnum();

    ~UPnPDeviceEnum();

    HRESULT StartFor(
                __in PCWSTR             pszbstrTypeUri,
                __in IUPnPDeviceFinder *pFinder );

    HRESULT MoveNext();

    IUPnPDevice* PCurrent() { return m_spCurrentDevice; }

private:

    class XFinderCallback;

    CComPtr<XFinderCallback> m_spFindCallback;
    CComPtr<IUPnPDevice> m_spCurrentDevice;

    void _Clear();
};
