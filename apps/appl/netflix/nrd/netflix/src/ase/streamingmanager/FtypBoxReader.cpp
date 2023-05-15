/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, 0xInc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, 0xInc. is strictly
 * prohibited.
 */

#include "FtypBoxReader.h"

using namespace std;
using namespace netflix;
using namespace ase;

fourcc_t const FtypBoxReader::type = BoxReader::getFourcc( "ftyp" );

bool FtypBoxReader::readBrands( fourcc_t& majorBrand,
                                uint32_t& minorVersion,
                                vector<fourcc_t>& compatibleBrands )
{
    if ( !read( majorBrand )
        || !read( minorVersion ) )
        return false;

    while( goodfor( sizeof( fourcc_t ) ) )
    {
        fourcc_t brand;
        if ( !read( brand ) )
            return false;
        compatibleBrands.push_back( brand );
    }

    return true;
}
