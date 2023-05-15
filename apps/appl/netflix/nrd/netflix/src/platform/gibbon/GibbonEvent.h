/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef __GIBBONEVENT_H__
#define __GIBBONEVENT_H__

#include "Debug.h"
#include "Screen.h"
#include "GibbonApplication.h"
#include "GibbonConfiguration.h"

#include <nrdbase/Instrumentation.h>
#include <nrdbase/PerformanceMarker.h>
#include <nrdbase/Time.h>
#include <nrd/AppLog.h>

#include "Point.h"
#include <vector>
#include <ctype.h>

#ifndef GIBBON_KEY
#define GIBBON_KEY(x) x
#endif
#define GIBBON_KEY_ALIAS(x, y) GIBBON_KEY(x) = GIBBON_KEY(y)

namespace netflix {
namespace gibbon {

class GibbonEvent : public Application::Event
{
public:
    enum Type {
        Type_Unknown,
        Type_KeyPress,
        Type_KeyRelease,
        Type_KeyClick,
        Type_MouseMove,
        Type_MousePress,
        Type_MouseChange,
        Type_MouseRelease,
        Type_TouchMove,
        Type_TouchPress,
        Type_TouchRelease,
        Type_FocusChange,
    };

    inline GibbonEvent(GibbonEvent::Type t=GibbonEvent::Type_Unknown, const Time & time = Time::mono()) :
        Application::Event(t, HighPriority), mTime(time) { }

    virtual void eventFired();
    virtual std::string describe() const { return std::string("GibbonEvent"); }

    const Time mTime;
};

class FocusChangeEvent : public GibbonEvent
{
public:
    inline FocusChangeEvent(bool focus=true) : GibbonEvent(GibbonEvent::Type_FocusChange), mFocus(focus)
    {}

    bool mFocus;
};

class InputEvent : public GibbonEvent
{
public:
    enum Modifier
    {
        MODIFIER_NONE   = 0x00,
        MODIFIER_CTRL   = 0x01,
        MODIFIER_ALT    = 0x02,
        MODIFIER_SHIFT  = 0x04,
        MODIFIER_META   = 0x08,
        MODIFIER_KEYPAD = 0x10
    };

    class Scope {
    public:
        Scope() { }
        ~Scope() { flush(); }

        inline void sendEvent(InputEvent *event) { sendEvent(shared_ptr<InputEvent>(event)); } //takes ownership
        void sendEvent(const shared_ptr<InputEvent> &event);
    protected:
        void flush();
        shared_ptr<InputEvent> mEvent;
    };

    inline InputEvent(GibbonEvent::Type t=GibbonEvent::Type_Unknown, const Time & time = Time::mono()) : GibbonEvent(t, time)
    { sLastInputEventTime = mTime; }
    inline static Time getIdleTime()
    { return Time::mono()-sLastInputEventTime; }

