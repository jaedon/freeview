/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include <Screen.h>
#include <InputManager.h>
#include <GibbonApplication.h>
#include <OpenGLContext.h>

#include <nrd/AppLog.h>
#include <nrdbase/Time.h>

#include <errno.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <X11/XKBlib.h>

using namespace netflix;
using namespace netflix::gibbon;

class InputManagerX11Private
{
public:
    InputManagerX11Private()
        : x11Display(0), fd(0)
    {
        const int ret = ::pipe(pipe);
        (void)ret;
        assert(!ret);
    }
    ~InputManagerX11Private()
    {
#ifndef GIBBON_GRAPHICSENGINE_GLES2
        XCloseDisplay(reinterpret_cast<Display*>(display));
#endif
        close(pipe[0]);
        close(pipe[1]);
    }

    void handleKeyEvent(XEvent &event);
    void handleMouseEvent(XEvent &event);

    Display *display()
    {
        if (!x11Display) {
            OpenGLContext* ctx = OpenGLContext::primary();
            if (!ctx) {
                Log::error(TRACE_GIBBON_INPUT, "No X11 display available");
                return 0;
            }
            x11Display = static_cast<Display*>(ctx->display());
            assert(x11Display);
            fd = XConnectionNumber(x11Display);
            assert(fd);
        }
        return x11Display;
    }

    int pipe[2];
    Display *x11Display;
    int fd;
};

void InputManager::init()
{
    mPriv.reset(new InputManagerX11Private);
}

void InputManager::cleanup()
{
    mPriv.reset();
}

static Bool findNextPress(Display *, XEvent *event, XPointer userData)
{
    if (event->type == KeyPress) {
        *reinterpret_cast<bool*>(userData) = true;
        return true;
    }
    return false;
}

static KeyEvent::Key keySymToKey(KeySym sym)
{
    KeyEvent::Key code = KeyEvent::KEY_UNKNOWN;
    switch (sym) {
    case XK_Return: code = KeyEvent::KEY_RETURN; break;
    case XK_End: code = KeyEvent::KEY_END; break;
    case XK_Home: code = KeyEvent::KEY_HOME; break;
    case XK_F1: code = KeyEvent::KEY_F1; break;
    case XK_F2: code = KeyEvent::KEY_F2; break;
    case XK_F3: code = KeyEvent::KEY_F3; break;
    case XK_F4: code = KeyEvent::KEY_F4; break;
    case XK_F5: code = KeyEvent::KEY_F5; break;
    case XK_F6: code = KeyEvent::KEY_F6; break;
    case XK_F7: code = KeyEvent::KEY_F7; break;
    case XK_F8: code = KeyEvent::KEY_F8; break;
    case XK_F9: code = KeyEvent::KEY_F9; break;
    case XK_F10: code = KeyEvent::KEY_F10; break;
    case XK_F11: code = KeyEvent::KEY_F11; break;
    case XK_F12: code = KeyEvent::KEY_F12; break;
    case XK_F13: code = KeyEvent::KEY_F13; break;
    case XK_F14: code = KeyEvent::KEY_F14; break;
    case XK_F15: code = KeyEvent::KEY_F15; break;
    case XK_F16: code = KeyEvent::KEY_F16; break;
    case XK_F17: code = KeyEvent::KEY_F17; break;
    case XK_F18: code = KeyEvent::KEY_F18; break;
    case XK_F19: code = KeyEvent::KEY_F19; break;
    case XK_F20: code = KeyEvent::KEY_F20; break;
    case XK_F21: code = KeyEvent::KEY_F21; break;
    case XK_F22: code = KeyEvent::KEY_F22; break;
    case XK_F23: code = KeyEvent::KEY_F23; break;
    case XK_F24: code = KeyEvent::KEY_F24; break;
    case XK_KP_Left:
    case XK_Left: code = KeyEvent::KEY_LEFT; break;
    case XK_KP_Up:
    case XK_Up: code = KeyEvent::KEY_UP; break;
    case XK_KP_Right:
    case XK_Right: code = KeyEvent::KEY_RIGHT; break;
    case XK_KP_Down:
    case XK_Down: code = KeyEvent::KEY_DOWN; break;
    case XK_BackSpace: code = KeyEvent::KEY_BACKSPACE; break;
    case XK_Shift_L:
    case XK_Shift_R: return KeyEvent::KEY_SHIFT;
    case XK_Control_L:
    case XK_Control_R: return KeyEvent::KEY_CONTROL;
    case XK_Alt_L:
    case XK_Alt_R: return KeyEvent::KEY_ALT;
    case XK_Meta_L:
    case XK_Meta_R: return KeyEvent::KEY_META;
    case XK_Super_L: return KeyEvent::KEY_SUPER_L;
    case XK_Super_R: return KeyEvent::KEY_SUPER_R;
    case XK_Hyper_L: return KeyEvent::KEY_HYPER_L;
    case XK_Hyper_R: return KeyEvent::KEY_HYPER_R;
    case XK_Caps_Lock: return KeyEvent::KEY_CAPSLOCK;
    case XK_Num_Lock: return KeyEvent::KEY_NUMLOCK;
    case XK_Scroll_Lock: return KeyEvent::KEY_SCROLLLOCK;
    case XK_Escape: return KeyEvent::KEY_ESCAPE;
    case XK_Tab: return KeyEvent::KEY_TAB;
    case XK_Insert: return KeyEvent::KEY_INSERT;
    case XK_Delete: return KeyEvent::KEY_DELETE;
    case XK_Prior: return KeyEvent::KEY_PRIOR;
    case XK_Next: return KeyEvent::KEY_NEXT;
    case XK_Print: return KeyEvent::KEY_PRINT;
    case XK_Pause: return KeyEvent::KEY_PAUSE;
    default:
        break;
    }
    return code;
}

