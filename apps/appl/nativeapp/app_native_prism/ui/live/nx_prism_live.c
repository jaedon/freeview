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
 * @file	  		nx_prism_live.c
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
#include <nx_prism_app.h>

#include "nx_prism_definition.h"

#include <nx_core_metadata_pf.h>


/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/
#define	NX_PRISM_LIVE_HYSTERESIS_TIMER_ID				0x001
#define	NX_PRISM_LIVE_HYSTERESIS_TIMER_INTERVAL			300		// 300msec
#define	NX_PRISM_LIVE_HYSTERESIS_THRESHOLD				2		// 2seconds

#define	NX_PRISM_LIVE_FCC_TIMER_ID						0x002
#define	NX_PRISM_LIVE_FCC_TIMER_INTERVAL				0		// immediately

#if defined(CONFIG_APUI_SCREEN_SAVER)
#define NX_PRISM_LIVE_SCREENSAVER_TIMER_ID				0x003
#endif

#if defined(CONFIG_DEBUG)
#define NX_PRISM_LIVE_SCREENSAVER_TIMER_LIVE	 		300 		// 5 minutes
#define NX_PRISM_LIVE_SCREENSAVER_TIMER_MEDIA	 		300 		// 5 minutes
#else
#define NX_PRISM_LIVE_SCREENSAVER_TIMER_LIVE	 		300 	// 5 minutes
#define NX_PRISM_LIVE_SCREENSAVER_TIMER_MEDIA	 		300 	// 5 minutes
#endif


#define	NX_PRISM_LIVE_MAX_VIEW_CNT						32

/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/
typedef struct
{
	HUINT32	svcuid;
	HCHAR eventName[EPG_HUMAX_NAME_LEN];
	HCHAR shortDescription[EPG_HUMAX_SHORT_LEN];
}NxUiLivePfData_t;

typedef struct
{
	HINT32	ver;
	HINT32	currentChannelIndex;			// channel list 목록에서 현재 zap 된 service 의 index
	HINT32	nChannelListNum;
	NX_Channel_t *pstChannelList;
	NX_ChListGroup_t	eCurGroupType;

	HCHAR	szChName[EPG_HUMAX_NAME_LEN];
	NxUiLivePfData_t stPfData;

	// hysteresis
	HUINT32			ulLastSetChannelTime;
	NX_Channel_t	stHystChannel;

	// Previous Channel Index
	NX_Channel_t		prevChannel;
	NX_ChListGroup_t	ePrevGroupType;

	// status
	HUINT32			eLastLiveEventStatus[NX_PRISM_LIVE_MAX_VIEW_CNT];	// live event change status
	HUINT32			eLastLiveViewStatus[NX_PRISM_LIVE_MAX_VIEW_CNT];	// av under run, no pmt ...
	HUINT32			eLastLiveLockStatus[NX_PRISM_LIVE_MAX_VIEW_CNT];	// block AV lock status

	// screen saver
	HBOOL					bRunScreenSaverTimer;
	NX_ScreenSaverMode_e	eScreenSaverMode;
}NxUiLiveContents_t;


/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/
static NxUiLiveContents_t	s_UiLiveContents;


/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/
static NxUiLiveContents_t* 	nx_live_GetContents(void);
static HERROR 				nx_live_CheckNoService(void);
static HERROR			nx_live_ReloadChannelListByGroup(NxUiLiveContents_t *pstContents, Handle_t hAct, HINT32 p1, HINT32 p2, HINT32 p3);

