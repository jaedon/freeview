/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#ifndef _Instrumentation_h
#define _Instrumentation_h

#include <nrdbase/Variant.h>
#include <nrdbase/Time.h>
#include <nrdbase/AsyncQueue.h>
#include <nrdbase/StringFormatter.h>
#include <nrdbase/ThreadLocalStore.h>
#include <nrdbase/Thread.h>
#include <nrdbase/config.h>
#include <nrdbase/ObjectCount.h>

struct InstrumentationEventCount : public netflix::ObjectCount::Record
{
public:
    inline InstrumentationEventCount(const std::string &n) : netflix::ObjectCount::Record(n) { }
    inline virtual ~InstrumentationEventCount() { }
    virtual std::string describe(void *object) const;
};

NRDP_DECLARE_OBJECTCOUNT_RECORD(InstrumentationEventCount, InstrumentationEvent);

//-----------------------------------------------------------------------------

#if !defined(NRDP_HAS_INSTRUMENTATION)
# undef NRDP_HAS_ON_INSTRUMENTATION
# undef NRDP_HAS_SWITCHED_INSTRUMENTATION
# undef NRDP_HAS_TEST_INSTRUMENTATION
# undef NRDP_HAS_DEBUG_INSTRUMENTATION
#elif defined(NRDP_HAS_ON_INSTRUMENTATION) || \
    defined(NRDP_HAS_SWITCHED_INSTRUMENTATION) || \
    defined(NRDP_HAS_TEST_INSTRUMENTATION) || \
    defined(NRDP_HAS_DEBUG_INSTRUMENTATION)
#define INSTRUMENTATION_ENABLED             1
#endif
#define INSTRUMENTATION_NOOP do{}while(0)

//-----------------------------------------------------------------------------
// Don't use this class directly - only use the macros at the end
//-----------------------------------------------------------------------------
// If none of the categories are in, disable everything

#ifdef INSTRUMENTATION_ENABLED

//.............................................................................
// This is experimental code that captures events to thread local store
// and then flushes them to the real queue. It seems to work fine but
// I'm not sure it is worth the complexity

//#define INSTRUMENTATION_LOCAL_CAPTURE_ENABLED   1

//.............................................................................

namespace netflix {
namespace instrumentation {

    class Context
    {
    public:
        inline Context(const std::string & _name) : name(_name) {}
        inline Context(const std::string & _name, const void *_ctx) : name(StringFormatter::sformat("%s/%p", _name.c_str(), _ctx)) {}
        inline Context(const std::string & _name, const std::string &_ctx) : name(StringFormatter::sformat("%s/%s" , _name.c_str(), _ctx.c_str())) {}
        const std::string name;
    };

    enum Type {
        On = 0, Switched = 1, Test = 2, Debug = 3,
        /* Special */
        Config         = 10,
        Flush          = 11,
        Disable        = 12,
        Enable         = 13,
        SwitchedOn     = 14,
        SwitchedOff    = 15,
        StashOn        = 16,
        StashOff       = 17,
        PopStash       = 18,
        AddListener    = 19,
        RemoveListener = 20
    };

    //-------------------------------------------------------------------------

    class Event;

    typedef AsyncQueue<Event> Queue;

    //-------------------------------------------------------------------------

    shared_ptr<Queue> getQueue();

    //-------------------------------------------------------------------------

    class Listener
    {
    public:
        Listener() {}
        virtual ~Listener() {}
        virtual void receive(const Variant & event) = 0;
        virtual void verboseChanged(bool verbose) { NRDP_UNUSED(verbose); }
    };

    //-------------------------------------------------------------------------

    enum IntervalOp { None, Start, Count, Tag, Mark, CountMark, End, Cancel };

    class Event
    {
    public:
        typedef shared_ptr<Event> SharedPtr;

