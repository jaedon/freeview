/********************************************************************
 * $Workfile:   cmd_epg.c  $
 * Project    :
 * Author     :
 * Description:
 *
 *                                 Copyright (c) 2008 HUMAX Co., Ltd.
 *                                               All rights reserved.
 ********************************************************************/

/********************************************************************
 Header Files
 ********************************************************************/
#include <stdio.h>
#include <string.h>

#include <htype.h>
#include <vkernel.h>
#include <hlib.h>
#include <octo_common.h>
#include <pal_pipe.h>

#include "util_task.h"
#include "linkedlist.h"
#include <db_svc.h>
//#include <pal_dsmcc.h>
#include <svc_epg.h>

#include "_svc_epg_common.h"
#include "_svc_epg_common_lib.h"
#if defined(CONFIG_MW_EPG_TVTV)
#include "psvc_epg.h"
#endif
#include "_svc_epg_db_raw_lib.h"

//#include "mgr_action.h"
//#include <mgr_live.h>





/********************************************************************
 Macro Definition
 ********************************************************************/

typedef enum
{
	CMD_CASE_EITPF = 1,
	CMD_CASE_EITSCH,

	CMD_CASE_TVST,
	CMD_CASE_DGKT,
	CMD_CASE_TSMT,
	CMD_CASE_LTOT,
	CMD_CASE_EPG,
	CMD_CASE_MAX

}CMD_TVTV_CASE_e;






/********************************************************************
 Global/Static Variable Definition
 ********************************************************************/
#if defined(CONFIG_DEBUG)
extern	int g_EpgCallback_Level;
extern	int g_EpgComm_Level;
extern	int g_ContextEitPf_Level;
extern	int g_ContextEitSch_Level;
extern	int g_EpgContext_Level;
extern	int g_EpgDb_Level;
extern	int g_EitLib_Level;
extern	int g_EpgRawMgr_Level;
#if		defined( CONFIG_MW_EPG_TVTV)
extern 	HUINT32			g_ContextTvtv_Level;
extern	HUINT32			g_TvtvData_Level;
extern	HUINT32			g_tvtvlib_level;
#endif
extern	int g_MwEpgMain_Level;
#endif



/********************************************************************
 External Variable Declaration
 ********************************************************************/
#if defined(CONFIG_DEBUG)
//extern void CMD_AddCommandTableItem(CMD_TABLE_ITEM_t *pItem);

extern HUINT32 MGR_ACTION_GetMainActionId(void);
extern HERROR MGR_LIVE_GetServiceHandle(HUINT32, HUINT32, Handle_t *);

#if		defined( CONFIG_MW_EPG_TVTV)
extern HERROR MGR_EPG_Start (HUINT32, HUINT32, HUINT32);
extern HERROR MGR_EPG_Stop (HUINT32);
#endif
#endif

/*****************************************************************************
*   Static function prototypes
******************************************************************************/
#if defined(CONFIG_DEBUG)
int	cmd_epg_Parse(void *szArgs)
{
	int				result = HxCMD_ERR;
	char				*szArg = NULL;
	unsigned long		arg = 0;
//	unsigned char		bArg = FALSE;
	HERROR			hError = ERR_FAIL;
//	HUINT32			ulMessage = 0;
//	Handle_t			hAction = HANDLE_NULL;
//	Handle_t			hSvcHandle = HANDLE_NULL;

	NOT_USED_PARAM(hError);

	if(szArgs == NULL)
	{
		HLIB_CMD_Printf("[cmd_epg_Parse] Param NULL!!\n ");
	}

	// get 1st argument
	szArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgs);
	if (szArg == NULL)
	{
		HLIB_CMD_Printf("[cmd_epg_Parse] Invalid param 1!!\n ");
		goto EXIT_EPGCMD_FUNC;
	}

	result = sscanf(szArg, "%ld", &arg);
	if (result == 0)
	{
		HLIB_CMD_Printf("[cmd_epg_Parse] Invalid param 2!!\n ");
		goto EXIT_EPGCMD_FUNC;
	}

#if defined(CONFIG_MW_EPG_TVTV)
	// 컴파일 에러로인해 임시 막음 : 추후 xml load 진행해 open해서 사용.
	SVC_EPG_CmdCreateBuffer_Tvtv();
#endif

	switch(arg)
	{
		case CMD_CASE_EITPF :
			break;

		case CMD_CASE_EITSCH :
			break;

#if		defined(CONFIG_MW_EPG_TVTV)
		case CMD_CASE_TVST :
			hError = SVC_EPG_CmdCreateTvstInfo_Tvtv();
			if(hError != HxCMD_OK)
			{
				HLIB_CMD_Printf("[cmd_epg_Parse] CreateTvstInfo fail!!\n ");
				goto EXIT_EPGCMD_FUNC;
			}
			break;
		case CMD_CASE_DGKT :
			hError = SVC_EPG_CmdCreateDgktInfo_Tvtv(FALSE);
			if(hError != HxCMD_OK)
			{
				HLIB_CMD_Printf("[cmd_epg_Parse] CreateDgktInfo fail!!\n ");
				goto EXIT_EPGCMD_FUNC;
			}
			break;

		case CMD_CASE_TSMT :
			hError = SVC_EPG_CmdCreateTvtvSystemInfo_Tvtv(FALSE);
			if(hError != HxCMD_OK)
			{
				HLIB_CMD_Printf("[cmd_epg_Parse] CreateTvtvSystemInfo fail!!\n ");
				goto EXIT_EPGCMD_FUNC;
			}

			hError = SVC_EPG_CmdCreateTsmtInfo_Tvtv(FALSE);
			if(hError != HxCMD_OK)
			{
				HLIB_CMD_Printf("[cmd_epg_Parse] CreateTsmtInfo fail!!\n ");
				goto EXIT_EPGCMD_FUNC;
			}
			break;
		case CMD_CASE_LTOT :
			hError = SVC_EPG_CmdCreateTvtvSystemInfo_Tvtv(FALSE);
			if(hError != HxCMD_OK)
			{
				HLIB_CMD_Printf("[cmd_epg_Parse] CreateTvtvSystemInfo fail!!\n ");
				goto EXIT_EPGCMD_FUNC;
			}

			hError = SVC_EPG_CmdCreateLtotInfo_Tvtv(FALSE);
			if(hError != HxCMD_OK)
			{
				HLIB_CMD_Printf("[cmd_epg_Parse] CreateLtotInfo fail!!\n ");
				goto EXIT_EPGCMD_FUNC;
			}
			break;
		case CMD_CASE_EPG :
			hError = SVC_EPG_CmdCreateTvtvSystemInfo_Tvtv(FALSE);
			if(hError != HxCMD_OK)
			{
				HLIB_CMD_Printf("[cmd_epg_Parse] CreateTvtvSystemInfo fail!!\n ");
				goto EXIT_EPGCMD_FUNC;
			}

			hError = SVC_EPG_CmdCreateEpgInfo_Tvtv(FALSE);
			if(hError != HxCMD_OK)
			{
				HLIB_CMD_Printf("[cmd_epg_Parse] CreateEpgInfo fail!!\n ");
				goto EXIT_EPGCMD_FUNC;
			}
			break;
#endif
		default :
			break;
	}
	result = HxCMD_OK;

EXIT_EPGCMD_FUNC :

#if defined(CONFIG_MW_EPG_TVTV)
	SVC_EPG_CmdInitBuffer_Tvtv();
#endif

	return result;
}



