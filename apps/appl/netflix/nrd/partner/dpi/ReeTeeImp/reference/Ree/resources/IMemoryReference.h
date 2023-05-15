/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

/**
 * @file IMemoryReference.h
 * @author fpaun@netflix.com
 * @date April, 2014
 *
 * IMemoryReference abstracts a passed-by-reference parameter for an RPC between
 * a client application and a service application.
 *
 * IMemoryReference designates shared memory between a client application and a
 * service application to facilitate efficient exchange of large data.
 *
 * For example, use IMemoryReference to decrypt the contents of a large buffer.
 */
#ifndef I_MEMORY_REFERENCE_H_
#define I_MEMORY_REFERENCE_H_

#include <stdint.h>

#include "ISession.h"
#include "TeeApiErrorTypes.h"

/**
 * Indicate the direction of data flow with respect to the RPC callee.
 */
typedef enum {
   nfMEMORY_REFERENCE_READ_ONLY = 1,/**< Read only access for RPC callee. */
   nfMEMORY_REFERENCE_WRITE_ONLY,   /**< Write only access for RPC callee. */
   nfMEMORY_REFERENCE_READ_WRITE    /**< Read and write access for RPC callee. */
} nfMemoryReferenceReadWriteType_t;

/**
 * Abstracts implementation details for supporting shared memory capabilities.
 */
typedef struct nfMemoryReferenceImp_ * nfMemoryReferenceImp_t;

/**
 * Convenience container to describe a memory block where a client application
 * and service application can store/load data.
 */
typedef struct {
    void *addr;     /**< Shared memory pointer. */
    uint32_t size;  /**< Size of shared memory. */
    nfMemoryReferenceReadWriteType_t type; /**< Read, write or read/write type of shared memory. */
    nfMemoryReferenceImp_t imp; /**< Implementation specific. */
} nfMemoryReference_t;


#ifdef __cplusplus
extern "C" {
#endif

/**
 * Allocate a block of shared memory. Caller specifies size and type of the
 * shared memory block. The underlying implementation return the memory
 * address for the allocated memory block. The underlying implementation owns
 * the returned allocated memory block.
 *
 * @param session is the logical session-based connection between a client
 * application and a service application.
 * @param memRef specifies the size and type of the requested shared memory
 * block
 * @return nfTAE_SUCCESS if allocation of shared memory is successful,
 *         nfTAE_NULL_PARAMETER if the session or memRef params are null,
 *         nfTAE_INVALID_PARAMETER if the memRef.type is invalid,
 *         nfTAE_OUT_OF_MEMORY if the memRef param cannot be initialized due to memory constrains
 *
 */
nfTeeApiError_t nfMemoryReferenceAllocate(const nfSession_t session, nfMemoryReference_t *memRef);

/**
 * Register a block of shared memory. Caller specifies the memory address and
 * size of the memory block to be registered and the type of shared memory
 * block. The underlying implementation does not own the provided memory
 * address that it registered to a shared memory block.
 *
 * @param session is the logical session-based connection between a client
 * application and a service application.
 * @param memRef specifies the memory pointer and size of the memory block
 * to be registered, and the type of the requested shared memory.
 * @return nfTAE_SUCCESS if allocation of shared memory is successful,
 *         nfTAE_NULL_PARAMETER if the session or memRef params are null,
 *         nfTAE_INVALID_PARAMETER if the memRef.type is invalid, if the memRef.addr is NULL or memRef.size is zero.
 *         nfTAE_OUT_OF_MEMORY if the memRef param cannot be initialized due to memory constrains
 */
nfTeeApiError_t nfMemoryReferenceRegister(const nfSession_t session, nfMemoryReference_t *memRef);

/**
 * Release an allocated or registerd shared memory block.
 *
 * @param memRef must reference a shared memory that has been allocated or registered,
 * further usage of this parameter results in an undefined behaviour.
 * @return nfTAE_SUCCESS if allocation of shared memory is successful,
 *         nfTAE_NULL_PARAMETER if the memRef parameter is NULL,
 *         nfTAE_INVALID_PARAMETER if memRef.type member is set to an invalid value or the memRef.imp member is NULL or corrupted.
 *
 */
nfTeeApiError_t nfMemoryReferenceRelease(nfMemoryReference_t *memRef);

#ifdef __cplusplus
}
#endif


#endif
