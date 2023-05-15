/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "RingBuffer.h"

#include <assert.h>
#include <string.h>

using namespace netflix::device;
using namespace netflix;

// RULES
//
// "WP" can only be modified by the sender side.
// "RP" can only be modified by the receiver side.
// "WP == RP" means buffer is empty.
// "(WP+1) == RP" (with wrap-around handled) means buffer is full.

RingBuffer::RingBuffer(unsigned char* data, uint32_t size, uint32_t maxAccessUnits)
    : wp_(0), rp_(0), size_(size), data_(data),
      wpAuQueue_(0), rpAuQueue_(0), maxAccessUnits_(maxAccessUnits + 1),
      accessUnitQueue_(new AccessUnit[maxAccessUnits + 1])
{
    uint32_t i;
    for(i = 0; i < maxAccessUnits_; i ++)
        accessUnitQueue_[i].wpBegin_ = INVALID_WP;
}

RingBuffer::~RingBuffer()
{
    delete [] accessUnitQueue_;
}

uint32_t RingBuffer::availableSpace() const
{
    uint32_t rp_snapshot = rp_;

    if(rp_snapshot > wp_)
        return rp_snapshot - wp_ - 1;
    else
        return size_ + rp_snapshot - wp_ - 1;
}

uint32_t RingBuffer::contiguousSpace() const
{
    return size_ - wp_;
}

uint32_t RingBuffer::totalSpace() const
{
    return size_;
}

void RingBuffer::sendAccessUnitBegin(AccessUnitAttributes& accessUnitAttributes)
{
    accessUnitQueue_[wpAuQueue_].wpBegin_ = wp_;
    accessUnitQueue_[wpAuQueue_].attributes_ = accessUnitAttributes;
}

bool RingBuffer::write(const unsigned char* data, uint32_t size)
{
    if(availableSpace() < size)
        return false; // not enough space in ring buffer

    if(wp_ + size >= size_) // wrap-around
    {
        uint32_t half = size_ - wp_;

        memcpy(&data_[wp_], data, half);
        size -= half;

        if(size > 0)
            memcpy(&data_[0], &data[half], size);

        wp_ = size;
    }
    else if(size > 0)
    {
        memcpy(&data_[wp_], data, size);
        wp_ += size;
    }

    return true;
}

bool RingBuffer::sendAccessUnitEnd(uint32_t dependentViewOffset)
{
    if(accessUnitQueue_[wpAuQueue_].wpBegin_ == INVALID_WP)
        return false; // this function is not paired with sendAccessUnitBegin() properly

    uint32_t next = wpAuQueue_ + 1;
    if(next >= maxAccessUnits_)
        next = 0;

    if(next == rpAuQueue_)
        return false; // access-unit queue is full
    if(dependentViewOffset == 0) {
    accessUnitQueue_[wpAuQueue_].wpDependentView_ = 0;
    } else {
    accessUnitQueue_[wpAuQueue_].wpDependentView_ = accessUnitQueue_[wpAuQueue_].wpBegin_
        + dependentViewOffset;
    }
    accessUnitQueue_[wpAuQueue_].wpEnd_ = wp_;
    wpAuQueue_ = next;
    return true;
}

bool RingBuffer::currAccessUnitInfo(unsigned char* &dataBegin,
                                    uint32_t& dataSize,
                                    bool& dataWrappedAround) const
{
    uint32_t wp_begin = accessUnitQueue_[wpAuQueue_].wpBegin_;

    if(wp_begin == INVALID_WP)
    {
        // It's not called between sendAccessUnitBegin() and sendAccessUnitEnd()
        return false;
    }

    dataBegin = &data_[wp_begin];

    if(wp_ >= wp_begin)
        dataSize = wp_ - wp_begin;
    else
        dataSize = size_ + wp_ - wp_begin;

    dataWrappedAround = (wp_ < wp_begin && wp_ != 0);

    return true;
}

unsigned char* RingBuffer::getWritePointer() const
{
    return &data_[wp_];
}

bool RingBuffer::canWritePointerWraparound() const
{
    uint32_t rp_snapshot = rp_;
    return (rp_snapshot <= wp_ && rp_snapshot > 0);
}

bool RingBuffer::advanceWritePointer(uint32_t bytes)
{
    if(availableSpace() < bytes)
        return false; // not enough space in ring buffer

    if(wp_ + bytes >= size_) // wrap-around
        wp_ = wp_ + bytes - size_;
    else
        wp_ = wp_ + bytes;

    return true;
}

