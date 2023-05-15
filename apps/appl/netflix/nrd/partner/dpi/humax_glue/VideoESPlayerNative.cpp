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
#include "DecoderStatsCounter.h"
#include <nrdbase/Log.h>
#include <nrdbase/Time.h>
#include <nrdbase/Thread.h>
#include <nrd/AppThread.h>
#include <nrdbase/ScopedMutex.h>
#include <nrdbase/NFErr.h>

#include "MediaUtils.h"
#include "AVStatusManager.h"
#include "cso_def.h"
#include "cso_uva.h"

using namespace netflix;
using namespace netflix::device::esplayer;

VideoESPlayerNative* 	VideoESPlayerNative::s_hVideoPlayer = NULL;
pthread_mutex_t 		VideoESPlayerNative::s_mtxVideoPlayer;

VideoESPlayerNative::~VideoESPlayerNative()
{
    NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [VideoESPlayerNative::%s:%d] %s() (+)\n", __func__, __LINE__, __func__);
    setCloseThreadsFlag();
    mVideoDecoderThread.reset();  // Thread destructors wait for threads to exit.
#if 0
    mVideoRendererThread.reset(); // We need the renderer thread destroyed before
                                  // the manager releases the renderer.
#endif
    MutexLock();
    s_hVideoPlayer = NULL;
    MutexUnlock();
    pthread_mutex_destroy(&s_mtxVideoPlayer);
    NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [VideoESPlayerNative::%s:%d] %s() (-)\n", __func__, __LINE__, __func__);
}

static const IPlaybackGroup::Rect zeroRect = {0, 0, 0, 0};

VideoESPlayerNative::VideoESPlayerNative(unsigned int session_id)
                                            : mCurrent3DFormat(NOT_3D),
                                             mTransitionStartTime(0),
                                             mTransitionEndTime(0),
                                             mSrcVideoWindow(zeroRect),
                                             mDstVideoWindow(zeroRect),
                                             mCurrentVideoWindow(zeroRect),
                                             mTsOfLastFrameDeliveredToRenderer(INVALID_TIMESTAMP),
                                             m_session_id(session_id),
                                             m_source_buffer(NULL),
                                             m_dblLastCurVideoPts(0.0)
{
    pthread_mutex_init(&s_mtxVideoPlayer, NULL);
    MutexLock();
    s_hVideoPlayer = this;
    MutexUnlock();
}

bool VideoESPlayerNative::setSourceBuffer(void *src_buf)
{
    bool bRet = true;

    if(src_buf == NULL)
    {
        Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [%s:%d] src_buf is NULL\n", __func__, __LINE__);
        bRet = false;
        goto func_exit;
    }

    m_source_buffer = src_buf;

func_exit:
    return bRet;
}

void *VideoESPlayerNative::getSourceBuffer()
{
    return m_source_buffer;
}

void VideoESPlayerNative::close()
{
    setCloseThreadsFlag();
    mVideoDecoderThread.reset();  // Thread destructors wait for threads to exit.
#if 0
    mVideoRendererThread.reset(); // We need the renderer thread destroyed before
                                  // the manager releases the renderer.
#endif
    ESPlayerNative::close();
}

NFErr VideoESPlayerNative::init(const struct StreamPlayerInitData& initData,
                                shared_ptr<IESPlayerCallback> callback,
                                PlaybackGroupNative *playbackGroup)

