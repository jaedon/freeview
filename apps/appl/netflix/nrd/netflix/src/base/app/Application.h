/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include <nrdbase/config.h>

#include <vector>

#include <nrdbase/PerformanceMarker.h>
#include <nrdbase/Instrumentation.h>
#include <nrdbase/EventLoop.h>
#include <nrdbase/ObjectCount.h>
#include <nrdbase/ConsoleSink.h>
#include <nrdbase/Noncopyable.h>
#include <nrdbase/DataBuffer.h>
#include <nrdbase/ScopedMutex.h>
#include <nrdbase/TraceAreas.h>
#include <nrdbase/Thread.h>
#include <nrdbase/Mutex.h>
#include <nrdbase/Grep.h>
#include <nrdbase/Time.h>

class ObjectCountTimer;
namespace netflix {
class LogFileSink;
class Console;
class Application
{
public:
    typedef EventLoop::Event Event;
    typedef EventLoop::Timer Timer;

    static const Time MONO_STARTED;
    inline static Application *instance() { return sInstance; }

    Application();
    virtual ~Application();

    virtual bool start();

    inline shared_ptr<TraceAreas> traceAreas() const { return mTraceAreas; }

    inline Console::SharedPtr console() const { return mConsole; }
    inline ConsoleSink::SharedPtr consoleSink() const { return mConsoleSink; }
    void setLogCatcher(const shared_ptr<LogCatcher> &catcher) { mLogCatcher = catcher; }
    std::string logfile() const;
    bool openLogFile(const std::string &logFile);

    inline EventLoop::SharedPtr eventLoop() const { return mEventLoop; }

    inline bool startTimer(const shared_ptr<Timer> &timer) {
        if(mEventLoop)
            return mEventLoop->startTimer(timer);
        return false;
    }
    inline void stopTimer(const shared_ptr<Timer> &timer) {
        if(mEventLoop)
            mEventLoop->stopTimer(timer);
    }

    inline bool postEvent(Event *event) { return postEvent(Event::SharedPtr(event)); }
    inline bool postEvent(const shared_ptr<Event> &event) {
        if(mEventLoop)
            return mEventLoop->postEvent(event);
        return false;
    }
    inline void clearEvents() {
        if(mEventLoop)
            mEventLoop->clear();
    }
    inline void flushEvents() {
        if(mEventLoop)
            mEventLoop->flushEvents();
    }

    virtual bool initConfiguration(int argc, char **argv) { return Configuration::init(argc, argv); }
    virtual ConfigurationOptions defaultConfigurationOptions() const { return Configuration::defaultOptions(); }

#ifdef INSTRUMENTATION_ENABLED
    virtual void instrumentation(const instrumentation::Event::SharedPtr &) { }
#endif

    inline bool sendEvent(Event *event) { return sendEvent(Event::SharedPtr(event)); } //takes ownership
    inline bool sendEvent(const Event::SharedPtr &event)
    {
        if (isAppThread()) {
            event->eventFired();
            return true;
        }
        return postEvent(event);
    }

    void executed(const std::string &msg);

    Console::Filters setFilters(const Console::Filters &filters)
    {
        ScopedMutex lock(mMutex);
        const Console::Filters ret = mFilters;
        mFilters = filters;
        return ret;
    }
    Console::Filters clearFilters()
    {
        ScopedMutex lock(mMutex);
        Console::Filters ret;
        std::swap(ret, mFilters);
        return ret;
    }
    Console::Filters filters() const
    {
        ScopedMutex lock(mMutex);
        return mFilters;
    }

    template <typename T>
    static void callInMainThread(const shared_ptr<T> &t, void (T::*function)(), Event::Priority priority = Event::NormalPriority)
    {
        shared_ptr<CallInMainThreadEvent<T> > ev(new CallInMainThreadEvent<T>(t, function, priority));
        instance()->postEvent(ev);
    }

    template <typename T>
    static void callInMainThread(T *t, void (T::*function)(), Event::Priority priority = Event::NormalPriority)
    {
        shared_ptr<CallInMainThreadEvent<T> > ev(new CallInMainThreadEvent<T>(t, function, priority));
        instance()->postEvent(ev);
    }