#define	PRINT_TIME																				\
{																								\
	HLIB_CMD_Printf("\n[cmd_epg_PrintEventInfo] System Time (%d)\n", VK_TIMER_GetSystemTick()%100000);	\
}



int	cmd_epg_PrintEventInfo(void *szArgs)
{
#if 0
	int						result = HxCMD_ERR;
	int						len = 0;
//	int 						index = 0;
	int 						svcindex = 0;
	char						*szArg1 = NULL;
	char						*szArg2 = NULL;
//	unsigned char				bArg = FALSE;
	HERROR					hError = ERR_FAIL;
//	HUINT32					ulMessage = 0;
	Handle_t					hAction = HANDLE_NULL;
	Handle_t					hSvcHandle = HANDLE_NULL;
	DbSvc_Attr_t 				stAttr;
	DbSvc_Info_t				stSvcInfo;
	HUINT32 					ulCount = 0;
	Handle_t					*phSvcList = NULL;
	SvcEpg_FindParam_t 		stParam;
	POINTER_LIST_T			*pRoot = NULL;
	POINTER_LIST_T			*pList = NULL;

	POINTER_LIST_T			*pSvcRoot = NULL;
	POINTER_LIST_T			*pTempSvc = NULL;
	SvcEpg_FindSvc_t			*pFindSvcInfo = NULL;
//	POINTER_LIST_T			*pEvtRoot = NULL;
	POINTER_LIST_T			*pTempEvtList = NULL;
	SvcEpg_Event_t				*pTempEvent = NULL;
//	SvcEpg_ExtEvent_t			*pExtEvent = NULL;
	SvcEpg_ExtEvent_t			*pExtTempEvent = NULL;
	HxDATETIME_t			stDateTime;
	HxDATETIME_t 			stEndDateTime;
	HINT32					lSvcNum = 0;
	HUINT8					*pucName = NULL;
	HUINT32					ulEventId = 0;
	HBOOL					bDetail = FALSE;

	if(szArgs == NULL)
	{
		HLIB_CMD_Printf("[cmd_epg_PrintEventInfo] Param NULL!!\n ");
	}

	// get 1st argument
	szArg1 = HLIB_CMD_GetNextArg((HCHAR**)&szArgs);
	if (szArg1 == NULL)
	{
		HLIB_CMD_Printf("[cmd_epg_PrintEventInfo] Invalid param 1!!\n ");
		goto EXIT_EPGCMD_FUNC;
	}
	HxSTD_memset(&stParam, 0x00, sizeof(SvcEpg_FindParam_t));
	DB_SVC_InitSvcAttribute (&stAttr);

	len = strlen(szArg1);
	if(!strcmp(szArg1,"key"))
	{
		szArg2 = HLIB_CMD_GetNextArg((HCHAR**)&szArgs);
		if (szArg2 == NULL)
		{
			HLIB_CMD_Printf("[cmd_epg_PrintEventInfo] please command with service number\n ");
			goto EXIT_EPGCMD_FUNC;
		}

		stParam.ulFindMask |= EPG_FIND_MASK_KEYWORD;
		stParam.aucKeyword[0] = 0x05;
		strncpy(&(stParam.aucKeyword[1]), szArg2, (EPG_FIND_KEYWORD_LEN-1));

		hError = MGR_LIVE_GetServiceHandle(MGR_ACTION_GetMainActionId(), 1/*eLiveViewPlaySvc_MASTER*/, &hSvcHandle);
		if(hError != ERR_OK)
		{
			HxLOG_Print("[cmd_epg_PrintEventInfo] AP_WTV_GetSvcHandle fail!!\n ");
			goto EXIT_EPGCMD_FUNC;
		}
		stParam.hSvcHandle = hSvcHandle;

		PRINT_TIME;
		SVC_EPG_GetWholeFindList(&stParam, 200, &pSvcRoot);
		PRINT_TIME;

		pTempSvc = pSvcRoot;

		while(pTempSvc)
		{
			pFindSvcInfo = UTIL_LINKEDLIST_GetContents(pTempSvc);
			if(pFindSvcInfo)
			{
				HLIB_CMD_Printf("\n\n\n\n");
				HxSTD_memset(&stSvcInfo, 0x00, sizeof(DbSvc_Info_t));
				hError = DB_SVC_GetServiceInfo(pFindSvcInfo->hSvcHandle, &stSvcInfo);
				if(hError != ERR_OK)
				{
					continue;
				}
				HLIB_CMD_Printf("--Service Name(%s)--!!\n", stSvcInfo.szSvcName);

				pTempEvtList = pFindSvcInfo->pEventList;
				while(pTempEvtList)
				{
					pTempEvent = (SvcEpg_Event_t*)UTIL_LINKEDLIST_GetContents(pTempEvtList);
					if(pTempEvent)
					{
						HLIB_DATETIME_ConvertUnixTimeToDateTime(pTempEvent->ulStartTime, &stDateTime);

						pucName = NULL;
						SVC_EPG_ConvertEpgTextToString(pTempEvent->astShortText[0].pEventName, &pucName);
#if						1
						HLIB_CMD_Printf("		Event name(%c%c%c%c)(start time %02d.%02d	%02d:%02d)\n ",
																	pucName[0],
																	pucName[1],
																	pucName[2],
																	pucName[3],
																	stDateTime.stDate.ucMonth,
																	stDateTime.stDate.ucDay,
																	stDateTime.stTime.ucHour,
																	stDateTime.stTime.ucMinute);
#endif

					}
					pTempEvtList = UTIL_LINKEDLIST_GetNextPointer(pTempEvtList);
				}
			}
			pTempSvc = UTIL_LINKEDLIST_GetNextPointer(pTempSvc);
		}

		SVC_EPG_ReleaseWholeFindList(pSvcRoot);

	}
	else if(!strcmp(szArg1,"res"))
	{
		HLIB_CMD_Printf("[cmd_epg_PrintEventInfo] call SVC_EPG_PrintEpgResource()\n ");
		hError = SVC_EPG_PrintEpgResource();
		if(hError != ERR_OK)
		{
			HLIB_CMD_Printf("[cmd_epg_PrintEventInfo] MW_EPGAPI_PrintEpgResource fail!!\n");
			goto EXIT_EPGCMD_FUNC;
		}
	}
	else if(!strcmp(szArg1,"pf"))
	{

		stParam.ulFindMask |= EPG_FIND_MASK_TIME;
		stParam.ulStartTime = 0;
		stParam.ulEndTime = 0xffffffff;
#if 0
		hAction = PAL_PIPE_GetActionHandle(MGR_ACTION_GetMainActionId());
		hError = MGR_LIVE_GetServiceHandle(MGR_ACTION_GetMainActionId(), 1/*eLiveViewPlaySvc_MASTER*/, &hSvcHandle);
		if(hError != ERR_OK)
		{
			HLIB_CMD_Printf("[cmd_epg_PrintEventInfo] AP_WTV_GetSvcHandle fail!!\n ");
			goto EXIT_EPGCMD_FUNC;
		}
#endif
		hError = DB_SVC_FindServiceList(eSvcGroup_All, &stAttr, 0, HANDLE_NULL, eSvcFindDirection_FowardFromStart, &ulCount, &phSvcList);
		if(hError != ERR_OK)
		{
			HLIB_CMD_Printf("[cmd_epg_PrintEventInfo] DB_SVC_FindServiceList fail!!\n ");
			goto EXIT_EPGCMD_FUNC;
		}

		stParam.ulFindMask |= EPG_FIND_MASK_PF;
		for(svcindex = 0; svcindex < ulCount; svcindex++)
		{
			stParam.hSvcHandle = phSvcList[svcindex];

			hError = SVC_EPG_GetEventList(&stParam, &pList);
			if(hError != ERR_OK)
			{
				continue;
			}
			pRoot = pList;
			hError = DB_SVC_GetServiceInfo(phSvcList[svcindex], &stSvcInfo);
			if(hError != ERR_OK)
			{
				continue;
			}
			HLIB_CMD_Printf("--Service Name(%s)--!!\n", stSvcInfo.szSvcName);
			while(pList)
			{
				pTempEvent = (SvcEpg_Event_t*)UTIL_LINKEDLIST_GetContents(pList);
				if(pTempEvent)
				{
					bDetail = FALSE;
					if(pTempEvent->stEventInfo.stEitEventInfo.usEitDesMask & eEPG_DES_EXTENDED_EVENT)
					{
						bDetail = TRUE;
					}
					HLIB_DATETIME_ConvertUnixTimeToDateTime(pTempEvent->ulStartTime, &stDateTime);
					HLIB_DATETIME_ConvertUnixTimeToDateTime((pTempEvent->ulStartTime + pTempEvent->ulDuration), &stEndDateTime);
					pucName = NULL;
					SVC_EPG_ConvertEpgTextToString(pTempEvent->astShortText[0].pEventName, &pucName);
#if					1
					HLIB_CMD_Printf("		Event id(%d)bDetail(%d)(start time %02d.%02d	%02d:%02d, end time %02d.%02d	%02d:%02d\n ",
																	pTempEvent->stEventInfo.stEitEventInfo.nEventId,
																	bDetail,
																	stDateTime.stDate.ucMonth,
																	stDateTime.stDate.ucDay,
																	stDateTime.stTime.ucHour,
																	stDateTime.stTime.ucMinute,
																	stEndDateTime.stDate.ucMonth,
																	stEndDateTime.stDate.ucDay,
																	stEndDateTime.stTime.ucHour,
																	stEndDateTime.stTime.ucMinute);
#endif
				}
				pList = UTIL_LINKEDLIST_GetNextPointer(pList);
			}
			hError = SVC_EPG_ReleaseEventList(pRoot);
			if(hError != ERR_OK)
			{
				HLIB_CMD_Printf("ReleaseEventList fail!!\n");
			}
			HLIB_CMD_Printf("\n\n\n");
		}
	}
	else if(!strcmp(szArg1,"sch"))
	{
		stParam.ulFindMask |= EPG_FIND_MASK_PF|EPG_FIND_MASK_SCH|EPG_FIND_MASK_TIME;
		stParam.ulStartTime = 0;
		stParam.ulEndTime = 0xffffffff;
#if 0
		hAction = PAL_PIPE_GetActionHandle(MGR_ACTION_GetMainActionId());
		hError = MGR_LIVE_GetServiceHandle(MGR_ACTION_GetMainActionId(), 1/*eLiveViewPlaySvc_MASTER*/, &hSvcHandle);
		if(hError != ERR_OK)
		{
			HLIB_CMD_Printf("[cmd_epg_PrintEventInfo] AP_WTV_GetSvcHandle fail!!\n ");
			goto EXIT_EPGCMD_FUNC;
		}
#endif
		hError = DB_SVC_FindServiceList(eSvcGroup_All, &stAttr, 0, HANDLE_NULL, eSvcFindDirection_FowardFromStart, &ulCount, &phSvcList);
		if(hError != ERR_OK)
		{
			HLIB_CMD_Printf("[cmd_epg_PrintEventInfo] DB_SVC_FindServiceList fail!!\n ");
			goto EXIT_EPGCMD_FUNC;
		}

		for(svcindex = 0; svcindex < ulCount; svcindex++)
		{
			stParam.hSvcHandle = phSvcList[svcindex];

			hError = SVC_EPG_GetEventList(&stParam, &pList);
			if(hError != ERR_OK)
			{
				continue;
			}
			pRoot = pList;
			hError = DB_SVC_GetServiceInfo(phSvcList[svcindex], &stSvcInfo);
			if(hError != ERR_OK)
			{
				continue;
			}
			HLIB_CMD_Printf("--Service Name(%s)--!!\n", stSvcInfo.szSvcName);
			while(pList)
			{
				pTempEvent = (SvcEpg_Event_t*)UTIL_LINKEDLIST_GetContents(pList);
				if(pTempEvent)
				{
					bDetail = FALSE;
					if(pTempEvent->stEventInfo.stEitEventInfo.usEitDesMask & eEPG_DES_EXTENDED_EVENT)
					{
						bDetail = TRUE;
					}
					HLIB_DATETIME_ConvertUnixTimeToDateTime(pTempEvent->ulStartTime, &stDateTime);
					HLIB_DATETIME_ConvertUnixTimeToDateTime((pTempEvent->ulStartTime + pTempEvent->ulDuration), &stEndDateTime);
					pucName = NULL;
					SVC_EPG_ConvertEpgTextToString(pTempEvent->astShortText[0].pEventName, &pucName);
#if					1
					HLIB_CMD_Printf("		Event id(%d)bDetail(%d)(start time %02d.%02d	%02d:%02d, end time %02d.%02d	%02d:%02d\n ",
																	pTempEvent->stEventInfo.stEitEventInfo.nEventId,
																	bDetail,
																	stDateTime.stDate.ucMonth,
																	stDateTime.stDate.ucDay,
																	stDateTime.stTime.ucHour,
																	stDateTime.stTime.ucMinute,
																	stEndDateTime.stDate.ucMonth,
																	stEndDateTime.stDate.ucDay,
																	stEndDateTime.stTime.ucHour,
																	stEndDateTime.stTime.ucMinute);
#endif
				}
				pList = UTIL_LINKEDLIST_GetNextPointer(pList);
			}
			hError = SVC_EPG_ReleaseEventList(pRoot);
			if(hError != ERR_OK)
			{
				HLIB_CMD_Printf("ReleaseEventList fail!!\n");
			}
			HLIB_CMD_Printf("\n\n\n");
		}
	}
	else if(!strcmp(szArg1,"sc"))
	{

		stParam.ulFindMask |= EPG_FIND_MASK_TIME;
		stParam.ulStartTime = 0;
		stParam.ulEndTime = 0xffffffff;
		stParam.ulFindMask |= EPG_FIND_MASK_SCH;

		szArg2 = HLIB_CMD_GetNextArg((HCHAR**)&szArgs);
		if (szArg2 == NULL)
		{
			HLIB_CMD_Printf("[cmd_epg_PrintEventInfo] please command with service number\n ");
			goto EXIT_EPGCMD_FUNC;
		}
		lSvcNum = atoi(szArg2);

		hAction = PAL_PIPE_GetActionHandle(MGR_ACTION_GetMainActionId());
		hError = MGR_LIVE_GetServiceHandle(MGR_ACTION_GetMainActionId(), 1/*eLiveViewPlaySvc_MASTER*/, &hSvcHandle);
		if(hError != ERR_OK)
		{
			HLIB_CMD_Printf("[cmd_epg_PrintEventInfo] AP_WTV_GetSvcHandle fail!!\n ");
			goto EXIT_EPGCMD_FUNC;
		}
		hError = DB_SVC_GetServiceInfo(hSvcHandle, &stSvcInfo);
		if(hError != ERR_OK)
		{
			goto EXIT_EPGCMD_FUNC;
		}

		hError = DB_SVC_GetServiceHandleByNumber(stSvcInfo.eOrgDeliType, eDxSVC_TYPE_All, lSvcNum, &hSvcHandle);
		if(hError != ERR_OK)
		{
			HLIB_CMD_Printf("[cmd_epg_PrintEventInfo] DB_SVC_FindServiceList fail!!\n ");
			goto EXIT_EPGCMD_FUNC;
		}
		stParam.hSvcHandle = hSvcHandle;

		hError = SVC_EPG_GetEventList(&stParam, &pList);
		if(hError != ERR_OK)
		{
			goto EXIT_EPGCMD_FUNC;
		}
		pRoot = pList;
		hError = DB_SVC_GetServiceInfo(hSvcHandle, &stSvcInfo);
		if(hError != ERR_OK)
		{
			goto EXIT_EPGCMD_FUNC;
		}
		HLIB_CMD_Printf("--Service Name(%s)--!!\n",stSvcInfo.szSvcName);
		while(pList)
		{
			pTempEvent = (SvcEpg_Event_t*)UTIL_LINKEDLIST_GetContents(pList);
			if(pTempEvent)
			{
				bDetail = FALSE;
				if(pTempEvent->stEventInfo.ucEpgDataType == EPG_DATA_TYPE_EIT)
				{
					ulEventId = pTempEvent->stEventInfo.stEitEventInfo.nEventId;
					if(pTempEvent->stEventInfo.stEitEventInfo.usEitDesMask & eEPG_DES_EXTENDED_EVENT)
					{
						bDetail = TRUE;
					}
				}
#if				defined(CONFIG_MW_EPG_TVTV)
				else if(pTempEvent->stEventInfo.ucEpgDataType == EPG_DATA_TYPE_TVTV)
				{
					ulEventId = pTempEvent->stEventInfo.stTvtvEventInfo.ulProgId;
					if(pTempEvent->stEventInfo.stTvtvEventInfo.usEventInfoMask & eEPG_DES_EXTENDED_EVENT)
					{
						bDetail = TRUE;
					}
				}
#endif
				HLIB_DATETIME_ConvertUnixTimeToDateTime(pTempEvent->ulStartTime, &stDateTime);
				HLIB_CMD_Printf("		Event id(%d)bDetail(%d)(start time %02d.%02d	%02d:%02d)\n ",
													ulEventId,
													bDetail,
													stDateTime.stDate.ucMonth,
													stDateTime.stDate.ucDay,
													stDateTime.stTime.ucHour,
													stDateTime.stTime.ucMinute);

			}
			pList = UTIL_LINKEDLIST_GetNextPointer(pList);
		}
		hError = SVC_EPG_ReleaseEventList(pRoot);
		if(hError != ERR_OK)
		{
			HLIB_CMD_Printf("ReleaseEventList fail!!\n");
		}
		HLIB_CMD_Printf("\n\n\n");
	}
	else if(!strcmp(szArg1,"pi"))
	{
		stParam.ulFindMask |= (EPG_FIND_MASK_TIME | EPG_FIND_MASK_PF);
		stParam.ulStartTime = 0;
		stParam.ulEndTime = 0xffffffff;

		szArg2 = HLIB_CMD_GetNextArg((HCHAR**)&szArgs);
		if (szArg2 == NULL)
		{
			HLIB_CMD_Printf("[cmd_epg_PrintEventInfo] please command with service number\n ");
			goto EXIT_EPGCMD_FUNC;
		}
		lSvcNum = atoi(szArg2);

		hAction = PAL_PIPE_GetActionHandle(MGR_ACTION_GetMainActionId());
		hError = MGR_LIVE_GetServiceHandle(MGR_ACTION_GetMainActionId(), 1/*eLiveViewPlaySvc_MASTER*/, &hSvcHandle);
		if(hError != ERR_OK)
		{
			HLIB_CMD_Printf("[cmd_epg_PrintEventInfo] AP_WTV_GetSvcHandle fail!!\n ");
			goto EXIT_EPGCMD_FUNC;
		}
		hError = DB_SVC_GetServiceInfo(hSvcHandle, &stSvcInfo);
		if(hError != ERR_OK)
		{
			goto EXIT_EPGCMD_FUNC;
		}

		hError = DB_SVC_GetServiceHandleByNumber(stSvcInfo.eOrgDeliType, eDxSVC_TYPE_All, lSvcNum, &hSvcHandle);
		if(hError != ERR_OK)
		{
			HLIB_CMD_Printf("[cmd_epg_PrintEventInfo] DB_SVC_FindServiceList fail!!\n ");
			goto EXIT_EPGCMD_FUNC;
		}
		stParam.hSvcHandle = hSvcHandle;

		hError = SVC_EPG_GetExtEvent(&stParam, &pList);
		if(hError != ERR_OK || pList == NULL)
		{
			HLIB_CMD_Printf("--NO LIST!!--!!\n");
			goto EXIT_EPGCMD_FUNC;
		}
		pRoot = pList;
		hError = DB_SVC_GetServiceInfo(hSvcHandle, &stSvcInfo);
		if(hError != ERR_OK)
		{
			goto EXIT_EPGCMD_FUNC;
		}
		HLIB_CMD_Printf("--Service Name(%s)--!!\n",stSvcInfo.szSvcName);
		while(pList)
		{
			pExtTempEvent = (SvcEpg_ExtEvent_t*)UTIL_LINKEDLIST_GetContents(pList);
			if(pExtTempEvent)
			{
				HLIB_DATETIME_ConvertUnixTimeToDateTime(pExtTempEvent->ulStartTime, &stDateTime);
				HLIB_CMD_Printf("		nEventId(%02x)(start time %02d.%02d	%02d:%02d)\n ",
															pExtTempEvent->stEventId.nEventId,
															stDateTime.stDate.ucMonth,
															stDateTime.stDate.ucDay,
															stDateTime.stTime.ucHour,
															stDateTime.stTime.ucMinute);

			}
			pList = UTIL_LINKEDLIST_GetNextPointer(pList);
		}
		hError = SVC_EPG_ReleaseEventList(pRoot);
		if(hError != ERR_OK)
		{
			HLIB_CMD_Printf("ReleaseEventList fail!!\n");
		}
		HLIB_CMD_Printf("\n\n\n");
	}
#if 0

	else if(!strcmp(szArg1,"newpf"))
	{
		svc_epg_Print(eOxEPG_TYPE_PF,"pf.txt");
	}
	else if(!strcmp(szArg1,"newsc"))
	{
		svc_epg_Print(eOxEPG_TYPE_SC,"sc.txt");
	}
#if defined(CONFIG_OP_FREESAT)
	else if(!strcmp(szArg1,"newpfext"))
	{
		svc_epg_Print(eOxEPG_TYPE_PF_EX,"pfex.txt");
	}
	else if(!strcmp(szArg1,"newscext"))
	{
		svc_epg_Print(eOxEPG_TYPE_SC_EX,"scex.txt");
	}
#endif
#endif

	else if(!strcmp(szArg1,"si"))
	{
		stParam.ulFindMask |= (EPG_FIND_MASK_TIME | EPG_FIND_MASK_SCH);
		stParam.ulStartTime = 0;
		stParam.ulEndTime = 0xffffffff;

		szArg2 = HLIB_CMD_GetNextArg((HCHAR**)&szArgs);
		if (szArg2 == NULL)
		{
			HLIB_CMD_Printf("[cmd_epg_PrintEventInfo] please command with service number\n ");
			goto EXIT_EPGCMD_FUNC;
		}
		lSvcNum = atoi(szArg2);

		hAction = PAL_PIPE_GetActionHandle(MGR_ACTION_GetMainActionId());
		hError = MGR_LIVE_GetServiceHandle(MGR_ACTION_GetMainActionId(), 1/*eLiveViewPlaySvc_MASTER*/, &hSvcHandle);
		if(hError != ERR_OK)
		{
			HLIB_CMD_Printf("[cmd_epg_PrintEventInfo] AP_WTV_GetSvcHandle fail!!\n ");
			goto EXIT_EPGCMD_FUNC;
		}
		hError = DB_SVC_GetServiceInfo(hSvcHandle, &stSvcInfo);
		if(hError != ERR_OK)
		{
			goto EXIT_EPGCMD_FUNC;
		}

		hError = DB_SVC_GetServiceHandleByNumber(stSvcInfo.eOrgDeliType, eDxSVC_TYPE_All, lSvcNum, &hSvcHandle);
		if(hError != ERR_OK)
		{
			HLIB_CMD_Printf("[cmd_epg_PrintEventInfo] DB_SVC_FindServiceList fail!!\n ");
			goto EXIT_EPGCMD_FUNC;
		}
		stParam.hSvcHandle = hSvcHandle;

		hError = SVC_EPG_GetExtEvent(&stParam, &pList);
		if(hError != ERR_OK)
		{
			goto EXIT_EPGCMD_FUNC;
		}
		pRoot = pList;
		hError = DB_SVC_GetServiceInfo(hSvcHandle, &stSvcInfo);
		if(hError != ERR_OK)
		{
			goto EXIT_EPGCMD_FUNC;
		}
		HLIB_CMD_Printf("--Service Name(%s)--!!\n",stSvcInfo.szSvcName);
		while(pList)
		{
			pExtTempEvent = (SvcEpg_ExtEvent_t*)UTIL_LINKEDLIST_GetContents(pList);
			if(pExtTempEvent)
			{
				HLIB_DATETIME_ConvertUnixTimeToDateTime(pExtTempEvent->ulStartTime, &stDateTime);
				HLIB_CMD_Printf("		nEventId(%02x)(start time %02d.%02d	%02d:%02d)\n ",
															pExtTempEvent->stEventId.nEventId,
															stDateTime.stDate.ucMonth,
															stDateTime.stDate.ucDay,
															stDateTime.stTime.ucHour,
															stDateTime.stTime.ucMinute);

			}
			pList = UTIL_LINKEDLIST_GetNextPointer(pList);
		}
		hError = SVC_EPG_ReleaseEventList(pRoot);
		if(hError != ERR_OK)
		{
			HLIB_CMD_Printf("ReleaseEventList fail!!\n");
		}
		HLIB_CMD_Printf("\n\n\n");
	}
	else if(!strcmp(szArg1,"tree"))
	{
		hAction = PAL_PIPE_GetActionHandle(MGR_ACTION_GetMainActionId());
		hError = MGR_LIVE_GetServiceHandle(MGR_ACTION_GetMainActionId(), 1/*eLiveViewPlaySvc_MASTER*/, &hSvcHandle);
		if(hError != ERR_OK)
		{
			HLIB_CMD_Printf("[cmd_epg_PrintEventInfo] AP_WTV_GetSvcHandle fail!!\n ");
			goto EXIT_EPGCMD_FUNC;
		}

		hError = DB_SVC_FindServiceList(eSvcGroup_All, &stAttr, 0, HANDLE_NULL, eSvcFindDirection_FowardFromStart, &ulCount, &phSvcList);
		if(hError != ERR_OK)
		{
			HLIB_CMD_Printf("[cmd_epg_PrintEventInfo] DB_SVC_FindServiceList fail!!\n ");
			goto EXIT_EPGCMD_FUNC;
		}
		for(svcindex = 0; svcindex < ulCount; svcindex++)
		{
			hError = DB_SVC_GetServiceInfo(phSvcList[svcindex], &stSvcInfo);
			if(hError != ERR_OK)
			{
				continue;
			}
			HLIB_CMD_Printf("--Service Name(%s)--!!\n", stSvcInfo.szSvcName);
			HLIB_CMD_Printf("\t\tucAntIdx(%08x)usOnId(%08x)usTsId(%08x)usSvcId(%08x)\n",	stSvcInfo.ucAntIdx,
																				stSvcInfo.usOnId,
																				stSvcInfo.usTsId,
																				stSvcInfo.usSvcId);
		}
		SVC_EPG_PrintEventTree();
	}
	else if(!strcmp(szArg1,"cit"))
	{

	}
	else if(!strcmp(szArg1,"citall"))
	{
	}

	result = HxCMD_OK;

EXIT_EPGCMD_FUNC :

	return result;
#else
	return 0;
#endif
}






