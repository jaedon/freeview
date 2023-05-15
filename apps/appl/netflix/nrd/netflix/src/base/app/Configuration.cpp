/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "Configuration.h"

#include <nrdbase/ConfigData.h>
#include <nrdbase/Configuration.h>
#include <nrdbase/Log.h>
#include <nrdbase/EmbeddedResources.h>
#include <nrdbase/StringTokenizer.h>
#include <nrdbase/StringUtils.h>
#include <nrdbase/UnionCast.h>
#include <nrdbase/HMAC.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <iterator>
#include <algorithm>
#include <errno.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <assert.h>

using namespace netflix;

static const unsigned char sOverrideSecret[48] = {
    0xc2, 0xd4, 0x0d, 0xf3, 0xc5, 0x21, 0x8c, 0x69,
    0x77, 0xfc, 0x5d, 0xd0, 0x1a, 0x98, 0xb9, 0x5d,
    0x39, 0xe5, 0xae, 0x9d, 0xb9, 0xa9, 0x6f, 0x58,
    0x40, 0x7a, 0x10, 0x40, 0x81, 0x0c, 0xe1, 0x6b,
    0xf0, 0xb7, 0xff, 0xb2, 0x04, 0x13, 0x61, 0x74,
    0xda, 0xd4, 0xec, 0x82, 0x8c, 0x19, 0x17, 0x9e
};

enum PathFileType {
    NotExistType = 0,
    DirectoryType = 1,
    FileType = 2
};

static PathFileType pathExists(const std::string &path)
{
#ifdef NRDP_NO_FILESYSTEM
    return NotExistType;
#else
    struct stat stat_buff;
    if(::stat(path.c_str(), &stat_buff))
        return NotExistType;
    if(S_ISDIR(stat_buff.st_mode))
        return DirectoryType;
    return FileType;
#endif
}

bool pathAbsolute(const std::string &path)
{
    return path[0] == '/' || path.find(':') != std::string::npos;
}

#ifdef BUILD_DEBUG
static bool sShowDump = true;
#else
static bool sShowDump = false;
#endif
static bool sShowDumpConfig = false;
static bool sShowHelp = false;

std::string Configuration::sResourcesDirectory = "resources/";

ConfigurationOptions Configuration::sFinalOptions;
Variant Configuration::sConfigDataValues;
#if defined(BUILD_DEBUG)
bool Configuration::sTelnetEnabled = true;
#else
bool Configuration::sTelnetEnabled = false;
#endif
std::string Configuration::sDnsServers;
std::string Configuration::sDnsFallbackServers;
bool Configuration::sDnsCacheEnabled = true;
llong Configuration::sTelnetPort = 9536;
#ifdef NRDP_MUTEX_DETECT_DEADLOCK
llong Configuration::sMutexLockTimeTrackingInterval = 0;
#endif
#ifdef NRDP_MUTEX_TRACK_LOCK
llong Configuration::sMutexTrackCount = 0;
llong Configuration::sMutexTrackBacktrace = 0;
#endif
#ifdef NRDP_MUTEX_STACK
bool Configuration::sMutexStackEnabled = true;
#endif
#ifdef NRDP_THREAD_TRACK_LOCK
#if defined(BUILD_DEBUG) && defined(__i386__)
bool Configuration::sMutexThreadTrackLock = true;
#else
bool Configuration::sMutexThreadTrackLock = false;
# endif
#endif
std::string Configuration::sTraceAreas;
bool Configuration::sTraceAreasOnly = false;

llong Configuration::sEventLoopMaxSendTime = 1000;
llong Configuration::sEventLoopMaxTimerTime = 1000;
llong Configuration::sEventLoopMaxPendingTime = 250;
llong Configuration::sConsoleSinkLogLevel = Log::Warn;
#if defined(__i386__)
bool Configuration::sConsoleEnabled = true;
#else
bool Configuration::sConsoleEnabled = false;
#endif
bool Configuration::sConsoleSinkLogColor = true;
bool Configuration::sConsoleSinkLogAbsolute = true;
bool Configuration::sConsoleSinkLogProcess = false;
bool Configuration::sLogFileEnabledTracesOnly = false;
#if defined(BUILD_DEBUG) && defined(__i386__)
std::string Configuration::sLogFile = "netflix.log"; // stupid file
#else
std::string Configuration::sLogFile;
#endif

const char *defaultColors = "logHeader=brightdefault,info=brightblue,warning=brightyellow,error=brightred,fatal=brightmagenta,grep=brightcyan";
std::string Configuration::sTerminalColors = defaultColors;
std::string Configuration::sTelnetColors = defaultColors;

Configuration::ColorScheme Configuration::sTerminalColorscheme;
Configuration::ColorScheme Configuration::sTelnetColorscheme;

ConfigurationOptions Configuration::sDefaultOptions;
std::string Configuration::sDataReadDirectory = "./data";
std::string Configuration::sDataWriteDirectory = "var";
std::string Configuration::sDiskStoreDirectory = "cache";
bool Configuration::sResetWriteLimits = false;
std::string Configuration::sILSecret;
std::string Configuration::sILDirectory = "tiles";
std::string Configuration::sCrashLogDirectory = Configuration::sDiskStoreDirectory + NRDP_PATH_SEPARATOR_CHAR + "crashlog";
bool Configuration::sCrashReportEnabled = true;
bool Configuration::sCrashReportShutdowns = true;
std::string Configuration::sConfigFile;
std::string Configuration::sBinaryHash;
llong Configuration::sLogCacheCapacity = 0;
llong Configuration::sUICacheCapacity = 0;
// socket connection pool disabled by default
// by setting zero for number of sockets
llong Configuration::sMaxNumReusedSocketConnection = 0;
llong Configuration::sObjectCountTimerInterval = 0;
bool Configuration::sSuspendModeEnabled = true;
std::string Configuration::sSuspendModeOnStart;

#if defined(__i386__)
llong Configuration::sDiskCacheCapacity = 12 * 1024 * 1024;
llong Configuration::sDiskCacheWriteLimit = 0;
#else
llong Configuration::sDiskCacheWriteLimit = 0;
llong Configuration::sDiskCacheCapacity = 0;
#endif

llong Configuration::sDiskCacheMaxPending = -1;
llong Configuration::sDiskCacheCatalogTimer = 30;
llong Configuration::sDiskCacheWriteSuspendInterval = 60000;
std::string Configuration::sDiskCachePath = "nrd/gibbon";
llong Configuration::sResourceManagerCapacity = 5 * 1024 * 1024;
bool Configuration::sResourceManagerGzipCompressionEnabled = true;

#ifdef NRDP_HAS_SOCKETIZER
std::string Configuration::sSocketizerConditionFile;
#endif

const resources::ResourceEntry* getResource(const std::string &path)
{
    NTRACE(TRACE_RESOURCES, "(getResource) Retrieve %s", path.c_str());

    std::string resourcePath = path;
    if(!resourcePath.compare(0, Configuration::dataReadPath().length(), Configuration::dataReadPath()))
        resourcePath = resourcePath.substr(Configuration::dataReadPath().length() + 1);

    const resources::ResourceEntry* result = resources::getResourceEntry(resourcePath);

    if(!result) {
        NTRACE(TRACE_RESOURCES, "(getResource) %s is not present in the executable", resourcePath.c_str());
    } else {
        NTRACE(TRACE_RESOURCES, "(getResource) %s is present in the executable", resourcePath.c_str());
    }

    return result;
}


