/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#include "AudioESPlayerNative.h"
#include "PlaybackGroupNative.h"
#include "ESManagerNative.h"
#include "DrmDecrypter.h"
#include "RendererManager.h"
#include "ESPlayerConstants.h"
#include "AudioDecoder.h"
#include "DeviceThread.h"
#include "SampleWriterNative.h"
#include "ReferenceClockNative.h"
#include <nrd/AppThread.h>
#include <nrdbase/Log.h>
#include <nrdbase/Time.h>
#include <nrdbase/Thread.h>
#include <nrdbase/ScopedMutex.h>
#include <nrdbase/NFErr.h>

#include "MediaUtils.h"
#include "AVStatusManager.h"

#include "cso_def.h"
#include "cso_uva.h"

using namespace netflix::device::esplayer;
using namespace netflix;

AudioESPlayerNative* 	AudioESPlayerNative::s_hAudioPlayer = NULL;
pthread_mutex_t 		AudioESPlayerNative::s_mtxAudioPlayer;

AudioESPlayerNative::~AudioESPlayerNative()
{
    NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [AudioESPlayerNative::%s:%d] %s() (+)\n", __func__, __LINE__, __func__);
    
    setCloseThreadsFlag();
    mAudioDecoderThread.reset(); // Thread destructor waits for thread to exit.
    mAudioDecoder.reset();

    /* need a mutex to sync */
    MutexLock();
    s_hAudioPlayer = NULL;
    m_source_buffer = NULL;
    MutexUnlock();

    NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [AudioESPlayerNative::%s:%d] %s() (-)\n", __func__, __LINE__, __func__);
}

void AudioESPlayerNative::close()
{
    setCloseThreadsFlag();
    mAudioDecoderThread.reset(); // Thread destructor waits for thread to exit.
    mAudioDecoder.reset();
    ESPlayerNative::close();
}

AudioESPlayerNative::AudioESPlayerNative(unsigned int session_id) 
    :     mPlaybackPending(false), 
        m_session_id(session_id), 
        m_source_buffer(NULL),
        m_dblLastCurAudioPts(0.0)
{
    /* need a mutex to sync */
    MutexLock();
    s_hAudioPlayer = this;
    MutexUnlock();
}

bool AudioESPlayerNative::setSourceBuffer(void *src_buf)
{
    bool bRet = true;
    
    if(src_buf == NULL)
    {
        Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [AudioESPlayerNative::%s:%d] src_buf is NULL\n", __func__, __LINE__);            
        bRet = false;
        goto func_done;    
    }

    m_source_buffer = src_buf;

func_done:
    return bRet;
}

void *AudioESPlayerNative::getSourceBuffer()
{
    return m_source_buffer;
}

