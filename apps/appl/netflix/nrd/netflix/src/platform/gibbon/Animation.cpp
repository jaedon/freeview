/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strict
 * prohibited.
 */


#include <Debug.h>
#include <Animation.h>
#include <Screen.h>
#include <Widget.h>
#include <GibbonApplication.h>
#include <GibbonConfiguration.h>
#include <GibbonFreetype.h>
#include <FontManager.h>
#include <SurfaceCache.h>
#include <nrd/AppThread.h>
#include <nrd/AppLog.h>
#ifdef GIBBON_BENCHMARKS
#include <Benchmarks.h>
#endif

#include <nrdbase/Instrumentation.h>
#include <nrdbase/tr1.h>
#include <nrdbase/Time.h>
#include <nrdbase/MutexRanks.h>
#include <nrdbase/PerformanceMarker.h>
#include <nrdbase/ScopedMutex.h>
#include <nrdbase/Stopwatch.h>

#include <iostream>
#include <iomanip>
#include <stdio.h>

using namespace netflix;
using namespace netflix::gibbon;

bool Animation::sActive = false;
Animation::EntryContainer Animation::sEntries;
Mutex Animation::sLock(GIBBON_ANIMATION_MUTEX, "GibbonAnimation");
Mutex Animation::sThreadPtrLock(GIBBON_ANIMATION_THREAD_MUTEX, "GibbonAnimation::Thread");
void* Animation::sThreadPtr = 0;

#if defined(GIBBON_NO_THREADANIMATION)
class AnimationTimer : public GibbonApplication::Timer
{
public:
    AnimationTimer(int timeout) : GibbonApplication::Timer(timeout) { }
    ~AnimationTimer() { }
    void timerFired() {
        PERFORMANCE_MARKER_SCOPED("animationtimer.fired");
        Animation::advance(Time::mono().ms());
    }
};
static shared_ptr<AnimationTimer> sAnimationTimer;

class AnimationStartEvent;
static shared_ptr<AnimationStartEvent> sAnimationStartEvent;
static Mutex sAnimationStartEventMutex(UNTRACKED_MUTEX, "GibbonStartEventMutex");
class AnimationStartEvent : public GibbonApplication::Event
{
public:
    inline AnimationStartEvent() : GibbonApplication::Event() { }
    inline ~AnimationStartEvent() { }
    std::string describe() const { return std::string("AnimationStartEvent"); }
    void eventFired() {
        {
            ScopedMutex animationLock(sAnimationStartEventMutex);
            if(sAnimationStartEvent.get() != this)
                return;
            sAnimationStartEvent.reset();
        }

        Animation::advance(Time::mono().ms());
        if(!sAnimationTimer) {
            sAnimationTimer.reset(new AnimationTimer((int)gibbonApp()->getScreen()->getVsyncInterval()));
            gibbonApp()->startTimer(sAnimationTimer);
        }
    }
};
#else
class AnimationThread;
static AnimationThread *sAnimationThread = 0;
class AnimationThread : public Thread
{
public:
    enum {
        State_Done    = 0x01,
        State_Wakeup  = 0x02,
        State_Purge   = 0x04,
        State_Start   = 0x08,
        State_InRun   = 0x10
    };

