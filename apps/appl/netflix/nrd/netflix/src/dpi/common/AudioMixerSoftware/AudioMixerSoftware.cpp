/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "AudioMixerThread.h"
#include "AudioMixerSoftware.h"
#include "AudioMixerDecoderVorbis.h"
#include "AudioMixerDecoderWAV.h"
#include <nrd/AppLog.h>
#include <nrd/NrdApplication.h>
#include <climits>

using namespace netflix;
using namespace netflix::device::esplayer;
using namespace netflix::device;

#ifdef BUILD_DEBUG
#define DEBUG_ONLY(x) x;
#define DEFAULT_UPPER_BOUND 3
#else
#define DEBUG_ONLY(x) ;
#endif

class PlayTimer;

struct AudioMixerClip {
    AudioMixerClip()
        : listener(NULL)
    {
    }

    int32_t             peakVolume;
    int32_t             currentVolume;
    int32_t             fadeOutStartVolume;

    // Note: samples = total samples
    // So, a stereo clip will have 2x the samples
    // of a mono clip of the same time duration.

    uint32_t            channels;
    uint32_t            samplerate;
    uint64_t            samples;

    uint32_t            timeElapsed;
    uint32_t            fadeInFrames;
    uint32_t            fadeOutFrames;
    IAudioMixer::Ease   fadeInEase;
    IAudioMixer::Ease   fadeOutEase;

    uint32_t            fadeOutIndexStart;

    std::string         URL;

    uint32_t            stopIndex;
    uint32_t            lastFramesCopied;

    IAudioMixer::AudioListener *    listener;
    shared_ptr<AudioMixerDecoder>   decoder;
    shared_ptr<PlayTimer>           timer;

    bool                freePending;
};

class PlayTimer : public Application::Timer
{
public:
    PlayTimer(int interval, AudioMixerClip *clip)
      : Timer(interval)
      , mClip(clip)
    {
        setSingleShot(true);
    }

    virtual std::string describe() const { return "AudioMixer::PlayTimer"; }

    virtual void timerFired()
    {
        if (mClip->listener) {
            NTRACE(TRACE_DPI, "%s: calling playDone listener for %s", __FUNCTION__, mClip->URL.c_str());
            mClip->listener->playDone(mClip->URL);
        }
    }

private:
    AudioMixerClip *mClip;
};

AudioMixerSoftware::AudioMixerSoftware(const shared_ptr<IAudioMixerOutput> &AMO) :
    mRunning(false), mSupportDeferredPlay(true), mAudioMixerOutput(AMO),
    mDevicePreferredFrames(0), mDeviceMaximumFrames(0), mDeviceMaximumFramesEstimate(0),
    mClipMutex(UNTRACKED_MUTEX, "AudioMixerMutex"),
    mEase(EaseLinear),
    mEaseTimeElapsed(0),
    mEaseTransitionFrames(0),
    mCurrentMasterVolume(1 << 16),
    mStartMasterVolume(1 << 16),
    mTargetMasterVolume(1 << 16),
    mListener(NULL)
{
    mAudioMixerOutput->getDeviceInfo(mDevicePreferredFrames, mDeviceMaximumFrames, mSupportDeferredPlay);

    if (mDevicePreferredFrames && mDeviceMaximumFrames) {
        mMixingBuffer.resize(framesToSamples(mDevicePreferredFrames));
        mMixingSourceBuffer.resize(framesToSamples(mDevicePreferredFrames));
        mLimitedBuffer.resize(framesToSamples(mDevicePreferredFrames));

        mRunning = true;
        mAudioMixerThread.reset(new AudioMixerThread(*this, &AudioMixerSoftware::mixingTask));
    } else {
        Log::error(TRACE_DPI, "Audio mixer thread not started because of getDeviceInfo() failure.");
    }
}

AudioMixerSoftware::~AudioMixerSoftware()
{
    if (mRunning) {
        {
            ScopedMutex lock(mClipMutex);
            mRunning = false;
            mCondition.signal();
        }
        assert(mAudioMixerThread);
        mAudioMixerThread->Wait();
    }
}

IAudioMixer::Capability AudioMixerSoftware::getCapability() const
{
    IAudioMixer::Capability capability;
    capability.uiSoundBufferSize = framesToDoubleMs(mDeviceMaximumFrames);
    capability.uiSoundBufferTransferSize = framesToDoubleMs(mDevicePreferredFrames);
    capability.uiSoundBufferSizeEstimate = framesToDoubleMs(mDeviceMaximumFramesEstimate);
    return capability;
}