    virtual std::string describe() const { return std::string("InputEvent"); }

private:
    static Time sLastInputEventTime;
};

class KeyEvent : public InputEvent
{
public:
    // these keys match WindowsKeyboardCodes.h from WebKit
    enum Key {
        GIBBON_KEY(KEY_UNKNOWN) = 0x00,
        GIBBON_KEY(KEY_LBUTTON) = 0x01,
        GIBBON_KEY(KEY_RBUTTON) = 0x02,
        GIBBON_KEY(KEY_CANCEL) = 0x03,
        GIBBON_KEY(KEY_MBUTTON) = 0x04,
        GIBBON_KEY(KEY_XBUTTON1) = 0x05,
        GIBBON_KEY(KEY_XBUTTON2) = 0x06,
        GIBBON_KEY(KEY_BACKSPACE) = 0x08,
        GIBBON_KEY(KEY_TAB) = 0x09,
        GIBBON_KEY(KEY_CLEAR) = 0x0C,
        GIBBON_KEY(KEY_RETURN) = 0x0D, GIBBON_KEY_ALIAS(KEY_ENTER, KEY_RETURN),
        GIBBON_KEY(KEY_SHIFT) = 0x10,
        GIBBON_KEY(KEY_CONTROL) = 0x11, // CTRL key
        GIBBON_KEY(KEY_MENU) = 0x12, GIBBON_KEY_ALIAS(KEY_ALT, KEY_MENU), GIBBON_KEY_ALIAS(KEY_META, KEY_MENU), // ALT key
        GIBBON_KEY(KEY_PAUSE) = 0x13,
        GIBBON_KEY(KEY_CAPITAL) = 0x14, GIBBON_KEY_ALIAS(KEY_CAPSLOCK, KEY_CAPITAL), // CAPS LOCK key
        GIBBON_KEY(KEY_KANA) = 0x15, // Input Method Editor (IME) Kana mode
        GIBBON_KEY(KEY_HANGUL) = 0x15, // IME Hangul mode
        GIBBON_KEY(KEY_JUNJA) = 0x17, // IME Junja mode
        GIBBON_KEY(KEY_FINAL) = 0x18, // IME final mode
        GIBBON_KEY(KEY_HANJA) = 0x19, // IME Hanja mode
        GIBBON_KEY(KEY_KANJI) = 0x19, // IME Kanji mode
        GIBBON_KEY(KEY_ESCAPE) = 0x1B, // ESC key
        GIBBON_KEY(KEY_CONVERT) = 0x1C, // IME convert
        GIBBON_KEY(KEY_NONCONVERT) = 0x1D, // IME nonconvert
        GIBBON_KEY(KEY_ACCEPT) = 0x1E, // IME accept
        GIBBON_KEY(KEY_MODECHANGE) = 0x1F, // IME mode change request
        GIBBON_KEY(KEY_SPACE) = 0x20, // SPACE key
        GIBBON_KEY(KEY_PRIOR) = 0x21, GIBBON_KEY_ALIAS(KEY_PAGE_UP, KEY_PRIOR), // PAGE UP key
        GIBBON_KEY(KEY_NEXT) = 0x22, GIBBON_KEY_ALIAS(KEY_PAGE_DOWN, KEY_NEXT), // PAGE DOWN key
        GIBBON_KEY(KEY_END) = 0x23, // END key
        GIBBON_KEY(KEY_HOME) = 0x24, // HOME key
        GIBBON_KEY(KEY_LEFT) = 0x25, // LEFT ARROW key
        GIBBON_KEY(KEY_UP) = 0x26, // UP ARROW key
        GIBBON_KEY(KEY_RIGHT) = 0x27, // RIGHT ARROW key
        GIBBON_KEY(KEY_DOWN) = 0x28, // DOWN ARROW key
        GIBBON_KEY(KEY_SELECT) = 0x29, // SELECT key mapped to KEY_RETURN
        GIBBON_KEY(KEY_PRINT) = 0x2A, // PRINT key
        GIBBON_KEY(KEY_EXECUTE) = 0x2B, // EXECUTE key
        GIBBON_KEY(KEY_SNAPSHOT) = 0x2C, // PRINT SCREEN key
        GIBBON_KEY(KEY_INSERT) = 0x2D, // INS key
        GIBBON_KEY(KEY_DELETE) = 0x2E, // DEL key
        GIBBON_KEY(KEY_HELP) = 0x2F, // HELP key
        GIBBON_KEY(KEY_0) = 0x30,
        GIBBON_KEY(KEY_1) = 0x31,
        GIBBON_KEY(KEY_2) = 0x32,
        GIBBON_KEY(KEY_3) = 0x33,
        GIBBON_KEY(KEY_4) = 0x34,
        GIBBON_KEY(KEY_5) = 0x35,
        GIBBON_KEY(KEY_6) = 0x36,
        GIBBON_KEY(KEY_7) = 0x37,
        GIBBON_KEY(KEY_8) = 0x38,
        GIBBON_KEY(KEY_9) = 0x39,
        GIBBON_KEY(KEY_A) = 0x41,
        GIBBON_KEY(KEY_B) = 0x42,
        GIBBON_KEY(KEY_C) = 0x43,
        GIBBON_KEY(KEY_D) = 0x44,
        GIBBON_KEY(KEY_E) = 0x45,
        GIBBON_KEY(KEY_F) = 0x46,
        GIBBON_KEY(KEY_G) = 0x47,
        GIBBON_KEY(KEY_H) = 0x48,
        GIBBON_KEY(KEY_I) = 0x49,
        GIBBON_KEY(KEY_J) = 0x4A,
        GIBBON_KEY(KEY_K) = 0x4B,
        GIBBON_KEY(KEY_L) = 0x4C,
        GIBBON_KEY(KEY_M) = 0x4D,
        GIBBON_KEY(KEY_N) = 0x4E,
        GIBBON_KEY(KEY_O) = 0x4F,
        GIBBON_KEY(KEY_P) = 0x50,
        GIBBON_KEY(KEY_Q) = 0x51,
        GIBBON_KEY(KEY_R) = 0x52,
        GIBBON_KEY(KEY_S) = 0x53,
        GIBBON_KEY(KEY_T) = 0x54,
        GIBBON_KEY(KEY_U) = 0x55,
        GIBBON_KEY(KEY_V) = 0x56,
        GIBBON_KEY(KEY_W) = 0x57,
        GIBBON_KEY(KEY_X) = 0x58,
        GIBBON_KEY(KEY_Y) = 0x59,
        GIBBON_KEY(KEY_Z) = 0x5A,
        GIBBON_KEY(KEY_LWIN) = 0x5B, GIBBON_KEY_ALIAS(KEY_SUPER_L, KEY_LWIN), GIBBON_KEY_ALIAS(KEY_HYPER_L, KEY_LWIN), // Left Windows key (Microsoft Natural keyboard)
        GIBBON_KEY(KEY_RWIN) = 0x5C, GIBBON_KEY_ALIAS(KEY_SUPER_R, KEY_RWIN), GIBBON_KEY_ALIAS(KEY_HYPER_R, KEY_RWIN), // Right Windows key (Natural keyboard)
        GIBBON_KEY(KEY_APPS) = 0x5D, // Applications key (Natural keyboard)
        GIBBON_KEY(KEY_SLEEP) = 0x5F, // Computer Sleep key
        // Num pad keys
        GIBBON_KEY(KEY_NUMPAD0) = 0x60,
        GIBBON_KEY(KEY_NUMPAD1) = 0x61,
        GIBBON_KEY(KEY_NUMPAD2) = 0x62,
        GIBBON_KEY(KEY_NUMPAD3) = 0x63,
        GIBBON_KEY(KEY_NUMPAD4) = 0x64,
        GIBBON_KEY(KEY_NUMPAD5) = 0x65,
        GIBBON_KEY(KEY_NUMPAD6) = 0x66,
        GIBBON_KEY(KEY_NUMPAD7) = 0x67,
        GIBBON_KEY(KEY_NUMPAD8) = 0x68,
        GIBBON_KEY(KEY_NUMPAD9) = 0x69,
        GIBBON_KEY(KEY_MULTIPLY) = 0x6A,
        GIBBON_KEY(KEY_ADD) = 0x6B,
        GIBBON_KEY(KEY_SEPARATOR) = 0x6C,
        GIBBON_KEY(KEY_SUBTRACT) = 0x6D,
        GIBBON_KEY(KEY_DECIMAL) = 0x6E,
        GIBBON_KEY(KEY_DIVIDE) = 0x6F,
        GIBBON_KEY(KEY_F1) = 0x70,
        GIBBON_KEY(KEY_F2) = 0x71,
        GIBBON_KEY(KEY_F3) = 0x72,
        GIBBON_KEY(KEY_F4) = 0x73,
        GIBBON_KEY(KEY_F5) = 0x74,
        GIBBON_KEY(KEY_F6) = 0x75,
        GIBBON_KEY(KEY_F7) = 0x76,
        GIBBON_KEY(KEY_F8) = 0x77,
        GIBBON_KEY(KEY_F9) = 0x78,
        GIBBON_KEY(KEY_F10) = 0x79,
        GIBBON_KEY(KEY_F11) = 0x7A,
        GIBBON_KEY(KEY_F12) = 0x7B,
        GIBBON_KEY(KEY_F13) = 0x7C,
        GIBBON_KEY(KEY_F14) = 0x7D,
        GIBBON_KEY(KEY_F15) = 0x7E,
        GIBBON_KEY(KEY_F16) = 0x7F,
        GIBBON_KEY(KEY_F17) = 0x80,
        GIBBON_KEY(KEY_F18) = 0x81,
        GIBBON_KEY(KEY_F19) = 0x82,
        GIBBON_KEY(KEY_F20) = 0x83,
        GIBBON_KEY(KEY_F21) = 0x84,
        GIBBON_KEY(KEY_F22) = 0x85,
        GIBBON_KEY(KEY_F23) = 0x86,
        GIBBON_KEY(KEY_F24) = 0x87,
        GIBBON_KEY(KEY_NUMLOCK) = 0x90,
        GIBBON_KEY(KEY_SCROLL) = 0x91, GIBBON_KEY_ALIAS(KEY_SCROLLLOCK, KEY_SCROLL),
        GIBBON_KEY(KEY_FJ_JISHO) = 0x92,
        GIBBON_KEY(KEY_LSHIFT) = 0xA0,
        GIBBON_KEY(KEY_RSHIFT) = 0xA1,
        GIBBON_KEY(KEY_LCONTROL) = 0xA2,
        GIBBON_KEY(KEY_RCONTROL) = 0xA3,
        GIBBON_KEY(KEY_LMENU) = 0xA4,
        GIBBON_KEY(KEY_RMENU) = 0xA5,
        GIBBON_KEY(KEY_BROWSER_BACK) = 0xA6, // Windows 2000/XP: Browser Back key
        GIBBON_KEY(KEY_BROWSER_FORWARD) = 0xA7, // Windows 2000/XP: Browser Forward key
        GIBBON_KEY(KEY_BROWSER_REFRESH) = 0xA8, // Windows 2000/XP: Browser Refresh key
        GIBBON_KEY(KEY_BROWSER_STOP) = 0xA9, // Windows 2000/XP: Browser Stop key
        GIBBON_KEY(KEY_BROWSER_SEARCH) = 0xAA, // Windows 2000/XP: Browser Search key
        GIBBON_KEY(KEY_BROWSER_FAVORITES) = 0xAB, // Windows 2000/XP: Browser Favorites key
        GIBBON_KEY(KEY_BROWSER_HOME) = 0xAC, // Windows 2000/XP: Browser Start and Home key
        GIBBON_KEY(KEY_VOLUME_MUTE) = 0xAD, // Windows 2000/XP: Volume Mute key
        GIBBON_KEY(KEY_VOLUME_DOWN) = 0xAE, // Windows 2000/XP: Volume Down key
        GIBBON_KEY(KEY_VOLUME_UP) = 0xAF, // Windows 2000/XP: Volume Up key
        GIBBON_KEY(KEY_MEDIA_NEXT_TRACK) = 0xB0, // Windows 2000/XP: Next Track key
        GIBBON_KEY(KEY_MEDIA_PREV_TRACK) = 0xB1, // Windows 2000/XP: Previous Track key
        GIBBON_KEY(KEY_MEDIA_STOP) = 0xB2, // Windows 2000/XP: Stop Media key
        GIBBON_KEY(KEY_MEDIA_PLAY_PAUSE) = 0xB3, // Windows 2000/XP: Play/Pause Media key
        GIBBON_KEY(KEY_MEDIA_LAUNCH_MAIL) = 0xB4, // Windows 2000/XP: Start Mail key
        GIBBON_KEY(KEY_MEDIA_LAUNCH_MEDIA_SELECT) = 0xB5, // Windows 2000/XP: Select Media key
        GIBBON_KEY(KEY_MEDIA_LAUNCH_APP1) = 0xB6, // KEY_LAUNCH_APP1 (B6) Windows 2000/XP: Start Application 1 key
        GIBBON_KEY(KEY_MEDIA_LAUNCH_APP2) = 0xB7, // KEY_LAUNCH_APP2 (B7) Windows 2000/XP: Start Application 2 key
        // Used for miscellaneous characters; it can vary by keyboard. Windows 2000/XP: For the US standard keyboard, the ';:' key
        GIBBON_KEY(KEY_OEM_1) = 0xBA, // Windows 2000/XP: For any country/region, the '+' key
        GIBBON_KEY(KEY_OEM_PLUS) = 0xBB, // Windows 2000/XP: For any country/region, the ',' key
        GIBBON_KEY(KEY_OEM_COMMA) = 0xBC, // Windows 2000/XP: For any country/region, the '-' key
        GIBBON_KEY(KEY_OEM_MINUS) = 0xBD, // Windows 2000/XP: For any country/region, the '.' key
        GIBBON_KEY(KEY_OEM_PERIOD) = 0xBE, // KEY_OEM_2 (BF) Used for miscellaneous characters; it can vary by keyboard. Windows 2000/XP: For the US standard keyboard, the '/?' key
        GIBBON_KEY(KEY_OEM_2) = 0xBF, // KEY_OEM_3 (C0) Used for miscellaneous characters; it can vary by keyboard. Windows 2000/XP: For the US standard keyboard, the '`~' key
        GIBBON_KEY(KEY_OEM_3) = 0xC0, // KEY_OEM_4 (DB) Used for miscellaneous characters; it can vary by keyboard. Windows 2000/XP: For the US standard keyboard, the '[{' key
        GIBBON_KEY(KEY_OEM_4) = 0xDB, GIBBON_KEY_ALIAS(KEY_RED, KEY_OEM_4), // KEY_OEM_5 (DC) Used for miscellaneous characters; it can vary by keyboard. Windows 2000/XP: For the US standard keyboard, the '\|' key
        GIBBON_KEY(KEY_OEM_5) = 0xDC, GIBBON_KEY_ALIAS(KEY_GREEN, KEY_OEM_5), // KEY_OEM_6 (DD) Used for miscellaneous characters; it can vary by keyboard. Windows 2000/XP: For the US standard keyboard, the ']}' key
        GIBBON_KEY(KEY_OEM_6) = 0xDD, GIBBON_KEY_ALIAS(KEY_BLUE, KEY_OEM_6), // KEY_OEM_7 (DE) Used for miscellaneous characters; it can vary by keyboard. Windows 2000/XP: For the US standard keyboard, the 'single-quote/double-quote' key
        GIBBON_KEY(KEY_OEM_7) = 0xDE, GIBBON_KEY_ALIAS(KEY_YELLOW, KEY_OEM_7), // KEY_OEM_8 (DF) Used for miscellaneous characters; it can vary by keyboard.
        GIBBON_KEY(KEY_OEM_8) = 0xDF, // KEY_OEM_102 (E2) Windows 2000/XP: Either the angle bracket key or the backslash key on the RT 102-key keyboard
        GIBBON_KEY(KEY_OEM_AX) = 0xE1,
        GIBBON_KEY(KEY_OEM_102) = 0xE2, // Windows 95/98/Me, Windows NT 4.0, Windows 2000/XP: IME PROCESS key
        GIBBON_KEY(KEY_ICO_HELP) = 0xE3,
        GIBBON_KEY(KEY_ICO_00) = 0xE4,
        GIBBON_KEY(KEY_PROCESSKEY) = 0xE5,
        GIBBON_KEY(KEY_ICO_CLEAR) = 0xE6,
        // Windows 2000/XP: Used to pass Unicode characters as if they were
        // keystrokes. The KEY_PACKET key is the low word of a 32-bit Virtual Key
        // value used for non-keyboard input methods. For more information, see
        // Remark in KEYBDINPUT,SendInput, WM_KEYDOWN, and WM_KEYUP
        GIBBON_KEY(KEY_PACKET) = 0xE7,
        GIBBON_KEY(KEY_OEM_RESET) = 0xE9,
        GIBBON_KEY(KEY_OEM_JUMP) = 0xEA,
        GIBBON_KEY(KEY_OEM_PA1) = 0xEB,
        GIBBON_KEY(KEY_OEM_PA2) = 0xEC,
        GIBBON_KEY(KEY_OEM_PA3) = 0xED,
        GIBBON_KEY(KEY_OEM_WSCTRL) = 0xEE,
        GIBBON_KEY(KEY_OEM_CUSEL) = 0xEF,
        GIBBON_KEY(KEY_OEM_ATTN) = 0xF0,
        GIBBON_KEY(KEY_OEM_FINISH) = 0xF1,
        GIBBON_KEY(KEY_OEM_COPY) = 0xF2,
        GIBBON_KEY(KEY_OEM_AUTO) = 0xF3,
        GIBBON_KEY(KEY_OEM_ENLW) = 0xF4,
        GIBBON_KEY(KEY_OEM_BACKTAB) = 0xF5,
        GIBBON_KEY(KEY_ATTN) = 0xF6, // Attn key
        GIBBON_KEY(KEY_CRSEL) = 0xF7, // CrSel key
        GIBBON_KEY(KEY_EXSEL) = 0xF8, // ExSel key
        GIBBON_KEY(KEY_EREOF) = 0xF9, // Erase EOF key
        GIBBON_KEY(KEY_PLAY) = 0xFA, // Play key
        GIBBON_KEY(KEY_ZOOM) = 0xFB, // Zoom key
        GIBBON_KEY(KEY_NONAME) = 0xFC, // Reserved for future use
        GIBBON_KEY(KEY_PA1) = 0xFD, // KEY_PA1 (FD) PA1 key
        GIBBON_KEY(KEY_OEM_CLEAR) = 0xFE, // Clear key

