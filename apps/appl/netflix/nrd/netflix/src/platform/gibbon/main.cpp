/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include <nrdbase/config.h>
#include <nrdbase/LRU.h>
#include <gibbon/config.h>

#include <GibbonConfiguration.h>
#include <GibbonApplication.h>
#include <nrdbase/Serialize.h>

#include <nrd/AppLog.h>
#include <nrd/AppThread.h>

#include <nrdbase/Configuration.h>
#include <nrdbase/Instrumentation.h>
#include <nrdbase/Stopwatch.h>
#include <nrdbase/Backtrace.h>
#include <nrdbase/Exportable.h>
#include <nrdbase/Version.h>
#include <nrdbase/ReadDir.h>

#include <stdio.h>
#include <errno.h>
#include <fcntl.h>

#ifdef HAVE_MCHECK_H
# include <mcheck.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>

using namespace netflix;
using namespace netflix::gibbon;

#if defined(HAVE_SIGNAL_HANDLERS) && !defined(NRDP_SIGNAL_HANDLER)
# include <signal.h>
# define NRDP_SIGNAL_HANDLER 1
#endif
#ifdef NRDP_SIGNAL_HANDLER
# define NRDP_WATCHDOG 1
#endif

// #undef NRDP_SIGNAL_HANDLER
#ifdef NRDP_SIGNAL_HANDLER
#ifdef NRDP_CRASH_REPORTING_ENABLED
static int sCrashFD = -1;
std::string sCrashFile;
#ifdef NRDP_CRASH_REPORTING_BREAKPAD
#ifdef NRDP_CPP11
#        define typeof __typeof__
#      endif
#      include <client/linux/handler/exception_handler.h>
#else
#      include <stdlib.h>
#    endif
#  endif
#if defined(NRDP_HAS_EXECINFO_H) && NRDP_SIGNAL_HANDLER > 0
#   include <execinfo.h>
#  endif
#elif defined(NRDP_CRASH_REPORTING_BREAKPAD)
#  undef NRDP_CRASH_REPORTING_ENABLED
#ifdef NRDP_CRASH_REPORTING_BREAKPAD
#    undef NRDP_CRASH_REPORTING_BREAKPAD
#  endif
#endif


class GibbonShutdownThread : public Thread
{
public:
    GibbonShutdownThread() : Thread(&THREAD_GIBBON_SHUTDOWN), mMutex(ZERO_MUTEX, "ShutdownThread::mMutex")
    {
        if(pipe(mPipe) == -1)
            Log::error(TRACE_LOG, "Unable to create pipe for GibbonShutdownThread (%d)", errno);
    }

    ~GibbonShutdownThread()
    {
        Wait();
        close(mPipe[0]);
        close(mPipe[1]);
    }

    inline void shutdown()
    {
        int ret;
        do {
            ret = write(mPipe[1], "q", 1);
        } while(ret == -1 && errno == EINTR);
        if(ret != 1)
            fprintf(stderr, "ShutdownThread::error failed to write to pipe %d/%d\n", ret, errno);
    }

    virtual void Run()
    {
        fd_set read;
        FD_ZERO(&read);
        FD_SET(mPipe[0], &read);
        int ret;
        do {
            ret = select(mPipe[0] + 1, &read, 0, 0, 0);
        } while (ret == -1 && errno == EINTR);

        assert(ret);
        if(ret == -1) {
            fprintf(stderr, "ShutdownThread::Error selecting on pipe %d\n", errno);
            return;
        }

        if(NrdApplication *app = nrdApp())
            app->scheduleQuit();
    }
private:
    Mutex mMutex;
    int mPipe[2];
};
static shared_ptr<GibbonShutdownThread> sShutdownThread;

static void libedit_cleanup()
{
#ifdef HAVE_LIBEDIT
    if(GibbonConfiguration::libEditEnabled())
        GibbonConsole::cleanupLibEdit();
#endif
}

#if defined(NRDP_CRASH_REPORTING_INTERNAL)
static void itoafd(int fd, unsigned int val, int base)
{
    static char buf[32] = {0};
    int i = 30;
    for(; val && i ; --i, val /= base)
        buf[i] = "0123456789abcdef"[val % base];

    ssize_t bytes = write(fd, &buf[i+1], 30 - i);
    (void)bytes;
}

