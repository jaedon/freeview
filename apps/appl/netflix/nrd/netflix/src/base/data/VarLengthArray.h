/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef VARLENGTHARRAY_H
#define VARLENGTHARRAY_H

#include <nrdbase/config.h>
#include <nrdbase/Exportable.h>
#include <assert.h>

namespace netflix {

enum { ComplexType = true };

template<typename T, size_t Prealloc, bool Complex = false>
class NRDP_EXPORTABLE VarLengthArray
{
public:
    VarLengthArray(size_t size = 0);
    VarLengthArray(const VarLengthArray<T, Prealloc, Complex>& other);

    ~VarLengthArray();

    void clear();
    void resize(size_t size);

    T* data() { assert(mSize); return mHeapData ? mHeapData : reinterpret_cast<T*>(&mStackData[0]); }
    const T* data() const { assert(mSize); return mHeapData ? mHeapData : reinterpret_cast<const T*>(&mStackData[0]); }
    const T* constData() const { return data(); }

    size_t size() const { return mSize; }
    size_t heapSize() const { return mHeapSize; }
    size_t heapBytes() const { return mHeapSize * sizeof(T); }
    bool isEmpty() const { return !mSize; }

    T& front() { assert(mSize); return mHeapData ? *mHeapData : *reinterpret_cast<T*>(&mStackData[0]); }
    const T& front() const { assert(mSize); return mHeapData ? *mHeapData : *reinterpret_cast<const T*>(&mStackData[0]); }
    T& back() { assert(mSize); return mHeapData ? *(mHeapData + (mSize - 1)) : *(reinterpret_cast<T*>(&mStackData[0]) + (mSize - 1)); }
    const T& back() const { assert(mSize); return mHeapData ? *(mHeapData + (mSize - 1)) : *(reinterpret_cast<const T*>(&mStackData[0]) + (mSize - 1)); }

    void append(const T& item);

    T& operator[](size_t pos);
    const T& operator[](size_t pos) const;
    const T& at(size_t pos) const;

