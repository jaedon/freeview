/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "WebSocketServer.h"
#include <nrdbase/MutexRanks.h>
#include <nrdbase/ScopedMutex.h>

using namespace netflix;

IWebSocketServer::IWebSocketServer()
    : IWebSocketHandler(), mMutex(MDXWEBSOCKETSERVER_MUTEX, "WebsocketServer"), mIsClosed(false)
{
    WSVERBOSE("IWebSockServer()");
}

IWebSocketServer::~IWebSocketServer()
{
    WSVERBOSE("~IWebSocketServer()");
}

void IWebSocketServer::closeConnection()
{
    ScopedMutex lock(mMutex);

    mpConnection->closeConnection();
}

void IWebSocketServer::onClose(int code, const std::string &reason)
{
    (void)code;
    (void)reason;

    ScopedMutex lock(mMutex);

    mIsClosed = true;
    mClosedCondition.broadcast();
}

void IWebSocketServer::waitForClose()
{
    ScopedMutex lock(mMutex);

    if(!mIsClosed) {
        mClosedCondition.wait(&mMutex);
    }
}

void IWebSocketServer::lock()
{
    mMutex.lock();
}

void IWebSocketServer::unlock()
{
    mMutex.unlock();
}
