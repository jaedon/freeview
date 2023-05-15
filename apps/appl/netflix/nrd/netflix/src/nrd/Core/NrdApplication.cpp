/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "NrdApplication.h"

#include <fstream>
#include <sstream>

#include <nrd/IDeviceLib.h>
#include <nrdnet/DnsManager.h>

#include <curl/curl.h>
#include <expat.h>
#include <openssl/crypto.h>
#include <openssl/opensslv.h>
#include <zlib.h>
#include <ares.h>
#include <errno.h>

#include <nrd/config.h>

#include <nrdase/HttpRequestManager.h>

#include <nrdbase/Configuration.h>
#include <nrdbase/Console.h>
#include <nrdbase/Instrumentation.h>
#include <nrdbase/JSONFormatter.h>
#include <nrdbase/Log.h>
#include <nrdbase/MemoryStats.h>
#include <nrdbase/ReadDir.h>
#include <nrdbase/StringFormatter.h>
#include <nrdbase/StringTokenizer.h>
#include <nrdbase/StringTokenizer.h>
#include <nrdbase/EmbeddedResources.h>
#include <nrdbase/StringUtils.h>
#include <nrdbase/TraceAreas.h>
#include <nrdbase/UrlEncoder.h>
#include <nrdbase/Version.h>

#include <nrdnet/ResourceManager.h>

#include "AppLog.h"
#include "AppThread.h"
#include "EventConnection.h"
#include "EventSourceEventConnection.h"
#include "NfObject.h"
#include "NrdConfiguration.h"
#include "Request.h"
#include "Response.h"
#include "NrdDiskCache.h"
#include "StreamingConfigParameter.h"
#include "StreamingPersistentStore.h"
#include "InstrumentationThread.h"
#include "ISystemEventForwarder.h"
#include "NrdIpConnectivityManager.h"
#include "NrdIpConfigurationListener.h"
#include "NrdConfiguration.h"
#include "SecureStore.h"
#include "SystemData.h"
#include "DiskStore.h"
#include "bridge/LogBridge.h"
#include "bridge/NrdpBridge.h"
#include "bridge/DeviceBridge.h"
#ifdef NRDP_HAS_PARTNER_INTEGRATION
#include "bridge/TileBridge.h"
#endif

#include "IAdaptiveStreamingPlayer.h"

#ifdef NRDP_ASE
# include <nrdase/NetworkMonitor.h>
#endif

#include "bridge/SystemBridge.h"
#include "bridge/NetworkBridge.h"

#ifdef NRDP_MDX_ENABLED
# include <nrd/MdxBridge.h>
# include <nrd/MdxVersion.h>
#endif

#ifdef HAVE_MTRACE
# include <mcheck.h>
#endif

#ifdef NRDP_COMPILED_RESOURCES
namespace netflix {
extern int initializeResources();
}
#endif

#if defined(BUILD_DEBUG) && HAVE_MALLOC_H
#include <malloc.h>
static inline void dumpHeap(const char *label)
{
    struct mallinfo info = mallinfo();
    NTRACE(TRACE_SYSTEM,
           "%s: used (small + heap) = %d bytes\n"
           "%s: total heap = %d bytes\n",
           label, (info.usmblks + info.uordblks), label, info.arena);
}
#endif

using namespace netflix;
using namespace resources;
using namespace netflix::util;
using namespace netflix::mediacontrol;
using namespace netflix::device;
using namespace netflix::ase;


#ifdef INSTRUMENTATION_ENABLED
class InstrumentationCommand : public Console::Command
{
public:
    InstrumentationCommand()
        : Console::Command("instrumentation", "Dump instrumentation information",
                           Variant::StringMap("flush")("enable")("disable")("event")("switched",Variant::StringMap("on")("off"))("stash",Variant::StringMap("on")("off")("pop")))
    {}
    virtual void invoke(const Arguments &arguments) const
    {
        if(arguments.size() == 1) {
            instrumentation::flush();
        } else if(arguments.size() > 1) {
            if(arguments[1] == "flush") {
                instrumentation::flush();
            } else if(arguments[1] == "enable") {
                instrumentation::enable();
            } else if(arguments[1] == "disable") {
                instrumentation::disable();
            } else if(arguments[1] == "stash" && arguments.size() > 2) {
                if(arguments[2] == "on") {
                    instrumentation::stash(true);
                } else if(arguments[2] == "off") {
                    instrumentation::stash(false);
                } else if(arguments[2] == "pop") {
                    Variant events = instrumentation::pop_stash();
                    Log::warn(TRACE_SYSTEM) << "Popped " << events.size() << " event(s)";
                }
            } else if(arguments[1] == "event" && arguments.size() > 2) {
                INSTRUMENTATION_EVENT_ON( arguments[2] );
            } else if(arguments[1] == "switched" && arguments.size() > 2) {
                instrumentation::switched( arguments[2] == "on" );
            }
        }
    }
    virtual std::vector<Help> help() const
    {
        std::vector<Help> result;
        result.push_back(Help("flush", "Flush all instrumentation pending events."));
        result.push_back(Help("enable", "Enable instrumentation."));
        result.push_back(Help("disable", "Disable instrumentation."));
        result.push_back(Help("stash", "<mode>", "Command stash.") << Help("on", "Enable") << Help("off", "Disable") << Help("pop", "Pop current stash"));
        result.push_back(Help("switched", "<mode>", "Toggle switched events.")  << Help("on", "Enable") << Help("off", "Disable"));
        result.push_back(Help("event", "<event>", "Turn on event <event>"));
        return result;
    }
};
#endif

class ResourcesCommand : public Console::Command
{
public:
    ResourcesCommand() : Console::Command("resources", "Display compiled in resources") { }

    virtual void invoke(const Arguments &arguments) const
    {
        if(arguments.size() > 1 && Configuration::resourceExists(arguments[1])) {
            const DataBuffer data = Configuration::resourceContent(arguments[1]);
            Log::warn(TRACE_SYSTEM, "Resource: %s\n%s", arguments[1].c_str(), Log::hexDump(data.data(), data.size()).c_str());
        } else if(sResourceList) {
            for(ResourceTableIterator it = sResourceList->begin(); it != sResourceList->end(); ++it) {
                if(arguments.size() < 2 || arguments[1] == it->second.type)
                    Log::warn(TRACE_SYSTEM, "Resource(%s): %s: %zd\n",
                              it->second.type, it->first.c_str(), it->second.end-it->second.start);
            }
        } else {
            Log::warn(TRACE_SYSTEM, "Resource list unavailable.");
        }
    }
};

class WriteLimiterCommand : public Console::Command
{
public:
    WriteLimiterCommand() : Console::Command("writelimiter", "Display info about write limiters") { }

    virtual void invoke(const Arguments &) const
    {
        WriteLimiter::Dumper dumper;
        if (shared_ptr<NrdDiskCache> diskCache = static_pointer_cast<NrdDiskCache>(nrdApp()->resourceManager()->diskCache())) {
            dumper << *diskCache;
        }
        const std::vector<std::string> lines = dumper.toConsole();
        for (std::vector<std::string>::const_iterator it = lines.begin(); it != lines.end(); ++it) {
            Log::warn(TRACE_SYSTEM, "%s", it->c_str());
        }
    }
};


class DiskStoreCommand : public Console::Command
{
public:
    DiskStoreCommand()
        : Console::Command("diskstore", "Display information about the diskstore")
    {}

    void invoke(const Arguments &arguments) const
    {
        if (shared_ptr<DiskStore> diskStore = nrdApp()->diskStore()) {
            DumpInfo dump;
            if (arguments.size() > 1)
                dump.match = arguments[1];
            diskStore->dump(dump);
        } else {
            Log::warn(TRACE_DISK_STORE, "No diskstore to be found");
        }
    }
};

class HelpCommand : public Console::Command
{
public:
    HelpCommand(const std::string &completions) : Console::Command("help", "This helpful document!", completions) { }

    virtual void invoke(const Arguments &arguments) const
    {
        std::ostringstream helpCommand;
        std::vector<std::string> helpArgs;
        StringTokenizer::tokenize(arguments.command, helpArgs);
        for(size_t i = 1; i < helpArgs.size(); ++i) {
            if(i != 1)
                helpCommand << " ";
            else
                helpCommand << "/";
            helpCommand << helpArgs[i];
        }
        nrdApp()->console()->handleHelp(helpCommand.str());
    }
};

class QuitCommand : public Console::Command
{
public:
    QuitCommand()
        : Console::Command("quit", "Quit application")
    {}
    virtual void invoke(const Arguments &arguments) const
    {
        int code = 0;
        if(arguments.size() > 1)
            code = atoi(arguments[1].c_str());
        nrdApp()->scheduleQuit(code);
    }
    virtual std::vector<Help> help() const
    {
        std::vector<Help> result;
        result.push_back(Help("[code]", "Quit application with [code]."));
        return result;
    }
};

class SuspendCommand : public Console::Command
{
public:
    SuspendCommand()
        : Console::Command("suspend", "Toggle suspend mode", "true false on off")
    {}
    virtual void invoke(const Arguments &arguments) const
    {
        std::string reason;
        bool suspend = !nrdApp()->isSuspended();
        for (size_t i=1; i<arguments.size(); ++i) {
            const std::string arg = arguments[i];
            if (arg == "true" || arg == "on")
                suspend = true;
            else if (arg == "false" || arg == "off")
                suspend = false;
            else
                reason = arg;
        }
        NTRACE(TRACE_SUSPEND, "SuspendCommand %d %s", suspend, reason.c_str());
        nrdApp()->setSuspended(suspend, reason);
    }
    virtual std::vector<Help> help() const
    {
        std::vector<Help> result;
        result.push_back(Help("on", "Switch to suspended state."));
        result.push_back(Help("off", "Switch out of suspended state."));
        result.push_back(Help("[reason]", "Pass [reason] as reason for switching suspended state."));
        return result;
    }
};

class DNSCommand : public Console::Command
{
public:
    DNSCommand()
        : Console::Command("dns", "Take DNS up and down", "up down config records")
    {}
    virtual void invoke(const Arguments &arguments) const
    {
        if (arguments.size() > 1) {
            if (arguments[1] == "up") {
                DnsManager::up();
            } else if (arguments[1] == "down") {
                DnsManager::down();
            } else if (arguments[1] == "config") {
                DnsManager::Configuration conf;
                DnsManager::getConfiguration(conf);
                Log::warn(TRACE_SYSTEM, "DNS Config: %s", conf.toString().c_str());
            } else if (arguments[1] == "records") {
                const std::vector<std::string> records = DnsManager::currentRecords();
                int i = 0;
                for (std::vector<std::string>::const_iterator it = records.begin(); it != records.end(); ++it) {
                    Log::warn(TRACE_SYSTEM, "Record: %d/%zu %s", ++i, records.size(), it->c_str());
                }
            }
        } else {
            Log::warn(TRACE_SYSTEM, "Usage: /dns up|down|config|records");
        }
    }
    virtual std::vector<Help> help() const
    {
        std::vector<Help> result;
        result.push_back(Help("up", "Disable."));
        result.push_back(Help("down", "Enable."));
        result.push_back(Help("config", "Info about configuration."));
        result.push_back(Help("records", "Info about current records."));
        return result;
    }
};

