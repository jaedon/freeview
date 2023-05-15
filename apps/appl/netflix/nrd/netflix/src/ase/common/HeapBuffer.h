/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#ifndef HEAP_BUFFER_H
#define HEAP_BUFFER_H

#include <nrdase/AseBuffer.h>

namespace netflix {
namespace ase {

class HeapBuffer : public AseBuffer
{
public:
    typedef shared_ptr<HeapBuffer> HeapBufferPtr;

    static int construct( size_t size, HeapBufferPtr& pBuffer );

    virtual size_t resize( size_t size );

    virtual ~HeapBuffer();

protected:
    HeapBuffer( unsigned char* pData, size_t size );

    static unsigned char* allocate( size_t size );

    static void deallocate( unsigned char* pData );
};

typedef HeapBuffer::HeapBufferPtr HeapBufferPtr;

}}
#endif /* HEAP_BUFFER_H */
