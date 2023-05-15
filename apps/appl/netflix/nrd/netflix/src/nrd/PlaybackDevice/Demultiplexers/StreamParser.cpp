/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#include <nrd/NrdConfiguration.h>
#include <nrd/IPlaybackDevice.h>
#include "EventQueue.h"
#include "StreamParser.h"

#ifndef NDEBUG
#define NDEBUG 1
#endif
#ifdef NDEBUG
#define PRINT(fmt, ...)
#else
#include <stdio.h>
#define PRINT(fmt, ...) printf("DEMUX DEBUG:: " fmt, __VA_ARGS__ )
#endif

#define HEVC_NAL_UNIT_ID_VPS 32
#define HEVC_NAL_UNIT_ID_SPS 33
#define HEVC_NAL_UNIT_ID_PPS 34
#define POPULATE_START_CODE 1
//#define STREAM_ERROR_DEBUG

using namespace std;
using namespace netflix;
using namespace netflix::device;
using namespace netflix::containerlib::mp4parser;

const uint32_t Mp4Demultiplexer::StreamParser::NALULengthSize_ = 4;


template <typename ListType>
class Mp4Demultiplexer::StreamParser::BlockListCache {
public:
    BlockListCache(ListType& blockList)
      : blockList_(blockList)
      , begin_locked_(false)
      , last_locked_(false)
      , num_bytes_available_(0)
    {
    }

    void updateSnapshot() {
        bool first_update_after_reset = blockList_.updateListSnapshot();

        // the list was cleared, reset the cache
        if (first_update_after_reset) resetCache();

        if (!begin_locked_ && blockList_.begin() != blockList_.end()) {
            // lock the begin iterator
            begin_locked_ = true;
            begin_ = blockList_.begin();
        }

        if (!begin_locked_) return; // list is empty, nothing to do.

        if (!last_locked_) {
            // lock the last iterator
            last_ = blockList_.begin();
            num_bytes_available_ += blockList_.begin()->getDataSize();
            last_locked_ = true;
        }

        if (last_locked_) {
            typename ListType::iterator prev_last = last_;
            while (++prev_last != blockList_.end()) {
                num_bytes_available_ += prev_last->getDataSize();
                last_ = prev_last;
            }
        }
    }

    uint32_t consumeBytes(uint32_t bytes_to_consume) {
        uint32_t remaining_bytes = bytes_to_consume;
        while(remaining_bytes > 0)
        {
            if(blockList_.begin()->getDataSize() > remaining_bytes)
            {
                blockList_.begin()->advanceOffset(remaining_bytes);
                num_bytes_available_ -= remaining_bytes;
                remaining_bytes       = 0;
            } else {
                uint32_t size = blockList_.begin()->getDataSize();
                blockList_.begin()->advanceOffset(size);
                num_bytes_available_ -= size;
                remaining_bytes      -= size;
                ++begin_;
                blockList_.pop_front();

                if(blockList_.begin() == blockList_.end()) {
                    // the list is empty and the begin iterator is no longer valid.
                    assert(num_bytes_available_ == 0);
                    resetCache();
                    break;
                }
            }
        }
        return bytes_to_consume - remaining_bytes;
    }

    void popFront() {
        if(blockList_.begin() != blockList_.end())
            num_bytes_available_ -= blockList_.begin()->getDataSize();

        blockList_.pop_front();
        if(blockList_.begin() != blockList_.end()) {
            ++begin_;
        } else {
            // the list is empty and the begin iterator is no longer valid.
            assert(num_bytes_available_ == 0);
            resetCache();
        }
    }

    uint32_t getNumBytesAvailable() {
        return num_bytes_available_;
    }

private:
    void resetCache() {
        begin_locked_        = false;
        last_locked_         = false;
        num_bytes_available_ = 0;
    }

    ListType& blockList_;

    bool begin_locked_;
    typename ListType::iterator begin_; // begin of list;
    bool last_locked_;
    typename ListType::iterator last_;  // end - 1
    uint32_t num_bytes_available_;
};


/**
 *
 * MovieBoxParseContext
 *
 */
bool MovieBoxParseContext::isProtectionSchemeSupported(uint32_t schemeType)
{
    if(schemeType == 0x70696666) // 'piff'
        return true;
    else
        return false;
}


void MovieBoxParseContext::setTemporalLayerInfo(
    std::vector<TemporalLayerEntry::Data> temporalLayerEntries,
    std::vector<TemporalLayerEntryInfo>& temporalLayerEntryInfo)
{
    std::vector<TemporalLayerEntry::Data>::const_iterator iter;
    for(iter  = temporalLayerEntries.begin(); iter != temporalLayerEntries.end(); iter++)
    {
        struct TemporalLayerEntryInfo info;
        info.tlTemporalLayerId_ = iter->temporalLayerId_;
        info.tlLevelIdc_        = iter->tllevel_idc_ ;
        info.tlFrameRateValue_  = iter->tlAvgFrameRate_ ;
        info.tlFrameRateScale_  = 256;

        temporalLayerEntryInfo.push_back(info);
    }
}

// The data argument is a succession of parameter set NALUs each preceded by
// a 4-byte, network-byte-ordered NALU size (NALULengthSize of 4 is
// assumed).
void MovieBoxParseContext::appendParameterSet(
    list<shared_ptr<TrackContext::ParameterSet> >::iterator startIter,
    list<shared_ptr<TrackContext::ParameterSet> >::iterator endIter,
    vector<unsigned char>& data)
{
    list< shared_ptr<TrackContext::ParameterSet> >::const_iterator iter;

    for(iter  = startIter; iter != endIter; iter++)
    {
        uint32_t psSize = (*iter)->size();
        if(psSize == 0)
            continue;

#if defined( POPULATE_START_CODE )
        // Add ISO 14496-10:Annex B start codes.
        data.push_back(0);
        data.push_back(0);
        data.push_back(0);
        data.push_back(1);
#endif

        uint32_t s = data.size();
        data.resize(s + psSize);
        memcpy(&data[s], &(*(*iter))[0], psSize);
    }
}

void MovieBoxParseContext::appendParameterSet(
    list<shared_ptr<TrackContext::ParameterSet> > parameterSets,
    vector<unsigned char>& data)
{
    appendParameterSet(parameterSets.begin(),
                       parameterSets.end(),
                       data);
}

Context::ParserState MovieBoxParseContext::getAudioAttrOnTrackParseEnd(shared_ptr<TrackContext> trackContext)
{
    assert(audioAttr_);

    switch(trackContext->sampleType())
    {
    case _FOURCC_BE_('m','p','4','a'):
        // Note: here we assumed that HE-AAC is the only format used
        // as MP4A.
        audioAttr_->formatID_ = 0x1610; // MEDIASUBTYPE_MPEG_HEAAC
        break;

    case _FOURCC_BE_('e','c','-','3'):
        audioAttr_->formatID_ = 0xEAC3; // DOLBY DIGITAL PLUS
        break;

    case _FOURCC_BE_('o','v','r','b'):
        // Note: we don't really know which version of Vorbis it
        // is. Assume that it's always 3+.
        audioAttr_->formatID_ = 0x6771; // OGG VORBIS 3 PLUS
        break;
    }
    trackContext->getAudioSampleInfo(audioAttr_->numberOfChannels_,
                                     audioAttr_->bitsPerSample_,
                                     audioAttr_->samplesPerSecond_);
    audioAttr_->samplesPerSecond_ >>= 16;

    if(audioAttr_->formatID_ == 0x1610) // HE-AAC
    {
        // Note: this is a temporary workaround.
        //
        // For HE-AAC streams, the sampling frequency information from ISO
        // AudioSampleEntry is actually the sampling frequency when it is
        // treated as a LC-AAC stream, which is 1/2 of the actual frequency
        // if treated as HE-AAC.
        audioAttr_->samplesPerSecond_ *= 2;
    }
    audioAttr_->bytesPerSecond_ = (audioAttr_->samplesPerSecond_ *
                                   audioAttr_->bitsPerSample_) >> 3;
    audioAttr_->codecSpecificData_ = trackContext->decoderSpecificData();

    return PARSE_CONTINUE;
}

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
Context::ParserState MovieBoxParseContext::getMVCAttrOnTrackParseEnd(shared_ptr<TrackContext> trackContext)
{
    videoAttr_->format3D_ = esplayer::MVC_COMBINED;
    if(videoAttr_->mvcAttributes_)
        delete videoAttr_->mvcAttributes_;
    videoAttr_->mvcAttributes_ = new MVCAttributes;
    // gets deleted when videoAttr_
    // does in PlaybackDeviceNative::close()

    // Copy the SSPS.
    videoAttr_->mvcAttributes_->subsequenceParameterSet_.clear();
    appendParameterSet(--trackContext->mvcSequenceParameterSets().end(),
                       trackContext->mvcSequenceParameterSets().end(),
                       videoAttr_->mvcAttributes_->subsequenceParameterSet_);
    if(verifyAVCParameterSetNaluType(
           videoAttr_->mvcAttributes_->subsequenceParameterSet_, 15) == false)
    {
        Log::error(TRACE_MEDIAPLAYBACK, "Mp4ParseError: mvc subsequence "
                   "parameter set has wrong type");
        return PARSE_ERROR_DATA;
    }

    // Copy the dependent view PPS.
    videoAttr_->mvcAttributes_->pictureParameterSet_.clear();
    appendParameterSet(--trackContext->mvcPictureParameterSets().end(),
                       trackContext->mvcPictureParameterSets().end(),
                       videoAttr_->mvcAttributes_->pictureParameterSet_);
    if(verifyAVCParameterSetNaluType(
           videoAttr_->mvcAttributes_->pictureParameterSet_, 8) == false)
    {
        Log::error(TRACE_MEDIAPLAYBACK, "Mp4ParseError: mvc picture "
                   "parameter set has wrong type");
        return PARSE_ERROR_DATA;
    }

    // We may also need the mapping of nalus to base and dependent views if
    // the device requests MVC_SPLIT.
    videoAttr_->mvcAttributes_->naluToMvcViewMaps_ = trackContext->naluToMvcViewMaps();

    videoAttr_->mvcAttributes_->NALULengthSize_ = trackContext->NALULengthSize();

    return PARSE_CONTINUE;
}

// The paramSet argument is an H.264/AVC parameter set that begins with a
// 4-byte Annex B start code or Mp4-style 4-byte size field. The 5th byte
// contains the NALU type for the parameters set in bits 4-8.
bool MovieBoxParseContext::verifyAVCParameterSetNaluType(const vector<unsigned char>& paramSet,
                                                         unsigned char naluTypeCode)
{
    if(paramSet.size() > 0)
    {
        if((paramSet.size() < 5) ||
           ((paramSet[4] & 0x1f) != naluTypeCode))
        {
            return false;
        }
    }
    return true;
}

bool MovieBoxParseContext::verifyHEVCParameterSetNaluType(const vector<unsigned char>& paramSet,
                                                          unsigned char naluTypeCode)
{
    if(paramSet.size() > 0) {
        if( paramSet.size() < 6 ) {
            return false;
        }

        if( ( ( paramSet[4] & 0x7e ) >> 1 ) != naluTypeCode ) {
            PRINT("wrong parameterSet:expected %d, actual %d\n", naluTypeCode, (paramSet[4] & 0x7e) >> 1 );
            return false;
        }
    }
    return true;
}

