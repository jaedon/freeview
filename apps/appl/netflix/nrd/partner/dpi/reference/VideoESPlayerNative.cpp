/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#include <sstream>
#include "VideoESPlayerNative.h"
#include "ESManagerNative.h"
#include "DrmDecrypter.h"
#include "RendererManager.h"
#include "PlaybackGroupNative.h"
#include "ESPlayerConstants.h"
#include "DeviceThread.h"
#include "VideoDecoder.h"
#include "VideoRenderer.h"
#include "DecoderStatsCounter.h"
#include <nrdbase/Log.h>
#include <nrdbase/Time.h>
#include <nrdbase/Thread.h>
#include <nrd/AppThread.h>
#include <nrdbase/ScopedMutex.h>
#include <nrdbase/NFErr.h>

using namespace netflix;
using namespace netflix::device::esplayer;


VideoESPlayerNative::~VideoESPlayerNative()
{
    setCloseThreadsFlag();
    mVideoDecoderThread.reset();  // Thread destructors wait for threads to exit.
    mVideoRendererThread.reset(); // We need the renderer thread destroyed before
                                  // the manager releases the renderer.

}

static const IPlaybackGroup::Rect zeroRect = {0, 0, 0, 0};

VideoESPlayerNative::VideoESPlayerNative() : mCurrent3DFormat(NOT_3D),
                                             mTransitionStartTime(0),
                                             mTransitionEndTime(0),
                                             mSrcVideoWindow(zeroRect),
                                             mDstVideoWindow(zeroRect),
                                             mCurrentVideoWindow(zeroRect),
                                             mTsOfLastFrameDeliveredToRenderer(INVALID_TIMESTAMP)
{
}

void VideoESPlayerNative::close()
{
    setCloseThreadsFlag();
    mVideoDecoderThread.reset();  // Thread destructors wait for threads to exit.
    mVideoRendererThread.reset(); // We need the renderer thread destroyed before
                                  // the manager releases the renderer.
    ESPlayerNative::close();
}

NFErr VideoESPlayerNative::init(const struct StreamPlayerInitData& initData,
                                shared_ptr<IESPlayerCallback> callback,
                                PlaybackGroupNative *playbackGroup)

{
    NFErr err;
    ullong deviceSpecificErroCode;
    ESPlayerNative::init(initData, callback, playbackGroup);

    NTRACE(TRACE_MEDIAPLAYBACK
           , "VideoESPlayerNative::init FrameRate : %d %d, codecType %s\n"
           , initData.mMaxVideoFrameRateValue, initData.mMaxVideoFrameRateScale
           , initData.mInitialStreamAttributes->mVideoAttributes->mCodecParam.c_str()
        );
    // Create video renderer and get info.
    mVideoRenderer = playbackGroup->getESManager()->
        getRendererManager()->createVideoRenderer(*(initData.mInitialStreamAttributes->mVideoAttributes),
                                                  initData.mMaxWidth,
                                                  initData.mMaxHeight,
                                                  initData.mMaxVideoFrameRateValue,
                                                  initData.mMaxVideoFrameRateScale,
                                                  mPipelineId);
    if (!mVideoRenderer)
    {
        NTRACE(TRACE_MEDIAPLAYBACK, "[%d] Error VideoESPlayerNative::init: could not create video renderer", mPipelineId);
        deviceSpecificErroCode = refAppDeviceSpecific_VideoPlayerInitializationFailed;
        Variant errInfo;
        std::ostringstream ss;
        ss << "[" << mPipelineIdString << "Error VideoESPlayerNative::init: could not create video renderer";
        errInfo["errorDescription"] = ss.str();


        return err.push(new IDeviceError(INITIALIZATION_ERROR,
                                         deviceSpecificErroCode,
                                         errInfo));
    }
    VideoRenderer::VideoRendererInfo rendererInfo;
    mVideoRenderer->getInfo(rendererInfo);


    // Create video decoder
    mVideoDecoder = playbackGroup->getESManager()->getDecoderManager()->createVideoDecoder(
        *(initData.mInitialStreamAttributes->mVideoAttributes),
        *mPlaybackGroup->getReferenceClock(),
        rendererInfo,
        mPipelineId);
    if (!mVideoDecoder) {
        NTRACE(TRACE_MEDIAPLAYBACK, "[%d] Error VideoESPlayerNative::init: could not create video decoder", mPipelineId);
        deviceSpecificErroCode = refAppDeviceSpecific_VideoPlayerInitializationFailed;
        Variant errInfo;
        std::ostringstream ss;
        ss <<  "[" << mPipelineIdString << "] Error VideoESPlayerNative::init: could not create video decoder";
        errInfo["errorDescription"] = ss.str();
        return err.push(new IDeviceError(INITIALIZATION_ERROR,
                                         deviceSpecificErroCode,
                                         errInfo));
    }

    // Initialize the sample writer that we'll hand to the SDK to get access
    // units of encoded video. If this is MVC and we've requested MVC_SPLIT
    // initialize the sample writer to handle two views.
    mCurrent3DFormat = initData.mInitialStreamAttributes->
        mVideoAttributes->mFormat3D;
    mSampleWriter.reset(new SampleWriterNative(mCurrent3DFormat, callback));


    // Start the decoder and render threads.
    mVideoDecoderThread.reset(
        new DeviceThread(*this, &ESPlayerNative::decoderTask, &THREAD_REFERENCE_DPI_VIDEO_DECODER));
#if defined(BUILD_OPENMAXIL_VIDEO_RENDERER) && defined(BUILD_OPENMAXIL_VIDEO_DECODER)
    //don't need the rendererTask --Sam
#else
    mVideoRendererThread.reset(
        new DeviceThread(*this, &ESPlayerNative::rendererTask, &THREAD_REFERENCE_DPI_VIDEO_DISPLAY));
#endif
     return err;
}

