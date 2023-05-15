/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "Thread.h"

#include <errno.h>
#include <limits.h>
#include <openssl/err.h>
#include <pthread.h>
#include <sched.h>
#include <signal.h>
#include <unistd.h>

#if defined(HAVE_PRCTL_H)
# include <sys/prctl.h>
#endif

#include <nrdbase/Assert.h>
#include <nrdbase/config.h>
#include <nrdbase/Log.h>
#include <nrdbase/MutexStack.h>
#include <nrdbase/MutexRanks.h>
#include <nrdbase/ConditionVariable.h>
#include <nrdbase/ScopedMutex.h>
#include <nrdbase/Semaphore.h>
#include <nrdbase/ThreadLocalStore.h>

#ifdef __APPLE__
# include <mach/mach_init.h>
# include <mach/thread_act.h>
# include <mach/mach_port.h>
#elif defined(NRDP_HAS_GETTID)
# include <unistd.h>
# include <sys/syscall.h>
#endif

#if defined(__ANDROID__)
# include <asm/page.h>
#endif

using namespace netflix;

Thread::Affinity Thread::sGlobalAffinity;
Thread::GlobalSpawnFunc Thread::sGlobalSpawnFunc = 0;
const size_t Thread::defaultStackSize = 65536;
const uint8_t Thread::maxPriority = 99;
const uint8_t Thread::minPriority = 1;

namespace netflix {

DEFINE_THREAD(TELNET_SERVER, 0);
DEFINE_THREAD(TELNET_CONNECTION, 0);
DEFINE_THREAD(HTTPREQUEST_MANAGER, 0);
DEFINE_THREAD(NBP_POOL, 0, 98304);
DEFINE_THREAD(DNS_MANAGER, 0);
DEFINE_THREAD(MC_EVENT, 0, 32768);
DEFINE_THREAD(MC_PUMPING, 0, 131072);
DEFINE_THREAD(MC_PUMPING2, 0, 131072);
DEFINE_THREAD(DISK_CACHE, 1);
DEFINE_THREAD(RESOURCE_HTTP, 0);
DEFINE_THREAD(WEBSOCKET, 0);

}

#if _POSIX_VERSION < 199506L
# error Need POSIX version >= 199506
#endif
//#ifndef _POSIX_THREAD_PRIORITY_SCHEDULING
//# error This Posix library does not support PTHREAD_PRIORITY_SCHEDULING.
//#endif

/*
 * An object containing the thread name that we attach to its TLS. This
 * allows us to get the current thread's name when don't have access to
 * the Thread instance.
 */
class ThreadPointer
{
public:
    static void set( Thread *thread )
    {
        return sThreads.set(thread);
    }

    static Thread *get()
    {
        return sThreads.get();
    }

private:
    ThreadPointer() {}
    static ThreadLocalStore<Thread> sThreads;
};

ThreadLocalStore<Thread> ThreadPointer::sThreads(ThreadLocalStore<Thread>::DontDelete);

/**
 * @class Thread::Impl Thread.cpp
 * @brief Thread pthread implementation.
 */
class Thread::Impl : private Noncopyable
{
public:
    /** Constructor. */
    Impl();

    void setState(Thread::State state);
    bool waitForState(Thread::State state, const Time &time);

    /**
     * Set the stack size in bytes.
     *
     * @param words stack size in bytes.
     */
    void setStackSize(size_t bytes)
    {
        stackSize = std::max<size_t>(bytes, PTHREAD_STACK_MIN*4);
    }

    /**
     * Return the stack size.
     *
     * @return the stack size in bytes.
     */
    size_t getStackSize() const { return stackSize; }

    /**
     * Joins this thread.
     *
     * @return zero if successful, otherwise the return value of
     *         pthread_join.
     */
    int joinThread();

    /**
     * Returns the thread's return value. The return value is
     * undefined until the thread has been joined.
     *
     * @return the thread's return value.
     */
    void* getReturnValue() const { return returnValue; }

    /**
     * Sets the priority of a thread.
     *
     * @param[in] thread the thread.
     * @param[in] priority the new priority.
     * @return NFErr_OK if successful;
     *         NFErr_Bad if the priority could not be changed.
     */
    NFErr setThreadPriority(int priority);

