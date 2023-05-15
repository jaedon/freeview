/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef __NRDAPPLICATION_H__
#define __NRDAPPLICATION_H__

#include <map>
#include <nrd/ApplicationEvent.h>
#include <nrd/ConfigurationKeys.h>
#include <nrd/IDeviceLib.h>
#include <nrd/ISystem.h>
#include <nrd/config.h>
#include <nrd/config.h>
#include <nrdbase/Application.h>
#include <nrdbase/ConditionVariable.h>
#include <nrdbase/ConsoleSink.h>
#include <nrdbase/DataBuffer.h>
#include <nrdbase/Grep.h>
#include <nrdbase/Log.h>
#include <nrdbase/Mutex.h>
#include <nrdbase/Noncopyable.h>
#include <nrdbase/ThreadPool.h>
#include <nrdbase/Variant.h>
#include <nrdbase/WeakPtrSet.h>
#include <nrdnet/AseUrl.h>
#include <nrdnet/TelnetServer.h>
#include <nrd/NrdConfiguration.h>
#include <set>
#include <stdarg.h>
#include <string>
#include <vector>

#ifdef NRDP_WEBSERVER_ENABLED
extern "C" {
#include <nrd/mongoose.h>
}
#endif

class NetworkListener;
namespace netflix {

class FPSTimer;
class NetworkBridge;
class ResourceManager;
class SecureStore;
class DiskStore;

namespace mediacontrol {
class IMediaSourcePlayer;
class IAdaptiveStreamingPlayer;
}

namespace device {
class ISystemEventForwarder;
}

namespace util {
class NrdIpConfigurationListener;
}

class NfObject;
class Request;
class Response;
class EventConnection;
class NrdpBridge;

class SystemData;
class SystemDataListener;

class NrdApplication : public Application
{
public:
    NrdApplication();
    virtual ~NrdApplication();

    static NrdApplication *instance() { return static_cast<NrdApplication*>(Application::instance()); }

    std::string uiLanguages() const;
    std::map<std::string, std::string> serviceParameters(const std::string &file) const;

    std::string readSystemConfiguration(const std::string &key);
    std::string readSystemConfiguration(SystemKeys::Key key);

    enum WriteMode {
        WriteMode_None,
        WriteMode_Flush
    };

    void writeSystemConfiguration(const std::string &key, const std::string &value, WriteMode mode = WriteMode_None);
    void writeSystemConfiguration(SystemKeys::Key key, const std::string &value, WriteMode mode = WriteMode_None);

    shared_ptr<netflix::device::IDeviceLib> getDeviceLib() const { return mDeviceLib; }
    shared_ptr<netflix::device::ISystem> getSystem() const { assert(mDeviceLib); return mDeviceLib->getSystem(); }
    shared_ptr<netflix::device::IDrmSystem> getDrmSystem() const { return mDeviceLib->getDrmSystem(); }
    shared_ptr<SystemData> systemData() const { return mSystemData; }

    std::string esnPrefix() const;

    void setAppTrustStore(const std::string &trustStore);
    std::string appTrustStore() const;

    void setServerTime(const Time &time);

    void initDiskCache(const std::string &spec);

    enum Status {
        Status_NotStarted,
        Status_Starting,
        Status_Running,
        Status_Suspending,
        Status_Suspended,
        Status_Stopping,
        Status_Stopped
    };
    Status status() const;
    void setStatus(Status status);
    bool waitForStatus(Status status, Time *time=0) const;
    inline bool waitForStatus(Status status, Time &time) const { return waitForStatus(status, &time); }

    bool hasQuit() const;
    inline int quitCode() const { return mQuitCode; }
    void quit(int code=0); /* Call this function to shut down nrd */
    virtual bool start();
    void scheduleQuit(int code=0);

    void suspendComplete();
    bool isSuspended() const;
    void requestSuspend(const std::string &reason=std::string());
    void setSuspended(bool suspend, const Variant &data); /* Call this function to suspend/resume nrd */
    void setHasSuspendHandler(bool s);
    bool hasSuspendHandler() const;
    inline bool waitForSuspend(Time &time) const { return waitForStatus(Status_Suspended, time); }

    Time monoApplicationStarted() const;
    static Time timeSinceApplicationStarted();

    Time monoFirstLocationSet() const;
    static Time timeSinceFirstLocationSet();

    Time monoLocationChanged() const;
    static Time timeSinceLocationChanged();

    void setResourceManager(const shared_ptr<ResourceManager> &manager) { mResourceManager = manager; }
    shared_ptr<ResourceManager> resourceManager() const { return mResourceManager; } // ### lock?

    shared_ptr<SecureStore> secureStore() const { return mSecureStore; }
    shared_ptr<DiskStore> diskStore() const { return mDiskStore; }

    // NBP

    virtual Variant send(const Request &request);
    virtual bool debuggerAttached() const { return false; }

    Variant post(const Request &request);
    int sendResponse(const Response &response);

    shared_ptr<ThreadPool> threadPool() { return mThreadPool; }

    shared_ptr<NfObject> findObject(const std::string &path);
    void startQuitTimer(int ms);

    virtual void setTestDriverIpAddress(const std::string &);

    inline int countEventConnections() const {
        ScopedMutex lock(mEventConnectionsMutex);
        return mEventConnections.size();
    }
    void addEventConnection(const shared_ptr<EventConnection> &eventConnection);
    void removeEventConnection(const shared_ptr<EventConnection> &eventConnection);
    void clearEventConnections();

    Variant versionInfo() const;
    void addLibraryInfo(const Variant &data);

    void addEventListener(const shared_ptr<ApplicationEventListener> &listener);
    void removeEventListener(const shared_ptr<ApplicationEventListener> &listener);

