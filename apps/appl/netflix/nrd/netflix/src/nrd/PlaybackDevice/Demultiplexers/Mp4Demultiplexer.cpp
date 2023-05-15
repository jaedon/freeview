/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#include <cassert>
#include <list>
#include <string.h>
#include <limits.h>

#include <nrdbase/Base64.h>
#include <nrdbase/Log.h>
#include <nrddemux/Context.h>
#include <nrddemux/Mp4Parser.h>
#include <nrddemux/Reader.h>
#include <nrd/NrdConfiguration.h>

#include "StreamAttributes.h"
#include "SampleAttributes.h"
#include "DeviceConstants.h"
#include "Mp4Demultiplexer.h"
#include "StreamParser.h"

using namespace std;
using namespace netflix;
using namespace netflix::device;
using namespace netflix::containerlib::mp4parser;

#define _FOURCC_LE_(a,b,c,d)  (a | (b << 8) | (c << 16) | (d << 24))
#define _FOURCC_BE_(a,b,c,d)  (d | (c << 8) | (b << 16) | (a << 24))

#ifdef NDEBUG
#define PRINT(fmt, ...)
#else
#include <stdio.h>
#define PRINT(fmt, ...) printf("DEMUX DEBUG:: " fmt, __VA_ARGS__ )
#endif

static const uint32_t MAX_UINT32           = 0xFFFFFFFF;
static const uint32_t INVALID_PTS          = MAX_UINT32;

/**
 * static function to parse moov header and return drm header and attributes
 */
