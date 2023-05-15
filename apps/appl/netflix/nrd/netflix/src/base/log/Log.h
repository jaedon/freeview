/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef Log_h
#define Log_h

#include <string>
#include <stdio.h>
#include <sstream>
#include <map>
#include <vector>
#include <stdarg.h>

#include <nrdbase/config.h>

#include <nrdbase/Time.h>
#include <nrdbase/Compiler.h>
#include <nrdbase/Exportable.h>
#include <nrdbase/DataBuffer.h>
#include <nrdbase/StringFormatter.h>
#include <nrdbase/Noncopyable.h>

struct TraceArea : public netflix::Noncopyable
{
public:
    inline const std::string &getName() const { return name; } //all-caps
    inline const std::vector<std::string> &getGroups() const { return groups; } //lowercase

    static const std::vector<std::string> getAllGroups();
    static const std::vector<const TraceArea*> getAllAreas(const std::string &group="");

    static const TraceArea *find(const std::string &area);

    bool operator==(const TraceArea &other) const { return other.name == name; }
    bool operator!=(const TraceArea &other) const { return other.name != name; }

    TraceArea(const char *name, const char *groups);
    ~TraceArea();

private:
    std::string name;
    std::vector<std::string> groups;
};
#define DEFINE_TRACEAREA(name, group) NRDP_EXPORTABLE TraceArea TRACE_ ## name(#name, #group)
#define DECLARE_TRACEAREA(name) extern TraceArea TRACE_ ## name

DECLARE_TRACEAREA(CONFIGDATA);
DECLARE_TRACEAREA(CRYPTO);
DECLARE_TRACEAREA(CURL_MULTI_THREAD);
DECLARE_TRACEAREA(DISK_CACHE);
DECLARE_TRACEAREA(DISK_STORE);
DECLARE_TRACEAREA(DNS_MANAGER);
DECLARE_TRACEAREA(DRMSYSTEM);
DECLARE_TRACEAREA(EVENTCONNECTION);
DECLARE_TRACEAREA(EVENTLOOP);
DECLARE_TRACEAREA(EXECUTED);
DECLARE_TRACEAREA(HTTPLIB);
DECLARE_TRACEAREA(HTTP_SERVICE_THREAD);
DECLARE_TRACEAREA(INSTRUMENTATION);
DECLARE_TRACEAREA(IP_CONNECTIVITY_MANAGER);
DECLARE_TRACEAREA(JSONPARSER);
DECLARE_TRACEAREA(LOG);
DECLARE_TRACEAREA(MEDIACONTROL);
DECLARE_TRACEAREA(MEDIAFILECACHE);
DECLARE_TRACEAREA(MEDIALISTENER);
DECLARE_TRACEAREA(MEDIAPLAYBACK);
DECLARE_TRACEAREA(MEDIAREQUESTBRIDGE);
DECLARE_TRACEAREA(MEDIASTARTUP);
DECLARE_TRACEAREA(MEMORY);
DECLARE_TRACEAREA(MILESTONE);
DECLARE_TRACEAREA(MSPBRIDGE);
DECLARE_TRACEAREA(NBP);
DECLARE_TRACEAREA(NETWORK);
DECLARE_TRACEAREA(NRDP_SCRIPT);
DECLARE_TRACEAREA(PLAYBACK_REPORTER);
DECLARE_TRACEAREA(RESOURCEMANAGER);
DECLARE_TRACEAREA(RESOURCES);
DECLARE_TRACEAREA(SECURE_STORE);
DECLARE_TRACEAREA(STARTUP);
DECLARE_TRACEAREA(STREAMERSLOG);
DECLARE_TRACEAREA(STREAMINGMANAGER);
DECLARE_TRACEAREA(STREAMINGSTAT);
DECLARE_TRACEAREA(SUSPEND);
DECLARE_TRACEAREA(SYSTEM);
DECLARE_TRACEAREA(THREAD);
DECLARE_TRACEAREA(THREADPOOL);
DECLARE_TRACEAREA(TRACE);
DECLARE_TRACEAREA(UI_SCRIPT);
DECLARE_TRACEAREA(UTIL);
DECLARE_TRACEAREA(VARIANT);
DECLARE_TRACEAREA(WRITELIMITER);

