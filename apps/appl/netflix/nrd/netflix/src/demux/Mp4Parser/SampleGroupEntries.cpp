/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#include "Reader.h"
#include "Dumper.h"
#include "macros.h"
#include "Context.h"

#include "SampleGroupEntries.h"

using namespace std;
using namespace netflix::containerlib::mp4parser;

/*******************************************************************************
 * Table of Box Types
 */

/*******************************************************************************
 * TemporalLevelEntry (tele)
 * ISO 14496-12. chapter 10.5
 */
void TemporalLevelEntry::clearSpecifics()
{

}

bool TemporalLevelEntry::readSpecifics(Reader& reader, Context& context, uint32_t entry)
{
    reader.read(data_.level_independently_decodable_, 1);
    PRINT("-----------------------------------------------%s","\n");
    PRINT("level_independently_decodable_ %d\n", data_.level_independently_decodable_);

    reader.read(data_.reserved_, 7);

    context.currentTrackContext()->temporalLevelEntries()[entry] = data_;
    return reader.good();
}

void TemporalLevelEntry::dumpSpecifics(Dumper& /*dumper*/) const
{

}

/*******************************************************************************
 * HEVC related sample groups
 */
//DEFINE_STD_BOXTYPE(SyncSampleEntryBox, "sync");
//DEFINE_STD_BOXTYPE(TemporalLayerEntryBox, "tscl");


/*******************************************************************************
 * SyncSampleEntryBox
 */
void SyncSampleEntry::clearSpecifics()
{

}

bool SyncSampleEntry::readSpecifics(Reader& reader, Context& context, uint32_t entry)
{
    (void)context;
    (void)entry;

    reader.skipBits(2); // reserved = '00'b;
    reader.read(data_.NAL_unit_type_, 6);

    if(!reader.good())
        return false;

    return true;
}

void SyncSampleEntry::dumpSpecifics(Dumper& dumper) const
{
    DUMP(data_.NAL_unit_type_);
}

/*******************************************************************************
 * TemporalLayerEntryBox
 */
void TemporalLayerEntry::clearSpecifics()
{

}

bool TemporalLayerEntry::readSpecifics(Reader& reader, Context& context, uint32_t entry)
{
    reader.read(data_.temporalLayerId_, 8);
    reader.read(data_.tlprofile_spaces_, 2);
    reader.read(data_.tltier_flag_, 1);
    reader.read(data_.tlprofile_idc_, 5);
    reader.read(data_.tlprofile_compatibility_indications_, 32);
    reader.read(data_.tlconstraint_indicator_flags_, 48);
    reader.read(data_.tllevel_idc_, 8);
    reader.read(data_.tlMaxBitRate_, 16);
    reader.read(data_.tlAvgBitrate_, 16);
    reader.read(data_.tlConstantFrameRate_, 8);
    reader.read(data_.tlAvgFrameRate_, 16);

    PRINT("-----------------------------------------------%s","\n");
    PRINT("temporalLayerId\t %d\n", data_.temporalLayerId_ );
    PRINT("tlprofile_spaces_ \t %d\n",data_.tlprofile_spaces_);
    PRINT("tltier_flag_ \t %d\n",data_.tltier_flag_);
    PRINT("tlprofile_idc_ \t %d\n",data_.tlprofile_idc_);
    PRINT("tlprofile_compatibility_indications_ \t %d\n",data_.tlprofile_compatibility_indications_);
    PRINT("tlconstraint_indicator_flags_ \t %lld\n",data_.tlconstraint_indicator_flags_);
    PRINT("tllevel_idc_ \t %d\n",data_.tllevel_idc_);
    PRINT("tlMaxBitRate_ \t %d\n",data_.tlMaxBitRate_);
    PRINT("tlAvgBitrate_ \t %d\n",data_.tlAvgBitrate_);
    PRINT("tlConstantFrameRate_ \t %d\n",data_.tlConstantFrameRate_);
    PRINT("tlAvgFrameRate_ \t %d\n",data_.tlAvgFrameRate_);

    if(!reader.good())
        return false;
    context.currentTrackContext()->temporalLayerEntries()[entry] = data_;

    return true;
}

void  TemporalLayerEntry::dumpSpecifics(Dumper& dumper) const
{
    DUMP(data_.temporalLayerId_);
    DUMP(data_.tlprofile_spaces_);
    DUMP(data_.tltier_flag_);
    DUMP(data_.tlprofile_idc_);
    DUMP(data_.tlprofile_compatibility_indications_);
    DUMP(data_.tlconstraint_indicator_flags_);
    DUMP(data_.tllevel_idc_);
    DUMP(data_.tlMaxBitRate_);
    DUMP(data_.tlAvgBitrate_);
    DUMP(data_.tlConstantFrameRate_);
    DUMP(data_.tlAvgFrameRate_);
}

/*******************************************************************************
 * MVC related sample groups
 */
//DEFINE_STD_BOXTYPE(SampleMapGroupEntryBox, "scnm");
//DEFINE_STD_BOXTYPE_END(SampleMapGroupEntryBox, "scnm");

ScalableNALUMapEntry::~ScalableNALUMapEntry()
{

}

void ScalableNALUMapEntry::clearSpecifics()
{

}

bool ScalableNALUMapEntry::readSpecifics(Reader& reader, Context& context, uint32_t entry)
{
    int j;
    char reserved, NALU_count, groupID;
    reader.read(reserved, 8);
    reader.read(NALU_count, 8);

    for(j = 0; j < NALU_count; j++) {
        reader.read(groupID, 8);
        context.currentTrackContext()->naluToMvcViewMaps()[entry].push_back(groupID);
    }

    return reader.good();
}

void ScalableNALUMapEntry::dumpSpecifics(Dumper& /*dumper*/) const
{

}


