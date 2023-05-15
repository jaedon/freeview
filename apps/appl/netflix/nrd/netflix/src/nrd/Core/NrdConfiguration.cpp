/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is stictly
 * prohibited.
 */

#include <nrd/config.h>
#include <nrd/NrdConfiguration.h>
#include "InstrumentationThread.h"

#include <nrdnet/NetConfiguration.h>
#include <nrdnet/AseUrl.h>

#include <nrdbase/LexicalCast.h>
#include <nrdbase/Log.h>
#include <nrdbase/ReadDir.h>
#include <nrdbase/StringTokenizer.h>
#include <nrdbase/StringUtils.h>

#include <sys/stat.h>

using namespace netflix;
using namespace netflix::ase;

#ifdef BUILD_PRODUCTION
bool NrdConfiguration::sAllowInsecureNetworkRequests = false;
#else
bool NrdConfiguration::sAllowInsecureNetworkRequests = true;
#endif
std::string NrdConfiguration::sQueryString;
Variant NrdConfiguration::sBootRequest = Variant::StringMap("url", "http://localcontrol.netflix.com/js/boot.js")("no-inject", true);
Variant NrdConfiguration::sUIRequest;
std::string NrdConfiguration::sAppbootUrl;
std::string NrdConfiguration::sAppbootParams;
std::string NrdConfiguration::sJsOpts;
std::string NrdConfiguration::sSMAPDirectory;

#ifdef NRDP_WEBSERVER_ENABLED
#if defined(BUILD_PRODUCTION)
bool NrdConfiguration::sAllowAllInterfaces = false;
#else
bool NrdConfiguration::sAllowAllInterfaces = true;
#endif
#if defined(BUILD_DEBUG)
bool NrdConfiguration::sNBPDEnabled = true;
#else
bool NrdConfiguration::sNBPDEnabled = false;
#endif
llong NrdConfiguration::sNBPDPort = 6359;
#endif

#ifdef NRDP_MDX_ENABLED
# include <nrd/MdxVersion.h>
# include <nrd/MdxConfiguration.h>
#include <nrdbase/ReadDir.h>
bool NrdConfiguration::sMDXEnabled = true;
#endif

#ifdef NRDP_DPI_CONFIGURATION_OPTIONS
ConfigurationOptions getDeviceLibOptions();
#endif

llong NrdConfiguration::sNBPThreadPoolMaxThreadCount = 5;
llong NrdConfiguration::sNBPThreadPoolMinThreadCount = 2;
llong NrdConfiguration::sNBPThreadPoolIdleTime = 30 * 1000;
std::string NrdConfiguration::sVidRanges;
llong NrdConfiguration::sMinAudRate = 0;
llong NrdConfiguration::sMaxAudRate = 0xffffffff;
bool NrdConfiguration::sSendStreamInfoEvents = false;
bool NrdConfiguration::sJsAsserts = false;
llong NrdConfiguration::sJsDelay = 100;
llong NrdConfiguration::sMediaCacheCapacity = 0;
llong NrdConfiguration::sQuitTimerInterval = 0;

bool NrdConfiguration::isValid()
{
    return true;
}

bool NrdConfiguration::init(int argc, char **argv, const ConfigurationOptions &options)
{
    if(!NetConfiguration::init(argc, argv, options))
        return false;
    if (!NrdConfiguration::isValid()) {
        Log::error(TRACE_STARTUP, "Invalid NRD Configuration");
        return false;
    }
    videoBitrateRanges();
    return true;
}

static shared_ptr<BitrateRange> createBitrateRange(ContentProfile profile, llong min, llong max)
{
    shared_ptr<BitrateRange> range(new BitrateRange);
    range->mContentProfile = profile;
    range->mMinVideoBitrate = min;
    range->mMaxVideoBitrate = max;
    return range;
}

