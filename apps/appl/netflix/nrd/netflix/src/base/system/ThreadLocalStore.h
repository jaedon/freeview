/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef __THREADLOCALSTORE_H__
#define __THREADLOCALSTORE_H__

#include <pthread.h>
#include <nrdbase/Noncopyable.h>

namespace netflix {

template<typename T>
class ThreadLocalStore : public Noncopyable
{
public:
    enum Flag {
        None = 0x0,
        DontDelete = 0x1
    };
    ThreadLocalStore(unsigned int flags = None) { init(flags); }
    ThreadLocalStore(const T& t) { init(); set(t); }
    ThreadLocalStore(T* t) { init(); set(t); }
    ~ThreadLocalStore() { clear(); }

    unsigned int flags() const { return mFlags; }

    void clear() { pthread_key_delete(mKey); }

    void set(T* t) { setData(t); }

    void remove() { setData(0); }
    bool has() const { return getData() != 0; }

    T* get() { return reinterpret_cast<T*>(getData()); }
    const T* get() const { return reinterpret_cast<const T*>(getData()); }

    T* operator->() { return reinterpret_cast<T*>(getData()); }
    const T* operator->() const { return reinterpret_cast<const T*>(getData()); }

private:
    void init(unsigned int flags = None)
    {
        mFlags = flags;
        if (mFlags & DontDelete)
            pthread_key_create(&mKey, 0);
        else
            pthread_key_create(&mKey, deleteValue);
    }
    void setData(void* data)
    {
        if (!(mFlags & DontDelete))
            delete reinterpret_cast<T*>(getData());
        pthread_setspecific(mKey, data);
    }
    void* getData() const
    {
        return pthread_getspecific(mKey);
    }
    static void deleteValue(void* val)
    {
        delete reinterpret_cast<T*>(val);
    }

private:
    unsigned int mFlags;
    pthread_key_t mKey;
};

} // namespace netflix

#endif // __THREADLOCALSTORE_H__