    inline AnimationThread() : Thread(&THREAD_GIBBON_ANIMATION),
                               mLock(GIBBON_ANIMATIONLOOP_MUTEX, "GibbonAnimationThread"),
                               mState(0) { }
    inline ~AnimationThread() { }
    inline bool isInRun() const { return checkState(State_InRun); }
    inline void shutdown() {
        assert(MutexStack::locked(Animation::lock().name()));
        {
            std::list<GibbonApplication::Event::SharedPtr> events;
            {
                ScopedMutex _lock(mLock);
                wakeup(State_Done);
                std::swap(mEvents, events);
            }
            for(std::list<GibbonApplication::Event::SharedPtr>::const_iterator it = events.begin();
                 it != events.end();
                 ++it) {
                (*it)->eventCanceled();
            }
        }
        while(sAnimationThread)
            mQuitCondition.wait(&Animation::lock());
        Wait();
        delete this;
    }
    inline void startAnimations() { wakeup(State_Start);  }
    inline bool postEvent(const GibbonApplication::Event::SharedPtr &event) {
        ScopedMutex _lock(mLock);
        if(mState & State_Done)
            return false;
        mEvents.push_back(event);
        wakeup();
        return true;
    }
    inline bool startTimer(const shared_ptr<GibbonApplication::Timer> &timer)
    {
        ScopedMutex _lock(mLock);
        if(mState & State_Done) {
            Log::error(TRACE_EVENTLOOP, "Timer: started after shutting down: %p [%d]", timer.get(), timer->interval());
            return false;
        }
        mTimers.push_back(AnimationTimer::SharedPtr(new AnimationTimer(timer)));
        wakeup();
        return true;
    }
    inline void stopTimer(const shared_ptr<GibbonApplication::Timer> &timer)
    {
        ScopedMutex _lock(mLock);
        for(std::list<AnimationTimer::SharedPtr>::iterator it = mTimers.begin(); it != mTimers.end(); ) {
            if((*it)->mTimer == timer)
                it = mTimers.erase(it);
            else
                ++it;
        }
        wakeup();
    }
    inline bool checkState(unsigned char state) const {
        ScopedMutex _lock(mLock);
        return (mState & state) == state;
    }
    inline void wakeup(unsigned char state=0) {
        ScopedMutex _lock(mLock);
        mState |= (state|State_Wakeup);
        mWakeCondition.signal();
    }

private:
    void Run() {
        Animation::setAnimationThreadPtr(CurrentThreadPtr());

        gibbonApp()->initGraphics();
        Screen::SharedPtr screen = gibbonApp()->getScreen();
        screen->makeCurrent(true);

#if GIBBON_BENCHMARKS
        if(!GibbonConfiguration::benchmark().empty()) {
            Benchmarks::run(GibbonConfiguration::benchmark());
            return;
        }
#endif

        Stopwatch sw;
        bool wasAdvancing = false, needsPurge = false;
        while(true) {
            PERFORMANCE_MARKER_SCOPED("animation.loop");

            const Stopwatch frameStart;
            bool startAnimations = false;

            Time nextTimer;
            std::list<AnimationTimer::SharedPtr> timers;
            std::list<GibbonApplication::Event::SharedPtr> events;
            {
                ScopedMutex _lock(mLock);
#ifdef GIBBON_GRAPHICSENGINE_GLES2
                GraphicsEngine::freeSurfaces();
#endif
                if(mState & State_Done)
                    break;
                std::swap(mEvents, events);
                if(!mTimers.empty()) {
                    const Time now = Time::mono();
                    for(std::list<AnimationTimer::SharedPtr>::const_iterator it = mTimers.begin(); it != mTimers.end(); ++it) {
                        if((*it)->mNext <= now)
                            timers.push_back(*it);
                        else if(!nextTimer.ms() || (*it)->mNext < nextTimer)
                            nextTimer = (*it)->mNext;
                    }
                }
                if(mState & State_Wakeup) {
                    needsPurge = true;
                    mState &= ~State_Wakeup;
                }
                if(mState & State_Start) {
                    startAnimations = true;
                    mState &= ~ State_Start;
                }
                mState |= State_InRun;
            }

            //fire timers
            if(!timers.empty()) {
                PERFORMANCE_MARKER_SCOPED("animation.loop.timers");

                for(std::list<AnimationTimer::SharedPtr>::const_iterator it = timers.begin(); it != timers.end(); ++it) {
                    Application::Timer::SharedPtr timer = (*it)->mTimer;
                    timer->timerFired();
                    (*it)->mNext = Time::mono().ms() + timer->interval();
                    if(!nextTimer.ms() || (*it)->mNext < nextTimer)
                        nextTimer = (*it)->mNext;
                }
            }

            //fire events
            if(!events.empty()) {
                PERFORMANCE_MARKER_SCOPED("animation.loop.events");

                for(std::list<GibbonApplication::Event::SharedPtr>::const_iterator it = events.begin(); it != events.end(); ++it)
                    (*it)->eventFired();
                startAnimations = true;
                events.clear();
            }

            //frame pass
            {
                PERFORMANCE_MARKER_SCOPED("animation.loop.frame");

                bool advanced = false;
                if(startAnimations || Animation::isAnimating()) {
                    if(!wasAdvancing)
                        INSTRUMENTATION_INTERVAL_SWITCHED_START("gibbon.animation", Variant::null(), 60);
                    advanced = Animation::advance((ullong)frameStart.started());
                    if(!wasAdvancing) {
                        if(!advanced) {
                            INSTRUMENTATION_INTERVAL_SWITCHED_CANCEL("gibbon.animation");
                        } else {
                            wasAdvancing = true;
                        }
                    }
                }
                {
                    ScopedMutex _lock(mLock);
                    mState &= ~State_InRun;
                }
                if(screen->hasFrameGraph())
                    screen->addAnimationTime(frameStart.elapsed());
                {
                    const bool rendered = screen->render();
                    static const Variant vRendered(Variant::StringMap("rendered", true));
                    static const Variant vNotRendered(Variant::StringMap("rendered", false));
                    INSTRUMENTATION_INTERVAL_SWITCHED_COUNT_MARK("gibbon.animation", "frames", rendered ? vRendered : vNotRendered);
                }
                if(!advanced && wasAdvancing) {
                    wasAdvancing = false;
                    INSTRUMENTATION_INTERVAL_SWITCHED_END("gibbon.animation");
                }
            }

            if(!Animation::isAnimating()) { //idle time
                Stopwatch::ms timeLeft = NAN;
                bool purgeWidgets = false;
                {
                    if(nextTimer.ms()) {
                        const llong timerWait = (nextTimer - Time::mono()).ms();
                        timeLeft = isnan(timeLeft) ? timerWait : std::min(timeLeft, (Stopwatch::ms)timerWait);
                    }

                    if(needsPurge && isnan(timeLeft) && !GibbonApplication::instance()->getSurfaceCache()->isDecoding()) {
                        purgeWidgets = true;
                        timeLeft = (Time::fromSeconds(5) - Widget::lastPurged()).ms();
                    }

                    ScopedMutex _lock(mLock);
                    if(mState & State_Done)
                        break;
                    if(!Debug::TestAnimationNoIdle && !(mState & State_Wakeup)) {
                        Time tmpTime, *timeToWait = 0;
                        bool wait = false;
                        if(isnan(timeLeft)) {
                            wait = true;
                        } else if(int t = (int)floor(timeLeft)) {
                            wait = true;
                            tmpTime = Time::fromMS(t);
                            timeToWait = &tmpTime;
                        }
                        while(wait) {
                            PERFORMANCE_MARKER_SCOPED("animation.loop.idle");
                            if(mWakeCondition.wait(&mLock, timeToWait) == NFErr_TimedOut || (mState & State_Wakeup))
                                break;
                        }
                    }
                    if(mState & State_Wakeup)
                        purgeWidgets = false;
                }
                if(purgeWidgets) {
                    PERFORMANCE_MARKER_SCOPED("animation.loop.purge");
                    needsPurge = false;
                    ScopedMutex lock(Widget::lock());
                    if(const unsigned int purged = Widget::purgeAll(Widget::Purge_Preemptive)) {
                        (void)purged;
                        NTRACE(TRACE_GIBBON_SURFACE, "Preemptive Purged: %d [%d]", purged, __LINE__);
                    }
                }
                INSTRUMENTATION_INTERVAL_SWITCHED_MARK("gibbon.animation","wait", timeLeft);
            }
        }
        screen->makeCurrent(false);
        gibbonApp()->cleanupGraphics();
        {
            ScopedMutex _lock(Animation::lock());
            sAnimationThread = 0;
            Animation::setAnimationThreadPtr(0);
            mQuitCondition.broadcast();
        }
    }

    mutable Mutex mLock;
    unsigned char mState;
    ConditionVariable mWakeCondition, mQuitCondition;
    struct AnimationTimer {
        typedef shared_ptr<AnimationTimer> SharedPtr;
        inline AnimationTimer(const GibbonApplication::Timer::SharedPtr &timer) : mTimer(timer) {
            mNext = Time::mono() + Time::fromMS(timer->interval());
        }
        GibbonApplication::Timer::SharedPtr mTimer;
        Time mNext; // mono
    };
    std::list<GibbonApplication::Event::SharedPtr> mEvents;
    std::list<AnimationTimer::SharedPtr> mTimers;
};

#endif

struct AnimationAdvance {
    inline AnimationAdvance(int p, const AnimationStart &_start,
                            const AnimationFinish &_finish=AnimationFinish()) :
        property(p), start(_start), finish(_finish) {
    }
    inline AnimationAdvance(int p, const AnimationFinish &_finish) :
        property(p), finish(_finish) {
    }
    inline void update(AnimationObject *object) const {
        if(finish.state == AnimationFinish::State_None)
            return;
        object->endAnimatingProperty(property, finish);
        if(finish.state == AnimationFinish::State_Restarting)
            object->startAnimatingProperty(property, start);
    }
    int id;
    int property;
    AnimationStart start;
    AnimationFinish finish;

};

class CubicBezierInterpolatorParams : public Animation::Info::InterpolatorParams
{
public:
    inline static float defaultX() { return 0.5f; }
    inline static float defaultY() { return 0.9f; }

    inline CubicBezierInterpolatorParams(float x1, float y1, float x2, float y2) {
        cx = 3.0f * x1;
        bx = 3.0f * (x2 - x1) - cx;
        ax = 1.0f - cx - bx;
        cy = 3.0f * y1;
        by = 3.0f * (y2 - y1) - cy;
        ay = 1.0f - cy - by;

    }

    inline float ease(float t, float b, float c, float d) const
    {
        return c * sampleCurveY(solveCurveX(t/d, 1.0f / (200.0f * d))) + b;
    }

private:
    float solveCurveX(float x, float e) const {
        float t2 = x;
        for(int i = 0; i < 8; i++) {
            const float x2 = sampleCurveX(t2) - x;
            if(fabs (x2) < e)
                return t2;
            const float d2 = sampleCurveDerivativeX(t2);
            if(fabs(d2) < 1e-6)
                break;
            t2 = t2 - x2 / d2;
        }
        // Fall back to the bisection method for reliability.
        float t0 = 0.0f, t1 = 1.0f;
        t2 = x;
        if(t2 < t0)
            return t0;
        else if(t2 > t1)
            return t1;
        while (t0 < t1) {
            const float x2 = sampleCurveX(t2);
            if(fabs(x2 - x) < e)
                return t2;
            if(x > x2)
                t0 = t2;
            else
                t1 = t2;
            t2 = (t1 - t0) * .5f + t0;
        }
        return t2; // Failure.
    }