Context::ParserState MovieBoxParseContext::getVideoAttrOnTrackParseEnd(shared_ptr<TrackContext> trackContext)
{
    switch(trackContext->sampleType())
    {
    case _FOURCC_BE_('a','v','c','1'):
        videoAttr_->formatID_ = _FOURCC_LE_('a','v','c','1');
        break;

    case _FOURCC_BE_('m','p','4','v'):
        // Note: here we assumed that MP4P2 is the only format used as MP4V.
        videoAttr_->formatID_ = _FOURCC_LE_('M','P','4','V');
        break;

    case _FOURCC_BE_('h','v','c','1'):
        videoAttr_->formatID_ = _FOURCC_LE_('h','v','c','1');
        break;

    case _FOURCC_BE_('h','e','v','1'):
        videoAttr_->formatID_ = _FOURCC_LE_('h','e','v','1');
        break;
    };

    trackContext->getVideoImageSize(videoAttr_->imageWidth_,
                                    videoAttr_->imageHeight_);
    trackContext->getVideoPixelAspectRatio(videoAttr_->pixelAspectRatioX_,
                                           videoAttr_->pixelAspectRatioY_);
    trackContext->getVideoFrameRate(videoAttr_->frameRateValue_,
                                    videoAttr_->frameRateScale_);
    trackContext->getColorInformation(videoAttr_->colorPrimaries_,
                                      videoAttr_->transferCharacteristics_,
                                      videoAttr_->matrixCoefficients_,
                                      videoAttr_->fullRange_);
    trackContext->getVideoBitsDepth(videoAttr_->bitsDepthLuma_,
                                    videoAttr_->bitsDepthChroma_);
    trackContext->getGeneralLevelIdc(videoAttr_->generalLevelIdc_);
    // when new track is parsed, clear previous information
    videoAttr_->temporalLayerEntryInfo_.clear();
    setTemporalLayerInfo(trackContext->temporalLayerEntries(), videoAttr_->temporalLayerEntryInfo_);
    if( videoAttr_->temporalLayerEntryInfo_.size() > 1 )
    {
        videoAttr_->temporalLayerStream_ = true;
    }

    // Copy sequence and picture parameter sets for AVC and MVC.
    if(videoAttr_->formatID_ == _FOURCC_LE_('a','v','c','1'))
    {
        // Copy SPS and PPS for AVC or MVC base view.
        videoAttr_->sequenceParameterSet_.clear();
        appendParameterSet(trackContext->sequenceParameterSets(),
                           videoAttr_->sequenceParameterSet_);
        if(verifyAVCParameterSetNaluType(
               videoAttr_->sequenceParameterSet_, 7) == false)
        {
            Log::error(TRACE_MEDIAPLAYBACK, "Mp4ParseError: AVC sequence "
                       "parameter set has wrong type");
            return PARSE_ERROR_DATA;
        }

        videoAttr_->pictureParameterSet_.clear();
        appendParameterSet(trackContext->pictureParameterSets(),
                           videoAttr_->pictureParameterSet_);
        if(verifyAVCParameterSetNaluType(
               videoAttr_->pictureParameterSet_, 8) == false)
        {
            Log::error(TRACE_MEDIAPLAYBACK, "Mp4ParseError: AVC picture "
                       "parameter set has wrong type");
            return PARSE_ERROR_DATA;
        }

        videoAttr_->embeddedConfigData_.clear();

        // If this is MVC, copy SSPS and PPS for dependent view.
        if(trackContext->mvcSequenceParameterSets().size() > 0) {
            Context::ParserState state;
            if((state = getMVCAttrOnTrackParseEnd(trackContext))
               != PARSE_CONTINUE)
            {
                return state;
            }
        }
    } else if (videoAttr_->formatID_ == _FOURCC_LE_('h','v','c','1')) {
        // HEVC has different number for NALs. NAL numbers should not be shared with AVC
       if(trackContext->temporalLayerEntries().size()){
            NTRACE(TRACE_MEDIAPLAYBACK, "Streaming HEVC (hvc1) temporal layered stream with %zu layers",
                   trackContext->temporalLayerEntries().size());
        } else {
            NTRACE(TRACE_MEDIAPLAYBACK, "Streaming HEVC (hvc1) non-temporal layered stream");
        }

        // Copy VPS
        videoAttr_->videoParameterSet_.clear();
        appendParameterSet(trackContext->videoParameterSets(), videoAttr_->videoParameterSet_);
        if(verifyHEVCParameterSetNaluType(videoAttr_->videoParameterSet_, HEVC_NAL_UNIT_ID_VPS) == false ) {
            Log::error(TRACE_MEDIAPLAYBACK, "Mp4ParseError: HEVC video parameter set has wrong type");
            return PARSE_ERROR_DATA;
        }
        // Copy SPS
        videoAttr_->sequenceParameterSet_.clear();
        appendParameterSet(trackContext->sequenceParameterSets(), videoAttr_->sequenceParameterSet_);
        if(verifyHEVCParameterSetNaluType(videoAttr_->sequenceParameterSet_, HEVC_NAL_UNIT_ID_SPS) == false ) {
            Log::error(TRACE_MEDIAPLAYBACK, "Mp4ParseError: HEVC sequence parameter set has wrong type");
            return PARSE_ERROR_DATA;
        }
        // Copy PPS
        videoAttr_->pictureParameterSet_.clear();
        appendParameterSet(trackContext->pictureParameterSets(), videoAttr_->pictureParameterSet_);
        if(verifyHEVCParameterSetNaluType(videoAttr_->pictureParameterSet_, HEVC_NAL_UNIT_ID_PPS) == false ) {
            Log::error(TRACE_MEDIAPLAYBACK, "Mp4ParseError: HEVC picture parameter set has wrong type");
            return PARSE_ERROR_DATA;
        }
        videoAttr_->embeddedConfigData_.clear();


    } else if (videoAttr_->formatID_ == _FOURCC_LE_('h','e','v','1')) {
        // hev1 carries parameter sets in band
        if(trackContext->temporalLayerEntries().size()){
            NTRACE(TRACE_MEDIAPLAYBACK, "Streaming HEVC (hev1) temporal layered stream with %zu layers",
                   trackContext->temporalLayerEntries().size());
        } else {
            NTRACE(TRACE_MEDIAPLAYBACK, "Streaming HEVC (hev1) non-temporal layered stream");
        }
        videoAttr_->videoParameterSet_.clear();
        videoAttr_->sequenceParameterSet_.clear();
        videoAttr_->pictureParameterSet_.clear();
        videoAttr_->embeddedConfigData_.clear();
    } else {
        videoAttr_->format3D_ = esplayer::NOT_3D;
        Log::error(TRACE_MEDIAPLAYBACK, "Mp4ParseError: videoAttr_->formatID_:"
                   " 0x%x unrecognized\n", videoAttr_->formatID_);
        assert(trackContext->decoderSpecificData().size() == 0);
        return PARSE_ERROR_DATA;
    }
    return PARSE_CONTINUE;
}


Context::ParserState MovieBoxParseContext::onTrackParseEnd(
    shared_ptr<TrackContext> trackContext)
{
    Context::ParserState state;

    // Store the audio or video attributes from track context.
    if(audioAttr_)
    {
        if((state = getAudioAttrOnTrackParseEnd(trackContext))
           != PARSE_CONTINUE)
        {
            // TO DO log error.
            return state;
        }
    }
    if(videoAttr_)
    {
        if((state = getVideoAttrOnTrackParseEnd(trackContext))
           != PARSE_CONTINUE)
        {
            // TO DO log error
            return state;
        }
    }

    // Get timescale and duration info.
    ullong duration;
    uint32_t timescale;
    trackContext->getMediaTimeInfo(duration, timescale);
    if(videoAttr_)
    {
        videoAttr_->timeScale_ = timescale;
    }

    if(timescale > 0)
        timescaleConverterRatio_ = (1LL << 31) *
            DeviceConstants::DEFAULT_TIMESCALE  / timescale;

    return PARSE_CONTINUE;
}

Context::ParserState MovieBoxParseContext::onProtectionSystemHeaderFound(
    std::vector<uint8_t>& systemID,
    std::vector<uint8_t>& protectionSystemSpecificData)
{
    // check for the Netflix PSSH box
    if (memcmp(&systemID[0], &DeviceConstants::NETFLIX_SYSTEM_ID[0], 16) == 0)
    {
        if (nflxHeader_)
            (*nflxHeader_) = protectionSystemSpecificData;
    }

    if(drmSystemID_)
    {
        // Note: an implicitly assumption are made here - if the target
        //       drmSystemID_ is not NULL, it must be pointing to a
        //       16-bytes (128-bits) long UUID field.

        assert(systemID.size() == 16);

        // If the system ID of this ProtectionSystemHeader does not match
        // the target system ID, skip this header and look for the next one.

        if(memcmp(&systemID[0], drmSystemID_, 16) != 0)
            return PARSE_CONTINUE;
    }

    if(protectionSystemSpecificData.size() > 0)
    {
        if(drmHeader_)
            (*drmHeader_) = protectionSystemSpecificData;

        if(audioAttr_)
            audioAttr_->encrypted_ = true;

        if(videoAttr_)
            videoAttr_->encrypted_ = true;
    }

    return PARSE_CONTINUE;
}

Context::ParserState MovieBoxParseContext::onMovieParseEnd(
    shared_ptr<MovieContext> /*movieContext*/)
{
    getDefaultDrmInfo(defaultDrmAlgorithmID_, defaultDrmIVSize_, defaultDrmKID_);
    return PARSE_STOP;
}

/**
 *
 * StreamParser
 *
 */
Mp4Demultiplexer::StreamParser::StreamParser(ullong& syncPTS,
                                             bool& syncPTSValidFlag,
                                             IConsumerListView<DataBlock>& list,
                                             esplayer::MediaType mediaType,
                                             esplayer::Format3D video3DFormat,
                                             const std::vector<ContentProfile>& videoProfiles,
                                             const bool supportSeamlessFramerateSwitching)
  : movieFragmentParseContext_(trackFragmentContext_),
    fragmentParseState_(SKIP_UNTIL_NEXT_FRAGMENT),
    syncMode_(NOP),
    endOfStream_(false),
    endOfStreamLogged_(false),
    discontinuity_(true),
    firstSample_(true),
    isStartOfFragment_(false),
    sampleWritePending_(false),
    syncPTS_(syncPTS),
    syncPTSIsValid_(syncPTSValidFlag),
    currentGopTimeOffset_(-1),
    ctsOffsetOfFirstSampleInGop_(0),
    blockList_(list),
    blockListCache_(new BlockListCache< IConsumerListView<DataBlock> >(blockList_)),
    currentOffsetFromMoof_(0),
    mediaType_(mediaType),
    format3D_(video3DFormat),
    level_idc_(0),
    videoProfiles_(videoProfiles),
    supportSeamlessFramerateSwitching_(supportSeamlessFramerateSwitching),
    HEVC_Level_5_1_Supported_(false),
    HEVC_Level_5_0_Supported_(false),
    currBitDepth_(0)
{
    mp4Demux_.reset();
    vector<ContentProfile>::iterator iter;
    for (iter = videoProfiles_.begin(); iter != videoProfiles_.end(); iter ++)
    {
        if( *iter == HEVC_MAIN_L50_DASH_CENC || *iter == HEVC_MAIN10_L50_DASH_CENC )
            HEVC_Level_5_0_Supported_ = true;
        if( *iter == HEVC_MAIN_L51_DASH_CENC || *iter == HEVC_MAIN10_L51_DASH_CENC )
            HEVC_Level_5_1_Supported_ = true;
    }
}

// Counts the total number of bytes in the list of blocks.
uint32_t Mp4Demultiplexer::StreamParser::getNumBytesAvailable(IConsumerListView<DataBlock>::iterator start,
                                                              IConsumerListView<DataBlock>::iterator end)
{

    uint32_t blockBytesAvailable = 0;
    IConsumerListView<DataBlock>::iterator it;
    for(it = start; it != end; ++it) {
        blockBytesAvailable += it->getDataSize();
    }
    return blockBytesAvailable;
}