// Write crash report section header
static void writeSection(int fd, const char* name)
{
    ssize_t bytes = write(fd, "---------- ", 11);
    bytes = write(fd, name, strlen(name));
    bytes = write(fd, "\n", 1);
    (void)bytes;
}

// Write file contents to the crash report
static void writeFile(int fd, const char* fileName)
{
    int result;
    (void)result;
    ssize_t bytes;
    (void)bytes;
    int readfd = open(fileName, O_RDONLY);

    // Error opening file
    if (readfd < 0) {
        bytes = write(fd, "ERROR: Can't open ", 18);
        bytes = write(fd, fileName, strlen(fileName));
        bytes = write(fd, "\n", 1);
        return;
    }

    static char data[4096] = {0}; // 4k buffer to hold file contents

    bytes = read(readfd, data, 4096);
    while (bytes) {
        bytes = write(fd, data, bytes);
        bytes = read(readfd, data, 4096);
    }
    result = close(readfd);
}

static void writeProcFile(int fd, const char* fileName)
{
    // string buffer
    static char buffer[128] = {0};

    // atoi buffer
    static char buf[32] = {0};
    int i = 30;
    int val = getpid();
    for(; val && i ; --i, val /= 10)
        buf[i] = "0123456789abcdef"[val % 10];

    buffer[0] = '\0';
    (void)strncat(buffer, "/proc/", 6);
    (void)strncat(buffer, &buf[i+1], 30 - i);
    (void)strncat(buffer, "/", 1);
    (void)strncat(buffer, fileName, strlen(fileName));

    writeFile(fd, buffer);
}
#endif

#ifdef NRDP_SIGNAL_HANDLER
struct sigaction old_sigsegv, old_sigbus, old_sigabrt, old_sigill, old_sigfpe;
static void gibbon_signalHandler(int signal, siginfo_t *info, void */*context*/)
{
    if(signal == SIGINT || signal == SIGTERM) {
        if(sShutdownThread) {
            sShutdownThread->shutdown();
            return;
        }
        _exit(signal);
    }

    // Write our own crash log only if breakpad is not enabled, otherwise we would overwrite it
#if defined(NRDP_CRASH_REPORTING_ENABLED) && defined(NRDP_CRASH_REPORTING_INTERNAL)
    if (sCrashFD != -1) {
        // Crash report header, Netflix 1 format.
        ssize_t bytes = write(sCrashFD, "NFLX1\n", 6);

        // Write the build id
        bytes = write(sCrashFD, GIBBON_BUILD_ID, 32);
        bytes = write(sCrashFD, "\n", 1);

        // Write the signal number
        itoafd(sCrashFD, signal, 10);
        bytes = write(sCrashFD, "\n", 1);

        // Write the crash address
        bytes = write(sCrashFD, "0x", 2);
        itoafd(sCrashFD, (unsigned int)info->si_addr, 16);
        bytes = write(sCrashFD, "\n", 1);
        (void)bytes;

        // Backtrace
        writeSection(sCrashFD, "backtrace");
        Backtrace::backtraceToFD(sCrashFD);

        // Process map
        writeSection(sCrashFD, "maps");
        writeProcFile(sCrashFD, "maps");

        // Command line
        writeSection(sCrashFD, "cmdline");
        writeProcFile(sCrashFD, "cmdline");

        // Close
        bytes = write(sCrashFD, "\n", 1);
        int result = close(sCrashFD);
        (void)result;
    }
#else
    (void)info;
#endif

    fprintf(stderr, "SIGNAL [%d] at address %p!\n", signal, info->si_addr);
#if defined(BUILD_DEBUG)
#if NRDP_SIGNAL_HANDLER > 0 && defined(NRDP_HAS_EXECINFO_H)
    void *stack[128];
    const int size = ::backtrace(stack, sizeof(stack) / sizeof(stack[0]));
    if (size > 0) {
        char **symbols = ::backtrace_symbols(stack, size);
        if (symbols) {
            for (int i=0; i<size - 1; ++i) {
                fprintf(stderr, "%d/%d %s (%p)\n", i + 1, size - 1, symbols[i], stack[i]);
            }
        }
    }
#endif
#endif
    fflush(stderr);

#ifdef HAVE_LIBEDIT
    libedit_cleanup();
#endif

    struct sigaction *previousHandler;
    switch (signal) {
    case SIGSEGV:
        previousHandler = &old_sigsegv;
        break;
    case SIGBUS:
        previousHandler = &old_sigbus;
        break;
    case SIGABRT:
        previousHandler = &old_sigabrt;
        break;
    case SIGILL:
        previousHandler = &old_sigill;
        break;
    case SIGFPE:
        previousHandler = &old_sigfpe;
        break;
    default:
        _exit(signal);
        break;
    }
    if (previousHandler->sa_handler != SIG_DFL
        && previousHandler->sa_handler != SIG_ERR
        && previousHandler->sa_handler != SIG_IGN
        && previousHandler->sa_handler != NULL) {
        (previousHandler->sa_handler)(signal);
    } else {
        _exit(signal);
    }
}
#endif

