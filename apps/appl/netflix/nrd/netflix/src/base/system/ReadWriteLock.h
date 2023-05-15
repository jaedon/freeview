/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef ReadWriteLock_h
#define ReadWriteLock_h

#include <pthread.h>
#include <assert.h>

#include <nrdbase/Assert.h>
#include <nrdbase/ConditionVariable.h>
#include <nrdbase/Configuration.h>
#include <nrdbase/Mutex.h>
#include <nrdbase/Thread.h>
#include <nrdbase/Stopwatch.h>
#include <nrdbase/MutexRanks.h>
#include <nrdbase/MutexStack.h>
#include <nrdbase/ScopedMutex.h>

namespace netflix {

template<typename T> class AbstractReadWriteLock : private Noncopyable
{
public:
    inline AbstractReadWriteLock(MutexRank rank=ZERO_MUTEX, const char *name=NULL) :
        mMutex(READWRITELOCK_MUTEX, "ReadWriteLock"),
        mRank(MutexRankValue(rank)),
        mName(name)
    {
        mWriters.hasOwner = false;
        mReaders.count = mWriters.count = mAccesses = 0;
    }
    virtual ~AbstractReadWriteLock() {
        assert(!mAccesses);
        assert(!mReaders.count);
        assert(!mWriters.hasOwner);
        assert(!mWriters.count);
    }

    enum LockType {
        Read,
        Write
    };

protected:
    netflix::NFErr lockOwner(LockType type, T owner)
    {
        {
            ScopedMutex _mutex(mMutex);
            if (type == Write) {
                if (!mWriters.hasOwner || !compareOwner(owner, mWriters.owner)) {
                    while (mAccesses != 0) {
                        ++mWriters.count;
                        mWriters.waiting.wait(&mMutex);
                        --mWriters.count;
                    }
                    assert(!mWriters.hasOwner);
                    mWriters.owner = owner;
                    mWriters.hasOwner = true;
                }
                --mAccesses;
            } else {
                if (mAccesses < 0 && mWriters.hasOwner &&
                    compareOwner(owner, mWriters.owner)) {
                    --mAccesses; //ewww
                } else {
                    while (mAccesses < 0) {
                        ++mReaders.count;
                        mReaders.waiting.wait(&mMutex);
                        --mReaders.count;
                    }
                    ++mAccesses;
                }
            }
        }

        // notify mutex stack
        MutexStack::lock(mRank, mName);
#ifdef NRDP_THREAD_TRACK_LOCK
        if(Configuration::mutexThreadTrackLock()) {
            if(Thread *thread = Thread::CurrentThread())
                thread->Locked(mName);
        }
#endif
        return NFErr_OK;
    }

    netflix::NFErr unlockOwner()
    {
        {
            ScopedMutex _mutex(mMutex);
            if (mAccesses > 0) {
                --mAccesses;
            } else if ((++mAccesses) == 0) {
                assert(mWriters.hasOwner);
                mWriters.hasOwner = false;
            }
            if (!mAccesses) {
                if (mWriters.count)
                    mWriters.waiting.signal();
                else if (mReaders.count)
                    mReaders.waiting.broadcast();
            }
        }

        // report this to the MutexStack
        MutexStack::unlock(mRank, mName);

#ifdef NRDP_THREAD_TRACK_LOCK
        if(Configuration::mutexThreadTrackLock()) {
            if(Thread *thread = Thread::CurrentThread())
                thread->Unlocked(mName);
        }
#endif

        return NFErr_OK;
    }

    virtual bool compareOwner(T a, T b) = 0;

private:
    Mutex mMutex;
    int mAccesses;
    struct {
        int count;
        ConditionVariable waiting;
        T owner;
        bool hasOwner;
    } mWriters;
    struct {
        int count;
        ConditionVariable waiting;
    } mReaders;

    const int mRank;
    const char *const mName;
};

class ReadWriteLock : public AbstractReadWriteLock<pthread_t>
{
public:
    ReadWriteLock(MutexRank rank, const char *name)
        : AbstractReadWriteLock<pthread_t>(rank, name)
    {}
    ~ReadWriteLock() {}

    netflix::NFErr lock(LockType type)
    {
        return lockOwner(type, ::pthread_self());
    }
    netflix::NFErr lockForWrite()
    {
        return lockOwner(Write, ::pthread_self());
    }
    netflix::NFErr lockForRead()
    {
        return lockOwner(Read, ::pthread_self());
    }
    netflix::NFErr unlock()
    {
        return unlockOwner();
    }

protected:
    bool compareOwner(pthread_t a, pthread_t b)
    {
        return pthread_equal(a, b);
    }
};

class ScopedReadWriteLock : private Noncopyable
{
public:
    /**
     * Acquire the lock.
     *
     * @param[in] lock the lock.
     */
    explicit ScopedReadWriteLock(ReadWriteLock& lock, ReadWriteLock::LockType type) : mLock(lock)
    {
        netflix::NFErr err = mLock.lock(type);
        ASSERT(err == netflix::NFErr_OK, "Failed to acquire lock err = %s",
               err.toString().c_str());
        (void)err;    // avoid compiler warning when ASSERT is off
    }

    /** Release the lock. */
    ~ScopedReadWriteLock()
    {
        mLock.unlock();
    }

private:
    ReadWriteLock& mLock; //!< The acquired lock.
};

}

#endif
