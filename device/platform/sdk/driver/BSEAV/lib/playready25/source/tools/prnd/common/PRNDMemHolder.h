/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#pragma once

/// <summary>
/// Holds a block of memory allocated by Drm_Prnd_MemAlloc
/// and safely frees it on cleanup or destruction through
/// Drm_Prnd_MemFree.
/// </summary>
class CPRNDMemHolder
{
public:

    CPRNDMemHolder()
        : m_pbMem( 0 )
        , m_cbMem( 0 )
    {
    }

    ~CPRNDMemHolder()
    {
        Cleanup();
    }

    void Cleanup()
    {
        m_cbMem = 0;
        DRM_PRND_SAFEMEMFREE( m_pbMem );
    }

    DRM_DWORD Size() const
    {
        return m_cbMem;
    }

    const DRM_BYTE* Data() const
    {
        return m_pbMem;
    }

    DRM_BYTE* Data()
    {
        return m_pbMem;
    }

    void Attach( __in_bcount( cb ) DRM_BYTE *pb, __in DRM_DWORD cb )
    {
        Cleanup();

        m_pbMem = pb;
        m_cbMem = cb;
    }

    DRM_BYTE* Detach()
    {
        DRM_BYTE *pb = m_pbMem;

        m_pbMem = NULL;
        m_cbMem = 0;

        return pb;
    }

    HRESULT Alloc( __in DRM_DWORD cb )
    {
        Cleanup();

        m_pbMem = Drm_Prnd_MemAlloc( cb );

        if( m_pbMem == NULL )
        {
            return DRM_E_OUTOFMEMORY;
        }

        m_cbMem = cb;

        return DRM_SUCCESS;
    }

    HRESULT CopyFrom( __in_bcount( cb ) const void *pv, __in DWORD cb )
    {
        // Move any current memory to a temp object in case
        // the copy is reflexive
        HRESULT        dr = S_OK; 
        CPRNDMemHolder temp;

        temp.m_pbMem = m_pbMem;
        temp.m_cbMem = m_cbMem;

        m_pbMem = NULL;
        m_cbMem = 0;

        dr = Alloc( cb );
        if( SUCCEEDED( dr ) )
        {
            memcpy_s( m_pbMem, m_cbMem, pv, cb );
        }

        return dr;
    }

    void TakeFrom( __inout CPRNDMemHolder &that )
    {
        DRM_DWORD cb = that.Size();
        DRM_BYTE* pb = that.Detach();

        Attach( pb, cb );
    }

private:

    // Deny copy semantics
    CPRNDMemHolder( const CPRNDMemHolder& );
    void operator = ( const CPRNDMemHolder& );

    // Attributes
    __bcount( m_cbMem ) DRM_BYTE *m_pbMem;
    DRM_DWORD m_cbMem;
};
