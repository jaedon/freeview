/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#ifndef Thread_h
#define Thread_h

#include <set>
#include <vector>

#include <nrdbase/config.h>
#include <nrdbase/Noncopyable.h>
#include <nrdbase/tr1.h>
#include <nrdbase/Exportable.h>

#include <nrdbase/NFErr.h>
#include <nrdbase/Stopwatch.h>
#include <nrdbase/Mutex.h>
#include <nrdbase/ScopedMutex.h>
#include <nrdbase/Time.h>

namespace netflix {

class ThreadConfig;

/**
 * @class Thread Thread.h
 * @brief Kernel thread abstraction implemented using pthreads.
 */
class NRDP_EXPORTABLE Thread : private Noncopyable
{
public:
    /**
     * Construct a new thread with the same priority as the creating
     * thread.
     *
     * @param[in] name optional thread name.
     */
    Thread(ThreadConfig *config, const std::string &name=std::string());

    /** Destructor. */
    virtual ~Thread();

    /** Thread state. */
    typedef enum {
        NEW,
        STARTING,
        RUNNING,
        TERMINATED,
        JOINED
    } State;

    typedef uint8_t Priority;
#ifdef NRDP_HAS_PTHREAD_SET_AFFINITY
    typedef cpu_set_t Affinity;
#else
    typedef uint8_t Affinity;
#endif

    static const Priority minPriority;     // Minimum thread priority (1).
    static const Priority maxPriority;     // Maximum thread priority (99).
    static const size_t defaultStackSize;  // Default stack size (65k).

    inline ThreadConfig *GetConfig() const { return config; }

    /**
     * Returns the thread priority.
     *
     * @return the thread priority.
     */
    Priority GetPriority() const;

    /**
     * Returns the thread affinity.
     *
     * @return the thread affinity.
     */
    Affinity GetAffinity() const;

    /**
     * Returns the thread state.
     *
     * @return the thread state.
     */
    State GetState() const;

    /**
     * Waits for thread to transition to state state.
     *
     * @return NFErr_OK if the thread has transitioned to expected state;
     *         NFErr_TimedOut if the timeout period expires;
     */
    netflix::NFErr WaitForState(State state, const Time &time = Time(0));

    /**
     * Returns the thread cpu usage.
     *
     * @return the thread cpu usage.
     */
    Variant GetCPUStats() const;

    /**
     * Set to true for the thread to delete itself once the Run()
     * method returns. By default a thread does not do this.
     *
     * N.B. This method must be called before Start() to avoid a
     * memory leak.
     */
    void SetDeleteSelf(bool flag);

    /**
     * Starts the thread with the specified stack size. This calls
     * Run().
     *
     */
    virtual void Start();

    /**
     * Block until concrete subclass has signalled that it is running.
     *
     */
     inline void WaitRunning() { WaitForState(RUNNING); }

    /**
     * Waits for the thread to terminate.
     *
     * @param[in] time wait timeout.
     * @return NFErr_OK if the thread has terminated;
     *         NFErr_TimedOut if the timeout period expires;
     *         NFErr_Interrupted it the timeout was interrupted;
     *         NFErr_WaitingSelf if the thread is waiting for itself.
     */
    netflix::NFErr Wait(const Time &time = Time(0));

    /**
     * Makes the current thread sleep.
     *
     * @param[in] time sleep time.
     */
    static void Sleep(const Time &time);

    static void SetSingleCore(bool single);

    virtual std::string GetTask() const { return std::string(); }

#ifdef NRDP_THREAD_TRACK_LOCK
    struct LockInfo {
        LockInfo() : locked(false), locks(0), recurse(0), lockTime(false), held(0) {}
        std::string name;
        bool locked;
        int locks, recurse;
        Stopwatch lockTime;
        Stopwatch::ms held;
    };
    std::vector<LockInfo> GetLocks();
    void Locked(const char *name);
    void Unlocked(const char *name, bool force=false);
#endif

    static void Disown();
    static Thread *Adopt(ThreadConfig *config);

    /**
     * Returns the current thread.
     */
    static Thread *CurrentThread();

    /**
     * Returns the current thread's name.
     */
    static std::string CurrentThreadName();

    /**
     * Return the thread name.
     *
     * @return the thread name.
     */
    std::string GetName() const;

    /**
     * Returns the current thread's id.
     */
    static uint32_t CurrentThreadId();

    /**
     * Returns the current thread's pointer
     */
    static void *CurrentThreadPtr();

    /**
     * Returns the thread's id.
     */
    inline uint32_t ThreadId() { return threadId; }

    /**
     * Returns the thread's pthread_t.
     */
    inline void *ThreadPtr() { return threadPtr; }

