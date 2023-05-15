/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "MediaFragment.h"

#include <nrdbase/Log.h>

using namespace netflix::ase;
using namespace std;
using namespace netflix;

namespace netflix {
namespace ase {
DEFINE_LOGGER( MediaFragmentLog );
}
}

AseErrorCode MediaFragment::construct(
                          uint32_t fragmentId,
                          int32_t manifestId,
                          int32_t trackId,
                          MediaType mediaType,
                          uint32_t cdnId,
                          AseTimeStamp const&      startPts,
                          MediaFragmentInfo const&     fragmentInfo,
                          shared_ptr<MediaStream> pMediaStream,
                          shared_ptr<MediaPresentation>    pMediaPresentation,
                          IMediaSinkPtr const&   pMediaSink,
                          uint32_t bufferUnitSize,
                          IMediaBufferPoolPtr pMediaBufferPool,
                          bool lastFragment,
                          ByteRange const& byteRange,
                          uint32_t numOfFragments,
                          std::deque<ullong> const& startOfFragmentsByteOffsets,
                          int32_t predictedDlTimeForBackupStream,
                          uint32_t maxDataDryPeriod,
                          bool forwardDataBeforeComplete,
                          uint32_t minBlockCompleteTime,
                          uint32_t blockReceptionBufferBudget,
                          uint32_t abandonBlockControlTime,
                          uint32_t abandonBlockControlPct,
                          uint32_t alternativeFragmentSize,
                          MediaFragmentPtr&     pFragment)
{
    pFragment.reset( new MediaFragment( fragmentId,
                                        manifestId, trackId,
                                        mediaType, cdnId,
                                        startPts, fragmentInfo,
                                        pMediaStream, pMediaPresentation,
                                        pMediaSink, bufferUnitSize, pMediaBufferPool,
                                        lastFragment,
                                        byteRange,
                                        numOfFragments, startOfFragmentsByteOffsets,
                                        predictedDlTimeForBackupStream,
                                        maxDataDryPeriod,
                                        forwardDataBeforeComplete,
                                        minBlockCompleteTime,
                                        blockReceptionBufferBudget,
                                        abandonBlockControlTime,
                                        abandonBlockControlPct,
                                        alternativeFragmentSize) );
    return AS_NO_ERROR;
}

