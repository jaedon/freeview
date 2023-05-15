/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "RemoteScreen.h"

#include <Screen.h>
#include <GibbonConfiguration.h>

#include <nrdbase/Time.h>
#include <nrdbase/Endian.h>
#include <nrdbase/Version.h>
#include <nrdbase/Console.h>
#include <nrdbase/Backtrace.h>
#include <nrdbase/ScopedMutex.h>
#include <nrdbase/StringFormatter.h>
#include <nrdbase/Application.h>
#include <nrdbase/AtomicCount.h>

#include <nrd/EventConnection.h>

#include <nrd/AppThread.h>

#include <netdb.h>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <netinet/tcp.h>

#include <cstring>
#include <string>

using namespace netflix;
using namespace netflix::gibbon;

NRDP_DEFINE_OBJECTCOUNT(GibbonRemoteScreenConnection);

#define GIBBON_REMOTESCREEN_PORT 6900
#if 1
# define GIBBON_REMOTESCREEN_MAX_SERVER_FRAMES 50
#endif
#if defined(GIBBON_GRAPHICSENGINE_NULL)
# define GIBBON_REMOTESCREEN_SERVER_NO_GRAPHICS
//# define GIBBON_REMOTESCREEN_CLIENT_NO_GRAPHICS
#endif
#if 0
# define GIBBON_REMOTESCREEN_NO_NBP
#endif

static AtomicCount sSurfaceSerial;

DEFINE_TRACEAREA(GIBBON_REMOTESCREEN, 0);
#if defined(BUILD_DEBUG)
# define DEBUG_REMOTESCREEN_FRAME(fmt, ...)      \
    do {                                        \
        if(Debug::DebugRemoteFrames) {          \
            Log::warn(TRACE_GIBBON_REMOTESCREEN, "REMOTESCREEN_FRAME:" fmt, __VA_ARGS__); \
        } else {                                \
            NTRACE(TRACE_GIBBON_REMOTESCREEN, "REMOTESCREEN_FRAME:" fmt, __VA_ARGS__); \
        }                                       \
    } while (0)
# define DEBUG_REMOTESCREEN_SURFACE(fmt, ...)    \
          do {                                  \
              if(Debug::DebugRemoteSurface) {    \
                  Log::warn(TRACE_GIBBON_REMOTESCREEN, "REMOTESCREEN_SURFACE:" fmt, __VA_ARGS__); \
              } else {                          \
                  NTRACE(TRACE_GIBBON_REMOTESCREEN, "REMOTESCREEN_SURFACE:" fmt, __VA_ARGS__); \
              }                                 \
          } while (0)
#else
# define DEBUG_REMOTESCREEN_FRAME(...)
# define DEBUG_REMOTESCREEN_SURFACE(...)
#endif

#if !defined(GIBBON_REMOTESCREEN_NO_BSDIFF)
# include <bsdiff.h>
static int gibbon_bsdiff_write(struct bsdiff_stream *stream, const void *buffer, int size)
{
    DataBuffer *db = (DataBuffer*)stream->opaque;
    db->append((const unsigned char *)buffer, size);
    return 0;
}
int gibbon_bsdiff_read(const struct bspatch_stream *stream, void *buffer, int length)
{
    DataBuffer *db = (DataBuffer*)stream->opaque;
    const int use = std::min(length, db->length());
    memcpy(buffer, db->constData(), use);
    *db = db->slice(use);
    return 0;
}
#endif

// GibbonServerOp_SurfaceRender type
enum SurfaceRenderType {
    SurfaceRenderType_NONE   = 0,
    SurfaceRenderType_GZIP   = 1,
    SurfaceRenderType_BSDIFF = 2
};

// GibbonServerOp_SurfaceDecode type
enum SurfaceDecodeType {
    SurfaceDecodeType_URL     = 0,
    SurfaceDecodeType_Data    = 1,
    SurfaceDecodeType_Pixels  = 2
};

enum {
    //General
    GibbonOp_Ping                 = 1,
    GibbonOp_Pong                 = 2,

    //From Server
    GibbonServerOp_Init              = 10,
    GibbonServerOp_SurfaceDownload   = 11,
    GibbonServerOp_SurfaceDecode     = 12,
    GibbonServerOp_SurfaceRelease    = 13,
    GibbonServerOp_SurfaceRender     = 14,
    GibbonServerOp_SurfaceStarted    = 15,
    GibbonServerOp_ScreenRender      = 16,
    GibbonServerOp_ScreenShowSplash  = 17,
    GibbonServerOp_NBPSend           = 18,
    GibbonServerOp_AnimationStart    = 19,
    GibbonServerOp_AnimationEnd      = 20,

    //From Client
    GibbonClientOp_Init              = 128,
    GibbonClientOp_NBPInit           = 129,
    GibbonClientOp_Quit              = 130,
    GibbonClientOp_Eval              = 131,
    GibbonClientOp_Event             = 132,
    GibbonClientOp_ScreenRendered    = 133,
    GibbonClientOp_SurfaceDownloaded = 135,
    GibbonClientOp_SurfaceDecoded    = 136,
    GibbonClientOp_NBPReceive        = 137,
    GibbonClientOp_SuspendChanged    = 138,

    GibbonOp_Unknown      = 0
};

namespace netflix {
namespace gibbon {

inline Serializer &operator<<(Serializer &serializer, const NetworkMetrics &m)
{
    NRDP_UNUSED(m);
    return serializer;
}

inline Deserializer &operator>>(Deserializer &deserializer, NetworkMetrics &m)
{
    NRDP_UNUSED(m);
    return deserializer;
}

inline Serializer &operator<<(Serializer &serializer, const HttpHeaders &h)
{
    NRDP_UNUSED(h);
    return serializer;
}

inline Deserializer &operator>>(Deserializer &deserializer, HttpHeaders &h)
{
    NRDP_UNUSED(h);
    return deserializer;
}

class RemoteScreenServerGraphicsListener : public Screen::Listener, public SurfaceCache::Listener, public Surface::Controller::Listener
{
public:
    inline RemoteScreenServerGraphicsListener(const shared_ptr<RemoteScreenServer::Connection> &connection) : mConnection(connection) { }
    inline ~RemoteScreenServerGraphicsListener() { }

    /* SurfaceCache::Listener */
    virtual void onSurfaceDecode(const SurfaceLoader::Source::SharedPtr &source, const shared_ptr<GibbonResource> &resource) {
        if(shared_ptr<RemoteScreenServer::Connection> connection = mConnection.lock())
            connection->onSurfaceDecode(source, resource);
    }
    virtual void onSurfaceDecoded(const SurfaceCache::DecodeInfo &, const Surface::SharedPtr &surface) {
        if(shared_ptr<RemoteScreenServer::Connection> connection = mConnection.lock())
            connection->onSurfaceDecoded(surface);
    }
    virtual void onSurfaceModified(const Surface::SharedPtr &surface, Modification modification) {
        if(shared_ptr<RemoteScreenServer::Connection> connection = mConnection.lock())
            connection->onSurfaceModified(surface, modification);
    }

    /* Screen::Listener */
    virtual void onScreenRendered(const GraphicsEngine *engine, const Rect &rect) {
        if(shared_ptr<RemoteScreenServer::Connection> connection = mConnection.lock()) {
            assert(engine->isDisplayList());
            connection->onScreenRendered(engine, rect);
        }
    }
    virtual void onScreenShowSplash(bool show) {
        if(shared_ptr<RemoteScreenServer::Connection> connection = mConnection.lock())
            connection->onScreenShowSplash(show);
    }

    /* Surface::Listener */
    virtual void onSurfaceStarted(const Surface::SharedPtr &surface, bool started) {
        if(shared_ptr<RemoteScreenServer::Connection> connection = mConnection.lock())
            connection->onSurfaceStarted(surface, started);
    }

private:
    weak_ptr<RemoteScreenServer::Connection> mConnection;
};

class RemoteScreenResource : public GibbonResource
{
public:
    RemoteScreenResource() : GibbonResource() { }

    virtual SurfaceDecoder::SurfaceInfo surfaceInfo() const { return mInfo; }
    inline void setSurfaceInfo(SurfaceDecoder::SurfaceInfo info) { mInfo = info; }

    virtual shared_ptr<Surface> surfaceDecode(int, uint8_t flags) const {
        if(!mInfo.size.width || !mInfo.size.height)
            return Surface::SharedPtr();
        return Surface::SharedPtr(GibbonApplication::instance()->getSurfaceCache()->requestSurface(mInfo.size.width,
                                                                                                   mInfo.size.height,
                                                                                                   mInfo.flags | flags));
    }
private:
    SurfaceDecoder::SurfaceInfo mInfo;
};

class RemoteScreenLatencyTimer : public Application::Timer
{
public:
    inline RemoteScreenLatencyTimer(const shared_ptr<RemoteScreenConnection> &connection) : Timer(0), mConnection(connection) { }
    virtual std::string describe() const { return "RemoteScreenLatencyTimer"; }
    virtual void timerFired() {
        if(shared_ptr<RemoteScreenConnection> connection = mConnection.lock())
            connection->checkLatency();
    }

private:
    weak_ptr<RemoteScreenConnection> mConnection;
};

class RemoteScreenDisplayListRenderCommand : public DisplayList::Command
{
public:
    inline RemoteScreenDisplayListRenderCommand(const shared_ptr<RemoteScreenClient> &client, uint64_t frameTime) : Command(Type_Unknown), mClient(client), mFrameTime(frameTime), mSent(false) {}
    ~RemoteScreenDisplayListRenderCommand() { process(false); }
    virtual std::string describe() const {
        std::ostringstream s;
        s << "RemoteScreenRender(" << mFrameTime << ")";
        return s.str();
    }
    virtual void apply(GraphicsEngine *, State *) const {
        process(true);
    }

    virtual void write(DisplayList::Writer &) const { }
    virtual void read(DisplayList::Reader &) { }

private:
    void process(bool applied) const {
        if(!mSent) {
            mSent = true;
            if(shared_ptr<RemoteScreenClient> client = mClient.lock())
                client->onScreenRendered(mFrameTime, !applied);
        }
    }
    weak_ptr<RemoteScreenClient> mClient;
    uint64_t mFrameTime;
    mutable bool mSent;
};

class RemoteScreenClientGraphicsListener : public Surface::Controller::Listener
{
public:
    inline RemoteScreenClientGraphicsListener(const shared_ptr<RemoteScreenClient> &client) : mClient(client) { }
    inline ~RemoteScreenClientGraphicsListener() { }

