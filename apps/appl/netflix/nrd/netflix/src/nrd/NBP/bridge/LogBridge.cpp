/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "LogBridge.h"

#include <stdlib.h>

#include <nrdbase/AseErrorCodes.h>
#include <nrdbase/Instrumentation.h>
#include <nrdbase/Log.h>
#include <nrdbase/LogSink.h>
#include <nrdbase/LexicalCast.h>
#include <nrdbase/NFErrName.h>
#include <nrdbase/TraceAreas.h>
#include <nrdbase/StringCompressor.h>
#include <nrdbase/Base64.h>

#include <nrd/Request.h>
#include <nrd/ISystem.h>

#include "NrdLog.h"
#include "NrdApplication.h"
#include "SystemData.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>

#include <fstream>
#include <sstream>
#include <string>

#include <nrdbase/config.h>

#ifdef NRDP_CRASH_REPORTING_BREAKPAD
# include <nrdbase/minidump.h>
#endif

#ifdef NRDP_CRASH_REPORTING_ENABLED
# include <nrdbase/guid_creator.h>
#endif
#include <nrdbase/ReadDir.h>

using namespace netflix;

#define CRITICAL_VECTOR_MAX_SIZE 100

#define LogPropertyList(Property)               \
    Property(UIContext, NfObject::ReadWrite)    \
    Property(appid, NfObject::ReadOnly)         \
    Property(areas, NfObject::ReadOnly)         \
    Property(aseCodes, NfObject::ReadOnly)      \
    Property(errorCodes, NfObject::ReadOnly)    \
    Property(haveTracing, NfObject::ReadOnly)   \
    Property(level, NfObject::ReadWrite)        \
    Property(levels, NfObject::ReadOnly)        \
    Property(sessionid, NfObject::ReadOnly)     \
    Property(traceAreas, NfObject::ReadWrite)

PropertyTable(LogBridge, LogPropertyList, properties)

#define MethodList(Method)                      \
    Method(createArea)                          \
    Method(deleteCriticalMessages)              \
    Method(flush)                               \
    Method(getCriticalMessages)                 \
    Method(getLogMessages)                      \
    Method(log)                                 \
    Method(milestone)                           \
    Method(resetAppID)                          \
    Method(resetSessionID)

MethodTable(LogBridge, MethodList)

/////////
// critical messages
/////////
static Mutex sCriticalErrorsMutex(netflix::ZERO_MUTEX, "CriticalErrorsMutex");