    VarLengthArray<T, Prealloc, Complex>& operator=(const VarLengthArray<T, Prealloc, Complex>& other);

private:
    void growHeap(size_t newSize);

private:
    char mStackData[Prealloc * sizeof(T)];
    T* mHeapData;
    size_t mSize, mHeapSize;
};

template<typename T, size_t Prealloc, bool Complex>
VarLengthArray<T, Prealloc, Complex>::VarLengthArray(size_t size)
    : mHeapData(0), mSize(size), mHeapSize(0)
{
    if (size <= Prealloc) {
        if (!Complex)
            return;
        // initialize the stack
        for (size_t i = 0; i < size; ++i) {
            new (reinterpret_cast<T*>(&mStackData[0]) + i) T;
        }
    } else {
        // bad call by the user but we need to init our heap data
        mHeapData = static_cast<T*>(calloc(size, sizeof(T)));
        mHeapSize = size;
        if (!Complex)
            return;
        for (size_t i = 0; i < size; ++i) {
            new (mHeapData + i) T;
        }
    }
}

template<typename T, size_t Prealloc, bool Complex>
VarLengthArray<T, Prealloc, Complex>::VarLengthArray(const VarLengthArray<T, Prealloc, Complex>& other)
    : mHeapData(0), mSize(0), mHeapSize(0)
{
    operator=(other);
}

template<typename T, size_t Prealloc, bool Complex>
VarLengthArray<T, Prealloc, Complex>::~VarLengthArray()
{
    clear();
}

template<typename T, size_t Prealloc, bool Complex>
void VarLengthArray<T, Prealloc, Complex>::clear()
{
    resize(0);
    mSize = mHeapSize = 0;
    if (mHeapData) {
        free(mHeapData);
        mHeapData = 0;
    }
}

template<typename T, size_t Prealloc, bool Complex>
void VarLengthArray<T, Prealloc, Complex>::growHeap(size_t newSize)
{
    mHeapSize += newSize + Prealloc;
    assert(mHeapSize >= mSize);
    mHeapData = static_cast<T*>(malloc(mHeapSize * sizeof(T)));
}

template<typename T, size_t Prealloc, bool Complex>
void VarLengthArray<T, Prealloc, Complex>::resize(size_t size)
{
    if (size == mSize)
        return;

    //printf("resizing %u => %u (%u)\n", mSize, size, Prealloc);

    if (size <= Prealloc || size < mHeapSize) {
        if (!Complex) {
            mSize = size;
            return;
        }
        if (size < mSize) {
            // resized smaller, run the d'tor on elements that are now gone
            if (mHeapData) {
                for (size_t i = size; i < mSize; ++i) {
                    reinterpret_cast<T*>(&mHeapData[i])->~T();
                }
            } else {
                for (size_t i = size; i < mSize; ++i) {
                    (reinterpret_cast<T*>(&mStackData[0]) + i)->~T();
                }
            }
        } else {
            assert(size > mSize);
            // initialize the new elements
            if (mHeapData) {
                for (size_t i = mSize; i < size; ++i) {
                    new (&mHeapData[i]) T;
                }
            } else {
                for (size_t i = mSize; i < size; ++i) {
                    new (reinterpret_cast<T*>(&mStackData[0]) + i) T;
                }
            }
        }
        mSize = size;
        return;
    }
    if (size > mHeapSize) {
        T* oldData = mHeapData;
        growHeap(size - mHeapSize);
        if (!oldData) {
            // copy the stack data over to the heap data
            if (Complex) {
                for (size_t i = 0; i < mSize; ++i) {
                    const T& t = *(reinterpret_cast<T*>(&mStackData[0]) + i);
                    new (&mHeapData[i]) T(t);
                    (reinterpret_cast<T*>(&mStackData[0]) + i)->~T();
                }
            } else {
                memcpy(mHeapData, mStackData, mSize * sizeof(T));
            }
        } else {
            // copy the old heap to the new heap
            if (Complex) {
                for (size_t i = 0; i < mSize; ++i) {
                    new (&mHeapData[i]) T(oldData[i]);
                    reinterpret_cast<T*>(&oldData[i])->~T();
                }
            } else {
                memcpy(mHeapData, oldData, mSize * sizeof(T));
            }
            free(oldData);
        }
    }
    assert(mSize < size);
    if (Complex) {
        // run the c'tor on any new elements
        if (mHeapData) {
            for (size_t i = mSize; i < size; ++i) {
                new (&mHeapData[i]) T;
            }
        } else {
            for (size_t i = mSize; i < size; ++i) {
                new (&mStackData[i * sizeof(T)]) T;
            }
        }
    } else {
        // memset the new data
        if (mHeapData) {
            memset(mHeapData + mSize, 0, size - mSize);
        } else {
            memset(&mStackData[mSize * sizeof(T)], 0, size - mSize);
        }
    }
    mSize = size;
}

template<typename T, size_t Prealloc, bool Complex>
VarLengthArray<T, Prealloc, Complex>& VarLengthArray<T, Prealloc, Complex>::operator=(const VarLengthArray<T, Prealloc, Complex>& other)
{
    // clear any existing items
    resize(0);

    if (other.mHeapData) {
        if (!mHeapData || other.mHeapSize > mHeapSize) {
            mHeapData = static_cast<T*>(realloc(mHeapData, other.mHeapSize * sizeof(T)));
        }
        mHeapSize = other.mHeapSize;
        if (Complex) {
            for (size_t i = 0; i < other.mSize; ++i) {
                mHeapData[i] = other.mHeapData[i];
            }
        } else {
            memcpy(mHeapData, other.mHeapData, other.mSize * sizeof(T));
        }
    } else {
        if (mHeapData) {
            free(mHeapData);
            mHeapData = 0;
        }

        assert(other.mSize <= Prealloc);
        if (Complex) {
            for (size_t i = 0; i < other.mSize; ++i) {
                *(static_cast<T*>(&mStackData[0]) * i) = *(static_cast<T*>(&other.mStackData[0]) * i);
            }
        } else {
            memcpy(mStackData, other.mStackData, other.mSize * sizeof(T));
        }
    }
    mSize = other.mSize;
    return *this;
}

template<typename T, size_t Prealloc, bool Complex>
void VarLengthArray<T, Prealloc, Complex>::append(const T& item)
{
    resize(mSize + 1);
    if (mSize <= Prealloc) { // good, we can take the fast path
        *(reinterpret_cast<T*>(&mStackData[0]) + (mSize - 1)) = item;
    } else {
        mHeapData[mSize - 1] = item;
    }
}

template<typename T, size_t Prealloc, bool Complex>
const T& VarLengthArray<T, Prealloc, Complex>::at(size_t pos) const
{
    assert(pos < mSize);
    if (mHeapData)
        return *(mHeapData + pos);
    return *(reinterpret_cast<const T*>(&mStackData[0]) + pos);
}

template<typename T, size_t Prealloc, bool Complex>
const T& VarLengthArray<T, Prealloc, Complex>::operator[](size_t pos) const
{
    return at(pos);
}

template<typename T, size_t Prealloc, bool Complex>
T& VarLengthArray<T, Prealloc, Complex>::operator[](size_t pos)
{
    assert(pos < mSize);
    if (mHeapData)
        return *(mHeapData + pos);
    return *(reinterpret_cast<T*>(&mStackData[0]) + pos);
}

} // netflix

#endif