NFErr AudioESPlayerNative::init(const struct StreamPlayerInitData& initData,
                          shared_ptr<IESPlayerCallback> callback,
                          PlaybackGroupNative* playbackGroup)
{
    NFErr err = NFErr_OK;
    ullong deviceSpecificErroCode = 0;
    ESPlayerNative::init(initData, callback, playbackGroup);
    uint32_t samplingRate = 48000;

    if(initData.mInitialStreamAttributes->mAudioAttributes->mSamplesPerSecond != 0)
    {
        samplingRate = initData.mInitialStreamAttributes->
            mAudioAttributes->mSamplesPerSecond;
    }

#if 0
    mAudioRenderer = mPlaybackGroup->getESManager()->getRendererManager()->
            createAudioRenderer(*(mPlaybackGroup->getReferenceClock()),
                            *this,
                            samplingRate,
                            ESPlayerConstants::AUDIO_REN_OUTPUT_BUF_SAMPLES,
                            ESPlayerConstants::AUDIO_REN_MAX_FRAME_SAMPLES,
                            ESPlayerConstants::AUDIO_REN_MAX_NUM_MAX_FRAMES,
                            ESPlayerConstants::AUDIO_REN_PTS_QUEUE_SIZE);

    if(!mAudioRenderer) {
        NTRACE(TRACE_MEDIAPLAYBACK, "Error AudioESPlayerNative::init: could not create audio renderer");

        deviceSpecificErroCode = skeletonDeviceSpecific_AudioPlayerInitializationFailed;
        Variant errInfo;
        errInfo["errorDescription"] = "Error AudioESPlayerNative::init: could not create audio renderer";
        return err.push(new IDeviceError(INITIALIZATION_ERROR,
                                         deviceSpecificErroCode,
                                         errInfo));
    }
#endif

#if 0
    mAudioDecoder = mPlaybackGroup->getESManager()->getDecoderManager()->
            createAudioDecoder(mAudioRenderer->getNumOutputChannels());
#else
    mAudioDecoder = mPlaybackGroup->getESManager()->getDecoderManager()->
            createAudioDecoder(ESPlayerConstants::AUDIO_REN_PTS_QUEUE_SIZE);
#endif
    if (!mAudioDecoder) {
        NTRACE(TRACE_MEDIAPLAYBACK, "Error AudioESPlayerNative::init: could not create audio decoder");

        deviceSpecificErroCode = skeletonDeviceSpecific_AudioPlayerInitializationFailed;
        Variant errInfo;
        errInfo["errorDescription"] = "Error AudioESPlayerNative::init: could not create audio decoder";
        return err.push(new IDeviceError(INITIALIZATION_ERROR,
                                         deviceSpecificErroCode,
                                         errInfo));
    }

    mSampleWriter.reset(new SampleWriterNative(NOT_3D, callback));
    mAudioDecoderThread.reset(new DeviceThread(*this, &ESPlayerNative::decoderTask,
                                               &THREAD_REFERENCE_DPI_AUDIO_DECODER));

    return err;
}

// For the reference audio renderer, the renderer thread runs in the renderer
// plugin.
void AudioESPlayerNative::rendererTask()
{
}