static HINT32 			nx_live_FindIndexBySvcUid(NxUiLiveContents_t *pstContents, HUINT32 ulFindSvcUid);
static HERROR 			nx_live_ChannelUp(NxUiLiveContents_t *pstContents);
static HERROR 			nx_live_ChannelDown(NxUiLiveContents_t *pstContents);
static ONDK_Result_t	nx_live_ChannelDCNSet(NxUiLiveContents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static HERROR 			nx_live_PrevChannel(NxUiLiveContents_t *pstContents);
static HERROR			nx_live_TrySetLastChannel(NxUiLiveContents_t *pstContents);
static HERROR			nx_live_ChannelTVRadio(NxUiLiveContents_t *pstContents);
static ONDK_Result_t	nx_live_RecordKey(NxUiLiveContents_t *pstContents, HINT32 nKey);

static HERROR			nx_live_UpdatePf(NxUiLiveContents_t *pstContents, HUINT32 svcuid);
static HERROR			nx_live_UpdateDolbyState(NxUiLiveContents_t *pstContents);

static HERROR 			nx_live_ActionRemoveLiveChannelList(NxUiLiveContents_t *pstContents);
static ONDK_Result_t	nx_live_ActionProcessStartService(NxUiLiveContents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);

static ONDK_Result_t	nx_live_CheckIsOTA(NxUiLiveContents_t *pstContents, Handle_t hAct,HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3);

static HERROR 			nx_live_SetChannel(NxUiLiveContents_t *pstContents, NX_Channel_t *channel);

static HERROR			nx_live_tryFastChannelChange(void);
static HERROR			nx_live_SetFastChannelChange(NxUiLiveContents_t *pstContents);

static HERROR 			nx_live_Initilization(NxUiLiveContents_t *pstContents, Handle_t hAct, HINT32 p1, HINT32 p2, HINT32 p3);



// APK Message Action process
static HERROR			nx_live_ActionProcessTuneLocked(NxUiLiveContents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);

static ONDK_Result_t	nx_live_APKMessage(NxUiLiveContents_t *pstContents, Handle_t hAct,HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3);

static ONDK_Result_t 	nx_live_MsgGwmCreate(NxUiLiveContents_t *pstContents, Handle_t hAct, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	nx_live_MsgGwmDestroy(NxUiLiveContents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_live_MsgGwmHapiMsg(Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	nx_live_MsgGwmKeyDown(NxUiLiveContents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	nx_live_MsgGwmTimer(NxUiLiveContents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);

//static ONDK_Result_t 	nx_live_CreateUIObject(NxUiLiveContents_t *pstContents, Handle_t hAct, HINT32 p1, HINT32 p2, HINT32 p3);

/********** related channel inform object (channel list, hub..) ************/
static ONDK_Result_t 	nx_live_UpdateChannelInformObject(NxUiLiveContents_t *pstContents, HINT32 keyId);

/********** related screen saver ************/
static ONDK_Result_t	nx_live_UpdateScreenSaverStatus(NxUiLiveContents_t *pstContents, Handle_t hAct,HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3);
static void 			nx_live_SetRunScreenSaver(NxUiLiveContents_t *pstContents, HBOOL bRunScreenSaver);
/***********  end screen saver  *************/

/************  end channel list *************/

/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/
static ONDK_Result_t	nx_live_ResponseDialogProc(HINT32 lMessage, Handle_t hHandle, HINT32 lParam1, HINT32 lParam2, HINT32 lParam3)
{
	switch (lMessage)
	{
		case MSG_GWM_KEYDOWN:
			NX_PRISM_UTIL_KeyConv_UiCommon (&lParam1);
			switch (lParam1)
			{
				case KEY_RED:
				case KEY_GREEN:
				case KEY_YELLOW:
				case KEY_BLUE:
				case KEY_ARROWUP :
				case KEY_ARROWDOWN:
				case KEY_TVRADIO:
				case KEY_EXT_AV:
				case KEY_COMBO:
				case KEY_TEXT:
				case KEY_TTX_HOLD:
				case KEY_TTX_SUBPAGE:
				case KEY_AUDIO:
				case KEY_SUBTITLE:
				case KEY_WIDE:
				case KEY_3D:
				case KEY_INFO:
				case KEY_BACK:
				case KEY_EXT_AV_PLUS:
				case KEY_EXT_AV_MINUS:
					ONDK_GWM_APP_Destroy(0);
					return	ONDK_MESSAGE_BREAK;
				default:
					break;
			}
			break;

		case MSG_APP_SYSTEM_SVC_CHANGED :
			ONDK_GWM_APP_Destroy(0);
			return	ONDK_MESSAGE_PASS;

		default :
			break;
	}

	return	NX_PRISM_DIALOGUE_ResponseMessage_Proc(lMessage, hHandle, lParam1, lParam2, lParam3);
}


static void		nx_live_CreateResponseDialouge(HUINT8* pszString)
{
	if (pszString == NULL)
	{
		NX_APP_Error("Error!!! Pls check pszString was NULL!! \n");
		return;
	}

	ONDK_GWM_APP_Create (ONDK_UPPER_WINDOW, "nx_live_ResponseDialogProc", APP_DEFAULT_PRIORITY,
						nx_live_ResponseDialogProc, 0, (HINT32)pszString, 0, 0);
}


static NxUiLiveContents_t*	nx_live_GetContents(void)
{
	return &s_UiLiveContents;
}

static HERROR	nx_live_Initilization(NxUiLiveContents_t *pstContents, Handle_t hAct, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR		hError = ERR_FAIL;
	HINT32		lGroupType = (HINT32)eNxChListGroup_TV, lTvSvcCount = 0, lRadioSvcCount = 0;
	HUINT32		ulExtraFilter = 0;
	DxService_t *lastService = NULL;


	lTvSvcCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_TV);
	lRadioSvcCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_RADIO);

	// no service
	if ((lTvSvcCount + lRadioSvcCount) < 1)
	{
		NX_APP_Error("\n\n Error!!! Not found any service !!\n\n\n");

		return	ERR_FAIL;
	}

	hError = NX_CHANNEL_GetLastServiceGroup(&lGroupType);
	if (hError != ERR_OK)
	{
		NX_APP_Error("\n\n Error!!! NO DB LAST SERVICE GROUP !!\n\n\n");
		lGroupType = (HINT32)eNxChListGroup_TV;
		NX_CHANNEL_SetLastServiceGroup(lGroupType);
	}

	pstContents->eCurGroupType = (NX_ChListGroup_t)lGroupType;
	pstContents->ePrevGroupType = pstContents->eCurGroupType;
	ulExtraFilter = NX_CHANNEL_GROUP_GetExtraFilterByGroupType(pstContents->eCurGroupType);

	hError = NX_CHANNEL_GROUP_MakeNxChannelListGroup(&pstContents->pstChannelList, &pstContents->nChannelListNum,
												pstContents->eCurGroupType, ulExtraFilter	/* DxAlpabetGruopType_b .. etc*/);
	if (hError != ERR_OK)
	{
		NX_APP_Error("Error!!! NX_CHANNEL_GROUP_MakeNxChannelListAll() !!\n");

		/* Try aging TV Service if do not load last group list */
		if (lTvSvcCount > 0)
		{
			lGroupType = (HINT32)eNxChListGroup_TV;
		}
		else if (lRadioSvcCount > 0)
		{
			lGroupType = (HINT32)eNxChListGroup_RADIO;
		}
		pstContents->eCurGroupType = (NX_ChListGroup_t)lGroupType;
		pstContents->ePrevGroupType = pstContents->eCurGroupType;
		ulExtraFilter = NX_CHANNEL_GROUP_GetExtraFilterByGroupType(pstContents->eCurGroupType);
		hError = NX_CHANNEL_GROUP_MakeNxChannelListGroup(&pstContents->pstChannelList, &pstContents->nChannelListNum,
													pstContents->eCurGroupType, ulExtraFilter	/* DxAlpabetGruopType_b .. etc*/);
		if (hError != ERR_OK)
		{
			NX_APP_Error("Error!!! Try again NX_CHANNEL_GROUP_MakeNxChannelListAll() - lGroupType(0x%x) !!\n", lGroupType);
			return ERR_FAIL;
		}
	}

	lastService = NX_CHANNEL_GetLastService();
	if (lastService)
	{
		pstContents->currentChannelIndex = nx_live_FindIndexBySvcUid(pstContents, lastService->uid);
		if (pstContents->currentChannelIndex < 0)
		{
			NX_APP_Error("Error!!! nx_live_FindIndexBySvcUid() Not Found service UID :[0x%x] !!", lastService->uid);
			pstContents->currentChannelIndex = 0;
		}
	}
	else
	{
		pstContents->currentChannelIndex = 0;
	}

	// load favourite
	NX_CHANNEL_FAV_Initialize();

	nx_live_SetChannel(pstContents, &pstContents->pstChannelList[pstContents->currentChannelIndex]);

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);
	NX_COMMON_NOT_USED_PARAM(hAct);

	return ERR_OK;
}

static HERROR nx_live_CheckNoService(void)
{
	HINT32 nCnt = 0;

	nCnt = NX_CHANNEL_GetChanenlCountAll();

	if(nCnt < 1)
	{
		if (ONDK_GWM_APP_Get((ONDK_GWM_Callback_t)NX_PRISM_NO_SERVICE_MGR_Proc) == NULL)
		{
			ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW,"NX_PRISM_NO_SERVICE_MGR_Proc", APP_UIMGR_PRIORITY, NX_PRISM_NO_SERVICE_MGR_Proc, HANDLE_NULL, 0, 0, 0);
		}
	}
	else
	{
		if (ONDK_GWM_APP_Get((ONDK_GWM_Callback_t)NX_PRISM_NO_SERVICE_MGR_Proc) != NULL)
		{
			ONDK_GWM_APP_Destroy(NX_PRISM_NO_SERVICE_MGR_Proc);
		}
	}

	return ERR_OK;
}

static HERROR	nx_live_ReloadChannelListByGroup(NxUiLiveContents_t *pstContents, Handle_t hAct, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HBOOL		bChangeFirstSvc = FALSE, bSkipFirstSvc = (HBOOL)p1;
	HERROR		hError = ERR_FAIL;
	HINT32		lGroupType = (HINT32)eNxChListGroup_TV;
	HINT32		lChIndex = -1, lCount = 0;
	HUINT32		ulExtraFilter = 0;
	DxService_t 	*stLastService = NULL;


	hError = NX_CHANNEL_GetLastServiceGroup(&lGroupType);
	if (hError != ERR_OK)
	{
		lGroupType = (HINT32)eNxChListGroup_TV;
		NX_CHANNEL_SetLastServiceGroup(lGroupType);
	}

	// 1 - Load Last Group
	pstContents->eCurGroupType = (NX_ChListGroup_t)lGroupType;
	ulExtraFilter = NX_CHANNEL_GROUP_GetExtraFilterByGroupType(pstContents->eCurGroupType);

	hError = NX_CHANNEL_GROUP_MakeNxChannelListGroup(&pstContents->pstChannelList, &pstContents->nChannelListNum,
												pstContents->eCurGroupType, ulExtraFilter	/* DxAlpabetGruopType_b .. etc*/);
	if ((hError == ERR_OK) && (pstContents->nChannelListNum > 0))
	{
		// 1-1 Check last service include Current Group list
		stLastService = NX_CHANNEL_GetLastService();
		if (stLastService)
		{
			lChIndex = nx_live_FindIndexBySvcUid(pstContents, stLastService->uid);
			if (lChIndex < 0)
			{
				NX_APP_Error("Error!!! nx_live_FindIndexBySvcUid() Not Found Last service UID :[0x%x] in Group(0x%x)!!\n", stLastService->uid, pstContents->eCurGroupType);
				bChangeFirstSvc = TRUE;
				lChIndex = 0;
			}
			else
			{
				bChangeFirstSvc = TRUE;
			}
		}
		else
		{
			NX_APP_Error("Error!!! NX_CHANNEL_GetLastService() Last service Fail!! \n!!");
			bChangeFirstSvc = TRUE;
			lChIndex = 0;
		}

		// 1-2 Channel Chanage first Service in Group list
		if (bChangeFirstSvc == TRUE && bSkipFirstSvc != TRUE)
		{
			NX_PRISM_LIVE_ServiceStartMainView(TRUE, pstContents->pstChannelList[lChIndex].svcUid, eNxChListGroup_CURRENT, 0);
		}
	}
	else
	{
		NX_APP_Error("Error!!! NX_CHANNEL_GROUP_MakeNxChannelListGroup() Fail !! Group(0x%x)!!\n", pstContents->eCurGroupType);
		// 2. Load Other Group
		lCount = NX_CHANNEL_GetChanenlCountAll();
		if (lCount > 0)
		{
			lGroupType = (HINT32)eNxChListGroup_TV;
			lCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_TV);
			if (lCount < 1)
			{
				lGroupType = (HINT32)eNxChListGroup_RADIO;
				lCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_RADIO);
			}

			if (lCount < 1)
			{
				NX_APP_Error("Error!!! Error!!! Error!!! No Service - Oh my God !! \n");
				return ERR_FAIL;
			}

			lChIndex = 0;
			pstContents->eCurGroupType = (NX_ChListGroup_t)lGroupType;
			ulExtraFilter = NX_CHANNEL_GROUP_GetExtraFilterByGroupType(pstContents->eCurGroupType);

			hError = NX_CHANNEL_GROUP_MakeNxChannelListGroup(&pstContents->pstChannelList, &pstContents->nChannelListNum,
														pstContents->eCurGroupType, ulExtraFilter	/* DxAlpabetGruopType_b .. etc*/);
			if ((hError == ERR_OK) && (pstContents->nChannelListNum > 0))
			{
				ONDK_GWM_SendMessage(NULL, MSG_APP_SYSTEM_RELOAD_SERVICELIST, 0, 0, 0, 0);
				NX_PRISM_LIVE_ServiceStartMainView(TRUE, pstContents->pstChannelList[lChIndex].svcUid, pstContents->eCurGroupType, 0);
			}
			else
			{
				NX_APP_Error("Error!!! NX_CHANNEL_GROUP_MakeNxChannelListGroup() Fail- Oh my God !! Group(0x%x)!!\n", pstContents->eCurGroupType);
				return ERR_FAIL;
			}
		}
		else
		{
			NX_APP_Error("Error!!! No more service !!\n");
			return ERR_FAIL;
		}
	}

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);
	NX_COMMON_NOT_USED_PARAM(hAct);

	return ERR_OK;
}

static HINT32	nx_live_FindIndexBySvcUid(NxUiLiveContents_t *pstContents, HUINT32 ulFindSvcUid)
{
	HINT32	lFindIndex = 0;

	for (lFindIndex = 0; lFindIndex < pstContents->nChannelListNum; lFindIndex++)
	{
		if (pstContents->pstChannelList[lFindIndex].svcUid == ulFindSvcUid)
		{
			return	lFindIndex;
		}
	}

	return -1;
}

static HERROR	nx_live_SetChannel(NxUiLiveContents_t *pstContents, NX_Channel_t *channel)
{
	HERROR		hErr = ERR_FAIL;
	HBOOL		bChangable = FALSE;
	HINT32  	lInfoDisplayTime = NX_COMMON_TIME_DEFAULT;
	ONDK_GWM_App_t	pGwmApp = NULL;
	ONDK_Result_t	eResult = ONDK_RET_FAIL;

	if (channel == NULL)
	{
		HxLOG_Error("channel is NULL!\n");
		return ERR_FAIL;
	}

	if(pstContents == NULL)
	{
		HxLOG_Error("pstContents is NULL!\n");
		return ERR_FAIL;
	}

	if (NX_CHANNEL_CheckPlayChangable(channel, &bChangable) != ERR_OK)
	{
		HxLOG_Error("disable channel change!\n");
		return ERR_FAIL;
	}

	if (bChangable)
	{

		HINT32 nDiff = abs(HLIB_STD_GetSystemTime() - pstContents->ulLastSetChannelTime);
		HxLOG_Print("_ diff %d\n", nDiff);
		// --- hysteresis channe input---
		// it can avoid a over heading of set channel.
		// if it activated then the set channel is called after 300msec via timer. (val diff is a key of funtion activation)
		if(nDiff < NX_PRISM_LIVE_HYSTERESIS_THRESHOLD)
		{
			eResult = ONDK_GWM_SetTimer(NX_PRISM_LIVE_HYSTERESIS_TIMER_ID, NX_PRISM_LIVE_HYSTERESIS_TIMER_INTERVAL);
			if (eResult == GWM_RET_LINK_CONFLICT)
			{
				ONDK_GWM_ResetTimer(NX_PRISM_LIVE_HYSTERESIS_TIMER_ID);
			}
			pstContents->stHystChannel.lcn = channel->lcn;
			pstContents->stHystChannel.svcUid = channel->svcUid;
			hErr = ERR_OK;
		}
		else
		{
			HxLOG_Warning("live Set Channel direct : [ %04d: %08x]\n", channel->lcn, channel->svcUid);

			NX_PRISM_LIVE_SetPrevChannel(channel);

			hErr = NX_CHANNEL_SetChannel(channel);
		}
		pstContents->ulLastSetChannelTime = HLIB_STD_GetSystemTime();

		if (hErr == ERR_OK)
		{
			// update channel info
			pGwmApp = ONDK_GWM_APP_Get((ONDK_GWM_Callback_t)NX_PRISM_CHINFO_Proc);
			if(pGwmApp != NULL)
			{
				ONDK_GWM_APP_Destroy(NX_PRISM_CHINFO_Proc);
			}

			NX_SYSTEM_GetInfoDisplayTime(&lInfoDisplayTime);
			if (lInfoDisplayTime > 0)
			{
				ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, "NX_PRISM_CHINFO_Proc", APP_BACKGROUND_OSD_PRIORITY, NX_PRISM_CHINFO_Proc, 0, channel->lcn, channel->svcUid, 0);
			}
		}

		return ERR_OK;
	}
	else
	{
		HxLOG_Error("____ cannot changealbe!!![ %04d: %08x]\n", channel->lcn, channel->svcUid);
		nx_live_CreateResponseDialouge((HUINT8*)ONDK_GetString(RES_MESG_3196_ID)); // cannot changeable
	}

	NX_COMMON_NOT_USED_PARAM(pstContents);

	return ERR_FAIL;
}