int	cmd_epg_CommandEpgService(void *szArgs)
{
	int						result = HxCMD_ERR;
//	int 						index = 0;
//	int 						svcindex = 0;
	char						*szArg1 = NULL;
//	char						*szArg2 = NULL;
//	unsigned char				bArg = FALSE;
	HERROR					hError = ERR_FAIL;
//	HUINT32					ulMessage = 0;
	Handle_t					hAction = HANDLE_NULL;
	Handle_t					hSvcHandle = HANDLE_NULL;
//	DbSvc_Attr_t 				stAttr;
//	DbSvc_Info_t				stSvcInfo;
//	HUINT32 					ulCount = 0;
//	Handle_t					*phSvcList = NULL;
//	HINT32					lSvcNum = 0;
//	HUINT8					*pucName = NULL;
	SvcEpg_EpgKind_e				eEpgKind = eEPG_KIND_DVB;

#if defined(CONFIG_OP_FREESAT)
	eEpgKind = eEPG_KIND_FSAT;
#endif

	if(szArgs == NULL)
	{
		HLIB_CMD_Printf("[cmd_epg_CommandEpgService] Param NULL!!\n ");
	}

	// get 1st argument
	szArg1 = HLIB_CMD_GetNextArg((HCHAR**)&szArgs);
	if (szArg1 == NULL)
	{
		HLIB_CMD_Printf("[cmd_epg_CommandEpgService] Invalid param 1!!\n ");
		goto EXIT_EPGCMD_FUNC;
	}

	hAction = PAL_PIPE_GetActionHandle(MGR_ACTION_GetMainActionId());

	if(!strcmp(szArg1,"startlive"))
	{
		hError = MGR_LIVE_GetServiceHandle(MGR_ACTION_GetMainActionId(), 1/*eLiveViewPlaySvc_MASTER*/, &hSvcHandle);
		if(hError != ERR_OK)
		{
			HLIB_CMD_Printf("[cmd_epg_CommandEpgService] MGR_LIVE_GetServiceHandle fail!!\n ");
			goto EXIT_EPGCMD_FUNC;
		}
		//MW_EPG_StartLive(hAction, hSvcHandle);
		SVC_EPG_Start(eEpgKind, eEPG_EVT_KIND_ALL, hAction, hSvcHandle, 0, 0, 0);
	}
	else if(!strcmp(szArg1,"stoplive"))
	{
		//MW_EPG_StopLive(hAction);
		SVC_EPG_Stop(eEpgKind, eEPG_EVT_KIND_ALL, hAction);
	}
#if	defined(CONFIG_MW_EPG_TVTV)
	else if(!strcmp(szArg1,"starttvtv"))
	{
		MGR_EPG_Start(1/*eEpgKind_TVTV*/, 0/*eEpgStartType_Normal*/, 2/*eEpgCallType_NomalAction*/);
	}
	else if(!strcmp(szArg1,"stoptvtv"))
	{
		MGR_EPG_Stop(1/*eEpgKind_TVTV*/);
	}
	else if(!strcmp(szArg1,"loadxml"))
	{
		hError = MGR_LIVE_GetServiceHandle(MGR_ACTION_GetMainActionId(), 1/*eLiveViewPlaySvc_MASTER*/, &hSvcHandle);
		if(hError != ERR_OK)
		{
			HLIB_CMD_Printf("[cmd_epg_CommandEpgService] MGR_LIVE_GetServiceHandle fail!!\n ");
			goto EXIT_EPGCMD_FUNC;
		}
		//MW_EPG_LoadXMLFile(hAction, hSvcHandle);
		SVC_EPG_LoadData(eEPG_KIND_TVTV, hAction, hSvcHandle);
	}
	else if(!strcmp(szArg1,"rmtvtv"))
	{
#if 1
		// HDD에 저장할지 DB에 저장할지 확인후 적용함.
#else
		hError = HLIB_FILE_Delete((const HUINT8 *)EPG_FS_TVTV_PATH);
#endif
		if (hError != ERR_OK)
		{
			HLIB_CMD_Printf("[cmd_epg_CommandEpgService] HLIB_FILE_Delete fail!!\n ");
			goto EXIT_EPGCMD_FUNC;
		}
	}
#endif
	else if(!strcmp(szArg1,"print_epgsvc"))
	{
		SVC_EPGSERVICE_Print();
	}
	result = HxCMD_OK;

EXIT_EPGCMD_FUNC :

	return result;
}

