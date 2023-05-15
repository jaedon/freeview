/*
 * (c) 1997-2015 Netflix, Inc. All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef __CONSOLE_H__
#define __CONSOLE_H__

#include <nrdbase/Configuration.h>
#include <nrdbase/ScopedMutex.h>
#include <nrdbase/StringUtils.h>
#include <nrdbase/Variant.h>
#include <nrdbase/Mutex.h>
#include <nrdbase/tr1.h>
#include <string>
#include <vector>

namespace netflix {

class Console
{
public:
    typedef shared_ptr<Console> SharedPtr;

    Console();
    virtual ~Console() {}

    class Filter
    {
    public:
        Filter() { }
        virtual ~Filter() { }

        virtual bool filter(std::string &text, const Configuration::ColorScheme &colorScheme = Configuration::ColorScheme()) const = 0;

        virtual std::string describe() const { return std::string(); }
    };
    typedef std::vector<shared_ptr<Filter> > Filters;
    Filters parseFilters(const std::vector<std::string>::const_iterator start,
                         const std::vector<std::string>::const_iterator end,
                         bool &ok);
    static std::string filter(const Filters &filters, const std::string &output,
                              const Configuration::ColorScheme &colorScheme = Configuration::ColorScheme());

    virtual shared_ptr<Filter> createFilter(std::vector<std::string>::const_iterator start,
                                            std::vector<std::string>::const_iterator end);

    class FilterScope
    {
    public:
        FilterScope(const Console::Filters &filters);
        ~FilterScope();
    private:
        Console::Filters mFilters;
    };

    class Command : public enable_shared_from_this<Command>
    {
    public:
        enum Flag {
            None = 0x0,
            Silent = 0x1
        };

        Command(const std::string &n, const std::string &d, const Variant &c = Variant(), unsigned int f = None)
            : name(n), description(d), completions(c), flags(f)
        {}
        virtual ~Command()
        {}

        struct Arguments {
	        inline Arguments(const std::string &_command, const std::vector<std::string> &_arguments, const Console::Filters &filters)
                : command(_command), arguments(_arguments)
            {
                if(!filters.empty())
                    mFilters.reset(new FilterScope(filters));
            }
            inline const std::string &operator[](int index) const
            {
                return arguments[index];
            }
            inline bool contains(const std::string &str) const
            {
                return std::find(arguments.begin(), arguments.end(), str) != arguments.end();
            }
            inline bool isEmpty() const { return !size(); }
            inline size_t size() const
            {
                return arguments.size();
            }

            const std::string command;
            const std::vector<std::string> arguments;
            shared_ptr<FilterScope> mFilters;
        };

        virtual void invoke(const Arguments &arguments) const = 0;

        struct Help {
            inline Help() { }
            inline Help(const std::string &_name, const std::string &_description) :
                name(_name), description(_description)
            {
            }
            inline Help(const std::string &_name, const std::string &_value, const std::string &_description) :
                name(_name), value(_value), description(_description)
            {
            }

            inline Help &operator<<(const Help &command) {
                append(command);
                return *this;
            }
            inline void append(const Help &command) { subcommands.push_back(command); }

            std::string name, value, description;
            std::vector<Help> subcommands;
        };
        virtual std::vector<Help> help() const { return std::vector<Help>(); }

        const std::string name;
        const std::string description;
        const Variant completions;
        const unsigned int flags;
    };

    void addCommand(const shared_ptr<Command> &command)
    {
        assert(command);
        ScopedMutex lock(mMutex);
#ifdef BUILD_DEBUG
        if (!command->name.empty()) {
            for (std::vector<shared_ptr<Command> >::const_iterator it = mCommandList.begin(); it != mCommandList.end(); ++it) {
                assert(command->name != (*it)->name);
            }
        }
#endif
        mCommandList.push_back(command);
    }

    std::vector<shared_ptr<Command> > commands() const
    {
        ScopedMutex lock(mMutex);
        return mCommandList;
    }

    virtual void handleHelp(const std::string &command);
    enum Flag {
        Console_None = 0x0,
        Console_NoHistory = 0x1,
        Console_Help = 0x2
    };
    virtual bool handleCommand(const std::string &command, unsigned int flags);
    virtual bool handleSequence(const std::string &/*sequence*/) { return false; }

    virtual void stop();

    static std::vector<std::string> shellTokenize(const std::string &string);
private:
    void dump(const Command::Arguments &arguments, const std::vector<Command::Help> &help, int depth=0);
    mutable Mutex mMutex;
    std::vector<shared_ptr<Command> > mCommandList;
};

}

#endif // __CONSOLE_H__
