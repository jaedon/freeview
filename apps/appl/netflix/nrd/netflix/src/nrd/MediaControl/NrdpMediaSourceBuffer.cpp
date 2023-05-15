/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#include "NrdpMediaSourceBuffer.h"

#include <nrdbase/Log.h>

#define INVALID_TRACKID 0xffff

//#define DEBUG_VERBOSE 1

/*
 * **************
 * 3D stream mode
 * **************
 */

#define WRITE_TO_SAMPLEWRITER(startptr, size)                   \
    do{                                                         \
        if((size) != sampleWriter.write((startptr), (size)))    \
        {                                                       \
            assert(0);                                          \
            return device::esplayer::SAMPLE_WRITER_ERROR;       \
        }                                                       \
    }while(0)                                                   \

#define WRITE_TO_SAMPLEWRITER_VIEWID(startptr, size, viewid)            \
    do{                                                                 \
        if((size) != sampleWriter.write((startptr), (size), (viewid)))  \
        {                                                               \
            assert(0);                                                  \
            return device::esplayer::SAMPLE_WRITER_ERROR;               \
        }                                                               \
    }while(0)                                                           \
                                                                        \
/*
 * ****************
 * static functions
 * ****************
 */
// Takes an array of bytes and returns a string of the byte values in hex.
static std::string bytesToHex(const uint8_t *bytes, uint32_t numBytes)
{
    std::stringstream ss;
    ss.setf(std::ios::hex, std::ios::basefield);

    for(uint32_t i = 0; i < numBytes; i++)
    {
        ss << static_cast<uint32_t>(bytes[i]);
    }
    return ss.str();
}

using namespace std;
using namespace netflix;
using namespace netflix::ase;
using namespace netflix::mediacontrol;
using namespace netflix::streamparser;
using namespace netflix::device;
using namespace netflix::device::esplayer;

static uint32_t getNalSize(const uint8_t* buff, const uint32_t numBytes)
{
    assert(numBytes <= 4);
    uint32_t result = 0;
    uint32_t i;
    for(i = 0; i < numBytes; i++) {
        result <<= 8;
        result += buff[i];
    }
    return result;
}

NrdpMediaSourceBuffer::NrdpMediaSourceBuffer(uint32_t srcBufferId,
                                             ase::MediaType mediaType,
                                             uint32_t maxTrailingBufferDuration,
                                             uint32_t maxTrailingBufferSize,
                                             ase::AseTimeInterval minAudioPtsGap)
                : mSrcBufferId(srcBufferId)
                , mMediaType(mediaType)
                , mMutex( NRDP_MEDIASOURCEBUFFER_MUTEX, "NrdpMediaSourceBuffer" )
                , mHavePendingTimestampOffset( false )
                , mMaxTrailingBufferDuration(maxTrailingBufferDuration)
                , mMaxTrailingBufferSize(maxTrailingBufferSize)
                , mHaveAudioSyncronizationPts( false )
                , mAudioSynchronizationPts( AseTimeStamp::ZERO )
                , mEndOfTrack(false)
                , mFlushPending(false)
                , mMediaSourceStreamParser( mediaType )
                , mOutputTrackId(INVALID_TRACKID)
                , mOutputTrackFragmentIndex(0)
                , mOutputTrackSampleIndex(0)
                , mPrimaryTrackId(INVALID_TRACKID)
                , mSecondaryTrackId(INVALID_TRACKID)
                , mIsSecondaryTrackDolbyVisionTrack(false)
                , mAudioDiscontinuity(true)
                , mEndOfStreamLogged(false)
                , mMinAudioPtsGap(minAudioPtsGap)
                , mSampleFeedingDisabled ( false )
                , mParseErrorHappened ( false )
{
    NTRACE(TRACE_MEDIACONTROL, "NrdpMediaSourceBuffer(%u)::%s mMinAudioPtsGap %lld",
           mMediaType, __func__, mMinAudioPtsGap.getValueInMs());
}

NrdpMediaSourceBuffer::~NrdpMediaSourceBuffer()
{

}

void NrdpMediaSourceBuffer::flush()
{
    ScopedMutex scopedMutex(mMutex);

    NTRACE(TRACE_MEDIACONTROL, "NrdpMediaSourceBuffer(%u)::%s", mMediaType, __func__);

    mFlushPending = true;

    mEndOfTrack = false;

    mAudioDiscontinuity = true;

    mOutputTrackId = INVALID_TRACKID;
    mOutputTrackFragmentIndex = 0;
    mOutputTrackSampleIndex = 0;

    mPrimaryTrackId = INVALID_TRACKID;
    mSecondaryTrackId = INVALID_TRACKID;

    mEndOfStreamLogged = false;

    mHavePendingTimestampOffset = false;

    mCurrentTimestampOffset = TimestampOffset();

    mHaveAudioSyncronizationPts = false;

    mAudioSynchronizationPts = AseTimeStamp::ZERO;
}

void NrdpMediaSourceBuffer::completeFlush()
{
    ScopedMutex scopedMutex(mMutex);

    NTRACE(TRACE_MEDIACONTROL, "NrdpMediaSourceBuffer(%u)::%s, %s", mMediaType, __func__,
            mFlushPending ? "true" : "false" );

    if ( mFlushPending )
    {
        mTrackHeaders.clear();
        mMediaTrackFragments.clear();

        mMediaSourceStreamParser.reset();

        while ( !mAppendableList.empty() )
        {
            mAppendableList.front().mAppendablePtr->cancelForwarding();
            mAppendableList.pop_front();
        }

        mFlushPending = false;
    }
}

bool NrdpMediaSourceBuffer::queryTimeRange(AseTimeStamp& startPts, AseTimeStamp& endPts)
{
    //NTRACE(TRACE_MEDIACONTROL, "NrdpMediaSourceBuffer(%u)::%s", mMediaType, __func__);
    ScopedMutex scopedMutex(mMutex);

    if (mPrimaryTrackId == INVALID_TRACKID)
        return false;

    if (mMediaTrackFragments[mPrimaryTrackId].empty() || mFlushPending)
        return false;

    startPts = mMediaTrackFragments[mPrimaryTrackId].front()->mFragmentHeaderPtr->mStartPts;
    endPts = mMediaTrackFragments[mPrimaryTrackId].back()->mFragmentHeaderPtr->mEndPts;

    //NTRACE(TRACE_MEDIACONTROL, "NrdpMediaSourceBuffer(%u)::%s start %lld, end %lld"
    //       , mMediaType, __func__, startPts.getValueInMs(), endPts.getValueInMs());

    return true;
}

AseTimeStamp NrdpMediaSourceBuffer::skip(ase::AseTimeStamp const& pts)
{
    NTRACE(TRACE_MEDIACONTROL, "NrdpMediaSourceBuffer(%u)::%s", mMediaType, __func__);
    ScopedMutex scopedMutex(mMutex);

    uint32_t index;

    if ( mFlushPending || !findPlaybackPosition( pts, index ) )
        return AseTimeStamp();

    mOutputTrackFragmentIndex = index;
    mOutputTrackSampleIndex = 0;
    mOutputTrackId = mPrimaryTrackId;
    
    FragmentHeader const& fragment = *(mMediaTrackFragments[mPrimaryTrackId][mOutputTrackFragmentIndex]->mFragmentHeaderPtr);

    AseTimeStamp actual_ts = fragment.mStartPts;

    if ( mMediaType == MEDIA_AUDIO )
    {
        Sample const* pSample = &fragment.mSamples[ mOutputTrackSampleIndex ];

        while( AseTimeStamp( pSample->mPts, 1000 ) + AseTimeStamp( fragment.mSampleDuration, fragment.mTimescale ) < pts )
        {
            ++mOutputTrackSampleIndex;

            if ( mOutputTrackSampleIndex >= fragment.mSamples.size() )
            {
                NTRACE(TRACE_MEDIACONTROL, "NrdpMediaSourceBuffer(%u)::%s: No sample in chosen fragment spanning skip pts!!", mMediaType, __func__);
                return AseTimeStamp();
            }

            pSample = &fragment.mSamples[ mOutputTrackSampleIndex ];
        }

        actual_ts = AseTimeStamp( fragment.mSamples[ mOutputTrackSampleIndex ].mPts, 1000 );
    }

    NTRACE(TRACE_MEDIASTARTUP, "NrdpMediaSourceBuffer::%s: fragment %d, sample %d, fragment pts %llu, actual pts %llums", __func__,
            index, mOutputTrackSampleIndex, fragment.mStartPts.getValueInMs(), actual_ts.getValueInMs() );

    return actual_ts;
}

void NrdpMediaSourceBuffer::setAudioSyncTimestamp( ase::AseTimeStamp const& pts )
{
    NTRACE(TRACE_MEDIACONTROL, "NrdpMediaSourceBuffer(%u)::%s: %llums", mMediaType, __func__, pts.getValueInMs());
    ScopedMutex scopedMutex(mMutex);

    mHaveAudioSyncronizationPts = true;

    mAudioSynchronizationPts = pts;
}

void NrdpMediaSourceBuffer::endOfStream()
{
    NTRACE(TRACE_MEDIACONTROL, "NrdpMediaSourceBuffer(%u)::%s", mMediaType, __func__);
    ScopedMutex scopedMutex(mMutex);
    mEndOfTrack = true;
}

