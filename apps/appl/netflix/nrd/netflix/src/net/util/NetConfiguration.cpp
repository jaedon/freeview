/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include <nrdnet/NetConfiguration.h>

using namespace netflix;

bool NetConfiguration::sSSLPeerVerification = true;
llong NetConfiguration::sTrafficShaperSpeed = 0;
llong NetConfiguration::sTrafficShaperMinSleepInterval = 10;
llong NetConfiguration::sTrafficShaperMaxSleepInterval = 100;
llong NetConfiguration::sTrafficShaperSoftCapacity = 0;
llong NetConfiguration::sTrafficShaperHardCapacity = 5*1024*1024;

bool NetConfiguration::init(int argc, char **argv, const ConfigurationOptions &options)
{
    if(!Configuration::init(argc, argv, options))
        return false;
    return true;
}

ConfigurationOptions NetConfiguration::defaultOptions()
{
    ConfigurationOptions options = Configuration::defaultOptions();
    options.push_back("Networking Options");

    options.push_back(ConfigurationOption('P', "ssl-peer-verification", &sSSLPeerVerification, "SSL peer verification."));
    options.push_back(ConfigurationOption(0, "traffic-shaper-speed", ConfigurationOption::SIZE_ARGUMENT,
                                          &sTrafficShaperSpeed, "Network traffic shaper speed in bytes per second"));
    options.push_back(ConfigurationOption(0, "traffic-shaper-min-sleep-interval", ConfigurationOption::TIME_ARGUMENT,
                                          &sTrafficShaperMinSleepInterval, "Network traffic shaper minimum sleep interval"));
    options.push_back(ConfigurationOption(0, "traffic-shaper-max-sleep-interval", ConfigurationOption::TIME_ARGUMENT,
                                          &sTrafficShaperMaxSleepInterval, "Network traffic shaper maximum sleep interval"));
    options.push_back(ConfigurationOption(0, "traffic-shaper-soft-capacity", ConfigurationOption::SIZE_ARGUMENT,
                                          &sTrafficShaperSoftCapacity, "Network traffic shaper soft capacity"));
    options.push_back(ConfigurationOption(0, "traffic-shaper-hard-capacity", ConfigurationOption::SIZE_ARGUMENT,
                                          &sTrafficShaperHardCapacity, "Network traffic shaper hard capacity"));
    return options;
}
