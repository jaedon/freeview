/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef __WEBSOCKETSERVER_H__
#define __WEBSOCKETSERVER_H__

#include "WebSocket.h"
#include <nrdbase/ConditionVariable.h>
#include <nrdbase/Mutex.h>

namespace netflix
{
class IWebSocketServer : public IWebSocketHandler
{
public:

    IWebSocketServer();
    virtual ~IWebSocketServer();

    void closeConnection();

    virtual void onClose(int code, const std::string &reason);

    virtual void waitForClose();

protected:

    // This and derived class to
    // utilize for synchronization
    void lock();
    void unlock();

private:

    Mutex             mMutex;
    bool                    mIsClosed;
    ConditionVariable mClosedCondition;
};

}

#endif // __WEBSOCKETSERVER_H__
