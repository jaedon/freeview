/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef __EVENTLOOP_H__
#define __EVENTLOOP_H__

#include <nrdbase/Mutex.h>
#include <nrdbase/LinkedList.h>
#include <nrdbase/ScopedMutex.h>
#include <nrdbase/PriorityQueue.h>
#include <nrdbase/ConditionVariable.h>
#include <nrdbase/AtomicCount.h>

struct EventLoopTimerCount : public netflix::ObjectCount::Record
{
public:
    inline EventLoopTimerCount(const std::string &n) : netflix::ObjectCount::Record(n) { }
    inline virtual ~EventLoopTimerCount() { }
    virtual std::string describe(void *object) const;
};

struct EventLoopEventCount : public netflix::ObjectCount::Record
{
public:
    inline EventLoopEventCount(const std::string &n) : netflix::ObjectCount::Record(n) { }
    inline virtual ~EventLoopEventCount() { }
    virtual std::string describe(void *object) const;
};

NRDP_DECLARE_OBJECTCOUNT_RECORD(EventLoopEventCount, EventLoopEvent);
NRDP_DECLARE_OBJECTCOUNT_RECORD(EventLoopTimerCount, EventLoopTimer);

namespace netflix {

class EventLoop
{
public:
    typedef shared_ptr<EventLoop> SharedPtr;

    class Timer : public enable_shared_from_this<Timer>, public Noncopyable
    {
    public:
        typedef shared_ptr<Timer> SharedPtr;
        typedef weak_ptr<Timer> WeakPtr;
        Timer(int interval) : mInterval(interval), mRunning(false), mSingleShot(false)
        {
            NRDP_OBJECTCOUNT_REF(EventLoopTimer, this);
        }
        virtual ~Timer()
        {
            NRDP_OBJECTCOUNT_DEREF(EventLoopTimer, this);
        }

        virtual bool start();
        virtual void stop();
        inline bool restart(int interval = -1)
        {
            if(isRunning())
                stop();
            if(interval != -1)
                mInterval = interval;
            return start();
        }

        inline void setSingleShot(bool s) { mSingleShot = s; }
        inline bool isSingleShot() { return mSingleShot; }

        inline bool isRunning() const { return mRunning; }
        inline void updateNextTime(const Time &now) { mNextTime = isRunning() ? now + interval() : Time(); }
        inline Time nextTime() const { return mNextTime; }

        inline int interval() const { return mInterval; }
        inline void setRunning(bool running) { mRunning = running; }
        virtual void timerFired() = 0;
        virtual std::string describe() const
        {
            return StringFormatter::sformat("Timer(%dms%s) %lld", mInterval, isRunning() ? "" : " not running", ((long long)nextTime().ms())-Time::mono().ms());
        }

        Timer::SharedPtr next, prev;

    private:
        int mInterval;
        bool mRunning;
        Time mNextTime;
        bool mSingleShot;
    };

    class Event : public Noncopyable
    {
    public:
        typedef shared_ptr<Event> SharedPtr;
        typedef weak_ptr<Event> WeakPtr;

        enum Priority { HighPriority=0, NormalPriority=1, LowPriority=2 };

        Event(Priority priority) : mType(-1), mTime(Time::mono()), mPriority(priority), mTimerPreemption(0)
        {
            NRDP_OBJECTCOUNT_REF(EventLoopEvent, this);
        }
        Event(int type = -1, Priority priority=NormalPriority) : mType(type), mTime(Time::mono()), mPriority(priority), mTimerPreemption(0)
        {
            NRDP_OBJECTCOUNT_REF(EventLoopEvent, this);
        }
        virtual ~Event()
        {
            NRDP_OBJECTCOUNT_DEREF(EventLoopEvent, this);
        }

        virtual void eventFired() = 0;
        virtual void eventCanceled() { }
        virtual std::string describe() const
        {
            return StringFormatter::sformat("Event(%d, %llu, %d)", mType, (Time::mono() - mTime).val(), mPriority);
        }
        inline int type() const { return mType; }
        inline Time time() const { return mTime; }
        inline Priority priority() const { return mPriority; }

        int timerPreemption() const { return mTimerPreemption; }
    private:
        const int mType;
        const Time mTime;
        const Priority mPriority;
        size_t mTimerPreemption;
        friend class EventLoop;
    };

    EventLoop();
    virtual ~EventLoop();

    void stop();
    inline bool sendEvents() { return sendEvents(Send); }
    inline bool flushEvents() { return sendEvents(Flush); }
    inline Time eventDelay() const {
        ScopedMutex _lock(mMutex);
        return mPostEvents.empty() ? Time(0) : Time::mono() - mPostEvents.top()->time();
    }
    inline bool hasEvents() const {
        ScopedMutex _lock(mMutex);
        return !mPostEvents.empty();
    }

    inline int getIteration() const { return mIteration; }

    inline void clear()
    {
        ScopedMutex _lock(mMutex);
        mPostEvents.clear();
        mTimers.clear();
        wakeup();
    }

    inline void enter()
    {
        ScopedMutex _lock(mMutex);
        ++mRunning;
    }
    inline void exit()
    {
        ScopedMutex _lock(mMutex);
        if(mRunning) {
            --mRunning;
            wakeup();
        }
    }
    void run()
    {
        enter();
        while(true) {
            {
                ScopedMutex _lock(mMutex);
                if(!mRunning)
                    break;
            }
            if(!spin(-1))
                break;
        }
    }
    bool spin(llong waitMS);

    virtual void wakeup();

    bool postEvent(const Event::SharedPtr &event);
    bool startTimer(const Timer::SharedPtr &timer);
    void stopTimer(const Timer::SharedPtr &timer);
    inline Time nextTimer() const {
        ScopedMutex _lock(mMutex);
        return mTimers.empty() ? Time(0) : mTimers.first()->nextTime();
    }
    void dumpTimers() const;

protected:
    inline void insertTimer(const Timer::SharedPtr &timer) {
        assert(mMutex.isLocked());
        Timer::SharedPtr before = mTimers.first();
        while(before) {
            if(before->nextTime() > timer->nextTime())
                break;
            before = before->next;
        }
        mTimers.insertBefore(timer, before);
    }

    enum Mode {
        Flush,
        Send
    };
    bool sendEvents(Mode mode);
    enum { PriorityRange = 3 };
    typedef PriorityQueue<Event::SharedPtr, PriorityRange, 256> EventQueue;
    virtual bool compress(const Event::SharedPtr &, EventQueue &) { return false; }
    virtual void wait(llong waitMS);

private:
    int mRunning;
    mutable Mutex mMutex;

    bool mWakeup;
    int mIteration;
    ConditionVariable mWakeupCondition;
    bool mShutdown;

    LinkedList<Timer::SharedPtr, Timer> mTimers;
    AtomicCount mTimerIndex;

    EventQueue mPostEvents;
};
}

#endif /* __EVENTLOOP_H__ */
