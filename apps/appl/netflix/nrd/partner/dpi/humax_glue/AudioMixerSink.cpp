/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "AudioMixerSink.h"
#include <nrd/AppLog.h>

using namespace netflix;
using namespace netflix::device;

struct SinkClip {
    SinkClip()
      : listener(NULL)
    {
    }

    std::vector<unsigned char> data;
    int channels;
    int sample_rate;
    std::string URL;

    IAudioMixer::AudioListener *listener;
    bool playing;
    unsigned play_pos;
    double volume;
};

AudioMixerSink::AudioMixerSink()
{

}

AudioMixerSink::~AudioMixerSink()
{

}

std::vector<AudioCodecType> AudioMixerSink::supportedCodecs()
{
    std::vector<AudioCodecType> types;
    types.push_back(HEAAC_AUDIO);
    types.push_back(OGG_VORBIS_AUDIO);
    types.push_back(WAV_AUDIO);
    types.push_back(MP3_AUDIO);
    types.push_back(WMA_AUDIO);

    return types;
}

NFErr AudioMixerSink::prepareClip(const std::string &URL, const DataBuffer &data,
                                  AudioClip &aClip, unsigned int &clipSize)
{

    printf("%s\n", __func__ );
    NRDP_UNUSED(URL);
    NRDP_UNUSED(data);
    NRDP_UNUSED(aClip);
    NRDP_UNUSED(clipSize);

    return NFErr_OK;
}

void AudioMixerSink::freeClip(AudioClip aClip, const std::string &URL)
{
    printf("%s\n", __func__ );
    NRDP_UNUSED(URL);
    NRDP_UNUSED(aClip);
}


//NFErr AudioMixerSink::playClip(AudioClip aClip, double volume,
 //                              AudioListener *listener, const std::string &URL,
//                               PlayId &playId)
//{
//    printf("%s\n", __func__ );
//    NRDP_UNUSED(aClip);
//    NRDP_UNUSED(volume);
//    NRDP_UNUSED(listener);
//    NRDP_UNUSED(URL);
//    NRDP_UNUSED(playId);

//    return NFErr_OK;
//}

//NFErr AudioMixerSink::stopClip(AudioClip aClip, const std::string &URL, PlayId playId)
//{
//    printf("%s\n", __func__ );
//    NRDP_UNUSED(aClip);
//    NRDP_UNUSED(URL);
//    NRDP_UNUSED(playId);

//    return NFErr_OK;
//}

