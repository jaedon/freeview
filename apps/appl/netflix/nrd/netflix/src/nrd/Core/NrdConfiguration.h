/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef NRDCONFIGURATION_H
#define NRDCONFIGURATION_H

#include <nrdbase/Configuration.h>
#include <nrdase/BitrateRange.h>
#include <nrd/config.h>

#include <string>
#include <vector>
#include <nrdbase/StdInt.h>

namespace netflix
{

class NrdConfiguration
{
public:
    static bool isValid();

    static ConfigurationOptions defaultOptions();
    static std::vector<std::string> arguments() { return Configuration::arguments(defaultOptions()); }
    static bool init(int argc, char **argv, const ConfigurationOptions &options);
    static bool init(int argc, char **argv) { return init(argc, argv, defaultOptions()); }

    static Variant bootRequest() { return sBootRequest; }
    static std::string appBootUrl() { return sAppbootUrl; }

    static void setUIRequest(const Variant &request) { sUIRequest = request; }
    static Variant uiRequest() { return sUIRequest; }

    static const std::string &queryString() { return sQueryString; }
    static void setQueryString(std::string const& str) { sQueryString = str; }

    static bool allowInsecureNetworkRequests() { return sAllowInsecureNetworkRequests; }
    static bool mdxEnabled() { return sMDXEnabled; }

    static const std::string &smapDirectory() { return sSMAPDirectory; }

#ifdef NRDP_WEBSERVER_ENABLED
    static bool allowAllInterfaces() { return sAllowAllInterfaces; }
    inline static bool nbpdEnabled() { return sNBPDEnabled; }
    static llong nbpdPort() { return sNBPDPort; }
#endif

    // MEDIA
    inline static llong minAudioBitrate() { return sMinAudRate; }
    inline static llong maxAudioBitrate() { return sMaxAudRate; }
    static std::vector<shared_ptr<netflix::ase::BitrateRange> > videoBitrateRanges();

    // NRD
    inline static llong nbpThreadPoolMaxThreadCount() { return sNBPThreadPoolMaxThreadCount; }
    inline static llong nbpThreadPoolMinThreadCount() { return sNBPThreadPoolMinThreadCount; }
    inline static llong nbpThreadPoolIdleTime() { return sNBPThreadPoolIdleTime; }

    inline static llong quitTimerInterval() { return sQuitTimerInterval; }

    inline static bool sendStreamInfoEvents() { return sSendStreamInfoEvents; }
    inline static void setSendStreamInfoEvents(bool enable) { sSendStreamInfoEvents = enable; }

    inline static llong mediaCacheCapacity() { return sMediaCacheCapacity; }
private:
    NrdConfiguration();

    static bool parseJSOpts(ConfigurationOption::FunctionMode mode, Variant &variant);
    static bool parseBootRequest(ConfigurationOption::FunctionMode mode, Variant &variant);
    static bool parseUIRequest(ConfigurationOption::FunctionMode mode, Variant &variant);

    static std::string appendURLParams(const std::string &base, const std::string &params);

    static bool sAllowInsecureNetworkRequests;
    static std::string sQueryString;
    static Variant sBootRequest;
    static Variant sUIRequest;
    static std::string sAppbootUrl;
    static std::string sAppbootParams;
    static std::string sJsOpts;

    static bool sMDXEnabled;
    static std::string sSMAPDirectory;
#ifdef NRDP_WEBSERVER_ENABLED
    static bool sAllowAllInterfaces;
    static bool sNBPDEnabled;
    static llong sNBPDPort;
    static std::string sResourcesDirectory; // Location of the resources directory
#endif

    // NBP
    static llong sNBPThreadPoolMaxThreadCount;
    static llong sNBPThreadPoolMinThreadCount;
    static llong sNBPThreadPoolIdleTime;
    static bool sJsAsserts;
    static llong sJsDelay;

    // MEDIA
    static std::string sVidRanges;
    static llong sMinAudRate;
    static llong sMaxAudRate;
    static bool sSendStreamInfoEvents;

    static llong sQuitTimerInterval;
    static llong sMediaCacheCapacity;
};

}

#endif // NRDCONFIGURATION_H
