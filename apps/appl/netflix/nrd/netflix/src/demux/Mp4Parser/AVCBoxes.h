/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef __NETFLIX_CONTAINERLIB_MP4PARSER_AVCBoxes__
#define __NETFLIX_CONTAINERLIB_MP4PARSER_AVCBoxes__

#include "Base.h"

namespace netflix {
namespace containerlib {
namespace mp4parser {

/*******************************************************************************
 * AVCConfigurationBox
 */

class AVCConfigurationBox : public Box
{
public:
    static const uint32_t ExpectedBoxType_;

private:
    uint16_t configurationVersion_;
    uint16_t AVCProfileIndication_;
    uint16_t profile_compatibility_;
    uint16_t AVCLevelIndication_;
    uint16_t lengthSizeMinusOne_;
    uint32_t numOfSequenceParameterSets_;
    uint32_t numOfPictureParameterSets_;
    std::list< shared_ptr<Box> > childBoxes_;

protected:
    virtual void clearSpecifics();
    virtual bool readSpecifics(Reader& reader, Context& context);
    virtual void dumpSpecifics(Dumper& dumper) const;
};

class MVCConfigurationBox : public Box
{
public:
    static const uint32_t ExpectedBoxType_;

private:
    uint16_t configurationVersion_;
    uint16_t AVCProfileIndication_;
    uint16_t profile_compatibility_;
    uint16_t AVCLevelIndication_;
    bool complete_representation_;
    bool explicit_au_track_;
    uint16_t lengthSizeMinusOne_;
    uint32_t numOfSequenceParameterSets_;
    uint32_t numOfPictureParameterSets_;
    std::list< shared_ptr<Box> > childBoxes_;


 protected:
    virtual void clearSpecifics();
    virtual bool readSpecifics(Reader& reader, Context& context);
    virtual void dumpSpecifics(Dumper& dumper) const;
};


}}} // namespace netflix::containerlib::mp4parser

#endif // __NETFLIX_CONTAINERLIB_MP4PARSER_AVCBoxes__
