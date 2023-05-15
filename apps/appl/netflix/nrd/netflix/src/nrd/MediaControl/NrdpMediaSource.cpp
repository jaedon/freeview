/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#include "NrdpMediaSource.h"
#include "MediaRequestManager.h"

#include <nrdbase/Log.h>
#include <nrd/NrdApplication.h>

using namespace std;
using namespace netflix;
using namespace netflix::ase;
using namespace netflix::mediacontrol;


////////////////////
// NrdpMediaSourceManager
////////////////////

NrdpMediaSourceManager::NrdpMediaSourceManager() :
    m_nextSourceId(0)
{
}

NrdpMediaSourceManager::~NrdpMediaSourceManager()
{
}

//////////
// createMediaSource
//////////
uint32_t NrdpMediaSourceManager::createMediaSource(IAseConfigParameterPtr streamingConfig)
{
    uint32_t sourceId = m_nextSourceId++;

    shared_ptr<NrdpMediaSource> source(new NrdpMediaSource(streamingConfig));

    // add to map
    m_sources[sourceId] = source;

    NTRACE(TRACE_MEDIACONTROL, "created mediaSource: %d", sourceId);

    return sourceId;
};

//////////
// destroyMediaSource
//////////
void NrdpMediaSourceManager::destroyMediaSource(uint32_t sourceId)
{
    NTRACE(TRACE_MEDIACONTROL, "destroy mediaSource: %d", sourceId);
    m_sources.erase(sourceId);
}

//////////
// destroyMediaSource
//////////
uint32_t NrdpMediaSourceManager::numberOfMediaSources()
{
    return m_sources.size();
}

//////////
// getMediaSource
//////////
shared_ptr<NrdpMediaSource> NrdpMediaSourceManager::getMediaSource(uint32_t sourceId)
{
    SourceMap::iterator it = m_sources.find(sourceId);
    if (it == m_sources.end())
    {
        NTRACE(TRACE_MEDIACONTROL, "getMediaSource: cannot find sourceId: %d", sourceId);
        return shared_ptr<NrdpMediaSource>();
    }

    NTRACE(TRACE_MEDIACONTROL, "getMediaSource %d", sourceId);
    return it->second;
}

////////////////////
// NrdpMediaSource
////////////////////

NrdpMediaSource::NrdpMediaSource(ase::IAseConfigParameterPtr pConfigParameter)
                : mMutex(NRDP_MEDIASOURCE_MUTEX, "NrdpMediaSource")
                , mConfigParameterPtr(pConfigParameter)
                , mActiveAudioSourceBuffer(shared_ptr<NrdpMediaSourceBuffer>())
                , mActiveVideoSourceBuffer(shared_ptr<NrdpMediaSourceBuffer>())
                , mCurrentPlayoutTime(AseTimeStamp::INFINITE)
                , mLastFlushPlayoutTime(AseTimeStamp::INFINITE)
                , mDidSetAudioSyncTimestamp( false )
                , mLatestSrcBufferId(0)
                , mTotalVideoBufferSize(0)
                , mTotalAudioBufferSize(0)
                , mMaxTrailingBufferLen(0)
                , mMaxVideoTrailingBuffer(0)
                , mMaxAudioTrailingBuffer(0)
                , mMinPrebufLength(AseTimeStamp::ZERO)
                , mMinAudioPtsGap(AseTimeInterval::ZERO)
{
    loadConfigParas();
}


NrdpMediaSource::~NrdpMediaSource()
{
    NTRACE(TRACE_MEDIACONTROL, "NrdpMediaSource cleared");
    mMediaSourceBufferMap.clear();
    mActiveAudioSourceBuffer.reset();
    mActiveVideoSourceBuffer.reset();
}

