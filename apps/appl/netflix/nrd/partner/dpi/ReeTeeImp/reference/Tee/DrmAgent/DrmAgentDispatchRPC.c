#include "TeeLog.h"
#include "oemhaldefs.h"
#include "oemhal.h"
#include "teeoemhal.h"
#include "teeoemhalstructs.h"

#include "TeeTypes.h"
#include "CommTypes.h"
#include "TeeDrmTypes.h"
#include "TeeDebug.h"
#include "TeeInternalDpi.h"
#include "AgentApi.h"
#include "DrmAgent.h"

#define MODULE "DrmAgentDispatchRPC"
/*
 * The following return values can be used:
 *  nfTGR_INTERNAL_ERROR,
 *  nfTGR_INVALID_PARAM_COUNT,
 *  nfTGR_INVALID_PARM0,
 *  nfTGR_INVALID_PARM1,
 *  nfTGR_INVALID_PARM2,
 *  nfTGR_INVALID_PARM3,
 */

/*
 * Get reference to the WRITE_ONLY or READ_WRITE variables
 */
static uint32_t getValRef(nfAgentApiParameter_t params[], nfAgentApiPermission_t perm, uint32_t size, uint32_t index, uint32_t ** ppa, uint32_t ** ppb)
{
    uint32_t result = 0;

    if ((index < size) && (size <= MAXIMUM_NUMBER_OF_RPC_PARAMETERS)) {
        if (params[index].Type == nfAAPT_VALUE) {
            if (params[index].Permission == perm) {
                nfRPCValue_t * v = &params[index].data.Value;
                if (ppa) *ppa = &v->a;
                if (ppb) *ppb = &v->b;
                DRMDBG("%s:%s[%d] --> ( %d %d )", MODULE, __func__, index, v->a, v->b);
            } else {
                DRMERR("%s:%s : wrong parameter [%d] permission %08x, expected %08x", MODULE, __func__, index, params[index].Permission, perm);
                result = (index == 0) ? nfTGR_INVALID_PARM0 : (index == 1) ? nfTGR_INVALID_PARM1 : (index == 2) ? nfTGR_INVALID_PARM2 : nfTGR_INVALID_PARM3;
                goto Exit;
            }
        } else {
            DRMERR("%s:%s : wrong parameter [%d] type %08x, expected %08x", MODULE, __func__, index, params[index].Type, nfAAPT_VALUE);
            result = (index == 0) ? nfTGR_INVALID_PARM0 : (index == 1) ? nfTGR_INVALID_PARM1 : (index == 2) ? nfTGR_INVALID_PARM2 : nfTGR_INVALID_PARM3;
            goto Exit;
        }
    } else {
        DRMERR("%s:%s : wrong parameter count %d, index %d", MODULE, __func__, size, index);
        result = nfTGR_INVALID_PARAM_COUNT;
        goto Exit;
    }

    Exit:
    return result;
}

/*
 * Copy values of READ_ONLY values
 */
static uint32_t getVal(nfAgentApiParameter_t params[], nfAgentApiPermission_t perm, uint32_t size, uint32_t index, uint32_t * pa, uint32_t * pb)
{
    uint32_t result = 0;

    if ((index < size) && (size <= MAXIMUM_NUMBER_OF_RPC_PARAMETERS)) {
        if (params[index].Type == nfAAPT_VALUE) {
            if (params[index].Permission == perm) {
                nfRPCValue_t * v = &params[index].data.Value;
                if (pa) *pa = v->a;
                if (pb) *pb = v->b;
                DRMDBG("%s:%s[%d] --> ( %d %d )", MODULE, __func__, index, v->a, v->b);
            } else {
                DRMERR("%s:%s : wrong parameter [%d] permission %08x, expected %08x", MODULE, __func__, index, params[index].Permission, perm);
                result = (index == 0) ? nfTGR_INVALID_PARM0 : (index == 1) ? nfTGR_INVALID_PARM1 : (index == 2) ? nfTGR_INVALID_PARM2 : nfTGR_INVALID_PARM3;
                goto Exit;
            }
        } else {
            DRMERR("%s:%s : wrong parameter [%d] type %08x, expected %08x", MODULE, __func__, index, params[index].Type, nfAAPT_VALUE);
            result = (index == 0) ? nfTGR_INVALID_PARM0 : (index == 1) ? nfTGR_INVALID_PARM1 : (index == 2) ? nfTGR_INVALID_PARM2 : nfTGR_INVALID_PARM3;
            goto Exit;
        }
    } else {
        DRMERR("%s:%s : wrong parameter count %d, index %d", MODULE, __func__, size, index);
        result = nfTGR_INVALID_PARAM_COUNT;
        goto Exit;
    }

    Exit:
    return result;
}

/*
 * TBD - checking validity of memory reference. May do some validation based on permissions (read-only vs write-only vs read-write)
 */
static uint32_t memValidate(const DRM_BYTE *memPtr, uint32_t memSize, nfAgentApiPermission_t perm) {
    (void)memPtr; // TBD
    (void)memSize; // TBD
    (void)perm; // TBD
    uint32_t result = 0;
    // TBD
    return result;
}