uint32_t Mp4Demultiplexer::StreamParser::getInt(unsigned char* buff, const uint32_t numBytes)
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
bool Mp4Demultiplexer::StreamParser::getNaluSize(IConsumerListView<DataBlock>::iterator startPos,
                                                 IConsumerListView<DataBlock>::iterator endPos,
                                                 const int offset,
                                                 uint32_t& size,
                                                 uint8_t& naluFirstByte)
{
    size = 0;
    assert(NALULengthSize_ != 0);
#if defined(STREAM_ERROR_DEBUG)
    if(mediaType_ == esplayer::VIDEO) {
        printf("%s: startPos->getData() %u, offset for next Nal %d\n", __func__, startPos->getData(), offset);
    }
#endif
    if((startPos->getDataSize()-offset) >= NALULengthSize_) {

        // size of NAL unit including size bytes
        size = getInt(startPos->getData()+offset, NALULengthSize_) + NALULengthSize_;

        // Get the first byte of the nalu.  If it's not in the same block,
        // it starts the next block.
        if((startPos->getDataSize()-offset) > NALULengthSize_)
        {
            naluFirstByte = *(startPos->getData()+offset+NALULengthSize_);
        } else {
            IConsumerListView<DataBlock>::iterator pos = startPos;
            ++pos;
            if(pos != endPos)
            {
                naluFirstByte = *(pos->getData());
            } else {
                Log::error(TRACE_MEDIAPLAYBACK, "Error in getNaluSize."
                           " pos == endPos. 4-byte Nalu size: %u", size);
                return false;
            }
        }
    } else {
        // buff is to hold size data
        unsigned char buff[NALULengthSize_];
        uint32_t i, numCopied;
        for(i = 0; i < (startPos->getDataSize()-offset); i++) {
            buff[i] = startPos->getData()[i+offset];
        }
        numCopied = startPos->getDataSize() - offset;
        assert(numCopied < NALULengthSize_);
        ++startPos;
        if(startPos != endPos) {
            for(i = numCopied; i < NALULengthSize_; i++) {
                buff[i] = startPos->getData()[i-numCopied];
            }
            size = getInt(buff, NALULengthSize_) + NALULengthSize_;
            naluFirstByte =  startPos->getData()[NALULengthSize_-numCopied];
        } else {
            Log::error(TRACE_MEDIAPLAYBACK, "Error in getNaluSize."
                       " startPos == endPos. Block list contains too few bytes to read NALU size");
            return false; // not enough bytes to read size.
        }
    }
    return true;
}

/**
 *
 * This is only for HEVC
 *
 */
bool Mp4Demultiplexer::StreamParser::getHEVCLevelIdcFromSPS(const vector<unsigned char> sequenceParameterSet,
                                                            uint32_t& level_idc )
{
    uint32_t level_idc_byte_pos = 14; // 14 = 2(nal header) + 1(SPS 1st) + 11(profile_tier)
    uint8_t  read_byte = 0;
    uint8_t  zero_read = 0;
    uint32_t i = 0;
    uint32_t size = sequenceParameterSet.size();

    do{
        read_byte = sequenceParameterSet[NALULengthSize_ + i++];
        if(read_byte == 0) {
            zero_read++;
        } else {
            if( (zero_read == 2) && (read_byte == 0x03) ){
                level_idc_byte_pos += 1;
                zero_read = 0;
            } else if ( (zero_read == 2) && (read_byte != 0x03) ) {
                zero_read = 0;
            }
        }

        if( i == level_idc_byte_pos){
            level_idc = sequenceParameterSet[NALULengthSize_ + level_idc_byte_pos];
            return true;
        }
    }while(size > i);
    return false;
}

bool Mp4Demultiplexer::StreamParser::getHEVCLevelIdcFromNalu(IConsumerListView<DataBlock>::iterator startPos,
                                                             IConsumerListView<DataBlock>::iterator endPos,
                                                             const uint32_t size,
                                                             const int offset,
                                                             uint32_t& level_idc )
{
    uint32_t profile_idc_byte_pos = 3; // 3 = 2(nal header) + 1(SPS 1st)
    uint32_t level_idc_byte_pos = 14; // 14 = 2(nal header) + 1(SPS 1st) + 11(profile_tier)
    uint8_t  read_byte = 0;
    uint8_t  zero_read = 0;
    uint32_t i = 0;
    uint32_t j = 0;

    do{
        read_byte = *(startPos->getData() + offset + NALULengthSize_ + i++);
        if(read_byte == 0) {
            zero_read++;
        } else {
            if( (zero_read == 2) && (read_byte == 0x03) ){
                level_idc_byte_pos += 1;
                zero_read = 0;
            } else if ( (zero_read == 2) && (read_byte != 0x03) ) {
                zero_read = 0;
            }
        }

        if( i == profile_idc_byte_pos ) {
            uint8_t profile_info = *(startPos->getData() + offset + NALULengthSize_ + profile_idc_byte_pos);
            (void)profile_info;

            NTRACE(TRACE_MEDIAPLAYBACK, "HEVC profile_space %d, tier %d, profile_idc %d",
                   ((profile_info >> 6) & 0x03) ,
                   ((profile_info >> 5) & 0x01) ,
                   ( profile_info & 0x1f));

        }
        if( i == level_idc_byte_pos){
            level_idc = *(startPos->getData() + offset + NALULengthSize_ + level_idc_byte_pos);
            NTRACE(TRACE_MEDIAPLAYBACK, "HEVC 0x%x", level_idc);
            return true;
        }
    }while( (startPos->getDataSize()-offset - NALULengthSize_ - i) > 0 );

    ++startPos;
    if(startPos == endPos) {
        Log::error(TRACE_MEDIAPLAYBACK, "Error in getLevelIdcFromNalu."
                   " startPos == endPos. Block list contains too few bytes to read SPS");
        return false; // not enough bytes to read size.
    }

    // find data including level_idc from next block
    do{
        read_byte = *(startPos->getData() + j++);
        if(read_byte == 0) {
            zero_read++;
        } else {
            if( (zero_read == 2) && (read_byte == 0x03) ){
                level_idc_byte_pos += 1;
                zero_read = 0;
            } else if ( (zero_read == 2) && (read_byte != 0x03) ) {
                zero_read = 0;
            }
        }

        if( (j + i) == level_idc_byte_pos){
            level_idc = *(startPos->getData()+j);
            return true;
        }
    }while( size > (j + i) );

    return false;
}

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
bool Mp4Demultiplexer::StreamParser::getNextNaluInfo(IConsumerListView<DataBlock>::iterator& startPos,
                                                     IConsumerListView<DataBlock>::iterator endPos,
                                                     const VideoAttributes* videoAttr,
                                                     uint32_t& offset,
                                                     NaluInfo& naluInfo,
                                                     uint32_t& level_idc_out )
{
    // Iterator startIt points to the block where the nalu
    // begins. The offset shows where the nalu begins relative to
    // the start of the block.
    if(getNaluSize(startPos, endPos, offset,
                   naluInfo.size_,
                   naluInfo.firstByte_) == false) {
        return false;
    }

#if defined(STREAM_ERROR_DEBUG)
    printf("naluInfo.size %d\n", naluInfo.size_);
#endif

    // HEVC nal unit type check
    if(videoAttr->formatID_ == _FOURCC_LE_('h','e','v','1') ||
       videoAttr->formatID_ == _FOURCC_LE_('h','v','c','1') ) {
        // HEVC bit depth change server report. In same title, no bit depth
        // change are expected. If bit depth change happens, it must be server
        // side error. We inform server of wrong stream.

        if ( (currBitDepth_ != 0) && (currBitDepth_ != videoAttr->bitsDepthLuma_) ){
            // if first stream with valid bit depth was playing, but stream's
            // bit depth has been changed since then, it means there is some
            // server issue. We need to report this event to server
            Log::warn(TRACE_MEDIAPLAYBACK,
                      "HEVC stream bit depth has changed from %d to %d. There must be wrong stream in CDN",
                      currBitDepth_, videoAttr->bitsDepthLuma_);
        }
        currBitDepth_ = videoAttr->bitsDepthLuma_;

        // HEVC stream level change log
        uint8_t old_level_idc = level_idc_out;

        if ( videoAttr->temporalLayerEntryInfo_.size() > 1 ) {
            // when temporal layered stream
            uint32_t upperLayerLevelIdc = 0;
            std::vector<TemporalLayerEntryInfo>::const_iterator iter;
            for(iter  = videoAttr->temporalLayerEntryInfo_.begin();
                iter != videoAttr->temporalLayerEntryInfo_.end();
                iter++)
            {
                if (iter->tlLevelIdc_ > upperLayerLevelIdc ){
                    upperLayerLevelIdc = iter->tlLevelIdc_;
                }
            }
            level_idc_out = upperLayerLevelIdc;

        } else {
            // when SFR stream
            level_idc_out = videoAttr->generalLevelIdc_;
        }

        // Log when level changed
        if( old_level_idc != level_idc_out){
            if (!videoAttr->temporalLayerStream_){
                NTRACE(TRACE_MEDIAPLAYBACK,
                       "Playing a non-temporal-layered HEVC stream with new SPS : general_level_idc %d",
                       level_idc_out);
            } else {
                NTRACE(TRACE_MEDIAPLAYBACK,
                       "Playing a temporal-layered HEVC stream with new SPS :"
                       "general_level_idc %d, upper temporal layer level_idc %d",
                       videoAttr->generalLevelIdc_,
                       level_idc_out);

                // for the devices that support high frame rate
                if( HEVC_Level_5_1_Supported_){
                    NTRACE(TRACE_MEDIAPLAYBACK,
                           "Device support HEVC Level 5.1 : no temporal layer filtering will be performed");
                }
                // for the devices that does not support high frame rate
                if( !HEVC_Level_5_1_Supported_ && HEVC_Level_5_0_Supported_ ) {
                    if( supportSeamlessFramerateSwitching_){
                        // if seamless framerate switching is supported, we dropp upper
                        // temporal layer only for streams with above 5.0 level
                        NTRACE(TRACE_MEDIAPLAYBACK,
                               "Device does not support HEVC Level 5.1, but supports seamless framerate switching");
                        if( level_idc_out > 150){
                            NTRACE(TRACE_MEDIAPLAYBACK,
                                   "stream level is 5.1 and above. High framerate temporal layer at 4K will be filtered");
                        } else {
                            NTRACE(TRACE_MEDIAPLAYBACK,
                                   "stream level is 5.0 and below. No temporal layer filtering will be performed");
                        }
                    } else {
                        NTRACE(TRACE_MEDIAPLAYBACK,
                               "Device does not support HEVC Level 5.1, and also does not support seamless framerate switching : high framerate temporal layer at all resolution will be filtered");
                    }
                }

            }
        }
    }

    // Loop through the data blocks collecting info about all
    // the blocks needed to complete the NALU.
    NaluBlockPosInfo blockInfo;
    uint32_t blockBytesRemaining = startPos->getDataSize() - offset;
    blockInfo.block_ = &(*startPos);
    blockInfo.startOffset_ = offset;
    while((blockBytesRemaining < naluInfo.size_ )) {
        blockInfo.endOffset_ = startPos->getDataSize();
        naluInfo.blockPositions_.push_back(blockInfo);
        ++startPos;
        if(startPos == endPos) {
            Log::error(TRACE_MEDIAPLAYBACK, "Error in getNextNaluInfo. startPos == endPos. "
                       "Block list contains too few bytes for NALU. Nalu size: %u.",
                       naluInfo.size_);

            // this should not happen. When it happens, it is either from bug in
            // this file, or from stream corruption at network
            return false;
        }
        blockBytesRemaining += startPos->getDataSize();
        blockInfo.startOffset_ = 0;
        blockInfo.block_ = &(*startPos);
    }

    // If we get here, the nalu ends in the block where startIt is
    // currently pointing. Calculate the end offset in the
    // block. That's also the starting offset for the next nalu.
    offset = startPos->getDataSize() - (blockBytesRemaining - naluInfo.size_);
    blockInfo.endOffset_ = offset;
    naluInfo.blockPositions_.push_back(blockInfo);

    // If the offset is the end of the block, the next nalu begins
    // in the next block. Increment the startIt and set the offset
    // to 0.
    if(offset == startPos->getDataSize()) {
        ++startPos;
        offset = 0;
    }
    return true;
}

