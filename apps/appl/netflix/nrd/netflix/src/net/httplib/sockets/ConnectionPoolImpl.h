/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef CONNECTION_POOL_IMPL
#define CONNECTION_POOL_IMPL

#include "nrdbase/tr1.h"
#include "AsyncHttpSocketConnection.h"

namespace netflix {
namespace ConnectionPool{

// when closeConnection is called at AsyncHttpSocketClient
bool addConnection(shared_ptr<AsyncHttpSocketConnection> connection);

// when openConnection is called at AsyncHttpSocketClient
shared_ptr<AsyncHttpSocketConnection> getConnection(std::string const& hostName,
                                                                  uint16_t portNumber);

}
}

#endif

