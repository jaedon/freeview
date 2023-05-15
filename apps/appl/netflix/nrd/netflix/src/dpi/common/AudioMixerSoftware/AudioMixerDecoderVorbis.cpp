/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */


#include "AudioMixerDecoderVorbis.h"
#include <nrdbase/Log.h>
#include <cstring>
using namespace netflix;

namespace netflix {
namespace device {

size_t readOgg(void* dst, size_t size, size_t nmemb, void* datasource)
{
    OggFile * of = static_cast<OggFile*>(datasource);

    size_t len = size * nmemb;

    if ((of->curPtr + len) > (of->filePtr + of->fileSize)) {
        len = of->filePtr + of->fileSize - of->curPtr;
    }
    memcpy(dst, of->curPtr, len );
    of->curPtr += len;
    return len;
}

int seekOgg(void *datasource, ogg_int64_t offset, int whence )
{
    OggFile * of = static_cast<OggFile*>(datasource);

    switch (whence) {
        case SEEK_SET:
            of->curPtr = of->filePtr + offset;
            break;
        case SEEK_CUR:
            of->curPtr += offset;
            break;
        case SEEK_END:
            of->curPtr = of->filePtr + of->fileSize - offset;
            break;
        default:
            return -1;
    }
    if (of->curPtr < of->filePtr) {
        of->curPtr = of->filePtr;
        return -1;
    }
    if (of->curPtr > of->filePtr + of->fileSize) {
        of->curPtr = of->filePtr + of->fileSize;
        return -1;
    }

    return 0;
}

int closeOgg(void *)
{
    return 0;
}

long tellOgg(void * datasource)
{
    OggFile * of = static_cast<OggFile*>(datasource);
    return (of->curPtr - of->filePtr);
}

size_t AudioMixerDecoderVorbis::fillPcmCache(const size_t pcmCacheSize)
{
    mPcmCachePosition = 0;

    size_t  totalBytesRead = 0;
    size_t  requestChunkSize = pcmCacheSize;
    size_t  bytesRead = 0;
    char *  dstPtr = (char*)&mPcmCacheBuffer[0];

    for (;;) {
        bytesRead = ov_read(&mVf, dstPtr, requestChunkSize, &mCurrentSection);
        totalBytesRead   += bytesRead;
        dstPtr           += bytesRead;
        requestChunkSize -= bytesRead;
        if (!bytesRead) break;
        if (!requestChunkSize) break;
    }

    return totalBytesRead;
}


AudioMixerDecoderVorbis::AudioMixerDecoderVorbis(const void * sourceBuffer,
            const size_t sourceBufferSize, const size_t pcmCacheSize) :
            mPcmCacheMaximumAllowed(pcmCacheSize), mPcmCompletelyCached(false)
{
    mPcmCacheBuffer.resize(pcmCacheSize);
    mSourceBuffer.resize(sourceBufferSize);
    memcpy(&mSourceBuffer[0], sourceBuffer, sourceBufferSize);
    memset(&mVf, 0, sizeof(mVf));
    memset(&mOf, 0, sizeof(mOf));
    reset();
}

AudioMixerDecoderVorbis::~AudioMixerDecoderVorbis()
{
    if (!mPcmCompletelyCached) {
        ov_clear(&mVf);
    }
}

uint32_t AudioMixerDecoderVorbis::read(void * destinationBuffer, uint32_t bytesToRead)
{
    size_t bytesToCopy = 0;
    if (mPcmCachePosition < mPcmCacheBuffer.size()) {
        if ((bytesToRead + mPcmCachePosition) <= mPcmCacheBuffer.size()) {
            /* Everything fits in one shot */
            memcpy(destinationBuffer, &mPcmCacheBuffer[mPcmCachePosition], bytesToRead);
            mPcmCachePosition += bytesToRead;
            mTotalSamplesRead += bytesToRead >> mChannels;
            return bytesToRead;
        } else {
            /* Copy what we've got. */
            bytesToCopy = mPcmCacheBuffer.size()-mPcmCachePosition;
            memcpy(destinationBuffer, &mPcmCacheBuffer[mPcmCachePosition], bytesToCopy);
            mPcmCachePosition += bytesToCopy;
        }
    }

    if (mPcmCompletelyCached) {
        mTotalSamplesRead += bytesToCopy >> mChannels;
        return bytesToCopy;
    }

    size_t  totalBytesRead = 0;
    size_t  requestChunkSize = bytesToRead - bytesToCopy;
    size_t  bytesRead = 0;
    char *  dstPtr = (char*)destinationBuffer+bytesToCopy;

    for (;;) {
        bytesRead = ov_read(&mVf, dstPtr, requestChunkSize, &mCurrentSection);
        totalBytesRead   += bytesRead;
        dstPtr           += bytesRead;
        requestChunkSize -= bytesRead;
        if (!bytesRead) break;
        if (!requestChunkSize) break;
    }

    mTotalSamplesRead += (bytesToCopy+totalBytesRead) >> mChannels;

    return bytesToCopy+totalBytesRead;
}

void AudioMixerDecoderVorbis::reset()
{
    mTotalSamplesRead = 0;

    if (mPcmCompletelyCached) {
        mPcmCachePosition = 0;
    } else if (mPcmCachePosition && mVf.seekable) {
        mPcmCachePosition = 0;
        ov_pcm_seek(&mVf, mPcmCacheBuffer.size() >> mChannels);
    } else {
        /* FIXME: Should reset the Vorbis context and keep the current
        buffer instead of this. */
        if (mVf.datasource) {
            ov_clear(&mVf);
        }

        ov_callbacks callbacks;
        callbacks.read_func =  readOgg;
        callbacks.seek_func =  seekOgg;
        callbacks.close_func = closeOgg;
        callbacks.tell_func =  tellOgg;

        mOf.curPtr = &mSourceBuffer[0];
        mOf.filePtr = &mSourceBuffer[0];
        mOf.fileSize = mSourceBuffer.size();

        mCurrentSection = 0;

        ov_open_callbacks(&mOf, &mVf, NULL, 0, callbacks);

        vorbis_info * Vi = ov_info(&mVf, -1);
        mChannels   = Vi->channels;
        mSampleRate = Vi->rate;

        mSamples = ov_pcm_total(&mVf, -1) * mChannels;

        size_t cachedBytes = fillPcmCache(mPcmCacheMaximumAllowed);

        mPcmCacheBuffer.resize(cachedBytes);

        if (cachedBytes < mPcmCacheMaximumAllowed) {
            mPcmCompletelyCached = true;
            ov_clear(&mVf);
        }
    }
}

void AudioMixerDecoderVorbis::info(uint32_t & channels, uint32_t & samplerate, uint64_t & samples)
{
    channels   = mChannels;
    samplerate = mSampleRate;
    samples    = mSamples;
}

bool AudioMixerDecoderVorbis::isEOS()
{
    return (mSamples <= (mTotalSamplesRead * mChannels));
}

bool AudioMixerDecoderVorbis::probe(const void * sourceBuffer, const size_t sourceBufferSize)
{
    if (sourceBufferSize < 4) {
        return false;
    }

    ov_callbacks callbacks;
    callbacks.read_func =  readOgg;
    callbacks.seek_func =  seekOgg;
    callbacks.close_func = closeOgg;
    callbacks.tell_func =  tellOgg;

    OggFile Of;
    Of.curPtr = (char*)(sourceBuffer);
    Of.filePtr = (char*)(sourceBuffer);
    Of.fileSize = sourceBufferSize;

    int channels = 0;
    int samplerate = 0;
    OggVorbis_File Vf;
    int rc = ov_test_callbacks(&Of, &Vf, NULL, 0, callbacks);
    if (rc == 0) {
        vorbis_info * Vi = ov_info(&Vf, -1);
        channels   = Vi->channels;
        samplerate = Vi->rate;
    }
    ov_clear(&Vf);

    if ((rc == 0) && ((channels == 1) || (channels == 2)) && (samplerate == 48000)) {
        return true;
    } else {
        return false;
    }
}


}}
