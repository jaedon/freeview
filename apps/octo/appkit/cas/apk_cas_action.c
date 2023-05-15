/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  apk_cas_action.c
	@brief

	Description:  									\n
	Module: APPKITv2			\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

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

#include <oapi.h>
#include <apk_cas_action.h>

#include <_apk_int.h>
#include <apk_event.h>



#define ___INTERNAL_STRUCTURES___



#define ___INTERNAL_VARIABLES___

STATIC	HxList_t		*apkCasAction_ObjectHandlerList_t = NULL;




#define ___PRIVATE_FUNCTION___




#define _____NOTIFIER_FUNCTION_____

STATIC void apk_cas_action_onStatusChanged(void)
{
	HxList_t	*list;
	APK_CAS_ACTION_Notifier_t 	fnNotifier;
	HERROR					hErr;
	APKS_CAS_ActionInfo_t			stStatus;

	HxLOG_Info("+[%s]...\n", __FUNCTION__);

	hErr = OAPI_CAS_ACTION_GetStatus(&stStatus);

	if(hErr == ERR_OK)
	{
		HxLOG_Info("stStatus.eCasActionType[%02x], slot[%02x], CAS group[%02x], pData[%s]\n\n",
			stStatus.eCasActionType, stStatus.unSlotId, stStatus.eCasGrpId, stStatus.szData);

		switch(stStatus.eCasActionType)
		{
			case eCasActionType_TuneRequest:
			case eCasActionType_ForcedTune:
			case eCasActionType_ReservedTune:
			case eCasActionType_UpdateStart:
			case eCasActionType_UpdateStop:
				list = apkCasAction_ObjectHandlerList_t;
				while (list)
				{
					fnNotifier = (APK_CAS_ACTION_Notifier_t)HLIB_LIST_Data(list);
					if (fnNotifier)
					{
						APK_EVENT_EMIT_ARGS	stApkEvtArgs = {0,};

						stApkEvtArgs.pfnFreeFunc = NULL;
						stApkEvtArgs.ulArgCount = 4;
						stApkEvtArgs.apArgV[0] = (void *)fnNotifier;
						stApkEvtArgs.apArgV[1] = (void *)stStatus.eCasActionType;
						stApkEvtArgs.apArgV[2] = (void *)stStatus.eCasGrpId;
						stApkEvtArgs.apArgV[3] = (void *)stStatus.unSlotId;

						APK_EVENT_Emit(&stApkEvtArgs);
					}
					list = list->next;
				}
				break;
			default:
				break;
		}
	}

	HxLOG_Info("-[%s]...\n", __FUNCTION__);


}





#define ___API_FUNCTION___

HERROR	APK_CAS_ACTION_Init(void)
{
	OAPI_CAS_ACTION_Init(apk_cas_action_onStatusChanged);

	return ERR_OK;
}

void	APK_CAS_ACTION_RegisterNotifier(APK_CAS_ACTION_Notifier_t fnNotfiler)
{
	HxList_t	*list;

	list = HLIB_LIST_Find(apkCasAction_ObjectHandlerList_t, fnNotfiler);
	if (list)
	{
		HxLOG_Error("fnNotfiler(%X) is already registered!!!\n", fnNotfiler);
		return;
	}

	apkCasAction_ObjectHandlerList_t = HLIB_LIST_Append(apkCasAction_ObjectHandlerList_t, (void*)fnNotfiler);

}

void	APK_CAS_ACTION_UnRegisterNotifier(APK_CAS_ACTION_Notifier_t fnNotfiler)
{
	HxList_t	*list;

	list = HLIB_LIST_Find(apkCasAction_ObjectHandlerList_t, fnNotfiler);
	if (list == NULL)
	{
		HxLOG_Error("fnNotfiler(%X) does not registered yet..\n", fnNotfiler);

		return;
	}

	apkCasAction_ObjectHandlerList_t = HLIB_LIST_Remove(apkCasAction_ObjectHandlerList_t, fnNotfiler);

}

HERROR 	APK_CAS_ACTION_GetActionType(HUINT8 *pucActionType)
{
	HERROR					hErr;
	APKS_CAS_ActionInfo_t			stStatus;

	hErr = OAPI_CAS_ACTION_GetStatus(&stStatus);
	if(hErr == ERR_OK)
	{
		*pucActionType = (HUINT8)stStatus.eCasActionType;
	}
	else
	{
		*pucActionType = eCasActionType_None;
	}

	return hErr;
}

HERROR		APK_CAS_ACTION_GetSlotId(HUINT8 *pucSlotId)
{
	HERROR					hErr;
	APKS_CAS_ActionInfo_t			stStatus;

	hErr = OAPI_CAS_ACTION_GetStatus(&stStatus);
	if(hErr == ERR_OK)
	{
		*pucSlotId = (HUINT8)stStatus.unSlotId;
	}
	else
	{
		*pucSlotId = 0;
	}

	return hErr;
}

HERROR 	APK_CAS_ACTION_GetCasId(HUINT8 *pucCasId)
{
	HERROR					hErr;
	APKS_CAS_ActionInfo_t			stStatus;

	hErr = OAPI_CAS_ACTION_GetStatus(&stStatus);
	if(hErr == ERR_OK)
	{
		*pucCasId = (HUINT8)stStatus.eCasGrpId;
	}
	else
	{
		*pucCasId = 0;
	}

	return hErr;
}

HERROR 	APK_CAS_ACTION_GetData(HUINT8 *pucData)
{
	HERROR					hErr;
	APKS_CAS_ActionInfo_t			stStatus;

	hErr = OAPI_CAS_ACTION_GetStatus(&stStatus);
	if(hErr == ERR_OK)
	{
		memcpy(pucData, stStatus.szData, APKS_CAS_SUB_DATA_LENG);
	}
	else
	{
		memset(pucData, 0x00, APKS_CAS_SUB_DATA_LENG);
	}

	return hErr;
}

// EOF
