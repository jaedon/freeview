/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "Mp4Demultiplexer.h"

#include <cassert>
#include <list>
#include <string.h>
#include <limits.h>

#include <nrdbase/Log.h>
#include <nrddemux/Context.h>
#include <nrddemux/Mp4Parser.h>
#include <nrddemux/Reader.h>

#include "StreamAttributes.h"
#include "SampleAttributes.h"
#include "DeviceConstants.h"

#define _FOURCC_LE_(a,b,c,d)  (a | (b << 8) | (c << 16) | (d << 24))
#define _FOURCC_BE_(a,b,c,d)  (d | (c << 8) | (b << 16) | (a << 24))

class MovieBoxParseContext : public netflix::containerlib::mp4parser::Context
{
private:
    netflix::device::AudioAttributes* audioAttr_;
    netflix::device::VideoAttributes* videoAttr_;
    uint32_t&                         defaultDrmAlgorithmID_;
    uint32_t&                         defaultDrmIVSize_;
    std::vector<uint8_t>*             defaultDrmKID_;
    ullong&                           timescaleConverterRatio_;
    std::vector<unsigned char>*       drmHeader_;
    const unsigned char*              drmSystemID_;
    std::vector<unsigned char>*       nflxHeader_;

public:
MovieBoxParseContext(netflix::device::AudioAttributes* audioAttr,
                     netflix::device::VideoAttributes* videoAttr,
                     uint32_t&                         defaultDrmAlgorithmID,
                     uint32_t&                         defaultDrmIVSize,
                     std::vector<uint8_t>*             defaultDrmKID,
                     ullong&                           timescaleConverterRatio,
                     std::vector<unsigned char>*       drmHeader,
                     const unsigned char*              drmSystemID,
                     std::vector<unsigned char> *      nflxHeader)

    : audioAttr_(audioAttr),
      videoAttr_(videoAttr),
      defaultDrmAlgorithmID_(defaultDrmAlgorithmID),
      defaultDrmIVSize_(defaultDrmIVSize),
      defaultDrmKID_(defaultDrmKID),
      timescaleConverterRatio_(timescaleConverterRatio),
      drmHeader_(drmHeader),
      drmSystemID_(drmSystemID),
      nflxHeader_(nflxHeader)
    {}

    virtual bool isProtectionSchemeSupported(uint32_t schemeType);

protected:
    void setTemporalLayerInfo(
        std::vector<netflix::containerlib::mp4parser::TemporalLayerEntry::Data> temporalLayerEntries,
        std::vector<netflix::device::TemporalLayerEntryInfo>& temporalLayerEntryInfo);

    // The data argument is a succession of parameter set NALUs each preceded by
    // a 4-byte, network-byte-ordered NALU size (NALULengthSize of 4 is
    // assumed).
    void appendParameterSet(
        std::list<shared_ptr<netflix::containerlib::mp4parser::TrackContext::ParameterSet> >::iterator startIter,
        std::list<shared_ptr<netflix::containerlib::mp4parser::TrackContext::ParameterSet> >::iterator endIter,
        std::vector<unsigned char>& data);

    void appendParameterSet(
        std::list<shared_ptr<netflix::containerlib::mp4parser::TrackContext::ParameterSet> > parameterSets,
        std::vector<unsigned char>& data);

    ParserState getAudioAttrOnTrackParseEnd(shared_ptr<netflix::containerlib::mp4parser::TrackContext> trackContext);

    // In the Mp4 moov header, the mvcC contains both the base view sequence
    // parameter set (SPS) and the dependent view subsequence parameter set
    // (SSPS). The track context gives us a list containing both in the order in
    // which they appear in the mvcC. Since ISO 14496-15 mandates that the SSPS
    // follow any SPSs, if we take the last one in the list, it should be the
    // SSPS.
    //
    // In the Mp4 moov header, the mvcC contains both the base view picture
    // parameter set (PPS) and the dependent view PPS. We specifically want the
    // dependent view PPS. In the case of PPSs from the mvc box, the last one in
    // the list should be the dependent view picture parameter set.
    ParserState getMVCAttrOnTrackParseEnd(shared_ptr<netflix::containerlib::mp4parser::TrackContext> trackContext);