        // Non standard keys added by Netflix for completeness
        GIBBON_KEY(KEY_POWER) = 0x1000,
        GIBBON_KEY(KEY_MEDIA_FAST_FORWARD) = 0x1001,
        GIBBON_KEY(KEY_MEDIA_REWIND) = 0x1002,
        GIBBON_KEY(KEY_MEDIA_RECORD) = 0x1003,
        GIBBON_KEY(KEY_CHANNEL_UP) = 0x1004,
        GIBBON_KEY(KEY_CHANNEL_DOWN) = 0x1005,

        GIBBON_KEY(KEY_FIRST_UNKNOWN) = 10000
    };

    inline static Key keyFromAscii(char ascii)
    {
        if (std::isalnum(ascii))
            return static_cast<Key>(std::toupper(ascii));
        return GIBBON_KEY(KEY_UNKNOWN);
    }

    inline KeyEvent(GibbonEvent::Type t=GibbonEvent::Type_Unknown, unsigned k=GIBBON_KEY(KEY_UNKNOWN),
                    const std::string &text = std::string(), unsigned int m=MODIFIER_NONE,
                    bool repeat=false, int count=1, const std::string &uiKey = std::string())
        : InputEvent(t), mKey(k), mText(text), mCount(count), mRepeat(repeat), mModifiers(m), mUIKey(uiKey)
    {
        INSTRUMENTATION_INTERVAL_SWITCHED_START(INSTRUMENTATION_CTX("gibbon.key-event" , this ), Variant::StringMap("type",t)("key",k)("mod",m)("repeat",repeat)("text", text)("count",count));
    }
    inline ~KeyEvent() {
        INSTRUMENTATION_INTERVAL_SWITCHED_CANCEL(INSTRUMENTATION_CTX("gibbon.key-event", this));
    }