class NetworkCommand : public Console::Command
{
public:
    NetworkCommand()
        : Console::Command("network", "Display network information",
                           "requests hst shaper-speed shaper-min-sleep-interval shaper-max-sleep-interval shaper-soft-capacity shaper-hard-capacity")
    {}

    virtual void invoke(const Arguments &arguments) const
    {
        DumpInfo dump;
        for (size_t i=1; i<arguments.size(); ++i) {
            if (arguments[i] == "requests") {
                dump.flags |= ResourceManager::Dump_Requests;
            } else if (arguments[i] == "hst") {
                dump.flags |= ResourceManager::Dump_HST;
            } else if (arguments[i] == "shaper-speed") {
                if(i+1 < arguments.size()) {
                    NetConfiguration::setTrafficShaperSpeed(Configuration::parseInt(arguments[++i].c_str(), ConfigurationOption::SIZE_ARGUMENT));
                    NrdApplication::instance()->systemData()->updateServiceParameters();
                }
            } else if (arguments[i] == "shaper-min-sleep-interval") {
                if(i+1 < arguments.size()) {
                    NetConfiguration::setTrafficShaperMinSleepInterval(Configuration::parseInt(arguments[++i].c_str(), ConfigurationOption::TIME_ARGUMENT));
                    NrdApplication::instance()->systemData()->updateServiceParameters();
                }
            } else if (arguments[i] == "shaper-max-sleep-interval") {
                if(i+1 < arguments.size()) {
                    NetConfiguration::setTrafficShaperMaxSleepInterval(Configuration::parseInt(arguments[++i].c_str(), ConfigurationOption::TIME_ARGUMENT));
                    NrdApplication::instance()->systemData()->updateServiceParameters();
                }
            } else if (arguments[i] == "shaper-soft-capacity") {
                if(i+1 < arguments.size()) {
                    NetConfiguration::setTrafficShaperSoftCapacity(Configuration::parseInt(arguments[++i].c_str(), ConfigurationOption::SIZE_ARGUMENT));
                    NrdApplication::instance()->systemData()->updateServiceParameters();
                }
            } else if (arguments[i] == "shaper-hard-capacity") {
                if(i+1 < arguments.size()) {
                    NetConfiguration::setTrafficShaperHardCapacity(Configuration::parseInt(arguments[++i].c_str(), ConfigurationOption::SIZE_ARGUMENT));
                    NrdApplication::instance()->systemData()->updateServiceParameters();
                }
            } else {
                dump.set(arguments[i]);
            }
        }
        if (!dump.flags)
            dump.flags = ResourceManager::Dump_Requests;
        nrdApp()->resourceManager()->dumpNetwork(dump);
        Log::warn(TRACE_SYSTEM, "shaper-speed: %lld", NetConfiguration::getTrafficShaperSpeed());
        Log::warn(TRACE_SYSTEM, "shaper-min-sleep-interval: %lld", NetConfiguration::getTrafficShaperMinSleepInterval());
        Log::warn(TRACE_SYSTEM, "shaper-max-sleep-interval: %lld", NetConfiguration::getTrafficShaperMaxSleepInterval());
        Log::warn(TRACE_SYSTEM, "shaper-soft-capacity: %lld", NetConfiguration::getTrafficShaperSoftCapacity());
        Log::warn(TRACE_SYSTEM, "shaper-hard-capacity: %lld", NetConfiguration::getTrafficShaperHardCapacity());
    }
    virtual std::vector<Help> help() const
    {
        std::vector<Help> result;
        result.push_back(Help("requests", "Display outstanding requests"));
        result.push_back(Help("hst", "Display HTTPServiceThread information."));
        result.push_back(Help("shaper-speed", "<size>", "Set shaper speed to <size>."));
        result.push_back(Help("shaper-min-sleep-interval", "<time>", "Set min sleep to <time>."));
        result.push_back(Help("shaper-max-sleep-interval", "<time>", "Set max sleep to <time>."));
        result.push_back(Help("shaper-soft-capacity", "<size>", "Set soft capacity to <size>."));
        result.push_back(Help("shaper-hard-capacity", "<size>", "Set hard capacity to <size>."));
        return result;
    }
};

class TrustStoreCommand : public Console::Command
{
public:
    TrustStoreCommand()
        : Console::Command("certs", "Show trust store certs")
    {}
    virtual void invoke(const Arguments &) const
    {
        shared_ptr<TrustStore> trustStore = nrdApp()->resourceManager()->trustStore();
        if (trustStore->x509s.empty()) {
            Log::warn(TRACE_SYSTEM) << "Trust store is empty";
        } else {
            int n = 1;
            for (std::vector<X509*>::const_iterator it = trustStore->x509s.begin(); it != trustStore->x509s.end(); ++it) {
                BIO *o = BIO_new(BIO_s_mem());
                X509_print_ex(o, *it, XN_FLAG_COMPAT, X509_FLAG_COMPAT);
                char *data = 0;
                size_t len = BIO_get_mem_data(o, &data);
                assert(data && len);
                Log::warn(TRACE_SYSTEM) << "Cert # " << n++ << '/' << trustStore->x509s.size() << "\n" << std::string(data, len);
                BIO_free(o);
            }
        }
    }
};

class NetworkListener : public device::ISystem::EventDispatcher
{
public:
    NetworkListener() {}
    virtual void capabilityChanged() {}
    virtual void videoOutputResolutionChanged() {}
    virtual void screensaverStateChanged() {}
    virtual void languageChanged() {}
    virtual void commandReceived(const std::string &) {}
    virtual void viewModeChanged() {}
    virtual void outputVolumeChanged() {}
    virtual void videoOutputStatusChanged() {}
    virtual void networkChanged()
    {
        // // Log it!
        NrdApplication *app = nrdApp();
        assert(app);
        app->systemData()->updateNetworkInterfaceInfo();
        app->configureDns();
        NetworkInterfaceInfo interfaceInfo = app->systemData()->getNetworkInterfaceInfo();
        if (shared_ptr<mediacontrol::IAdaptiveStreamingPlayer> player = app->player())
            player->networkChange(interfaceInfo);
    }
};


class ReseedEntropyTimer : public Application::Timer
{
public:
    ReseedEntropyTimer()
        : Application::Timer(MilliSecondsPerHour)
    {
    }

    virtual std::string describe() const { return "ReseedEntropyTimer"; }

    virtual void timerFired()
    {
        PERFORMANCE_MARKER_SCOPED("reseedentropytimer.fired");
        NTRACE(TRACE_SYSTEM, "reseeding SSL entropy");
        nrdApp()->getDeviceLib()->getSystem()->reseedSslEntropy();
    }
};

class NBPSendJob : public ThreadPoolJob
{
public:
    NBPSendJob(const Request &req)
        : request(req)
    {
    }

    virtual void execute()
    {
        PERFORMANCE_MARKER_SCOPED("nbpsendjob");
        nrdApp()->send(request);
    }

    virtual std::string description() const
    {
        return request.describe();
    }

    const Request request;
};

class NrdApplicationSystemDataListener : public SystemDataListener
{
public:
    virtual void datumChanged(SystemDataKey::Key key)
    {
        nrdApp()->sendEvent(ApplicationEvent::Event_SystemDataDatumChanged, key);
    }
};

using namespace netflix;
using namespace netflix::mediacontrol;

class SMAPSThread : public Thread
{
public:
    static void init(int interval) {
        ScopedMutex _lock(sMutex);
        assert(!sThread);
        sThread = new SMAPSThread(interval);
        sThread->Start();
    }

    static void shutdown() {
        SMAPSThread *thread = 0;
        {
            ScopedMutex _lock(sMutex);
            std::swap(thread, sThread);
        }
        if(thread) {
            ScopedMutex _lock(thread->sMutex);
            thread->mInterval = 0;
            while (!thread->mDone) {
                thread->mCond.wait(&thread->sMutex);
            }
            thread->Wait();
            delete thread;
        }
    }
protected:
    SMAPSThread(int interval) : Thread(&THREAD_SMAPS_COLLECTOR), mInterval(interval), mDone(false) { }

    virtual void Run()
    {
        assert(!NrdConfiguration::smapDirectory().empty());
        mkdir(NrdConfiguration::smapDirectory().c_str(), 0755);
        while (true) {
            std::ostringstream smap_filename;
            smap_filename << NrdConfiguration::smapDirectory() << "/" << Time::systemTime().seconds() << ".smap";
            if (FILE *smap = fopen(smap_filename.str().c_str(), "w")) {
                const std::string smaps = NrdApplication::instance()->stats(NrdApplication::Stat_All).toJSON();
                const int data_len = smaps.length();
                const char *data = smaps.c_str();
                for (int offset = 0; offset < data_len; ) {
                    const size_t wrote = fwrite(data+offset, 1, data_len-offset, smap);
                    offset += wrote;
                }
                fclose(smap);
            }

            {
                ScopedMutex _lock(sMutex);
                if (mInterval) {
                    const Time sleep = Time::fromSeconds(mInterval);
                    mCond.wait(&sMutex, sleep);
                }
                if (!mInterval) {
                    mDone = true;
                    mCond.broadcast();
                    break;
                }
            }
        }
    }

    int mInterval;
    static Mutex sMutex;
    ConditionVariable mCond;
    bool mDone;
    static SMAPSThread *sThread;
};
Mutex SMAPSThread::sMutex(netflix::UNTRACKED_MUTEX, "SMAPSThread");
SMAPSThread *SMAPSThread::sThread = 0;

NrdApplication::NrdApplication()
    : mQuitCode(0), mStatus(Status_NotStarted), mHasSuspendHandlers(false),
      mMutex(NRDAPPLICATION_MUTEX, "NrdApplication::mutex", Mutex::NonRecursive),
      mFirstLocationSet(Time(0)), mLocationChanged(Time::mono()),
      mEventConnectionsMutex(ZERO_MUTEX, "NBPEventConnections", Mutex::NonRecursive), mShutdownEventConnections(false),
      mThreadPool(new ThreadPool), mPlayerMutex(ZERO_MUTEX, "NrdApplication::Player::mutex", Mutex::NonRecursive)
#ifdef NRDP_WEBSERVER_ENABLED
    , mMongoose(0), mAllInterfacesEnabled(NrdConfiguration::allowAllInterfaces()),
      mHttpMutex(HTTPNBP_MUTEX, "NrdApplication::HttpNBPMutex", Mutex::NonRecursive)
#endif
{
    if (Configuration::telnetEnabled()) {
        const int port = Configuration::telnetPort();
        assert(port);
        if (port) {
            mTelnetServer.reset(new TelnetServer(port));
            mTelnetServer->Start();
        }
    }


#if defined(NRDP_HAS_TRACING)
    const char * formatString =
        "NRD (%s::%s) - "
#ifdef BUILD_DEBUG
        "Built " __TIME__ " " __DATE__ " "
#endif
#ifdef BUILD_NUMBER
        " #" BUILD_NUMBER
#endif
#ifdef BUILD_BRANCH
        " (" BUILD_BRANCH ") "
#endif
        "[DEBUG:%s] [PRODUCTION:%s] [MDX: %s]\n";

#ifdef BUILD_DEBUG
    const char * debugString = "yes";
#else
    const char * debugString = "no";
#endif

#ifdef BUILD_PRODUCTION
    const char * productionString = "yes";
#else
    const char * productionString = "no";
#endif

#ifdef NRDP_MDX_ENABLED
    const char * mdxString = MDXLIB_VERSION_STRING;
#else
    const char * mdxString = "no";
#endif

    NTRACE(TRACE_SYSTEM, formatString, NRDP_VERSION_STRING, NRDP_VERSION_STRING, debugString, productionString, mdxString);
#endif

#ifdef NRDP_COMPILED_RESOURCES
    initializeResources();
#endif
}

