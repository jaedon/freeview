/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_XMLHASHIMPLREAL_C
#include <drmxmlhash.h>
#include <drmlastinclude.h>

ENTER_PK_NAMESPACE_CODE;

DRM_API DRM_RESULT DRM_CALL DRM_XMLHASH_Update(
    __in DRM_XML_HASH_CONTEXT *f_poContext,
    __in const DRM_XML_HASH_SIG *f_poSig,
    __in const DRM_CONST_STRING *f_pdstrXML,
    __in const DRM_CONST_STRING *f_pdstrNode,
    __in const DRM_CONST_STRING *f_pdstrNodeData )
{
    return Oem_XMLHash_Update(
        f_poContext,
        f_poSig,
        f_pdstrXML,
        f_pdstrNode,
        f_pdstrNodeData );
}

DRM_API DRM_RESULT DRM_CALL DRM_XMLHASH_Lookup(
    __in DRM_XML_HASH_CONTEXT *f_poContext,
    __in const DRM_XML_HASH_SIG *f_poSig,
    __in const DRM_CONST_STRING *f_pdstrXML,
    __in const DRM_CONST_STRING *f_pdstrTag,
    __out DRM_CONST_STRING *f_pdstrNodeOut,
    __out DRM_CONST_STRING *f_pdstrNodeDataOut )
{
    return Oem_XMLHash_Lookup(
        f_poContext,
        f_poSig,
        f_pdstrXML,
        f_pdstrTag,
        f_pdstrNodeOut,
        f_pdstrNodeDataOut );
}

DRM_API DRM_RESULT DRM_CALL DRM_XMLHASH_CalcParamSig(
    __in const DRM_XML_HASH_CONTEXT *f_poContext,
    __in const DRM_CONST_STRING *f_pdstrXML,
    __in const DRM_CONST_STRING *f_pdstrTag,
    __in const DRM_DWORD f_iNode,
    __in const DRM_DWORD f_iLayer,
    __out DRM_XML_HASH_SIG *f_poSig )
{
    return Oem_XMLHash_CalcParamSig(
        f_poContext,
        f_pdstrXML,
        f_pdstrTag,
        f_iNode,
        f_iLayer,
        f_poSig );
}

DRM_API DRM_RESULT DRM_CALL DRM_XMLHASH_GetContext(
    __out DRM_XML_HASH_CONTEXT **f_ppoContext )
{
    return Oem_XMLHash_GetContext(
        f_ppoContext );
}

DRM_API DRM_RESULT DRM_CALL DRM_XMLHASH_Start( DRM_VOID )
{
    return Oem_XMLHash_Start();
}

DRM_API DRM_RESULT DRM_CALL DRM_XMLHASH_Stop( DRM_VOID )
{
    return Oem_XMLHash_Stop();
}

DRM_API DRM_RESULT DRM_CALL DRM_XMLHASH_Init( DRM_VOID )
{
    return Oem_XMLHash_Init();
}

DRM_API DRM_RESULT DRM_CALL DRM_XMLHASH_UnInit( DRM_VOID )
{
    return Oem_XMLHash_UnInit();
}

DRM_API DRM_RESULT DRM_CALL DRM_XMLHASH_Cleanup( DRM_VOID )
{
    return Oem_XMLHash_Cleanup();
}

EXIT_PK_NAMESPACE_CODE;

