/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#include <algorithm>
#include "Context.h"

using namespace std;
using namespace netflix::containerlib::mp4parser;

/*******************************************************************************
 * MovieContext
 */

MovieContext::MovieContext()
{
    reset();
}

void MovieContext::reset()
{
}

/*******************************************************************************
 * TrackContext
 */

TrackContext::TrackContext()
{
    reset();
}

void TrackContext::reset()
{
    trackID_ = 0;
    handlerType_ = 0;
    sampleType_ = 0;
    currSchemeType_ = 0;
    mediaDuration_ = 0;
    mediaTimescale_ = 0;
    videoImageWidth_ = 0;
    videoImageHeight_ = 0;
    videoPixelAspectRatioH_ = 1;
    videoPixelAspectRatioV_ = 1;
    videoFrameRateValue_ = 0;
    videoFrameRateScale_ = 1;
    videoBitsDepthLuma_ = 8;
    videoBitsDepthChroma_ = 8;
    colorPrimaries_ = -1;
    transferCharacteristics_ = -1;
    matrixCoefficients_ = -1;
    fullRange_ = -1;
    NALULengthSize_ = 0;
    audioChannelCount_ = 0;
    audioSampleSize_ = 0;
    audioSampleRate_ = 0;
    universalSampleSize_ = false;

    defaultSampleDescriptionIndex_ = 0;
    defaultSampleDuration_ = 0;
    defaultSampleSize_ = 0;
    defaultSampleFlags_ = 0;

    defaultDrmAlgorithmID_ = 0;
    defaultDrmIVSize_ = 0;
    defaultDrmKID_.clear();

    videoParameterSets_.clear();
    sequenceParameterSets_.clear();
    pictureParameterSets_.clear();
    supplementalEnhancementInfos_.clear();

    timeToSampleEntries_.clear();
    compositionOffsetEntries_.clear();
    sampleSizeEntries_.clear();
    sampleToChunkEntries_.clear();
    chunkOffsetEntries_.clear();

    temporalLayerEntries_.clear();
    temporalLevelEntries_.clear();
}

/*******************************************************************************
 * MovieFragmentContext
 */

MovieFragmentContext::MovieFragmentContext()
{
    reset();
}

void MovieFragmentContext::reset()
{
    baseDataOffset_ = 0;
}

/*******************************************************************************
 * TrackFragmentContext
 */

TrackFragmentContext::TrackFragmentContext()
{
    reset();
}

void TrackFragmentContext::reset()
{
    trackID_ = 0;
    currentDataOffset_ = 0;
    currentDTS_ = 0;

    sampleDescriptionIndex_ = 0;
    defaultSampleDuration_ = 0;
    defaultSampleSize_ = 0;
    defaultSampleFlags_ = 0;

    durationIsEmpty_ = false;

    sampleEntries_.clear();
    drmInfoEntries_.clear();
    encryptionMapEntries_.clear();
    encryptionEntries_.clear();
    IVDataPool_.clear();
    dataOffsetsFromTruns_.clear();

    drmAlgorithmID_ = 0;
}

ullong TrackFragmentContext::getMaxDataOffset()
{
    std::vector<uint64_t>::iterator result;
    result = std::max_element(dataOffsetsFromTruns_.begin(), dataOffsetsFromTruns_.end());
    return *result;
}

/*******************************************************************************
 * TrackIndexContext
 */

TrackIndexContext::TrackIndexContext()
{
    reset();
}

void TrackIndexContext::reset()
{
    trackID_ = 0;
    timescale_ = 0;
    indexEntries_.clear();
}

/*******************************************************************************
 * Context
 */

void Context::reset()
{
    parserState_ = PARSE_CONTINUE;

    onMovieReset();

    movieContext_.reset();
    currentTrackContext_.reset();
    trackContexts_.clear();

    onMovieFragmentReset();

    movieFragmentContext_.reset();
    currentTrackFragmentContext_.reset();
    trackFragmentContexts_.clear();

    onTrackIndicesReset();

    currentTrackIndexContext_.reset();
    trackIndexContexts_.clear();

    defaultDrmAlgorithmID_ = 0;
    defaultDrmIVSize_ = 0;
}

