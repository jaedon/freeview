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
 * @file	  		nx_core_channel.c
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

#include <nx_core_system.h>
#include <nx_core_channel.h>
#include <nx_core_metadata_pf.h>

#include "nx_core_channel_local.h"
/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/


/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/


/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/
static NX_ChannelManager_t *s_firstInstance = NULL;		// 이걸 어케하나..

/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/

static NX_ChannelManager_t * nx_channel_GetManager (NX_ChannelManagerHandle_t managerHandle);
static void		nx_channel_EventListener (HINT32 event, const HINT32 *args, void *userdata);
static HBOOL	nx_channel_BaseUpdateChecker (const HCHAR *typeName, const void *_old, const void *_new, HBOOL *updated);
static HINT32	nx_channel_CompareLogicalNumber(const void *arg1, const void *arg2);
static HINT32	nx_channel_CompareHINT32(const void *arg1, const void *arg2);
static HINT32	nx_channel_SortByLCN(const void *pvArg1, const void *pvArg2);
static HINT32 	nx_channel_SortByName(void *pvArg1, void *pvArg2);
static NX_ChannelManager_t *nx_channel_LoadList (NX_ChannelManager_t *manager, HBOOL force);
static char*	nx_channel_MakeCCID (char *buf, size_t n, int uid, int onid, int tsid, int sid);
static HBOOL	nx_channel_FindService(void *data, void *userdata);
static HUINT32	nx_channel_FindSvcUidByCcid(const char *ccid);
static HERROR	nx_channel_TuneTry(HUINT32 ulViewId, NX_Channel_t *channel);
static HERROR 	nx_channel_FCCTry(HUINT32 ulViewId, NX_Channel_t *channel);
static HERROR	nx_channel_bindChannel(NX_Channel_t *channel);
static HERROR	nx_channel_bindFCC(NX_Channel_t *channel);
static HERROR	nx_channel_StopChannel(void);
static HUINT32	nx_channel_GetSpecificInformation(void);
static HUINT32	nx_channel_ConvertNxChannelGroupToCLIBGroupType(NX_ChListGroup_t eChGroupType);
static HUINT32	nx_channel_ConvertNxChannelAlpabetGroupToCLIBAlphabetGroupFilter(NX_ChListGroup_t eChAlphabetGroupType);
static void		nx_channel_SetupDamaNotifier(void);
static void 	nx_channel_OnDamaServiceUpdated(const HCHAR *pszName, HUID hUid, const void *pvData, HUINT32 ulSize, void *pvUser);

static HxVector_t	*nx_channel_group_GetReferServiceList(HxVector_t *totalSvcList, NX_ChListGroup_t eChGroupType);
static HERROR	nx_channel_group_MakeNxChannelListItemByGroup(NX_ChListGroup_t eChGroupType, HxVector_t **ddstListItem, HINT32 *nListItemNum);
static HERROR 	nx_channel_group_GetGroupInfo(NX_ChListGroup_t eChGroupType, NX_GroupInfoList_t **ddstItem);




#if defined(CONFIG_DEBUG)
static void	nx_channel_Command_Init(void);
#endif

/**************************************************************************************************/
#define ______________Private_functions__________________________________________
/**************************************************************************************************/

static NX_ChannelManager_t * nx_channel_GetManager (NX_ChannelManagerHandle_t managerHandle)
{
    NX_ChannelManager_t	*manager = (NX_ChannelManager_t *)managerHandle;

    if (manager == NULL)
    {
        static NX_ChannelManager_t	s_manager;		// svc 저장용.

		HxSTD_MemSet(&s_manager, 0, sizeof(NX_ChannelManager_t));

        manager = &s_manager;
    }

    return nx_channel_LoadList(manager, FALSE/*NOT forced*/);
}

static void		nx_channel_EventListener (HINT32 event, const HINT32 *args, void *userdata)
{
	HxVector_t		*list = NULL;
	HxTREE_t		*uidMap = NULL;
	DxService_t		*service = NULL;
	NX_ChannelManager_t *manager;

	manager = (NX_ChannelManager_t *)userdata;

	list = NX_PORT_META_SVC_GetServiceList();

	/* Appkit에서 0, 1로 올려 준다. apk_meta_svc_OnResponse()*/
	switch (event)
	{
		case eNxCh_State_Update:	// Something change to channel (Add, Delete, Update)
		case eNxCh_State_Load:		// Not change
			HxLOG_Error("\n\n=====> nx_channel_EventListener() event[%d] !!!\n", event);
			ONDK_GWM_PostMessage(NULL, MSG_CORE_CHANNEL_EVT_STATE_UPDATED, 0, event, 0, 0);
			break;
		default:
			break;
	}

	if (list == NULL)
		return;

	uidMap = HLIB_TREE_New(nx_channel_CompareHINT32);
	if (uidMap == NULL)
	{
		HLIB_VECTOR_Delete(list);
		HxLOG_Error("Out of memory!!!\n");
		return ;
	}

	HxVECTOR_EACH(list, DxService_t *, service, {
	    HLIB_TREE_Insert(uidMap, (void *)service->uid, (void *)service);
	});

	{
		int i;
		int len = HLIB_VECTOR_Length(list);

		for ( i =0; i < len; i++)
		{
			DxService_t *data = (DxService_t*)HLIB_VECTOR_ItemAt(list,i);
			if (data)
			{
				HxLOG_Debug("[%s.%d] data: svcid(%d), name(%s), uid(%d), lcn(%d) \n",
					__FUNCTION__,__LINE__,
					data->svcid,
					data->name,
					data->uid,
					data->lcn);
			}
		}
	}

	if (manager->list)
	    HLIB_VECTOR_Delete(manager->list);
	if (manager->map)
	    HLIB_TREE_Destroy(manager->map);

	manager->list = list;
	manager->map  = uidMap;

	NX_CHANNEL_Update(manager->list);

	NX_COMMON_NOT_USED_PARAM(args);
}

static HBOOL nx_channel_BaseUpdateChecker (const HCHAR *typeName, const void *_old, const void *_new, HBOOL *updated)
{
	DxService_t *pOld = (DxService_t*)_old;
	DxService_t *pNew = (DxService_t*)_new;

	*updated = FALSE;

#define	INT_UPDATED(prop)	(pOld->prop != pNew->prop)
#define	MEM_UPDATED(prop,s)	(HxSTD_MemCmp(pOld->prop, pNew->prop, s) != 0)
#define STR_UPDATED(prop)	(HxSTD_StrCmp(pOld->prop, pNew->prop) != 0)

	if (pOld && pNew)	// updated
	{
		if (HxSTD_StrCmp(typeName, DxNAMEOF(DxService_t)) == 0)
		{
			if (   INT_UPDATED(uid)
				|| INT_UPDATED(tsuid)
				|| INT_UPDATED(prvuid)
				|| INT_UPDATED(antuid)
				|| MEM_UPDATED(grpuids, (sizeof(HINT32) * DxGROUP_LEN))
				|| MEM_UPDATED(bqtuids, (sizeof(HINT32) * DxBOUQUET_LEN))
				|| INT_UPDATED(svcid)
				|| INT_UPDATED(tsid)
				|| INT_UPDATED(onid)
				|| INT_UPDATED(lcn)
//				|| INT_UPDATED(locked)
				|| (STR_UPDATED(name) && pNew->renamed)
				|| INT_UPDATED(removed)
//				|| INT_UPDATED(audioPid)
//				|| INT_UPDATED(user.subtitle)
//				|| INT_UPDATED(user.subtitleTrack)
//				|| INT_UPDATED(user.audio)
//				|| INT_UPDATED(user.audioTrack)
//				|| INT_UPDATED(user.ratingPassedEventId)
			) {
				*updated = TRUE;

			HxLOG_Critical("CH DB Update Check [%d%d%d%d:%d%d%d%d:%d%d%d%d]\n",
								INT_UPDATED(uid)
								,INT_UPDATED(tsuid)
								,INT_UPDATED(prvuid)
								,INT_UPDATED(antuid)
								,MEM_UPDATED(grpuids, (sizeof(HINT32) * DxGROUP_LEN))
								,MEM_UPDATED(bqtuids, (sizeof(HINT32) * DxBOUQUET_LEN))
								,INT_UPDATED(svcid)
								,INT_UPDATED(tsid)
								,INT_UPDATED(onid)
								,INT_UPDATED(lcn)
				//				,INT_UPDATED(locked)
								,(STR_UPDATED(name) && pNew->renamed)
								,INT_UPDATED(removed)
				//				, INT_UPDATED(audioPid)
				//				,INT_UPDATED(user.subtitle)
				//				,INT_UPDATED(user.subtitleTrack)
				//				,INT_UPDATED(user.audio)
				//				,INT_UPDATED(user.audioTrack)
				//				,INT_UPDATED(user.ratingPassedEventId)
			);

			}
		}
	}
	else if (pOld)		// removed
	{
		if (HxSTD_StrCmp(typeName, DxNAMEOF(DxService_t)) == 0)
			*updated = TRUE;
	}
	else if (pNew)		// added
	{
		if (HxSTD_StrCmp(typeName, DxNAMEOF(DxService_t)) == 0)
			*updated = TRUE;
	}
	else
	{
		HxLOG_Critical("[%s] WTF!!!!!!\n", __FUNCTION__);
	}

#undef	INT_UPDATED
#undef	MEM_UPDATED
#undef	STR_UPDATED

	return TRUE;
}

static HINT32 nx_channel_CompareLogicalNumber(const void *arg1, const void *arg2)
{
	//STEP;
	const CListItem_t *lhs = *(const CListItem_t **)arg1;
	const CListItem_t *rhs = *(const CListItem_t **)arg2;

	return (lhs->nLogicalNo - rhs->nLogicalNo);
}

static HINT32 nx_channel_CompareHINT32 (const void *arg1, const void *arg2)
{
	return (HINT32)arg1 - (HINT32)arg2;
}

static HINT32 nx_channel_SortByLCN (const void *pvArg1, const void *pvArg2)
{
	const DxService_t *pstService1 = *(const DxService_t **)pvArg1;
	const DxService_t *pstService2 = *(const DxService_t **)pvArg2;

	return (pstService1->lcn - pstService2->lcn);
}

static HINT32 nx_channel_SortByName(void *pvArg1, void *pvArg2)
{
	DxService_t *pstSvc1 = (DxService_t *)pvArg1;
	DxService_t *pstSvc2 = (DxService_t *)pvArg2;

	if(NULL == pstSvc1) return 1;
	if(NULL == pstSvc2) return -1;

	return HxSTD_StrCmp(pstSvc2->name, pstSvc1->name);
}

static NX_ChannelManager_t *nx_channel_LoadList (NX_ChannelManager_t *manager, HBOOL force)
{
    HxVector_t		*list 	= NULL;
    HxTREE_t		*uidMap	= NULL;
    DxService_t	*service	= NULL;

    if (manager->list && !force)
        return manager;

    HxLOG_Trace();

    list = NX_PORT_META_SVC_GetServiceList();
    if (list == NULL)
    {
        HxLOG_Debug("Cannot get channel list from appkit\n");
#ifdef CONFIG_OP_JAPAN
        if(manager->list)
        {
            HLIB_VECTOR_RemoveAll(manager->list);
        }
        if(manager->map)
        {
            HLIB_TREE_RemoveAll(manager->map);
        }
#endif
        return manager;
    }
    uidMap = HLIB_TREE_New(nx_channel_CompareHINT32);
    if (uidMap == NULL)
    {
        HLIB_VECTOR_Delete(list);
        HxLOG_Error("Out of memory!!!\n");
        return NULL;
    }
    HxVECTOR_EACH(list, DxService_t *, service, {
        HLIB_TREE_Insert(uidMap, (void *)service->uid, (void *)service);
    });

    if (manager->list)
        HLIB_VECTOR_Delete(manager->list);
    if (manager->map)
        HLIB_TREE_Destroy(manager->map);

    (void) HLIB_VECTOR_Sort(list, nx_channel_SortByLCN);

    manager->list = list;
    manager->map  = uidMap;

	NX_CHANNEL_Update(manager->list);

#if DONOTSAVE
    //OPL_Favourite_Update();
#endif

    return manager;
}

static char * nx_channel_MakeCCID (char *buf, size_t n, int uid, int onid, int tsid, int sid)
{
   	if (buf == NULL || n == 0)
    {
        return NULL;
    }
    HxSTD_PrintToStrN(buf, n, "ccid:%d.%d.%d.%d", uid, onid, tsid, sid);

    return buf;
}

static HBOOL nx_channel_FindService(void *data, void *userdata)
{
	DxService_t *pstService = (DxService_t *)data;
	HUINT32 findSvcUid = (HUINT32)userdata;

	if(pstService->uid == findSvcUid)
		return TRUE;
	else
		return FALSE;
}

static HUINT32 nx_channel_FindSvcUidByCcid(const char *ccid)
{
    HCHAR	buf[64];
    HCHAR	*token, *save;

    HxLOG_Assert(ccid);

    if (!HLIB_STD_StrStartWith((const HCHAR *)ccid, "ccid:"))
        return 0;

    HLIB_STD_StrNCpySafe(buf, (const HCHAR *)ccid + 5, 64);

    token = HxSTD_StrTok(buf, ".", &save);
    if (token == NULL)
        return 0;

    if (!HLIB_STD_IsInteger(token))
        return 0;

    return (int)HLIB_STD_StrToINT32(token);
}

static HBOOL nx_channel_FindListItemByNxChannel(void *data, void *userdata)
{
	CListItem_t 	*pstListItem 	= (CListItem_t *)data;
	NX_Channel_t 	*pstChannel 	= (NX_Channel_t*)userdata;

	if (pstListItem->uid == pstChannel->svcUid)
		return TRUE;
	else
		return FALSE;
}

static HBOOL nx_channel_FindListItemBySvcUidCB(void *data, void *userdata)
{
	CListItem_t 	*pstListItem 	= (CListItem_t *)data;
	HUINT32 		svcUid			= (HUINT32)userdata;

	if (pstListItem->uid == svcUid )
		return TRUE;
	else
		return FALSE;
}

static HERROR nx_channel_TuneTry(HUINT32 ulViewId, NX_Channel_t *channel)
{
	HUINT32 ulSessionId = 0;
	OxMediaPlay_StartInfo_t 	 unStart;

	unStart.stLive.eType			= eOxMP_MEDIATYPE_LIVE;
	unStart.stLive.eLiveType		= eOxMP_LIVETYPE_MAIN;
	unStart.stLive.ulMasterSvcUid	= channel->svcUid;
	unStart.stLive.ulSuppleSvcUid	= channel->svcUid;
	unStart.stLive.eSuppSvcType		= eOxMP_SUPPLESVCTYPE_Normal;
	unStart.stLive.ulMajorCHNum		= channel->lcn;
	unStart.stLive.eTuneMode		= 0x00;

	return NX_PORT_MEDIA_PLAY_Start (ulViewId, &unStart, &ulSessionId);
}

static HERROR nx_channel_FCCTry(HUINT32 ulViewId, NX_Channel_t *channel)
{
	HUINT32 ulSessionId = 0;
	OxMediaPlay_StartInfo_t 	 unStart;

	unStart.stLive.eType			= eOxMP_MEDIATYPE_LIVE;
	unStart.stLive.eLiveType		= eOxMP_LIVETYPE_FCC;
	unStart.stLive.ulMasterSvcUid	= channel->svcUid;
	unStart.stLive.ulSuppleSvcUid	= channel->svcUid;
	unStart.stLive.eSuppSvcType		= eOxMP_SUPPLESVCTYPE_Normal;
	unStart.stLive.ulMajorCHNum		= channel->lcn;
	unStart.stLive.eTuneMode		= 0x00;

	return NX_PORT_MEDIA_PLAY_Start (ulViewId, &unStart, &ulSessionId);
}


