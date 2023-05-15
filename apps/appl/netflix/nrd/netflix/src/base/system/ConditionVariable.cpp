/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include <nrdbase/ConditionVariable.h>

#include <pthread.h>
#include <sys/time.h>
#include <errno.h>
#include <time.h>   // POSIX defines timespec here

#include <nrdbase/Thread.h>
#include <nrdbase/Configuration.h>
#include <nrdbase/NFErrorTimedOut.h>

using namespace netflix;

ConditionVariable::ConditionVariable()
{
    pthread_cond_init(&mCond, NULL);
}

ConditionVariable::~ConditionVariable()
{
    pthread_cond_destroy(&mCond);
}

void ConditionVariable::signal()
{
    pthread_cond_signal(&mCond);
}

void ConditionVariable::broadcast()
{
    pthread_cond_broadcast(&mCond);
}

NFErr ConditionVariable::wait(Mutex *mutex, Time *time)
{
    if (!time)
        return wait(mutex);
    if (!time->val())
        return new NFErrorTimedOut(0);
    const Time before = Time::mono();
    const NFErr err = wait(mutex, *time);
    const Time elapsed = (Time::mono() - before);
    if (elapsed >= *time) {
        *time = Time();
    } else {
        *time -= elapsed;
    }
    return err;
}

NFErr ConditionVariable::wait(Mutex *mutex, const Time &time)
{
    NFErr ret = NFErr_OK;
    if (mutex == NULL) {
        return NFErr_BadParameter;
    }
#ifndef NRDP_HAS_RECURSIVE_MUTEX
    if (!mutex->isCurrentOwner())
        return NFErr_InvalidAccess;
    pthread_t owner = mutex->mOwner;
    int lockCount = mutex->mLockCount;
    mutex->mLockCount = 0;
#endif
#ifdef NRDP_THREAD_TRACK_LOCK
    if(Configuration::mutexThreadTrackLock()) {
        if(Thread *thread = Thread::CurrentThread())
            thread->Unlocked(mutex->mName, true);
    }
#endif

    int rv = 0;
    if (time.val() == 0) {
        rv = pthread_cond_wait(&mCond, &mutex->mLock);
    } else {
        struct timeval now;
        gettimeofday(&now, 0);

        struct timespec timeout;

        // convert everything to Time
        Time tNow(0, 0, 0, now.tv_sec, (now.tv_usec / 1000));
        Time tThen = (tNow + time);
        // Now generate the timeout value for pthread_cond_timeout
        timeout.tv_sec = (time_t)tThen.seconds();
        timeout.tv_nsec = (long)((tThen.val() - (tThen.seconds() * MilliSecondsPerSecond))
                                 * NanoSecondsPerMillisecond);

        rv = pthread_cond_timedwait(&mCond, &mutex->mLock, &timeout);
    }

    if (rv == EINVAL) {
        ret = NFErr_BadParameter;
    } else if (rv == ETIMEDOUT) {
        ret.push(new NFErrorTimedOut(time.ms()));
    } else if (rv == EPERM) {
        ret = NFErr_InvalidAccess;
    }
#ifndef NRDP_HAS_RECURSIVE_MUTEX
    mutex->mOwner = owner;
    mutex->mLockCount = lockCount;
#endif
#ifdef NRDP_THREAD_TRACK_LOCK
    if(Configuration::mutexThreadTrackLock()) {
        if(Thread *thread = Thread::CurrentThread())
            thread->Locked(mutex->mName);
    }
#endif
    return ret;
}