std::vector<shared_ptr<BitrateRange> > NrdConfiguration::videoBitrateRanges()
{
    std::vector<shared_ptr<BitrateRange> > ranges;
    if (sVidRanges.empty())
        return ranges;

    /* format: [profile: ]range[, profile: range, ...]
     * range is one of val, min-, -max, min-max
     * examples:
     *
     * all profiles, only 1000-2000:
     *   1000-2000
     * all profiles, 4000 or greater:
     *   4000-
     * all profiles, less than 600:
     *   -600
     * all profiles, only 235:
     *   235
     *
     * baseline 500-1000, main 750-1500:
     *   playready-h264bpl30-dash: 500-1000, playready-h264mpl30-dash: 750-1500, playready-h264mpl31-dash: 750-1500, playready-h264mpl40-dash: 750-1500
     */
    std::vector<std::string> config;
    StringTokenizer::tokenize(sVidRanges, config, ",");
    for (size_t i = 0; i < config.size(); i++) {
        std::string profileString;
        std::string rangeString;
        {
            std::vector<std::string> parts;
            StringTokenizer::tokenize(config[i], parts, ":");
            if (parts.size() == 1) {
                trim(parts[0]);
                rangeString = parts[0];
            } else if (parts.size() == 2) {
                // first part is the profile, second part is the range
                trim(parts[0]);
                trim(parts[1]);
                profileString = parts[0];
                rangeString = parts[1];
            }
        }

        // we can deal with an empty profile, not with an empty range
        if (rangeString.empty()) {
            continue;
        }

        llong minBitrate = 0;
        llong maxBitrate = 0xffffff;

        {
            if (rangeString.find('-') == std::string::npos) {
                // val
                minBitrate = maxBitrate = lexical_cast<llong>(rangeString);
            } else if (rangeString[0] == '-') {
                // -max
                minBitrate = 0;
                maxBitrate = lexical_cast<llong>(rangeString.substr(1));
            } else if (rangeString[rangeString.length() - 1] == '-') {
                // min-
                minBitrate = lexical_cast<llong>(rangeString.substr(0, rangeString.length() - 1));
                maxBitrate = 0xffffffff;
            } else {
                // range is separated by hyphen
                std::vector<std::string> values;
                StringTokenizer::tokenize(rangeString, values, "-");
                if (values.size() != 2) {
                    continue;
                }

                // min-max
                minBitrate = lexical_cast<llong>(trimmed(values[0]));
                maxBitrate = lexical_cast<llong>(trimmed(values[1]));
            }
        }

        if (!profileString.empty()) {
            ContentProfile profile = stringToProfile(profileString);
            if (profile == UNKNOWN_PROFILE) {
                continue;
            }
            ranges.push_back(createBitrateRange(profile, minBitrate, maxBitrate));
        } else {
            // do it for all profiles
            for (int profile = VIDEO_PROFILE_START + 1; profile < LAST_VIDEO_PROFILE; ++profile) {
                ranges.push_back(createBitrateRange(static_cast<ContentProfile>(profile), minBitrate, maxBitrate));
            }
        }
    }

    return ranges;
}

