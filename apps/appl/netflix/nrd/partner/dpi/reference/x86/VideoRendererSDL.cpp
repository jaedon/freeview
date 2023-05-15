/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "VideoRendererSDL.h"
#include "../ESPlayerConstants.h"
#include <nrdbase/Exportable.h>

// SDL headers
#include <SDL/SDL.h>

#include <nrddpi/Common.h>

using namespace netflix::device::esplayer;
using namespace netflix;


VideoRenderer *createVideoRenderer(const VideoAttributes& attributes,
                                   uint32_t maxImageWidth,
                                   uint32_t maxImageHeight)
{
    NRDP_UNUSED(maxImageWidth);
    NRDP_UNUSED(maxImageHeight);
    return new VideoRendererSDL(attributes,
                                ESPlayerConstants::VIDEO_REN_SCREEN_WIDTH_4K,
                                ESPlayerConstants::VIDEO_REN_SCREEN_HEIGHT_4K);
}

SDL_Surface* VideoRendererSDL::primarySurface_ = NULL;
const bool   VideoRendererSDL::supportScaling_ = false;

VideoRendererSDL::VideoRendererSDL(const VideoAttributes& attributes,
                                   uint32_t maxImageWidth,
                                   uint32_t maxImageHeight)

    : VideoRenderer(maxImageWidth, maxImageHeight),
      overlayBuffers_(new SDL_Overlay*[ESPlayerConstants::VIDEO_REN_FRAME_BUFFERS]),
      dstRect_(new SDL_Rect)
{

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "Can't initialize SDL\n");
        return;
    }

    uint32_t flags = SDL_HWSURFACE | SDL_ASYNCBLIT | SDL_HWACCEL | SDL_DOUBLEBUF ;

    primarySurface_ = SDL_SetVideoMode(ESPlayerConstants::VIDEO_REN_SCREEN_WIDTH_4K,
                                       ESPlayerConstants::VIDEO_REN_SCREEN_HEIGHT_4K, 0, flags);

    if (primarySurface_ == NULL) {
        fprintf(stderr, "failed in SDL_SetVideoMode");
        return;
    }

    uint32_t i;

    for(i = 0; i < ESPlayerConstants::VIDEO_REN_FRAME_BUFFERS; i ++)
        overlayBuffers_[i] = NULL;

    // Calculate the destination rectangle on the primary surface
    //

    pixelAspectRatioX_ = attributes.mPixelAspectRatioX;
    pixelAspectRatioY_ = attributes.mPixelAspectRatioY;
    imageWidth_  = ESPlayerConstants::VIDEO_REN_SCREEN_WIDTH_4K;//attributes.mImageWidth;
    imageHeight_ = ESPlayerConstants::VIDEO_REN_SCREEN_HEIGHT_4K;//attributes.mImageHeight;

    Rect r;
    r.x = 0;
    r.y = 0;
    r.width = ESPlayerConstants::VIDEO_REN_SCREEN_WIDTH_4K;
    r.height = ESPlayerConstants::VIDEO_REN_SCREEN_HEIGHT_4K;
    setVideoWindow(r);

    // Create overlay buffers (the frame/back buffers)
    // YV12 is the only supported format for now.

    uint32_t pixelFormat = _FOURCC_LE_('Y','V','1','2');

    for(i = 0; i < ESPlayerConstants::VIDEO_REN_FRAME_BUFFERS; i ++)
    {

#if 0
        overlayBuffers_[i] = SDL_CreateYUVOverlay(maxImageWidth_,
                                                  maxImageHeight_,
                                                  SDL_YV12_OVERLAY,
                                                  primarySurface_);
#else
        // for 4K
        overlayBuffers_[i] = SDL_CreateYUVOverlay(ESPlayerConstants::VIDEO_REN_SCREEN_WIDTH_4K,
                                                  ESPlayerConstants::VIDEO_REN_SCREEN_HEIGHT_4K,
                                                  SDL_YV12_OVERLAY,
                                                  primarySurface_);
#endif
        if(overlayBuffers_[i] == NULL)
        {
            if(i < ESPlayerConstants::VIDEO_REN_MIN_FRAME_BUFFERS) {
                fprintf(stderr, "failed to create minimum frame buffers via SDL_CreateYUVOverlay");
                cleanUp();
                return;
            }
            break;
        }
    }

    SDL_FillRect(primarySurface_, NULL, 0);
    SDL_UpdateRect(primarySurface_, 0, 0, 0, 0);

    init(i, pixelFormat, supportScaling_); // finish initialization
}

