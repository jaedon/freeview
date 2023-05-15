/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef DEVICE_AUDIORENDEREROMXIL_H
#define DEVICE_AUDIORENDEREROMXIL_H

#include "../AudioRenderer.h"
#include "ComponentOMXIL.h"

class AudioDeviceThreadOMXIL;

namespace netflix {
namespace device {
namespace esplayer {

/**
 * @class AudioRendererOMXIL AudioRendererOMXIL.h
 * @brief Implementation of openmax audio rendering.
 */
class AudioRendererOMXIL : public AudioRenderer, private ComponentOMXIL
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
    AudioRendererOMXIL(ReferenceClockNative& referenceClock,
                      AudioESPlayerNative& ESPlayer,
                      uint32_t samplesPerSecond,
                      uint32_t outputBufferSamples,
                      uint32_t maxFrameSamples, uint32_t maxNumMaxFrames,
                      uint32_t timestampQueueSize,
                      uint32_t preferredNumOutputChannels);

    /**
     * Destructor.
     */
    virtual ~AudioRendererOMXIL();

    virtual void writeSamples(unsigned char* dataSeg1, uint32_t bytesSeg1,
                              unsigned char* dataSeg2, uint32_t bytesSeg2,
                              uint32_t bytesSilence, void* context);

    virtual void resetAudioDevice(uint32_t samplesPerSecond);

    void audioDeviceTask(const Time &wait);

protected:
    void reset();

    uint32_t mConsumed;
    ComponentOMXIL mRenderer;
    AudioDeviceThreadOMXIL *mThread;

};

} // namespace esplayer
} // namespace device
} // namespace netflix

#endif // DEVICE_AUDIORENDEREROMXIL_H
