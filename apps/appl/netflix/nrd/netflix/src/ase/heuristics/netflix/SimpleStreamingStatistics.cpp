/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "SimpleStreamingStatistics.h"

using namespace netflix::ase;
using namespace std;

SimpleStreamingStatistics::SimpleStreamingStatistics() :
                                mPlaybackTime(0)
{
}

void
SimpleStreamingStatistics::updateStreamingPts( int32_t trackIndex,
                                               AseTimeStamp const& movieTime)
{
    mTrackToStreamingTime[trackIndex] = movieTime.getValueInMs();
}

void
SimpleStreamingStatistics::updatePlaybackPts( AseTimeStamp const& movieTime )
{
    mPlaybackTime = movieTime.getValueInMs();
}

uint32_t
SimpleStreamingStatistics::streamingBufferLevel()
{
    uint32_t streamingTime = obtainStreamingTime();

    if ( (streamingTime == MAX_UINT32) || (streamingTime <= mPlaybackTime) )
        return 0;
    else
        return streamingTime - mPlaybackTime;
}

uint32_t
SimpleStreamingStatistics::obtainStreamingTime()
{
    uint32_t streamingTime = MAX_UINT32;
    for ( map<uint32_t, uint32_t>::iterator iter = mTrackToStreamingTime.begin();
            iter != mTrackToStreamingTime.end();
            ++iter )
    {
        if ( (*iter).second < streamingTime )
            streamingTime = (*iter).second;
    }

    if ( streamingTime == MAX_UINT32 )
        streamingTime = 0;

    return streamingTime;
}