static HERROR	nx_channel_bindChannel(NX_Channel_t *channel)
{
	HERROR					hErr = ERR_FAIL;
	OxMediaPlay_MediaType_e	eOapiType;
	HUINT32					ulMasterSvcUid;

	HUINT32 ulViewId = NX_AV_GetVideoResourceId();

	NX_PORT_MEDIA_PLAY_GetPlayType(ulViewId, &eOapiType);

	switch(eOapiType)
	{
		case eOxMP_MEDIATYPE_LIVE:
			NX_PORT_MEDIA_PLAY_GetRequestId(ulViewId, eOxMP_MEDIATYPE_LIVE, &ulMasterSvcUid);

			// 같은 채널인 경우..
			if (channel->svcUid == ulMasterSvcUid)
			{
			}
			hErr = ERR_OK;
			break;

		case eOxMP_MEDIATYPE_PVRPLAYBACK:
		case eOxMP_MEDIATYPE_TSRPLAYBACK:
		case eOxMP_MEDIATYPE_MEDIAPLAY:
		{
			hErr = NX_PORT_MEDIA_PLAY_Stop (ulViewId);
			break;
		}
		default:
			hErr = ERR_OK;
			break;
	}

	if (hErr != ERR_OK)
	{
		HxLOG_Error ("Cannot stop the service : ulViewId=0x%08x, SVCUID=%d\n", ulViewId, channel->svcUid);
		return ERR_FAIL;
	}

	hErr = nx_channel_TuneTry(ulViewId, channel);
	if (hErr != ERR_OK)
	{
		HxLOG_Error ("[%s:%d] nx_channel_TuneTry err: ViewId=%d\n", __FUNCTION__, __LINE__, ulViewId);
	}

	// search PF
	hErr = NX_METADATA_PF_SearchPF(channel);

	return ERR_OK;
}


static HERROR	nx_channel_bindFCC(NX_Channel_t *channel)
{
	HERROR					hErr = ERR_FAIL;
	OxMediaPlay_MediaType_e	eOapiType;
	HUINT32 ulViewId = NX_AV_GetVideoResourceId();

	NX_PORT_MEDIA_PLAY_GetPlayType(ulViewId, &eOapiType);

	if(eOapiType == eOxMP_MEDIATYPE_LIVE)
	{
		hErr = nx_channel_FCCTry(ulViewId, channel);
		if (hErr != ERR_OK)
		{
			HxLOG_Error ("nx_channel_FCCTry err: ViewId=%d\n", ulViewId);
		}
	}

	return ERR_OK;
}


static HERROR	nx_channel_StopChannel(void)
{
	HERROR		hError = ERR_FAIL;
	HUINT32 	ulViewId = 0;
	OxMediaPlay_MediaType_e eOapiType = eOxMP_MEDIATYPE_NONE;

	ulViewId = NX_AV_GetVideoResourceId();
	NX_PORT_MEDIA_PLAY_GetPlayType(ulViewId, &eOapiType);

	switch(eOapiType)
	{
		case eOxMP_MEDIATYPE_LIVE:
			hError = NX_PORT_MEDIA_PLAY_Stop(ulViewId);
			break;

		case eOxMP_MEDIATYPE_PVRPLAYBACK:
		case eOxMP_MEDIATYPE_TSRPLAYBACK:
		case eOxMP_MEDIATYPE_MEDIAPLAY:
			// add to do???
			break;

		default:
			hError = ERR_OK;
			break;
	}

	if (hError != ERR_OK)
	{
		HxLOG_Error ("Cannot stop the service : ulViewId=0x%08x\n", ulViewId);
		return ERR_FAIL;
	}

	return ERR_OK;
}


static HUINT32	nx_channel_GetSpecificInformation(void)
{
	HERROR		hError = ERR_FAIL;
	HUINT32		ulOperatorId = CLIB_OP_NULL;
	DxOperator_e	eOpType = eDxOPERATOR_None;

	hError = NX_SYSTEM_GetOperatorType(&eOpType);
	if (hError != ERR_OK)
	{
		NX_APP_Error("ERROR!!! NX_SYSTEM_GetOperatorType() \n");
	}

	switch (eOpType)
	{
		case eDxOPERATOR_MiddleEast:
			ulOperatorId = CLIB_OP_ME;
			break;
		case eDxOPERATOR_ERtelecom:
			ulOperatorId = CLIB_OP_ERTELECOM;
			break;
		case eDxOPERATOR_Ses:
			ulOperatorId = CLIB_OP_SES;
			break;
		case eDxOPERATOR_MBC:
			ulOperatorId = CLIB_OP_MBC;
			break;
		case eDxOPERATOR_AMX:
			ulOperatorId = CLIB_OP_AMX;
			break;
		// TODO: Add Operator Type

		default:
			ulOperatorId = CLIB_OP_NULL;
			break;
	}

	NX_APP_Info("eOpType(%u), ulOperatorId(0x%x)\n", eOpType, ulOperatorId);

#if defined(CONFIG_OP_UK_DTT)
	ulOperatorId = CLIB_OP_UK_DTT;
#else
	// TODO: Add else if with product config
#endif

	NX_APP_Info("Final ulOperatorId(0x%x)\n", ulOperatorId);

	return ulOperatorId;
}


static HUINT32	nx_channel_ConvertNxChannelGroupToCLIBGroupType(NX_ChListGroup_t eChGroupType)
{
	HUINT32		ulContentsType = CLIB_TYPE_TV;

	switch (eChGroupType)
	{
		case eNxChListGroup_TV:			ulContentsType = CLIB_TYPE_TV;			break;
		case eNxChListGroup_RADIO:		ulContentsType = CLIB_TYPE_RADIO;		break;
		case eNxChListGroup_HDTV:		ulContentsType = CLIB_TYPE_HD;			break;
		case eNxChListGroup_FTA:		ulContentsType = CLIB_TYPE_CAS;			break;
		case eNxChListGroup_CAS:		ulContentsType = CLIB_TYPE_CAS;			break;
		case eNxChListGroup_ALPHABET:
		case eNxChListGroup_AlphabetAll:
		case eNxChListGroup_AlphabetA:
		case eNxChListGroup_AlphabetB:
		case eNxChListGroup_AlphabetC:
		case eNxChListGroup_AlphabetD:
		case eNxChListGroup_AlphabetE:
		case eNxChListGroup_AlphabetF:
		case eNxChListGroup_AlphabetG:
		case eNxChListGroup_AlphabetH:
		case eNxChListGroup_AlphabetI:
		case eNxChListGroup_AlphabetJ:
		case eNxChListGroup_AlphabetK:
		case eNxChListGroup_AlphabetL:
		case eNxChListGroup_AlphabetM:
		case eNxChListGroup_AlphabetN:
		case eNxChListGroup_AlphabetO:
		case eNxChListGroup_AlphabetP:
		case eNxChListGroup_AlphabetQ:
		case eNxChListGroup_AlphabetR:
		case eNxChListGroup_AlphabetS:
		case eNxChListGroup_AlphabetT:
		case eNxChListGroup_AlphabetU:
		case eNxChListGroup_AlphabetV:
		case eNxChListGroup_AlphabetW:
		case eNxChListGroup_AlphabetX:
		case eNxChListGroup_AlphabetY:
		case eNxChListGroup_AlphabetZ:
		case eNxChListGroup_AlphabetDigit:
		case eNxChListGroup_AlphabetEtc:
			ulContentsType = CLIB_TYPE_ALPABET;		break;
		default:
			NX_APP_Error("Error!!! Not Support Group Type : [0x%x]!!\n", eChGroupType);
			ulContentsType = CLIB_TYPE_TV;
			break;
	}

	return	ulContentsType;
}


static HUINT32	nx_channel_ConvertNxChannelAlpabetGroupToCLIBAlphabetGroupFilter(NX_ChListGroup_t eChAlphabetGroupType)
{
	HUINT32		ulExtraFilter = CLIB_TYPE_ALPABET;

	switch (eChAlphabetGroupType)
	{
		case eNxChListGroup_ALPHABET:		ulExtraFilter = eDxAlphabetGroup_AlphabetAll;		break;
		case eNxChListGroup_AlphabetAll:	ulExtraFilter = eDxAlphabetGroup_AlphabetAll;		break;
		case eNxChListGroup_AlphabetA:		ulExtraFilter = eDxAlphabetGroup_AlphabetA;			break;
		case eNxChListGroup_AlphabetB:		ulExtraFilter = eDxAlphabetGroup_AlphabetB;			break;
		case eNxChListGroup_AlphabetC:		ulExtraFilter = eDxAlphabetGroup_AlphabetC;			break;
		case eNxChListGroup_AlphabetD:		ulExtraFilter = eDxAlphabetGroup_AlphabetD;			break;
		case eNxChListGroup_AlphabetE:		ulExtraFilter = eDxAlphabetGroup_AlphabetE;			break;
		case eNxChListGroup_AlphabetF:		ulExtraFilter = eDxAlphabetGroup_AlphabetF;			break;
		case eNxChListGroup_AlphabetG:		ulExtraFilter = eDxAlphabetGroup_AlphabetG;			break;
		case eNxChListGroup_AlphabetH:		ulExtraFilter = eDxAlphabetGroup_AlphabetH;			break;
		case eNxChListGroup_AlphabetI:		ulExtraFilter = eDxAlphabetGroup_AlphabetI;			break;
		case eNxChListGroup_AlphabetJ:		ulExtraFilter = eDxAlphabetGroup_AlphabetJ;			break;
		case eNxChListGroup_AlphabetK:		ulExtraFilter = eDxAlphabetGroup_AlphabetK;			break;
		case eNxChListGroup_AlphabetL:		ulExtraFilter = eDxAlphabetGroup_AlphabetL;			break;
		case eNxChListGroup_AlphabetM:		ulExtraFilter = eDxAlphabetGroup_AlphabetM;			break;
		case eNxChListGroup_AlphabetN:		ulExtraFilter = eDxAlphabetGroup_AlphabetN;			break;
		case eNxChListGroup_AlphabetO:		ulExtraFilter = eDxAlphabetGroup_AlphabetO;			break;
		case eNxChListGroup_AlphabetP:		ulExtraFilter = eDxAlphabetGroup_AlphabetP;			break;
		case eNxChListGroup_AlphabetQ:		ulExtraFilter = eDxAlphabetGroup_AlphabetQ;			break;
		case eNxChListGroup_AlphabetR:		ulExtraFilter = eDxAlphabetGroup_AlphabetR;			break;
		case eNxChListGroup_AlphabetS:		ulExtraFilter = eDxAlphabetGroup_AlphabetS;			break;
		case eNxChListGroup_AlphabetT:		ulExtraFilter = eDxAlphabetGroup_AlphabetT;			break;
		case eNxChListGroup_AlphabetU:		ulExtraFilter = eDxAlphabetGroup_AlphabetU;			break;
		case eNxChListGroup_AlphabetV:		ulExtraFilter = eDxAlphabetGroup_AlphabetV;			break;
		case eNxChListGroup_AlphabetW:		ulExtraFilter = eDxAlphabetGroup_AlphabetW;			break;
		case eNxChListGroup_AlphabetX:		ulExtraFilter = eDxAlphabetGroup_AlphabetX;			break;
		case eNxChListGroup_AlphabetY:		ulExtraFilter = eDxAlphabetGroup_AlphabetY;			break;
		case eNxChListGroup_AlphabetZ:		ulExtraFilter = eDxAlphabetGroup_AlphabetZ;			break;
		case eNxChListGroup_AlphabetDigit:	ulExtraFilter = eDxAlphabetGroup_AlphabetDigit;		break;
		case eNxChListGroup_AlphabetEtc:	ulExtraFilter = eDxAlphabetGroup_AlphabetEtc;		break;
		default:
			NX_APP_Error("Error!!! Not Support Alphabet Group : [0x%x]!!\n", eChAlphabetGroupType);
			ulExtraFilter = eDxAlphabetGroup_AlphabetAll;		break;
			break;
	}

	return	ulExtraFilter;
}

static void	nx_channel_SetupDamaNotifier(void)
{
#if !defined(CONFIG_PROD_OS_EMULATOR) || defined(CONFIG_SINGLE_PROCESS_MULTI_THREAD)
	DAPI_AddTableNotifier(DxNAMEOF(DxService_t), nx_channel_OnDamaServiceUpdated, NULL);
#endif
	NX_PORT_META_SVC_AllowTableNotifyMyself(DxNAMEOF(DxService_t), TRUE);
}

static void nx_channel_OnDamaServiceUpdated(const HCHAR *pszName, HUID hUid, const void *pvData, HUINT32 ulSize, void *pvUser)
{
	HxLOG_Debug("\nReceived Dama Update Event. (%s) , ulSize(%d)\n", pszName, ulSize);
}


/**************************************************************************************************/
#define _____________Public_functions____________________________________________
/**************************************************************************************************/
HERROR NX_CHANNEL_Init(void)
{
	DxOperator_e 		eOpType = eDxOPERATOR_None;
	NX_ChannelManager_t *instance = NULL;

	s_firstInstance = instance = nx_channel_GetManager(NULL);

	// set channel update listener
	NX_SYSTEM_GetOperatorType(&eOpType);
	switch (eOpType)
	{
		// TODO: Add Checker for operator or product config

		default:
			NX_APP_Info(" Set Base Update Checker !!! \n");
			NX_PORT_META_SVC_SetUpdateChecker(nx_channel_BaseUpdateChecker);
			break;
	}

	NX_PORT_META_SVC_SetListener(nx_channel_EventListener, (void *)instance);

	nx_channel_SetupDamaNotifier();

#if defined(CONFIG_DEBUG)
	nx_channel_Command_Init();
#endif
	//nx_channel_GetManager(NULL);

	return ERR_OK;
}

static HINT32 nx_channel_channge(HINT32 svcuid, HINT32 chnum)
{
	HUINT32 ulSessionId = 0;
	HUINT32 ulMainViewId = 0;
	OxMediaPlay_StartInfo_t 	 unStart;

	NX_PORT_MEDIA_PLAY_GetMainViewId(&ulMainViewId);

	unStart.stLive.eType			= eOxMP_MEDIATYPE_LIVE;
	unStart.stLive.eLiveType		= eOxMP_LIVETYPE_MAIN;
	unStart.stLive.ulMasterSvcUid	= svcuid;
	unStart.stLive.ulSuppleSvcUid	= svcuid;
	unStart.stLive.eSuppSvcType		= eOxMP_SUPPLESVCTYPE_Normal;
	unStart.stLive.ulMajorCHNum		= chnum;
	unStart.stLive.eTuneMode		= 0x00;

	NX_PORT_MEDIA_PLAY_Start (ulMainViewId, &unStart, &ulSessionId);

	return HxCMD_OK;
}

void	NX_CHANNEL_LoadList(HBOOL forced)
{
	nx_channel_LoadList(nx_channel_GetManager(s_firstInstance), forced);
}

HERROR	NX_CHANNEL_Update(HxVector_t *list)
{
	int len = 0;

	if (list == NULL)
		return ERR_FAIL;

	len = HLIB_VECTOR_Length(list);

	if (len <= 0)
		return ERR_FAIL;

#if 0
{
	int i;
	if (s_testChannel != NULL)
	{
		NX_APP_Free(s_testChannel);
		s_testChannel = NULL;
	}

	s_testChannel = (NX_TestChannelZap_t*)NX_APP_Calloc(sizeof(NX_TestChannelZap_t) * len);

	for ( i =0; i < len; i++)
	{
		DxService_t *data = (DxService_t*)HLIB_VECTOR_ItemAt(list,i);
		if (data)
		{
			HxLOG_Debug("\n[%s.%d] data: svcid(%d), name(%s), uid(%d), lcn(%d) \n",
				__FUNCTION__,__LINE__,
				data->svcid,
				data->name,
				data->uid,
				data->lcn);
			s_testChannel[i].uid = data->uid;
			s_testChannel[i].lcn = data->lcn;
		}
	}
}
#endif

	return ERR_OK;
}


HERROR	NX_CHANNEL_InitLastChannel(void)
{
	HERROR	hError = ERR_FAIL;
	char	szRetCCID[NX_TEXT_SIZE_128] = {0,};

	nx_channel_MakeCCID(szRetCCID, NX_CORE_CHANNEL_MAKE_CCID_MAX_SIZE, 0, 0, 0, 0);
	HxLOG_Debug("[%s.%d] Save Channel Inform to UDB (CCID : %s)\n",__FUNCTION__,__LINE__, szRetCCID);

	hError = NX_PORT_DB_SetStr(USERDB_SVC_LAST_CHANNEL, szRetCCID);
	if (hError != ERR_OK)
	{
		NX_APP_Error("Error!!! NX_PORT_DB_SetStr() - USERDB_SVC_LAST_CHANNEL:(%s)!!\n", szRetCCID);
		return ERR_FAIL;
	}

	return ERR_OK;
}


