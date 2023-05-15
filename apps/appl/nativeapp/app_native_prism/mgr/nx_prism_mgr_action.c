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
 * @file	  		nx_prism_mgr_action.c
 *
 * 	Description:  													\n
 *
 * @author															\n
 * @date															\n
 * @Attention		Copyright (c) 2013 HUMAX Co., Ltd. - All rights reserved.
 *
 **/
/******************************************************************************/


/******************************************************************************/
#define _________Private_Include______________________________________________
/******************************************************************************/
#include <nx_common.h>
#include <nx_prism_app.h>


/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/


/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/


/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/


/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/
static ONDK_Result_t	nx_prism_MgrAction_CreateBGAction(Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);

static ONDK_Result_t	nx_prism_MgrAction_MsgGwmCreate(Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	nx_prism_MgrAction_MsgGwmDestroy(HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_prism_MgrAction_MsgGwmHapiMsg(Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_prism_MgrAction_MsgGwmkeyDown(HINT32 message, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);


/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/

static ONDK_Result_t	nx_prism_MgrAction_CreateBGAction(Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	// Reservion
	ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, "NX_PRISM_SYSTEM_RSV_Proc", APP_RSV_MGR_PRIORITY, NX_PRISM_SYSTEM_RSV_Proc, handle, p1, p2, p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_prism_MgrAction_MsgGwmCreate(Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	nx_prism_MgrAction_CreateBGAction(handle, p1, p2, p3);

	ONDK_GWM_APP_SetUIHideFlag(GWM_UI_Hide_Never);

	NX_COMMON_NOT_USED_ARGUMENT(NULL, p1, p2, p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_prism_MgrAction_MsgGwmDestroy(HINT32 p1, HINT32 p2, HINT32 p3)
{
	// Reservion
	ONDK_GWM_APP_Destroy((ONDK_GWM_Callback_t)NX_PRISM_SYSTEM_RSV_Proc);

	NX_COMMON_NOT_USED_ARGUMENT(NULL, p1, p2, p3);
	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_prism_MgrAction_MsgGwmHapiMsg(Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	NX_COMMON_NOT_USED_ARGUMENT(NULL, p1, p2, p3);
	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_prism_MgrAction_MsgGwmApkMsg(Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	NX_COMMON_NOT_USED_ARGUMENT(NULL, p1, p2, p3);
	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_prism_MgrAction_MsgGwmkeyDown(HINT32 message, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HBOOL	bGoStandby = FALSE;

	NX_PRISM_UTIL_KeyConv_UiCommon(&p1);

	switch (p1)
	{
		case KEY_STANDBY:
			bGoStandby = NX_PRISM_SPEC_GetGoPassiveStandby();
			if (bGoStandby == TRUE)
			{
				ONDK_GWM_APP_Destroy(0);
			}
			return ONDK_MESSAGE_PASS;
		default:
			break;
	}

	return ONDK_RET_OK;
}


/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/
ONDK_Result_t	NX_PRISM_MgrAction_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t		eResult = ONDK_RET_OK;

	switch (lMessage)
	{
		case MSG_GWM_CREATE:
			eResult = nx_prism_MgrAction_MsgGwmCreate(handle, p1, p2, p3);
			break;

		case MSG_GWM_KEYDOWN:
			eResult = nx_prism_MgrAction_MsgGwmkeyDown(lMessage, handle, p1, p2, p3);
			break;

		case MSG_GWM_TIMER:
			break;

		case MSG_GWM_HAPI_MESSAGE:
			eResult = nx_prism_MgrAction_MsgGwmHapiMsg(handle, p1, p2, p3);
			break;

		case MSG_GWM_APK_MESSAGE:
			eResult = nx_prism_MgrAction_MsgGwmApkMsg(handle, p1, p2, p3);
			break;

		case MSG_GWM_DESTROY:
			nx_prism_MgrAction_MsgGwmDestroy(p1, p2, p3);
			break;

		default:
			break;
	}

    if (eResult != ONDK_RET_OK)
    {
        return eResult;
    }

	return	ONDK_GWM_ProcessMessageDefault(lMessage, handle, p1, p2, p3);
}


/******************************************************************************/
#define _________End_Of_File____________________________________________________
/******************************************************************************/