    /**
     * Sets the affinity of a thread.
     *
     * @param[in] thread the thread.
     * @param[in] affinity the new affinity.
     * @return NFErr_OK if successful;
     *         NFErr_Bad if the priority could not be changed.
     */
    NFErr setThreadAffinity(Thread::Affinity affinity);

    /**
     * Converts a kernel priority to a Netflix priority.
     *
     * @param[in] the kernel priority.
     * @return the Netflix priority.
     */
    static Priority LinuxPriorityToNetflixPriority(int priority);

    /**
     * Converts a Netflix priority to a kernel priority.
     *
     * @param[in] the Netflix priority.
     * @return the kernel priority.
     */
    static int NetflixPriorityToLinuxPriority(Priority priority);

    Mutex stateMutex;      //!< pthread state mutex.
    ConditionVariable stateCVar;        //!< pthred state condition variable.

    uint32_t threadId;
    pthread_t pThread;   //!< The pthread.
    State state;        //!< Thread state.
    bool deleteSelf;    //!< True if this thread should delete itself.
    Priority pendingPriority;  //!< Thread priority. in Netflix range
    Thread::Affinity pendingAffinity;  //!< Thread affinity

    static int minPriority; //!< Kernel minimum priority.
    static int maxPriority; //!< Kernel maximum priority.

private:
    size_t stackSize;  //!< Thread stack size in bytes.
    void* returnValue; //!< Thread return value.
};

int Thread::Impl::minPriority = sched_get_priority_min(NRDP_SCHEDULER_TYPE);
int Thread::Impl::maxPriority = sched_get_priority_max(NRDP_SCHEDULER_TYPE);

Thread::Impl::Impl() : stateMutex(THREADSTATE_MUTEX, "ThreadState")
{
    deleteSelf = false;
    state = NEW;
    stackSize = defaultStackSize;
    pendingPriority = 0;
#ifdef NRDP_HAS_PTHREAD_SET_AFFINITY
    CPU_ZERO(&pendingAffinity);
#else
    pendingAffinity = 0;
#endif
}

void Thread::Impl::setState(Thread::State s)
{
    state = s;
    stateCVar.broadcast();
}

bool Thread::Impl::waitForState(Thread::State s, const Time &time)
{
    if(pthread_equal(pThread,  pthread_self()) ) {
        Log::fatal(TRACE_THREAD, "Wait() on self... returning immediately");
        assert(false);
        return false;
    }
    while(state != s) {
        if(s == JOINED && state == TERMINATED) {
            // Now we know we've gone through the localterminated
            // code. Wait until the thread has completely finished with
            // all its resources (including the stack).
            joinThread();
            setState(JOINED);
            break;
        } else if(s == RUNNING && (state == TERMINATED || state == JOINED)) {
            break;
        }
        if(stateCVar.wait(&stateMutex, time) == NFErr_TimedOut)
            break;
    }
    return state == s;
}

int Thread::Impl::joinThread()
{
    int err = 0;
    if (state != NEW && state != JOINED)
        err = pthread_join(pThread, &returnValue);
    return err;
}

NFErr Thread::Impl::setThreadPriority(int linuxPriority)
{
    struct sched_param schedParam;
    schedParam.sched_priority = linuxPriority;
    return !pthread_setschedparam(pThread, NRDP_SCHEDULER_TYPE, &schedParam) ? NFErr_OK : NFErr_Bad;
}

NFErr Thread::Impl::setThreadAffinity(Thread::Affinity affinity)
{
#ifdef NRDP_HAS_PTHREAD_SET_AFFINITY
    if (CPU_COUNT(&affinity) > 0) {
        if(pthread_setaffinity_np(pThread, sizeof(affinity), &affinity)) {
            Log::error(TRACE_LOG, "[%s:%d]: ", __func__, __LINE__);
            return NFErr_Bad;
        }
    }
#else
    (void)affinity;
#endif
    return NFErr_OK;
}

Thread::Priority Thread::Impl::LinuxPriorityToNetflixPriority(int priority)
{
    float linuxRange = maxPriority - minPriority;
    float nfRange = Thread::maxPriority - Thread::minPriority;

    int nfPrio = Thread::minPriority + static_cast<int> (((priority - minPriority) * nfRange) / linuxRange);
    // Clamp the priority in case of integer rounding errors.
    nfPrio = std::max(nfPrio, (int)Thread::minPriority);
    nfPrio = std::min(nfPrio, (int)Thread::maxPriority);
    //printf("Converted Linux prio %d to Nf prio %d\n", priority, nfPrio);
    return nfPrio;
}

