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
#define	NXSTART_COLDBOOT_TIMER_ID		0x002
#define	NXSTART_COLDBOOT_TIME			(1000) // msec
#define	DEF_BOOTDONE_WAIT_LIMIT			30

/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/
typedef struct NxStart_Context
{
	HBOOL			fGoStandby;
	HINT32			nBootDoneWaitCount;
} NxStart_Context_t;


/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/


static NxStart_Context_t		s_stStart_Contents;
static NXUI_DialogInstance_t 	s_prismStartProcessDlg;

/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/

static NxStart_Context_t*	nx_Start_GetContents (void );

static ONDK_Result_t 	nx_Start_MsgGwmDestroy(NxStart_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	nx_Start_MsgGwmCreate(NxStart_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	nx_Start_MsgGwmKeyDown(NxStart_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);

/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/

static NxStart_Context_t*	nx_Start_GetContents(void)
{
	return &s_stStart_Contents;
}

static ONDK_Result_t	nx_Start_ProcessingMsgProc(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return NX_PRISM_DIALOGUE_ProcessingMessage_Proc(&s_prismStartProcessDlg, message, hAction, p1, p2, p3);
}

static void nx_Start_ProcessingMsgCreate(void)
{
	NX_PRISM_DIALOGUE_SetDialog(&s_prismStartProcessDlg,
								ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis()),
								(NX_DLG_TYPE_NO_EXIT|NX_DLG_TIMEOUT_NOTIMEOUT|NX_DLG_NO_BUTTON|NX_DLG_TYPE_PM_ANIMAION),
								(HUINT8 *)NULL,
								(HUINT8 *)(HINT32)ONDK_GetString(RES_UPDATING_SYSTEM_TIME_ID));

	ONDK_GWM_APP_Create (ONDK_UPPER_WINDOW,
						"nx_Start_ProcessingMsgProc",
						APP_DIALOGBOX_PRIORITY,
						nx_Start_ProcessingMsgProc,
						0, 0, 0, 0);
}

static void nx_Start_ProcessingMsgDestroy(void)
{
	ONDK_GWM_APP_Destroy(nx_Start_ProcessingMsgProc);
}


static ONDK_Result_t	nx_Start_MsgGwmCreate(NxStart_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HBOOL bCheckColdBootDone = FALSE;
	SystemWakeUpReason_e eWakeupReason= eSYSTEM_WAKEUP_NONE;

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! Please check!! \n");
		return	ONDK_RET_FAIL;
	}
	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	HxLOG_Warning("#####################################\n");
	HxLOG_Warning("## NX_PRISM_START_Proc create\n");
	HxLOG_Warning("#####################################\n");

	/***************************************
	* Check Power Status
	***************************************/
	pstContents->fGoStandby = FALSE;
	pstContents->nBootDoneWaitCount = 0;

	if(NX_SYSTEM_IsPowerStateStandbyMode() == TRUE)
		pstContents->fGoStandby = TRUE;
	else
		pstContents->fGoStandby = FALSE;

	NX_PORT_SYSTEM_GetWakeupReason(&eWakeupReason);
	switch (eWakeupReason)
	{
		case eSYSTEM_WAKEUP_KEY:
		case eSYSTEM_WAKEUP_HDMICEC:
			pstContents->fGoStandby = FALSE;
			break;
		case eSYSTEM_WAKEUP_NONE:
		case eSYSTEM_WAKEUP_TIMER:
		case eSYSTEM_WAKEUP_ACPOWER:
		case eSYSTEM_WAKEUP_REBOOT:
			// nothing
			break;
		default:
			break;
	}

	/***************************************
	* Check boot done
	***************************************/
	NX_SYSTEM_GetColdBootDone(&bCheckColdBootDone);

#if defined(CONFIG_PROD_OS_EMULATOR) && !defined(CONFIG_SINGLE_PROCESS_MULTI_THREAD)
	bCheckColdBootDone = TRUE;
#endif

	HxLOG_Warning("## cold boot done status [%d]\n", bCheckColdBootDone);

	if (bCheckColdBootDone == TRUE)
	{
		ONDK_GWM_APP_Destroy(0);
	}
	else
	{
		ONDK_GWM_SetTimer(NXSTART_COLDBOOT_TIMER_ID, NXSTART_COLDBOOT_TIME);
		nx_Start_ProcessingMsgCreate();
	}


	/***************************************
	* Check AV Enable
	***************************************/
	if(pstContents->fGoStandby == FALSE)
	{
		NX_SYSTEM_SetAvEnable(TRUE);
	}

	return ONDK_RET_OK;
}

