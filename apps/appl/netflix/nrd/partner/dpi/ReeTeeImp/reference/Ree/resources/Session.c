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

#include "ISession.h"
#include "Session.h"

nfTeeApiError_t nfOpenSession(const nfConnection_t connection, const nfServiceId_t id, nfSession_t *outSession, nfRPCResult_t *result)
{
    // Verify the in parameters.
    if (NULL == connection || NULL == outSession || NULL == result) {
        return nfTAE_NULL_PARAMETER;
    }

    // Determine the service UUID.
    const uint8_t *uuid = NULL;
    switch(id)
    {
        case nfSERVICE_NONE:
        {
            uuid = nfUUID_NONE;
            break;
        }
        case nfSERVICE_NETFLIX_TASK_GROUP:
        {
            uuid = nfUUID_NETFLIX_TASK_GROUP;
            break;
        }
        default:
        {
            return nfTAE_INVALID_SERVICE_ID;
        }
    }

    // Initialize the session object.
    nfSession_t session = (nfSession_t)malloc(sizeof(struct nfSession_));
    if (NULL == session)
    {
        return nfTAE_OUT_OF_MEMORY;
    }
    memset(session, 0, sizeof(struct nfSession_));
    session->connection = connection;
    memcpy(session->serviceId, uuid, nfUUID_SIZE_IN_BYTES);
    session->id = nfSESSION_ID_NOT_IMPLEMENTED;

    /* Set the out parameters. */
    *outSession = session;
    *result = nfRPC_RESULT_SUCCESS;

    return nfTAE_SUCCESS;
}

nfTeeApiError_t nfCloseSession(nfSession_t session, nfRPCResult_t *result)
{
    // Verify the in parameters.
    if (NULL == result || NULL == session)
    {
        return nfTAE_NULL_PARAMETER;
    }

    // Close the session.
    if (NULL != session)
    {
        memset(session, 0, sizeof(struct nfSession_));
        free(session);
    }

    /* Set the out parameters. */
    *result = nfRPC_RESULT_SUCCESS;

    return nfTAE_SUCCESS;
}