    virtual void onSurfaceChanged(const Surface::SharedPtr &surface, const Rect &) {
        surface->upload();
        if(shared_ptr<RemoteScreenClient> client = mClient.lock())
            gibbonApp()->getScreen()->update();
    }
private:
    weak_ptr<RemoteScreenClient> mClient;
};

class RemoteScreenResourceRequest : public GibbonResourceRequest
{
public:
    RemoteScreenResourceRequest(const shared_ptr<RemoteScreenClient> &client,
                                       const SurfaceLoader::Source::SharedPtr &source) :
        GibbonResourceRequest(GibbonResourceManager::Surface|GibbonResourceManager::Trusted, source->url(), Resource::Method_Get),
        mClient(client)
    {
        if(ullong t = source->timeout().ms())
            setTimeout(t);
        if(ullong t = source->connectTimeout().ms())
            setConnectTimeout(t);
        if(ullong t = source->stallTimeout().ms())
            setStallTimeout(t);
        setRequestHeaders(source->headers());
        setCacheKey(source->cacheKey());
    }

    virtual void finish()
    {
        if(shared_ptr<RemoteScreenClient> client = mClient.lock())
            client->onSurfaceDownloaded(static_pointer_cast<GibbonResource>(resource()), state());
    }
private:
    weak_ptr<RemoteScreenClient> mClient;
};


class RemoteScreenDisplayListWriter : public DisplayList::Writer
{
public:
    inline RemoteScreenDisplayListWriter(RemoteScreenServer::Connection *connection) : mConnection(connection) { }
    inline ~RemoteScreenDisplayListWriter() { }

protected:
    virtual void writeSurface(const Surface::SharedPtr &surface) {
        *this << mConnection->onSurfaceRendered(surface);
    }

private:
    RemoteScreenServer::Connection *mConnection;
};

class RemoteScreenSurfaceLoader : public SurfaceLoader
{
public:
    inline RemoteScreenSurfaceLoader(const shared_ptr<RemoteScreenClient> &client, const SurfaceLoader::Source::SharedPtr &source) : SurfaceLoader(source),
                                                                                                                                     mMutex(ZERO_MUTEX, "RemoteScreenSurfaceLoader"),
                                                                                                                                     mClient(client) { }
    inline ~RemoteScreenSurfaceLoader() { }
    virtual void onSurfaceDecoded(const Surface::SharedPtr &surface, bool)
    {
        if(shared_ptr<RemoteScreenClient> client = mClient.lock())
            client->onSurfaceDecoded(surface, getSource()->cacheKey());
        {
            ScopedMutex _lock(mMutex);
            mSurface = surface;
            mCondition.broadcast();
        }
    }

    inline Surface::SharedPtr wait() {
        ScopedMutex _lock(mMutex);
        while(!mSurface)
            mCondition.wait(&mMutex);
        return mSurface;
    }

private:
    Mutex mMutex;
    Surface::SharedPtr mSurface;
    ConditionVariable mCondition;
    weak_ptr<RemoteScreenClient> mClient;
};

class RemoteScreenNBPEventConnection : public EventConnection
{
public:
    inline RemoteScreenNBPEventConnection(const shared_ptr<RemoteScreenClient> &client) : EventConnection("RemoteScreen"), mClient(client)
    {
        if(client->flags() & RemoteScreenClient::Flag_DPI) {
            std::set<std::string> in;
            in.insert("nrdp.log");
            in.insert("nrdp.mdx");
            in.insert("nrdp.player");
            in.insert("nrdp.mediarequest");
            setFilters(in, std::set<std::string>());
        }
    }
    inline ~RemoteScreenNBPEventConnection() { }

protected:
    virtual bool writeResponse(const Response &response) {
        if(shared_ptr<RemoteScreenClient> client = mClient.lock())
            client->handleNBPReceive(response);
        return true;
    }

private:
    weak_ptr<RemoteScreenClient> mClient;
};

class RemoteScreenDisplayListReader : public DisplayList::Reader
{
public:
    inline RemoteScreenDisplayListReader(RemoteScreenClient *client) : mClient(client) { }
    inline ~RemoteScreenDisplayListReader() { }

protected:
    virtual void readSurface(Surface::SharedPtr &surface) {
        uint32_t id;
        *this >> id;
        surface = mClient->findSurface(id);
    }

private:
    RemoteScreenClient *mClient;
};

}}

RemoteScreenServer::RemoteScreenServer(int display)
    : Thread(&THREAD_GIBBON_REMOTESCREEN_SERVER)
    , mPort(GIBBON_REMOTESCREEN_PORT+display)
    , mListeningSocket(-1)
    , mQuit(false)
{
    if(pipe(mPipe) == -1)
        Log::error(TRACE_GIBBON_REMOTESCREEN, "Unable to create pipe for RemoteScreenServer (%d)", errno);
    else
        fcntl(mPipe[0], F_SETFL, O_NONBLOCK);
}

RemoteScreenServer::~RemoteScreenServer()
{
    quit();
    Wait();
    close(mPipe[0]);
    close(mPipe[1]);
    if(mListeningSocket >= 0)
        close(mListeningSocket);
    mListeningSocket = -1;
    assert(mConnections.empty());
}

shared_ptr<GibbonResource> RemoteScreenServer::handleSurfaceDownload(const ResourceRequest::SharedPtr &request)
{
#if defined(GIBBON_GRAPHICSENGINE_NULL)
    if(!isLocalUrl(request->url()) && !mConnections.empty()) {
        for(std::set<shared_ptr<RemoteScreenServer::Connection> >::iterator it = mConnections.begin(); it != mConnections.end(); ++it) {
            if(shared_ptr<GibbonResource> resource = (*it)->onSurfaceDownload(request))
                return resource;
        }
    }
#else
    NRDP_UNUSED(request);
#endif
    return shared_ptr<GibbonResource>();
}

bool RemoteScreenServer::handleNBPSend(const Request &request)
{
    enum {
        None    = 0x00,
        Send    = 0x01,
        Consume = 0x02
    };
    uint8_t used = None;
    shared_ptr<NfObject> object = request.object();
#ifndef GIBBON_REMOTESCREEN_NO_NBP
    if(object) {
        if(object->path() == "nrdp.player") {
            used = Send|Consume;
        } else if(object->path() == "nrdp.mdx") {
            used = Send|Consume;
        } else if(object->path() == "nrdp.mediarequest") {
            used = Send|Consume;
        } else if(request.type() == Request::Method && object->path() == "nrdp") {
            const std::string method = request.methodName();
            if(method == "setConfigData" || method == "setTrustStore" || method == "setTestDriverIpAddress")
                used = Send;
        } else if(request.type() == Request::Method && object->path() == "nrdp.log") {
            used = Send;
        }
    }
#endif
#if 0
    Log::error(TRACE_LOG, "[%s:%d]: Handling: %s [0x%x]", __FILE__, __LINE__, request->describe().c_str(), used);
#endif
    if(used & Send) {
        assert(!(request.flags() & Request::Synchronous));
        for(std::set<shared_ptr<RemoteScreenServer::Connection> >::iterator it = mConnections.begin(); it != mConnections.end(); ++it) {
            if((*it)->flags() & RemoteScreenConnection::Flag_DPI) {
                Serializer serializer;
                serializer << request.variant();
                (*it)->send(GibbonServerOp_NBPSend, serializer);
                return (used & Consume);
            }
        }
    }
    return false;
}

void RemoteScreenServer::ping(bool dump) const
{
    Log::error(TRACE_GIBBON_REMOTESCREEN, "Connections: %zu", mConnections.size());
    for(std::set<shared_ptr<RemoteScreenServer::Connection> >::const_iterator it = mConnections.begin(); it != mConnections.end(); ++it)
        (*it)->ping(dump);
}

void RemoteScreenServer::dumpSurfaces() const
{
    Log::error(TRACE_GIBBON_REMOTESCREEN, "Connections: %zu", mConnections.size());
    for(std::set<shared_ptr<RemoteScreenServer::Connection> >::const_iterator it = mConnections.begin(); it != mConnections.end(); ++it)
        (*it)->dumpSurfaces();
}

void RemoteScreenServer::dump() const
{
    Log::error(TRACE_GIBBON_REMOTESCREEN, "Connections: %zu", mConnections.size());
    for(std::set<shared_ptr<RemoteScreenServer::Connection> >::const_iterator it = mConnections.begin(); it != mConnections.end(); ++it)
        (*it)->dump();
}

void RemoteScreenServer::startAnimation()
{
    for(std::set<shared_ptr<RemoteScreenServer::Connection> >::const_iterator it = mConnections.begin(); it != mConnections.end(); ++it)
        (*it)->send(GibbonServerOp_AnimationStart);
}

void RemoteScreenServer::endAnimation()
{
    for(std::set<shared_ptr<RemoteScreenServer::Connection> >::const_iterator it = mConnections.begin(); it != mConnections.end(); ++it)
        (*it)->send(GibbonServerOp_AnimationEnd);
}

void RemoteScreenServer::Run()
{
    unsigned int listenFailCount = 0;
    handleIdleSuspend();
    while(true) {
        if(mQuit)
            break;
        if(mListeningSocket < 0) {
            if(!createListeningSocket()) {
                listenFailCount++;
                if(++listenFailCount > 10) {
                    Log::error(TRACE_GIBBON_REMOTESCREEN, "Too many failures attempting to listen, shutting down");
                    break;
                }
                Sleep(3000);
                continue;
            }
            handleConnections();
        }

        fd_set read_set;
        int result;
        do {
            FD_ZERO(&read_set);
            FD_SET(mPipe[0], &read_set);
            FD_SET(mListeningSocket, &read_set);
            result = select(std::max(mPipe[0], mListeningSocket) + 1, &read_set, 0, 0, 0);
        } while(result == -1 && errno == EINTR);

        if(mQuit)
            break;

        if(result < 0) {
            Log::error(TRACE_GIBBON_REMOTESCREEN, "Failed to select");
            break;
        }
        for(std::set<shared_ptr<RemoteScreenServer::Connection> >::iterator it = mConnections.begin(); it != mConnections.end(); ) {
            if(!(*it)->isRunning()) {
                (*it)->Wait();
                mConnections.erase(it++);
            } else {
                ++it;
            }
        }
        if(FD_ISSET(mPipe[0], &read_set)) {
            char buff;
            const int r = read(mPipe[0], &buff, 1);
            if(r == -1) {
                Log::error(TRACE_GIBBON_REMOTESCREEN, "Failed to read from wakeup pipe: %d", errno);
                break;
            } else if(r == 0) {
                Log::error(TRACE_GIBBON_REMOTESCREEN, "Failed to read from wakeup pipe, connection closed");
                break;
            } else if(buff == 'q') {
                assert(r == 1);
                break;
            }
        }
        if(FD_ISSET(mListeningSocket, &read_set))
            handleConnections();
    }

    NTRACE(TRACE_GIBBON_REMOTESCREEN, "RemoteScreenServer going away, clearing %zu clients", mConnections.size());
    for(std::set<shared_ptr<RemoteScreenServer::Connection> >::iterator it = mConnections.begin(); it != mConnections.end(); ++it)
        (*it)->quit();
}