NFErr NrdpMediaSourceBuffer::append( MediaSourceAppendablePtr pAppendable )
{
    //NTRACE(TRACE_MEDIACONTROL, "NrdpMediaSourceBuffer(%u)::%s", mMediaType, __func__);
    if ( mFlushPending )
    {
        NTRACE(TRACE_MEDIACONTROL, "NrdpMediaSourceBuffer(%u)::%s - ignoring whilst buffer flushing", mMediaType, __func__);
        return NFErr_OK;
    }

    if ( mHavePendingTimestampOffset )
    {
        mAppendableList.push_back( AppendableRecord( mPendingTimestampOffset, pAppendable ) );
        mHavePendingTimestampOffset = false;
    }
    else
    {
        mAppendableList.push_back( AppendableRecord( pAppendable ) );
    }

    if ( mAppendableList.size() == 1 )
    {
        processAppendedData();
    }

    return NFErr_OK;
}

NFErr NrdpMediaSourceBuffer::setTimestampOffset( int32_t timeOffset, uint32_t timescale )
{
    NTRACE(TRACE_MEDIACONTROL, "NrdpMediaSourceBuffer(%u)::%s timeOffset %d, timescale %u"
           , mMediaType, __func__, timeOffset, timescale);
    if ( mFlushPending )
    {
        NTRACE(TRACE_MEDIACONTROL, "NrdpMediaSourceBuffer(%u)::%s - ignoring whilst buffer flushing", mMediaType, __func__);
        return NFErr_OK;
    }

    mPendingTimestampOffset = TimestampOffset( timeOffset, timescale );
    mHavePendingTimestampOffset = true;

    return NFErr_OK;
}

bool NrdpMediaSourceBuffer::empty()
{
    NTRACE(TRACE_MEDIACONTROL, "NrdpMediaSourceBuffer(%u)::%s", mMediaType, __func__);
    ScopedMutex scopedMutex(mMutex);
    return mFlushPending || mMediaTrackFragments[mPrimaryTrackId].empty();
}

void NrdpMediaSourceBuffer::processAppendedData()
{
    while( !mAppendableList.empty() )
    {
        if ( mAppendableList.front().mHaveTimestampOffset )
        {
            mCurrentTimestampOffset = mAppendableList.front().mTimestampOffset;
            mAppendableList.front().mHaveTimestampOffset = false;
        }

        if ( !mAppendableList.front().mAppendablePtr->forward( shared_from_this() ) )
            break;

        // Appendable is complete
        mAppendableList.pop_front();
    }
}

void NrdpMediaSourceBuffer::indicateBlocksAvailable()
{
    if ( !mFlushPending )
        processAppendedData();
}

void NrdpMediaSourceBuffer::flush(AseTimeStamp const& pts )
{
    //NTRACE(TRACE_MEDIACONTROL, "NrdpMediaSourceBuffer(%u)::%s", mMediaType, __func__);
    ScopedMutex scopedMutex(mMutex);

    if (mPrimaryTrackId == INVALID_TRACKID)
        return;

    if ( mMediaTrackFragments[mPrimaryTrackId].empty() )
        return;

    while( !mMediaTrackFragments[mPrimaryTrackId].empty() &&
           (pts > mMediaTrackFragments[mPrimaryTrackId].front()->getFragmentEndTime()) &&
           ((pts - mMediaTrackFragments[mPrimaryTrackId].front()->getFragmentEndTime()).getValueInMs() > mMaxTrailingBufferDuration ) )
    {
        NTRACE(TRACE_MEDIAPLAYBACK, "[%u] MEDIAFRAGMENT_DELETED for (pts %lld - %lld) remaining fragments %zu (exceeding duration limit)",
               mMediaType,
               mMediaTrackFragments[mPrimaryTrackId].front()->getFragmentStartTime().getValueInMs(),
               mMediaTrackFragments[mPrimaryTrackId].front()->getFragmentEndTime().getValueInMs(),
               mMediaTrackFragments[mPrimaryTrackId].size()-1);

        mMediaTrackFragments[mPrimaryTrackId].pop_front();
        if( mSecondaryTrackId != INVALID_TRACKID)
        {
            mMediaTrackFragments[mSecondaryTrackId].pop_front();
        }

        // decrement index for deque
        --mOutputTrackFragmentIndex;
    }

    // remove fragment that is over trailing buffer size limitation
    uint32_t trailingBufferSize = 0;
    uint32_t index = 0;

    while( !mMediaTrackFragments[mPrimaryTrackId].empty() &&
            (index < mMediaTrackFragments[mPrimaryTrackId].size()) &&
            (mMediaTrackFragments[mPrimaryTrackId][index]->getFragmentEndTime() < pts) )
    {
        trailingBufferSize += mMediaTrackFragments[mPrimaryTrackId][index]->getFragmentSizeInBuffer();
        if( mSecondaryTrackId != INVALID_TRACKID)
        {
            trailingBufferSize += mMediaTrackFragments[mSecondaryTrackId][index]->getFragmentSizeInBuffer();
        }

        if( trailingBufferSize > mMaxTrailingBufferSize )
        {
            NTRACE(TRACE_MEDIAPLAYBACK, "[%u] MEDIAFRAGMENT_DELETED for (pts %lld - %lld) remaining fragments %zu (exceeding size limit)",
                   mMediaType,
                   mMediaTrackFragments[mPrimaryTrackId].front()->getFragmentStartTime().getValueInMs(),
                   mMediaTrackFragments[mPrimaryTrackId].front()->getFragmentEndTime().getValueInMs(),
                   mMediaTrackFragments[mPrimaryTrackId].size()-1);

            trailingBufferSize -= mMediaTrackFragments[mPrimaryTrackId].front()->getFragmentSizeInBuffer();
            mMediaTrackFragments[mPrimaryTrackId].pop_front();
            if( mSecondaryTrackId != INVALID_TRACKID)
            {
                trailingBufferSize -= mMediaTrackFragments[mSecondaryTrackId].front()->getFragmentSizeInBuffer();
                mMediaTrackFragments[mSecondaryTrackId].pop_front();
            }

            // decrement track fragment index
            --mOutputTrackFragmentIndex;
        }
        else
        {
            ++index;
        }
    }
}

