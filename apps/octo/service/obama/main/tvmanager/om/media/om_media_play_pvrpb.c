/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  om_media_play_pvrpb.c
	@brief

	Description:  									\n
	Module: TVMANAGER / OAPI Manager				\n

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
#include <obama_rpcprotocol.h>
#include <oapi.h>

#include <svc_av.h>
#include <svc_si.h>
#include <svc_cas.h>

#include <bus.h>
#include <mgr_common.h>
#include <mgr_action.h>
#include <mgr_live.h>
#include <mgr_cas.h>

#include <mgr_output.h>
#include <mgr_svclist.h>

#include "_om_media_play.h"

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/
#define ___LOCAL_FUNCTIONS___
#define _____MSG_PROCESS_FUNCTIONS_____
STATIC BUS_Result_t om_media_play_pvrpb_MsgAppPlaybackNotifyStarted (omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32					 ulViewId = 0;
	HUINT32					 ulActionId = SVC_PIPE_GetActionId(hAction);
	HUINT32					 ulSessionId = pstContext->ulSessionId;
	MgrPb_Info_t  			 stPbInfo;
	Handle_t				 hErr;

	if (pstContext->ulActionId != ulActionId)
	{
		return MESSAGE_PASS;
	}

	hErr = MGR_ACTION_GetViewIdByActionId(ulActionId, &ulViewId);
	if (ERR_OK != hErr)
	{
		return MESSAGE_PASS;
	}

	hErr = MGR_PLAYBACK_GetPlaybackInfo (ulActionId, &stPbInfo);
	if( ERR_OK == hErr )
	{
		if (/*WAIT_PB_LOCK*/4 == stPbInfo.nPbState)
		{
			om_media_play_CbOnPlayErrorChanged(ulViewId, ulSessionId, eOxMP_MEDIATYPE_PVRPLAYBACK, eOxMP_PLAYERROR_PLAY_ERROR, 0);
		}
		else
		{
			om_media_play_CbOnPlayStateChanged( ulViewId, ulSessionId, eOxMP_MEDIATYPE_PVRPLAYBACK, eOxMP_PLAYSTATE_PLAYING);
		}
	}

	NOT_USED_PROC_ARGUMENT(hAction, p1, p2, p3);
	return MESSAGE_PASS;
}

STATIC BUS_Result_t om_media_play_pvrpb_MsgAppPlaybackNotifyStopped (omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32					 ulViewId = 0;
	HUINT32					 ulActionId = SVC_PIPE_GetActionId(hAction);
//	HUINT32					 ulSessionId = pstContext->ulSessionId;
	Handle_t				 hErr;

	if (pstContext->ulActionId != ulActionId)
	{
		return MESSAGE_PASS;
	}

	hErr = MGR_ACTION_GetViewIdByActionId(ulActionId, &ulViewId);
	if (ERR_OK != hErr)
	{
		return MESSAGE_PASS;
	}

	/* clean cache */
	om_media_play_CleanCache(pstContext, eOxMP_MEDIATYPE_PVRPLAYBACK);

	om_media_play_CbOnPlayStateChanged(ulViewId, pstContext->ulSessionId, eOxMP_MEDIATYPE_PVRPLAYBACK, eOxMP_PLAYSTATE_STOPPED);
	NOT_USED_PROC_ARGUMENT(hAction, p1, p2, p3);
	return MESSAGE_PASS;
}

STATIC BUS_Result_t om_media_play_pvrpb_MsgAppPlaybackNotifyErrorStopped (omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	NOT_USED_PROC_ARGUMENT(hAction, p1, p2, p3);
	return MESSAGE_PASS;
}

STATIC BUS_Result_t om_media_play_pvrpb_MsgOutputAVStarted (omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32					 ulViewId = 0;
	HUINT32					 ulActionId = SVC_PIPE_GetActionId(hAction);
	HUINT32					 ulSessionId = pstContext->ulSessionId;
	Handle_t				 hErr;

	if (pstContext->ulActionId != ulActionId)
	{
		return MESSAGE_PASS;
	}

	hErr = MGR_ACTION_GetViewIdByActionId(ulActionId, &ulViewId);
	if (ERR_OK != hErr)
	{
		return MESSAGE_PASS;
	}

	om_media_play_CbOnAVStarted(ulViewId, ulSessionId, eOxMP_MEDIATYPE_PVRPLAYBACK);

	NOT_USED_PROC_ARGUMENT(hAction, p1, p2, p3);
	return MESSAGE_PASS;
}

STATIC BUS_Result_t om_media_play_pvrpb_EvtAvVideoStarted (omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32					 ulViewId = 0;
	HUINT32					 ulActionId = SVC_PIPE_GetActionId(hAction);
	HUINT32					 ulSessionId = pstContext->ulSessionId;
	VideoStreamInfo_t		 stVideoStreamInfo;
	OxMediaPlay_VideoStreamInfo_t	 stOapiVideoStreamInfo = {0,};
	Handle_t				 hErr;

	if (pstContext->ulActionId != ulActionId)
	{
		return MESSAGE_PASS;
	}

	hErr = MGR_ACTION_GetViewIdByActionId(ulActionId, &ulViewId);
	if (ERR_OK != hErr)
	{
		return MESSAGE_PASS;
	}


	hErr = SVC_AV_GetVideoStreamInfo(ulActionId, &stVideoStreamInfo);
	if (ERR_OK == hErr)
	{
		stOapiVideoStreamInfo.ulHSize = stVideoStreamInfo.ulHorizontalSize;
		stOapiVideoStreamInfo.ulVSize = stVideoStreamInfo.ulVerticalSize;
		stOapiVideoStreamInfo.eAspectRatio = stVideoStreamInfo.eAspectRatio;
	}

	om_media_play_CbOnVideoStreamInfo(ulViewId, ulSessionId, eOxMP_MEDIATYPE_PVRPLAYBACK, &stOapiVideoStreamInfo);

	NOT_USED_PROC_ARGUMENT(hAction, p1, p2, p3);
	return MESSAGE_PASS;
}

STATIC BUS_Result_t om_media_play_pvrpb_MsgOutputAVStopped (omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32					 ulViewId = 0;
	HUINT32					 ulActionId = SVC_PIPE_GetActionId(hAction);
	HUINT32					 ulSessionId = pstContext->ulSessionId;
	Handle_t				 hErr;

	if (pstContext->ulActionId != ulActionId)
	{
		return MESSAGE_PASS;
	}

	hErr = MGR_ACTION_GetViewIdByActionId(ulActionId, &ulViewId);
	if (ERR_OK != hErr)
	{
		return MESSAGE_PASS;
	}

	om_media_play_CbOnAVStopped(ulViewId, ulSessionId, eOxMP_MEDIATYPE_PVRPLAYBACK);

	NOT_USED_PROC_ARGUMENT(hAction, p1, p2, p3);
	return MESSAGE_PASS;
}

