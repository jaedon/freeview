/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

/// <summary>
/// Utility classes to use with Automation ( i.e., VARIANT, SAFEARRAY, etc )
/// <summary>

#pragma once


/// <summary>
/// Auxiliary template class to unwrap and access a one-dimension array of
/// elements packed in a SAFEARRAY
/// </summary>

template< typename T >
class SafeArrayAccessor
{
public:

    SafeArrayAccessor()
        : m_psa( NULL )
        , m_pElements( NULL )
        , m_cElements( 0 )
        , m_fOwnsSafeArray( false )
    {
    }

    ~SafeArrayAccessor()
    {
        Clear();
    }

    const T* Elements() const
    {
        return m_pElements;
    }

    T* Elements()
    {
        return m_pElements;
    }

    ULONG Size() const
    {
        return m_cElements;
    }

    HRESULT Attach( __in_opt SAFEARRAY* psaSource );

    HRESULT CopyFrom( __in SAFEARRAY* psaSource );

    HRESULT CopyFrom(
                __in_ecount( cElements ) const T    *pElements,
                __in                           ULONG cElements );

    HRESULT AccessByReference( __in SAFEARRAY* psaSource );

    HRESULT Resize( __in ULONG cElements );

    SAFEARRAY* Detach();

    void Clear();

private:

    HRESULT _LockElements();

    void _UnlockElementsIfNecessary();

    bool _AcceptsTypeOf( __in SAFEARRAY *psaSource );

    static VARTYPE _VarType();

    static HRESULT _CopyWorker(
                __in_ecount( cElements ) const T    *pSrcElements,
                __out_ecount( cElements )      T    *pDstElements,
                __in                           ULONG cElements );

    // Prevent copy
    SafeArrayAccessor( const SafeArrayAccessor& );
    void operator = ( const SafeArrayAccessor& );

    // Attributes
    SAFEARRAY               *m_psa;
    __ecount( cElements ) T *m_pElements;
    ULONG                    m_cElements;
    bool                     m_fOwnsSafeArray;
};

/// <summary>
/// Conversion utilities
/// </summary>

struct VariantConverter
{
    static HRESULT FromGenericData(
                        __in_bcount( cbValue ) LPCVOID  pvValue,
                        __in                   ULONG    cbValue,
                        __out                  VARIANT *pOutVar );

    static HRESULT FromDrmID(
                        __in const DRM_ID  &value,
                        __out      VARIANT *pOutVar );

    static HRESULT ToGenericData(
                        __in const VARIANT                 *pInVar,
                        __inout    SafeArrayAccessor<BYTE> *pOutValue );

    static HRESULT ToDrmID(
                        __in const VARIANT *pInVar,
                        __out      DRM_ID  *pOutValue );

    static HRESULT MoveArrayToVariant(
                        __inout SafeArrayAccessor<BYTE> *pSource,
                        __out   VARIANT                 *pOutVar );
};

/// <summary>
/// Loads and holds a ITypeInfo from a type library.
/// A major difference between this class and ATL's CComTypeInfoHolder
/// is more flexibility in handling type libraries that are embedded
/// as resources in a module
/// </summary>

class TypeInfoHolder
{
public:

    HRESULT LoadFromModule(
                __in_opt   HMODULE hModule,
                __in const GUID   &guid );

    HRESULT GetTypeInfoCount(
                __out UINT *pcTInfo );

    HRESULT GetTypeInfo(
                __in  UINT        iTInfo, 
                __in  LCID        lcid, 
                __out ITypeInfo **ppTInfo );

    HRESULT GetIDsOfNames(
                __in                  REFIID    riid, 
                __in_ecount( cNames ) LPOLESTR *rgszNames, 
                __in_range( 0,16384 ) UINT      cNames,
                __in                  LCID      lcid, 
                __out                 DISPID   *rgdispid );

    HRESULT Invoke(
                __inout   IDispatch  *p, 
                __in      DISPID      dispidMember, 
                __in      REFIID      riid,
                __in      LCID        lcid, 
                __in      WORD        wFlags, 
                __in      DISPPARAMS *pdispparams, 
                __out_opt VARIANT    *pvarResult,
                __out_opt EXCEPINFO  *pexcepinfo, 
                __out_opt UINT       *puArgErr );

private:

    CComPtr<ITypeInfo> m_spTypeInfo;
};
