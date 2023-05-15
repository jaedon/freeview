/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef __REMOTESCREEN_H__
#define __REMOTESCREEN_H__

#include <GibbonEvent.h>
#include <SurfaceCache.h>
#include <GraphicsEngine.h>

#include <nrdbase/Log.h>
#include <nrdbase/Endian.h>
#include <nrdbase/Thread.h>

#include <errno.h>
#include <sys/socket.h>

DECLARE_TRACEAREA(GIBBON_REMOTESCREEN);

#if defined(BUILD_DEBUG)
# define GIBBON_REMOTESCREEN_ACCOUNTING
#endif
#if 0
# define DEBUG_REMOTESCREEN_DATASEND 5
#endif
#if 0
# define DEBUG_REMOTESCREEN_DATARECV 5
#endif

#if !defined(GIBBON_HAVE_BSDIFF) || 1
# define GIBBON_REMOTESCREEN_NO_BSDIFF
#endif
#if 1
# define GIBBON_REMOTESCREEN_NO_COMPRESS
#endif

namespace netflix {
namespace gibbon {

class RemoteScreenLatencyTimer;
class RemoteScreenSurfaceLoader;
class RemoteScreenResource;
class RemoteScreenDisplayListWriter;
class RemoteScreenDisplayListReader;
class RemoteScreenNBPEventConnection;
class RemoteScreenServerGraphicsListener;
class RemoteScreenClientGraphicsListener;

class RemoteScreenConnection : public Thread, public enable_shared_from_this<RemoteScreenConnection>
{
public:
    enum {
        Flag_CPULittleEndian = 0x01,
        Flag_GPULittleEndian = 0x02,
        Flag_CompressGzip    = 0x04,
        Flag_CompressBSDiff  = 0x08,
        Flag_DPI             = 0x10
    };

    virtual void dump() const;

    RemoteScreenConnection();
    virtual ~RemoteScreenConnection();

    void openSocket(int socket);
    void closeSocket();
    inline int socket() const {
        ScopedMutex _lock(mSocketMutex);
        return mSocket;
    }

    inline uint8_t flags() const { return mFlags; }
    inline void setFlags(uint8_t flags) { mFlags = flags; }

    virtual void Run();

    inline int latency() const {
        ScopedMutex _lock(mStats.mutex);
        return mStats.latency;
    }

    void ping(bool dump);

    void quit();
    void wakeup() const;
    virtual bool isActive() const { return isRunning(); }
    inline bool isRunning() const {
        ScopedMutex _lock(mQuitMutex);
        return mRunning;
    }

    inline bool send(const unsigned char *data, int length) const {
        return mOutput.append(data, length);
    }
    inline bool send(const std::string &str) const { return send((const unsigned char *)str.c_str(), str.length()); }
    inline bool send(uint8_t op, const Serializer &serializer=Serializer()) const {
        ScopedMutex _lock(mOutput.lock());
        const DataBuffer data = serializer.data();
#if defined(DEBUG_REMOTESCREEN_DATASEND) && DEBUG_REMOTESCREEN_DATASEND >= 2
        Log::error(TRACE_GIBBON_REMOTESCREEN, "%p: Sending op %d [%d]", this, op, data.length() + sizeof(op));
#endif
        const uint32_t length = htonl(data.length() + sizeof(op));
        if(!send((unsigned char *)&length, sizeof(length)))
            return false;
        if(!send(&op, sizeof(op)))
            return false;
        if(data.length() && !send(data.constData(), data.length()))
            return false;
        return true;
    }

    inline Time updateBaseTime(uint64_t remoteTime) {
        const Time now = Time::mono();
        mRemoteBaseTime = remoteTime;
        mLocalBaseTime = now.ms();
        return now;
    }
    inline Time toLocalTime(uint64_t remoteTime) const {
        return Time::fromMS(mLocalBaseTime+(remoteTime-mRemoteBaseTime));
    }
    inline Time toRemoteTime(uint64_t localTime) const {
        return Time::fromMS(mRemoteBaseTime+(localTime-mLocalBaseTime));
    }

protected:
    struct Stats {
        inline Stats() : mutex(GIBBON_REMOTESCREEN_STATS_MUTEX, "RemoteScreenConnection::Stats"), lastPong(Time::mono()),
                         dumpLatency(false), latency(0), renderedFrames(0), droppedFrames(0), pendingFrames(0) {  }
        mutable Mutex mutex;
        Time lastDump, lastPong;
        bool dumpLatency;
        int latency;
        int renderedFrames, droppedFrames, pendingFrames;
#if defined(GIBBON_REMOTESCREEN_ACCOUNTING)
        std::list<std::pair<Time, int> > received, sent;
#endif
    };
    mutable Stats mStats;
    uint64_t mLocalBaseTime, mRemoteBaseTime;

