/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef DEVICE_AUDIODECODERNATIVE_H
#define DEVICE_AUDIODECODERNATIVE_H

/* @file AudioDecoderNative.h - Audio decoder class used in the reference
 * application's audio elementary stream player implementation. The file does
 * not implement a DPI interface. It provides reference-implementation-specific
 * audio decoding functionality.
 */

#include "AudioDecoder.h"
//include "DecoderStatsCounter.h"

// FFMPEG headers
extern "C"
{
#if 0 /* dragon */
#include <libavcodec/avcodec.h>
#include <libswresample/swresample.h>
#endif
}

struct AVCodecContext;

namespace netflix {
namespace device {
namespace esplayer {


typedef struct AudioCodecSpecificData
{
	uint8_t *extradata;
	int32_t extradata_size;
}AudioCodecSpecificData_t;


/**
 * @class AudioDecoderNative AudioDecoderNative.h
 * @brief Wrapper class of native audio decoders
 */
class AudioDecoderNative : public AudioDecoder
{
public:
    /**
     * Constructor.
     *
     * @param[in] numOutputChannels the number of output channels of the audio renderer.
     */
    AudioDecoderNative(uint32_t numOutputChannels);

    /**
     * Destructor.
     */
    virtual ~AudioDecoderNative();

    virtual bool decode(const ISampleAttributes*  pMediaSample,
                        const unsigned char* auData, uint32_t auDataSize,
                        UVAWriteFrame_t *pstUVAWriteFrame,
                        AudioESPlayerNative::DecodedAudioBuffer& outputBuffer);

    virtual void reset();
    virtual bool updateAttributes(const AudioAttributes* attributes);

private:
    static const unsigned char GUID_FORMAT_OggIdentHeader[];
    static const unsigned char GUID_FORMAT_DolbyDigitalPlus[];

    AVCodecContext* codecContext_;
#ifdef HAVE_DDPLUS
    void* ddplusdecContext_;
#endif

    bool decode(const unsigned char* auData, uint32_t auDataSize,
                unsigned char* outputBuffer, int capacity, int& decodedBytes);

private: // helpers

    void handleWaveFormatExtensible(const uint8_t *codecSpecificData, uint32_t codecSpecificDataLen);
    void handleOggIdentHeader(const unsigned char* data, uint32_t bytes);

private: // OGG parser

    class OggPacketParser;
    std::auto_ptr<OggPacketParser> oggPacketParser_;

private: // ffmpeg data packets
#if 0 /* dragon */
    AVFrame *decoded_frame_;         //decoded audio frame
    AVPacket avpkt_;                        //bitstream packet
    SwrContext *swr_;
#endif
    AudioCodecSpecificData_t m_stSpecificData;
};

}}} // namespace netflix::device::IESP

#endif // DEVICE_AUDIODECODERNATIVE_H
