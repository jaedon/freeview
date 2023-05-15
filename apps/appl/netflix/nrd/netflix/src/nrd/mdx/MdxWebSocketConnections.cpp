/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "MdxWebSocketConnections.h"
#include "WebSocketMdx.h"
#include "MdxInternal.h"
#include <nrdbase/Mutex.h>
#include <nrdbase/ScopedMutex.h>

using namespace netflix;
using namespace netflix::mdx;

typedef std::map<std::string, shared_ptr<MdxServer> > MdxConnectionMap;

static MdxConnectionMap sConnectionMap;
static Mutex            sConnectionMutex(MDXWEBSOCKETCONNECTIONS_MUTEX, "MdxWebsocketConnections");

bool MdxWebSocketConnections::addConnection(const std::string &host, shared_ptr<MdxServer> pServer)
{
    ScopedMutex lock(sConnectionMutex);

    MdxConnectionMap::iterator i = sConnectionMap.find(host);

    // only one connection per host is allowed for MDX
    if(i == sConnectionMap.end())
    {
        sConnectionMap[host] = pServer;

        MdxInternal::getInstance()->callListener(WebSocketConnectEvent(host));

        return true;
    }

    return false;
}

shared_ptr<MdxServer> MdxWebSocketConnections::getConnection(const std::string &host)
{
    ScopedMutex lock(sConnectionMutex);

    MdxConnectionMap::iterator i = sConnectionMap.find(host);

    if(i != sConnectionMap.end())
    {
        return i->second;
    }
    else
    {
        return shared_ptr<MdxServer>();
    }
}

bool MdxWebSocketConnections::removeConnection(const std::string &host, shared_ptr<MdxServer> pServer)
{
    ScopedMutex lock(sConnectionMutex);

    MdxConnectionMap::iterator i = sConnectionMap.find(host);

    if(i != sConnectionMap.end())
    {
        if(i->second == pServer)
        {
            MdxInternal::getInstance()->callListener(WebSocketDisconnectEvent(host));
            sConnectionMap.erase(i);
            return true;
        }
    }

    return false;
}

void MdxWebSocketConnections::clearConnections()
{
    ScopedMutex lock(sConnectionMutex);

    MdxConnectionMap::iterator i = sConnectionMap.begin();

    while(i != sConnectionMap.end())
    {
        MdxInternal::getInstance()->callListener(WebSocketDisconnectEvent(i->first));

        i->second->mpConnection->closeConnection();
        ++i;
    }

    //    sConnectionMap.clear();
}
