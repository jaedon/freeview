/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <devicedevcert.h>

ENTER_PK_NAMESPACE_CODE;

DRM_API DRM_RESULT DRM_CALL DRM_DDC_Certificates_Initialize(
    __in     DRM_APP_CONTEXT  *f_pAppContext,
    __in     DRM_VOID         *f_pOEMContext )
{
    /* No certificates to initialize. */
    return DRM_SUCCESS;
}

DRM_API DRM_RESULT DRM_CALL DRM_DDC_GetXMLDeviceCertificate(
    __in_opt       DRM_VOID            *f_pOEMContext,
    __in           DRM_KEYFILE_CONTEXT *f_poKeyFileContext,
    __inout        DRM_STRING          *f_pdstrWMDRMCertStr,
    __in     const DRM_DWORD            f_dwFlags,
    __inout        DRM_CRYPTO_CONTEXT  *f_pcontextCRYP )
{
    /* Should not be called if assets aren't supported. */
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_DDC_GetBinaryDeviceCertificate(
    __in_opt                               DRM_VOID            *f_pOEMContext,
    __in                                   DRM_KEYFILE_CONTEXT *f_poKeyFileContext,
    __out_bcount(*f_pcbBinaryDevCertChain) DRM_BYTE             *f_pBinaryDevCertChain,
    __inout                                DRM_DWORD            *f_pcbBinaryDevCertChain,
    __in                                   const DRM_DWORD      f_dwFlags,
    __inout                                DRM_CRYPTO_CONTEXT   *f_pCryptoCtx )
{
    /* Should not be called if assets aren't supported. */
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_DDC_MatchRobustnessPlatformAndModel(
    __in_opt                        DRM_VOID  *f_pOEMContext,
    __in_bcount(f_cbCertData) const DRM_BYTE  *f_pbCertData,
    __in                            DRM_DWORD f_cbCertData,
    __in                            DRM_BOOL  f_fDeviceCert )
{
    /* Should not be called if assets aren't supported. */
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_DDC_LoadXMLCertPropertiesCache(
    __in_opt       DRM_VOID                        *f_pOEMContext,
    __in_opt       DRM_DEVICE_CERT_PARSER_CACHE    *f_poParserCache,
    __in           DRM_BB_CONTEXT                  *f_pBlackBoxContext,
    __in_opt       DRM_KEYFILE_CONTEXT             *f_poKeyFileContext,
    __in     const DRM_CONST_STRING                *f_pdstrXMLDeviceCert )
{
    /* Should not be called if assets aren't supported. */
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_DDC_LoadBinaryCertPropertiesCache(
    __in_opt                                DRM_VOID         *f_pOEMContext,
    __in                                    DRM_BB_CONTEXT   *f_pBlackBoxContext,
    __in_bcount(f_cbBinaryDeviceCert) const DRM_BYTE         *f_pbBinaryDeviceCert,
    __in                                    DRM_DWORD         f_cbBinaryDeviceCert )
{
    /* Should not be called if assets aren't supported. */
    return DRM_E_NOTIMPL;
}

EXIT_PK_NAMESPACE_CODE;

