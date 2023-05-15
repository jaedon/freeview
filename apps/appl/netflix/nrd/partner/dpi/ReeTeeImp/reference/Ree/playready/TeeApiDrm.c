/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include <stdlib.h>
#include <stdio.h>
#include <memory.h>

#include "oemhaldefs.h"
#include "oemhal.h"
#include "teeoemhal.h"
#include "teeoemhalstructs.h"

#include <nrddpi/IConnection.h>
#include <nrddpi/ISession.h>
#include <nrddpi/IMemoryReference.h>
#include <nrddpi/IValue.h>
#include <nrddpi/IMessage.h>

#include "RPCErrorTypes.h"
#include <nrddpi/TeeApiErrorTypes.h>
#include <nrddpi/TeeDrmTypes.h>
#include "TeeLog.h"

static int tee_initialized = 0;
static nfConnection_t connection = 0;
static nfSession_t session = 0;

/********************
 * HELPERS: BEGIN { *
 ********************/

#define MODULE "TeeApiDrm"

#ifdef REE_DRM_DEBUG
#define FUNC_ENTER TEE_TRACE("****** %s:%s [%d] ENTER", MODULE, __func__, __LINE__);
#define FUNC_EXIT TEE_TRACE("****** %s:%s [%d] EXIT: %08x", MODULE, __func__, __LINE__, result);
#else
#define FUNC_ENTER
#define FUNC_EXIT
#endif

#define NONE 0

#define CHECK_ERR(err,str)                      \
    do {                                        \
        if (err != nfTAE_SUCCESS) {             \
            teeLog(Tee_Error, "%s\n", str);     \
            result = DRM_E_FAIL;                \
            goto Exit;                          \
        }                                       \
    } while(0)

#define CHECK_INDEX(I, S) do { if (index >= S) { teeLog(Tee_Error, "%s:%s: invalid index %d, size %d", MODULE, __func__, (I), (S)); exit(1); } } while(0)

#define NRDP_UNUSED(X) (void)(X)

static nfTeeApiError_t setVal(nfParameter_t params[], uint32_t index, uint32_t nParams, uint32_t a, uint32_t b, nfValueType_t type)
{
    CHECK_INDEX(index, nParams);

    nfTeeApiError_t err = nfTAE_SUCCESS;

    nfParameter_t *param = &params[index];

    memset(param, 0, sizeof(nfParameter_t));
    param->type = nfPARAMETER_VALUE_TYPE;
    nfValue_t *value = (nfValue_t*)&param->data;
    value->type = type;
    value->a = a;
    value->b = b;
    TEE_TRACE("%s:%s[%d] <-- ( %d %d )", MODULE, __func__, index, value->a, value->b);

    return err;
}

static nfTeeApiError_t setInVal(nfParameter_t params[], uint32_t index, uint32_t nParams, uint32_t a, uint32_t b)
{
    return setVal(params, index, nParams, a, b, nfVALUE_READ_ONLY);
}

static nfTeeApiError_t setInOutVal(nfParameter_t params[], uint32_t index, uint32_t nParams, uint32_t a, uint32_t b)
{
    return setVal(params, index, nParams, a, b, nfVALUE_READ_WRITE);
}

static nfTeeApiError_t setOutVal(nfParameter_t params[], uint32_t index, uint32_t nParams)
{
    return setVal(params, index, nParams, 0, 0, nfVALUE_WRITE_ONLY);
}

static nfTeeApiError_t getVal(nfParameter_t params[], uint32_t index, uint32_t nParams, uint32_t *a, uint32_t *b)
{
    CHECK_INDEX(index, nParams);
    nfParameter_t *param = &params[index];
    nfTeeApiError_t err = nfTAE_SUCCESS;
    nfValue_t *value = (nfValue_t*)&param->data;
    if (a) *a = value->a;
    if (b) *b = value->b;
    TEE_TRACE("%s:%s[%d] --> ( %d %d )", MODULE, __func__, index, value->a, value->b);
    return err;
}

// Playready memory buffers must be registered, not allocated
static nfTeeApiError_t _setRef(nfParameter_t params[], uint32_t index, uint32_t nParams, const void *buf, size_t size, nfMemoryReferenceReadWriteType_t type)
{
    CHECK_INDEX(index, nParams);
    nfTeeApiError_t err;

    nfParameter_t *param = &params[index];
    memset(param, 0, sizeof(nfParameter_t));
    param->type = nfPARAMETER_MEMORY_REFERENCE_TYPE;
    nfMemoryReference_t *registeredMemRef = (nfMemoryReference_t *)(&param->data);
    registeredMemRef->addr = (void *)buf;
    registeredMemRef->size = size;
    registeredMemRef->type = type;
    TEE_TRACE("%s:%s[%d] <-- { %p %d }", MODULE, __func__, index, registeredMemRef->addr, registeredMemRef->size);
    err = nfMemoryReferenceRegister(session, registeredMemRef);
    return err;
}

static nfTeeApiError_t setInRef(nfParameter_t params[], uint32_t index, uint32_t nParams, const void *buf, size_t size) {
    return _setRef(params, index, nParams, buf, size, nfMEMORY_REFERENCE_READ_ONLY);
}

static nfTeeApiError_t setOutRef(nfParameter_t params[], uint32_t index, uint32_t nParams, const void *buf, size_t size) {
    return _setRef(params, index, nParams, buf, size, nfMEMORY_REFERENCE_WRITE_ONLY);
}

static nfTeeApiError_t setInOutRef(nfParameter_t params[], uint32_t index, uint32_t nParams, const void *buf, size_t size) {
    return _setRef(params, index, nParams, buf, size, nfMEMORY_REFERENCE_READ_WRITE);
}