        static inline Event::SharedPtr make(Type type, ullong mono , const std::string & name, const Variant & value, const uint32_t thread = 0)
        {
            return Event::SharedPtr(new Event(type, mono, name, value, thread));
        }

        static inline Event::SharedPtr make_simple(Type type)
        {
            return Event::SharedPtr(new Event(type));
        }

        static inline Event::SharedPtr make_interval_start(Type type, ullong mono, const std::string &name, const Variant &value, ullong reset = 0)
        {
            Event::SharedPtr result(new Event(type, mono, name, value));
            result->intervalOp = Start;
            result->intervalReset = reset;
            return result;
        }

        static inline Event::SharedPtr make_interval_count(Type type, const std::string &name, const std::string &counter, llong increment)
        {
            assert(!counter.empty());
            Event::SharedPtr result(new Event(type, 0, name));
            result->intervalOp = Count;
            result->intervalCounter = counter;
            result->intervalCountIncrement = increment;
            return result;
        }

        static inline Event::SharedPtr make_interval_tag(Type type, const std::string &name, const Variant &value)
        {
            Event::SharedPtr result(new Event(type, 0, name, value));
            result->intervalOp = Tag;
            return result;
        }

        static inline Event::SharedPtr make_interval_mark(Type type, ullong mono, bool count, const std::string &name, const std::string &mark, const Variant & value)
        {
            assert(!mark.empty());
            Event::SharedPtr result(new Event(type, mono, name, value));
            result->intervalOp = count ? CountMark : Mark;
            result->intervalCounter = mark;
            return result;
        }

        static inline Event::SharedPtr make_interval_end(Type type, ullong mono, const std::string &name, const Variant &value)
        {
            Event::SharedPtr result(new Event(type, mono, name, value));
            result->intervalOp = End;
            return result;
        }

        static inline Event::SharedPtr make_interval_cancel(Type type , const std::string &name)
        {
            Event::SharedPtr result(new Event(type, 0, name));
            result->intervalOp = Cancel;
            return result;
        }

        static inline Event::SharedPtr make_listener(Type type, shared_ptr<Listener> listener)
        {
            Event::SharedPtr result = make_simple(type);
            result->listener = listener;
            return result;
        }

        virtual ~Event()
        {
            NRDP_OBJECTCOUNT_DEREF(InstrumentationEvent, this);
        }

        const ullong            mono;
        const uint32_t          thread;
        const Type              type;
        const std::string       name;
        const Variant           value;

        IntervalOp              intervalOp;
        ullong                intervalReset;
        std::string             intervalCounter; // or mark, or both
        llong                 intervalCountIncrement;
        shared_ptr<Listener>    listener;

    protected:

        inline Event(Type _type) : mono(0), thread(0), type(_type), intervalOp(None),
                                   intervalReset(0), intervalCountIncrement(1)
        {
            NRDP_OBJECTCOUNT_REF(InstrumentationEvent, this);
        }

        inline Event(Type _type, ullong _mono, const std::string &_name) : mono(_mono), thread(Thread::CurrentThreadId()),
                                                                                type(_type), name(_name), intervalOp(None),
                                                                                intervalReset(0), intervalCountIncrement(1)
        {
            NRDP_OBJECTCOUNT_REF(InstrumentationEvent, this);
            assert( value.isNull() || value.isStringMap() );
            assert( ! name.empty() );
        }

        explicit inline Event( Type _type , ullong _mono , const std::string & _name , const Variant & _value , const uint32_t _thread = 0 ) :
            mono(_mono), thread(_thread ? _thread : Thread::CurrentThreadId()), type(_type), name(_name), value(_value),
            intervalOp(None), intervalReset(0), intervalCountIncrement(1)
        {
            NRDP_OBJECTCOUNT_REF(InstrumentationEvent, this);
            assert(!name.empty());
        }
    };

