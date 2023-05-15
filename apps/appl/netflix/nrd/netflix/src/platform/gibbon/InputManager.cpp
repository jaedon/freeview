/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "InputManager.h"

#include <GibbonConfiguration.h>

using namespace netflix::gibbon;
using namespace netflix;

namespace netflix {
namespace gibbon {

class InputManagerAutoRepeatTimer : public Application::Timer
{
public:
    InputManagerAutoRepeatTimer(unsigned int key, const std::string &text, unsigned int modifiers)
        : Application::Timer(0), mKey(key), mText(text), mModifiers(modifiers), mFirst(true)
    {
    }

    inline unsigned int getKey() const { return mKey; }
    inline unsigned int getModifiers() const { return mModifiers; }
    inline std::string getText() const { return mText; }

    void startInterval(int delay, int interval) {
        mInterval = interval;
        restart(delay);
    }
    virtual std::string describe() const { return "InputManagerAutoRepeatTimer"; }
    void done() { mKey = KeyEvent::KEY_UNKNOWN; }
    void timerFired() {
        PERFORMANCE_MARKER_SCOPED("inputmanagerautorepeatetimer.fired");

        if (InputManager::SharedPtr ptr = GibbonApplication::instance()->getInputManager()) {
            if (ptr->hasEvents())
                return;
        }
        if(mKey == KeyEvent::KEY_UNKNOWN) {
            stop();
            return;
        }

        if(mFirst) {
            mFirst = false;
            restart(mInterval);
        }
        GibbonApplication::instance()->sendEvent(new KeyEvent(InputEvent::Type_KeyPress, mKey, mText, mModifiers, true, 1));
    }
private:
    unsigned int mKey;
    std::string mText;
    unsigned int mModifiers;
    int mInterval;
    bool mFirst;
};

}} // namespace netflix::gibbon


static struct {
    const char *name;
    InputManager::HoldStyle style;
} sHoldMap[] = {
    { "PRESS+RELEASE", InputManager::HoldStyle_PressRelease, },
    { "PRESS+RELEASE+PRESS+RELEASE", InputManager::HoldStyle_PressReleasePressRelease, },
    { "PRESS+PRESS+RELEASE", InputManager::HoldStyle_PressPressRelease, },
    { 0, InputManager::HoldStyle_Unknown },
};

InputManager::InputManager() : mWakeup(false), mMutex(GIBBON_INPUTMANAGER_MUTEX, "InputManager")
{
    InputManager::setHoldStyle(GibbonConfiguration::keyRepeatStyle());
    init();
}

InputManager::~InputManager()
{
    cleanup();
    endAutoRepeat();
}

std::string InputManager::describeHoldStyle(InputManager::HoldStyle style)
{
    for(int i = 0; sHoldMap[i].name; ++i) {
        if(sHoldMap[i].style == style)
            return sHoldMap[i].name;
    }
    return "None";
}

void InputManager::setHoldStyle(const std::string &style)
{
    InputManager::HoldStyle found = InputManager::HoldStyle_Unknown;
    for(int i = 0; sHoldMap[i].name; ++i) {
        if(style == sHoldMap[i].name) {
            found = sHoldMap[i].style;
            break;
        }
    }
    if(found == InputManager::HoldStyle_Unknown) {
        Log::fatal(TRACE_GIBBON_INPUT, "Invalid holdstyle %s", style.c_str());
        assert("Can't parse style" && 0);
    } else {
        mHoldStyle = found;
    }
}

void InputManager::startAutoRepeat(unsigned int key, const std::string &text, unsigned int modifiers, int delay, int interval)
{
    shared_ptr<InputManagerAutoRepeatTimer> old;
    shared_ptr<InputManagerAutoRepeatTimer> r;
    {
        ScopedMutex _lock(mMutex);
        if(mAutoRepeat && mAutoRepeat->getKey() == key && mAutoRepeat->getModifiers() == modifiers)
            return;
        if (mAutoRepeat) {
            mAutoRepeat->done();
            old.swap(mAutoRepeat);
        }
        mAutoRepeat.reset(new InputManagerAutoRepeatTimer(key, text, modifiers));
        r = mAutoRepeat;
    }

    if (old)
        old->stop();
    if (r)
        r->startInterval(delay, interval);
}

void InputManager::endAutoRepeat()
{
    shared_ptr<InputManagerAutoRepeatTimer> old;
    {
        ScopedMutex _lock(mMutex);
        if(mAutoRepeat) {
            mAutoRepeat->done();
            old.swap(mAutoRepeat);
        }
    }
    if (old)
        old->stop();
}

bool InputManager::getAutoRepeat(unsigned int *key, std::string *text, unsigned int *modifiers)
{
    ScopedMutex _lock(mMutex);
    if(!mAutoRepeat)
        return false;
    if(key)
        *key = mAutoRepeat->getKey();
    if(modifiers)
        *modifiers = mAutoRepeat->getModifiers();
    if(text)
        *text = mAutoRepeat->getText();
    return mAutoRepeat->getKey() != KeyEvent::KEY_UNKNOWN;
}
