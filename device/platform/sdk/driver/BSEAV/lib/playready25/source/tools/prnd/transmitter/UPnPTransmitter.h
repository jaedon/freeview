/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#pragma once


/// <summary>
/// Initializes COM context on demand for the UPnP framework
/// and holds the context until destruction
/// </summary>
class CUPnPComContext
{
public:

    CUPnPComContext()
        : m_fComInitialized( false )
    {
    }

    ~CUPnPComContext();

    HRESULT Init();

private:

    bool m_fComInitialized;
};

/// <summary>
/// Exposes the PRND functionality of the application
/// as a UPnP Device with a PRND service
/// </summary>
class CUPnPTransmitter
{
public:

    static CUPnPTransmitter* MakeInstance();

    virtual ~CUPnPTransmitter() = 0;

    virtual HRESULT Start(
                        __in const char *pszDeviceName,
                        __in const char *pszResourcePath ) = 0;

protected:

    CUPnPTransmitter();
};