/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "MdxTracing.h"
#include <nrdbase/ScopedMutex.h>
#include <nrdbase/Thread.h>
#include <nrdbase/Time.h>
#include <algorithm>
#include <string>

using namespace netflix;

static inline ullong now_usec()
{
    ullong result;
#ifdef __APPLE__
    mach_timebase_info_data_t timebaseInfo;
    mach_timebase_info(&timebaseInfo);
    result = ((mach_absolute_time() * timebaseInfo.numer) / (1000 * timebaseInfo.denom));
#else
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    result = ((ts.tv_sec * 1000000) + (ts.tv_nsec / 1000) );
#endif
    return result;
}

Timer::Timer(const std::string &category)
    : mCategory(category)
{
}

Timer::~Timer()
{
}

void Timer::begin()
{
    mBegin  = now_usec();
}

void Timer::end()
{
    mEnd = now_usec();
}

std::string Timer::getCategory() const
{
    return mCategory;
}

ullong Timer::getBegin() const
{
    return mBegin;
}

ullong Timer::getEnd() const
{
    return mEnd;
}

Tracing::Event::Event(const std::string &name,
                      ullong id,
                      const std::string &category,
                      const std::string &stepName,
                      ullong timeUS,
                      const Variant *args)
    : mName(name)
    , mId(id)
    , mCategory(category)
    , mTime(timeUS)
{
    if(args)
    {
        mArgs = *args;
    }

    mArgs["step"] = stepName;
}

Tracing::Scope::Scope(const std::string &name)
    : mParent(NULL)
    , mName(name)
{
}

Tracing::Scope::Scope(Scope *parent, const std::string &name)
    : mParent(parent)
    , mName(name)
{
}

Tracing::Scope::~Scope()
{
    Scopes::iterator it = mChildren.begin();
    for(; it != mChildren.end(); ++it)
    {
        delete it->second;
    }
    std::vector<Event *>::iterator instantEventIt = mInstantEvents.begin();
    for(; instantEventIt != mInstantEvents.end(); ++instantEventIt)
    {
        delete *instantEventIt;
    }
    Events::iterator eventIt = mStartEvents.begin();
    for(; eventIt != mStartEvents.end(); ++eventIt)
    {
        delete *eventIt;
    }
    eventIt = mStepEvents.begin();
    for(; eventIt != mStepEvents.end(); ++eventIt)
    {
        delete *eventIt;
    }
    eventIt = mFinishEvents.begin();
    for(; eventIt != mFinishEvents.end(); ++eventIt)
    {
        delete *eventIt;
    }
    mChildren.clear();
}

Tracing::Scope *Tracing::Scope::findAddScope(const std::string &name)
{
    Scope *scope = new Scope(this, name);
    Scopes::value_type item(scope->mName, scope);
    std::pair<Scopes::iterator, bool> result = mChildren.insert(item);
    if(!result.second)
    {
        delete scope;
    }

    return result.first->second;
}