    using Application::sendEvent;
    void sendEvent(ApplicationEvent::Type event, const Variant &data = Variant());

    bool isPlayerOpen() const;
    void setPlayer(const std::string &name, const shared_ptr<netflix::mediacontrol::IMediaSourcePlayer> &player);
    shared_ptr<netflix::mediacontrol::IMediaSourcePlayer> player(const std::string &name) const;
    void setPlayer(const shared_ptr<netflix::mediacontrol::IAdaptiveStreamingPlayer> &player);
    shared_ptr<netflix::mediacontrol::IAdaptiveStreamingPlayer> player() const;
    void setPlayerType(enum PlayerType);
    enum PlayerType playerType();

    shared_ptr<NrdpBridge> nrdpBridge() const;
    void addObject(const std::string &parentPath, const shared_ptr<NfObject> &object);
    void addSystemListener(const shared_ptr<netflix::device::ISystem::EventDispatcher> &listener);
    void removeSystemListener(const shared_ptr<netflix::device::ISystem::EventDispatcher> &listener);
    void sendSystemCommand(const std::string &command);

    void configureDns();
    void useFalbackDnsConfig(bool fallback);
    void updateDeviceBridgeNetworkProperty();

    DataBuffer resourceContent(const std::string &path,
                               const Variant &params = Variant(),
                               Time *modificationTime = 0) const; // reimplement for Wii?

    struct DataListEntry {
        AseUrl name, href;
        int size;
    };

    DataBuffer dataList(const std::vector<DataListEntry> &entries) const;

    enum {
        Stat_None    = 0x00,
        Stat_Threads = 0x01,
        Stat_Memory  = 0x02,
        Stat_JS      = 0x04,
        Stat_Objects = 0x08,
        Stat_All     = 0xFF
    };
    virtual Variant stats(uint8_t flags) const;
    virtual bool initConfiguration(int argc, char **argv) { return NrdConfiguration::init(argc, argv); }
    virtual ConfigurationOptions defaultConfigurationOptions() const { return NrdConfiguration::defaultOptions(); }

#ifdef NRDP_WEBSERVER_ENABLED
    std::string secureIP() const { return mSecureIP; }
    bool isSecureIP(uint32_t ip);

    static bool writeData(struct mg_connection *conn, const char *data, int length);
    static bool writeDataF(struct mg_connection *conn, const char *format, ...);
#endif

protected:
    virtual void event(ApplicationEvent::Type type, const Variant &data);
    virtual shared_ptr<ResourceManager> createResourceManager();
    virtual DataBuffer dataURI(const std::string &path) const;
    virtual std::vector<shared_ptr<LogSink> > logSinks() const;

    void checkLibraryVersions();
    bool startMongoose();
private:
    void quitImpl();
    void initVersions();

    int mQuitCode;
    Status mStatus;
    bool mHasSuspendHandlers;
    mutable Mutex mMutex;
    mutable ConditionVariable mCondition;

    shared_ptr<netflix::device::IDeviceLib> mDeviceLib;
    Time mFirstLocationSet;
    Time mLocationChanged;

    shared_ptr<ResourceManager> mResourceManager;

    shared_ptr<SystemData> mSystemData;
    shared_ptr<DiskStore> mDiskStore;
    shared_ptr<SecureStore> mSecureStore;

    // nbp

    shared_ptr<NrdpBridge> mNrdpBridge;

    mutable Mutex mEventConnectionsMutex;
    std::set<shared_ptr<EventConnection> > mEventConnections;
    bool mShutdownEventConnections;

    struct {
        std::map<std::string, Variant> libraries;
        std::map<std::string, Variant> components;
    } mVersion;

    shared_ptr<TelnetServer> mTelnetServer;
    shared_ptr<ThreadPool> mThreadPool;
    shared_ptr<SystemDataListener> mSystemDataListener;
    WeakPtrSet<ApplicationEventListener> mEventListeners;

    mutable Mutex mPlayerMutex;
    std::map<std::string, shared_ptr<netflix::mediacontrol::IMediaSourcePlayer> > mPlayers;
    shared_ptr<netflix::mediacontrol::IAdaptiveStreamingPlayer> mPlayer;
    enum PlayerType mPlayerType;
    shared_ptr<NetworkListener> mNetworkListener;

    shared_ptr<netflix::device::ISystemEventForwarder> mSystemDispatcher;
    shared_ptr<netflix::util::NrdIpConfigurationListener> mNrdIpConfigurationListener;
#ifdef NRDP_WEBSERVER_ENABLED
    static void* s_mongooseCallback(enum mg_event event, struct mg_connection* conn, const struct mg_request_info* requestInfo);
    bool allowAllInterfaces();
    void *mongooseCallback(enum mg_event event, struct mg_connection *conn, const struct mg_request_info *requestInfo);
    std::string forbiddenMessage(int statusCode);

    void handlePing(struct mg_connection *conn);
    void handleForbidden(struct mg_connection *conn, int statusCode=404);
    bool handleEventSource(struct mg_connection *conn, const struct mg_request_info *requestInfo);
    void handleUiBoot(struct mg_connection *conn, const struct mg_request_info *requestInfo);
    void handleIpc(struct mg_connection *conn, const struct mg_request_info *requestInfo);
    bool handleResource(struct mg_connection *conn, const mg_request_info *requestInfo, bool raw=false);

    std::string portsString() const;

    mg_context *mMongoose;
    bool mAllInterfacesEnabled;
    std::string mSecureIP;
    Mutex mHttpMutex;
#endif
};

static inline NrdApplication *nrdApp() { return NrdApplication::instance(); }
}

#endif /* __NRDAPPLICATION_H__ */
