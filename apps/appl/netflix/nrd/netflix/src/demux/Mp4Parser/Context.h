/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef __NETFLIX_CONTAINERLIB_MP4PARSER_Context__
#define __NETFLIX_CONTAINERLIB_MP4PARSER_Context__

#include <nrdbase/StdInt.h>
#include <vector>
#include <list>

#include <nrdbase/tr1.h>
#include "SampleGroupEntries.h"

namespace netflix {
namespace containerlib {
namespace mp4parser {

/*******************************************************************************
 * MovieContext
 */

class MovieContext
{
public:
    MovieContext();

    void reset();
};

/*******************************************************************************
 * TrackContext
 */

class TrackContext
{
public:
    struct TimeToSampleEntry
    {
        uint32_t sample_count_;
        uint32_t sample_delta_;
    };

    struct CompositionOffsetEntry
    {
        uint32_t sample_count_;
        uint32_t sample_offset_;
    };

    struct SampleToChunkEntry
    {
        uint32_t first_chunk_;
        uint32_t samples_per_chunk_;
        uint32_t sample_description_index_;
    };

    struct EditListEntry
    {
        uint64_t segment_duration_;
        uint64_t media_time_;
        uint32_t media_rate_integer_;
        uint32_t media_rate_fraction_;
    };

    typedef std::vector<uint8_t> ParameterSet;

private:
    uint32_t trackID_;
    uint32_t handlerType_;
    uint32_t sampleType_;
    uint32_t currSchemeType_;
    ullong mediaDuration_;
    uint32_t mediaTimescale_;
    uint32_t videoImageWidth_;
    uint32_t videoImageHeight_;
    uint32_t videoPixelAspectRatioH_;
    uint32_t videoPixelAspectRatioV_;
    uint32_t videoFrameRateValue_;
    uint32_t videoFrameRateScale_;
    uint32_t videoBitsDepthLuma_;
    uint32_t videoBitsDepthChroma_;
    int32_t colorPrimaries_;
    int32_t transferCharacteristics_;
    int32_t matrixCoefficients_;
    int32_t fullRange_;
    uint32_t generalLevelIdc_;
    uint32_t NALULengthSize_;
    uint32_t audioChannelCount_;
    uint32_t audioSampleSize_;
    uint32_t audioSampleRate_;

    uint32_t audioMaxBitrate_;
    uint32_t audioAveBitrate_;

    bool universalSampleSize_;

    uint32_t defaultSampleDescriptionIndex_;
    uint32_t defaultSampleDuration_;
    uint32_t defaultSampleSize_;
    uint32_t defaultSampleFlags_;

    uint32_t defaultDrmAlgorithmID_;
    uint32_t defaultDrmIVSize_;
    std::vector<uint8_t> defaultDrmKID_;

    std::list< shared_ptr<ParameterSet> > videoParameterSets_;
    std::list< shared_ptr<ParameterSet> > sequenceParameterSets_;
    std::list< shared_ptr<ParameterSet> > mvcSequenceParameterSets_;
    std::list< shared_ptr<ParameterSet> > pictureParameterSets_;
    std::list< shared_ptr<ParameterSet> > mvcPictureParameterSets_;

    // strictly speaking, SEI is not parameter set, but here, we just use
    // ParameterSet instead of redefining std::vector<uint8_t>
    std::list< shared_ptr<ParameterSet> > supplementalEnhancementInfos_;


    std::vector<uint8_t> decoderSpecificData_;

    std::vector<TimeToSampleEntry> timeToSampleEntries_;
    std::vector<CompositionOffsetEntry> compositionOffsetEntries_;
    std::vector<uint32_t> sampleSizeEntries_;
    std::vector<SampleToChunkEntry> sampleToChunkEntries_;
    std::vector<uint32_t> chunkOffsetEntries_;
    std::vector<std::vector<uint8_t> > naluToMvcViewMaps_;
                                     // There is a map for each sample group.
    std::vector<TemporalLayerEntry::Data> temporalLayerEntries_;
    std::vector<TemporalLevelEntry::Data> temporalLevelEntries_;
    std::vector<EditListEntry> editListEntries_;

public:
    TrackContext();

    void reset();

    inline void setTrackID(uint32_t trackID)
    {
        trackID_ = trackID;
    }

    inline uint32_t trackID() { return trackID_; }

