/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef PRODUCERCONSUMERLIST_H
#define PRODUCERCONSUMERLIST_H

/** @file ProducerConsumerList a list for concurrent access by a producer and
 * consumer
 *
 * ProducerConsumerList implements a list that can be shared by a producer
 * thread and a consumer thread (note that the list does not support more than
 * one producer or consumer). The list allows the producer to push items to the
 * end of the list while the consumer concurrently iterates elements and pops
 * elements from the front. The implementation was designed to not use mutexes
 * for performance reasons.  Also for performance reasons, the list does not
 * allocate or deallocate any memory after the list has been initialized.
 *
 * While the implementation was designed to not use a mutexes, by default it
 * does.  The reason is that as it is written currently, the implementation can
 * break if an optimizing compiler reorders certain instructions in an
 * inopportune way or if a hardware architecture reorders instructions in an
 * inopportune way during execution. Using the atomic type of C++11, the
 * mutex-free implementation can be easily updated to prevent these issues.
 * Until C++11 is ubitquitous, leaving the mutexes in is safest. That said,
 * using gcc and running on x86 the mutex-free implemention has proven safe.
 *
 * When the list is initialized, a pool of list elements is created. When the
 * producer adds an element to the list, the implementation takes an element
 * from the pool, initializes it with the object the user has pushed, and adds
 * it to the list. If the pool is exhausted, adding an element to the list will
 * fail. When the consumer pops elements from the front, the the implementation
 * marks the elements for recycling.  The push_back method available to the
 * producer recycles marked elements back to the pool before trying to get an
 * element from the pool to push back.
 *
 * To use the ProducerConsumerList, you may wish to implement a cleanup object
 * that the list calls when it recycles elements that the consumer has popped
 * from the font.  The cleanup object must be derived from the
 * IListElementCleanup, defined below.  Then, when you create a
 * ProducerConsumerList object, you specify how many pool items should be
 * allocated and you can also pass in a pointer to an instance of your cleanup
 * object.
 *
 * After you've created your ProducerConsumerList, you pass the producer thread
 * an IProducerListView interface to the list and pass the consumer an
 * IConsumerListView interface to the list. On the producer thread, call
 * push_back to add items to the list. On the consumer thread, call
 * IConsumerListView::updateListSnapshot when you want to make sure you are
 * seeing the latest elements that the producer has added.  The consumer will
 * not see a new element that the producer pushes to the back until
 * IConsumerListView::updateListSnapshot is called. Then, on the consumer
 * thread, iterate through the list and access elements as needed. When you are
 * done with elements call IConsumerListView::pop_front() to remove elements
 * from the front of the list.
 *
 * If you want to clear the list, return all the elements to the pool and reset
 * consumer's view of the list, call ProducerConsumerList::clear().  You should
 * only do this when the consumer thread is guaranteed to not be accessing any
 * of the IConsumerListView methods before clear() has returned.  The consumer
 * thread must then call updateListSnapshot() before accessing any other method
 * on the list.
 */

#define USE_MUTEXES_PCLIST

#include <nrdbase/StdInt.h>
#include <vector>
#include <assert.h>
#include <nrdbase/Mutex.h>
#include <nrdbase/MutexRanks.h>
#include <nrdbase/ScopedMutex.h>
#include <nrdbase/Log.h>