// Builds a vector of naluInfos for the nalus that make up the sample
// starting at the front of the blockList_. When this returns, the nalu
// infos specify which portions of which blocks contain the NALU.  The
// method also examines the NALUs' ref_idc bits to determine whether the AU
// it is used for reference. It sets the AUInfo's isUsedForReference flag to
// true if any NALU's ref_idc bits are set.
//
// If this returns bad, there's something wrong with the encoding.  We
// should skip until the next fragment.
// @param1[in] offset: bytes from the start of current blockList_.begin() where new AU start.
//                     if offset is not 0, it means that sample.startOffset_ is bigger than
//                     currentOffsetFromMoof_.
//                     This happens with download optimized temporal layered stream
NFErr Mp4Demultiplexer::StreamParser::getAUInfo(uint32_t offset,uint32_t auSize, const VideoAttributes* videoAttr)
{
#if defined(STREAM_ERROR_DEBUG)
    const TrackFragmentContext::SampleEntry& sample = (trackFragmentContext_->sampleEntries())[currentSampleEntryIndex_];
    printf("\ngetAUInfo: offset %u auSize %u sample.startOffset %llu, curOffsetFromMoof %u --------------------\n"
       , offset, auSize, sample.startOffset_, currentOffsetFromMoof_);
#endif

    uint32_t sumNaluSizes = 0;
    bool isUsedForReference = false;

    // Advance pos to the correct position in blockList_. This happens
    // when temporal layered stream is fed. layer 2's AU is behind all
    // layer 1's AUs, so, startOffset_ of layer 2 AU is very big while
    // currentOffsetFromMoof_, which indicates blockList's first data,
    // is small. Therefore, pos will be incremented until it finds
    // layer 2's AU.
    IConsumerListView<DataBlock>::iterator pos = blockList_.begin();
    for( ; offset > 0 && pos != blockList_.end(); ++pos )
    {
        if ( offset < pos->getDataSize() )
        break;

        offset -= pos->getDataSize();
    }

    if ( pos == blockList_.end() )
    {
        Log::error(TRACE_MEDIAPLAYBACK, "Inconsistent stream: Mp4Parser::"
                           "getAUInfo: called with insufficient data for offset %u, auSize %u, currentOffset %u", offset, auSize, currentOffsetFromMoof_ );
        return NFErr_Bad;
    }

    au_.reset();
    if(auSize > 0)
    {
        //  Iterate through the NALUs in the AU. Find nalu sizes
        //  and find where tha NALUs lie on the data blocks.
        sumNaluSizes = 0;
        while(sumNaluSizes < auSize) {
            au_.nalus_.resize(au_.nalus_.size() + 1);
            if(getNextNaluInfo(pos, blockList_.end(),
                               videoAttr, offset,  au_.nalus_.back(), level_idc_) == false) {
                // If getNextNaluInfo returns false, it means that according
                // to the nalu size bytes that begin the nalu, the nalu is
                // longer than the number of bytes remaining in our block
                // list.  Since this method only gets called when the AU
                // size as specified in the Mp4 trun sampleSize is less than
                // the number of bytes remaining in the block list, there
                // has either been an error in the sample size as parsed
                // from the trun, or there is an error in the nalu size
                // bytes.
                Log::error(TRACE_MEDIAPLAYBACK, "Inconsistent stream: Mp4Parser::"
                           "getAUInfo: getNextNaluInfo returns false. Sum nalu "
                           "sizes: %u, au size %u", sumNaluSizes, auSize);
                return NFErr_Bad;
            }
            sumNaluSizes +=  au_.nalus_.back().size_;

            if(videoAttr->formatID_ == _FOURCC_LE_('h','e','v','1') ||
               videoAttr->formatID_ == _FOURCC_LE_('h','v','c','1') ) {
                uint8_t naluType = (au_.nalus_.back().firstByte_ & 0x7e ) >> 1;
                if( (naluType <= 14) && (naluType%2 == 0) ){
                    // HEVC non-reference type. If a picture has nal_unit_type
                    // equal to TRAIL_N, TSA_N, STSA_N, RADL_N, RASL_N,
                    // RSV_VCL_N10,RSV_VCL_N12, or RSV_VCL_N14, the picture is a
                    // sub-layer non-reference picture. Otherwise, the picture
                    // is a sub-layer reference picture.
                    isUsedForReference = false;
                } else {
                    isUsedForReference = true;
                }
            } else {
                // The nalu's ref_idc bits indicate whether the nalu is used for
                // reference. As defined in 14496-10:2010, ref_idc bits are the
                // second and third bits in the first byte. If they are
                // non-zero, the nalu is used for reference.
                if((au_.nalus_.back().firstByte_ & 0x60) != 0)
                {
                    isUsedForReference = true;
                }
            }
        }
        if(sumNaluSizes != auSize)
        {
            Log::error(TRACE_MEDIAPLAYBACK, "Inconsistent stream: Mp4Parser::"
                       "getAUInfo: sumNaluSizes %u != auSize %d.",
                       sumNaluSizes, auSize);
            return NFErr_Bad;
        }
        au_.isUsedForReference_ = isUsedForReference;
    }
    return NFErr_OK;
}

////
// Looks at the MP4 fragment's sample to group box to determine
// which sample group description of type scnm (scalable NALU map
// entry) specifies, for this AU, the views that each NALU belongs
// to.  Finds the view index mapping for each NALU and then
// enqueues the NALU in the AU's ordered vector of nalus for that
// view.
//
NFErr Mp4Demultiplexer::StreamParser::addNaluToViewGroup(NaluInfo& nalu, uint32_t naluOrderNum,
                                                         const MVCAttributes* mvcAttr)
{
    TrackFragmentContext::SampleEntry& sampleEntry =
        trackFragmentContext_->sampleEntries()[currentSampleEntryIndex_];
    assert(mvcAttr);

    if(mvcAttr->naluToMvcViewMaps_.size() <= sampleEntry.sampleGroup_ ||
       mvcAttr->naluToMvcViewMaps_[sampleEntry.sampleGroup_].size() <= naluOrderNum) {
        assert(0);

        NTRACE(TRACE_MEDIAPLAYBACK,
               "MVC reordering: sample group %d. Num NALUs in group %zu. Num NALUs counted %d\n",
               sampleEntry.sampleGroup_,
               mvcAttr->naluToMvcViewMaps_.size() > sampleEntry.sampleGroup_ ?
               mvcAttr->naluToMvcViewMaps_[sampleEntry.sampleGroup_].size() : -1,
               naluOrderNum);

        return NFErr_MP4ParsedData;
    }

    uint8_t viewID = mvcAttr->naluToMvcViewMaps_[sampleEntry.sampleGroup_][naluOrderNum];
    if(viewID > 1) { // for stereo, we should have only 2 views, 0 and 1.
        return NFErr_MP4ParsedData;
    }

    if(au_.nalusGroupedByView_.size() <= viewID) {
        au_.nalusGroupedByView_.resize(viewID+1);
    }
    au_.nalusGroupedByView_[viewID].push_back(&nalu);

    if(au_.viewSize_.size() <= viewID) {
        au_.viewSize_.resize(viewID+1);
        au_.viewSize_[viewID] = 0;
    }
    au_.viewSize_[viewID] += nalu.size_;
    return NFErr_OK;
}

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
NFErr Mp4Demultiplexer::StreamParser::computeNaluReordering(const VideoAttributes* videoAttr)
{
    vector<NaluInfo>::iterator naluIt;
    uint32_t naluCount = 0;

    if(videoAttr->encrypted_ == false)
    {
        for(naluIt = au_.nalus_.begin(); naluIt != au_.nalus_.end(); naluIt++)
        {
            NFErr err;
            if((err = addNaluToViewGroup(*naluIt, naluCount++,
                                         videoAttr->mvcAttributes_)) != NFErr_OK)

            {
                return err;
            }
        }
    } else
    {
        TrackFragmentContext::DrmInfoEntry& drmInfo =
            trackFragmentContext_->drmInfoEntries()[currentSampleEntryIndex_];
        TrackFragmentContext::EncryptionMapEntry& encMapEntry =
            trackFragmentContext_->encryptionMapEntries()[drmInfo.encryptionMapStartIndex_];

        au_.encryptionMapEntriesUsed_ = drmInfo.encryptionMapEntriesUsed_;
        uint32_t encMapEntryIndex = drmInfo.encryptionMapStartIndex_;
        uint32_t numClearBytes = encMapEntry.bytesOfClearData_;
        uint32_t totalNumEncryptedBytes = 0;

        // Each nalu maps to at most one encryption map entry.
        for(naluIt = au_.nalus_.begin(); naluIt != au_.nalus_.end(); naluIt++)
        {
            if(encMapEntryIndex >= (au_.encryptionMapEntriesUsed_ +
                                    drmInfo.encryptionMapStartIndex_)) {
                naluIt->numEncryptedBytes_ = 0;
                naluIt->numLeadingClearBytes_ = naluIt->size_;
            } else
            {
                if(naluIt->size_ < numClearBytes)
                {
                    // The whole nalu is in the clear.
                    numClearBytes -= naluIt->size_;
                    naluIt->numLeadingClearBytes_ = naluIt->size_;
                } else
                {
                    naluIt->numLeadingClearBytes_ = numClearBytes;
                    if(encMapEntry.bytesOfEncryptedData_ >
                       (naluIt->size_ - numClearBytes))
                    {
                        Log::warn(TRACE_MEDIAPLAYBACK, "Subsample decryption segment"
                                  " spans MVC NALU boundary\n");
                        return NFErr_Bad; // A decryption segment should
                        // not span a nalu boundary.
                    }
                    naluIt->encByteOffset_ = totalNumEncryptedBytes;
                    naluIt->numEncryptedBytes_ = encMapEntry.bytesOfEncryptedData_;
                    totalNumEncryptedBytes += encMapEntry.bytesOfEncryptedData_;

                    // We're done with this encryption entry.  Get the next one.
                    if(++encMapEntryIndex < (au_.encryptionMapEntriesUsed_ +
                                             drmInfo.encryptionMapStartIndex_))
                    {
                        encMapEntry = trackFragmentContext_->
                            encryptionMapEntries()[encMapEntryIndex];
                        numClearBytes = encMapEntry.bytesOfClearData_;

                        // Some clear bytes may be used at the end of this
                        // NALU for alignment.
                        uint32_t naluBytesUsed = naluIt->numLeadingClearBytes_ +
                            naluIt->numEncryptedBytes_;
                        if( naluIt->size_ > naluBytesUsed)
                        {
                            if(numClearBytes < (naluIt->size_ - naluBytesUsed))
                            {
                                Log::warn(TRACE_MEDIAPLAYBACK, "Subsample decryption clear"
                                          " segment does not reach MVC NALU boundary\n");
                                return NFErr_Bad;
                            }
                            numClearBytes -= (naluIt->size_ - naluBytesUsed);
                        }
                    }
                }
            }
            NFErr err;
            if((err = addNaluToViewGroup(*naluIt, naluCount++,
                                         videoAttr->mvcAttributes_)) != NFErr_OK) {
                return err;
            }
        }
    }
    return NFErr_OK;
}

