/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdafx.h>

#include <AutomationUtils.h>


////////////////////////////////////////////////////////////////////////////////
//
// SafeArrayAccessor<T> implementation
//
// Note: OK to implement the template in the code file instead of a header
// because the template is instantiated explicitily for a limited set
// of types.
//
////////////////////////////////////////////////////////////////////////////////

template< typename T >
HRESULT SafeArrayAccessor<T>::Attach( __in_opt SAFEARRAY *psaSource )
{
    HRESULT dr = S_OK;

    if( psaSource != nullptr )
    {
        ChkBOOL( _AcceptsTypeOf( psaSource ), E_INVALIDARG );

        Clear();

        // Only take full owership after the lock succeeds,
        // otherwise the caller still owns the array

        m_psa = psaSource;

        ChkDR( _LockElements() );

        m_fOwnsSafeArray = true;
    }
    else
    {
        Clear();
    }

ErrorExit:

   if( FAILED( dr ) )
   {
       Clear();
   }

    return dr;
}

template< typename T >
HRESULT SafeArrayAccessor<T>::CopyFrom( __in SAFEARRAY *psaSource )
{
    HRESULT dr = S_OK;

    AssertChkArg( psaSource != nullptr );

    ChkBOOL( _AcceptsTypeOf( psaSource ), E_INVALIDARG );

    Clear();

    // Take ownership of the copy even if lock fails,
    // otherwise the copy will leak

    ChkDR( ::SafeArrayCopy( psaSource, &m_psa ) );

    m_fOwnsSafeArray = true;

    ChkDR( _LockElements() );

ErrorExit:

    return dr;
}

template< typename T >
HRESULT SafeArrayAccessor<T>::CopyFrom(
    __in_ecount( cElements ) const T    *pElements,
    __in                           ULONG cElements )
{
    HRESULT dr = S_OK;

    AssertChkArg( pElements != nullptr );

    ChkDR( Resize( cElements ) );

    ChkDR( _CopyWorker( pElements, m_pElements, cElements ) );

ErrorExit:

    return dr;
}


template< typename T >
HRESULT SafeArrayAccessor<T>::AccessByReference( __in SAFEARRAY *psaSource )
{
    HRESULT dr = S_OK;

    AssertChkArg( psaSource != nullptr );

    ChkBOOL( _AcceptsTypeOf( psaSource ), E_INVALIDARG );

    Clear();

    // Lock the elements but do not take ownership of the array

    m_psa = psaSource;
    m_fOwnsSafeArray = false;

    ChkDR( _LockElements() );

ErrorExit:

    return dr;
}

template< typename T >
HRESULT SafeArrayAccessor<T>::Resize( __in ULONG cElements )
{
    HRESULT dr = S_OK;

    SAFEARRAYBOUND bounds[1];

    bounds[0].lLbound = 1;
    bounds[0].cElements = cElements;

    if( m_psa == nullptr )
    {
        ChkMem( m_psa = ::SafeArrayCreate( _VarType(), ARRAYSIZE( bounds ), bounds ) );

        m_fOwnsSafeArray = true;

        ChkDR( _LockElements() );
    }
    else
    {
        _UnlockElementsIfNecessary();

        ChkDR( ::SafeArrayRedim( m_psa, bounds ) );

        ChkDR( _LockElements() );
    }

ErrorExit:

    return dr;
}


template< typename T >
SAFEARRAY* SafeArrayAccessor<T>::Detach()
{
    _UnlockElementsIfNecessary();

    SAFEARRAY* psaDetached = nullptr;

    psaDetached = m_psa;

    m_psa = nullptr;
    m_fOwnsSafeArray = false;

    return psaDetached;
}


template< typename T >
void SafeArrayAccessor<T>::Clear()
{
    _UnlockElementsIfNecessary();

    if( m_fOwnsSafeArray )
    {
        DRMASSERT( m_psa != NULL );

        ::SafeArrayDestroy( m_psa );

        m_fOwnsSafeArray = false;
    }

    m_psa = nullptr;
}


template< typename T >
HRESULT SafeArrayAccessor<T>::_LockElements()
{
    HRESULT dr = S_OK;
    LONG lowBound = 0;
    LONG upBound = 0;
    void* pvRawData = nullptr;

    // Must be called only when the accessor has
    // a safearray, but hasn't locked the
    // elements yet
    DRMASSERT( m_psa != nullptr );
    DRMASSERT( m_pElements == nullptr );
    DRMASSERT( m_cElements == 0 );

    // Check the bounds
    ChkDR( ::SafeArrayGetLBound( m_psa, 1, &lowBound ) );
    ChkDR( ::SafeArrayGetUBound( m_psa, 1, &upBound ) );
    ChkBOOL( ( 0 <= lowBound ) && ( lowBound <= upBound ) && ( upBound < MAXLONG ), E_INVALIDARG );

    // Lock the array's data
    ChkDR( ::SafeArrayAccessData( m_psa, &pvRawData ) );

    m_pElements = reinterpret_cast<T*>( pvRawData );
    m_cElements = static_cast<ULONG>( upBound - lowBound + 1 );

ErrorExit:

    return dr;
}

