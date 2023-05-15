/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#include <GibbonApplication.h>
#include <GibbonEvent.h>

#include <nrdbase/Application.h>

#include "ApplicationManagerClient.h"
#include "TileManagerBridge.h"

using namespace netflix::gibbon;
using namespace netflix;

static ApplicationManagerClient *sAppManagerClient = 0;

void onSuspend(Reason reason)
{
    printf("%s\n", __FUNCTION__);

    char str[256];
    snprintf(str, sizeof(str), "exit_type=%d", reason);

    if(reason != static_cast<Reason>(ApplicationManagerClient::DummyReason))
        netflix::nrdApp()->sendSystemCommand(str);
    netflix::nrdApp()->setSuspended(true, "");
    netflix::nrdApp()->waitForStatus(netflix::NrdApplication::Status_Suspended);
}


void onResume(const char * p, int trackId, Source sourceType)
{
    printf("%s(%s,%d,%d)\n", __FUNCTION__, p, trackId, sourceType);

    char str[256];
    if(p)
        snprintf(str, sizeof(str), "p=%s&source_type=%d&trackId=%d", p, trackId, sourceType);
    else
        snprintf(str, sizeof(str), "source_type=%d", sourceType);


    netflix::nrdApp()->sendSystemCommand(str);
    netflix::nrdApp()->setSuspended(false, "");
    netflix::nrdApp()->waitForStatus(netflix::NrdApplication::Status_Running);
}

void onBackground(Reason reason)
{
    printf("%s\n", __FUNCTION__);

    char str[256];
    snprintf(str, sizeof(str), "exit_type=%d", reason);

    if(reason != static_cast<Reason>(ApplicationManagerClient::DummyReason))
        netflix::nrdApp()->sendSystemCommand(str);
    netflix::nrdApp()->setSuspended(true, "background");
    netflix::nrdApp()->waitForStatus(netflix::NrdApplication::Status_Suspended);
}

void onStop(Reason reason)
{
    printf("%s\n", __FUNCTION__);

    char str[256];
    snprintf(str, sizeof(str), "exit_type=%d", reason);

    netflix::nrdApp()->sendSystemCommand(str);
    netflix::nrdApp()->quit();
    netflix::nrdApp()->waitForStatus(netflix::NrdApplication::Status_Stopped);
}

void onKeyUp(int keyCode)
{
    printf("%s\n", __FUNCTION__);
    gibbonApp()->sendEvent(new KeyEvent(InputEvent::Type_KeyPress, keyCode, std::string(), 0));
}

void onKeyDown(int keyCode)
{
    printf("%s\n", __FUNCTION__);
    gibbonApp()->sendEvent(new KeyEvent(InputEvent::Type_KeyRelease, keyCode, std::string(), 0));
}

void onPower(ApplicationManagerClient::PowerReason reason)
{
    printf("%s\n", __FUNCTION__);
    switch (reason) {
    case ApplicationManagerClient::PowerReason_Device_Sleep:
        netflix::nrdApp()->setSuspended(true, "sleep");
        break;
    case ApplicationManagerClient::PowerReason_Device_Wake:
        netflix::nrdApp()->setSuspended(false, "wake");
        break;
    default:
        break;
    }
}

void ApplicationManagerRequestSuspend() //called from DPI
{
    if(sAppManagerClient)
        sAppManagerClient->notifyRequestSuspend();
}

void ApplicationManagerRequestBackground() //called from DPI
{
    if(sAppManagerClient)
        sAppManagerClient->notifyRequestBackground();
}

void gibbon_oem_event(GibbonOEMEvent event, void * /* data */)
{
    switch(event) {
    case GibbonOEM_Rendered:
        break;
    case GibbonOEM_Init:
        sAppManagerClient = new ApplicationManagerClient;
        break;
    case GibbonOEM_Start: {
        static ApplicationManagerClient::Callbacks cb = {
            onResume, onSuspend, onBackground, onStop,
            onKeyUp, onKeyDown, onPower
        };
        sAppManagerClient->setCallbacks(cb);
        sAppManagerClient->notifyStarted();
        if(!Configuration::suspendModeOnStart().empty())
            sAppManagerClient->notifySuspended();
        break; }
    case GibbonOEM_Suspended:
        sAppManagerClient->notifySuspended();
        break;
    case GibbonOEM_Started:
        break;
    case GibbonOEM_Cleanup:
        sAppManagerClient->stop();
        delete sAppManagerClient;
        sAppManagerClient = 0;
        break;
    case GibbonOEM_WatchdogNoHeartbeat:
    case GibbonOEM_WatchdogSigChild:
        break;
    }
}