std::vector<AudioCodecType> AudioMixerSoftware::supportedCodecs()
{
    std::vector<AudioCodecType> types;
    types.push_back(OGG_VORBIS_AUDIO);
    types.push_back(WAV_AUDIO);
    return types;
}

NFErr AudioMixerSoftware::prepareClip(const std::string &URL, const DataBuffer &data,
                                      AudioClip &aClip, unsigned int &clipSize)
{
    AudioMixerClip * clip = new AudioMixerClip;

    if (AudioMixerDecoderVorbis::probe(data.data(), data.size())) {
        clip->decoder.reset(new AudioMixerDecoderVorbis(data.data(), data.size(), 64*1024));
    } else if (AudioMixerDecoderWav::probe(data.data(), data.size())) {
        clip->decoder.reset(new AudioMixerDecoderWav(data.data(), data.size()));
    } else {
        Log::error(TRACE_DPI, "%s is not a supported file.", URL.c_str());
        free(clip);
        return NFErr_Bad;
    }

    clip->URL = URL;
    clip->decoder->info(clip->channels, clip->samplerate, clip->samples);
    clip->timer.reset(new PlayTimer(0, clip));

    aClip = static_cast<AudioClip>(clip);
    clipSize = data.size();

    return NFErr_OK;
}

void AudioMixerSoftware::freeClip(AudioClip aClip, const std::string &URL)
{
    NRDP_UNUSED(URL);

    ScopedMutex lock(mClipMutex);

    AudioMixerClip* clip = static_cast<AudioMixerClip*>(aClip);

     if (clip) {
        clip->timer.reset();

        if (mClipSet.find(clip) == mClipSet.end()) {
            delete clip;
        } else {
            // This path only happens when the destructor is called in the middle
            // of playback.
            Log::warn(TRACE_DPI, "freeClip on %s called while playing", URL.c_str());
            clip->freePending = true;
        }
    }
}

double AudioMixerSoftware::getVolume()
{
    return ((double)mCurrentMasterVolume)/(1 << 16);
}

NFErr AudioMixerSoftware::setVolume(double volume, int transitionMs, IAudioMixer::Ease ease)
{
    ScopedMutex lock(mClipMutex);
    if (transitionMs) {
        mStartMasterVolume = mCurrentMasterVolume;
        mTargetMasterVolume = (1 << 16)*volume;
        mEase = ease;
        mEaseTimeElapsed = 0;
        mEaseTransitionFrames = msToFrames(transitionMs);
        mCondition.signal();
    } else {
        mCurrentMasterVolume = (1 << 16)*volume;
        mStartMasterVolume = (1 << 16)*volume;
        mTargetMasterVolume = (1 << 16)*volume;
        mEase = ease;
        mEaseTimeElapsed = msToFrames(transitionMs);
        mEaseTransitionFrames = msToFrames(transitionMs);
    }
    return NFErr_OK;
}