LogBridge::LogBridge()
    : NfObject("log",
               Properties::LogBridge::properties, Properties::LogBridge::PropertyCount,
               Methods::LogBridge::methods, Methods::LogBridge::MethodCount),
      mMutex(ZERO_MUTEX, "LogBridgeMutex"),
      mNextCriticalId(0),
      mLogLevel(Log::Debug),
      mWarned(false)
{
    // Boot messages
    loadCriticalErrors();

    Variant crashReport;

    std::string ui = nrdApp()->readSystemConfiguration(SystemKeys::UIVersion);
    crashReport["uiVersion"] = ui;

#if defined(NRDP_CRASH_REPORTING_BREAKPAD)
    crashReport["crashReportClient"] = "breakpad";
#elif defined(NRDP_CRASH_REPORTING_INTERNAL)
    crashReport["crashReportClient"] = "nflx1";
#elif !defined(NRDP_CRASH_REPORTING_ENABLED)
    crashReport["crashReportClient"] = "off";
#else
    crashReport["crashReportClient"] = "unknown";
#endif

#ifdef NRDP_CRASH_REPORTING_ENABLED
    GUID guid;
    char guid_str[kGUIDStringLength + 1];
    if (!CreateGUID(&guid) || !GUIDToString(&guid, guid_str, sizeof(guid_str))) {
        crashReport["guid"] = "";
    } else {
        crashReport["guid"] = guid_str;
    }
#else
    crashReport["guid"] = "";
#endif

    std::string sstr = nrdApp()->readSystemConfiguration(SystemKeys::nrdStarted);

    std::string crashCount = nrdApp()->readSystemConfiguration(SystemKeys::crashCount);
    std::istringstream iss(crashCount);
    int crashes = 0;
    iss >> std::dec >> crashes;

    if (crashes) {
        Variant report = nrdApp()->getSystem()->getReport();

        if (!report.empty()) {
            if (report["reporter"].empty()) {
                crashReport["reporter"] = "proprietary";
            }

            if (report["crashReportClient"].empty()) {
                crashReport["crashReportClient"]= "proprietary";
            }

            crashReport["crashCode"]        = report["crashCode"];
            crashReport["crashAddress"]     = report["crashAddress"];
            crashReport["reason"]           = report["reason"];
            crashReport["dump"]             = report["dump"];

            if (report["crashCode"] == SIGKILL) {
                crashReport["log"] = "The Netflix application received SIGKILL";
            } else {
                crashReport["log"] = "Partner crash report";
            }
        }
    }

#ifdef NRDP_CRASH_REPORTING_ENABLED
    // See if there is a crash report
    const std::string crashFile = Configuration::crashLogPath() + NRDP_PATH_SEPARATOR_CHAR + "crash.log";
    if (ReadDir::isFile(crashFile)) {
#ifdef NRDP_CRASH_REPORTING_BREAKPAD
        google_breakpad::Minidump minidump(crashFile);

        if (minidump.Read()) {
            google_breakpad::MinidumpException* exception = minidump.GetException();
            if (exception) {
                const MDRawExceptionStream* record = exception->exception();
                if (record) {
                    crashReport["crashCode"] = record->exception_record.exception_code;
                    crashReport["crashAddress"].stringStream() << "0x" << std::hex << record->exception_record.exception_address;

                    switch(record->exception_record.exception_address) {
                    case 0xDECEA5ED: // Test crash using the /crash command
                        crashReport["reason"] = "test_crash";
                        break;
                    case 0xBBADBEEF: // JavaScriptCore ran out of memory
                        crashReport["reason"] = "oom_javascript";
                        break;
                    case 0xDEFEC8ED: // Out of memory detected by the allocator
                        crashReport["reason"] = "oom";
                        break;
                    default:
                        crashReport["reason"] = "crash";
                        break;
                    }
                }

            }
        }
#endif
        std::ifstream in(crashFile.c_str(), std::ios::in | std::ios::ate | std::ios::binary);
        if (in && in.tellg() != (std::streampos)0) {
            in.seekg(0, std::ios::beg);
            std::ostringstream crashLog;
            crashLog << in.rdbuf();
            in.close();

            // Clear the file
            std::ofstream out(crashFile.c_str(), std::ios::trunc);
            out.close();

            if (sstr.size()) {
                crashReport["log"] = "Crash from application ID " + sstr;
            } else {
                crashReport["log"] = "The Netflix application crashed";
            }

            if (crashReport["reason"].empty()) {
                crashReport["reason"] = "crash";
            }

            // See if this is a NFLX crash report type
            std::string contents = crashLog.str();

            if (contents.substr(0, 6) == "NFLX1\n") {
                // Crash report type 1:
                // NFLX1
                // <build id, 32 digit hex number>
                // <signal number in decimal>
                // 0x<crash address in hex>
                // <freeform text>

                crashReport["reporter"] = "nflx1";

                std::stringstream ss(contents);
                std::string line;

                int lineNum = 0;
                crashReport["dump"] = contents;

                while(lineNum < 4 && std::getline(ss, line, '\n')) {
                    if (lineNum == 2) {
                        // Reparse the value to make sure we don't write unknown/binary text to the logblog
                        std::istringstream iss(line);
                        int integer = 0;
                        iss >> std::dec >> integer;
                        crashReport["crashCode"] = integer;
                    } else if (lineNum == 3) {
                        // Reparse the value to make sure we don't write unknown/binary text to the logblog
                        std::istringstream iss(line);
                        unsigned int integer = 0;
                        iss >> std::hex >> integer;
                        crashReport["crashAddress"].stringStream() << "0x" << std::hex << integer;

                        //TODO: address to reason function, so it can be reused
                        switch(integer) {
                        case 0xDECEA5ED: // Test crash using the /crash command
                            crashReport["reason"] = "test_crash";
                            break;
                        case 0xBBADBEEF: // JavaScriptCore ran out of memory
                            crashReport["reason"] = "oom_javascript";
                            break;
                        case 0xDEFEC8ED: // Out of memory detected by the allocator
                            crashReport["reason"] = "oom";
                            break;
                        default:
                            crashReport["reason"] = "crash";
                            break;
                        }
                    }
                    ++lineNum;
                }
            }

#ifdef NRDP_CRASH_REPORTING_BREAKPAD
            crashReport["dump"] = Base64::encode(crashLog.str());
            crashReport["reporter"] = "breakpad";
#else
            if (crashReport["dump"].empty())
                crashReport["dump"] = Base64::encode(crashLog.str());
            else
                crashReport["dump"] = Base64::encode(crashReport["dump"].value<std::string>());

            if (crashReport["reporter"].empty())
                crashReport["reporter"] = "unknown";
#endif
        }
    }
#endif

    // See if we have a "we have crashed" tag and the crash count
    if (Configuration::crashReportShutdowns()) {
        if (sstr.size()) {
            crashReport["appid"] = sstr;
            if (crashReport["log"].empty())
                crashReport["log"] = "Abnormal shutdown from application ID " + sstr;
            if (crashReport["reason"].empty())
                crashReport["reason"] = "abnormal_shutdown";

            std::string crashCount = nrdApp()->readSystemConfiguration(SystemKeys::crashCount);
            std::istringstream iss(crashCount);
            int crashes = 0;
            iss >> std::dec >> crashes;
            crashReport["count"] = crashes;
        }

        // Mark the boot
        nrdApp()->writeSystemConfiguration(SystemKeys::nrdStarted, lexical_cast<std::string>(Log::appId()));
    }

    if (!crashReport["log"].empty()) {
        Log::Message m(TRACE_SYSTEM, Log::Error, "crashreport");
        m.m_message = crashReport.toJSON();
        m.m_flags |= (Log::Message::Critical|Log::Message::SendToAppBoot);

        NTRACE(TRACE_STARTUP, "Crash Report: %s", m.m_message.c_str());
        mCriticalMap.insert(std::make_pair(++mNextCriticalId, m));
        NTRACES(TRACE_LOG, "Added critical crash report " << mNextCriticalId << " " << m.m_type);
    }

    NTRACE(TRACE_STARTUP, "LogBridge::LogBridge critical vector size=%zu appId=%lld", mCriticalMap.size(), Log::appId());
}