#ifdef NRDP_CRASH_REPORTING_BREAKPAD
static bool dumpCallback(const google_breakpad::MinidumpDescriptor& /*descriptor*/, void* /*context*/, bool /*succeeded*/)
{
#ifdef BUILD_DEBUG
    fprintf(stderr, "Minidump file written\n");
    fflush(stderr);
#endif

    // Return false so the next handler in the signal handler chain is called
    return false;
}
#endif

#if defined(GIBBON_NICE_THREADS) && defined(NRDP_HAS_GETTID)
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/time.h>
#include <sys/resource.h>
DECLARE_THREAD(NICESPAWNER);
DEFINE_THREAD(NICESPAWNER, 0, 0);
static inline int mapPriority(int p) { return (19 - (float(p - Thread::minPriority) / float(Thread::maxPriority - Thread::minPriority) * 19)); }
static ThreadPool::SharedPtr sThreadSpawner;
class ThreadSpawnJob : public ThreadPoolJob
{
public:
    inline ThreadSpawnJob(Thread *thread, void *attr) : mThread(thread), mAttr(attr), mMutex(ZERO_MUTEX, "ThreadSpawnJob::Mutex"), mSpawned(false) { }
    virtual std::string description() const { return "ThreadSpawnJob"; }

    virtual void execute()
    {
        mResult = mThread->Spawn(mAttr);
        if(ThreadConfig *config = mThread->GetConfig()) {
            NRDP_UNUSED(config);
            const int thread_prio = mThread->GetConfig()->GetPriority();
            if(thread_prio && mThread->WaitForState(Thread::RUNNING) == NFErr_OK) {
                const int prio = mapPriority(thread_prio);
                //Log::error(TRACE_LOG, "%s: Change priority: %d -> %d", config->GetName().c_str(), thread_prio, prio);
                setpriority(PRIO_PROCESS, mThread->ThreadId(), prio);
            }
        }
        ScopedMutex scope(mMutex);
        mSpawned = true;
        mCond.signal();
    }

    inline int wait()
    {
        ScopedMutex scope(mMutex);
        while(!mSpawned)
            mCond.wait(&mMutex);
        return mResult;
    }

private:
    Thread *mThread;
    void *mAttr;

    int mResult;
    Mutex mMutex;
    bool mSpawned;
    ConditionVariable mCond;
};
static int NiceThreadSpawn(Thread *thread, void *attr)
{
    if(thread->GetConfig() == &THREAD_NICESPAWNER)
        return thread->Spawn(attr);

    assert(sThreadSpawner);
    shared_ptr<ThreadSpawnJob> job(new ThreadSpawnJob(thread, attr));
    sThreadSpawner->post(job);
    return job->wait();
}
#endif