    void pushed(const Event::SharedPtr &event);
    inline void push_back(const Event::SharedPtr &event)
    {
#ifdef INSTRUMENTATION_LOCAL_CAPTURE_ENABLED
        LocalCapture::push(event);
#else
        getQueue()->push(event);
#endif
        pushed(event);
    }
    inline void push_front(const Event::SharedPtr &event)
    {
        getQueue()->push_front(event);
        pushed(event);
    }

    inline void event(Type type, ullong mono, const Context &ctx, const Variant &value = Variant::null())
    {
        push_back(Event::make(type, mono, ctx.name, value));
    }

    inline void event(Type type, ullong mono, const char *name, const Variant & value = Variant::null())
    {
        push_back(Event::make(type, mono, name, value));
    }

    inline void interval_start(Type type, ullong mono, const Context & ctx, const Variant &value = Variant::null(), unsigned resetSeconds = 0)
    {
        push_back(Event::make_interval_start(type, mono, ctx.name, value, Time::fromSeconds(resetSeconds).ms()));
    }

    inline void interval_start(Type type, ullong mono, const char *name, const Variant &value = Variant::null(), unsigned resetSeconds = 0)
    {
        push_back(Event::make_interval_start(type, mono, name, value, Time::fromSeconds(resetSeconds).ms()));
    }

    inline void interval_count(Type type, const Context &ctx, const std::string &counter, llong increment = 1)
    {
        push_back(Event::make_interval_count(type, ctx.name, counter, increment));
    }

    inline void interval_count(Type type, const char *name, const std::string &counter, llong increment = 1)
    {
        push_back(Event::make_interval_count(type , name , counter , increment));
    }

    inline void interval_tag(Type type, const Context &ctx, const Variant &value)
    {
        push_back(Event::make_interval_tag(type, ctx.name, value));
    }

    inline void interval_tag(Type type, const char *name, const Variant &value)
    {
        push_back(Event::make_interval_tag(type, name, value));
    }

    inline void interval_mark(Type type, ullong mono, bool count, const Context &ctx, const std::string &mark, const Variant &value = Variant::null())
    {
        push_back(Event::make_interval_mark(type, mono, count, ctx.name, mark, value));
    }
    inline void interval_mark(Type type, ullong mono, bool count, const char *name, const std::string &mark, const Variant &value = Variant::null())
    {
        push_back(Event::make_interval_mark(type, mono, count, name, mark, value));
    }

    inline void interval_end(Type type, ullong mono, const Context &ctx, const Variant &value = Variant::null())
    {
        push_back(Event::make_interval_end(type, mono, ctx.name, value));
    }

    inline void interval_end(Type type, ullong mono, const char *name, const Variant &value = Variant::null())
    {
        push_back(Event::make_interval_end(type, mono, name, value));
    }

    inline void interval_cancel(Type type, const Context &ctx)
    {
        push_back(Event::make_interval_cancel(type, ctx.name));
    }

    inline void interval_cancel(Type type , const char * name)
    {
        push_back(Event::make_interval_cancel(type, name));
    }

    inline void flush()
    {
        push_front(Event::make_simple(Flush));
    }

    inline void disable()
    {
        push_front(Event::make_simple(Disable));
    }

    inline void enable()
    {
        push_front(Event::make_simple(Enable));
    }

    inline void config()
    {
        push_front(Event::make_simple(Config));
    }

    inline void switched(bool on)
    {
        push_front(Event::make_simple(on ? SwitchedOn : SwitchedOff));
    }

    inline void stash( bool on )
    {
        push_front(Event::make_simple(on ? StashOn : StashOff));
    }

    Variant pop_stash();

    inline void addListener(shared_ptr<Listener> listener)
    {
        push_front(Event::make_listener(AddListener, listener));
    }

    inline void removeListener(shared_ptr<Listener> listener)
    {
        push_front(Event::make_listener(RemoveListener, listener));
    }


#ifdef INSTRUMENTATION_LOCAL_CAPTURE_ENABLED
    inline void capture_start()
    {
        LocalCapture::start();
    }

