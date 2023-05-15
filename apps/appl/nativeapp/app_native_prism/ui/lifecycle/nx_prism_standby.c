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
 * @file	  		nx_prism_standby.c
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
#include <nx_port.h>
#include <nx_core_message.h>


/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/
#define	NX_PRISM_STANDBY_TRANSITION_TIMER_ID			1
#define	NX_PRISM_STANDBY_TRANSITION_TIME				(500) // 500 mSec


typedef enum
{
	eNX_STANDBY_STATUS_NORMAL =0,
	eNX_STANDBY_STATUS_GOTOWAKEUP
} NXStandby_Status_e;


/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/
typedef struct NxStandby_Context
{
	NXStandby_Status_e			eStatus;
} NxStandby_Context_t;


/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/


static NxStandby_Context_t		s_stStandby_Contents;


/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/

static NxStandby_Context_t*	nx_Standby_GetContents (void );

static void		nx_Standby_GoOperation(HINT32 p1, HINT32 p2, HINT32 p3);

static ONDK_Result_t 	nx_Standby_MsgGwmDestroy(NxStandby_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	nx_Standby_MsgGwmCreate(NxStandby_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	nx_Standby_MsgGwmKeyDown(NxStandby_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	nx_Standby_MsgGwmTimer(NxStandby_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_Standby_MsgGwmHapiMsg(NxStandby_Context_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_Standby_MsgGwmPowerStateChangedMsg(NxStandby_Context_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);


/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/

static NxStandby_Context_t*	nx_Standby_GetContents(void)
{
	return &s_stStandby_Contents;
}


static void		nx_Standby_GoOperation(HINT32 p1, HINT32 p2, HINT32 p3)
{
	HxLOG_Warning("################################################\n");
	HxLOG_Warning("Go to Operation \n");
	HxLOG_Warning("################################################\n");

	HxLOG_Warning("#Step 1 \n");
	NX_SYSTEM_SetAvEnable(TRUE);

	HxLOG_Warning("#Step 2 \n");
	ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, "NX_PRISM_START_Proc", APP_UIMGR_PRIORITY, NX_PRISM_START_Proc, 0, p1, p2, p3);

	HxLOG_Warning("#Step 3 \n");
	ONDK_KEY_Active(TRUE);

	HxLOG_Warning("#Step DONE \n");
}


static ONDK_Result_t	nx_Standby_MsgGwmCreate(NxStandby_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! Please check!! \n");
		return	ONDK_RET_FAIL;
	}

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	HxSTD_MemSet(pstContents, 0, sizeof(NxStandby_Context_t));
	pstContents->eStatus = eNX_STANDBY_STATUS_NORMAL;

	// TODO: destroy Activated hbbtv window or App
	// TODO: stop AV - but actually the stop action has to triggered in owner proc. (ex, the live av stop has to called in nx_live)
	// TODO: stop PB/TSR/ChasePB - same with the above.

	HxLOG_Warning("################################################\n");
	HxLOG_Warning("Create Standby \n");
	HxLOG_Warning("################################################\n");

	HxLOG_Warning("#Step 1 \n");
	ONDK_KEY_Active(FALSE);

	HxLOG_Warning("#Step 2 \n");
	NX_SYSTEM_SetAvEnable(FALSE);

	HxLOG_Warning("#Step 3 \n");
	NX_SYSTEM_RequestShutDown();

	HxLOG_Warning("#Step 4 \n");

	HxLOG_Warning("#Step Done \n");

	return ONDK_RET_OK;
}

static ONDK_Result_t	nx_Standby_MsgGwmKeyDown(NxStandby_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32		lKeyId = p1;

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! Please check!! \n");
		return	ONDK_RET_FAIL;
	}

	switch (lKeyId)
	{
		case KEY_STANDBY:
			break;
		default:
			break;
	}

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	return ONDK_MESSAGE_BREAK;
}

static ONDK_Result_t 	nx_Standby_MsgGwmTimer(NxStandby_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32		ulTimerID = p1;

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	if (ulTimerID == NX_PRISM_STANDBY_TRANSITION_TIMER_ID)
	{
		ONDK_GWM_APP_Destroy(0);
		return	ONDK_MESSAGE_BREAK;
	}

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_Standby_MsgGwmDestroy(NxStandby_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! Please check!! \n");
		return	ONDK_RET_FAIL;
	}
	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	HxLOG_Warning("################################################\n");
	HxLOG_Warning("Destroy Standby \n");
	HxLOG_Warning("################################################\n");

	nx_Standby_GoOperation(p1, p2, p3);

	return	ONDK_MESSAGE_PASS;
}

static ONDK_Result_t	nx_Standby_MsgGwmHapiMsg(NxStandby_Context_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! Please check!! \n");
		return	ONDK_RET_FAIL;
	}
	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	return	ONDK_MESSAGE_PASS;
}

static ONDK_Result_t	nx_Standby_MsgGwmPowerStateChangedMsg(NxStandby_Context_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HBOOL						bStandbyMode = FALSE;
	GWM_APK_MSG_Type_e			eAPK_Type = (GWM_APK_MSG_Type_e)handle;

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! Please check!! \n");
		return	ONDK_RET_FAIL;
	}
	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	HxLOG_Warning("### Power State Changed Received \n");

	if (eAPK_Type == GWM_APK_POWER)
	{
		HxLOG_Warning("[%s:%d] power event received:\n", __FUNCTION__, __LINE__);

		bStandbyMode = NX_SYSTEM_IsStandbyMode();
		// Alreay standby mode value was changed so this check function is right
		if (bStandbyMode == FALSE)
		{
			if (pstContents->eStatus == eNX_STANDBY_STATUS_NORMAL)
			{
				pstContents->eStatus = eNX_STANDBY_STATUS_GOTOWAKEUP;
				HxLOG_Warning("#1 request Operation \n");
				NX_SYSTEM_RequestOperation();

				HxLOG_Warning("#2 set Transition timer about %d mSec\n", NX_PRISM_STANDBY_TRANSITION_TIME);
				ONDK_GWM_SetTimer(NX_PRISM_STANDBY_TRANSITION_TIMER_ID, NX_PRISM_STANDBY_TRANSITION_TIME);
			}
		}
	}

	return	ONDK_MESSAGE_PASS;
}