void NrdpMediaSourceBuffer::provideBlock(AseBufferPtr pData )
{
    bool done = false;

    ScopedMutex scopedMutex(mMutex);

#if defined(DEBUG_VERBOSE)
    NTRACE(TRACE_MEDIAPLAYBACK, "NrdpMediaSourceBuffer(%u)::provideBlock: data in: %zu bytes",
           mMediaType, pData->getFilledSpaceSize() );
#endif

    if ( mFlushPending ){
        NTRACE(TRACE_MEDIAPLAYBACK,
               "NrdpMediaSourceBuffer(%u)::provideBlock: return because of mFlushPending true", mMediaType);
        return;
    }

    if ( mParseErrorHappened ) {
        NTRACE(TRACE_MEDIAPLAYBACK,
               "NrdpMediaSourceBuffer(%u)::provideBlock: return because of mParseErrorHappened true",
               mMediaType);
        return;
    }

    mMediaSourceStreamParser.datain( pData, mCurrentTimestampOffset.mOffset, mCurrentTimestampOffset.mTimescale);

    while( !done )
    {
        shared_ptr<streamparser::TrackHeader> pTrackHeaderPtr;
        shared_ptr<streamparser::MediaFragment> pMediaFragmentPtr;
        MediaSourceStreamParser::ParseResult result;
        result = mMediaSourceStreamParser.dataout( pTrackHeaderPtr, pMediaFragmentPtr );
        switch ( result )
        {
        case MediaSourceStreamParser::MEDIAHEADER_AVAILABLE:
        {
            NTRACE(TRACE_MEDIAPLAYBACK,"[%u] MEDIAHEADER_AVAILABLE", mMediaType);
            uint32_t trackId = pTrackHeaderPtr->mTrackId;
            mTrackHeaders[trackId] = pTrackHeaderPtr;

            if(pTrackHeaderPtr->mCodecParam == "dvhe"){
                mSecondaryTrackId = trackId;
                mIsSecondaryTrackDolbyVisionTrack = true;
            } else {
                mPrimaryTrackId = trackId;
            }

            NTRACE(TRACE_MEDIAPLAYBACK
                   ,"[%u] MEDIAHEADER_AVAILABLE : trackId[%d]\n"
                   , mMediaType, trackId);

            break;
        }
        case MediaSourceStreamParser::MEDIAFRAGMENT_AVAILABLE:
        {
            // MediaFragment is available. Add it to queue
            uint32_t trackId = INVALID_TRACKID;
            if ( pMediaFragmentPtr->mTrackHeaderPtr ) {
                trackId = pMediaFragmentPtr->mTrackHeaderPtr->mTrackId;
            } else if ( pMediaFragmentPtr->mFragmentHeaderPtr ) {
                trackId = pMediaFragmentPtr->mFragmentHeaderPtr->mTrackId;
            } else if ( mPrimaryTrackId != INVALID_TRACKID ) {
                // this should not happen, but just in case if there is a bug in
                // retrieving trackId, we use mPrimaryTrackId which is already
                // set.
                assert(0);
                trackId = mPrimaryTrackId;
                Log::warn(TRACE_MEDIAPLAYBACK
                          ,"[%u] MEDIAFRAGMENT_AVAILABLE with fallback trackId [%d]: both track header, fragment header null"
                          , mMediaType, trackId);
            }

            if(mPrimaryTrackId == INVALID_TRACKID){
                mPrimaryTrackId = trackId;
            }
            mMediaTrackFragments[trackId].push_back(pMediaFragmentPtr);

            // For audio only
            //
            // compare pts between first sample of current fragment, and last
            // sample of previous fragment. If there are overlap in pts, remove
            // samples from previous fragment to remove overlapped time
            if( mMediaType == MEDIA_AUDIO ) {
                // end pts of previous audio fragment
                uint32_t const fragments = mMediaTrackFragments[trackId].size();
                if( fragments > 1 )
                {
                    FragmentHeader& previousFragment = *mMediaTrackFragments[trackId][fragments - 2]->mFragmentHeaderPtr;
                    FragmentHeader& currentFragment = *mMediaTrackFragments[trackId][fragments - 1]->mFragmentHeaderPtr;

                    ase::AseTimeInterval audioPtsGap;
                    ase::AseTimeStamp startPtsOfPreviousFrag = previousFragment.mStartPts;
                    ase::AseTimeStamp endPtsOfPreviousFrag = previousFragment.mEndPts;
                    ase::AseTimeStamp startPtsOfCurrentFrag = currentFragment.mStartPts;
                    ase::AseTimeStamp const oneMs(1, 1000);

                    // what is downloaded into mMediaTrackFragments should be always increasing in time
                    if(startPtsOfPreviousFrag > startPtsOfCurrentFrag){
                        NTRACE(TRACE_MEDIAPLAYBACK ,
                               "(%u) previousFragment.mStartPts(%lld) < currentFragment.mStartPts(%lld)",
                               mMediaType,
                               startPtsOfPreviousFrag.getValueInMs(),
                               startPtsOfCurrentFrag.getValueInMs());
                        assert(0);
                    }

                    if ( ( endPtsOfPreviousFrag > startPtsOfCurrentFrag ) ||
                         ( startPtsOfCurrentFrag > endPtsOfPreviousFrag + oneMs) )
                    {
                        // either when there is a overlap between fragments, or
                        // when there already is a gap between fragment that is
                        // smaller than minAudioPtsGap. difference bigger than
                        // oneMs is considered gap.

                        NTRACE(TRACE_MEDIAPLAYBACK,
                               "(%u) checking fragments for sample removal : endPtsOfPrevFrag %lld, startPtsOfStartFrag %lld (%s:%lld)",
                               mMediaType, endPtsOfPreviousFrag.getValueInMs(), startPtsOfCurrentFrag.getValueInMs(),
                               (startPtsOfCurrentFrag.getValueInMs() > (endPtsOfPreviousFrag.getValueInMs() + 1) )? "gap":"overlap",
                               startPtsOfCurrentFrag.getValueInMs() - endPtsOfPreviousFrag.getValueInMs() );

                        while( !previousFragment.mSamples.empty() &&
                               ( ( endPtsOfPreviousFrag + mMinAudioPtsGap ) > startPtsOfCurrentFrag ) )
                        {
                            ase::AseTimeStamp const lastPts( previousFragment.mSamples.back().mPts, 1000 );
                            previousFragment.mSamples.pop_back();
                            previousFragment.mEndPts = lastPts;
                            endPtsOfPreviousFrag =  lastPts;
                            NTRACE(TRACE_MEDIAPLAYBACK,
                                   "(%u) remove Sample with startPts %lld, duration %u\n",
                                   mMediaType, lastPts.getValueInMs(), previousFragment.mSampleDuration);
                        }
                        audioPtsGap = startPtsOfCurrentFrag - endPtsOfPreviousFrag;
                        if( audioPtsGap > oneMs){
                            // ignore oneMs as a gap. oneMs still means contiguous.
                            NTRACE(TRACE_MEDIAPLAYBACK ,"audioPtsGapInMs %lld\n", audioPtsGap.getValueInMs());
                            currentFragment.setDiscontinuityGapInMs(audioPtsGap);
                        }
                        if(audioPtsGap < mMinAudioPtsGap){
                            Log::error(TRACE_MEDIAPLAYBACK,
                                       "could not honor minAudioPtsGap (%lld) : prevFrag's sample size %zu",
                                       mMinAudioPtsGap.getValueInMs(),
                                       previousFragment.mSamples.size());
                        }
                    }
                }
            }

#ifndef NDEBUG
            if( mMediaType == MEDIA_VIDEO )
            {
                // end pts of previous audio fragment
                uint32_t const fragments = mMediaTrackFragments[trackId].size();
                if( fragments > 1 )
                {
                    FragmentHeader& previousFragment = *mMediaTrackFragments[trackId][fragments - 2]->mFragmentHeaderPtr;
                    FragmentHeader& currentFragment = *mMediaTrackFragments[trackId][fragments - 1]->mFragmentHeaderPtr;

                    ase::AseTimeStamp startPtsOfPreviousFrag = previousFragment.mStartPts;
                    ase::AseTimeStamp startPtsOfCurrentFrag = currentFragment.mStartPts;

                    // what is downloaded into mMediaTrackFragments should be always increasing in time
                    if(startPtsOfPreviousFrag > startPtsOfCurrentFrag){
                        NTRACE(TRACE_MEDIAPLAYBACK ,
                               "(%u) previousFragment.mStartPts(%lld) < currentFragment.mStartPts(%lld)",
                               mMediaType,
                               startPtsOfPreviousFrag.getValueInMs(),
                               startPtsOfCurrentFrag.getValueInMs());
                        assert(0);
                    }
                }
            }
#endif

            NTRACE(TRACE_MEDIAPLAYBACK,
                   "[%u] MEDIAFRAGMENT_AVAILABLE for trackId %d pts %lld - %lld (%lld) remaining fragments %zu",
                   mMediaType, trackId,
                   mMediaTrackFragments[trackId].back()->mFragmentHeaderPtr->mStartPts.getValueInMs(),
                   mMediaTrackFragments[trackId].back()->mFragmentHeaderPtr->mEndPts.getValueInMs(),
                   mMediaTrackFragments[trackId].back()->mFragmentHeaderPtr->mEndPts.getValueInMs()
                   - mMediaTrackFragments[trackId].back()->mFragmentHeaderPtr->mStartPts.getValueInMs(),
                   mMediaTrackFragments[trackId].size());
            break;
        }

        case MediaSourceStreamParser::NOTHING_AVAILABLE:
            //NTRACE(TRACE_MEDIACONTROL,"NOTHING_AVAILABLE\n");
            done = true;
            break;

        case MediaSourceStreamParser::STREAM_PARSING_ERROR:
            NTRACE(TRACE_MEDIAPLAYBACK, "NrdpMediaSourceBuffer(%u) : stream parsing error", mMediaType);
            done = true;
            mParseErrorHappened = true;
            break;
        }
    }
#if defined(DEBUG_VERBOSE)
    NTRACE(TRACE_MEDIAPLAYBACK,
           "NrdpMediaSourceBuffer(%u)::provideBlock: returns ============================",
           mMediaType);
#endif
}

bool NrdpMediaSourceBuffer::findPlaybackPosition( AseTimeStamp const& pts, uint32_t& index )
{
    NTRACE(TRACE_MEDIACONTROL, "NrdpMediaSourceBuffer(%u)::%s", mMediaType, __func__);

    ScopedMutex scopedMutex(mMutex);

    if (mPrimaryTrackId == INVALID_TRACKID)
        return false;

    if ( mMediaTrackFragments[mPrimaryTrackId].empty()
         || pts < mMediaTrackFragments[mPrimaryTrackId].front()->mFragmentHeaderPtr->mStartPts
         || pts >= mMediaTrackFragments[mPrimaryTrackId].back()->mFragmentHeaderPtr->mEndPts )
        return false;

    index = 0;
    for( ; index < mMediaTrackFragments[mPrimaryTrackId].size()
           && (mMediaTrackFragments[mPrimaryTrackId][index]->mFragmentHeaderPtr->mStartPts) < pts
         ; ++index );

    if ( index == mMediaTrackFragments[mPrimaryTrackId].size() )
        return false;


    NTRACE(TRACE_MEDIASTARTUP,"Playback Position Found - %d\n", index);
    return true;
}

bool updateADTSHeaderSizeBytes(vector<uint8_t>& ADTSHeader, uint32_t sampleSize)
{
    assert(ADTSHeader.size() > 5);
    if(ADTSHeader.size() <= 5)
    {
        Log::error(TRACE_MEDIAPLAYBACK, "MediaUtils::updateADTSHeaderSizeBytes: header "
                   "size %zu <= 5 bytes", ADTSHeader.size());
        return false;
    }
    uint8_t *buf = &(ADTSHeader[0]);
    sampleSize += ADTSHeader.size();

    buf[3] = (buf[3] & 0xfc) | (sampleSize >> 11);
    buf[4] = (sampleSize >> 3) & 0xff;
    buf[5] = (buf[5] & 0x1f) | ((sampleSize & 0x7) << 5);

    return true;
}

void NrdpMediaSourceBuffer::enableSampleFeeding()
{
    ScopedMutex scopedMutex(mMutex);

    NTRACE(TRACE_MEDIACONTROL, "NrdpMediaSourceBuffer(%u)::%s", mMediaType, __func__);

    mSampleFeedingDisabled = false;
}