static uint32_t getMem(
                    nfAgentApiParameter_t params[],
                    nfAgentApiPermission_t perm,
                    uint32_t size,
                    uint32_t index,
                    DRM_BYTE **memPtr,
                    uint32_t *memSize)
{
    uint32_t result = 0;

    if ((index < size) && (size <= MAXIMUM_NUMBER_OF_RPC_PARAMETERS)) {
        if (params[index].Type == nfAAPT_MEMREF) {
            if (params[index].Permission == perm) {
                nfRPCMemoryReference_t * m = &params[index].data.MemRef;
                if (memPtr) *memPtr = m->memPtr;
                if (memSize) *memSize = m->memSize;
                DRMDBG("%s:%s[%d] --> { %p %d }", MODULE, __func__, index, m->memPtr, m->memSize);
                result = memValidate(*memPtr, *memSize, perm);
            } else {
                DRMERR("%s:%s: wrong parameter [%d] permission %08x, expected %08x", MODULE, __func__, index, params[index].Permission, perm);
                result = (index == 0) ? nfTGR_INVALID_PARM0 : (index == 1) ? nfTGR_INVALID_PARM1 : (index == 2) ? nfTGR_INVALID_PARM2 : nfTGR_INVALID_PARM3;
                goto Exit;
            }
        } else {
            DRMERR("%s:%s : wrong parameter [%d] type %08x, expected %08x", MODULE, __func__, index, params[index].Type, nfAAPT_MEMREF);
            result = (index == 0) ? nfTGR_INVALID_PARM0 : (index == 1) ? nfTGR_INVALID_PARM1 : (index == 2) ? nfTGR_INVALID_PARM2 : nfTGR_INVALID_PARM3;
            goto Exit;
        }
    } else {
        DRMERR("%s:%s : wrong parameter count %d, index %d", MODULE, __func__, size, index);
        result = nfTGR_INVALID_PARAM_COUNT;
        goto Exit;
    }

    Exit:
    return result;
}

static uint32_t getInMem(
                    nfAgentApiParameter_t params[],
                    nfAgentApiPermission_t perm,
                    uint32_t size,
                    uint32_t index,
              const DRM_BYTE **memPtr,
                    uint32_t *memSize)
{
    return getMem(params, perm, size, index, (DRM_BYTE **)memPtr, memSize);
}

static uint32_t getInMemFixed(
                    nfAgentApiParameter_t params[],
                    nfAgentApiPermission_t perm,
                    uint32_t size,
                    uint32_t index,
              const DRM_BYTE **memPtr,
                    uint32_t memSize)
{
    uint32_t result = 0;
    uint32_t actualSize = 0;

    result = getMem(params, perm, size, index, (DRM_BYTE **)memPtr, &actualSize);

    if (result) {
        if (actualSize != memSize) {
            result = (index == 0) ? nfTGR_INVALID_PARM0 : (index == 1) ? nfTGR_INVALID_PARM1 : (index == 2) ? nfTGR_INVALID_PARM2 : nfTGR_INVALID_PARM3;
        }
    }

    return result;
}

static int isDrmEncryptedKey(
    OEM_HAL_KEY_TYPE f_eWrappingKeyType,
    const DRM_BYTE  *f_pbWrappedKey,
    DRM_DWORD  f_cbWrappedKey,
    DRM_BOOL *isEncryptedKey)
{
    int result = 0;
    *isEncryptedKey = 0;

    if (f_eWrappingKeyType == OEM_HAL_KEY_TYPE_AES_KEYFILE) {
        TEE_TRACE("%s:%s:%d - get DRM_ENCRYPTED_KEY", MODULE, __func__, __LINE__);
        if (f_cbWrappedKey == sizeof(DRM_ENCRYPTED_KEY)) {
            DRM_ENCRYPTED_KEY *key = (DRM_ENCRYPTED_KEY *)f_pbWrappedKey;
            TEE_TRACE("data ptr [%p %d]", key->pbEncryptedKey, key->cbEncryptedKey);
            if (key->cbEncryptedKey > WRAPPED_KEY_WORK_BUF_SIZE) {
                teeLog(Tee_Error, "**** %s: PANIC: %d", __func__, __LINE__);
                result = 1;
            }
            *isEncryptedKey = 1;
        } else {
            teeLog(Tee_Error, "**** %s: PANIC: %d", __func__, __LINE__);
            result = 1;
        }
    }

    return result;
}