LogBridge::~LogBridge()
{
    mLogmsgs.clear();
}

void LogBridge::loadCriticalErrors()
{
    // decode the persistent value
    std::string value;
    Variant errorMsgs;

    std::vector<Log::Message> logmsgs;
    std::string sstr = nrdApp()->readSystemConfiguration(SystemKeys::criticalErrors);
    StringCompressor::inflateB64(/*out*/value, sstr);
    errorMsgs = Variant::fromJSON(value);
    // convert from variant to logmsgs;
    for(int j = 0; j < errorMsgs.size(); j++) {
        const Variant errorMsg = errorMsgs[j];
        Log::Message m;
        m.m_flags |= Log::Message::Critical;
        bool ok;
        //        int advtPeriod = request.value<int> ("advertisingPeriod", &ok);
        m.m_logLevel = (Log::LogLevel)errorMsg.mapValue<int>("level", &ok);
        if(errorMsg.mapValue<bool>("sendtoappboot"))
            m.m_flags |= Log::Message::SendToAppBoot;
        if(errorMsg.mapValue<bool>("sendtoichnaea"))
            m.m_flags |= Log::Message::SendToIchnaea;
        m.m_message = errorMsg["message"].string();
        const Variant &variantTags = errorMsg["tags"];
        const Variant::StringMapIterator end = variantTags.stringMapEnd();
        for (Variant::StringMapIterator it = variantTags.stringMapBegin(); it != end; ++it) {
            m.m_tags[it->first] = it->second.string();
        }
        m.m_baseMonoTime = errorMsg["basemonotime"].integer();
        m.m_monoTime = errorMsg["monotime"].integer();
        m.m_time = Time::fromRFC1123(errorMsg["time"].string());
//        m.m_traceArea = (const TraceArea) errorMsg.mapValue<int>("tracearea", &ok);
        m.m_type = errorMsg["logtype"].string();

        mCriticalMap.insert(std::make_pair(++mNextCriticalId, m));

        NTRACES(TRACE_LOG, "Loaded critical " << mNextCriticalId << " " << m.m_type);
    }

}

void LogBridge::storeCriticalErrors(const CriticalMap & errors)
{
    // compress it

    Variant errorMsgs;

    bool flush = false;
    CriticalMap::const_iterator it1;
    for (it1 = errors.begin(); it1 != errors.end(); ++it1) {
        const Log::Message & message = it1->second;
        Variant errorMap;
        //store getTime
        // store whether ot not nccp time
        errorMap["basemonotime"] = message.getBaseMonoTime().ms();
        errorMap["monotime"] = message.m_monoTime.ms();
        errorMap["time"] = message.m_time.rfc1123();
        errorMap["critical"] = message.m_flags & Log::Message::Critical;
        if(message.m_flags & Log::Message::SendToAppBoot)
            errorMap["sendtoappboot"] = true;
        if(message.m_flags & Log::Message::SendToIchnaea)
            errorMap["sendtoichnaea"] = true;
        errorMap["level"] = message.m_logLevel;
        errorMap["message"] = message.m_message;
        errorMap["logtype"] = message.m_type;
        errorMap["tags"] = message.m_tags;
//        errorMap["tracearea"] = message.m_traceArea;
        errorMsgs.push_back(errorMap);
        if(message.m_traceArea == &TRACE_MILESTONE) {
            flush = true;
        }
        NTRACES(TRACE_LOG, "Saving critical " << (it1->first) << " " << message.m_type);
    }
    std::string json;
    std::string compressed;
    json = errorMsgs.toJSON();
    StringCompressor::deflateB64(compressed, json);
    nrdApp()->writeSystemConfiguration(SystemKeys::criticalErrors, compressed,
                                       flush ? NrdApplication::WriteMode_Flush : NrdApplication::WriteMode_None);

}


void LogBridge::shutdown()
{
    nrdApp()->writeSystemConfiguration(SystemKeys::upTime, lexical_cast<std::string>(nrdApp()->timeSinceApplicationStarted()));

    if (Configuration::crashReportShutdowns()) {
        // this removes the keys
        nrdApp()->writeSystemConfiguration(SystemKeys::nrdStarted, std::string());
        nrdApp()->writeSystemConfiguration(SystemKeys::crashCount, std::string());
    }
}

