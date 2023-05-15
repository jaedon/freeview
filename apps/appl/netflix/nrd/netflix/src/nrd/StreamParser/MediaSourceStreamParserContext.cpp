/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "MediaSourceStreamParser.h"

#define _FOURCC_LE_(a,b,c,d)  (a | (b << 8) | (c << 16) | (d << 24))
#define _FOURCC_BE_(a,b,c,d)  (d | (c << 8) | (b << 16) | (a << 24))

#define HEVC_NAL_UNIT_ID_VPS 32
#define HEVC_NAL_UNIT_ID_SPS 33
#define HEVC_NAL_UNIT_ID_PPS 34

using namespace std;
using namespace netflix;
using namespace netflix::streamparser;
using namespace netflix::containerlib::mp4parser;

ParserContext::ParserContext(std::map<uint32_t, shared_ptr<TrackHeader> >& trackHeaders,
                             std::map<uint32_t, ase::EditList>& editLists)
  : mTrackHeaders(trackHeaders)
  , mEditLists(editLists)
{

}

bool ParserContext::isProtectionSchemeSupported(uint32_t schemeType)
{
    if(schemeType == 0x70696666) // 'piff'
        return true;
    return false;
}

Context::ParserState ParserContext::onMovieParseEnd(shared_ptr<MovieContext> /*movieContext*/)
{
    return PARSE_STOP;
    //return PARSE_CONTINUE;
}

void ParserContext::setFragmentHeader(shared_ptr<FragmentHeader> fragmentHeaderPtr)
{
    mFragmentHeaderPtr = fragmentHeaderPtr;
}

/*
 * *********
 * V I D E O
 * *********
 */

/*
 * static helper functions for track parsing and header construction
 */