    // The paramSet argument is an H.264/AVC parameter set that begins with a
    // 4-byte Annex B start code or Mp4-style 4-byte size field. The 5th byte
    // contains the NALU type for the parameters set in bits 4-8.
    bool verifyAVCParameterSetNaluType(const std::vector<unsigned char>& paramSet,
                                       unsigned char naluTypeCode);

    bool verifyHEVCParameterSetNaluType(const std::vector<unsigned char>& paramSet,
                                        unsigned char naluTypeCode);

    ParserState getVideoAttrOnTrackParseEnd(shared_ptr<netflix::containerlib::mp4parser::TrackContext> trackContext);

    virtual ParserState onTrackParseEnd(shared_ptr<netflix::containerlib::mp4parser::TrackContext> trackContext);

    virtual ParserState onProtectionSystemHeaderFound(
        std::vector<uint8_t>& systemID,
        std::vector<uint8_t>& protectionSystemSpecificData);

    virtual ParserState onMovieParseEnd(shared_ptr<netflix::containerlib::mp4parser::MovieContext> /*movieContext*/);
};

/**
 *
 *  MovieFragmentParseContext
 *
 */
class MovieFragmentParseContext : public netflix::containerlib::mp4parser::Context
{
private:
    shared_ptr<netflix::containerlib::mp4parser::TrackFragmentContext>& trackFragmentContext_;

public:
    MovieFragmentParseContext(shared_ptr<netflix::containerlib::mp4parser::TrackFragmentContext>& trackFragmentContext)
        : trackFragmentContext_(trackFragmentContext) {}

protected:
    virtual ParserState onTrackFragmentParseEnd(
        shared_ptr<netflix::containerlib::mp4parser::TrackFragmentContext> trackFragmentContext)
    {
        trackFragmentContext_ = trackFragmentContext;
        return PARSE_STOP;
    }
};

/**
 *
 * StreamParser
 *
 */
class netflix::device::Mp4Demultiplexer::StreamParser
{
private:
    template <typename ListType> class BlockListCache;

    enum FragmentParseState
    {
        NEW_FRAGMENT,
        NOT_ENOUGH_DATA,
        CONTINUE_FRAGMENT,
        SKIP_UNTIL_NEXT_FRAGMENT
    };

    class DataBlockStream : public netflix::containerlib::mp4parser::ByteStream
    {
    private:
        IConsumerListView<DataBlock>::iterator startBlock_;
        IConsumerListView<DataBlock>::iterator endBlock_;

        ullong totalOffset_; // Offset in the whole pool of bytes in the list
                               // of blocks.
        ullong totalSize_;   // The total number of bytes in all of the
                               // blocks.
        uint32_t currentBlockOffset_;

        IConsumerListView<DataBlock>::iterator currentBlock_;
        bool     good_;

    public:
        DataBlockStream(IConsumerListView<DataBlock>::iterator startBlock,
                        IConsumerListView<DataBlock>::iterator endBlock,
                        uint32_t totalSize) :
          startBlock_(startBlock),
          endBlock_(endBlock),
          totalOffset_(0),
          totalSize_(totalSize),
          currentBlockOffset_(0),
          currentBlock_(startBlock_),
          good_(true)
        {
            assert(startBlock_ != NULL);
        }

        void reset()
        {
            totalOffset_ = 0;
            currentBlockOffset_ = 0;
            currentBlock_ = startBlock_;
            good_ = true;
        }


        virtual bool readByte(uint8_t& data)
        {
            if(totalOffset_ >= totalSize_)
            {
                good_ = false;
                return false;
            }

            while(currentBlock_ != endBlock_ && currentBlock_->getDataSize() - currentBlockOffset_ < 1)
            {
                ++currentBlock_;
                currentBlockOffset_ = 0;
            }

            // We should have at least a byte because we checked above.
            assert(currentBlock_ != endBlock_);

            data = *(currentBlock_->getData() + currentBlockOffset_);
            currentBlockOffset_++;
            totalOffset_++;
            return true;
        }