void RemoteScreenServer::handleConnections()
{
    while(true) {
        const int acceptSocket = accept(mListeningSocket, NULL, NULL);
        if(acceptSocket < 0)
            break;

        int buflen;
        socklen_t optLen = sizeof(buflen);
        if(!getsockopt(acceptSocket, SOL_SOCKET, SO_SNDBUF, &buflen, &optLen)) {
            NTRACE(TRACE_GIBBON_REMOTESCREEN, "doubling size of sndbuf from %d to %d", buflen, buflen << 1);
            buflen <<= 1;
            setsockopt(acceptSocket, SOL_SOCKET, SO_SNDBUF, &buflen, optLen);
        }
        shared_ptr<RemoteScreenServer::Connection> connection(new RemoteScreenServer::Connection(this));
        mConnections.insert(connection);
        connection->openSocket(acceptSocket);
    }
    handleIdleSuspend();
}

void RemoteScreenServer::handleSuspend(const Variant &reason)
{
#if defined(GIBBON_GRAPHICSENGINE_NULL)
    bool suspend = true;
    if(!mConnections.empty()) {
        suspend = false;
        for(std::set<shared_ptr<RemoteScreenServer::Connection> >::iterator it = mConnections.begin(); it != mConnections.end(); ++it) {
            if((*it)->isSuspended()) {
                suspend = true;
                break;
            }
        }
    }
    if(nrdApp()->isSuspended() != suspend)
        nrdApp()->setSuspended(suspend, reason);
#else
    NRDP_UNUSED(reason);
#endif
}

bool RemoteScreenServer::createListeningSocket()
{
    NTRACE(TRACE_GIBBON_REMOTESCREEN, "creating listening socket for port %d", mPort);
    int listeningSocket = socket(AF_INET, SOCK_STREAM, 0);
    if(listeningSocket < 0) {
        Log::error(TRACE_GIBBON_REMOTESCREEN, "Could not create socket");
        return false;
    }

    int flags = fcntl(listeningSocket, F_GETFL, 0);
    if(flags != -1)
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
    if(result < 0) {
        close(listeningSocket);
        return false;
    }

    result = listen(listeningSocket, 1);
    if(result < 0) {
        Log::error(TRACE_GIBBON_REMOTESCREEN, "Could not listen on port");
        close(listeningSocket);
        return false;
    }

    NTRACE(TRACE_GIBBON_REMOTESCREEN, "Successfully listening for clients");
    mListeningSocket = listeningSocket;
    return true;
}

void RemoteScreenServer::wakeup()
{
    if(!mQuit)
        NRDP_UNUSED_RETURN(write(mPipe[1], "w", 1));
}

void RemoteScreenServer::quit()
{
    mQuit = true;
    NRDP_UNUSED_RETURN(write(mPipe[1], "q", 1));
}

RemoteScreenConnection::RemoteScreenConnectionWriter::RemoteScreenConnectionWriter() :
    Thread(&THREAD_GIBBON_REMOTESCREEN_CONNECTION_WRITER),
    mMutex(GIBBON_REMOTESCREEN_CONNECTION_WRITER_MUTEX, "RemoteScreenConnection::Writer"),
    mQuit(false), mOffset(0), mConnection(0)
{
    if(pipe(mPipe) == -1)
        Log::error(TRACE_GIBBON_REMOTESCREEN, "Unable to create pipe for RemoteScreenConnectionWriter (%d)", errno);
    else
        fcntl(mPipe[0], F_SETFL, O_NONBLOCK);
}

void RemoteScreenConnection::RemoteScreenConnectionWriter::init(RemoteScreenConnection *connection)
{
    mConnection = connection;
}

RemoteScreenConnection::RemoteScreenConnectionWriter::~RemoteScreenConnectionWriter()
{
    quit();
    Wait();
    close(mPipe[0]);
    close(mPipe[1]);
}

void RemoteScreenConnection::RemoteScreenConnectionWriter::Run()
{
    while(true) {
        bool haveWriteData;
        fd_set read_set, write_set;
        while(true) {
            int maxSocket = 0;
            {
                ScopedMutex _lock(mMutex);
                if(mQuit)
                    return;
                const int socket = mConnection->socket();
                if(socket == -1)
                    return;
                FD_ZERO(&read_set);
                FD_ZERO(&write_set);
                maxSocket = mPipe[0];
                FD_SET(mPipe[0], &read_set);
                haveWriteData = (mData.size()-mOffset);
                if(haveWriteData) {
                    FD_SET(socket, &write_set);
                    if(socket > maxSocket)
                        maxSocket = socket;
                }
            }
            const int result = select(maxSocket+1, &read_set, haveWriteData ? &write_set : 0, 0, 0);
            if(result == -1) {
                if(errno != EINTR)
                    return;
            } else {
                break;
            }
        }
        {
            ScopedMutex _lock(mMutex);
            if(mQuit)
                break;
            if(FD_ISSET(mPipe[0], &read_set)) {
                char byte;
                while(read(mPipe[0], &byte, 1) != -1);
            }
        }
        if(haveWriteData && !sendPendingData())
            break;
    }
}

void RemoteScreenConnection::RemoteScreenConnectionWriter::quit()
{
    ScopedMutex _lock(lock());
    mQuit = true;
    NRDP_UNUSED_RETURN(write(mPipe[1], "q", 1));
}

void RemoteScreenConnection::RemoteScreenConnectionWriter::wakeup() const
{
    ScopedMutex _lock(lock());
    NRDP_UNUSED_RETURN(write(mPipe[1], "w", 1));
}

RemoteScreenConnection::RemoteScreenConnection() : Thread(&THREAD_GIBBON_REMOTESCREEN_CONNECTION),
                                                   mLocalBaseTime(0), mRemoteBaseTime(0),
                                                   mSocketMutex(GIBBON_REMOTESCREEN_CONNECTION_SOCKET_MUTEX, "RemoteScreenConnection::Socket"),
                                                   mConnected(false), mSocket(-1),
                                                   mQuitMutex(ZERO_MUTEX, "RemoteScreenConnection::Quit"),
                                                   mQuit(false), mRunning(true)
{
    mOutput.init(this);
    initFlags();
    if(pipe(mPipe) == -1)
        Log::error(TRACE_GIBBON_REMOTESCREEN, "Unable to create pipe for RemoteScreenConnection (%d)", errno);
    else
        fcntl(mPipe[0], F_SETFL, O_NONBLOCK);
}

RemoteScreenConnection::~RemoteScreenConnection()
{
    quit();
    Wait();
    close(mPipe[0]);
    close(mPipe[1]);
    closeSocket();
}

#if defined(GIBBON_REMOTESCREEN_ACCOUNTING)
struct StatsInfo {
    StatsInfo(const char *d, Time b) : desc(d), bound(b), total(0) { }
    const char *desc;
    Time bound;
    int total;
};
static std::string stats(const Time &now, std::list<std::pair<Time, int> > &data)
{
    std::vector<StatsInfo> info;
    info.push_back(StatsInfo("30s", Time::fromSeconds(30)));
    info.push_back(StatsInfo("1m", Time::fromSeconds(60)));
    info.push_back(StatsInfo("5m", Time::fromSeconds(5*60)));
    for(std::list<std::pair<Time, int> >::iterator it = data.begin(); it != data.end(); ) {
        int match = 0;
        for(std::vector<StatsInfo>::iterator info_it = info.begin(); info_it != info.end(); ++info_it) {
            if((*it).first >= now-(*info_it).bound) {
                (*info_it).total += (*it).second;
                ++match;
            }
        }
        if(!match)
            it = data.erase(it);
        else
            ++it;
    }

    std::ostringstream result;
    for(std::vector<StatsInfo>::const_iterator info_it = info.begin(); info_it != info.end(); ++info_it)
        result << " (" << (*info_it).desc << " " << (((*info_it).total/1024) / (*info_it).bound.seconds()) << "kB/s)";
    return result.str();
}
#endif

void RemoteScreenConnection::dump() const
{
    ScopedMutex _lock(mStats.mutex);
    const Time now = Time::mono();
    mStats.lastDump = now;
    Log::error(TRACE_GIBBON_REMOTESCREEN, "---------- RemoteScreen: %p ------ \n", this);
#if defined(GIBBON_REMOTESCREEN_ACCOUNTING)
    Log::error(TRACE_GIBBON_REMOTESCREEN, "Input:%p:%s", this, stats(now, mStats.received).c_str());
    Log::error(TRACE_GIBBON_REMOTESCREEN, "Output:%p:%s", this, stats(now, mStats.sent).c_str());
#endif
    Log::error(TRACE_GIBBON_REMOTESCREEN, "Latency:%p: %dms", this, mStats.latency);
    Log::error(TRACE_GIBBON_REMOTESCREEN, "Frames:%p: %d / %d [%d%%]", this, mStats.renderedFrames, mStats.droppedFrames,
               int((float(mStats.droppedFrames) / mStats.renderedFrames) * 100));
}

void RemoteScreenConnection::updateLatency(uint64_t time)
{
    ScopedMutex _lock(mStats.mutex);
    const Time now = Time::mono();
    mStats.lastPong = now;
    mStats.latency = now.ms() - time;
    if(mStats.dumpLatency) {
        mStats.dumpLatency = false;
        Log::error(TRACE_GIBBON_REMOTESCREEN, "Latency:%p: %dms", this, mStats.latency);
    }
    if(now-mStats.lastDump >= Time::fromSeconds(30))
        dump();
}

void RemoteScreenConnection::ping(bool dump)
{
    {
        ScopedMutex _lock(mStats.mutex);
        mStats.dumpLatency = dump;
    }
    Serializer serializer;
    serializer << Time::mono().ms();
    send(GibbonOp_Ping, serializer);
}

void RemoteScreenConnection::checkLatency()
{
    if(isActive()) {
        {
            ScopedMutex _lock(mStats.mutex);
            if(!mStats.lastPong.ms())
                return;
            mStats.lastPong = Time(0);
        }
        ping(false);
    }
}

void RemoteScreenConnection::initFlags()
{
    mFlags = 0;
#if defined(NRDP_LITTLE_ENDIAN)
    mFlags |= Flag_CPULittleEndian;
#endif
#if defined(NRDP_GPU_LITTLE_ENDIAN)
    mFlags |= Flag_GPULittleEndian;
#endif
#if !defined(GIBBON_REMOTESCREEN_NO_COMPRESS)
    mFlags |= Flag_CompressGzip;
#endif
#if !defined(GIBBON_REMOTESCREEN_NO_BSDIFF)
    mFlags |= Flag_CompressBSDiff;
#endif
}

void RemoteScreenConnection::openSocket(int socket)
{
    {
        ScopedMutex _lock(mSocketMutex);
        mConnected = true;
        mInput.clear();
        initFlags();

        if(mSocket != -1) {
            ::close(mSocket);
            mSocket = -1;
        }
        mSocket = socket;
        {
            const int flags = fcntl(mSocket, F_GETFL, 0);
            if(flags == -1 || fcntl(mSocket, F_SETFL, flags | O_NONBLOCK) == -1)
                Log::error(TRACE_GIBBON_REMOTESCREEN, "[%s:%d]: Unable to set NONBLOCK!", __FILE__, __LINE__);
        }
        {
            int flag = 1;
            if(setsockopt(mSocket, IPPROTO_TCP, TCP_NODELAY, (char *)&flag, sizeof(flag)) < 0)
                Log::error(TRACE_GIBBON_REMOTESCREEN, "[%s:%d]: Unable to set NODELAY!", __FILE__, __LINE__);
        }
    }
    if(GetState() != RUNNING) {
        Start();
        mOutput.Start();
    }
}

