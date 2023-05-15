/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

/**
 * @file Session.h
 * @author fpaun@netflix.com
 * @date April, 2014
 *
 * Session implements the ISession interface.
 */
#ifndef SESSION_H_
#define SESSION_H_

#include "IConnection.h"
#include "ISession.h"
#include "RPCTypes.h"

static const uint8_t nfUUID_NONE[nfUUID_SIZE_IN_BYTES] = { 0x0 };

/** Id of the Netflix Task Group. */
static const uint8_t nfUUID_NETFLIX_TASK_GROUP[nfUUID_SIZE_IN_BYTES] = { 0x0 };

/**
 *  The current implementation does not make use of sessions. Sessions are a
 * placeholder and all sessions must be identified by the not implemented id.
 */
static const uint32_t nfSESSION_ID_NOT_IMPLEMENTED = 0x0;

struct nfSession_ {
    nfConnection_t connection;                  /**< Connection handle, a session belongs to a connection. */
    uint8_t serviceId[nfUUID_SIZE_IN_BYTES];    /**< Id of the application at other end of the session. */
    uint32_t id;                                /**< Set to nfSESSION_ID_NOT_IMPLEMENTED */
};

#endif
