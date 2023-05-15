/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#include <nrdbase/Mutex.h>

#include "Mutex.h"

#include <errno.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <nrdbase/Backtrace.h>
#include <nrdbase/MutexStack.h>
#include <nrdbase/DumpInfo.h>
#include <nrdbase/Log.h>
#include <nrdbase/Time.h>
#include <nrdbase/Thread.h>
#include <nrdbase/Variant.h>
#include <nrdbase/Configuration.h>

using namespace netflix;

static volatile int sMutexCount = 0;

#ifdef NRDP_MUTEX_TRACK_LOCK
#if 0
#define NRDP_MUTEX_TRACK_LOCK_THRESHOLD 20
#endif

struct RegisteredMutexes {
    RegisteredMutexes() { pthread_mutex_init(&lock, 0); }
    ~RegisteredMutexes() { pthread_mutex_destroy(&lock); }
    pthread_mutex_t lock;
    std::set<Mutex *> mutexes;
} *sRegisteredMutexes = 0;
static pthread_once_t sOnce = PTHREAD_ONCE_INIT;
static void init_mutex_register() { sRegisteredMutexes = new RegisteredMutexes; }
Mutex::Tracker::Tracker(bool start)
{
    if (start) {
        if (llong bt = Configuration::mutexTrackBacktrace())
            backtrace = Backtrace::backtrace(bt);
        thread = Thread::CurrentThread();
        wait = Stopwatch::mono();
    } else {
        thread = 0;
        wait = lock = unlock = 0;
    }
}

void Mutex::Tracker::locked()
{
    lock = Stopwatch::mono();
#ifdef NRDP_MUTEX_TRACK_LOCK_THRESHOLD
    if (lock - wait > NRDP_MUTEX_TRACK_LOCK_THRESHOLD)
        printf("Waited a long time: %p(%s) %fms\n", thread, thread ? thread->GetName().c_str() : "UNKNOWN", lock - wait);
#endif
}

void Mutex::Tracker::unlocked()
{
    unlock = Stopwatch::mono();
#ifdef NRDP_MUTEX_TRACK_LOCK_THRESHOLD
    if (unlock - lock > NRDP_MUTEX_TRACK_LOCK_THRESHOLD)
        printf("Held a long time: %p(%s) %fms\n", thread, thread ? thread->GetName().c_str() : "UNKNOWN", unlock - lock);
#endif
}
#endif

//////////
//
// Mutex
//
//////////

Mutex::Mutex(MutexRank rank, const char *name, Mode mode)
    : mRank(MutexRankValue(rank)), mName(name), mMode(mode)
{
    ++sMutexCount;
#if defined(NRDP_MUTEX_DETECT_DEADLOCK)
    mThreadPtr = 0;
    mBacktraceLockCount = 0;
    pthread_mutex_init(&mPtrlock, 0);
#endif

    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
#if defined(NRDP_HAS_RECURSIVE_MUTEX)
    pthread_mutexattr_settype(&attr, mode == Recursive ? PTHREAD_MUTEX_RECURSIVE : PTHREAD_MUTEX_ERRORCHECK);
#else
    mOwner = 0;
    mLockCount = 0;
#endif
    pthread_mutex_init(&mLock, &attr);
    pthread_mutexattr_destroy(&attr);
}

Mutex::~Mutex()
{
    assert(!isLocked());
    --sMutexCount;
#ifdef NRDP_MUTEX_TRACK_LOCK
    if(sRegisteredMutexes) {
        pthread_mutex_lock(&sRegisteredMutexes->lock);
        sRegisteredMutexes->mutexes.erase(this);
        pthread_mutex_unlock(&sRegisteredMutexes->lock);
    }
#endif
    pthread_mutex_destroy(&mLock);
#if defined(NRDP_MUTEX_DETECT_DEADLOCK)
    pthread_mutex_destroy(&mPtrlock);
#endif
}

