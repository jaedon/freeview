/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef SIMPLE_STREAMING_STAT_H
#define SIMPLE_STREAMING_STAT_H
#include <nrdase/IStreamingStatistics.h>

#include <map>

namespace netflix {
namespace ase {
/**
 * @class SimpleStreamingStatistics
 */
class SimpleStreamingStatistics : public IStreamingStatistics
{
public:
    typedef shared_ptr<SimpleStreamingStatistics> SimpleStreamingStatisticsPtr;

    SimpleStreamingStatistics();

    // IStreamingStatistics
    virtual void updateStreamingPts( int32_t trackIndex, AseTimeStamp const& movieTime );
    virtual void updatePlaybackPts( AseTimeStamp const& movieTime );
    virtual uint32_t streamingBufferLevel();
    virtual uint32_t obtainStreamingTime();

protected:
    static const uint32_t MAX_UINT32           = 0xFFFFFFFF;

    std::map<uint32_t, uint32_t> mTrackToStreamingTime;
    uint32_t mPlaybackTime;

};

typedef SimpleStreamingStatistics::SimpleStreamingStatisticsPtr SimpleStreamingStatisticsPtr;


}}

#endif
