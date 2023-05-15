/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef DEVICE_AUDIODECODER_H
#define DEVICE_AUDIODECODER_H

/* @file AudioDecoder.h - Audio decoder base class used in the reference
 * application's audio elementary stream player implementation. The file does
 * not implement a DPI interface. It provides reference-implementation-specific
 * audio decoding functionality.
 */

#include "AudioESPlayerNative.h"
#include <nrdbase/Mutex.h>

namespace netflix {
namespace device {
namespace esplayer {


struct AccessUnitAttributes;

/**
 * @class AudioDecoder AudioDecoder.h
 * @brief Base implementation for audio decoder engines.
 */
class AudioDecoder
{
public:
    /**
     * Constructor.
     *
     * @param[in] numOutputChannels the number of output channels of the audio renderer.
     */
    AudioDecoder(uint32_t numOutputChannels);

    /**
     * Destructor.
     */
    virtual ~AudioDecoder();

    /**
     * Decode the input access-unit and output the decoded samples into the sample buffer.
     *
     * @param[in] auData pointer to the access-unit data.
     * @param[in] auDataSize number of bytes of the access-unit data.
     * @param[in] accessUnitAttributes the attributes associated with the access-unit.
     * @param[out] sampleBuffer the output sample buffer.
     *
     * @return true if successful, false if there is some decoding problem.
     */
    virtual bool decode(const ISampleAttributes*  pMediaSampleAttr,
                        const unsigned char* auData, uint32_t auDataSize,
                        UVAWriteFrame *pstUVAWriteFrame,
                        AudioESPlayerNative::DecodedAudioBuffer& outputBuffer) = 0;

    /** Reset the decoder. */
    virtual void reset() = 0;

    /**
     * Update decoder attributes.
     *
     * @param[in] attributes the new attributes.
     *
     * return true if successful, false if something went wrong.
     */
    virtual bool updateAttributes(const AudioAttributes* attributes) = 0;

    /**
     * Set the state of flush operation (inactive/has-begun/has-ended).
     *
     * @param[in] flushState the new flush operation state to be set.
     */
    void setFlushState(ESPlayerNative::FlushState flushState);

    /**
     * Get the state of flush operation (inactive/has-begun/has-ended).
     *
     * @return the current flush operation state.
     */
    ESPlayerNative::FlushState getFlushState();

    /**
     * Set or reset the state of input data exhaustion.
     *
     * @param[in] inputExhausted true to set the exhaustion state; false to reset.
     */
    void setInputExhausted(bool inputExhausted);

    /**
     * Check whether the input data is exhausted.
     *
     * @return true if the input data is exhausted; false otherwise.
     */
    bool isInputExhausted();

    DecoderStatsCounter& getStatsCounter()
    {
        return statsCounter_;
    }

protected:
    /** the number of output channels of the audio renderer */
    uint32_t numOutputChannels_;

    DecoderStatsCounter statsCounter_;

private:
    ESPlayerNative::FlushState flushState_;
    bool inputExhausted_;
    Mutex mutex_;
};

} // namespace esplayer
} // namespace device
} // namespace netflix

#endif // DEVICE_AUDIODECODER_H