MediaFragment::MediaFragment( uint32_t fragmentId,
                          int32_t manifestId, int32_t trackId,
                          MediaType mediaType, uint32_t cdnId,
                          AseTimeStamp const&      startPts,
                          MediaFragmentInfo const&     fragmentInfo,
                          shared_ptr<MediaStream> pMediaStream,
                          shared_ptr<MediaPresentation>    pMediaPresentation,
                          IMediaSinkPtr const&   pMediaSink,
                          uint32_t bufferUnitSize,
                          IMediaBufferPoolPtr pMediaBufferPool,
                          bool lastFragment,
                          ByteRange const& byteRange,
                          uint32_t numOfFragments,
                          std::deque<ullong> const& startOfFragmentsByteOffsets,
                          int32_t predictedDlTimeForBackupStream,
                          uint32_t maxDataDryPeriod,
                          bool forwardDataBeforeComplete,
                          uint32_t minBlockCompleteTime,
                          uint32_t blockReceptionBufferBudget,
                          uint32_t abandonBlockControlTime,
                          uint32_t abandonBlockControlPct,
                          uint32_t alternativeFragmentSize)
                    : mMediaStreamPtr(pMediaStream)
                    , mMediaPresentationPtr(pMediaPresentation)
                    , mMediaSinkPtr(pMediaSink)
                    , mNextMediaFragmentPtr(shared_ptr<MediaFragment>())
                    , mPrevMediaFragmentPtr(shared_ptr<MediaFragment>())
                    , mBufferUnitSize(bufferUnitSize)
                    , mMediaBufferPoolPtr(pMediaBufferPool)
                    , mForward(false)
                    , mFragmentId(fragmentId)
                    , mManifestId(manifestId)
                    , mTrackId(trackId)
                    , mMediaType(mediaType)
                    , mCdnId(cdnId)
                    , mFragmentStartPts(startPts)
                    , mExpectedSequenceNum(0)
                    , mFinalBlock(false)
                    , mLastFragment(lastFragment)
                    , mDownloadedAndForwarded(false)
                    , mByteRange(byteRange)
                    , mAllowToForward(forwardDataBeforeComplete)
                    , mPredictDlTimeForBackup(predictedDlTimeForBackupStream)
                    , mNumOfFragments(numOfFragments)
                    , mStartOfFragmentsByteOffsets(startOfFragmentsByteOffsets)
                    , mDataFromPreviousBlock(NULL)
                    , mDataSizeFromPreviousBlock(0)
                    , mMaxDataDryPeriod(maxDataDryPeriod)
                    , mHasStartedForwarding(false)
                    , mNumOfBlocksReceived(0)
                    , mNumOfBlocksDelivered(0)
                    , mCompletionDeadline( AseTimeVal::INFINITE )
                    , mFragmentCreateTime( AseTimeVal::now() )
                    , mAbandonBlockControlTime( mFragmentCreateTime + AseTimeVal::fromMS(abandonBlockControlTime) )
                    , mFirstDataArrivalTime( AseTimeVal::ZERO )
                    , mAbandonBlockControlPct( abandonBlockControlPct )
                    , mAlternativeFragmentSize( alternativeFragmentSize )
{
    mFragmentInfo.mDuration = fragmentInfo.mDuration;
    mFragmentInfo.mSize = fragmentInfo.mSize;
    mFragmentInfo.mReceived = 0;
    mFragmentInfo.mStreamId = mMediaStreamPtr.lock()->streamId();

    double currentPlaybackRate;
    AseTimeStamp currentPlaybackPosition;

    mMediaSinkPtr->currentPlaybackInfo( currentPlaybackRate, currentPlaybackPosition );
    if ( currentPlaybackRate > 0 )
    {
        uint32_t timeNeedThisBlock = (startPts - currentPlaybackPosition).getValueInMs();
        if (timeNeedThisBlock > minBlockCompleteTime)
        {
            uint32_t const scaledTimeNeedThisBlock = timeNeedThisBlock * blockReceptionBufferBudget / 100;
            timeNeedThisBlock = scaledTimeNeedThisBlock > minBlockCompleteTime
                                        ? scaledTimeNeedThisBlock
                                        : minBlockCompleteTime;
        }

        mCompletionDeadline = mFragmentCreateTime + AseTimeVal::fromMS( timeNeedThisBlock );
    }

    uint32_t numOfBlocks = computeNumberOfBlocks((uint32_t)mFragmentInfo.mSize, bufferUnitSize);
    mFinalSequenceNum = numOfBlocks;

    if ( mNumOfFragments > 1 )
    {
        mDataFromPreviousBlock = new unsigned char[mBufferUnitSize];
        memset(mDataFromPreviousBlock, 0, mBufferUnitSize);
    }

    mCurrentByteOffset = mByteRange.getStart();
    mCurrentGopStartPts = mFragmentStartPts;
    mCurrentTimestamp = AseTimeStamp::INFINITE;
    mGopDuration = AseTimeStamp(mFragmentInfo.mDuration.getValueInMs() / mNumOfFragments, 1000);

    numOfBlocks = computeNumberOfBlocks( computeCurrentGopSize(), bufferUnitSize);
    mTimeSlicePerBlock = AseTimeStamp(mGopDuration.getValueInMs() * 1000 / numOfBlocks, 1000 * 1000);



    mLastBlockMetaInfo.reset();

    DEBUGLOG(MediaFragmentLog,
                "constructed fragment at track %d, stream %d, with start pts: %lld ms, number of fragments: %u, time slice per block: %llu ms",
                mTrackId,
                mFragmentInfo.mStreamId,
                mFragmentStartPts.getValueInMs(),
                mNumOfFragments,
                mTimeSlicePerBlock.getValueInMs());
}