    inline void setHandlerType(uint32_t handlerType)
    {
        handlerType_ = handlerType;
    }

    inline uint32_t handlerType() { return handlerType_; }

    inline void setSampleType(uint32_t sampleType)
    {
        sampleType_ = sampleType;
    }

    inline uint32_t sampleType() { return sampleType_; }

    inline void setCurrProtectionScheme(uint32_t schemeType)
    {
        currSchemeType_ = schemeType;
    }

    inline uint32_t currProtectionScheme() { return currSchemeType_; }

    inline void setNALULengthSize(uint32_t NALULengthSize)
    {
        NALULengthSize_ = NALULengthSize;
    }

    inline uint32_t NALULengthSize() { return NALULengthSize_; }

    inline void setUniversalSampleSize(bool universalSampleSize)
    {
        universalSampleSize_ = universalSampleSize;
    }

    inline bool universalSampleSize() { return universalSampleSize_; }

    inline void setDefaultDrmInfo(uint32_t algorithmID, uint32_t IVSize, std::vector<uint8_t> KID)
    {
        defaultDrmAlgorithmID_ = algorithmID;
        defaultDrmIVSize_ = IVSize;
        defaultDrmKID_ = KID;
    }

    inline void getDefaultDrmInfo(uint32_t& algorithmID, uint32_t& IVSize, std::vector<uint8_t>* KID)
    {
        algorithmID = defaultDrmAlgorithmID_;
        IVSize = defaultDrmIVSize_;
        *KID = defaultDrmKID_;
    }

    inline void setDefaultSampleInfo(uint32_t defaultSampleDescriptionIndex,
                                     uint32_t defaultSampleDuration,
                                     uint32_t defaultSampleSize,
                                     uint32_t defaultSampleFlags)
    {
        defaultSampleDescriptionIndex_ = defaultSampleDescriptionIndex;
        defaultSampleDuration_ = defaultSampleDuration;
        defaultSampleSize_ = defaultSampleSize;
        defaultSampleFlags_ = defaultSampleFlags;
    }

    inline void getDefaultSampleInfo(uint32_t& defaultSampleDescriptionIndex,
                                     uint32_t& defaultSampleDuration,
                                     uint32_t& defaultSampleSize,
                                     uint32_t& defaultSampleFlags)
    {
        defaultSampleDescriptionIndex = defaultSampleDescriptionIndex_;
        defaultSampleDuration = defaultSampleDuration_;
        defaultSampleSize = defaultSampleSize_;
        defaultSampleFlags = defaultSampleFlags_;
    }

    inline void setMediaTimeInfo(ullong duration, uint32_t timescale)
    {
        mediaDuration_ = duration;
        mediaTimescale_ = timescale;
    }

    inline void getMediaTimeInfo(ullong& duration, uint32_t& timescale)
    {
        duration = mediaDuration_;
        timescale = mediaTimescale_;
    }

    inline void setVideoImageSize(uint32_t width, uint32_t height)
    {
        videoImageWidth_ = width;
        videoImageHeight_ = height;
    }

    inline void getVideoImageSize(uint32_t& width, uint32_t& height)
    {
        width = videoImageWidth_;
        height = videoImageHeight_;
    }

    inline void setVideoPixelAspectRatio(uint32_t hSpacing, uint32_t vSpacing)
    {
        videoPixelAspectRatioH_ = hSpacing;
        videoPixelAspectRatioV_ = vSpacing;
    }

    inline void getVideoPixelAspectRatio(uint32_t& hSpacing, uint32_t& vSpacing)
    {
        hSpacing = videoPixelAspectRatioH_;
        vSpacing = videoPixelAspectRatioV_;
    }

    inline void setVideoFrameRate(uint32_t frameRateValue, uint32_t frameRateScale)
    {
        videoFrameRateValue_ = frameRateValue;
        videoFrameRateScale_ = frameRateScale;
    }

    inline void getVideoFrameRate(uint32_t& frameRateValue, uint32_t& frameRateScale)
    {
        frameRateValue = videoFrameRateValue_;
        frameRateScale = videoFrameRateScale_;
    }

    inline void setVideoBitsDepth(uint32_t bitsDepthLuma, uint32_t bitsDepthChroma)
    {
        videoBitsDepthLuma_ = bitsDepthLuma;
        videoBitsDepthChroma_ = bitsDepthChroma;
    }

