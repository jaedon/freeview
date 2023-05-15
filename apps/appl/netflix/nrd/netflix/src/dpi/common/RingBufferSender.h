/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef DEVICE_RINGBUFFERSENDER_H
#define DEVICE_RINGBUFFERSENDER_H

#include "Common.h"

namespace netflix {
namespace device {

/**
 * @class RingBufferSender RingBufferSender.h
 * @brief Sender side interface of RingBuffer
 */
class RingBufferSender
{
public:
    virtual ~RingBufferSender() {}

    /**
     * Get the size of available ring buffer space (in bytes) that starts from
     * the current write-pointer position.
     *
     * @return the size of available ring buffer space in bytes.
     */
    virtual uint32_t availableSpace() const = 0;

    /**
     * Get the size of contiguous ring buffer space (in bytes) that starts from
     * the current write-pointer position. Note that this contiguous space may
     * not be entirely available for writing. The availableSpace() function
     * should be used to determine how much space is available for writing.
     *
     * @return the size of contiguous ring buffer space in bytes.
     */
    virtual uint32_t contiguousSpace() const = 0;

    /**
     * Get the size of the total ring buffer space in bytes.
     *
     * @return the size of the total ring buffer space in bytes.
     */
    virtual uint32_t totalSpace() const = 0;

    /**
     * Mark the beginning of sending an access-unit.
     *
     * @param[in] accessUnitAttributes the attributes associated with the access-unit.
     */
    virtual void sendAccessUnitBegin(AccessUnitAttributes& accessUnitAttributes) = 0;

    /**
     * Write data (which is part of the current access-unit) into ring buffer.
     *
     * @param[in] data the source data.
     * @param[in] size number of bytes of the source data.
     *
     * @return true on success; false on failure (available space isn't enough).
     */
    virtual bool write(const unsigned char* data, uint32_t size) = 0;

    /**
     * Mark the end of sending an access-unit, and actually send it into the
     * access-unit queue.
     *
     * @return true on success; false on failure (when access-unit queue is full, or
     *         this function is not paired with sendAccessUnitBegin() properly)
     */
    virtual bool sendAccessUnitEnd(uint32_t dependentViewOffset = 0) = 0;

    /**
     * Return information about the current access-unit before it is sent into the
     * access-unit queue.
     *
     * @param[out] dataBegin the data pointer which points to the beginning of the
     *             current access-unit data space.
     * @param[out] dataSize the size of the current access-unit data in bytes.
     * @param[out] dataWrappedAround true if the data space of the current access-unit
     *             has wrapped-around, false otherwise.
     *
     * @return true on success; false on failure (when this function is not called
     *         between sendAccessUnitBegin() and sendAccessUnitEnd())
     */
    virtual bool currAccessUnitInfo(unsigned char* &dataBegin,
                                    uint32_t& dataSize,
                                    bool& dataWrappedAround) const = 0;

    /**
     * Get the memory address pointed-to by the current write-pointer.
     *
     * @return the memory address pointed-to by the current write-pointer.
     */
    virtual unsigned char* getWritePointer() const = 0;

    /**
     * Check whether the write-pointer is allowed to wrap-around to the beginning
     * of the ring buffer space.
     *
     * @return true if wraparound is allowed; false otherwise.
     */
    virtual bool canWritePointerWraparound() const = 0;

    /**
     * Advance the write-pointer by the specified number of bytes. If the
     * available space in ring buffer is not big enough to allow this,
     * this function would fail.
     *
     * @return true on success; false on failure (available space isn't enough).
     */
    virtual bool advanceWritePointer(uint32_t bytes) = 0;

    /**
     * Advance the write-pointer until it wraps-around to the beginning of the
     * ring buffer space. If the current ring buffer state disallows this
     * operation, this function would fail.
     *
     * @return true on success; false on failure (disallowed).
     */
    virtual bool advanceWritePointerUntilWraparound() = 0;
};

}} // namespace netflix::device

#endif // DEVICE_RINGBUFFERSENDER_H
