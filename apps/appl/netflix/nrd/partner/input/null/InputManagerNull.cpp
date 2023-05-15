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

#include <nrdbase/ScopedMutex.h>
#include <nrdbase/Time.h>

using namespace netflix::gibbon;
using namespace netflix;

void InputManager::init()
{
}

void InputManager::cleanup()
{
}

void InputManager::moveMouse(const Point &)
{
}

void InputManager::waitForInput(llong waitMS)
{
    ScopedMutex lock(mMutex);
    Time wait(waitMS);
    while(!mWakeup) {
        if(mCondition.wait(&mMutex, &wait) == NFErr_TimedOut)
            break;
    }
    mWakeup = false;
}

void InputManager::wakeup()
{
    ScopedMutex lock(mMutex);
    mWakeup = true;
    mCondition.signal();
}

bool InputManager::hasEvents() const
{
    return false;
}
