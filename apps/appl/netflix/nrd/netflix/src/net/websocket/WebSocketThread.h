/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef __WEBSOCKETTHREAD_H__
#define __WEBSOCKETTHREAD_H__

#include "WebSocket.h"
#include <nrdbase/Thread.h>

namespace netflix
{
class WebSocketThread : public Thread
{
public:

    WebSocketThread(const std::string &name);
    ~WebSocketThread();

    void Run();

    void addConnection(const shared_ptr<IWebSocketHandler> &pHandler);
    void removeConnection(const shared_ptr<IWebSocketHandler> &pHander);

    void stop();
    void wakeup();

private:
    std::set<shared_ptr<IWebSocketHandler> > mConnections;
    std::map<shared_ptr<IWebSocketHandler>, bool> mPending;
    Mutex mMutex;
    bool mRunning;
    int mPipe[2];
};

}


#endif
