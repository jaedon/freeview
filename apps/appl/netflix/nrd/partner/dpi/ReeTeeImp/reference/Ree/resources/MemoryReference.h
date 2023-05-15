/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

/**
 * @file MemoryReference.h
 * @author fpaun@netflix.com
 * @date April, 2014
 *
 * MemoryReference implements interface IMemoryReference.
 */
#ifndef MEMORY_REFERENCE_H_
#define MEMORY_REFERENCE_H_

#include "IMemoryReference.h"
#include "TeeApiErrorTypes.h"

/**
 *  A memory reference object encapsulates either
 *  - a registered block of memory which is not managed by this implementation
 *  - an allocated block of memory which is managed by this implementation.
 */
typedef enum {
    nfMEMORY_REFERENCE_REGISTERED = 1,
    nfMEMORY_REFERENCE_ALLOCATED
} nfMemoryReferenceImpType;

/** Internal implementation of a memory reference object. */
struct nfMemoryReferenceImp_ {
    nfMemoryReferenceImpType type;      /**< Allocated or registered. */
};

#ifdef __cplusplus
extern "C" {
#endif

/** Translate a memory address from the memory space of the source container to the memory space of the destination container. */
nfTeeApiError_t translateMemoryReferenceFromSourceToDestination(nfMemoryReference_t *srcMemRef, void **dstMemPtr);

/** Translate a memory address from the memory space of the destination container to the memory space of the source container. */
nfTeeApiError_t translateMemoryReferenceFromDestinationToSource(void *dstMemPtr, nfMemoryReference_t *srcMemRef);

#ifdef __cplusplus
}
#endif


#endif /* MEMORY_REFERENCE_H_ */