esplayer::Status Mp4Demultiplexer::StreamParser::writeNalu(const NaluInfo& nalu,
                                                           esplayer::ISampleWriter& writer,
                                                           uint32_t viewNum)
{
    static const uint8_t startcode[4] = {0,0,0,1};

    uint32_t writeSize;
    vector<NaluBlockPosInfo>::const_iterator blockPosIt;

#if defined( POPULATE_START_CODE )
    // Write start code
    if(writer.write(startcode, 4, viewNum) != 4)
    {
        return esplayer::ERROR;
    }

    // Skip the first four bytes in the NALU since they are the size bytes.
    blockPosIt = nalu.blockPositions_.begin();
    if(naluBytesInBlock(blockPosIt) > 4)
    {
        // Skip 4 bytes and write the remaining nalu bytes from this block.
        writeSize = naluBytesInBlock(blockPosIt) - 4;
        if(writer.write(blockPosIt->block_->getData() + blockPosIt->startOffset_ + 4,
                        writeSize, viewNum) != writeSize)
        {
            return esplayer::ERROR;
        }
    } else if (naluBytesInBlock(blockPosIt) < 4) {

        // Skip to the next block, skip over the size field bytes that
        // extend into it, and then write the remaining nalu bytes from the
        // second block.
        uint32_t offsetIn2ndBlock  = 4 - naluBytesInBlock(blockPosIt);
        ++blockPosIt; // go on to the next block.
        writeSize = naluBytesInBlock(blockPosIt) - offsetIn2ndBlock;
        if(writer.write(blockPosIt->block_->getData() + offsetIn2ndBlock,
                        writeSize, viewNum) != writeSize)
        {
            return esplayer::ERROR;
        }
    }
#else
    // Copy the size bytes also
    blockPosIt = nalu.blockPositions_.begin();
    if(naluBytesInBlock(blockPosIt) > 4)
    {
        writeSize = naluBytesInBlock(blockPosIt);
        if(writer.write(blockPosIt->block_->getData() + blockPosIt->startOffset_ ,
                        writeSize, viewNum) != writeSize)
        {
            return esplayer::ERROR;
        }
    } else if (naluBytesInBlock(blockPosIt) < 4) {

        // Skip to the next block, skip over the size field bytes that
        // extend into it, and then write the remaining nalu bytes from the
        // second block.
        uint32_t offsetIn2ndBlock  = 4 - naluBytesInBlock(blockPosIt);
        ++blockPosIt; // go on to the next block.
        writeSize = naluBytesInBlock(blockPosIt) - offsetIn2ndBlock;
        if(writer.write(blockPosIt->block_->getData() + offsetIn2ndBlock,
                        writeSize, viewNum) != writeSize)
        {
            return esplayer::ERROR;
        }
    }
#endif

    // Write out the remainder of the nalu.
    for(++blockPosIt; blockPosIt != nalu.blockPositions_.end(); ++blockPosIt)
    {
        if(writer.write(blockPosIt->block_->getData(),
                        naluBytesInBlock(blockPosIt), viewNum) !=
           naluBytesInBlock(blockPosIt))
        {
            return esplayer::ERROR;
        }
    }
    return esplayer::OK;
}

esplayer::Status Mp4Demultiplexer::StreamParser::writeAU(esplayer::ISampleWriter& writer)
{
    vector<struct NaluInfo>::const_iterator naluIt;
#if defined(STREAM_ERROR_DEBUG)
    printf("%s enter\n", __func__);
#endif
    for(naluIt = au_.nalus_.begin(); naluIt != au_.nalus_.end(); ++naluIt)
    {
        if(writeNalu(*naluIt, writer) != esplayer::OK)
        {
            return esplayer::ERROR;
        }
    }
#if defined(STREAM_ERROR_DEBUG)
    printf("%s exit\n", __func__);
#endif
    return esplayer::OK;
}

esplayer::Status  Mp4Demultiplexer::StreamParser::writeMvcSplitAU(esplayer::ISampleWriter& writer)
{
    vector<NaluInfo*>::const_iterator naluIt;
    vector<NaluBlockPosInfo>::const_iterator blockPosIt;

    // Write the nalus to the ring buffer grouped by view.
    for(uint32_t viewID = 0; viewID < au_.nalusGroupedByView_.size(); ++viewID)
    {
        for(naluIt = au_.nalusGroupedByView_[viewID].begin();
            naluIt != au_.nalusGroupedByView_[viewID].end();  ++naluIt)
        {
            if(writeNalu(**naluIt, writer, viewID) != esplayer::OK)
            {
                return esplayer::ERROR;
            }
        } // for each nalu in this view
    } // for each view.

    return esplayer::OK;
}

void Mp4Demultiplexer::StreamParser::parseNewFragment()
{
    NFErr err;
    ullong fragHeaderSize;
    DataBlockStream blockStream(blockList_.begin(),
                                blockList_.end(),
                                blockListCache_->getNumBytesAvailable());

    bool moofFound =  movieFragmentParser_.getMovieFragmentSize(blockStream,
                                                                fragHeaderSize);

    if(!moofFound) {
        NTRACE(TRACE_MEDIAPLAYBACK, "StreamParser::parseNewFragment: type: %s, "
               "MP4 fragment does not begin with moof.",
               mediaType_== esplayer::AUDIO? "AUDIO" : "VIDEO");
        fragmentParseState_ = SKIP_UNTIL_NEXT_FRAGMENT;
        currentGopTimeOffset_ = blockList_.begin()->getDataBuffer().timestamp_;
        return;
    }

    if(fragHeaderSize > blockStream.getNumBytesAvailable()) {
        fragmentParseState_ = NOT_ENOUGH_DATA;
        return;
    }
    IConsumerListView<DataBlock>::iterator front = blockList_.begin();
    blockStream.reset();
    movieFragmentParser_.reset();
    movieFragmentParseContext_.reset();
    movieFragmentParseContext_.setDefaultDrmInfo(
        front->getStreamAttributes()->defaultDrmAlgorithmID_,
        front->getStreamAttributes()->defaultDrmIVSize_,
        front->getStreamAttributes()->defaultDrmKID_ );

    movieFragmentParser_.
        parseMovieFragment(movieFragmentParseContext_,
                           blockStream);

    if(movieFragmentParseContext_.parserState() != Context::PARSE_STOP ||
       trackFragmentContext_->sampleEntries().size() == 0)
    {
        fragmentParseState_ = SKIP_UNTIL_NEXT_FRAGMENT;
        currentGopTimeOffset_ = front->getDataBuffer().timestamp_;
    } else
    {
        fragmentParseState_ = NEW_FRAGMENT;
        currentSampleEntryIndex_ = 0;
        currentGopTimeOffset_ = front->getDataBuffer().timestamp_;
        timescaleConverterRatio_ = front->getStreamAttributes()->timescaleConverterRatio_;
    }
}

void Mp4Demultiplexer::StreamParser::popBytesFromDataBlockList(uint32_t bytesToRemove)
{
#if defined(STREAM_ERROR_DEBUG)
    if(mediaType_ == esplayer::VIDEO)
    {
        printf("%s entering: getData() %u\n", __func__, blockList_.begin()->getData());
        TrackFragmentContext::SampleEntry const& sample
            = (trackFragmentContext_->sampleEntries())[currentSampleEntryIndex_];
        uint32_t const layer = sample.temporalLayerId_;
        uint32_t const sampleSize = sample.sampleSize_;
        uint64_t const sampleOffset = sample.startOffset_;
        uint32_t const lowestSubsequentOffset = sample.lowestSubsequentOffset_;

        printf("%s: layer %d, ssize %5d, sOffset %5llu, LSubsOffset %5d, currOffsetFromMoof %5d,"
           " bytesToRemove %5d, blockSize %5d\n",
               __func__, layer, sampleSize, sampleOffset, lowestSubsequentOffset, currentOffsetFromMoof_,
           bytesToRemove, blockList_.begin()->getDataSize());
    }
#endif

    currentOffsetFromMoof_ += blockListCache_->consumeBytes(bytesToRemove);

#if defined(STREAM_ERROR_DEBUG)
    if(mediaType_ == esplayer::VIDEO) {
        printf("%s exiting: getData() %u\n", __func__, blockList_.begin()->getData());
    }
#endif

    return;
}



// Searches for a fragment header in the list of blocks starting from
// startBlock_ and attempts to parse.  If it succeeds in parsing, it
// returns.  If parsing fails, searches for the next fragment header.  Keeps
// searching til it runs out of blocks or succeeds.
//
// Side effects, updateds startBlock_ to the block containing the end of the
// fragment header, if one is successfully parsed. Otherwise, returns with
// startBlock_ = lEndBlock.
bool Mp4Demultiplexer::StreamParser::findNextFragmentHeaderAndParse()
{
    while(fragmentParsingIsNeeded())
    {
        if(!blockListStartsWithUnparsedFragment())
        {
            blockListCache_->popFront();

            if(blockList_.begin() == blockList_.end())
            {
                // Never found the beginning of a new fragment.
                assert(fragmentParseState_ == SKIP_UNTIL_NEXT_FRAGMENT);
                return false;
            }
        } else {
            parseNewFragment();
            if(fragmentParseState_ == NOT_ENOUGH_DATA)
            {
                return false;
            }
            // Prevent the fragment header in this block from being parsed
            // again.  In the case of audio, especially we may also be reading
            // several samples out of this block.
            blockList_.begin()->setIsParsed();
        }
    }

    // If we get here we either parsed a fragment successfully or there was
    // not enough data.
    if(fragmentParseState_ == NEW_FRAGMENT)
    {
#if defined(STREAM_ERROR_DEBUG)
        if(mediaType_ == esplayer::VIDEO){
            printf("================================ new fragment ====================================\n");
        }
#endif
        isStartOfFragment_ = true;
        currentOffsetFromMoof_ = 0;
        popBytesFromDataBlockList(trackFragmentContext_->sampleEntries()[0].startOffset_);

        return true;
    }

    assert(0);
    return false;
}

// We don't want to deliver an audio sample with a time stamp that precedes
// the first video sample.  The audio parser is initialized with sync mode
// SYNC_TO_PTS and is set to SYNC_TO_PTS at every flush.  The video parser
// is set to SEARCH_SYNC_PTS, on the other hand. syncPTS_ is a shared
// reference between both parsers as is syncPTSIsValid_. Once the video
// stream (set to SEARCH_SYNC_PTS) delivers a sample, it sets the syncPTS_
// and makes it valid.
bool Mp4Demultiplexer::StreamParser::sampleShouldBeSkipped()
{
    TrackFragmentContext::SampleEntry& sample =
        (trackFragmentContext_->sampleEntries())[currentSampleEntryIndex_];

    if(syncMode_ == SYNC_TO_PTS)
    {
        // If the sync pts is not valid, we should have returned
        // NO_SAMPLES_AVAILABLE early in getNextSample and never gotten
        // here.
        assert(syncPTSIsValid_);

        ullong sampleEndPTS = currentGopTimeOffset_ +
            timescaleConvert(sample.CTS_ + sample.sampleDuration_);

        if(sampleEndPTS <= syncPTS_)
        {
            return true;
        }
        syncMode_ = NOP;
    }
    else if (syncMode_ == SEARCH_SYNC_PTS)
    {
        // While PTS values for video should take into consideration CTS,
        // this is an IDR-frame.  We do not correct PTS offsets due to
        // positive CTS using track offsets from the ELST MP4 atom as
        // recommended by DASH.  Instead, we rely on the property of CE3
        // streams that video and audio tracks start with PTS 0. The CTS
        // offset correction for the samples of a GOP should therefore be
        // the CTS value for the IDR frame. Since this is an IDR frame, we
        // can ignore the CTS value when calculating the PTS because we
        // would remove it anyway.
        assert(sample.DTS_ == 0); // Assert that this is the first sample in
        // the GOP.

        syncPTS_ =  currentGopTimeOffset_; // + timescaleConvert(sample.CTS_);
        syncPTSIsValid_ = true;
        syncMode_ = NOP;
    }
    else if (syncMode_ == SYNC_TO_CURRENT_PLAYBACK_PTS )
    {
        // This is only after audio flush is called. When audio flush is called,
        // there should always demuxer object.
        assert(mp4Demux_);

        // Audio sample will be dropped if current playback point is already
        // past the audio PTS. This can happen when audio language is switched,
        // and some past audio data is arrived from slow network while video
        // playback has already proceeded.
        ullong currentPts = mp4Demux_->getCurrentPlaybackPosition();
        ullong samplePTS = currentGopTimeOffset_ + timescaleConvert(sample.CTS_ );
        if( currentPts > samplePTS ){
            NTRACE(TRACE_MEDIAPLAYBACK,
                   "Audio Sync : skipping audio  with PTS %lld since current playback position is %lld",
                   samplePTS,
                   currentPts);

            return true;
        }
        syncMode_ = NOP;
    }

    return false;
}

