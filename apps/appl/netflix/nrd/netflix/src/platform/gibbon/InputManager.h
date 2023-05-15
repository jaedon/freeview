/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef __INPUTMANAGER_H__
#define __INPUTMANAGER_H__

#include <list>
#include <vector>

#include <gibbon/config.h>
#include <GibbonEvent.h>

#include <nrdbase/tr1.h>



#if defined(GIBBON_INPUT_DIRECTFB)
# include <directfb.h>
  class InputManagerReleaseTimer;
#endif

#if defined(GIBBON_INPUT_X11)
  class InputManagerX11Private;
#endif

namespace netflix {
namespace gibbon {

class InputManagerAutoRepeatTimer;

class InputManager
{
public:
    typedef shared_ptr<InputManager> SharedPtr;
    typedef weak_ptr<InputManager> WeakPtr;

    InputManager();
    ~InputManager();

    void moveMouse(const Point &pos);

    void waitForInput(llong mseconds); //-1 means wait forever
    void wakeup();

    bool getAutoRepeat(unsigned int *key, std::string *text, unsigned int *modifiers);
    void startAutoRepeat(unsigned int key, const std::string &text, unsigned int modifiers=0,
                         int delay = GibbonConfiguration::keyAutoRepeatDelay(), int interval = GibbonConfiguration::keyAutoRepeatInterval());
    void endAutoRepeat();

    bool hasEvents() const;

    enum HoldStyle {
        HoldStyle_Unknown,
        HoldStyle_PressPressRelease,
        HoldStyle_PressRelease,
        HoldStyle_PressReleasePressRelease
    };
    void setHoldStyle(const std::string &style);
    inline void setHoldStyle(HoldStyle style) { mHoldStyle = style; }
    inline HoldStyle getHoldStyle() const { return mHoldStyle; }
    static std::string describeHoldStyle(InputManager::HoldStyle style);

#ifdef GIBBON_INPUT_DEVINPUT
    void rescan();
#endif

protected:
    void init();
    void cleanup();

    bool mWakeup;
    HoldStyle mHoldStyle;
    Point mLastMousePosition;
    Mutex mMutex;
    shared_ptr<InputManagerAutoRepeatTimer> mAutoRepeat;
#if defined(GIBBON_INPUT_DIRECTFB)
    bool mWindowed;
    IDirectFBEventBuffer *mKeyBuffer;
    shared_ptr<InputManagerReleaseTimer> mRelease;

    void processKeyEvent(InputEvent::Type type,
                         DFBInputDeviceKeyIdentifier key_id,
                         DFBInputDeviceKeySymbol key_symbol,
                         int key_code,
                         unsigned int modifiers);
    InputEvent::Scope *mEvents;

#elif defined(GIBBON_INPUT_NULL)
    ConditionVariable mCondition;
#elif defined(GIBBON_INPUT_X11)
    int mPipe[2];
    shared_ptr<InputManagerX11Private> mPriv;
#elif defined(GIBBON_INPUT_DEVINPUT)
    int mPipe[2];
    std::vector<int> mDevices;
    bool shift;
    bool handleInput(int fd);
#endif
    class PlatformData;
    PlatformData *mPlatform;
};

}} // namespace netflix::gibbon

#endif /* __INPUTMANAGER_H__ */