STATIC BUS_Result_t om_media_play_pvrpb_MsgSvclistPidChanged (omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32				ulActionId = SVC_PIPE_GetActionId (hAction);
	HUINT32 			ulViewId = 0;
	HUINT32 			ulSessionId = pstContext->ulSessionId;
	HUINT32 			ulContentId = 0;
	HERROR				hErr;
	SvcMeta_Service_t	stMetaInfo;

	if (pstContext->ulActionId != ulActionId)
	{
		return MESSAGE_PASS;
	}

	hErr = MGR_ACTION_GetViewIdByActionId(ulActionId, &ulViewId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("MGR_ACTION_GetViewIdByActionId Error \n");
		return MESSAGE_PASS;
	}

	hErr = SVC_META_GetContentIdByhAction(hAction, &ulContentId);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("SVC_META_GetContentIdByhAction Error \n");
		return MESSAGE_PASS;
	}

	HxSTD_memset(&stMetaInfo, 0x00, sizeof(SvcMeta_Service_t));

	hErr = SVC_META_GetServiceInfo(ulContentId, &stMetaInfo);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("SVC_META_GetServiceInfo Error \n");
		return MESSAGE_PASS;
	}

	// Main language
	// eDB_PARAM_ITEM_LANG_MAIN
	{
		SvcSi_AudioList_t 	stAudioList;

		HUINT32				i , j, ulSize = 0;
		HBOOL				bDolbyflag = FALSE;
		HINT32 				nOriCurrIndex = -1, nOriCurrSubIndex = -1;
		HINT32				nCurrIndex = -1, nCurrSubIndex = -1;
		HUINT16 			usAudioPid = PID_NULL;

		OxMediaPlay_AudioCompTable_t *pstAudioCompTable = &pstContext->stCache.stAudioCompTable;

		HxSTD_memset(&stAudioList, 0x00, sizeof(SvcSi_AudioList_t));

		nOriCurrIndex = pstContext->stCache.stAudioCompTable.nCurrIndex;
		nOriCurrSubIndex = pstContext->stCache.stAudioCompTable.nCurrSubIndex;

		hErr = SVC_SI_GetAudioList(ulActionId, stMetaInfo.usSvcId, &stAudioList);
		if (ERR_OK != hErr)
		{
			HxLOG_Error("SVC_SI_GetAudioList Error \n");
			return hErr;
		}

		bDolbyflag = stMetaInfo.bDolbyFlag;
		if (bDolbyflag == TRUE)
		{
			usAudioPid = stMetaInfo.usDolbyPid;
		}
		else
		{
			usAudioPid = stMetaInfo.usAudioPid;
		}

		for (i = 0; i < stAudioList.nAudio; i++)
		{
			if (stAudioList.pstAudioInfo[i].usPid == usAudioPid)
			{
				nCurrIndex = i;
				break;
			}
		}

		// aux audio가 별도로 필요한 모델은 이렇게 처리
		// 아직 수요가 많지 않아서 UK DTT로 처리했는데 향후 다른 모델들도 지원이 필요하면
		// 여기에 define을 늘리지 말고 CONFIG을 추가해서 적용해야 함
#if defined(CONFIG_OP_UK_DTT)
		for(i = 0; i < stAudioList.nAudio; i++ )
		{
			if( stAudioList.pstAudioInfo[i].usPid == stMetaInfo.usAudioAuxPid )
			{
				nCurrSubIndex = i;
				break;
			}
		}
#else
		if (nCurrIndex == -1) {
			for( i=0 ; i<stAudioList.nAudio; i++ )
			{
				if( stAudioList.pstAudioInfo[i].usPid == stMetaInfo.usAudioAuxPid )
				{
					nCurrSubIndex = i;
					break;
				}
			}
		}
#endif
		if (nCurrIndex == -1)
		{
			nCurrIndex = 0;
		}

		pstAudioCompTable->nArraySize		=	stAudioList.nAudio;
		pstAudioCompTable->nMaxSize 		=	sizeof(OxMediaPlay_AudioCompTable_t);
		pstAudioCompTable->nCurrIndex		=	nCurrIndex;
		pstAudioCompTable->nCurrSubIndex	=	nCurrSubIndex;

		for (i = 0; i < stAudioList.nAudio; i++)
		{
			pstAudioCompTable->astCompArray[i].nPid 			=	stAudioList.pstAudioInfo[i].usPid;
			pstAudioCompTable->astCompArray[i].eCompType		=	eOxMP_COMPONENT_AUDIO;
			pstAudioCompTable->astCompArray[i].nComponentTag	=	stAudioList.pstAudioInfo[i].ucComponentTag;
			pstAudioCompTable->astCompArray[i].eCodec			=	stAudioList.pstAudioInfo[i].eCodec;
			pstAudioCompTable->astCompArray[i].bIsDolby 		=	stAudioList.pstAudioInfo[i].bIsDolby;

			ulSize = sizeof(pstAudioCompTable->astCompArray[i].astLangInfo);
			if (ulSize > sizeof(stAudioList.pstAudioInfo[i].stIsoLangInfo))
			{
				ulSize = sizeof(stAudioList.pstAudioInfo[i].stIsoLangInfo);
			}
			HxSTD_MemCopy(pstAudioCompTable->astCompArray[i].astLangInfo, stAudioList.pstAudioInfo[i].stIsoLangInfo, ulSize);
			pstAudioCompTable->astCompArray[i].nLangNum = 0;

			for (j = 0; j < OxMP_AUDIO_LANG_NUM; j++)
			{
				if (pstAudioCompTable->astCompArray[i].astLangInfo[j].szLangCode[0] != 0)
				{
					pstAudioCompTable->astCompArray[i].nLangNum++;
				}
			}

			pstAudioCompTable->astCompArray[i].eSupAudType= stAudioList.pstAudioInfo[i].suppAudioEditorial;
			pstAudioCompTable->astCompArray[i].ucMixType = stAudioList.pstAudioInfo[i].ucMixType;
		}

		SVC_SI_FreeAudioList(&stAudioList);

		HxLOG_Print("\n\n[%s,%d] nAudioIndex(0x%x -> 0x%x) SubAudioIndex(0x%x -> 0x%x) \n\n", __FUNCTION__, __LINE__, nOriCurrIndex ,pstAudioCompTable->nCurrIndex , nOriCurrSubIndex,  pstAudioCompTable->nCurrSubIndex );

		if (pstAudioCompTable->nCurrIndex != nOriCurrIndex || pstAudioCompTable->nCurrSubIndex != nOriCurrSubIndex)
		{
			om_media_play_CbOnComponentAudioChanged(ulViewId, ulSessionId, eOxMP_MEDIATYPE_PVRPLAYBACK, pstAudioCompTable);
			om_media_play_CbOnComponentSelected(ulViewId, ulSessionId, eOxMP_MEDIATYPE_PVRPLAYBACK, eOxMP_COMPONENT_AUDIO, pstAudioCompTable->nCurrIndex, pstAudioCompTable->nCurrSubIndex);
		}
	}
	{
		SvcSi_SubtitleList_t	stSubtitleList;

		HINT32 	i = 0;
		HINT32 	nCurrIndex = -1, nOriCurrIndex = -1;

		OxMediaPlay_SubtitleCompTable_t *pstSubtitleCompTable = &pstContext->stCache.stSubtitleCompTable;

		HxSTD_memset(&stSubtitleList, 0x00, sizeof(SvcSi_SubtitleList_t));

		nOriCurrIndex = pstContext->stCache.stSubtitleCompTable.nCurrIndex;

		hErr = SVC_SI_GetSubtitleList(ulActionId, stMetaInfo.usSvcId, &stSubtitleList);
		if (ERR_OK != hErr)
		{
			HxLOG_Error("SVC_SI_GetSubtitleList Error \n");
			return hErr;
		}

		HxLOG_Debug("ucSubttlIdx (0x%x)\n", stMetaInfo.ucSubttlIdx);
		if (stMetaInfo.ucSubttlIdx == 0xff)
		{
			nCurrIndex = -1;
		}
		else
		{
			nCurrIndex = stMetaInfo.ucSubttlIdx;
		}

		pstSubtitleCompTable->nArraySize	=	stSubtitleList.ulSubtitleNum;
		pstSubtitleCompTable->nMaxSize		=	sizeof(OxMediaPlay_SubtitleCompTable_t);
		pstSubtitleCompTable->nCurrIndex	=	nCurrIndex;

		for (i = 0; i < stSubtitleList.ulSubtitleNum; i++)
		{
			pstSubtitleCompTable->astCompArray[i].nPid			=	stSubtitleList.pstSubtitleInfo[i].usPid;
			pstSubtitleCompTable->astCompArray[i].eCompType 	=	eOxMP_COMPONENT_SUBTITLE;

			switch(stSubtitleList.pstSubtitleInfo[i].eType)
			{
				case eSI_SUBTITLE_DVB:
				{
					pstSubtitleCompTable->astCompArray[i].eSubtitleType 						=	eOxMP_SUBTITLETYPE_Dvb;
					pstSubtitleCompTable->astCompArray[i].utArg.stDvbInfo.nTypeValue			=	stSubtitleList.pstSubtitleInfo[i].utArg.stDvbInfo.ucType;
					pstSubtitleCompTable->astCompArray[i].utArg.stDvbInfo.nComponentTag 		=	stSubtitleList.pstSubtitleInfo[i].utArg.stDvbInfo.ucComponentTag;
					pstSubtitleCompTable->astCompArray[i].utArg.stDvbInfo.nCompositionPageId	=	stSubtitleList.pstSubtitleInfo[i].utArg.stDvbInfo.usCompositionPageId;
					pstSubtitleCompTable->astCompArray[i].utArg.stDvbInfo.nAncillaryPageId		=	stSubtitleList.pstSubtitleInfo[i].utArg.stDvbInfo.usAncillaryPageId;

					HLIB_STD_MemCpySafe((pstSubtitleCompTable->astCompArray[i].utArg.stDvbInfo.szLangCode), 4,
										(stSubtitleList.pstSubtitleInfo[i].utArg.stDvbInfo.aucLangCode), 4);
				}
				break;

				case eSI_SUBTITLE_EBU:
				{
					pstSubtitleCompTable->astCompArray[i].eSubtitleType 					=	eOxMP_SUBTITLETYPE_Ebu;
					pstSubtitleCompTable->astCompArray[i].utArg.stEbuInfo.nTypeValue		=	stSubtitleList.pstSubtitleInfo[i].utArg.stEbuInfo.ucType;
					pstSubtitleCompTable->astCompArray[i].utArg.stEbuInfo.nComponentTag 	=	stSubtitleList.pstSubtitleInfo[i].utArg.stEbuInfo.ucComponentTag;
					pstSubtitleCompTable->astCompArray[i].utArg.stEbuInfo.nMagazineNum		=	stSubtitleList.pstSubtitleInfo[i].utArg.stEbuInfo.ucMagazineNum;
					pstSubtitleCompTable->astCompArray[i].utArg.stEbuInfo.nPageNum			=	stSubtitleList.pstSubtitleInfo[i].utArg.stEbuInfo.ucPageNum;

					HLIB_STD_MemCpySafe((pstSubtitleCompTable->astCompArray[i].utArg.stEbuInfo.szLangCode), 4,
										(stSubtitleList.pstSubtitleInfo[i].utArg.stEbuInfo.aucLangCode), 4);
				}
				break;

				case eSI_SUBTITLE_SUPERCC:
				default:
					break;
			}
		}

		SVC_SI_FreeSubtitleList(&stSubtitleList);

		if (pstSubtitleCompTable->nCurrIndex != nOriCurrIndex)
		{
			HxLOG_Debug("eOxMP_COMPONENT_SUBTITLE = nCurrIndex(0x%x) \n", pstSubtitleCompTable->nCurrIndex);
			om_media_play_CbOnComponentSubtitleChanged(ulViewId, ulSessionId, eOxMP_MEDIATYPE_PVRPLAYBACK, pstSubtitleCompTable);
			om_media_play_CbOnComponentSelected(ulViewId, ulSessionId, eOxMP_MEDIATYPE_PVRPLAYBACK, eOxMP_COMPONENT_SUBTITLE, pstSubtitleCompTable->nCurrIndex, -1);
		}
	}

	//Notify
	NOT_USED_PROC_ARGUMENT(hAction, p1, p2, p3);
	return MESSAGE_PASS;
}

