/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "GibbonConsole.h"
#include <nrdbase/config.h>
#include <nrd/config.h>
#include <nrdbase/Endian.h>
#include <nrdbase/JSONFormatter.h>
#include <nrdnet/DiskCache.h>
#include <nrdnet/ResourceManager.h>
#include <nrdnet/DnsManager.h>
#include <GibbonConsole.h>
#if GIBBON_BENCHMARKS
#include <Benchmarks.h>
#endif
#include <RemoteScreen.h>
#include <SurfaceCache.h>
#include <nrdbase/DumpInfo.h>
#include <GibbonConfiguration.h>
#include <GibbonApplication.h>
#include <SurfaceDecoder.h>
#include <WidgetBridge.h>
#include <InputManager.h>
#include <FontManager.h>
#include <GibbonFreetype.h>
#include <GibbonEvent.h>
#include <Screen.h>
#include <Debug.h>

#include <nrd/AppLog.h>
#include <nrd/AppThread.h>
#include <nrdbase/Instrumentation.h>
#include <nrdbase/PerformanceMarker.h>
#include <nrdbase/ReadDir.h>
#include <nrdbase/TraceAreas.h>
#include <nrdbase/StringTokenizer.h>
#include <nrdbase/StringUtils.h>
#include <nrdbase/ConsoleSink.h>

#ifdef HAVE_GIBBON_TCMALLOC
# include <gperftools/malloc_extension.h>
#endif

#ifdef NRDP_HAS_SOCKETIZER
# include <nrdsocketizer/Socketizer.h>
using namespace netflix::socketizer;
#endif

#if defined(NRDP_HAS_PERFORMANCE_MARKERS)
# define GIBBON_SAMPLE_ENABLED
#endif

#ifdef HAVE_LIBEDIT
extern "C" int tgetent(char *, const char *);
extern "C" int tgetnum(char *);
#endif

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#if 0 /* HUMAX */
#if defined(NRDP_PLATFORM_LINUX) && !defined(__APPLE__)
#include <gnu/libc-version.h>
#endif
#else
#if defined(NRDP_PLATFORM_LINUX) && !defined(__APPLE__)
#if !defined(__UCLIBC__)
#include <gnu/libc-version.h>
#endif
#endif
#endif /* HUMAX */


#include <signal.h>


using namespace netflix::mediacontrol;
using namespace netflix::gibbon;
using namespace netflix;

static shared_ptr<GibbonConsole> gibbonConsole()
{
    return app() ? static_pointer_cast<GibbonConsole>(app()->console()) : shared_ptr<GibbonConsole>();
}

static inline FILE *backupOpen(const std::string &file)
{
    if(FILE *f = fopen(file.c_str(), "r+")) {
        fseek(f, 0, SEEK_END);
        const int size = ftell(f);
        if(size) {
            char buf[1024];
            snprintf(buf, sizeof(buf), "%s.bak", file.c_str());
            if(FILE *bak = fopen(buf, "w")) {
                fseek(f, 0, SEEK_SET);
                int i = 0;
                while(i < size) {
                    const int read = fread(buf, sizeof(char), sizeof(buf), f);
                    if(read <= 0)
                        break;
                    const int written = fwrite(buf, sizeof(char), read, bak);
                    i += written; // assuming that sizeof(char) == 1
                }
                fclose(bak);
            }
        } else {
            return f;
        }
        fclose(f);
    }
    return fopen(file.c_str(), "w");
}


namespace netflix {
namespace gibbon {

class KeyRepeatTimer : public GibbonApplication::Timer
{
public:
    KeyRepeatTimer(int interval, unsigned int keycode, const std::string &text, int total, const Time &sent)
        : GibbonApplication::Timer(GibbonConfiguration::keyAutoRepeatDelay()), mInterval(interval), mText(text), mKeyCode(keycode), mCount(1), mTotal(total), mSent(sent)
    {
    }

    virtual std::string describe() const { return "KeyRepeatTimer"; }

    virtual void timerFired()
    {
        PERFORMANCE_MARKER_SCOPED("consoletimer.fired");
        if(mCount == 1)
            restart(mInterval);
        Log::warn(TRACE_UI_ENGINE, "Send Key: %s [%d] (%d/%d)", mText.c_str(), mKeyCode, ++mCount, mTotal);
        gibbonApp()->sendEvent(new KeyEvent(InputEvent::Type_KeyPress, mKeyCode, mText, 0, true));
        if(mCount == mTotal) {
            gibbonApp()->sendEvent(new KeyEvent(InputEvent::Type_KeyRelease, mKeyCode, mText, 0));
            stop();
        }
    }

private:
    int mInterval;
    std::string mText;
    unsigned int mKeyCode;
    int mCount;
    int mTotal;
    Time mSent;
};

class DumpAllocTimer : public GibbonApplication::Timer
{
public:
    DumpAllocTimer(int interval, int m)
        : GibbonApplication::Timer(interval), mode(m)
    {
    }

    virtual std::string describe() const { return "DumpAllocTimer"; }

    virtual void timerFired()
    {
        PERFORMANCE_MARKER_SCOPED("dumpalloctimer.fired");
        gibbonApp()->dumpMalloc(mode);
    }

private:
    int mode;
};

class GibbonConsoleCommandEvent : public Application::Event
{
public:
    GibbonConsoleCommandEvent(const shared_ptr<const Console::Command> &command,
                              const Console::Command::Arguments &args)
        : Application::Event(), mCommand(command), mArguments(args)
    {
        assert(mCommand);
    }

    virtual void eventFired()
    {
        mCommand->invoke(mArguments);
    }
private:
    shared_ptr<const Console::Command> mCommand;
    Console::Command::Arguments mArguments;
};

class GibbonConsoleJSEvent : public GibbonApplication::Event
{
public:
    GibbonConsoleJSEvent(const std::string &script)
        : mScript(script)
    {}
    std::string describe() const { return std::string("GibbonConsoleJSEvent"); }
    virtual void eventFired()
    {
        Variant result;
        gibbonApp()->getScriptEngine()->evaluate(mScript, AseUrl("CONSOLE"), 0, &result,
                                                 GibbonConfiguration::consoleJSDepth());
        Log::warn(TRACE_JAVASCRIPT, "Result:\n%s", gibbonConsole()->formatJS(result).c_str());
        gibbonApp()->executed(mScript);
    }
protected:
    const std::string mScript;
};

#if GIBBON_BENCHMARKS
class GibbonRunBenchmarkEvent : public GibbonApplication::Event
{
public:
    GibbonRunBenchmarkEvent(const std::string &name, const std::vector<std::string> &arguments) : mName(name), mArguments(arguments) { }

    void eventFired() { Benchmarks::run(mName, mArguments); }

private:
    std::string mName;
    std::vector<std::string> mArguments;
};
#endif

std::string gibbon_indent(int indent)
{
    std::ostringstream result;
    result << "[" << indent << "] ";
    for(int i = 0; i < indent; ++i)
        result << "  ";
    return result.str();
}

}};

static void checkViews(Widget::SharedPtr w, bool all)
{
    if(all || !w->isOnScreen())
        return;
    const Rect rect = w->getTransformedRenderRect();
    const Widget::ChildContainer &children = w->getChildren();
    for(Widget::ChildContainer::const_iterator it = children.begin(); it != children.end(); ++it) {
        Widget::SharedPtr child = (*it);
        if(!w->getClipEnabled()) {
            const Rect childRect = child->getTransformedRenderRect();
            if(childRect.x < 0)
                Log::warn(TRACE_UI_ENGINE, "%s is off the left!\n", child->describe().c_str());
            if(childRect.y < 0)
                Log::warn(TRACE_UI_ENGINE, "%s is off the top!\n", child->describe().c_str());
            if(childRect.bottom() > rect.bottom())
                Log::warn(TRACE_UI_ENGINE, "%s is off the bottom!\n", child->describe().c_str());
            if(childRect.right() > rect.right())
                Log::warn(TRACE_UI_ENGINE, "%s is off the right!\n", child->describe().c_str());
        }
        checkViews(child, all);
    }
}

struct Key {
    KeyEvent::Key key;
    const char *name;
    const char *uiKey;
} static const keys[] = {
    { KeyEvent::KEY_UP, "up", "key.up" },
    { KeyEvent::KEY_DOWN, "down", "key.down" },
    { KeyEvent::KEY_LEFT, "left", "key.left" },
    { KeyEvent::KEY_RIGHT, "right", "key.right" },
    { KeyEvent::KEY_END, "end", 0 },
    { KeyEvent::KEY_HOME, "home", 0 },
    { KeyEvent::KEY_F1, "f1", 0 },
    { KeyEvent::KEY_F2, "f2", 0 },
    { KeyEvent::KEY_F3, "f3", 0 },
    { KeyEvent::KEY_F4, "f4", 0 },
    { KeyEvent::KEY_F5, "f5", 0 },
    { KeyEvent::KEY_F6, "f6", 0 },
    { KeyEvent::KEY_F7, "f7", 0 },
    { KeyEvent::KEY_F8, "f8", 0 },
    { KeyEvent::KEY_F9, "f9", 0 },
    { KeyEvent::KEY_F10, "f10", 0 },
    { KeyEvent::KEY_F11, "f11", 0 },
    { KeyEvent::KEY_F12, "f12", 0 },
    { KeyEvent::KEY_RETURN, "return", "key.enter" },
    { KeyEvent::KEY_RETURN, "enter", "key.enter" },
    { KeyEvent::KEY_RETURN, "select", "key.enter" },
    { KeyEvent::KEY_ESCAPE, "escape", "key.back" },
    { KeyEvent::KEY_ESCAPE, "back", "key.back" },
    { KeyEvent::KEY_UNKNOWN, 0, 0 }
};

static bool sendKeyEvent(const std::string &events, int count, int interval)
{
    bool result = false;
    std::vector<std::string> k;
    StringTokenizer::tokenize(events, k, ",");
    for(size_t i = 0; i < k.size(); ++i) {
        std::string key = k.at(i);
        KeyEvent *event = 0;
        for(int j=0; keys[j].name; ++j) {
            if(!strcmp(key.c_str(), keys[j].name)) {
                event = new KeyEvent(InputEvent::Type_KeyPress, keys[j].key, std::string(), 0, false, 1, keys[j].uiKey ? std::string(keys[j].uiKey) : std::string());
                break;
            }
        }
        if(!event) {
            if(key.substr(0,2)=="0x") {
                event = new KeyEvent(InputEvent::Type_KeyPress, strtoul(key.c_str(), 0, 0), std::string(), 0, false);
            } else {
                if(key == "comma")
                    key = ',';
                event = new KeyEvent(InputEvent::Type_KeyPress, KeyEvent::keyFromAscii(*key.c_str()), key, 0, false);
            }
        }
        if(event) {
            Time sent = Time::mono();
            const unsigned event_key = event->mKey;
            const std::string event_text = event->mText;
            gibbonApp()->sendEvent(event);
            if(count > 1) {
                shared_ptr<KeyRepeatTimer> timer(new KeyRepeatTimer(interval, event_key, event_text, count, sent));
                gibbonApp()->startTimer(timer);
            } else {
                gibbonApp()->sendEvent(new KeyEvent(InputEvent::Type_KeyRelease, event_key, event_text, 0));
            }
            result = true;
        }
    }
    return result;
}

static bool dumpWidget(const shared_ptr<WidgetBridge> &widget, const std::string &file)
{
    if(FILE *js = backupOpen(file.c_str())) {
        const Variant result = widget->dump(WidgetBridge::Dump_Recursive);
        const JSONFormatter formatter(JSONFormatter::Pretty|JSONFormatter::UnprettySmallObjects);
        fprintf(js, "// %s\n", widget->getWidget()->describe().c_str());
        fprintf(js, "var widget = %s;\n", formatter.format(result).c_str());
        fprintf(js, "function createWidget(obj) {\n");
        fprintf(js, "     return nrdp.gibbon.makeWidget(obj);\n");
        fprintf(js, "}\n");
        fprintf(js, "function makeScene() { nrdp.gibbon.scene.widget = createWidget(widget); } \n");
        fprintf(js, "function main() {\n");
        int downloaded_fonts = 0;
        {
            const FontManager::FontRegistry &fonts = FontManager::getFontRegistry();
            for(FontManager::FontRegistry::const_iterator it = fonts.begin(); it != fonts.end(); ++it) {
                if(!it->second.downloaded.empty()) {
                    if(!downloaded_fonts++)
                        fprintf(js, "    var fonts = [];\n");
                    fprintf(js, "    fonts.push(%s);\n", it->second.downloaded.toJSON().c_str());
                }
            }
        }
        if(downloaded_fonts) { //download all fonts before loading the scene
            fprintf(js, "    var waiting = %d;\n", downloaded_fonts);
            fprintf(js, "    function callback(data) { if(!--waiting) makeScene(); };\n");
            fprintf(js, "    for(var font in fonts) { nrdp.gibbon.addFont(fonts[font], callback); }\n");
        } else {
            fprintf(js, "    makeScene();\n");
        }
        fprintf(js, "}\n");
        fprintf(js, "nrdp.gibbon._baseUrl = \"%s\";\n", gibbonApp()->getLocation().str().c_str());
        fprintf(js, "nrdp.addEventListener(\"init\", main);\n");
        fprintf(js, "nrdp.isReady ? main() : nrdp.init();\n");
        fclose(js);
        return true;
    }
    return false;
}

static int dumpViews(Widget::SharedPtr w, unsigned char flags, int indent, int depth)
{
    if(!depth)
        return 0;
    if((flags & GibbonConsole::Dump_NoOffscreen) && !w->isOnScreen())
        return 0;
    if((flags & GibbonConsole::Dump_NoEmpty) && !w->hasContent())
        return 0;

    int result = 1;
    std::string description = w->describe(true);
    if(!indent && w->getParent())
        description += " parent:" + w->getParent()->describe(false);
    Log::error(TRACE_UI_ENGINE) << gibbon_indent(indent) << description << std::endl;
    if(!(flags & GibbonConsole::Dump_NoCached) || !w->shouldRenderCache()) {
        const Widget::ChildContainer &children = w->getChildren(!(flags & GibbonConsole::Dump_NoViewOrder));
        for(Widget::ChildContainer::const_iterator it = children.begin(); it != children.end(); ++it)
            result += dumpViews((*it), flags, indent+1, depth-1);
    }
    return result;
}
int GibbonConsole::dumpAllViews(unsigned char flags)
{
    if(Widget::SharedPtr widget = gibbonApp()->getScreen()->root())
        return dumpViews(widget, flags, 0, -1);
    return 0;
}

#ifdef GIBBON_GRAPHICSENGINE_DIRECTFB
DFBEnumerationResult dfbEnumDisplayLayer(DFBDisplayLayerID layer_id, DFBDisplayLayerDescription, void *callbackdata)
{
    char buf[16];
    snprintf(buf, sizeof(buf), "%d", layer_id);
    (*reinterpret_cast<Variant*>(callbackdata))[buf] = "true false";
    return DFENUM_OK;
}
#endif

static inline Variant setCompletions()
{
    std::string debug;
    const std::string formats = StringTokenizer::join(Screen::formats(), " ");
    {
        const std::vector<Console::Command::Help> debugHelp = Debug::help();
        for(std::vector<Console::Command::Help>::const_iterator it = debugHelp.begin(); it != debugHelp.end(); ++it) {
            if(!debug.empty())
                debug += " ";
            debug += (*it).name;
        }
    }
    std::string traceAreas = StringTokenizer::join(TraceAreas::all(), " ");
#ifdef GIBBON_GRAPHICSENGINE_DIRECTFB
    Variant layers;
    Screen::SharedPtr screen = gibbonApp()->getScreen();
    if(screen && screen->isRunning()) {
        std::string dfbOptions;
        IDirectFB *dfb = screen->getDirectFB();
        dfb->EnumDisplayLayers(dfb, dfbEnumDisplayLayer, &layers);
    }
#endif
    traceAreas += " none all";
    const Variant ret = Variant::StringMap("fps")
        ("log",traceAreas)
        ("screen",Variant::StringMap("suspend")("alpha",formats)("opaque",formats)
#ifdef GIBBON_GRAPHICSENGINE_DIRECTFB
         ("dfb", layers)
#endif
            )
        ("debug",debug)
        ("thread")
        ("jsMaxLength")
        ("cache",Variant::StringMap("memory","clear")("disk","clear clearStats flush"));
    return ret;
}

static inline Variant keyCompletions()
{
    Variant ret;
    ret["comma"] = Variant();
    for(int i=0; keys[i].name; ++i) {
        ret[keys[i].name] = Variant();
    }
    return ret;
}

class GibbonPostedCommand : public Console::Command
{
public:
    enum DesiredThread {
        Thread_Any,
        Thread_Animation,
        Thread_Application
    };
    GibbonPostedCommand(DesiredThread desiredThread,
                        const char *name,
                        const char *description,
                        const Variant &completions = Variant())
        : Console::Command(name, description, completions, Silent), mDesiredThread(desiredThread)
    {}

    virtual void invoke(const Arguments &arguments) const
    {
        invokeOnThread(mDesiredThread, arguments);
    }
    virtual void invokeOnCurrentThread(const Arguments &arguments) const = 0;
protected:
    void invokeOnThread(DesiredThread thread, const Arguments &arguments) const
    {
        switch (thread) {
        case Thread_Any:
            break;
        case Thread_Animation:
#ifndef GIBBON_NO_THREADANIMATION
            if(!Animation::isAnimationThread()) {
                Animation::postEvent(shared_ptr<Application::Event>(new GibbonConsoleCommandEvent(shared_from_this(), arguments)));
                return;
            }
            break;
#endif
            // fall through
        case Thread_Application:
            if(!Application::isAppThread()) {
                app()->postEvent(shared_ptr<Application::Event>(new GibbonConsoleCommandEvent(shared_from_this(), arguments)));
                return;
            }
            break;
        }
        invokeOnCurrentThread(arguments);
        app()->executed(arguments.command);
    }
private:
    const DesiredThread mDesiredThread;
};

class ApplicationCommand : public GibbonPostedCommand
{
public:
    ApplicationCommand(const char *name, const char *description, const Variant &completions = Variant())
        : GibbonPostedCommand(Thread_Application, name, description, completions)
    {}
};

class AnimationCommand : public GibbonPostedCommand
{
public:
    AnimationCommand(const char *name, const char *description, const Variant &completions = Variant())
        : GibbonPostedCommand(Thread_Animation, name, description, completions)
    {}
};

class RawCommand : public Console::Command
{
public:
    RawCommand()
        : Console::Command("raw", "Put console in raw mode", "key ps2")
    {}
    virtual void invoke(const Arguments &arguments) const
    {
        if(shared_ptr<GibbonConsole> console = gibbonConsole()) {
            GibbonConsole::KeyboardMode mode = GibbonConsole::Keyboard_Keyboard;
            if(arguments.size() > 1 && arguments[1] == "ps2")
                mode = GibbonConsole::Keyboard_PS2;
            Log::error(TRACE_LOG, "Entered Raw Mode: %d", mode);
            console->setKeyboardMode(mode);
        }
    }
    virtual std::vector<Help> help() const
    {
        std::vector<Help> result;
        result.push_back(Help("ps2", "Interpret raw commands as a ps3 mouse protocol."));
        return result;
    }
};

class RemoteScreenCommand : public Console::Command
{
public:
    RemoteScreenCommand()
        : Console::Command("remotescreen", "RemoteScreen information")
    {}
    virtual void invoke(const Arguments &arguments) const
    {
        if(shared_ptr<RemoteScreenServer> server = gibbonApp()->getRemoteScreenServer()) {
            if(arguments.size() > 1) {
                if(arguments[1] == "ping")
                    server->ping(true);
                else if(arguments[1] == "surfaces")
                    server->dumpSurfaces();
                else
                    server->dump();
            } else {
                server->dump();
            }
        } else if(shared_ptr<RemoteScreenClient> client = gibbonApp()->getRemoteScreenClient()) {
            if(arguments.size() > 1) {
                if(arguments[1] == "ping") {
                    client->ping(true);
                } else if(arguments[1] == "surfaces") {
                    client->dumpSurfaces();
                } else if(arguments[1] == "eval") {
                    std::string::size_type space1 = arguments.command.find(' ');
                    if(space1 != std::string::npos) {
                        std::string::size_type space2 = arguments.command.find(' ', space1);
                        if(space2 != std::string::npos) {
                            const std::string line = arguments.command.substr(space2+1);
                            client->serverEval(line);
                        }
                    }
                } else {
                    client->dump();
                }
            } else {
                client->dump();
            }
        }
    }
};

class AnimationsCommand : public Console::Command
{
public:
    AnimationsCommand()
        : Console::Command("animations", "Dump animation information")
    {}
    virtual void invoke(const Arguments &) const
    {
        Animation::dump();
    }
};

