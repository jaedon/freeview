/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "MediaSourceStreamParser.h"
#include <nrdbase/Log.h>
#include <nrddemux/Base.h>
#include <nrddemux/Mp4Parser.h>

/*
 * To figure out box size, minimally, we need 12 bytes.
 * size 4 bytes
 * type 4 bytes
 * version/flag 4 bytes
 *
 * uuid does not need to be parsed for extended type, and we know we are not
 * using large size box.
 */
#define BOX_BOUNDARY_BUFFER_SIZE 12
#define BOX_SIZE_CHECK_MARGIN 32

using namespace std;
using namespace netflix;
using namespace netflix::streamparser;
using namespace ase;
using namespace netflix::containerlib::mp4parser;

MediaSourceStreamParser::MediaSourceStreamParser( ase::MediaType mediaType )
  : mMediaType( mediaType )
  , mFragmentOffset (0)
  , mCurrentBlockSize (0)
  , mRemainingBoxSize (0)
  , mParseState( STATE_BOX_START )
  , mPreviousParseState ( STATE_BOX_START )
  , mEnableBoxSizeCheck ( false )
  , mExpectedMaxBoxSize (INT_MAX)
  , mFragmentOffsetForBoxBoundaryCondition(0)
{
#ifndef NDEBUG
    mThread = 0;
#endif
    mAseBufferPtrForBoxBoundaryCondition.reset();
}

void MediaSourceStreamParser::reset()
{
#ifndef NDEBUG
    checkThread();
#endif
    NTRACE(TRACE_MEDIAPLAYBACK
           , "MediaSourceStreamParser(%u)::reset: was mFragment Offset %d, mCurrentBlockSize %d, mRemainingBoxSize %d, mParseState %d"
           , mMediaType
           , mFragmentOffset
           , mCurrentBlockSize
           , mRemainingBoxSize
           , mParseState);

    mFragmentOffset = 0;
    mCurrentBlockSize = 0;
    mRemainingBoxSize = 0;
    mParseState = STATE_BOX_START;
    mPreviousParseState = STATE_BOX_START;
    mEnableBoxSizeCheck = false;
    mExpectedMaxBoxSize = INT_MAX; // int32 max is big enough for initial value
    mMovieHeaderDataPtr.reset();
    mFragmentHeaderDataPtr.reset();
    mMediaFragmentPtr.reset();
    mFragmentHeaderPtr.reset();
    mBoxInfoData.clear();
    mVideoContext.reset();
    mAudioContext.reset();
    mEditLists.clear();
    mTrackHeaderPtrs.clear();
    mOutputRecordQueue = std::queue<OutputRecord>();
    mFragmentOffsetForBoxBoundaryCondition = 0;
    mAseBufferPtrForBoxBoundaryCondition.reset();
}

