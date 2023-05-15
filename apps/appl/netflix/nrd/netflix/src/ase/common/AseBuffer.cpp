/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include <nrdase/AseBuffer.h>

namespace netflix {
namespace ase
{

size_t AseBuffer::resize( size_t newSize )
{
    if ( newSize > mStorageCapacity )
        newSize = mStorageCapacity;

    if ( newSize < mFreeOffset )
        mFreeOffset = newSize;

    if ( newSize < mStartOffset )
    {
        mStartOffset = 0;
        mFreeOffset = 0;
    }

    mCapacity = newSize;

    return mCapacity;
}

void AseBuffer::shuffle()
{
    if ( mStoragePtr && mStartOffset != 0 )
    {
        ::memmove( mStoragePtr, mStoragePtr + mStartOffset, mFreeOffset - mStartOffset );
        mFreeOffset -= mStartOffset;
        mStartOffset = 0;
    }
}

void AseBuffer::shuffleback()
{
    if ( mStoragePtr && mFreeOffset != mCapacity )
    {
        size_t const shift = mCapacity - mFreeOffset;
        ::memmove( mStoragePtr + mStartOffset, mStoragePtr + mStartOffset + shift, mFreeOffset - mStartOffset );
        mFreeOffset = mCapacity;
        mStartOffset += shift;
    }
}

}}