#ifdef NRDP_HAS_VALGRIND
#include </usr/include/valgrind/memcheck.h>
#include </usr/include/valgrind/callgrind.h>
class ValgrindCommand : public Console::Command
{
public:
    ValgrindCommand() : Console::Command("valgrind", "Dump valgrind information",
                                         "VALGRIND_DO_CHANGED_LEAK_CHECK VALGRIND_DO_LEAK_CHECK CALLGRIND_DUMP_STATS CALLGRIND_ZERO_STATS CALLGRIND_TOGGLE_COLLECT VALGRIND_DO_QUICK_LEAK_CHECK CALLGRIND_START_INSTRUMENTATION CALLGRIND_STOP_INSTRUMENTATION")
    {
    }
    virtual void invoke(const Arguments &arguments) const
    {
        if(!RUNNING_ON_VALGRIND)
            Log::error(TRACE_LOG, "Not running with valgrind!");
        else if(arguments.size() < 2 || arguments[1] == "VALGRIND_DO_CHANGED_LEAK_CHECK")
            VALGRIND_DO_CHANGED_LEAK_CHECK;
        else if(arguments[1] == "VALGRIND_DO_LEAK_CHECK"|| arguments[1] == "leaks")
            VALGRIND_DO_LEAK_CHECK;
        else if(arguments[1] == "VALGRIND_DO_QUICK_LEAK_CHECK")
            VALGRIND_DO_QUICK_LEAK_CHECK;
        else if(arguments[1] == "CALLGRIND_DUMP_STATS")
            CALLGRIND_DUMP_STATS;
        else if(arguments[1] == "CALLGRIND_ZERO_STATS")
            CALLGRIND_ZERO_STATS;
        else if(arguments[1] == "CALLGRIND_TOGGLE_COLLECT")
            CALLGRIND_TOGGLE_COLLECT;
        else if(arguments[1] == "CALLGRIND_START_INSTRUMENTATION")
            CALLGRIND_START_INSTRUMENTATION;
        else if(arguments[1] == "CALLGRIND_STOP_INSTRUMENTATION")
            CALLGRIND_STOP_INSTRUMENTATION;
    }
    virtual std::vector<Help> help() const
    {
        std::vector<Help> result;
        result.push_back(Help("VALGRIND_DO_CHANGED_LEAK_CHECK", ""));
        result.push_back(Help("VALGRIND_DO_LEAK_CHECK", ""));
        result.push_back(Help("VALGRIND_DO_QUICK_LEAK_CHECK", ""));
        result.push_back(Help("CALLGRIND_DUMP_STATS", ""));
        result.push_back(Help("CALLGRIND_ZERO_STATS", ""));
        result.push_back(Help("CALLGRIND_TOGGLE_COLLECT", ""));
        result.push_back(Help("CALLGRIND_START_INSTRUMENTATION", ""));
        result.push_back(Help("CALLGRIND_STOP_INSTRUMENTATION", ""));
        return result;
    }
};
#endif

class MemoryCommand : public GibbonPostedCommand
{
public:
    MemoryCommand()
        : GibbonPostedCommand(Thread_Any, "memory", "Dump memory information",
                              Variant::StringMap("js", Variant::StringMap("count")("heap")("strings", "raw"))
                              ("alloc")
                              ("stats")
                              ("timer")
                              ("tcmalloc")
                              ("gc")
                              ("aggressive", "off")
                              ("all")
#ifdef NRDP_JSC_ENABLE_OSALLOCATOR_CALLBACKS
                              ("osallocator", "full")
#endif
            )
    {}
    virtual void invokeOnCurrentThread(const Arguments &arguments) const
    {
        GibbonConsole::dumpMemory(arguments.arguments);
    }
    virtual void invoke(const Arguments &arguments) const
    {
        const bool isjs = arguments.contains("js") || arguments.contains("osallocator") || arguments.contains("all");
        invokeOnThread(isjs ? Thread_Application : Thread_Any, arguments);
    }
    virtual std::vector<Help> help() const
    {
        std::vector<Help> result;
        result.push_back(Help("js", "Enable JS memory information dump.")
                         << Help("count", "Count information")
                         << Help("heap", "Dump full JS heap")
                         << Help("strings", "Dump string data"));
        result.push_back(Help("osallocator", "Enable JS 'osallocator' information.") << Help("full", "Enable all information"));
        result.push_back(Help("alloc", "Dump allocated summaries (either malloc or js)."));
        result.push_back(Help("stats", "Dump smaps values"));
        result.push_back(Help("timer", "<ms>", "Start a timer to repeat the query every <ms>."));
        result.push_back(Help("tcmalloc", "Dump tcmalloc information."));
        result.push_back(Help("gc", "Release tcmalloc memory back to system."));
        result.push_back(Help("aggressive", "Release memory back aggressively") << Help("off", "Turn off"));
        result.push_back(Help("all", "Show all memory"));
        return result;
    }
};

class WidgetCommand : public ApplicationCommand
{
public:
    WidgetCommand()
        : ApplicationCommand("widget", "Display bridge information in JS.")
    {}
    virtual void invokeOnCurrentThread(const Arguments &arguments) const
    {
        ScopedMutex locker(Widget::lock());
        const int id = arguments.size() > 1 ? atoi(arguments[1].c_str()) : -1;
        if(arguments.size() > 2) {
            std::ostringstream eval;
            eval << "nrdp.gibbon.findWidget(" << id << ").";
            {
                std::vector<std::string> widgetArgs;
                StringTokenizer::tokenize(arguments.command, widgetArgs);
                for(size_t i = 2; i < widgetArgs.size(); ++i) {
                    if(i != 2)
                        eval << " ";
                    eval << widgetArgs[i];
                }
            }
            Log::error(TRACE_LOG, "[%s:%d]: EVAL %s", __func__, __LINE__, eval.str().c_str());
            Variant result;
            gibbonApp()->getScriptEngine()->evaluate(eval.str(), AseUrl("CONSOLE"), 0, &result, 1);
            {
                const std::string result_str = gibbonConsole()->formatJS(result);
                Log::warn(TRACE_UI_ENGINE, "Result:\n%s", result_str.c_str());
            }
        } else if(shared_ptr<WidgetBridge> widget = gibbonApp()->getBridge()->getWidget(id)) {
            const Variant result = widget->dump(WidgetBridge::Dump_All);
            Log::warn(TRACE_UI_ENGINE, "WidgetBridge(%d):\n%s", widget->getID(), gibbonConsole()->formatJS(result).c_str());
        }
    }
    virtual std::vector<Help> help() const
    {
        std::vector<Help> result;
        result.push_back(Help("<id>", "Dump all information about widget (either widget id# or name)."));
        result.push_back(Help("<js>", "Evaluate js on widget matching <id>."));
        return result;
    }
};

#ifdef GIBBON_SAMPLE_ENABLED
static bool sSampleEnabled = false;
class GibbonSample : public ThreadLocalObject
{
public:
    GibbonSample() { }
    virtual ~GibbonSample() {
        while(!mSamples.empty())
            pop();
    }
    void push(const std::string &name)
    {
        mSamples.push_back(std::make_pair(name+"(ms)", Stopwatch()));
    }
    void pop()
    {
        if(mSamples.empty())
            return;
        const std::pair<std::string, Stopwatch> &sample = mSamples.back();
        const Stopwatch::ms elapsed = sample.second.elapsed();
        Log::debug(TRACE_UI_ENGINE, "SAMPLE[%p] \"%s\"@%lld %f\n", Thread::CurrentThreadPtr(),
                   sample.first.c_str(), Time::mono().ms(), elapsed);
        mSamples.pop_back();
    }

protected:
    std::vector<std::pair<std::string, Stopwatch> > mSamples;
};

class SampleCommand : public Console::Command
{
public:
    SampleCommand() : Console::Command("sample", "Gibbon Samples", Variant::StringMap("start")("stop")) { }
    virtual void invoke(const Arguments &arguments) const
    {
        if(arguments.size() > 1)
            sSampleEnabled = arguments[1] == "start";
    }
};

namespace netflix {
void performanceMarkerStart(const char * name, bool)
{
    if(!sSampleEnabled)
        return;

    GibbonSample *sample = (GibbonSample*)ThreadLocalStore::getObject("gibbon.sample");
    if(!sample) {
        sample = new GibbonSample;
        ThreadLocalStore::setObject("gibbon.sample", sample);
    }
    sample->push(name);
}

void performanceMarkerEnd()
{
    if(!sSampleEnabled)
        return;

    if(GibbonSample *sample = (GibbonSample*)ThreadLocalStore::getObject("gibbon.sample"))
        sample->pop();
}
}
#endif

class FooCommand : public Console::Command
{
public:
    FooCommand()
        : Console::Command("foo", "Do your foo here")
    {}

    virtual void invoke(const Arguments &arguments) const
    {
        (void)arguments;
    }
};

class TasksCommand : public Console::Command
{
public:
    TasksCommand()
        : Console::Command("tasks", "Dump current JS tasks")
    {}

    virtual void invoke(const Arguments &) const
    {
        if(shared_ptr<GibbonBridge> gibbon = gibbonApp()->getBridge())
            gibbon->dumpTasks();
    }
};

class GCCommand : public ApplicationCommand
{
public:
    GCCommand()
        : ApplicationCommand("gc", "Force a garbage collection", "surfaces")
    {}

    virtual void invokeOnCurrentThread(const Arguments &arguments) const
    {
        uint8_t collectFlags = GibbonApplication::Collect_JS;
        for(size_t i = 1; i < arguments.size(); ++i) {
            if(arguments[i] == "surfaces") {
                collectFlags |= GibbonApplication::Collect_Surfaces;
            } else if(arguments[i] == "tcmalloc") {
                collectFlags |= GibbonApplication::Collect_TcMalloc;
            }
        }
        const size_t collected = gibbonApp()->garbageCollect(collectFlags);
        Log::warn(TRACE_UI_ENGINE, "Collected garbage, %s freed\n", prettyBytes(collected).c_str());
    }
    virtual std::vector<Help> help() const
    {
        std::vector<Help> result;
        result.push_back(Help("surfaces", "Include unused surfaces in GC attempt."));
        return result;
    }
};

class RenderCommand : public AnimationCommand
{
public:
    RenderCommand()
        : AnimationCommand("render", "Display render commands.")
    {}

    virtual void invokeOnCurrentThread(const Arguments &arguments) const
    {
        ScopedMutex locker(Widget::lock());
        unsigned char flags=Widget::Render_Comment;
        std::list<Widget::SharedPtr> widgets;
        for(size_t i = 1; i < arguments.size(); ++i) {
            if(arguments[i] == "nocache")
                flags |= Widget::Render_NoCache;
            else if(Widget::SharedPtr w = gibbonApp()->findWidget(DumpInfo(arguments[i])))
                widgets.push_back(w);
        }
        if(widgets.empty()) {
            widgets.push_back(gibbonApp()->getScreen()->root());
            widgets.push_back(gibbonApp()->getScreen()->overlay());
        }
        if(DisplayList::SharedPtr grab = gibbonApp()->getScreen()->rootDisplayList())
            grab->dump();
        for(std::list<Widget::SharedPtr>::const_iterator it = widgets.begin(); it != widgets.end(); ++it) {
            if(Widget::SharedPtr widget = (*it)) {
                if(DisplayList::SharedPtr grab = widget->grab(flags))
                    grab->dump();
            }
        }
    }
    virtual std::vector<Help> help() const
    {
        std::vector<Help> result;
        result.push_back(Help("nocache", "Dump actual commands for widgets that would have been cached."));
        return result;
    }
};

class ReloadCommand : public ApplicationCommand
{
public:
    ReloadCommand()
        : ApplicationCommand("reload", "Reload current page", "base uiboot current")
    {}

    virtual void invokeOnCurrentThread(const Arguments &arguments) const
    {
        gibbonApp()->reloadCaches();
        if(arguments.size() == 1) {
            gibbonApp()->reload(GibbonApplication::Location_Current);
        } else {
            if(arguments[1] == "base") {
                gibbonApp()->reload(GibbonApplication::Location_Base);
            } else if(arguments[1] == "current") {
                gibbonApp()->reload(GibbonApplication::Location_Current);
            } else {
                std::string location = arguments[1];
                char* end;
                const long int n = strtol(location.c_str(), &end, 10);
                if(end == location.c_str()+location.size()) {
                    ScopedMutex locker(Widget::lock());
                    if(shared_ptr<WidgetBridge> widget = gibbonApp()->getBridge()->getWidget(n)) {
                        const std::string tmpDump = "/tmp/reload.js";
                        if(dumpWidget(widget, tmpDump))
                            location = "file://" + tmpDump;
                    }
                }
                gibbonApp()->setLocation(location);
            }
        }
    }
    virtual std::vector<Help> help() const
    {
        std::vector<Help> result;
        result.push_back(Help("base", "Reload starting from appboot."));
        result.push_back(Help("current", "Reload starting from the current page."));
        result.push_back(Help("<url>", "Send gibbon to <url> rather than its known pages."));
        return result;
    }
};

class PurgeCommand : public Console::Command
{
public:
    PurgeCommand()
        : Console::Command("purge", "Purge offscreen surfaces from surface cache.", "all resources")
    {}

    virtual void invoke(const Arguments &arguments) const
    {
        unsigned purged = 0;
        if(arguments.size() > 1 && arguments[1] == "resources") {
            purged = gibbonApp()->resourceManager()->purge();
        } else {
            if(arguments.size() > 1 && arguments[1] == "all")
                purged = Widget::purgeAll(Widget::Purge_Force);
            else
                purged = Widget::purgeAll(Widget::Purge_Offscreen);
        }
        const unsigned surfacePurged = gibbonApp()->getSurfaceCache()->purge();
        Log::warn(TRACE_UI_ENGINE, "Purged: %s (%s)\n", prettyBytes(purged).c_str(),
                  prettyBytes(surfacePurged).c_str());
    }
    virtual std::vector<Help> help() const
    {
        std::vector<Help> result;
        result.push_back(Help("resources", "Purge only resource cache entries."));
        result.push_back(Help("all", "Purge all widget surfaces"));
        return result;
    }
};

class GrabCommand : public AnimationCommand
{
public:
    GrabCommand()
        : AnimationCommand("grab", "Grab existing render tree in JS and image format")
    {}

    virtual void invokeOnCurrentThread(const Arguments &arguments) const
    {
        ScopedMutex locker(Widget::lock());
        shared_ptr<WidgetBridge> widget;
        if(arguments.size() > 1) {
            if(arguments[1] == "screen") {
                //null means screen...ewwww...
            } else if(arguments[1] == "root") {
                widget = gibbonApp()->getBridge()->getRoot();
            } else if(arguments[1] == "overlay") {
                widget = gibbonApp()->getBridge()->getOverlay();
            } else {
                int id = arguments.size() <= 1 ? -1 : atoi(arguments[1].c_str());
                widget = gibbonApp()->getBridge()->getWidget(id);
                if(!widget)
                    return;
            }
        } else {
            widget = gibbonApp()->getBridge()->getWidget(-1);
        }
        if(!widget) {
            Log::error(TRACE_LOG, "Widget not found!");
            return;
        }

        static int serial = 0;
        std::string fileName = StringFormatter::sformat("/tmp/widget_%d", serial++);
        Log::warn(TRACE_UI_ENGINE, "[%s] DUMPED %s", widget ? widget->getWidget()->describe().c_str() : "screen", fileName.c_str());

        if(widget)
            dumpWidget(widget, std::string(fileName) + ".js");

        Surface::SharedPtr surface;
        if(widget) {
            shared_ptr<Widget> w = widget->getWidget();
            assert(w);
            if(DisplayList::SharedPtr grab = w->grab(Widget::Render_Comment)) {
                if(FILE *f = backupOpen(fileName + ".gfx")) {
                    int line = 0;

                    const std::list<DisplayList::Command::SharedPtr> &commands = grab->getCommands();
                    for(std::list<DisplayList::Command::SharedPtr>::const_iterator it = commands.begin(); it != commands.end(); ++it)
                        fprintf(f, "%d: %s\n", line++, (*it)->describe().c_str());
                    fclose(f);
                }
                surface = grab->flatten();
                assert(surface);
            }
        } else {
            surface = gibbonApp()->getScreen()->grab();
        }
        if(surface)
            SurfaceDecoders::instance()->encode(surface, fileName);
    }
    virtual std::vector<Help> help() const
    {
        std::vector<Help> result;
        result.push_back(Help("screen", "Grab from the backing buffer of the screen."));
        result.push_back(Help("root", "Grab from the root widget down."));
        result.push_back(Help("overlay", "Grab from the overlay widget down."));
        result.push_back(Help("<id>", "Grab widget (either widget id# or name)."));
        return result;
    }
};

class DisplayListCommand : public AnimationCommand
{
public:
    class Writer : public DisplayList::Writer, public SurfaceCache::Listener, public Screen::Listener
    {
        class SubWriter : public DisplayList::Writer
        {
        public:
            inline SubWriter(DisplayListCommand::Writer *s) : mWriter(s) { }
            virtual void writeSurface(const Surface::SharedPtr &surface) { *this << mWriter->lookupSurface(surface); }
        private:
            DisplayListCommand::Writer *mWriter;
        };
    public:
        Writer(const std::string &directory) : DisplayList::Writer(), mDirectory(directory)
        {
            Log::warn(TRACE_GIBBON_DISPLAYLIST, "DisplayList:%p: StartSave: %s", this, directory.c_str());
            mkdir(mDirectory.c_str(), 0755);
        }
        ~Writer()
        {
            Log::warn(TRACE_GIBBON_DISPLAYLIST, "DisplayList:%p: StopSave", this);
        }

        static DataBuffer generateFileName(const DataBuffer &cacheKey) {
            DataBuffer fileName = ResourceManager::prettyCacheKey(cacheKey);
            for(int i = 0; i < fileName.length(); ++i) {
                if(fileName[i] == '/' || fileName[i] == ':')
                    fileName[i] = '_';
            }
            return fileName;
        }

        DataBuffer lookupSurface(const Surface::SharedPtr &surface)
        {
            const DataBuffer cacheKey = generateFileName(surface->cacheKey());
            const std::string subDirectory = mDirectory + "/surfaces/";
            mkdir(subDirectory.c_str(), 0755);

            std::string result = StringFormatter::sformat("%lld_", nrdApp()->monoApplicationStarted().ms()) + cacheKey.toString();
            const char *suffixes[] = { ".displaylist", ".surface", 0 };
            for(int suffix = 0; suffixes[suffix]; ++suffix) {
                struct stat stat_buff;
                const std::string key = result + suffixes[suffix];
                const std::string fileName = subDirectory + key;
                if(!stat(fileName.c_str(), &stat_buff)) {
                    result = key;
                    break;
                }

                DataBuffer data;
                if(!suffix) {
                    if(DisplayList::SharedPtr dl = surface->displayList()) {
                        result = key;
                        SubWriter writer(this);
                        writer.write(dl);
                        data = writer.data();
                    }
                } else {
                    result = key;
                    std::map<Surface*, DataBuffer>::const_iterator it = mSurfaces.find(surface.get());
                    if(it == mSurfaces.end()) {
                        data = SurfaceDecoders::instance()->encode(surface);
                    } else {
                        data = it->second;
                        mSurfaces.erase(surface.get());
                    }
                }
                if(!data.empty()) {
                    if(FILE *f = fopen(fileName.c_str(), "w")) {
                        Log::info(TRACE_GIBBON_DISPLAYLIST, "  DisplayList:%p: Encoded(%s) %p", this,
                                  result.c_str(), surface.get());
                        NRDP_UNUSED_RETURN(fwrite(&data[0], data.size(), 1, f));
                        fclose(f);
                    }
                    break;
                }
            }
            return result;
        }

        virtual void writeSurface(const Surface::SharedPtr &surface)
        {
            *this << lookupSurface(surface);
        }

    protected:
        virtual void onSurfaceDecoded(const SurfaceCache::DecodeInfo &info, const Surface::SharedPtr &surface)
        {
            mSurfaces[surface.get()] = info.resource->responseData();
        }
        virtual void onSurfaceModified(const Surface::SharedPtr &surface, Modification)
        {
            mSurfaces.erase(surface.get());
        }
        virtual void onScreenRendered(const GraphicsEngine *engine, const Rect &)
        {
            if(engine->isDisplayList()) {
                write(engine->getDisplayList());
                const DataBuffer data = this->data();
                if(data.empty())
                    return;

                const std::string subDirectory = mDirectory + "/frames/";
                const std::string frame = StringFormatter::sformat("%lld_%lld.frame",
                                                                   nrdApp()->monoApplicationStarted().ms(),
                                                                   Time::mono().ms());
                const std::string fileName = subDirectory + frame;
                mkdir(subDirectory.c_str(), 0755);
                if(FILE *f = fopen(fileName.c_str(), "w")) {
                    Log::info(TRACE_GIBBON_DISPLAYLIST, "DisplayList:%p: Save %s", this, frame.c_str());
                    NRDP_UNUSED_RETURN(fwrite(data.data(), data.size(), 1, f));
                    fclose(f);
                }
            } else {
                Log::error(TRACE_UI_ENGINE, "DisplayList unexpected surface render!");
            }
        }
    private:
        std::string mDirectory;
        std::map<Surface*, DataBuffer> mSurfaces;
    };
    class Reader : public DisplayList::Reader
    {
    public:
        Reader(const std::string &directory) : DisplayList::Reader(), mDirectory(directory)
        {
        }
        ~Reader()
        {
            stop();
        }

        DisplayList::SharedPtr readFrame(const std::string &frame) {
            Stopwatch sw;
            DisplayList::SharedPtr result = readFrame_internal(mDirectory + "/frames/" + frame);
            Log::info(TRACE_GIBBON_DISPLAYLIST, "DisplayList:%p: Load(%p) %s (%fms)", this, result.get(), frame.c_str(), sw.elapsed());
            return result;
        }