void NrdpMediaSourceBuffer::disableSampleFeeding()
{
    ScopedMutex scopedMutex(mMutex);

    NTRACE(TRACE_MEDIACONTROL, "NrdpMediaSourceBuffer(%u)::%s", mMediaType, __func__);

    mSampleFeedingDisabled = true;
}

bool NrdpMediaSourceBuffer::isSampleFeedingDisabled()
{
    // do not put mutex around since this function will be called with mutex
    return mSampleFeedingDisabled;
}

/*
 * Note : USE_MVC_SPLIT mode
 * 1. sample size recalculating
 * 2. sub sample map re-construction
 * 3. sample writing per view -
 * 3.1. view 0: sps, pps, view 0's nalu
 * 3.2. view 1: ssps, dependent view pps, view 1's nalu
 */
NFErr NrdpMediaSourceBuffer::constructSampleAttributes(streamparser::MediaFragmentPtr& fragmentPtr,
                                                       streamparser::Sample& sample,
                                                       SampleAttributes& sampleAttributes)
{
    if ( mParseErrorHappened ){
        return NFErr_MP4ParsedData;
    }

    // not a single header is received when mEndOfTrack is set
    if ( (mPrimaryTrackId == INVALID_TRACKID) || (mMediaTrackFragments.size() == 0) )
    {
        if(mEndOfTrack){
            if(!mEndOfStreamLogged){
                NTRACE(TRACE_MEDIAPLAYBACK,
                       "NrdpMediaSourceBuffer(%u)::%s END_OF_STREAM set and no MediaTrackFragments",
                       mMediaType, __func__);
                mEndOfStreamLogged = true;
            }
            return NFErr_EndOfStream;
        } else {
            NTRACE(TRACE_MEDIAPLAYBACK, "NrdpMediaSourceBuffer(%u)::%s mPrimaryTrackId %u, tracks size %zu",
                   mMediaType, __func__, mPrimaryTrackId, mMediaTrackFragments.size());
            return NFErr_Pending;
        }
    }

    // decide from which track, sample should be retrieved.
    if ( mOutputTrackId == INVALID_TRACKID ){
        // first time when track is selected
        mOutputTrackId = mPrimaryTrackId;
    }

    // not a single fragment is received when mEndOfTrack is set
    if( mMediaTrackFragments[mOutputTrackId].size() == 0){
        NTRACE(TRACE_MEDIAPLAYBACK,"NrdpMediaSourceBuffer(%u)::%s fragment for trackId %d unavailable"
               ,mMediaType,__func__, mOutputTrackId);
        if(mEndOfTrack){
            if(!mEndOfStreamLogged){
                NTRACE(TRACE_MEDIAPLAYBACK, "[%u] NrdpMediaSourceBuffer::END_OF_STREAM", mMediaType);
                mEndOfStreamLogged = true;
            }
            return NFErr_EndOfStream;
        } else {
            return NFErr_Pending;
        }
    }

    if ( mMediaType == MEDIA_AUDIO && !mHaveAudioSyncronizationPts )
    {
        NTRACE(TRACE_MEDIAPLAYBACK,"NrdpMediaSourceBuffer(%u)::%s no audio synchronization PTS available"
               ,mMediaType,__func__);
        return NFErr_Pending;
    }

    do
    {
        // see if we have fragment available for feeding
        if ( mOutputTrackFragmentIndex >= mMediaTrackFragments[mOutputTrackId].size() ){
            NTRACE(TRACE_MEDIAPLAYBACK,
                   "[%u] mOutputTrackFragmentIndex %d, mMediaTrackFragments[%d].size() %zu\n",
                   mMediaType,mOutputTrackFragmentIndex,mOutputTrackId,mMediaTrackFragments[mOutputTrackId].size());

            if(mEndOfTrack){
                if(!mEndOfStreamLogged){
                    NTRACE(TRACE_MEDIAPLAYBACK, "[%u] NrdpMediaSourceBuffer::END_OF_STREAM", mMediaType);
                    mEndOfStreamLogged = true;
                }
                return NFErr_EndOfStream;
            } else {
                return NFErr_Pending;
            }
        }

        fragmentPtr = mMediaTrackFragments[mOutputTrackId][mOutputTrackFragmentIndex];

        if (!fragmentPtr->mFragmentHeaderPtr->mSamples.size()) {
            return NFErr_MP4ParsedData;
        }

        sample = fragmentPtr->mFragmentHeaderPtr->mSamples[mOutputTrackSampleIndex];

        // return NFErr_Pending if all data for current sample have not arrived.
        // Especially, when this is the only fragment available, we need to check if
        // sample data is arrived.
        if( !fragmentPtr->hasSample(mOutputTrackSampleIndex) ){
            NTRACE(TRACE_MEDIAPLAYBACK
                   , "NrdpMediaSourceBuffer(%u)::%s sample's data for mOutputTrackSampleIndex %d has not arrived yet"
                   , mMediaType, __func__, mOutputTrackSampleIndex);
            return NFErr_Pending;
        }

        if ( mMediaType == MEDIA_VIDEO ) break;

        AseTimeStamp const endPts = AseTimeStamp( sample.mPts, 1000 )
                                    + AseTimeStamp( fragmentPtr->mFragmentHeaderPtr->mSampleDuration,
                                                    fragmentPtr->mFragmentHeaderPtr->mTimescale );

        if ( endPts >= mAudioSynchronizationPts ) break;

        // Skip this sample
        NTRACE(TRACE_MEDIAPLAYBACK
                   , "NrdpMediaSourceBuffer(%u)::%s skipping sample with pts %lldms"
                   , mMediaType, __func__, (llong)sample.mPts );

        ++mOutputTrackSampleIndex;
        if ( mOutputTrackSampleIndex >= fragmentPtr->mFragmentHeaderPtr->mSamples.size() )
        {
            ++mOutputTrackFragmentIndex;
            mOutputTrackSampleIndex = 0;

        }

    } while( true );

    NFErr err = NFErr_OK;

    // construct SampleAttributes for audio or video
    esplayer::MediaType mediaType = (mMediaType == MEDIA_VIDEO) ? esplayer::VIDEO : esplayer::AUDIO;
    if(mediaType == esplayer::VIDEO){
        err = constructVideoSampleAttributes(fragmentPtr, sample, sampleAttributes);
    } else if( mMediaType == MEDIA_AUDIO) {

        // check first if this is the last fragment in the buffer, and audio
        // sample is within minimum pts gap range from end of the fragment. If
        // it is the case, we should not send that sample to dpi because
        // following fragment may have an overlapping samples.
        uint32_t timescale = fragmentPtr->mFragmentHeaderPtr->mTimescale;
        uint64_t sampleDuration = ( timescale ?
                                    fragmentPtr->mFragmentHeaderPtr->mSampleDuration * 1000 / timescale :
                                    20); // fallback to 20 milisecond if something is wrong

        ase::AseTimeStamp endPtsOfCurrSample = ase::AseTimeStamp(sample.mPts + sampleDuration, 1000);
        ase::AseTimeStamp ptsOfGuardedSample
            = fragmentPtr->mFragmentHeaderPtr->mEndPts
            - ( mMinAudioPtsGap + AseTimeStamp(2*sampleDuration, 1000));
        if( !mEndOfTrack &&
            (mOutputTrackFragmentIndex == (mMediaTrackFragments[mOutputTrackId].size() - 1) ) &&
            ptsOfGuardedSample < endPtsOfCurrSample )
        {
            NTRACE(TRACE_MEDIAPLAYBACK,
                   "NrdpMediaSourceBuffer(%u)::%s Pending in order not to send possibly overlapping samples",
                   mMediaType, __func__);
            NTRACE(TRACE_MEDIAPLAYBACK,
                   "NrdpMediaSourceBuffer(%u)::%s ptsOfGuardedSample %lld, endPtsOfCurrSample %lld",
                   mMediaType, __func__, ptsOfGuardedSample.getValueInMs(), endPtsOfCurrSample.getValueInMs());

            return NFErr_Pending;
        }

        constructAudioSampleAttributes(fragmentPtr, sample, sampleAttributes);
    }

    return err;
}