int Thread::Impl::NetflixPriorityToLinuxPriority(Priority priority)
{
    float linuxRange = maxPriority - minPriority;
    float nfRange = Thread::maxPriority - Thread::minPriority;

    int lPrio = minPriority + static_cast<int> (((priority - Thread::minPriority) * linuxRange) / nfRange);
    // Clamp the priority in case of integer rounding errors.
    if (minPriority < maxPriority) {
        lPrio = std::max<int>(lPrio, minPriority);
        lPrio = std::min<int>(lPrio, maxPriority);
    } else {
        lPrio = std::min<int>(lPrio, minPriority);
        lPrio = std::max<int>(lPrio, maxPriority);
    }
    //printf("Converted NF prio %d to Linux prio %d\n", priority, lPrio);
    return lPrio;
}

Thread::Thread(ThreadConfig *_config, const std::string &_name)
  : impl(new Impl())
  , config(_config)
  , name(_name)
  , threadId(0)
  , threadPtr(0)
{
    NTRACE(TRACE_THREAD, "Creating thread %p(%s), address %p",
           config, GetName().c_str(), static_cast<void*>(this));
}

Thread::~Thread()
{
    NTRACE(TRACE_THREAD, "Killing thread %p(%s)", config, GetName().c_str());
    bool wait = true;
    if (impl->deleteSelf) {
        ScopedMutex lock(impl->stateMutex);
        if ((impl->state != NEW) && (impl->state != JOINED) &&
            pthread_equal(impl->pThread, pthread_self()))
        {
            wait = false;
        }
    }
    // Wait for the thread to terminate. No timeout.
    if (wait)
        Wait();
}

Thread::Priority Thread::GetPriority() const
{
    ScopedMutex lock(impl->stateMutex);
    return impl->pendingPriority;
}

Thread::Affinity Thread::GetAffinity() const
{
    ScopedMutex lock(impl->stateMutex);
    return impl->pendingAffinity;
}

void Thread::SetState(State state) const
{
    ScopedMutex lock(impl->stateMutex);
    impl->setState(state);
}

Thread::State Thread::GetState() const
{
    ScopedMutex lock(impl->stateMutex);
    return impl->state;
}

void Thread::SetAffinity(Thread::Affinity affinity)
{
    ScopedMutex lock(impl->stateMutex);
    bool success = true;
    if ((impl->state == STARTING || impl->state == RUNNING)) {
        success = (impl->setThreadAffinity(affinity) == NFErr_OK);
    }
    if(success)
        impl->pendingAffinity = affinity;
}

void Thread::SetPriority(Priority priority)
{
    ScopedMutex lock(impl->stateMutex);
    bool success = true;
    if ((impl->state == STARTING || impl->state == RUNNING) && priority) {
        const int lPrio = Impl::NetflixPriorityToLinuxPriority(priority);
        success = (impl->setThreadPriority(lPrio) == NFErr_OK);
    }
    if(success)
        impl->pendingPriority = priority;
}

void Thread::SetDeleteSelf(bool flag)
{
    impl->deleteSelf = flag;
}

