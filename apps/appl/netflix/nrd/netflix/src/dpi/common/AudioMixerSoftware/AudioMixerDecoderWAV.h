/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef DEVICE_AUDIOMIXER_DECODER_WAV_H
#define DEVICE_AUDIOMIXER_DECODER_WAV_H

#include "AudioMixerDecoder.h"
#include <nrdbase/Endian.h>

namespace netflix {
namespace device {

class AudioMixerDecoderWav : public AudioMixerDecoder
{
public:

    AudioMixerDecoderWav(const void * sourceBuffer, const size_t sourceBufferSize) :
         mBuffer(NULL), mSize(0), mPosition(0)
    {
        mBuffer = new uint8_t [sourceBufferSize];
        mSize = sourceBufferSize;
        memcpy(mBuffer, sourceBuffer, sourceBufferSize);

        const char * srcBuffer  = (char*)sourceBuffer;
        mChannels   = Endian::little_to_native(*(uint16_t*)&srcBuffer[22]);
        mSampleRate = Endian::little_to_native(*(uint32_t*)&srcBuffer[24]);
        mDataSize   = Endian::little_to_native(*(uint32_t*)&srcBuffer[40]);
        mSamples    = mDataSize / 2;
        mTotalSamplesRead = 0;
    };

    virtual ~AudioMixerDecoderWav()
    {
        delete[] mBuffer;
    };

    virtual uint32_t read(void * destinationBuffer, uint32_t bytesToRead)
    {
        if ((mPosition + bytesToRead) > mDataSize) {
            bytesToRead = mDataSize - mPosition;
        }

        short * dstBuffer = (short *)destinationBuffer;
        short * srcBuffer = (short *)(mBuffer+44+mPosition);
        const int samplesToCopy = bytesToRead / 2;

        for (int i = 0; i < samplesToCopy; i++) {
            dstBuffer[i] = Endian::little_to_native(srcBuffer[i]);
        }
        mPosition += bytesToRead;
        mTotalSamplesRead += bytesToRead >> 1;

        return bytesToRead;
    };

    virtual void reset()
    {
        /* Only accepting basic wav files with a fmt and data subchunk */
        mPosition = 0;
        mTotalSamplesRead = 0;
    };

    virtual void info(uint32_t & channels, uint32_t & samplerate, uint64_t & samples)
    {
        channels = mChannels;
        samplerate = mSampleRate;
        samples = mSamples;
    }

    virtual bool isEOS()
    {
        return (mSamples <= mTotalSamplesRead);
    }

    static bool probe(const void * sourceBuffer, const size_t sourceBufferSize)
    {
        /* Only accept basic wav files with a fmt and data subchunk */
        if (sourceBufferSize < 44) {
            return false;
        }

        const char * srcBuffer       = (char*)sourceBuffer;
        const uint32_t ChunkID       = Endian::big_to_native(*(uint32_t*)&srcBuffer[0]);
        const uint32_t Format        = Endian::big_to_native(*(uint32_t*)&srcBuffer[8]);
        const uint32_t Subchunk1ID   = Endian::big_to_native(*(uint32_t*)&srcBuffer[12]);
        const uint16_t AudioFormat   = Endian::little_to_native(*(uint16_t*)&srcBuffer[20]);
        const uint16_t NumChannels   = Endian::little_to_native(*(uint16_t*)&srcBuffer[22]);
        const uint32_t SampleRate    = Endian::little_to_native(*(uint32_t*)&srcBuffer[24]);
        const uint16_t BitsPerSample = Endian::little_to_native(*(uint16_t*)&srcBuffer[34]);
        const uint32_t Subchunk2ID   = Endian::big_to_native(*(uint32_t*)&srcBuffer[36]);

        if ((ChunkID == 0x52494646) && (Format == 0x57415645) &&
            (Subchunk1ID == 0x666d7420) && (AudioFormat == 1) &&
            ((NumChannels == 1) || (NumChannels == 2)) &&
            (SampleRate == 48000) && (BitsPerSample == 16) &&
            (Subchunk2ID == 0x64617461)) {
            return true;
        } else {
            return false;
        }
    }

private:
    uint8_t *   mBuffer;
    size_t      mSize;
    size_t      mPosition;
    int         mChannels;
    int         mSampleRate;
    uint64_t    mSamples;
    uint64_t    mDataSize;
    uint64_t    mTotalSamplesRead;
};

}}

#endif
