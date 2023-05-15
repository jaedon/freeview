/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "EventLoop.h"

#include <nrdbase/Application.h>
#include <nrdbase/PerformanceMarker.h>

using namespace netflix;

NRDP_DEFINE_OBJECTCOUNT_RECORD(EventLoopEventCount, EventLoopEvent);
NRDP_DEFINE_OBJECTCOUNT_RECORD(EventLoopTimerCount, EventLoopTimer);

std::string EventLoopTimerCount::describe(void *object) const
{
    EventLoop::Timer *timer = reinterpret_cast<EventLoop::Timer*>(object);
    return StringFormatter::sformat("Timer(%p): %s(%dms) %lld [%s]%s", timer, timer->isSingleShot() ? "SINGLESHOT" : "",
                                    timer->interval(), ((long long)timer->nextTime().ms())-Time::mono().ms(),
                                    timer->describe().c_str(), timer->isRunning() ? " RUNNING" : "");
}

std::string EventLoopEventCount::describe(void *object) const
{
    return reinterpret_cast<EventLoop::Event*>(object)->describe();
}

#if 0
# define DEBUG_EVENTLOOP(fmt, ...) Log::error(TRACE_EVENTLOOP, fmt, __VA_ARGS__)
#else
# define DEBUG_EVENTLOOP(fmt, ...)
#endif

bool EventLoop::Timer::start()
{
    return Application::instance()->startTimer(shared_from_this());
}

void EventLoop::Timer::stop()
{
    Application::instance()->stopTimer(shared_from_this());
}

EventLoop::EventLoop() : mRunning(0), mMutex(netflix::EVENTLOOP_MUTEX, "EventLoop"),
                         mWakeup(false), mIteration(0), mShutdown(false), mTimerIndex(0)
{
}

EventLoop::~EventLoop()
{
    assert(!mRunning);
    mTimers.deleteAll();
}

void EventLoop::wakeup()
{
    PERFORMANCE_MARKER_SCOPED("eventloop.wakeup");

    DEBUG_EVENTLOOP("%s!", "Wakeup");
    {
        ScopedMutex _lock(mMutex);
        mWakeup = true;
        mWakeupCondition.signal();
    }
}

