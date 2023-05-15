/*
 * (c) 2011-2013 Humax Co., Ltd.
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
 * CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.

 * This License is effective until terminated. You may terminate this License at any time by destroying all copies
 * of the Software including all documentation. This License will terminate immediately without notice from Humax
 * to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
 * of the Software and all documentation.

 * The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
 * All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
 * in the Republic of Korea.
*/

/******************************************************************************/
/**
 * @file	  		nx_prism_no_service.c
 *
 * 	Description:  													\n
 *
 * @author															\n
 * @date															\n
 * @Attention		Copyright (c) 2015 HUMAX Co., Ltd. - All rights reserved.
 *
 **/
/******************************************************************************/

/******************************************************************************/
#define _________Private_Include______________________________________________
/******************************************************************************/
#include <nx_main.h>
#include <nx_core.h>
#include <nx_port.h>

#include <nx_common.h>
#include <nx_core_message.h>
#include <nx_prism_app.h>

#include "nx_prism_definition.h"

/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/

/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/
typedef struct
{
	HINT32	ver;
}NxUiNoSvcContents_t;

/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/
static NxUiNoSvcContents_t	s_UiNoSvcContents;

/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/
static NxUiNoSvcContents_t* 	nx_no_service_GetContents(void);
static ONDK_Result_t 	nx_no_service_MsgGwmCreate(NxUiNoSvcContents_t *pstContents, Handle_t hAct, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	nx_no_service_MsgGwmDestroy(NxUiNoSvcContents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);

/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/
static NxUiNoSvcContents_t*	nx_no_service_GetContents(void)
{
	return &s_UiNoSvcContents;
}

static ONDK_Result_t	nx_no_service_MsgGwmCreate(NxUiNoSvcContents_t *pstContents, Handle_t hAct, HINT32 p1, HINT32 p2, HINT32 p3)
{
	NX_PRISM_Dialogue_DestroyNoService();	
	HxSTD_MemSet(pstContents, 0, sizeof(NxUiNoSvcContents_t));
	NX_PRISM_Dialogue_CreateNoService();

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);
	return	ONDK_RET_OK;
}


static ONDK_Result_t nx_no_service_MsgGwmDestroy(NxUiNoSvcContents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	NX_PRISM_Dialogue_DestroyNoService();

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);
	return	ONDK_RET_OK;
}


/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/
ONDK_Result_t		NX_PRISM_NO_SERVICE_MGR_Proc(HINT32 lMessage, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t			lRet = ONDK_RET_OK;
	NxUiNoSvcContents_t		*pstContents = nx_no_service_GetContents();
	HxLOG_Print("Message(%d) : %s, handle: %p p1(0x%x) p2(0x%x) p3(0x%x) \n", lMessage, ONDK_GWM_GwmMsg2String(lMessage), p1, p2, p3);

	switch(lMessage)
	{
		case MSG_GWM_CREATE:
			HxLOG_Warning("MSG_GWM_CREATE IN NX_PRISM_NO_SERVICE_Proc\n");
			nx_no_service_MsgGwmCreate(pstContents, hHandle, p1, p2, p3);
			break;

		case MSG_GWM_TIMER:
			break;
		case MSG_GWM_APK_MESSAGE:
			break;
		case MSG_GWM_KEYDOWN:
			break;
		case MSG_GWM_KEYUP:
			break;

		case MSG_GWM_APP_SPAWNED:
			if(NX_PRISM_UTIL_GetUiState() != UISTATE_NORMAL)
			{
				NX_PRISM_Dialogue_DestroyNoService();	
			}
			break;

		case MSG_GWM_APP_KILLED:
			if(NX_PRISM_UTIL_GetUiState() == UISTATE_NORMAL)
			{
				NX_PRISM_Dialogue_CreateNoService();	
			}		
			break;

		case MSG_GWM_DESTROY:
			HxLOG_Warning("MSG_GWM_DESTROY IN NX_PRISM_NO_SERVICE_Proc\n");
			lRet = nx_no_service_MsgGwmDestroy(pstContents, p1, p2, p3);
			break;
	}

	if(lRet != ONDK_RET_OK)
	{
		return lRet;
	}

	return ONDK_GWM_ProcessMessageDefault(lMessage, hHandle, p1, p2, p3);
}

/******************************************************************************/
#define _________End_Of_File____________________________________________________
/******************************************************************************/
