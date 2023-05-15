/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmcommon.h>
#include <drmutilities.h>
#include <drmcrt.h>
#include <drmxmlparser.h>
#include <drmdevcert.h>
#include <oemdes.h>
#include <oempkcrypto.h>
#include <drmbcertparser.h>
#include <oem.h>
#include <drmantirollbackclock.h>
#include <drmcertparsercache.h>
#include <drmdevcertconstants.h>

ENTER_PK_NAMESPACE_CODE;

DRM_API DRM_RESULT DRM_CALL DRM_DCP_GetAttribute(
    __in_opt  DRM_DEVICE_CERT_PARSER_CACHE *f_poParserCache,
    __in      const DRM_CONST_STRING       *f_pdstrDevCert,
    __in      eDRM_DEVCERT_ATTRIBUTES       f_eAttribute,
    __out_opt DRM_CONST_STRING             *f_pdstrNode,
    __out_opt DRM_CONST_STRING             *f_pdstrValue )
{
    UNREFERENCED_PARAMETER( f_poParserCache );
    UNREFERENCED_PARAMETER( f_pdstrDevCert  );
    UNREFERENCED_PARAMETER( f_eAttribute    );
    UNREFERENCED_PARAMETER( f_pdstrNode     );
    UNREFERENCED_PARAMETER( f_pdstrValue    );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_DCP_StartCertParserCache(
    __in     const DRM_CONST_STRING       *f_pdstrDevCert,
    __in_opt DRM_KEYFILE_CONTEXT          *f_poKeyFileContext,
    __out    DRM_DEVICE_CERT_PARSER_CACHE *f_poParserCache )
{
    UNREFERENCED_PARAMETER( f_pdstrDevCert     );
    UNREFERENCED_PARAMETER( f_poKeyFileContext );
    UNREFERENCED_PARAMETER( f_poParserCache    );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_DCP_StopCertParserCache(
    __in_opt DRM_KEYFILE_CONTEXT          *f_poKeyFileContext,
    __in     DRM_DEVICE_CERT_PARSER_CACHE *f_poParserCache )
{
    UNREFERENCED_PARAMETER( f_poKeyFileContext );
    UNREFERENCED_PARAMETER( f_poParserCache    );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_DCP_VerifyDeviceCert(
    __in_opt   DRM_DEVICE_CERT_PARSER_CACHE *f_poParserCache,
    __in const DRM_CONST_STRING             *f_pdstrDevCert,
    __in       DRM_DWORD                     f_dwVerifyFlags,
    __in_opt   DRM_KEYFILE_CONTEXT          *f_poKeyFileContext,
    __in       DRM_CRYPTO_CONTEXT           *f_pcontextCRYP )
{
    UNREFERENCED_PARAMETER( f_poParserCache    );
    UNREFERENCED_PARAMETER( f_pdstrDevCert     );
    UNREFERENCED_PARAMETER( f_dwVerifyFlags    );
    UNREFERENCED_PARAMETER( f_poKeyFileContext );
    UNREFERENCED_PARAMETER( f_pcontextCRYP     );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_DCP_LoadPropertiesCache(
    __in_opt DRM_DEVICE_CERT_PARSER_CACHE          *f_poParserCache,
    __in     const DRM_CONST_STRING                *f_pdstrDevCert,
    __in_opt DRM_KEYFILE_CONTEXT                   *f_poKeyFileContext,
    __in     DRM_CRYPTO_CONTEXT                    *f_poCryptoContext,
    __out    DRM_DEVICE_CERTIFICATE_CACHED_VALUES  *f_pcacheDevCert )
{
    UNREFERENCED_PARAMETER( f_poParserCache    );
    UNREFERENCED_PARAMETER( f_pdstrDevCert     );
    UNREFERENCED_PARAMETER( f_poKeyFileContext );
    UNREFERENCED_PARAMETER( f_poCryptoContext  );
    UNREFERENCED_PARAMETER( f_pcacheDevCert    );
    return DRM_E_NOTIMPL;
}

EXIT_PK_NAMESPACE_CODE;