NrdApplication::~NrdApplication()
{
#ifdef NRDP_WEBSERVER_ENABLED
    if (mMongoose) {
        mg_stop(mMongoose);
        mMongoose = 0;
    }
#endif
    netflix::nrdlog::InstrumentationThread::shutdown();
    mTelnetServer.reset();
}

std::string NrdApplication::readSystemConfiguration(const std::string &key)
{
    assert(mSecureStore);
    return mSecureStore->getByAccount(SystemKeys::prefix, key);
}

void NrdApplication::writeSystemConfiguration(const std::string &key, const std::string &value, WriteMode mode)
{
    assert(mSecureStore);
    if (!value.empty()) {
        mSecureStore->setByAccount(SystemKeys::prefix, key, value);
    } else {
        mSecureStore->unsetByAccount(SystemKeys::prefix, key);
    }
    if (mode == WriteMode_Flush)
        mSecureStore->flushEncrypted();
}

std::string NrdApplication::readSystemConfiguration(SystemKeys::Key key)
{
    assert(mSecureStore);
    return mSecureStore->get<std::string>(key);
}

void NrdApplication::writeSystemConfiguration(SystemKeys::Key key, const std::string &value, WriteMode mode)
{
    assert(mSecureStore);
    if (!value.empty()) {
        mSecureStore->set(key, value);
    } else {
        mSecureStore->unset(key);
    }
    if (mode == WriteMode_Flush)
        mSecureStore->flushEncrypted();
}

std::string NrdApplication::esnPrefix() const
{
    const std::string esn = getDeviceLib()->getSystem()->getEsn();
    // This ESN prefix was created before there was a standard format.
    if (esn.find("SDK-") == 0)
        return "SDK-";
    return esn.substr(0, 10);
}

std::string NrdApplication::uiLanguages() const
{
    std::string ret;
    const std::vector<std::string> languages = mSystemData->languagePreferences();
    size_t count = languages.size();
    for (size_t i = 0; i < count; i++) {
        if (i)
            ret += ",";
        ret += languages.at(i);
    }
    return ret;
}

std::map<std::string, std::string> NrdApplication::serviceParameters(const std::string &file) const
{
    return mSystemData->serviceParameters(file);
}

void NrdApplication::setAppTrustStore(const std::string &trustStore)
{
    mSystemData->setAppTrustStore(trustStore);
    DataBuffer db = trustStore;
    NRDP_OBJECTCOUNT_DESCRIBE(db, "TRUSTSTORE");
    resourceManager()->setTrustStoreData(db);
    mNrdpBridge->trustStoreUpdated();
}

std::string NrdApplication::appTrustStore() const
{
    return mSystemData->appTrustStore();
}

bool NrdApplication::start()
{
    setStatus(Status_Starting);
    if(!Application::start())
        return false;

    assert(!mDeviceLib);
    mDeviceLib.reset(netflix::device::createDeviceLib());
    assert(mDeviceLib);
    if (!NrdConfiguration::isValid())
        return false;

    mSecureStore.reset(new SecureStore);
    mDiskStore.reset(new DiskStore(mSecureStore));
    mDiskStore->init();

    mSystemData.reset(new SystemData);

    mSystemDataListener.reset(new NrdApplicationSystemDataListener);
    mSystemData->addListener(mSystemDataListener);

    initVersions();

    shared_ptr<NrdpBridge> nrdpBridge;
    {
        ScopedMutex _lock(NfObject::mutex());
        mNrdpBridge.reset(new NrdpBridge);
        nrdpBridge = mNrdpBridge;
    }

    mSystemData->addServiceFile("streaming");
    mSystemData->addServiceFile("networking");

    shared_ptr<StreamingConfigParameter> pStreamingConfigParameter;
    pStreamingConfigParameter.reset(new StreamingConfigParameter);
    shared_ptr<StreamingPersistentStore> pPersistentStore;
    pPersistentStore.reset(new StreamingPersistentStore);

    HttpRequestManager::instance()->configure(pStreamingConfigParameter, pPersistentStore);
#ifdef NRDP_ASE
    NetworkMonitor::instance()->configure(pStreamingConfigParameter, pPersistentStore);
#endif

    mSystemDispatcher.reset(new ISystemEventForwarder);
    mNrdpBridge->init();

    initLogging(); // LogBridge needs to be created first

    // Initialize DeviceLib.
    shared_ptr<ISystem> system = getSystem();
    system->init(mSystemDispatcher);

    NrdIpConnectivityManager *pNrdIpConnectivityManager;
    pNrdIpConnectivityManager = new NrdIpConnectivityManager;
    pNrdIpConnectivityManager->load();

    //Construct the IP connectivity manager.
    IpConnectivityManager::setInstance(pNrdIpConnectivityManager);

    mNrdIpConfigurationListener.reset(new NrdIpConfigurationListener(pNrdIpConnectivityManager));

    mSystemData->addListener(mNrdIpConfigurationListener);

    // Create the global DNS request manager.
    configureDns();
    DnsManager::create();

    assert(mResourceManager);
    mResourceManager->init();

    initDiskCache(readSystemConfiguration(SystemKeys::DiskCache));
    if (!NrdConfiguration::smapDirectory().empty())
        SMAPSThread::init(1);

    NTRACE(TRACE_STARTUP, "ESN = %s", system->getEsn().c_str());

#if defined(BUILD_DEBUG) && HAVE_MALLOC_H
    dumpHeap("start");
#endif

    mNetworkListener.reset(new NetworkListener);
    addSystemListener(mNetworkListener);

    if (shared_ptr<Console> c = console()) {
        c->addCommand(shared_ptr<Console::Command>(new QuitCommand));
        c->addCommand(shared_ptr<Console::Command>(new SuspendCommand));
        c->addCommand(shared_ptr<Console::Command>(new DNSCommand));
        c->addCommand(shared_ptr<Console::Command>(new TrustStoreCommand));
        c->addCommand(shared_ptr<Console::Command>(new NetworkCommand));
        c->addCommand(shared_ptr<Console::Command>(new ResourcesCommand));
        c->addCommand(shared_ptr<Console::Command>(new DiskStoreCommand));
        c->addCommand(shared_ptr<Console::Command>(new WriteLimiterCommand));
#ifdef INSTRUMENTATION_ENABLED
        c->addCommand(shared_ptr<Console::Command>(new InstrumentationCommand));
#endif

        std::vector<shared_ptr<Console::Command> > commands = c->commands();
        std::ostringstream helpCompletions;
        bool first = true;
        for (std::vector<shared_ptr<Console::Command> >::const_iterator it = commands.begin(); it != commands.end(); ++it) {
            if (first) {
                first = false;
            } else {
                helpCompletions << ' ';
            }
            helpCompletions << (*it)->name;
        }

        c->addCommand(shared_ptr<Console::Command>(new HelpCommand(helpCompletions.str())));
    }

    startTimer(shared_ptr<ReseedEntropyTimer>(new ReseedEntropyTimer));

    mThreadPool->init(NrdConfiguration::nbpThreadPoolMaxThreadCount(), &THREAD_NBP_POOL,
                      NrdConfiguration::nbpThreadPoolMinThreadCount(),
                      Time(NrdConfiguration::nbpThreadPoolIdleTime()));
    netflix::nrdlog::InstrumentationThread::start();
    INSTRUMENTATION_EVENT_ON("nrdlib.start", Variant::StringMap
                             ("NRDP", NRDP_VERSION_STRING)
                             ("esn", system->getEsn())
                             ("swversion", system->getSoftwareVersion()));
    if(!isSuspended())
        setStatus(Configuration::suspendModeOnStart().empty() ? Status_Running : Status_Suspended);
    sendEvent(ApplicationEvent::Event_Started);
    setObjectCountTimerInterval(Configuration::objectCountTimerInterval());
    if (const int quitTimer = NrdConfiguration::quitTimerInterval()) {
        startQuitTimer(quitTimer);
    }


    // Boot information
    // ---------------------------------------------------------------------------------
    std::string sstr = mSecureStore->get<std::string>(SystemKeys::nrdStarted);
    Variant bootReport = app()->versionInfo();
    if (sstr.size())
        bootReport["appid"] = sstr;

    std::string crashCount = mSecureStore->get<std::string>(SystemKeys::crashCount);
    if (crashCount.size()) {
        bootReport["crash_count"] = crashCount;
    } else {
        bootReport["crash_count"] = "0";
    }

#if defined(NRDLIB_CRASH_REPORTING_BREAKPAD)
    bootReport["crashReportClient"] = "breakpad";
#elif defined(NRDLIB_CRASH_REPORTING_INTERNAL)
    bootReport["crashReportClient"] = "nflx1";
#elif !defined(CRASH_REPORTING_ENABLED)
    bootReport["crashReportClient"] = "off";
#else
    bootReport["crashReportClient"] = "unknown";
#endif

    Log::Message m(TRACE_SYSTEM, Log::Info, "boot");
    std::string uptime = mSecureStore->get<std::string>(SystemKeys::upTime);

    if (uptime.empty())
        bootReport["previous_uptime"] = 0;
    else
        bootReport["previous_uptime"] = uptime;

    m.m_message = bootReport.toJSON(true);
    m.send();


    return true;
}

NrdApplication::Status NrdApplication::status() const
{
    ScopedMutex lock(mMutex);
    return mStatus;
}

Time NrdApplication::monoApplicationStarted() const
{
    return MONO_STARTED;
}

Time NrdApplication::monoFirstLocationSet() const
{
    return mFirstLocationSet;
}

Time NrdApplication::monoLocationChanged() const
{
    return mLocationChanged;
}

Time NrdApplication::timeSinceApplicationStarted()
{
    return Time::mono() - MONO_STARTED;
}

Time NrdApplication::timeSinceFirstLocationSet()
{
    if (NrdApplication * self = instance())
        return Time::mono() - self->mFirstLocationSet;
    return Time(0);
}

Time NrdApplication::timeSinceLocationChanged()
{
    if (NrdApplication * self = instance())
        return Time::mono() - self->mLocationChanged;
    return Time(0);
}

Variant NrdApplication::send(const Request &request)
{
    enum { MaxErrorLength = 2048 };
    if (request.type() == Request::Invalid) {
        JSONFormatter formatter;
        formatter.setMaxLength(MaxErrorLength);
        Log::warn(TRACE_NBP, "Invalid request: %s", formatter.format(request.variant()).c_str());
        return false;
    }
    shared_ptr<NfObject> object = request.object();
    if (!object) {
        JSONFormatter formatter;
        formatter.setMaxLength(MaxErrorLength);
        bool shutdown = false;
        {
            ScopedMutex lock(NfObject::mutex());
            shutdown = !mNrdpBridge;
        }
        Log::error(TRACE_NBP, shutdown ? "Request during shutdown not delivered %s" : "Protocol error: %s",
                   formatter.format(request.variant()).c_str());
        return false;
    }

    return object->send(request);
}

