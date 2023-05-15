#ifndef __TEEOEMHALSTRUCTS_H__
#define __TEEOEMHALSTRUCTS_H__

#include <stdlib.h>
#include <stdio.h>
#include <memory.h>

#include "oemhaldefs.h"
#include "oemhal.h"

typedef struct {
    DRM_DWORD                  f_cbMessage;
    DRM_DWORD                  f_cbSignature;
    OEM_HAL_HASH_TYPE          f_eHashType;
    OEM_HAL_SIGNATURE_SCHEME   f_eSignatureScheme;
    OEM_HAL_KEY_REGISTER_INDEX f_indexIntegrityKey;
} nfTeeDrmPRVerifyMessageSignature_t;

typedef struct {
    DRM_DWORD                  f_cbMessage;
    DRM_DWORD                  f_cbSignature;
    OEM_HAL_HASH_TYPE          f_eHashType;
    OEM_HAL_SIGNATURE_SCHEME   f_eSignatureScheme;
    OEM_HAL_KEY_TYPE           f_eKeyType;
    OEM_HAL_KEY_REGISTER_INDEX f_indexIntegrityKey;
} nfTeeDrmPRSignWithPrivateKey_t;

typedef struct {
    DRM_DWORD                  f_cbData;
    DRM_DWORD                  f_cbSignature;
    OEM_HAL_KEY_TYPE           f_eKeyType;
    OEM_HAL_KEY_REGISTER_INDEX f_indexKey;
} nfTeeDrmPRVerifyOMAC1Signature_t;

typedef struct {
    DRM_DWORD                  f_cbData;
    DRM_DWORD                  f_cbSignature;
    OEM_HAL_KEY_TYPE f_eKeyType;
    OEM_HAL_KEY_REGISTER_INDEX f_indexKey;
} nfTeeDrmPRCreateOMAC1Signature_t;

typedef struct {
    DRM_DWORD f_cbBlock;
    DRM_DWORD f_cbEncryptedBlock;
    OEM_HAL_KEY_TYPE f_eKeyType;
    OEM_HAL_KEY_REGISTER_INDEX f_indexMessageEncryptKey;
} nfTeeDrmPREncryptDataBlock_t;

#define WRAPPED_KEY_WORK_BUF_SIZE 1024
typedef struct {
    OEM_HAL_KEY_TYPE            f_eKeyType;
    OEM_HAL_KEY_REGISTER_INDEX  f_indexKey;
    OEM_HAL_KEY_TYPE            f_eWrappingKeyType;
    OEM_HAL_KEY_REGISTER_INDEX  f_indexWrappingKey;
    DRM_DWORD                   f_cbWrappedKeyData;
    DRM_DWORD                   f_cbParameterData;
    DRM_BYTE                    f_pbEncryptedKey[WRAPPED_KEY_WORK_BUF_SIZE];
} nfTeeDrmPRUnwrapKey_t;

typedef struct {
    OEM_HAL_KEY_TYPE            f_eKeyType;
    OEM_HAL_KEY_REGISTER_INDEX  f_indexKey;
    OEM_HAL_KEY_TYPE            f_eWrappingKeyType;
    OEM_HAL_KEY_REGISTER_INDEX  f_indexWrappingKey;
    DRM_DWORD                   f_cbWrappedKey;
    DRM_BYTE                    f_pbEncryptedKey[WRAPPED_KEY_WORK_BUF_SIZE];
} nfTeeDrmPRWrapKey_t;

typedef struct {
    DRM_DWORD                   f_cbData;
    OEM_HAL_KEY_TYPE            f_eKeyType;
    OEM_HAL_KEY_REGISTER_INDEX  f_indexContentKey;
    DRM_UINT64                  f_qwInitializationVector;
    DRM_UINT64                  f_qwSampleByteOffset;
} nfTeeDrmPRDecryptContent_t;

typedef struct {
    DRM_DWORD                   f_cbData;
    OEM_HAL_KEY_TYPE            f_eKeyType;
    OEM_HAL_KEY_REGISTER_INDEX  f_indexContentKey;
    DRM_UINT64                  f_qwInitializationVector;
    DRM_UINT64                  f_qwSampleByteOffset;
} nfTeeDrmPRDecryptContentOpaque_t;

typedef struct {
    OEM_HAL_KEY_TYPE                f_eKeyType;
    OEM_HAL_KEY_REGISTER_INDEX      f_indexKey;
    OEM_HAL_SAMPLE_ENCRYPTION_MODE  f_eSampleEncryptionMode;
} nfTeeDrmPRCreateSampleEncryptionContext_t;

typedef struct {
    DRM_DWORD                   f_cbRevocationInfo;
    OEM_HAL_KEY_REGISTER_INDEX  f_indexSigningKey ;
} nfTeeDrmPRLoadPlayReadyRevocationInfo_t;

typedef struct {
    DRM_DWORD                   f_cbCRL;
    OEM_HAL_KEY_REGISTER_INDEX  f_indexSigningKey;
} nfTeeDrmPRLoadPlayReadyCrl_t;

#endif