uint32_t NrdpMediaSource::createSourceBuffer(MediaType mediaType)
{
    ScopedMutex scopedMutex(mMutex);
    uint32_t srcBufferId = mLatestSrcBufferId;
    ++mLatestSrcBufferId;

    uint32_t maxTrailingBufferSize;
    uint32_t maxTrailingBufferLen = mMaxTrailingBufferLen;
    if ( mediaType == MEDIA_VIDEO )
    {
        maxTrailingBufferSize = mMaxVideoTrailingBuffer;
    }
    else
    {
        maxTrailingBufferSize = mMaxAudioTrailingBuffer;
    }

    shared_ptr<NrdpMediaSourceBuffer> pSrcBuffer;
    pSrcBuffer.reset(new NrdpMediaSourceBuffer(srcBufferId,
                                               mediaType,
                                               maxTrailingBufferLen,
                                               maxTrailingBufferSize,
                                               mMinAudioPtsGap));

    mMediaSourceBufferMap[srcBufferId] = pSrcBuffer;

    switch (mediaType)
    {
    case MEDIA_AUDIO:
        if (mActiveAudioSourceBuffer.get() == NULL )
            mActiveAudioSourceBuffer = pSrcBuffer;
        break;
    case MEDIA_VIDEO:
        if (mActiveVideoSourceBuffer.get() == NULL )
            mActiveVideoSourceBuffer = pSrcBuffer;
        break;
    default:
        break;
    }
    return srcBufferId;
}

void NrdpMediaSource::flushSourceBuffer(MediaType mediaType)
{
    ScopedMutex scopedMutex(mMutex);
    NTRACE(TRACE_MEDIACONTROL,
            "NrdpMediaSource::flushSourceBuffer, media type:%u", mediaType);
    shared_ptr<NrdpMediaSourceBuffer> pSrcBuffer;
    switch (mediaType)
    {
    case MEDIA_AUDIO:
        pSrcBuffer = mActiveAudioSourceBuffer;
        break;
    case MEDIA_VIDEO:
        pSrcBuffer = mActiveVideoSourceBuffer;
        break;
    default:
        break;
    }

    if ( pSrcBuffer.get() != NULL )
    {
        pSrcBuffer->flush();

        shared_ptr<MediaRequestManager> pMediaRequestManager = mMediaRequestManager.lock();
        if ( pMediaRequestManager )
            pMediaRequestManager->completeFlushSourceBuffer( shared_from_this(), mediaType );

    }

    return;
}

void NrdpMediaSource::flushSourceBuffers()
{
    ScopedMutex scopedMutex(mMutex);
    NTRACE(TRACE_MEDIACONTROL, "NrdpMediaSource::flushSourceBuffer");
    for ( map<uint32_t, shared_ptr<NrdpMediaSourceBuffer> >::iterator iter
                        = mMediaSourceBufferMap.begin();
                iter != mMediaSourceBufferMap.end();
                ++iter )
    {
        (iter->second)->flush();
    }

    shared_ptr<MediaRequestManager> pMediaRequestManager = mMediaRequestManager.lock();
    if ( pMediaRequestManager )
        pMediaRequestManager->completeFlushSourceBuffers( shared_from_this() );
}

void NrdpMediaSource::completeFlushSourceBuffer(MediaType mediaType)
{
    ScopedMutex scopedMutex(mMutex);
    NTRACE(TRACE_MEDIACONTROL,
            "NrdpMediaSource::completeFlushSourceBuffer, media type:%u", mediaType);
    shared_ptr<NrdpMediaSourceBuffer> pSrcBuffer;
    switch (mediaType)
    {
    case MEDIA_AUDIO:
        pSrcBuffer = mActiveAudioSourceBuffer;
        break;
    case MEDIA_VIDEO:
        pSrcBuffer = mActiveVideoSourceBuffer;
        break;
    default:
        break;
    }

    if ( pSrcBuffer.get() != NULL )
    {
        pSrcBuffer->completeFlush();
    }

    if (mediaType == MEDIA_AUDIO)
    {
        mDidSetAudioSyncTimestamp = false;
    }

    return;
}

void NrdpMediaSource::completeFlushSourceBuffers()
{
    ScopedMutex scopedMutex(mMutex);
    NTRACE(TRACE_MEDIACONTROL, "NrdpMediaSource::completeFlushSourceBuffers");
    for ( map<uint32_t, shared_ptr<NrdpMediaSourceBuffer> >::iterator iter
                        = mMediaSourceBufferMap.begin();
                iter != mMediaSourceBufferMap.end();
                ++iter )
    {
        (iter->second)->completeFlush();
    }

    mLastFlushPlayoutTime = mCurrentPlayoutTime;

    mDidSetAudioSyncTimestamp = false;
}