Variant NrdApplication::post(const Request &request)
{
    if (request.flags() & Request::Synchronous) {
        return send(request);
    } else {
        threadPool()->post(new NBPSendJob(request));
        return Variant();
    }
}

void NrdApplication::setTestDriverIpAddress(const std::string &ip)
{
    if (mTelnetServer)
        return;
    if (ip.empty())
        return;
    const int port = Configuration::telnetPort();
    assert(port);
    if (port) {
        mTelnetServer.reset(new TelnetServer(port));
        mTelnetServer->Start();
    }
#ifdef NRDP_WEBSERVER_ENABLED
    mSecureIP = ip;
    startMongoose(); //if not already running

    if (!mAllInterfacesEnabled && allowAllInterfaces()) {
        NTRACE(TRACE_MONGOOSE, "Enabling all interfaces for nbpd port %lld", NrdConfiguration::nbpdPort());
        mAllInterfacesEnabled = true;
        mg_modify_listening_ports(mMongoose, portsString().c_str());
    }
#endif
}

void NrdApplication::quit(int code)
{
    if (isAppThread()) {
        mQuitCode = code;
        quitImpl();
        return;
    }
    scheduleQuit(code);
}

void NrdApplication::scheduleQuit(int code)
{
    mQuitCode = code;
    callInMainThread(this, &NrdApplication::quitImpl, Event::HighPriority);
}

void NrdApplication::requestSuspend(const std::string &reason)
{
    if(!Configuration::suspendModeEnabled())
        return;
    NTRACE(TRACE_SUSPEND, "NrdApplication::requestSuspend(\"%s\") %d", reason.c_str(), isSuspended());
    if(!isSuspended()) {
        if (shared_ptr<netflix::device::ISystem> system = getSystem())
            system->requestSuspend(reason);
    }
}

void NrdApplication::suspendComplete()
{
    NTRACE(TRACE_SUSPEND, "NrdApplication::suspendComplete() %d", status());
    assert(Configuration::suspendModeEnabled());
    if(status() == NrdApplication::Status_Suspending) {
        setStatus(NrdApplication::Status_Suspended);
        sendEvent(ApplicationEvent::Event_SuspendComplete);
    }
}

void NrdApplication::setSuspended(bool suspend, const Variant &reason)
{
    if(!Configuration::suspendModeEnabled())
        return;
    NTRACE(TRACE_SUSPEND, "NrdApplication::setSuspended(%d, %s)", suspend, reason.toJSON().c_str());
    if(suspend) {
        setStatus(Status_Suspending);
        sendEvent(ApplicationEvent::Event_Suspend, reason);
    } else {
        setStatus(Status_Running);
        sendEvent(ApplicationEvent::Event_Resume, reason);
    }
}

bool NrdApplication::hasQuit() const
{
    ScopedMutex lock(mMutex);
    return mStatus == Status_Stopping || mStatus == Status_Stopped;
}

bool NrdApplication::isSuspended() const
{
    if(!Configuration::suspendModeEnabled())
        return false;
    ScopedMutex lock(mMutex);
    return mStatus == Status_Suspended || mStatus == Status_Suspending;
}

void NrdApplication::quitImpl()
{
#ifdef NRDP_HAS_TRACING
    {
        Status status;
        mMutex.lock();
        status = mStatus;
        mMutex.unlock();
        NTRACE(TRACE_SUSPEND, "quit: start (%d) [%d]", mQuitCode, status);
    }
#endif

    {
        ScopedMutex lock(mMutex);
        switch (mStatus) {
        case Status_Starting:
        case Status_Stopped:
        case Status_NotStarted:
        case Status_Stopping:
            return;
        case Status_Suspending:
        case Status_Suspended:
        case Status_Running:
            mStatus = Status_Stopping;
            break;
        }
    }

    if (shared_ptr<Console> c = console())
        c->stop();

    NTRACE(TRACE_NBP, "quit: sending shutdown event");
    sendEvent(ApplicationEvent::Event_Shutdown); // shutdown nrdapp
    SMAPSThread::shutdown();
    NTRACE(TRACE_NBP, "quit: shutting down resourceManager");
    mResourceManager->shutdown();
    mResourceManager.reset();

    // begin shutting down some threads
    NTRACE(TRACE_NBP, "quit: shutting down nrdp bridge");
    mNrdpBridge->shutdown();
    if (mTelnetServer) {
        NTRACE(TRACE_NBP, "quit: shutting down telnet server");
        mTelnetServer->shutdown();
    }

    NTRACE(TRACE_NBP, "quit: shutting down threadpool");
    mThreadPool->shutdown();
    mThreadPool.reset();
    NTRACE(TRACE_NBP, "quit: clearing event connections");
    clearEventConnections();
    mSecureStore->shutdown();
    NTRACE(TRACE_NBP, "quit: stopping nrdlib");
    mSystemData->removeListener(mSystemDataListener);
    mSecureStore->flushEncrypted();
    DnsManager::logStats(); // explicitly log here before logging becomes impossible
#if defined(BUILD_DEBUG) && HAVE_MALLOC_H
    dumpHeap("quit");
#endif

    shared_ptr<NrdpBridge> nrdpBridge;
    {
        NTRACE(TRACE_NBP, "quit: clearing nfobjects");
        ScopedMutex _lock(NfObject::mutex());
        std::swap(nrdpBridge, mNrdpBridge);
    }
    nrdpBridge.reset();

    NTRACE(TRACE_NBP, "quit: destroying nrdlib");
    mSystemData->removeListener(mNrdIpConfigurationListener);

    mNrdIpConfigurationListener.reset();

    // destroy dns manager
    DnsManager::destroy();

    // Clean up in reverse order.
    getSystem()->shutdown();

    IpConnectivityManager::instance()->store();

    // Destroy the IP connectivity manager.
    IpConnectivityManager::destroy();

    {
        NTRACE(TRACE_NBP, "quit: signalling condition variable");
        ScopedMutex lock(mMutex);
        assert(mStatus == Status_Stopping);
        mStatus = Status_Stopped;
        mCondition.broadcast();
    }
}

int NrdApplication::sendResponse(const Response &response)
{
    int sent = 0;
    std::set<shared_ptr<EventConnection> > copy;
    {
        ScopedMutex locker(mEventConnectionsMutex);
        copy = mEventConnections;
    }

    for (std::set<shared_ptr<EventConnection> >::const_iterator it = copy.begin(); it != copy.end(); ++it) {
        shared_ptr<EventConnection> connection = *it;
        if (response.object() && connection->isFiltered(response.object()->path()))
            continue;
        if (!connection->write(response)) {
            removeEventConnection(connection);
        } else {
            ++sent;
        }
    }
    return sent;
}

static void sync(const shared_ptr<NfObject> &object, const shared_ptr<EventConnection> &connection, std::vector<Response> &responses)
{
    assert(connection);
    assert(!(object->flags() & NfObject::Flag_NoPropertySync));
    if (!connection->isFiltered(object->path())) {
        const Variant props = object->syncProperties();
        if (!props.isNull()) {
            responses.push_back(Response::createPropertiesUpdate(object, props));
        }
    }

    const NfObject::ObjectMap &children = object->children();
    for (NfObject::ObjectMap::const_iterator it = children.begin(); it != children.end(); ++it) {
        if (!(it->second->flags() & NfObject::Flag_NoPropertySync)) {
            sync(it->second, connection, responses);
        }
    }
}

void NrdApplication::addEventConnection(const shared_ptr<EventConnection> &eventConnection)
{
    assert(eventConnection);
    {
        bool ok;
        {
            ScopedMutex locker(mEventConnectionsMutex);
            ok = !mShutdownEventConnections;
            if (ok)
                mEventConnections.insert(eventConnection);
        }
        if (!ok) {
            eventConnection->finish();
            return;
        }
    }
    shared_ptr<NrdpBridge> root;
    {
        ScopedMutex _lock(NfObject::mutex());
        root = mNrdpBridge;
    }
    if (root) {
        std::vector<Response> responses;
        const std::map<std::string, NfObject::ClassInfo> &classes = NfObject::getRegisteredClasses();
        for (std::map<std::string, NfObject::ClassInfo>::const_iterator it = classes.begin(); it != classes.end(); ++it) {
            Variant v(Variant::Type_StringMap);
            if ((*it).second.properties) {
                for (int i = 0; (*it).second.properties[i].name; ++i)
                    v["properties"].push_back((*it).second.properties[i].name);
            }
            if ((*it).second.methods) {
                for (int i = 0; (*it).second.methods[i]; ++i)
                    v["methods"].push_back((*it).second.methods[i]);
            }
            responses.push_back(Response::createClassSync((*it).first, v));
        }
        ::sync(root, eventConnection, responses);
        if (!eventConnection->sync(responses)) {
            removeEventConnection(eventConnection);
            eventConnection->finish();
        }
    }
}

void NrdApplication::clearEventConnections()
{
    std::set<shared_ptr<EventConnection> > eventConnections;
    {
        ScopedMutex locker(mEventConnectionsMutex);
        mShutdownEventConnections = true;
        std::swap(eventConnections, mEventConnections);
    }
    for (std::set<shared_ptr<EventConnection> >::const_iterator it = eventConnections.begin(); it != eventConnections.end(); ++it) {
        (*it)->finish();
    }
}

void NrdApplication::removeEventConnection(const shared_ptr<EventConnection> &eventConnection)
{
    assert(eventConnection);
    {
        ScopedMutex locker(mEventConnectionsMutex);
        mEventConnections.erase(eventConnection);
    }
    eventConnection->finish();
}

shared_ptr<NfObject> NrdApplication::findObject(const std::string &path)
{
    char buf[1024];
    const size_t size = path.size();
    if (!startsWith(path, "nrdp", 4) || size > sizeof(buf) - 1)
        return shared_ptr<NfObject>();

    if (size == 4)
        return mNrdpBridge;

    if (path[4] != '.')
        return shared_ptr<NfObject>();

    assert(size < 1024);
    memcpy(buf, path.c_str() + 5, path.size() - 4);
    char *ptr = buf;
    shared_ptr<NfObject> object = mNrdpBridge;
    while (true) {
        char *dot = strchr(ptr, '.');
        if (dot)
            *dot = '\0';

        const NfObject::ObjectMap::const_iterator it = object->mChildren.find(ptr);
        if (it == object->mChildren.end()) {
            break;
        } else if (!dot) {
            return it->second;
        }
        object = it->second;
        ptr = dot + 1;
    }
    return shared_ptr<NfObject>();
}

Variant NrdApplication::versionInfo() const
{
    Variant map = Application::versionInfo();

    ScopedMutex _lock(mMutex);
    // Return information about third party libraries
    for (Variant::StringMapIterator it = mVersion.libraries.begin();
         it != mVersion.libraries.end(); ++it) {
        const Variant &library = (*it).second;
        if (library["component"].string() == "nrdlib")
            map["libraries"].push_back(library);
        else
            map["uiLibraries"].push_back(library);
    }
    for (std::map<std::string, Variant>::const_iterator it = mVersion.components.begin();
         it != mVersion.components.end(); ++it)
        map["components"][it->first] = it->second;
    return map;
}