MediaFragment::~MediaFragment()
{
    mBufferedData.clear();

#if 0
    DEBUGLOG(MediaFragmentLog,
                "destructing framgent at track %d, stream %d, with start pts: %lld ms",
                mTrackId,
                mFragmentInfo.mStreamId,
                mFragmentStartPts.getValueInMs());
#endif

    if ( mDataFromPreviousBlock )
    {
        delete[] mDataFromPreviousBlock;
    }
}

size_t MediaFragment::getBytesReceived() const
{
    return mFragmentInfo.mReceived;
}

uint32_t MediaFragment::remainingBytes() const
{
    return (mFragmentInfo.mSize - mFragmentInfo.mReceived);
}

AseTimeStamp MediaFragment::getStartPts() const
{
    return mFragmentStartPts;
}

uint32_t MediaFragment::getFragmentId() const
{
    return mFragmentId;
}

BufferedFragmentInfo MediaFragment::getFragmentInfo() const
{
    return mFragmentInfo;
}

void MediaFragment::setNextFragment( shared_ptr<MediaFragment> pFragment )
{
    mNextMediaFragmentPtr = pFragment;
}

void MediaFragment::setPrevFragment( shared_ptr<MediaFragment> pFragment )
{
    mPrevMediaFragmentPtr = pFragment;
}

void MediaFragment::setMediaRequest( shared_ptr<MediaRequest> pMediaRequest )
{
    mMediaRequestPtr = pMediaRequest;
}

shared_ptr<MediaFragment> MediaFragment::getNextFragment()
{
    return mNextMediaFragmentPtr.lock();
}

shared_ptr<MediaFragment> MediaFragment::getPrevFragment()
{
    return mPrevMediaFragmentPtr.lock();
}

AseErrorCode MediaFragment::getNextRequestRange(ByteRange& byteRange)
{
    AseErrorCode retCode = AS_NO_ERROR;
    byteRange = mByteRange;
    return retCode;
}

AseTimeVal MediaFragment::getCompletionDeadline() const
{
    return mCompletionDeadline;
}

bool MediaFragment::downloadedAndForwarded()
{
    return mDownloadedAndForwarded;
}

void MediaFragment::startForwarding()
{
    NTRACE(TRACE_STREAMINGSTAT,
            "Start forwarding fragment for track: %d, stream %d, with start pts: %lld ms, allow to Forward: %u, "
            "byte range: %lld-%lld",
            mTrackId,
            mFragmentInfo.mStreamId,
            mFragmentStartPts.getValueInMs(),
            mAllowToForward,
            mByteRange.getStart(),
            mByteRange.getEnd());
    mForward = true;
    if ( mAllowToForward )
        deliverDataToSink();
}

