/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef DEVICE_AUDIOMIXEROUTPUT_SINK_H
#define DEVICE_AUDIOMIXEROUTPUT_SINK_H

#include <nrd/IAudioMixerOutput.h>

namespace netflix {
namespace device {

class AudioMixerOutputSink : public IAudioMixerOutput
{
public:

    AudioMixerOutputSink();
    ~AudioMixerOutputSink();

    virtual NFErr Play();

    virtual NFErr Reset();

    virtual NFErr getBuffer(void ** buffer_ptr, const size_t requestedFrames, size_t& contiguousFrames);

    virtual NFErr commitBuffer(const size_t writtenFrames);

    virtual NFErr getBufferStatus(size_t& availableFrames);

    virtual void  getBufferInfo(size_t& preferredFrames, size_t& maximumFrames);

private:
    uint32_t framesLeftToPlayOut();

    static uint32_t msToFrames(const uint32_t ms) { return (ms * 48); }

    static const size_t PREFERRED_FRAME_SIZE = 768;
    static const size_t FIFO_SIZE_IN_MS = 50;

    std::vector<char>   mBuffer;
    bool                mPlaying;
    uint32_t            mPlayStart;
    uint32_t            mCommittedFrames;
};

} // namespace device
} // namespace netflix

#endif // DEVICE_AUDIOMIXEROUTPUT_SINK_H