bool RemoteScreenConnection::receive(uint8_t &op, Deserializer &data)
{
    while(receive_internal(op, data)) {
        switch(op) {
        case GibbonOp_Pong: {
            ullong then;
            data >> then;
            updateLatency(then);
            break; }
        case GibbonOp_Ping: {
            ullong ms;
            data >> ms;
            {
                Serializer serializer;
                serializer << ms;
                send(GibbonOp_Pong, serializer);
            }
            break; }
        default:
            return true;
        }
    }
    return false;
}

void RemoteScreenConnection::closeSocket()
{
    ScopedMutex _lock(mSocketMutex);
    if(mSocket != -1) {
        close(mSocket);
        mSocket = -1;
    }
}

void RemoteScreenConnection::quit()
{
    ScopedMutex _lock(mSocketMutex);
    if(!mQuit) {
        mQuit = true;
        NRDP_UNUSED_RETURN(::write(mPipe[1], "q", 1));
    }
}

void RemoteScreenConnection::wakeup() const
{
    ScopedMutex _lock(mSocketMutex);
    if(!mQuit)
        NRDP_UNUSED_RETURN(::write(mPipe[1], " ", 1));
}

bool RemoteScreenConnection::waitForData()
{
    fd_set read_set;
    while(true) {
        int maxSocket = 0;
        {
            ScopedMutex _lock(mSocketMutex);
            if(mQuit || mSocket == -1)
                return false;
            FD_ZERO(&read_set);
            maxSocket = std::max(mPipe[0], mSocket);
            FD_SET(mPipe[0], &read_set);
            FD_SET(mSocket, &read_set);
        }
#if defined(DEBUG_REMOTESCREEN_DATARECV) && DEBUG_REMOTESCREEN_DATARECV >= 3
        Log::error(TRACE_GIBBON_REMOTESCREEN, "%p: Waiting for Data", this);
#endif
        const int result = select(maxSocket+1, &read_set, 0, 0, 0);
#if defined(DEBUG_REMOTESCREEN_DATARECV) && DEBUG_REMOTESCREEN_DATARECV >= 3
        Log::error(TRACE_GIBBON_REMOTESCREEN, "%p: Done Waiting for Data (%d)", this, result);
#endif
        if(result == -1) {
            if(errno != EINTR)
                return false;
        } else {
            break;
        }
    }

    int total_received = 0;
    {
        ScopedMutex _lock(mSocketMutex);
        if(mQuit)
            return false;

        if(FD_ISSET(mPipe[0], &read_set)) {
            char byte;
            while(read(mPipe[0], &byte, 1) != -1);
        }

        if(mSocket < 0)
            return false;
        if(FD_ISSET(mSocket, &read_set)) {
            const int bufSize = 8096;
            unsigned char buf[bufSize];
            while(true) {
                const ssize_t received = recv(mSocket, buf, bufSize - 1, 0);
#if defined(DEBUG_REMOTESCREEN_DATARECV) && DEBUG_REMOTESCREEN_DATARECV >= 4
                Log::error(TRACE_GIBBON_REMOTESCREEN, "  + Received (%d)", received);
#endif
#if defined(DEBUG_REMOTESCREEN_DATARECV) && DEBUG_REMOTESCREEN_DATARECV >= 5
                if(received > 0)
                    Log::error(TRACE_GIBBON_REMOTESCREEN, "%s", Log::hexDump(buf, received).c_str());
#endif

                if(received <= 0) {
                    if(received == -1 && errno == EWOULDBLOCK)
                        break;
                    return false;
                }
                total_received += received;
                mInput.append(buf, received);
            }
        }
    }
#if defined(GIBBON_REMOTESCREEN_ACCOUNTING)
    {
        const Time now = Time::mono();
        ScopedMutex _lock(mStats.mutex);
        mStats.received.push_back(std::make_pair(now, total_received));
        if(now-mStats.lastDump >= Time::fromSeconds(30))
            dump();
    }
#endif
    return true;
}

void RemoteScreenConnection::Run()
{
    shared_ptr<RemoteScreenLatencyTimer> latencyTimer;
    Log::info(TRACE_GIBBON_REMOTESCREEN, "RemoteScreen(%p): connected", this);
    started();
    while(mConnected) {
        if(!waitForData())
            mConnected = false;
        if(!processInput())
            break;
        if(!latencyTimer && isActive()) {
            latencyTimer.reset(new RemoteScreenLatencyTimer(shared_from_this()));
            latencyTimer->restart(10000);
        }
        trimInput();
    }
    Log::warn(TRACE_GIBBON_REMOTESCREEN, "RemoteScreen(%p): disconnected", this);
    if(latencyTimer)
        latencyTimer->stop();
    {
        ScopedMutex _lock(mQuitMutex);
        mRunning = false;
    }
    closeSocket();
    finished();
}

RemoteScreenServer::Connection::Connection(RemoteScreenServer *server) : RemoteScreenConnection(), mServer(server),
                                                                         mMutex(GIBBON_REMOTESCREENSERVER_CONNECTION_MUTEX, "RemoteScreenServer::Connection"),
                                                                         mState(State_HandShake)

{
    NRDP_OBJECTCOUNT_REF(GibbonRemoteScreenConnection, this);
}

RemoteScreenServer::Connection::~Connection()
{
    gibbonApp()->getScreen()->removeListener(mListener);
    gibbonApp()->getSurfaceCache()->removeListener(mListener);
    mServer->wakeup();
    NRDP_OBJECTCOUNT_DEREF(GibbonRemoteScreenConnection, this);
}


void RemoteScreenServer::Connection::dump() const
{
    RemoteScreenConnection::dump();
    {
        ScopedMutex _lock(mMutex);
        for(std::map<DataBuffer, shared_ptr<RemoteScreenResource> >::const_iterator it = mResources.begin(); it != mResources.end(); ++it)
            Log::error(TRACE_GIBBON_REMOTESCREEN, "Resource: (%s) [%s]", it->first.toString().c_str(), it->second->url().c_str());
    }
}

void RemoteScreenServer::Connection::dumpSurfaces() const
{
    std::map<DataBuffer, RemoteSurface*> surfaces;
    {
        ScopedMutex _lock(mMutex);
        surfaces = mSurfaces;
    }
    Log::error(TRACE_GIBBON_REMOTESCREEN, "---------- RemoteScreen: %p ------ \n", this);
    for(std::map<DataBuffer, RemoteSurface*>::const_iterator it = surfaces.begin(); it != surfaces.end(); ++it) {
        Surface::SharedPtr surface = gibbonApp()->getSurfaceCache()->find(it->first);
        Log::error(TRACE_GIBBON_REMOTESCREEN, "Surface: (%d:%s) [%s]", it->second->state, it->first.c_str(),
                   surface ? surface->describe().c_str() : "N/A");
    }
}

void RemoteScreenServer::Connection::finished()
{
    std::map<DataBuffer, shared_ptr<RemoteScreenResource> > resources;
    {
        ScopedMutex _lock(mMutex);
        for(std::map<DataBuffer, RemoteSurface*>::iterator it = mSurfaces.begin(); it != mSurfaces.end(); ++it) {
            if((*it).second->state == RemoteSurface::State_Decoding) {
                (*it).second->state = RemoteSurface::State_Aborted;
                (*it).second->condition.broadcast();
            }
        }
        std::swap(resources, mResources);
    }
    for(std::map<DataBuffer, shared_ptr<RemoteScreenResource> >::const_iterator it = resources.begin(); it != resources.end(); ++it)
        nrdApp()->resourceManager()->finish(it->second, AS_NO_ERROR, 500, Resource::State_Network);
    mServer->wakeup();
}