int MediaFragment::provideResponseBody( AseBufferPtr const& pBuffer, int sequence, AseTimeVal& abandontime )
{
    MediaRequestPtr const pMediaRequest = mMediaRequestPtr.lock();
    if ( pMediaRequest && pMediaRequest->isAbandoned() )
    {
        abandontime = AseTimeVal::ZERO;
        return 0;
    }
    else
    {
        abandontime = AseTimeVal::INFINITE;
    }

    AseTimeVal const now  = AseTimeVal::now();
    if ( mFirstDataArrivalTime == AseTimeVal::ZERO )
        mFirstDataArrivalTime = now;

    ++mNumOfBlocksReceived;

    BufferObj bufferObj = BufferObj(pBuffer, sequence);
    list<BufferObj>::iterator insertPos;
    // insert the buffer into the buffer data list
    if (mBufferedData.empty())
        mBufferedData.push_back(bufferObj);
    else
    {
        for ( insertPos =  mBufferedData.begin();
                insertPos != mBufferedData.end();
                ++insertPos )
        {
            if ( insertPos->mSequence > sequence )
                break;
        }
        if ( insertPos == mBufferedData.end() )
            mBufferedData.push_back(bufferObj);
        else
            mBufferedData.insert(insertPos, bufferObj);
    }

    mFragmentInfo.mReceived += pBuffer->getFilledSpaceSize();
    mMediaPresentationPtr.lock()->updateDataReception(pBuffer->getFilledSpaceSize());

    if (mBufferedData.size() == mFinalSequenceNum)
    {
        mAllowToForward = true;
    }

    if ( mForward && mAllowToForward )
    {
        deliverDataToSink();
    }

    if ( mDownloadedAndForwarded )
        return 1;

    // Consider abandoning this block if it is not forwarding, has received more than the
    // minimum percent and has more remaining than the alternative
    if ( !mForward
            && mFragmentInfo.mReceived * 100 / mFragmentInfo.mSize > mAbandonBlockControlPct
            && remainingBytes() > mAlternativeFragmentSize )
    {
        if ( remainingBytes() > mAlternativeFragmentSize )
        {
            double playbackRate;
            AseTimeStamp playbackTime;
            (void) mMediaSinkPtr->currentPlaybackInfo( playbackRate, playbackTime );

            if ( playbackRate != 0.0 )
            {

                ullong const timeToPlayback = ( mFragmentStartPts - playbackTime ).getValueInMs();

                // Predict the time at which our abandon trigger will be met
                // timeToDownload = ( time - mFirstDataArrivalTime ) * <total size> / <received size>
                //
                // Trigger an abandon when timeToDownload > timeToPlayback
                // => ( time - mFirstDataArrivalTime ) * <total size> / <received size> > timeToPlayback
                // => ( time - mFirstDataArrivalTime ) > timeToPlayback * <received size> / <totalsize>
                // => time >= timeToPlayback * <received size> / <totalsize> + mFirstDataArrivalTime
                //
                // Only set the abandon timer if there will be time to download an alternative fragment
                // otherwise, we might as well head into the rebuffer without discarding data

                abandontime = mFirstDataArrivalTime +
                                AseTimeVal::fromMS( std::max( timeToPlayback * mFragmentInfo.mReceived / mFragmentInfo.mSize,
                                                            getDuration().getValueInMs() ) );

                if ( (int32_t)( ( now.ms() + timeToPlayback ) - abandontime.ms() ) < mPredictDlTimeForBackup )
                {
                    abandontime = AseTimeVal::INFINITE;
                }

#if 0
                DEBUGLOG(MediaFragmentLog,
                    "provideResponseBody track: %d, stream %d, pts: %lldms, %d%%, play in %lldms, abandon in %lldms, backup %ums, %u bytes, alt %u bytes",
                        mTrackId,
                        mFragmentInfo.mStreamId,
                        mFragmentStartPts.getValueInMs(),
                        mFragmentInfo.mReceived * 100 / mFragmentInfo.mSize,
                        timeToPlayback,
                        ( abandontime - now ).ms(),
                        mPredictDlTimeForBackup,
                        remainingBytes(),
                        mAlternativeFragmentSize );
#endif
            }
            else
            {
                DEBUGLOG(MediaFragmentLog,
                        "provideResponseBody track: %d, stream %d, pts: %lldms, %u remaining, alt %u bytes",
                            mTrackId,
                            mFragmentInfo.mStreamId,
                            mFragmentStartPts.getValueInMs(),
                            remainingBytes(),
                            mAlternativeFragmentSize );
            }
        }
    }

    return 0;
}

void MediaFragment::provideFailureCode( AseErrorCode errCode )
{
    if ( errCode == AS_ABANDONED )
    {
        // We only report abandoned error once in the media fragment
        MediaRequestPtr const pMediaRequest = mMediaRequestPtr.lock();
        if ( pMediaRequest && !pMediaRequest->isAbandoned() )
        {
            DEBUGLOG(MediaFragmentLog, "provideFailureCode: abandoned fragment start pts: %llu ms",
                    mFragmentStartPts.getValueInMs());

            pMediaRequest->setAbandoned();

            MediaFragmentPtr pNextMediaFragment = mNextMediaFragmentPtr.lock();
            if ( pNextMediaFragment )
            {
                pNextMediaFragment->provideFailureCode( AS_ABANDONED );
            }

            mMediaStreamPtr.lock()->reportFailure(errCode, mFragmentId);
        }
    }
    else
    {
        mMediaStreamPtr.lock()->reportFailure( errCode, mFragmentId );
    }
}