int	cmd_epg_PrintEpgFindCommand(void *szArgs)
{
	int					result = HxCMD_ERR;
	char				*szArg = NULL;
	HBOOL				bIsNeed2ndArg = FALSE;
	HUINT8				aucStr[20];
	HUINT32				ulKindAugStatus = 0;

	if(szArgs == NULL)
	{
		HLIB_CMD_Printf("[cmd_epg_PrintEpgRawSecCountPrint] Param NULL!!\n ");
	}

	// get 1st argument
	szArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgs);
	if (szArg == NULL)
	{
		HLIB_CMD_Printf("[cmd_epg_PrintEpgRawSecCountPrint] Invalid param 1!!\n ");
		goto EXIT_EPGCMD_FUNC;
	}

	result = sscanf(szArg, "%s", aucStr);
	if (result == 0)
	{
		HLIB_CMD_Printf("[cmd_epg_PrintEpgRawSecCountPrint] Invalid param 2!!\n ");
		goto EXIT_EPGCMD_FUNC;
	}

	HLIB_STD_StrUpper(aucStr);

	if (strcmp(aucStr, "PRINT") == 0)
	{
		ulKindAugStatus = 1;
		bIsNeed2ndArg = TRUE;
	}
	else if(strcmp(aucStr, "KEYWORD") == 0)
	{
		ulKindAugStatus = 2;
	}

	if (bIsNeed2ndArg == TRUE)
	{
		// get 2nd argument
		szArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgs);
		if (szArg != NULL)
		{
			result = sscanf(szArg, "%s", aucStr);
			if (result == 0)
			{
				HLIB_CMD_Printf("[cmd_epg_PrintEpgRawSecCountPrint] Invalid param 3!!\n ");
				goto EXIT_EPGCMD_FUNC;
			}

			HLIB_STD_StrUpper(aucStr);
		}
	}

	switch(ulKindAugStatus)
	{
	case 1:
		break;

	default:
		break;
	}

