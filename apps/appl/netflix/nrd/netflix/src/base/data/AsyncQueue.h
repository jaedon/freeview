/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef AsyncQueue_h
#define AsyncQueue_h

#include <list>
#include <algorithm>
#include <functional>
#include <unistd.h>

#include <nrdbase/Noncopyable.h>
#include <nrdbase/Mutex.h>
#include <nrdbase/NFErr.h>
#include <nrdbase/ConditionVariable.h>
#include <nrdbase/ScopedMutex.h>

namespace netflix {

/*
 * The AsyncQueue class enables simple message passing between threads.
 * It lets you push objects into it and pop them from other threads.
 * You can pop waiting forever or with a timeout (pop). You can also try to
 * pop either acquiring the mutex (try_pop) or not (try_fast_pop).
 *
 * Pushing an item into the queue implies transferring ownership of
 * the item to the queue. If the queue is cleared, items are deleted
 * by the queue. When you pop an item from the queue, ownership is
 * transferred back to the caller with a shared pointer.
 *
 * AsyncQueue also has a notion of being "closed" which means that
 * pushing items into it does nothing and trying to pop items returns
 * an empty shared pointer immediately. This is a useful mechanism for
 * telling threads to exit.
 *
 * By closing the queue with wait = true, you can also ensure
 * that all waiting threads terminate (if their loop is written correctly).
 *
 * You should use a shared_ptr to share the queue among threads. The idiom
 * is as follows:
 *
 * <Where "queue" is an AsyncQueue<Item>::SharedPtr passed to the thread
 * in its constructor>
 *
 * Thread::Run()
 * {
 *      while(queue->is_open())
 *      {
 *          if(shared_ptr<Item> item = queue->pop())
 *          {
 *              // do something with item
 *          }
 *      }
 * }
 *
 */

template <typename T>
class AsyncQueue : private Noncopyable
{
public:

    typedef shared_ptr<AsyncQueue> SharedPtr;
    typedef shared_ptr<T> Item;
    typedef typename std::list<Item> List;
    typedef typename std::list<Item>::iterator ListIterator;
    typedef typename std::list<Item>::const_iterator ConstListIterator;

    // You can install an instance of this class to filter the
    // items that are pushed into the queue. If the push
    // function returns true, the item will be pushed into
    // the queue, otherwise, it will be discarded.

    class Filter : public std::unary_function<Item, bool>
    {
    public:
        virtual ~Filter() {}
        virtual bool operator()(const Item &) const { return true; }
    };

    AsyncQueue()
        :
        mMutex( ZERO_MUTEX , "AsyncQueueMutex" ),
        mWaiters( 0 ),
        mClosed( false )
    {}

    // Pushes the item at the end of the queue and signals
    // all waiting threads. Pushing an item implies transferring
    // ownership of it to the queue. If the queue is closed, the
    // item will be destroyed and not added to the queue.
    // Returns the number of items in the queue after the push.

    std::size_t push(shared_ptr<T> value)
    {
        std::size_t result;
        {
            ScopedMutex lock(mMutex);
            bool ready = false;
            if(should_push(value)) {
                ready = mQueue.empty();
                mQueue.push_back(value);
            }
            result = mQueue.size();
            if(ready)
                mCondition.signal();
        }
        return result;
    }

    std::size_t push(T *value)
    {
        return push(shared_ptr<T>(value));
    }

    // Pushes a list of items at the end of the queue and signals
    // all waiting threads. Pushing an item implies transferring
    // ownership of it to the queue. If the queue is closed, the
    // item will be destroyed and not added to the queue.
    // Returns the number of items in the queue after the push.

    std::size_t push(const List & list)
    {
        std::size_t result;
        {
            ScopedMutex lock(mMutex);
            bool ready = false;
            if(!mClosed) {
                if(!mFilter) {
                    ready = mQueue.empty();
                    mQueue.insert(mQueue.end(), list.begin(), list.end());
                } else {
                    const bool wasEmpty = mQueue.empty();
                    std::remove_copy_if(list.begin(), list.end(), mQueue.end(), std::not1(*mFilter));
                    ready = wasEmpty && !mQueue.empty();
                }
            }
            result = mQueue.size();
            if(ready)
                mCondition.broadcast();
        }
        return result;
    }

    // Pushes the item at the front of the queue and signals
    // all waiting threads. Pushing an item implies transferring
    // ownership of it to the queue. If the queue is closed, the
    // item will be destroyed and not added to the queue.
    // Returns the number of items in the queue after the push.

    std::size_t push_front(const shared_ptr<T> &value)
    {
        std::size_t result;
        {
            ScopedMutex lock(mMutex);
            bool ready = false;
            if(should_push(value)) {
                ready = mQueue.empty();
                mQueue.push_front(value);
            }
            result = mQueue.size();
            if(ready)
                mCondition.signal();
        }
        return result;
    }

    std::size_t push_front(T *value)
    {
        return push_front(shared_ptr<T>(value));
    }