std::string Configuration::resourceURL(const std::string &path)
{

#ifndef BUILD_DEBUG
    if(getResource(path))
        return std::string();
#endif

#ifndef NRDP_NO_FILESYSTEM
    if(pathExists(path)) {
        std::string absPath = path;
        if(!pathAbsolute(absPath)) {
            char cwd[2048];
            absPath = getcwd(cwd, 2048) + std::string("/") + absPath;
        }

        std::string result = "file://" + absPath;

#ifdef NRDP_COMPILED_RESOURCES
        NTRACE(TRACE_RESOURCES, "(file override) %s -> %s", path.c_str(), result.c_str());
#endif

        return result;
    }
#endif

    return std::string();
}

bool Configuration::resourceExists(const std::string &path)
{
    if(getResource(path))
        return true;
    PathFileType exists = pathExists(path);
    if(!exists && !pathAbsolute(path) &&
       path.compare(0, Configuration::dataReadPath().length(), Configuration::dataReadPath())) {
        exists = pathExists(Configuration::dataReadPath() + "/" + path);
    }

    if(exists == FileType)
        return true;
    return false;
}

DataBuffer Configuration::resourceContent(const std::string &path)
{
    DataBuffer output;
    if(const resources::ResourceEntry* p = getResource(path)) {
        ullong resourceSize = p->end - p->start;
        output = DataBuffer::fromRawData(reinterpret_cast<const unsigned char*>(p->start), resourceSize);
#ifndef BUILD_DEBUG
        return output;
#endif
    }

    std::string filePath = path;
    PathFileType exists = pathExists(path);
    // Look for a file matching the resource name. Try also inside the data directory if the file path requested is not in it.
    if(!exists && !pathAbsolute(filePath) &&
       filePath.compare(0, Configuration::dataReadPath().length(), Configuration::dataReadPath())) {
        filePath = Configuration::dataReadPath() + "/" + filePath;
        exists = pathExists(filePath);
    }

    if(exists == FileType) {
        output = DataBuffer::fromFile(filePath.c_str());
#ifdef NRDP_COMPILED_RESOURCES
        NTRACE(TRACE_RESOURCES, " [file override] %s -> %s", path.c_str(), filePath.c_str());
#endif
    }

    if (output.isEmpty()) {
        NTRACE(TRACE_RESOURCES, "[not found] %s", path.c_str());
    } else {
        NTRACE(TRACE_RESOURCES, "[found] %s", path.c_str());
    }

    return output;
}

void Configuration::replaceTokens(DataBuffer &content, const Variant &replace)
{
    if(replace.isStringMap()) {
        const Variant::StringMapIterator end = replace.stringMapEnd();
        for(Variant::StringMapIterator it = replace.stringMapBegin(); it != end; ++it) {
            const DataBuffer key = '%' + it->first + '%';
            const DataBuffer value = it->second.value<DataBuffer>();
            int found = 0;
            while (true) {
                found = content.indexOf(key, found);
                if(found == -1)
                    break;
                content.replace(found, key.length(), value);
                found += value.length();
            }
        }
    }
}


static bool fileOrResourceExists(std::string path)
{
    bool result = false;
    result = getResource(path);

    if(!result)
        result = pathExists(path) == FileType;

    return result;
}

void ConfigurationOption::finalize()
{
    if(flags & STRING_ARGUMENT) {
        std::string &str = *static_cast<std::string*>(argument);
        if((flags & PATH_ARGUMENT) == PATH_ARGUMENT) {
            if((flags & DATAWRITEPATH_ARGUMENT) == DATAWRITEPATH_ARGUMENT) {
                str = Configuration::fixPath(str, Configuration::dataWritePath());
            } else if((flags & DATAPATH_ARGUMENT) == DATAPATH_ARGUMENT) {
                str = Configuration::fixPath(str, Configuration::dataReadPath());
            } else {
                if(!str.empty() && !startsWith(str, "file://"))
                    str.insert(0, "file://");
            }
        } else if((flags & URL_ARGUMENT) == URL_ARGUMENT) {
            Configuration::resolveUrlArgument(str);
        }
    }
}

bool Configuration::isValid()
{
#if !defined(NRDP_NO_FILESYSTEM) && !defined(NRDP_COMPILED_RESOURCES)
    if(pathExists(Configuration::dataReadPath()) != DirectoryType) {
        fprintf(stderr, "Data path must exist!\n");
        return false;
    }
#endif
    if(!sTerminalColorscheme.parse(sTerminalColors)) {
        fprintf(stderr, "Invalid terminal colors %s\n", sTerminalColors.c_str());
        return false;
    }
    if(!sTelnetColorscheme.parse(sTelnetColors)) {
        fprintf(stderr, "Invalid telnet colors %s\n", sTelnetColors.c_str());
        return false;
    }
    if(sDiskCacheCapacity < 0) {
        fprintf(stderr, "--disk-cache-capacity must be >= 0!");
        return false;
    }
    if(sResourceManagerCapacity < 0) {
        fprintf(stderr, "--resource-cache-capacity must be >= 0!");
        return false;
    }

    if(sDiskCacheCapacity && sDiskCachePath.empty()) {
        fprintf(stderr, "DiskCachePath must not be empty!\n");
        return false;
    }

    if(!sILSecret.empty() && sILSecret.size() != 48) {
        fprintf(stderr, "--il-secret must be 48 bytes!\n");
        return false;
    }

    return true;
}

std::string Configuration::fixPath(std::string path, const std::string &dpath)
{
    (void)path;
    (void)dpath;
#ifndef NRDP_NO_FILESYSTEM
    if(!path.empty() && !startsWith(path, "../") && !startsWith(path, "./")
        && !pathAbsolute(path) && path.compare(0, dpath.size(), dpath)) {
        path.insert(0, (dpath + "/"));
    }
#endif
    return path;
}

void Configuration::resolveUrlArgument(std::string &url)
{
#if !defined(NRDP_NO_FILESYSTEM)
    const int idx = url.find('?');
    std::string query;
    if(idx != -1) {
        query = url.substr(idx);
        url = url.substr(0, idx);
    }
    char buf[PATH_MAX];
    if(realpath(url.c_str(), buf)) {
        url = "file://";
        url += buf;
    }
    if(!query.empty()) {
        url += query;
    }
#endif
    (void)url;
}

std::vector<std::string> Configuration::arguments(const ConfigurationOptions &options)
{
    std::vector<std::string> argv;
    // process default options
    for(ConfigurationOptions::const_iterator it = options.begin(); it != options.end(); ++it) {
        const ConfigurationOption &option = (*it);
        if(option.description || option.shortOption) {
            if(option.flags & ConfigurationOption::BOOL_ARGUMENT) {
                const bool enabled = *static_cast<bool*>(option.argument);
                if(enabled != static_cast<bool>((option.flags & ConfigurationOption::TRUE_ARGUMENT) == ConfigurationOption::TRUE_ARGUMENT)) {
                    std::string arg;
                    if(option.longOption) {
                        arg = "--" + std::string(option.longOption);
                    } else if(option.shortOption) {
                        arg = "-" + std::string(&option.shortOption, 1);
                    }
                    if(!enabled) {
                        arg += "=0";
                    }
                    argv.push_back(arg);
                }
            } else if(option.flags & ConfigurationOption::STRING_ARGUMENT) {
                if(option.longOption) {
                    argv.push_back("--" + std::string(option.longOption));
                } else if(option.shortOption) {
                    argv.push_back("-" + std::string(&option.shortOption, 1));
                }
                argv.push_back(*static_cast<std::string*>(option.argument));
            } else if(option.flags & ConfigurationOption::INT_ARGUMENT) {
                if(option.longOption) {
                    argv.push_back("--" + std::string(option.longOption));
                } else if(option.shortOption) {
                    argv.push_back("-" + std::string(&option.shortOption, 1));
                }
                std::stringstream stream;
                stream << *static_cast<llong*>(option.argument);
                argv.push_back(stream.str());
            }
        }
    }
    return argv;
}