        void stop() {
            if(mTimer) {
                Log::warn(TRACE_GIBBON_DISPLAYLIST, "DisplayList:%p: Stop", this);
                mTimer->stop();
                mTimer.reset();
            }
        }
        void start(const std::string &from, const std::string &to) {
            stop();
            Log::warn(TRACE_GIBBON_DISPLAYLIST, "DisplayList:%p: Start: %s - %s", this, from.c_str(), to.c_str());
            mTimer.reset(new ShowTimer(this, from, to));
            mTimer->restart(0);
            mTimer->setCurrent(from);
            readFrame(from);
        }
        void advance() {
            show();
            if(!mTimer)
                return;
            const std::string current = mTimer->current();
            if(current == mTimer->to()) {
                stop();
            } else {
                const std::string next = frameNext(current);
                if(next.empty()) {
                    stop();
                } else {
                    readFrame(next);
                    mTimer->restart(frameTime(next).ms() - frameTime(current).ms());
                    mTimer->setCurrent(next);
                }
            }
        }

        void show(int steps=-1) {
            assert(mDisplayList);
            if(steps != -1) {
                Log::warn(TRACE_GIBBON_DISPLAYLIST, "DisplayList:%p: Show(%p:%d)", this, mDisplayList.get(), steps);
                GibbonApplication::instance()->getScreen()->setRootDisplayList(mDisplayList->duplicate(steps));
            } else {
                GibbonApplication::instance()->getScreen()->setRootDisplayList(mDisplayList);
            }
        }

        void dump(const std::string &frame=std::string()) {
            if(frame.empty()) {
                if(mDisplayList) {
                    mDisplayList->dump();
                } else {
                    int i = 0;
                    const std::list<std::string> frames = this->frames();
                    for(std::list<std::string>::const_iterator it = frames.begin(); it != frames.end(); ++it)
                        Log::warn(TRACE_UI_ENGINE, "Frame[%d]: %s", i++, (*it).c_str());
                }
            } else if(frame == "current") {
                if(mDisplayList)
                    mDisplayList->dump();
            } else {
                Reader reader(mDirectory);
                if(reader.readFrame(frame))
                    reader.mDisplayList->dump();
            }
        }

        virtual void readSurface(Surface::SharedPtr &surface) {
            DataBuffer key;
            *this >> key;

            SurfaceCache::SharedPtr surfaceCache = gibbonApp()->getSurfaceCache();
            surface = surfaceCache->find(key);
            if(surface)
                return;

            struct stat stat_buff;
            const std::string subDirectory = mDirectory + "/surfaces/";
            const std::string fileName = subDirectory + key.toString();
            if(!stat(fileName.c_str(), &stat_buff)) {
                if(endsWith(fileName, ".displaylist")) {
                    Reader reader(mDirectory);
                    if(DisplayList::SharedPtr displayList = reader.readFrame_internal(fileName)) {
                        surface = GibbonApplication::instance()->getSurfaceCache()->requestSurface(displayList->getWidth(),
                                                                                                   displayList->getHeight(),
                                                                                                   Surface::Flags_Alpha|Surface::Flags_RenderTarget);
                        GibbonApplication::instance()->getSurfaceCache()->setCacheKey(surface, key);
                        GraphicsEngine engine(surface);
                        engine.apply(displayList, displayList->getRect(), displayList->getRect());
                    }
                } else {
                    SurfaceLoader::Source::SharedPtr source(new SurfaceLoader::Source);
                    source->setUrl(AseUrl("file:" + fileName));
                    source->setCacheKey(key);
                    source->setLoadFlags(SurfaceLoader::Load_Synchronous);
                    SurfaceLoader::SharedPtr loader(new SurfaceLoader(source));
                    surfaceCache->load(loader);
                    surface = surfaceCache->find(key);
                }
            }
            Log::info(TRACE_GIBBON_DISPLAYLIST, "  DisplayList:%p: Decoded(%s) %p", this, key.c_str(), surface.get());
        }

        int frameCount() const { return frames().size(); }

        std::string frame(const std::string &f) const {
            if(endsWith(f, ".frame"))
                return f;
            if(f[0] == '-')
                return frame(frameCount()+atoi(f.c_str())-1);
            return frame(atoi(f.c_str()));
        }

        std::string frame(int i) const {
            const std::list<std::string> frames = this->frames();
            for(std::list<std::string>::const_iterator it = frames.begin(); it != frames.end(); ++it) {
                if(!i--)
                    return (*it);
            }
            return std::string();
        }

        std::string frameNext(const std::string &frame) {
            const std::list<std::string> frames = this->frames();
            for(std::list<std::string>::const_iterator it = frames.begin(); it != frames.end(); ++it) {
                if((*it) == frame) {
                    ++it;
                    if(it != frames.end())
                        return (*it);
                    break;
                }
            }
            return std::string();
        }

        Time frameTime(const std::string &frame) {
            llong ms;
            if(sscanf(frame.c_str(), "%*d_%lld.frame", &ms) == 1)
                return Time::fromMS(ms);
            return Time();
        }

    private:

        std::list<std::string> frames() const {
            std::list<std::string> result;
            const std::string subDirectory = mDirectory + "/frames/";
            const std::vector<ReadDir::Entry> entries = ReadDir::entries(subDirectory);
            for(std::vector<ReadDir::Entry>::const_iterator it = entries.begin(); it != entries.end(); ++it) {
                const char *fn = it->fileName();
                if(fn[0] != '.')
                    result.push_back(fn);
            }
            result.sort();
            return result;
        }

        DisplayList::SharedPtr readFrame_internal(const std::string &fileName) {
            mDisplayList.reset();
            if(FILE *f = fopen(fileName.c_str(), "r")) {
                DataBuffer buffer;
                {
                    char tmpbuf[8192];
                    size_t r;
                    while(!feof(f)) {
                        r = fread(tmpbuf, 1, sizeof(tmpbuf), f);
                        if(r > 0)
                            buffer.append(tmpbuf, r);
                    }
                }
                mDisplayList = read(buffer);
                fclose(f);
            }
            clear();
            return mDisplayList;
        }

        class ShowTimer : public GibbonApplication::Timer
        {
        public:
            ShowTimer(Reader *reader, const std::string &from, const std::string &to) : GibbonApplication::Timer(0),
                                                                                                    mReader(reader),
                                                                                                    mFrom(from), mTo(to) { }
            ~ShowTimer() { }
            void timerFired() { mReader->advance(); }

            virtual std::string describe() const { return "ShowTimer"; }

#if !defined(GIBBON_NO_THREADANIMATION)
            virtual bool start() { return Animation::startTimer(shared_from_this()); }
            virtual void stop() { Animation::stopTimer(shared_from_this()); }
#endif

            inline void setCurrent(const std::string &c) { mCurrent = c; }
            inline std::string current() const { return mCurrent; }

            inline std::string from() const { return mFrom; }
            inline std::string to() const { return mTo; }

        private:
            Reader *mReader;
            const std::string mFrom, mTo;
            std::string mCurrent;
        };

        std::string mDirectory;
        shared_ptr<ShowTimer> mTimer;
        DisplayList::SharedPtr mDisplayList;
    };

    DisplayListCommand() : AnimationCommand("displaylist", "Operate on DisplayLists", "load step save frame play dump close") { }
    ~DisplayListCommand() { closeAll(); }

    virtual void invokeOnCurrentThread(const Arguments &arguments) const
    {
        if(arguments.size() == 1) {
            if(mReader)
                mReader->dump();
            else
                Log::error(TRACE_UI_ENGINE, "Need a command!");
        } else if(arguments[1] == "save") {
            closeAll();
            if(arguments.size() > 2) {
                if(GibbonConfiguration::screenRenderDisplayList()) {
                    mWriter.reset(new Writer(arguments[2]));
                    gibbonApp()->getScreen()->addListener(mWriter);
                    gibbonApp()->getSurfaceCache()->addListener(mWriter);
                } else {
                    Log::warn(TRACE_UI_ENGINE, "DisplayList save mode not supported!");
                }
            } else {
                Log::warn(TRACE_UI_ENGINE, "DisplayList save mode not active!");
            }
        } else if(arguments[1] == "load") {
            closeAll();
            if(arguments.size() > 2) {
                mReader.reset(new Reader(arguments[2]));
                if(arguments.size() > 3)
                    mReader->readFrame(mReader->frame(arguments[3]));
            }
        } else if(arguments[1] == "close") {
            closeAll();
        } else if(arguments[1] == "dump") {
            if(mReader) {
                std::string frame;
                if(arguments.size() > 2)
                    frame = mReader->frame(arguments[2]);
                mReader->dump(frame);
            } else {
                Log::warn(TRACE_UI_ENGINE, "No DisplayList loaded!");
            }
        } else if(arguments[1] == "frame") {
            if(mReader && arguments.size() > 2) {
                mReader->readFrame(mReader->frame(arguments[2]));
                mReader->show();
            } else {
                Log::warn(TRACE_UI_ENGINE, "No DisplayList loaded!");
            }
        } else if(arguments[1] == "step") {
            if(mReader)
                mReader->show(arguments.size() > 2 ? atoi(arguments[2].c_str()) : -1);
            else
                Log::warn(TRACE_UI_ENGINE, "No DisplayList loaded!");
        } else if(arguments[1] == "play") {
            if(mReader) {
                std::string from, to;
                if(arguments.size() == 2) {
                    from = mReader->frame(0);
                    to = mReader->frame(mReader->frameCount()-1);
                } else {
                    if(arguments.size() > 2)
                        from = mReader->frame(arguments[2]);
                    if(arguments.size() > 3)
                        to = mReader->frame(arguments[3]);
                }
                if(from.empty())
                    std::swap(from, to);
                if(to.empty()) {
                    mReader->readFrame(from);
                    mReader->show();
                } else if(!from.empty()) {
                    mReader->start(from, to);
                }
            } else {
                Log::warn(TRACE_UI_ENGINE, "No displaylist loaded!");
            }
        }
    }
    virtual std::vector<Help> help() const
    {
        std::vector<Help> result;
        result.push_back(Help("close", "Close any open DisplayLists."));
        result.push_back(Help("save", "<directory>", "Store DisplayLists into <directory>."));
        result.push_back(Help("load", "<directory>", "Load DisplayLists from directory <directory>."));
        result.push_back(Help("frame", "<file>", "Load a frame from DisplayLists."));
        {
            Help displayListPlay("play", "Playback a loaded DisplayList.");
            displayListPlay << Help("<from>", "Start playback at <from>.");
            displayListPlay << Help("<to>", "End playback at <to>.");
            result.push_back(displayListPlay);
        }
        result.push_back(Help("step", "<steps>", "Replay a <steps> from loaded DisplayLists."));
        result.push_back(Help("dump", "Display information from a loaded DisplayLists."));
        return result;
    }

private:
    void closeAll() const {
        if(mReader) {
            mReader.reset();
            GibbonApplication::instance()->getScreen()->setRootDisplayList(DisplayList::SharedPtr());
        }
        if(mWriter) {
            gibbonApp()->getScreen()->removeListener(mWriter);
            gibbonApp()->getSurfaceCache()->removeListener(mWriter);
            mWriter.reset();
        }
    }

    mutable shared_ptr<Writer> mWriter;
    mutable shared_ptr<Reader> mReader;
};

class SizeContentCommand : public Console::Command
{
public:
    SizeContentCommand()
        : Console::Command("sizecontent", "Calculate size content for a widget")
    {}

    virtual void invoke(const Arguments &arguments) const
    {
        ScopedMutex locker(Widget::lock());
        Size maxSize;
        Widget::SharedPtr widget = gibbonApp()->getScreen()->root();
        for(size_t i = 1; i < arguments.size(); ++i) {
#ifdef GIBBON_COORDINATE_FLOAT
            const char *sizeFormat = "%fx%f";
#else
            const char *sizeFormat = "%dx%d";
#endif
            Size size;
            if(sscanf(arguments[i].c_str(), sizeFormat, &size.width, &size.height) == 2)
                maxSize = size;
            else
                widget = gibbonApp()->findWidget(DumpInfo(arguments[1]));
        }
        if(!maxSize.width)
            maxSize.width = CoordinateMax;
        if(!maxSize.height)
            maxSize.height = CoordinateMax;
        if(widget) {
            const Size size = widget->calculateSizeContent(maxSize.width, maxSize.height);
            Log::warn(TRACE_UI_ENGINE) << "Size: " << size;
        } else {
            Log::warn(TRACE_UI_ENGINE, "No views!\n");
        }
    }
    virtual std::vector<Help> help() const
    {
        std::vector<Help> result;
        result.push_back(Help("screen", "Grab from the backing buffer of the screen."));
        result.push_back(Help("<id>", "Grab widget (either widget id# or name)."));
        return result;
    }
};

class UpdateCommand : public Console::Command
{
public:
    UpdateCommand()
        : Console::Command("update", "Cause a re-render of the scene", "images")
    {}

    virtual void invoke(const Arguments &arguments) const
    {
        ScopedMutex locker(Widget::lock());
        bool reload = false;
        Widget::SharedPtr widget = gibbonApp()->getScreen()->root();
        for(size_t i = 1; i < arguments.size(); ++i) {
            if(arguments[i] == "images")
                reload = true;
            else if(Widget::SharedPtr w = gibbonApp()->findWidget(DumpInfo(arguments[i])))
                widget = w;
        }
        if(widget) {
            if(reload)
                widget->reload();
            widget->needsRender();
        }
    }
    virtual std::vector<Help> help() const
    {
        std::vector<Help> result;
        result.push_back(Help("images", "Force an image reload before rendering."));
        result.push_back(Help("<id>", "Force update for widget (either widget id# or name)."));
        return result;
    }
};

class ViewsCommand : public AnimationCommand
{
public:
    ViewsCommand()
        : AnimationCommand("views", "Display information about <widget>", "check onscreen noorder uncached content garbage")
    {}

    virtual void invokeOnCurrentThread(const Arguments &arguments) const
    {
        ScopedMutex locker(Widget::lock());
        int depth = -1;
        unsigned char flags = 0;
        enum { Check_None, Check_Offscreen, Check_Garbage } check = Check_None;
        Widget::SharedPtr root = gibbonApp()->getScreen()->root();
        Widget::SharedList widgets;
        for(size_t i = 1; i < arguments.size(); ++i) {
            if(arguments[i] == "all") {
                flags = 0;
            } else if(arguments[i] == "check") {
                check = Check_Offscreen;
            } else if(arguments[i] == "garbage") {
                check = Check_Garbage;
            } else if(arguments[i] == "onscreen") {
                flags |= GibbonConsole::Dump_NoOffscreen;
            } else if(arguments[i] == "uncached") {
                flags |= GibbonConsole::Dump_NoCached;
            } else if(arguments[i] == "noorder") {
                flags |= GibbonConsole::Dump_NoViewOrder;
            } else if(arguments[i] == "content") {
                flags |= GibbonConsole::Dump_NoEmpty;
            } else if(arguments[i] == "mouse") {
                Widget::SharedList hit = root->widgetsAt(MouseEvent::getLastPosition());
                if(!hit.empty())
                    widgets.insert(widgets.end(), hit.begin(), hit.end());
            } else if(arguments[i][0] == '-') {
                depth = -(atoi(arguments[i].c_str())-1);
            } else if(root) {
                if(Widget::SharedPtr w = gibbonApp()->findWidget(DumpInfo(arguments[i]))) {
                    widgets.push_back(w);
                } else {
                    Point point;
#ifdef GIBBON_COORDINATE_FLOAT
                    const char *pointFormat = "%f,%f";
#else
                    const char *pointFormat = "%d,%d";
#endif
                    if(sscanf(arguments[i].c_str(), pointFormat, &point.x, &point.y) == 2) {
                        Widget::SharedList hit = root->widgetsAt(point);
                        if(!hit.empty())
                            widgets.insert(widgets.end(), hit.begin(), hit.end());
                    }
                }
            }
        }
        if(widgets.empty() && root)
            widgets.push_back(root);
        if(widgets.empty()) {
            Log::warn(TRACE_UI_ENGINE, "No views!\n");
        } else {
            for(Widget::SharedList::const_iterator it = widgets.begin(); it != widgets.end(); ++it) {
                Widget::SharedPtr widget = (*it);
                if(check == Check_Offscreen) {
                    checkViews(widget, !(flags & GibbonConsole::Dump_NoOffscreen));
                } else if(check == Check_Garbage) {
                    std::set<Widget::SharedPtr> garbage;
                    const std::list<Widget::SharedPtr> widgets = gibbonApp()->getWidgets();
                    for(std::list<Widget::SharedPtr>::const_iterator it = widgets.begin(); it != widgets.end(); ++it) {
                        Widget::SharedPtr root;
                        for(Widget::SharedPtr widget = (*it); widget; ) {
                            if(widget->isRoot())
                                break;
                            Widget::SharedPtr parent = widget->getParent();
                            if(!parent) {
                                garbage.insert(widget);
                                break;
                            }
                            widget = parent;
                        }
                    }
                    int garbageCount = 0;
                    for(std::set<Widget::SharedPtr>::const_iterator git = garbage.begin(); git != garbage.end(); ++git)
                        garbageCount += dumpViews((*git), flags, 0, depth);
                    Log::warn(TRACE_UI_ENGINE, "Garbage: %d", garbageCount);
                } else {
                    Log::warn(TRACE_UI_ENGINE, "Views: %d", dumpViews(widget, flags, 0, depth));
                }
            }
        }
    }
    virtual std::vector<Help> help() const
    {
        std::vector<Help> result;
        result.push_back(Help("<id>", "Display engine widget information (either widget id# or name)."));
        result.push_back(Help("all", "Display all widgets in tree."));
        result.push_back(Help("garbage", "Only dump views that are not in the scene."));
        result.push_back(Help("mouse", "Only dump views that are under the mouse pointer."));
        result.push_back(Help("content", "Only dump views that have some content."));
        result.push_back(Help("onscreen", "Only dump views that are currently rendering on the screen."));
        result.push_back(Help("uncached", "Only dump views that are not currently cached to surface."));
        result.push_back(Help("noorder", "Dump views in tree order (rather than view order)."));
        return result;
    }
};

class SurfaceCommand : public Console::Command
{
public:
    SurfaceCommand()
        : Console::Command("surface", "Display surface information", "pending decoders cache touch atlas")
    {}

    virtual void invoke(const Arguments &arguments) const
    {
        DumpInfo dump;
        enum { Surface_Cache, Surface_Pending, Surface_Touch, Surface_Decoders, Surface_Atlas, Surface_None } mode = Surface_Cache;
        for(size_t i = 1; i < arguments.size(); ++i) {
            if(arguments[i] == "pending") {
                mode = Surface_Pending;
            } else if(arguments[i] == "touch") {
                mode = Surface_Touch;
            } else if(arguments[i] == "decoders") {
                mode = Surface_Decoders;
            } else if(arguments[i] == "atlas") {
                mode = Surface_Atlas;
            } else if(arguments[i] == "cache") {
                if(i + 1 < arguments.size()) {
                    if(Widget::SharedPtr w = gibbonApp()->findWidget(DumpInfo(arguments[++i]))) {
                        if(Surface::SharedPtr surface = w->getCachedSurface())
                            dump.ptr = surface.get();
                        else
                            mode = Surface_None;
                    }
                }
            } else if(arguments[i] == "effect") {
                if(i + 1 < arguments.size()) {
                    if(Widget::SharedPtr w = gibbonApp()->findWidget(DumpInfo(arguments[++i]))) {
                        if(DisplayList::SharedPtr dl = w->getEffectDisplayList()) {
                            dl->dump();
                            if(Surface::SharedPtr surface = dl->getSurface())
                                dump.ptr = surface.get();
                            else
                                mode = Surface_None;
                        }
                    }
                }
            } else {
                dump.set(arguments[i]);
            }
        }
        if(mode == Surface_Touch)
            gibbonApp()->getSurfaceCache()->dumpTouch(dump);
        else if(mode == Surface_Pending)
            gibbonApp()->getSurfaceCache()->dumpPending(dump);
        else if(mode == Surface_Decoders)
            SurfaceDecoders::instance()->dump();
        else if(mode == Surface_Cache)
            gibbonApp()->getSurfaceCache()->dump(dump);
        else if(mode == Surface_Atlas)
            gibbonApp()->getSurfaceCache()->dumpAtlas(dump);
    }
    virtual std::vector<Help> help() const
    {
        std::vector<Help> result;
        result.push_back(Help("<id>", "Dump only information about surface matching <id>."));
        result.push_back(Help("cache", "<id>", "Dump only information about surface cache for widget <id>."));
        result.push_back(Help("effect", "<id>", "Dump only information about effect surface for widget <id>."));
        result.push_back(Help("pending", "Dump information about surfaces currently pending download/decode."));
        result.push_back(Help("touch", "Dump touch information about surfaces."));
        result.push_back(Help("decoders", "Dump currently decoding surfaces."));
        result.push_back(Help("atlas", "Dump surface atlas information."));
        return result;
    }
};

class CacheCommand : public Console::Command
{
public:
    CacheCommand()
        : Console::Command("cache", "Display cached data.",
                           Variant::StringMap("disk", "raw hex data")("memory", "raw hex data"))
    {}

