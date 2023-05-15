/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "AudioDecoder.h"
#include <nrdbase/ScopedMutex.h>
#include <nrdbase/MutexRanks.h>

using namespace netflix::device::esplayer;
using namespace netflix;

AudioDecoder::AudioDecoder(uint32_t numOutputChannels)
  : numOutputChannels_(numOutputChannels),
    flushState_(ESPlayerNative::FLUSH_OFF),
    inputExhausted_(true),
    mutex_(ESP_DECODER_MUTEX, "AudioDecoder Mutex")
{
}

AudioDecoder::~AudioDecoder()
{
}

void AudioDecoder::setFlushState(ESPlayerNative::FlushState flushState)
{
    ScopedMutex cs(mutex_);
    flushState_ = flushState;
}

ESPlayerNative::FlushState AudioDecoder::getFlushState()
{
    ScopedMutex cs(mutex_);
    return flushState_;
}

void AudioDecoder::setInputExhausted(bool inputExhausted)
{
    ScopedMutex cs(mutex_);
    inputExhausted_ = inputExhausted;
}

bool AudioDecoder::isInputExhausted()
{
    ScopedMutex cs(mutex_);
    return inputExhausted_;
}