        virtual bool readBytes(std::vector<uint8_t>& data)
        {
            if(good_ == false)
            {
                return false;
            }
            if(data.size() > totalSize_ - totalOffset_)
            {
                good_ = false;
                return false;
            }
            if(data.size() > 0)
            {
                writeBytes(data, 0, data.size());
            }
            return true;
        }

        bool writeBytes(esplayer::ISampleWriter& writer, uint32_t numBytes)
        {
            if(good_ == false)
            {
                return false;
            }
            if(numBytes > totalSize_ - totalOffset_)
            {
                good_ = false;
                return false;
            }
            if(numBytes > 0)
            {
                if(writeBytesInternal(writer, numBytes) == false)
                {
                    good_ = false;
                    return false;
                }
            }
            return true;

        }

        bool writeBytesInternal(esplayer::ISampleWriter& writer, uint32_t numBytes)
        {
            uint32_t numBytesWritten = 0;
            while(numBytesWritten < numBytes)
            {
                assert(currentBlock_ != endBlock_);

                if(currentBlock_->getDataSize() - currentBlockOffset_ >= numBytes - numBytesWritten) {
                    uint32_t writeSize = numBytes - numBytesWritten;
                    if(writer.write( currentBlock_->getData() + currentBlockOffset_, writeSize) != writeSize)
                    {
                        return false;
                    }
                    currentBlockOffset_ += writeSize;
                    break;
                } else {
                    uint32_t writeSize = currentBlock_->getDataSize() - currentBlockOffset_;
                    if(writer.write( currentBlock_->getData() + currentBlockOffset_, writeSize) != writeSize)
                    {
                        return false;
                    }
                    numBytesWritten += writeSize;
                    ++currentBlock_;
                    currentBlockOffset_ = 0;
                }
            }
            totalOffset_+= numBytes;
            return true;
        }


        void writeBytes(std::vector<uint8_t>& data, uint32_t offset, uint32_t numBytes)
        {
            uint32_t numBytesWritten = 0;
            while(numBytesWritten < numBytes)
            {
                assert(currentBlock_ != endBlock_);

                if(currentBlock_->getDataSize() - currentBlockOffset_ >= numBytes - numBytesWritten) {
                    uint32_t writeSize = numBytes - numBytesWritten;
                    memcpy(&data[numBytesWritten+offset], currentBlock_->getData() + currentBlockOffset_, writeSize);
                    currentBlockOffset_ += writeSize;
                    break;
                } else {
                    uint32_t writeSize = currentBlock_->getDataSize() - currentBlockOffset_;
                    memcpy(&data[numBytesWritten+offset], currentBlock_->getData() + currentBlockOffset_, writeSize);
                    numBytesWritten += writeSize;
                    ++currentBlock_;
                    currentBlockOffset_ = 0;
                }
            }
            totalOffset_+= numBytes;
        }

        virtual bool readBytes(std::vector<uint8_t>& data, uint32_t offset, uint32_t numBytes)
        {
            assert(offset < data.size() &&
                   offset + numBytes <= data.size() &&
                   offset + numBytes > offset);

            if(good_ == false)
            {
                return false;
            }
            if(numBytes > totalSize_ - totalOffset_)
            {
                good_ = false;
                return false;
            }
            if(numBytes > 0)
            {
                writeBytes(data, offset, numBytes);
            }
            return true;
        }

        virtual bool skipBytes(ullong numBytes)
        {
            if(good_ == false)
            {
                return false;
            }
            if(numBytes > totalSize_ - totalOffset_)
            {
                good_ = false;
                return false;
            }
            ullong numBytesSkipped = 0;
            while(numBytesSkipped < numBytes)
            {
                assert(currentBlock_ != endBlock_);

                if(currentBlock_->getDataSize() - currentBlockOffset_ >= numBytes - numBytesSkipped) {
                    uint32_t skipSize = numBytes - numBytesSkipped;
                    currentBlockOffset_ += skipSize;
                    break;
                } else {
                    uint32_t skipSize = currentBlock_->getDataSize() - currentBlockOffset_;
                    numBytesSkipped += skipSize;
                    currentBlockOffset_ = 0;
                    ++currentBlock_;
                }
            }
            totalOffset_+= numBytes;
            return true;
        }