NFErr AudioMixerSoftware::playClip(AudioClip aClip, double volume,
                                   AudioListener *listener, const std::string &URL,
                                   PlayId &playId,
                                   int fadeInMs,
                                   int fadeOutMs,
                                   IAudioMixer::Ease fadeInEase,
                                   IAudioMixer::Ease fadeOutEase)
{
     if (!mRunning) {
        Log::error(TRACE_DPI, "Audio mixer thread failed to start.");
        return NFErr_Bad;
     }

    AudioMixerClip* clip = static_cast<AudioMixerClip*>(aClip);
    if (!clip) {
        return NFErr_Bad;
    }

    playId = (PlayId)1;

    uint32_t clipFrames         = clip->samples / clip->channels;
    uint32_t fadeInFrames       = msToFrames(fadeInMs);
    uint32_t fadeOutFrames      = msToFrames(fadeOutMs);

    clip->peakVolume            = (1 << 16)*volume;
    clip->currentVolume         = fadeInMs?0:clip->peakVolume;
    clip->fadeOutStartVolume    = clip->peakVolume;

    clip->listener              = listener;
    clip->URL                   = URL;

    clip->timeElapsed           = 0;
    clip->fadeInEase            = fadeInEase;
    clip->fadeOutEase           = fadeOutEase;
    clip->fadeInFrames          = fadeInFrames;
    clip->fadeOutFrames         = fadeOutFrames;
    clip->stopIndex             = clipFrames;

    clip->freePending = false;

    if (fadeInFrames > clipFrames) {
        Log::warn(TRACE_DPI, "Fade in exceeds clip length.  Adjusted length to clip length.");
        clip->fadeInFrames = clipFrames;
    }

    if (fadeOutFrames > clipFrames) {
        Log::warn(TRACE_DPI, "Fade out exceeds clip length.  Adjusted length to clip length.");
        clip->fadeOutFrames = clipFrames;
    }

    if ((clip->fadeInFrames + clip->fadeOutFrames) > clipFrames) {
        Log::warn(TRACE_DPI, "Fade in and fade out overlap.  Fade out placed after fade in.");
        clip->fadeOutFrames = clipFrames - clip->fadeInFrames;
        clip->fadeOutIndexStart = clip->fadeInFrames;
     } else {
        clip->fadeOutIndexStart = clipFrames - clip->fadeOutFrames;
     }

    if (!mListener) {
        mListener = listener;
    }

    ScopedMutex lock(mClipMutex);
    mClipSet.insert(clip);
    mCondition.signal();

    return NFErr_OK;
}

NFErr AudioMixerSoftware::stopClip(AudioClip aClip, const std::string &URL, PlayId playId, int fadeOutMs, IAudioMixer::Ease fadeOutEase)
{
    NRDP_UNUSED(URL);
    NRDP_UNUSED(playId);

    AudioMixerClip* clip = static_cast<AudioMixerClip*>(aClip);
    if (!clip) {
        return NFErr_Bad;
    }

    ScopedMutex lock(mClipMutex);
    for (std::set<AudioMixerClip*>::iterator it=mClipSet.begin(); it!=mClipSet.end(); it++) {
        if ((*it)->URL == clip->URL) {
            (*it)->fadeOutStartVolume = (*it)->currentVolume;
            (*it)->fadeOutIndexStart  = (*it)->timeElapsed;
            (*it)->fadeOutEase        = fadeOutEase;

            uint32_t clipFrames         = (*it)->samples / (*it)->channels;
            if (((*it)->timeElapsed + msToFrames(fadeOutMs)) > clipFrames) {
                Log::warn(TRACE_DPI, "Fade out exceeds clip length.  Adjusted length to clip length.");
                (*it)->stopIndex      = clipFrames;
                (*it)->fadeOutFrames  = clipFrames - (*it)->timeElapsed;
            } else {
                (*it)->stopIndex      = (*it)->timeElapsed + msToFrames(fadeOutMs);
                (*it)->fadeOutFrames  = msToFrames(fadeOutMs);
            }

            break;
        }
    }
    mCondition.signal();

    return NFErr_OK;
}

void AudioMixerSoftware::Limit(short * dstBuffer, const int * srcBuffer, const size_t samples)
{
    for (size_t i = 0; i < samples; i++) {
        if (srcBuffer[i] > SHRT_MAX) {
            dstBuffer[i] = SHRT_MAX;
        } else if (srcBuffer[i] < SHRT_MIN) {
            dstBuffer[i] = SHRT_MIN;
        } else {
            dstBuffer[i] = srcBuffer[i];
        }
    }
}

void AudioMixerSoftware::Ease(short * dstBuffer, const short * srcBuffer, const size_t samples /* Already stereo*/)
{
    int delta = mTargetMasterVolume - mStartMasterVolume;

    for (size_t i = 0; i < samples; i += 2, mEaseTimeElapsed++) {

        if (mEaseTimeElapsed < mEaseTransitionFrames) {
            mCurrentMasterVolume = mEasing.easeNext(mEase, mEaseTimeElapsed, mStartMasterVolume, delta, mEaseTransitionFrames - 1);
        }

        dstBuffer[i]   = ((int)srcBuffer[i]   * mCurrentMasterVolume) >> 16;
        dstBuffer[i+1] = ((int)srcBuffer[i+1] * mCurrentMasterVolume) >> 16;
    }
}

