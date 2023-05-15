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
 * @file	  		nx_prism_not_available.c
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
#include <nx_port_system.h>


/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/
#define	NX_PRISM_NOTAVAILABLE_EXPIRE_TIMER_ID			0x001
#define	NX_PRISM_NOTAVAILABLE_EXPIRE_TIME				(3 * NX_COMMON_TIME_SECOND)

// Object ID
#define	NX_PRISM_NOTAVAILABLE_DIM_ID					1
#define	NX_PRISM_NOTAVAILABLE_ID						2

// ui frame
#define	NX_PRISM_NOTAVAILABLE_FRAME_X					1087
#define	NX_PRISM_NOTAVAILABLE_FRAME_Y					4
#define	NX_PRISM_NOTAVAILABLE_FRAME_W					170
#define	NX_PRISM_NOTAVAILABLE_FRAME_H					170

// dim
#define	NX_PRISM_NOTAVAILABLE_DIMICON_X					0
#define	NX_PRISM_NOTAVAILABLE_DIMICON_Y					0
#define	NX_PRISM_NOTAVAILABLE_DIMICON_W					170
#define	NX_PRISM_NOTAVAILABLE_DIMICON_H					170

// icon
#define	NX_PRISM_NOTAVAILABLE_ICON_X					40
#define	NX_PRISM_NOTAVAILABLE_ICON_Y					40
#define	NX_PRISM_NOTAVAILABLE_ICON_W					90
#define	NX_PRISM_NOTAVAILABLE_ICON_H					90


/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/
typedef struct NxVolume_Context
{
	HUINT32			ulDummy;
} NxNotAvailable_Context_t;


/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/


static NxNotAvailable_Context_t		s_stNotAvailable_Contents;


/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/

static NxNotAvailable_Context_t*	nx_notAvailable_GetContents (void );
static ONDK_Result_t 	nx_notAvailable_SetUiObject(NxNotAvailable_Context_t *pstContents);

static ONDK_Result_t 	nx_notAvailable_MsgGwmDestroy(NxNotAvailable_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	nx_notAvailable_MsgGwmCreate(NxNotAvailable_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	nx_notAvailable_MsgGwmKeyUp(NxNotAvailable_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	nx_notAvailable_MsgGwmKeyDown(NxNotAvailable_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);

/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/

static NxNotAvailable_Context_t*	nx_notAvailable_GetContents(void)
{
	return &s_stNotAvailable_Contents;
}

static ONDK_Result_t nx_notAvailable_Destroy(NxNotAvailable_Context_t *pstContents)
{
	ONDK_GWM_KillTimer(NX_PRISM_NOTAVAILABLE_EXPIRE_TIMER_ID);

	NX_COMMON_NOT_USED_PARAM(pstContents);
	return ONDK_RET_OK;
}


static ONDK_Result_t 	nx_notAvailable_SetUiObject(NxNotAvailable_Context_t *pstContents)
{
	ONDK_Rect_t 	stRect;


	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! Please check!! \n");
		return	ONDK_RET_FAIL;
	}

	ONDK_GWM_APP_SetAppArea(NX_PRISM_NOTAVAILABLE_FRAME_X, NX_PRISM_NOTAVAILABLE_FRAME_Y, NX_PRISM_NOTAVAILABLE_FRAME_W, NX_PRISM_NOTAVAILABLE_FRAME_H);

	stRect = ONDK_Rect(NX_PRISM_NOTAVAILABLE_DIMICON_X,NX_PRISM_NOTAVAILABLE_DIMICON_Y,NX_PRISM_NOTAVAILABLE_DIMICON_W,NX_PRISM_NOTAVAILABLE_DIMICON_H);
	ONDK_GWM_COM_Image_Create(NX_PRISM_NOTAVAILABLE_DIM_ID, &stRect);
	ONDK_GWM_COM_Image_SetImage(NX_PRISM_NOTAVAILABLE_DIM_ID, RES_105_00_BG_UP_PNG);

	stRect = ONDK_Rect(NX_PRISM_NOTAVAILABLE_ICON_X,NX_PRISM_NOTAVAILABLE_ICON_Y,NX_PRISM_NOTAVAILABLE_ICON_W,NX_PRISM_NOTAVAILABLE_ICON_H);
	ONDK_GWM_COM_Image_Create(NX_PRISM_NOTAVAILABLE_ID, &stRect);
	ONDK_GWM_COM_Image_SetImage(NX_PRISM_NOTAVAILABLE_ID, RES_210_00_NOTAVAILABLE_PNG);

	ONDK_GWM_Obj_SetVisible((NX_PRISM_NOTAVAILABLE_DIM_ID), TRUE);
	ONDK_GWM_Obj_SetVisible((NX_PRISM_NOTAVAILABLE_ID), TRUE);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_notAvailable_MsgGwmCreate(NxNotAvailable_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! Please check!! \n");
		return	ONDK_RET_FAIL;
	}

	/* 절대 hide가 되지 않는다. */
	ONDK_GWM_APP_SetUIHideFlag(GWM_UI_Hide_Never);
	nx_notAvailable_SetUiObject(pstContents);
	ONDK_GWM_APP_SetAttribute(GWM_HiddenUI_App);

	ONDK_GWM_SetTimer(NX_PRISM_NOTAVAILABLE_EXPIRE_TIMER_ID, NX_PRISM_NOTAVAILABLE_EXPIRE_TIME);

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	return ONDK_RET_OK;
}

static ONDK_Result_t	nx_notAvailable_MsgGwmKeyUp(NxNotAvailable_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! Please check!! \n");
		return	ONDK_RET_FAIL;
	}

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);
	return ONDK_MESSAGE_PASS;
}

