/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "AudioRenderer.h"
#include "RendererManager.h"
#include "ESPlayerConstants.h"
#include <cstdlib>
#include <cstring>
#include "ReferenceClockNative.h"
#include <nrd/AppLog.h>
#include <nrd/IAudioMixer.h>
#include <nrd/IPlaybackDevice.h>
#include <nrdbase/ScopedMutex.h>
#include <nrdbase/MutexRanks.h>


using namespace netflix::device::esplayer;
using namespace netflix::device;
using namespace netflix;

AudioRenderer::AudioRenderer(ReferenceClockNative& referenceClock,
                             AudioESPlayerNative& ESPlayer)
    : mESPlayer(ESPlayer),
      numOutputChannels_(0), maxFrameSize_(0),
      timestampOfLastConsumedSample_(INVALID_TIMESTAMP),
      timestampOfLastDeliveredSample_(INVALID_TIMESTAMP),
      flushState_( ESPlayerNative::FLUSH_OFF), inputExhausted_(true),
      wp_(0), rp_(0), size_(0), data_(NULL),
      wpTsQueue_(0), rpTsQueue_(0), timestampQueueSize_(0), timestampQueue_(NULL),
      mEase(IAudioMixer::EaseLinear), mStartVolume(1.0), mCurrentVolume(1.0), mTargetVolume(1.0), mEaseTimeElapsed(0), mEaseTransitionFrames(0),
      referenceClock_(referenceClock),
      bytesPerSampleBlock_(0), bytesPerSampleBlock_Po2_(-1), outputBufferSize_(0),
      mutex_(ESP_RENDERER_MUTEX, "Audio Renderer Mutex")
{
}

AudioRenderer::~AudioRenderer()
{
    if(data_ != NULL)
        delete [] data_;

    if(timestampQueue_ != NULL)
        delete [] timestampQueue_;
}

void AudioRenderer::updateSamplingRate(
        uint32_t samplesPerSecond)
{
    if(samplesPerSecond == 0) {
        fprintf(stderr, "audio sampling rate shouldn't be zero");
        return;
    }

    ticks_65536th_PerByte_ = TIMESTAMP_FREQ_HZ * 65536 / (bytesPerSampleBlock_ * samplesPerSecond);
    bytes_256th_PerTick_ = bytesPerSampleBlock_ * samplesPerSecond * 256 / TIMESTAMP_FREQ_HZ;

    resetAudioDevice(samplesPerSecond);
}

// Called from the audio decoder thread.
bool AudioRenderer::getSampleBuffer(AudioESPlayerNative::DecodedAudioBuffer& sampleBuffer)
{
    ScopedMutex cs(mutex_);

    assert(numOutputChannels_ != 0); // init() should have been called by child class.

    uint32_t next = wpTsQueue_ + 1;
    if(next >= timestampQueueSize_)
        next = 0;

    if(next == rpTsQueue_)
        return false; // timestamp queue has no space

    uint32_t space, rp_snapshot = rp_;

    if(rp_snapshot > wp_)
        space = rp_snapshot - wp_ - 1;
    else
        space = size_ + rp_snapshot - wp_ - 1;

    if(space < maxFrameSize_)
        return false; // sample buffer doesn't have enough space


    sampleBuffer.data_      = &data_[wp_];
    sampleBuffer.size_      = maxFrameSize_;
    sampleBuffer.dataBytes_ = 0;
    sampleBuffer.timestamp_ = INVALID_TIMESTAMP;
    return true;
}

