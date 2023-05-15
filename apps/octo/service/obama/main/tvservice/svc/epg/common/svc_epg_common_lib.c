/**
	@file     file_name.c File Name을 적습니다.
	@brief    file_name & simple comment.

	Description: File에 대한 설명을 적습니다.		\n
	Module: Layer와 모듈 명을 적습니다.			 	\n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>

#include <db_common.h>
#include <db_param.h>

#include <svc_si.h>
#include <pal_pipe.h>
#include <svc_epg.h>
#include "../local_include/_svc_epg.h"
#include "../local_include/_svc_epg_common.h"
#include "../local_include/_svc_epg_common_lib.h"

#include "psvc_epg.h"
/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define EPG_MSG_PART_COMMAND					0x0000FFFF
#define EPG_MSG_PART_PROCESS					0xFF000000
#define EPG_MSG_BIT_OFFSET_ACTION			16
#define EPG_MSG_BIT_OFFSET_PROCESS			24

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/
STATIC HERROR			svc_epg_SendMessage(HUINT32 ulMessage, Handle_t hAction, HUINT32 param2, HUINT32 param3);
STATIC HERROR			svc_epg_SendMessageEx(HUINT32 ulMessage, Handle_t hAction, HUINT32 param2, HUINT32 param3, HUINT32 param4, HUINT32 param5);
STATIC HERROR			svc_epg_MgrPostMessage(HUINT32 ulMessage, Handle_t hAction, HUINT32 param1, HUINT32 param2, HUINT32 param3);
STATIC HERROR			svc_epg_MakeEpgMessage(HUINT16 usCommand, HUINT16 usProcess, HUINT32 *pulMessage);
STATIC HERROR			svc_epg_GetEpgMessageInfo(HUINT32 ulMessage, HUINT16 *pusCommand, HUINT16 *pusProcess);
STATIC HERROR			svc_epg_GetEpgActionType(Handle_t hAction, HUINT16 *pusActionType);
STATIC HERROR			svc_epg_GetSpec(DxDeliveryType_e eDeliType, Handle_t hSvcHandle,EPG_SpecType_e *peSpecType);



#define ____GLOBAL_FUNC____
HERROR			SVC_EPG_SendMessage(HUINT32 ulMessage, Handle_t hAction, HUINT32 param2, HUINT32 param3)
{
	return svc_epg_SendMessage(ulMessage, hAction, param2, param3);
}

HERROR			SVC_EPG_SendMessageEx(HUINT32 ulMessage, Handle_t hAction, HUINT32 param2, HUINT32 param3, HUINT32 param4, HUINT32 param5)
{
	return svc_epg_SendMessageEx(ulMessage, hAction, param2, param3, param4, param5);
}

HERROR			SVC_EPG_MgrPostMessage(HUINT32 nMessage, Handle_t hAction, HUINT32 param1, HUINT32 param2, HUINT32 param3)
{
	return svc_epg_MgrPostMessage(nMessage, hAction, param1, param2, param3);
}

HERROR			SVC_EPG_MakeEpgMessage(HUINT16 usCommand, HUINT16 usProcess, HUINT32 *pulMessage)
{
	return svc_epg_MakeEpgMessage(usCommand, usProcess, pulMessage);
}

HERROR			SVC_EPG_GetEpgMessageInfo(HUINT32 ulMessage, HUINT16 *pusCommand, HUINT16 *pusProcess)
{
	return svc_epg_GetEpgMessageInfo(ulMessage, pusCommand, pusProcess);
}

HERROR			SVC_EPG_GetEpgActionType(Handle_t hAction, HUINT16 *pusActionType)
{
	return svc_epg_GetEpgActionType(hAction, pusActionType);
}

HERROR			SVC_EPG_GetSpec(DxDeliveryType_e eDeliType, Handle_t hSvcHandle, EPG_SpecType_e *peSpecType)
{
	return svc_epg_GetSpec(eDeliType, hSvcHandle, peSpecType);
}


#define ____LOCAL_FUNC____
STATIC HERROR			svc_epg_SendMessage(HUINT32 ulMessage, Handle_t hAction, HUINT32 param2, HUINT32 param3)
{
	svcEpg_Msg_t			stMsg;
	HERROR			hError = ERR_EPG_FAIL;
	HUINT32			ulEpgMessageQId = 0;
	HINT32			lResult = 0;

	HxSTD_memset(&stMsg, 0x00, sizeof(svcEpg_Msg_t));

	stMsg.ulMessage = (HUINT32)ulMessage;
	stMsg.ulParam1 = (HUINT32)hAction;
	stMsg.ulParam2 = param2;
	stMsg.ulParam3 = param3;

	hError = svc_epg_GetMsgQueueId(&ulEpgMessageQId);
	if(hError != ERR_EPG_OK)
	{
		HxLOG_Print("GetMsgQueueId fail!!\n");
		goto EXIT_EPGCOMM_FUNC;
	}

	lResult = VK_MSG_Send(ulEpgMessageQId, &stMsg, sizeof(svcEpg_Msg_t));
	if(lResult != VK_OK)
	{
		HxLOG_Print("VK_MSG_Send fail!!\n");
		goto EXIT_EPGCOMM_FUNC;
	}

	hError = ERR_EPG_OK;

EXIT_EPGCOMM_FUNC :

	return hError;


}

STATIC HERROR			svc_epg_SendMessageEx(HUINT32 ulMessage, Handle_t hAction, HUINT32 param2, HUINT32 param3, HUINT32 param4, HUINT32 param5)
{
	svcEpg_Msg_t			stMsg;
	HERROR			hError = ERR_EPG_FAIL;
	HUINT32			ulEpgMessageQId = 0;
	HINT32			lResult = 0;


	HxSTD_memset(&stMsg, 0x00, sizeof(svcEpg_Msg_t));

	stMsg.ulMessage = (HUINT32)ulMessage;
	stMsg.ulParam1 = (HUINT32)hAction;
	stMsg.ulParam2 = param2;
	stMsg.ulParam3 = param3;
	stMsg.ulParam4 = param4;
	stMsg.ulParam5 = param5;

	hError = svc_epg_GetMsgQueueId(&ulEpgMessageQId);
	if(hError != ERR_EPG_OK)
	{
		HxLOG_Print("GetMsgQueueId fail!!\n");
		goto EXIT_EPGCOMM_FUNC;
	}

	lResult = VK_MSG_Send(ulEpgMessageQId, &stMsg, sizeof(svcEpg_Msg_t));
	if(lResult != VK_OK)
	{
		HxLOG_Print("VK_MSG_Send fail!!\n");
		goto EXIT_EPGCOMM_FUNC;
	}

	hError = ERR_EPG_OK;

EXIT_EPGCOMM_FUNC :

	return hError;


}

STATIC HERROR			svc_epg_MgrPostMessage(HUINT32 nMessage, Handle_t hAction, HUINT32 param1, HUINT32 param2, HUINT32 param3)
{
	HERROR				hError = ERR_EPG_FAIL;
	PostMsgToMgrCb_t	fnPostMsgCb = NULL;

	fnPostMsgCb = svc_epg_GetPostMsgCb();
	if (fnPostMsgCb)
	{
		hError = (* fnPostMsgCb)(nMessage, hAction, param1, param2, param3);
	}

	return hError;
}


STATIC HERROR				svc_epg_MakeEpgMessage(HUINT16 usCommand, HUINT16 usProcess, HUINT32 *pulMessage)
{
	HERROR				hError = ERR_EPG_FAIL;

	if(pulMessage == NULL)
	{
		HxLOG_Print("param fail!!\n");
		goto EXIT_EPGCOMM_FUNC;
	}

	*pulMessage = (usCommand | ((0x00ff & usProcess) << EPG_MSG_BIT_OFFSET_PROCESS));

	hError = ERR_EPG_OK;

EXIT_EPGCOMM_FUNC :

	return hError;

}





STATIC HERROR				svc_epg_GetEpgMessageInfo(HUINT32 ulMessage, HUINT16 *pusCommand, HUINT16 *pusProcess)
{
	HERROR				hError = ERR_EPG_FAIL;

	if(pusCommand == NULL ||pusProcess == NULL)
	{
		HxLOG_Print("param fail!!\n");
		goto EXIT_EPGCOMM_FUNC;
	}

	*pusCommand = (HUINT16)(ulMessage & EPG_MSG_PART_COMMAND);
	*pusProcess = (HUINT16)((ulMessage & EPG_MSG_PART_PROCESS)>>EPG_MSG_BIT_OFFSET_PROCESS);

	hError = ERR_EPG_OK;

EXIT_EPGCOMM_FUNC :

	return hError;

}


STATIC HERROR				svc_epg_GetEpgActionType(Handle_t hAction, HUINT16 *pusActionType)
{
	HERROR				hError = ERR_EPG_FAIL;
	HUINT32				ulActionId = 0;

	if(pusActionType == NULL)
	{
		HxLOG_Print("param fail!!\n");
		goto EXIT_EPGCOMM_FUNC;
	}

	ulActionId = PAL_PIPE_GetActionId(hAction);
	if ((ulActionId >= eActionId_VIEW_FIRST) && (ulActionId <= eActionId_VIEW_LAST))
	{
		*pusActionType = (HUINT16)(eEPG_ACTTYPE_VIEW_0 + (ulActionId - eActionId_VIEW_FIRST));
	}
	else if ((ulActionId >= eActionId_REC_FIRST) && (ulActionId <= eActionId_REC_LAST))
	{
		*pusActionType = (HUINT32)(eEPG_ACTTYPE_REC_0 + (ulActionId - eActionId_REC_FIRST));
	}
	else if ((ulActionId >= eActionId_BG_FIRST) && (ulActionId <= eActionId_BG_LAST))
	{
		*pusActionType = (HUINT16)(eEPG_ACTTYPE_BG_0 + (ulActionId - eActionId_BG_FIRST));
	}
	else
	{
		*pusActionType = (HUINT16)0xffff;
		goto EXIT_EPGCOMM_FUNC;
	}

	hError = ERR_EPG_OK;

EXIT_EPGCOMM_FUNC :

	return hError;

}


// plugin으로 위치 옮길 것.
STATIC HERROR				svc_epg_GetSpec(DxDeliveryType_e eDeliType, Handle_t hSvcHandle,EPG_SpecType_e *peSpecType)
{
	DxOperator_e		eOperatorType = eDxOPERATOR_NOT_DEFINED;
	HxCountry_e			eCountryId = eCountryID_NONE;
	HERROR				hErr;

	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_COUNTRY_ID, (HUINT32 *)&eCountryId, 0);
	if (ERR_OK != hErr)
	{
		HxLOG_Warning("eCountryId = 0x%x, DB_PARAM_GetItem(eDB_PARAM_ITEM_COUNTRY_ID) Error!!\n", eCountryId);
	}

	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_OPERATOR_TYPE, (HUINT32 *)&eOperatorType, 0);
	if (ERR_OK != hErr)
	{
		HxLOG_Warning("eOperatorType = %d, DB_PARAM_GetItem(eDB_PARAM_ITEM_OPERATOR_TYPE) Error!!\n", eOperatorType);
	}

	switch (eDeliType)
	{
/**************************************************************************************
  *
  * SAT
  *
  **************************************************************************************/
		case eDxDELIVERY_TYPE_SAT:
			switch (eCountryId)
			{
				case eCountryID_DEU:
				case eCountryID_AUS:
				case eCountryID_CHE:
				case eCountryID_ITA:
				case eCountryID_FIN:
				case eCountryID_SWE:
				case eCountryID_NOR:
				case eCountryID_JPN:
				case eCountryID_GBR:
					*peSpecType = eEPG_SPEC_BASE;
					break;

				default:
					*peSpecType = eEPG_SPEC_BASE;
					break;
			}
			break;