EXIT_EPGCMD_FUNC :
	return result;
}




int	cmd_epg_PrintEpgRawSecCountPrint(void *szArgs)
{
	int					result = HxCMD_ERR;
	char				*szArg = NULL;
	HBOOL				bIsNeed2ndArg = FALSE;
	HUINT8				aucStr[20];
	HUINT32				ulKindAugStatus = 0;

	if(szArgs == NULL)
	{
		HLIB_CMD_Printf("[cmd_epg_PrintEpgRawSecCountPrint] Param NULL!!\n ");
	}

	// get 1st argument
	szArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgs);
	if (szArg == NULL)
	{
		HLIB_CMD_Printf("[cmd_epg_PrintEpgRawSecCountPrint] Invalid param 1!!\n ");
		goto EXIT_EPGCMD_FUNC;
	}

	result = sscanf(szArg, "%s", aucStr);
	if (result == 0)
	{
		HLIB_CMD_Printf("[cmd_epg_PrintEpgRawSecCountPrint] Invalid param 2!!\n ");
		goto EXIT_EPGCMD_FUNC;
	}

	HLIB_STD_StrUpper(aucStr);

	if (strcmp(aucStr, "PRINT") == 0)
	{
		ulKindAugStatus = 1;
		bIsNeed2ndArg = TRUE;
	}
	else if(strcmp(aucStr, "BYTERATE") == 0)
	{
		ulKindAugStatus = 2;
	}

	if (bIsNeed2ndArg == TRUE)
	{
		// get 2nd argument
		szArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgs);
		if (szArg != NULL)
		{
			result = sscanf(szArg, "%s", aucStr);
			if (result == 0)
			{
				HLIB_CMD_Printf("[cmd_epg_PrintEpgRawSecCountPrint] Invalid param 3!!\n ");
				goto EXIT_EPGCMD_FUNC;
			}

			HLIB_STD_StrUpper(aucStr);
		}
	}

	switch(ulKindAugStatus)
	{
	case 1:
		if(szArg == NULL)
		{
			//result = xsvc_epgdb_PrintInfo_Base(NULL);
		}
		else
		{
			//result = xsvc_epgdb_PrintInfo_Base(aucStr);
		}
		break;

	case 2:
		//SVC_EPGCONTEXT_EnableCheckByteRateNSectionCount();
		result = ERR_OK;
		break;
	default:
		break;
	}

