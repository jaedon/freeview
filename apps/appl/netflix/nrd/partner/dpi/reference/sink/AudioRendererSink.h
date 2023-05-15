/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef DEVICE_AUDIORENDERERSINK_H
#define DEVICE_AUDIORENDERERSINK_H

#include "../AudioRenderer.h"

namespace netflix {
namespace device {
namespace esplayer {


/**
 * @class AudioRendererSink AudioRendererSink.h
 * @brief Implementation of sink audio rendering.
 */
class AudioRendererSink : public AudioRenderer
{
public:
    //friend class RendererManager;

    /**
     * Constructor.
     *
     * @param[in] referenceClock the reference clock.
     * @param[in] samplesPerSecond the output sampling frequency.
     * @param[in] outputBufferSamples the output buffer size in number of samples.
     * @param[in] maxFrameSamples the maximum number of samples that can be decoded each time.
     * @param[in] maxNumMaxFrames the internal sample buffer size in number of maximum-size-frames.
     * @param[in] timestampQueueSize size of the internal timestamp queue in entries.
     * @param[in] preferredNumOutputChannels the preferred number of output channels.
     */
    AudioRendererSink(ReferenceClockNative& referenceClock,
                      AudioESPlayerNative& ESPlayer,
                      uint32_t samplesPerSecond,
                      uint32_t outputBufferSamples,
                      uint32_t maxFrameSamples, uint32_t maxNumMaxFrames,
                      uint32_t timestampQueueSize,
                      uint32_t preferredNumOutputChannels);

    /**
     * Destructor.
     */
    virtual ~AudioRendererSink();

    virtual void writeSamples(unsigned char* dataSeg1, uint32_t bytesSeg1,
                              unsigned char* dataSeg2, uint32_t bytesSeg2,
                              uint32_t bytesSilence, void* context);

    virtual void resetAudioDevice(uint32_t samplesPerSecond);

private:
    class SinkAudioDeviceThread;
    SinkAudioDeviceThread* deviceThread_;
    volatile bool consumingSamples_;

    void audioDeviceTask();
};

} // namespace esplayer
} // namespace device
} // namespace netflix

#endif // DEVICE_AUDIORENDERERSINK_H