static ONDK_Result_t	nx_Start_MsgGwmKeyDown(NxStart_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
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

static ONDK_Result_t 	nx_Start_MsgGwmTimer(NxStart_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HBOOL bCheckColdBootDone = FALSE;

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	switch (p1)
	{
		case NXSTART_COLDBOOT_TIMER_ID:
			pstContents->nBootDoneWaitCount++;
			NX_SYSTEM_GetColdBootDone(&bCheckColdBootDone);
			HxLOG_Warning("# COLD BOOT TIMER wait cnt [%d/%d] - val(%d) \n", pstContents->nBootDoneWaitCount, DEF_BOOTDONE_WAIT_LIMIT, bCheckColdBootDone);
#if defined(CONFIG_PROD_OS_EMULATOR) && !defined(CONFIG_SINGLE_PROCESS_MULTI_THREAD)
			bCheckColdBootDone = TRUE;
#endif
			if (bCheckColdBootDone == TRUE || pstContents->nBootDoneWaitCount > DEF_BOOTDONE_WAIT_LIMIT)
			{
				nx_Start_ProcessingMsgDestroy();
				ONDK_GWM_KillTimer(NXSTART_COLDBOOT_TIMER_ID);
				ONDK_GWM_APP_Destroy(0);
			}
			break;
		default:
			break;
	}

	return ONDK_RET_OK;

}

static ONDK_Result_t	nx_Start_MsgGwmDestroy(NxStart_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR	hErr = ERR_FAIL;
	HBOOL	fConsumed = FALSE, bFirstTimeBoot= FALSE;

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! Please check!! \n");
		return	ONDK_RET_FAIL;
	}
	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	HxLOG_Warning("#####################################\n");
	HxLOG_Warning("## NX_PRISM_START_Proc destroy\n");
	HxLOG_Warning("#####################################\n");

	// 1. go to standby on
	if (!fConsumed)
	{
		if (pstContents->fGoStandby == TRUE)
		{
			HxLOG_Warning("## goto Standby \n");
			ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, "NX_PRISM_STANDBY_Proc", APP_DEFAULT_POPUP_PRIORITY, NX_PRISM_STANDBY_Proc, 0, p1, p2, p3);
			fConsumed = TRUE;
		}
		else
		{
			HxLOG_Warning("## goto Operation \n");
			NX_SYSTEM_RequestOperation();
		}
	}

	// 2. go to operation with installation wizard
	if (!fConsumed)
	{
		hErr = NX_SYSTEM_GetFirstTimeBoot(&bFirstTimeBoot);
		if ((hErr != ERR_OK) || (bFirstTimeBoot == TRUE))
		{
			HxLOG_Warning("## goto installation wizard [%d : %d] \n", hErr, bFirstTimeBoot);
            ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, "NX_PRISM_Install_Proc", (APP_DEFAULT_PRIORITY | UISTATE_INSTALLWIZARD), NX_PRISM_Install_Proc, 0, p1, p2, p3);
			fConsumed = TRUE;
		}
	}

	// 3. go to operation with live
	if (!fConsumed)
	{
		HxLOG_Warning("## goto live \n");
		ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, "NX_PRISM_MgrInit_Proc", APP_UIMGR_PRIORITY, NX_PRISM_MgrInit_Proc, 0, p1, p2, p3);
		fConsumed = TRUE;
	}

	return	ONDK_MESSAGE_PASS;
}

static ONDK_Result_t	nx_Start_MsgGwmHapiMsg(NxStart_Context_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! Please check!! \n");
		return	ONDK_RET_FAIL;
	}
	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	return	ONDK_MESSAGE_PASS;
}


/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/
ONDK_Result_t		NX_PRISM_START_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t		eResult	= ONDK_RET_OK;
	NxStart_Context_t *pstContents = nx_Start_GetContents();

	HxLOG_Print("event: %08x [%s]\n", lMessage, ONDK_GWM_GwmMsg2String(lMessage));

	switch (lMessage)
	{
		case MSG_GWM_CREATE:
			eResult = nx_Start_MsgGwmCreate(pstContents, p1, p2, p3);
			break;
		case MSG_GWM_KEYUP:
			break;
		case MSG_GWM_KEYDOWN:
			eResult = nx_Start_MsgGwmKeyDown(pstContents, p1, p2, p3);
			break;
		case MSG_GWM_TIMER:
			eResult = nx_Start_MsgGwmTimer(pstContents, p1, p2, p3);
			break;
		case MSG_GWM_DESTROY:
			eResult = nx_Start_MsgGwmDestroy(pstContents, p1, p2, p3);
			break;
		case MSG_GWM_HAPI_MESSAGE:
			eResult = nx_Start_MsgGwmHapiMsg(pstContents, handle, p1, p2, p3);
			break;
		case MSG_GWM_APK_MESSAGE :
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