static inline unsigned int buttons(int state)
{
    unsigned int buttons = 0;
    if (state & Button1Mask)
        buttons |= MouseEvent::BUTTON_LEFT;
    if (state & Button2Mask)
        buttons |= MouseEvent::BUTTON_MIDDLE;
    if (state & Button3Mask)
        buttons |= MouseEvent::BUTTON_MIDDLE;
    return buttons;
}

static inline MouseEvent::Button button(int button)
{
    switch (button) {
    case Button1: return MouseEvent::BUTTON_LEFT;
    case Button2: return MouseEvent::BUTTON_MIDDLE;
    case Button3: return MouseEvent::BUTTON_RIGHT;
    default:
        break;
    }
    return MouseEvent::BUTTON_NONE;
}

static inline unsigned int modifiers(int state)
{
    unsigned int modifiers = 0;
    if (state & ShiftMask)
        modifiers |= KeyEvent::MODIFIER_SHIFT;
    if (state & ControlMask)
        modifiers |= KeyEvent::MODIFIER_CTRL;
    if (state & (Mod1Mask|Mod2Mask|Mod3Mask|Mod4Mask|Mod5Mask))
        modifiers |= KeyEvent::MODIFIER_META;
    return modifiers;
}

void InputManager::moveMouse(const Point &)
{

}

void InputManager::waitForInput(llong mseconds)
{
    Display *display = mPriv->display();
    if(!display)
        return;

    int pending = XPending(display);

    if (!pending && mseconds) {
        struct timeval tv;
        struct timeval * tvp = 0;
        if (mseconds >= 0) {
            tv.tv_sec = mseconds / 1000;
            tv.tv_usec = (mseconds % 1000) * MicroSecondsPerMillisecond;
            tvp = &tv;
        }
        fd_set readset;
        FD_ZERO(&readset);
        FD_SET(mPriv->fd, &readset);
        FD_SET(mPriv->pipe[0], &readset);
        int s;
        do {
            s = select(std::max(mPriv->fd, mPriv->pipe[0]) + 1, &readset, 0, 0, tvp);
        } while (s == -1 && errno == EINTR);
        switch (s) {
        case -1:
            Log::error(TRACE_GIBBON_INPUT, "Select returned -1, errno: %d", errno);
            break;
        case 0:
            break;
        default:
            if (FD_ISSET(mPriv->pipe[0], &readset)) {
                char buff;
                size_t size;
                size = read(mPriv->pipe[0], &buff, 1);
                (void)size;
            }
            if (FD_ISSET(mPriv->fd, &readset)) {
                pending = XPending(display);
            }
        }
    }

    while (pending) {
        XEvent event;
        XNextEvent(display, &event);

        switch (event.type) {
        case Expose:
            GibbonApplication::instance()->getScreen()->update();
            break;

        case ClientMessage:
            GibbonApplication::instance()->quit();
            return;

        case KeyPress:
        case KeyRelease:
            mPriv->handleKeyEvent(event);
            break;
        case ButtonPress:
        case ButtonRelease:
        case MotionNotify:
            mPriv->handleMouseEvent(event);
            break;
        default:
            break;
        }
        pending = XPending(display);
    }
}

