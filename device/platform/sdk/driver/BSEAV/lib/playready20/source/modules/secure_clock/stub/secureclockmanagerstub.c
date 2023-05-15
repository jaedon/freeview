/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmcommon.h>
#include <drmsecureclock.h>
#include <drmcontextsizes.h>
#include <drmmanager.h>

ENTER_PK_NAMESPACE_CODE;

DRM_API DRM_RESULT DRM_CALL DRM_CLK_CheckSecureClock(
    IN DRM_APP_CONTEXT_INTERNAL *f_poAppContextInternal )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_CLK_Drm_SecureClock_ProcessResponseImpl(
    IN                          DRM_APP_CONTEXT *f_poAppContext,
    __in_bcount( f_cbResponse ) DRM_BYTE        *f_pbResponse,
    IN                          DRM_DWORD        f_cbResponse,
    OUT                         DRM_RESULT      *f_pResult )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_CLK_Drm_SecureClock_GenerateChallengeImpl(
    IN                                  DRM_APP_CONTEXT *f_poAppContext,
    __out_ecount_opt( *f_pcchURL )      DRM_WCHAR       *f_pwszURL,
    IN OUT                              DRM_DWORD       *f_pcchURL,
    __out_bcount_opt( *f_pcbChallenge ) DRM_BYTE        *f_pbChallenge,
    IN OUT                              DRM_DWORD       *f_pcbChallenge )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_CLK_Drm_SecureClock_GetValueImpl(
    IN                                            DRM_APP_CONTEXT *f_poAppContext,
    __in_ecount_opt( *f_pcchSecureTime )          DRM_WCHAR       *f_pwszSecureTime,
    IN OUT                                        DRM_DWORD       *f_pcchSecureTime,
    IN                                            DRM_DWORD       *f_pdwFlags,
    __in_bcount_opt( *f_pcbSecureTimeWithStatus ) DRM_BYTE        *f_pbSecureTimeWithStatus,
    IN OUT                                        DRM_DWORD       *f_pcbSecureTimeWithStatus )
{
    return DRM_E_NOTIMPL;
}

EXIT_PK_NAMESPACE_CODE;

