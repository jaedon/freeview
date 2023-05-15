/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef DEVICE_AUDIOMIXEROUTPUT_ALSA_H
#define DEVICE_AUDIOMIXEROUTPUT_ALSA_H

#include <nrd/IAudioMixerOutput.h>
#include <nrdbase/tr1.h>
#include <alsa/asoundlib.h>

namespace netflix {
namespace device {

class AudioMixerOutputALSA : public IAudioMixerOutput
{
public:

    AudioMixerOutputALSA();
    ~AudioMixerOutputALSA();

    virtual void  getDeviceInfo(size_t& preferredFrames, size_t& maximumFrames, bool& supportDeferredPlay);

    virtual NFErr Play();

    virtual NFErr Reset();

    virtual NFErr getBufferAvailable(size_t& availableFrames);

    virtual NFErr getBufferDelay(size_t& delayFrames);

    virtual NFErr getBuffer(void ** buffer_ptr, const size_t requestedFrames, size_t& contiguousFrames);

    virtual NFErr commitBuffer(const size_t writtenFrames);

private:
    bool init(void);

    void Recover(const char * s, int err);

    snd_pcm_t *         mHandle;
    std::vector<char>   mBuffer;
    bool                mRecoverFromUnderflow;

    // 16 ms of LPCM =  768 frames =  3072 bytes
    // 32 ms of LPCM = 1536 frames =  6144 bytes
    // 48 ms of LPCM = 2304 frames =  9216 bytes
    // 64 ms of LPCM = 3072 frames = 12288 bytes
    static const size_t PREFERRED_FRAME_SIZE = 768;
    static const size_t ALSA_LATENCY_FRAMES  = PREFERRED_FRAME_SIZE * 4;
};

} // namespace device
} // namespace netflix

#endif // DEVICE_AUDIOMIXEROUTPUT_ALSA_H
