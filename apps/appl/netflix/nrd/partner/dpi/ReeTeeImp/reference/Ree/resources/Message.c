/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include <string.h>

#include "IMessage.h"
#include "Message.h"

#include "MemoryReference.h"
#include "MessageUtils.h"
#include "RPCTypes.h"
#include "RPCUtils.h"
#include "Session.h"

#ifdef REE_MSG_DEBUG
#define MSG_DEBUG(...) fprintf(stderr, __VA_ARGS__)
#else
#define MSG_DEBUG(...)
#endif

nfTeeApiError_t nfSendMessage(nfSession_t session, uint32_t operation, nfParameter_t *params, uint32_t numOfParams, uint32_t *result)
{
    nfTeeApiError_t error;

    // Check incoming parameters.
    error = isValidSession(session);
    if (nfTAE_SUCCESS != error)
    {
        return error;
    }
    error = areValidParameters(params, numOfParams);
    if (nfTAE_SUCCESS != error)
    {
        return error;
    }
    if (NULL == result)
    {
        return nfTAE_NULL_PARAMETER;
    }

    MSG_DEBUG("%s: initializing xport header\n", __FUNCTION__);

    // Construct the header.
    nfRPCTransportHeader_t header;
    memset(&header, 0, sizeof header);

    header.version = nfRPC_TRANSPORT_HEADER_VERSION;
    memset(header.xid, 0, nfXID_SIZE_IN_BYTES);
    memcpy(header.uuid, session->serviceId, nfUUID_SIZE_IN_BYTES);

    // Construct the payload.
    nfRPCTransportPayload_t payload;
    memset(&payload, 0, sizeof payload);

    payload.code.operationCode = operation;

    MSG_DEBUG("%s: operation is %d\n", __FUNCTION__, operation);

    nfRESET_RPC_PARAMETER_DESCRIPTION(payload.paramDescription);
    uint32_t ctr = 0;
    for (ctr = 0; ctr < numOfParams; ctr++)
    {
        MSG_DEBUG("%s: init'ing param %d\n", __FUNCTION__, ctr);
        nfParameter_t *param = &(params[ctr]);
        if (nfPARAMETER_VALUE_TYPE == param->type)
        {
            nfValue_t *value = &param->data.value;
            uint32_t rpcValueDescr = getRPCParamDescriptionOfValueTypeParam(value);
            nfPASS_ITH_RPC_PARAMETER_BY(rpcValueDescr, ctr, payload.paramDescription);

            nfRPCValue_t *rpcValue = &payload.param[ctr].value;
            rpcValue->a = value->a;
            rpcValue->b = value->b;

            MSG_DEBUG("%s: init'ed immed param %d/%d\n", __FUNCTION__, value->a, value->b);
        }
        else if (nfPARAMETER_MEMORY_REFERENCE_TYPE == param->type)
        {
            nfMemoryReference_t *srcMemRef = &param->data.memRef;
            uint32_t rpcMemRefDescr = getRPCParamDescriptionOfMemoryReferenceTypeParam(srcMemRef);
            MSG_DEBUG("%s: rpcMemRefDescr is %08x\n", __FUNCTION__, rpcMemRefDescr);
            nfPASS_ITH_RPC_PARAMETER_BY(rpcMemRefDescr, ctr, payload.paramDescription);

            void *dstMemPtr = NULL;
            error = translateMemoryReferenceFromSourceToDestination(srcMemRef, &dstMemPtr);
            if (nfTAE_SUCCESS != error) {
                return error;
            }
            nfRPCMemoryReference_t *rpcMemRef = &payload.param[ctr].memReference;
            rpcMemRef->memPtr = dstMemPtr;
            rpcMemRef->memSize = srcMemRef->size;
            MSG_DEBUG("%s: init'ed mem param %p/%d\n", __FUNCTION__, dstMemPtr, srcMemRef->size);
        }
        else
        {
            MSG_DEBUG("%s: unrecognized param->type (%d)\n", __FUNCTION__, param->type);
            return nfTAE_INVALID_RPC_PARAMETER_TYPE;
        }
    }

    // Serialize the header
    uint8_t headerBuf[sizeof(nfRPCTransportHeader_t)] = { 0x0 };
    serializeRPCHeader(&header, headerBuf);

    // Serialize the payload
    uint8_t payloadBuf[sizeof(nfRPCTransportPayload_t)] = { 0x0 };
    serializeRPCPayload(&payload, payloadBuf);

    // Send the message
    nfTeeDispatchRPCMessage(headerBuf, payloadBuf);

    // Deserialize the return code
    deserializeRPCResult(payloadBuf, result);

    // Deserialize the out parameters
    error = deserializeOutParameters(&payload, payloadBuf, params, numOfParams);
    if (nfTAE_SUCCESS != error)
    {
        return error;
    }

    return nfTAE_SUCCESS;
}

