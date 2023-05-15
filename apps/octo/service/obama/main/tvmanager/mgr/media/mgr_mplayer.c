/*******************************************************************************
*
* File Name    : mgr_mplayer.c
*
* Description  : new media player body for OCTO 2.0
*
*
* Copyright (C) 2012, Humax Co., Ltd. All rights reserved.
*
* Revision History :
*
* Date        Modification                                  Name
* ----        ------------                                 --------
* 2012/01/09	createded									ultracat,jwkim
* 2012/01/09	implementation								jwkim
*
*******************************************************************************/

#ifdef CONFIG_SUPPORT_IMMA
#include <svc_mplayercso.h>
#else
#include <svc_mplayer.h>
#endif
#include <svc_output.h>

#include <mgr_common.h>
#include <mgr_action.h>
#include <mgr_media.h>
#include <pmgr_media.h>

/*******************************************************************************
*	Typedef
*******************************************************************************/

#define SUPPORTED_URI_DLNA 				"hdlna://"

/*******************************************************************************
*	Static Variables
*******************************************************************************/
STATIC HINT32			 s_RefCnt = 0;
STATIC MGR_MPlayer_t	*s_ahPlayers[NUM_VIEW_ACTION];

/*******************************************************************************
*	Static functions
*******************************************************************************/
STATIC BUS_Result_t	proc_mplayer_Action1(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
STATIC BUS_Result_t	proc_mplayer_Action2(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);

STATIC HINT32 mgr_mplayer_NotifyMessage(HINT32 nMsg, Handle_t hAct, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	BUS_Result_t	hRes = BUS_PostMessage(NULL, nMsg, hAct, nParam1, nParam2, nParam3);

	return (hRes != ERR_BUS_NO_ERROR) ? ERR_FAIL : ERR_OK;
}

#ifdef CONFIG_SUPPORT_IMMA
STATIC inline SvcCso_t *__SVC_CSO(MGR_MPlayer_t *mplayer)
{
	return (SvcCso_t *)mplayer;
}
#else
STATIC inline SvcMp_MediaPlayer_t *__mp(MGR_MPlayer_t *hPlayer)
{
	return (SvcMp_MediaPlayer_t *)hPlayer;
}
#endif

STATIC BUS_Callback_t mgr_mplayer_GetAction (HUINT32 ulActionId)
{
	if(ulActionId == eActionId_VIEW_0)
	{
		return proc_mplayer_Action1;
	}
	else if(ulActionId == eActionId_VIEW_1)
	{
		return proc_mplayer_Action2;
	}
	else
	{
		HxLOG_Print("Invalid Action ID(%d)\n",ulActionId);
		return NULL;
	}
}

#if 0
STATIC HERROR mgr_mplayer_TakeAction(MGR_MPlayer_t *hPlayer)
{
	HERROR eRet = ERR_OK;
	HUINT32	ulActionId = 0;
	MgrAction_Type_e	eActType;

#ifdef CONFIG_SUPPORT_IMMA
	ulActionId = SVC_CSO_GetActionId(__SVC_CSO(hPlayer));
#else
	ulActionId = SVC_MP_GetActionId(__mp(hPlayer));
#endif
	eActType = MGR_ACTION_GetType (ulActionId);
	if (eViewType_MOVIE_PB != eActType)
	{
		eRet = MGR_ACTION_SetupMovie (ulActionId, eAmPriority_VIEW_ACTION);
		if ( eRet != ERR_OK )
		{
			return ERR_FAIL;
		}

		eRet = MGR_ACTION_Take (ulActionId, eViewType_MOVIE_PB, BUS_MGR_GetMgrCallback(BUS_MGR_GetThis()));
		if ( eRet != ERR_OK )
		{
			return ERR_FAIL;
		}
	}

	return ERR_OK;
}
#endif

#if 0
STATIC HERROR mgr_mplayer_ReleaseAction(MGR_MPlayer_t *hPlayer)
{
#ifdef CONFIG_SUPPORT_IMMA
	return MGR_ACTION_Release(SVC_CSO_GetActionId(__SVC_CSO(hPlayer)), eViewType_MOVIE_PB);
#else
	return MGR_ACTION_Release(SVC_MP_GetActionId(__mp(hPlayer)), eViewType_MOVIE_PB);
#endif
}
#endif

STATIC BUS_Result_t proc_mplayer_Action1(HINT32 message, Handle_t handle, HINT32 param1,
						HINT32 param2, HINT32 param3)
{
	MgrMplayer_Proc_t pfnProc = pmgr_mplayer_GetProc();

	if (pfnProc)
	{
		return pfnProc (s_ahPlayers[eActionId_VIEW_0], message, handle, param1, param2, param3);
	}
	return ERR_BUS_NO_OBJECT;
}

STATIC BUS_Result_t proc_mplayer_Action2(HINT32 message, Handle_t handle, HINT32 param1,
						HINT32 param2, HINT32 param3)
{
	MgrMplayer_Proc_t pfnProc = pmgr_mplayer_GetProc();

	if (pfnProc)
	{
		return pfnProc (s_ahPlayers[eActionId_VIEW_1], message, handle, param1, param2, param3);
	}
	return ERR_BUS_NO_OBJECT;
}

/*******************************************************************************
*	internal public functions
*******************************************************************************/
BUS_Callback_t	MGR_MPlayer_GetProc(HUINT32 ulActionId)
{
	return mgr_mplayer_GetAction (ulActionId);
}

#ifndef CONFIG_SUPPORT_IMMA
SvcMp_MediaPlayer_t *MGR_MPlayer_GetMWPlayer(MGR_MPlayer_t *hPlayer)
{
	return (SvcMp_MediaPlayer_t *)hPlayer;
}
#endif

/*******************************************************************************
*	Interfaces
*******************************************************************************/
MGR_MPlayer_t *MGR_MPlayer_Create(HUINT32 hActionId)
{
	MGR_MPlayer_t *pstPlayer=NULL;

#ifdef CONFIG_SUPPORT_IMMA
	pstPlayer = (MGR_MPlayer_t *)SVC_CSO_GetActivePlayer(hActionId);
#else
	pstPlayer = (MGR_MPlayer_t *)SVC_MP_GetActivePlayer(hActionId);
#endif
	if ( pstPlayer == NULL )
	{
#ifdef CONFIG_SUPPORT_IMMA
		/* PostMsg Callback can't need, Register SVC_CSO_Init(PostMsg callback) of Obama_Init()*/
		pstPlayer = (MGR_MPlayer_t *)SVC_CSO_Create(hActionId, NULL);
#else
		pstPlayer = (MGR_MPlayer_t *)SVC_MP_CreatePlayer(hActionId);
#endif
		if ( pstPlayer == NULL )
		{
			return NULL;
		}
	}
	s_RefCnt++;

	HxLOG_Print("Succeeded to create player. (s_RefCnt = %d)\n", s_RefCnt);

	s_ahPlayers[hActionId] = pstPlayer;

	return pstPlayer;
}

void MGR_MPlayer_Destroy(MGR_MPlayer_t *hPlayer)
{
	if ( --s_RefCnt <= 0 )
	{
		HxLOG_Print("Succeeded to destroy player. (s_RefCnt = %d)\n", s_RefCnt);
#ifdef CONFIG_SUPPORT_IMMA
		SVC_CSO_Destroy(__SVC_CSO(hPlayer));
		if(__SVC_CSO(hPlayer)->pszUrl != NULL)
		{
			HLIB_STD_MemFree(__SVC_CSO(hPlayer)->pszUrl);
			__SVC_CSO(hPlayer)->pszUrl = NULL;
		}
		s_ahPlayers[SVC_CSO_GetActionId(__SVC_CSO(hPlayer))] = NULL;
#else
		SVC_MP_DestroyPlayer(__mp(hPlayer));
		s_ahPlayers[SVC_MP_GetActionId(__mp(hPlayer))] = NULL;
#endif
		return;
	}

	HxLOG_Print("Failed to destroy player. (s_RefCnt = %d)\n", s_RefCnt);
}

MGR_MPlayer_t *MGR_MPlayer_GetActivePlayer(HUINT32 hActionId)
{
#ifdef CONFIG_SUPPORT_IMMA
	return SVC_CSO_GetActivePlayer(hActionId);
#else
	return SVC_MP_GetActivePlayer(hActionId);
#endif
}

HERROR MGR_MPlayer_Probe(MGR_MPlayer_t *hPlayer, const HINT8 *szUrl, MgrMplayer_Type_e ePlayType)
{
	BUS_Result_t eBusErr = ERR_BUS_NO_ERROR;
	HUINT32 ulActionId = 0;

#ifdef CONFIG_SUPPORT_IMMA
	SvcCso_t * SvcCso_t = __SVC_CSO(hPlayer);
	ulActionId = SVC_CSO_GetActionId (__SVC_CSO(hPlayer));
#else
	ulActionId = SVC_MP_GetActionId (__mp(hPlayer));
#endif
	switch ( ulActionId )
	{
	case eActionId_VIEW_0:
		if(BUS_MGR_Get(proc_mplayer_Action1) == NULL)
		{
			eBusErr = BUS_MGR_Create("proc_mplayer_Action1", APP_ACTION_PRIORITY, proc_mplayer_Action1,
				0, 0, 0, 0);
		}
		break;

	case eActionId_VIEW_1:
		if(BUS_MGR_Get(proc_mplayer_Action2) == NULL)
		{
			eBusErr = BUS_MGR_Create("proc_mplayer_Action2", APP_ACTION_PRIORITY, proc_mplayer_Action2,
				0, 0, 0, 0);
		}
		break;
	default:
		return ERR_FAIL;
	}

	if ( eBusErr != ERR_BUS_NO_ERROR ) return ERR_FAIL;

#ifdef CONFIG_SUPPORT_IMMA
	/* SVC_CSO Sholud know the URL for Subtitle...*/
	if ( szUrl )
	{
		HINT32 nUrlLen = 0;
		nUrlLen = HxSTD_StrLen(szUrl);

		if(SvcCso_t->pszUrl != NULL)
		{
			HLIB_STD_MemFree(SvcCso_t->pszUrl);
			SvcCso_t->pszUrl = NULL;
		}
		SvcCso_t->pszUrl = HLIB_STD_MemAlloc(nUrlLen+1);
		if(SvcCso_t->pszUrl == NULL)
		{
			HxLOG_Error("Out of Memory!\n");
			return ERR_FAIL;
		}
		HxSTD_MemSet(SvcCso_t->pszUrl, 0, nUrlLen+1);
		HxSTD_StrNCpy(SvcCso_t->pszUrl, szUrl, nUrlLen);
	}
#else
	return SVC_MP_Probe(__mp(hPlayer), (SvcMp_PlayType_e)ePlayType, szUrl);
#endif
	return ERR_OK;
}

HERROR MGR_MPlayer_Play(MGR_MPlayer_t *hPlayer, HUINT32 ulStartTime)
{
	BUS_Result_t eBusErr = ERR_BUS_NO_ERROR;
	HUINT32 ulActionId = 0;

#ifdef CONFIG_SUPPORT_IMMA
	SvcCso_t * SvcCso_t = __SVC_CSO(hPlayer);
	ulActionId = SVC_CSO_GetActionId (__SVC_CSO(hPlayer));
#else
	ulActionId = SVC_MP_GetActionId (__mp(hPlayer));
#endif
	switch ( ulActionId )
	{
	case eActionId_VIEW_0:
		if(BUS_MGR_Get(proc_mplayer_Action1) == NULL)
		{
			eBusErr = BUS_MGR_Create("proc_mplayer_Action1", APP_ACTION_PRIORITY, proc_mplayer_Action1,
				0, 0, 0, 0);
		}
		break;

	case eActionId_VIEW_1:
		if(BUS_MGR_Get(proc_mplayer_Action2) == NULL)
		{
			eBusErr = BUS_MGR_Create("proc_mplayer_Action2", APP_ACTION_PRIORITY, proc_mplayer_Action2,
				0, 0, 0, 0);
		}
		break;
	default:
		return ERR_FAIL;
	}

	if ( eBusErr != ERR_BUS_NO_ERROR ) return ERR_FAIL;

#ifdef CONFIG_SUPPORT_IMMA
	HxLOG_Debug("call SVC_CSO_Play(url:%s)\n", SvcCso_t->pszUrl);
	return SVC_CSO_Play(__SVC_CSO(hPlayer), ulStartTime);
#else /* CONFIG_SUPPORT_IMMA */
	return SVC_MP_Play(__mp(hPlayer), ulStartTime);
#endif /* CONFIG_SUPPORT_IMMA */

}

HERROR MGR_MPlayer_Reset(MGR_MPlayer_t *hPlayer)
{
	pmgr_mplayer_CaptionOnOff(hPlayer, FALSE);

#ifdef CONFIG_SUPPORT_IMMA
	(void)SVC_CSO_Stop(__SVC_CSO(hPlayer));
#else
	(void)SVC_MP_Stop(__mp(hPlayer));
#endif

	return ERR_OK;
}

HERROR MGR_MPlayer_Stop(MGR_MPlayer_t *hPlayer)
{
	BUS_Callback_t actionProc = NULL;
	HUINT32 ulActionId;

	MGR_MPlayer_Reset(hPlayer);

#ifdef CONFIG_SUPPORT_IMMA
	ulActionId = SVC_CSO_GetActionId (__SVC_CSO(hPlayer));
#else
	ulActionId = SVC_MP_GetActionId (__mp(hPlayer));
#endif

	// 명시적으로 media player stop이 호출될 경우 Flag 설정을 위해서 내부 message 호출
	actionProc = mgr_mplayer_GetAction(ulActionId);
	if(actionProc == NULL)
	{
		return ERR_FAIL;
	}

	BUS_SendMessage(mgr_mplayer_GetAction(ulActionId),  eMEVT_MEDIA_MPLAYER_EXPLICIT_STOP, (Handle_t)NULL, 0, 0, 0);

	return ERR_OK;
}

HERROR MGR_MPlayer_SetSaveThumbnail(MGR_MPlayer_t *hPlayer, HINT8 *szFileName, HINT32 nWidth, HINT32 nHeight )
{
	HUINT32 ulVDecID = 0;

#ifdef CONFIG_SUPPORT_IMMA
	HUINT32 ulActionID = SVC_CSO_GetActionId(__SVC_CSO(hPlayer));
#else
	HUINT32 ulActionID = SVC_MP_GetActionId(__mp(hPlayer));
#endif

	(void)SVC_PIPE_GetResourceId(ulActionID, eRxRSCTYPE_AV_DEC, &ulVDecID);

	return SVC_OUT_CaptureVideoToPngFile(ulVDecID, nWidth, nHeight, szFileName);
}

HERROR MGR_MPlayer_SetSubtitleUsage(MGR_MPlayer_t *hPlayer, HBOOL bUse)
{
	HERROR herr = ERR_OK;
	
#ifdef CONFIG_SUPPORT_IMMA
	/* This is workarround for file subtitle issue in switching between file and MSE playback */
	/* Reference http://svn:3000/issues/110709 */
	herr = SVC_CSO_SetSubtitleUsage(__SVC_CSO(hPlayer), bUse);
#else 
	/* Unsupported at SVC_MP; */
	/* herr = SVC_MP_SetSubtitleUsage(__mp(hPlayer), bUse); */
#endif

	return herr;
}

HERROR MGR_MPlayer_SI_Start(MGR_MPlayer_t *hPlayer)
{
#ifdef CONFIG_SUPPORT_ROVI
	SvcSi_MediaPbSpec_t 	stSiSpec;
	HERROR					hErr = ERR_OK;	
	HUINT32 				ulActionId = 0;

	ulActionId = SVC_CSO_GetActionId (__SVC_CSO(hPlayer));
	if(ulActionId > eActionId_VIEW_LAST)
	{
		HxLOG_Error("SVC_CSO_GetActionId Error\n");
		return ERR_FAIL;	
	}
	
	hErr = MGR_PLAYBACK_GetMediaPbSiSpec(&stSiSpec);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("MGR_PLAYBACK_GetMediaPbSiSpec Error\n");
		return hErr;
	}
	else
	{
		hErr = SVC_SI_StartMediaPb(SVC_PIPE_GetActionHandle(ulActionId), &stSiSpec, FALSE);
		if (hErr != ERR_OK)
		{
			HxLOG_Error("SVC_SI_StartMediaPb Error !!\n");
		}
	}
	return hErr;
#else /* CONFIG_SUPPORT_ROVI */
	UNUSED(hPlayer);
	HxLOG_Error("%s(%d) Not Support SI\n", __FUNCTION__, __LINE__);
	return ERR_FAIL;
#endif /* CONFIG_SUPPORT_ROVI */
}