void* Thread::Spawn_internal(void* p)
{
    // Don't start the child thread until pthread_create() returns in
    // the parent thread.
    Thread* thread = static_cast<Thread*>(p);
    if(thread->config)
        thread->config->registerThread(thread);
#ifdef NRDP_PTHREAD_SELF_ID
    thread->threadPtr = reinterpret_cast<void*>(NRDP_PTHREAD_SELF_ID);
#else
    thread->threadPtr = reinterpret_cast<void*>(pthread_self());
#endif
#if defined(__APPLE__)
    thread->threadId = mach_thread_self();
#elif defined(NRDP_HAS_GETTID)
    thread->threadId = syscall(SYS_gettid);
#else
    static int sThreadId = 0;
    thread->threadId = ++sThreadId;
#endif

    Impl* internal = thread->impl.get();
    internal->threadId = thread->threadId;
    thread->SetState(RUNNING);
    if (internal->pendingPriority)
        internal->setThreadPriority(Impl::NetflixPriorityToLinuxPriority(internal->pendingPriority));
    int policy;
    struct sched_param schedParam;
    pthread_getschedparam(pthread_self(), &policy, &schedParam);
    internal->pendingPriority = Impl::LinuxPriorityToNetflixPriority(schedParam.sched_priority);

#ifdef NRDP_HAS_PTHREAD_SET_AFFINITY
    Affinity affinity = internal->pendingAffinity;
    CPU_OR(&affinity, &affinity, &sGlobalAffinity);
    internal->setThreadAffinity(affinity);
    pthread_getaffinity_np(pthread_self(), sizeof(sizeof(internal->pendingAffinity)), &internal->pendingAffinity);
#endif

    std::string threadName = thread->GetName();
#if defined(HAVE_PRCTL_H)
    prctl(PR_SET_NAME, threadName.c_str(), 0, 0, 0);
#elif defined(NRDP_HAS_PTHREAD_SETNAME_1)
    pthread_setname_np(threadName.c_str());
#elif defined(NRDP_HAS_PTHREAD_SETNAME_2)
    pthread_setname_np(pthread_self(), threadName.c_str());
#elif !defined(HAS_PTHREAD_ATTR_SETNAME_NP)
# warning "Thread naming is not supported"
#endif

    // Mask out all the signals - we don't want any by default.
    sigset_t maskedSignals;
    sigemptyset(&maskedSignals);
    sigaddset(&maskedSignals, SIGTERM);
    int m = pthread_sigmask(SIG_BLOCK, &maskedSignals, NULL);
    NRDP_UNUSED(m);  // prevent warning in prod mode
    NTRACE(TRACE_THREAD, "Starting thread %s, id = %p(%d), priority = %d, sigprocmask = %d, stack size = %zu",
           threadName.c_str(), CurrentThreadPtr(), CurrentThreadId(), Impl::LinuxPriorityToNetflixPriority(schedParam.sched_priority), m, internal->getStackSize());
    ThreadPointer::set( thread );
#ifdef __OBJC__
    @autoreleasepool {
#endif
    thread->Run();
#ifdef __OBJC__
    }
#endif
    NTRACE(TRACE_THREAD, "Thread %s Run() finished", threadName.c_str());

    if(thread->config)
        thread->config->unregisterThread(thread);
    thread->SetState(TERMINATED);

    ERR_remove_state(0); // for openssl memory leaks; man 3 ERR_remove_state

    thread->threadId = 0;
    thread->threadPtr = NULL;
    if (internal->deleteSelf)
        delete thread;
    assert(!MutexStack::size());
    return 0;
}

#define PTHREAD_CHECK(val)                                              \
    do {                                                                \
        if (val != 0)                                                   \
        {                                                               \
            Log::error(TRACE_THREAD,                                    \
                       "Thread::Start() %d: err = 0x%08x, errno = 0x%08x", \
                       __LINE__, val, errno);                           \
            ASSERT(false, "Thread failure");                            \
        }                                                               \
    } while(0)

