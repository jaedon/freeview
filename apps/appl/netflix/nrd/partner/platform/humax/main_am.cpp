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
//#include "TileManagerBridge.h"

#define        NRD_STATUS_CHECK_TIMEOUT        3 //(3 sec)
#define		   NRD_QUIT_TIMEOUT					5 //(5 sec)
#define		   USE_AUDIOMIXER_ALTERNATIVE_STOP

using namespace netflix::gibbon;
using namespace netflix;

extern "C" void ScreenDFBPreInit(const char *caller);
extern "C" void ScreenDFBPostCleanup(const char *caller);
extern "C" void ScreenDFBNotifyOEMEvent(const char *caller, GibbonOEMEvent event);


static ApplicationManagerClient *sAppManagerClient = 0;

static void AM_TaskSleep(unsigned int ulTime)
{
    struct timespec    delay;
    struct timespec    rem;
    int    rc = 0;

    delay.tv_sec = ulTime / 1000;
    delay.tv_nsec = (ulTime % 1000) * 1000000;

    for (;;)
    {

        rc = nanosleep(&delay, &rem);
        if (0 != rc)
        {
            if (EINTR == errno)
            {
                delay = rem;
                continue;
            }

            return;
        }
        break;
    }
    return;
}

char onSuspend(Reason reason)
{
    char i=0, rc=0;
    char str[256];

    printf("[AMClient] ++%s++\n", __FUNCTION__);
    
    snprintf(str, sizeof(str), "exit_type=%d", reason);

    if(reason != static_cast<Reason>(ApplicationManagerClient::DummyReason))
    {
        netflix::nrdApp()->sendSystemCommand(str);
    }

    ScreenDFBNotifyOEMEvent("onSuspend", GibbonOEM_Suspended);

    printf("[AMClient][%s] --> netflix::nrdApp()->setSuspended()\n", __FUNCTION__);    

#ifdef USE_AUDIOMIXER_ALTERNATIVE_STOP
	/* Before go into suspend, audio mixer should be stopped! */
    if(sAppManagerClient->pfnCB)
       sAppManagerClient->pfnCB();
#endif

	netflix::nrdApp()->setSuspended(true, "");
    
    Time t(1000);
    for(i = 0; i < NRD_STATUS_CHECK_TIMEOUT; i++)
    {
        printf("[AM][%s] [%d] --> waitForStatus(Status_Suspended)\n", __FUNCTION__, i);
        if (netflix::nrdApp()->waitForStatus(netflix::NrdApplication::Status_Suspended, t)) /* timeout for waitForStatus donot work */
        {
                rc = 0;
                break;
        }
        AM_TaskSleep(1000);
    }
        
    if(i == NRD_STATUS_CHECK_TIMEOUT)
    {
        /* gibbon_oem_event >> GibbonOEM_Suspended */
        printf("[AM][%s] Error> netflix::nrdApp()->setSuspended(true) Time-OUT!!! \n", __FUNCTION__);
        rc = 1;        
        //if (0) ScreenDFBNotifyOEMEvent("onSuspend:Error", GibbonOEM_Suspended); 
    }
    else
    {
            printf("[AM][%s] setSuspended(true) completed!\n", __FUNCTION__);
    }

    printf("[AM]--%s-- rc(%d)\n", __FUNCTION__, rc);
    return rc;
}


char onResume(const char * payload)
{
    char i=0, rc=0;

    printf("[AMClient] ++%s++(%s)\n", __FUNCTION__, payload);
  
    netflix::nrdApp()->sendSystemCommand(payload);
    netflix::nrdApp()->setSuspended(false, "");
    //netflix::nrdApp()->waitForStatus(netflix::NrdApplication::Status_Running);

    Time t(1000);
    for(i = 0; i < NRD_STATUS_CHECK_TIMEOUT; i++)
    {
        printf("[AM][%s] [%d] --> waitForStatus(Status_Running)\n", __FUNCTION__, i);
        if (netflix::nrdApp()->waitForStatus(netflix::NrdApplication::Status_Running, t))            
        {
            rc = 0;
            break;
        }
        AM_TaskSleep(1000);
    }
        
    if(i == NRD_STATUS_CHECK_TIMEOUT)
    {
        printf("[AM][%s] Error> netflix::nrdApp()->setSuspended(false) Time-OUT!!! \n", __FUNCTION__);
        rc = 1;
        //if (0) ScreenDFBNotifyOEMEvent("onSuspend:Error", GibbonOEM_Suspended); 
    }
    
    printf("[AMClient]--%s-- rc(%d)\n", __FUNCTION__, rc);
    return rc;
}