    template <typename T, typename U>
    static void callInMainThread(const shared_ptr<T> &t, void (T::*function)(const U&), const U& arg, Event::Priority priority = Event::NormalPriority)
    {
        shared_ptr<CallInMainThreadEvent<T, U> > ev(new CallInMainThreadEvent<T, U>(t, function, arg, priority));
        instance()->postEvent(ev);
    }

    template <typename T, typename U>
    static void callInMainThread(T *t, void (T::*function)(const U&), const U& arg, Event::Priority priority = Event::NormalPriority)
    {
        shared_ptr<CallInMainThreadEvent<T, U> > ev(new CallInMainThreadEvent<T, U>(t, function, arg, priority));
        instance()->postEvent(ev);
    }

    static inline bool isAppThread()
    {
        return instance()->mThread == Thread::CurrentThreadPtr();
    }

    inline void *thread() const { return mThread; }
    void setObjectCountTimerInterval(int interval);

    static DataBuffer generateSecret(size_t len);

    virtual Variant versionInfo() const;

protected:
    inline void cleanupEventLoop() {
        assert(isAppThread());
        if(mEventLoop) {
            mEventLoop->stop();
            mEventLoop.reset();
        }
    }
    virtual EventLoop::SharedPtr createEventLoop() const { return EventLoop::SharedPtr(new EventLoop); }

    inline void cleanupConsoleSink() {
        assert(isAppThread());
        if(mConsoleSink) {
            LogSink::remove(mConsoleSink);
            mConsoleSink.reset();
        }
    }
    virtual ConsoleSink::SharedPtr createConsoleSink() const { return ConsoleSink::SharedPtr(new ConsoleSink); }

    inline void cleanupConsole() {
        assert(isAppThread());
        if(mConsole) {
            mConsole->stop();
            mConsole.reset();
        }
    }
    virtual Console::SharedPtr createConsole() const { return Console::SharedPtr(); }

    static bool initOpenSSL();
    static void cleanupOpenSSL();
    void initLogging();
    virtual std::vector<shared_ptr<LogSink> > logSinks() const;
private:
    bool createLogFileSink(const std::string &logFile);
    bool initTraceAreas();

    static void callSetupOpenSSL();
    template <typename T, typename U = char>
    class CallInMainThreadEvent : public Event
    {
    public:
        CallInMainThreadEvent(const shared_ptr<T> &t, void (T::*function)(), Priority priority = NormalPriority)
            : Event(priority), mObject(t), mNaked(t.get()), mNoArgFunction(function), mArgFunction(0), mArg(U())
        {
        }
        CallInMainThreadEvent(T *t, void (T::*function)(), Priority priority = NormalPriority)
            : Event(priority), mNaked(t), mNoArgFunction(function), mArgFunction(0), mArg(U())
        {
        }
        CallInMainThreadEvent(const shared_ptr<T> &t, void (T::*function)(const U&), const U& arg, Priority priority = NormalPriority)
            : Event(priority), mObject(t), mNaked(t.get()), mNoArgFunction(0), mArgFunction(function), mArg(arg)
        {
        }
        CallInMainThreadEvent(T *t, void (T::*function)(const U&), const U& arg, Priority priority = NormalPriority)
            : Event(priority), mNaked(t), mNoArgFunction(0), mArgFunction(function), mArg(arg)
        {
        }

        virtual std::string describe() const { return "CallInMainThread"; }

        virtual void eventFired()
        {
            PERFORMANCE_MARKER_SCOPED("callinmainthreadevent.fired");
            if (mArgFunction) {
                (mNaked->*mArgFunction)(mArg);
            } else {
                (mNaked->*mNoArgFunction)();
            }
        }
    private:
        const shared_ptr<T> mObject;
        T *mNaked;
        void (T::*mNoArgFunction)();
        void (T::*mArgFunction)(const U&);
        const U mArg;
    };

    static Application *sInstance;
    mutable Mutex mMutex;

    shared_ptr<LogCatcher> mLogCatcher;
    shared_ptr<LogFileSink> mLogFileSink;
    ConsoleSink::SharedPtr mConsoleSink;
    EventLoop::SharedPtr mEventLoop;
    shared_ptr<TraceAreas> mTraceAreas;
    Console::Filters mFilters;
    void *mThread;
    shared_ptr<ObjectCountTimer> mObjectCountTimer;
    Console::SharedPtr mConsole;
};

static inline Application *app() { return Application::instance(); }

}

#endif /* __NRDAPPLICATION_H__ */