void NrdApplication::checkLibraryVersions()
{
    std::map<std::string, Variant> libraries;
    {
        ScopedMutex _lock(mMutex);
        libraries = mVersion.libraries;
    }
    for (Variant::StringMapIterator it = libraries.begin(); it != libraries.end(); ++it) {
        const Variant &library = it->second;

        Log::info(TRACE_STARTUP, "%s %s (compiled: %s)", library["name"].string().c_str(),
                  library["runtime"].string().c_str(), library["compiled"].string().c_str());
        if (!library["runtime"].string().empty() && !library["compiled"].string().empty() &&
            library["runtime"] != library["compiled"]) {
            Log::error(TRACE_STARTUP, "Library '%s' reports version '%s' but '%s' was used when compiling",
                       library["name"].string().c_str(), library["runtime"].string().c_str(),
                       library["compiled"].string().c_str());
        }
    }
}

void NrdApplication::addLibraryInfo(const Variant &data)
{
    if (data.isNull())
        return;
    assert(data["name"].isString());
    assert(data["compiled"].isString());
    assert(data["runtime"].isString());
    shared_ptr<DeviceBridge> device;
    {
        ScopedMutex _lock(mMutex);
        mVersion.libraries[data["name"].string()] = data;
        if(mNrdpBridge && mNrdpBridge->deviceBridge)
            device = mNrdpBridge->deviceBridge;
    }
    if(device)
        device->updateSDKVersion();
}

bool NrdApplication::waitForStatus(Status status, Time *time) const
{
    ScopedMutex lock(mMutex);
    while(mStatus != status && mStatus != Status_Stopped) {
        if(mCondition.wait(&mMutex, time) == NFErr_TimedOut)
            break;
    }
    return mStatus == status;
}

void NrdApplication::setStatus(Status status)
{
    ScopedMutex lock(mMutex);
    mStatus = status;
    mCondition.broadcast();
}

void NrdApplication::setHasSuspendHandler(bool s)
{
    ScopedMutex lock(mMutex);
    mHasSuspendHandlers = s;
}

bool NrdApplication::hasSuspendHandler() const
{
    ScopedMutex lock(mMutex);
    return mHasSuspendHandlers;
}

void NrdApplication::initVersions()
{
    mVersion.components["nrdp"] = NRDP_VERSION_STRING;
    mVersion.components["nrdlib"] = NRDP_VERSION_STRING;
    mVersion.components["nrdapp"] = NRDP_VERSION_STRING;
#ifdef NRDP_MDX_ENABLED
    mVersion.components["mdxlib"] = NRDP_VERSION_STRING;
#endif


    Variant bundleVersion = Configuration::configDataValues().value("bundleVersion");

    const std::string platformVersion = bundleVersion["platformVersion"].value<std::string>();
    if (platformVersion.empty()) {
        bundleVersion["platformVersion"] = NRDP_VERSION_STRING;
    }

    //mVersion.components["sdk"] = bundleVersion["platformVersion"];
    mVersion.components["platform"] = bundleVersion;

    { //openssl
        Variant library;
        library["name"] = "openssl";
        library["component"] = "nrdlib";
        library["runtime"] = SSLeay_version(SSLEAY_VERSION);
        library["compiled"] = OPENSSL_VERSION_TEXT;
        library["options"] = SSLeay_version(SSLEAY_CFLAGS);
        addLibraryInfo(library);
    }
    { //curl
        Variant library;
        library[ "name" ] = "libcurl";
        library["component"] = "nrdlib";
        curl_version_info_data* cv = curl_version_info(CURLVERSION_NOW);
        library["runtime"] = cv->version;
        library["compiled"] = LIBCURL_VERSION;

        std::ostringstream versionString;
        // -------------------------------------------------------
#define CURL_FEATURE_CHECK(feature)                                     \
        do {                                                            \
            if (cv->features & CURL_VERSION_ ## feature) {              \
                if ( versionString.str().length() != 0 )                \
                    versionString << " ";                               \
                versionString << #feature;                              \
            }                                                           \
        } while (0)                                                     \
            // -------------------------------------------------------
        CURL_FEATURE_CHECK(IPV6);
        CURL_FEATURE_CHECK(KERBEROS4);
        CURL_FEATURE_CHECK(SSL);
        CURL_FEATURE_CHECK(LIBZ);
        if (cv->features & CURL_VERSION_LIBZ)
            versionString << " (" << cv->libz_version << ")";
        CURL_FEATURE_CHECK(NTLM);
        CURL_FEATURE_CHECK(GSSNEGOTIATE);
        CURL_FEATURE_CHECK(DEBUG);
        CURL_FEATURE_CHECK(SPNEGO);
        CURL_FEATURE_CHECK(LARGEFILE);
        CURL_FEATURE_CHECK(IDN);
        CURL_FEATURE_CHECK(SSPI);
        CURL_FEATURE_CHECK(CONV);
        CURL_FEATURE_CHECK(CURLDEBUG);
#ifdef CURL_VERSION_ASYNCHDNS //Might not be present if libcurl is a bit old
        CURL_FEATURE_CHECK(ASYNCHDNS);
#endif
        if (cv->ares)
            versionString << " (ares " << cv->ares << ")";
        for (int i = 0; cv->protocols[i]; ++i)
            versionString << " " << cv->protocols[i];

        library["options"] = versionString.str();
        addLibraryInfo(library);
    }
    { //cares
        Variant library;
        library["name"] = "cares";
        library["component"] = "nrdlib";
        library["runtime"] = ares_version(0);
        library["compiled"] = ARES_VERSION_STR;
        addLibraryInfo(library);
    }
    { //libz
        Variant library;
        library["name"] = "libz";
        library["component"] = "nrdlib";
        library["runtime"] = zlibVersion();
        library["compiled"] = ZLIB_VERSION;
        addLibraryInfo(library);
    }

    { //expat
        Variant library;
        library[ "name" ] = "expat";
        library["component"] = "nrdlib";

        std::ostringstream versionString;
        XML_Expat_Version v = XML_ExpatVersionInfo();
        versionString << v.major << "." << v.minor << "." << v.micro;
        library["runtime"] = versionString.str();

        versionString.str("");
        versionString << XML_MAJOR_VERSION << "." << XML_MINOR_VERSION << "." << XML_MICRO_VERSION;
        library[ "compiled" ] = versionString.str();

        addLibraryInfo(library);
    }
}

class QuitTimer : public Application::Timer
{
public:
    inline QuitTimer(int interval) : Application::Timer(interval)
    {
        setSingleShot(true);
    }

    virtual std::string describe() const { return "QuitTimer"; }

    virtual void timerFired()
    {
        PERFORMANCE_MARKER_SCOPED("quittimer.fired");
        nrdApp()->quit();
    }
};

void NrdApplication::startQuitTimer(int interval)
{
    assert(interval > 0);
    shared_ptr<QuitTimer> quitTimer(new QuitTimer(interval));
    quitTimer->start();
}

void NrdApplication::addEventListener(const shared_ptr<ApplicationEventListener> &listener)
{
    assert(listener);
    ScopedMutex lock(mMutex);
    mEventListeners.insert(listener);
}

void NrdApplication::removeEventListener(const shared_ptr<ApplicationEventListener> &listener)
{
    ScopedMutex lock(mMutex);
    mEventListeners.erase(listener);
}

void NrdApplication::sendEvent(ApplicationEvent::Type type, const Variant &data)
{
    event(type, data);
    std::vector<shared_ptr<ApplicationEventListener> > listeners;
    {
        ScopedMutex lock(mMutex);
        mEventListeners.lock(listeners);
    }
    for (std::vector<shared_ptr<ApplicationEventListener> >::const_iterator it = listeners.begin(); it != listeners.end(); ++it)
        (*it)->event(type, data);
}

void NrdApplication::event(ApplicationEvent::Type event, const Variant &data)
{
    switch (event) {
    case ApplicationEvent::Event_UrlChanged:
        mLocationChanged = Time::mono();
        if (mFirstLocationSet.val() == 0)
            mFirstLocationSet = mLocationChanged;
        break;
    case ApplicationEvent::Event_SuspendComplete: {
        Log::Message msg(TRACE_SYSTEM, Log::Info, "suspend");
        msg.m_message = "NRDP Suspended";
        msg.m_flags |= (Log::Message::Critical | Log::Message::SendToAppBoot);
        msg.send();
        break; }
    case ApplicationEvent::Event_Suspend: {
        Log::Message msg(TRACE_SYSTEM, Log::Info, "suspend");
        msg.m_message = "NRDP Suspend";
        msg.m_tags["reason"] = data.toJSON();
        msg.m_flags |= (Log::Message::Critical | Log::Message::SendToAppBoot);
        msg.send();
        break; }
    case ApplicationEvent::Event_Resume: {
        Log::Message msg(TRACE_SYSTEM, Log::Info, "resume");
        msg.m_message = "NRDP Resume";
        msg.m_tags["reason"] = data.toJSON();
        msg.m_flags |= (Log::Message::Critical | Log::Message::SendToAppBoot);
        msg.send();
        DnsManager::restartResolver();
        break; }
    case ApplicationEvent::Event_FactoryReset:
        // clear everything from the configuration store
        mSecureStore->clear();
        // Delete the network cache files
        resourceManager()->clear();
        if (shared_ptr<DiskCache> diskCache = resourceManager()->diskCache())
            diskCache->factoryReset();
        break;
    case ApplicationEvent::Event_ServerTimeChanged:
        if (shared_ptr<NrdDiskCache> diskCache = static_pointer_cast<NrdDiskCache>(resourceManager()->diskCache()))
            diskCache->onTimeChanged();
        break;
    case ApplicationEvent::Event_Started:
    case ApplicationEvent::Event_Shutdown:
    case ApplicationEvent::Event_PlaybackEntered:
    case ApplicationEvent::Event_PlaybackExited:
    case ApplicationEvent::Event_UrlAboutToChange:
        break;
    case ApplicationEvent::Event_SystemDataDatumChanged:
        if (data.value<int>() == SystemDataKey::SYSTEM_FILE) {
            const std::map<std::string, std::string> params(nrdApp()->serviceParameters("streaming"));

            DnsManager::setGroupName(mapValue<std::string>(params, "groupName"));

            shared_ptr<StreamingConfigParameter> pStreamingConfigParameter;
            pStreamingConfigParameter.reset(new StreamingConfigParameter);
            /** TODO do not configure persistent store when system data changes */
            shared_ptr<StreamingPersistentStore> pPersistentStore;
            pPersistentStore.reset(new StreamingPersistentStore);

            HttpRequestManager::instance()->configure( pStreamingConfigParameter, pPersistentStore );
#ifdef NRDP_ASE
            NetworkMonitor::instance()->configure( pStreamingConfigParameter, pPersistentStore );
#endif
            const std::map<std::string, std::string> networking_params = serviceParameters("networking");
            const std::map<std::string, std::string> streaming_params = serviceParameters("streaming");
            writeSystemConfiguration(SystemKeys::ScriptEngine, mapValue<std::string, std::string>(streaming_params, "gibbon_script"));
            if (networking_params.find("disk_cache") != networking_params.end()) {//try networking params first
                writeSystemConfiguration(SystemKeys::DiskCache, mapValue<std::string, std::string>(networking_params, "disk_cache"));
            } else { //fallback to streaming params
                writeSystemConfiguration(SystemKeys::DiskCache, mapValue<std::string, std::string>(streaming_params, "gibbon_disk_cache"));
            }
        }
        break;
    }
}