static ONDK_Result_t	nx_notAvailable_MsgGwmKeyDown(NxNotAvailable_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! Please check!! \n");
		return	ONDK_RET_FAIL;
	}

	ONDK_GWM_ResetTimer(NX_PRISM_NOTAVAILABLE_EXPIRE_TIMER_ID);

	// if it receive a all kind of key it destroyed self and pass a key.
	ONDK_GWM_APP_Destroy(0);
	return ONDK_MESSAGE_PASS;

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);
}


static ONDK_Result_t 	nx_notAvailable_MsgGwmTimer(NxNotAvailable_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32		ulTimerID = p1;

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! Please check!! \n");
		return	ONDK_RET_FAIL;
	}

	if (ulTimerID == NX_PRISM_NOTAVAILABLE_EXPIRE_TIMER_ID)
	{
		ONDK_GWM_APP_Destroy(0); //mute 상태는 숨지 않는다.
		return	ONDK_MESSAGE_BREAK;
	}

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_notAvailable_MsgGwmDestroy(NxNotAvailable_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! Please check!! \n");
		return	ONDK_RET_FAIL;
	}

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	return	nx_notAvailable_Destroy(pstContents);
}

static ONDK_Result_t	nx_notAvailable_MsgGwmHapiMsg(NxNotAvailable_Context_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! Please check!! \n");
		return	ONDK_RET_FAIL;
	}
	NX_COMMON_NOT_USED_PARAM(handle);
	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	return	ONDK_MESSAGE_PASS;
}


/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/
ONDK_Result_t		NX_PRISM_NotAvailable_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t		eResult	= ONDK_RET_OK;
	NxNotAvailable_Context_t *pstContents = nx_notAvailable_GetContents();

	switch (lMessage)
	{
		case MSG_GWM_CREATE:
			eResult = nx_notAvailable_MsgGwmCreate(pstContents, p1, p2, p3);
			break;
		case MSG_GWM_KEYUP:
//			eResult = nx_notAvailable_MsgGwmKeyUp(pstContents, p1, p2, p3);
			break;
		case MSG_GWM_KEYDOWN:
			eResult = nx_notAvailable_MsgGwmKeyDown(pstContents, p1, p2, p3);
			break;
		case MSG_GWM_TIMER:
			eResult = nx_notAvailable_MsgGwmTimer(pstContents, p1, p2, p3);
			break;
		case MSG_GWM_DESTROY:
			eResult = nx_notAvailable_MsgGwmDestroy(pstContents, p1, p2, p3);
			break;
		case MSG_GWM_HAPI_MESSAGE:
			eResult = nx_notAvailable_MsgGwmHapiMsg(pstContents, handle, p1, p2, p3);
			break;

		default:
			break;
	}

	if(eResult != ONDK_RET_OK)
	{
		return	eResult;
	}

	return	ONDK_GWM_ProcessMessageDefault(lMessage, handle, p1, p2, p3);
}


ONDK_Result_t		NX_PRISM_NotAvailable_Show(void)
{
	ONDK_GWM_App_t	pGwmApp = NULL;

	pGwmApp = ONDK_GWM_APP_Get((ONDK_GWM_Callback_t)NX_PRISM_NotAvailable_Proc);
	if(pGwmApp != NULL)
	{
		ONDK_GWM_APP_Destroy(NX_PRISM_NotAvailable_Proc);
	}

	return	ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, "NX_PRISM_NotAvailable_Proc", APP_NOT_AVAILABLE_MSG_PRIORITY, NX_PRISM_NotAvailable_Proc, HANDLE_NULL, 0, 0, 0);
}


/******************************************************************************/
#define _________End_Of_File____________________________________________________
/******************************************************************************/