std::string ConfigurationOption::dump(bool dumpExtra) const
{
    if(description) {
        std::ostringstream stream;
        if(!shortOption && !longOption && description) {
            stream << "\n" << description << ":\n";
        } else {
            if(shortOption)
                stream << "  -" << shortOption << "|";
            else
                stream << "     ";
            if(longOption) {
                stream << "--" << longOption << "|";
                if(flags & ConfigurationOption::BOOL_ARGUMENT)
                    stream << "--no-" << longOption << "|";
                if(dumpExtra) {
                    const std::string longOptionStr = longOption;
                    stream << Configuration::environmentVariableOption(longOptionStr) << "|" << Configuration::xmlOption(longOptionStr);
                }
                if(requiresArgument())
                    stream << " [value]";
            }
            if(description && *description)
                stream << ": " << description;
            std::string value = printArgument();
            if(!value.empty())
                stream << " (" << value << ")";
        }
        return stream.str();
    }
    return std::string();
}

static bool applyXML(const DataBuffer &contents, const std::string &file, ConfigData &config, ConfigurationOptions &options, std::string &error)
{
    if(!config.init(contents, file)) {
        Log::printf("Unable to parse configuration file: %s\n", file.c_str());
        return false;
    }

    for(ConfigurationOptions::iterator it = options.begin(); it != options.end(); ++it) {
        ConfigurationOption &option = (*it);
        if(option.longOption) {
            const std::string longOption = Configuration::jsOption(option.longOption); // we use jsOption since we don't need the <> here
            if(!longOption.empty()) {
                // printf("Looking for(cfg): %s\n", longOption.c_str());
                if(config.hasValue(longOption)) {
                    const std::string value = config[longOption];
                    if(!option.parseArgument(value.c_str())) {
                        error = "Unable to set XML option: " + longOption + ": '" + value + "'";
                        break;
                    }
                    option.source = ConfigurationOption::Source_Xml;
                } else if(option.flags & ConfigurationOption::BOOL_ARGUMENT) {
                    // printf("Trying to find %s\n", longOption.c_str());
                    struct {
                        const char *prefix;
                        const char *suffix;
                        const bool value;
                    } const permutations[] = {
                        { "no_", 0, false },
                        { "disable_", 0, false },
                        { 0, "_disabled", false },
                        { 0, "_enabled", true },
                        { "enable_", 0, true },
                        { 0, 0, false }
                    };

                    for(int i=0; permutations[i].prefix || permutations[i].suffix; ++i) {
                        std::string opt = longOption;
                        if(permutations[i].prefix)
                            opt.insert(0, permutations[i].prefix);
                        if(permutations[i].suffix)
                            opt += permutations[i].suffix;
                        // printf("XML Looking for %s\n", opt.c_str());
                        if(config.hasValue(opt)) {
                            const std::string value = config[opt];
                            if(!option.parseArgument(value.c_str())) {
                                error = "Unable to set XML option: " + longOption + ": '" + value + "'";
                                break;
                            }
                            option.source = ConfigurationOption::Source_Xml;
                            if(!permutations[i].value)
                                option.flipBool();
                            break;
                        }
                    }
                }
            }
        }
    }
    return error.empty();
}

