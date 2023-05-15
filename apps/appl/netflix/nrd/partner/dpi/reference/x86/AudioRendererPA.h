/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef DEVICE_AUDIORENDERERPA_H
#define DEVICE_AUDIORENDERERPA_H

#include "../AudioRenderer.h"

// PA headers
#include <portaudio.h>

namespace netflix {
namespace device {
namespace esplayer {

/**
 * @class AudioRendererPA AudioRendererPA.h
 * @brief Implementation of audio rendering over PA.
 */
class AudioRendererPA : public AudioRenderer
{
public:
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
    AudioRendererPA(ReferenceClockNative& referenceClock,
                    AudioESPlayerNative& ESPlayer,
                    uint32_t samplesPerSecond,
                      uint32_t outputBufferSamples,
                      uint32_t maxFrameSamples, uint32_t maxNumMaxFrames,
                      uint32_t timestampQueueSize,
                      uint32_t preferredNumOutputChannels);

    /**
     * Destructor.
     */
    virtual ~AudioRendererPA();

    virtual void writeSamples(unsigned char* dataSeg1, uint32_t bytesSeg1,
                              unsigned char* dataSeg2, uint32_t bytesSeg2,
                              uint32_t bytesSilence, void* context);

    virtual void resetAudioDevice(uint32_t samplesPerSecond);

    virtual void setFlushState(ESPlayerNative::FlushState flushState);

    bool isDeviceClosed;

private:
    PaStream*         stream_;

private:
    void open(uint32_t samplesPerSecond);
    void close();

    int callback(const void *input,
                 void *output,
                 unsigned long frameCount,
                 const PaStreamCallbackTimeInfo *timeInfo,
                 PaStreamCallbackFlags statusFlags);

    static int staticCallback(const void *input,
                              void *output,
                              unsigned long frameCount,
                              const PaStreamCallbackTimeInfo *timeInfo,
                              PaStreamCallbackFlags statusFlags,
                              void *userData);

    static void staticFinishedCallback(void *userData);
};

} // namespace esplayer
} // namespace device
} // namespace netflix

#endif // DEVICE_AUDIORENDERERPA_H