    inline void capture_commit(ullong frequency = 0)
    {
        LocalCapture::commit(frequency);
    }

    inline void capture_end()
    {
        LocalCapture::end();
    }
#endif

    class ScopedInterval
    {
    public:
        inline ScopedInterval(Type type, ullong mono, const char *_name, const Variant &value = Variant::null()) : mContext(_name), mType(type)
        {
            interval_start(type, mono, mContext, value);
        }
        inline ScopedInterval(Type type, ullong mono, const Context &ctx, const Variant &value = Variant::null()) : mContext(ctx), mType(type)
        {
            interval_start(type, mono, mContext, value);
        }
        inline ~ScopedInterval()
        {
            interval_end(mType, Time::monoMS(), mContext, mValue);
        }
        inline void tag(const Variant & value)
        {
            if(mValue.isNull()) {
                mValue = value;
            } else {
                const Variant::StringMapIterator end = value.stringMapEnd();
                for(Variant::StringMapIterator it = value.stringMapBegin(); it != end; ++it)
                    mValue[it->first] = it->second;
            }
        }
    private:
        Context     mContext;
        Type        mType;
        Variant     mValue;
    };

    //-------------------------------------------------------------------------

    class PopStashEvent : public Event
    {
    public:

        inline PopStashEvent(Variant *events, ConditionVariable *condition, Mutex *mutex, bool *done) : Event(PopStash), mEvents(events),
                                                                                                        mCondition(condition), mMutex(mutex),
                                                                                                        mDone(done)
        {
            assert(mEvents);
            assert(mCondition);
            assert(mutex);
            assert(done);
        }

        virtual ~PopStashEvent()
        {
            ScopedMutex lock(*mMutex);
            *mDone = true;
            mCondition->broadcast();
        }

        inline void swap(Variant  &source)
        {
            std::swap(source, *mEvents);
        }

    private:
        Variant *            mEvents;
        ConditionVariable *  mCondition;
        Mutex *              mMutex;
        bool *               mDone;
    };

#ifdef INSTRUMENTATION_LOCAL_CAPTURE_ENABLED

    class LocalCapture : public ThreadLocalObject
    {
    public:
        LocalCapture();
        ~LocalCapture();

        static void push(Event * event);
        static void start();
        static void end();
        static void commit(ullong frequency);

    private:

        static LocalCapture * instance();

        void push_internal(Event * event);
        void end_internal();
        void commit_internal(ullong frequency = 0);

        ullong            mCaptures;
        shared_ptr<Queue>   mQueue;
        Queue::List         mLocalList;
        Time                mLastCommit;
    };

#endif

}} // instrumentation

//-----------------------------------------------------------------------------
// These are internal, to make the other macros shorter
#define INSTRUMENTATION_EVENT(type,...)                 instrumentation::event(instrumentation::type,Time::monoMS(),__VA_ARGS__)
#define INSTRUMENTATION_INTERVAL_START(type,...)        instrumentation::interval_start(instrumentation::type,Time::monoMS(),__VA_ARGS__)
#define INSTRUMENTATION_INTERVAL_COUNT(type,...)        instrumentation::interval_count(instrumentation::type,__VA_ARGS__)
#define INSTRUMENTATION_INTERVAL_TAG(type,...)          instrumentation::interval_tag(instrumentation::type,__VA_ARGS__)
#define INSTRUMENTATION_INTERVAL_MARK(type,...)         instrumentation::interval_mark(instrumentation::type,Time::monoMS(),false,__VA_ARGS__)
#define INSTRUMENTATION_INTERVAL_COUNT_MARK(type,...)   instrumentation::interval_mark(instrumentation::type,Time::monoMS(),true,__VA_ARGS__)
#define INSTRUMENTATION_INTERVAL_END(type,...)          instrumentation::interval_end(instrumentation::type,Time::monoMS(),__VA_ARGS__)
#define INSTRUMENTATION_INTERVAL_CANCEL(type,...)       instrumentation::interval_cancel(instrumentation::type,__VA_ARGS__)
#define INSTRUMENTATION_INTERVAL_SCOPED(v,type,...)     instrumentation::ScopedInterval _##v_(instrumentation::type,Time::monoMS(),__VA_ARGS__)
#define INSTRUMENTATION_INTERVAL_SCOPED_TAG(v,...)      _##v_.tag(__VA_ARGS__)
//-----------------------------------------------------------------------------