static HERROR	nx_live_tryFastChannelChange(void)
{
	ONDK_Result_t	eResult = ONDK_RET_FAIL;

	// pass to timer
	eResult = ONDK_GWM_SetTimer(NX_PRISM_LIVE_FCC_TIMER_ID, NX_PRISM_LIVE_FCC_TIMER_INTERVAL);
	if (eResult == GWM_RET_LINK_CONFLICT)
	{
		ONDK_GWM_ResetTimer(NX_PRISM_LIVE_FCC_TIMER_ID);
	}
	return ERR_OK;
}

static HERROR	nx_live_SetFastChannelChange(NxUiLiveContents_t *pstContents)
{
#if defined(CONFIG_SUPPORT_FCC)
	#define		DEF_FCC_INTANCE		2

	HUINT32 	ulCnt = 0;
	HINT32		nFccTargetIndex;

	if (pstContents->pstChannelList == NULL ||
		pstContents->nChannelListNum <= 0)
	{
		return ERR_FAIL;
	}

	for(ulCnt = 0 ; ulCnt < DEF_FCC_INTANCE ; ulCnt++)
	{
		nFccTargetIndex = pstContents->currentChannelIndex;
		switch(ulCnt)
		{
			case 0:	// next
				nFccTargetIndex++;
				if (nFccTargetIndex >= pstContents->nChannelListNum)
				{
					nFccTargetIndex = 0;
				}
				break;
			case 1:	// previous
				nFccTargetIndex--;
				if (nFccTargetIndex < 0)
				{
					nFccTargetIndex = pstContents->nChannelListNum - 1;
				}
				break;
			case 2:	// next X 2
				break;
			case 3:	// preious X 2
				break;
			default:
				break;
		}
		NX_CHANNEL_SetFastChannelChange(&pstContents->pstChannelList[nFccTargetIndex]);

	}

#endif
	return ERR_OK;
}


static HERROR	nx_live_ChannelUp(NxUiLiveContents_t *pstContents)
{
	HERROR		hErr = ERR_FAIL;
	HBOOL		bChangable = FALSE;
	HINT32		ulCnt = 0;

	if (pstContents->pstChannelList == NULL ||
		pstContents->nChannelListNum <= 0)
		return ERR_FAIL;

	for(ulCnt = 0 ; ulCnt < pstContents->nChannelListNum ; ulCnt++)
	{
		pstContents->currentChannelIndex++;

		if (pstContents->currentChannelIndex >= pstContents->nChannelListNum)
			pstContents->currentChannelIndex = 0;

		hErr = NX_CHANNEL_CheckPlayChangable(&pstContents->pstChannelList[pstContents->currentChannelIndex], &bChangable);
		if(hErr == ERR_OK && bChangable == TRUE)
		{
			break;
		}
	}

	pstContents->ePrevGroupType = pstContents->eCurGroupType;

	return nx_live_SetChannel(pstContents, &pstContents->pstChannelList[pstContents->currentChannelIndex]);
}


static HERROR	nx_live_ChannelDown(NxUiLiveContents_t *pstContents)
{
	HERROR		hErr = ERR_FAIL;
	HBOOL		bChangable = FALSE;
	HINT32		ulCnt = 0;

	if (pstContents->pstChannelList == NULL ||
		pstContents->nChannelListNum <= 0)
		return ERR_FAIL;

	for(ulCnt = 0 ; ulCnt < pstContents->nChannelListNum ; ulCnt++)
	{
		pstContents->currentChannelIndex--;

		if (pstContents->currentChannelIndex < 0)
			pstContents->currentChannelIndex = pstContents->nChannelListNum - 1;

		hErr = NX_CHANNEL_CheckPlayChangable(&pstContents->pstChannelList[pstContents->currentChannelIndex], &bChangable);
		if(hErr == ERR_OK && bChangable == TRUE)
		{
			break;
		}
	}

	pstContents->ePrevGroupType = pstContents->eCurGroupType;

	return nx_live_SetChannel(pstContents, &pstContents->pstChannelList[pstContents->currentChannelIndex]);
}

static HERROR	nx_live_PrevChannel(NxUiLiveContents_t *pstContents)
{
	HERROR			hErr = ERR_FAIL;
	HBOOL			bChangable = FALSE;
	NX_Channel_t	stCh = pstContents->prevChannel;
	HINT32			channelIndex = 0, lPrevGroupType = (HINT32)pstContents->ePrevGroupType;
	DxService_t 	*stFindService = NULL;

	if((pstContents->prevChannel.svcUid == 0 && pstContents->prevChannel.lcn == 0)||
		NX_CHANNEL_GROUP_GetGroupCount(pstContents->ePrevGroupType) < 0)
	{
		NX_APP_Error("Error!!! No prevChannel svc info !!\n");
		return ERR_FAIL;
	}

	hErr = NX_CHANNEL_CheckPlayChangable(&stCh, &bChangable);
	if(hErr == ERR_FAIL || bChangable == FALSE)
	{
		HxLOG_Error("Error!!! disable channel change!\n");
		return ERR_FAIL;
	}

	if(pstContents->prevChannel.svcUid == pstContents->pstChannelList[pstContents->currentChannelIndex].svcUid)
	{
		HxLOG_Error("Error!!! same channel !\n");
		return ERR_FAIL;
	}

	pstContents->ePrevGroupType = pstContents->eCurGroupType;

	if(lPrevGroupType != (HINT32)pstContents->eCurGroupType){
		stFindService = NX_CHANNEL_GROUP_FindServiceInGroupByServiceUid((NX_ChListGroup_t)lPrevGroupType, eNxFindSvcDir_Current, stCh.svcUid);
		if (stFindService == NULL)
		{
			if(NX_CHANNEL_GROUP_IsServiceInGroup(eNxChListGroup_RADIO, &stCh) == ERR_OK)
			{
				lPrevGroupType = (HINT32)eNxChListGroup_RADIO;
			}
			else
			{
				lPrevGroupType = (HINT32)eNxChListGroup_TV;
			}
		}
		// first remove group channel list in context.
		nx_live_ActionRemoveLiveChannelList(pstContents);

		// Change Group
		pstContents->eCurGroupType = (NX_ChListGroup_t)lPrevGroupType;
		NX_CHANNEL_SetLastServiceGroup(pstContents->eCurGroupType);

		// reload group channel list
		nx_live_ReloadChannelListByGroup(pstContents, 0, TRUE, 0, 0);
	}

	// Find Service Index
	channelIndex = nx_live_FindIndexBySvcUid(pstContents, stCh.svcUid);
	if (channelIndex < 0)
	{
		NX_APP_Error("Error!!! nx_live_FindIndexBySvcUid() Not Found service UID :[0x%x] !!", stCh.svcUid);
		return ERR_FAIL;
	}

	pstContents->currentChannelIndex = channelIndex;
	stCh.lcn = pstContents->pstChannelList[channelIndex].lcn;

	return nx_live_SetChannel(pstContents, &stCh);
}

static HERROR	nx_live_TrySetLastChannel(NxUiLiveContents_t *pstContents)
{
	HERROR		hErr = ERR_FAIL;
	HBOOL		bChangable = FALSE;

	if (pstContents->pstChannelList == NULL ||
		pstContents->nChannelListNum <= 0)
		return ERR_FAIL;

	hErr = NX_CHANNEL_CheckPlayChangable(&pstContents->pstChannelList[pstContents->currentChannelIndex], &bChangable);

	pstContents->ePrevGroupType = pstContents->eCurGroupType;

	return nx_live_SetChannel(pstContents, &pstContents->pstChannelList[pstContents->currentChannelIndex]);
}

static ONDK_Result_t	nx_live_ChannelDCNSet(NxUiLiveContents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR			hErr = ERR_FAIL;
	HBOOL			bChangable = FALSE;
	NX_Channel_t	stCh;
	HINT32			lChannelIndex = 0;

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	stCh.lcn = p1;
	stCh.svcUid = p2;

	hErr = NX_CHANNEL_CheckPlayChangable(&stCh, &bChangable);
	if(hErr == ERR_OK && bChangable == TRUE)
	{
		if(stCh.svcUid == pstContents->pstChannelList[pstContents->currentChannelIndex].svcUid)
		{
			HxLOG_Error("Error!!! Tune Fail ,same channel! \n");
			return ERR_FAIL;
		}

		// Find Service Index
		lChannelIndex = nx_live_FindIndexBySvcUid(pstContents, stCh.svcUid);
		if (lChannelIndex < 0)
		{
			NX_APP_Error("Error!!! nx_live_FindIndexBySvcUid() Not Found service UID :[0x%x] !!", stCh.svcUid);
			return ERR_FAIL;
		}
		pstContents->currentChannelIndex = lChannelIndex;

		pstContents->ePrevGroupType = pstContents->eCurGroupType;
		nx_live_SetChannel(pstContents, &stCh);

	}
	else
	{
		nx_live_CreateResponseDialouge((HUINT8*)ONDK_GetString(RES_MESG_3196_ID));
	}

	return ONDK_MESSAGE_BREAK;
}