void Thread::Start()
{
    int ret;
    pthread_attr_t attr;
    ret = pthread_attr_init(&attr);
    PTHREAD_CHECK(ret);
    {
        ScopedMutex lock(impl->stateMutex);
        size_t stackSize = config ? config->GetStackSize() : 0;
        if(!stackSize)
            stackSize = defaultStackSize;

        // set a minimum stack size if set
#ifdef NRDP_MIN_STACK_SIZE
        if (stackSize < NRDP_MIN_STACK_SIZE)
        {
            Log::info(TRACE_THREAD,
                      "Bumping requested stack size %d to minimum %d",
                      stackSize, NRDP_MIN_STACK_SIZE);
            stackSize = NRDP_MIN_STACK_SIZE;
        }
#endif

        impl->setStackSize(stackSize);

        if (impl->state == STARTING || impl->state == RUNNING) {
            // The thread is already running,
            // Assert: should not call this method twice
            return;
        }
        SetState(STARTING);


        if (impl->deleteSelf) {
            ret = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
            PTHREAD_CHECK(ret);
        }

        ret = pthread_attr_setstacksize(&attr, impl->getStackSize());
        PTHREAD_CHECK(ret);

        NTRACE(TRACE_THREAD, "Thread %p(%s): size: %zu",
               config, GetName().c_str(), stackSize );

#if HAS_PTHREAD_ATTR_SETNAME_NP
        pthread_attr_setname_np(&attr, GetName().c_str());
#endif

        ret = pthread_attr_setschedpolicy(&attr, NRDP_SCHEDULER_TYPE);
        PTHREAD_CHECK(ret);
        if (!impl->pendingPriority) {
            Thread::Priority priority = config ? config->GetPriority() : 0;
            if(!priority) {
                int policy;
                struct sched_param schedParam;
                ret = pthread_getschedparam(pthread_self(), &policy, &schedParam);
                PTHREAD_CHECK(ret);
                if (!ret)
                    priority = Impl::LinuxPriorityToNetflixPriority(schedParam.sched_priority);
            }
            impl->pendingPriority = priority;
        }
    }

#ifdef HAS_PTHREAD_ATTR_SETAFFINITY_NP
    Affinity requestedAffinity = config ? config->GetAffinity() : 0;
    if(requestedAffinity != 0) {
        cpu_set_t cpuset = requestedAffinity;
        pthread_attr_setaffinity_np(&attr, sizeof(cpuset), &cpuset);
    }
#endif

    if(sGlobalSpawnFunc)
        ret = (*sGlobalSpawnFunc)(this, &attr);
    else
        ret = this->Spawn(&attr);
    PTHREAD_CHECK(ret);
    if(ret)
        SetState(JOINED); // failed to spawn
    pthread_attr_destroy(&attr);
}

int Thread::Spawn(void *attr)
{
    return pthread_create(&impl->pThread, (pthread_attr_t*)attr, Spawn_internal, this);
}

NFErr Thread::WaitForState(State state, const Time &time)
{
    ScopedMutex lock(impl->stateMutex);
    return impl->waitForState(state, time) ? NFErr_OK : NFErr_TimedOut;
}

NFErr Thread::Wait(const Time &time)
{
    NTRACE(TRACE_THREAD, "Waiting for thread %p(%s)", config, GetName().c_str());
    ScopedMutex lock(impl->stateMutex);
    if (impl->state == JOINED || impl->state == NEW)
        return NFErr_OK; // already joined
    return impl->waitForState(JOINED, time) ? NFErr_OK : NFErr_TimedOut;
}

#ifdef NRDP_THREAD_TRACK_LOCK
std::vector<Thread::LockInfo> Thread::GetLocks()
{
    std::vector<Thread::LockInfo> result;
    for(std::map<const char *, Thread::LockInfo>::const_iterator it = locks.begin(); it != locks.end(); ++it) {
        LockInfo info = (*it).second;
        info.name = (*it).first;
        result.push_back(info);
    }
    return result;
}

void Thread::Locked(const char *name)
{
    LockInfo &lock = locks[name];
    lock.locked = true;
    if(lock.lockTime.isPaused()) {
        lock.lockTime.resume();
    } else {
        ++lock.locks;
        if(!lock.recurse++)
            lock.lockTime.start();
    }
}

void Thread::Unlocked(const char *name, bool force)
{
    LockInfo &lock = locks[name];
    if(force) {
        lock.locked = false;
        lock.lockTime.pause();
    } else if(!--lock.recurse) {
        lock.locked = false;
        lock.held += lock.lockTime.stop();
    }
}
#endif

void Thread::Sleep(const Time &time)
{
    if (time.ms() == 0) {
        Log::warn(TRACE_THREAD, "Attempt to sleep for 0ms, this causes performance problems");
    }
    usleep(time.ms() * MicroSecondsPerMillisecond);
}

void *Thread::CurrentThreadPtr()
{
    if(Thread *thread = CurrentThread())
        return thread->ThreadPtr();
    return
        // oh yuck
#ifdef NRDP_PTHREAD_SELF_ID
        reinterpret_cast<void*>(NRDP_PTHREAD_SELF_ID);
#else
        reinterpret_cast<void*>(pthread_self());
#endif
}

