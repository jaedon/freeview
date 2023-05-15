/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef __NETFLIX_CONTAINERLIB_MP4PARSER_MP4Boxes__
#define __NETFLIX_CONTAINERLIB_MP4PARSER_MP4Boxes__

#include "Base.h"

namespace netflix {
namespace containerlib {
namespace mp4parser {

/*******************************************************************************
 * ESDBox
 */

class ESDBox : public FullBox
{
public:
    static const uint32_t ExpectedBoxType_;

private:
    std::list< shared_ptr<BaseDescriptor> > childDescriptors_;

protected:
    virtual void clearSpecifics();
    virtual bool readSpecifics(Reader& reader, Context& context);
    virtual void dumpSpecifics(Dumper& dumper) const;
};


}}} // namespace netflix::containerlib::mp4parser

#endif // __NETFLIX_CONTAINERLIB_MP4PARSER_MP4Boxes__
