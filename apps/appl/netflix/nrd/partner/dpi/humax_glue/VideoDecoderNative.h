/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef DEVICE_VIDEODECODERNATIVE_H
#define DEVICE_VIDEODECODERNATIVE_H

/* @file VideoDecoderNative.h - Video decoder class used in the reference
 * application's video elementary stream player implementation. The file does
 * not implement a DPI interface. It provides reference-implementation-specific
 * video decoding functionality.
 */


#include "VideoDecoder.h"
#include <vector>

// FFMPEG headers
extern "C"
{
#if 0 /* dragon */
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#endif
}

//#define DBG_FILEDUMP 1

struct AVCodecContext;
struct AVFrame;
struct SwsContext;

namespace netflix {
namespace device {
namespace esplayer {


class ReferenceClockNative;

typedef struct H264ExtraData
{
	uint8_t *extradata;
	int32_t extradata_size;
	int32_t width;
	int32_t height;
}H264ExtraData_t;

/**
 * @class VideoDecoderNative VideoDecoderNative.h
 * @brief Wrapper class of native video decoders
 */
class VideoDecoderNative : public VideoDecoder
{
public:
#if 0
    static shared_ptr<VideoDecoderNative> create(const VideoAttributes& attributes,
                                                 ReferenceClockNative& referenceClock,
                                                 const VideoRenderer::VideoRendererInfo& rendererInfo,
                                                 uint32_t pipelineId = 0)
    {
        bool error;
        shared_ptr<VideoDecoderNative> decoder;
        decoder.reset(new VideoDecoderNative(attributes, referenceClock, rendererInfo, pipelineId, error));
        if(error == true){
            decoder.reset();
        }
        return decoder;
    }
#else
    static shared_ptr<VideoDecoderNative> create(const VideoAttributes& attributes, uint32_t pipelineId = 0)
    {
        bool error;
        shared_ptr<VideoDecoderNative> decoder;
        decoder.reset(new VideoDecoderNative(attributes, pipelineId, error));
        if(error == true){
            decoder.reset();
        }
        return decoder;
    }
#endif

    /**
     * Destructor.
     */
    virtual ~VideoDecoderNative();

#if 0
    virtual RefAppDeviceSpecificError decode(const ISampleAttributes *pSampleAttr,
#else
    virtual ESNativeError decode(const ISampleAttributes *pSampleAttr,
#endif
                                             const unsigned char* auData, uint32_t auDataSize,
                                             UVAWriteFrame_t *pstUvaWriteFrame, int32_t nCsoVideoCodec,
                                             bool& gotPicture,
                                             bool& resend,
                                             Format3D& format3D);

    virtual void reset();

private:
    /**
     * Constructor.
     *
     * @param[in] attributes the video attributes.
     * @param[in] rendererInfo information of the video renderer.
     * @param[out] error if there was an error in construction
     */
#if 0
    VideoDecoderNative(const VideoAttributes& attributes,
                       ReferenceClockNative& referenceClock,
                       const VideoRenderer::VideoRendererInfo& rendererInfo,
                       uint32_t pipelineId,
                       bool& error);
#else
    VideoDecoderNative(const VideoAttributes& attributes,
                       uint32_t pipelineId,
                       bool& error);
#endif

#if 0
    void imageConvert(AVFrame& decodedFrame, VideoRenderer::VideoFrameBuffer& frameBuffer);
#else
    void imageConvert(AVFrame& decodedFrame);
#endif

    // Returns true if codec should be reset.
    bool getUpdatedH264ExtraData(
        unsigned char*& new_extradata, int& new_extradata_size,
        const std::vector<const uint8_t*>& paramSets,
        const std::vector<uint32_t>& paramSetLens,
        int& new_width, int& new_height) const;

    bool checkIfShouldDropFrame(const int64_t& DTS, const int64_t& currPresentationTime,
                                const bool dropable) const;
    bool checkIfShouldDropUntilNextGOP(const ISampleAttributes* pSampleAttr,
                                       const int64_t& currPresentationTime);
    bool resetCodecWithUpdatedExtraData(unsigned char*& new_extradata, const int& new_extradata_size,
                                        const int& new_width, const int& new_height);
    void cleanUp();

    //ReferenceClockNative& referenceClock_;
#if 0 /* dragon */
    AVCodecContext* codecContext_;
    AVFrame* decodedFrame_;
    AVPacket avpkt_;
    SwsContext* imageConvertContext_;
#endif

    uint32_t videoDroppedFrameCountDec_;
    uint32_t consecutiveDroppedFrameCount_;
    bool     dropUntilNextGOP_;
    VideoAttributes mAttributes;

    uint32_t currentPixelAspectRatioX_;
    uint32_t currentPixelAspectRatioY_;

    H264ExtraData_t m_stH264ExtraData;

#if defined(DBG_FILEDUMP)
    FILE* fp_;
#endif
};

} // namespace esplayer
} // namespace device
} // namespace netflix

#endif // DEVICE_VIDEODECODERNATIVE_H
