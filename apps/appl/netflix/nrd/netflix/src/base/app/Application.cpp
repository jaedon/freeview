/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "Application.h"
#include <nrdbase/Version.h>
#include <nrdbase/OpenSSLLib.h>
#include <nrdbase/EmbeddedResources.h>
#include <nrdbase/PerformanceMarker.h>
#include <nrdbase/LogFileSink.h>
#include <nrdbase/ReadDir.h>
#if defined(NRDP_PLATFORM_LINUX) && !defined(__APPLE__)
#include <features.h>
#endif

using namespace netflix;
using namespace resources;

Application *Application::sInstance = 0;
const Time Application::MONO_STARTED(Time::mono());

class ObjectCountTimer : public Application::Timer
{
public:
    ObjectCountTimer(int interval)
        : Application::Timer(interval)
    {}

    virtual std::string describe() const { return "ObjectCountTimer"; }

    virtual void timerFired()
    {
        PERFORMANCE_MARKER_SCOPED("objectcounttimer.fired");
        ObjectCount::dump();
    }
};

Application::Application() : mMutex(ZERO_MUTEX, "ApplicationMutex"), mThread(Thread::CurrentThreadPtr())
{
    initOpenSSL();
    mTraceAreas.reset(new TraceAreas);
    assert(!sInstance);
    sInstance = this;
}

Application::~Application()
{
    assert(sInstance == this);
    cleanupEventLoop();
    cleanupConsole();
    cleanupOpenSSL();
    resources::destroyResources();
    if(mConsoleSink)
        mConsoleSink->shutdownMilestones();
    if (mLogFileSink) {
        LogSink::remove(mLogFileSink);
        mLogFileSink.reset();
    }
    mLogCatcher.reset();
#ifdef NRDP_HAS_OBJECTCOUNT
    ObjectCount::dump(ObjectCount::Shutdown|ObjectCount::ShowObjects);
#endif
    cleanupConsoleSink();
    sInstance = 0;
}

bool Application::start()
{
    assert(isAppThread());
    if(!Configuration::isValid())
        return false;
    if(!initTraceAreas())
        return false;
    mConsole = createConsole();

    mEventLoop = createEventLoop();
    if(!mEventLoop)
        return false;

    return true;
}

void Application::setObjectCountTimerInterval(int interval)
{
    shared_ptr<ObjectCountTimer> start, stop;
    {
        ScopedMutex lock(mMutex);
        stop = mObjectCountTimer;
        if(!interval) {
            mObjectCountTimer.reset();
        } else {
            mObjectCountTimer.reset(new ObjectCountTimer(interval));
            start = mObjectCountTimer;
        }
    }
    if(stop)
        stop->stop();
    if(start)
        start->start();
}

bool Application::initTraceAreas()
{
    if(Configuration::traceAreas().empty())
        return true;

    if(Configuration::traceAreas() == "list") {
        printf("Traceareas:\n");
        const std::vector<const TraceArea*> &allAreas = TraceArea::getAllAreas();
        for(std::vector<const TraceArea*>::const_iterator it = allAreas.begin(); it != allAreas.end(); ++it) {
            if((*it))
                printf("  %s\n", (*it)->getName().c_str());
        }
        printf("Tracearea Groups:\n");
        const std::vector<std::string> &allGroups = TraceArea::getAllGroups();
        for(std::vector<std::string>::const_iterator git = allGroups.begin(); git != allGroups.end(); ++git) {
            const std::vector<const TraceArea*> &allAreas = TraceArea::getAllAreas(*git);
            if(allAreas.empty())
                continue;
            printf("  %s: ", git->c_str());
            for(std::vector<const TraceArea*>::const_iterator it = allAreas.begin(); it != allAreas.end(); ++it) {
                if(it != allAreas.begin())
                    printf(",");
                if((*it))
                    printf("%s", (*it)->getName().c_str());
            }
            printf("\n");
        }
        return false;
    }

    traceAreas()->parse(Configuration::traceAreas());

    return true;
}

Variant Application::versionInfo() const
{
    std::ostringstream os;
    os << "NRDP " << NRDP_VERSION_STRING << " - Built " __TIME__ << ' ' << __DATE__;
    Variant map;
#ifdef BUILD_DEBUG
    map["debug"] = true;
#else
    map["debug"] = false;
#endif
#ifdef BUILD_NUMBER
    map["build"] = BUILD_NUMBER;
#endif
#ifdef BUILD_BRANCH
    map["branch"] = BUILD_BRANCH;
#endif
#ifdef BUILD_PRODUCTION
    map["production"] = true;
#else
    map["production"] = false;
#endif
#ifdef NRDP_HAS_TRACING
    map["tracing"] = true;
#else
    map["tracing"] = false;
#endif
    map["string"] = os.str();
    // Deprecated!
    map["version"] = NRDP_VERSION_NUMBER;
    map["versionString"] = NRDP_VERSION_STRING;
#ifdef NRDP_VERSION_SUFFIX
    map["versionSuffix"] = NRDP_VERSION_SUFFIX;
#endif
    map["compiler"]["cflags"] = NRDP_CFLAGS;
    map["compiler"]["lflags"] = NRDP_LFLAGS;
    map["compiler"]["cxxflags"] = NRDP_CXXFLAGS;

#if defined(__UCLIBC__)
    std::ostringstream uclibc;
    uclibc << __UCLIBC_MAJOR__ << "." << __UCLIBC_MINOR__ << "." << __UCLIBC_SUBLEVEL__;
    map["compiler"]["libc"]["type"] = "uclibc";
    map["compiler"]["libc"]["version"] = uclibc.str();
#elif defined(__GLIBC__)
    std::ostringstream glibc;
    glibc << __GLIBC__ << "." << __GLIBC_MINOR__;
    map["compiler"]["libc"]["type"] = "glibc";
    map["compiler"]["libc"]["version"] = glibc.str();
#endif

#if defined(__GNUC__)
    map["compiler"]["type"] = "gcc";
    map["compiler"]["version"] = __VERSION__;
#endif
#if defined(_MSC_VER)
    map["compiler"]["type"] = "msvc";
    map["compiler"]["version"] = _MSC_VER;
#endif
#if defined(__clang__)
    map["compiler"]["type"] = "clang";
    map["compiler"]["version"] = __clang_version__;
#endif
    map["buildTime"] = __TIME__;
    map["buildDate"] = __DATE__;

    return map;
}