bool RemoteScreenServer::Connection::processInput()
{
    while(true) {
        if(state() == State_Ready) {
            uint8_t op;
            Deserializer data;
            while(receive(op, data)) {
                switch(op) {
                case GibbonClientOp_SurfaceDownloaded: {
                    shared_ptr<RemoteScreenResource> resource;
                    DataBuffer cacheKey;
                    data >> cacheKey;
                    {
                        ScopedMutex _lock(mMutex);
                        std::map<DataBuffer, shared_ptr<RemoteScreenResource> >::iterator it = mResources.find(cacheKey);
                        if(it != mResources.end()) {
                            resource = it->second;
                            mResources.erase(it);
                        }
                    }
                    if(resource) {
                        DEBUG_REMOTESCREEN_SURFACE("ClientDownloaded:%p: [%s]", this, ResourceManager::prettyCacheKey(cacheKey).toString().c_str());
                        SurfaceDecoder::SurfaceInfo info;
                        data >> info.size;
                        data >> info.flags;
                        resource->setSurfaceInfo(info);
                        int32_t nativeErrorCode;
                        data >> nativeErrorCode;
                        int32_t httpStatusCode;
                        data >> httpStatusCode;
                        NetworkMetrics metrics;
                        data >> metrics;
                        nrdApp()->resourceManager()->finish(resource, nativeErrorCode, httpStatusCode, Resource::State_Network, metrics);
                    }
                    break; }
                case GibbonClientOp_SurfaceDecoded: {
                    DataBuffer cacheKey;
                    data >> cacheKey;
                    bool success;
                    data >> success;
                    bool controller;
                    data >> controller;
                    Surface::WeakPtr surface;
                    {
                        ScopedMutex _lock(mMutex);
                        DEBUG_REMOTESCREEN_SURFACE("ClientDecoded:%p: %s", this, ResourceManager::prettyCacheKey(cacheKey).toString().c_str());
                        std::map<DataBuffer, RemoteSurface*>::iterator it = mSurfaces.find(cacheKey);
                        if(it != mSurfaces.end()) {
                            RemoteSurface *s = it->second;
                            if(s->state == RemoteSurface::State_Decoding) {
                                s->state = RemoteSurface::State_Decoded;
                                s->needsController = controller;
                                s->condition.broadcast();
                            }
                            surface = s->surface;
                        }
                    }
                    if(controller) {
                        ScopedMutex _lock(SurfaceCache::lock());
                        if(Surface::SharedPtr s = surface.lock())
                            setupController(s);
                    }
                    break; }
                case GibbonClientOp_ScreenRendered: {
                    uint64_t frameTime;
                    data >> frameTime;
                    bool dropped;
                    data >> dropped;
                    {
                        ScopedMutex _lock(mStats.mutex);
                        --mStats.pendingFrames;
                        assert(mStats.pendingFrames >= 0);
                        DEBUG_REMOTESCREEN_FRAME("[%lld]:%p:CLIENT_%s(%d)", (ullong)frameTime, this, dropped ? "DROPPED" : "RENDERED", mStats.pendingFrames);
                    }
                    break; }
                case GibbonClientOp_NBPReceive: {
                    Variant variant;
                    data >> variant;
                    nrdApp()->sendResponse(Response::fromVariant(variant));
                    break; }
                case GibbonClientOp_Quit: {
                    gibbonApp()->quit();
                    break; }
                case GibbonClientOp_Eval: {
                    std::string command;
                    data >> command;
                    if(shared_ptr<Console> console = app()->console())
                        console->handleCommand(command, Console::Console_NoHistory);
                    break; }
                case GibbonClientOp_SuspendChanged: {
                    bool suspend;
                    data >> suspend;
                    Variant reason;
                    data >> reason;
                    mSuspended = suspend;
                    mServer->handleSuspend();
                    break; }
                case GibbonClientOp_Event: {
                    uint8_t type;
                    data >> type;
                    switch(type) {
                    case GibbonEvent::Type_KeyPress:
                    case GibbonEvent::Type_KeyRelease:
                    case GibbonEvent::Type_KeyClick: {
                        KeyEvent *keyEvent = new KeyEvent((GibbonEvent::Type)type);
                        data >> keyEvent->mModifiers;
                        data >> keyEvent->mKey;
                        data >> keyEvent->mText;
                        data >> keyEvent->mUIKey;
                        data >> keyEvent->mCount;
                        data >> keyEvent->mRepeat;
                        gibbonApp()->postEvent(keyEvent);
                        break; }
                    case GibbonEvent::Type_MouseChange: {
                        MouseChangeEvent *mouseEvent = new MouseChangeEvent;
                        data >> mouseEvent->mVisible;
                        gibbonApp()->postEvent(mouseEvent);
                        break; }
                    case GibbonEvent::Type_MouseMove:
                    case GibbonEvent::Type_MousePress:
                    case GibbonEvent::Type_MouseRelease: {
                        MouseEvent *mouseEvent = new MouseEvent((GibbonEvent::Type)type);
                        data >> mouseEvent->mModifiers;
                        data >> mouseEvent->mPos;
                        data >> mouseEvent->mButton;
                        data >> mouseEvent->mButtons;
                        gibbonApp()->postEvent(mouseEvent);
                        break; }
                    case GibbonEvent::Type_FocusChange: {
                        FocusChangeEvent *focusEvent = new FocusChangeEvent;
                        data >> focusEvent->mFocus;
                        gibbonApp()->postEvent(focusEvent);
                        break; }
                    default:
                        assert(false);
                        break;
                    }
                    break; }
                }
            }
            break;
        } else if(state() == State_HandShake) {
            DataBuffer clientVersion = receiveLine();
            assert(clientVersion.startsWith("GIBBON "));
            send((const unsigned char *)"CONNECTED\n", 10);

            Serializer serializer;
            uint8_t flags = this->flags();
#ifndef GIBBON_REMOTESCREEN_NO_NBP
            flags |= Flag_DPI;
#endif
            serializer << flags;
            serializer << uint64_t(Time::mono().ms());
            serializer << uint32_t(getpid());
            serializer << uint16_t(NRDP_VERSION_MAJOR) << uint16_t(NRDP_VERSION_MINOR) << uint16_t(NRDP_VERSION_PATCH);
            serializer << uint16_t(GibbonConfiguration::screenWidth());
            serializer << uint16_t(GibbonConfiguration::screenHeight());
            send(GibbonServerOp_Init, serializer);
            {
                ScopedMutex _lock(mMutex);
                char esnBuffer[513];
                sscanf(clientVersion.c_str(), "GIBBON %04d.%02d.%02d %100s\n", &mVerMajor, &mVerMinor, &mVerPatch, esnBuffer);
                mESN = esnBuffer;
                Log::warn(TRACE_GIBBON_REMOTESCREEN, "Client:%p: %d.%d.%d (%s)", this, mVerMajor, mVerMinor, mVerPatch, mESN.c_str());
                mState = State_Init;
            }
        } else if(state() == State_Init) {
            uint8_t op;
            Deserializer data;
            if(!receive(op, data))
                break;
            shared_ptr<RemoteScreenServer::Connection> that = static_pointer_cast<RemoteScreenServer::Connection>(shared_from_this());
            assert(op == GibbonClientOp_Init);
            uint8_t flags;
            data >> flags;
            setFlags(flags);
            uint64_t baseTime;
            data >> baseTime;
            updateBaseTime(baseTime);
            uint32_t pid;
            data >> pid;
            bool suspended;
            data >> suspended;
            uint32_t surfaceCacheCapacity;
            data >> surfaceCacheCapacity;
            if(surfaceCacheCapacity < gibbonApp()->getSurfaceCache()->capacity())
                gibbonApp()->getSurfaceCache()->setCapacity(surfaceCacheCapacity);
            uint16_t targetFPS;
            data >> targetFPS;
            uint16_t width, height;
            data >> width;
            data >> height;

            Screen::SharedPtr screen = gibbonApp()->getScreen();
            SurfaceCache::SharedPtr surfaceCache = gibbonApp()->getSurfaceCache();
            shared_ptr<RemoteScreenServerGraphicsListener> listener(new RemoteScreenServerGraphicsListener(that));
            screen->addListener(listener);
            surfaceCache->addListener(listener);
            surfaceCache->abortDecodeJobs();

            {
                ScopedMutex _widget_lock(Widget::lock());
                ScopedMutex _animation_lock(Animation::lock());
                ScopedMutex _screen_lock(Screen::lock());
                {
                    ScopedMutex _connection_lock(mMutex);
                    Log::warn(TRACE_GIBBON_REMOTESCREEN, " Screen:%p:%d: %dx%d [0x%x]", this, pid, width, height, flags);
                    mSuspended = suspended;
                    mListener = listener;
                    mState = State_Ready;
                    mPid = pid;
                }
                GibbonFreetype::clearCache();
                Widget::purgeAll(Widget::Purge_Force);
                surfaceCache->clear(SurfaceCache::RemoteClient);
                screen->setTargetFPS(std::min(screen->getTargetFPS(), (int)targetFPS));
                screen->update();
            }
            mServer->handleSuspend("serverIdle");
        }
    }
    return true;
}

void RemoteScreenServer::Connection::onSurfaceStarted(const Surface::SharedPtr &surface, bool show)
{
    if(!isActive())
        return;

    Serializer serializer;
    serializer << surface->cacheKey();
    serializer << show;
    send(GibbonServerOp_SurfaceStarted, serializer);
}

void RemoteScreenServer::Connection::onSurfaceDecode(const SurfaceLoader::Source::SharedPtr &source,
                                                     const shared_ptr<GibbonResource> &resource)
{
    if(!isActive())
        return;

    uint32_t id;
    const DataBuffer cacheKey = resource->cacheKey();
    {
        ScopedMutex _lock(mMutex);
        if(mSurfaces.find(cacheKey) != mSurfaces.end())
           return;
        id = sSurfaceSerial.ref();
        mSurfaces.insert(std::make_pair(cacheKey, new RemoteSurface(RemoteSurface::State_Decoding, id)));
    }
    DEBUG_REMOTESCREEN_SURFACE("ServerDecode:%p: %s", this, ResourceManager::prettyCacheKey(cacheKey).c_str());

    Serializer serializer;
    serializer << cacheKey;
    serializer << id;
    if(mServer->isLocalUrl(resource->url())) {
        serializer << uint8_t(SurfaceDecodeType_Data);
        serializer << resource->responseData();
    } else {
        serializer << uint8_t(SurfaceDecodeType_URL);
        serializer << resource->url().str();
        serializer << source->headers();
    }
    send(GibbonServerOp_SurfaceDecode, serializer);
}

void RemoteScreenServer::Connection::setupController(const Surface::SharedPtr &surface)
{
    assert(surface);
    DEBUG_REMOTESCREEN_SURFACE("ServerListener:%p: %s", this, surface->describe().c_str());
    if(!surface->getController())
        surface->setController(Surface::Controller::SharedPtr(new Surface::Controller(surface)));
    surface->getController()->addListener(mListener);
}

shared_ptr<GibbonResource> RemoteScreenServer::Connection::onSurfaceDownload(const ResourceRequest::SharedPtr &request)
{
    if(isActive()) {
        const DataBuffer cacheKey = request->cacheKey();

        Serializer serializer;
        serializer << cacheKey;
        serializer << request->url().str();
        serializer << request->requestHeaders();
        if(send(GibbonServerOp_SurfaceDownload, serializer)) {
            DEBUG_REMOTESCREEN_SURFACE("ClientDownload:%p: %s [%s]", this, request->url().c_str(), ResourceManager::prettyCacheKey(cacheKey).toString().c_str());
            shared_ptr<RemoteScreenResource> resource;
            {
                ScopedMutex _lock(mMutex);
                std::map<DataBuffer, shared_ptr<RemoteScreenResource> >::iterator it = mResources.find(cacheKey);
                if(it == mResources.end()) {
                    resource.reset(new RemoteScreenResource);
                    mResources.insert(std::make_pair(cacheKey, resource));
                }
            }
            return resource;
        }
    }
    return shared_ptr<GibbonResource>();
}

void RemoteScreenServer::Connection::onSurfaceDecoded(const Surface::SharedPtr &surface)
{
    if(!isActive() || !surface)
        return;
    DEBUG_REMOTESCREEN_SURFACE("ServerDecoded:%p: %s", this, surface->describe().c_str());

    bool needsController = false;
    const DataBuffer cacheKey = surface->cacheKey();
    {
        ScopedMutex _lock(mMutex);
        std::map<DataBuffer, RemoteSurface*>::iterator it = mSurfaces.find(cacheKey);
        if(it != mSurfaces.end()) {
            RemoteSurface *s = it->second;
            s->surface = surface;
            {
                DEBUG_REMOTESCREEN_SURFACE("ServerDecodeWait:%p: %s", this, surface->describe().c_str());
                while(s->state == RemoteSurface::State_Decoding)
                    s->condition.wait(&mMutex);
                DEBUG_REMOTESCREEN_SURFACE("~ServerDecodeWait:%p: %s", this, surface->describe().c_str());
            }
            std::swap(s->needsController, needsController);
        }
    }
    if(needsController)
        setupController(surface);
}

void RemoteScreenServer::Connection::onSurfaceModified(const Surface::SharedPtr &surface,
                                                       SurfaceCache::Listener::Modification modification)
{
    if(!isActive())
        return;

    bool released = false;
    const DataBuffer cacheKey = surface->cacheKey();
    {
        ScopedMutex _lock(mMutex);
        std::map<DataBuffer, RemoteSurface*>::iterator it = mSurfaces.find(cacheKey);
        if(it != mSurfaces.end()) {
            const RemoteSurface *s = it->second;
            switch(modification) {
            case SurfaceCache::Listener::Modified_Released:
            case SurfaceCache::Listener::Modified_Reused:
                released = true;
                break;
            case SurfaceCache::Listener::Modified_Changed:
                released = s->state == RemoteSurface::State_Generated;
                break;
            }
            if(released) {
                mSurfaces.erase(it);
                delete s;
            }
        }
    }
    if(released) {
        if(surface->getController())
            surface->getController()->removeListener(mListener);

        Serializer serializer;
        serializer << cacheKey;
        DEBUG_REMOTESCREEN_SURFACE("ServerRelease:%p: %s", this, surface->describe().c_str());
        send(GibbonServerOp_SurfaceRelease, serializer);
    }
}