shared_ptr<TrackContext> Context::findTrackContext(uint32_t trackID)
{
    vector< shared_ptr<TrackContext> >::iterator iter;
    for(iter = trackContexts_.begin(); iter != trackContexts_.end(); iter ++)
    {
        if(trackID == (*iter)->trackID())
            return (*iter);
    }

    return shared_ptr<TrackContext>();
}

void Context::netflixHeaderFound(llong fileSize,
                                 llong timescale,
                                 llong duration,
                                 llong offsetToFirstMoof,
                                 llong sizeOfAllMoofAndMDat)
{
    parserState_ = onNetflixHeaderFound(fileSize,
                                        timescale,
                                        duration,
                                        offsetToFirstMoof,
                                        sizeOfAllMoofAndMDat);
}

void Context::protectionSystemHeaderFound(
        std::vector<uint8_t>& systemID,
        std::vector<uint8_t>& protectionSystemSpecificData)
{
    parserState_ = onProtectionSystemHeaderFound(systemID,
                                                 protectionSystemSpecificData);
}

void Context::movieParseBegin(ullong startOffset, ullong boxSize)
{
    if(movieContext_.get() == NULL)
    {
        movieContext_ = createMovieContext();
        parserState_ = onMovieParseBegin(startOffset, boxSize);
    }
    else
    {
        parserState_ = PARSE_ERROR_DUPLICATED_OBJECT;
    }
}

void Context::movieParseEnd()
{
    parserState_ = onMovieParseEnd(movieContext_);
}

void Context::trackParseBegin(ullong startOffset, ullong boxSize)
{
    currentTrackContext_ = createTrackContext();
    trackContexts_.push_back(currentTrackContext_);

    parserState_ = onTrackParseBegin(startOffset, boxSize);
}

void Context::trackParseEnd()
{
    parserState_ = onTrackParseEnd(currentTrackContext_);
    currentTrackContext_.reset();
}

void Context::movieFragmentParseBegin(ullong startOffset, ullong boxSize)
{
    onMovieFragmentReset();

    movieFragmentContext_.reset();
    currentTrackFragmentContext_.reset();
    trackFragmentContexts_.clear();

    movieFragmentContext_ = createMovieFragmentContext();
    movieFragmentContext_->setBaseDataOffset(startOffset);

    parserState_ = onMovieFragmentParseBegin(startOffset, boxSize);
}

void Context::movieFragmentParseEnd()
{
    parserState_ = onMovieFragmentParseEnd(movieFragmentContext_);
}

void Context::trackFragmentParseBegin(ullong startOffset, ullong boxSize)
{
    currentTrackFragmentContext_ = createTrackFragmentContext();
    trackFragmentContexts_.push_back(currentTrackFragmentContext_);

    parserState_ = onTrackFragmentParseBegin(startOffset, boxSize);
}

void Context::trackFragmentParseEnd()
{
    // Calculate the lowestSubsequentOffset
    std::vector<TrackFragmentContext::SampleEntry>::iterator it = currentTrackFragmentContext_->sampleEntries().begin();
    std::vector<TrackFragmentContext::SampleEntry>::iterator const end = currentTrackFragmentContext_->sampleEntries().end();
    ullong lowestSubsequentOffset = 0;
    for( ; it != end; ++it )
    {
        if ( it->startOffset_ + it->sampleSize_ > lowestSubsequentOffset )
            lowestSubsequentOffset = it->startOffset_ + it->sampleSize_;
    }

    std::vector<TrackFragmentContext::SampleEntry>::reverse_iterator rit = currentTrackFragmentContext_->sampleEntries().rbegin();
    std::vector<TrackFragmentContext::SampleEntry>::reverse_iterator const rend = currentTrackFragmentContext_->sampleEntries().rend();
    for( ; rit != rend; ++rit )
    {
        rit->lowestSubsequentOffset_ = lowestSubsequentOffset;
        lowestSubsequentOffset = lowestSubsequentOffset < rit->startOffset_ ? lowestSubsequentOffset : rit->startOffset_;
    }

    parserState_ = onTrackFragmentParseEnd(currentTrackFragmentContext_);

    movieFragmentContext_->setBaseDataOffset(
        currentTrackFragmentContext_->currentDataOffset());

    currentTrackFragmentContext_.reset();
}

