/**************************************************************
*	@file	dial.c
*	http://www.humaxdigital.com
*	@author			humax
*	@brief			DLNA / DIAL
**************************************************************/

/*
* ⓒ 2011-2012 Humax Co., Ltd.
* This program is produced by Humax Co., Ltd. ("Humax") and
* the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
* non-assignable, non-transferable and non-exclusive license to use this Software.
* You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
* you agree to the responsibility to take all reasonable efforts to protect the any information
* you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
* reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
* If you have no authorized license, discontinue using this Software immediately.
*
* THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS,
* IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
* IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
*  CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.
*
* This License is effective until terminated. You may terminate this License at any time by destroying all copies
 of the Software including all documentation. This License will terminate immediately without notice from Humax
* to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
* of the Software and all documentation.
*
* The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
* All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
* in the Republic of Korea.
*/

#define	________________Header_Files_______________________________ 
#include "pl_dial.h"
#include "dial.h"

#define ________________Defines____________________________________


#define ________________Static_Variables___________________________
static DIAL_EventCallback_t s_dial_event_cb = NULL;
static HBOOL	s_bIsStart = FALSE;


static PL_DIAL_Status __start_cb(const HCHAR *appname, const HCHAR * appUrl, const HCHAR *args, size_t arglen);
static PL_DIAL_Status __status_cb(const HCHAR *appname, HINT32 *pCanStop);
static void __stop_cb(const HCHAR *appname);

struct DIALAppCallbacks appHandler = {__start_cb, __stop_cb, __status_cb};


#define ________________Internal_Functions_________________________ 
/*  아래 callback 을 app 마다 구성 할지, 통합할지는 다음 app 등록시 검토하기로함.  */
static PL_DIAL_Status __start_cb(const HCHAR *appname, const HCHAR * appUrl, const HCHAR *args, size_t arglen)
{
	HCHAR payload[DIAL_MAX_PAYLOAD + 1] = {0,};

	if (!strcmp(appname, "YouTube")) {
		
		// Original: "https://www.youtube.com/tv?%s&additionalDataUrl=http://localhost:56789/dialData?app=YouTube"
		HxSTD_snprintf(payload, sizeof(payload), "https://www.youtube.com/tv?%s&additionalDataUrl=http%%3A%%2F%%2Flocalhost%%3A56789%%2FdialData%%3Fapp%%3DYouTube", args);
		
	} else if (!strcmp(appname, "Netflix")) {
		
		HxSTD_snprintf(payload, sizeof(payload), "%s", args);

	} else if (!strncmp(appname, "com.humaxtvportal", strlen("com.humaxtvportal"))) {

		// Humax extension
		
		HCHAR temp[2048] = {0,};
		if (args && args[0] != 0) {
			snprintf(temp, sizeof(temp), "%s%s", HxSTD_StrStr(appUrl, "?") ? "&" : "?", args);
		}		
		HxSTD_snprintf(payload, sizeof(payload), "%s%s", appUrl, temp);
		
	} else {
		
		HxSTD_snprintf(payload, sizeof(payload), "%s", args);
	}

    HxLOG_Error("\n\n ** LAUNCH %s - %s  **\n", appname, payload);

	if(s_dial_event_cb) {
	   	s_dial_event_cb(DIAL_APP_LAUNCH, appname, (void *)payload);
	}

	return DIAL_Status_Running;
}

static PL_DIAL_Status __status_cb(const HCHAR *appname, HINT32 *pCanStop) 
{
    *pCanStop = 1;
	HxLOG_Error( "\n\n ** STATUS %s ** \n\n", appname);
	return DIAL_Status_Stop;
}

static void __stop_cb(const HCHAR *appname) 
{
    HxLOG_Error("\n\n ** STOP %s **\n\n", appname);
	if(s_dial_event_cb) {
	   	s_dial_event_cb(DIAL_APP_STOP, appname, NULL);
	}
}

#define ________________Public_Interfaces__________________________
HERROR 	DIAL_Init(HCHAR *pszFriendlyName)
{
	HxLOG_Print("#########################################################################\n");
	HxLOG_Print("#### DIAL Server Init \n");
	HxLOG_Print("#########################################################################\n");

	if(s_bIsStart == FALSE)
	{
		if(ERR_OK != PL_DIAL_start(pszFriendlyName))
			return ERR_FAIL;
	}
	else 
		return ERR_FAIL;

	s_bIsStart = TRUE;

	PL_DIAL_registerAppHandler(&appHandler);

	return ERR_OK;
}

HERROR	DIAL_Deinit(void)
{
    PL_DIAL_stop();
	s_bIsStart = FALSE;
    return ERR_OK;
}

HERROR	DIAL_SetApplicationURL(void)
{
	HxLOG_Print("#########################################################################\n");
	HxLOG_Print("#### DIAL Server SetApplicationURL \n");
	HxLOG_Print("#########################################################################\n");

	if(ERR_OK != PL_DIAL_SetApplicationURL())
		return ERR_FAIL;

	return ERR_OK;
}

HERROR	DIAL_RegisterCallback(DIAL_EventCallback_t cb)
{
	s_dial_event_cb = cb;
    return ERR_OK;
}

HERROR	DIAL_SetFriendlyName(const HCHAR *pszFriendlyName)
{
	return PL_DIAL_SetFriendlyName(pszFriendlyName);
}

HERROR	DIAL_UnregisterCallback(void)
{
	s_dial_event_cb = NULL;
    return ERR_OK;
}

HERROR	DIAL_RegisterApplication(const HCHAR *name)
{
    //struct DIALAppCallbacks cb = {__start_cb, __stop_cb, __status_cb};
    //return PL_DIAL_register_app(name, &cb);
	return PL_DIAL_register_app(name);
}

HERROR	DIAL_UnregisterApplication(const HCHAR *name)
{
    return PL_DIAL_unregister_app(name);
}

HERROR	DIAL_ApplicationStatusUpdate(const HCHAR *name, DIAL_State_e status)
{
	return PL_DIAL_update_state_app(name, status);
}

HBOOL	DIAL_IsStart(void)
{
	return s_bIsStart;
}

HERROR	DIAL_SetCookie(const HCHAR * cookie) {
	PL_DIAL_setCookie(cookie);
	return ERR_OK;
}

HERROR DIAL_GetFriendlyName(HCHAR * pFriendlyName, HUINT32 size) {
	return PL_DIAL_GetFriendlyName(pFriendlyName, size);
}

HERROR DIAL_GetAdditionalDataUrl(HCHAR * pAdditionalDataUrl, HUINT32 size, HCHAR * appName) {
	return PL_DIAL_GetAdditionalDataUrl(pAdditionalDataUrl, size, appName);
}