    virtual void invoke(const Arguments &arguments) const
    {
        bool disk = false;
        DumpInfo dump;
        for(size_t i = 1; i < arguments.size(); ++i) {
            if(arguments[i] == "disk")
                disk = true;
            else if(arguments[i] == "memory")
                disk = false;
            else if(arguments[i] == "raw")
                dump.flags |= DumpInfo::Raw;
            else if(arguments[i] == "hex")
                dump.flags &= ~DumpInfo::Raw;
            else if(arguments[i] == "data")
                dump.flags = DumpInfo::Data;
            else
                dump.set(arguments[i]);
        }
        if(disk) {
            if(DiskCache::SharedPtr diskCache = gibbonApp()->resourceManager()->diskCache()) {
                diskCache->dump(dump);
            }
        } else {
            gibbonApp()->resourceManager()->dump(dump);
        }
    }
    virtual std::vector<Help> help() const
    {
        std::vector<Help> result;
        result.push_back(Help("<id>", "Dump only information about entry matching <id>."));
        result.push_back(Help("disk", "Display information about disk cache."));
        result.push_back(Help("memory", "Display information about resource cache."));
        result.push_back(Help("raw", "Display information in raw format."));
        result.push_back(Help("hex", "Display information in hex format."));
        result.push_back(Help("headers", "Display header information."));
        return result;
    }
};

class FontsCommand : public Console::Command
{
public:
    FontsCommand()
        : Console::Command("fonts", "Display font information",
                           Variant::StringMap("linking", "help dump")("cache")("dump")("verbose"))
    {}

    virtual void invoke(const Arguments &arguments) const
    {
        if(arguments.size() > 1) {
            if(arguments[1] == "verbose") {
                FontManager::dumpFontStatistics(true);
            }
            else if(arguments[1] == "linking") {
                if(arguments.size() > 2) {
                    if(arguments [2] == "dump") {
                        FontManager::dumpFontLinkingTree();
                    } else {
                        std::istringstream iss(arguments[2]);
                        unsigned long result = 0;
                        if(iss >> std::hex >> result) {
                            if(arguments.size() > 4) {
                                Font::Weight w = Font::WEIGHT_NORMAL;
                                Font::Style s = Font::STYLE_NORMAL;
                                std::string locale;

                                if(arguments[3] == "bold" || arguments[3] == "b") {
                                    w = Font::WEIGHT_BOLD;
                                }
                                if(arguments[4] == "italic" || arguments[4] == "i") {
                                    s = Font::STYLE_ITALIC;
                                }

                                if(arguments.size() > 5) {
                                    locale = arguments[5];
                                }

                                FontManager::dumpFontLinkingTree(result, w, s, locale);

                            } else {
                                FontManager::dumpFontLinkingTree(result);
                            }
                        } else {
                            Log::warn(TRACE_UI_ENGINE, "Invalid argument");
                        }
                    }
                }
            } else if(arguments[1] == "cache") {
                GibbonFreetype::dumpCacheInformation();
            } else if(arguments[1] == "dump") {
                GibbonFreetype::saveCacheSurface();
            }
        } else {
            FontManager::dumpFontStatistics();
        }
    }
    virtual std::vector<Help> help() const
    {
        std::vector<Help> result;
        result.push_back(Help("verbose", "Dump all font statistics"));
        result.push_back(Help("linking", "Dump font linking information.") << Help("bold", "Dump only bold fonts") << Help("italic", "Dump only italic fonts"));
        result.push_back(Help("cache", "Dump font cache information."));
        result.push_back(Help("dump", "Save glyph cache"));
        return result;
    }
};

class SetCommand : public Console::Command
{
public:
    SetCommand()
        : Console::Command("set", "Set internal state information", setCompletions())
    {}

    virtual void invoke(const Arguments &arguments) const
    {
        bool showHelp = false;
        if(arguments.size() < 2) {
            showHelp = true;
        } else if(arguments[1] == "smoothScale") {
            if(arguments.size() == 3)
                GraphicsEngine::setDefaultSmoothScale(arguments[2] == "true");
            Log::warn(TRACE_UI_ENGINE, "SmoothScale: %d", GraphicsEngine::defaultSmoothScale());
        } else if(arguments[1] == "fps") {
            if(arguments.size() == 3) {
                if(arguments[2] == "timer") {
                    if(gibbonApp()->runningFpsTimer())
                        gibbonApp()->stopFpsTimer();
                    else
                        gibbonApp()->startFpsTimer();
                } else {
                    gibbonApp()->getScreen()->setTargetFPS(atoi(arguments[2].c_str()));
                }
            }
            Log::warn(TRACE_UI_ENGINE, "FPS timer: %s", gibbonApp()->runningFpsTimer() ? "on" : "off");
            Log::warn(TRACE_UI_ENGINE, "FPS target: %d", gibbonApp()->getScreen()->getTargetFPS());
        } else if(arguments[1] == "log") {
            if(arguments.size() < 3) {
                Log::warn(TRACE_UI_ENGINE, "Traceareas:");
                const std::vector<const TraceArea*> &allAreas = TraceArea::getAllAreas();
                shared_ptr<TraceAreas> traceAreas = app()->traceAreas();
                for(std::vector<const TraceArea*>::const_iterator it = allAreas.begin(); it != allAreas.end(); ++it) {
                    if((*it))
                        Log::warn(TRACE_UI_ENGINE, "%c %s", traceAreas->isEnabled(*it) ? '*' : ' ', (*it)->getName().c_str());
                }
                Log::warn(TRACE_UI_ENGINE, "Tracearea Groups:\n");
                const std::vector<std::string> &allGroups = TraceArea::getAllGroups();
                for(std::vector<std::string>::const_iterator git = allGroups.begin(); git != allGroups.end(); ++git) {
                    std::ostringstream stream;
                    const std::vector<const TraceArea*> &allAreas = TraceArea::getAllAreas(*git);
                    if(allAreas.empty())
                        continue;
                    stream << "  " << *git << ": ";
                    for(std::vector<const TraceArea*>::const_iterator it = allAreas.begin(); it != allAreas.end(); ++it) {
                        if(it != allAreas.begin())
                            stream << ",";
                        if((*it))
                            stream << (*it)->getName();
                    }
                    Log::warn(TRACE_UI_ENGINE, "%s", stream.str().c_str());
                }
            } else {
                app()->traceAreas()->parse(arguments[2]);
            }
        } else if(arguments[1] == "focus") {
            int focus = -1;
            if(arguments[2] == "on")
                focus = 1;
            else if(arguments[2] == "off")
                focus = 0;
            if(focus != -1) {
                gibbonApp()->sendEvent(new FocusChangeEvent(focus));
                Log::warn(TRACE_UI_ENGINE, "FocusChanged: %d", focus);
            }
        } else if(arguments[1] == "mouse") {
            shared_ptr<InputManager> input = gibbonApp()->getInputManager();
            if(arguments.size() >= 4) {
                const Point pos(atoi(arguments[2].c_str()), atoi(arguments[3].c_str()));
                input->moveMouse(pos);
                Log::warn(TRACE_UI_ENGINE) << "MouseMoved: " << pos.x << " " << pos.y;
            } else if(arguments.size() >= 3) {
                int visible = -1;
                if(arguments[2] == "visible")
                    visible = 1;
                else if(arguments[2] == "invisible")
                    visible = 0;
                if(visible != -1) {
                    gibbonApp()->sendEvent(new MouseChangeEvent(visible));
                    Log::warn(TRACE_UI_ENGINE, "MouseChanged: %d", visible);
                }
            }
            Log::warn(TRACE_UI_ENGINE, "Position: %s", MouseEvent::getLastPosition().toString().c_str());
        } else if(arguments[1] == "input") {
            shared_ptr<InputManager> input = gibbonApp()->getInputManager();
#ifdef GIBBON_INPUT_DEVINPUT
            if(arguments.size() >= 3 && arguments[2] == "rescan") {
                input->rescan();
                input->wakeup();
            } else {
#endif
                if(arguments.size() >= 3)
                    input->setHoldStyle(arguments[2]);
                Log::warn(TRACE_UI_ENGINE, "Style: %s", InputManager::describeHoldStyle(input->getHoldStyle()).c_str());
#ifdef GIBBON_INPUT_DEVINPUT
            }
#endif
        } else if(arguments[1] == "screen") {
            Screen::SharedPtr screen = gibbonApp()->getScreen();
            if(arguments.size() >= 3) {
                if(arguments[2] == "alpha" && arguments.size() > 3) {
                    screen->setFormat(Screen::SurfaceFormat_Alpha, arguments[3]);
                } else if(arguments[2] == "sleep" && arguments.size() > 3) {
                    screen->setFlipSleep(Configuration::parseInt(arguments[3].c_str(), ConfigurationOption::TIME_ARGUMENT));
                } else if(arguments[2] == "opaque" && arguments.size() > 3) {
                    screen->setFormat(Screen::SurfaceFormat_Opaque, arguments[3]);
                } else if(arguments[2] == "endian" && arguments.size() > 3) {
                    screen->setEndian(arguments[3]);
#if defined(GIBBON_GRAPHICSENGINE_DIRECTFB)
                } else if(arguments[2] == "dfb" && arguments.size() > 3) {
                    Screen::setConfiguration(Screen::Configuration(atoi(arguments[3].c_str()), arguments.size() > 4 ? (arguments[4] == "true") : true));
#elif defined(GIBBON_GRAPHICSENGINE_GLES2)
                } else if(arguments[2] == "gl" && arguments.size() > 3) {
                    Screen::setConfiguration(Screen::Configuration(atoi(arguments[3].c_str())));
#endif
                } else {
                    Log::warn(TRACE_UI_ENGINE, "Cannot find format type: %s", arguments[2].c_str());
                }
            }
            Log::warn(TRACE_UI_ENGINE, "Suspend: %s", gibbonApp()->isSuspended() ? "true" : "false");
            Log::warn(TRACE_UI_ENGINE, "Size: %s (%s)", screen->getSize().toString().c_str(), screen->getUISize().toString().c_str());
            Log::warn(TRACE_UI_ENGINE, "Alpha: %s", Screen::describeFormat(screen->getFormat(Screen::SurfaceFormat_Alpha)).c_str());
            Log::warn(TRACE_UI_ENGINE, "Opaque: %s", Screen::describeFormat(screen->getFormat(Screen::SurfaceFormat_Opaque)).c_str());
            Log::warn(TRACE_UI_ENGINE, "Text: %s", Screen::describeFormat(screen->getFormat(Screen::SurfaceFormat_Text)).c_str());
            if(int ms = screen->flipSleep())
                Log::warn(TRACE_UI_ENGINE, "Sleep: %d", ms);
            Log::warn(TRACE_UI_ENGINE, "Memory: %s", prettyBytes(screen->getGraphicsMemoryCapacity()).c_str());
            Log::warn(TRACE_UI_ENGINE, "Endian: %s", Screen::describeEndian(screen->getEndian()).c_str());
#if defined(GIBBON_GRAPHICSENGINE_DIRECTFB)
            Log::warn(TRACE_UI_ENGINE, "DFB: %d%s", Screen::getConfiguration().layer, Screen::getConfiguration().window ? " (window)" : "");
#elif defined(GIBBON_GRAPHICSENGINE_GLES2)
            Log::warn(TRACE_UI_ENGINE, "GL: %d samples", Screen::getConfiguration().targetSamples);
#endif
#if defined(GIBBON_SCREEN_FLIPTHREAD)
            Log::warn(TRACE_UI_ENGINE, "Flip-Thread!");
#endif
#if defined(GIBBON_SCREEN_FLIPVSYNC)
            Log::warn(TRACE_UI_ENGINE, "VSYNC!");
#endif
#if defined(GIBBON_SCREEN_FULLRENDER)
            Log::warn(TRACE_UI_ENGINE, "Full-Screen!");
#endif
        } else if(arguments[1] == "debug") {
            if(arguments.size() < 3) {
                Debug::setProperty(std::string());
            } else {
                for(size_t i = 2; i < arguments.size(); ++i)
                    Debug::setProperty(arguments[i]);
            }
        } else if(arguments[1] == "thread") {
            if(arguments.size() < 4)
                Log::warn(TRACE_UI_ENGINE, "/%s thread <name> <priority>", arguments[0].c_str());
            else if(ThreadConfig *thread = ThreadConfig::find(arguments[2]))
                thread->SetPriority(atoi(arguments[3].c_str()));
            else
                Log::warn(TRACE_UI_ENGINE, "Cannot find thread: %s", arguments[2].c_str());
        } else if(arguments[1] == "cache") {
            if(arguments.size() < 3) {
                Log::warn(TRACE_UI_ENGINE, "/%s cache <memory|disk|surface> <capacity>", arguments[0].c_str());
            } else if(arguments[2] == "memory") {
                if(arguments.size() > 3) {
                    if(arguments[3] == "clear") {
                        gibbonApp()->resourceManager()->clear();
                    } else {
                        gibbonApp()->resourceManager()->setCapacity(Configuration::parseInt(arguments[3].c_str(), ConfigurationOption::SIZE_ARGUMENT));
                    }
                }
                Log::warn(TRACE_UI_ENGINE, "Cache: %s: %s", arguments[2].c_str(),
                          prettyBytes(gibbonApp()->resourceManager()->capacity()).c_str());
            } else if(arguments[2] == "disk") {
                if(arguments.size() > 3) {
                    if(arguments[3] == "clear") {
                        if(DiskCache::SharedPtr cache = gibbonApp()->resourceManager()->diskCache())
                            cache->clear();
                    } else if(arguments[3] == "clearStats") {
                        if(DiskCache::SharedPtr cache = gibbonApp()->resourceManager()->diskCache())
                            cache->clearStats();
                    } else if(arguments[3] == "flush") {
                        if(DiskCache::SharedPtr cache = gibbonApp()->resourceManager()->diskCache())
                            cache->flush();
                    } else {
                        nrdApp()->initDiskCache(arguments[3].c_str());
                        shared_ptr<GibbonBridge> gibbon = gibbonApp()->getBridge();
                        const int index = gibbon->propertyIndex("diskCacheConfiguration");
                        if(index != -1)
                            gibbon->propertiesUpdated(&index, 1);
                    }
                }
                if(DiskCache::SharedPtr cache = gibbonApp()->resourceManager()->diskCache()) {
                    Log::warn(TRACE_UI_ENGINE, "Cache: %s: %s", arguments[2].c_str(),
                              prettyBytes(cache->capacity()).c_str());
                } else {
                    Log::warn(TRACE_UI_ENGINE, "Cache: %s: Disabled", arguments[2].c_str());
                }
            } else if(arguments[2] == "surface") {
                if(arguments.size() > 3) {
                    if(arguments[3] == "clear") {
                        gibbonApp()->getSurfaceCache()->clear();
                    } else {
                        gibbonApp()->getSurfaceCache()->setCapacity(Configuration::parseInt(arguments[3].c_str(), ConfigurationOption::SIZE_ARGUMENT));
                    }
                }
                Log::warn(TRACE_UI_ENGINE, "Cache: %s: %s", arguments[2].c_str(),
                          prettyBytes(gibbonApp()->getSurfaceCache()->capacity()).c_str());
            }
        } else if(arguments[1] == "jsMaxLength") {
            shared_ptr<GibbonConsole> console = gibbonConsole();
            if(arguments.size() > 2) {
                const int len = atoi(arguments[2].c_str());
                console->setJSMaxLength(len);
            }
            Log::warn(TRACE_LOG, "JS Max length is %d", console->jsMaxLength());
        } else {
            showHelp = true;
        }
        if(showHelp)
            Log::warn(TRACE_UI_ENGINE, "/%s <cache|thread|debug|screen>", arguments[0].c_str());
    }
    virtual std::vector<Help> help() const
    {
        std::vector<Help> result;

        Help cache("cache", "Set information into caches.");
        {
            Help surfaceCache("surface", "Surface Cache");
            surfaceCache << Help("clear", "Clear cache contents.");
            surfaceCache << Help("<size>", "Set capacity to <size>.");
            cache << surfaceCache;
        }
        {
            Help memoryCache("memory", "Resource Cache");
            memoryCache << Help("clear", "Clear cache contents.");
            memoryCache << Help("<size>", "Set capacity to <size>.");
            cache << memoryCache;
        }
        {
            Help diskCache("disk", "Disk Cache");
            diskCache << Help("clear", "Clear cache contents.");
            diskCache << Help("clearStats", "Clear accumlated cache statistics.");
            diskCache << Help("flush", "Flush pending writes to disk.");
            diskCache << Help("<size>", "Set capacity to <size>.");
            cache << diskCache;
        }
        result.push_back(cache);

        {
            Help focus("focus", "Trigger focus change");
            focus << Help("on", "Gain focus.");
            focus << Help("off", "Lose focus.");
            result.push_back(focus);
        }
        {
            Help mouse("mouse", "Trigger mouse change");
            mouse << Help("visible", "Show mouse.");
            mouse << Help("invisible", "Hide mouse.");
            result.push_back(mouse);
        }
        {
            Help debug("debug", "Debug information");
            Help debugKey("<key>", "[value]", "Set <key> debugging state to [value].");
            const std::vector<Console::Command::Help> debugHelp = Debug::help();
            for(std::vector<Console::Command::Help>::const_iterator it = debugHelp.begin(); it != debugHelp.end(); ++it)
                debugKey << (*it);
            debug << debugKey;
            result.push_back(debug);
        }

        Help screen("screen", "Set information into graphics subsystem.");
        screen << Help("alpha", "<format>", "Set the alpha blended format to <format>.");
        screen << Help("opaque", "<format>", "Set the opaque blended format to <format>.");
        screen << Help("endian", "<format>", "Set the GPU endianness to <format>.");
        screen << Help("sleep", "<ms>", "Force flip to sleep <ms> between renders.");
#if defined(GIBBON_GRAPHICSENGINE_DIRECTFB)
        {
            Help dfbScreen("dfb", "Set directfb specific information.");
            dfbScreen << Help("<layer>", "Force directfb to render into <layer>.");
            dfbScreen << Help("<windowed>", "Toggle whether directfb is in windowed mode.");
            screen << dfbScreen;
        }
#elif defined(GIBBON_GRAPHICSENGINE_GLES2)
        {
            Help glScreen("gl", "Set GLES2 specific information.");
            glScreen << Help("<sample>", "Force GLES2 sample buffer size.");
        }
#endif
        result.push_back(screen);

        result.push_back(Help("fps", "<target>", "Set fps target to <target>."));
        result.push_back(Help("smoothScale", "[value]", "Set default smoothscale to <value>."));
        result.push_back(Help("jsMaxLength", "<size>", "Set the maximum JS length output to <size>."));

        {
            Help log("log", "<areas>", "Set console logging to -T <areas>.");
            const std::vector<const TraceArea*> areas = TraceArea::getAllAreas();
            for(std::vector<const TraceArea*>::const_iterator it = areas.begin(); it != areas.end(); ++it)
                log << Help((*it)->getName(), (*it)->getGroups().empty() ? "" : "Groups: " + StringTokenizer::join((*it)->getGroups(), ", "));
            result.push_back(log);
        }

        result.push_back(Help("thread", "Set thread configuration information.") << Help("<name>", "Lookup thread config named <name>.")
                         << Help("<priority>", "Set thread config <name> to <priority>."));

        return result;
    }

};

class OverlayCommand : public Console::Command
{
public:
    OverlayCommand()
        : Console::Command("overlay", "Draw an overlay at <rect>", "clear")
    {}