bool Configuration::init(int argc, char **argv, const ConfigurationOptions &o)
{
    sFinalOptions = o;
    std::vector<DataBuffer> arguments;
    if(const char *e = getenv("NF_FLAGS")) {
        std::vector<std::string> flags;
        StringTokenizer::tokenize(e, flags, " ");
        const int size = flags.size();
        for(int i = 0; i < size; ++i)
            arguments.push_back(flags[i]);
    }
    for(int i = 1; i < argc; ++i)
        arguments.push_back(DataBuffer::fromRawData((const unsigned char *)argv[i], strlen(argv[i])));

#ifdef BUILD_DEBUG
    // sanity check commandline
    std::vector<char> shortOptions;
    std::vector<std::string> longOptions;
    for(ConfigurationOptions::iterator it = sFinalOptions.begin(); it != sFinalOptions.end(); ++it) {
        const ConfigurationOption &option = (*it);
        if(option.shortOption) {
            if(std::find(shortOptions.begin(), shortOptions.end(), option.shortOption) != shortOptions.end()) {
                Log::printf("Option: %c(%s): is used twice!\n", option.shortOption, option.longOption);
                return false;
            }
            shortOptions.push_back(option.shortOption);
        }
        if(option.longOption) {
            const std::string opt = option.longOption;
            if(std::find(longOptions.begin(), longOptions.end(), opt) != longOptions.end()) {
                Log::printf("Option: %c(%s): is used twice!\n", option.shortOption, option.longOption);
                return false;
            }
            longOptions.push_back(opt);
        }
    }
#endif
    if (getenv("NF_DUMP_UNUSED")) {
        bool used[256];
        memset(used, 0, sizeof(used));

        for (ConfigurationOptions::iterator it = sFinalOptions.begin(); it != sFinalOptions.end(); ++it)
            used[static_cast<unsigned char>(it->shortOption)] = true;
        for (size_t i=0; i<sizeof(used); ++i) {
            if (!used[i] && isalnum(i))
                Log::printf("-%c ", (char)i);
        }
        Log::printf("\n");
        exit(0);
    }

    if(const char* dataDirEnv = getenv("NF_DATA_DIR"))
        sDataReadDirectory = dataDirEnv;

    if(!startsWith(sDataReadDirectory, "/")) {
        char cwd[PATH_MAX];
        if(getcwd(cwd, sizeof(cwd) - 1)) {
            if(startsWith(sDataReadDirectory, "./")) {
                sDataReadDirectory.erase(0, 1);
            } else {
                cwd[strlen(cwd)] = '/';
            }

            sDataReadDirectory.insert(0, cwd);
        }
    }

    // configuration
    std::string configFile = sConfigFile;
    if(const char *e = getenv("NF_CONFIG_FILE"))
        configFile = e;
    const int argumentsSize = arguments.size();
    for(int i = 0; i < argumentsSize; ++i) {
        if(arguments[i] == "-x" || arguments[i] == "--config-file")
            configFile = arguments[i+1].toString();
    }
    sConfigFile = fixDatapath(configFile);
    if(!sConfigFile.empty() && !fileOrResourceExists(sConfigFile)) { // automatically prepend configuration path and xml suffix heuristic
        std::string h = fixDatapath("etc/conf/" + configFile + ".xml");
        if(fileOrResourceExists(h))
            sConfigFile = h;
    }
    // autodocument
    sFinalOptions.insert(sFinalOptions.begin(), ConfigurationOption('z', "dump", &sShowDump, 0));
    sFinalOptions.insert(sFinalOptions.begin(), ConfigurationOption('Z', "dump-config", &sShowDumpConfig, 0));
    sFinalOptions.insert(sFinalOptions.begin(), ConfigurationOption('h', "help", &sShowHelp, "Show Help"));

    // parse config.xml
    ConfigData config;
    //printf("ConfigFile: %s\n", sConfigFile.c_str());
    std::string error;
    if(!sConfigFile.empty()) {
        if(!fileOrResourceExists(sConfigFile)) {
            Log::printf("Unable to find configuration file: %s\n", sConfigFile.c_str());
            return false;
        } else {
            const DataBuffer data = Configuration::resourceContent(sConfigFile);
            if (!applyXML(data, sConfigFile, config, sFinalOptions, error)) {
                Log::printf("Unable to parse configuration file: %s (%s)\n", sConfigFile.c_str(), error.c_str());
                return false;
            }
        }
    }
    sConfigDataValues = config.values();
    // check environment variables
    for(ConfigurationOptions::iterator it = sFinalOptions.begin(); it != sFinalOptions.end(); ++it) {
        ConfigurationOption &option = (*it);
        if(option.longOption) {
            const std::string longOption = environmentVariableOption(option.longOption);
            if(!longOption.empty()) {
                // printf("Looking for(env): %s\n", longOption.c_str());
                if(const char *e = getenv(longOption.c_str())) {
                    if(!option.parseArgument(e)) {
                        error = "Unable to set environment variable: " + longOption + ": '" + e + "'";
                        break;
                    }
                    option.source = ConfigurationOption::Source_EnvironmentVariable;
                } else if(option.flags & ConfigurationOption::BOOL_ARGUMENT) {
                    struct {
                        const char *prefix;
                        const char *suffix;
                        const bool value;
                    } const permutations[] = {
                        { "NO_", 0, false },
                        { "DISABLE_", 0, false },
                        { 0, "_DISABLED", false },
                        { 0, "_ENABLED", true },
                        { "ENABLE_", 0, true },
                        { 0, 0, false }
                    };

                    for(int i=0; permutations[i].prefix || permutations[i].suffix; ++i) {
                        std::string opt = longOption;
                        if(permutations[i].prefix) {
                            opt.insert(3, permutations[i].prefix);
                            if(permutations[i].suffix)
                                opt += permutations[i].suffix;
                            // printf("ENV Looking for %s\n", opt.c_str());
                            if(const char *e = getenv(opt.c_str())) {
                                if(!option.parseArgument(e)) {
                                    error = "Unable to set environment variable: " + longOption + ": '" + e + "'";
                                } else if(!permutations[i].value) {
                                    option.flipBool();
                                }
                                option.source = ConfigurationOption::Source_EnvironmentVariable;
                                break;
                            }
                        }
                    }
                }
            }
        }
    }

    //parse commandline
    const int optionsLength = sFinalOptions.size();
    for(int idx=0; idx<argumentsSize && error.empty(); ++idx) {
        const DataBuffer &db = arguments[idx];
        const char *arg = db.c_str();
        const int argLength = db.size();
        const char *option = 0;
        const char *equals = 0;
        bool shortOption = false;
        int optionLength = -1;
        if(argLength >= 2) {
            if(argLength >= 3 && !strncmp(arg, "--", 2)) {
                option = arg + 2;
                equals = strchr(option + 1, '=');
                if(equals) {
                    optionLength = equals - option;
                } else {
                    optionLength = argLength - 2;
                }
            } else if(arg[0] == '-') {
                shortOption = true;
                option = arg + 1;
                optionLength = argLength - 1;
                if(optionLength > 2 && option[1] == '=') {
                    equals = option + 1;
                    optionLength = 1;
                }
            }
        }
        if(!option) {
            error = "Invalid option [";
            error += arg;
            error += ']';
            break;
        }
        assert(option);
        assert(optionLength > 0);

        std::string longOpt;
        if(!shortOption)
            longOpt.assign(option, equals ? equals-option : optionLength);

        while (optionLength) {
            bool boolValue = true;
            const char *shortOptValue = 0;
            ConfigurationOption *found = 0;
            {
                for(int i=0; !found && i<optionsLength; ++i) {
                    ConfigurationOption *opt = &sFinalOptions[i];
                    if(shortOption) {
                        if(opt->shortOption == *option) {
                            found = opt;
                            if(opt->requiresArgument() && optionLength > 1) {
                                shortOptValue = option + 1;
                                optionLength = 1;
                            }
                        }
                    } else if(opt->longOption) {
                        assert(!longOpt.empty());
                        if(opt->longOption == longOpt) {
                            found = opt;
                        } else if(opt->flags & ConfigurationOption::BOOL_ARGUMENT) {
                            struct {
                                const char *prefix;
                                const char *suffix;
                                const bool value;
                            } const permutations[] = {
                                { "no-", 0, false },
                                { "disable-", 0, false },
                                { 0, "-disabled", false },
                                { 0, "-enabled", true },
                                { "enable-", 0, true },
                                { 0, 0, false }
                            };
                            for(int i=0; permutations[i].prefix || permutations[i].suffix; ++i) {
                                std::string o;
                                if(permutations[i].prefix)
                                    o += permutations[i].prefix;
                                o += opt->longOption;
                                if(permutations[i].suffix)
                                    o += permutations[i].suffix;
                                // printf("Comparing %s and %s\n", o.c_str(), longOpt.c_str());
                                if(longOpt == o) {
                                    boolValue = permutations[i].value;
                                    found = opt;
                                }
                            }
                        }
                    }
                }
            }

            if(!found) {
                error = "Unrecognized option [";
                if(shortOption) {
                    error += *option;
                } else {
                    error += option;
                }
                error += ']';
                break;
            } else {
                found->source = ConfigurationOption::Source_CommandLine;
                if(equals && !found->hasArgument()) {
                    error = "option [";
                    if(shortOption) {
                        error += *option;
                    } else {
                        error += option;
                    }
                    error += "] does not take an argument";
                    break;
                } else if(found->requiresArgument() && !equals && !shortOption && idx + 1 >= argumentsSize) {
                    error = "option [";
                    if(shortOption) {
                        error += *option;
                    } else {
                        error += option;
                    }
                    error += "] takes an argument";
                    break;
                } else if(shortOption && optionLength > 1 && found->requiresArgument()) {
                    error = "Can't use argument for this option [";
                    if(shortOption) {
                        error += *option;
                    } else {
                        error += option;
                    }
                    error += ']';
                    break;
                }
            }

            const char *value = 0;
            if(found->hasArgument()) {
                if(equals) {
                    value = equals + 1;
                } else if(shortOptValue) {
                    value = shortOptValue;
                } else if(idx + 1 < argumentsSize && (!shortOption || optionLength == 1)
                          && (found->requiresArgument()
                              || ((!found->requiresArgument() && !arguments[idx + 1].startsWith("-"))))) {
                    value = arguments[++idx].c_str();
                }
            }
            if(!found->parseArgument(value)) {
                char buffer[2048];
                const int l = snprintf(buffer, 2048, "Unable to set option: %c(%s): '%s'",
                                       found->shortOption ? found->shortOption : ' ',
                                       found->longOption ? found->longOption : "",
                                       value ? value : "");
                error = std::string(buffer, l);
                break;
            }
            if((found->flags & ConfigurationOption::BOOL_ARGUMENT) && !boolValue)
                found->flipBool();
            if(shortOption && optionLength > 1) {
                --optionLength;
                ++option;
            } else {
                break;
            }
        }
    }
    if(!error.empty())
        sShowHelp = true;

    loadOverrideFile();
    // done
    for(ConfigurationOptions::iterator it = sFinalOptions.begin(); it != sFinalOptions.end(); ++it) {
        ConfigurationOption &option = (*it);
        option.finalize();
    }

    if(sShowHelp)
        Log::printf("----------------------------\nUsage: %s [options]\n----------------------------\n", argv ? argv[0] : "EXE");
    for(ConfigurationOptions::const_iterator it = sFinalOptions.begin(); it != sFinalOptions.end(); ++it) {
        const ConfigurationOption &option = (*it);
        if(sShowDump || (sShowHelp && option.description))
            Log::printf("%s\n", option.dump(sShowDump).c_str());
    }
    if(!error.empty())
        Log::printf("\nError: %s\n", error.c_str());
    if(sShowHelp)
        return false;
    if(sShowDumpConfig) {
        Log::printf("ConfigData:\n%s", sConfigDataValues.toJSON(true).c_str());
    }
    if(!Configuration::isValid()) {
        Log::printf("Invalid Configuration\n");
        return false;
    }
    return true;
}