void AudioESPlayerNative::decoderTask()
{
    static const Time WAIT_FOR_AUDIO_SAMPLE_BUFFER(30);
    static const Time WAIT_FOR_AUDIO_DATA(30);
    static const Time WAIT_WHILE_IDLING(100);
    static const Time WAIT_FOR_SETTING_PLAY_STATUS(10);

    Status status;
    const ISampleAttributes* pSampleAttr = NULL;
    DecodedAudioBuffer decodedAudioBuffer;
    bool errorReported = false;
    NFErr err = NFErr_OK;
    uint32_t consecutiveMediaSampleNotAvailable = 0;
    AVStatusManager *hStatusManager = NULL;
    bool bAddSource = false, bGotFrame = false, bResend = false, bRet = true, bAudioCodecSwitchStatus = false, bEndOfStreamStatus = false;
    int32_t nCsoAudioCodec = 0, nCurrentCsoAudioCodec = 0;
    void *src_buf = NULL;

    CSO_ERR csoErr = CSO_ERR_OK;
    AVStatusMgr_FlushStatus_e eFlushStatus = AVStatusMgr_FlushStatus_None;
    
    NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [AudioESPlayerNative::%s:%d] %s() (+)\n", __func__, __LINE__, __func__);    
    
    AVStatusManager::MutexLock();
    hStatusManager = AVStatusManager::getInstance();
    if(hStatusManager == NULL)
    {
        AVStatusManager::MutexUnlock();
        Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [AudioESPlayerNative::%s:%d] hStatusManager is NULL\n", __func__, __LINE__);
        goto func_done;
    }
    AVStatusManager::MutexUnlock();

    while(closeThreadsFlagIsSet() == false)
    {
        if(mAudioDecoder->getFlushState() != FLUSH_OFF)
        {
            if(mAudioDecoder->getFlushState() == FLUSH_BEGIN)
            {
                mAudioDecoder->reset();
                mAudioDecoder->setInputExhausted(true);

                /* Check if OCTO EndofStream Status is TRUE */
                AVStatusManager::MutexLock();
                bEndOfStreamStatus = hStatusManager->getEndOfStreamStatus();    
                if(bEndOfStreamStatus)
                {
                    hStatusManager->setEndOfStreamStatus(false);
                }
                AVStatusManager::MutexUnlock();
                
                mAudioDecoder->setFlushState(ESPlayerNative::FLUSH_END);
                ScopedMutex cs(mDecoderTaskMutex);
                mEndOfStreamFlag = false;
                bResend = false;
            }
            Thread::Sleep(ESPlayerConstants::WAIT_FOR_FLUSH_OFF);
            continue;
        }

        if(errorReported)
        {
            Thread::Sleep(WAIT_WHILE_IDLING);
            continue;
        }

        // Playback may be pending if the player was enabled while the playback
        // group's playback state was PLAY. This would be the case after an
        // on-the-fly audio switch, for example. When playback is pending, the
        // renderer continues to act as if the player is disabled (e.g. it
        // doesn't render output and it doesn't report underflow). If we have
        // enough audio frames buffered to begin playback, we should lower the
        // pending flag and let playback begin.
        AVStatusManager::MutexLock();

        if(hStatusManager->getCsoAudioFlushStatus())
        {
            AVStatusManager::MutexUnlock();
            Thread::Sleep(WAIT_FOR_SETTING_PLAY_STATUS);
            continue;
        }

        if(playbackPending() && readyForPlaybackStart())
        {
            setPlaybackPending(false);
        }

        AVStatusManager::MutexUnlock();
        
#if 0
        if(decodedAudioBuffer.size() == 0)
        {
            if(!mAudioRenderer->getSampleBuffer(decodedAudioBuffer))
            {
                Thread::Sleep(WAIT_FOR_AUDIO_SAMPLE_BUFFER);
                continue;
            }
        }
#endif

        if (!bResend)
        {
            status = mCallback->getNextMediaSample(*mSampleWriter);
            if(status == NO_AVAILABLE_SAMPLES) 
            {
#if 0
                mAudioDecoder->setInputExhausted(true);
#else
                consecutiveMediaSampleNotAvailable++;
                // check consecutive NO_AVAILABLE_SAMPLES count to prevent premature underflow report
                if ( consecutiveMediaSampleNotAvailable > ESPlayerConstants::DECODER_INPUT_EXHAUSTION_THRESHOLD*10)
                {
                    NTRACE(TRACE_MEDIAPLAYBACK, "consecutive NO_AVAILABLE_SAMPLES %d "
                           , consecutiveMediaSampleNotAvailable);
                    mAudioDecoder->setInputExhausted(true);
                }
#endif
                Thread::Sleep(WAIT_FOR_AUDIO_DATA);
                continue;
            } else if (status == NO_AVAILABLE_BUFFERS)
            {
                Thread::Sleep(WAIT_FOR_AUDIO_DATA);
                continue;
            } else if (status == END_OF_STREAM)
            {
                // This is the only thread that sets mEndOfStream.  We don't need
                // the mutex to read it.
                if(mEndOfStreamFlag == false)
                {
                    NTRACE(TRACE_MEDIAPLAYBACK, "AudioESPlayerNative::decoderTask: "
                           "getNextMediaSample returns END_OF_STREAM");
                }
                {
                    // The renderer thread reads mEndOfStreamFlag. We need the mutex
                    // to set it.
                    ScopedMutex cs(mDecoderTaskMutex);
                    mEndOfStreamFlag = true;
                }
                
                csoErr = CSO_UVA_SRCBUF_EOS(getSourceBuffer());
                if(csoErr != CSO_ERR_OK)
                {
                    Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [AudioESPlayerNative::%s:%d] Failed to set EOS, csoErr = %d\n", __func__, __LINE__, csoErr);
                }
                
                mAudioDecoder->setInputExhausted(true);
                Thread::Sleep(WAIT_FOR_AUDIO_DATA);
                continue;
            } else if (status == STREAM_ERROR) {
                Log::error(TRACE_MEDIAPLAYBACK, "AudioESPlayerNative::decoderTask: "
                           "getNextMediaSample returns STREAM_ERROR. ");
                errorReported = true;
                continue;
            } else if (status == ERROR) {
                Log::error(TRACE_MEDIAPLAYBACK, "AudioESPlayerNative::decoderTask: "
                           "getNextMediaSample returns ERROR. ");
                errorReported = true;
                continue;
            } else {
                pSampleAttr = mSampleWriter->getSampleAttributes();
                err = decrypt(pSampleAttr, *mSampleWriter);
                if(!err.ok())
                {
                    NTRACE(TRACE_MEDIAPLAYBACK, "AudioESPlayer receives encrypted data. SDK "
                           "version 3.2 does not support concurrent decryption of more than "
                           "one elementary stream.  Audio stream should be clear.");
                    mCallback->reportError(err);
                    errorReported = true;
                    continue;
                }
            }
            consecutiveMediaSampleNotAvailable = 0;
            mAudioDecoder->setInputExhausted(false);
    
            if(pSampleAttr->getMediaAttributes() != 0)
            {
                // Update the audio decoder attributes after each discontinuity
                // (right after open() or flush()). This is because the selected
                // audio stream might be different after discontinuity, we must
                // update the corresponding audio attributes to the decoder.
                assert(pSampleAttr->getMediaAttributes()->mAudioAttributes);
                NTRACE(TRACE_MEDIAPLAYBACK, "AudioESPlayer::decoderTask: updating audio decoder attributes"
                       " because sample media attributes are non-null.");
    
                if(pSampleAttr->getMediaAttributes()->mAudioAttributes != NULL)
                {
                    /* Update Attributes : Codec context renew and decoder open with new codec context */
                    if(!mAudioDecoder->updateAttributes(pSampleAttr->getMediaAttributes()->mAudioAttributes))
                    {
#if 0
                        if(mAudioDecoder->getFlushState() != FLUSH_OFF){
                            NTRACE(TRACE_MEDIAPLAYBACK, "[%d] AudioESPlayerNative::%s ignoring updateAttribute while flushing"
                                   , mPipelineId, __func__);
                            continue;
                        }
    
                        if(closeThreadsFlagIsSet() == true){
                            NTRACE(TRACE_MEDIAPLAYBACK, "[%d] AudioESPlayerNative::%s ignoring updateAttribute err while closing"
                                   , mPipelineId, __func__);
                            return;
                        }
#endif
    
                        err = NFErr_OK;
                        assert(false);
                        std::ostringstream ss;
                        ss << "AudioESPlayerNative::decoderTask: error from updateAttributes";
                        ullong deviceSpecificErroCode = skeletonDeviceSpecific_UpdateAttributesFailed;
                        Variant errInfo;
                        errInfo["errorDescription"] = ss.str();
                        err.push(new IDeviceError(UNEXPECTED_MEDIA_ATTRIBUTES, deviceSpecificErroCode, errInfo));
                        mCallback->reportError(err);
                        errorReported = true;
    
                        NTRACE(TRACE_MEDIAPLAYBACK, "%s", ss.str().c_str());
                        continue;
                    }
#if 0
                    // Reset audio device based on the audio sampling frequency of
                    // the selected audio stream.
                    //
                    // Partner should check the current audio settings, if they have
                    // changed, then they need to reconfigure their decoder.
                    //
                    // This is an optional feature.  The audio attributes for a stream
                    // should stay the same.
                    mAudioRenderer->updateSamplingRate(
                            pSampleAttr->getMediaAttributes()->mAudioAttributes->mSamplesPerSecond);
#endif

                    /* Attach a source buffer */
                    if(!bAddSource)
                    {
                        /* Get the audio attributes */
                        nCsoAudioCodec = 0;
                        bRet = getAudioCodecFromAttributes(pSampleAttr, &nCsoAudioCodec);
                        if(!bRet)
                        {
                            Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [AudioESPlayerNative::%s:%d] Getting audio codec failed\n", __func__, __LINE__);
                            continue;
                        }

                        /* Add source buffer to CSO_UVA */
                        csoErr = CSO_UVA_AddSource(m_session_id, NETFLIX_AUDIO_SOURCE_BUFFER_ID, CSO_MEDIATYPE_AUDIO, nCsoAudioCodec, &src_buf);
                        if(csoErr != CSO_ERR_OK)
                        {
                            Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [AudioESPlayerNative::%s:%d] Adding source buffer failed\n", __func__, __LINE__);
                            continue;
                        }
                        else
                        {
                            bRet = setSourceBuffer(src_buf);
                            if(!bRet)
                            {
                                Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [AudioESPlayerNative::%s:%d] Setting source buffer failed\n", __func__, __LINE__);
                            }
                            bAddSource = true;

                            AVStatusManager::MutexLock();
                            /* need a mutex to sync */
                            hStatusManager->setAudioSourceBufferFlag(true);
                            AVStatusManager::MutexUnlock();
                        }
                    }
                    else
                    {
                        bRet = getAudioCodecFromAttributes(pSampleAttr, &nCurrentCsoAudioCodec);
                        if(!bRet)
                        {
                            Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [AudioESPlayerNative::%s:%d] Getting audio codec failed\n", __func__, __LINE__);
                            continue;
                        }

                        if(nCsoAudioCodec != nCurrentCsoAudioCodec)
                        {
                            Log::info(TRACE_MEDIAPLAYBACK, "@@@@@ [AudioESPlayerNative::%s:%d] Audio codec has been changed, (%d) -> (%d)\n", __func__, __LINE__, nCsoAudioCodec, nCurrentCsoAudioCodec);
                            nCsoAudioCodec = nCurrentCsoAudioCodec;
                            
                            AVStatusManager::MutexLock();
                            bAudioCodecSwitchStatus = hStatusManager->getAudioCodecSwitchStatus();
                            if(!bAudioCodecSwitchStatus)
                            {
                                bAudioCodecSwitchStatus = true;
                                hStatusManager->setAudioCodecSwitchStatus(bAudioCodecSwitchStatus);
                            }
                            AVStatusManager::MutexUnlock();

                            csoErr = CSO_UVA_SRCBUF_SetCodec(getSourceBuffer(), nCsoAudioCodec);
                            if(csoErr != CSO_ERR_OK)
                            {
                                Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [AudioESPlayerNative::%s:%d] Failed to set an audio codec (%d), csoErr = %d\n", __func__, __LINE__, nCsoAudioCodec, csoErr);
                            }
                            
                            /* Invoke PlaybackGroupNative::audioCodecSwitch() */
                            mPlaybackGroup->audioCodecSwitch(nCsoAudioCodec);
                        }
                    }
                }
            }
        } /* if (!bResend) */

        if(pSampleAttr){
            NTRACE(TRACE_MEDIAPLAYBACK, "[%d] AudioESPlayerNative::%s decode pts %lld, dts %lld"
                   , mPipelineId, __func__, pSampleAttr->getPTS(), pSampleAttr->getDTS());
        }

        AVStatusManager::MutexLock();
        if(hStatusManager->getAudioCodecSwitchStatus())
        {
            Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [AudioESPlayerNative::%s:%d] Now, player is switching an audio codec..\n", __func__, __LINE__);
            AVStatusManager::MutexUnlock();
            bResend = true;
            Thread::Sleep(WAIT_FOR_SETTING_PLAY_STATUS);
            continue;
        }
        
        /* If playback doesn't start to play, then this task doesn't send ES Data to decode */
        if(hStatusManager->getPlaybackStatus() != AVStatusMgr_PlaybackStatus_Playing)
        {
            eFlushStatus = hStatusManager->getFlushStatus();
            AVStatusManager::MutexUnlock();
            if(eFlushStatus != AVStatusMgr_FlushStatus_None)
            {
                bResend = false;
            }
            else
            {
                bResend = true;
                Thread::Sleep(WAIT_FOR_SETTING_PLAY_STATUS);
                continue;
            }
        }
        else
        {
            AVStatusManager::MutexUnlock();
            bResend = false;
        }
		if(!bGotFrame)
		{
			if(mAudioDecoder->decode(pSampleAttr,
	                                 mSampleWriter->getBuffer(),
	                                 mSampleWriter->getSampleSize(),
	                                 &m_stUVAWriteFrame,
	                                 decodedAudioBuffer))
	        {
	            // push buffers down to audio decoder here.
	            //sampleWriter.getBuffer()  pointer to the buffer
	            //sampleWriter.getSampleSize()  size of the sample
#if 0
	            if(!mAudioRenderer->sendSampleBuffer(decodedAudioBuffer) == true)
#endif
	            {
	                if(m_stUVAWriteFrame.data != NULL)
	                {
	                    bGotFrame = true;
	                }
	                // The frame was dropped by the renderer.  Log it!
	                mCallback->updateRenderingStats(0,1,IESPlayerCallback::STAT_NOT_SET);
	            }
	        }
	        else
	        {    
	        	Log::error(TRACE_MEDIAPLAYBACK, "Error AudioESPlayerNative::decoderTask: decode returns false.");

	            /* We don't call mCallback->reportError() here so that an issue
	             * decoding one audio frame doesn't force the UI to unload the
	             * movie.  We continue decoding and try recover on the next
	             * sample.*/
#if 0
	            if(mAudioDecoder->getFlushState() != FLUSH_OFF){
	                NTRACE(TRACE_MEDIAPLAYBACK, "[%d] AudioESPlayerNative::%s ignoring decode error while flushing"
	                       , mPipelineId, __func__);
	                continue;
	            }

	            if(closeThreadsFlagIsSet() == true){
	                NTRACE(TRACE_MEDIAPLAYBACK, "[%d] AudioESPlayerNative::%s ignoring decode error while closing"
	                       , mPipelineId, __func__);
	                return;
	            }
#endif
	        }
		}

        AVStatusManager::MutexLock();
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
                Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [AudioESPlayerNative::%s:%d] Writing audio data failed\n", __func__, __LINE__);
            }
            else
            {
                bGotFrame = false;
                setLastCurrentAudioPts(m_stUVAWriteFrame.pts);
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
                    Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [AudioESPlayerNative::%s:%d] Writing audio data failed\n", __func__, __LINE__);
                }
                else
                {
                    bGotFrame = false;
                    setLastCurrentAudioPts(m_stUVAWriteFrame.pts);
                }
            }
			else if((bGotFrame) &&
					(eFlushStatus != AVStatusMgr_FlushStatus_None) && 
					(hStatusManager->getDecodingStatus() != AVStatusMgr_DecodingStatus_Decoding))
			{
                AVStatusManager::MutexUnlock();
				bResend = true;
				Thread::Sleep(WAIT_FOR_AUDIO_DATA);
				continue;
			}
            else
            {
                AVStatusManager::MutexUnlock();
            }
        }

        // if possible, partner should report dropped frames through this method
        reportDecodeOutcome(mAudioDecoder->getStatsCounter());
    }

    if(src_buf != NULL)
    {
        csoErr = CSO_UVA_RemoveSource(m_session_id, src_buf);
        if(csoErr != CSO_ERR_OK)
        {
            Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [AudioESPlayerNative::%s:%d] Failed to remove a audio source buffer, csoErr = %d\n", __func__, __LINE__, csoErr);
        }
        src_buf = NULL;
    }