char onBackground(Reason reason)
{
    char i=0, rc=0;
    char str[256];

    printf("[AMClient] ++%s++\n", __FUNCTION__);
    
    snprintf(str, sizeof(str), "exit_type=%d", reason);

    if(reason != static_cast<Reason>(ApplicationManagerClient::DummyReason))
        netflix::nrdApp()->sendSystemCommand(str);
    
    netflix::nrdApp()->setSuspended(true, "background");
    //netflix::nrdApp()->waitForStatus(netflix::NrdApplication::Status_Suspended);

    Time t(1000);
    for(i = 0; i < NRD_STATUS_CHECK_TIMEOUT; i++)
    {
        printf("[AM][%s] --> netflix::nrdApp()->waitForStatus()\n", __FUNCTION__);
        if (netflix::nrdApp()->waitForStatus(netflix::NrdApplication::Status_Suspended, t))            
        {
            rc = 0;
            break;
        }
        AM_TaskSleep(1000);
    }
        
    if(i == NRD_STATUS_CHECK_TIMEOUT)
    {
        printf("[AM][%s] Error> netflix::nrdApp()->setSuspended(ture, background) Time-OUT!!! \n", __FUNCTION__);
        rc = 1;
        //if (0) ScreenDFBNotifyOEMEvent("onSuspend:Error", GibbonOEM_Suspended); 
    }

    printf("[AMClient]--%s-- rc(%d)\n", __FUNCTION__, rc);
    return rc;
}

char onStop(Reason reason)
{
    char i=0, rc=0;
    char str[256];

    printf("[AMClient] ++%s++\n", __FUNCTION__);
    
    snprintf(str, sizeof(str), "exit_type=%d", reason);

    netflix::nrdApp()->sendSystemCommand(str);

#ifdef USE_AUDIOMIXER_ALTERNATIVE_STOP
	/* During Stop, Audio Mixer System need to be stopped immediately */
	if(sAppManagerClient->pfnCB)
		sAppManagerClient->pfnCB();
#endif
    netflix::nrdApp()->quit();
    
    //netflix::nrdApp()->waitForStatus(netflix::NrdApplication::Status_Stopped);

    Time t(1000);
    for(i = 0; i < NRD_QUIT_TIMEOUT; i++)
    {
        printf("[AM][%s] --> netflix::nrdApp()->waitForStatus()\n", __FUNCTION__);
        if (netflix::nrdApp()->waitForStatus(netflix::NrdApplication::Status_Stopped, t))            
        {
            rc = 0;
            break;
        }
        AM_TaskSleep(1000);
    }
        
    if(i == NRD_QUIT_TIMEOUT)
    {
        printf("[AM][%s] Error> netflix::nrdApp()->quit() Time-OUT!!! \n", __FUNCTION__);
        rc = 1;
        //if (0) ScreenDFBNotifyOEMEvent("onSuspend:Error", GibbonOEM_Suspended); 
    }
	else
    {
        printf("[AM][%s] nrdApp()->quit()completed!\n", __FUNCTION__);
    }

    printf("[AMClient]--%s-- rc(%d)\n", __FUNCTION__, rc);
    return rc;
}

void onKeyUp(int keyCode)
{
    printf("[AMClient]%s(%d)\n", __FUNCTION__, keyCode);
    gibbonApp()->sendEvent(new KeyEvent(InputEvent::Type_KeyPress, keyCode, std::string(), 0));
}

void onKeyDown(int keyCode)
{
    printf("[AMClient]%s(%d)\n", __FUNCTION__, keyCode);
    gibbonApp()->sendEvent(new KeyEvent(InputEvent::Type_KeyRelease, keyCode, std::string(), 0));
}