void Tracing::Scope::report(Variant &traceEvents) const
{
    std::vector<Timer>::const_iterator timerIt = mTimers.begin();
    for(; timerIt != mTimers.end(); ++timerIt)
    {
        Variant traceEvent;
        traceEvent["name"] = mName;
        traceEvent["cat"] = timerIt->getCategory();
        traceEvent["ph"] = "B";
        traceEvent["ts"] = timerIt->getBegin();
        traceEvents.push_back(traceEvent);

        traceEvent["ph"] = "E";
        traceEvent["ts"] = timerIt->getEnd();
        traceEvents.push_back(traceEvent);
    }

    Events::const_iterator eventIt = mInstantEvents.begin();
    for(; eventIt != mInstantEvents.end(); ++eventIt)
    {
        Variant traceEvent;
        traceEvent["name"] = (*eventIt)->mName;
        traceEvent["cat"] = (*eventIt)->mCategory;
        traceEvent["ts"] = (*eventIt)->mTime;
        traceEvent["ph"] = "I";
        traceEvent["args"] = (*eventIt)->mArgs;
        traceEvents.push_back(traceEvent);
    }

    eventIt = mStartEvents.begin();
    for(; eventIt != mStartEvents.end(); ++eventIt)
    {
        Variant traceEvent;
        traceEvent["name"] = (*eventIt)->mName;
        traceEvent["cat"] = (*eventIt)->mCategory;
        traceEvent["id"] = (*eventIt)->mId;
        traceEvent["ts"] = (*eventIt)->mTime;
        traceEvent["ph"] = "S";
        traceEvent["args"] = (*eventIt)->mArgs;
        traceEvents.push_back(traceEvent);
    }
    eventIt = mStepEvents.begin();
    for(; eventIt != mStepEvents.end(); ++eventIt)
    {
        Variant traceEvent;
        traceEvent["name"] = (*eventIt)->mName;
        traceEvent["cat"] = (*eventIt)->mCategory;
        traceEvent["id"] = (*eventIt)->mId;
        traceEvent["ts"] = (*eventIt)->mTime;
        traceEvent["ph"] = "T";
        traceEvent["args"] = (*eventIt)->mArgs;
        traceEvents.push_back(traceEvent);
    }
    eventIt = mFinishEvents.begin();
    for(; eventIt != mFinishEvents.end(); ++eventIt)
    {
        Variant traceEvent;
        traceEvent["name"] = (*eventIt)->mName;
        traceEvent["cat"] = (*eventIt)->mCategory;
        traceEvent["id"] = (*eventIt)->mId;
        traceEvent["ts"] = (*eventIt)->mTime;
        traceEvent["ph"] = "F";
        traceEvent["args"] = (*eventIt)->mArgs;
        traceEvents.push_back(traceEvent);
    }

    Scopes::const_iterator childIt = mChildren.begin();
    for(; childIt != mChildren.end(); ++childIt)
    {
        childIt->second->report(traceEvents);
    }
}

Tracing::Context::Context(const std::string &name)
    : mName(name)
    , mThreadId(0)
    , mProcessId(0)
    , mRootScope("root")
    , mCurrentScope(NULL)
{
    mCurrentScope = &mRootScope;
}

Tracing::Context::~Context()
{

}

Tracing::Tracing(const std::string &name)
    : mName(name)
    , mContextMutex(UNTRACKED_MUTEX, "mdx::Tracing")
    , mEnabled(false)
{
}

Tracing::~Tracing()
{
}

void Tracing::enable()
{
    mEnabled = true;
}

void Tracing::disable()
{
    mEnabled = false;
}

void Tracing::beginScope(const std::string &name)
{
    if(!mEnabled) return;

    Context *context = getContext();

    Scope *scope = context->mCurrentScope->findAddScope(name);
    context->mCurrentScope = scope;
}

void Tracing::endScope(const std::string &name)
{
    if(!mEnabled) return;

    Context *context = getContext();

    if(context->mCurrentScope->mName != name ||
       context->mCurrentScope->mParent == NULL)
    {
        // error, popped too many scopes
    }
    else
    {
        context->mCurrentScope = context->mCurrentScope->mParent;
    }
}

void Tracing::add(const Timer &timer)
{
    if(!mEnabled) return;

    Context *context = getContext();

    context->mCurrentScope->mTimers.push_back(timer);
}

void Tracing::addInstantEvent(const std::string &name, const std::string &category, const Variant *args)
{
    if(!mEnabled) return;

    Context *context = getContext();

    Event *event = new Event(name, 0, category, "only", now_usec(), args);
    context->mCurrentScope->mInstantEvents.push_back(event);
}

void Tracing::addEventStart(const std::string &name,
                            ullong id,
                            const std::string &category,
                            const std::string &stepName,
                            const Variant *args)
{
    if(!mEnabled) return;

    Context *context = getContext();

    Event *event = new Event(name, id, category, stepName, now_usec(), args);
    context->mCurrentScope->mStartEvents.push_back(event);
}