void NrdpMediaSource::querySourceBufferTimeRange(AseTimeStamp& startPts, AseTimeStamp& endPts)
{
    ScopedMutex scopedMutex(mMutex);

    if ( (mActiveVideoSourceBuffer.get() == NULL) ||
            (mActiveAudioSourceBuffer.get() == NULL) ||
            mActiveAudioSourceBuffer->empty() ||
            mActiveVideoSourceBuffer->empty() )
    {
        startPts = AseTimeStamp::ZERO;
        endPts = AseTimeStamp::ZERO;
        return;
    }

    AseTimeStamp videoStartPts, videoEndPts;
    AseTimeStamp audioStartPts, audioEndPts;
    mActiveVideoSourceBuffer->queryTimeRange(videoStartPts, videoEndPts);
    mActiveAudioSourceBuffer->queryTimeRange(audioStartPts, audioEndPts);

    startPts = (videoStartPts > audioStartPts) ? videoStartPts : audioStartPts;
    endPts = (videoEndPts < audioEndPts) ? videoEndPts : audioEndPts;

    endPts = (endPts > startPts + mMinPrebufLength) ?
                        endPts - mMinPrebufLength : startPts;
    return;
}

void NrdpMediaSource::endOfStream(uint32_t srcBufferId)
{
    ScopedMutex scopedMutex(mMutex);
    if ( mMediaSourceBufferMap.find(srcBufferId) != mMediaSourceBufferMap.end() )
        mMediaSourceBufferMap[srcBufferId]->endOfStream();
}


NFErr NrdpMediaSource::skip(ase::AseTimeStamp const& pts)
{
    ScopedMutex scopedMutex(mMutex);
    AseTimeStamp startPts, endPts;
    querySourceBufferTimeRange(startPts, endPts);
    if ( (pts < startPts) || (pts > endPts) || (startPts == endPts) )
        return NFErr_NotAllowed;

    if ( (mActiveAudioSourceBuffer.get() == NULL) ||
            (mActiveVideoSourceBuffer.get() == NULL) )
    {
        return NFErr_NotAllowed;
    }

    AseTimeStamp const videoPts = mActiveVideoSourceBuffer->skip(pts);
    mActiveAudioSourceBuffer->skip(videoPts);

    return NFErr_OK;
}

NFErr NrdpMediaSource::append(MediaSourceAppendablePtr pAppendable, uint32_t srcBufferId)
{
    ScopedMutex scopedMutex(mMutex);

    NFErr retCode = NFErr_OK;

    if ( mMediaSourceBufferMap.find(srcBufferId) != mMediaSourceBufferMap.end() )
    {
        shared_ptr<NrdpMediaSourceBuffer> pSrcBuffer = mMediaSourceBufferMap[srcBufferId];

        if ( !pSrcBuffer )
        {
            NTRACE(TRACE_MEDIACONTROL, "NrdpMediaSource::append for known but null source buffer %u", srcBufferId );
            assert( false );
        }

        retCode = pSrcBuffer->append(pAppendable);
    }
    else
    {
        retCode = NFErr_BadParameter;
        // log error here
    }

    return retCode;
}

NFErr NrdpMediaSource::setTimestampOffset( uint32_t srcBufferId, int32_t timeOffset, uint32_t timescale )
{
    ScopedMutex scopedMutex(mMutex);

    NFErr retCode = NFErr_OK;

    if ( mMediaSourceBufferMap.find(srcBufferId) != mMediaSourceBufferMap.end() )
    {
        shared_ptr<NrdpMediaSourceBuffer> pSrcBuffer = mMediaSourceBufferMap[srcBufferId];
        retCode = pSrcBuffer->setTimestampOffset(timeOffset,timescale);
    }
    else
    {
        retCode = NFErr_BadParameter;
        // log error here
    }

    return retCode;
}