EXIT_EPGCMD_FUNC :
	return result;
}

static HERROR cmd_epg_getTime(void **szArgList, UNIXTIME *utTime)
{
	HCHAR	*aucArg=NULL;
	HxDATETIME_t	stTime;

	aucArg = NULL;
	aucArg = HLIB_CMD_GetNextArg((HCHAR**)szArgList );
	if( aucArg == NULL )
	{
		HLIB_CMD_Printf("eitepg getTime1 [y] [m] [d] [h] [m] [s]\n");
		return HxCMD_ERR;
	}
	stTime.stDate.usYear = HLIB_STD_StrToINT32(aucArg);

	aucArg = NULL;
	aucArg = HLIB_CMD_GetNextArg((HCHAR**)szArgList );
	if( aucArg == NULL )
	{
		HLIB_CMD_Printf("eitepg getTime2 [y] [m] [d] [h] [m] [s]\n");
		return HxCMD_ERR;
	}
	stTime.stDate.ucMonth = HLIB_STD_StrToINT32(aucArg);

	aucArg = NULL;
	aucArg = HLIB_CMD_GetNextArg((HCHAR**)szArgList );
	if( aucArg == NULL )
	{
		HLIB_CMD_Printf("eitepg getTime3 [y] [m] [d] [h] [m] [s]\n");
		return HxCMD_ERR;
	}
	stTime.stDate.ucDay = HLIB_STD_StrToINT32(aucArg);

	aucArg = NULL;
	aucArg = HLIB_CMD_GetNextArg((HCHAR**)szArgList );
	if( aucArg == NULL)
	{
		HLIB_CMD_Printf("eitepg getTime4 [y] [m] [d] [h] [m] [s]\n");
		return HxCMD_ERR;
	}
	stTime.stTime.ucHour = HLIB_STD_StrToINT32(aucArg);

	aucArg = NULL;
	aucArg = HLIB_CMD_GetNextArg((HCHAR**)szArgList );
	if( aucArg == NULL )
	{
		HLIB_CMD_Printf("eitepg getTime5 [y] [m] [d] [h] [m] [s]\n");
		return HxCMD_ERR;
	}
	stTime.stTime.ucMinute = HLIB_STD_StrToINT32(aucArg);

	aucArg = NULL;
	aucArg = HLIB_CMD_GetNextArg((HCHAR**)szArgList );
	if( aucArg == NULL )
	{
		HLIB_CMD_Printf("eitepg getTime6 [y] [m] [d] [h] [m] [s]\n");
		return HxCMD_ERR;
	}
	stTime.stTime.ucSecond = HLIB_STD_StrToINT32(aucArg);

	HLIB_CMD_Printf("eitepg getTime [y:%d] [m:%d] [d:%d] [h:%d] [m:%d] [s:%d]\n" ,
		stTime.stDate.usYear , stTime.stDate.ucMonth, stTime.stDate.ucDay,
		stTime.stTime.ucHour , stTime.stTime.ucMinute, stTime.stTime.ucSecond );

	HLIB_DATETIME_ConvertDateTimeToUnixTime(&stTime, utTime);
	return HxCMD_OK;
}

