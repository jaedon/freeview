/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmcertparsercache.h>

ENTER_PK_NAMESPACE_CODE;

PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_NONCONST_BUFFER_PARAM, "Changing parameters to const to satisfy warning would make them not match the real interface.")

DRM_API DRM_RESULT DRM_CALL DRM_CERTPARSERCACHE_PackCertParserEntry(
    __in  DRM_WORD                            f_iAttrib,
    __in_bcount( f_cbPacked )
          DRM_BYTE                           *f_pbPacked,
    __in  DRM_DWORD                           f_cbPacked,
    __out DRM_DEVICE_CERT_PARSER_CACHE_ENTRY *f_poEntry )
{
    UNREFERENCED_PARAMETER( f_iAttrib );
    UNREFERENCED_PARAMETER( f_pbPacked );
    UNREFERENCED_PARAMETER( f_cbPacked );
    UNREFERENCED_PARAMETER( f_poEntry );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_CERTPARSERCACHE_UnpackCertParserEntry(
    __in  DRM_WORD                            f_iAttrib,
    __in_bcount( f_cbPacked )
          const DRM_BYTE                     *f_pbPacked,
    __in  DRM_DWORD                           f_cbPacked,
    __out DRM_DEVICE_CERT_PARSER_CACHE_ENTRY *f_poEntry )
{
    UNREFERENCED_PARAMETER( f_iAttrib );
    UNREFERENCED_PARAMETER( f_pbPacked );
    UNREFERENCED_PARAMETER( f_cbPacked );
    UNREFERENCED_PARAMETER( f_poEntry );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_CERTPARSERCACHE_DCP_StartCertParserCache(
    __in     const DRM_CONST_STRING       *f_pdstrDevCert,
    __in_opt DRM_KEYFILE_CONTEXT          *f_poKeyFileContext,
    __out    DRM_DEVICE_CERT_PARSER_CACHE *f_poParserCache )
{
    UNREFERENCED_PARAMETER( f_pdstrDevCert );
    UNREFERENCED_PARAMETER( f_poKeyFileContext );
    UNREFERENCED_PARAMETER( f_poParserCache );
    return DRM_SUCCESS;
}

DRM_API DRM_RESULT DRM_CALL DRM_CERTPARSERCACHE_DCP_StopCertParserCache(
    __in_opt DRM_KEYFILE_CONTEXT          *f_poKeyFileContext,
    __in     DRM_DEVICE_CERT_PARSER_CACHE *f_poParserCache )
{
    UNREFERENCED_PARAMETER( f_poKeyFileContext );
    UNREFERENCED_PARAMETER( f_poParserCache );
    return DRM_SUCCESS;
}


PREFAST_POP

EXIT_PK_NAMESPACE_CODE;