STATIC BUS_Result_t om_media_play_pvrpb_MsgPvrRatingCtrlChanged (omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32					 nBlocked = (HINT32)p1;
	HINT32					 nRating = (HINT32)p2;
	HINT32					 nDrmSystemId = (HINT32)p3;
	HUINT32					 ulViewId = 0;
	HUINT32					 ulActionId = SVC_PIPE_GetActionId(hAction);
	HUINT32					 ulSessionId = pstContext->ulSessionId;
	Handle_t				 hErr;

	if (pstContext->ulActionId != ulActionId)
	{
		return MESSAGE_PASS;
	}

	hErr = MGR_ACTION_GetViewIdByActionId(ulActionId, &ulViewId);
	if (ERR_OK != hErr)
	{
		return MESSAGE_PASS;
	}

	om_media_play_CbOnRatingChanged(ulViewId, ulSessionId, eOxMP_MEDIATYPE_PVRPLAYBACK, nBlocked, nRating, nDrmSystemId);

	NOT_USED_PROC_ARGUMENT(hAction, p1, p2, p3);
	return MESSAGE_PASS;
}

/*
om_media_play_pvrpb_MsgPvrDrmBlock Func modified at 2014/11/25

After call "om_media_play_pvrpb_MsgPvrDrmBlock", OIPF onDRMRightsError Callback will be called.
Before modifying, contentID, DRMSystemID, RightsIssuerURL is not setting correctly. And CI+ is only using errorState.
In order to use correctly onDRMRightsError, this func was changed.
But CI+ is not tested. so it can abnormally operate.
*/
STATIC BUS_Result_t om_media_play_pvrpb_MsgPvrDrmBlock (omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32							 nBlocked = (HINT32)p1;
//	HINT32							 nRating = (HINT32)p2;
//	HINT32							 nDrmSystemId = (HINT32)p3;
	HUINT32							 ulViewId = 0;
	HUINT32							 ulActionId = SVC_PIPE_GetActionId(hAction);
	HUINT32							 ulSessionId = pstContext->ulSessionId;
	OxMediaPlay_DrmRightsError_t	 stDrmRightsError;
	Handle_t						 hErr;

	if (pstContext->ulActionId != ulActionId)
	{
		return MESSAGE_PASS;
	}

	hErr = MGR_ACTION_GetViewIdByActionId(ulActionId, &ulViewId);
	if (ERR_OK != hErr)
	{
		return MESSAGE_PASS;
	}

	HxSTD_MemSet(stDrmRightsError.szContentID, 0x00, sizeof(stDrmRightsError.szContentID));
	HxSTD_MemSet(stDrmRightsError.szDRMSystemID, 0x00, sizeof(stDrmRightsError.szDRMSystemID));
	HxSTD_MemSet(stDrmRightsError.szRightsIssuerURL, 0x00, sizeof(stDrmRightsError.szRightsIssuerURL));
	stDrmRightsError.eErrorState = nBlocked;

	om_media_play_CbOnDrmBlocked(ulViewId, ulSessionId, eOxMP_MEDIATYPE_PVRPLAYBACK, &stDrmRightsError);

	NOT_USED_PROC_ARGUMENT(hAction, p1, p2, p3);
	return MESSAGE_PASS;
}

STATIC BUS_Result_t om_media_play_pvrpb_MsgSvclistRadioBgChanged (omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HBOOL					 bRadioBackground = (HBOOL)p1;
	HUINT32					 ulViewId = 0;
	HUINT32					 ulActionId = SVC_PIPE_GetActionId(hAction);
	HUINT32					 ulSessionId = pstContext->ulSessionId;
	Handle_t				 hErr;

	if (pstContext->ulActionId != ulActionId)
	{
		return MESSAGE_PASS;
	}

	hErr = MGR_ACTION_GetViewIdByActionId(ulActionId, &ulViewId);
	if (ERR_OK != hErr)
	{
		return MESSAGE_PASS;
	}

	om_media_play_CbOnRadioBGChanged(ulViewId, ulSessionId, eOxMP_MEDIATYPE_PVRPLAYBACK, bRadioBackground);

	NOT_USED_PROC_ARGUMENT(hAction, p1, p2, p3);
	return MESSAGE_PASS;
}

STATIC BUS_Result_t om_media_play_pvrpb_EvtAVSessionStarted (omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	NOT_USED_PROC_ARGUMENT(hAction, p1, p2, p3);
	return MESSAGE_PASS;
}

STATIC BUS_Result_t om_media_play_pvrpb_EvtPbStart (omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	NOT_USED_PROC_ARGUMENT(hAction, p1, p2, p3);
	return MESSAGE_PASS;
}

STATIC BUS_Result_t om_media_play_pvrpb_EvtPbSetup (omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32					 ulViewId = 0;
	HUINT32					 ulActionId = SVC_PIPE_GetActionId(hAction);
	HUINT32					 ulSessionId = pstContext->ulSessionId;
	Handle_t				 hErr;

	if (pstContext->ulActionId != ulActionId)
	{
		return MESSAGE_PASS;
	}

	hErr = MGR_ACTION_GetViewIdByActionId(ulActionId, &ulViewId);
	if (ERR_OK != hErr)
	{
		return MESSAGE_PASS;
	}

	/* take over start informaion */
	{
		HUINT32	ulNumSpeed;
		HINT32	anSpeeds[OxMP_SUPPORTING_SPEED_MAX];

		om_media_play_pvrpb_GetSupportedSpeeds(ulActionId, &ulNumSpeed, anSpeeds);

		om_media_play_CbOnSupportedSpeedChanged(ulViewId, ulSessionId, eOxMP_MEDIATYPE_PVRPLAYBACK, ulNumSpeed, (HINT32 *)anSpeeds);
		HxLOG_Trace();
	}

	NOT_USED_PROC_ARGUMENT(hAction, p1, p2, p3);
	return MESSAGE_PASS;
}

STATIC BUS_Result_t om_media_play_pvrpb_EvtPbBOF (omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32					 ulViewId = 0;
	HUINT32					 ulActionId = SVC_PIPE_GetActionId(hAction);
	HUINT32					 ulSessionId = pstContext->ulSessionId;
	Handle_t				 hErr;

	if (pstContext->ulActionId != ulActionId)
	{
		return MESSAGE_PASS;
	}

	hErr = MGR_ACTION_GetViewIdByActionId(ulActionId, &ulViewId);
	if (ERR_OK != hErr)
	{
		return MESSAGE_PASS;
	}

	om_media_play_CbOnPlayStateChanged(ulViewId, pstContext->ulSessionId, eOxMP_MEDIATYPE_PVRPLAYBACK, eOxMP_PLAYSTATE_EOS);
	om_media_play_CbOnBof(ulViewId, ulSessionId, eOxMP_MEDIATYPE_PVRPLAYBACK);

	NOT_USED_PROC_ARGUMENT(hAction, p1, p2, p3);
	return MESSAGE_PASS;
}

STATIC BUS_Result_t om_media_play_pvrpb_EvtPbEOF (omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32					 ulViewId = 0;
	HUINT32					 ulActionId = SVC_PIPE_GetActionId(hAction);
	HUINT32					 ulSessionId = pstContext->ulSessionId;
	Handle_t				 hErr;

	if (pstContext->ulActionId != ulActionId)
	{
		return MESSAGE_PASS;
	}

	hErr = MGR_ACTION_GetViewIdByActionId(ulActionId, &ulViewId);
	if (ERR_OK != hErr)
	{
		return MESSAGE_PASS;
	}

	om_media_play_CbOnPlayStateChanged(ulViewId, pstContext->ulSessionId, eOxMP_MEDIATYPE_PVRPLAYBACK, eOxMP_PLAYSTATE_EOS);
	om_media_play_CbOnEof(ulViewId, ulSessionId, eOxMP_MEDIATYPE_PVRPLAYBACK);

	NOT_USED_PROC_ARGUMENT(hAction, p1, p2, p3);
	return MESSAGE_PASS;
}

