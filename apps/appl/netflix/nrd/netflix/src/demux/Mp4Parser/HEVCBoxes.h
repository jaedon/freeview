/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef __NETFLIX_CONTAINERLIB_MP4PARSER_HEVCBoxes__
#define __NETFLIX_CONTAINERLIB_MP4PARSER_HEVCBoxes__

#include "Base.h"
#include "ISOBoxes.h"

namespace netflix {
namespace containerlib {
namespace mp4parser {

/*******************************************************************************
 * HEVCConfigurationBox (hvcC)
 */

class HEVCConfigurationBox : public Box
{
public:
    static const uint32_t  ExpectedBoxType_;

private:
    uint16_t configurationVersion_;
    uint16_t profile_space_;
    uint16_t tier_flag_;
    uint16_t profile_idc_;
    ullong profile_compatibility_indications_;
    uint16_t progressive_source_flag_;
    uint16_t interlaced_source_flag_;
    uint16_t non_packed_constraint_flag_;
    uint16_t frame_only_constraint_flag_;
    ullong constraint_indicator_flags_;
    uint16_t level_idc_;

    /*
     * paralleismType_ indicates the type of parallelism that is used to meet
     * the restrictions imposed by min_spatial_segmentation_idc when
     * min_spatial_segmentation_idc is greater than 0.
     *
     * parallelismType_
     * value 0 : mixed type of paralle decoding or parallelism type unknown
     * value 1 : slice based parallel decoding.
     * value 2 : tile based parallel decoding.
     * value 3 : entropy coding synchronization based parallel decoding
     */
    uint16_t min_spatial_segmentation_idc_;
    uint16_t parallelismType_;

    uint16_t chromaFormat_;
    uint16_t bitDepthLumaMinux8_;
    uint16_t bitDepthChromaMinux8_;

    /*
     * by unit of frames/256sec. 0 means unspecified avarage frame rate
     */
    uint16_t avgFrameRate_;

    /*
     * 0 : may or may not constant frame rate (don't know)
     * 1 : constant frame rate
     * 2 : each temporal layer if constant frame rate
     */
    uint16_t constantFrameRate_;

    /*
     * numTemporalLayers
     * 0 : unknown
     * 1 : no temporal layer used
     * n > 1 : temporal layer used
     */
    uint16_t numTemporalLayers_;

    /*
     * 0 : at leaset one of the SPSs has sps_temporal_id_nesting_flag = 0.
     * 1 : all SPSs has sps_temporal_id_nesting_flag = 1, and temporal sublayer
     *     up-switching to any higher temporal layer can be done at any sample.
     */
    uint16_t temporalIdNested_;

    /*
     * NAL length byte 0,1,3 corresponding to 1,2,4 bytes for length
     */
    uint16_t lengthSizeMinusOne_;

    /*
     * array of NALs for VPS/SPS/PPS/SEI
     */
    uint16_t numOfArrays_;

    /*
     * array_completeness is 1 when all NAL units of the given NAL_unit_type
     * are in the following array and none are in the stream. it means that
     * VPS/SPS/PPS or SEI NALs exist only here. array_completenss is 0 when
     * additional NAL units of the indicated type may be in the stream.
     */
    uint16_t array_completeness_;

    /*
     * restricted to only VPS/SPS/PPS/SEI. For SEI, it should be information
     * about the whole stream.
     */
    uint16_t NAL_unit_type_;
    uint16_t numNalus_;
    std::list< shared_ptr<Box> > childBoxes_;

protected:
    virtual void clearSpecifics();
    virtual bool readSpecifics(Reader& reader, Context& context);
    virtual void dumpSpecifics(Dumper& dumper) const;
};

}}} // namespace netflix::containerlib::mp4parser

#endif
