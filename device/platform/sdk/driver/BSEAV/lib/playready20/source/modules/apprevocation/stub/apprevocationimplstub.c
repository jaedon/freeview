/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmapprevocation.h>

ENTER_PK_NAMESPACE_CODE;

PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_NONCONST_BUFFER_PARAM, "Changing parameters to const to satisfy warning would make them not match the real interface.")

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_RVK_GetCurrentAppRevocationList(
    IN      DRM_REVOCATIONSTORE_CONTEXT *f_pContextRev,
    __in_bcount( *f_pcbBuffer ) DRM_BYTE *f_pbBuffer,
    IN  OUT DRM_DWORD            *f_pcbBuffer,
    OUT     DRM_DWORD            *f_pidCRLApp)
{
    UNREFERENCED_PARAMETER( f_pContextRev );
    UNREFERENCED_PARAMETER( f_pbBuffer );
    UNREFERENCED_PARAMETER( f_pcbBuffer );
    UNREFERENCED_PARAMETER( f_pidCRLApp );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_RVK_PerformAppRevocationCheck(
    IN     DRM_CRYPTO_CONTEXT          *f_pContextCrypto,
    IN     DRM_REVOCATIONSTORE_CONTEXT *f_pContextRev,
    IN     DRM_CONST_STRING            *f_pdstrContentLicense,
    IN     APPCERT                     *f_pAppCert,
    __in_bcount( f_cbRevocationBuffer ) DRM_BYTE *f_pbRevocationBuffer,
    IN     DRM_DWORD                    f_cbRevocationBuffer,
    IN OUT DRM_REVLIST_ID_VALUES       *f_pRevListIDs )
{
    UNREFERENCED_PARAMETER( f_pContextCrypto );
    UNREFERENCED_PARAMETER( f_pContextRev );
    UNREFERENCED_PARAMETER( f_pdstrContentLicense );
    UNREFERENCED_PARAMETER( f_pAppCert );
    UNREFERENCED_PARAMETER( f_pbRevocationBuffer );
    UNREFERENCED_PARAMETER( f_cbRevocationBuffer );
    UNREFERENCED_PARAMETER( f_pRevListIDs );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_RVK_CheckAppCertForRevocation(
    __in_bcount( f_cbAppCRL ) DRM_BYTE *f_pbAppCRL,
    DRM_DWORD             f_cbAppCRL,
    APPCERT              *f_pappcert)
{
    UNREFERENCED_PARAMETER( f_pbAppCRL );
    UNREFERENCED_PARAMETER( f_cbAppCRL );
    UNREFERENCED_PARAMETER( f_pappcert );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_RVK_UpdateAppRevocationListA(
        DRM_CRYPTO_CONTEXT   *f_pcontextCRY,
        DRM_REVOCATIONSTORE_CONTEXT * f_pContextRev,
        __in_bcount( f_cbBuffer ) DRM_BYTE *f_pbBuffer,
        DRM_DWORD             f_cbBuffer,
        __in_ecount( f_pdasstrRevocationList->m_ich + f_pdasstrRevocationList->m_cch ) DRM_CHAR *f_rgchBase,
  const DRM_SUBSTRING        *f_pdasstrRevocationList,
        APPCERT              *f_pappcert,
        DRM_DWORD             f_idCRLAppCurrent,
        DRM_BOOL             *f_pfUpdatedAppRevocationList)
{
    UNREFERENCED_PARAMETER( f_pcontextCRY );
    UNREFERENCED_PARAMETER( f_pContextRev );
    UNREFERENCED_PARAMETER( f_pbBuffer );
    UNREFERENCED_PARAMETER( f_cbBuffer );
    UNREFERENCED_PARAMETER( f_rgchBase );
    UNREFERENCED_PARAMETER( f_pdasstrRevocationList );
    UNREFERENCED_PARAMETER( f_pappcert );
    UNREFERENCED_PARAMETER( f_idCRLAppCurrent );
    UNREFERENCED_PARAMETER( f_pfUpdatedAppRevocationList );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_RVK_UpdateAppRevocationListW(
    DRM_CRYPTO_CONTEXT   *f_pcontextCRY,
    DRM_REVOCATIONSTORE_CONTEXT *f_pContextRev,
    __in_bcount( f_cbBuffer ) DRM_BYTE *f_pbBuffer,
    DRM_DWORD             f_cbBuffer,
    DRM_CONST_STRING     *f_pdstrRevocationList,
    APPCERT              *f_pappcert,
    DRM_DWORD             f_idCRLAppCurrent,
    DRM_BOOL             *f_pfUpdatedAppRevocationList,
    OUT DRM_DWORD        *f_pdwVersion)
{
    UNREFERENCED_PARAMETER( f_pcontextCRY );
    UNREFERENCED_PARAMETER( f_pContextRev );
    UNREFERENCED_PARAMETER( f_pbBuffer );
    UNREFERENCED_PARAMETER( f_cbBuffer );
    UNREFERENCED_PARAMETER( f_pdstrRevocationList );
    UNREFERENCED_PARAMETER( f_pappcert );
    UNREFERENCED_PARAMETER( f_idCRLAppCurrent );
    UNREFERENCED_PARAMETER( f_pfUpdatedAppRevocationList );
    UNREFERENCED_PARAMETER( f_pdwVersion );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_NO_INLINE DRM_CALL DRM_RVK_UpdateAppRevocationListDecoded(
    DRM_CRYPTO_CONTEXT   *f_pcontextCRY,
    DRM_REVOCATIONSTORE_CONTEXT *f_pContextRev,
    __in_bcount( f_cbRevocationList ) DRM_BYTE *f_pbRevocationList,
    DRM_DWORD             f_cbRevocationList,
    APPCERT              *f_pappcert,
    DRM_DWORD             f_idCRLAppCurrent,
    DRM_BOOL             *f_pfUpdatedAppRevocationList,
    OUT DRM_DWORD        *f_pdwVersion)
{
    UNREFERENCED_PARAMETER( f_pcontextCRY );
    UNREFERENCED_PARAMETER( f_pContextRev );
    UNREFERENCED_PARAMETER( f_pbRevocationList );
    UNREFERENCED_PARAMETER( f_cbRevocationList );
    UNREFERENCED_PARAMETER( f_pappcert );
    UNREFERENCED_PARAMETER( f_idCRLAppCurrent );
    UNREFERENCED_PARAMETER( f_pfUpdatedAppRevocationList );
    UNREFERENCED_PARAMETER( f_pdwVersion );
    return DRM_E_NOTIMPL;
}

PREFAST_POP

EXIT_PK_NAMESPACE_CODE;

