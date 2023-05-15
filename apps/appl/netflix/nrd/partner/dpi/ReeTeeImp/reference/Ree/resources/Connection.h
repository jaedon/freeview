/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

/**
 * @file Connection.h
 * @author fpaun@netflix.com
 * @date April, 2014
 *
 * Connection implements the IConnection interface.
 */
#ifndef CONNECTION_H_
#define CONNECTION_H_

#include "IConnection.h"

struct nfConnection_ {
    nfContainerId_t id; /**< Id of container at other end of the connection. */
};

#endif /* CONNECTION_H_ */
