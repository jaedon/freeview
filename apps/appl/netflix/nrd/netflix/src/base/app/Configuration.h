/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <string>
#include <vector>
#include <nrdbase/StdInt.h>
#include <nrdbase/config.h>

#include <nrdbase/Mutex.h>
#include <nrdbase/MutexStack.h>
#include <nrdbase/ConfigData.h>
#include <nrdbase/Variant.h>

namespace netflix {

struct ConfigurationOption
{
    char shortOption;
    const char *longOption;
    enum ArgumentType {
        NO_ARGUMENT            = 0x00000,
        STRING_ARGUMENT        = 0x00001,
        PATH_ARGUMENT          = STRING_ARGUMENT | 0x00010,
        DATAPATH_ARGUMENT      = PATH_ARGUMENT | 0x00020,
        DATAWRITEPATH_ARGUMENT = PATH_ARGUMENT | 0x00040,
        URL_ARGUMENT           = STRING_ARGUMENT | 0x00080,
        INT_ARGUMENT           = 0x00002,
        SIZE_ARGUMENT          = INT_ARGUMENT | 0x00100,
        TIME_ARGUMENT          = INT_ARGUMENT | 0x00200,
        BOOL_ARGUMENT          = 0x00004,
        TRUE_ARGUMENT          = BOOL_ARGUMENT | 0x01000,
        FALSE_ARGUMENT         = BOOL_ARGUMENT | 0x02000,
        FUNCTION_ARGUMENT      = 0x00008,
        IGNORE_ARGUMENT        = 0x20000
    };

    enum Source {
        Source_Default,
        Source_Xml,
        Source_EnvironmentVariable,
        Source_CommandLine
    };

    enum FunctionMode {
        Mode_Set,
        Mode_Get,
        Mode_Flip
    };
    typedef bool (*FunctionArgumentType)(FunctionMode mode, Variant &variant);
    unsigned int flags;
    Source source;
    void *argument;
    const char *description;

    ConfigurationOption(const char *desc = 0)
        : shortOption(0), longOption(0), flags(NO_ARGUMENT), source(Source_Default), argument(0), description(desc)
    {}
    ConfigurationOption(char shortOpt, const char *longOpt, unsigned int argFlags, llong *arg, const char *desc)
        : shortOption(shortOpt), longOption(longOpt), flags(argFlags), source(Source_Default), argument(arg), description(desc)
    {
        assert(argument);
        assert(flags & INT_ARGUMENT);
    }

    ConfigurationOption(char shortOpt, const char *longOpt, bool *arg, const char *desc)
        : shortOption(shortOpt), longOption(longOpt), flags(BOOL_ARGUMENT), source(Source_Default), argument(arg), description(desc)
    {
        assert(argument);
        if (*arg) {
            flags |= TRUE_ARGUMENT;
        } else {
            flags |= FALSE_ARGUMENT;
        }
    }

    ConfigurationOption(char shortOpt, const char *longOpt, unsigned int argFlags, std::string *arg, const char *desc)
        : shortOption(shortOpt), longOption(longOpt), flags(argFlags), source(Source_Default), argument(arg), description(desc)
    {
        assert(argument);
        assert(flags & STRING_ARGUMENT);
    }

    ConfigurationOption(char shortOpt, const char *longOpt, FunctionArgumentType func, const char *desc)
        : shortOption(shortOpt), longOption(longOpt), flags(FUNCTION_ARGUMENT), source(Source_Default),
          argument(reinterpret_cast<void*>(func)), description(desc)
    {
        assert(argument);
    }

    ConfigurationOption(char shortOpt, const char *longOpt, unsigned int argFlags, FunctionArgumentType func, const char *desc)
        : shortOption(shortOpt), longOption(longOpt), flags(FUNCTION_ARGUMENT|argFlags), source(Source_Default),
          argument(reinterpret_cast<void*>(func)), description(desc)
    {
        assert(argument);
    }