bool EventLoop::spin(llong waitMS)
{
    PERFORMANCE_MARKER_SCOPED("eventloop.spin");
    ++mIteration;

    //handle posted events
    if(!sendEvents(Send))
        return false;

    //handle timers
    const Time timerLimit = Time::mono() + Configuration::eventloopMaxTimerTime();
    int oldRunning = 0;
    PERFORMANCE_MARKER_SCOPED("eventloop.timers");
    std::vector<Timer::SharedPtr> timers;
    {
        ScopedMutex _lock(mMutex);
        if(!mRunning)
            return false;
        oldRunning = mRunning;
        if(!mTimers.empty()) {
            timers.reserve(mTimers.size());
            for(Timer::SharedPtr timer = mTimers.first(); timer; timer = timer->next) {
                if(timer->nextTime() > timerLimit)
                    break;
                assert(timer->isRunning());
                timers.push_back(timer);
            }
        }
    }
    if(!timers.empty()) {
        DEBUG_EVENTLOOP("Timers: %zu", timers.size());
        for(std::vector<Timer::SharedPtr>::const_iterator it = timers.begin(); it != timers.end(); ++it) {
            Timer::SharedPtr timer = (*it);
            if(!timer->isRunning())
                continue;
            const Time now = Time::mono();
            if(timer->nextTime() > now)
                break;
            DEBUG_EVENTLOOP("Timers[%lld]: fire: %p: %lld [%lld/%d]", Time::mono().ms(), timer.get(), timer->nextTime().ms(), now.ms(), timer->interval());
            mTimerIndex.ref();
            timer->timerFired();
            DEBUG_EVENTLOOP("Timers[%lld]: ~fire: %p: %lld [%lld/%d]", Time::mono().ms(), timer.get(), timer->nextTime().ms(), now.ms(), timer->interval());
            {
                ScopedMutex _lock(mMutex);
                if (mShutdown)
                    return false;
                const Time after = Time::mono();
                if(timer->isRunning()) {
                    mTimers.remove(timer);
                    if(timer->isSingleShot()) {
                        timer->setRunning(false);
                        DEBUG_EVENTLOOP("Timer: singleshot: %p", timer.get());
                    } else { //reinsert
                        timer->updateNextTime(after);
                        insertTimer(timer);
                    }
                }
                if(oldRunning > mRunning)
                    return false;
                if(after > timerLimit) {
#if defined(NRDP_HAS_TRACING)
                    for(++it; it != timers.end(); ++it) {
                        Timer::SharedPtr timer = (*it);
                        if(timer->isRunning())
                            NTRACE(TRACE_EVENTLOOP, "EventLoop::spin took too long, preempting %p timer (%lld)", timer.get(), (after.ms()-timer->nextTime().ms()));
                    }
#endif
                    break;
                }
                if(!mPostEvents.empty() && mPostEvents.top()->priority() == Event::HighPriority)
                    break;
            }
        }
    }

    //wait for next spin
    {
        PERFORMANCE_MARKER_SCOPED("eventloop.spin.wait");
        {
            ScopedMutex _lock(mMutex);
            if(!mRunning)
                return false;

            DEBUG_EVENTLOOP("CalcWaitTime %zu/%zu [%lld]", mPostEvents.size(), mTimers.size(), nextTimer().ms());
            if(!mPostEvents.empty()) {
                waitMS = 0;
            } else if(!mTimers.empty()) {
                const Time timer = (nextTimer()-Time::mono());
                if(waitMS == -1 || timer < waitMS)
                    waitMS = timer.ms();
            }
            DEBUG_EVENTLOOP("~CalcWaitTime %zu/%zu [%lld]", mPostEvents.size(), mTimers.size(), waitMS);
        }

        DEBUG_EVENTLOOP("Sleep %lld [%llu]", waitMS, Time::mono().ms());
        wait(waitMS);
        DEBUG_EVENTLOOP("Awake [%llu]", Time::mono().ms());
    }
    return true;
}

void EventLoop::wait(llong waitMS)
{
    PERFORMANCE_MARKER_SCOPED("eventloop.wait");

    ScopedMutex _lock(mMutex);
    Time wait(waitMS);
    while(!mWakeup) {
        if(mWakeupCondition.wait(&mMutex, &wait) == NFErr_TimedOut)
            break;
    }
    mWakeup = false;
}

void EventLoop::stop()
{
    LinkedList<Timer::SharedPtr, Timer> timers;
    EventQueue events;
    {
        ScopedMutex _lock(mMutex);
        assert(!mShutdown);
        mShutdown = true;
        std::swap(timers, mTimers);
        std::swap(events, mPostEvents);
        exit();
    }
    if(!events.empty()) {
        DEBUG_EVENTLOOP("Events: %zu", events.size());
        while (!events.empty()) {
            Event::SharedPtr event = events.pop();
            assert(event);
            PERFORMANCE_MARKER_SCOPED("eventloop.cancelevents");
            DEBUG_EVENTLOOP("Events[%lld]:  cancel: %p (%s)", Time::mono().ms(), event.get(), event->describe().c_str());
            event->eventCanceled();
            DEBUG_EVENTLOOP("Events[%lld]: ~cancel: %p", Time::mono().ms(), event.get());
        }
    }
    timers.deleteAll();
}