func_done:    
    NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [AudioESPlayerNative::%s:%d] %s() (-)\n", __func__, __LINE__, __func__);    
    return;
}

bool AudioESPlayerNative::getAudioCodecFromAttributes(const ISampleAttributes *pSampleAttr, int32_t *pnCsoAudioCodec)
{
    bool bRet = true;
    const MediaAttributes *pMediaAttr = NULL;
    unsigned int unCodecTag = 0;

    NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [AudioESPlayerNative::%s:%d] %s() (+)\n", __func__, __LINE__, __func__);    
    
    if(pSampleAttr == NULL)
    {
        Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [AudioESPlayerNative::%s:%d] pSampleAttr is NULL\n", __func__, __LINE__);
        bRet = false;            
        goto func_done;
    }

    if(pnCsoAudioCodec == NULL)
    {
        Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [AudioESPlayerNative::%s:%d] pnCsoAudioCodec is NULL\n", __func__, __LINE__);
        bRet = false;    
        goto func_done;
    }

    pMediaAttr = pSampleAttr->getMediaAttributes();
    if(pMediaAttr == NULL)
    {
        Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [AudioESPlayerNative::%s:%d] pMediaAttr is NULL\n", __func__, __LINE__);
        bRet = false;
        goto func_done;
    }
    if(pMediaAttr->mAudioAttributes == NULL)
    {
        Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [AudioESPlayerNative::%s:%d] pMediaAttr->mAudioAttributes is NULL\n", __func__, __LINE__);
        bRet = false;
        goto func_done;
    }

    unCodecTag = MediaUtils::getFourCCFromCodecParam(pMediaAttr->mAudioAttributes->mCodecParam);
    switch(unCodecTag)
    {
        case 0x1610:    /* MEDIASUBTYPE_MPEG_HEAAC */
            *pnCsoAudioCodec = CSO_ACODEC_AAC;    /* ?? */
            break;
        case 0xeac3:    /* DOLBY DIGITAL PLUS */
            *pnCsoAudioCodec = CSO_ACODEC_AC3_PLUS;
            break;
        case 0x6771:    /* OGG VORBIS 3 PLUS */
            break;
        default:
            break;
    }
    
func_done:
    NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [AudioESPlayerNative::%s:%d] %s() (-)\n", __func__, __LINE__, __func__);    
    return bRet;
}

