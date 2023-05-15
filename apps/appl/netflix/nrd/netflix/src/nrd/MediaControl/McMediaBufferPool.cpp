/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#include "McMediaBufferPool.h"
#include <nrdbase/Log.h>

using namespace netflix;
using namespace netflix::ase;
using namespace netflix::device;
using namespace netflix::mediacontrol;

weak_ptr<IMediaBufferPool> McMediaBufferPool::sMediaBufferPool;
Mutex McMediaBufferPool::sMutex(MEDIABUFFERPOOL_MUTEX, "McMediaBufferPool");
//
// MediaBufferAllocator
//

class netflix::mediacontrol::MediaBufferAllocator
{
public:
    MediaBufferAllocator(size_t itemSize, size_t itemCount);
    ~MediaBufferAllocator();

    void * allocateItem();
    void freeItem(void * p);

private:
    struct FreeNode
    {
        FreeNode * mNext;
    };

    size_t mItemSize;
    size_t mItemCount;
    unsigned char * mBuffer;
    FreeNode * mFreeHead;
    Mutex mMutex;
};

MediaBufferAllocator::MediaBufferAllocator(size_t itemSize, size_t itemCount) :
    mItemSize(itemSize),
    mItemCount(itemCount),
    mBuffer(0),
    mFreeHead(0),
    mMutex(MCMEDIABUFFERPOOL_MUTEX, "MediaBufferAllocator")
{
    mBuffer = new unsigned char[itemSize * itemCount];

    for (unsigned int i = 0; i < mItemCount; ++i) {
        FreeNode * node = (FreeNode *)(mBuffer + (i * mItemSize));
        node->mNext = mFreeHead;
        mFreeHead = node;
    }
}

MediaBufferAllocator::~MediaBufferAllocator()
{
    delete [] mBuffer;
}

void * MediaBufferAllocator::allocateItem()
{
    ScopedMutex sm(mMutex);
    assert(mFreeHead != 0);
    void * p = mFreeHead;
    mFreeHead = mFreeHead->mNext;
    return p;
}

void MediaBufferAllocator::freeItem(void * p)
{
    ScopedMutex sm(mMutex);
    FreeNode * node = (FreeNode *)p;
    node->mNext = mFreeHead;
    mFreeHead = node;
}

//
// McMediaBuffer
//

shared_ptr<MediaBufferAllocator> McMediaBuffer::sBufferAllocator;

McMediaBuffer::McMediaBuffer(
    unsigned char* pStorage,
    size_t size,
    void* handle,
    IBufferManager::BufferDataType type,
    uint32_t allocSeq,
    uint32_t cdnid,
    shared_ptr<IBufferManager>& bufferManager)

    : AseBuffer(pStorage, size, allocSeq, cdnid, handle)
    , mType(type)
    , mBufferManager(bufferManager)
    , mAttached(true)
{
}

McMediaBuffer::~McMediaBuffer()
{
    if(mAttached)
    {
        IBufferManager::Buffer buffer(NULL);
        buffer.bufferDataType_ = mType;
        buffer.byteBuffer_ = mStoragePtr;
        buffer.capacity_ = mStorageCapacity;
        buffer.dataSize_ = 0;
        buffer.timestamp_ = 0;
        buffer.flags_ = 0;
        buffer.allocSeq_ = mSeq;
        buffer.private_ = mHandle;

        mBufferManager->free(buffer);
    }
}

void McMediaBuffer::detachStorage()
{
    mAttached = false;
}

void McMediaBuffer::setAllocator(shared_ptr<MediaBufferAllocator> bufferAllocator)
{
    sBufferAllocator = bufferAllocator;
}

void* McMediaBuffer::operator new(size_t size)
{
    NRDP_UNUSED(size);
    assert(size == sizeof(McMediaBuffer));
    return sBufferAllocator->allocateItem();
}

void McMediaBuffer::operator delete(void* p)
{
    if (sBufferAllocator.get())
        sBufferAllocator->freeItem(p);
}

//
// McMediaBufferPool
//

AseErrorCode McMediaBufferPool::getBufferPool( shared_ptr<IBufferManager> pBufferManager,
                                               shared_ptr<IMediaBufferPool>& pBufferPool)
{
    ScopedMutex lock(sMutex);
    pBufferPool = sMediaBufferPool.lock();
    if (pBufferPool.get() == NULL)
    {
        pBufferPool.reset( new McMediaBufferPool(pBufferManager) );
        sMediaBufferPool = pBufferPool;
        NTRACE(TRACE_MEDIACONTROL, "created a new buffer pool");
    }
    return AS_NO_ERROR;
}

McMediaBufferPool::McMediaBufferPool(
    shared_ptr<IBufferManager> bufferManager)

    : mBufferManager(bufferManager)
    , mUnitSize(0)
    , mBufferAllocator()
    , mReservedVideoUnits( 0 )
    , mReservedAudioUnits( 0 )
{
}

McMediaBufferPool::~McMediaBufferPool()
{
    ScopedMutex lock(sMutex);
    close();
    NTRACE(TRACE_MEDIACONTROL, "buffer pool is deleted");
}