void setTemporalLayerInfo(std::vector<TemporalLayerEntry::Data> temporalLayerEntries,
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
void appendParameterSet(list<shared_ptr<TrackContext::ParameterSet> > parameterSets,
                        vector<unsigned char>& data)
{
    list< shared_ptr<TrackContext::ParameterSet> >::const_iterator iter;

    for(iter  = parameterSets.begin(); iter != parameterSets.end(); iter++) {
        uint32_t psSize = (*iter)->size();
        if(psSize == 0)
            continue;

        /*
        uint32_t naluSize =
        (data[i]     << 24) +
        (data[i + 1] << 16) +
        (data[i + 2] <<  8) +
        data[i + 3];

        data[i]     = 0;
        data[i + 1] = 0;
        data[i + 2] = 0;
        data[i + 3] = 1;
        */
        data.push_back( (psSize >> 24) & 0xff );
        data.push_back( (psSize >> 16) & 0xff );
        data.push_back( (psSize >> 8 ) & 0xff );
        data.push_back( (psSize >> 0 ) & 0xff );


        uint32_t s = data.size();
        data.resize(s + psSize);
        memcpy(&data[s], &(*(*iter))[0], psSize);
    }
}

void appendParameterSet(list<shared_ptr<TrackContext::ParameterSet> >::iterator startIter,
                        list<shared_ptr<TrackContext::ParameterSet> >::iterator endIter,
                        vector<unsigned char>& data)
{
    list< shared_ptr<TrackContext::ParameterSet> >::const_iterator iter;

    for(iter  = startIter; iter != endIter; iter++)
    {
        uint32_t psSize = (*iter)->size();
        if(psSize == 0)
            continue;

        data.push_back( (psSize >> 24) & 0xff );
        data.push_back( (psSize >> 16) & 0xff );
        data.push_back( (psSize >> 8 ) & 0xff );
        data.push_back( (psSize >> 0 ) & 0xff );

        uint32_t s = data.size();
        data.resize(s + psSize);
        memcpy(&data[s], &(*(*iter))[0], psSize);
    }
}

bool verifyAVCParameterSetNaluType(const vector<unsigned char>& paramSet,
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

bool verifyHEVCParameterSetNaluType(const vector<unsigned char>& paramSet,
                                    unsigned char naluTypeCode)
{
    if(paramSet.size() > 0) {
        if( paramSet.size() < 6 ) {
            return false;
        }

        if( ( ( paramSet[4] & 0x7e ) >> 1 ) != naluTypeCode ) {
            NTRACE(TRACE_MEDIAPLAYBACK, "wrong parameterSet:expected %d, actual %d\n", naluTypeCode, (paramSet[4] & 0x7e) >> 1 );
            return false;
        }
    }
    return true;
}

VideoParserContext::VideoParserContext(
    std::map<uint32_t, shared_ptr<TrackHeader> >& videoTrackHeaders,
    std::map<uint32_t, ase::EditList>& videoEditLists)
  : ParserContext(videoTrackHeaders,videoEditLists)
{

}

Context::ParserState VideoParserContext::onTrackParseEnd(shared_ptr<TrackContext> trackContext)
{
#if defined(STREAMPARSER_VERBOSE_LOG)
    NTRACE(TRACE_MEDIAPLAYBACK
           , "VideoMovieBoxParseContext::onTrackParseEnd trackId %d"
           , trackContext->trackID());
#endif

    /*
     * TrackHeader creation
     * - whenever new track is parse, create an instance of VideoTrackHeader
     */
    shared_ptr<VideoTrackHeader> videoTrackHeaderPtr(new VideoTrackHeader);
    videoTrackHeaderPtr->mTrackId = trackContext->trackID();
    mTrackHeaders[trackContext->trackID()] = videoTrackHeaderPtr;


    /*
     * Time information
     */
    ullong duration;
    uint32_t timescale;
    trackContext->getMediaTimeInfo(duration, timescale);
    videoTrackHeaderPtr->mDuration = duration;
    videoTrackHeaderPtr->mTimeScale = timescale;

    /*
     * DRM information
     */
    trackContext->getDefaultDrmInfo(videoTrackHeaderPtr->mDefaultDrmAlgorithmID,
                                    videoTrackHeaderPtr->mDefaultDrmIVSize,
                                    &videoTrackHeaderPtr->mDefaultDrmKID);
#if defined(STREAMPARSER_VERBOSE_LOG)
    NTRACE(TRACE_MEDIAPLAYBACK, "defaultDrmAlgorithmID_ %d,  defaultDrmIVSize_ %d\n",
           videoTrackHeaderPtr->mDefaultDrmAlgorithmID,
           videoTrackHeaderPtr->mDefaultDrmIVSize);
#endif

    /*
     * Video informaton
     */

    /*
     * Summary of how parameter is delivered in mp4 file format
     *
     * When MVC stream with base view and dependent view,
     * AVCDecoderConfigurationRecords should bring informaiton of "base view" only
     */

    /*
    aligned(8) class AVCDecoderConfigurationRecord {
        unsigned int(8) configurationVersion = 1;
        unsigned int(8) AVCProfileIndication;
        unsigned int(8) profile_compatibility;
        unsigned int(8) AVCLevelIndication;
        bit(6) reserved = '111111'b;
        unsigned int(2) lengthSizeMinusOne;
        bit(3) reserved = '111'b;
        unsigned int(5) numOfSequenceParameterSets;
        for (i=0; i< numOfSequenceParameterSets;  i++) {
            unsigned int(16) sequenceParameterSetLength ;
            bit(8*sequenceParameterSetLength) sequenceParameterSetNALUnit;
        }
        unsigned int(8) numOfPictureParameterSets;
        for (i=0; i< numOfPictureParameterSets;  i++) {
            unsigned int(16) pictureParameterSetLength;
            bit(8*pictureParameterSetLength) pictureParameterSetNALUnit;
        }
        if( profile_idc  ==  100  ||  profile_idc  ==  110  ||
            profile_idc  ==  122  ||  profile_idc  ==  144 )
        {
            bit(6) reserved = '111111'b;
            unsigned int(2) chroma_format;
            bit(5) reserved = '11111'b;
            unsigned int(3) bit_depth_luma_minus8;
            bit(5) reserved = '11111'b;
            unsigned int(3) bit_depth_chroma_minus8;
            unsigned int(8) numOfSequenceParameterSetExt;
            for (i=0; i< numOfSequenceParameterSetExt; i++) {
                unsigned int(16) sequenceParameterSetExtLength;
                bit(8*sequenceParameterSetExtLength) sequenceParameterSetExtNALUnit;
            }
        }
    }
    */

    /*
     * When MVC stream with base view and dependent view,
     * MVCDecoderConfigurationRecords should bring informaiton of "dependent
     * view" only. Therefore, PPS of this box should be delivered for dependent
     * view pictures
     */

    /*
    aligned(8) class MVCDecoderConfigurationRecord {
    unsigned int(8) configurationVersion = 1;
    unsigned int(8) AVCProfileIndication;
    unsigned int(8) profile_compatibility;
    unsigned int(8) AVCLevelIndication;
    bit(1) complete_representation;
    bit(1) explicit_au_track;
    bit(4) reserved = '1111'b;
    unsigned int(2) lengthSizeMinusOne;
    bit(1) reserved = '0'b;
    unsigned int(7) numOfSequenceParameterSets;
    for (i=0; i< numOfSequenceParameterSets; i++) {
            unsigned int(16) sequenceParameterSetLength ;
            bit(8*sequenceParameterSetLength) sequenceParameterSetNALUnit;
    }
    unsigned int(8) numOfPictureParameterSets;
    for (i=0; i< numOfPictureParameterSets; i++) {
            unsigned int(16) pictureParameterSetLength;
            bit(8*pictureParameterSetLength) pictureParameterSetNALUnit;
    }
    }
    */

    // fill up VideoTrackHeader!!!
    switch(trackContext->sampleType())
    {
    case _FOURCC_BE_('a','v','c','1'):
        // mFormatID
        videoTrackHeaderPtr->mFormatID = _FOURCC_LE_('a','v','c','1');

        // Common for AVC and MVC
        // Copy SPS and PPS for AVC or MVC base view.
        appendParameterSet(trackContext->sequenceParameterSets(),
                           videoTrackHeaderPtr->mSequenceParameterSet);
        if(verifyAVCParameterSetNaluType(videoTrackHeaderPtr->mSequenceParameterSet, 7) == false){
            Log::error(TRACE_MEDIAPLAYBACK, "Mp4ParseError: AVC sequence parameter set has wrong type");
            //return PARSE_ERROR_DATA;
        }

        appendParameterSet(trackContext->pictureParameterSets(),
                           videoTrackHeaderPtr->mPictureParameterSet);
        if(verifyAVCParameterSetNaluType(videoTrackHeaderPtr->mPictureParameterSet, 8) == false){
            Log::error(TRACE_MEDIAPLAYBACK, "Mp4ParseError: AVC picture parameter set has wrong type");
            //return PARSE_ERROR_DATA;
        }

        // If this is MVC, copy SSPS and PPS for dependent view.
        if(trackContext->mvcSequenceParameterSets().size() > 0) {
            // for MVC specific.
            // information extracted form MVCConfiguration - MVCDecoderConfigurationRecord
            // 1. Copy the SSPS.
            videoTrackHeaderPtr->mSubsequenceParameterSet.clear();
            //printf("size of trackContext->mvcSequenceParameterSets() %d\n", trackContext->mvcSequenceParameterSets().size());
            appendParameterSet(--trackContext->mvcSequenceParameterSets().end(),
                               trackContext->mvcSequenceParameterSets().end(),
                               videoTrackHeaderPtr->mSubsequenceParameterSet);
            if(verifyAVCParameterSetNaluType(
                   videoTrackHeaderPtr->mSubsequenceParameterSet, 15) == false)
            {
                Log::error(TRACE_MEDIAPLAYBACK
                           , "Mp4ParseError: mvc subsequence parameter set has wrong type");
                assert(0);
                return PARSE_ERROR_DATA;
            }

            // Copy the dependent view PPS.
            videoTrackHeaderPtr->mDependentViewPictureParameterSet.clear();
            //printf("size of trackContext->mvcPictureParameterSets() %d\n", trackContext->mvcPictureParameterSets().size());
            appendParameterSet(--trackContext->mvcPictureParameterSets().end(),
                               trackContext->mvcPictureParameterSets().end(),
                               videoTrackHeaderPtr->mDependentViewPictureParameterSet);
            if(verifyAVCParameterSetNaluType(
                   videoTrackHeaderPtr->mDependentViewPictureParameterSet, 8) == false)
            {
                Log::error(TRACE_MEDIAPLAYBACK, "Mp4ParseError: mvc picture "
                           "parameter set has wrong type");
                assert(0);
                return PARSE_ERROR_DATA;
            }

            // Copy naluToMvcViewMaps extracted from scnm. Defining map groups
            // requires that there is a limited number of map grouping patterns
            // for all access units. If there is a varying number of NAL units
            // in successive access units for a given tier, Aggregators can be
            // used to make these varying structures consistent and to reduce
            // the number of map groups required.
            //
            // groupID is from SampleToGroupBox. In MVC case, it will be 0 and 1.
            //
            // - groupID 0 is for IDR. e.g,
            // videoTrackHeaderPtr->mNaluToMvcViewMaps[0] = [0: 0] [1: 1] [2: 1]
            // [3: 0] [4: 0] [5: 0] [6: 0] [7: 0] [8: 0] [9: 0] [10: 0] [11: 0]
            // [12: 0] [13: 0] [14: 0] [15: 1] [16: 1] [17: 1] [18: 1] [19: 1]
            // [20: 1] ( here, [naluNumer : viewID])
            // - groupID 1 is for non-IDR frames. e.g,
            // videoTrackHeaderPtr->mNaluToMvcViewMaps[1] = [0: 0]
            // [1: 1] [2: 0] [3: 0] [4: 0] [5: 0] [6: 0] [7: 0] [8: 0] [9: 0]
            // [10: 0] [11: 0] [12: 0] [13: 0] [14: 1] [15: 1] [16: 1] [17: 1]
            // [18: 1] [19: 1]

            /*
            class ScalableNALUMapEntry() extends VisualSampleGroupEntry ('scnm') {
                unsigned int(8) reserved = 0;
                unsigned int(8) NALU_count;
                for (i=1; i<= NALU_count; i++){
                    unsigned int(8) groupID;
                }
            }
            */

            videoTrackHeaderPtr->mNaluToMvcViewMaps = trackContext->naluToMvcViewMaps();
            /*
            for(int i = 0; i< videoTrackHeaderPtr->mNaluToMvcViewMaps.size(); i++){
                printf("videoTrackHeaderPtr->mNaluToMvcViewMaps[%d] = ", i);
                for(int j = 0; j< videoTrackHeaderPtr->mNaluToMvcViewMaps[i].size(); j++){
                    printf("[%d: %d] ", j, videoTrackHeaderPtr->mNaluToMvcViewMaps[i][j]);
                }
                printf("\n");
            }
            */
            // set codec param for mvc1
            videoTrackHeaderPtr->mCodecParam = "mvc1";
        } else {
            // for AVC specific
            //codecString += "avc1." + bytesToHex(&videoTrackHeaderPtr->mSequenceParameterSet[5], 3);
        }
        break;

    case _FOURCC_BE_('m','p','4','v'):
        // Note: here we assumed that MP4P2 is the only format used as MP4V.
        videoTrackHeaderPtr->mFormatID = _FOURCC_LE_('M','P','4','V');
        videoTrackHeaderPtr->mCodecParam = "mp4v";
        break;

    case _FOURCC_BE_('h','v','c','1'):
        videoTrackHeaderPtr->mFormatID = _FOURCC_LE_('h','v','c','1');
#if defined(STREAMPARSER_VERBOSE_LOG)
        // HEVC has different number for NALs. NAL numbers should not be shared with AVC
        if(trackContext->temporalLayerEntries().size()){
            NTRACE(TRACE_MEDIAPLAYBACK, "Streaming HEVC (hvc1) temporal layered stream with %zu layers",
                   trackContext->temporalLayerEntries().size());
        } else {
            NTRACE(TRACE_MEDIAPLAYBACK, "Streaming HEVC (hvc1) non-temporal layered stream");
        }
#endif
        // Copy VPS
        videoTrackHeaderPtr->mVideoParameterSet.clear();
        appendParameterSet(trackContext->videoParameterSets(),
                           videoTrackHeaderPtr->mVideoParameterSet);
        if(verifyHEVCParameterSetNaluType(videoTrackHeaderPtr->mVideoParameterSet,
                                          HEVC_NAL_UNIT_ID_VPS) == false ) {
            Log::error(TRACE_MEDIAPLAYBACK, "Mp4ParseError: HEVC video parameter set has wrong type");
            return PARSE_ERROR_DATA;
        }
        // Copy SPS
        videoTrackHeaderPtr->mSequenceParameterSet.clear();
        appendParameterSet(trackContext->sequenceParameterSets(), videoTrackHeaderPtr->mSequenceParameterSet);
        if(verifyHEVCParameterSetNaluType(videoTrackHeaderPtr->mSequenceParameterSet,
                                          HEVC_NAL_UNIT_ID_SPS) == false ) {
            Log::error(TRACE_MEDIAPLAYBACK, "Mp4ParseError: HEVC sequence parameter set has wrong type");
            return PARSE_ERROR_DATA;
        }
        // Copy PPS
        videoTrackHeaderPtr->mPictureParameterSet.clear();
        appendParameterSet(trackContext->pictureParameterSets(), videoTrackHeaderPtr->mPictureParameterSet);
        if(verifyHEVCParameterSetNaluType(videoTrackHeaderPtr->mPictureParameterSet,
                                          HEVC_NAL_UNIT_ID_PPS) == false ) {
            Log::error(TRACE_MEDIAPLAYBACK, "Mp4ParseError: HEVC picture parameter set has wrong type");
            return PARSE_ERROR_DATA;
        }

        videoTrackHeaderPtr->mCodecParam = "hvc1";
        // + "." + profile_space in no_charactor, 1, 2, 3
        // + "." + profile_idc in decimal
        // + "." + profile_compatibility indications in hex
        // + "." + tier_flag in "L" or "H"
        // + "." + level_idc in decimal number
        // + "." ....
        break;

    case _FOURCC_BE_('h','e','v','1'):
        videoTrackHeaderPtr->mFormatID = _FOURCC_LE_('h','e','v','1');
        // hev1 carries parameter sets in band

#if defined(STREAMPARSER_VERBOSE_LOG)
        if(trackContext->temporalLayerEntries().size()){
            NTRACE(TRACE_MEDIAPLAYBACK, "Streaming HEVC (hev1) temporal layered stream with %zu layers",
                   trackContext->temporalLayerEntries().size());
        } else {
            NTRACE(TRACE_MEDIAPLAYBACK, "Streaming HEVC (hev1) non-temporal layered stream");
        }
#endif
        videoTrackHeaderPtr->mVideoParameterSet.clear();
        videoTrackHeaderPtr->mSequenceParameterSet.clear();
        videoTrackHeaderPtr->mPictureParameterSet.clear();
        videoTrackHeaderPtr->mCodecParam = "hev1.";
        break;

    case _FOURCC_BE_('d','v','h','e'):
        videoTrackHeaderPtr->mFormatID = _FOURCC_LE_('d','v','h','e');
        // hev1 carries parameter sets in band
        videoTrackHeaderPtr->mVideoParameterSet.clear();
        videoTrackHeaderPtr->mSequenceParameterSet.clear();
        videoTrackHeaderPtr->mPictureParameterSet.clear();
        videoTrackHeaderPtr->mCodecParam = "dvhe";
        break;

    default:
        break;
    };

    trackContext->getVideoImageSize(videoTrackHeaderPtr->mImageWidth,
                                    videoTrackHeaderPtr->mImageHeight);
    trackContext->getVideoPixelAspectRatio(videoTrackHeaderPtr->mPixelAspectRatioX,
                                           videoTrackHeaderPtr->mPixelAspectRatioY);
    trackContext->getVideoFrameRate(videoTrackHeaderPtr->mFrameRateValue,
                                    videoTrackHeaderPtr->mFrameRateScale);
    trackContext->getColorInformation(videoTrackHeaderPtr->mColorPrimaries,
                                      videoTrackHeaderPtr->mTransferCharacteristics,
                                      videoTrackHeaderPtr->mMatrixCoefficients,
                                      videoTrackHeaderPtr->mFullRange);
    trackContext->getVideoBitsDepth(videoTrackHeaderPtr->mBitsDepthLuma,
                                    videoTrackHeaderPtr->mBitsDepthChroma);
    trackContext->getGeneralLevelIdc(videoTrackHeaderPtr->mGeneralLevelIdc);
    setTemporalLayerInfo(trackContext->temporalLayerEntries(), videoTrackHeaderPtr->mTemporalLayerEntryInfo);
    if( videoTrackHeaderPtr->mTemporalLayerEntryInfo.size() > 1 )
    {
        videoTrackHeaderPtr->mTemporalLayerStream = true;
    }

    NTRACE(TRACE_MEDIAPLAYBACK, "mImageWidth %d", videoTrackHeaderPtr->mImageWidth);
    NTRACE(TRACE_MEDIAPLAYBACK, "mImageHeight %d", videoTrackHeaderPtr->mImageHeight);
    NTRACE(TRACE_MEDIAPLAYBACK, "mBitsDepthLuma %d", videoTrackHeaderPtr->mBitsDepthLuma);
    NTRACE(TRACE_MEDIAPLAYBACK, "mBitsDepthChroma %d", videoTrackHeaderPtr->mBitsDepthChroma);
    NTRACE(TRACE_MEDIAPLAYBACK, "mPixelAspectRatioX %d", videoTrackHeaderPtr->mPixelAspectRatioX);
    NTRACE(TRACE_MEDIAPLAYBACK, "mPixelAspectRatioY %d", videoTrackHeaderPtr->mPixelAspectRatioY);
    NTRACE(TRACE_MEDIAPLAYBACK, "mFrameRateValue %d", videoTrackHeaderPtr->mFrameRateValue);
    NTRACE(TRACE_MEDIAPLAYBACK, "mFrameRateScale %d", videoTrackHeaderPtr->mFrameRateScale);
    NTRACE(TRACE_MEDIAPLAYBACK, "mColorPrimaries %d", videoTrackHeaderPtr->mColorPrimaries);
    NTRACE(TRACE_MEDIAPLAYBACK, "mTransferCharacteristics %d", videoTrackHeaderPtr->mTransferCharacteristics);
    NTRACE(TRACE_MEDIAPLAYBACK, "mMatrixCoefficients %d", videoTrackHeaderPtr->mMatrixCoefficients);
    NTRACE(TRACE_MEDIAPLAYBACK, "mFullRange %d", videoTrackHeaderPtr->mFullRange);
    NTRACE(TRACE_MEDIAPLAYBACK, "mTimeScale %d", videoTrackHeaderPtr->mTimeScale);
    NTRACE(TRACE_MEDIAPLAYBACK, "mGeneralLevelIdc %d", videoTrackHeaderPtr->mGeneralLevelIdc);
    NTRACE(TRACE_MEDIAPLAYBACK, "mNominalBitrateInKbps %d", videoTrackHeaderPtr->mNominalBitrateInKbps);
    NTRACE(TRACE_MEDIAPLAYBACK, "mVideoTrackHeaders.size() %zu", mTrackHeaders.size());

    /*
     * EditList construction
     */
    ase::EditList editList;
    uint32_t editListSize = trackContext->editListEntries().size();

    NTRACE(TRACE_MEDIAPLAYBACK, "editList Size %d", editListSize);

    for(uint32_t i=0; i<editListSize; i++){
        TrackContext::EditListEntry& editListEntry = trackContext->editListEntries()[i];
        editList.add(editListEntry.segment_duration_,
                     editListEntry.media_time_,
                     editListEntry.media_rate_integer_ == 0);
        NTRACE(TRACE_MEDIAPLAYBACK, "editList segment_duration_ %llu, media_time %llu, media_rate_integer_ %d"
               , (ullong)editListEntry.segment_duration_
               , (ullong)editListEntry.media_time_
               , editListEntry.media_rate_integer_);
    }

    NTRACE(TRACE_MEDIAPLAYBACK, "editList timescale %d", timescale);

    editList.setTimescale(timescale);
    mEditLists[trackContext->trackID()] = editList;

    return PARSE_CONTINUE;
}

/*
 * *********
 * A U D I O
 * *********
 */
AudioParserContext::AudioParserContext(
    std::map<uint32_t, shared_ptr<TrackHeader> >& audioTrackHeaders,
    std::map<uint32_t, ase::EditList>& audioEditLists)
  : ParserContext(audioTrackHeaders,audioEditLists)
{

}

bool createADTSHeader(const vector<uint8_t>& codecSpecificData,
                      vector<unsigned char>& ADTSHeader)
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

    ADTSHeader.clear();
    ADTSHeader.push_back(0xff);
    ADTSHeader.push_back(ID ? 0xf9 : 0xf1);
    ADTSHeader.push_back(
        0x40 | (samplingFrequencyIndex << 2) | (channelConfiguration >> 2));
    ADTSHeader.push_back((channelConfiguration & 0x3) << 6);
    ADTSHeader.push_back(0);
    ADTSHeader.push_back(0x1f);
    ADTSHeader.push_back(0xfc);

    return true;

}

Context::ParserState AudioParserContext::onTrackParseEnd(shared_ptr<TrackContext> trackContext)
{
#if defined(STREAMPARSER_VERBOSE_LOG)
    NTRACE(TRACE_MEDIAPLAYBACK, "AudioMovieBoxParseContext::onTrackParseEnd\n");
#endif

    // whenever new track is parse, create an instance of AudioTrackHeader
    shared_ptr<AudioTrackHeader> audioTrackHeaderPtr(new AudioTrackHeader);
    // once track header information is filled up from trak box, add header
    audioTrackHeaderPtr->mTrackId = trackContext->trackID();
    mTrackHeaders[trackContext->trackID()] = audioTrackHeaderPtr;

    /*
     * Time information
     */
    ullong duration;
    uint32_t timescale;
    trackContext->getMediaTimeInfo(duration, timescale);
    audioTrackHeaderPtr->mDuration = duration;
    audioTrackHeaderPtr->mTimeScale = timescale;

    /*
     * DRM information
     */
    getDefaultDrmInfo(audioTrackHeaderPtr->mDefaultDrmAlgorithmID,
                      audioTrackHeaderPtr->mDefaultDrmIVSize,
                      &audioTrackHeaderPtr->mDefaultDrmKID);

    /*
     * Codec information
     */
    switch(trackContext->sampleType())
    {
    case _FOURCC_BE_('m','p','4','a'):
        // Note: here we assumed that HE-AAC is the only format used
        // as MP4A.
        audioTrackHeaderPtr->mFormatID = 0x1610; // MEDIASUBTYPE_MPEG_HEAAC
        break;

    case _FOURCC_BE_('e','c','-','3'):
        audioTrackHeaderPtr->mFormatID = 0xEAC3; // DOLBY DIGITAL PLUS
        break;

    case _FOURCC_BE_('o','v','r','b'):
        // Note: we don't really know which version of Vorbis it
        // is. Assume that it's always 3+.
        audioTrackHeaderPtr->mFormatID = 0x6771; // OGG VORBIS 3 PLUS
        break;
    }
    trackContext->getAudioSampleInfo(audioTrackHeaderPtr->mNumberOfChannels,
                                     audioTrackHeaderPtr->mBitsPerSample,
                                     audioTrackHeaderPtr->mSamplesPerSecond);
    audioTrackHeaderPtr->mSamplesPerSecond >>= 16;

    audioTrackHeaderPtr->mCodecSpecificData = trackContext->decoderSpecificData();


    uint32_t maximumBitrateInKbps;
    trackContext->getAudioElementaryStreamData(maximumBitrateInKbps,
                                               audioTrackHeaderPtr->mNominalBitrateInKbps);

    switch (audioTrackHeaderPtr->mFormatID) {
    case 0x1610: // MEDIASUBTYPE_MPEG_HEAAC

        // Note: this is a temporary workaround.
        //
        // For HE-AAC streams, the sampling frequency information from ISO
        // AudioSampleEntry is actually the sampling frequency when it is
        // treated as a LC-AAC stream, which is 1/2 of the actual frequency
        // if treated as HE-AAC.
        audioTrackHeaderPtr->mSamplesPerSecond *= 2;
        createADTSHeader(audioTrackHeaderPtr->mCodecSpecificData, audioTrackHeaderPtr->mADTSHeader);

        audioTrackHeaderPtr->mNominalBitrateInKbps >>= 10;
        break;
    case 0xEAC3:  // DOLBY DIGITAL PLUS
        break;
    default:
        audioTrackHeaderPtr->mNominalBitrateInKbps = 0;
    }

    audioTrackHeaderPtr->mBytesPerSecond = (audioTrackHeaderPtr->mSamplesPerSecond *
                                            audioTrackHeaderPtr->mBitsPerSample) > 3;

    return PARSE_CONTINUE;
}

Context::ParserState ParserContext::onTrackFragmentParseEnd(shared_ptr<TrackFragmentContext> trackFragmentContext)
{
#if defined(STREAMPARSER_VERBOSE_LOG)
    NTRACE(TRACE_MEDIAPLAYBACK, "onTrackFragmentParseEnd\n");
    NTRACE(TRACE_MEDIAPLAYBACK, "entry size: sample %d",trackFragmentContext->sampleEntries().size());
    NTRACE(TRACE_MEDIAPLAYBACK, "entry size: drm info %d",trackFragmentContext->drmInfoEntries().size());
    NTRACE(TRACE_MEDIAPLAYBACK, "entry size: enc map %d",trackFragmentContext->encryptionMapEntries().size());
    NTRACE(TRACE_MEDIAPLAYBACK, "track algorithm id %d\n",trackFragmentContext->drmAlgorithmID());
#endif

    uint32_t trackId = trackFragmentContext->trackID();
    mFragmentHeaderPtr->mTrackId = trackId;

    std::map<uint32_t, shared_ptr<TrackHeader> >::iterator trackHeaderIt = mTrackHeaders.find(trackId);
    if (trackHeaderIt == mTrackHeaders.end()) {
        Log::error(TRACE_MEDIAPLAYBACK, "trackId %d does not exist", trackId);
        return PARSE_ERROR_DATA;
    }

    uint32_t timescale = trackHeaderIt->second->mTimeScale;
    mFragmentHeaderPtr->mTimescale = timescale;

    bool editlistExist = false;
    std::map<uint32_t, ase::EditList>::iterator cit = mEditLists.find(trackId);
    if(cit != mEditLists.end()){
        editlistExist = true;
    }

    bool encrypted = trackFragmentContext->encryptionEntries().size() > 0 ;

    uint32_t firstSampleCompositionTimeOffset
        = trackFragmentContext->sampleEntries()[0].sampleCompositionTimeOffset_;

    int64_t currentTimeOffset = (int64_t)(mFragmentHeaderPtr->mCurrentTimestampOffset) * 1000;
    int64_t currentTimeOffsetTimescale = (int64_t)(mFragmentHeaderPtr->mCurrentTimestampOffsetTimescale);
    int64_t signedPts = 0, signedDts = 0, biggestPts = 0;

    NTRACE(TRACE_MEDIAPLAYBACK, "time offset %d\t timescale passed %u,\t timescale %d adjusted offset %lld\n"
           , mFragmentHeaderPtr->mCurrentTimestampOffset
           , mFragmentHeaderPtr->mCurrentTimestampOffsetTimescale
           , timescale
           , (llong)(currentTimeOffset/currentTimeOffsetTimescale));

    for(uint32_t i = 0 ; i< trackFragmentContext->sampleEntries().size() ; i++){
        TrackFragmentContext::SampleEntry& sampleEntry = (trackFragmentContext->sampleEntries())[i];
        Sample sample;
        sample.mFragmentOffset = sampleEntry.startOffset_;
        sample.mSize = sampleEntry.sampleSize_ ;
        sample.mSampleGroup = sampleEntry.sampleGroup_;
        mFragmentHeaderPtr->mTotalSampleSize += sample.mSize;
        //NTRACE(TRACE_MEDIAPLAYBACK,
        //"MediaSourceStreamParser: totalSampleSize %lld", mFragmentHeaderPtr->mTotalSampleSize);


        /*
        if (encrypted)
            printf("sample group %d, sampleSize %d\n", sampleEntry.sampleGroup_, sampleEntry.sampleSize_);
        */

        /*
          aligned(8) class SampleDependencyTypeBox extends FullBox('sdtp', version = 0, 0) {
          for (i=0; i < sample_count; i++){
          unsigned int(2) is_leading;
          unsigned int(2) sample_depends_on;
          unsigned int(2) sample_is_depended_on;
          unsigned int(2) sample_has_redundancy;
          }}
        */
        const uint8_t flags = sampleEntry.independentAndDisposableFlags_;
        if(0 == flags){
            // 0 means that, dependency is unknown. conservatively, don't throw
            sample.mIsReferenced = true;
        } else {
            // having 3rd bit set (b100) means that, sample_is_dependent_on is "not disposable"
            sample.mIsReferenced = ((flags & 0x04) != 0);
        }

        /*
         * **********************
         * Timestamp construction
         */
        uint64_t cts;
        ase::AseTimeStamp pts;
        // PTS
        if(editlistExist){
            cts = (trackFragmentContext->getBaseMediaDecodeTime()
                   + sampleEntry.DTS_
                   + sampleEntry.sampleCompositionTimeOffset_);
            cit->second.map( ase::AseTimeStamp( cts, timescale ), pts );
            sample.mPts = pts.getValueInMs();
        } else {
            // subtract firstSampleCompositionTimeOffset from pts in order for
            // PTS to have 0 starting value
            sample.mPts =  (trackFragmentContext->getBaseMediaDecodeTime()
                            + sampleEntry.DTS_
                            + sampleEntry.sampleCompositionTimeOffset_
                            - firstSampleCompositionTimeOffset);
            sample.mPts *= 1000;
            sample.mPts /= timescale;
        }

        // DTS, with firstSampleCompositionTimeOffset subtracted for PTS, DTS
        // also need same subtraction. This will cause first DTS being negative
        // value.
        sample.mDts = ( trackFragmentContext->getBaseMediaDecodeTime()
                        + sampleEntry.DTS_
                        - firstSampleCompositionTimeOffset );
        sample.mDts *= 1000;  // mp4 file format's timescale is for sec, while ours is ms. So, x1000 is needed
        sample.mDts /= timescale;

        // PTS/DTS adjustment for segmented playlist
#if defined(STREAMPARSER_VERBOSE_LOG)
        NTRACE(TRACE_MEDIAPLAYBACK, "before adjusting pts %lld \t dts %lld \t with %lld"
               , sample.mPts, sample.mDts
               , currentTimeOffset/currentTimeOffsetTimescale);
#endif

        signedPts = (int64_t)sample.mPts + currentTimeOffset/currentTimeOffsetTimescale;
        sample.mPts = (uint64_t)signedPts;
        if(sample.mPts >= biggestPts){
            biggestPts = sample.mPts;
        }
        signedDts = (int64_t)sample.mDts + currentTimeOffset/currentTimeOffsetTimescale;
        sample.mDts = (uint64_t)signedDts;

#if defined(STREAMPARSER_VERBOSE_LOG)
        NTRACE(TRACE_MEDIAPLAYBACK, "Sample: size %d, fragmentOffset %d, pts %lld, dts %lld"
               ,sample.mSize, sample.mFragmentOffset, sample.mPts, sample.mDts);
        NTRACE(TRACE_MEDIAPLAYBACK
               ,"adjusted pts %lld, dts %lld, basetime %lld, timescale %d, sampleCompositionTimeOffset %d firstSampleTimeOffset %d\n"
               ,sample.mPts, sample.mDts, trackFragmentContext->getBaseMediaDecodeTime()
               ,timescale, sampleEntry.sampleCompositionTimeOffset_, firstSampleCompositionTimeOffset);
#endif

        /*
         * ******************************
         * Drm and subsamplemap construction
         */
        if( encrypted ){

            TrackFragmentContext::EncryptionEntry& encryptionEntry = (trackFragmentContext->encryptionEntries())[i];
            sample.mIV = encryptionEntry.IV_;
#if defined(STREAMPARSER_VERBOSE_LOG)
            NTRACE(TRACE_MEDIAPLAYBACK, "IV:\n");
            for(uint32_t j=0; j<sample.mIV.size(); j++){
                NTRACE(TRACE_MEDIAPLAYBACK, "0x%x ", sample.mIV[j]);
            }
            NTRACE(TRACE_MEDIAPLAYBACK, "\nsubSampleMap\n");
#endif
            for(uint32_t k=0; k<encryptionEntry.subSampleMap_.size(); k++){
#if defined(STREAMPARSER_VERBOSE_LOG)
                NTRACE(TRACE_MEDIAPLAYBACK, "[%d:%d],",
                       encryptionEntry.subSampleMap_[k].bytesOfClearData_,
                       encryptionEntry.subSampleMap_[k].bytesOfEncryptedData_);
#endif
                sample.mSubSampleMap.push_back(encryptionEntry.subSampleMap_[k].bytesOfClearData_);
                sample.mSubSampleMap.push_back(encryptionEntry.subSampleMap_[k].bytesOfEncryptedData_);
            }
            //NTRACE(TRACE_MEDIAPLAYBACK, "\n");

        }
        /*
         * push_back Sample structure into FragmentHeader
         */
        mFragmentHeaderPtr->mSamples.push_back(sample);
    }

    uint64_t lastSampleDuration = trackFragmentContext->sampleEntries().back().sampleDuration_ * 1000 / timescale;
    mFragmentHeaderPtr->mStartPts = ase::AseTimeStamp(mFragmentHeaderPtr->mSamples.front().mPts, 1000);
    mFragmentHeaderPtr->mEndPts = ase::AseTimeStamp(biggestPts + lastSampleDuration, 1000);
    mFragmentHeaderPtr->mSampleDuration = trackFragmentContext->sampleEntries().front().sampleDuration_;

#if defined(STREAMPARSER_VERBOSE_LOG)
    NTRACE(TRACE_MEDIAPLAYBACK, "size of samples %zu (%lld ms - %lld ms)\n",
           mFragmentHeaderPtr->mSamples.size(),
           mFragmentHeaderPtr->mStartPts.getValueInMs(),
           mFragmentHeaderPtr->mEndPts.getValueInMs());
#endif

    mFragmentHeaderPtr->mExpectedMaxBoxSize = trackFragmentContext->getMaxDataOffset();

    return PARSE_STOP;

}