void serializeRPCHeader(nfRPCTransportHeader_t *header, uint8_t *headerBuf)
{
    /* Serialize the version */
    nfPutWord32(headerBuf, header->version);
    headerBuf += sizeof header->version;

    /* Serialize the XID */
    memcpy(headerBuf, header->xid, nfXID_SIZE_IN_BYTES);
    headerBuf += nfXID_SIZE_IN_BYTES;

    /* Serialize the UUID */
    memcpy(headerBuf, header->uuid, nfUUID_SIZE_IN_BYTES);
}

void serializeRPCPayload(nfRPCTransportPayload_t *payload, uint8_t *payloadBuf) {
    /* Serialize the operation code */
    nfPutWord32(payloadBuf, payload->code.operationCode);
    payloadBuf += sizeof payload->code.operationCode;

    /* Serialize the parameter description */
    nfPutWord32(payloadBuf, payload->paramDescription);
    payloadBuf += sizeof(payload->paramDescription);

    /* Serialize the parameters */
    uint32_t ctr = 0;
    for (ctr = 0; ctr < MAXIMUM_NUMBER_OF_RPC_PARAMETERS; ctr++)
    {
        nfRPCParameter_t *param = payload->param + ctr;
        MSG_DEBUG("%s: payload->paramDescription is %08x\n", __FUNCTION__, payload->paramDescription);
        if (nfIS_ITH_RPC_PARAMETER_PASSED_BY_VALUE(ctr, payload->paramDescription))
        {
            nfRPCValue_t *value = &param->value;
            nfPutWord32(payloadBuf, value->a);
            payloadBuf += sizeof value->a;
            nfPutWord32(payloadBuf, value->b);
            payloadBuf += sizeof value->b;
            MSG_DEBUG("%s: passed by val, %d/%d\n", __FUNCTION__, value->a, value->b);
        }
        else if (nfIS_ITH_RPC_PARAMETER_PASSED_BY_REFERENCE(ctr, payload->paramDescription))
        {
            nfRPCMemoryReference_t *memRef = &param->memReference;
            uint32_t memPtrValue = (uint32_t)memRef->memPtr;
            nfPutWord32(payloadBuf, memPtrValue);
            payloadBuf += sizeof memPtrValue;
            nfPutWord32(payloadBuf, memRef->memSize);
            payloadBuf += sizeof memRef->memSize;
            MSG_DEBUG("%s: passed by ref, %p/%d\n", __FUNCTION__, memRef->memPtr, memRef->memSize);
        }
        else
        {
            MSG_DEBUG("%s: param is NONE, clearing payloadBuf\n", __FUNCTION__ );
            /* Parameter is NONE. */
            uint32_t zero = 0;
            nfPutWord32(payloadBuf, zero);
            payloadBuf += sizeof zero;
            nfPutWord32(payloadBuf, zero);
            payloadBuf += sizeof zero;
        }
    }
}

void deserializeRPCResult(uint8_t *payloadBuf, uint32_t *result)
{
    nfGetWord32(payloadBuf, result);
}

nfTeeApiError_t deserializeOutParameters(nfRPCTransportPayload_t *payload, uint8_t *payloadBuf, nfParameter_t *params, uint32_t numOfParams)
{
    if (NULL == payload || NULL == payloadBuf || NULL == params)
    {
        return nfTAE_NULL_PARAMETER;
    }

    // Return RPC out values
    uint32_t paramDescription = payload->paramDescription;
    uint32_t offset = sizeof(payload->code) + sizeof(paramDescription);
    uint32_t paramSizeInBytes = sizeof(payload->param[0]);
    uint32_t ctr;

    MSG_DEBUG("%s: %d parms...\n", __FUNCTION__, numOfParams );

    for (ctr = 0; ctr < numOfParams; ctr++)
    {
        nfRPCParameter_t *param = payload->param + ctr;
        uint32_t paramOffset = offset + ctr * paramSizeInBytes;
        if (nfIS_ITH_RPC_PARAMETER_PASSED_BY_VALUE(ctr, paramDescription) &&
                nfIS_ITH_RPC_PARAMETER_PASSED_BY_VALUE_AND_CAN_WRITE(ctr, paramDescription))
        {
            nfRPCValue_t rpcValue;
            nfGetWord32(payloadBuf + paramOffset, &rpcValue.a);
            nfGetWord32(payloadBuf + paramOffset + sizeof rpcValue.a, &rpcValue.b);

            nfValue_t *value = &params[ctr].data.value;
            value->a = rpcValue.a;
            value->b = rpcValue.b;
        }
        else if(nfIS_ITH_RPC_PARAMETER_PASSED_BY_REFERENCE(ctr, paramDescription) &&
                nfIS_ITH_RPC_PARAMETER_PASSED_BY_REFERENCE_AND_CAN_WRITE(ctr, paramDescription))
        {
            nfRPCMemoryReference_t *rpcMemRef = &param->memReference;

            uint32_t memPtrValue = 0;
            nfGetWord32(payloadBuf + paramOffset, &memPtrValue);
            rpcMemRef->memPtr = (void *)(uintptr_t)memPtrValue;
            nfGetWord32(payloadBuf + paramOffset + sizeof memPtrValue, &rpcMemRef->memSize);

            /**
             *  The translation of a param, passed by reference that the TEE
             * modified, from the RPC payload to a param that the caller can
             * use is implementation specific.
             *
             *  This reference implementation assumes the REE and TEE have
             * access to the same virtual memory space therefore the REE
             * has access to the contents of what the param is pointing at
             * by default. This assumption does not hold for all
             * implementations. Each implementation must make available
             * the contents stored at:
             *      rpcMemRef->memPtr
             * to the caller, i.e. return the result of the TEE operation.
             */
        }
        else
        {
            /* Parameter is of type NONE: nothing more to do. */
        }
    }

    return nfTAE_SUCCESS;
}