// Called from the audio decoder thread.
bool AudioRenderer::sendSampleBuffer(AudioESPlayerNative::DecodedAudioBuffer& sampleBuffer)
{
    ScopedMutex cs(mutex_);

    assert(numOutputChannels_ != 0); // init() should have been called by child class.

    if(sampleBuffer.size_ == 0) {
        fprintf(stderr, "attempt to send an invalid sample buffer");
        return false;
    }

    if(sampleBuffer.data_ != &data_[wp_]) {
        fprintf(stderr, "attempt to send a sample buffer with bad data address");
        return false;
    }

    // FIXME: === BEGIN ===
    //        This is an over-simplified version of audio sample dropping implementation after audio track switch.
    //        It can be improved by:
    //          - Finer grain control in the amount of samples to drop.
    //          - Handle the case when some sample-block might not have a valid timestamp attached.
    //          - Make the audio sample dropping period explicit so that normal rendering operation can not be affected.
    //
    int64_t pts = referenceClock_.getCurrPresentationTime();
    if(pts != INVALID_TIMESTAMP && sampleBuffer.timestamp_ != INVALID_TIMESTAMP && sampleBuffer.timestamp_ < pts)
    {
        sampleBuffer.size_ = 0;

        Log::warn(TRACE_MEDIAPLAYBACK, "AudioRenderer: Dropping sample buffer in renderer. Sample PTS: %llu, "
               "currPresentationTime: %llu", sampleBuffer.timestamp_, pts);
        return false;
    }
    // FIXME: ==== END ====

    timestampOfLastDeliveredSample_ = sampleBuffer.timestamp_;

    if(sampleBuffer.dataBytes_ > 0)
    {
        // always enqueue timestamp information first
        //

        if(sampleBuffer.timestamp_ != INVALID_TIMESTAMP)
        {
            timestampQueue_[wpTsQueue_].timestamp_ = sampleBuffer.timestamp_;
            timestampQueue_[wpTsQueue_].wp_ = wp_;

            uint32_t next = wpTsQueue_ + 1;
            if(next >= timestampQueueSize_)
                next = 0;
            wpTsQueue_ = next;
        }

        // then move the sample buffer WP (make sample data available to the receiver)
        //

        if(wp_ + sampleBuffer.dataBytes_ >= size_) // wrap-around
        {
            uint32_t half = wp_ + sampleBuffer.dataBytes_ - size_;

            if(half > 0) // move data from trailing space to the head of sample buffer
                memcpy(&data_[0], &data_[size_], half);

            wp_ = half;
        }
        else
            wp_ += sampleBuffer.dataBytes_;
    }

    sampleBuffer.size_ = 0;
    return true;
}

void AudioRenderer::init(
        uint32_t samplesPerSecond,
        uint32_t numOutputChannels, uint32_t outputBufferSamples,
        uint32_t maxFrameSamples, uint32_t maxNumMaxFrames,
        uint32_t timestampQueueSize)
{
    assert(numOutputChannels_ == 0); // init() can only be called once per object

    assert(numOutputChannels   != 0 &&
           outputBufferSamples != 0 &&
           maxFrameSamples     != 0 &&
           maxNumMaxFrames     != 0 &&
           timestampQueueSize  != 0 );

    if(samplesPerSecond == 0) {
        fprintf(stderr, "audio sampling rate shouldn't be zero");
        return;
    }

    // a sample-block is the collection of samples of all channels for a single time-point.
    uint32_t bitsPerSampleBlock = numOutputChannels * ESPlayerConstants::AUDIO_OUTPUT_BITS;

    if((bitsPerSampleBlock & 0x7) != 0) {
        fprintf(stderr, "can't handle non-byte-aligned audio sample blocks");
        return;
    }

    bytesPerSampleBlock_ = (bitsPerSampleBlock >> 3);
    bytesPerSampleBlock_Po2_ = -1;

    int i;
    for(i = 0; i <= 8; i ++) // check if the sample-block-size happens to be 2^N between 1 and 256.
    {
        if(bytesPerSampleBlock_ == (1U << i))
        {
            bytesPerSampleBlock_Po2_ = i;
            break;
        }
    }

    // set parameters
    //

    outputBufferSize_   = bytesPerSampleBlock_ * outputBufferSamples;
    maxFrameSize_       = bytesPerSampleBlock_ * maxFrameSamples;
    size_               = maxFrameSize_ * maxNumMaxFrames;
    data_               = new unsigned char[size_ + maxFrameSize_]; // added trailing space
    timestampQueueSize_ = timestampQueueSize;
    timestampQueue_     = new TimestampQueueItem[timestampQueueSize];

    // pre-calculate some values
    //

    // the corresponding time length in 1/65536 timestamp-ticks for each decoded data byte
    ticks_65536th_PerByte_ = TIMESTAMP_FREQ_HZ * 65536 / (bytesPerSampleBlock_ * samplesPerSecond);

    // the corresponding data amount in 1/256 bytes for each timestamp-tick
    bytes_256th_PerTick_ = bytesPerSampleBlock_ * samplesPerSecond * 256 / TIMESTAMP_FREQ_HZ;

    // numOutputChannels_ is validated as the last step, so that its validity indicates
    // that the audio renderer has been initialized properly.

    numOutputChannels_  = numOutputChannels;
}