NFErr NrdpMediaSourceBuffer::constructVideoSampleAttributes(streamparser::MediaFragmentPtr& fragmentPtr,
                                                            streamparser::Sample& sample,
                                                            device::esplayer::SampleAttributes& sampleAttributes)
{
    streamparser::VideoTrackHeader* videoTrackHeaderPtr
        = static_cast< streamparser::VideoTrackHeader* > (fragmentPtr->mTrackHeaderPtr.get());
    uint32_t seqParamSize = videoTrackHeaderPtr->mSequenceParameterSet.size();
    uint32_t picParamSize = videoTrackHeaderPtr->mPictureParameterSet.size();
    uint32_t subSeqParamSize = videoTrackHeaderPtr->mSubsequenceParameterSet.size();
    uint32_t depViewPicParamSize = videoTrackHeaderPtr->mDependentViewPictureParameterSet.size();
    NTRACE(TRACE_MEDIAPLAYBACK,"sps size %d, pps size %d, ssps size %d, dep-pps size %d"
           , seqParamSize, picParamSize, subSeqParamSize, depViewPicParamSize);

    // reset sampleAttributes only for video because video's
    // sampleAttributes has subsample map which is getting added for each
    // call. This needs to be reset for new sample.
    esplayer::Format3D format3D = esplayer::NOT_3D;
    if(subSeqParamSize){
#if defined(USE_MVC_SPLIT)
        format3D = esplayer::MVC_SPLIT;
#else
        format3D = esplayer::MVC_COMBINED;
#endif
    }
    sampleAttributes.reset(esplayer::VIDEO, format3D);

    /*
     * **************************************
     * Codec related information at IDR frame
     * **************************************
     *
     * Here, initialize media attributes for codec related attributes
     * For video, MediaAttributes->mVideoAttributes needs to be updated for each key frame
     * For video, prepend parameters if first sample in the fragment
     */
    if( mOutputTrackSampleIndex == 0 ){
        // For video, MediaAttributes->mVideoAttributes needs to be updated for each key frame
        sampleAttributes.initMediaAttributes(esplayer::VIDEO);
        sampleAttributes.setIsKeyFrame();

#if defined(USE_MVC_SPLIT)
        // parameter sets should be written in same order to sampleWriter as SubsampleMappingEntries were added
        sampleAttributes.addSubsampleMappingEntry(seqParamSize + picParamSize, 0, 0);
        sampleAttributes.setSize(seqParamSize + picParamSize, 0);
        if(seqParamSize)
            sampleAttributes.addNaluSizeEntry(seqParamSize, 0);
        if(picParamSize)
            sampleAttributes.addNaluSizeEntry(picParamSize, 0);
        if(subSeqParamSize + depViewPicParamSize){
            sampleAttributes.addSubsampleMappingEntry(subSeqParamSize + depViewPicParamSize, 0, 1);
            sampleAttributes.setSize(subSeqParamSize + depViewPicParamSize, 1);
            sampleAttributes.addNaluSizeEntry(subSeqParamSize, 1);
            sampleAttributes.addNaluSizeEntry(depViewPicParamSize, 1);
        }
#else
        // AVC 2D or MVC combined mode
        if(seqParamSize + picParamSize + subSeqParamSize + depViewPicParamSize) {
            sampleAttributes.setSize(seqParamSize + picParamSize + subSeqParamSize + depViewPicParamSize);
            sampleAttributes.addSubsampleMappingEntry(seqParamSize + picParamSize + subSeqParamSize + depViewPicParamSize, 0, 0);

            if(seqParamSize)
                sampleAttributes.addNaluSizeEntry(seqParamSize, 0);
            if(subSeqParamSize)
                sampleAttributes.addNaluSizeEntry(subSeqParamSize, 0);
            if(picParamSize)
                sampleAttributes.addNaluSizeEntry(picParamSize, 0);
            if(depViewPicParamSize){
                sampleAttributes.addNaluSizeEntry(depViewPicParamSize, 0);
            }
        }

#if defined(DEBUG_VERBOSE)
        NTRACE(TRACE_MEDIAPLAYBACK, "%s SubSampleMap : MediaSource C(%d : SPS+PPS), E(%d)\n"
               ,__func__, seqParamSize + picParamSize, 0);
#endif

#endif
        esplayer::VideoAttributes* pVideoAttributes = sampleAttributes.mMediaAttributes.mVideoAttributes;
        if(videoTrackHeaderPtr->mFormatID == _FOURCC_LE_('a','v','c','1')){
            if(subSeqParamSize >= 8 ){
                pVideoAttributes->mCodecParam
                    = "mvc1."+ bytesToHex(&videoTrackHeaderPtr->mSubsequenceParameterSet[5], 3)+ ", ";

            } else {
                pVideoAttributes->mCodecParam = "avc1.";
                if(seqParamSize >= 8) {
                    pVideoAttributes->mCodecParam
                        += bytesToHex(&videoTrackHeaderPtr->mSequenceParameterSet[5], 3);
                }
            }
        } else if (videoTrackHeaderPtr->mFormatID == _FOURCC_LE_('h','v','c','1')) {
            pVideoAttributes->mCodecParam = "hvc1";
        } else if (videoTrackHeaderPtr->mFormatID == _FOURCC_LE_('h','e','v','1')) {
            pVideoAttributes->mCodecParam = "hev1";
        } else if (videoTrackHeaderPtr->mFormatID ==  _FOURCC_LE_('m','p','4','v')) {
            pVideoAttributes->mCodecParam = "mp4v";
        } else if (videoTrackHeaderPtr->mFormatID ==  _FOURCC_LE_('d','v','h','e')) {
            pVideoAttributes->mCodecParam = "dvhe";
        } else {
            assert(0);
        }
        pVideoAttributes->mPixelAspectRatioX = videoTrackHeaderPtr->mPixelAspectRatioX;
        pVideoAttributes->mPixelAspectRatioY = videoTrackHeaderPtr->mPixelAspectRatioY;
        pVideoAttributes->mFrameRateValue = videoTrackHeaderPtr->mFrameRateValue;
        pVideoAttributes->mFrameRateScale = videoTrackHeaderPtr->mFrameRateScale;
        pVideoAttributes->mImageWidth = videoTrackHeaderPtr->mImageWidth;
        pVideoAttributes->mImageHeight = videoTrackHeaderPtr->mImageHeight;
        pVideoAttributes->mFormat3D = format3D;

        // currently as of 2015, CE3 content with H.264 does not have framerate
        if( (videoTrackHeaderPtr->mFormatID == _FOURCC_LE_('a','v','c','1')) &&
            (videoTrackHeaderPtr->mFrameRateValue == 0) )
        {
            calculateFrameRate(fragmentPtr->mTrackHeaderPtr->mTimeScale,
                               fragmentPtr->mFragmentHeaderPtr->mSampleDuration,
                               pVideoAttributes->mFrameRateValue,
                               pVideoAttributes->mFrameRateScale);
        }

        // construct mCodecSpecificData for SampleAttributes
        //
        // TODO : let's not set pointer of videoTrackHeader's data. This opens
        // the risk that DPI can change header data
        //
        // If there are mvc sequence and picture parameter sets, the
        // order of parameter sets should be sps, ssps, pps (base), pps (dependent).

        // this initialization may need to be done at SampleAttributes::reset method
        pVideoAttributes->mCodecSpecificData.clear();
        pVideoAttributes->mCodecSpecificDataLen.clear();

        // SPS
        if (seqParamSize) {
            pVideoAttributes->mCodecSpecificData.push_back
                (const_cast<const uint8_t*> (&(videoTrackHeaderPtr->mSequenceParameterSet[0])));
            pVideoAttributes->mCodecSpecificDataLen.push_back(seqParamSize);
        }

        // SSPS
        if(subSeqParamSize) {
            pVideoAttributes->mCodecSpecificData.push_back
                (const_cast<const uint8_t*> (&(videoTrackHeaderPtr->mSubsequenceParameterSet[0])));
            pVideoAttributes->mCodecSpecificDataLen.push_back(subSeqParamSize);
        }

        // PPS - base view
        if (picParamSize) {
            pVideoAttributes->mCodecSpecificData.push_back
                (const_cast<const uint8_t*> (&(videoTrackHeaderPtr->mPictureParameterSet[0])));
            pVideoAttributes->mCodecSpecificDataLen.push_back(picParamSize);
        }

        // PPS - dependent view
        if(depViewPicParamSize) {
            pVideoAttributes->mCodecSpecificData.push_back
                (const_cast<const uint8_t*> (&(videoTrackHeaderPtr->mDependentViewPictureParameterSet[0])));
            pVideoAttributes->mCodecSpecificDataLen.push_back(depViewPicParamSize);
        }

        NTRACE(TRACE_MEDIACONTROL
               ,"NrdpMediaSourceBuffer::%s codecSpecificData.size %zu, codecSpecificLen %zu"
               , __func__
               ,pVideoAttributes->mCodecSpecificData.size(), pVideoAttributes->mCodecSpecificDataLen.size() );
    }

    /*
     * ****************
     * SampleAttributes
     * ****************
     *
     * Here, only initialize sample realted attributes, not codec related attributes.
     */
    sampleAttributes.setPTS(sample.mPts);
    sampleAttributes.setDTS(sample.mDts);
    sampleAttributes.setTimescale(fragmentPtr->mTrackHeaderPtr->mTimeScale);
    sampleAttributes.setAlgorithmID(fragmentPtr->mTrackHeaderPtr->mDefaultDrmAlgorithmID);
    sampleAttributes.setIVData(sample.mIV );
    sampleAttributes.setIVSize(sample.mIV.size());
    sampleAttributes.setKeyID(fragmentPtr->mTrackHeaderPtr->mDefaultDrmKID);
    sampleAttributes.setIsDependedOn(sample.mIsReferenced);

    NFErr err;
#if defined(USE_MVC_SPLIT)
    if(subSeqParamSize)
        err = rewriteSubSampleMapAndSampleSizeForMVCSplit(fragmentPtr, sample, sampleAttributes);
    else
        err = rewriteSubSampleMapAndSize(fragmentPtr, sample, sampleAttributes);
#else
    err = rewriteSubSampleMapAndSize(fragmentPtr, sample, sampleAttributes);
#endif
    // Dolby Vision EL set
    if( (mOutputTrackId == mSecondaryTrackId) && mIsSecondaryTrackDolbyVisionTrack ) {
        sampleAttributes.setIsDolbyVisionELFrame();
    }

    return err;
}

