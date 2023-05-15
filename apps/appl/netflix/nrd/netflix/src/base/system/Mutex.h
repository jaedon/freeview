/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#ifndef Mutex_h
#define Mutex_h

#include <pthread.h>

#include <nrdbase/config.h>
#include <nrdbase/Time.h>
#include <nrdbase/tr1.h>
#include <nrdbase/Stopwatch.h>
#include <nrdbase/NFErrorCode.h>
#include <nrdbase/MutexRanks.h>
#include <nrdbase/Exportable.h>

#include <list>

#if defined(BUILD_DEBUG) && defined(__i386__)
# define NRDP_MUTEX_TRACK_LOCK
#endif
#if (defined(BUILD_DEBUG) && defined(NRDP_HAS_EXECINFO_H))
# define NRDP_MUTEX_DETECT_DEADLOCK
#endif
#if !defined(NRDP_NO_THREAD_TRACK_LOCK)
# define NRDP_THREAD_TRACK_LOCK
#endif

#ifdef NRDP_MUTEX_TRACK_LOCK
# include <nrdbase/CircularBuffer.h>
#endif

namespace netflix {
class Thread;
struct DumpInfo;
class Backtrace;
class ConditionVariable;
class Variant;
class MutexDumpData;

/**
 * @class Mutex Mutex.h
 * @brief A mutex.
 */
class NRDP_EXPORTABLE Mutex {
    friend class ConditionVariable;
    friend class MutexDumpData;

public:
    /**
     * Construct a new Mutex.
     */
    enum Mode {
        Recursive,
        NonRecursive
    };
    Mutex(MutexRank rank, const char *name, Mode mode = Recursive);

    /** Destructor. */
    ~Mutex();

    int rank() const { return mRank; }
    const char *name() const { return mName; }
    Mode mode() const { return mMode; }

    bool isLocked() const;

    /**
     * Acquire the mutex.
     *
     * @return NFErr_OK if the mutex has been acquired;
     *         NFErr_Uninitialized_Mutex if the mutex is not
     *         initialized;
     *         NFErr_DeadLock if a deadlock is detected;
     *         NFErr_Bad if something went wrong.
     */
    inline NFErrorCode lock() { return lock(true); }


    /**
     * For debugging, Log::warn() with information about each mutex and its locks
     */
    enum {
        Dump_Reset     = 0x01,
        Dump_Backtrace = 0x02
    };
    static bool dumpVariant(const DumpInfo &info, uint8_t flags, Variant* data);
    static void dump(const DumpInfo &info, uint8_t flags);

    /**
     * Release the mutex.
     *
     * @return NFErr_OK if the mutex has been released;
     *         NFErr_Uninitialized_Mutex if the mutex is not
     *         initialized;
     *         NFErr_InvalidAcess if the thread does not have access
     *         to the mutex;
     *         NFErr_Bad if something went wrong.
     */
    inline NFErrorCode unlock() { return unlock(true); }

    /**
     * Attempt mutex acquisition.
     *
     * @return NFErr_OK if the mutex has been acquired;
     *         NFErr_NotAllowed if the mutex couldn't be acquired;
     *         NFErr_Bad if something went wrong.
     */
    NFErrorCode tryLock();
private:
    NFErrorCode lock(bool track);
    NFErrorCode unlock(bool track);
    Mutex(const Mutex &other);             // no copy
    Mutex& operator=(const Mutex &other);  // no copy

    const int mRank;
    const char *const mName;
    const Mode mMode;

    pthread_mutex_t mLock;
#ifdef NRDP_MUTEX_TRACK_LOCK
    struct Tracker {
        Tracker(bool start=false);
        void locked();
        void unlocked();
        Thread *thread;
        shared_ptr<Backtrace> backtrace;
        Stopwatch::ms wait, lock, unlock;
    };
    Tracker mTracker;
    CircularBuffer<Tracker> mTracks;

    static void createVariant(const Tracker& tracker, uint8_t flags, bool contentious, Variant* out);
#endif
#ifndef NRDP_HAS_RECURSIVE_MUTEX
    /* Information about the thread that currently holds the mutex. */
    pthread_t mOwner;
    int mLockCount;

    /**
     * Returns whether the current thread owns this mutex.
     *
     * @return true if the mutex is locked and the current thread is the owner
     *         of the lock.
     */

    bool isCurrentOwner();
#endif
#if defined(NRDP_MUTEX_DETECT_DEADLOCK)
    pthread_mutex_t mPtrlock;
    shared_ptr<Backtrace> mBacktrace;
    void *mThreadPtr;
    int mBacktraceLockCount;
    Time mLockTime;
#endif
};

} // namespace netflix

#endif