void McMediaBufferPool::setBufferPoolSizes( uint32_t videoMemorySize,
                                            uint32_t videoVirtualMemorySize,
                                            uint32_t audioMemorySize )
{
    NRDP_UNUSED(videoMemorySize);
    NRDP_UNUSED(videoVirtualMemorySize);
    NRDP_UNUSED(audioMemorySize);
}

AseErrorCode McMediaBufferPool::open(
    uint32_t unitSize, uint32_t unitCount)
{
    assert(unitSize > 0);

    if(mUnitSize > 0) // already open
        return AS_NOT_ALlOWED;

    if(!mBufferManager)
        return AS_UNKNOWN_ERROR; // FIXME: better error code?

    NFErr err = mBufferManager->open( unitSize, unitSize);
    if(err == NFErr_Bad)
    {
        return  AS_NOT_ENOUGH_MEMORY;
    }

    mUnitSize = unitSize;

    // unitCount = unitCount; // FIXME: there is no way to control the buffer total space Wei
    NRDP_UNUSED(unitCount);

    uint32_t units = 0;
    units += mBufferManager->getBufferPoolSize(IBufferManager::VIDEO) / mUnitSize;
    units += mBufferManager->getBufferPoolSize(IBufferManager::AUDIO) / mUnitSize;
    mBufferAllocator.reset(new MediaBufferAllocator(sizeof(McMediaBuffer), units));
    McMediaBuffer::setAllocator(mBufferAllocator);

    return AS_NO_ERROR;
}

AseErrorCode McMediaBufferPool::close()
{
    if(mUnitSize > 0) // currently opened
    {
        mBufferManager->close();
        mUnitSize = 0;
    }
    McMediaBuffer::setAllocator(shared_ptr<MediaBufferAllocator>());

    return AS_NO_ERROR;
}

uint32_t McMediaBufferPool::getUnitSize() const
{
    return mUnitSize;
}

uint32_t McMediaBufferPool::getUnitCount() const
{
    assert(mUnitSize > 0);

    uint32_t size = mBufferManager->getBufferPoolSize(IBufferManager::VIDEO);
    return size / mUnitSize;
}

uint32_t McMediaBufferPool::getBufferPoolSize(
        MediaType mediaType) const
{
    if ( mediaType == MEDIA_VIDEO )
        return mBufferManager->getBufferPoolSize(IBufferManager::VIDEO);
    else if ( mediaType == MEDIA_AUDIO)
        return mBufferManager->getBufferPoolSize(IBufferManager::AUDIO);
    else
        return 0;
}


AseErrorCode McMediaBufferPool::allocate(
    MediaType mediaType, uint32_t cdnid,
    AseBufferPtr& pBuffer)
{
    assert(mUnitSize > 0);
    assert(mediaType == MEDIA_VIDEO || mediaType == MEDIA_AUDIO);

    IBufferManager::BufferDataType const dataType =
                    ( mediaType == MEDIA_VIDEO ) ? IBufferManager::VIDEO : IBufferManager::AUDIO;

    IBufferManager::Buffer buffer(NULL);
    NFErr err = mBufferManager->alloc(dataType, mUnitSize, buffer);

    if(err == NFErr_OK)
    {
        pBuffer.reset(new McMediaBuffer(buffer.byteBuffer_,
                                        buffer.capacity_,
                                        buffer.private_,
                                        buffer.bufferDataType_,
                                        buffer.allocSeq_,
                                        cdnid,
                                        mBufferManager));

        uint32_t& reserved = ( mediaType == MEDIA_VIDEO ) ? mReservedVideoUnits : mReservedAudioUnits;
        if ( reserved > 0 ) --reserved;

        return AS_NO_ERROR;
    }

    if(err == NFErr_Pending)
        return AS_NOT_ENOUGH_MEMORY;
    else
        return AS_UNKNOWN_ERROR;
}

uint32_t McMediaBufferPool::getFreeUnitCount(
    MediaType mediaType) const
{
    assert(mUnitSize > 0);

    uint32_t size;
    if ( mediaType == MEDIA_VIDEO )
        size = mBufferManager->getBufferPoolFreeSpace(IBufferManager::VIDEO);
    else
        size = mBufferManager->getBufferPoolFreeSpace(IBufferManager::AUDIO);

    return size / mUnitSize;
}

bool McMediaBufferPool::reserveUnits( MediaType mediaType, uint32_t units )
{
    uint32_t const free = getFreeUnitCount( mediaType );
    uint32_t& reserved = ( mediaType == MEDIA_VIDEO ) ? mReservedVideoUnits : mReservedAudioUnits;
    if ( free - reserved < units ) return false;
    reserved += units;
    return true;
}

void McMediaBufferPool::unreserveUnits( ase::MediaType mediaType, uint32_t units )
{
    uint32_t& reserved = ( mediaType == MEDIA_VIDEO ) ? mReservedVideoUnits : mReservedAudioUnits;
    reserved -= std::min( reserved, units );
}


uint32_t McMediaBufferPool::getUnreservedUnitCount( MediaType mediaType ) const
{
    uint32_t const free = getFreeUnitCount( mediaType );
    uint32_t const reserved = ( mediaType == MEDIA_VIDEO ) ? mReservedVideoUnits : mReservedAudioUnits;
    return free - reserved;
}




