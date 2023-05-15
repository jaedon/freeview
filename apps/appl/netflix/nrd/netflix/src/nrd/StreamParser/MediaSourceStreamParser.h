/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef NRDP_MEDIA_SOURCE_STREAMPARSER_H
#define NRDP_MEDIA_SOURCE_STREAMPARSER_H

#include <nrdase/AseCommonDataTypes.h>
#include <nrdase/AseBuffer.h>
#include <nrdase/EditList.h>
#include <nrdbase/Thread.h>
#include <nrdbase/Log.h>
#include <nrddemux/Context.h>
#include <nrddemux/Mp4Parser.h>
#include <nrddemux/Reader.h>

#include <stdio.h>
#include <queue>
#include <map>
#include <climits>

#define _FOURCC_LE_(a,b,c,d)  (a | (b << 8) | (c << 16) | (d << 24))
#define _FOURCC_BE_(a,b,c,d)  (d | (c << 8) | (b << 16) | (a << 24))

namespace netflix {
namespace streamparser {

//#define STREAMPARSER_VERBOSE_LOG 1

/*
 * Byte stream input for the parser
 */
typedef std::vector<unsigned char> TrackHeaderData;
typedef std::vector<unsigned char> FragmentHeaderData;

/*
 * Term definition
 * class XXXHeader holds only information extracted from stream itself.
 * class XXXAttributes holds information both from stream itself and manifest
 */

struct TemporalLayerEntryInfo
{
    uint32_t tlTemporalLayerId_;
    uint32_t tlLevelIdc_;
    uint32_t tlFrameRateValue_;
    uint32_t tlFrameRateScale_;
};

/*
 * TrackHeader encapsulate data parsed from trak box. A stream can have multiple
 * video tracks or audio tracks.
 */
class TrackHeader
{
public:
    uint32_t mStreamID;
    uint32_t mFormatID; /** FourCC */
    std::string mCodecParam;
    uint32_t mTrackId;
    bool     mEncrypted;
    uint64_t mDuration;
    uint32_t mTimeScale;
    uint32_t mDefaultDrmAlgorithmID;
    uint32_t mDefaultDrmIVSize;
    std::vector<uint8_t> mDefaultDrmKID;
};

class VideoTrackHeader : public TrackHeader
{
public:
    uint32_t            mImageWidth;
    uint32_t            mImageHeight;
    uint32_t            mBitsDepthLuma;
    uint32_t            mBitsDepthChroma;
    uint32_t            mPixelAspectRatioX;
    uint32_t            mPixelAspectRatioY;
    uint32_t            mFrameRateValue;
    uint32_t            mFrameRateScale;
    int32_t             mColorPrimaries;
    int32_t             mTransferCharacteristics;
    int32_t             mMatrixCoefficients;
    int32_t             mFullRange;
    uint32_t            mGeneralLevelIdc;
    uint32_t            mNominalBitrateInKbps;

    std::vector<unsigned char> mSequenceParameterSet;
    std::vector<unsigned char> mPictureParameterSet;

    /*
     * HEVC specific
     */
    std::vector<unsigned char> mVideoParameterSet;
    bool                       mTemporalLayerStream;
    std::vector<struct TemporalLayerEntryInfo> mTemporalLayerEntryInfo;

    /*
     * MVC specific
     */
    // The sub-sequence paramater set and picture parameter set for the
    // dependent view are place here.
    std::vector<unsigned char> mSubsequenceParameterSet;
    std::vector<unsigned char> mDependentViewPictureParameterSet;

