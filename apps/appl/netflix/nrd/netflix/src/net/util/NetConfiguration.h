/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef NETCONFIGURATION_H
#define NETCONFIGURATION_H

#include <nrdbase/config.h>
#include <nrdbase/Configuration.h>

#include <string>
#include <vector>

namespace netflix
{

class NetConfiguration
{
public:
    static ConfigurationOptions defaultOptions();
    static std::vector<std::string> arguments() { return Configuration::arguments(defaultOptions()); }
    static bool init(int argc, char **argv, const ConfigurationOptions &options);
    static bool init(int argc, char **argv) { return init(argc, argv, defaultOptions()); }

    // GENERAL
    inline static bool getSSLPeerVerification() { return sSSLPeerVerification; }

    // TRAFFIC SHAPING
    inline static llong getTrafficShaperSpeed() { return sTrafficShaperSpeed; }
    inline static void setTrafficShaperSpeed(llong v) { sTrafficShaperSpeed = v; }
    inline static llong getTrafficShaperMinSleepInterval() { return sTrafficShaperMinSleepInterval; }
    inline static void setTrafficShaperMinSleepInterval(llong v) { sTrafficShaperMinSleepInterval = v; }
    inline static llong getTrafficShaperMaxSleepInterval() { return sTrafficShaperMaxSleepInterval; }
    inline static void setTrafficShaperMaxSleepInterval(llong v) { sTrafficShaperMaxSleepInterval = v; }
    inline static llong getTrafficShaperSoftCapacity() { return sTrafficShaperSoftCapacity; }
    inline static void setTrafficShaperSoftCapacity(llong v) { sTrafficShaperSoftCapacity = v; }
    inline static llong getTrafficShaperHardCapacity() { return sTrafficShaperHardCapacity; }
    inline static void setTrafficShaperHardCapacity(llong v) { sTrafficShaperHardCapacity = v; }

private:
    NetConfiguration();

    static bool sSSLPeerVerification;

    //TRAFFIC SHAPING
    static llong sTrafficShaperSpeed;
    static llong sTrafficShaperMinSleepInterval;
    static llong sTrafficShaperMaxSleepInterval;
    static llong sTrafficShaperSoftCapacity;
    static llong sTrafficShaperHardCapacity;
};

}

#endif // NETCONFIGURATION_H
