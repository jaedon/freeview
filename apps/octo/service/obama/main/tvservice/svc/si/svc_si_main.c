/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  svc_si_main.c
	@brief

	Description:  									\n
	Module: SVC / SI_NEW					\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding 을 사용하는 source file 입니다.
* MS949 encodingを使用して source fileです。
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

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <hlib.h>
#include <silib.h>
#include <vkernel.h>
#include <octo_common.h>

#include <pal_sirawpool.h>
#include <pal_pipe.h>
#include <svc_si.h>
#include <_svc_si.h>
#include <_svc_si_lib.h>
#include <_svc_si_filtering.h>
#include <_svc_si_homechannel.h>
#include <psvc_si.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

#define svcSi_GetAudPriorityTable(pstInfo)					((pstInfo)->stAudio.astTable)
#define svcSi_SetAudPriorityTable(pstInfo, astValue)		(pstInfo)->stAudio.astTable = astValue
#define svcSi_GetAudPriorityItemNum(pstInfo)				((pstInfo)->stAudio.ulItemNum)
#define svcSi_SetAudPriorityItemNum(pstInfo, ulValue)		(pstInfo)->stAudio.ulItemNum = ulValue

#define svcSi_GetSubttlPriorityTable(pstInfo)				((pstInfo)->stSubtitle.astTable)
#define svcSi_SetSubttlPriorityTable(pstInfo, astValue)		(pstInfo)->stSubtitle.astTable = astValue
#define svcSi_GetSubttlPriorityItemNum(pstInfo)				((pstInfo)->stSubtitle.ulItemNum)
#define svcSi_SetSubttlPriorityItemNum(pstInfo, ulValue)	(pstInfo)->stSubtitle.ulItemNum = ulValue

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef void (*svcSi_FreeCb_t) (void *ptr);

typedef struct
{
	SvcSi_SiSpec_t			 unSiSpec;
	svcSi_ContextState_e	 eState;

	Handle_t				 hAction;
	void					 (*pfAction)(SvcSi_Msg_t *pstMsg, void *pvData);
	void					*pvData;
	unsigned long			 ulSyncSem;
} svcSi_Context_t;

typedef struct
{
	struct
	{
		SvcSi_EsTypePriority_t		*astTable;
		HUINT32 					 ulItemNum;
	} stAudio;
	struct
	{
		SvcSi_EsTypePriority_t		*astTable;
		HUINT32 					 ulItemNum;
	} stSubtitle;
} svcSi_PriorityInfo_t;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC unsigned long		 		 s_ulSvcSi_TaskId = 0, ulSvcSi_MsgQId = 0;
STATIC svcSi_Context_t		 		 s_aulSvcSi_Context[eActionId_MAX];

STATIC PostMsgToMgrCb_t		 		 s_fnSvcSiPostMsg = NULL;
STATIC PostDataToMgrCb_t	 		 s_fnSvcSiPostData = NULL;
STATIC SvcSi_GetSysConfigCb_t 		 s_fnGetSysConfig = NULL;
STATIC SvcSi_Config_t 				 s_stConfig = { { 0, }, };
STATIC svcSi_PriorityInfo_t			 s_stPriorityInfo;

#define ___LOCAL_FUNCTIONS___
STATIC INLINE unsigned long svc_si_GetSiMsgQueueId (void)
{
	return ulSvcSi_MsgQId;
}

STATIC INLINE svcSi_Context_t *svc_si_GetContext (HUINT32 ulActionId)
{
	return (ulActionId < eActionId_MAX) ? &(s_aulSvcSi_Context[ulActionId]) : NULL;
}

STATIC HERROR svc_si_BuildSysConfig(SvcSi_ConfigClass_e bwClass, SvcSi_Config_t *pstConfig, SvcSi_UserSettings_t *pstUserSettings)
{
	if (NULL == pstConfig)
		return ERR_FAIL;

	if (s_fnGetSysConfig)
	{
		return (* s_fnGetSysConfig)(bwClass, pstConfig, pstUserSettings);
	}
	return ERR_FAIL;
}


