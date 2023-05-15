/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include <Screen.h>
#include <GibbonEvent.h>
#include <InputManager.h>
#include <GibbonApplication.h>
#include <nrd/AppLog.h>

#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>

#include <nrdbase/utf8_unchecked.h>
#include <nrdbase/Time.h>

#include <directfb/directfb_version.h>

#include <hapi.h>

#if defined(GIBBON_USE_GLOBALCONFIG)
#include <globalconfig.h>
#endif

using namespace netflix::gibbon;
using namespace netflix;

static unsigned int sLastButton = KeyEvent::KEY_UNKNOWN;
static HBOOL        s_bOverlayBypassKey = FALSE;
static HBOOL        s_bStopWaitingKey = FALSE;
static HBOOL        s_bKeyPressed = FALSE;

class InputManagerReleaseTimer : public Application::Timer
{
public:
    InputManagerReleaseTimer(unsigned button, const std::string &text, unsigned int modifiers)
        : Application::Timer(GibbonConfiguration::keyAutoRepeatDelay()),
          mButton(button), mText(text), mModifiers(modifiers), mSent(false)
    {}
    virtual std::string describe() const { return "InputManagerReleaseTimer"; }
    void timerFired() { sendRelease(); }
    inline void sendRelease()
    {
        if(!mSent) {
            mSent = true;
            stop();
            sLastButton = KeyEvent::KEY_UNKNOWN;
            GibbonApplication::instance()->sendEvent(new KeyEvent(InputEvent::Type_KeyRelease, mButton, mText, mModifiers, false, 1));
        }
    }
private:
    const unsigned mButton;
    const std::string mText;
    const unsigned int mModifiers;
    bool mSent;
};


static unsigned keyFromDIKI(DFBInputDeviceKeyIdentifier diki);
static unsigned __keyFromDIKS(DFBInputDeviceKeySymbol diks);
static HERROR   __onNotifyOverlayState(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser);
static HBOOL    __getInputEvent(HInputEvent_t   *phEvent, int ms);

void InputManager::moveMouse(const Point &)
{
}

static inline MouseEvent::Button button(DFBInputDeviceButtonIdentifier button)
{
    switch (button) {
    case DIBI_LEFT:
        return MouseEvent::BUTTON_LEFT;
    case DIBI_RIGHT:
        return MouseEvent::BUTTON_RIGHT;
    case DIBI_MIDDLE:
        return MouseEvent::BUTTON_MIDDLE;
    default:
        break;
    }
    return MouseEvent::BUTTON_NONE;
}

static inline unsigned buttons(DFBInputDeviceButtonMask buttons)
{
    unsigned ret = MouseEvent::BUTTON_NONE;
    if(buttons & DIBM_LEFT)
        ret |= MouseEvent::BUTTON_LEFT;
    if(buttons & DIBM_RIGHT)
        ret |= MouseEvent::BUTTON_RIGHT;
    if(buttons & DIBM_MIDDLE)
        ret |= MouseEvent::BUTTON_MIDDLE;
    return ret;
}

static bool __isUsableKey(DFBInputDeviceKeySymbol diks)
{
    switch (diks) {
      case DIKS_VOLUME_UP:
      case DIKS_VOLUME_DOWN:
      case DIKS_MUTE:
      case DIKS_POWER:
      /* humax key scenario. bypass*/
      case DIKS_CHANNEL_UP :
      case DIKS_CHANNEL_DOWN :
      case DIKS_MENU :
      case DIKS_HOME :
      case DIKS_INTERNET :
      case DIKS_PLAYER: // Min side key (TDC YSR-2000)
      case DIKS_EPG :
      case DIKS_EXIT:
          return false;
      default:
          break;
    }

    return true;
}

static bool __needBlockKey(DFBInputDeviceKeySymbol diks)
{
	switch(diks){
		case DIKS_RECORD:
		case DIKS_CUSTOM65:
			return true;
		default:
			break;
	}
	return false;
}

