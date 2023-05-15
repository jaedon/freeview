/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "AverageResponseTime.h"

namespace netflix {
namespace ase
{

/** Constructor. */
AverageResponseTime::AverageResponseTime()
                                            : mSampleSize( 1 ),
                                              mSampleCount( 0 ),
                                              mResponseTimeSum( 0 ),
                                              mAverageResponseTime( 0 )
{
}
/** Constructor. */
AverageResponseTime::AverageResponseTime( ullong sampleSize )
                                            : mSampleSize( sampleSize ),
                                              mSampleCount( 0 ),
                                              mResponseTimeSum( 0 ),
                                              mAverageResponseTime( 0 )
{
}

/** Add a response time sample. */
void AverageResponseTime::addResponseTimeSample( ullong responseTime )
{
    if (mSampleCount >= mSampleSize)
    {
        mResponseTimeSum -= mSampleGroup.front();

        mSampleGroup.pop_front();

        mSampleCount--;
    }

    mResponseTimeSum += responseTime;

    mSampleGroup.push_back( responseTime );

    mSampleCount++;

    mAverageResponseTime = mResponseTimeSum / mSampleCount;
}

/** Get the current average response time, in ms.*/
ullong AverageResponseTime::getAverageResponseTime()
{
    return mAverageResponseTime;
}

}}