static int isXmrUnwrapInfo(
    OEM_HAL_KEY_TYPE f_eWrappingKeyType,
    const DRM_BYTE  *f_pbWrappedKey,
    DRM_DWORD  f_cbWrappedKey,
    DRM_BOOL *isXmrUnwrapInfoKey)
{
    int result = 0;
    *isXmrUnwrapInfoKey = 0;

    if (f_eWrappingKeyType == OEM_HAL_KEY_TYPE_ECC_P256_CONTENT_ENCRYPT) {
        TEE_TRACE("%s:%s:%d - get DRM_XMR_UNWRAP_INFO", MODULE, __func__, __LINE__);
        if (f_cbWrappedKey == sizeof(DRM_XMR_UNWRAP_INFO))
        {
            const DRM_XMR_UNWRAP_INFO* pStruct = (DRM_XMR_UNWRAP_INFO*)f_pbWrappedKey;
            if (pStruct->pbXMRLic != NULL) {
                int size = pStruct->cbXMRLic;
                if (pStruct->poChecksum) {
                    size += sizeof(DRM_BB_CHECKSUM_CONTEXT);
                }
                if (size > WRAPPED_KEY_WORK_BUF_SIZE) {
                    teeLog(Tee_Error, "**** %s: PANIC: %d", __func__, __LINE__);
                    result = 1;
                }
                *isXmrUnwrapInfoKey = 1;
            } else {
                teeLog(Tee_Error, "**** %s: PANIC: %d", __func__, __LINE__);
                result = 1;
            }
        } else {
            teeLog(Tee_Error, "**** %s: PANIC: %d", __func__, __LINE__);
            result = 1;
        }
    }

    return result;
}

#define GET_IN_VAL(I, A, B)        do { result = getVal(params, nfREAD,          numParams, (I), (A), (B)); if (result) { DRMERR("%s:%s [%d] - FAILED", MODULE, __func__, __LINE__); goto Exit; } } while(0)
#define GET_OUT_VAL(I, A, B)       do { result = getValRef(params, nfWRITE,        numParams, (I), (A), (B)); if (result) { DRMERR("%s:%s [%d] - FAILED", MODULE, __func__, __LINE__); goto Exit; } } while(0)
#define GET_IN_OUT_VAL(I, A, B)    do { result = getValRef(params, nfREAD_WRITE,        numParams, (I), (A), (B)); if (result) { DRMERR("%s:%s [%d] - FAILED", MODULE, __func__, __LINE__); goto Exit; } } while(0)
#define GET_IN_REF(I, A, T, B)     do { const T ** memPtr = (const T **)(A); result = getInMem(params, nfREAD, numParams, (I), (const DRM_BYTE **)memPtr, (B)); if (result) { DRMERR("%s:%s [%d] - FAILED", MODULE, __func__, __LINE__); goto Exit; } } while(0)
#define GET_OUT_REF(I, A, T, B)    do { T ** memPtr = (T **)A; result = getMem(params, nfWRITE, numParams, (I), (DRM_BYTE **)memPtr, (B)); if (result) { DRMERR("%s:%s [%d] - FAILED", MODULE, __func__, __LINE__); goto Exit; } } while(0)
#define GET_IN_OUT_REF(I, A, B)    do { result = getMem(params, nfREAD_WRITE, numParams, (I), (A), (B)); if (result) { DRMERR("%s:%s [%d] - FAILED", MODULE, __func__, __LINE__); goto Exit; } } while(0)
#define GET_IN_STRUCT(I, A, T)     do { const T ** memPtr = (const T **)(A); result = getInMemFixed(params, nfREAD, numParams, (I), (const DRM_BYTE **)memPtr, sizeof(T)); if (result) { DRMERR("%s:%s [%d] - FAILED", MODULE, __func__, __LINE__); goto Exit; } } while(0)

#define GET_IN_OUT_STRUCT(I, A, T) do { const T ** memPtr = (const T **)(A); result = getInMemFixed(params, nfREAD_WRITE, numParams, (I), (const DRM_BYTE **)memPtr, sizeof(T)); if (result) { DRMERR("%s:%s [%d] - FAILED", MODULE, __func__, __LINE__); goto Exit; } } while(0)

