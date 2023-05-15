#ifndef LinkedList_h
#define LinkedList_h

/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include <assert.h>

namespace netflix {

template <typename T, typename U = T>
class LinkedList
{
public:
    LinkedList()
        : mFirst(T()), mLast(T()), mCount(0)
    {}
    ~LinkedList()
    {
        assert(mCount == 0);
        assert(!mFirst);
        assert(!mLast);
    }

    void clear()
    {
        mCount = 0;
        mFirst = mLast = T();
    }

    void deleteAll()
    {
        T t = mFirst;
        Deleter<T, U> deleter;
        while (t) {
            T tmp = t;
            t = t->next;
            deleter(tmp);
        }
        mCount = 0;
        mFirst = mLast = T();
    }

    void append(const T& t)
    {
        assert(t);
        t->next = T();
        if (!mFirst) {
            t->prev = T();
            mFirst = mLast = t;
        } else {
            assert(mLast);
            t->prev = mLast;
            mLast->next = t;
            mLast = t;
        }
        ++mCount;
    }
    void push_back(const T& t)
    {
        append(t);
    }
    void prepend(const T& t)
    {
        assert(t);
        t->prev = T();
        if (!mFirst) {
            t->next = T();
            mFirst = mLast = t;
        } else {
            t->next = mFirst;
            mFirst->prev = t;
            mFirst = t;
        }
        ++mCount;
    }
    void push_front(const T& t)
    {
        prepend(t);
    }
    void remove(const T t)
    {
        assert(t);
        assert(mCount > 0);
        if (mCount == 1) {
            assert(mFirst == mLast);
            assert(mFirst == t);
            mFirst = mLast = T();
        } else if (t == mFirst) {
            assert(!t->prev);
            mFirst = t->next;
            mFirst->prev = T();
        } else if (t == mLast) {
            assert(!t->next);
            mLast = t->prev;
            mLast->next = T();
        } else {
            t->prev->next = t->next;
            t->next->prev = t->prev;
        }
        t->next = t->prev = T();
        --mCount;
    }

    void insertBefore(const T& t, const T& before)
    {
        assert(t);
        assert(!contains(t));
        if (!before) {
            append(t);
        } else if (!before->prev) {
            prepend(t);
        } else {
            t->prev = before->prev;
            t->prev->next = t;
            t->next = before;
            before->prev = t;
            ++mCount;
        }
    }

    void insertAfter(const T& t, const T& after)
    {
        assert(t);
        if (!after) {
            prepend(t);
        } else if (!after->next) {
            append(t);
        } else {
            t->prev = after;
            t->next = after->next;
            t->next->prev = t;
            after->next = t;
            ++mCount;
        }
    }

    void moveToEnd(const T& t)
    {
        if (t != mLast) {
            remove(t);
            t->next = t->prev = T();
            append(t);
        }
    }

    void moveToBeginning(const T& t)
    {
        if (t != mFirst) {
            remove(t);
            t->next = t->prev = T();
            prepend(t);
        }
    }

    T first() const { return mFirst; }
    T last() const { return mLast; }
    T pop_front()
    {
        if (mFirst) {
            T ret = mFirst;
            remove(mFirst);
            return ret;
        }
        return T();
    }
    T pop_back()
    {
        if (mLast) {
            T ret = mLast;
            remove(mLast);
            return ret;
        }
        return T();
    }

    bool contains(const T& t) const
    {
        return t->next || t->prev || mFirst == t || mLast == t;
    }

    int count() const { return mCount; }
    int size() const { return mCount; }
    bool empty() const { return isEmpty(); }
    bool isEmpty() const { return !mCount; }

    void swap(LinkedList<T, U> &other)
    {
        std::swap(mFirst, other.mFirst);
        std::swap(mLast, other.mLast);
        std::swap(mCount, other.mCount);
    }

private:
    template<typename V, typename W>
    struct Deleter
    {
        void operator()(V& v)
        {
            delete v;
        }
    };

    template<typename V>
    struct Deleter<shared_ptr<V>, V>
    {
        void operator()(shared_ptr<V>& v)
        {
            v->next.reset();
            v->prev.reset();
            v.reset();
        }
    };

    T mFirst, mLast;
    int mCount;
};

}

namespace std
{
template <typename T, typename U>
inline void swap(netflix::LinkedList<T, U> &l, netflix::LinkedList<T, U> &r) { l.swap(r); }
}

#endif