VideoRendererSDL::~VideoRendererSDL()
{
    cleanUp();
    SDL_Quit();
}

bool VideoRendererSDL::lockBackBuffer(
    uint32_t       backBufferIndex,
    void*          &priv,
    unsigned char* &plane0,
    unsigned char* &plane1,
    unsigned char* &plane2,
    unsigned char* &plane3,
    uint32_t       &pitch0,
    uint32_t       &pitch1,
    uint32_t       &pitch2,
    uint32_t       &pitch3)
{
    assert(backBufferIndex < getNumFrameBuffers());
    SDL_Overlay* overlay = overlayBuffers_[backBufferIndex];

    SDL_LockYUVOverlay(overlay);

    priv = NULL; // not used

    plane0 = overlay->pixels[0];
    plane1 = overlay->pixels[1];
    plane2 = overlay->pixels[2];
    plane3 = NULL;

    pitch0 = overlay->pitches[0];
    pitch1 = overlay->pitches[1];
    pitch2 = overlay->pitches[2];
    pitch3 = 0;

    return true;
}

bool VideoRendererSDL::unlockBackBuffer(
    uint32_t backBufferIndex, void* /*priv*/)
{
    assert(backBufferIndex < getNumFrameBuffers());

    SDL_UnlockYUVOverlay(overlayBuffers_[backBufferIndex]);
    return true;
}

bool VideoRendererSDL::flipBackBuffer(
    uint32_t backBufferIndex,
    uint32_t width,
    uint32_t height,
    void* /*priv*/)
{
    assert(backBufferIndex < getNumFrameBuffers());

#if 0
    /*
     *temporarily disable while we test CE4 stream against CE3 information
     */
    assert(width  > 0 && width  <= maxImageWidth_ &&
           height > 0 && height <= maxImageHeight_ );
#endif

    if(supportScaling_)
    {
        overlayBuffers_[backBufferIndex]->w = width;
        overlayBuffers_[backBufferIndex]->h = height;
    }

    if(clearOnFlip_)
    {
        clearOnFlip_ = false;

        SDL_FillRect(primarySurface_, NULL, 0);
        SDL_UpdateRect(primarySurface_, 0, 0, 0, 0);
    }

    SDL_DisplayYUVOverlay(overlayBuffers_[backBufferIndex], dstRect_);
    return true;
}

void VideoRendererSDL::cleanUp()
{
    uint32_t i;

    for(i = 0; i < ESPlayerConstants::VIDEO_REN_FRAME_BUFFERS; i ++)
    {
        if(overlayBuffers_[i] != NULL)
        {
            if(supportScaling_)
            {
                overlayBuffers_[i]->w = maxImageWidth_;
                overlayBuffers_[i]->h = maxImageHeight_;
            }

            SDL_FreeYUVOverlay(overlayBuffers_[i]);
        }
    }

    delete [] overlayBuffers_;
    delete dstRect_;
}

void VideoRendererSDL::setVideoWindow(Rect r)
{
    uint32_t dstRectX, dstRectY, dstRectWidth, dstRectHeight;

    if(r.width == 0)
        r.width = ESPlayerConstants::VIDEO_REN_SCREEN_WIDTH_4K;
    if(r.height == 0)
        r.height = ESPlayerConstants::VIDEO_REN_SCREEN_HEIGHT_4K;

    calcDstRectOnScreen(pixelAspectRatioX_,
                        pixelAspectRatioY_,
                        ESPlayerConstants::VIDEO_REN_SCREEN_PAR_X,
                        ESPlayerConstants::VIDEO_REN_SCREEN_PAR_Y,
                        imageWidth_,
                        imageHeight_,
                        r.width,
                        r.height,
                        dstRectX,
                        dstRectY,
                        dstRectWidth,
                        dstRectHeight);

    dstRect_->x = r.x + dstRectX;
    dstRect_->y = r.y + dstRectY;
    dstRect_->w = dstRectWidth;
    dstRect_->h = dstRectHeight;

    clearOnFlip_ = true;
}