STATIC BUS_Result_t om_media_play_pvrpb_EvtPbSpeedChanged (omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32					 ulViewId = 0;
	HUINT32					 ulActionId = SVC_PIPE_GetActionId(hAction);
	HUINT32					 ulSessionId = pstContext->ulSessionId;
	Handle_t				 hErr;

	if (pstContext->ulActionId != ulActionId)
	{
		return MESSAGE_PASS;
	}

	hErr = MGR_ACTION_GetViewIdByActionId(ulActionId, &ulViewId);
	if (ERR_OK != hErr)
	{
		return MESSAGE_PASS;
	}

	if( 0 == p1 )	om_media_play_CbOnPlayStateChanged(ulViewId, pstContext->ulSessionId, eOxMP_MEDIATYPE_PVRPLAYBACK, eOxMP_PLAYSTATE_PAUSE);
	else			om_media_play_CbOnPlayStateChanged(ulViewId, pstContext->ulSessionId, eOxMP_MEDIATYPE_PVRPLAYBACK, eOxMP_PLAYSTATE_PLAYING);

	om_media_play_CbOnSpeedChanged(ulViewId, ulSessionId, eOxMP_MEDIATYPE_PVRPLAYBACK, p1);

	NOT_USED_PROC_ARGUMENT(hAction, p1, p2, p3);
	return MESSAGE_PASS;
}

STATIC BUS_Result_t om_media_play_pvrpb_EvtPbTimecodeChange (omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32					 ulViewId = 0;
	HUINT32					 ulActionId = SVC_PIPE_GetActionId(hAction);
	HUINT32					 ulSessionId = pstContext->ulSessionId;
	MgrPb_Info_t			 stMgrPlayInfo;
	Handle_t				 hErr;

	if (pstContext->ulActionId != ulActionId)
	{
		return MESSAGE_PASS;
	}

	hErr = MGR_ACTION_GetViewIdByActionId(ulActionId, &ulViewId);
	if (ERR_OK != hErr)
	{
		return MESSAGE_PASS;
	}

	HxSTD_memset(&stMgrPlayInfo, 0x00, sizeof(MgrPb_Info_t));

	hErr = MGR_PLAYBACK_GetPlaybackInfo(ulActionId, &stMgrPlayInfo);
	if(hErr == ERR_OK)
	{
		om_media_play_CbOnPlayTimeChanged(ulViewId, ulSessionId, eOxMP_MEDIATYPE_PVRPLAYBACK, stMgrPlayInfo.ulPlayTime - stMgrPlayInfo.ulStartTime,  stMgrPlayInfo.ulEndTime - stMgrPlayInfo.ulStartTime);
	}

	NOT_USED_PROC_ARGUMENT(hAction, p1, p2, p3);
	return MESSAGE_PASS;
}

STATIC BUS_Result_t om_media_play_pvrpb_EvtRecScrambled (omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	NOT_USED_PROC_ARGUMENT(hAction, p1, p2, p3);
	return MESSAGE_PASS;
}

STATIC BUS_Result_t om_media_play_pvrpb_EvtSiEit (omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32					 ulViewId = 0;
	HUINT32					 ulActionId = SVC_PIPE_GetActionId(hAction);
	HUINT32					 ulSessionId = pstContext->ulSessionId;
	Handle_t				 hErr;

	if (pstContext->ulActionId != ulActionId)
	{
		return MESSAGE_PASS;
	}

	hErr = MGR_ACTION_GetViewIdByActionId(ulActionId, &ulViewId);
	if (ERR_OK != hErr)
	{
		return MESSAGE_PASS;
	}

	om_media_play_CbOnSiEITChanged(ulViewId, ulSessionId, eOxMP_MEDIATYPE_PVRPLAYBACK);

	NOT_USED_PROC_ARGUMENT(hAction, p1, p2, p3);
	return MESSAGE_PASS;
}

STATIC BUS_Result_t om_media_play_pvrpb_EvtSiDrm (omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32									 ulViewId = 0;
	HUINT32									 ulActionId = SVC_PIPE_GetActionId(hAction);
	HUINT32									 ulSessionId = pstContext->ulSessionId;
	DxCopyrightControl_t					 stSiCcInfo = {0, };
	MgrCopyrightControl_MediaControlInfo_t	 stDetailDrmInfo;
	Handle_t								 hErr;

	if (pstContext->ulActionId != ulActionId)
	{
		return MESSAGE_PASS;
	}

	hErr = MGR_ACTION_GetViewIdByActionId(ulActionId, &ulViewId);
	if (ERR_OK != hErr)
	{
		return MESSAGE_PASS;
	}

	HLIB_STD_MemCpySafe(&(stSiCcInfo), sizeof(DxCopyrightControl_t), (void *)p3, sizeof(DxCopyrightControl_t));

	pstContext->stOmCache.stCcInfo.bSiCc = TRUE;
	HLIB_STD_MemCpySafe(&(pstContext->stOmCache.stCcInfo.stSiCcInfo), sizeof(DxCopyrightControl_SiCcInfo_t), &(stSiCcInfo.stSiCcInfo), sizeof(DxCopyrightControl_SiCcInfo_t));

	// SI DRM은 OP 요구사항 이므로 OP type에 따라서 선택하도록 하자.
	if(pstContext->stOmCache.stCcInfo.bCasCc)
	{
		switch(pstContext->stOmCache.stCcInfo.stSiCcInfo.eSiCcType)
		{
			case eDxCopyrightControl_SiCcType_Ses:
				// SES는 SI보다 CAS 우선순위이다. 그래서 이미 CAS DRM이 수신 된경우에는 Web으로 drm 정보를 올려주지 않는다.
				if(pstContext->stOmCache.stCcInfo.stCasCcInfo.eCasCcType == eDxCopyrightControl_CasCcType_NagraSes)
				{
					return MESSAGE_PASS;
				}
				break;
			case eDxCopyrightControl_SiCcType_Nordig:
				// TODO : OP 사양 확인
				break;
			default:
				HxLOG_Error("Check the SI DRM... Is the SI DRM should affected?\n");
				return MESSAGE_PASS;
		}
	}

	hErr = MGR_COPYRIGHTCONTROL_GetMediaControlInfo(TRUE, &(pstContext->stOmCache.stCcInfo), &stDetailDrmInfo);
	if(hErr == ERR_OK)
	{
		pstContext->stCache.uTrickRestrictMode = stDetailDrmInfo.eTrickModeCtrl;

#if defined(CONFIG_OP_SES)
		pstContext->stCache.eStorageRight = stDetailDrmInfo.eStorageRight;
		om_media_play_CbCcTrickRestrictModeChanged(ulViewId, ulSessionId, eOxMP_MEDIATYPE_PVRPLAYBACK, pstContext->stCache.uTrickRestrictMode, pstContext->stCache.eStorageRight);
#else
		om_media_play_CbCcTrickRestrictModeChanged(ulViewId, ulSessionId, eOxMP_MEDIATYPE_PVRPLAYBACK, pstContext->stCache.uTrickRestrictMode);
#endif
	}

	return MESSAGE_PASS;
}

STATIC BUS_Result_t om_media_play_pvrpb_EvtCasDrm (omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32									 ulViewId = 0;
	HUINT32									 ulActionId = SVC_PIPE_GetActionId(hAction);
	HUINT32									 ulSessionId = pstContext->ulSessionId;
	DxCopyrightControl_t					 stCasCcInfo = {0, };
	MgrCopyrightControl_MediaControlInfo_t	 stDetailDrmInfo;
	Handle_t								 hErr;

	if (pstContext->ulActionId != ulActionId)
	{
		return MESSAGE_PASS;
	}

	hErr = MGR_ACTION_GetViewIdByActionId(ulActionId, &ulViewId);
	if (ERR_OK != hErr)
	{
		return MESSAGE_PASS;
	}

	HLIB_STD_MemCpySafe(&(stCasCcInfo), sizeof(DxCopyrightControl_t), (void *)p3, sizeof(DxCopyrightControl_t));

	// DRM(URI) 정보가 올라 올때 몇몇 CAS spec에서 요구하는 Trick Restrict를동작시키기 위한 내용으로 상관없는 type의 DRM은 적용되지 않도록 한다. Base에서는 필요한 CAS Type에 대해서만 처리하도록 수정
	// 이력: Nagra & SES spec을 맞추기 위해 iCord-Pro Branch에서 처음 적용된 부분이고, 이후 Base로 들어옴. CDIG-2000 모델에서 Conax spec을 처리하기 위해 이 부분을 사용함.
	switch(stCasCcInfo.stCasCcInfo.eCasCcType)
	{
		case eDxCopyrightControl_CasCcType_CiPlus:
			HxLOG_Info("URI of CI+ do not affect at om media play...\n");
			return MESSAGE_PASS;
		case eDxCopyrightControl_CasCcType_Conax:
		case eDxCopyrightControl_CasCcType_NagraSes:
			HxLOG_Info("DRM(URI) is going to be affected...\n");
			break;
		default:
			HxLOG_Error("Check the DRM... Is the DRM should affected?\n");
			return MESSAGE_PASS;
	}

	pstContext->stOmCache.stCcInfo.bCasCc = TRUE;
	HLIB_STD_MemCpySafe(&(pstContext->stOmCache.stCcInfo.stCasCcInfo), sizeof(DxCopyrightControl_CasCcInfo_t), &(stCasCcInfo.stCasCcInfo), sizeof(DxCopyrightControl_CasCcInfo_t));

	hErr = MGR_COPYRIGHTCONTROL_GetMediaControlInfo(TRUE, &(pstContext->stOmCache.stCcInfo), &stDetailDrmInfo);
	if (ERR_OK == hErr)
	{
		pstContext->stCache.uTrickRestrictMode = stDetailDrmInfo.eTrickModeCtrl;
#if defined(CONFIG_OP_SES)
        pstContext->stCache.eStorageRight = stDetailDrmInfo.eStorageRight;
        om_media_play_CbCcTrickRestrictModeChanged(ulViewId, ulSessionId, eOxMP_MEDIATYPE_PVRPLAYBACK, pstContext->stCache.uTrickRestrictMode, pstContext->stCache.eStorageRight);
#else
		om_media_play_CbCcTrickRestrictModeChanged(ulViewId, ulSessionId, eOxMP_MEDIATYPE_PVRPLAYBACK, pstContext->stCache.uTrickRestrictMode);
#endif
	}

	return MESSAGE_PASS;
}