bool Mp4Demultiplexer::StreamParser::temporalLayerShouldBeSkipped()
{
    // should be effective only to VIDEO
    if(mediaType_ != esplayer::VIDEO){
        return false;
    }

    TrackFragmentContext::SampleEntry& sample =
        (trackFragmentContext_->sampleEntries())[currentSampleEntryIndex_];

    // for the devices that support high frame rate
    if( HEVC_Level_5_1_Supported_)
        return false;

    // for the devices that does not support high frame rate
    if( !HEVC_Level_5_1_Supported_ && HEVC_Level_5_0_Supported_ ) {
        if( supportSeamlessFramerateSwitching_){
            // if seamless framerate switching is supported, we dropp upper
            // temporal layer only for streams with above 5.0 level
            if( level_idc_ > 150 && sample.temporalLayerId_ >= 2){
                return true;
            } else {
                return false;
            }
        } else {
            // if seamless framerate switching is not supported, we drop upper
            // temporal layer for all streams (if temporal layer exist)
            if (sample.temporalLayerId_ >= 2)
                return true;
            else
                return false;
        }
    }
    return false;
}


// If this is a key frame, we will be prepending the SPS and PPS.  If this
// is mvc, we will also be prepending dependent-view SSPS and PPS.  If we
// are in mvc split mode, the dependent view SSPS and PPS will prepended to
// the second view buffer.
//
// Similarly, nalu size entries are added for the nalus of the view
// including any parameter sets that are prepended.
void Mp4Demultiplexer::StreamParser::setVideoSampleSizeAndNaluSizeAttributes( VideoAttributes *videoAttr)
{
    const int size = (trackFragmentContext_->sampleEntries())
        [currentSampleEntryIndex_].sampleSize_;

    if(isStartOfFragment_)
    {
        assert(sampleAttributes_.mMediaAttributes.mVideoAttributes);
        vector<uint32_t>& paramSetLens =
            sampleAttributes_.mMediaAttributes.mVideoAttributes->mCodecSpecificDataLen;
        if(format3D_ == esplayer::NOT_3D)
        {
            if(paramSetLens.size() != 0 ){
                sampleAttributes_.setSize(size + paramSetLens[0]);
                sampleAttributes_.addNaluSizeEntry(videoAttr->sequenceParameterSet_.size());
                sampleAttributes_.addNaluSizeEntry(videoAttr->pictureParameterSet_.size());
            } else {
                sampleAttributes_.setSize(size);
            }
        } else if (format3D_ == esplayer::MVC_COMBINED) {

            sampleAttributes_.setSize(size + paramSetLens[0]);
            sampleAttributes_.addNaluSizeEntry(
                videoAttr->sequenceParameterSet_.size());
            sampleAttributes_.addNaluSizeEntry(
                videoAttr->mvcAttributes_->subsequenceParameterSet_.size());
            sampleAttributes_.addNaluSizeEntry(
                videoAttr->pictureParameterSet_.size());
            sampleAttributes_.addNaluSizeEntry(
                videoAttr->mvcAttributes_->pictureParameterSet_.size());
        } else if (format3D_ == esplayer::MVC_SPLIT) {
            assert(paramSetLens.size() == 2);

            sampleAttributes_.setSize(au_.viewSize_[0] + paramSetLens[0], 0);
            sampleAttributes_.setSize(au_.viewSize_[1] + paramSetLens[1], 1);

            sampleAttributes_.addNaluSizeEntry(
                videoAttr->sequenceParameterSet_.size(), 0);
            sampleAttributes_.addNaluSizeEntry(
                videoAttr->mvcAttributes_->subsequenceParameterSet_.size(), 1);
            sampleAttributes_.addNaluSizeEntry(
                videoAttr->pictureParameterSet_.size(), 0);
            sampleAttributes_.addNaluSizeEntry(
                videoAttr->mvcAttributes_->pictureParameterSet_.size(), 1);
        }
    } else {
        if(format3D_ == esplayer::MVC_SPLIT)
        {
            sampleAttributes_.setSize(au_.viewSize_[0], 0);
            sampleAttributes_.setSize(au_.viewSize_[1], 1);
        } else {
            sampleAttributes_.setSize(size);
        }
    }
    addNonParamSetNaluSizeEntries();
}

// Adds entries to the sample attributes arrays of nalu sizes. Does not add
// entries for parameter set nalus that will be prepended. Those entries
// should be added before this method is called.
void Mp4Demultiplexer::StreamParser::addNonParamSetNaluSizeEntries()
{
    uint32_t i, viewNum;

    // Set nalu sizes for param sets that might get prepended.
    if(format3D_ == esplayer::MVC_SPLIT)
    {
        assert(au_.nalusGroupedByView_.size() == 2);
        for(viewNum = 0; viewNum < au_.nalusGroupedByView_.size(); viewNum++)
        {
            for(i = 0; i < au_.nalusGroupedByView_[viewNum].size(); i++)
            {
                sampleAttributes_.
                    addNaluSizeEntry(au_.nalusGroupedByView_[viewNum][i]->size_,
                                     viewNum);
            }
        }
    } else {
        for(i = 0; i < au_.nalus_.size(); i++)
        {
            sampleAttributes_.addNaluSizeEntry(au_.nalus_[i].size_);
        }
    }
}

// See details about the sdtp flags in 14496-12:2008.
void Mp4Demultiplexer::StreamParser::setVideoSampleDependencyFlags()
{
    // Since sdtp box in CE3 is unreliable as of 9/5/2012, using info taken
    // from ref_idc flags in each nalu.
    sampleAttributes_.setIsDependedOn(au_.isUsedForReference_);

    /*
      const uint8_t flags = (trackFragmentContext_->sampleEntries())
      [currentSampleEntryIndex_].independentAndDisposableFlags_;

      // If there's no sdtp, be conservative.
      if(0 == flags)
      {
      sampleAttributes_.setIsDependedOn(true);
      } else {
      sampleAttributes_.setIsDependedOn((flags & 0x04) != 0);
      }
    */
}

bool Mp4Demultiplexer::StreamParser::setSampleVideoAttributes(uint32_t offset, uint32_t size, VideoAttributes *videoAttr)
{
    // Go through the AVC AU, get info about all the NALU sizes.
    if(getAUInfo(offset, size, videoAttr) != NFErr_OK)
    {
        return false;
    }

    if(format3D_ == esplayer::MVC_SPLIT)
    {
        if(computeNaluReordering(videoAttr)!= NFErr_OK)
        {
            return false;
        }
    }

    if(isStartOfFragment_)
    {
        sampleAttributes_.initMediaAttributes(esplayer::VIDEO);

        // By putting the timescale in copyAttributes "frameRateValue"
        // argument, and sampleDuration in the "frameRateScale" argument,
        // frameRateValue/frameRateScale = timescale/sampleDuration which
        // gives the frame rate.

        // this is to report correct frame rate for temporal layer filtering
        uint32_t factor = 1;

        // for the devices that support high frame rate
        if( HEVC_Level_5_1_Supported_)
            factor = 1;

        // for the devices that does not support high frame rate
        if( !HEVC_Level_5_1_Supported_ && HEVC_Level_5_0_Supported_ ) {
            if( supportSeamlessFramerateSwitching_){
                // if seamless framerate switching is supported, we dropp upper
                // temporal layer only for streams with 5.0 level and above
                if( level_idc_ > 150 && videoAttr->temporalLayerStream_){
                    factor = 2;
                } else {
                    factor = 1;
                }
            } else {
                // if seamless framerate switching is not supported, we drop upper
                // temporal layer for all streams (if temporal layer exist)
                if (videoAttr->temporalLayerStream_)
                    factor = 2;
                else
                    factor = 1;
            }
        }

        // construct sampleAttributes from videoAttr
        Mp4Demultiplexer::copyAttributes(videoAttr,
                                         sampleAttributes_.mMediaAttributes,
                                         format3D_);

        // since we are filtering, framerate should be divided by 2
        sampleAttributes_.mMediaAttributes.mVideoAttributes->mFrameRateValue /= factor;

        const bool sendStreamInfo = NrdConfiguration::sendStreamInfoEvents();
        if (sendStreamInfo
#ifdef NRDP_HAS_TRACING
            || true
#endif
            )
        {
            const std::string info  =
                StringFormatter::sformat("Gop-wide StreamInfo for ESPlayer: width/height %d/%d, pixelAsp %d/%d framerate value/scale %d/%d, bitDepthLuma/Chroma %d/%d\n",
                                         sampleAttributes_.mMediaAttributes.mVideoAttributes->mImageWidth,
                                         sampleAttributes_.mMediaAttributes.mVideoAttributes->mImageHeight,
                                         sampleAttributes_.mMediaAttributes.mVideoAttributes->mPixelAspectRatioX,
                                         sampleAttributes_.mMediaAttributes.mVideoAttributes->mPixelAspectRatioY,
                                         sampleAttributes_.mMediaAttributes.mVideoAttributes->mFrameRateValue,
                                         sampleAttributes_.mMediaAttributes.mVideoAttributes->mFrameRateScale,
                                         sampleAttributes_.mMediaAttributes.mVideoAttributes->mBitsDepthLuma,
                                         sampleAttributes_.mMediaAttributes.mVideoAttributes->mBitsDepthChroma);
#ifdef NRDP_HAS_TRACING
            Log::Message msg(TRACE_MEDIAPLAYBACK, Log::Trace);
            msg.m_message = info;
            msg.send();
#endif

            if ( mp4Demux_ && mp4Demux_->getAttachedEventQueue() ){
                mp4Demux_->getAttachedEventQueue()->postEvent(netflix::device::IPlaybackDevice::EVENT_STREAM_INFO, 0, 0, info);
            }
        }

        sampleAttributes_.setIsKeyFrame();
        discontinuity_ = false;
    }
    setVideoSampleSizeAndNaluSizeAttributes(videoAttr);
    setVideoSampleDependencyFlags();
    // set contentId from videoAttributes
    //sampleAttributes_.setContentId(videoAttr->contentID_);

    return true;
}

bool Mp4Demultiplexer::StreamParser::setSampleAudioAttributes( uint32_t size, const AudioAttributes *audioAttr)
{
    if(discontinuity_) {
        sampleAttributes_.initMediaAttributes(esplayer::AUDIO);
        Mp4Demultiplexer::copyAttributes(audioAttr,
                                         sampleAttributes_.mMediaAttributes);

        // If this is aac, update the ADTS header.
        if(audioAttr->formatID_ ==  0x1610) {
            if(createADTSHeader(audioAttr->codecSpecificData_) == false)
            {
                return false;
            }
        } else {
            ADTSHeader_.clear();
        }
        discontinuity_ = false;
    }
    sampleAttributes_.setIsDependedOn(false);
    if(audioAttr->formatID_ == 0x1610) {
        if(updateADTSHeaderSizeBytes(size + ADTSHeader_.size()) == false)
        {
            return false;
        }
        sampleAttributes_.setSize(size + ADTSHeader_.size());
    } else {
        sampleAttributes_.setSize(size);
    }
    return true;
}