    inline void getVideoBitsDepth(uint32_t& bitsDepthLuma, uint32_t& bitsDepthChroma)
    {
        bitsDepthLuma = videoBitsDepthLuma_;
        bitsDepthChroma = videoBitsDepthChroma_;
    }

    inline void getColorInformation(int32_t& colorPrimaries,
                                    int32_t& transferCharacteristics,
                                    int32_t& matrixCoefficients,
                                    int32_t& fullRange)
    {
        colorPrimaries = colorPrimaries_;
        transferCharacteristics = transferCharacteristics_;
        matrixCoefficients = matrixCoefficients_;
        fullRange = fullRange_ ;
    }

    inline void setColorInformation(int32_t colorPrimaries,
                                    int32_t transferCharacteristics,
                                    int32_t matrixCoefficients,
                                    int32_t fullRange)
    {
        colorPrimaries_ = colorPrimaries;
        transferCharacteristics_ = transferCharacteristics;
        matrixCoefficients_ = matrixCoefficients;
        fullRange_ = fullRange;
    }

    inline void setGeneralLevelIdc(uint32_t generalLevelIdc)
    {
        generalLevelIdc_ = generalLevelIdc;
    }

    inline void getGeneralLevelIdc(uint32_t& generalLevelIdc)
    {
        generalLevelIdc = generalLevelIdc_;
    }

    inline void setAudioSampleInfo(uint32_t audioChannelCount,
                                   uint32_t audioSampleSize,
                                   uint32_t audioSampleRate)
    {
        audioChannelCount_ = audioChannelCount;
        audioSampleSize_ = audioSampleSize;
        audioSampleRate_ = audioSampleRate;
    }

    inline void getAudioSampleInfo(uint32_t& audioChannelCount,
                                   uint32_t& audioSampleSize,
                                   uint32_t& audioSampleRate)
    {
        audioChannelCount = audioChannelCount_;
        audioSampleSize = audioSampleSize_;
        audioSampleRate = audioSampleRate_;
    }

    inline void setAudioElementaryStreamData(uint32_t& audioMaxBitrate,
                                             uint32_t& audioAveBitrate)
    {
        audioMaxBitrate_ = audioMaxBitrate;
        audioAveBitrate_ = audioAveBitrate;
    }

    inline void getAudioElementaryStreamData(uint32_t& audioMaxBitrate,
                                             uint32_t& audioAveBitrate)
    {
        audioMaxBitrate = audioMaxBitrate_;
        audioAveBitrate = audioAveBitrate_;
    }

    inline std::list< shared_ptr<ParameterSet> >& videoParameterSets()
    {
        return videoParameterSets_;
    }

    inline std::list< shared_ptr<ParameterSet> >& sequenceParameterSets()
    {
        return sequenceParameterSets_;
    }

    inline std::list< shared_ptr<ParameterSet> >& pictureParameterSets()
    {
        return pictureParameterSets_;
    }


    inline std::list< shared_ptr<ParameterSet> >& mvcSequenceParameterSets()
    {
        return mvcSequenceParameterSets_;
    }

    inline std::list< shared_ptr<ParameterSet> >& mvcPictureParameterSets()
    {
        return mvcPictureParameterSets_;
    }

    inline std::list< shared_ptr<ParameterSet> >& supplementalEnhancementInfos()
    {
        return supplementalEnhancementInfos_;
    }

    inline std::vector<uint8_t>& decoderSpecificData()
    {
        return decoderSpecificData_;
    }

    inline std::vector<TimeToSampleEntry>& timeToSampleEntries()
    {
        return timeToSampleEntries_;
    }

    inline std::vector<CompositionOffsetEntry>& compositionOffsetEntries()
    {
        return compositionOffsetEntries_;
    }

    inline std::vector<uint32_t>& sampleSizeEntries()
    {
        return sampleSizeEntries_;
    }

    inline std::vector<SampleToChunkEntry>& sampleToChunkEntries()
    {
        return sampleToChunkEntries_;
    }

    inline std::vector<uint32_t>& chunkOffsetEntries()
    {
        return chunkOffsetEntries_;
    }

    inline std::vector<std::vector<uint8_t> >& naluToMvcViewMaps()
    {
	return naluToMvcViewMaps_;
    }

    inline std::vector<TemporalLayerEntry::Data>& temporalLayerEntries()
    {
        return temporalLayerEntries_;
    }

