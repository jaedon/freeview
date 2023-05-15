/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef __MUTEX_STACK_H__
#define __MUTEX_STACK_H__

namespace netflix {

#if defined(BUILD_DEBUG)
#define NRDP_MUTEX_STACK
#endif

class MutexStack
{
public:
    // called by Mutex's when they lock/unlock
#ifdef NRDP_MUTEX_STACK
    static int size();
    static bool locked(const char *name);
    static void lock(int rank, const char *name);
    static void unlock(int rank, const char *name);
    static void dump();
#else
    inline static int size() { return 0; }
    inline static bool locked(const char *) { return true; }
    inline static void lock(int, const char *) { }
    inline static void unlock(int, const char *) { }
    inline static void dump() { }
#endif
};

}

#endif // __MUTEX_STACK_H__
