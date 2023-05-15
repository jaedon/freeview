/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef __DOLBYVISION_BOXEX__
#define __DOLBYVISION_BOXEX__

#include "Base.h"
#include "ISOBoxes.h"

namespace netflix {
namespace containerlib {
namespace mp4parser {

/*******************************************************************************
 * DolbyVisionConfiguration Box ('dvcC') as defined below
 */

class DolbyVisionConfigurationBox : public Box
{
public:
    static const uint32_t  ExpectedBoxType_;

private:
    uint16_t dv_version_major_; // 8-bit
    uint16_t dv_version_minor_; // 8-bit
    uint16_t dv_profile_;       // 13-bit
    uint16_t rpu_present_;      // 1-bit
    uint16_t el_present_;       // 1-bit
    uint16_t bl_present_;       // 1-bit
    uint32_t reserved_[5];      // 20 bytes

    std::list< shared_ptr<Box> > childBoxes_;

protected:
    virtual void clearSpecifics();
    virtual bool readSpecifics(Reader& reader, Context& context);
    virtual void dumpSpecifics(Dumper& dumper) const;
};

}}}
#endif