float EaseNext(IAudioMixer::Ease ease, float t, float b, float c, float d)
{
    switch (ease) {
    default:
    case IAudioMixer::EaseLinear:
        return c*t/d + b;
    case IAudioMixer::EaseInCubic:
        t/=d;
        return c*t*t*t + b;
    case IAudioMixer::EaseOutCubic:
        t=t/d-1;
        return c*(t*t*t + 1) + b;
    }
}

void AudioRenderer::Ease(short * buffer, const size_t samples)
{
    float delta = mTargetVolume - mStartVolume;

    for (size_t i = 0; i < samples; i += 2) {

        if (mEaseTransitionFrames && (mEaseTimeElapsed < mEaseTransitionFrames)) {
            mCurrentVolume = EaseNext(mEase, mEaseTimeElapsed, mStartVolume, delta, mEaseTransitionFrames - 1);
        }

        buffer[i]   *= mCurrentVolume;
        buffer[i+1] *= mCurrentVolume;

        if (mEaseTimeElapsed < mEaseTransitionFrames) {
            mEaseTimeElapsed++;
        }
    }
}

// Called from the audio renderer thread -- a portaudio thread, for example.
void AudioRenderer::play(uint32_t bytes, void* context)
{
    assert(numOutputChannels_ != 0); // init() should have been called by child class.
    assert(bytes <= outputBufferSize_);

    uint32_t speed_snapshot = referenceClock_.getSpeed();
    uint32_t wp_snapshot;

    int64_t pts;
    uint32_t ptsLoc = 0;

    {
        ScopedMutex cs(mutex_);
        wp_snapshot = wp_;

        if(flushState_ != ESPlayerNative::FLUSH_OFF)
        {
            if(flushState_ ==  ESPlayerNative::FLUSH_BEGIN)
            {
                NTRACE(TRACE_MEDIAPLAYBACK, "AudioRenderer::play is flushing");
                dequeueTimestampsInRange(rp_, wp_snapshot, pts, ptsLoc);
                rp_ = wp_snapshot;
                timestampOfLastConsumedSample_ = INVALID_TIMESTAMP;
                timestampOfLastDeliveredSample_ = INVALID_TIMESTAMP;
                inputExhausted_ = true;
                flushState_ =  ESPlayerNative::FLUSH_END;
            }

            if(bytes > 0)
                writeSamples(NULL, 0, NULL, 0, bytes, context); // fill the entire output buffer with silence

            return;
        }
    }

    if(bytes == 0)
        return;

    // based on the current speed and the output buffer space, calculate the amount
    // of audio data that should be consumed.

    uint32_t bytesToConsume;

    if(speed_snapshot == IPlaybackDevice::NORMAL_SPEED)
    {
        bytesToConsume = bytes;
    }
    else
    {
        if(speed_snapshot == IPlaybackDevice::PAUSED_SPEED || mESPlayer.isDisabledOrPending() )
        {
            int64_t videoPTS = referenceClock_.getVideoPresentationTime();
            uint32_t ticksToConsume = 0;

            if(videoPTS != INVALID_TIMESTAMP)
            {
                if(timestampOfLastConsumedSample_ == INVALID_TIMESTAMP)
                {
                    // if audio time position is not known yet, consume 1/30 second worth of data
                    ticksToConsume = 33;
                }
                else if(timestampOfLastConsumedSample_ < videoPTS)
                {
                    // the time amount to consume for audio consumption to catch up with video
                    ticksToConsume = videoPTS - timestampOfLastConsumedSample_;
                }
            }

            // convert the "amount of time to consume" into the "number of audio bytes to consume"
            bytesToConsume = ((ticksToConsume * bytes_256th_PerTick_) >> 8);
        }
        else
            bytesToConsume = ((bytes * speed_snapshot) >> IPlaybackDevice::NORMAL_SPEED_Po2);

        if(bytesToConsume > 0)
        {
            // round-down the amount of audio data (to consume) to whole sample-blocks
            if(bytesPerSampleBlock_Po2_ >= 0)
                bytesToConsume = ((bytesToConsume >> bytesPerSampleBlock_Po2_) << bytesPerSampleBlock_Po2_);
            else
                bytesToConsume = ((bytesToConsume / bytesPerSampleBlock_) * bytesPerSampleBlock_);
        }
    }

    // check availability of audio data in the internal sample buffer, calculate the range
    // of audio data to be consumed from the internal sample buffer.

    // calculate the available bytes in the internal sample buffer.
    uint32_t available;
    if(wp_snapshot >= rp_)
        available = wp_snapshot - rp_;
    else
        available = size_ + wp_snapshot - rp_;

    if(bytesToConsume > available)
    {
        bytesToConsume = available;
        inputExhausted_ = true;
        mESPlayer.underflowReporter();
    } else {
        inputExhausted_ = false;
    }
    uint32_t rp_next = rp_ + bytesToConsume;

    if(rp_next >= size_)
        rp_next -= size_;

    // dequeue all timestamp-info items within the range of audio data to be consumed,
    // calculate the "last consumed audio sample time position".

    int numSamplesDequeued = dequeueTimestampsInRange(rp_, rp_next, pts, ptsLoc);

    if(pts != INVALID_TIMESTAMP)
    {
        // if a referenced timestamp is found, extrapolate from the last referenced point to find out
        // the time position of the last consumed audio sample.

        if(ptsLoc < rp_next)
            ptsLoc = rp_next - ptsLoc; // now ptsLoc means the distance between reference point and rp_next.
        else
            ptsLoc = size_ + rp_next - ptsLoc; // (same meaning as above)

        timestampOfLastConsumedSample_ = pts + ((ptsLoc * ticks_65536th_PerByte_) >> 16);
    }
    else if(timestampOfLastConsumedSample_ != INVALID_TIMESTAMP)
    {
        // if there is no referenced timestamp for the segment(s) currently being consumed, use the
        // previous "last consumed audio sample time position" to extrapolate for the new one.

        timestampOfLastConsumedSample_ += ((bytesToConsume * ticks_65536th_PerByte_) >> 16);
    }

    // update the reference clock.
    //

    if(speed_snapshot == IPlaybackDevice::PAUSED_SPEED)
    {
        referenceClock_.invalidateCurrPresentationTime();
        referenceClock_.invalidateAudioPresentationTime();
    }
    else if(bytesToConsume > 0 && timestampOfLastConsumedSample_ != INVALID_TIMESTAMP)
    {
        // output buffer latency calculation:
        // first, the number of old remaining bytes in output buffer is normalized with speed
        uint32_t latency = (((outputBufferSize_ - bytes) * speed_snapshot) >> IPlaybackDevice::NORMAL_SPEED_Po2);

        // add the number of newly delivered bytes normalized with speed
        latency += bytesToConsume;

        // convert the normalized number of bytes into PTS ticks
        latency = ((latency * ticks_65536th_PerByte_) >> 16);

        // the current PTS to be updated is compensated with the output buffer latency.
        int64_t currPTS = timestampOfLastConsumedSample_ - latency;

        if(currPTS != INVALID_TIMESTAMP) {
            referenceClock_.updateCurrPresentationTime(currPTS);
            referenceClock_.updateAudioPresentationTime(currPTS);
            mESPlayer.updatePlaybackPosition(currPTS);
        }
    }

    // write to the output buffer.
    //

    if(bytesToConsume == 0 || speed_snapshot == IPlaybackDevice::PAUSED_SPEED || mESPlayer.isDisabledOrPending())
    {
        writeSamples(NULL, 0, NULL, 0, bytes, context); // fill the entire output buffer with silence
    }
    else if(speed_snapshot == IPlaybackDevice::NORMAL_SPEED)
    {
        // Rendering audio samples for playback.
        mESPlayer.getCallback()->updateRenderingStats(numSamplesDequeued, 0, 0);

        ScopedMutex cs(mutex_);
        if(rp_next < rp_) { // wrap-around
            Ease((int16_t*)&data_[rp_], (size_ - rp_) / sizeof(int16_t));
            Ease((int16_t*)&data_[0], rp_next / sizeof(int16_t));

            writeSamples(&data_[rp_], size_ - rp_,
                         &data_[0], rp_next, bytes - bytesToConsume, context);
        } else {
            Ease((int16_t*)&data_[rp_], bytesToConsume / sizeof(int16_t));

            writeSamples(&data_[rp_], bytesToConsume, NULL, 0,
                         bytes - bytesToConsume, context);
        }
    }
    else
    {
        // the simplest way to implement abnormal speed audio playback - fill silence
        writeSamples(NULL, 0, NULL, 0, bytes, context);
    }

    rp_ = rp_next;
}

