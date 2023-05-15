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
#include "ESPlayerConstants.h"
#include "DeviceThread.h"
#include "SampleWriterNative.h"
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
}

void AudioESPlayerNative::close()
{
    setCloseThreadsFlag();
    ESPlayerNative::close();
}

AudioESPlayerNative::AudioESPlayerNative() :
    mPlaybackPending(false),
    mInputExhausted(false),
    mIsFlushing(false)
{
}

NFErr
AudioESPlayerNative::init(const struct StreamPlayerInitData& initData,
                          shared_ptr<IESPlayerCallback> callback,
                          PlaybackGroupNative* playbackGroup)
{
    NFErr err = NFErr_OK;
    ESPlayerNative::init(initData, callback, playbackGroup);
    uint32_t samplingRate = 48000;
    NRDP_UNUSED(samplingRate);
    if(initData.mInitialStreamAttributes->mAudioAttributes->mSamplesPerSecond != 0)
    {
        samplingRate = initData.mInitialStreamAttributes->
            mAudioAttributes->mSamplesPerSecond;
    }
    mSampleWriter.reset(new SampleWriterNative(NOT_3D, callback));
    mAudioDecoderThread.reset(new DeviceThread(*this, &ESPlayerNative::decoderTask,
                                               &THREAD_SKELETON_DPI_AUDIO_DECODER));

    return err;
}

void AudioESPlayerNative::decoderTask()
{
    static const Time WAIT_FOR_AUDIO_SAMPLE_BUFFER(30);
    static const Time WAIT_FOR_AUDIO_DATA(30);
    static const Time WAIT_WHILE_IDLING(100);


    Status status;
    const ISampleAttributes* pSampleAttr;
    DecodedAudioBuffer decodedAudioBuffer;
    bool errorReported = false;
    NFErr err = NFErr_OK;

    while(closeThreadsFlagIsSet() == false)
    {
        if(mIsFlushing)
        {
            {
                ScopedMutex cs(mDecoderTaskMutex);
                mInputExhausted = true;
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

        status = mCallback->getNextMediaSample(*mSampleWriter);
        if(status == NO_AVAILABLE_SAMPLES)
        {
            mInputExhausted = true;
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
            mInputExhausted = true;
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
                Log::error(TRACE_MEDIAPLAYBACK, "AudioESPlayer receives encrypted data. SDK "
                       "version 3.2 does not support concurrent decryption of more than "
                       "one elementary stream.  Audio stream should be clear.");
                mCallback->reportError(err);
                errorReported = true;
                continue;
            }
        }
        mInputExhausted = false;

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
                // Reset audio device based on the audio sampling frequency of
                // the selected audio stream.
                //
                // Partner should check the current audio settings, if they have
                // changed, then they need to reconfigure their decoder.
                //
                // This is an optional feature.  The audio attributes for a stream
                // should stay the same.
            }
        }

        if( /*Feed the decoder here*/ 1)
        {
            // push buffers down to audio decoder here.
            //sampleWriter.getBuffer()  pointer to the buffer
            //sampleWriter.getSampleSize()  size of the sample
        }
        else
        {
            Log::error(TRACE_MEDIAPLAYBACK, "Error AudioESPlayerNative::decoderTask: decode returns false.");
        }

        // if possible, partner should report dropped frames through this method
        // reportDecodeOutcome(mAudioDecoder->getStatsCounter());

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

    // PARTNER: Partners should set this flag when the decoder has enough buffer
    return true;
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
    mIsFlushing = true;
}

void AudioESPlayerNative::endFlush()
{
    mIsFlushing = false;
}

bool AudioESPlayerNative::inputsAreExhausted()
{
    if(mInputExhausted)
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