    virtual void eventFired() {
        PERFORMANCE_MARKER_SCOPED("keyevent.fired");
        INSTRUMENTATION_INTERVAL_SWITCHED_END(INSTRUMENTATION_CTX("gibbon.key-event", this));
#ifdef BUILD_DEBUG
        {
            if((mModifiers & KeyEvent::MODIFIER_CTRL) && mKey == GIBBON_KEY(KEY_HOME)) {
                if(type() == GibbonEvent::Type_KeyRelease) {
                    GibbonApplication::instance()->reloadCaches();
                    GibbonApplication::instance()->reload(GibbonApplication::Location_Current);
                }
                return;
            } else if((mModifiers & KeyEvent::MODIFIER_ALT) && mKey == GIBBON_KEY(KEY_HOME)) {
                if(type() == GibbonEvent::Type_KeyRelease) {
                    GibbonApplication::instance()->reloadCaches();
                    GibbonApplication::instance()->reload(GibbonApplication::Location_Base);
                }
                return;
            } else if((mModifiers & KeyEvent::MODIFIER_CTRL) && mKey == GIBBON_KEY(KEY_END)) {
                if(type() == GibbonEvent::Type_KeyRelease)
                    nrdApp()->scheduleQuit();
                return;
            } else if((mModifiers & KeyEvent::MODIFIER_ALT) && mKey == GIBBON_KEY(KEY_END)) {
                if(type() == GibbonEvent::Type_KeyRelease)
                    nrdApp()->setSuspended(!nrdApp()->isSuspended(), Variant("keyboard"));
                return;
            }
        }
#endif
        GibbonEvent::eventFired();
    }

