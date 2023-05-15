/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#ifndef Semaphore_h
#define Semaphore_h

#include <nrdbase/Mutex.h>
#include <nrdbase/ScopedMutex.h>
#include <nrdbase/ConditionVariable.h>

namespace netflix {

/**
 * Encapsulates a counting semaphore.
 */
class Semaphore
{
  public:
    /**
     * Creates a new semaphore. Optionally specify an initial count.
     */
    Semaphore(uint32_t initialCount=0);

    /**
     * Destroys this semaphore.
     */
    virtual ~Semaphore();

    /**
     * Decrements the semaphore count, if it is positive; otherwise waits
     * until it is positive and then decrements the count.
     */
    void wait();

    /**
     * Decrements the semaphore count and returns <tt>true</tt>, if the
     * count is positive; returns <tt>false</tt> if not.  Does not block.
     */
    bool tryWait();

    /**
     * Decrements the semaphore count, if it is positive; otherwise waits
     * until it is positive and then decrements the count and returns
     * <tt>true</tt>.  Gives up after <tt>milliSeconds</tt> milliseconds and
     * returns <tt>false</tt> instead.
     */
    bool timedWait(uint32_t milliSeconds);

    /**
     * Increments the semaphore count, and if it was zero, marks a thread
     * (if any) currently waiting on this semaphore as ready-to-run.  If
     * multiple threads are waiting, only one will be woken, but it is not
     * specified which.
     *
     * This method never blocks.
     */
    void post();

    /**
     * Reset the semaphore count to zero. Does not block.
     */
    void reset();

    /**
     * Return internal count. Mostly for debug / testing.
     */
   inline uint32_t getCount() const {
       ScopedMutex _lock(mutex);
       return count;
   }

  private:
    Semaphore(const Semaphore &);             // no copying
    Semaphore& operator=(const Semaphore&);   // no copying
    mutable Mutex mutex;
    ConditionVariable cond;
    uint32_t count;
};

} // namespace netflix

#endif