    inline std::vector<TemporalLevelEntry::Data>& temporalLevelEntries()
    {
        return temporalLevelEntries_;
    }

    inline std::vector<EditListEntry>& editListEntries()
    {
        return editListEntries_;
    }

};

/*******************************************************************************
 * MovieFragmentContext
 */

class MovieFragmentContext
{
private:
    ullong baseDataOffset_;

public:
    MovieFragmentContext();

    void reset();

    inline void setBaseDataOffset(ullong dataOffset)
    {
        baseDataOffset_ = dataOffset;
    }

    inline ullong baseDataOffset() { return baseDataOffset_; }
};

/*******************************************************************************
 * TrackFragmentContext
 */

class TrackFragmentContext
{
public:
    struct SampleEntry
    {
        SampleEntry() : startOffset_(0),
                        lowestSubsequentOffset_(0),
                        independentAndDisposableFlags_(0),
                        temporalLayerId_(0),
                        temporalLayerAccessSample_(0)
        {
        }
        ullong startOffset_;              // offset from start of fragment (moof box)
        ullong lowestSubsequentOffset_;   // lowest offset of any subsequent sample in decode order
        uint32_t sampleDescriptionIndex_;
        uint32_t sampleSize_;
        uint32_t sampleFlags_;
        ullong DTS_; // relative to the beginning of the track fragment
        ullong CTS_; // relative to the beginning of the track fragment
        uint32_t sampleCompositionTimeOffset_;
        uint32_t sampleDuration_;
	uint8_t  sampleGroup_; // for indexing into nalu to mvc-view table.
        uint8_t  independentAndDisposableFlags_;
        uint8_t  temporalLayerId_;
        uint8_t  temporalLayerAccessSample_;
    };

    struct DrmInfoEntry
    {
        uint32_t IVDataOffset_;
        uint32_t IVSize_;
        uint32_t encryptionMapStartIndex_;
        uint32_t encryptionMapEntriesUsed_;
    };

    struct EncryptionMapEntry
    {
        uint32_t bytesOfClearData_;
        uint32_t bytesOfEncryptedData_;
    };

    /*
     * EncryptionEntry will replace DrmInfoEntry
     */
    struct EncryptionEntry
    {
        std::vector<uint8_t> IV_;
        std::vector<struct EncryptionMapEntry> subSampleMap_;
    };

private:
    uint32_t trackID_;
    ullong currentDataOffset_;
    ullong currentDTS_;

    uint32_t sampleDescriptionIndex_;
    uint32_t defaultSampleDuration_;
    uint32_t defaultSampleSize_;
    uint32_t defaultSampleFlags_;

    bool durationIsEmpty_;

    std::vector<SampleEntry> sampleEntries_;
    std::vector<DrmInfoEntry> drmInfoEntries_;
    std::vector<EncryptionEntry> encryptionEntries_;
    std::vector<EncryptionMapEntry> encryptionMapEntries_;
    std::vector<uint8_t> IVDataPool_;
    std::vector<uint64_t> dataOffsetsFromTruns_;

    uint32_t drmAlgorithmID_;
    uint64_t baseMediaDecodeTime_;

public:
    TrackFragmentContext();

    void reset();

    inline void setTrackID(uint32_t trackID)
    {
        trackID_ = trackID;
    }

    inline uint32_t trackID() { return trackID_; }

    inline void setBaseMediaDecodeTime(uint64_t baseMediaDecodeTime)
    {
        baseMediaDecodeTime_ = baseMediaDecodeTime;
    }

    inline uint64_t getBaseMediaDecodeTime()
    {
        return baseMediaDecodeTime_;
    }

    inline void setCurrentDataOffset(ullong dataOffset)
    {
        currentDataOffset_ = dataOffset;
        dataOffsetsFromTruns_.push_back(dataOffset);
    }

    inline ullong currentDataOffset() { return currentDataOffset_; }

    ullong getMaxDataOffset();

    inline void setCurrentDTS(ullong currentDTS)
    {
        currentDTS_ = currentDTS;
    }

    inline ullong currentDTS() { return currentDTS_; }

