/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef __NETFLIX_CONTAINERLIB_MP4PARSER_NFLXBoxes__
#define __NETFLIX_CONTAINERLIB_MP4PARSER_NFLXBoxes__

#include "Base.h"

namespace netflix {
namespace containerlib {
namespace mp4parser {

/*******************************************************************************
 * NetflixHeaderBox
 */

class NetflixHeaderBox : public FullBox
{
public:
    static const UUID ExpectedBoxUserType_;

private:
    llong fileSize_;
    llong timescale_;
    llong duration_;
    llong offsetToFirstMoof_;
    llong sizeOfAllMoofAndMDat_;

protected:
    virtual void clearSpecifics() {}
    virtual bool readSpecifics(Reader& reader, Context& context);
    virtual void dumpSpecifics(Dumper& dumper) const;
};

/*******************************************************************************
 * NetflixIndexBox
 */

class NetflixIndexBox : public Box
{
public:
    static const UUID ExpectedBoxUserType_;

private:
    std::list< shared_ptr<Box> > childBoxes_;

protected:
    virtual void clearSpecifics();
    virtual bool readSpecifics(Reader& reader, Context& context);
    virtual void dumpSpecifics(Dumper& dumper) const;
};

/*******************************************************************************
 * NetflixTrackIndexBox
 */

class NetflixTrackIndexBox : public FullBox
{
public:
    static const UUID ExpectedBoxUserType_;

private:
    llong timescale_;
    std::list< shared_ptr<Box> > childBoxes_;

protected:
    virtual void clearSpecifics();
    virtual bool readSpecifics(Reader& reader, Context& context);
    virtual void dumpSpecifics(Dumper& dumper) const;
};

/*******************************************************************************
 * OggVorbisSpecificInfo
 */

class OggVorbisSpecificInfo : public FullBox
{
public:
    static const uint32_t ExpectedBoxType_;

protected:
    virtual void clearSpecifics() {}
    virtual bool readSpecifics(Reader& reader, Context& context);
    virtual void dumpSpecifics(Dumper& /*dumper*/) const {}
};

/*******************************************************************************
 * EC3SpecificBox
 */

class EC3SpecificBox : public Box
{
public:
    static const uint32_t ExpectedBoxType_;

private:
    uint32_t data_rate_;
    uint32_t num_ind_sub_;

    struct {
        uint32_t fscod_;
        uint32_t bsid_;
        uint32_t bsmod_;
        uint32_t acmod_;
        uint32_t lfeon_;
        uint32_t reserved_;
        uint32_t num_dep_sub_;
        union {
            uint32_t chan_loc_;
            uint32_t reserved2_;
        };
    } ind_sub_stream_[8];

protected:
    virtual void clearSpecifics() {}
    virtual bool readSpecifics(Reader& reader, Context& context);
    virtual void dumpSpecifics(Dumper&) const {}
};

}}} // namespace netflix::containerlib::mp4parser

#endif // __NETFLIX_CONTAINERLIB_MP4PARSER_NFLXBoxes__
