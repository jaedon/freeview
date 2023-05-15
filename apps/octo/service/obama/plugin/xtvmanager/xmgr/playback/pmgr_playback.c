/**
	@file     	pmgr_playback.c
	@brief    	humax recorded file & single program ts file playback control application.	\n

	Description:  						\n
	Module: 		ap/pvr			 	\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#include <isosvc.h>
#include <db_svc.h>
#include <db_param.h>

#include <svc_fs.h>
#include <svc_rec.h>
#include <svc_pb.h>
#include <svc_meta.h>
#include <svc_sbtl.h>

#include <svc_si.h>

#include <bus.h>
#include <mgr_common.h>
#include <mgr_action.h>

#include <mgr_playback.h>
#include <pmgr_playback.h>

#include <_xmgr_playback.h>

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
/********************      Functions       *************************/
/*******************************************************************/

HERROR pmgr_playback_Init(void)
{
	return xmgr_playback_Init_Base ();
}

HERROR pmgr_playback_SetPlaySetup (HUINT32 ulActionId, MgrPb_PlaySetup_t *pstPbSetup)
{
	return xmgr_playback_SetPlaySetup_Base (ulActionId, pstPbSetup);
}

BUS_Callback_t	pmgr_playback_GetProcFunc(HUINT32 ulActionId)
{
#if defined(CONFIG_OP_SES)
	return xmgr_playback_GetProcFunc_Ses (ulActionId);
#elif defined (CONFIG_MW_CI_PLUS)
	return xmgr_playback_CiPlusGetProcFunc_Base (ulActionId);
#elif (CONFIG_MW_CAS_IRDETO_PVR)
	return xmgr_playback_IrdetoGetProcFunc_Base (ulActionId);
#elif (CONFIG_MW_CAS_NAGRA_OP_UPC)
	return xmgr_playback_NagraGetProcFunc_Base (ulActionId);
//#elif (CONFIG_OP_UK_DTT)
//	return xmgr_playback_GetProcFunc_Freeview (ulActionId);
#elif defined(CONFIG_MW_CAS_CONAX)
	return xmgr_playback_ConaxGetProcFunc_Base (ulActionId);
#elif defined(CONFIG_MW_CAS_VERIMATRIX)
	return xmgr_playback_VerimatrixGetProcFunc_Base (ulActionId);
#else
	return xmgr_playback_GetProcFunc_Base (ulActionId);
#endif
}

HERROR pmgr_playback_SetPlaySpeed (HUINT32 ulActionId, HINT32 nPlaySpeed)
{
	return xmgr_playback_SetPlaySpeed_Base (ulActionId, nPlaySpeed);
}

HERROR pmgr_playback_SetJumpPlay(HUINT32 ulActionId, HUINT32 ulTimeToPlay)
{
#if defined(CONFIG_MW_CAS_CONAX)
	return xmgr_playback_ConaxSetJumpPlay_Base (ulActionId, ulTimeToPlay);
#else
	return xmgr_playback_SetJumpPlay_Base (ulActionId, ulTimeToPlay);
#endif
}

HERROR pmgr_playback_GetPlaybackInfo (HUINT32 ulActionId, MgrPb_Info_t *pstInfo)
{
	return xmgr_playback_GetPlaybackInfo_Base (ulActionId, pstInfo);
}

HERROR pmgr_playback_GetPlaybackTimeInfo (HUINT32 ulActionId, MgrPb_Info_t *pstInfo)
{
	return xmgr_playback_GetPlaybackTimeInfo_Base (ulActionId, pstInfo);
}

HERROR pmgr_playback_StopPlayback (HUINT32 ulActionId)
{
	return xmgr_playback_StopPlayback_Base (ulActionId);
}

HERROR pmgr_playback_SetAudioInfo (HUINT32 ulActionId, HUINT32 ulIndex, DxStereoSelect_e eSoundMode)
{
	return xmgr_playback_SetAudioInfo_Base (ulActionId, ulIndex, eSoundMode);
}

HERROR pmgr_playback_SetSubTitleDisplay (HUINT32 ulActionId, HBOOL bDisplay)
{
	return xmgr_playback_SetSubTitleDisplay_Base (ulActionId, bDisplay);
}

HERROR pmgr_playback_SetSubtitleLanguage (HUINT32 ulActionId, HUINT32 ulIndex)
{
	return xmgr_playback_SetSubtitleLanguage_Base (ulActionId, ulIndex);
}

HERROR pmgr_playback_GetSubtitleDisplay (HUINT32 ulActionId, HBOOL *pbDisplay)
{
	return xmgr_playback_GetSubtitleDisplay_Base (ulActionId, pbDisplay);
}

HERROR pmgr_playback_GetServiceInfo (HUINT32 ulActionId, SvcMeta_Service_t *pstService)
{
	return xmgr_playback_GetServiceInfo_Base (ulActionId, pstService);
}

HERROR pmgr_playback_SetServiceInfo (HUINT32 ulActionId, SvcMeta_Service_t *pstService, HBOOL bUpdate)
{
	return xmgr_playback_SetServiceInfo_Base (ulActionId, pstService, bUpdate);
}

HUINT32	pmgr_playback_GetPlaybackParam (MgrPb_ParamType_e eType)
{
	return xmgr_playback_GetPlaybackParam_Base (eType);
}

HBOOL pmgr_playback_IsPlaybackId(HUINT32 ulContentId, MgrPb_Info_t *pstInfo)
{
	return xmgr_playback_IsPlaybackId_Base (ulContentId, pstInfo);
}

HERROR pmgr_playback_GetMenuSubtitleLangIndex(DbParam_Item_e eSubtitleItem, SvcSi_SubtitleList_t *pstSubtitleList, HINT32 *pnIndex)
{
	return xmgr_playback_GetMenuSubtitleLangIndex(eSubtitleItem, pstSubtitleList, pnIndex);
}