Variant ConfigurationOption::variantArgument() const
{
    Variant ret;
    if(flags & STRING_ARGUMENT) {
        ret = printArgument();
    } else if(flags & FUNCTION_ARGUMENT) {
        ((FunctionArgumentType)argument)(Mode_Get, ret);
    } else if(flags & BOOL_ARGUMENT) {
        ret = *static_cast<bool*>(argument);
    } else if(flags & INT_ARGUMENT) {
        ret = *static_cast<llong*>(argument);
    }
    return ret;
}

std::string ConfigurationOption::printArgument() const
{
    std::string ret;
    if(flags & FUNCTION_ARGUMENT) {
        Variant r = variantArgument();
        ret = r.toJSON();
        if(r.isArray()) // remove [] from arrays when printing
            ret = ret.substr(1, ret.size() - 2);
    } else if(flags & BOOL_ARGUMENT) {
        ret = *static_cast<bool*>(argument) ? "true" : "false";
        if(*static_cast<bool*>(argument) != static_cast<bool>((flags & TRUE_ARGUMENT) == TRUE_ARGUMENT))
            ret += " *";
    } else if(flags & STRING_ARGUMENT) {
        ret = *static_cast<std::string*>(argument);
    } else if(flags & INT_ARGUMENT) {
        std::stringstream stream;
        stream << *static_cast<llong*>(argument);
        ret = stream.str();
    }
    return ret;
}

bool ConfigurationOption::parseArgument(const char *a)
{
    assert(argument);
    if(flags & IGNORE_ARGUMENT) {
        return true;
    } else if(flags & FUNCTION_ARGUMENT) {
        Variant variant(a);
        return ((FunctionArgumentType)argument)(Mode_Set, variant);
    } else if(flags & BOOL_ARGUMENT) {
        bool enabled = true;
        if(a && *a) {
            if(!strcasecmp(a, "true") || !strcmp(a, "1"))
                enabled = true;
            else if(!strcasecmp(a, "false") || !strcmp(a, "0"))
                enabled = false;
            else
                return false;
            // printf("%s=%s Value returns %d 0x%0x\n", longOption, a, enabled, flags);
        }
        *static_cast<bool*>(argument) = enabled;
        return true;
    } else if(flags & STRING_ARGUMENT) {
        if(a) {
            std::string b(a);
            if(a[0] == '$') {
                size_t slash = b.find('/');
                std::string env = b.substr(1, slash - 1);
                char *val = getenv(env.c_str());
                if(val)
                    b.replace(0, slash, val);
            }
            *static_cast<std::string*>(argument) = b;
            return true;
        }
    } else if(flags & INT_ARGUMENT) {
        if(a) {
            llong &arg = *static_cast<llong*>(argument);
            bool ok = true;
            arg = Configuration::parseInt(a, flags, &ok);
            return ok;
        }
    }
    return false;
}

bool ConfigurationOption::flipBool()
{
    assert(flags & BOOL_ARGUMENT);
    if(flags & FUNCTION_ARGUMENT) {
        Variant variant;
        return ((FunctionArgumentType)argument)(Mode_Flip, variant);
    }
    *static_cast<bool*>(argument) = !*static_cast<bool*>(argument);
    return true;
}

bool Configuration::setTime(ConfigurationOption::FunctionMode mode, Variant &variant)
{
    switch (mode) {
    case ConfigurationOption::Mode_Flip:
        assert(0);
        return false;
    case ConfigurationOption::Mode_Get:
        variant = Time::systemTime().rfc1123();
        break;
    case ConfigurationOption::Mode_Set: {
        const std::string str = variant.string();
        if(!str.empty()) {
            if(isdigit(str[0]) || str[0] == '.') {
                TimeMultiplier::setMultiplier(variant.value<float>());
                return true;
            } else if(setSystemTime(variant.value<std::string>())) {
                return true;
            }
        }
        return false; }
    }
    return true;
}

bool Configuration::setSilent(ConfigurationOption::FunctionMode mode, Variant &variant)
{
    switch (mode) {
    case ConfigurationOption::Mode_Flip:
        assert(0);
        return false;
    case ConfigurationOption::Mode_Get:
        variant = (sConsoleSinkLogLevel == INT_MAX);
        break;
    case ConfigurationOption::Mode_Set:
        sConsoleSinkLogLevel = INT_MAX;
        sShowDump = false;
        break;
    }
    return true;
}

bool Configuration::setEnvironmentVariable(ConfigurationOption::FunctionMode mode, Variant &variant)
{
    assert(mode != ConfigurationOption::Mode_Flip);
    if(mode == ConfigurationOption::Mode_Set) {
        std::string argument(variant.value<std::string>()), value("1");
        const size_t equal = argument.find("=");
        if(equal != std::string::npos) {
            value = argument.substr(equal+1, argument.length()-equal);
            argument = argument.substr(0, equal);
        }
        // printf("SetEnv: :%s::%s:\n", argument.c_str(), value.c_str());
        setenv(argument.c_str(), value.c_str(), 1);
    }
    return true;
}

bool Configuration::setSuspendOnStart(ConfigurationOption::FunctionMode mode, Variant &variant)
{
    if(mode == ConfigurationOption::Mode_Set) {
        std::string value = variant.value<std::string>();
        if(value.empty() || value == "true" || value == "1")
            value = "startup";
        else if(value == "none" || value == "false" || value == "0")
            value = "";
        Configuration::setSuspendOnStart(value);
    } else if(mode == ConfigurationOption::Mode_Get) {
        Configuration::setSuspendOnStart(sSuspendModeOnStart);
    } else if(mode == ConfigurationOption::Mode_Flip) {
        if(sSuspendModeOnStart.empty()) {
            Configuration::setSuspendOnStart("startup");
        } else {
            Configuration::setSuspendOnStart("");
        }
    }
    return true;
}

bool Configuration::addCapability(ConfigurationOption::FunctionMode mode, Variant &variant)
{
    switch (mode) {
    case ConfigurationOption::Mode_Flip:
        assert(0);
        return false;
    case ConfigurationOption::Mode_Get:
        variant = Configuration::sConfigDataValues["capabilities"];
        break;
    case ConfigurationOption::Mode_Set: {
        const std::string str = variant.string();
        const size_t eq = str.find('=');
        if (eq == std::string::npos)
            return false;
        const std::string keys = trimmed(str.substr(0, eq));
        const std::string value = trimmed(str.substr(eq + 1));
        std::vector<std::string> keySplit;
        StringTokenizer::tokenize(keys, keySplit, ".");
        const int count = keySplit.size();
        if (!count)
            return false;
        Variant *var = &Configuration::sConfigDataValues["capabilities"];
        for (int i=0; i<count; ++i) {
            if (!var->isNull() && !var->isStringMap()) {
                return false;
            }
            Variant &ref = (*var)[keySplit.at(i)];
            var = &ref;
        }
        if (var->isStringMap())
            return false;
        *var = value;
        break; }
    }
    return true;
}


