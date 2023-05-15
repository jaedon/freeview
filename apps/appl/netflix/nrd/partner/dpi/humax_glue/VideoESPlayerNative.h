/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#ifndef VIDEOESPLAYERNATIVE_H
#define VIDEOESPLAYERNATIVE_H

/** @file VideoESPlayerNative.h - Reference implementation of the video
 * IElementaryStreamPlayer.
 *
 * VideoESPlayerNative is the reference application's implementation of a video
 * player. It implements the IElementaryStreamPlayer interface (see
 * IElementaryStreamPlayer.h). A device partner may use or modify this header
 * and accompanying .cpp as needed.
 *
 * A VideoESPlayerNative is created by the
 * PlaybackGroupNative::createStreamPlayer() method each time a movie is loaded
 * for viewing and persists until the movie is unloaded. When it's created, the
 * player spawns a decoder thread and a renderer thread. The decoder thread uses
 * the IESPlayerCallback object the player receives when it is created to pull
 * video samples (also known as access units in H.264/AVC) from the netlix
 * application. When it receives a sample, it decrypts it as needed, then
 * decodes the frame and adds it to the renderer's input queue. The renderer
 * thread uses the PlaybackGroupNative's ReferenceClock and the PTS timestamps
 * on the decoded frames to determine when to remove decoded frames from its
 * input queue and write them to the video driver.
 *
 * In the reference application, there there are several different options for
 * the video renderer that is used. The VideoESPlayer uses the ESManager (which
 * is available via the PlaybackGroupNative) to get the RendererManager that
 * provides the video renderer that was selected at run time.
 */

#include "ESPlayerNative.h"
#include "SampleWriterNative.h"


namespace netflix {
namespace device {
namespace esplayer {

class VideoDecoder;
class DeviceThread;
class AVStatusManager;

class VideoESPlayerNative :  public ESPlayerNative
{
public:

    virtual ~VideoESPlayerNative();

    virtual NFErr init(const struct StreamPlayerInitData& initData,
                        shared_ptr<IESPlayerCallback> callback,
                        PlaybackGroupNative* playbackGroup);
    virtual void flush();
    virtual void close();
    virtual void decoderTask();
    virtual void rendererTask();
    virtual bool inputsAreExhausted();
    virtual MediaType getMediaType();
    /*
     * This function is polled by the upper layer, return true when the decoder
     * has enough buffer to start without an underflow.
     */
    virtual bool readyForPlaybackStart();
    virtual IPlaybackGroup::Rect setVideoWindow(const IPlaybackGroup::Rect& targetRect,
                                                uint32_t transitionDuration);

    double getLastCurrentVideoPts();
    void setLastCurrentVideoPts(double dblCurPts);

    static VideoESPlayerNative* getVideoPlayer() { return s_hVideoPlayer; }
    
    static void MutexLock()
    {
        int32_t nRet = 0;
    
        nRet = pthread_mutex_lock(&s_mtxVideoPlayer);
        if(nRet)
        {
            NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [%s:%d] Failed to lock s_mtxVideoPlayer, nRet = %d\n", __func__, __LINE__, nRet);
        }
    }
    
    static void MutexUnlock()
    {
        int32_t nRet = 0;
        
        nRet = pthread_mutex_unlock(&s_mtxVideoPlayer);
        if(nRet)
        {
            NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [%s:%d] Failed to unlock s_mtxVideoPlayer, nRet = %d\n", __func__, __LINE__, nRet);
        }
    }

private:
    friend class PlaybackGroupNative;

    VideoESPlayerNative(unsigned int session_id);

    virtual void beginFlush();
    virtual void endFlush();

    void updateTransition();
    void log3DType(Format3D format3D);
    void handleFrameDropReporting(const int64_t& refTimestamp,
                                  const int64_t& picTimestamp);

    bool setSourceBuffer(void *src_buf);
    void *getSourceBuffer();
    bool getVideoCodecFromAttributes(const ISampleAttributes *pSampleAttr, int32_t *pnCsoVideoCodec);

    shared_ptr<DeviceThread>          mVideoDecoderThread;
    shared_ptr<DeviceThread>          mVideoRendererThread;
    shared_ptr<VideoDecoder>          mVideoDecoder;
    shared_ptr<SampleWriterNative>    mSampleWriter;
    Format3D                          mCurrent3DFormat;

    // Video window transition information
    int64_t mTransitionStartTime;
    int64_t mTransitionEndTime;
    IPlaybackGroup::Rect mSrcVideoWindow;
    IPlaybackGroup::Rect mDstVideoWindow;

    // Location of the window right now (updated during transitions)
    IPlaybackGroup::Rect mCurrentVideoWindow;

    // Keep track of timestamps and rendering times so we can log when the
    // renderer drops a frame.
    int64_t mPrevPicTimestamp;
    int64_t mPrevRefTimestamp;

    int64_t mTsOfLastFrameDeliveredToRenderer;

    unsigned int         m_session_id;
    void                 *m_source_buffer;
    UVAWriteFrame_t     m_stUVAWriteFrame;
    double                m_dblLastCurVideoPts;

    static VideoESPlayerNative     *s_hVideoPlayer;
    static pthread_mutex_t         s_mtxVideoPlayer;
};

} // esplayer
} // namespace device
} // namespace native

#endif
