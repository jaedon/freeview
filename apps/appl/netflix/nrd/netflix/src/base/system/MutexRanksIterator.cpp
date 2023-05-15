/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include <assert.h>
#include <vector>
#include <sys/types.h>
#include <algorithm>
#include <string.h>
#include "MutexRanksIterator.h"

namespace netflix {

#define DEFINE_MUTEX_RANK_VAL(rank, val) { rank, #rank, val }
#define DEFINE_MUTEX_RANK(rank) { rank, #rank, MutexRankValue(rank) }

struct RankGreaterThan
{
    bool operator()(const Rank &l, const Rank &r)
    {
        if (l.value == r.value)
            return l.name < r.name;
        return static_cast<unsigned int>(l.value) > static_cast<unsigned int>(r.value);
    }
};

std::vector<Rank> mutexRanks()
{
    const Rank ranks[] = {
#include "MutexRanksInternal.h"
    };
    const size_t count = sizeof(ranks) / sizeof(ranks[0]);
    std::vector<Rank> ret(ranks, ranks + count);
    RankGreaterThan gt;
    std::sort(ret.begin(), ret.end(), gt);
    return ret;
}
}
