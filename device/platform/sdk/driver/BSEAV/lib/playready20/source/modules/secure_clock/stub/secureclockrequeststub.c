/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmcommon.h>
#include <drmcontextsizes.h>
#include <drmsecureclock.h>

ENTER_PK_NAMESPACE_CODE;

DRM_EXPORT_VAR const DRM_ID g_idSData     = { 'S', 0, 'E', 0, 'C', 0, 'U', 0, 'R', 0, 'E', 0, '1', 0, '\0', 0 };
DRM_EXPORT_VAR const DRM_ID g_idSDataPrev = { 'S', 0, 'E', 0, 'C', 0, 'U', 0, 'R', 0, 'E', 0, '2', 0, '\0', 0 };

DRM_VOID _PrepareSecureClockDataForWriting( DRM_SECSTORE_CLOCK_DATA *pData )
{
    /*
    ** Do nothing.
    */
    return;
}

/******************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_CLK_CreateChallenge(
    IN const DRM_CONST_STRING      *pdstrURL,
    IN const DRM_BYTE               rgbSecStoreInPswd [__CB_DECL(DRM_SHA1_DIGEST_LEN)],
    IN       DRM_CRYPTO_CONTEXT    *pContext,
    IN       DRM_DST               *poDatastore,
    IN       DRM_SECSTORE_CONTEXT  *poSecureStoreContext,
    IN       DRM_VOID              *pOEMContext,
    OUT      DRM_WCHAR             *pwszUrl,
    IN OUT   DRM_DWORD             *pcchUrl,
    IN       DRM_BYTE              *pbData,
    IN OUT   DRM_DWORD             *pcbData )
{
    return DRM_E_NOTIMPL;
}

/******************************************************************************/

DRM_API DRM_RESULT DRM_CALL DRM_CLK_GetSecureClock(
    OUT      DRM_WCHAR            *pwszSecureTime,
    IN OUT   DRM_DWORD            *pcchSecureTime,
    OUT      DRM_DWORD            *pdwFlags,
    IN       DRM_BYTE             *pbSecureTimeWithStatus,
    IN OUT   DRM_DWORD            *pcbSecureTimeWithStatus,
    IN const DRM_BYTE              rgbSecStoreInPswd [__CB_DECL(DRM_SHA1_DIGEST_LEN)],
    IN       DRM_CRYPTO_CONTEXT   *pContext,
    IN       DRM_DST              *poDatastore,
    IN       DRM_SECSTORE_CONTEXT *poSecureStoreContext,
    IN       DRM_VOID             *pOEMContext )
{
    return DRM_E_NOTIMPL;
}

/******************************************************************************/

DRM_API DRM_RESULT DRM_CALL DRM_CLK_InitCheckSecureClock(
    IN const DRM_BYTE              rgbSecStoreInPswd [__CB_DECL(DRM_SHA1_DIGEST_LEN)],
    IN       DRM_CRYPTO_CONTEXT   *pContext,
    IN       DRM_DST              *poDatastore,
    IN OUT   DRM_BOOL             *pfClockSet,
    IN       DRM_SECSTORE_CONTEXT *poSecureStoreContext,
    IN       DRM_LICEVAL_CONTEXT  *poLicEvalContext,
    IN       DRM_VOID             *pOEMContext )
{
    return DRM_E_NOTIMPL;
}

EXIT_PK_NAMESPACE_CODE;