    inline void setSampleInfo(uint32_t sampleDescriptionIndex,
                              uint32_t defaultSampleDuration,
                              uint32_t defaultSampleSize,
                              uint32_t defaultSampleFlags,
                              bool durationIsEmpty)
    {
        sampleDescriptionIndex_ = sampleDescriptionIndex;
        defaultSampleDuration_ = defaultSampleDuration;
        defaultSampleSize_ = defaultSampleSize;
        defaultSampleFlags_ = defaultSampleFlags;
        durationIsEmpty_ = durationIsEmpty;
    }

    inline void getSampleInfo(uint32_t& sampleDescriptionIndex,
                              uint32_t& defaultSampleDuration,
                              uint32_t& defaultSampleSize,
                              uint32_t& defaultSampleFlags,
                              bool& durationIsEmpty)
    {
        sampleDescriptionIndex = sampleDescriptionIndex_;
        defaultSampleDuration = defaultSampleDuration_;
        defaultSampleSize = defaultSampleSize_;
        defaultSampleFlags = defaultSampleFlags_;
        durationIsEmpty = durationIsEmpty_;
    }

    inline std::vector<SampleEntry>& sampleEntries()
    {
        return sampleEntries_;
    }

    inline std::vector<DrmInfoEntry>& drmInfoEntries()
    {
        return drmInfoEntries_;
    }

    inline std::vector<EncryptionMapEntry>& encryptionMapEntries()
    {
        return encryptionMapEntries_;
    }

    inline std::vector<EncryptionEntry>& encryptionEntries()
    {
        return encryptionEntries_;
    }

    inline std::vector<uint8_t>& IVDataPool()
    {
        return IVDataPool_;
    }

    inline void setDrmAlgorithmID(uint32_t drmAlgorithmID)
    {
        drmAlgorithmID_ = drmAlgorithmID;
    }

    inline uint32_t drmAlgorithmID() { return drmAlgorithmID_; }
};

/*******************************************************************************
 * TrackIndexContext
 */

class TrackIndexContext
{
public:
    struct IndexEntry
    {
        ullong time_;
        ullong moofOffset_;
        uint32_t trafNumber_;
        uint32_t trunNumber_;
        uint32_t sampleNumber_;
    };

private:
    uint32_t trackID_;
    llong timescale_;
    std::vector<IndexEntry> indexEntries_;

public:
    TrackIndexContext();

    void reset();

    inline void setTrackID(uint32_t trackID)
    {
        trackID_ = trackID;
    }

    inline uint32_t trackID() { return trackID_; }

    inline void setTimescale(llong timescale)
    {
        timescale_ = timescale;
    }

    inline llong timescale() { return timescale_; }

    inline std::vector<IndexEntry>& indexEntries()
    {
        return indexEntries_;
    }
};

/*******************************************************************************
 * Context
 */

class Context
{
public:
    enum ParserState
    {
        PARSE_CONTINUE,
        PARSE_STOP,
        PARSE_ERROR_DATA,
        PARSE_ERROR_INVALID_OBJECT_SIZE,
        PARSE_ERROR_DUPLICATED_OBJECT,
        PARSE_ERROR_MISSING_OBJECT
    };

private:
    friend class Box;
    friend class SampleSizeBox;
    friend class ChunkOffsetBox;
    friend class BaseDescriptor;

    ParserState parserState_;

    inline void setParserState(ParserState parserState)
    {
        parserState_ = parserState;
    }

protected:
    shared_ptr<MovieContext> movieContext_;

    std::vector< shared_ptr<TrackContext> > trackContexts_;
    shared_ptr<TrackContext> currentTrackContext_;

    shared_ptr<MovieFragmentContext> movieFragmentContext_;

    std::vector< shared_ptr<TrackFragmentContext> > trackFragmentContexts_;
    shared_ptr<TrackFragmentContext> currentTrackFragmentContext_;

    std::vector< shared_ptr<TrackIndexContext> > trackIndexContexts_;
    shared_ptr<TrackIndexContext> currentTrackIndexContext_;

    // this should not be defined with Context. This should exist for each
    // trackContext for multi-track content
    uint32_t defaultDrmAlgorithmID_;
    uint32_t defaultDrmIVSize_;
    std::vector<uint8_t> defaultDrmKID_;

public:
    Context() : parserState_(PARSE_CONTINUE),
                defaultDrmAlgorithmID_(0),
                defaultDrmIVSize_(0)
    {
        defaultDrmKID_.clear();
    }

    virtual ~Context() {}

    void reset();