    // These maps are the ScalableNALUMapEntry tables read from the moov header
    // (see ITU-T R. 14496-15:2010). They are used to split the NALUs in MVC
    // access units into base and dependent views when an MVC decoder
    // implementation needs to access views separately (see DUAL_STREAM_MVC in
    // the MP4Demultiplexer).
    //
    // Each access unit has a sampleGroup id read from the sample to group box
    // in the track fragment. That id indexes into naluToMvcViewMaps_ to specify
    // a vector with a size equal to the number of NALUs in the access unit.
    // The entries in the vector specify the views that successive NALUs in the
    // access unit belong to.
    std::vector<std::vector<uint8_t> > mNaluToMvcViewMaps;
};

class AudioTrackHeader : public TrackHeader
{
public:
    // information to be acquired audio moov box
    uint32_t mNumberOfChannels;
    uint32_t mSamplesPerSecond;
    uint32_t mBytesPerSecond;
    uint32_t mNominalBitrateInKbps;
    uint32_t mBlockAlignment;
    uint32_t mBitsPerSample;

    std::vector<unsigned char> mCodecSpecificData;
    std::vector<unsigned char> mADTSHeader; // For AAC
};

/*
 * Sample
 */
class Sample
{
public:
    bool mIsReferenced;
    uint32_t mFragmentOffset; // offset from start of fragment(moof)
    uint32_t mSize;
    int64_t mPts; // in milisecond
    int64_t mDts; // in milisecond
    uint32_t mSampleGroup; // for mvc mapping
    uint32_t mTemporalLayerId;
    std::vector<uint8_t>   mIV;
    std::vector<uint32_t>  mSubSampleMap;
};

/*
 * FragmentHeader
 */
class FragmentHeader
{
public:
    FragmentHeader(int32_t currentTimestampOffset, uint32_t currentTimestampOffsetTimescale)
      :mTotalSampleSize(0)
      ,mExpectedMaxBoxSize(INT_MAX)
      ,mDiscontinuityGap(ase::AseTimeStamp())
      ,mCurrentTimestampOffset(currentTimestampOffset)
      ,mCurrentTimestampOffsetTimescale(currentTimestampOffsetTimescale)
    {
    }

    ~FragmentHeader()
    {
    };

    uint32_t getTrackId()
    {
        return mTrackId;
    };

    void setDiscontinuityGapInMs(ase::AseTimeStamp gap)
    {
        mDiscontinuityGap = gap;
    }

public:
    uint32_t mTrackId;
    uint32_t mTimescale;
    uint64_t mBaseMediaDecodeTime;
    uint32_t mSampleDuration;
    uint64_t mTotalSampleSize; // sum of all sample size in the fragment
    uint64_t mExpectedMaxBoxSize;

    std::vector<Sample> mSamples;           // list of information for samples
    ase::AseTimeStamp   mDiscontinuityGap;  // discontinuity gap between previous fragment and current fragment
    ase::AseTimeStamp   mStartPts;          // start pts in ms for fragment
    ase::AseTimeStamp   mEndPts;            // end pts in ms for fragment

    int32_t mCurrentTimestampOffset;
    uint32_t mCurrentTimestampOffsetTimescale;
};

/*
 * MediaFragment
 */
class MediaFragment
{
public:
    shared_ptr<TrackHeader>      mTrackHeaderPtr;    // common info across the fragments in track
    shared_ptr<FragmentHeader>   mFragmentHeaderPtr; // common info across the samples in fragment

public:
    MediaFragment()
        : mSizeInBuffer( 0 )
    {
    }

    ~MediaFragment()
    {
        //printf("destroying MediaFragment %lld\n", getFragmentStartTime().getValueInMs());
    }

    std::vector< std::pair<int32_t, ase::AseBufferPtr> > const& data() const
    {
        return mDataBlocks;
    }

    void push( int32_t offset, ase::AseBufferPtr pBuffer )
    {
        mDataBlocks.push_back( std::make_pair( offset, pBuffer ) );
        mSizeInBuffer += pBuffer->getCapacity();
    }

    inline ase::AseTimeStamp getFragmentStartTime() const
    {
        return mFragmentHeaderPtr->mStartPts;
    }
    inline ase::AseTimeStamp getFragmentEndTime() const
    {
        return mFragmentHeaderPtr->mEndPts;
    }
    inline uint32_t getFragmentSizeInBuffer() const
    {
        return mSizeInBuffer;
    }