int	cmd_epg_PrintEpgRawSec(void *szArgs)
{
	int					result = HxCMD_ERR;
	char				*szArg = NULL;
	unsigned long		ulKindOfTable = 0,ulTableHead=0,ulKindOfCmd=0;
	HERROR				hError = ERR_FAIL;
	HUINT8				aucStr[20];
	HUINT32				ulMessage = 0;

	if(szArgs == NULL)
	{
		HLIB_CMD_Printf("[cmd_epg_PrintEpgRawSec] Param NULL!!\n ");
	}

	// get 1st argument
	szArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgs);
	if (szArg == NULL)
	{
		HLIB_CMD_Printf("[cmd_epg_PrintEpgRawSec] Invalid param 1!!\n ");
		goto EXIT_EPGCMD_FUNC;
	}

	result = sscanf(szArg, "%s", aucStr);
	if (result == 0)
	{
		HLIB_CMD_Printf("[cmd_epg_PrintEpgRawSec] Invalid param 2!!\n ");
		goto EXIT_EPGCMD_FUNC;
	}

	HLIB_STD_StrUpper(aucStr);

	if (strcmp(aucStr, "PRINT") == 0)
	{
		ulKindOfCmd = EPG_CMD_PRINT;
	}
	else if (strcmp(aucStr, "CHECK") == 0)
	{
		ulKindOfCmd = EPG_CMD_CHECK_DONE;
	}
	else if (strcmp(aucStr, "STATUS") == 0)
	{
		ulKindOfCmd = EPG_CMD_CHECK_STATUS;
		goto SKIP_HEAD_PARAM;
	}
#if 0
	else if( strcmp( aucStr, "FIND" ) == 0 )
	{
		SvcEpg_Type_b bwType = eOxEPG_TYPE_ALL;
		UNIXTIME	stTime =0 , endTime =0;
		HUINT32		duration = 0, ulGenre =0;
		Handle_t		hSvc = 0;
		HINT32		hSvcNum =0;

		/* bwType */
		szArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgs);
		if (szArg == NULL)
		{
			HLIB_CMD_Printf("[cmd_epg_PrintEpgRawSec] plz input PF / SC / ALL!!\n ");
			goto EXIT_EPGCMD_FUNC;
		}

		result = sscanf(szArg, "%s", aucStr);
		if (result == 0)
		{
			HLIB_CMD_Printf("[cmd_epg_PrintEpgRawSec] get fail / plz input PF / SC / ALL!!\n ");
			goto EXIT_EPGCMD_FUNC;
		}

		if (strcmp(aucStr, "pf") == 0)
		{
			bwType = eOxEPG_TYPE_PF;
		}
		else if (strcmp(aucStr, "sch") == 0)
		{
			bwType = eOxEPG_TYPE_SC;
		}
		else
		{
			bwType = eOxEPG_TYPE_PF | eOxEPG_TYPE_SC;
		}

		HLIB_CMD_Printf("[cmd_epg_PrintEpgRawSec] bwType : %d !\n ", bwType);

		/* svc */
		szArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgs);
		if (szArg == NULL)
		{
			HLIB_CMD_Printf("[cmd_epg_PrintEpgRawSec] CUR / ALL / SVCID !!\n ");
			goto EXIT_EPGCMD_FUNC;
		}

		result = sscanf(szArg, "%s", aucStr);
		if (result == 0)
		{
			HLIB_CMD_Printf("[cmd_epg_PrintEpgRawSec] CUR / ALL / SVCID !!\n ");
			goto EXIT_EPGCMD_FUNC;
		}

		if (strcmp(aucStr, "cur") == 0)
		{
			if (MGR_ACTION_GetMasterSvcHandle(MGR_ACTION_GetMainActionId(), &hSvc) != ERR_OK)
			{
				HLIB_CMD_Printf("[cmd_epg_PrintEpgRawSec] fail to get current service handle !!\n ");
				goto EXIT_EPGCMD_FUNC;
			}
		}
		else if (strcmp(aucStr, "all") == 0)
		{
			hSvc = 0;
		}
		else
		{
			hSvcNum = HLIB_STD_StrToINT32(aucStr);
			hError = DB_SVC_GetServiceHandleByNumber( eDxDELIVERY_TYPE_CAB , eDxSVC_TYPE_All, hSvcNum, &hSvc );
			if( hError != ERR_OK )
			{
				HLIB_CMD_Printf("[cmd_epg_PrintEpgRawSec] invalid svc number : %s !!\n ", aucStr);
				goto EXIT_EPGCMD_FUNC;
			}
		}

		HLIB_CMD_Printf("[cmd_epg_PrintEpgRawSec] hsvc : %d !\n ", hSvc);

		/* search type  */
		szArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgs);
		if (szArg == NULL)
		{
			HLIB_CMD_Printf("[cmd_epg_PrintEpgRawSec] CUR / ALL / SVCID !!\n ");
			goto EXIT_EPGCMD_FUNC;
		}

		result = sscanf(szArg, "%s", aucStr);
		if (result == 0)
		{
			HLIB_CMD_Printf("[cmd_epg_PrintEpgRawSec] time / genre / keyword !!\n ");
			goto EXIT_EPGCMD_FUNC;
		}

		if (strcmp(aucStr, "time") == 0)
		{
			/* start time */
			hError = cmd_epg_getTime( &szArgs,	&stTime);
			if( hError != ERR_OK )
			{
				HLIB_CMD_Printf("[cmd_epg_PrintEpgRawSec] invalid start Time !!\n ");
				goto EXIT_EPGCMD_FUNC;
			}

			/* duration time */
			szArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgs );
			if( szArg == NULL )
			{
				HLIB_CMD_Printf("[cmd_epg_PrintEpgRawSec] invalid duration !!\n ");
				goto EXIT_EPGCMD_FUNC;
			}
			duration = HLIB_STD_StrToUINT32(szArg);
			endTime = stTime + duration;

			HLIB_CMD_Printf( "starttime : %d , end time : %d , duartion : %d \n" ,stTime , endTime, duration );

			SVC_EPG_PrintEventsTime( (HINT32)bwType, hSvc, stTime, endTime );
		}
		else if (strcmp(aucStr, "genre") == 0)
		{
			szArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgs );
			if( szArg == NULL )
			{
				HLIB_CMD_Printf("[cmd_epg_PrintEpgRawSec] invalid genre !!\n ");
				goto EXIT_EPGCMD_FUNC;
			}

			ulGenre = HLIB_STD_StrToINT32(aucStr);

			HLIB_CMD_Printf( "genre : 0x%x \n" , ulGenre);

			SVC_EPG_PrintEventsGenre( (HINT32)bwType, hSvc, ulGenre );
		}
		else if (strcmp(aucStr, "keyword") == 0)
		{
			szArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgs );
			if( szArg == NULL )
			{
				HLIB_CMD_Printf("[cmd_epg_PrintEpgRawSec] invalid keyword !!\n ");
				goto EXIT_EPGCMD_FUNC;
			}

			HLIB_CMD_Printf( "keyword : %s \n" , szArg);
			SVC_EPG_PrintEventsKeyword( (HINT32)bwType, hSvc, szArg );
		}
		else
		{
			HLIB_CMD_Printf( "search type : %s \n" , aucStr);
		}

		HLIB_CMD_Printf("End Print event!\n");
		goto EXIT_EPGCMD_FUNC;
	}
