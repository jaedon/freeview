/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef OMXIL_H
#define OMXIL_H

#include <queue>

#include <nrdbase/Mutex.h>
#include <nrdbase/ConditionVariable.h>

#if 0
# define OMX_VERBOSE
#endif

#define OMX_INIT_STRUCTURE(a)                           \
    memset(&(a), 0, sizeof(a));                         \
    (a).nSize = sizeof(a);                              \
    (a).nVersion.s.nVersionMajor = OMX_VERSION_MAJOR;   \
    (a).nVersion.s.nVersionMinor = OMX_VERSION_MINOR;   \
    (a).nVersion.s.nRevision = OMX_VERSION_REVISION;    \
    (a).nVersion.s.nStep = OMX_VERSION_STEP
#if defined(OMX_VERBOSE)
# define OMX_CALLED(x) Log::error(TRACE_LOG, "[%s:%d]: Called: %s", __func__, __LINE__, #x)
#else
# define OMX_CALLED(x)
#endif
#if 1
# define OMX_VERIFY(x) {                                                \
        const OMX_ERRORTYPE result = x;                                 \
        if(result != OMX_ErrorNone)                                     \
            Log::error(TRACE_LOG, "[%s:%d]: !!!!!!!!!!!! Failure: %s (0x%x)", __func__, __LINE__, #x, result); \
        else { OMX_CALLED(x); }                                          }
#else
# define OMX_VERIFY(x) x
#endif

#include <string>

#include <OMX_Component.h>
#include <OMX_Core.h>

#include <nrdbase/Log.h>

namespace netflix {
namespace device {
namespace esplayer {

class ComponentOMXIL
{
public:
    struct OMXEvent {
        inline OMXEvent(OMX_EVENTTYPE eEvent, OMX_U32 nData1, OMX_U32 nData2, OMX_PTR pEventData) : event(eEvent),
                                                                                                    data1(nData1),
                                                                                                    data2(nData2),
                                                                                                    ptr(pEventData) { }
        std::string describe() const;
        OMX_EVENTTYPE event;
        OMX_U32 data1;
        OMX_U32 data2;
        OMX_PTR ptr;
    };

    inline ComponentOMXIL() : mHandle(0), mInput(0), mOutput(0),
                              mEventLock(ZERO_MUTEX, "OMX_EventLock"),
                              mBufferLock(ZERO_MUTEX, "OMX_BufferLock"),
                              mOutputBuffer(0) { }
    inline ComponentOMXIL(const std::string &name, OMX_INDEXTYPE index, OMX_CALLBACKTYPE *callbacks=0) : mHandle(0), mInput(0), mOutput(0),
                                                                                                         mEventLock(ZERO_MUTEX, "OMX_EventLock"),
                                                                                                         mBufferLock(ZERO_MUTEX, "OMX_BufferLock"),
                                                                                                         mOutputBuffer(0) { init(name, index, callbacks); }
    virtual ~ComponentOMXIL() { cleanup(); }

    inline bool isValid() const { return mHandle; }

    inline std::string name() const { return mName; }
    inline OMX_HANDLETYPE handle() const { return mHandle; }
    bool init(const std::string &name, OMX_INDEXTYPE index, OMX_CALLBACKTYPE *callbacks=0);
    void cleanup();

    bool waitForEvent(OMX_EVENTTYPE event, OMX_U32 nData1,
                      Time wait=Time(0));
    bool waitForEvent(OMX_EVENTTYPE event, OMX_U32 nData1, OMX_U32 nData2,
                      Time wait=Time(0));
    bool waitForCommandComplete(OMX_COMMANDTYPE command, OMX_U32 nData,
                                Time wait=Time(0));


    static void globalInit();
    static void globalCleanup();

    OMX_STATETYPE state() const;
    bool setState(OMX_STATETYPE state);

    static Time getClockTime();
    static shared_ptr<ComponentOMXIL> getClock();
    static void startClockTime(const Time &time);

    inline unsigned int inputPort() const { return mInput; }
    inline unsigned int outputPort() const { return mOutput; }

    bool disablePorts();
    bool enablePort(unsigned int port);
    bool disablePort(unsigned int port);
    inline bool receive(ComponentOMXIL *component, int componentPort = -1, int port = -1) {
        if(componentPort == -1)
            componentPort = component->outputPort();
        if(port == -1)
            port = inputPort();
        return receive(Connection(component, componentPort, port));
    }

    OMX_BUFFERHEADERTYPE *inputBuffer(Time wait=Time(0));
    bool flush(int port);
    inline bool flushInput() { return flush(mInput); }
    bool flushOutput() { return flush(mOutput); }
    bool emptyBuffer(OMX_BUFFERHEADERTYPE *);
    bool fillBuffer(OMX_BUFFERHEADERTYPE *);
    void cleanupOutputBuffer(int port = -1);

    static OMX_TICKS convertTime(const Time &time);
    static Time convertTime(const OMX_TICKS &ticks);

protected:
    virtual OMX_ERRORTYPE eventHandler(OMXEvent);
    virtual void filledBuffer(OMX_BUFFERHEADERTYPE *buffer);
    virtual void emptiedBuffer(OMX_BUFFERHEADERTYPE *buffer);
    bool sendData(const unsigned char *data, uint32_t dataSize, OMX_U32 flags, OMX_TICKS timeStamp, const char *desc);

    std::string mName;
    OMX_HANDLETYPE mHandle;
    unsigned int mInput, mOutput;

    Mutex mEventLock;
    std::list<OMXEvent> mEvents;
    ConditionVariable mEventCondition;

    Mutex mBufferLock;
    OMX_BUFFERHEADERTYPE *mOutputBuffer;
    ConditionVariable mInputBufferCondition;
    std::queue<OMX_BUFFERHEADERTYPE*> mInputBuffers, mPendingInputBuffers;

    static shared_ptr<ComponentOMXIL> sClock;

    struct Connection {
        inline Connection(ComponentOMXIL *_component, int _componentPort, int _port) : port(_port), componentPort(_componentPort), component(_component) { }
        int port;
        int componentPort;
        ComponentOMXIL *component;
    };
    std::list<Connection> mConnections;
    bool receive(const Connection &connection);

    int initInputBuffers(int port = -1);
    void cleanupInputBuffers(int port = -1);
    void cleanupBuffers();
    Mutex& bufferLock() { return mBufferLock; }

    static OMX_ERRORTYPE omxil_EventHandler(OMX_HANDLETYPE, OMX_PTR pAppData, OMX_EVENTTYPE eEvent, OMX_U32 nData1, OMX_U32 nData2, OMX_PTR pEventData);
    static OMX_ERRORTYPE omxil_EmptyBufferDone(OMX_HANDLETYPE, OMX_PTR pAppData, OMX_BUFFERHEADERTYPE* pBuffer)
    {
        ((ComponentOMXIL*)pAppData)->emptiedBuffer(pBuffer);
        return OMX_ErrorNone;
    }
    static OMX_ERRORTYPE omxil_FillBufferDone(OMX_HANDLETYPE, OMX_PTR pAppData, OMX_BUFFERHEADERTYPE* pBuffer)
    {
        assert(((ComponentOMXIL*)pAppData)->mOutputBuffer == pBuffer);
        ((ComponentOMXIL*)pAppData)->filledBuffer(pBuffer);
        return OMX_ErrorNone;
    }
};

}}} // namespace netflix::device::IESP

#endif // OMXIL_H
