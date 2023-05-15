/*
 * (c) 1997-2015 Netflix, Inc. All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef __TELNET_H__
#define __TELNET_H__

#include <nrdbase/Thread.h>
#include <nrdbase/tr1.h>

namespace netflix {

class TelnetConnection;

class TelnetServer : public Thread
{
public:
    TelnetServer(int port);
    ~TelnetServer();

    void shutdown();

    virtual void Run();

    bool quit();
    bool wakeup();

private:
    bool createListeningSocket();

    int mPort;
    int mListeningSocket;

    int mPipe[2];
    bool mQuit;

    bool mLoggedError;
};

}

#endif // __TELNET_H__
