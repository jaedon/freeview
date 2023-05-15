/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmcontextsizes.h>
#include <drmsecureclock.h>

ENTER_PK_NAMESPACE_CODE;

DRM_EXPORT_VAR const DRM_ID g_idSData     = { 'S', 0, 'E', 0, 'C', 0, 'U', 0, 'R', 0, 'E', 0, '1', 0, '\0', 0 };
DRM_EXPORT_VAR const DRM_ID g_idSDataPrev = { 'S', 0, 'E', 0, 'C', 0, 'U', 0, 'R', 0, 'E', 0, '2', 0, '\0', 0 };

DRM_API DRM_VOID DRM_CALL _PrepareSecureClockDataForWriting( DRM_SECSTORE_CLOCK_DATA *pData )
{
    /*
    ** Do nothing.
    */
    return;
}

/******************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_CLK_CreateChallenge(
    __in                       const DRM_CONST_STRING      *pdstrURL,
    __in                       const DRM_BYTE               rgbSecStoreInPswd [__CB_DECL(DRM_SHA1_DIGEST_LEN)],
    __in                             DRM_CRYPTO_CONTEXT    *pContext,
    __in                             DRM_DST               *poDatastore,
    __in                             DRM_SECSTORE_CONTEXT  *poSecureStoreContext,
    __in                             DRM_VOID              *pOEMContext,
    __out_ecount_opt(*pcchUrl)       DRM_WCHAR             *pwszUrl,
    __inout_opt                      DRM_DWORD             *pcchUrl,
    __out_ecount_opt(*pbData)        DRM_BYTE              *pbData,
    __inout                          DRM_DWORD             *pcbData )
{
    return DRM_E_NOTIMPL;
}

/******************************************************************************/

DRM_API DRM_RESULT DRM_CALL DRM_CLK_GetSecureClock(
    __out_ecount_opt(*pcchSecureTime)                DRM_WCHAR            *pwszSecureTime,
    __inout_opt                                      DRM_DWORD            *pcchSecureTime,
    __out_ecount_opt(1)                              DRM_DWORD            *pdwFlags,
    __out_ecount_opt(*pcbSecureTimeWithStatus)       DRM_BYTE             *pbSecureTimeWithStatus,
    __inout_opt                                      DRM_DWORD            *pcbSecureTimeWithStatus,
    __in                                       const DRM_BYTE              rgbSecStoreInPswd [__CB_DECL(DRM_SHA1_DIGEST_LEN)],
    __in                                             DRM_CRYPTO_CONTEXT   *pContext,
    __in                                             DRM_DST              *poDatastore,
    __in                                             DRM_SECSTORE_CONTEXT *poSecureStoreContext,
    __in                                             DRM_VOID             *pOEMContext )
{
    return DRM_E_NOTIMPL;
}

/******************************************************************************/

DRM_API DRM_RESULT DRM_CALL DRM_CLK_InitCheckSecureClock(
    __in    const DRM_BYTE              rgbSecStoreInPswd [__CB_DECL(DRM_SHA1_DIGEST_LEN)],
    __in    const DRM_CRYPTO_CONTEXT   *pContext,
    __in          DRM_DST              *poDatastore,
    __inout       DRM_BOOL             *pfClockSet,
    __in          DRM_SECSTORE_CONTEXT *poSecureStoreContext,
    __in          DRM_LICEVAL_CONTEXT  *poLicEvalContext,
    __in          DRM_VOID             *pOEMContext )
{
    return DRM_E_NOTIMPL;
}

EXIT_PK_NAMESPACE_CODE;