static weak_ptr<LogCatcher> sCatcher;
static int gibbon_launch()
{
#ifdef HAVE_LIBEDIT
    atexit(libedit_cleanup);
#endif

#if defined(GIBBON_NICE_THREADS) && defined(NRDP_HAS_GETTID)
    Thread::SetGlobalSpawn(NiceThreadSpawn);
    sThreadSpawner.reset(new ThreadPool);
    sThreadSpawner->init(1, &THREAD_NICESPAWNER);
    setpriority(PRIO_PROCESS, syscall(SYS_gettid), mapPriority(THREAD_GIBBON_MAIN.GetPriority()));
#endif

#ifdef NRDP_SIGNAL_HANDLER
    if(GibbonConfiguration::shutdownThreadEnabled()) {
        sShutdownThread.reset(new GibbonShutdownThread);
        sShutdownThread->Start();
    }

#ifdef NRDP_CRASH_REPORTING_ENABLED
    // Crash reporting is enabled. Create a file descriptor now so we don't have to call
    // an unsafe function at crash time.
    if(Configuration::crashReportEnabled()) {
        const std::string crashPath = Configuration::crashLogPath();
        NTRACE(TRACE_STARTUP, "Crash log directory is %s", crashPath.c_str());
        ReadDir::recursiveMakeDirectory(crashPath.c_str(), S_IRWXU);
        sCrashFile = crashPath + NRDP_PATH_SEPARATOR_CHAR + "crash.log";
        sCrashFD = open(sCrashFile.c_str(), O_CREAT | O_WRONLY , S_IRWXU);
    }
#endif

    // Our own signal handler
    struct sigaction new_action;
    memset(&new_action, 0, sizeof(new_action));
    new_action.sa_sigaction = gibbon_signalHandler;
    sigemptyset(&new_action.sa_mask);
    new_action.sa_flags = SA_SIGINFO;
    // Gracefully shutdown when told to terminate
    sigaction(SIGINT, &new_action, NULL);
    sigaction(SIGTERM, &new_action, NULL);

#ifdef NRDP_CRASH_REPORTING_BREAKPAD
    // Enable breakpad. This has to be done before our signal handler
    shared_ptr<google_breakpad::ExceptionHandler> eh;
    if (Configuration::crashReportEnabled()) {
        google_breakpad::MinidumpDescriptor descriptor(sCrashFD);
        eh.reset(new google_breakpad::ExceptionHandler(descriptor, NULL, dumpCallback, NULL, true, -1, google_breakpad::DUMP_MEDIUM | google_breakpad::DUMP_LIMIT_STACK_SIZE));
    } else
# endif
    {
        // Save the old signal handlers
        sigaction(SIGSEGV, &new_action, &old_sigsegv);
        sigaction(SIGBUS, &new_action, &old_sigbus);
        sigaction(SIGABRT, &new_action, &old_sigabrt);
        sigaction(SIGILL, &new_action, &old_sigill);
        sigaction(SIGFPE, &new_action, &old_sigfpe);
    }

    signal(SIGPIPE, SIG_IGN);

#endif

    gibbon_oem_event(GibbonOEM_Start);
    INSTRUMENTATION_EVENT_DEBUG("gibbon.oem-start");

    GibbonApplication app;
    if (shared_ptr<LogCatcher> catcher = sCatcher.lock()) {
        sCatcher.reset();
        app.setLogCatcher(catcher);
    }
    INSTRUMENTATION_EVENT_DEBUG("gibbon.create-application");
    const int ret = app.exec();
    NTRACE(TRACE_NBP, "quit: exec returned %d", ret);

#if defined(NRDP_SIGNAL_HANDLER) && defined(NRDP_CRASH_REPORTING_ENABLED)
    if(sCrashFD != -1)
        close(sCrashFD);
    (void)unlink(sCrashFile.c_str());
#endif
    gibbon_oem_event(GibbonOEM_Cleanup);

    return ret;
}

static void gibbon_cleanup()
{
    libedit_cleanup();

    NTRACE(TRACE_NBP, "quit: oem cleanup returned");

#ifdef NRDP_SIGNAL_HANDLER
    if(sShutdownThread) {
        sShutdownThread->shutdown();
        sShutdownThread.reset();
    }
#endif

#if defined(GIBBON_NICE_THREADS) && defined(NRDP_HAS_GETTID)
    if(sThreadSpawner) {
        Thread::SetGlobalSpawn(0);
        sThreadSpawner->shutdown();
        sThreadSpawner.reset();
    }
#endif

    Thread::Disown();

#ifdef BUILD_DEBUG
    const std::vector<ThreadConfig*> configs = ThreadConfig::getAllConfigs();
    for (std::vector<ThreadConfig*>::const_iterator config_it = configs.begin(); config_it != configs.end(); ++config_it) {
        const std::set<Thread*> threads = (*config_it)->getThreads();
        if(threads.empty())
            continue;
        fprintf(stderr, "%s still has %zu thread %s\n", (*config_it)->GetName().c_str(), threads.size(),
                threads.size() == 1 ? "" : "s");
        assert(threads.empty());
    }
#endif
}