namespace netflix {

// Forward declarations
template<class T> class IConsumerListView;
template<class T> class IProducerListView;
template<class T> class IListElementCleanup;
template<class T> class Node;

/**
 * ProducerConsumerList
 *
 *  Implements methods that need to access both the consumer and producer
 *  interfaces. To use ProducerConsumerList, instantiate one of these, and then
 *  pass an IConsumerListView interface to the consumer thread and an
 *  IProducerListView to the producer thread.
 *
 */
template<class T> class ProducerConsumerList :
        public IConsumerListView<T>,
        public IProducerListView<T>
{
public:

    /**
     * param[in] poolSize specifies how many list elements containing objects T
     *           are allocated. The list will only be able to hold poolSize
     *           elements at a time.
     *
     * param[in] cleanupObj argument, if non-null, will be called upon whenever
     *           a list element is popped from the front of the
     *           consumer's view of the list and whenever all the
     *           elements are recycled via a call to clear().
     **/
    ProducerConsumerList(int poolSize, IListElementCleanup<T>* cleanupObj = NULL);
    virtual ~ProducerConsumerList();

    /**
     * This should only be called when the consumer thread is stopped and is not
     * stopped inside any list method. When the consumer thread resumes, it must
     * call updateListSnapshot() before accessing any other IConsumerListView
     * method.
     */
    void clear();

protected:
    bool updateListSnapshot();
    void popUnneededFromFront();
    Mutex mMutex;

};


/**
 * IConsumerListView
 *
 * The interface to a ProducerConsumerList object that the consumer thread
 * should use. The consumer can use the iterator defined within, begin() and
 * end(), to examine list elements.  When no longer needed, the consumer thread
 * should call pop_front() to indicate that the head element can by recycled by
 * the producer. An iterator pointing to begin() will be invalid after a
 * pop_front.
 *
 */
template<class T> class IConsumerListView
{
public:
    class iterator;

    IConsumerListView(IListElementCleanup<T>* cleanupObj, Mutex& mutex);
    virtual ~IConsumerListView();

    iterator begin();
    iterator end();
    void pop_front();

    // This should be called to update the consumer's view of the list so that
    // it can see the latest nodes that the producer may have added to the
    // tail. The consumer's list end node can change when this is called so it
    // should not be done in the middle of operations that expect the list tail
    // to remain constant.
    virtual bool updateListSnapshot() = 0;

    class iterator
    {
    public:
        iterator();
        iterator(Node<T>* node);
        ~iterator();

        bool operator==(const iterator&b);
        bool operator!=(const iterator&b);
        iterator& operator=(const iterator& b);
        // prefix operator
        iterator& operator++();
        // post fix
        iterator operator++(int);
        T* operator->();
        T& operator*();
        Node<T>* getNode();
    private:
        friend class IConsumerListView;
        Node<T> *mNode;
    };

protected:
    Node<T> *mConsumerHead, *mConsumerTail;

    // Gets updated to consumer head when listViewSnapshot is updated.
    iterator mBeginIt;

    IListElementCleanup<T>* mCleanupObj;
    Mutex& mMutex;

};

/** IListElementCleanup
 *
 * Derive a class from this to specify cleanup that should be executed when a
 * list element is popped from the consumer view, or, if it is never popped from
 * the consumer view, when it is recycled to the pool in a
 * ProducerConsumerList::clear(). This object does not need to be thread-safe
 * because a ProducerConsumerList::clear() should only come when the consumer
 * thread is guaranteed to not access the list. The cleanup will be called
 * exactly once on a list element that is removed from the list.
 */
template<class T> class IListElementCleanup
{
public:
    virtual ~IListElementCleanup()
    {
    }

    virtual void cleanup(T& t) = 0;
};

/**
 * IProducerListView
 *
 * The interface to a ProducerConsumerList object that the producer thread
 * should use. The producer can elements to the list via push_back. Internally,
 * push_back checks to see if the consumer has popped elements from the head of
 * the list. Any elements that have been popped are recycled.  If a cleanup
 * object has been provided, the cleanup method on the object is called on each
 * list element before it is recycled.
 */
template<class T> class IProducerListView
{
public:
    IProducerListView(int poolSize, Mutex& mutex);
    virtual ~IProducerListView();

    // Returns true on success, false if the nodePool is out of elements.
    bool push_back(T x);

    virtual void popUnneededFromFront() = 0;

protected:
    Node<T> *mHead, *mTail;

    class NodePool
    {
    public:
        NodePool(uint32_t poolSize);
        ~NodePool();

        Node<T>* getNode();
        void recycleNode(Node<T> *node);
    private:
        // We use a vector but don't use the dynamic resizing property of vector
        // because it may invalidate pointers to elements.
        std::vector<Node<T> > mPool;
        Node<T>* mHead;
    };

    NodePool mPool;
    Mutex& mMutex;
};



/**
 * ProducerConsumerList implementation.
 */

template<class T> class Node
{
public:
    Node() : mNext(NULL),
             mConsumerTailFlag(false),
             mIsCleanedUp(false)
    {
    }

    ~Node()
    {
    }

    void reset()
    {
        mNext = NULL;
        mConsumerTailFlag = false;
        mIsCleanedUp = false;
    }

    // For the consumer's iterator, if there is an end flag on the node, it
    // means advancing the iterator yields end.
    Node *mNext;
    bool mConsumerTailFlag;
    T mData;
    bool mIsCleanedUp;
};


template<class T>
IConsumerListView<T>::iterator::iterator() : mNode(NULL)
{
}

template<class T>
IConsumerListView<T>::iterator::iterator(Node<T>* node) : mNode(node)
{
}

template<class T>
IConsumerListView<T>::iterator::~iterator()
{
}

template<class T>
bool IConsumerListView<T>::iterator::operator==(const iterator&b)
{
    return mNode == b.mNode;
}

template<class T>
bool IConsumerListView<T>::iterator::operator!=(const iterator&b)
{
    return mNode != b.mNode;
}

template<class T>
typename IConsumerListView<T>::iterator& IConsumerListView<T>::iterator::operator=(const iterator& b)
{
    mNode = b.mNode;
    return *this;
}

// prefix operator
template<class T>
typename IConsumerListView<T>::iterator& IConsumerListView<T>::iterator::operator++()
{
    if(mNode)
    {
        if(mNode->mConsumerTailFlag)
        {
            mNode = NULL;
        } else
        {
            mNode = mNode->mNext;
        }
    }
    return *this;
}

// post fix
template<class T>
typename IConsumerListView<T>::iterator IConsumerListView<T>::iterator::operator++(int)
{
    iterator originalPos(*this);

    if(mNode)
    {
        if(mNode->mConsumerTailFlag)
        {
            mNode = NULL;
        } else
        {
            mNode = mNode->mNext;
        }
    }
    return originalPos;
}

template<class T>
T* IConsumerListView<T>::iterator::operator->()
{
    assert(mNode);
    if(mNode == NULL)
    {
        return NULL;
    } else {
        return &(mNode->mData);
    }
}

template<class T>
T& IConsumerListView<T>::iterator::operator*()
{
    // If we're at the end node, it should segfault.
    assert(mNode);
    return mNode->mData;
}

template<class T>
Node<T>* IConsumerListView<T>::iterator::getNode()
{
    return mNode;
}

template<class T>
IConsumerListView<T>::IConsumerListView(IListElementCleanup<T> *cleanup, Mutex& mutex)
    : mConsumerHead(NULL),
      mConsumerTail(NULL),
      mBeginIt(NULL),
      mCleanupObj(cleanup),
      mMutex(mutex)
{
}

template<class T>
IConsumerListView<T>::~IConsumerListView()
{
}

template<class T>
typename IConsumerListView<T>::iterator IConsumerListView<T>::begin()
{
    return mBeginIt;
}

template<class T>
typename IConsumerListView<T>::iterator IConsumerListView<T>::end()
{
    return iterator(NULL);
}


template<class T>
void IConsumerListView<T>::pop_front()
{
#ifdef USE_MUTEXES_PCLIST
    ScopedMutex scopedMutex(mMutex);
#endif
    if(mConsumerHead)
    {
        // We can't let consumer head be null because then during cleanup
        // the producer implementation won't know whether we haven't started
        // reading yet or we've read them all. Therefore we alway leave head
        // at the tail position and set our begin iterator to end when we're
        // done with the last node in the consumer view of the list. When
        // the implementation refreshes the ListConsumerView, it will take
        // the begin iteraor not pointing to the head consumer node to mean
        // that we emptied the list before the refresh and we can advance
        // the consumer head by one.
        if(mConsumerHead == mConsumerTail)
        {
            if(mBeginIt.getNode() == mConsumerHead)
            {
                if(mCleanupObj && mConsumerHead->mIsCleanedUp == false)
                {
                    mCleanupObj->cleanup(mConsumerHead->mData);
                    mConsumerHead->mIsCleanedUp = true;
                }
                ++mBeginIt;
            } else {
                // Nothing to pop.
                assert(mBeginIt.getNode() == NULL);
            }
        } else
        {
            assert(mBeginIt.getNode());
            if(mCleanupObj && mConsumerHead->mIsCleanedUp == false)
            {
                mCleanupObj->cleanup(mConsumerHead->mData);
                mConsumerHead->mIsCleanedUp = true;
            }
            ++mBeginIt;
            mConsumerHead = mConsumerHead->mNext;
        }
    }
}

template<class T>
IProducerListView<T>::NodePool::NodePool(uint32_t poolSize) : mPool(poolSize)
{
    for(uint32_t i = 0; i < poolSize-1; i++)
    {
        mPool[i].mNext = &(mPool[i+1]);
    }
    mHead = &(mPool[0]);
}

template<class T>
IProducerListView<T>::NodePool::~NodePool()
{
}

template<class T>
Node<T>* IProducerListView<T>::NodePool::getNode()
{
    Node<T>* node = mHead;
    if(mHead)
    {
        mHead = mHead->mNext;
        node->reset();
    }
    return node;
}

template<class T>
void IProducerListView<T>::NodePool::recycleNode(Node<T> *node)
{
    if(mHead == NULL)
    {
        mHead = node;
    } else
    {
        node->mNext = mHead;
        mHead = node;
    }
}

template<class T>
IProducerListView<T>::IProducerListView(int poolSize, Mutex& mutex)
    : mHead(NULL),
      mTail(NULL),
      mPool(poolSize),
      mMutex(mutex)
{
}

template<class T>
IProducerListView<T>::~IProducerListView()
{
}

// Returns true on success, false if the nodePool is out of elements.
template<class T>
bool IProducerListView<T>::push_back(T x)
{
    popUnneededFromFront();

    Node<T> *node = NULL;
    if((node = mPool.getNode()) == NULL)
    {
        return false;
    } else {
#ifdef USE_MUTEXES_PCLIST
        ScopedMutex scopedMutex(mMutex);
#endif

        node->mNext = NULL;
        node->mData = x;
        if(mTail == NULL)
        {
            assert(mHead == NULL);
            mTail = node;
            mHead = node;
        }
        else
        {
            mTail->mNext = node;
            mTail = node;
        }
        return true;
    }
}

template<class T>
ProducerConsumerList<T>::ProducerConsumerList(int poolSize,
                                              IListElementCleanup<T>* cleanupObj)
    : IConsumerListView<T>(cleanupObj, mMutex),
      IProducerListView<T>(poolSize, mMutex),
      mMutex(PRODUCERCONSUMERLIST_MUTEX, "ProducerConsumerListMutex")
{
}

template<class T>
ProducerConsumerList<T>::~ProducerConsumerList()
{
}

template<class T>
bool ProducerConsumerList<T>::updateListSnapshot()
{
    bool invalidate_iterators = false;
#ifdef USE_MUTEXES_PCLIST
    ScopedMutex scopedMutex(mMutex);
#endif
    if(IConsumerListView<T>::mConsumerTail)
    {
        IConsumerListView<T>::mConsumerTail->mConsumerTailFlag = false;
    }
    IConsumerListView<T>::mConsumerTail = IProducerListView<T>::mTail;

    if(IConsumerListView<T>::mConsumerTail)
    {
        IConsumerListView<T>::mConsumerTail->mConsumerTailFlag = true;
    }

    if(IConsumerListView<T>::mConsumerHead)
    {
        if(IConsumerListView<T>::mConsumerHead !=
           IConsumerListView<T>::mBeginIt.getNode())
        {
            // The consumer is done with this head node. We can't let consumer
            // head be null because then during cleanup the producer
            // implementation won't know whether we haven't started reading yet
            // or we've read them all. Therefore we alway leave head at the tail
            // position and set our begin iterator to end (NULL) when we're done
            // with the last node in the consumer view of the list. When the
            // implementation refreshes the ListConsumerView, it takes the begin
            // iteraor not pointing to the head consumer node to mean that we
            // emptied the list before the refresh and we can advance the
            // consumer head by one if the head is not longer equal to the new
            // tail.
            //
            if(IConsumerListView<T>::mConsumerHead != IConsumerListView<T>::mConsumerTail)
            {
                IConsumerListView<T>::mConsumerHead =
                    IConsumerListView<T>::mConsumerHead->mNext;
                typename IConsumerListView<T>::iterator it(IConsumerListView<T>::mConsumerHead);
                IConsumerListView<T>::mBeginIt = it;
            }
        }
    } else
    {
        IConsumerListView<T>::mConsumerHead = IProducerListView<T>::mHead;
        typename IConsumerListView<T>::iterator it(IConsumerListView<T>::mConsumerHead);
        IConsumerListView<T>::mBeginIt = it;
        invalidate_iterators = true;
    }

    return invalidate_iterators;
}

template<class T>
void ProducerConsumerList<T>::popUnneededFromFront()
{

    Node<T>* node = NULL;
    Node<T>* consumerHeadSnapshot;
    {

#ifdef USE_MUTEXES_PCLIST
        ScopedMutex scopedMutex(mMutex);
#endif
        consumerHeadSnapshot = IConsumerListView<T>::mConsumerHead;
    }

    if (consumerHeadSnapshot && IProducerListView<T>::mHead)
    {
        // If mHead != consumerHeadSnapshot, we know that head is before the consumer's
        // current read position in the list.
        while(IProducerListView<T>::mHead != consumerHeadSnapshot)
        {
            node = IProducerListView<T>::mHead;
            IProducerListView<T>::mHead = IProducerListView<T>::mHead->mNext;
            node->mNext = NULL;
            if(IProducerListView<T>::mHead == NULL) {
                IProducerListView<T>::mTail = NULL;
            }

            IProducerListView<T>::mPool.recycleNode(node);
        }
    }
}

template<class T>
void ProducerConsumerList<T>::clear()
{
#ifdef USE_MUTEXES_PCLIST
    ScopedMutex scopedMutex(mMutex);
#endif

    // This should only happen when consumer thread is stopped.
    IConsumerListView<T>::mConsumerHead = NULL;
    IConsumerListView<T>::mConsumerTail = NULL;

    while(IProducerListView<T>::mHead)
    {
        Node<T>* node =IProducerListView<T>::mHead;
        IProducerListView<T>::mHead = IProducerListView<T>::mHead->mNext;
        node->mNext = NULL;
        if(IConsumerListView<T>::mCleanupObj )
        {
            if(node->mIsCleanedUp == false)
            {
                IConsumerListView<T>::mCleanupObj->cleanup(node->mData);
                node->mIsCleanedUp = true;
            }
        }
        IProducerListView<T>::mPool.recycleNode(node);
    }
    IProducerListView<T>::mTail = NULL;
}

} // namespace netflix


#endif
