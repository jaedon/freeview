/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef MDXTRACING_H
#define MDXTRACING_H

#include <nrdbase/Mutex.h>
#include <nrdbase/Variant.h>
#include <string>
#include <map>
#include <sys/types.h>

#ifdef MDXLIB_HAVE_TRACING
#define TIMER(name, category) TimerPushPop(getTracing(), name, category)
#define EVENT(name, category, args) getTracing().addInstantEvent(name, category, args)
#define EVENT_START(name, id, category, stepName, args) getTracing().addEventStart(name, id, category, stepName, args)
#define EVENT_STEP(name, id, category, stepName, args) getTracing().addEventStep(name, id, category, stepName, args)
#define EVENT_FINISH(name, id, category, stepName, args) getTracing().addEventFinish(name, id, category, stepName, args)
#else
#define TIMER(name, category) do{} while(0)
#define EVENT(name, category, args)  do{} while(0)
#define EVENT_START(name, id, category, stepName, args)  do{} while(0)
#define EVENT_STEP(name, id, category, stepName, args) do {} while(0)
#define EVENT_FINISH(name, id, category, stepName, args)  do{} while(0)
#endif

#ifdef MDXLIB_HAVE_TRACING

namespace netflix {
class Variant;
}

class Timer
{
public:
    Timer(const std::string &category);
    virtual ~Timer();

    void begin();
    void end();

    std::string getCategory() const;
    ullong    getBegin() const;
    ullong    getEnd() const;

private:

    std::string mCategory;
    ullong    mBegin;
    ullong    mEnd;
};

class Tracing
{
public:

    Tracing(const std::string &name);
    ~Tracing();

    void enable();
    void disable();

    void beginScope(const std::string &name);
    void endScope(const std::string &name);

    void add(const Timer &timer);
    void addInstantEvent(const std::string &name,
                         const std::string &category,
                         const Variant *args = NULL);
    void addEventStart(const std::string &name,
                       ullong id,
                       const std::string &category,
                       const std::string &stepName,
                       const Variant *args = NULL);
    void addEventStep(const std::string &name,
                      ullong id,
                      const std::string &category,
                      const std::string &stepName,
                      const Variant *args = NULL);
    void addEventFinish(const std::string &name,
                        ullong id,
                        const std::string &category,
                        const std::string &stepName,
                        const Variant *args = NULL);
    uint32_t getReport(Variant &report);
    void clear();

private:

    struct Event;
    struct Scope;
    struct Context;
    typedef std::map<std::string, Scope *> Scopes;
    typedef std::vector<Event *> Events;

    struct Event
    {
        Event(const std::string &name,
              ullong id,
              const std::string &category,
              const std::string &stepName,
              ullong timeUS,
              const Variant *args = NULL);

        std::string             mName;
        ullong                mId;
        std::string             mCategory;
        ullong                mTime;
        Variant  mArgs;
    };

    // For tracking profile hierarchy
    struct Scope
    {
        Scope(const std::string &name);
        Scope(Scope *parent, const std::string &name);
        ~Scope();
        Scope *findAddScope(const std::string &name);

        void report(Variant &traceEvents) const;

        Scope *             mParent;
        std::string         mName;
        std::vector<Timer>  mTimers;
        Events              mInstantEvents;
        Events              mStartEvents;
        Events              mStepEvents;
        Events              mFinishEvents;
        Scopes              mChildren;
    };

    // For tracking per thread hierarchy
    struct Context
    {
        Context(const std::string &name);
        ~Context();

        const std::string   mName;
        std::string         mThreadName;
        uint32_t            mThreadId;
        uint32_t            mProcessId;
        Scope               mRootScope;
        Scope *             mCurrentScope;
    };
    typedef std::map<std::string, Context *> Contexts;

    Context *getContext();
    Context *findAddContext(const std::string &name);

    std::string             mName;
    Contexts                mContexts;
    Mutex    mContextMutex;
    bool                    mEnabled;
};

class TimerPushPop : public Timer
{
public:
    TimerPushPop(Tracing &timing,
                 const std::string &name,
                 const std::string &category);
    ~TimerPushPop();

private:

    std::string mName;
    Tracing &   mTracing;
};

Tracing &getTracing();
int getTracingId();
#endif

#endif

