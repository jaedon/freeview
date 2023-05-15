/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include <stdlib.h>
#include <string.h>

#include "Connection.h"

nfTeeApiError_t nfOpenConnection(const nfContainerId_t id, nfConnection_t *outConnection)
{
    // Verify the in parameters.
    if (nfCONTAINER_ID_TEE != id)
    {
        return nfTAE_INVALID_CONTAINER_ID;
    }
    if (NULL == outConnection)
    {
        return nfTAE_NULL_PARAMETER;
    }

    // Initialize the connection object.
    nfConnection_t connection = (nfConnection_t) malloc(sizeof(struct nfConnection_));
    if (NULL == connection)
    {
        return nfTAE_OUT_OF_MEMORY;
    }
    memset(connection, 0, sizeof(struct nfConnection_));
    connection->id = id;

    // Set the out parameter.
    *outConnection = connection;
    return nfTAE_SUCCESS;
}

nfTeeApiError_t nfCloseConnection(nfConnection_t connection)
{
    if (NULL == connection)
    {
        return nfTAE_NULL_PARAMETER;
    }

    free(connection);
    return nfTAE_SUCCESS;
}