        virtual ullong tell() const
        {
            return totalOffset_;
        }

        virtual bool good() const
        {
            return good_;
        }

        ullong getNumBytesAvailable()
        {
            return totalSize_;
        }
    };


    // Structures for recording the position and size information of AVC/MVC
    // access units. The structures are used to provide NALU size information
    // and for MVC NAL unit reordering.  These are used when an mvc stream with
    // access units that contain both views needs to be split into two streams,
    // base and dependent, that can be decrypted and fed as independent decoder
    // inputs.
    ///////////////////////////////////////////////////////////////////////////

    // A NALU may span several blocks.  This structure is a record of
    // the portion of a data block that belongs to a particular NALU.
    //
    struct NaluBlockPosInfo {
        DataBlock*    block_;
        uint32_t      startOffset_;
        uint32_t      endOffset_;
    };

     //
     // A NALU may span several blocks.  This structure contains a
     // vector of all the blocks that contain bytes for this NALU.  The
     // struct also records the size of the NALU which should be the sum
     // of (endOffset_ - startOffset) summed over all the constituent
     // blocks.
    struct NaluInfo {
        NaluInfo() : size_(0),
                     encByteOffset_(0),
                     numLeadingClearBytes_(0),
                     numEncryptedBytes_(0),
                     firstByte_(0)
        {}

        std::vector<NaluBlockPosInfo> blockPositions_;
        uint32_t                      size_;
        ullong                        encByteOffset_;
        uint32_t                      numLeadingClearBytes_;
        uint32_t                      numEncryptedBytes_;
        uint8_t                       firstByte_;
    };
     //
     // AU info consists of vector of the nalus in the order that they
     // appear in the stream, a vector of nalus grouped according to
     // view group.
    class AUInfo {
    public:
        AUInfo() :
          nalusGroupedByView_(2),
          encryptionMapEntriesUsed_(0),
          isUsedForReference_(false)
        {
        }

        void reset()
        {
            nalus_.clear();
            nalusGroupedByView_.clear();
            viewSize_.clear();
            viewSize_.clear();
            encryptionMapEntriesUsed_ = 0;
            isUsedForReference_ = false;
        }

        std::vector<NaluInfo>                nalus_;
        std::vector<std::vector<NaluInfo*> > nalusGroupedByView_;
        std::vector<uint32_t>                viewSize_;
        uint32_t                             encryptionMapEntriesUsed_;
        bool                                 isUsedForReference_;
    };

public:
    enum SyncMode
    {
        NOP,
        SYNC_TO_PTS,
        SEARCH_SYNC_PTS,
        SYNC_TO_CURRENT_PLAYBACK_PTS,
    };

private:
    netflix::containerlib::mp4parser::Mp4Parser movieFragmentParser_;
    MovieFragmentParseContext movieFragmentParseContext_;
    shared_ptr<netflix::containerlib::mp4parser::TrackFragmentContext> trackFragmentContext_;

    FragmentParseState fragmentParseState_;
    SyncMode syncMode_;

    bool endOfStream_;
    bool endOfStreamLogged_;

    bool discontinuity_;
    bool firstSample_;
    bool isStartOfFragment_;
    bool sampleWritePending_;

    // The MP4Demultiplexer passes both StreamParsers a reference shared objects
    // so the audio parser can see when the video parse sets these values.
    ullong& syncPTS_;
    bool&     syncPTSIsValid_;

    uint32_t currentSampleEntryIndex_;
    llong  currentGopTimeOffset_;

