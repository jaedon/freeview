/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef BUFFER_H
#define BUFFER_H

#include <nrdase/AseCommonDataTypes.h>
#include <string.h>
#include <algorithm>

namespace netflix {
namespace ase {

/**
 *  @class AseBuffer
 *
 *  @brief AseBuffer provides a generic buffer class wrapper.
 *
 *  It's assumed that the buffer is filled at the back and emptied from
 *  the front.
 *
 *  To write to the buffer use copyin method, or:
 *
 *
 *      AseBufferPtr pAseBufferPtr;
 *      unsigned int freeSpaceSize = pAseBufferPtr->getFreeSpaceSize();
 *      unsigned char* pWritePosition = pAseBufferPtr->getFreeSpaceStart();
 *
 *      writeSize = recv( socket, pWritePosition, freeSpaceSize, 0 );
 *
 *      pAseBufferPtr->commit( writeSize );
 *
 *  To read from the buffer use copyout method, or:
 *
 *      AseBufferPtr pAseBufferPtr;
 *      unsigned int filledSpaceSize = pAseBufferPtr->getFilledSpaceSize();
 *      unsigned char* pReadPosition = pAseBufferPtr->getFilledSpaceStart();
 *
 *      readSize = send( socket, pReadPosition, filledSpaceSize, 0 );
 *
 *      pAseBufferPtr->release( readSize );
 *
 *  The derived classes implement the actual memory management logic
 *  specific to different types of memory management strategies.
 *
 */
class AseBuffer
{
public:
    typedef shared_ptr<AseBuffer> AseBufferPtr;
    typedef shared_ptr<AseBuffer const> AseBufferConstPtr;

public:
    /** Destructor. */
    virtual ~AseBuffer();

    /** Get the capacity of the buffer */
    size_t getCapacity() const;

    /** True if the buffer is empty */
    bool empty() const;

    /** Erase all data in the buffer
     * @return the number of bytes actually erased
     */
    size_t eraseAll();

    /** Resize the buffer
     *
     *  The default implementation will only
     *  resize within the original storage capacity
     */
    virtual size_t resize( size_t newSize );

    /** Copy data into the back of the buffer
     *  @param[in] pSource The source data
     *  @param[in] size The size of the source data
     *  @return The number of bytes copied
     **/
    size_t copyin( unsigned char const* pSource, size_t size );

    /** Insert data into the front of the buffer
     *  @param[in] pSource The source data
     *  @param[in] size The size of the source data
     *  @return The number of bytes copied
     **/
    size_t insert( unsigned char const* pSource, size_t size );

    /** Copy data out of the front of the buffer
     *  @param[in] pDestination The destination to copy data to
     *  @param[in] size The maximum number of bytes to copy
     *  return The number of bytes copied
     */
    size_t copyout( unsigned char* pDestination, size_t size );

    /** Copy data from the front of another buffer to the back of this one
     *  @param[in] buffer The buffer to copy from
     *  @param[in[ size The maximum number of bytes to copy
     *  return The number of bytes copied
     */
    size_t copyfrom( AseBufferPtr const& buffer, size_t size = 0 );

    /** Copy data from the back of another buffer to the front of this one
     *  @param[in] buffer The buffer to copy from
     *  @param[in[ size The maximum number of bytes to copy
     *  return The number of bytes copied
     */
    size_t insertfrom( AseBufferPtr const& buffer, size_t size = 0 );

    /** Get the size of the free space at the back of the buffer */
    size_t getFreeSpaceSize() const;

    /** Get the start of the free space */
    unsigned char* getFreeSpaceStart();

    /** Commit what was just copied into the buffer
     *  @param size The number of bytes to commit
     *  @return The number of bytes actually committed
     */
    size_t commit( size_t size );

    /** Get the size of the filled space */
    size_t getFilledSpaceSize( ) const;

    /** Get the start of the filled space. */
    unsigned char const* getFilledSpaceStart() const;

    /** Release what was just copied out from the front of the buffer
     *  @param size The number of bytes to release
     *  @return The number of bytes actually released
     */
    size_t release( size_t size );

    /** Release what was just copied out from the back of the buffer
     *  @param size The number of bytes to release
     *  @return The number of bytes actually released
     */
    size_t releaseback( size_t size );

    /** Shuffle data to the start of the buffer, to maximize free space */
    void shuffle();

    /** Shuffle data to the end of the buffer, to maximize free space at the front */
    void shuffleback();

    /** Get the sequence number */
    uint32_t getSeq() const;

    /** Get the id of the cdn servign the block */
    uint32_t getCdnId() const;

    /** Get implementation specific opaque handle associated with the buffer */
    void* getHandle() const;

    /** If storage is detached, freeing resource on destruction is not needed */
    virtual void detachStorage();