    void finalize();
    std::string dump(bool dumpExtra=false) const;
    Variant variantArgument() const;
    std::string printArgument() const;
    bool parseArgument(const char *a);
    bool flipBool();
    bool requiresArgument() const { return !(flags & (BOOL_ARGUMENT|FUNCTION_ARGUMENT)); }
    bool hasArgument() const { return flags != NO_ARGUMENT; }
};
typedef std::vector<ConfigurationOption> ConfigurationOptions;

class Configuration
{
public:
    static bool isValid();

    static llong parseInt(const char *str, unsigned int flags, bool *ok = 0);

    inline static std::string configFile() { return sConfigFile; }
    inline static void setConfigFile(const std::string &configFile) { sConfigFile = configFile; }
    inline static std::string binaryHash() { return sBinaryHash; }

    inline static const std::string &logFile() { return sLogFile; }
    inline static bool logFileEnabledTracesOnly() { return sLogFileEnabledTracesOnly; }
    inline static const std::string &dataReadPath() { return sDataReadDirectory; }
    inline static const std::string &dataWritePath() { return sDataWriteDirectory; }

    static std::string fixPath(std::string path, const std::string &dpath);
    inline static std::string fixDatapath(std::string path) { return fixPath(path, dataReadPath()); }

    static const std::string &resourcesPath() { return sResourcesDirectory; }

    inline static const std::string &diskStorePath() { return sDiskStoreDirectory; }
    inline static const std::string &crashLogPath() { return sCrashLogDirectory; }
    inline static bool crashReportEnabled() { return sCrashReportEnabled; }
    inline static bool crashReportShutdowns() { return sCrashReportShutdowns; }
    inline static llong logCacheCapacity() { return sLogCacheCapacity; }
    inline static llong uiCacheCapacity() { return sUICacheCapacity; }

    // Connection Pool
    static bool reuseSocketConnection() {
        if(sMaxNumReusedSocketConnection > 0 ){
            return true;
        } else {
            return false;
        }
    }
    static llong maxNumReusedSocketConnection() { return sMaxNumReusedSocketConnection; }
    static void setMaxNumReusedSocketConnection(llong num) {
#if defined(NRDP_LIMIT_SOCKET_COUNT)
        num = 0;
#endif
        sMaxNumReusedSocketConnection = num;
    }

    static void setSuspendOnStart(const std::string &suspend) { sSuspendModeOnStart = suspend; }
    inline static std::string suspendModeOnStart() { return sSuspendModeOnStart; }
    inline static bool suspendModeEnabled() { return sSuspendModeEnabled; }

    inline static bool telnetEnabled() { return sTelnetEnabled; }
    inline static llong telnetPort() { return sTelnetPort; }

    inline static std::string dnsServers() { return sDnsServers; }
    inline static std::string dnsFallbackServers() { return sDnsFallbackServers; }
    inline static bool dnsCacheEnabled() { return sDnsCacheEnabled; }

    inline static bool consoleEnabled() { return sConsoleEnabled; }
    inline static bool consoleSinkLogColor() { return sConsoleSinkLogColor; }
    inline static void setConsoleSinkLogColor(bool on) { sConsoleSinkLogColor = on; }
    inline static bool consoleSinkLogAbsolute() { return sConsoleSinkLogAbsolute; }
    inline static bool consoleSinkLogProcess() { return sConsoleSinkLogProcess; }
    inline static unsigned consoleSinkLogLevel() { return sConsoleSinkLogLevel; }

    inline static std::vector<unsigned char> ilSecret() { return std::vector<unsigned char>(sILSecret.begin(), sILSecret.end()); }
    inline static std::string ilDirectory() { return sILDirectory; }

    inline static const std::string &traceAreas() { return sTraceAreas; }
    inline static bool traceAreasOnly() { return sTraceAreasOnly; }

    static void resolveUrlArgument(std::string &url);

