/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef TRAFFIC_SHAPER_H
#define TRAFFIC_SHAPER_H

#include <assert.h>
#include "AseTimeVal.h"

namespace netflix {

class TrafficShaper
{
public:
    typedef shared_ptr<TrafficShaper> TrafficShaperPtr;

public:
    /** Create a new leaky bucket traffic shaper
     *  @param speed [in] The bucket draining speed in kbit/s
     *  @param maxSleepInterval [in] The maximum sleep time in ms
     *  @param capacitySoftLimit [in] Bucket capacity controlling sleep times on reserve
     *  @param capacityHardLimit [in] A hard capacity limit above which you get free capacity
     */
    TrafficShaper(  uint32_t speed,
                    uint32_t minSleepInterval,
                    uint32_t maxSleepInterval,
                    uint32_t capacitySoftLimit,
                    uint32_t capacityHardLimit );

    /** Sleep until the bucket had drained, so that more bytes can be received */
    void sleep();

    /** Test if we need to sleep */
    bool wouldSleep();

    /** Add bytes received to the bucket
     *  @param bytes [in] the number of bytes received
     */
    void addBytes(int32_t bytes);

protected:

    /** Calculate how long to sleep in ms */
    uint64_t calculateSleepTime();

protected:
    static const uint32_t defaultCapacitySoftLimit; // bytes
    static const uint32_t defaultCapacityHardLimit;

    static const uint32_t defaultMaxSleepInterval; // ms
    static const uint32_t defaultDrainingSpeed; // kbps

    uint32_t const  mDrainingSpeed;            // kilo bytes per second

    uint32_t const  mMinimumSleepInterval;

    uint32_t const  mMaximumSleepInterval;

    uint32_t const  mCapacitySoftLimit;

    uint32_t const  mCapacityHardLimit;

    AseTimeVal      mPrevUpdateTime;

    uint32_t        mBytesInTheBucket;
};

typedef TrafficShaper::TrafficShaperPtr TrafficShaperPtr;

inline bool TrafficShaper::wouldSleep()
{
    return ( calculateSleepTime() > mMinimumSleepInterval );
}


inline void TrafficShaper::addBytes(int32_t bytes)
{
    mBytesInTheBucket = (mBytesInTheBucket + bytes > mCapacityHardLimit) ?
                                mCapacityHardLimit : mBytesInTheBucket + bytes;
}

}

#endif