HERROR	NX_CHANNEL_SaveLastChannel(HUINT32 svcUid)
{
	NX_ChannelManager_t 	*instance 		= NULL;
	DxService_t 			*pstFindService = NULL;
	char szRetCCID[NX_TEXT_SIZE_128] = {0,};

	instance = nx_channel_GetManager(s_firstInstance);
	if ((instance == NULL) && (instance->list == NULL))
	{
		HxLOG_Critical("\n[%s.%d] Channel Manager instance is NULL !!\n",__FUNCTION__,__LINE__);
		return ERR_FAIL;
	}

	pstFindService = HLIB_VECTOR_Find(instance->list, nx_channel_FindService, (void*)svcUid);
	if (pstFindService == NULL)
	{
		HxLOG_Debug("[%s.%d] Not found service data( uid : %d)\n",__FUNCTION__,__LINE__, svcUid);
		return ERR_FAIL;
	}

	nx_channel_MakeCCID(szRetCCID, NX_CORE_CHANNEL_MAKE_CCID_MAX_SIZE, pstFindService->uid, pstFindService->onid, pstFindService->tsid, pstFindService->svcid);

	HxLOG_Debug("[%s.%d] Save Channel Inform to UDB (CCID : %s)\n",__FUNCTION__,__LINE__, szRetCCID);

	NX_PORT_DB_SetStr(USERDB_SVC_LAST_CHANNEL, szRetCCID);

	return ERR_OK;
}


DxService_t		*NX_CHANNEL_GetLastService(void)
{
	HCHAR					szRetCCID[NX_TEXT_SIZE_128] = {0,};
	DxService_t 			*pstFindService = NULL;
	NX_ChannelManager_t 	*pstInstance = NULL;


	if (NX_PORT_DB_GetStr((char *)USERDB_SVC_LAST_CHANNEL, szRetCCID, NX_TEXT_SIZE_128) != ERR_OK)
	{
		HxLOG_Critical("\n[%s.%d] Not Found Last Channel inform!!!\n",__FUNCTION__,__LINE__);
		return NULL;
	}

	pstInstance = nx_channel_GetManager(s_firstInstance);
	if ((pstInstance == NULL) || (pstInstance->list == NULL))
	{
		HxLOG_Critical("\n[%s.%d] Channel Manager instance is NULL !!\n",__FUNCTION__,__LINE__);
		return NULL;
	}

	pstFindService = HLIB_VECTOR_Find(pstInstance->list, nx_channel_FindService, (void*)nx_channel_FindSvcUidByCcid(szRetCCID));
	if (pstFindService != NULL)
	{
		HxLOG_Debug("[%s.%d]Found last Service inform: svcUID (%ld), onid(%d), tsid(%d), svcid(%d)\n", __FUNCTION__,__LINE__,
			pstFindService->uid, pstFindService->onid, pstFindService->tsid, pstFindService->svcid);
	}

	return pstFindService;
}

DxService_t		*NX_CHANNEL_FindServiceBySvcUid(HUINT32 svcuid)
{
	DxService_t 			*pstFindService = NULL;
	NX_ChannelManager_t 	*pstInstance = NULL;

	pstInstance = nx_channel_GetManager(s_firstInstance);
	if ((pstInstance == NULL) || (pstInstance->list == NULL))
	{
		HxLOG_Critical("\n[%s.%d] Channel Manager instance is NULL !!\n",__FUNCTION__,__LINE__);
		return NULL;
	}
	pstFindService = HLIB_VECTOR_Find(pstInstance->list, nx_channel_FindService, (void*)svcuid);
	if (pstFindService != NULL)
	{
		HxLOG_Debug("[%s.%d]Found last Service inform: svcUID (%ld), onid(%d), tsid(%d), svcid(%d)\n", __FUNCTION__,__LINE__,
			pstFindService->uid, pstFindService->onid, pstFindService->tsid, pstFindService->svcid);
	}

	return pstFindService;
}

HERROR	NX_CHANNEL_GetLockState(OxMediaPlay_LiveLockState_e *peLockState)
{
	HUINT32 ulViewId = NX_AV_GetVideoResourceId();

	return NX_PORT_MEDIA_PLAY_GetLockState(ulViewId, peLockState);
}

HERROR	NX_CHANNEL_CheckPlayChangable(NX_Channel_t *pstChannel, HBOOL *pbChangable)
{
	HUINT32 ulViewId = NX_AV_GetVideoResourceId();

	return NX_PORT_MEDIA_PLAY_CheckPlayChangable(ulViewId, pstChannel->svcUid, pbChangable);
}

HERROR	NX_CHANNEL_SetChannel(NX_Channel_t *channel)
{
	HUINT32 ulMainViewId;

	if (channel == NULL)
	{
		HxLOG_Error("channel is NULL\n");
		return ERR_FAIL;
	}

	HxLOG_Warning("________________ Set Channel : [ %04d: %08x] ________________\n", channel->lcn, channel->svcUid);

	nx_channel_bindChannel(channel);

	if (NX_PORT_MEDIA_PLAY_GetMainViewId(&ulMainViewId) == ERR_OK)
	{
		HUINT32 ulViewId = NX_AV_GetVideoResourceId();

		if (ulViewId == ulMainViewId)
		{
			NX_CHANNEL_SaveLastChannel(channel->svcUid);
		}
	}
	return ERR_OK;
}


HERROR	NX_CHANNEL_StopChannel(void)
{
	return nx_channel_StopChannel();
}

HERROR	NX_CHANNEL_SetFastChannelChange(NX_Channel_t *channel)
{
	if (channel == NULL)
	{
		HxLOG_Error("channel is NULL\n");
		return ERR_FAIL;
	}

	HxLOG_Warning("_ FCC : [ %04d: %08x]\n", channel->lcn, channel->svcUid);

	nx_channel_bindFCC(channel);

	return ERR_OK;
}


HERROR	NX_CHANNEL_UpdateService(DxService_t *pstService)
{
	return	NX_PORT_META_SVC_SetService(pstService);
}


HERROR	NX_CHANNEL_RemoveService(DxService_t *pstService)
{
	HERROR	hError = ERR_FAIL;

	hError = NX_PORT_META_SVC_RemoveService(pstService);
	if (hError != ERR_OK)
	{
		NX_APP_Error("Error!!! NX_PORT_META_SVC_RemoveService() !!\n");
	}
	else
	{
		// Remove Service in all favourite group List
		NX_CHANNEL_FAV_RemoveServiceInAllFavouriteGroup(pstService);

		/* Reload Base Service List */
		NX_CHANNEL_LoadList(TRUE);
	}

	return	hError;
}


HERROR		NX_CHANNEL_RemoveChannelListAll(void)
{
	HERROR		hError = ERR_FAIL;
	NX_ChannelManager_t 	*manager = NULL;

	hError = NX_PORT_META_SVC_Reset();
	if (hError == ERR_FAIL)
	{
		HxLOG_Error("Failed Meta Service Reset!\n");
		return hError;
	}
	manager = nx_channel_GetManager(s_firstInstance);
	if (manager == NULL && manager->list == NULL)
	{
		HxLOG_Critical("\n[%s.%d] Channel Manager instance is NULL !!\n",__FUNCTION__,__LINE__);
		return ERR_FAIL;
	}

	// remove nx channel list
	if (manager->list)
	{
        HLIB_VECTOR_Delete(manager->list);
		manager->list = NULL;
	}
    if (manager->map)
    {
        HLIB_TREE_Destroy(manager->map);
		manager->map = NULL;
    }

	hError = NX_CHANNEL_InitLastChannel();
	if (hError != ERR_OK)
	{
		HxLOG_Error("Error!!! NX_CHANNEL_InitLastChannel() Fail !!\n");
	}
	hError = NX_PORT_SYSTEM_SetLastServiceGroup(eNxChListGroup_TV);
	if (hError != ERR_OK)
	{
		HxLOG_Error("Error!!! NX_PORT_SYSTEM_SetLastServiceGroup() Fail !!\n");
	}

	return ERR_OK;
}


HINT32		NX_CHANNEL_GetChanenlCountAll(void)
{
	HINT32		lTvSvcCount = 0, lRadioSvcCount = 0;

	lTvSvcCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_TV);
	lRadioSvcCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_RADIO);

	return	(lTvSvcCount + lRadioSvcCount);
}


HERROR	NX_CHANNEL_GetServiceListBySatelliteName(HUINT8 *szSatName, HxVector_t *stResultList, HINT32 *lNum)
{
	HERROR			hError = ERR_FAIL;
	HINT32			lListItemNum = 0, i = 0, lItemNum = 0;
	HxVector_t		*pstListItem = NULL;
	CListItem_t 	*pstItem = NULL;
	DxService_t 	*pstFoundService = NULL;

	hError = nx_channel_group_MakeNxChannelListItemByGroup(eNxChListGroup_TV, &pstListItem, &lListItemNum);
	if ((hError == ERR_OK) && (lListItemNum > 0))
	{
		for (i = 0; i < lListItemNum; i++)
		{
			pstItem = (CListItem_t*)HLIB_VECTOR_ItemAt(pstListItem, i);
			if (pstItem)
			{
				pstFoundService = NX_CHANNEL_GROUP_FindServiceInGroupByServiceUid(eNxChListGroup_TV, eNxFindSvcDir_Current, pstItem->uid);
				if (pstFoundService && (HxSTD_StrCmp((char*)szSatName, pstFoundService->satType) == 0))
				{
					if (stResultList)
					{
						HLIB_VECTOR_Add(stResultList, pstItem);
					}

					lItemNum++;
				}
			}
		}

		if (pstListItem)
		{
			HLIB_VECTOR_Delete(pstListItem);
			pstListItem = NULL;
		}
	}

	hError = nx_channel_group_MakeNxChannelListItemByGroup(eNxChListGroup_RADIO, &pstListItem, &lListItemNum);
	if ((hError == ERR_OK) && (lListItemNum > 0))
	{
		for (i = 0; i < lListItemNum; i++)
		{
			pstItem = (CListItem_t*)HLIB_VECTOR_ItemAt(pstListItem, i);
			if (pstItem)
			{
				pstFoundService = NX_CHANNEL_GROUP_FindServiceInGroupByServiceUid(eNxChListGroup_RADIO, eNxFindSvcDir_Current, pstItem->uid);
				if (pstFoundService && (HxSTD_StrCmp((char*)szSatName, pstFoundService->satType) == 0))
				{
					if (stResultList)
					{
						HLIB_VECTOR_Add(stResultList, pstItem);
					}
					lItemNum++;
				}
			}
		}

		if (pstListItem)
		{
			HLIB_VECTOR_Delete(pstListItem);
			pstListItem = NULL;
		}
	}

	*lNum = lItemNum;

	return	ERR_OK;
}


HERROR		NX_CHANNEL_DeleteServiceListBySatelliteName(HUINT8 *szSatName)
{
	HBOOL			bDeleteSvc = FALSE;
	HERROR			hError = ERR_FAIL;
	HINT32			lListItemNum = 0, i = 0;
	HxVector_t		*pstListItem = NULL;
	CListItem_t 	*pstItem = NULL;
	DxService_t 	*pstFoundService = NULL;

	hError = nx_channel_group_MakeNxChannelListItemByGroup(eNxChListGroup_TV, &pstListItem, &lListItemNum);
	if ((hError == ERR_OK) && (lListItemNum > 0))
	{
		for (i = 0; i < lListItemNum; i++)
		{
			pstItem = (CListItem_t*)HLIB_VECTOR_ItemAt(pstListItem, i);
			if (pstItem)
			{
				pstFoundService = NX_CHANNEL_GROUP_FindServiceInGroupByServiceUid(eNxChListGroup_TV, eNxFindSvcDir_Current, pstItem->uid);
				if (pstFoundService && (HxSTD_StrCmp((char*)szSatName, pstFoundService->satType) == 0))
				{
					NX_CHANNEL_RemoveService(pstFoundService);
					bDeleteSvc = TRUE;
				}
			}
		}

		if (pstListItem)
		{
			HLIB_VECTOR_Delete(pstListItem);
			pstListItem = NULL;
		}
	}

	hError = nx_channel_group_MakeNxChannelListItemByGroup(eNxChListGroup_RADIO, &pstListItem, &lListItemNum);
	if ((hError == ERR_OK) && (lListItemNum > 0))
	{
		for (i = 0; i < lListItemNum; i++)
		{
			pstItem = (CListItem_t*)HLIB_VECTOR_ItemAt(pstListItem, i);
			if (pstItem)
			{
				pstFoundService = NX_CHANNEL_GROUP_FindServiceInGroupByServiceUid(eNxChListGroup_RADIO, eNxFindSvcDir_Current, pstItem->uid);
				if (pstFoundService && (HxSTD_StrCmp((char*)szSatName, pstFoundService->satType) == 0))
				{
					NX_CHANNEL_RemoveService(pstFoundService);
					bDeleteSvc = TRUE;
				}
			}
		}

		if (pstListItem)
		{
			HLIB_VECTOR_Delete(pstListItem);
			pstListItem = NULL;
		}
	}

	hError = (bDeleteSvc == TRUE) ? ERR_OK : ERR_FAIL;

	return	hError;
}


HERROR	NX_CHANNEL_GetLastServiceGroup(HINT32 *plLastSvcGroup)
{
	return	NX_PORT_SYSTEM_GetLastServiceGroup(plLastSvcGroup);
}


HERROR	NX_CHANNEL_SetLastServiceGroup(HINT32 lLastSvcGroup)
{
	return	NX_PORT_SYSTEM_SetLastServiceGroup(lLastSvcGroup);
}


#define ______________RELATED_CHANNEL_GROUP____________________
/***************************************************************************
Related Channel Group
***************************************************************************/
static	HxVector_t	*nx_channel_group_GetReferServiceList(HxVector_t *totalSvcList, NX_ChListGroup_t eChGroupType)
{
	HxVector_t	*pstReferList = NULL;

#if defined(CONFIG_OP_MBC)
	pstReferList = nx_channel_group_mbc_GetReferServiceList(totalSvcList, eChGroupType);
#endif

	return pstReferList;
}

static	HERROR	nx_channel_group_MakeNxChannelListItemByGroup(NX_ChListGroup_t eChGroupType, HxVector_t **ddstListItem, HINT32 *nListItemNum)
{
	HINT32			i = 0, ulGroupLen = 0;
	HUINT32 		ulOperatorId = CLIB_OP_NULL, ulContentsType = 0 , ulExtraFilter = 0;
	HxVector_t		*pstGroupList = NULL;
	NX_ChannelManager_t 	*stInstance = NULL;
	HxVector_t		*svcList = NULL, *referList = NULL;			// group 처리할 service list

	// full list 얻기위한 용도.
	HxVector_t		*pstTvList = NULL, *pstRadioList = NULL, *fullList = NULL;
	HINT32			tvLen = 0, radioLen = 0;

	ulOperatorId = nx_channel_GetSpecificInformation();
	stInstance = nx_channel_GetManager(s_firstInstance);
	if ((stInstance == NULL) || (stInstance->list == NULL))
	{
		HxLOG_Critical("\n[%s.%d] Channel Manager instance is NULL !!\n",__FUNCTION__,__LINE__);
		return ERR_FAIL;
	}

	ulContentsType = nx_channel_ConvertNxChannelGroupToCLIBGroupType(eChGroupType);
	NX_APP_Info("===== [Load Channel List] eChGroupType:[0x%x]-ulContentsType[0x%x], ulOperatorId:[0x%x], ulExtraFilter:(0x%x) !! \n", eChGroupType, ulContentsType, ulOperatorId, ulExtraFilter);

	ulExtraFilter = NX_CHANNEL_GROUP_GetExtraFilterByGroupType(eChGroupType);

	// group 할 svc list 를 얻어오고, 없다면 전체 svc list 를 사용한다.
	// proc 나 op 별로 달라지는 부분에 대한 지원.
	referList = nx_channel_group_GetReferServiceList(stInstance->list, eChGroupType);

	if ((referList != NULL) && (HLIB_VECTOR_Length(referList) > 0))
		svcList = referList;
	else
		svcList = stInstance->list;

	// 전체에서 group type 별로 얻어오려면.. clib 에 pstOrgList 로 전체 리스트를 줘야함.
	pstTvList = CLIB_GenerateList(svcList, NULL, CLIB_TYPE_TV, ulOperatorId, 0, TRUE);
	pstRadioList = CLIB_GenerateList(svcList, NULL, CLIB_TYPE_RADIO, ulOperatorId, 0, TRUE);

	if (pstTvList != NULL)
		tvLen = HLIB_VECTOR_Length(pstTvList);
	if (pstRadioList != NULL)
		radioLen = HLIB_VECTOR_Length(pstRadioList);

	fullList = HLIB_VECTOR_NewEasy(tvLen+radioLen, NULL, NULL);

	for (i = 0; i < tvLen; i++)
		HLIB_VECTOR_Add(fullList, (void*)HLIB_VECTOR_ItemAt(pstTvList, i));
	for (i = 0; i < radioLen; i++)
		HLIB_VECTOR_Add(fullList, (void*)HLIB_VECTOR_ItemAt(pstRadioList, i));

	pstGroupList = CLIB_GenerateList(svcList, fullList, ulContentsType, ulOperatorId, ulExtraFilter, TRUE);
	if (pstGroupList != NULL)
	{
		ulGroupLen = HLIB_VECTOR_Length(pstGroupList);
	}
	else
	{
		ulGroupLen = 0;
		NX_APP_Error("ERROR!!! CLIB_GenerateList() is NULL \n");
	}

	if (referList)		{		HLIB_VECTOR_Delete(referList);			referList = NULL;		}
	// full list 를 위해 사용한것 release
	if (pstTvList)		{		HLIB_VECTOR_Delete(pstTvList);			pstTvList = NULL;		}
	if (pstRadioList)	{		HLIB_VECTOR_Delete(pstRadioList);		pstRadioList = NULL;	}
	if (fullList)		{		HLIB_VECTOR_Delete(fullList);			fullList = NULL;		}

	if (ulGroupLen <= 0)
	{
		HxLOG_Debug("\n[%s.%d] Load Group List is 0\n",__FUNCTION__,__LINE__);
		*ddstListItem		= NULL;
		*nListItemNum		= 0;
		return ERR_FAIL;
	}

	*ddstListItem		= pstGroupList;
	*nListItemNum		= ulGroupLen;

	return ERR_OK;
}