    inline bool hasSample( int index ) const
    {
        if ( mDataBlocks.empty() || index<0  || (((unsigned)index) >= mFragmentHeaderPtr->mSamples.size()) )
        {
            NTRACE(TRACE_MEDIAPLAYBACK,
                   "MediaSourceStreamParser::hasSample  mDataBlocks.empty() %s, index %d, sampleSize %zu",
                   (mDataBlocks.empty()?"true":"false"), index, mFragmentHeaderPtr->mSamples.size());
            return false;
        }

        const Sample &sample = mFragmentHeaderPtr->mSamples[ index ];
        const int32_t nextSampleOffset = sample.mFragmentOffset + sample.mSize;

        // if we already have much more sample data than requested
        if(mDataBlocks.back().first >= nextSampleOffset)
            return true;

        if( (mDataBlocks.back().first < nextSampleOffset)
            && int32_t( mDataBlocks.back().first + mDataBlocks.back().second->getFilledSpaceSize() ) >= nextSampleOffset){
            return true;
        }
        return false;
    }

protected:
    // first: offset value of start of AseBufferPtr from moof (fragment)
    // second : AseBufferPtr
    std::vector< std::pair<int32_t, ase::AseBufferPtr> > mDataBlocks;

    uint32_t                    mSizeInBuffer;
};
typedef shared_ptr<MediaFragment> MediaFragmentPtr;


/*
 * **************
 * Parser Context
 * **************
 *
 */

class ParserContext : public netflix::containerlib::mp4parser::Context
{
public:
    ParserContext(std::map<uint32_t, shared_ptr<TrackHeader> >& trackHeaders,
                  std::map<uint32_t, ase::EditList>& editLists);

    virtual Context::ParserState onMovieParseEnd(shared_ptr<netflix::containerlib::mp4parser::MovieContext> /*movieContext*/);
    virtual Context::ParserState onTrackFragmentParseEnd(shared_ptr<netflix::containerlib::mp4parser::TrackFragmentContext> trackFragmentContext);
    virtual bool isProtectionSchemeSupported(uint32_t schemeType);
    void setFragmentHeader(shared_ptr<FragmentHeader> fragmentHeaderPtr);

protected:
    std::map<uint32_t, shared_ptr<TrackHeader> >& mTrackHeaders;
    shared_ptr<FragmentHeader> mFragmentHeaderPtr;
    std::map<uint32_t, ase::EditList >& mEditLists;
};

class VideoParserContext : public ParserContext
{
public:
    VideoParserContext(std::map<uint32_t, shared_ptr<TrackHeader> >& videoTrackHeaders,
                       std::map<uint32_t, ase::EditList>& videoEditLists);

    virtual Context::ParserState onTrackParseEnd(shared_ptr<netflix::containerlib::mp4parser::TrackContext> trackContext);

protected:

};


class AudioParserContext : public ParserContext
{
public:
    AudioParserContext(std::map<uint32_t, shared_ptr<TrackHeader> >& audioTrackHeaders,
                       std::map<uint32_t, ase::EditList>& audioEditLists);

    virtual Context::ParserState onTrackParseEnd(shared_ptr<netflix::containerlib::mp4parser::TrackContext> trackContext);

protected:

};

/*
 *
 */
class MediaSourceStreamParser
{
public:
    typedef std::vector<unsigned char>  MovieHeaderData;
    typedef shared_ptr<MovieHeaderData> MovieHeaderDataPtr;

    typedef std::vector<unsigned char>     FragmentHeaderData;
    typedef shared_ptr<FragmentHeaderData> FragmentHeaderDataPtr;

    enum ParseResult
    {
        NOTHING_AVAILABLE = 0,
        MEDIAHEADER_AVAILABLE,
        MEDIAFRAGMENT_AVAILABLE,
        STREAM_PARSING_ERROR,
    };

    MediaSourceStreamParser( ase::MediaType mediaType );

    /** Reset to original state */
    void reset();

    /** Provide data accoring to Media Source byte stream format
     *  @param pData The data block
     */
    void datain( ase::AseBufferPtr pData,
                 int32_t currentTimestampOffset,
                 uint32_t currentTimestampOffsetTimescale);