void MediaSourceStreamParser::datain( ase::AseBufferPtr pData,
                                      int32_t currentTimestampOffset,
                                      uint32_t currentTimestampOffsetTimescale)
{
#ifndef NDEBUG
    checkThread();
#endif

#if defined(STREAMPARSER_VERBOSE_LOG)
    NTRACE(TRACE_MEDIAPLAYBACK, "MediaSourceStreamParser(%u)::%s dataSize %d"
           , mMediaType, __func__, pData->getFilledSpaceSize());
#endif

    mDataPtr = pData;
    bool continueParsing = true;
    int32_t currentBufferOffset = 0;

    // mFragmentOffset is meaningful only after moof box is parsed
    mFragmentOffset = mFragmentOffset + mCurrentBlockSize;
    mCurrentBlockSize = pData->getFilledSpaceSize();

    while(continueParsing){
        if ( mRemainingBoxSize &&
             ((mDataPtr->getFilledSpaceSize() - currentBufferOffset) >= mRemainingBoxSize ) ) {
            /*
             * This is when current box data is ready, and to be parsed
             */
            switch(mParseState)
            {
            case STATE_MOOV_BOX:
            {
                NTRACE(TRACE_MEDIAPLAYBACK, "MediaSourceStreamParser(%u)::%s data ready for STATE_MOOV_BOX"
                       , mMediaType, __func__ );

                // for STATE_MOOV_BOX, prepare byte stream for MediaHeader parsing
                mMovieHeaderDataPtr->insert(
                    mMovieHeaderDataPtr->end(),
                    pData->getFilledSpaceStart() + currentBufferOffset,
                    pData->getFilledSpaceStart() + currentBufferOffset + mRemainingBoxSize);

                // enough data to parse. construct ByteStream, and parse it to create mMediaHeaderPtr
                parseMoovBox();

                // construct output using parsed mMediaHeaderPtr
                std::map<uint32_t, shared_ptr<TrackHeader> >::iterator it = mTrackHeaderPtrs.begin();
                for( ; it != mTrackHeaderPtrs.end(); it++){
                    OutputRecord outputRecord;
                    outputRecord.mParseResult = MEDIAHEADER_AVAILABLE;
                    outputRecord.mTrackHeaderPtr = it->second;
                    mOutputRecordQueue.push(outputRecord);
                }
                break;
            }
            case STATE_MOOF_BOX:
            {
                NTRACE(TRACE_MEDIAPLAYBACK, "MediaSourceStreamParser(%u)::%s data ready for STATE_MOOF_BOX"
                       , mMediaType, __func__);

                mFragmentHeaderDataPtr->insert(
                    mFragmentHeaderDataPtr->end(),
                    pData->getFilledSpaceStart() + currentBufferOffset,
                    pData->getFilledSpaceStart() + currentBufferOffset + mRemainingBoxSize);

                // enough data to parse. construct ByteStream, and parse it to create mFragmentHeaderPtr
                if( !parseMoofBox(currentTimestampOffset, currentTimestampOffsetTimescale) )
                {
                    mParseState = STATE_PARSE_ERROR;
                    return;
                }

                // even if actual data of MDAT has not arrived, we output the
                // constructed information of samples here, and expect the
                // device thread to check if data is really arrived or not.
                // device thread need to be very careful when getting the sample.
                // 1. It needs to check if mFragmentHeaderPtr is valid.
                // 2. It needs to check if data for sample are all arrived.
                mMediaFragmentPtr->mTrackHeaderPtr = mTrackHeaderPtrs[mFragmentHeaderPtr->mTrackId];
                mMediaFragmentPtr->mFragmentHeaderPtr = mFragmentHeaderPtr;
                OutputRecord outputRecord;
                outputRecord.mParseResult = MEDIAFRAGMENT_AVAILABLE;
                outputRecord.mMediaFragmentPtr = mMediaFragmentPtr;

                mExpectedMaxBoxSize = mFragmentHeaderPtr->mExpectedMaxBoxSize;

                NTRACE(TRACE_MEDIAPLAYBACK, "MediaSourceStreamParser(%u) expected max box size %lld",
                       mMediaType, mExpectedMaxBoxSize);

                mFragmentHeaderPtr->mTotalSampleSize = 0;

                mOutputRecordQueue.push(outputRecord);
                break;
            }
            case STATE_MDAT_BOX:
            {
                NTRACE(TRACE_MEDIAPLAYBACK, "MediaSourceStreamParser(%u)::%s data ready for STATE_MDAT_BOX"
                       , mMediaType, __func__ );
                // enough data for whole mdat.
                mMediaFragmentPtr->push(mFragmentOffset, pData);
                mMediaFragmentPtr.reset(); // once pushed into outputRecord, reset for new fragment
                break;
            }
            default:
                break;
            }

            // current box parsing is done. initiate new box parsing
            currentBufferOffset += mRemainingBoxSize;
            mRemainingBoxSize = 0;
            mPreviousParseState = mParseState;
            mParseState = STATE_BOX_START;

        } else if ( mRemainingBoxSize ) {
            /*
             * This is when current box need more data, and no parsing is available
             */
            //printf("mRemainingBoxSize %d\n", mRemainingBoxSize);

            switch(mParseState)
            {
            case STATE_MDAT_BOX:
                mMediaFragmentPtr->push( mFragmentOffset, pData );
                break;

            case STATE_MOOF_BOX:
                mFragmentHeaderDataPtr->insert(mFragmentHeaderDataPtr->end(),
                                               pData->getFilledSpaceStart() + currentBufferOffset,
                                               pData->getFilledSpaceStart() + pData->getFilledSpaceSize());
                break;

            case STATE_MOOV_BOX:
                mMovieHeaderDataPtr->insert(mMovieHeaderDataPtr->end(),
                                            pData->getFilledSpaceStart() + currentBufferOffset,
                                            pData->getFilledSpaceStart() + pData->getFilledSpaceSize() );
                break;

            default:
                break;
            }

            // no state change. Just change offset and return to receive more data
            // since we need to take more data, return to get more datain
            mRemainingBoxSize -= (mDataPtr->getFilledSpaceSize() - currentBufferOffset) ;
            currentBufferOffset = 0;
            return;
        } else if ( (mRemainingBoxSize == 0) && (mParseState != STATE_BOX_START)) {
            // This should not happen, but can happen if data coming into is wrong
            NTRACE(TRACE_MEDIAPLAYBACK
                   , "MediaSourceStreamParser(%u)::%s data coming into parser is not correct parserState %d"
                   , mMediaType, __func__, mParseState);

            mParseState = STATE_PARSE_ERROR;
            return;
        }

        if (mDataPtr->getFilledSpaceSize() == static_cast<uint32_t>(currentBufferOffset)){
            return;
        }

        // start of new box either from 0 offset of pData, or middle of pData
        if (mParseState == STATE_BOX_START) {

            // start of new box
            ullong size = 0;
            uint32_t boxType = 0;
            Mp4Parser mp4Parser;

            if( (mDataPtr->getFilledSpaceSize() - currentBufferOffset) < BOX_BOUNDARY_BUFFER_SIZE){
                // to figure out box type and size, at least we need BOX_BOUNDARY_BUFFER_SIZE bytes
                NTRACE(TRACE_MEDIAPLAYBACK
                       , "MediaSourceStreamParser(%u)::%s BOX_BOUNDARY_CONDITION need more(%zu) data for box info parsing"
                       , mMediaType, __func__, (mDataPtr->getFilledSpaceSize() - currentBufferOffset) );
                mBoxInfoData.insert(mBoxInfoData.end(),
                                    pData->getFilledSpaceStart() + currentBufferOffset,
                                    pData->getFilledSpaceStart() + pData->getFilledSpaceSize());

                if(mPreviousParseState == STATE_MOOF_BOX){
                    NTRACE(TRACE_MEDIAPLAYBACK, "BOX_BOUNDARY_CONDITION:  need more data for MDAT parsing");
                }

                // when we need more data for box parsing, we don't know what box this is about. In case of MDAT, we
                // will need to put buffer information into MediaFragment. For that, we remember the information here
                // for MDAT
                mFragmentOffsetForBoxBoundaryCondition = mFragmentOffset;
                mAseBufferPtrForBoxBoundaryCondition = pData;
                return;
            }

            int bufferedBoxInfoDataSize = mBoxInfoData.size();
            int remainingBoxInfoDataSize = BOX_BOUNDARY_BUFFER_SIZE - bufferedBoxInfoDataSize;
            if( remainingBoxInfoDataSize != BOX_BOUNDARY_BUFFER_SIZE ){
                if ( ! ((currentBufferOffset == 0) && (remainingBoxInfoDataSize > 0)) ){
                    Log::error(TRACE_MEDIAPLAYBACK,
                               " Assert at %s :  currentBufferOffset %d, remainingBoxInfoDataSize %d",
                               __func__, currentBufferOffset, remainingBoxInfoDataSize);
                    assert( (currentBufferOffset == 0) && (remainingBoxInfoDataSize > 0));
                }
                mBoxInfoData.insert(mBoxInfoData.end(),
                                    pData->getFilledSpaceStart() + currentBufferOffset,
                                    pData->getFilledSpaceStart() + remainingBoxInfoDataSize);

                mp4Parser.getNextBoxInfo(&mBoxInfoData[0],
                                         mBoxInfoData.size(),
                                         size, /* out */
                                         boxType /* out */);

                // if expecting mdat, but if boxType was not mdat, pop from mMEdiaFragmentPtr
                if( boxType == Box::buildFourCC("mdat") ) {
                    NTRACE(TRACE_MEDIAPLAYBACK, "BOX_BOUNDARY_CONDITION: needed more data for MDAT parsing, and MDAT actually arrived");
                    mMediaFragmentPtr->push(mFragmentOffsetForBoxBoundaryCondition, mAseBufferPtrForBoxBoundaryCondition);
                } else {
                    NTRACE(TRACE_MEDIAPLAYBACK, "BOX_BOUNDARY_CONDITION: needed more data for BOX parsing, but it was not MDAT");
                }
                // below information on fragmentOffset and AseBufferPtr is necessary only for MDAT. Reset for other case.
                mFragmentOffsetForBoxBoundaryCondition = 0;
                mAseBufferPtrForBoxBoundaryCondition.reset();
            } else {
                mp4Parser.getNextBoxInfo(mDataPtr->getFilledSpaceStart() + currentBufferOffset,
                                         mDataPtr->getFilledSpaceSize() - currentBufferOffset,
                                         size, /* out */
                                         boxType /* out */);
            }

            mParseState = ( (boxType == Box::buildFourCC("moov")) ? STATE_MOOV_BOX :
                            (boxType == Box::buildFourCC("moof")) ? STATE_MOOF_BOX :
                            (boxType == Box::buildFourCC("mdat")) ? STATE_MDAT_BOX :
                            STATE_OTHER_BOX );

            NTRACE(TRACE_MEDIAPLAYBACK,"[%u] MediaSourceStreamParser -------------------------\n", mMediaType);
            if( mParseState == STATE_MDAT_BOX){
                NTRACE(TRACE_MEDIAPLAYBACK,
                       "[%u] MediaSourceStreamParser: %c%c%c%c :size %lld, expected max boxsize %lld, diff(%d)"
                       , mMediaType
                       , static_cast<char>(boxType>>24)
                       , static_cast<char>((boxType >> 16) & 0xFF)
                       , static_cast<char>((boxType >> 8) & 0xFF)
                       , static_cast<char>(boxType & 0xFF)
                       , size
                       , mExpectedMaxBoxSize
                       , static_cast<int> (size - mExpectedMaxBoxSize)
                    );
            } else {
                NTRACE(TRACE_MEDIAPLAYBACK,
                       "[%u] MediaSourceStreamParser: %c%c%c%c :size %lld"
                       , mMediaType
                       , static_cast<char>(boxType>>24)
                       , static_cast<char>((boxType >> 16) & 0xFF)
                       , static_cast<char>((boxType >> 8) & 0xFF)
                       , static_cast<char>(boxType & 0xFF)
                       , size);
#ifndef NDEBUG
                if(size > 1024*1024){
                    Log::error(TRACE_MEDIAPLAYBACK, "[%u] size %lld too big for non mdat", mMediaType, size);
                    assert(0);
                }
#endif
            }

            mRemainingBoxSize = size;

            // Box size checking :
            //
            // expected maximum mdat box size is, distance from start of moof to
            // end of mdat. since this is bigger than any box sizes between
            // moof and mdat, any boxes inbetween will be checked against big
            // enough value.
            //
            // For the other boxes that follows mdat, assumption is that, it is
            // not bigger than above expected max mdat size.
            //
            // expected max mdat size consider multiple trun cases. when when
            // there are multiple truns, max mdat size is calculated furthest
            // mdat from moof.
            //

            if ( mPreviousParseState == STATE_MOOF_BOX )
            {
                // we only want to check box sizes after we get moof.
                mEnableBoxSizeCheck = true;
            }

            if ( (mParseState == STATE_MOOF_BOX) || (mParseState == STATE_MOOV_BOX) )
            {
                // we only want to check box sizes after we get moof.
                mEnableBoxSizeCheck = false;
            }

            // margin may not be necessary since expected max box size already is big enough
            if (mEnableBoxSizeCheck && ( size > mExpectedMaxBoxSize + BOX_SIZE_CHECK_MARGIN ) )
            {
                Log::error(TRACE_MEDIAPLAYBACK,
                           "[%u] MediaSourceStreamParser: current box size %lld, expected max boxSize %lld",
                           mMediaType, size, mExpectedMaxBoxSize);

                if(mBoxInfoData.size())
                {
                    Log::error(TRACE_MEDIAPLAYBACK, "[%u] MediaSourceStreamParser: mBoxInfoData.size() %zu",
                               mMediaType, mBoxInfoData.size());
                    for(unsigned int i=0; i<mBoxInfoData.size(); i++)
                    {
                        Log::error(TRACE_MEDIAPLAYBACK, "[%u] MediaSourceStreamParser: mBoxInfoData 0x%x",
                                   mMediaType, mBoxInfoData[i]);
                    }
                }
#ifndef NDEBUG
                FILE* fp_ = fopen("incorrect.mdat", "wb");
                fwrite(static_cast<const void*>(mDataPtr->getFilledSpaceStart() + currentBufferOffset),
                       sizeof(char),
                       mDataPtr->getFilledSpaceSize() - currentBufferOffset,
                       fp_);
                fclose(fp_);
#endif
                mParseState = STATE_PARSE_ERROR;
                assert(0);
                return;
            }

            if(mBoxInfoData.size() == BOX_BOUNDARY_BUFFER_SIZE){
                mRemainingBoxSize -=  bufferedBoxInfoDataSize;
            }

            switch(mParseState){
            case STATE_MOOV_BOX:
                mMovieHeaderDataPtr.reset(new MovieHeaderData);
                if(mBoxInfoData.size() == BOX_BOUNDARY_BUFFER_SIZE){
                    mMovieHeaderDataPtr->insert(mMovieHeaderDataPtr->end()
                                                , &mBoxInfoData[0]
                                                , &mBoxInfoData[0] + bufferedBoxInfoDataSize);
                }
                break;

            case STATE_MOOF_BOX:
                mFragmentHeaderDataPtr.reset(new FragmentHeaderData);
                if(mBoxInfoData.size() == BOX_BOUNDARY_BUFFER_SIZE){
                    mFragmentHeaderDataPtr->insert(mFragmentHeaderDataPtr->end()
                                                   ,&mBoxInfoData[0]
                                                   ,&mBoxInfoData[0] + bufferedBoxInfoDataSize);
                    mFragmentOffset = bufferedBoxInfoDataSize;
                } else {
                    mFragmentOffset = - currentBufferOffset;
                }

                // mark that current offset is where fragment mdat
                // starts. Sample offset is based on this point. This offset is
                // offset at first AseBuffer block. First sample offset will be
                // mMDATOffset + MDAT header size from first AseBuffer in MediaFragment
                mMediaFragmentPtr.reset(new MediaFragment);
                break;

            case STATE_MDAT_BOX:
                break;

            default:
                break;
            }

            if(mBoxInfoData.size() == BOX_BOUNDARY_BUFFER_SIZE){
                mBoxInfoData.clear();
            }
        }
    }
}