static	HERROR nx_channel_group_GetGroupInfo(NX_ChListGroup_t eChGroupType, NX_GroupInfoList_t **ddstItem)
{
	NX_GroupInfoList_t	*item = NULL;

	HINT32	nCnt = NX_CHANNEL_GROUP_GetGroupCount(eChGroupType);

	if (nCnt > 0)
	{
		item = (NX_GroupInfoList_t*)NX_APP_Calloc(sizeof(NX_GroupInfoList_t));
		item->eGroupType	= eChGroupType;
		item->nSvcCnt		= nCnt;
		*ddstItem = item;
		return ERR_OK;
	}
	*ddstItem = NULL;
	return ERR_FAIL;
}

HxList_t *	NX_CHANNEL_GROUP_SortNxChannelsByName(NX_Channel_t *dstChannelList, HINT32 nListNum)
{
	HINT32	i = 0;
	NX_Channel_t *pstChannelList = dstChannelList;
	HxList_t		*pRetSortList = NULL;

	if (pstChannelList == NULL || nListNum <= 0)
	{
		HxLOG_Debug("\nSort data is NULL or Length 0\n");
		return NULL;
	}

	for (i = 0; i < nListNum; i++)
	{
		NX_Channel_t	*pChannel = &pstChannelList[i];
		DxService_t *pDxService = NX_CHANNEL_FindServiceBySvcUid(pChannel->svcUid);
		pRetSortList = HLIB_LIST_AppendSortEx(pRetSortList, pDxService, nx_channel_SortByName);
	}

	return pRetSortList;
}

HERROR	NX_CHANNEL_GROUP_SortNxChannelsByLcn(NX_Channel_t *dstChannelList, HINT32 nChNum)
{
	HINT32	i = 0, j = 0;
	NX_Channel_t *pstChannelList = dstChannelList;
	NX_Channel_t	*pCurChannel = NULL, *pNextChannel = NULL;
	NX_Channel_t	tempChannel;

	if (pstChannelList == NULL || nChNum <= 0)
	{
		HxLOG_Debug("\nSort data is NULL or Length 0\n");
		return ERR_FAIL;
	}

	// 직접 sorting
	for (i = 0; i < (nChNum -1); i++)
	{
		pCurChannel 	= &pstChannelList[i];

		for (j = i+1; j < nChNum; j++)
		{
			pNextChannel	= &pstChannelList[j];

			if (pCurChannel->lcn > pNextChannel->lcn)
			{
				tempChannel.lcn 	= pCurChannel->lcn;
				tempChannel.svcUid	= pCurChannel->svcUid;

				pCurChannel->lcn	= pNextChannel->lcn;
				pCurChannel->svcUid = pNextChannel->svcUid;

				pNextChannel->lcn	= tempChannel.lcn;
				pNextChannel->svcUid= tempChannel.svcUid;
			}
		}
	}

	return ERR_OK;
}

HERROR		NX_CHANNEL_GROUP_GetIsGroupInfoList(HxVector_t	*dstList, HINT32 *nNum)
{
	HINT32		nCnt = 0;
	NX_GroupInfoList_t	*item = NULL;


	if (dstList == NULL)
		return ERR_FAIL;

	if (nx_channel_group_GetGroupInfo(eNxChListGroup_TV, &item) == ERR_OK){
		HLIB_VECTOR_Add(dstList, item);
		nCnt++;
	}
	if (nx_channel_group_GetGroupInfo(eNxChListGroup_RADIO, &item) == ERR_OK){
		HLIB_VECTOR_Add(dstList, item);
		nCnt++;
	}
	if (nx_channel_group_GetGroupInfo(eNxChListGroup_HDTV, &item) == ERR_OK){
		HLIB_VECTOR_Add(dstList, item);
		nCnt++;
	}
	if (nx_channel_group_GetGroupInfo(eNxChListGroup_FTA, &item) == ERR_OK){
		HLIB_VECTOR_Add(dstList, item);
		nCnt++;
	}
	if (nx_channel_group_GetGroupInfo(eNxChListGroup_CAS, &item) == ERR_OK){
		HLIB_VECTOR_Add(dstList, item);
		nCnt++;
	}
	if (nx_channel_group_GetGroupInfo(eNxChListGroup_ALPHABET, &item) == ERR_OK){
		HLIB_VECTOR_Add(dstList, item);
		nCnt++;
	}
	if (nx_channel_group_GetGroupInfo(eNxChListGroup_Favorite1, &item) == ERR_OK){
		HLIB_VECTOR_Add(dstList, item);
		nCnt++;
	}
	if (nx_channel_group_GetGroupInfo(eNxChListGroup_Favorite2, &item) == ERR_OK){
		HLIB_VECTOR_Add(dstList, item);
		nCnt++;
	}
	if (nx_channel_group_GetGroupInfo(eNxChListGroup_Favorite3, &item) == ERR_OK){
		HLIB_VECTOR_Add(dstList, item);
		nCnt++;
	}
	if (nx_channel_group_GetGroupInfo(eNxChListGroup_Favorite4, &item) == ERR_OK){
		HLIB_VECTOR_Add(dstList, item);
		nCnt++;
	}
	if (nx_channel_group_GetGroupInfo(eNxChListGroup_Favorite5, &item) == ERR_OK){
		HLIB_VECTOR_Add(dstList, item);
		nCnt++;
	}

	*nNum = nCnt;

	return ERR_OK;
}


HERROR		NX_CHANNEL_GROUP_GetAvailableAlphabetGroupInfoList(HxVector_t *dstList, HINT32 *nNum)
{
	HINT32		nCnt = 0;
	NX_GroupInfoList_t	*item = NULL;


	if (dstList == NULL)
		return ERR_FAIL;

	if (nx_channel_group_GetGroupInfo(eNxChListGroup_AlphabetAll, &item) == ERR_OK){
		HLIB_VECTOR_Add(dstList, item);
		nCnt++;
	}
	if (nx_channel_group_GetGroupInfo(eNxChListGroup_AlphabetA, &item) == ERR_OK){
		HLIB_VECTOR_Add(dstList, item);
		nCnt++;
	}
	if (nx_channel_group_GetGroupInfo(eNxChListGroup_AlphabetB, &item) == ERR_OK){
		HLIB_VECTOR_Add(dstList, item);
		nCnt++;
	}
	if (nx_channel_group_GetGroupInfo(eNxChListGroup_AlphabetC, &item) == ERR_OK){
		HLIB_VECTOR_Add(dstList, item);
		nCnt++;
	}
	if (nx_channel_group_GetGroupInfo(eNxChListGroup_AlphabetD, &item) == ERR_OK){
		HLIB_VECTOR_Add(dstList, item);
		nCnt++;
	}
	if (nx_channel_group_GetGroupInfo(eNxChListGroup_AlphabetE, &item) == ERR_OK){
		HLIB_VECTOR_Add(dstList, item);
		nCnt++;
	}
	if (nx_channel_group_GetGroupInfo(eNxChListGroup_AlphabetF, &item) == ERR_OK){
		HLIB_VECTOR_Add(dstList, item);
		nCnt++;
	}
	if (nx_channel_group_GetGroupInfo(eNxChListGroup_AlphabetG, &item) == ERR_OK){
		HLIB_VECTOR_Add(dstList, item);
		nCnt++;
	}
	if (nx_channel_group_GetGroupInfo(eNxChListGroup_AlphabetH, &item) == ERR_OK){
		HLIB_VECTOR_Add(dstList, item);
		nCnt++;
	}
	if (nx_channel_group_GetGroupInfo(eNxChListGroup_AlphabetI, &item) == ERR_OK){
		HLIB_VECTOR_Add(dstList, item);
		nCnt++;
	}
	if (nx_channel_group_GetGroupInfo(eNxChListGroup_AlphabetJ, &item) == ERR_OK){
		HLIB_VECTOR_Add(dstList, item);
		nCnt++;
	}
	if (nx_channel_group_GetGroupInfo(eNxChListGroup_AlphabetK, &item) == ERR_OK){
		HLIB_VECTOR_Add(dstList, item);
		nCnt++;
	}
	if (nx_channel_group_GetGroupInfo(eNxChListGroup_AlphabetL, &item) == ERR_OK){
		HLIB_VECTOR_Add(dstList, item);
		nCnt++;
	}
	if (nx_channel_group_GetGroupInfo(eNxChListGroup_AlphabetM, &item) == ERR_OK){
		HLIB_VECTOR_Add(dstList, item);
		nCnt++;
	}
	if (nx_channel_group_GetGroupInfo(eNxChListGroup_AlphabetN, &item) == ERR_OK){
		HLIB_VECTOR_Add(dstList, item);
		nCnt++;
	}
	if (nx_channel_group_GetGroupInfo(eNxChListGroup_AlphabetO, &item) == ERR_OK){
		HLIB_VECTOR_Add(dstList, item);
		nCnt++;
	}
	if (nx_channel_group_GetGroupInfo(eNxChListGroup_AlphabetP, &item) == ERR_OK){
		HLIB_VECTOR_Add(dstList, item);
		nCnt++;
	}
	if (nx_channel_group_GetGroupInfo(eNxChListGroup_AlphabetQ, &item) == ERR_OK){
		HLIB_VECTOR_Add(dstList, item);
		nCnt++;
	}
	if (nx_channel_group_GetGroupInfo(eNxChListGroup_AlphabetR, &item) == ERR_OK){
		HLIB_VECTOR_Add(dstList, item);
		nCnt++;
	}
	if (nx_channel_group_GetGroupInfo(eNxChListGroup_AlphabetS, &item) == ERR_OK){
		HLIB_VECTOR_Add(dstList, item);
		nCnt++;
	}
	if (nx_channel_group_GetGroupInfo(eNxChListGroup_AlphabetT, &item) == ERR_OK){
		HLIB_VECTOR_Add(dstList, item);
		nCnt++;
	}
	if (nx_channel_group_GetGroupInfo(eNxChListGroup_AlphabetU, &item) == ERR_OK){
		HLIB_VECTOR_Add(dstList, item);
		nCnt++;
	}
	if (nx_channel_group_GetGroupInfo(eNxChListGroup_AlphabetV, &item) == ERR_OK){
		HLIB_VECTOR_Add(dstList, item);
		nCnt++;
	}
	if (nx_channel_group_GetGroupInfo(eNxChListGroup_AlphabetW, &item) == ERR_OK){
		HLIB_VECTOR_Add(dstList, item);
		nCnt++;
	}
	if (nx_channel_group_GetGroupInfo(eNxChListGroup_AlphabetX, &item) == ERR_OK){
		HLIB_VECTOR_Add(dstList, item);
		nCnt++;
	}
	if (nx_channel_group_GetGroupInfo(eNxChListGroup_AlphabetY, &item) == ERR_OK){
		HLIB_VECTOR_Add(dstList, item);
		nCnt++;
	}
	if (nx_channel_group_GetGroupInfo(eNxChListGroup_AlphabetZ, &item) == ERR_OK){
		HLIB_VECTOR_Add(dstList, item);
		nCnt++;
	}

	*nNum = nCnt;

	return ERR_OK;
}


HINT32		NX_CHANNEL_GROUP_GetGroupCount(NX_ChListGroup_t eChGroupType)
{
	HBOOL		bIsFavGroup = FALSE;
	HINT32		retCount = 0;
	HUINT32 	ulOperatorId = CLIB_OP_NULL, ulContentsType = 0, ulExtraFilter = 0;
	NX_ChannelManager_t *stInstance = NULL;
	HxVector_t			*pstGroupList = NULL;
	NX_FavGroupID_e 	eFavGroupID = eNxFavGroupID_None;

	/* Favourite Group Process */
	bIsFavGroup = NX_CHANNEL_GROUP_IsFavouriteGroup(eChGroupType);
	if (bIsFavGroup == TRUE)
	{
		NX_APP_Info("NX_CHANNEL_GROUP_IsFavouriteGroup() - eChGroupType(0x%x) !! \n", eChGroupType);
		eFavGroupID = NX_CHANNEL_FAV_ConvertChannelFavGroupToFavGroup(eChGroupType);
		if (eFavGroupID != eNxFavGroupID_None)
		{
			retCount = NX_CHANNEL_FAV_GetFavouriteNum(eFavGroupID);
		}
		else
		{
			NX_APP_Error("Error!! NX_CHANNEL_FAV_ConvertChannelFavGroupToFavGroup() - eChGroupType(0x%x) !! \n", eChGroupType);
			retCount = 0;
		}

		return	retCount;
	}

	/* Normal Group Process */
	ulOperatorId = nx_channel_GetSpecificInformation();
	ulExtraFilter = NX_CHANNEL_GROUP_GetExtraFilterByGroupType(eChGroupType);

	stInstance = nx_channel_GetManager(s_firstInstance);
	if ((stInstance == NULL) || (stInstance->list == NULL))
	{
		HxLOG_Critical("\n[%s.%d] Channel Manager instance is NULL !!\n",__FUNCTION__,__LINE__);
		return 0;
	}

	ulContentsType = nx_channel_ConvertNxChannelGroupToCLIBGroupType(eChGroupType);
	NX_APP_Info("===== [CLIB_GenerateList] eChGroupType:[0x%x]-ulContentsType[0x%x], ulOperatorId:[0x%x], ulExtraFilter:(0x%x) !! \n", eChGroupType, ulContentsType, ulOperatorId, ulExtraFilter);

	pstGroupList = CLIB_GenerateList(stInstance->list, NULL, ulContentsType, ulOperatorId, ulExtraFilter, TRUE);
	if (pstGroupList != NULL)
	{
		retCount = HLIB_VECTOR_Length(pstGroupList);
	}
	else
	{
		retCount = 0;
		NX_APP_Error("ERROR!!! CLIB_GenerateList() is NULL \n");
	}

	if (pstGroupList)
	{
		HLIB_VECTOR_Delete(pstGroupList);
		pstGroupList = NULL;
	}

	return retCount;
}