bool EventLoop::sendEvents(Mode mode)
{
    PERFORMANCE_MARKER_SCOPED("eventloop.sendevents");
    int oldRunning = 0;
    const ullong started = Time::monoMS();
    while(true) {
        Event::SharedPtr event;
        {
            ScopedMutex _lock(mMutex);
            mWakeup = false;
            if(!mRunning || oldRunning > mRunning)
                return false;
            if (mPostEvents.empty())
                break;
            event = mPostEvents.top();
            assert(event);

            if(mode != Flush && event->priority() != Event::HighPriority) {
                const Time now = Time::mono();
                if(!mTimers.empty() && now >= nextTimer() && (now-event->time()) < Configuration::eventloopMaxPendingTime()) {
                    NTRACE(TRACE_EVENTLOOP, "Preempting event %p (priority %d) because of expired timers (%lld)",
                           event.get(), event->priority(), (now-event->time()).ms());
                    break;
                } else if (now-started > Configuration::eventloopMaxSendTime()) {
                    NTRACE(TRACE_EVENTLOOP, "EventLoop::sendEvents took too long, preempting %d events", mPostEvents.size());
                    break;
                }
            }
            assert(event.use_count() > 1);
            assert(mPostEvents.top() == event);
            mPostEvents.pop();
            oldRunning = mRunning;
        }
        assert(event);
        event->mTimerPreemption = mTimerIndex.count() - event->mTimerPreemption;
        DEBUG_EVENTLOOP("Events:  fire: %p (%s)", event.get(), event->describe().c_str());
        event->eventFired();
        DEBUG_EVENTLOOP("Events: ~fire: %p", event.get());
    }
    return true;
}

bool EventLoop::postEvent(const Event::SharedPtr &event)
{
    PERFORMANCE_MARKER_SCOPED("eventloop.postevent");

    ScopedMutex _lock(mMutex);
    if(mShutdown) {
        NTRACE(TRACE_EVENTLOOP, "Event posted after shutting down: %p %s", event.get(), event->describe().c_str());
        return false;
    }

    DEBUG_EVENTLOOP("Events: posting: %p (%s)::%d", event.get(), event->describe().c_str(), event->priority());
    assert(event->priority() >= 0 && static_cast<int>(event->priority()) < PriorityRange);
    if(compress(event, mPostEvents)) {
        DEBUG_EVENTLOOP(" + Compressed %p", event.get());
        return true;
    }
    assert(event);
    event->mTimerPreemption = mTimerIndex.count();
    mPostEvents.push(event, event->priority());
    wakeup();
    return true;
}

bool EventLoop::startTimer(const Timer::SharedPtr &timer)
{
    PERFORMANCE_MARKER_SCOPED("eventloop.starttime");

    ScopedMutex _lock(mMutex);
    if(mShutdown) {
        Log::error(TRACE_EVENTLOOP, "Timer: started after shutting down: %p [%d]", timer.get(), timer->interval());
        return false;
    }
    if(timer->isRunning())
        stopTimer(timer);
    timer->setRunning(true);
    timer->updateNextTime(Time::mono());
    DEBUG_EVENTLOOP("Timer: starting: %p [%d] [%lld]", timer.get(), timer->interval(), timer->nextTime().ms());
    insertTimer(timer);
    wakeup();
    return true;
}

void EventLoop::stopTimer(const Timer::SharedPtr &timer)
{
    PERFORMANCE_MARKER_SCOPED("eventloop.stoptimer");

    ScopedMutex _lock(mMutex);
    if(timer->isRunning() && !mShutdown) {
        timer->setRunning(false);
        DEBUG_EVENTLOOP("Timer: stopping: %p", timer.get());
        mTimers.remove(timer);
        wakeup();
    }
}

void EventLoop::dumpTimers() const
{
    ScopedMutex _lock(mMutex);
    int i = 0;
    for(Timer::SharedPtr timer = mTimers.first(); timer; timer = timer->next)
        Log::warn(TRACE_LOG, "Timer(%p):%d: %s(%dms) %lld [%s]", timer.get(), i++, timer->isSingleShot() ? "SINGLESHOT" : "",
                  timer->interval(), ((long long)timer->nextTime().ms())-Time::mono().ms(),
                  timer->describe().c_str());
}
