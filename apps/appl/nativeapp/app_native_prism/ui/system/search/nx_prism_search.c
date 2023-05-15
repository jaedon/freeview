/**************************************************************
*	@file		ap_startlogo.c
*	Minimized Graphic Library for DirectFB
*
*	http://www.humaxdigital.com
*
*	@data			2012/09/17
*	@author			humax
*	@brief			Minimized Graphic Library for DirectFB
**************************************************************/


/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding À» »ç¿ëÇÏ´Â source file ÀÔ´Ï´Ù.
* MS949 encodingªòÞÅéÄª·ªÆ source fileªÇª¹¡£
* Quelldatei, die MS949-Codierung verwendet.
*
*/


/*
* ¨Ï 2011-2012 Humax Co., Ltd.
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
* of the Software including all documentation. This License will terminate immediately without notice from Humax
* to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
* of the Software and all documentation.
*
* The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
* All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
* in the Republic of Korea.
*/

/**************************************************************************************************/
#define _________PRISM_SEARCH_Private_Include_________________________________________________
/**************************************************************************************************/
#include <nx_main.h>
#include <nx_core.h>
#include <nx_port.h>
#include <nx_port_system.h>
#include <nx_prism_app.h>

#include "nx_prism_definition.h"


typedef struct
{
	HINT32	aAntUid[4];
}NxPrismSearchContext_t;

NxPrismSearchContext_t *s_PrismSearchCtx = NULL;


/**************************************************************************************************/
#define _________PRISM_SEARCH_Private_Prototype_________________________________________________
/**************************************************************************************************/
NxPrismSearchContext_t *GetPrismSearchContext(void);

/**************************************************************************************************/
#define _________PRISM_SEARCH_Private_Prototype_________________________________________________
/**************************************************************************************************/
NxPrismSearchContext_t *GetPrismSearchContext(void)
{
	if (s_PrismSearchCtx == NULL)
	{
		s_PrismSearchCtx = NX_APP_Calloc(sizeof(NxPrismSearchContext_t));
	}
	return s_PrismSearchCtx;
}

static ONDK_Result_t nx_ui_search_MsgGwmDestroy(NxPrismSearchContext_t *pstContext, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HxLOG_Trace();

	ONDK_WINDOW_Clear(ONDK_GWM_GetWindow(ONDK_UPPER_WINDOW));
	ONDK_WINDOW_Update(ONDK_GWM_GetWindow(ONDK_UPPER_WINDOW),NULL);
	return ONDK_RET_OK;
}

static ONDK_Result_t nx_ui_search_MsgGwmKeyDown(NxPrismSearchContext_t *pstContext, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32 nKeyId = p1;

	switch (nKeyId)
	{

	case KEY_OK:
	case KEY_ARROWUP:
	case KEY_ARROWDOWN:
		return	ONDK_RET_OK;

	case KEY_STANDBY:
		return ONDK_MESSAGE_NONCONSUMED;
	default:
		return ONDK_MESSAGE_PASS;
	}
}

/**************************************************************************************************/
#define _________PRISM_SEARCH_Public_Functions__________________________________________________________
/**************************************************************************************************/

ONDK_Result_t		NX_PRISM_SEARCH_Proc(HINT32 lMessage, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t			lRet = ONDK_RET_OK;
	NxPrismSearchContext_t	*pstContext = GetPrismSearchContext();
	ONDK_Print(HxANSI_COLOR_YELLOW("[%s:%d] Message(%d) : %s, handle: %p p1(0x%x) p2(0x%x) p3(0x%x) \n"), __FUNCTION__, __LINE__, lMessage, ONDK_GWM_GwmMsg2String(lMessage), p1, p2, p3);

	switch(lMessage)
	{
		case MSG_GWM_CREATE:
			break;
		case MSG_GWM_TIMER:
			break;

		case MSG_GWM_HAPI_MESSAGE:
			lRet = ONDK_MESSAGE_PASS;
			break;

		case MSG_GWM_CLICKED :
			break;

		case MSG_GWM_KEYDOWN:
			lRet = nx_ui_search_MsgGwmKeyDown(pstContext, p1, p2, p3);
			break;
		case MSG_GWM_KEYUP:
			break;

		case MSG_GWM_OBJ_FOCUS_CHANGED:
			lRet = ONDK_RET_OK;
			break;

		case MSG_GWM_DESTROY:
			lRet = nx_ui_search_MsgGwmDestroy(pstContext, p1, p2, p3);
			break;
	}

	if(lRet != ONDK_RET_OK)
	{
		return lRet;
	}

	return ONDK_GWM_ProcessMessageDefault(lMessage, hHandle, p1, p2, p3);
}