#ifdef NRDP_WATCHDOG
static Mutex sWatchdogMutex(ZERO_MUTEX, "Gibbon::WatchdogLock");
static int sWatchdogFD = -1;

void gibbon_watchdog_event(GibbonWatchdogEvent event, const DataBuffer &data)
{
    ScopedMutex _lock(sWatchdogMutex);
    NRDP_UNUSED_RETURN(write(sWatchdogFD, &event, sizeof(event)));
    const unsigned int length = data.length() + sizeof(event);
    NRDP_UNUSED_RETURN(write(sWatchdogFD, &length, sizeof(length)));
    if(!data.empty())
        NRDP_UNUSED_RETURN(write(sWatchdogFD, data.constData(), data.length()));
}

static void watchdog_signalHandler(int signal)
{
    printf("WatchdogSignal:%d: %d\n", sWatchdogFD, signal);
    if(signal == SIGCHLD)
        gibbon_watchdog_event(GibbonWatchdog_SigChild);
    else if(signal == SIGTERM || signal == SIGINT)
        gibbon_watchdog_event(GibbonWatchdog_Shutdown);
}

static void watchdog_launch_internal()
{
    setsid();
    if(Configuration::consoleEnabled())
        tcsetpgrp(STDIN_FILENO, getpid());

    const int ret = gibbon_launch();
    gibbon_cleanup();
    exit(ret);
}

static bool watchdog_launch()
{
    int childPipe[2];
    if(pipe(childPipe) == -1)
        Log::error(TRACE_LOG, "Unable to create pipe for child (%d)", errno);
    if(int child = fork()) {
        if(child == -1) {
            printf("Failed to fork!\n");
            return false;
        }

        struct sigaction new_action;
        memset(&new_action, 0, sizeof(new_action));
        new_action.sa_handler = watchdog_signalHandler;
        sigemptyset(&new_action.sa_mask);
        new_action.sa_flags = 0;

        sigaction(SIGCHLD, &new_action, 0);
        sigaction(SIGTERM, &new_action, 0);
        sigaction(SIGINT, &new_action, 0);

        int watcherPipe[2];
        if(pipe(watcherPipe) == -1)
            Log::error(TRACE_LOG, "Unable to create pipe for watcher (%d)", errno);
        sWatchdogFD = watcherPipe[1];

        std::list<int> pipes;
        pipes.push_back(watcherPipe[0]);
        pipes.push_back(childPipe[0]);
        printf("WatchdogLaunched(%d::%d): %d(%d::%d)\n", watcherPipe[0], watcherPipe[1],
               child, childPipe[0], childPipe[1]);

        bool done = false;
        while(!done) {
            int max_fd = 0;
            fd_set read_fd_set;
            FD_ZERO(&read_fd_set);
            for(std::list<int>::const_iterator it = pipes.begin(); it != pipes.end(); ++it) {
                const int fd = (*it);
                FD_SET(fd, &read_fd_set);
                if(fd > max_fd)
                    max_fd = fd;
            }
            timeval tm = { 15, 0 };
            const int ret = select(max_fd+1, &read_fd_set, 0, 0, &tm);
            if(ret == 0) {
                gibbon_oem_event(GibbonOEM_WatchdogNoHeartbeat, &child);
                continue;
            }
            for(std::list<int>::const_iterator it = pipes.begin(); !done && it != pipes.end(); ++it) {
                const int fd = (*it);
                if(!FD_ISSET(fd, &read_fd_set))
                    continue;

                GibbonWatchdogEvent event;
                if(read(fd, &event, sizeof(event)) == -1) {
                    done = true;
                    break;
                }
                unsigned int length;
                if(read(fd, &length, sizeof(length)) == -1) {
                    done = true;
                    break;
                }
                DataBuffer data;
                if(const int remainder = length-sizeof(event)) {
                    data.resize(remainder);
                    if(read(fd, data.data(), remainder) == -1) {
                        done = true;
                        break;
                    }
                }
                //printf("WatchdogEvent: %d\n", event);
                switch(event) {
                case GibbonWatchdog_Instrumentation: {
                    //printf("Heatbeat: %d\n", child);
                    break; }
                case GibbonWatchdog_Relaunch:
                case GibbonWatchdog_Shutdown: {
                    if(child != -1) {
                        kill(child, SIGTERM);
                        waitpid(child, 0, 0);
                        printf("Killed: %d\n", child);
                        child = -1;
                    }
                    if(event == GibbonWatchdog_Shutdown) {
                        done = true;
                        break;
                    }
                    child = fork();
                    if(child == -1) {
                        done = true;
                        break;
                    } else if(!child) {
                        watchdog_launch_internal();
                        done = true;
                        break;
                    }
                    printf("WatchdogRelaunched: %d\n", child);
                    break; }
                case GibbonWatchdog_SigChild: {
                    int status;
                    waitpid(child, &status, 0);
                    printf("WatchdogReaped: %d [%d]\n", child, status);
                    gibbon_oem_event(GibbonOEM_WatchdogSigChild, &child);
                    if(!status) {
                        done = true;
                        break;
                    }
                    child = fork();
                    if(child == -1) {
                        done = true;
                        break;
                    } else if(!child) {
                        done = true;
                        break;
                    }
                    printf("WatchdogRelaunched: %d\n", child);
                    break; }
                }
            }
        }
        close(watcherPipe[0]);
        close(watcherPipe[1]);
        close(childPipe[0]);
        close(childPipe[1]);
    } else {
        sWatchdogFD = childPipe[1];
        watchdog_launch_internal();
    }
    return true;
}

