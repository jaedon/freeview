/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#include "Log.h"

#include <assert.h>
#include <set>
#include <iomanip>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

#include <nrdbase/tr1.h>

#include <nrdbase/Time.h>
#include <nrdbase/Thread.h>
#include <nrdbase/LogSink.h>
#include <nrdbase/TraceAreas.h>
#include <nrdbase/ScopedMutex.h>
#include <nrdbase/StringUtils.h>
#include <nrdbase/ObjectCount.h>
#include <nrdbase/StringTokenizer.h>
#include <nrdbase/Application.h>

#include <nrdbase/PerformanceMarker.h>

using namespace netflix;

struct LogMessageCount : public netflix::ObjectCount::Record
{
public:
    inline LogMessageCount(const std::string &n) : netflix::ObjectCount::Record(n) { }
    inline virtual ~LogMessageCount() { }
    virtual std::string describe(void *object) const { return ConsoleSink::format(*static_cast<Log::Message*>(object)); }
};

NRDP_DEFINE_OBJECTCOUNT_RECORD(LogMessageCount, LogMessage);

//////////
//
// TraceArea
//
//////////
static std::map<std::string, const TraceArea *> *s_traceAreas = 0;
TraceArea::TraceArea(const char *n, const char *g) : name(n)
{
    // make sure we're not already in the list
#if defined(BUILD_DEBUG)
    assert(!TraceArea::find(n));
#endif
    if (g && *g && *g != '0')
    {
        std::vector<std::string> tokens;
        StringTokenizer::tokenize(g, tokens, "|");
        for (std::vector<std::string>::const_iterator tok = tokens.begin();
             tok != tokens.end(); ++tok)
            groups.push_back(StringTokenizer::trim(*tok));
    }
    if (!s_traceAreas)
        s_traceAreas = new std::map<std::string, const TraceArea *>;
    (*s_traceAreas)[n] = this;
}

TraceArea::~TraceArea()
{
    assert(s_traceAreas);
    std::map<std::string, const TraceArea*>::iterator it = s_traceAreas->find(name);
    // make sure we found ourselves in the list
    assert(it != (*s_traceAreas).end());
    s_traceAreas->erase(it);
    if (s_traceAreas->empty())
    {
        delete s_traceAreas;
        s_traceAreas = 0;
    }
}

const std::vector<std::string> TraceArea::getAllGroups()
{
    std::set<std::string> groups;
    if (s_traceAreas)
    {
        for (std::map<std::string, const TraceArea *>::const_iterator it = s_traceAreas->begin();
             it != s_traceAreas->end(); it++)
        {
            const std::vector<std::string> &g = it->second->getGroups();
            for (std::vector<std::string>::const_iterator git = g.begin();
                 git != g.end(); ++git)
            {
                groups.insert(*git);
            }
        }
    }
    return std::vector<std::string>(groups.begin(), groups.end());
}

const std::vector<const TraceArea*> TraceArea::getAllAreas(const std::string &group)
{
    std::vector<const TraceArea*> v;
    if(!s_traceAreas)
        return v;

    for (std::map<std::string, const TraceArea *>::const_iterator it = s_traceAreas->begin();
         it != s_traceAreas->end(); it++)
    {
        if (group.empty())
        {
            v.push_back(it->second);
        }
        else
        {
            const std::vector<std::string> &g = it->second->getGroups();
            for(std::vector<std::string>::const_iterator git = g.begin(); git != g.end();
                ++git)
            {
                if (*git == group)
                    v.push_back(it->second);
            }
        }
    }
    return v;
}

const TraceArea *TraceArea::find(const std::string &area)
{
    if(!s_traceAreas)
        return 0;
    if(startsWith(area, "TRACE_"))
        return find(area.substr(6));
    std::map<std::string, const TraceArea*>::const_iterator it = s_traceAreas->find(area);
    return (it == s_traceAreas->end()) ? 0 : it->second;
}