void VideoESPlayerNative::decoderTask()
{
    static const Time WAIT_FOR_VIDEO_FRAME_BUFFER(10);
    static const Time WAIT_FOR_VIDEO_DATA(100);
    static const Time WAIT_WHILE_IDLING(100);

    const ISampleAttributes *pSampleAttr = NULL;
    VideoRenderer::VideoFrameBuffer frameBuffer;
    bool resend = false;
    bool EOSNullSampleSent = false;
    bool errorReported = false;
    NFErr err = NFErr_OK;
    uint32_t consecutiveMediaSampleNotAvailable = 0;

    while(closeThreadsFlagIsSet() == false)
    {
        if(mVideoDecoder->getFlushState() != FLUSH_OFF)
        {
            if(mVideoDecoder->getFlushState() == FLUSH_BEGIN)
            {
                ScopedMutex cs(mDecoderTaskMutex);
                mVideoDecoder->reset();
                frameBuffer.discardPicture();
                mVideoDecoder->setInputExhausted(true);
                mVideoDecoder->setFlushState(FLUSH_END);

                // vanguard hevc decoder need explicit flush call
                bool dummy;
                Format3D dummyFormat3D;
                mVideoDecoder->decode(NULL, NULL, 0, frameBuffer, dummy, dummy, dummyFormat3D);

                EOSNullSampleSent = false;
                mEndOfStreamFlag = false;
                mTsOfLastFrameDeliveredToRenderer = INVALID_TIMESTAMP;
                resend = false;
            }
            if(!mVideoRendererThread.get() && mVideoRenderer->getFlushState() == FLUSH_BEGIN) {
                mVideoRenderer->flushFrameBufferQueue();
                mVideoRenderer->setInputExhausted(true);
                mVideoRenderer->setFlushState(FLUSH_END);
                mPrevPicTimestamp = INVALID_TIMESTAMP;
                mPrevRefTimestamp = INVALID_TIMESTAMP;
            }
            Thread::Sleep(ESPlayerConstants::WAIT_FOR_FLUSH_OFF);
            continue;
        }

        if(errorReported)
        {
            Thread::Sleep(WAIT_WHILE_IDLING);
            continue;
        }

        if(!mVideoRendererThread.get()) {
            static const Time WAIT_FOR_NEXT_FRAME(10);
            static const Time WAIT_FOR_AVSYNC_MASTER(10);
            int64_t picTimestamp = 0;
            int64_t refTimestamp = mPlaybackGroup->getReferenceClock()->getCurrPresentationTime();
            if (refTimestamp != INVALID_TIMESTAMP ){
                while(mVideoRenderer->getNextDisplayTimestamp(picTimestamp)){
                    mVideoRenderer->setInputExhausted(false);
                    picTimestamp += ESPlayerConstants::VIDEO_REN_PTS_TWEAK;
                    if(refTimestamp < picTimestamp)
                        break;

                    mVideoRenderer->displayFrame();
                    handleFrameDropReporting(refTimestamp, picTimestamp);
                    mPlaybackGroup->getReferenceClock()->updateVideoPresentationTime(picTimestamp);
                    mCallback->updatePlaybackPosition(picTimestamp);
                }
            }
        }

        // If the frame-buffer has picture ready, send it to the video renderer.
        // If the queue inside video renderer is full, re-attempt sending it later.
        if(frameBuffer.gotPicture())
        {

            if(!mVideoRenderer->sendFrameBuffer(frameBuffer) && mVideoRendererThread.get())
            {
                Thread::Sleep(WAIT_FOR_VIDEO_FRAME_BUFFER);
                continue;
            }
            {
                // We need the mutex because the variable is read from the SDK's
                // thread.
                ScopedMutex cs(mDecoderTaskMutex);
                mTsOfLastFrameDeliveredToRenderer = frameBuffer.getTimestamp();
            }
        }

        if(!resend && !mEndOfStreamFlag)
        {
            Status status;

            // Check if any access-unit data is available to decode.
            status = mCallback->getNextMediaSample(*mSampleWriter);
            if(status == NO_AVAILABLE_SAMPLES) {
                pSampleAttr = NULL;
                consecutiveMediaSampleNotAvailable++;
                // check consecutive NO_AVAILABLE_SAMPLES count to prevent premature underflow report
                if ( consecutiveMediaSampleNotAvailable > ESPlayerConstants::DECODER_INPUT_EXHAUSTION_THRESHOLD ){
                    mVideoDecoder->setInputExhausted(true);
                }
                NTRACE(TRACE_MEDIAPLAYBACK
                       ,"[%d] VideoESPlayerNative::decoderTask::getNextMediaSample returns NO_AVAILABLE_SAMPLES"
                       ,mPipelineId);
                Thread::Sleep(WAIT_FOR_VIDEO_DATA);
                continue;
            } else if(status == NO_AVAILABLE_BUFFERS) {
                pSampleAttr = NULL;
                NTRACE(TRACE_MEDIAPLAYBACK
                       ,"[%d] VideoESPlayerNative::decoderTask::getNextMediaSample returns NO_AVAILABLE_BUFFERS"
                       ,mPipelineId);
                Thread::Sleep(WAIT_FOR_VIDEO_DATA);
                continue;
            } else if (status == END_OF_STREAM) {
                {
                    ScopedMutex cs(mDecoderTaskMutex);
                    mEndOfStreamFlag = true;
                }
                mVideoDecoder->setInputExhausted(true);
                mSampleWriter->reset();
                pSampleAttr = NULL; // As a precaution.  GetNextMediaSample
                                    // should set it to NULL.
                NTRACE(TRACE_MEDIAPLAYBACK
                       ,"[%d] VideoESPlayerNative::decoderTask::getNextMediaSample returns END_OF_STREAM"
                       ,mPipelineId);
            } else if(status == STREAM_ERROR) {
                Log::error(TRACE_MEDIAPLAYBACK
                           ,"[%d] VideoESPlayerNative::decoderTask::getNextMediaSample returns STREAM_ERROR."
                           ,mPipelineId);
                errorReported = true;
                continue;
            } else if (status == ERROR) {
                Log::error(TRACE_MEDIAPLAYBACK,
                           "[%d] VideoESPlayerNative::decoderTask::getNextMediaSample returns ERROR. "
                           ,mPipelineId);
                errorReported = true;
                continue;
            } else {
                consecutiveMediaSampleNotAvailable = 0;
                mVideoDecoder->setInputExhausted(false);
                pSampleAttr = mSampleWriter->getSampleAttributes();

                // loop around decrypt until we get drmsession object for
                // current sample. Once we retrieve sample, we should keep it
                // until we consume it.
                while(mVideoDecoder->getFlushState() == FLUSH_OFF){
                    err = decrypt(pSampleAttr, *mSampleWriter);
                    if(!err.ok()) {
                        // with multisession drm, even if contract between JS and
                        // nrdlib/app mandate DRMed content is delivered only after
                        // licence is installed, just for safety, we wait in a loop
                        // until license is arrived and corresponding drm session is
                        // created.
                        if (err == NFErr_Pending) {
                            Thread::Sleep(WAIT_FOR_VIDEO_DATA);
                            continue;
                        }

                        Log::error(TRACE_MEDIAPLAYBACK
                                   ,"[%d] VideoESPlayerNative::decoderTask: error in decrypt."
                                   ,mPipelineId);
                        mCallback->reportError(err);
                        errorReported = true;
                    }
                    break;
                }

                if (mVideoDecoder->getFlushState() != FLUSH_OFF) {
                    continue;
                }

                if(errorReported){
                    continue;
                }
            }
        }

        // If we get here, either pSampleAttr is valid or mEndOfStreamFlag is
        // set.
        //
        // If we've received endOfStream and sent a null sample to the decoder
        // to force it to flush, the thread's work is done.
        if(EOSNullSampleSent) {
            Thread::Sleep(WAIT_FOR_VIDEO_DATA);
            continue;
        }

        // Lock a frame-buffer, decode the access-unit and write the decoded
        // picture into the frame-buffer. The decoder may or may not output a
        // picture.
        if (mVideoRenderer->lockFrameBuffer(frameBuffer) == NFErr_OK) {
            bool gotPicture = false;
            Format3D format3D = NOT_3D;
            uint64_t decodeErr = refAppDeviceSpecific_NoError;

            if(pSampleAttr){
                NTRACE(TRACE_MEDIAPLAYBACK
                       , "[%d] VideoESPlayerNative::%s decode pts %lld, dts %lld"
                       , mPipelineId, __func__, pSampleAttr->getPTS(), pSampleAttr->getDTS());
            }

            if((mVideoDecoder->getFlushState() != FLUSH_OFF) || closeThreadsFlagIsSet()){
                mVideoRenderer->unlockFrameBuffer(frameBuffer, gotPicture);
                NTRACE(TRACE_MEDIAPLAYBACK
                       , "[%d] VideoESPlayerNative::%s flush was issued. Abort decoding pts %lld, dts %lld"
                       , mPipelineId, __func__, pSampleAttr->getPTS(), pSampleAttr->getDTS());
                continue;
            }

            // The ffmpeg decoder cannot decode MVC but can decode the base
            // view. To allow at least partial testing of MVC in split mode, we
            // allow the SDK to reorder the nalus and write the base and
            // dependent view separately with our sample writer. Our
            // samplewriter concatenates the base and dependent nalus and can
            // provide pointers and sizes to each. For our decoder, we just
            // present the base sample and size.
            decodeErr = mVideoDecoder->decode(pSampleAttr, mSampleWriter->getBuffer(),
                                              mSampleWriter->getSampleSize(),
                                              frameBuffer, gotPicture, resend, format3D);
            if( decodeErr != refAppDeviceSpecific_NoError ){

                if( (decodeErr == refAppDeviceSpecific_FfmpegDecoderError) &&
                    (mVideoDecoder->getFlushState() != FLUSH_OFF) ){
                    NTRACE(TRACE_MEDIAPLAYBACK
                           ,"[%d] VideoESPlayerNative::decoderTask: ignore decode error during flush"
                           ,mPipelineId);
                    continue;
                }

                err = NFErr_OK;
                if( decodeErr == refAppDeviceSpecific_WrongBitDepth ){
                    NTRACE(TRACE_MEDIAPLAYBACK
                           ,"[%d] VideoESPlayerNative::decoderTask: fatal error in decode bitdepth is not consistent"
                           ,mPipelineId);
                    std::ostringstream ss;
                    ss << "[" << mPipelineIdString << "] VideoESPlayerNative::decoderTask: fatal error in decode."
                       << "bitdepth is not consistent";
                    Variant errInfo;
                    errInfo["errorDescription"] = ss.str();
                    err.push(new IDeviceError(DECODE_ERROR, decodeErr, errInfo));
                    mCallback->reportError(err);
                } else {
                    Log::error(TRACE_MEDIAPLAYBACK
                               ,"[%d] VideoESPlayerNative::decoderTask: fatal error in decode", mPipelineId);
                    std::ostringstream ss;
                    ss << "[" << mPipelineIdString << "] VideoESPlayerNative::decoderTask: fatal error in decode."
                       << "NALU sizes do not sum to access unit size";
                    Variant errInfo;
                    errInfo["errorDescription"] = ss.str();
                    err.push(new IDeviceError(DECODE_ERROR, decodeErr, errInfo));
                    mCallback->reportError(err);
                }
                errorReported = true;
                continue;
            }
            // Report that a frame was decoded or skipped and report whether there were any non-fatal errors.
            reportDecodeOutcome(mVideoDecoder->getStatsCounter());

            if(!resend && pSampleAttr != NULL && pSampleAttr->isKeyFrame())
            {
                log3DType(format3D);
            }
            mVideoRenderer->unlockFrameBuffer(frameBuffer, gotPicture);
            EOSNullSampleSent = (pSampleAttr == NULL) && !resend && mEndOfStreamFlag;
        } else {
            NTRACE(TRACE_MEDIAPLAYBACK, "[%d] VideoESPlayerNative:%s lockFrameBuffer failed", mPipelineId, __func__);
        }
    }
}