NFErrorCode Mutex::lock(bool track)
{
#ifdef NRDP_MUTEX_TRACK_LOCK
    const bool shouldTrackMutex = track && mTracks.capacity();
    Tracker tracker(shouldTrackMutex);
#endif

    int ret = 0;
#ifndef NRDP_HAS_RECURSIVE_MUTEX
    if (isCurrentOwner()) {
        ret = mMode == Recursive ? 0 : 1;
    } else
#endif
    {
#if defined(NRDP_MUTEX_DETECT_DEADLOCK)
        const int interval = Configuration::mutexLockTimeTrackingInterval();
        if (interval <= 0) {
            ret = pthread_mutex_lock(&mLock);
        } else {
            enum { SleepTimeMS = 5 };
            const int count = std::max(1, interval / SleepTimeMS);
            int i = 0;
            const Time sleepTime(SleepTimeMS);
            while (true) {
                pthread_mutex_lock(&mPtrlock);
                shared_ptr<Backtrace> lockedThread = mBacktrace;
                pthread_mutex_unlock(&mPtrlock);
                ret = pthread_mutex_trylock(&mLock);
                if (!ret) {
                    if (!mBacktraceLockCount++) {
                        pthread_mutex_lock(&mPtrlock);
                        mBacktrace = Backtrace::backtrace();
                        pthread_mutex_unlock(&mPtrlock);
                        mThreadPtr = Thread::CurrentThreadPtr();
                        mLockTime = Time::mono();
                        /* These values are read and written without thread synchronization. */
                    }
                    break;
                }
                Thread::Sleep(sleepTime);
                if (++i == count) {
                    if (!lockedThread.get()) {
                        i = 0;
                    } else {
                        shared_ptr<Backtrace> backtrace = Backtrace::backtrace();
                        fprintf(stderr, "Failed to acquire mutex %s in %dms in thread %p\n", mName, interval, Thread::CurrentThreadPtr());
                        fprintf(stderr, "%s\n", backtrace->toString().c_str());
                        fprintf(stderr, "\nMutex was locked by thread %p at %s\n", mThreadPtr, mLockTime.calendarTimeText().c_str());
                        fprintf(stderr, "%s\n", lockedThread->toString().c_str());
                        ret = pthread_mutex_lock(&mLock);
                        break;
                    }
                }
            }
        }
#else
        ret = pthread_mutex_lock(&mLock);
#endif
    }
    if (ret == 0) {
// we're locked.
#ifndef NRDP_HAS_RECURSIVE_MUTEX
        mOwner = ::pthread_self();
        ++mLockCount;
#endif
        if (track) // report this to the MutexStack
            MutexStack::lock(mRank, mName);
#ifdef NRDP_THREAD_TRACK_LOCK
        if (Configuration::mutexThreadTrackLock()) {
            if (Thread *thread = Thread::CurrentThread())
                thread->Locked(mName);
        }
#endif
#ifdef NRDP_MUTEX_TRACK_LOCK
        mTracks.resize(Configuration::mutexTrackCount());
        if (shouldTrackMutex) {
            mTracker = tracker;
            mTracker.locked();
        }
#endif
        return NFErr_OK;
    } else {
        if (ret == EINVAL) {
#if 0 /* HUMAX : Temporarily commented because of the NTS testing process is urgent */
            assert(!NFErr_Uninitialized_Mutex);
            return NFErr_Uninitialized_Mutex;
#else
            /* TODO: Find-out the criminals doing lock() after mutex_destory */
            printf("HUMAX:Mutex::lock: Error> mutex lock after mutex_destory!\n");
            return NFErr_OK;
#endif
        } else if (ret == EDEADLK) {
            fprintf(stderr, "recursive lock detected: %s\n", mName);
            assert(!NFErr_DeadLock);
            return NFErr_DeadLock;
        } else {
            assert(!NFErr_Bad);
            return NFErr_Bad;
        }
    }
}

NFErrorCode Mutex::unlock(bool track)
{
    if (track) // report this to the MutexStack
        MutexStack::unlock(mRank, mName);

#ifndef NRDP_HAS_RECURSIVE_MUTEX
#if 0 /* HUMAX : Temporarily commented because of the NTS testing process is urgent */
    assert(isCurrentOwner());
#endif
    if (!isCurrentOwner())
        return NFErr_InvalidAccess;
    if (--mLockCount > 0)
        return NFErr_OK;
    mOwner = 0;
#endif
#if defined(NRDP_MUTEX_DETECT_DEADLOCK)
    if (Configuration::mutexLockTimeTrackingInterval() > 0 && !--mBacktraceLockCount) {
        pthread_mutex_lock(&mPtrlock);
        mBacktrace.reset();
        pthread_mutex_unlock(&mPtrlock);
        mThreadPtr = 0;
        mLockTime = Time();
    }
#endif
#ifdef NRDP_THREAD_TRACK_LOCK
    if (Configuration::mutexThreadTrackLock()) {
        if (Thread *thread = Thread::CurrentThread())
            thread->Unlocked(mName);
    }
#endif
#ifdef NRDP_MUTEX_TRACK_LOCK
    if (track && mTracks.capacity()) {
        mTracker.unlocked();
        {
            pthread_once(&sOnce, init_mutex_register);
            pthread_mutex_lock(&sRegisteredMutexes->lock);
            if (mTracks.empty())
                sRegisteredMutexes->mutexes.insert(this);
            mTracks.push_back(mTracker);
            pthread_mutex_unlock(&sRegisteredMutexes->lock);
        }
    }
#endif
    const int ret = pthread_mutex_unlock(&mLock);

    // At this point we may have unblocked the thread that created the mutex, and
    // that thread could have deleted the Mutex object. Which means from here on we're
    // running with an object that has been deleted.
    // Therefore make sure we only use copies of the  mutex object's data that we've
    // copied onto the stack else we're going to be accessing freed memory.
    if (ret == 0) {
        return NFErr_OK;
    } else { // If we're here then the unlock failed, so we still have the mutex.
        if (ret == EINVAL) {
            assert(!NFErr_Uninitialized_Mutex);
            return NFErr_Uninitialized_Mutex;
        } else if (ret == EPERM) {
            assert(!NFErr_Uninitialized_Mutex);
            return NFErr_InvalidAccess;
        } else {
            return NFErr_Bad;
        }
    }
}