    // As per 14496-12:2010 and the as-yet-unofficial DASH standard
    // (Amd.3:2011(E) 8/17/2011 as of this writing), PTS values are computed as
    // DTS + CTS + offset from moov atom's edit list box for the track. However,
    // because CE3 streams didn't initially include an edit list box, we don't
    // apply any offset that might be supplied there. Instead, we assume that
    // the PTS time stamp of the first sample in the stream (an IDR frame in the
    // case of video) should be 0, which is correct for CE3 streams (verified
    // with D. Ronca).
    //
    // For streams with B-Frames, there is a non-zero CTS value for the first
    // IDR frame in a GOP (in CE3 streams the CTS value is one frame period).
    // This causes the presentation times for the samples in the GOP to be
    // offset by the CTS value for the IDR frame.  Instead of starting with a
    // PTS of zero, this would cause a video stream with B-frames to start with
    // a PTS equal to one frame period.  This introduces an AV synch offset
    // relative to the audio track.  Worse, if a Baseline profile stream with no
    // B-frames switches to a Main profile stream with B-frames, PTS values will
    // skip a frame period in the switch to the Main Profile stream.
    //
    // To remove the offset, we note the CTS value for the IDR frame and
    // use it to correct the PTS values of the GOP's samples.
    llong  ctsOffsetOfFirstSampleInGop_;

    ullong timescaleConverterRatio_;
    static const uint32_t NALULengthSize_; // For H.264, H.265
    std::vector<uint8_t> ADTSHeader_; // For AAC


    uint32_t videoSampleDuration_;

    IConsumerListView<DataBlock>& blockList_;
    shared_ptr< BlockListCache< IConsumerListView<DataBlock> > > blockListCache_;

    // The byte offset from the start of the moof box corresponding to the first byte of the
    // first block on blockList_ (the byte at blockList_.begin()->getOffset()
    uint32_t currentOffsetFromMoof_;

    esplayer::SampleAttributes    sampleAttributes_;
    esplayer::MediaType           mediaType_;
    esplayer::Format3D            format3D_;
    AUInfo    au_;
    uint32_t  level_idc_;
    std::vector<ContentProfile> videoProfiles_;
    bool supportSeamlessFramerateSwitching_;
    bool HEVC_Level_5_1_Supported_;
    bool HEVC_Level_5_0_Supported_;

    // store current stream's bit depth so that we can report when bit depth changed
    uint32_t currBitDepth_;

    shared_ptr<Mp4Demultiplexer> mp4Demux_;

/*
 * public method for Mp4Demultiplexer::StreamParser
*/
public:
    StreamParser(ullong& syncPTS,
                 bool& syncPTSValidFlag,
                 IConsumerListView<DataBlock>& list,
                 esplayer::MediaType mediaType,
                 esplayer::Format3D video3DFormat = esplayer::NOT_3D,
                 const std::vector<ContentProfile>& videoProfiles = std::vector<ContentProfile>(),
                 const bool supportSeamlessFramerateSwitching = false);

    // Counts the total number of bytes in the list of blocks.
    static uint32_t getNumBytesAvailable(IConsumerListView<DataBlock>::iterator start,
                                         IConsumerListView<DataBlock>::iterator end);

    static uint32_t getInt(unsigned char* buff, const uint32_t numBytes);

    /////
    // Reads the network-byte-ordered NALU size field in an avc elementary stream.
    //
    // Args:
    //      itStart[in]: an iterator to the first block where the Nalu appears.
    //
    //      itEnd:   an end-iterator for the list of blocks.
    //      offset:  the offset in bytes of the NALU's position in the first
    //               block (relative to the pointer returned by getData()).
    //      NALULengthSize:
    //               Should be either 2 or 4. It's a sequence parameter set
    //               value that can be read from the avcConfigData in the
    //               MP4 file.
    //      size[out]:
    //               The size of the NALU as given in the elementary stream.
    // Returns: true if a nalu size was read as an integer of length NALULengthSize bytes.
    //          false if there were not enough bytes to read the size.
    bool getNaluSize(IConsumerListView<DataBlock>::iterator startPos,
                     IConsumerListView<DataBlock>::iterator endPos,
                     const int offset,
                     uint32_t& size,
                     uint8_t& naluFirstByte);
    static bool getHEVCLevelIdcFromSPS(const std::vector<unsigned char> sequenceParameterSet,
                                       uint32_t& level_idc );

