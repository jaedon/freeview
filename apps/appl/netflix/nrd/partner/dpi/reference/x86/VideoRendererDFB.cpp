/*
 * (c) 1997-2015 Netflix, Inc. All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "VideoRendererDFB.h"
#include "../ESPlayerConstants.h"
#include <nrddpi/Common.h>
#include <nrdbase/MutexRanks.h>
#include <nrdbase/Exportable.h>
#include <nrdbase/ScopedMutex.h>
#include <nrdbase/Log.h>

//#define YUV_RENDER 1

#define DFB_CALL(op, onFailure)                                         \
    {                                                                   \
        const DFBResult result = op;                                    \
        if (result != DFB_OK) {                                         \
            NTRACE(TRACE_MEDIAPLAYBACK, "VideoRendererDFB: %s => %s\n", \
                   #op, DirectFBErrorString(result));                   \
            onFailure;                                                  \
        }                                                               \
    }

using namespace netflix;
using namespace netflix::device::esplayer;

typedef bool (*DFBFlipFunction)(void *data, IDirectFBSurface *buffer, int srcWidth, int srcHeight, int dstWidth, int dstHeight);

static void *sFlipFunctionData = 0;
static DFBFlipFunction sFlipFunction = 0;
static Mutex sFlipFunctionMutex(ZERO_MUTEX, "VideoRendererDFB::FlipFunction");
void setVideoRendererDFBFlipFunction(DFBFlipFunction func, void *data)
{
    ScopedMutex _lock(sFlipFunctionMutex);
    sFlipFunction = func;
    sFlipFunctionData = data;
}

VideoRendererDFB::VideoRendererDFB(const VideoAttributes& attributes,
                                   uint32_t maxImageWidth,
                                   uint32_t maxImageHeight,
                                   uint32_t pipelineId)
  : VideoRenderer(maxImageWidth, maxImageHeight, pipelineId)
    , mDfb(0)
    , mLayer(0)
    , mLayerWidth(-1)
    , mLayerHeight(-1)
    , mWindow(0)
    , mWindowSurface(0)
    , mBuffers(new IDirectFBSurface*[ESPlayerConstants::VIDEO_REN_FRAME_BUFFERS])
    , mAttributes(attributes)
    , mClearOnFlip(false)
    , mMaxImageWidth(maxImageWidth)
    , mMaxImageHeight(maxImageHeight)
    , mLastFramePresented(-1)
{
}

VideoRendererDFB::~VideoRendererDFB()
{
    for (uint32_t i = 0; i < ESPlayerConstants::VIDEO_REN_FRAME_BUFFERS; ++i) {
        mBuffers[i]->Unlock(mBuffers[i]);
        mBuffers[i]->Release(mBuffers[i]);
    }

    delete [] mBuffers;

    if (mWindowSurface)
        mWindowSurface->Release(mWindowSurface);
    if (mWindow)
        mWindow->Release(mWindow);
    if (mLayer)
        mLayer->Release(mLayer);
    if (mDfb)
        mDfb->Release(mDfb);
}

bool VideoRendererDFB::init()
{
    int argc = 0;
    char **argv = 0;
    DFB_CALL(DirectFBInit(&argc, &argv), return false); // what if we're already init'ed?
    DFB_CALL(DirectFBCreate(&mDfb), return false);

    DFB_CALL(mDfb->GetDisplayLayer(mDfb, DLID_PRIMARY, &mLayer), return false);
    IDirectFBSurface *layerSurface = 0;
    DFB_CALL(mLayer->SetCooperativeLevel(mLayer, DLSCL_ADMINISTRATIVE), return false);
    DFB_CALL(mLayer->GetSurface(mLayer, &layerSurface), return false);
    DFB_CALL(layerSurface->GetSize(layerSurface, &mLayerWidth, &mLayerHeight), return false);
    layerSurface->Release(layerSurface);
    DFB_CALL(mLayer->SetCooperativeLevel(mLayer, DLSCL_SHARED), return false);

    DFBWindowDescription description;
    memset(&description, 0, sizeof(description));
    description.flags = static_cast<DFBWindowDescriptionFlags>(DWDESC_CAPS|DWDESC_PIXELFORMAT);
    description.caps = DWCAPS_NODECORATION;
#if defined(YUV_RENDER)
    description.pixelformat = DSPF_I420;
#else
    description.pixelformat = DSPF_RGB32;
#endif
    DFB_CALL(mLayer->CreateWindow(mLayer, &description, &mWindow), return false);
    DFB_CALL(mWindow->LowerToBottom(mWindow), return false);
    DFB_CALL(mWindow->SetOpacity(mWindow, 255), return false);

    const Rect r = { 0, 0, static_cast<uint32_t>(mLayerWidth), static_cast<uint32_t>(mLayerHeight) };
    setVideoWindow(r);

    DFBSurfaceDescription surfaceDescription;
    memset(&surfaceDescription, 0, sizeof(DFBSurfaceDescription));
    surfaceDescription.flags = static_cast<DFBSurfaceDescriptionFlags>(DSDESC_PIXELFORMAT|DSDESC_WIDTH|DSDESC_HEIGHT);
    surfaceDescription.width = mMaxImageWidth;
    surfaceDescription.height = mMaxImageHeight;
#if defined(YUV_RENDER)
    surfaceDescription.pixelformat = DSPF_I420;
#else
    surfaceDescription.pixelformat = DSPF_RGB32;
#endif
    uint32_t i;
    for (i = 0; i < ESPlayerConstants::VIDEO_REN_FRAME_BUFFERS; ++i)
        DFB_CALL(mDfb->CreateSurface(mDfb, &surfaceDescription, &mBuffers[i]), return false);
#if defined(YUV_RENDER)
    VideoRenderer::init(i, _FOURCC_LE_('Y','V','1','2'), true);
#else
    VideoRenderer::init(i, _FOURCC_LE_('R','G','B',' '), true);
#endif

    return true;
}

bool VideoRendererDFB::lockBackBuffer(uint32_t backBufferIndex,
                                      void* &,
                                      unsigned char* &plane0,
                                      unsigned char* &/*plane1*/,
                                      unsigned char* &/*plane2*/,
                                      unsigned char* &/*plane3*/,
                                      uint32_t &pitch0,
                                      uint32_t &/*pitch1*/,
                                      uint32_t &/*pitch2*/,
                                      uint32_t &/*pitch3*/)
{
    assert(backBufferIndex < getNumFrameBuffers());
    DFB_CALL(mBuffers[backBufferIndex]->Lock(mBuffers[backBufferIndex], DSLF_WRITE, &reinterpret_cast<void *&>(plane0), &reinterpret_cast<int&>(pitch0)), return false);
    return true;
}

