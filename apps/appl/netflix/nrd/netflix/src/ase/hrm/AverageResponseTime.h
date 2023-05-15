/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef AVERAGE_RESPONSE_TIME_H
#define AVERAGE_RESPONSE_TIME_H

#include <list>

#include "AseCommonDataTypes.h"

namespace netflix {
namespace ase
{

/**
 * @class AverageResponseTime
 * @brief average response time.
 */
class AverageResponseTime
{
public:
    /** Constructor. */
    AverageResponseTime();

    /** Constructor. */
    AverageResponseTime( ullong sampleSize );

    /** Add a response time sample.
     *
     *  @param responseTime [In] The response time, in ms.
     */
    void addResponseTimeSample( ullong responseTime );

    /** Get the current average response time, in ms.*/
    ullong getAverageResponseTime();

protected:
    /** The sample size */
    ullong mSampleSize;

    /** The current sample count. */
    ullong mSampleCount;

    /** The sample group, the response time is in ms. */
    std::list<ullong> mSampleGroup;

    /** The sum of the resonse time samples. */
    ullong mResponseTimeSum;

    /** The current average response time. */
    ullong mAverageResponseTime;
};

}}
#endif