#if defined(GIBBON_USE_GLOBALCONFIG)
static void __registAceptableKeyForModel(void)
{
#if defined(CONFIG_OP_TDC)
	HINT32	anKeyList[] = {DIKS_RETURN, DIKS_BACK, DIKS_PLAYPAUSE, DIKS_STOP,
						DIKS_CURSOR_LEFT, DIKS_CURSOR_RIGHT,  DIKS_CURSOR_UP, DIKS_CURSOR_DOWN,
						DIKS_REWIND, DIKS_FASTFORWARD, DIKS_PREVIOUS, DIKS_NEXT,
						DIKS_RECORD, DIKS_CUSTOM65};

	HAPI_RegisterAcceptableKeyList(eAWINDOW_NETFLIX, TRUE, anKeyList, sizeof(anKeyList));
#else
	printf("[%s:%d] Received All Key Events. \n", __FUNCTION__, __LINE__);
#endif
}
#endif

void InputManager::init()
{
    printf("[%s:%d] ++\n", __FUNCTION__, __LINE__);

    mEvents = 0;
    mWindowed = 0;
    mKeyBuffer = NULL;

    HAPI_AddVirtualWindow(eAWINDOW_NETFLIX);

#if defined(GIBBON_USE_GLOBALCONFIG)
	__registAceptableKeyForModel();
#endif

    HAPI_SetWindowStyle(eAWINDOW_NETFLIX, eHWS_WindowInputAlways,	FALSE);

    s_bOverlayBypassKey = FALSE;
    HAPI_ConnectSignal("signal:onNotifyOverlayState",  NULL, __onNotifyOverlayState);
}

void InputManager::waitForInput(llong mseconds)
{
    HInputEvent_t   hEvent;

    const Time waitUntil = mseconds == -1 ? Time() : Time::mono() + Time::fromMS(mseconds);

    int seconds = 0, ms = 500;
    // We can't use WaitForEvent without timeout since there's an
    // inherent race condition in the way it is implemented (at least in
    // dfb 1.2.9). It doesn't set any state when being woken up so if
    // the call to IDirectFBEventBuffer_WakeUp happens before the call
    // to IDirectFBEventBuffer_WaitForEvent we would potentially wait
    // forever.

    bool allowOne = !mseconds;

    while(true)
    {
        {
            ScopedMutex lock(mMutex);
            if(mWakeup && !allowOne)
            {
                mWakeup = false;
                return;
            }

            if(allowOne)
            {
                allowOne = false;
                seconds = 0;
                ms = 1;
            }
            else
            {
                const Time now = Time::mono();
                if(mseconds != -1) {
                    const int timeLeft = (waitUntil - now).ms();
                    if(timeLeft <= 0)
                        return;

                    ms = timeLeft;
                }
            }
        }

        if(__getInputEvent(&hEvent, ms) == TRUE)
        {
            InputEvent::Scope events;
            mEvents = &events;

            switch (hEvent.type)
            {
                case DIET_KEYPRESS:
                case DIET_KEYRELEASE:
                    {
                        unsigned int modifiers = KeyEvent::MODIFIER_NONE;
                        /*
                        if(window.type == DWET_KEYDOWN || window.type == DWET_KEYUP) {
                            if(window.modifiers & DIMM_CONTROL)
                                modifiers |= KeyEvent::MODIFIER_CTRL;
                            if(window.modifiers & DIMM_ALT)
                                modifiers |= KeyEvent::MODIFIER_ALT;
                            if(window.modifiers & DIMM_SHIFT)
                                modifiers |= KeyEvent::MODIFIER_SHIFT;
                        }
                        */

						if(__needBlockKey((DFBInputDeviceKeySymbol)hEvent.key_symbol) == true)
						{
							printf("[%s:%d] Return(CONSUMED) [%s-%d]\n", __FUNCTION__, __LINE__, (hEvent.type==DIET_KEYPRESS)?"Press":"Release", hEvent.key_symbol);
							HAPI_ReturnKeyEvent(eAWINDOW_NETFLIX, eHINPUTRET_CONSUMED, &hEvent);
						}
						else if((__isUsableKey((DFBInputDeviceKeySymbol)hEvent.key_symbol) == false) || (s_bOverlayBypassKey == TRUE)) /* HUMAX/HAMA */
                        {
                            printf("[%s:%d] Return(PASS) [%s-%d]\n", __FUNCTION__, __LINE__, (hEvent.type==DIET_KEYPRESS)?"Press":"Release", hEvent.key_symbol);
                            HAPI_ReturnKeyEvent(eAWINDOW_NETFLIX, eHINPUTRET_PASS, &hEvent);
                        }
                        else
                        {
                            if((hEvent.type==DIET_KEYRELEASE) && (s_bKeyPressed == FALSE))
                            {
                                printf("[%s:%d] No-press event => Return(PASS) [%s-%d]\n", __FUNCTION__, __LINE__, (hEvent.type==DIET_KEYPRESS)?"Press":"Release", hEvent.key_symbol);
                                HAPI_ReturnKeyEvent(eAWINDOW_NETFLIX, eHINPUTRET_PASS, &hEvent);
                            }
                            else
                            {
                                printf("[%s:%d] Return(CONSUMED) [%s-%d]\n", __FUNCTION__, __LINE__, (hEvent.type==DIET_KEYPRESS)?"Press":"Release", hEvent.key_symbol);
                                HAPI_ReturnKeyEvent(eAWINDOW_NETFLIX, eHINPUTRET_CONSUMED, &hEvent);
                                processKeyEvent(hEvent.type == DIET_KEYPRESS ? InputEvent::Type_KeyPress : InputEvent::Type_KeyRelease,
                                            (DFBInputDeviceKeyIdentifier)hEvent.key_id, (DFBInputDeviceKeySymbol)hEvent.key_symbol, (int)hEvent.key_code, modifiers);

                                if(hEvent.type==DIET_KEYPRESS)
                                {
                                    s_bKeyPressed = TRUE;
                                }
                                else//if(hEvent.type==DIET_KEYRELEASE)
                                {
                                    s_bKeyPressed = FALSE;
                                }
                            }
                        }
                    }
                    break;
                default:
                    break;
            }

            mEvents = 0;
        }
    }
}

