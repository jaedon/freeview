/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef DEVICE_AUDIODECODERSINK_H
#define DEVICE_AUDIODECODERSINK_H

/* @file AudioDecoderSink.h - Audio decoder class used in the reference
 * application's audio elementary stream player implementation. The file does
 * not implement a DPI interface. It provides reference-implementation-specific
 * audio decoding functionality.
 */

#include "../AudioDecoder.h"

namespace netflix {
namespace device {
namespace esplayer {


/**
 * @class AudioDecoderSink AudioDecoderSink.h
 * @brief Wrapper class of sink audio decoders
 */
class AudioDecoderSink : public AudioDecoder
{
public:
    /**
     * Constructor.
     *
     * @param[in] numOutputChannels the number of output channels of the audio renderer.
     */
    AudioDecoderSink(uint32_t numOutputChannels);

    /**
     * Destructor.
     */
    virtual ~AudioDecoderSink();

    virtual bool decode(const ISampleAttributes*  pMediaSample,
                        const unsigned char* auData, uint32_t auDataSize,
                        AudioESPlayerNative::DecodedAudioBuffer& outputBuffer);

    virtual void reset();
    virtual bool updateAttributes(const AudioAttributes* attributes);
};

}}} // namespace netflix::device::IESP

#endif // DEVICE_AUDIODECODERSINK_H
