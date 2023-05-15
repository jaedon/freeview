/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "ThreadPool.h"

#include <assert.h>
#include <sstream>
#include <string>

#include <nrdbase/ConditionVariable.h>
#include <nrdbase/PerformanceMarker.h>
#include <nrdbase/Log.h>
#include <nrdbase/Mutex.h>
#include <nrdbase/PerformanceMarker.h>
#include <nrdbase/ScopedMutex.h>
#include <nrdbase/Thread.h>

using namespace netflix;

NRDP_DEFINE_OBJECTCOUNT(ThreadPoolJob);

class ThreadPoolThread : public Thread
{
public:

    ThreadPoolThread(ThreadPool::Queue::SharedPtr queue, ThreadConfig *config ,
                     const std::string & name , shared_ptr<ThreadPool> threadPool , const Time &idleTime)
        :
        Thread(config, name), mQueue(queue), mThreadPool(threadPool), mIdleTime(idleTime), mMutex(ZERO_MUTEX, "ThreadPoolThreadMutex")
    {}

protected:

    std::string GetTask() const
    {
        shared_ptr<ThreadPoolJob> job;
        {
            ScopedMutex _lock(mMutex);
            job = mJob;
        }
        if(job)
            return job->description();
        return std::string();
    }

    void Run()
    {
        while(mQueue->is_open()) {
            PERFORMANCE_MARKER_SCOPED("threadpool.loop");

            if(shared_ptr<ThreadPoolJob> job = mQueue->pop(mIdleTime)) {
                PERFORMANCE_MARKER_SCOPED(("threadpool.execute " + job->description()).c_str(), false);
                const Thread::Priority oldPriority = CurrentThread()->GetPriority();
                const Thread::Priority priority = job->priority();
                if(priority)
                    CurrentThread()->SetPriority(priority);
#ifdef BUILD_DEBUG
                Stopwatch::ms timeInQueue = job->mStopwatch.restart();
                (void)timeInQueue;
                NTRACES(TRACE_THREADPOOL, "Executing job in thread " << GetName() << "@" << int(priority) << "/" << int(oldPriority) << ". " << job->description() );
#endif
                {
                    ScopedMutex _lock(mMutex);
                    mJob = job;
                }
                job->execute();
                {
                    ScopedMutex _lock(mMutex);
                    mJob.reset();
                }
#ifdef BUILD_DEBUG
                Stopwatch::ms runTime = job->mStopwatch.elapsed();
                (void)runTime;
                NTRACES(TRACE_THREADPOOL,
                        "Job finished in thread "
                        << GetName() << ". "
                        << job->description()
                        << ". Waited " << timeInQueue << " ms, ran in " << runTime << " ms" );
#endif
                if(priority)
                    CurrentThread()->SetPriority(oldPriority);
            } else if(!mQueue->is_closed()) {
                if(ThreadPool::SharedPtr pool = mThreadPool.lock()) {
                    if(pool->idleThreadShouldExit(this))
                        break;
                }
            }
        }
    }

private:

    ThreadPool::Queue::SharedPtr    mQueue;
    weak_ptr< ThreadPool>           mThreadPool;
    Time                            mIdleTime;
    shared_ptr<ThreadPoolJob>       mJob;
    mutable Mutex                   mMutex;
};

ThreadPool::ThreadPool() : mMutex(THREADPOOL_MUTEX, "ThreadPool"), mMaxBacklogSize(-1), mMaxThreadCount(-1),
                           mNextThreadNumber(1), mThreadConfig(0), mMinThreadCount(-1)
{
}

ThreadPool::~ThreadPool()
{
   // This ensures that someone has called shutdown and
   // destroyed the queue before they destroy the thread pool
   assert( ! mQueue );
}

void ThreadPool::init(int threadCount, ThreadConfig *config,int minThreadCount,const Time & idleTime, int maxBacklog)
{
    assert(minThreadCount <= threadCount);
    ScopedMutex lock(mMutex);

    assert(!mQueue);
    mQueue.reset(new Queue());
    NTRACES( TRACE_THREADPOOL , "Initializing ThreadPool " << this << maxBacklog
            << " with " << threadCount << " (" << ( ( minThreadCount >= 0 ) ? minThreadCount : threadCount ) << ")"
            << " threads (" << config->GetName() << ")" );

    mMaxBacklogSize = maxBacklog;
    mMaxThreadCount = threadCount;
    mMinThreadCount = minThreadCount < 0 ? threadCount : minThreadCount;
    assert( mMinThreadCount == mMaxThreadCount || idleTime != 0 );
    mIdleTime = mMinThreadCount == mMaxThreadCount ? Time(0) : idleTime;
    mThreadConfig = config;
    for (int i=0; i< mMinThreadCount; ++i)
        startThread();
}