NFErr Mp4Demultiplexer::parseStreamHeader(
    const unsigned char*        headerData,
    uint32_t                    headerDataSize,
    StreamAttributes&           streamAttributes,
    std::vector<unsigned char>* drmHeader,
    const unsigned char*        drmSystemID,
    std::vector<unsigned char>* nflxHeader)
{
    streamAttributes.defaultDrmAlgorithmID_ = 0;
    streamAttributes.defaultDrmIVSize_ = 0;
    streamAttributes.defaultDrmKID_.clear();
    streamAttributes.timescaleConverterRatio_ = 0;

    if(streamAttributes.audioAttributes_)
    {
        streamAttributes.audioAttributes_->streamID_ = 0; // not used.
        streamAttributes.audioAttributes_->formatID_ = 0;
        streamAttributes.audioAttributes_->blockAlignment_ = 4;
        streamAttributes.audioAttributes_->encrypted_ = false;
    }

    if(streamAttributes.videoAttributes_)
    {
        streamAttributes.videoAttributes_->streamID_ = 0; // not used.
        streamAttributes.videoAttributes_->formatID_ = 0;
        streamAttributes.videoAttributes_->bitsDepthLuma_ = 8;
        streamAttributes.videoAttributes_->bitsDepthChroma_ = 8;
        streamAttributes.videoAttributes_->encrypted_ = false;
    }

    MovieBoxParseContext context(streamAttributes.audioAttributes_,
                                 streamAttributes.videoAttributes_,
                                 streamAttributes.defaultDrmAlgorithmID_,
                                 streamAttributes.defaultDrmIVSize_,
                                 &(streamAttributes.defaultDrmKID_),
                                 streamAttributes.timescaleConverterRatio_,
                                 drmHeader, drmSystemID, nflxHeader);

    Mp4Parser mp4Parser;

    mp4Parser.parseMovieBox(context, headerData, headerDataSize);

    if (streamAttributes.defaultDrmKID_.size() > 0)
    {
        std::string kid((char*)&streamAttributes.defaultDrmKID_[0], streamAttributes.defaultDrmKID_.size());
        std::string kidB64 = Base64::encode(kid);
        Log::info(TRACE_MEDIAPLAYBACK, "%s: AlgorithmID %d, IVSize %d, KID %s\n"
                  ,__func__
                  ,streamAttributes.defaultDrmAlgorithmID_
                  ,streamAttributes.defaultDrmIVSize_
                  ,kidB64.c_str());
    }

    if(context.parserState() != Context::PARSE_STOP)
    {
        Log::warn(TRACE_MEDIAPLAYBACK,
                  "Error: Something wrong in MP4 header parsing\n");

        return NFErr_MP4ParsedData;
    }

    if(streamAttributes.audioAttributes_)
    {
        bool codecSpecificDataMandatory = false;
        if(streamAttributes.audioAttributes_->formatID_ == 0x1610) // HE-AAC
            codecSpecificDataMandatory = true;

        if(streamAttributes.audioAttributes_->formatID_         == 0 ||
           streamAttributes.audioAttributes_->numberOfChannels_ == 0 ||
           streamAttributes.audioAttributes_->bitsPerSample_    == 0 ||
           streamAttributes.audioAttributes_->samplesPerSecond_ == 0 ||
           (codecSpecificDataMandatory &&
            streamAttributes.audioAttributes_->codecSpecificData_.empty()))
        {
            Log::warn(TRACE_MEDIAPLAYBACK,
                      "Error: invalid audio stream attributes\n");

            return NFErr_MP4ParsedData;
        }
    }

    if(streamAttributes.videoAttributes_)
    {
        if(streamAttributes.videoAttributes_->formatID_          == 0 ||
           streamAttributes.videoAttributes_->imageWidth_        == 0 ||
           streamAttributes.videoAttributes_->imageHeight_       == 0 ||
           streamAttributes.videoAttributes_->pixelAspectRatioX_ == 0 ||
           streamAttributes.videoAttributes_->pixelAspectRatioY_ == 0 )
        {
            // common case for avc/mvc/hvc1/hev1
            Log::warn(TRACE_MEDIAPLAYBACK, "Error: invalid video stream attributes for all codecs\n");
            return NFErr_MP4ParsedData;
        }

        if(streamAttributes.videoAttributes_->formatID_  ==  _FOURCC_LE_('a','v','c','1') ) {
            // AVC should have valid SPS/PPS
            if( streamAttributes.videoAttributes_->sequenceParameterSet_.empty() ||
                streamAttributes.videoAttributes_->pictureParameterSet_.empty())
            {
                Log::warn(TRACE_MEDIAPLAYBACK, "Error: invalid video stream attributes for avc\n");
                return NFErr_MP4ParsedData;
            }
        }

        if(streamAttributes.videoAttributes_->formatID_  ==  _FOURCC_LE_('h','v','c','1') ) {
            // HEVC should have valid VPS/SPS/PPS
            if( streamAttributes.videoAttributes_->videoParameterSet_.empty() ||
                streamAttributes.videoAttributes_->sequenceParameterSet_.empty() ||
                streamAttributes.videoAttributes_->pictureParameterSet_.empty())
            {
                Log::warn(TRACE_MEDIAPLAYBACK, "Error: invalid video stream attributes for hevc(hvc1)\n");
                return NFErr_MP4ParsedData;
            }
        }

        if (streamAttributes.videoAttributes_->formatID_  ==  _FOURCC_LE_('h','e','v','1') ) {

        }
    }

    return NFErr_OK;
}

/**
 * Constructor
 */
Mp4Demultiplexer::Mp4Demultiplexer(IConsumerListView<DataBlock>& audioBlockList,
                                   IConsumerListView<DataBlock>& videoBlockList,
                                   esplayer::Format3D video3DFormat,
                                   const std::vector<ContentProfile>& videoProfiles,
                                   const bool supportSeamlessFramerateSwitching,
                                   shared_ptr<EventQueue> eventQueue
    )
    : firstVideoKeyFramePTSIsValid_(false),
      firstVideoKeyFramePTS_(ULONG_MAX),
      videoProfiles_(videoProfiles),
      supportSeamlessFramerateSwitching_(supportSeamlessFramerateSwitching),
      currentPlaybackPositionPTS_(INVALID_PTS),
      eventQueue_(eventQueue)

{
    audioParser_.reset(new StreamParser(firstVideoKeyFramePTS_,
                                        firstVideoKeyFramePTSIsValid_,
                                        audioBlockList,
                                        esplayer::AUDIO));


    videoParser_.reset(new StreamParser(firstVideoKeyFramePTS_,
                                        firstVideoKeyFramePTSIsValid_,
                                        videoBlockList,
                                        esplayer::VIDEO,
                                        video3DFormat,
                                        videoProfiles,
                                        supportSeamlessFramerateSwitching));

    audioParser_->setSyncMode(StreamParser::SYNC_TO_PTS);
    videoParser_->setSyncMode(StreamParser::SEARCH_SYNC_PTS);
}

