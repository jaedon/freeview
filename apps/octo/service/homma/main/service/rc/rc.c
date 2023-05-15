/**************************************************************
*	@file	rc.c
*	http://www.humaxdigital.com
*	@author			humax
*	@brief			DLNA / RC
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
#include "pl_rc.h"
#include "rc.h"

#define ________________Defines____________________________________

#define ________________Static_Variables___________________________
static RC_EventCallback_t s_rc_event_cb = NULL;
static HBOOL	s_bIsStart = FALSE;
static HBOOL	s_bIsPaired = FALSE;


#define ________________Internal_Functions_________________________

static PL_RC_Status __pair_cb(const HCHAR *pMsg, const HCHAR *pName)
{    
    HxLOG_Error( "\n\n ** PAIR => %s / %s ** \n\n", pMsg, pName);

	if(s_rc_event_cb)
	   	s_rc_event_cb(RC_CLIENT_PAIRED, (void*)pMsg, (void*)pName);

	return RC_Status_Paired;
}

static PL_RC_Status __unpair_cb(const HCHAR *pMsg) 
{    
	HxLOG_Error( "\n\n ** UNPAIR => %s ** \n\n", pMsg);

	if(s_rc_event_cb)
	   	s_rc_event_cb(RC_CLIENT_UNPAIRED, (void*)pMsg, NULL);

	return RC_Status_Unpaired;
}

static void __message_cb(const HCHAR *pMsg) 
{
    HxLOG_Error("\n\n ** CustomerMessage: %s **\n\n", pMsg);

	if(s_rc_event_cb)
	   	s_rc_event_cb(RC_CLIENT_MESSAGE, (void*)pMsg, NULL);
}

static void __launch_cb(const HCHAR *pUrl) 
{
    HxLOG_Error("\n\n ** launch url: %s **\n\n", pUrl);

	if(s_rc_event_cb)
	   	s_rc_event_cb(RC_LAUNCH_APP, (void*)pUrl, NULL);
}

static void __set_friendly_name_cb(const HCHAR *pName) 
{
    HxLOG_Error("\n\n ** set_friendlyname: %s **\n\n", pName);

	if(s_rc_event_cb)
	   	s_rc_event_cb(RC_SET_FRIENDLY_NAME, (void*)pName, NULL);
}


#define ________________Public_Interfaces__________________________
HERROR 	RC_Init(HCHAR *pszFriendlyName)
{
	struct RCClientCallbacks cb = {__pair_cb, __unpair_cb, __message_cb, __launch_cb, __set_friendly_name_cb};
	
	HxLOG_Print("#########################################################################\n");
	HxLOG_Print("#### RC Init \n");
	HxLOG_Print("#########################################################################\n");

	if(s_bIsStart == FALSE)
	{		
		if(ERR_OK != PL_RC_start(pszFriendlyName, &cb))
			return ERR_FAIL;
	}
	else
	{
		return ERR_FAIL;
	}

	s_bIsStart = TRUE;

	return ERR_OK;
}

HERROR	RC_Deinit(void)
{
	PL_RC_stop();
	s_bIsStart = FALSE;
	return ERR_OK;
}

/*
HERROR	RC_SetApplicationURL(void)
{
	HxLOG_Print("#########################################################################\n");
	HxLOG_Print("#### RC Server SetApplicationURL \n");
	HxLOG_Print("#########################################################################\n");

	if(ERR_OK != PL_RC_SetApplicationURL())
		return ERR_FAIL;

	return ERR_OK;
}
*/

HERROR	RC_UnpairClient(void)
{
	PL_RC_UnpairClient();
	s_bIsPaired = FALSE;
	return ERR_OK;
}

HERROR 	RC_SetPowerStatus(HBOOL bOperating)
{
	return PL_RC_SetPowerStatus(bOperating);
}


HERROR	RC_RegisterCallback(RC_EventCallback_t cb)
{
	s_rc_event_cb = cb;
    return ERR_OK;
}

HERROR	RC_SetFriendlyName(const HCHAR *pszFriendlyName)
{
	return PL_RC_SetFriendlyName(pszFriendlyName);
}

HERROR	RC_UnregisterCallback(void)
{
	s_rc_event_cb = NULL;
    return ERR_OK;
}

HBOOL	RC_IsStart(void)
{
	return s_bIsStart;
}