/**
 * This function is driven by
 * 1. when new data comes;
 * 2. kick start from startForwarding()
 * 3. periodical check from startForwarding() for the out-of-buffer case
 */
void MediaFragment::deliverDataToSink()
{
    AseErrorCode retCode = AS_NO_ERROR;
    // attempt to deliver the last block of the previous gop, which previous attempted delivery has been failed
    if ( mLastBlockMetaInfo.get() )
    {
        retCode =
            constructAndDeliverLastBlockOfAGop(mLastBlockMetaInfo->mBlockSize, mLastBlockMetaInfo->mStartOfFragment, mLastBlockMetaInfo->mFinalBlock);
        if ( retCode == AS_NO_ERROR )
        {
            mLastBlockMetaInfo.reset();
        }
        else
        {
            // we can not deliver other blocks before delivering this block
            return;
        }

        if ( mDataSizeFromPreviousBlock > 0 )
        {
            retCode = deliverRemainingData(mFinalBlock);

            if ( retCode != AS_NO_ERROR )
                return;
        }
    }

    BufferObj bufferObj;
    while ( !mBufferedData.empty() )
    {
        bufferObj = mBufferedData.front();
        assert( bufferObj.mSequence >= (int32_t)mExpectedSequenceNum );
        if ( bufferObj.mSequence == (int32_t)mExpectedSequenceNum )
        {
            mFinalBlock = (mExpectedSequenceNum + 1) == mFinalSequenceNum;
            retCode =
                deliverOneBlock(bufferObj.mAseBufferPtr, mFinalBlock);
            ++mExpectedSequenceNum;
            mBufferedData.pop_front();
            if ( retCode != AS_NO_ERROR )
            {
                // we need to update the sequence number before we break in the error case, because
                // the only error condition is that the player does not have buffer for the last remaining
                // bytes of a gop. The original media buffer has been delivered to media sink.
                break;
            }
        }
        else
        {
            break;
        }
    }

    if ( mDownloadedAndForwarded )
    {
        completeForwarding();
    }
}


shared_ptr<MediaStream> MediaFragment::getMediaStream() const
{
    return mMediaStreamPtr.lock();
}

AseTimeStamp MediaFragment::getDuration() const
{
    return mFragmentInfo.mDuration;
}

int MediaFragment::getNominalBitrate() const
{
    return mMediaStreamPtr.lock()->getAverageBitrate();
}

void MediaFragment::bufferFull()
{
    mMediaPresentationPtr.lock()->bufferFull();
}

void MediaFragment::abandonImmediately()
{
    DEBUGLOG(MediaFragmentLog, "abandon media fragment with start pts: %llu ms", mFragmentStartPts.getValueInMs());
    MediaRequestPtr const pMediaRequest = mMediaRequestPtr.lock();
    if ( pMediaRequest )
        pMediaRequest->setAbandoned();

    return;
}

