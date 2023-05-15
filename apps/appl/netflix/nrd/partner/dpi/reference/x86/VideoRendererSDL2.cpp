/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "VideoRendererSDL2.h"
#include <nrdbase/Exportable.h>

// SDL headers
#include <SDL2/SDL.h>
#include <nrddpi/Common.h>

using namespace netflix::device::esplayer;
using namespace netflix;


VideoRenderer *createVideoRenderer(const VideoAttributes& attributes,
                                   uint32_t maxImageWidth,
                                   uint32_t maxImageHeight)
{
    NRDP_UNUSED(maxImageWidth);
    NRDP_UNUSED(maxImageHeight);
    return new VideoRendererSDL2(attributes,
                                 ESPlayerConstants::VIDEO_REN_SCREEN_WIDTH_4K,
                                 ESPlayerConstants::VIDEO_REN_SCREEN_HEIGHT_4K);
}

SDL_Surface* VideoRendererSDL2::primarySurface_ = NULL;
const bool   VideoRendererSDL2::supportScaling_ = false;

VideoRendererSDL2::VideoRendererSDL2(const VideoAttributes& attributes,
                                     uint32_t maxImageWidth,
                                     uint32_t maxImageHeight)

    : VideoRenderer(maxImageWidth, maxImageHeight),
      renderBuffers_(new uint8_t*[ESPlayerConstants::VIDEO_REN_FRAME_BUFFERS]),
      window_(NULL),
      renderer_(NULL)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "Can't initialize SDL\n");
        std::string msg(SDL_GetError());
        printf("%s\n", msg.c_str());

        return;
    }

    window_ =  SDL_CreateWindow("An SDL2 window",
                                SDL_WINDOWPOS_UNDEFINED,
                                SDL_WINDOWPOS_UNDEFINED,
                                ESPlayerConstants::VIDEO_REN_SCREEN_WIDTH_4K,
                                ESPlayerConstants::VIDEO_REN_SCREEN_HEIGHT_4K,
                                0);
    if(!window_){
        fprintf(stderr, "Can't create Window\n");
        std::string msg(SDL_GetError());
        printf("%s\n", msg.c_str());
        return;
    }

    //renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_PRESENTVSYNC|SDL_RENDERER_ACCELERATED);
    renderer_ = SDL_CreateRenderer(window_, -1, 0);
    if(!renderer_){
        fprintf(stderr, "Can't create Renderer\n");
        std::string msg(SDL_GetError());
        printf("%s\n", msg.c_str());
        return;
    }

    // Create texture
    videoTexture_ = SDL_CreateTexture(renderer_,
                                      SDL_PIXELFORMAT_YV12,
                                      SDL_TEXTUREACCESS_STREAMING,
                                      ESPlayerConstants::VIDEO_REN_SCREEN_WIDTH_4K,
                                      ESPlayerConstants::VIDEO_REN_SCREEN_HEIGHT_4K);
    if(videoTexture_ == NULL)
    {
        fprintf(stderr, "failed to create minimum frame buffers via SDL_CreateTexture");
        std::string msg(SDL_GetError());
        printf("%s\n", msg.c_str());
        cleanUp();
        return;

    }

    // allocate render buffer
    // render buffer size for YUV420
    uint32_t buffer_size
        = ESPlayerConstants::VIDEO_REN_SCREEN_WIDTH_4K * ESPlayerConstants::VIDEO_REN_SCREEN_HEIGHT_4K * 3 / 2;
    for(uint32_t i = 0; i < ESPlayerConstants::VIDEO_REN_FRAME_BUFFERS; i ++){
        posix_memalign((void**)&renderBuffers_[i], 4096, buffer_size);
    }

    pixelAspectRatioX_ = attributes.mPixelAspectRatioX;
    pixelAspectRatioY_ = attributes.mPixelAspectRatioY;
    imageWidth_  = ESPlayerConstants::VIDEO_REN_SCREEN_WIDTH_4K;//attributes.mImageWidth;
    imageHeight_ = ESPlayerConstants::VIDEO_REN_SCREEN_HEIGHT_4K;//attributes.mImageHeight;
    uint32_t pixelFormat = _FOURCC_LE_('Y','V','1','2');
    init(ESPlayerConstants::VIDEO_REN_FRAME_BUFFERS, pixelFormat, supportScaling_); // finish initialization
}

VideoRendererSDL2::~VideoRendererSDL2()
{
    cleanUp();
    SDL_DestroyTexture(videoTexture_);
    SDL_DestroyRenderer(renderer_);
    SDL_DestroyWindow(window_);
    SDL_Quit();
}

bool VideoRendererSDL2::lockBackBuffer(
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

    uint32_t UV_size
        = ESPlayerConstants::VIDEO_REN_SCREEN_WIDTH_4K * ESPlayerConstants::VIDEO_REN_SCREEN_HEIGHT_4K / 4;


    SDL_LockTexture(videoTexture_, NULL, (void**)&renderBuffers_[backBufferIndex], (int*)&pitch0 );

    priv = NULL; // not used

    plane0 = (unsigned char*)(renderBuffers_[backBufferIndex]);
    plane1 = (unsigned char*)(renderBuffers_[backBufferIndex] + UV_size*4);
    plane2 = (unsigned char*)(renderBuffers_[backBufferIndex] + UV_size*5);
    plane3 = NULL;

    pitch0 = ESPlayerConstants::VIDEO_REN_SCREEN_WIDTH_4K;
    pitch1 = ESPlayerConstants::VIDEO_REN_SCREEN_WIDTH_4K / 2;
    pitch2 = ESPlayerConstants::VIDEO_REN_SCREEN_WIDTH_4K / 2;
    pitch3 = 0;

    return true;
}

bool VideoRendererSDL2::unlockBackBuffer(
    uint32_t backBufferIndex, void* /*priv*/)
{
    assert(backBufferIndex < getNumFrameBuffers());
    SDL_UnlockTexture(videoTexture_);
    return true;
}

bool VideoRendererSDL2::flipBackBuffer(
    uint32_t backBufferIndex,
    uint32_t width,
    uint32_t height,
    void* /*priv*/)
{
    assert(backBufferIndex < getNumFrameBuffers());

    SDL_UpdateTexture(videoTexture_, NULL, renderBuffers_[backBufferIndex], 0);
    SDL_RenderClear(renderer_);
    SDL_RenderCopy(renderer_, videoTexture_, NULL, NULL);
    SDL_RenderPresent(renderer_);

    return true;
}

void VideoRendererSDL2::cleanUp()
{
    uint32_t i;

    for(i = 0; i < ESPlayerConstants::VIDEO_REN_FRAME_BUFFERS; i ++)
    {
        if(renderBuffers_[i] != NULL)
        {
            delete renderBuffers_[i];
        }
    }
    delete [] renderBuffers_;

}

