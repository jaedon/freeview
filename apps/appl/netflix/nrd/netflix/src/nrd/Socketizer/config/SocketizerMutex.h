/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef SOCKETIZERMUTEX_H
#define SOCKETIZERMUTEX_H

#include <pthread.h>
#include <stdio.h>
#include <assert.h>

namespace netflix {
namespace socketizer {

class Mutex
{
public:
    Mutex();
    ~Mutex();

    void lock();
    void unlock();

private:
    Mutex(const Mutex &);
    Mutex &operator=(const Mutex &);
    pthread_mutex_t mMutex;
};

class MutexLocker
{
public:
    MutexLocker(Mutex& mutex)
        : mMutex(mutex), mLocked(true)
    {
        mMutex.lock();
    }
    ~MutexLocker()
    {
        if (mLocked) {
            mMutex.unlock();
        }
    }

    void unlock()
    {
        assert(mLocked);
        mMutex.unlock();
        mLocked = false;
    }

    void relock()
    {
        assert(!mLocked);
        mMutex.lock();
        mLocked = true;
    }

private:
    Mutex& mMutex;
    bool mLocked;
};

inline Mutex::Mutex()
{
    pthread_mutex_init(&mMutex, 0);
}

inline Mutex::~Mutex()
{
    pthread_mutex_destroy(&mMutex);
}

inline void Mutex::lock()
{
    pthread_mutex_lock(&mMutex);
}

inline void Mutex::unlock()
{
    pthread_mutex_unlock(&mMutex);
}
}
}

#endif
