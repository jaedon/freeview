/*
 * (c) 1997-2015 Netflix, Inc. All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */


#include "ConsoleSink.h"

#include <assert.h>

#include <nrdbase/config.h>
#include <nrdbase/Configuration.h>
#include <nrdbase/Log.h>
#include <nrdbase/MutexRanks.h>
#include <nrdbase/ScopedMutex.h>
#include <nrdbase/StringFormatter.h>
#include <nrdbase/TraceAreas.h>
#include <nrdbase/StringTokenizer.h>
#include <nrdbase/ObjectCount.h>
#include <nrdbase/Application.h>
#include <nrdbase/StringUtils.h>
#include <nrdbase/WeakPtrSet.h>

#include <nrdbase/PerformanceMarker.h>

using namespace netflix;

ConsoleSink::ConsoleSink()
    : mMutex(CONSOLESINK_MUTEX, "ConsoleSink"), mFlags(defaultFlags()), mLogLevel(Log::Warn)
{
}

std::string ConsoleSink::format(const Log::Message &msg, uint8_t flags, const Configuration::ColorScheme &colorScheme)
{
    static const size_t MAX_HEADER_SIZE = 80;

    const bool noColor = !(flags & LogColor) || colorScheme.isNull();
    std::string msg_color, reset_color, color;
    std::ostringstream os;
    if (!noColor) {
        reset_color = Configuration::colorToEscapeCode(Configuration::Default);
        color = reset_color + Configuration::colorToEscapeCode(colorScheme.logHeader);
        switch (msg.m_logLevel) {
        case Log::Console:
        case Log::Trace:
        case Log::Debug:
            msg_color = reset_color + Configuration::colorToEscapeCode(colorScheme.trace);
            break;
        case Log::Info:
            msg_color = reset_color + Configuration::colorToEscapeCode(colorScheme.info);
            break;
        case Log::Warn:
            msg_color = reset_color + Configuration::colorToEscapeCode(colorScheme.warning);
            break;
        case Log::Error:
            msg_color = reset_color + Configuration::colorToEscapeCode(colorScheme.error);
            break;
        case Log::Fatal:
            msg_color = reset_color + Configuration::colorToEscapeCode(colorScheme.fatal);
            break;
        default:
            break;
        }
        os << color;
    }


    char header[MAX_HEADER_SIZE];
    int headerSize;
    if (flags & LogAbsoluteTime) {
        const Time now = msg.getTime();
        struct tm t;
        time_t sec = now.seconds();
        localtime_r(&sec, &t);
        headerSize = snprintf(header, MAX_HEADER_SIZE,
                              "%d-%02d-%02d %02d:%02d:%02d.%03lld",
                              t.tm_year + 1900,
                              t.tm_mon + 1,
                              t.tm_mday,
                              t.tm_hour,
                              t.tm_min,
                              t.tm_sec,
                              now.ms() % MilliSecondsPerSecond);
    } else {
        const Time now = msg.m_monoTime - msg.getBaseMonoTime();
        headerSize = snprintf(header, MAX_HEADER_SIZE,
                              "%02lld:%02lld:%02lld.%03lld",
                              now.ms() / MilliSecondsPerHour,
                              (now.ms() % MilliSecondsPerHour) / MilliSecondsPerMinute,
                              (now.ms() % MilliSecondsPerMinute) / MilliSecondsPerSecond,
                              now.ms() % MilliSecondsPerSecond);
    }
    os.write(header, headerSize);

    os << " [" << msg.m_threadName;
    if(!msg.m_threadName.empty())
        os << ":";
    os << msg.m_threadId;
    if(flags & LogProcess)
        os << ":" << getpid();
    os << "] ";

    os << (msg.m_traceArea ? msg.m_traceArea->getName() : "NONE") << "("
       << msg_color << Log::logLevelToString(msg.m_logLevel) << color
       << "):";

    if (!msg.m_type.empty())
        os << " type=" << msg.m_type;

    os << reset_color;
    os << ' ';

    std::string prefix;
    if (!msg.m_message.empty()) {
        size_t last = 0;

        while (true) {
            const size_t idx = msg.m_message.find('\n', last);
            if (!prefix.empty()) {
                os << prefix;
            } else if (idx != std::string::npos || !msg.m_tags.empty()) {
                prefix = os.str();
            }
            os << msg_color;
            if (idx == std::string::npos) {
                os.write(msg.m_message.c_str() + last, msg.m_message.size() - last);
                os << reset_color << std::endl;
                break;
            }

            os.write(msg.m_message.c_str() + last, idx - last);
            os << reset_color << std::endl;
            last = idx + 1;
        }
    }
    if (!msg.m_tags.empty()) {
        for (std::map<std::string, std::string>::const_iterator it = msg.m_tags.begin(); it != msg.m_tags.end(); ++it) {
            if (!prefix.empty()) {
                os << prefix;
            } else {
                prefix = os.str();
            }

            os << "Tag: " << msg_color << (*it).first << color << ": " << msg_color << (*it).second << reset_color << std::endl;
        }
    }
    return os.str();
}

void ConsoleSink::receive(const Log::Message &msg)
{
    PERFORMANCE_MARKER_SCOPED("log.consolesink.receive");
    ScopedMutex lock(mMutex);
    assert(msg.m_traceArea);
    if(msg.m_traceArea == &TRACE_EXECUTED)
        return;
    if (msg.m_traceArea == &TRACE_MILESTONE) {
        mMilestones.push_back(msg);
        while(mMilestones.size() > 200)
            mMilestones.pop_front();
    }

    const bool enabled = app()->traceAreas()->isEnabled(msg.m_traceArea);
    if (Configuration::traceAreasOnly() && !enabled)
        return;
    if (msg.m_logLevel >= logLevel() || enabled) {
        const Configuration::ColorScheme colorScheme = Configuration::terminalColors();
        std::string output = ConsoleSink::format(msg, mFlags, colorScheme);
        if (!output.empty()) {
            if(msg.m_flags & Log::Message::SendToFilters) {
                if(Application *a = app()) {
                    output = netflix::Console::filter(a->filters(), output, colorScheme);
                    if (output.empty())
                        return;
                }
            }
            receive(msg.m_logLevel, output);
        }
    }
}

int ConsoleSink::logLevel() const
{
    ScopedMutex lock(mMutex);
    return mLogLevel;
}

void ConsoleSink::setLogLevel(int logLevel)
{
    ScopedMutex lock(mMutex);
    mLogLevel = logLevel;
}

void ConsoleSink::receive(int, const std::string &msg)
{
    ScopedMutex lock(mMutex);
    NRDP_UNUSED_RETURN(fwrite(msg.c_str(), msg.size(), 1, stdout));
    fflush(stdout);
}