#if defined(NRDP_HAS_TRACING)
# define NTRACE(a, ...) netflix::Log::trace(a, __VA_ARGS__)
# define NTRACEIF(condition, a, ...) if(condition) NTRACE(a, __VA_ARGS__)
# define NTRACES(a,s) netflix::Log::stream(a,Log::Trace) << s
# define PERFTRACE(...) // netflix::Log::printf(__VA_ARGS__)
#else
# define NTRACE(a, ...)
# define NTRACEIF(condition, a, ...)
# define NTRACES(a,s)
// remove me for the final release build
# define PERFTRACE(...)
#endif

namespace netflix {
namespace Log {

/*
 * These match the severities in the NCCP spec.
 */
enum LogLevel {
    // 0 is reserved -- do not use
    Console    = -1, // Messages never sent to the server
    Trace      = 10, // Messages that are useful only during
                     // development.
    Debug      = 20, // For collecting information about the
                     // units in the field, such as network
                     // statistics and stack usage.
    Info       = 30, // Informational messages (successful
                     // operations, etc.)
    Warn       = 40, // Information about an unexpected failure, where
                     // customer experience is not negatively affected
    Error      = 50, // Non-fatal, recoverable problematic
                     // events (no loss of functionality).
                     // Should also be used to log caught
                     // exceptions.
    Fatal      = 60  // Non-recoverable errors leading to loss
                     // of functionality that could affect all
                     // units. Anything that's logged FATAL
                     // should set alarm bells ringing.
};

const char *logLevelToString(Log::LogLevel level);
std::string hexDump(const unsigned char *s, int len);
inline std::string hexDump(const DataBuffer &buffer) { return hexDump(buffer.data(), buffer.size()); }
inline std::string hexDump(const std::string &s) { return hexDump(reinterpret_cast<const unsigned char*>(s.c_str()), s.size()); }

class Message
{
public:
    Message();
    Message(const Message &msg);
    Message(const TraceArea &area, LogLevel level,
            const std::string &type = std::string());
    ~Message();

    void setTag(const std::string &tagName, const std::string &value);
    inline void setTag(const std::string &tagName, long long value)
    {
        if (value != -1) {
            std::ostringstream str;
            str << value;
            setTag(tagName, str.str());
        }
    }
    void send();

    // returns the size in bytes of this message
    uint32_t size() const;

    std::string toString() const;

    // return the real time this message was sent (offset from monotonic clock)
    inline Time getTime() const { return (Time::serverTime() - (Time::mono() - m_monoTime)); }
    // return the mono for when the application started for this message
    Time getBaseMonoTime() const;

    Time m_time;      // Time::now() at message creation
    Time m_monoTime;  // Time::mono() at message creation
    Time m_baseMonoTime;  // Application::MONO_STARTED
    const TraceArea *m_traceArea;
    LogLevel m_logLevel;
    std::string m_message;
    std::map<std::string, std::string> m_tags;
    std::string m_type;
    std::string m_threadName;
    void *m_threadId;
    std::string m_jsStack; // if the message came from JS, via gibbon
    enum {
        Critical      = 0x01,
        SendToAppBoot = 0x02,
        SendToJS      = 0x04,
        SendToFilters = 0x08,
        SendToIchnaea = 0x10,
        DefaultFlags  = SendToFilters|SendToJS
    };
    uint8_t m_flags;
};

class stream
{
public:

    stream(const TraceArea &area,
           LogLevel level,
           const std::string & type=std::string(),
           uint8_t flags=Message::DefaultFlags);
    stream(const stream &other);

    // If the stream has a message, it is sent when the stream is destroyed. That is,
    // the destructor calls flush.
    virtual ~stream();

    // Catch-all
    template <typename T>
    inline stream &operator <<(const T &t) { init(); mStream << t; return *this; }

    // For STL manipulators that don't take an argument (like std::endl)
    inline stream &operator <<(std::ostream &(*f)(std::ostream &)) { init(); f(mStream); return *this; }

    // For our own manipulators that don't take an argument (like flush)
    inline stream &operator <<(stream &(*f)(stream &)) { return f(*this); }

    // For the 'tag' manipulator
    struct tag { std::string name; std::string value; };
    stream &operator << (const tag &t);

    // For the 'type' manipulator
    struct type { std::string name; };
    stream &operator << (const type &t);

    // Sends the underlying message. This discards all tags and the message text.
    stream &flush();

protected:
    void init();

