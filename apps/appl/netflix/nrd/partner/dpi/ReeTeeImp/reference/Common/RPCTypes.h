/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

/**
 * @file RPCTypes.h
 * @author fpaun@netflix.com
 * @date April, 2014
 *
 * RPCTypes defines RPC types common among all containers.
 */

#ifndef RPC_TYPES_H_
#define RPC_TYPES_H_

#include "CommonIncludes.h"

/*----------------- Inter-Container Message Structure ------------------------*/

/* RPC Header Definitions */

#define nfUUID_SIZE_IN_BYTES 16
#define nfXID_SIZE_IN_BYTES 8

/**
 *  The RPC transport header encapsulates metadata for a logical connection
 * between two containers.
 */
typedef struct {
    uint32_t version;                   /**< Version of the RPC header. */
    uint8_t xid[nfXID_SIZE_IN_BYTES];   /**< Uniquely identifies an RPC exchange between two containers. */
    uint8_t uuid[nfUUID_SIZE_IN_BYTES]; /**< Uniquely identifies the service to receive the RPC payload. */
} nfRPCTransportHeader_t;


/** Version of the RPC transport header */
#define nfRPC_TRANSPORT_HEADER_VERSION 1

/* RPC Payload Definitions */

/** RPC operation code type. */
typedef uint32_t nfRPCOperationCode_t;

/** RPC return code type. */
typedef uint32_t nfRPCReturnCode_t;

/** The RPC code encapsulates the operation and result code of an RPC. */
typedef union {
    nfRPCOperationCode_t operationCode;
    nfRPCReturnCode_t returnCode;
} nfRPCCode_t;

/**
 * Set of values to describe two properties of an RPC parameter:
 * 1. Is the parameter passed by value or by memory reference?
 * 2. Is the parameter read-only, write-only or read-write with respect to
 * the RPC callee.
 */
typedef enum {
    nfRPC_PARAMETER_NONE = 0x0, /**< Parameter is not used. */

    nfRPC_PARAMETER_VALUE_READ_ONLY = 0x1,  /**< Parameter is passed by value and read-only. */
    nfRPC_PARAMETER_VALUE_WRITE_ONLY = 0x2, /**< Parameter is passed by value and write-only. */
    nfRPC_PARAMETER_VALUE_READ_WRITE = 0x3, /**< Parameter is passed by value and read-write. */

    nfRPC_PARAMETER_MEMORY_REFERENCE_READ_ONLY = 0x10,  /**< Parameter is passed by memory reference and read-only. */
    nfRPC_PARAMETER_MEMORY_REFERENCE_WRITE_ONLY = 0x20, /**< Parameter is passed by memory reference and write-only. */
    nfRPC_PARAMETER_MEMORY_REFERENCE_READ_WRITE = 0x30, /**< Parameter is passed by memory reference and write-read. */
} nfRPCParameterType_t;

/**
 * Fully qualifies the four RPC parameters:
 * 1. passed by value vs passed by memory reference
 * 2. passed as read-only vs write-only vs read-write
 */
typedef uint32_t nfRPCParameterDescription_t;

/**
 *  RPC parameter passed by value for efficient exchange of up to two four byte
 * immediate values between two containers.
 */
typedef struct {
    uint32_t a; /**< First 4 byte immediate value. */
    uint32_t b; /**< Second 4 byte immediate value. */
} nfRPCValue_t;

/**
 *  RPC parameter passed by memory reference for efficient exchange of bulk
 * data using shared memory between two containers.
 */
typedef struct {
    uint8_t *memPtr;    /**< The memory address storing the bulk data. */
    uint32_t memSize;   /**< The size of the bulk data. */
} nfRPCMemoryReference_t;

/**
 *  RPC parameter can be either a parameter passed by value or a parameter
 * passed by memory reference.
 */
typedef union {
    nfRPCValue_t value;
    nfRPCMemoryReference_t memReference;
} nfRPCParameter_t;

/** An RPC uses up to four parameters. */
#define MAXIMUM_NUMBER_OF_RPC_PARAMETERS 4

/** The RPC transport payload encapsulates the data needed to execute an RPC. */
typedef struct {
    nfRPCCode_t code;   /**< The RPC operation/result code. */
    nfRPCParameterDescription_t paramDescription;   /**< The description of the RPC params, one byte description for each param. */
    nfRPCParameter_t param[MAXIMUM_NUMBER_OF_RPC_PARAMETERS]; /**< Up to four RPC parameters. */
} nfRPCTransportPayload_t;

/*----------------- Inter-Container Message Passing --------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  This is the TEE entry point function definition.
 *
 *  @param rpcHeader, a buffer containing a serialized RPC transport header.
 *  @param rpcPayload, a buffer containing a serialized RPC transport payload.
 */
void nfTeeDispatchRPCMessage(uint8_t *rpcHeader, uint8_t *rpcPayload);

#ifdef __cplusplus
}
#endif


#endif /* RPC_TYPES_H_ */
