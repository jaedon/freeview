/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmdevicerevocation.h>

ENTER_PK_NAMESPACE_CODE;

PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_NONCONST_BUFFER_PARAM, "Changing parameters to const to satisfy warning would make them not match the real interface.")

DRM_NO_INLINE DRM_RESULT DRM_RVK_UpdateRevocationListDecoded(
    IN          DRM_CRYPTO_CONTEXT   *f_pcontextCrypto,
    IN          DRM_REVOCATIONSTORE_CONTEXT *f_pContextRev,
    IN          DRM_BYTE             *f_pbRevocationList,
    IN          DRM_DWORD             f_cbRevocationList,
    IN  const   DRM_BYTE             *f_pbRevocationBuffer,
    IN          DRM_DWORD             f_cbRevocationBuffer,
    IN  const   DRM_GUID             *f_pGUID,
    IN  const   PUBKEY               *f_ppubkey,
    OUT         DRM_BOOL             *f_pfUpdated)
{
    UNREFERENCED_PARAMETER( f_pcontextCrypto );
    UNREFERENCED_PARAMETER( f_pContextRev );
    UNREFERENCED_PARAMETER( f_pbRevocationList );
    UNREFERENCED_PARAMETER( f_cbRevocationList );
    UNREFERENCED_PARAMETER( f_pbRevocationBuffer );
    UNREFERENCED_PARAMETER( f_cbRevocationBuffer );
    UNREFERENCED_PARAMETER( f_pGUID );
    UNREFERENCED_PARAMETER( f_ppubkey );
    UNREFERENCED_PARAMETER( f_pfUpdated );
    return DRM_E_NOTIMPL;
}

DRM_RESULT DRM_RVK_UpdateRevocationList(
    IN                                                                                   DRM_CRYPTO_CONTEXT          *f_pcontextCrypto,
    IN                                                                                   DRM_REVOCATIONSTORE_CONTEXT *f_pContextRev,
    __in_ecount( f_pdasstrRevocationList->m_ich + f_pdasstrRevocationList->m_cch )       DRM_CHAR                    *f_rgchBase,
    IN                                                                             const DRM_SUBSTRING               *f_pdasstrRevocationList,
    __in_bcount( f_cbRevocationBuffer )                                            const DRM_BYTE                    *f_pbRevocationBuffer,
    IN                                                                                   DRM_DWORD                    f_cbRevocationBuffer,
    IN                                                                             const DRM_GUID                    *f_pGUID,
    IN                                                                             const PUBKEY                      *f_ppubkey )
{
    UNREFERENCED_PARAMETER( f_pcontextCrypto );
    UNREFERENCED_PARAMETER( f_pContextRev );
    UNREFERENCED_PARAMETER( f_rgchBase );
    UNREFERENCED_PARAMETER( f_pdasstrRevocationList );
    UNREFERENCED_PARAMETER( f_pbRevocationBuffer );
    UNREFERENCED_PARAMETER( f_cbRevocationBuffer );
    UNREFERENCED_PARAMETER( f_pGUID );
    UNREFERENCED_PARAMETER( f_ppubkey );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_RVK_GetDeviceRevocationList(
    IN                                             DRM_CRYPTO_CONTEXT          *f_pcontextCRYP,
    IN                                             DRM_REVOCATIONSTORE_CONTEXT *f_pContextRev,
    __out_bcount_opt( *f_pcbRevocationData ) const DRM_BYTE                    *f_pbRevocationData,
    IN OUT                                         DRM_DWORD                   *f_pcbRevocationData,
    OUT                                            DRM_DWORD                   *f_pidSerial )
{
    UNREFERENCED_PARAMETER( f_pcontextCRYP );
    UNREFERENCED_PARAMETER( f_pContextRev );
    UNREFERENCED_PARAMETER( f_pbRevocationData );
    UNREFERENCED_PARAMETER( f_pcbRevocationData );
    UNREFERENCED_PARAMETER( f_pidSerial );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_RVK_CheckDeviceRevocation(
    __in  const DRM_CONST_STRING *f_pdstrDevCert,
    __in  const DRM_CONST_STRING *f_pdstrXMLExclusion,
    __out       DRM_BOOL          *f_pfExcluded )
{
    UNREFERENCED_PARAMETER( f_pdstrDevCert );
    UNREFERENCED_PARAMETER( f_pdstrXMLExclusion );
    UNREFERENCED_PARAMETER( f_pfExcluded );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_DEVICEREVOCATION_IsDeviceRevoked(
    __in                      const DRM_CONST_STRING *f_pdstrDeviceCert,
    __in_bcount(f_ccbCRLSize) const DRM_BYTE         *f_pbCRLData,
    __in                      const DRM_DWORD         f_ccbCRLSize )
{
    UNREFERENCED_PARAMETER( f_pdstrDeviceCert );
    UNREFERENCED_PARAMETER( f_pbCRLData );
    UNREFERENCED_PARAMETER( f_ccbCRLSize );
    return DRM_E_NOTIMPL;
}

PREFAST_POP

EXIT_PK_NAMESPACE_CODE;

