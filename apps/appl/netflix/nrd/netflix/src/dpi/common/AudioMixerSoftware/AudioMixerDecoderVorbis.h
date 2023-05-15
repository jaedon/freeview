/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef DEVICE_AUDIOMIXER_DECODER_VORBIS_H
#define DEVICE_AUDIOMIXER_DECODER_VORBIS_H

#include "AudioMixerDecoder.h"
#include <tremor/ivorbisfile.h>
#include <tremor/ivorbiscodec.h>
#include <vector>

namespace netflix {
namespace device {

struct OggFile
{
    char * curPtr;
    char * filePtr;
    size_t fileSize;
};

class AudioMixerDecoderVorbis : public AudioMixerDecoder
{
public:
    AudioMixerDecoderVorbis(const void * sourceBuffer, const size_t sourceBufferSize, const size_t pcmCacheSize);

    virtual ~AudioMixerDecoderVorbis();

    virtual uint32_t read(void * destinationBuffer, uint32_t bytesToRead);

    virtual void reset();

    virtual void info(uint32_t & channels, uint32_t & samplerate, uint64_t & samples);

    virtual bool isEOS();

    static bool probe(const void * sourceBuffer, const size_t sourceBufferSize);

private:
    size_t fillPcmCache(const size_t pcmCacheSize);

    std::vector<char>   mPcmCacheBuffer;
    size_t              mPcmCachePosition;
    size_t              mPcmCacheMaximumAllowed;
    bool                mPcmCompletelyCached;

    std::vector<char>   mSourceBuffer;

    OggVorbis_File      mVf;
    OggFile             mOf;
    int                 mCurrentSection;

    int                 mChannels;
    int                 mSampleRate;
    ogg_int64_t         mSamples;
    ogg_int64_t         mTotalSamplesRead;
};

}}

#endif
