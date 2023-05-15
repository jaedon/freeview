/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */


#ifndef FTYP_BOX_READER_H
#define FTYP_BOX_READER_H

#include <vector>
#include "BoxReader.h"

namespace netflix {
namespace ase {

/*  aligned(8) class FileTypeBox extends Box('ftyp') {
        unsigned int(32) major_brand;
        unsigned int(32) minor_version;
        unsigned int(32) compatible_brands[]; // to end of the box
     }
*/

class FtypBoxReader : public BoxReader
{
public:
    static fourcc_t const type;

    FtypBoxReader( BoxReader const& other );

    bool readBrands( fourcc_t& majorBrand, uint32_t& minorVersion, std::vector<fourcc_t>& compatibleBrands );
};

inline FtypBoxReader::FtypBoxReader( BoxReader const& other )
    : BoxReader( other )
{
}


}} // namespace netflix::ase

#endif /* FTYP_BOX_READER_H */
