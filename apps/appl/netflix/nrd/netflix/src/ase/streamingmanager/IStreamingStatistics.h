/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef I_STREAMING_STAT_H
#define I_STREAMING_STAT_H

#include <nrdase/AseTimeStamp.h>

namespace netflix {
namespace ase {
/**
 * @class IStreamingStatistics
 * @brief The interface to provide streaming statistics
 */
class IStreamingStatistics
{
public:
   /**
     * Update the current streaming time.
     * @param[in] trackIndex the index of the track
     * @param[in] movieTime the current streaming pts
     */
    virtual void updateStreamingPts( int32_t trackIndex, AseTimeStamp const& movieTime ) = 0;

    /** Update the current playback time
     * @param[in] movieTime the current playback pts
     */
    virtual void updatePlaybackPts( AseTimeStamp const& movieTime ) = 0;

    /**
     * @return the current streaming buffer level
     */
    virtual uint32_t streamingBufferLevel() = 0;

    /**
     * @return the current streaming time
     */
    virtual uint32_t obtainStreamingTime() = 0;

    virtual ~IStreamingStatistics() {}


    typedef shared_ptr<IStreamingStatistics> IStreamingStatisticsPtr;
};

typedef IStreamingStatistics::IStreamingStatisticsPtr IStreamingStatisticsPtr;
}}
#endif