AseErrorCode MediaFragment::deliverOneBlock(AseBufferPtr pDataBlock, bool finalBlock)
{
    mHasStartedForwarding = true;
    bool startOfFragment = false;
    // roughly deliver the pts of each frame by interpolating the presentation time
    AseErrorCode retCode = AS_NO_ERROR;
    if ( mNumOfFragments == 1 )
    {
        if (!mExpectedSequenceNum)
        {
            startOfFragment = true;
            mCurrentTimestamp = mCurrentGopStartPts;
        }

        mMediaSinkPtr->deliver( pDataBlock,
                mManifestId,
                mTrackId, mFragmentInfo.mStreamId,
                startOfFragment, mCurrentTimestamp);
        ++mNumOfBlocksDelivered;
        mCurrentByteOffset += pDataBlock->getFilledSpaceSize();
    }
    else
    {
        startOfFragment = isStartOfFragment();
        size_t currentBlockSize = computeCurrentBlockSize(pDataBlock);

        if ( (mDataSizeFromPreviousBlock == 0) && (currentBlockSize == pDataBlock->getFilledSpaceSize()) )
        {
            if ( startOfFragment )
            {
                updateGopStartPts();
            }
            mMediaSinkPtr->deliver( pDataBlock,
                                    mManifestId,
                                    mTrackId, mFragmentInfo.mStreamId,
                                    startOfFragment, mCurrentTimestamp);
            mCurrentByteOffset += currentBlockSize;
            ++mNumOfBlocksDelivered;
        }
        else
        {
            // reconstruct the data block using the stored data from previous block
            // and partial current data block
            // A -- pDataBlock, B -- mDataFromPreviousBlock, C -- pTmpBlock
            unsigned char* pTmpBlock = new unsigned char[mBufferUnitSize];
            size_t totalBackupDataSize = pDataBlock->copyout(pTmpBlock, pDataBlock->getFilledSpaceSize()); // A --> C
            pDataBlock->eraseAll();
            assert(currentBlockSize > mDataSizeFromPreviousBlock);
            if ( mDataSizeFromPreviousBlock > 0)
                pDataBlock->copyin(mDataFromPreviousBlock, mDataSizeFromPreviousBlock); // B --> A[B|...]
            size_t remainingBlockSize = currentBlockSize - mDataSizeFromPreviousBlock;
            pDataBlock->copyin(pTmpBlock, remainingBlockSize); // C[1|2] --> A[B|1]
            mDataSizeFromPreviousBlock = totalBackupDataSize - remainingBlockSize;
            memset(mDataFromPreviousBlock, 0, mBufferUnitSize);
            if ( mDataSizeFromPreviousBlock > 0)
            {
                memcpy(mDataFromPreviousBlock, pTmpBlock + remainingBlockSize, mDataSizeFromPreviousBlock); // C[1|2] --> B[2]
            }
            if ( startOfFragment )
            {
                updateGopStartPts();
            }
            mMediaSinkPtr->deliver( pDataBlock,
                                    mManifestId,
                                    mTrackId, mFragmentInfo.mStreamId,
                                    startOfFragment, mCurrentTimestamp);
            ++mNumOfBlocksDelivered;

            mCurrentByteOffset += pDataBlock->getFilledSpaceSize();

            retCode = deliverRemainingData(finalBlock);

            delete[] pTmpBlock;
        }

    }

    if ( retCode == AS_NO_ERROR)
    {
        mCurrentTimestamp += mTimeSlicePerBlock;
        if (finalBlock && (mDataSizeFromPreviousBlock == 0) )
            mDownloadedAndForwarded = true;
    }

    return retCode;
}

size_t MediaFragment::computeCurrentBlockSize(bool finalBlock)
{
    size_t currentBlockSize = mBufferUnitSize;

    if ( finalBlock )
        currentBlockSize = mDataSizeFromPreviousBlock;

    currentBlockSize = updateBlockSizeForFragmentBoundary(currentBlockSize);

    return currentBlockSize;
}

size_t MediaFragment::computeCurrentBlockSize(AseBufferPtr pDataBlock)
{
    size_t currentBlockSize = mBufferUnitSize;

    currentBlockSize = mBufferUnitSize < (mDataSizeFromPreviousBlock + pDataBlock->getFilledSpaceSize()) ?
                                mBufferUnitSize : (mDataSizeFromPreviousBlock + pDataBlock->getFilledSpaceSize());
    currentBlockSize = updateBlockSizeForFragmentBoundary(currentBlockSize);

    return currentBlockSize;
}