HUINT32		NX_CHANNEL_GROUP_GetExtraFilterByGroupType(NX_ChListGroup_t eChGroupType)
{
	HUINT32		ulExtraFilter = 0;

	switch (eChGroupType)
	{
		case eNxChListGroup_TV:
		case eNxChListGroup_RADIO:
		case eNxChListGroup_HDTV:
		case eNxChListGroup_Favorite1:
		case eNxChListGroup_Favorite2:
		case eNxChListGroup_Favorite3:
		case eNxChListGroup_Favorite4:
		case eNxChListGroup_Favorite5:
			break;
		case eNxChListGroup_FTA:
			ulExtraFilter = eDxCAS_TYPE_FTA;
			break;
		case eNxChListGroup_CAS:
			ulExtraFilter = 0xFFFE;
			break;
		case eNxChListGroup_ALPHABET:
		case eNxChListGroup_AlphabetAll:
		case eNxChListGroup_AlphabetA:
		case eNxChListGroup_AlphabetB:
		case eNxChListGroup_AlphabetC:
		case eNxChListGroup_AlphabetD:
		case eNxChListGroup_AlphabetE:
		case eNxChListGroup_AlphabetF:
		case eNxChListGroup_AlphabetG:
		case eNxChListGroup_AlphabetH:
		case eNxChListGroup_AlphabetI:
		case eNxChListGroup_AlphabetJ:
		case eNxChListGroup_AlphabetK:
		case eNxChListGroup_AlphabetL:
		case eNxChListGroup_AlphabetM:
		case eNxChListGroup_AlphabetN:
		case eNxChListGroup_AlphabetO:
		case eNxChListGroup_AlphabetP:
		case eNxChListGroup_AlphabetQ:
		case eNxChListGroup_AlphabetR:
		case eNxChListGroup_AlphabetS:
		case eNxChListGroup_AlphabetT:
		case eNxChListGroup_AlphabetU:
		case eNxChListGroup_AlphabetV:
		case eNxChListGroup_AlphabetW:
		case eNxChListGroup_AlphabetX:
		case eNxChListGroup_AlphabetY:
		case eNxChListGroup_AlphabetZ:
		case eNxChListGroup_AlphabetDigit:
		case eNxChListGroup_AlphabetEtc:
			ulExtraFilter = nx_channel_ConvertNxChannelAlpabetGroupToCLIBAlphabetGroupFilter(eChGroupType);
			break;

		default:
			NX_APP_Info("Error!!! Not Support Group Item [0x%x] !!\n", eChGroupType);
			break;
	}

	return ulExtraFilter;
}

HERROR	NX_CHANNEL_GROUP_FindChannelBySvcUidInGroup(NX_ChListGroup_t eChGroupType, HUINT32 svcUid, NX_Channel_t *retChannel)
{
	HxVector_t		*pListItem		= NULL;
	HINT32			nListItemNum	= 0;
	HUINT32			unFindSvcUid	= svcUid;
	CListItem_t 	*pstFindListItem = NULL;
	HERROR			retErr = ERR_FAIL;

	if (NX_CHANNEL_GROUP_IsFavouriteGroup(eChGroupType))
	{
		NX_FavGroupID_e eFavID = NX_CHANNEL_FAV_ConvertChannelFavGroupToFavGroup(eChGroupType);
		if(eFavID == eNxFavGroupID_None)
			goto END_FUNC;

		return NX_CHANNEL_FAV_FindChannelBySvcUidInFavGroup(eFavID, svcUid, retChannel);
	}

	if (nx_channel_group_MakeNxChannelListItemByGroup(eChGroupType, &pListItem, &nListItemNum) != ERR_OK)
	{
		goto END_FUNC;
	}

	pstFindListItem = HLIB_VECTOR_Find(pListItem, nx_channel_FindListItemBySvcUidCB, (void*)unFindSvcUid);

	if (pstFindListItem == NULL)
	{
		goto END_FUNC;
	}

	retChannel->svcUid	= pstFindListItem->uid;
	retChannel->lcn		= pstFindListItem->nLogicalNo;

	retErr = ERR_OK;

END_FUNC:
	if (pListItem)
	{
		HLIB_VECTOR_Delete(pListItem);
		pListItem = NULL;
	}

	return retErr;

}

DxService_t *NX_CHANNEL_GROUP_FindServiceInGroup(NX_ChListGroup_t eChGroupType, NX_FindSvcDir_e eFindDir, NX_Channel_t *pstCurrentChannel)
{
	HxVector_t		*pListItem 		= NULL;
	HINT32			nListItemNum	= 0, i = 0;
	DxService_t		*retFoundService= NULL;
	HINT32			nFindSvcIndex = -1;

	if (NX_CHANNEL_GROUP_IsFavouriteGroup(eChGroupType))
	{
		NX_FavGroupID_e eFavID = NX_CHANNEL_FAV_ConvertChannelFavGroupToFavGroup(eChGroupType);
		if(eFavID == eNxFavGroupID_None)
			goto END_FUNC;

		return NX_CHANNEL_FAV_FindServiceInFavGroup(eFavID, eFindDir, pstCurrentChannel);
	}

	if (nx_channel_group_MakeNxChannelListItemByGroup(eChGroupType, &pListItem, &nListItemNum) != ERR_OK)
	{
		retFoundService = NULL;
		goto END_FUNC;
	}

	for (i = 0; i < nListItemNum; i++)
	{
		CListItem_t *item = (CListItem_t*)HLIB_VECTOR_ItemAt(pListItem, i);

		if ((item->uid 			== pstCurrentChannel->svcUid)		&&
			(item->nLogicalNo 	== pstCurrentChannel->lcn))
		{
			switch(eFindDir)
			{
			case eNxFindSvcDir_Next:
				if ((i+1) < nListItemNum)
					nFindSvcIndex = i+1;
				break;

			case eNxFindSvcDir_Prev:
				if ((i-1) >= 0)
					nFindSvcIndex = i-1;
				break;

			case eNxFindSvcDir_Current:
				nFindSvcIndex = i;
				break;

			default:
				nFindSvcIndex = -1;
				break;
			}

			if(nFindSvcIndex >= 0)
			{
				CListItem_t *foundItem = (CListItem_t*)HLIB_VECTOR_ItemAt(pListItem, nFindSvcIndex);
				retFoundService = NX_CHANNEL_FindServiceBySvcUid(foundItem->uid);
			}
			goto END_FUNC;
		}
	}

END_FUNC:
	if (pListItem)
	{
		HLIB_VECTOR_Delete(pListItem);
		pListItem = NULL;
	}

	return retFoundService;
}


DxService_t *NX_CHANNEL_GROUP_FindServiceInGroupByServiceUid(NX_ChListGroup_t eChGroupType, NX_FindSvcDir_e eFindDir, HUINT32 ulSvcUid)
{
	HxVector_t		*pListItem = NULL;
	HINT32			nListItemNum = 0, i = 0;
	CListItem_t 	*stfoundItem = NULL;
	DxService_t		*retFoundService = NULL;
	HINT32			nFindSvcIndex = -1;

	if (NX_CHANNEL_GROUP_IsFavouriteGroup(eChGroupType))
	{
		NX_FavGroupID_e eFavID = NX_CHANNEL_FAV_ConvertChannelFavGroupToFavGroup(eChGroupType);
		if(eFavID == eNxFavGroupID_None)
			goto END_FUNC;

		return NX_CHANNEL_FAV_FindServiceInFavGroupByServiceUid(eFavID, eFindDir, ulSvcUid);
	}

	if (nx_channel_group_MakeNxChannelListItemByGroup(eChGroupType, &pListItem, &nListItemNum) != ERR_OK)
	{
		retFoundService = NULL;
		goto END_FUNC;
	}

	if (nListItemNum == 0)
	{
		retFoundService = NULL;
		goto END_FUNC;
	}

	if ((eFindDir == eNxFindSvcDir_First) || (eFindDir == eNxFindSvcDir_Last))
	{
		if (eFindDir == eNxFindSvcDir_First)
		{
			nFindSvcIndex = 0;
		}
		else
		{
			nFindSvcIndex = nListItemNum - 1;
		}

		stfoundItem = (CListItem_t*)HLIB_VECTOR_ItemAt(pListItem, nFindSvcIndex);
		if (stfoundItem)
		{
			retFoundService = NX_CHANNEL_FindServiceBySvcUid(stfoundItem->uid);
		}
		else
		{
			retFoundService = NULL;
		}

		goto END_FUNC;
	}

	for (i = 0; i < nListItemNum; i++)
	{
		CListItem_t *item = (CListItem_t*)HLIB_VECTOR_ItemAt(pListItem, i);

		if (item->uid == ulSvcUid)
		{
			switch(eFindDir)
			{
			case eNxFindSvcDir_Next:
				if ((i+1) < nListItemNum)
					nFindSvcIndex = i+1;
				break;

			case eNxFindSvcDir_Prev:
				if ((i-1) >= 0)
					nFindSvcIndex = i-1;
				break;

			case eNxFindSvcDir_Current:
				nFindSvcIndex = i;
				break;

			default:
				nFindSvcIndex = -1;
				break;
			}

			if (nFindSvcIndex >= 0)
			{
				stfoundItem = (CListItem_t*)HLIB_VECTOR_ItemAt(pListItem, nFindSvcIndex);
				if (stfoundItem)
				{
					retFoundService = NX_CHANNEL_FindServiceBySvcUid(stfoundItem->uid);
				}
				else
				{
					retFoundService = NULL;
				}
			}
			goto END_FUNC;
		}
	}

END_FUNC:
	if (pListItem)
	{
		HLIB_VECTOR_Delete(pListItem);
		pListItem = NULL;
	}

	return retFoundService;
}


DxService_t *NX_CHANNEL_GROUP_FindAvailableServiceInGroupList(NX_ChListGroup_t eChGroupType, HUINT32 ulSvcUid, NX_ChListGroup_t *peFindChGroupType)
{
	DxSvcType_e		svcType = eDxSVC_TYPE_TV;
	DxService_t		*pstFoundService = NULL, *pstServiceInfo = NULL;
	NX_ChListGroup_t eFindChGroupType = eNxChListGroup_TV;


	eFindChGroupType = eChGroupType;

	// Get Current Svc Type TV & Radio
	pstServiceInfo = NX_CHANNEL_FindServiceBySvcUid(ulSvcUid);
	if (pstServiceInfo)
	{
		svcType = pstServiceInfo->svcType;
	}

	// Find Service in current group with service uid
	pstFoundService = NX_CHANNEL_GROUP_FindServiceInGroupByServiceUid(eChGroupType, eNxFindSvcDir_Current, ulSvcUid);
	if (pstFoundService == NULL)
	{
		NX_APP_Error("Error!!! NX_CHANNEL_GROUP_FindServiceInGroupByServiceUid() eChGroupType:(0x%x), eNxFindSvcDir_Current,  ulSvcUid:(0x%x) Fail !!\n", eChGroupType, ulSvcUid);

		// Find Service in current Service Type Group with service uid
		eFindChGroupType = (svcType == eDxSVC_TYPE_TV) ? eNxChListGroup_TV : eNxChListGroup_RADIO ;
		pstFoundService = NX_CHANNEL_GROUP_FindServiceInGroupByServiceUid(eFindChGroupType, eNxFindSvcDir_Current, ulSvcUid);
		if (pstFoundService == NULL)
		{
			NX_APP_Error("Error!!! NX_CHANNEL_GROUP_FindServiceInGroupByServiceUid() eFindChGroupType:(0x%x), eNxFindSvcDir_Current,  ulSvcUid:(0x%x) Fail !!\n", eFindChGroupType, ulSvcUid);

			// Find Fisrst service TV Group
			eFindChGroupType = eNxChListGroup_TV;
			pstFoundService = NX_CHANNEL_GROUP_FindServiceInGroupByServiceUid(eFindChGroupType, eNxFindSvcDir_First, 0);
			if (pstFoundService == NULL)
			{
				NX_APP_Error("Error!!! NX_CHANNEL_GROUP_FindServiceInGroupByServiceUid() eFindChGroupType:(0x%x), eNxFindSvcDir_First,  ulSvcUid:(0x%x) Fail !!\n", eFindChGroupType, 0);

				// Find Fisrst service Radio Group
				eFindChGroupType = eNxChListGroup_RADIO;
				pstFoundService = NX_CHANNEL_GROUP_FindServiceInGroupByServiceUid(eFindChGroupType, eNxFindSvcDir_First, 0);
			}
		}
	}

	*peFindChGroupType = eFindChGroupType;

	if (pstFoundService == NULL)
	{
		*peFindChGroupType = eNxChListGroup_CURRENT;
		NX_APP_Error("Error!!! NX_CHANNEL_GROUP_FindServiceInGroupByServiceUid() Fail !!\n");
	}

	return	pstFoundService;
}


HBOOL		NX_CHANNEL_GROUP_GetGroupOfChannel(NX_ChListGroup_t *eRetGroup, NX_Channel_t *pstChannel)
{
	HBOOL bFound = FALSE;
	if (NX_CHANNEL_GROUP_IsServiceInGroup(eNxChListGroup_TV, pstChannel) == TRUE)
	{
		*eRetGroup = eNxChListGroup_TV;
		bFound = TRUE;
	}
	else
	if (NX_CHANNEL_GROUP_IsServiceInGroup(eNxChListGroup_RADIO, pstChannel) == TRUE)
	{
		*eRetGroup = eNxChListGroup_RADIO;
		bFound = TRUE;
	}
	return bFound;
}

HBOOL		NX_CHANNEL_GROUP_IsServiceInGroup(NX_ChListGroup_t eChGroupType, NX_Channel_t *pstChannel)
{
	HxVector_t		*pListItem = NULL;
	HINT32			lListItemNum = 0;
	CListItem_t 	*pstFindListItem = NULL;
	HBOOL			bRetIs = FALSE, bIsFavGroup = FALSE;
	NX_FavGroupID_e 	eFavGroupID = eNxFavGroupID_None;

	/* Favourite Group Process */
	bIsFavGroup = NX_CHANNEL_GROUP_IsFavouriteGroup(eChGroupType);
	if (bIsFavGroup == TRUE)
	{
		NX_APP_Info("NX_CHANNEL_GROUP_IsFavouriteGroup() - eChGroupType(0x%x) !! \n", eChGroupType);
		eFavGroupID = NX_CHANNEL_FAV_ConvertChannelFavGroupToFavGroup(eChGroupType);
		if (eFavGroupID != eNxFavGroupID_None)
		{
			bRetIs = NX_CHANNEL_FAV_IsFavouriteItem(eFavGroupID, pstChannel);
		}
		else
		{
			NX_APP_Error("Error!! NX_CHANNEL_FAV_ConvertChannelFavGroupToFavGroup() - eChGroupType(0x%x) !! \n", eChGroupType);
			bRetIs = FALSE;
		}

		return	bRetIs;
	}


	if (nx_channel_group_MakeNxChannelListItemByGroup(eChGroupType, &pListItem, &lListItemNum) != ERR_OK)
	{
		bRetIs = FALSE;
		goto END_FUNC;
	}

	pstFindListItem = HLIB_VECTOR_Find(pListItem, nx_channel_FindListItemByNxChannel, (void*)pstChannel);

	if (pstFindListItem)
	{
		HxLOG_Debug("\nFound Channel. uid(%d) , lcn(%d)\n", pstFindListItem->uid, pstFindListItem->nLogicalNo);
		bRetIs = TRUE;
	}

END_FUNC:
	if (pListItem)
	{
		HLIB_VECTOR_Delete(pListItem);
		pListItem = NULL;
	}

	return bRetIs;
}


HBOOL	NX_CHANNEL_GROUP_IsFavouriteGroup(NX_ChListGroup_t eChGroupType)
{
	HBOOL	bIsFavGroup = FALSE;

	switch (eChGroupType)
	{
		case eNxChListGroup_Favorite1:
		case eNxChListGroup_Favorite2:
		case eNxChListGroup_Favorite3:
		case eNxChListGroup_Favorite4:
		case eNxChListGroup_Favorite5:
			bIsFavGroup = TRUE;
			break;

		default:
			bIsFavGroup = FALSE;
			break;
	}

	return	bIsFavGroup;
}