NFErr NrdpMediaSourceBuffer::constructAudioSampleAttributes(streamparser::MediaFragmentPtr& fragmentPtr,
                                                            streamparser::Sample& sample,
                                                            device::esplayer::SampleAttributes& sampleAttributes)
{
    if ( mOutputTrackSampleIndex == 0 ) {
        // since audio gap should exist only before first sample of fragment.
        sampleAttributes.setDiscontinuityGapInMs(fragmentPtr->mFragmentHeaderPtr->mDiscontinuityGap.getValueInMs());
    } else {
        sampleAttributes.setDiscontinuityGapInMs(0);
    }
    sampleAttributes.setSize(sample.mSize);
    sampleAttributes.setPTS(sample.mPts);
    sampleAttributes.setDTS(sample.mDts);
    sampleAttributes.setTimescale(fragmentPtr->mTrackHeaderPtr->mTimeScale);
    sampleAttributes.setAlgorithmID(fragmentPtr->mTrackHeaderPtr->mDefaultDrmAlgorithmID);
    sampleAttributes.setIVData(sample.mIV );
    sampleAttributes.setIVSize(sample.mIV.size());
    sampleAttributes.setKeyID(fragmentPtr->mTrackHeaderPtr->mDefaultDrmKID);
    sampleAttributes.setIsDependedOn(sample.mIsReferenced);

    streamparser::AudioTrackHeader* audioTrackHeaderPtr
        = static_cast< streamparser::AudioTrackHeader* > (fragmentPtr->mTrackHeaderPtr.get());

    /*
     * Here, initialize media attributes for codec
     * For audio, MediaAttributes->mAudioAttributes needs to be updated on discontinuity
     * Also, set codecSpecificData and create ADTS header if needed
     */
    if ( mAudioDiscontinuity) {
        sampleAttributes.initMediaAttributes(esplayer::AUDIO);
        device::esplayer::AudioAttributes* pAudioAttributes
            = sampleAttributes.mMediaAttributes.mAudioAttributes;

        if(audioTrackHeaderPtr->mFormatID ==  0x1610) {
            pAudioAttributes->mCodecParam = "mp4a.40.2, mp4a.40.5";
        } else if (audioTrackHeaderPtr->mFormatID == 0xeac3) {
            pAudioAttributes->mCodecParam = "ec-3.A6";
        } else if (audioTrackHeaderPtr->mFormatID == 0x6771) {
            pAudioAttributes->mCodecParam = "ogg vorbis";
        }
        pAudioAttributes->mNumberOfChannels = audioTrackHeaderPtr->mNumberOfChannels;
        pAudioAttributes->mSamplesPerSecond = audioTrackHeaderPtr->mSamplesPerSecond;

        pAudioAttributes->mBitrate = audioTrackHeaderPtr->mNominalBitrateInKbps;

        pAudioAttributes->mBlockAlignment =  audioTrackHeaderPtr->mBlockAlignment;
        pAudioAttributes->mCodecSpecificDataLen = audioTrackHeaderPtr->mCodecSpecificData.size();
        if(pAudioAttributes->mCodecSpecificDataLen){
            pAudioAttributes->mCodecSpecificData =
                (const_cast<const uint8_t*>(&audioTrackHeaderPtr->mCodecSpecificData[0]));
        }

        // this may need mutex protection
        mAudioDiscontinuity = false;
    } else {
        sampleAttributes.mMediaAttributes.mAudioAttributes = NULL;
    }

    if ( audioTrackHeaderPtr->mFormatID == 0x1610 ){
        // AAC
        updateADTSHeaderSizeBytes(audioTrackHeaderPtr->mADTSHeader, sample.mSize);
        sampleAttributes.setSize(sample.mSize + audioTrackHeaderPtr->mADTSHeader.size());
    }

    return NFErr_OK;
}

device::esplayer::Status NrdpMediaSourceBuffer::peekNextSampleAttributes(
    device::esplayer::SampleAttributes& sampleAttributes)
{
    //NTRACE(TRACE_MEDIACONTROL,"NrdpMediaSourceBuffer::peekNextSampleAttributes\n");

    ScopedMutex scopedMutex(mMutex);
    streamparser::MediaFragmentPtr fragmentPtr;
    streamparser::Sample sample;

    // construct SampleAttributes
    NFErr err = constructSampleAttributes(fragmentPtr, sample, sampleAttributes);
    if(err == NFErr_Pending){
        return device::esplayer::NO_AVAILABLE_SAMPLES;
    } else if (err == NFErr_EndOfStream ) {
        return device::esplayer::END_OF_STREAM;
    } else if (err == NFErr_MP4ParsedData ) {
        return device::esplayer::STREAM_ERROR;
    }
    return device::esplayer::OK;
}

device::esplayer::Status NrdpMediaSourceBuffer::getNextSample(
    device::esplayer::ISampleWriter& sampleWriter,
    device::esplayer::SampleAttributes& sampleAttributes)
{
    //NTRACE(TRACE_MEDIACONTROL,"[%u]NrdpMediaSourceBuffer::getNextSample\n", mMediaType);

    ScopedMutex scopedMutex(mMutex);
    streamparser::MediaFragmentPtr fragmentPtr;
    streamparser::Sample sample;

    if(isSampleFeedingDisabled()){
        NTRACE(TRACE_MEDIACONTROL,"[%u]NrdpMediaSourceBuffer::getNextSample sample feeding disabled\n", mMediaType);
        return device::esplayer::NO_AVAILABLE_SAMPLES;
    }

    // construct SampleAttributes
    NFErr err = constructSampleAttributes(fragmentPtr, sample, sampleAttributes);
    if(err == NFErr_Pending){
        return device::esplayer::NO_AVAILABLE_SAMPLES;
    } else if (err == NFErr_EndOfStream ) {
        return device::esplayer::END_OF_STREAM;
    } else if (err == NFErr_MP4ParsedData ) {
        return device::esplayer::STREAM_ERROR;
    }

    // write sample to sample writer
    esplayer::Status status = sampleWriter.initSample(sampleAttributes);
    if( OK != status ){
        return status;
    }

    status = writeSample(sampleWriter, fragmentPtr, sample);
    if( OK != status ){
        return status;
    }

    if(mMediaType == netflix::ase::MEDIA_AUDIO){
        mOutputTrackSampleIndex++;
    } else {
        // update track, fragment and sample for next getNextSample
        //if ( mMediaTrackFragments.size() > 1 ) {
        if ( mTrackHeaders.size() > 1 ) {
            // for multi track case
            if ( mOutputTrackId == mPrimaryTrackId ){
                // primary track sample written
                mOutputTrackId = mSecondaryTrackId;
            } else {
                // secondary track sample written
                mOutputTrackId = mPrimaryTrackId;
                mOutputTrackSampleIndex++;
            }
        } else {
            // for signle track case
            mOutputTrackSampleIndex++;
        }
    }

    // this is when current fragment data are all delivered.
    if( mOutputTrackSampleIndex >= fragmentPtr->mFragmentHeaderPtr->mSamples.size() )
    {
        // no more sample in this fragment. move on to next fragment. For multi
        // track content, this will be updated when last track data are all
        // delivered.
        mOutputTrackFragmentIndex++;
        mOutputTrackSampleIndex = 0;
    }

    return device::esplayer::OK;
}