#if defined (CONFIG_MW_MM_AIT_REC)
STATIC BUS_Result_t om_media_play_pvrpb_EvtSiAit (omMediaPlay_Context_t *pstContext, Handle_t hAction, HUINT32 ulAitMsg)
{
	HUINT8							 ucDummyRaw = 0;
	HUINT32							 i, ulAitSecCount = 0, ulAitTotalLen = 0;
	HUINT32							 ulViewId = 0;
	HUINT32							 ulActionId = SVC_PIPE_GetActionId(hAction);
	HUINT32							 ulSessionId = pstContext->ulSessionId;
	HUINT8							*pucAllSitRawSec = NULL;
	HUINT32							 aulSecLen[OxMP_AIT_SECTION_NUM_MAX];
	OxMediaPlay_AitSectionData_t	 stAitSectionData;
	Handle_t						 hErr;

	if (pstContext->ulActionId != ulActionId)
	{
		return MESSAGE_PASS;
	}

	hErr = MGR_ACTION_GetViewIdByActionId(ulActionId, &ulViewId);
	if (ERR_OK != hErr)
	{
		return MESSAGE_PASS;
	}

	HxSTD_MemSet(aulSecLen, 0x00, sizeof(aulSecLen));
	HxSTD_MemSet(&stAitSectionData, 0x00, sizeof(OxMediaPlay_AitSectionData_t));

	switch(ulAitMsg)
	{
		case eSEVT_SI_NONE_AIT:		stAitSectionData.eAitState = eAitState_None;		break;
		case eSEVT_SI_AIT_TIMEOUT:	stAitSectionData.eAitState = eAitState_Timeout;		break;
		case eSEVT_SI_AIT:			stAitSectionData.eAitState = eAitState_Received;	break;

		default:
			HxLOG_Error("invalid evt si AIT msg(%d) \n", ulAitMsg);
			return ERR_FAIL;
	}

	if (stAitSectionData.eAitState == eAitState_Received)
	{
		HUINT32		ulCurPos 			= 0;
		HUINT8		*pucSiRawSection	=	NULL;
		HUINT8		*paucSitRawSec[OxMP_AIT_SECTION_NUM_MAX]	=	{NULL, };

		hErr = SVC_SI_GetAitRawSection(ulActionId, OxMP_AIT_SECTION_NUM_MAX, &paucSitRawSec[0], &ulAitSecCount);
		if ((hErr != ERR_FAIL) && (ulAitSecCount > 0))
		{
			// count total ait length
			for (i = 0; i < ulAitSecCount; i++)
			{
				pucSiRawSection = paucSitRawSec[i];
				aulSecLen[i] = (HUINT32)HxMACRO_Get12Bit(pucSiRawSection + 1) + 3;
				ulAitTotalLen += aulSecLen[i];
			}

			// make ait section data for OAPI
			pucAllSitRawSec = (HUINT8 *)OxMGR_Calloc(sizeof(HUINT8) * ulAitTotalLen);
			if (pucAllSitRawSec == NULL)
			{
				HxLOG_Error("OxMGR_Calloc Error: size(%d) \n", ulAitTotalLen);
				return ERR_FAIL;
			}

			ulCurPos = 0;
			for (i = 0; i < ulAitSecCount; i++)
			{
				pucSiRawSection = paucSitRawSec[i];
				HxSTD_MemCopy(&pucAllSitRawSec[ulCurPos], pucSiRawSection, aulSecLen[i]);
				ulCurPos += aulSecLen[i];
			}

			stAitSectionData.ulAitSecCount 		= ulAitSecCount;
			stAitSectionData.ulAitTotaSeclLen 	= ulAitTotalLen;
			HxSTD_MemCopy(stAitSectionData.aulAitSecLen, aulSecLen, sizeof(stAitSectionData.aulAitSecLen));
		}
	}
	else
	{
		stAitSectionData.ulAitTotaSeclLen = 1;
		pucAllSitRawSec = &ucDummyRaw;
	}

	om_media_play_CbOnSiAITChanged(ulViewId, ulSessionId, eOxMP_MEDIATYPE_PVRPLAYBACK, &stAitSectionData, pucAllSitRawSec);

	return MESSAGE_PASS;
}
#endif
STATIC BUS_Result_t om_media_play_pvrpb_EvtSiPmt (omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32					 ulContentId = pstContext->stCache.stPvrPb.ulContentId;
	HUINT32					 ulViewId = 0;
	HUINT32					 ulActionId = SVC_PIPE_GetActionId(hAction);
	HUINT32					 ulSessionId = pstContext->ulSessionId;
	SvcMeta_Service_t		 stContentInfo = {0,};
	Handle_t				 hErr;

	if (pstContext->ulActionId != ulActionId)
	{
		return MESSAGE_PASS;
	}

	hErr = MGR_ACTION_GetViewIdByActionId(ulActionId, &ulViewId);
	if (ERR_OK != hErr)
	{
		return MESSAGE_PASS;
	}

	/* VIDEO, AUDIO, SUBTITLE Component */
	if (0 == ulContentId)
		return ERR_OK;

	hErr = SVC_META_GetServiceInfo (ulContentId, &stContentInfo);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("Error, ulContentId(0x%08x)\n", ulContentId);
		goto END_FUNC;
	}

	/*Video*/
	{
		OxMediaPlay_VideoCompTable_t *pstVideoCompTable	=	&pstContext->stCache.stVideoCompTable;

		HxSTD_MemSet (pstVideoCompTable, 0, sizeof(OxMediaPlay_VideoCompTable_t));

		pstVideoCompTable->nArraySize	=	1;
		pstVideoCompTable->nMaxSize	=	sizeof(OxMediaPlay_VideoCompTable_t);
		pstVideoCompTable->nCurrIndex	=	0;

		{
			pstVideoCompTable->astCompArray[0].nPid				=	stContentInfo.usVideoPid;
			pstVideoCompTable->astCompArray[0].eCodec			=	stContentInfo.eVideoCodec;
			pstVideoCompTable->astCompArray[0].eCompType		=	eOxMP_COMPONENT_VIDEO;
			pstVideoCompTable->astCompArray[0].nHdVideo			=	(stContentInfo.eVideoType == eDxVIDEO_TYPE_HD)?TRUE:FALSE;
			pstVideoCompTable->astCompArray[0].nComponentTag	=	0; // INDEX ???
			pstVideoCompTable->astCompArray[0].nCasFound		=	0; // ???
		}

		/* broadcast */
		om_media_play_CbOnComponentVideoChanged(ulViewId, ulSessionId, eOxMP_MEDIATYPE_PVRPLAYBACK, pstVideoCompTable );
	}

	/*Audio*/
	{
		OxMediaPlay_AudioCompTable_t *pstAudioCompTable = &pstContext->stCache.stAudioCompTable;

		HINT32 i;
		HINT32 nCurrIndex = -1;

		SvcSi_AudioList_t stAudioList;

		HxSTD_MemSet (&stAudioList, 0, sizeof(SvcSi_AudioList_t));
		HxSTD_MemSet (pstAudioCompTable, 0, sizeof(OxMediaPlay_AudioCompTable_t));

		hErr = SVC_SI_GetAudioList (ulActionId, stContentInfo.usSvcId, &stAudioList);
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("SVC_SI_GetAudioList Error. ulContentId(0x%08x)\n", ulContentId);
		}

		for( i=0 ; i<stAudioList.nAudio; i++ )
		{
			if( stAudioList.pstAudioInfo[i].usPid == stContentInfo.usAudioPid )
			{
				nCurrIndex = i;
				break;
			}
		}

		// aux audio가 별도로 필요한 모델은 이렇게 처리
		// 아직 수요가 많지 않아서 UK DTT로 처리했는데 향후 다른 모델들도 지원이 필요하면
		// 여기에 define을 늘리지 말고 CONFIG을 추가해서 적용해야 함
