/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef DEVICE_VIDEORENDERER_H
#define DEVICE_VIDEORENDERER_H

/* @file VideoRenderer.h - Video renderer base class used in the reference
 * application's video elementary stream player implementation. The file does
 * not implement a DPI interface. It provides reference-implementation-specific
 * video rendering functionality. The reference application provides several
 * video renderers that each implement the video renderer interface defined
 * here.
 */

#include "VideoESPlayerNative.h"
#include <nrdbase/Exportable.h>
#include <nrdbase/Mutex.h>

namespace netflix {
namespace device {
namespace esplayer {

/**
 * @class VideoRenderer VideoRenderer.h
 * @brief Base implementation for video renderer engines.
 */
class NRDP_EXPORTABLE VideoRenderer
{
protected:
    friend class RendererManager;
    /**
     * Constructor.
     *
     * @param[in] maxImageWidth the maximum width (in pixels) of all possible images.
     * @param[in] maxImageHeight the maximum height (in pixels) of all possible images.
     */
    VideoRenderer(uint32_t maxImageWidth, uint32_t maxImageHeight);
    VideoRenderer(uint32_t maxImageWidth, uint32_t maxImageHeight, uint32_t pipelineId);

    /**
     * Destructor.
     */
    virtual ~VideoRenderer();

public:

    /**
     * @brief Video Frame Buffer
     */
    class VideoFrameBuffer
    {
      friend class VideoRenderer;

    public:
        VideoFrameBuffer() : locked_(false), gotPicture_(false) {}

        void discardPicture()                { gotPicture_ = false;                          }
        bool gotPicture()                    { return gotPicture_;                           }
        unsigned char* plane(uint32_t i)     { assert(i < 4 && locked_); return planes_[i];  }
        uint32_t pitch(uint32_t i)           { assert(i < 4 && locked_); return pitches_[i]; }
        void setTimestamp(int64_t timestamp) { assert(locked_); timestamp_ = timestamp;      }
        void setWidth(uint32_t width)        { assert(locked_); width_ = width;              }
        void setHeight(uint32_t height)      { assert(locked_); height_ = height;            }
        int64_t& getTimestamp()              { return timestamp_; }

    private:
        bool           locked_;
        bool           gotPicture_;
        unsigned char* planes_[4];
        uint32_t       pitches_[4];
        int64_t        timestamp_;
        uint32_t       width_;
        uint32_t       height_;
        uint32_t       index_;
        void*          priv_;
    };


      /**
     * @brief The video renderer information
     */
    struct VideoRendererInfo
    {
        uint32_t outputPixelFormat_; /** output pixel format (FourCC code) */
        uint32_t maxWidth_;          /** maximum width of renderer frame buffer in pixels */
        uint32_t maxHeight_;         /** maximum height of renderer frame buffer in pixels */
        bool     supportScaling_;    /** true if image scaling is supported */
    };

    typedef IPlaybackGroup::Rect Rect;


    /**
     * Return the number of frame buffers allocated.
     *
     * @return number of frame buffers allocated.
     */
    uint32_t getNumFrameBuffers() const { return numFrameBuffers_; }

    /**
     * Get information of the video renderer.
     *
     * @param[out] info to receive the video renderer information.
     */
    void getInfo(VideoRendererInfo& info) const;

    /**
     * Lock a frame buffer from the video renderer.
     *
     * This is the only method for the caller to get access to the memory in frame buffers.
     * The caller should unlock the locked frame buffer as soon as data writing is finished.
     *
     * @param[out] frameBuffer to receive the locked frame buffer.
     */
    NFErr lockFrameBuffer(VideoFrameBuffer& frameBuffer);

    /**
     * Unlock a frame buffer (that we previously locked). After unlock, the memory in
     * frame buffer is no longer accessible.
     *
     * @param[in] frameBuffer the frame buffer to be unlocked.
     * @param[in] gotPicture true if we have stored picture successfully in the frame buffer.
     */
    void unlockFrameBuffer(VideoFrameBuffer& frameBuffer, bool gotPicture);