int RemoteScreenServer::Connection::onSurfaceRendered(const Surface::SharedPtr &surface)
{
    if(!surface || !surface->isValid()|| !isActive())
        return 0;

    int id = 0;
    const DataBuffer cacheKey = surface->cacheKey();
    if(surface->displayList()) {
        {
            ScopedMutex _lock(mMutex);
            std::map<DataBuffer, RemoteSurface*>::const_iterator it = mSurfaces.find(cacheKey);
            if(it != mSurfaces.end()) {
                assert(it->second->state == RemoteSurface::State_Generated);
                return it->second->id;
            }
            id = sSurfaceSerial.ref();
            mSurfaces.insert(std::make_pair(cacheKey, new RemoteSurface(RemoteSurface::State_Generated, id)));
        }
        RemoteScreenDisplayListWriter writer(this);
        writer.write(surface->displayList());
        Serializer serializer;
        serializer << cacheKey;
        serializer << id;
        DEBUG_REMOTESCREEN_SURFACE("ServerRender:%p: %s", this, ResourceManager::prettyCacheKey(cacheKey).toString().c_str());
        serializer << surface->getDescription();
        DataBuffer contents = writer.data();
        if(flags() & Flag_CompressGzip) {
            serializer << uint8_t(SurfaceRenderType_GZIP);
            serializer << contents.compress(DataBuffer::Compression_GZip);
        } else {
            serializer << uint8_t(SurfaceRenderType_NONE);
            serializer << contents;
        }
        send(GibbonServerOp_SurfaceRender, serializer);
    } else {
        ScopedMutex _lock(mMutex);
        std::map<DataBuffer, RemoteSurface*>::const_iterator it = mSurfaces.find(cacheKey);
        if(mSurfaces.find(cacheKey) != mSurfaces.end())
            id = it->second->id;
    }
    if(!id)
        Log::error(TRACE_GIBBON_REMOTESCREEN, "Uh-oh:Render:%p:[%s]", this, surface->describe().c_str());
    return id;
}

void RemoteScreenServer::Connection::onScreenShowSplash(bool show)
{
    if(!isActive())
        return;

    Serializer serializer;
    serializer << show;
    send(GibbonServerOp_ScreenShowSplash, serializer);
}

void RemoteScreenServer::Connection::onScreenRendered(const GraphicsEngine *engine, const Rect &rect)
{
    if(!isActive())
        return;

    RemoteScreenDisplayListWriter writer(this);
    writer.write(engine->getDisplayList());

    bool dropFrame = false;
    const uint64_t frameTime = Time::mono().ms();
    {
        ScopedMutex _lock(mStats.mutex);
#ifdef GIBBON_REMOTESCREEN_MAX_SERVER_FRAMES
        if(mStats.pendingFrames > GIBBON_REMOTESCREEN_MAX_SERVER_FRAMES)
            dropFrame = true;
#endif
        ++mStats.renderedFrames;
        if(dropFrame)
            ++mStats.droppedFrames;
        else
            ++mStats.pendingFrames;
        DEBUG_REMOTESCREEN_FRAME("[%lld]:%p:%s(%d/%d/%d)", (ullong)frameTime, this, dropFrame ? "DROPPED" : "SENT",
                                 mStats.renderedFrames, mStats.droppedFrames, mStats.pendingFrames);
    }

    if(!dropFrame) {
        Serializer serializer;
        serializer << frameTime;
        serializer << rect;
        DataBuffer contents = writer.data();
#if !defined(GIBBON_REMOTESCREEN_NO_BSDIFF)
        if(flags() & Flag_CompressBSDiff) {
            serializer << uint8_t(SurfaceRenderType_BSDIFF);
            serializer << contents.length();
            DataBuffer bsdiff_result;
            bsdiff_stream bsdiff_handle = { &bsdiff_result, malloc, free, gibbon_bsdiff_write };
            bsdiff(mDiffReference.constData(), mDiffReference.size(),
                   contents.constData(), contents.size(), &bsdiff_handle);
            bsdiff_result = bsdiff_result.compress(DataBuffer::Compression_GZip);
            serializer << bsdiff_result;
            mDiffReference = contents;
        } else
#endif
            if(flags() & Flag_CompressGzip) {
                serializer << uint8_t(SurfaceRenderType_GZIP);
                serializer << contents.compress(DataBuffer::Compression_GZip);
            } else {
                serializer << uint8_t(SurfaceRenderType_NONE);
                serializer << contents;
            }
        send(GibbonServerOp_ScreenRender, serializer);
    }
}
int RemoteScreenClient::connect_internal(const std::string &host, int display)
{
    struct hostent *server = gethostbyname(host.c_str());
    if (server == NULL) {
        Log::error(TRACE_GIBBON_REMOTESCREEN, "Could not resolve '%s'", host.c_str());
        return -1;
    }

    const int fd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        Log::error(TRACE_GIBBON_REMOTESCREEN, "Could not create socket");
        return -1;
    }

    struct sockaddr_in sockAddrIn;
    const int port = GIBBON_REMOTESCREEN_PORT+display;
    sockAddrIn.sin_family = AF_INET;
    memcpy((char *)&sockAddrIn.sin_addr.s_addr, (char *)server->h_addr, server->h_length);
    sockAddrIn.sin_port = htons(port);

    for(int attempt = 0; true; ++attempt) {
        if(!::connect(fd, (struct sockaddr *)&sockAddrIn, sizeof(sockAddrIn))) {
            Log::warn(TRACE_GIBBON_REMOTESCREEN, "Connected! %s:%d [%d]", host.c_str(), port, attempt);
            break;
        }
        Log::error(TRACE_GIBBON_REMOTESCREEN, "Could not connect %s:%d [%d]", host.c_str(), port, attempt);
        sleep(2);
        if(attempt == 5) {
            ::close(fd);
            return -1;
        }
    }
    return fd;
}

bool RemoteScreenClient::connect(const AseUrl &url)
{
    std::string key = url.GetPath();
    while(key.length() && key[0] == '/')
        key = key.substr(1);
    return connect(url.GetHost(), url.GetPortNumber(), key);
}

bool RemoteScreenClient::connect(const std::string &host, int display, const std::string &key)
{
    mKey = key.empty() ? nrdApp()->getSystem()->getEsn() : key;
    const int socket = connect_internal(host, display);
    if(socket == -1)
        return false;
    openSocket(socket);
    return true;
}

void RemoteScreenClient::startHandshake()
{
    mState = State_HandShake;
    send(StringFormatter::sformat("GIBBON %04d.%02d.%02d %s\n", NRDP_VERSION_MAJOR, NRDP_VERSION_MINOR, NRDP_VERSION_PATCH, mKey.c_str()));
}

void RemoteScreenClient::handleNBPReceive(const Response &response)
{
    if(response.object()) {
        Serializer serializer;
        Variant variant = response.toVariant();
        /* Last chance for FIXUPS! ### HACKS! */
        if(variant.contains("time"))
            variant["time"] = toRemoteTime(variant.mapValue<ullong>("time")).ms();
        if(variant.mapValue<std::string>("object") == "nrdp.player" &&
           variant.mapValue<std::string>("type") == "Event") {
            if(variant.mapValue<std::string>("name") == "updatepts") {
                Variant &data = variant["data"];
                if(data.contains("pts"))
                    data["pts"] = toRemoteTime(data.mapValue<ullong>("pts")).ms();
                if(data.contains("time"))
                    data["time"] = toRemoteTime(data.mapValue<ullong>("time")).ms();

            }
        }
        /* ************************* */
        serializer << variant;
        send(GibbonClientOp_NBPReceive, serializer);
    }
}

void RemoteScreenClient::onSurfaceDownloaded(const shared_ptr<GibbonResource> &resource, Resource::State)
{
    const DataBuffer cacheKey = resource->cacheKey();
    DEBUG_REMOTESCREEN_SURFACE("ClientDownloaded:%p: [%s]", this, cacheKey.toString().c_str());
    {
        Serializer serializer;
        serializer << cacheKey;
        const SurfaceDecoder::SurfaceInfo info = resource->surfaceInfo();
        serializer << info.size;
        serializer << info.flags;
        serializer << (int32_t)resource->nativeErrorCode();
        serializer << (int32_t)resource->httpStatusCode();
        serializer << resource->networkMetrics();
        send(GibbonClientOp_SurfaceDownloaded, serializer);
    }
}

void RemoteScreenClient::onSurfaceDecoded(const Surface::SharedPtr &surface, const DataBuffer &cacheKey)
{
    uint32_t id;
    {
        ScopedMutex _lock(mMutex);
        std::map<DataBuffer, RemoteSurface*>::const_iterator it = mSurfaces.find(cacheKey);
        if(it == mSurfaces.end() || !it->second->loader)
            return;
        id = it->second->id;
    }
    sendSurfaceDecoded(surface, cacheKey, id);
}

void RemoteScreenClient::sendSurfaceDecoded(const Surface::SharedPtr &surface, const DataBuffer &cacheKey, uint32_t id)
{
    DEBUG_REMOTESCREEN_SURFACE("ClientDecoded:%p: %s [%s]", this, surface ? surface->describe().c_str() : "N/A", ResourceManager::prettyCacheKey(cacheKey).toString().c_str());
    {
        Serializer serializer;
        serializer << cacheKey;
        serializer << ((bool)surface);
        serializer << (surface && surface->getController());
        send(GibbonClientOp_SurfaceDecoded, serializer);
    }
    addSurface(surface, cacheKey, id);
}

void RemoteScreenClient::addSurface(const Surface::SharedPtr &surface, const DataBuffer &cacheKey, uint32_t id)
{
    if(surface) {
        surface->upload();
        if(startsWith(surface->getDescription(), "REMOTE_RELEASED:"))
            surface->setDescription("REMOTE:" + surface->getDescription().substr(16));
        else if(!startsWith(surface->getDescription(), "REMOTE:"))
            surface->setDescription(StringFormatter::sformat("REMOTE:%d:", id) + surface->getDescription());
    }

    shared_ptr<RemoteScreenSurfaceLoader> loader;
    {
        ScopedMutex _lock(mMutex);
        std::map<DataBuffer, RemoteSurface*>::iterator it = mSurfaces.find(cacheKey);
        if(it == mSurfaces.end()) {
            RemoteSurface *s = new RemoteSurface(surface, id);
            mSurfaces.insert(std::make_pair(cacheKey, s));
            mSurfacesById.insert(std::make_pair(id, s));
        } else {
            RemoteSurface *s = it->second;
            std::swap(loader, s->loader);
            s->surface = surface;
            s->id = id;
            mSurfacesById.insert(std::make_pair(id, s));
        }
    }
    if(loader)
        gibbonApp()->getSurfaceCache()->abort(loader);
}