bool Mp4Demultiplexer::init()
{
    shared_ptr<Mp4Demultiplexer> that = static_pointer_cast<Mp4Demultiplexer>(shared_from_this());
    audioParser_->setDemultiplexer(that);
    videoParser_->setDemultiplexer(that);
    return true;
}

void Mp4Demultiplexer::close()
{
    // break circular reference
    audioParser_->resetDemultiplexer();
    videoParser_->resetDemultiplexer();
}

void Mp4Demultiplexer::deliverEndOfStream(IPlaybackDevice::StreamType type)
{

    if(type == IPlaybackDevice::AUDIO)
    {
        audioParser_->setEndOfStream();
    } else if (type == IPlaybackDevice::VIDEO) {
        videoParser_->setEndOfStream();
    }

}

void Mp4Demultiplexer::setDiscontinuity()
{
    audioParser_->setDiscontinuity();
    videoParser_->setDiscontinuity();

    firstVideoKeyFramePTSIsValid_ = false;
    audioParser_->setSyncMode(StreamParser::SYNC_TO_PTS);
    videoParser_->setSyncMode(StreamParser::SEARCH_SYNC_PTS);
}

void Mp4Demultiplexer::setAudioDiscontinuity()
{
    audioParser_->setDiscontinuity();
    audioParser_->setSyncMode(StreamParser::SYNC_TO_CURRENT_PLAYBACK_PTS);
}

void  Mp4Demultiplexer::set3DFormat(esplayer::Format3D format3D)
{
    videoParser_->set3DFormat(format3D);
}

uint32_t Mp4Demultiplexer::getVideoSampleDuration()
{
    return videoParser_->getVideoSampleDuration();
}

esplayer::Status Mp4Demultiplexer::getNextSample(esplayer::MediaType mediaType,
                                                 esplayer::ISampleWriter& sampleWriter)
{
    if(mediaType == esplayer::AUDIO)
    {
        return audioParser_->getNextSample(sampleWriter);

    } else
    {
        return videoParser_->getNextSample(sampleWriter);
    }
}

esplayer::Status Mp4Demultiplexer::getNextSampleSize(esplayer::MediaType mediaType,
                                                     uint32_t& sampleSize,
                                                     uint32_t viewNum)
{
    if(mediaType == esplayer::AUDIO)
    {
        return audioParser_->getNextSampleSize(sampleSize);

    } else
    {
        return videoParser_->getNextSampleSize(sampleSize, viewNum);
    }
}

esplayer::Status Mp4Demultiplexer::getNextSampleTimestamps(esplayer::MediaType mediaType,
                                                           llong& dts,
                                                           llong& pts)
{
    if(mediaType == esplayer::AUDIO)
    {
        return audioParser_->getNextSampleTimestamps(dts, pts);

    } else
    {
        return videoParser_->getNextSampleTimestamps(dts, pts);
    }
}

shared_ptr<EventQueue> Mp4Demultiplexer::getAttachedEventQueue()
{
    return eventQueue_;
}