    void checkLatency();
    void updateLatency(uint64_t time);

    inline bool receive(uint8_t &op, Deserializer &data);
    inline DataBuffer receiveLine() {
        DataBuffer line;
        while(!mInput.deserializeLine(line)) {
            if(!waitForData())
                break;
        }
        return line;
    }

    virtual bool processInput() { return false; }
    virtual void started() { }
    virtual void finished() { }

private:
    void initFlags();

    inline bool receive_internal(uint8_t &op, Deserializer &data) {
        uint32_t length;
        if(mInput.length() >= sizeof(length)) {
            mInput.deserialize((uint8_t*)&length, sizeof(length));
            length = ntohl(length);
            receiveData(length);
            if(mInput.length() >= length) {
                data = Deserializer(mInput, length);
                data >> op;
#if defined(DEBUG_REMOTESCREEN_DATARECV) && DEBUG_REMOTESCREEN_DATARECV >= 1
                Log::error(TRACE_GIBBON_REMOTESCREEN, "%p: Received op %d [%d]", this, op, length);
#endif
                return true;
            }
        }
        return false;
    }
    inline int sendData(const unsigned char *data, int length) const {
        const int fd = socket();
        if(fd == -1)
            return -1;

#if defined(DEBUG_REMOTESCREEN_DATASEND) && DEBUG_REMOTESCREEN_DATASEND >= 3
        Log::error(TRACE_GIBBON_REMOTESCREEN, "%p: Sending Data %p (%d)", this, data, length);
#endif
        int total_sent = 0;
        while(length != total_sent) {
            const ssize_t sent = ::send(fd, data+total_sent, length-total_sent, 0);
#if defined(DEBUG_REMOTESCREEN_DATASEND) && DEBUG_REMOTESCREEN_DATASEND >= 4
            Log::error(TRACE_GIBBON_REMOTESCREEN, "  + Sent (%d)", sent);
#endif
#if defined(DEBUG_REMOTESCREEN_DATASEND) && DEBUG_REMOTESCREEN_DATASEND >= 5
            if(sent > 0)
                Log::error(TRACE_GIBBON_REMOTESCREEN, "%s", Log::hexDump(data+total_sent, sent).c_str());
#endif
            if(sent <= 0) {
                if(sent == -1 && errno == EWOULDBLOCK)
                    break;
                return -1;
            }
            total_sent += sent;
        }
#if defined(GIBBON_REMOTESCREEN_ACCOUNTING)
        if(total_sent) {
            const Time now = Time::mono();
            ScopedMutex _lock(mStats.mutex);
            mStats.sent.push_back(std::make_pair(now, total_sent));
            if(now-mStats.lastDump >= Time::fromSeconds(30))
                dump();
        }
#endif
#if defined(DEBUG_REMOTESCREEN_DATASEND) && DEBUG_REMOTESCREEN_DATASEND >= 3
        Log::error(TRACE_GIBBON_REMOTESCREEN, "%p: Done Sending %d", this, total_sent);
#endif
        return total_sent;
    }
    inline bool receiveData(uint32_t minLength) {
        bool received = false;
        while(mInput.length() < minLength) {
            if(!waitForData())
                break;
            received = true;
        }
        return received;
    }
    bool waitForData();
    inline void trimInput() {
        if(mInput.offset() >= 8096)
            mInput.trim();
    }

    mutable Mutex mSocketMutex;
    Deserializer mInput;
    bool mConnected;
    int mSocket;
    class RemoteScreenConnectionWriter : public Thread
    {
    public:
        RemoteScreenConnectionWriter();
        ~RemoteScreenConnectionWriter();

        void init(RemoteScreenConnection *connection);

        virtual void Run();

        void quit();
        void wakeup() const;