#endif
#if defined(CONFIG_MW_EPG_RAW_SAVE_DATA)
#if 0
	else if (strcmp(aucStr, "SAVE") == 0)
	{
		result = HxCMD_OK;
		EPGRAWDB_SaveEitSection(eEitSectionFileFlag_Hdd|eEitSectionFileFlag_Flash, 0);
		HxLOG_Print("Save file!\n");
		goto EXIT_EPGCMD_FUNC;
	}
	else if (strcmp(aucStr, "LOAD") == 0)
	{
		result = HxCMD_OK;
		EPGRAWDB_LoadEitSection(eEitSectionFileFlag_Hdd|eEitSectionFileFlag_Flash);
		HxLOG_Print("Load file!\n");
		goto EXIT_EPGCMD_FUNC;
	}
	else if (strcmp(aucStr, "DEL") == 0)
	{
		result = HxCMD_OK;
		EPGRAWDB_DeleteEpgFile(eEitSectionFileFlag_Hdd|eEitSectionFileFlag_Flash);
		HxLOG_Print("Delete file!\n");
		goto EXIT_EPGCMD_FUNC;
	}
#endif
#endif
	else
	{
		HLIB_CMD_Printf("[cmd_epg_PrintEpgRawSec] Invalid param :%s!!\n ",aucStr);
		goto EXIT_EPGCMD_FUNC;
	}
	HLIB_CMD_Printf("[cmd_epg_PrintEpgRawSec] ulKindOfCmd:%d \n ",ulKindOfCmd);


	// get 1st argument
	szArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgs);
	if (szArg == NULL)
	{
		HLIB_CMD_Printf("[cmd_epg_PrintEpgRawSec] Invalid param 1!!\n ");
		goto EXIT_EPGCMD_FUNC;
	}

	result = sscanf(szArg, "%s", aucStr);
	if (result == 0)
	{
		HLIB_CMD_Printf("[cmd_epg_PrintEpgRawSec] Invalid param 2!!\n ");
		goto EXIT_EPGCMD_FUNC;
	}

	HLIB_STD_StrUpper(aucStr);

	if (strcmp(aucStr, "PF") == 0)
	{
		ulKindOfTable = eEPG_PROCESS_EIT_PF;
	}
	else if (strcmp(aucStr, "SC") == 0)
	{
		ulKindOfTable = eEPG_PROCESS_EIT_SCH;
	}
	else
	{
		HLIB_CMD_Printf("[cmd_epg_PrintEpgRawSec] Invalid param :%s!!\n ",aucStr);
		goto EXIT_EPGCMD_FUNC;
	}
	HLIB_CMD_Printf("[cmd_epg_PrintEpgRawSec] ulKindOfTable:%d \n ",ulKindOfTable);

	// get 1st argument
	szArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgs);
	if (szArg == NULL)
	{
		HLIB_CMD_Printf("[cmd_epg_PrintEpgRawSec] No Table Head Information #1\n ");
		goto SKIP_HEAD_PARAM;
	}

	result = sscanf(szArg, "%s", aucStr);
	if (result == 0)
	{
		HLIB_CMD_Printf("[cmd_epg_PrintEpgRawSec] No Table Head Information #2\n ");
		goto SKIP_HEAD_PARAM;
	}

	HLIB_STD_StrUpper(aucStr);

	if (strcmp(aucStr, "ACT") == 0)
	{
		ulTableHead = 0;//ACT_HEAD;
	}
	else if (strcmp(aucStr, "OTH") == 0)
	{
		ulTableHead = 1;//OTH_HEAD;
	}
	else
	{
		HLIB_CMD_Printf("[cmd_epg_PrintEpgRawSec] Invalid param :%s!!\n ",aucStr);
		goto EXIT_EPGCMD_FUNC;
	}
	HLIB_CMD_Printf("[cmd_epg_PrintEpgRawSec] ulTableHead:%d \n ",ulTableHead);
SKIP_HEAD_PARAM:
	result = HxCMD_OK;

	hError = SVC_EPG_MakeEpgMessage(ulKindOfCmd, ulKindOfTable, &ulMessage);
	if(hError != ERR_EPG_OK)
	{
		HLIB_CMD_Printf("[cmd_epg_PrintEpgRawSec] MW_EPG_MakeEpgMessage fail!!\n");
		goto EXIT_EPGCMD_FUNC;
	}
	hError = SVC_EPG_SendMessage(ulMessage, 0, (HUINT32)ulTableHead, (HUINT32)0);
	if(hError != ERR_EPG_OK)
	{
		HLIB_CMD_Printf("[cmd_epg_PrintEpgRawSec] MW_EPG_MakeEpgMessage fail!!\n");
		goto EXIT_EPGCMD_FUNC;
	}

EXIT_EPGCMD_FUNC :

	return result;
}
#endif

void	CMD_Register_MW_EPG(void)
{
#if defined(CONFIG_DEBUG)
#define	hCmdHandle		"svc_epg"
	HLIB_CMD_RegisterWord(hCmdHandle, cmd_epg_Parse,			"epgp" ,
												"epg parsing" ,
												"parse [index(%d)]");

	HLIB_CMD_RegisterWord(hCmdHandle, cmd_epg_PrintEventInfo,	"epge",
												"epg event info",
												"epg event info");

	HLIB_CMD_RegisterWord(hCmdHandle, cmd_epg_CommandEpgService,	"epgc",
												"epg service command",
												"epg service command");

	HLIB_CMD_RegisterWord(hCmdHandle, cmd_epg_PrintEpgFindCommand, "epgfind",
												"epgfind command",
												"epgfind command");

	HLIB_CMD_RegisterWord(hCmdHandle, cmd_epg_PrintEpgRawSec, "rawepg",
												"rawepg [print/check] [pf/sc] [act/oth]",
												"rawepg [print/check] [pf/sc] [act/oth]");
	HLIB_CMD_RegisterWord(hCmdHandle, cmd_epg_PrintEpgRawSecCountPrint, "rawepgcnt",
												"rawepgcnt [print/byterate] [none/pf/sc/extpf/extsc]",
												"rawepgcnt [print/byterate] [none/pf/sc/extpf/extsc]");
#endif
}