    enum Color {
        Invalid,
        Default,
        Black,
        Red,
        Green,
        Yellow,
        Blue,
        Magenta,
        Cyan,
        White,
        BrightDefault,
        BrightBlack,
        BrightRed,
        BrightGreen,
        BrightYellow,
        BrightBlue,
        BrightMagenta,
        BrightCyan,
        BrightWhite
    };

    static Color toColor(const std::string &name);
    static const char *colorToEscapeCode(Color color);

    struct ColorScheme
    {
        ColorScheme() { reset(Invalid); }
        void reset(Color col) { logHeader = trace = info = warning = error = fatal = grep = col; }
        bool parse(const std::string &spec);
        bool isNull() const
        {
            return (logHeader == Invalid && trace == Invalid && info == Invalid
                    && warning == Invalid && error == Invalid && fatal == Invalid && grep == Invalid);
        }
        std::string describe() const;

        Color logHeader, trace, info, warning, error, fatal, grep;
    };

    static ColorScheme terminalColors() { return sTerminalColorscheme; }
    static void setTerminalColors(const ColorScheme &scheme) { sTerminalColorscheme = scheme; }
    static ColorScheme telnetColors() { return sTelnetColorscheme; }
    static void setTelnetColors(const ColorScheme &scheme) { sTelnetColorscheme = scheme; }

    static ConfigurationOptions defaultOptions();
    static void addDefaultOption(const ConfigurationOption &option);
    static bool setOverrideContents(const std::string &xml);
    inline static std::vector<std::string> arguments() { return arguments(defaultOptions()); }
    static std::vector<std::string> arguments(const ConfigurationOptions &options);
    static bool init(int argc, char **argv, const ConfigurationOptions &options);
    inline static bool init(int argc, char **argv) { return init(argc, argv, defaultOptions()); }

    inline static const Variant &configDataValues() { return sConfigDataValues; }
    inline static const ConfigurationOptions &finalOptionValues() { return sFinalOptions; }
    static ConfigurationOption::Source source(const std::string &longOption);

    static bool resourceExists(const std::string &path);
    static std::string resourceURL(const std::string &path);
    static DataBuffer resourceContent(const std::string &path);
    static void replaceTokens(DataBuffer &content, const Variant &params);

    static inline llong eventloopMaxSendTime() { return sEventLoopMaxSendTime; }
    static inline llong eventloopMaxPendingTime() { return sEventLoopMaxPendingTime; }
    static inline llong eventloopMaxTimerTime() { return sEventLoopMaxTimerTime; }

    static llong diskCacheCapacity() { return sDiskCacheCapacity; }
    static llong diskCacheMaxPending() { return sDiskCacheMaxPending; }
    static llong diskCacheWriteLimit() { return sDiskCacheWriteLimit; }
    static llong diskCacheCatalogTimer() { return sDiskCacheCatalogTimer; }
    static llong diskCacheWriteSuspendInterval() { return sDiskCacheWriteSuspendInterval; }
    static std::string diskCachePath() { return sDiskCachePath; }
    static llong resourceManagerCapacity() { return sResourceManagerCapacity; }
    static bool resourceManagerGzipCompressionEnabled() { return sResourceManagerGzipCompressionEnabled; }
    static std::string xmlOption(const std::string &longOption);
    static std::string jsOption(const std::string &longOption);
    static std::string environmentVariableOption(const std::string &longOption);
    static bool resetWriteLimits() { return sResetWriteLimits; }

#ifdef NRDP_MUTEX_DETECT_DEADLOCK
    inline static llong mutexLockTimeTrackingInterval() { return sMutexLockTimeTrackingInterval; }
#endif
#ifdef NRDP_MUTEX_TRACK_LOCK
    inline static llong mutexTrackCount() { return sMutexTrackCount; }
    inline static llong mutexTrackBacktrace() { return sMutexTrackBacktrace; }
#endif
#ifdef NRDP_MUTEX_STACK
    inline static bool mutexStackEnabled() { return sMutexStackEnabled; }
#endif
#ifdef NRDP_THREAD_TRACK_LOCK
    inline static bool mutexThreadTrackLock() { return sMutexThreadTrackLock; }
#endif
    inline static llong objectCountTimerInterval() { return sObjectCountTimerInterval; }
#ifdef NRDP_HAS_SOCKETIZER
    static std::string socketizerConditionFile() { return sSocketizerConditionFile; }
#endif
    static bool setSystemTime(const std::string &argument);
private:
    Configuration();

