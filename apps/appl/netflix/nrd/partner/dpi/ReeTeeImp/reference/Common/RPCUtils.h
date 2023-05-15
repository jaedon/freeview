/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */


/**
 * @file RPCUtils.h
 * @author fpaun@netflix.com
 * @date April, 2014
 *
 * RPCUtils defines convenience methods and macros to encode and decode data
 * between two containers.
 */
#ifndef RPC_UTILS_H_
#define RPC_UTILS_H_

#include "RPCTypes.h"

/**
 * Convenience methods for encoding and decoding primitive types. All
 * containers must use these methods to encode and decode primitive
 * types when exchanging RPC meta-data.
 */

/** Encode a four byte unisnged integer to a binary value. */
void nfPutWord32(uint8_t *dst, uint32_t value);
/** Decode a four byte unisnged integer from a binary value. */
void nfGetWord32(uint8_t *src, uint32_t *value);


/**
 * Convenience macros for encoding and decoding RPC parameter types. All
 * containers must use these macros to encode and decode RPC parameter types.
 *
 * Encode up to four parameter types into a four byte value. Reserve one byte
 * for each parameter as follows:
 * - parameter 0 uses bit 0 through bit 7
 * - parameter 1 uses bit 8 through bit 15
 * - parameter 2 uses bit 16 through bit 23
 * - parameter 3 uses bit 24 through bit 31
 *
 * For each parameter, reserve the lower four bits to describe a parameter
 * passed by value and reserve the high four bits to describe a parameter
 * passed by reference.
 */

/** Eight bits describe a parameter type and permissions. */
#define nfRPC_PARAMETER_DESCRIPTION_MASK 0xFF

/** Bit 0 and 1 describe a parameter passed by value. */
#define nfRPC_PARAMETER_PASSED_BY_VALUE_BIT_MASK 0xFC

/** Bit 4 and 5 describe a parameter passed by reference. */
#define nfRPC_PARAMETER_PASSED_BY_REFERENCE_BIT_MASK 0xCF


/** Describe the ith parameter, i must be between 0 and 3. */
#define nfPASS_ITH_RPC_PARAMETER_BY(passByMethod, i, rpcParameterDescription) \
    rpcParameterDescription &= (~(0xFFU << ((i) * 8))); \
    rpcParameterDescription |= (((passByMethod) & 0xFFU) << ((i) * 8));

/**
 * Reset the parameter description by setting the descrption of each parameter,
 * to the value 0, i.e. parameter is not used.
 */
#define nfRESET_RPC_PARAMETER_DESCRIPTION(rpcParameterDescription) \
    nfPASS_ITH_RPC_PARAMETER_BY(nfRPC_PARAMETER_NONE, 0, rpcParameterDescription); \
    nfPASS_ITH_RPC_PARAMETER_BY(nfRPC_PARAMETER_NONE, 1, rpcParameterDescription); \
    nfPASS_ITH_RPC_PARAMETER_BY(nfRPC_PARAMETER_NONE, 2, rpcParameterDescription); \
    nfPASS_ITH_RPC_PARAMETER_BY(nfRPC_PARAMETER_NONE, 3, rpcParameterDescription);


/**
 *  @param i must be between 0 and 3.
 *  @param rpcParameterDescription encodes the description of all four parameters.
 *  @return true if the ith parameter is none, i.e. not used.
 */
#define nfIS_ITH_RPC_PARAMETER_NRDP_UNUSED(i, rpcParameterDescription) \
    (((rpcParameterDescription) & (nfRPC_PARAMETER_DESCRIPTION_MASK << ((i) * 8))) == (unsigned int)(nfRPC_PARAMETER_NONE << ((i) * 8)))

/**
 *  @param i must be between 0 and 3.
 *  @param rpcParameterDescription encodes the description of all four parameters.
 *  @return true if the ith parameter is passed by value.
 */
#define nfIS_ITH_RPC_PARAMETER_PASSED_BY_VALUE(i, rpcParameterDescription) \
      ( (!nfIS_ITH_RPC_PARAMETER_NRDP_UNUSED(i, rpcParameterDescription)) && \
        (((rpcParameterDescription) & (nfRPC_PARAMETER_PASSED_BY_VALUE_BIT_MASK << ((i) * 8))) == 0) )

/**
 *  @param i must be between 0 and 3.
 *  @param rpcParameterDescription encodes the description of all four parameters.
 *  @return true if the ith parameter is passed by value and can be read by
 *  the receiving container.
 */
#define nfIS_ITH_RPC_PARAMETER_PASSED_BY_VALUE_AND_CAN_READ(i, rpcParameterDescription) \
    ( (nfIS_ITH_RPC_PARAMETER_PASSED_BY_VALUE(i, rpcParameterDescription)) && \
    (((rpcParameterDescription) & (nfRPC_PARAMETER_VALUE_READ_ONLY << ((i) * 8))) != 0) )

/**
 *  @param i must be between 0 and 3.
 *  @param rpcParameterDescription encodes the description of all four parameters.
 *  @return true if the ith parameter is passed by value and can be written by
 *  the receiving container.
 */
#define nfIS_ITH_RPC_PARAMETER_PASSED_BY_VALUE_AND_CAN_WRITE(i, rpcParameterDescription) \
    ( (nfIS_ITH_RPC_PARAMETER_PASSED_BY_VALUE(i, rpcParameterDescription)) && \
      (((rpcParameterDescription) & (nfRPC_PARAMETER_VALUE_WRITE_ONLY << ((i) * 8))) != 0) )


/**
 *  @param i must be between 0 and 3.
 *  @param rpcParameterDescription encodes the description of all four parameters.
 *  @return true if the ith parameter is passed by reference.
 */
#define nfIS_ITH_RPC_PARAMETER_PASSED_BY_REFERENCE(i, rpcParameterDescription) \
      ( (!nfIS_ITH_RPC_PARAMETER_NRDP_UNUSED(i, rpcParameterDescription)) && \
      (((rpcParameterDescription) & (nfRPC_PARAMETER_PASSED_BY_REFERENCE_BIT_MASK << ((i) * 8))) == 0) )

/**
 *  @param i must be between 0 and 3.
 *  @param rpcParameterDescription encodes the description of all four parameters.
 *  @return true if the ith parameter is passed by reference and can be read by the
 *  receiving container.
 */
#define nfIS_ITH_RPC_PARAMETER_PASSED_BY_REFERENCE_AND_CAN_READ(i, rpcParameterDescription) \
      ( nfIS_ITH_RPC_PARAMETER_PASSED_BY_REFERENCE(i, rpcParameterDescription) && \
        (((rpcParameterDescription) & (nfRPC_PARAMETER_MEMORY_REFERENCE_READ_ONLY << ((i) * 8))) != 0) )


/**
 *  @param i must be between 0 and 3.
 *  @param rpcParameterDescription encodes the description of all four parameters.
 *  @return true if the ith parameter is passed by reference and can be written by
 *  the receiving container.
 */
#define nfIS_ITH_RPC_PARAMETER_PASSED_BY_REFERENCE_AND_CAN_WRITE(i, rpcParameterDescription) \
      ( nfIS_ITH_RPC_PARAMETER_PASSED_BY_REFERENCE(i, rpcParameterDescription) && \
        (((rpcParameterDescription) & (nfRPC_PARAMETER_MEMORY_REFERENCE_WRITE_ONLY << ((i) * 8))) != 0) )

#endif /* RPC_UTILS_H_ */