device::esplayer::Status NrdpMediaSourceBuffer::writeSample(device::esplayer::ISampleWriter& sampleWriter,
                                                            streamparser::MediaFragmentPtr& fragmentPtr,
                                                            streamparser::Sample& sample)
{
    device::esplayer::Status status = device::esplayer::OK;
    static const uint8_t startcode[4] = {0,0,0,1};
    streamparser::VideoTrackHeader* videoTrackHeaderPtr
        = static_cast< streamparser::VideoTrackHeader* > (fragmentPtr->mTrackHeaderPtr.get());

    /*
     * *******************
     * write paremter sets
     * *******************
     */
    if( (mOutputTrackSampleIndex == 0) && (mMediaType == MEDIA_VIDEO) ){
        // For video, SPS/PPS needs to be in the stream
        if( (videoTrackHeaderPtr->mSequenceParameterSet.size() > 4) &&
            (videoTrackHeaderPtr->mPictureParameterSet.size() > 4 ) ){
            // SPS
            WRITE_TO_SAMPLEWRITER(startcode, 4);
            WRITE_TO_SAMPLEWRITER(&videoTrackHeaderPtr->mSequenceParameterSet[4],
                                  videoTrackHeaderPtr->mSequenceParameterSet.size() - 4);
            // PPS
            WRITE_TO_SAMPLEWRITER(startcode, 4);
            WRITE_TO_SAMPLEWRITER(&videoTrackHeaderPtr->mPictureParameterSet[4],
                                  videoTrackHeaderPtr->mPictureParameterSet.size() - 4);

            if ( (videoTrackHeaderPtr->mSubsequenceParameterSet.size() > 4) &&
                 (videoTrackHeaderPtr->mDependentViewPictureParameterSet.size() > 4)){
                //printf("size ssps %d\n", videoTrackHeaderPtr->mSubsequenceParameterSet.size());
                //printf("size d-pps %d\n", videoTrackHeaderPtr->mDependentViewPictureParameterSet.size());
                uint32_t viewId = 0;
#if defined(USE_MVC_SPLIT)
                viewId = 1;
#endif
                // SSPS
                WRITE_TO_SAMPLEWRITER_VIEWID(startcode, 4, viewId);
                WRITE_TO_SAMPLEWRITER_VIEWID(&videoTrackHeaderPtr->mSubsequenceParameterSet[4],
                                             videoTrackHeaderPtr->mSubsequenceParameterSet.size() - 4, viewId);
                // PPS for dependent view
                WRITE_TO_SAMPLEWRITER_VIEWID(startcode, 4, viewId);
                WRITE_TO_SAMPLEWRITER_VIEWID(&videoTrackHeaderPtr->mDependentViewPictureParameterSet[4],
                                             videoTrackHeaderPtr->mDependentViewPictureParameterSet.size() - 4, viewId);
            }
        } else {
            // avc1 has sps/pps/ssps in moov box. those can not be in mdat.
            // hevc1 can not have those in moov box. those should be in mdat.
            NTRACE(TRACE_MEDIAPLAYBACK, "SPS/PPS size too small (abnormal for avc1, normal for hev1): sps %zu, pps %zu"
                   , videoTrackHeaderPtr->mSequenceParameterSet.size(), videoTrackHeaderPtr->mPictureParameterSet.size());
        }
    }

    // For audio, especially HE-AAC, need ADTS header in front of sample
    if ( mMediaType == MEDIA_AUDIO ){
        streamparser::AudioTrackHeader* audioTrackHeaderPtr
            = static_cast< streamparser::AudioTrackHeader* > (fragmentPtr->mTrackHeaderPtr.get());
        if ( (audioTrackHeaderPtr->mFormatID == 0x1610) && audioTrackHeaderPtr->mADTSHeader.size() ){
            // prepend ADTS header
            WRITE_TO_SAMPLEWRITER(&audioTrackHeaderPtr->mADTSHeader[0], audioTrackHeaderPtr->mADTSHeader.size());
        }
    }

    /*
     * *****************
     * write sample data
     * *****************
     */
    // 1.find the data block (AseBuffer) that has the first byte of Sample
    uint32_t sampleSizeRemaining = sample.mSize;
    uint32_t dataBlockIndex = 0;
    for ( ; dataBlockIndex < fragmentPtr->data().size(); dataBlockIndex++ ) {
        if( fragmentPtr->data()[dataBlockIndex].first > static_cast<int32_t>(sample.mFragmentOffset) ){
            break;
        }
    }
    dataBlockIndex--;

    // 2. write into ISampleWriter
    // nal unit writing handling
    uint8_t buff[4]; // buffer to save nal size
    uint32_t remainingNalSizeToWrite = 0; // remaining nal size to write with next block
    uint32_t remainingNalLengthBytes = 0;
    uint32_t naluNum = 0;
    uint32_t viewId = 0;
    uint32_t fragmentOffset = sample.mFragmentOffset;

    while(sampleSizeRemaining){
        const uint8_t* startPtr = fragmentPtr->data()[dataBlockIndex].second->getFilledSpaceStart()
            + (fragmentOffset - fragmentPtr->data()[dataBlockIndex].first);

        uint32_t remainingDataInCurrentDataBlock
            = fragmentPtr->data()[dataBlockIndex].second->getFilledSpaceSize()
            - (startPtr - fragmentPtr->data()[dataBlockIndex].second->getFilledSpaceStart());

        uint32_t writeSize;
        if(sampleSizeRemaining > remainingDataInCurrentDataBlock){
            // write all remaining data in current data block, and proceed block index
            writeSize = remainingDataInCurrentDataBlock;
            dataBlockIndex++;
        } else {
            writeSize = sampleSizeRemaining;
        }

        /*
         * **********************
         * startup code injection
         * **********************
         */
        if(mMediaType == MEDIA_AUDIO){
            WRITE_TO_SAMPLEWRITER(startPtr, writeSize);
        } else {
            // written size spanning multiple nals
            uint32_t writtenNalSize = 0;
            while( writtenNalSize < writeSize) {
                // start of new nal
                if(remainingNalSizeToWrite == 0) {

                    if( remainingDataInCurrentDataBlock >= 4 ){
                        // if there is nal length bytes to copy
                        if( remainingNalLengthBytes) {
                            for(uint32_t i = 0; i < remainingNalLengthBytes; i++) {
                                buff[i + 4 - remainingNalLengthBytes ] = *((uint8_t*)startPtr++);
                            }
                            remainingNalSizeToWrite = getNalSize(buff, 4);
                            remainingDataInCurrentDataBlock -= remainingNalLengthBytes;
                            writtenNalSize += remainingNalLengthBytes;
                            //printf("writtenNalSize %d, nalSize %d\n", writtenNalSize, remainingNalSizeToWrite);
                        } else {
                            remainingNalSizeToWrite = getNalSize(startPtr, 4);
                            startPtr +=4;
                            remainingDataInCurrentDataBlock -= 4;
                            writtenNalSize += 4;
                            //printf("writtenNalSize %d, nalSize %d, 0x%x\n", writtenNalSize, remainingNalSizeToWrite, startPtr );
                        }
#if defined(USE_MVC_SPLIT)
                        if( videoTrackHeaderPtr->mNaluToMvcViewMaps.size() )
                            viewId = videoTrackHeaderPtr->mNaluToMvcViewMaps[sample.mSampleGroup][naluNum];
#endif
                        if( sampleWriter.write(startcode, 4, viewId) != 4){
                            assert(0);
                            return device::esplayer::SAMPLE_WRITER_ERROR;
                        }
                        remainingNalLengthBytes = 0; // reset so that no copy into buff can happen
                        //printf("nalSize[%d] = %d\n", naluNum, remainingNalSizeToWrite);
                        naluNum++;
                    } else {
                        for(uint32_t i = 0; i < remainingDataInCurrentDataBlock; i++) {
                            buff[i] = *((uint8_t*)startPtr++);
                            writtenNalSize++;
                        }
                        remainingNalLengthBytes = 4 - remainingDataInCurrentDataBlock;
                        remainingDataInCurrentDataBlock = 0;
                        //printf("remainingNalLengthBytes %d\n", remainingNalLengthBytes);
                        // no more data in current block. break out of loop.
                        break;
                    }
                }

                if(remainingDataInCurrentDataBlock > remainingNalSizeToWrite){
                    // all data for current nal is in current block
                    if( remainingNalSizeToWrite != sampleWriter.write(startPtr, remainingNalSizeToWrite, viewId) ){
                        assert(0);
                        return device::esplayer::SAMPLE_WRITER_ERROR;
                    }
                    startPtr += remainingNalSizeToWrite;
                    remainingDataInCurrentDataBlock -= remainingNalSizeToWrite;
                    writtenNalSize += remainingNalSizeToWrite;
                    remainingNalSizeToWrite = 0; // finish writing current nal
                } else {
                    // if next block is needed for current nal
                    if( remainingDataInCurrentDataBlock != sampleWriter.write(startPtr, remainingDataInCurrentDataBlock, viewId) ){
                        assert(0);
                        return device::esplayer::SAMPLE_WRITER_ERROR;
                    }
                    startPtr += remainingDataInCurrentDataBlock;
                    remainingNalSizeToWrite -= remainingDataInCurrentDataBlock;
                    writtenNalSize += remainingDataInCurrentDataBlock;
                    remainingDataInCurrentDataBlock = 0;
                }
            } // end of while

            if( writeSize != writtenNalSize) {
                //printf("writeSize %d, writtenNalSize %d\n",writeSize, writtenNalSize);
                assert(writeSize == writtenNalSize);
                return device::esplayer::SAMPLE_WRITER_ERROR;
            }
        }

        sampleSizeRemaining -= writeSize;
        fragmentOffset += writeSize;

#if defined(DEBUG_VERBOSE)
        NTRACE(TRACE_MEDIAPLAYBACK
               ,"[%u] %s SampleSize %d : written size %d, remaining %d, offset %d (sampleOffset %d)\n"
               , mMediaType, __func__, sample.mSize
               , writeSize, sampleSizeRemaining, fragmentOffset, sample.mFragmentOffset );
#endif
    }
    return status;
}

/*
 * TODO: improvement to avoid duplicated code with about writeSample method
 */
