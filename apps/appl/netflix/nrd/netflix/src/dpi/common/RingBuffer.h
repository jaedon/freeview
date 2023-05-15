/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef DEVICE_RINGBUFFER_H
#define DEVICE_RINGBUFFER_H

#include <vector>

#include "RingBufferSender.h"
#include "RingBufferReceiver.h"

namespace netflix {
namespace device {

/**
 * @class RingBuffer RingBuffer.h
 * @brief Implementation of circular buffer for storing audio/video data for decoding
 */
class RingBuffer : public RingBufferSender, public RingBufferReceiver
{
public:
    /**
     * Constructor.
     *
     * @param[in] data the data space for the ring buffer.
     * @param[in] size number of bytes the ring buffer data space can hold.
     * @param[in] maxAccessUnits number of access-units the access-unit-queue
     *            (associated with the ring buffer) should be able to hold.
     */
    RingBuffer(unsigned char* data, uint32_t size, uint32_t maxAccessUnits);

    virtual ~RingBuffer();

    // RingBufferSender Interface
    //

    virtual uint32_t availableSpace() const;
    virtual uint32_t contiguousSpace() const;
    virtual uint32_t totalSpace() const;
    virtual void sendAccessUnitBegin(AccessUnitAttributes& accessUnitAttributes);
    virtual bool write(const unsigned char* data, uint32_t size);
    virtual bool sendAccessUnitEnd(uint32_t dependentViewOffset = 0);
    virtual bool currAccessUnitInfo(unsigned char* &dataBegin,
                                    uint32_t& dataSize,
                                    bool& dataWrappedAround) const;
    virtual unsigned char* getWritePointer() const;
    virtual bool canWritePointerWraparound() const;
    virtual bool advanceWritePointer(uint32_t bytes);
    virtual bool advanceWritePointerUntilWraparound();

    // RingBufferReceiver Interface
    //

    virtual bool readAccessUnitPeek(const unsigned char* &data,
                                    uint32_t &size,
				    AccessUnitAttributes& accessUnitAttributes) const;

    // For use with DUAL_STREAM_MVC.  The access unit in the ring buffer is split
    // so that baseView NALUs are first, then dependent view NALUs.
    virtual bool readAccessUnitPeek(const unsigned char* &baseViewData,
				    uint32_t &baseViewSize,
				    const unsigned char* &dependentViewData,
				    uint32_t &dependentViewSize,
				    AccessUnitAttributes& accessUnitAttributes) const;
				

    virtual bool readAccessUnitCommit();
    virtual void flush();
    virtual void getBufferFullness(uint32_t& totalBufferSizeInBytes, uint32_t& numBytesInUse,
        uint32_t& numSamplesInBuffer);

private:
    static const uint32_t INVALID_WP = 0xFFFFFFFF;

    struct AccessUnit
    {
        uint32_t wpBegin_;
	uint32_t wpDependentView_; // for 3D.
        uint32_t wpEnd_;
        AccessUnitAttributes attributes_;
    };

    uint32_t volatile wp_;
    uint32_t volatile rp_;
    uint32_t const size_;
    unsigned char* const data_;

    uint32_t volatile wpAuQueue_;
    uint32_t volatile rpAuQueue_;
    uint32_t const maxAccessUnits_;
    AccessUnit* const accessUnitQueue_;

#ifdef __SUPPORT_AU_MERGE__
    mutable std::vector<unsigned char> mergeBuffer_;
#endif
};

}} // namespace netflix::device

#endif // DEVICE_RINGBUFFER_H