size_t MediaFragment::updateBlockSizeForFragmentBoundary(size_t currentBlockSize)
{
    if ( !mStartOfFragmentsByteOffsets.empty() )
    {
        uint32_t bytesToNextFragment = 0;
        if ( mCurrentByteOffset == mStartOfFragmentsByteOffsets[0] )
        {
            if ( mStartOfFragmentsByteOffsets.size() > 1 )
            {
                bytesToNextFragment = mStartOfFragmentsByteOffsets[1] - mStartOfFragmentsByteOffsets[0];
            }
            else
            {
                bytesToNextFragment = mByteRange.getEnd() - mCurrentByteOffset;
            }
        }
        else
        {
            bytesToNextFragment = mStartOfFragmentsByteOffsets[0] - mCurrentByteOffset;
        }

        if ( bytesToNextFragment < currentBlockSize )
        {
            currentBlockSize = bytesToNextFragment;
        }
    }

    return currentBlockSize;
}

uint32_t MediaFragment::predictCompleteTime(uint32_t throughput)
{
    AseTimeVal now = AseTimeVal::now();
    if ( mAbandonBlockControlTime > now )
    {
        // has not spent enough time on this fragment yet
        return 0;
    }

    uint32_t completeTime = 0;
    if ( throughput > 0 )
    {
        completeTime = remainingBytes() * 8 / throughput;
    }

    return completeTime;
}

bool MediaFragment::hasStartedForwardingData()
{
    return mHasStartedForwarding;
}

void MediaFragment::debugOutput(AseBufferPtr pDataBlock, uint32_t startIndex)
{
    unsigned char* pTmpBlock = new unsigned char[mBufferUnitSize];
    size_t totalBackupDataSize = pDataBlock->copyout(pTmpBlock, pDataBlock->getFilledSpaceSize()); // A --> C
    DEBUGLOG(MediaFragmentLog, "start of fragment: byte offset: %llu @x%x:%x:%x:%x",
            mCurrentByteOffset, pTmpBlock[startIndex], pTmpBlock[startIndex + 1],
            pTmpBlock[startIndex + 2], pTmpBlock[startIndex + 3]);
    NRDP_UNUSED(startIndex);
    pDataBlock->eraseAll();
    pDataBlock->copyin(pTmpBlock, totalBackupDataSize);
    delete[] pTmpBlock;

    return;
}

uint32_t MediaFragment::computeNumberOfBlocks(uint32_t fragmentSize, uint32_t bufferUnitSize)
{
    uint32_t numOfBlocks = fragmentSize / bufferUnitSize;
    if ( numOfBlocks * bufferUnitSize < fragmentSize )
        ++ numOfBlocks;

    return numOfBlocks;
}

uint32_t MediaFragment::computeCurrentGopSize()
{
   if ( mStartOfFragmentsByteOffsets.size() > 1 )
   {
       return mStartOfFragmentsByteOffsets[1] - mStartOfFragmentsByteOffsets[0];
   }
   else
   {
       return mByteRange.getEnd() - mStartOfFragmentsByteOffsets[0];
   }
}

AseErrorCode MediaFragment::constructAndDeliverLastBlockOfAGop(uint32_t blockSize, bool startOfFragment, bool finalBlock)
{
    unsigned char* pTmpBlock = new unsigned char[mBufferUnitSize];
    AseBufferPtr pMediaBuffer = AseBufferPtr();

    AseErrorCode retCode = AS_NO_ERROR;

    retCode = mMediaBufferPoolPtr->allocate(mMediaType, mCdnId, pMediaBuffer);

    if ( pMediaBuffer )
    {
        pMediaBuffer->copyin(mDataFromPreviousBlock, blockSize);
        size_t remainingBytes = mDataSizeFromPreviousBlock - blockSize;
        if ( remainingBytes > 0 )
        {
            memset(pTmpBlock, 0, mBufferUnitSize);
            memcpy(pTmpBlock, mDataFromPreviousBlock + blockSize, remainingBytes);
            memset(mDataFromPreviousBlock, 0, mBufferUnitSize);
            memcpy(mDataFromPreviousBlock, pTmpBlock, remainingBytes);
        }
        else
        {
            memset(mDataFromPreviousBlock, 0, mBufferUnitSize);
        }
        mDataSizeFromPreviousBlock = remainingBytes;
        if ( startOfFragment )
        {
            updateGopStartPts();
        }
        mMediaSinkPtr->deliver( pMediaBuffer,
                                mManifestId,
                                mTrackId, mFragmentInfo.mStreamId,
                                startOfFragment, mCurrentTimestamp);
        mCurrentByteOffset += pMediaBuffer->getFilledSpaceSize();
        if ( finalBlock && (mDataSizeFromPreviousBlock == 0) )
            mDownloadedAndForwarded = true;
    }

    delete[] pTmpBlock;

    return retCode;
}

