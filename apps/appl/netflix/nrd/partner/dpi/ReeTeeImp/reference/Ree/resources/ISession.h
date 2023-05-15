/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

/**
 * @file ISession.h
 * @author fpaun@netflix.com
 * @date April, 2014
 *
 * ISession abstracts a logical connection between a client application and
 * a service application.
 *
 * For example, a session must be established from the REE (potentially
 * untrusted) DRM client application, implementing the DRM business logic,
 * to the TEE (trusted) DRM service application, implementing DRM
 * security-related functionality.
 */
#ifndef I_SESSION_H_
#define I_SESSION_H_

#include <nrddpi/IConnection.h>
#include <nrddpi/RPCErrorTypes.h>
#include <nrddpi/TeeApiErrorTypes.h>

/**
 * nfServiceId_t identifies service applications accepting a session.
 */
typedef enum {
    nfSERVICE_NONE = 0,
    nfSERVICE_NETFLIX_TASK_GROUP   /**< TEE Netflix service identifier. */
} nfServiceId_t;

/**
 * nfSession_t is an opaque handle for a logical session connecting a
 * client application and a service application.
 */
typedef struct nfSession_ * nfSession_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Open a session between a client application and a service application.
 *
 * @param inConnection is an open connection to the container hosting the
 * service application.
 * @param id identifies the name of the service application.
 * @param outSession is a logical session to the specified service application.
 * @param result TBD
 * @return nfTAE_SUCCESS if openning the session is successful,
 *         nfTAE_NULL_PARAMETER if connection, session or result params are null,
           nfTAE_INVALID_SERVICE_ID if service id param is invalid
 */
nfTeeApiError_t nfOpenSession(const nfConnection_t inConnection, const nfServiceId_t id, nfSession_t *outSession, nfRPCResult_t *result);

/**
 * Close a session between a client application and a service application and
 * release system resources allocated for openning a session.
 *
 * @param session is a logical session-based connection between a client
 * application and a service application.
 * @param result TBD
 * @return nfTAE_NO_ERROR if closing the session is successful,
 *         nfTAE_NULL_PARAMETER if session or result params are null
 */
nfTeeApiError_t nfCloseSession(nfSession_t session, nfRPCResult_t *result);

#ifdef __cplusplus
}
#endif

#endif
