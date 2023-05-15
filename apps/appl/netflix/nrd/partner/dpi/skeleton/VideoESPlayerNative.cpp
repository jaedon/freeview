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
#include "PlaybackGroupNative.h"
#include "ESPlayerConstants.h"
#include "DeviceThread.h"
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
    mVideoDecoder.reset();         // Thread destructors wait for threads to exit.
    mVideoDecoderThread.reset();   // We want to ensure decoder is cleaned up because
                                   // it uses ES player resources
}

static const IPlaybackGroup::Rect zeroRect = {0, 0, 0, 0};

VideoESPlayerNative::VideoESPlayerNative() : mCurrent3DFormat(NOT_3D),
                                             mTransitionStartTime(0),
                                             mTransitionEndTime(0),
                                             mSrcVideoWindow(zeroRect),
                                             mDstVideoWindow(zeroRect),
                                             mCurrentVideoWindow(zeroRect),
                                             mInputExhausted(false),
                                             mIsFlushing(false)
{
}

void VideoESPlayerNative::close()
{
    setCloseThreadsFlag();
    mVideoDecoderThread.reset();  // Thread destructors wait for threads to exit.
    ESPlayerNative::close();
}

NFErr VideoESPlayerNative::init(const struct StreamPlayerInitData& initData,
                                shared_ptr<IESPlayerCallback> callback,
                                PlaybackGroupNative *playbackGroup)

{
    NFErr err;
    ullong deviceSpecificErroCode;
    ESPlayerNative::init(initData, callback, playbackGroup);

    // Initialize the sample writer that we'll hand to the SDK to get access
    // units of encoded video. If this is MVC and we've requested MVC_SPLIT
    // initialize the sample writer to handle two views.
    mCurrent3DFormat = initData.mInitialStreamAttributes->
        mVideoAttributes->mFormat3D;
    mSampleWriter.reset(new SampleWriterNative(mCurrent3DFormat, callback));


    // Initialize decoder
    mVideoDecoder.reset(new VideoDecoder(this, callback));

    // Start the decoder and render threads.
    mVideoDecoderThread.reset(
        new DeviceThread(*this, &ESPlayerNative::decoderTask, &THREAD_SKELETON_DPI_VIDEO_DECODER));

     return err;
}

