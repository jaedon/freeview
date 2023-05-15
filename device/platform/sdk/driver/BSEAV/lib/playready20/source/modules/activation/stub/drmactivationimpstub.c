/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmactivationimp.h>
#include <drmresults.h>

ENTER_PK_NAMESPACE_CODE;

PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_NONCONST_BUFFER_PARAM, "Changing parameters to const to satisfy warning would make them not match the real interface.")

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_ACT_GenerateChallengeInternal(
    __in                                      DRM_DWORD   f_dwClientSdkVersion,
    __in                                      DRM_DWORD   f_dwClientSdkType,
    __in                                      DRM_DWORD   f_dwPlatform,
    __in                                      DRM_DWORD   f_dwTransformVersion,
    __in_bcount( f_cbPrimaryPayload )   const DRM_BYTE   *f_pbPrimaryPayload,
    __in                                      DRM_DWORD   f_cbPrimaryPayload,
    __in_ecount_opt( f_cbAuxPayload )   const DRM_BYTE   *f_pbAuxPayload,
    __in                                      DRM_DWORD   f_cbAuxPayload,
    __inout_bcount_opt( *f_pcbChallenge )     DRM_BYTE   *f_pbChallenge,
    __inout                                   DRM_DWORD  *f_pcbChallenge )
{
    UNREFERENCED_PARAMETER(f_pbPrimaryPayload);
    UNREFERENCED_PARAMETER(f_cbPrimaryPayload);
    UNREFERENCED_PARAMETER(f_pbAuxPayload);
    UNREFERENCED_PARAMETER(f_cbAuxPayload);
    UNREFERENCED_PARAMETER(f_dwClientSdkVersion);
    UNREFERENCED_PARAMETER(f_dwClientSdkType);
    UNREFERENCED_PARAMETER(f_dwPlatform);
    UNREFERENCED_PARAMETER(f_dwTransformVersion);
    UNREFERENCED_PARAMETER(f_pbChallenge);
    UNREFERENCED_PARAMETER(f_pcbChallenge);

    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_ACT_ProcessResponseGenerateChallenge(
    __inout_bcount( ACTIVATION_CONTEXT_SIZE )  DRM_BYTE        *f_pbActivationCtx,
    __in_bcount_opt( f_cbResponse )            DRM_BYTE        *f_pbResponse,
    __in_opt                                   DRM_DWORD        f_cbResponse,
    __out_bcount_opt( *f_pcbChallenge )        DRM_BYTE        *f_pbChallenge,
    __inout                                    DRM_DWORD       *f_pcbChallenge,
    __out_ecount_opt( *f_pcchUrl )             DRM_CHAR        *f_pszUrl,
    __inout                                    DRM_DWORD       *f_pcchUrl,
    __out                                      DRM_RESULT      *f_pResult )
{
    UNREFERENCED_PARAMETER( f_pbActivationCtx );
    UNREFERENCED_PARAMETER( f_pbResponse );
    UNREFERENCED_PARAMETER( f_cbResponse );
    UNREFERENCED_PARAMETER( f_pbChallenge );
    UNREFERENCED_PARAMETER( f_pcbChallenge );
    UNREFERENCED_PARAMETER( f_pszUrl );
    UNREFERENCED_PARAMETER( f_pcchUrl );
    UNREFERENCED_PARAMETER( f_pResult );

    return DRM_E_NOTIMPL;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_ACT_ProcessResponseInternal(
    __in                                     DRM_ANSI_CONST_STRING *f_pdastrResponse,
    __inout_bcount( *f_pcbPrimaryPayload )   DRM_BYTE              *f_pbPrimaryPayload,
    __inout                                  DRM_DWORD             *f_pcbPrimaryPayload,
    __out                                    DRM_ANSI_STRING       *f_pdastrAuxPayloadUrl )
{
    UNREFERENCED_PARAMETER( f_pdastrResponse );
    UNREFERENCED_PARAMETER( f_pbPrimaryPayload );
    UNREFERENCED_PARAMETER( f_pcbPrimaryPayload );
    UNREFERENCED_PARAMETER( f_pdastrAuxPayloadUrl );

    return DRM_E_NOTIMPL;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_ACT_VerifyAppData(
    __in_ecount(f_cbAppId) const DRM_BYTE  *f_pbAppId,
    __in                         DRM_DWORD  f_cbAppId,
    __in                         DRM_DWORD  f_dwAppVersion )
{
    UNREFERENCED_PARAMETER(f_pbAppId);
    UNREFERENCED_PARAMETER(f_cbAppId);
    UNREFERENCED_PARAMETER(f_dwAppVersion);

    return DRM_E_NOTIMPL;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_ACT_ProcessWSDLResponseInternal(
    __in                                    DRM_ANSI_CONST_STRING  *f_pdastrResponse,
    __out                                   DRM_ANSI_STRING        *f_pdastrActivationUrl )
{
    UNREFERENCED_PARAMETER(f_pdastrResponse);
    UNREFERENCED_PARAMETER(f_pdastrActivationUrl);

    return DRM_E_NOTIMPL;
}

PREFAST_POP

EXIT_PK_NAMESPACE_CODE;

