/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef DEVICE_AUDIODECODEROMXIL_H
#define DEVICE_AUDIODECODEROMXIL_H

/* @file AudioDecoderOMXIL.h - Audio decoder class used in the reference
 * application's audio elementary stream player implementation. The file does
 * not implement a DPI interface. It provides reference-implementation-specific
 * audio decoding functionality.
 */

#include "../AudioDecoder.h"
#include "ComponentOMXIL.h"

namespace netflix {
namespace device {
namespace esplayer {

/**
 * @class AudioDecoderOMXIL AudioDecoderOMXIL.h
 * @brief Wrapper class of omxil audio decoders
 */
class AudioDecoderOMXIL : public AudioDecoder, private ComponentOMXIL
{
public:
    /**
     * Constructor.
     *
     * @param[in] numOutputChannels the number of output channels of the audio renderer.
     */
    AudioDecoderOMXIL(uint32_t numOutputChannels);

    /**
     * Destructor.
     */
    virtual ~AudioDecoderOMXIL();

    virtual bool decode(const ISampleAttributes *pMediaSample,
                        const unsigned char *auData, uint32_t auDataSize,
                        AudioESPlayerNative::DecodedAudioBuffer &outputBuffer);

    virtual void reset();
    virtual bool updateAttributes(const AudioAttributes *attributes);

protected:

    unsigned int mFrames;
#if defined(BUILD_OPENMAXIL_AUDIO_RENDERER)
    ComponentOMXIL mRenderer;
#endif

};

}}} // namespace netflix::device::IESP

#endif // DEVICE_AUDIODECODEROMXIL_H
