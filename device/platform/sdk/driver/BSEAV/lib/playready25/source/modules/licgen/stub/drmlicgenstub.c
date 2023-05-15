/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmlicgen.h>

ENTER_PK_NAMESPACE_CODE;

DRM_API DRM_NO_INLINE DRM_BOOL DRM_CALL DRM_LICGEN_IsLicGenSupported(DRM_VOID) { return FALSE; }
DRM_API DRM_NO_INLINE DRM_BOOL DRM_CALL DRM_LICGEN_IsLicGenUnsupported(DRM_VOID) { return TRUE; }

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL Drm_LocalLicense_InitializePolicyDescriptor(
    __inout                             DRM_LOCAL_LICENSE_POLICY_DESCRIPTOR *f_poDescriptor )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL Drm_LocalLicense_CreateLicense(
    __inout                               DRM_APP_CONTEXT                     *f_poAppContext,
    __in                            const DRM_LOCAL_LICENSE_POLICY_DESCRIPTOR *f_poDescriptor,
    __in                            const DRM_LOCAL_LICENSE_TYPE               f_eLicensType,
    __in                            const DRM_KID                             *f_pKeyId,
    __in_opt                        const DRM_DWORD                            f_cbRemoteCert,
    __in_ecount_opt(f_cbRemoteCert) const DRM_BYTE                            *f_pbRemoteCert,
    __in_opt                        const DRM_LICENSE_HANDLE                   f_hRootLicense,
    __out                                 DRM_LICENSE_HANDLE                  *f_phLicense )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL Drm_LocalLicense_CreatePlayReadyObject(
    __in_opt                      const DRM_LICENSE_HANDLE                   f_hLicense,
    __in_opt                      const DRM_DWORD                            f_cbRMHeader,
    __in_opt                      const DRM_BYTE                            *f_pbRMHeader,
    __out                               DRM_DWORD                           *f_pcbPROBlob,
    __out_bcount( *f_pcbPROBlob )       DRM_BYTE                           **f_ppbPROBlob )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL Drm_LocalLicense_CreateRMHeader(
    __in_opt                      const DRM_KID                             *f_pKeyId,
    __out                               DRM_DWORD                           *f_pcbRMHeader,
    __out_bcount( *f_pcbRMHeader )      DRM_BYTE                           **f_ppbRMHeader )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL Drm_LocalLicense_StoreLicense(
    __in                          const DRM_LICENSE_HANDLE                   f_hLicense,
    __in                          const DRM_LOCAL_LICENSE_STORE              f_eLicenseStore  )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL Drm_LocalLicense_EncryptSample(
    __in                          const DRM_LICENSE_HANDLE                   f_hLicense,
    __in                          const DRM_DWORD                            f_cSubsamples,
    __in_ecount( f_cSubsamples )  const DRM_DWORD                           *f_pcbSubamples,
    __inout_ecount( f_cSubsamples )     DRM_BYTE                           **f_ppbSubamples,
    __out                               DRM_UINT64                          *f_pqwIV )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL Drm_LocalLicense_EncryptOpaqueSample(
    __in                          const DRM_LICENSE_HANDLE               f_hLicense,
    __out_ecount_opt(1)                 OEM_HAL_SAMPLE_METADATA         *f_pMetadata,
    __in                                OEM_OPAQUE_BUFFER_HANDLE         f_hInBuf,
    __in                                OEM_OPAQUE_BUFFER_HANDLE         f_hOutBuf,
    __in                                DRM_DWORD                        f_cbData,
    __out                               DRM_UINT64                      *f_pqwIV )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL Drm_LocalLicense_GetKID(
    __in                          const DRM_LICENSE_HANDLE                   f_hLicense,
    __out                               DRM_KID                             *f_pKeyID )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL Drm_LocalLicense_GetXMRLicense(
    __in                          const DRM_LICENSE_HANDLE                   f_hLicense,
    __out                               DRM_DWORD                           *f_pcbXMRLicense,
    __out                               DRM_BYTE                           **f_ppbXMRLicense )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL Drm_LocalLicense_AddRef (
    __inout                             DRM_LICENSE_HANDLE                   f_hLicense )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL Drm_LocalLicense_Release(
    __inout                             DRM_LICENSE_HANDLE                  *f_phLicense )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_LOCALLICENSE_CleanCache(
    __inout                             DRM_APP_CONTEXT                     *f_poAppContext )
{
    return DRM_E_NOTIMPL;
}

EXIT_PK_NAMESPACE_CODE;