#define _____MSG_ARGUMENT_FUNCTIONS_____
STATIC HERROR svc_si_GetStartSessionMsgArgPosAndSize (SvcSi_Msg_t *pstMsg, svcSi_MsgArgType_e eArgType, void **ppvArgPos, HUINT32 *pulArgSize)
{
	switch (eArgType)
	{
	case eSIMSG_ARGTYPE_SyncMode:
		*ppvArgPos = (void *)(&(pstMsg->unArg.stSessionStart.bSyncMode));
		*pulArgSize = sizeof(HBOOL);
		break;

	case eSIMSG_ARGTYPE_ApiResultPtr:
		*ppvArgPos = (void *)(&(pstMsg->unArg.stSessionStart.phResult));
		*pulArgSize = sizeof(HERROR *);
		break;

	case eSIMSG_ARGTYPE_SiSpecs:
		*ppvArgPos = (void *)(&(pstMsg->unArg.stSessionStart.unSpec));
		*pulArgSize = sizeof(SvcSi_SiSpec_t);
		break;

	default:
		return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC HERROR svc_si_GetEndSessionMsgArgPosAndSize (SvcSi_Msg_t *pstMsg, svcSi_MsgArgType_e eArgType, void **ppvArgPos, HUINT32 *pulArgSize)
{
	switch (eArgType)
	{
	case eSIMSG_ARGTYPE_SyncMode:
		*ppvArgPos = (void *)(&(pstMsg->unArg.stSessionEnd.bSyncMode));
		*pulArgSize = sizeof(HBOOL);
		break;

	case eSIMSG_ARGTYPE_ApiResultPtr:
		*ppvArgPos = (void *)(&(pstMsg->unArg.stSessionEnd.phResult));
		*pulArgSize = sizeof(HERROR *);
		break;

	default:
		return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC HERROR svc_si_GetStartLiveMsgArgPosAndSize (SvcSi_Msg_t *pstMsg, svcSi_MsgArgType_e eArgType, void **ppvArgPos, HUINT32 *pulArgSize)
{
	switch (eArgType)
	{
	case eSIMSG_ARGTYPE_SyncMode:
		*ppvArgPos = (void *)(&(pstMsg->unArg.stStartLive.bSyncMode));
		*pulArgSize = sizeof(HBOOL);
		break;

	case eSIMSG_ARGTYPE_ApiResultPtr:
		*ppvArgPos = (void *)(&(pstMsg->unArg.stStartLive.phResult));
		*pulArgSize = sizeof(HERROR *);
		break;

	case eSIMSG_ARGTYPE_SiSpecs:
		*ppvArgPos = (void *)(&(pstMsg->unArg.stStartLive.unSpec));
		*pulArgSize = sizeof(SvcSi_SiSpec_t);
		break;

	case eSIMSG_ARGTYPE_SvcHandle:
	case eSIMSG_ARGTYPE_MasterSvcHandle:
		*ppvArgPos = (void *)(&(pstMsg->unArg.stStartLive.hMasterSvc));
		*pulArgSize = sizeof(Handle_t);
		break;

	case eSIMSG_ARGTYPE_SuppleSvcHandle:
		*ppvArgPos = (void *)(&(pstMsg->unArg.stStartLive.hSuppleSvc));
		*pulArgSize = sizeof(Handle_t);
		break;

	case eSIMSG_ARGTYPE_SuppleType:
		*ppvArgPos = (void *)(&(pstMsg->unArg.stStartLive.ulSupType));
		*pulArgSize = sizeof(HUINT32);
		break;

	case eSIMSG_ARGTYPE_LiveType:
		*ppvArgPos = (void *)(&(pstMsg->unArg.stStartLive.eLiveType));
		*pulArgSize = sizeof(SvcSi_LiveType_e);
		break;

	default:
		return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC HERROR svc_si_GetStopLiveMsgArgPosAndSize (SvcSi_Msg_t *pstMsg, svcSi_MsgArgType_e eArgType, void **ppvArgPos, HUINT32 *pulArgSize)
{
	switch (eArgType)
	{
	case eSIMSG_ARGTYPE_SyncMode:
		*ppvArgPos = (void *)(&(pstMsg->unArg.stStopLive.bSyncMode));
		*pulArgSize = sizeof(HBOOL);
		break;

	case eSIMSG_ARGTYPE_ApiResultPtr:
		*ppvArgPos = (void *)(&(pstMsg->unArg.stStopLive.phResult));
		*pulArgSize = sizeof(HERROR *);
		break;

	case eSIMSG_ARGTYPE_SvcHandle:
		*ppvArgPos = (void *)(&(pstMsg->unArg.stStopLive.hNextSvc));
		*pulArgSize = sizeof(Handle_t);
		break;

	default:
		return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC HERROR svc_si_GetStartSrchMsgArgPosAndSize (SvcSi_Msg_t *pstMsg, svcSi_MsgArgType_e eArgType, void **ppvArgPos, HUINT32 *pulArgSize)
{
	switch (eArgType)
	{
	case eSIMSG_ARGTYPE_SyncMode:
		*ppvArgPos = (void *)(&(pstMsg->unArg.stStartSrch.bSyncMode));
		*pulArgSize = sizeof(HBOOL);
		break;

	case eSIMSG_ARGTYPE_ApiResultPtr:
		*ppvArgPos = (void *)(&(pstMsg->unArg.stStartSrch.phResult));
		*pulArgSize = sizeof(HERROR *);
		break;

	case eSIMSG_ARGTYPE_SiSpecs:
		*ppvArgPos = (void *)(&(pstMsg->unArg.stStartSrch.unSpec));
		*pulArgSize = sizeof(SvcSi_SiSpec_t);
		break;

	case eSIMSG_ARGTYPE_SearchType:
		*ppvArgPos = (void *)(&(pstMsg->unArg.stStartSrch.stOption.ulSearchType));
		*pulArgSize = sizeof(HUINT32);
		break;

	case eSIMSG_ARGTYPE_HumaxSvcType:
		*ppvArgPos = (void *)(&(pstMsg->unArg.stStartSrch.stOption.ulSvcType));
		*pulArgSize = sizeof(HUINT32);
		break;

	case eSIMSG_ARGTYPE_ChannelType:
		*ppvArgPos = (void *)(&(pstMsg->unArg.stStartSrch.stOption.ulCasType));
		*pulArgSize = sizeof(HUINT32);
		break;

	case eSIMSG_ARGTYPE_SessionHandle:
		*ppvArgPos = (void *)(&(pstMsg->unArg.stStartSrch.stOption.hSession));
		*pulArgSize = sizeof(SvcSi_SessionH_t);
		break;

	case eSIMSG_ARGTYPE_NetId:
		*ppvArgPos = (void *)(&(pstMsg->unArg.stStartSrch.stOption.ulNetId));
		*pulArgSize = sizeof(HUINT32);
		break;

	default:
		return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC HERROR svc_si_GetStopSrchMsgArgPosAndSize (SvcSi_Msg_t *pstMsg, svcSi_MsgArgType_e eArgType, void **ppvArgPos, HUINT32 *pulArgSize)
{
	switch (eArgType)
	{
	case eSIMSG_ARGTYPE_SyncMode:
		*ppvArgPos = (void *)(&(pstMsg->unArg.stStopSrch.bSyncMode));
		*pulArgSize = sizeof(HBOOL);
		break;

	case eSIMSG_ARGTYPE_ApiResultPtr:
		*ppvArgPos = (void *)(&(pstMsg->unArg.stStopSrch.phResult));
		*pulArgSize = sizeof(HERROR *);
		break;

	default:
		return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC HERROR svc_si_GetStartExtSrchMsgArgPosAndSize (SvcSi_Msg_t *pstMsg, svcSi_MsgArgType_e eArgType, void **ppvArgPos, HUINT32 *pulArgSize)
{
	switch (eArgType)
	{
	case eSIMSG_ARGTYPE_SyncMode:
		*ppvArgPos = (void *)(&(pstMsg->unArg.stStartExtSrch.bSyncMode));
		*pulArgSize = sizeof(HBOOL);
		break;

	case eSIMSG_ARGTYPE_ApiResultPtr:
		*ppvArgPos = (void *)(&(pstMsg->unArg.stStartExtSrch.phResult));
		*pulArgSize = sizeof(HERROR *);
		break;

	case eSIMSG_ARGTYPE_SiSpecs:
		*ppvArgPos = (void *)(&(pstMsg->unArg.stStartExtSrch.unSpec));
		*pulArgSize = sizeof(SvcSi_SiSpec_t);
		break;

	case eSIMSG_ARGTYPE_SearchType:
		*ppvArgPos = (void *)(&(pstMsg->unArg.stStartExtSrch.stOption.ulExtSearchType));
		*pulArgSize = sizeof(HUINT32);
		break;

	default:
		return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC HERROR svc_si_GetStopExtSrchMsgArgPosAndSize (SvcSi_Msg_t *pstMsg, svcSi_MsgArgType_e eArgType, void **ppvArgPos, HUINT32 *pulArgSize)
{
	switch (eArgType)
	{
	case eSIMSG_ARGTYPE_SyncMode:
		*ppvArgPos = (void *)(&(pstMsg->unArg.stStopExtSrch.bSyncMode));
		*pulArgSize = sizeof(HBOOL);
		break;

	case eSIMSG_ARGTYPE_ApiResultPtr:
		*ppvArgPos = (void *)(&(pstMsg->unArg.stStopExtSrch.phResult));
		*pulArgSize = sizeof(HERROR *);
		break;

	default:
		return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC HERROR svc_si_GetStartSatRecordMsgArgPosAndSize (SvcSi_Msg_t *pstMsg, svcSi_MsgArgType_e eArgType, void **ppvArgPos, HUINT32 *pulArgSize)
{
	switch (eArgType)
	{
	case eSIMSG_ARGTYPE_SyncMode:
		*ppvArgPos = (void *)(&(pstMsg->unArg.stStartSatRecord.bSyncMode));
		*pulArgSize = sizeof(HBOOL);
		break;

	case eSIMSG_ARGTYPE_ApiResultPtr:
		*ppvArgPos = (void *)(&(pstMsg->unArg.stStartSatRecord.phResult));
		*pulArgSize = sizeof(HERROR *);
		break;

	case eSIMSG_ARGTYPE_SiSpecs:
		*ppvArgPos = (void *)(&(pstMsg->unArg.stStartSatRecord.unSpec));
		*pulArgSize = sizeof(SvcSi_SiSpec_t);
		break;

	case eSIMSG_ARGTYPE_SearchType:
		*ppvArgPos = (void *)(&(pstMsg->unArg.stStartSatRecord.stOption.ulSatRecordMode));
		*pulArgSize = sizeof(HUINT32);
		break;

	default:
		return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC HERROR svc_si_GetStopSatRecordMsgArgPosAndSize (SvcSi_Msg_t *pstMsg, svcSi_MsgArgType_e eArgType, void **ppvArgPos, HUINT32 *pulArgSize)
{
	switch (eArgType)
	{
	case eSIMSG_ARGTYPE_SyncMode:
		*ppvArgPos = (void *)(&(pstMsg->unArg.stStopSatRecord.bSyncMode));
		*pulArgSize = sizeof(HBOOL);
		break;

	case eSIMSG_ARGTYPE_ApiResultPtr:
		*ppvArgPos = (void *)(&(pstMsg->unArg.stStopSatRecord.phResult));
		*pulArgSize = sizeof(HERROR *);
		break;

	default:
		return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC HERROR svc_si_GetStartRecMsgArgPosAndSize (SvcSi_Msg_t *pstMsg, svcSi_MsgArgType_e eArgType, void **ppvArgPos, HUINT32 *pulArgSize)
{
	switch (eArgType)
	{
	case eSIMSG_ARGTYPE_SyncMode:
		*ppvArgPos = (void *)(&(pstMsg->unArg.stStartRec.bSyncMode));
		*pulArgSize = sizeof(HBOOL);
		break;

	case eSIMSG_ARGTYPE_ApiResultPtr:
		*ppvArgPos = (void *)(&(pstMsg->unArg.stStartRec.phResult));
		*pulArgSize = sizeof(HERROR *);
		break;

	case eSIMSG_ARGTYPE_SiSpecs:
		*ppvArgPos = (void *)(&(pstMsg->unArg.stStartRec.unSpec));
		*pulArgSize = sizeof(SvcSi_SiSpec_t);
		break;

	case eSIMSG_ARGTYPE_SvcHandle:
	case eSIMSG_ARGTYPE_MasterSvcHandle:
		*ppvArgPos = (void *)(&(pstMsg->unArg.stStartRec.hMasterSvc));
		*pulArgSize = sizeof(Handle_t);
		break;

	case eSIMSG_ARGTYPE_SuppleSvcHandle:
		*ppvArgPos = (void *)(&(pstMsg->unArg.stStartRec.hSuppleSvc));
		*pulArgSize = sizeof(Handle_t);
		break;

	case eSIMSG_ARGTYPE_SuppleType:
		*ppvArgPos = (void *)(&(pstMsg->unArg.stStartRec.ulSupType));
		*pulArgSize = sizeof(HUINT32);
		break;

	default:
		return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC HERROR svc_si_GetStopRecMsgArgPosAndSize (SvcSi_Msg_t *pstMsg, svcSi_MsgArgType_e eArgType, void **ppvArgPos, HUINT32 *pulArgSize)
{
	switch (eArgType)
	{
	case eSIMSG_ARGTYPE_SyncMode:
		*ppvArgPos = (void *)(&(pstMsg->unArg.stStopRec.bSyncMode));
		*pulArgSize = sizeof(HBOOL);
		break;

	case eSIMSG_ARGTYPE_ApiResultPtr:
		*ppvArgPos = (void *)(&(pstMsg->unArg.stStopRec.phResult));
		*pulArgSize = sizeof(HERROR *);
		break;

	case eSIMSG_ARGTYPE_SvcHandle:
		*ppvArgPos = (void *)(&(pstMsg->unArg.stStopRec.hNextSvc));
		*pulArgSize = sizeof(Handle_t);
		break;

	default:
		return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC HERROR svc_si_GetStartSatipMsgArgPosAndSize (SvcSi_Msg_t *pstMsg, svcSi_MsgArgType_e eArgType, void **ppvArgPos, HUINT32 *pulArgSize)
{
	switch (eArgType)
	{
	case eSIMSG_ARGTYPE_SyncMode:
		*ppvArgPos = (void *)(&(pstMsg->unArg.stStartSatip.bSyncMode));
		*pulArgSize = sizeof(HBOOL);
		break;

	case eSIMSG_ARGTYPE_ApiResultPtr:
		*ppvArgPos = (void *)(&(pstMsg->unArg.stStartSatip.phResult));
		*pulArgSize = sizeof(HERROR *);
		break;

	case eSIMSG_ARGTYPE_SiSpecs:
		*ppvArgPos = (void *)(&(pstMsg->unArg.stStartSatip.unSpec));
		*pulArgSize = sizeof(SvcSi_SiSpec_t);
		break;

	case eSIMSG_ARGTYPE_SvcHandle:
	case eSIMSG_ARGTYPE_MasterSvcHandle:
		*ppvArgPos = (void *)(&(pstMsg->unArg.stStartSatip.hMasterSvc));
		*pulArgSize = sizeof(Handle_t);
		break;

	case eSIMSG_ARGTYPE_SuppleSvcHandle:
		*ppvArgPos = (void *)(&(pstMsg->unArg.stStartSatip.hSuppleSvc));
		*pulArgSize = sizeof(Handle_t);
		break;

	case eSIMSG_ARGTYPE_SuppleType:
		*ppvArgPos = (void *)(&(pstMsg->unArg.stStartSatip.ulSupType));
		*pulArgSize = sizeof(HUINT32);
		break;

	default:
		return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC HERROR svc_si_GetStopSatipMsgArgPosAndSize (SvcSi_Msg_t *pstMsg, svcSi_MsgArgType_e eArgType, void **ppvArgPos, HUINT32 *pulArgSize)
{
	switch (eArgType)
	{
	case eSIMSG_ARGTYPE_SyncMode:
		*ppvArgPos = (void *)(&(pstMsg->unArg.stStopSatip.bSyncMode));
		*pulArgSize = sizeof(HBOOL);
		break;

	case eSIMSG_ARGTYPE_ApiResultPtr:
		*ppvArgPos = (void *)(&(pstMsg->unArg.stStopSatip.phResult));
		*pulArgSize = sizeof(HERROR *);
		break;

	case eSIMSG_ARGTYPE_SvcHandle:
		*ppvArgPos = (void *)(&(pstMsg->unArg.stStopSatip.hNextSvc));
		*pulArgSize = sizeof(Handle_t);
		break;

	default:
		return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC HERROR svc_si_GetStartPbMsgArgPosAndSize (SvcSi_Msg_t *pstMsg, svcSi_MsgArgType_e eArgType, void **ppvArgPos, HUINT32 *pulArgSize)
{
	switch (eArgType)
	{
	case eSIMSG_ARGTYPE_SyncMode:
		*ppvArgPos = (void *)(&(pstMsg->unArg.stStartPb.bSyncMode));
		*pulArgSize = sizeof(HBOOL);
		break;

	case eSIMSG_ARGTYPE_ApiResultPtr:
		*ppvArgPos = (void *)(&(pstMsg->unArg.stStartPb.phResult));
		*pulArgSize = sizeof(HERROR *);
		break;

	case eSIMSG_ARGTYPE_SiSpecs:
		*ppvArgPos = (void *)(&(pstMsg->unArg.stStartPb.unSpec));
		*pulArgSize = sizeof(SvcSi_SiSpec_t);
		break;

	default:
		return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC HERROR svc_si_GetStopPbMsgArgPosAndSize (SvcSi_Msg_t *pstMsg, svcSi_MsgArgType_e eArgType, void **ppvArgPos, HUINT32 *pulArgSize)
{
	switch (eArgType)
	{
	case eSIMSG_ARGTYPE_SyncMode:
		*ppvArgPos = (void *)(&(pstMsg->unArg.stStopPb.bSyncMode));
		*pulArgSize = sizeof(HBOOL);
		break;

	case eSIMSG_ARGTYPE_ApiResultPtr:
		*ppvArgPos = (void *)(&(pstMsg->unArg.stStopPb.phResult));
		*pulArgSize = sizeof(HERROR *);
		break;

	default:
		return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC HERROR svc_si_GetStartMediaPbMsgArgPosAndSize (SvcSi_Msg_t *pstMsg, svcSi_MsgArgType_e eArgType, void **ppvArgPos, HUINT32 *pulArgSize)
{
	switch (eArgType)
	{
	case eSIMSG_ARGTYPE_SyncMode:
		*ppvArgPos = (void *)(&(pstMsg->unArg.stStartMediaPb.bSyncMode));
		*pulArgSize = sizeof(HBOOL);
		break;

	case eSIMSG_ARGTYPE_ApiResultPtr:
		*ppvArgPos = (void *)(&(pstMsg->unArg.stStartMediaPb.phResult));
		*pulArgSize = sizeof(HERROR *);
		break;

	case eSIMSG_ARGTYPE_SiSpecs:
		*ppvArgPos = (void *)(&(pstMsg->unArg.stStartMediaPb.unSpec));
		*pulArgSize = sizeof(SvcSi_SiSpec_t);
		break;

	default:
		return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC HERROR svc_si_GetStopMediaPbMsgArgPosAndSize (SvcSi_Msg_t *pstMsg, svcSi_MsgArgType_e eArgType, void **ppvArgPos, HUINT32 *pulArgSize)
{
	switch (eArgType)
	{
	case eSIMSG_ARGTYPE_SyncMode:
		*ppvArgPos = (void *)(&(pstMsg->unArg.stStopMediaPb.bSyncMode));
		*pulArgSize = sizeof(HBOOL);
		break;

	case eSIMSG_ARGTYPE_ApiResultPtr:
		*ppvArgPos = (void *)(&(pstMsg->unArg.stStopMediaPb.phResult));
		*pulArgSize = sizeof(HERROR *);
		break;

	default:
		return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC HERROR svc_si_GetStartOtaCheckMsgArgPosAndSize (SvcSi_Msg_t *pstMsg, svcSi_MsgArgType_e eArgType, void **ppvArgPos, HUINT32 *pulArgSize)
{
	switch (eArgType)
	{
	case eSIMSG_ARGTYPE_SyncMode:
		*ppvArgPos = (void *)(&(pstMsg->unArg.stStartSwupCheck.bSyncMode));
		*pulArgSize = sizeof(HBOOL);
		break;

	case eSIMSG_ARGTYPE_ApiResultPtr:
		*ppvArgPos = (void *)(&(pstMsg->unArg.stStartSwupCheck.phResult));
		*pulArgSize = sizeof(HERROR *);
		break;

	case eSIMSG_ARGTYPE_SiSpecs:
		*ppvArgPos = (void *)(&(pstMsg->unArg.stStartSwupCheck.unSpec));
		*pulArgSize = sizeof(SvcSi_SiSpec_t);
		break;

	case eSIMSG_ARGTYPE_OtaOption:
		*ppvArgPos = (void *)(&(pstMsg->unArg.stStartSwupCheck.stOption));
		*pulArgSize = sizeof(SvcSi_SwupCheckOption_t);
		break;

	default:
		return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC HERROR svc_si_GetStopOtaCheckMsgArgPosAndSize (SvcSi_Msg_t *pstMsg, svcSi_MsgArgType_e eArgType, void **ppvArgPos, HUINT32 *pulArgSize)
{
	switch (eArgType)
	{
	case eSIMSG_ARGTYPE_SyncMode:
		*ppvArgPos = (void *)(&(pstMsg->unArg.stStopSwupCheck.bSyncMode));
		*pulArgSize = sizeof(HBOOL);
		break;

	case eSIMSG_ARGTYPE_ApiResultPtr:
		*ppvArgPos = (void *)(&(pstMsg->unArg.stStopSwupCheck.phResult));
		*pulArgSize = sizeof(HERROR *);
		break;

	default:
		return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC HERROR svc_si_GetStartOtaLiveMsgArgPosAndSize (SvcSi_Msg_t *pstMsg, svcSi_MsgArgType_e eArgType, void **ppvArgPos, HUINT32 *pulArgSize)
{
	switch (eArgType)
	{
	case eSIMSG_ARGTYPE_SyncMode:
		*ppvArgPos = (void *)(&(pstMsg->unArg.stStartLiveSwupCheck.bSyncMode));
		*pulArgSize = sizeof(HBOOL);
		break;

	case eSIMSG_ARGTYPE_ApiResultPtr:
		*ppvArgPos = (void *)(&(pstMsg->unArg.stStartLiveSwupCheck.phResult));
		*pulArgSize = sizeof(HERROR *);
		break;

	case eSIMSG_ARGTYPE_SiSpecs:
		*ppvArgPos = (void *)(&(pstMsg->unArg.stStartLiveSwupCheck.unSpec));
		*pulArgSize = sizeof(SvcSi_SiSpec_t);
		break;

	case eSIMSG_ARGTYPE_OtaOption:
		*ppvArgPos = (void *)(&(pstMsg->unArg.stStartLiveSwupCheck.stOption));
		*pulArgSize = sizeof(SvcSi_LiveSwupOption_t);
		break;

	default:
		return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC HERROR svc_si_GetStopOtaLiveMsgArgPosAndSize (SvcSi_Msg_t *pstMsg, svcSi_MsgArgType_e eArgType, void **ppvArgPos, HUINT32 *pulArgSize)
{
	switch (eArgType)
	{
	case eSIMSG_ARGTYPE_SyncMode:
		*ppvArgPos = (void *)(&(pstMsg->unArg.stStopLiveSwupCheck.bSyncMode));
		*pulArgSize = sizeof(HBOOL);
		break;

	case eSIMSG_ARGTYPE_ApiResultPtr:
		*ppvArgPos = (void *)(&(pstMsg->unArg.stStopLiveSwupCheck.phResult));
		*pulArgSize = sizeof(HERROR *);
		break;

	default:
		return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC HERROR svc_si_GetStartTimeUpdateMsgArgPosAndSize (SvcSi_Msg_t *pstMsg, svcSi_MsgArgType_e eArgType, void **ppvArgPos, HUINT32 *pulArgSize)
{
	switch (eArgType)
	{
	case eSIMSG_ARGTYPE_SyncMode:
		*ppvArgPos = (void *)(&(pstMsg->unArg.stStartTimeUpdate.bSyncMode));
		*pulArgSize = sizeof(HBOOL);
		break;

	case eSIMSG_ARGTYPE_ApiResultPtr:
		*ppvArgPos = (void *)(&(pstMsg->unArg.stStartTimeUpdate.phResult));
		*pulArgSize = sizeof(HERROR *);
		break;

	case eSIMSG_ARGTYPE_SiSpecs:
		*ppvArgPos = (void *)(&(pstMsg->unArg.stStartTimeUpdate.unSpec));
		*pulArgSize = sizeof(SvcSi_SiSpec_t);
		break;

	default:
		return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC HERROR svc_si_GetStopTimeUpdateMsgArgPosAndSize (SvcSi_Msg_t *pstMsg, svcSi_MsgArgType_e eArgType, void **ppvArgPos, HUINT32 *pulArgSize)
{
	switch (eArgType)
	{
	case eSIMSG_ARGTYPE_SyncMode:
		*ppvArgPos = (void *)(&(pstMsg->unArg.stStartTimeUpdate.bSyncMode));
		*pulArgSize = sizeof(HBOOL);
		break;

	case eSIMSG_ARGTYPE_ApiResultPtr:
		*ppvArgPos = (void *)(&(pstMsg->unArg.stStartTimeUpdate.phResult));
		*pulArgSize = sizeof(HERROR *);
		break;

	default:
		return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC HERROR svc_si_GetStartDynamicUpdateMsgArgPosAndSize (SvcSi_Msg_t *pstMsg, svcSi_MsgArgType_e eArgType, void **ppvArgPos, HUINT32 *pulArgSize)
{
	switch (eArgType)
	{
	case eSIMSG_ARGTYPE_SyncMode:
		*ppvArgPos = (void *)(&(pstMsg->unArg.stStartDynamicUpdate.bSyncMode));
		*pulArgSize = sizeof(HBOOL);
		break;

	case eSIMSG_ARGTYPE_ApiResultPtr:
		*ppvArgPos = (void *)(&(pstMsg->unArg.stStartDynamicUpdate.phResult));
		*pulArgSize = sizeof(HERROR *);
		break;

	case eSIMSG_ARGTYPE_SiSpecs:
		*ppvArgPos = (void *)(&(pstMsg->unArg.stStartDynamicUpdate.unSpec));
		*pulArgSize = sizeof(SvcSi_SiSpec_t);
		break;

	default:
		return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC HERROR svc_si_GetStopDynamicUpdateMsgArgPosAndSize (SvcSi_Msg_t *pstMsg, svcSi_MsgArgType_e eArgType, void **ppvArgPos, HUINT32 *pulArgSize)
{
	switch (eArgType)
	{
	case eSIMSG_ARGTYPE_SyncMode:
		*ppvArgPos = (void *)(&(pstMsg->unArg.stStopDynamicUpdate.bSyncMode));
		*pulArgSize = sizeof(HBOOL);
		break;

	case eSIMSG_ARGTYPE_ApiResultPtr:
		*ppvArgPos = (void *)(&(pstMsg->unArg.stStopDynamicUpdate.phResult));
		*pulArgSize = sizeof(HERROR *);
		break;

	default:
		return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC HERROR svc_si_GetStartHbbtvMsgArgPosAndSize (SvcSi_Msg_t *pstMsg, svcSi_MsgArgType_e eArgType, void **ppvArgPos, HUINT32 *pulArgSize)
{
	switch (eArgType)
	{
	case eSIMSG_ARGTYPE_SyncMode:
		*ppvArgPos = (void *)(&(pstMsg->unArg.stStartHbbtv.bSyncMode));
		*pulArgSize = sizeof(HBOOL);
		break;

	case eSIMSG_ARGTYPE_ApiResultPtr:
		*ppvArgPos = (void *)(&(pstMsg->unArg.stStartHbbtv.phResult));
		*pulArgSize = sizeof(HERROR *);
		break;

	case eSIMSG_ARGTYPE_SiSpecs:
		*ppvArgPos = (void *)(&(pstMsg->unArg.stStartHbbtv.unSpec));
		*pulArgSize = sizeof(SvcSi_SiSpec_t);
		break;

	case eSIMSG_ARGTYPE_OnId:
		*ppvArgPos = (void *)(&(pstMsg->unArg.stStartHbbtv.usOnId));
		*pulArgSize = sizeof(HUINT16);
		break;

	case eSIMSG_ARGTYPE_TsId:
		*ppvArgPos = (void *)(&(pstMsg->unArg.stStartHbbtv.usTsId));
		*pulArgSize = sizeof(HUINT16);
		break;

	case eSIMSG_ARGTYPE_SvcId:
		*ppvArgPos = (void *)(&(pstMsg->unArg.stStartHbbtv.usSvcId));
		*pulArgSize = sizeof(HUINT16);
		break;

	default:
		return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC HERROR svc_si_GetStopHbbtvMsgArgPosAndSize (SvcSi_Msg_t *pstMsg, svcSi_MsgArgType_e eArgType, void **ppvArgPos, HUINT32 *pulArgSize)
{
	switch (eArgType)
	{
	case eSIMSG_ARGTYPE_SyncMode:
		*ppvArgPos = (void *)(&(pstMsg->unArg.stStopHbbtv.bSyncMode));
		*pulArgSize = sizeof(HBOOL);
		break;

	case eSIMSG_ARGTYPE_ApiResultPtr:
		*ppvArgPos = (void *)(&(pstMsg->unArg.stStopHbbtv.phResult));
		*pulArgSize = sizeof(HERROR *);
		break;

	default:
		return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC HERROR svc_si_GetMsgArgPosAndSize (SvcSi_Msg_t *pstMsg, svcSi_MsgArgType_e eArgType, void **ppvArgPos, HUINT32 *pulArgSize)
{
	switch (pstMsg->eMsgClass)
	{
	case eSIMSG_CMD_START_SESSION:
		return svc_si_GetStartSessionMsgArgPosAndSize (pstMsg, eArgType, ppvArgPos, pulArgSize);

	case eSIMSG_CMD_END_SESSION:
		return svc_si_GetEndSessionMsgArgPosAndSize (pstMsg, eArgType, ppvArgPos, pulArgSize);

	case eSIMSG_CMD_START_LIVE:
		return svc_si_GetStartLiveMsgArgPosAndSize (pstMsg, eArgType, ppvArgPos, pulArgSize);

	case eSIMSG_CMD_STOP_LIVE:
		return svc_si_GetStopLiveMsgArgPosAndSize (pstMsg, eArgType, ppvArgPos, pulArgSize);

	case eSIMSG_CMD_START_SEARCH:
		return svc_si_GetStartSrchMsgArgPosAndSize (pstMsg, eArgType, ppvArgPos, pulArgSize);

	case eSIMSG_CMD_STOP_SEARCH:
		return svc_si_GetStopSrchMsgArgPosAndSize (pstMsg, eArgType, ppvArgPos, pulArgSize);

	case eSIMSG_CMD_START_EXTSEARCH:
		return svc_si_GetStartExtSrchMsgArgPosAndSize (pstMsg, eArgType, ppvArgPos, pulArgSize);

	case eSIMSG_CMD_STOP_EXTSEARCH:
		return svc_si_GetStopExtSrchMsgArgPosAndSize (pstMsg, eArgType, ppvArgPos, pulArgSize);

	case eSIMSG_CMD_START_SATRECORD:
		return svc_si_GetStartSatRecordMsgArgPosAndSize (pstMsg, eArgType, ppvArgPos, pulArgSize);

	case eSIMSG_CMD_STOP_SATRECORD:
		return svc_si_GetStopSatRecordMsgArgPosAndSize (pstMsg, eArgType, ppvArgPos, pulArgSize);

	case eSIMSG_CMD_START_REC:
		return svc_si_GetStartRecMsgArgPosAndSize (pstMsg, eArgType, ppvArgPos, pulArgSize);

	case eSIMSG_CMD_STOP_REC:
		return svc_si_GetStopRecMsgArgPosAndSize (pstMsg, eArgType, ppvArgPos, pulArgSize);

	case eSIMSG_CMD_START_PB:
		return svc_si_GetStartPbMsgArgPosAndSize (pstMsg, eArgType, ppvArgPos, pulArgSize);

	case eSIMSG_CMD_STOP_PB:
		return svc_si_GetStopPbMsgArgPosAndSize (pstMsg, eArgType, ppvArgPos, pulArgSize);

	case eSIMSG_CMD_START_MEDIAPB:
		return svc_si_GetStartMediaPbMsgArgPosAndSize (pstMsg, eArgType, ppvArgPos, pulArgSize);

	case eSIMSG_CMD_STOP_MEDIAPB:
		return svc_si_GetStopMediaPbMsgArgPosAndSize (pstMsg, eArgType, ppvArgPos, pulArgSize);

	case eSIMSG_CMD_START_OTACHECK:
		return svc_si_GetStartOtaCheckMsgArgPosAndSize (pstMsg, eArgType, ppvArgPos, pulArgSize);

	case eSIMSG_CMD_STOP_OTACHECK:
		return svc_si_GetStopOtaCheckMsgArgPosAndSize (pstMsg, eArgType, ppvArgPos, pulArgSize);

	case eSIMSG_CMD_START_OTALIVE:
		return svc_si_GetStartOtaLiveMsgArgPosAndSize (pstMsg, eArgType, ppvArgPos, pulArgSize);

	case eSIMSG_CMD_STOP_OTALIVE:
		return svc_si_GetStopOtaLiveMsgArgPosAndSize (pstMsg, eArgType, ppvArgPos, pulArgSize);

	case eSIMSG_CMD_START_TIMEUPDATE:
		return svc_si_GetStartTimeUpdateMsgArgPosAndSize (pstMsg, eArgType, ppvArgPos, pulArgSize);

	case eSIMSG_CMD_STOP_TIMEUPDATE:
		return svc_si_GetStopTimeUpdateMsgArgPosAndSize (pstMsg, eArgType, ppvArgPos, pulArgSize);

	case eSIMSG_CMD_START_SATIP:
		return svc_si_GetStartSatipMsgArgPosAndSize (pstMsg, eArgType, ppvArgPos, pulArgSize);

	case eSIMSG_CMD_STOP_SATIP:
		return svc_si_GetStopSatipMsgArgPosAndSize (pstMsg, eArgType, ppvArgPos, pulArgSize);

	case eSIMSG_CMD_START_DYNAMICUPDATE:
		return svc_si_GetStartDynamicUpdateMsgArgPosAndSize (pstMsg, eArgType, ppvArgPos, pulArgSize);

	case eSIMSG_CMD_STOP_DYNAMICUPDATE:
		return svc_si_GetStopDynamicUpdateMsgArgPosAndSize (pstMsg, eArgType, ppvArgPos, pulArgSize);

	case eSIMSG_CMD_START_HBBTV:
		return svc_si_GetStartHbbtvMsgArgPosAndSize(pstMsg, eArgType, ppvArgPos, pulArgSize);

	case eSIMSG_CMD_STOP_HBBTV:
		return svc_si_GetStopHbbtvMsgArgPosAndSize(pstMsg, eArgType, ppvArgPos, pulArgSize);

	default:
		break;
	}

	// SI Section Callback Messages:


	// Nothing:
	return ERR_FAIL;
}

STATIC HERROR svc_si_GetMsgArg (SvcSi_Msg_t *pstMsg, svcSi_MsgArgType_e eArgType, void *pvData)
{
	HUINT32				 ulArgSize = 0;
	void				*pvSrcPos = NULL;
	HERROR				 hErr;

	hErr = svc_si_GetMsgArgPosAndSize (pstMsg, eArgType, &pvSrcPos, &ulArgSize);
	if ((ERR_OK != hErr) || (NULL == pvSrcPos) || (0 == ulArgSize))
	{
		return ERR_FAIL;
	}

	HxSTD_MemCopy (pvData, pvSrcPos, ulArgSize);
	return ERR_OK;
}

#if 0
STATIC HERROR svc_si_SetMsgArg (SvcSi_Msg_t *pstMsg, svcSi_MsgArgType_e eArgType, void *pvData)
{
	HUINT32				 ulArgSize = 0;
	void				*pvSrcPos = NULL;
	HERROR				 hErr;

	hErr = svc_si_GetMsgArgPosAndSize (pstMsg, eArgType, &pvSrcPos, &ulArgSize);
	if ((ERR_OK != hErr) || (NULL == pvSrcPos) || (0 == ulArgSize))
	{
		return ERR_FAIL;
	}

	HxSTD_MemCopy (pvSrcPos, pvData, ulArgSize);
	return ERR_OK;
}
#endif

#define _____MSG_RELATED_FUNCTIONS_____
STATIC HERROR svc_si_MakeStopMsgByNextStartMsg (svcSi_Context_t *pstContext, SvcSi_Msg_t *pstStartMsg, SvcSi_Msg_t *pstStopMsg)
{
	HxSTD_MemSet (pstStopMsg, 0, sizeof(SvcSi_Msg_t));

	pstStopMsg->hAction = pstContext->hAction;

	switch (pstStartMsg->eMsgClass)
	{
	case eSIMSG_CMD_START_LIVE:
		pstStopMsg->eMsgClass = eSIMSG_CMD_STOP_LIVE;
		pstStopMsg->unArg.stStopLive.hNextSvc		= pstStartMsg->unArg.stStartLive.hSuppleSvc;
		pstStopMsg->unArg.stStopLive.bSyncMode		= FALSE;
		break;

	case eSIMSG_CMD_START_SEARCH:
		pstStopMsg->eMsgClass = eSIMSG_CMD_STOP_SEARCH;
		pstStopMsg->unArg.stStopSrch.bSyncMode		= FALSE;
		break;

	case eSIMSG_CMD_START_EXTSEARCH:
		pstStopMsg->eMsgClass = eSIMSG_CMD_STOP_EXTSEARCH;
		pstStopMsg->unArg.stStopSrch.bSyncMode		= FALSE;
		break;

	case eSIMSG_CMD_START_SATRECORD:
		pstStopMsg->eMsgClass = eSIMSG_CMD_STOP_SATRECORD;
		pstStopMsg->unArg.stStopSrch.bSyncMode		= FALSE;
		break;

	case eSIMSG_CMD_START_OTACHECK:
		pstStopMsg->eMsgClass = eSIMSG_CMD_STOP_OTACHECK;
		pstStopMsg->unArg.stStopSwupCheck.bSyncMode	= FALSE;
		break;

	case eSIMSG_CMD_START_OTALIVE:
		pstStopMsg->eMsgClass = eSIMSG_CMD_STOP_OTALIVE;
		pstStopMsg->unArg.stStopLiveSwupCheck.bSyncMode	= FALSE;
		break;

	case eSIMSG_CMD_START_REC:
		pstStopMsg->eMsgClass = eSIMSG_CMD_STOP_REC;
		pstStopMsg->unArg.stStopRec.hNextSvc		= pstStartMsg->unArg.stStartRec.hSuppleSvc;
		pstStopMsg->unArg.stStopRec.bSyncMode		= FALSE;
		break;

	case eSIMSG_CMD_START_PB:
		pstStopMsg->eMsgClass = eSIMSG_CMD_STOP_PB;
		pstStopMsg->unArg.stStopPb.bSyncMode		= FALSE;
		break;

	case eSIMSG_CMD_START_MEDIAPB:
		pstStopMsg->eMsgClass = eSIMSG_CMD_STOP_MEDIAPB;
		pstStopMsg->unArg.stStopPb.bSyncMode		= FALSE;
		break;

	case eSIMSG_CMD_START_DYNAMICUPDATE:
		pstStopMsg->eMsgClass = eSIMSG_CMD_STOP_DYNAMICUPDATE;
		pstStopMsg->unArg.stStopDynamicUpdate.bSyncMode		= FALSE;
		break;

	default:
		HxLOG_Error ("the msg (%d) has no pairing stop message:\n", pstStartMsg->eMsgClass);
		return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC HERROR svc_si_ExecuteContextAction (svcSi_Context_t *pstContext, SvcSi_Msg_t *pstMsg)
{
	if (NULL == pstContext->pfAction)				{ return ERR_FAIL; }

	pstContext->pfAction (pstMsg, pstContext->pvData);
	return ERR_OK;
}

#define _____ACTION_ALLOCATION_____
STATIC HERROR svc_si_ResetContext (svcSi_Context_t *pstContext)
{
	SvcSi_TemplateSpec_t		*pstBaseSpec = &(pstContext->unSiSpec.stTemplate);

	pstContext->pfAction		= NULL;
	if (NULL != pstContext->pvData)
	{
		psvc_si_FreeActionData (pstBaseSpec->eActionKind, pstBaseSpec->ulActionSpec, pstContext->pvData);
		pstContext->pvData = NULL;
	}

	pstBaseSpec->eActionKind	= eSVCSI_ACT_KIND_None;
	pstBaseSpec->ulActionSpec	= eSVCSI_ACT_SPEC_None;

	return ERR_OK;
}

STATIC HERROR svc_si_InitContext (svcSi_Context_t *pstContext, Handle_t hAction, SvcSi_Msg_t *pstMsg)
{
	SvcSi_SiSpec_t			*pstSiSpec = &(pstContext->unSiSpec);
	HERROR					 hErr;

	pstContext->hAction = hAction;

	hErr = svc_si_GetMsgArg (pstMsg, eSIMSG_ARGTYPE_SiSpecs, (void *)pstSiSpec);
	if (ERR_OK != hErr)
	{
		HxLOG_Critical ("MSG has no SI SPEC Args:\n");
		goto ERROR;
	}

	pstContext->pfAction = psvc_si_GetActionFunc (pstSiSpec->stTemplate.eActionKind, pstSiSpec->stTemplate.ulActionSpec);
	if (NULL == pstContext->pfAction)
	{
		HxLOG_Critical ("psvc_si_GetActionFunc return NULL:\n");
		goto ERROR;
	}

	pstContext->pvData = psvc_si_AllocActionData (pstSiSpec->stTemplate.eActionKind, pstSiSpec->stTemplate.ulActionSpec);
	if (NULL == pstContext->pvData)
	{
		HxLOG_Critical ("PSVC_SI_GetActionData return NULL:\n");
		goto ERROR;
	}

	return ERR_OK;

ERROR:
	svc_si_ResetContext (pstContext);
	return ERR_FAIL;
}

#define _____CMD_FUNCTIONS_____
STATIC HERROR svc_si_CmdStop (svcSi_Context_t *pstContext, SvcSi_Msg_t *pstMsg)
{
	Handle_t			 hAction = pstMsg->hAction;
	HERROR				 hResult = ERR_FAIL;

	HxLOG_Info ("MsgClass(%d), Status(%d)\n", pstMsg->eMsgClass, pstContext->eState);

	switch (pstContext->eState)
	{
	case eSvcSi_CtxtState_None:
		pstContext->hAction = hAction;
		hResult = ERR_OK;
		break;

	case eSvcSi_CtxtState_Waiting:
		pstContext->hAction = hAction;
		if (NULL != pstContext->pfAction)
		{
			svc_si_ExecuteContextAction (pstContext, pstMsg);
		}

		hResult = ERR_OK;
		break;

	case eSvcSi_CtxtState_Running:
		pstContext->hAction	= hAction;
		pstContext->eState	= eSvcSi_CtxtState_Waiting;

		if (NULL != pstContext->pfAction)
		{
			svc_si_ExecuteContextAction (pstContext, pstMsg);
		}
		else
		{
			HxLOG_Critical ("!!! State Running but No Action !!!\n");
		}

		hResult = ERR_OK;
		break;

	default:
		hResult = ERR_FAIL;
		break;
	}

	svc_si_PostMsgToAp (eSEVT_SI_STOPPED, hAction, 0, 0, 0);
	return hResult;
}

STATIC HERROR svc_si_CmdStart (svcSi_Context_t *pstContext, SvcSi_Msg_t *pstMsg)
{
	Handle_t				 hAction = pstMsg->hAction;
	SvcSi_ActionKind_e		 eActionKind;
	HUINT32					 ulActionSpec;
	SvcSi_SiSpec_t			 unSiSpec;
	SvcSi_Msg_t 			*pstStopMsg = NULL;
	SvcSi_TemplateSpec_t	*pstCntxSpec = &(pstContext->unSiSpec.stTemplate);
	HERROR					 hErr, hResult = ERR_FAIL;

	HxLOG_Info ("MsgClass(%d), Status(%d)\n", pstMsg->eMsgClass, pstContext->eState);

	switch (pstContext->eState)
	{
	case eSvcSi_CtxtState_None:
		hErr = svc_si_InitContext (pstContext, hAction, pstMsg);
		if (ERR_OK == hErr)
		{
			pstContext->hAction = hAction;
			pstContext->eState = eSvcSi_CtxtState_Waiting;

			hResult = svc_si_CmdStart (pstContext, pstMsg);
		}
		break;

	case eSvcSi_CtxtState_Waiting:
		if (NULL == pstContext->pfAction)
		{
			HxLOG_Critical ("Action Function shall be set in the waiting state!!!\n");
			return ERR_FAIL;
		}

		hErr = svc_si_GetMsgArg (pstMsg, eSIMSG_ARGTYPE_SiSpecs, (void *)&unSiSpec);
		if (ERR_OK == hErr)
		{
			eActionKind = unSiSpec.stTemplate.eActionKind;
			ulActionSpec= unSiSpec.stTemplate.ulActionSpec;
		}
		else
		{
			eActionKind = pstCntxSpec->eActionKind;
			ulActionSpec = pstCntxSpec->ulActionSpec;
		}

		if ((eActionKind != pstCntxSpec->eActionKind) || (ulActionSpec != pstCntxSpec->ulActionSpec))
		{
			// 현재 data 모두 초기화 시키고
			svc_si_ResetContext (pstContext);
			pstCntxSpec->eActionKind = eActionKind;

			// 새로운 action kind로 초기화
			hErr = svc_si_InitContext (pstContext, hAction, pstMsg);
			if (ERR_OK != hErr)
			{
				HxLOG_Critical ("context init. failed !!\n");
				return ERR_FAIL;
			}

			hResult = svc_si_CmdStart (pstContext, pstMsg);
		}
		else
		{
			pstContext->hAction	= hAction;
			pstContext->eState	= eSvcSi_CtxtState_Running;
			svc_si_ExecuteContextAction (pstContext, pstMsg);
			hResult = ERR_OK;
		}

		break;

	case eSvcSi_CtxtState_Running:
		pstStopMsg = (SvcSi_Msg_t *)OxSI_Calloc (sizeof(SvcSi_Msg_t));
		if (NULL != pstStopMsg)
		{
			hErr = svc_si_MakeStopMsgByNextStartMsg (pstContext, pstMsg, pstStopMsg);
			if (ERR_OK == hErr)
			{
				svc_si_CmdStop (pstContext, pstStopMsg);
			}

			OxSI_Free (pstStopMsg);
		}

		if (eSvcSi_CtxtState_Running != pstContext->eState)
		{
			hResult = svc_si_CmdStart (pstContext, pstMsg);
		}
		break;

	default:
		hResult = ERR_FAIL;
		break;
	}

	return hResult;
}

STATIC void svc_si_Task (void *pvArg)
{
	HBOOL					 bSync;
	HUINT32					 ulActionId;
	HERROR					*phResult;
	SvcSi_Msg_t				 stMsg;
	svcSi_Context_t			*pstContext;
	HERROR					 hErr, hResult;

	while (1)
	{
		hErr = VK_MSG_Receive (svc_si_GetSiMsgQueueId(), &stMsg, sizeof (SvcSi_Msg_t));
		if (VK_OK != hErr)					{ continue; }

		ulActionId = PAL_PIPE_GetActionId (stMsg.hAction);
		pstContext = svc_si_GetContext (ulActionId);
		if (NULL == pstContext)
		{
			bSync = FALSE;
			hErr = svc_si_GetMsgArg (&stMsg, eSIMSG_ARGTYPE_SyncMode, (void *)&bSync);
			if ((ERR_OK == hErr) && (TRUE == bSync))
			{
				// sync인 경우에만 result값을 적어준다
				phResult = NULL;
				hErr = svc_si_GetMsgArg (&stMsg, eSIMSG_ARGTYPE_ApiResultPtr, (void *)&phResult);
				if ((ERR_OK == hErr) && (NULL != phResult))
				{
					*phResult = ERR_FAIL;
				}

				svc_si_ReleaseSyncSem (0);
			}

			continue;
		}

		hResult = ERR_FAIL;

		switch (stMsg.eMsgClass)
		{
		case eSIMSG_CMD_START_SESSION:
		case eSIMSG_CMD_END_SESSION:
			hResult = ERR_OK;
			break;

		case eSIMSG_CMD_START_LIVE:
		case eSIMSG_CMD_START_SEARCH:
		case eSIMSG_CMD_START_EXTSEARCH:
		case eSIMSG_CMD_START_SATRECORD:
		case eSIMSG_CMD_START_OTACHECK:
		case eSIMSG_CMD_START_OTALIVE:
		case eSIMSG_CMD_START_REC:
		case eSIMSG_CMD_START_PB:
		case eSIMSG_CMD_START_MEDIAPB:
		case eSIMSG_CMD_START_DYNAMICUPDATE:
		case eSIMSG_CMD_START_TIMEUPDATE:
		case eSIMSG_CMD_START_SATIP:
		case eSIMSG_CMD_START_HBBTV:
			if (PAL_PIPE_IsActionHandleLatest (stMsg.hAction) == ERR_OK)
			{
				PAL_SIRAWPOOL_Clear(PAL_PIPE_GetActionId(stMsg.hAction));
				hResult = svc_si_CmdStart (pstContext, &stMsg);
			}

			break;

		case eSIMSG_CMD_STOP_LIVE:
		case eSIMSG_CMD_STOP_SEARCH:
		case eSIMSG_CMD_STOP_EXTSEARCH:
		case eSIMSG_CMD_STOP_SATRECORD:
		case eSIMSG_CMD_STOP_OTACHECK:
		case eSIMSG_CMD_STOP_OTALIVE:
		case eSIMSG_CMD_STOP_REC:
		case eSIMSG_CMD_STOP_PB:
		case eSIMSG_CMD_STOP_MEDIAPB:
		case eSIMSG_CMD_STOP_DYNAMICUPDATE:
		case eSIMSG_CMD_STOP_TIMEUPDATE:
		case eSIMSG_CMD_STOP_SATIP:
		case eSIMSG_CMD_STOP_HBBTV:
			// Stop은 절대로 메시지를 막거나 하지 않는다.
			hResult = svc_si_CmdStop (pstContext, &stMsg);
			break;

		default:
			if ((eSvcSi_CtxtState_Running == pstContext->eState) && (stMsg.hAction == pstContext->hAction))
			{
				hResult = svc_si_ExecuteContextAction (pstContext, &stMsg);
			}
			else
			{
				hResult = ERR_FAIL;
			}

			break;
		}

		bSync = FALSE;
		hErr = svc_si_GetMsgArg (&stMsg, eSIMSG_ARGTYPE_SyncMode, (void *)&bSync);
		if ((ERR_OK == hErr) && (TRUE == bSync))
		{
			// sync인 경우에만 result값을 적어준다
			phResult = NULL;
			hErr = svc_si_GetMsgArg (&stMsg, eSIMSG_ARGTYPE_ApiResultPtr, (void *)&phResult);
			if ((ERR_OK == hErr) && (NULL != phResult))
			{
				*phResult = hResult;
			}

			svc_si_ReleaseSyncSem (ulActionId);
		}
	}
}

#define ___MEMBER_FUNCTIONS___
void svc_si_SetCallback(SvcSi_GetSysConfigCb_t fnSetSysConfig)
{
	s_fnGetSysConfig = fnSetSysConfig;
}

HERROR svc_si_InitTask(PostMsgToMgrCb_t fnPostMsg, PostDataToMgrCb_t fnPostData)
{
	HUINT32					 ulCnt;
	HCHAR					 szSemName[16];
	HERROR					 hErr;

	s_fnSvcSiPostMsg = fnPostMsg;
	s_fnSvcSiPostData = fnPostData;

	hErr = VK_MSG_Create (MW_SI_MSGQ_SIZE, sizeof(SvcSi_Msg_t), "SvcSiQ", &ulSvcSi_MsgQId, VK_SUSPENDTYPE_FIFO);
	if (VK_OK != hErr)
	{
		HxLOG_Error ("VK_MSG_Create error:\n");
		return ERR_FAIL;
	}

	hErr = VK_TASK_Create (svc_si_Task, MW_SI_TASK_PRIORITY, MW_SI_TASK_STACK_SIZE * 2, "SvcSiTask", NULL, &s_ulSvcSi_TaskId, 0);
	if (VK_OK != hErr)
	{
		HxLOG_Error ("VK_TASK_Create error:\n");
		return ERR_FAIL;
	}

	for (ulCnt = 0; ulCnt < eActionId_MAX; ulCnt++)
	{
		svcSi_Context_t			*pstContext = svc_si_GetContext (ulCnt);
		SvcSi_TemplateSpec_t	*pstBaseSpec = &(pstContext->unSiSpec.stTemplate);

		HxSTD_MemSet (pstContext, 0, sizeof(svcSi_Context_t));

		pstContext->hAction			= HANDLE_NULL;
		pstBaseSpec->eActionKind	= eSVCSI_ACT_KIND_None;
		pstBaseSpec->ulActionSpec	= eSVCSI_ACT_SPEC_None;

		HxSTD_PrintToStrN (szSemName, 16, "SvcSiSync%02d", ulCnt);

		hErr = VK_SEM_Create (&(pstContext->ulSyncSem), (const char *)szSemName, VK_SUSPENDTYPE_PRIORITY);
		if (ERR_OK == hErr)
		{
			svc_si_GetSyncSem (ulCnt);
		}
		else
		{
			HxLOG_Error ("VK_SEM_Create err: Sem(%d) failed\n", ulCnt);
		}
	}

	// Init the other SI sub modules:
	svc_si_FilterInit();
	svc_si_CbInit();

#if	defined(CONFIG_MW_SI_HOME_CHANNEL)
	svc_si_HomeChannelInitChannelInfo();
#endif

	/* Start SI Task */
	VK_TASK_Start (s_ulSvcSi_TaskId);
	return ERR_OK;
}

HERROR svc_si_GetSiState (HUINT32 ulActionId, svcSi_ContextState_e *peState)
{
	svcSi_Context_t 		*pstContext = svc_si_GetContext (ulActionId);

	if ((NULL == peState) || (NULL == pstContext))
	{
		return ERR_FAIL;
	}

	*peState = pstContext->eState;
	return ERR_OK;
}

HERROR svc_si_GetSiSpec_regardlessOfState (HUINT32 ulActionId, SvcSi_SiSpec_t *punSpec)
{
	svcSi_Context_t 		*pstContext = svc_si_GetContext (ulActionId);

	if ((NULL == punSpec) || (NULL == pstContext))
	{
		return ERR_FAIL;
	}

	HxSTD_MemCopy (punSpec, &(pstContext->unSiSpec), sizeof(SvcSi_SiSpec_t));
	return ERR_OK;
}

HERROR svc_si_GetSiSpec (HUINT32 ulActionId, SvcSi_SiSpec_t *punSpec)
{
	svcSi_Context_t 		*pstContext = svc_si_GetContext (ulActionId);

	if ((NULL == punSpec) || (NULL == pstContext) || (eSvcSi_CtxtState_Running != pstContext->eState))
	{
		return ERR_FAIL;
	}

	HxSTD_MemCopy (punSpec, &(pstContext->unSiSpec), sizeof(SvcSi_SiSpec_t));
	return ERR_OK;
}

void *svc_si_GetSiData (HUINT32 ulActionId)
{
	svcSi_Context_t 		*pstContext = svc_si_GetContext (ulActionId);

	if (NULL != pstContext)
	{
		return pstContext->pvData;
	}

	return NULL;
}

HERROR	svc_si_GetPrivateSpecifyValue(HUINT32 ulActSpec,HUINT32 *pulPrivateSpecifyValue)
{
	return psvc_si_GetSiPrivateSpecifyValue (ulActSpec, (HUINT32*)pulPrivateSpecifyValue);
}

const SvcSi_Config_t *svc_si_GetSysConfig(SvcSi_UserSettings_t *pstUserSettings)
{
	HERROR			 hErr;
//	HBOOL			 bUserDataChanged = FALSE;
	SvcSi_Config_t	 *pstConfig = NULL;
//	STATIC HBOOL					 s_bSiCfgInit = TRUE;
//	STATIC SvcSi_UserSettings_t		*s_pstUser = NULL;

//	bUserDataChanged = (s_pstUser != pstUserSettings) ? TRUE : FALSE;

	pstConfig = &s_stConfig;
//	if (s_bSiCfgInit || bUserDataChanged)
	{
		SvcSi_Config_t	stConfig;

//		s_bSiCfgInit = FALSE;
//		s_pstUser = pstUserSettings;
		// ==========================================================
		// Get Configuration : System, Audio Langauge, Audio Output, ...
		// ==========================================================
		HxSTD_MemSet(&stConfig, 0x00, sizeof(SvcSi_Config_t));
		hErr = svc_si_BuildSysConfig(eSvcSi_CFG_ALL, &stConfig, pstUserSettings);
		if (ERR_OK != hErr)
		{
			HxLOG_Warning("svc_si_BuildSysConfig() Error!!\n");
			pstConfig = NULL;
		}
		else
		{
			HxSTD_MemCopy(&s_stConfig, &stConfig, sizeof(SvcSi_Config_t));
			pstConfig = &s_stConfig;
		}
	}

	return (const SvcSi_Config_t *)pstConfig;
}

HERROR svc_si_SvcInfo2UserSettings(const DbSvc_Info_t *pstSvcInfo, SvcSi_UserSettings_t *pstUserSettings)
{
	if (NULL == pstSvcInfo)
		return ERR_FAIL;

	if (NULL == pstUserSettings)
		return ERR_FAIL;

	pstUserSettings->stAudio.bUserSelected = DbSvc_GetAudUserSelected(pstSvcInfo);
	pstUserSettings->stAudio.eLang = HLIB_LANG_639ToId((const HCHAR *)DbSvc_GetAudioLanguage(pstSvcInfo));
	pstUserSettings->stAudio.bDolby = DbSvc_GetDolbyFlag(pstSvcInfo);
	pstUserSettings->stAudio.bAudioDescription = DbSvc_GetAdFlag(pstSvcInfo);

//	pstUserSettings->stSubtitle.eLang

	return ERR_OK;
}

HERROR svc_si_SetAudioPriorityTable(SvcSi_EsTypePriority_t *astTable, HUINT32 ulNum)
{
	SvcSi_EsTypePriority_t	*astNewTable;

	if (NULL == astTable)
		return ERR_FAIL;

	if (0 == ulNum)
		return ERR_FAIL;

	astNewTable = OxSI_MemDup(astTable, sizeof(SvcSi_EsTypePriority_t)*ulNum);
	if (NULL == astNewTable)
		return ERR_FAIL;

	if (NULL != svcSi_GetAudPriorityTable(&s_stPriorityInfo))
	{
		OxSI_Free(svcSi_GetAudPriorityTable(&s_stPriorityInfo));
		svcSi_SetAudPriorityTable(&s_stPriorityInfo, NULL);
		svcSi_SetAudPriorityItemNum(&s_stPriorityInfo, 0);
	}

	svcSi_SetAudPriorityTable(&s_stPriorityInfo, astNewTable);
	svcSi_SetAudPriorityItemNum(&s_stPriorityInfo, ulNum);

	return ERR_OK;
}

HERROR svc_si_GetAudioPriorityTable(SvcSi_EsTypePriority_t **pastTable, HUINT32 *pulNum)
{
	HUINT32						 ulItemNum;
	SvcSi_EsTypePriority_t	*astTable;

	if (NULL == pastTable)		return ERR_FAIL;
	if (NULL == pulNum)			return ERR_FAIL;

	astTable = svcSi_GetAudPriorityTable(&s_stPriorityInfo);
	ulItemNum = svcSi_GetAudPriorityItemNum(&s_stPriorityInfo);
	if (NULL == astTable || 0 == ulItemNum)
		return ERR_FAIL;

	*pastTable = astTable;
	*pulNum = ulItemNum;

	return ERR_OK;
}

HERROR svc_si_SetSubtitlePriorityTable(SvcSi_EsTypePriority_t *astTable, HUINT32 ulNum)
{
	SvcSi_EsTypePriority_t	*astNewTable;

	if (NULL == astTable)		return ERR_FAIL;
	if (0 == ulNum)				return ERR_FAIL;

	astNewTable = OxSI_MemDup(astTable, sizeof(SvcSi_EsTypePriority_t)*ulNum);
	if (NULL == astNewTable)
		return ERR_FAIL;

	if (NULL != svcSi_GetSubttlPriorityTable(&s_stPriorityInfo))
	{
		OxSI_Free(svcSi_GetSubttlPriorityTable(&s_stPriorityInfo));
		svcSi_SetSubttlPriorityTable(&s_stPriorityInfo, NULL);
		svcSi_SetSubttlPriorityItemNum(&s_stPriorityInfo, 0);
	}

	svcSi_SetSubttlPriorityTable(&s_stPriorityInfo, astNewTable);
	svcSi_SetSubttlPriorityItemNum(&s_stPriorityInfo, ulNum);

	return ERR_OK;
}

HERROR svc_si_GetSubtitlePriorityTable(SvcSi_EsTypePriority_t **pastTable, HUINT32 *pulNum)
{
	HUINT32					 ulItemNum;
	SvcSi_EsTypePriority_t	*astTable;

	if (NULL == pastTable)			return ERR_FAIL;
	if (NULL == pulNum)				return ERR_FAIL;

	astTable = svcSi_GetSubttlPriorityTable(&s_stPriorityInfo);
	ulItemNum = svcSi_GetSubttlPriorityItemNum(&s_stPriorityInfo);
	if (NULL == astTable || 0 == ulItemNum)
		return ERR_FAIL;

	*pastTable = astTable;
	*pulNum = ulItemNum;

	return ERR_OK;
}

HERROR svc_si_SendMsgToSiTask (SvcSi_Msg_t *pstMsg)
{
	int				 nRet;

	if (NULL == pstMsg)						{ return ERR_FAIL; }

	nRet = VK_MSG_Send (svc_si_GetSiMsgQueueId(), pstMsg, sizeof(SvcSi_Msg_t));
	if (VK_OK != nRet)
	{
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR svc_si_SendSectionMessage (svcSi_MsgType_e eMsgType, Handle_t hAction, HUINT32 ulTableStatus, HUINT32 ulFilterId)
{
	SvcSi_Msg_t		stMsg;

	if (hAction == HANDLE_NULL)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}
	HxSTD_memset(&stMsg, 0, sizeof(SvcSi_Msg_t));

	stMsg.eMsgClass		= (HUINT32)eMsgType;
	stMsg.hAction		= hAction;
	stMsg.unArg.stSection.ulTableStatus	= ulTableStatus;
	stMsg.unArg.stSection.ulFilterId	= ulFilterId;

	return svc_si_SendMsgToSiTask (&stMsg);
}

void svc_si_GetSyncSem (HUINT32 ulActionId)
{
	svcSi_Context_t 		*pstContext = svc_si_GetContext (ulActionId);
	HxLOG_Debug("ulActionId(%d)\n", ulActionId);

	if ((NULL != pstContext) && (0 != pstContext->ulSyncSem))
	{
		VK_SEM_Get (pstContext->ulSyncSem);
	}
}

void svc_si_ReleaseSyncSem (HUINT32 ulActionId)
{
	svcSi_Context_t			*pstContext = svc_si_GetContext (ulActionId);

	HxLOG_Debug("ulActionId(%d)\n", ulActionId);

	if ((NULL != pstContext) && (0 != pstContext->ulSyncSem))
	{
		VK_SEM_Release (pstContext->ulSyncSem);
	}
}

void svc_si_PostMsgToAp (HINT32 nMsg, Handle_t hAct, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	if (NULL != s_fnSvcSiPostMsg)
	{
		(void)(*s_fnSvcSiPostMsg)(nMsg, hAct, nParam1, nParam2, nParam3);
	}
}

void svc_si_PostDataToAp (HINT32 nMsg, Handle_t hAct, HINT32 nParam1, HINT32 nParam2, void *pvData, HUINT32 ulSize)
{
	if (NULL != s_fnSvcSiPostData)
	{
		(void)(*s_fnSvcSiPostData)(nMsg, hAct, nParam1, nParam2, pvData, ulSize);
	}
}

HERROR	svc_si_LibGetUndefinedServiceName(HUINT32 ulActionId, HUINT8 **ppszServiceName)
{
	if (NULL == ppszServiceName)
		return ERR_FAIL;

	return psvc_si_GetUndefinedServiceName(ulActionId, ppszServiceName);
}

HERROR	svc_si_LibGetUndefinedProviderName(HUINT32 ulActionId, HUINT8 **ppszProviderName)
{
	if (NULL == ppszProviderName)
		return ERR_FAIL;

	return psvc_si_GetUndefinedProviderName(ulActionId, ppszProviderName);
}

HERROR	svc_si_LibGetUndefinedNetworkName(HUINT32 ulActionId, HUINT8 **ppszNetworkName)
{
	if (NULL == ppszNetworkName)
		return ERR_FAIL;

	return psvc_si_GetUndefinedNetworkName(ulActionId, ppszNetworkName);
}


#define ___DEBUG_FUNCTIONS___

#if defined(CONFIG_DEBUG)
#include <isosvc.h>
#include <_svc_si_live.h>


STATIC HUINT8 *svc_si_GetLiveStateStr(svcSi_LiveState_e eState)
{
	switch(eState)
	{
		ENUM_TO_STR(eSI_LIVESTATE_Init)
		ENUM_TO_STR(eSI_LIVESTATE_Skipped)
		ENUM_TO_STR(eSI_LIVESTATE_Running)
		ENUM_TO_STR(eSI_LIVESTATE_Done)
		default:			break;
	}

	return "INVALID state";
}

STATIC HUINT8 *svc_si_GetSiTxtEncodeTypeStr(SiTxtEncodedType_t eType)
{
	switch(eType)
	{
		ENUM_TO_STR(eSITEXT_ENCODED_NONE)
		ENUM_TO_STR(eSITEXT_ENCODED_300468)
		ENUM_TO_STR(eSITEXT_ENCODED_300468_EUROPE)
		ENUM_TO_STR(eSITEXT_ENCODED_300468_UK)
		ENUM_TO_STR(eSITEXT_ENCODED_300468_TRUEVISION)
		ENUM_TO_STR(eSITEXT_ENCODED_JAPAN_JIS)
		default:			break;
	}

	return "Unknown encode type";
}

STATIC HUINT8 *svc_si_GetSiTableTypeStr(SIxTable_Type_e eType)
{
	switch(eType)
	{
		ENUM_TO_STR(eSIxTABLETYPE_Pat)
		ENUM_TO_STR(eSIxTABLETYPE_Pmt)
		ENUM_TO_STR(eSIxTABLETYPE_NitAct)
		ENUM_TO_STR(eSIxTABLETYPE_NitOth)
		ENUM_TO_STR(eSIxTABLETYPE_Bat)
		ENUM_TO_STR(eSIxTABLETYPE_SdtAct)
		ENUM_TO_STR(eSIxTABLETYPE_SdtOth)
		ENUM_TO_STR(eSIxTABLETYPE_EitActPf)
		ENUM_TO_STR(eSIxTABLETYPE_EitOthPf)
		ENUM_TO_STR(eSIxTABLETYPE_EitActSc)
		ENUM_TO_STR(eSIxTABLETYPE_EitOthSc)
		ENUM_TO_STR(eSIxTABLETYPE_Tdt)
		ENUM_TO_STR(eSIxTABLETYPE_Tot)
		ENUM_TO_STR(eSIxTABLETYPE_Cat)

		default:			break;
	}

	return "Unknown table type";
}

#if 0
STATIC HUINT8 *svc_si_GetSiFilterTypeStr(svcSi_FilteringType_b bType)
{
	switch(bType)
	{
		ENUM_TO_STR(bSI_FILTERING_PAT)
		ENUM_TO_STR(bSI_FILTERING_PMT)
		ENUM_TO_STR(bSI_FILTERING_CAT)
		ENUM_TO_STR(bSI_FILTERING_BAT)
		ENUM_TO_STR(bSI_FILTERING_SDT_ACT)
		ENUM_TO_STR(bSI_FILTERING_SDT_OTH)
		ENUM_TO_STR(bSI_FILTERING_NIT_ACT)
		ENUM_TO_STR(bSI_FILTERING_NIT_OTH)
		ENUM_TO_STR(bSI_FILTERING_EIT_ACT_PF)
		ENUM_TO_STR(bSI_FILTERING_EIT_OTH_PF)
		ENUM_TO_STR(bSI_FILTERING_EIT_ACT_SCH)
		ENUM_TO_STR(bSI_FILTERING_EIT_OTH_SCH)
		ENUM_TO_STR(bSI_FILTERING_TDT)
		ENUM_TO_STR(bSI_FILTERING_TOT)
		ENUM_TO_STR(bSI_FILTERING_DSI)
		ENUM_TO_STR(bSI_FILTERING_DDB)
		ENUM_TO_STR(bSI_FILTERING_AIT)
		ENUM_TO_STR(bSI_FILTERING_PMT_N)
		ENUM_TO_STR(bSI_FILTERING_BIT)
		ENUM_TO_STR(bSI_FILTERING_EIT_OTH_PFEXT)
		ENUM_TO_STR(bSI_FILTERING_EIT_OTH_SCHEXT)
		ENUM_TO_STR(bSI_FILTERING_EIT_CC)
		ENUM_TO_STR(bSI_FILTERING_POSTCODE)
		ENUM_TO_STR(bSI_FILTERING_RPD)
		ENUM_TO_STR(bSI_FILTERING_RCT)

		default:			break;
	}

	return "Unknown table type";
}
#endif

HUINT32 svc_si_PrintFilterState(void)
{
	HUINT32 			i;
//	HUINT32 			j;
	svcSi_Context_t		*pstContext = NULL;
	svcSi_LiveData_t	*pstLive = NULL;

	HLIB_CMD_Printf("\n============================================\n");
	HLIB_CMD_Printf(" Live SI Filter state \n");
	HLIB_CMD_Printf("============================================\n");
	for (i=eActionId_VIEW_FIRST; i <= eActionId_VIEW_LAST ; i++)
	{
		pstContext = svc_si_GetContext (i);
		if (pstContext == NULL)
		{
			HLIB_CMD_Printf("viewId(%d) context null ! \n", i);
			continue;
		}

		HLIB_CMD_Printf("viewId(%d) actKind(%d) actSpec(%d) eState(%d)\n", i, pstContext->unSiSpec.stTemplate.eActionKind, pstContext->unSiSpec.stTemplate.ulActionSpec, pstContext->eState);
		if (pstContext->unSiSpec.stTemplate.eActionKind == eSVCSI_ACT_KIND_Live)
		{
			pstLive = (svcSi_LiveData_t *)pstContext->pvData;
			HLIB_CMD_Printf("Live :: \n");

			HLIB_CMD_Printf("\t hAction(0x%X) hService(0x%X) hSupService(0x%X) ulDemuxId(0x%X) \n", pstLive->hAction, pstLive->hService, pstLive->hSupService, pstLive->ulDemuxId);
			HLIB_CMD_Printf("\t state(%s) eTextEncType(%s)\n", svc_si_GetLiveStateStr(pstLive->eState), svc_si_GetSiTxtEncodeTypeStr(pstLive->eTextEncType));

			HLIB_CMD_Printf("\t eTableFiltering(0x%X) : ", pstLive->eTableFiltering);
			if (pstLive->eTableFiltering & bSI_FILTERING_PAT)				{ HLIB_CMD_Printf("PAT "); }
			if (pstLive->eTableFiltering & bSI_FILTERING_PMT)				{ HLIB_CMD_Printf("PMT "); }
			if (pstLive->eTableFiltering & bSI_FILTERING_CAT)				{ HLIB_CMD_Printf("CAT "); }
			if (pstLive->eTableFiltering & bSI_FILTERING_BAT)				{ HLIB_CMD_Printf("BAT "); }
			if (pstLive->eTableFiltering & bSI_FILTERING_SDT_ACT)			{ HLIB_CMD_Printf("SDTact "); }
			if (pstLive->eTableFiltering & bSI_FILTERING_SDT_OTH)			{ HLIB_CMD_Printf("SDToth "); }
			if (pstLive->eTableFiltering & bSI_FILTERING_NIT_ACT)			{ HLIB_CMD_Printf("NITact "); }
			if (pstLive->eTableFiltering & bSI_FILTERING_NIT_OTH)			{ HLIB_CMD_Printf("NIToth "); }
			if (pstLive->eTableFiltering & bSI_FILTERING_EIT_ACT_PF)		{ HLIB_CMD_Printf("EITactpf "); }
			if (pstLive->eTableFiltering & bSI_FILTERING_EIT_OTH_PF)		{ HLIB_CMD_Printf("EITothpf "); }
			if (pstLive->eTableFiltering & bSI_FILTERING_EIT_ACT_SCH)		{ HLIB_CMD_Printf("EITactsch "); }
			if (pstLive->eTableFiltering & bSI_FILTERING_EIT_OTH_SCH)		{ HLIB_CMD_Printf("EITothsch "); }
			if (pstLive->eTableFiltering & bSI_FILTERING_TDT)				{ HLIB_CMD_Printf("TDT "); }
			if (pstLive->eTableFiltering & bSI_FILTERING_TOT)				{ HLIB_CMD_Printf("TOT "); }
			if (pstLive->eTableFiltering & bSI_FILTERING_DSI)				{ HLIB_CMD_Printf("DSI "); }
			if (pstLive->eTableFiltering & bSI_FILTERING_DDB)				{ HLIB_CMD_Printf("DDB "); }
			if (pstLive->eTableFiltering & bSI_FILTERING_AIT)				{ HLIB_CMD_Printf("AIT "); }
			if (pstLive->eTableFiltering & bSI_FILTERING_PMT_N)				{ HLIB_CMD_Printf("PMTn "); }
			if (pstLive->eTableFiltering & bSI_FILTERING_BIT)				{ HLIB_CMD_Printf("BIT "); }
			if (pstLive->eTableFiltering & bSI_FILTERING_EIT_OTH_PFEXT)		{ HLIB_CMD_Printf("EITothpfext "); }
			if (pstLive->eTableFiltering & bSI_FILTERING_EIT_OTH_SCHEXT)	{ HLIB_CMD_Printf("EITothschext "); }
			if (pstLive->eTableFiltering & bSI_FILTERING_EIT_CC)			{ HLIB_CMD_Printf("EITcc "); }
			if (pstLive->eTableFiltering & bSI_FILTERING_UNT)				{ HLIB_CMD_Printf("UNT "); }
			/* should be removed..
			if (pstLive->eTableFiltering & bSI_FILTERING_POSTCODE)			{ HLIB_CMD_Printf("Postcode "); }
			if (pstLive->eTableFiltering & bSI_FILTERING_RPD)				{ HLIB_CMD_Printf("RPD "); }
			if (pstLive->eTableFiltering & bSI_FILTERING_RCT)				{ HLIB_CMD_Printf("RCT "); }
			*/
			HLIB_CMD_Printf("\n\n");

#if 0
			for (j=0; j<eSIxTABLETYPE_MAX; j++)
			{
				HLIB_CMD_Printf("\t [%02d] aulTimeoutFilterId(0x%04X) crc32(0x%08X) <%s> \n", j, pstLive->aulTimeoutFilterId[j], pstLive->aulCrc32[j], svc_si_GetSiTableTypeStr((SIxTable_Type_e) j));
			}
#endif

			HLIB_CMD_Printf("\n\n");
			HLIB_CMD_Printf("--------------------------------------------\n");
		}
		else
		{
			// TODO: impl.
		}
	}

	HLIB_CMD_Printf("\n\n============================================\n\n\n");

	return 0;
}
#endif

#if defined(CONFIG_DEBUG)
Cutest_Suite_t	*svc_si_UtGetSuites(void)
{
	return psvc_si_UtGetSuites();
}
#endif


HERROR svc_si_GetAlternativeAudioPriority (const svcSi_AudioEs_t *pstAudioEs, HBOOL bLangMatched, HINT32 *pnUserPriority, HxLANG_Id_e *peLangId, DxDualMonoSelect_e *peDualMonoSelect)
{
	if (NULL == pstAudioEs)
		return ERR_FAIL;

	if (NULL == pnUserPriority)
		return ERR_FAIL;

	return psvc_si_GetAlternativeAudioPriority(pstAudioEs, bLangMatched, pnUserPriority, peLangId, peDualMonoSelect);
}

/* end of file */