void VideoESPlayerNative::decoderTask()
{
    static const Time WAIT_FOR_VIDEO_FRAME_BUFFER(30);
    static const Time WAIT_FOR_VIDEO_DATA(30);
    static const Time WAIT_WHILE_IDLING(100);

    const ISampleAttributes *pSampleAttr = NULL;
    bool errorReported = false;
    NFErr err = NFErr_OK;

    while(closeThreadsFlagIsSet() == false)
    {
        if(errorReported)
        {
            Thread::Sleep(WAIT_WHILE_IDLING);
            continue;
        }

        if (mEndOfStreamFlag)
        {
            Thread::Sleep(WAIT_WHILE_IDLING);
            continue;
        }

        // Since our fake decoder has not Q, don't feed it data
        if (mPlaybackGroup->getPlaybackState() == IPlaybackGroup::PAUSE)
        {
            Thread::Sleep(WAIT_WHILE_IDLING);
            continue;
        }

        Status status;

        // Check if any access-unit data is available to decode.
        status = mCallback->getNextMediaSample(*mSampleWriter);
        if(status == NO_AVAILABLE_SAMPLES)
        {
            pSampleAttr = NULL;
            mInputExhausted = true;
            Thread::Sleep(WAIT_FOR_VIDEO_DATA);
            continue;
        } else if(status == NO_AVAILABLE_BUFFERS)
        {
            pSampleAttr = NULL;
            Thread::Sleep(WAIT_FOR_VIDEO_DATA);
            continue;
        } else if (status == END_OF_STREAM) {
            {
                ScopedMutex cs(mDecoderTaskMutex);
                mEndOfStreamFlag = true;
                mVideoDecoder->setEndOfStream(true);
            }
            mInputExhausted = true;
            mSampleWriter->reset();
            pSampleAttr = NULL; // As a precaution.  GetNextMediaSample
                                // should set it to NULL.
            NTRACE(TRACE_MEDIAPLAYBACK, "VideoESPlayerNative::decoderTask: "
                   "getNextMediaSample returns END_OF_STREAM");

        } else if(status == STREAM_ERROR) {
            Log::error(TRACE_MEDIAPLAYBACK, "VideoESPlayerNative::decoderTask: "
                       "getNextMediaSample returns STREAM_ERROR. ");
            errorReported = true;
            continue;
        } else if (status == ERROR) {
            Log::error(TRACE_MEDIAPLAYBACK, "VideoESPlayerNative::decoderTask: "
                       "getNextMediaSample returns ERROR. ");
            errorReported = true;
            continue;
        } else {
            mInputExhausted = false;
            pSampleAttr = mSampleWriter->getSampleAttributes();

            err = decrypt(pSampleAttr, *mSampleWriter);
            if(!err.ok())
            {
                Log::error(TRACE_MEDIAPLAYBACK, "VideoESPlayerNative::decoderTask: error in decrypt.");
                mCallback->reportError(err);
                errorReported = true;
                continue;
            }

            // Partner should feed actual data (mSampleWriter->getBuffer()) into the decoder here
            mVideoDecoder->feedDecoder(pSampleAttr->getPTS());
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
    {
        ScopedMutex cs(mDecoderTaskMutex);
        // Make sure to return ready if we have already sent the last
        // access unit.
        if(mEndOfStreamFlag == true)
        {
            return true;
        }
    }

    // PARTNER: The reference application uses the current renderer state to
    // determine if we should return true here.  Partners will need to set
    // a flag in the decoder context when their decoder buffer has enough
    // data.  In this current state, we will never go ready!
    return true;
}

bool VideoESPlayerNative::inputsAreExhausted()
{
    return mInputExhausted;
}

void VideoESPlayerNative::flush()
{
    // Not implemented.  Video is only flushed via a flush on the entire
    // playback group.
    assert(0);
}

void VideoESPlayerNative::beginFlush()
{
    mIsFlushing = true;
}


void VideoESPlayerNative::endFlush()
{
    mIsFlushing = false;
    {
        ScopedMutex cs(mDecoderTaskMutex);
        mEndOfStreamFlag = false;
    }
    mVideoDecoder->setEndOfStream(false);
}


static inline int64_t now()
{
    return Time::serverTime().val();
}

IPlaybackGroup::Rect
VideoESPlayerNative::setVideoWindow(const IPlaybackGroup::Rect& targetRect, uint32_t transitionDuration)
{
    // No transition required
    mTransitionStartTime = 0;
    mTransitionEndTime = 0;
    mCurrentVideoWindow = targetRect;
    // PARTNER: Set the window here.

    NTRACE(TRACE_MEDIAPLAYBACK, "VideoESPlayerNative::setVideoWindow %d %d %d %d - %d => %d %d %d %d",
           targetRect.x, targetRect.y, targetRect.width, targetRect.height, transitionDuration,
           mCurrentVideoWindow.x, mCurrentVideoWindow.y, mCurrentVideoWindow.width, mCurrentVideoWindow.height);
    return mCurrentVideoWindow;
}

void VideoESPlayerNative::updateTransition()
{
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
}

MediaType VideoESPlayerNative::getMediaType()
{
    return VIDEO;
}

bool VideoESPlayerNative::closeThreadsFlagIsSet()
{
    return ESPlayerNative::closeThreadsFlagIsSet();
}

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////  Fake Decoder related code to simulate playback
///////////////////////////////////////////////////////////////////////////////////////

VideoESPlayerNative::VideoDecoder::VideoDecoder(VideoESPlayerNative* ESPlayer, shared_ptr<IESPlayerCallback> callback)
                                                : Thread(&THREAD_SKELETON_DPI_VIDEO_DECODER),
                                                  mESPlayer(ESPlayer),
                                                  mCallback(callback),
                                                  mVideoDecoderMutex(ESP_PLAYER_MUTEX, "VideoDecoder Mutex"),
                                                  mLastFedPts(0),
                                                  mEndOfStream(false)
{
    Start();
    WaitRunning();
}

VideoESPlayerNative::VideoDecoder::~VideoDecoder()
{
    Wait();
    // Any other cleanup?
}

void VideoESPlayerNative::VideoDecoder::Run()
{
    updatePtsTask();
}

void VideoESPlayerNative::VideoDecoder::setEndOfStream(bool eos)
{
    mEndOfStream = eos;
}

void VideoESPlayerNative::VideoDecoder::updatePtsTask()
{
    const Time UPDATE_PTS_FREQUENCY(150);   // update pts every 100 ms
    int64_t startTime = -1;
    int64_t endTime = -1;
    int64_t firstPts = -1;
    int64_t lastPts = -1;
    int64_t lastFedPts = -1;
    int64_t curr;
    IPlaybackGroup::PlaybackState currentState = IPlaybackGroup::PAUSE;

    while(!mESPlayer->closeThreadsFlagIsSet())
    {
        if (mESPlayer->mPlaybackGroup->getPlaybackState() == IPlaybackGroup::PLAY)
        {
            if (currentState == IPlaybackGroup::PAUSE)
            {
                // There has been a state change, reset everything
                currentState = IPlaybackGroup::PLAY;
                startTime = Time::serverTime().ms();
                firstPts = -1;
                lastPts = 0;
                endTime = 0;
                mEndOfStream = false;
            }

            {
                ScopedMutex cs(mVideoDecoderMutex);
                lastFedPts = mLastFedPts;
            }

            if (!mEndOfStream)
            {
                if (lastFedPts == -1)
                {
                    Thread::Sleep(UPDATE_PTS_FREQUENCY);
                    continue;
                }
                // store the first pts
                if (firstPts == -1)
                {
                    Thread::Sleep(UPDATE_PTS_FREQUENCY);
                    firstPts = lastFedPts;
                }

                // Keep the last pts and possible end time around
                if (lastPts < lastFedPts)
                {
                    lastPts = lastFedPts;
                    endTime = Time::serverTime().ms() - startTime + lastPts;
                }

                curr = Time::serverTime().ms() - startTime + firstPts;
                mCallback->updatePlaybackPosition(curr);
            }
            else
            {
                if (curr >= endTime)
                {
                    if (!mCallback->playbackCompletedReceived())
                    {
                        mESPlayer->underflowReporter();
                    }
                }
                else
                {
                    // "Drain" out the data.
                    curr = Time::serverTime().ms() - startTime + firstPts;
                    mCallback->updatePlaybackPosition(curr);
                }
            }
        }
        else
        {
            currentState = IPlaybackGroup::PAUSE;
        }
        Thread::Sleep(UPDATE_PTS_FREQUENCY);
    }
}

void VideoESPlayerNative::VideoDecoder::feedDecoder(int64_t pts)
{
    ScopedMutex cs(mVideoDecoderMutex);
    mLastFedPts = pts;
}
