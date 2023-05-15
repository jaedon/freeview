/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#include "Semaphore.h"

#include <nrdbase/Assert.h>
#include <nrdbase/MutexRanks.h>
#include <nrdbase/ScopedMutex.h>
#include <nrdbase/NFErr.h>

using namespace netflix;

/*
Our ucLibc lacks sem_timedwait.  Accordingly, the only way to get timed semaphores
is to implement them ourselves using condition variables. Use this implementation
in native mode too for consistency.
*/
Semaphore::Semaphore(uint32_t initialCount) :
    mutex(SEMAPHORE_MUTEX, "Semaphore"),
    count(initialCount)
{
}

Semaphore::~Semaphore()
{
}

void Semaphore::post()
{
    ScopedMutex lock(mutex);
    count++;
    if (count == 1)
    {
        cond.signal();
    }
}

void Semaphore::wait()
{
    ScopedMutex lock(mutex);
    while (count <= 0)
    {
        NFErr result = cond.wait(&mutex);
        // any failure is fatal here
        ASSERT(result == NFErr_OK, "ConditionVariable::wait failed");
    }
    count--;
}

bool Semaphore::tryWait()
{
    ScopedMutex lock(mutex);
    if (count > 0) {
        --count;
        return true;
    }
    return false;
}

bool Semaphore::timedWait(uint32_t relTimeOutMs)
{
    ScopedMutex lock(mutex);

    while (count == 0)
    {
        NFErr result = cond.wait(&mutex, relTimeOutMs);
        // catch a timeout
        if (result == NFErr_TimedOut)
        {
            return false;
        }
        // any other failure is fatal
        if (result != NFErr_OK)
        {
            ASSERT(false, "ConditionVariable::timedwait failed");
            return false;
        }
    }
    count--;

    return true;
}

void Semaphore::reset()
{
    ScopedMutex lock(mutex);
    count = 0;
}