    float ax, ay;
    float bx, by;
    float cx, cy;
    inline float sampleCurveX(float t) const { return ((ax * t + bx) * t + cx) * t; }
    inline float sampleCurveY(float t) const { return ((ay * t + by) * t + cy) * t; }
    inline float sampleCurveDerivativeX(float t) const { return (3.0f * ax * t + 2.0f * bx) * t + cx; }
};

class BlendInOutInterpolatorParams : public Animation::Info::InterpolatorParams
{
public:
    inline BlendInOutInterpolatorParams() : velocity(0), lastTime(0), lastVelocity(0) { }

    inline void blend(const shared_ptr<BlendInOutInterpolatorParams> &params) {
        lastVelocity = velocity = params->lastVelocity;
        lastValue = params->lastValue;
        lastTime = params->lastTime;
    }

    inline float ease(float t, float b, float c, float d) const
    {
        const double d2 = d*d;
        const float value = (((3*velocity/d2 - 6*c/(d2*d)) / 3) * (t*t*t) +
                             ((6*c/d2 - 4*velocity/d) * 0.5f) * (t*t) + velocity * t) + b;
        if(lastTime != t)
            lastVelocity = (value-lastValue) / (t-lastTime);
        lastValue = value;
        lastTime = t;
        return value;
    }

private:
    float velocity;
    mutable float lastTime, lastValue, lastVelocity;
};

class AnimationEndEvent : public GibbonApplication::Event
{
public:
    inline AnimationEndEvent(const AnimationObject::SharedPtr &object, const AnimationAdvance &advance) :
        mObject(object), mAdvance(advance) { }
    inline ~AnimationEndEvent() { }
    std::string describe() const { return std::string("AnimationEndEvent"); }
    void eventFired() {
        PERFORMANCE_MARKER_SCOPED("animationendevent.fired");
        ScopedMutex _lock(Widget::lock());
        if(AnimationObject::SharedPtr object = mObject.lock())
            mAdvance.update(object.get());
    }
protected:
    AnimationObject::WeakPtr mObject;
    AnimationAdvance mAdvance;
};

void Animation::init()
{
#if defined(GIBBON_NO_THREADANIMATION)
    Screen::SharedPtr screen = gibbonApp()->getScreen();
    screen->makeCurrent(true);
    gibbonApp()->addLibraryInfo(screen->getLibraryInfo());
#else
    ScopedMutex _lock(Animation::lock());
    sAnimationThread = new AnimationThread();
    sAnimationThread->Start();
#endif
}

void Animation::shutdown()
{
    {
        ScopedMutex _lock(Animation::lock());
#if defined(GIBBON_NO_THREADANIMATION)
        {
            ScopedMutex lock(sAnimationStartEventMutex);
            sAnimationStartEvent.reset();
        }
        if(sAnimationTimer) {
            gibbonApp()->stopTimer(sAnimationTimer);
            sAnimationTimer.reset();
            sEntries.clear();
        }
        //gibbonApp()->makeCurrent(false);
#else
        if(sAnimationThread)
            sAnimationThread->shutdown();
        sEntries.clear();
#endif
        if(sActive) {
            sActive = false;
            gibbonApp()->endAnimation();
        }
    }
}

void Animation::destroy(AnimationObject *object)
{
    ScopedMutex _lock(Animation::lock());
    sEntries.erase(object);
}

void Animation::cancel(const AnimationObject::SharedPtr &object, const Task &task, float endValue, const char *desc)
{
    const float fps = task.dumpTicks(object.get(), desc, task.startTime);
    if(!isnan(endValue))
        object->endAnimatingProperty(task.property, AnimationFinish(task.currentFrame, endValue, AnimationFinish::State_Aborted, fps));
    for(std::vector<KeyFrame>::const_iterator it = task.keyFrames.begin(); it != task.keyFrames.end(); ++it) {
        const KeyFrame &frame = (*it);
#ifdef DEBUG_ANIMATION_FPS
        Log::info(TRACE_GIBBON_ANIMATION) << "Animation(ABORTED:" << desc << "):" << frame.info.id << ":" << object->describe()
                                          << ":" << object->getAnimatedPropertyName(task.property)
                                          << ":" << Info::convertRound(frame.info.round) << ":" << Info::convertInterpolator(frame.info.interpolator);
#endif
        object->endAnimatingProperty(task.property, AnimationFinish(frame, endValue, AnimationFinish::State_Aborted, fps));
    }
}

void Animation::stop(const AnimationObject::SharedPtr &object, Task &task, Stop stopType)
{
    switch(stopType) {
    case Stop_End:
        if(task.currentValue != task.currentFrame.endValue) {
            object->setAnimatedProperty(task.property, task.currentFrame.endValue);
            task.currentValue = task.currentFrame.endValue;
        }
        Animation::cancel(object, task, task.currentValue);
        break;
    case Stop_Pending:
        Animation::cancel(object, task, NAN);
        task.keyFrames.clear();
        break;
    case Stop_Current:
        Animation::cancel(object, task, task.currentValue);
        break;
    }
}

void Animation::stop(const AnimationObject::SharedPtr &object, int property, Stop stopType)
{
    ScopedMutex _lock(Animation::lock());
    assert(object);

    Animation::EntryContainer::iterator entryit = sEntries.find(object.get());
    if(entryit != sEntries.end()) {
        Tasks &tasks = (*entryit).second;
        if(!property) {
            for(Animation::Tasks::iterator taskit = tasks.begin(); taskit != tasks.end(); taskit++)
                stop(object, taskit->second, stopType);
            if(stopType != Stop_Pending)
                tasks.clear();
        } else {
            Animation::Tasks::iterator taskit = tasks.find(property);
            if(taskit != tasks.end()) {
                stop(object, taskit->second, stopType);
                if(stopType != Stop_Pending)
                    tasks.erase(taskit);
            }
        }
        if(tasks.empty())
            sEntries.erase(entryit);
    }
}

float Animation::getValue(const AnimationObject::SharedPtr &object, int property, float *minValue, float *maxValue)
{
    assert(property);
    ScopedMutex animationLock(Animation::lock());
    Animation::EntryContainer::iterator entryit = sEntries.find(object.get());
    if(entryit != sEntries.end()) {
        Tasks &tasks = (*entryit).second;
        Tasks::const_iterator taskit = tasks.find(property);
        if(taskit != tasks.end()) {
            const Task &task = taskit->second;
            if(minValue)
                *minValue = std::min(task.currentFrame.startValue, task.currentFrame.endValue);
            if(maxValue)
                *maxValue = std::max(task.currentFrame.startValue, task.currentFrame.endValue);
            return task.currentValue;
        }
    }
    return NAN;
}