    static bool getHEVCLevelIdcFromNalu(IConsumerListView<DataBlock>::iterator startPos,
                                 IConsumerListView<DataBlock>::iterator endPos,
                                 const uint32_t size,
                                 const int offset,
                                 uint32_t& level_idc_out );

     /////
     // Finds the size of the NALU and determines which data blocks
     // contain the nalu and the byte ranges in the blocks that it
     // occupies.
     //
     // Arguments: startIt [in/out]: an iterator the block where the
     //               nalu starts. Upon return the iterator points to the
     //               endIt (end of list), or to the block where the next
     //               nalu starts.
     //            endIt: the end of the list of blocks.
     //            offset [in/out]: the offset of the nalu in the block
     //               where the nalu appears. Upon retur, the offset is
     //               offset of te next nalu in the block pointed to by
     //               startIt upon return.
     //           naluInfo[out]: size and block position info for the Nalu.
     //
     // Returns: true if the nalu size bytes and end bytes are successfully
     //          parsed and the end lies within the bounds of the block list
     //          iterators.  Returns false otherwise.
    bool getNextNaluInfo(IConsumerListView<DataBlock>::iterator& startPos,
                         IConsumerListView<DataBlock>::iterator endPos,
                         const VideoAttributes* videoAttr,
                         uint32_t& offset,
                         NaluInfo& naluInfo,
                         uint32_t& level_idc_out);


    // Builds a vector of naluInfos for the nalus that make up the sample
    // starting at the front of the blockList_. When this returns, the nalu
    // infos specify which portions of which blocks contain the NALU.  The
    // method also examines the NALUs' ref_idc bits to determine whether the AU
    // it is used for reference. It sets the AUInfo's isUsedForReference flag to
    // true if any NALU's ref_idc bits are set.
    //
    // If this returns bad, there's something wrong with the encoding.  We
    // should skip until the next fragment.
    NFErr getAUInfo(uint32_t offset, uint32_t auSize, const VideoAttributes* videoAttr);

    ////
     // Looks at the MP4 fragment's sample to group box to determine
     // which sample group description of type scnm (scalable NALU map
     // entry) specifies, for this AU, the views that each NALU belongs
     // to.  Finds the view index mapping for each NALU and then
     // enqueues the NALU in the AU's ordered vector of nalus for that
     // view.
     //
    NFErr addNaluToViewGroup(NaluInfo& nalu, uint32_t naluOrderNum,
                             const MVCAttributes* mvcAttr);

    ////
     // Looks at the sample's encryption map entry to see which bytes
     // in the access unit are clear and which are encrypted. Based on
     // nalu order and sizes, determines how the clear and encrypted au
     // bytes map onto each NALU. For each nalu, computes the number of
     // clear bytes and the number of encrypted. Also records total number
     // of AU bytes encrypted before the encrypted portion of a
     // nalu. With this info, we can decrypt nalus independently of one
     // another and not just in the order that they were encrypted.
     //
    NFErr computeNaluReordering(const VideoAttributes* videoAttr);

    esplayer::Status writeNalu(const NaluInfo& nalu,
                               esplayer::ISampleWriter& writer,
                               uint32_t viewNum = 0);

    esplayer::Status writeAU(esplayer::ISampleWriter& writer);

    esplayer::Status  writeMvcSplitAU(esplayer::ISampleWriter& writer);

    void parseNewFragment();

    void popBytesFromDataBlockList(uint32_t bytesToRemove);

    // Searches for a fragment header in the list of blocks starting from
    // startBlock_ and attempts to parse.  If it succeeds in parsing, it
    // returns.  If parsing fails, searches for the next fragment header.  Keeps
    // searching til it runs out of blocks or succeeds.
    //
    // Side effects, updateds startBlock_ to the block containing the end of the
    // fragment header, if one is successfully parsed. Otherwise, returns with
    // startBlock_ = lEndBlock.
    bool findNextFragmentHeaderAndParse();

