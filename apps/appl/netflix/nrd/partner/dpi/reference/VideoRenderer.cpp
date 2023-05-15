/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "VideoRenderer.h"
#include <nrddpi/Common.h>
#include "ESPlayerConstants.h"
#include <nrdbase/Log.h>
#include <nrdbase/ScopedMutex.h>
#include <nrdbase/MutexRanks.h>

//#define TIMESTAMP_DEBUG 1

using namespace netflix;
using namespace netflix::device::esplayer;

VideoRenderer::VideoRenderer(
        uint32_t maxImageWidth, uint32_t maxImageHeight)

    : maxImageWidth_(maxImageWidth), maxImageHeight_(maxImageHeight),
      mPipelineId(0),
      numFrameBuffers_(0), pixelFormat_(ESPlayerConstants::PIXEL_FORMAT_NONE),
      wp_(0), rp_(0), data_(NULL),
      flushState_(ESPlayerNative::FLUSH_OFF), inputExhausted_(true),
      mutex_(ESP_RENDERER_MUTEX, "Video Renderer Mutex")
{
}

VideoRenderer::VideoRenderer(uint32_t maxImageWidth,
                             uint32_t maxImageHeight,
                             uint32_t pipelineId)
  : maxImageWidth_(maxImageWidth), maxImageHeight_(maxImageHeight),
    mPipelineId(pipelineId),
    numFrameBuffers_(0), pixelFormat_(ESPlayerConstants::PIXEL_FORMAT_NONE),
    wp_(0), rp_(0), data_(NULL),
    flushState_(ESPlayerNative::FLUSH_OFF), inputExhausted_(true),
    mutex_(ESP_RENDERER_MUTEX, "Video Renderer Mutex")
{
}

VideoRenderer::~VideoRenderer()
{
    if(data_ != NULL)
        delete [] data_;
}

void VideoRenderer::getInfo(VideoRendererInfo& info) const
{
    info.outputPixelFormat_ = pixelFormat_;
    info.maxWidth_          = maxImageWidth_;
    info.maxHeight_         = maxImageHeight_;
    info.supportScaling_    = supportScaling_;
}

NFErr VideoRenderer::lockFrameBuffer(VideoFrameBuffer& frameBuffer)
{
    ScopedMutex cs(mutex_);

    assert(pixelFormat_ != ESPlayerConstants::PIXEL_FORMAT_NONE); // init() should have been called by child class.

    if(frameBuffer.locked_) {
        NTRACE(TRACE_MEDIAPLAYBACK, "[%d] VideoRenderer::lockFrameBuffer: attempt to lock "
               "with an already locked frame buffer", mPipelineId);
        return NFErr_Bad;
    }

    if(!lockBackBuffer(wp_, frameBuffer.priv_,
                            frameBuffer.planes_[0],
                            frameBuffer.planes_[1],
                            frameBuffer.planes_[2],
                            frameBuffer.planes_[3],
                            frameBuffer.pitches_[0],
                            frameBuffer.pitches_[1],
                            frameBuffer.pitches_[2],
                            frameBuffer.pitches_[3]))
    {
        NTRACE(TRACE_MEDIAPLAYBACK, "[%d] cannot lock back-buffer in video renderer engine", mPipelineId);
        return NFErr_Bad;
    }

    frameBuffer.locked_     = true;
    frameBuffer.gotPicture_ = false;
    frameBuffer.timestamp_  = INVALID_TIMESTAMP;
    frameBuffer.width_      = 0;
    frameBuffer.height_     = 0;
    frameBuffer.index_      = wp_;

    return NFErr_OK;
}

