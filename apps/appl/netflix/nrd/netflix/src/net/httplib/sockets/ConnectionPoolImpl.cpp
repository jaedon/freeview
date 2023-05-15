/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#include <nrdbase/Mutex.h>
#include <nrdbase/ScopedMutex.h>
#include <nrdbase/Log.h>
#include <nrdnet/ConnectionPool.h>
#include "ConnectionPoolImpl.h"

using namespace std;

//#define CONNECTION_POOL_VERBOSE 1

namespace netflix {
namespace ConnectionPool{

class Connection
{
public:
    Connection(shared_ptr<AsyncHttpSocketConnection> connection, bool inUse)
      : mConnection(connection), mInUse(inUse)
    {
        // how can I make a AseTimeVal with current time?
        mTimeEnteredToPool = AseTimeVal::now();
    }
    ~Connection()
    {
    }

public:
    shared_ptr<AsyncHttpSocketConnection> mConnection;
    bool mInUse;
    AseTimeVal mTimeEnteredToPool;
};

static vector< shared_ptr<Connection> > sConnectionPool;
static uint32_t sConnectionPoolSize = 0;
static Mutex sMutex(CONNECTION_POOL_MUTEX, "ConnectionPoolMutex");

void reset(uint32_t maxConnectionPoolSize)
{
    ScopedMutex lock(sMutex);
    sConnectionPool.clear();
    sConnectionPoolSize = maxConnectionPoolSize;
    NTRACE(TRACE_HTTPLIB, "resetting connection pool with pool size %d", sConnectionPoolSize);
}

bool addConnection(shared_ptr<AsyncHttpSocketConnection> connectionToReuse)
{
    ScopedMutex lock(sMutex);

    NTRACE(TRACE_HTTPLIB, "Trying to add connection %p into pool of size %zu",
           connectionToReuse.get(), sConnectionPool.size());

    vector<  shared_ptr<Connection> > ::iterator connectionItr;
    connectionItr = sConnectionPool.begin();
    while (connectionItr != sConnectionPool.end()) {
        if( (*connectionItr)->mConnection == connectionToReuse) {
            NTRACE(TRACE_HTTPLIB, "this connection was already originally from connection pool");
            (*connectionItr)->mInUse = false;
            return true;
        }
        connectionItr++;
    }

    if(sConnectionPool.size() >= sConnectionPoolSize){
        // pool is overpopulated. find oldest connection among not-in-use connections
        AseTimeVal oldestTime = AseTimeVal::now();
        vector<  shared_ptr<Connection> > ::iterator connectionItr, oldestItr;
        connectionItr = oldestItr = sConnectionPool.begin();
        while (connectionItr != sConnectionPool.end()) {
            if( ((*connectionItr)->mTimeEnteredToPool < oldestTime) &&
                ((*connectionItr)->mInUse == false) ) {
                oldestTime = (*connectionItr)->mTimeEnteredToPool;
                oldestItr = connectionItr;
            }
            connectionItr++;
        }

        // if there is no room to get in, return false.
        if( (*oldestItr)->mInUse == true){
            // oldestItr being in-use means that there is no room. return false, so
            // that this passed socket is actually closed
            NTRACE(TRACE_HTTPLIB, "connection pool overpopulated. connection not added");
            return false;
        }

        // before erase it, check if connection state, and close if needed
        if ( ((*oldestItr)->mConnection->getState() < IAsyncHttpConnection::CLOSED_STATE) ||
             ((*oldestItr)->mConnection->getSocketHandle() == -1) ) {
            (*oldestItr)->mConnection->close();
        }
        sConnectionPool.erase(oldestItr);
        NTRACE(TRACE_HTTPLIB, "oldest connection evicted: pool size now %zu", sConnectionPool.size());
    }

    shared_ptr<Connection> connection(new Connection(connectionToReuse, false));
    sConnectionPool.push_back(connection);

    NTRACE(TRACE_HTTPLIB, "Into pool, host %s port %d",
           connectionToReuse->getServerName().c_str(),
           connectionToReuse->getClientPortNumber());
    NTRACE(TRACE_HTTPLIB, "connection added : connection pool size %zu", sConnectionPool.size());

    return true;
}

shared_ptr<AsyncHttpSocketConnection> getConnection(std::string const& hostName,
                                                    uint16_t portNumber)
{
    // I need scoped Mutex here
    ScopedMutex lock(sMutex);
    vector<  shared_ptr<Connection> > ::iterator connectionItr;
    connectionItr = sConnectionPool.begin();
    NTRACE(TRACE_HTTPLIB, "%s : size of connection pool %zu: request hostName %s, port %d (%d)",
           __func__, sConnectionPool.size(), hostName.c_str(), portNumber, htons(portNumber));

    // first, delete inactive connections
    while (connectionItr != sConnectionPool.end())
    {
        if ( ((*connectionItr)->mConnection->getState() >= IAsyncHttpConnection::CLOSED_STATE)
             || ((*connectionItr)->mConnection->getSocketHandle() == -1)  ) {
            (*connectionItr)->mConnection->close();
            connectionItr = sConnectionPool.erase(connectionItr);
        } else {
            connectionItr++;
        }
    }

    // now, get idle connection
    connectionItr = sConnectionPool.begin();
    while (connectionItr != sConnectionPool.end())
    {
        NTRACE(TRACE_HTTPLIB, "In pool, server:%s:%d, client port:%d, socketfd:%d, state:%d, isUsing:%s "
               , (*connectionItr)->mConnection->getServerName().c_str()
               , (*connectionItr)->mConnection->getDestinationPortNumber()
               , (*connectionItr)->mConnection->getClientPortNumber()
               , (*connectionItr)->mConnection->getSocketHandle()
               , (*connectionItr)->mConnection->getState()
               , (*connectionItr)->mInUse? "in use": "not in use" );

        if ((*connectionItr)->mConnection->getServerName() == hostName && // same server name
            (*connectionItr)->mConnection->getDestinationPortNumber() == htons( portNumber) && // same port number
            (*connectionItr)->mConnection->getSocketHandle() != -1 &&
            (*connectionItr)->mInUse != true && // not in use
            (*connectionItr)->mConnection->getState() == IAsyncHttpConnection::IDLE_STATE // idle state only
            // ttl not expired
            )
        {
            NTRACE(TRACE_HTTPLIB, " <<< returning connection from connection pool %p >>>", (*connectionItr)->mConnection.get());
            (*connectionItr)->mInUse = true;
            return (*connectionItr)->mConnection;
        }
        connectionItr++;
    }

    // if not found, return null
    NTRACE(TRACE_HTTPLIB, "no connection to reuse from connection pool");
    return shared_ptr<AsyncHttpSocketConnection>();
}

}
}