// If the mCurrentFormat3D is MVC_SPLIT, logs that.  Otherwise it logs
// the type entered as the argument.
void VideoESPlayerNative::log3DType(Format3D format3D)
{
    std::string format;

    if(mCurrent3DFormat == MVC_SPLIT)
    {
        format = "MVC_SPLIT";
    } else if(format3D == MVC_COMBINED){
        format = "MVC_COMBINED";
    } else {
        return;
        // Don't log  NOT_3D
    }

    NTRACE(TRACE_MEDIAPLAYBACK
           ,"[%d] VideoESPlayerNative:decoderTask: Elementary stream's 3D format is %s"
           ,mPipelineId, format.c_str());
}

bool VideoESPlayerNative::readyForPlaybackStart()
{
#if defined(BUILD_SINK_VIDEO_RENDERER)
    return true;
#endif

    {
        ScopedMutex cs(mDecoderTaskMutex);
        if(mEndOfStreamFlag == true)
        {
            return true;
        }
    }

    int64_t tsAtHeadOfRenderQueue;
    if(mVideoRenderer->getNextDisplayTimestamp(tsAtHeadOfRenderQueue) == true)
    {

        // mTsOfLastDeliveredFrameToRenderer is set on the decoder thread so we
        // need the mutex to access it.
        ScopedMutex cs(mDecoderTaskMutex);
        int64_t renderBacklogInMs = mTsOfLastFrameDeliveredToRenderer - tsAtHeadOfRenderQueue;
        if(renderBacklogInMs > ESPlayerConstants::VIDEO_PLAYBACK_START_THRESHOLD_IN_MS)
        {
            return true;
        }

        NTRACE(TRACE_MEDIAPLAYBACK, "[%d] video %s return false : renderBacklogInMs %lld = (%lld - %lld)"
               , mPipelineId, __func__, renderBacklogInMs, mTsOfLastFrameDeliveredToRenderer, tsAtHeadOfRenderQueue);

    } else {
        NTRACE(TRACE_MEDIAPLAYBACK, "[%d] video %s return false: no frame to render getNextDisplayTimestamp"
               , mPipelineId, __func__);
    }
    return false;
}