    inline ParserState parserState() { return parserState_; }

    inline shared_ptr<MovieContext> movieContext()
    {
        return movieContext_;
    }

    inline shared_ptr<TrackContext> currentTrackContext()
    {
        return currentTrackContext_;
    }

    inline shared_ptr<MovieFragmentContext> movieFragmentContext()
    {
        return movieFragmentContext_;
    }

    inline shared_ptr<TrackFragmentContext> currentTrackFragmentContext()
    {
        return currentTrackFragmentContext_;
    }

    inline shared_ptr<TrackIndexContext> currentTrackIndexContext()
    {
        return currentTrackIndexContext_;
    }

    inline void setDefaultDrmInfo(uint32_t algorithmID, uint32_t IVSize, std::vector<uint8_t> KID)
    {
        defaultDrmAlgorithmID_ = algorithmID;
        defaultDrmIVSize_ = IVSize;
        defaultDrmKID_ = KID;
    }

    inline void getDefaultDrmInfo(uint32_t& algorithmID, uint32_t& IVSize, std::vector<uint8_t>* KID)
    {
        algorithmID = defaultDrmAlgorithmID_;
        IVSize = defaultDrmIVSize_;
        *KID = defaultDrmKID_;
    }

    shared_ptr<TrackContext> findTrackContext(uint32_t trackID);

    void netflixHeaderFound(llong fileSize,
                            llong timescale,
                            llong duration,
                            llong offsetToFirstMoof,
                            llong sizeOfAllMoofAndMDat);

    void protectionSystemHeaderFound(
                std::vector<uint8_t>& systemID,
                std::vector<uint8_t>& protectionSystemSpecificData);

    void movieParseBegin(ullong startOffset, ullong boxSize);
    void movieParseEnd();
    void trackParseBegin(ullong startOffset, ullong boxSize);
    void trackParseEnd();

    void movieFragmentParseBegin(ullong startOffset, ullong boxSize);
    void movieFragmentParseEnd();
    void trackFragmentParseBegin(ullong startOffset, ullong boxSize);
    void trackFragmentParseEnd();

    void trackIndexParseBegin(ullong startOffset, ullong boxSize);
    void trackIndexParseEnd();

    virtual bool isProtectionSchemeSupported(uint32_t schemeType);

protected:
    virtual shared_ptr<MovieContext> createMovieContext();
    virtual shared_ptr<TrackContext> createTrackContext();
    virtual shared_ptr<MovieFragmentContext> createMovieFragmentContext();
    virtual shared_ptr<TrackFragmentContext> createTrackFragmentContext();
    virtual shared_ptr<TrackIndexContext> createTrackIndexContext();

    virtual void onMovieReset();
    virtual void onMovieFragmentReset();
    virtual void onTrackIndicesReset();

    virtual ParserState onNetflixHeaderFound(
                llong fileSize,
                llong timescale,
                llong duration,
                llong offsetToFirstMoof,
                llong sizeOfAllMoofAndMDat);

    virtual ParserState onProtectionSystemHeaderFound(
                std::vector<uint8_t>& systemID,
                std::vector<uint8_t>& protectionSystemSpecificData);

    virtual ParserState onMovieParseBegin(
                ullong startOffset, ullong boxSize);
    virtual ParserState onMovieParseEnd(
                shared_ptr<MovieContext> movieContext);

    virtual ParserState onTrackParseBegin(
                ullong startOffset, ullong boxSize);
    virtual ParserState onTrackParseEnd(
                shared_ptr<TrackContext> trackContext);

    virtual ParserState onMovieFragmentParseBegin(
                ullong startOffset, ullong boxSize);
    virtual ParserState onMovieFragmentParseEnd(
                shared_ptr<MovieFragmentContext> movieFragmentContext);

    virtual ParserState onTrackFragmentParseBegin(
                ullong startOffset, ullong boxSize);
    virtual ParserState onTrackFragmentParseEnd(
                shared_ptr<TrackFragmentContext> trackFragmentContext);

    virtual ParserState onTrackIndexParseBegin(
                ullong startOffset, ullong boxSize);
    virtual ParserState onTrackIndexParseEnd(
                shared_ptr<TrackIndexContext> trackIndexContext);
};

}}} // namespace netflix::containerlib::mp4parser

#endif // __NETFLIX_CONTAINERLIB_MP4PARSER_Context__