void Animation::start(const AnimationObject::SharedPtr &object, int property, float startValue, const std::vector<KeyFrame> &keyFrames, bool append)
{
    assert(MutexStack::locked(Widget::lock().name()));
    if(!gibbonApp()->getScreen()->getTargetFPS()) {
        KeyFrame lastFrame = keyFrames.back();
        lastFrame.updateFlags();
        const float endValue = lastFrame.endValue;
        object->startAnimatingProperty(property, AnimationStart(lastFrame));
        object->setAnimatedProperty(property, endValue);
        object->endAnimatingProperty(property, AnimationFinish(lastFrame, endValue, AnimationFinish::State_Done, NAN));
        return;
    }

    Task task;
    task.property = property;
    task.integral = object->isIntegralProperty(task.property);
    task.currentFrame.info.interpolator = Info::Interpolator_None;
    task.currentValue = isnan(startValue) ? object->getAnimatedProperty(task.property) : startValue;

    bool started = true;
    if(!isAnimating())
        gibbonApp()->startAnimation();
    {
        ScopedMutex animationLock(Animation::lock());
#if !defined(GIBBON_NO_THREADANIMATION)
        if(!sAnimationThread)
            return;
#endif
        sActive = true;

        // find entry
        Task *oldTask = 0;
        Tasks *oldTasks = 0;
        {
            Animation::EntryContainer::iterator it = sEntries.find(object.get());
            if(it != sEntries.end())
                oldTasks = &it->second;
        }
        if(oldTasks) {
            {
                Tasks::iterator it = oldTasks->find(property);
                if(it != oldTasks->end())
                    oldTask = &it->second;
            }
            if(oldTask && append) {
                for(std::vector<KeyFrame>::const_iterator it = keyFrames.begin(); it != keyFrames.end(); ++it)
                    oldTask->appendFrame(object.get(), *it);
                return;
            }
        }
        for(std::vector<KeyFrame>::const_iterator it = keyFrames.begin(); it != keyFrames.end(); ++it)
            task.appendFrame(object.get(), *it);

        const ullong now = Time::mono().ms();
        task.lastTime = now;
        task.popFrame(now);
#if(defined(DEBUG_ANIMATION_FPS))
        task.description = "set";
#endif

        if(oldTask) { // if task found, update it
            const bool continuation = oldTask->startTime && oldTask->currentFrame.info.interpolator == task.currentFrame.info.interpolator && isnan(startValue);
            if(continuation && task.currentFrame.info.interpolator == Info::Interpolator_Linear) { //continuation
                Animation::cancel(object, *oldTask, oldTask->currentValue, "CONTINUE");
                task.currentValue = task.currentFrame.startValue = oldTask->nextValue(task.startTime-oldTask->startTime);
                task.deltaValue = task.currentFrame.endValue - task.currentFrame.startValue;
                task.lastTime = now;
                started = false;
            } else if(continuation && task.currentFrame.info.interpolator == Info::Interpolator_BlendInOut) {
                shared_ptr<BlendInOutInterpolatorParams> newParams = static_pointer_cast<BlendInOutInterpolatorParams>(task.currentFrame.info.interpolatorParams);
                shared_ptr<BlendInOutInterpolatorParams> oldParams = static_pointer_cast<BlendInOutInterpolatorParams>(oldTask->currentFrame.info.interpolatorParams);
                assert(newParams && oldParams);
                newParams->blend(oldParams);
                Animation::cancel(object, *oldTask, oldTask->currentValue, "BLEND");
            } else {
                Animation::cancel(object, *oldTask, oldTask->currentValue, "RESTART");
            }
            *oldTask = task;
        } else  if(oldTasks) { // if no task, add it
            (*oldTasks)[task.property] = task;
        } else { // if not found, create one and add it
            Tasks tasks;
            tasks[task.property] = task;
            sEntries[object.get()] = tasks;
        }
#ifdef DEBUG_ANIMATION_FPS
        Log::info(TRACE_GIBBON_ANIMATION) << "Animation(START):" << task.currentFrame.info.id << ":" << object->describe() << ":"
                                          << object->getAnimatedPropertyName(task.property)
                                          << ":" << Info::convertInterpolator(task.currentFrame.info.interpolator) << ":"
                                          << "(" << task.currentFrame.startValue << "->" << task.currentFrame.endValue << ":" << task.deltaValue << "):" << task.currentFrame.info.duration;
#endif
    }
    object->startAnimatingProperty(task.property, AnimationStart(task.currentFrame));
    object->setAnimatedProperty(task.property, task.currentValue);
    if(started) {
#if defined(GIBBON_NO_THREADANIMATION)
        shared_ptr<AnimationStartEvent> event;
        {
            ScopedMutex animationLock(sAnimationStartEventMutex);
            if(!sAnimationStartEvent) {
                sAnimationStartEvent.reset(new AnimationStartEvent);
                event = sAnimationStartEvent;
            }
        }
        if(event)
            gibbonApp()->postEvent(event);
#else
        ScopedMutex _lock(Animation::lock());
        sAnimationThread->startAnimations();
#endif
    }
}