    virtual void invoke(const Arguments &arguments) const
    {
        ScopedMutex locker(Widget::lock());
        if(arguments.size() == 1) {
            if(Widget::SharedPtr overlay = gibbonApp()->getScreen()->overlay())
                dumpViews(overlay, 0, 0, -1);
        } else if(arguments.size() == 2 && arguments[1] == "ruler") {
            Widget::SharedPtr ruler(new Widget);
            ruler->setRect(Rect(0, 0, 1280, 720));
            ruler->setName("Ruler");
            ruler->setOpacity(0.75);
            {
                Image::SharedPtr image(new Image(-1, ruler, Image::Type_Foreground));
                image->load("http://localcontrol.netflix.com/img/ruler.png");
                ruler->addImage(image);
            }
            gibbonApp()->getScreen()->addOverlay(ruler);
        } else if(arguments.size() == 2 && arguments[1] == "clear") {
            gibbonApp()->getScreen()->setOverlay(Widget::SharedPtr());
        } else {
            Rect rect;
            Widget::SharedPtr widget;
            std::vector<std::string> dimensions;
            if(arguments.size() == 2) {
                if(!contains(arguments[1], ",") && !contains(arguments[1], "x"))
                    widget = gibbonApp()->findWidget(DumpInfo(arguments[1]));
                if(!widget)
                    dimensions.push_back(arguments[1]);
            } else if(arguments.size() == 3) {
                widget = gibbonApp()->findWidget(DumpInfo(arguments[1]));
                dimensions.push_back(arguments[2]);
            } else if(arguments.size() == 5) {
                dimensions.push_back(arguments[1]);
                dimensions.push_back(arguments[2]);
                dimensions.push_back(arguments[3]);
                dimensions.push_back(arguments[4]);
            } else if(arguments.size() == 6) {
                widget = gibbonApp()->findWidget(DumpInfo(arguments[1]));
                dimensions.push_back(arguments[2]);
                dimensions.push_back(arguments[3]);
                dimensions.push_back(arguments[4]);
                dimensions.push_back(arguments[5]);
            }
            if(dimensions.size() == 1 && dimensions[0] != "clear") {
                const char *rectStr = dimensions[0].c_str();
#ifdef GIBBON_COORDINATE_FLOAT
                const char *format1 = "%f,%f+%fx%f";
                const char *format2 = "[%f,%f+%fx%f]";
                const char *format3 = "%f,%f,%f,%f";
#else
                const char *format1 = "%d,%d+%dx%d";
                const char *format2 = "[%d,%d+%dx%d]";
                const char *format3 = "%d,%d,%d,%d";
#endif
                if(sscanf(rectStr, format1, &rect.x, &rect.y, &rect.width, &rect.height) != 4 &&
                   sscanf(rectStr, format2, &rect.x, &rect.y, &rect.width, &rect.height) != 4 &&
                   sscanf(rectStr, format3, &rect.x, &rect.y, &rect.width, &rect.height) != 4)
                    rect.clear();
            } else if(dimensions.size() == 4) {
                rect.x = atoi(dimensions[0].c_str());
                rect.y = atoi(dimensions[1].c_str());
                rect.width = atoi(dimensions[2].c_str());
                rect.height = atoi(dimensions[3].c_str());
            }
            if(widget) {
                const Rect renderRect = widget->getGlobalRect();
                if(rect.empty()) {
                    rect = renderRect;
                } else {
                    rect.x += renderRect.x;
                    rect.y += renderRect.y;
                }
            }
            if(Widget::SharedPtr overlay = gibbonApp()->getScreen()->createOverlay(rect)) {
                if(widget)
                    overlay->setName(overlay->getName() + "::" + widget->describe());
                Log::warn(TRACE_UI_ENGINE, "Creating overlay: %s (%s)", rect.toString().c_str(), overlay->describe().c_str());
            }
        }
    }
    virtual std::vector<Help> help() const
    {
        std::vector<Help> result;
        result.push_back(Help("clear", "Clear existing overlays"));
        result.push_back(Help("ruler", "Overlay ruler."));
        result.push_back(Help("[rectangle]", "Render an overlay rectangle at [rectangle]. [rectangle] is interpreted as: 'x,y+wxh' 'x y w h' or 'x,y,w,h'"));
        result.push_back(Help("<id>", "Overlay widget (either widget id# or name)."));
        return result;
    }
};

class TextCommand : public Console::Command
{
public:
    TextCommand()
        : Console::Command("text", "Display rich text information for <widget>")
    {}

    virtual void invoke(const Arguments &arguments) const
    {
        ScopedMutex locker(Widget::lock());
        const char* colors[15][2] = {
            { "magnuson", "#6464dc64" }, { "maroon", "#80000064" }, { "red", "#ff000064" },
            { "orange", "#ffa50064" }, { "yellow", "#ffff0064" }, { "olive", "#80800064" },
            { "purple", "#80008064" }, { "fuchsia", "#ff00ff64" }, { "lime", "#00ff0064" },
            { "green", "#00800064" }, { "navy", "#00008064" }, { "blue", "#0000ff64" },
            { "aqua", "#00ffff64" }, { "teal", "#00808064" }, { "silver", "#c0c0c064" }
        };

        if(arguments.size() < 2) {
            Log::warn(TRACE_UI_ENGINE, "/%s [widget]", arguments[0].c_str());
        } else if(Widget::SharedPtr widget = gibbonApp()->findWidget(arguments[1])) {
            unsigned int flags = 0;
            if(arguments.size() > 2) {

                if(arguments[2] == "clear") {
                    gibbonApp()->getScreen()->setOverlay(Widget::SharedPtr());
                    return;

                } else if(arguments[2] == "overlay" || arguments[2] == "overlayc") {
                    if(Text::SharedPtr text = widget->getText()) {
                        if (arguments[2] == "overlayc")
                            gibbonApp()->getScreen()->setOverlay(Widget::SharedPtr());

                        int type = 0; // runs
                        std::string key;
                        const Rect renderRect = widget->getGlobalRect();

                        int line = -1;
                        int column = -1;
                        bool all = false;

                        if (arguments.size() == 3) {
                            line = -1;
                            all = true;
                        } else {
                            if (arguments[3] == "padding") {
                                type = 1; // padding
                            } else if (arguments[3] == "fonts" || arguments[3] == "font") {
                                type = 2;
                                key = "fonts";
                            } else if (arguments[3] == "directions" || arguments[3] == "direction") {
                                type = 2;
                                key = "direction";
                            } else if (arguments[3] == "locales" || arguments[3] == "locale") {
                                type = 2;
                                key = "locales";
                            } else if (arguments[3] == "scripts" || arguments[3] == "scripts") {
                                type = 2;
                                key = "scripts";
                            } else if (arguments[3] == "clear") {
                                gibbonApp()->getScreen()->setOverlay(Widget::SharedPtr());
                                return;
                            } else {
                                line = atoi(arguments[3].c_str());
                                all = false;
                            }
                        }

                        if (arguments.size() > 4) {
                            if (arguments[4] == "all" || arguments[4] == "print")
                                all = true;
                            else
                                column = atoi(arguments[4].c_str());
                        }

                        int color = 0;
                        std::map<std::string, std::vector<Rect> > m = text->dumpRectangles(line, column, all, type, key);
                        if (type == 0 || type == 1) { // single vector property
                            std::vector<Rect> v = m["rects"];

                            if (v.size() > 1)
                                color = 1;
                            for (std::vector<Rect>::size_type i = 0; i < v.size(); ++i) {
                                v[i] += renderRect.origin();
                                if(Widget::SharedPtr overlay = gibbonApp()->getScreen()->createOverlay(v[i], Color(colors[color][1]))) {
                                    if(widget)
                                        overlay->setName(overlay->getName() + "::" + widget->describe());
                                }
                                ++color;
                                if (color > 14)
                                    color = 1;
                            }
                        } else { // map of vectors
                            color = 1;
                            for (std::map<std::string, std::vector<Rect> >::iterator mit = m.begin(); mit != m.end(); ++mit) {
                                std::vector<Rect> v = mit->second;

                                for (std::vector<Rect>::size_type i = 0; i < v.size(); ++i) {
                                    v[i] += renderRect.origin();
                                    if(Widget::SharedPtr overlay = gibbonApp()->getScreen()->createOverlay(v[i], Color(colors[color][1]))) {
                                        if(widget)
                                            overlay->setName(overlay->getName() + "::" + widget->describe());
                                    }
                                }

                                ++color;
                                if (color > 14)
                                    color = 1;

                            }
                        }

                        return;
                    } else {
                        Log::warn(TRACE_UI_ENGINE, "%s has no text!", widget->describe().c_str());
                    }

                    return;

                } else {
                    for(size_t i = 2; i < arguments.size(); ++i) {
                        if(arguments[i] == "attr") {
                            flags |= Text::Dump_Attributes;
                        } else if(arguments[i] == "direction") {
                            flags |= Text::Dump_Direction;
                        } else if(arguments[i] == "scripts") {
                            flags |= Text::Dump_Scripts;
                        }  else if(arguments[i] == "locales") {
                            flags |= Text::Dump_Locale;
                        } else if(arguments[i] == "words") {
                            flags |= Text::Dump_WordWrap;
                        } else if(arguments[i] == "final") {
                            flags |= Text::Dump_Final;
                        } else if(arguments[i] == "all") {
                            flags |= Text::Dump_All;
                        }
                    }
                }
            }

            if(Text::SharedPtr text = widget->getText()) {
                text->dump(flags);
            } else {
                Log::warn(TRACE_UI_ENGINE, "%s has no text!", widget->describe().c_str());
            }
        } else {
            Log::warn(TRACE_UI_ENGINE, "Widget not found!");
        }
    }
    virtual std::vector<Help> help() const
    {
        std::vector<Help> result;
        result.push_back(Help("<id> [items]", "Text information for widget (either widget id# or name). [Items] can be attr, direction, scripts, locales, words, final or all"));
        return result;
    }
};


#if GIBBON_BENCHMARKS
class BenchmarkCommand : public Console::Command
{
public:
    BenchmarkCommand()
        : Console::Command("benchmark", "Run a benchmark", Variant::StringMap("scroll", "graphics engine painting")("scrollWidgets", "widget painting"))
    {}

    virtual void invoke(const Arguments &arguments) const
    {
        if(arguments.size() >= 2) {
            std::vector<std::string> benchmarkArgs;
            for(size_t i = 2; i < arguments.size(); ++i)
                benchmarkArgs.push_back(arguments[i]);
#if defined(GIBBON_NO_THREADANIMATION)
            Widget::lock().unlock();
            Benchmarks::run(arguments[1], benchmarkArgs);
            Widget::lock().lock();
#else
            GibbonRunBenchmarkEvent::SharedPtr e(new GibbonRunBenchmarkEvent(arguments[1], benchmarkArgs));
            Animation::postEvent(e);
#endif
        }
    }
    virtual std::vector<Help> help() const
    {
        return Benchmarks::help();
    }
};
#endif

class SetBreakpointCommand : public ApplicationCommand
{
public:
    SetBreakpointCommand()
        : ApplicationCommand("break", "Set JS breakpoint")
    {}

    virtual void invokeOnCurrentThread(const Arguments &arguments) const
    {
        if(arguments.size() == 1) {
            std::vector<ScriptEngine::Breakpoint> brks;
            gibbonApp()->getScriptEngine()->listBreakpoints(brks);
            if(brks.empty())
                Log::warn(TRACE_UI_ENGINE, "No breakpoints");
            else {
                for(unsigned int i = 0; i < brks.size(); ++i) {
                    std::string str = StringFormatter::sformat("%d %s:%d", i + 1, brks[i].url.str().c_str(), brks[i].line);
                    if(!brks[i].condition.empty()) {
                        str += " (";
                        str += brks[i].condition;
                        str += ')';
                    }
                    Log::warn(TRACE_UI_ENGINE, "%s", str.c_str());
                }
            }
            return;
        }
        assert(arguments.size() >= 2);

        char* end;
        const long int n = strtol(arguments[1].c_str(), &end, 10);
        if(end == arguments[1].c_str() + arguments[1].size()) {
            // first argument is an integer, assume conditional breakpoint
            std::string cond;
            for(unsigned int i = 2; i < arguments.size(); ++i) {
                cond += arguments[i];
                if(i + 1 < arguments.size())
                    cond += " ";
            }
            gibbonApp()->getScriptEngine()->setCondition(n, cond);
            return;
        } else if(arguments.size() != 3) {
            Log::warn(TRACE_UI_ENGINE, "Invalid arguments");
            return;
        }

        const std::string file = arguments[1];
        const int line = atoi(arguments[2].c_str());
        const unsigned int id = gibbonApp()->getScriptEngine()->setBreakpoint(file, line);
        Log::warn(TRACE_UI_ENGINE, "Breakpoint %u set at %s:%d", id, file.c_str(), line);
    }
};

class RemoveBreakpointCommand : public ApplicationCommand
{
public:
    RemoveBreakpointCommand()
        : ApplicationCommand("delete", "Remove JS breakpoint")
    {}

    virtual void invokeOnCurrentThread(const Arguments &arguments) const
    {
        if(arguments.size() < 2)
            return;

        char* end;
        const long int id = strtol(arguments[1].c_str(), &end, 10);
        if(!id && end == arguments[1].c_str()) {
            Log::warn(TRACE_UI_ENGINE, "Invalid integer");
            return;
        }
        gibbonApp()->getScriptEngine()->removeBreakpoint(id);
    }
};

class BacktraceCommand : public ApplicationCommand
{
public:
    BacktraceCommand()
        : ApplicationCommand("backtrace", "Get a stack trace at the current breakpoint")
    {}

    virtual void invokeOnCurrentThread(const Arguments &) const
    {
        std::vector<ScriptEngine::StackFrame> frames;
        gibbonApp()->getScriptEngine()->stackTrace(frames);
        if(frames.empty()) {
            Log::warn(TRACE_UI_ENGINE, "No stack trace");
        } else {
            for(unsigned int i = 0; i < frames.size(); ++i) {
                Log::warn(TRACE_UI_ENGINE, "%d %s:%d %s", i + 1, frames[i].url.str().c_str(),
                          frames[i].line, frames[i].function.c_str());
            }
        }
    }
};

class SetFrameCommand : public ApplicationCommand
{
public:
    SetFrameCommand()
        : ApplicationCommand("frame", "Set the current frame for the current break")
    {}

    virtual void invokeOnCurrentThread(const Arguments &arguments) const
    {
        if(arguments.size() >= 2) {
            const int frame = atoi(arguments[1].c_str());
            if(frame >= 0)
                gibbonApp()->getScriptEngine()->setCurrentFrame(static_cast<size_t>(frame));
        }
    }
};

class IdentifiersCommand : public ApplicationCommand
{
public:
    IdentifiersCommand()
        : ApplicationCommand("identifiers", "Get all identifiers for the current breakpoint")
    {}
    virtual void invokeOnCurrentThread(const Arguments &) const
    {
        Variant idents;
        gibbonApp()->getScriptEngine()->listIdentifiers(idents);
        if(idents.empty()) {
            Log::warn(TRACE_UI_ENGINE, "No identifiers");
        } else {
            Log::warn(TRACE_UI_ENGINE, "%s", gibbonConsole()->formatJS(idents).c_str());
        }
    }
};


class ContinueCommand : public ApplicationCommand
{
public:
    ContinueCommand()
        : ApplicationCommand("continue", "Continue from JS breakpoint")
    {}
    virtual void invokeOnCurrentThread(const Arguments &) const
    {
        gibbonApp()->getScriptEngine()->continueFromBreakpoint();
    }
};

class NextCommand : public ApplicationCommand
{
public:
    NextCommand()
        : ApplicationCommand("next", "Step over JS statement")
    {}
    virtual void invokeOnCurrentThread(const Arguments &) const
    {
        gibbonApp()->getScriptEngine()->stepOver();
    }
};

class StepCommand : public ApplicationCommand
{
public:
    StepCommand()
        : ApplicationCommand("step", "Step into JS statement")
    {}
    virtual void invokeOnCurrentThread(const Arguments &) const
    {
        gibbonApp()->getScriptEngine()->stepInto();
    }
};

class FinishCommand : public ApplicationCommand
{
public:
    FinishCommand()
        : ApplicationCommand("finish", "Finish JS statement")
    {}
    virtual void invokeOnCurrentThread(const Arguments &) const
    {
        gibbonApp()->getScriptEngine()->finish();
    }
};

class CookiesCommand : public Console::Command
{
public:
    CookiesCommand()
        : Console::Command("cookies", "Display all cookies in the cookie jar")
    {}
    virtual void invoke(const Arguments &) const
    {
        gibbonApp()->resourceManager()->cookieManager()->dump();
    }
};

class LayoutCommand : public Console::Command
{
public:
    LayoutCommand()
        : Console::Command("layout", "Cause a re-layout of the scene")
    {}
    virtual void invoke(const Arguments &arguments) const
    {
        ScopedMutex locker(Widget::lock());
        if(Widget::SharedPtr widget = arguments.size() > 1 ? gibbonApp()->findWidget(DumpInfo(arguments[1])) : gibbonApp()->getScreen()->root()) {
            widget->layout(true);
        }
    }
    virtual std::vector<Help> help() const
    {
        std::vector<Help> result;
        result.push_back(Help("<id>", "Force relayout for widget (either widget id# or name)."));
        return result;
    }
};

class KillCommand : public Console::Command
{
public:
    KillCommand()
        : Console::Command("kill", "Kill a view")
    {}
    virtual void invoke(const Arguments &arguments) const
    {
        {
            ScopedMutex locker(Widget::lock());

            if(arguments.size() > 1 && arguments[1] == "offscreen") {
                std::list<Widget::SharedPtr> widgets = gibbonApp()->getWidgets();
                for(std::list<Widget::SharedPtr>::const_iterator it = widgets.begin(), end = widgets.end(); it != end; ++it) {
                    if(!(*it)->isOnScreen()) {
                        (*it)->setParent(Widget::SharedPtr());
                    }
                }
            } else {
                for(size_t i = 1; i < arguments.size(); ++i) {
                    if(Widget::SharedPtr widget = gibbonApp()->findWidget(DumpInfo(arguments[i])))
                        widget->setParent(Widget::SharedPtr());
                }
            }
        };
    }
    virtual std::vector<Help> help() const
    {
        std::vector<Help> result;
        result.push_back(Help("<id>", "Remove widget from render tree (either widget id# or name)."));
        result.push_back(Help("offscreen", "Remove all offscreen widgets."));
        return result;
    }
};

class KeyCommand : public Console::Command
{
public:
    KeyCommand()
        : Console::Command("key", "Simulate <key> press/release", keyCompletions())
    {}
    virtual void invoke(const Arguments &arguments) const
    {
        if(arguments.size() > 1) {
            sendKeyEvent(arguments[1], arguments.size() > 2 ? atoi(arguments[2].c_str()) : 1, arguments.size() > 3 ? atoi(arguments[3].c_str()) : GibbonConfiguration::keyAutoRepeatInterval());
        };
    }
    virtual std::vector<Help> help() const
    {
        std::vector<Help> result;
        result.push_back(Help("<key>", "Send <key> as press+release."));
        result.push_back(Help("<repeat>", "Send <key> <repeat> times."));
        result.push_back(Help("<interval>", "Send <repeat> with <interval> between each repeat."));
        return result;
    }
};