bool VideoRendererDFB::unlockBackBuffer(uint32_t backBufferIndex, void* /*priv*/)
{
    assert(backBufferIndex < getNumFrameBuffers());
    DFB_CALL(mBuffers[backBufferIndex]->Unlock(mBuffers[backBufferIndex]), return false);
    return true;
}

bool VideoRendererDFB::flipBackBuffer(uint32_t backBufferIndex,
                                      uint32_t width,
                                      uint32_t height,
                                      void* /*priv*/)
{
    assert(backBufferIndex < getNumFrameBuffers());
    if (mClearOnFlip) {
        mWindowSurface->Clear(mWindowSurface, 0, 0, 0, 255);
        mClearOnFlip = false;
    }
    void *flipFunctionData = 0;
    DFBFlipFunction flipFunction = 0;
    {
        ScopedMutex _lock(sFlipFunctionMutex);
        flipFunctionData = sFlipFunctionData;
        flipFunction = sFlipFunction;
    }
    if(flipFunction)
        return flipFunction(flipFunctionData, mBuffers[backBufferIndex], width, height, mVideoWindow.width, mVideoWindow.height);

    /*
    printf("source width %d, height %d, target width %d, height %d\n",
           width, height, mVideoWindow.width, mVideoWindow.height);
    */

    // Netflix interface use uint32_t for width/height, but DirectFB use int
    DFBRectangle src;
    src.x = src.y = 0;
    src.w = width;
    src.h = height;

    DFBRectangle target;
    target.x = target.y = 0;
    target.w = mVideoWindow.width;
    target.h = mVideoWindow.height;
    DFB_CALL(mWindowSurface->SetBlittingFlags(mWindowSurface, DSBLIT_NOFX), return false);

    // Do not use Blit. UI can call setVideoWindow for resizing purpose, and
    // this resizing should be done at rendering time by renderer. VideoDecoder
    // does scaling, but that is for display size, and put it into renderer
    // queue. What will be resized is frame from renderer queue.
    DFB_CALL(mWindowSurface->StretchBlit(mWindowSurface, mBuffers[backBufferIndex], &src, &target), return false);
    DFB_CALL(mWindowSurface->Flip(mWindowSurface, 0, DSFLIP_ONSYNC), return false);

    mLastFramePresented = backBufferIndex;
    return true;
}

