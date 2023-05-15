/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef DEVICE_RENDERERMANAGER_H
#define DEVICE_RENDERERMANAGER_H

#include <nrd/IElementaryStreamPlayer.h>
#include <nrdbase/Mutex.h>
#include "ReferenceClockNative.h"

/* @file RendererManager.h - Defines a class used in the reference application's
 * implementations of IElementaryStreamManager and IElementaryStreamPlayer to
 * handle reference-implementation-specific initialization of audio and video
 * renderers. The file does not implement a DPI interface.
 */

namespace netflix {
namespace device {
namespace esplayer {

/**
 * @class RendererManager RendererManager.h
 * @brief Manager of audio/video renderers
 */

class RendererManager
{
public:
    ~RendererManager();
#if 0
    /**
     * Create audio renderer.
     *
     * @param[in] referenceClock the reference clock.
     * @param[in] samplesPerSecond the output sampling frequency.
     * @param[in] outputBufferSamples the output buffer size in number of samples.
     * @param[in] maxFrameSamples the maximum number of samples that can be decoded each time.
     * @param[in] maxNumMaxFrames the internal sample buffer size in number of maximum-size-frames.
     * @param[in] timestampQueueSize size of the internal timestamp queue in entries.
     * @param[in] preferredNumOutputChannels the preferred number of output channels.
     *
     * @return pointer of the createialized audio renderer if succeeded; NULL otherwise.
     */
    shared_ptr<AudioRenderer> createAudioRenderer(ReferenceClockNative& referenceClock,
                                                  AudioESPlayerNative& ESPlayer,
                                                  uint32_t samplemPerSecond,
                                                  uint32_t outputBufferSamples,
                                                  uint32_t maxFrameSamples, uint32_t maxNumMaxFrames,
                                                  uint32_t timestampQueueSize,
                                                  uint32_t preferredNumOutputChannels = 2);

     /**
     * Create video renderer.
     *
     * @param[in] attributes the video attributes.
     * @param[in] maxImageWidth the maximum width (in pixels) of all possible images.
     * @param[in] maxImageHeight the maximum height (in pixels) of all possible images.
     *
     * @return pointr of the createialized video renderer if succeeded; NULL otherwise.
     */
    shared_ptr<VideoRenderer> createVideoRenderer(const VideoAttributes& attributes,
                                                  uint32_t maxImageWidth,
                                                  uint32_t maxImageHeight,
                                                  uint32_t maxFrameRateValue,
                                                  uint32_t maxFrameRateScale,
                                                  uint32_t pipelineId);
#endif
};

} // namespace esplayer
} // namespace device
} // namespace netflix

#endif // DEVICE_RENDERERMANAGER_H