#if defined(CONFIG_OP_UK_DTT)
		if (nCurrIndex == -1)
		{
			nCurrIndex = 0;
		}

		pstAudioCompTable->nCurrSubIndex = -1;
		for(i = 0; i < stAudioList.nAudio; i++ )
		{
			if( stAudioList.pstAudioInfo[i].usPid == stContentInfo.usAudioAuxPid)
			{
				pstAudioCompTable->nCurrSubIndex = i;
				break;
			}
		}
#else
		if (nCurrIndex == -1) {
			for( i=0 ; i<stAudioList.nAudio; i++ )
			{
				if( stAudioList.pstAudioInfo[i].usPid == stContentInfo.usAudioAuxPid)
				{
					nCurrIndex = i;
					break;
				}
			}
		}

		if (nCurrIndex == -1)
		{
			nCurrIndex = 0;
		}
#endif

		pstAudioCompTable->nArraySize	=	stAudioList.nAudio;
		pstAudioCompTable->nMaxSize	=	sizeof(OxMediaPlay_AudioCompTable_t);
		pstAudioCompTable->nCurrIndex	=	nCurrIndex;

		{
			for( i=0 ; i<stAudioList.nAudio; i++ )
			{
				pstAudioCompTable->astCompArray[i].nPid				=	stAudioList.pstAudioInfo[i].usPid;
				pstAudioCompTable->astCompArray[i].eCompType		=	eOxMP_COMPONENT_AUDIO;
				pstAudioCompTable->astCompArray[i].nComponentTag	=	stAudioList.pstAudioInfo[i].ucComponentTag;
				pstAudioCompTable->astCompArray[i].nLangNum			=	0; //stAudioList.pstAudioInfo[i]. ???
				pstAudioCompTable->astCompArray[i].eCodec			=	stAudioList.pstAudioInfo[i].eCodec;
				pstAudioCompTable->astCompArray[i].bIsDolby			=	stAudioList.pstAudioInfo[i].bIsDolby;

				/*convert 필요*/
				HLIB_STD_MemCpySafe((pstAudioCompTable->astCompArray[i].astLangInfo), sizeof(OxMediaPlay_AudioALangInfo_t) * OxMP_AUDIO_LANG_NUM,
									(stAudioList.pstAudioInfo[i].stIsoLangInfo), sizeof(SvcSi_Iso639LangInfo_t) * SvcSi_MAX_AUD_NUM);
			}

		}

		SVC_SI_FreeAudioList(&stAudioList);

		/* broadcast */
		om_media_play_CbOnComponentAudioChanged(ulViewId, ulSessionId, eOxMP_MEDIATYPE_PVRPLAYBACK, pstAudioCompTable );
		om_media_play_CbOnComponentSelected( ulViewId, ulSessionId, eOxMP_MEDIATYPE_PVRPLAYBACK, eOxMP_COMPONENT_AUDIO, nCurrIndex, -1);
	}

	/*Subtitle*/
	{
		OxMediaPlay_SubtitleCompTable_t *pstSubtitleCompTable = &pstContext->stCache.stSubtitleCompTable;

		HINT32 i;
		HINT32 nCurrIndex = 0;
		SvcSi_SubtitleList_t stSubtitleList;
//		HBOOL bSubtitleEnable = FALSE;

		HxSTD_MemSet (&stSubtitleList, 0, sizeof(SvcSi_SubtitleList_t));
		HxSTD_MemSet (pstSubtitleCompTable, 0, sizeof(OxMediaPlay_SubtitleCompTable_t));

		hErr = SVC_SI_GetSubtitleList (ulActionId, stContentInfo.usSvcId, &stSubtitleList);
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("SVC_SI_GetSubtitleList Error. ulContentId(0x%08x)\n", ulContentId);
			goto END_FUNC;
		}

		if(stContentInfo.bSubttlUserFlag == TRUE)
		{// user select subtitle
			if (stContentInfo.ucSubttlIdx == 0xff)
			{
				nCurrIndex = -1;
			}
			else
			{
				nCurrIndex = stContentInfo.ucSubttlIdx;
			}
		}
		else
		{ // Apply the menu language
			// 1. Finding subtitle-language same with primary subtitle language
			hErr = MGR_PLAYBACK_GetMenuSubtitleLangIndex(eDB_PARAM_ITEM_LANG_SUBTITLE0, &stSubtitleList, &nCurrIndex);
			if(hErr != ERR_OK)
			{
				// 2. Finding subtitle-language same with secandary subtitle language
				hErr = MGR_PLAYBACK_GetMenuSubtitleLangIndex(eDB_PARAM_ITEM_LANG_SUBTITLE1, &stSubtitleList, &nCurrIndex);
				if(hErr != ERR_OK)
				{
#if defined( CONFIG_OP_NORDIG )
					// if subtitle-language same with menu is not exist, does not display subtitle at nordig.
					nCurrIndex = -1;
#else
					// 3. Finding subtitle-language same with what was written in metafile.
					if (stContentInfo.ucSubttlIdx == 0xff)
					{
						nCurrIndex = -1;
					}
					else
					{
						nCurrIndex = stContentInfo.ucSubttlIdx;
					}
#endif
				}
			}
		}

		HxLOG_Error ("Selected ucSubttlIdx ( %d )\n", nCurrIndex);

		pstSubtitleCompTable->nArraySize	=	stSubtitleList.ulSubtitleNum;
		pstSubtitleCompTable->nMaxSize	=	sizeof(OxMediaPlay_SubtitleCompTable_t);
		pstSubtitleCompTable->nCurrIndex	=	nCurrIndex;

		{
			for( i=0 ; i<stSubtitleList.ulSubtitleNum; i++ )
			{
				pstSubtitleCompTable->astCompArray[i].nPid			=	stSubtitleList.pstSubtitleInfo[i].usPid;
				pstSubtitleCompTable->astCompArray[i].eCompType	=	eOxMP_COMPONENT_SUBTITLE;

				switch(stSubtitleList.pstSubtitleInfo[i].eType)
				{
					case eSI_SUBTITLE_DVB:
					{
						pstSubtitleCompTable->astCompArray[i].eSubtitleType	=	eOxMP_SUBTITLETYPE_Dvb;
						pstSubtitleCompTable->astCompArray[i].utArg.stDvbInfo.nTypeValue			=	stSubtitleList.pstSubtitleInfo[i].utArg.stDvbInfo.ucType;
						pstSubtitleCompTable->astCompArray[i].utArg.stDvbInfo.nComponentTag			=	stSubtitleList.pstSubtitleInfo[i].utArg.stDvbInfo.ucComponentTag;
						pstSubtitleCompTable->astCompArray[i].utArg.stDvbInfo.nCompositionPageId	=	stSubtitleList.pstSubtitleInfo[i].utArg.stDvbInfo.usCompositionPageId;
						pstSubtitleCompTable->astCompArray[i].utArg.stDvbInfo.nAncillaryPageId		=	stSubtitleList.pstSubtitleInfo[i].utArg.stDvbInfo.usAncillaryPageId;

						HLIB_STD_MemCpySafe((pstSubtitleCompTable->astCompArray[i].utArg.stDvbInfo.szLangCode), 4,
											(stSubtitleList.pstSubtitleInfo[i].utArg.stDvbInfo.aucLangCode), 4);
					}
					break;

					case eSI_SUBTITLE_EBU:
					{
						pstSubtitleCompTable->astCompArray[i].eSubtitleType	=	eOxMP_SUBTITLETYPE_Ebu;
						pstSubtitleCompTable->astCompArray[i].utArg.stEbuInfo.nTypeValue		=	stSubtitleList.pstSubtitleInfo[i].utArg.stEbuInfo.ucType;
						pstSubtitleCompTable->astCompArray[i].utArg.stEbuInfo.nComponentTag		=	stSubtitleList.pstSubtitleInfo[i].utArg.stEbuInfo.ucComponentTag;
						pstSubtitleCompTable->astCompArray[i].utArg.stEbuInfo.nMagazineNum		=	stSubtitleList.pstSubtitleInfo[i].utArg.stEbuInfo.ucMagazineNum;
						pstSubtitleCompTable->astCompArray[i].utArg.stEbuInfo.nPageNum			=	stSubtitleList.pstSubtitleInfo[i].utArg.stEbuInfo.ucPageNum;

						HLIB_STD_MemCpySafe((pstSubtitleCompTable->astCompArray[i].utArg.stEbuInfo.szLangCode), 4,
											(stSubtitleList.pstSubtitleInfo[i].utArg.stEbuInfo.aucLangCode), 4);
					}
					break;

					case eSI_SUBTITLE_SUPERCC:
					default:
						break;
				}
			}
		}

		SVC_SI_FreeSubtitleList(&stSubtitleList);

		/* broadcast */
		om_media_play_CbOnComponentSubtitleChanged(ulViewId, ulSessionId, eOxMP_MEDIATYPE_PVRPLAYBACK, pstSubtitleCompTable );
		om_media_play_CbOnComponentSelected( ulViewId, ulSessionId, eOxMP_MEDIATYPE_PVRPLAYBACK, eOxMP_COMPONENT_SUBTITLE, nCurrIndex, -1);
	}

	/* broadcast */
	om_media_play_CbOnSiPMTChanged( ulViewId, ulSessionId, eOxMP_MEDIATYPE_PVRPLAYBACK );