class VersionsCommand : public Console::Command
{
public:
    VersionsCommand()
        : Console::Command("versions", "Get a list of third party libraries in use and their versions")
    {}
    virtual void invoke(const Arguments &) const
    {
        const Variant result = gibbonApp()->versionInfo();
        if(!result.isNull()) {
            Log::warn(TRACE_UI_ENGINE) << "Gibbon built " << result["buildDate"].string() << " at " << result["buildTime"].string();
            Log::warn(TRACE_UI_ENGINE) << "Platform: " << GibbonConfiguration::platform();
            Log::warn(TRACE_UI_ENGINE) << "Production: " << std::boolalpha << result["production"].boolean() << ", Debug: " << std::boolalpha << result["debug"].boolean();
            Log::warn(TRACE_UI_ENGINE) << "LFLAGS: " << result["compiler"]["lflags"].string();
            Log::warn(TRACE_UI_ENGINE) << "CFLAGS: " << result["compiler"]["cflags"].string();
            Log::warn(TRACE_UI_ENGINE) << "CXXFLAGS: " << result["compiler"]["cxxflags"].string();
            if(result.contains("branch"))
                Log::warn(TRACE_UI_ENGINE) << "Branch: " << result["branch"].string();
            if(result.contains("build"))
                Log::warn(TRACE_UI_ENGINE) << "Build: " << result["build"].string();
#if defined(NRDP_LITTLE_ENDIAN)
            Log::warn(TRACE_UI_ENGINE) << "CPU endianness: little";
#elif defined(NRDP_BIG_ENDIAN)
            Log::warn(TRACE_UI_ENGINE) << "CPU endianness: big";
#elif defined(NRDP_PDP_ENDIAN)
            Log::warn(TRACE_UI_ENGINE) << "CPU endianness: pdp";
#else
            Log::warn(TRACE_UI_ENGINE) << "CPU endianness: unknown";
#endif
#if defined(NRDP_GPU_LITTLE_ENDIAN)
            Log::warn(TRACE_UI_ENGINE) << "GPU endianness: little";
#elif defined(NRDP_GPU_BIG_ENDIAN)
            Log::warn(TRACE_UI_ENGINE) << "GPU endianness: big";
#elif defined(NRDP_GPU_PDP_ENDIAN)
            Log::warn(TRACE_UI_ENGINE) << "GPU endianness: pdp";
#else
            Log::warn(TRACE_UI_ENGINE) << "GPU endianness: unknown";
#endif

#if defined(NRDP_COMPILER_GCC)
            Log::warn(TRACE_UI_ENGINE) << "Compiled with gcc version " << __VERSION__;
#elif defined (NRDP_COMPILER_CLANG)
            Log::warn(TRACE_UI_ENGINE) << "Compiled with clang version " << __clang_version__;
#elif defined(NRDP_COMPILER_MSVC)
            Log::warn(TRACE_UI_ENGINE) << "Compiled with msvc version " << _MSC_VER;
#endif
#if defined(NRDP_PLATFORM_LINUX) && !defined(__APPLE__)
#if defined(__UCLIBC__)
            Log::warn(TRACE_UI_ENGINE) << "uclibc version: " << __UCLIBC_MAJOR__ << "." << __UCLIBC_MINOR__ << "." << __UCLIBC_SUBLEVEL__;
#elif defined(__GLIBC__)
            Log::warn(TRACE_UI_ENGINE) << "glibc version - Compiled: " << __GLIBC__ << "." << __GLIBC_MINOR__ << " - Runtime: " << gnu_get_libc_version() << " " << gnu_get_libc_release();
# endif
            int sched_type = NRDP_SCHEDULER_TYPE;
            std::string scheduler;
            switch (sched_type) {
            case SCHED_FIFO:
                scheduler = "SCHED_FIFO";
                break;
            case SCHED_RR:
                scheduler = "SCHED_RR";
                break;
            case SCHED_OTHER:
                scheduler = "SCHED_OTHER";
                break;
            default:
                std::ostringstream stream;
                stream << "unknown: " << sched_type;
                scheduler = stream.str();
                break;
            }
            Log::warn(TRACE_UI_ENGINE) << "Thread scheduling policy: " << scheduler;
#endif

            Variant components = result["components"];
            Variant::StringMapIterator begin = components.stringMapBegin();
            Variant::StringMapIterator end = components.stringMapEnd();

            Log::warn(TRACE_UI_ENGINE) << " ";
            Log::warn(TRACE_UI_ENGINE) << "Components:";

            for(Variant::StringMapIterator it = begin; it != end; ++it) {
                const std::pair<std::string, Variant> &node = *it;
                Log::warn(TRACE_UI_ENGINE) << "  - " << node.first << ": " << node.second.string();
            }

            Log::warn(TRACE_UI_ENGINE) << " ";
            Log::warn(TRACE_UI_ENGINE) << "Build options:";
            Log::warn(TRACE_UI_ENGINE) << "  - Platform: " << GIBBON_PLATFORM;
            Log::warn(TRACE_UI_ENGINE) << "  - Event loop: " << GIBBON_EVENTLOOP;
            Log::warn(TRACE_UI_ENGINE) << "  - JavaScript engine: " << GIBBON_SCRIPT;
            Log::warn(TRACE_UI_ENGINE) << "  - Graphics: " << GIBBON_GRAPHICS;
            Log::warn(TRACE_UI_ENGINE) << "  - Input: " << GIBBON_INPUT;
            Log::warn(TRACE_UI_ENGINE) << "  - Network: " << GIBBON_NETWORK;

            Log::warn(TRACE_UI_ENGINE) << "  - Image formats: "
#ifdef GIBBON_SURFACEDECODER_JPEG
                                       << "JPEG ("
#if defined(GIBBON_JPEG_SLOW)
                                       << "slow"
#elif defined(GIBBON_JPEG_FLOAT)
                                       << "float"
#else
                                       << "fast"
#endif
                                       << ", upsample: "
#ifdef GIBBON_JPEG_UPSAMPLE
                                       << "yes"
#else
                                       << "no"
#endif
                                       << ") "
#endif
#ifdef GIBBON_SURFACEDECODER_PNG
                                       << "PNG "
#endif
#ifdef GIBBON_SURFACEDECODER_MNG
                                       << "MNG "
#endif
#ifdef GIBBON_SURFACEDECODER_WEBP
                                       << "WEBP "
#endif
                ;

            Variant libraries = result["libraries"];
            Log::warn(TRACE_UI_ENGINE) << " ";
            Log::warn(TRACE_UI_ENGINE) << "Libraries:";
            for(int i = 0; i < libraries.size(); ++i) {
                const Variant &library = libraries[i];

                Log::stream info = Log::stream(TRACE_UI_ENGINE, Log::Warn) << "  - " << library["name"].string();

                if(!library["runtime"].isNull()) {
                    info << " " << library["runtime"].string();
                }

                if(!library["compiled"].isNull()) {
                    info << " (compiled " << library["compiled"].string() << ")";
                }

                info.flush();

                if(!library["options"].isNull()) {
                    Log::warn(TRACE_UI_ENGINE) << "    Options: " << library["options"].string();
                }
            }

            libraries = result["uiLibraries"];
            Log::warn(TRACE_UI_ENGINE) << " ";
            Log::warn(TRACE_UI_ENGINE) << "UI Libraries:";
            for(int i = 0; i < libraries.size(); ++i) {
                const Variant &library = libraries[i];

                Log::stream info = Log::stream(TRACE_UI_ENGINE, Log::Warn) << "  - " << library["name"].string();

                if(!library["runtime"].string().empty()) {
                    info << " " << library["runtime"].string();
                }

                if(!library["compiled"].string().empty()) {
                    info << " (compiled " << library["compiled"].string() << ")";
                }

                info.flush();

                if(!library["component"].isNull() && !library["component"].string().empty())
                    Log::warn(TRACE_UI_ENGINE) << "    Component: " << library["component"].string();
                if(!library["options"].isNull() && !library["options"].string().empty())
                    Log::warn(TRACE_UI_ENGINE) << "    Options: " << library["options"].string();
            }
        }
    }
};

class SleepCommand : public ApplicationCommand
{
public:
    SleepCommand()
        : ApplicationCommand("sleep", "Sleep the main thread for <n> seconds")
    {}
    virtual void invokeOnCurrentThread(const Arguments &arguments) const
    {
        unsigned int sleep_time = 0;
        for(size_t i = 1; i < arguments.size(); ++i) {
            if(arguments[i] == "application") {
                assert(Application::isAppThread());
#ifndef GIBBON_NO_THREADANIMATION
            } else if(arguments[i] == "animation") {
                assert(Animation::isAnimationThread());
#endif
            } else {
                sleep_time = atoi(arguments[1].c_str());
            }
        }
        if(sleep_time) {
            Log::warn(TRACE_UI_ENGINE, "Sleeping for %u", sleep_time);
            sleep(sleep_time);
            Log::warn(TRACE_UI_ENGINE, "Slept for %u", sleep_time);
        }
    }
    virtual void invoke(const Arguments &arguments) const
    {
        invokeOnThread(arguments.contains("animation") ? Thread_Animation : Thread_Application, arguments);
    }
    virtual std::vector<Help> help() const
    {
        std::vector<Help> result;
        result.push_back(Help("application", "Sleep in the main thread."));
        result.push_back(Help("animation", "Sleep in the animation thread."));
        result.push_back(Help("<ms>", "Sleep for <ms>."));
        return result;
    }
};

#ifdef NRDP_HAS_SOCKETIZER
class SocketizerCommand : public Console::Command
{
public:
    SocketizerCommand()
        : Console::Command("socketizer", "Modify socketizer rules", Variant::StringMap("add", "host call probability timeout block errno count countprobability filedescriptor domain type protocol"))
    {}
    virtual void invoke(const Arguments &arguments) const
    {
        Socketizer::Condition condition;
        std::string host;
        bool hasCall = false;
        Socketizer::Call call;

        if(arguments.isEmpty()) {
            Log::error(TRACE_UI_ENGINE, "No arguments passed to /socketizer");
            return;
        }

        if(arguments[0] == "add") {
            const size_t sz = arguments.size();
            for(size_t i = 1; i < sz; ++i) {
                if(arguments[i] == "host") {
                    host = getArgument(arguments, i);
                    if(!i) {
                        Log::error(TRACE_UI_ENGINE, "Invalid argument to 'host'");
                        return;
                    }
                } else if(arguments[i] == "call") {
                    call = Socketizer::stringToCall(getArgument(arguments, i), &hasCall);
                    if(!i || !hasCall) {
                        Log::error(TRACE_UI_ENGINE, "Invalid argument to 'call'");
                        return;
                    }
                } else if(arguments[i] == "probability") {
                    const std::string prob = getArgument(arguments, i);
                    if(!i) {
                        Log::error(TRACE_UI_ENGINE, "Invalid argument to 'probability'");
                        return;
                    }
                    char* end;
                    errno = 0;
                    condition.probability = strtof(prob.c_str(), &end);
                    condition.flags |= Socketizer::Condition::Probability;
                    if(end != prob.c_str() + prob.size() || errno == ERANGE) {
                        Log::error(TRACE_UI_ENGINE, "Invalid floating point number passed to 'probability'");
                        return;
                    }
                } else if(arguments[i] == "timeout") {
                    const std::string timeout = getArgument(arguments, i);
                    if(!i) {
                        Log::error(TRACE_UI_ENGINE, "Invalid argument to 'timeout'");
                        return;
                    }
                    char* end;
                    condition.timeout = strtoul(timeout.c_str(), &end, 10);
                    condition.flags |= Socketizer::Condition::Timeout;
                    if(end != timeout.c_str() + timeout.size()) {
                        Log::error(TRACE_UI_ENGINE, "Invalid number passed to 'timeout'");
                        return;
                    }
                } else if(arguments[i] == "block") {
                    const std::string block = getArgument(arguments, i);
                    if(!i) {
                        Log::error(TRACE_UI_ENGINE, "Invalid argument to 'block'");
                        return;
                    }
                    char* end;
                    condition.block = strtoul(block.c_str(), &end, 10);
                    condition.flags |= Socketizer::Condition::Block;
                    if(end != block.c_str() + block.size()) {
                        Log::error(TRACE_UI_ENGINE, "Invalid number passed to 'block'");
                        return;
                    }
                } else if(arguments[i] == "errno") {
                    const std::string err = getArgument(arguments, i);
                    if(!i) {
                        Log::error(TRACE_UI_ENGINE, "Invalid argument to 'errno'");
                        return;
                    }
                    char* end;
                    condition.err = strtoul(err.c_str(), &end, 10);
                    condition.flags |= Socketizer::Condition::Errno;
                    if(end != err.c_str() + err.size()) {
                        Log::error(TRACE_UI_ENGINE, "Invalid number passed to 'errno'");
                        return;
                    }
                } else if(arguments[i] == "count") {
                    const std::string count = getArgument(arguments, i);
                    if(!i) {
                        Log::error(TRACE_UI_ENGINE, "Invalid argument to 'count'");
                        return;
                    }
                    char* end;
                    condition.count = strtoul(count.c_str(), &end, 10);
                    condition.flags |= Socketizer::Condition::Count;
                    if(end != count.c_str() + count.size()) {
                        Log::error(TRACE_UI_ENGINE, "Invalid number passed to 'count'");
                        return;
                    }
                } else if(arguments[i] == "countprobability") {
                    condition.flags |= Socketizer::Condition::CountProbability;
                } else if(arguments[i] == "filedescriptor") {
                    const std::string fd = getArgument(arguments, i);
                    if(!i) {
                        Log::error(TRACE_UI_ENGINE, "Invalid argument to 'filedescriptor'");
                        return;
                    }
                    char* end;
                    condition.fd = strtoul(fd.c_str(), &end, 10);
                    condition.flags |= Socketizer::Condition::FileDescriptor;
                    if(end != fd.c_str() + fd.size()) {
                        Log::error(TRACE_UI_ENGINE, "Invalid number passed to 'filedescriptor'");
                        return;
                    }
                } else if(arguments[i] == "domain") {
                    const std::string domain = getArgument(arguments, i);
                    if(!i) {
                        Log::error(TRACE_UI_ENGINE, "Invalid argument to 'domain'");
                        return;
                    }
                    char* end;
                    condition.domain = strtoul(domain.c_str(), &end, 10);
                    condition.flags |= Socketizer::Condition::Domain;
                    if(end != domain.c_str() + domain.size()) {
                        Log::error(TRACE_UI_ENGINE, "Invalid number passed to 'domain'");
                        return;
                    }
                } else if(arguments[i] == "type") {
                    const std::string type = getArgument(arguments, i);
                    if(!i) {
                        Log::error(TRACE_UI_ENGINE, "Invalid argument to 'type'");
                        return;
                    }
                    char* end;
                    condition.type = strtoul(type.c_str(), &end, 10);
                    condition.flags |= Socketizer::Condition::Type;
                    if(end != type.c_str() + type.size()) {
                        Log::error(TRACE_UI_ENGINE, "Invalid number passed to 'type'");
                        return;
                    }
                } else if(arguments[i] == "protocol") {
                    const std::string proto = getArgument(arguments, i);
                    if(!i) {
                        Log::error(TRACE_UI_ENGINE, "Invalid argument to 'protocol'");
                        return;
                    }
                    char* end;
                    condition.protocol = strtoul(proto.c_str(), &end, 10);
                    condition.flags |= Socketizer::Condition::Protocol;
                    if(end != proto.c_str() + proto.size()) {
                        Log::error(TRACE_UI_ENGINE, "Invalid number passed to 'protocol'");
                        return;
                    }
                } else if(arguments[i] == "pattern") {
                    const std::string match = getArgument(arguments, i);
                    if(!i) {
                        Log::error(TRACE_UI_ENGINE, "Invalid match argument to 'pattern'");
                        return;
                    }
                    const std::string repeat = getArgument(arguments, i);
                    if(!i) {
                        condition.pattern.repeat = -1;
                    } else {
                        char* end;
                        condition.pattern.repeat = strtol(repeat.c_str(), &end, 10);
                        condition.pattern.match.resize(match.size());
                        if(end != repeat.c_str() + repeat.size()) {
                            Log::error(TRACE_UI_ENGINE, "Invalid number passed to 'pattern'");
                            return;
                        }
                    }
                    for(size_t i = 0; i < match.size(); ++i) {
                        if(match[i] == '1')
                            condition.pattern.match[i] = true;
                    }
                    condition.flags |= Socketizer::Condition::Pattern;
                }
            }

            if(!condition.flags) {
                Log::error(TRACE_UI_ENGINE, "No conditions passed to /socketizer add");
                return;
            }

            if(hasCall) {
                if(!host.empty())
                    Socketizer::instance()->addCondition(host, call, condition);
                else
                    Socketizer::instance()->addCondition(call, condition);
            } else {
                if(!host.empty())
                    Socketizer::instance()->addCondition(host, condition);
                else
                    Socketizer::instance()->addCondition(condition);
            }
        }
    }

private:
    std::string getArgument(const Arguments& arguments, size_t& idx) const
    {
        if(idx + 1 >= arguments.size()) {
            idx = 0;
            return std::string();
        }
        ++idx;
        return arguments[idx];
    }
};
#endif

void crashAt(unsigned int address)
{
    void *deceased = reinterpret_cast<void*>(address);
    *reinterpret_cast<unsigned int*>(deceased) = 0;
}

class CrashCommand : public ApplicationCommand
{
public:
    CrashCommand()
        : ApplicationCommand("crash", "Crash the application by dereferencing a bad pointer or sending a signal.",
          Variant::StringMap("signal", "SIGSEGV SIGABRT SIGFPE SIGILL SIGBUS")("reason", "crash crash_test oom oom_javascript")("address"))
    {}

    virtual void invokeOnCurrentThread(const Arguments &arguments) const
    {
        if(arguments.size() > 2) {
            if(arguments[1] == "signal") {
                int signum = 0;
                if(arguments[2] == "sigsegv" || arguments[2] == "SIGSEGV") {
                    signum = SIGSEGV;
                } else if(arguments[2] == "sigabrt" || arguments[2] == "SIGABRT") {
                    signum = SIGABRT;
                } else if(arguments[2] == "sigfpe" || arguments[2] == "SIGFPE") {
                    signum = SIGFPE;
                } else if(arguments[2] == "sigill" || arguments[2] == "SIGILL") {
                    signum = SIGILL;
                } else if(arguments[2] == "sigbus" || arguments[2] == "SIGBUS") {
                    signum = SIGBUS;
                } else {
                    signum = SIGSEGV;
                }
                Log::warn(TRACE_UI_ENGINE, "Crashing with signal %d", signum);
                (void)kill(getpid(), signum);
            } else if(arguments[1] == "reason") {
                Log::warn(TRACE_UI_ENGINE, "Crashing with reason %s", arguments[2].c_str());
                if(arguments[2] == "oom") {
                    crashAt(0xdefec8ed);
                } else if(arguments[2] == "oom_javascript") {
                    crashAt(0xbbadbeef);
                } else if(arguments[2] == "crash") {
                    crashAt(0);
                } else {
                    crashAt(0xdecea5ed);
                }

            } else if(arguments[1] == "address") {
                unsigned int address = 0xdecea5ed;
                sscanf(arguments[2].c_str(), "0x%x", &address);
                Log::warn(TRACE_UI_ENGINE, "Crashing at address %u", address);
                crashAt(address);
            }
        } else {
            crashAt(0xdecea5ed);
        }
    }
    virtual std::vector<Help> help() const
    {
        std::vector<Help> result;
        result.push_back(Help("sigsegv", "Send SIGSEG signal."));
        result.push_back(Help("sigfpe", "Send SIGFPE signal."));
        result.push_back(Help("sigabrt", "Send SIGABRT signal."));
        result.push_back(Help("sigill", "Send SIGILL signal."));
        result.push_back(Help("sigbus", "Send SIGBUS signal."));
        return result;
    }
};

struct Run {
    enum State {
        Normal,
        Quote,
        Comment,
        Keyword
    };

    Run(const char *c = 0, int l = 0, State s = Normal)
        : ch(c), length(l), state(s)
    {}

    const char *ch;
    int length;
    State state;
};

static bool findEndQuote(const char *&ch)
{
    int escapes = 0;
    const char quote = *ch;
    while (*++ch) {
        if(*ch == '\\') {
            ++escapes;
            continue;
        }

        if(escapes % 2 == 0 && *ch == quote) {
            return true;
        }
        escapes = 0;
    }
    return false;
}

static inline bool isBoundary(char ch)
{
    // for our purposes . is not a boundary. We don't want "catch" to be a
    // keyword in foobar.catch.foobar
    return !isalnum(ch) && ch != '_' && ch != '$' && ch != '.';
}

static bool findKeyword(const char *&ch)
{
    struct {
        const char *word;
        const size_t length;
    } static const keywords[] = {
        { "do", strlen("do") },
        { "if", strlen("if") },
        { "in", strlen("in") },
        { "for", strlen("for") },
        { "let", strlen("let") },
        { "new", strlen("new") },
        { "try", strlen("try") },
        { "var", strlen("var") },
        { "case", strlen("case") },
        { "else", strlen("else") },
        { "enum", strlen("enum") },
        { "eval", strlen("eval") },
        { "null", strlen("null") },
        { "this", strlen("this") },
        { "true", strlen("true") },
        { "void", strlen("void") },
        { "with", strlen("with") },
        { "break", strlen("break") },
        { "catch", strlen("catch") },
        { "class", strlen("class") },
        { "const", strlen("const") },
        { "false", strlen("false") },
        { "super", strlen("super") },
        { "throw", strlen("throw") },
        { "while", strlen("while") },
        { "yield", strlen("yield") },
        { "delete", strlen("delete") },
        { "export", strlen("export") },
        { "import", strlen("import") },
        { "public", strlen("public") },
        { "return", strlen("return") },
        { "static", strlen("static") },
        { "switch", strlen("switch") },
        { "typeof", strlen("typeof") },
        { "default", strlen("default") },
        { "extends", strlen("extends") },
        { "finally", strlen("finally") },
        { "package", strlen("package") },
        { "private", strlen("private") },
        { "continue", strlen("continue") },
        { "debugger", strlen("debugger") },
        { "function", strlen("function") },
        { "arguments", strlen("arguments") },
        { "interface", strlen("interface") },
        { "protected", strlen("protected") },
        { "implements", strlen("implements") },
        { "instanceof", strlen("instanceof") },
        { 0, 0 }
    };

    for(int i=0; keywords[i].word; ++i) {
        if(!strncmp(keywords[i].word, ch, keywords[i].length) && isBoundary(ch[keywords[i].length])) {
            ch += keywords[i].length;
            return true;
        }
    }
    return false;
}

static std::vector<Run> formatJS(const DataBuffer &text)
{
    const char *ch = text.c_str();
    const char *start = ch;
    std::vector<Run> runs;
    char last = '\0';
    while (*ch) {
        switch (*ch) {
        case '\'':
        case '"': {
            if(start != ch)
                runs.push_back(Run(start, ch - start, Run::Normal));
            start = ch;
            if(!findEndQuote(ch))
                return std::vector<Run>(); // error
            runs.push_back(Run(start, ch - start + 1, Run::Quote));
            start = ch + 1;
            break; }
        case '*':
        case '/':
            if(last == '/') {
                if(start != ch - 1)
                    runs.push_back(Run(start, ch - start - 1, Run::Normal));
                const bool c = *ch == '*';
                const char *end = strstr(ch + 1, (c ? "*/" : "\n"));
                if(!end)
                    return std::vector<Run>(); // error
                runs.push_back(Run(ch - 1, end - ch + (c ? 3 : 2), Run::Comment));
                start = ch = end + (c ? 2 : 1);
            }
            break;
        default:
            if(!last || isBoundary(last)) {
                const char *b = ch;
                if(findKeyword(ch)) {
                    if(start != b)
                        runs.push_back(Run(start, b - start, Run::Normal));
                    runs.push_back(Run(b, ch - b, Run::Keyword));
                    start = ch;
                }
            }
        }
        last = *ch++;
    }
    if(ch - 1> start)
        runs.push_back(Run(start, ch - start + 1, Run::Normal));
    return runs;
}

// ### this should do line numbers
static inline void printRuns(const AseUrl &url, const std::vector<Run> &runs)
{
    std::string out;
    Configuration::Color color = Configuration::Default;
    for(std::vector<Run>::const_iterator it = runs.begin(); it != runs.end(); ++it) {
        switch (it->state) {
        case Run::Normal:
            color = Configuration::Default;
            break;
        case Run::Comment:
            color = Configuration::BrightCyan;
            break;
        case Run::Quote:
            color = Configuration::Green;
            break;
        case Run::Keyword:
            color = Configuration::BrightYellow;
            break;
        }
        out.append(Configuration::colorToEscapeCode(color));
        out.append(it->ch, it->length);
    }
    if(color != Configuration::Default)
        out.append(Configuration::colorToEscapeCode(Configuration::Default));

    std::vector<std::string> lines;
    StringTokenizer::tokenize(out, lines, "\n", StringTokenizer::Tokenize_KeepEmpty);

    std::string str;
    str.reserve(static_cast<size_t>(out.size() * 1.2));

    int digits = 0;
    for(int i=lines.size() + 1; i > 0; i/=10) {
        ++digits;
    }

    char format[16];
    snprintf(format, sizeof(format), "\n%%%dd ", digits);
    int line = 0;
    for(std::vector<std::string>::const_iterator it = lines.begin(); it != lines.end(); ++it) {
        char buf[16];
        const int w = snprintf(buf, sizeof(buf), format, ++line);
        str.append(buf, w);
        str.append(*it);
    }

    Log::warn(TRACE_LOG, "%s%s", url.c_str(), str.c_str());
}