char onPower(ApplicationManagerClient::PowerReason reason)
{
    char         i, rc = 0;
    netflix::NrdApplication::Status      expected_status;
    
    printf("[AMClient] ++%s++ (%d)\n", __FUNCTION__, reason);
    
    switch (reason) 
    {
        case ApplicationManagerClient::PowerReason_Device_Sleep:
            netflix::nrdApp()->setSuspended(true, "sleep");
            expected_status = netflix::NrdApplication::Status_Suspended;
            break;
        case ApplicationManagerClient::PowerReason_Device_Wake:
            netflix::nrdApp()->setSuspended(false, "wake");
            expected_status = netflix::NrdApplication::Status_Running;
            break;
        default:            
            printf("[AMClient] %s => Unknown reason (%d)\n", __FUNCTION__, reason);
            rc = 1;
            break;
    }

    if(rc)
    {
        rc = 0;
    }
    else
    {
        Time t(1000);
        for(i = 0; i < NRD_STATUS_CHECK_TIMEOUT; i++)
        {
            printf("[AM][%s] --> netflix::nrdApp()->waitForStatus()\n", __FUNCTION__);
            if (netflix::nrdApp()->waitForStatus(expected_status, t))            
            {
                rc = 0;
                break;
            }
            AM_TaskSleep(1000);
        }
            
        if(i == NRD_STATUS_CHECK_TIMEOUT)
        {
            printf("[AM][%s] Error> netflix::nrdApp()->setSuspended() Time-OUT!!! \n", __FUNCTION__);
            rc = 1;
            //if (0) ScreenDFBNotifyOEMEvent("onSuspend:Error", GibbonOEM_Suspended); 
        }
    }

    printf("[AMClient]--%s-- rc(%d)\n", __FUNCTION__, rc);
    return rc;
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

void ApplicationManagerRegisterSuspendCallback( void (*pfnNotifier)(void) )
{
    if(sAppManagerClient)
        sAppManagerClient->pfnCB = pfnNotifier;
}

void gibbon_oem_event(GibbonOEMEvent event, void * /* data */)
{
    switch(event) {
    case GibbonOEM_Rendered:
        ScreenDFBNotifyOEMEvent("am:gibbon_oem_event", event); 
        break;
    case GibbonOEM_Init:
        sAppManagerClient = new ApplicationManagerClient;

        printf("[AMClient]gibbon_oem_event >> GibbonOEM_Init\n");
        ScreenDFBNotifyOEMEvent("am:gibbon_oem_event", event); 
        ScreenDFBPreInit("am:gibbon_oem_event"); /* for doing DirectFBInit and NEXUS_Platform_Join earlier */
        break;
        
    case GibbonOEM_Start: 
        {
            printf("[AMClient]gibbon_oem_event >> GibbonOEM_Start\n");
            ScreenDFBNotifyOEMEvent("am:gibbon_oem_event", event); 
        
            static ApplicationManagerClient::Callbacks cb = {
                onResume, onSuspend, onBackground, onStop,
                onKeyUp, onKeyDown, onPower
            };
            sAppManagerClient->setCallbacks(cb);
            //sAppManagerClient->notifyStarted();
            if(!Configuration::suspendModeOnStart().empty())
                sAppManagerClient->notifySuspended();
            break; 
        }
    case GibbonOEM_Suspended:
        printf("[AMClient]gibbon_oem_event >> GibbonOEM_Suspended\n");
        ScreenDFBNotifyOEMEvent("am:gibbon_oem_event", event); 
        sAppManagerClient->notifySuspended();
#ifndef USE_AUDIOMIXER_ALTERNATIVE_STOP
		/* This could be called after timeout. Move to suspend initiating */		
        if(sAppManagerClient->pfnCB)
            sAppManagerClient->pfnCB();
#endif		
        break;
        
    case GibbonOEM_Started:
        printf("\n\n\n @@@@ [AMClient]gibbon_oem_event >> GibbonOEM_Started\n\n\n\n");
        sAppManagerClient->notifyStarted();
        break;
        
    case GibbonOEM_Cleanup:
        printf("[AMClient]gibbon_oem_event >> GibbonOEM_Cleanup\n");
        ScreenDFBNotifyOEMEvent("am:gibbon_oem_event", event); 
        sAppManagerClient->stop();
        //delete sAppManagerClient;
        //sAppManagerClient = 0;
        break;
        
    case GibbonOEM_WatchdogNoHeartbeat:
    case GibbonOEM_WatchdogSigChild:
        break;
    }
}

