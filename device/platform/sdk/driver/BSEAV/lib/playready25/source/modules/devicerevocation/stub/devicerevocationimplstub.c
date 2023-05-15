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
PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_NONCONST_PARAM, "Changing parameters to const to satisfy warning would make them not match the real interface.")

DRM_NO_INLINE DRM_RESULT DRM_RVK_UpdateRevocationListDecoded(
    __in                                   DRM_CRYPTO_CONTEXT          *f_pcontextCrypto,
    __in                                   DRM_REVOCATIONSTORE_CONTEXT *f_pContextRev,
    __in_ecount(f_cbRevocationList)        DRM_BYTE                    *f_pbRevocationList,
    __in                                   DRM_DWORD                    f_cbRevocationList,
    __in                             const DRM_BYTE                    *f_pbRevocationBuffer,
    __in                                   DRM_DWORD                    f_cbRevocationBuffer,
    __in                             const DRM_GUID                    *f_pGUID,
    __in                             const PUBKEY                      *f_ppubkey,
    __out_opt                              DRM_BOOL                    *f_pfUpdated )
{
    CLAW_AUTO_RANDOM_CIPHER

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
    __in                                                                                    DRM_CRYPTO_CONTEXT          *f_pcontextCrypto,
    __in                                                                                    DRM_REVOCATIONSTORE_CONTEXT *f_pContextRev,
PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_COUNT_REQUIRED_FOR_WRITABLE_BUFFER, "f_rgchBase length defined by f_pdasstrRevocationList")
    __inout_ecount( f_pdasstrRevocationList->m_ich + f_pdasstrRevocationList->m_cch )       DRM_CHAR                    *f_rgchBase,
PREFAST_POP
    __in                                                                              const DRM_SUBSTRING               *f_pdasstrRevocationList,
    __in_bcount( f_cbRevocationBuffer )                                               const DRM_BYTE                    *f_pbRevocationBuffer,
    __in                                                                                    DRM_DWORD                    f_cbRevocationBuffer,
    __in                                                                              const DRM_GUID                    *f_pGUID,
    __in                                                                              const PUBKEY                      *f_ppubkey )
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
    __in                                           DRM_CRYPTO_CONTEXT          *f_pcontextCRYP,
    __in                                           DRM_REVOCATIONSTORE_CONTEXT *f_pContextRev,
    __out_bcount_opt( *f_pcbRevocationData ) const DRM_BYTE                    *f_pbRevocationData,
    __inout                                        DRM_DWORD                   *f_pcbRevocationData,
    __out                                          DRM_DWORD                   *f_pidSerial )
{
    CLAW_AUTO_RANDOM_CIPHER

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

PREFAST_POP /* Changing parameters to const to satisfy warning would make them not match the real interface. */
PREFAST_POP /* Changing parameters to const to satisfy warning would make them not match the real interface. */

EXIT_PK_NAMESPACE_CODE;