{
    NFErr err;
    ullong deviceSpecificErroCode;
    ESPlayerNative::init(initData, callback, playbackGroup);

    NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [VideoESPlayerNative::%s:%d] %s() (+)\n", __func__, __LINE__, __func__);

    /* Set max image size for checking SD contents playback under HDCP:on */
    /* Use PlaybackDevice::initVideoPlayer(VideoAttributes *), not get thru decoder information */
    (void)NativeBridge::playbackGroup().notifyStreamMaxSize(initData.mMaxWidth, initData.mMaxHeight);
    NTRACE(TRACE_MEDIAPLAYBACK, "VideoESPlayerNative--MaxImageSize(width:%d, height:%d)\n", initData.mMaxWidth, initData.mMaxHeight);

    NTRACE(TRACE_MEDIAPLAYBACK
           , "VideoESPlayerNative::init FrameRate : %d %d, codecType %s\n"
           , initData.mMaxVideoFrameRateValue, initData.mMaxVideoFrameRateScale
           , initData.mInitialStreamAttributes->mVideoAttributes->mCodecParam.c_str()
        );

#if 0
    // Create video renderer and get info.
    mVideoRenderer = playbackGroup->getESManager()->
        getRendererManager()->createVideoRenderer(*(initData.mInitialStreamAttributes->mVideoAttributes),
                                                  initData.mMaxWidth,
                                                  initData.mMaxHeight,
                                                  initData.mMaxVideoFrameRateValue,
                                                  initData.mMaxVideoFrameRateScale,
                                                  mPipelineId); // <--
    if (!mVideoRenderer)
    {
        NTRACE(TRACE_MEDIAPLAYBACK, "[%d] Error VideoESPlayerNative::init: could not create video renderer", mPipelineId);
        deviceSpecificErroCode = skeletonDeviceSpecific_VideoPlayerInitializationFailed;
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
#endif

    // Create video decoder
#if 0
    mVideoDecoder = playbackGroup->getESManager()->getDecoderManager()->createVideoDecoder(
        *(initData.mInitialStreamAttributes->mVideoAttributes),
        *mPlaybackGroup->getReferenceClock(),
        rendererInfo,
        mPipelineId);
#else
    mVideoDecoder = playbackGroup->getESManager()->getDecoderManager()->createVideoDecoder(
        *(initData.mInitialStreamAttributes->mVideoAttributes),
        mPipelineId);
#endif
    if (!mVideoDecoder) {
        NTRACE(TRACE_MEDIAPLAYBACK, "[%d] Error VideoESPlayerNative::init: could not create video decoder", mPipelineId);
        deviceSpecificErroCode = skeletonDeviceSpecific_VideoPlayerInitializationFailed;
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

#if 0
    // Initialize decoder
    mVideoDecoder.reset(new VideoDecoder(this, callback));
#endif

    // Start the decoder and render threads.
    mVideoDecoderThread.reset(
        new DeviceThread(*this, &ESPlayerNative::decoderTask, &THREAD_SKELETON_DPI_VIDEO_DECODER));
#if 0
    mVideoRendererThread.reset(
        new DeviceThread(*this, &ESPlayerNative::rendererTask, &THREAD_SKELETON_DPI_VIDEO_DISPLAY));
#endif

    NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [VideoESPlayerNative::%s:%d] %s() (-)\n", __func__, __LINE__, __func__);
    return err;
}

void VideoESPlayerNative::decoderTask()
{
    static const Time WAIT_FOR_VIDEO_FRAME_BUFFER(10);
    static const Time WAIT_FOR_VIDEO_DATA(10);
    static const Time WAIT_WHILE_IDLING(100);
    static const Time WAIT_FOR_SETTING_PLAY_STATUS(10);

    const ISampleAttributes *pSampleAttr = NULL;
#if 0
    VideoRenderer::VideoFrameBuffer frameBuffer;
#endif
    bool resend = false;
    bool EOSNullSampleSent = false;
    bool errorReported = false;
    NFErr err = NFErr_OK;
    uint32_t consecutiveMediaSampleNotAvailable = 0;
    AVStatusManager *hStatusManager = NULL;
    bool bGotFrame = false, bAddSource = false, bRet = true, bEndOfStreamStatus = false;
    int32_t nCsoVideoCodec = 0;
    void *src_buf = NULL;
    CSO_ERR csoErr = CSO_ERR_OK;
    AVStatusMgr_FlushStatus_e eFlushStatus = AVStatusMgr_FlushStatus_None;
    uint64_t ullSamplePts = 0;
	
    NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [VideoESPlayerNative::%s:%d] %s() (+)\n", __func__, __LINE__, __func__);

    AVStatusManager::MutexLock();
    hStatusManager = AVStatusManager::getInstance();
    if(hStatusManager == NULL)
    {
        AVStatusManager::MutexUnlock();
        Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [VideoESPlayerNative::%s:%d] hStatusManager is NULL\n", __func__, __LINE__);
        goto func_exit;
    }
    AVStatusManager::MutexUnlock();

    while(closeThreadsFlagIsSet() == false)
    {
        if(mVideoDecoder->getFlushState() != FLUSH_OFF)
        {
            if(mVideoDecoder->getFlushState() == FLUSH_BEGIN)
            {
                ScopedMutex cs(mDecoderTaskMutex);
                /* Flush */
#if 0
                mVideoDecoder->reset();
                frameBuffer.discardPicture();
#endif
                mVideoDecoder->setInputExhausted(true);

                /* Check if OCTO EndofStream Status is TRUE */
                AVStatusManager::MutexLock();
                bEndOfStreamStatus = hStatusManager->getEndOfStreamStatus();
                if(bEndOfStreamStatus)
                {
                    hStatusManager->setEndOfStreamStatus(false);
                }
                AVStatusManager::MutexUnlock();
                mVideoDecoder->setFlushState(FLUSH_END);

#if 0
                // vanguard hevc decoder need explicit flush call
                bool dummy;
                Format3D dummyFormat3D;
                mVideoDecoder->decode(NULL, NULL, 0, frameBuffer, dummy, dummy, dummyFormat3D);
#endif

                EOSNullSampleSent = false;
                mEndOfStreamFlag = false;
                mTsOfLastFrameDeliveredToRenderer = INVALID_TIMESTAMP;
                resend = false;
            }
            /*
            if(!mVideoRendererThread.get() && mVideoRenderer->getFlushState() == FLUSH_BEGIN) {
                mVideoRenderer->flushFrameBufferQueue();
                mVideoRenderer->setInputExhausted(true);
                mVideoRenderer->setFlushState(FLUSH_END);
                mPrevPicTimestamp = INVALID_TIMESTAMP;
                mPrevRefTimestamp = INVALID_TIMESTAMP;
            }
            */
            Thread::Sleep(ESPlayerConstants::WAIT_FOR_FLUSH_OFF);
            continue;
        }

        AVStatusManager::MutexLock();
        /* Write the ES data to source buffer */
        if( (bGotFrame) &&
            (hStatusManager->getPlaybackStatus() == AVStatusMgr_PlaybackStatus_Playing))
        {
            AVStatusManager::MutexUnlock();
			csoErr = CSO_UVA_SRCBUF_Write(getSourceBuffer(),
                                        m_stUVAWriteFrame.pts / 1000.0,
                                        m_stUVAWriteFrame.dts,
                                        m_stUVAWriteFrame.duration,
                                        m_stUVAWriteFrame.data_size,
                                        m_stUVAWriteFrame.data,
                                        m_stUVAWriteFrame.ext_size,
                                        m_stUVAWriteFrame.ext_data);
            if(csoErr != CSO_ERR_OK)
            {
                Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [VideoESPlayerNative::%s:%d] Writing video data failed\n", __func__, __LINE__);
            }
            else
            {
                bGotFrame = false;
                setLastCurrentVideoPts(m_stUVAWriteFrame.pts);
            }
        }
        else
        {	
            eFlushStatus = hStatusManager->getFlushStatus();
            /* Do not send packets to UVA during flushing */
            if( (bGotFrame) &&
                (eFlushStatus != AVStatusMgr_FlushStatus_Flushing) &&
                (hStatusManager->getPlaybackStatus() != AVStatusMgr_PlaybackStatus_Playing))
            {
                AVStatusManager::MutexUnlock();
				csoErr = CSO_UVA_SRCBUF_Write(getSourceBuffer(),
                            m_stUVAWriteFrame.pts / 1000.0,
                            m_stUVAWriteFrame.dts,
                            m_stUVAWriteFrame.duration,
                            m_stUVAWriteFrame.data_size,
                            m_stUVAWriteFrame.data,
                            m_stUVAWriteFrame.ext_size,
                            m_stUVAWriteFrame.ext_data);
                if(csoErr != CSO_ERR_OK)
                {
                    Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [VideoESPlayerNative::%s:%d] Writing video data failed\n", __func__, __LINE__);
                }
                else
                {
                    bGotFrame = false;
                    setLastCurrentVideoPts(m_stUVAWriteFrame.pts);
                }
            }
			else if((bGotFrame) &&
					(eFlushStatus != AVStatusMgr_FlushStatus_None) &&
					(hStatusManager->getDecodingStatus() != AVStatusMgr_DecodingStatus_Decoding))
			{
				AVStatusManager::MutexUnlock();
				Thread::Sleep(WAIT_FOR_VIDEO_DATA);
				continue;
			}
            else
            {
                AVStatusManager::MutexUnlock();
            }		
        }

        if(errorReported)
        {
            Thread::Sleep(WAIT_WHILE_IDLING);
            continue;
        }

#if 0
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
#endif

        // If the frame-buffer has picture ready, send it to the video renderer.
        // If the queue inside video renderer is full, re-attempt sending it later.
#if 0
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
#endif

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

                csoErr = CSO_UVA_SRCBUF_EOS(getSourceBuffer());
                if(csoErr != CSO_ERR_OK)
                {
                    Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [VideoESPlayerNative::%s:%d] Failed to set EOS, csoErr = %d\n", __func__, __LINE__, csoErr);
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
#if 0
                err = decrypt(pSampleAttr, *mSampleWriter);
                if(!err.ok())
                {
                    Log::error(TRACE_MEDIAPLAYBACK, "VideoESPlayerNative::decoderTask: error in decrypt.");
                    mCallback->reportError(err);
                    errorReported = true;
                    continue;
                }
#endif
                if(!bAddSource)
                {
                    ullSamplePts = pSampleAttr->getPTS();

                    /* Get the video attributes */
                    nCsoVideoCodec = 0;
                    bRet = getVideoCodecFromAttributes(pSampleAttr, &nCsoVideoCodec);
                    if(!bRet)
                    {
                        Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [VideoESPlayerNative::%s:%d] Getting a video codec failed\n", __func__, __LINE__);
                        continue;
                    }

                    /* Add source buffer to CSO_UVA */
                    csoErr = CSO_UVA_AddSource(m_session_id, NETFLIX_VIDEO_SOURCE_BUFFER_ID, CSO_MEDIATYPE_VIDEO, nCsoVideoCodec, &src_buf);
                    if(csoErr != CSO_ERR_OK)
                    {
                        Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [VideoESPlayerNative::%s:%d] Adding source buffer failed, csoErr = %d\n", __func__, __LINE__, csoErr);
                        continue;
                    }
                    else
                    {
                        bRet = setSourceBuffer(src_buf);
                        if(!bRet)
                        {
                            Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [VideoESPlayerNative::%s:%d] Setting a source buffer failed\n", __func__, __LINE__);
                        }
                        bAddSource = true;

                        AVStatusManager::MutexLock();
                        hStatusManager->updatePlayPosition((uint32_t)ullSamplePts);
                        hStatusManager->setVideoSourceBufferFlag(true);
                        Log::info(TRACE_MEDIAPLAYBACK, "@@@@@ [VideoESPlayerNative::%s:%d] Player has updated the first PTS = %llu\n", __func__, __LINE__, ullSamplePts);
                        AVStatusManager::MutexUnlock();
                    }
                }

            }
        }

        AVStatusManager::MutexLock();
        /* If playback doesn't start to play, then this task doesn't send ES Data to decode */
        if(hStatusManager->getPlaybackStatus() != AVStatusMgr_PlaybackStatus_Playing)
        {
            eFlushStatus = hStatusManager->getFlushStatus();
            AVStatusManager::MutexUnlock();
            if(eFlushStatus != AVStatusMgr_FlushStatus_None)
            {
                resend = false;
            }
            else
            {
                resend = true;
                Thread::Sleep(WAIT_FOR_SETTING_PLAY_STATUS);
                continue;
            }
        }
        else
        {
            AVStatusManager::MutexUnlock();
            resend = false;
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
#if 0
        if (mVideoRenderer->lockFrameBuffer(frameBuffer) == NFErr_OK)
#endif
        {
            bool gotPicture = false;
            Format3D format3D = NOT_3D;
            uint64_t decodeErr = skeletonDeviceSpecific_NoError;

#if 0
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
#endif

            // The ffmpeg decoder cannot decode MVC but can decode the base
            // view. To allow at least partial testing of MVC in split mode, we
            // allow the SDK to reorder the nalus and write the base and
            // dependent view separately with our sample writer. Our
            // samplewriter concatenates the base and dependent nalus and can
            // provide pointers and sizes to each. For our decoder, we just
            // present the base sample and size.
            decodeErr = mVideoDecoder->decode(pSampleAttr, mSampleWriter->getBuffer(),
                                              mSampleWriter->getSampleSize(),
                                              &m_stUVAWriteFrame, nCsoVideoCodec,
                                              gotPicture, resend, format3D);
            if( decodeErr != skeletonDeviceSpecific_NoError )
            {
#if 0
                if( (decodeErr == skeletonDeviceSpecific_FfmpegDecoderError) &&
                    (mVideoDecoder->getFlushState() != FLUSH_OFF) ){
                    NTRACE(TRACE_MEDIAPLAYBACK
                           ,"[%d] VideoESPlayerNative::decoderTask: ignore decode error during flush"
                           ,mPipelineId);
                    continue;
                }
#endif

                err = NFErr_OK;
                if( decodeErr == skeletonDeviceSpecific_WrongBitDepth ){
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
            else
            {
                if(m_stUVAWriteFrame.data != NULL)
                {
                    bGotFrame = true;
                }
            }

            // Report that a frame was decoded or skipped and report whether there were any non-fatal errors.
            reportDecodeOutcome(mVideoDecoder->getStatsCounter());

            if(!resend && pSampleAttr != NULL && pSampleAttr->isKeyFrame())
            {
                log3DType(format3D);
            }
#if 0
            mVideoRenderer->unlockFrameBuffer(frameBuffer, gotPicture);
#endif
            EOSNullSampleSent = (pSampleAttr == NULL) && !resend && mEndOfStreamFlag;
        }
        /*
        else {
            NTRACE(TRACE_MEDIAPLAYBACK, "[%d] VideoESPlayerNative:%s lockFrameBuffer failed", mPipelineId, __func__);
        }
        */
    }

    if(src_buf != NULL)
    {
        csoErr = CSO_UVA_RemoveSource(m_session_id, src_buf);
        if(csoErr != CSO_ERR_OK)
        {
            Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [VideoESPlayerNative::%s:%d] Failed to remove a video source buffer, csoErr = %d\n", __func__, __LINE__, csoErr);
        }

        src_buf = NULL;
    }

func_exit:
    NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [VideoESPlayerNative::%s:%d] %s() (-)\n", __func__, __LINE__, __func__);
    return;
}

bool VideoESPlayerNative::getVideoCodecFromAttributes(const ISampleAttributes *pSampleAttr, int32_t *pnCsoVideoCodec)
{
    bool bRet = true;
    const MediaAttributes *pMediaAttr = NULL;
    unsigned int unCodecTag = 0;

    NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [VideoESPlayerNative::%s:%d] %s() (+)\n", __func__, __LINE__, __func__);

    if(pSampleAttr == NULL)
    {
        Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [%s:%d] pSampleAttr is NULL\n", __func__, __LINE__);
        bRet = false;
        goto func_exit;
    }

    if(pnCsoVideoCodec == NULL)
    {
        Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [%s:%d] pnCsoVideoCodec is NULL\n", __func__, __LINE__);
        bRet = false;
        goto func_exit;
    }

    pMediaAttr = pSampleAttr->getMediaAttributes();
    if(pMediaAttr == NULL)
    {
        Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [%s:%d] pMediaAttr is NULL\n", __func__, __LINE__);
        bRet = false;
        goto func_exit;
    }
    if(pMediaAttr->mVideoAttributes == NULL)
    {
        Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [%s:%d] pMediaAttr->mVideoAttributes is NULL\n", __func__, __LINE__);
        bRet = false;
        goto func_exit;
    }

    unCodecTag = MediaUtils::getFourCCFromCodecParam(pMediaAttr->mVideoAttributes->mCodecParam);
    switch(unCodecTag)
    {
        case _FOURCC_LE_('W','M','V','3'):    /* Windows Media Video 9 */
            break;
        case _FOURCC_LE_('W','V','C','1'):
            *pnCsoVideoCodec = CSO_VCODEC_VC1;
            break;
        case _FOURCC_LE_('W','M','V','A'):
            *pnCsoVideoCodec = CSO_VCODEC_VC1;
            break;
        case _FOURCC_LE_('M','P','4','V'):
            *pnCsoVideoCodec = CSO_VCODEC_MPEG4Part2;
            break;
        case _FOURCC_LE_('a','v','c','1'):
            *pnCsoVideoCodec = CSO_VCODEC_H264;
            break;
        case _FOURCC_LE_('h','e','v','1'):
        case _FOURCC_LE_('h','v','c','1'):
            *pnCsoVideoCodec = CSO_VCODEC_H265;
            break;
        default:
            break;
    }

func_exit:
    NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [VideoESPlayerNative::%s:%d] %s() (-)\n", __func__, __LINE__, __func__);
    return bRet;
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
    AVStatusManager *hStatusManager = NULL;

    {
        ScopedMutex cs(mDecoderTaskMutex);
        // Make sure to return ready if we have already sent the last
        // access unit.
        if(mEndOfStreamFlag == true)
        {
            return true;
        }
    }

    hStatusManager = AVStatusManager::getInstance();
    if(hStatusManager == NULL)
    {
        Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [VideoESPlayerNative::%s:%d] hStatusManager is NULL\n", __func__, __LINE__);
        return false;
    }

    if(hStatusManager->getVideoSourceBufferFlag())
    {
        return true;
    }

    // PARTNER: The reference application uses the current renderer state to
    // determine if we should return true here.  Partners will need to set
    // a flag in the decoder context when their decoder buffer has enough
    // data.  In this current state, we will never go ready!
#if 0
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
#endif
    return false;
}

void VideoESPlayerNative::rendererTask()
{
#if 0
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
#endif
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
    if(mVideoDecoder->isInputExhausted()
        /*&& mVideoRenderer->isInputExhausted()*/)
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
#if 0
    setFlushAndWait(mVideoRenderer);
#endif
}


void VideoESPlayerNative::endFlush()
{
#if 0
    mVideoRenderer->setFlushState(FLUSH_OFF);
#endif
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

     if(/*mVideoRenderer &&*/ transitionDuration) // Don't bother transitioning if the renderer is absent
    {
        // Initialize transition information for updateTransition()
        mTransitionStartTime = now();
        mTransitionEndTime = mTransitionStartTime + (transitionDuration * 1000);
        mSrcVideoWindow = mCurrentVideoWindow;
        mDstVideoWindow = targetRect;

        if (0) /* Allow setVideoSize(0,0,0,0); OCTO will resize automatically */
        {
            if(!mSrcVideoWindow.width)
                mSrcVideoWindow.width = ESPlayerConstants::VIDEO_REN_SCREEN_WIDTH;
            if(!mSrcVideoWindow.height)
                mSrcVideoWindow.height = ESPlayerConstants::VIDEO_REN_SCREEN_HEIGHT;

            if(!mDstVideoWindow.width)
                mDstVideoWindow.width = ESPlayerConstants::VIDEO_REN_SCREEN_WIDTH;
            if(!mDstVideoWindow.height)
                mDstVideoWindow.height = ESPlayerConstants::VIDEO_REN_SCREEN_HEIGHT;
        }

        if (1) /* if (mDstVideoWindow.width && mDstVideoWindow.height) */
        {
            /* (0,0,0,0) -> resetVideoSize() */
            (void)NativeBridge::playbackGroup().setVideoSize(mDstVideoWindow.x, mDstVideoWindow.y, mDstVideoWindow.width, mDstVideoWindow.height);
        }
        return mCurrentVideoWindow;
    }
    else
    {
        // No transition required
        mTransitionStartTime = 0;
        mTransitionEndTime = 0;
        mDstVideoWindow = targetRect;

        if (1) /* if (mDstVideoWindow.width && mDstVideoWindow.height) */
        {
            /* (0,0,0,0) -> resetVideoSize() */
            (void)NativeBridge::playbackGroup().setVideoSize(mDstVideoWindow.x, mDstVideoWindow.y, mDstVideoWindow.width, mDstVideoWindow.height);
        }
        return mDstVideoWindow;
    }
}

void VideoESPlayerNative::updateTransition()
{
#if 0
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
    // PARTNER: set video window here
    if(mVideoRenderer)
        mVideoRenderer->setVideoWindow(mCurrentVideoWindow);
#endif
}

MediaType VideoESPlayerNative::getMediaType()
{
    return VIDEO;
}

double VideoESPlayerNative::getLastCurrentVideoPts()
{
    ScopedMutex cs(mDecoderTaskMutex);
    return m_dblLastCurVideoPts;
}
void VideoESPlayerNative::setLastCurrentVideoPts(double dblCurPts)
{
    ScopedMutex cs(mDecoderTaskMutex);
    m_dblLastCurVideoPts = dblCurPts;
}

