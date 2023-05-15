/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef __MUTEX_RANKS_H__
#define __MUTEX_RANKS_H__

/*
    To add a new MutexRank:

    1) add a new value to the enum MutexRank below:
    2) add a MAP_MUTEX_RANK entry to the MutexRankValue function in MutexRanks.cpp
    3) add a #define YOUR_MUTEX_VAL() to map the enum to an int value in MutexRanks.cpp
       if rank is a simple value, use v(value)
       if rank is dependent on another, use p1(OTHER_MUTEX_VAL())
       if rank is dependent on multiple mutex's, use maxRankOf(n, FIRST_MUTEX_RANK(), etc)
 */

#ifndef DEFINE_MUTEX
#define DEFINE_MUTEX_RANK(mutex) mutex
#endif
namespace netflix {

enum MutexRank
{
#include <nrdbase/MutexRanksInternal.h>
};

// returns the numeric rank of a MutexRank
int MutexRankValue(MutexRank rank);

#undef DEFINE_MUTEX_RANK
}

#endif // __MUTEX_RANKS_H__