    /** Retrieve parsed data
     *  @param pData [out] The output data if result != PARSE_NOTHING
     */
    ParseResult dataout( shared_ptr<TrackHeader>& pTrackHeaderPtr,
                         shared_ptr<MediaFragment>& pMediaFragmentPtr);

protected:
    /*
     * parse moov box. After this call, all track information from moov will be
     * populated into mTrackHeaderPtrs, and all editlist information into
     * mEditLists
     */
    bool parseMoovBox();

    /*
     * for moof parsing of multi-track, we don't know yet which track is this
     * moof for. So, we need to input StreamHeader to get information for
     * whichever moof track.  After this call, moof information will be
     * populated for a single track into mFragmentHeaderPtr. Subsequent mdat can
     * use mFragmentHeaderPtr for MediaFragmentPtr construction.
     */
    bool parseMoofBox(int32_t currentTimestampOffset,
                      uint32_t currentTimestampOffsetTimescale);

    enum ParseState
    {
        STATE_BOX_START = 0,
        STATE_MOOV_BOX,
        STATE_MOOF_BOX,
        STATE_MDAT_BOX,
        STATE_OTHER_BOX,
        STATE_PARSE_ERROR,
        STATE_OUT_OF_MEMORY
    };

    struct OutputRecord
    {
        ParseResult                      mParseResult;
        shared_ptr<TrackHeader>          mTrackHeaderPtr;
        shared_ptr<MediaFragment>        mMediaFragmentPtr;
    };

private:
#ifndef NDEBUG
    void *mThread;
    void checkThread()
    {
        if (!mThread) {
            mThread = Thread::CurrentThreadPtr();
        } else {
            if(mThread != Thread::CurrentThreadPtr()){
                Log::error(TRACE_MEDIAPLAYBACK,
                           "MediaSourceStreamParser accessed by different threads: mThread %p, currThread %p",
                           mThread, Thread::CurrentThreadPtr());
            }
            assert(mThread == Thread::CurrentThreadPtr());
        }
    }
#endif

    /*
     * offset to first Sample from start of mDataBlocks[0]. this value should be
     * used when constructing SampleInformation for first sample.
     */
    ase::MediaType mMediaType;
    // mFragmentOffset is offset from start byte of moof box. This value will be
    // set into data block to indicate what kind of data the datablock is
    // holding. If moof starts in the middle of datablock, this value will be
    // negative.
    int32_t mFragmentOffset;
    uint32_t mCurrentBlockSize;
    uint32_t mRemainingBoxSize;
    ase::AseBufferPtr mDataPtr;

    shared_ptr<MovieHeaderData>    mMovieHeaderDataPtr;
    shared_ptr<FragmentHeaderData> mFragmentHeaderDataPtr;

    // created upon moov parsing. one moov can have multiple tracks
    std::map<uint32_t, shared_ptr<TrackHeader> >  mTrackHeaderPtrs;
    std::map<uint32_t, ase::EditList> mEditLists;

    // created upon moof parsing. one moof can have one fragment header for a track
    shared_ptr<FragmentHeader>        mFragmentHeaderPtr;
    // created upon mdat parsing. one mdat can have one fragment data for a track
    shared_ptr<MediaFragment>  mMediaFragmentPtr;  // created upon mdat parsing

    enum ParseState mParseState;
    enum ParseState mPreviousParseState;

    bool mEnableBoxSizeCheck;
    ullong mExpectedMaxBoxSize;

    // BOX_BOUNDARY_CONDITION
    int32_t mFragmentOffsetForBoxBoundaryCondition;
    ase::AseBufferPtr mAseBufferPtrForBoxBoundaryCondition;
        
    // Queue of output data ready to return in dataout
    std::queue<OutputRecord>    mOutputRecordQueue;

    // to store partial data for box info parsing
    std::vector<unsigned char> mBoxInfoData;

    shared_ptr<VideoParserContext> mVideoContext;
    shared_ptr<AudioParserContext> mAudioContext;
};

}}

#endif
