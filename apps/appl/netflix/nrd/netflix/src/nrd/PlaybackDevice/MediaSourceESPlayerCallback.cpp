/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#include "MediaSourcePlaybackDevice.h"
#include "DeviceConstants.h"
#include <nrdbase/Log.h>
#include <nrdbase/ScopedMutex.h>

using namespace netflix;
using namespace netflix::device;
using namespace netflix::device::esplayer;


MediaSourcePlaybackDevice::ESPlayerCallback::~ESPlayerCallback()
{
}

MediaSourcePlaybackDevice::ESPlayerCallback::ESPlayerCallback(
    MediaSourcePlaybackDevice& playbackDevice,
    esplayer::MediaType mediaType) :
  mMediaType(mediaType),
  mMediaSourcePlaybackDevice(playbackDevice),
  mDisabled(true),
  mUnderflowReported(false),
  mPlaybackComplete(false),
  mMutex(ESP_CALLBACK_MUTEX, "ESPlayer Callback Mutex")
{
}

esplayer::Status
MediaSourcePlaybackDevice::ESPlayerCallback::getNextMediaSample(esplayer::ISampleWriter& writer)
{
    //ScopedMutex cs(mMutex);

    if( !(mMediaSourcePlaybackDevice.getMediaSource().get()) || mDisabled ) {
        return esplayer::NO_AVAILABLE_SAMPLES;
    }

    // access MediaSource to retrieve the Sample
    esplayer::Status status = mMediaSourcePlaybackDevice.getMediaSource()->getNextSample(mMediaType,
                                                                                         writer,
                                                                                         mSampleAttributes);
    // report error up to the playback device
    if(status == STREAM_ERROR){
        reportError(esplayer::STREAM_ERROR,
                    mMediaType + 1,
                    "getNextMediaSample() encountered an error.");
    }

    if(status == SAMPLE_WRITER_ERROR){
        reportError(esplayer::SAMPLE_WRITER_ERROR,
                    mMediaType + 1,
                    "getNextMediaSample() encountered an sample writer error.");
    }

    return status;
}

esplayer::Status
MediaSourcePlaybackDevice::ESPlayerCallback::getNextMediaSampleSize(uint32_t& sampleSize,
                                                                    uint32_t viewNum)
{
    esplayer::Status status = OK;

    // Since mDisabled is set by the pumping thread and this is a device thread,
    // get the mutex.
    ScopedMutex cs(mMutex);

    if( !(mMediaSourcePlaybackDevice.getMediaSource().get()) || mDisabled ) {
        return esplayer::NO_AVAILABLE_SAMPLES;
    }

    // access MediaSource to retrieve the Sample
    status = mMediaSourcePlaybackDevice.getMediaSource()->peekNextSampleAttributes(mMediaType,
                                                                                   mSampleAttributes);

    if( status == esplayer::OK ){
        sampleSize = mSampleAttributes.getSize(viewNum);
    } else if(status == STREAM_ERROR){
        reportError(esplayer::STREAM_ERROR,
                    mMediaType + 1,
                    "getNextMediaSampleSize() encountered an stream error.");
    }

    return status;
}

esplayer::Status
MediaSourcePlaybackDevice::ESPlayerCallback::getNextMediaSampleTimestamps(llong& dts,
                                                                          llong& pts)
{
    esplayer::Status status = OK;

    // Since mDisabled is set by the pumping thread and this is a device thread,
    // get the mutex.

    if( !(mMediaSourcePlaybackDevice.getMediaSource().get()) || mDisabled ) {
        return esplayer::NO_AVAILABLE_SAMPLES;
    }

    // access MediaSource to retrieve the Sample
    status = mMediaSourcePlaybackDevice.getMediaSource()->peekNextSampleAttributes(mMediaType,
                                                                                   mSampleAttributes);

    // TODO: MVC_SPLIT handling
    if( status  == esplayer::OK ){
        pts = mSampleAttributes.getPTS();
        dts = mSampleAttributes.getDTS();

    }

    return status;
}