    // We don't want to deliver an audio sample with a time stamp that precedes
    // the first video sample.  The audio parser is initialized with sync mode
    // SYNC_TO_PTS and is set to SYNC_TO_PTS at every flush.  The video parser
    // is set to SEARCH_SYNC_PTS, on the other hand. syncPTS_ is a shared
    // reference between both parsers as is syncPTSIsValid_. Once the video
    // stream (set to SEARCH_SYNC_PTS) delivers a sample, it sets the syncPTS_
    // and makes it valid.
    bool sampleShouldBeSkipped();

    bool temporalLayerShouldBeSkipped();

    // If this is a key frame, we will be prepending the SPS and PPS.  If this
    // is mvc, we will also be prepending dependent-view SSPS and PPS.  If we
    // are in mvc split mode, the dependent view SSPS and PPS will prepended to
    // the second view buffer.
    //
    // Similarly, nalu size entries are added for the nalus of the view
    // including any parameter sets that are prepended.
    void setVideoSampleSizeAndNaluSizeAttributes( VideoAttributes *videoAttr);

    // Adds entries to the sample attributes arrays of nalu sizes. Does not add
    // entries for parameter set nalus that will be prepended. Those entries
    // should be added before this method is called.
    void addNonParamSetNaluSizeEntries();

    // See details about the sdtp flags in 14496-12:2008.
    void setVideoSampleDependencyFlags();

    bool setSampleVideoAttributes(uint32_t offset,uint32_t size, VideoAttributes *videoAttr);

    bool setSampleAudioAttributes(uint32_t size, const AudioAttributes *audioAttr);

    void setSampleDrmAttributes(const DataBlock& block);

    void setSampleTimestampAttributes(const netflix::containerlib::mp4parser::TrackFragmentContext::SampleEntry& sample,
                                      const DataBlock& block);

    esplayer::Status setSampleAttributes(const DataBlock& block);

    bool blockListIsEmpty();

    esplayer::Status endOfStream();

    esplayer::Status getNextSample(esplayer::ISampleWriter& sampleWriter);

    esplayer::Status getNextSampleSize(uint32_t& sampleSize, uint32_t viewNum = 0);

    esplayer::Status getNextSampleTimestamps(llong& dts, llong& pts);

    esplayer::Status getNextSampleAttributes();

    // If this is encrypted video, we need to add subsample mapping entries for
    // the parameter set NALUs we prepend to the IDR frame at the start of a
    // fragment. Since CE3 parameter sets are not encrypted, the mapping entry's
    // clear part is the length of the NALUs that are prepended and the
    // encrypted part is of length 0.
    //
    // This should only be called for samples that are encrypted video IDR
    // frames.
    void addSubsampleEntriesForVideoParamSetsThatWillBePrepended();

    void prependVideoParameterSets(esplayer::ISampleWriter& sampleWriter);

    esplayer::Status writeSample(esplayer::ISampleWriter& sampleWriter);

    bool createADTSHeader(const std::vector<uint8_t>& codecSpecificData);

    bool updateADTSHeaderSizeBytes(uint32_t sampleSize);

    /**
     *  inline functions
     */

    // setDiscontinuity is called on the AdaptiveStreamingPlayer pumping loop
    // thread. It is called during a flush when the audio and video elementary
    // stream player callbacks (ESPlayerCallback) are blocked from calling into
    // the Mp4Demuxer.
    inline void setDiscontinuity()
    {
        discontinuity_ = true;
        firstSample_ = true;
        sampleWritePending_ = false;

        fragmentParseState_ = SKIP_UNTIL_NEXT_FRAGMENT;
        endOfStream_ = false;
        endOfStreamLogged_ = false;
    }

    inline void setEndOfStream()
    {
        endOfStream_ = true;
    }

    inline void setSyncMode(SyncMode syncMode) { syncMode_ = syncMode; }

    inline SyncMode syncMode() { return syncMode_; }

    inline uint32_t getVideoSampleDuration() { return videoSampleDuration_; }

