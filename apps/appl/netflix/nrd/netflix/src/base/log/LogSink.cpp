/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "LogSink.h"

#include <nrdbase/Application.h>
#include <nrdbase/Log.h>
#include <nrdbase/ScopedMutex.h>
#include <nrdbase/WeakPtrSet.h>

using namespace netflix;

//////////
//
// statics
//
//////////

Mutex LogSink::sSinksMutex(LOG_SINKS_MUTEX, "LogSinks");
static WeakPtrSet<LogSink> sSinks;

void LogSink::add(const shared_ptr<LogSink> &sink)
{
    ScopedMutex lock(sSinksMutex);
    sSinks.insert(sink);
}

void LogSink::remove(const shared_ptr<LogSink> &sink)
{
    ScopedMutex lock(sSinksMutex);
    sSinks.erase(sink);
}

void LogSink::welcome()
{
    Log::Message welcome(TRACE_LOG, Log::Warn);
    Variant info;
    if (Application *app = Application::instance())
        info = app->versionInfo();
    info["systemTime"] = Time::systemTime().seconds();
    info["serverTime"] = Time::now().seconds();
    welcome.m_message = info.toJSON();
    receive(welcome);
}

//////////
//
// sendToAllSinks
//
//////////
void LogSink::send(const Log::Message &msg)
{
    PERFORMANCE_MARKER_SCOPED("log.logsink.send");
    std::vector<shared_ptr<LogSink> > sinks;
    {
        ScopedMutex lock(sSinksMutex);
        if (sSinks.empty()) {
#ifdef BUILD_DEBUG
            if (!msg.m_message.empty()) {
                static int logWhenNoSinks = -1;
                if (logWhenNoSinks == -1) {
                    logWhenNoSinks = 0;
                    if (const char *e = getenv("NF_LOG_STARTUP"))
                        logWhenNoSinks = atoi(e);
                }
                if (logWhenNoSinks == 1)
                    printf("%s\n", msg.m_message.c_str());
            }
#endif
            return;
        }
        sSinks.lock(sinks);
    }

    for (std::vector<shared_ptr<LogSink> >::const_iterator it = sinks.begin(); it != sinks.end(); ++it) {
        (*it)->receive(msg);
    }
}

void LogCatcher::receive(const Log::Message &msg)
{
    PERFORMANCE_MARKER_SCOPED("log.logcatcher.receive");
    ScopedMutex lock(LogSink::mutex());
    if (mMax && mMessages.size() == mMax)
        mMessages.erase(mMessages.begin());
    mMessages.push_back(msg);
    assert(!mMax || mMessages.size() <= mMax);
}
