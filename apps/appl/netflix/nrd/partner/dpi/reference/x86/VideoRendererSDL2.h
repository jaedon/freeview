/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef DEVICE_VIDEORENDERERSDL2_H
#define DEVICE_VIDEORENDERERSDL2_H

#include "../VideoRenderer.h"
#include "../ESPlayerConstants.h"
#include <SDL2/SDL.h>

struct SDL_Surface;
struct SDL_Overlay;
struct SDL_Rect;

namespace netflix {
namespace device {
namespace esplayer {

/**
 * @class VideoRendererSDL2 VideoRendererSDL2.h
 * @brief Implementation of video rendering over SDL.
 */
class VideoRendererSDL2 : public VideoRenderer
{
public:
    /**
     * Constructor.
     *
     * @param[in] attributes the video attributes.
     * @param[in] maxImageWidth the maximum width (in pixels) of all possible images.
     * @param[in] maxImageHeight the maximum height (in pixels) of all possible images.
     */
    VideoRendererSDL2(const VideoAttributes& attributes,
                      uint32_t maxImageWidth, uint32_t maxImageHeight);

    /**
     * Destructor.
     */
    virtual ~VideoRendererSDL2();

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
    static SDL_Surface* primarySurface_;
    static const bool   supportScaling_;

    SDL_Window*   window_;
    SDL_Renderer* renderer_;
    SDL_Texture* videoTexture_;

    uint8_t** renderBuffers_;
    uint32_t pixelAspectRatioX_;
    uint32_t pixelAspectRatioY_;
    uint32_t imageWidth_;
    uint32_t imageHeight_;
    bool clearOnFlip_;

private: // helpers
    void cleanUp();
};

} // namespace esplayer
} // namespace device
} // namespace netflix

#endif // DEVICE_VIDEORENDERERSDL2_H