bool LogBridge::getProperty(int index, Variant *value) const
{
    switch (index) {
    case Properties::LogBridge::areas:
        *value = TraceAreas::all();
        break;
    case Properties::LogBridge::errorCodes: {
        uint32_t err;

#define HANDLE_CATEGORY(category)                                       \
        for (err = category + 1; err != category + 0x10000; ++err) {    \
            const std::string name = NFErrName::lookup(static_cast<NFErrorCode>(err)); \
            if (name.empty())                                           \
                break;                                                  \
            (*value)[name] = static_cast<int>(err);                     \
        }                                                               \
        do {} while(0)

        HANDLE_CATEGORY(NFERR_CATEGORY_SUCCESS_VAL);
        HANDLE_CATEGORY(NFERR_CATEGORY_GENERICERROR_VAL);
        HANDLE_CATEGORY(NFERR_CATEGORY_MCERROR_VAL);
        HANDLE_CATEGORY(NFERR_CATEGORY_DEVICEACCOUNT_VAL);
        HANDLE_CATEGORY(NFERR_CATEGORY_NCCP_VAL);
        HANDLE_CATEGORY(NFERR_CATEGORY_CRYPTO_VAL);
#undef HANDLE_CATEGORY

        break;
    }
    case Properties::LogBridge::aseCodes:
#define HANDLE_AS_ERROR(err) (*value)[#err] = err;

        /** Common error codes. */
        HANDLE_AS_ERROR(AS_NO_ERROR);
        HANDLE_AS_ERROR(AS_UNKNOWN_ERROR);
        HANDLE_AS_ERROR(AS_NOT_ENOUGH_MEMORY);
        HANDLE_AS_ERROR(AS_NOT_FOUND);
        HANDLE_AS_ERROR(AS_INVALID_VALUE);
        HANDLE_AS_ERROR(AS_NOT_ALlOWED);
        HANDLE_AS_ERROR(AS_DEAD_LOCK);
        HANDLE_AS_ERROR(AS_PERMISSION_ERROR);
        HANDLE_AS_ERROR(AS_BUSY);
        HANDLE_AS_ERROR(AS_NOT_SUPPORTED);
        HANDLE_AS_ERROR(AS_AGAIN);
        HANDLE_AS_ERROR(AS_IN_PROGRESS);
        HANDLE_AS_ERROR(AS_PENDING);
        HANDLE_AS_ERROR(AS_TIMEOUT);
        HANDLE_AS_ERROR(AS_INTERRUPTED);
        HANDLE_AS_ERROR(AS_PARSE_ERROR);
        HANDLE_AS_ERROR(AS_MORE);
        HANDLE_AS_ERROR(AS_STALE_CREDENTIALS);
        HANDLE_AS_ERROR(AS_INIT_FAILURE);
        HANDLE_AS_ERROR(AS_DRM_FAILURE);
        HANDLE_AS_ERROR(AS_COMPRESSION_ERROR);
        HANDLE_AS_ERROR(AS_ABANDONED);

        /** Async HTTP Library error codes. */
        HANDLE_AS_ERROR(AS_SOCKET_ERROR);
        HANDLE_AS_ERROR(AS_SELECT_ERROR);

        HANDLE_AS_ERROR(AS_DNS_ERROR);
        HANDLE_AS_ERROR(AS_DNS_TIMEOUT);
        HANDLE_AS_ERROR(AS_DNS_NOT_FOUND);

        HANDLE_AS_ERROR(AS_CONNECTION_NOT_OPEN);
        HANDLE_AS_ERROR(AS_CONNECTION_ERROR);
        HANDLE_AS_ERROR(AS_CONNECTION_TIMEOUT);
        HANDLE_AS_ERROR(AS_CONNECTION_REFUSED);
        HANDLE_AS_ERROR(AS_CONNECTION_NET_UNREACHABLE);
        HANDLE_AS_ERROR(AS_CONNECTION_RESET);
        HANDLE_AS_ERROR(AS_CONNECTION_CLOSED);
        HANDLE_AS_ERROR(AS_CONNECTION_SLOW);
        HANDLE_AS_ERROR(AS_CONNECTION_NO_ROUTE_TO_HOST);
        HANDLE_AS_ERROR(AS_CONNECTION_NETWORK_DOWN);
        HANDLE_AS_ERROR(AS_CONNECTION_NO_ADDRESS);
        HANDLE_AS_ERROR(AS_CONNECTION_RESET_ON_CONNECT);
        HANDLE_AS_ERROR(AS_CONNECTION_RESET_WHILE_RECEIVING);

        HANDLE_AS_ERROR(AS_HTTP_CONNECTION_STALL);
        HANDLE_AS_ERROR(AS_HTTP_PROTOCOL_ERROR);
        HANDLE_AS_ERROR(AS_HTTP_PROTOCOL_1_0);
        HANDLE_AS_ERROR(AS_HTTP_RESPONSE_3XX);
        HANDLE_AS_ERROR(AS_HTTP_RESPONSE_4XX);
        HANDLE_AS_ERROR(AS_HTTP_RESPONSE_5XX);
        HANDLE_AS_ERROR(AS_HTTP_PIPELINING_NOT_SUPPORTED);
        HANDLE_AS_ERROR(AS_HTTP_PIPELINING_NOT_STABLE);

        HANDLE_AS_ERROR(AS_DNS_NO_DATA);
        HANDLE_AS_ERROR(AS_DNS_QUERY_MALFORMED);
        HANDLE_AS_ERROR(AS_DNS_SERVER_FAILURE);
        HANDLE_AS_ERROR(AS_DNS_NOT_IMPLEMENTED);
        HANDLE_AS_ERROR(AS_DNS_QUERY_REFUSED);
        HANDLE_AS_ERROR(AS_DNS_BAD_NAME);
        HANDLE_AS_ERROR(AS_DNS_CONNECTION_REFUSED);
        HANDLE_AS_ERROR(AS_DNS_NO_MEMORY);
        HANDLE_AS_ERROR(AS_DNS_CHANNEL_DESTROYED);

        HANDLE_AS_ERROR(AS_CONNECTION_INVALID_DESTINATION);

        HANDLE_AS_ERROR(AS_HTTP_INVALID_URL);
        HANDLE_AS_ERROR(AS_HTTP_CONNECTION_ERROR);
        HANDLE_AS_ERROR(AS_HTTP_CONNECTION_TIMEOUT);

        HANDLE_AS_ERROR(AS_DNS_RESPONSE_MALFORMED);

        HANDLE_AS_ERROR(AS_HTTP_TOO_MANY_REDIRECTS);
        HANDLE_AS_ERROR(AS_HTTP_TRANSACTION_TIMEOUT);

        HANDLE_AS_ERROR(AS_SSL_ERROR);

        HANDLE_AS_ERROR(AS_HTTP_MESSAGE_LENGTH_ERROR);
        HANDLE_AS_ERROR(AS_HTTP_HEADER_LENGTH_ERROR);


        HANDLE_AS_ERROR(AS_DNS_FORMAT_ERROR);
        HANDLE_AS_ERROR(AS_DNS_BAD_FAMILY);
        HANDLE_AS_ERROR(AS_DNS_EOF);
        HANDLE_AS_ERROR(AS_DNS_FILE);
        HANDLE_AS_ERROR(AS_DNS_BAD_STRING);
        HANDLE_AS_ERROR(AS_DNS_BAD_FLAGS);
        HANDLE_AS_ERROR(AS_DNS_NO_NAME);
        HANDLE_AS_ERROR(AS_DNS_BAD_HINTS);
        HANDLE_AS_ERROR(AS_DNS_NOT_INITIALIZED);
        HANDLE_AS_ERROR(AS_DNS_CANCELLED);

        /** Byte range downloader error codes. */
        HANDLE_AS_ERROR(AS_MEDIA_BUFFER_UNAVAILABLE);
        HANDLE_AS_ERROR(AS_NO_RECEIVING_TASK);

        /** HTTP Request Manager */
        HANDLE_AS_ERROR(AS_NETWORK_FAILURE);
        HANDLE_AS_ERROR(AS_HTTP_CLIENT_ERROR);
        HANDLE_AS_ERROR(AS_HTTP_SERVER_ERROR);
        HANDLE_AS_ERROR(AS_DOMAIN_TIMEOUT);
        HANDLE_AS_ERROR(AS_DOMAIN_SLOW);
        HANDLE_AS_ERROR(AS_DOMAIN_CONNECTION_ERROR);
        HANDLE_AS_ERROR(AS_INCOMPATIBLE_URL);
        HANDLE_AS_ERROR(AS_HTTP_ALL_URLS_FAILED);

        /** Media Presentation Tree */
        HANDLE_AS_ERROR(AS_MEDIA_PRESENTATION_FAILURE);
        HANDLE_AS_ERROR(AS_MEDIA_DURATION_ERROR);
        HANDLE_AS_ERROR(AS_MEDIA_HEADER_ERROR);
        HANDLE_AS_ERROR(AS_MEDIA_INDEX_ERROR);
        HANDLE_AS_ERROR(AS_MEDIA_FILE_REQUEST_PENDING);
        HANDLE_AS_ERROR(AS_MEDIA_FILE_NO_REQUEST_NEEDED);
        HANDLE_AS_ERROR(AS_MEDIA_END);
        HANDLE_AS_ERROR(AS_MEDIA_NOT_SUPPORTED);

        /** Domain Selector */
        HANDLE_AS_ERROR(AS_DOMAIN_SELECTION_FAILURE);
        HANDLE_AS_ERROR(AS_DOMAIN_SELECTION_FAILURE_FAILED_TOO_LONG);
        HANDLE_AS_ERROR(AS_DOMAIN_SELECTION_FAILURE_FAILED_TOO_MANY);
        HANDLE_AS_ERROR(AS_DOMAIN_SELECTION_FAILURE_BAD_STREAM);
        HANDLE_AS_ERROR(AS_DOMAIN_SELECTION_FAILURE_NO_AVAILABLE_LOCATION);
        HANDLE_AS_ERROR(AS_DOMAIN_SELECTION_FAILURE_FAILED_TOO_MANY_CONSECUTIVE_TIMES);
        HANDLE_AS_ERROR(AS_LAST_DOMAIN_SELECTION_FAILURE);

        /** Media File Factory */
        HANDLE_AS_ERROR(AS_UNSUPPORTED_MIME_TYPE);

        /** Dash parser */
        HANDLE_AS_ERROR(AS_DASH_INCOMPATIBLE_STREAMS_IN_TRACK);

        /** Persistent store */
        HANDLE_AS_ERROR(AS_NOT_ENOUGH_SPACE);
        HANDLE_AS_ERROR(AS_ENCODE_ERROR);

        HANDLE_AS_ERROR(AS_REQUEST_CREATION_FAILURE);
        HANDLE_AS_ERROR(AS_ILLEGAL_REQUEST);

        break;
    case Properties::LogBridge::haveTracing:
#if defined(NRDP_HAS_TRACING)
        *value = true;
#else
        *value = false;
#endif
        break;
    case Properties::LogBridge::level:
        *value = mLogLevel;
        break;
    case Properties::LogBridge::levels: {
        Variant vm;
        vm["console"] = Log::Console;
        vm["trace"] = Log::Trace;
        vm["debug"] = Log::Debug;
        vm["info"] = Log::Info;
        vm["warn"] = Log::Warn;
        vm["error"] = Log::Error;
        vm["fatal"] = Log::Fatal;
        *value = vm;
        break;
    }
    case Properties::LogBridge::traceAreas:
        *value = app()->traceAreas()->spec();
        break;
    case Properties::LogBridge::UIContext:
        *value = mUIContext;
        break;
    case Properties::LogBridge::appid:
        *value = lexical_cast<std::string>(Log::appId());
        break;
    case Properties::LogBridge::sessionid:
        *value = lexical_cast<std::string>(Log::sessionId());
        break;
    default:
        return false;
    }
    return true;
}

