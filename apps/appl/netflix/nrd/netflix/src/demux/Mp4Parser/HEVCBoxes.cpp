/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "HEVCBoxes.h"

#include "Context.h"

#include "Reader.h"
#include "Dumper.h"
#include "macros.h"

#include <stdio.h>

using namespace std;
using namespace netflix::containerlib::mp4parser;

/*******************************************************************************
 * Table of Box Types
 */
DEFINE_STD_BOXTYPE(HEVCConfigurationBox, "hvcC");

/*******************************************************************************
 * HEVCConfigurationBox
 */

DEFINE_CHILD_BOXES(HEVCConfigurationBox);
DEFINE_CHILD_BOXES_END(HEVCConfigurationBox);

void HEVCConfigurationBox::clearSpecifics()
{
    childBoxes_.clear();
}

bool HEVCConfigurationBox::readSpecifics(Reader& reader, Context& context)
{
    uint16_t reserved;

    PRINT("HEVCConfigurationBox::%s\n", __func__);

    reader.read(configurationVersion_, 8);
    PRINT("configurationVersion_ %d\n", configurationVersion_);

    reader.read(profile_space_, 2);
    PRINT("profile_space_ %d\n",profile_space_ );

    reader.read(tier_flag_, 1);
    PRINT("tier_flag_ %d\n", tier_flag_);

    reader.read(profile_idc_, 5);
    PRINT("profile_idc_ %d\n", profile_idc_);

    reader.read(profile_compatibility_indications_, 32);
    PRINT("profile_compatibility_indications_ %lld\n", profile_compatibility_indications_);

    reader.read(progressive_source_flag_, 1);
    PRINT("progressive_source_flag_ %d\n", progressive_source_flag_);

    reader.read(interlaced_source_flag_, 1);
    PRINT("interlaced_source_flag_ %d\n", interlaced_source_flag_);

    reader.read(non_packed_constraint_flag_, 1);
    PRINT("non_packed_constraint_flag_ %d\n",non_packed_constraint_flag_ );

    reader.read(frame_only_constraint_flag_, 1);
    PRINT("frame_only_constraint_flag_ %d\n", frame_only_constraint_flag_);

    reader.read(constraint_indicator_flags_, 44);
    PRINT("constraint_indicator_flags_ %lld\n", constraint_indicator_flags_);

    reader.read(level_idc_, 8);
    PRINT("level_idc_ %d\n",level_idc_ );

    // bit(4) reserved = '1111'b;
    reader.read(reserved, 4);
    PRINT("reserved 0x%x\n", reserved);

    reader.read(min_spatial_segmentation_idc_, 12);
    PRINT("min_spatial_segmentation_idc_ %d\n",min_spatial_segmentation_idc_ );

    // bit(6) reserved = '111111'b;
    reader.read(reserved, 6);
    PRINT("reserved 0x%x\n", reserved);

    reader.read(parallelismType_, 2);
    PRINT("parallelismType_ %d\n", parallelismType_);

    // bit(6) reserved = '111111'b;
    reader.read(reserved, 6);
    PRINT("reserved 0x%x\n", reserved);

    reader.read(chromaFormat_, 2);
    PRINT("chromaFormat_ %d\n", chromaFormat_);

    // bit(5) reserved = '11111'b;
    reader.read(reserved, 5);
    PRINT("reserved 0x%x\n", reserved);

    reader.read(bitDepthLumaMinux8_, 3);
    PRINT("bitDepthLumaMinux8_ %d\n", bitDepthLumaMinux8_);

    // bit(5) reserved = '11111'b;
    reader.read(reserved, 5);
    PRINT("reserved 0x%x\n", reserved);

    reader.read(bitDepthChromaMinux8_, 3);
    PRINT("bitDepthChromaMinux8_ %d\n", bitDepthChromaMinux8_);

    reader.read(avgFrameRate_, 16);
    PRINT("avgFrameRate_ %d\n",avgFrameRate_ );

    reader.read(constantFrameRate_, 2);
    PRINT("constantFrameRate_ %d\n", constantFrameRate_);

    reader.read(numTemporalLayers_, 3);
    PRINT("numTemporalLayers_ %d\n",numTemporalLayers_ );

    reader.read(temporalIdNested_, 1);
    PRINT("temporalIdNested_ %d\n", temporalIdNested_);

    reader.read(lengthSizeMinusOne_, 2);
    PRINT("lengthSizeMinusOne_ %d\n",lengthSizeMinusOne_ );

    reader.read(numOfArrays_, 8);
    PRINT("numOfArrays %d\n", numOfArrays_ );

    if(!reader.good())
        return false;

    // ISO-14496-15, HEVCDecoderConfigurationRecords's avgFrameRate is unit of 256
    context.currentTrackContext()->setGeneralLevelIdc(level_idc_);
    context.currentTrackContext()->setVideoFrameRate(avgFrameRate_, 256);
    context.currentTrackContext()->setNALULengthSize(lengthSizeMinusOne_ + 1);
    context.currentTrackContext()->setVideoBitsDepth(bitDepthLumaMinux8_ + 8, bitDepthChromaMinux8_ +8 );

    for ( uint32_t i = 0; i < numOfArrays_ ; i++ ) {
        reader.read(array_completeness_, 1);
        reader.skipBits(1); // bit(5) reserved = '0'b;
        reader.read(NAL_unit_type_, 6);
        PRINT("NAL_unit_type_ %d\n", NAL_unit_type_ );
        reader.read(numNalus_, 16);

        for ( uint32_t j = 0 ; j< numNalus_ ; j++ ) {
            // ParameterSet is vector<uin8_t>. This is basically for a single NAL unit
            // One kind of ParameterSets can be composed of multiple NALs.
            // So, parameterSet should be pushed into as many as numNalus.
            shared_ptr<TrackContext::ParameterSet> parameterSet(new TrackContext::ParameterSet);
            // this is single type of NAL data with multiple NALs
            uint16_t length = 0;
            reader.read(length, 16);
            PRINT("NAL length %d\n", length );
            parameterSet->resize(length);
            reader.readBytes(*parameterSet);

            // copy parameter set read into stack to context object
            switch(NAL_unit_type_){
            case 32: //VPS
                context.currentTrackContext()->videoParameterSets().push_back(parameterSet);
                break;

            case 33: //SPS
                context.currentTrackContext()->sequenceParameterSets().push_back(parameterSet);
                break;

            case 34: //PPS
                context.currentTrackContext()->pictureParameterSets().push_back(parameterSet);
                break;

            case 39: // PREFIX_SEI
            case 40: // SUFFIX_SEI
                context.currentTrackContext()->supplementalEnhancementInfos().push_back(parameterSet);
                break;

            default:
                break;
            }
        }

        // check point
        if(!reader.good())
            return false;

    }

    return readBoxes(context, CHILD_BOXES(HEVCConfigurationBox), childBoxes_, reader);
}

void HEVCConfigurationBox::dumpSpecifics(Dumper& dumper) const
{
    DUMP(configurationVersion_);
    DUMP(profile_space_);
    DUMP(tier_flag_);
    DUMP(profile_idc_);
    DUMP(profile_compatibility_indications_);
    DUMP(progressive_source_flag_);
    DUMP(interlaced_source_flag_);
    DUMP(non_packed_constraint_flag_);
    DUMP(frame_only_constraint_flag_);
    DUMP(constraint_indicator_flags_);
    DUMP(level_idc_);
    DUMP(min_spatial_segmentation_idc_);
    DUMP(parallelismType_);
    DUMP(chromaFormat_);
    DUMP(bitDepthLumaMinux8_);
    DUMP(bitDepthChromaMinux8_);
    DUMP(avgFrameRate_);
    DUMP(constantFrameRate_);
    DUMP(numTemporalLayers_);
    DUMP(temporalIdNested_);
    DUMP(lengthSizeMinusOne_);
    dumpBoxes(childBoxes_, dumper);
}

