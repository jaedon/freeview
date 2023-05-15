/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

/**
 * @file IMessage.h
 * @author fpaun@netflix.com
 * @date April, 2014
 *
 * IMessage invokes an RPC between a client application and a service
 * application.
 */
#ifndef I_MESSAGE_H_
#define I_MESSAGE_H_

#include <stdint.h>

#include <nrddpi/ISession.h>
#include <nrddpi/IMemoryReference.h>
#include <nrddpi/IValue.h>
#include "TeeApiErrorTypes.h"

#include <nrddpi/RPCErrorTypes.h>

/**
 * The type of RPC parameter.
 */
typedef enum {
   nfPARAMETER_VALUE_TYPE = 1,          /**< Parameter passed by value. */
   nfPARAMETER_MEMORY_REFERENCE_TYPE    /**< Parameter passed by reference. */
} nfParameterType_t;

/**
 * The type of RPC parameter data.
 */
typedef union {
    nfValue_t value;            /**< Data is a value. */
    nfMemoryReference_t memRef; /**< Data is a memory pointer. */
} nfParameterData_t;

/**
 * An RPC parameter.
 */
typedef struct {
    nfParameterType_t type; /**< Type of RPC parameter: by-value or by-reference. */
    nfParameterData_t data; /**< The RPC parameter data. */
} nfParameter_t;


/** Maximum number of parameters supported by an RPC. */
#define MAXIMUM_NUMBER_OF_PARAMETERS 4

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Invoke an RPC between a client application and a service application.
 *
 * @param session is the logical session-based connection between a client
 * application and a service application.
 * @param operation is the RPC operation to be executed by the RPC callee.
 * @param params are the RPC parameters for the RPC operation.
 * @param numOfParams are the number of provided RPC parameters.
 * @param result is the status returned by the RPC operation.
 * @return nfTAE_SUCCESS if allocation of shared memory is successful,
 *         nfTAE_NULL_PARAMETER if session, params or result are NULL,
 *         nfTAE_INVALID_PARAMETER if the parameters are malformed.
 *         nfTAE_INVALID_NUMBER_OF_RPC_PARAMETERS if numOfParams is larger
 *      then MAXIMUM_NUMBER_OF_PARAMETERS,
 *         nfTAE_INVALID_RPC_PARAMETER_TYPE if an RPC param has a malformed type,
 *         nfTAE_INVALID_RPC_PARAMETER_VALUE_TYPE if a parameter of type value is
 *      invalid,
 *         nfTAE_INVALID_RPC_PARAMETER_MEMORY_REFERENCE_TYPE if a parameter of
 *      type memory reference is invalid.
 */
nfTeeApiError_t nfSendMessage(nfSession_t session, uint32_t operation, nfParameter_t *params, uint32_t numOfParams, uint32_t *result);

#ifdef __cplusplus
}
#endif


#endif