void NrdpMediaSource::setMediaRequestManager( shared_ptr<MediaRequestManager> pMediaReqiestManager )
{
    mMediaRequestManager = pMediaReqiestManager;
}

void NrdpMediaSource::mediaSourceTasks()
{
    ScopedMutex scopedMutex(mMutex);

    if ( !mDidSetAudioSyncTimestamp && mActiveAudioSourceBuffer && mActiveVideoSourceBuffer )
    {
        AseTimeStamp videoStartPts, videoEndPts;

        if ( mActiveVideoSourceBuffer->queryTimeRange( videoStartPts, videoEndPts ) )
        {
            mActiveAudioSourceBuffer->setAudioSyncTimestamp( videoStartPts );

            mDidSetAudioSyncTimestamp = true;

            NTRACE(TRACE_MEDIAPLAYBACK, "mediaSourceTasks setAudioSyncTimestamp with %lld", videoStartPts.getValueInMs());
        }
    }

    if ( mCurrentPlayoutTime != mLastFlushPlayoutTime )
    {
        if ( mActiveAudioSourceBuffer )
            mActiveAudioSourceBuffer->flush( mCurrentPlayoutTime );
        if ( mActiveVideoSourceBuffer )
            mActiveVideoSourceBuffer->flush( mCurrentPlayoutTime );

        mLastFlushPlayoutTime = mCurrentPlayoutTime;
    }
}

void NrdpMediaSource::updatePts(uint32_t currentPts)
{
    ScopedMutex scopedMutex(mMutex);

    mCurrentPlayoutTime.set(currentPts, 1000);

    return;
}

void NrdpMediaSource::enableSampleFeeding()
{
    ScopedMutex scopedMutex(mMutex);

    if ( mActiveAudioSourceBuffer )
        mActiveAudioSourceBuffer->enableSampleFeeding();
    if ( mActiveVideoSourceBuffer )
        mActiveVideoSourceBuffer->enableSampleFeeding();

    return;
}

void NrdpMediaSource::disableSampleFeeding()
{
    ScopedMutex scopedMutex(mMutex);

    if ( mActiveAudioSourceBuffer )
        mActiveAudioSourceBuffer->disableSampleFeeding();
    if ( mActiveVideoSourceBuffer )
        mActiveVideoSourceBuffer->disableSampleFeeding();

    return;
}

void NrdpMediaSource::loadConfigParas()
{
    shared_ptr<device::IBufferManager> bufferManager = nrdApp()->getDeviceLib()->getBufferManager();
    if(bufferManager.get()){
        mTotalVideoBufferSize = bufferManager->getBufferPoolSize(device::IBufferManager::VIDEO);
        mTotalAudioBufferSize = bufferManager->getBufferPoolSize(device::IBufferManager::AUDIO);
    } else {
        mTotalVideoBufferSize = 32 * 1024 * 1024;
        mTotalAudioBufferSize = 4 * 1024 * 1024;
    }

    mConfigParameterPtr->reloadSystemParameters();
    mMinPrebufLength  = AseTimeStamp(mConfigParameterPtr->getParameterAs<int>("minPrebufSize", 4004), 1000);
    
    uint32_t dpiMinAudioPtsGap = nrdApp()->getSystem()->getCapability().minAudioPtsGap;
    uint32_t configMinAudioPtsGap = mConfigParameterPtr->getParameterAs<uint32_t>("minAudioPtsGap", 0);

    // audio minimal gap priority
    // 1. config value since we want to override any value set by dpi or default
    // 2. dpi value : partners have limitation figured out at development time
    // 3. default : 0 - partners can handle any gap.
    if ( configMinAudioPtsGap ){
        mMinAudioPtsGap = AseTimeInterval(configMinAudioPtsGap, 1000);
    } else if ( dpiMinAudioPtsGap ) {
        mMinAudioPtsGap = AseTimeInterval(dpiMinAudioPtsGap, 1000);
    } else {
        mMinAudioPtsGap =AseTimeInterval(0, 1000);
    }

    if (enableSkipBack())
    {
        mMaxTrailingBufferLen =
            mConfigParameterPtr->getParameterAs<int>("maxTrailingBufferLen", 15000);
        mMaxVideoTrailingBuffer =
            mConfigParameterPtr->getParameterAs<int>("maxVideoTrailingBufferSize", 8 * 1024 * 1024);
        mMaxAudioTrailingBuffer =
            mConfigParameterPtr->getParameterAs<int>("maxAudioTrailingBufferSize", 384 * 1024);
    }
    else
    {
        // don't maintain a back buffer when skip back is not enabled
        mMaxTrailingBufferLen = 2002;
        mMaxVideoTrailingBuffer = 0;
        mMaxAudioTrailingBuffer = 0;
    }
    NTRACE( TRACE_MEDIACONTROL,
            "loadConfigParas: mMaxTrailingBufferLen: %u, mMaxVideoTrailingBuffer: %u, mMaxAudioTrailingBuffer: %u, mMinAudioGap %lld(ms)",
            mMaxTrailingBufferLen, mMaxVideoTrailingBuffer, mMaxAudioTrailingBuffer, mMinAudioPtsGap.getValueInMs());
}


