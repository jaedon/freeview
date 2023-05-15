/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "MessageUtils.h"

#include "RPCTypes.h"

uint32_t getRPCParamDescriptionOfValueTypeParam(nfValue_t *value) {
    switch (value->type)
    {
        case nfVALUE_READ_ONLY:
            return nfRPC_PARAMETER_VALUE_READ_ONLY;
        case nfVALUE_WRITE_ONLY:
            return nfRPC_PARAMETER_VALUE_WRITE_ONLY;
        case nfVALUE_READ_WRITE:
            return nfRPC_PARAMETER_VALUE_READ_WRITE;
        default:
            return nfRPC_PARAMETER_NONE;
    }
}

uint32_t getRPCParamDescriptionOfMemoryReferenceTypeParam(nfMemoryReference_t *memRef) {
    switch (memRef->type)
    {
        case nfMEMORY_REFERENCE_READ_ONLY:
            return nfRPC_PARAMETER_MEMORY_REFERENCE_READ_ONLY;
        case nfMEMORY_REFERENCE_WRITE_ONLY:
            return nfRPC_PARAMETER_MEMORY_REFERENCE_WRITE_ONLY;
        case nfMEMORY_REFERENCE_READ_WRITE:
            return nfRPC_PARAMETER_MEMORY_REFERENCE_READ_WRITE;
        default:
            return nfRPC_PARAMETER_NONE;
    }
}