void ThreadPool::shutdown()
{
    ScopedMutex lock( mMutex );

    if(!mQueue || mQueue->is_closed())
        return;
    NTRACES(TRACE_THREADPOOL, "Shutting down thread pool");

    // Closes the queue so that no pops or pushes will succeed and waits
    // until all waiting threads are done waiting for the queue.
    NTRACES(TRACE_THREADPOOL , "Closing queue" );
    mQueue->close(true);
    NTRACES(TRACE_THREADPOOL , "Closed queue" );

    // Wait for each thread and delete it. All threads should have stopped waiting
    // on the queue when "close" was called and are now either done or on their way
    // out.
    for(ThreadSet::iterator it = mThreads.begin(); it != mThreads.end(); ++it) {
        (*it)->Wait();
        delete *it;
    }
    for(ThreadSet::iterator it = mDeadThreads.begin(); it != mDeadThreads.end(); ++it) {
        (*it)->Wait();
        delete *it;
    }
    mThreads.clear();
    mDeadThreads.clear();

    // Removes any existing jobs from the queue
    if(std::size_t count = mQueue->clear()) {
        NRDP_UNUSED(count); // suppress unused variable warning
        NTRACES(TRACE_THREADPOOL, "Deleted " << count << " jobs from backlog");
    }
    mQueue.reset();
}


// Lock should be held

void ThreadPool::startThread()
{
    std::ostringstream s;
    s << mNextThreadNumber++;
    std::string name = s.str();
    Thread * thread = new ThreadPoolThread( mQueue, mThreadConfig, name , shared_from_this() , mIdleTime );
    thread->Start();
    mThreads.insert( thread );
    assert(mThreads.size() <= static_cast<size_t>(mMaxThreadCount));
}

bool ThreadPool::post(const ThreadPoolJob::SharedPtr &job)
{
    PERFORMANCE_MARKER_SCOPED("threadpool.post");
    assert( job );
#ifdef NRDP_HAS_TRACING
    const std::string desc = job->description();
#endif

    ScopedMutex lock(mMutex);
    if(!mQueue || mQueue->is_closed())
        return false;

    std::size_t count = mQueue->size();
    if (count >= (size_t)mMaxBacklogSize) {
        lock.unlock();
        Log::error(TRACE_THREADPOOL) << "Too many outstanding jobs (" << count << "). Ignoring job. " << job->description();
        return false;
    }

    NTRACE(TRACE_THREADPOOL, "Pushing job to backlog (%zu). %s", count, desc.c_str());
    job->mStopwatch.restart();
    count = mQueue->push(job);

    // Take this opportunity to get rid of dead threads. These should have exited
    // so the wait won't take long. Meanwhile, the job is in the queue and it can
    // be started.
    if(!mDeadThreads.empty()) {
        NTRACE(TRACE_THREADPOOL, "Waiting for %zu dead thread(s).", mDeadThreads.size());

        for( ThreadSet::iterator it = mDeadThreads.begin(); it != mDeadThreads.end(); ++it ) {
            (*it)->Wait();
            delete *it;
        }
        mDeadThreads.clear();
    }

    // We also check to see if we need to spin up more threads
    // If mMinThreadCount >= mMaxThreadCount, it means we are in dynamic mode
    // If the number of threads is less than the max and
    // the number of jobs in the queue exceeds the current number of threads.

    if((mMinThreadCount < mMaxThreadCount) && (static_cast<int>(mThreads.size()) < mMaxThreadCount) && (count > mThreads.size())) {
        NTRACE(TRACE_THREADPOOL, "Starting a new thread because we have %zu threads and %zu jobs.", mThreads.size(), count);
        startThread();
    }
    return true;
}

// This gets called by the thread when it has been idle for the given time

bool ThreadPool::idleThreadShouldExit(Thread * thread)
{
    // Because another thread could have the mutex locked waiting for this
    // one to die, we try lock. The down side is that if that is not the case
    // and this thread cannot get the mutex right away, it will sit around
    // waiting on the queue for idle time.

    bool result = false;
    if(mMutex.tryLock() == NFErr_OK) {
        if(mMinThreadCount < mMaxThreadCount && (static_cast<int>(mThreads.size()) > mMinThreadCount)) {
            mThreads.erase(thread);
            // We cannot join this thread here, because it is the one that
            // called us, so we set it aside in another set.
            mDeadThreads.insert(thread);
            NTRACE(TRACE_THREADPOOL, "Terminating idle thread %s. We now have %zu threads left.", thread->GetName().c_str(), mThreads.size());
            result = true;
        }
        mMutex.unlock();
    }
    return result;
}

