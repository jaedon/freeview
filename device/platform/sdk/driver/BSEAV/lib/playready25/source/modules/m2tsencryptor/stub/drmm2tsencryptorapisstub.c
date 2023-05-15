/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmm2tsencryptor.h>

ENTER_PK_NAMESPACE_CODE;

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL Drm_M2ts_Initialize(
    __in                              const DRM_DWORD                        f_cbMaxHeapAllowed,
    __out                                   DRM_M2TS_ENCRYPTOR_HANDLE       *f_phEncryptor )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL Drm_M2ts_Uninitialize(
    __in                                    DRM_M2TS_ENCRYPTOR_HANDLE       f_phEncryptor )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL Drm_M2ts_SetEncryptionProperty(
        __in                                DRM_M2TS_ENCRYPTOR_HANDLE        f_hEncryptor,
        __in                          const DRM_M2TS_ENCRYPTION_PROPERTY    *f_poProperty )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL Drm_M2ts_SetKeyData(
    __in                                    DRM_M2TS_ENCRYPTOR_HANDLE        f_phEncryptor,
    __in                                    DRM_LICENSE_HANDLE               f_hLicense,
    __in                              const DRM_GUID                        *f_poKeyId,
    __in                              const DRM_M2TS_PRO_TYPE                f_ePROType,
    __in                              const DRM_DWORD                        f_dwDuration)
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL Drm_M2ts_Encrypt(
    __in                                    DRM_M2TS_ENCRYPTOR_HANDLE        f_hEncryptor,
    __in_bcount_opt( f_cbDataIn )     const DRM_BYTE                        *f_pbDataIn,
    __in                              const DRM_DWORD                        f_cbDataIn,
    __out_bcount( *f_pcbDataOut )           DRM_BYTE                        *f_pbDataOut,
    __inout                                 DRM_DWORD                       *f_pcbDataOut,
    __in                              const DRM_BOOL                         f_fLastTSPackets )
{
    return DRM_E_NOTIMPL;
}
    
EXIT_PK_NAMESPACE_CODE;

