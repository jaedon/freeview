/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef AtomicCount_h
#define AtomicCount_h

#include <nrdbase/config.h>
#ifndef NRDP_HAS_ATOMICS
# include <nrdbase/Mutex.h>
# include <nrdbase/ScopedMutex.h>
#endif

namespace netflix {
class AtomicCount
{
public:
    inline AtomicCount(int r = 1) : mRef(r) {}
    inline ~AtomicCount() {}

    inline int ref(int count = 1)
    {
#ifdef NRDP_HAS_ATOMICS
        return __sync_add_and_fetch(&mRef, count);
#else
        ScopedMutex lock(mutex());
        return mRef += count;
#endif
    }

    inline int deref(int count = 1)
    {
#ifdef NRDP_HAS_ATOMICS
        return __sync_sub_and_fetch(&mRef, count);
#else
        ScopedMutex lock(mutex());
        return mRef -= count;
#endif
    }

    inline int count() const
    {
#ifdef NRDP_HAS_ATOMICS
        __sync_add_and_fetch(&mRef, 0);
#else
        ScopedMutex lock(mutex());
#endif
        return mRef;
    }

private:
#ifndef NRDP_HAS_ATOMICS
    static Mutex &mutex()
    {
        static Mutex sMutex(UNTRACKED_MUTEX, "AtomicCountMutex");
        return sMutex;
    }
#endif
    mutable volatile int mRef;
};
}


#endif
