/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmxmlhash.h>

ENTER_PK_NAMESPACE_CODE;

DRM_RESULT DRM_API DRM_CALL DRM_XMLHASH_Update(
    __in DRM_XML_HASH_CONTEXT *f_poContext,
    __in DRM_XML_HASH_SIG *f_poSig,
    __in const DRM_CONST_STRING *f_pdstrXML,
    __in DRM_CONST_STRING *f_pdstrNode,
    __in DRM_CONST_STRING *f_pdstrNodeData )
{
    UNREFERENCED_PARAMETER( f_poContext );
    UNREFERENCED_PARAMETER( f_poSig );
    UNREFERENCED_PARAMETER( f_pdstrXML );
    UNREFERENCED_PARAMETER( f_pdstrNode );
    UNREFERENCED_PARAMETER( f_pdstrNodeData );
    return DRM_E_NOTIMPL;
}

DRM_RESULT DRM_API DRM_CALL DRM_XMLHASH_Lookup(
    __in DRM_XML_HASH_CONTEXT *f_poContext,
    __in DRM_XML_HASH_SIG *f_poSig,
    __in const DRM_CONST_STRING *f_pdstrXML,
    __in const DRM_CONST_STRING *f_pdstrTag,
    __out DRM_CONST_STRING *f_pdstrNodeOut,
    __out DRM_CONST_STRING *f_pdstrNodeDataOut )
{
    UNREFERENCED_PARAMETER( f_poContext );
    UNREFERENCED_PARAMETER( f_poSig );
    UNREFERENCED_PARAMETER( f_pdstrXML );
    UNREFERENCED_PARAMETER( f_pdstrTag );
    UNREFERENCED_PARAMETER( f_pdstrNodeOut );
    UNREFERENCED_PARAMETER( f_pdstrNodeDataOut );
    return DRM_E_NOTIMPL;
}

DRM_RESULT DRM_API DRM_CALL DRM_XMLHASH_CalcParamSig(
    __in DRM_XML_HASH_CONTEXT *f_poContext,
    __in const DRM_CONST_STRING *f_pdstrXML,
    __in const DRM_CONST_STRING *f_pdstrTag,
    __in const DRM_DWORD f_iNode,
    __in const DRM_DWORD f_iLayer,
    __out DRM_XML_HASH_SIG *f_poSig )
{
    UNREFERENCED_PARAMETER( f_poContext );
    UNREFERENCED_PARAMETER( f_pdstrXML );
    UNREFERENCED_PARAMETER( f_pdstrTag );
    UNREFERENCED_PARAMETER( f_iNode );
    UNREFERENCED_PARAMETER( f_iLayer );
    UNREFERENCED_PARAMETER( f_poSig );
    return DRM_E_NOTIMPL;
}

DRM_RESULT DRM_API DRM_CALL DRM_XMLHASH_GetContext(
    __out DRM_XML_HASH_CONTEXT **f_ppoContext )
{
    UNREFERENCED_PARAMETER( f_ppoContext );
    return DRM_E_NOTIMPL;
}

DRM_RESULT DRM_API DRM_CALL DRM_XMLHASH_Start( DRM_VOID )
{
    return DRM_E_NOTIMPL;
}

DRM_RESULT DRM_API DRM_CALL DRM_XMLHASH_Stop( DRM_VOID )
{
    return DRM_E_NOTIMPL;
}

DRM_RESULT DRM_API DRM_CALL DRM_XMLHASH_Init( DRM_VOID )
{
    return DRM_SUCCESS;
}

DRM_RESULT DRM_API DRM_CALL DRM_XMLHASH_UnInit( DRM_VOID )
{
    return DRM_SUCCESS;
}

DRM_RESULT DRM_API DRM_CALL DRM_XMLHASH_Cleanup( DRM_VOID )
{
    return DRM_E_NOTIMPL;
}

EXIT_PK_NAMESPACE_CODE;