static nfTeeApiError_t memRelease(nfParameter_t params[], uint32_t nParams) {
    nfTeeApiError_t err;
    uint32_t i;
    for (i = 0; i < nParams; i++) {
        if (params[i].type == nfPARAMETER_MEMORY_REFERENCE_TYPE) {
            nfMemoryReference_t *registeredMemRef = (nfMemoryReference_t *)(&(params[i].data));
            err = nfMemoryReferenceRelease(registeredMemRef);
            if (err != nfTAE_SUCCESS) {
                teeLog(Tee_Error, "Failed to Release allocated memory reference");
                goto Exit;
            }
        }
    }

Exit:
    return err;
}

#ifdef REE_DRM_DEBUG
static void prthex(const char * func, int line, const uint8_t *p, uint32_t size) {
    printf("%s:%s:%d ", MODULE, func, line);
    for (DRM_DWORD i = 0; i < size; i++) {
        printf("%02x ", p[i]);
    }
    printf("\n");
}
#define PRTHEX(X, Y) prthex(__func__, __LINE__, X, Y)
#else
#define PRTHEX(X, Y)
#endif

#define DRM_E_FAIL 1

static nfTeeApiError_t testErr = nfTAE_SUCCESS;
static uint32_t testReturnCode = nfTGR_SUCCESS;

void testGetDrmErrors(nfTeeApiError_t *err, uint32_t *returnCode) {
    *err = testErr;
    *returnCode = testReturnCode;
}

void testClearDrmErrors() {
    testErr = nfTAE_SUCCESS;
    testReturnCode = nfTGR_SUCCESS;
}

#define INIT(N) nfTeeApiError_t err = nfTAE_SUCCESS; \
                uint32_t returnCode = nfTGR_SUCCESS; \
                uint32_t result = 0; \
                nfParameter_t params[N]; \
                const int nParams = N

#define SET_IN_VAL(I, A, B)           do { err = setInVal(params, (I), nParams, (A), (B)); CHECK_ERR(err, "Failed to Set In Value"); } while (0)

#define SET_IN_OUT_VAL(I, A, B)       do { err = setInOutVal(params, (I), nParams, (A), (B)); CHECK_ERR(err, "Failed to Set In/Out Value"); } while (0)

#define SET_OUT_VAL(I)                do { err = setOutVal(params, (I), nParams); CHECK_ERR(err, "Failed to Set Out Value"); } while (0)

#define DO_SEND(OPCODE)               do { err = nfSendMessage(session, (OPCODE), params, nParams, &returnCode); \
                                           if ((err != nfTAE_SUCCESS) || (returnCode != nfTGR_SUCCESS)) { \
                                               teeLog(Tee_Error, "%s [%d] FAILED: %08x %08x", __func__, __LINE__, err, returnCode); \
                                               result = DRM_E_FAIL; \
                                               goto Exit; } \
                                         } while(0)

#define GET_IN_OUT_VAL(I, X, Y)       do { err = getVal(params, (I), nParams, (X), (Y)); CHECK_ERR(err, "Failed to Get In/Out Value"); } while(0)

#define GET_OUT_VAL(I, X, Y)          do { err = getVal(params, (I), nParams, (X), (Y)); CHECK_ERR(err, "Failed to Get Out Value"); } while(0)

#define SET_IN_REF(I, BUF, SIZE)      do { err = setInRef(params, (I), nParams, (BUF), (SIZE)); CHECK_ERR(err, "Failed to Set In MemRef"); } while(0)

#define SET_OUT_REF(I, BUF, SIZE)     do { err = setOutRef(params, (I), nParams, (BUF), (SIZE)); CHECK_ERR(err, "Failed to Set Out MemRef"); } while(0)

#define SET_IN_OUT_REF(I, BUF, SIZE)  do { err = setInOutRef(params, (I), nParams, (BUF), (SIZE)); CHECK_ERR(err, "Failed to Set In/Out MemRef"); } while(0)

#define RETURN                        do { (void)memRelease(params, nParams); \
                                               testErr = err; \
                                               testReturnCode = returnCode; \
                                               return result; } while(0)

#define NOT_SUPPORTED teeLog(Tee_Error, "%s:%s:%d NOT SUPPORTED", MODULE, __func__, __LINE__); return DRM_E_FAIL

/******************
 * } HELPERS: END *
 ******************/

DRM_API DRM_RESULT DRM_CALL Oem_Hal_AllocateRegister(
    __in              OEM_HAL_KEY_TYPE            f_eKeyType,
    __out_ecount( 1 ) OEM_HAL_KEY_REGISTER_INDEX *f_pKeyRegisterIndex )
{
    FUNC_ENTER;
    INIT(2);

    SET_IN_VAL(0, f_eKeyType, NONE);
    SET_OUT_VAL(1);

    DO_SEND(nfTO_DRM_OP_PR_ALLOCATE_REGISTER);

    GET_OUT_VAL(1, (uint32_t*)f_pKeyRegisterIndex, &result);

Exit:
    FUNC_EXIT;
    RETURN;
}

DRM_API DRM_RESULT DRM_CALL Oem_Hal_FreeRegister(
    __in OEM_HAL_KEY_TYPE           f_eKeyType,
    __in OEM_HAL_KEY_REGISTER_INDEX f_keyRegisterIndex )
{
    FUNC_ENTER;
    INIT(2);

    SET_IN_VAL(0, f_eKeyType, f_keyRegisterIndex);
    SET_OUT_VAL(1);

    DO_SEND(nfTO_DRM_OP_PR_FREE_REGISTER);

    GET_OUT_VAL(1, &result, NULL);

Exit:
    FUNC_EXIT;
    RETURN;
}