bool LogBridge::setProperty(int index, const Variant &value)
{
    switch (index) {
    case Properties::LogBridge::level: {
        mLogLevel = value.value<int>(0, mLogLevel);
        if (mLogLevel < Log::Debug)
            mLogLevel = Log::Debug;
        size_t origSize, newSize;
        NRDP_UNUSED(origSize);
        NRDP_UNUSED(newSize);
        {
            ScopedMutex lock(mMutex);
            origSize = mLogmsgs.size();
            std::list<Log::Message>::iterator it = mLogmsgs.begin();
            while (it != mLogmsgs.end()) {
                if (it->m_type.empty() && it->m_logLevel < mLogLevel) {
                    mLogmsgs.erase(it++);
                } else {
                    ++it;
                }
            }
            newSize = mLogmsgs.size();
        }
        NTRACE(TRACE_NBP, "Updated LogBridge::level to %s removed %zu messages, from %zu to %zu",
               Log::logLevelToString(static_cast<Log::LogLevel>(mLogLevel)),
               origSize - newSize, origSize, newSize);
        propertyUpdated(Properties::LogBridge::level);
        break;
    }
    case Properties::LogBridge::traceAreas: {
        bool ok;
        const std::string spec = value.value<std::string>(&ok);
        if (!ok)
            return false;
        app()->traceAreas()->parse(spec.c_str());
        propertyUpdated(Properties::LogBridge::traceAreas);
        break;
    }
    case Properties::LogBridge::UIContext:
        mUIContext = value.value<std::string>();
        INSTRUMENTATION_EVENT_SWITCHED("nrdp.ui-context" , Variant::StringMap("context" , mUIContext ) );
        propertyUpdated(Properties::LogBridge::UIContext);
        break;
    default:
        return false;
    }
    return true;
}

