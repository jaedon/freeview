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
 * @file	  		nx_prism_system_device.c
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
#include <nx_main.h>
#include <nx_core.h>
#include <nx_port.h>
#include <nx_common.h>
#include <nx_core_message.h>


/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/
typedef enum
{
	eSTATUS_DEVICE_CLEAR,
	eSTATUS_DEVICE_USB_WIZARD,
	eSTATUS_DEVICE_UNDEFINED
} eDeviceMsgStatus_e;

/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/
typedef struct
{
	eDeviceMsgStatus_e				eLastMsgStatus;
} NxSysDeviceContext_t;


/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/
static NxSysDeviceContext_t		s_stSysDevice;


/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/
static NxSysDeviceContext_t*	nx_systemDevice_GetContext(void);
static ONDK_Result_t	nx_systemDevice_MsgGwmCreate(NxSysDeviceContext_t *pstContext, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	nx_systemDevice_MsgGwmDestroy(NxSysDeviceContext_t *pstContext, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_systemDevice_MsgGwmHapiMsg(NxSysDeviceContext_t *pstContext, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_systemDevice_MsgGwmKeyDown(NxSysDeviceContext_t *pstContext, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_systemDevice_MsgCoreDeviceChange(NxSysDeviceContext_t *pstContext, HINT32 p1, HINT32 p2, HINT32 p3);


/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/
static NxSysDeviceContext_t*	nx_systemDevice_GetContext(void)
{
	return &s_stSysDevice;
}

static ONDK_Result_t	nx_systemDevice_MsgGwmCreate(NxSysDeviceContext_t *pstContext, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (pstContext == NULL)
	{
		NX_APP_Error("Error!!! pstContext is NULL !!\n");
		return ONDK_RET_FAIL;
	}

	pstContext->eLastMsgStatus = eSTATUS_DEVICE_CLEAR;

	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_systemDevice_MsgGwmDestroy(NxSysDeviceContext_t *pstContext, HINT32 p1, HINT32 p2, HINT32 p3)
{
	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_systemDevice_MsgGwmHapiMsg(NxSysDeviceContext_t *pstContext, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_MESSAGE_PASS;
}


static ONDK_Result_t	nx_systemDevice_MsgGwmKeyDown(NxSysDeviceContext_t *pstContext, HINT32 p1, HINT32 p2, HINT32 p3)
{
	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_MESSAGE_PASS;
}

static ONDK_Result_t	nx_systemDevice_MsgCoreDeviceChange(NxSysDeviceContext_t *pstContext, HINT32 p1, HINT32 p2, HINT32 p3)
{
#if 0//defined(CONFIG_MW_MM_MEDIA)
	HERROR		hError = ERR_FAIL;
	HUINT32		ulDevId = (HUINT32)p1;
	NX_PhysicalStorageData_t	*physicalStorageInfo = NULL;

	hError = NX_FILEMGR_STORAGE_GetPhysicalStorageInfoWithDevId(ulDevId, &physicalStorageInfo);
	if (hError != ERR_OK || physicalStorageInfo == NULL)
	{
		NX_APP_Error("Error!!! GetPhysicalStorageInfoWithDevId FAIL !!\n");
		return ONDK_RET_FAIL;
	}

	if (p2 == eNxStorageState_Attach)
	{
		if (physicalStorageInfo->eInterface == eStorageInterface_USB)
		{
			if (pstContext->eLastMsgStatus != eSTATUS_DEVICE_USB_WIZARD)
			{		
				pstContext->eLastMsgStatus = eSTATUS_DEVICE_USB_WIZARD;
				ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, (HCHAR*)"NX_PRISM_MEDIA_TypeSelect_Proc", (APP_DEFAULT_PRIORITY | UISTATE_MEDIA), (ONDK_GWM_Callback_t)NX_PRISM_MEDIA_TypeSelect_Proc,
													 0, (HINT32)ONDK_GetString(RES_MESG_3439_ID), 0, (HINT32)FALSE);
			}
		}
	}
#endif

	return	ONDK_MESSAGE_PASS;
}

/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/
ONDK_Result_t	NX_PRISM_SYSTEM_DEVICE_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t		eResult = ONDK_RET_OK;
	NxSysDeviceContext_t	*pstContext = nx_systemDevice_GetContext();

	switch (lMessage)
	{
		case MSG_GWM_CREATE:
			eResult = nx_systemDevice_MsgGwmCreate(pstContext, p1, p2, p3);
			break;
		case MSG_GWM_KEYUP:
			break;
		case MSG_GWM_KEYDOWN:
			eResult = nx_systemDevice_MsgGwmKeyDown(pstContext, p1, p2, p3);
			break;
		case MSG_GWM_DESTROY:
			eResult = nx_systemDevice_MsgGwmDestroy(pstContext, p1, p2, p3);
			break;
		case MSG_GWM_HAPI_MESSAGE:
			eResult = nx_systemDevice_MsgGwmHapiMsg(pstContext, handle, p1, p2, p3);
			break;
		case MSG_CORE_STORAGE_DEVICE_CHANGE:
			eResult = nx_systemDevice_MsgCoreDeviceChange(pstContext, p1, p2, p3);
			break;
		case MSG_APP_MEDIA_TYPE_SELECT_DLG_DESTROY:
			if (pstContext->eLastMsgStatus == eSTATUS_DEVICE_USB_WIZARD)
			{
				pstContext->eLastMsgStatus = eSTATUS_DEVICE_CLEAR;
			}
			break;
		default:
			break;
	}
	if(eResult != ONDK_RET_OK)
	{
		return eResult;
	}

	return ONDK_GWM_ProcessMessageDefault(lMessage, handle, p1, p2, p3);
}


/******************************************************************************/
#define _________End_Of_File____________________________________________________
/******************************************************************************/