static HERROR	nx_live_ChannelTVRadio(NxUiLiveContents_t *pstContents)
{
	HINT32		lGroupNum = 0;
	HUINT32		ulExtraFilter = 0;
	NX_ChListGroup_t eTargetChGroupType = eNxChListGroup_TV;
	NX_Channel_t	*pstGroupList = NULL;

	if (pstContents->eCurGroupType != eNxChListGroup_TV)
	{
		eTargetChGroupType = eNxChListGroup_TV;
	}
	else
	{
		eTargetChGroupType = eNxChListGroup_RADIO;
	}

	ulExtraFilter = NX_CHANNEL_GROUP_GetExtraFilterByGroupType(eTargetChGroupType);
	// make group list
	if (ERR_OK != NX_CHANNEL_GROUP_MakeNxChannelListGroup(&pstGroupList, &lGroupNum, eTargetChGroupType, ulExtraFilter))
	{
		HxLOG_Debug("\nFailed Make Group list! (eChGroupType : %d)\n", eTargetChGroupType);
		return ERR_FAIL;
	}

	if (lGroupNum <= 0)
	{
		NX_APP_Error("Error!!! eChGroupType : (0x%x)(lGroupNum <= 0) !!\n", eTargetChGroupType);
		if(pstGroupList)
		{
			NX_APP_Free(pstGroupList);
			pstGroupList = NULL;
		}
		return ERR_FAIL;
	}

	HxLOG_Debug("\nNew group list Num : %d \n\n", lGroupNum);

	if(pstGroupList != NULL)
	{
		NX_PRISM_LIVE_ServiceStartMainView(TRUE, pstGroupList[0].svcUid, eTargetChGroupType, 0);
	}

	if(pstGroupList != NULL)
	{
		NX_APP_Free(pstGroupList);
		pstGroupList = NULL;
	}

	return ERR_OK;
}

static ONDK_Result_t	nx_live_RecordKey(NxUiLiveContents_t *pstContents, HINT32 nKey)
{
#if defined(CONFIG_MW_MM_PVR)
	HERROR hErr = ERR_FAIL;
	HUINT32 ulViewId = 0;
	OxMediaPlay_MediaType_e		eMediaType = eOxMP_MEDIATYPE_NONE;
	HINT32	nRecInstCnt = 0;

	NX_MEDIA_PLAY_GetMainViewId(&ulViewId);
	NX_MEDIA_PLAY_GetPlayType(ulViewId, &eMediaType);

	if(eMediaType != eOxMP_MEDIATYPE_LIVE)
	{
		HxLOG_Warning("No live status\n");
		return ONDK_RET_OK;
	}

	switch (nKey)
	{
		case KEY_RECORD:
			if(pstContents && pstContents->pstChannelList)
			{
				HUINT32 ulSvcUid = pstContents->pstChannelList[pstContents->currentChannelIndex].svcUid;

				if(NX_RECORDER_FindRecordingInstBySvcUid(ulSvcUid) == TRUE)
				{
					// TODO: if same service > change time dialog
					HxLOG_Error("change record time dialog !!!\n");
					break;
				}
				else
				{
					hErr = NX_RECORDER_RecordNow(&pstContents->pstChannelList[pstContents->currentChannelIndex], NX_RECORDER_DEFAULT_REC_DURATION);
					if(hErr != ERR_OK)
					{
						HxLOG_Warning("failure to start record\n");
					}
				}
			}
			else
			{
				HxLOG_Warning("No channel available\n");
			}
			return ONDK_MESSAGE_CONSUMED;

		case KEY_STOP:
			nRecInstCnt = NX_RECORDER_GetRecordingInstCount();
			if(nRecInstCnt == 0)
			{
				HxLOG_Error("No Recording Instances are there!!!\n");
				break;
			}
			else if (nRecInstCnt == 1)
			{
				NxRecInstItem_t	*pRecInstItem = NX_RECORDER_GetRecordingInstItem(0); // index 0
				if(pRecInstItem)
				{
					hErr = NX_RECORDER_StopRecording(pRecInstItem->ulSessionId);
					if(hErr != ERR_OK)
					{
						HxLOG_Warning("failure to stop record\n");
					}
				}
			}
			else
			{
				// TODO: show stop dlg
			}

			return ONDK_MESSAGE_CONSUMED;

		default:
			break;
	}
#else
	HxLOG_Warning("Not supported function!!!");
#endif

	NX_COMMON_NOT_USED_PARAM(pstContents);
	NX_COMMON_NOT_USED_PARAM(nKey);

	return ONDK_RET_OK;
}


static HERROR	nx_live_UpdatePf(NxUiLiveContents_t *pstContents, HUINT32 svcuid)
{
	NX_COMMON_NOT_USED_PARAM(pstContents);
	NX_COMMON_NOT_USED_PARAM(svcuid);

	return ERR_OK;
}


static HERROR	nx_live_UpdateDolbyState(NxUiLiveContents_t *pstContents)
{
	HERROR			hError = ERR_FAIL;
	HINT32			lAudioTrackIndex = 0;
	HUINT32			ulViewId = 0;
	OxMediaPlay_MediaType_e		eMediaType = eOxMP_MEDIATYPE_NONE;
	DxAudioCodec_e				eAudCodec = eDxAUDIO_CODEC_DOLBY_AC3;
	OxMediaPlay_Component_t  	*pstAudioComponent = NULL;


	NX_MEDIA_PLAY_GetMainViewId(&ulViewId);
	NX_MEDIA_PLAY_GetPlayType(ulViewId, &eMediaType);

	switch (eMediaType)
	{
		case eOxMP_MEDIATYPE_LIVE:
		case eOxMP_MEDIATYPE_PVRPLAYBACK:
		case eOxMP_MEDIATYPE_TSRPLAYBACK:
			pstAudioComponent = (OxMediaPlay_Component_t*)NX_APP_Calloc(sizeof(OxMediaPlay_Component_t));
			hError = NX_MEDIA_PLAY_GetComponentIndex(ulViewId, eOxMP_COMPONENT_AUDIO, &lAudioTrackIndex, NULL);
			if ((hError == ERR_OK) && (lAudioTrackIndex >= 0))
			{
				HxSTD_MemSet (pstAudioComponent, 0x0, sizeof(OxMediaPlay_Component_t));
				hError = NX_MEDIA_PLAY_GetComponent(ulViewId, eOxMP_COMPONENT_AUDIO, lAudioTrackIndex, pstAudioComponent);
				if (hError == ERR_OK)
				{
					switch (pstAudioComponent->stAudio.eCodec)
					{
						case eDxAUDIO_CODEC_DOLBY_AC3:
						case eDxAUDIO_CODEC_DOLBY_AC3P:
							eAudCodec = pstAudioComponent->stAudio.eCodec;
							NX_PRISM_DOLBY_ShowDolbyLogo(eAudCodec);
							break;
						default:
							NX_APP_Info("Only display DOLBY Audio : (%d)\n", pstAudioComponent->stAudio.eCodec);
							break;
					}
				}
				else
				{
					NX_APP_Error("[%s:%d] Error!!! NX_MEDIA_PLAY_GetComponent()-eOxMP_COMPONENT_AUDIO ulViewId:(%d), lAudioTrackIndex:(%d)\n", __FUNCTION__, __HxLINE__, ulViewId, lAudioTrackIndex);
				}
			}
			else
			{
				NX_APP_Error("[%s:%d] Error!!! NX_MEDIA_PLAY_GetComponentIndex()-eOxMP_COMPONENT_AUDIO ulViewId:(%d)\n", __FUNCTION__, __HxLINE__, ulViewId);
			}

			if (pstAudioComponent)
			{
				NX_APP_Free(pstAudioComponent);
				pstAudioComponent = NULL;
			}
			break;

		case eOxMP_MEDIATYPE_MEDIAPLAY:
			return eOxMP_SUBTITLETYPE_Media;

		default:
			break;
	}

	NX_COMMON_NOT_USED_PARAM(pstContents);

	return ERR_OK;
}

static HERROR	nx_live_UpdateRadioStill(NxUiLiveContents_t *pstContents)
{
	HUINT32					ulViewId = 0;
	OxMediaPlay_MediaType_e	eMediaType = eOxMP_MEDIATYPE_NONE;
	HBOOL					bNeedRadioBG = FALSE;

	NX_MEDIA_PLAY_GetMainViewId(&ulViewId);
	NX_MEDIA_PLAY_GetPlayType(ulViewId, &eMediaType);

	NX_MEDIA_PLAY_GetNeedRadioBg(ulViewId,&bNeedRadioBG);
	if(bNeedRadioBG == TRUE)
	{
		HxLOG_Warning("Radio Still show\n");
		pstContents->eScreenSaverMode = eNxScreenSaverMode_RADIO;
		nx_live_SetRunScreenSaver(pstContents, TRUE);
		NX_PRISM_RADIO_CreateStill();
	}
	else
	{
		HxLOG_Warning("Radio Still Hide\n");
		pstContents->eScreenSaverMode = eNxScreenSaverMode_LIVE;
		NX_PRISM_RADIO_CloseStill();
	}

	NX_COMMON_NOT_USED_PARAM(pstContents);

	return ERR_OK;
}