void MediaSourcePlaybackDevice::ESPlayerCallback::updatePlaybackPosition(const llong& timestamp)
{
    ScopedMutex cs(mMutex);

    if(!mDisabled)
    {
        // Report video pts updates only to prevent us from needing a mutex in
        // MediaSourcePlaybackDevice::positionUpdatedReported().
        if(mMediaType == esplayer::VIDEO)
        {
            // The MediaSourcePlaybackDevice gets the postEventMutex inside this call. The
            // MediaSourcePlaybackDevice never holds that mutex when it calls into
            // ESPlayerCallback so it shouldn't cause deadlock.
            mMediaSourcePlaybackDevice.positionUpdateReported(timestamp, Time::mono().ms());
        }
    }
}


void MediaSourcePlaybackDevice::ESPlayerCallback::reportUnderflow()
{
    ScopedMutex cs(mMutex);

    if(!mDisabled)
    {
        NTRACE(TRACE_MEDIAPLAYBACK, "ESPlayerCallback:reportUnderflow. MediaType: %s",
               mMediaType == esplayer::AUDIO ?  "AUDIO" : "VIDEO");

        if(!mUnderflowReported)
        {
            if(mMediaSourcePlaybackDevice.reportUnderflow() == true)
            {
                mUnderflowReported = true;
            }
        }
    }
}

void MediaSourcePlaybackDevice::ESPlayerCallback::resetUnderflowReported()
{
    ScopedMutex cs(mMutex);
    mUnderflowReported = false;
}

bool MediaSourcePlaybackDevice::ESPlayerCallback::reportUnderflowReceived()
{
    ScopedMutex cs(mMutex);
    return mUnderflowReported;
}

void MediaSourcePlaybackDevice::ESPlayerCallback::playbackCompleted()
{
    ScopedMutex cs(mMutex);
    if(!mDisabled)
    {
        NTRACE(TRACE_MEDIAPLAYBACK, "MediaSourcePlaybackDevice::ESPlayerCallback::playbackCompleted() "
               " MediaType: %s", mMediaType == esplayer::AUDIO ?  "AUDIO" : "VIDEO");

        if(!mPlaybackComplete)
        {
            mMediaSourcePlaybackDevice.playbackCompleteReported(mMediaType);
            mPlaybackComplete = true;
        }
    }
}

bool MediaSourcePlaybackDevice::ESPlayerCallback::playbackCompletedReceived()
{
    ScopedMutex cs(mMutex);
    return mPlaybackComplete;
}

void MediaSourcePlaybackDevice::ESPlayerCallback::resetPlaybackCompleted()
{
    ScopedMutex cs(mMutex);
    mPlaybackComplete = false;
}

void MediaSourcePlaybackDevice::ESPlayerCallback::reportError(esplayer::Status errorCategory,
                                                              ullong errorCode,
                                                              const std::string& errorMessage)
{
    Log::error(TRACE_MEDIAPLAYBACK, "MediaSourcePlaybackDevice::ESPlayerCallback: Stream player reports "
               "error. MediaType: %s, category: %d, code: %llx, message: %s",
               mMediaType == esplayer::AUDIO ? "audio" : "video",
               static_cast<int>(errorCategory),
               errorCode,
               errorMessage.c_str());
    mMediaSourcePlaybackDevice.errorReported(errorCategory, errorCode, errorMessage);
}

void MediaSourcePlaybackDevice::ESPlayerCallback::reportError(IDeviceError err)
{
    reportError(err.getCategoryCode(),
                err.getDeviceSpecificErrorCode(),
                err.getDeviceSpecificErrorInfoStr());
}

void MediaSourcePlaybackDevice::ESPlayerCallback::reportError(NFErr err)
{
    shared_ptr<const NFError> error = err.find(NFErr_PlaybackDeviceError);
    if( error.get()){
        IDeviceError *derr = reinterpret_cast<IDeviceError*> (const_cast<NFError*> (error.get()));
        if( derr ){
            esplayer::Status status = derr->getCategoryCode();
            ullong dserr = derr->getDeviceSpecificErrorCode();
            std::string errMsg = derr->getDeviceSpecificErrorInfoStr();
            reportError(status, dserr, errMsg);
        }
    }
}