void Mp4Demultiplexer::StreamParser::setSampleDrmAttributes(const DataBlock& block)
{
    sampleAttributes_.setAlgorithmID(trackFragmentContext_->drmAlgorithmID());
    if(trackFragmentContext_->drmAlgorithmID() == 0)
    {
        return;
    }

    TrackFragmentContext::DrmInfoEntry& drmInfo =
        trackFragmentContext_->drmInfoEntries()[currentSampleEntryIndex_];

    if(drmInfo.IVSize_ == 0)
    {
        sampleAttributes_.setIVData(NULL);
    } else {
        sampleAttributes_.setIVData(&(trackFragmentContext_->IVDataPool()[drmInfo.IVDataOffset_]));
    }
    sampleAttributes_.setIVSize(drmInfo.IVSize_);
    sampleAttributes_.setKeyID(block.getStreamAttributes()->defaultDrmKID_);

    // Copy the subsample encryption map(s) to the sample attributes
    // object. First add entries for any param set nalus that will be
    // prepended to the sample when we copy it out.
    if(mediaType_ == esplayer::VIDEO && sampleAttributes_.isKeyFrame())
    {
        addSubsampleEntriesForVideoParamSetsThatWillBePrepended();
    }
    if(format3D_ != esplayer::MVC_SPLIT)
    {
        for(uint32_t i = 0; i < drmInfo.encryptionMapEntriesUsed_; i ++)
        {
            TrackFragmentContext::EncryptionMapEntry& entry =
                trackFragmentContext_->encryptionMapEntries()
                [drmInfo.encryptionMapStartIndex_ + i];
            sampleAttributes_.addSubsampleMappingEntry(entry.bytesOfClearData_,
                                                       entry.bytesOfEncryptedData_);
        }
    } else {
        vector<NaluInfo*>::const_iterator naluIt;
        uint32_t numClearBytes;

        for(uint32_t viewNum = 0; viewNum < au_.nalusGroupedByView_.size(); viewNum++)
        {
            numClearBytes = 0;
            for(naluIt = au_.nalusGroupedByView_[viewNum].begin();
                naluIt != au_.nalusGroupedByView_[viewNum].end(); naluIt++)
            {
                if((*naluIt)->numEncryptedBytes_ == 0)
                {
                    numClearBytes+= (*naluIt)->numLeadingClearBytes_;
                    continue;
                } else
                {
                    numClearBytes+= (*naluIt)->numLeadingClearBytes_;
                    sampleAttributes_.
                        addSubsampleMappingEntry(
                            numClearBytes,(*naluIt)->numEncryptedBytes_, viewNum);
                    numClearBytes = 0;
                }
            }

            // if at the end of the loop you have some clear bytes accumulated,
            // then you need to output a subsample encryption map entry for
            // them: it could happen that you have, for example, two subsample
            // NAL units ( clear = 4, encrypted = 1234 ), ( clear = 234,
            // encrypted = 0 ).
            if( numClearBytes > 0 ) {
                sampleAttributes_.addSubsampleMappingEntry(numClearBytes, 0, viewNum);
            }
        }
    }
}

void Mp4Demultiplexer::StreamParser::setSampleTimestampAttributes(const TrackFragmentContext::SampleEntry& sample,
                                                                  const DataBlock& block)
{
    // See declaration of ctsOffsetOfFirstSampleInGop_ for more explanation.
    llong PTS = currentGopTimeOffset_
        + timescaleConvert(sample.CTS_)
        - timescaleConvert(ctsOffsetOfFirstSampleInGop_)
        + block.getStreamAttributes()->ptsOffset_;

    llong DTS = currentGopTimeOffset_
        + timescaleConvert(sample.DTS_)
        - timescaleConvert(ctsOffsetOfFirstSampleInGop_)
        + block.getStreamAttributes()->ptsOffset_;

    /*
    esplayer::MediaType mediaType  = block.getStreamAttributes()->audioAttributes_ ?
        esplayer::AUDIO : esplayer::VIDEO;
    if(mediaType != esplayer::AUDIO) {
        printf("Timestamp : pts = %lld\t = %lld\t + %lld\t - %lld\t + %lld\n",
               PTS,
               (long long)currentGopTimeOffset_ ,
               (long long)timescaleConvert(sample.CTS_),
               (long long)timescaleConvert(ctsOffsetOfFirstSampleInGop_),
               (long long)block.getStreamAttributes()->ptsOffset_);
        printf("Timestamp : dts = %lld\t = %lld\t + %lld\t + %lld\n",
               DTS,
               (long long)currentGopTimeOffset_,
               (long long)timescaleConvert(sample.DTS_),
               (long long)block.getStreamAttributes()->ptsOffset_);
        printf("Timestamp : -----------------------------------------------------------\n");
    }
    */

    assert(PTS >= 0);
    if(PTS < 0)
    {
        PTS = 0;
        Log::error(TRACE_MEDIAPLAYBACK, "setSampleAttributes: PTS value < 0. PTS: %lld", PTS);
    }
    sampleAttributes_.setPTS(PTS);
    sampleAttributes_.setDTS(DTS);
    sampleAttributes_.setTimescale(DeviceConstants::DEFAULT_TIMESCALE);
}

esplayer::Status Mp4Demultiplexer::StreamParser::setSampleAttributes(const DataBlock& block)
{
    esplayer::MediaType mediaType;
    const TrackFragmentContext::SampleEntry& sample =
        (trackFragmentContext_->sampleEntries())[currentSampleEntryIndex_];

    mediaType = block.getStreamAttributes()->audioAttributes_ ?
        esplayer::AUDIO : esplayer::VIDEO;
    sampleAttributes_.reset(mediaType, format3D_);
    if(mediaType == esplayer::AUDIO)
    {
        if(setSampleAudioAttributes(sample.sampleSize_,
                                    block.getStreamAttributes()->audioAttributes_) == false)
        {
            Log::error(TRACE_MEDIAPLAYBACK, "STREAM_ERROR from cdnId %u at timestamp %u of AUDIO",
                       block.getDataBuffer().cdnId_, block.getDataBuffer().timestamp_);
            return esplayer::STREAM_ERROR;
        }

        // CTS offset should always be 0 for audio.
        ctsOffsetOfFirstSampleInGop_ = 0;
    }
    else
    {
        if(setSampleVideoAttributes(sample.startOffset_ - currentOffsetFromMoof_,
                                    sample.sampleSize_,
                                    block.getStreamAttributes()->videoAttributes_) == false)
        {
            Log::error(TRACE_MEDIAPLAYBACK, "STREAM_ERROR from cdnId %u at timestamp %u of VIDEO",
                       block.getDataBuffer().cdnId_, block.getDataBuffer().timestamp_);
            return esplayer::STREAM_ERROR;
        }
        // isKeyFrame flag is set in setSampleVideoAttributes
        if(sampleAttributes_.isKeyFrame())
        {
            ctsOffsetOfFirstSampleInGop_ = sample.CTS_;
        }
    }
    setSampleTimestampAttributes(sample, block);
    setSampleDrmAttributes(block);

    return esplayer::OK;
}

bool Mp4Demultiplexer::StreamParser::blockListIsEmpty()
{
    if(blockList_.begin() == blockList_.end())
    {
        return true;
    }

    if(blockList_.begin()->getDataSize() == 0)
    {
        blockListCache_->popFront();

        if(blockList_.begin() == blockList_.end())
        {
            return true;
        }
    }
    return false;
}

esplayer::Status Mp4Demultiplexer::StreamParser::endOfStream()
{
    if(!endOfStreamLogged_)
    {
        NTRACE(TRACE_MEDIAPLAYBACK, "StreamParser::getNextSample() returning"
               " END_OF_STREAM for MediaType: %s",
               mediaType_== esplayer::AUDIO? "AUDIO" : "VIDEO");
        endOfStreamLogged_ = true;
    }

    // If this is the video player, the audio player could get stuck waiting
    // for a valid sync pts if we deliver end of stream and never set the
    // sync pts.
    if (syncPTSIsValid_ == false && syncMode_ == SEARCH_SYNC_PTS)
    {
        syncPTS_ = 0;
        syncPTSIsValid_ = true;
    }

    return esplayer::END_OF_STREAM;
}

esplayer::Status Mp4Demultiplexer::StreamParser::getNextSample(esplayer::ISampleWriter& sampleWriter)
{
    esplayer::Status status;

    if((status = getNextSampleAttributes()) != esplayer::OK)
    {
        return status;
    }

    status =  writeSample(sampleWriter);

    // If we couldn't write the sample to the device because it doesn't have
    // room for it, raise sampleWritePending_ so the next time we're called
    // we know we're all ready to write out a sample.
    sampleWritePending_ = (status == esplayer::NO_AVAILABLE_BUFFERS);
    return status;
}

esplayer::Status Mp4Demultiplexer::StreamParser::getNextSampleSize(uint32_t& sampleSize, uint32_t viewNum)
{
    esplayer::Status status;
    sampleSize = 0;

    if((status = getNextSampleAttributes()) != esplayer::OK)
    {
        return status;
    }
    sampleSize = sampleAttributes_.getSize(viewNum);
    return esplayer::OK;
}

esplayer::Status Mp4Demultiplexer::StreamParser::getNextSampleTimestamps(llong& dts, llong& pts)
{
    esplayer::Status status;
    dts = 0;

    if((status = getNextSampleAttributes()) != esplayer::OK)
    {
        return status;
    }
    dts = sampleAttributes_.getDTS();
    pts = sampleAttributes_.getPTS();
    return esplayer::OK;
}

esplayer::Status Mp4Demultiplexer::StreamParser::getNextSampleAttributes()
{
    bool bFoundSampleToDeliver = false;
    esplayer::Status status;

    // If we didn't deliver a sample after the last time
    // getNextSampleAttributes was successfully called , we've already
    // stored the next sample's attributes. We've computed everything we
    // need to deliver the sample. Skip ahead.
    if(sampleWritePending_ == false)
    {
        while(bFoundSampleToDeliver == false)
        {
            // Update our view of the list of blocks that we are the consumer
            // of and the AdaptiveStreamPlayer pumping loop is the producer of.
            blockListCache_->updateSnapshot();
            if(blockListIsEmpty())
            {
                if(endOfStream_) {
                    return endOfStream();
                }
                PRINT("%s@%d returns NO_AVAILABLE_SAMPLES\n", __func__, __LINE__);
                return esplayer::NO_AVAILABLE_SAMPLES;
            }

            // If we need to parse a fragment header, parse the next fragment header
            // that appears in our block list.
            if(fragmentParsingIsNeeded())
            {
                findNextFragmentHeaderAndParse();
                if(fragmentParseState_ != NEW_FRAGMENT)
                {
                    if(endOfStream_)
                    {
                        return endOfStream();
                    }
                    PRINT("%s@%d returns NO_AVAILABLE_SAMPLES\n", __func__, __LINE__);
                    return esplayer::NO_AVAILABLE_SAMPLES;
                }
            }

            // If syncMode_ is SYNC_TO_PTS, which will be true if this is the audio
            // stream and we've just started up, we're supposed to skip this sample
            // if its PTS plus duration is less than the syncPTS_ (which is the
            // video's PTS for its first frame). If the video player hasn't received
            // a frame yet, we don't know its first PTS and don't have a valid
            // syncPTS_.  We can't decide whether to deliver this sample or skip it
            // until we have a valid syncPTS_.
            if(syncMode_ == SYNC_TO_PTS && !syncPTSIsValid_)
            {
                PRINT("%s@%d returns NO_AVAILABLE_SAMPLES\n", __func__, __LINE__);
                return esplayer::NO_AVAILABLE_SAMPLES;
            }

            // If we've already delivered all the samples listed in the fragment
            // header, we need to skip to the next fragment.  There's something
            // wrong with the file because we should have seen a fragment start
            // (isStartOfGop) flag and known it was time to parse. We return
            // NO_AVAILABLE_SAMPLES and we'll force a fragment parse for the next
            // time getNextSample is called.
            if(currentSampleEntryIndex_ >= trackFragmentContext_->sampleEntries().size())
            {
                Log::error(TRACE_MEDIAPLAYBACK, "Mp4Demultiplexer::getNextSample:"
                           "Error: expected fragment header");
                fragmentParseState_ = SKIP_UNTIL_NEXT_FRAGMENT;
                PRINT("%s@%d returns NO_AVAILABLE_SAMPLES\n", __func__, __LINE__);
                return esplayer::NO_AVAILABLE_SAMPLES;
            }

            TrackFragmentContext::SampleEntry const& sample = (trackFragmentContext_->
                                                    sampleEntries())[currentSampleEntryIndex_];

            uint32_t const sampleSize = sample.sampleSize_;
            uint32_t const sampleOffset = sample.startOffset_;
            uint32_t const lowestSubsequentOffset = sample.lowestSubsequentOffset_;

            // If this is audio with timestamp that precedes the first video
            // sample's timestamp, we should skip the sample. If we skip it,
            // loop back and try again with the next sample.

            // We don't want to skip a sample which is partially within the blockList_, this would get
            // confusing. We just need enough bytes to advance as much as we can (to the lowest
            // offset of any subsequent sample)

            uint32_t const bytesToSkipSample = lowestSubsequentOffset - currentOffsetFromMoof_;
            uint32_t const bytesAvailable = blockListCache_->getNumBytesAvailable();
            if(bytesToSkipSample <= bytesAvailable &&
                (sampleShouldBeSkipped() || temporalLayerShouldBeSkipped()))
            {
                popBytesFromDataBlockList(bytesToSkipSample);
                currentSampleEntryIndex_++;
                continue;
            }

            // Check if the sample itself is within the remaining bytes .
            if( sampleOffset + sampleSize > currentOffsetFromMoof_ + blockListCache_->getNumBytesAvailable())
            {
                if(endOfStream_)
                {
                    return endOfStream();
                }
                return esplayer::NO_AVAILABLE_SAMPLES;
            }

            bFoundSampleToDeliver = true;
        }

        // Set the sample attributes.
        if( (status = setSampleAttributes(*(blockList_.begin()))) != esplayer::OK)
        {
            return status;
        }
    }
    sampleWritePending_ = true; // If this gets called again before we
    // successfully write out the sample, we
    // already have the attributes for the next
    // sample.
    return esplayer::OK;
}