bool Application::initOpenSSL()
{
    const DataBuffer random = generateSecret(OpenSSLLib::MIN_SEED_LEN);
    if (random.isEmpty())
        return false;

    netflix::NFErr err = OpenSSLLib::init(random);
    if (err != netflix::NFErr_OK) {
        Log::warn(TRACE_STARTUP, "OpenSSLLib::init() failed err [%s]",
                  err.toString().c_str());
        return false;
    }
    return true;
}

void Application::cleanupOpenSSL()
{
    OpenSSLLib::cleanup();
}

DataBuffer Application::generateSecret(size_t len)
{
    DataBuffer ret;
    ret.resize(len);
    NRDP_OBJECTCOUNT_DESCRIBE(ret, "GENERATESECRET");
    {
        const std::string randomLocation = "/dev/urandom";
        if (FILE *file = fopen(randomLocation.c_str(), "r")) {
            if (!fread(reinterpret_cast<char*> (&ret[0]), len, 1, file)) {
                Log::error(TRACE_STARTUP, "Failed to read %zu bytes from %s",
                           len, randomLocation.c_str());
                ret.clear();
                fclose(file);
            }
            fclose(file);
        } else {
            Log::error(TRACE_STARTUP, "Failed to open %s!", randomLocation.c_str());
        }
    }
    return ret;
}

void Application::executed(const std::string &msg)
{
    Log::warn(TRACE_EXECUTED, ~Log::Message::SendToFilters, "Executed: %s", msg.c_str());
}


class RemoveLogFiles : public ReadDir
{
public:
    RemoveLogFiles(const std::string &logFile)
        : mLogFile(logFile)
    {
        unlink(logFile.c_str());
    }
    virtual Result visitEntry(const Entry &file)
    {
        if (file.type == File && contains(file.absolutePath, mLogFile))
            unlink(file.absolutePath.c_str());

        return Continue;
    }
    const std::string mLogFile;
};

std::string Application::logfile() const
{
    if (mLogFileSink)
        return mLogFileSink->fileName();
    return std::string();
}

bool Application::createLogFileSink(const std::string &logFile)
{
    if (mLogFileSink) {
        LogSink::remove(mLogFileSink);
        mLogFileSink.reset();
    }
    if (logFile.empty())
        return true;
    size_t lastSlash = logFile.rfind('/');
    if (lastSlash != std::string::npos) {
        RemoveLogFiles remove(logFile);
        remove.visit(logFile.substr(0, lastSlash + 1));
    }
    shared_ptr<LogFileSink> logFileSink(new LogFileSink);
    if (logFileSink->open(logFile)) {
        mLogFileSink = logFileSink;
        return true;
    }
    return false;
}

bool Application::openLogFile(const std::string &logFile)
{
    if (createLogFileSink(logFile) && mLogFileSink) {
        LogSink::add(mLogFileSink);
        return true;
    }
    return false;
}

void Application::initLogging()
{
    mConsoleSink = createConsoleSink();
    if (mConsoleSink)
        mConsoleSink->setLogLevel(Configuration::consoleSinkLogLevel());

    const bool logFileError = !createLogFileSink(Configuration::logFile());
    std::vector<shared_ptr<LogSink> > sinks;
    std::vector<Log::Message> messages;
    {
        ScopedMutex lock(LogSink::mutex());
        if (mLogCatcher) {
            messages = mLogCatcher->finish();
            LogSink::remove(mLogCatcher);
            mLogCatcher.reset();
        }
        sinks = logSinks();
        for (std::vector<shared_ptr<LogSink> >::const_iterator it = sinks.begin(); it != sinks.end(); ++it) {
            LogSink::add(*it);
        }
    }

    for (std::vector<Log::Message>::iterator it = messages.begin(); it != messages.end(); ++it) {
        it->send();
    }
    for (std::vector<shared_ptr<LogSink> >::const_iterator it = sinks.begin(); it != sinks.end(); ++it) {
        (*it)->welcome();
    }
    if (logFileError)
        Log::error(TRACE_LOG, "Can't open %s for writing", Configuration::logFile().c_str());
}

std::vector<shared_ptr<LogSink> > Application::logSinks() const
{
    assert(LogSink::mutex().isLocked());
    std::vector<shared_ptr<LogSink> > sinks;
    if (mConsoleSink)
        sinks.push_back(mConsoleSink);
    if (mLogFileSink)
        sinks.push_back(mLogFileSink);
    return sinks;
}