    /**
     * Send a frame buffer (that we previously locked and store picture in it) into the
     * frame-buffer queue, so that the display task can "flip" (display) it later.
     * A frame buffer should be unlocked first before being sent. Sending a frame buffer
     * with no picture stored will trigger an exception.
     *
     * @param[in] frameBuffer the frame buffer to be sent into queue.
     *
     * @return true if successful, false if the frame-buffer queue in video renderer is full.
     */
    bool sendFrameBuffer(VideoFrameBuffer& frameBuffer);

    /**
     * Get the timestamp which specifies the time to display the next video frame.
     *
     * @param[out] timestamp the timestamp.
     *
     * @return true if successful, false if otherwise (frame-buffer queue is empty).
     */
    bool getNextDisplayTimestamp(int64_t& timestamp);

    /**
     * Display the next video frame from the frame-buffer queue. After being displayed,
     * the same video frame is removed from queue.
     *
     * @return true if the displayed frame is updated, false if otherwise (frame-buffer queue is empty).
     */
    bool displayFrame();

    /** Remove all video frames from the frame-buffer queue. (should be called by the receiver side) */
    void flushFrameBufferQueue();

    /**
     * Set the state of flush operation (inactive/has-begun/has-ended).
     *
     * @param[in] flushState the new flush operation state to be set.
     */
    void setFlushState(ESPlayerNative::FlushState flushState);

    /**
     * Get the state of flush operation (inactive/has-begun/has-ended).
     *
     * @return the current flush operation state.
     */
    ESPlayerNative::FlushState getFlushState();

    /**
     * Set or reset the state of input data exhaustion.
     *
     * @param[in] inputExhausted true to set the exhaustion state; false to reset.
     */
    void setInputExhausted(bool inputExhausted) { inputExhausted_ = inputExhausted; }

    /**
     * Check whether the input data is exhausted.
     *
     * @return true if the input data is exhausted; false otherwise.
     */
    bool isInputExhausted() { return inputExhausted_; }

    /**
     * Immediately changes location of video output. Might be called many times while
     * a video is being moved with animation.
     */
    virtual void setVideoWindow(Rect r);

    virtual void threadStart() { }
    virtual void threadEnd() { }

protected:
    /**
     * Finish initialization of the video renderer (Child class should call this function to
     * finish the renderer initialization procedure).
     *
     * @param[in] numFrameBuffers number of frame buffers allocated.
     * @param[in] pixelFormat the pixel format (FourCC code) of video renderer.
     * @param[in] supportScaling true if image scaling is supported.
     */
    void init(uint32_t numFrameBuffers,
              uint32_t pixelFormat,
              bool     supportScaling);


    /**
     * Renderer specific initialization
     */
    virtual bool init() = 0;

    /**
     * Lock one specific back-buffer surface, and get its data plane pointers and pitch values.
     *
     * @param[in] backBufferIndex the zero-based index of the back-buffer surface to be locked.
     * @param[out] priv a typeless (void*) pointer that's for video renderer engine's private use.
     * @param[out] plane0 1st data plane pointer of the back-buffer surface.
     * @param[out] plane1 2nd data plane pointer of the back-buffer surface.
     * @param[out] plane2 3rd data plane pointer of the back-buffer surface.
     * @param[out] plane3 4th data plane pointer of the back-buffer surface.
     * @param[out] pitch0 1st data plane pitch of the back-buffer surface.
     * @param[out] pitch1 2nd data plane pitch of the back-buffer surface.
     * @param[out] pitch2 3rd data plane pitch of the back-buffer surface.
     * @param[out] pitch3 4th data plane pitch of the back-buffer surface.
     *
     * @return true if succeeded, false if otherwise.
     */
    virtual bool lockBackBuffer(uint32_t       backBufferIndex,
                                void*          &priv,
                                unsigned char* &plane0,
                                unsigned char* &plane1,
                                unsigned char* &plane2,
                                unsigned char* &plane3,
                                uint32_t       &pitch0,
                                uint32_t       &pitch1,
                                uint32_t       &pitch2,
                                uint32_t       &pitch3) = 0;