bool NrdpMediaSource::enableSkipBack()
{
    uint32_t minVideoBufferRequiredForSkipBack =
            mConfigParameterPtr->getParameterAs<int>(
                "minVideoBufferRequiredForSkipBack", 24 * 1024 * 1024);
    uint32_t minAudioBufferRequiredForSkipBack =
        mConfigParameterPtr->getParameterAs<int>(
            "minAudioBufferRequiredForSkipBack", 1024 * 1024);

    bool enableVideoSkipBack = false;
    if ( ( mTotalVideoBufferSize >= minVideoBufferRequiredForSkipBack)
        && ( mTotalAudioBufferSize >= minAudioBufferRequiredForSkipBack) )
    {
        enableVideoSkipBack = true;
    }
    NTRACE(TRACE_MEDIACONTROL, "video skip back enabled: %u", enableVideoSkipBack);
    return enableVideoSkipBack;
}

device::esplayer::Status NrdpMediaSource::getNextSample(device::esplayer::MediaType mediaType,
                                                        device::esplayer::ISampleWriter& sampleWriter,
                                                        device::esplayer::SampleAttributes& sampleAttributes)
{
    device::esplayer::Status status = device::esplayer::OK;

    shared_ptr<NrdpMediaSourceBuffer> pSrcBuffer;
    switch (mediaType) {
    case device::esplayer::AUDIO:
        pSrcBuffer = mActiveAudioSourceBuffer;
        break;
    case device::esplayer::VIDEO:
        pSrcBuffer = mActiveVideoSourceBuffer;
        break;
    default:
        break;
    }

    if ( pSrcBuffer.get() == NULL ) {
        // Device thread can be created before JSASE has even created the source
        // buffers, so return PENDING for now until we fix the media.js
        // timing. This will sleep the pumping thread until the source buffers
        // are ready
        status = device::esplayer::NO_AVAILABLE_SAMPLES;
    } else {
        status = pSrcBuffer->getNextSample(sampleWriter, sampleAttributes);
    }

    return status;
}

device::esplayer::Status NrdpMediaSource::peekNextSampleAttributes(
    device::esplayer::MediaType mediaType,
    device::esplayer::SampleAttributes& sampleAttributes)
{
    device::esplayer::Status status = device::esplayer::OK;

    shared_ptr<NrdpMediaSourceBuffer> pSrcBuffer;
    switch (mediaType) {
    case device::esplayer::AUDIO:
        pSrcBuffer = mActiveAudioSourceBuffer;
        break;
    case device::esplayer::VIDEO:
        pSrcBuffer = mActiveVideoSourceBuffer;
        break;
    default:
        break;
    }

    if ( pSrcBuffer.get() == NULL ) {
        // Device thread can be created before JSASE has even created the source
        // buffers, so return PENDING for now until we fix the media.js
        // timing. This will sleep the pumping thread until the source buffers
        // are ready
        status = device::esplayer::NO_AVAILABLE_SAMPLES;
    } else {
        status = pSrcBuffer->peekNextSampleAttributes(sampleAttributes);
    }

    return status;

}