const TraceArea *LogBridge::ensureTraceArea(std::string traceArea, std::string groups)
{
    const TraceArea *area = TraceArea::find(traceArea);
    if (area)
        return area;

    if (groups.empty()) {
        groups = "dynamic";
    } else {
        groups += "|dynamic";
    }

    NTRACE(TRACE_TRACE, "Creating trace area with name '%s' in groups '%s'", traceArea.c_str(), groups.c_str());

    shared_ptr<TraceArea> area_ptr(new TraceArea(traceArea.c_str(), groups.c_str()));
    mDynamicTraceAreas.insert(area_ptr);
    app()->traceAreas()->isEnabled(area_ptr.get()); // updates traceAreas
    const int properties[] = {
        Properties::LogBridge::areas,
        Properties::LogBridge::traceAreas
    };
    propertiesUpdated(properties, sizeof(properties) / sizeof(properties[0]));

    return area_ptr.get();
}

Variant LogBridge::invoke(int method, const Variant &data)
{
        bool ok;
    switch (method) {
    case Methods::LogBridge::createArea: {
        const std::string traceArea = data.mapValue<std::string>("area", &ok);
        if (!ok) {
            invalidArgumentError(method, "area");
            return false;
        }
        std::string groups = data.mapValue<std::string>("groups", 0);
        ensureTraceArea(traceArea, groups);
        break; }
    case Methods::LogBridge::milestone: {
        const std::string msg = data.mapValue<std::string>("msg", &ok);
        if(!ok) {
            invalidArgumentError(method, "msg");
            return false;
        }
        Log::milestone("%s", msg.c_str());
        break; }
    case Methods::LogBridge::log: {
        Log::Message message;
        message.m_logLevel = static_cast<Log::LogLevel>(data.mapValue<int>("logLevel", &ok));
        if (!ok) {
            invalidArgumentError(method, "logLevel");
            return false;
        }
        {
            message.m_message = data.mapValue<std::string>("msg");
            if (message.m_logLevel >= Log::Fatal) {
                const std::string stack = data.mapValue<std::string>("jsDebuggerStack");
                if (!stack.empty()) {
                    if (!message.m_message.empty())
                        message.m_message += "\nJS Stack:\n";
                    message.m_message += stack;
                }
            }

            const std::string traceArea = data.mapValue<std::string>("traceArea", 0, TRACE_UI_SCRIPT.getName());
            message.m_traceArea = ensureTraceArea(traceArea, std::string());
            message.m_type = data.mapValue<std::string>("type");

            // this comes from gibbon, and will always be here if logLevel >=
            // Fatal or if debugger is attached and logLevel > Trace
            message.m_jsStack = data.value("jsDebuggerStack").toJSON();
            message.m_flags &= ~Log::Message::SendToJS;

            Variant variant = data.value("tags");
            if (!variant.isNull()) {
                if (variant.isString())
                    variant = Variant::fromJSON(variant.string());
                for(Variant::StringMapIterator it = variant.stringMapBegin(); it != variant.stringMapEnd(); ++it) {
                    const std::string value = it->second.value<std::string>();
                    message.setTag(it->first, value);
                }
            }
            if(data.mapValue<bool>("sendtoappboot"))
                message.m_flags |= Log::Message::SendToAppBoot;
            if(data.mapValue<bool>("sendtoichnaea"))
                message.m_flags |= Log::Message::SendToIchnaea;
            bool critical = data.mapValue<bool>("critical");
            if (critical) {
                message.m_flags |= Log::Message::Critical;
                CriticalMap localMap;
                size_t added = 0;
                NRDP_UNUSED(added);
                {
                    ScopedMutex lock(sCriticalErrorsMutex);
                    // store the critical data in secure store if there is enough room
                    if (mCriticalMap.size() < CRITICAL_VECTOR_MAX_SIZE)  {
                        mCriticalMap.insert(std::make_pair(++mNextCriticalId, message));
                        added = mNextCriticalId;
                        localMap = mCriticalMap;
                    }
                }
                if (!localMap.empty()) {
                    NTRACES(TRACE_LOG, "Added critical " << added << " " << message.m_type);
                    storeCriticalErrors(localMap);
                }
            }

            message.send();
        }
        break; }
    case Methods::LogBridge::getLogMessages: {
        Variant array;
        popLogMessages(array);
        NTRACE(TRACE_NBP, "LogBridge::getLogMessages sending %d messages", array.size());
        if (array.size())
            sendEvent("logMsgsReady", array);
        break; }
    case Methods::LogBridge::getCriticalMessages: {
        Variant array;
        array.resizeArray(0);
        getCriticalMessages(array);
        NTRACE(TRACE_NBP, " getCriticalMessages %d messages", array.size());
        sendEvent("criticalMsgsReady", array);
        break; }
    case Methods::LogBridge::deleteCriticalMessages: {
        Variant array = data.value("criticalMessages");
        deleteCriticalMessages(array);
        NTRACE(TRACE_NBP, "LogBridge::getLogMessages deleteCriticalMessages %d messages", array.size());
        break; }
    case Methods::LogBridge::flush: {
        // send a flush message through the log pipeline
        Log::Message m(TRACE_LOG, Log::Info, nrdlog::MessageType::flush);
        m.send();
        break; }
    case Methods::LogBridge::resetAppID: {
        Variant evtData;
        bool ok;
        const std::string appIDStr = data.mapValue<std::string>("appid", &ok);
        if (!ok) {
            NTRACE(TRACE_NBP, "LogBridge::resetAppID no argument passed, will generate random");
            nrdApp()->systemData()->resetAppID();
        } else {
            nrdApp()->systemData()->resetAppID(lexical_cast<ullong>(appIDStr));
        }
        propertyUpdated(Properties::LogBridge::appid);
        evtData["appid"] = Log::appId();
        sendEvent("appIdResetComplete", evtData);
        break; }
    case Methods::LogBridge::resetSessionID: {
        nrdApp()->systemData()->resetSessionID();
        propertyUpdated(Properties::LogBridge::sessionid);
        Variant newval;
        getProperty(Properties::LogBridge::sessionid, &newval);
        return newval; }
    default:
        return NfObject::invoke(method, data);
    }
    return Variant();
}

