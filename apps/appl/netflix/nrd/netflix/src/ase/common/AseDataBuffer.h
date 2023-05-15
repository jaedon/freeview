/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef ASE_DATA_BUFFER_H
#define ASE_DATA_BUFFER_H

#include "nrdbase/DataBuffer.h"
#include "AseBuffer.h"

namespace netflix {
namespace ase {

class AseDataBuffer : protected DataBuffer
                    , public AseBuffer
{
public:
    typedef shared_ptr<AseDataBuffer> AseDataBufferPtr;

    static int construct( DataBuffer const& other, AseDataBufferPtr& pAseDataBuffer );

    virtual ~AseDataBuffer();

    virtual size_t resize( size_t newSize );

protected:
    AseDataBuffer( DataBuffer const& other );
};

typedef AseDataBuffer::AseDataBufferPtr AseDataBufferPtr;

inline int AseDataBuffer::construct( DataBuffer const& other, AseDataBufferPtr& pAseDataBuffer )
{
    pAseDataBuffer.reset( new AseDataBuffer( other ) );
    return 0;
}

inline AseDataBuffer::AseDataBuffer( DataBuffer const& other )
    : DataBuffer( other )
    , ase::AseBuffer( 0, 0, 0 /*seq*/, 0 /*cdnid*/ )
{
    mStoragePtr = mData->ptr()->data + mOffset;
    mStorageCapacity = DataBuffer::capacity() - mOffset;
    mCapacity = mStorageCapacity;
    mFreeOffset = DataBuffer::size();
}

inline AseDataBuffer::~AseDataBuffer() {}

inline size_t AseDataBuffer::resize( size_t newSize )
{
    DataBuffer::resize( newSize );
    AseBuffer::resize( newSize );
    mStoragePtr = mData->ptr()->data + mOffset;
    return size();
}

}
}

#endif