/**************************************************************************************
  *
  * CABLE
  *
  **************************************************************************************/
		case eDxDELIVERY_TYPE_CAB:
			switch (eCountryId)
			{
				case eCountryID_AUS:
				case eCountryID_NLD:
				case eCountryID_DEU:
				case eCountryID_FIN:
				case eCountryID_SWE:
				case eCountryID_DNK:
				case eCountryID_JPN:
				case eCountryID_CHE:
					*peSpecType = eEPG_SPEC_BASE;
					break;

				default:
					*peSpecType = eEPG_SPEC_BASE;
					break;
			}
			break;

/**************************************************************************************
  *
  * TER
  *
  **************************************************************************************/
		case eDxDELIVERY_TYPE_TER:
			HxLOG_Print("eCountry:0x%X\n", eCountryId);
			switch (eCountryId)
			{
				case eCountryID_DEU:
				case eCountryID_AUS:
				case eCountryID_CHE:
				case eCountryID_IRL:
				case eCountryID_ITA:
				case eCountryID_FRA:
				case eCountryID_NLD:
				case eCountryID_FIN:
				case eCountryID_SWE:
				case eCountryID_NOR:
				case eCountryID_DNK:
				case eCountryID_JPN:
					*peSpecType = eEPG_SPEC_BASE;
					break;

				case eCountryID_GBR:
					*peSpecType = eEPG_SPEC_UKDTT;
					break;

				default:
					*peSpecType = eEPG_SPEC_BASE;
					break;
			}
			break;

		default:
			HxLOG_Error("eDeliType fail!!\n");
			return ERR_FAIL;
	}

	return ERR_OK;
}

/*********************** End of File ******************************/