class SourceRequest : public GibbonResourceRequest
{
public:
    SourceRequest(const AseUrl &url)
        : GibbonResourceRequest(GibbonResourceManager::Script|GibbonResourceManager::Trusted, url, Resource::Method_Get)
    {}

    virtual void finish()
    {
        if(resource()->httpStatusCode() >= 200 && resource()->httpStatusCode() < 300) {
            const std::vector<Run> formatted = formatJS(resource()->responseData());
            if(formatted.empty()) {
                Log::warn(TRACE_LOG, "Failed to format JS %s\n%s", url().c_str(), resource()->responseData().constData());
            } else {
                printRuns(url(), formatted);
            }

        } else {
            Log::warn(TRACE_LOG, "Failed to load resource %s", url().c_str());
        }
    }
};

class SourceCommand : public Console::Command
{
public:
    SourceCommand()
        : Console::Command("source", "Display javascript source code for ")
    {}
    virtual void invoke(const Arguments &arguments) const
    {
        AseUrl url = gibbonApp()->getLocation();
        if(arguments.size() > 1) {
            url = arguments[1];
        }
        nrdApp()->resourceManager()->sendRequest(shared_ptr<SourceRequest>(new SourceRequest(url)));
    }
};

GibbonConsole::GibbonConsole()
    : Thread(&THREAD_GIBBON_CONSOLE),
      Console(),
#ifdef HAVE_LIBEDIT
      mMutex(GIBBON_CONSOLE_DIRTY_MUTEX, "GibbonConsoleDirtyMutex"),
      mLibEditLogFile(0), mState(0), mHistory(0),
#endif
      mDone(false), mKeyboardMode(Keyboard_Normal), mEscape(0), mJSMaxLength(GibbonConfiguration::consoleJSMaxLength())
{
    addCommand(shared_ptr<Command>(new RawCommand));
    addCommand(shared_ptr<Command>(new WidgetCommand));
    addCommand(shared_ptr<Command>(new AnimationsCommand));
    addCommand(shared_ptr<Command>(new RemoteScreenCommand));
    addCommand(shared_ptr<Command>(new MemoryCommand));
    addCommand(shared_ptr<Command>(new TasksCommand));
#ifdef NRDP_HAS_VALGRIND
    addCommand(shared_ptr<Command>(new ValgrindCommand));
#endif
#ifdef GIBBON_SAMPLE_ENABLED
        addCommand(shared_ptr<Command>(new SampleCommand));
#endif
    addCommand(shared_ptr<Command>(new FooCommand));
    addCommand(shared_ptr<Command>(new GCCommand));
    addCommand(shared_ptr<Command>(new CookiesCommand));
    addCommand(shared_ptr<Command>(new RenderCommand));
    addCommand(shared_ptr<Command>(new ReloadCommand));
    addCommand(shared_ptr<Command>(new PurgeCommand));
    addCommand(shared_ptr<Command>(new GrabCommand));
    addCommand(shared_ptr<Command>(new DisplayListCommand));
    addCommand(shared_ptr<Command>(new LayoutCommand));
    addCommand(shared_ptr<Command>(new SizeContentCommand));
    addCommand(shared_ptr<Command>(new UpdateCommand));
    addCommand(shared_ptr<Command>(new ViewsCommand));
    addCommand(shared_ptr<Command>(new KillCommand));
    addCommand(shared_ptr<Command>(new SurfaceCommand));
    addCommand(shared_ptr<Command>(new CacheCommand));
    addCommand(shared_ptr<Command>(new KeyCommand));
    addCommand(shared_ptr<Command>(new FontsCommand));
    addCommand(shared_ptr<Command>(new SetCommand));
    addCommand(shared_ptr<Command>(new OverlayCommand));
    addCommand(shared_ptr<Command>(new TextCommand));
    addCommand(shared_ptr<Command>(new SetBreakpointCommand));
    addCommand(shared_ptr<Command>(new RemoveBreakpointCommand));
    addCommand(shared_ptr<Command>(new ContinueCommand));
    addCommand(shared_ptr<Command>(new NextCommand));
    addCommand(shared_ptr<Command>(new StepCommand));
    addCommand(shared_ptr<Command>(new FinishCommand));
    addCommand(shared_ptr<Command>(new BacktraceCommand));
#if GIBBON_BENCHMARKS
    addCommand(shared_ptr<Command>(new BenchmarkCommand));
#endif
    addCommand(shared_ptr<Command>(new SetFrameCommand));
    addCommand(shared_ptr<Command>(new IdentifiersCommand));
    addCommand(shared_ptr<Command>(new VersionsCommand));
    addCommand(shared_ptr<Command>(new SleepCommand));
#ifdef NRDP_HAS_SOCKETIZER
    addCommand(shared_ptr<Command>(new SocketizerCommand));
#endif
    addCommand(shared_ptr<Command>(new CrashCommand));
    addCommand(shared_ptr<Command>(new SourceCommand));

    mPipe[0] = mPipe[1] = -1;
    if(Configuration::consoleEnabled()) {
#ifdef HAVE_LIBEDIT
        if(GibbonConfiguration::libEditEnabled()) {
            mHistory = history_init();
            mLibEditLogFile = fopen("/dev/null", "w");
            sEditLine = el_init("netflix", stdin, stdout, mLibEditLogFile ? mLibEditLogFile : stderr);
            if(!sEditLine || !mHistory) {
                if(mHistory) {
                    history_end(mHistory);
                    mHistory = 0;
                }
                if(sEditLine) {
                    el_end(sEditLine);
                    sEditLine = 0;
                }
            } else {
                HistEvent histEvent;
                history(mHistory, &histEvent, H_SETSIZE, 1024*32);
                history(mHistory, &histEvent, H_SETUNIQUE, 1);

                el_set(sEditLine, EL_SIGNAL, 1);
                el_set(sEditLine, EL_EDITOR, "emacs");
                el_set(sEditLine, EL_HIST, history, mHistory);
                el_set(sEditLine, EL_CLIENTDATA, this);
                el_set(sEditLine, EL_GETCFN, &GibbonConsole::getChar);
                el_set(sEditLine, EL_PROMPT_ESC, &GibbonConsole::prompt, '\1');
                el_set(sEditLine, EL_ADDFN, "ed-complete", "Complete argument", &GibbonConsole::complete);
                el_set(sEditLine, EL_BIND, "^I", "ed-complete", 0);

                if(const char *home = getenv("HOME")) {
                    mHistoryFile = home;
                    mHistoryFile += "/.gibbon_history";
                    history(mHistory, &histEvent, H_LOAD, mHistoryFile.c_str());
                    std::string rc = home;
                    rc += "/.editrc";
                    el_source(sEditLine, rc.c_str());
                }
            }

            // mCompletionRoot->dump();
        }
#endif
        if(pipe(mPipe) == -1) {
            Log::error(TRACE_UI_ENGINE, "Unable to create pipe for GibbonConsole (%d)", errno);
        } else {
            fcntl(mPipe[0], F_SETFL, O_NONBLOCK);
        }
    }
}

void GibbonConsole::stop()
{
    if(mPipe[0] == -1)
        return;

    int ret;
    do {
        ret = write(mPipe[1], "q", 1);
    } while(ret == -1 && errno == EINTR);
    if(ret == -1)
        Log::error(TRACE_UI_ENGINE, "Unable to write 'q' to pipe in GibbonConsole (%d)", errno);
    Wait();
    close(mPipe[0]);
    close(mPipe[1]);
    mPipe[0] = mPipe[1] = -1;
#ifdef HAVE_LIBEDIT
    if(mHistory) {
        history_end(mHistory);
        mHistory = 0;
    }
    if(sEditLine) {
        el_end(sEditLine);
        sEditLine = 0;
        if(mLibEditLogFile)
            fclose(mLibEditLogFile);
    }
#endif
    Console::stop();
}

bool GibbonConsole::handleSequence(const std::string &sequence)
{
    if(mKeyboardMode == Keyboard_PS2) {
        const Point global_position = MouseEvent::getLastPosition();
        static MouseEvent::Buttons global_buttons = MouseEvent::BUTTON_NONE;
        for(size_t i = 0; i < sequence.size(); ) {
            int advance = 1;
            Point position(global_position);
            MouseEvent::Buttons buttons = MouseEvent::BUTTON_NONE;
            if(sequence[i] & 0x08 && i+2 < sequence.size()) {
                const int dx = sequence[i+1], dy = -sequence[i+2];
                position.x = std::max(static_cast<Coordinate>(0), position.x+dx);
                position.y = std::max(static_cast<Coordinate>(0), position.y+dy);
                advance += 2;
            }
            if(sequence[i] & 0x01) {
                if(!(global_buttons & MouseEvent::BUTTON_LEFT))
                    gibbonApp()->sendEvent(new MouseEvent(InputEvent::Type_MousePress, position, MouseEvent::BUTTON_LEFT));
                buttons |= MouseEvent::BUTTON_LEFT;
            } else if(global_buttons & MouseEvent::BUTTON_LEFT) {
                gibbonApp()->sendEvent(new MouseEvent(InputEvent::Type_MouseRelease, position, MouseEvent::BUTTON_LEFT));
            }
            if(sequence[i] & 0x02) {
                if(!(global_buttons & MouseEvent::BUTTON_RIGHT))
                    gibbonApp()->sendEvent(new MouseEvent(InputEvent::Type_MousePress, position, MouseEvent::BUTTON_RIGHT));
                buttons |= MouseEvent::BUTTON_RIGHT;
            } else if(global_buttons & MouseEvent::BUTTON_RIGHT) {
                gibbonApp()->sendEvent(new MouseEvent(InputEvent::Type_MouseRelease, position, MouseEvent::BUTTON_RIGHT));
            }
            if(sequence[i] & 0x02) {
                if(!(global_buttons & MouseEvent::BUTTON_MIDDLE))
                    gibbonApp()->sendEvent(new MouseEvent(InputEvent::Type_MousePress, position, MouseEvent::BUTTON_MIDDLE));
                buttons |= MouseEvent::BUTTON_MIDDLE;
            } else if(global_buttons & MouseEvent::BUTTON_MIDDLE) {
                gibbonApp()->sendEvent(new MouseEvent(InputEvent::Type_MouseRelease, position, MouseEvent::BUTTON_MIDDLE));
            }
            if(position != global_position) {
                MouseEvent *me = new MouseEvent(InputEvent::Type_MouseMove, position, MouseEvent::BUTTON_NONE, buttons);
                me->mShowCursor = true;
                gibbonApp()->sendEvent(me);
            }
            i += advance;
        }
    } else if(mKeyboardMode == Keyboard_Keyboard) {
        for(size_t i = 0; i < sequence.size(); ++i) {
            KeyEvent::Key keycode = KeyEvent::KEY_UNKNOWN;
            const char ch = sequence[i];
            if(!mEscape) {
                if(ch == 0x1b)
                    mEscape = ch;
                else if(ch == 0x0d)
                    keycode = KeyEvent::KEY_RETURN;
            } else if(mEscape == 0x1b) {
                mEscape = 0;
                if(ch == 0x5b)
                    mEscape = ch;
            } else if(mEscape == 0x5b) {
                mEscape = 0;
                if(ch == 0x44)
                    keycode = KeyEvent::KEY_LEFT;
                else if(ch == 0x43)
                    keycode = KeyEvent::KEY_RIGHT;
                else if(ch == 0x41)
                    keycode = KeyEvent::KEY_UP;
                else if(ch == 0x42)
                    keycode = KeyEvent::KEY_DOWN;
            }
            if(keycode != KeyEvent::KEY_UNKNOWN) {
                printf("Handled: %d (0x%x)\n", keycode, ch);
                gibbonApp()->sendEvent(new KeyEvent(InputEvent::Type_KeyPress, keycode, std::string(), 0, false));
                gibbonApp()->sendEvent(new KeyEvent(InputEvent::Type_KeyRelease, keycode, std::string(), 0));
            } else if(!mEscape) {
                printf("Unhandled: 0x%x\n", ch);
            }
        }
    }
    return true;
}

bool GibbonConsole::handleCommand(const std::string &cmd, unsigned int flags)
{
    if(flags & Console_Help)
        return Console::handleCommand(cmd, flags);

    const char *cstr = cmd.c_str();
    bool spaces = false;
    while(isspace(*cstr)) {
        spaces = true;
        ++cstr;
    }
    if(spaces)
        return handleCommand(cstr, flags);

    if(cmd.empty())
        return false;

    std::string command;
    if(cmd == "/") {
        command = mLastCommand;
    } else {
        command = cmd;
        if(command != mLastCommand) {
            mLastCommand = cmd;
#ifdef HAVE_LIBEDIT
            if(mHistory && !(flags & Console_NoHistory)) {
                HistEvent histEvent;
                history(mHistory, &histEvent, H_ENTER, cmd.c_str());
                history(mHistory, &histEvent, H_SAVE, mHistoryFile.c_str());
            }
#endif
        }
    }

    if(command[0] != '/') {
        shared_ptr<GibbonConsoleJSEvent> event(new GibbonConsoleJSEvent(cmd));
        gibbonApp()->postEvent(event);
        return true;
    }

    if(!Console::handleCommand(command, flags)) {
        Log::error(TRACE_LOG, "Unknown command: %s", cmd.c_str());
        return false;
    }
    return true;
}

struct HeapCountInfo {
    HeapCountInfo()
        : count(0), cellSize(0), stringSize(0)
    {}
    size_t count;
    size_t cellSize;
    size_t stringSize;
};

#ifdef HAVE_LIBEDIT
class GibbonConsoleJavascriptCompletionEvent : public GibbonApplication::Event
{
public:
    GibbonConsoleJavascriptCompletionEvent(const std::string &commandLine)
        : mCommandLine(commandLine), mMutex(netflix::ZERO_MUTEX, "GibbonConsoleJavascriptCompletionEvent"), mDone(false)
    {}

    ~GibbonConsoleJavascriptCompletionEvent()
    {
        ScopedMutex lock(mMutex);
        mDone = true;
        mCond.broadcast();
    }

    std::string describe() const
    {
        return "GibbonConsoleJavascriptCompletionEvent";
    }

    virtual void eventFired()
    {
        assert(gibbonApp());
        shared_ptr<ScriptEngine> engine = gibbonApp()->getScriptEngine();
        mCompletions = engine->completions(mCommandLine);
        ScopedMutex lock(mMutex);
        mDone = true;
        mCond.broadcast();
    }

    std::vector<std::string> wait()
    {
        ScopedMutex lock(mMutex);
        while(!mDone)
            mCond.wait(&mMutex);
        return mCompletions;
    }
protected:
    const std::string mCommandLine;
    std::vector<std::string> mCompletions;
    weak_ptr<GibbonConsole> mConsole;
    const std::string mCommand;
    std::string mResult;
    Mutex mMutex;
    ConditionVariable mCond;
    bool mDone;
};
#endif

void GibbonConsole::Run()
{
    assert(Configuration::consoleEnabled());

    char *pending = 0;
    int pendingSize = 0;
#ifdef HAVE_LIBEDIT
    size_t completionsSize = 0;
#endif
    while(!mDone) {
        PERFORMANCE_MARKER_SCOPED("console.loop");
#ifdef HAVE_LIBEDIT
        const std::vector<shared_ptr<Command> > cmds = commands();
        if(cmds.size() != completionsSize) {
            completionsSize = cmds.size();
            mCompletionRoot.reset(new Completion);
            for(std::vector<shared_ptr<Command> >::const_iterator it = cmds.begin(); it != cmds.end(); ++it) {
                if((*it)->name != "/") {
                    mCompletionRoot->add(std::string("/") + (*it)->name, (*it)->completions);
                }
            }
        }

        if(sEditLine) {
            if(readFromPipe() & Quit)
                return;
            {
                ScopedMutex lock(mMutex);
                mState = Reading;
            }

            int count;
            const char *ch = el_gets(sEditLine, &count);
            {
                ScopedMutex lock(mMutex);
                mState &= ~Reading;
            }

            if(!ch || count <= 0)
                break;

            handleCommand(std::string(ch, count - 1), Console::Console_None);
            continue;
        }
#endif
        int ret;
        fd_set r;
        do {
            int max;
            FD_ZERO(&r);
            FD_SET(mPipe[0], &r);
            FD_SET(STDIN_FILENO, &r);
            max = std::max(STDIN_FILENO, mPipe[0]);
            ret = select(max + 1, &r, 0, 0, 0);
        } while(ret == -1 && errno == EINTR);

        if(ret == -1)
            break;

        if(FD_ISSET(mPipe[0], &r)) {
            char ch;
            int r = read(mPipe[0], &ch, 1);
            if(r == -1)
                Log::error(TRACE_UI_ENGINE, "Unable to read from pipe in GibbonConsole::Run() (%d)", errno);
            break;
        }
        if(FD_ISSET(STDIN_FILENO, &r)) {
            int len;
            char buffer[1024];
            do {
                len = read(STDIN_FILENO, buffer, sizeof(buffer));
                if(len > 0) {
                    pending = reinterpret_cast<char*>(realloc(pending, pendingSize + len));
                    memcpy(pending + pendingSize, buffer, len);
                    pendingSize += len;
                } else {
                    mDone = true;
                }
            } while(len == sizeof(buffer));
            int last = 0;
            int idx = 0;
            while(idx < pendingSize) {
                if(pending[idx] == '\n') {
                    pending[idx] = '\0';
                    handleCommand(std::string(pending + last, idx - last), Console::Console_None);
                    last = idx + 1;
                }
                ++idx;
            }
            if(last < idx) {
                memmove(pending, pending + last, pendingSize - last);
                pendingSize = last;
                pending = reinterpret_cast<char*>(realloc(pending, pendingSize));
            } else {
                free(pending);
                pending = 0;
                pendingSize = 0;
            }
        }
    }
    if(pending)
        free(pending);
}

void GibbonConsole::consoleReceive(const std::string &msg)
{
    int ret;
#ifdef HAVE_LIBEDIT
    if(GibbonConfiguration::libEditEnabled()) {
        {
            ScopedMutex lock(mMutex);
            if((mState & (Reading|Refresh)) == Reading) {
                ret = write(STDOUT_FILENO, "\n", 1);
                (void)ret;
            }
        }
        if(mPipe[1] != -1) {
            do {
                ret = write(mPipe[1], "r", 1);
            } while(ret == -1 && errno == EINTR);
        }
    }
#endif
    ret = write(STDOUT_FILENO, msg.c_str(), msg.size());
    (void)ret;
}

#ifdef HAVE_LIBEDIT
EditLine *GibbonConsole::sEditLine = 0;
int GibbonConsole::getChar(EditLine *edit, char *ch)
{
    GibbonConsole *console = 0;
    el_get(edit, EL_CLIENTDATA, &console);
    assert(console);

    while(true) {
        fd_set fdRead;
        FD_ZERO(&fdRead);
        FD_SET(STDIN_FILENO, &fdRead);
        FD_SET(console->mPipe[0], &fdRead);
        const int max = std::max(console->mPipe[0], STDIN_FILENO) + 1;
        int ret;
        bool refreshDirty;
        {
            ScopedMutex lock(console->mMutex);
            refreshDirty = console->mState & Refresh;
        }

        do {
            timeval timeout;
            if(refreshDirty) {
                memset(&timeout, 0, sizeof(timeval));
                timeout.tv_sec = 1;
            }
            ret = select(max, &fdRead, 0, 0, refreshDirty ? &timeout : 0);
        } while(ret == -1 && errno == EINTR);

        if(ret > 0 && FD_ISSET(console->mPipe[0], &fdRead)) {
            const unsigned state = console->readFromPipe();
            if(state & Quit) {
                break;
            } else if(state & Refresh) {
                ScopedMutex lock(console->mMutex);
                console->mState |= Refresh;
            }
        } else if(ret > 0 && FD_ISSET(STDIN_FILENO, &fdRead)) {
            assert(FD_ISSET(STDIN_FILENO, &fdRead));
            *ch = '\0';
            do {
                ret = ::read(STDIN_FILENO, ch, 1);
            } while(ret == -1 && errno == EINTR);

            ScopedMutex lock(console->mMutex);
            if(console->mState & Refresh) {
                console->mState &= ~Refresh;
                el_set(edit, EL_REFRESH);
            }
            return ret == 1 ? 1 : -1;
        } else {
            ScopedMutex lock(console->mMutex);
            if(console->mState & Refresh) {
                console->mState &= ~Refresh;
                el_set(edit, EL_REFRESH);
            }
        }
    }
    *ch = '\0';
    return -1;
}

static inline unsigned common(const char *left, const char *right)
{
    unsigned i = 0;
    while(left[i] && left[i] == right[i])
        ++i;
    return i;
}

GibbonConsole::Completion::~Completion()
{
    for(std::map<std::string, Completion*>::const_iterator it = completions.begin(); it != completions.end(); ++it) {
        delete it->second;
    }
}