        inline Mutex &lock() const { return mMutex; }
        inline bool append(const unsigned char *data, int length) {
            assert(data && length);
            ScopedMutex _lock(lock());
#if defined(DEBUG_REMOTESCREEN_DATASEND) && DEBUG_REMOTESCREEN_DATASEND >= 1
            Log::error(TRACE_GIBBON_REMOTESCREEN, "%p: Putting into backlog %d", mConnection, length);
#endif
#if defined(DEBUG_REMOTESCREEN_DATASEND) && DEBUG_REMOTESCREEN_DATASEND >= 5
            Log::error(TRACE_GIBBON_REMOTESCREEN, "%s", Log::hexDump(data, length).c_str());
#endif
            mData.append(data, length);
            wakeup();
            return true;
        }

    private:
        inline bool sendPendingData() {
            DataBuffer data;
            {
                ScopedMutex _lock(lock());
                const int length = mData.size()-mOffset;
                if(!length)
                    return true;
                data = mData.slice(mOffset, length);
            }

#if defined(DEBUG_REMOTESCREEN_DATASEND) && DEBUG_REMOTESCREEN_DATASEND >= 1
            Log::error(TRACE_GIBBON_REMOTESCREEN, "%p: Sending Backlog (%d)", mConnection, data.length());
#endif
            const int sent = mConnection->sendData(data.constData(), data.length());
            if(sent == -1)
                return false;
            {
                ScopedMutex _lock(lock());
                mOffset += sent;
                if(mOffset >= 8096) {
                    mData.remove(0, mOffset);
                    mOffset = 0;
                }
            }
            return true;
        }

        mutable Mutex mMutex;
        int mPipe[2];
        bool mQuit;

        int mOffset;
        DataBuffer mData;
        RemoteScreenConnection *mConnection;
    };
    mutable RemoteScreenConnectionWriter mOutput;

    mutable Mutex mQuitMutex;
    int mPipe[2];
    bool mQuit;

    bool mRunning;
    uint8_t mFlags;

    friend class RemoteScreenLatencyTimer;
    friend class RemoteScreenConnectionWriter;
};

class RemoteScreenServer : public Thread
{
public:
    class Connection : public RemoteScreenConnection
    {
    public:
        inline Connection(RemoteScreenServer *server);
        virtual ~Connection();

        void dumpSurfaces() const;
        virtual void dump() const;

        virtual bool isActive() const { return RemoteScreenConnection::isActive() && state() == State_Ready; }
        inline bool isSuspended() const { return mSuspended; }

    protected:
        void onScreenShowSplash(bool show);
        void onScreenRendered(const GraphicsEngine *engine, const Rect &rect);
        void onSurfaceStarted(const Surface::SharedPtr &surface, bool show);
        void onSurfaceDecoded(const Surface::SharedPtr &surface);
        void onSurfaceDecode(const SurfaceLoader::Source::SharedPtr &source, const shared_ptr<GibbonResource> &resource);
        void onSurfaceModified(const Surface::SharedPtr &surface, SurfaceCache::Listener::Modification modification);
        shared_ptr<GibbonResource> onSurfaceDownload(const ResourceRequest::SharedPtr &request);
        int onSurfaceRendered(const Surface::SharedPtr &surface);

        virtual bool processInput();
        virtual void finished();

    private:
        void setupController(const Surface::SharedPtr &surface);

        RemoteScreenServer *mServer;
        mutable Mutex mMutex;

        enum State {
            State_HandShake,
            State_Init,
            State_Ready
        };
        State mState;
        inline State state() const {
            ScopedMutex _lock(mMutex);
            return mState;
        }

        int mPid;
        bool mSuspended;
        std::string mESN;
        int mVerMajor, mVerMinor, mVerPatch;

        class RemoteSurface {
        public:
            enum State { State_Decoding, State_Decoded, State_Aborted, State_Generated };
            inline RemoteSurface(State _state, uint32_t _id) : needsController(false), state(_state), id(_id) { }
            inline RemoteSurface(const RemoteSurface &other) : needsController(other.needsController), state(other.state), id(other.id) { }
            ConditionVariable condition;
            Surface::WeakPtr surface;
            bool needsController;
            State state;
            uint32_t id;
        };
        std::map<DataBuffer, RemoteSurface*> mSurfaces;
        std::map<DataBuffer, shared_ptr<RemoteScreenResource> > mResources;

#if !defined(GIBBON_REMOTESCREEN_NO_BSDIFF)
        DataBuffer mDiffReference;
#endif
        shared_ptr<RemoteScreenServerGraphicsListener> mListener;

        friend class RemoteScreenServerGraphicsListener;
        friend class RemoteScreenDisplayListWriter;
        friend class RemoteScreenServer;
    };