void InputManager::wakeup()
{
    ScopedMutex lock(mMutex);
    mWakeup = true;
    s_bStopWaitingKey = TRUE;
}

void InputManager::cleanup()
{
    if(mKeyBuffer != NULL) {
        mKeyBuffer->Release(mKeyBuffer);
        mKeyBuffer = NULL;
    }

    //if(sLastButton != KeyEvent::KEY_UNKNOWN)
    {
        HInputEvent_t   hEvent;

        if(__getInputEvent(&hEvent, 300) == TRUE)
        {
            //printf("\n\n [IM:%d] key(%s:%d)\n", __LINE__, hEvent.type == DIET_KEYPRESS ? "Press" : "Release", hEvent.key_symbol);
            if((__isUsableKey((DFBInputDeviceKeySymbol)hEvent.key_symbol) == false) || (s_bOverlayBypassKey == TRUE)) /* HUMAX/HAMA */
            {
                printf("[%s:%d] Return(1) [%s-%d]\n", __FUNCTION__, __LINE__, (hEvent.type==DIET_KEYPRESS)?"Press":"Release", hEvent.key_symbol);
                HAPI_ReturnKeyEvent(eAWINDOW_NETFLIX, eHINPUTRET_PASS, &hEvent);
            }
            else
            {
                printf("[%s:%d] Return(2) [%s-%d]\n", __FUNCTION__, __LINE__, (hEvent.type==DIET_KEYPRESS)?"Press":"Release", hEvent.key_symbol);
                HAPI_ReturnKeyEvent(eAWINDOW_NETFLIX, eHINPUTRET_CONSUMED, &hEvent);
            }
            sLastButton = KeyEvent::KEY_UNKNOWN;
            s_bOverlayBypassKey = FALSE;
        }
    }

    printf("[InputManager::cleanup:%d] --\n", __LINE__);
}


