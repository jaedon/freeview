/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef DEVICE_VIDEODECODERHEVC_H
#define DEVICE_VIDEODECODERHEVC_H

/* @file VideoDecoderHEVC.h - Video decoder class used in the reference
 * application's video elementary stream player implementation. The file does
 * not implement a DPI interface. It provides reference-implementation-specific
 * video decoding functionality.
 */

#include <vector>
#include <hevc/hevc_dec_api.h>
#include "../VideoDecoder.h"

#define HEVC_DEBUG 1

#ifndef HEVC_DEBUG
#define PRINT(fmt, ...)
#else
#include <stdio.h>
#define PRINT(fmt, ...) printf("[HEVC_DEBUG] "fmt, __VA_ARGS__ )
#endif

namespace netflix {
namespace device {
namespace esplayer {


class ReferenceClockNative;
class VideoRenderer;

/**
 * @class VideoDecoderHEVCSink VideoDecoderHEVCSink.h
 * @brief Wrapper class of HEVC video decoders
 */
class VideoDecoderHEVCSink : public VideoDecoder
{
public:
    static shared_ptr<VideoDecoderHEVCSink> create(const VideoAttributes& attributes,
                                                   ReferenceClockNative& referenceClock,
                                                   const VideoRenderer::VideoRendererInfo& rendererInfo)
    {
        bool error;
        shared_ptr<VideoDecoderHEVCSink> decoder;
        decoder.reset(new VideoDecoderHEVCSink(attributes, referenceClock, rendererInfo, error));
        return decoder;
    }

    /**
     * Destructor.
     */
    virtual ~VideoDecoderHEVCSink();



    virtual ESNativeError decode(const ISampleAttributes *pSampleAttr,
                                 const unsigned char* auData, uint32_t auDataSize,
                                 VideoRenderer::VideoFrameBuffer& frameBuffer,
                                 bool& gotPicture,
                                 bool& resend,
                                 Format3D& format3D);

    virtual void reset();

    void resetCodecWithUpdatedExtraData(unsigned char*& new_extradata,
                                        const int& new_extradata_size,
                                        const int& new_width,
                                        const int& new_height);

private:
    /**
     * Constructor.
     *
     * @param[in] attributes the video attributes.
     * @param[in] rendererInfo information of the video renderer.
     * @param[out] error if there was an error in construction
     */
    VideoDecoderHEVCSink(const VideoAttributes& attributes,
                         ReferenceClockNative& referenceClock,
                         const VideoRenderer::VideoRendererInfo& rendererInfo,
                         bool& error);


    bool checkIfShouldDropFrame(const int64_t& DTS, const int64_t& currPresentationTime,
                                const bool dropable) const;
    bool checkIfShouldDropUntilNextGOP(const ISampleAttributes* pSampleAttr,
                                       const int64_t& currPresentationTime);

    ReferenceClockNative& referenceClock_;

    uint32_t videoDroppedFrameCountDec_;
    uint32_t consecutiveDroppedFrameCount_;
    bool     dropUntilNextGOP_;
    const VideoAttributes mAttributes;

    // Vanguard HEVC decoder specific
    hevcd_settings_t decoderSettings_;
    hevcd_dec_t decoderHandle_; // decoder handle
    hevc_callbacks_t callbacks_;
    hevc_timestamp_t timestamp_;

    FILE* fp_;


};

} // namespace esplayer
} // namespace device
} // namespace netflix

#endif // DEVICE_VIDEODECODERHEVC_H