static ONDK_Result_t	nx_live_UpdateChannelInformObject(NxUiLiveContents_t *pstContents, HINT32 keyId)
{
	ONDK_GWM_App_t		pGwmApp = NULL;

	switch(keyId)
	{
	case KEY_ARROWDOWN:
	case KEY_ARROWUP:
		pGwmApp= ONDK_GWM_APP_Get((ONDK_GWM_Callback_t)NX_PRISM_CHANNELLIST_Proc);
		if (pGwmApp == NULL)
		{
			ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW,"NX_PRISM_CHANNELLIST_Proc", APP_BACKGROUND_OSD_PRIORITY,
									NX_PRISM_CHANNELLIST_Proc, HANDLE_NULL,
									pstContents->currentChannelIndex, pstContents->nChannelListNum, 0);
		}
		return ONDK_MESSAGE_BREAK;

	case KEY_OK:
		{
			#if defined(CONFIG_OP_MBC)
				pGwmApp= ONDK_GWM_APP_Get((ONDK_GWM_Callback_t)NX_PRISM_CHANNELLIST_Proc);
				if (pGwmApp == NULL)
				{
					ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW,"NX_PRISM_CHANNELLIST_Proc", APP_BACKGROUND_OSD_PRIORITY,
											NX_PRISM_CHANNELLIST_Proc, HANDLE_NULL,
											pstContents->currentChannelIndex, pstContents->nChannelListNum, 0);
				}
				return ONDK_MESSAGE_BREAK;
			#else
				pGwmApp= ONDK_GWM_APP_Get((ONDK_GWM_Callback_t)NX_PRISM_INFOHUB_Proc);
				if (pGwmApp == NULL)
				{
					ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW,"NX_PRISM_INFOHUB_Proc", APP_BACKGROUND_OSD_PRIORITY,
											NX_PRISM_INFOHUB_Proc, HANDLE_NULL,
											pstContents->currentChannelIndex, pstContents->nChannelListNum, 0);
				}
				return ONDK_MESSAGE_BREAK;
			#endif
			#if 0
				NX_MetadataSearch_t *pstMetaSearch = NULL;
				HxLOG_Error("\n\nTEST EPG SEARCH \n\n\n");
				NX_APP_Debug("\n\n[%s.%d] search uid list : %d, %d, %d ,%d ,%d %d \n\n\n", __FUNCTION__,__LINE__,
					pstContents->pstChannelList[pstContents->currentChannelIndex].svcUid,
					pstContents->pstChannelList[pstContents->currentChannelIndex+1].svcUid,
					pstContents->pstChannelList[pstContents->currentChannelIndex+2].svcUid,
					pstContents->pstChannelList[pstContents->currentChannelIndex+3].svcUid,
					pstContents->pstChannelList[pstContents->currentChannelIndex+4].svcUid,
					pstContents->pstChannelList[pstContents->currentChannelIndex+5].svcUid);


				pstMetaSearch = NX_METADATA_SEARCH_New(eNX_META_SEARCH_TARGET_SCHEDULED);

				NX_METADATA_SEARCH_AddChannelConstraint(pstMetaSearch, &pstContents->pstChannelList[pstContents->currentChannelIndex]);
				NX_METADATA_SEARCH_AddChannelConstraint(pstMetaSearch, &pstContents->pstChannelList[pstContents->currentChannelIndex+1]);
				NX_METADATA_SEARCH_AddChannelConstraint(pstMetaSearch, &pstContents->pstChannelList[pstContents->currentChannelIndex+2]);
				NX_METADATA_SEARCH_AddChannelConstraint(pstMetaSearch, &pstContents->pstChannelList[pstContents->currentChannelIndex+3]);
				NX_METADATA_SEARCH_AddChannelConstraint(pstMetaSearch, &pstContents->pstChannelList[pstContents->currentChannelIndex+4]);
				NX_METADATA_SEARCH_AddChannelConstraint(pstMetaSearch, &pstContents->pstChannelList[pstContents->currentChannelIndex+5]);

				NX_METADATA_EPG_SearchEPG(pstMetaSearch);
			#endif
		}
		//return ONDK_MESSAGE_BREAK;

	default:
		break;
	}

	return ONDK_RET_OK;
}


static HERROR	nx_live_ActionRemoveLiveChannelList(NxUiLiveContents_t *pstContents)
{
	if (pstContents->pstChannelList != NULL)
	{
		HLIB_STD_MemFree(pstContents->pstChannelList);
		pstContents->pstChannelList = NULL;
	}

	pstContents->nChannelListNum = 0;

	return ERR_OK;
}


static HERROR	nx_live_ActionProcessTuneLocked(NxUiLiveContents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	OxMediaPlay_MediaType_e		eType = p2;

	switch (eType)
	{
		case eOxMP_MEDIATYPE_LIVE:
			nx_live_UpdateDolbyState(pstContents);
			break;
		default:
			break;
	}


	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	return ERR_OK;
}


static ONDK_Result_t	nx_live_ActionProcessStartService(NxUiLiveContents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HBOOL				bChangeGroupOnly = FALSE;
	HINT32				lChannelIndex = 0;
	HUINT32				ulSvcUid = (HUINT32)p1, ulViewId = 0;
	DxService_t 		*stFindService = NULL;
	NX_Channel_t		*stChannel = NULL, *pstCurPlayChannel = NULL;
	NX_ChListGroup_t	eChListGroup = (NX_ChListGroup_t)p2;
	OxMediaPlay_MediaType_e	eMediaType = eOxMP_MEDIATYPE_NONE;
	OxMediaPlay_PlayState_e	eMediaPlayState = eOxMP_PLAYSTATE_NONE;


	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! live pstContents NULL!!\n");
		return	ONDK_RET_INITFAIL;
	}

	NX_MEDIA_PLAY_GetMainViewId (&ulViewId);
	NX_MEDIA_PLAY_GetPlayType(ulViewId, &eMediaType);
	NX_MEDIA_PLAY_GetPlayState(ulViewId, &eMediaPlayState);

	NX_APP_Info("[Info] Start Live State ulViewId:(0x%x), eMediaType:(%d), eMediaPlayState:(%d)!! \n", ulViewId, eMediaType, eMediaPlayState);

	pstCurPlayChannel = NX_PRISM_LIVE_GetMasterChannel();
	if (pstCurPlayChannel == NULL)
	{
		NX_APP_Error("Error!!! live pstContents NULL!!\n");
		bChangeGroupOnly = FALSE;
	}
	else if (pstCurPlayChannel->svcUid == ulSvcUid)
	{
		if ((eMediaType == eOxMP_MEDIATYPE_LIVE) && (eMediaPlayState == eOxMP_PLAYSTATE_PLAYING))
		{
			bChangeGroupOnly = TRUE;
			NX_APP_Error("Start Service! But same service and differt group. so only change group!!\n");
		}
		else
		{
			bChangeGroupOnly = FALSE;
		}
	}
	else
	{
		bChangeGroupOnly = FALSE;
	}

	if (eChListGroup == eNxChListGroup_CURRENT)
	{
		lChannelIndex = nx_live_FindIndexBySvcUid(pstContents, ulSvcUid);
		if (lChannelIndex < 0)
		{
			NX_APP_Error("Error!!! nx_live_FindIndexBySvcUid() Not Found service UID :[0x%x] !!\n", ulSvcUid);
			return ONDK_RET_FAIL;
		}
		pstContents->ePrevGroupType = pstContents->eCurGroupType;

		stChannel = &pstContents->pstChannelList[lChannelIndex];

		pstContents->currentChannelIndex = lChannelIndex;
		nx_live_SetChannel(pstContents, stChannel);
	}
	else
	{
		stFindService = NX_CHANNEL_GROUP_FindServiceInGroupByServiceUid(eChListGroup, eNxFindSvcDir_Current, ulSvcUid);
		if (stFindService)
		{
			// save previous channel group
			pstContents->ePrevGroupType = pstContents->eCurGroupType;

			// first remove group channel list in context.
			nx_live_ActionRemoveLiveChannelList(pstContents);

			// Change Group
			pstContents->eCurGroupType = eChListGroup;
			NX_CHANNEL_SetLastServiceGroup(eChListGroup);

			// reload group channel list
			nx_live_ReloadChannelListByGroup(pstContents, 0, TRUE, p2, p3);

			// Find Service Index
			lChannelIndex = nx_live_FindIndexBySvcUid(pstContents, ulSvcUid);
			if (lChannelIndex < 0)
			{
				NX_APP_Error("Error!!! nx_live_FindIndexBySvcUid() Not Found service UID :[0x%x] !!\n", ulSvcUid);
				return ONDK_RET_FAIL;
			}

			stChannel = &pstContents->pstChannelList[lChannelIndex];
			pstContents->currentChannelIndex = lChannelIndex;

			if (bChangeGroupOnly == TRUE)
			{
				NX_PRISM_LIVE_SetPrevChannel(stChannel);
			}
			else
			{
				nx_live_SetChannel(pstContents, stChannel);
			}
		}
	}

	NX_COMMON_NOT_USED_PARAM(p3);

	return ONDK_RET_OK;
}

static ONDK_Result_t	nx_live_CheckIsOTA(NxUiLiveContents_t *pstContents, Handle_t hAct,HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3)
{

	HCHAR szSwupdateVersion[NX_TEXT_SIZE_256] = {'\0',};
	HCHAR szSwVersion[NX_TEXT_SIZE_256] = {'\0',};


	if (NX_PORT_SYSTEM_GetSWUpdateVersion(szSwupdateVersion, NX_TEXT_SIZE_256) == ERR_OK)
	{
		if (szSwupdateVersion != NULL)
		{
			NX_PORT_SYSTEM_GetSoftwareVersion(szSwVersion, NX_TEXT_SIZE_256);
		}
	}
	return ONDK_RET_OK;
}

static ONDK_Result_t	nx_live_UpdateScreenSaverStatus(NxUiLiveContents_t *pstContents, Handle_t hAct,HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3)
{
	GWM_APK_MSG_Type_e			eAPK_Type = (GWM_APK_MSG_Type_e)hAct;
	HUINT32						ulViewId  =	(ulParam1 >> 24) & 0x000F;
	OxMediaPlay_MediaType_e		eType	  =	(ulParam1 >> 16) & 0x000F;
	OxMediaPlay_Event_e			eEvent	  =	(ulParam1 & 0x00FF);

	if (eAPK_Type == GWM_APK_MEDIA)
	{
		switch(eEvent)
		{
		case eOxMP_EVENT_STARTED:
			if( (eType == eOxMP_MEDIATYPE_PVRPLAYBACK) ||
				(eType == eOxMP_MEDIATYPE_TSRPLAYBACK) ||
				(eType == eOxMP_MEDIATYPE_MEDIAPLAY) )
			{
				/*
				 * author : hsseo
				 * note : MainAction이 Playback중 ScreenSaver 시나리오(PB는 without ScreenSaver) 적용.
				 */
				HERROR hErr;
				HBOOL					bNeedRadioBG = FALSE;
				hErr = NX_MEDIA_PLAY_GetNeedRadioBg(ulViewId, &bNeedRadioBG);
				if( hErr == ERR_OK )
				{
					if (TRUE == bNeedRadioBG)
						pstContents->eScreenSaverMode = eNxScreenSaverMode_RADIO;
					else
						pstContents->eScreenSaverMode = eNxScreenSaverMode_LIVE;
				}
				else
				{
					HxLOG_Error("can't call to func. 'APK_MEDIA_PLAY_GetNeedRadioBg()' @ret (%d) ", hErr);
				}

				/*without ScreenSaver*/
				nx_live_SetRunScreenSaver(pstContents, FALSE);
			}
			break;

		case eOxMP_EVENT_LIVE_ViewStateChanged:
			{
				OxMediaPlay_LiveViewState_e eViewState = ulParam2;
				if (eViewState != eOxMP_LiveViewState_OK)
					nx_live_SetRunScreenSaver(pstContents, TRUE);
				else
					nx_live_SetRunScreenSaver(pstContents, FALSE);
			}
			break;

		case eOxMP_EVENT_LIVE_LockStateChanged:
			{
				OxMediaPlay_LiveLockState_e	eLockState = ulParam2;
				if (eLockState != eOxMP_LiveLockState_OK)
					nx_live_SetRunScreenSaver(pstContents, TRUE);
				else
					nx_live_SetRunScreenSaver(pstContents, FALSE);
			}
			break;

		default:
			break;
		}
	}

	return ONDK_RET_OK;
}