template< typename T >
void SafeArrayAccessor<T>::_UnlockElementsIfNecessary()
{
    if( m_pElements != nullptr )
    {
        DRMASSERT( m_psa != nullptr );

        ::SafeArrayUnaccessData( m_psa );

        m_pElements = nullptr;
        m_cElements = 0;
    }
}

template< typename T >
bool SafeArrayAccessor<T>::_AcceptsTypeOf( __in SAFEARRAY *psaSource )
{
    VARTYPE vt = VT_EMPTY;

    return(
        SUCCEEDED( ::SafeArrayGetVartype( psaSource, &vt ) )
        && ( vt == _VarType() )
        && ( 1 == ::SafeArrayGetDim( psaSource ) )
        );
}

//
// Explicit instantiation of SafeArrayAccessor<BYTE>
//

template<>
/*static*/ VARTYPE SafeArrayAccessor<BYTE>::_VarType()
{
    return VT_UI1;
}

template<>
/*static*/ HRESULT SafeArrayAccessor<BYTE>::_CopyWorker(
    __in_ecount( cElements ) const BYTE *pSrcElements,
    __out_ecount( cElements )      BYTE *pDstElements,
    __in                           ULONG cElements )
{
    memcpy_s( pDstElements, cElements, pSrcElements, cElements );
    return S_OK;
}

template class SafeArrayAccessor<BYTE>;

//
// Explicit instantiation of SafeArrayAccessor<VARIANT>
//

template<>
/*static*/ VARTYPE SafeArrayAccessor<VARIANT>::_VarType()
{
    return VT_VARIANT;
}

template<>
/*static*/ HRESULT SafeArrayAccessor<VARIANT>::_CopyWorker(
    __in_ecount( cElements ) const VARIANT *pSrcElements,
    __out_ecount( cElements )      VARIANT *pDstElements,
    __in                           ULONG    cElements )
{
    DRMASSERT( FALSE );
    return E_NOTIMPL;
}

template class SafeArrayAccessor<VARIANT>;

////////////////////////////////////////////////////////////////////////////////
//
// Converter implementation
//
////////////////////////////////////////////////////////////////////////////////

/*static*/ HRESULT VariantConverter::FromGenericData(
    __in_bcount( cbValue ) LPCVOID  pvValue,
    __in                   ULONG    cbValue,
    __out                  VARIANT *pOutVar )
{
    HRESULT dr = S_OK;
    SafeArrayAccessor<BYTE> maker;

    AssertChkArg( pvValue != nullptr );
    AssertChkArg( pOutVar != nullptr );
    AssertChkArg( pOutVar->vt == VT_EMPTY );

    //
    // Copy the data to a SAFEARRAY( BYTE )
    //

    ChkDR( maker.CopyFrom( reinterpret_cast<const BYTE*>( pvValue ), cbValue ) );

    //
    // Hand off array to the variant
    //

    pOutVar->vt = ( VT_UI1 | VT_ARRAY );
    pOutVar->parray = maker.Detach();

ErrorExit:

    return dr;
}

/*static*/ HRESULT VariantConverter::FromDrmID(
    __in const DRM_ID  &value,
    __out      VARIANT *pOutVar )
{
    return VariantConverter::FromGenericData( &value, sizeof( value ), pOutVar );
}

/*static*/ HRESULT VariantConverter::ToGenericData(
    __in const VARIANT                 *pInVar,
    __inout    SafeArrayAccessor<BYTE> *pOutValue )
{
    HRESULT dr = S_OK;

    AssertChkArg( pInVar != nullptr );
    AssertChkArg( pOutValue != nullptr );

    ChkBOOL( pInVar->vt == ( VT_UI1 | VT_ARRAY ), E_INVALIDARG );

    ChkDR( pOutValue->AccessByReference( pInVar->parray ) );

ErrorExit:

    return dr;
}