    inline uint32_t naluBytesInBlock(const std::vector<NaluBlockPosInfo>::const_iterator&  it)
    {
        return it->endOffset_ - it->startOffset_;
    }

    inline uint32_t timescaleConvert(ullong time)
    {
        return static_cast<uint32_t>(
            (time * timescaleConverterRatio_ + (1 << 30)) >> 31);
    }

    inline bool blockListStartsWithUnparsedFragment()
    {
        return  (blockList_.begin()->getDataBuffer().isStartOfGop() &&
                 blockList_.begin()->isParsed() == false);
    }

    inline bool fragmentParsingIsNeeded()
    {
        return (blockListStartsWithUnparsedFragment() ||
                fragmentParseState_ == SKIP_UNTIL_NEXT_FRAGMENT);
    }

    /**
     *  static functions
     */

    // Takes an array of bytes and returns a string of the byte values
    // in hex.
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

        // As specified in IElementaryStreamPlayer, the codec parameter strings for
    // avc and mvc specify the type of the codec configuration (avc1 or mvc1 and
    // avc1) plus the profile, constraint flags, and level expressed in hex.
    // Bytes indexed 5-7 in the sequence parameter and subsequence parameter
    // sets, stored in the sequenceParameterSet_ and subsequenceParameterSet,
    // respectively, contain the profile,constraint, and level (see ITU-T Rec
    // 14496-10:2010 pg 40).
    static std::string getVideoCodecParam(const VideoAttributes *videoAttr)
    {
        assert(videoAttr);
        std::string codecString;

        if(videoAttr->formatID_ ==  _FOURCC_BE_('1','c','v','a'))
        {
            if(videoAttr->mvcAttributes_) {
                if(videoAttr->mvcAttributes_->subsequenceParameterSet_.size()
                   >= 8)
                {
                    codecString = "mvc1." +
                        bytesToHex(&videoAttr->mvcAttributes_->
                                   subsequenceParameterSet_[5], 3);
                    codecString += ", ";
                }
            }
            if(videoAttr->sequenceParameterSet_.size() >= 8)
            {
                codecString += "avc1." +
                    bytesToHex(&videoAttr->sequenceParameterSet_[5], 3);
            }
        } else if (videoAttr->formatID_ == _FOURCC_BE_('M','P','4','V'))
        {
            codecString =  "mp4v";
        } else if (videoAttr->formatID_ == _FOURCC_LE_('h','v','c','1'))
        {
            codecString =  "hvc1";
            // + "." + profile_space in no_charactor, 1, 2, 3
            // + "." + profile_idc in decimal
            // + "." + profile_compatibility indications in hex
            // + "." + tier_flag in "L" or "H"
            // + "." + level_idc in decimal number
            // + "." ....
        } else if (videoAttr->formatID_ == _FOURCC_LE_('h','e','v','1'))
        {
            codecString =  "hev1.";
        }

        if(codecString.empty())
        {        Log::error(TRACE_MEDIAPLAYBACK, "PlaybackDeviceNative::getVideoCodecParam(): "
                   "unknown codec fourCC %x", videoAttr->formatID_);
        }
        return codecString;
    }

    static std::string getAudioCodecParam(const AudioAttributes *audioAttr)
    {
        assert(audioAttr);

        if(audioAttr->formatID_ ==  0x1610)
        {
            return "mp4a.40.2, mp4a.40.5";
        } else if (audioAttr->formatID_ == 0xeac3) {
            return  "ec-3.A6";
        } else if (audioAttr->formatID_ == 0x6771) {
            return  "ogg vorbis";
        }
        Log::error(TRACE_MEDIAPLAYBACK, "Mp4Demultiplexer::StreamParser::getAudioCodecParam(): "
                   "unknown codec fourCC %x", audioAttr->formatID_);
        return "";
    }

    void setDemultiplexer(const shared_ptr<Mp4Demultiplexer> mp4Demux);
    void resetDemultiplexer();
    inline void set3DFormat(esplayer::Format3D format3D)
    {
        format3D_ = format3D;
    }

};
