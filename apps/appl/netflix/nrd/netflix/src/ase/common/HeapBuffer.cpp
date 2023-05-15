/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "HeapBuffer.h"

namespace netflix {
namespace ase {

int HeapBuffer::construct( size_t size, HeapBufferPtr& pBuffer )
{
    unsigned char* pData = allocate( size );

    if ( pData == 0 )
        return -1;

    pBuffer.reset( new HeapBuffer( pData, size ) );

    return 0;
}

HeapBuffer::HeapBuffer( unsigned char* pData, size_t size )
    : AseBuffer( pData, size, 0, 0 )
{
}

HeapBuffer::~HeapBuffer()
{
    if ( mStoragePtr )
        deallocate( mStoragePtr );
}

size_t HeapBuffer::resize( size_t size )
{
    if ( size <= mStorageCapacity )
    {
        mCapacity = size;
    }
    else
    {
        mCapacity = mStorageCapacity;
        unsigned char* pData = allocate( size );
        if ( pData == 0 )
            return mCapacity;

        memcpy( pData, mStoragePtr, mStorageCapacity );
        deallocate( mStoragePtr );
        mStoragePtr = pData;
        mStorageCapacity = size;
        mCapacity = size;
    }

    return 0;
}

unsigned char* HeapBuffer::allocate( size_t size )
{
    return new (std::nothrow) unsigned char[ size ];
}

void HeapBuffer::deallocate( unsigned char* pData )
{
    if ( pData )
        delete[] pData;
}

}}
