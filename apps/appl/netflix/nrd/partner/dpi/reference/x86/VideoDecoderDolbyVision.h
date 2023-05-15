/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef DEVICE_VIDEODECODER_DOLBYVISION_H
#define DEVICE_VIDEODECODER_DOLBYVISION_H

/* @file VideoDecoderDolbyVision.h - Video decoder class used in the reference
 * application's video elementary stream player implementation. The file does
 * not implement a DPI interface. It provides reference-implementation-specific
 * video decoding functionality.
 */

#include <vector>
#include <hevc/hevc_dec_api.h>
#include "../VideoDecoder.h"

struct SwsContext;

namespace netflix {
namespace device {
namespace esplayer {

class ReferenceClockNative;
class VideoRenderer;

/**
 * @class VideoDecoderDolbyVision VideoDecoderDolbyVision.h
 * @brief Wrapper class of DolbyVision video decoders
 */
class VideoDecoderDolbyVision : public VideoDecoder
{
public:
    static shared_ptr<VideoDecoderDolbyVision> create(const VideoAttributes& attributes,
                                                      ReferenceClockNative& referenceClock,
                                                      const VideoRenderer::VideoRendererInfo& rendererInfo,
                                                      uint32_t pipelineId)
    {
        bool error;
        shared_ptr<VideoDecoderDolbyVision> decoder;
        decoder.reset(new VideoDecoderDolbyVision(attributes, referenceClock, rendererInfo, pipelineId, error));
        return decoder;
    }

    /**
     * Destructor.
     */
    virtual ~VideoDecoderDolbyVision();



    virtual RefAppDeviceSpecificError decode(const ISampleAttributes *pSampleAttr,
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
    VideoDecoderDolbyVision(const VideoAttributes& attributes,
                            ReferenceClockNative& referenceClock,
                            const VideoRenderer::VideoRendererInfo& rendererInfo,
                            uint32_t pipelineId,
                            bool& error);


    bool checkIfShouldDropFrame(const int64_t& DTS, const int64_t& currPresentationTime,
                                const bool dropable) const;
    bool checkIfShouldDropUntilNextGOP(const ISampleAttributes* pSampleAttr,
                                       const int64_t& currPresentationTime);

    void imageConvert(ptr_hevc_vframe_descriptor_t p_vframe,
                      VideoRenderer::VideoFrameBuffer& frameBuffer);

    hevc_error_t onFrameDecoded(hevc_vframe_descriptor_t * dec_frame);

    void init_buffer(int width, int height);

    void cleanup_buffer();

    void shift_and_copy_lowerbyte(unsigned char* dest, unsigned char* src, int size);

    void vector_copy_lowerbyte(void * dst,
                               const void * src,
                               const size_t width,
                               const size_t bytes_per_pel);

    void writeDolbyVisionEL2File(const ISampleAttributes *pSampleAttr,
                                 const unsigned char* auData,
                                 uint32_t auDataSize);

    ReferenceClockNative& referenceClock_;

    uint32_t videoDroppedFrameCountDec_;
    uint32_t consecutiveDroppedFrameCount_;
    bool     dropUntilNextGOP_;
    VideoAttributes mAttributes;

    // Vanguard DolbyVision decoder specific
    hevcd_settings_t decoderSettings_;
    hevcd_dec_t decoderHandle_; // decoder handle
    hevc_callbacks_t callbacks_;
    hevc_timestamp_t timestamp_;


    // for debugging
    FILE* fp_;
    uint32_t frameFeedCounter_;
    uint32_t frameReadCounter_;

    SwsContext* imageConvertContext_;
    bool enablePrintDTS_;

    uint32_t currentPixelAspectRatioX_;
    uint32_t currentPixelAspectRatioY_;

    uint32_t initialBitsDepth_;
    uint32_t currentBitsDepth_;

    std::string mDolbyFileDumpPath;
    FILE* fpDolbyFile;

    unsigned char *y_data;
    unsigned char *cr_data;
    unsigned char *cb_data;

    unsigned char *y8_data;
    unsigned char *cr8_data;
    unsigned char *cb8_data;

};

} // namespace esplayer
} // namespace device
} // namespace netflix

#endif // DEVICE_VIDEODECODERDolbyVision_H
