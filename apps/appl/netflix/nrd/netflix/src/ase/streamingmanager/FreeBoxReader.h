/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */


#ifndef FREE_BOX_READER_H
#define FREE_BOX_READER_H

#include <vector>
#include "BoxReader.h"

namespace netflix {
namespace ase {

/*  aligned(8) class FileTypeBox extends Box('Free') {
        unsigned int(32) major_brand;
        unsigned int(32) minor_version;
        unsigned int(32) compatible_brands[]; // to end of the box
     }
*/

class FreeBoxReader : public BoxReader
{
public:
    static fourcc_t const type;

    FreeBoxReader( BoxReader const& other );

    bool readToken( char (&freetoken)[16] );
};

inline FreeBoxReader::FreeBoxReader( BoxReader const& other )
    : BoxReader( other )
{
}


}} // namespace netflix::ase

#endif /* FREE_BOX_READER_H */