void VideoRendererDFB::setVideoWindow(Rect r)
{
    if (r.width == 0)
        r.width = ESPlayerConstants::VIDEO_REN_SCREEN_WIDTH;
    if (r.height == 0)
        r.height = ESPlayerConstants::VIDEO_REN_SCREEN_HEIGHT;

    uint32_t dstRectX, dstRectY, dstRectWidth, dstRectHeight;

    calcDstRectOnScreen(mAttributes.mPixelAspectRatioX,
                        mAttributes.mPixelAspectRatioY,
                        ESPlayerConstants::VIDEO_REN_SCREEN_PAR_X,
                        ESPlayerConstants::VIDEO_REN_SCREEN_PAR_Y,
                        mAttributes.mImageWidth,
                        mAttributes.mImageHeight,
                        r.width,
                        r.height,
                        dstRectX,
                        dstRectY,
                        dstRectWidth,
                        dstRectHeight);
    mVideoWindow.x = r.x + dstRectX;
    mVideoWindow.y = r.y + dstRectY;
    mVideoWindow.width = dstRectWidth;
    mVideoWindow.height = dstRectHeight;

    mClearOnFlip = true;
    if (mWindowSurface){
        mWindowSurface->Clear(mWindowSurface, 0, 0, 0, 255);
        mWindowSurface->Flip(mWindowSurface, 0, DSFLIP_BLIT);
        mWindowSurface->Release(mWindowSurface);
    }

    assert(mWindow);
    DFB_CALL(mWindow->SetBounds(mWindow, mVideoWindow.x, mVideoWindow.y, mVideoWindow.width, mVideoWindow.height), return);
    DFB_CALL(mWindow->GetSurface(mWindow, &mWindowSurface), return);

    assert(mWindowSurface);
    if(mWindowSurface){
        mWindowSurface->Clear(mWindowSurface, 0, 70, 0, 255);

        if (getNumFrameBuffers() && (mLastFramePresented > 0)) {
            DFBRectangle src;
            src.x = src.y = 0;
            src.w = ESPlayerConstants::VIDEO_REN_SCREEN_WIDTH;
            src.h = ESPlayerConstants::VIDEO_REN_SCREEN_HEIGHT;

            DFBRectangle target;
            target.x = target.y = 0;
            target.w = mVideoWindow.width;
            target.h = mVideoWindow.height;
            mWindowSurface->SetBlittingFlags(mWindowSurface, DSBLIT_NOFX);
            mWindowSurface->StretchBlit(mWindowSurface, mBuffers[mLastFramePresented], &src, NULL);
        }

        mWindowSurface->Flip(mWindowSurface, 0, DSFLIP_BLIT);
    }
}