DRM_API DRM_RESULT DRM_CALL Oem_Hal_RegisterCount(
    __in            OEM_HAL_KEY_TYPE     f_eKeyType,
    __out_ecount(1) DRM_DWORD           *f_pdwTotalRegisters,
    __out_ecount(1) DRM_DWORD           *f_pdwAllocatedRegisters )
{
    FUNC_ENTER;
    INIT(3);

    SET_IN_VAL(0, f_eKeyType, NONE);
    SET_OUT_VAL(1);
    SET_OUT_VAL(2);

    DO_SEND(nfTO_DRM_OP_PR_REGISTER_COUNT);

    GET_OUT_VAL(1, f_pdwTotalRegisters, f_pdwAllocatedRegisters);
    GET_OUT_VAL(2, &result, NULL);

Exit:
    FUNC_EXIT;
    RETURN;
}

DRM_API DRM_RESULT DRM_CALL Oem_Hal_GetPreloadedIndex(
    __in                           DRM_VOID                   *f_pOEMContext, // ignoring
    __in                           OEM_HAL_KEY_TYPE            f_eKeyType,
    __in_bcount( f_cbKeyId ) const DRM_BYTE                   *f_pbKeyId,
    __in                           DRM_DWORD                   f_cbKeyId,
    __out_ecount( 1 )              OEM_HAL_KEY_REGISTER_INDEX *f_pIndexKey )
{
    FUNC_ENTER;
    (void)f_pOEMContext;

    INIT(3);

    SET_IN_VAL(0, f_eKeyType, f_cbKeyId);
    SET_IN_REF(1, (uint32_t*)f_pbKeyId, f_cbKeyId);
    SET_OUT_VAL(2);

    DO_SEND(nfTO_DRM_OP_PR_GET_PRELOADED_INDEX);

    GET_OUT_VAL(2, (uint32_t*)f_pIndexKey, &result);

Exit:
    FUNC_EXIT;
    RETURN;
}

int Drm_Tee_Connect()
{
    if (tee_initialized) return 0;

    nfTeeApiError_t err = nfTAE_SUCCESS;
    uint32_t returnCode = nfRPC_RESULT_SUCCESS;

    err = nfOpenConnection(nfCONTAINER_ID_TEE, &connection);
    if (err != nfTAE_SUCCESS) {
        teeLog(Tee_Error, "%s:%s:%d: Failed to Open Connection to the TEE: err %08x", MODULE, __func__, __LINE__, err);
        return 1;
    }
    err = nfOpenSession(connection, nfSERVICE_NETFLIX_TASK_GROUP, &session, &returnCode);
    if (err != nfTAE_SUCCESS) {
        teeLog(Tee_Error, "%s:%s:%d: Failed to Open Session to the TEE: err %08x", MODULE, __func__, __LINE__, err);
        return 1;
    }
    if (returnCode != nfRPC_RESULT_SUCCESS) {
        teeLog(Tee_Error, "%s:%s:%d: Failed to Open Session to the TEE: returnCode %08x", MODULE, __func__, __LINE__, returnCode);
        return 1;
    }

    tee_initialized = 1;

    return 0;
}

int Drm_Tee_Disconnect()
{
    if (!tee_initialized) return 0;

    nfTeeApiError_t err = nfTAE_SUCCESS;
    uint32_t returnCode = nfTGR_SUCCESS;

    err = nfCloseSession(session, &returnCode);
    if (err != nfTAE_SUCCESS) {
        teeLog(Tee_Error, "%s:%s:%d: Failed to Close Session to the TEE: err %08x", MODULE, __func__, __LINE__, err);
        return 1;
    }
    if (returnCode != nfTGR_SUCCESS) {
        teeLog(Tee_Error, "%s:%s:%d: Failed to Close Session to the TEE: returnCode %08x", MODULE, __func__, __LINE__, returnCode);
        return 1;
    }
    err = nfCloseConnection(connection);
    if (err != nfTAE_SUCCESS) {
        teeLog(Tee_Error, "%s:%s:%d: Failed to Close Connection to the TEE: err %08x", MODULE, __func__, __LINE__, err);
        return 1;
    }

    return 0;
}

DRM_API DRM_RESULT DRM_CALL Oem_Hal_Initialize()
{
    if (Drm_Tee_Connect()) return DRM_E_FAIL;

    FUNC_ENTER;
    INIT(1);
    SET_OUT_VAL(0);
    DO_SEND(nfTO_DRM_OP_PR_INITIALIZE);
    GET_OUT_VAL(0, &result, NULL);
Exit:
    FUNC_EXIT;
    RETURN;
}

DRM_API DRM_RESULT DRM_CALL Oem_Hal_Deinitialize()
{
    FUNC_ENTER;
    INIT(1);
    SET_OUT_VAL(0);
    DO_SEND(nfTO_DRM_OP_PR_DEINITIALIZE);
    GET_OUT_VAL(0, &result, NULL);

#if 0
    if (Drm_Tee_Disconnect()) return DRM_E_FAIL; // wrong place to disconnect
#endif

Exit:
    FUNC_EXIT;
    RETURN;
}

