/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#ifndef ScopedMutex_h
#define ScopedMutex_h

#include <nrdbase/Noncopyable.h>

#include <nrdbase/Assert.h>
#include <nrdbase/Mutex.h>

namespace netflix {

/**
 * @class ScopedMutex ScopedMutex.h
 * @brief RAII Mutex wrapper.
 *
 * Uses the "Resource Acquisition is Initialization" idiom to implement
 * the Scoped Locking pattern (see POSA2). Also similar to STL's auto_ptr.
 * The idea is to use compiler generated ctor/dtor calls to make sure that a
 * locked mutex is always unlocked, regardless of the exit path from a given
 * scope.
 *
 * For example, instead of this:
 *
 * void foo(Mutex mutex)
 * {
 *     mutex.lock();
 *     ...
 *     // do something complicated, if an excep is thrown mutex left locked
 *     ...
 *     if (error)
 *     {
 *         mutex.unlock();     // make sure you don't forget to unlock!
 *         return;
 *     }
 *     ...
 *     // do more stuff
 *     ...
 *     mutex.unlock();        // if you forget this line mutex is left locked
 * }
 *
 * Do this:
 *
 * void foo(Mutex mutex)
 * {
 *     ScopedMutex guard(mutex);  // mutex.lock() called inside guard ctor
 *     ...
 *     // do something complicated, might throw, safe now
 *     ...
 *     if (error)
 *         return;    // can just return anywhere, guard dtor will unlock mutex
 *     }
 *     ...
 *     // do more stuff
 *
 * }  // mutex be unlocked because guard dtor is called on scope exit
 *
 */
class ScopedMutex : private Noncopyable
{
public:
    /**
     * Acquire the lock.
     *
     * @param[in] lock the lock.
     */
    explicit ScopedMutex(Mutex& lock) : mLock(lock), mLocked(false)
    {
        relock();
    }

    /** Release the lock. */
    ~ScopedMutex()
    {
        unlock();
    }

    void unlock()
    {
        if (mLocked) {
            mLocked = false;
            const NFErrorCode err = mLock.unlock();
#if 0
            ASSERT(err == netflix::NFErr_OK, "Failed to unlock mutex err = %d", err);
#endif
            (void)err;
        }
    }

    void relock()
    {
        if (!mLocked) {
            mLocked = true;
            netflix::NFErrorCode err = mLock.lock();
            ASSERT(err == netflix::NFErr_OK, "Failed to acquire lock err = %d", err);
            (void)err;    // avoid compiler warning when ASSERT is off
        }
    }

private:
    Mutex& mLock; //!< The acquired lock.
    bool mLocked;
};

} // namespace netflix

#endif