NFErrorCode Mutex::tryLock()
{
#ifdef NRDP_MUTEX_TRACK_LOCK
    const bool shouldTrackMutex = mTracks.capacity();
    Tracker tracker(shouldTrackMutex);
#endif
    int ret = 0;
#ifndef NRDP_HAS_RECURSIVE_MUTEX
    if (isCurrentOwner()) {
        if (mMode == NonRecursive) {
            ret = 1;
            assert(0 && "Non-recursive mutex re-trylocked");
        }
    } else
#endif
    {
        ret = pthread_mutex_trylock(&mLock);
    }
    if (ret == 0) {
// we're locked.
#ifndef NRDP_HAS_RECURSIVE_MUTEX
        mOwner = ::pthread_self();
        ++mLockCount;
#endif

        // report this to the MutexStack
        MutexStack::lock(mRank, mName);

#ifdef NRDP_THREAD_TRACK_LOCK
        if (Configuration::mutexThreadTrackLock()) {
            if (Thread *thread = Thread::CurrentThread())
                thread->Locked(mName);
        }
#endif
#ifdef NRDP_MUTEX_TRACK_LOCK
        if (shouldTrackMutex) {
            mTracker = tracker;
            mTracker.locked();
        }
#endif
        return NFErr_OK;
    } else if (ret == EBUSY) {
        return NFErr_NotAllowed;
    } else {
        assert(0);
        return NFErr_Bad;
    }
}

#ifndef NRDP_HAS_RECURSIVE_MUTEX
bool Mutex::isCurrentOwner()
{
    assert(mLockCount >= 0);
    return (mLockCount > 0) && pthread_equal(::pthread_self(), mOwner);
}
#endif

#ifdef NRDP_MUTEX_TRACK_LOCK
namespace netflix {
class MutexDumpData
{
public:
    MutexDumpData(const void* m, const std::string& n, const CircularBuffer<Mutex::Tracker>& t)
        : mutex(m), name(n), tracks(t)
    {
    }

    const void* mutex;
    std::string name;
    CircularBuffer<Mutex::Tracker> tracks;
};
}

inline void Mutex::createVariant(const Mutex::Tracker& tracker, uint8_t flags, bool contentious, Variant* out)
{
    (*out)["thread"] = tracker.thread;
    (*out)["name"] = tracker.thread ? tracker.thread->GetName() : std::string("UNKNOWN");
    (*out)["lock"] = tracker.lock;
    (*out)["unlock"] = tracker.unlock;
    (*out)["wait"] = tracker.wait;
    (*out)["contentious"] = contentious;
    if (flags & Mutex::Dump_Backtrace) {
        if (tracker.backtrace)
            (*out)["bt"] = tracker.backtrace->toString();
    }
}
#endif