bool AudioESPlayerNative::readyForPlaybackStart()
{
    AVStatusManager *hStatusManager = NULL;

    {
        // The decoder thread sets mEndOfStreamFlag. This method is called on
        // the SDK's pumping loop thread. We need the mutex to read it.
        ScopedMutex cs(mDecoderTaskMutex);
        if(mEndOfStreamFlag)
        {
            return true;
        }
    }

    // PARTNER: Partners should set this flag when the decoder has enough buffer
    hStatusManager = AVStatusManager::getInstance();
    if(hStatusManager == NULL)
    {
        Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [AudioESPlayerNative::%s:%d] hStatusManager is NULL\n", __func__, __LINE__);
        return false;
    }
    
    if(hStatusManager->getAudioSourceBufferFlag())
    {
        return true;
    }
    
#if 0
    int64_t videoPTS_snapshot, tsAtHeadOfRenderQueue, tsOfLastDeliveredSample, renderBacklogInMs;

    videoPTS_snapshot = mPlaybackGroup->getReferenceClock()->getVideoPresentationTime();
    #if 0
    mAudioRenderer->getTimestampOfNextSampleToRender(tsAtHeadOfRenderQueue);
    mAudioRenderer->getTimestampOfLastDeliveredSample(tsOfLastDeliveredSample);
    #endif
    if(videoPTS_snapshot == INVALID_TIMESTAMP)
    {
        renderBacklogInMs = tsOfLastDeliveredSample - tsAtHeadOfRenderQueue;
    } else
    {
        renderBacklogInMs = tsOfLastDeliveredSample -
            (videoPTS_snapshot > tsAtHeadOfRenderQueue ? videoPTS_snapshot : tsAtHeadOfRenderQueue);
    }

    if(renderBacklogInMs > ESPlayerConstants::AUDIO_PLAYBACK_START_THRESHOLD_IN_MS)
    {
        return true;
    }
    NTRACE(TRACE_MEDIAPLAYBACK,
           "audio %s return false : renderBacklogInMs %lld, videoPTS_snapshot %lld, lastDeliveredSample %lld, headOfRenderQueue %lld"
           , __func__, renderBacklogInMs, videoPTS_snapshot, tsOfLastDeliveredSample, tsAtHeadOfRenderQueue);
#endif
    return false;
}

