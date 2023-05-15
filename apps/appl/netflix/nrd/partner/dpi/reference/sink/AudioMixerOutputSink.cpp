/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "AudioMixerSoftware.h"
#include "AudioMixerOutputSink.h"
#include <nrd/AppLog.h>
#include <nrd/NrdApplication.h>
#include <nrdbase/Time.h>

using namespace netflix;
using namespace netflix::application;
using namespace netflix::base;
using namespace netflix::device;

AudioMixerOutputSink::AudioMixerOutputSink() :
    mPlaying(false), mCommittedFrames(0)
{
}

AudioMixerOutputSink::~AudioMixerOutputSink()
{
}

NFErr AudioMixerOutputSink::Play()
{
    mPlaying = true;
    mPlayStart = Time::monoMS();
    return NFErr_OK;
}

NFErr AudioMixerOutputSink::Reset()
{
    uint32_t framesLeft = framesLeftToPlayOut();

    if (framesLeft) {
        Thread::Sleep(AudioMixerSoftware::framesToMs(framesLeft)+1);
    }

    mPlaying = false;
    mCommittedFrames = 0;
    return NFErr_OK;
}

void AudioMixerOutputSink::getBufferInfo(size_t& preferredFrames, size_t& maximumFrames)
{
    preferredFrames = PREFERRED_FRAME_SIZE;
    maximumFrames = msToFrames(FIFO_SIZE_IN_MS);

    mBuffer.resize(AudioMixerSoftware::framesToBytes(PREFERRED_FRAME_SIZE));
}

NFErr AudioMixerOutputSink::getBufferStatus(size_t& availableFrames)
{
    if (mPlaying) {
        uint32_t framesLeft = framesLeftToPlayOut();
        availableFrames = msToFrames(FIFO_SIZE_IN_MS) - framesLeft;
    } else {
        availableFrames = msToFrames(FIFO_SIZE_IN_MS) - mCommittedFrames;
    }

    return NFErr_OK;
}

NFErr AudioMixerOutputSink::getBuffer(void ** buffer_ptr, const size_t requestFrames, size_t& contiguousFrames)
{
    *buffer_ptr = &mBuffer[0];
    contiguousFrames = requestFrames;
    return NFErr_OK;
}

NFErr AudioMixerOutputSink::commitBuffer(const size_t framesWritten)
{
    mCommittedFrames += framesWritten;
    return NFErr_OK;
}

uint32_t AudioMixerOutputSink::framesLeftToPlayOut()
{
        uint32_t timeElapsed = Time::monoMS() - mPlayStart;
        int32_t framesLeftToPlayOut = mCommittedFrames - msToFrames(timeElapsed);
        return std::max(framesLeftToPlayOut, 0);
}
