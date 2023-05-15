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

#include <nrdbase/tr1.h>

#include "../AudioDecoder.h"

#if HAVE_OPENAL
# include <OpenAL/al.h>
# include <OpenAL/alc.h>
#else
# include <portaudio.h>
#endif

namespace netflix {
namespace device {

class AudioMixerNative : public IAudioMixer
{
public:
    AudioMixerNative();

    virtual ~AudioMixerNative();

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
                         Ease ease = EaseLinear);
#if HAVE_OPENAL
private:
    ALCdevice *mDevice;
    ALCcontext *mContext;
#endif
};

} // namespace device
} // namespace netflix

#endif // DEVICE_AUDIOMIXER_H