#endif // INSTRUMENTATION_ENABLED

//-----------------------------------------------------------------------------

#ifdef INSTRUMENTATION_ENABLED
#define INSTRUMENTATION_IF(cond, ...)                       if(true) __VA_ARGS__
#define INSTRUMENTATION_FLUSH()                             instrumentation::flush()
#define INSTRUMENTATION_DISABLE()                           instrumentation::disable()
#define INSTRUMENTATION_ENABLE()                            instrumentation::enable()
#define INSTRUMENTATION_CTX(...)                            instrumentation::Context(__VA_ARGS__)
#define INSTRUMENTATION_ADD_LISTENER(listener)              instrumentation::addListener(listener)
#define INSTRUMENTATION_REMOVE_LISTENER(listener)           instrumentation::removeListener(listener)
#else
#define INSTRUMENTATION_IF(cond, ...)                       INSTRUMENTATION_NOOP
#define INSTRUMENTATION_FLUSH()                             INSTRUMENTATION_NOOP
#define INSTRUMENTATION_DISABLE()                           INSTRUMENTATION_NOOP
#define INSTRUMENTATION_ENABLE()                            INSTRUMENTATION_NOOP
#define INSTRUMENTATION_CTX(...)                            ("")
#define INSTRUMENTATION_ADD_LISTENER(listener)              INSTRUMENTATION_NOOP
#define INSTRUMENTATION_REMOVE_LISTENER(listener)           INSTRUMENTATION_NOOP
#endif

#ifdef INSTRUMENTATION_LOCAL_CAPTURE_ENABLED
#define INSTRUMENTATION_CAPTURE_START()                     instrumentation::capture_start()
#define INSTRUMENTATION_CAPTURE_COMMIT(...)                 instrumentation::capture_commit(__VA_ARGS__)
#define INSTRUMENTATION_CAPTURE_END()                       instrumentation::capture_end()
#else
#define INSTRUMENTATION_CAPTURE_START()                     INSTRUMENTATION_NOOP
#define INSTRUMENTATION_CAPTURE_COMMIT(...)                 INSTRUMENTATION_NOOP
#define INSTRUMENTATION_CAPTURE_END()                       INSTRUMENTATION_NOOP
#endif

// EVENT( name [ , variant ] )
// INTERVAL_START( name [ , payload [ , frequency seconds ] ] )
// INTERVAL_COUNT( name , counter name [ , increment ] )
// INTERVAL_TAG( name , variant )
// INTERVAL_END( name [ , variant ] )
// INTERVAL_CANCEL( name )
//
// name can be replaced with INSTRUMENTATION_CTX( name , <something else> )

