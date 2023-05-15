/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef RefCounted_h
#define RefCounted_h

#include <nrdbase/config.h>
#include <nrdbase/AtomicCount.h>

namespace netflix {
class RefCountedBase
{
public:
    RefCountedBase(int t, int r = 1) : mCount(r), mType(t) {}
    virtual ~RefCountedBase() {}

    inline int ref() { return mCount.ref(); }
    inline int deref() // deletes this if ref becomes 0
    {
        const int result = mCount.deref();
        if (!result)
            delete this;
        return result;
    }
    inline long long count() const { return mCount.count(); }
    int type() const { return mType; }
protected:
    AtomicCount mCount;
    const int mType;
};

template <typename T>
class RefCounted : public RefCountedBase
{
public:
    RefCounted(int type = 0, const T &tt = T())
        : RefCountedBase(type), mT(tt)
    {}

    const T *ptr() const { return &mT; }
    T *ptr() { return &mT; }
    const T &reference() const { return mT; }
    T &reference() { return mT; }

    enum DetachMode {
        Force,
        Count
    };
    RefCounted<T> *detach(DetachMode mode = Count)
    {
        if (mode == Count && count() == 1)
            return this;

        RefCounted<T> *ret = new RefCounted<T>(type(), mT);
        deref();
        return ret;
    }
private:
    T mT;
};
}

#endif