void AudioMixerSoftware::Notify()
{
    bool     needToNotify = false;
    bool     gotTotalLatency = false;
    uint32_t totalLatency = 0;
    uint32_t baseLatency = 0;
    uint32_t maxFramesCopied = 0;

    for (std::set<AudioMixerClip*>::iterator it = mClipSet.begin(); it != mClipSet.end(); ++it) {
        if ((*it)->decoder->isEOS() || ((*it)->timeElapsed >= (*it)->stopIndex)) {
            maxFramesCopied = std::max(maxFramesCopied, (*it)->lastFramesCopied);
            needToNotify = true;
        }
    }

    if (needToNotify) {
        size_t delayFrames = 0;
        if (NFErr_OK != mAudioMixerOutput->getBufferDelay(delayFrames)) {
            Log::error(TRACE_DPI, "getBufferDelay failed");
        } else {
            totalLatency = delayFrames;
            gotTotalLatency = true;
        }

        if (gotTotalLatency) {
            if (totalLatency > maxFramesCopied) {
                baseLatency = totalLatency - maxFramesCopied;
            } else {
                baseLatency = totalLatency;
            }
        }

        std::set<AudioMixerClip*>::iterator it = mClipSet.begin();
        while (it != mClipSet.end()) {
            if ((*it)->decoder->isEOS() || ((*it)->timeElapsed >= (*it)->stopIndex)) {
                (*it)->decoder->reset();
                if (!(*it)->freePending) {
                    (*it)->timer->restart(framesToMs(baseLatency + (*it)->lastFramesCopied) + 1);
                } else {
                    Log::warn(TRACE_DPI, "pending freeClip on %s completed", (*it)->URL.c_str());
                    delete(*it);
                }
                mClipSet.erase(*it++);
            } else {
                ++it;
            }
        }
    }
}

size_t AudioMixerSoftware::Mix(std::set<AudioMixerClip*> & attachedSet, int * dstBuffer, const size_t samples) // mClipMutex always held
{
    int bytes = 0;
    int largest_chunk = 0;

    memset(dstBuffer, 0, samples*sizeof(int));

    std::set<AudioMixerClip*>::iterator it = attachedSet.begin();

    while (it != attachedSet.end()) {

        int bytesToRead = mMixingSourceBuffer.size()*sizeof(mMixingSourceBuffer[0]);
        if ((*it)->channels == 1) {
            bytesToRead >>= 1;
        }

        bytes = (*it)->decoder->read(&mMixingSourceBuffer[0], bytesToRead);

        int srcIncrement = 2;
        int srcOffset = 1;

        if ((*it)->channels == 1) {
            bytes <<= 1;
            srcIncrement = 1;
            srcOffset = 0;
        }

        if (bytes > largest_chunk) {
            largest_chunk = bytes;
        }

        for (uint32_t s = 0, d = 0; d < bytesToSamples(bytes); s+=srcIncrement, d+=2) {

            if ((*it)->fadeOutFrames && ((*it)->timeElapsed >= (*it)->fadeOutIndexStart)) {
                uint32_t timeElapsedInFadeOut = (*it)->timeElapsed - (*it)->fadeOutIndexStart;
                (*it)->currentVolume = mEasing.easeNext((*it)->fadeOutEase, timeElapsedInFadeOut, (*it)->fadeOutStartVolume, -(*it)->fadeOutStartVolume, (*it)->fadeOutFrames - 1);
            } else if ((*it)->fadeInFrames && ((*it)->timeElapsed < (*it)->fadeInFrames)) {
                (*it)->currentVolume = mEasing.easeNext((*it)->fadeInEase, (*it)->timeElapsed, 0, (*it)->peakVolume, (*it)->fadeInFrames - 1);
            }

            dstBuffer[d]   += (mMixingSourceBuffer[s]          * (*it)->currentVolume) >> 16;
            dstBuffer[d+1] += (mMixingSourceBuffer[s+srcOffset]* (*it)->currentVolume) >> 16;

            (*it)->timeElapsed++;
        }

        (*it)->lastFramesCopied = bytesToFrames(bytes);

        ++it;
    }

    return bytesToSamples(largest_chunk);
}