ConfigurationOptions Configuration::defaultOptions()
{
    ConfigurationOptions options;
    options.push_back("Options");
    options.push_back(ConfigurationOption(0, "write-data-path", ConfigurationOption::DATAPATH_ARGUMENT,
                                          &sDataWriteDirectory, "Write persistent information here."));
    options.push_back(ConfigurationOption('R', "resources-path", ConfigurationOption::DATAPATH_ARGUMENT,
                                          &sResourcesDirectory, "Path to find UI resources in."));
    options.push_back(ConfigurationOption('x', "config-file", ConfigurationOption::STRING_ARGUMENT|ConfigurationOption::IGNORE_ARGUMENT,
                                          &sConfigFile, "Override the configuration XML file."));
    options.push_back(ConfigurationOption(0, "suspend-mode",
                                          &sSuspendModeEnabled, "Suspend handling enabled."));
    options.push_back(ConfigurationOption(0, "suspend", ConfigurationOption::BOOL_ARGUMENT, setSuspendOnStart, "Enter suspend mode immediate on startup."));
    options.push_back(ConfigurationOption(0, "num-of-reuse-socket-connection", ConfigurationOption::INT_ARGUMENT,
                                          &sMaxNumReusedSocketConnection, "Maximum number of connections to be reused"));
    options.push_back(ConfigurationOption(0, "cache-path", ConfigurationOption::DATAWRITEPATH_ARGUMENT,
                                          &sDiskStoreDirectory, "Write persistent cache here."));
    options.push_back(ConfigurationOption(0, "ui-cache-capacity", ConfigurationOption::SIZE_ARGUMENT,
                                          &sUICacheCapacity, "Set the ui cache capacity."));
    options.push_back(ConfigurationOption(0, "reset-write-limits", &sResetWriteLimits, "Reset write limits."));
    options.push_back(ConfigurationOption(0, "il-directory", ConfigurationOption::DATAWRITEPATH_ARGUMENT,
                                          &sILDirectory, "Write IL data here."));
    options.push_back(ConfigurationOption(0, "il-secret", ConfigurationOption::STRING_ARGUMENT,
                                          &sILSecret, "Shared secret for IL encryption."));
    options.push_back(ConfigurationOption('D', "define-environment", &Configuration::setEnvironmentVariable,
                                          "Define a variable into the environment."));
    options.push_back(ConfigurationOption(0, "time", &Configuration::setTime, "Override system time or time multiplier."));
    options.push_back(ConfigurationOption(0, "capability", &Configuration::addCapability, "Add capability."));
    options.push_back(ConfigurationOption('l', "loglevel", ConfigurationOption::INT_ARGUMENT,
                                          &sConsoleSinkLogLevel, "Override Console sink's log level."));
    options.push_back(ConfigurationOption(0, "silent", &Configuration::setSilent, "Be silent"));
    options.push_back(ConfigurationOption(0, "console", &sConsoleEnabled, "Enable console."));

    options.push_back(ConfigurationOption(0, "logcolor", &sConsoleSinkLogColor, "Color output enabled in Console log."));
    options.push_back(ConfigurationOption(0, "log-pid", &sConsoleSinkLogProcess, "Process enabled in Console log."));
    options.push_back(ConfigurationOption(0, "logabsolute", &sConsoleSinkLogAbsolute, "Absolute time enabled in Console log."));
    options.push_back(ConfigurationOption('L', "logfile", ConfigurationOption::DATAPATH_ARGUMENT,
                                          &sLogFile, "Write logs to file."));
    options.push_back(ConfigurationOption('t', "log-file-enabled-traces-only",
                                          &sLogFileEnabledTracesOnly, "Only write enabled trace areas to log file"));
    options.push_back(ConfigurationOption('T', "trace-areas", ConfigurationOption::STRING_ARGUMENT,
                                          &sTraceAreas, "Turn on logging trace areas."));
    options.push_back(ConfigurationOption(0, "trace-areas-only", &sTraceAreasOnly, "Only log for the given trace areas, nothing more."));
    options.push_back(ConfigurationOption(0, "dnsservers", ConfigurationOption::STRING_ARGUMENT,
                                          &sDnsServers, "DNS server list (semicolon separated)."));
    options.push_back(ConfigurationOption(0, "dnsfallbackservers", ConfigurationOption::STRING_ARGUMENT,
                                          &sDnsFallbackServers, "DNS fallback server list (semicolon separated)."));
    options.push_back(ConfigurationOption(0, "dnscache", &sDnsCacheEnabled, "DNS cache enabled."));
    options.push_back(ConfigurationOption(0, "telnet", &sTelnetEnabled, "telnet server console enabled."));
    options.push_back(ConfigurationOption(0, "telnet-port", ConfigurationOption::INT_ARGUMENT,
                                          &sTelnetPort, "Set the Telnet port to <value>."));
#ifdef NRDP_MUTEX_TRACK_LOCK
    options.push_back(ConfigurationOption(0, "mutex-lock-track-count", ConfigurationOption::INT_ARGUMENT,
                                          &sMutexTrackCount, "Set the mutex track lock history size. 0 means disabled."));
    options.push_back(ConfigurationOption(0, "mutex-lock-track-backtrace", ConfigurationOption::INT_ARGUMENT,
                                          &sMutexTrackBacktrace, "Set the mutex track lock backtrace depth. 0 means disabled."));
#endif
#ifdef NRDP_MUTEX_DETECT_DEADLOCK
    options.push_back(ConfigurationOption('M', "mutex-lock-time-tracking-interval", ConfigurationOption::TIME_ARGUMENT,
                                          &sMutexLockTimeTrackingInterval, "Set the mutex lock time tracking interval to <value>ms. 0 means disabled."));
#endif
#ifdef NRDP_MUTEX_STACK
    options.push_back(ConfigurationOption(0, "mutex-stack", &sMutexStackEnabled, "Mutex stack enabled."));
#endif
#ifdef NRDP_THREAD_TRACK_LOCK
    options.push_back(ConfigurationOption(0, "mutex-thread-track-lock", &sMutexThreadTrackLock, "Record mutex currently locked in thread config."));
#endif
    options.push_back(ConfigurationOption(0, "binary-hash", ConfigurationOption::STRING_ARGUMENT,
                                          &sBinaryHash, "Binary hash of the executable."));

    options.push_back(ConfigurationOption(0, "terminal-colors", ConfigurationOption::STRING_ARGUMENT,
                                          &sTerminalColors, "Color specification for terminal"));
    options.push_back(ConfigurationOption(0, "telnet-colors", ConfigurationOption::STRING_ARGUMENT,
                                          &sTelnetColors, "Color specification for telnet"));

    options.push_back(ConfigurationOption(0, "event-loop-max-send-time", ConfigurationOption::TIME_ARGUMENT,
                                          &sEventLoopMaxSendTime, "How long in milliseconds to process posted events"));
    options.push_back(ConfigurationOption(0, "event-loop-max-timer-time", ConfigurationOption::TIME_ARGUMENT,
                                          &sEventLoopMaxTimerTime, "How long in milliseconds to process timers"));
    options.push_back(ConfigurationOption(0, "event-loop-max-pending-time", ConfigurationOption::TIME_ARGUMENT,
                                          &sEventLoopMaxPendingTime, "How long in milliseconds to allow a posted event to go unprocessed"));

    options.push_back(ConfigurationOption(0, "object-count-timer-interval", ConfigurationOption::TIME_ARGUMENT,
                                          &sObjectCountTimerInterval, "How often in milliseconds to run object count"));

    options.push_back(ConfigurationOption(0, "disk-cache-catalog-timer", ConfigurationOption::TIME_ARGUMENT,
                                          &sDiskCacheCatalogTimer, "Disk cache catalog timer interval."));
    options.push_back(ConfigurationOption(0, "disk-cache-write-suspend-interval", ConfigurationOption::TIME_ARGUMENT,
                                          &sDiskCacheWriteSuspendInterval, "Disk cache suspend interval."));
    options.push_back(ConfigurationOption(0, "disk-cache-capacity", ConfigurationOption::SIZE_ARGUMENT,
                                          &sDiskCacheCapacity, "Disk cache capacity."));
    options.push_back(ConfigurationOption(0, "disk-cache-max-pending", ConfigurationOption::SIZE_ARGUMENT,
                                          &sDiskCacheMaxPending, "Disk cache pending capacity."));
    options.push_back(ConfigurationOption(0, "disk-cache-write-limit", ConfigurationOption::SIZE_ARGUMENT,
                                          &sDiskCacheWriteLimit, "Disk cache max bytes written per day."));
    options.push_back(ConfigurationOption(0, "disk-cache-path", ConfigurationOption::DATAWRITEPATH_ARGUMENT,
                                          &sDiskCachePath, "Disk cache path."));
    options.push_back(ConfigurationOption(0, "resource-cache-capacity", ConfigurationOption::SIZE_ARGUMENT,
                                          &sResourceManagerCapacity, "Resource cache capacity"));
    options.push_back(ConfigurationOption(0, "resource-manager-gzip-compression", &sResourceManagerGzipCompressionEnabled,
                                          "Whether gzip compression is enabled for ResourceManager"));

    options.push_back("Crash Reporting Options");
    options.push_back(ConfigurationOption(0, "shutdown-report",
                                          &sCrashReportShutdowns, "Report abnormal shutdowns."));
    options.push_back(ConfigurationOption(0, "crash-report",
                                          &sCrashReportEnabled, "Crash reporting enabled."));
    options.push_back(ConfigurationOption(0, "crash-report-path", ConfigurationOption::DATAWRITEPATH_ARGUMENT,
                                          &sCrashLogDirectory, "Path to save crash reports to."));

#ifdef NRDP_HAS_SOCKETIZER
    options.push_back("Socketizer Options");
    options.push_back(ConfigurationOption(0, "socketizer-condition-file", ConfigurationOption::STRING_ARGUMENT,
                                          &sSocketizerConditionFile, "Read Socketizer conditions from this file"));
#endif

    for(ConfigurationOptions::const_iterator it = sDefaultOptions.begin(); it != sDefaultOptions.end(); ++it)
        options.push_back(*it);
    return options;
}