nfTeeApiError_t isValidSession(nfSession_t session)
{
    if (NULL == session)
    {
        return nfTAE_NULL_PARAMETER;
    }

    if (NULL == session->connection)
    {
        return nfTAE_INVALID_PARAMETER;
    }

    return nfTAE_SUCCESS;
}

nfTeeApiError_t areValidParameters(const nfParameter_t *params, uint32_t numOfParams)
{
    if (NULL == params && 0 == numOfParams)
    {
        return nfTAE_SUCCESS;
    }

    if (NULL == params)
    {
        return nfTAE_NULL_PARAMETER;
    }

    if (MAXIMUM_NUMBER_OF_PARAMETERS < numOfParams)
    {
        return nfTAE_INVALID_NUMBER_OF_RPC_PARAMETERS;
    }

    uint32_t ctr = 0;
    for (ctr = 0; ctr < numOfParams; ctr++)
    {
        nfTeeApiError_t error = isValidParameter(&params[ctr]);
        if (nfTAE_SUCCESS != error)
        {
            return error;
        }
    }

    return nfTAE_SUCCESS;
}


nfTeeApiError_t isValidParameter(const nfParameter_t *param)
{
    if (NULL == param) {
        return nfTAE_NULL_PARAMETER;
    }

    if (nfPARAMETER_MEMORY_REFERENCE_TYPE == param->type)
    {
        // Validate the memory reference parameter.
        const nfMemoryReference_t *memRef = &param->data.memRef;
        if (memRef->addr == NULL)
        {
            return nfTAE_INVALID_RPC_PARAMETER_MEMORY_REFERENCE_TYPE;
        }
        if (0 == memRef->size)
        {
            return nfTAE_INVALID_RPC_PARAMETER_MEMORY_REFERENCE_TYPE;
        }
        if (nfMEMORY_REFERENCE_READ_ONLY != memRef->type &&
                nfMEMORY_REFERENCE_WRITE_ONLY != memRef->type &&
                nfMEMORY_REFERENCE_READ_WRITE != memRef->type)
        {
            return nfTAE_INVALID_RPC_PARAMETER_MEMORY_REFERENCE_TYPE;
        }
        if (NULL == memRef->imp)
        {
            return nfTAE_INVALID_RPC_PARAMETER_MEMORY_REFERENCE_TYPE;
        }
        if (nfMEMORY_REFERENCE_REGISTERED != memRef->imp->type &&
                nfMEMORY_REFERENCE_ALLOCATED != memRef->imp->type)
        {
            return nfTAE_INVALID_RPC_PARAMETER_MEMORY_REFERENCE_TYPE;
        }
    }
    else if (nfPARAMETER_VALUE_TYPE == param->type)
    {
        // Validate the value parameter.
        const nfValue_t *val = &param->data.value;
        if (nfVALUE_READ_ONLY != val->type &&
                nfVALUE_WRITE_ONLY != val->type &&
                nfVALUE_READ_WRITE != val->type)
        {
            return nfTAE_INVALID_RPC_PARAMETER_VALUE_TYPE;
        }
        return nfTAE_SUCCESS;
    }
    else
    {
        return nfTAE_INVALID_RPC_PARAMETER_TYPE;
    }

    return nfTAE_SUCCESS;
}