static void nx_live_SetRunScreenSaver(NxUiLiveContents_t *pstContents, HBOOL bRunScreenSaver)
{
	#if defined(CONFIG_APUI_SCREEN_SAVER)
	ONDK_Result_t	retTimer = ONDK_RET_FAIL;
	HUINT32			ulPeriodTime = 0;
	if (bRunScreenSaver)
	{
		if(pstContents->eScreenSaverMode == eNxScreenSaverMode_LIVE)
		{
			ulPeriodTime = NX_PRISM_LIVE_SCREENSAVER_TIMER_LIVE;
		}
		else if (pstContents->eScreenSaverMode == eNxScreenSaverMode_RADIO)
		{
			ulPeriodTime = NX_PRISM_LIVE_SCREENSAVER_TIMER_MEDIA;
		}
		else if (pstContents->eScreenSaverMode == eNxScreenSaverMode_MUSIC)
		{
			ulPeriodTime = NX_PRISM_LIVE_SCREENSAVER_TIMER_MEDIA;
		}
		// set timer
		retTimer = ONDK_GWM_SetTimer(NX_PRISM_LIVE_SCREENSAVER_TIMER_ID, ulPeriodTime*1000);
		if (retTimer == GWM_RET_LINK_CONFLICT)	// 이미 등록되어 종료되지 않은거라면..
		{
			ONDK_GWM_ResetTimer(NX_PRISM_LIVE_SCREENSAVER_TIMER_ID);
		}
		pstContents->bRunScreenSaverTimer = TRUE;
	}
	else
	{
		if(pstContents->eScreenSaverMode == eNxScreenSaverMode_RADIO)
		{
			ulPeriodTime = NX_PRISM_LIVE_SCREENSAVER_TIMER_MEDIA;
			// set timer
			retTimer = ONDK_GWM_SetTimer(NX_PRISM_LIVE_SCREENSAVER_TIMER_ID, ulPeriodTime*1000);
			if (retTimer == GWM_RET_LINK_CONFLICT)	// 이미 등록되어 종료되지 않은거라면..
			{
				ONDK_GWM_ResetTimer(NX_PRISM_LIVE_SCREENSAVER_TIMER_ID);
			}
			pstContents->bRunScreenSaverTimer = TRUE;
		}
		else
		{
			pstContents->bRunScreenSaverTimer = FALSE;
			ONDK_GWM_KillTimer(NX_PRISM_LIVE_SCREENSAVER_TIMER_ID); //kill ScreenSaver timer
		}
	}
	#endif
}

static ONDK_Result_t	nx_live_APKMessage(NxUiLiveContents_t *pstContents, Handle_t hAct,HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3)
{
	ONDK_Result_t				ret		  = ONDK_RET_OK;
	GWM_APK_MSG_Type_e			eAPK_Type = (GWM_APK_MSG_Type_e)hAct;
	HUINT32						ulViewId  =	(ulParam1 >> 24) & 0x000F;
	OxMediaPlay_MediaType_e		eType	  =	(ulParam1 >> 16) & 0x000F;
	OxMediaPlay_Event_e			eEvent	  =	(ulParam1 & 0x00FF);

	if (eAPK_Type == GWM_APK_MEDIA)
	{

		switch(eEvent)
		{
		case eOxMP_EVENT_STARTED:
			HxLOG_Debug(" ++++++++++ Received eOxMP_EVENT_STARTED  In PRISM LIVE ++++++++++++++\n");
			nx_live_UpdateScreenSaverStatus(pstContents, hAct, ulParam1, ulParam2, ulParam3);
			pstContents->eLastLiveEventStatus[ulViewId] = (HUINT32)eEvent;
			break;
		case eOxMP_EVENT_COMPONENT_AUDIO_CHANGED:
			HxLOG_Debug(" ++++++++++ Received eOxMP_EVENT_COMPONENT_AUDIO_CHANGED in PRISM LIVE ++++++++++++++\n");
			pstContents->eLastLiveEventStatus[ulViewId] = (HUINT32)eEvent;
			break;
		case eOxMP_EVENT_LIVE_SvcChanged:
			HxLOG_Debug(" ++++++++++ Received eOxMP_EVENT_LIVE_SvcChanged in PRISM LIVE ++++++++++++++\n");
#if defined(CONFIG_MW_TELETEXT)
			NX_PRISM_Teletext_StopService();
#endif
			pstContents->eLastLiveEventStatus[ulViewId] = (HUINT32)eEvent;
			pstContents->eLastLiveViewStatus[ulViewId] = (HUINT32)eOxMP_LiveViewState_NONE;
			pstContents->eLastLiveLockStatus[ulViewId] = (HUINT32)eOxMP_LiveLockState_NONE;
			ONDK_GWM_PostMessage(NULL, MSG_APP_SYSTEM_SVC_CHANGED, 0, 0, 0, 0);
			break;
		case eOxMP_EVENT_SI_EIT_CHANGED:
			pstContents->eLastLiveEventStatus[ulViewId] = (HUINT32)eEvent;
			break;
		case eOxMP_EVENT_SI_RCT_CHANGED:
			pstContents->eLastLiveEventStatus[ulViewId] = (HUINT32)eEvent;
			break;
		case eOxMP_EVENT_LIVE_EventRelay:
			pstContents->eLastLiveEventStatus[ulViewId] = (HUINT32)eEvent;
			break;
		case eOxMP_EVENT_LIVE_Ews:
			pstContents->eLastLiveEventStatus[ulViewId] = (HUINT32)eEvent;
			break;
		case eOxMP_EVENT_PinCtrlChanged:
			pstContents->eLastLiveEventStatus[ulViewId] = (HUINT32)eEvent;
			ONDK_GWM_SendMessage(NULL, MSG_APP_CHANNEL_PINCTRLCHANGED, (HINT32)NULL, (HINT32)eOxMP_EVENT_PinCtrlChanged, ulParam2, 0);		// maturity rating check
			break;
		case eOxMP_EVENT_LIVE_PinCtrlChangedByBCAS:
			pstContents->eLastLiveEventStatus[ulViewId] = (HUINT32)eEvent;
			break;
		case eOxMP_EVENT_TUNE_Locked:
			HxLOG_Debug(" ++++++++++ Received eOxMP_EVENT_TUNE_Locked in PRISM LIVE ++++++++++++++\n");
			pstContents->eLastLiveEventStatus[ulViewId] = (HUINT32)eEvent;
			ONDK_GWM_SendMessage(NULL, MSG_APP_CHANNEL_TUNE_LOCKED, (HINT32)NULL, ulViewId, eType, 0);
			break;
		case eOxMP_EVENT_TUNE_NoSignal:
			HxLOG_Debug(" ++++++++++ Received eOxMP_EVENT_TUNE_NoSignal in PRISM LIVE ++++++++++++++\n");
			pstContents->eLastLiveEventStatus[ulViewId] = (HUINT32)eEvent;
			ONDK_GWM_SendMessage(NULL, MSG_APP_CHANNEL_TUNE_NOSIGNAL, (HINT32)NULL, ulViewId, eType, 0);
			break;
		case eOxMP_EVENT_TUNE_AntennaNok:
			HxLOG_Debug(" ++++++++++ Received eOxMP_EVENT_TUNE_AntennaNok in PRISM LIVE ++++++++++++++\n");
			pstContents->eLastLiveEventStatus[ulViewId] = (HUINT32)eEvent;
			break;
		case eOxMP_EVENT_LIVE_ViewStateChanged:
			HxLOG_Debug(" ++++++++++ Received eOxMP_EVENT_LIVE_ViewStateChanged in PRISM LIVE ++++++++++++++ %d %d\n", ulParam2, ulParam3);
#if defined(CONFIG_MW_TELETEXT)
			if ((OxMediaPlay_LiveViewState_e)ulParam2 != eOxMP_LiveViewState_OK)
			{
				NX_PRISM_Teletext_StopService();
			}
			else
			{
				NX_PRISM_Teletext_StartService();
			}
#endif
			pstContents->eLastLiveEventStatus[ulViewId] = (HUINT32)eEvent;
			pstContents->eLastLiveViewStatus[ulViewId] = (HUINT32)ulParam2;
			nx_live_UpdateScreenSaverStatus(pstContents, hAct, ulParam1, ulParam2, ulParam3);
			ONDK_GWM_SendMessage(NULL, MSG_APP_CHANNEL_LIVE_VIEWSTATECHANGED, (HINT32)NULL, ulViewId, eType, ulParam2);
			break;
		case eOxMP_EVENT_LIVE_LockStateChanged:
			HxLOG_Debug(" ++++++++++ Received eOxMP_EVENT_LIVE_LockStateChanged in PRISM LIVE ++++++++++++++ %d \n", ulParam2);
			pstContents->eLastLiveEventStatus[ulViewId] = (HUINT32)eEvent;
			pstContents->eLastLiveLockStatus[ulViewId] = (HUINT32)ulParam2;
			nx_live_UpdateScreenSaverStatus(pstContents, hAct, ulParam1, ulParam2, ulParam3);
			ONDK_GWM_SendMessage(NULL, MSG_APP_CHANNEL_LIVE_LOCKSTATECHANGED, (HINT32)NULL, ulViewId, eType, ulParam2);
			break;
		case eOxMP_EVENT_LIVE_SvcStopped:
			HxLOG_Debug(" ++++++++++ Received eOxMP_EVENT_LIVE_SvcStopped in PRISM LIVE ++++++++++++++\n");
#if defined(CONFIG_MW_TELETEXT)
			NX_PRISM_Teletext_StopService();
#endif
			pstContents->eLastLiveEventStatus[ulViewId] = (HUINT32)eEvent;
			ONDK_GWM_PostMessage(NULL, MSG_APP_SYSTEM_SVC_STOPPED, 0, 0, 0, 0);
			break;

		case eOxMP_EVENT_RADIOBG_CHANGED:
			HxLOG_Debug(" ++++++++++ Received eOxMP_EVENT_RADIOBG_CHANGED in PRISM LIVE ++++++++++++++\n");
			nx_live_UpdateRadioStill(pstContents);
			break;
		default:
			break;
		}
	}

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, ulParam1, ulParam2, ulParam3);
	return ret;
}


