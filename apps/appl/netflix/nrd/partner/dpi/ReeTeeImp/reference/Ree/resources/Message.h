/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

/**
 * @file Message.h
 * @author fpaun@netflix.com
 * @date April, 2014
 *
 * Message implements the IMessage interface.
 */
#ifndef MESSAGE_H_
#define MESSAGE_H_

#include "MemoryReference.h"
#include "MessageUtils.h"
#include "RPCTypes.h"
#include "RPCUtils.h"
#include "Session.h"


/** Validate a session object.
 *
 * @param session is the session to be validated.
 * @return nfTAE_SUCCESS if session is valid,
 *         nfTAE_NULL_PARAMETER if the session pointer is null,
 *         nfTAE_INVALID_PARAMETER if the session is invalid.
 */
nfTeeApiError_t isValidSession(nfSession_t session);

/** Validate a list of parameters created by an REE client.
 *
 * @param params is an array of parameters to be validated.
 * @param numOfParams is the length of the array of parameters.
 * @return nfTAE_SUCCESS if all the parameters are valid,
 *         nfTAE_NULL_PARAMETER if the array of parameters is null,
 *         nfTAE_INVALID_NUMBER_OF_RPC_PARAMETERS if caller provided more than
 *      MAXIMUM_NUMBER_OF_PARAMETERS,
 *         nfTAE_INVALID_RPC_PARAMETER_TYPE if a parameter is invalid,
 *         nfTAE_INVALID_RPC_PARAMETER_VALUE_TYPE if a parameter of type value is
 *      invalid,
 *         nfTAE_INVALID_RPC_PARAMETER_MEMORY_REFERENCE_TYPE if a parameter of
 *      type memory reference is invalid.
 */
nfTeeApiError_t areValidParameters(const nfParameter_t *params, uint32_t numOfParams);

/** Validate a parameter created by an REE client.
 *
 * @param param is the parameter to be validated.
 * @return nfTAE_SUCCESS if all the parameters are valid,
 *         nfTAE_NULL_PARAMETER if the array of parameters is null,
 *      MAXIMUM_NUMBER_OF_PARAMETERS,
 *         nfTAE_INVALID_RPC_PARAMETER_TYPE if a parameter is invalid,
 *         nfTAE_INVALID_RPC_PARAMETER_VALUE_TYPE if a parameter of type value is
 *      invalid,
 *         nfTAE_INVALID_RPC_PARAMETER_MEMORY_REFERENCE_TYPE if a parameter of
 *      type memory reference is invalid.
 */
nfTeeApiError_t isValidParameter(const nfParameter_t *param);

/** Encode the RPC header structure into a binary value. */
void serializeRPCHeader(nfRPCTransportHeader_t *header, uint8_t *headerBuf);

/** Encode the RPC payload structure into a binary value. */
void serializeRPCPayload(nfRPCTransportPayload_t *payload, uint8_t *payloadBuf);

/** Decode the RPC result from a binary value. */
void deserializeRPCResult(uint8_t *payloadBuf, uint32_t *result);

/** Update the caller's parameters with the RPC result.
 *
 * @param payload is a payload structure whose contents were used to invoke the RPC.
 * @param payloadBuf is the binary value of the payload resulting from the RPC
 * execution.
 * @param params is the array of params provided by the caller.
 * @param numOfParams is the length of the params array.
 * @return nfTAE_SUCCESS if updating the params was successful,
 *         nfTAE_NULL_PARAMETER if payload, payloadBuf or params are null pointers.
 */
nfTeeApiError_t deserializeOutParameters(nfRPCTransportPayload_t *payload, uint8_t *payloadBuf, nfParameter_t *params, uint32_t numOfParams);


#endif