bool RemoteScreenClient::processInput()
{
    while(true) {
        if(mState == State_Ready) {
            uint8_t op;
            Deserializer data;
            while(receive(op, data)) {
                switch(op) {
                case GibbonServerOp_AnimationStart: {
                    if(Debug::DebugAnimationFPS) {
                        ScopedMutex _lock(mMutex);
                        if(!mAnimationTicks)
                            mAnimationTicks = new Ticks;
                    }
                    gibbonApp()->startAnimation();
                    break; }
                case GibbonServerOp_AnimationEnd: {
                    if(Debug::DebugAnimationFPS) {
                        Ticks *ticks = 0;
                        {
                            ScopedMutex _lock(mMutex);
                            std::swap(ticks, mAnimationTicks);
                        }
                        if(ticks) {
                            const unsigned int now = Time::mono().ms();
                            const unsigned duration = now - ticks->start;
                            if(duration) {
                                int rendered = 0;
                                const unsigned frames = ticks->frames.size()-1;
                                Log::stream stream(Debug::DebugAnimationFPS ? Log::warn(TRACE_GIBBON_REMOTESCREEN) : Log::info(TRACE_GIBBON_REMOTESCREEN));
                                if(frames) {
                                    const float fps = 1000.0f/(duration/frames);
                                    stream << "RemoteAnimation" << "(" << frames << "@" << duration << "::" << fps << "fps):";
                                    unsigned last_localTime = ticks->frames.begin()->localTime;
                                    for(std::list<Ticks::Frame>::const_iterator it = ++ticks->frames.begin(); it != ticks->frames.end(); ++it) {
                                        if(!it->dropped)
                                            ++rendered;
                                        stream << (it->localTime - last_localTime) << "[" << (it->localTime-toLocalTime(it->remoteTime)) << "]" << (it->dropped ? "*" : "") << " ";
                                        last_localTime = it->localTime;
                                    }
                                    stream.flush();
                                }
                                if(rendered != static_cast<int>(frames)) {
                                    const float fps = 1000.0f/(duration/rendered);
                                    stream << "RemoteAnimationRendered" << "(" << rendered << "@" << duration << "::" << fps << "fps):";
                                    unsigned last_localTime = ticks->frames.begin()->localTime;
                                    for(std::list<Ticks::Frame>::const_iterator it = ++ticks->frames.begin(); it != ticks->frames.end(); ++it) {
                                        if(!it->dropped)
                                            stream << (it->localTime - last_localTime) << "[" << (it->localTime-toLocalTime(it->remoteTime)) << "]" << " ";
                                        last_localTime = it->localTime;
                                    }
                                    stream.flush();
                                }
                            }
                            delete ticks;
                        }
                    }
                    gibbonApp()->endAnimation();
                    break; }
                case GibbonServerOp_NBPSend: {
                    Variant variant;
                    data >> variant;
                    Request request(variant, Request::FromTrustedSource);
                    nrdApp()->post(request);
                    break; }
                case GibbonServerOp_SurfaceDownload: {
                    SurfaceLoader::Source::SharedPtr source(new SurfaceLoader::Source);
                    {
                        DataBuffer cacheKey;
                        data >> cacheKey;
                        source->setCacheKey(cacheKey);
                    }
                    {
                        std::string url;
                        data >> url;
                        source->setUrl(url);
                    }
                    {
                        HttpHeaders headers;
                        data >> headers;
                        source->setHeaders(headers);
                    }
                    DEBUG_REMOTESCREEN_SURFACE("ClientDownload:%p: %s [%s]", this, source->url().c_str(), ResourceManager::prettyCacheKey(source->cacheKey()).toString().c_str());
                    shared_ptr<RemoteScreenClient> that = static_pointer_cast<RemoteScreenClient>(shared_from_this());
                    shared_ptr<RemoteScreenResourceRequest> request(new RemoteScreenResourceRequest(that, source));
                    nrdApp()->resourceManager()->sendRequest(request);
                    break; }
                case GibbonServerOp_SurfaceDecode: {
                    DataBuffer cacheKey;
                    data >> cacheKey;
                    uint32_t id;
                    data >> id;
#if defined(GIBBON_REMOTESCREEN_CLIENT_NO_GRAPHICS) && 0
                    sendSurfaceDecoded(Surface::SharedPtr(), cacheKey, id);
#else
                    SurfaceCache::SharedPtr surfaceCache = gibbonApp()->getSurfaceCache();
                    if(Surface::SharedPtr surface = surfaceCache->find(cacheKey)) {
                        sendSurfaceDecoded(surface, cacheKey, id);
                    } else {
                        shared_ptr<RemoteScreenSurfaceLoader> loader;
                        {
                            ScopedMutex _lock(mMutex);
                            std::map<DataBuffer, RemoteSurface*>::iterator it = mSurfaces.find(cacheKey);
                            if(it == mSurfaces.end()) {
                                uint8_t type;
                                data >> type;

                                SurfaceLoader::Source::SharedPtr source(new SurfaceLoader::Source);
                                if(type == SurfaceDecodeType_Data) {
                                    DataBuffer imageData;
                                    data >> imageData;
                                    source->setData(imageData);
                                    DEBUG_REMOTESCREEN_SURFACE("ClientDecode:%p: %s (DATA:%d)", this, ResourceManager::prettyCacheKey(cacheKey).toString().c_str(), imageData.size());
                                } else if(type == SurfaceDecodeType_URL) {
                                    std::string url;
                                    data >> url;
                                    DEBUG_REMOTESCREEN_SURFACE("ClientDecode:%p: %s (URL:%s)", this, ResourceManager::prettyCacheKey(cacheKey).toString().c_str(), url.c_str());
                                    source->setUrl(AseUrl(url));
                                    HttpHeaders headers;
                                    data >> headers;
                                    source->setHeaders(headers);
                                }
                                cacheKey.detach();
                                source->setCacheKey(cacheKey);
                                loader.reset(new RemoteScreenSurfaceLoader(static_pointer_cast<RemoteScreenClient>(shared_from_this()), source));
                                RemoteSurface *s = new RemoteSurface(loader, id);
                                mSurfaces.insert(std::make_pair(cacheKey, s));
                                mSurfacesById[id] = s;
                            } else if(!it->second->loader) {
                                sendSurfaceDecoded(it->second->surface, cacheKey, id);
                            }
                        }
                        if(loader)
                            surfaceCache->load(loader);
                    }
#endif
                    break; }
                case GibbonServerOp_SurfaceRelease: {
                    DataBuffer cacheKey;
                    data >> cacheKey;

                    shared_ptr<RemoteScreenSurfaceLoader> loader;
                    {
                        ScopedMutex _lock(mMutex);
                        std::map<DataBuffer, RemoteSurface*>::iterator it = mSurfaces.find(cacheKey);
                        if(it != mSurfaces.end()) {
                            RemoteSurface *s = it->second;
                            if(Surface::SharedPtr surface = s->surface) {
                                assert(startsWith(surface->getDescription(), "REMOTE:"));
                                surface->setDescription("REMOTE_RELEASED:" + surface->getDescription().substr(7));
                            }
                            loader = s->loader;
                            mSurfacesById.erase(s->id);
                            mSurfaces.erase(cacheKey);
                            delete s;
                        }
                    }

                    SurfaceCache::SharedPtr surfaceCache = gibbonApp()->getSurfaceCache();
                    // if(remoteSurface.surface)
                    //     surfaceCache->remove(remoteSurface.surface);
                    if(loader)
                        surfaceCache->abort(loader);
                    break; }
                case GibbonServerOp_SurfaceStarted: {
                    DataBuffer cacheKey;
                    data >> cacheKey;
                    bool started;
                    data >> started;

                    Surface::SharedPtr surface;
                    {
                        ScopedMutex _lock(mMutex);
                        std::map<DataBuffer, RemoteSurface*>::iterator it = mSurfaces.find(cacheKey);
                        assert(it != mSurfaces.end());
                        if(it != mSurfaces.end())
                            surface = it->second->surface;
                    }
                    if(surface) {
                        DEBUG_REMOTESCREEN_SURFACE("Client%s:%p: %s [%s]", started ? "Started" : "Stopped", this,
                                                   surface->describe().c_str(), ResourceManager::prettyCacheKey(cacheKey).c_str());
                        if(started) {
                            if(surface->getController())
                                surface->getController()->addListener(mListener);
                            surface->start();
                        } else {
                            if(surface->getController())
                                surface->getController()->removeListener(mListener);
                            surface->stop();
                        }
                    }
                    break; }
                case GibbonServerOp_SurfaceRender: {
#if !defined(GIBBON_REMOTESCREEN_CLIENT_NO_GRAPHICS)
                    DataBuffer cacheKey;
                    data >> cacheKey;
                    uint32_t id;
                    data >> id;

                    std::string description;
                    data >> description;
                    uint8_t type;
                    data >> type;

                    DataBuffer contents;
                    if(type == SurfaceRenderType_BSDIFF) {
                        assert(false);
                    } else if(type == SurfaceRenderType_GZIP) {
                        data >> contents;
                        contents = contents.uncompress(DataBuffer::Compression_GZip);
                    } else {
                        data >> contents;
                    }

                    RemoteScreenDisplayListReader reader(this);
                    if(DisplayList::SharedPtr displayList = reader.read(contents)) {
                        ScopedMutex _lock(mMutex);
                        Surface::SharedPtr surface = gibbonApp()->getSurfaceCache()->find(cacheKey);
                        if(!surface) {
                            surface = gibbonApp()->getSurfaceCache()->requestSurface(displayList->getWidth(), displayList->getHeight(),
                                                                                     Surface::Flags_Alpha|Surface::Flags_RenderTarget);
                            gibbonApp()->getSurfaceCache()->setCacheKey(surface, cacheKey);
                        }
                        surface->setDescription(description);
                        surface->setDisplayList(displayList);
                        addSurface(surface, cacheKey, id);
                    }
#endif
                    break; }
                case GibbonServerOp_ScreenShowSplash: {
                    bool show;
                    data >> show;
                    gibbonApp()->getScreen()->setShowSplashScreen(show);
                    break; }
                case GibbonServerOp_ScreenRender: {
                    uint64_t frameTime;
                    data >> frameTime;
                    Rect rect;
                    data >> rect;

                    {
                        ScopedMutex _lock(mStats.mutex);
                        ++mStats.pendingFrames;
                        DEBUG_REMOTESCREEN_FRAME("[%lld]:RECEIVED(%d)", (ullong)frameTime, mStats.pendingFrames);
                    }

#if defined(GIBBON_REMOTESCREEN_CLIENT_NO_GRAPHICS)
                    onScreenRendered(frameTime, true);
#else
                    uint8_t type;
                    data >> type;

                    DataBuffer contents;
                    if(type == SurfaceRenderType_BSDIFF) {
#if !defined(GIBBON_REMOTESCREEN_NO_BSDIFF)
                        uint32_t resultLength;
                        data >> resultLength;
                        contents.resize(resultLength);
                        DataBuffer patch;
                        data >> patch;
                        patch = patch.uncompress(DataBuffer::Compression_GZip);
                        bspatch_stream bspatch_handle = { &patch, gibbon_bsdiff_read };
                        bspatch(mDiffReference.constData(), mDiffReference.size(),
                                contents.data(), contents.size(), &bspatch_handle);
                        mDiffReference = contents;
#else
                        assert(false);
#endif
                    } else if(type == SurfaceRenderType_GZIP) {
                        data >> contents;
                        contents = contents.uncompress(DataBuffer::Compression_GZip);
                    } else {
                        data >> contents;
                    }

                    if(GibbonApplication::instance()->getScreen()->isRunning()) {
                        RemoteScreenDisplayListReader reader(this);
                        if(DisplayList::SharedPtr displayList = reader.read(contents)) {
                            if(displayList->hasCommands()) {
                                displayList->appendCommand(new RemoteScreenDisplayListRenderCommand(static_pointer_cast<RemoteScreenClient>(shared_from_this()), frameTime));
                                GibbonApplication::instance()->getScreen()->setRootDisplayList(displayList, rect);
                            }
                        }
                    }
#endif
                    break; }
                default:
                    Log::error(TRACE_GIBBON_REMOTESCREEN, "Unhandled: %d", op);
                    break;
                }
            }
            break;
        } else if(mState == State_HandShake) {
            const DataBuffer response = receiveLine();
            if(response == "CONNECTED\n") {
                mState = State_Init;
                Log::warn(TRACE_GIBBON_REMOTESCREEN, "Accepted!");
            } else if(response.startsWith("RECONNECT ")) {
                const AseUrl url(response.mid(10, response.length()-11).toString());
                assert(url.GetScheme() == "gibbon");
                Log::warn(TRACE_GIBBON_REMOTESCREEN, "Redirected! (%s)", url.str().c_str());
                const int socket = connect_internal(url.GetHost(), url.GetPortNumber());
                if(socket == -1)
                    return false;
                openSocket(socket);
                startHandshake();
            }
        } else if(mState == State_Init) {
            uint8_t op;
            Deserializer data;
            if(!receive(op, data))
                break;
            shared_ptr<RemoteScreenClient> that = static_pointer_cast<RemoteScreenClient>(shared_from_this());
            assert(op == GibbonServerOp_Init);
            uint8_t flags;
            data >> flags;
            uint64_t baseTime;
            data >> baseTime;
            uint32_t pid;
            data >> pid;
            uint16_t major, minor, patch;
            data >> major >> minor >> patch;
            uint16_t width, height;
            data >> width;
            data >> height;
            Log::warn(TRACE_GIBBON_REMOTESCREEN, " Screen:%p:%d: %dx%d [0x%x] (%04d.%02d.%02d)", this, pid, width, height, flags, major, minor, patch);

            Serializer serializer;
            uint8_t outFlags = this->flags();
#ifndef GIBBON_REMOTESCREEN_NO_NBP
            if(flags & Flag_DPI)
                outFlags |= Flag_DPI;
#endif
            if(!(flags & Flag_CompressGzip))
                outFlags &= ~Flag_CompressGzip;
            if(!(flags & Flag_CompressBSDiff))
                outFlags &= ~Flag_CompressBSDiff;
            setFlags(outFlags);
            serializer << outFlags;
            serializer << uint64_t(updateBaseTime(baseTime).ms());
            serializer << uint32_t(getpid());
            serializer << nrdApp()->isSuspended();
            uint32_t surfaceCacheCapacity = gibbonApp()->getSurfaceCache()->capacity();
            surfaceCacheCapacity -=  Surface::estimateSize(GibbonConfiguration::fontGlyphCacheWidth(),
                                                           GibbonConfiguration::fontGlyphCacheHeight(),
                                                           Surface::Flags_Alpha | Surface::Flags_Text);
            surfaceCacheCapacity -= (int)(surfaceCacheCapacity * 0.10f);
            serializer << surfaceCacheCapacity;
            serializer << uint16_t(gibbonApp()->getScreen()->getTargetFPS());
            serializer << uint16_t(GibbonConfiguration::screenWidth());
            serializer << uint16_t(GibbonConfiguration::screenHeight());
            send(GibbonClientOp_Init, serializer);
            mListener.reset(new RemoteScreenClientGraphicsListener(that));
            mState = State_Ready;
            {
                mNBPEventConnection.reset(new RemoteScreenNBPEventConnection(that));
                nrdApp()->addEventConnection(mNBPEventConnection);
            }
        } else if(mState == State_Disconnect) {
            return false;
        }
    }
    return true;
}