    inline Message *takeMessage() { Message *result = mMessage; mMessage = 0; return result; }
    inline Message *getMessage() { return mMessage; }

    inline void fill() { if (mMessage) mMessage->m_message=mStream.str(); }

private:
    friend std::ostream & operator << (std::ostream & , const stream &);

    const TraceArea *   mTraceArea;
    LogLevel            mLogLevel;
    std::string         mType;
    mutable Message *   mMessage;
    std::ostringstream  mStream;
    uint8_t             mFlags;
};

stream &flush(stream &s);
stream::tag tag(const std::string &name, const std::string &value);
stream::tag tag(const std::string &name, long long value);
stream::tag tag(const std::string &name, float value);
stream::tag tag(const std::string &name, void *value);
stream::tag tag(const std::string &name, double value);
stream::tag tag(const std::string &name, int value);
stream::type type(const std::string &name);

// This one lets us stream the stream to another stream using Message::toString
std::ostream & operator << (std::ostream & o, const Log::stream & s);

// get/set the xid,appid,sessionid that will be added to Message's
ullong xid();
void setXid(ullong xid);
ullong appId();
void setAppId(ullong appid);
ullong sessionId();
void setSessionId(ullong sessionid);

NRDP_EXPORTABLE void printf(const char *fmt, ...) _NETFLIX_PRINTF_ATTR(1,2);
inline void log(const TraceArea &area, LogLevel level, uint8_t flags, const char *fmt, va_list args)
{
    Log::Message msg(area, level);
    msg.m_flags = flags;
    msg.m_message = StringFormatter::vsformat(fmt, args);
    va_end(args);
    msg.send();
}
NRDP_EXPORTABLE void log(const TraceArea &area, LogLevel level, uint8_t flags, const char *fmt, ...) _NETFLIX_PRINTF_ATTR(4,5);
inline void log(const TraceArea &area, LogLevel level, uint8_t flags, const char *fmt, ...)
{ va_list args; va_start(args, fmt); log(area, level, flags, fmt, args); }

NRDP_EXPORTABLE void milestone(const char *fmt, ...) _NETFLIX_PRINTF_ATTR(1,2);
inline void milestone(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    Log::Message msg(TRACE_MILESTONE, Info, "milestone");
    msg.m_flags = Message::Critical| Message::SendToAppBoot | Message::SendToJS | Message::SendToIchnaea;
    msg.m_message = StringFormatter::vsformat(fmt, args);
    va_end(args);
    msg.send();
}
inline stream milestone()    \
{ return stream(TRACE_MILESTONE, Info, "milestone", Message::Critical | Message::SendToAppBoot | Message::SendToJS | Message::SendToIchnaea); }

#define MAKE_LOG_LEVEL_FUNCTIONS(name, level) \
    NRDP_EXPORTABLE void name(const TraceArea &area, uint8_t flags, const char *fmt, ...) _NETFLIX_PRINTF_ATTR(3,4); \
    inline void name(const TraceArea &area, uint8_t flags, const char *fmt, ...) \
    { va_list args; va_start(args, fmt); log(area, level, flags, fmt, args); } \
    NRDP_EXPORTABLE void name(const TraceArea &area, const char *fmt, ...) _NETFLIX_PRINTF_ATTR(2,3); \
    inline void name(const TraceArea &area, const char *fmt, ...) \
    { va_list args; va_start(args, fmt); log(area, level, Log::Message::DefaultFlags, fmt, args); } \
    inline stream name(const TraceArea & area = TRACE_LOG, uint8_t flags=Log::Message::DefaultFlags) \
    { return stream(area, level, std::string(), flags); }
MAKE_LOG_LEVEL_FUNCTIONS(console, Console)
MAKE_LOG_LEVEL_FUNCTIONS(trace, Trace)
MAKE_LOG_LEVEL_FUNCTIONS(debug, Debug)
MAKE_LOG_LEVEL_FUNCTIONS(info, Info)
MAKE_LOG_LEVEL_FUNCTIONS(warn, Warn)
MAKE_LOG_LEVEL_FUNCTIONS(error, Error)
MAKE_LOG_LEVEL_FUNCTIONS(fatal, Fatal)
#undef MAKE_LOG_LEVEL_FUNCTIONS

}
}

#endif
