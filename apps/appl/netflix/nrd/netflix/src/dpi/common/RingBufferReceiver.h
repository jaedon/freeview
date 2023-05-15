/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef DEVICE_RINGBUFFERRECEIVER_H
#define DEVICE_RINGBUFFERRECEIVER_H

#include "Common.h"

#define DUAL_STREAM_MVC

namespace netflix {
namespace device {

/**
 * @class RingBufferReceiver RingBufferReceiver.h
 * @brief Receiver side interface of RingBuffer
 */
class RingBufferReceiver
{
public:
    virtual ~RingBufferReceiver() {}

    /**
     * Peek at the end of the access-unit queue (the oldest access-unit in queue).
     *
     * @param[out] data pointer to the beginning of access-unit data.
     * @param[out] size number of bytes of the access-unit data.
     * @param[out] accessUnitAttributes the attributes associated with the access-unit.
     *
     * @return true on success; false on failure (when access-unit queue is empty)
     */
    virtual bool readAccessUnitPeek(const unsigned char* &data,
                                    uint32_t &size,
                                    AccessUnitAttributes& accessUnitAttributes) const = 0;

#ifdef DUAL_STREAM_MVC
    // For use with DUAL_STREAM_MVC.  The access unit in the ring buffer is split
    // so that baseView NALUs are first, then dependent view NALUs.
    virtual bool readAccessUnitPeek(const unsigned char* &baseViewData,
				    uint32_t &baseViewSize,
				    const unsigned char* &dependentViewData,
				    uint32_t &dependentViewSize,
				    AccessUnitAttributes& accessUnitAttributes) const = 0;
#endif

    /**
     * Remove item from the end of the access-unit queue (the oldest access-unit in queue).
     *
     * @return true on success; false on failure (when access-unit queue is empty)
     */
    virtual bool readAccessUnitCommit() = 0;

    /** Remove all items from the access-unit queue. */
    virtual void flush() = 0;

    /** Returns the size of the ring buffer and the number of bytes in use **/
    virtual void getBufferFullness(uint32_t& totalBufferSizeInBytes, uint32_t& numBytesInUse,
        uint32_t& numSamplesInBuffer) = 0;

};

}} // namespace netflix::device

#endif // DEVICE_RINGBUFFERRECEIVER_H
