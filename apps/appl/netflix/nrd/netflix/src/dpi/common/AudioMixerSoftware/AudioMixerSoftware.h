/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef DEVICE_AUDIOMIXER_H
#define DEVICE_AUDIOMIXER_H

#include <nrd/IAudioMixer.h>
#include <nrd/IAudioMixerOutput.h>
#include <nrdbase/tr1.h>
#include <nrdbase/Mutex.h>
#include <nrdbase/ConditionVariable.h>
#include <set>

struct AudioMixerClip;

namespace netflix {
namespace device {

class AudioMixerThread;


class Easing {
public:
    Easing() { buildTables(); }
    int easeNext(IAudioMixer::Ease ease, int t, int b, int c, int d);

private:
    void buildTables(void);
    float floatEaseNext(IAudioMixer::Ease ease, float t, float b, float c, float d);

    std::vector<int> mEasingTable[16];

    static const int TABLE_LENGTH = 128;
};



class AudioMixerSoftware : public IAudioMixer
{
public:
    AudioMixerSoftware(const shared_ptr<IAudioMixerOutput> &AMO);
    virtual ~AudioMixerSoftware();

    virtual Capability getCapability() const;

    virtual std::vector<AudioCodecType> supportedCodecs();

    virtual NFErr prepareClip(const std::string &URL, const DataBuffer &data,
                              AudioClip &clip, unsigned int &clipSize);

    virtual void freeClip(AudioClip clip, const std::string &URL);

    virtual NFErr playClip(AudioClip clip, double volume,
                           AudioListener *listener, const std::string &URL,
                           PlayId &playId,
                           int fadeInMs = 0,
                           int fadeOutMs = 0,
                           Ease fadeInEase = EaseLinear,
                           Ease fadeOutEase = EaseLinear);

    virtual NFErr stopClip(AudioClip clip, const std::string &URL, PlayId playId, int fadeOutMs = 0, Ease fadeOutEase = EaseLinear);

    virtual double getVolume();

    virtual NFErr setVolume(double volume,
                         int transitionMs = 0,
                         IAudioMixer::Ease ease = IAudioMixer::EaseLinear);

public:
    static uint32_t framesToMs(const uint32_t frames) { return (frames / 48); }
    static uint32_t framesToRoundedMs(const uint32_t frames) { return ((frames+24) / 48); }
    static double   framesToDoubleMs(const uint32_t frames) { return (static_cast<double>(frames) / 48); }
    static uint32_t framesToBytes(const uint32_t frames) { return (frames << 2); }
    static uint32_t framesToSamples(const uint32_t frames) { return (frames << 1); }
    static uint32_t samplesToFrames(const uint32_t samples) { return (samples >> 1); }
    static uint32_t bytesToSamples(const uint32_t bytes) { return (bytes >> 1); }
    static uint32_t bytesToFrames(const uint32_t bytes) { return (bytes >> 2); }
    static uint32_t msToFrames(const uint32_t ms) { return (ms * 48); }
    static uint32_t msToSamples(const uint32_t ms) { return (ms * 96); }

private:
    size_t  Mix(std::set<AudioMixerClip*> &mixSet, int * dstBuffer, const size_t samples);
    void    Ease(short * dstBuffer, const short * srcBuffer, const size_t samples);
    void    Limit(short * dstBuffer, const int * srcBuffer, const size_t samples);
    void    Notify();
    void    mixingTask();

    bool                                mRunning;
    bool                                mSupportDeferredPlay;

    shared_ptr<IAudioMixerOutput>       mAudioMixerOutput;
    shared_ptr<AudioMixerThread>        mAudioMixerThread;
    size_t                              mDevicePreferredFrames;
    size_t                              mDeviceMaximumFrames;
    size_t                              mDeviceMaximumFramesEstimate;

    std::set<AudioMixerClip*>           mClipSet;

    Mutex                               mClipMutex;
    ConditionVariable                   mCondition;

    std::vector<int>                    mMixingBuffer;
    std::vector<short>                  mLimitedBuffer;
    std::vector<short>                  mMixingSourceBuffer;

    IAudioMixer::Ease                   mEase;
    uint32_t                            mEaseTimeElapsed;
    uint32_t                            mEaseTransitionFrames;
    int32_t                             mCurrentMasterVolume;
    int32_t                             mStartMasterVolume;
    int32_t                             mTargetMasterVolume;

    Easing                              mEasing;

    IAudioMixer::AudioListener *        mListener;

#ifdef BUILD_DEBUG
    void  verifyTime(uint64_t startTime, uint64_t lowerBound, uint64_t upperBound, const char * msg);
#endif

};

} // namespace device
} // namespace netflix

#endif // DEVICE_AUDIOMIXER_H
