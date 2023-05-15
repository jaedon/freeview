/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef ThreadPool_h
#define ThreadPool_h

#include <set>
#include <nrdbase/Noncopyable.h>
#include <nrdbase/Thread.h>
#include <nrdbase/AsyncQueue.h>
#include <nrdbase/Stopwatch.h>
#include <nrdbase/tr1.h>

class ThreadPoolThread;

NRDP_DECLARE_OBJECTCOUNT(ThreadPoolJob);

namespace netflix {

class ThreadPoolJob : public Noncopyable
{
public:
    typedef shared_ptr<ThreadPoolJob> SharedPtr;

    ThreadPoolJob() { NRDP_OBJECTCOUNT_REF(ThreadPoolJob, this); }
    virtual ~ThreadPoolJob() { NRDP_OBJECTCOUNT_DEREF(ThreadPoolJob, this); }
    virtual void execute() = 0;
    virtual Thread::Priority priority() const { return 0; }
    virtual std::string description() const { return std::string(); }
    Stopwatch mStopwatch;
};

class ThreadPool : public enable_shared_from_this<ThreadPool>
{
public:
    typedef shared_ptr<ThreadPool> SharedPtr;
    ThreadPool();
    ~ThreadPool();
    void init(int threadCount, ThreadConfig *config, int minThreadCount=-1, const Time & idleTime = Time(0), int maxBacklogSize=500);
    void shutdown();
    bool post(const ThreadPoolJob::SharedPtr &job);
    inline void post(ThreadPoolJob *job) { post(ThreadPoolJob::SharedPtr(job)); }
    typedef AsyncQueue<ThreadPoolJob> Queue;
    bool idleThreadShouldExit( Thread * thread );
private:
    void startThread();
    Queue::SharedPtr mQueue;

    typedef std::set<Thread*> ThreadSet;

    // This mutex should only be used to protect the members below
    Mutex           mMutex;
    ThreadSet       mThreads;
    int             mMaxBacklogSize;
    int             mMaxThreadCount;
    int             mNextThreadNumber;
    ThreadConfig *  mThreadConfig;
    Time            mIdleTime;
    int             mMinThreadCount;
    ThreadSet       mDeadThreads;
};
}

#endif