static ONDK_Result_t	nx_live_MsgGwmCreate(NxUiLiveContents_t *pstContents, Handle_t hAct, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! live pstContents NULL!!\n");
		return	ONDK_RET_INITFAIL;
	}

	HxSTD_MemSet(pstContents, 0, sizeof(NxUiLiveContents_t));

	nx_live_Initilization(pstContents, hAct, p1, p2, p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t nx_live_MsgGwmDestroy(NxUiLiveContents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HxLOG_Trace();

	NX_PRISM_RADIO_CloseStill();
	ONDK_GWM_APP_Destroy(NX_PRISM_NO_SERVICE_MGR_Proc);

	ONDK_GWM_KillTimer(NX_PRISM_LIVE_HYSTERESIS_TIMER_ID); // clear hysteesis timer
	ONDK_GWM_KillTimer(NX_PRISM_LIVE_FCC_TIMER_ID); // FCC timer

	if (pstContents->pstChannelList != NULL)
	{
		HLIB_STD_MemFree(pstContents->pstChannelList);
		pstContents->pstChannelList = NULL;
	}
	ONDK_WINDOW_Clear(ONDK_GWM_GetWindow(ONDK_UPPER_WINDOW));
	ONDK_WINDOW_Update(ONDK_GWM_GetWindow(ONDK_UPPER_WINDOW),NULL);

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_live_MsgGwmHapiMsg(Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	#if defined(CONFIG_APUI_SCREEN_SAVER)
	HAPIMSG_e	eMsg = (HAPIMSG_e)handle;
	NxUiLiveContents_t		*pstContents = nx_live_GetContents();

	HxLOG_Trace();

	switch(eMsg)
	{
	case eHMSG_INPUT_NOTIFY:

		if (pstContents->bRunScreenSaverTimer)
			ONDK_GWM_ResetTimer(NX_PRISM_LIVE_SCREENSAVER_TIMER_ID);
		break;

	default:
		break;
	}
	#endif

	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_MESSAGE_PASS;
}

static ONDK_Result_t	nx_live_MsgGwmKeyDown(NxUiLiveContents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32			lKeyId = p1;
	ONDK_GWM_App_t	pGwmApp = NULL;

	NX_PRISM_UTIL_KeyConv_UiCommon(&lKeyId);

	switch (lKeyId)
	{

	// Rec Keys
	case KEY_RECORD:
	case KEY_STOP:
		return nx_live_RecordKey(pstContents, lKeyId);

	case KEY_CH_PLUS:
		(void)nx_live_ChannelUp(pstContents);
		return ONDK_MESSAGE_BREAK;

	case KEY_CH_MINUS:
		(void)nx_live_ChannelDown(pstContents);
		return ONDK_MESSAGE_BREAK;

	case KEY_TVRADIO:
		if(nx_live_ChannelTVRadio(pstContents) == ERR_OK)
		{
			return ONDK_MESSAGE_BREAK;
		}
		return ONDK_MESSAGE_PASS;

	case KEY_BACK:
		(void)nx_live_PrevChannel(pstContents);
		return ONDK_MESSAGE_BREAK;

	case KEY_ARROWUP:
	case KEY_ARROWDOWN:
	case KEY_OK:
		return nx_live_UpdateChannelInformObject(pstContents, lKeyId);

	case KEY_GUIDE:
		pGwmApp = ONDK_GWM_APP_Get((ONDK_GWM_Callback_t)NX_PRISM_GUIDE_Proc);
		if (pGwmApp == NULL)
		{
			ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW,"NX_PRISM_GUIDE_Proc", (APP_BACKGROUND_OSD_PRIORITY | UISTATE_EPG), NX_PRISM_GUIDE_Proc,
									HANDLE_NULL, pstContents->currentChannelIndex, pstContents->nChannelListNum, 0);
		}
		return ONDK_MESSAGE_BREAK;

	case KEY_0:
	case KEY_1:
	case KEY_2:
	case KEY_3:
	case KEY_4:
	case KEY_5:
	case KEY_6:
	case KEY_7:
	case KEY_8:
	case KEY_9:
		pGwmApp = ONDK_GWM_APP_Get((ONDK_GWM_Callback_t)NX_PRISM_DCN_Proc);
		if (pGwmApp == NULL)
		{
			NX_PRISM_INFOHUB_Close();
			NX_PRISM_DOLBY_CloseDolbyLogo();

			ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW,"NX_PRISM_DCN_Proc", APP_BACKGROUND_OSD_PRIORITY,	NX_PRISM_DCN_Proc,
									(HINT32)ONDK_GWM_APP_Get(0), pstContents->eCurGroupType, 0, lKeyId);
		}
		return ONDK_MESSAGE_BREAK;

	case KEY_TEXT:
#if defined(CONFIG_MW_TELETEXT)
		ONDK_GWM_PostMessage(NULL, MSG_APP_START_TELETEXT, 0, 1, 0, 0);
#endif
		return ONDK_MESSAGE_BREAK;

	case KEY_STANDBY:
		NX_AV_StopChannel();
		return ONDK_MESSAGE_PASS;

#if defined(DEMO_MEMINFO_MYKIM)
	case KEY_RED:
		if(ONDK_GWM_APP_Get((ONDK_GWM_Callback_t)NX_PRISM_DEMO_MEMINFO_Proc) ==NULL)
		{
			ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW,"NX_PRISM_DEMO_MEMINFO_Proc", APP_BACKGROUND_OSD_PRIORITY,
									NX_PRISM_DEMO_MEMINFO_Proc, HANDLE_NULL, 0, 0, 0);
		}
		else
		{
		}

		return ONDK_MESSAGE_BREAK;
#endif

#if 0
	case KEY_RED:
		ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW,"NX_PRISM_SMARTSEARCH_Proc", APP_DEFAULT_POPUP_PRIORITY, NX_PRISM_SMARTSEARCH_Proc, HANDLE_NULL, 0, 0, 0);

		return ONDK_MESSAGE_BREAK;
#endif

	case KEY_SMART_SEARCH:
#if defined(CONFIG_APUI_SET_SMARTSEARCH_UI_DISPLAY)
		ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW,"NX_PRISM_SMARTSEARCH_Proc", APP_DEFAULT_POPUP_PRIORITY, NX_PRISM_SMARTSEARCH_Proc, HANDLE_NULL, pstContents->currentChannelIndex, 0, 0);
#endif
		return ONDK_MESSAGE_BREAK;

	default:
		return ONDK_MESSAGE_PASS;
	}

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);
}


static ONDK_Result_t	nx_live_MsgGwmTimer(NxUiLiveContents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR	hErr = ERR_FAIL;
	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	if(pstContents == NULL)
	{
		HxLOG_Error("pstContents is NULL!\n");
		return ERR_FAIL;
	}

	switch(p1)
	{
		case NX_PRISM_LIVE_HYSTERESIS_TIMER_ID: // hysteesis timer
			ONDK_GWM_KillTimer(NX_PRISM_LIVE_HYSTERESIS_TIMER_ID);
			HxLOG_Warning("live Set Channel delayed : [ %04d: %08x]\n", pstContents->stHystChannel.lcn, pstContents->stHystChannel.svcUid);

			NX_PRISM_LIVE_SetPrevChannel(&pstContents->stHystChannel);

			hErr = NX_CHANNEL_SetChannel(&pstContents->stHystChannel);
			break;

		case NX_PRISM_LIVE_FCC_TIMER_ID:
			ONDK_GWM_KillTimer(NX_PRISM_LIVE_FCC_TIMER_ID);	// FCC timer
			nx_live_SetFastChannelChange(pstContents);
			break;
		#if defined(CONFIG_APUI_SCREEN_SAVER)
		case NX_PRISM_LIVE_SCREENSAVER_TIMER_ID:
			HxLOG_Debug("\nScreen Timer fired.\n");
			if(ONDK_GWM_APP_Get((ONDK_GWM_Callback_t)NX_PRISM_SCREENSAVER_Proc) ==NULL)
			{
				ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, "NX_PRISM_SCREENSAVER_Proc", APP_INDEPENDENT_OSD_GRP_PRIORITY, NX_PRISM_SCREENSAVER_Proc, HANDLE_NULL, p1, p2, p3);
			}
			break;
		#endif
		default:
			break;
	}

	return GWM_RET_OK;
}

