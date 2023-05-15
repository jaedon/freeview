/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef __NETFLIX_CONTAINERLIB_MP4PARSER_SAMPLE_GROUP_ENTRIES__
#define __NETFLIX_CONTAINERLIB_MP4PARSER_SAMPLE_GROUP_ENTRIES__

namespace netflix {
namespace containerlib {
namespace mp4parser {

class Context;
class Reader;
class Dumper;

/*******************************************************************************
 * All sample group entries
 */

/*******************************************************************************
 * SampleGroupDescriptionEntry : abstract entry definition
 */
class SampleGroupDescriptionEntry
{
public:
    virtual ~SampleGroupDescriptionEntry(){};
    virtual void clearSpecifics() = 0;
    virtual bool readSpecifics(Reader& reader, Context& context, uint32_t entry) = 0;
    virtual void dumpSpecifics(Dumper& dumper) const = 0;

private:
};

/*******************************************************************************
 * VisualSampleGroupEntry : abstract entry definition
 */

class VisualSampleGroupEntry : public SampleGroupDescriptionEntry
{

};


/*******************************************************************************
 * TemporalLevelEntry (tele)
 * ISO 14496-12. chapter 10.5
 */
class TemporalLevelEntry : public VisualSampleGroupEntry
{
public:
    virtual void clearSpecifics();
    virtual bool readSpecifics(Reader& reader, Context& context, uint32_t entry);
    virtual void dumpSpecifics(Dumper& /*dumper*/) const;

    struct Data
    {
        /* level_independently_decodable_
         * 1 : all sample of this level have no coding depenency
         * 0 : no information is provided
         */
        uint16_t level_independently_decodable_;
        uint16_t reserved_;
    };

    struct Data data_;
};

/*******************************************************************************
 * HEVC sample group entries
 */

/*******************************************************************************
 * SyncSampleEntry
 *
 */
class SyncSampleEntry : public VisualSampleGroupEntry
{
public:
    virtual void clearSpecifics();
    virtual bool readSpecifics(Reader& reader, Context& context, uint32_t entry);
    virtual void dumpSpecifics(Dumper& /*dumper*/) const;

    struct Data
    {
        uint16_t NAL_unit_type_; // must be a type of valid sync sample (e.g. IDR)
    };

    struct Data data_;
};

/*******************************************************************************
 * TemporalLayerEntryBox
 * sekwon: I guess this entry is per layer?
 */
class TemporalLayerEntry : public VisualSampleGroupEntry
{
public:
    virtual void clearSpecifics();
    virtual bool readSpecifics(Reader& reader, Context& context, uint32_t entry);
    virtual void dumpSpecifics(Dumper& /*dumper*/) const;

    struct Data
    {
        /*
         * current temporal layer's id
         */
        uint16_t temporalLayerId_;
        uint16_t tlprofile_spaces_;
        uint16_t tltier_flag_;
        uint16_t tlprofile_idc_;
        uint32_t tlprofile_compatibility_indications_;
        uint64_t tlconstraint_indicator_flags_;
        uint16_t tllevel_idc_;
        uint16_t tlMaxBitRate_;
        uint16_t tlAvgBitrate_;
        uint16_t tlConstantFrameRate_;
        uint16_t tlAvgFrameRate_;
    };

    struct Data data_;
};

/*******************************************************************************
 * Temporal sub-layer sample group entry (tsas)
 */
class TemporalSubLayerEntry : public VisualSampleGroupEntry
{
public:
    virtual void clearSpecifics();
    virtual bool readSpecifics(Reader& reader, Context& context, uint32_t entry);
    virtual void dumpSpecifics(Dumper& /*dumper*/) const;

    struct Data
    {
        uint16_t tbd;
    };

    struct Data data_;
};

/*******************************************************************************
 * MVC sample group entries
 */

/*******************************************************************************
 * ScalableNALUMapEntry
 */
class ScalableNALUMapEntry : public VisualSampleGroupEntry
{
public:
    virtual ~ScalableNALUMapEntry();
    virtual void clearSpecifics();
    virtual bool readSpecifics(Reader& reader, Context& context, uint32_t entry);
    virtual void dumpSpecifics(Dumper& /*dumper*/) const;

    struct Data
    {
        uint16_t reserved_;
        uint16_t NALU_count_;
        uint16_t groupID_;
    };

    struct Data data_;
};


}}}


#endif