int AudioRenderer::dequeueTimestampsInRange(
        uint32_t rpBegin, uint32_t rpEnd,
        int64_t& latestTimestamp,
        uint32_t& latestTimestampLocation)
{
    // dequeue all timestamp-info items which are corresponding to locations between rpBegin and rpEnd,
    // the latest dequeued information (timestamp and the corresponding location) will be returned.
    int numDequeued = 0;
    ScopedMutex cs(mutex_);
    latestTimestamp = INVALID_TIMESTAMP;

    while(rpTsQueue_ != wpTsQueue_)
    {
        uint32_t loc = timestampQueue_[rpTsQueue_].wp_;

        if(rpEnd < rpBegin) // wrap-around
        {
            if(loc >= rpEnd && loc < rpBegin) // not in range
                return numDequeued;
        }
        else
        {
            if(loc >= rpEnd || loc < rpBegin) // not in range
                return numDequeued;
        }

        latestTimestamp = timestampQueue_[rpTsQueue_].timestamp_;
        latestTimestampLocation = loc;

        uint32_t next = rpTsQueue_ + 1;
        if(next >= timestampQueueSize_)
            next = 0;
        rpTsQueue_ = next;
        numDequeued++;
    }
    return numDequeued;
}

void AudioRenderer::getTimestampOfNextSampleToRender(int64_t& timestamp)
{
    ScopedMutex cs(mutex_);

    if(timestampOfLastConsumedSample_ != INVALID_TIMESTAMP)
    {
        timestamp = timestampOfLastConsumedSample_;
    } else {
        uint32_t rpTsQueue_snapshot = rpTsQueue_;
        timestamp = timestampQueue_[rpTsQueue_snapshot].timestamp_;
    }
}

void AudioRenderer::getTimestampOfLastDeliveredSample(int64_t& timestamp)
{
    ScopedMutex cs(mutex_);
    timestamp =  timestampOfLastDeliveredSample_;
}

void AudioRenderer::setFlushState(ESPlayerNative::FlushState flushState)
{
    ScopedMutex cs(mutex_);
    flushState_ = flushState;
}

ESPlayerNative::FlushState AudioRenderer::getFlushState()
{
    ScopedMutex cs(mutex_);
    return flushState_;
}

void AudioRenderer::setFade(double target, uint32_t duration, IAudioMixer::Ease ease)
{
    ScopedMutex cs(mutex_);
    if (duration) {
        mStartVolume = mCurrentVolume;
        mTargetVolume = target;
        mEase = ease;
        mEaseTimeElapsed = 0;
        mEaseTransitionFrames = duration * 48;
    } else {
        mStartVolume = target;
        mTargetVolume = target;
        mCurrentVolume = target;
        mEase = ease;
        mEaseTimeElapsed = 0;
        mEaseTransitionFrames = 0;
    }
}

double AudioRenderer::getCurrentVolume()
{
    ScopedMutex cs(mutex_);
    return mCurrentVolume;
}
