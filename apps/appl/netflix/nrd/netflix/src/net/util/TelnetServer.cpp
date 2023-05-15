/*
 * (c) 1997-2015 Netflix, Inc. All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "TelnetServer.h"

#include <nrdbase/Console.h>
#include <nrdbase/Configuration.h>
#include <nrdbase/Log.h>
#include <nrdbase/LogSink.h>
#include <nrdbase/PerformanceMarker.h>
#include <nrdbase/ScopedMutex.h>
#include <nrdbase/StringFormatter.h>
#include <nrdbase/Time.h>
#include <nrdbase/ConsoleSink.h>
#include <nrdbase/TraceAreas.h>
#include <nrdbase/Grep.h>
#include <nrdbase/Application.h>

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <string>

DEFINE_TRACEAREA(TELNET, 0);

using namespace netflix;

namespace netflix {

class TelnetConnection : public Thread, public LogSink
{
public:
    TelnetConnection(int socket, TelnetServer *server);
    ~TelnetConnection();

    virtual void Run();
    bool finished() { return !mRunning; }

    void receive(const Log::Message &msg);

    int send(const std::string &message) {
        ScopedMutex _lock(mMutex);

        fd_set write_set;
        FD_ZERO(&write_set);
        FD_SET(mSocket, &write_set);

        size_t amountSent = 0;
        do
        {
            int result = select(mSocket + 1, 0, &write_set, 0, 0);
            if (result > 0)
            {
                amountSent += ::send(mSocket, &message.c_str()[amountSent], message.length()-amountSent, 0);
            }
            else
            {
                Log::error(TRACE_TELNET, "select for writing to telnet returned %d", result);
                return result;
            }
        }
        while (mSendFullMessage && amountSent<message.length());

        return amountSent;
    }

    bool quit();
    void setRawMode(bool raw) {
        mRaw = raw;
        if(raw)
            send("\377\375\042\377\373\001");
        else
            send("\377\373\042\377\375\001\377\374\003");
    }

private:
    bool processInput();
    TelnetServer *mServer;
    std::string mInput;

    Mutex mMutex;
    int mSocket;

    int mPipe[2];
    bool mQuitting;

    Time mStartTime;
    shared_ptr<TraceAreas> mTraceAreas;
    Log::LogLevel mLevel;
    bool mRunning;
    bool mRaw;

    bool mSendFullMessage;
};

}

TelnetServer::TelnetServer(int port)
  : Thread(&THREAD_TELNET_SERVER)
  , mPort(port)
  , mListeningSocket(-1)
  , mQuit(false)
  , mLoggedError(false)
{
    const int ret = pipe(mPipe);
    (void)ret;
    assert(!ret);
}

TelnetServer::~TelnetServer()
{
    quit();
    Wait();
    close(mPipe[0]);
    close(mPipe[1]);
    if (mListeningSocket >= 0)
        close(mListeningSocket);
    mListeningSocket = -1;
}

void TelnetServer::shutdown()
{
    quit();
}

void TelnetServer::Run()
{
    std::set<shared_ptr<TelnetConnection> > connections;

    unsigned int listenFailCount = 0;
    unsigned int acceptFailCount = 0;
    while (true) {
        PERFORMANCE_MARKER_SCOPED("telnetserver.loop");

        if (mQuit)
            break;
        if (mListeningSocket < 0) {
            if (!createListeningSocket()) {
                listenFailCount++;
                if (++listenFailCount > 10) {
                    Log::error(TRACE_TELNET, "Too many failures attempting to listen, shutting down");
                    break;
                }
                Sleep(3000);
                continue;
            }
        }

        fd_set read_set;
        int result;
        do {
            FD_ZERO(&read_set);
            FD_SET(mPipe[0], &read_set);
            FD_SET(mListeningSocket, &read_set);
            result = select(std::max(mPipe[0], mListeningSocket) + 1, &read_set, 0, 0, 0);
        } while (result == -1 && errno == EINTR);

        if (mQuit)
            break;

        if (result < 0) {
            Log::error(TRACE_TELNET, "Failed to select");
            break;
        }
        for(std::set<shared_ptr<TelnetConnection> >::iterator it = connections.begin(); it != connections.end(); ) {
            if((*it)->finished()) {
                LogSink::remove((*it));
                (*it)->Wait();
                connections.erase(it++);
            } else {
                ++it;
            }
        }
        if (FD_ISSET(mPipe[0], &read_set)) {
            char buff;
            const int r = read(mPipe[0], &buff, 1);
            if (r == -1) {
                Log::error(TRACE_TELNET, "Failed to read from wakeup pipe: %d", errno);
                break;
            } else if (r == 0) {
                Log::error(TRACE_TELNET, "Failed to read from wakeup pipe, connection closed");
                break;
            } else if (buff == 'q') {
                assert(r == 1);
                break;
            }
        }
        if (FD_ISSET(mListeningSocket, &read_set)) {
            const int acceptSocket = accept(mListeningSocket, NULL, NULL);
            if (acceptSocket < 0) {
                if (++acceptFailCount > 1) {
                    // this will force us to reopen the listening socket
                    close(mListeningSocket);
                    mListeningSocket = -1;
                    acceptFailCount = 0;
                } else if (acceptFailCount == 1) {
                    // only log the first time
                    Log::error(TRACE_TELNET, "Failed to accept socket");
                }
                continue;
            }
            int buflen;
            socklen_t optLen = sizeof(buflen);
            int retVal = getsockopt(
                acceptSocket,
                SOL_SOCKET,
                SO_SNDBUF,
                &buflen,
                &optLen );
            if (retVal == 0) {
                NTRACE(TRACE_TELNET, "doubling size of sndbuf from %d to %d", buflen, buflen << 1);
                buflen <<= 1;
                setsockopt(
                    acceptSocket,
                    SOL_SOCKET,
                    SO_SNDBUF,
                    &buflen,
                    optLen );
            }
            acceptFailCount = 0;
            shared_ptr<TelnetConnection> newConn(new TelnetConnection(acceptSocket, this));
            connections.insert(newConn);
            newConn->Start();
            LogSink::add(newConn);
            newConn->welcome();
        }
    }

    NTRACE(TRACE_TELNET, "TelnetServer going away, clearing %zu connections", connections.size());
    for(std::set<shared_ptr<TelnetConnection> >::iterator it = connections.begin(); it != connections.end(); ++it) {
        LogSink::remove(*it);
        (*it)->quit();
    }
}

bool TelnetServer::createListeningSocket()
{
    NTRACE(TRACE_TELNET, "creating listening socket for port %d", mPort);
    int listeningSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (listeningSocket < 0) {
        Log::error(TRACE_TELNET, "could not create socket");
        return false;
    }

    int flags = fcntl(listeningSocket, F_GETFL, 0);
    if (flags != -1)
        fcntl(listeningSocket, F_SETFL, flags | O_NONBLOCK);

    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(mPort);

#if 1
    int reuseaddr = 1;
    setsockopt(listeningSocket, SOL_SOCKET, SO_REUSEADDR, &reuseaddr, sizeof(reuseaddr));
#endif

    int result = bind(listeningSocket, (struct sockaddr*)&server_address,
            sizeof(server_address));
    if (result < 0) {
        if (!mLoggedError) {
            Log::error(TRACE_TELNET, "could not bind to port");
            mLoggedError = true;
        }
        close(listeningSocket);
        return false;
    }

    result = listen(listeningSocket, 1);
    if (result < 0) {
        Log::error(TRACE_TELNET, "could not listen on port");
        close(listeningSocket);
        return false;
    }

    NTRACE(TRACE_TELNET, "successfully listening for connections");
    mListeningSocket = listeningSocket;
    return true;
}

bool TelnetServer::wakeup()
{
    if (mQuit)
        return true;
    return (write(mPipe[1], " ", 1) == 1);
}

bool TelnetServer::quit()
{
    if (mQuit)
        return true;
    mQuit = true;
    return (write(mPipe[1], "q", 1) == 1);
}

TelnetConnection::TelnetConnection(int socket, TelnetServer *server)
  : Thread(&THREAD_TELNET_CONNECTION)
  , mServer(server)
  , mMutex(UNTRACKED_MUTEX, "TelnetMutex")
  , mSocket(socket)
  , mQuitting(false)
  , mStartTime(Time::mono())
  , mLevel(Log::Warn)
  , mRunning(true)
  , mRaw(false)
  , mSendFullMessage(false)
{
    mTraceAreas.reset(new TraceAreas);
    const int ret = pipe(mPipe);
    (void)ret;
    assert(!ret);
}

TelnetConnection::~TelnetConnection()
{
    quit();
    Wait();
    close(mPipe[0]);
    close(mPipe[1]);
    if (mSocket >= 0)
        close(mSocket);
    mSocket = -1;
}

bool TelnetConnection::quit()
{
    if (mQuitting) return true;
    mQuitting = (write(mPipe[1], "q", 1) == 1);
    return mQuitting;
}

void TelnetConnection::Run()
{
    NTRACE(TRACE_TELNET, "accepted telnet connection");

    while (true) {
        PERFORMANCE_MARKER_SCOPED("telnetconnection.loop");

        if (mSocket < 0 || mQuitting)
            break;

        fd_set read_set;

        int result;
        do {
            FD_ZERO(&read_set);

            FD_SET(mPipe[0], &read_set);
            FD_SET(mSocket, &read_set);
            result = select(std::max(mPipe[0], mSocket) + 1, &read_set, 0, 0, 0);
        } while (result == -1 && errno == EINTR);
        if (result < 0)
            break;

        if (mQuitting || FD_ISSET(mPipe[0], &read_set))
            break;

        if (mSocket < 0)
            break;

        if (FD_ISSET(mSocket, &read_set)) {
            const int bufSize = 1024;
            char buf[bufSize];
            ssize_t readBytes = recv(mSocket, buf, bufSize - 1, 0);
            if (readBytes <= 0)
                break;

            buf[readBytes] = '\0';
            mInput += buf;

            if (!processInput())
                break;
        }
    }
    mRunning = false;
    mServer->wakeup();
    NTRACE(TRACE_TELNET, "telnet connection closed");
}

/* always called while holding socket mutex */
bool TelnetConnection::processInput()
{
    while (!mInput.empty()) {
        if (mQuitting || mSocket < 0)
            return false;

        if(mRaw) {
            std::string sequence;
            size_t slash = mInput.find_first_of("/");
            if (slash != std::string::npos) {
                setRawMode(false);
                sequence = mInput.substr(0, slash);
                mInput = mInput.erase(0, slash+1);
            } else {
                sequence = mInput;
                mInput.clear();
            }
            if (shared_ptr<Console> console = app()->console())
                console->handleSequence(sequence);
            if(mInput.empty())
                return true;
        }

        size_t first = mInput.find_first_of("\r\n");
        if (first == std::string::npos)
            return true;

        std::string cmd = mInput.substr(0, first);
        mInput.erase(0, first + 1);

        if (cmd.empty())
            continue;

        if (cmd.compare(0, 12, "ClientHello ") == 0) {
            std::string resp = "ServerHello\n";
            if (send(resp) < 0)
                return false;
            cmd.erase(0, 12);
            if(!cmd.compare(0, 6, "trace ")) {
                cmd.erase(0, 6);
                mLevel = Log::Trace;
            } else if(!cmd.compare(0, 6, "debug ")) {
                cmd.erase(0, 6);
                mLevel = Log::Debug;
            } else if(!cmd.compare(0, 5, "info ")) {
                cmd.erase(0, 5);
                mLevel = Log::Info;
            } else if(!cmd.compare(0, 5, "warn ")) {
                cmd.erase(0, 5);
                mLevel = Log::Warn;
            } else if(!cmd.compare(0, 6, "error ")) {
                cmd.erase(0, 6);
                mLevel = Log::Error;
            } else if(!cmd.compare(0, 6, "fatal ")) {
                cmd.erase(0, 6);
                mLevel = Log::Fatal;
            }
            {
                std::string msg = "LogLevel: " + std::string(Log::logLevelToString(mLevel)) + "\n";
                if (send(msg) < 0)
                    return false;
            }
            trim(cmd);
            if (!cmd.empty())
                mTraceAreas->parse(cmd);
            if (send("TraceAreas: " + mTraceAreas->spec() + '\n') < 0)
                return false;

            continue;
        }

        if(!cmd.compare(0, 6, "/level")) {
            if(!cmd.compare(6, 6, " trace"))
                mLevel = Log::Trace;
            else if(!cmd.compare(6, 6, " debug"))
                mLevel = Log::Debug;
            else if(!cmd.compare(6, 5, " info"))
                mLevel = Log::Info;
            else if(!cmd.compare(6, 5, " warn"))
                mLevel = Log::Warn;
            else if(!cmd.compare(6, 6, " error"))
                mLevel = Log::Error;
            else if(!cmd.compare(6, 6, " fatal"))
                mLevel = Log::Fatal;
            const std::string msg = "LogLevel: " + std::string(Log::logLevelToString(mLevel)) + "\n";
            if (send(msg) < 0)
                return false;
        } else if(!cmd.compare(0, 4, "/raw")) {
            setRawMode(true);
            if(shared_ptr<Console> console = app()->console()) //let the console go into raw mode too?
                console->handleCommand(cmd, Console::Console_None);
        } else if(!cmd.compare(0, 6, "/trace")) {
            std::string msg;
            if(cmd[6] == ' ') {
                mTraceAreas->parse(cmd.substr(7));
                msg = mTraceAreas->spec();
            }
            msg += "\n";
            if (send("TraceAreas: " + msg) < 0)
                return false;
        } else if(!cmd.compare(0, 15, "/togglefullmsgs")) {
            mSendFullMessage = !mSendFullMessage;
        } else if(shared_ptr<Console> console = app()->console()) {
            console->handleCommand(cmd, Console::Console_None);
        }
    }
    return true;
}

void TelnetConnection::receive(const Log::Message &msg)
{
    if (mQuitting || mSocket < 0 || finished())
        return;

    PERFORMANCE_MARKER_SCOPED("log.telnetconnection.receive");
    // print if at least one of:
    // 1. the logLevel is greater than or equal to mLevel
    // 2. mTraceAreas is empty and the traceArea is enabled
    // 3. mTraceAreas is set and the traceArea is in mTraceAreas
    if ((msg.m_logLevel >= mLevel)
        || (mTraceAreas->isEmpty() && app()->traceAreas()->isEnabled(msg.m_traceArea))
        || mTraceAreas->isEnabled(msg.m_traceArea)) {
        const Configuration::ColorScheme colorScheme = Configuration::telnetColors();
        std::string osstr = ConsoleSink::format(msg, colorScheme);
        if (!osstr.empty()) {
            if(msg.m_flags & Log::Message::SendToFilters) {
                if (Application *a = app()) {
                    osstr = Console::filter(a->filters(), osstr, colorScheme);
                    if (osstr.empty())
                        return;
                }
            }

            if (send(osstr) < 0) {
                quit();
            }
        }
    }
}
