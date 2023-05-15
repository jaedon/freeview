/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#ifndef MEDIACONTROL_RW_QUEUE_H
#define MEDIACONTROL_RW_QUEUE_H

#include <nrdbase/ScopedMutex.h>

namespace netflix {
namespace mediacontrol {

// RULES
//
// "WP" can only be modified by the sender side.
// "RP" can only be modified by the retriever side.
// "WP == RP" means queue is empty.
// "(WP+1) == RP" (with wrap-around handled) means queue is full.

/**
 * @class RWQueue RWQueue.h
 * @brief A Read/Write Pointers Based Template Queue Class.
 */
template <typename T>
class RWQueue
{
public:
    /**
     * Constructor.
     *
     * @param[in] maxItems number of items the queue can hold. (NOTE: the actual
     *            capacity is in fact one less than this number)
     */
    RWQueue(uint32_t maxItems) : wp_(0), rp_(0), maxItems_(maxItems)
    {
        items_ = new T[maxItems_];
    }

    /** Destructor. */
    ~RWQueue()
    {
        delete [] items_;
    }

    /**
     * Send one item into the queue.
     *
     * @param[in] item the new item to be send into the queue.
     *
     * @return true if succeeded, otherwise false (queue is full).
     */
    bool send(T& item)
    {
        // Calculate the next WP position with wrap-around handled.

        uint32_t next = wp_ + 1;
        if(next >= maxItems_)
            next = 0;

        // Queue is full if the next WP equals to the current RP.

        if(next == rp_)
            return false;

        // Complete sending by copying the new item into the items table, and then
        // move WP to the next position.

        items_[wp_] = item;
        wp_ = next;

        return true;
    }

    /**
     * Get one item (if any) from the queue (first sent first received).
     *
     * @param[out] item to receive the oldest item in queue (if any).
     *
     * @return true if succeeded, otherwise false (queue is empty).
     */
    bool get(T& item)
    {
        // Queue is empty if the current WP and RP are equal.

        if(rp_ == wp_)
            return false;

        // Copy the item corresponding to RP from the items table, into the
        // output item variable. Reset the item in table.

        item = items_[rp_];
        items_[rp_].reset();

        // Calculate the next RP position with wrap-around handled.

        uint32_t next = rp_ + 1;
        if(next >= maxItems_)
            next = 0;

        // Complete retrieval by moving RP to the next position.

        rp_ = next;
        return true;
    }

    /**
     * Discard all items in the queue.
     *
     * NOTE: the queue can only be safely emptied from the receiver side.
     */
    void flush()
    {
        while(rp_ != wp_) // keep looping until the queue is empty.
        {
            // Reset the item corresponding to RP in table.

            items_[rp_].reset();

            // Move RP to the next position with wrap-around handled.

            uint32_t next = rp_ + 1;
            if(next >= maxItems_)
                next = 0;
            rp_ = next;
        }
    }

private:
    uint32_t volatile wp_;
    uint32_t volatile rp_;
    uint32_t const maxItems_;
    T* items_;
};

/**
 * @class SRMWQueue SRMWQueue.h
 * @brief A Single-Reader-Multi-Writer Template Queue Class.
 */
template <typename T>
class SRMWQueue : public RWQueue<T>
{
public:
    /**
     * Constructor.
     *
     * @param[in] maxItems number of items the queue can hold. (NOTE: the actual
     *            capacity is in fact one less than this number)
     */
    SRMWQueue(uint32_t maxItems) :
        RWQueue<T>(maxItems),
        writeMutex_(SRMWQUEUE_MUTEX, "SRMWQueue")
    {}

    /**
     * Send one item into the queue.
     *
     * @param[in] item the new item to be send into the queue.
     *
     * @return true if succeeded, otherwise false (queue is full).
     */
    bool send(T& item)
    {
        // Multiple threads are expected to send items into the queue, so mutex
        // locking is required during sending.

        ScopedMutex scopedMutex(writeMutex_);
        return RWQueue<T>::send(item);
    }

private:
    mutable Mutex writeMutex_;
};

}} // namespace netflix::mediacontrol

#endif // MEDIACONTROL_RW_QUEUE_H
