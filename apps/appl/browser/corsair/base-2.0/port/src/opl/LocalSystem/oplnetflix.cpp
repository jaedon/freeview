/**************************************************************************************/
/**
 * @file bpllocalsystem.c
 *
 * System information
 *
 * @attention   Copyright (c) 2013 Humax - All rights reserved.
 *
 **************************************************************************************
 **/

#include "oplnetflix.h"
#if defined(CONFIG_NETFLIX_APP)
#include "netflixmanager.h"
#endif

#include "oapi.h"
#include "hapi.h"

#include <stdlib.h>
#include <string.h>


/******************************************************************
	class OplNetflix
******************************************************************/
#if defined(CONFIG_NETFLIX_APP)
static Launch_Param_t oplNetflixObj={
	(NFLIX_Launch_Source_Type_t)Source_TESTING,
};
#endif

StatusChangedEvent_t OplNetflix::s_statusChangedCallback = NULL;

OplNetflix::OplNetflix()
{
}

OplNetflix::~OplNetflix()
{
}

bool OplNetflix::startWithType(int type)
{

	printf("OplNetflix::startWithType :: type(%d)\n", type);

#if defined(CONFIG_NETFLIX_APP)
    HxSTD_MemSet(&oplNetflixObj, 0, sizeof(Launch_Param_t));
    oplNetflixObj.enSrc=(NFLIX_Launch_Source_Type_t)type;

	HxLOG_Debug("---> signal:onStartNetflixAppl\n");
	if (HAPI_BroadcastSignal("signal:onStartNetflixAppl", 0, "b", &oplNetflixObj,sizeof(Launch_Param_t)) != ERR_OK)
	{
		HxLOG_Error("Error> failed to (signal:onStartNetflixAppl)\n");
	}
    // test for plugin works well
    //HAPI_BroadcastSignal("signal:onNetflixStateChanged", 0, "i", 10000);
#endif
	return ERR_OK;
}

bool OplNetflix::startWithTypeUrl(int type, unsigned char *pucUrl)
{
#if defined(CONFIG_NETFLIX_APP)
    printf("OplNetflix::startWithTypeUrl :: type(%d), url(%s)\n", type, oplNetflixObj.szURL);

    HxSTD_MemSet(&oplNetflixObj, 0, sizeof(Launch_Param_t));
    oplNetflixObj.enSrc=(NFLIX_Launch_Source_Type_t)type;
	HxSTD_snprintf((char*)oplNetflixObj.szURL, MAX_ENCORDED_URL_LEN, "%s", (char*)pucUrl);

	HxLOG_Debug("---> signal:onStartNetflixAppl\n");
	if (HAPI_BroadcastSignal("signal:onStartNetflixAppl", 0, "b", &oplNetflixObj,sizeof(Launch_Param_t)) != ERR_OK)
	{
		HxLOG_Error("Error> failed to (signal:onStartNetflixAppl)\n");
	}
#endif

	return ERR_OK;
}

bool OplNetflix::startWith6params(int type, unsigned char *pucUrl, int chID, int prevChid, int nextChid, int category)
{
#if defined(CONFIG_NETFLIX_APP)
    HxSTD_MemSet(&oplNetflixObj, 0, sizeof(Launch_Param_t));
    oplNetflixObj.enSrc=(NFLIX_Launch_Source_Type_t)type;
	HxSTD_snprintf((char*)oplNetflixObj.szURL, MAX_ENCORDED_URL_LEN, "%s", (char*)pucUrl);
    oplNetflixObj.ulChId=chID;
    oplNetflixObj.ulPrevChId=prevChid;
    oplNetflixObj.ulNextChId=nextChid;
    oplNetflixObj.enCategory=(NFLIX_Category_t)category;

	HxLOG_Debug("---> signal:onStartNetflixAppl\n");
	if (HAPI_BroadcastSignal("signal:onStartNetflixAppl", 0, "b", &oplNetflixObj,sizeof(Launch_Param_t)) != ERR_OK)
	{
		HxLOG_Error("Error> failed to (signal:onStartNetflixAppl)\n");
	}
#endif
    return ERR_OK;
}

bool OplNetflix::stopWithReason(int type)
{
#if defined(CONFIG_NETFLIX_APP)
	if (HAPI_BroadcastSignal("signal:onStopNetflixAppl", 0, "i", (NFLIX_Exit_Reason_Type_t)type) != ERR_OK)
	{
		HxLOG_Error("Error> failed to (signal:onStartNetflixAppl)\n");
	}
#endif
	return ERR_OK;
}

bool OplNetflix::notifyOverlayState(int state)
{
	printf("OplNetflix::notifyOverlayState-------------state(%d)\n", state);
#if defined(CONFIG_NETFLIX_APP)
	if (HAPI_BroadcastSignal("signal:onNotifyOverlayState", 0, "i", state) != ERR_OK)
	{
		HxLOG_Error("Error> failed to (signal:onStartNetflixAppl)\n");
	}
#endif
	return ERR_OK;
}

void OplNetflix::registerStatusChangedCallback(StatusChangedEvent_t callback)
{
	printf("[OplNetflix::callStatusChangedCallback]---regist!!!!!!!!!!!!!!\n");

	s_statusChangedCallback = callback;
}

void OplNetflix::callStatusChangedCallback(int status)
{
#if defined(CONFIG_NETFLIX_APP)
	NFLIX_NotifyState_t appState = (NFLIX_NotifyState_t)status;
	
	if (!s_statusChangedCallback)
	{
		HxLOG_Error("[OplNetflix::callStatusChangedCallback] Error> No registered callback!\n");
        return;
    }

    switch (appState) {
        case NFLIX_NOTIFY_START_NORMAL : 
            /*  Mheg off */
            HAPI_BroadcastSignal("signal:ChangeMhegStatusByOtherAppl", 0, "ii", FALSE, 1);
            HAPI_BroadcastSignal("signal:RequestMhegBlock", 0, "i", TRUE);
            s_statusChangedCallback(status);
            break;

        case NFLIX_NOTIFY_EXIT_NORMAL  : /* processed between webapp and netflixManager */
        case NFLIX_NOTIFY_EXIT_STANDBY : 
        case NFLIX_NOTIFY_EXIT_CH_UP   :
        case NFLIX_NOTIFY_EXIT_CH_DOWN :
        case NFLIX_NOTIFY_EXIT_DIAL    :
        case NFLIX_NOTIFY_EXIT_DIAL_OTHER_LAUNCH :
            /*  Mheg on */
            HAPI_BroadcastSignal("signal:ChangeMhegStatusByOtherAppl", 0, "i", TRUE);
            HAPI_BroadcastSignal("signal:RequestMhegBlock", 0, "i", FALSE);

            HxLOG_Error("[OplNetflix::callStatusChangedCallback] -- [status:%d]\n", status);
            s_statusChangedCallback(status);
            break;

        case NFLIX_NOTIFY_TRYTO_EXIT : /* processed between netflixManager and netflix, except webapp */

        default :
            break;
    }
#endif
	return;
}