    /**
     * Sets the thread priority. You can't call this directly; set the
     * ThreadConfig's priority to set this. But hopefully you're setting the
     * ThreadConfig's priority before you even create the thread.
     *
     * @param[in] priority new new thread priority. Valid values are
     *            [1,99] with 1 being low priority and 99 being high
     *            priority.
     * @return the previous thread priority.
     */
    void SetPriority(Priority priority);

    /**
     * Specifies to which processor the thread can be assigned
     *
     * @param[in] _affinity A bit mask specifying allowed processors, 0 means platform default
     */
    void SetAffinity(Affinity _affinity);

    typedef int (*GlobalSpawnFunc)(Thread *, void *attr);
    static void SetGlobalSpawn(GlobalSpawnFunc func) { sGlobalSpawnFunc = func; }
    int Spawn(void *attr);

protected:
    friend class ThreadConfig;

    class Impl;
    static Affinity sGlobalAffinity;
    static GlobalSpawnFunc sGlobalSpawnFunc;
    std::auto_ptr<Impl> const impl; //!< Thread implementation.
    ThreadConfig *config;           //!< Thread configuration.
    std::string name;
    uint32_t threadId;
    void *threadPtr;
#ifdef NRDP_THREAD_TRACK_LOCK
    std::map<const char *, LockInfo> locks;
#endif
#ifdef __linux__
    struct cpu_stats {
        inline cpu_stats() {
            utime_ticks = stime_ticks = 0;
            cutime_ticks = cstime_ticks = 0;
            cpu_total_time = 0;
        }
        bool parse(pid_t pid);
        long unsigned int utime_ticks;
        long int cutime_ticks;
        long unsigned int stime_ticks;
        long int cstime_ticks;
        long unsigned int cpu_total_time;
    };
    mutable cpu_stats mStats;
#endif

private:
    /** Called by Start() to execute the thread. */
    virtual void Run() { assert(false); }

    /**
     * The startup callback function.
     *
     * @param[in] p the Thread.
     * @return NULL.
     */
    static void* Spawn_internal(void* p);

    /**
     * Transition state
     *
     * @param[in] state the state being entered.
     */
    void SetState(State state) const;
};

class ThreadConfig : public Noncopyable {
public:
    typedef std::map<std::string, ThreadConfig*> Configs;

    ThreadConfig(const char *_name, Thread::Priority _priority=0, size_t _stackSize=Thread::defaultStackSize);
    ~ThreadConfig();

    inline const std::string &GetName() const { return name; }

    void SetStackSize(size_t _stackSize);
    inline size_t GetStackSize() const { return stackSize; }

    void SetPriority(Thread::Priority _priority);
    inline Thread::Priority GetPriority() const { return priority; }

    void SetAffinity(Thread::Affinity _affinity);
    inline Thread::Affinity GetAffinity() const { return affinity; }

    static ThreadConfig *find(const std::string&);
    static std::vector<ThreadConfig*> getAllConfigs();
    inline std::set<Thread*> getThreads() const { ScopedMutex _lock(mutex); return threads; }
    inline int getMaxThreads() const { return maxThreads; }
    inline int getNumThreads() const { return numThreads; }

    /**
     * For debugging, Log::warn() with information about each thread config and all of its threads
     */
    enum {
        ShowAll          = 0x01,
        ShowLocks        = 0x02
    };
    static void dump(uint8_t flags);

    bool operator==(const ThreadConfig &other) const { return other.name == name; }
    bool operator!=(const ThreadConfig &other) const { return other.name != name; }

protected:
    friend class Thread;
    void registerThread(Thread *);
    void unregisterThread(Thread *);

    mutable Mutex mutex;
    std::string name;
    size_t stackSize;
    Thread::Priority priority;
    Thread::Affinity affinity;

    std::set<Thread*> threads;
    size_t numThreads, maxThreads;
};

#define DEFINE_THREAD(name, ...) NRDP_EXPORTABLE netflix::ThreadConfig THREAD_ ## name(#name, __VA_ARGS__)
#define DECLARE_THREAD(name) extern netflix::ThreadConfig THREAD_ ## name

DECLARE_THREAD(TELNET_SERVER);
DECLARE_THREAD(TELNET_CONNECTION);
DECLARE_THREAD(DISK_CACHE);
DECLARE_THREAD(RESOURCE_HTTP);
DECLARE_THREAD(HTTPREQUEST_MANAGER);
DECLARE_THREAD(NBP_POOL);
DECLARE_THREAD(DNS_MANAGER);
DECLARE_THREAD(MC_EVENT);
DECLARE_THREAD(MC_PUMPING);
DECLARE_THREAD(MC_PUMPING2);
DECLARE_THREAD(WEBSOCKET);

} // namespace netflix

#endif