uint32_t Thread::CurrentThreadId()
{
    if(Thread *thread = CurrentThread())
        return thread->ThreadId();
#if defined(__APPLE__)
    return mach_thread_self();
#elif defined(NRDP_HAS_GETTID)
    return syscall(SYS_gettid);
#else
    return 0;
#endif
}

std::string Thread::CurrentThreadName()
{
    if ( Thread * thread = ThreadPointer::get() )
        return thread->GetName();
    return std::string();
}

Thread *Thread::CurrentThread()
{
    return ThreadPointer::get();
}

std::string Thread::GetName() const
{
    std::string result = GetConfig()->GetName();
    if(!name.empty())
        result += " [" + name + "]";
    return result;
}

static ThreadConfig::Configs *s_threadConfigs = 0;

ThreadConfig::ThreadConfig(const char *_name, Thread::Priority _priority, size_t _stackSize)
    : mutex(THREADCONFIG_MUTEX, "ThreadConfig"),
      name(_name), stackSize(_stackSize), priority(_priority),
      numThreads(0), maxThreads(0)
{
    if (!s_threadConfigs)
        s_threadConfigs = new ThreadConfig::Configs;
    (*s_threadConfigs)[_name] = this;
#ifdef NRDP_HAS_PTHREAD_SET_AFFINITY
    CPU_ZERO(&affinity);
#else
    affinity = 0;
#endif
}

ThreadConfig::~ThreadConfig()
{
    {
        for(std::set<Thread*>::iterator it = threads.begin(); it != threads.end(); ++it) {
            assert((*it)->config == this);
            (*it)->config = 0;
        }
    }
    assert(s_threadConfigs);
    std::map<std::string, ThreadConfig*>::iterator it = s_threadConfigs->find(name);
    // make sure we found ourselves in the list
    assert(it != (*s_threadConfigs).end());
    s_threadConfigs->erase(it);
    if (s_threadConfigs->empty()) {
        delete s_threadConfigs;
        s_threadConfigs = 0;
    }
}

void ThreadConfig::SetStackSize(size_t _stackSize)
{
    ScopedMutex _lock(mutex);
    stackSize = _stackSize;
}

void ThreadConfig::SetAffinity(Thread::Affinity _affinity)
{
    ScopedMutex _lock(mutex);
    affinity = _affinity;
    for(std::set<Thread*>::iterator it = threads.begin(); it != threads.end(); ++it)
        (*it)->SetAffinity(affinity);
}

void ThreadConfig::SetPriority(Thread::Priority _priority)
{
    ScopedMutex _lock(mutex);
    priority = _priority;
    for(std::set<Thread*>::iterator it = threads.begin(); it != threads.end(); ++it)
        (*it)->SetPriority(priority);
}

void ThreadConfig::registerThread(Thread *thread)
{
    ScopedMutex _lock(mutex);
    threads.insert(thread);
    if(threads.size() > maxThreads)
        maxThreads = threads.size();
    ++numThreads;
}

void ThreadConfig::unregisterThread(Thread *thread)
{
    ScopedMutex _lock(mutex);
    threads.erase(thread);
}

ThreadConfig * ThreadConfig::find(const std::string &name)
{
    if(!s_threadConfigs)
        return 0;
    for(ThreadConfig::Configs::const_iterator it = s_threadConfigs->begin(); it != s_threadConfigs->end(); ++it) {
        if((*it).second->GetName() == name)
            return (*it).second;
    }
    return 0;
}

std::vector<ThreadConfig*> ThreadConfig::getAllConfigs()
{
    std::vector<ThreadConfig*> configs;
    if(!s_threadConfigs)
        return configs;
    for(ThreadConfig::Configs::const_iterator it = s_threadConfigs->begin(); it != s_threadConfigs->end(); ++it)
        configs.push_back((*it).second);
    return configs;
}

