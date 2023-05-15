/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef DEVICE_VIDEORENDERERSINK_H
#define DEVICE_VIDEORENDERERSINK_H

#include "../VideoRenderer.h"

namespace netflix {
namespace device {
namespace esplayer {

/**
 * @class VideoRendererSink VideoRendererSink.h
 * @brief Implementation of sink video rendering.
 */
class VideoRendererSink : public VideoRenderer
{
public:
    friend class RendererManager;

    /**
     * Constructor.
     *
     * @param[in] attributes the video attributes.
     * @param[in] maxImageWidth the maximum width (in pixels) of all possible images.
     * @param[in] maxImageHeight the maximum height (in pixels) of all possible images.
     */
    VideoRendererSink(const VideoAttributes& attributes,
                       uint32_t maxImageWidth, uint32_t maxImageHeight);

    /**
     * Destructor.
     */
    virtual ~VideoRendererSink();

    virtual bool init();

    virtual bool lockBackBuffer(uint32_t       backBufferIndex,
                                void*          &priv,
                                unsigned char* &plane0,
                                unsigned char* &plane1,
                                unsigned char* &plane2,
                                unsigned char* &plane3,
                                uint32_t       &pitch0,
                                uint32_t       &pitch1,
                                uint32_t       &pitch2,
                                uint32_t       &pitch3);

    virtual bool unlockBackBuffer(uint32_t backBufferIndex, void* priv);

    virtual bool flipBackBuffer(uint32_t backBufferIndex,
                                uint32_t width,
                                uint32_t height,
                                void*    priv);

private:
    struct Frame
    {
        unsigned char* y;
        unsigned char* u;
        unsigned char* v;

        uint32_t y_pitch;
        uint32_t u_pitch;
        uint32_t v_pitch;

        uint32_t y_lines;
        uint32_t u_lines;
        uint32_t v_lines;
    };

    Frame* frameBuffers_;
};

} // namespace esplayer
} // namespace device
} // namespace netflix


#endif // DEVICE_VIDEORENDERERSINK_H
