/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmwmdrm.h>

ENTER_PK_NAMESPACE_CODE;

PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_NONCONST_PARAM, "Changing parameters to const to satisfy warning would make them not match the real interface.")

DRM_API DRM_BOOL DRM_CALL DRM_WMDRM_IsWMDRMSupported(DRM_VOID)
{
    return FALSE;
}

DRM_API DRM_BOOL DRM_CALL DRM_WMDRM_IsWMDRMUnsupported(DRM_VOID)
{
    return TRUE;
}

DRM_API DRM_RESULT DRM_CALL DRM_WMDRM_HDR_VerifyLegacySignature(
    __in const DRM_CONST_STRING   *f_pdstrContentHeader,
    __in const DRM_CONST_STRING   *f_pdstrPubKey,
    __in       DRM_CONST_STRING   *f_pdstrData,
    __in       DRM_CRYPTO_CONTEXT *f_pContext )
{
    UNREFERENCED_PARAMETER( f_pdstrContentHeader );
    UNREFERENCED_PARAMETER( f_pdstrPubKey        );
    UNREFERENCED_PARAMETER( f_pdstrData          );
    UNREFERENCED_PARAMETER( f_pContext           );
    return DRM_E_NOTIMPL;
}

PREFAST_POP /* Changing parameters to const to satisfy warning would make them not match the real interface. */

EXIT_PK_NAMESPACE_CODE;