    unsigned mKey;
    std::string mText;
    int mCount;
    bool mRepeat;
    unsigned int mModifiers;
    std::string mUIKey;
};

class MouseEvent : public InputEvent
{
public:
    enum Button
    {
        BUTTON_NONE   = 0x00,
        BUTTON_LEFT   = 0x01,
        BUTTON_MIDDLE = 0x02,
        BUTTON_RIGHT  = 0x04
    };
    typedef unsigned int Buttons; // combination of Button flags

    inline MouseEvent(GibbonEvent::Type t=GibbonEvent::Type_Unknown, Point pos = Point(-1, -1),
                      Button button = BUTTON_NONE, Buttons buttons = 0, unsigned int modifiers = 0)
        : InputEvent(t), mPos(pos), mButton(button), mShowCursor(false), mButtons(buttons), mModifiers(modifiers)
    {}

    static inline Point getLastPosition() { return sLastPosition; }

    virtual void eventFired() {
        PERFORMANCE_MARKER_SCOPED("mouseevent.fired");
        {
            ScopedMutex _lock(Widget::lock());
            sLastPosition = mPos;
            if(mShowCursor) {
                static Widget::WeakPtr mouse_overlay;
                if(Widget::SharedPtr mo = mouse_overlay.lock()) {
                    mo->setX(mPos.x);
                    mo->setY(mPos.y);
                } else {
                    mouse_overlay = GibbonApplication::instance()->getScreen()->createOverlay(Rect(mPos, 5, 5));
                }
            }
            if(type() == GibbonEvent::Type_MouseRelease && mButton == BUTTON_RIGHT) {
                const std::list<Widget::SharedPtr> widgets = GibbonApplication::instance()->getScreen()->widgetsAt(mPos);
#ifdef BUILD_DEBUG
                for(std::list<Widget::SharedPtr>::const_iterator it = widgets.begin(); it != widgets.end(); ++it)
                    Log::error(TRACE_UI_ENGINE, "%s: %s", mPos.toString().c_str(), (*it)->describe(true).c_str());
                Log::warn(TRACE_UI_ENGINE, "Widgets: %zu (%d, %d)", widgets.size(), (int)mPos.x, (int)mPos.y);
#endif
                if(!widgets.empty()) {                 // notify the debugger bridge
                    if(shared_ptr<GibbonBridge> bridge = GibbonApplication::instance()->getBridge()) {
                        if(shared_ptr<DebuggerBridge> debugger = bridge->getDebugger())
                            debugger->notifyClicked(widgets);
                    }
                }
            }
        }
        GibbonEvent::eventFired();
    }

    Point mPos;
    Button mButton;
    bool mShowCursor;
    Buttons mButtons;
    unsigned int mModifiers;
    static Point sLastPosition;
};

class MouseChangeEvent : public InputEvent
{
public:
    inline MouseChangeEvent(bool visible=true) :
        InputEvent(GibbonEvent::Type_MouseChange), mVisible(visible)
    {}

    bool mVisible;
};

class TouchEvent : public InputEvent
{
public:
    inline TouchEvent(GibbonEvent::Type t=GibbonEvent::Type_Unknown)
        : InputEvent(t), mPoints()
    {}

    virtual void eventFired() {
        PERFORMANCE_MARKER_SCOPED("mouseevent.fired");
        ScopedMutex _lock(Widget::lock());
        InputEvent::eventFired();
    }

    struct TouchPoint
    {
        int mID;
        Point mPos;
        float mPressure;
    };

    std::vector<TouchPoint> mPoints;
};

}} // namespace netflix::gibbon

#endif /* __GIBBONEVENT_H__ */