void InputManagerX11Private::handleKeyEvent(XEvent &event)
{
    static unsigned int lastRepeatCode = 0;
    const unsigned int modifiers = ::modifiers(event.xkey.state);

    char ascii = '\0';
    KeySym sym = 0;
    std::string text;
    const int result = XLookupString(&event.xkey, &ascii, sizeof(ascii), &sym, 0);
    int key = KeyEvent::KEY_UNKNOWN;
    if (result) {
        if (ascii) {
            text = ascii;
            key = KeyEvent::keyFromAscii(ascii);
        }
    } else {
        sym = XkbKeycodeToKeysym(x11Display, event.xkey.keycode, 0, 0);
    }
    if (key == KeyEvent::KEY_UNKNOWN)
        key = keySymToKey(sym);
    if (key == KeyEvent::KEY_UNKNOWN) {
        if (text.empty())
            return;
        key = KeyEvent::KEY_FIRST_UNKNOWN + sym;
    }
    NTRACE(TRACE_GIBBON_INPUT, "Mapped(id) 0x%x %d/%d -- %d (%u/%p)\n",
           event.xkey.keycode, event.type, ascii, key, lastRepeatCode, &event);

    bool autoRepeat = false;
    if (event.type == KeyPress) {
        autoRepeat = event.xkey.keycode == lastRepeatCode;
        lastRepeatCode = 0;
    } else {
        XEvent nextEvent;
        bool ok = false;
        if (XCheckIfEvent(x11Display, &nextEvent, findNextPress, reinterpret_cast<char*>(&ok)))
            XPutBackEvent(x11Display, &nextEvent);
        if (ok) {
            if (nextEvent.xkey.time - event.xkey.time <= 10 && event.xkey.keycode == nextEvent.xkey.keycode) {
                autoRepeat = true;
                lastRepeatCode = event.xkey.keycode;
            }
        }
    }

    GibbonApplication::instance()->sendEvent(new KeyEvent(event.type == KeyPress ?
                                                          InputEvent::Type_KeyPress :
                                                          InputEvent::Type_KeyRelease,
                                                          key, text, modifiers, autoRepeat));
}

void InputManagerX11Private::handleMouseEvent(XEvent &event)
{
    switch (event.type) {
    case MotionNotify: {
        XMotionEvent &motion = event.xmotion;
        const Point pos(motion.x, motion.y);
        const unsigned int modifiers = ::modifiers(motion.state);
        const unsigned int buttons = ::buttons(motion.state);
        GibbonApplication::instance()->sendEvent(new MouseEvent(InputEvent::Type_MouseMove, pos, MouseEvent::BUTTON_NONE, buttons, modifiers));
        break; }
    case ButtonPress:
    case ButtonRelease: {
        XButtonEvent &xbutton = event.xbutton;
        const Point pos(xbutton.x, xbutton.y);
        const unsigned int modifiers = ::modifiers(xbutton.state);
        const unsigned int buttons = ::buttons(xbutton.state);
        const MouseEvent::Button button = ::button(xbutton.button);
        GibbonApplication::instance()->sendEvent(new MouseEvent(event.type == ButtonPress ? InputEvent::Type_MousePress : InputEvent::Type_MouseRelease,
                                                                pos, button, buttons, modifiers));
        break; }
    default:
        break;
    }
}

void InputManager::wakeup()
{
    size_t size=0;
    size = write(mPriv->pipe[1], " ", 1);
    (void)size;
}

bool InputManager::hasEvents() const
{
    return false;
}