#else
void gibbon_watchdog_event(GibbonWatchdogEvent, const DataBuffer &)
{
}
#endif

extern "C" {
NRDP_EXPORTABLE int gibbon_main(int argc, char **argv)
{
#ifdef NRDP_SIGNAL_HANDLER
    memset(&old_sigsegv, 0, sizeof(old_sigsegv));
    memset(&old_sigfpe, 0, sizeof(old_sigfpe));
    memset(&old_sigill, 0, sizeof(old_sigill));
    memset(&old_sigabrt, 0, sizeof(old_sigabrt));
#endif

    shared_ptr<LogCatcher> catcher(new LogCatcher(1024));
    sCatcher = catcher;
    LogSink::add(catcher);
    Thread::Adopt(&THREAD_GIBBON_MAIN);
#ifdef HAVE_MCHECK_H
    if(const char *mtrace_file = getenv("MALLOC_TRACE")) {
        printf("DUMPING MEMORY TRACE TO: %s\n", mtrace_file);
        mtrace();
    }
#endif
    Log::milestone("gibbon.gibbon_main");
    INSTRUMENTATION_EVENT_ON("gibbon.main", Variant("NRDP",NRDP_VERSION_STRING));
    gibbon_oem_event(GibbonOEM_Init);
    INSTRUMENTATION_EVENT_DEBUG("gibbon.oem-init");

    GibbonApplication::setArgs(argc, argv);

    int ret = 0;
    NTRACE(TRACE_PERFORMANCE, "Bookmark: Entry Point");
    if(!GibbonConfiguration::init(argc, argv)) {
        ret = 1;
    } else {
#ifdef NRDP_WATCHDOG
        if(!GibbonConfiguration::watchdogEnabled() || !watchdog_launch())
#endif
            ret = gibbon_launch();
        gibbon_cleanup();
    }

#ifdef INSTRUMENTATION_ENABLED
    instrumentation::getQueue()->clear();
#endif
    return ret;
}
}

#if defined(GIBBON_MAIN)
int main(int argc, char **argv)
{
    Log::milestone("gibbon.main");
    return gibbon_main(argc, argv);
}
#endif