void Mp4Demultiplexer::copyAttributes(VideoAttributes *videoAttr,
                                      esplayer::MediaAttributes& mediaAttr,
                                      esplayer::Format3D format3D)
{
    assert(videoAttr && mediaAttr.mVideoAttributes);

    esplayer::VideoAttributes* pESAttr = mediaAttr.mVideoAttributes;

    pESAttr->mFrameRateValue = videoAttr->frameRateValue_;
    pESAttr->mFrameRateScale = videoAttr->frameRateScale_;
    pESAttr->mFormat3D = format3D;
    pESAttr->mCodecParam = StreamParser::getVideoCodecParam(videoAttr);
    pESAttr->mImageWidth = videoAttr->imageWidth_;
    pESAttr->mImageHeight = videoAttr->imageHeight_;
    pESAttr->mPixelAspectRatioX = videoAttr->pixelAspectRatioX_;
    pESAttr->mPixelAspectRatioY = videoAttr->pixelAspectRatioY_;
    pESAttr->mBitsDepthLuma = videoAttr->bitsDepthLuma_;
    pESAttr->mBitsDepthChroma = videoAttr->bitsDepthChroma_;

    // Copy pointers to the sequence and picture parameter sets and the
    // lengths of the two arrays.
    pESAttr->mCodecSpecificData.clear();
    pESAttr->mCodecSpecificDataLen.clear();
    videoAttr->embeddedConfigData_.clear();

    // If there are mvc sequence and picture parameter sets, the
    // order of parameter sets should be sps, ssps, pps (base), pps (dependent).
    if(videoAttr->mvcAttributes_)
    {
        int depIndex;
        vector<vector<unsigned char> >& psets = videoAttr->embeddedConfigData_;

        if(format3D == esplayer::MVC_COMBINED)
        {
            depIndex = 0;
            psets.resize(1);
        } else {
            depIndex = 1;
            psets.resize(2);
        }

        psets[0] = videoAttr->sequenceParameterSet_;
        psets[depIndex].insert(psets[depIndex].end(),
                               videoAttr->mvcAttributes_->subsequenceParameterSet_.begin(),
                               videoAttr->mvcAttributes_->subsequenceParameterSet_.end());
        psets[0].insert(psets[0].end(),
                        videoAttr->pictureParameterSet_.begin(),
                        videoAttr->pictureParameterSet_.end());
        psets[depIndex].insert(psets[depIndex].end(),
                               videoAttr->mvcAttributes_->pictureParameterSet_.begin(),
                               videoAttr->mvcAttributes_->pictureParameterSet_.end());

        // For MVC combined, all four parameter sets are in
        // videoAttr->embeddedConfigData_[0].
        pESAttr->mCodecSpecificData.push_back(const_cast<const uint8_t*>(&(psets[0])[0]));
        pESAttr->mCodecSpecificDataLen.push_back(psets[0].size());

        // For MVC split, the SSPS and dependent view PPS is in
        // videoAttr->embeddedConfigData[1].
        if(format3D != esplayer::MVC_COMBINED)
        {
            pESAttr->mCodecSpecificData.push_back(const_cast<const uint8_t*>(&(psets[1])[0]));
            pESAttr->mCodecSpecificDataLen.push_back(psets[1].size());
        }
    } else {
        if( videoAttr->formatID_  ==  _FOURCC_LE_('h','v','c','1') ){
            // This is HEVC with hvc1
            videoAttr->embeddedConfigData_.resize(1);
            vector<unsigned char>& psets = videoAttr->embeddedConfigData_[0];
            psets = videoAttr->videoParameterSet_;
            psets.insert(psets.end(), videoAttr->sequenceParameterSet_.begin(),
                         videoAttr->sequenceParameterSet_.end());
            psets.insert(psets.end(), videoAttr->pictureParameterSet_.begin(),
                         videoAttr->pictureParameterSet_.end());

            pESAttr->mCodecSpecificData.push_back(const_cast<const uint8_t*>(&psets[0]));
            pESAttr->mCodecSpecificDataLen.push_back(psets.size());

        } else if (videoAttr->formatID_  ==  _FOURCC_LE_('a','v','c','1') ){
            // This is plain AVC.  IElementaryStreamPlayer specifies that SPS is
            // first, then PPS.
            videoAttr->embeddedConfigData_.resize(1);
            vector<unsigned char>& psets = videoAttr->embeddedConfigData_[0];
            psets = videoAttr->sequenceParameterSet_;
            psets.insert(psets.end(), videoAttr->pictureParameterSet_.begin(),
                         videoAttr->pictureParameterSet_.end());

            pESAttr->mCodecSpecificData.push_back(const_cast<const uint8_t*>(&psets[0]));
            pESAttr->mCodecSpecificDataLen.push_back(psets.size());
        }
        // nothing to be done for hev1
    }
}