HERROR		NX_CHANNEL_GROUP_MakeNxChannelListGroup(NX_Channel_t **ddstChannelList, HINT32 *nListNum, NX_ChListGroup_t eChGroupType, HUINT32 ulExtraFilter)
{
	HINT32			i = 0, svcNum = 0, ulGroupLen = 0;
	HxVector_t		*pstGroupList = NULL;
	NX_Channel_t	*pstRetChannelList = NULL;

	if (NX_CHANNEL_GROUP_IsFavouriteGroup(eChGroupType))
	{
		NX_FavGroupID_e eFavID = NX_CHANNEL_FAV_ConvertChannelFavGroupToFavGroup(eChGroupType);
		if(eFavID == eNxFavGroupID_None)
			goto END_FUNC;

		return NX_CHANNEL_FAV_MakeNxChannelListByFavGroup(eFavID, ddstChannelList, nListNum);
	}

	if (nx_channel_group_MakeNxChannelListItemByGroup(eChGroupType, &pstGroupList, &ulGroupLen) != ERR_OK)
	{
		HxLOG_Print("Group list is 0. \n");
		goto END_FUNC;
	}

	pstRetChannelList = NX_APP_Calloc(sizeof(NX_Channel_t) * ulGroupLen);

	for (i = 0; i < ulGroupLen; i++)
	{
		CListItem_t *data = (CListItem_t*)HLIB_VECTOR_ItemAt(pstGroupList, i);
		if (data)
		{
			pstRetChannelList[i].svcUid = (HUINT32)data->uid;
			pstRetChannelList[i].lcn = (HUINT32)data->nLogicalNo;

			svcNum++;
		}
	}

END_FUNC:
	if (pstGroupList)
	{
		HLIB_VECTOR_Delete(pstGroupList);
		pstGroupList = NULL;
	}

	*nListNum = svcNum;
	*ddstChannelList = pstRetChannelList;

	return ERR_OK;
}


HERROR		NX_CHANNEL_GROUP_MakeNxChannelListAll(NX_Channel_t **ddstChannelList, HINT32 *nListNum)
{
	NX_ChannelManager_t 	*stInstance = NULL;
	int i = 0, len = 0, svcNum = 0;

	HxVector_t *pstTvList = NULL, *pstRadioList = NULL;
	HxVector_t *fullList = NULL;
	int tvLen = 0, radioLen = 0;
	HUINT32 ulOperatorId = CLIB_OP_NULL;

	NX_Channel_t *pstRetChannelList = NULL;


	ulOperatorId = nx_channel_GetSpecificInformation();
	stInstance = nx_channel_GetManager(s_firstInstance);

	if ((stInstance == NULL) || (stInstance->list == NULL))
	{
		HxLOG_Critical("\n[%s.%d] Channel Manager instance is NULL !!\n",__FUNCTION__,__LINE__);
		return ERR_FAIL;
	}

	pstTvList = CLIB_GenerateList(stInstance->list, NULL, CLIB_TYPE_TV, ulOperatorId, 0, TRUE);
	pstRadioList = CLIB_GenerateList(stInstance->list, NULL, CLIB_TYPE_RADIO, ulOperatorId, 0, TRUE);

	if (pstTvList != NULL)
		tvLen = HLIB_VECTOR_Length(pstTvList);
	if (pstRadioList != NULL)
		radioLen = HLIB_VECTOR_Length(pstRadioList);

	fullList = HLIB_VECTOR_NewEasy(tvLen+radioLen, NULL, NULL);

	for (i = 0; i < tvLen; i++)
		HLIB_VECTOR_Add(fullList, (void*)HLIB_VECTOR_ItemAt(pstTvList, i));
	for (i = 0; i < radioLen; i++)
		HLIB_VECTOR_Add(fullList, (void*)HLIB_VECTOR_ItemAt(pstRadioList, i));


	if (fullList != NULL)
		len = HLIB_VECTOR_Length(fullList);

	if (len <= 0)
	{
		HxLOG_Debug("\n[%s.%d] Load Group List is 0\n",__FUNCTION__,__LINE__);
		return ERR_FAIL;
	}

	pstRetChannelList = NX_APP_Calloc(sizeof(NX_Channel_t) * len);

	for (i = 0; i < len; i++)
	{
		CListItem_t *data = (CListItem_t*)HLIB_VECTOR_ItemAt(fullList, i);
		if (data)
		{
			pstRetChannelList[i].svcUid = (HUINT32)data->uid;
			pstRetChannelList[i].lcn = (HUINT32)data->nLogicalNo;

			svcNum++;
		}
	}

	if (pstTvList)
	{
		HLIB_VECTOR_Delete(pstTvList);
		pstTvList = NULL;
	}

	if (pstRadioList)
	{
		HLIB_VECTOR_Delete(pstRadioList);
		pstRadioList = NULL;
	}

	if (fullList)
	{
		HLIB_VECTOR_Delete(fullList);
		fullList = NULL;
	}

	*nListNum = svcNum;
	*ddstChannelList = pstRetChannelList;

	return ERR_OK;
}

HERROR		NX_CHANNEL_GROUP_MakeNxChannelListByList(HxList_t * pServiceList, NX_Channel_t **ddstChannelList, HINT32 *nListNum)
{
	HINT32	len = 0, i = 0;
	NX_Channel_t *pstRetChannelList = NULL;
	HxList_t	*list = NULL;
	DxService_t	*pTemp = NULL;

	len = HLIB_LIST_Length(pServiceList);

	pstRetChannelList = NX_APP_Calloc(sizeof(NX_Channel_t) * len);

	list = pServiceList;

	while(list)
	{
		pTemp = (DxService_t*)list->data;
		pstRetChannelList[i].lcn = pTemp->lcn;
		pstRetChannelList[i++].svcUid = pTemp->uid;
		list = list->next;
	}

	*nListNum = len;
	*ddstChannelList = pstRetChannelList;

	return ERR_OK;

}

#define ______________RELATED_FAVOURITE_GROUP____________________

static NX_FavouriteList_t	g_ArrFavList[NX_CORE_CHANNEL_SUPPORTED_FAV_NUM];

HINT32	nx_channel_fav_ConvertToIndexByFavID(NX_FavGroupID_e eFavId)
{
	HINT32 retIndex = 0;
	HINT32 nId = (HINT32)eFavId;

	switch(nId)
	{
	case eNxFavGroupID_User:		retIndex = 0;		break;
	case eNxFavGroupID_User+1:		retIndex = 1;		break;
	case eNxFavGroupID_User+2:		retIndex = 2;		break;
	case eNxFavGroupID_User+3:		retIndex = 3;		break;
	case eNxFavGroupID_User+4:		retIndex = 4;		break;

	default:
		retIndex = 0;		break;
	}
	return retIndex;
}

HCHAR *nx_channel_fav_GetFavName(HUID aUID)
{
	switch(aUID)
	{
	case eNxFavGroupID_User:		return "Favourite1";
	case eNxFavGroupID_User+1:		return "Favourite2";
	case eNxFavGroupID_User+2:		return "Favourite3";
	case eNxFavGroupID_User+3:		return "Favourite4";
	case eNxFavGroupID_User+4:		return "Favourite5";
	default:	return "Favourite1";
	}
}

void nx_channel_fav_LoadFavCB(const HCHAR* aName, HUID aUid, const void* aData, HUINT32 aSize, void* aSelf)
{
#if !defined(CONFIG_PROD_OS_EMULATOR) || defined(CONFIG_SINGLE_PROCESS_MULTI_THREAD)
	DAxIterator_t	iter;
	NX_FavouriteList_t	*pstStoreFavList = (NX_FavouriteList_t *)aSelf;
#endif

#if !defined(CONFIG_PROD_OS_EMULATOR) || defined(CONFIG_SINGLE_PROCESS_MULTI_THREAD)
	DAPI_InitIterator(&iter, aData, aSize, 0);

	while (DAPI_NextIterator(&iter))
	{
		DxGroupList_t* groupInfo = (DxGroupList_t*)iter.data;

		HxLOG_Debug("Iter.uid (%lld), self->uid (%lld)\n", iter.uid, pstStoreFavList->aUID);

 		if ((groupInfo != NULL) && (pstStoreFavList->aUID == iter.uid))
		{
			HxVector_t* groupList = CLIB_DeSerialize((CList_t*)groupInfo->pucData);

			pstStoreFavList->list 	= groupList;
			HxLOG_Debug("Completed Load. Fav Group(%lld), item len(%d)\n\n", iter.uid, HLIB_VECTOR_Length(groupList));

			break;
		}
    }
#endif
}

static void nx_channel_fav_PrinFavGroup(NX_FavGroupID_e eFavID)
{
	HINT32		i = 0 ,favDataIndex = 0, length = 0;
	HUID		aUID = 0;


	aUID = eFavID;
	favDataIndex = nx_channel_fav_ConvertToIndexByFavID(eFavID);

	if (g_ArrFavList[favDataIndex].list == NULL)
		return;

	if (g_ArrFavList[favDataIndex].list != NULL)
		length = HLIB_VECTOR_Length(g_ArrFavList[favDataIndex].list);
	else
		length = 0;

	NX_APP_Debug("\n[%s.%d] FAV ID : (%lld) , Length (%d)\n",__FUNCTION__,__LINE__, aUID, length);

	for (i = 0; i < length; i++)
	{
		CListItem_t* listItem = (CListItem_t*)HLIB_VECTOR_ItemAt(g_ArrFavList[favDataIndex].list, i);

		NX_APP_Debug("[%s.%d] item : (%d) : svcUid(%lld) : lcn (%d)\n",__FUNCTION__,__LINE__, i, listItem->uid, listItem->nLogicalNo);
	}
}

static HERROR	nx_channel_fav_UpdateFavGroup(HxVector_t *list, HUID aUID)
{
	HINT32			ssize = 0, dsize = 0;
    CList_t			*pstCList;
    DxGroupList_t	*groupList;

	pstCList = CLIB_Serialize(list, &ssize);
    dsize = sizeof(DxGroupList_t) + ssize;

	groupList = (DxGroupList_t*)NX_APP_Calloc(dsize);
    groupList->nSize = dsize - sizeof(DxGroupList_t);
    HxSTD_MemCopy(groupList->pucData, pstCList, ssize);
    HxSTD_StrNCpy(groupList->szGroupName, nx_channel_fav_GetFavName(aUID), (DxNAME_LEN-1));

	HxLOG_Debug("\ncommit favourite list. FavID(%lld)\n", aUID);
#if !defined(CONFIG_PROD_OS_EMULATOR) || defined(CONFIG_SINGLE_PROCESS_MULTI_THREAD)
    DAPI_Set(DxNAMEOF(DxGroupList_t), aUID, (const void*)groupList, dsize);
    NX_APP_Free(groupList);
	NX_APP_Free(pstCList);

	DAPI_Sync(DxNAMEOF(DxGroupList_t));
#endif

	return ERR_OK;
}

HERROR		NX_CHANNEL_FAV_Initialize(void)
{
	HINT32	i = 0;

	HxLOG_Print("\n\n====== Load Favourite Group List... ======\n\n");

	for (i=0; i < NX_CORE_CHANNEL_SUPPORTED_FAV_NUM; i++)
	{
		HxSTD_MemSet(&g_ArrFavList[i], 0, sizeof(NX_FavouriteList_t));
		g_ArrFavList[i].aUID = (eNxFavGroupID_User+i);
		NX_CHANNEL_FAV_LoadFavFromDB((eNxFavGroupID_User + i), &g_ArrFavList[i]);
#if !defined(CONFIG_PROD_OS_EMULATOR) || defined(CONFIG_SINGLE_PROCESS_MULTI_THREAD)
	    DAPI_Sync(DxNAMEOF(DxGroupList_t));
#endif
	}

	return ERR_OK;
}

HERROR 	NX_CHANNEL_FAV_UpdateFavouriteGroup(NX_FavGroupID_e eFavID, HxVector_t *pstList)
{
	HxVector_t 		*newFavList 	= NULL;
	CListItem_t 	*item 			= NULL;
	HINT32 			i = 0, favDataIndex = 0, nLength = 0;

	favDataIndex = nx_channel_fav_ConvertToIndexByFavID(eFavID);
	nLength = HLIB_VECTOR_Length(pstList);

	newFavList = HLIB_VECTOR_NewEasy(nLength, NULL, NULL);

	for (i = 0 ; i < nLength ; i++)
	{
		NX_Channel_t* listItem = (NX_Channel_t*)HLIB_VECTOR_ItemAt(pstList, i);

		item = (CListItem_t*)NX_APP_Calloc(sizeof(CListItem_t));
		item->uid 			= (HUID)listItem->svcUid;
		item->nLogicalNo 	= (HINT32)listItem->lcn;

		HLIB_VECTOR_Add(newFavList, item);
	}

	if (g_ArrFavList[favDataIndex].list != NULL)
	{
		HLIB_VECTOR_Delete(g_ArrFavList[favDataIndex].list);
		g_ArrFavList[favDataIndex].list = NULL;
	}

	g_ArrFavList[favDataIndex].list = newFavList;

#if defined(CONFIG_DEBUG)
	nx_channel_fav_PrinFavGroup(eFavID);
#endif

	// save
	nx_channel_fav_UpdateFavGroup(g_ArrFavList[favDataIndex].list, eFavID);

	return ERR_OK;
}

HERROR	NX_CHANNEL_FAV_IsUpdateFavouriteName(NX_FavGroupID_e eFavID, HBOOL *pbIsUpdate)
{
	HINT32 	favDataIndex = 0;
	HERROR 	herror = ERR_FAIL;
	HINT32		nValue = -1;

	if (pbIsUpdate == NULL)
	{
		return ERR_FAIL;
	}

	favDataIndex = nx_channel_fav_ConvertToIndexByFavID(eFavID);
	switch(favDataIndex)
	{
		case 0:
			herror = NX_PORT_DB_GetInt(USERDB_FAVOURITE1_NAME_UPDATE_FLAG_ID, &nValue);
			break;
		case 1:
			herror = NX_PORT_DB_GetInt(USERDB_FAVOURITE2_NAME_UPDATE_FLAG_ID, &nValue);
			break;
		case 2:
			herror = NX_PORT_DB_GetInt(USERDB_FAVOURITE3_NAME_UPDATE_FLAG_ID, &nValue);
			break;
		case 3:
			herror = NX_PORT_DB_GetInt(USERDB_FAVOURITE4_NAME_UPDATE_FLAG_ID, &nValue);
			break;
		case 4:
			herror = NX_PORT_DB_GetInt(USERDB_FAVOURITE5_NAME_UPDATE_FLAG_ID, &nValue);
			break;
		default:
			break;
	}

	*pbIsUpdate = (HBOOL)nValue;

	return ERR_OK;
}

HERROR 	NX_CHANNEL_FAV_GetFavouriteName(NX_FavGroupID_e eFavID, HCHAR *pszFavName)
{
	HINT32 	favDataIndex = 0;
	HCHAR	szData[NX_TEXT_SIZE_128] = {'\0',};
	HERROR 	herror = ERR_FAIL;

	if (pszFavName == NULL)
		return ERR_FAIL;

	favDataIndex = nx_channel_fav_ConvertToIndexByFavID(eFavID);
	switch(favDataIndex)
	{
		case 0:
			herror = NX_PORT_DB_GetStr(USERDB_FAVOURITE1_NAME_ID, szData, NX_TEXT_SIZE_128);
			break;
		case 1:
			herror = NX_PORT_DB_GetStr(USERDB_FAVOURITE2_NAME_ID, szData, NX_TEXT_SIZE_128);
			break;
		case 2:
			herror = NX_PORT_DB_GetStr(USERDB_FAVOURITE3_NAME_ID, szData, NX_TEXT_SIZE_128);
			break;
		case 3:
			herror = NX_PORT_DB_GetStr(USERDB_FAVOURITE4_NAME_ID, szData, NX_TEXT_SIZE_128);
			break;
		case 4:
			herror = NX_PORT_DB_GetStr(USERDB_FAVOURITE5_NAME_ID, szData, NX_TEXT_SIZE_128);
			break;
		default:
			break;
	}
	if (herror == ERR_FAIL)
	{
		HxSTD_sprintf(pszFavName, "%s", nx_channel_fav_GetFavName(eFavID));
		return ERR_OK;
	}

	HxSTD_sprintf(pszFavName, "%s", szData);

	return ERR_OK;
}


HERROR 	NX_CHANNEL_FAV_SetFavouriteName(NX_FavGroupID_e eFavID, HCHAR *pszName, HBOOL bUserUpdateFlag)
{
	HINT32 	favDataIndex = 0;

	favDataIndex = nx_channel_fav_ConvertToIndexByFavID(eFavID);
	switch(favDataIndex)
	{
		case 0:
			NX_PORT_DB_SetStr(USERDB_FAVOURITE1_NAME_ID, pszName);
			NX_PORT_DB_SetInt(USERDB_FAVOURITE1_NAME_UPDATE_FLAG_ID, bUserUpdateFlag);
			break;
		case 1:
			NX_PORT_DB_SetStr(USERDB_FAVOURITE2_NAME_ID, pszName);
			NX_PORT_DB_SetInt(USERDB_FAVOURITE2_NAME_UPDATE_FLAG_ID, bUserUpdateFlag);
			break;
		case 2:
			NX_PORT_DB_SetStr(USERDB_FAVOURITE3_NAME_ID, pszName);
			NX_PORT_DB_SetInt(USERDB_FAVOURITE3_NAME_UPDATE_FLAG_ID, bUserUpdateFlag);
			break;
		case 3:
			NX_PORT_DB_SetStr(USERDB_FAVOURITE4_NAME_ID, pszName);
			NX_PORT_DB_SetInt(USERDB_FAVOURITE4_NAME_UPDATE_FLAG_ID, bUserUpdateFlag);
			break;
		case 4:
			NX_PORT_DB_SetStr(USERDB_FAVOURITE5_NAME_ID, pszName);
			NX_PORT_DB_SetInt(USERDB_FAVOURITE5_NAME_UPDATE_FLAG_ID, bUserUpdateFlag);
			break;
		default:	break;
	}

	return ERR_OK;
}


