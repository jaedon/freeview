/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef STORAGETYPES_H_
#define STORAGETYPES_H_

#include "CommTypes.h"
#include "CommonIncludes.h"

/*
 * storage agent messages
 */

typedef enum
{
	nfTO_STORAGE_OP_SIMPLE_READ = nfTOB_STORAGE_AGENT + 1,
	nfTO_STORAGE_OP_COMPOUND_READ,
	nfTO_STORAGE_OP_PROTECT,
	nfTO_STORAGE_OP_UNPROTECT,
	// insert new message types here...

    nfTO_STORAGE_OP_INVALID // leave this last!!
} nfTeeOperation_StorageOp_t;

#define nfTEE_IS_STORAGE_OP(x) (((x) > nfTOB_STORAGE_AGENT) && ((x) < nfTO_STORAGE_OP_INVALID))

/*
 * Secure store data elements are defined by a "key". To keep
 * the TEE code simple, keys are 32-bit values (no strings).
 */
typedef uint32_t nfSecureStoreClientKey_t;

typedef struct
{
	nfSecureStoreClientKey_t	RequestedKey;
} nfTeeStorageSimpleReadReq_t;

#define nfMIN_SIMPLE_READ_REQ_SIZE sizeof(nfTeeStorageSimpleReadReq_t)
#define nfTEE_SIMPLE_READ_REQ_PARAM_COUNT	3

typedef struct
{
	nfTeeClientId_t	ClientId; // client id associated with this data
	uint32_t		DataLength;
	uint8_t			Data[/*DataLength*/];
} nfTeeStorageSimpleReadRsp_t;

#define nfMIN_SIMPLE_READ_RSP_SIZE sizeof(nfTeeStorageSimpleReadRsp_t)

/*
 * For application secure store, the app requests "protect" operations to
 * encrypt/authenticate date, and "unprotect" operations to verify/decrypt
 * data. It is expected that the application will ensure that the item to
 * be protected/unprotected fits entirely in a single message. We may
 * add multi-message data support in the future (but not today).
 *
 * When you protect data, it is expanded according to the following format:
 *
 * {version}{IV}{encrypted data}{HMAC}
 * |-------------RawLength ----------|
 */

#if defined(HUMAX_NRDP_4_2_0) /* HUMAX PATCH : should be checked Security Part */
#define nfTEE_STORAGE_OVERHEAD	(4+16+32+15)
#define nfTEE_MAX_INTERNAL_SIZE	(32768*4+43780) // SDK requires 128K
#define nfTEE_MAX_PROTECT_SIZE (((3*nfTEE_MAX_INTERNAL_SIZE)/4)-nfTEE_STORAGE_OVERHEAD)
#endif

// TBD: define all related defs somewhere central so defs can be shared...
#define nfTEE_MIN_PROTECTED_DATA_SIZE (sizeof(uint32_t) + 16 + 16 + 32)

typedef struct
{
	uint32_t		DataLength;
	uint8_t			Data[/*DataLength*/];
} nfTeeStorageProtectReq_t;

#define nfMIN_STORAGE_PROTECT_REQ_SIZE (sizeof(nfTeeStorageProtectReq_t) + 1)
#define nfTEE_STORAGE_PROTECT_REQ_PARAM_COUNT	2

typedef struct
{
	uint32_t		ProtectedDataLength;
	uint8_t			ProtectedData[/*DataLength*/];
} nfTeeStorageProtectRsp_t;

#define nfMIN_STORAGE_PROTECT_RSP_SIZE (sizeof(nfTeeStorageProtectRsp_t) + nfTEE_MIN_PROTECTED_DATA_SIZE)

typedef struct
{
	uint32_t		ProtectedDataLength;
	uint8_t			ProtectedData[/*DataLength*/];
} nfTeeStorageUnprotectReq_t;

#define nfMIN_STORAGE_UNPROTECT_REQ_SIZE (sizeof(nfTeeStorageUnprotectReq_t) + nfTEE_MIN_PROTECTED_DATA_SIZE)
#define nfTEE_STORAGE_UNPROTECT_REQ_PARAM_COUNT 2

typedef struct
{
	uint32_t		DataLength;
	uint8_t			Data[/*DataLength*/];
} nfTeeStorageUnprotectRsp_t;

#define nfMIN_STORAGE_UNPROTECT_RSP_SIZE (sizeof(nfTeeStorageUnprotectRsp_t) + 1)

#endif /* STORAGETYPES_H_ */