END_FUNC:
	NOT_USED_PROC_ARGUMENT(hAction, p1, p2, p3);
	return MESSAGE_PASS;
}

#if defined(CONFIG_CAS_CONAX)
STATIC BUS_Result_t om_media_play_pvrpb_EvtCasConaxPlaybackError (omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32 ulViewId;
	OxMediaPlay_MediaType_e eMediaType;
	HUINT32 ulActionId = SVC_PIPE_GetActionId(hAction);

	if (pstContext->ulActionId != ulActionId)
	{
		return MESSAGE_BREAK;
	}

	if(MGR_ACTION_GetViewIdByActionId(pstContext->ulActionId, &ulViewId) != ERR_OK)
	{
		return MESSAGE_BREAK;
	}

	if((DxCAS_GroupId_e)p2 == eDxCAS_GROUPID_CX)
	{
		om_media_play_pvrpb_Stop (pstContext->ulActionId);

		om_media_play_CleanCache(pstContext, eOxMP_MEDIATYPE_PVRPLAYBACK);
		om_media_play_CbOnPlayErrorChanged(ulViewId, pstContext->ulSessionId, eOxMP_MEDIATYPE_PVRPLAYBACK, eOxMP_PLAYERROR_PLAY_ERROR, 0);

		if(MGR_OUTPUT_ResetCopyProtect(pstContext->ulActionId) != ERR_OK)
		{
			HxLOG_Error("MGR_OUTPUT_ResetCopyProtect Failed \n");
		}
	}

	return MESSAGE_BREAK;
}
#endif

#define ___MEMBER_FUNCTIONS___
BUS_Result_t proc_om_media_play_pvrpb_Proto (omMediaPlay_Context_t *pstContext, HINT32 message, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t			 eResult;
	switch (message)
	{
		case eMEVT_PVR_NOTIFY_PLAYBACK_STARTED:
			HxLOG_Trace();
			eResult = om_media_play_pvrpb_MsgAppPlaybackNotifyStarted (pstContext, hHandle, p1, p2, p3);
			break;
		case eMEVT_PVR_NOTIFY_PLAYBACK_STOPPED:
			HxLOG_Trace();
			eResult = om_media_play_pvrpb_MsgAppPlaybackNotifyStopped (pstContext, hHandle, p1, p2, p3);
			break;
		case eMEVT_PVR_NOTIFY_PLAYBACK_ERROR_STOPPED:
			eResult = om_media_play_pvrpb_MsgAppPlaybackNotifyErrorStopped (pstContext, hHandle, p1, p2, p3);
			break;
		case eMEVT_OUTPUT_AV_STARTED:
			HxLOG_Trace();
			eResult = om_media_play_pvrpb_MsgOutputAVStarted (pstContext, hHandle, p1, p2, p3);
			break;
		case eSEVT_AV_VIDEO_STARTED:
			HxLOG_Trace();
			eResult = om_media_play_pvrpb_EvtAvVideoStarted (pstContext, hHandle, p1, p2, p3);
			break;
		case eMEVT_OUTPUT_AV_STOPPED:
			HxLOG_Trace();
			eResult = om_media_play_pvrpb_MsgOutputAVStopped (pstContext, hHandle, p1, p2, p3);
			break;
		case eMEVT_SVCLIST_SVC_PID_CHANGED:
			eResult = om_media_play_pvrpb_MsgSvclistPidChanged (pstContext, hHandle, p1, p2, p3);
			break;
		case eMEVT_PVR_NOTIFY_PLAYBACK_EVENT_RATING_CTRL_CHANGED:
			eResult = om_media_play_pvrpb_MsgPvrRatingCtrlChanged (pstContext, hHandle, p1, p2, p3);
			break;
		case eMEVT_PVR_PLAYBACK_EVENT_DRM_BLOCK:
			eResult = om_media_play_pvrpb_MsgPvrDrmBlock (pstContext, hHandle, p1, p2, p3);
			break;
		case eMEVT_SVCLIST_RADIOBG_CHANGED:
			eResult = om_media_play_pvrpb_MsgSvclistRadioBgChanged (pstContext, hHandle, p1, p2, p3);
			break;
		case eSEVT_SI_PMT:
			eResult = om_media_play_pvrpb_EvtSiPmt (pstContext, hHandle, p1, p2, p3);
			break;
		case eSEVT_AV_SESSION_STARTED:
			eResult = om_media_play_pvrpb_EvtAVSessionStarted (pstContext, hHandle, p1, p2, p3);
			break;
		case eSEVT_PB_START:
			eResult = om_media_play_pvrpb_EvtPbStart (pstContext, hHandle, p1, p2, p3);
			break;
		case eSEVT_PB_SETUP:
			eResult = om_media_play_pvrpb_EvtPbSetup (pstContext, hHandle, p1, p2, p3);
			break;
		case eSEVT_PB_BOF:
			eResult = om_media_play_pvrpb_EvtPbBOF (pstContext, hHandle, p1, p2, p3);
			break;
		case eSEVT_PB_EOF:
			eResult = om_media_play_pvrpb_EvtPbEOF (pstContext, hHandle, p1, p2, p3);
			break;
		case eSEVT_PB_SPEED_CHANGED:
			eResult = om_media_play_pvrpb_EvtPbSpeedChanged (pstContext, hHandle, p1, p2, p3);
			break;
		case eSEVT_PB_TIMECODE_CHANGE:
			eResult = om_media_play_pvrpb_EvtPbTimecodeChange (pstContext, hHandle, p1, p2, p3);
			break;
		case eSEVT_REC_SCRAMBLED:
			eResult = om_media_play_pvrpb_EvtRecScrambled (pstContext, hHandle, p1, p2, p3);
			break;
		case eSEVT_SI_EIT:
			eResult = om_media_play_pvrpb_EvtSiEit (pstContext, hHandle, p1, p2, p3);
			break;
		case eSEVT_SI_DRM_SIGNALLED:
			eResult = om_media_play_pvrpb_EvtSiDrm (pstContext, hHandle, p1, p2, p3);
			break;
		case eSEVT_CAS_DRM_SIGNALLED:
			eResult = om_media_play_pvrpb_EvtCasDrm (pstContext, hHandle, p1, p2, p3);
			break;
#if defined (CONFIG_MW_MM_AIT_REC)
		case eSEVT_SI_AIT:
			eResult = om_media_play_pvrpb_EvtSiAit (pstContext, hHandle, eSEVT_SI_AIT);
			break;
#endif
#if defined(CONFIG_CAS_CONAX)
		case eMEVT_SYSCTL_CONAX_PLAYBACK_ERROR:
			eResult = om_media_play_pvrpb_EvtCasConaxPlaybackError (pstContext, hHandle, p1, p2, p3);
			break;
#endif
//		case EVT_PVR_SIT_CHANGED:
		default:
			eResult = MESSAGE_PASS;
			break;
	}

	return (MESSAGE_BREAK == eResult) ? MESSAGE_BREAK : MESSAGE_PASS;
}

HERROR om_media_play_pvrpb_Init(void)
{
	return ERR_OK;
}

HERROR om_media_play_pvrpb_Start (HUINT32 ulActionId, OxMediaPlay_StartPvrPb_t *pstStart)
{
	HERROR				hErr;
	omMediaPlay_Context_t*		pstContext = om_media_play_GetContext (ulActionId);

	hErr = MGR_PVR_StartPlayback (pstStart->ulContentId, pstStart->ulStartTime);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("MGR_PVR_StartPlayback Failed.\n");
	}

	if (ERR_OK != hErr)
	{	// START Error
		pstContext->stCache.eState 	= eOxMP_PLAYSTATE_NONE;
		pstContext->stCache.eError	= eOxMP_PLAYERROR_UNKNWON;
	}

	return hErr;
}

HERROR om_media_play_pvrpb_Stop (HUINT32 ulActionId)
{
	BUS_SendMessage (NULL, eMEVT_PVR_PLAYBACK_STOP, SVC_PIPE_GetActionHandle (ulActionId), 0, 0, 0);

	return ERR_OK;
}