ConfigurationOptions NrdConfiguration::defaultOptions()
{
    ConfigurationOptions options = NetConfiguration::defaultOptions();


    options.push_back("Application Options");
    options.push_back(ConfigurationOption('U', "ui-url", NrdConfiguration::parseUIRequest,
                                          "Override default UI URL."));
    options.push_back(ConfigurationOption('0', "boot-url", &NrdConfiguration::parseBootRequest,
                                          "Override boot loader request."));
    options.push_back(ConfigurationOption('B', "appboot-url", ConfigurationOption::URL_ARGUMENT,
                                          &sAppbootUrl, "Set the URL to AppBoot."));
    options.push_back(ConfigurationOption(0, "appboot-params", ConfigurationOption::STRING_ARGUMENT,
                                          &sAppbootParams, "Specify additional query params for AppBoot."));
    options.push_back(ConfigurationOption('J', "js-opts", &NrdConfiguration::parseJSOpts, "Set arguments that JS knows how to use."));

    options.push_back(ConfigurationOption('Q', "ui-query-string", ConfigurationOption::STRING_ARGUMENT,
                                          &sQueryString, "Arguments passed to the UI url."));
    options.push_back(ConfigurationOption(0, "smap-directory", ConfigurationOption::STRING_ARGUMENT,
                                          &sSMAPDirectory, "Location to dump smap information to."));
#ifdef NRDP_MDX_ENABLED
    options.push_back(ConfigurationOption(0, "mdx",
                                          &sMDXEnabled, "MDX enabled."));
#endif
#if !defined(BUILD_PRODUCTION) || 1
    options.push_back(ConfigurationOption('X', "allow-insecure-requests",
                                          &sAllowInsecureNetworkRequests, "Allow insecure network requests."));
#endif
#ifdef NRDP_WEBSERVER_ENABLED
    options.push_back("NBPD Options");
    options.push_back(ConfigurationOption('i', "nbpd-all-interfaces",
                                          &sAllowAllInterfaces, "Allow NBPD to bind to all interfaces."));
    options.push_back(ConfigurationOption(0, "nbpd",
                                          &sNBPDEnabled, "Enable the NBPD/HTTP server."));
    options.push_back(ConfigurationOption('p', "nbpd-port", ConfigurationOption::INT_ARGUMENT,
                                          &sNBPDPort, "Set the NBP port to <value>."));
#endif
#ifdef NRDP_DPI_CONFIGURATION_OPTIONS
    {
        ConfigurationOptions deviceLib = ::getDeviceLibOptions();
        for(ConfigurationOptions::const_iterator it = deviceLib.begin(); it != deviceLib.end(); ++it)
            options.push_back(*it);
    }
#endif
#ifdef NRDP_MDX_ENABLED
    {
        ConfigurationOptions mdxLib = netflix::mdx::Configuration::getMdxLibOptions();
        for(ConfigurationOptions::const_iterator it = mdxLib.begin(); it != mdxLib.end(); ++it)
            options.push_back(*it);
    }
#endif

    options.push_back("NRD Options");

    options.push_back(ConfigurationOption(0, "nbp-threadpool-max-threadcount", ConfigurationOption::INT_ARGUMENT,
                                          &sNBPThreadPoolMaxThreadCount,
                                          "Maximum number of threads to use for NBP's message pump."));
    options.push_back(ConfigurationOption(0, "nbp-threadpool-min-threadcount", ConfigurationOption::INT_ARGUMENT,
                                          &sNBPThreadPoolMinThreadCount,
                                          "Minimum number of threads to use for NBP's message pump."));
    options.push_back(ConfigurationOption(0, "nbp-threadpool-idle-time", ConfigurationOption::TIME_ARGUMENT,
                                          &sNBPThreadPoolIdleTime,
                                          "Time to wait before shutting down extranous NBP thread pool threads."));
    options.push_back(ConfigurationOption(0, "video-bitrate-ranges", ConfigurationOption::STRING_ARGUMENT,
                                          &sVidRanges, "Video bitrates to use: profile:min-max[,...]"));
    options.push_back(ConfigurationOption(0, "min-audio-bitrate", ConfigurationOption::INT_ARGUMENT,
                                          &sMinAudRate, "Minimum audio bitrate to use"));
    options.push_back(ConfigurationOption(0, "max-audio-bitrate", ConfigurationOption::INT_ARGUMENT,
                                          &sMaxAudRate, "Maximum audio bitrate to use"));
    options.push_back(ConfigurationOption(0, "send-stream-info", &sSendStreamInfoEvents, "Send stream information events to js"));

#if !defined(BUILD_PRODUCTION)
    options.push_back(ConfigurationOption(0, "js-assert", &sJsAsserts, "Assert on uncaught JS exceptions."));
#endif
    options.push_back(ConfigurationOption(0, "media-cache-capacity", ConfigurationOption::SIZE_ARGUMENT,
                                          &sMediaCacheCapacity, "Max size of media cache"));
    options.push_back(ConfigurationOption(0, "js-delay", ConfigurationOption::TIME_ARGUMENT,
                                          &sJsDelay, "Maximum amount of milliseconds a JS event should wait to be handled before warning"));
    options.push_back(ConfigurationOption(0, "quit-timer", ConfigurationOption::TIME_ARGUMENT,
                                          &sQuitTimerInterval, "Quit after this many milliseconds"));

    ConfigurationOptions instrumentationOptions = netflix::nrdlog::InstrumentationThread::defaultOptions();
    for(ConfigurationOptions::const_iterator it = instrumentationOptions.begin(); it != instrumentationOptions.end(); ++it)
        options.push_back(*it);

    return options;
}

bool NrdConfiguration::parseJSOpts(ConfigurationOption::FunctionMode mode, Variant &variant)
{
    if (mode == ConfigurationOption::Mode_Set) {
        if (!sJsOpts.empty())
            sJsOpts += ',';
        sJsOpts += variant.value<std::string>();
    } else if(mode == ConfigurationOption::Mode_Get) {
        variant = sJsOpts;
    } else {
        assert(false);
    }
    return true;
}

static bool parseUrl(ConfigurationOption::FunctionMode mode, Variant &location, Variant &variant)
{
    if(mode == ConfigurationOption::Mode_Set) {
        std::string argument = variant.value<std::string>();
        if (argument[0] == '@') {
            bool ok;
            argument = DataBuffer::fromFile(argument.substr(1), 1024 * 1024, &ok).toString();
            if (!ok)
                return false;
        }
        if (argument[0] == '{') {
            bool ok;
            location = Variant::fromJSON(argument, &ok);
            return ok;
        }

        const AseUrl url(argument);
        if (!url.HasScheme()) {
            std::string path = url.GetPath();
            if (ReadDir::isFile(path) && startsWith(path, "./")) {
                char buf[PATH_MAX];
                if (getcwd(buf, sizeof(buf)))
                    path.replace(0, 1, buf);
            }
            location = Variant::StringMap("url", "file://" + path);
        } else {
            location = Variant::StringMap("url", argument);
        }
    } else if(mode == ConfigurationOption::Mode_Get) {
        variant = location;
    } else {
        assert(false);
    }
    return true;
}

bool NrdConfiguration::parseBootRequest(ConfigurationOption::FunctionMode mode, Variant &variant)
{
    return parseUrl(mode, sBootRequest, variant);
}

bool NrdConfiguration::parseUIRequest(ConfigurationOption::FunctionMode mode, Variant &variant)
{
    return parseUrl(mode, sUIRequest, variant);
}