DRM_API DRM_RESULT DRM_CALL Oem_Hal_VerifyMessageSignature(
    __in_bcount( f_cbMessage )   const DRM_BYTE                  *f_pbMessage,
    __in                               DRM_DWORD                  f_cbMessage,
    __in_bcount( f_cbSignature ) const DRM_BYTE                  *f_pbSignature,
    __in                               DRM_DWORD                  f_cbSignature,
    __in                               OEM_HAL_HASH_TYPE          f_eHashType,
    __in                               OEM_HAL_SIGNATURE_SCHEME   f_eSignatureScheme,
    __in                               OEM_HAL_KEY_REGISTER_INDEX f_indexIntegrityKey )
{
    FUNC_ENTER;
    nfTeeDrmPRVerifyMessageSignature_t args;
    memset(&args, 0, sizeof(args));
    args.f_cbMessage = f_cbMessage;
    args.f_cbSignature = f_cbSignature;
    args.f_eHashType = f_eHashType;
    args.f_eSignatureScheme = f_eSignatureScheme;
    args.f_indexIntegrityKey = f_indexIntegrityKey;

    INIT(4);

    SET_IN_REF(0, f_pbMessage, f_cbMessage);
    SET_IN_REF(1, f_pbSignature, f_cbSignature);
    SET_IN_REF(2, &args, sizeof(args));
    SET_OUT_VAL(3);

    DO_SEND(nfTO_DRM_OP_PR_VERIFY_MSG_SIGNATURE);

    GET_OUT_VAL(3, &result, NULL);

Exit:
    FUNC_EXIT;
    RETURN;
}

DRM_API DRM_RESULT DRM_CALL Oem_Hal_SignWithPrivateKey(
    __in                                DRM_VOID                  *f_pOEMContext, // ignore
    __inout_ecount_opt( 1 )             DRM_CRYPTO_CONTEXT        *f_pCryptoContext, // ignore
    __in_bcount( f_cbMessage )    const DRM_BYTE                  *f_pbMessage,
    __in                                DRM_DWORD                  f_cbMessage,
    __out_bcount_opt( *f_pcbSignature ) DRM_BYTE                  *f_pbSignature,
    __inout                             DRM_DWORD                 *f_pcbSignature,
    __in                                OEM_HAL_HASH_TYPE          f_eHashType,
    __in                                OEM_HAL_SIGNATURE_SCHEME   f_eSignatureScheme,
    __in                                OEM_HAL_KEY_TYPE           f_eKeyType,
    __in                                OEM_HAL_KEY_REGISTER_INDEX f_indexIntegrityKey )
{
    FUNC_ENTER;
    (void)f_pOEMContext;
    (void)f_pCryptoContext;

    nfTeeDrmPRSignWithPrivateKey_t args;
    memset(&args, 0, sizeof(args));
    args.f_cbMessage = f_cbMessage;
    args.f_cbSignature = *f_pcbSignature;
    args.f_eHashType = f_eHashType;
    args.f_eSignatureScheme = f_eSignatureScheme;
    args.f_eKeyType = f_eKeyType;
    args.f_indexIntegrityKey = f_indexIntegrityKey;

    INIT(4);

    PRTHEX(f_pbSignature, *f_pcbSignature);

    SET_IN_REF( 0, &args, sizeof(args));
    SET_IN_REF( 1, f_pbMessage, f_cbMessage);
    SET_OUT_REF(2, f_pbSignature, *f_pcbSignature);
    SET_OUT_VAL(3);

    DO_SEND(nfTO_DRM_OP_PR_SIGN_WITH_PRIVATE_KEY);

    GET_OUT_VAL(3, f_pcbSignature, &result);

    PRTHEX(f_pbSignature, *f_pcbSignature);

Exit:
    FUNC_EXIT;
    RETURN;
}

DRM_API DRM_RESULT DRM_CALL Oem_Hal_VerifyOMAC1Signature(
    __in_bcount( f_cbData )      const DRM_BYTE                   *f_pbData,
    __in                               DRM_DWORD                   f_cbData,
    __in_bcount( f_cbSignature ) const DRM_BYTE                   *f_pbSignature,
    __in                               DRM_DWORD                   f_cbSignature,
    __in                               OEM_HAL_KEY_TYPE            f_eKeyType,
    __in                               OEM_HAL_KEY_REGISTER_INDEX  f_indexKey )
{
    FUNC_ENTER;
    nfTeeDrmPRVerifyOMAC1Signature_t args;
    memset(&args, 0, sizeof(args));
    args.f_cbData = f_cbData;
    args.f_cbSignature = f_cbSignature;
    args.f_eKeyType = f_eKeyType;
    args.f_indexKey = f_indexKey;

    INIT(4);

    SET_IN_REF(0, f_pbData, f_cbData);
    SET_IN_REF(1, f_pbSignature, f_cbSignature);
    SET_IN_REF(2, &args, sizeof(args));
    SET_OUT_VAL(3);

    DO_SEND(nfTO_DRM_OP_PR_VERIFY_OMAC1_SIGNATURE);

    GET_OUT_VAL(3, &result, NULL);

Exit:
    FUNC_EXIT;
    RETURN;
}

DRM_API DRM_RESULT DRM_CALL Oem_Hal_CreateOMAC1Signature(
    __in_bcount( f_cbData )        const DRM_BYTE                   *f_pbData,
    __in                                 DRM_DWORD                   f_cbData,
    __out_bcount_opt( *f_pcbSignature )  DRM_BYTE                   *f_pbSignature,
    __inout                              DRM_DWORD                  *f_pcbSignature,
    __in                                 OEM_HAL_KEY_TYPE            f_eKeyType,
    __in                                 OEM_HAL_KEY_REGISTER_INDEX  f_indexKey )
{
    FUNC_ENTER;
    nfTeeDrmPRCreateOMAC1Signature_t args;
    memset(&args, 0, sizeof(args));
    args.f_cbData = f_cbData;
    args.f_cbSignature = *f_pcbSignature;
    args.f_eKeyType = f_eKeyType;
    args.f_indexKey = f_indexKey;

    INIT(4);

    PRTHEX(f_pbSignature, *f_pcbSignature);

    SET_IN_REF( 0, &args, sizeof(args));
    SET_IN_REF( 1, f_pbData, f_cbData);
    SET_OUT_REF(2, f_pbSignature, *f_pcbSignature);
    SET_OUT_VAL(3);

    DO_SEND(nfTO_DRM_OP_PR_CREATE_OMAC1_SIGNATURE);

    PRTHEX(f_pbSignature, *f_pcbSignature);

    GET_OUT_VAL(3, f_pcbSignature, &result);

Exit:
    FUNC_EXIT;
    RETURN;
}

