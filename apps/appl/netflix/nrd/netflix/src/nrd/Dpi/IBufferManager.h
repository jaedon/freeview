/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef NRDP_IBUFFERMANAGER_H
#define NRDP_IBUFFERMANAGER_H

/** @file IBufferManager.h The interface for managing media buffers.
 *
 * IBufferManager.h declares the interface for the partner implemented object that
 * manages the allocation of buffers for storing media data.
 *
 * For an example, see the Linux-desktop DPI implementation
 * (nrdlib-device/src/Playback/BufferManagerNative.cpp).
 */

#include <nrdbase/NFErr.h>

namespace netflix {
namespace device {

/**
 * @class IBufferManager IBufferManager.h
 * @brief Interface for encapsulation of device buffer management
 *        functionalities.
 */
class IBufferManager
{
public:
    /** The Type of Data Stored in a Buffer Object. */
    enum BufferDataType
    {
        AUDIO,  /**< Audio Data  */
        VIDEO   /**< Video Data  */
    };

    /** The Descriptor Structure of a Buffer Object. */
    struct Buffer
    {
        enum {
            START_OF_GOP = 1 << 0
        };

        Buffer(bool* inUse) : inUse_(inUse) {}

        BufferDataType bufferDataType_; /**< The type of stored data */
        unsigned char* byteBuffer_;     /**< The pointer to the byte buffer */
        uint32_t       capacity_;       /**< The capacity of the byte buffer */
        uint32_t       dataSize_;       /**< The size of stored data in bytes */
        uint32_t       cdnId_;          /**< cdn id where stored data come from */
        uint32_t       timestamp_;      /**< The timestamp associated with data */
        uint32_t       flags_;          /**< The flags associated with data */
        void*          private_;        /**< An implementation specific private data */

        uint32_t       allocSeq_;      /**< The allocation sequence number */
        bool*          inUse_;

        inline bool isStartOfGop() const { return ((flags_ & START_OF_GOP) != 0); }
    };

public:
    /** Free all buffer pools and resources. */
    virtual ~IBufferManager() {}

    /**
     * Open the device buffer manager.
     *
     * @return NFErr_OK if successful, NFErr_Bad if something went wrong.
     */
    virtual NFErr open( uint32_t audioUnitSize, uint32_t videoUnitSize ) = 0;

    /**
     * Close the device buffer manager. Free all buffer pools and resources.
     */
    virtual void close() = 0;


    /**
     * Get the size of the buffer pool of the specified data type.
     *
     * @param[in] dataType data type of the pool whose size is being queried.
     *
     * @return the buffer pool total capacity in number of bytes.
     */
    virtual uint32_t getBufferPoolSize(BufferDataType dataType) = 0;

    /**
     * Get free space of the buffer pool of the specified data type.
     *
     * @param[in] dataType data type of the pool whose free space is being
     *            queried.
     *
     * @return the buffer pool free space in number of bytes.
     */
    virtual uint32_t getBufferPoolFreeSpace(BufferDataType dataType) = 0;

    /**
     * Allocate a buffer object with capacity of the specified number of bytes.
     *
     * If NFErr_Pending is returned, the buffer pool is temporarily out of
     * space. The caller should wait and retry later.
     *
     * If a nonzero unit size is set for the buffer pool via
     * setBufferPoolSize(), the requested allocation size for the buffer
     * object here must match the buffer pool unit size.
     *
     * @param[in] dataType type of data which the buffer is allocated for.
     * @param[in] size the requested capacity of the allocated buffer object
     *            in number of bytes.
     * @param[out] buffer to receive the allocated buffer object.
     *
     * @return NFErr_OK if successful, NFErr_Pending if the space in buffer
     *         pool has run out, NFErr_Bad if something went wrong,
     *         NFErr_BadParameter if a parameter is invalid.
     */
    virtual NFErr alloc(BufferDataType dataType, uint32_t size,
                        Buffer& buffer) = 0;

    /**
     * Free a buffer object (which will be no longer usable).
     *
     * @param[in] buffer the buffer object to be freed.
     *
     * @return NFErr_OK if successful, NFErr_Bad if something went wrong.
     */
    virtual NFErr free(Buffer& buffer) = 0;
};

}} // namespace netflix::device

#endif // NRDP_IBUFFERMANAGER_H