HERROR MGR_MPlayer_SI_Stop(MGR_MPlayer_t *hPlayer)
{
#ifdef CONFIG_SUPPORT_ROVI
	HERROR					hErr = ERR_OK;	
	HUINT32 				ulActionId = 0;

	ulActionId = SVC_CSO_GetActionId (__SVC_CSO(hPlayer));
	if(ulActionId > eActionId_VIEW_LAST)
	{
		HxLOG_Error("SVC_CSO_GetActionId Error\n");
		return ERR_FAIL;	
	}
	
	hErr = SVC_SI_StopMediaPb(SVC_PIPE_GetActionHandle(ulActionId), FALSE);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("SVC_SI_StopMediaPb Error\n");
	}

	return hErr;
#else /* CONFIG_SUPPORT_ROVI */
	UNUSED(hPlayer);
	HxLOG_Error("%s(%d) Not Support SI\n", __FUNCTION__, __LINE__);
	return ERR_FAIL;
#endif /* CONFIG_SUPPORT_ROVI */
}


HERROR MGR_MPlayer_DVB_Sbtl_Start(MGR_MPlayer_t *hPlayer, HUINT16 usPid, HUINT16 usCompPageId, HUINT16 usAncPageId)
{
	BUS_Callback_t actionProc = NULL;
	HUINT32 ulActionId;

#ifdef CONFIG_SUPPORT_IMMA
	ulActionId = SVC_CSO_GetActionId (__SVC_CSO(hPlayer));
#else
	ulActionId = SVC_MP_GetActionId (__mp(hPlayer));
#endif

	// 명시적으로 media player stop이 호출될 경우 Flag 설정을 위해서 내부 message 호출
	actionProc = mgr_mplayer_GetAction(ulActionId);
	if(actionProc == NULL)
	{
		return ERR_FAIL;
	}

	BUS_SendMessage(mgr_mplayer_GetAction(ulActionId),	eMEVT_MEDIA_DVB_SUBTITLE_START, (Handle_t)NULL, usPid, usCompPageId, usAncPageId);

	return ERR_OK;
}

HERROR MGR_MPlayer_DVB_Sbtl_Stop(MGR_MPlayer_t *hPlayer)
{
	BUS_Callback_t actionProc = NULL;
	HUINT32 ulActionId;

#ifdef CONFIG_SUPPORT_IMMA
	ulActionId = SVC_CSO_GetActionId (__SVC_CSO(hPlayer));
#else
	ulActionId = SVC_MP_GetActionId (__mp(hPlayer));
#endif

	// 명시적으로 media player stop이 호출될 경우 Flag 설정을 위해서 내부 message 호출
	actionProc = mgr_mplayer_GetAction(ulActionId);
	if(actionProc == NULL)
	{
		return ERR_FAIL;
	}

	BUS_SendMessage(mgr_mplayer_GetAction(ulActionId),	eMEVT_MEDIA_DVB_SUBTITLE_STOP, (Handle_t)NULL, 0, 0, 0);

	return ERR_OK;
}