// If this is encrypted video, we need to add subsample mapping entries for
// the parameter set NALUs we prepend to the IDR frame at the start of a
// fragment. Since CE3 parameter sets are not encrypted, the mapping entry's
// clear part is the length of the NALUs that are prepended and the
// encrypted part is of length 0.
//
// This should only be called for samples that are encrypted video IDR
// frames.
void Mp4Demultiplexer::StreamParser::addSubsampleEntriesForVideoParamSetsThatWillBePrepended()
{
    assert(trackFragmentContext_->drmAlgorithmID() != 0);
    if(trackFragmentContext_->drmAlgorithmID() > 0)
    {
        esplayer::VideoAttributes* attr =
            sampleAttributes_.mMediaAttributes.mVideoAttributes;
        assert(attr);

        for(uint32_t i = 0; i < attr->mCodecSpecificData.size(); i++)
        {
            sampleAttributes_.addSubsampleMappingEntry(
                attr->mCodecSpecificDataLen[i], 0, i);
        }
    }
}

void Mp4Demultiplexer::StreamParser::prependVideoParameterSets(esplayer::ISampleWriter& sampleWriter)
{
    // If this is AVC, the SPS and PPS are in mCodecSpecificData[0]. If this
    // is MVC_COMBINED, SPS, SSPS, PPS_base, and PPS_depended are in
    // mCodecSpecificData[0]. If this is MVC_SPLIT, SPS,PPS_base are in
    // mCodecSpecificData[0] and SSPS and PPS_dependent are in
    // mCodecSpecificData[1].
    esplayer::VideoAttributes* attr =
        sampleAttributes_.mMediaAttributes.mVideoAttributes;
    assert(attr);

    for(uint32_t i = 0; i < attr->mCodecSpecificData.size(); i++)
    {
        sampleWriter.write(attr->mCodecSpecificData[i],
                           attr->mCodecSpecificDataLen[i],i);
    }
}

esplayer::Status Mp4Demultiplexer::StreamParser::writeSample(esplayer::ISampleWriter& sampleWriter)
{
    TrackFragmentContext::SampleEntry const& sample = (trackFragmentContext_->
                                 sampleEntries())[currentSampleEntryIndex_];
    uint32_t const sampleSizeInMdat = sample.sampleSize_;
    uint32_t const sampleOffsetInMoof = sample.startOffset_;
    uint32_t const lowestSubsequentOffset = sample.lowestSubsequentOffset_;

    // Initialize the sample writer with the sample attributes to see if the
    // device can currently accept the sample. If it can't, leave the sample
    // data in the queue and we'll try to deliver it next time they ask for
    // a sample.
    esplayer::Status status = sampleWriter.initSample(sampleAttributes_);
    if(status != esplayer::OK)
    {
        if(status == esplayer::NO_AVAILABLE_BUFFERS) {
            return status;
        } else {
            Log::error(TRACE_MEDIAPLAYBACK, "Mp4Demultiplexer::getNextSample:"
                       "SampleWriter provided by the device returns error from "
                       "initSample sample.");
            return esplayer::ERROR;
        }
    }

    // The writer has accepted writing the sample. If this is a video IDR
    // frame (first frame in fragment), prepend parameter
    // sets to the sample.
    if(isStartOfFragment_ && mediaType_ == esplayer::VIDEO)
    {
        prependVideoParameterSets(sampleWriter);
    }

    // X360 AAC codec can't take ADTS header
    // If ADTSHeader is non-zero, this is AAC. Prepend the ADTS header to
    // the sample. The header has already been updated to reflect the size
    // of this sample.
    if(ADTSHeader_.size() > 0)
    {
        sampleWriter.write(&(ADTSHeader_[0]), ADTSHeader_.size());
    }

    // For audio samples, we use a DataBlockStream to do the
    // successive writes from each block to the device via the sample writer
    // that the device has provided. Video samples are written out an
    // access unit at a time so we insert start codes.
    if(mediaType_ == esplayer::AUDIO) {
        DataBlockStream blockStream(blockList_.begin(),
                                    blockList_.end(),
                                    blockListCache_->getNumBytesAvailable());
        blockStream.skipBytes( sampleOffsetInMoof - currentOffsetFromMoof_ );
        if(blockStream.writeBytes(sampleWriter, sampleSizeInMdat) ==false)
        {
            return esplayer::ERROR;
        }
    } else {
        if(format3D_ == esplayer::MVC_SPLIT)
        {
            if(writeMvcSplitAU(sampleWriter) != esplayer::OK)
            {
                return esplayer::ERROR;
            }
        } else {
            if(writeAU(sampleWriter) != esplayer::OK)
            {
                return esplayer::ERROR;
            }
        }
    }

    // If we get here we succeeded in writing the sample.
    // Sample has been init properly, okay to reset this flag.
    isStartOfFragment_ = false;

    popBytesFromDataBlockList(lowestSubsequentOffset-currentOffsetFromMoof_);
    currentSampleEntryIndex_++;
    if(firstSample_)
    {
        NTRACE(TRACE_MEDIAPLAYBACK, "StreamParser::GetNextSample: type %s, "
               "first sample delivered size %d, pts %lld",
               mediaType_== esplayer::AUDIO? "AUDIO" : "VIDEO",
               sampleSizeInMdat, sampleAttributes_.getPTS());
        firstSample_ = false;
    }
    return esplayer::OK;
}

bool Mp4Demultiplexer::StreamParser::createADTSHeader(const vector<uint8_t>& codecSpecificData)
{
    // parse AudioSpecificConfig() (in ISO/IEC 14496-3 Table 1.15) and
    // use info to build adts header (ISO/IEC 14496-3 Table 1.A.6 & Table 1.A.7)
    assert(codecSpecificData.size() >= 2);
    if(codecSpecificData.size() < 2)
    {
        Log::error(TRACE_MEDIAPLAYBACK,
                   "Mp4Parser::createADTSHeader: codecSpecificData len < 2.");
        return false;
    }

    uint8_t samplingFrequencyIndex = ((codecSpecificData[0] & 0x7) << 1) |
        (codecSpecificData[1] >> 7);

    uint8_t channelConfiguration = (codecSpecificData[1] >> 3) & 0xf;
    uint8_t audioObjectType	= (codecSpecificData[0] >> 3) & 0x1f;

    uint8_t ID = 1; // start with MPEG-2

    if (audioObjectType == 5) {
        ID = 0; // MPEG-4
    } else if (audioObjectType == 2) {
        // parse further to see if there's an extensionAudioObjectType of 5,
        //   if so, overwrite ID with 0

        // Skip 3 bits of GASpecificConfig()

        if (codecSpecificData.size() >= 4) {
            // if (extensionAudioObjectType != 5 && bits_to_decode() >= 16)
            uint32_t syncExtensionType = codecSpecificData[2] << 3 |
                codecSpecificData[3] >> 5;
            if (syncExtensionType == 0x2b7) {
                uint8_t extensionAudioObjectType = codecSpecificData[3] & 0x1f;
                if (extensionAudioObjectType == 5) {
                    ID = 0;
                } else {
                    Log::error(TRACE_MEDIAPLAYBACK, "Mp4Parser::createADTSHeader:invalid "
                               "extensionAudioObjectType %d", extensionAudioObjectType);
                    return false;
                }
            }
        }
    } else {
        Log::error(TRACE_MEDIAPLAYBACK, "Mp4Parser::createADTSHeader:invalid "
                   "audioObjectType %d", audioObjectType);
        return false;
    }

    ///
    // build adts header
    //
    // syncword: 0xfff
    // ID: <varies>
    // layer: 0
    // protection_absent: 1
    // profile_ObjectType: 1
    // sampling_frequency_index: <varies>
    // private_bit: 0
    // channel_configuration: <varies>
    // original_copy: 0
    // home: 0
    // copyright_identification_bit: 0
    // copyright_identification_start: 0
    // aac_frame_length: <varies>
    // adts_buffer_fullness: 0x7ff
    // number_of_raw_data_blocks_in_frame: 0
    ///

    ADTSHeader_.clear();
    ADTSHeader_.push_back(0xff);
    ADTSHeader_.push_back(ID ? 0xf9 : 0xf1);
    ADTSHeader_.push_back(
        0x40 | (samplingFrequencyIndex << 2) | (channelConfiguration >> 2));
    ADTSHeader_.push_back((channelConfiguration & 0x3) << 6);
    ADTSHeader_.push_back(0);
    ADTSHeader_.push_back(0x1f);
    ADTSHeader_.push_back(0xfc);

    return true;

}

bool Mp4Demultiplexer::StreamParser::updateADTSHeaderSizeBytes(uint32_t sampleSize)
{
    assert(ADTSHeader_.size() > 5);
    if(ADTSHeader_.size() <= 5)
    {
        Log::error(TRACE_MEDIAPLAYBACK, "MediaUtils::updateADTSHeaderSizeBytes: header "
                   "size %zu <= 5 bytes", ADTSHeader_.size());
        return false;
    }
    uint8_t *buf = &(ADTSHeader_[0]);

    buf[3] = (buf[3] & 0xfc) | (sampleSize >> 11);
    buf[4] = (sampleSize >> 3) & 0xff;
    buf[5] = (buf[5] & 0x1f) | ((sampleSize & 0x7) << 5);

    return true;
}

void Mp4Demultiplexer::StreamParser::setDemultiplexer(const shared_ptr<Mp4Demultiplexer> mp4Demux)
{
    if(!mp4Demux_)
        mp4Demux_ = mp4Demux;
}

void Mp4Demultiplexer::StreamParser::resetDemultiplexer()
{
    mp4Demux_.reset();
}