    static bool setTime(ConfigurationOption::FunctionMode mode, Variant &variant);
    static bool setSilent(ConfigurationOption::FunctionMode mode, Variant &variant);
    static bool setEnvironmentVariable(ConfigurationOption::FunctionMode mode, Variant &variant);
    static bool setSuspendOnStart(ConfigurationOption::FunctionMode mode, Variant &variant);
    static bool addCapability(ConfigurationOption::FunctionMode mode, Variant &variant);

    static void loadOverrideFile();

    static ConfigurationOptions sFinalOptions;
    static Variant sConfigDataValues;

    static llong sEventLoopMaxSendTime;
    static llong sEventLoopMaxTimerTime;
    static llong sEventLoopMaxPendingTime;
    static llong sTelnetPort;
    static bool sTelnetEnabled;
    static std::string sDnsServers;
    static std::string sDnsFallbackServers;
    static bool sDnsCacheEnabled;
    static std::string sDnsDomains;
#ifdef NRDP_MUTEX_DETECT_DEADLOCK
    static llong sMutexLockTimeTrackingInterval;
#endif
#ifdef NRDP_MUTEX_TRACK_LOCK
    static llong sMutexTrackCount;
    static llong sMutexTrackBacktrace;
#endif
#ifdef NRDP_MUTEX_STACK
    static bool sMutexStackEnabled;
#endif
#ifdef NRDP_THREAD_TRACK_LOCK
    static bool sMutexThreadTrackLock;
#endif
    static std::string sTraceAreas;
    static bool sTraceAreasOnly;
    static bool sConsoleEnabled;
    static bool sConsoleSinkLogColor;
    static bool sConsoleSinkCallstack;
    static bool sConsoleSinkLogAbsolute;
    static bool sConsoleSinkLogProcess;
    static llong sConsoleSinkLogLevel;
    static std::string sLogFile;
    static bool sLogFileEnabledTracesOnly;

    static ConfigurationOptions sDefaultOptions;
    static std::string sConfigFile;    // Location of the config xml file
    static std::string sBinaryHash;
    static std::string sDataReadDirectory; // Location of the data directory
    static std::string sDataWriteDirectory; // Location of the data directory
    static std::string sDiskStoreDirectory;
    static bool sResetWriteLimits;
    static std::string sCrashLogDirectory;
    static std::string sILDirectory;
    static std::string sILSecret;
    static bool sCrashReportEnabled;
    static bool sCrashReportShutdowns;
    static llong sLogCacheCapacity;
    static llong sUICacheCapacity;

    static bool sSuspendModeEnabled;
    static std::string sSuspendModeOnStart;

    // Connection Pool
    static llong sMaxNumReusedSocketConnection;

    static std::string sResourcesDirectory;

    static std::string sTerminalColors;
    static std::string sTelnetColors;

    static ColorScheme sTerminalColorscheme;
    static ColorScheme sTelnetColorscheme;

    static llong sObjectCountTimerInterval;

    static llong sDiskCacheCapacity;
    static llong sDiskCacheMaxPending;
    static llong sDiskCacheWriteLimit;
    static llong sDiskCacheCatalogTimer;
    static llong sDiskCacheWriteSuspendInterval;
    static llong sResourceManagerCapacity;
    static bool sResourceManagerGzipCompressionEnabled;
    static std::string sDiskCachePath;

#ifdef NRDP_HAS_SOCKETIZER
    static std::string sSocketizerConditionFile;
#endif
};

}

#endif // CONFIGUeRATION_H
