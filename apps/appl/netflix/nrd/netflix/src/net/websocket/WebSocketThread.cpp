/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "WebSocketThread.h"
#include <nrdbase/Log.h>
#include <nrdbase/PerformanceMarker.h>
#include <nrdbase/ScopedMutex.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

using namespace netflix;

#define EINTRWRAP(VAR, BLOCK)                   \
    do {                                        \
        VAR = BLOCK;                            \
    } while (VAR == -1 && errno == EINTR)

WebSocketThread::WebSocketThread(const std::string &name)
    : Thread(&THREAD_WEBSOCKET, name), mMutex(WEBSOCKETTHREAD_MUTEX, "WebSocketThread")
{
    mPipe[0] = mPipe[1] = -1;
    int ret = ::pipe(mPipe);
    assert(ret != -1);
    if (ret == -1) {
        return;
    }
    EINTRWRAP(ret, ::fcntl(mPipe[0], F_GETFL, 0));
    assert(ret != -1);
    if (ret == -1)
        return;

    const int flags = ret | O_NONBLOCK;

    EINTRWRAP(ret, ::fcntl(mPipe[0], F_SETFL, flags));
    assert(ret != -1);
}

WebSocketThread::~WebSocketThread()
{
    if (mPipe[0] >= 0)
        close(mPipe[0]);
    if (mPipe[1] >= 0)
        close(mPipe[1]);
}

void WebSocketThread::Run()
{
    {
        ScopedMutex lock(mMutex);
        mRunning = true;
    }

    NTRACE(TRACE_WEBSOCKET, "Thread %s running...", CurrentThreadName().c_str());

    while (true) {
        PERFORMANCE_MARKER_SCOPED("websocket.loop");

        {
            ScopedMutex lock(mMutex);
            for (std::map<shared_ptr<IWebSocketHandler>, bool>::const_iterator it = mPending.begin(); it != mPending.end(); ++it) {
                if (it->second) {
                    mConnections.insert(it->first);
                } else {
                    mConnections.erase(it->first);
                }
            }
            mPending.clear();
        }


        fd_set readSet;
        fd_set writeSet;
        int ret;
        FD_ZERO(&readSet);
        FD_ZERO(&writeSet);
        FD_SET(mPipe[0], &readSet);
        int maxFd = mPipe[0];

        int count = 0;
        for (std::set<shared_ptr<IWebSocketHandler> >::const_iterator it = mConnections.begin(); it != mConnections.end(); ++it) {
            const shared_ptr<IWebSocketHandler> &pConnection = *it;
            if (!pConnection->mpConnection->isClosed()) {
                ++count;
                const int socket = pConnection->mpConnection->getSocket();
                FD_SET(socket, &readSet);
                if (pConnection->pendingSend()) {
                    FD_SET(socket, &writeSet);
                }

                maxFd = std::max(socket, maxFd);
            }
        }
        do {
            ret = select(maxFd + 1, &readSet, &writeSet, 0, 0);
        } while (ret == -1 && errno == EINTR);

        if (ret == -1) {
            sleep(1); // don't busy loop if we get an error
            continue;
        }

        for (std::set<shared_ptr<IWebSocketHandler> >::const_iterator it = mConnections.begin(); it != mConnections.end(); ++it) {
            const shared_ptr<IWebSocketHandler> &pConnection = *it;
            const int socket = pConnection->mpConnection->getSocket();

            if (FD_ISSET(socket, &readSet)) {
                pConnection->mpConnection->process();
            }
            if (FD_ISSET(socket, &writeSet)) {
                pConnection->onSend();
            }
        }

        if (FD_ISSET(mPipe[0], &readSet)) {
            bool quit = false;
            do {
                char ch = '\0';
                EINTRWRAP(ret, ::read(mPipe[0], &ch, sizeof(ch)));
                if (ret == 0) {
                    assert(false);
                    break;
                } else if (ret == -1) {
                    assert(errno == EAGAIN || errno == EWOULDBLOCK); // pipe is empty
                    break;
                } else if (ch == 'q') {
                    quit = true;
                }
            } while (ret == 1);
            if (quit) {
                break;
            }
        }
    }
#ifndef NDEBUG
    {
        ScopedMutex lock(mMutex);
        assert(!mRunning);
    }
#endif

    if (!mConnections.empty()) {
        NTRACE(TRACE_WEBSOCKET, "Thread %s forcing close for %zu connections",
               CurrentThreadName().c_str(), mConnections.size());

        for (std::set<shared_ptr<IWebSocketHandler> >::const_iterator it = mConnections.begin(); it != mConnections.end(); ++it) {
            (*it)->mpConnection->closeConnection();
        }

        mConnections.clear();
    }

    NTRACE(TRACE_WEBSOCKET, "Thread %s ending", CurrentThreadName().c_str());
    {
        ScopedMutex lock(mMutex);
        mPending.clear();
    }
}

void WebSocketThread::addConnection(const shared_ptr<IWebSocketHandler> &pConnection)
{
    assert(pConnection);
    ScopedMutex lock(mMutex);
    if (mRunning) {
        NTRACE(TRACE_WEBSOCKET, "Adding connection %s", pConnection->mpConnection->getClient().c_str());
        mPending[pConnection] = true;

        int ret;
        EINTRWRAP(ret, ::write(mPipe[1], "a", 1));
        if (ret == -1)
            Log::error(TRACE_WEBSOCKET, "Unable to write 'a' to pipe in WebSocketThread (%d)", errno);
    }
}

void WebSocketThread::removeConnection(const shared_ptr<IWebSocketHandler> &pConnection)
{
    assert(pConnection);
    ScopedMutex lock(mMutex);
    if (mRunning) {
        NTRACE(TRACE_WEBSOCKET, "Removing connection %s", pConnection->mpConnection->getClient().c_str());
        mPending[pConnection] = false;

        int ret;
        EINTRWRAP(ret, ::write(mPipe[1], "r", 1));
        if (ret == -1)
            Log::error(TRACE_WEBSOCKET, "Unable to write 'r' to pipe in WebSocketThread (%d)", errno);
    }
}

void WebSocketThread::stop()
{
    ScopedMutex lock(mMutex);
    if (mRunning) {
        mRunning = false;
        int ret;
        EINTRWRAP(ret, ::write(mPipe[1], "q", 1));
        if (ret == -1)
            Log::error(TRACE_WEBSOCKET, "Unable to write 'q' to pipe in WebSocketThread (%d)", errno);
    }
}

void WebSocketThread::wakeup()
{
    ScopedMutex lock(mMutex);
    if (mRunning) {
        int ret;
        EINTRWRAP(ret, ::write(mPipe[1], "w", 1));
        if (ret == -1)
            Log::error(TRACE_WEBSOCKET, "Unable to write 'w' to pipe in WebSocketThread (%d)", errno);
    }
}