void VideoESPlayerNative::rendererTask()
{
    mVideoRenderer->threadStart();
    static const Time WAIT_FOR_NEXT_FRAME(10);
    static const Time WAIT_FOR_AVSYNC_MASTER(10);
    int64_t picTimestamp = 0;
    mPrevPicTimestamp = INVALID_TIMESTAMP;
    mPrevRefTimestamp = INVALID_TIMESTAMP;

    while(closeThreadsFlagIsSet() == false)
    {
        updateTransition();

        if(mVideoRenderer->getFlushState() != FLUSH_OFF)
        {
            if(mVideoRenderer->getFlushState() == FLUSH_BEGIN)
            {
                mVideoRenderer->flushFrameBufferQueue();
                mVideoRenderer->setInputExhausted(true);
                mVideoRenderer->setFlushState(FLUSH_END);
                mPrevPicTimestamp = INVALID_TIMESTAMP;
                mPrevRefTimestamp = INVALID_TIMESTAMP;
            }

            Thread::Sleep(ESPlayerConstants::WAIT_FOR_FLUSH_OFF);
            continue;
        }

        if(!mVideoRenderer->getNextDisplayTimestamp(picTimestamp))
        {
            mVideoRenderer->setInputExhausted(true);
            underflowReporter();
            Thread::Sleep(WAIT_FOR_NEXT_FRAME);
            continue;
        }

        mVideoRenderer->setInputExhausted(false);
        picTimestamp += ESPlayerConstants::VIDEO_REN_PTS_TWEAK;

        int64_t refTimestamp = mPlaybackGroup->getReferenceClock()->getCurrPresentationTime();
        if(refTimestamp == INVALID_TIMESTAMP)
        {
            Thread::Sleep(WAIT_FOR_AVSYNC_MASTER);
            continue;
        }

        if(picTimestamp == INVALID_TIMESTAMP || refTimestamp >= picTimestamp)
        {
            mVideoRenderer->displayFrame();
            handleFrameDropReporting(refTimestamp, picTimestamp);

            if(picTimestamp == INVALID_TIMESTAMP) {
                Thread::Sleep(WAIT_FOR_NEXT_FRAME);
            } else {
                mPlaybackGroup->getReferenceClock()->updateVideoPresentationTime(picTimestamp);
                mCallback->updatePlaybackPosition(picTimestamp);
                //printf("renderer report %lld PTS_RENDERED\n", picTimestamp);
            }
            continue;
        }

        Thread::Sleep(WAIT_FOR_NEXT_FRAME);
    }
    mVideoRenderer->threadEnd();
}

