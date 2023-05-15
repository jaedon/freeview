/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef PriorityQueue_h
#define PriorityQueue_h

#include <nrdbase/CircularBuffer.h>

namespace netflix {

template <typename T, int PriorityRange, int Capacity>
class PriorityQueue
{
public:
    template <typename PriorityQueuePointer>
    class iterator_base
    {
    public:
        inline iterator_base(PriorityQueuePointer buffer, int index) : mBuffer(buffer), mIndex(index) { }
        inline iterator_base(const iterator_base &other) : mBuffer(other.mBuffer), mIndex(other.mIndex) { }
        template <typename P> inline iterator_base(const iterator_base<P> &other) : mBuffer(other.buffer()), mIndex(other.index()) {}

        inline bool operator==(const iterator_base &other) const { return mBuffer == other.mBuffer && mIndex == other.mIndex; }
        inline bool operator!=(const iterator_base &other) const { return !(*this == other); }
        inline iterator_base &operator++() { ++mIndex; return *this; }

        inline const T &operator*() const { return mBuffer->at(mIndex); }
        inline const T *operator->() const { return &operator*(); }

        inline int index() const { return mIndex; }
        inline PriorityQueuePointer buffer() const { return mBuffer; }
    protected:
        PriorityQueuePointer mBuffer;
        int mIndex;
    };

    class iterator : public iterator_base<PriorityQueue*>
    {
    public:
        inline iterator(PriorityQueue *buffer, int index) : iterator_base<PriorityQueue*>(buffer, index) { }
        inline T &operator*() { return iterator_base<PriorityQueue*>::mBuffer->at(iterator_base<PriorityQueue*>::mIndex); }
        inline T *operator->() { return &operator*(); }
    };

    typedef iterator_base<const PriorityQueue*> const_iterator;

    PriorityQueue()
        : mSize(0)
    {
        assert(Capacity > 0);
        for (int i=0; i<PriorityRange; ++i) {
            mBuffers[i].resize(Capacity);
        }
    }

    void swap(PriorityQueue<T, PriorityRange, Capacity> &other)
    {
        std::swap(mSize, other.mSize);
        for (int i=0; i<PriorityRange; ++i) {
            mBuffers[i].swap(other.mBuffers[i]);
        }
    }

    inline const_iterator begin() const { return const_iterator(this, 0); }
    inline const_iterator end() const { return const_iterator(this, mSize); }
    inline const_iterator cbegin() const { return begin(); }
    inline const_iterator cend() const { return end(); }

    inline iterator begin() { return iterator(this, 0); }
    inline iterator end() { return iterator(this, mSize); }

    inline void push(const T &t, int priority)
    {
        assert(priority >= 0);
        assert(priority < PriorityRange);
        CircularBuffer<T> &buffer = mBuffers[priority];
        if (buffer.size() == buffer.capacity()) {
            buffer.resize(buffer.capacity() * 2);
        }
        buffer.push_back(t);
        ++mSize;
    }
    inline const T &top() const
    {
        assert(mSize);
        int buffer = 0;
        while (buffer < PriorityRange) {
            if (!mBuffers[buffer].empty())
                break;
            ++buffer;
        }
        assert(buffer < PriorityRange);
        return mBuffers[buffer].first();
    }
    inline T pop()
    {
        assert(mSize);
        int buffer = 0;
        while (buffer < PriorityRange) {
            if (!mBuffers[buffer].empty())
                break;
            ++buffer;
        }
        --mSize;
        return mBuffers[buffer].pop_front();
    }

    inline void clear()
    {
        for (int i=0; i<PriorityRange; ++i) {
            mBuffers[i].clear();
        }
        mSize = 0;
    }

    inline T &at(int idx)
    {
        assert(idx < mSize);
        assert(idx >= 0);
        int buffer = 0;
        while (buffer < PriorityRange) {
            const int size = mBuffers[buffer].size();
            if (idx < size)
                break;
            idx -= size;
            ++buffer;
        }
        return mBuffers[buffer][idx];
    }

    inline const T &at(int idx) const
    {
        assert(idx < mSize);
        assert(idx >= 0);
        int buffer = 0;
        while (buffer < PriorityRange) {
            const int size = mBuffers[buffer].size();
            if (idx < size)
                break;
            idx -= size;
            ++buffer;
        }
        assert(buffer < PriorityRange);
        return mBuffers[buffer][idx];
    }

    inline int size() const { return mSize; }
    inline int count() const { return mSize; }
    inline bool isEmpty() const { return !mSize; }
    inline bool empty() const { return !mSize; }
private:
    CircularBuffer<T> mBuffers[PriorityRange];
    int mSize;
};

} // namespace netflix

namespace std
{
template <typename T, int PriorityRange, int Capacity>
inline void swap(netflix::PriorityQueue<T, PriorityRange, Capacity> &l, netflix::PriorityQueue<T, PriorityRange, Capacity> &r) { l.swap(r); }
}

#endif