unsigned keyFromDIKI(DFBInputDeviceKeyIdentifier diki)
{
    switch (diki) {
    case DIKI_KP_DIV: return KeyEvent::KEY_DIVIDE;
    case DIKI_KP_MULT: return KeyEvent::KEY_MULTIPLY;
    case DIKI_KP_MINUS: return KeyEvent::KEY_SUBTRACT;
    case DIKI_KP_PLUS: return KeyEvent::KEY_ADD;
    case DIKI_KP_ENTER: return KeyEvent::KEY_RETURN;
    case DIKI_KP_DECIMAL: return KeyEvent::KEY_DECIMAL;
    case DIKI_KP_0: return KeyEvent::KEY_NUMPAD0;
    case DIKI_KP_1: return KeyEvent::KEY_NUMPAD1;
    case DIKI_KP_2: return KeyEvent::KEY_NUMPAD2;
    case DIKI_KP_3: return KeyEvent::KEY_NUMPAD3;
    case DIKI_KP_4: return KeyEvent::KEY_NUMPAD4;
    case DIKI_KP_5: return KeyEvent::KEY_NUMPAD5;
    case DIKI_KP_6: return KeyEvent::KEY_NUMPAD6;
    case DIKI_KP_7: return KeyEvent::KEY_NUMPAD7;
    case DIKI_KP_8: return KeyEvent::KEY_NUMPAD8;
    case DIKI_KP_9: return KeyEvent::KEY_NUMPAD9;
    case DIKI_F1: return KeyEvent::KEY_F1;
    case DIKI_F2: return KeyEvent::KEY_F2;
    case DIKI_F3: return KeyEvent::KEY_F3;
    case DIKI_F4: return KeyEvent::KEY_F4;
    case DIKI_F5: return KeyEvent::KEY_F5;
    case DIKI_F6: return KeyEvent::KEY_F6;
    case DIKI_F7: return KeyEvent::KEY_F7;
    case DIKI_F8: return KeyEvent::KEY_F8;
    case DIKI_F9: return KeyEvent::KEY_F9;
    case DIKI_F10: return KeyEvent::KEY_F10;
    case DIKI_F11: return KeyEvent::KEY_F11;
    case DIKI_F12: return KeyEvent::KEY_F12;
    case DIKI_SHIFT_L:
    case DIKI_SHIFT_R: return KeyEvent::KEY_SHIFT;
    case DIKI_CONTROL_L:
    case DIKI_CONTROL_R: return KeyEvent::KEY_CONTROL;
    case DIKI_ALT_L:
    case DIKI_ALT_R: return KeyEvent::KEY_ALT;
    case DIKI_META_L:
    case DIKI_META_R: return KeyEvent::KEY_META;
    case DIKI_SUPER_L: return KeyEvent::KEY_SUPER_L;
    case DIKI_SUPER_R: return KeyEvent::KEY_SUPER_R;
    case DIKI_HYPER_L: return KeyEvent::KEY_HYPER_L;
    case DIKI_HYPER_R: return KeyEvent::KEY_HYPER_R;
    case DIKI_CAPS_LOCK: return KeyEvent::KEY_CAPSLOCK;
    case DIKI_NUM_LOCK: return KeyEvent::KEY_NUMLOCK;
    case DIKI_SCROLL_LOCK: return KeyEvent::KEY_SCROLLLOCK;
    case DIKI_ESCAPE: return KeyEvent::KEY_ESCAPE;
    case DIKI_LEFT: return KeyEvent::KEY_LEFT;
    case DIKI_RIGHT: return KeyEvent::KEY_RIGHT;
    case DIKI_UP: return KeyEvent::KEY_UP;
    case DIKI_DOWN: return KeyEvent::KEY_DOWN;
    case DIKI_TAB: return KeyEvent::KEY_TAB;
    case DIKI_ENTER: return KeyEvent::KEY_RETURN;
    case DIKI_BACKSPACE: return KeyEvent::KEY_BACKSPACE;
    case DIKI_SPACE: return KeyEvent::KEY_SPACE;
    case DIKI_INSERT: return KeyEvent::KEY_INSERT;
    case DIKI_DELETE: return KeyEvent::KEY_DELETE;
    case DIKI_HOME: return KeyEvent::KEY_HOME;
    case DIKI_END: return KeyEvent::KEY_END;
    case DIKI_PAGE_UP: return KeyEvent::KEY_PAGE_UP;
    case DIKI_PAGE_DOWN: return KeyEvent::KEY_PAGE_DOWN;
    case DIKI_PRINT: return KeyEvent::KEY_PRINT;
    case DIKI_PAUSE: return KeyEvent::KEY_PAUSE;
    case DIKI_MINUS_SIGN: return KeyEvent::KEY_OEM_MINUS;
    case DIKI_QUOTE_LEFT: return KeyEvent::KEY_OEM_3;
    case DIKI_EQUALS_SIGN: return KeyEvent::KEY_OEM_PLUS;
    case DIKI_BRACKET_LEFT: return KeyEvent::KEY_OEM_4;
    case DIKI_BRACKET_RIGHT: return KeyEvent::KEY_OEM_6;
    case DIKI_BACKSLASH: return KeyEvent::KEY_OEM_5;
    case DIKI_SEMICOLON: return KeyEvent::KEY_OEM_1;
    case DIKI_QUOTE_RIGHT: return KeyEvent::KEY_OEM_7;
    case DIKI_COMMA: return KeyEvent::KEY_OEM_COMMA;
    case DIKI_PERIOD: return KeyEvent::KEY_OEM_PERIOD;
    case DIKI_SLASH: return KeyEvent::KEY_OEM_2;
    default: break;
    }
    return KeyEvent::KEY_UNKNOWN;
}

