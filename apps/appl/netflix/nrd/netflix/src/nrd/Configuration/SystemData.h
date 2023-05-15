/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#ifndef SYSTEMDATA_H_
#define SYSTEMDATA_H_

#include <iterator>
#include <map>
#include <string>
#include <vector>

#include <nrdbase/tr1.h>
#include <nrdbase/Mutex.h>
#include <nrdbase/ScopedMutex.h>
#include <nrdbase/WeakPtrSet.h>
#include <nrdbase/NetworkInterfaceInfo.h>

#include "SystemDataKey.h"
#include "SystemDataListener.h"

namespace netflix {

class SecureStore;
class DiskStore;

/**
 * @class SystemData SystemData.h
 * @brief Base class for all system data.
 */
class SystemData
{
public:
    /** Constructor. */
    SystemData();

    /**
     * Registers a listener with this SystemData.
     *
     * @param listener the listener.
     */
    void addListener(const shared_ptr<SystemDataListener> &listener);

    /**
     * Unregisters a listener with this SystemData.
     *
     * @param listener the listener.
     */
    void removeListener(const shared_ptr<SystemDataListener> &listener);

    // System ID's (application, session, transaction)
    // Could just use setAppId, but that function is moved around in 14.2
    void resetAppID(ullong appID = 0);
    ullong resetSessionID();

    // request parameters: sent to server in Request
    std::vector<std::string> languagePreferences() const;
    void setLanguagePreferences(const std::vector<std::string> &languages);
    void setInstrumentationEnabled(bool enabled);
    bool instrumentationEnabled() const;
    void setInstrumentationEvents(const std::string & events);
    std::string instrumentationEvents() const;

    // parameters received from config service
    void addServiceFile(const std::string& file);
    std::vector<std::string> serviceFiles() const;
    void setServiceParameters(const std::string& file, const std::map<std::string, std::string>& params);
    inline void updateServiceParameters() { notifyListeners(SystemDataKey::SYSTEM_FILE); }
    std::map<std::string, std::string> serviceParameters(const std::string& file);

    // AppTrustStore
    std::string appTrustStore() const;
    void setAppTrustStore(const std::string &trustStore);

    NetworkInterfaceInfo getNetworkInterfaceInfo();
    void updateNetworkInterfaceInfo();
    uint32_t appTrustStoreBudget() const { return AppTrustStoreBudget; }
private:
    enum { AppTrustStoreBudget = 32 * 1024 };
    /**
     * Informs all listeners of a datum value change.
     *
     * @param key the datum key.
     */
    void notifyListeners(SystemDataKey::Key key)
    {
        std::vector<shared_ptr<SystemDataListener> > listeners;
        {
            ScopedMutex lock(mx_);
            listeners_.lock(listeners);
        }
        for(std::vector<shared_ptr<SystemDataListener> >::const_iterator it = listeners.begin(); it != listeners.end(); ++it)
            (*it)->datumChanged(key);
    }

    void updateIpVersionsPreferenceAndPolicy();
    void setAppID(ullong appID);

    Mutex mx_; //!< Listeners mutex.
    WeakPtrSet<SystemDataListener> listeners_; //!< Listeners.

    // protect the authentication parameters by a different mutex than the
    // server parameters. This is to prevent possible deadlocks with threads
    // locking separate parts of the data and grabbing in them different orders
    // (i.e. locking transaction authentication and then logging data
    //  vs. logging data and server info)
    mutable Mutex paramMutex_;

    bool instrumentationEnabled_;
    std::string instrumentationEvents_;


    // parameters retrieved from config service
    std::vector<std::string> serviceParamsList_;
    std::map<std::string, std::map<std::string, std::string> > serviceParams_;


    std::string appTrustStore_;

    // Network Interface Info
    NetworkInterfaceInfo networkInterfaceInfo_;
    bool                 networkInterfaceInfoIsValid_;
};

} // namespace netflix::config

#endif /*SYSTEMDATA_H_*/