    // Waits for the queue to have at least one item,
    // pops it from the front of the queue and returns it.
    // If there is a timeout or other error waiting for
    // an item, or the queue is closed, returns an empty
    // shared pointer.

    shared_ptr<T> pop(const Time & time = Time(0))
    {
        shared_ptr<T> result;
        NFErr error;
        ScopedMutex lock(mMutex);
        while(mQueue.empty() && !mClosed && error.ok()) {
            ++mWaiters;
            error = mCondition.wait(&mMutex, time);
            --mWaiters;
        }
        if(!mQueue.empty() && !mClosed && error.ok()) {
            result = mQueue.front();
            mQueue.pop_front();
        }
        return result;
    }

    // Waits for the queue to have at least one item,
    // pops at most 'max' items from the front of the
    // queue and returns them. If 'max' is 0, it pops
    // all items. If there is a timeout or other error
    // waiting for an item, or the queue is closed,
    // returns an empty list.

    List pop_all(const Time & time = Time(0), size_t max = 0)
    {
        List result;
        NFErr error;
        ScopedMutex lock(mMutex);
        while(mQueue.empty() && !mClosed && error.ok()) {
            ++mWaiters;
            error = mCondition.wait(&mMutex, time);
            --mWaiters;
        }
        if(!mQueue.empty() && !mClosed && error.ok()) {
            if (max == 0 || (mQueue.size() <= max)) {
                result.swap(mQueue);
            } else {
                ListIterator last = mQueue.begin();
                std::advance(last, (ssize_t)max); // std::advance requires that the second argument is signed
                result.splice(result.begin(), mQueue, mQueue.begin(), last);
            }
        }
        return result;
    }

    // Acquires the queue's mutex and returns the item
    // in the front of the queue. If the queue is empty,
    // or closed, returns an empty shared pointer.

    shared_ptr<T> try_pop()
    {
        shared_ptr<T> result;
        ScopedMutex lock(mMutex);
        if(!mClosed && !mQueue.empty()) {
            result = mQueue.front();
            mQueue.pop_front();
        }
        return result;
    }


    // The same as try_pop, except that it returns an empty
    // shared pointer if it cannot acquire the queue's mutex
    // immediately.

    shared_ptr<T> try_fast_pop()
    {
        shared_ptr<T> result;
        if(mMutex.tryLock() == NFErr_OK) {
            if(!mClosed && !mQueue.empty()) {
                result = mQueue.front();
                mQueue.pop_front();
            }
            mMutex.unlock();
        }
        return result;
    }


    // Returns true if the queue is empty.

    bool empty() const
    {
        ScopedMutex lock(mMutex);
        return mQueue.empty();
    }

    // Returns the number of items in the queue.

    std::size_t size() const
    {
        ScopedMutex lock(mMutex);
        return mQueue.size();
    }

    // Removes all items from the queue, whether it is closed
    // or not. Returns the number of items that were in the
    // queue before it was cleared.

    std::size_t clear()
    {
        Queue q;
        {
            ScopedMutex lock(mMutex);
            std::swap(q, mQueue);
        }
        return q.size();
    }

    // Closes the queue, causing it to ignore pushes and
    // pops. Wakes up all waiting threads. If wait is true,
    // this function will wait until no other threads are
    // waiting on the queue.

    void close(bool wait = true)
    {
        bool done = false;
        {
            ScopedMutex lock(mMutex);
            mClosed = true;
            done = (mWaiters == 0);
        }
        while(!done) {
            {
                ScopedMutex lock(mMutex);

                mCondition.broadcast();

                // If the caller does not want to wait, we bail
                // after we have broadcast to all waiting threads.
                if(!wait)
                    return;

                done = (mWaiters == 0);
            }
            usleep(1000);
        }
    }

    // Opens the queue for business again.

    void open()
    {
        ScopedMutex lock(mMutex);
        mClosed = false;
    }

    // Returns true if the queue has been closed.

    bool is_closed() const
    {
        ScopedMutex lock(mMutex);
        return mClosed;
    }

    // Returns true if the queue is open.

    bool is_open() const
    {
        return !is_closed();
    }

    // Set a new filter for pushed items.

    void set_filter(const shared_ptr<Filter> &filter)
    {
        ScopedMutex lock(mMutex);
        mFilter = filter;
        if(mFilter) {
            for(ListIterator it = mQueue.begin(); it != mQueue.end();) {
                if(!(*mFilter)(*it))
                    mQueue.erase(it++);
                else
                    ++it;
            }
        }
    }

    typedef typename std::list< shared_ptr<T> > Queue;

    Queue pending() const {
        ScopedMutex lock(mMutex);
        if(!mClosed)
            return mQueue;
        return Queue();
    }

private:

    inline bool should_push(const shared_ptr<T> &item)
    {
        if(mClosed)
            return false;
        if(!mFilter)
            return true;
        return (*mFilter)(item);
    }

    mutable Mutex       mMutex;
    ConditionVariable   mCondition;
    Queue               mQueue;
    std::size_t         mWaiters;
    bool                mClosed;
    shared_ptr<Filter>  mFilter;
};

} // netflix


#endif