void GibbonConsole::Completion::add(const std::string &name, const Variant &variant, unsigned flags)
{
    assert(completions.find(name) == completions.end());
    switch (variant.type()) {
    case Variant::Type_String: {
        Completion *c = new Completion;
        c->flags = flags;
        completions[name] = c;
        std::vector<std::string> keys;
        StringTokenizer::tokenize(variant.string(), keys, " ");
        const int count = keys.size();
        assert(count);
        for(int i=0; i<count; ++i) {
            c->add(keys.at(i), Variant(), flags);
        }
        break; }
    case Variant::Type_Null:
        if(flags) {
            Completion *c = new Completion;
            c->flags = flags;
            completions[name] = c;
        } else {
            completions[name] = 0;
        }
        break;
    case Variant::Type_StringMap: {
        Completion *c = new Completion;
        c->flags = flags;
        completions[name] = c;
        const Variant::StringMapIterator end = variant.stringMapEnd();
        for(Variant::StringMapIterator it = variant.stringMapBegin(); it != end; ++it) {
            c->add(it->first, it->second, flags);
        }
        break; }
    default:
        Log::error(TRACE_UI_ENGINE, "Got invalid type %d\n", variant.type());
        assert(0);
        break;
    }
}

std::vector<std::string> GibbonConsole::Completion::alternatives(int &longest, const std::string &base) const
{
    std::vector<std::string> alternatives;
    std::map<std::string, Completion*>::const_iterator it = base.empty() ? completions.begin() : completions.lower_bound(base);
    longest = 0;
    while(it != completions.end() && (base.empty() || startsWith(it->first, base))) {
        alternatives.push_back(it->first);
        longest = std::max<int>(longest, it->first.size());

        ++it;
    }
    return alternatives;
}

const GibbonConsole::Completion *GibbonConsole::Completion::match(std::string &text) const
{
    std::map<std::string, Completion*>::const_iterator it = completions.lower_bound(text);
    std::string match;
    const Completion *c = 0;
    bool full = false;
    unsigned lastFlags = 0;
    while(it != completions.end() && (text.empty() || startsWith(it->first, text))) {
        if(match.empty()) {
            match = it->first;
            c = it->second;
            if(!c && flags & Multi)
                c = this;
            if(c)
                lastFlags = c->flags;
            full = true;
        } else {
            lastFlags = 0;
            c = 0;
            full = false;
            const unsigned l = common(match.c_str(), it->first.c_str());
            if(l < match.size()) {
                match.resize(l);
            }
        }
        ++it;
    }
    bool addSeparator = false;
    if(match.size() > text.size()) {
        text = match;
        addSeparator = full;
    } else if(match.size() == text.size() && full) {
        addSeparator = true;
    }
    if(addSeparator && !(lastFlags & NoTrailingSpace)) {
        text += ' ';
    }

    return c;
}

enum SkipType {
    Space,
    NonSpace
};
static const char *skip(const char *ch, SkipType type, const char *end)
{
    while(ch != end) {
        bool match = false;
        switch (type) {
        case Space: match = isspace((unsigned char)*ch); break;
        case NonSpace: match = !isspace((unsigned char)*ch); break;
        }
        if(!match)
            break;
        ++ch;
    }
    return ch;
}

unsigned char GibbonConsole::complete(EditLine *edit, int)
{
    assert(edit);
    const LineInfo *lineInfo = el_line(edit);
    assert(lineInfo);
    GibbonConsole *console = 0;
    el_get(edit, EL_CLIENTDATA, &console);
    assert(console);

    const char *ch = skip(lineInfo->buffer, Space, lineInfo->cursor);
    if(*ch == '/') {
        return console->completeConsole(edit, lineInfo);
    } else {
        return console->completeJavascript(edit, lineInfo);
    }
}

static inline void printAlternatives(const std::vector<std::string> &alternatives, int longest)
{
    if(!alternatives.empty()) {
        int termWidth = 72;
        char termbuf[2048];
        static char *termtype = getenv("TERM");

        if(tgetent(termbuf, termtype) >= 0) {
            char co[3];
            strcpy(co, "co");
            termWidth = tgetnum(co);
        }
        const int column = longest + 5;
        const int columns = std::max<int>(2, termWidth / column);
        const int s = alternatives.size();
        printf("\n");
        for(int i=0; i<s; ++i) {
            const std::string &alternative = alternatives.at(i);
            const std::string fill(column - alternative.size(), ' ');
            printf("%s%s", alternative.c_str(), fill.c_str());
            if(i + 1 == s || (i + 1) % columns == 0)
                printf("\n");
        }
    }
}


unsigned char GibbonConsole::completeConsole(EditLine *edit, const LineInfo *lineInfo) const
{
    assert(edit);
    assert(lineInfo);
    const char *end = lineInfo->cursor;
    const char *ch = skip(lineInfo->buffer, Space, end);
    const Completion *c = mCompletionRoot.get();
    std::vector<std::string> alternatives;
    int longest = 0;
    if(ch == end) {
        alternatives = c->alternatives(longest);
    } else {
        while(ch != end) {
            const char *start = ch;
            ch = skip(ch, NonSpace, end);
            std::string word(start, ch - start);
            const std::string old = word;
            const unsigned oldLength = word.length();
            const Completion *cc = c->match(word);
            ch = skip(ch, Space, end);
            if(ch == end) {
                char cursorChar = lineInfo->cursor > lineInfo->buffer ? *(lineInfo->cursor - 1) : '\0';
                if(word.length() > oldLength && (cursorChar != ' ' || word.at(oldLength) != ' ' || word.size() - oldLength != 1)) {

                    el_insertstr(edit, word.c_str() + oldLength);
                    return CC_REFRESH;
                } else if(word.length() <= oldLength) {
                    alternatives = c->alternatives(longest, word);
                } else if(cc) {
                    alternatives = cc->alternatives(longest);
                }
                break;
            } else if(!cc) {
                alternatives = c->alternatives(longest, old);
                break;
            } else {
                c = cc;
            }
        }
    }
    if(!alternatives.empty()) {
        printAlternatives(alternatives, longest);
        return CC_REDISPLAY;
    }

    return CC_ERROR;
}

static inline const char *findBeginningOfStatement(const char *c, const char *end)
{
    const char *best = c;
    while(*c && c < end) {
        switch (*c) {
        case '\'':
        case '"': {
            const char quote = *c;
            int slashes = 0;
            bool done = false;
            while(*++c && !done && c < end) {
                switch (*c) {
                case '\'':
                case '"':
                    if(*c == quote) {
                        if(slashes % 2 == 0)
                            done = true;
                        slashes = 0;
                    }
                    break;
                case '\\':
                    ++slashes;
                    break;
                default:
                    slashes = 0;
                    break;
                }
            }
            break; }
        case ',':
        case '(':
        case ')':
        case '<':
        case '>':
        case '+':
        case '-':
        case '=':
        case '/':
        case '*':
        case '%':
        case '^':
        case '|':
        case '&':
        case '{':
        case '}':
        case '[':
        case ']':
            best = c + 1;
            break;
        }
        ++c;
    }
    assert(best);
    while(best < end && isspace(static_cast<unsigned char>(*best)))
        ++best;
    return best;
}

unsigned char GibbonConsole::completeJavascript(EditLine *edit, const LineInfo *lineInfo) const
{
    // FILE *f = fopen("/tmp/log", "a");
    assert(edit);
    assert(lineInfo);

    assert(edit);

    assert(lineInfo);
    const char *end = lineInfo->cursor;
    const char *c = findBeginningOfStatement(lineInfo->buffer, end);
    const std::string word(c, end - c);
    shared_ptr<GibbonConsoleJavascriptCompletionEvent> event(new GibbonConsoleJavascriptCompletionEvent(word));
    gibbonApp()->postEvent(event);
    const std::vector<std::string> alternatives = event->wait();
    int longest = 0;
    std::string shared;
    // fprintf(f, "Completing with [%s] => %d\n", word.c_str(), alternatives.size());
    for(std::vector<std::string>::const_iterator it = alternatives.begin(); it != alternatives.end(); ++it) {
        longest = std::max<int>(longest, it->size());
        if(it == alternatives.begin()) {
            shared = *it;
        } else {
            const size_t len = ::common(shared.c_str(), it->c_str());
            if(len < shared.size())
                shared.resize(len);
        }
    }
    // fprintf(f, "shared is [%s]\n", shared.c_str());
    // fclose(f);

    if(shared.empty()) {
        return CC_ERROR;
    } else if(shared != word) {
        el_insertstr(edit, shared.c_str() + word.size());
        return CC_REFRESH;
    } else if(alternatives.size()) {
        printAlternatives(alternatives, longest);
        return CC_REDISPLAY;
    }
    return CC_REFRESH;
}

unsigned GibbonConsole::readFromPipe() // always called from GibbonConsole::Run
{
    unsigned state = None;
    int ret;
    char ch;
    do {
        ch = '\0';
        ret = read(mPipe[0], &ch, 1);
        switch (ch) {
        case 'q':
            state |= Quit;
            break;
        case 'r':
            state |= Refresh;
            break;
        }
    } while(ret == 1 || (ret == -1 && errno == EINTR));
    return state;
}

#endif

GibbonConsoleSink::GibbonConsoleSink() : ConsoleSink()
{
    if(GibbonConfiguration::watchdogEnabled())
        setFlag(ConsoleSink::LogProcess);
}

void GibbonConsoleSink::receive(int logLevel, const std::string &msg)
{
    if(shared_ptr<GibbonConsole> console = gibbonApp()->console()) {
        console->consoleReceive(msg);
    } else {
        ConsoleSink::receive(logLevel, msg);
    }
}

void GibbonConsole::dumpMemory(const std::vector<std::string> &arguments)
{
    shared_ptr<GibbonConsole> that = gibbonConsole();
    std::string file;
    enum {
        ModeNone               = 0x0000,
        ModeJS                 = 0x0001,
        ModeJSAlloc            = 0x0002,
        ModeHeap               = 0x0004,
        ModeAlloc              = 0x0008,
        ModeThreads            = 0x0010,
        ModeStats              = 0x0020,
        ModeCount              = 0x0040,
        ModeFull               = 0x0080,
        ModeTCMalloc           = 0x0100,
        ModeTCMallocFree       = 0x0200,
        ModeTCMallocAggressive = 0x0400,
        ModeOSAllocator        = 0x0800,
        ModeStrings            = 0x1000,
        ModeRaw                = 0x2000
    };
    int mode = ModeNone;
    int timer = -1;
    bool on = true;
    for(size_t i = 1; i < arguments.size(); ++i) {
        if(arguments[i] == "js") {
            mode |= ModeJS;
        } else if(arguments[i] == "osallocator") {
            mode |= ModeOSAllocator;
        } else if(arguments[i] == "strings") {
            mode |= ModeStrings;
        } else if(arguments[i] == "raw") {
            mode |= ModeRaw;
        } else if(arguments[i] == "full") {
            mode |= ModeFull;
        } else if(arguments[i] == "count") {
            mode |= ModeCount;
        } else if(arguments[i] == "heap") {
            mode |= ModeHeap;
        } else if(arguments[i] == "threads") {
            mode |= ModeThreads;
        } else if(arguments[i] == "alloc") {
            mode |= ModeAlloc;
        } else if(arguments[i] == "stats") {
            mode |= ModeStats;
        } else if(arguments[i] == "timer") {
            if(i + 1 < arguments.size()) {
                ++i;
                timer = atoi(arguments[i].c_str());
            }
        } else if(arguments[i] == "all") {
            mode |= 0xFFFFFFFF;
        } else if(arguments[i] == "tcmalloc") {
            mode |= ModeTCMalloc;
        } else if(arguments[i] == "gc") {
            mode |= ModeTCMallocFree;
        } else if(arguments[i] == "aggressive") {
            mode |= ModeTCMallocAggressive;
        } else if(arguments[i] == "off") {
            on = false;
        } else {
            file = arguments[i];
        }
    }
    if(mode == ModeJS)
        mode |= ModeAlloc;
    if(timer >= 0) {
        if(mode & ModeJS) {
            if(that->mDumpJSTimer)
                gibbonApp()->stopTimer(that->mDumpJSTimer);
            if(timer > 0) {
                that->mDumpJSTimer.reset(new DumpAllocTimer(timer * 1000, GibbonApplication::MallocJavascript));
                gibbonApp()->startTimer(that->mDumpJSTimer);
            }
        } else {
            if(that->mDumpMallinfoTimer)
                gibbonApp()->stopTimer(that->mDumpMallinfoTimer);
            if(timer > 0) {
                that->mDumpMallinfoTimer.reset(new DumpAllocTimer(timer * 1000, GibbonApplication::MallocSystem));
                gibbonApp()->startTimer(that->mDumpMallinfoTimer);
            }
        }
    }
    if(mode & ModeThreads)
        Log::warn(TRACE_UI_ENGINE, "Threads: ******\n%s\n******\n", gibbonApp()->stats(GibbonApplication::Stat_Threads).toJSON(true).c_str());
    if(mode & ModeStats)
        Log::warn(TRACE_UI_ENGINE, "Stats: ******\n%s\n******\n", gibbonApp()->stats(GibbonApplication::Stat_All).toJSON(true).c_str());
    if((mode & ModeJS) && timer == -1) {
        if(mode & ModeCount) {
            std::string result;
            gibbonApp()->getScriptEngine()->dumpCounts(result);
            std::map<std::string, HeapCountInfo> map;

            std::stringstream stream(result);
            std::string line;
            while(std::getline(stream, line)) {
                std::stringstream sline(line);
                std::vector<std::string> parts;
                std::string part;
                while(std::getline(sline, part, ',')) {
                    if(!part.empty())
                        parts.push_back(part);
                }
                assert(parts.size() >= 2);
                HeapCountInfo & info = map[parts[0]];
                info.count += 1;
                info.cellSize += atoi(parts[1].c_str());
                if(parts.size() >= 3) {
                    info.stringSize += atoi(parts[2].c_str());
                }
            }
            {
                std::ostringstream stream;
                size_t totalCount = 0;
                size_t totalCellSize = 0;
                size_t totalStringSize = 0;
                for(std::map<std::string, HeapCountInfo>::const_iterator it = map.begin(); it != map.end(); ++it) {
                    const HeapCountInfo & info(it->second);
                    stream << it->first << "," << info.count << "," << info.cellSize << "," << info.stringSize << "\n";
                    totalCount += info.count;
                    totalCellSize += info.cellSize;
                    totalStringSize += info.stringSize;
                }
                stream << "TOTAL," << totalCount << "," << totalCellSize << "," << totalStringSize << "\n";
                Log::warn(TRACE_UI_ENGINE) << "\n" << stream.str();
            }
            if(!file.empty()) {
                const std::string jsheap_file("jsheap_count_" + file);
                if(FILE *f = fopen(jsheap_file.c_str(), "w")) {
                    fprintf(f, "%s", result.c_str());
                    fclose(f);
                }
            }
        } else if(mode & ModeAlloc) {
            gibbonApp()->dumpMalloc(GibbonApplication::MallocJavascript);
        } else if(mode & ModeHeap) {
            if(!file.empty()) {
                const std::string jsheap_file("jsheap_" + file);
                if(FILE *f = fopen(jsheap_file.c_str(), "w")) {
                    gibbonApp()->getScriptEngine()->heap(f);
                    fclose(f);
                } else {
                    Log::error(TRACE_UI_ENGINE, "Couldn't open %s for writing", jsheap_file.c_str());
                }
            } else {
                Log::error(TRACE_UI_ENGINE, "You need to pass a filename for /memory js heap. Eg. /memory js heap foobar.dump");
            }
#ifdef NRDP_JSC_ENABLE_OSALLOCATOR_CALLBACKS
        } else if(mode & ModeStrings) {
            const ScriptEngine::StringsData data = gibbonApp()->getScriptEngine()->stringsData();
            Log::warn(TRACE_UI_ENGINE, "Strings: %d Bytes: %d Average: %.2f",
                      data.count, data.bytes, data.bytes ? static_cast<double>(data.bytes) / data.count : .0);
            if(!file.empty()) {
                const std::string jsstring_file("jsstrings_" + file);
                if(FILE *f = fopen(jsstring_file.c_str(), "w")) {
                    if(!gibbonApp()->getScriptEngine()->dumpStrings(f, mode & ModeRaw ? ScriptEngine::Raw : ScriptEngine::Encoded))
                        Log::error(TRACE_UI_ENGINE, "Couldn't write to %s", jsstring_file.c_str());
                    char buf[1024];
                    getcwd(buf, sizeof(buf));
                    Log::warn(TRACE_UI_ENGINE, "Wrote to file: %s/%s", buf, jsstring_file.c_str());
                } else {
                    Log::error(TRACE_UI_ENGINE, "Couldn't open %s for writing", jsstring_file.c_str());
                }
            }
#endif
        }
    } else if(!(mode & ModeJS) && (mode & ModeAlloc) && timer == -1) {
        gibbonApp()->dumpMalloc(GibbonApplication::MallocSystem);
        if(mode & ModeFull) {
            ullong system, metadata, freebytes;
            size_t stringCount, jsStringsSize, returned, committed, decommitted;
            GibbonApplication::instance()->getScriptEngine()->queryMalloc(&system, &metadata, &freebytes, &returned,
                                                                          &stringCount, &jsStringsSize,
                                                                          &committed, &decommitted);
            size_t /*tcmallocAllocated = 0, */tcmallocHeap = 0;
#ifdef HAVE_GIBBON_TCMALLOC
            MallocExtension* ext = MallocExtension::instance();
            assert(ext);
            ext->GetNumericProperty("generic.heap_size", &tcmallocHeap);
#endif
            Log::error(TRACE_UI_ENGINE, "Memory: FastMalloc: %llu, OSAllocator: %zu, TCMalloc: %zu, Strings: %zu/%zu",
                       system - returned, committed, tcmallocHeap, stringCount, jsStringsSize);
            Log::error(TRACE_UI_ENGINE, "Memory: Total: %llu", (system - returned) + committed + tcmallocHeap);
        }
    }
#ifdef NRDP_JSC_ENABLE_OSALLOCATOR_CALLBACKS
    if(mode & ModeOSAllocator) {
        gibbonApp()->getScriptEngine()->dumpOSAllocator(mode & ModeFull
                                                        ? ScriptEngine::OSAllocator_Full
                                                        : ScriptEngine::OSAllocator_Summary);
    }
#endif
    if(mode & ModeTCMallocFree) {
#ifdef HAVE_GIBBON_TCMALLOC
        Log::error(TRACE_UI_ENGINE, "Releasing tcmalloc memory");
        MallocExtension* ext = MallocExtension::instance();
        assert(ext);
        ext->ReleaseFreeMemory();
#else
        Log::error(TRACE_UI_ENGINE, "tcmalloc support not enabled");
#endif
    }
    if(mode & ModeTCMallocAggressive) {
#ifdef HAVE_GIBBON_TCMALLOC
        Log::error(TRACE_UI_ENGINE, "Setting aggressive %s", on ? "on" : "off");
        MallocExtension* ext = MallocExtension::instance();
        assert(ext);
        ext->SetNumericProperty("tcmalloc.aggressive_memory_decommit", on ? 1 : 0);
#else
        (void)on;
        Log::error(TRACE_UI_ENGINE, "tcmalloc support not enabled");
#endif
    }
    if(mode & ModeTCMalloc) {
#ifdef HAVE_GIBBON_TCMALLOC
        MallocExtension* ext = MallocExtension::instance();
        assert(ext);
        size_t allocated, heapSize, pageheapFree, pageheapUnmapped;
        if(!ext->GetNumericProperty("generic.current_allocated_bytes", &allocated)) {
            allocated = 0;
        }
        if(!ext->GetNumericProperty("generic.heap_size", &heapSize)) {
            heapSize = 0;
        }
        if(!ext->GetNumericProperty("tcmalloc.pageheap_free_bytes", &pageheapFree)) {
            pageheapFree = 0;
        }
        if(!ext->GetNumericProperty("tcmalloc.pageheap_unmapped_bytes", &pageheapUnmapped)) {
            pageheapUnmapped = 0;
        }

        Log::error(TRACE_UI_ENGINE, "TCMalloc:");
        Log::error(TRACE_UI_ENGINE, "  %zu bytes allocated", allocated);
        Log::error(TRACE_UI_ENGINE, "  %zu bytes in the heap", heapSize);
        Log::error(TRACE_UI_ENGINE, "  %zu bytes free in the page heap", pageheapFree);
        Log::error(TRACE_UI_ENGINE, "  %zu bytes unmapped in the page heap", pageheapUnmapped);

        char stats[16384];
        ext->GetStats(stats, sizeof(stats));
        Log::error(TRACE_UI_ENGINE, "Stats:");
        Log::error(TRACE_UI_ENGINE, "%s", stats);
#else
        Log::error(TRACE_UI_ENGINE, "tcmalloc support not enabled");
#endif
    }
}

std::string GibbonConsole::formatJS(const Variant &variant) const
{
    JSONFormatter formatter(JSONFormatter::Pretty
                            |JSONFormatter::DataBuffersAsArrays
                            |JSONFormatter::NullVariantToUndefined
                            |JSONFormatter::UnprettySmallObjects);
    formatter.setMaxLength(mJSMaxLength);
    return formatter.format(variant);
}
