/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef IMEDIABUFFER_POOL_H
#define IMEDIABUFFER_POOL_H

#include <nrdase/AseBuffer.h>
#include <nrdbase/AseErrorCodes.h>

namespace netflix {
namespace ase {

/* @class IMediaBufferPool
 *
 * @brief Provides a pool of constant sized buffer objects
 *
 * AseBuffer objects implement the IBuffer interface. The buffer is
 * freed when the buffer object is deleted.
 */
class IMediaBufferPool
{
public:
    typedef shared_ptr<IMediaBufferPool> IMediaBufferPoolPtr;

public:
    /** Destructor. */
    virtual ~IMediaBufferPool() {}

    /** Open the buffer pool.
     *
     *  @param unitSize [In] The size of the buffer unit, in bytes.
     *  @param unitCount [In] The number of buffer units.
     *
     *  @return AS_NO_ERROR on success
     *          AS_UNKNOWN_ERROR unknown error
     *          AS_NOT_ENOUGH_MEMORY not enough memory.
     *          AS_NOT_ALlOWED The pool is already open.
     */
    virtual AseErrorCode open( uint32_t unitSize, uint32_t unitCount ) = 0;

    /** Close  the buffer pool.
     *
     *  @return AS_NO_ERROR on success
     *          AS_UNKNOWN_ERROR unknown error
     */
    virtual AseErrorCode close() = 0;

    virtual void setBufferPoolSizes( uint32_t videoMemorySize,
                                     uint32_t videoVirtualMemorySize,
                                     uint32_t audioMemorySize ) = 0;

    /** Get the size of the buffer unit, in bytes. */
    virtual uint32_t getUnitSize() const = 0;

    /** Get the number of buffer units. */
    virtual uint32_t getUnitCount() const = 0;

    /** Get the buffer pool size of the given media type
     * @param mediaType [in] The media type
     */
    virtual uint32_t getBufferPoolSize(MediaType mediaType) const = 0;

    /** Allocate a buffer.
     *
     *  @param mediaType [In] The media type this buffer is allocated for.
     *  @param cdnid [In] The id of the cdn serving the block
     *  @param pBuffer [Out] The allocated buffer.
     *
     *  @return AS_NO_ERROR on success
     *          AS_UNKNOWN_ERROR unknown error
     *          AS_NOT_ENOUGH_MEMORY no free unit available.
     */
    virtual AseErrorCode allocate( MediaType mediaType, uint32_t cdnid, AseBufferPtr& pBuffer ) = 0;

    /** Get the number of free units.
     *
     *  @param mediaType [In] The media type
     *  @return The number of free units.
     */
    virtual uint32_t getFreeUnitCount(MediaType mediaType) const = 0;

    /** Reserve units
     * @param mediaType the media type to reserve
     * @param units the nuber of units
     * @return true if there were enough unreserved units
     */
    virtual bool reserveUnits( MediaType mediaType, uint32_t units ) = 0;

    /** Unreserve units
     *  @param mediaTYpe the media type to unreserve
     *  @param units the number of units
     */
    virtual void unreserveUnits( ase::MediaType mediaType, uint32_t units ) = 0;

    /** Get the number of unreserved units
     *  @param mediaType the media type
     */
    virtual uint32_t getUnreservedUnitCount( MediaType mediaType ) const = 0;

};

typedef IMediaBufferPool::IMediaBufferPoolPtr IMediaBufferPoolPtr;

}}
#endif //IBUFFER_POOL_H