void VideoRenderer::unlockFrameBuffer(
        VideoFrameBuffer& frameBuffer, bool gotPicture)
{
    ScopedMutex cs(mutex_);
    assert(pixelFormat_ != ESPlayerConstants::PIXEL_FORMAT_NONE); // init() should have been called by child class.

    if(!frameBuffer.locked_) {
        NTRACE(TRACE_MEDIAPLAYBACK, "[%d] attempt to unlock a frame buffer that is not locked\n", mPipelineId);
        return;
    }

    if(frameBuffer.index_ != wp_) {
        NTRACE(TRACE_MEDIAPLAYBACK, "[%d] attempt to unlock a frame buffer with bad index\n", mPipelineId);
        return;
    }

    if(!unlockBackBuffer(wp_, frameBuffer.priv_)) {
        NTRACE(TRACE_MEDIAPLAYBACK, "[%d] cannot unlock back-buffer in video renderer engine\n", mPipelineId);
        return;
    }

    frameBuffer.locked_     = false;
    frameBuffer.gotPicture_ = gotPicture;
}

bool VideoRenderer::sendFrameBuffer(VideoFrameBuffer& frameBuffer)
{
    ScopedMutex cs(mutex_);
    assert(pixelFormat_ != ESPlayerConstants::PIXEL_FORMAT_NONE); // init() should have been called by child class.

    if(frameBuffer.locked_) {
        NTRACE(TRACE_MEDIAPLAYBACK, "[%d] attempt to send a locked frame buffer\n", mPipelineId);
        return false;
    }

    if(!frameBuffer.gotPicture_) {
        NTRACE(TRACE_MEDIAPLAYBACK, "[%d] attempt to send a frame buffer with no picture\n", mPipelineId);
        return false;
    }

    if(frameBuffer.index_ != wp_) {
        NTRACE(TRACE_MEDIAPLAYBACK, "[%d] attempt to send a frame buffer with bad index\n", mPipelineId);
        return false;
    }

    uint32_t next = wp_ + 1;
    if(next >= numFrameBuffers_)
        next = 0;

#if defined(BUILD_DEBUG) && defined(TIMESTAMP_DEBUG)
    if(next == rp_){
        int wp = (wp_ > 0) ? wp_ - 1 : numFrameBuffers_ - 1;
        NTRACE(TRACE_MEDIAPLAYBACK
               ,"[%d] %s frame buffers full: last decoded (%d) pts %lld, about to display (%d) pts %lld (diff %lld)"
               , mPipelineId
               , __func__
               , wp, data_[wp].timestamp_
               , rp_, data_[rp_].timestamp_
               , data_[wp].timestamp_ - data_[rp_].timestamp_ );
        return false; // frame-buffer queue is full
    }

    static int64_t prev;
    NTRACE(TRACE_MEDIAPLAYBACK, "[%d] %s, curr %lld, prev %lld, (diff %lld)"
           , mPipelineId, __func__, frameBuffer.timestamp_, prev, frameBuffer.timestamp_ - prev);
    prev = frameBuffer.timestamp_;
#else
    if(next == rp_)
        return false; // frame-buffer queue is full
#endif

    data_[wp_].timestamp_ = frameBuffer.timestamp_;
    data_[wp_].width_     = frameBuffer.width_;
    data_[wp_].height_    = frameBuffer.height_;
    data_[wp_].priv_      = frameBuffer.priv_;

    wp_ = next;

    frameBuffer.gotPicture_ = false;
    return true;
}

bool VideoRenderer::getNextDisplayTimestamp(int64_t& timestamp)
{
    ScopedMutex cs(mutex_);
    assert(pixelFormat_ !=ESPlayerConstants::PIXEL_FORMAT_NONE); // init() should have been called by child class.

    if(rp_ == wp_)
        return false; // frame-buffer queue is empty

    timestamp = data_[rp_].timestamp_;
    return true;
}

bool VideoRenderer::displayFrame()
{
    ScopedMutex cs(mutex_);
    assert(pixelFormat_ !=ESPlayerConstants::PIXEL_FORMAT_NONE); // init() should have been called by child class.

    if(rp_ == wp_)
        return false; // frame-buffer queue is empty

    if(!flipBackBuffer(rp_, data_[rp_].width_, data_[rp_].height_, data_[rp_].priv_)) {
        NTRACE(TRACE_MEDIAPLAYBACK, "[%d] cannot flip back-buffer in video renderer engine"
               "rp_ %d, width %d, height%d"
               , mPipelineId
               ,rp_, data_[rp_].width_, data_[rp_].height_);

        //do not return false when flipBackBuffer fails. Whether DFB operation
        //was successful or not, if DFB failure was transient, rendering should
        //continue by incrementing ring buffer display pointer

        //return false;
    }

    uint32_t next = rp_ + 1;
    if(next >= numFrameBuffers_)
        next = 0;
    rp_ = next;

    return true;
}