bool NrdApplication::isPlayerOpen() const
{
    if (shared_ptr<mediacontrol::IAdaptiveStreamingPlayer> p = player()) {
        switch (p->getState()) {
        case mediacontrol::IAdaptiveStreamingPlayer::OPENING:
        case mediacontrol::IAdaptiveStreamingPlayer::PLAYING:
            return true;
        case mediacontrol::IAdaptiveStreamingPlayer::PAUSED:
        case mediacontrol::IAdaptiveStreamingPlayer::STOPPED:
        case mediacontrol::IAdaptiveStreamingPlayer::CLOSED:
            return false;
        }
    }
    return false;
}

shared_ptr<IAdaptiveStreamingPlayer> NrdApplication::player() const
{
    ScopedMutex lock(mPlayerMutex);
    return mPlayer;
}

shared_ptr<mediacontrol::IMediaSourcePlayer> NrdApplication::player(const std::string &name) const
{
    ScopedMutex lock(mPlayerMutex);
    std::map<std::string, shared_ptr<mediacontrol::IMediaSourcePlayer> >::const_iterator it = mPlayers.find(name);
    if(it != mPlayers.end())
        return it->second;
    return shared_ptr<mediacontrol::IMediaSourcePlayer>();
}

void NrdApplication::setPlayer(const std::string &name, const shared_ptr<mediacontrol::IMediaSourcePlayer> &player)
{
    ScopedMutex lock(mPlayerMutex);
    if(player) {
        mPlayers[name] = player;
    } else {
        std::map<std::string, shared_ptr<mediacontrol::IMediaSourcePlayer> >::iterator it = mPlayers.find(name);
        if(it != mPlayers.end())
            mPlayers.erase(it);
    }
}

void NrdApplication::setPlayer(const shared_ptr<netflix::mediacontrol::IAdaptiveStreamingPlayer> &player)
{
    ScopedMutex lock(mPlayerMutex);
//  TODO: recover the assert once finding a good way of creating NrdpVideo
//  assert(!mPlayer);
    mPlayer = player;
}

void NrdApplication::setPlayerType(enum PlayerType type)
{
    mPlayerType = type;
}

enum PlayerType NrdApplication::playerType()
{
    return mPlayerType;
}

void NrdApplication::addSystemListener(const shared_ptr<ISystem::EventDispatcher> &listener)
{
    assert(mSystemDispatcher);
    mSystemDispatcher->addDispatcher(listener);
}

void NrdApplication::removeSystemListener(const shared_ptr<netflix::device::ISystem::EventDispatcher> &listener)
{
    assert(mSystemDispatcher);
    mSystemDispatcher->removeDispatcher(listener);
}

void NrdApplication::sendSystemCommand(const std::string &command)
{
    if(mSystemDispatcher)
        mSystemDispatcher->commandReceived(command);
}

void NrdApplication::configureDns()
{
    IpConnectivityManager *ipConnectivityManager = IpConnectivityManager::instance();
    assert(ipConnectivityManager);
    if (ipConnectivityManager) {
        DnsManager::Configuration dnsConfig;
        dnsConfig.mServers = ipConnectivityManager->dnsServers();
        dnsConfig.mParallelLookups = ipConnectivityManager->getDnsParallelLookupFlag();
        dnsConfig.mLookupTimeout = AseTimeVal::fromMS(ipConnectivityManager->getDnsLookupTimeout());
        dnsConfig.mLookupRetries = ipConnectivityManager->getDnsLookupRetryCount();
        dnsConfig.mCacheMaxEntries = ipConnectivityManager->getDnsCacheEntryLimit();
        dnsConfig.mCacheMaxTime = AseTimeVal::fromMS(ipConnectivityManager->getDnsCacheTimeout());
        dnsConfig.mTtlMinTime = AseTimeVal::fromMS(ipConnectivityManager->getDnsMinimumTtl());
        dnsConfig.mFallbackServers = ipConnectivityManager->dnsFallbackServers();
        dnsConfig.mFallbackDelay = AseTimeVal::fromMS(ipConnectivityManager->getDnsFallbackDelay());
        dnsConfig.mLogLookups = ipConnectivityManager->getDnsLogLookupsFlag();
        dnsConfig.mLogQueries = ipConnectivityManager->getDnsLogQueriesFlag();
        dnsConfig.mPrefetchHosts = ipConnectivityManager->getDnsPrefetchHosts();
        dnsConfig.mConsecutiveFailuresLimit = ipConnectivityManager->getDnsConsecutiveFailuresLimit();
        if (dnsConfig.mServers.empty())
            dnsConfig.mServers = getSystem()->getDNSList();
#if !defined(BUILD_PRODUCTION)
        if (!Configuration::dnsServers().empty()) {
            dnsConfig.mServers.clear();
            StringTokenizer::tokenize(Configuration::dnsServers(), dnsConfig.mServers, ";");
        }
        if (!Configuration::dnsFallbackServers().empty()) {
            dnsConfig.mFallbackServers.clear();
            StringTokenizer::tokenize(Configuration::dnsFallbackServers(), dnsConfig.mFallbackServers, ";");
        }
        if (!Configuration::dnsCacheEnabled()) {
            dnsConfig.mCacheMaxEntries = 0;
            dnsConfig.mCacheMaxTime = 0;
        }
#endif
        DnsManager::setConfiguration(dnsConfig);
    }
}
void NrdApplication::useFalbackDnsConfig(bool fallback)
{
    NTRACE(TRACE_DNS_MANAGER, "useFalbackDnsConfig[%d]", fallback);
    if (fallback) {
        std::vector<std::string> dnsServers;
        DnsManager::Configuration dnsConfig;
        DnsManager::getConfiguration(dnsConfig);
        dnsConfig.mServers = dnsConfig.mFallbackServers;
        dnsConfig.mFallbackServers = dnsServers;
        DnsManager::setConfiguration(dnsConfig);
    } else {
        configureDns();
    }
}
void NrdApplication::updateDeviceBridgeNetworkProperty()
{
    if(mNrdpBridge && mNrdpBridge->deviceBridge)
        mNrdpBridge->deviceBridge->updateNetworkProperty();
}

void NrdApplication::setServerTime(const Time &time)
{
    Time::setServerTime(time);
    Log::warn(TRACE_SYSTEM) << "servertime=" << time << ",systemtime=" << Time::systemTime().seconds();
    sendEvent(ApplicationEvent::Event_ServerTimeChanged);
}

void NrdApplication::initDiskCache(const std::string &spec)
{
    if (!Configuration::diskCacheCapacity()) {
        assert(!resourceManager()->diskCache());
        return;
    }

    llong capacity = Configuration::diskCacheCapacity();
    llong writeLimit = Configuration::diskCacheWriteLimit();
    llong maxPending = Configuration::diskCacheMaxPending();
    llong catalogTimer = Configuration::diskCacheCatalogTimer();

    bool ok = false;
    if (!spec.empty()) {
        const llong cap = Configuration::parseInt(spec.c_str(), ConfigurationOption::SIZE_ARGUMENT, &ok);
        if(ok)
            capacity = cap;
    }
    if (!ok) {
        const Variant var = Variant::fromJSON(spec);

        switch (var.type()) {
        case Variant::Type_StringMap:
            capacity = var.mapValue<int>("capacity", 0, capacity);
            writeLimit = var.mapValue<int>("writeLimit", 0, writeLimit);
            maxPending = var.mapValue<int>("maxPending", 0, maxPending);
            catalogTimer = var.mapValue<int>("catalogTimer", 0, catalogTimer);
            break;
        case Variant::Type_Null:
        case Variant::Type_Integer:
            break;
        default:
            Log::warn(TRACE_DISK_CACHE, "Unexpected spec \"%s\" => %d", spec.c_str(), var.type());
            break;
        }
    }

    if (capacity) {
        capacity = std::min(capacity, Configuration::diskCacheCapacity());
        if (maxPending == -1)
            maxPending = capacity / 2;
        if (Configuration::diskCacheMaxPending() != -1)
            maxPending = std::min(maxPending, Configuration::diskCacheMaxPending());
        if (Configuration::diskCacheWriteLimit() != -1)
            writeLimit = std::min(writeLimit, Configuration::diskCacheWriteLimit());
    }

    resourceManager()->configureDiskCache(capacity, maxPending, catalogTimer);
    if (shared_ptr<NrdDiskCache> cache = static_pointer_cast<NrdDiskCache>(resourceManager()->diskCache())) {
        cache->setLimit(writeLimit);
    }
}

shared_ptr<ResourceManager> NrdApplication::createResourceManager()
{
    return shared_ptr<ResourceManager>(new ResourceManager);
}

DataBuffer NrdApplication::dataList(const std::vector<DataListEntry> &entries) const
{
    std::ostringstream stream;
    stream << "<html><head><title>Index</title>\n"
           << "<style>\n"
           << "  th {text-align: left;}\n"
           << "  iframe { position: absolute; right: 0px; bottom: 0px;}\n"
           << "</style></head>\n"
           << "<body><h1>Index</h1>\n"
           << "<iframe scrolling=\"no\" frameborder=\"0\" name=\"swmapfr\" id=\"swmapfr\"></iframe>\n"
           << "<script>\n"
           << "  function showPreview(url) { document.getElementById('swmapfr').src = url;}\n"
           << "  function clearPreview() { document.getElementById('swmapfr').src = ''; }\n"
           << "</script>\n"
           << "<table style=\"table-layout: fixed; width: 100%\" cellpadding=\"0\">\n"
           << "  <tr><th width=\"90%\">Name</th>"
           << "<th align=\"right\">Size</th>"
           << "</tr>"
           << "  <tr><td colspan=\"2\"><hr></td></tr>\n";
    for(std::vector<DataListEntry>::const_iterator it = entries.begin(); it != entries.end(); ++it) {
        const DataListEntry &entry = (*it);
        stream << " <tr>"
               << "  <td width=\"90%\"><a onmouseover='showPreview(\"" << entry.href << "\")' onMouseOut=\"clearPreview();\""
               << "         href=\"" << entry.href << "\">" << entry.name << "</a></td>"
               << "  <td align=\"right\">&nbsp;";
        if(entry.size < 1024)
            stream << entry.size;
        else if(entry.size < 1024 * 1024)
            stream << double(entry.size) / 1024.0 << "k";
        else if (entry.size < 1024 * 1024 * 1024)
            stream << double(entry.size) / 1048576 << "M";
        else
            stream << double(entry.size) / 1073741824 << "G";
        stream << "  </td></td>";
        stream << " </tr>\n";
    }
    stream << "</table></body></html>\n";
    return DataBuffer(stream.str());
}