static unsigned __keyFromDIKS(DFBInputDeviceKeySymbol diks)
{
  switch (diks) {
    /* HUMAX RCU Key Mapping */
    case DIKS_CURSOR_LEFT: return KeyEvent::KEY_LEFT;
    case DIKS_CURSOR_RIGHT: return KeyEvent::KEY_RIGHT;
    case DIKS_CURSOR_UP: return KeyEvent::KEY_UP;
    case DIKS_CURSOR_DOWN: return KeyEvent::KEY_DOWN;

    /* Numeric key */
    case DIKS_0: return KeyEvent::KEY_0;
    case DIKS_1: return KeyEvent::KEY_1;
    case DIKS_2: return KeyEvent::KEY_2;
    case DIKS_3: return KeyEvent::KEY_3;
    case DIKS_4: return KeyEvent::KEY_4;
    case DIKS_5: return KeyEvent::KEY_5;
    case DIKS_6: return KeyEvent::KEY_6;
    case DIKS_7: return KeyEvent::KEY_7;
    case DIKS_8: return KeyEvent::KEY_8;
    case DIKS_9: return KeyEvent::KEY_9;

    case DIKS_RED: return KeyEvent::KEY_RED;
    case DIKS_GREEN: return KeyEvent::KEY_GREEN;
    case DIKS_YELLOW: return KeyEvent::KEY_YELLOW;
    case DIKS_BLUE: return KeyEvent::KEY_BLUE;

    case DIKS_STOP: return KeyEvent::KEY_F10; // kye.stop
    case DIKS_PLAY: return KeyEvent::KEY_F8;  // key.play
    case DIKS_PAUSE: return KeyEvent::KEY_F6;  // key.pause
    case DIKS_PLAYPAUSE : return KeyEvent::KEY_F8;  // key.play

    case DIKS_FASTFORWARD: return KeyEvent::KEY_F9; // key.fastfwd
    case DIKS_REWIND: return KeyEvent::KEY_F7; // key.rewind

    //case DIKS_POWER : return KeyEvent::KEY_OEM_PLUS; //key.powser

    case DIKS_ENTER : return KeyEvent::KEY_ENTER;
    case DIKS_SELECT : return KeyEvent::KEY_ENTER; /* for TDC(YSR-2000) */
    case DIKS_BACK : return KeyEvent::KEY_ESCAPE; // key.back
    case DIKS_LAST : return KeyEvent::KEY_ESCAPE;  // key.back

    case DIKS_AUDIO : return KeyEvent::KEY_F11; //key.audio
    case DIKS_SUBTITLE : return KeyEvent::KEY_NUMPAD2; //key.subtitile

    //case DIKS_EXIT : return KeyEvent::KEY_F12;  // key.exit
                     /* humax key scenario : convert to key.exit */
    //case DIKS_MENU : return KeyEvent::KEY_F12; //-->key.exit
    //case DIKS_HOME : return KeyEvent::KEY_F12; //-->key.exit
    //case DIKS_INTERNET : return KeyEvent::KEY_F12; //-->key.exit
    //case DIKS_EPG : return KeyEvent::KEY_F12; //-->key.exit

    case DIKS_INFO : return KeyEvent::KEY_DELETE; //key.info
    case DIKS_OPTION : return KeyEvent::KEY_END; //key.options

    //case DIKS_VOLUME_UP : return KeyEvent::KEY_VOLUME_UP;
    //case DIKS_VOLUME_DOWN : return KeyEvent::KEY_VOLUME_DOWN;
    //case DIKS_MUTE : return KeyEvent::KEY_VOLUME_MUTE;

    //case DIKS_CHANNEL_UP : return KeyEvent::KEY_CHANNEL_UP;
    //case DIKS_CHANNEL_DOWN : return KeyEvent::KEY_CHANNEL_DOWN;

    case DIKS_ZOOM : return KeyEvent::KEY_ZOOM;

    case DIKS_PREVIOUS : return KeyEvent::KEY_F3; // key.skip_back
    case DIKS_NEXT : return KeyEvent::KEY_F4; // key.skip_forward

    case DIKS_CLEAR : return KeyEvent::KEY_MULTIPLY; //key.clear

    case DIKS_CUSTOM6 : return KeyEvent::KEY_NUMPAD6; //key.search
    //case DIKS_CUSTOM65: return KeyEvent::KEY_F3; // key.skip_back
    //case DIKS_CUSTOM66: return KeyEvent::KEY_F4; // key.skip_forward

    //case DIKS_CUSTOM68 : return KeyEvent::KEY_PRIOR; //key.netflix
    case DIKS_CUSTOM68 : return KeyEvent::KEY_DELETE; //key.info

    default: break;
  }
    return KeyEvent::KEY_UNKNOWN;
}