    /**
     * Unlock one specific back-buffer surface (when writing is done), so later it could be flipped.
     *
     * @param[in] backBufferIndex the zero-based index of the back-buffer surface to be unlocked.
     * @param[in] priv a typeless (void*) pointer that's for video renderer engine's private use.
     *
     * @return true if succeeded, false if otherwise.
     */
    virtual bool unlockBackBuffer(uint32_t backBufferIndex, void* priv) = 0;

    /**
     * Flip one specific back-buffer surface (when it's time to display it) to the front.
     *
     * @param[in] backBufferIndex the zero-based index of the back-buffer surface to be flipped.
     * @param[in] width width of the decoded picture in number of pixels.
     * @param[in] height height of the decoded picture in number of pixels.
     * @param[in] priv a typeless (void*) pointer that's for video renderer engine's private use.
     *
     * @return true if succeeded, false if otherwise.
     */
     virtual bool flipBackBuffer(uint32_t backBufferIndex,
                                uint32_t width,
                                uint32_t height,
                                void* priv) = 0;

public: // helpers

    /**
     * Calculate the destination rectangle for rendering video based on the video
     * dimension/pixel-aspect-ratio and display screen dimension/pixel-aspect-ratio.
     *
     * The algorithm always try to stretch the video to fill up the most area of
     * screen, while keeping the aspect-ratio correct. If there is space not taken
     * by video on the top/bottom of screen, a pair of equal height letter-boxes
     * will fill the space on top and bottom of screen. If there is space not taken
     * by video on the left/right of screen, a pair of equal width pillar-boxes will
     * fill the space on left and right of screen.
     *
     * @param[in] videoPixelAspectRatioX X component (width) of the video pixel aspect ratio.
     * @param[in] videoPixelAspectRatioY Y component (height) of the video pixel aspect ratio.
     * @param[in] screenPixelAspectRatioX X component (width) of the screen pixel aspect ratio.
     * @param[in] screenPixelAspectRatioY Y component (height) of the screen pixel aspect ratio.
     * @param[in] videoWidth width of the video pictures.
     * @param[in] videoHeight height of the video pictures.
     * @param[in] screenWidth width of the display screen.
     * @param[in] screenHeight height of the display screen.
     * @param[out] dstRectX X coordinate of the top-left corner of destination rectangle on screen.
     * @param[out] dstRectY Y coordinate of the top-left corner of destination rectangle on screen.
     * @param[out] dstRectWidth width of the destination rectangle on screen.
     * @param[out] dstRectHeight height of the destination rectangle on screen.
     */
     static void calcDstRectOnScreen(uint32_t  videoPixelAspectRatioX,
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
                                     uint32_t& dstRectHeight);

protected:
    uint32_t maxImageWidth_;
    uint32_t maxImageHeight_;
    uint32_t mPipelineId;

private: // for frame-buffer queue handling

    struct FrameBufferQueueItem
    {
        int64_t  timestamp_;
        uint32_t width_;
        uint32_t height_;
        void*    priv_;
    };

    uint32_t numFrameBuffers_;
    uint32_t pixelFormat_;

    // Accessed from the decoder and renderer threads.
    uint32_t volatile wp_;
    uint32_t volatile rp_;
    FrameBufferQueueItem* data_;

    // Accessed from the SDK thread and the renderer threads.
    ESPlayerNative::FlushState flushState_;

    bool inputExhausted_; // only accessed from the rendererTask.
    bool supportScaling_; // only accessed by the SDK thread.

    Mutex    mutex_;
};

} // namespace esplayer
} // namespace device
} // namespace netflix

#endif // DEVICE_VIDEORENDERER_H
