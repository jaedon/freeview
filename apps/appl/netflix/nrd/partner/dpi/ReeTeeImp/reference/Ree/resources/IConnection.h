/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */


/**
 * @file IConnection.h
 * @author fpaun@netflix.com
 * @date April, 2014
 *
 * IConnection abstracts a logical connection between two containers.
 *
 * For example, a connection must be established from the REE container to the
 * TEE container.
 */
#ifndef I_CONNECTION_H_
#define I_CONNECTION_H_

#include "TeeApiErrorTypes.h"

/**
 * nfContainerId_t identifies containers accepting a connection.
 */
typedef enum
{
    nfCONTAINER_ID_TEE = 1, /**< TEE container identifier. */
} nfContainerId_t;

/**
 * nfConnection_t is an opaque handle for a logical connection between two
 * containers.
 */
typedef struct nfConnection_ * nfConnection_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Open a connection to a container.
 *
 * @param id identifies the name of the container.
 * @param outConnection is a logical connection to the specified container.
 * @return nfTAE_SUCCESS if openning the connection is successful,
 *         nfTAE_INVALID_CONTAINER_ID if container id is invalid,
 *         nfTAE_NULL_PARAMETER if out connection param is null
 */
nfTeeApiError_t nfOpenConnection(const nfContainerId_t id, nfConnection_t *outConnection);

/**
 * Close connection to a container and release system resources allocated
 * for opening a connection.
 *
 * @param connection is a logical connection to a container.
 * @return nfTAE_NO_ERROR if closing the connection is successful,
 *         nfTAE_NULL_PARAMETER if connection param is null.
 */
nfTeeApiError_t nfCloseConnection(nfConnection_t connection);

#ifdef __cplusplus
}
#endif

#endif
