/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_SECURECLOCKMANAGER_C
#include <drmsecureclock.h>
#include <drmlastinclude.h>

ENTER_PK_NAMESPACE_CODE;

DRM_API DRM_RESULT DRM_CALL DRM_CLK_CheckSecureClock(
    __in DRM_APP_CONTEXT_INTERNAL *f_poAppContextInternal )
{
    DRM_RESULT dr = DRM_SUCCESS;

    AssertChkArg( f_poAppContextInternal != NULL );
    f_poAppContextInternal->fClockSet = TRUE;

ErrorExit:
    return dr;
}

DRM_API DRM_RESULT DRM_CALL DRM_CLK_Drm_SecureClock_ProcessResponseImpl(
    __in                        DRM_APP_CONTEXT *f_poAppContext,
    __in_bcount( f_cbResponse ) DRM_BYTE        *f_pbResponse,
    __in                        DRM_DWORD        f_cbResponse,
    __out                       DRM_RESULT      *f_pResult )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_CLK_Drm_SecureClock_GenerateChallengeImpl(
    __in                                DRM_APP_CONTEXT *f_poAppContext,
    __out_ecount_opt( *f_pcchURL )      DRM_WCHAR       *f_pwszURL,
    __inout_opt                         DRM_DWORD       *f_pcchURL,
    __out_bcount_opt( *f_pcbChallenge ) DRM_BYTE        *f_pbChallenge,
    __inout                             DRM_DWORD       *f_pcbChallenge )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_CLK_Drm_SecureClock_GetValueImpl(
    __in                                          DRM_APP_CONTEXT *f_poAppContext,
    __in_ecount_opt( *f_pcchSecureTime )          DRM_WCHAR       *f_pwszSecureTime,
    __inout_opt                                   DRM_DWORD       *f_pcchSecureTime,
    __in_opt                                      DRM_DWORD       *f_pdwFlags,
    __in_bcount_opt( *f_pcbSecureTimeWithStatus ) DRM_BYTE        *f_pbSecureTimeWithStatus,
    __inout                                       DRM_DWORD       *f_pcbSecureTimeWithStatus )
{
    return DRM_E_NOTIMPL;
}

EXIT_PK_NAMESPACE_CODE;