void Configuration::addDefaultOption(const ConfigurationOption &option)
{
    sDefaultOptions.push_back(option);
}

bool Configuration::setOverrideContents(const std::string &xml)
{
    const std::string fn = sDataWriteDirectory + "/override.xml";
    if (xml.empty()) {
        unlink(fn.c_str());
        return true;
    }

    FILE *f = fopen(fn.c_str(), "w");
    if (!f) {
        Log::error(TRACE_SYSTEM, "Couldn't open %s for writing (%d)", fn.c_str(), errno);
        return false;
    }

    bool ret = true;
    HMAC hmac(sOverrideSecret, sizeof(sOverrideSecret));
    const DataBuffer signature = hmac.hmac<DataBuffer>(xml);
    const int signatureSize = signature.size();
    if (!fwrite(&signatureSize, sizeof(signatureSize), 1, f)
        || !fwrite(signature.data(), signature.size(), 1, f)
        || !fwrite(xml.c_str(), xml.size(), 1, f)) {
        Log::error(TRACE_SYSTEM, "Couldn't write to %s (%d)", fn.c_str(), errno);
        ret = false;
    }

    fclose(f);
    return ret;
}


void Configuration::loadOverrideFile()
{
    std::string overrideFile;
    if (pathAbsolute(sDataWriteDirectory)) {
        overrideFile = sDataWriteDirectory + "/override.xml";
    } else {
        overrideFile = sDataReadDirectory + "/" + sDataWriteDirectory + "/override.xml";
    }
    if (!fileOrResourceExists(overrideFile))
        return;
    const DataBuffer overrideContents = DataBuffer::fromFile(overrideFile);
    unlink(overrideFile.c_str());
    if (overrideContents.size() <= static_cast<int>(sizeof(int)))
        return;

    int signatureSize;
    memcpy(&signatureSize, overrideContents.data<const unsigned char>(), sizeof(int));
    if (signatureSize > EVP_MAX_MD_SIZE || overrideContents.size() <= static_cast<int>(signatureSize + sizeof(int)))
        return;

    ConfigData config;
    std::string err;
    if (!applyXML(overrideContents.slice(signatureSize + sizeof(int)), overrideFile, config, sFinalOptions, err)) {
        Log::printf("Unable to parse configuration override file: %s (%s)\n", overrideFile.c_str(), err.c_str());
        return;
    }
    const Variant configDataValues = config.values();
    if (configDataValues.isStringMap()) {
        sConfigDataValues.merge(configDataValues);
    }
}

const char *Configuration::colorToEscapeCode(Color color)
{
    const char *colors[] = {
        0, // invalid
        "\x1b[0m", // Default
        "\x1b[30m", // Black
        "\x1b[31m", // Red
        "\x1b[32m", // Green
        "\x1b[33m", // Yellow
        "\x1b[34m", // Blue
        "\x1b[35m", // Magenta
        "\x1b[36m", // Cyan
        "\x1b[37m", // White
        "\x1b[0;1m", // BrightDefault
        "\x1b[30;1m", // BrightBlack
        "\x1b[31;1m", // BrightRed
        "\x1b[32;1m", // BrightGreen
        "\x1b[33;1m", // BrightYellow
        "\x1b[34;1m", // BrightBlue
        "\x1b[35;1m", // BrightMagenta
        "\x1b[36;1m", // BrightCyan
        "\x1b[37;1m" // BrightWhite
    };
    return colors[color];
}

Configuration::Color Configuration::toColor(const std::string &name)
{
    if(name.empty() || !strcasecmp(name.c_str(), "default")) {
        return Default;
    } else if(!strcasecmp(name.c_str(), "brightdefault")) {
        return BrightDefault;
    } else if(!strcasecmp(name.c_str(), "black")) {
        return Black;
    } else if(!strcasecmp(name.c_str(), "red")) {
        return Red;
    } else if(!strcasecmp(name.c_str(), "green")) {
        return Green;
    } else if(!strcasecmp(name.c_str(), "yellow")) {
        return Yellow;
    } else if(!strcasecmp(name.c_str(), "blue")) {
        return Blue;
    } else if(!strcasecmp(name.c_str(), "magenta")) {
        return Magenta;
    } else if(!strcasecmp(name.c_str(), "cyan")) {
        return Cyan;
    } else if(!strcasecmp(name.c_str(), "white")) {
        return White;
    } else if(!strcasecmp(name.c_str(), "brightblack")) {
        return BrightBlack;
    } else if(!strcasecmp(name.c_str(), "brightred")) {
        return BrightRed;
    } else if(!strcasecmp(name.c_str(), "brightgreen")) {
        return BrightGreen;
    } else if(!strcasecmp(name.c_str(), "brightyellow")) {
        return BrightYellow;
    } else if(!strcasecmp(name.c_str(), "brightblue")) {
        return BrightBlue;
    } else if(!strcasecmp(name.c_str(), "brightmagenta")) {
        return BrightMagenta;
    } else if(!strcasecmp(name.c_str(), "brightcyan")) {
        return BrightCyan;
    } else if(!strcasecmp(name.c_str(), "brightwhite")) {
        return BrightWhite;
    }
    return Invalid;
}