    /** whether the buffer is in use */
    virtual bool& inUse();

protected:
    /** Construct new buffer */
    AseBuffer( unsigned char* pStorage, size_t size, uint32_t seq, uint32_t cdnid, void* handle = NULL );

protected:
    unsigned char*  mStoragePtr;
    size_t          mStorageCapacity;
    size_t          mCapacity;
    size_t          mStartOffset;
    size_t          mFreeOffset;
    uint32_t        mSeq;
    uint32_t        mCdnId;
    void* const     mHandle;
    bool            mInuse;
};

typedef AseBuffer::AseBufferPtr AseBufferPtr;

inline AseBuffer::AseBuffer( unsigned char* pStorage, size_t size, uint32_t seq, uint32_t cdnid, void* handle )
    : mStoragePtr( pStorage )
    , mStorageCapacity( size )
    , mCapacity( size )
    , mStartOffset( 0 )
    , mFreeOffset( 0 )
    , mSeq( seq )
    , mCdnId( cdnid )
    , mHandle( handle )
    , mInuse( false )
{
}

inline AseBuffer::~AseBuffer()
{
}

inline size_t AseBuffer::getCapacity() const
{
    return mCapacity;
}

inline size_t AseBuffer::copyin( unsigned char const* pSource, size_t size )
{
    size_t copy = std::min( size, getFreeSpaceSize() );
    memcpy( getFreeSpaceStart(), pSource, copy );
    commit( copy );
    return copy;
}

inline size_t AseBuffer::insert( unsigned char const* pSource, size_t size )
{
    size_t copy = std::min( size, mStartOffset );
    mStartOffset -= copy;
    memcpy( mStoragePtr + mStartOffset, pSource, copy );
    return copy;
}

inline size_t AseBuffer::copyout( unsigned char* pDestination, size_t size )
{
    size_t copy = std::min( size, getFilledSpaceSize() );
    memcpy( pDestination, getFilledSpaceStart(), copy );
    release( copy );
    return copy;
}

inline size_t AseBuffer::copyfrom( AseBufferPtr const& buffer, size_t size )
{
    size_t copy = ( size == 0 ) ? buffer->getFilledSpaceSize()
                                : std::min( size, buffer->getFilledSpaceSize() );
    copy = copyin( buffer->getFilledSpaceStart(), copy );
    buffer->release( copy );
    return copy;
}

inline size_t AseBuffer::insertfrom( AseBufferPtr const& buffer, size_t size )
{
    size_t copy = ( size == 0 ) ? buffer->getFilledSpaceSize()
                                : std::min( size, buffer->getFilledSpaceSize() );
    copy = std::min( copy, mStartOffset );
    mStartOffset -= copy;
    unsigned char const* const pSource = buffer->getFilledSpaceStart() + buffer->getFilledSpaceSize() - copy;
    memcpy( mStoragePtr + mStartOffset, pSource, copy );

    return copy;
}

inline bool AseBuffer::empty() const
{
    return getFilledSpaceSize() == 0;
}

inline size_t AseBuffer::eraseAll()
{
    size_t size = getFilledSpaceSize();
    memset(mStoragePtr, 0, mCapacity);
    mStartOffset = 0;
    mFreeOffset = 0;
    return size;
}


inline size_t AseBuffer::getFreeSpaceSize() const
{
    return mCapacity - mFreeOffset;
}

inline unsigned char* AseBuffer::getFreeSpaceStart()
{
    return mStoragePtr + mFreeOffset;
}

inline size_t AseBuffer::commit( size_t size )
{
    size = std::min( size, getFreeSpaceSize() );
    mFreeOffset += size;
    return size;
}

inline size_t AseBuffer::getFilledSpaceSize( ) const
{
    return mFreeOffset - mStartOffset;
}

inline unsigned char const* AseBuffer::getFilledSpaceStart() const
{
    return mStoragePtr + mStartOffset;
}

inline size_t AseBuffer::release( size_t size )
{
    size = std::min( size, getFilledSpaceSize() );
    mStartOffset += size;
    return size;
}

inline size_t AseBuffer::releaseback( size_t size )
{
    size = std::min( size, getFilledSpaceSize() );
    mFreeOffset -= size;
    return size;
}

inline uint32_t AseBuffer::getSeq() const
{
    return mSeq;
}

inline uint32_t AseBuffer::getCdnId() const
{
    return mCdnId;
}

inline void* AseBuffer::getHandle() const
{
    return mHandle;
}

inline void AseBuffer::detachStorage()
{
    // It's up to the derived class to handle storage detachment.
}

inline bool& AseBuffer::inUse()
{
    return mInuse;
}


}}
#endif //BUFFER_H