DRM_API DRM_RESULT DRM_CALL Oem_Hal_EncryptDataBlock(
    __in_bcount( f_cbData )             const DRM_BYTE                   *f_pbBlock,
    __in                                      DRM_DWORD                   f_cbBlock,
    __out_bcount_opt( *f_pcbEncryptedBlock )  DRM_BYTE                   *f_pbEncryptedBlock,
    __inout                                   DRM_DWORD                  *f_pcbEncryptedBlock,
    __in                                      OEM_HAL_KEY_TYPE            f_eKeyType,
    __in                                      OEM_HAL_KEY_REGISTER_INDEX  f_indexMessageEncryptKey )
{
#if 1
    NRDP_UNUSED(f_pbBlock);
    NRDP_UNUSED(f_cbBlock);
    NRDP_UNUSED(f_pbEncryptedBlock);
    NRDP_UNUSED(f_pcbEncryptedBlock);
    NRDP_UNUSED(f_eKeyType);
    NRDP_UNUSED(f_indexMessageEncryptKey);
    NOT_SUPPORTED;
#else
    FUNC_ENTER;
    nfTeeDrmPREncryptDataBlock_t args;
    memset(&args, 0, sizeof(args));
    args.f_cbBlock = f_cbBlock;
    args.f_cbEncryptedBlock = *f_pcbEncryptedBlock;
    args.f_eKeyType = f_eKeyType;
    args.f_indexMessageEncryptKey = f_indexMessageEncryptKey;

    INIT(4);

    SET_IN_REF(0, f_pbBlock, f_cbBlock);
    SET_OUT_REF(1, f_pbEncryptedBlock, *f_pcbEncryptedBlock);
    SET_IN_REF(2, &args, sizeof(args));
    SET_OUT_VAL(3);

    DO_SEND(nfTO_DRM_OP_PR_ENCRYPT_DATA_BLOCK);

    GET_OUT_VAL(3, f_pcbEncryptedBlock, &result);

Exit:
    FUNC_EXIT;
    RETURN;
#endif
}

