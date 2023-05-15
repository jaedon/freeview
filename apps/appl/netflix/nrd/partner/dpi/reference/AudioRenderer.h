/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef DEVICE_AUDIORENDERER_H
#define DEVICE_AUDIORENDERER_H

/* @file AudioRenderer.h - Audio renderer base class used in the reference
 * application's audio elementary stream player implementation. The file does
 * not implement a DPI interface. It provides reference-implementation-specific
 * audio rendering functionality. The reference application provides several
 * audio renderers that each implement the audio renderer interface defined
 * here.
 */

#include "AudioESPlayerNative.h"
#include <nrdbase/Mutex.h>
#include "DecoderStatsCounter.h"

namespace netflix {
namespace device {
namespace esplayer {


class ReferenceClockNative;


/**
 * @class AudioRenderer AudioRenderer.h
 * @brief Base implementation for audio renderer engines.
 */
class AudioRenderer
{
public:

protected:
    friend class RendererManager;


    /**
     * Constructor.
     *
     * @param[in] referenceClock the reference clock.
     */
    AudioRenderer(ReferenceClockNative& referenceClock,
                  AudioESPlayerNative& ESPlayer);

    /**
     * Destructor.
     */
    virtual ~AudioRenderer();

public:

    /**
     * Update sampling rate of the underlying audio device.
     *
     * @param[in] samplesPerSecond the new sampling rate.
     */
    void updateSamplingRate(uint32_t samplesPerSecond);

    /**
     * Return the number of output channels.
     *
     * @return number of output channels.
     */
    uint32_t getNumOutputChannels() const { return numOutputChannels_; }

    /**
     * Get a buffer from the audio renderer, for storing decoded audio samples.
     * It's guaranteed to have capacity for "maxFrameSamples" (an argument of init()).
     *
     * @param[out] sampleBuffer to receive the sample-buffer.
     *
     * @return true if successful, false if the free sample space in audio renderer is not enough.
     */
    bool getSampleBuffer(AudioESPlayerNative::DecodedAudioBuffer& sampleBuffer);

    /**
     * Send a sample-buffer (in which we stored decoded audio samples) into the audio renderer's
     * internal storage, so these samples can be fetched when it is time.
     *
     * @param[in] sampleBuffer the sample-buffer to be sent into audio renderer.
     *
     * @return true if sample enqueued for rendering, false if dropped because
     *         it's too late, the buffer is invalid, or has a bad address.
     */
    bool sendSampleBuffer(AudioESPlayerNative::DecodedAudioBuffer& sampleBuffer);

    /**
     * Set the state of flush operation (inactive/has-begun/has-ended).
     *
     * @param[in] flushState the new flush operation state to be set.
     */
    virtual void setFlushState(ESPlayerNative::FlushState flushState);

    /**
     * Get the state of flush operation (inactive/has-begun/has-ended).
     *
     * @return the current flush operation state.
     */
    ESPlayerNative::FlushState getFlushState();


    /**
     * Set or reset the state of input data exhaustion.
     *
     * @param[in] inputExhausted true to set the exhaustion state; false to reset.
     */
    void setInputExhausted(bool inputExhausted) { inputExhausted_ = inputExhausted; }

    /**
     * Check whether the input data is exhausted.
     *
     * @return true if the input data is exhausted; false otherwise.
     */
    bool isInputExhausted() { return inputExhausted_; }

    /**
     * Gives a snapshot of the current rendering position.
     */
    void getTimestampOfNextSampleToRender(int64_t& timestamp);

    /**
     * Tells the timestamp of the last sample that was sent to the renderer and enqueued.
     */
    void getTimestampOfLastDeliveredSample(int64_t& timestamp);

    /**
     * Start a fade
     */
    void setFade(double target, uint32_t duration, IAudioMixer::Ease ease);

    /**
     * Get the current volume
     */
    double getCurrentVolume();

protected:

    /**
     * Finish initialization of the audio renderer (Child class should call this function to
     * finish the renderer initialization procedure).
     *
     * @param[in] samplesPerSecond the output sampling frequency.
     * @param[in] numOutputChannels the number of output channels.
     * @param[in] outputBufferSamples the output buffer size in number of samples.
     * @param[in] maxFrameSamples the maximum number of samples that can be decoded each time.
     * @param[in] maxNumMaxFrames the internal sample buffer size in number of maximum-size-frames.
     * @param[in] timestampQueueSize size of the internal timestamp queue in entries.
     */
    void init(uint32_t samplesPerSecond,
              uint32_t numOutputChannels, uint32_t outputBufferSamples,
              uint32_t maxFrameSamples, uint32_t maxNumMaxFrames,
              uint32_t timestampQueueSize);