    RemoteScreenServer(int display=0);
    ~RemoteScreenServer();

    inline void shutdown() { quit(); }

    void dump() const;
    void dumpSurfaces() const;
    void ping(bool dump) const;

    inline bool isLocalUrl(const AseUrl &url) const {
        return url.GetScheme() == "data" || url.GetScheme() == "file" || GibbonResourceManager::isNBP(url);
    }

    void startAnimation();
    void endAnimation();

    bool handleNBPSend(const Request &request);
    void handleSuspend(const Variant &reason=Variant());
    inline void handleIdleSuspend() { handleSuspend("serverIdle"); }
    shared_ptr<GibbonResource> handleSurfaceDownload(const ResourceRequest::SharedPtr &request);

    virtual void Run();

    void quit();
    void wakeup();

private:
    bool createListeningSocket();
    void handleConnections();

    int mPort;
    int mListeningSocket;
    std::set<shared_ptr<Connection> > mConnections;

    int mPipe[2];
    bool mQuit;
};

class RemoteScreenClient : public RemoteScreenConnection
{
public:
    inline RemoteScreenClient() : RemoteScreenConnection(), mMutex(UNTRACKED_MUTEX, "RemoteScreenClient"), mState(State_HandShake), mAnimationTicks(0) { }
    virtual ~RemoteScreenClient() { disconnect(); }

    virtual bool isActive() const { return RemoteScreenConnection::isActive() && mState == State_Ready; }

    bool connect(const AseUrl &url);
    bool connect(const std::string &host, int display=0, const std::string &key=std::string());
    void disconnect();

    void serverShutdown() const;
    void serverEval(const std::string &command) const;

    void dumpSurfaces() const;
    Surface::SharedPtr findSurface(int id);

    void handleEvent(GibbonEvent *event);
    void handleSuspendChanged(bool suspend, const Variant &reason);

    void onSurfaceDownloaded(const shared_ptr<GibbonResource> &resource, Resource::State state);

    void onSurfaceDecoded(const Surface::SharedPtr &surface, const DataBuffer &cacheKey);

    void onScreenRendered(uint64_t remoteTime, bool dropped);

    void handleNBPReceive(const Response &response);

protected:

    virtual void started() { startHandshake(); }
    virtual void finished();
    virtual bool processInput();

private:
    void startHandshake();
    void sendSurfaceDecoded(const Surface::SharedPtr &surface, const DataBuffer &cacheKey, uint32_t id);
    void addSurface(const Surface::SharedPtr &surface, const DataBuffer &cacheKey, uint32_t id);
    int connect_internal(const std::string &host, int display);

    mutable Mutex mMutex;

    enum State {
        State_Disconnect,
        State_HandShake,
        State_Init,
        State_Ready
    };
    State mState;

    struct Ticks {
        inline Ticks() : start(Time::mono().ms()) { }
        struct Frame {
            Frame(uint64_t _remoteTime, bool _dropped) : localTime(Time::mono().ms()), remoteTime(_remoteTime), dropped(_dropped) { }
            uint64_t localTime, remoteTime;
            bool dropped;
        };
        std::list<Frame> frames;
        unsigned int start;
    } *mAnimationTicks;

    class RemoteSurface {
    public:
        inline RemoteSurface(const shared_ptr<RemoteScreenSurfaceLoader> &_loader, uint32_t _id) : loader(_loader), id(_id) { }
        inline RemoteSurface(const Surface::SharedPtr &_surface, uint32_t _id) : surface(_surface), id(_id) { }
        shared_ptr<RemoteScreenSurfaceLoader> loader;
        Surface::SharedPtr surface;
        uint32_t id;
    };
    std::map<DataBuffer, RemoteSurface*> mSurfaces;
    std::map<uint32_t, RemoteSurface*> mSurfacesById;

    shared_ptr<RemoteScreenNBPEventConnection> mNBPEventConnection;
    shared_ptr<RemoteScreenClientGraphicsListener> mListener;

#if !defined(GIBBON_REMOTESCREEN_NO_BSDIFF)
    DataBuffer mDiffReference;
#endif
    Time mLastRepeatPress;
    std::string mKey;

    friend class RemoteScreenSurfaceLoader;
    friend class RemoteScreenDisplayListReader;
    friend class RemoteScreenNBPEventConnection;
    friend class RemoteScreenClientGraphicsListener;

};

}}

#endif /* __REMOTESCREEN_H__ */