bool RingBuffer::advanceWritePointerUntilWraparound()
{
    if(!canWritePointerWraparound())
        return false;

    wp_ = 0;
    return true;
}


bool RingBuffer::readAccessUnitPeek(const unsigned char* &data,
                                    uint32_t &size,
                                    AccessUnitAttributes& accessUnitAttributes) const
{
    const unsigned char* dependentViewData;
    uint32_t dependentViewSize;

    bool ret = readAccessUnitPeek(data, size, dependentViewData,
                  dependentViewSize, accessUnitAttributes);
    size = size + dependentViewSize;

    return ret;
}

bool RingBuffer::readAccessUnitPeek(const unsigned char* &baseViewData,
                                    uint32_t &baseViewSize,
                    const unsigned char* &dependentViewData,
                                    uint32_t &dependentViewSize,
                                    AccessUnitAttributes& accessUnitAttributes) const
{
    if(rpAuQueue_ == wpAuQueue_)
        return false; // access-unit queue is empty

    uint32_t begin = accessUnitQueue_[rpAuQueue_].wpBegin_;
    uint32_t end   = accessUnitQueue_[rpAuQueue_].wpEnd_;
    uint32_t dependBegin = accessUnitQueue_[rpAuQueue_].wpDependentView_;

    accessUnitAttributes = accessUnitQueue_[rpAuQueue_].attributes_;

    if(begin <= end) // no wrap-around
    {
    baseViewData = &data_[begin];
        if(dependBegin != 0) {
        baseViewSize = dependBegin - begin;
        dependentViewData = &data_[dependBegin];
        dependentViewSize = end - dependBegin;
    } else {
        baseViewSize = end - begin;
        dependentViewSize = 0;
        dependentViewData = NULL;
    }
    }
    else
    {
#ifdef __SUPPORT_AU_MERGE__

        // wrap-around, use "merge_buffer" (not needed if it's hardware solution)

        uint32_t half = size_ - begin;
        baseViewSize = half + end;

        if(baseViewSize > mergeBuffer_.size())
            mergeBuffer_.resize(size);

        memcpy(&mergeBuffer_[0], &data_[begin], half);
        if(end > 0)
            memcpy(&mergeBuffer_[half], &data_[0], end);

        baseViewData = &mergeBuffer_[0];
    if(dependBegin != 0) {
        dependentViewSize = baseViewSize - (dependBegin - begin);
        baseViewSize = baseViewSize - dependentViewSize;
        dependentViewData =  &mergeBuffer_[baseViewSize];
    } else {
        dependentViewSize = 0;
        dependentViewData = NULL;
    }


#else // __SUPPORT_AU_MERGE__

        if(end > 0)
        {
            assert(false); // we're not able to handle AU wrap-around.
        }
        else
        {
            // The remaining contiguous space size (in ring-buffer) matches the
            // access-unit size exactly.
        baseViewData = &data_[begin];
        if(dependBegin != 0) {
        baseViewSize = dependBegin - begin;
        dependentViewData = &data_[dependBegin];
        dependentViewSize = size_ - dependBegin;
        } else {
        baseViewSize = size_ - begin;
        dependentViewData = NULL;
        dependentViewSize = 0;
        }

        }

#endif // __SUPPORT_AU_MERGE__
    }

    return true;
}

bool RingBuffer::readAccessUnitCommit()
{
    if(rpAuQueue_ == wpAuQueue_)
        return false; // access-unit queue is empty

    accessUnitQueue_[rpAuQueue_].wpBegin_ = INVALID_WP;
    rp_ = accessUnitQueue_[rpAuQueue_].wpEnd_;

    uint32_t next = rpAuQueue_ + 1;
    if(next >= maxAccessUnits_)
        next = 0;
    rpAuQueue_ = next;

    return true;
}

void RingBuffer::flush()
{
    while(1)
    {
        // keep reading access-unit from queue until it is empty
        if(!readAccessUnitCommit())
            return;
    }
}

void RingBuffer::getBufferFullness(uint32_t& totalBufferSizeInBytes, uint32_t& numBytesInUse, uint32_t& numFrames)
{
    if(wp_ >= rp_)
    {
        numBytesInUse = wp_ - rp_;
    }
    else
    {
        numBytesInUse = size_ - rp_ + wp_;
    }
    totalBufferSizeInBytes = size_;

    if(wpAuQueue_ >= rpAuQueue_)
    {
        numFrames = wpAuQueue_ - rpAuQueue_;
    } else
    {
        numFrames = maxAccessUnits_ - rpAuQueue_ + wpAuQueue_;
    }
}
