/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#ifndef MEDIACONTROL_MEDIABUFFER_POOL_H
#define MEDIACONTROL_MEDIABUFFER_POOL_H

#include <nrd/IBufferManager.h>
#include <nrdase/IMediaBufferPool.h>

namespace netflix {
namespace mediacontrol {

class MediaBufferAllocator;

/* @class McMediaBuffer McMediaBufferPool.h
 * @brief the data buffer object which is issued by the McMediaBufferPool.
 */
class McMediaBuffer : public ase::AseBuffer
{
private:
    device::IBufferManager::BufferDataType mType;
    shared_ptr<device::IBufferManager> mBufferManager;
    bool mAttached;

    static shared_ptr<MediaBufferAllocator> sBufferAllocator;

public:
    McMediaBuffer(unsigned char* pStorage,
                  size_t size,
                  void* handle,
                  device::IBufferManager::BufferDataType type,
                  uint32_t allocSeq,
                  uint32_t cdnid,
                  shared_ptr<device::IBufferManager>& bufferManager);

    virtual ~McMediaBuffer();

    virtual void detachStorage();

    static void setAllocator(shared_ptr<MediaBufferAllocator> bufferAllocator);
    static void* operator new(size_t size);
    static void operator delete(void* p);
};

/* @class McMediaBufferPool McMediaBufferPool.h
 * @brief implementation of the IMediaBufferPool interface for streaming engine.
 */
class McMediaBufferPool : public ase::IMediaBufferPool
{
private:
    shared_ptr<device::IBufferManager> mBufferManager;
    uint32_t mUnitSize;
    shared_ptr<MediaBufferAllocator> mBufferAllocator;

    uint32_t mReservedVideoUnits;
    uint32_t mReservedAudioUnits;

    static weak_ptr<IMediaBufferPool> sMediaBufferPool;
    static Mutex sMutex;

public:
    static AseErrorCode getBufferPool( shared_ptr<device::IBufferManager> pBufferManager,
                                       shared_ptr<IMediaBufferPool>& pBufferPool);


public:
    McMediaBufferPool(shared_ptr<device::IBufferManager> bufferManager);
    virtual ~McMediaBufferPool();
    // IMediaBufferPool
    //
    virtual AseErrorCode open(uint32_t unitSize, uint32_t unitCount);
    virtual AseErrorCode close();
    virtual void setBufferPoolSizes( uint32_t videoMemorySize,
                                     uint32_t videoVirtualMemorySize,
                                     uint32_t audioMemorySize );
    virtual uint32_t getUnitSize() const;
    virtual uint32_t getUnitCount() const;
    virtual AseErrorCode allocate(ase::MediaType mediaType, uint32_t cdnid, ase::AseBufferPtr& pBuffer);
    virtual uint32_t getFreeUnitCount(ase::MediaType mediaType) const;
    virtual uint32_t getBufferPoolSize(ase::MediaType mediaType) const;
    virtual bool reserveUnits( ase::MediaType mediaType, uint32_t units );
    virtual void unreserveUnits( ase::MediaType mediaType, uint32_t units );
    virtual uint32_t getUnreservedUnitCount( ase::MediaType mediaType ) const;
};

}} // namespace netflix::mediacontrol

#endif // MEDIACONTROL_MEDIABUFFER_POOL_H