std::string Configuration::ColorScheme::describe() const
{
    std::ostringstream str;
    const char *names[] = {
        0,
        "default",
        "black",
        "red",
        "green",
        "yellow",
        "blue",
        "magenta",
        "cyan",
        "white",
        "brightdefault",
        "brightblack",
        "brightred",
        "brightgreen",
        "brightyellow",
        "brightblue",
        "brightmagenta",
        "brightcyan",
        "brightwhite"
    };
    struct {
        const char *name;
        const Color *col;
    } const colors[] = {
        { "logHeader", &logHeader },
        { "trace", &trace },
        { "info", &info },
        { "warning", &warning },
        { "error", &error },
        { "fatal", &fatal },
        { "grep", &grep },
        { 0, 0 }
    };
    bool first = true;
    for(int i=0; colors[i].name; ++i) {
        if(*colors[i].col) {
            if(!first) {
                str << ',';
            } else {
                first = false;
            }
            str << colors[i].name << "=" << names[*colors[i].col];
        }
    }
    return str.str();
}

bool Configuration::ColorScheme::parse(const std::string &spec)
{
    reset(Default);
    std::vector<std::string> split;
    StringTokenizer::tokenize(spec, split, ",");
    for(std::vector<std::string>::iterator it = split.begin(); it != split.end(); ++it) {
        removeAll(*it, " ");
        const size_t eq = it->find('=');
        if(eq == std::string::npos) {
            reset(Invalid);
            return false;
        }

        const char *cstr = it->c_str();
        Color *col = 0;
        struct {
            const char *name;
            Color *color;
        } const colors[] = {
            { "logHeader", &logHeader },
            { "trace", &trace },
            { "info", &info },
            { "warning", &warning },
            { "error", &error },
            { "fatal", &fatal },
            { "grep", &grep },
            { 0, 0 }
        };
        for(int i=0; colors[i].name; ++i) {
            if(!strncasecmp(cstr, colors[i].name, eq)) {
                col = colors[i].color;
                break;
            }
        }
        if(!col) {
            reset(Invalid);
            return false;
        }

        *col = toColor(it->substr(eq + 1));
        if(!*col) {
            reset(Invalid);
            return false;
        }
    }

    return true;
}

llong Configuration::parseInt(const char *str, unsigned int flags, bool *okptr)
{
    char *endptr;
    llong arg = strtoll(str, &endptr, 10);
    bool dummy;
    bool &ok = okptr ? *okptr : dummy;
    ok = true;
    if(*endptr) {
        ok = false;
        if((flags & ConfigurationOption::SIZE_ARGUMENT) == ConfigurationOption::SIZE_ARGUMENT) {
            if(!strcasecmp(endptr, "mb") || !strcasecmp(endptr, "m")) {
                arg *= 1024*1024;
                ok = true;
            } else if(!strcasecmp(endptr, "gb") || !strcasecmp(endptr, "g")) {
                arg *= 1024*1024*1024;
                ok = true;
            } else if(!strcasecmp(endptr, "kb") || !strcasecmp(endptr, "k")) {
                arg *= 1024;
                ok = true;
            } else if(!strcasecmp(endptr, "b")) {
                ok = true;
            }
        } else if((flags & ConfigurationOption::TIME_ARGUMENT) == ConfigurationOption::TIME_ARGUMENT) {
            if(!strcasecmp(endptr, "w") || !strcasecmp(endptr, "weeks") || !strcasecmp(endptr, "week")) {
                arg *= 7*24*60*60*1000;
                ok = true;
            } else if(!strcasecmp(endptr, "d") || !strcasecmp(endptr, "day") || !strcasecmp(endptr, "days")) {
                arg *= 24*60*60*1000;
                ok = true;
            } else if(!strcasecmp(endptr, "h") || !strcasecmp(endptr, "hour") || !strcasecmp(endptr, "hours")) {
                arg *= 60*60*1000;
                ok = true;
            } else if(!strcasecmp(endptr, "m") || !strcasecmp(endptr, "min") || !strcasecmp(endptr, "minute") || !strcasecmp(endptr, "minutes")) {
                arg *= 60*1000;
                ok = true;
            } else if(!strcasecmp(endptr, "s") || !strcasecmp(endptr, "sec") || !strcasecmp(endptr, "second") || !strcasecmp(endptr, "seconds")) {
                arg *= 1000;
                ok = true;
            } else if(!strcasecmp(endptr, "m") || !strcasecmp(endptr, "ms") || !strcasecmp(endptr, "millisecond") || !strcasecmp(endptr, "milliseconds")) {
                ok = true;
            }
        }
    }
    return arg;
}
inline static char xmlTransform(char c)
{
    if((c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9'))
        return c;
    else if(c >= 'a' && c <= 'z')
        return c;
    return '_';
}

std::string Configuration::xmlOption(const std::string &longOption)
{
    std::string xml = "<";
    std::transform(longOption.begin(), longOption.end(), std::back_inserter(xml), xmlTransform);
    xml += '>';
    return xml;
}

inline static char envTransform(char c)
{
    if((c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9'))
        return c;
    else if(c >= 'a' && c <= 'z')
        return std::toupper(static_cast<unsigned char>(c));
    return '_';
}

std::string Configuration::environmentVariableOption(const std::string &longOption)
{
    std::string env = "NF_";
    std::transform(longOption.begin(), longOption.end(), std::back_inserter(env), envTransform);
    return env;
}

std::string Configuration::jsOption(const std::string &longOption)
{
    std::string xml;
    std::transform(longOption.begin(), longOption.end(), std::back_inserter(xml), xmlTransform);
    return xml;
}

ConfigurationOption::Source Configuration::source(const std::string &longOption)
{
    for(ConfigurationOptions::const_iterator it = sFinalOptions.begin(); it != sFinalOptions.end(); ++it) {
        if(it->longOption && it->longOption == longOption)
            return it->source;
    }
    fprintf(stderr, "Invalid option %s\n", longOption.c_str());
    assert(0 && "Invalid option");
    return ConfigurationOption::Source_Default;
}

bool Configuration::setSystemTime(const std::string &argument)
{
    if(argument[0] == '-') {
        bool ok;
        const llong offset = Variant(argument).value<llong>(&ok);
        if(!ok) {
            Log::error(TRACE_SYSTEM, "Invalid args to /time");
            return false;
        }
        Time::setSystemTimeOffset(offset);
    } else if(argument[0] == '+') {
        bool ok;
        const llong offset = Variant(argument.substr(1)).value<llong>(&ok);
        if(!ok) {
            Log::error(TRACE_SYSTEM, "Invalid args to /time");
            return false;
        }
        Time::setSystemTimeOffset(offset);
    } else {
        ullong val = Time::fromRFC1123(argument).val();
        if(!val) {
            bool ok;
            val = Variant(argument).value<ullong>(&ok);
            if(!ok) {
                Log::error(TRACE_SYSTEM, "Invalid args to /time");
                return false;
            }
        }
        Time::setSystemTime(val);
    }
    return true;
}