MediaSourceStreamParser::ParseResult MediaSourceStreamParser::dataout( shared_ptr<TrackHeader>& pTrackHeader,
                                                                       shared_ptr<MediaFragment>& pMediaFragment)
{
#ifndef NDEBUG
    checkThread();
#endif

    if ((mParseState == STATE_PARSE_ERROR) || (mParseState == STATE_OUT_OF_MEMORY)) {
        /*
        NTRACE(TRACE_MEDIAPLAYBACK, "MediaSourceStreamParser(%u)::%s return NOTHING_AVAILABLE with state %d", mMediaType, __func__, mParseState );
        */
        return STREAM_PARSING_ERROR;
    }

    if ( mOutputRecordQueue.empty() ) {
        /*
        NTRACE(TRACE_MEDIAPLAYBACK, "MediaSourceStreamParser(%u)::%s return NOTHING_AVAILABLE with empty record", mMediaType, __func__ );
        */
        return NOTHING_AVAILABLE;
    }

    OutputRecord const& out = mOutputRecordQueue.front();
    const ParseResult result = out.mParseResult;
    pTrackHeader = out.mTrackHeaderPtr;
    pMediaFragment = out.mMediaFragmentPtr;
    mOutputRecordQueue.pop();
    //NTRACE(TRACE_MEDIAPLAYBACK, "MediaSourceStreamParser(%u)::%s returning %d", mMediaType, __func__, result );
    return result;
}