static HERROR	__onNotifyOverlayState(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
    HUINT32                ulOverlayState;

    if( nArgc < 1 )
    {
        return -1;
    }

    ulOverlayState = (HUINT32)HxOBJECT_INT(apArgv[0]);
    if(ulOverlayState == 0)
    {
        s_bOverlayBypassKey = TRUE;
    }
    else
    {
        s_bOverlayBypassKey = FALSE;
    }
    return ERR_OK;
}

static HBOOL   __getInputEvent(HInputEvent_t   *phEvent, int ms)
{
    HBOOL       bRet = FALSE;
    Time        tStart, tCur;
    int         nDiff = 0;

    s_bStopWaitingKey = FALSE;

    tStart = Time::mono();

    do
    {
        bRet = HAPI_GetInputEvent(eAWINDOW_NETFLIX, phEvent, 1);
        if(bRet == TRUE)
        {
            break;
        }

        if(s_bStopWaitingKey == TRUE)
        {
            //printf("[NxIM:%d] WakeUp: __getInputEvent()--\n", __LINE__);
            break;
        }

        tCur = Time::mono();
        nDiff = (tCur - tStart).ms();
    }while(nDiff < ms);

    return bRet;
}

void InputManager::processKeyEvent(InputEvent::Type type,
                                   DFBInputDeviceKeyIdentifier key_id,
                                   DFBInputDeviceKeySymbol key_symbol,
                                   int key_code,
                                   unsigned int modifiers)
{
    std::string text;
    if(!(key_symbol & DIKT_SPECIAL) && key_symbol >= 32 && key_symbol != DIKS_DELETE) {
        unsigned char buf[4] = { 0, 0, 0, 0 };
        const unsigned char *end = utf8::unchecked::append(key_symbol, buf);
        const int len = end - buf;
        text.assign(reinterpret_cast<const char*>(buf), len);
    }
#if 0 /* HUMAX */
    unsigned key = keyFromDIKI(key_id);
#else
    unsigned key = key_symbol ? __keyFromDIKS(key_symbol) : keyFromDIKI(key_id);
#endif

    if(key == KeyEvent::KEY_UNKNOWN) {
        if(text.size() == 1)
            key = KeyEvent::keyFromAscii(text[0]);
        if(key == KeyEvent::KEY_UNKNOWN && key_id && key_id != DIKI_UNKNOWN) {
            key = KeyEvent::KEY_FIRST_UNKNOWN + key_id;
        } else if(key == KeyEvent::KEY_UNKNOWN && key_symbol > 0) {
            key = KeyEvent::KEY_FIRST_UNKNOWN + key_symbol;
        } else if(key == KeyEvent::KEY_UNKNOWN && key_code > 0) {
            key = KeyEvent::KEY_FIRST_UNKNOWN + key_code;
        }
    }
    NTRACE(TRACE_GIBBON_INPUT, "%d: Mapped(id) %d/%d/%d -- %d (%u/%p)\n", getHoldStyle(),
           key_symbol, type, key_id, key, sLastButton, mRelease.get());

    switch (getHoldStyle()) {
    case HoldStyle_PressPressRelease: {
        if(type == InputEvent::Type_KeyPress) {
            mEvents->sendEvent(new KeyEvent(InputEvent::Type_KeyPress, key, text, modifiers, key == sLastButton));
            sLastButton = key;
            printf("[%s:%d] Press key(%d)\n", __FUNCTION__, __LINE__, key);
        } else if(type == InputEvent::Type_KeyRelease) {
            sLastButton = KeyEvent::KEY_UNKNOWN;
            mEvents->sendEvent(new KeyEvent(InputEvent::Type_KeyRelease, key, text, modifiers));
            printf("[%s:%d] Release key(%d)\n", __FUNCTION__, __LINE__, key);
        }
        break; }
    case HoldStyle_PressRelease: {
        if(type == InputEvent::Type_KeyPress) {
            mEvents->sendEvent(new KeyEvent(InputEvent::Type_KeyPress, key, text, modifiers));
            if(key != KeyEvent::KEY_UNKNOWN)
                GibbonApplication::instance()->getInputManager()->startAutoRepeat(key, text, modifiers);
        } else if(type == InputEvent::Type_KeyRelease) {
            GibbonApplication::instance()->getInputManager()->endAutoRepeat();
            mEvents->sendEvent(new KeyEvent(InputEvent::Type_KeyRelease, key, text, modifiers));
        }
        break; }
    case HoldStyle_PressReleasePressRelease: {
        if(mRelease && key != sLastButton) {
            sLastButton = KeyEvent::KEY_UNKNOWN;
            mRelease->sendRelease();
            mRelease.reset();
        }
        if(type == InputEvent::Type_KeyPress) {
            if(key == sLastButton) {
                mEvents->sendEvent(new KeyEvent(InputEvent::Type_KeyPress, key, text, modifiers, true));
            } else {
                assert(!mRelease);
                sLastButton = key;
                mRelease.reset(new InputManagerReleaseTimer(key, text, modifiers));
                mEvents->sendEvent(new KeyEvent(InputEvent::Type_KeyPress, key, text, modifiers));
            }
            if(mRelease)
                mRelease->restart(mRelease->interval());
        }
        break; }
    case HoldStyle_Unknown:
        break;
    }
}

bool InputManager::hasEvents() const
{
    if(mKeyBuffer == NULL)
    {
        return false;
    }
    else
    {
        return (mKeyBuffer->HasEvent(mKeyBuffer) == DFB_OK);
    }
}