/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/
ONDK_Result_t		NX_PRISM_STANDBY_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t		eResult	= ONDK_RET_OK;
	NxStandby_Context_t *pstContents = nx_Standby_GetContents();

	HxLOG_Print("event: %08x [%s]\n", lMessage, ONDK_GWM_GwmMsg2String(lMessage));

	switch (lMessage)
	{
		case MSG_GWM_CREATE:
			eResult = nx_Standby_MsgGwmCreate(pstContents, p1, p2, p3);
			break;
		case MSG_GWM_KEYUP:
			break;
		case MSG_GWM_KEYDOWN:
			eResult = nx_Standby_MsgGwmKeyDown(pstContents, p1, p2, p3);
			break;
		case MSG_GWM_TIMER:
			eResult = nx_Standby_MsgGwmTimer(pstContents, p1, p2, p3);
			break;
		case MSG_GWM_DESTROY:
			eResult = nx_Standby_MsgGwmDestroy(pstContents, p1, p2, p3);
			break;
		case MSG_GWM_HAPI_MESSAGE:
			eResult = nx_Standby_MsgGwmHapiMsg(pstContents, handle, p1, p2, p3);
			break;
		case MSG_GWM_APK_MESSAGE :
			eResult = nx_Standby_MsgGwmPowerStateChangedMsg(pstContents, handle, p1, p2, p3);
			break;
		default:
			break;
	}

	if(eResult != ONDK_RET_OK)
	{
		return	eResult;
	}

	return ONDK_MESSAGE_PASS;
}


/******************************************************************************/
#define _________End_Of_File____________________________________________________
/******************************************************************************/


