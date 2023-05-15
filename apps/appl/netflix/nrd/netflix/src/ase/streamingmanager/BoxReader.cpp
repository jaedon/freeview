/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "BoxReader.h"

namespace netflix {
namespace ase {

const fourcc_t BoxReader::UUID_4CC = BoxReader::getFourcc( "uuid" );

bool BoxReader::readBoxHeader( )
{
    uint32_t boxsize = 0;

    mpBoxStart = mpCurrent;

    if ( !read( boxsize ) || !read( mType ) )
        return false;

    if ( boxsize == 1 )
    {
        if ( !read( mSize ) )
            return false;
    }
    else
    {
        mSize = boxsize;
    }

    llong const left = mpEnd - mpBoxStart;
    if ( left < (llong) mSize )
    {
        mMissing = mSize - left;
    }
    else
    {
        mMissing = 0;
    }

    if ( mType == UUID_4CC )
    {
        if ( !read( mExtendedType ) )
            return false;
    }

    return true;
}

bool BoxReader::readFullBoxHeader( )
{
    return unpack32<8,24>( &mVersion, &mFlags );
}

bool BoxReader::nextBox()
{
    if ( mpBoxStart == mData.end() || mMissing )
        return false;

    mOffset += mSize;
    mpCurrent = mpBoxStart + mSize;
    mpBoxStart = mpCurrent;
    mType = 0;
    mSize = 0;
    mMissing = 0;

    return true;
}

void BoxReader::reset()
{
    Reader::reset();

    mOffset = 0;
    mType = 0;
    mSize = 0;
    mMissing = 0;
    mVersion = 0;
    mFlags = 0;
    mpBoxStart = mData.end();
}

}}