DataBuffer NrdApplication::resourceContent(const std::string &path, const Variant &params, Time *modificationTime) const
{
    DataBuffer content;
    if (!path.empty()) {
        const resources::ResourceEntry *entry = resources::getResourceEntry("resources/" + path);
        Time mTime;
        if (modificationTime)
            *modificationTime = Time();
        if (entry) {
            const ullong resourceSize = entry->end - entry->start;
            if (params.isNull()) {
                content = DataBuffer::fromRawData(reinterpret_cast<const unsigned char*>(entry->start), resourceSize);
            } else { // if we're going to modify the content we have to detach
                content.append(reinterpret_cast<const unsigned char*>(entry->start), resourceSize);
            }
        } else {
            const std::string p = Configuration::resourcesPath() + path;
            struct stat statBuf;
            memset(&statBuf, 0, sizeof(statBuf));
            if (!::stat(p.c_str(), &statBuf) && S_ISREG(statBuf.st_mode)) {
                if (modificationTime)
                    *modificationTime = Time::fromSeconds(statBuf.st_mtime);
                content = DataBuffer::fromFile(p.c_str());
            }
        }
        if (!content.isEmpty() && !params.isNull())
            Configuration::replaceTokens(content, params);
    }
    return content;
}

#ifndef NRDP_WEBSERVER_ENABLED
bool NrdApplication::startMongoose()
{
    return false;
}
#else
bool NrdApplication::startMongoose()
{
    ScopedMutex lock(mHttpMutex);
    if (!mMongoose) {
        std::map<std::string, std::string> configs;
        configs["listening_ports"] = portsString();
        configs["document_root"] = Configuration::resourcesPath();
        configs["enable_directory_listing"] = "no";
        configs["num_threads"] = "2";
        configs["max_threads"] = "10";
        configs["enable_keep_alive"] = "yes";
        //configs["max_request_size"] = "4096";

        const Variant &values = Configuration::configDataValues();
        const Variant::StringMapIterator end = values.stringMapEnd();
        for (Variant::StringMapIterator it = values.stringMapBegin(); it != end; ++it) {
            const std::string &key = (*it).first;
            if (key.size() > 3 && !key.compare(0, 3, "mg_"))
                configs[key.substr(3)] = (*it).second.string();
        }
        std::vector<const char *> mongooseOptions;
        mongooseOptions.reserve((configs.size() * 2) + 1);
        for (std::map<std::string, std::string>::const_iterator it = configs.begin(); it != configs.end(); ++it) {
            mongooseOptions.push_back((*it).first.c_str());
            mongooseOptions.push_back((*it).second.c_str());
            NTRACE(TRACE_MONGOOSE, "Mongoose option: %s = %s", (*it).first.c_str(), (*it).second.c_str());
        }
        mongooseOptions.push_back(0);
        mMongoose = mg_start(s_mongooseCallback, this, &mongooseOptions[0]);
        if (!mMongoose) {
            Log::error(TRACE_MONGOOSE, "Can't start mongoose - Check that there is not another instance of the application running");
            return false;
        }
    }
    return true;
}

void* NrdApplication::s_mongooseCallback(enum mg_event event, struct mg_connection* conn, const struct mg_request_info* requestInfo)
{
    return static_cast<NrdApplication*>(requestInfo->user_data)->mongooseCallback(event, conn, requestInfo);
}

std::string NrdApplication::portsString() const
{
    const Variant &configDataValues = Configuration::configDataValues();
    bool ok;
    const std::string port = configDataValues.mapValue<std::string>("mg_listening_ports", &ok);
    if (ok)
        return port;

    std::ostringstream os;
    if (!mAllInterfacesEnabled)
        os << "127.0.0.1:";
    os << NrdConfiguration::nbpdPort();
    return os.str();
}

static inline std::string readPostData(struct mg_connection *conn)
{
    const char *contentLengthString = mg_get_header(conn, "Content-Length");
    const int contentLength = (contentLengthString ? strtoull(contentLengthString, 0, 10) : 0);
    std::string ret;
    if (contentLength) {
        ret.resize(contentLength);
        const int read = mg_read(conn, &ret[0], contentLength);
        assert(read >= 0 && read <= contentLength);
        (void)read; // avoid warnings in release
    }
    return ret;
}

void NrdApplication::handlePing(struct mg_connection *conn)
{
    writeDataF(conn,
               "HTTP/1.1 200 OK\r\n"
               "Access-Control-Allow-Origin: *\r\n"
               "Cache: no-cache\r\n"
               "Content-Type: text/plain\r\n"
               "Connection: keep-alive\r\n"
               "Date: %s\r\n"
               "Content-Length: 4\r\n"
               "X-NBP-ESN: %s\r\n"
               "X-NBP-ESNPrefix: %s\r\n"
               "X-NBP-Language: %s\r\n"
               "X-NBP-UILanguages: %s\r\n\r\n"
               "pong",
               Time::serverTime().calendarTimeText().c_str(),
               UrlEncoder::encode(getSystem()->getEsn()).c_str(),
               UrlEncoder::encode(esnPrefix()).c_str(),
               UrlEncoder::encode(getSystem()->getLanguage()).c_str(),
               UrlEncoder::encode(uiLanguages()).c_str());
}

bool NrdApplication::handleResource(struct mg_connection *conn, const mg_request_info *requestInfo, bool raw)
{
    Variant params;
    if (requestInfo->query_string)
        params = Variant::fromQueryString(requestInfo->query_string);

    // duped in GibbonApplication, refactor
    const DataBuffer content = resourceContent(requestInfo->uri, params);
    if (!content.isEmpty()) {
        const std::string mimetype = ResourceManager::mimeType(AseUrl(requestInfo->uri));
        if (!raw) {
            NrdApplication::writeDataF(conn,
                                       "HTTP/1.1 200 OK\r\n"
                                       "Date: %s\r\n"
                                       "Access-Control-Allow-Origin: *\r\n"
                                       "Cache: no-cache\r\n"
                                       "Content-Type: %s\r\n"
                                       "Connection: keep-alive\r\n"
                                       "Content-Length: %d\r\n\r\n",
                                       Time::serverTime().calendarTimeText().c_str(), mimetype.empty() ? "test/html" : mimetype.c_str(),
                                       content.size());
        }
        writeData(conn, content.data<const char>(), content.size());
        return true;
    }
    return false;
}

void NrdApplication::handleUiBoot(struct mg_connection *conn, const struct mg_request_info *)
{
    const std::string url = NrdConfiguration::bootRequest().mapValue<std::string>("url");
    const char *extraHeader = "";

    NrdApplication::writeDataF(conn,
                               "HTTP/1.1 302 Moved Temporarily\r\n"
                               "Cache: no-cache\r\n"
                               "Cache-Control: private\r\n"
                               "Connection: keep-alive\r\n"
                               "Content-Length: 0\r\n"
                               "Location: %s\r\n"
                               "%s"
                               "Date: %s\r\n"
                               "X-NBP-ESN: %s\r\n"
                               "X-NBP-ESNPrefix: %s\r\n"
                               "X-NBP-Language: %s\r\n"
                               "X-NBP-UILanguages: %s\r\n\r\n",
                               url.c_str(),
                               extraHeader,
                               Time::serverTime().calendarTimeText().c_str(),
                               UrlEncoder::encode(getSystem()->getEsn()).c_str(),
                               UrlEncoder::encode(esnPrefix()).c_str(),
                               UrlEncoder::encode(getSystem()->getLanguage()).c_str(),
                               UrlEncoder::encode(uiLanguages()).c_str());

    NTRACE(TRACE_MONGOOSE, "UIBoot redirect: %s", url.c_str());
}

static bool matchUrl(const char *left, const char *right)
{
    if (!strcmp(left, right))
        return true;
    const size_t len = strlen(right);
    if (strlen(left) - 1 == len && left[len] == '/' && !strncmp(left, right, len)) {
        return true;
    }
    return false;
}

void* NrdApplication::mongooseCallback(enum mg_event ev, struct mg_connection* conn,
                                       const struct mg_request_info* requestInfo)
{
#ifdef BUILD_DEBUG
    if(requestInfo) {
        NTRACE(TRACE_MONGOOSE_REQUEST, "Got mongoose event (%d) uri: %s method: %s query: %s",
               ev, requestInfo->uri, requestInfo->request_method, requestInfo->query_string);
        for (int i=0; i<requestInfo->num_headers; ++i) {
            NTRACE(TRACE_MONGOOSE_REQUEST, "header: %d/%d: %s: %s", i, requestInfo->num_headers,
                   requestInfo->http_headers[i].name, requestInfo->http_headers[i].value);
        }
    }
#endif

    static void* const handled = reinterpret_cast<void*>(1);
    static void* const notHandled = 0;

    switch (ev) {
    case MG_EVENT_LOG:
        assert(requestInfo->log_message);
        NTRACE(TRACE_MONGOOSE, "Mongoose log message: [%s]", requestInfo->log_message);
        return handled;
    case MG_INIT_THREAD: {
        Thread* t = Thread::Adopt(&THREAD_NBPD_MONGOOSE);
        if (t)
            t->SetDeleteSelf(true);
        break; }
    case MG_CLEANUP_THREAD:
        Thread::Disown();
        break;
    case MG_NEW_REQUEST:
        if (strcmp(requestInfo->request_method, "POST")
            && strcmp(requestInfo->request_method, "GET")
            && strcmp(requestInfo->request_method, "HEAD")) {
            return notHandled;
        }

        {
            //Send the continue if the client expects one
            const char* expectString = mg_get_header(conn, "Expect");
            if (expectString && !strcmp(expectString, "100-continue")) {
                static const char* ok_continue =
                    "HTTP/1.1 100 Continue\r\n"
                    "\r\n";
                static const int okContinueLength = strlen(ok_continue);
                NrdApplication::writeData(conn, ok_continue, okContinueLength);
            }
        }

        if (matchUrl(requestInfo->uri, "/ipc")) {
            handleIpc(conn, requestInfo);
            return handled;
        }

        if (matchUrl(requestInfo->uri, "/ping")) {
            handlePing(conn);
            return handled;
        }

        if (matchUrl(requestInfo->uri, "/uiboot") || matchUrl(requestInfo->uri, "/")) {
            handleUiBoot(conn, requestInfo);
            return handled;
        }

        if (strstr(requestInfo->uri, "..")) {
            handleForbidden(conn, 403);
            return handled;
        }

        if (!strncmp(requestInfo->uri, "/cat", 4) && requestInfo->query_string) {
            if (FILE *f = fopen(requestInfo->query_string, "r")) {
                NrdApplication::writeDataF(conn,
                                           "HTTP/1.1 200 OK\r\n"
                                           "Date: %s\r\n"
                                           "Access-Control-Allow-Origin: *\r\n"
                                           "Transfer-Encoding: chunked\r\n"
                                           "Cache: no-cache\r\n\r\n",
                                           Time::serverTime().calendarTimeText().c_str(), requestInfo->query_string);
                char buffer[2048];
                for (int line = 1; fgets(buffer, sizeof(buffer), f); ++line) {
                    const int len = strlen(buffer);
                    NrdApplication::writeDataF(conn, "%x\r\n", len);
                    writeData(conn, buffer, len);
                    NrdApplication::writeData(conn, "\r\n", 2);
                }
                NrdApplication::writeDataF(conn, "0\r\n\r\n", 5);
                fclose(f);
            } else {
                handleForbidden(conn, 404);
            }
            return handled;
        }

        if (!strncmp(requestInfo->uri, "/local/", 7)) {
            return notHandled; // serve resources using mongoose's built-in mechanism
        }

        if (!strncmp(requestInfo->uri, "/http/", 6)) {
            if(handleResource(conn, requestInfo, true))
                return handled;
            return notHandled; // serve resources using mongoose's built-in mechanism
        }

        if (!strcmp(requestInfo->uri, "/eventsource") && handleEventSource(conn, requestInfo))
            return handled;

        {
            const DataBuffer data = dataURI(requestInfo->uri);
            if (!data.isEmpty()) {
                NrdApplication::writeDataF(conn,
                                           "HTTP/1.1 200 OK\r\n"
                                           "Date: %s\r\n"
                                           "Access-Control-Allow-Origin: *\r\n"
                                           "Cache: no-cache\r\n"
                                           "Connection: keep-alive\r\n"
                                           "Content-Length: %d\r\n\r\n",
                                           Time::serverTime().calendarTimeText().c_str(),
                                           data.size());
                writeData(conn, (const char*)data.data(), data.size());
                return handled;
            }
        }
        if (matchUrl(requestInfo->uri, "/favicon.ico") ||
            !strncmp(requestInfo->uri, "/js/", 4) ||
            !strncmp(requestInfo->uri, "/wav/", 5) ||
            !strncmp(requestInfo->uri, "/img/", 5) ||
            !strncmp(requestInfo->uri, "/css/", 5) ||
            !strncmp(requestInfo->uri, "/html/", 6)) {
            if(handleResource(conn, requestInfo))
                return handled;
            return notHandled; // serve resources using mongoose's built-in mechanism
        }

        handleForbidden(conn);
        Log::warn(TRACE_MONGOOSE, "Unknown path %s", requestInfo->uri);
        return handled;

    default:
        break;
    }
    return notHandled;
}