HINT32		NX_CHANNEL_FAV_GetFavouriteNum(NX_FavGroupID_e eFavID)
{
	HINT32			favDataIndex = 0;
	HUID 			aUID = 0;

	aUID = eFavID;
	favDataIndex = nx_channel_fav_ConvertToIndexByFavID(eFavID);

	if (g_ArrFavList[favDataIndex].list == NULL)
	{
		HxLOG_Debug("Not load Favourite list - favDataIndex(%d)\n", favDataIndex);
		return 0;
	}

	return HLIB_VECTOR_Length(g_ArrFavList[favDataIndex].list);
}


HERROR		NX_CHANNEL_FAV_LoadFavFromDB(NX_FavGroupID_e eFavID, NX_FavouriteList_t *pstFavList)
{
#if !defined(CONFIG_PROD_OS_EMULATOR) || defined(CONFIG_SINGLE_PROCESS_MULTI_THREAD)
	HUID aUID = eFavID;
#endif

#if !defined(CONFIG_PROD_OS_EMULATOR) || defined(CONFIG_SINGLE_PROCESS_MULTI_THREAD)
	if (aUID)
	{
		return DAPI_Get(DxNAMEOF(DxGroupList_t), aUID, nx_channel_fav_LoadFavCB, pstFavList);
	}
	else
	{
		return DAPI_GetAll(DxNAMEOF(DxGroupList_t), nx_channel_fav_LoadFavCB, pstFavList);
	}
#endif
	return ERR_OK;
}

HERROR		NX_CHANNEL_FAV_AddFavouriteItem(NX_FavGroupID_e eFavID, NX_Channel_t *pstChannel)
{
	HxVector_t		*newFavList 	= NULL;
	CListItem_t 	*item			= NULL;
	HINT32			i = 0 ,favDataIndex = 0, length = 0;
	HUID 			aUID = 0;


	aUID = eFavID;
	favDataIndex = nx_channel_fav_ConvertToIndexByFavID(eFavID);


	if (g_ArrFavList[favDataIndex].list != NULL)
		length = HLIB_VECTOR_Length(g_ArrFavList[favDataIndex].list);
	else
		length = 0;

	// 기존 데이터 리스트를 복사 (추가할 아이템개수를 더해 alloc)
	newFavList = HLIB_VECTOR_NewEasy(length + 1, NULL, NULL);

	for (i = 0; i < length; i++)
	{
		CListItem_t* listItem = (CListItem_t*)HLIB_VECTOR_ItemAt(g_ArrFavList[favDataIndex].list, i);

		item = (CListItem_t*)NX_APP_Calloc(sizeof(CListItem_t));
		item->uid		= listItem->uid;
		item->nLogicalNo= listItem->nLogicalNo;

		HLIB_VECTOR_Add(newFavList, item);
	}

	item = (CListItem_t*)NX_APP_Calloc(sizeof(CListItem_t));
	item->uid		= (HUID)pstChannel->svcUid;
	item->nLogicalNo= (HINT32)pstChannel->lcn;
	HLIB_VECTOR_Add(newFavList, item);

	// 기존 list 를 지우고.
	if (g_ArrFavList[favDataIndex].list != NULL)
	{
		HLIB_VECTOR_Delete(g_ArrFavList[favDataIndex].list);
		g_ArrFavList[favDataIndex].list = NULL;
	}

	g_ArrFavList[favDataIndex].list = newFavList;

#if defined(CONFIG_DEBUG)
	nx_channel_fav_PrinFavGroup(eFavID);
#endif

	// save
	nx_channel_fav_UpdateFavGroup(g_ArrFavList[favDataIndex].list, aUID);

	return ERR_OK;
}


HERROR	NX_CHANNEL_FAV_RemoveFavouriteItem(NX_FavGroupID_e eFavID, NX_Channel_t *pstChannel)
{
	HUID	aUID = eFavID;
	HINT32 	favDataIndex = 0, nRemovedNo = 0;

	favDataIndex = nx_channel_fav_ConvertToIndexByFavID(eFavID);
	if (g_ArrFavList[favDataIndex].list == NULL)
	{
		HxLOG_Debug("Not load Favourite list - favDataIndex(%d)\n", favDataIndex);
		return ERR_FAIL;
	}

	nRemovedNo = HLIB_VECTOR_RemoveIf(g_ArrFavList[favDataIndex].list, nx_channel_FindListItemByNxChannel, (void*)pstChannel);
	if (nRemovedNo > 0)
	{
		HxLOG_Debug("Successed Remove Item (%d).\n", nRemovedNo);

	#if defined(CONFIG_DEBUG)
		nx_channel_fav_PrinFavGroup(eFavID);
	#endif

		// update
		nx_channel_fav_UpdateFavGroup(g_ArrFavList[favDataIndex].list, aUID);
		return ERR_OK;
	}

	HxLOG_Debug("Failed Remove Item.\n");
	return ERR_FAIL;

}

HERROR	NX_CHANNEL_FAV_FindChannelBySvcUidInFavGroup(NX_FavGroupID_e eFavID, HUINT32 svcUid, NX_Channel_t *retDstChannel)
{
	HINT32		lFavDataIndex = 0, lFavNum = 0;
	CListItem_t *pstFindListItem = NULL;


	lFavNum = NX_CHANNEL_FAV_GetFavouriteNum(eFavID);
	if (lFavNum < 1)
	{
		HxLOG_Debug("Favourite list : (0x%x) was Zero\n", eFavID);
		return ERR_FAIL;
	}

	lFavDataIndex = nx_channel_fav_ConvertToIndexByFavID(eFavID);
	if (g_ArrFavList[lFavDataIndex].list == NULL)
	{
		HxLOG_Debug("Not load Favourite list - favDataIndex(%d)\n", lFavDataIndex);
		return ERR_FAIL;
	}

	pstFindListItem = HLIB_VECTOR_Find(g_ArrFavList[lFavDataIndex].list, nx_channel_FindListItemBySvcUidCB, (void*)svcUid);

	if (pstFindListItem != NULL)
	{
		HxLOG_Debug("Found Favourite Item. uid (%lld) , lcn(%d)\n", pstFindListItem->uid, pstFindListItem->nLogicalNo);
		retDstChannel->svcUid	= pstFindListItem->uid;
		retDstChannel->lcn		= pstFindListItem->nLogicalNo;
		return ERR_OK;
	}

	return ERR_FAIL;
}

DxService_t *NX_CHANNEL_FAV_FindServiceInFavGroup(NX_FavGroupID_e eFavID, NX_FindSvcDir_e eFindDir, NX_Channel_t *pstCurrentChannel)
{
	HINT32			nListItemNum	= 0, i = 0;
	DxService_t		*retFoundService= NULL;
	HINT32			nFindSvcIndex = -1;

	HINT32 		lFavDataIndex = 0, lFavNum = 0;

	lFavNum = NX_CHANNEL_FAV_GetFavouriteNum(eFavID);
	if (lFavNum < 1)
	{
		HxLOG_Debug("Favourite list : (0x%x) was Zero\n", eFavID);
		return NULL;
	}

	lFavDataIndex = nx_channel_fav_ConvertToIndexByFavID(eFavID);
	if (g_ArrFavList[lFavDataIndex].list == NULL)
	{
		HxLOG_Debug("Not load Favourite list - favDataIndex(%d)\n", lFavDataIndex);
		return NULL;
	}

	if(g_ArrFavList[lFavDataIndex].list != NULL)
		nListItemNum = HLIB_VECTOR_Length(g_ArrFavList[lFavDataIndex].list);
	else
		nListItemNum = 0;

	for (i = 0; i < nListItemNum; i++)
	{
		CListItem_t *item = (CListItem_t*)HLIB_VECTOR_ItemAt(g_ArrFavList[lFavDataIndex].list, i);

		if ((item->uid 			== pstCurrentChannel->svcUid)		&&
			(item->nLogicalNo 	== pstCurrentChannel->lcn))
		{
			switch(eFindDir)
			{
			case eNxFindSvcDir_Next:
				if ((i+1) < nListItemNum)
					nFindSvcIndex = i+1;
				break;

			case eNxFindSvcDir_Prev:
				if ((i-1) >= 0)
					nFindSvcIndex = i-1;
				break;

			case eNxFindSvcDir_Current:
				nFindSvcIndex = i;
				break;

			default:
				nFindSvcIndex = -1;
				break;
			}

			if(nFindSvcIndex >= 0)
			{
				CListItem_t *foundItem = (CListItem_t*)HLIB_VECTOR_ItemAt(g_ArrFavList[lFavDataIndex].list, nFindSvcIndex);
				retFoundService = NX_CHANNEL_FindServiceBySvcUid(foundItem->uid);
			}
			break;
		}
	}


	return retFoundService;
}

DxService_t *NX_CHANNEL_FAV_FindServiceInFavGroupByServiceUid(NX_FavGroupID_e eFavID, NX_FindSvcDir_e eFindDir, HUINT32 ulSvcUid)
{
	HINT32			nListItemNum = 0, i = 0;
	CListItem_t 	*stfoundItem = NULL;
	DxService_t		*retFoundService = NULL;
	HINT32			nFindSvcIndex = -1;

	HINT32 		lFavDataIndex = 0, lFavNum = 0;

	lFavNum = NX_CHANNEL_FAV_GetFavouriteNum(eFavID);
	if (lFavNum < 1)
	{
		HxLOG_Debug("Favourite list : (0x%x) was Zero\n", eFavID);
		return NULL;
	}

	lFavDataIndex = nx_channel_fav_ConvertToIndexByFavID(eFavID);
	if (g_ArrFavList[lFavDataIndex].list == NULL)
	{
		HxLOG_Debug("Not load Favourite list - favDataIndex(%d)\n", lFavDataIndex);
		return NULL;
	}

	if (g_ArrFavList[lFavDataIndex].list != NULL)
		nListItemNum = HLIB_VECTOR_Length(g_ArrFavList[lFavDataIndex].list);
	else
		nListItemNum = 0;


	if ((eFindDir == eNxFindSvcDir_First) || (eFindDir == eNxFindSvcDir_Last))
	{
		if (eFindDir == eNxFindSvcDir_First)
		{
			nFindSvcIndex = 0;
		}
		else
		{
			nFindSvcIndex = nListItemNum - 1;
		}

		stfoundItem = (CListItem_t*)HLIB_VECTOR_ItemAt(g_ArrFavList[lFavDataIndex].list, nFindSvcIndex);
		if (stfoundItem)
		{
			retFoundService = NX_CHANNEL_FindServiceBySvcUid(stfoundItem->uid);
		}
		else
		{
			retFoundService = NULL;
		}

		goto END_FUNC;
	}

	for (i = 0; i < nListItemNum; i++)
	{
		CListItem_t *item = (CListItem_t*)HLIB_VECTOR_ItemAt(g_ArrFavList[lFavDataIndex].list, i);

		if (item->uid == ulSvcUid)
		{
			switch(eFindDir)
			{
			case eNxFindSvcDir_Next:
				if ((i+1) < nListItemNum)
					nFindSvcIndex = i+1;
				break;

			case eNxFindSvcDir_Prev:
				if ((i-1) >= 0)
					nFindSvcIndex = i-1;
				break;

			case eNxFindSvcDir_Current:
				nFindSvcIndex = i;
				break;

			default:
				nFindSvcIndex = -1;
				break;
			}

			if (nFindSvcIndex >= 0)
			{
				stfoundItem = (CListItem_t*)HLIB_VECTOR_ItemAt(g_ArrFavList[lFavDataIndex].list, nFindSvcIndex);
				if (stfoundItem)
				{
					retFoundService = NX_CHANNEL_FindServiceBySvcUid(stfoundItem->uid);
				}
				else
				{
					retFoundService = NULL;
				}
			}
			goto END_FUNC;
		}
	}

END_FUNC:

	return retFoundService;
}


HBOOL	NX_CHANNEL_FAV_IsFavouriteItem(NX_FavGroupID_e eFavID, NX_Channel_t *pstSrcChannel)
{
	HINT32 		lFavDataIndex = 0, lFavNum = 0;
	CListItem_t	*pstFindListItem = NULL;


	lFavNum = NX_CHANNEL_FAV_GetFavouriteNum(eFavID);
	if (lFavNum < 1)
	{
		HxLOG_Debug("Favourite list : (0x%x) was Zero\n", eFavID);
		return FALSE;
	}

	lFavDataIndex = nx_channel_fav_ConvertToIndexByFavID(eFavID);
	if (g_ArrFavList[lFavDataIndex].list == NULL)
	{
		HxLOG_Debug("Not load Favourite list - favDataIndex(%d)\n", lFavDataIndex);
		return FALSE;
	}

	pstFindListItem = HLIB_VECTOR_Find(g_ArrFavList[lFavDataIndex].list, nx_channel_FindListItemByNxChannel, (void*)pstSrcChannel);

	if (pstFindListItem != NULL)
	{
		HxLOG_Debug("Found Favourite Item. uid (%lld) , lcn(%d)\n", pstFindListItem->uid, pstFindListItem->nLogicalNo);
		return TRUE;
	}

	HxLOG_Debug("Not found Favourite Item. uid(%d), lcn(%d)\n", pstSrcChannel->svcUid, pstSrcChannel->lcn);

	return FALSE;
}

void	NX_CHANNEL_FAV_RemoveFavouriteGroup(NX_FavGroupID_e eFavID)
{
#if !defined(CONFIG_PROD_OS_EMULATOR) || defined(CONFIG_SINGLE_PROCESS_MULTI_THREAD)
	HUID aUID = eFavID;
#endif

#if !defined(CONFIG_PROD_OS_EMULATOR) || defined(CONFIG_SINGLE_PROCESS_MULTI_THREAD)
	DAPI_Reset(DxNAMEOF(DxGroupList_t), aUID);
#endif
}

HERROR	NX_CHANNEL_FAV_MakeNxChannelListByFavGroup(NX_FavGroupID_e eFavID, NX_Channel_t **ddstChannelList, HINT32 *nListNum)
{
	HINT32			i = 0 ,favDataIndex = 0, length = 0;
	HUID			aUID = 0;
	NX_Channel_t 	*pstRetChannelList = NULL;


	aUID = eFavID;
	favDataIndex = nx_channel_fav_ConvertToIndexByFavID(eFavID);

	if (g_ArrFavList[favDataIndex].list != NULL)
		length = HLIB_VECTOR_Length(g_ArrFavList[favDataIndex].list);
	else
		length = 0;

	if (length <= 0)
	{
		HxLOG_Debug("\nFavourite Group Item length is 0.\n");
		*nListNum = 0;
		*ddstChannelList = NULL;
		return ERR_FAIL;
	}

	pstRetChannelList = NX_APP_Calloc(sizeof(NX_Channel_t) * length);

	for (i = 0; i < length; i++)
	{
		CListItem_t* listItem = (CListItem_t*)HLIB_VECTOR_ItemAt(g_ArrFavList[favDataIndex].list, i);

		pstRetChannelList[i].svcUid	= listItem->uid;
		pstRetChannelList[i].lcn	= listItem->nLogicalNo;

		HxLOG_Debug("MakeFavChannel (%d) : svcUid(%d) : lcn (%d)\n", i,	pstRetChannelList[i].svcUid, pstRetChannelList[i].lcn);
	}

	*nListNum = length;
	*ddstChannelList = pstRetChannelList;

	return ERR_OK;
}


