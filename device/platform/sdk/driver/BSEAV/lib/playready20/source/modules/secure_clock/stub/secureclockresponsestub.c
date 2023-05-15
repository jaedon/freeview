/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmcommon.h>
#include <drmcontextsizes.h>

ENTER_PK_NAMESPACE_CODE;

/******************************************************************************/

DRM_API DRM_RESULT DRM_CALL DRM_CLK_ProcessResponse(
    IN const PUBKEY                *ppubkeySecureClockServer,
    IN       DRM_BYTE              *pbResponse,
    IN       DRM_DWORD              cbResponse,
    IN const DRM_BYTE               rgbSecStoreInPswd [__CB_DECL(DRM_SHA1_DIGEST_LEN)],
    OUT      DRM_RESULT            *pResult,
    IN       DRM_CRYPTO_CONTEXT    *pContext,
    IN       DRM_DST               *poDatastore,
    IN       DRM_SECSTORE_CONTEXT  *poSecureStoreContext,
    IN       DRM_LICEVAL_CONTEXT   *poLicEvalContext,
    IN       DRM_VOID              *pOEMContext )
{
    return DRM_E_NOTIMPL;
}

EXIT_PK_NAMESPACE_CODE;