void Tracing::addEventStep(const std::string &name,
                           ullong id,
                           const std::string &category,
                           const std::string &stepName,
                           const Variant *args)
{
    if(!mEnabled) return;

    Context *context = getContext();

    Event *event = new Event(name, id, category, stepName, now_usec(), args);
    context->mCurrentScope->mStepEvents.push_back(event);
}

void Tracing::addEventFinish(const std::string &name,
                             ullong id,
                             const std::string &category,
                             const std::string &stepName,
                             const Variant *args)
{
    if(!mEnabled) return;

    Context *context = getContext();

    Event *event = new Event(name, id, category, stepName, now_usec(), args);
    context->mCurrentScope->mFinishEvents.push_back(event);
}

static bool sortByTimestamp(const Variant &a, const Variant &b)
{
    llong ats     = a["ts"].integer();
    std::string aph = a["ph"].string();
    llong bts     = b["ts"].integer();
    std::string bph = b["ph"].string();

    // ties go to whichever one is a starting phase, B for begin, S for start
    if(ats == bts)
    {
        return aph == "B" || aph == "S";
    }
    else
    {
        return ats < bts;
    }
}

uint32_t Tracing::getReport(Variant &report)
{
    ScopedMutex lock(mContextMutex);

    Variant traceEvents;

    Contexts::const_iterator it = mContexts.begin();
    for(; it != mContexts.end(); ++it)
    {
        Context *context = it->second;

        Variant contextInfo;
        Variant args;
        Variant scopeEvents;

        args["name"] = context->mThreadName;
        contextInfo["args"] = args;
        contextInfo["name"] = "thread_name";
        contextInfo["ph"] = "M";
        contextInfo["ts"] = 0;
        contextInfo["tid"] = context->mThreadId;
        contextInfo["pid"] = context->mProcessId;
        contextInfo["cat"] = "__metadata";
        traceEvents.push_back(contextInfo);

        const Scope *scope = &context->mRootScope;

        scope->report(scopeEvents);

        Variant::ArrayIterator it = scopeEvents.arrayBegin();
        for(; it != scopeEvents.arrayEnd(); ++it)
        {
            (*it)["tid"] = context->mThreadId;
            (*it)["pid"] = context->mProcessId;
        }

        traceEvents.insert(traceEvents.end(), scopeEvents.begin(), scopeEvents.end());
    }

    std::sort(traceEvents.begin(), traceEvents.end(), sortByTimestamp);

    report["traceEvents"] = traceEvents;

    return traceEvents.size();
}

void Tracing::clear()
{
    Contexts::iterator it = mContexts.begin();
    for(; it != mContexts.end(); ++it)
    {
        delete it->second;
    }
    mContexts.clear();
}

Tracing::Context *Tracing::getContext()
{
    const void *pId = Thread::CurrentThreadId();
    char contextName[11];
    snprintf(contextName, 10, "%p", pId);

    return findAddContext(contextName);
}

Tracing::Context *Tracing::findAddContext(const std::string &name)
{
    ScopedMutex lock(mContextMutex);

    Context *context = new Context(name);
    context->mThreadId = (uint32_t)Thread::CurrentThreadId();
    context->mProcessId = 0;
    context->mThreadName = Thread::CurrentThreadName();
    Contexts::value_type item(context->mName, context);
    std::pair<Contexts::iterator, bool> result = mContexts.insert(item);

    if(!result.second)
    {
        delete context;
    }

    return result.first->second;
}

TimerPushPop::TimerPushPop(Tracing &tracing,
                           const std::string &name,
                           const std::string &category)
    : Timer(category)
    , mName(name)
    , mTracing(tracing)
{
    mTracing.beginScope(mName);
    begin();
}

TimerPushPop::~TimerPushPop()
{
    end();
    mTracing.add(*this);
    mTracing.endScope(mName);
}

Tracing &getTracing()
{
    static Tracing tracing("mdx");

    return tracing;
}

int getTracingId()
{
    static int sTracingId = 0;

    return sTracingId++;
}
