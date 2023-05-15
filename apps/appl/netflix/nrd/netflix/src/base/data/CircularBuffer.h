/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef __CIRCULARBUFFER_H__
#define __CIRCULARBUFFER_H__

#include <assert.h>
#include <nrdbase/tr1.h>
namespace netflix {

template <typename T>
class CircularBufferDestroy
{
public:
    static void destroy(T &) { }
};

template <typename T>
class CircularBufferDestroy<shared_ptr<T> >
{
public:
    static void destroy(shared_ptr<T> &t) { t.reset(); }
};

template <typename T>
class CircularBuffer
{
public:
    template <typename BufferPointer>
    class iterator_base
    {
    public:
        inline iterator_base(BufferPointer buffer, int index) : mBuffer(buffer), mIndex(index) { }
        inline iterator_base(const iterator_base &other) : mBuffer(other.mBuffer), mIndex(other.mIndex) { }
        template <typename P> inline iterator_base(const iterator_base<P> &other) : mBuffer(other.buffer()), mIndex(other.index()) {}

        inline bool operator==(const iterator_base &other) const { return mBuffer == other.mBuffer && mIndex == other.mIndex; }
        inline bool operator!=(const iterator_base &other) const { return !(*this == other); }
        inline iterator_base &operator++() { ++mIndex; return *this; }

        inline const T &operator*() const { return mBuffer->at(mIndex); }
        inline const T *operator->() const { return &operator*(); }

        inline int index() const { return mIndex; }
        inline BufferPointer buffer() const { return mBuffer; }
    protected:
        BufferPointer mBuffer;
        int mIndex;
    };

    class iterator : public iterator_base<CircularBuffer*>
    {
        inline iterator(CircularBuffer *buffer, int index) : iterator_base<CircularBuffer*>(buffer, index) { }
    public:
        inline T &operator*() { return iterator_base<CircularBuffer*>::mBuffer->at(iterator_base<CircularBuffer*>::mIndex); }
        inline T *operator->() { return &operator*(); }
    private:
        friend class CircularBuffer<T>;
    };

    typedef iterator_base<const CircularBuffer*> const_iterator;

    inline CircularBuffer(int capacity=0) : mArray(0), mFirst(0), mLast(0), mCount(0), mCapacity(0) { resize(capacity); }
    inline ~CircularBuffer()
    {
        delete [] mArray;
    }
    inline CircularBuffer(const CircularBuffer<T> &other) : mArray(0) { copy(other); }
    inline CircularBuffer<T> &operator=(const CircularBuffer<T> &other)
    {
        copy(other);
        return *this;
    }

    inline void copy(const CircularBuffer<T> &other)
    {
        delete [] mArray;
        mFirst = other.mFirst;
        mLast = other.mLast;
        mCount = other.mCount;
        mCapacity = other.mCapacity;
        mArray = new T[mCapacity];
        for (int i = 0; i < mCount; ++i) {
            mArray[i] = other.mArray[i];
        }
    }

    void resize(int capacity)
    {
        if(capacity == mCapacity)
            return;
        T *old = mArray;
        if(capacity) {
            mArray = new T[capacity];
        } else {
            mArray = 0;
        }
        mCount = std::min(capacity, mCount);
        for(int i=0; i<mCount; ++i)
            mArray[i] = old[(mFirst + i) % mCapacity];
        delete [] old;
        mFirst = 0;
        mLast = std::max(mCount - 1, 0);
        mCapacity = capacity;
    }

    inline void clear()
    {
        for (int i=0; i<mCount; ++i) {
            CircularBufferDestroy<T>::destroy(operator[](i));
        }

        mCount = mFirst = mLast = 0;
    }

    void swap(CircularBuffer<T> &other)
    {
        std::swap(mCount, other.mCount);
        std::swap(mCapacity, other.mCapacity);
        std::swap(mLast, other.mLast);
        std::swap(mFirst, other.mFirst);
        std::swap(mArray, other.mArray);
    }

    inline void append(const T &t)
    {
        assert(mCount <= mCapacity);
        assert(mCapacity);
        if(mCount) {
            mLast = (mLast + 1) % mCapacity;
            if(mLast == mFirst)
                mFirst = (mFirst + 1) % mCapacity;
            else
                ++mCount;
        } else {
            mFirst = mLast = 0;
            mCount = 1;
        }
        mArray[mLast] = t;
    }
    inline void push_back(const T &t)
    {
        append(t);
    }

    inline const_iterator begin() const { return const_iterator(this, 0); }
    inline const_iterator end() const { return const_iterator(this, mCount); }
    inline const_iterator cbegin() const { return begin(); }
    inline const_iterator cend() const { return end(); }

    inline iterator begin() { return iterator(this, 0); }
    inline iterator end() { return iterator(this, mCount); }

    inline const T &operator[](int idx) const
    {
        assert(idx < mCount);
        assert(idx >= 0);
        return mArray[(mFirst + idx) % mCapacity];
    }
    inline T &operator[](int idx)
    {
        assert(idx < mCount);
        assert(idx >= 0);
        return mArray[(mFirst + idx) % mCapacity];
    }

    inline const T &at(int idx) const { return operator[](idx); }
    inline T &at(int idx) { return operator[](idx); }

    inline const T &first() const { assert(mCount); return mArray[mFirst]; }
    inline const T &last() const { assert(mCount); return mArray[mLast]; }
    inline T &first() { assert(mCount); return mArray[mFirst]; }
    inline T &last() { assert(mCount); return mArray[mLast]; }

    inline T pop_front()
    {
        const int front = mFirst;
        const T ret = mArray[front];
        mFirst = (mFirst + 1) % mCapacity;
        --mCount;
        CircularBufferDestroy<T>::destroy(mArray[front]);
        return ret;
    }
    inline T pop_back()
    {
        const int back = mLast;
        const T ret = mArray[back];
        --mLast;
        if(mLast < 0)
            mLast = mCapacity-1;
        --mCount;
        CircularBufferDestroy<T>::destroy(mArray[back]);
        return ret;
    }

    inline int capacity() const { return mCapacity; }
    inline int count() const { return mCount; }
    inline int size() const { return mCount; }
    inline bool isEmpty() const { return !mCount; }
    inline bool empty() const { return isEmpty(); }
private:
    T *mArray;
    int mFirst, mLast, mCount, mCapacity;
};

} // namespace netflix

namespace std
{
template <typename T>
inline void swap(netflix::CircularBuffer<T> &l, netflix::CircularBuffer<T> &r) { l.swap(r); }
}

#endif /* __CIRCULARBUFFER_H__ */