void ThreadConfig::dump(uint8_t flags)
{
    int running = 0;
    const std::vector<ThreadConfig*> configs = getAllConfigs();
    for(std::vector<ThreadConfig*>::const_iterator config_it = configs.begin(); config_it != configs.end(); ++config_it) {
        const std::set<Thread*> threads = (*config_it)->getThreads();
        if(!(flags & ShowAll) && threads.empty())
            continue;
        Log::warn(TRACE_THREAD, "Thread(%p): %s [%zu:%d] [%d/%d]",
                                 (*config_it), (*config_it)->GetName().c_str(),
                                 (*config_it)->GetStackSize(), (*config_it)->GetPriority(),
                                 (*config_it)->getNumThreads(), (*config_it)->getMaxThreads());
        for(std::set<Thread*>::const_iterator thread_it = threads.begin(); thread_it != threads.end(); ++thread_it) {
            ++running;
            const std::string task = (*thread_it)->GetTask();
            Log::warn(TRACE_THREAD, "   %p: %s (%s) [%d]",
                                     (*thread_it)->ThreadPtr(), (*thread_it)->GetName().c_str(), task.empty() ? "RUNNING" : task.c_str(),
                                     (*thread_it)->GetPriority());
#ifdef NRDP_THREAD_TRACK_LOCK
            if(flags & ShowLocks) {
                const std::vector<Thread::LockInfo> locks = (*thread_it)->GetLocks();
                for(std::vector<Thread::LockInfo>::const_iterator lock_it = locks.begin(); lock_it != locks.end(); ++lock_it) {
                    if(!(flags & ShowAll) && !(*lock_it).locked)
                        continue;
                    Log::warn(TRACE_THREAD, "    %c LOCK(%s): %d [%fms(%fms)] [%fms]",
                                             (*lock_it).locked ? '*' : ' ',
                                             (*lock_it).name.c_str(), (*lock_it).locks,
                                             (*lock_it).held,  (*lock_it).held / (*lock_it).locks,
                                             (*lock_it).lockTime.elapsed());
                }
            }
#endif
        }
    }
    Log::warn(TRACE_THREAD, "Running: %d", running);
}

#if defined(__linux__)
bool
Thread::cpu_stats::parse(pid_t tid)
{
    char buffer[8*1024];
    snprintf(buffer, sizeof(buffer), "/proc/%d/task/%d/stat", getpid(), tid);
    if(FILE *fptask = fopen(buffer, "r")) {
        if(!fgets(buffer, sizeof(buffer), fptask)) {
            fclose(fptask);
            return false;
        }
        char *offset = buffer;
        while(*offset++ != ' ');
        while(*offset++ != ')');
        offset++;
        for(int i = 0; i < 11; ++i)
            while(*offset++ != ' ');
        utime_ticks = strtoul(offset, &offset, 10);
        ++offset;
        stime_ticks = strtoul(offset, &offset, 10);
        ++offset;
        cutime_ticks = strtol(offset, &offset, 10);
        ++offset;
        cstime_ticks = strtol(offset, &offset, 10);
        fclose(fptask);
    } else {
        perror("FOPEN ERROR ");
        return false;
    }

#if 0
    if(FILE *fstat = fopen("/proc/stat", "r")) {
        long unsigned int cpu_time[10];
        bzero(cpu_time, sizeof(cpu_time));
        if (fscanf(fstat, "%*s %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu",
               &cpu_time[0], &cpu_time[1], &cpu_time[2], &cpu_time[3],
               &cpu_time[4], &cpu_time[5], &cpu_time[6], &cpu_time[7],
               &cpu_time[8], &cpu_time[9]) == EOF) {
            fclose(fstat);
            return false;
        }
        fclose(fstat);
        for(int i=0; i < 10;i++)
            cpu_total_time += cpu_time[i];
    } else {
        perror("FOPEN ERROR ");
        return false;
    }
#else
    char stat_filepath[PATH_MAX];
    snprintf(stat_filepath, PATH_MAX, "/proc/%d/stat", getpid());
    if(FILE *fstat = fopen(stat_filepath, "r")) {
        if(!fgets(buffer, sizeof(buffer), fstat)) {
            fclose(fstat);
            return false;
        }
        char *offset = buffer;
        while(*offset++ != ' ');
        while(*offset++ != ')');
        offset++;
        for(int i = 0; i < 11; ++i)
            while(*offset++ != ' ');
        cpu_total_time += strtoul(offset, &offset, 10);
        ++offset;
        cpu_total_time += strtoul(offset, &offset, 10);
        ++offset;
        cpu_total_time += strtol(offset, &offset, 10);
        ++offset;
        cpu_total_time += strtol(offset, &offset, 10);
        fclose(fstat);
    } else {
        perror("FOPEN ERROR ");
        return false;
    }
#endif
    return true; //success!
}
#endif