HERROR om_media_play_pvrpb_SetComponentIndex (HUINT32 ulActionId, OxMediaPlay_Component_e eCompType, HINT32 nCompIndex, HUINT8 *pstrLang)
{
	omMediaPlay_Context_t*		pstContext = om_media_play_GetContext (ulActionId);

	SvcMeta_Service_t	stService = {0, };

	HxLOG_Debug("ulActionId=%x, eCompType=%d, nCompIndex=%d\n", ulActionId, eCompType, nCompIndex);

	if (ERR_OK != MGR_PLAYBACK_GetServiceInfo(ulActionId, &stService))
	{
		HxLOG_Error ("can't get the service info from ap _player with act Id %x\n", ulActionId);
		return ERR_FAIL;
	}

	switch(eCompType)
	{
		case eOxMP_COMPONENT_VIDEO:
		case eOxMP_COMPONENT_AUDIO:
		{
			HxLOG_Debug("nCompIndex / nSize : (%d/%d)\n", nCompIndex, pstContext->stCache.stAudioCompTable.nArraySize);

			if( pstContext->stCache.stAudioCompTable.nArraySize <= nCompIndex )
				return ERR_FAIL;

			if( pstContext->stCache.stAudioCompTable.nCurrIndex != nCompIndex )
			{
				HBOOL			bUpdateSvc	= FALSE;
				OxMediaPlay_AudioComp_t *pstAudioInfo = &(pstContext->stCache.stAudioCompTable.astCompArray[nCompIndex]);

				HxLOG_Debug("Audio Selected : [%d]\n", nCompIndex);
				pstContext->stCache.stAudioCompTable.nCurrIndex = nCompIndex;

				HxLOG_Debug("bIsDolby [%p:%p] pid [%p:%p] eCodec [%p:%p]\n",
					pstAudioInfo->bIsDolby, stService.bDolbyFlag, pstAudioInfo->nPid, stService.usAudioPid, pstAudioInfo->eCodec, stService.eAudioCodec);

				if (pstAudioInfo->bIsDolby != stService.bDolbyFlag)
				{
					stService.bAudUserSetFlag	= TRUE;
					stService.bDolbyFlag		= pstAudioInfo->bIsDolby;
					bUpdateSvc					= TRUE;
				}

				if (pstAudioInfo->nPid != stService.usAudioPid)
				{
					stService.bAudUserSetFlag	= TRUE;
					stService.usAudioPid		= pstAudioInfo->nPid;
					bUpdateSvc					= TRUE;
				}

				if (pstAudioInfo->eCodec != stService.eAudioCodec)
				{
					stService.bAudUserSetFlag	= TRUE;
					stService.eAudioCodec		= pstAudioInfo->eCodec;
					bUpdateSvc					= TRUE;
				}

#if defined(SVC_AUDIO_LANGUAGE_CODE)
				if (NULL != pstrLang)
				{
					HLIB_STD_StrNCpySafe(stService.audioLanguage, pstrLang, DxLANG_CODE_LEN);
					bUpdateSvc					= TRUE;
				}
#endif
				if (TRUE == bUpdateSvc)
				{
					if (stService.bDolbyFlag == TRUE) // MOON_aud
					{
						if ( (stService.eAudioCodec == eDxAUDIO_CODEC_DOLBY_AC3)
							|| (stService.eAudioCodec == eDxAUDIO_CODEC_DOLBY_AC3P) )
						{
							if (stService.usAudioPid != stService.usDolbyPid)
								 stService.usDolbyPid = stService.usAudioPid;
						}
					}

					HxLOG_Debug("Audio selected [%d]\n", pstContext->stCache.stAudioCompTable.nCurrIndex);
					MGR_PLAYBACK_SetServiceInfo(ulActionId, &stService, FALSE);
					BUS_SendMessage (NULL, eMEVT_SVCLIST_SVC_PID_CHANGED, SVC_PIPE_GetActionHandle (pstContext->ulActionId), 0, 0, 0);
				}
			}

			break;
		}
		case eOxMP_COMPONENT_SUBTITLE:
			{
				HBOOL			bUpdate = FALSE;

				HxLOG_Debug("Subtitle Selected : [%d]\n", nCompIndex);

				if (nCompIndex == -1)
				{
					pstContext->stCache.stSubtitleCompTable.nCurrIndex = nCompIndex;
					stService.ucSubttlIdx = SUBTITLE_OFF_IDX;
					stService.bSubttlUserFlag = TRUE;
					bUpdate = TRUE;
				}
				else if ((nCompIndex != pstContext->stCache.stSubtitleCompTable.nCurrIndex) && (nCompIndex < pstContext->stCache.stSubtitleCompTable.nArraySize))
				{
					pstContext->stCache.stSubtitleCompTable.nCurrIndex = nCompIndex;
					stService.ucSubttlIdx = nCompIndex;
					stService.bSubttlUserFlag = TRUE;
					bUpdate = TRUE;
				} else
					return ERR_FAIL;

				if (bUpdate)
				{
					HxLOG_Debug("subtitle changed : (%s)[%d]\n", stService.aucServiceName, stService.ucSubttlIdx);
					MGR_PLAYBACK_SetServiceInfo(ulActionId, &stService, FALSE);
					BUS_SendMessage (NULL, eMEVT_SVCLIST_SVC_PID_CHANGED, SVC_PIPE_GetActionHandle (pstContext->ulActionId), 0, 0, 0);
				}
			}
			break;

		case eOxMP_COMPONENT_TELETEXT:
		case eOxMP_COMPONENT_CLOSEDCAPTION:
		case eOxMP_COMPONENT_DATA:
		default:
			break;
	}

	return ERR_OK;
}

HERROR om_media_play_pvrpb_SetPlaySpeed (HUINT32 ulActionId, HINT32 nSpeed)
{
	HERROR hErr;
	hErr = MGR_PLAYBACK_SetPlaySpeed(ulActionId, nSpeed);
	if( ERR_OK != hErr )
	{
		HxLOG_Error("MGR_PLAYBACK_SetPlaySpeed Error: \n");
		return hErr;
	}

	//notify

	return ERR_OK;
}

HERROR om_media_play_pvrpb_SetPlayPosition (HUINT32 ulActionId, HINT32 ulPosition)
{
	HERROR hErr;
	hErr = MGR_PLAYBACK_SetJumpPlay (ulActionId, ulPosition);
	if( ERR_OK != hErr )
	{
		HxLOG_Error("MGR_PVR_SetJumpPlay Error: \n");
		return hErr;
	}

	//notify

	return ERR_OK;
}

HERROR om_media_play_pvrpb_SetSubtitleEnable (HUINT32 ulActionId, HBOOL bEnable)
{
	HERROR hErr = ERR_FAIL;

	/* 추후 index 를 통한 off가 아닌 본 API 통하여 하도록 한다.*/
	//STATIC HERROR mgr_appmgr_mctrlusrpb_setSubtitleInfo (_mctrlUsrPbContext_t	*pstContext, MEDIADEC_SUBTITLE_t	*pstSubtitle)
	return hErr;
}

HERROR om_media_play_pvrpb_SetVideoHideEnable (HUINT32 ulActionId, HBOOL bEnable)
{
	HERROR hErr = ERR_FAIL;

	hErr = MGR_LIVE_SetVideoHideByAppkit (ulActionId, bEnable);
	if( ERR_OK != hErr )
	{
		HxLOG_Error("MGR_LIVE_SetVideoHideByAppkit Error.\n");
	}

	return hErr;
}

HERROR om_media_play_pvrpb_GetSupportedSpeeds (HUINT32 ulActionId, HUINT32 *pulNumSpeed, HINT32 *pSpeeds)
{
	HERROR					 hErr;
	HINT32					*pnSpeedArray = NULL;
	HUINT32					 ulSpeedNum = 0;

	MgrPb_Info_t					stPbInfo ;//= {0,};
	SvcMeta_Record_t				 stRecordInfo;//  = {0,};
	HBOOL						 bScrambled = FALSE;

	if (NULL == pSpeeds)
		return ERR_FAIL;

	HxSTD_MemSet(&stPbInfo, 0, sizeof(MgrPb_Info_t));
	HxSTD_MemSet(&stRecordInfo, 0, sizeof(SvcMeta_Record_t));

	if (MGR_PLAYBACK_GetPlaybackInfo(ulActionId, &stPbInfo) == ERR_OK)
	{
		if (SVC_META_GetRecordInfo(stPbInfo.ulContentId, &stRecordInfo) == ERR_OK)
		{
			bScrambled = stRecordInfo.u1cScrambled;
		}
	}
	HxLOG_Debug("########## Scrambled : [%s]\n", bScrambled ? "TRUE" : "FALSE");

	HxSTD_MemSet (pSpeeds, 0, sizeof(HINT32)*OxMP_SUPPORTING_SPEED_MAX);

	hErr = SVC_PB_GetSupportSpeeds (SVC_PIPE_GetActionHandle (ulActionId), bScrambled,  &ulSpeedNum, &pnSpeedArray);
	if ((ERR_OK == hErr) && (ulSpeedNum > 0) && (NULL != pnSpeedArray))
	{
		HUINT32			 ulCount;
		HUINT32			 ulMinSpeedNum = (OxMP_SUPPORTING_SPEED_MAX>ulSpeedNum)?ulSpeedNum : OxMP_SUPPORTING_SPEED_MAX;

		for (ulCount = 0; ulCount < ulMinSpeedNum ; ulCount++)
		{
			pSpeeds[ulCount] = pnSpeedArray[ulCount];
		}

		*pulNumSpeed = (HINT32)ulCount;

		return ERR_OK;
	}

	return ERR_FAIL;
}