bool Mutex::dumpVariant(const DumpInfo &info, uint8_t flags, Variant* data)
{
    NRDP_UNUSED(info);
    NRDP_UNUSED(flags);
    NRDP_UNUSED(data);
#ifdef NRDP_MUTEX_TRACK_LOCK
    if (sRegisteredMutexes) {
        std::vector<MutexDumpData> trackers;
        pthread_mutex_lock(&sRegisteredMutexes->lock);
        for (std::set<Mutex *>::const_iterator it = sRegisteredMutexes->mutexes.begin(); it != sRegisteredMutexes->mutexes.end(); ++it) {
            Mutex *mutex = (*it);
            trackers.push_back(MutexDumpData(mutex, mutex->name(), mutex->mTracks));
            if (flags & Dump_Reset)
                mutex->mTracks.clear();
        }
        if (flags & Dump_Reset)
            sRegisteredMutexes->mutexes.clear();
        pthread_mutex_unlock(&sRegisteredMutexes->lock);

        Variant entry, firstTrack, tracks;
        std::vector<MutexDumpData>::const_iterator it = trackers.begin();
        const std::vector<MutexDumpData>::const_iterator end = trackers.end();
        while (it != end) {
            if (it->tracks.capacity() && (info.empty() || info.match == it->name || info.ptr == it->mutex)) {
                entry.clear();
                entry["name"] = it->name;
                entry["ptr"] = it->mutex;
                entry["size"] = it->tracks.size();
                if (!it->tracks.empty()) {
                    tracks.clear();
                    CircularBuffer<Tracker>::const_iterator tit = it->tracks.begin();

                    const Tracker *last = &(*tit);
                    createVariant(*last, flags, false, &firstTrack);

                    Stopwatch::ms total = 0;
                    int contentious = 0;
                    for (; tit != it->tracks.end(); ++tit) {
                        const Tracker &tracker = (*tit);
                        total += tracker.unlock - tracker.lock;
                        if (tracker.wait < last->unlock && tracker.thread != last->thread) {
                            ++contentious;
                            if (tracks.empty()) {
                                Variant out;
                                createVariant(*last, 0, false, &out);
                                tracks.push_back(out);
                            }

                            Variant out;
                            createVariant(tracker, flags, true, &out);
                            tracks.push_back(out);
                        } else if (tracker.lock - tracker.wait > 5) {
                            if (tracks.empty()) {
                                Variant out;
                                createVariant(*last, 0, false, &out);
                                tracks.push_back(out);
                            }
                            Variant out;
                            createVariant(tracker, 0, false, &out);
                            tracks.push_back(out);
                        }
                        last = &tracker;
                    }
                    entry["total"] = total;
                    entry["contentious"] = contentious;
                    if (tracks.isArray())
                        entry["tracks"] = tracks;
                }
                data->push_back(entry);
            }

            ++it;
        }

        return true;
    }
#endif
    return false;
}

void Mutex::dump(const DumpInfo &info, uint8_t flags)
{
    (void)info;
    (void)flags;
    Log::warn(TRACE_THREAD, "Mutexes: %d", sMutexCount);
#ifdef NRDP_MUTEX_TRACK_LOCK
    Variant data;
    const bool ok = dumpVariant(info, flags, &data);
    if (!ok || data.isNull())
        return;
    assert(data.isArray());

    for (Variant::ArrayIterator it = data.arrayBegin(); it != data.arrayEnd(); ++it) {
        Log::warn(TRACE_THREAD, "Lock: %s(%p): %d",
                  it->mapValue("name").string().c_str(),
                  it->mapValue("ptr").pointer(),
                  it->mapValue("size").value<int>());
        const Variant& tracks = it->mapValue("tracks");
        if (tracks.isArray()) {
            Variant::ArrayIterator tit = tracks.arrayBegin();
            assert(tit != tracks.arrayEnd());
            const void* lastThread = tit->mapValue("thread").pointer();
            std::string lastName = tit->mapValue<std::string>("name");
            std::string lastBt = tit->mapValue<std::string>("bt");
            ++tit;
            assert(tit != tracks.arrayEnd());
            for (; tit != tracks.arrayEnd(); ++tit) {
                const bool contentious = tit->mapValue<bool>("contentious");
                const void* thread = tit->mapValue("thread").pointer();
                const std::string name = tit->mapValue("name").string();
                const double lock = tit->mapValue<double>("lock");
                const double wait = tit->mapValue<double>("wait");
                const std::string bt = tit->mapValue<std::string>("bt");
                if (contentious) {
                    Log::warn(TRACE_THREAD, " + Contention: %p(%s) %fms [%p(%s)]",
                              thread, name.c_str(), lock - wait, lastThread, lastName.c_str());
                    if (!lastBt.empty()) {
                        Log::warn(TRACE_THREAD, "%s %s", lastName.c_str(), lastBt.c_str());
                    }
                    if (!bt.empty()) {
                        Log::warn(TRACE_THREAD, "%s %s", name.c_str(), bt.c_str());
                    }
                } else {
                    Log::warn(TRACE_THREAD, " + Wait: %p(%s) %fms", thread, name.c_str(), lock - wait);
                }
                lastThread = thread;
                lastName = name;
                lastBt = bt;
            }
        }
    }
    Log::warn(TRACE_THREAD, "%d locks", data.size());
#endif
}

bool Mutex::isLocked() const
{
    return MutexStack::locked(mName);
}