void VideoESPlayerNative::handleFrameDropReporting(const int64_t& refTimestamp,
                                  const int64_t& picTimestamp)
{
    // If the difference between the time stamp and the rendering time grew by
    // half a frame period since the last rendered frame, we'll report that
    // we've dropped a frame at the renderer.
    if((picTimestamp != INVALID_TIMESTAMP && mPrevPicTimestamp != INVALID_TIMESTAMP) &&
       (((refTimestamp - picTimestamp)- (mPrevRefTimestamp - mPrevPicTimestamp)) >
        (picTimestamp - mPrevPicTimestamp)/2))
    {
        NTRACE(TRACE_MEDIAPLAYBACK
               ,"[%d] VideoESPlayer dropped frame in rendering: RefTS %lld, picTS %lld, mPrevRefTS %lld, mPrevPicTS %lld"
               ,mPipelineId, refTimestamp, picTimestamp, mPrevRefTimestamp, mPrevPicTimestamp);
        mCallback->updateRenderingStats(0, 1, IESPlayerCallback::STAT_NOT_SET);
    } else {
        mCallback->updateRenderingStats(1,0,IESPlayerCallback::STAT_NOT_SET);
    }
    mPrevPicTimestamp = picTimestamp;
    mPrevRefTimestamp = refTimestamp;
}