Variant Thread::GetCPUStats() const
{
    Variant result;
    result["name"] = GetName();
    result["id"] = threadId;
    result["pthread"] = threadPtr;
    result["priority"] = GetPriority();
    result["time"] = Time::mono().ms();
#if defined(__linux__)
    cpu_stats stats;
    if(stats.parse(threadId)) {
        const long unsigned int total_time_diff = stats.cpu_total_time -
                                                  mStats.cpu_total_time;
        if(total_time_diff) {
            result["user_ticks"] = stats.utime_ticks;
            result["user_time"] = 100 * (((stats.utime_ticks + stats.cutime_ticks) -
                                          (mStats.utime_ticks + mStats.cutime_ticks)) / (double) total_time_diff);
            result["system_ticks"] = stats.stime_ticks;
            result["system_time"] = 100 * ((((stats.stime_ticks + stats.cstime_ticks) -
                                             (mStats.stime_ticks + mStats.cstime_ticks))) / (double) total_time_diff);
        }
        mStats = stats;
    }
#elif defined(__APPLE__)
    mach_msg_type_number_t count;
    thread_basic_info_data_t info;

    count = THREAD_BASIC_INFO_COUNT;
    thread_info(threadId, THREAD_BASIC_INFO, (thread_info_t) &info, &count);
    result["user_ticks"] = (info.user_time.seconds * 1000 * 1000) + info.user_time.microseconds;
    result["system_ticks"] = (info.system_time.seconds * 1000 * 1000) + info.system_time.microseconds;
    result["user_time"] = std::min<int>(100, info.cpu_usage);
    result["system_time"] = 0;
#endif
    return result;
}

void Thread::Disown()
{
    if(Thread *thread = CurrentThread()) {
        if(thread->config)
            thread->config->unregisterThread(thread);
        ThreadPointer::set(0);
    }
    assert(!MutexStack::size());
}

Thread * Thread::Adopt(ThreadConfig *config)
{
    if(Thread *thread = CurrentThread()) {
        assert(thread->config == config);
        return thread;
    }

    Thread *thread = new Thread(config);
    thread->threadId = Thread::CurrentThreadId();
    thread->threadPtr = Thread::CurrentThreadPtr();
    thread->impl->pThread = pthread_self();
    thread->SetState(RUNNING);
    if(config) {
        config->registerThread(thread);
        if(Thread::Priority priority = config->GetPriority())
            thread->SetPriority(priority);
#ifdef NRDP_HAS_PTHREAD_SET_AFFINITY
        Affinity affinity = config->GetAffinity();
        CPU_OR(&affinity, &affinity, &sGlobalAffinity);
        thread->SetAffinity(affinity);
#endif
#if defined(HAVE_PRCTL_H)
        prctl(PR_SET_NAME, config->GetName().c_str(), 0, 0, 0);
#elif defined(NRDP_HAS_PTHREAD_SETNAME_1)
        pthread_setname_np(config->GetName().c_str());
#elif defined(NRDP_HAS_PTHREAD_SETNAME_2)
        pthread_setname_np(pthread_self(), config->GetName().c_str());
#elif !defined(HAS_PTHREAD_ATTR_SETNAME_NP)
# warning "Thread naming is not supported"
#endif
    }
    ThreadPointer::set(thread);
    return thread;
}

void Thread::SetSingleCore(bool single)
{
#ifdef NRDP_HAS_PTHREAD_SET_AFFINITY
    CPU_ZERO(&sGlobalAffinity);
    if(single)
        CPU_SET(0, &sGlobalAffinity);
    const std::vector<ThreadConfig*> configs = ThreadConfig::getAllConfigs();
    for(std::vector<ThreadConfig*>::const_iterator config_it = configs.begin(); config_it != configs.end(); ++config_it) {
        const std::set<Thread*> threads = (*config_it)->getThreads();
        for(std::set<Thread*>::const_iterator thread_it = threads.begin(); thread_it != threads.end(); ++thread_it) {
            Thread::Affinity affinity;
            if (single)
                affinity = sGlobalAffinity;
            else
                affinity = (*config_it)->GetAffinity();
            (*thread_it)->SetAffinity(affinity);
        }
    }
#else
    (void)single;
#endif
}
