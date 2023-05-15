/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

/**
 * @file TeeApiErrorTypes.h
 * @author fpaun@netflix.com
 * @date April, 2014
 *
 * TeeApiErrorTypes defines logical errors associated with setting up
 * and sending an RPC between the REE and the TEE.
 */
#ifndef TEE_API_ERROR_H_
#define TEE_API_ERROR_H_

#include <nrddpi/TeeLog.h>

typedef enum {
    nfTAE_SUCCESS = 0,

    nfTAE_INVALID_CONTAINER_ID = 1000,
    nfTAE_INVALID_SERVICE_ID,

    nfTAE_NULL_PARAMETER,
    nfTAE_INVALID_PARAMETER,
    nfTAE_OUT_OF_MEMORY,

    nfTAE_INVALID_NUMBER_OF_RPC_PARAMETERS,
    nfTAE_INVALID_RPC_PARAMETER_TYPE,
    nfTAE_INVALID_RPC_PARAMETER_MEMORY_REFERENCE_TYPE,
    nfTAE_INVALID_RPC_PARAMETER_VALUE_TYPE,

} nfTeeApiError_t;

#define CHECK_FOR_ERROR(err,str) do{ if ((err) != nfTAE_SUCCESS) { teeLog(Tee_Error, "%s", str); assert(0);}} while(0)

#endif /* TEE_API_ERROR_H_ */
