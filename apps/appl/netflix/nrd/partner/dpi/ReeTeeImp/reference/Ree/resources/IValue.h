/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

/**
 * @file IValue.h
 * @author fpaun@netflix.com
 * @date April, 2014
 *
 * IValue abstracts a passed-by-value parameter for an RPC between
 * a client application and a service application.
 *
 * IValue facilitates efficient exchange of small data.
 */
#ifndef I_VALUE_H_
#define I_VALUE_H_

#include <stdint.h>

/**
 * Indicate the direction of data flow with respect to the RPC callee.
 */
typedef enum {
  nfVALUE_READ_ONLY = 1,/**< Read only access for RPC callee. */
  nfVALUE_WRITE_ONLY,   /**< Write only access for RPC callee. */
  nfVALUE_READ_WRITE    /**< Read and write access for RPC callee. */
} nfValueType_t;

/**
 * Convenience container to describe where a client application
 * and service application can store/load up to two values.
 */
typedef struct {
    uint32_t a;         /**< First value. */
    uint32_t b;         /**< Second value. */
    nfValueType_t type; /**< Read, write or read/write type of value. */
} nfValue_t;

#endif
