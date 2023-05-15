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
#include "AudioRenderer.h"
#include "DeviceThread.h"
#include "SampleWriterNative.h"
#include "ReferenceClockNative.h"
#include <nrd/AppThread.h>
#include <nrdbase/Log.h>
#include <nrdbase/Time.h>
#include <nrdbase/Thread.h>
#include <nrdbase/ScopedMutex.h>
#include <nrdbase/NFErr.h>

using namespace netflix::device::esplayer;
using namespace netflix;

AudioESPlayerNative::~AudioESPlayerNative()
{
    setCloseThreadsFlag();
    mAudioDecoderThread.reset(); // Thread destructor waits for thread to exit.
    mAudioDecoder.reset();
}

void AudioESPlayerNative::close()
{
    setCloseThreadsFlag();
    mAudioDecoderThread.reset(); // Thread destructor waits for thread to exit.
    mAudioDecoder.reset();
    ESPlayerNative::close();
}

AudioESPlayerNative::AudioESPlayerNative() : mPlaybackPending(false)
{
}


NFErr AudioESPlayerNative::init(const struct StreamPlayerInitData& initData,
                                shared_ptr<IESPlayerCallback> callback,
                                PlaybackGroupNative* playbackGroup)
{
    NFErr err;
    ullong deviceSpecificErroCode = 0;
    ESPlayerNative::init(initData, callback, playbackGroup);
    uint32_t samplingRate = 48000;
    if(initData.mInitialStreamAttributes->mAudioAttributes->mSamplesPerSecond != 0)
    {
        samplingRate = initData.mInitialStreamAttributes->
            mAudioAttributes->mSamplesPerSecond;
    }

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

        deviceSpecificErroCode = refAppDeviceSpecific_AudioPlayerInitializationFailed;
        Variant errInfo;
        errInfo["errorDescription"] = "Error AudioESPlayerNative::init: could not create audio renderer";
        return err.push(new IDeviceError(INITIALIZATION_ERROR,
                                         deviceSpecificErroCode,
                                         errInfo));
    }

    mAudioDecoder = mPlaybackGroup->getESManager()->getDecoderManager()->
        createAudioDecoder(mAudioRenderer->getNumOutputChannels());
    if (!mAudioDecoder) {
        NTRACE(TRACE_MEDIAPLAYBACK, "Error AudioESPlayerNative::init: could not create audio decoder");

        deviceSpecificErroCode = refAppDeviceSpecific_AudioPlayerInitializationFailed;
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
    static const Time WAIT_FOR_AUDIO_DATA(100);
    static const Time WAIT_WHILE_IDLING(100);


    Status status;
    const ISampleAttributes* pSampleAttr;
    DecodedAudioBuffer decodedAudioBuffer;
    bool errorReported = false;
    NFErr err = NFErr_OK;
    uint32_t consecutiveMediaSampleNotAvailable = 0;

    while(closeThreadsFlagIsSet() == false)
    {
        if(mAudioDecoder->getFlushState() != FLUSH_OFF)
        {
            if(mAudioDecoder->getFlushState() == FLUSH_BEGIN)
            {
                mAudioDecoder->reset();
                mAudioDecoder->setInputExhausted(true);
                mAudioDecoder->setFlushState(ESPlayerNative::FLUSH_END);
                ScopedMutex cs(mDecoderTaskMutex);
                mEndOfStreamFlag = false;
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
        if(playbackPending() && readyForPlaybackStart())
        {
            setPlaybackPending(false);
        }

        if(decodedAudioBuffer.size() == 0)
        {
            if(!mAudioRenderer->getSampleBuffer(decodedAudioBuffer))
            {
                Thread::Sleep(WAIT_FOR_AUDIO_SAMPLE_BUFFER);
                continue;
            }
        }

        status = mCallback->getNextMediaSample(*mSampleWriter);
        if(status == NO_AVAILABLE_SAMPLES) {
            consecutiveMediaSampleNotAvailable++;
            // check consecutive NO_AVAILABLE_SAMPLES count to prevent premature underflow report
            if ( consecutiveMediaSampleNotAvailable > ESPlayerConstants::DECODER_INPUT_EXHAUSTION_THRESHOLD*10){
                NTRACE(TRACE_MEDIAPLAYBACK, "consecutive NO_AVAILABLE_SAMPLES %d "
                       , consecutiveMediaSampleNotAvailable);
                mAudioDecoder->setInputExhausted(true);
            }
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
                if(!mAudioDecoder->updateAttributes(pSampleAttr->getMediaAttributes()->mAudioAttributes))
                {
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

                    err = NFErr_OK;
                    assert(false);
                    std::ostringstream ss;
                    ss << "AudioESPlayerNative::decoderTask: error from updateAttributes";
                    ullong deviceSpecificErroCode = refAppDeviceSpecific_UpdateAttributesFailed;
                    Variant errInfo;
                    errInfo["errorDescription"] = ss.str();
                    err.push(new IDeviceError(UNEXPECTED_MEDIA_ATTRIBUTES, deviceSpecificErroCode, errInfo));
                    mCallback->reportError(err);
                    errorReported = true;

                    NTRACE(TRACE_MEDIAPLAYBACK, "%s", ss.str().c_str());
                    continue;
                }
                // Reset audio device based on the audio sampling frequency of
                // the selected audio stream.
                mAudioRenderer->updateSamplingRate(
                    pSampleAttr->getMediaAttributes()->mAudioAttributes->mSamplesPerSecond);
            }
        }

        if(pSampleAttr){
            NTRACE(TRACE_MEDIAPLAYBACK, "[%d] AudioESPlayerNative::%s decode pts %lld, dts %lld"
                   , mPipelineId, __func__, pSampleAttr->getPTS(), pSampleAttr->getDTS());
        }

        if(mAudioDecoder->getFlushState() != FLUSH_OFF){
            NTRACE(TRACE_MEDIAPLAYBACK, "[%d] AudioESPlayerNative::%s Abort decode  while flushing"
                   , mPipelineId, __func__);
            continue;
        }

        if(closeThreadsFlagIsSet() == true){
            NTRACE(TRACE_MEDIAPLAYBACK, "[%d] AudioESPlayerNative::%s Abort decode  while closing"
                   , mPipelineId, __func__);
            return;
        }

        if(mAudioDecoder->decode(pSampleAttr,
                                 mSampleWriter->getBuffer(),
                                 mSampleWriter->getSampleSize(),
                                 decodedAudioBuffer))
        {
            if(!mAudioRenderer->sendSampleBuffer(decodedAudioBuffer) == true)
            {
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
        }
        reportDecodeOutcome(mAudioDecoder->getStatsCounter());

    }
}

bool AudioESPlayerNative::readyForPlaybackStart()
{
    {
        // The decoder thread sets mEndOfStreamFlag. This method is called on
        // the SDK's pumping loop thread. We need the mutex to read it.
        ScopedMutex cs(mDecoderTaskMutex);
        if(mEndOfStreamFlag)
        {
            return true;
        }
    }

    int64_t videoPTS_snapshot, tsAtHeadOfRenderQueue, tsOfLastDeliveredSample, renderBacklogInMs;

    videoPTS_snapshot = mPlaybackGroup->getReferenceClock()->getVideoPresentationTime();
    mAudioRenderer->getTimestampOfNextSampleToRender(tsAtHeadOfRenderQueue);
    mAudioRenderer->getTimestampOfLastDeliveredSample(tsOfLastDeliveredSample);

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

    if(mPlaybackGroup->getPlaybackState() ==  IPlaybackGroup::PLAY)
    {
        NTRACE(TRACE_MEDIACONTROL,
                   "On-the-fly audio track switch in progress, setPlaybackPending");
        setPlaybackPending(true);
    }

    beginFlush();
    mPlaybackGroup->audioFlushed();
    endFlush();
}

void AudioESPlayerNative::beginFlush()
{
    setFlushAndWait(mAudioDecoder);
    setFlushAndWait(mAudioRenderer);
}

void AudioESPlayerNative::endFlush()
{
    mAudioRenderer->setFlushState(FLUSH_OFF);
    mAudioDecoder->setFlushState(FLUSH_OFF);
}

bool AudioESPlayerNative::inputsAreExhausted()
{
    if(mAudioDecoder->isInputExhausted() && mAudioRenderer->isInputExhausted())
    {
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
    mAudioRenderer->setFade(target, duration, ease);
}

double AudioESPlayerNative::getVolume()
{
    return mAudioRenderer->getCurrentVolume();
}