    /**
     * Child class should call this function regularly (from the ISR or device callback) when samples
     * space is available in the final output buffer, in order to play sound of the audio samples from
     * the internal sample buffer. This function calls writeSamples() to write samples data into the
     * final output buffer.
     *
     * @param[in] bytes size of the available samples space, in number of bytes.
     * @param[in] context context for device writing, actual meaning is defined by the child classes.
     */
    void play(uint32_t bytes, void* context);



    /**
     * To write audio samples into the final output buffer for playing sound.
     * This should be implemented by the child classes.
     *
     * @param[in] dataSeg1 data of samples for segment 1.
     * @param[in] bytesSeg1 data size of samples for segment 1, in bytes.
     * @param[in] dataSeg2 data of samples for segment 2.
     *            (it should be NULL except when internal sample buffer is wrapping-around)
     * @param[in] bytesSeg2 data size of samples for segment 2, in bytes.
     *            (it should be 0 except when internal sample buffer is wrapping-around)
     * @param[in] bytesSilence data size of audio silence to append, in bytes.
     * @param[in] context context for device writing, actual meaning is defined by the child classes.
     */
    virtual void writeSamples(unsigned char* dataSeg1, uint32_t bytesSeg1,
                              unsigned char* dataSeg2, uint32_t bytesSeg2,
                              uint32_t bytesSilence, void* context) = 0;

    /**
     * Reset the underlying audio device with the new sampling rate.
     *
     * @param[in] samplesPerSecond the new sampling rate.
     */
    virtual void resetAudioDevice(uint32_t samplesPerSecond) = 0;


private: // helpers

    // @return number of timestamps (audio frames) dequeued
    int dequeueTimestampsInRange(uint32_t rpBegin, uint32_t rpEnd,
                                  int64_t& latestTimestamp,
                                  uint32_t& latestTimestampLocation);

private:
    AudioESPlayerNative& mESPlayer;
    uint32_t numOutputChannels_;
    uint32_t maxFrameSize_;
    int64_t timestampOfLastConsumedSample_;
    int64_t timestampOfLastDeliveredSample_;


    ESPlayerNative::FlushState flushState_;     // SDK thread, renderer thread.
    bool                       inputExhausted_; // Accessed only from the
                                                // renderer thread.
    // For internal sample buffer handling
    uint32_t       wp_;    // Accessed from decoder and renderer threads.
    uint32_t       rp_;    // Accessed only from the renderer thread.
    uint32_t       size_;  // Accessed only from the decoder thread.
    unsigned char* data_;  // Accessed by decoder and renderer threads

    // For timestamp queue handling. Accessed from decoder and renderer threads.
    //
    struct TimestampQueueItem
    {
        int64_t timestamp_;
        uint32_t wp_;
    };
    uint32_t wpTsQueue_;
    uint32_t rpTsQueue_;
    uint32_t timestampQueueSize_;
    TimestampQueueItem* timestampQueue_;

    // pre-calculated values
    //
    uint32_t ticks_65536th_PerByte_;
    uint32_t bytes_256th_PerTick_;

    void Ease(short * buffer, const size_t samples);

    IAudioMixer::Ease mEase;
    double mStartVolume;
    double mCurrentVolume;
    double mTargetVolume;
    int mEaseTimeElapsed;
    int mEaseTransitionFrames;

protected:

    uint32_t getBytes256thPerTick()
    {
        return bytes_256th_PerTick_;
    }

    // a sample-block is the collection of samples of all channels for a single time-point.
    ReferenceClockNative& referenceClock_;
    uint32_t    bytesPerSampleBlock_;
    int32_t     bytesPerSampleBlock_Po2_; // the number of bytes per sample-block in power-of-two.
    uint32_t    outputBufferSize_;
    Mutex mutex_;
};

} // namespace esplayer
} // namespace device
} // namespace netflix

#endif // DEVICE_AUDIORENDERER_H
