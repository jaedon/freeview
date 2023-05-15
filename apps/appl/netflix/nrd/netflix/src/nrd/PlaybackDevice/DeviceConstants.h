/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#ifndef DEVICECONSTANTS_H
#define DEVICECONSTANTS_H

#include <nrd/IElementaryStreamPlayer.h>
#include <nrd/DrmManager.h>

namespace netflix {
namespace device {

class DeviceConstants
{
public:

    // PlaybackDevice settings

    // The blocks delivered via PlaybackDevice::feedData are enqueud in a
    // ProducerConsumerList. The ProducerConsumerList preallocates a
    // fixed-size pool of container nodes that can be added to the list.
    // These constants specify the size of the pool for audio and for
    // video. They have the effect of setting the max number of McSink data
    // blocks that can be in use by the PlaybackDevice at any one time.
    static const uint32_t MAX_NUM_AUDIO_DATA_BLOCKS_IN_USE;
    static const uint32_t MAX_NUM_VIDEO_DATA_BLOCKS_IN_USE;

    // Misc Settings
    static uint32_t PTS_EVENT_PERIOD; // minimum timestamp difference between PTS_UPDATE events
    static uint32_t EVENT_QUEUE_SIZE;

    static const int32_t DEFAULT_TIMESCALE;

    static const DrmManager::uuid_t PLAYREADY_SYSTEM_ID;
    static const DrmManager::uuid_t NETFLIX_SYSTEM_ID;
};


} // namespace netflix::device
} // namespace netflix

#endif
