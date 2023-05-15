/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "AudioMixerOutputAudioQueue.h"
#include <nrddpi/AudioMixerSoftware.h>
#include <nrd/AppLog.h>
#include <nrd/NrdApplication.h>

using namespace netflix;
using namespace netflix::device;

void AudioMixerOutputAudioQueue::Callback(void * u, AudioQueueRef queue, AudioQueueBufferRef bufRef)
{
    AudioMixerOutputAudioQueue * AMOAQ = (AudioMixerOutputAudioQueue *)u;
    int readIndex = AMOAQ->mReadIndex;

    if (AMOAQ->mBufferSizeinFrames[readIndex]) {
        AudioQueueBuffer * b = bufRef;

        b->mAudioDataByteSize = AudioMixerSoftware::framesToBytes(AMOAQ->mBufferSizeinFrames[readIndex]);
        memcpy(b->mAudioData, &AMOAQ->mBuffer[readIndex][0], b->mAudioDataByteSize);

        AudioQueueEnqueueBuffer(queue, bufRef, 0, NULL);

        AMOAQ->mBufferSizeinFrames[readIndex] = 0;
        AMOAQ->mReadIndex = (readIndex + 1) % 3;
    } else {
        if (AMOAQ->mResetPending) AMOAQ->mResetPending = false;
    }
}

AudioMixerOutputAudioQueue::AudioMixerOutputAudioQueue() :
    mReadIndex(0), mWriteIndex(0), mResetPending(false)
{
    for (int i = 0; i < 3; i++) {
        mBuffer[i].resize(AudioMixerSoftware::framesToSamples(PREFERRED_FRAME_SIZE));
        mBufferSizeinFrames[i] = 0;
    }

    AudioStreamBasicDescription fmt;
    fmt.mFormatID = kAudioFormatLinearPCM;
    fmt.mFormatFlags = kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsPacked;
    fmt.mSampleRate = 48000;
    fmt.mFramesPerPacket = 1;
    fmt.mChannelsPerFrame = 2;
    fmt.mBitsPerChannel = 16;
    fmt.mBytesPerFrame = fmt.mChannelsPerFrame * (fmt.mBitsPerChannel / 8);
    fmt.mBytesPerPacket = fmt.mBytesPerFrame * fmt.mFramesPerPacket;

    AudioQueueNewOutput(&fmt, Callback, this, NULL, NULL, 0, &mQueueRef);
}

AudioMixerOutputAudioQueue::~AudioMixerOutputAudioQueue()
{
    AudioQueueDispose(mQueueRef, true);
}

NFErr AudioMixerOutputAudioQueue::Play()
{
    mReadIndex = 0;
    mWriteIndex = 0;
    for (int i = 0; i < 3; i++) {
        mBufferSizeinFrames[i] = 0;
    }

    AudioQueueAllocateBuffer(mQueueRef, PREFERRED_FRAME_SIZE_BYTES, &mBufferRefPool[0]);
    mBufferRefPool[0]->mAudioDataByteSize = PREFERRED_FRAME_SIZE_BYTES;
    memset(mBufferRefPool[0]->mAudioData, 0, mBufferRefPool[0]->mAudioDataByteSize);
    AudioQueueEnqueueBuffer(mQueueRef, mBufferRefPool[0], 0, NULL);

    AudioQueueAllocateBuffer(mQueueRef, PREFERRED_FRAME_SIZE_BYTES, &mBufferRefPool[1]);
    mBufferRefPool[1]->mAudioDataByteSize = PREFERRED_FRAME_SIZE_BYTES;
    memset(mBufferRefPool[1]->mAudioData, 0, mBufferRefPool[1]->mAudioDataByteSize);
    AudioQueueEnqueueBuffer(mQueueRef, mBufferRefPool[1], 0, NULL);

    AudioQueueSetParameter(mQueueRef, kAudioQueueParam_Volume, 1.0);


    AudioQueueStart(mQueueRef, NULL);
    return NFErr_OK;
}

NFErr AudioMixerOutputAudioQueue::Reset()
{
    mResetPending = true;

    const struct timespec ts = { 0, 1000000};
    for (int i = 0; (mResetPending) && (i < 100); i++) {
        nanosleep(&ts, NULL);
    }

    AudioQueueFlush(mQueueRef);
    AudioQueueStop(mQueueRef, true);
    return NFErr_OK;
}

void AudioMixerOutputAudioQueue::getDeviceInfo(size_t& preferredFrames, size_t& maximumFrames, bool& supportDeferredPlay)
{
    preferredFrames = PREFERRED_FRAME_SIZE;
    maximumFrames = preferredFrames * 3;
    supportDeferredPlay = false;
}

NFErr AudioMixerOutputAudioQueue::getBufferAvailable(size_t& availableFrames)
{
    int emptyCount = 0;
    for (int i = 0; i < 3; i++) {
        if (!mBufferSizeinFrames[i]) {
            emptyCount++;
        }
    }

    availableFrames = emptyCount*PREFERRED_FRAME_SIZE;
    return NFErr_OK;
}

NFErr AudioMixerOutputAudioQueue::getBufferDelay(size_t& delayFrames)
{
    delayFrames = std::abs(mWriteIndex-mReadIndex)*PREFERRED_FRAME_SIZE;
    return NFErr_OK;
}

NFErr AudioMixerOutputAudioQueue::getBuffer(void ** buffer_ptr, const size_t requestFrames, size_t& contiguousFrames)
{
    if (!mBufferSizeinFrames[mWriteIndex]) {
        *buffer_ptr = &mBuffer[mWriteIndex][0];
        contiguousFrames = requestFrames;
    }

    return NFErr_OK;
}

NFErr AudioMixerOutputAudioQueue::commitBuffer(const size_t framesWritten)
{
    mBufferSizeinFrames[mWriteIndex] = framesWritten;
    mWriteIndex = (mWriteIndex + 1) % 3;
    return NFErr_OK;
}