DRM_API DRM_BOOL DRM_CALL Oem_Hal_IsDevelopmentPlatform()
{
    FUNC_ENTER;
    return FALSE;
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
            /* printf("data ptr [%p %d]\n", key->pbEncryptedKey, key->cbEncryptedKey); */
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

DRM_API DRM_RESULT DRM_CALL Oem_Hal_UnwrapKey(
    __inout_ecount_opt( 1 )                    DRM_CRYPTO_CONTEXT         *f_pCryptoContext, // ignore
    __in                                       OEM_HAL_KEY_TYPE            f_eKeyType,
    __in                                       OEM_HAL_KEY_REGISTER_INDEX  f_indexKey,
    __in                                       OEM_HAL_KEY_TYPE            f_eWrappingKeyType,
    __in                                       OEM_HAL_KEY_REGISTER_INDEX  f_indexWrappingKey,
    __in_bcount( f_cbWrappedKeyData )    const DRM_BYTE                   *f_pbWrappedKeyData,
    __in                                       DRM_DWORD                   f_cbWrappedKeyData,
    __in_bcount_opt( f_cbParameterData ) const DRM_BYTE                   *f_pbParameterData,
    __in                                       DRM_DWORD                   f_cbParameterData )
{
    FUNC_ENTER;
    (void)f_pCryptoContext;
    DRM_BYTE tmp[1];

    nfTeeDrmPRUnwrapKey_t args;
    memset(&args, 0, sizeof(args));
    args.f_eKeyType = f_eKeyType;
    args.f_indexKey = f_indexKey;
    args.f_eWrappingKeyType = f_eWrappingKeyType;
    args.f_indexWrappingKey = f_indexWrappingKey;
    args.f_cbWrappedKeyData = f_cbWrappedKeyData;
    args.f_cbParameterData = f_cbParameterData;

    INIT(4);

    PRTHEX(f_pbWrappedKeyData, f_cbWrappedKeyData);
    PRTHEX(f_pbParameterData, f_cbParameterData);

    DRM_BOOL isEncKey = 0;
    DRM_BOOL isXmrUnwrapKey = 0;

    if (isDrmEncryptedKey(f_eWrappingKeyType, f_pbWrappedKeyData, f_cbWrappedKeyData, &isEncKey)) {
        result = DRM_E_FAIL;
        goto Exit;
    }

    if (!isEncKey && isXmrUnwrapInfo(f_eWrappingKeyType, f_pbWrappedKeyData, f_cbWrappedKeyData, &isXmrUnwrapKey)) {
        result = DRM_E_FAIL;
        goto Exit;
    }

    if (isEncKey) {
        memcpy(args.f_pbEncryptedKey, ((DRM_ENCRYPTED_KEY *)f_pbWrappedKeyData)->pbEncryptedKey, ((DRM_ENCRYPTED_KEY *)f_pbWrappedKeyData)->cbEncryptedKey);
    } else if (isXmrUnwrapKey) {
        const DRM_XMR_UNWRAP_INFO* pStruct = (DRM_XMR_UNWRAP_INFO*)f_pbWrappedKeyData;
        memcpy(args.f_pbEncryptedKey, pStruct->pbXMRLic, pStruct->cbXMRLic);
        if (pStruct->poChecksum) {
            memcpy(args.f_pbEncryptedKey + pStruct->cbXMRLic, pStruct->poChecksum, sizeof(DRM_BB_CHECKSUM_CONTEXT));
        }
    }

    SET_IN_OUT_REF(0, &args, sizeof(args));
    SET_IN_REF(1, f_pbWrappedKeyData, f_cbWrappedKeyData);
    if (f_cbParameterData == 1) {
        teeLog(Tee_Error, ">>>>>>>>>>> REE PANIC 1");
        exit(1);
    }

    if (f_cbParameterData) {
        SET_IN_REF(2, f_pbParameterData, f_cbParameterData);
    } else {
        SET_IN_REF(2, &tmp, 1);
    }
    SET_OUT_VAL(3);

    DO_SEND(nfTO_DRM_OP_PR_UNWRAP_KEY);

    GET_OUT_VAL(3, &result, NULL);

Exit:
    FUNC_EXIT;
    RETURN;
}

DRM_API DRM_RESULT DRM_CALL Oem_Hal_WrapKey(
    __in                                 DRM_VOID                   *f_pOEMContext, // ignore
    __inout_ecount_opt( 1 )              DRM_CRYPTO_CONTEXT         *f_pCryptoContext, // ignore
    __in                                 OEM_HAL_KEY_TYPE            f_eKeyType,
    __in                                 OEM_HAL_KEY_REGISTER_INDEX  f_indexKey,
    __in                                 OEM_HAL_KEY_TYPE            f_eWrappingKeyType,
    __in                                 OEM_HAL_KEY_REGISTER_INDEX  f_indexWrappingKey,
    __out_bcount_opt( *f_pcbWrappedKey ) DRM_BYTE                   *f_pbWrappedKey,
    __inout_ecount(1)                    DRM_DWORD                  *f_pcbWrappedKey )
{
    FUNC_ENTER;
    (void)f_pOEMContext;
    (void)f_pCryptoContext;

    nfTeeDrmPRWrapKey_t args;
    memset(&args, 0, sizeof(args));
    args.f_eKeyType = f_eKeyType;
    args.f_indexKey = f_indexKey;
    args.f_eWrappingKeyType = f_eWrappingKeyType;
    args.f_indexWrappingKey = f_indexWrappingKey;
    args.f_cbWrappedKey = *f_pcbWrappedKey;

    INIT(4);

    DRM_BOOL isEncKey = 0;
    DRM_BOOL isXmrUnwrapKey = 0;

    if (isDrmEncryptedKey(f_eWrappingKeyType, f_pbWrappedKey, *f_pcbWrappedKey, &isEncKey)) {
        result = DRM_E_FAIL;
        goto Exit;
    }

    if (!isEncKey && isXmrUnwrapInfo(f_eWrappingKeyType, f_pbWrappedKey, *f_pcbWrappedKey, &isXmrUnwrapKey)) {
        result = DRM_E_FAIL;
        goto Exit;
    }

    SET_IN_OUT_REF(0, &args, sizeof(args));
    SET_IN_REF(1, f_pbWrappedKey, *f_pcbWrappedKey);
    SET_IN_OUT_VAL(2, *f_pcbWrappedKey, NONE);
    SET_OUT_VAL(3);

    PRTHEX(f_pbWrappedKey, *f_pcbWrappedKey);

    DO_SEND(nfTO_DRM_OP_PR_WRAP_KEY);

    GET_IN_OUT_VAL(2, f_pcbWrappedKey, NULL);
    GET_IN_OUT_VAL(3, &result, NULL);

    if (isEncKey) {
        memcpy(((DRM_ENCRYPTED_KEY *)f_pbWrappedKey)->pbEncryptedKey, args.f_pbEncryptedKey, ((DRM_ENCRYPTED_KEY *)f_pbWrappedKey)->cbEncryptedKey);
    } else if (isXmrUnwrapKey) {
        DRM_XMR_UNWRAP_INFO* pStruct = (DRM_XMR_UNWRAP_INFO*)f_pbWrappedKey;
        memcpy((void*)pStruct->pbXMRLic, args.f_pbEncryptedKey, pStruct->cbXMRLic);
        if (pStruct->poChecksum) {
            memcpy((void*)pStruct->poChecksum, args.f_pbEncryptedKey + pStruct->cbXMRLic, sizeof(DRM_BB_CHECKSUM_CONTEXT));
        }
    }

    PRTHEX(f_pbWrappedKey, *f_pcbWrappedKey);

Exit:
    FUNC_EXIT;
    RETURN;
}

DRM_API DRM_RESULT DRM_CALL Oem_Hal_GenerateKey(
    __in                    DRM_VOID                  *f_pOEMContext, // ignore
    __inout_ecount_opt( 1 ) DRM_CRYPTO_CONTEXT        *f_pCryptoContext, // ignore
    __in                    OEM_HAL_KEY_TYPE           f_eKeyType,
    __in                    OEM_HAL_KEY_REGISTER_INDEX f_indexKey,
    __in                    DRM_DWORD                  f_dwRequestedSecurityLevel )
{
    FUNC_ENTER;
    (void)f_pOEMContext;
    (void)f_pCryptoContext;

    INIT(3);

    SET_IN_VAL(0, f_eKeyType, f_indexKey);
    SET_IN_VAL(1, f_dwRequestedSecurityLevel, NONE);
    SET_OUT_VAL(2);

    DO_SEND(nfTO_DRM_OP_PR_GENERATE_KEY);

    GET_OUT_VAL(2, &result, NULL);

Exit:
    FUNC_EXIT;
    RETURN;
}

DRM_API DRM_RESULT DRM_CALL Oem_Hal_DecryptContent(
    __inout_bcount( f_cbData ) DRM_BYTE                   *f_pbData,
    __in                       DRM_DWORD                   f_cbData,
    __in                       OEM_HAL_KEY_TYPE            f_eKeyType,
    __in                       OEM_HAL_KEY_REGISTER_INDEX  f_indexContentKey,
    __in                       DRM_UINT64                  f_qwInitializationVector,
    __in                       DRM_UINT64                  f_qwSampleByteOffset )
{
    FUNC_ENTER;
    nfTeeDrmPRDecryptContent_t args;
    args.f_cbData = f_cbData;
    args.f_eKeyType = f_eKeyType;
    args.f_indexContentKey = f_indexContentKey;
    args.f_qwInitializationVector = f_qwInitializationVector;
    args.f_qwSampleByteOffset = f_qwSampleByteOffset;

    INIT(3);

    SET_IN_REF(0, &args, sizeof(args));
    SET_IN_OUT_REF(1, f_pbData, f_cbData);
    SET_OUT_VAL(2);

    DO_SEND(nfTO_DRM_OP_PR_DECRYPT_CONTENT);

    GET_OUT_VAL(2, &result, NULL);

Exit:
    FUNC_EXIT;
    RETURN;
}

DRM_API DRM_RESULT DRM_CALL Oem_Hal_DecryptContentOpaque(
    __in                       OEM_OPAQUE_BUFFER_HANDLE    f_hInBuf,
    __in                       OEM_OPAQUE_BUFFER_HANDLE    f_hOutBuf,
    __in                       DRM_DWORD                   f_cbData,
    __in                       OEM_HAL_KEY_TYPE            f_eKeyType,
    __in                       OEM_HAL_KEY_REGISTER_INDEX  f_indexContentKey,
    __in                       DRM_UINT64                  f_qwInitializationVector,
    __in                       DRM_UINT64                  f_qwSampleByteOffset )
{
#if 1
    NRDP_UNUSED(f_hInBuf);
    NRDP_UNUSED(f_hOutBuf);
    NRDP_UNUSED(f_cbData);
    NRDP_UNUSED(f_eKeyType);
    NRDP_UNUSED(f_indexContentKey);
    NRDP_UNUSED(f_qwInitializationVector);
    NRDP_UNUSED(f_qwSampleByteOffset);
    NOT_SUPPORTED;
#else
    FUNC_ENTER;
    nfTeeDrmPRDecryptContentOpaque_t args;
    args.f_cbData = f_cbData;
    args.f_eKeyType = f_eKeyType;
    args.f_indexContentKey = f_indexContentKey;
    args.f_qwInitializationVector = f_qwInitializationVector;
    args.f_qwSampleByteOffset = f_qwSampleByteOffset;

    INIT(4);

    SET_IN_REF(0, &args, sizeof(args));
    SET_IN_REF(1, f_hInBuf, 1);
    SET_IN_REF(2, f_hOutBuf, 1);
    SET_OUT_VAL(3);

    DO_SEND(nfTO_DRM_OP_PR_DECRYPT_CONTENT_OPAQUE);

    GET_OUT_VAL(3, &result, NULL);

Exit:
    FUNC_EXIT;
    RETURN;
#endif
}

DRM_API DRM_RESULT DRM_CALL Oem_Hal_CreateSampleEncryptionContext(
    __in            OEM_HAL_KEY_TYPE                f_eKeyType,
    __in            OEM_HAL_KEY_REGISTER_INDEX      f_indexKey,
    __in            OEM_HAL_SAMPLE_ENCRYPTION_MODE  f_eSampleEncryptionMode,
    __out_ecount(1) OEM_ENCRYPTION_HANDLE          *f_phEncryptionContext )
{
#if 1
    NRDP_UNUSED(f_eKeyType);
    NRDP_UNUSED(f_indexKey);
    NRDP_UNUSED(f_eSampleEncryptionMode);
    NRDP_UNUSED(f_phEncryptionContext);
    NOT_SUPPORTED;
#else
    FUNC_ENTER;
    nfTeeDrmPRCreateSampleEncryptionContext_t args;
    args.f_eKeyType = f_eKeyType;
    args.f_indexKey = f_indexKey;
    args.f_eSampleEncryptionMode = f_eSampleEncryptionMode;

    INIT(3);

    SET_IN_REF(0, &args, sizeof(args));
    SET_OUT_REF(1, *f_phEncryptionContext, 1);
    SET_OUT_VAL(2);

    DO_SEND(nfTO_DRM_OP_PR_CREATE_SAMPLE_ENCRYPTION_CONTEXT);

    GET_OUT_VAL(2, &result, NULL);

Exit:
    FUNC_EXIT;
    RETURN;
#endif
}

DRM_API DRM_RESULT DRM_CALL Oem_Hal_EncryptSampleData(
    __in                           DRM_VOID                 *f_pOEMContext, // ignoring
    __in                           OEM_ENCRYPTION_HANDLE     f_hEncryptionContext,
    __out_ecount_opt(1)            DRM_UINT64               *f_pqwInitializationVector, // ignoring
    __out_ecount_opt(1)            OEM_HAL_SAMPLE_METADATA  *f_pMetadata, // ignoring
    __in_opt                       OEM_OPAQUE_BUFFER_HANDLE  f_hInBuf, // ignoring
    __in_opt                       OEM_OPAQUE_BUFFER_HANDLE  f_hOutBuf, // ignoring
    __in                           DRM_DWORD                 f_cbData,
    __inout_bcount_opt( f_cbData ) DRM_BYTE                 *f_pbData )
{
#if 1
    NRDP_UNUSED(f_pOEMContext);
    NRDP_UNUSED(f_hEncryptionContext);
    NRDP_UNUSED(f_pqwInitializationVector);
    NRDP_UNUSED(f_pMetadata);
    NRDP_UNUSED(f_hInBuf);
    NRDP_UNUSED(f_hOutBuf);
    NRDP_UNUSED(f_cbData);
    NRDP_UNUSED(f_pbData);
    NOT_SUPPORTED;
#else
    FUNC_ENTER;
    (void)f_pOEMContext;
    (void)f_hInBuf;
    (void)f_hOutBuf;
    (void)f_pqwInitializationVector;
    (void)f_pMetadata;

    INIT(4);

    SET_IN_REF(0, f_hEncryptionContext, 1);
    SET_IN_OUT_REF(1, f_pbData, f_cbData);
    SET_IN_VAL(2, f_cbData, NONE);
    SET_OUT_VAL(3);

    DO_SEND(nfTO_DRM_OP_PR_ENCRYPT_SAMPLE_DATA);

    GET_OUT_VAL(3, &result, NULL);

Exit:
    FUNC_EXIT;
    RETURN;
#endif
}

DRM_API DRM_RESULT DRM_CALL Oem_Hal_QuerySampleMetadata(
    __in            OEM_ENCRYPTION_HANDLE    f_hEncryptionContext,
    __out_ecount(1) OEM_HAL_SAMPLE_METADATA *f_pMetadata )
{
#if 1
    NRDP_UNUSED(f_hEncryptionContext);
    NRDP_UNUSED(f_pMetadata);
    NOT_SUPPORTED;
#else
    FUNC_ENTER;
    INIT(3);

    SET_IN_REF(0, f_hEncryptionContext, 1);
    SET_OUT_REF(1, f_pMetadata, 1);
    SET_OUT_VAL(2);

    DO_SEND(nfTO_DRM_OP_PR_QUERY_SAMPLE_METADATA);

    GET_OUT_VAL(2, &result, NULL);

Exit:
    FUNC_EXIT;
    RETURN;
#endif
}

DRM_API DRM_RESULT DRM_CALL Oem_Hal_DestroySampleEncryptionContext(
    __in OEM_ENCRYPTION_HANDLE f_hEncryptionContext )
{
#if 1
    NRDP_UNUSED(f_hEncryptionContext);
    NOT_SUPPORTED;
#else
    FUNC_ENTER;
    INIT(2);

    SET_IN_REF(0, f_hEncryptionContext, 1);
    SET_OUT_VAL(1);

    DO_SEND(nfTO_DRM_OP_PR_DESTROY_SAMPLE_ENCRYPTION_CONTEXT);

    GET_OUT_VAL(1, &result, NULL);

Exit:
    FUNC_EXIT;
    RETURN;
#endif
}

DRM_API DRM_RESULT DRM_CALL Oem_Hal_LoadPlayReadyRevocationInfo(
    __inout_ecount_opt( 1 )                 DRM_CRYPTO_CONTEXT         *f_pCryptoContext, // ignoring
    __in_bcount( f_cbRevocationInfo ) const DRM_BYTE                   *f_pbRevocationInfo,
    __in                                    DRM_DWORD                   f_cbRevocationInfo,
    __in                                    OEM_HAL_KEY_REGISTER_INDEX  f_indexSigningKey )
{
#if 1
    NRDP_UNUSED(f_pCryptoContext);
    NRDP_UNUSED(f_pbRevocationInfo);
    NRDP_UNUSED(f_cbRevocationInfo);
    NRDP_UNUSED(f_indexSigningKey);
    NOT_SUPPORTED;
#else
    FUNC_ENTER;
    (void)f_pCryptoContext;

    nfTeeDrmPRLoadPlayReadyRevocationInfo_t args;
    args.f_cbRevocationInfo = f_cbRevocationInfo;
    args.f_indexSigningKey = f_indexSigningKey;

    INIT(3);

    SET_IN_REF(0, &args, sizeof(args));
    SET_IN_REF(1, f_pbRevocationInfo, f_cbRevocationInfo);
    SET_OUT_VAL(2);

    DO_SEND(nfTO_DRM_OP_PR_LOAD_PLAYREADY_REVOCATION_INFO);

    GET_OUT_VAL(2, &result, NULL);

Exit:
    FUNC_EXIT;
    RETURN;
#endif
}

DRM_API DRM_RESULT DRM_CALL Oem_Hal_LoadPlayReadyCrl(
    __inout_ecount_opt( 1 )      DRM_CRYPTO_CONTEXT         *f_pCryptoContext, // ignoring
    __in_bcount( f_cbCRL ) const DRM_BYTE                   *f_pbCRL,
    __in                         DRM_DWORD                   f_cbCRL,
    __in                         OEM_HAL_KEY_REGISTER_INDEX  f_indexSigningKey )
{
#if 1
    NRDP_UNUSED(f_pCryptoContext);
    NRDP_UNUSED(f_pbCRL);
    NRDP_UNUSED(f_cbCRL);
    NRDP_UNUSED(f_indexSigningKey);
    NOT_SUPPORTED;
#else
    FUNC_ENTER;
    (void)f_pCryptoContext;

    nfTeeDrmPRLoadPlayReadyCrl_t args;
    args.f_cbCRL = f_cbCRL;
    args.f_indexSigningKey = f_indexSigningKey;

    INIT(3);

    SET_IN_REF(0, &args, sizeof(args));
    SET_IN_REF(1, f_pbCRL, f_cbCRL);
    SET_OUT_VAL(2);

    DO_SEND(nfTO_DRM_OP_PR_LOAD_PLAYREADY_CRL);

    GET_OUT_VAL(2, &result, NULL);

Exit:
    FUNC_EXIT;
    RETURN;
#endif
}