// If the AudioESPlayer is enabled and the playback state is play, we are
// probably coming out of an on-the-fly audio switch. In this case, set playback
// to pending so that we don't actually try to start rendering media until we
// are sure we've buffered enough decoded frames that we are unlikely to
// immediately underflow at the renderer. Each time through the decoder loop we
// will check whether we are ready to move out of the pending state and begin
// playing back.
void AudioESPlayerNative::enable()
{
    ESPlayerNative::enable();
}

void AudioESPlayerNative::flush()
{
    CSO_ERR csoErr = CSO_ERR_OK;
    AVStatusManager *hStatusManager = NULL;
    bool bCsoAudioFlushStatus = false;

    if(mPlaybackGroup->getPlaybackState() ==  IPlaybackGroup::PLAY)
    {
        NTRACE(TRACE_MEDIACONTROL,
                   "On-the-fly audio track switch in progress, setPlaybackPending");
        setPlaybackPending(true);
    }

    beginFlush();
    /* UVA audio source buffer flush */
    csoErr = CSO_UVA_SRCBUF_AudioFlush(getSourceBuffer());
    if(csoErr != CSO_ERR_OK)
    {
        Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [AudioESPlayerNative::%s:%d] Failed to flush a audio buffer\n", __func__, __LINE__);
    }
    else
    {    
        AVStatusManager::MutexLock();
        hStatusManager = AVStatusManager::getInstance();
        if(hStatusManager == NULL)
        {
            Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [AudioESPlayerNative::%s:%d] hStatusManager is NULL\n", __func__, __LINE__);
        }
        else
        {
            bCsoAudioFlushStatus = hStatusManager->getCsoAudioFlushStatus();
            if(!bCsoAudioFlushStatus)
            {
                bCsoAudioFlushStatus = true;
                hStatusManager->setCsoAudioFlushStatus(bCsoAudioFlushStatus);
            }
        }
        AVStatusManager::MutexUnlock();
        mPlaybackGroup->audioFlushed();
    }

    endFlush();
}