void Context::trackIndexParseBegin(ullong startOffset, ullong boxSize)
{
    currentTrackIndexContext_ = createTrackIndexContext();
    trackIndexContexts_.push_back(currentTrackIndexContext_);

    parserState_ = onTrackIndexParseBegin(startOffset, boxSize);
}

void Context::trackIndexParseEnd()
{
    parserState_ = onTrackIndexParseEnd(currentTrackIndexContext_);
    currentTrackIndexContext_.reset();
}

bool Context::isProtectionSchemeSupported(uint32_t /*schemeType*/)
{
    return false;
}

shared_ptr<MovieContext> Context::createMovieContext()
{
    return shared_ptr<MovieContext>(new MovieContext);
}

shared_ptr<TrackContext> Context::createTrackContext()
{
    return shared_ptr<TrackContext>(new TrackContext);
}

shared_ptr<MovieFragmentContext> Context::createMovieFragmentContext()
{
    return shared_ptr<MovieFragmentContext>(new MovieFragmentContext);
}

shared_ptr<TrackFragmentContext> Context::createTrackFragmentContext()
{
    return shared_ptr<TrackFragmentContext>(new TrackFragmentContext);
}

shared_ptr<TrackIndexContext>
    Context::createTrackIndexContext()
{
    return shared_ptr<TrackIndexContext>(new TrackIndexContext);
}

void Context::onMovieReset()
{
}

void Context::onMovieFragmentReset()
{
}

void Context::onTrackIndicesReset()
{
}

Context::ParserState Context::onNetflixHeaderFound(
        llong /*fileSize*/,
        llong /*timescale*/,
        llong /*duration*/,
        llong /*offsetToFirstMoof*/,
        llong /*sizeOfAllMoofAndMDat*/)
{
    return PARSE_CONTINUE;
}

Context::ParserState Context::onProtectionSystemHeaderFound(
        std::vector<uint8_t>& /*systemID*/,
        std::vector<uint8_t>& /*protectionSystemSpecificData*/)
{
    return PARSE_CONTINUE;
}

Context::ParserState Context::onMovieParseBegin(
        ullong /*startOffset*/, ullong /*boxSize*/)
{
    return PARSE_CONTINUE;
}

Context::ParserState Context::onMovieParseEnd(
        shared_ptr<MovieContext> /*movieContext*/)
{
    return PARSE_CONTINUE;
}

Context::ParserState Context::onTrackParseBegin(
        ullong /*startOffset*/, ullong /*boxSize*/)
{
    return PARSE_CONTINUE;
}

Context::ParserState Context::onTrackParseEnd(
        shared_ptr<TrackContext> /*trackContext*/)
{
    return PARSE_CONTINUE;
}

Context::ParserState Context::onMovieFragmentParseBegin(
        ullong /*startOffset*/, ullong /*boxSize*/)
{
    return PARSE_CONTINUE;
}

Context::ParserState Context::onMovieFragmentParseEnd(
        shared_ptr<MovieFragmentContext> /*movieFragmentContext*/)
{
    return PARSE_CONTINUE;
}

Context::ParserState Context::onTrackFragmentParseBegin(
        ullong /*startOffset*/, ullong /*boxSize*/)
{
    return PARSE_CONTINUE;
}

Context::ParserState Context::onTrackFragmentParseEnd(
        shared_ptr<TrackFragmentContext> /*trackFragmentContext*/)
{
    return PARSE_CONTINUE;
}

Context::ParserState Context::onTrackIndexParseBegin(
        ullong /*startOffset*/, ullong /*boxSize*/)
{
    return PARSE_CONTINUE;
}

Context::ParserState Context::onTrackIndexParseEnd(
        shared_ptr<TrackIndexContext> /*trackIndexContext*/)
{
    return PARSE_CONTINUE;
}
