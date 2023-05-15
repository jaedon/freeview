/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "EditList.h"

using namespace netflix;
using namespace ase;

EditList::EditList()
    : mTimescale( 0 )
{
}

void EditList::setTimescale( ullong timescale )
{
    mTimescale = timescale;
}

bool EditList::empty()
{
    return mEntries.empty();
}

void EditList::add( ullong duration, llong media, bool dwell )
{
    mEntries.push_back( Entry( duration, media, dwell ) );
}

bool EditList::map( AseTimeStamp cts, AseTimeStamp& pts )
{
    AseTimeStamp const ctsmap( cts, mTimescale );
    if ( empty() )
    {
        pts = ctsmap;
        return true;
    }

    ullong ctsticks = ctsmap.getNumberOfTicks();
    ullong ptsticks = 0;
    std::vector<Entry>::const_iterator it = mEntries.begin();
    for( ; it != mEntries.end(); ++it )
    {
        if ( !it->mDwell
                && it->mMediaTime >= 0
                && (ullong) it->mMediaTime <= ctsticks
                && ( ( it->mDuration == 0 ) || ( (ullong) ( it->mMediaTime + it->mDuration ) > ctsticks ) ) )
        {
            ptsticks += ctsticks - it->mMediaTime;
            break;
        }

        ptsticks += it->mDuration;
    }

    if ( it == mEntries.end() )
        return false;

    pts = AseTimeStamp( ptsticks, mTimescale );

    return true;
}