void LogBridge::popLogMessages(Variant & array)
{
    std::list<Log::Message> list;
    {
        ScopedMutex lock(mMutex);
        list.swap(mLogmsgs);
    }

    for (std::list<Log::Message>::const_iterator it = list.begin(); it != list.end(); ++it) {
        Variant map;
        Variant tags;
        if((*it).m_baseMonoTime.ms() && (*it).m_baseMonoTime != Application::MONO_STARTED) //undefined is current session
            map["basemonotime"] = (*it).m_baseMonoTime.ms();
        map["monotime"] = (*it).m_monoTime.ms();
        map["area"] = (*it).m_traceArea->getName();
        map["level"] = (*it).m_logLevel;
        map["msg"] = (*it).m_message;
        map["tags"] = (*it).m_tags;
        map["logtype"] = (*it).m_type;
        map["threadname"] = (*it).m_threadName;
        map["threadid"] = (uintptr_t) (*it).m_threadId;
        if((*it).m_flags & Log::Message::SendToIchnaea)
            map["sendtoichnaea"] = true;
        if((*it).m_flags & Log::Message::Critical)
            map["critical"] = true;
        if((*it).m_flags & Log::Message::SendToAppBoot)
            map["sendtoappboot"] = true;
        // deliberately ignore jsStack and sendToJS
        array.push_back(map);
    }
}