void AudioESPlayerNative::beginFlush()
{
    setFlushAndWait(mAudioDecoder);
#if 0
    setFlushAndWait(mAudioRenderer);
#endif
}

void AudioESPlayerNative::endFlush()
{
#if 0
    mAudioRenderer->setFlushState(FLUSH_OFF);
#endif
    mAudioDecoder->setFlushState(FLUSH_OFF);
}

bool AudioESPlayerNative::inputsAreExhausted()
{
    if(mAudioDecoder->isInputExhausted() /*&& mAudioRenderer->isInputExhausted()*/)
    {
        // return true here when the audio decoder buffer is exhausted
        return true;
    }
    return false;
}

MediaType AudioESPlayerNative::getMediaType()
{
    return AUDIO;
}

void AudioESPlayerNative::underflowReporter()
{
    if(!playbackPending())
    {
        ESPlayerNative::underflowReporter();
    }
}

bool AudioESPlayerNative::isDisabledOrPending()
{
     ScopedMutex cs(mDecoderTaskMutex);
     return mDisabled || mPlaybackPending;
}

bool AudioESPlayerNative::playbackPending()
{
    ScopedMutex cs(mDecoderTaskMutex);
    return mPlaybackPending;
}

void AudioESPlayerNative::setPlaybackPending(bool val)
{
    ScopedMutex cs(mDecoderTaskMutex);
    mPlaybackPending = val;
}

