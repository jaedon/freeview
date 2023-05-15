/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef __GIBBONCONSOLE_H__
#define __GIBBONCONSOLE_H__

#include <nrdbase/Thread.h>
#include <nrdbase/ConsoleSink.h>
#include <nrdbase/StringUtils.h>
#include <nrdbase/StringTokenizer.h>
#include <nrdbase/Console.h>
#include <gibbon/config.h>
#include <nrdbase/DumpInfo.h>
#ifdef HAVE_LIBEDIT
#include <histedit.h>
#endif

struct Command;
class RawCommand;
namespace netflix {
namespace gibbon {

class DumpAllocTimer;
class GibbonConsoleProcessEvent;

class GibbonConsole : public Thread,
                      public enable_shared_from_this<GibbonConsole>,
                      public Console
{
public:
    GibbonConsole();

    enum DumpViewsFlag {
        Dump_NoOffscreen = 0x01,
        Dump_NoCached    = 0x02,
        Dump_NoEmpty     = 0x04,
        Dump_NoViewOrder = 0x08
    };
    static int dumpAllViews(unsigned char flags=0);
    static void dumpMemory(const std::vector<std::string> &arguments);

    enum KeyboardMode {
        Keyboard_Keyboard,
        Keyboard_PS2,
        Keyboard_Normal
    };

    void setKeyboardMode(KeyboardMode mode) { mKeyboardMode = mode; }
    KeyboardMode keyboardMode() const { return mKeyboardMode; }

    int jsMaxLength() const { return mJSMaxLength; }
    void setJSMaxLength(int len) { mJSMaxLength = len; }

    std::string formatJS(const Variant &variant) const;
#ifdef HAVE_LIBEDIT
    static void cleanupLibEdit() { if (sEditLine) { el_end(sEditLine); sEditLine = 0; } }
    static void resetLibEdit() { if (sEditLine) el_reset(sEditLine); }
#endif

    virtual bool handleCommand(const std::string &cmd, unsigned int flags);
    virtual bool handleSequence(const std::string &sequence);
    virtual void stop();
protected:
    virtual void Run();
private:
    friend class GibbonConsoleProcessEvent;
    void consoleReceive(const std::string &str);

    std::string mLastCommand;
    int mPipe[2];
    shared_ptr<DumpAllocTimer> mDumpJSTimer, mDumpMallinfoTimer;

#ifdef HAVE_LIBEDIT
    static unsigned char complete(EditLine *edit, int);
    unsigned char completeConsole(EditLine *edit, const LineInfo *lineInfo) const;
    unsigned char completeJavascript(EditLine *edit, const LineInfo *lineInfo) const;
    static int getChar(EditLine *edit, char *ch);
    static const char *prompt(EditLine *) { return "$ "; }
    unsigned readFromPipe();

    Mutex mMutex;
    enum State {
        None = 0x0,
        Reading = 0x1,
        Refresh = 0x2,
        Quit = 0x4
    };
    FILE *mLibEditLogFile;
    unsigned mState;
    static EditLine *sEditLine;
    History *mHistory;
    std::string mHistoryFile;

    struct Completion
    {
        Completion() : flags(0) {}
        ~Completion();
        void add(const std::string &name,
                 const Variant &variant = Variant(),
                 unsigned flags = 0);
        const Completion *match(std::string &text) const;
        std::vector<std::string> alternatives(int &longest, const std::string &base = std::string()) const;
        void dump(FILE *f = stdout, int indent = 0) const
        {
            for (std::map<std::string, Completion*>::const_iterator it = completions.begin(); it != completions.end(); ++it) {
                for (int i=0; i<indent; ++i)
                    fprintf(f, " ");
                fprintf(f, "%s\n", it->first.c_str());
                if (it->second)
                    it->second->dump(f, indent + 2);
            }
        }

        std::map<std::string, Completion*> completions;

        enum Flag {
            None = 0x0,
            Multi = 0x1,
            NoTrailingSpace = 0x2
        };
        unsigned flags;
    };
    std::auto_ptr<Completion> mCompletionRoot;
#endif
    bool mDone;
    KeyboardMode mKeyboardMode;
    char mEscape;
    int mJSMaxLength;
    friend class GibbonConsoleSink;
};

class GibbonConsoleSink : public ConsoleSink
{
public:
    GibbonConsoleSink();
    using ConsoleSink::receive;
    virtual void receive(int logLevel, const std::string &msg);
};

}} // namespace netflix::gibbon

#endif /* __GIBBONCONSOLE_H__ */