void MediaFragment::completeForwarding()
{

    mMediaStreamPtr.lock()->completeForwarding(mFragmentStartPts,
                                               mFragmentInfo.mDuration,
                                               mFinalSequenceNum, mLastFragment);
    NTRACE(TRACE_STREAMINGSTAT,
             "Complete forwarding fragment for track: %d, stream: %d with start pts: %lld ms"
             "received: %u size: %u bytes, byte range: %lld-%lld",
             mTrackId, mFragmentInfo.mStreamId,
             mFragmentStartPts.getValueInMs(),
             mFragmentInfo.mReceived, mFragmentInfo.mSize,
             mByteRange.getStart(), mByteRange.getEnd());

    PERFTRACE(
             "Complete forwarding fragment for track: %d, stream: %d with start pts: %lld ms time: %llu\n",
             mTrackId, mFragmentInfo.mStreamId,
             mFragmentStartPts.getValueInMs(),
             Time::mono().val());

    assert( (llong)mCurrentByteOffset == mByteRange.getEnd() + 1 );

    if (mNextMediaFragmentPtr.lock())
    {
        mNextMediaFragmentPtr.lock()->startForwarding( );
    }

    assert(mNumOfBlocksReceived == mNumOfBlocksDelivered);
    return;
}

bool MediaFragment::isStartOfFragment()
{
    bool startOfFragment = false;
    ullong nextFragmentStartByteOffset = 0;

    if ( !mStartOfFragmentsByteOffsets.empty() )
    {
        nextFragmentStartByteOffset = mStartOfFragmentsByteOffsets.front();
        if ( mCurrentByteOffset == nextFragmentStartByteOffset )
        {
            startOfFragment = true;
        }
    }

    return startOfFragment;
}

void MediaFragment::updateGopStartPts()
{
    if ( mCurrentTimestamp != AseTimeStamp::INFINITE )
    {
        mCurrentGopStartPts += mGopDuration;
        uint32_t currentGopSize = computeCurrentGopSize();
        uint32_t numOfBlocks = computeNumberOfBlocks( currentGopSize, mBufferUnitSize);
        mTimeSlicePerBlock = AseTimeStamp(mGopDuration.getValueInMs() * 1000 / numOfBlocks, 1000 * 1000);

    }
    mCurrentTimestamp = mCurrentGopStartPts;
    mStartOfFragmentsByteOffsets.pop_front();
    return;
}

AseErrorCode MediaFragment::deliverRemainingData(bool finalBlock)
{
    bool startOfFragment;
    uint32_t currentBlockSize;
    AseErrorCode retCode = AS_NO_ERROR;
    // check to see whether the remaining data includes the last bytes of the current fragment
    while ( true )
    {
        startOfFragment = isStartOfFragment();
        currentBlockSize = computeCurrentBlockSize(finalBlock);

        if ( (mDataSizeFromPreviousBlock > 0) && (currentBlockSize <= mDataSizeFromPreviousBlock) && (currentBlockSize > 0) )
        {
            retCode = constructAndDeliverLastBlockOfAGop(currentBlockSize, startOfFragment, finalBlock);

            if ( retCode != AS_NO_ERROR )
            {
                mLastBlockMetaInfo.reset(new BlockMetaInfo(currentBlockSize, startOfFragment, finalBlock));
                break;
            }
        }
        else
        {
            break;
        }
    }

    return retCode;

}

bool MediaFragment::isAbandoned()
{
    MediaRequestPtr const pMediaRequest = mMediaRequestPtr.lock();
    return pMediaRequest && pMediaRequest->isAbandoned();
}