uint32_t drmAgentDispatchRPCMsg(nfTeeOperation_t      operation,
                             nfAgentApiParameter_t params[],
                             uint32_t              numParams,
                             nfTeeClientId_t       ClientId)
{
    (void)ClientId; // unused

    uint32_t result = 0;
    uint32_t *res = &result; /* point to something sane in case things fail before res gets pointed to a param value */

    switch ( operation )
    {
        case nfTO_DRM_OP_PR_ALLOCATE_REGISTER:
        {
            OEM_HAL_KEY_TYPE f_eKeyType;
            OEM_HAL_KEY_REGISTER_INDEX *f_pKeyRegisterIndex;

            GET_IN_VAL(0, &f_eKeyType, NULL);
            GET_OUT_VAL(1, &f_pKeyRegisterIndex, &res);

            *res = tee_Oem_Hal_AllocateRegister(f_eKeyType, f_pKeyRegisterIndex);
        }
        break;

        case nfTO_DRM_OP_PR_FREE_REGISTER:
        {
            OEM_HAL_KEY_TYPE f_eKeyType;
            OEM_HAL_KEY_REGISTER_INDEX f_pKeyRegisterIndex;

            GET_IN_VAL(0, &f_eKeyType, &f_pKeyRegisterIndex);
            GET_OUT_VAL(1, &res, NULL);

            *res = tee_Oem_Hal_FreeRegister(f_eKeyType, f_pKeyRegisterIndex);
        }
        break;

        case nfTO_DRM_OP_PR_REGISTER_COUNT:
        {
            OEM_HAL_KEY_TYPE     f_eKeyType;
            DRM_DWORD           *f_pdwTotalRegisters;
            DRM_DWORD           *f_pdwAllocatedRegisters;

            GET_IN_VAL(0, &f_eKeyType, NULL);
            GET_OUT_VAL(1, &f_pdwTotalRegisters, &f_pdwAllocatedRegisters);
            GET_OUT_VAL(2, &res, NULL);

            *res = tee_Oem_Hal_RegisterCount(f_eKeyType, f_pdwTotalRegisters, f_pdwAllocatedRegisters);
        }
        break;

        case nfTO_DRM_OP_PR_GET_PRELOADED_INDEX:
        {
                  DRM_VOID                   *f_pOEMContext = NULL; // ignoring
                  OEM_HAL_KEY_TYPE            f_eKeyType;
            const DRM_BYTE                   *f_pbKeyId;
                  DRM_DWORD                   f_cbKeyId;
                  OEM_HAL_KEY_REGISTER_INDEX *f_pIndexKey;

            GET_IN_VAL(0, &f_eKeyType, &f_cbKeyId);
            GET_IN_REF(1, &f_pbKeyId, DRM_BYTE, &f_cbKeyId);
            GET_OUT_VAL(2, &f_pIndexKey, &res);

            *res = tee_Oem_Hal_GetPreloadedIndex(f_pOEMContext, f_eKeyType, f_pbKeyId, f_cbKeyId, f_pIndexKey);
        }
        break;

        case nfTO_DRM_OP_PR_INITIALIZE:
        {
            GET_OUT_VAL(0, &res, NULL);
            *res = tee_Oem_Hal_Initialize();
        }
        break;

        case nfTO_DRM_OP_PR_DEINITIALIZE:
        {
            GET_OUT_VAL(0, &res, NULL);
            *res = tee_Oem_Hal_Deinitialize();

        }
        break;

        case nfTO_DRM_OP_PR_VERIFY_MSG_SIGNATURE:
        {
            const DRM_BYTE                     *f_pbMessage;
                  DRM_DWORD                     f_cbMessage;
            const nfTeeDrmPRVerifyMessageSignature_t *args;
            const DRM_BYTE                  *f_pbSignature;
                  DRM_DWORD                  f_cbSignature;

            GET_IN_REF( 0, &f_pbMessage, DRM_BYTE, &f_cbMessage);
            GET_IN_REF( 1, &f_pbSignature, DRM_BYTE, &f_cbSignature);
            GET_IN_STRUCT( 2, &args, nfTeeDrmPRVerifyMessageSignature_t);
            GET_OUT_VAL(3, &res, NULL);
            *res = tee_Oem_Hal_VerifyMessageSignature(
                      f_pbMessage,
                args->f_cbMessage,
                      f_pbSignature,
                args->f_cbSignature,
                args->f_eHashType,
                args->f_eSignatureScheme,
                args->f_indexIntegrityKey);
        }
        break;

        case nfTO_DRM_OP_PR_SIGN_WITH_PRIVATE_KEY:
        {
                  DRM_VOID                       *f_pOEMContext = NULL; // ignore
                  DRM_CRYPTO_CONTEXT             *f_pCryptoContext = NULL; // ignore
            const DRM_BYTE                       *f_pbMessage;
                  DRM_DWORD                       f_cbMessage;
                  DRM_BYTE                       *f_pbSignature;
                  DRM_DWORD                       f_cbSignature;
                  DRM_DWORD                      *f_pcbSignature;
            nfTeeDrmPRSignWithPrivateKey_t *args;

            GET_IN_STRUCT(0, &args, nfTeeDrmPRSignWithPrivateKey_t);
            GET_IN_REF(   1, &f_pbMessage, DRM_BYTE, &f_cbMessage);
            GET_OUT_REF(  2, &f_pbSignature, DRM_BYTE, &f_cbSignature);
            GET_OUT_VAL(  3, &f_pcbSignature, &res);
            if (f_cbMessage != args->f_cbMessage) {
                DRMERR("%s:%s:%d - mismatch between memory sizes: %d vs %d", MODULE, __func__, __LINE__, f_cbMessage, args->f_cbMessage);
                result = nfTGR_INVALID_PARM1;
                goto Exit;
            }
            if (f_cbSignature != args->f_cbSignature) {
                DRMERR("%s:%s:%d - mismatch between memory sizes: %d vs %d", MODULE, __func__, __LINE__, f_cbSignature, args->f_cbSignature);
                result = nfTGR_INVALID_PARM2;
                goto Exit;
            }
            *f_pcbSignature = args->f_cbSignature;

            *res = tee_Oem_Hal_SignWithPrivateKey(
                      f_pOEMContext,
                      f_pCryptoContext,
                      f_pbMessage,
                args->f_cbMessage,
                      f_pbSignature,
                      f_pcbSignature,
                args->f_eHashType,
                args->f_eSignatureScheme,
                args->f_eKeyType,
                args->f_indexIntegrityKey);
        }
        break;

        case nfTO_DRM_OP_PR_VERIFY_OMAC1_SIGNATURE:
        {
            const DRM_BYTE                   *f_pbData;
                  DRM_DWORD                   f_cbData;
            const DRM_BYTE                   *f_pbSignature;
                  DRM_DWORD                   f_cbSignature;
            const nfTeeDrmPRVerifyOMAC1Signature_t *args;

            GET_IN_REF(0, &f_pbData, DRM_BYTE, &f_cbData);
            GET_IN_REF(1, &f_pbSignature, DRM_BYTE, &f_cbSignature);
            GET_IN_STRUCT(2, &args, nfTeeDrmPRVerifyOMAC1Signature_t);
            GET_OUT_VAL(3, &res, NULL);

            *res = tee_Oem_Hal_VerifyOMAC1Signature(
                      f_pbData,
                args->f_cbData,
                      f_pbSignature,
                args->f_cbSignature,
                args->f_eKeyType,
                args->f_indexKey);
        }
        break;

        case nfTO_DRM_OP_PR_CREATE_OMAC1_SIGNATURE:
        {
            const DRM_BYTE                   *f_pbData;
                  DRM_DWORD                   f_cbData;
                  DRM_BYTE                   *f_pbSignature;
                  DRM_DWORD                  *f_pcbSignature;
                  DRM_DWORD                   f_cbSignature;
            const nfTeeDrmPRCreateOMAC1Signature_t *args;

            GET_IN_STRUCT(0, &args, nfTeeDrmPRCreateOMAC1Signature_t);
            GET_IN_REF(   1, &f_pbData, DRM_BYTE, &f_cbData);
            GET_OUT_REF(  2, &f_pbSignature, DRM_BYTE, &f_cbSignature);
            GET_OUT_VAL(  3, &f_pcbSignature, &res);
            *f_pcbSignature = args->f_cbSignature;

            if (f_cbData != args->f_cbData) {
                DRMERR("%s:%s:%d - mismatch between memory sizes: %d vs %d", MODULE, __func__, __LINE__, f_cbData, args->f_cbData);
                result = nfTGR_INVALID_PARM1;
                goto Exit;
            }
            if (f_cbSignature != args->f_cbSignature) {
                DRMERR("%s:%s:%d - mismatch between memory sizes: %d vs %d", MODULE, __func__, __LINE__, f_cbSignature, args->f_cbSignature);
                result = nfTGR_INVALID_PARM2;
                goto Exit;
            }

            *res = tee_Oem_Hal_CreateOMAC1Signature(
                      f_pbData,
                args->f_cbData,
                      f_pbSignature,
                      f_pcbSignature,
                args->f_eKeyType,
                args->f_indexKey);
        }
        break;

        case nfTO_DRM_OP_PR_ENCRYPT_DATA_BLOCK:
        {
            const DRM_BYTE                   *f_pbBlock;
                  DRM_DWORD                   f_cbBlock;
                  DRM_BYTE                   *f_pbEncryptedBlock;
                  DRM_DWORD                  *f_pcbEncryptedBlock;
                  DRM_DWORD                   f_cbEncryptedBlock;
            const nfTeeDrmPREncryptDataBlock_t *args;

            GET_IN_REF( 0, &f_pbBlock, DRM_BYTE, &f_cbBlock);
            GET_OUT_REF(1, &f_pbEncryptedBlock, DRM_BYTE, &f_cbEncryptedBlock);
            GET_IN_STRUCT( 2, &args, nfTeeDrmPREncryptDataBlock_t);
            GET_OUT_VAL(3, &f_pcbEncryptedBlock, &res);
            *f_pcbEncryptedBlock = args->f_cbEncryptedBlock;

            *res = tee_Oem_Hal_EncryptDataBlock(
                      f_pbBlock,
                args->f_cbBlock,
                      f_pbEncryptedBlock,
                      f_pcbEncryptedBlock,
                args->f_eKeyType,
                args->f_indexMessageEncryptKey);
        }
        break;

        case nfTO_DRM_OP_PR_UNWRAP_KEY:
        {
                  DRM_CRYPTO_CONTEXT         *f_pCryptoContext = NULL; // ignore
            const DRM_BYTE                   *f_pbWrappedKeyData;
                  DRM_DWORD                   f_cbWrappedKeyData;
            const DRM_BYTE                   *f_pbParameterData;
                  DRM_DWORD                   f_cbParameterData;
                  nfTeeDrmPRUnwrapKey_t *args;

            DRM_BOOL isEncKey = 0;
            DRM_BYTE *encKeyPtr = NULL;
            DRM_BOOL isXmrUnwrapInfoKey = 0;
            DRM_BYTE *xmrLicPtr = NULL;
            DRM_BB_CHECKSUM_CONTEXT *checksumPtr = NULL;

            GET_IN_OUT_STRUCT( 0, &args, nfTeeDrmPRUnwrapKey_t);
            GET_IN_REF( 1, &f_pbWrappedKeyData, DRM_BYTE, &f_cbWrappedKeyData);
            if (args->f_cbParameterData > 1) {
                GET_IN_REF( 2, &f_pbParameterData, DRM_BYTE, &f_cbParameterData);
            } else {
                f_pbParameterData = NULL;
                f_cbParameterData = 0;
            }
            GET_OUT_VAL(3, &res, NULL);

            if (f_cbWrappedKeyData != args->f_cbWrappedKeyData) {
                DRMERR("%s:%s:%d - mismatch between memory sizes: %d vs %d", MODULE, __func__, __LINE__, f_cbWrappedKeyData, args->f_cbWrappedKeyData);
                result = nfTGR_INVALID_PARM1;
                goto Exit;
            }

            if (f_cbParameterData != args->f_cbParameterData) {
                DRMERR("%s:%s:%d - mismatch between memory sizes: %d vs %d", MODULE, __func__, __LINE__, f_cbParameterData, args->f_cbParameterData);
                result = nfTGR_INVALID_PARM2;
                goto Exit;
            }

            if (isDrmEncryptedKey(args->f_eWrappingKeyType, f_pbWrappedKeyData, f_cbWrappedKeyData, &isEncKey)) {
                DRMERR("%s:%s:%d - Invalid Key To Unwrap", MODULE, __func__, __LINE__);
                result = nfTGR_INVALID_PARM1;
                goto Exit;
            }

            if (isEncKey) {
                encKeyPtr = ((DRM_ENCRYPTED_KEY *)f_pbWrappedKeyData)->pbEncryptedKey;
                ((DRM_ENCRYPTED_KEY *)f_pbWrappedKeyData)->pbEncryptedKey = args->f_pbEncryptedKey;
            } else {
                if (isXmrUnwrapInfo(args->f_eWrappingKeyType, f_pbWrappedKeyData, f_cbWrappedKeyData, &isXmrUnwrapInfoKey)) {
                    DRMERR("%s:%s:%d - Invalid Key To Unwrap", MODULE, __func__, __LINE__);
                    result = nfTGR_INVALID_PARM1;
                    goto Exit;
                }
                if (isXmrUnwrapInfoKey) {
                    DRM_XMR_UNWRAP_INFO* pStruct = (DRM_XMR_UNWRAP_INFO*)f_pbWrappedKeyData;
                    xmrLicPtr = (DRM_BYTE*)pStruct->pbXMRLic;
                    pStruct->pbXMRLic = args->f_pbEncryptedKey;
                    if (pStruct->poChecksum) {
                        checksumPtr = (DRM_BB_CHECKSUM_CONTEXT*)pStruct->poChecksum;
                        pStruct->poChecksum = (DRM_BB_CHECKSUM_CONTEXT*)(args->f_pbEncryptedKey + pStruct->cbXMRLic);
                    }
                }
            }

            *res = tee_Oem_Hal_UnwrapKey(
                      f_pCryptoContext,
                args->f_eKeyType,
                args->f_indexKey,
                args->f_eWrappingKeyType,
                args->f_indexWrappingKey,
                      f_pbWrappedKeyData,
                      args->f_cbWrappedKeyData,
                      f_pbParameterData,
                args->f_cbParameterData);

            if (isEncKey) {
                ((DRM_ENCRYPTED_KEY *)f_pbWrappedKeyData)->pbEncryptedKey = encKeyPtr;
            } else if (isXmrUnwrapInfoKey) {
                ((DRM_XMR_UNWRAP_INFO*)f_pbWrappedKeyData)->pbXMRLic = xmrLicPtr;
                ((DRM_XMR_UNWRAP_INFO*)f_pbWrappedKeyData)->poChecksum = checksumPtr;
            }
        }
        break;

        case nfTO_DRM_OP_PR_WRAP_KEY:
        {
                  DRM_VOID                   *f_pOEMContext = NULL; // ignore
                  DRM_CRYPTO_CONTEXT         *f_pCryptoContext = NULL; // ignore
            const DRM_BYTE                   *f_pbWrappedKey;
                  DRM_DWORD                   f_cbWrappedKey;
                  DRM_DWORD                  *f_pcbWrappedKey;
                  nfTeeDrmPRWrapKey_t        *args;

            DRM_BOOL isEncKey = 0;
            DRM_BYTE *encKeyPtr = NULL;
            DRM_BOOL isXmrUnwrapInfoKey = 0;
            DRM_BYTE *xmrLicPtr = NULL;
            DRM_BB_CHECKSUM_CONTEXT *checksumPtr = NULL;

            GET_IN_OUT_STRUCT(0, &args, nfTeeDrmPRWrapKey_t);
            GET_IN_REF(1, &f_pbWrappedKey, DRM_BYTE, &f_cbWrappedKey);
            GET_IN_OUT_VAL(2, &f_pcbWrappedKey, NULL);
            GET_OUT_VAL(3, &res, NULL);
            *f_pcbWrappedKey = args->f_cbWrappedKey;

            if (isDrmEncryptedKey(args->f_eWrappingKeyType, f_pbWrappedKey, f_cbWrappedKey, &isEncKey)) {
                DRMERR("%s:%s:%d - Invalid Key To Unwrap", MODULE, __func__, __LINE__);
                result = nfTGR_INVALID_PARM1;
                goto Exit;
            }

            if (isEncKey) {
                encKeyPtr = ((DRM_ENCRYPTED_KEY *)f_pbWrappedKey)->pbEncryptedKey;
                ((DRM_ENCRYPTED_KEY *)f_pbWrappedKey)->pbEncryptedKey = args->f_pbEncryptedKey;
            } else {
                if (isXmrUnwrapInfo(args->f_eWrappingKeyType, f_pbWrappedKey, f_cbWrappedKey, &isXmrUnwrapInfoKey)) {
                    DRMERR("%s:%s:%d - Invalid Key To Wrap", MODULE, __func__, __LINE__);
                    result = nfTGR_INVALID_PARM1;
                    goto Exit;
                }
                if (isXmrUnwrapInfoKey) {
                    DRM_XMR_UNWRAP_INFO* pStruct = (DRM_XMR_UNWRAP_INFO*)f_pbWrappedKey;
                    xmrLicPtr = (DRM_BYTE*)pStruct->pbXMRLic;
                    pStruct->pbXMRLic = args->f_pbEncryptedKey;
                    if (pStruct->poChecksum) {
                        checksumPtr = (DRM_BB_CHECKSUM_CONTEXT*)pStruct->poChecksum;
                        pStruct->poChecksum = (DRM_BB_CHECKSUM_CONTEXT*)(args->f_pbEncryptedKey + pStruct->cbXMRLic);
                    }
                }
            }

            *res = tee_Oem_Hal_WrapKey(
                      f_pOEMContext,
                      f_pCryptoContext,
                args->f_eKeyType,
                args->f_indexKey,
                args->f_eWrappingKeyType,
                args->f_indexWrappingKey,
                      (DRM_BYTE *)f_pbWrappedKey,
                      f_pcbWrappedKey);

            if (isEncKey) {
                ((DRM_ENCRYPTED_KEY *)f_pbWrappedKey)->pbEncryptedKey = encKeyPtr;
            } else if (isXmrUnwrapInfoKey) {
                ((DRM_XMR_UNWRAP_INFO*)f_pbWrappedKey)->pbXMRLic = xmrLicPtr;
                ((DRM_XMR_UNWRAP_INFO*)f_pbWrappedKey)->poChecksum = checksumPtr;
            }
        }
        break;

        case nfTO_DRM_OP_PR_GENERATE_KEY:
        {
            DRM_VOID                  *f_pOEMContext = NULL; // ignore
            DRM_CRYPTO_CONTEXT        *f_pCryptoContext = NULL; // ignore
            OEM_HAL_KEY_TYPE           f_eKeyType;
            OEM_HAL_KEY_REGISTER_INDEX f_indexKey;
            DRM_DWORD                  f_dwRequestedSecurityLevel;

            GET_IN_VAL(0, &f_eKeyType, &f_indexKey);
            GET_IN_VAL(1, &f_dwRequestedSecurityLevel, NULL);
            GET_OUT_VAL(2, &res, NULL);

            *res = tee_Oem_Hal_GenerateKey(
                f_pOEMContext,
                f_pCryptoContext,
                f_eKeyType,
                f_indexKey,
                f_dwRequestedSecurityLevel);
        }
        break;

        case nfTO_DRM_OP_PR_DECRYPT_CONTENT:
        {
                  DRM_BYTE                   *f_pbData;
                  DRM_DWORD                   f_cbData;
            const nfTeeDrmPRDecryptContent_t *args;

            GET_IN_STRUCT(0, &args, nfTeeDrmPRDecryptContent_t);
            GET_IN_OUT_REF(1, &f_pbData, &f_cbData);
            GET_OUT_VAL(2, &res, NULL);

            *res = tee_Oem_Hal_DecryptContent(
                      f_pbData,
                args->f_cbData,
                args->f_eKeyType,
                args->f_indexContentKey,
                args->f_qwInitializationVector,
                args->f_qwSampleByteOffset);
        }
        break;

        case nfTO_DRM_OP_PR_DECRYPT_CONTENT_OPAQUE:
        {
                  OEM_OPAQUE_BUFFER_HANDLE    f_hInBuf;
                  OEM_OPAQUE_BUFFER_HANDLE    f_hOutBuf;
            const nfTeeDrmPRDecryptContentOpaque_t *args;
                  DRM_DWORD tmp;

            GET_IN_STRUCT(0, &args, nfTeeDrmPRDecryptContentOpaque_t);
            GET_IN_REF(1, &f_hInBuf, DRM_VOID, &tmp);
            GET_IN_REF(2, &f_hOutBuf, DRM_VOID, &tmp);
            GET_OUT_VAL(3, &res, NULL);

            *res = tee_Oem_Hal_DecryptContentOpaque(
                      f_hInBuf,
                      f_hOutBuf,
                args->f_cbData,
                args->f_eKeyType,
                args->f_indexContentKey,
                args->f_qwInitializationVector,
                args->f_qwSampleByteOffset);
        }
        break;

        case nfTO_DRM_OP_PR_CREATE_SAMPLE_ENCRYPTION_CONTEXT:
        {
                  OEM_ENCRYPTION_HANDLE          f_phEncryptionContext;
            const nfTeeDrmPRCreateSampleEncryptionContext_t *args;
                  DRM_DWORD tmp;

            GET_IN_STRUCT(0, &args, nfTeeDrmPRCreateSampleEncryptionContext_t);
            GET_OUT_REF(1, &f_phEncryptionContext, DRM_VOID, &tmp);
            GET_OUT_VAL(2, &res, NULL);

            *res = tee_Oem_Hal_CreateSampleEncryptionContext(
                args->f_eKeyType,
                args->f_indexKey,
                args->f_eSampleEncryptionMode,
                &f_phEncryptionContext);
        }
        break;

        case nfTO_DRM_OP_PR_ENCRYPT_SAMPLE_DATA:
        {
            DRM_VOID                 *f_pOEMContext = NULL; // ignoring
            OEM_ENCRYPTION_HANDLE     f_hEncryptionContext;
            DRM_UINT64               *f_pqwInitializationVector = NULL; // ignoring
            OEM_HAL_SAMPLE_METADATA  *f_pMetadata = NULL; // ignoring
            OEM_OPAQUE_BUFFER_HANDLE  f_hInBuf = NULL; // ignoring
            OEM_OPAQUE_BUFFER_HANDLE  f_hOutBuf = NULL; // ignoring
            DRM_DWORD                 f_cbData;
            DRM_BYTE                 *f_pbData;
            DRM_DWORD tmp;

            GET_IN_REF(0, &f_hEncryptionContext, DRM_VOID, &tmp);
            GET_IN_OUT_REF(1, &f_pbData, &tmp);
            GET_IN_VAL(2, &f_cbData, NULL);
            GET_OUT_VAL(3, &res, NULL);

            *res = tee_Oem_Hal_EncryptSampleData(
                f_pOEMContext,
                f_hEncryptionContext,
                f_pqwInitializationVector,
                f_pMetadata,
                f_hInBuf,
                f_hOutBuf,
                f_cbData,
                f_pbData);
        }
        break;

        case nfTO_DRM_OP_PR_QUERY_SAMPLE_METADATA:
        {
            OEM_ENCRYPTION_HANDLE    f_hEncryptionContext = NULL;
            OEM_HAL_SAMPLE_METADATA *f_pMetadata = NULL;
            DRM_DWORD tmp;

            GET_IN_REF( 0, &f_hEncryptionContext, DRM_VOID, &tmp);
            GET_OUT_REF(1, f_pMetadata, DRM_VOID, &tmp);
            GET_OUT_VAL(2, &res, NULL);

            *res = tee_Oem_Hal_QuerySampleMetadata(f_hEncryptionContext, f_pMetadata);
        }
        break;

        case nfTO_DRM_OP_PR_DESTROY_SAMPLE_ENCRYPTION_CONTEXT:
        {
            OEM_ENCRYPTION_HANDLE f_hEncryptionContext;
            DRM_DWORD tmp;

            GET_IN_REF(0, &f_hEncryptionContext, DRM_VOID, &tmp);
            GET_OUT_VAL(1, &res, NULL);

            *res = tee_Oem_Hal_DestroySampleEncryptionContext(f_hEncryptionContext);
        }
        break;

        case nfTO_DRM_OP_PR_LOAD_PLAYREADY_REVOCATION_INFO:
        {
                  DRM_CRYPTO_CONTEXT         *f_pCryptoContext = NULL; // ignoring
            const DRM_BYTE                   *f_pbRevocationInfo;
                  DRM_DWORD                   f_cbRevocationInfo;
            const nfTeeDrmPRLoadPlayReadyRevocationInfo_t *args;

            GET_IN_STRUCT(0, &args, nfTeeDrmPRLoadPlayReadyRevocationInfo_t);
            GET_IN_REF(1, &f_pbRevocationInfo, DRM_BYTE, &f_cbRevocationInfo);
            GET_OUT_VAL(2, &res, NULL);

            *res = tee_Oem_Hal_LoadPlayReadyRevocationInfo(
                      f_pCryptoContext,
                      f_pbRevocationInfo,
                args->f_cbRevocationInfo,
                args->f_indexSigningKey);
        }
        break;

        case nfTO_DRM_OP_PR_LOAD_PLAYREADY_CRL:
        {
                  DRM_CRYPTO_CONTEXT         *f_pCryptoContext = NULL; // ignoring
            const DRM_BYTE                   *f_pbCRL;
                  DRM_DWORD                   f_cbCRL;
            const nfTeeDrmPRLoadPlayReadyCrl_t *args;

            GET_IN_STRUCT(0, &args, nfTeeDrmPRLoadPlayReadyCrl_t);
            GET_IN_REF(1, &f_pbCRL, DRM_BYTE, &f_cbCRL);
            GET_OUT_VAL(2, &res, NULL);

            *res = tee_Oem_Hal_LoadPlayReadyCrl(
                      f_pCryptoContext,
                      f_pbCRL,
                args->f_cbCRL,
                args->f_indexSigningKey);
        }
        break;

        default:
        {
            teeLog(Tee_Error, "%s: unimplemented operation (%08x)", __FUNCTION__, operation);
            result = nfTGR_INTERNAL_ERROR;
        }
        break;

    }/* end switch ( operation ) */

Exit:
    DRMDBG("%s: returning %08x %08x", __FUNCTION__, result, *res);

    return result;
}