void RemoteScreenClient::dumpSurfaces() const
{
    ScopedMutex _lock(mMutex);
    Log::error(TRACE_GIBBON_REMOTESCREEN, "---------- RemoteScreenClient: %p ------ \n", this);
    for(std::map<DataBuffer, RemoteSurface*>::const_iterator it = mSurfaces.begin(); it != mSurfaces.end(); ++it)
        Log::error(TRACE_GIBBON_REMOTESCREEN, "Surface: (%s%s) [%s]", it->first.toString().c_str(), it->second->loader ? "*" : "",
                   it->second->surface ? it->second->surface->describe().c_str() : "N/A");
}

Surface::SharedPtr RemoteScreenClient::findSurface(int id)
{
#if defined(GIBBON_REMOTESCREEN_CLIENT_NO_GRAPHICS)
    NRDP_UNUSED(id);
    return Surface::SharedPtr();
#else
    if(!id)
        return Surface::SharedPtr();

    shared_ptr<RemoteScreenSurfaceLoader> loader;
    {
        ScopedMutex _lock(mMutex);
        std::map<uint32_t, RemoteSurface*>::const_iterator it = mSurfacesById.find(id);
        assert(it != mSurfacesById.end());
        RemoteSurface *s = it->second;
        loader = s->loader;
        if(!loader) {
            if(!s->surface)
                Log::error(TRACE_GIBBON_REMOTESCREEN, "Failure to locate surface: %d", id);
            return s->surface;
        }
    }
    Surface::SharedPtr surface = loader->wait();
    assert(surface);
    return surface;
#endif
}

void RemoteScreenClient::disconnect()
{
    GibbonApplication::instance()->getScreen()->setRootDisplayList(DisplayList::SharedPtr());

    std::map<DataBuffer, RemoteSurface*> surfaces;
    {
        ScopedMutex _lock(mMutex);
        mState = State_Disconnect;
        delete mAnimationTicks;
        mAnimationTicks = 0;
        std::swap(mSurfaces, surfaces);
        mSurfacesById.clear();
    }
    for(std::map<DataBuffer, RemoteSurface*>::iterator it = surfaces.begin(); it != surfaces.end(); ++it) {
        RemoteSurface *s = it->second;
        if(s->loader)
            gibbonApp()->getSurfaceCache()->abort(s->loader);
        delete s;
    }
    quit();
}


void RemoteScreenClient::handleSuspendChanged(bool suspend, const Variant &reason)
{
    if(!isActive())
        return;

    Serializer serializer;
    serializer << suspend;
    serializer << reason;
    send(GibbonClientOp_SuspendChanged, serializer);
}

void RemoteScreenClient::handleEvent(GibbonEvent *event)
{
    if(!isActive())
        return;

    const GibbonEvent::Type type = static_cast<GibbonEvent::Type>(event->type());
    Serializer serializer;
    serializer << uint8_t(type);
    switch (type) {
    case GibbonEvent::Type_KeyPress:
    case GibbonEvent::Type_KeyRelease:
    case GibbonEvent::Type_KeyClick: {
        const KeyEvent *keyEvent = static_cast<const KeyEvent*>(event);
        if(type == GibbonEvent::Type_KeyPress && keyEvent->mRepeat) {
            if(Time::mono()-mLastRepeatPress < Time::fromMS(80))
                break;
            mLastRepeatPress = Time::mono();
        } else {
            mLastRepeatPress = Time::fromMS(0);
        }

        serializer << keyEvent->mModifiers;
        serializer << keyEvent->mKey;
        serializer << keyEvent->mText;
        serializer << keyEvent->mUIKey;
        serializer << keyEvent->mCount;
        serializer << keyEvent->mRepeat;
        send(GibbonClientOp_Event, serializer);
        break; }
    case GibbonEvent::Type_MouseChange: {
        const MouseChangeEvent *mouseEvent = static_cast<const MouseChangeEvent*>(event);
        serializer << mouseEvent->mVisible;
        send(GibbonClientOp_Event, serializer);
        break; }
    case GibbonEvent::Type_MouseMove:
    case GibbonEvent::Type_MousePress:
    case GibbonEvent::Type_MouseRelease: {
        const MouseEvent *mouseEvent = static_cast<const MouseEvent*>(event);
        serializer << mouseEvent->mModifiers;
        serializer << mouseEvent->mPos;
        serializer << mouseEvent->mButton;
        serializer << mouseEvent->mButtons;
        send(GibbonClientOp_Event, serializer);
        break; }
    case GibbonEvent::Type_FocusChange: {
        const FocusChangeEvent *focusEvent = static_cast<const FocusChangeEvent*>(event);
        serializer << focusEvent->mFocus;
        send(GibbonClientOp_Event, serializer);
        break; }
    default:
        assert(false);
        break;
    }
}

void RemoteScreenClient::onScreenRendered(uint64_t frameTime, bool dropped)
{
    {
        ScopedMutex _lock(mStats.mutex);
        if(mAnimationTicks)
            mAnimationTicks->frames.push_back(Ticks::Frame(frameTime, dropped));
        --mStats.pendingFrames;
        ++mStats.renderedFrames;
        if(dropped)
            ++mStats.droppedFrames;
        assert(mStats.pendingFrames >= 0);
        DEBUG_REMOTESCREEN_FRAME("[%lld]:%s(%d/%d/%d)", (ullong)frameTime, dropped ? "DROPPED" : "RENDERED",
                                 mStats.renderedFrames, mStats.droppedFrames, mStats.pendingFrames);
    }
    {
        Serializer serializer;
        serializer << frameTime;
        serializer << dropped;
        send(GibbonClientOp_ScreenRendered, serializer);
    }
}

void RemoteScreenClient::finished()
{
    if(mState != State_Disconnect)
        gibbonApp()->quit();
}

void RemoteScreenClient::serverShutdown() const
{
    send(GibbonClientOp_Quit);
}

void RemoteScreenClient::serverEval(const std::string &command) const
{
    Serializer serializer;
    serializer << command;
    send(GibbonClientOp_Eval, serializer);
}