void AudioMixerSoftware::mixingTask()
{
    void *  outputBuffer = NULL;
    size_t  availableFrames = 0;
    size_t  contiguousFrames;

    bool    playing = false;
    bool    deferPlay = false;

    bool    estimatingFifoSize = true;
    size_t  estimatedFifoSizeFrames = 0;

    while (true) {
        {
            mClipMutex.lock();
            if (mClipSet.empty() && !(mEaseTimeElapsed < mEaseTransitionFrames)) {
                if (playing) {
                    DEBUG_ONLY(uint64_t r = Time::monoMS();)
                    if (NFErr_OK == mAudioMixerOutput->Reset()) {
                        playing = false;
                    } else {
                        Log::error(TRACE_DPI, "Audio mixer failed to return to reset state.");
                    }
                    DEBUG_ONLY(verifyTime(r, 0, framesToMs(mDeviceMaximumFrames)+DEFAULT_UPPER_BOUND, "Reset()");)
                }
            } else {
                if (!playing && mSupportDeferredPlay) {
                    deferPlay = true;
                }
            }
            while (mRunning && mClipSet.empty() && !(mEaseTimeElapsed < mEaseTransitionFrames)) {
                mCondition.wait(&mClipMutex);
            }
            if (!mRunning) {
                mClipMutex.unlock();
                break;
            }

            if (!playing && !mSupportDeferredPlay) {
                DEBUG_ONLY(uint64_t p = Time::monoMS();)
                if (NFErr_OK == mAudioMixerOutput->Play()) {
                    playing = true;
                } else {
                    Log::error(TRACE_DPI, "Audio mixer failed to enter play state. (non-deferred)");
                }
                DEBUG_ONLY(verifyTime(p, 0, DEFAULT_UPPER_BOUND, "Play()");)
            }

            DEBUG_ONLY(uint64_t t = Time::monoMS();)

            if (NFErr_OK != mAudioMixerOutput->getBufferAvailable(availableFrames)) {
                mClipMutex.unlock();
                // Device had a device specific error and could not return the available
                // frames.  We need to check on this ASAP to avoid potential underflow
                Thread::Sleep(1);
                continue;
            }

            if (availableFrames < mDevicePreferredFrames) {

                if (estimatingFifoSize) {
                    mDeviceMaximumFramesEstimate = estimatedFifoSizeFrames;
                    estimatingFifoSize = false;
                    if (mListener) {
                        mListener->capabilityChanged();
                    }
                }

                mClipMutex.unlock();
                /* Partner's PCM buffer is pretty full. */
                Thread::Sleep(framesToMs(mDevicePreferredFrames)/4);
                continue;
            }

            DEBUG_ONLY(verifyTime(t, 0, DEFAULT_UPPER_BOUND, "getBufferStatus()");)

            size_t mixedSamples = Mix(mClipSet, &mMixingBuffer[0], framesToSamples(mDevicePreferredFrames));

            if (mClipSet.empty() && (mEaseTimeElapsed < mEaseTransitionFrames)) {
                mixedSamples = framesToSamples(mDevicePreferredFrames);
            }

            Limit(&mLimitedBuffer[0], &mMixingBuffer[0], mixedSamples);
            Ease(&mLimitedBuffer[0], &mLimitedBuffer[0], mixedSamples);

            size_t mixedFrames = samplesToFrames(mixedSamples);

            DEBUG_ONLY(uint64_t t2 = Time::monoMS();)

            if (NFErr_OK != mAudioMixerOutput->getBuffer(&outputBuffer, mixedFrames, contiguousFrames)) {
                mAudioMixerOutput->commitBuffer(0);
                mClipMutex.unlock();
                Thread::Sleep(1);
                continue;
            }

            if (contiguousFrames >= mixedFrames) {
                memcpy(outputBuffer, &mLimitedBuffer[0], framesToBytes(mixedFrames));
                mAudioMixerOutput->commitBuffer(mixedFrames);
            } else {
                memcpy(outputBuffer, &mLimitedBuffer[0], framesToBytes(contiguousFrames));
                mAudioMixerOutput->commitBuffer(contiguousFrames);

                size_t secondChunkFrameSize = mixedFrames - contiguousFrames;
                mAudioMixerOutput->getBuffer(&outputBuffer, secondChunkFrameSize, contiguousFrames);
                memcpy(outputBuffer, &mLimitedBuffer[framesToSamples(contiguousFrames)], framesToBytes(secondChunkFrameSize));
                mAudioMixerOutput->commitBuffer(secondChunkFrameSize);
            }

            DEBUG_ONLY(verifyTime(t2, 0, DEFAULT_UPPER_BOUND, "getBuffer() and/or commitBuffer()");)

            if (estimatingFifoSize) {
                estimatedFifoSizeFrames += mixedFrames;
            }

            if (deferPlay) {
                DEBUG_ONLY(uint64_t p2 = Time::monoMS();)
                if (NFErr_OK == mAudioMixerOutput->Play()) {
                    playing = true;
                    deferPlay = false;
                } else {
                    Log::error(TRACE_DPI, "Audio mixer failed to enter play state.  (deferred)");
                }
                DEBUG_ONLY(verifyTime(p2, 0, DEFAULT_UPPER_BOUND, "Play()");)
            }

            Notify();

            mClipMutex.unlock();
        }
    }
}