void AudioESPlayerNative::setVolume(double target, uint32_t duration, IAudioMixer::Ease ease)
{
	AVStatusManager *hStatusManager = NULL;

	AVStatusManager::MutexLock();
	
	hStatusManager = AVStatusManager::getInstance();
	if(hStatusManager == NULL)
	{
		Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [AudioESPlayerNative::%s:%d] hStatusManager is NULL\n", __func__, __LINE__);
		goto func_exit;
	}

	hStatusManager->setFade(target, duration, (AVStatusMgr_Ease_e)ease);

func_exit:
	AVStatusManager::MutexUnlock();
	return;
}

double AudioESPlayerNative::getVolume()
{
	double dblVolume = 0.0;
	AVStatusManager *hStatusManager = NULL;

	AVStatusManager::MutexLock();
	
	hStatusManager = AVStatusManager::getInstance();
	if(hStatusManager == NULL)
	{
		Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [AudioESPlayerNative::%s:%d] hStatusManager is NULL\n", __func__, __LINE__);
		goto func_exit;
	}

	dblVolume = hStatusManager->getCurrentVolume();
	
func_exit:
	AVStatusManager::MutexUnlock();
	return dblVolume;
}

double AudioESPlayerNative::getLastCurrentAudioPts()
{
    ScopedMutex cs(mDecoderTaskMutex);
    return m_dblLastCurAudioPts;
}

void AudioESPlayerNative::setLastCurrentAudioPts(double dblCurPts)
{
    ScopedMutex cs(mDecoderTaskMutex);
    m_dblLastCurAudioPts = dblCurPts;
}