/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/
ONDK_Result_t		NX_PRISM_LIVE_Proc(HINT32 lMessage, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t			lRet = ONDK_RET_OK;
	NxUiLiveContents_t		*pstContents = nx_live_GetContents();
	HxLOG_Print("Message(%d) : %s, handle: %p p1(0x%x) p2(0x%x) p3(0x%x) \n", lMessage, ONDK_GWM_GwmMsg2String(lMessage), p1, p2, p3);

	switch(lMessage)
	{
		case MSG_GWM_CREATE:
			HxLOG_Debug("\n MSG_GWM_CREATE IN NX_UI_LIVE_Proc\n\n");
			nx_live_MsgGwmCreate(pstContents, hHandle, p1, p2, p3);
			nx_live_CheckNoService();
			break;

		case MSG_GWM_TIMER:
			lRet = nx_live_MsgGwmTimer(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_APK_MESSAGE:
			lRet = nx_live_APKMessage(pstContents, hHandle, p1, p2, p3);
			break;

		case MSG_GWM_HAPI_MESSAGE:
			lRet = nx_live_MsgGwmHapiMsg(hHandle, p1, p2, p3);
			break;

		case MSG_GWM_CLICKED :
			break;

		case MSG_GWM_KEYDOWN:
			lRet = nx_live_MsgGwmKeyDown(pstContents, p1, p2, p3);
			break;
		case MSG_GWM_KEYUP:
			break;

		case MSG_CORE_SERVICE_PF_CHANGED_PROGRAMMES:
			nx_live_UpdatePf(pstContents, p1);
			break;

		case MSG_CORE_SERVICE_EVT_RELOAD_CHANNELLIST_ALL:
			nx_live_ReloadChannelListByGroup(pstContents, hHandle, p1, p2, p3);
			//NX_PRISM_LIVE_ServiceStartByLastService();
			nx_live_CheckNoService();
			break;

		case MSG_APP_SYSTEM_UPDATE_SERVICE:
			nx_live_ReloadChannelListByGroup(pstContents, hHandle, p1, p2, p3);
			nx_live_CheckNoService();
			break;

		case MSG_APP_SYSTEM_DELETE_SERVICE_ALL:
			nx_live_ActionRemoveLiveChannelList(pstContents);
			nx_live_CheckNoService();
			break;

		case MSG_APP_CHANNEL_TUNE_LOCKED:
			nx_live_ActionProcessTuneLocked(pstContents, p1, p2, p3);
			break;

		case MSG_APP_SYSTEM_SVC_CHANGED:
			nx_live_tryFastChannelChange();
			break;

		case MSG_APP_SYSTEM_SVC_STOPPED:
			break;

		case MSG_APP_SYSTEM_START_SVC:
			lRet = nx_live_ActionProcessStartService(pstContents, p1, p2, p3);
			break;

		case MGS_APP_SYSTEM_START_LASTSVC:
			lRet = nx_live_TrySetLastChannel(pstContents);
			break;

		case MSG_APP_CHECK_FIRST_ACTION:
			lRet = nx_live_CheckIsOTA(pstContents, hHandle, p1, p2, p3);
			break;

		case MSG_APP_DCN_INPUT:
			lRet = nx_live_ChannelDCNSet(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_DESTROY:
			lRet = nx_live_MsgGwmDestroy(pstContents, p1, p2, p3);
			break;
	}

	if(lRet != ONDK_RET_OK)
	{
		return lRet;
	}

	return ONDK_GWM_ProcessMessageDefault(lMessage, hHandle, p1, p2, p3);
}

NX_Channel_t*	NX_PRISM_LIVE_GetLiveChannels(void)
{
	NX_Channel_t *retList = NULL;
	NxUiLiveContents_t		*pstContents = nx_live_GetContents();

	if (pstContents == NULL)
	{
		HxLOG_Debug(" live proc contents is NULL!\n");
	}
	else
	{
		retList = pstContents->pstChannelList;
	}

	return retList;
}


NX_Channel_t*	NX_PRISM_LIVE_GetMasterChannel(void)
{
	NX_Channel_t *retList = NULL;
	NxUiLiveContents_t		*pstContents = nx_live_GetContents();

	if (pstContents == NULL)
	{
		HxLOG_Debug(" live proc contents is NULL!\n");
	}
	else
	{
		if (pstContents->nChannelListNum > 0)
		{
		retList = &pstContents->pstChannelList[pstContents->currentChannelIndex];
	}
	}
	return retList;
}

HERROR	NX_PRISM_LIVE_SetPrevChannel(NX_Channel_t *channel)
{
	NxUiLiveContents_t		*pstContents = nx_live_GetContents();
	DxService_t 			*stLastService = NULL, *stFindService = NULL;

	if (channel == NULL)
	{
		HxLOG_Error("channel is NULL\n");
		return ERR_FAIL;
	}

	stLastService = NX_CHANNEL_GetLastService();
	if (stLastService == NULL)
	{
		HxLOG_Error("Error!!! channel is NULL!\n");
		return ERR_FAIL;
	}

	if(pstContents->prevChannel.svcUid == stLastService->uid)
	{
		HxLOG_Error("Error!!! previous channel == LastChannel !\n");
		return ERR_FAIL;
	}

	stFindService = NX_CHANNEL_GROUP_FindServiceInGroupByServiceUid(pstContents->ePrevGroupType, eNxFindSvcDir_Current, stLastService->uid);
	if (stFindService == NULL)
	{
		HxLOG_Error("Error!!! channel group error !\n");
		return ERR_FAIL;
	}

	pstContents->prevChannel.svcUid = stLastService->uid;
	pstContents->prevChannel.lcn = stLastService->lcn;
	return ERR_OK;
}


HERROR	NX_PRISM_LIVE_SetChannel(NX_Channel_t *channel, HINT32 curChannelIndex)
{
	NxUiLiveContents_t		*pstContents = nx_live_GetContents();
	if (pstContents == NULL || channel == NULL)
	{
		HxLOG_Debug(" live contents(0x%x) is NULL or channel(0x%x) is NULL!\n", pstContents, channel);
		return ERR_FAIL;
	}
	pstContents->ePrevGroupType = pstContents->eCurGroupType;

	pstContents->currentChannelIndex = curChannelIndex;
	nx_live_SetChannel(pstContents, channel);

	return	ERR_OK;
}


HERROR		NX_PRISM_LIVE_ServiceStartMainView(HBOOL bSendMsg, HUINT32 ulSvcUid, NX_ChListGroup_t eChListGroup, HUINT32 ulExtraFilter)
{
	ONDK_Result_t	eResult = ONDK_RET_FAIL;

	if (bSendMsg == TRUE)
	{
		eResult = ONDK_GWM_SendMessage(NULL, MSG_APP_SYSTEM_START_SVC, 0, ulSvcUid, eChListGroup, ulExtraFilter);
	}
	else
	{
		eResult = ONDK_GWM_PostMessage(NULL, MSG_APP_SYSTEM_START_SVC, 0, ulSvcUid, eChListGroup, ulExtraFilter);
	}

	if (eResult != ONDK_RET_OK)
	{
		NX_APP_Error(" Error!!! NX_PRISM_LIVE_ServiceStartMainView() - bSendMsg(%d) Fail!!\n", bSendMsg);
	}

	return ERR_OK;
}


HERROR		NX_PRISM_LIVE_ServiceStartByLastService(void)
{
	HERROR			hError = ERR_FAIL;
	HINT32			lChlistGroup = (HINT32)eNxChListGroup_TV;
	DxService_t 	*stLastService = NULL, *stFindService = NULL;

	hError = NX_CHANNEL_GetLastServiceGroup(&lChlistGroup);
	if (hError != ERR_OK)
	{
		lChlistGroup = (HINT32)eNxChListGroup_TV;
		NX_CHANNEL_SetLastServiceGroup(lChlistGroup);
	}

	stLastService = NX_CHANNEL_GetLastService();
	if (stLastService == NULL)
	{
		stFindService = NX_CHANNEL_GROUP_FindServiceInGroupByServiceUid(lChlistGroup, eNxFindSvcDir_First, 0);
	}
	else
	{
		stFindService = NX_CHANNEL_GROUP_FindServiceInGroupByServiceUid(lChlistGroup, eNxFindSvcDir_Current, stLastService->uid);
	}

	if (stFindService)
	{
		NX_PRISM_LIVE_ServiceStartMainView(TRUE, stFindService->uid, lChlistGroup, 0);
	}

	return ERR_OK;
}


NX_ChListGroup_t	NX_PRISM_LIVE_GetCurrentChGroupType(void)
{
	HERROR		hError = ERR_FAIL;
	HINT32	lGroupType = eNxChListGroup_TV;
	NxUiLiveContents_t		*pstContents = nx_live_GetContents();

	if (pstContents == NULL)
	{
		hError = NX_CHANNEL_GetLastServiceGroup(&lGroupType);
		if (hError != ERR_OK)
		{
			lGroupType = (HINT32)eNxChListGroup_TV;
		}

		return (NX_ChListGroup_t)lGroupType;
	}

	return pstContents->eCurGroupType;
}

HERROR	NX_PRISM_LIVE_GetCurChannelIndex(HINT32 *nChannelIndex)
{
	NxUiLiveContents_t		*pstContents = nx_live_GetContents();
	if (pstContents == NULL)
	{
		return ERR_FAIL;
	}
	*nChannelIndex = pstContents->currentChannelIndex;
	return ERR_OK;
}

HERROR 	NX_PRISM_LIVE_GetChannelListNum(HINT32 *nChannelListNum)
{
	NxUiLiveContents_t		*pstContents = nx_live_GetContents();
	if (pstContents == NULL)
	{
		return ERR_FAIL;
	}
	*nChannelListNum = pstContents->nChannelListNum;
	return ERR_OK;
}

HERROR		NX_PRISM_LIVE_GetCurChannelAudioCodec(DxAudioCodec_e *peAudCodec)
{
	HERROR			hError = ERR_FAIL;
	HINT32			lAudioTrackIndex = 0;
	HUINT32			ulViewId = 0;
	OxMediaPlay_MediaType_e		eMediaType = eOxMP_MEDIATYPE_NONE;
	DxAudioCodec_e				eAudCodec = eDxAUDIO_CODEC_DOLBY_AC3;
	OxMediaPlay_Component_t  	*pstAudioComponent = NULL;


	NX_MEDIA_PLAY_GetMainViewId(&ulViewId);
	NX_MEDIA_PLAY_GetPlayType(ulViewId, &eMediaType);

	switch (eMediaType)
	{
		case eOxMP_MEDIATYPE_LIVE:
		case eOxMP_MEDIATYPE_PVRPLAYBACK:
		case eOxMP_MEDIATYPE_TSRPLAYBACK:
			eAudCodec = eDxAUDIO_CODEC_UNKNOWN;
			pstAudioComponent = (OxMediaPlay_Component_t*)NX_APP_Calloc(sizeof(OxMediaPlay_Component_t));
			hError = NX_MEDIA_PLAY_GetComponentIndex(ulViewId, eOxMP_COMPONENT_AUDIO, &lAudioTrackIndex, NULL);
			if ((hError == ERR_OK) && (lAudioTrackIndex >= 0))
			{
				HxSTD_MemSet (pstAudioComponent, 0x0, sizeof(OxMediaPlay_Component_t));
				hError = NX_MEDIA_PLAY_GetComponent(ulViewId, eOxMP_COMPONENT_AUDIO, lAudioTrackIndex, pstAudioComponent);
				if (hError == ERR_OK)
				{
					switch (pstAudioComponent->stAudio.eCodec)
					{
						case eDxAUDIO_CODEC_DOLBY_AC3:
						case eDxAUDIO_CODEC_DOLBY_AC3P:
							eAudCodec = pstAudioComponent->stAudio.eCodec;
							break;
						default:
							NX_APP_Info("Other Audio Codec : (%d)\n", pstAudioComponent->stAudio.eCodec);
							eAudCodec = pstAudioComponent->stAudio.eCodec;
							break;
					}
				}
				else
				{
					NX_APP_Error("[%s:%d] Error!!! NX_MEDIA_PLAY_GetComponent()-eOxMP_COMPONENT_AUDIO ulViewId:(%d), lAudioTrackIndex:(%d)\n", __FUNCTION__, __HxLINE__, ulViewId, lAudioTrackIndex);
				}
			}
			else
			{
				NX_APP_Error("[%s:%d] Error!!! NX_MEDIA_PLAY_GetComponentIndex()-eOxMP_COMPONENT_AUDIO ulViewId:(%d)\n", __FUNCTION__, __HxLINE__, ulViewId);
			}

			if (pstAudioComponent)
			{
				NX_APP_Free(pstAudioComponent);
				pstAudioComponent = NULL;
			}
			break;

		case eOxMP_MEDIATYPE_MEDIAPLAY:
			eAudCodec = eDxAUDIO_CODEC_UNKNOWN;
			break;

		default:
			eAudCodec = eDxAUDIO_CODEC_UNKNOWN;
			break;
	}

	*peAudCodec = eAudCodec;

	return ERR_OK;
}


/******************************************************************************/
#define _________End_Of_File____________________________________________________
/******************************************************************************/
