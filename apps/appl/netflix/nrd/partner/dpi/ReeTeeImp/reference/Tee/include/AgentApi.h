/*
 * AgentApi.h
 *
 *  Created on: May 3, 2014
 *      Author: scott
 */

#ifndef AGENTAPI_H_
#define AGENTAPI_H_

#include "RPCTypes.h"
#include "RPCUtils.h"


/**
 *
 */
typedef enum
{
    nfREAD,
    nfWRITE,
    nfREAD_WRITE
} nfAgentApiPermission_t;

/**
 * The type of RPC parameter.
 */
typedef enum
{
	nfAAPT_UNDEFINED,
	nfAAPT_VALUE,
	nfAAPT_MEMREF,

	// leave this last!!
	nfAAPT_INVALID
} nfAgentApiParameterType_t;

/**
 * The type of RPC parameter data.
 */
typedef union
{
	nfRPCValue_t 			Value;            /**< Data is a value. */
    nfRPCMemoryReference_t	MemRef; /**< Data is a memory pointer. */
} nfAgentApiParameterData_t;

/**
 * An RPC parameter.
 */
typedef struct
{
    nfAgentApiPermission_t      Permission; /**< Type of read/write permissions. */
    nfAgentApiParameterType_t 	Type; /**< Type of RPC parameter: by-value or by-reference. */
    nfAgentApiParameterData_t	data; /**< The RPC parameter data. */
} nfAgentApiParameter_t;


/** Maximum number of parameters supported by an RPC. */
#define nfTEE_MAX_PARAMS MAXIMUM_NUMBER_OF_RPC_PARAMETERS


#endif /* AGENTAPI_H_ */