bool NrdApplication::handleEventSource(struct mg_connection *conn, const struct mg_request_info *requestInfo)
{
    if (!isSecureIP(requestInfo->remote_ip)) {
        Log::error(TRACE_MONGOOSE, "Disallowing event source connection from insecure ip");
        return false;
    }

    EventSourceEventConnection *ev;
    shared_ptr<EventConnection> connection(ev = new EventSourceEventConnection(conn, requestInfo));
    NTRACE(TRACE_EVENTCONNECTION, "Opening connection for EventConnection %p [%d]", connection.get(), countEventConnections());
    if (ev->writeHeader()) {
        addEventConnection(connection);
        ev->wait(); // blocks until the connection is closed
        removeEventConnection(connection);
    }
    NTRACE(TRACE_EVENTCONNECTION, "Closing connection for EventConnection %p active connections: %d",
           connection.get(), countEventConnections());
    return true;
}

void NrdApplication::handleForbidden(struct mg_connection *conn, int statusCode)
{
    NrdApplication::writeDataF(conn, "%s", forbiddenMessage(statusCode).c_str());
}

std::string NrdApplication::forbiddenMessage(int statusCode)
{
    std::string statusMessage = "Error";
    if(statusCode == 404)
        statusMessage = "Not Found";
    else if(statusCode == 504)
        statusMessage = "Proxy Timeout";
    else if(statusCode == 502)
        statusMessage = "Proxy Error";
    else if(statusCode == 403)
        statusMessage = "Forbidden";

    std::ostringstream os;
    os << "HTTP/1.1 " << statusCode << " " << statusMessage << "\r\n"
       << "Content-Type: text/html\r\n"
       << "Cache: no-cache\r\n"
       << "Cache-Control: private\r\n"
       << "Connection: keep-alive\r\n"
       << "Content-Length: 0\r\n"
       << "Date: " << Time::serverTime().calendarTimeText() << "\r\n"
       << "\r\n"
       << "<html><head><title>" << statusMessage << "</title></head>\r\n"
       << "<body>\r\n"
       << "<h1>Not Found</h1>\r\n"
       << "<p>The page you are requesting cannot be loaded! Try again.\r\n"
       << "</body>\r\n"
       << "</html>\r\n"
       << "\r\n";
    return os.str();
}

void NrdApplication::handleIpc(struct mg_connection *conn, const struct mg_request_info *requestInfo)
{
    if (!isSecureIP(requestInfo->remote_ip)) {
        Log::error(TRACE_MONGOOSE, "Disallowing ipc from insecure ip %s", readPostData(conn).c_str());
        return;
    }

    std::string postData;
    Variant htmlQuery = Variant::fromQueryString(requestInfo->query_string), ipcData;
    if (!strcmp(requestInfo->request_method, "POST")) {
        const char* contentLengthString = mg_get_header(conn, "Content-Length");
        const int contentLength = (contentLengthString ? strtoull(contentLengthString, 0, 10) : 0);
        NTRACE(TRACE_MONGOOSE_REQUEST, "ContentLength %d [%s]", contentLength, contentLengthString);
        if (contentLength) {
            postData = readPostData(conn);
            NTRACE(TRACE_MONGOOSE_REQUEST, "Body: [%s]", postData.c_str());
        }
    }
    if (postData.empty()) {
        postData = htmlQuery.mapValue<std::string>("data");
        ipcData = Variant::fromJSON(postData);
    } else {
        const char *contentTypeString = mg_get_header(conn, "Content-type");
        if (contentTypeString && !strcmp(contentTypeString, "text/plain"))
            ipcData = Variant::fromJSON(postData);
        else
            ipcData = Variant::fromQueryString(postData);
    }
    NTRACE(TRACE_MONGOOSE_REQUEST, "data: %s", ipcData.toJSON().c_str()); //SDM

    uint32_t flags = 0;
#ifndef BUILD_DEBUG
    if (isSecureIP(requestInfo->remote_ip))
#endif
        flags |= Request::FromTrustedSource;

    if (!ipcData.isStringMap())
        ipcData = std::map<std::string, Variant>();

    const Request request(ipcData, flags);
    post(request);
    NrdApplication::writeDataF(conn,
                               "HTTP/1.1 200 OK\r\n"
                               "Access-Control-Allow-Origin: *\r\n"
                               "Cache: no-cache\r\n"
                               "Connection: keep-alive\r\n"
                               "Content-Length: 0\r\n"
                               "Date: %s\r\n"
                               "\r\n",
                               Time::serverTime().calendarTimeText().c_str());
}

bool NrdApplication::writeDataF(struct mg_connection *conn, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    const std::string str = StringFormatter::vsformat(format, args);
    va_end(args);
    return writeData(conn, str.c_str(), str.size());
}

bool NrdApplication::writeData(struct mg_connection *conn, const char *data, int length)
{
    assert(conn);
    assert(data);
    assert(length > 0);
    int written = 0;
    while (written < length) {
        const int l = mg_write(conn, data + written, length - written);
        if (l <= 0) {
#ifdef BUILD_DEBUG
            const int e = errno;
            Log::warn(TRACE_MONGOOSE, "Failed to write something here: %s (%d) [%d]", data + written, length - written, e);
#endif
            // if (e != EPIPE)
            //     perror("WriteData problem");

            return false;
        }
        written += l;
    }
    return true;
}

// Determines whether or not to allow requests to NrdApplication
// to come in on all interfaces, or just localhost.
bool NrdApplication::allowAllInterfaces()
{
    if (NrdConfiguration::allowAllInterfaces())
        return true;

    NrdIpAddr ip(secureIP());

    return (ip.valid() && (ip != NrdIpAddr("127.0.0.1")));
}

bool NrdApplication::isSecureIP(uint32_t ip)
{
    if (NrdConfiguration::allowAllInterfaces())
        return true;

    NrdIpAddr localhost("127.0.0.1");
    NrdIpAddr secureIP_a(mSecureIP);

    in_addr inAddr;

    inAddr.s_addr = ip;

    NrdIpAddr ip_a(inAddr);

    if(!secureIP_a.valid()) {
#ifdef BUILD_PRODUCTION
        return false;
#else  // Allow everyone by default in debug builds
        return true;
#endif
    }

    if (!ip_a.valid())
    {
        return false;
    }

    if (ip_a == localhost) // If test driver is set, always allow requests from localhost
    {
        return true;
    }

    return (secureIP_a == ip_a);
}

#endif

Variant NrdApplication::stats(uint8_t flags) const
{
    Variant result;
    result["name"] = "NRDP";
    if(EventLoop::SharedPtr e = eventLoop())
        result["events"] = e->eventDelay().ms();
    result["databuffers"] = DataBuffer::currentTotalCapacity();
    if(flags & Stat_Objects) {
        Variant objects;
        ObjectCount::dump(ObjectCount::None, std::string(), &objects);
        result["objects"] = objects;
    }
    if(flags & Stat_Threads) {
        Variant &threads = result["threads"];
        const std::vector<ThreadConfig*> configs = ThreadConfig::getAllConfigs();
        for(std::vector<ThreadConfig*>::const_iterator config_it = configs.begin(); config_it != configs.end(); ++config_it) {
            const std::set<Thread*> allThreads = (*config_it)->getThreads();
            for(std::set<Thread*>::const_iterator thread_it = allThreads.begin(); thread_it != allThreads.end(); ++thread_it) {
                char buffer[20];
                snprintf(buffer, sizeof(buffer), "%d", (*thread_it)->ThreadId());
                threads[buffer] = (*thread_it)->GetCPUStats();
            }
        }
    }
    if(flags & Stat_Memory)
        result["memory"]["smaps"] = netflix::util::MemoryStats::collectRaw(getpid());
    return result;
}

DataBuffer NrdApplication::dataURI(const std::string &path) const
{
    if(startsWith(path, "/stats/")|| path == "/smaps/") {
        uint8_t flags = Stat_None;
        if(path == "/smaps/")
            flags |= Stat_Memory;
        else if(path == "/stats/")
            flags |= Stat_All;
        else if(path == "/stats/threads/")
            flags |= Stat_Threads;
        else if(path == "/stats/memory/all")
            flags |= (Stat_Memory|Stat_JS);
        else if(path == "/stats/memory/")
            flags |= Stat_Memory;
        else if(path == "/stats/js/")
            flags |= Stat_JS;
        return stats(flags).toJSON();
    } else if(path == "netflix.log") {
        const std::string logFile = Configuration::logFile();
        if(!logFile.empty())
            return DataBuffer::fromFile(logFile.c_str());
    }
    return DataBuffer();
}

shared_ptr<NrdpBridge> NrdApplication::nrdpBridge() const
{
    ScopedMutex lock(NfObject::mutex());
    return mNrdpBridge;
}

void NrdApplication::addObject(const std::string &parentPath, const shared_ptr<NfObject> &object)
{
    assert(object);
    shared_ptr<NfObject> parent = findObject(parentPath);
    assert(parent);
    if (parent)
        parent->addChild(object);
}

std::vector<shared_ptr<LogSink> > NrdApplication::logSinks() const
{
    std::vector<shared_ptr<LogSink> > ret = Application::logSinks();
    ret.push_back(mNrdpBridge->logBridge);
    return ret;
}
