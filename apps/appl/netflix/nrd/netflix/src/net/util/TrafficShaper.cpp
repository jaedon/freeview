/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include <nrdbase/MutexRanks.h>
#include <nrdbase/ScopedMutex.h>

#include "TrafficShaper.h"
#include <unistd.h>
#include <nrdbase/Log.h>

using namespace std;
using namespace netflix;

const uint32_t TrafficShaper::defaultCapacitySoftLimit = 1024 * 1024;
const uint32_t TrafficShaper::defaultCapacityHardLimit = 2048 * 1024;

const uint32_t TrafficShaper::defaultMaxSleepInterval = 500;
const uint32_t TrafficShaper::defaultDrainingSpeed = 10000;

/** constructor */
TrafficShaper::TrafficShaper(   uint32_t speed,
                                uint32_t minSleepInterval,
                                uint32_t maxSleepInterval,
                                uint32_t capacitySoftLimit,
                                uint32_t capacityHardLimit )
                    : mDrainingSpeed(speed / 8)
                    , mMinimumSleepInterval( minSleepInterval )
                    , mMaximumSleepInterval(maxSleepInterval)
                    , mCapacitySoftLimit(capacitySoftLimit)
                    , mCapacityHardLimit(capacityHardLimit)
                    , mPrevUpdateTime(AseTimeVal::now())
                    , mBytesInTheBucket(0)
{
}

uint64_t TrafficShaper::calculateSleepTime()
{
    AseTimeVal const now = AseTimeVal::now();

    uint32_t const timeElapsed = (now - mPrevUpdateTime).ms();

    mPrevUpdateTime = now;

    uint64_t const drainedBytes = timeElapsed * mDrainingSpeed;

    mBytesInTheBucket = (mBytesInTheBucket > drainedBytes) ?
                                    (mBytesInTheBucket - drainedBytes) :
                                    0;

    return mBytesInTheBucket > mCapacitySoftLimit ? (mBytesInTheBucket - mCapacitySoftLimit) / mDrainingSpeed : 0;
}

void TrafficShaper::sleep()
{
    uint64_t const sleepTime = calculateSleepTime();
    if ( sleepTime >= mMinimumSleepInterval )
    {
        ullong const actualSleepTime = ( (sleepTime > mMaximumSleepInterval) ? mMaximumSleepInterval : sleepTime );
#if 1
        NTRACE(TRACE_HTTPLIB, "TrafficShaper sleep for %llums (bucket=%u,capacity=%u)",
            actualSleepTime,
            mBytesInTheBucket,
            mCapacitySoftLimit );
#endif
        ::usleep( actualSleepTime * 1000 );
    }
}