void VideoRenderer::flushFrameBufferQueue()
{
    ScopedMutex cs(mutex_);
    assert(pixelFormat_ !=ESPlayerConstants::PIXEL_FORMAT_NONE); // init() should have been called by child class.
    rp_ = wp_;
}

bool VideoRenderer::init()
{
    return true;
}

void VideoRenderer::init(
        uint32_t numFrameBuffers, uint32_t pixelFormat, bool supportScaling)
{
    assert(pixelFormat_ ==ESPlayerConstants::PIXEL_FORMAT_NONE); // init() can only be called once per object

    assert(numFrameBuffers >= ESPlayerConstants::VIDEO_REN_MIN_FRAME_BUFFERS &&
           pixelFormat !=ESPlayerConstants::PIXEL_FORMAT_NONE);

    numFrameBuffers_ = numFrameBuffers;
    pixelFormat_     = pixelFormat;
    supportScaling_  = supportScaling;
    data_            = new FrameBufferQueueItem[numFrameBuffers];
}

void VideoRenderer::calcDstRectOnScreen(
        uint32_t  videoPixelAspectRatioX,
        uint32_t  videoPixelAspectRatioY,
        uint32_t  screenPixelAspectRatioX,
        uint32_t  screenPixelAspectRatioY,
        uint32_t  videoWidth,
        uint32_t  videoHeight,
        uint32_t  screenWidth,
        uint32_t  screenHeight,
        uint32_t& dstRectX,
        uint32_t& dstRectY,
        uint32_t& dstRectWidth,
        uint32_t& dstRectHeight)
{
    if(videoPixelAspectRatioX == 0 || videoPixelAspectRatioY == 0 ||
       screenPixelAspectRatioX == 0 || screenPixelAspectRatioY == 0 ||
       videoWidth == 0 || videoHeight == 0 || screenWidth == 0 || screenHeight == 0)
    {
        fprintf(stderr, "dimension/pixel-aspect-ratio parameters should be non-zero");
        dstRectX = dstRectY = dstRectWidth = dstRectHeight = 0;
        return;
    }

    //                  SCREEN_PAR_Y                        PICTURE_PAR_Y
    // SCREEN_HEIGHT * --------------  =  PICTURE_HEIGHT * ---------------
    //                  SCREEN_PAR_X                        PICTURE_PAR_X

    uint32_t targetScreenHeight = (videoHeight * videoPixelAspectRatioY * screenPixelAspectRatioX)
                                    / (videoPixelAspectRatioX * screenPixelAspectRatioY);

    // Try scale video to match the screen width.
    // If the scaled video height can be contained by the screen, accept it.

    dstRectWidth = screenWidth;
    dstRectHeight = targetScreenHeight * screenWidth / videoWidth;

    if(dstRectHeight <= screenHeight)
    {
        dstRectX = 0;
        dstRectY = ((screenHeight - dstRectHeight) >> 1);
        return;
    }

    // Try scale video to match the screen height.
    // If the scaled video width can be contained by the screen, accept it.

    dstRectHeight = screenHeight;
    dstRectWidth = videoWidth * screenHeight / targetScreenHeight;

    if(dstRectWidth <= screenWidth)
    {
        dstRectY = 0;
        dstRectX = ((screenWidth - dstRectWidth) >> 1);
        return;
    }

    assert(false); // it should never get here
}

void VideoRenderer::setVideoWindow(Rect /*r*/)
{
}

void VideoRenderer::setFlushState(ESPlayerNative::FlushState flushState)
{
    ScopedMutex cs(mutex_);
    flushState_ = flushState;
}

ESPlayerNative::FlushState VideoRenderer::getFlushState()
{
    ScopedMutex cs(mutex_);
    return flushState_;
}