NFErr NrdpMediaSourceBuffer::createNalSizeMap(streamparser::MediaFragmentPtr& fragmentPtr,
                                              streamparser::Sample& sample)
{
    mNalSizeMap.clear();

#if defined(USE_MVC_SPLIT)
    mMvcNalSizeMap[0].clear();
    mMvcNalSizeMap[1].clear();
    streamparser::VideoTrackHeader* videoTrackHeaderPtr
        = static_cast< streamparser::VideoTrackHeader* > (fragmentPtr->mTrackHeaderPtr.get());
#endif

    // 1.find the data block (AseBuffer) that has the first byte of Sample
    uint32_t sampleSizeRemaining = sample.mSize;
    uint32_t dataBlockIndex = 0;
    for ( ; dataBlockIndex < fragmentPtr->data().size(); dataBlockIndex++ ) {
        if( fragmentPtr->data()[dataBlockIndex].first > static_cast<int32_t>(sample.mFragmentOffset) ){
            break;
        }
    }
    dataBlockIndex--;

    uint8_t buff[4]; // buffer to save nal size
    uint32_t remainingNalSizeToWrite = 0; // remaining nal size to write with next block
    uint32_t remainingNalLengthBytes = 0;
    uint32_t naluNum = 0;

    uint32_t fragmentOffset = sample.mFragmentOffset;
    while(sampleSizeRemaining){
        const uint8_t* startPtr = fragmentPtr->data()[dataBlockIndex].second->getFilledSpaceStart()
            + (fragmentOffset - fragmentPtr->data()[dataBlockIndex].first);

        uint32_t remainingDataInCurrentDataBlock
            = fragmentPtr->data()[dataBlockIndex].second->getFilledSpaceSize()
            - (startPtr - fragmentPtr->data()[dataBlockIndex].second->getFilledSpaceStart());

        uint32_t writeSize;
        if(sampleSizeRemaining > remainingDataInCurrentDataBlock){
            // write all remaining data in current data block, and proceed block index
            writeSize = remainingDataInCurrentDataBlock;
            dataBlockIndex++;
        } else {
            writeSize = sampleSizeRemaining;
        }

        // written size spanning multiple nals
        uint32_t writtenNalSize = 0;
        while( writtenNalSize < writeSize) {
            // start of new nal
            if(remainingNalSizeToWrite == 0) {

                if( remainingDataInCurrentDataBlock >= 4 ){
                    // if there is nal length bytes to copy
                    if( remainingNalLengthBytes) {
                        for(uint32_t i = 0; i < remainingNalLengthBytes; i++) {
                            buff[i + 4 - remainingNalLengthBytes ] = *((uint8_t*)startPtr++);
                        }
                        remainingNalSizeToWrite = getNalSize(buff, 4);
                        remainingDataInCurrentDataBlock -= remainingNalLengthBytes;
                        writtenNalSize += remainingNalLengthBytes;
                        //printf("writtenNalSize %d, nalSize %d\n", writtenNalSize, remainingNalSizeToWrite);
                    } else {
                        remainingNalSizeToWrite = getNalSize(startPtr, 4);
                        startPtr +=4;
                        remainingDataInCurrentDataBlock -= 4;
                        writtenNalSize += 4;
                        //printf("writtenNalSize %d, nalSize %d, 0x%x\n", writtenNalSize, remainingNalSizeToWrite, startPtr );
                    }
#if defined(USE_MVC_SPLIT)
                    uint32_t viewId = 0;
                    if( videoTrackHeaderPtr->mNaluToMvcViewMaps.size() )
                        viewId = videoTrackHeaderPtr->mNaluToMvcViewMaps[sample.mSampleGroup][naluNum];
                    mMvcNalSizeMap[viewId].push_back(remainingNalSizeToWrite + 4);
#endif
                    mNalSizeMap.push_back(remainingNalSizeToWrite + 4);
                    remainingNalLengthBytes = 0; // reset so that no copy into buff can happen
                    //printf("nalSize[%d] = %d\n", naluNum, remainingNalSizeToWrite);
                    naluNum++;
                } else {
                    for(uint32_t i = 0; i < remainingDataInCurrentDataBlock; i++) {
                        buff[i] = *((uint8_t*)startPtr++);
                        writtenNalSize++;
                    }
                    remainingNalLengthBytes = 4 - remainingDataInCurrentDataBlock;
                    remainingDataInCurrentDataBlock = 0;
                    //printf("remainingNalLengthBytes %d\n", remainingNalLengthBytes);
                    // no more data in current block. break out of loop.
                    break;
                }
            }

            if(remainingDataInCurrentDataBlock > remainingNalSizeToWrite){
                startPtr += remainingNalSizeToWrite;
                remainingDataInCurrentDataBlock -= remainingNalSizeToWrite;
                writtenNalSize += remainingNalSizeToWrite;
                remainingNalSizeToWrite = 0; // finish writing current nal
            } else {
                // if next block is needed for current nal
                 startPtr += remainingDataInCurrentDataBlock;
                remainingNalSizeToWrite -= remainingDataInCurrentDataBlock;
                writtenNalSize += remainingDataInCurrentDataBlock;
                remainingDataInCurrentDataBlock = 0;
            }
        } // end of while

        if( writeSize != writtenNalSize) {
            Log::error(TRACE_MEDIAPLAYBACK, "createNalSizeMap: writeSize %d, writtenNalSize %d\n", writeSize, writtenNalSize);
            return NFErr_MP4ParsedData;
        }

        sampleSizeRemaining -= writeSize;
        fragmentOffset += writeSize;
    }
    return NFErr_OK;
}

NFErr NrdpMediaSourceBuffer::rewriteSubSampleMapAndSize(streamparser::MediaFragmentPtr& fragmentPtr,
                                                        streamparser::Sample& sample,
                                                        device::esplayer::SampleAttributes& sampleAttributes)
{
    // sub sample map entry should be constructed after sps/pps map is added

    // 1. nalu map creating
    NFErr err = createNalSizeMap(fragmentPtr, sample);

    if (err != NFErr_OK) {
        return err;
    }

    for(uint32_t i=0; i < mNalSizeMap.size(); i++){
        sampleAttributes.addNaluSizeEntry(mNalSizeMap[i], 0);
    }
    mNalSizeMap.clear();

    // 2. sub sample map creating
    for(uint32_t i=0; i < sample.mSubSampleMap.size() ; i+= 2)
    {
        sampleAttributes.addSubsampleMappingEntry(sample.mSubSampleMap[i],
                                                  sample.mSubSampleMap[i+1]);
#if defined(DEBUG_VERBOSE)
        NTRACE(TRACE_MEDIAPLAYBACK
               , "%s SubSampleMap : MediaSource C(%d),E(%d) sampleSize without SPS/PPS (%d)\n"
               ,__func__
               ,sample.mSubSampleMap[i]
               ,sample.mSubSampleMap[i+1]
               ,sample.mSize);
#endif
    }

    // 3. set sample size
    sampleAttributes.setSize(sampleAttributes.getSize() + sample.mSize);

    return NFErr_OK;
}

NFErr NrdpMediaSourceBuffer::rewriteSubSampleMapAndSampleSizeForMVCSplit(
    streamparser::MediaFragmentPtr& fragmentPtr,
    streamparser::Sample& sample,
    device::esplayer::SampleAttributes& sampleAttributes)
{
    uint32_t i = 0, viewId = 0;
    uint32_t viewSize[2];
    streamparser::VideoTrackHeader* videoTrackHeaderPtr
        = static_cast< streamparser::VideoTrackHeader* > (fragmentPtr->mTrackHeaderPtr.get());

    if(!videoTrackHeaderPtr->mNaluToMvcViewMaps.size()){
        return NFErr_OK;
    }

    // 1, nal size map creation
    NFErr err = createNalSizeMap(fragmentPtr, sample);
    if (err != NFErr_OK) {
        return err;
    }

    for(i=0; i<mMvcNalSizeMap[0].size(); i++){
        sampleAttributes.addNaluSizeEntry(mMvcNalSizeMap[0][i], 0);
    }
    for(i=0; i<mMvcNalSizeMap[1].size(); i++){
        sampleAttributes.addNaluSizeEntry(mMvcNalSizeMap[1][i], 1);
    }
    mMvcNalSizeMap[0].clear();
    mMvcNalSizeMap[1].clear();

    // 2. sub sample map entry creation
    // sub sample map entry should be constructed after sps/pps map is added
    for(i=0, viewSize[0]=0, viewSize[1]=0; i < sample.mSubSampleMap.size() ; i+= 2){
        viewId = videoTrackHeaderPtr->mNaluToMvcViewMaps[sample.mSampleGroup][i/2];
        sampleAttributes.addSubsampleMappingEntry(sample.mSubSampleMap[i], sample.mSubSampleMap[i+1], viewId);
        viewSize[viewId] += sample.mSubSampleMap[i];
        viewSize[viewId] += sample.mSubSampleMap[i+1];

#if defined(DEBUG_VERBOSE)
        NTRACE(TRACE_MEDIAPLAYBACK, "%s SubSampleMap MVC(%d): MediaSource C(%d),E(%d) sampleSize without SPS/PPS (%d)\n"
               ,__func__
               ,viewId
               ,sample.mSubSampleMap[i]
               ,sample.mSubSampleMap[i+1]
               ,sample.mSize);
#endif
    }
#if defined(DEBUG_VERBOSE)
    NTRACE(TRACE_MEDIAPLAYBACK, "%s viewSize base %d, dep %d, total %d\n"
           , __func__, viewSize[0], viewSize[1], viewSize[0] + viewSize[1]);
#endif
    // 3. sample size setting
    // in case of IDR, sampleAttributes.setSize is already set with parameter sets. Take those values account.
    sampleAttributes.setSize(sampleAttributes.getSize(0) + viewSize[0], 0);
    sampleAttributes.setSize(sampleAttributes.getSize(1) + viewSize[1], 1);

    return err;
}

void NrdpMediaSourceBuffer::calculateFrameRate(uint32_t timescale,
                                               uint32_t sampleDuration,
                                               uint32_t& framerateValue,
                                               uint32_t& framerateScale)
{
    double a = (double)256*timescale;
    double b = sampleDuration;

    if(sampleDuration){
        double c = a / b;
        c += 0.5;
        framerateValue = (uint32_t)(c);
        framerateScale = 256;
    }

    NTRACE(TRACE_MEDIAPLAYBACK
           , "timescale %d, sampleDuration %d, frameRateValue %d, frameRateScale %d\n"
           , timescale, sampleDuration, framerateValue, framerateScale);
}