bool VideoESPlayerNative::inputsAreExhausted()
{
    if(mVideoDecoder->isInputExhausted() && mVideoRenderer->isInputExhausted())
    {
        return true;
    }
    return false;
}

void VideoESPlayerNative::flush()
{
    // Not implemented.  Video is only flushed via a flush on the entire
    // playback group.
    assert(0);
}

void VideoESPlayerNative::beginFlush()
{
    setFlushAndWait(mVideoDecoder);
    setFlushAndWait(mVideoRenderer);
}


void VideoESPlayerNative::endFlush()
{
    mVideoRenderer->setFlushState(FLUSH_OFF);
    mVideoDecoder->setFlushState(FLUSH_OFF);
}


static inline int64_t now()
{
    return Time::serverTime().val();
}

IPlaybackGroup::Rect
VideoESPlayerNative::setVideoWindow(const IPlaybackGroup::Rect& targetRect, uint32_t transitionDuration)
{
    NTRACE(TRACE_MEDIAPLAYBACK
           ,"[%d] VideoESPlayerNative::setVideoWindow %d %d %d %d - %d from %d %d %d %d"
           ,mPipelineId, targetRect.x, targetRect.y, targetRect.width, targetRect.height, transitionDuration
           ,mCurrentVideoWindow.x, mCurrentVideoWindow.y, mCurrentVideoWindow.width, mCurrentVideoWindow.height);

     if(mVideoRenderer && transitionDuration) // Don't bother transitioning if the renderer is absent
    {
        // Initialize transition information for updateTransition()
        mTransitionStartTime = now();
        mTransitionEndTime = mTransitionStartTime + (transitionDuration * 1000);
        mSrcVideoWindow = mCurrentVideoWindow;
        mDstVideoWindow = targetRect;
        if(!mSrcVideoWindow.width)
            mSrcVideoWindow.width = ESPlayerConstants::VIDEO_REN_SCREEN_WIDTH;
        if(!mSrcVideoWindow.height)
            mSrcVideoWindow.height = ESPlayerConstants::VIDEO_REN_SCREEN_HEIGHT;
        if(!mDstVideoWindow.width)
            mDstVideoWindow.width = ESPlayerConstants::VIDEO_REN_SCREEN_WIDTH;
        if(!mDstVideoWindow.height)
            mDstVideoWindow.height = ESPlayerConstants::VIDEO_REN_SCREEN_HEIGHT;

        return mCurrentVideoWindow;
    }
    else
    {
        // No transition required
        mTransitionStartTime = 0;
        mTransitionEndTime = 0;
        mDstVideoWindow = targetRect;
        return mDstVideoWindow;
    }
}

