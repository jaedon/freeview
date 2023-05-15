/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef __IMEDIASOURCE_PLAYBACK_REPORTER_H__
#define __IMEDIASOURCE_PLAYBACK_REPORTER_H__

#include <nrd/config.h>

#include <nrd/IMediaPipelineReporter.h>
#include <nrd/IPlaybackDevice.h>

namespace netflix {
namespace mediacontrol {

class IMediaSourcePlaybackReporter : virtual public IMediaPipelineReporter
{
public:
    virtual ~IMediaSourcePlaybackReporter() {}

    virtual void setPlaybackQualityStats(const device::IPlaybackDevice::PlaybackQualityStats& audioStats,
                                         const device::IPlaybackDevice::PlaybackQualityStats& videoStats) = 0;
    /**
     * Notified when the skips forward/backward starts
     */
    virtual void skipStarted() = 0;

    /**
     * Notified when a key startplay event completed
     */
    virtual void reportKeyStartplayEvent( std::string const& eventName ) = 0;
};

}}

#endif // __IMEDIASOURCE_PLAYBACK_REPORTER_H__