#if defined( NRDP_HAS_ON_INSTRUMENTATION )
#define INSTRUMENTATION_EVENT_ON(...)                       INSTRUMENTATION_EVENT(On,__VA_ARGS__)
#define INSTRUMENTATION_INTERVAL_ON_START(...)              INSTRUMENTATION_INTERVAL_START(On,__VA_ARGS__)
#define INSTRUMENTATION_INTERVAL_ON_COUNT(...)              INSTRUMENTATION_INTERVAL_COUNT(On,__VA_ARGS__)
#define INSTRUMENTATION_INTERVAL_ON_TAG(...)                INSTRUMENTATION_INTERVAL_TAG(On,__VA_ARGS__)
#define INSTRUMENTATION_INTERVAL_ON_MARK(...)               INSTRUMENTATION_INTERVAL_MARK(On,__VA_ARGS__)
#define INSTRUMENTATION_INTERVAL_ON_COUNT_MARK(...)         INSTRUMENTATION_INTERVAL_COUNT_MARK(On,__VA_ARGS__)
#define INSTRUMENTATION_INTERVAL_ON_END(...)                INSTRUMENTATION_INTERVAL_END(On,__VA_ARGS__)
#define INSTRUMENTATION_INTERVAL_ON_CANCEL(...)             INSTRUMENTATION_INTERVAL_CANCEL(On,__VA_ARGS__)
#define INSTRUMENTATION_INTERVAL_ON_SCOPED(v,...)           INSTRUMENTATION_INTERVAL_SCOPED(v,On,__VA_ARGS__)
#define INSTRUMENTATION_INTERVAL_ON_SCOPED_TAG(v,...)       INSTRUMENTATION_INTERVAL_SCOPED_TAG(v,__VA_ARGS__)
#else
#define INSTRUMENTATION_EVENT_ON(...)                       INSTRUMENTATION_NOOP
#define INSTRUMENTATION_INTERVAL_ON_START(...)              INSTRUMENTATION_NOOP
#define INSTRUMENTATION_INTERVAL_ON_COUNT(...)              INSTRUMENTATION_NOOP
#define INSTRUMENTATION_INTERVAL_ON_TAG(...)                INSTRUMENTATION_NOOP
#define INSTRUMENTATION_INTERVAL_ON_MARK(...)               INSTRUMENTATION_NOOP
#define INSTRUMENTATION_INTERVAL_ON_COUNT_MARK(...)         INSTRUMENTATION_NOOP
#define INSTRUMENTATION_INTERVAL_ON_END(...)                INSTRUMENTATION_NOOP
#define INSTRUMENTATION_INTERVAL_ON_CANCEL(...)             INSTRUMENTATION_NOOP
#define INSTRUMENTATION_INTERVAL_ON_SCOPED(v,...)           INSTRUMENTATION_NOOP
#define INSTRUMENTATION_INTERVAL_ON_SCOPED_TAG(v,...)       INSTRUMENTATION_NOOP
#endif

#if defined( NRDP_HAS_SWITCHED_INSTRUMENTATION )
#define INSTRUMENTATION_EVENT_SWITCHED(...)                 INSTRUMENTATION_EVENT(Switched,__VA_ARGS__)
#define INSTRUMENTATION_INTERVAL_SWITCHED_START(...)        INSTRUMENTATION_INTERVAL_START(Switched,__VA_ARGS__)
#define INSTRUMENTATION_INTERVAL_SWITCHED_COUNT(...)        INSTRUMENTATION_INTERVAL_COUNT(Switched,__VA_ARGS__)
#define INSTRUMENTATION_INTERVAL_SWITCHED_TAG(...)          INSTRUMENTATION_INTERVAL_TAG(Switched,__VA_ARGS__)
#define INSTRUMENTATION_INTERVAL_SWITCHED_MARK(...)         INSTRUMENTATION_INTERVAL_MARK(Switched,__VA_ARGS__)
#define INSTRUMENTATION_INTERVAL_SWITCHED_COUNT_MARK(...)   INSTRUMENTATION_INTERVAL_COUNT_MARK(Switched,__VA_ARGS__)
#define INSTRUMENTATION_INTERVAL_SWITCHED_END(...)          INSTRUMENTATION_INTERVAL_END(Switched,__VA_ARGS__)
#define INSTRUMENTATION_INTERVAL_SWITCHED_CANCEL(...)       INSTRUMENTATION_INTERVAL_CANCEL(Switched,__VA_ARGS__)
#define INSTRUMENTATION_INTERVAL_SWITCHED_SCOPED(v,...)     INSTRUMENTATION_INTERVAL_SCOPED(v,Switched,__VA_ARGS__)
#define INSTRUMENTATION_INTERVAL_SWITCHED_SCOPED_TAG(v,...) INSTRUMENTATION_INTERVAL_SCOPED_TAG(v,__VA_ARGS__)
#else
#define INSTRUMENTATION_EVENT_SWITCHED(...)                 INSTRUMENTATION_NOOP
#define INSTRUMENTATION_INTERVAL_SWITCHED_START(...)        INSTRUMENTATION_NOOP
#define INSTRUMENTATION_INTERVAL_SWITCHED_COUNT(...)        INSTRUMENTATION_NOOP
#define INSTRUMENTATION_INTERVAL_SWITCHED_TAG(...)          INSTRUMENTATION_NOOP
#define INSTRUMENTATION_INTERVAL_SWITCHED_MARK(...)         INSTRUMENTATION_NOOP
#define INSTRUMENTATION_INTERVAL_SWITCHED_COUNT_MARK(...)   INSTRUMENTATION_NOOP
#define INSTRUMENTATION_INTERVAL_SWITCHED_END(...)          INSTRUMENTATION_NOOP
#define INSTRUMENTATION_INTERVAL_SWITCHED_CANCEL(...)       INSTRUMENTATION_NOOP
#define INSTRUMENTATION_INTERVAL_SWITCHED_SCOPED(v,...)     INSTRUMENTATION_NOOP
#define INSTRUMENTATION_INTERVAL_SWITCHED_SCOPED_TAG(v,...) INSTRUMENTATION_NOOP
#endif

