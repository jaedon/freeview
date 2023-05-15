/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

/** @file IAudioMixerOutput.h The header file for the audio mixer output.
 *
 *  ## Overview
 *  The Netflix application can have sound effects or music outside of the
 *  streamed video asset.  For example, sound effects on keypresses or music while in
 *  the content selection screen. This DPI defines how to handle and output
 *  these sound effects (potentially with streamed video playback).
 *
 *  ## Flow
 *  When required, the Netflix app instantiates an instance of `IAudioMixer` with
 *  a pointer that's passed to an `IAudioMixerOutput` instance. `IAudioMixer` can decode and
 *  mix various source materials and will route the decoded PCM to the `IAudioMixerOutput`.
 *
 *  Your implementation of `IAudioMixerOutput` takes the PCM data and route
 *  it to the device's applicable output interfaces like SPDIF, HDMI or DAC output.
 *
 *  ## Lifetime
 *  The lifetime of `IAudioMixer` and `IAudioMixerOutput` is not limited to when it will be fed
 *  data.  For instance, `IAudioMixer` and `IAudioMixerOutput` might exist when the
 *  device is streaming and sending E-AC3 as compressed data.  Since compressed
 *  streams and PCM cannot be mixed, you should ignore the PCM content.
 *
 *  ## Implementation
 *  Partners do not need to change the `IAudioMixer` implementation and only need
 *  to implement `IAudioMixerOutput`. The PCM fed to the partner interface is always:
 *
 *  - 48 KHz
 *  - Stereo (2-channel)
 *  - 16-bits/sample
 *  - L/R channel interleaved
 *  - Device endian
 *
 *  The `IAudioMixer` has a mixing thread that calls `getBuffer()`
 *  to acquire a pointer to copy the PCM data into that buffer.
 *  After writing the buffer, `commitBuffer()` lets the partner
 *  API know how much data has been written and will come in pairs.
 *  There will not be consecutive `getBuffer()` or consecutive `commitBuffer()` calls.
 *
 *  One frame is defined to be 4 bytes (Left 16-bit sample + Right 16-bit sample)
 */

#ifndef NRDP_AUDIOMIXEROUTPUT_H
#define NRDP_AUDIOMIXEROUTPUT_H

#include <nrdbase/NFErr.h>
#include <nrdbase/DataBuffer.h>

namespace netflix {
namespace device {
/*
 * @class netflix::device::IAudioMixerOutput
 * @brief Interface for outputting PCM audio samples.
 */
class IAudioMixerOutput
{
public:

    /** Free all mixer resources */
    virtual ~IAudioMixerOutput() {}

    /**
     * The partner API should move into the "play" state and start playing PCM samples.
     *
     * Note that `getBuffer()` and `commitBuffer()` calls can be made before the device has been
     * issued a Play() call to mitigate the risk of underflow at the outset.
     *
     * @return NFErr_OK if moved into "play" state.
     * @return NFErr_Bad if not moved into "play" state.
     */
    virtual NFErr Play(void) = 0;

    /**
     * The platform should play out PCM data in the partner FIFO then stop.  This call
       signifies there are no audio buffers to be sent.
     *
     * @return NFErr_OK if moved into "reset" state.
     * @return NFErr_Bad if not moved into "reset" state.
     */
    virtual NFErr Reset(void) = 0;

    /**
     * Get information on buffer memory sizes.
     *
     * This call is used to determined the latency of the device and the preferred
     * buffer transfer size.  The preferred amount is a recommendation to the mixer
     * that will affect the frequency of calls to transfer data.  It is required
     * to be a multiple of 64 frames (256 bytes).
     *
     * It is required that a small transfer size and small PCM FIFO are
     * used to reduce audio latency.  Recommended sizes may be platform dependent
     * but transfer size of 640 frames and FIFO size of 2560 frames should
     * give good audio responsiveness.
     *
     * Example sizes are provided:
     *
     *      21 us   of LPCM         4 bytes  =      1 frame
     *      13 ms   of LPCM     2,560 bytes  =    640 frames
     *      27 ms   of LPCM     5,120 bytes  =  1,280 frames
     *      40 ms   of LPCM     7,680 bytes  =  1,920 frames
     *     100 ms   of LPCM    19,200 bytes  =  4,800 frames
     *
     * @param[out] preferredFrames         The device's preferred transfer amount.
     * @param[out] availableSize           The total amount of frames device buffer points to.
     * @param[out] supportDeferredPlay     If the device allows buffers to be copied before being issued a play.
     *
     */
    virtual void getDeviceInfo(size_t& preferredFrames, size_t& maximumFrames, bool& supportDeferredPlay) = 0;

    /**
     * Get amount of frames that can be committed without blocking.
     *
     * It is valid to call this function when the device is in either play or reset
     * state.  This call is made to determine if subsequent getBuffer/commitBuffer calls
     * will be able to commit frames without blocking.
     *
     * @param[out] availableFrames  The amount of frames committable without blocking.
     *
     * @return NFErr_OK if the device successfully queried the amount of available frames.
     * @return NFErr_Bad if the device failed to determined amount of available frames.
     */
    virtual NFErr getBufferAvailable(size_t& availableFrames) = 0;

    /**
     * Get the time in frames that a newly committed frame would take to be audible.
     *
     * It is valid to call this function when the device is in either play or reset
     * state.  This call is made to determine when the audio is audible for state tracking.
     *
     * @param[out] delayFrames  The amount of frames it takes for a new sample to be audible.
     *
     * @return NFErr_OK if the device successfully queried the amount of delay.
     * @return NFErr_Bad if the device failed to determined amount of delay.
     */
    virtual NFErr getBufferDelay(size_t& delayFrames) = 0;

    /**
     * Get a buffer to copy the PCM data into.
     * This method:
     * - Can be called in the play or reset state.
     * - Will always be called before commitBuffer().
     * - Will not be called repeatedly.
     *
     * @param[out] bufferPtr        Pointer to buffer audio mixer can write to.
     * @param[in]  requestedFrames  The amount of frames requested by audio mixer.
     * @param[out] contiguousFrames The amount of contiguous frames the buffer points to.
     *                              This number can not exceed requestedSize.
     *
     * @return NFErr_OK if the buffer provided has at least requestedFrames.
     * @return NFErr_Bad if the device does not have at least requestedFrames.
        bufferPtr and contiguousFrames do not need to be set in this case and will be unused.
     */
    virtual NFErr getBuffer(void ** bufferPtr, const size_t requestedFrames, size_t& contiguousFrames) = 0;

    /**
     * Amount of frames the audio mixer has written to the buffer pointer returned by `getBuffer()`.
     * This method:
     * - Can be called in either play or reset state.
     * - Will always be called after getBuffer().
     * - Will not be called repeatedly.
     *
     * @param[in]  framesWritten    The amount of frames written.
     *
     * @return NFErr_OK if the device was able to commit the audio frames.
     * @return NFErr_Bad if the device is unable to commit the audio frames.
     */
    virtual NFErr commitBuffer(const size_t framesWritten) = 0;
};

}} // namespace netflix::device


#endif // NRDP_AUDIOMIXEROUTPUT_H
