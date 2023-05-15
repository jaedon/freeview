/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, 0xInc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, 0xInc. is strictly
 * prohibited.
 */

#include "FreeBoxReader.h"

using namespace std;
using namespace netflix;
using namespace ase;

fourcc_t const FreeBoxReader::type = BoxReader::getFourcc( "free" );

bool FreeBoxReader::readToken( char (&freetoken)[16] )
{
    return read( freetoken );
}
