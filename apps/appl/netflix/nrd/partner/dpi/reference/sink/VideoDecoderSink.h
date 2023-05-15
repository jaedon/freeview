/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef DEVICE_VIDEODECODERSINK_H
#define DEVICE_VIDEODECODERSINK_H

/* @file VideoDecoderSink.h - Video decoder class used in the reference
 * application's video elementary stream player implementation. The file does
 * not implement a DPI interface. It provides reference-implementation-specific
 * video decoding functionality.
 */


#include "../VideoDecoder.h"
#include <vector>

namespace netflix {
namespace device {
namespace esplayer {


class ReferenceClockNative;
class VideoRenderer;


/**
 * @class VideoDecoderSink VideoDecoderSink.h
 * @brief Wrapper class of sink video decoders
 */
class VideoDecoderSink : public VideoDecoder
{
public:
    static shared_ptr<VideoDecoderSink> create(const VideoAttributes& attributes,
                                               ReferenceClockNative& referenceClock,
                                               const VideoRenderer::VideoRendererInfo& rendererInfo)
    {
        bool error;
        shared_ptr<VideoDecoderSink> decoder;
        decoder.reset(new VideoDecoderSink(attributes, referenceClock, rendererInfo, error));
        return decoder;
    }

    /**
     * Destructor.
     */
    virtual ~VideoDecoderSink();

    virtual RefAppDeviceSpecificError decode(const ISampleAttributes *pSampleAttr,
                                             const unsigned char* auData, uint32_t auDataSize,
                                             VideoRenderer::VideoFrameBuffer& frameBuffer,
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
    VideoDecoderSink(const VideoAttributes& attributes,
                     ReferenceClockNative& referenceClock,
                     const VideoRenderer::VideoRendererInfo& rendererInfo,
                     bool& error);

    // Returns true if codec should be reset.
    bool getUpdatedH264ExtraData(unsigned char*& new_extradata,
                                 int& new_extradata_size,
                                 const std::vector<const uint8_t*>& paramSets,
                                 const std::vector<uint32_t>& paramSetLens,
                                 int& new_width, int& new_height) const;

    bool checkIfShouldDropFrame(const int64_t& DTS,
                                const int64_t& currPresentationTime,
                                const bool dropable) const;

    bool checkIfShouldDropUntilNextGOP(const ISampleAttributes* pSampleAttr,
                                       const int64_t& currPresentationTime);

    void resetCodecWithUpdatedExtraData(unsigned char*& new_extradata,
                                        const int& new_extradata_size,
                                        const int& new_width,
                                        const int& new_height);

    ReferenceClockNative& referenceClock_;

    shared_ptr<std::vector<uint8_t> > mCodecSpecificData;

    uint32_t videoDroppedFrameCountDec_;
    uint32_t consecutiveDroppedFrameCount_;
    bool     dropUntilNextGOP_;
    bool     enablePrintDTS_;
    bool firstDecodingAfterDecoderOpen_;
};

} // namespace esplayer
} // namespace device
} // namespace netflix

#endif // DEVICE_VIDEODECODERSINK_H