NX_FavGroupID_e		NX_CHANNEL_FAV_ConvertChannelFavGroupToFavGroup(NX_ChListGroup_t eChGroupType)
{
	NX_FavGroupID_e 	eFavID = eNxFavGroupID_None;

	switch (eChGroupType)
	{
		case eNxChListGroup_Favorite1:
			eFavID = eNxFavGroupID_User;
			break;
		case eNxChListGroup_Favorite2:
			eFavID = eNxFavGroupID_User + 1;
			break;
		case eNxChListGroup_Favorite3:
			eFavID = eNxFavGroupID_User + 2;
			break;
		case eNxChListGroup_Favorite4:
			eFavID = eNxFavGroupID_User + 3;
			break;
		case eNxChListGroup_Favorite5:
			eFavID = eNxFavGroupID_User + 4;
			break;
		default:
			NX_APP_Info("It is not Favourite channel list Group:(0x%x) \n", eChGroupType);
			eFavID = eNxFavGroupID_None;
			break;
	}

	return	eFavID;
}


HERROR	NX_CHANNEL_FAV_RemoveServiceInAllFavouriteGroup(DxService_t *pstService)
{
	HERROR				hError = ERR_FAIL;
	NX_Channel_t		stCurChannel;
	NX_FavGroupID_e 	eFavGroupID = eNxFavGroupID_None;

	// Find Channel Info
	HxSTD_memset(&stCurChannel, 0x00, sizeof(NX_Channel_t));
	hError = NX_CHANNEL_GROUP_FindChannelBySvcUidInGroup(eNxChListGroup_TV, pstService->uid, &stCurChannel);
	if (hError != ERR_OK)
	{
		NX_APP_Error("Error!!! NX_CHANNEL_GROUP_FindChannelBySvcUidInGroup() - eNxChListGroup_TV Svc Uid:(0x%x) Fail!! \n", pstService->uid);

		HxSTD_memset(&stCurChannel, 0x00, sizeof(NX_Channel_t));
		hError = NX_CHANNEL_GROUP_FindChannelBySvcUidInGroup(eNxChListGroup_RADIO, pstService->uid, &stCurChannel);
		if (hError != ERR_OK)
		{
			NX_APP_Error("Error!!! NX_CHANNEL_GROUP_FindChannelBySvcUidInGroup() - eNxChListGroup_RADIO Svc Uid:(0x%x) Fail!! \n", pstService->uid);
			return	ERR_FAIL;
		}
	}

	// Delete - eNxChListGroup_Favorite1
	eFavGroupID = NX_CHANNEL_FAV_ConvertChannelFavGroupToFavGroup(eNxChListGroup_Favorite1);
	if (eFavGroupID != eNxFavGroupID_None)
	{
		hError = NX_CHANNEL_FAV_RemoveFavouriteItem(eFavGroupID, &stCurChannel);
		if (hError == ERR_OK)
		{
			NX_APP_Debug("NX_CHANNEL_FAV_RemoveFavouriteItem() - eNxChListGroup_Favorite1\n");
		}
	}

	// Delete - eNxChListGroup_Favorite2
	eFavGroupID = NX_CHANNEL_FAV_ConvertChannelFavGroupToFavGroup(eNxChListGroup_Favorite2);
	if (eFavGroupID != eNxFavGroupID_None)
	{
		hError = NX_CHANNEL_FAV_RemoveFavouriteItem(eFavGroupID, &stCurChannel);
		if (hError == ERR_OK)
		{
			NX_APP_Debug("NX_CHANNEL_FAV_RemoveFavouriteItem() - eNxChListGroup_Favorite2\n");
		}
	}

	// Delete - eNxChListGroup_Favorite3
	eFavGroupID = NX_CHANNEL_FAV_ConvertChannelFavGroupToFavGroup(eNxChListGroup_Favorite3);
	if (eFavGroupID != eNxFavGroupID_None)
	{
		hError = NX_CHANNEL_FAV_RemoveFavouriteItem(eFavGroupID, &stCurChannel);
		if (hError == ERR_OK)
		{
			NX_APP_Debug("NX_CHANNEL_FAV_RemoveFavouriteItem() - eNxChListGroup_Favorite3\n");
		}
	}

	// Delete - eNxChListGroup_Favorite4
	eFavGroupID = NX_CHANNEL_FAV_ConvertChannelFavGroupToFavGroup(eNxChListGroup_Favorite4);
	if (eFavGroupID != eNxFavGroupID_None)
	{
		hError = NX_CHANNEL_FAV_RemoveFavouriteItem(eFavGroupID, &stCurChannel);
		if (hError == ERR_OK)
		{
			NX_APP_Debug("NX_CHANNEL_FAV_RemoveFavouriteItem() - eNxChListGroup_Favorite4\n");
		}
	}

	// Delete - eNxChListGroup_Favorite5
	eFavGroupID = NX_CHANNEL_FAV_ConvertChannelFavGroupToFavGroup(eNxChListGroup_Favorite5);
	if (eFavGroupID != eNxFavGroupID_None)
	{
		hError = NX_CHANNEL_FAV_RemoveFavouriteItem(eFavGroupID, &stCurChannel);
		if (hError == ERR_OK)
		{
			NX_APP_Debug("NX_CHANNEL_FAV_RemoveFavouriteItem() - eNxChListGroup_Favorite5\n");
		}
	}

	return	ERR_OK;
}


#define ______________RELATED_DCN_GROUP____________________
HBOOL	nx_channel_DCN_isCorrect(HUINT8 *p1, HUINT8 *p2)
{
	HINT32			nDcnLen = 0;
	HINT32			nCnt = 0;
	HINT32			nSameCnt = 0;

	if(p1 == NULL || p2 == NULL)
		return FALSE;

	nDcnLen = HxSTD_StrLen((char*)p1);
	if(nDcnLen == 0)
		return FALSE;

	for(nCnt = 0; nCnt < nDcnLen ; nCnt++)
	{
		if(p1[nCnt] == p2[nCnt])
		{
			nSameCnt++;
		}
		else
		{
			break;
		}
	}

	if(nSameCnt == nDcnLen) // same
		return TRUE;

	return FALSE;
}

HERROR	NX_CHANNEL_DCN_MakeNxChannelListByDCN(HUINT8 *pucDcn, NX_Channel_t **ddstChannelList, HINT32 *nListNum)
{
	HERROR			hError = ERR_FAIL;
	HINT32			lListItemNum = 0, i = 0, lItemNum = 0;
	HxVector_t		*pstListItemTv = NULL;
	HxVector_t		*pstListItemRa = NULL;
	CListItem_t 	*pstItem = NULL;
	NX_Channel_t	*pstRetChannelList = NULL;
	HxVector_t		*pResultList = HLIB_VECTOR_New(NULL, NULL);
	HUINT8			aucLCN[NX_TEXT_SIZE_8];

	if(pResultList == NULL)
	{
		HxLOG_Error("pResultList is NULL");
		return ERR_FAIL;
	}

	if(pucDcn == NULL)
	{
		return ERR_FAIL;
	}

	hError = nx_channel_group_MakeNxChannelListItemByGroup(eNxChListGroup_TV, &pstListItemTv, &lListItemNum);
	if ((hError == ERR_OK) && (lListItemNum > 0))
	{
		for (i = 0; i < lListItemNum; i++)
		{
			pstItem = (CListItem_t*)HLIB_VECTOR_ItemAt(pstListItemTv, i);
			if (pstItem)
			{
				HxSTD_MemSet(aucLCN, 0, NX_TEXT_SIZE_8);
				HxSTD_snprintf((char*)aucLCN, NX_TEXT_SIZE_8, (char*)"%d", pstItem->nLogicalNo);
				if(nx_channel_DCN_isCorrect(pucDcn, aucLCN) == TRUE)
				{
					if (pResultList)
					{
						HLIB_VECTOR_Add(pResultList, pstItem);
						lItemNum++;
					}
				}
			}
		}
	}

	hError = nx_channel_group_MakeNxChannelListItemByGroup(eNxChListGroup_RADIO, &pstListItemRa, &lListItemNum);
	if ((hError == ERR_OK) && (lListItemNum > 0))
	{
		for (i = 0; i < lListItemNum; i++)
		{
			pstItem = (CListItem_t*)HLIB_VECTOR_ItemAt(pstListItemRa, i);
			if (pstItem)
			{
				HxSTD_MemSet(aucLCN, 0, NX_TEXT_SIZE_8);
				HxSTD_snprintf((char*)aucLCN, NX_TEXT_SIZE_8, (char*)"%d", pstItem->nLogicalNo);
				if(nx_channel_DCN_isCorrect(pucDcn, aucLCN) == TRUE)
				{
					if (pResultList)
					{
						HLIB_VECTOR_Add(pResultList, pstItem);
						lItemNum++;
					}
				}
			}
		}
	}

	// make list
	pstRetChannelList = NX_APP_Calloc(sizeof(NX_Channel_t) * lItemNum);
	if(pstRetChannelList && pResultList)
	{
		for (i = 0; i < lItemNum; i++)
		{
			CListItem_t* listItem = (CListItem_t*)HLIB_VECTOR_ItemAt(pResultList, i);

			pstRetChannelList[i].svcUid = listItem->uid;
			pstRetChannelList[i].lcn = listItem->nLogicalNo;

//			HxLOG_Warning("MakeFavChannel (%d) : svcUid(%d) : lcn (%d)\n", i, pstRetChannelList[i].svcUid, pstRetChannelList[i].lcn);
		}

	}

	// sort
	NX_CHANNEL_GROUP_SortNxChannelsByLcn(pstRetChannelList, lItemNum);

	if (pstListItemTv)
	{
		HLIB_VECTOR_Delete(pstListItemTv);
		pstListItemTv = NULL;
	}

	if (pstListItemRa)
	{
		HLIB_VECTOR_Delete(pstListItemRa);
		pstListItemRa = NULL;
	}

	if (pResultList)
	{
		HLIB_VECTOR_Delete(pResultList);
		pResultList = NULL;
	}


	*nListNum = lItemNum;
	*ddstChannelList = pstRetChannelList;

	return	ERR_OK;
}

HERROR	NX_CHANNEL_DCN_MakeGroupNxChannelListByDCN(HUINT8 *pucDcn, NX_ChListGroup_t eChGroupType, NX_Channel_t **ddstChannelList, HINT32 *nListNum)
{
	HERROR			hError = ERR_FAIL;
	HINT32			lListItemNum = 0, i = 0, lItemNum = 0;
	HxVector_t		*pstListItemTv = NULL;
	CListItem_t 	*pstItem = NULL;
	NX_Channel_t	*pstRetChannelList = NULL;
	HxVector_t		*pResultList = HLIB_VECTOR_New(NULL, NULL);
	HUINT8			aucLCN[NX_TEXT_SIZE_8];

	if(pResultList == NULL)
	{
		HxLOG_Error("pResultList is NULL");
		return ERR_FAIL;
	}

	if(pucDcn == NULL)
	{
		return ERR_FAIL;
	}

	hError = nx_channel_group_MakeNxChannelListItemByGroup(eChGroupType, &pstListItemTv, &lListItemNum);
	if ((hError == ERR_OK) && (lListItemNum > 0))
	{
		for (i = 0; i < lListItemNum; i++)
		{
			pstItem = (CListItem_t*)HLIB_VECTOR_ItemAt(pstListItemTv, i);
			if (pstItem)
			{
				HxSTD_MemSet(aucLCN, 0, NX_TEXT_SIZE_8);
				HxSTD_snprintf((char*)aucLCN, NX_TEXT_SIZE_8, (char*)"%d", pstItem->nLogicalNo);
				if(nx_channel_DCN_isCorrect(pucDcn, aucLCN) == TRUE)
				{
					if (pResultList)
					{
						HLIB_VECTOR_Add(pResultList, pstItem);
						lItemNum++;
					}
				}
			}
		}
	}

	// make list
	pstRetChannelList = NX_APP_Calloc(sizeof(NX_Channel_t) * lItemNum);
	if(pstRetChannelList && pResultList)
	{
		for (i = 0; i < lItemNum; i++)
		{
			CListItem_t* listItem = (CListItem_t*)HLIB_VECTOR_ItemAt(pResultList, i);

			pstRetChannelList[i].svcUid = listItem->uid;
			pstRetChannelList[i].lcn = listItem->nLogicalNo;
//			HxLOG_Warning("MakeFavChannel (%d) : svcUid(%d) : lcn (%d)\n", i, pstRetChannelList[i].svcUid, pstRetChannelList[i].lcn);
		}

	}

	// sort
	NX_CHANNEL_GROUP_SortNxChannelsByLcn(pstRetChannelList, lItemNum);

	if (pstListItemTv)
	{
		HLIB_VECTOR_Delete(pstListItemTv);
		pstListItemTv = NULL;
	}

	if (pResultList)
	{
		HLIB_VECTOR_Delete(pResultList);
		pResultList = NULL;
	}


	*nListNum = lItemNum;
	*ddstChannelList = pstRetChannelList;

	return	ERR_OK;
}


#define ______________RELATED_CHANNEL_COMMAND____________________
/***************************************************************************
Related Search Command
***************************************************************************/
#if defined(CONFIG_DEBUG)

static HINT32 cmdStartPlay(void *szArgs)
{
	HUINT32 ulSessionId = 0;
	HUINT32 ulMainViewId = 0;
	OxMediaPlay_StartInfo_t 	 unStart;

	HUINT32 svcuid = 0;
	HUINT32 chnum = 0;

	char			 *aucArg=NULL;

	aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgs );
	if( aucArg != NULL )
	{
		if( sscanf(aucArg, "%d", &svcuid) == 0 )
			return HxCMD_ERR;
	}

	aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgs );
	if( aucArg != NULL )
	{
		if( sscanf(aucArg, "%d", &chnum) == 0 )
			return HxCMD_ERR;
	}

	NX_PORT_MEDIA_PLAY_GetMainViewId(&ulMainViewId);

	unStart.stLive.eType			= eOxMP_MEDIATYPE_LIVE;
	unStart.stLive.eLiveType		= eOxMP_LIVETYPE_MAIN;
	unStart.stLive.ulMasterSvcUid	= svcuid;
	unStart.stLive.ulSuppleSvcUid	= svcuid;
	unStart.stLive.eSuppSvcType		= eOxMP_SUPPLESVCTYPE_Normal;
	unStart.stLive.ulMajorCHNum		= chnum;
	unStart.stLive.eTuneMode		= 0x00;

	NX_PORT_MEDIA_PLAY_Start (ulMainViewId, &unStart, &ulSessionId);

	return HxCMD_OK;
}

static HINT32 cmdLoadGroup(void *szArgs)
{
	HUINT32 groupType = 0;

	char			 *aucArg=NULL;

	aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgs );
	if( aucArg != NULL )
	{
		if( sscanf(aucArg, "%d", &groupType) == 0 )
			return HxCMD_ERR;
	}

//	NX_CHANNEL_GROUP_LoadGroupList((HINT32)groupType);

	return HxCMD_OK;
}

static HINT32 cmdSaveChannel(void *szArgs)
{
	HUINT32 svcuid = 0;

	char			 *aucArg=NULL;

	aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgs );
	if( aucArg != NULL )
	{
		if( sscanf(aucArg, "%d", &svcuid) == 0 )
			return HxCMD_ERR;
	}

	NX_CHANNEL_SaveLastChannel((HINT32)svcuid);

	return HxCMD_OK;
}

static HINT32 cmdGetLastChannel(void *szArgs)
{
	NX_CHANNEL_GetLastService();
	NX_COMMON_NOT_USED_PARAM(szArgs);

	return HxCMD_OK;
}

static void nx_channel_Command_Init(void)
{
#define cmdHandle		"nxchannelcmd"

	HLIB_CMD_RegisterWord(cmdHandle, cmdStartPlay,
						 (HINT8 *)"live",
						 (HINT8 *)"start live",
						 (HINT8 *)"live");
	HLIB_CMD_RegisterWord(cmdHandle, cmdLoadGroup,
						(HINT8 *)"loadgroup",
						(HINT8 *)"load group, arg 1 : group type",
						(HINT8 *)"load group");
	HLIB_CMD_RegisterWord(cmdHandle, cmdSaveChannel,
						(HINT8 *)"savechannel",
						(HINT8 *)"save cahnnel by svcuid",
						(HINT8 *)"save channel");
	HLIB_CMD_RegisterWord(cmdHandle, cmdGetLastChannel,
						(HINT8 *)"getlastchannel",
						(HINT8 *)"get last service by ccid in db",
						(HINT8 *)"get last channel");
}

#endif		// #if defined(CONFIG_DEBUG)
