/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmcontextsizes.h>

ENTER_PK_NAMESPACE_CODE;

/******************************************************************************/

DRM_API DRM_RESULT DRM_CALL DRM_CLK_ProcessResponse(
    __in                    const PUBKEY                *ppubkeySecureClockServer,
    __in_ecount(cbResponse)       DRM_BYTE              *pbResponse,
    __in                          DRM_DWORD              cbResponse,
    __in                    const DRM_BYTE               rgbSecStoreInPswd [__CB_DECL(DRM_SHA1_DIGEST_LEN)],
    __out                         DRM_RESULT            *pResult,
    __in                          DRM_CRYPTO_CONTEXT    *pContext,
    __in                          DRM_DST               *poDatastore,
    __in                          DRM_SECSTORE_CONTEXT  *poSecureStoreContext,
    __in                          DRM_LICEVAL_CONTEXT   *poLicEvalContext,
    __in                          DRM_VOID              *pOEMContext )
{
    return DRM_E_NOTIMPL;
}

EXIT_PK_NAMESPACE_CODE;