DEFINE_TRACEAREA(CONFIGDATA, 0);
DEFINE_TRACEAREA(CRYPTO, security);
DEFINE_TRACEAREA(CURL_MULTI_THREAD, http);
DEFINE_TRACEAREA(DISK_CACHE, network);
DEFINE_TRACEAREA(DISK_STORE, 0);
DEFINE_TRACEAREA(DNS_MANAGER, dns|network);
DEFINE_TRACEAREA(DRMSYSTEM, 0);
DEFINE_TRACEAREA(EVENTCONNECTION, nbp|nbpd);
DEFINE_TRACEAREA(EVENTLOOP, base);
DEFINE_TRACEAREA(EXECUTED, 0);
DEFINE_TRACEAREA(HTTPLIB, http);
DEFINE_TRACEAREA(HTTP_SERVICE_THREAD, http);
DEFINE_TRACEAREA(INSTRUMENTATION,instrumentation);
DEFINE_TRACEAREA(IP_CONNECTIVITY_MANAGER, network);
DEFINE_TRACEAREA(JSONPARSER, base);
DEFINE_TRACEAREA(LOG, log);
DEFINE_TRACEAREA(MEDIACONTROL, media|playstart);
DEFINE_TRACEAREA(MEDIAFILECACHE, media|playstart);
DEFINE_TRACEAREA(MEDIALISTENER, media|playstart);
DEFINE_TRACEAREA(MEDIAPLAYBACK, dpi|media);
DEFINE_TRACEAREA(MEDIAREQUESTBRIDGE, media);
DEFINE_TRACEAREA(MEDIASTARTUP, media);
DEFINE_TRACEAREA(MEMORY, 0);
DEFINE_TRACEAREA(MILESTONE, log);
DEFINE_TRACEAREA(MSPBRIDGE, media);
DEFINE_TRACEAREA(NBP, nbp);
DEFINE_TRACEAREA(NETWORK, network);
DEFINE_TRACEAREA(NRDP_SCRIPT, ui|gibbon|playstart);
DEFINE_TRACEAREA(PLAYBACK_REPORTER, media|log);
DEFINE_TRACEAREA(RESOURCEMANAGER, network);
DEFINE_TRACEAREA(RESOURCES, ui|nbpd);
DEFINE_TRACEAREA(SECURE_STORE, 0);
DEFINE_TRACEAREA(STARTUP, 0);
DEFINE_TRACEAREA(STREAMERSLOG, media);
DEFINE_TRACEAREA(STREAMINGMANAGER, media);
DEFINE_TRACEAREA(STREAMINGSTAT, media);
DEFINE_TRACEAREA(SUSPEND, 0);
DEFINE_TRACEAREA(SYSTEM, 0);
DEFINE_TRACEAREA(THREAD, base);
DEFINE_TRACEAREA(THREADPOOL, base);
DEFINE_TRACEAREA(TRACE, log);
DEFINE_TRACEAREA(UI_SCRIPT, ui|gibbon|playstart);
DEFINE_TRACEAREA(UTIL, 0);
DEFINE_TRACEAREA(VARIANT, base);
DEFINE_TRACEAREA(WRITELIMITER, 0);