void VideoESPlayerNative::updateTransition()
{
    // if immediate video window change is requested, do it right away.
    if( (mTransitionStartTime == 0) &&
        (mTransitionEndTime == 0) &&
        ( (mDstVideoWindow.x != mCurrentVideoWindow.x) ||
          (mDstVideoWindow.y != mCurrentVideoWindow.y) ||
          (mDstVideoWindow.width != mCurrentVideoWindow.width) ||
          (mDstVideoWindow.height != mCurrentVideoWindow.height) ) )
    {
        mCurrentVideoWindow = mDstVideoWindow;
        if(mVideoRenderer)
            mVideoRenderer->setVideoWindow(mCurrentVideoWindow);
        return;
    }

    if(!mTransitionStartTime || !mTransitionEndTime)
        return;

    assert(mTransitionStartTime != mTransitionEndTime);

    IPlaybackGroup::Rect n;
    int64_t t = now();
    if(t >= mTransitionEndTime)
    {
        mTransitionStartTime = 0;
        mTransitionEndTime = 0;
        n = mDstVideoWindow;
    }
    else
    {
        double interpolate = ((double)t - mTransitionStartTime) / ((double)mTransitionEndTime - mTransitionStartTime);
        n.x = (int)mSrcVideoWindow.x + (interpolate * ((int)mDstVideoWindow.x - (int)mSrcVideoWindow.x));
        n.y = (int)mSrcVideoWindow.y + (interpolate * ((int)mDstVideoWindow.y - (int)mSrcVideoWindow.y));
        n.width = (int)mSrcVideoWindow.width + (interpolate * ((int)mDstVideoWindow.width - (int)mSrcVideoWindow.width));
        n.height = (int)mSrcVideoWindow.height + (interpolate * ((int)mDstVideoWindow.height - (int)mSrcVideoWindow.height));
    }

    mCurrentVideoWindow = n;
    if(mVideoRenderer)
        mVideoRenderer->setVideoWindow(mCurrentVideoWindow);
}

MediaType VideoESPlayerNative::getMediaType()
{
    return VIDEO;
}