/*
    Robert Penner's Easing Equations v1.5  May 1, 2003
    (ported to C++ by tmyles)
    (c) 2003 Robert Penner, all rights reserved.
    This work is subject to the terms in http://www.robertpenner.com/easing_terms_of_use.html

    (Following copy/pasted from http://www.robertpenner.com/easing_terms_of_use.html on 7/12/2010)
    TERMS OF USE - EASING EQUATIONS

    Open source under the BSD License.

    Copyright 2001 Robert Penner All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are
    met:

    Redistributions of source code must retain the above copyright notice,
    this list of conditions and the following disclaimer. Redistributions in
    binary form must reproduce the above copyright notice, this list of
    conditions and the following disclaimer in the documentation and/or
    other materials provided with the distribution. Neither the name of the
    author nor the names of contributors may be used to endorse or promote
    products derived from this software without specific prior written
    permission. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
    CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING,
    BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
    FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
    COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
    INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
    NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
    USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
    ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
    THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

inline static float tweenLinear(float t, float b, float c, float d)
{
    return c*t/d + b;
}

inline static float bounceEaseOut(float t, float b, float c, float d)
{
    if((t/=d) < (1.0f/2.75f)) {
        return c*(7.5625f*t*t) + b;
    } else if(t < (2.0f/2.75f)) {
        t-=(1.5f/2.75f);
        return c*(7.5625f*t*t + .75f) + b;
    } else if(t < (2.5f/2.75f)) {
        t-=(2.25f/2.75f);
        return c*(7.5625f*t*t + .9375f) + b;
    } else {
        t-=(2.625f/2.75f);
        return c*(7.5625f*t*t + .984375f) + b;
    }
}

inline static float bounceEaseIn(float t, float b, float c, float d)
{
    return c - bounceEaseOut(d-t, 0.0f, c, d) + b;
}

inline static float bounceEaseInOut(float t, float b, float c, float d)
{
    if(t < d/2.0f)
        return bounceEaseIn(t*2.0f, 0, c, d) * 0.5f + b;
    return bounceEaseOut(t*2.0f-d, 0, c, d) * 0.5f + c* 0.5f + b;
}

inline static float easeInQuad(float t, float b, float c, float d)
{
    t/=d;
    return c*t*t + b;
}

inline static float easeOutQuad(float t, float b, float c, float d)
{
    t/=d;
    return -c *t*(t-2.0f) + b;
}

inline static float easeInOutQuad(float t, float b, float c, float d)
{
    if((t/=d/2.0f) < 1.0f)
        return c/2.0f*t*t + b;
    --t;
    return -c/2.0f * (t*(t-2.0f) - 1.0f) + b;
}

inline static float easeInCubic(float t, float b, float c, float d)
{
    t/=d;
    return c*t*t*t + b;
}

inline static float easeOutCubic(float t, float b, float c, float d)
{
    t=t/d-1.0f;
    return c*(t*t*t + 1.0f) + b;
}

inline static float easeInOutCubic(float t, float b, float c, float d)
{
    if((t/=d/2.0f) < 1.0f)
        return c/2.0f*t*t*t + b;
    t-=2.0f;
    return c/2.0f*(t*t*t + 2.0f) + b;
}

inline static float easeInQuart(float t, float b, float c, float d)
{
    t/=d;
    return c*t*t*t*t + b;
}

inline static float easeOutQuart(float t, float b, float c, float d)
{
    t=t/d-1.0f;
    return -c * (t*t*t*t - 1.0f) + b;
}

inline static float easeInOutQuart(float t, float b, float c, float d)
{
    if((t/=d/2.0f) < 1.0f)
        return c/2.0f*t*t*t*t + b;
    t-=2.0f;
    return -c/2.0f * (t*t*t*t - 2.0f) + b;
}

inline static float easeInQuint(float t, float b, float c, float d)
{
    t/=d;
    return c*t*t*t*t*t + b;
}

inline static float easeOutQuint(float t, float b, float c, float d)
{
    t=t/d-1.0f;
    return c*(t*t*t*t*t + 1.0f) + b;
}

inline static float easeInOutQuint(float t, float b, float c, float d)
{
    if((t/=d/2.0f) < 1.0f)
        return c/2.0f*t*t*t*t*t + b;
    t-=2.0f;
    return c/2.0f*(t*t*t*t*t + 2.0f) + b;
}

// End Easing Equations ----------------------------------------------------

bool Animation::advance(ullong now)
{
    int more = 0;
    typedef std::vector<std::pair<AnimationObject::SharedPtr, AnimationAdvance> > Advances;
    Advances advances;
    ScopedMutex _lock(Widget::lock());
    {
        ScopedMutex animationLock(Animation::lock());
        for(Animation::EntryContainer::iterator entryit = sEntries.begin(); entryit != sEntries.end(); ) {
            Tasks &tasks = entryit->second;
            AnimationObject *object = entryit->first;
            for(Tasks::iterator taskit = tasks.begin(); taskit != tasks.end(); ) {
                Animation::Task &task = taskit->second;
                if(task.startTime >= now)
                    task.startTime = now;

                const KeyFrame frame = task.currentFrame;
                float endValue = frame.endValue;
                const ullong startTime = task.startTime;
                ullong elapsedTime = now - startTime;
                bool finished = task.finished(elapsedTime);
                while(finished && task.popFrame(now)) {
                    const char *pop_desc = "POP";
                    elapsedTime = now - task.startTime;
                    finished = task.finished(elapsedTime);
                    if(finished) {
                        pop_desc = "SKIP";
                        advances.push_back(std::make_pair(object->sharedPtr(),
                                                          AnimationAdvance(task.property,
                                                                           AnimationFinish(frame, task.currentFrame.endValue, AnimationFinish::State_Done, task.dumpTicks(object, "END", startTime)))));
                    } else {
                        task.addTick(Task::Tick(now, endValue));
                        advances.push_back(std::make_pair(object->sharedPtr(),
                                                          AnimationAdvance(task.property,
                                                                           AnimationStart(task.currentFrame),
                                                                           AnimationFinish(frame, endValue, AnimationFinish::State_Restarting, task.dumpTicks(object, pop_desc, startTime)))));
                    }
#ifdef DEBUG_ANIMATION_FPS
                    Log::info(TRACE_GIBBON_ANIMATION) << "Animation(" << pop_desc << ":" << task.description << "):" << task.currentFrame.info.id << ":" << object->describe()
                                                      << ":" << object->getAnimatedPropertyName(task.property) << ":"
                                                      << Info::convertInterpolator(task.currentFrame.info.interpolator)
                                                      << ":" << "@" << elapsedTime
                                                      << "(" << task.currentFrame.startValue << "->" << task.currentFrame.endValue << ")"
                                                      << ":" << task.currentFrame.info.duration;
#endif
                    endValue = task.currentValue = task.currentFrame.endValue;
                }

                if(!finished && !(task.currentFrame.flags & KeyFrame::NoOp)) {
                    const float newValue = task.nextValue(elapsedTime);
                    if(isnan(newValue) || isinf(newValue)) {
                        if(task.popFrame(now)) {
                            task.addTick(Task::Tick(now, endValue));
#ifdef DEBUG_ANIMATION_FPS
                            Log::info(TRACE_GIBBON_ANIMATION) << "Animation(POP2:" << task.description << "):" << task.currentFrame.info.id << ":"
                                                              << object->describe()
                                                              << ":" << object->getAnimatedPropertyName(task.property) << ":"
                                                              << Info::convertInterpolator(task.currentFrame.info.interpolator)
                                                              << ":" << "@" << elapsedTime
                                                              << "(" << task.currentFrame.startValue << "->" << task.currentFrame.endValue << ")"
                                                              << ":" << task.currentFrame.info.duration;
#endif
                            advances.push_back(std::make_pair(object->sharedPtr(),
                                                              AnimationAdvance(task.property,
                                                                               AnimationStart(task.currentFrame),
                                                                               AnimationFinish(frame, endValue, AnimationFinish::State_Restarting, task.dumpTicks(object, "POP", startTime)))));
                            endValue = task.currentValue = task.currentFrame.endValue;
                            elapsedTime = 0;
                        } else {
                            finished = true;
                        }
                    } else {
#if 1
                        float objectValue = newValue;
                        switch(task.currentFrame.info.round) {
                        case Info::Round_Floor:
                            objectValue = ::floor(objectValue);
                            break;
                        case Info::Round_Ceil:
                            objectValue = ::ceil(objectValue);
                            break;
                        case Info::Round_Round:
                            objectValue = ::round(objectValue+Epsilon);
                            break;
                        case Info::Round_Default:
                            assert(false);
                            break;
                        case Info::Round_None:
                            break;
                        }
#else
                        const float objectValue = newValue;
#endif
                        task.addTick(Task::Tick(now, objectValue));
#if(defined(DEBUG_ANIMATION_FPS) && DEBUG_ANIMATION_FPS >= 2)
                        Log::info(TRACE_GIBBON_ANIMATION) << "+ Animation(STEP):" << task.currentFrame.info.id << ":" << object->describe()
                                                          << ":" << object->getAnimatedPropertyName(task.property)
                                                          << ":" << newValue << "/" << objectValue << "@" << elapsedTime
                                                          << "(" << task.currentFrame.startValue << "->" << task.currentFrame.endValue
                                                          << ") [" << objectValue - task.currentValue << "]";
#endif
                        advances.push_back(std::make_pair(object->sharedPtr(),
                                                          AnimationAdvance(task.property, AnimationFinish(frame, objectValue, AnimationFinish::State_None, NAN))));
                        task.currentValue = newValue;
                    }
                }
                task.lastTime = now;
                if(finished) {
                    task.addTick(Task::Tick(now, endValue));
#if(defined(DEBUG_ANIMATION_FPS) && DEBUG_ANIMATION_FPS >= 2)
                    Log::info(TRACE_GIBBON_ANIMATION) << "+ Animation(FINAL):" << task.currentFrame.info.id << ":" << object->describe()
                                                      << ":" << object->getAnimatedPropertyName(task.property)
                                                      << ":" << "@" << elapsedTime
                                                      << "(" << task.currentFrame.startValue << "->" << task.currentFrame.endValue << ")"
                                                      << ":" << task.currentFrame.info.duration;
#endif
                    advances.push_back(std::make_pair(object->sharedPtr(),
                                                      AnimationAdvance(task.property, AnimationFinish(frame, endValue, AnimationFinish::State_Done, task.dumpTicks(object, "END", task.startTime)))));
                    tasks.erase(taskit++);
                } else {
                    ++more;
                    ++taskit;
                }
            }
            if(tasks.empty()) // this object is finished animating
                sEntries.erase(entryit++);
            else
                ++entryit;
        }
    }
    const bool active = more || !sEntries.empty();
    if(active != sActive) {
        sActive = active;
        if(!sActive) {
            gibbonApp()->endAnimation();
#if defined(GIBBON_NO_THREADANIMATION)
            if(sAnimationTimer) {
                gibbonApp()->stopTimer(sAnimationTimer);
                sAnimationTimer.reset();
            }
#endif
        }
    }
    if(!advances.empty()) {
        for(Advances::const_iterator it = advances.begin(); it != advances.end(); ++it) {
            const AnimationObject::SharedPtr &object = it->first;
            const AnimationAdvance &e = it->second;
            if(e.finish.state != AnimationFinish::State_Restarting && !isnan(e.finish.value))
                object->setAnimatedProperty(e.property, e.finish.value);
            if(e.finish.state != AnimationFinish::State_None) {
#if defined(GIBBON_NO_THREADANIMATION)
                gibbonApp()->sendEvent(GibbonApplication::Event::SharedPtr(new AnimationEndEvent(object, e)));
#else
                e.update(object.get());
#endif
            }
        }
    }
    return more != 0;
}

#if !defined(GIBBON_NO_THREADANIMATION)
void Animation::update(const Rect &)
{
    ScopedMutex _lock(Animation::lock());
    if(!sAnimationThread || sAnimationThread->isInRun())
        return;
    sAnimationThread->wakeup();
}

bool Animation::isAnimationThread()
{
    ScopedMutex _lock(sThreadPtrLock);
    return sThreadPtr == Thread::CurrentThreadPtr();
}

bool Animation::sendEvent(const GibbonApplication::Event::SharedPtr &event)
{
    PERFORMANCE_MARKER_SCOPED("animation.sendevent");

    if(isAnimationThread()) {
        event->eventFired();
        return true;
    }
    return postEvent(event);
}

bool Animation::postEvent(const GibbonApplication::Event::SharedPtr &event)
{
    ScopedMutex _lock(Animation::lock());
    if(sAnimationThread) {
        return sAnimationThread->postEvent(event);
    }
    return false;
}

bool Animation::startTimer(const shared_ptr<GibbonApplication::Timer> &timer)
{
    ScopedMutex _lock(Animation::lock());
    if(sAnimationThread) {
        sAnimationThread->startTimer(timer);
        return true;
    }
    return false;
}

void Animation::stopTimer(const shared_ptr<GibbonApplication::Timer> &timer)
{
    ScopedMutex _lock(Animation::lock());
    if(sAnimationThread)
        sAnimationThread->stopTimer(timer);
}
#endif

void Animation::Task::appendFrame(AnimationObject *object, Animation::KeyFrame newFrame)
{
    NRDP_UNUSED(object);
    if(newFrame.info.combine && !gibbon_float_compare(newFrame.startValue, newFrame.endValue)) {
        KeyFrame *previousFrame = 0;
        for(std::vector<KeyFrame>::reverse_iterator it = keyFrames.rbegin(); it != keyFrames.rend(); it++) {
            KeyFrame &frame = (*it);
            if(!frame.info.combine || frame.info.duration) {
                previousFrame = &frame;
                break;
            }
        }
        if(!previousFrame && !isnan(currentFrame.startValue))
            previousFrame = &currentFrame;
        if(previousFrame && previousFrame->info.interpolator == newFrame.info.interpolator) {
            if(isnan(newFrame.startValue))
                newFrame.startValue = previousFrame->endValue;
            if(gibbon_float_compare(newFrame.startValue, previousFrame->endValue) &&
               gibbon_float_compare((previousFrame->endValue-previousFrame->startValue) / previousFrame->info.duration,
                                    (newFrame.endValue-newFrame.startValue) / newFrame.info.duration)) {
                previousFrame->info.duration += newFrame.info.duration;
                previousFrame->endValue = newFrame.endValue;
                if(previousFrame == &currentFrame)
                    deltaValue = currentFrame.endValue - currentFrame.startValue;
#if(defined(DEBUG_ANIMATION_FPS))
                Log::info(TRACE_GIBBON_ANIMATION) << "Animation(COMBINE):" << newFrame.info.id << "(" << previousFrame->info.id << "):" << object->describe() << ":"
                                                  << object->getAnimatedPropertyName(property)
                                                  << ":" << Info::convertInterpolator(newFrame.info.interpolator) << ":"
                                                  << "(" << newFrame.startValue << "->" << newFrame.endValue << "):" << newFrame.info.duration
                                                  << "::" << Info::convertInterpolator(currentFrame.info.interpolator) << ":"
                                                  << "(" << currentFrame.startValue << "->" << currentFrame.endValue << "):" << currentFrame.info.duration;
#endif
                newFrame.info.duration = 0;
                keyFrames.push_back(newFrame);
                return;
            }
        }
    }

    if(isnan(newFrame.startValue)) {
        if(!keyFrames.empty())
            newFrame.startValue = keyFrames.back().endValue;
        else if(!isnan(currentFrame.startValue))
            newFrame.startValue = currentFrame.endValue;
    }
#if(defined(DEBUG_ANIMATION_FPS))
    Log::info(TRACE_GIBBON_ANIMATION) << "Animation" << (keyFrames.empty() ? "(BEGIN):" : "(APPEND):") << newFrame.info.id << ":"
                                      << object->describe() << ":" << object->getAnimatedPropertyName(property)
                                      << ":" << Info::convertInterpolator(newFrame.info.interpolator) << ":"
                                      << "(" << newFrame.startValue << "->" << newFrame.endValue << "):" << newFrame.info.duration;
#endif
    keyFrames.push_back(newFrame);
}

bool Animation::Task::popFrame(ullong now)
{
    if(keyFrames.empty()) {
        currentFrame.startValue = currentFrame.endValue = NAN;
        return false;
    }

    const KeyFrame *nextFrame = &keyFrames.front();
#if(defined(DEBUG_ANIMATION_FPS) && DEBUG_ANIMATION_FPS >= 2)
    Log::info(TRACE_GIBBON_ANIMATION) << "Animation(POPFRAME):" << currentFrame.info.id << ":" << currentValue << ":"
                                      << Info::convertInterpolator(nextFrame->info.interpolator)
                                      << "(" << nextFrame->startValue << "->" << nextFrame->endValue << ")"
                                      << ":" << nextFrame->info.duration << "/" << Info::convertInterpolator(currentFrame.info.interpolator)
                                      << "(" << currentFrame.startValue << "->" << currentFrame.endValue << ")" << ":" << currentFrame.info.duration;
#endif

#ifdef DEBUG_ANIMATION_FPS
    description = (nextFrame->info.combine && !nextFrame->info.duration) ? "combined" : "append";
#endif

    currentFrame.startValue = nextFrame->startValue;
    if(isnan(currentFrame.startValue))
        currentFrame.startValue = isnan(currentFrame.endValue) ? currentValue : currentFrame.endValue;
    currentValue = currentFrame.startValue;
    currentFrame.endValue = nextFrame->endValue;
    if(isnan(currentFrame.endValue))
        currentFrame.endValue = currentFrame.startValue;
    deltaValue = currentFrame.endValue - currentFrame.startValue;
    currentFrame.updateFlags();

    currentFrame.info = nextFrame->info;
    if(currentFrame.info.duration < 0)
        currentFrame.info.duration = 0;
    if(currentFrame.info.round == Info::Round_Default) {
        if(integral) {
            if(deltaValue < 0)
                currentFrame.info.round = Info::Round_Floor;
            else
                currentFrame.info.round = Info::Round_Ceil;
        } else {
            currentFrame.info.round = Info::Round_None;
        }
    }

    nextFrame = 0;
    keyFrames.erase(keyFrames.begin());

    startTime = now;
    return true;
}

float Animation::Task::nextValue(ullong elapsedTime) const
{
    float result = NAN;
    switch(currentFrame.info.interpolator) {
    case Info::Interpolator_None:
        result = currentValue;
        break;
    case Info::Interpolator_EaseIn:
    case Info::Interpolator_EaseInQuad:
        result = easeInQuad(elapsedTime, currentFrame.startValue, deltaValue, currentFrame.info.duration);
        break;
    case Info::Interpolator_EaseOut:
    case Info::Interpolator_EaseOutQuad:
        result = easeOutQuad(elapsedTime, currentFrame.startValue, deltaValue, currentFrame.info.duration);
        break;
    case Info::Interpolator_EaseInOut:
    case Info::Interpolator_EaseInOutQuad:
        result = easeInOutQuad(elapsedTime, currentFrame.startValue, deltaValue, currentFrame.info.duration);
        break;
    case Info::Interpolator_EaseInBounce:
        result = bounceEaseIn(elapsedTime, currentFrame.startValue, deltaValue, currentFrame.info.duration);
        break;
    case Info::Interpolator_EaseOutBounce:
        result = bounceEaseOut(elapsedTime, currentFrame.startValue, deltaValue, currentFrame.info.duration);
        break;
    case Info::Interpolator_EaseInOutBounce:
        result = bounceEaseInOut(elapsedTime, currentFrame.startValue, deltaValue, currentFrame.info.duration);
        break;
    case Info::Interpolator_EaseInCubic:
        result = easeInCubic(elapsedTime, currentFrame.startValue, deltaValue, currentFrame.info.duration);
        break;
    case Info::Interpolator_EaseOutCubic:
        result = easeOutCubic(elapsedTime, currentFrame.startValue, deltaValue, currentFrame.info.duration);
        break;
    case Info::Interpolator_EaseInOutCubic:
        result = easeInOutCubic(elapsedTime, currentFrame.startValue, deltaValue, currentFrame.info.duration);
        break;
    case Info::Interpolator_EaseInQuart:
        result = easeInQuart(elapsedTime, currentFrame.startValue, deltaValue, currentFrame.info.duration);
        break;
    case Info::Interpolator_EaseOutQuart:
        result = easeOutQuart(elapsedTime, currentFrame.startValue, deltaValue, currentFrame.info.duration);
        break;
    case Info::Interpolator_EaseInOutQuart:
        result = easeInOutQuart(elapsedTime, currentFrame.startValue, deltaValue, currentFrame.info.duration);
        break;
    case Info::Interpolator_EaseInQuint:
        result = easeInQuint(elapsedTime, currentFrame.startValue, deltaValue, currentFrame.info.duration);
        break;
    case Info::Interpolator_EaseOutQuint:
        result = easeOutQuint(elapsedTime, currentFrame.startValue, deltaValue, currentFrame.info.duration);
        break;
    case Info::Interpolator_EaseInOutQuint:
        result = easeInOutQuint(elapsedTime, currentFrame.startValue, deltaValue, currentFrame.info.duration);
        break;
    case Info::Interpolator_CubicBezier: {
        shared_ptr<CubicBezierInterpolatorParams> params = static_pointer_cast<CubicBezierInterpolatorParams>(currentFrame.info.interpolatorParams);
        assert(params);
        result = params->ease(elapsedTime, currentFrame.startValue, deltaValue, currentFrame.info.duration);
        break; }
    case Info::Interpolator_BlendInOut: {
        shared_ptr<BlendInOutInterpolatorParams> params = static_pointer_cast<BlendInOutInterpolatorParams>(currentFrame.info.interpolatorParams);
        assert(params);
        result = params->ease(elapsedTime, currentFrame.startValue, deltaValue, currentFrame.info.duration);
        break; }
    default:
    case Info::Interpolator_Linear:
        result = tweenLinear(elapsedTime, currentFrame.startValue, deltaValue, currentFrame.info.duration);
        break;
    }
    return result;
}

float
Animation::Task::dumpTicks(AnimationObject *object, const char *desc, ullong started) const
{
    Log::stream stream(Debug::DebugAnimationFPS && currentFrame.startValue != currentFrame.endValue ? Log::warn(TRACE_GIBBON_ANIMATION) : Log::info(TRACE_GIBBON_ANIMATION));

    const ullong now = ticks.empty() ? started : ticks.back().start;
    const ullong duration = now - started;
    if(!duration) {
        stream << "Animation(ABORTED:" << desc << "):" << currentFrame.info.id << ":" << object->describe()
               << ":" << object->getAnimatedPropertyName(property)
               << ":" << Info::convertRound(currentFrame.info.round) << ":" << Info::convertInterpolator(currentFrame.info.interpolator);
        return NAN;
    }

    if(const unsigned frames = ticks.size()-1) {
        const float fps = 1000.0f/(duration/frames);
        stream << "Animation(DONE:" << desc << "):" << currentFrame.info.id << ":" << object->describe()
               << ":" << object->getAnimatedPropertyName(property)
               << ": " << ticks.front().value << "->" << ticks.back().value
               << "(" << frames << "@" << duration << "::" << fps << "fps):"
               << Info::convertRound(currentFrame.info.round) << ":" << Info::convertInterpolator(currentFrame.info.interpolator) << ": ";
        ullong last_start = ticks.begin()->start;
        float last_value = ticks.begin()->value;
        for(std::list<Tick>::const_iterator it = ++ticks.begin(); it != ticks.end(); ++it) {
            stream << (it->value - last_value) << "[" << it->value << "]@" << (it->start - last_start) << " ";
            last_start = it->start;
            last_value = it->value;
        }
        ticks.clear();
        return fps;
    }
    return NAN;
}

void Animation::dump()
{
    const ullong now = Time::mono().ms();
    ScopedMutex animationLock(Animation::lock());
    for(Animation::EntryContainer::iterator entryit = sEntries.begin(); entryit != sEntries.end(); ++entryit) {
        Tasks &tasks = entryit->second;
        AnimationObject *object = entryit->first;
        for(Tasks::iterator taskit = tasks.begin(); taskit != tasks.end(); ++taskit) {
            Animation::Task &task = taskit->second;
            const ullong runtime = now - task.startTime;
            float fps = NAN;
            if(runtime) {
                const unsigned frames = task.ticks.size()-1;
                fps = 1000.0f/(float(runtime)/frames);
            }
            Log::warn(TRACE_GIBBON_ANIMATION, "Animation(%s:%p[%s]):%d: %f@(%llu/%d) %f->%f(%f) %s/%d/%s",
                      object->getAnimatedPropertyName(task.property).c_str(), object, object->describe().c_str(),
                      task.currentFrame.info.id, fps, runtime, task.currentFrame.info.duration,
                      task.currentFrame.startValue, task.currentFrame.endValue, task.currentValue,
                      Info::convertInterpolator(task.currentFrame.info.interpolator).c_str(), task.integral, Info::convertRound(task.currentFrame.info.round).c_str());
            for(std::vector<KeyFrame>::const_iterator fit = task.keyFrames.begin(); fit != task.keyFrames.end(); ++fit) {
                const KeyFrame &frame = (*fit);
                Log::warn(TRACE_GIBBON_ANIMATION, " + %d %d %f->%f %s/%s%s", frame.info.id, frame.info.duration, frame.startValue,
                          frame.endValue, Info::convertInterpolator(frame.info.interpolator).c_str(),
                          Info::convertRound(frame.info.round).c_str(), (frame.info.combine && !frame.info.duration) ? "*" : "");

            }
        }
    }
}

void Animation::setAnimationThreadPtr(void* ptr)
{
    ScopedMutex _lock(sThreadPtrLock);
    sThreadPtr = ptr;
}

Animation::Info::Info(const Variant &desc) : interpolator(Interpolator_Linear), combine(false), round(Round_Default)
{
    bool combine_set = false;
    id = desc.mapValue<int>("id", 0, INT_MAX);
    duration = static_cast<int>(ceil(desc.mapValue<float>("duration", 0, 0)));
    const Variant &ease_v = desc["ease"];
    if(ease_v.isString()) {
        if(Interpolator i = convertInterpolator(ease_v.string()))
            interpolator = i;
    } else if(ease_v.isStringMap()) {
        if(Interpolator i = convertInterpolator(ease_v["type"].string())) {
            interpolator = i;
        }
    }
    if(interpolator == Animation::Info::Interpolator_CubicBezier) {
        const Variant flags = ease_v["flags"];
        const float x1 = flags.mapValue<float>("x1", 0, CubicBezierInterpolatorParams::defaultX());
        const float y1 = flags.mapValue<float>("y1", 0, CubicBezierInterpolatorParams::defaultY());
        const float x2 = flags.mapValue<float>("x2", 0, x1);
        const float y2 = flags.mapValue<float>("y2", 0, 1.0f-y1);
        interpolatorParams.reset(new CubicBezierInterpolatorParams(x1, y1, x2, y2));
    } else if(interpolator == Animation::Info::Interpolator_BlendInOut) {
        interpolatorParams.reset(new BlendInOutInterpolatorParams);
    }
    if(desc.contains("flags")) {
        const Variant &flags = desc["flags"];
        if(flags.contains("combine")) {
            const Variant &combine_v = flags["combine"];
            if(combine_v.isBoolean()) {
                combine = combine_v.boolean();
                combine_set = true;
            }
        }
        if(flags.contains("subpixel")) {
            const Variant &subpixel_v = flags["subpixel"];
            if(subpixel_v.isBoolean()) {
                if(subpixel_v.value<bool>(0, false))
                    round = Round_None;
            } else if(subpixel_v.isString()) {
                round = convertRound(subpixel_v.string());
            }
        }
    }
    if(!combine_set && interpolator == Interpolator_Linear)
        combine = true; //compatibility
}

static struct {
    const char *name;
    Animation::Info::Interpolator interp;
} sInterpolatorMap[] = {
    { "none", Animation::Info::Interpolator_None },
    { "linear", Animation::Info::Interpolator_Linear },
    { "cubic-bezier", Animation::Info::Interpolator_CubicBezier },
    { "ease-in", Animation::Info::Interpolator_EaseIn },
    { "ease-out", Animation::Info::Interpolator_EaseOut },
    { "ease-in-out", Animation::Info::Interpolator_EaseInOut },
    { "ease-in-bounce", Animation::Info::Interpolator_EaseInBounce },
    { "ease-out-bounce", Animation::Info::Interpolator_EaseOutBounce },
    { "ease-in-out-bounce", Animation::Info::Interpolator_EaseInOutBounce },
    { "ease-in-quad", Animation::Info::Interpolator_EaseInQuad },
    { "ease-out-quad", Animation::Info::Interpolator_EaseOutQuad },
    { "ease-in-out-quad", Animation::Info::Interpolator_EaseInOutQuad },
    { "ease-in-cubic", Animation::Info::Interpolator_EaseInCubic },
    { "ease-out-cubic", Animation::Info::Interpolator_EaseOutCubic },
    { "ease-in-out-cubic", Animation::Info::Interpolator_EaseInOutCubic },
    { "ease-in-quint", Animation::Info::Interpolator_EaseInQuint },
    { "ease-out-quint", Animation::Info::Interpolator_EaseOutQuint },
    { "ease-in-out-quint", Animation::Info::Interpolator_EaseInOutQuint },
    { "ease-in-quart", Animation::Info::Interpolator_EaseInQuart },
    { "ease-out-quart", Animation::Info::Interpolator_EaseOutQuart },
    { "ease-in-out-quart", Animation::Info::Interpolator_EaseInOutQuart },
    { "blend-in-out", Animation::Info::Interpolator_BlendInOut },
    { 0, Animation::Info::Interpolator_None }
};

std::string Animation::Info::convertInterpolator(Interpolator interp)
{
    return sInterpolatorMap[interp].name;
}

Animation::Info::Interpolator Animation::Info::convertInterpolator(const std::string &interp)
{
    for(int i = 0; sInterpolatorMap[i].name; ++i) {
        if(interp == sInterpolatorMap[i].name)
            return sInterpolatorMap[i].interp;
    }
    return Animation::Info::Interpolator_None;
}

static struct {
    const char *name;
    Animation::Info::Round round;
} sRoundMap[] = {
    { "noround", Animation::Info::Round_None },
    { "floor", Animation::Info::Round_Floor },
    { "ceil", Animation::Info::Round_Ceil },
    { "round", Animation::Info::Round_Round },
    { "default", Animation::Info::Round_Default },
    { 0, Animation::Info::Round_None }
};

std::string Animation::Info::convertRound(Round round)
{
    return sRoundMap[round].name;
}

Animation::Info::Round Animation::Info::convertRound(const std::string &round)
{
    for(int i = 0; sRoundMap[i].name; ++i) {
        if(round == sRoundMap[i].name)
            return sRoundMap[i].round;
    }
    return Animation::Info::Round_Default;
}

AnimationObject::~AnimationObject()
{
    Animation::destroy(this);
}



