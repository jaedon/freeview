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
 * Constructor
 */
Mp4Demultiplexer::Mp4Demultiplexer(IConsumerListView<DataBlock>& videoBlockList,
                                   shared_ptr<EventQueue> eventQueue_) :
  mEventQueue(eventQueue)
{
    videoParser_.reset(new StreamParser(firstVideoKeyFramePTS_,
                                        firstVideoKeyFramePTSIsValid_,
                                        videoBlockList,
                                        esplayer::VIDEO,
                                        video3DFormat,
                                        videoProfiles,
                                        supportSeamlessFramerateSwitching));

    videoParser_->setSyncMode(StreamParser::SEARCH_SYNC_PTS);
}

NFErr Mp4Demultiplexer::parseStreamHeader(const unsigned char* headerData,
                                          uint32_t headerDataSize,
                                          VideoStreamAttributes& streamAttributes)
{
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
                                 streamAttributes.timescaleConverterRatio_,
                                 drmHeader, drmSystemID, nflxHeader);

    Mp4Parser mp4Parser;

    mp4Parser.parseMovieBox(context, headerData, headerDataSize);

    if(context.parserState() != Context::PARSE_STOP)
    {
        Log::warn(TRACE_MEDIAPLAYBACK,
                  "Error: Something wrong in MP4 header parsing\n");

        return NFErr_MP4ParsedData;
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


