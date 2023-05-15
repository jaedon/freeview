/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef __LOG_SINK_H__
#define __LOG_SINK_H__

#include <nrdbase/tr1.h>

#include <assert.h>
#include <vector>
#include <string>
#include <pthread.h>
#include <nrdbase/Mutex.h>
#include <nrdbase/ScopedMutex.h>
#include <nrdbase/PerformanceMarker.h>


namespace netflix {

namespace Log {
class Message;
}

// subclass from this to receive log messages
class LogSink
{
public:
    // subclasses should add and remove themselves
    static Mutex &mutex() { return sSinksMutex; }
    static void add(const shared_ptr<LogSink> &sink);
    static void remove(const shared_ptr<LogSink> &sink);

    LogSink() {}
    virtual ~LogSink() {}

    void welcome();
    virtual void receive(const Log::Message &) {}
private:
    friend class Log::Message;
    static void send(const Log::Message &msg);
    static Mutex sSinksMutex;
};

class LogCatcher : public LogSink, public enable_shared_from_this<LogCatcher>
{
public:
    LogCatcher(size_t max)
        : mMax(max)
    {}

    virtual void receive(const Log::Message &msg);
    std::vector<Log::Message> finish()
    {
        assert(LogSink::mutex().isLocked());
        std::vector<Log::Message> messages;
        std::swap(mMessages, messages);
        return messages;
    }
private:
    const size_t mMax;
    std::vector<Log::Message> mMessages;
};

template <typename T>
class LogSinkForwarder : public LogSink
{
public:
    LogSinkForwarder(const shared_ptr<T> &owner) : m_owner(owner) {}

    virtual void receive(const Log::Message &msg)
    {
        PERFORMANCE_MARKER_SCOPED("log.logsinkforwarder.receive");
        if (shared_ptr<T> owner = m_owner.lock())
            owner->receive(msg);
    }

private:
    weak_ptr<T> m_owner;
};
}

#endif // __LOG_SINK_H__