/*static*/ HRESULT VariantConverter::ToDrmID(
    __in const VARIANT *pInVar,
    __out      DRM_ID  *pOutValue )
{
    HRESULT                 dr = S_OK;
    SafeArrayAccessor<BYTE> accessor;

    AssertChkArg( pInVar != nullptr );
    AssertChkArg( pOutValue != nullptr );

    ChkDR( VariantConverter::ToGenericData( pInVar, &accessor ) );

    ChkBOOL( accessor.Size() == sizeof( *pOutValue ), E_INVALIDARG );

    memcpy_s( pOutValue, sizeof( *pOutValue ), accessor.Elements(), accessor.Size() );

ErrorExit:

    return dr;
}

/*static*/ HRESULT VariantConverter::MoveArrayToVariant(
    __inout SafeArrayAccessor<BYTE> *pSource,
    __out   VARIANT                 *pOutVar )
{
    HRESULT dr = S_OK;

    AssertChkArg( pSource != nullptr );
    AssertChkArg( pOutVar != nullptr );
    AssertChkArg( pOutVar->vt == VT_EMPTY );

    ChkBOOL( pSource->Elements() != nullptr, E_UNEXPECTED );

    pOutVar->vt = ( VT_UI1 | VT_ARRAY );
    pOutVar->parray = pSource->Detach();

ErrorExit:

    return dr;
}

////////////////////////////////////////////////////////////////////////////////
//
// TypeInfoHolder implementation
//
////////////////////////////////////////////////////////////////////////////////

HRESULT TypeInfoHolder::LoadFromModule(
    __in_opt   HMODULE hModule,
    __in const GUID  &guid )
{
    HRESULT           dr      = S_OK;
    DWORD             cchPath = 0;
    WCHAR             szModulePath[MAX_PATH];
    CComPtr<ITypeLib> spTypeLib;

    ChkBOOL( m_spTypeInfo == nullptr, HRESULT_FROM_WIN32( ERROR_ALREADY_INITIALIZED ) );

    cchPath = ::GetModuleFileNameW( hModule, reinterpret_cast<LPWSTR>( szModulePath ), ARRAYSIZE( szModulePath ) );

    ChkBOOL( 0 < cchPath && cchPath <= ARRAYSIZE( szModulePath ), E_FAIL );

    ChkDR( ::LoadTypeLib( szModulePath, &spTypeLib ) );

    ChkDR( spTypeLib->GetTypeInfoOfGuid( guid, &m_spTypeInfo ) );

ErrorExit:

    return dr;
}

HRESULT TypeInfoHolder::GetTypeInfoCount(
    __out UINT *pcTInfo )
{
    HRESULT dr = S_OK;

    ChkBOOL( pcTInfo != nullptr, E_POINTER );
    ChkBOOL( m_spTypeInfo != nullptr, E_NOT_SET );

    *pcTInfo = 1;

ErrorExit:

    return dr;
}

HRESULT TypeInfoHolder::GetTypeInfo(
    __in  UINT        iTInfo,
    __in  LCID        lcid,
    __out ITypeInfo **ppTInfo )
{
    HRESULT dr = S_OK;

    ChkBOOL( ppTInfo != nullptr, E_POINTER );

    *ppTInfo = nullptr;

    ChkBOOL( iTInfo == 0, DISP_E_BADINDEX );
    ChkBOOL( m_spTypeInfo != nullptr, E_NOT_SET );

    ChkDR( m_spTypeInfo.CopyTo( ppTInfo ) );

ErrorExit:

    return dr;
}

HRESULT TypeInfoHolder::GetIDsOfNames(
    __in                  REFIID    riid,
    __in_ecount( cNames ) LPOLESTR *rgszNames,
    __in_range( 0,16384 ) UINT      cNames,
    __in                  LCID      lcid,
    __out                 DISPID   *rgDispId )
{
    HRESULT dr = S_OK;

    ChkBOOL( m_spTypeInfo != nullptr, E_NOT_SET );

    ChkDR( m_spTypeInfo->GetIDsOfNames( rgszNames, cNames, rgDispId ) );

ErrorExit:

    return dr;
}

HRESULT TypeInfoHolder::Invoke(
    __inout   IDispatch  *p,
    __in      DISPID      dispidMember,
    __in      REFIID      riid,
    __in      LCID        lcid,
    __in      WORD        wFlags,
    __in      DISPPARAMS *pdispparams,
    __out_opt VARIANT    *pvarResult,
    __out_opt EXCEPINFO  *pexcepinfo,
    __out_opt UINT       *puArgErr )
{
    HRESULT dr = S_OK;

    ChkBOOL( m_spTypeInfo != nullptr, E_NOT_SET );

    ChkDR( m_spTypeInfo->Invoke( p, dispidMember, wFlags, pdispparams, pvarResult, pexcepinfo, puArgErr ) );

ErrorExit:

    return dr;
}
