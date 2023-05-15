/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#include "DeviceConstants.h"

using namespace netflix;
using namespace netflix::device;


// PlaybackDevice settings

// The blocks delivered via PlaybackDevice::feedData are enqueud in a
// ProducerConsumerList. The ProducerConsumerList preallocates a
// fixed-size pool of container nodes that can be added to the list.
// These constants specify the size of the pool for audio and for
// video. They have the effect of setting the max number of McSink data
// blocks that can be in use by the PlaybackDevice at any one time.
const uint32_t DeviceConstants::MAX_NUM_AUDIO_DATA_BLOCKS_IN_USE = 512;
const uint32_t DeviceConstants::MAX_NUM_VIDEO_DATA_BLOCKS_IN_USE = 4096;

// Misc Settings
uint32_t DeviceConstants::PTS_EVENT_PERIOD             = 500; // minimum timestamp difference between PTS_UPDATE events
uint32_t DeviceConstants::EVENT_QUEUE_SIZE             = 64;

// Other Constants
const int32_t  DeviceConstants::DEFAULT_TIMESCALE =  1000; // In ticks per second.

const DrmManager::uuid_t DeviceConstants::PLAYREADY_SYSTEM_ID =
{
    0x9A, 0x04, 0xF0, 0x79,
    0x98, 0x40, 0x42, 0x86,
    0xAB, 0x92, 0xE6, 0x5B,
    0xE0, 0x88, 0x5F, 0x95
};

const DrmManager::uuid_t DeviceConstants::NETFLIX_SYSTEM_ID =
{
    0x29, 0x70, 0x1F, 0xE4,
    0x3C, 0xC7, 0x4A, 0x34,
    0x8C, 0x5B, 0xAE, 0x90,
    0xC7, 0x43, 0x9A, 0x47
};