#if defined( NRDP_HAS_TEST_INSTRUMENTATION )
#define INSTRUMENTATION_EVENT_TEST(...)                     INSTRUMENTATION_EVENT(Test,__VA_ARGS__)
#define INSTRUMENTATION_INTERVAL_TEST_START(...)            INSTRUMENTATION_INTERVAL_START(Test,__VA_ARGS__)
#define INSTRUMENTATION_INTERVAL_TEST_COUNT(...)            INSTRUMENTATION_INTERVAL_COUNT(Test,__VA_ARGS__)
#define INSTRUMENTATION_INTERVAL_TEST_TAG(...)              INSTRUMENTATION_INTERVAL_TAG(Test,__VA_ARGS__)
#define INSTRUMENTATION_INTERVAL_TEST_MARK(...)             INSTRUMENTATION_INTERVAL_MARK(Test,__VA_ARGS__)
#define INSTRUMENTATION_INTERVAL_TEST_COUNT_MARK(...)       INSTRUMENTATION_INTERVAL_COUNT_MARK(Test,__VA_ARGS__)
#define INSTRUMENTATION_INTERVAL_TEST_END(...)              INSTRUMENTATION_INTERVAL_END(Test,__VA_ARGS__)
#define INSTRUMENTATION_INTERVAL_TEST_CANCEL(...)           INSTRUMENTATION_INTERVAL_CANCEL(Test,__VA_ARGS__)
#define INSTRUMENTATION_INTERVAL_TEST_SCOPED(v,...)         INSTRUMENTATION_INTERVAL_SCOPED(v,Test,__VA_ARGS__)
#define INSTRUMENTATION_INTERVAL_TEST_SCOPED_TAG(v,...)     INSTRUMENTATION_INTERVAL_SCOPED_TAG(v,__VA_ARGS__)
#else
#define INSTRUMENTATION_EVENT_TEST(...)                     INSTRUMENTATION_NOOP
#define INSTRUMENTATION_INTERVAL_TEST_START(...)            INSTRUMENTATION_NOOP
#define INSTRUMENTATION_INTERVAL_TEST_COUNT(...)            INSTRUMENTATION_NOOP
#define INSTRUMENTATION_INTERVAL_TEST_TAG(...)              INSTRUMENTATION_NOOP
#define INSTRUMENTATION_INTERVAL_TEST_MARK(...)             INSTRUMENTATION_NOOP
#define INSTRUMENTATION_INTERVAL_TEST_COUNT_MARK(...)       INSTRUMENTATION_NOOP
#define INSTRUMENTATION_INTERVAL_TEST_END(...)              INSTRUMENTATION_NOOP
#define INSTRUMENTATION_INTERVAL_TEST_CANCEL(...)           INSTRUMENTATION_NOOP
#define INSTRUMENTATION_INTERVAL_TEST_SCOPED(v,...)         INSTRUMENTATION_NOOP
#define INSTRUMENTATION_INTERVAL_TEST_SCOPED_TAG(v,...)     INSTRUMENTATION_NOOP
#endif

