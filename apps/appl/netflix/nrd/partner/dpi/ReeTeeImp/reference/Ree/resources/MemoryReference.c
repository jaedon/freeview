/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include <stdlib.h>
#include <string.h>

#include "IMemoryReference.h"
#include "MemoryReference.h"
#include "TeeApiErrorTypes.h"

/* Caller sets size and type of the memory block */
nfTeeApiError_t nfMemoryReferenceAllocate(const nfSession_t session, nfMemoryReference_t *memRef) {

    // Check the in parameters.
    if (session == NULL || memRef == NULL)
    {
        return nfTAE_NULL_PARAMETER;
    }
    if (memRef->type != nfMEMORY_REFERENCE_READ_ONLY &&
            memRef->type != nfMEMORY_REFERENCE_WRITE_ONLY &&
            memRef->type != nfMEMORY_REFERENCE_READ_WRITE)
    {
        return nfTAE_INVALID_PARAMETER;
    }

    // Initialize the memory reference object
    void *addr = malloc(memRef->size);
    nfMemoryReferenceImp_t imp = (nfMemoryReferenceImp_t) malloc(sizeof(struct nfMemoryReferenceImp_));
    if (NULL == addr || NULL == imp)
    {
        if (addr)
        {
            free(addr);
        }
        if (imp)
        {
            free(imp);
        }
        return nfTAE_OUT_OF_MEMORY;
    }

    memset(addr, 0, memRef->size);
    memRef->addr = addr;

    imp->type = nfMEMORY_REFERENCE_ALLOCATED;
    memRef->imp = imp;

    return nfTAE_SUCCESS;
}

/* Caller sets the addr, size and type of the memory block. */
nfTeeApiError_t nfMemoryReferenceRegister(const nfSession_t session, nfMemoryReference_t *memRef) {
    if (session == NULL || memRef == NULL)
    {
        return nfTAE_NULL_PARAMETER;
    }
    if (memRef->type != nfMEMORY_REFERENCE_READ_ONLY &&
            memRef->type != nfMEMORY_REFERENCE_WRITE_ONLY &&
            memRef->type != nfMEMORY_REFERENCE_READ_WRITE)
    {
        return nfTAE_INVALID_PARAMETER;
    }
    if (memRef->addr == NULL || memRef->size == 0)
    {
        return nfTAE_INVALID_PARAMETER;
    }

    // Initialize the memory reference object
    nfMemoryReferenceImp_t imp = (nfMemoryReferenceImp_t) malloc(sizeof(struct nfMemoryReferenceImp_));
    if (NULL == imp)
    {
        return nfTAE_OUT_OF_MEMORY;
    }
    imp->type = nfMEMORY_REFERENCE_REGISTERED;
    memRef->imp = imp;

    return nfTAE_SUCCESS;
}


/* Must pass a nfMemoryReference_t that has been allocated or registered. */
nfTeeApiError_t nfMemoryReferenceRelease(nfMemoryReference_t *memRef) {
    if (memRef == NULL)
    {
        return nfTAE_NULL_PARAMETER;
    }
    if (memRef->type != nfMEMORY_REFERENCE_READ_ONLY &&
            memRef->type != nfMEMORY_REFERENCE_WRITE_ONLY &&
            memRef->type != nfMEMORY_REFERENCE_READ_WRITE)
    {
        return nfTAE_INVALID_PARAMETER;
    }
    if (memRef->imp == NULL)
    {
        return nfTAE_INVALID_PARAMETER;
    }

    if (memRef->imp->type == nfMEMORY_REFERENCE_ALLOCATED && memRef->addr)
    {
        // Free the allocated memory.
        free(memRef->addr);
        memRef->addr = NULL;
    }
    else if (memRef->imp->type == nfMEMORY_REFERENCE_REGISTERED)
    {
        // Do nothing, the caller allocated the memory.
    }
    else
    {
        return nfTAE_INVALID_PARAMETER;
    }

    // Free the memory allocated for the internal implementation.
    free(memRef->imp);
    memRef->imp = NULL;

    return nfTAE_SUCCESS;
}

nfTeeApiError_t translateMemoryReferenceFromSourceToDestination(nfMemoryReference_t *srcMemRef, void **dstMemPtr)
{
    if (NULL == srcMemRef || NULL == dstMemPtr)
    {
        return nfTAE_NULL_PARAMETER;
    }

    *dstMemPtr = srcMemRef->addr;

    return nfTAE_SUCCESS;
}

nfTeeApiError_t translateMemoryReferenceFromDestinationToSource(void *dstMemPtr, nfMemoryReference_t *srcMemRef)
{
    if (NULL == srcMemRef || NULL == dstMemPtr)
    {
        return nfTAE_NULL_PARAMETER;
    }

    srcMemRef->addr = dstMemPtr;

    return nfTAE_SUCCESS;
}