void LogBridge::getCriticalMessages(Variant & array)
{
    ScopedMutex lock(sCriticalErrorsMutex);
    CriticalMap::const_iterator it;
    for (it = mCriticalMap.begin(); it != mCriticalMap.end(); ++it) {
        const size_t criticalId = it->first;
        const Log::Message & message = it->second;
        Variant map;
        Variant tags;
        map["basemonotime"] = message.getBaseMonoTime().ms();
        map["monotime"] = message.m_monoTime.ms();
        map["time"] = message.m_time.ms();
        // map["area"] = message.m_traceArea->getName();
        map["level"] = message.m_logLevel;
        map["msg"] = message.m_message;
        map["tags"] = message.m_tags;
        map["logtype"] = message.m_type;
        if(message.m_flags & Log::Message::SendToIchnaea)
            map["sendtoichnaea"] = true;
        if(message.m_flags & Log::Message::Critical)
            map["critical"] = true;
        if(message.m_flags & Log::Message::SendToAppBoot)
            map["sendtoappboot"] = true;
        // map["threadname"] = message.m_threadName;
        // map["threadid"] = (ullong) message.m_threadId;
        // deliberately ignore jsStack and sendToJS
        map["criticalId"] = criticalId;
        array.push_back(map);
    }

}

void LogBridge::deleteCriticalMessages(Variant & array)
{
    if (!array.isArray()) {
        return;
    }
    size_t initialCount = 0;
    size_t toDeleteCount = array.size();
    size_t deletedCount = 0;
    NRDP_UNUSED(initialCount);
    NRDP_UNUSED(toDeleteCount);
    CriticalMap localMap;
    {
        ScopedMutex lock(sCriticalErrorsMutex);
        initialCount = mCriticalMap.size();
        for (int it2 = 0; it2 < array.size(); ++it2) {
            const size_t criticalId = static_cast<size_t>(array[it2]["criticalId"].value<ullong>());
            CriticalMap::iterator found = mCriticalMap.find(criticalId);
            if (found != mCriticalMap.end()) {
                mCriticalMap.erase(found);
                ++deletedCount;
            }
        }
        // We can reset it here since there are no more left
        if (mCriticalMap.empty()) {
            mNextCriticalId = 0;
        }
        localMap = mCriticalMap;
    }
    NTRACES(TRACE_LOG, "Had " << initialCount
        << ", asked to delete " << toDeleteCount
        << ", deleted " << deletedCount
        << ", left with " << localMap.size());
    // write them no matter what; this is the only function that can remove
    // them, so if there aren't any left then the all need to be removed
    storeCriticalErrors(localMap);

    Variant carray;
    carray.resizeArray(0);
    getCriticalMessages(carray);
    NTRACE(TRACE_LOG, "getCriticalMessages %d messages", carray.size());
    sendEvent("criticalMsgsReady", carray);
}

//////////
//
// receive
//
//////////
void LogBridge::receive(const Log::Message &logmsg)
{
    if ((logmsg.m_flags & Log::Message::SendToJS) && (logmsg.m_logLevel >= mLogLevel || !logmsg.m_type.empty())) {
        bool warn = false;
        if (logmsg.m_flags & Log::Message::Critical) {
            CriticalMap localMap;
            size_t added = 0;
            NRDP_UNUSED(added);
            {
                ScopedMutex lock(sCriticalErrorsMutex);
                // store the critical data in secure store if there is enough room
                if (mCriticalMap.size() < CRITICAL_VECTOR_MAX_SIZE)  {
                    mCriticalMap.insert(std::make_pair(++mNextCriticalId, logmsg));
                    added = mNextCriticalId;
                    localMap = mCriticalMap;
                }
            }
            if (!localMap.empty()) {
                NTRACES(TRACE_LOG, "Received critical " << added << " " << logmsg.m_type);
                storeCriticalErrors(localMap);
            }
        } else {
            ScopedMutex lock(mMutex);
            if (mLogmsgs.size() >= 80000) {
                if (!mWarned) {
                    mWarned = true;
                    warn = true;
                }
            } else {
                mLogmsgs.push_back(logmsg);
            }
        }
        if (warn) {
            Log::Message msg(TRACE_NBP, Log::Error);
            msg.m_flags &= ~Log::Message::SendToJS;
            msg.m_message = ("Too many messages in log bridge. You probably need to do this in your example:\n"
                             "'nrdp.log.level = 100;'");
            msg.send();
        }
    }
    if (logmsg.m_type == nrdlog::MessageType::flush) {
        Variant carray;
        getCriticalMessages(carray);
        NTRACE(TRACE_NBP, "LogBridge::getLogMessages getCriticalMessages %d messages", carray.size());
        if (carray.size())
            sendEvent("criticalMsgsReady", carray);

        Variant array;
        popLogMessages(array);
        if (array.size())
            sendEvent("logMsgsReady", array);

        sendEvent("logflush");
    }
}

unsigned LogBridge::requestFlags(const Request &request) const
{
    unsigned ret = 0;
    if (request.type() == Request::Method) {
        switch (request.method()) {
        case Methods::LogBridge::getLogMessages:
        case Methods::LogBridge::getCriticalMessages:
        case Methods::LogBridge::flush:
            break;
        case Methods::LogBridge::log: {
            const int level = nrdApp()->debuggerAttached() ? Log::Info : Log::Fatal;
            if (request.value<int>("logLevel") >= level) {
                ret |= Request::JSBacktrace;
            }
            ret |= (Request::Synchronous|Request::NoLog);
            break; }
        default:
            ret |= Request::Synchronous;
            break;
        }
    }
    return ret;
}
