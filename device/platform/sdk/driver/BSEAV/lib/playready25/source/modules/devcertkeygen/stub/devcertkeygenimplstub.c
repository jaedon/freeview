/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmdevcertkeygen.h>

ENTER_PK_NAMESPACE_CODE;

DRM_API DRM_NO_INLINE DRM_BOOL DRM_CALL DRM_DDC_IsDevCertKeyGenSupported(DRM_VOID)   { return FALSE; }
DRM_API DRM_NO_INLINE DRM_BOOL DRM_CALL DRM_DDC_IsDevCertKeyGenUnsupported(DRM_VOID) { return TRUE; }

DRM_API DRM_RESULT DRM_CALL DRM_DDC_SetXMLDeviceCertificateAndKey(
    __in           DRM_BB_CONTEXT      *f_pBlackBoxContext,
    __in           DRM_KEYFILE_CONTEXT *f_poKeyFileContext,
    __inout        DRM_STRING          *f_pdstrWMDRMCertStr )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_DDC_CreateXMLDeviceCertificateAndKey(
    __in_opt       DRM_VOID                         *f_pOEMContext,
    __in           DRM_KEYFILE_CONTEXT              *f_poKeyFileContext,
    __in           DRM_DEVICE_CERT_PARSER_CACHE     *f_poParserCache,
    __in           DRM_BB_CONTEXT                   *f_pBlackBoxContext,
    __inout        DRM_STRING                       *f_pdstrWMDRMCertStr )
{
    return DRM_E_DEVCERT_READ_ERROR;
}

DRM_API DRM_RESULT DRM_CALL DRM_DDC_SetBinaryDeviceCertificateAndKey(
    __in_opt                               DRM_VOID             *f_pOEMContext,
    __in                                   DRM_KEYFILE_CONTEXT  *f_poKeyFileContext,
    __out_bcount(*f_pcbBinaryDevCertChain) DRM_BYTE             *f_pBinaryDevCertChain,
    __inout                                DRM_DWORD            *f_pcbBinaryDevCertChain,
    __in                                   DRM_BB_CONTEXT       *f_pBlackBoxContext )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_DDC_CreateBinaryDeviceCertificateAndKey(
    __in_opt                               DRM_VOID             *f_pOEMContext,
    __in                                   DRM_KEYFILE_CONTEXT  *f_poKeyFileContext,
    __out_bcount(*f_pcbBinaryDevCertChain) DRM_BYTE             *f_pBinaryDevCertChain,
    __inout                                DRM_DWORD            *f_pcbBinaryDevCertChain,
    __in                                   DRM_BB_CONTEXT       *f_pBlackBoxContext )
{
    return DRM_E_DEVCERT_READ_ERROR;
}

DRM_API DRM_RESULT DRM_CALL DRM_DDC_CreateSecureStoreKeyAndSaveKeyFile(
    __in_opt            DRM_VOID       *f_pOEMContext,
    __inout_ecount( 1 ) DRM_BB_CONTEXT *f_pBlackBoxContext )
{
    return DRM_E_DEVCERT_READ_ERROR;
}

EXIT_PK_NAMESPACE_CODE;