#ifdef BUILD_DEBUG
void AudioMixerSoftware::verifyTime(uint64_t startTime, uint64_t lowerBound, uint64_t upperBound, const char * msg)
{
    uint64_t elapsedTime = Time::monoMS() - startTime;
    if (elapsedTime < lowerBound) {
        Log::warn(TRACE_DPI, "%s took less time than expected.  Took %llu ms but expected > %llu ms.", msg, elapsedTime, lowerBound);
    }

    if (elapsedTime > upperBound) {
        Log::warn(TRACE_DPI, "%s took more time than expected.  Took %llu ms but expected < %llu ms.", msg, elapsedTime, upperBound);
    }
}
#endif

void Easing::buildTables(void)
{
    int ease       = static_cast<int>(IAudioMixer::EaseInCubic);
    int last_ease  = static_cast<int>(IAudioMixer::EaseOutCubic);

    for (; ease <= last_ease; ++ease) {
        mEasingTable[ease].resize(TABLE_LENGTH);

        for (int i = 0; i < TABLE_LENGTH; ++i) {
            IAudioMixer::Ease e = static_cast<IAudioMixer::Ease>(ease);
            mEasingTable[ease][i] = floatEaseNext(e, i, 0, 1.0, TABLE_LENGTH - 1) * (1 << 16);
        }
    }
}

int Easing::easeNext(IAudioMixer::Ease ease, int t, int b, int c, int d)
{
    switch (ease) {
    default:
    case IAudioMixer::EaseLinear:
        return ((int64_t)c*t)/d + b;
    case IAudioMixer::EaseInCubic:
    case IAudioMixer::EaseOutCubic:
        int e = static_cast<int>(ease);
        uint32_t inter = ((uint64_t)t << 16)/(d-1);
        uint32_t index = inter*(TABLE_LENGTH-1);
        uint16_t x1 = index >> 16;  // 16
        uint16_t x2 = index & 0xffff;
        uint32_t y1 = mEasingTable[e][x1];
        uint32_t y3;
        if ((x1+1) >= TABLE_LENGTH) {
            y3 = y1;
        } else {
            y3 = mEasingTable[e][x1+1];
        }

        uint32_t y2 = (x2*(y3-y1) >> 16) + y1;

        int ret = (((uint64_t)c*y2) >> 16) + b;
        return ret;
    }
}

/*
    Robert Penner's Easing Equations v1.5  May 1, 2003
    (ported to C++ by tmyles)
    (c) 2003 Robert Penner, all rights reserved.
    This work is subject to the terms in http://www.robertpenner.com/easing_terms_of_use.html

    (Following copy/pasted from http://www.robertpenner.com/easing_terms_of_use.html on 7/12/2010)
    TERMS OF USE - EASING EQUATIONS

    Open source under the BSD License.

    Copyright 2001 Robert Penner All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are
    met:

    Redistributions of source code must retain the above copyright notice,
    this list of conditions and the following disclaimer. Redistributions in
    binary form must reproduce the above copyright notice, this list of
    conditions and the following disclaimer in the documentation and/or
    other materials provided with the distribution. Neither the name of the
    author nor the names of contributors may be used to endorse or promote
    products derived from this software without specific prior written
    permission. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
    CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING,
    BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
    FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
    COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
    INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
    NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
    USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
    ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
    THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

float Easing::floatEaseNext(IAudioMixer::Ease ease, float t, float b, float c, float d)
{
    switch (ease) {
    default:
    case IAudioMixer::EaseInCubic:
        t/=d;
        return c*t*t*t + b;
    case IAudioMixer::EaseOutCubic:
        t=t/d-1;
        return c*(t*t*t + 1) + b;
    }
}

