/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdafx.h>

#include <deque>
#include <UPnPUtils.h>
#include <UPnPDeviceEnum.h>


/// <summary>
/// Private class used by UPnPDeviceEnum to hook up to the finder's callback.
/// </summary>
class UPnPDeviceEnum::XFinderCallback
    : public CComObjectRootEx<CComMultiThreadModel>
    , public IUPnPDeviceFinderCallback
{
public:

    /// <summary>
    /// Called by owner enumerator to start the search
    /// </summary>
    HRESULT InitOnce(
                __in BSTR               bstrTypeUri,
                __in IUPnPDeviceFinder *pFinder )
    {
        HRESULT dr = S_OK;

        DRMASSERT( m_hQueueUpdated == NULL );
        DRMASSERT( m_spFinder == NULL );
        DRMASSERT( pFinder != NULL );

        m_hQueueUpdated = CreateEvent( NULL, TRUE, FALSE, NULL );

        ChkDR( VerifyWin32( m_hQueueUpdated != NULL ) );

        ChkDR( pFinder->CreateAsyncFind( bstrTypeUri, 0, this, &m_lFindData ) );

        m_spFinder = pFinder;

        ChkDR( m_spFinder->StartAsyncFind( m_lFindData ) );

    ErrorExit:

        return dr;
    }

    /// <summary>
    /// Called by owner enumerator to dispose of the callback
    /// ( it cancels the search if one is pending )
    /// </summary>
    void Dispose()
    {
        CComPtr<IUPnPDeviceFinder> spFinder;

        {
            ObjectLock lock( this );
            spFinder.Attach( m_spFinder.Detach() );
        }

        if( spFinder != NULL )
        {
            spFinder->CancelAsyncFind( m_lFindData );
        }
    }

    /// <summary>
    /// Takes the next device from the queue ( may block waiting for updates )
    /// </summary>
    HRESULT GetNext( __out IUPnPDevice **ppDevice )
    {
        HRESULT dr = S_OK;

        DRMASSERT( ppDevice != NULL );
        *ppDevice = NULL;

        while( true )
        {
            ChkBOOL( WaitForSingleObject( m_hQueueUpdated, INFINITE ) == WAIT_OBJECT_0, E_UNEXPECTED );

            {
                ObjectLock lock( this );

                if( !m_deviceQueue.empty() )
                {
                    *ppDevice = m_deviceQueue.front().Detach();
                    m_deviceQueue.pop_front();
                    break;
                }

                if( m_fAsyncSearchEnded )
                {
                    dr = S_FALSE;
                    break;
                }

                ResetEvent( m_hQueueUpdated );
            }
        }

    ErrorExit:

        return dr;
    }

protected:

    BEGIN_COM_MAP( XFinderCallback )
        COM_INTERFACE_ENTRY( IUPnPDeviceFinderCallback )
    END_COM_MAP()

    XFinderCallback()
        : m_hQueueUpdated( NULL )
        , m_fAsyncSearchEnded( false )
    {
        _AtlInitialConstruct();
    }

    ~XFinderCallback()
    {
        if( m_hQueueUpdated != NULL )
        {
            CloseHandle( m_hQueueUpdated );
        }

        DRMASSERT( m_spFinder == NULL );
    }

    //
    // IUPnPDeviceFinderCallback
    //

    IFACEMETHOD( DeviceAdded )( 
        __in     LONG         lFindData,
        __in_opt IUPnPDevice *pDevice )
    {
        HRESULT dr = S_OK;

        if( pDevice != NULL )
        {
            ObjectLock lock( this );

            if( m_spFinder != NULL )
            {
                DRMASSERT( lFindData == m_lFindData );

                m_deviceQueue.push_back( pDevice );

                ChkDR( VerifyWin32( SetEvent( m_hQueueUpdated ) ) );
            }
        }

    ErrorExit:
        return dr;
    }

    IFACEMETHOD( DeviceRemoved )( 
        __in LONG lFindData,
        __in BSTR bstrUDN )
    {
        return S_OK;
    }

    IFACEMETHOD( SearchComplete )(
        __in LONG lFindData )
    {
        HRESULT dr = S_OK;

        ObjectLock lock( this );

        m_fAsyncSearchEnded = true;

        ChkDR( VerifyWin32( SetEvent( m_hQueueUpdated ) ) );

ErrorExit:

        return dr;
    }

private:

    HANDLE                           m_hQueueUpdated;
    std::deque<CComPtr<IUPnPDevice>> m_deviceQueue;
    CComPtr<IUPnPDeviceFinder>       m_spFinder;
    LONG                             m_lFindData;
    bool                             m_fAsyncSearchEnded;
};

////////////////////////////////////////////////////////////////////////////////

UPnPDeviceEnum::UPnPDeviceEnum()
{
}

UPnPDeviceEnum::~UPnPDeviceEnum()
{
    _Clear();
}

void UPnPDeviceEnum::_Clear()
{
    m_spCurrentDevice.Release();

    if( m_spFindCallback != NULL )
    {
        m_spFindCallback->Dispose();
        m_spFindCallback.Release();
    }
}

HRESULT UPnPDeviceEnum::StartFor(
    __in PCWSTR             pszbstrTypeUri,
    __in IUPnPDeviceFinder *pFinder )
{
    HRESULT dr = S_OK;

    DRMASSERT( pszbstrTypeUri != NULL );
    DRMASSERT( pFinder != NULL );

    CComBSTR bstrTypeUri( pszbstrTypeUri );

    _Clear();

    m_spFindCallback = new ( std::nothrow ) CComObjectNoLock<XFinderCallback>();
    ChkMem( m_spFindCallback );

    ChkDR( m_spFindCallback->InitOnce( bstrTypeUri, pFinder ) );

ErrorExit:
    return dr;
}

HRESULT UPnPDeviceEnum::MoveNext()
{
    HRESULT dr = S_OK;

    m_spCurrentDevice.Release();

    ChkBOOL( m_spFindCallback != NULL, E_UNEXPECTED );

    ChkDR( m_spFindCallback->GetNext( &m_spCurrentDevice ) );

ErrorExit:

    return dr;
}
