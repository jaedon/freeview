/*
 * (c) 1997-2015 Netflix, Inc. All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "Console.h"
#include "Application.h"
#include <nrdbase/Log.h>
#include <nrdbase/Grep.h>
#include <nrdbase/Thread.h>
#include <nrdbase/LogSink.h>
#include <nrdbase/DumpInfo.h>
#include <nrdbase/ConsoleSink.h>
#include <nrdbase/MutexRanksIterator.h>

#include <unistd.h>

using namespace netflix;

class WordCount : public Console::Filter
{
public:
    inline WordCount() : mChars(0), mLines(0) { }
    ~WordCount() {
        netflix::Log::error(TRACE_SYSTEM, "WC: Lines: %d, Chars: %d", mLines, mChars);
    }

    bool init(const std::vector<std::string> &) { return true; }

    virtual bool filter(std::string &text, const Configuration::ColorScheme &) const
    {
        ++mLines;
        mChars += text.size();
        return false;
    }

    virtual std::string describe() const { return "wc"; }
private:
    mutable int mChars, mLines;
};

class LogFileCommand : public Console::Command
{
public:
    LogFileCommand()
        : Console::Command("logfile", "Dump current logfile", "clear")
    {}
    virtual void invoke(const Arguments &arguments) const
    {
        if (arguments.size() >= 2) {
            if (arguments[1] == "clear") {
                if (!app()->openLogFile(app()->logfile())) {
                    Log::error(TRACE_LOG, "Can't open %s for writing", app()->logfile().c_str());
                }
            } else if (!app()->openLogFile(arguments[1])) {
                Log::error(TRACE_LOG, "Can't open %s for writing", arguments[1].c_str());
            }
        }
        const std::string logfile = app()->logfile();
        if (logfile.empty()) {
            Log::warn(TRACE_SYSTEM, "No log file");
        } else {
            Log::warn(TRACE_SYSTEM, "Log file: %s", logfile.c_str());
        }
    }
    virtual std::vector<Help> help() const
    {
        std::vector<Help> result;
        result.push_back(Help("clear", "Clear current log file"));
        result.push_back(Help("[filename]", "Reopen [filename] as log file."));
        return result;
    }
};

class SeparatorCommand : public Console::Command
{
public:
    SeparatorCommand()
        : Console::Command("/", "Draws a handy line separator, useful for debugging")
    {}
    virtual void invoke(const Arguments &arguments) const
    {
        std::string text;
        if(arguments.size() > 1) {
            for(unsigned i = 1; i < arguments.size(); ++i) {
                text.append(arguments[i]).append(" ");
            }
        } else {
            text = " ";
        }
        Log::warn(TRACE_SYSTEM, "\n\n\n\n\n\n\n%s\n", text.c_str());
    }
    virtual std::vector<Help> help() const
    {
        std::vector<Help> result;
        result.push_back(Help("<string>", "Display <string>."));
        return result;
    }
};

DECLARE_THREAD(SINGLE_CORE);
DEFINE_THREAD(SINGLE_CORE, 0, 0);
class SingleCoreCommand : public Console::Command
{
    class CPUThief : public Thread
    {
    public:
        CPUThief() : Thread(&THREAD_SINGLE_CORE) { }
        void Run() {
            while(true);
        }
    };


public:
    SingleCoreCommand()
        : Console::Command("singlecore", "Switch all threads to run on CPU 0")
    {}

    virtual void invoke(const Arguments &arguments) const
    {
        Thread::SetSingleCore(true);
        if(arguments.size() > 1) {
            for(int i = 0; i < atoi(arguments[1].c_str()); ++i) {
                CPUThief *thief = new CPUThief();
                thief->Start();
            }
        }
    }
    virtual std::vector<Help> help() const
    {
        std::vector<Help> result;
        result.push_back(Help("<number>", "Spin up <number> threads to steal CPU time."));
        return result;
    }
};

#if defined(NRDP_PLATFORM_LINUX)
#include <sys/resource.h>
class UlimitCommand : public Console::Command
{
public:
    UlimitCommand()
        : Console::Command("ulimit", "Print ulimit information")
    {}

    virtual void invoke(const Arguments &) const
    {
#define PRINT_ULIMIT(limit, desc) do { struct rlimit r; if(!getrlimit(RLIMIT_ ## limit, &r)) \
                                                     Log::warn(TRACE_SYSTEM, "%s(%s): %s [%s]", desc, #limit,  \
                                                               prettyBytes(r.rlim_cur).c_str(), prettyBytes(r.rlim_max).c_str()); } while(0)
        PRINT_ULIMIT(AS, "Address Space");
        PRINT_ULIMIT(CORE, "Core Size");
        PRINT_ULIMIT(CPU, "CPU Time");
        PRINT_ULIMIT(DATA, "Data Segment");
        PRINT_ULIMIT(FSIZE, "File Size");
        PRINT_ULIMIT(MEMLOCK, "Memory");
#if !defined(__APPLE__)
        PRINT_ULIMIT(NICE, "Nice Level");
#endif
        PRINT_ULIMIT(NPROC, "Processes");
        PRINT_ULIMIT(STACK, "Stack Size");
#undef PRINT_ULIMIT
    }
};
#endif

class CatCommand : public Console::Command
{
public:
    CatCommand()
        : Console::Command("cat", "Display file")
    {}

    virtual void invoke(const Arguments &arguments) const
    {
        for(size_t i = 1; i < arguments.size(); ++i) {
            if(FILE *f = fopen(arguments[i].c_str(), "r")) {
                char buffer[2048];
                Log::warn(TRACE_SYSTEM, "Reading file: %s\n", arguments[i].c_str());
                for(int line = 1; fgets(buffer, sizeof(buffer), f); ++line)
                    Log::warn(TRACE_SYSTEM, "%d: %s\n", line, buffer);
                fclose(f);
            } else {
                Log::warn(TRACE_SYSTEM, "Error opening: %s\n", arguments[i].c_str());
            }
        }
    }
    virtual std::vector<Help> help() const
    {
        std::vector<Help> result;
        result.push_back(Help("<file>", "Display <file> to console."));
        return result;
    }
};

class TimeCommand : public Console::Command
{
public:
    TimeCommand()
        : Console::Command("time", "Set time multiplier or system time.", "system")
    {}
    virtual void invoke(const Arguments &arguments) const
    {
        if (arguments.size() == 2) {
            TimeMultiplier::setMultiplier(atof(arguments[1].c_str()));
        } else if (arguments.size() == 3 && arguments[1] == "system" && !Configuration::setSystemTime(arguments[2])) {
            Log::error(TRACE_SYSTEM, "Unable to set system time: %s", arguments[2].c_str());
        }
        Log::warn(TRACE_SYSTEM, "Current server time: %s (%lld)",
                  Time::hasServerTime() ? Time::serverTime().calendarTimeText().c_str() : "no server time",
                  Time::hasServerTime() ? Time::serverTime().val() : 0);
        Log::warn(TRACE_SYSTEM, "Current system time %s %llu (%lld)",
                  Time::systemTime().rfc1123().c_str(),
                  Time::systemTime().val(),
                  Time::systemTimeOffset());
        Log::warn(TRACE_SYSTEM, "Current time multiplier: %f", TimeMultiplier::multiplier());
    }
    virtual std::vector<Help> help() const
    {
        std::vector<Help> result;
        result.push_back(Help("<float>", "Set a time multiplier for all mono time requests."));
        result.push_back(Help("system", "Set system time (leading + or - means set offset)."));
        return result;
    }
};

class TimersCommand : public Console::Command
{
public:
    TimersCommand()
        : Console::Command("timers", "Dump outstanding timers")
    {}

    virtual void invoke(const Arguments &) const
    {
        app()->eventLoop()->dumpTimers();
    }
};

class MilestonesCommand : public Console::Command
{
public:
    MilestonesCommand()
        : Console::Command("milestones", "Dump all milestones")
    {}
    virtual void invoke(const Arguments &) const
    {
        if(ConsoleSink::SharedPtr sink = app()->consoleSink()) {
            const std::list<Log::Message> milestones = sink->milestones();
            for(std::list<Log::Message>::const_iterator it = milestones.begin(); it != milestones.end(); ++it)
                Log::error(TRACE_SYSTEM, "Milestone: %s", ConsoleSink::format((*it)).c_str());
        }
    }
};

class ThreadsCommand : public Console::Command
{
public:
    ThreadsCommand()
        : Console::Command("threads", "Display information about threads", "all locks")
    {}
    virtual void invoke(const Arguments &arguments) const
    {
        uint8_t flags = 0;
        for(size_t i = 1; i < arguments.size(); ++i) {
            if(arguments[i] == "all")
                flags |= ThreadConfig::ShowAll;
            else if(arguments[i] == "locks")
                flags |= ThreadConfig::ShowLocks;
        }
        ThreadConfig::dump(flags);
    }
    virtual std::vector<Help> help() const
    {
        std::vector<Help> result;
        result.push_back(Help("all", "Display all thread configs even if there are no active threads."));
        result.push_back(Help("locks", "Display lock contention stats."));
        return result;
    }
};

class GrepCommand : public Console::Command
{
public:
    GrepCommand(const std::string &name)
        : Console::Command(name, "Sets a grep command for the console", "-i -v --invert-match --ignore-case --color --no-color --highlight")
    {}
    virtual void invoke(const Arguments &arguments) const
    {
        if(arguments.size() == 1 || (arguments.size() == 2 && arguments[1].empty())) {
            app()->clearFilters();
        } else {
            std::vector<std::string> grepArguments = arguments.arguments;
            if(shared_ptr<Console::Filter> filter = app()->console()->createFilter(grepArguments.begin(), grepArguments.end())) {
                Console::Filters filters;
                filters.push_back(filter);
                app()->setFilters(filters);
            } else {
                Log::error(TRACE_SYSTEM, "Invalid /grep");
            }
        }
    }
};


static inline Variant objectsCompletions()
{
    Variant ret;
    ret["timer"] = Variant();
    const std::vector<std::string> types = ObjectCount::types();
    ret["all"] = StringTokenizer::join(types, " ");
    for (std::vector<std::string>::const_iterator it = types.begin(); it != types.end(); ++it)
        ret[*it] = "all";

    return ret;
}
class ObjectsCommand : public Console::Command
{
public:
    ObjectsCommand()
        : Console::Command("objects", "Dump object count or start/stop a timer that does it at intervals", objectsCompletions())
    {}
    virtual void invoke(const Arguments &arguments) const
    {
        if(arguments.size() == 3 && arguments[1] == "timer") {
            char *end = 0;
            const int interval = strtol(arguments[2].c_str(), &end, 10);
            if(!*end)
                app()->setObjectCountTimerInterval(interval);
        } else {
            unsigned int flags = ObjectCount::ShowDiff;
            std::string filter;
            for(size_t i = 1; i < arguments.size(); ++i) {
                if(arguments[i] == "objects" || arguments[i] == "all") {
                    flags |= ObjectCount::ShowObjects;
                } else {
                    filter = arguments[i];
                }
            }
            ObjectCount::dump(flags, filter);
        }
    }
    virtual std::vector<Help> help() const
    {
        std::vector<Help> result;
        result.push_back(Help("timer", "<ms>", "Start a timer to dump object information every <ms>."));
        result.push_back(Help("objects", "Dump all object pointers."));

        return result;
    }
};

class ColorCommand : public Console::Command
{
public:
    ColorCommand()
        : Console::Command("color", "Disabled and enable color for console", "on off telnet terminal")
    {}
    virtual void invoke(const Arguments &arguments) const
    {
        bool error = false;
        if(arguments.size() == 1) {
            Log::warn(TRACE_SYSTEM, "Color is %s", Configuration::consoleSinkLogColor() ? "enabled" : "disabled");
        } else if(arguments.size() == 2) {
            if(arguments[1] == "on") {
                Configuration::setConsoleSinkLogColor(true);
            } else if(arguments[1] == "off") {
                Configuration::setConsoleSinkLogColor(false);
            } else if(arguments[1] == "telnet") {
                Log::warn(TRACE_SYSTEM, "telnet colors: %s", Configuration::telnetColors().describe().c_str());
           } else if(arguments[1] == "terminal") {
                Log::warn(TRACE_SYSTEM, "telnet colors: %s", Configuration::terminalColors().describe().c_str());
            } else {
                error = true;
            }
        } else if(arguments.size() == 3) {
            if(arguments[1] == "telnet") {
                Configuration::ColorScheme scheme;
                if(!scheme.parse(arguments[2])) {
                    Log::warn(TRACE_SYSTEM, "Invalid arguments: /color on|off|terminal|telnet");
                } else {
                    Configuration::setTelnetColors(scheme);
                }

            } else if(arguments[1] == "terminal") {
                Configuration::ColorScheme scheme;
                if(!scheme.parse(arguments[2])) {
                    Log::warn(TRACE_SYSTEM, "Invalid arguments: /color on|off|terminal|telnet");
                } else {
                    Configuration::setTerminalColors(scheme);
                }
            } else {
                error = true;
            }
        } else {
            error = true;
        }
        if(error)
            Log::warn(TRACE_SYSTEM, "Invalid arguments: /color on|off|terminal|telnet");
    }
    virtual std::vector<Help> help() const
    {
        std::vector<Help> result;
        result.push_back(Help("on", "Enable color for console."));
        result.push_back(Help("off", "Disable color for console."));
        result.push_back(Help("terminal", "[scheme]", "Set terminal colors to <scheme>."));
        result.push_back(Help("telnet", "[scheme]", "Set telnet colors to <scheme>."));
        return result;
    }
};

class EnvCommand : public Console::Command
{
public:
    EnvCommand()
        : Console::Command("env", "Show environment variables")
    {}
    virtual void invoke(const Arguments &arguments) const
    {
#ifdef __APPLE__
        extern char **environ;
#endif

        if(arguments.size() == 3) {
            setenv(arguments[1].c_str(), arguments[2].c_str(), 1);
        } else if(arguments.size() == 2) {
            Log::warn(TRACE_SYSTEM, "Environment: %s: %s", arguments[1].c_str(), getenv(arguments[1].c_str()));
        } else {
            for(int i = 0; environ[i]; ++i)
                Log::warn(TRACE_SYSTEM, "Environment[%d]: %s", i, environ[i]);
        }
    }
    virtual std::vector<Help> help() const
    {
        std::vector<Help> result;
        result.push_back(Help("[key]", "[value]", "Set [key] to [value] in the environment for this process."));
        return result;
    }
};


class LocksCommand : public Console::Command
{
public:
    LocksCommand()
        : Console::Command("locks", "Show mutex information", "reset backtrace ranks")
    {}
    virtual void invoke(const Arguments &arguments) const
    {
        uint8_t flags = 0;
        DumpInfo info;
        bool ranks = false;
        for(size_t i = 1; i < arguments.size(); ++i) {
            if(arguments[i] == "reset") {
                flags |= Mutex::Dump_Reset;
            }
            if(arguments[i] == "ranks") {
                ranks = true;
            } else if(arguments[i] == "stack" || arguments[i] == "bt" || arguments[i] == "backtrace") {
                flags |= Mutex::Dump_Backtrace;
            } else {
                info.set(arguments[i]);
            }
        }
        if(ranks) {
            const std::vector<Rank> ranks = mutexRanks();
            for(std::vector<Rank>::const_iterator it = ranks.begin(); it != ranks.end(); ++it) {
                if(it->value == MutexRankValue(UNTRACKED_MUTEX)) {
                    Log::warn(TRACE_SYSTEM, "%s(%d): UNTRACKED", it->name.c_str(), it->rank);
                } else {
                    Log::warn(TRACE_SYSTEM, "%s(%d): %d", it->name.c_str(), it->rank, it->value);
                }
            }
            if(!flags && info.empty())
                return;
        }
        Mutex::dump(info, flags);
    }
    virtual std::vector<Help> help() const
    {
        std::vector<Help> result;
        result.push_back(Help("reset", "Reset lock information."));
        result.push_back(Help("ranks", "Display all known mutex ranks."));
        result.push_back(Help("stack", "Display stack information for outstanding locks."));
        return result;
    }
};

class CommandLineCommand : public Console::Command
{
public:
    CommandLineCommand()
        : Console::Command("commandline", "Display commandline arguments", Variant())
    {}

    virtual void invoke(const Arguments &arguments) const
    {
        bool dump = false, set = false;
        std::string key, value;
        for(size_t i = 1; i < arguments.size(); ++i) {
            if(arguments[i] == "dump") {
                dump = true;
            } else if(arguments[i] == "override") {
                const size_t idx = arguments.command.find(" override ");
                assert(idx != std::string::npos);
                Configuration::setOverrideContents(arguments.command.substr(idx + 10));
                return;
            } else if(key.empty()) {
                key = arguments[i];
            } else {
                set = true;
                value = arguments[i];
            }
        }
        if(!key.empty() && set) {
            char *argv[] = { 0, strdup(std::string("--" + key).c_str()), strdup(value.c_str()), 0 };
            app()->initConfiguration(3, argv);
            free(argv[1]);
            free(argv[2]);
        }
        if(!dump || !key.empty()) {
            ConfigurationOptions options = app()->defaultConfigurationOptions();
            for(ConfigurationOptions::const_iterator it = options.begin(); it != options.end(); ++it) {
                const ConfigurationOption &option = (*it);
                if(option.description && (key.empty() || (option.longOption && key == option.longOption) || (option.shortOption && key == &option.shortOption)))
                    Log::warn(TRACE_SYSTEM, "%s", option.dump().c_str());
            }
        } else {
            std::string response;
            const std::vector<std::string> commandline = Configuration::arguments(app()->defaultConfigurationOptions());
            for(size_t i = 0; i < commandline.size(); ++i) {
                if(!response.empty())
                    response += " ";
                response += commandline[i];
            }
            Log::warn(TRACE_SYSTEM, "Commandline: %s", response.c_str());
        }
    }
    virtual std::vector<Help> help() const
    {
        std::vector<Help> result;
        Help commandline;
        commandline << Help("override", "<xml>", "Set override xml for next run for the configuration system.");
        commandline << Help("<key>", "[value]", "Set <key> to [value] in configuration system.");
        ConfigurationOptions options = app()->defaultConfigurationOptions();
        for(ConfigurationOptions::const_iterator it = options.begin(); it != options.end(); ++it) {
            if(it->longOption && it->description)
                commandline << Help(it->longOption, it->description);
        }
        result.push_back(commandline);
        return result;
    }
};

Console::Console()
    : mMutex(ZERO_MUTEX, "Console::Mutex")
{
    addCommand(shared_ptr<Command>(new SeparatorCommand));
    addCommand(shared_ptr<Command>(new LogFileCommand));
    addCommand(shared_ptr<Command>(new CatCommand));
#if defined(NRDP_PLATFORM_LINUX)
    addCommand(shared_ptr<Command>(new UlimitCommand));
#endif
    addCommand(shared_ptr<Command>(new TimeCommand));
    addCommand(shared_ptr<Command>(new MilestonesCommand));
    addCommand(shared_ptr<Command>(new SingleCoreCommand));
    addCommand(shared_ptr<Command>(new ThreadsCommand));
    addCommand(shared_ptr<Command>(new TimersCommand));
    addCommand(shared_ptr<Command>(new GrepCommand("grep")));
    addCommand(shared_ptr<Command>(new GrepCommand("hilite")));
    addCommand(shared_ptr<Command>(new ObjectsCommand));
    addCommand(shared_ptr<Command>(new ColorCommand));
    addCommand(shared_ptr<Command>(new EnvCommand));
    addCommand(shared_ptr<Command>(new LocksCommand));
    addCommand(shared_ptr<Command>(new CommandLineCommand));
}

static inline std::string mkspaces(int s)
{
    std::string result;
    while(s--)
        result += " ";
    return result;
}

void Console::stop()
{
    std::vector<shared_ptr<Command> > commands;
    {
        ScopedMutex lock(mMutex);
        std::swap(mCommandList, commands);
    }
}

bool Console::handleCommand(const std::string &command, unsigned int flags)
{
    const char *cstr = command.c_str();
    bool spaces = false;
    while(isspace(*cstr)) {
        spaces = true;
        ++cstr;
    }
    if(spaces)
        return handleCommand(cstr, flags);

    if(command.empty())
        return true;

    std::string cmd;
    Console::Filters filters;
    if(!startsWith("/grep", command)) {
        const std::vector<std::string> arguments = shellTokenize(command[0] == '/' ? command.substr(1) : command);
        const std::vector<std::string>::const_iterator it = std::find(arguments.begin(), arguments.end(), "|");
        if(it != arguments.end()) {
            bool ok;
            filters = Console::parseFilters(it, arguments.end(), ok);
            if(!ok)
                return false;
            cmd = command.substr(0, command.find('|') - 1);
        }
    }
    if(cmd.empty())
        cmd = command;

    std::string warning;
    std::list<shared_ptr<Command> > commands;
    const std::vector<std::string> arguments = shellTokenize(cmd);
    const char *arg0 = arguments[0].c_str();
    if(*arg0 == '/') {
        ++arg0;
        const int argLength = arguments[0].size() - 1;
        ScopedMutex lock(mMutex);
        for(std::vector<shared_ptr<Command> >::const_iterator it = mCommandList.begin(); it != mCommandList.end(); ++it) {
            const char *str = (*it)->name.c_str();
            if(!strcmp(arg0, str)) {
                commands.push_back((*it));
                break;
            } else if(!strncmp(arg0, str, argLength)) {
                commands.push_back((*it));
            }
        }
    }
    if(commands.empty())
        return false;

    if(!(flags & Console_Help) && commands.size() > 1) {
        Log::error(TRACE_SYSTEM, "Ambiguous commandline: %s", command.c_str());
        return false;
    }

    for(std::list<shared_ptr<Command> >::const_iterator command_it = commands.begin(); command_it != commands.end(); ++command_it) {
        shared_ptr<Command> c = (*command_it);
        const Command::Arguments commandArguments(command, arguments, filters);
        if(flags & Console_Help) {
            const std::vector<Command::Help> help = c->help();
            if(help.empty()) {
                Log::warn(TRACE_SYSTEM, "/%s -- %s", c->name.c_str(), c->description.c_str());
            } else {
                if(command_it != commands.begin())
                    Log::warn(TRACE_SYSTEM, " ");
                Log::warn(TRACE_SYSTEM, "%s", c->description.c_str());
                Log::warn(TRACE_SYSTEM, "----------------------------");
                Log::warn(TRACE_SYSTEM, "%s [command]", c->name.c_str());
                Log::warn(TRACE_SYSTEM, "----------------------------");
                Log::warn(TRACE_SYSTEM, "Commands:");
                Log::warn(TRACE_SYSTEM, " ");
                dump(commandArguments, help);
            }
        } else {
            c->invoke(commandArguments);
            if(!(c->flags & Command::Silent))
                app()->executed(command);
        }
    }
    return true;
}

std::vector<std::string> Console::shellTokenize(const std::string &in)
{
    std::string string = in;
    std::vector<std::string> ret;
    int last = -1;
    int idx = 0;
    int size = string.size();
    while(idx < size) {
        if(string.at(idx) == '|') {
            if(last != -1) {
                ret.push_back(string.substr(last, idx - last));
                last = -1;
            }
            ret.push_back("|");
            ++idx;
            continue;
        }

        if(isspace((unsigned char)string.at(idx))) {
            if(last != -1) {
                ret.push_back(string.substr(last, idx - last));
                last = -1;
            }
            ++idx;
            while(idx < size && isspace((unsigned char)string.at(idx)))
                ++idx;
            if(idx == size)
                break;
            continue;
        }

        switch (string.at(idx)) {
        case '\'':
        case '"': {
            if(last == -1)
                last = idx;
            const int start = idx;
            const char quote = string.at(idx);
            int escape = 0;
            while(++idx < size) {
                const char ch = string.at(idx);
                if(ch == '\\') {
                    ++escape;
                } else {
                    if(ch == quote) {
                        if(escape % 2 == 0) {
                            string.erase(idx, 1);
                            string.erase(start, 1);
                            size -= 2;
                            --idx;
                            break;
                        }
                    }
                    escape = 0;
                }
            }
            break; }
        default:
            if(last == -1)
                last = idx;
            ++idx;
            break;
        }
    }
    if(last != -1)
        ret.push_back(string.substr(last));
    return ret;
}

struct pred
{
    bool operator()(const shared_ptr<Console::Command> &a, const shared_ptr<Console::Command> & b) const
    {
        return a->name < b->name;
    }
};

void Console::handleHelp(const std::string &command)
{
    if(command.empty()) {
        std::vector<shared_ptr<Command> > commands;
        {
            ScopedMutex lock(mMutex);
            commands = mCommandList;
        }
        std::sort(commands.begin(), commands.end(), pred());
        for(std::vector<shared_ptr<Command> >::const_iterator it = commands.begin(); it != commands.end(); ++it) {
            if(!(*it)->description.empty())
                Log::warn(TRACE_SYSTEM, "/%s -- %s", (*it)->name.c_str(), (*it)->description.c_str());
        }
    } else {
        handleCommand(command, Console_Help);
    }
}

void Console::dump(const Command::Arguments &arguments, const std::vector<Command::Help> &help, int depth)
{
    const size_t minSize = 3;
    size_t commandSize = minSize;
    for(std::vector<Command::Help>::const_iterator it = help.begin(); it != help.end(); ++it) {
        if(depth || arguments.size() == 1 || arguments[1] == (*it).name)
            commandSize = std::max(commandSize, (*it).value.size() + (*it).name.size() + minSize);
    }
    for(std::vector<Command::Help>::const_iterator it = help.begin(); it != help.end(); ++it) {
        if(depth || arguments.size() == 1 || arguments[1] == (*it).name) {
            std::string tmp = (*it).name;
            if(!(*it).value.empty())
                tmp += " " + (*it).value;
            tmp += ":";
            Log::warn(TRACE_SYSTEM, "%s%s%s%s", mkspaces(minSize*depth).c_str(), tmp.c_str(),
                      mkspaces(commandSize - tmp.size()).c_str(), (*it).description.c_str());
            dump(arguments, (*it).subcommands, depth+1);
        }
    }
}

Console::Filters Console::parseFilters(const std::vector<std::string>::const_iterator start,
                                       const std::vector<std::string>::const_iterator end,
                                       bool &ok)
{
    ok = true;
    Filters filters;
    for(std::vector<std::string>::const_iterator it = start; it != end; ) {
        if(*it != "|") {
            ok = false;
            break;
        }

        std::vector<std::string>::const_iterator start_it = ++it;
        while(it != end && *it != "|")
            ++it;
        if(start_it == it) {
            ok = false;
            break;
        }

        shared_ptr<Filter> filter = createFilter(start_it, it);
        if(!filter) {
            ok = false;
            break;
        }
        filters.push_back(filter);
    }
    if(!ok)
        filters.clear();
    return filters;
}

std::string Console::filter(const Filters &filters, const std::string &output,
                            const Configuration::ColorScheme &colorScheme)
{
    if(filters.empty())
        return output;

    std::vector<std::string> lines;
    StringTokenizer::tokenize(output, lines, "\n");
    const bool newLine = lines.size() > 1 || endsWith(output, "\n");

    for(std::vector<std::string>::iterator it = lines.begin(); it != lines.end(); ) {
        bool erase = false;
        std::string &line = (*it);
        for(Filters::const_iterator filter_it = filters.begin(); filter_it != filters.end(); ++filter_it) {
            if(!(*filter_it)->filter(line, colorScheme)) {
                erase = true;
                break;
            }
        }
        if(erase)
            it = lines.erase(it);
        else
            ++it;
    }

    std::string ret;
    for(std::vector<std::string>::iterator it = lines.begin(); it != lines.end(); ++it) {
        ret += (*it);
        if(newLine)
            ret += "\n";
    }
    return ret;
}

shared_ptr<Console::Filter> Console::createFilter(std::vector<std::string>::const_iterator start,
                                                  std::vector<std::string>::const_iterator end)
{
    if(*start == "grep" || *start == "/grep") {
        shared_ptr<Grep> grep(new Grep);
        if(grep->init(std::vector<std::string>(start+1, end)))
            return grep;
    } else if(*start == "/hilite" || *start == "hl" || *start == "highlight" || *start == "hilite") {
        shared_ptr<Grep> highlight(new Grep);
        if(highlight->init(std::vector<std::string>(start+1, end), Grep::Highlight))
            return highlight;
    } else if(*start == "wc") {
        shared_ptr<WordCount> wc(new WordCount);
        if(wc->init(std::vector<std::string>(start+1, end)))
            return wc;
    }
    return shared_ptr<Console::Filter>();
}

Console::FilterScope::FilterScope(const Console::Filters &filters)
{
    mFilters = app()->setFilters(filters);
}

Console::FilterScope::~FilterScope()
{
    app()->setFilters(mFilters);
}