namespace netflix {
namespace Log {

//////////
//
// LogLevel
//
//////////
const char *logLevelToString(Log::LogLevel level)
{
    switch (level)
    {
        case Log::Console:
            return "";
        case Log::Trace:
            return "trace";
        case Log::Debug:
            return "debug";
        case Log::Info:
            return "info";
        case Log::Warn:
            return "warn";
        case Log::Error:
            return "error";
        case Log::Fatal:
            return "fatal";
    }

    return "";
}

//////////
//
// hexDump
//
//////////
std::string hexDump(const unsigned char *data, int dataLen)
{
    std::ostringstream result;
    const int hexLen = 32;
    result << std::setfill('0');
    for(int i = 0; i < dataLen; i += hexLen) {
        result << std::setw(4) << std::dec << i << ")";
        result << std::hex;
        for(int j = 0; j < hexLen; ++j) {
            if(!(j % 8))
                result << " ";
            if (i + j < dataLen) {
                result << " " << std::setw(2) << int(*(data+i+j));
            } else {
                result << "   ";
            }
        }
        result << "  |";
        for(int j = 0; j <hexLen; ++j) {
            if (i + j < dataLen) {
                result << char(!isprint(data[i+j]) && !isspace(data[i+j]) ? '.' : *(data+i+j));
            } else {
                result << "  ";
            }
        }
        result << "|" << std::endl;
    }
    return result.str();
}

//////////
//
// Message
//
//////////
Message::Message()
    : m_time(Time::serverTime()),
      m_monoTime(Time::mono()),
      m_baseMonoTime(0),
      m_traceArea(0),
      m_logLevel(Trace),
      m_threadName(Thread::CurrentThread() ? Thread::CurrentThreadName() : std::string()),
      m_threadId(Thread::CurrentThreadPtr()),
      m_flags(DefaultFlags)
{
    NRDP_OBJECTCOUNT_REF(LogMessage, this);
}

Message::Message(const TraceArea &area, LogLevel level, const std::string &type)
    : m_time(Time::serverTime()),
      m_monoTime(Time::mono()),
      m_baseMonoTime(0),
      m_traceArea(&area),
      m_logLevel(level),
      m_type(type),
      m_threadName(Thread::CurrentThread() ? Thread::CurrentThreadName() : std::string()),
      m_threadId(Thread::CurrentThreadPtr()),
      m_flags(DefaultFlags)
{
    NRDP_OBJECTCOUNT_REF(LogMessage, this);
}

Message::Message(const Message &msg)
{
    *this = msg;
    NRDP_OBJECTCOUNT_REF(LogMessage, this);
}

Message::~Message()
{
    NRDP_OBJECTCOUNT_DEREF(LogMessage, this);
}

void Message::setTag(const std::string &tagName, const std::string &value)
{
    if (!value.empty())
        m_tags[tagName] = value;
}

void Message::send()
{
    PERFORMANCE_MARKER_SCOPED("log.message.send");

    static std::string s_appidstr("appid");

    // append the xid to the non-debug message if not already there
    while (!m_message.empty() && m_message[m_message.size()-1] == '\n')
        m_message.resize(m_message.size()-1);
    if (!m_type.empty())
    {
        if ((m_tags.find(s_appidstr) == m_tags.end()) && appId())
            setTag(s_appidstr, appId());
    }
    LogSink::send(*this);
}

uint32_t Message::size() const
{
    // start with message str bytes
    uint32_t sz = m_message.size();

    // add in tags
    std::map<std::string, std::string>::const_iterator it;
    for (it = m_tags.begin(); it != m_tags.end(); ++it)
        sz += it->first.size() + it->second.size();

    return sz;
}

Time Message::getBaseMonoTime() const
{
    if(!m_baseMonoTime.ms())
        return Application::MONO_STARTED;
    return m_baseMonoTime;
}

std::string Message::toString() const
{
    // get fields
    std::string severity(logLevelToString(m_logLevel));

    // header
    std::string msg_str = StringFormatter::sformat(
        "%llu type=%s,sev=%s",
        m_time.seconds(),
        m_type.c_str(),
        severity.c_str());

    // message if there
    if (!m_message.empty())
        msg_str += ",msg=" + m_message;

    // other tags
    const std::map<std::string, std::string>::const_iterator e = m_tags.end();
    for (std::map<std::string, std::string>::const_iterator t = m_tags.begin();
         t != e; ++t)
    {
        msg_str += ',' + (*t).first + "=" + (*t).second;
    }

    return msg_str;
}

//////////
//
// stream
//
//////////

stream::stream(const TraceArea &area, LogLevel level, const std::string &type, uint8_t flags)
    : mTraceArea(&area), mLogLevel(level), mType(type), mMessage(0), mFlags(flags)
{
}

stream::stream(const stream &other)
    : mTraceArea(other.mTraceArea), mLogLevel(other.mLogLevel), mType(other.mType), mMessage(other.mMessage),
      mStream(other.mStream.str(),std::ios::ate), mFlags(other.mFlags)
{
    other.mMessage=0;
}

stream::~stream()
{
    flush();
}

stream & stream::operator << (const stream::tag & t)
{
    init();
    mMessage->setTag(t.name , t.value);
    return *this;
}

stream & stream::operator << (const type & t)
{
    mType = t.name;
    if ( mMessage )
    {
        mMessage->m_type=mType;
    }
    return *this;
}


stream & stream::flush()
{
    if (mMessage)
    {
        fill();
        mMessage->send();
        delete mMessage;
        mMessage = 0;
    }
    return *this;
}

void stream::init()
{
    if (!mMessage)
    {
        mMessage=new Message(*mTraceArea, mLogLevel, mType);
        mMessage->m_flags = mFlags;
        mStream.str(std::string());
    }
}

std::ostream & operator << (std::ostream & o, const Log::stream & s)
{
    if (s.mMessage)
    {
        s.mMessage->m_message=s.mStream.str();
        o << s.mMessage->toString();
    }
    return o;
}

//////////
//
// stream manipulators
//
//////////

stream & flush(stream & s)
{
    return s.flush();
}

stream::tag tag(const std::string & name, const std::string & value)
{
    stream::tag t;
    t.name = name;
    t.value = value;
    return t;
}

stream::tag tag(const std::string & name, long long value)
{
    stream::tag t;
    t.name = name;
    if (value!=-1)
    {
        std::ostringstream s;
        s << value;
        t.value = s.str();
    }
    return t;
}

stream::tag tag(const std::string & name, float value)
{
    stream::tag t;
    t.name = name;
    std::ostringstream s;
    s << value;
    t.value = s.str();
    return t;
}

stream::tag tag(const std::string & name, double value)
{
    stream::tag t;
    t.name = name;
    std::ostringstream s;
    s << value;
    t.value = s.str();
    return t;
}

stream::tag tag(const std::string & name, void * value)
{
    stream::tag t;
    t.name = name;
    std::ostringstream s;
    s << std::hex << value;
    t.value = s.str();
    return t;
}

stream::tag tag(const std::string & name, int value)
{
    stream::tag t;
    t.name = name;
    std::ostringstream s;
    s << value;
    t.value = s.str();
    return t;
}


stream::type type(const std::string & name)
{
    stream::type t;
    t.name = name;
    return t;
}

void printf(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    ::vfprintf(stdout, fmt, args);
    ::fflush(stdout);
    va_end(args);
}

//////////
//
// xid,appid,sessionid
//
//////////
static ullong s_xid = 0;
ullong xid() { return s_xid; }
void setXid(ullong xid) { s_xid = xid; }

static ullong s_appid = 0;
ullong appId() { return s_appid; }
void setAppId(ullong appid) { s_appid = appid; }

static ullong s_sessionid = 0;
ullong sessionId() { return s_sessionid; }
void setSessionId(ullong sessionid) { s_sessionid = sessionid; }

}} // namespace Log