void MediaSourcePlaybackDevice::ESPlayerCallback::beginFlush()
{
    ScopedMutex cs(mMutex);
    mDisabled = true;
}

void MediaSourcePlaybackDevice::ESPlayerCallback::endFlush()
{
    ScopedMutex cs(mMutex);
    mDisabled = true;
    assert(mDisabled == true);
    mUnderflowReported = false;
    mPlaybackComplete = false;
    mDisabled = false;
}

esplayer::MediaType MediaSourcePlaybackDevice::ESPlayerCallback::getMediaType()
{
    return mMediaType;
}
void MediaSourcePlaybackDevice::ESPlayerCallback::disable()
{
    ScopedMutex cs(mMutex);
    mDisabled = true;
}

void MediaSourcePlaybackDevice::ESPlayerCallback::enable()
{
    ScopedMutex cs(mMutex);
    mDisabled = false;
}

void MediaSourcePlaybackDevice::ESPlayerCallback::updateDecodingStats(int numDecoded,
                                                                      int numSkipped,
                                                                      int numReducedQualityDecodes,
                                                                      int numErrors)
{
    ScopedMutex cs(mMutex);

    if(numDecoded != STAT_NOT_SET) {
        if(mStatTotals.mNumDecoded == STAT_NOT_SET)
        {
            mStatTotals.mNumDecoded = numDecoded;
        } else {
            mStatTotals.mNumDecoded += numDecoded;
        }
    }
    if(numSkipped != STAT_NOT_SET) {
        if(mStatTotals.mNumSkipped == STAT_NOT_SET)
        {
            mStatTotals.mNumSkipped = numSkipped;
        } else {
            mStatTotals.mNumSkipped += numSkipped;
        }
    }
    if(numReducedQualityDecodes != STAT_NOT_SET){
        if(mStatTotals.mNumReducedQualityDecodes == STAT_NOT_SET){
            mStatTotals.mNumReducedQualityDecodes = numReducedQualityDecodes;
        } else {
            mStatTotals.mNumReducedQualityDecodes +=
                numReducedQualityDecodes;
        }
    }
    if(numErrors != STAT_NOT_SET){
        if(mStatTotals.mNumDecoderErrors == STAT_NOT_SET) {
            mStatTotals.mNumDecoderErrors = numErrors;
        } else {
            mStatTotals.mNumDecoderErrors += numErrors;
        }
    }
}

void MediaSourcePlaybackDevice::ESPlayerCallback::updateRenderingStats(int numRendered,
                                                                       int numDropped,
                                                                       int numErrors)
{
    ScopedMutex cs(mMutex);

    if(numRendered != STAT_NOT_SET) {
        if(mStatTotals.mNumRendered == STAT_NOT_SET)
        {
            mStatTotals.mNumRendered = numRendered;
        } else {
            mStatTotals.mNumRendered += numRendered;
        }
    }
    if(numDropped != STAT_NOT_SET) {
        if(mStatTotals.mNumDropped == STAT_NOT_SET)
        {
            mStatTotals.mNumDropped = numDropped;
        } else {
            mStatTotals.mNumDropped += numDropped;
        }
    }
    if(numErrors != STAT_NOT_SET){
        if(mStatTotals.mNumRendererErrors == STAT_NOT_SET) {
            mStatTotals.mNumRendererErrors = numErrors;
        } else {
            mStatTotals.mNumRendererErrors += numErrors;
        }
    }
}

void MediaSourcePlaybackDevice::ESPlayerCallback::getPlaybackQualityStats(IPlaybackDevice::PlaybackQualityStats& stats)
{
    ScopedMutex cs(mMutex);

    stats = mStatTotals;
}