/**
 * moov parsing
 */
bool MediaSourceStreamParser::parseMoovBox()
{
#ifndef NDEBUG
    checkThread();
#endif
    Mp4Parser mp4Parser;
    if(mMediaType == MEDIA_AUDIO){
        mAudioContext.reset(new AudioParserContext(mTrackHeaderPtrs, mEditLists));
        mp4Parser.parseMovieBox(*mAudioContext, &(*mMovieHeaderDataPtr)[0], mMovieHeaderDataPtr->size());
    } else if(mMediaType == MEDIA_VIDEO) {
        mVideoContext.reset(new VideoParserContext(mTrackHeaderPtrs, mEditLists));
        mp4Parser.parseMovieBox(*mVideoContext, &(*mMovieHeaderDataPtr)[0], mMovieHeaderDataPtr->size());
    }

#if defined(STREAMPARSER_VERBOSE_LOG)
    std::map<uint32_t, shared_ptr<TrackHeader> >::iterator it = mTrackHeaderPtrs.begin();
    for(;it != mTrackHeaderPtrs.end(); it++){
        NTRACE(TRACE_MEDIAPLAYBACK,"timescale after moov parsing %d\n", it->second->mTimeScale);
    }
#endif

    return true;
}

/**
 * moof parsing. moof box can not be completely parsed without moov box. Netflix
 * use tenc box to describe drm information, and there is no drm information in
 * senc. In order to provide drm information to construct drm map for each
 * sample, moov information is provided into this function
 */

bool MediaSourceStreamParser::parseMoofBox(int32_t currentTimestampOffset,
                                           uint32_t currentTimestampOffsetTimescale)
{
#ifndef NDEBUG
    checkThread();
#endif
    Mp4Parser mp4Parser;
    mFragmentHeaderPtr.reset(new FragmentHeader(currentTimestampOffset, currentTimestampOffsetTimescale) );

    if( (mMediaType == MEDIA_AUDIO) && mAudioContext.get() )
    {
        mAudioContext->setFragmentHeader(mFragmentHeaderPtr);
        mp4Parser.parseMovieFragment(*mAudioContext,
                                     &(*mFragmentHeaderDataPtr)[0],
                                     mFragmentHeaderDataPtr->size());
    }
    else if( (mMediaType == MEDIA_VIDEO) && mVideoContext.get() )
    {
        mVideoContext->setFragmentHeader(mFragmentHeaderPtr);
        mp4Parser.parseMovieFragment(*mVideoContext,
                                     &(*mFragmentHeaderDataPtr)[0],
                                     mFragmentHeaderDataPtr->size());
    } else {
        Log::error(TRACE_MEDIAPLAYBACK, "[%u] ParseContext is null", mMediaType);
        return false;
    }
    return true;
}