#if defined( NRDP_HAS_DEBUG_INSTRUMENTATION )
#define INSTRUMENTATION_EVENT_DEBUG(...)                    INSTRUMENTATION_EVENT(Debug,__VA_ARGS__)
#define INSTRUMENTATION_INTERVAL_DEBUG_START(...)           INSTRUMENTATION_INTERVAL_START(Debug,__VA_ARGS__)
#define INSTRUMENTATION_INTERVAL_DEBUG_COUNT(...)           INSTRUMENTATION_INTERVAL_COUNT(Debug,__VA_ARGS__)
#define INSTRUMENTATION_INTERVAL_DEBUG_TAG(...)             INSTRUMENTATION_INTERVAL_TAG(Debug,__VA_ARGS__)
#define INSTRUMENTATION_INTERVAL_DEBUG_MARK(...)            INSTRUMENTATION_INTERVAL_MARK(Debug,__VA_ARGS__)
#define INSTRUMENTATION_INTERVAL_DEBUG_COUNT_MARK(...)      INSTRUMENTATION_INTERVAL_COUNT_MARK(Debug,__VA_ARGS__)
#define INSTRUMENTATION_INTERVAL_DEBUG_END(...)             INSTRUMENTATION_INTERVAL_END(Debug,__VA_ARGS__)
#define INSTRUMENTATION_INTERVAL_DEBUG_CANCEL(...)          INSTRUMENTATION_INTERVAL_CANCEL(Debug,__VA_ARGS__)
#define INSTRUMENTATION_INTERVAL_DEBUG_SCOPED(v,...)        INSTRUMENTATION_INTERVAL_SCOPED(v,Debug,__VA_ARGS__)
#define INSTRUMENTATION_INTERVAL_DEBUG_SCOPED_TAG(v,...)    INSTRUMENTATION_INTERVAL_SCOPED_TAG(v,__VA_ARGS__)
#else
#define INSTRUMENTATION_EVENT_DEBUG(...)                    INSTRUMENTATION_NOOP
#define INSTRUMENTATION_INTERVAL_DEBUG_START(...)           INSTRUMENTATION_NOOP
#define INSTRUMENTATION_INTERVAL_DEBUG_COUNT(...)           INSTRUMENTATION_NOOP
#define INSTRUMENTATION_INTERVAL_DEBUG_TAG(...)             INSTRUMENTATION_NOOP
#define INSTRUMENTATION_INTERVAL_DEBUG_MARK(...)            INSTRUMENTATION_NOOP
#define INSTRUMENTATION_INTERVAL_DEBUG_COUNT_MARK(...)      INSTRUMENTATION_NOOP
#define INSTRUMENTATION_INTERVAL_DEBUG_END(...)             INSTRUMENTATION_NOOP
#define INSTRUMENTATION_INTERVAL_DEBUG_CANCEL(...)          INSTRUMENTATION_NOOP
#define INSTRUMENTATION_INTERVAL_DEBUG_SCOPED(v,...)        INSTRUMENTATION_NOOP
#define INSTRUMENTATION_INTERVAL_DEBUG_SCOPED_TAG(v,...)    INSTRUMENTATION_NOOP
#endif

#endif // _Instrumentation_h
