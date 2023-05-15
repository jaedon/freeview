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

namespace netflix {
namespace device {

class AudioMixerSink : public IAudioMixer
{
public:
    AudioMixerSink();

    virtual ~AudioMixerSink();

    virtual std::vector<AudioCodecType> supportedCodecs();

    virtual NFErr prepareClip(const std::string &URL, const DataBuffer &data,
                              AudioClip &clip, unsigned int &clipSize);
    virtual void freeClip(AudioClip clip, const std::string &URL);

   // virtual NFErr playClip(AudioClip clip, double volume,
   //                        AudioListener *listener, const std::string &URL,
   //                        PlayId &playId);
   // virtual NFErr stopClip(AudioClip clip, const std::string &URL, PlayId playId);
};

} // namespace device
} // namespace netflix

#endif // DEVICE_AUDIOMIXER_H
