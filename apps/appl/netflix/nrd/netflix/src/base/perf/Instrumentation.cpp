/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include <nrdbase/Instrumentation.h>
#include <nrdbase/Application.h>

//-----------------------------------------------------------------------------
// Everything else is conditional on instrumentation being enabled

NRDP_DEFINE_OBJECTCOUNT_RECORD(InstrumentationEventCount, InstrumentationEvent);

#ifdef INSTRUMENTATION_ENABLED

//-----------------------------------------------------------------------------
// Tracing behavior

#if 0
#define INTRACES(s) NTRACES(TRACE_INSTRUMENTATION,s)
#else
#define INTRACES(s) do{}while(0)
#endif

std::string InstrumentationEventCount::describe(void *object) const
{
    return static_cast<netflix::instrumentation::Event*>(object)->name;
}

//-----------------------------------------------------------------------------

namespace netflix {
namespace instrumentation {

static shared_ptr<Queue> sQueue;
static pthread_once_t sOnce = PTHREAD_ONCE_INIT;

static void init() {
    sQueue.reset(new Queue());
}

shared_ptr<Queue> getQueue() {
    pthread_once(&sOnce, init);
    return sQueue;
}

void pushed(const Event::SharedPtr &event)
{
    if(Application *app = Application::instance())
        app->instrumentation(event);
}

Variant pop_stash()
{
    Mutex mutex( UNTRACKED_MUTEX , "INSTRUMENTATION_STASH_MUTEX" );
    ConditionVariable condition;
    Variant events(Variant::Type_Array);
    bool done = false;

    ScopedMutex lock(mutex);
    push_front(Event::SharedPtr(new PopStashEvent(&events, &condition, &mutex, &done)));

    while(!done)
        condition.wait(&mutex);

    return events;
}

//=============================================================================

#ifdef INSTRUMENTATION_LOCAL_CAPTURE_ENABLED

LocalCapture::LocalCapture() : mCaptures(0), mQueue(getQueue()), mLastCommit(Time::mono())
{
    INTRACES("CREATING LOCAL CAPTURE");
}

LocalCapture::~LocalCapture()
{
    INTRACES("DESTROYING LOCAL CAPTURE");
    commit_internal();
}

void LocalCapture::push(Event * event)
{
    instance()->push_internal(event);
}

void LocalCapture::start()
{
    INTRACES( "STARTING LOCAL CAPTURE" );
    instance()->mCaptures++;
}

void LocalCapture::end()
{
    INTRACES("ENDING LOCAL CAPTURE");
    instance()->end_internal();
}

void LocalCapture::commit(ullong frequency)
{
    instance()->commit_internal(frequency);
}

instrumentation::LocalCapture * LocalCapture::instance()
{
    return ThreadLocalStore::getOrSetObject<LocalCapture>("InstrLC");
}

void LocalCapture::push_internal(const Event::SharedPtr &event)
{
    assert(event);
    if(mCaptures)
        mLocalList.push_back(event);
    else
        mQueue->push(event);
}

void LocalCapture::end_internal()
{
    assert(mCaptures);
    if(!--mCaptures)
        commit_internal();
}

void LocalCapture::commit_internal(ullong frequency)
{
    if(!mLocalList.empty()) {
        Time now = Time::mono();
        if(frequency && mLastCommit + frequency <= now)
            INTRACES("COMMITTING LOCAL CAPTURE BECAUSE FREQUENCY REACHED");
        else if(!frequency)
            INTRACES("FORCING COMMIT OF LOCAL CAPTURE");
        else
            return;

        INTRACES("  MOVING " << mLocalList.size() << " EVENT(S) TO REAL QUEUE");
        mQueue->push(mLocalList);
        mLocalList.clear();
        mLastCommit = now;
    }
}

#endif // INSTRUMENTATION_LOCAL_CAPTURE_ENABLED

}}

#endif // INSTRUMENTATION_ENABLED