void Mp4Demultiplexer::copyAttributes(const AudioAttributes *audioAttr,
                                      esplayer::MediaAttributes& mediaAttr)
{
    assert(audioAttr && mediaAttr.mAudioAttributes);
    mediaAttr.mAudioAttributes->mCodecParam = StreamParser::getAudioCodecParam(audioAttr);
    mediaAttr.mAudioAttributes->mNumberOfChannels = audioAttr->numberOfChannels_;
    mediaAttr.mAudioAttributes->mSamplesPerSecond = audioAttr->samplesPerSecond_;
    mediaAttr.mAudioAttributes->mBitrate = audioAttr->nominalBitrateInKbps_;
    //printf("StreamInfo at Mp4Demux audio bitrate %d\n", mediaAttr.mAudioAttributes->mBitrate);
    mediaAttr.mAudioAttributes->mBlockAlignment =  audioAttr->blockAlignment_;
    mediaAttr.mAudioAttributes->mCodecSpecificDataLen = audioAttr->codecSpecificData_.size();
    if(mediaAttr.mAudioAttributes->mCodecSpecificDataLen){
        mediaAttr.mAudioAttributes->mCodecSpecificData =
            const_cast<unsigned char*>(&audioAttr->codecSpecificData_[0]);
    }
}


Mp4Demultiplexer::DataBlock::DataBlock() :
    dataBuffer_(NULL),
    offset_(0),
    streamAttributes_(NULL),
    isParsed_(false)
{
}

Mp4Demultiplexer::DataBlock::~DataBlock()
{
}

uint32_t Mp4Demultiplexer::DataBlock::getDataSize()
{
    return dataBuffer_.dataSize_ - offset_;
}

unsigned char* Mp4Demultiplexer::DataBlock::getData()
{
    if(offset_ >= dataBuffer_.dataSize_)
    {
#if defined(STREAM_ERROR_DEBUG)
        if(streamAttributes_->videoAttributes_){
	    printf("getData return NULL\n");
        }
#endif
        return NULL;
    }
#if defined(STREAM_ERROR_DEBUG)
    if(streamAttributes_->videoAttributes_){
        printf("getData (dataBuffer_.byteBuffer + offset_) = %u + %u = %u\n",
               (unsigned char*)dataBuffer_.byteBuffer_ ,offset_,
               ((unsigned char*)dataBuffer_.byteBuffer_ + offset_));
    }
#endif
    return ((unsigned char*)dataBuffer_.byteBuffer_ + offset_);
}

void Mp4Demultiplexer::DataBlock::advanceOffset(uint32_t numBytes)
{
    offset_ += numBytes;
    if(offset_ > dataBuffer_.dataSize_)
        offset_ = dataBuffer_.dataSize_;
#if defined(STREAM_ERROR_DEBUG)
    if(streamAttributes_->videoAttributes_){
        printf("DataBlock::advanceOffset offset_ %4u, numBytes %5u, dataBuffer_.dataSize_ %5d, dataBuffer_.byteBuffer %u getData() will be %u\n",
	       offset_, numBytes, dataBuffer_.dataSize_, dataBuffer_.byteBuffer_, dataBuffer_.byteBuffer_ + offset_ );
    }
#endif
}

void Mp4Demultiplexer::DataBlock::setDataBuffer(IBufferManager::Buffer& buffer)
{
    dataBuffer_ = buffer;
}

void Mp4Demultiplexer::DataBlock::setStreamAttributes(StreamAttributes *attributes)
{
    streamAttributes_ = attributes;
}

void Mp4Demultiplexer::DataBlock::setIsParsed()
{
    isParsed_ = true;
}

bool Mp4Demultiplexer::DataBlock::isParsed()
{
    return isParsed_;
}

const IBufferManager::Buffer&  Mp4Demultiplexer::DataBlock::getDataBuffer() const
{
    return dataBuffer_;
}

const StreamAttributes*  Mp4Demultiplexer::DataBlock::getStreamAttributes() const
{
    return streamAttributes_;
}


void Mp4Demultiplexer::DataBlock::reset()
{
    offset_ = 0;
    streamAttributes_ = NULL;
    isParsed_ = false;
    dataBuffer_.inUse_ = NULL;
    dataBuffer_.byteBuffer_ = NULL;
}
