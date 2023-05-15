/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "AudioDecoderSink.h"
#include "../ESPlayerNative.h"
#include "../MediaUtils.h"
#include <cstring>


#include <nrdbase/ScopedMutex.h>
#include <nrdbase/Endian.h>
#include <nrdbase/Log.h>
#include <nrddpi/Common.h>
#include "../DecoderLock.h"

using namespace std;
using namespace netflix::device::esplayer;
using namespace netflix;

AudioDecoderSink::AudioDecoderSink(uint32_t numOutputChannels)
  : AudioDecoder(numOutputChannels)
{

}

AudioDecoderSink::~AudioDecoderSink()
{

}

bool AudioDecoderSink::decode(const ISampleAttributes*  pSampleAttr,
                                const unsigned char* auData, uint32_t auDataSize,
                                AudioESPlayerNative::DecodedAudioBuffer& decodedAudioBuffer)
{
    NRDP_UNUSED(auData);
    NRDP_UNUSED(auDataSize);
    decodedAudioBuffer.setDataBytes(8192);
    decodedAudioBuffer.setTimestamp(pSampleAttr->getPTS());
    return true;
}

void AudioDecoderSink::reset()
{

}

bool AudioDecoderSink::updateAttributes(const AudioAttributes* attributes)
{
    NRDP_UNUSED(attributes);
    return true;
}
