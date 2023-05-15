/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef DEVICE_VIDEORENDERERDFB_H
#define DEVICE_VIDEORENDERERDFB_H

#include "../VideoRenderer.h"
#include <directfb.h>

namespace netflix {
namespace device {
namespace esplayer {


/**
 * @class VideoRendererDFB VideoRendererDFB.h
 * @brief Implementation of video rendering over DirectFB.
 */
class VideoRendererDFB : public VideoRenderer
{
public:
    /**
     * Constructor.
     *
     * @param[in] attributes the video attributes.
     * @param[in] maxImageWidth the maximum width (in pixels) of all possible images.
     * @param[in] maxImageHeight the maximum height (in pixels) of all possible images.
     */
    VideoRendererDFB(const VideoAttributes& attributes,
                     uint32_t maxImageWidth,
                     uint32_t maxImageHeight,
                     uint32_t pipelineId);

    /**
     * Destructor.
     */
    virtual ~VideoRendererDFB();

    virtual bool init();

    virtual bool lockBackBuffer(uint32_t backBufferIndex,
                                void* &priv,
                                unsigned char* &plane0,
                                unsigned char* &plane1,
                                unsigned char* &plane2,
                                unsigned char* &plane3,
                                uint32_t &pitch0,
                                uint32_t &pitch1,
                                uint32_t &pitch2,
                                uint32_t &pitch3);

    virtual bool unlockBackBuffer(uint32_t backBufferIndex, void* priv);

    virtual bool flipBackBuffer(uint32_t backBufferIndex,
                                uint32_t width,
                                uint32_t height,
                                void* priv);
    virtual void setVideoWindow(Rect r);
private:
    IDirectFB *mDfb;
    IDirectFBDisplayLayer *mLayer;
    int mLayerWidth, mLayerHeight;
    IDirectFBWindow *mWindow;
    IDirectFBSurface *mWindowSurface;
    IDirectFBSurface **mBuffers;
    Rect mVideoWindow;
    const VideoAttributes mAttributes;
    bool mClearOnFlip;
    uint32_t mMaxImageWidth;
    uint32_t mMaxImageHeight;
    int32_t  mLastFramePresented;
};

} // namespace esplayer
} // namespace device
} // namespace netflix

#endif // DEVICE_VIDEORENDERERDFB_H
