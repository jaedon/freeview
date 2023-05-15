/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  xmgr_schedule_base.c
	@brief

	Description:  									\n
	Module: OBAMA / plugin / tvmanager / schedule	\n

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
#include <hapi.h>
#include <sapi.h>
#include <papi.h>
#include <octo_common.h>
#include <otl.h>

#include <db_svc.h>
#include <db_param.h>

#include <svc_fs.h>
#include <svc_pipe.h>
#include <svc_ringbuf.h>
#include <svc_mediaserver.h>
#include <svc_output.h>
#include <svc_sys.h>

#include <mgr_common.h>
#include <mgr_action.h>
#include <mgr_schedule.h>
#include <mgr_live.h>
#include <mgr_pvr_action.h>
#include <mgr_satip.h>
#include <mgr_ntls.h>
#include <mgr_epg.h>
#if defined(CONFIG_APCORE_EMM_UPDATE)
#include <mgr_emmupdate.h>
#endif
#if defined(CONFIG_MW_SATRECORD)
#include <mgr_satrecord.h>
#endif
#include <mgr_storage.h>
#include <om_common.h>
#include "../local_include/_xmgr_schedule.h"

/*******************************************************************/
/********************      Local Definition         *************************/
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
STATIC XmgrSch_BaseContext_t		 stXmgrSchedBase_Context;

#define ___LOCAL_FUNCTIONS___
STATIC HBOOL xmgr_schedule_CmpScheduleSlot_Base (void *pvUserData, void *pvListData)
{
	DxSchedule_t			*pstSchedule = (DxSchedule_t *)pvUserData;
	XmgrSch_RunningRec_t	*pstRunRec = (XmgrSch_RunningRec_t *)pvListData;

	if ((NULL != pstSchedule) && (NULL != pstRunRec))
	{
		if (pstSchedule->ulSlot == pstRunRec->ulSlot)
		{
			return TRUE;
		}
	}

	return FALSE;
}


STATIC HUINT32 s_eStandbySch[] =
{
	DxRSVTYPE_EMM,
	DxRSVTYPE_DESC_REC_STANDBY,
	DxRSVTYPE_CHECK_SATRECORD_TIMER
};


STATIC HERROR xmgr_schedule_CancelStandbySchedule_Base (void)
{
	HUINT32 		ulCnt = 0, ulNumSch = 0, ulNumSch2 = 0;
	DxSchedule_t	*pstSchArray = NULL;
	HERROR			hErr = ERR_FAIL;
	DxRsvType_e		eRsvType;
	HUINT32			i = 0;
	HUINT32			ulTotalCnt = 0;

	HxLOG_Trace();

	ulTotalCnt = sizeof(s_eStandbySch) / sizeof(HUINT32);

	for(i=0; i<ulTotalCnt; i++)
	{
		eRsvType = s_eStandbySch[i];

		ulNumSch = 0;
		hErr = SAPI_CountScheduleByType(eRsvType, &ulNumSch);
		HxLOG_Debug("Schedule Count ... (%d) \n", ulNumSch);
		if(hErr == ERR_OK && ulNumSch > 0)
		{
			pstSchArray = (DxSchedule_t *)HLIB_STD_MemAlloc(sizeof(DxSchedule_t) * ulNumSch);
			if (pstSchArray)
			{
				hErr = SAPI_GetAllSchedulesByRsvType(ulNumSch, &ulNumSch2, pstSchArray, eRsvType);
				if(hErr == ERR_OK)
				{
					for (ulCnt = 0; ulCnt < ulNumSch2; ulCnt++)
					{
						DxSchedule_t	*pstSchedule = &(pstSchArray[ulCnt]);
						if(pstSchedule != NULL && pstSchedule->eRsvType == eRsvType)
						{
							HxLOG_Debug("Cancel Schedule Slot (%d) \n", pstSchedule->ulSlot);
							SAPI_CancelSchedule(pstSchedule->ulSlot);
						}
					}
				}
				HLIB_STD_MemFree(pstSchArray);
			}
		}
	}

	return ERR_OK;
}

STATIC HERROR xmgr_schedule_ChangePaddingTime_Base (HUINT32 ulOffsetTime, HBOOL bIsPreOffset)
{

	HxLOG_Trace();
	(void)SAPI_ChangePaddingTime(ulOffsetTime, bIsPreOffset);

	return ERR_OK;

}

#if defined(CONFIG_STREAMING_SATIP)
STATIC HERROR xmgr_schedule_StartSATIP_Base(HUINT32 ulSlot, DxRSV_Streaming_t *pStreamInfo, Handle_t hRingBuf, HUINT32 *pulActionId)
{
	HERROR				hErr = ERR_FAIL;
	MgrSatIp_Setup_t	stSetup;
	DbSvc_AntInfo_t		stAntInfo;
	DbSvc_TripleId_t	stTripleId;
	Handle_t			hSvcHandle = NULL;
	HINT32				i;

	if ( (NULL == pStreamInfo) || (HANDLE_NULL == hRingBuf) || (NULL == pulActionId) )
	{
		HxLOG_Error("Null handle : pStreamInfo(0x%x), hRingBuf(0x%x), pulActionId(0x%x)\n", pStreamInfo, hRingBuf, pulActionId);
		return ERR_FAIL;
	}

	HxSTD_MemSet (&stSetup, 0, sizeof(MgrSatIp_Setup_t));
	HxSTD_MemCopy (&(stSetup.stTuneParam), &(pStreamInfo->stTuningParam), sizeof(DxTuneParam_t));

	if (eDxDELIVERY_TYPE_SAT == stSetup.stTuneParam.eDeliType)
	{
		HxSTD_MemSet (&stAntInfo, 0, sizeof(DbSvc_AntInfo_t));
		hErr = DB_SVC_GetAntInfo (stSetup.stTuneParam.sat.tuningInfo.antuid, &stAntInfo);
		if (ERR_OK != hErr)
		{
			HxLOG_Critical("DB_SVC_GetAntInfo ret:0x%X(AntId:%d)\n", hErr, stSetup.stTuneParam.sat.tuningInfo.antuid);
			return hErr;
		}

		OTL_CONV_DbAntInfo2DxAntInfo (&stSetup.stTuneParam.sat.antInfo, &stAntInfo);
	}

	/* manual pid */
	if (DxRSVSTREAM_TYPE_SATIP_LIVE_ALLPID == pStreamInfo->eStreamType)
	{
		/* if pid-count is -1, all pid */
		stSetup.nPIDsCount = -1;
	}
	else if (pStreamInfo->nPIDsCount > 0)
	{
		HxSTD_MemCopy ( &(stSetup.usPIDs),
						pStreamInfo->usPIDs,
						sizeof(HUINT16) * pStreamInfo->nPIDsCount );

		stSetup.nPIDsCount = pStreamInfo->nPIDsCount;
	}

	if((TRUE == pStreamInfo->bTranscoding) || (TRUE == pStreamInfo->bDrm))
	{
		HxSTD_MemCopy (&(stTripleId), &(pStreamInfo->stTripleId), sizeof(DbSvc_TripleId_t));
		hErr = DB_SVC_FindServiceHandleByTripleId(stSetup.stTuneParam.eDeliType, stTripleId, &hSvcHandle);
		if (ERR_OK != hErr)
		{
			HxLOG_Error("Cannot find SvcHandle: stTripleId(%d)\n", stTripleId);
			return hErr;
		}
		stSetup.stStartArg.hSvcHandle = hSvcHandle;
	}

	HxSTD_MemCopy (&(stSetup.stStartArg.stProfile), &(pStreamInfo->stProfile), sizeof(MgrSatIp_TranscoderStreamProfile));
	stSetup.stStartArg.bTranscode			= pStreamInfo->bTranscoding;
	stSetup.stStartArg.bPcrTsPacketInsert	= pStreamInfo->bPcrTsPacketInsert;
	stSetup.bDrm		= pStreamInfo->bDrm;
	stSetup.hRingBuf	= hRingBuf;
	/* Save RsvSlot */
	stSetup.ulRsvSlotId	= ulSlot;
	stSetup.eAllocMode	= pStreamInfo->eAllocMode;
	stSetup.usPriority	= pStreamInfo->usPriority;

	HxLOG_Debug ("\t**<SCH:SATIP>**\n");
	HxLOG_Debug ("\t<hRingBuf\t\t\t\t:\t%x>\n",		stSetup.hRingBuf);
	HxLOG_Debug ("\t<ulRsvSlotId\t\t\t\t:\t%d>\n",	stSetup.ulRsvSlotId);
	HxLOG_Debug ("\t<AllocMode\t\t\t\t:\t%d>\n",	stSetup.eAllocMode);
	HxLOG_Debug ("\t<Priority\t\t\t\t:\t%d>\n",		stSetup.usPriority);
	HxLOG_Debug ("\t<hSvcHandle\t\t\t\t:\t0x%x>\n",	stSetup.stStartArg.hSvcHandle);
	HxLOG_Debug ("\t<bTranscoding\t\t\t\t:\t%d>\n",	pStreamInfo->bTranscoding);
	HxLOG_Debug ("\t<bDrm\t\t\t\t\t:\t%d>\n",		pStreamInfo->bDrm);
	HxLOG_Debug ("\t<nPIDsCount\t\t\t\t:\t%d>\n",	stSetup.nPIDsCount);
	{
		for (i=0 ; i<stSetup.nPIDsCount ; i++)
		{
			HxLOG_Debug ("\t(%d) [0x%x] = [%d]\n", i, stSetup.usPIDs[i], stSetup.usPIDs[i]);
		}
		HxLOG_Debug("\n");
	}

	HxLOG_Debug ("\t<TunerPararm:eDeliType\t\t\t:\t%d>\n", 				pStreamInfo->stTuningParam.eDeliType);
	HxLOG_Debug ("\t<TunerPararm:ulTunerGroupId\t\t:\t%d>\n", 			pStreamInfo->stTuningParam.sat.ulTunerGroupId);
	HxLOG_Debug ("\t<TunerPararm:ulRfInputId\t\t:\t%d>\n", 				pStreamInfo->stTuningParam.sat.ulRfInputId);
	HxLOG_Debug ("\t<TunerPararm:antennaType\t\t:\t%d>\n", 				pStreamInfo->stTuningParam.sat.antennaType);
	HxLOG_Debug ("\t<TunerPararm:TuningTPInfo:ulFrequency\t:\t%d>\n", 	pStreamInfo->stTuningParam.sat.tuningInfo.ulFrequency);
	HxLOG_Debug ("\t<TunerPararm:TuningTPInfo:ulSymbolRate\t:\t%d>\n",	pStreamInfo->stTuningParam.sat.tuningInfo.ulSymbolRate);
	HxLOG_Debug ("\t<TunerPararm:TuningTPInfo:eFecCodeRate\t:\t%d>\n",	pStreamInfo->stTuningParam.sat.tuningInfo.eFecCodeRate);
	HxLOG_Debug ("\t<TunerPararm:TuningTPInfo:ePolarization\t:\t%d>\n", pStreamInfo->stTuningParam.sat.tuningInfo.ePolarization);
	HxLOG_Debug ("\t<TunerPararm:TuningTPInfo:eTransSpec\t:\t%d>\n", 	pStreamInfo->stTuningParam.sat.tuningInfo.eTransSpec);
	HxLOG_Debug ("\t<TunerPararm:TuningTPInfo:ePskMod\t:\t%d>\n", 		pStreamInfo->stTuningParam.sat.tuningInfo.ePskMod);
	HxLOG_Debug ("\t<TunerPararm:TuningTPInfo:ePilot\t:\t%d>\n", 		pStreamInfo->stTuningParam.sat.tuningInfo.ePilot);
	HxLOG_Debug ("\t<TunerPararm:TuningTPInfo:eRollOff\t:\t%d>\n", 		pStreamInfo->stTuningParam.sat.tuningInfo.eRollOff);
	HxLOG_Debug ("\t<TunerPararm:TuningTPInfo:satType\t:\t%s>\n", 		pStreamInfo->stTuningParam.sat.tuningInfo.satType);
	HxLOG_Debug ("\t<TunerPararm:TuningTPInfo:antuid\t:\t%d>\n", 		pStreamInfo->stTuningParam.sat.tuningInfo.antuid);

	hErr = MGR_SATIP_Start (&stSetup, pulActionId);
	if(ERR_OK != hErr)
	{
		HxLOG_Error("MGR_SATIP_Start failed:\n");
		return hErr;
	}

	return hErr;
}
#endif

#if defined(CONFIG_NON_TRANSCODING_STREAMING)
STATIC HERROR xmgr_schedule_StartLiveNTLS_Base(DxRSV_Streaming_t *pStreamInfo,HUINT32 ulSlot, HUINT32 ulSvcUid, Handle_t hRingBuf, HUINT32 *pulActionId)
{
	HERROR			hErr = ERR_FAIL;
	Handle_t		hSvc = HANDLE_NULL;
	MgrNtls_Setup_t	stSetup;

	if ( (NULL == pStreamInfo) || (HANDLE_NULL == hRingBuf) || (NULL == pulActionId) )
		return ERR_FAIL;

	hErr = DB_SVC_GetServiceHandleBySvcIdx((HUINT16)(ulSvcUid & 0xFFFF), &hSvc);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("Cannot find SvcHandle: ulSvcUid(%d)\n", ulSvcUid);
		return ERR_FAIL;
	}

	HxSTD_MemSet(&stSetup, 0, sizeof(MgrNtls_Setup_t));
	stSetup.hSvc		= hSvc;
	stSetup.hRingBuf	= hRingBuf;

	HxLOG_Debug ("\t**<SCH:LIVE_NTLS>**\n");
	HxLOG_Debug ("\t<ulSvcUid :%d>\n", ulSvcUid);
	HxLOG_Debug ("\t<hSvc     :%d>\n", stSetup.hSvc);
	HxLOG_Debug ("\t<hRingBuf :%d>\n", stSetup.hRingBuf);

	hErr = MGR_NTLS_Start (&stSetup, pulActionId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("MGR_LIVE_Start failed:\n");
		return ERR_FAIL;
	}

	return hErr;
}

STATIC HERROR xmgr_schedule_StartPvrNTLS_Base(DxRSV_Streaming_t *pStreamInfo, Handle_t hRingBuf, HUINT32 *pulActionId)
{
	HERROR hErr = ERR_FAIL;
#if 0
	MgrPvr_PbTrcdToMemSetup_t stStartArg;


	if ( (NULL == pStreamInfo) || (HANDLE_NULL == hRingBuf) || (NULL == pulActionId) )
		return ERR_FAIL;

	HxSTD_MemSet(&stStartArg, 0, sizeof(MgrPvr_PbTrcdToMemSetup_t));

	stStartArg.ulContentId			=	pStreamInfo->ulContentUId;
	stStartArg.hRingBuf				=	hRingBuf;
	stStartArg.bPcrTsPacketInsert	=	pStreamInfo->bPcrTsPacketInsert;
	stStartArg.bTranscode			=	pStreamInfo->bTranscoding;

	/* get profile from json string */
	hErr = MGR_PVR_GetProfileFromJSON(pStreamInfo->szProfileString, &(stStartArg.stProfile));
	if (ERR_OK != hErr)
	{
		HxLOG_Error("MGR_LIVE_GetProfileFromJSON error:\n");
		return ERR_FAIL;
	}

	HxLOG_Debug ("\t**<SCH:PVR_NTLS>**\n");
	HxLOG_Debug ("\t<ulContentId\t:%d>\n",			stStartArg.ulContentId);
	HxLOG_Debug ("\t<hRingBuf\t:%d>\n",				stStartArg.hRingBuf);
	HxLOG_Debug ("\t<bPcrTsPacketInsert\t:%d>\n", 	stStartArg.bPcrTsPacketInsert);
	HxLOG_Debug ("\t<bTranscode\t:%d>\n", 			stStartArg.bTranscode);

	HxLOG_Debug ("\t<Profile:eVideoCodec\t:%d>\n",			stStartArg.stProfile.eVideoCodec);
	HxLOG_Debug ("\t<Profile:eAudioCodec\t:%d>\n",			stStartArg.stProfile.eAudioCodec);
	HxLOG_Debug ("\t<Profile:nAudioBitrateKbps\t:%d>\n",	stStartArg.stProfile.nAudioBitrateKbps);
	HxLOG_Debug ("\t<Profile:nVideoBitrateKbps\t:%d>\n",	stStartArg.stProfile.nVideoBitrateKbps);
	HxLOG_Debug ("\t<Profile:nHdVideoBitrateKbps\t:%d>\n",	stStartArg.stProfile.nHdVideoBitrateKbps);
	HxLOG_Debug ("\t<Profile:nHdVideoBitrateKbps\t:%d>\n",	stStartArg.stProfile.nHdVideoBitrateKbps);
	HxLOG_Debug ("\t<Profile:nFrameRate\t:%d>\n",			stStartArg.stProfile.nFrameRate);
	HxLOG_Debug ("\t<Profile:nScreenWidth\t:%d>\n", 		stStartArg.stProfile.nScreenWidth);
	HxLOG_Debug ("\t<Profile:nScreenHeight\t:%d>\n",		stStartArg.stProfile.nScreenHeight);
	HxLOG_Debug ("\t<Profile:bInterlaceMode\t:%d>\n",		stStartArg.stProfile.bInterlaceMode);
	HxLOG_Debug ("\t<Profile:eVideoAspectRatio\t:%d>\n",	stStartArg.stProfile.eVideoAspectRatio);
	HxLOG_Debug ("\t<Profile:eVideoCodecProfile\t:%d>\n",	stStartArg.stProfile.eVideoCodecProfile);
	HxLOG_Debug ("\t<Profile:eVideoCodecLevel\t:%d>\n", 	stStartArg.stProfile.eVideoCodecLevel);
	HxLOG_Debug ("\t<Profile:bUsingLiveAudioCodec\t:%d>\n", stStartArg.stProfile.bUsingLiveAudioCodec);
	HxLOG_Debug ("\t<Profile:bUsingLiveAspectRatio\t:%d>\n",stStartArg.stProfile.bUsingLiveAspectRatio);

	hErr = MGR_PVR_StartPlaybackTranscodeToRingBuf (&stStartArg, pulActionId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("MGR_PVR_StartPlaybackTranscodeToRingBuf error:\n");
		return ERR_FAIL;
	}
#endif
	return hErr;
}
#endif

#if defined(CONFIG_TRANSCODING_STREAMING)
STATIC HERROR xmgr_schedule_StartLiveHLS_Base(DxRSV_Streaming_t *pStreamInfo, HUINT32 ulSvcUid, Handle_t hRingBuf, HUINT32 *pulActionId)
{
	HERROR hErr = ERR_FAIL;
	Handle_t hSvcHandle;
	MgrLive_START_t stStartArg;

	if ( (NULL == pStreamInfo) || (HANDLE_NULL == hRingBuf) || (NULL == pulActionId) )
		return ERR_FAIL;

	hErr = DB_SVC_GetServiceHandleBySvcIdx((HUINT16)(ulSvcUid & 0xFFFF), &hSvcHandle);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("Cannot find SvcHandle: ulSvcUid(%d)\n", ulSvcUid);
		return ERR_FAIL;
	}

	HxSTD_MemSet(&stStartArg, 0, sizeof(MgrLive_START_t));

	stStartArg.eLiveMode	=	eMgrLiveMode_LiveStreaming;
	stStartArg.hMasterSvc	=	hSvcHandle;
	stStartArg.hService 	=	hSvcHandle;

	stStartArg.unExtArg.stLiveStreamArg.hRingBuf = hRingBuf;

	/* get profile from json string */
	hErr = MGR_LIVE_GetProfileFromJSON(pStreamInfo->szProfileString, &stStartArg.unExtArg.stLiveStreamArg);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("MGR_LIVE_GetProfileFromJSON Error\n");
		return ERR_FAIL;
	}

	HxLOG_Debug ("\t**<SCH:LIVE_HLS:MGR_LIVE_Start>**\n");
	HxLOG_Debug ("\t<eLiveMode\t:%d>\n", 	stStartArg.eLiveMode);
	HxLOG_Debug ("\t<hMasterSvc\t:%d>\n",	stStartArg.hMasterSvc);
	HxLOG_Debug ("\t<hService\t:%d>\n", 	stStartArg.hService);

	HxLOG_Debug ("\t<bTranscode\t:%d>, <pStreamInfo->bTranscoding:%d>\n", 			stStartArg.unExtArg.stLiveStreamArg.bTranscode, pStreamInfo->bTranscoding);
	HxLOG_Debug ("\t<bPcrTsPacketInsert\t:%d>\n", 	stStartArg.unExtArg.stLiveStreamArg.bPcrTsPacketInsert);

	HxLOG_Debug ("\t<Profile:eVideoCodec\t:%d>\n",			stStartArg.unExtArg.stLiveStreamArg.stProfile.eVideoCodec);
	HxLOG_Debug ("\t<Profile:eAudioCodec\t:%d>\n",			stStartArg.unExtArg.stLiveStreamArg.stProfile.eAudioCodec);
	HxLOG_Debug ("\t<Profile:nAudioBitrateKbps\t:%d>\n",	stStartArg.unExtArg.stLiveStreamArg.stProfile.nAudioBitrateKbps);
	HxLOG_Debug ("\t<Profile:nVideoBitrateKbps\t:%d>\n",	stStartArg.unExtArg.stLiveStreamArg.stProfile.nVideoBitrateKbps);
	HxLOG_Debug ("\t<Profile:nHdVideoBitrateKbps\t:%d>\n",	stStartArg.unExtArg.stLiveStreamArg.stProfile.nHdVideoBitrateKbps);
	HxLOG_Debug ("\t<Profile:nHdVideoBitrateKbps\t:%d>\n",	stStartArg.unExtArg.stLiveStreamArg.stProfile.nHdVideoBitrateKbps);
	HxLOG_Debug ("\t<Profile:nFrameRate\t:%d>\n",			stStartArg.unExtArg.stLiveStreamArg.stProfile.nFrameRate);
	HxLOG_Debug ("\t<Profile:nScreenWidth\t:%d>\n",			stStartArg.unExtArg.stLiveStreamArg.stProfile.nScreenWidth);
	HxLOG_Debug ("\t<Profile:nScreenHeight\t:%d>\n",		stStartArg.unExtArg.stLiveStreamArg.stProfile.nScreenHeight);
	HxLOG_Debug ("\t<Profile:bInterlaceMode\t:%d>\n",		stStartArg.unExtArg.stLiveStreamArg.stProfile.bInterlaceMode);
	HxLOG_Debug ("\t<Profile:eVideoAspectRatio\t:%d>\n",	stStartArg.unExtArg.stLiveStreamArg.stProfile.eVideoAspectRatio);
	HxLOG_Debug ("\t<Profile:eVideoCodecProfile\t:%d>\n",	stStartArg.unExtArg.stLiveStreamArg.stProfile.eVideoCodecProfile);
	HxLOG_Debug ("\t<Profile:eVideoCodecLevel\t:%d>\n",		stStartArg.unExtArg.stLiveStreamArg.stProfile.eVideoCodecLevel);
	HxLOG_Debug ("\t<Profile:bUsingLiveAudioCodec\t:%d>\n",	stStartArg.unExtArg.stLiveStreamArg.stProfile.bUsingLiveAudioCodec);
	HxLOG_Debug ("\t<Profile:bUsingLiveAspectRatio\t:%d>\n",stStartArg.unExtArg.stLiveStreamArg.stProfile.bUsingLiveAspectRatio);

	stStartArg.unExtArg.stLiveStreamArg.bTranscode 			= pStreamInfo->bTranscoding;
	stStartArg.unExtArg.stLiveStreamArg.bPcrTsPacketInsert	= pStreamInfo->bPcrTsPacketInsert;


	hErr = MGR_LIVE_Start (&stStartArg, pulActionId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("MGR_LIVE_Start failed:\n");
		return ERR_FAIL;
	}

	return hErr;
}

STATIC HERROR xmgr_schedule_StartPvrHLS_Base(DxRSV_Streaming_t *pStreamInfo, Handle_t hRingBuf, HUINT32 *pulActionId)
{
	HERROR hErr = ERR_FAIL;
	MgrPvr_PbTrcdToMemSetup_t stStartArg;


	if ( (NULL == pStreamInfo) || (HANDLE_NULL == hRingBuf) || (NULL == pulActionId) )
		return ERR_FAIL;

	HxSTD_MemSet(&stStartArg, 0, sizeof(MgrPvr_PbTrcdToMemSetup_t));

	stStartArg.ulContentId			=	pStreamInfo->ulContentUId;
	stStartArg.hRingBuf				=	hRingBuf;
	stStartArg.bPcrTsPacketInsert	=	pStreamInfo->bPcrTsPacketInsert;
	stStartArg.bTranscode			=	pStreamInfo->bTranscoding;
	stStartArg.ulStartTime			=	pStreamInfo->ulStartTime;

	/* get profile from json string */
	hErr = MGR_PVR_GetProfileFromJSON(pStreamInfo->szProfileString, &(stStartArg.stProfile));
	if (ERR_OK != hErr)
	{
		HxLOG_Error("MGR_LIVE_GetProfileFromJSON error:\n");
		return ERR_FAIL;
	}

	HxLOG_Debug ("\t**<SCH:PVR_HLS>**\n");
	HxLOG_Debug ("\t<ulContentId\t:%d>\n",			stStartArg.ulContentId);
	HxLOG_Debug ("\t<hRingBuf\t:%d>\n",				stStartArg.hRingBuf);
	HxLOG_Debug ("\t<bPcrTsPacketInsert\t:%d>\n", 	stStartArg.bPcrTsPacketInsert);
	HxLOG_Debug ("\t<bTranscode\t:%d>\n", 			stStartArg.bTranscode);

	HxLOG_Debug ("\t<Profile:eVideoCodec\t:%d>\n",			stStartArg.stProfile.eVideoCodec);
	HxLOG_Debug ("\t<Profile:eAudioCodec\t:%d>\n",			stStartArg.stProfile.eAudioCodec);
	HxLOG_Debug ("\t<Profile:nAudioBitrateKbps\t:%d>\n",	stStartArg.stProfile.nAudioBitrateKbps);
	HxLOG_Debug ("\t<Profile:nVideoBitrateKbps\t:%d>\n",	stStartArg.stProfile.nVideoBitrateKbps);
	HxLOG_Debug ("\t<Profile:nHdVideoBitrateKbps\t:%d>\n",	stStartArg.stProfile.nHdVideoBitrateKbps);
	HxLOG_Debug ("\t<Profile:nHdVideoBitrateKbps\t:%d>\n",	stStartArg.stProfile.nHdVideoBitrateKbps);
	HxLOG_Debug ("\t<Profile:nFrameRate\t:%d>\n",			stStartArg.stProfile.nFrameRate);
	HxLOG_Debug ("\t<Profile:nScreenWidth\t:%d>\n", 		stStartArg.stProfile.nScreenWidth);
	HxLOG_Debug ("\t<Profile:nScreenHeight\t:%d>\n",		stStartArg.stProfile.nScreenHeight);
	HxLOG_Debug ("\t<Profile:bInterlaceMode\t:%d>\n",		stStartArg.stProfile.bInterlaceMode);
	HxLOG_Debug ("\t<Profile:eVideoAspectRatio\t:%d>\n",	stStartArg.stProfile.eVideoAspectRatio);
	HxLOG_Debug ("\t<Profile:eVideoCodecProfile\t:%d>\n",	stStartArg.stProfile.eVideoCodecProfile);
	HxLOG_Debug ("\t<Profile:eVideoCodecLevel\t:%d>\n", 	stStartArg.stProfile.eVideoCodecLevel);
	HxLOG_Debug ("\t<Profile:bUsingLiveAudioCodec\t:%d>\n", stStartArg.stProfile.bUsingLiveAudioCodec);
	HxLOG_Debug ("\t<Profile:bUsingLiveAspectRatio\t:%d>\n",stStartArg.stProfile.bUsingLiveAspectRatio);

	hErr = MGR_PVR_StartPlaybackTranscodeToRingBuf (&stStartArg, pulActionId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("MGR_PVR_StartPlaybackTranscodeToRingBuf error:\n");
		return ERR_FAIL;
	}

	return hErr;
}
#endif

STATIC HERROR	xmgr_schedule_StopRunRec (void *pvContext, Handle_t hAction)
{
	HxList_t				*pstListItem    =       NULL;
	XmgrSch_RunningRec_t    *pstRunRec	      =       NULL;
	XmgrSch_BaseContext_t   *pstContext = (XmgrSch_BaseContext_t *)pvContext;
	HUINT32				 ulStoppedActionId = 0;
	HERROR				  hErr = ERR_FAIL;

	ulStoppedActionId = SVC_PIPE_GetActionId(hAction);
	HxLOG_Debug("pvr action stopped. actionId=(%d) \n",ulStoppedActionId);
	for (pstListItem = HLIB_LIST_First(pstContext->pstRunRecList)
			; pstListItem != NULL
			; pstListItem = HLIB_LIST_Next(pstListItem))
	{
		pstRunRec = HLIB_LIST_Data(pstListItem);
		if (pstRunRec == NULL
			|| pstRunRec->ulActionId != ulStoppedActionId)
		{
			continue;
		}

		HxLOG_Debug("stop slot(%d) \n",pstRunRec->ulSlot);
		hErr = SAPI_CancelSchedule(pstRunRec->ulSlot);
		if (hErr != ERR_OK)
		{
			HxLOG_Error("Fail to cancel stopped pvr pb slot. actionId(%d) slot(%d) \n",ulStoppedActionId,pstRunRec->ulSlot);
		}

		break;
	}

	return MESSAGE_PASS;
}

#if defined(CONFIG_STREAMING_SATIP)
STATIC HERROR	xmgr_schedule_TrySatIp (DxSchedule_t *pstSchedule, HBOOL bForce, HBOOL bTranscode)
{
	MgrSatIp_Setup_t stSetup;
	HERROR			 hErr = ERR_FAIL;
	DxRSV_Streaming_t *pStreamInfo = NULL;

	HxSTD_MemSet(&stSetup, 0, sizeof(MgrSatIp_Setup_t));
	pStreamInfo = &(pstSchedule->uExtInfo.stStrm);
	HxSTD_MemCopy( &(stSetup.stTuneParam), &(pStreamInfo->stTuningParam), sizeof(DxTuneParam_t) );
	if (pStreamInfo->eAllocMode == DxRsvStream_AllocMode_FORCE)
	{
		HxLOG_Error("Input mode is Force but failed???? \n");
	}
	stSetup.eAllocMode = (bForce==TRUE)?DxRsvStream_AllocMode_FORCE: DxRsvStream_AllocMode_BEG;
	stSetup.usPriority = pStreamInfo->usPriority;
	stSetup.stStartArg.bTranscode = bTranscode;

	if (eDxDELIVERY_TYPE_SAT == stSetup.stTuneParam.eDeliType)
	{
		HERROR hErr;
		DbSvc_AntInfo_t stAntInfo;

		HxSTD_MemSet (&stAntInfo, 0, sizeof(DbSvc_AntInfo_t));
		hErr =	DB_SVC_GetAntInfo (stSetup.stTuneParam.sat.tuningInfo.antuid, &stAntInfo);
		if (ERR_OK == hErr)
			OTL_CONV_DbAntInfo2DxAntInfo (&stSetup.stTuneParam.sat.antInfo, &stAntInfo);
	}

	hErr = MGR_SATIP_Try(&stSetup);
	if (hErr != ERR_OK)
	{
		HxLOG_Debug("Satip try failed. \n");
		goto END_FUNC;
	}

	hErr = ERR_OK;
END_FUNC:
	return hErr;
}
#endif

HERROR xmgr_schedule_makeScheduleFailReason_Base (void *pvContext, DxSchedule_t *pstSchedule)
{
#if defined(CONFIG_MW_MM_PVR)
	XmgrSch_BaseContext_t	*pstContext = (XmgrSch_BaseContext_t *)pvContext;
	MgrRec_RecSetup_t		 stRecSetup;
	HERROR					 hErr;
	Handle_t				 hSvcHandle;
	MgrRec_EndReasonType_e	eEndReason = eMgrRecFailed_NOFAIL;

	if ((NULL == pstContext) || (NULL == pstSchedule))					{ return ERR_FAIL; }


	HxLOG_Debug("slot: %d, eStatus: %d,  eRemovedReason: %d\n", pstSchedule->ulSlot, pstSchedule->eStatus, pstSchedule->stRemovedInfo.eRemovedReason);

	if(pstSchedule->eStatus == DxRSVSTATUS_RUNNING)
	{
		HxLOG_Debug("do not consider running schedule here.\n");
		return ERR_FAIL;
	}

	// 여기서는 power off나 no hdd인 경우만 만들어 주면 된다. 다른 경우는 이미 pvr 에서 만들었을 테니까.
	switch(pstSchedule->stRemovedInfo.eRemovedReason)
	{
		case DxSchedule_RemovedReason_Expired:
			eEndReason = eMgrRecFailed_POWER_OFF;
			break;

		case DxSchedule_RemovedReason_NoPvrDevice:
			eEndReason = eMgrRecFailed_NOHDD;
			break;

		default:
			HxLOG_Debug("do not process eRemovedReason: %d \n", pstSchedule->stRemovedInfo.eRemovedReason);
			return ERR_OK;

	}

	HxSTD_MemSet(&stRecSetup, 0, sizeof(MgrRec_RecSetup_t));

	switch (pstSchedule->eRsvType)
	{

	case DxRSVTYPE_RECORD_EBR:
		stRecSetup.eRecType = eMgrRecType_EBR;
		break;

	case DxRSVTYPE_RECORD_SERIES_EBR:
		stRecSetup.eRecType = eMgrRecType_SBR;
		break;

	case DxRSVTYPE_RECORD_TBR:
	case DxRSVTYPE_RECORD_SATRECORD_TBR:
		stRecSetup.eRecType = eMgrRecType_TBR;
		break;

	default:
		HxLOG_Debug("Do not process eRsvType(%d)\n", pstSchedule->eRsvType);
		return ERR_OK;
	}

 	hErr = DB_SVC_GetServiceHandleBySvcIdx((HUINT16)(pstSchedule->ulSvcUid & 0xFFFF), &hSvcHandle);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("Cannot find SvcHandle: ulSvcUid(%d)\n", pstSchedule->ulSvcUid);
		return ERR_FAIL;
	}

 	stRecSetup.eRecDst				= eMgrRecOutput_HDD;
	stRecSetup.hMasterSvc			= hSvcHandle;
	stRecSetup.hCurrSvc				= hSvcHandle;
	stRecSetup.ulSlot				= pstSchedule->ulSlot;
	stRecSetup.ulSourceContentId	= 0xFFFFFFFF;

	hErr = MGR_RECORDER_CreateRecordFailReason(&stRecSetup, eEndReason, pstSchedule);
#else
	HERROR					 hErr=ERR_OK;
#endif
	return hErr;
}


STATIC BUS_Result_t xproc_schedule_Base (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	XmgrSch_BaseContext_t	*pstContext = xmgr_schedule_GetContext_Base();
	BUS_Result_t			 eRet = MESSAGE_PASS;
	HERROR				hErr = ERR_FAIL;
	HUINT32				ulOffsetTime = 0;

	switch (message)
	{
	case eMEVT_BUS_CREATE:
		HxLOG_Info("eMEVT_BUS_CREATE \n");
		BUS_MGR_SetMgrName(BUS_MGR_GetThis(), (HINT8*) __FUNCTION__);
		HxSTD_MemSet(pstContext, 0, sizeof(XmgrSch_BaseContext_t));
		eRet = MESSAGE_BREAK;
		break;

	case eMEVT_SCHEDULE_TIME_UP:
		HxLOG_Info("eMEVT_SCHEDULE_TIME_UP \n");
		eRet = xmgr_schedule_MsgTimeUp_Base((void *)pstContext, hAction, p1, p2, p3);
		break;

	case eMEVT_SCHEDULE_CANCELED:
		HxLOG_Info("eMEVT_SCHEDULE_CANCELED \n");
		(void)xmgr_schedule_makeScheduleFailReason_Base((void *)pstContext, (DxSchedule_t *)p3);
		eRet = MGR_SCHEDULE_SetScheduleInCancelDB(p2, (DxSchedule_t *)p3);
		eRet |= xmgr_schedule_MsgTimeUp_Base((void *)pstContext, hAction, p1, eSAPI_TIMER_TYPE_Stop, p3);
		break;

#if defined(CONFIG_MW_MM_PVR)
	case eMEVT_PVR_RECORD_CHANGE_TSR_REC_TO_DELAYED_RECORD_RESULT:
		HxLOG_Info("eMEVT_PVR_RECORD_CHANGE_TSR_REC_TO_DELAYED_RECORD_RESULT\n");
		eRet = xmgr_schedule_MsgDelayedRecordSetupResult_Base((void *)pstContext, hAction, p1, p2, p3);
		break;

	case eMEVT_STORAGE_NOTIFY_PVR_ENABLED:
		HxLOG_Info("eMEVT_STORAGE_NOTIFY_PVR_ENABLED\n");
		eRet=xmgr_schedule_MsgRecordSetupMaxInstance_Base((void *)pstContext, hAction, p1, p2, p3);
		break;

	case eMEVT_PVR_NOTIFY_RECORD_SETUP_FAILED:
		HxLOG_Info("eMEVT_PVR_NOTIFY_RECORD_SETUP_FAILED \n");
		eRet = xmgr_schedule_MsgRecordSetupFail_Base((void *)pstContext, hAction, p1, p2, p3);
		break;

	case eMEVT_PVR_PLAYBACK_STOP:
		HxLOG_Info("eMEVT_PVR_PLAYBACK_STOP \n");
		eRet = xmgr_schedule_MsgPvrPlaybackStop_Base((void *)pstContext, hAction, p1, p2, p3);
		break;
#endif

#if defined(CONFIG_NON_TRANSCODING_STREAMING)
	case eMEVT_SCHEDULE_NTLS_CHANGEPID:
		HxLOG_Info("eMEVT_SCHEDULE_NTLS_CHANGEPID \n");
		eRet = xmgr_schedule_MsgNtlsChangePid_Base((void *)pstContext, hAction, p1, p2, p3);
		break;

	case eMEVT_NTLS_NOTIFY_START_STOPPED:
		HxLOG_Info("eMEVT_NTLS_NOTIFY_START_STOPPED \n");
		eRet = xmgr_schedule_MsgNtlsStopped_Base((void *)pstContext, hAction, p1, p2, p3);
		break;

	case eMEVT_NTLS_NOTIFY_START_FAILED:
		HxLOG_Info("eMEVT_NTLS_NOTIFY_START_FAILED \n");
		eRet = xmgr_schedule_MsgNtlsStartFailed_Base((void *)pstContext, hAction, p1, p2, p3);
		break;
#endif

#if defined(CONFIG_STREAMING_SATIP)
	case eMEVT_SCHEDULE_SATIP_CHANGEPID:
		HxLOG_Info("eMEVT_SCHEDULE_SATIP_CHANGEPID \n");
		eRet = xmgr_schedule_MsgSatIpChangePid_Base((void *)pstContext, hAction, p1, p2, p3);
		break;

	case eMEVT_SATIP_NOTIFY_START_FAILED:
		HxLOG_Info("eMEVT_SATIP_NOTIFY_START_FAILED \n");
		eRet = xmgr_schedule_MsgSatipStartFailed_Base((void *)pstContext, hAction, p1, p2, p3);
		break;
#endif

#if defined(CONFIG_TRANSCODING_STREAMING)
	case eMEVT_SCHEDULE_HLS_CHANGE_PLAY_POSITION:
		HxLOG_Info("eMEVT_SCHEDULE_HLS_CHANGE_PLAY_POSITION \n");
		eRet = xmgr_schedule_MsgHlsChangePlayPosition((void *)pstContext, hAction, p1, p2, p3);
		break;
#endif

	case eSEVT_DB_PARAM_UPDATED:
		HxLOG_Info("eSEVT_DB_PARAM_UPDATED \n");
		switch(p1)
		{
			case eDB_PARAM_ITEM_ANT_CONN_TYPE:
				HxLOG_Info("eDB_PARAM_ITEM_ANT_CONN_TYPE\n");
				MGR_SCHEDULE_UpdateAntennaConnectionInfo();
				break;

			case eDB_PARAM_ITEM_PADDINGTIME_START:
				HxLOG_Info("eDB_PARAM_ITEM_PADDINGTIME_START\n");
				hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_PADDINGTIME_START, (HUINT32 *)&ulOffsetTime, 0);

				if(hErr == ERR_FAIL)
				{
					HxLOG_Error("get padding time error \n");
					return hErr;
				}

				xmgr_schedule_ChangePaddingTime_Base(ulOffsetTime, TRUE);
				break;

			case eDB_PARAM_ITEM_PADDINGTIME_STOP:
				HxLOG_Info("eDB_PARAM_ITEM_PADDINGTIME_STOP\n");
				hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_PADDINGTIME_STOP, (HUINT32 *)&ulOffsetTime, 0);

				if(hErr == ERR_FAIL)
				{
					HxLOG_Error("get padding time error \n");
					return hErr;
				}

				xmgr_schedule_ChangePaddingTime_Base(ulOffsetTime, FALSE);
				break;

			default:
				break;

		}
		eRet = MESSAGE_PASS;
		break;

	case eMEVT_SCHEDULE_EVENT_OCCURED:
		HxLOG_Info("eMEVT_SCHEDULE_EVENT_OCCURED\n");
		(void)MGR_SCHEDULE_CheckFrontPannelDisplay();
		break;

	case eMEVT_BUS_GO_OPERATION:
		HxLOG_Info("eMEVT_BUS_GO_OPERATION\n");
		(void)xmgr_schedule_CancelStandbySchedule_Base();
		break;

	default:
		eRet = MESSAGE_PASS;
		break;
	}

	return (MESSAGE_BREAK == eRet) ? MESSAGE_BREAK : MESSAGE_PASS;
}

STATIC HERROR			xmgr_schedule_GetBatchEpgCBListTvtv_Base(void)
{
	HERROR			hError = ERR_FAIL;
	HBOOL			bTvtvEnable = FALSE;
	MgrAction_BatchProcCBList_t fnBatchProcCBList;

	HxLOG_Trace();

	fnBatchProcCBList.fnAppInit = NULL;
	fnBatchProcCBList.fnAppDeinit = NULL;
	fnBatchProcCBList.fnAppReSchedule = NULL;


	hError = DB_PARAM_GetItem(eDB_PARAM_ITEM_TVTV_ENABLE, (HUINT32 *)&bTvtvEnable, 0);
	if(hError != ERR_OK )
	{
		HxLOG_Error("eDB_PARAM_ITEM_TVTV_ENABLE fail\n");
		return ERR_FAIL;
	}

	if(bTvtvEnable == FALSE)
	{
		HxLOG_Info("bTvtvEnable == FALSE \n");
		return ERR_FAIL;
	}


	fnBatchProcCBList.fnAppInit = MGR_EPG_InitBatch;
	fnBatchProcCBList.fnAppDeinit = MGR_EPG_DeinitBatch;
	fnBatchProcCBList.fnAppReSchedule = MGR_EPG_RescheduleBatch;

	HxLOG_Print("CBLIST Init[%x] Deinit[%x] Reschedule[%x]\n", fnBatchProcCBList.fnAppInit, fnBatchProcCBList.fnAppDeinit, fnBatchProcCBList.fnAppReSchedule);

	MGR_ACTION_AddBatchProc (eDxBAT_TYPE_STANDBY, eDxBAT_PROC_EPGTVTV, fnBatchProcCBList, 0, 0, TRUE);

	return ERR_OK;
}

#if defined(CONFIG_APCORE_EMM_UPDATE)
STATIC HERROR xmgr_schedule_BatchAddEMMProOnStandby_base(void)
{
	MgrAction_BatchProcCBList_t fnAppCBList;
	UNIXTIME 					ulStartTime = 0; // 0 으로. 당장 시작.
	HUINT32 					ulDuration = 0;
	HERROR					hErr = ERR_FAIL;

	HxLOG_Trace();

	hErr = MGR_EMMUpdate_GetConfiguration(NULL, NULL, &ulDuration);
	if(hErr == ERR_FAIL)
	{
		HxLOG_Error("MGR_EMMUpdate_GetConfiguration() fail \n");
		return ERR_FAIL;
	}

	// TODO: s_stBatchInfo 와 같은 형식으로 만들어서 mgr_emmupdate.c 에 MGR_EMMUpdate_GetBatchCBListForSchedule(), MGR_EMMUpdate_GetBatchTimeForSchedule() 이런 형식으로 만들자.

	fnAppCBList.fnAppInit = MGR_EMMUpdate_InitBatch;
	fnAppCBList.fnAppDeinit = MGR_EMMUpdate_DeinitBatch;
	fnAppCBList.fnAppReSchedule = MGR_EMMUpdate_RescheduleBatch;


	HxLOG_Print("CBLIST Init[%x] Deinit[%x] Reschedule[%x]\n", fnAppCBList.fnAppInit, fnAppCBList.fnAppDeinit, fnAppCBList.fnAppReSchedule);


	MGR_ACTION_AddBatchProc (eDxBAT_TYPE_STANDBY, eDxBAT_PROC_EMMUPDATE, fnAppCBList, ulStartTime, ulDuration, TRUE);


	return ERR_OK;
}
#endif

#if defined(CONFIG_MW_SATRECORD)
STATIC HERROR xmgr_schedule_BatchAddSatRecProOnStandby_base(void)
{
	MgrAction_BatchProcCBList_t fnAppCBList;
	UNIXTIME 					ulStartTime = 0; // 0 으로. 당장 시작.
	HUINT32 					ulDuration = 2*60;
	HERROR					hErr = ERR_FAIL;

	HxLOG_Trace();

	if(hErr == ERR_FAIL)
	{
		HxLOG_Error("MGR_EMMUpdate_GetConfiguration() fail \n");
		return ERR_FAIL;
	}

	// TODO: s_stBatchInfo 와 같은 형식으로 만들어서 mgr_emmupdate.c 에 MGR_EMMUpdate_GetBatchCBListForSchedule(), MGR_EMMUpdate_GetBatchTimeForSchedule() 이런 형식으로 만들자.

	fnAppCBList.fnAppInit = MGR_SATRECORD_InitBatch;
	fnAppCBList.fnAppDeinit = MGR_SATRECORD_DeinitBatch;
	fnAppCBList.fnAppReSchedule = MGR_SATRECORD_RescheduleBatch;


	HxLOG_Print("CBLIST Init[%x] Deinit[%x] Reschedule[%x]\n", fnAppCBList.fnAppInit, fnAppCBList.fnAppDeinit, fnAppCBList.fnAppReSchedule);


	MGR_ACTION_AddBatchProc (eDxBAT_TYPE_STANDBY, eDxBAT_PROC_SATRECUPDATE, fnAppCBList, ulStartTime, ulDuration, TRUE);


	return ERR_OK;
}
#endif

STATIC HBOOL xmgr_schedule_cbReadyCompare_Base (void *userdata, void *data)
{
	DxSchedule_t			*pstSchedule = (DxSchedule_t *)userdata;
	XmgrSch_ReadyRec_t		*pstReady = (XmgrSch_ReadyRec_t *)data;

	if (pstSchedule != NULL && pstReady != NULL)
	{
		if (pstSchedule->ulSlot == pstReady->ulSlot)
			return TRUE;
	}

	return FALSE;

}

STATIC HERROR	xmgr_schedule_AddReadySchedule_Base (void *pvContext, DxSchedule_t *pstSchedule)
{
	XmgrSch_BaseContext_t		*pstContext = (XmgrSch_BaseContext_t *)pvContext;
	HxList_t					*pListItem;
	XmgrSch_ReadyRec_t 			*pstReadyRec = NULL;

	if (pvContext == NULL || pstSchedule == NULL)
	{
		HxLOG_Error("null param. \n");
		return ERR_FAIL;
	}

	pListItem = HLIB_LIST_FindEx(pstContext->pstReadyRecList, (const void*)pstSchedule, xmgr_schedule_cbReadyCompare_Base);
	if (pListItem != NULL)
	{
		HxLOG_Debug("already in the list. slot=(%d) \n", pstSchedule->ulSlot);
		return ERR_OK;
	}

	pstReadyRec = (XmgrSch_ReadyRec_t *)OxMGR_Calloc (sizeof(XmgrSch_ReadyRec_t));
	if (NULL == pstReadyRec)
	{
		HxLOG_Error("Memory Allocation failed:\n");
		return ERR_FAIL;
	}

	pstReadyRec->ulSlot		= pstSchedule->ulSlot;

	pstContext->pstReadyRecList = HLIB_LIST_Append (pstContext->pstReadyRecList, (void *)pstReadyRec);

	return ERR_OK;
}

STATIC HERROR	xmgr_schedule_RemoveReadySchedule_Base (void *pvContext, DxSchedule_t *pstSchedule)
{
	XmgrSch_BaseContext_t		*pstContext = (XmgrSch_BaseContext_t *)pvContext;

	pstContext->pstReadyRecList = HLIB_LIST_RemoveIf(pstContext->pstReadyRecList
														, (void*)pstSchedule, xmgr_schedule_cbReadyCompare_Base, OxMGR_Free_CB);

	return ERR_OK;
}


STATIC HUINT64	xmgr_schedule_CalcRecDiskSpace_Base(HUINT32 ulDuration, HBOOL bIsHD)
{
	HUINT64	ullRecSpace = 0;

	if(bIsHD)
	{
		/* HD : 1시간에 4G 기준. 초당 KByte */
		ullRecSpace = (STORAGE_SIZE_FOR_HD_PER_HOUR * 1024 * 1024) / (SECONDS_PER_HOUR);
	}
	else
	{
		/* SD : 1시간에 1.5G 기준. 초당  KByte */
		ullRecSpace = (STORAGE_SIZE_FOR_SD_PER_HOUR * 1024 * 1024) / (SECONDS_PER_HOUR);
	}
	ullRecSpace *= ulDuration;

	return ullRecSpace;
}

STATIC HBOOL xmgr_schedule_IsAutoDeleteNeeded_Base (DxSchedule_t *pstSchedule, SAPI_TimerItemType_e eTimer)
{
	if (eTimer == eSAPI_TIMER_TYPE_ReadyTracking)
	{
		return TRUE;
	}
	else if (eTimer == eSAPI_TIMER_TYPE_Ready)
	{
		if (pstSchedule->eRsvType == DxRSVTYPE_RECORD_TBR
			|| pstSchedule->eRsvType == DxRSVTYPE_RECORD_EBR
			|| pstSchedule->eRsvType == DxRSVTYPE_RECORD_SERIES_EBR)
		{
			return TRUE;
		}

	}
	return FALSE;
}

STATIC HERROR		xmgr_schedule_RequestAutoDelete_Base (void *pvContext, DxSchedule_t *pstSchedule)
{
	XmgrSch_BaseContext_t	*pstBaseContext = (XmgrSch_BaseContext_t *)pvContext;
	HERROR					hErr;
	HUINT32					i;
	HUINT32					ulRecCount;
	HxList_t				*pstListItemRec;
	XmgrSch_ReadyRec_t		*pstReadyRec;
	XmgrSch_RunningRec_t	*pstRunningRec;
	DxSchedule_t			*pstDummySchedule;
	UNIXTIME				utCurrTime;
	UNIXTIME				utStartTime;
	UNIXTIME				utEndTime;

	Handle_t		hSvc = HANDLE_NULL;
	DbSvc_Info_t	stSvcInfo;
	HBOOL			bIsHD = TRUE;

	HUINT64	ullRequestSizeKb = 0;
	HUINT32	ulRequestSizeKbUpper;
	HUINT32	ulRequestSizeKbLower;

	if (  pstSchedule->eRsvType != DxRSVTYPE_RECORD_EBR
		&& pstSchedule->eRsvType != DxRSVTYPE_RECORD_TBR
		&& pstSchedule->eRsvType != DxRSVTYPE_RECORD_SERIES_EBR )
	{
		HxLOG_Debug("Not supported rsv type. type=(%d)[%s] \n",pstSchedule->eRsvType,DLIB_ConvertEnum2Str(DxNAMEOF(DxRsvType_e), pstSchedule->eRsvType));
		return ERR_FAIL;
	}
	pstDummySchedule = (DxSchedule_t *)OxMGR_Malloc(sizeof(DxSchedule_t));
	if (pstDummySchedule == NULL)
	{
		return ERR_FAIL;
	}

	utCurrTime = (UNIXTIME)HLIB_STD_GetSystemTime();

	ullRequestSizeKb = 0;

	// 1. tracking ready ~ ready ~ start 사이의 schedule이 차지할 공간을 고려한다.
	ulRecCount = HLIB_LIST_Length(pstBaseContext->pstReadyRecList);
	for (i=0;i < ulRecCount; i++)
	{
		pstListItemRec = HLIB_LIST_GetListItem(HLIB_LIST_First(pstBaseContext->pstReadyRecList), i);
		pstReadyRec = HLIB_LIST_Data(pstListItemRec);
		if (pstReadyRec == NULL)
		{
			HxLOG_Error("Fail to find running rec. index=(%d)\n",i);
			continue;
		}

		hErr = SAPI_GetSchedule(pstReadyRec->ulSlot, pstDummySchedule);
		if (hErr != ERR_OK)
		{
			HxLOG_Error("SAPI_GetSchedule failed. slot=(%d) \n", pstReadyRec->ulSlot);
			continue;
		}

		bIsHD = TRUE;
		hErr = DB_SVC_GetServiceHandleBySvcIdx((HUINT16)(pstDummySchedule->ulSvcUid& 0xFFFF), &hSvc);
		if (ERR_OK == hErr)
		{
			hErr = DB_SVC_GetServiceInfo(hSvc, &stSvcInfo);
			if (ERR_OK == hErr)
			{
				if(DbSvc_GetSvcType(&stSvcInfo) != eDxSVC_TYPE_TV || DbSvc_GetVideoType(&stSvcInfo) != eDxVIDEO_TYPE_HD)
				{
					// 최악의 경우를 위해 default를 TRUE로 넣고, HD가 아니라는 것을 확정할 때 FALSE로 넣는다.
					bIsHD = FALSE;
				}

			}
		}

		ullRequestSizeKb += xmgr_schedule_CalcRecDiskSpace_Base(pstDummySchedule->ulDuration, bIsHD);
		HxLOG_Debug("(%d/%d) ready sch requested Size=(%lld), duration = (%d) \n",i, ulRecCount, ullRequestSizeKb, pstDummySchedule->ulDuration);
	}

	// 2. 현재 running rec의 남아 있는 duration이 차지할 공간을 고려한다.
	ulRecCount = HLIB_LIST_Length(pstBaseContext->pstRunRecList);
	for (i=0;i < ulRecCount; i++)
	{
		pstListItemRec = HLIB_LIST_GetListItem(HLIB_LIST_First(pstBaseContext->pstRunRecList), i);
		pstRunningRec = HLIB_LIST_Data(pstListItemRec);
		if (pstRunningRec == NULL)
		{
			HxLOG_Error("Fail to find running rec. index=(%d)\n",i);
			continue;
		}

		hErr = SAPI_GetSchedule(pstRunningRec->ulSlot, pstDummySchedule);
		if (hErr != ERR_OK)
		{
			HxLOG_Error("SAPI_GetSchedule failed. slot=(%d) \n", pstRunningRec->ulSlot);
			continue;
		}

		bIsHD = TRUE;
		hErr = DB_SVC_GetServiceHandleBySvcIdx((HUINT16)(pstDummySchedule->ulSvcUid& 0xFFFF), &hSvc);
		if (ERR_OK == hErr)
		{
			hErr = DB_SVC_GetServiceInfo(hSvc, &stSvcInfo);
			if (ERR_OK == hErr)
			{
				if(DbSvc_GetSvcType(&stSvcInfo) != eDxSVC_TYPE_TV || DbSvc_GetVideoType(&stSvcInfo) != eDxVIDEO_TYPE_HD)
				{
					// 최악의 경우를 위해 default를 TRUE로 넣고, HD가 아니라는 것을 확정할 때 FALSE로 넣는다.
					bIsHD = FALSE;
				}

			}
		}

		HLIB_DATETIME_ConvertDateTimeToUnixTime(&(pstDummySchedule->stStartTime), &utStartTime);
		utStartTime -= pstDummySchedule->uExtInfo.stRec.ulPreOffsetTime;
		utEndTime	= utStartTime + pstDummySchedule->ulDuration + pstDummySchedule->uExtInfo.stRec.ulPostOffsetTime;
		utStartTime = (utStartTime < utCurrTime)? utCurrTime : utStartTime; // MAX

		if ( utEndTime > utStartTime)
		{
			ullRequestSizeKb += xmgr_schedule_CalcRecDiskSpace_Base(utEndTime - utStartTime, bIsHD);
		}
		else
		{
			HxLOG_Error("start time is past end time. start time=(%d) end time=(%d) \n",utStartTime,utEndTime);
		}
		HxLOG_Debug("(%d/%d) running sch requested Size=(%lld), duration = (%d) \n",i, ulRecCount, ullRequestSizeKb, utEndTime - utStartTime);
	}

	ulRequestSizeKbUpper = (ullRequestSizeKb >> 32);
	ulRequestSizeKbLower = ullRequestSizeKb & 0xffffffff;

	HxLOG_Debug("req %lld \n",ullRequestSizeKb);
	HxLOG_Debug("upper %u \n",ulRequestSizeKbUpper);
	HxLOG_Debug("lower %u \n",ulRequestSizeKbLower);
	BUS_PostMessage(NULL, eOEVT_METARECLIST_REQUEST_AUTO_DELETE, HANDLE_NULL, (HINT32)ulRequestSizeKbUpper, (HINT32)ulRequestSizeKbLower, 0);

	OxMGR_Free(pstDummySchedule);

	return ERR_OK;

}

#if defined(CONFIG_STREAMING_SATIP)
STATIC DxSchedule_DetailInfo xmgr_schedule_GetSatIpFailInfo_Base (HUINT32 ulSlot)
{
	DxRSV_Streaming_t *pStreamInfo = NULL;
	HUINT32 ulForcePossible;
	HUINT32	ulNoTranscodePossible;
	HBOOL	bForce;
	HBOOL	bTranscode;
	DxSchedule_t	*pstSchedule = NULL;
	DxSchedule_DetailInfo	eDetailInfo = DxSchedule_DetailInfo_SatIp_NoTuner;
	HERROR					hErr;

	pstSchedule = (DxSchedule_t *)HLIB_STD_MemAlloc(sizeof(DxSchedule_t));
	if (pstSchedule == NULL)
	{
		HxLOG_Error("mem alloc failed. slot(%d) \n",ulSlot);
		goto END_FUNC;
	}

	hErr = SAPI_GetSchedule(ulSlot, pstSchedule);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("SAPI_GetSchedule failed. slot=(%d) \n",ulSlot);
		goto END_FUNC;
	}

	pStreamInfo = &pstSchedule->uExtInfo.stStrm;
	bForce = (pStreamInfo->eAllocMode == DxRsvStream_AllocMode_FORCE)? TRUE: FALSE;
#if defined(CONFIG_MW_MM_TRANSCODING)
	bTranscode = pStreamInfo->bTranscoding;
#else
	bTranscode = FALSE;
#endif

	// Case 1: Transcode	&& Force   	: 1. Try No Transcode  2. Error
	// Case 2: Transcode	&& No Force	: 1. Try No Transcode. 2. Try No Transcode with Force
	// Case 3: No Transcode && Force	: 1. Error
	// Case 4: No Transcode && No Force	: 1. Try Force
	HxLOG_Debug("Input force(%d) transcode(%d) priority(0x%x). \n",bForce, bTranscode,pStreamInfo->usPriority);
	if (bTranscode == TRUE)
	{
		// Case 1,2: Transcode
		HxLOG_Debug("Try No Transcode. slot(%d) \n",ulSlot);
		ulNoTranscodePossible = xmgr_schedule_TrySatIp(pstSchedule,bForce,FALSE);
		if (ulNoTranscodePossible == ERR_OK)
		{
			HxLOG_Debug("No Transcode Possible. slot(%d) \n",ulSlot);
			eDetailInfo = DxSchedule_DetailInfo_SatIp_NoTransCoder;
		}
		else
		{
			if (bForce == FALSE)
			{
				// Case 1: Transcode && Force -> Error
				HxLOG_Error("No Transcode impossible with force. slot=(%d) \n",ulSlot);
			}
			else
			{
				// Case 2: Transcode && No Force
				HxLOG_Debug("Try Force. slot(%d) \n",ulSlot);
				ulForcePossible = xmgr_schedule_TrySatIp(pstSchedule, TRUE, FALSE);
				if (ulForcePossible == ERR_OK)
				{
					HxLOG_Debug("Force possible. slot=(%d) \n",ulSlot);
					eDetailInfo = DxSchedule_DetailInfo_SatIp_ForcePossible;
				}
				else
				{
					HxLOG_Debug("No Transcoder & Force impossible. slot=(%d) \n",ulSlot);
				}
			}
		}
	}
	else
	{
		// Case 3,4: No Transcode
		if (bForce == TRUE)
		{
			// Case 3.: No Transcode && Force -> Error
			HxLOG_Error("No Transcode & Force failed. slot=(%d) \n",ulSlot);
			eDetailInfo = DxSchedule_DetailInfo_SatIp_NoTuner;
		}
		else
		{
			// Case 4: No Transcode && No Force
			HxLOG_Debug("Try Force. slot(%d) \n",ulSlot);
			ulForcePossible = xmgr_schedule_TrySatIp(pstSchedule, TRUE, FALSE);
			if (ulForcePossible == ERR_OK)
			{
				eDetailInfo = DxSchedule_DetailInfo_SatIp_ForcePossible;
			}
		}
	}

	if (eDetailInfo == DxSchedule_DetailInfo_SatIp_NoTuner)
	{
		HINT32			i,nNumOfTunerGroup;
		HBOOL	bIsIdle = FALSE;

		SVC_CH_GetNumberOfTunerGroup(&nNumOfTunerGroup);

		for (i=0; i<nNumOfTunerGroup; i++)
		{
			(void)SVC_CH_IsIdleTuner(i, &bIsIdle);
			if (bIsIdle == TRUE)
			{
				// idle tuner exists -> no LNB
				HxLOG_Debug("%d-th tuner idle \n",i);
				eDetailInfo = DxSchedule_DetailInfo_SatIp_NoLNB;
				break;
			}
		}
	}

	HxLOG_Debug("eDetailInfo(%d). slot(%d) \n",eDetailInfo, ulSlot);

END_FUNC:
	if (pstSchedule != NULL)
	{
		HLIB_STD_MemFree(pstSchedule);
	}

	return eDetailInfo;
}
#endif

void xmgr_schedule_reminder_base(void)
{
	MgrAction_State_e action_state;
	HSystemBootupStatus_t stBootupReason;

	action_state = MGR_ACTION_GetState();

	// active standby 중 schedule 로 인해 깨어난경우, bootup reason 을
	// eHBootUpReason_POWER 에서 eHBootUpReason_TIMER 으로 봐꾸어준다.
	if(action_state == eAmState_ACTIVE_STANDBY)
	{
		stBootupReason.eBootUpReason		= eHBootUpReason_TIMER;
		stBootupReason.eSystemCurrentMode   = eHSystemOperationMode_NORMAL;
		HAPI_BroadcastSignal("signal:onBootupStateChanged", 1, "b", &stBootupReason, sizeof(HSystemBootupStatus_t));
	}
}

#define ___GLOBAL_FUNCTIONS___
HERROR xmgr_schedule_Init_Base (void)
{
	HERROR			 hErr;

	hErr = BUS_MGR_Create(NULL, APP_ACTION_PRIORITY, xproc_schedule_Base,
							HANDLE_NULL, 0, 0, 0);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("BUS_MGR_Create err:\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR xmgr_schedule_CheckFrontPannelDisplay_Base(void)
{
	HUINT32						 ulCnt, ulNumSch = 0;
	DxSchedule_t					*pstSchArray = NULL;
	HERROR						 hErr = ERR_OK;
	HBOOL						bTurnOnReserveIcon = FALSE;

	HxLOG_Trace();

	(void)SAPI_CountSchedule(&ulNumSch);
	if (0 == ulNumSch)
	{
		HxLOG_Debug("No Schedule: Nothing to stop\n");
		goto END_FUNC;
	}

	pstSchArray = (DxSchedule_t *)HLIB_STD_MemAlloc(sizeof(DxSchedule_t) * ulNumSch);
	if (NULL == pstSchArray)
	{
		HxLOG_Error("memory allocation failed\n");
		hErr = ERR_FAIL;
		goto END_FUNC;
	}

	hErr = SAPI_GetAllSchedules(ulNumSch, NULL, pstSchArray);


	if(hErr == ERR_OK)
	{
		for (ulCnt = 0; ulCnt < ulNumSch; ulCnt++)
		{
			DxSchedule_t		*pstSchedule = &(pstSchArray[ulCnt]);

			if(pstSchedule->eStatus != DxRSVSTATUS_RUNNING)
			{
				if(SAPI_GetSapiActionTypeBySchedule(pstSchedule) == eSAPI_ACTION_TYPE_LiveView
					|| SAPI_GetSapiActionTypeBySchedule(pstSchedule) == eSAPI_ACTION_TYPE_Record
					|| pstSchedule->eRsvType == DxRSVTYPE_POWER_ON
					||pstSchedule->eRsvType == DxRSVTYPE_POWER_OFF)
				{
					bTurnOnReserveIcon = TRUE;
					HxLOG_Debug("bTurnOnReserveIcon: %d \n", bTurnOnReserveIcon);
					break;
				}
			}
		}
	}


	hErr = ERR_OK;

END_FUNC:

	HxLOG_Info("bTurnOnReserveIcon: %d \n", bTurnOnReserveIcon);
	SVC_SYS_SetLedOnOff(eDxLED_ID_RESERVE, bTurnOnReserveIcon);

	if (pstSchArray)
	{
		HLIB_STD_MemFree(pstSchArray);
	}

	return hErr;
}

HERROR xmgr_schedule_RecordStopPadding_Base (void *pvContext, DxSchedule_t *pstSchedule)
{
	HxList_t				*pstListItem;
	XmgrSch_RunningRec_t    *pstRunRec;
	XmgrSch_BaseContext_t   *pstContext = (XmgrSch_BaseContext_t *)pvContext;

	if ((NULL == pstContext) || (NULL == pstSchedule))				      { return ERR_FAIL; }

	pstListItem = HLIB_LIST_FindEx(pstContext->pstRunRecList, (const void *)pstSchedule, xmgr_schedule_CmpScheduleSlot_Base);
	pstRunRec = (XmgrSch_RunningRec_t *)HLIB_LIST_Data (pstListItem);

	if (NULL == pstRunRec)
	{
		HxLOG_Error("No Running Record with slot (%d)\n", pstSchedule->ulSlot);
		return ERR_FAIL;
	}

	if (MGR_ACTION_GetType(pstRunRec->ulActionId) == eRecType_REC)
	{
		// major: record, minor ACTION_PRIORITY_LOWER_MASK -1
		MGR_ACTION_ChangePriority(pstRunRec->ulActionId, eAmPriority_RECORD - 1);
	}

	return ERR_OK;
}

XmgrSch_BaseContext_t *xmgr_schedule_GetContext_Base (void)
{
	return &stXmgrSchedBase_Context;
}

HERROR xmgr_schedule_StartLive_Base (void *pvContext, DxSchedule_t *pstSchedule)
{
	HUINT32					 ulMainActionId, ulActionId;
	Handle_t				 hSvcHandle;
	MgrLive_START_t			 stStartArg;
	XmgrSch_BaseContext_t	*pstContext = (XmgrSch_BaseContext_t *)pvContext;
	HERROR					 hErr;

	if ((NULL == pstContext) || (NULL == pstSchedule))
	{
		HxLOG_Error ("[%s:%d] no Context or no Schedule. error.\n");
		return ERR_FAIL;
	}

	HxSTD_MemSet(&stStartArg, 0, sizeof(MgrLive_START_t));

	hErr = DB_SVC_GetServiceHandleBySvcIdx((HUINT16)(pstSchedule->ulSvcUid & 0xFFFF), &hSvcHandle);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("Cannot find SvcHandle: ulSvcUid(%d)\n", pstSchedule->ulSvcUid);
		return ERR_FAIL;
	}

	ulMainActionId = MGR_ACTION_GetMainActionId();
	if (MGR_ACTION_GetType(ulMainActionId) == eViewType_WTV)
	{
		Handle_t		 hCurrSvc = HANDLE_NULL;

		hErr = MGR_LIVE_GetServiceHandle(ulMainActionId, eLiveViewPlaySvc_ACTUAL, &hCurrSvc);
		if ((ERR_OK == hErr) && (hSvcHandle == hCurrSvc))
		{
			HxLOG_Info("Already the same service is being playing... SVCHandle(0x%08x)\n", hCurrSvc);
			return ERR_OK;
		}
	}

	stStartArg.eLiveMode	= eMgrLiveMode_MainLive;
	stStartArg.hMasterSvc	= hSvcHandle;
	stStartArg.hService	= hSvcHandle;

	HxLOG_Debug ("\t**<SCH:MGR_LIVE_Start>**\n");
	HxLOG_Debug ("\t<eLiveMode\t:%d>\n", 	stStartArg.eLiveMode);
	HxLOG_Debug ("\t<hMasterSvc\t:%d>\n", 	stStartArg.hMasterSvc);
	HxLOG_Debug ("\t<hService\t:%d>\n", 	stStartArg.hService);

	hErr = MGR_LIVE_Start(&stStartArg, &ulActionId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("MGR_LIVE_Start Error: SvcHandle(0x%08x)\n", hSvcHandle);
		return ERR_FAIL;
	}

	// Operation/Standby 상관없이 PAPI로 Operation상태를 부르도록 한다.
	(void)PAPI_RequestOperation();

	HxLOG_Trace();
	return ERR_OK;
}

HERROR xmgr_schedule_StartMemRecord_Base (void *pvContext, DxSchedule_t *pstSchedule)
{
#if defined(CONFIG_STREAMING_SATIP) || defined(CONFIG_NON_TRANSCODING_STREAMING) || defined(CONFIG_TRANSCODING_STREAMING)
	HUINT32					 ulActionId=0;
	Handle_t				 hRingBuf = HANDLE_NULL;
	XmgrSch_BaseContext_t	*pstContext = (XmgrSch_BaseContext_t *)pvContext;
 	HERROR					 hErr = ERR_OK;
	XmgrSch_RunningRec_t 	*pstRunRec = NULL;
	SvcRingBuf_Setup_t		 stRingBufSetup;
	DxRSV_Streaming_t		*pStreamInfo = NULL;
	DxSchedule_RemovedInfo_t stRemovedInfo = {0, };

	if ((NULL == pstContext) || (NULL == pstSchedule))					{ return ERR_FAIL; }

	pStreamInfo = &(pstSchedule->uExtInfo.stStrm);
	if (NULL == pStreamInfo)
	{
		HxLOG_Error("Error.\n");
		return ERR_FAIL;
	}

	/* create ringbuf */
	{
		HxSTD_MemSet (&stRingBufSetup, 0, sizeof(SvcRingBuf_Setup_t));
		stRingBufSetup.eBufType 		= eRingBuf_Type_Linear;
		stRingBufSetup.ulMaxSize		= (4 * 1024 * 1128);		// cache용으로 4mb정도만 .. (추후 더 줄여도 된다.)
		stRingBufSetup.ulChunkSize		= 0;
		stRingBufSetup.bBlockOverflow	= FALSE;
		stRingBufSetup.eRuleAfterRead	= eRingBuf_Read_Remove;
		stRingBufSetup.cbSignal			= SVC_MediaServer_RINGBUF_Signal;

		hRingBuf = SVC_RINGBUF_Create (&stRingBufSetup);
		if (HANDLE_NULL == hRingBuf)
		{
			HxLOG_Error("Error. : Create RingBuf!!\r\n");
			return ERR_FAIL;
		}
	}

#if defined(CONFIG_STREAMING_SATIP)
	/* start service*/
	if (DxRSVSTREAM_TYPE_SATIP_MASK & pStreamInfo->eStreamType)
	{
		hErr = xmgr_schedule_StartSATIP_Base (pstSchedule->ulSlot, pStreamInfo, hRingBuf, &ulActionId);
		if (ERR_OK == hErr)
		{
			DxSchedule_t		 *pstSrcSchedule = (DxSchedule_t *)OxMGR_Calloc(sizeof(DxSchedule_t));

			// SAT>IP 작업이 일어나면 Tuner ID를 schedule에 update 해 준다.
			if (NULL != pstSrcSchedule)
			{
				HUINT32			 ulTunerId = 0;
				HERROR			 hRet;

				hRet = SAPI_GetSchedule(pstSchedule->ulSlot, pstSrcSchedule);
				if (ERR_OK == hRet)
				{
					hRet = SVC_PIPE_GetResourceId(ulActionId, eRxRSCTYPE_TUNER_GROUP, &ulTunerId);
					if (ERR_OK == hRet)
					{
						switch(pstSrcSchedule->uExtInfo.stStrm.stTuningParam.eDeliType)
						{
						case eDxDELIVERY_TYPE_SAT:
							pstSrcSchedule->uExtInfo.stStrm.stTuningParam.sat.ulTunerGroupId = ulTunerId;
							pstSrcSchedule->uExtInfo.stStrm.hAction = SVC_PIPE_GetActionHandle (ulActionId);
							break;

						default:
							break;
						}

						(void)SAPI_UpdateSchedule(pstSchedule->ulSlot, pstSrcSchedule, NULL, TRUE, TRUE);
					}
				}

				OxMGR_Free(pstSrcSchedule);
			}
		}
	}
#endif
#if defined(CONFIG_NON_TRANSCODING_STREAMING)
	if (DxRSVSTREAM_TYPE_NTLS_MASK & pStreamInfo->eStreamType)
	{
		switch (pStreamInfo->eStreamType)
		{
			case DxRSVSTREAM_TYPE_NTLS_LIVE_VIDEO:
			case DxRSVSTREAM_TYPE_NTLS_LIVE_AUDIO:
			{
				hErr = xmgr_schedule_StartLiveNTLS_Base (pStreamInfo, pstSchedule->ulSlot,pstSchedule->ulSvcUid, hRingBuf, &ulActionId);
			}
			break;

			case DxRSVSTREAM_TYPE_NTLS_FILE_PVR:
			case DxRSVSTREAM_TYPE_NTLS_FILE_PVR_AUDIO:
			{
				hErr = xmgr_schedule_StartPvrNTLS_Base (pStreamInfo, hRingBuf, &ulActionId);
			}
			break;

			default:
				hErr = ERR_FAIL;
				break;
		}
	}
#endif
#if defined(CONFIG_TRANSCODING_STREAMING)
	if (DxRSVSTREAM_TYPE_HLS_MASK & pStreamInfo->eStreamType)
	{
		switch (pStreamInfo->eStreamType)
		{
			case DxRSVSTREAM_TYPE_HLS_LIVE_VIDEO:
			case DxRSVSTREAM_TYPE_HLS_LIVE_AUDIO:
			{
				hErr = xmgr_schedule_StartLiveHLS_Base (pStreamInfo, pstSchedule->ulSvcUid, hRingBuf, &ulActionId);
			}
			break;

			case DxRSVSTREAM_TYPE_HLS_FILE_PVR:
			case DxRSVSTREAM_TYPE_HLS_FILE_PVR_AUDIO:
			{
				hErr = xmgr_schedule_StartPvrHLS_Base (pStreamInfo, hRingBuf, &ulActionId);
			}
			break;

			default:
				hErr = ERR_FAIL;
				break;
		}

	}
#endif

	if (ERR_OK == hErr)
	{
		hErr = SVC_MediaServer_Watchdog_Add(hRingBuf, pstSchedule);
		if (ERR_OK != hErr)
		{
			HxLOG_Error("SVC_MediaServer_Watchdog_Add fail!!\r\n");
			HxLOG_Error("SVC_MediaServer_Watchdog_Add fail!!\r\n");
		}

		/* register */
		{
			pstRunRec = (XmgrSch_RunningRec_t *)OxMGR_Calloc (sizeof(XmgrSch_RunningRec_t));
			if (NULL == pstRunRec)
			{
				HxLOG_Error("Memory Allocation failed:\n");
				return ERR_FAIL;
			}

			pstRunRec->ulSlot		= pstSchedule->ulSlot;
			pstRunRec->ulActionId	= ulActionId;
			pstRunRec->hRingBuf		= hRingBuf;

			pstContext->pstRunRecList = HLIB_LIST_Append (pstContext->pstRunRecList, (void *)pstRunRec);
		}
	}
	else
	{
		if (HANDLE_NULL != hRingBuf)
		{
			SVC_RINGBUF_Destroy (hRingBuf);
		}

#if defined(CONFIG_STREAMING_SATIP)
		if (DxRSVSTREAM_TYPE_SATIP_MASK & pStreamInfo->eStreamType)
		{
			stRemovedInfo.eRemovedReason = DxSchedule_RemovedReason_SatipFailed;
			stRemovedInfo.eDetailInfo = xmgr_schedule_GetSatIpFailInfo_Base(pstSchedule->ulSlot);
		}
		else
#endif
		{
			stRemovedInfo.eRemovedReason = DxSchedule_RemovedReason_Unknown;
		}
		SAPI_IncompleteSchedule(pstSchedule->ulSlot, &stRemovedInfo);

		HxLOG_Error("Error. : Start Action!!\r\n");
		return ERR_FAIL;
	}
#endif
	return ERR_OK;
}


HERROR xmgr_schedule_StopMemRecord_Base (void *pvContext, DxSchedule_t *pstSchedule)
{
#if defined(CONFIG_TRANSCODING_STREAMING) || defined(CONFIG_NON_TRANSCODING_STREAMING) || defined(CONFIG_STREAMING_SATIP)
	HxList_t				*pstListItem;
	XmgrSch_RunningRec_t	*pstRunRec;
	XmgrSch_BaseContext_t	*pstContext = (XmgrSch_BaseContext_t *)pvContext;
	DxRSV_Streaming_t		*pStreamInfo = NULL;

	if ((NULL == pstContext) || (NULL == pstSchedule))					{ return ERR_FAIL; }

	SVC_MediaServer_Watchdog_RemoveBySamaSlot(pstSchedule->ulSlot);

	pstListItem = HLIB_LIST_FindEx(pstContext->pstRunRecList, (const void *)pstSchedule, xmgr_schedule_CmpScheduleSlot_Base);
	pstRunRec = (XmgrSch_RunningRec_t *)HLIB_LIST_Data (pstListItem);

	if (NULL == pstRunRec)
	{
		HxLOG_Error("No Running MemRecord with slot (%d)\n", pstSchedule->ulSlot);
		return ERR_FAIL;
	}

	pstContext->pstRunRecList = HLIB_LIST_Remove(pstContext->pstRunRecList, (const void *)pstRunRec);

	pStreamInfo = &(pstSchedule->uExtInfo.stStrm);
	if (NULL == pStreamInfo)
	{
		HxLOG_Error("Error.\n");
		return ERR_FAIL;
	}

	if (DxRSVSTREAM_TYPE_SATIP_MASK & pStreamInfo->eStreamType)
	{
		MGR_SATIP_Stop(pstRunRec->ulActionId);
	}
	else if (DxRSVSTREAM_TYPE_NTLS_MASK & pStreamInfo->eStreamType)
	{
		MGR_NTLS_Stop(pstRunRec->ulActionId);
	}
	else if (DxRSVSTREAM_TYPE_HLS_MASK & pStreamInfo->eStreamType)
	{
		switch (pStreamInfo->eStreamType)
		{
			case DxRSVSTREAM_TYPE_HLS_LIVE_VIDEO:
			case DxRSVSTREAM_TYPE_HLS_LIVE_AUDIO:
			{
				MGR_LIVE_Stop(pstRunRec->ulActionId);
			}
			break;

			case DxRSVSTREAM_TYPE_HLS_FILE_PVR:
			case DxRSVSTREAM_TYPE_HLS_FILE_PVR_AUDIO:
			{
				MGR_PVR_StopPlaybackTranscodeToRingBuf (pstRunRec->ulActionId);
			}
			break;

			default:
				break;
		}
	}

	if(HANDLE_NULL != pstRunRec->hRingBuf && 0 != pstRunRec->hRingBuf)
	{
		SVC_RINGBUF_Destroy(pstRunRec->hRingBuf);
	}

	OxMGR_Free (pstRunRec);
#endif

	return ERR_OK;
}

#if defined(CONFIG_MW_MM_PVR)
HERROR xmgr_schedule_StartRecord_Base (void *pvContext, DxSchedule_t *pstSchedule)
{
	HUINT32					 ulActionId;
	Handle_t				 hSvcHandle = HANDLE_NULL;
	XmgrSch_BaseContext_t	*pstContext = (XmgrSch_BaseContext_t *)pvContext;
	XmgrSch_RunningRec_t	*pstRunRec= NULL;
	MgrRec_RecSetup_t		 stRecSetup;
	MgrRec_EndReasonType_e	eFailReason = eMgrRecFailed_UNKNOWN;
	HERROR					hErr = ERR_FAIL;


	if ((NULL == pstContext) || (NULL == pstSchedule))
	{
		HxLOG_Error("Context or pstSchedule is NULL.\n");
		goto END_FUNC;
	}

	HxSTD_MemSet(&stRecSetup, 0, sizeof(MgrRec_RecSetup_t));
	stRecSetup.eRecDst				= eMgrRecOutput_HDD;
	stRecSetup.hMasterSvc			= hSvcHandle;
	stRecSetup.hCurrSvc 			= hSvcHandle;
	stRecSetup.ulSlot				= pstSchedule->ulSlot;
	stRecSetup.ulSourceContentId	= 0xFFFFFFFF;

	if(pstSchedule->eRsvType != DxRSVTYPE_DESC_REC_STANDBY)
	{
		hErr = DB_SVC_GetServiceHandleBySvcIdx((HUINT16)(pstSchedule->ulSvcUid & 0xFFFF), &hSvcHandle);
		if (ERR_OK != hErr)
		{
			HxLOG_Error("Cannot find SvcHandle: ulSvcUid(%d)\n", pstSchedule->ulSvcUid);
			eFailReason = eMgrRecFailed_NONSERVICE;
			goto END_FUNC;
		}
	}

	switch (pstSchedule->eRsvType)
	{
#if !defined(CONFIG_MW_MM_FULLTS_REC)
	case DxRSVTYPE_RECORD_INSTANT:
		stRecSetup.eRecType = eMgrRecType_INSTANT;
		stRecSetup.ulSourceContentId	= 0xFFFFFFFF;
		break;

	case DxRSVTYPE_RECORD_DELAYED:
		{
			HUINT32 	ulTsrActId;

			hErr = MGR_PVR_GetTsrRecActionId (hSvcHandle, &ulTsrActId);
			if (ERR_OK != hErr)
			{
				HxLOG_Error("no TSR: Delayed Record impossible\n");
				eFailReason = eMgrRecFailed_MAXNUM;
				goto END_FUNC;
			}

			pstRunRec = (XmgrSch_RunningRec_t *)OxMGR_Calloc (sizeof(XmgrSch_RunningRec_t));
			if (NULL == pstRunRec)
			{
				HxLOG_Error("Memory Allocation failed\n");
				goto END_FUNC;
			}

			pstRunRec->ulActionId	= ulTsrActId;
			pstRunRec->ulSlot		= pstSchedule->ulSlot;
			pstContext->pstRunRecList = HLIB_LIST_Append (pstContext->pstRunRecList, (void *)pstRunRec);

			BUS_SendMessage(NULL, eMEVT_PVR_RECORD_CHANGE_TSR_REC_TO_DELAYED_RECORD, SVC_PIPE_GetActionHandle(ulTsrActId), (HINT32)pstSchedule->ulSlot, 0, 0);
		}
		return ERR_OK;
#else
	case DxRSVTYPE_RECORD_INSTANT:
		stRecSetup.eRecType = eMgrRecType_FULLTSREC;
		stRecSetup.ulSourceContentId	= 0xFFFFFFFF;
		break;

	case DxRSVTYPE_RECORD_DELAYED:
		stRecSetup.eRecType = eMgrRecType_FULLTSREC;
		stRecSetup.ulSourceContentId	= 0xFFFFFFFF;
		break;
#endif


	case DxRSVTYPE_RECORD_EBR:
		stRecSetup.eRecType = eMgrRecType_EBR;
		stRecSetup.ulSourceContentId	= 0xFFFFFFFF;
		break;

	case DxRSVTYPE_RECORD_SERIES_EBR:
		stRecSetup.eRecType = eMgrRecType_SBR;
		stRecSetup.ulSourceContentId	= 0xFFFFFFFF;
		break;

	case DxRSVTYPE_RECORD_TBR:
	case DxRSVTYPE_RECORD_SATRECORD_TBR:
		stRecSetup.eRecType = eMgrRecType_TBR;
		stRecSetup.ulSourceContentId	= 0xFFFFFFFF;
		break;

	case DxRSVTYPE_DESC_REC_STANDBY:
		{
			// 1시 - 5시 사이에 깨어난다는 가정...
			// 이 시간대에 다른 레코딩 스케쥴이 없다는 가정...
			DxRecListData_t *pstRecData = NULL;

			stRecSetup.eRecType 			= eMgrRecType_SCR2DESC;

			pstRecData = HLIB_STD_MemCalloc(sizeof(DxRecListData_t));
			if(pstRecData){
				if(MGR_SCHEDULE_StartDescRecordFinedItem(pstRecData) != ERR_OK)
				{
					HxLOG_Error("Can not found Scramble Item...\n");
					HLIB_MEM_Free(pstRecData);
					return ERR_FAIL;
				}

				if(MGR_PVR_StartPlayback (pstRecData->ulContentId, 0) != ERR_OK)
				{
					HxLOG_Error("MGR_PVR_StartPlayback Fail...\n");
					HLIB_MEM_Free(pstRecData);
					return ERR_FAIL;
				}
				stRecSetup.ulSourceContentId	= pstRecData->ulContentId;
				HLIB_MEM_Free(pstRecData);
			}
		}
		break;

	default:
		HxLOG_Error("Wrong RsvType: eRsvType(%d)\n", pstSchedule->eRsvType);
		return ERR_FAIL;
	}

	stRecSetup.eRecDst				= eMgrRecOutput_HDD;
	if(pstSchedule->eRsvType == DxRSVTYPE_DESC_REC_STANDBY)
	{
		stRecSetup.hMasterSvc			= HANDLE_NULL;
		stRecSetup.hCurrSvc 			= HANDLE_NULL;
		stRecSetup.ulSlot			= -1;
	}
	else
	{
		stRecSetup.hMasterSvc			= hSvcHandle;
		stRecSetup.hCurrSvc			= hSvcHandle;
		stRecSetup.ulSlot			= pstSchedule->ulSlot;
	}

	pstRunRec = (XmgrSch_RunningRec_t *)OxMGR_Calloc (sizeof(XmgrSch_RunningRec_t));
	if (NULL == pstRunRec)
	{
		HxLOG_Error("Memory Allocation failed:\n");
		goto END_FUNC;
	}

	eFailReason = MGR_PVR_StartRecording (&stRecSetup, &ulActionId);
	if (eMgrRecFailed_NOFAIL != eFailReason)
	{
		HxLOG_Error("MGR_PVR_StartRecording failed: reason(%d) \n",eFailReason);
		goto END_FUNC;
	}
	// start recording 이후의 실패로 인한 리소스 정리는 recorder에서 담당한다.

	pstRunRec->ulActionId	= ulActionId;
	pstRunRec->ulSlot		= pstSchedule->ulSlot;

	pstContext->pstRunRecList = HLIB_LIST_Append (pstContext->pstRunRecList, (void *)pstRunRec);
	return ERR_OK;

END_FUNC:
	if (pstRunRec != NULL)
	{
		OxMGR_Free(pstRunRec);
	}
	MGR_RECORDER_CreateRecordFailReason(&stRecSetup, eFailReason, pstSchedule);
	BUS_SendMessage (NULL, eMEVT_PVR_NOTIFY_RECORD_SETUP_FAILED, 0, eFailReason, stRecSetup.ulSlot, 0);
	return ERR_FAIL;
}

HERROR xmgr_schedule_StopRecord_Base (void *pvContext, DxSchedule_t *pstSchedule)
{
	HxList_t				*pstListItem;
	XmgrSch_RunningRec_t	*pstRunRec;
	XmgrSch_BaseContext_t	*pstContext = (XmgrSch_BaseContext_t *)pvContext;

	if ((NULL == pstContext) || (NULL == pstSchedule))					{ return ERR_FAIL; }

	pstListItem = HLIB_LIST_FindEx(pstContext->pstRunRecList, (const void *)pstSchedule, xmgr_schedule_CmpScheduleSlot_Base);
	pstRunRec = (XmgrSch_RunningRec_t *)HLIB_LIST_Data (pstListItem);

	if (NULL == pstRunRec)
	{
		HxLOG_Error("No Running Record with slot (%d)\n", pstSchedule->ulSlot);
		return ERR_FAIL;
	}

	pstContext->pstRunRecList = HLIB_LIST_Remove(pstContext->pstRunRecList, (const void *)pstRunRec);
	BUS_SendMessage(NULL, eMEVT_PVR_RECORD_STOP_REC, SVC_PIPE_GetActionHandle(pstRunRec->ulActionId), 0, 0, 0);
	OxMGR_Free (pstRunRec);

	return ERR_OK;
}
#endif

#define ___MESSAGE_FUNCTIONS___
#if defined(CONFIG_STREAMING_SATIP)
BUS_Result_t xmgr_schedule_MsgSatIpChangePid_Base (void *pvContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	DxSchedule_t						*pstSchedule = (DxSchedule_t *)p3;
	DxRSV_Streaming_t					*pStreamInfo = NULL;
	HxList_t							*pstListItem = NULL;
	XmgrSch_RunningRec_t				*pstRunRec	 = NULL;
	XmgrSch_BaseContext_t				*pstContext	 = (XmgrSch_BaseContext_t *)pvContext;
	MgrSatIp_TranscoderStreamProfile	stProfile;

	if ((NULL == pstContext) || (NULL == pstSchedule))
	{
		HxLOG_Error("Error. Null pointer!\n");
		return ERR_FAIL;
	}

	pStreamInfo = &(pstSchedule->uExtInfo.stStrm);
	if (NULL == pStreamInfo)
	{
		HxLOG_Error("Error. pStreamInfo is Null.\n");
		return ERR_FAIL;
	}

	pstListItem = HLIB_LIST_FindEx(pstContext->pstRunRecList, (const void *)pstSchedule, xmgr_schedule_CmpScheduleSlot_Base);
	pstRunRec = (XmgrSch_RunningRec_t *)HLIB_LIST_Data (pstListItem);

	if (NULL != pstRunRec)
	{
		if (DxRSVSTREAM_TYPE_SATIP_MASK & pStreamInfo->eStreamType)
		{
			if (DxRsvStream_Update_Pids & pStreamInfo->eUpdate)
			{
				HINT32			 nPIDsCount	= (DxRSVSTREAM_TYPE_SATIP_LIVE_ALLPID != pStreamInfo->eStreamType) ? pStreamInfo->nPIDsCount : -1;

				HxLOG_Print("[%s:%d] Update Pids\r\n", __FUNCTION__, __LINE__);

				MGR_SATIP_ChangePids(pstRunRec->ulActionId, nPIDsCount, pStreamInfo->usPIDs);

				SVC_MediaServer_Watchdog_UpdateMsgIdByRingbuf(pstRunRec->hRingBuf, pstSchedule);
			}

			if (DxRsvStream_Update_Priority & pStreamInfo->eUpdate)
			{
				HxLOG_Debug("Change Priority (%d) \n",pStreamInfo->usPriority);
				(void)MGR_SATIP_ChangePriority(pstRunRec->ulActionId, pStreamInfo->usPriority);
			}

			if (DxRsvStream_Update_Transcoding & pStreamInfo->eUpdate)
			{
				HxLOG_Print("[%s:%d] Update Transcoding Profile.\r\n", __FUNCTION__, __LINE__);

				HxSTD_MemCopy (&(stProfile), &(pStreamInfo->stProfile), sizeof(MgrSatIp_TranscoderStreamProfile));
				(void)MGR_SATIP_ChangeTranscodingProfile(pstRunRec->ulActionId, stProfile);
			}
		}
	}

	NOT_USED_PARAM (hAction);
	NOT_USED_PARAM (p1);
	NOT_USED_PARAM (p2);

	return MESSAGE_PASS;
}
#endif

#if defined(CONFIG_NON_TRANSCODING_STREAMING)
BUS_Result_t xmgr_schedule_MsgNtlsChangePid_Base (void *pvContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	DxSchedule_t		*pstSchedule = (DxSchedule_t *)p3;

	DxRSV_Streaming_t		*pStreamInfo = NULL;

	pStreamInfo = &(pstSchedule->uExtInfo.stStrm);
	if (NULL == pStreamInfo)
	{
		HxLOG_Error("Error.\n");
		return ERR_FAIL;
	}

	{
		HxList_t				*pstListItem	=	NULL;
		XmgrSch_RunningRec_t	*pstRunRec		=	NULL;
		XmgrSch_BaseContext_t	*pstContext = (XmgrSch_BaseContext_t *)pvContext;

		if ((NULL == pstContext) || (NULL == pstSchedule))					{ return ERR_FAIL; }

		pstListItem = HLIB_LIST_FindEx(pstContext->pstRunRecList, (const void *)pstSchedule, xmgr_schedule_CmpScheduleSlot_Base);
		pstRunRec = (XmgrSch_RunningRec_t *)HLIB_LIST_Data (pstListItem);

		if (NULL != pstRunRec)
		{
			if (DxRSVSTREAM_TYPE_NTLS_MASK & pStreamInfo->eStreamType)
			{
				if (DxRsvStream_Update_Pids & pStreamInfo->eUpdate)
				{
					HxLOG_Print("Update Pids\r\n");

					//SVC_MediaServer_Watchdog_RemoveByRingbuf(pstRunRec->hRingBuf);

					MGR_NTLS_ChangePids(pstRunRec->ulActionId, pStreamInfo->nPIDsCount, pStreamInfo->usPIDs);

					//SVC_RINGBUF_ClearBuffer(pstRunRec->hRingBuf);
					//SVC_MediaServer_Watchdog_Add(pstRunRec->hRingBuf, pstSchedule);

					SVC_MediaServer_Watchdog_UpdateMsgIdByRingbuf(pstRunRec->hRingBuf, pstSchedule);
				}
			}
		}
	}
	NOT_USED_PARAM (hAction);
	NOT_USED_PARAM (p1);
	NOT_USED_PARAM (p2);

	return MESSAGE_PASS;
}
#endif

#if defined(CONFIG_TRANSCODING_STREAMING)
BUS_Result_t xmgr_schedule_MsgHlsChangePlayPosition (void *pvContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	DxSchedule_t		*pstSchedule = (DxSchedule_t *)p3;

	DxRSV_Streaming_t		*pStreamInfo = NULL;

	pStreamInfo = &(pstSchedule->uExtInfo.stStrm);
	if (NULL == pStreamInfo)
	{
		HxLOG_Error("Error.\n");
		return ERR_FAIL;
	}

	{
		HxList_t				*pstListItem	=	NULL;
		XmgrSch_RunningRec_t	*pstRunRec		=	NULL;
		XmgrSch_BaseContext_t	*pstContext = (XmgrSch_BaseContext_t *)pvContext;

		if ((NULL == pstContext) || (NULL == pstSchedule))					{ return ERR_FAIL; }

		pstListItem = HLIB_LIST_FindEx(pstContext->pstRunRecList, (const void *)pstSchedule, xmgr_schedule_CmpScheduleSlot_Base);
		pstRunRec = (XmgrSch_RunningRec_t *)HLIB_LIST_Data (pstListItem);

		if (NULL != pstRunRec)
		{
			if ((DxRSVSTREAM_TYPE_HLS_FILE_PVR == pStreamInfo->eStreamType)
				|| (DxRSVSTREAM_TYPE_HLS_FILE_PVR_AUDIO == pStreamInfo->eStreamType))
			{
				if (DxRsvStream_Update_PlayPosition & pStreamInfo->eUpdate)
				{
					HxLOG_Print("ulActionId = %d, ulPlayerPosition = %d\r\n", pstRunRec->ulActionId, pstSchedule->uExtInfo.stStrm.ulPlayerPosition);

					MGR_PLAYBACK_SetJumpPlay(pstRunRec->ulActionId, pstSchedule->uExtInfo.stStrm.ulPlayerPosition);

					SVC_MediaServer_Watchdog_UpdateMsgIdByRingbuf(pstRunRec->hRingBuf, pstSchedule);
				}
			}
		}
	}
	NOT_USED_PARAM (hAction);
	NOT_USED_PARAM (p1);
	NOT_USED_PARAM (p2);

	return MESSAGE_PASS;
}
#endif

BUS_Result_t xmgr_schedule_MsgTimeUp_Base (void *pvContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32					 ulSlotId		= (HUINT32)p1;
	SAPI_TimerItemType_e	 eTimerType		= (SAPI_TimerItemType_e)p2;
	DxSchedule_t			*pstSchedule	= (DxSchedule_t *)p3;

	if (NULL == pstSchedule)
	{
		HxLOG_Error("pstSchedule NULL: SlotId(%d)\n", ulSlotId);
		return MESSAGE_PASS;
	}


	// 안전하게 ready로. 혹은...start 로 해도 무방 ??
	if (eTimerType == eSAPI_TIMER_TYPE_Ready && MGR_ACTION_GetBatchContext() != NULL)
	{

		if(SAPI_GetSapiActionTypeBySchedule(pstSchedule) == eSAPI_ACTION_TYPE_Record)
		{
			BUS_SendMessage(NULL, eMEVT_ACTION_SHUTDOWN_DESTROY, HANDLE_NULL, 0, 0, 0);
		}
	}

	HxLOG_Debug("pstSchedule->eRsvType: 0x%x \n", pstSchedule->eRsvType);
	switch (pstSchedule->eRsvType)
	{
#if defined(CONFIG_TRANSCODING_STREAMING) || defined(CONFIG_NON_TRANSCODING_STREAMING) || defined(CONFIG_STREAMING_SATIP)
	case DxRSVTYPE_NETSTREAMING_LIVE:
	case DxRSVTYPE_NETSTREAMING_FILE:
		switch (eTimerType)
		{
		case eSAPI_TIMER_TYPE_Start:
			(void)xmgr_schedule_StartMemRecord_Base(pvContext, pstSchedule);
			break;

		case eSAPI_TIMER_TYPE_Stop:
			(void)xmgr_schedule_StopMemRecord_Base(pvContext, pstSchedule);
			break;

		default:
			break;
		}

		break;
#endif

#if defined(CONFIG_MW_MM_PVR)
	case DxRSVTYPE_RECORD_EBR:
	case DxRSVTYPE_RECORD_TBR:
	case DxRSVTYPE_RECORD_INSTANT:
	case DxRSVTYPE_RECORD_DELAYED:
	case DxRSVTYPE_RECORD_SERIES_EBR:
	case DxRSVTYPE_RECORD_SATRECORD_TBR:
	case DxRSVTYPE_DESC_REC_STANDBY:
		switch (eTimerType)
		{
		case eSAPI_TIMER_TYPE_Ready:
			(void)xmgr_schedule_AddReadySchedule_Base(pvContext, pstSchedule);
			if (xmgr_schedule_IsAutoDeleteNeeded_Base(pstSchedule, eTimerType) == TRUE)
			{
				(void)xmgr_schedule_RequestAutoDelete_Base(pvContext, pstSchedule);
			}
			break;
		case eSAPI_TIMER_TYPE_Start:
			(void)xmgr_schedule_StartRecord_Base(pvContext, pstSchedule);
			(void)xmgr_schedule_RemoveReadySchedule_Base(pvContext, pstSchedule);
			break;

		case eSAPI_TIMER_TYPE_Stop:
			(void)xmgr_schedule_RemoveReadySchedule_Base(pvContext, pstSchedule);
			(void)xmgr_schedule_StopRecord_Base(pvContext, pstSchedule);
			break;

		case eSAPI_TIMER_TYPE_PaddingStopReady:
			(void)xmgr_schedule_RecordStopPadding_Base(pvContext, pstSchedule);
			break;

		default:
			break;
		}

		break;

	case DxRSVTYPE_WATCHING_EBR:
	case DxRSVTYPE_WATCHING_TBR:
		xmgr_schedule_reminder_base();
		break;

	case DxRSVTYPE_WATCHING_SERIES_EBR:
	case DxRSVTYPE_POWER_ON:
		/* 해당처리가.. 웹으로 이동... */
		break;
#endif

	case DxRSVTYPE_POWER_OFF:
		switch (eTimerType)
		{
		case eSAPI_TIMER_TYPE_Start:
			(void)PAPI_RequestShutDown();
			break;

		default:
			break;
		}
		break;

	case DxRSVTYPE_TVTVEPG_TIMER: // TODO: batch 로 넣는다.
		if (eAmState_ACTIVE_STANDBY == MGR_ACTION_GetState())
		{
			switch (eTimerType)
			{
			case eSAPI_TIMER_TYPE_Start:
				//MGR_EPG_Start(eEpgKind_TVTV, eEpgStartType_Normal, eEpgCallType_Shutdown);
				xmgr_schedule_GetBatchEpgCBListTvtv_Base();
				break;

			default:
				break;
			}
		}

		break;



#if defined(CONFIG_APCORE_EMM_UPDATE)
	case DxRSVTYPE_EMM:
		HxLOG_Debug("[DxRSVTYPE_EMM] eTimerType: %d\n", eTimerType);
		switch (eTimerType)
		{
		case eSAPI_TIMER_TYPE_Ready:
			HxLOG_Debug("[eSAPI_TIMER_TYPE_Ready] \n");
			{
#if 0
				MgrAction_State_e eActionState;

				eActionState = MGR_ACTION_GetState();
				HxLOG_Debug("eActionState: %d \n", eActionState);
				if (eAmState_ACTIVE_STANDBY == eActionState)
#endif
				{
					xmgr_schedule_BatchAddEMMProOnStandby_base();
				}
			}
			break;

		default:
			break;
		}

		break;
#endif

#if defined (CONFIG_BURST_OTA)
	case DxRSVTYPE_REGULAR_OTA:
		if(eTimerType == eSAPI_TIMER_TYPE_Ready || eTimerType == eSAPI_TIMER_TYPE_Start)
		{
			HxLOG_Debug("Burst Update will start soon!!!!! eTimerType: %d \n", eTimerType);
		}
		else if(eTimerType == eSAPI_TIMER_TYPE_Stop)
		{
			MgrAction_State_e eActionState;

			if(pstSchedule->stRemovedInfo.eRemovedReason != DxSchedule_RemovedReason_Completed)
			{
				HxLOG_Debug("Burst Update removed! do not executing or reschedule!");
				break;
			}

			eActionState = MGR_ACTION_GetState();
			if (eAmState_NORMAL == eActionState)
			{
				MGR_SCHEDULE_ReScheduleBurstUpdate();
			}
			else if (eAmState_ACTIVE_STANDBY == eActionState)
			{
				MGR_SCHEDULE_CheckConflictSchedule();
			}
		}
		break;
#endif
#if defined (CONFIG_MW_SATRECORD)
	case DxRSVTYPE_CHECK_SATRECORD_TIMER:
		if(eTimerType == eSAPI_TIMER_TYPE_Start)
		{
			MgrAction_State_e eActionState;

			eActionState = MGR_ACTION_GetState();
			if (eAmState_ACTIVE_STANDBY == eActionState)
			{
				xmgr_schedule_BatchAddSatRecProOnStandby_base();
			}
		}
		break;
#endif
	default:
		break;
	}

	NOT_USED_PARAM (p1);
	NOT_USED_PARAM (p2);
	NOT_USED_PARAM (p3);

	return MESSAGE_PASS;
}

BUS_Result_t xmgr_schedule_MsgDelayedRecordSetupResult_Base (void *pvContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32 						ulSlotId		= (HUINT32)p1;
	HERROR							hResultErr		= (HERROR)p2;
	MgrRec_EndReasonType_e			eRecFailReason	= (MgrRec_EndReasonType_e)p3;

	HxList_t						*pstListItem	=	NULL;
	XmgrSch_RunningRec_t			*pstRunRec		=	NULL;
	XmgrSch_BaseContext_t			*pstContext 	= (XmgrSch_BaseContext_t *)pvContext;
	DxSchedule_t					stSchedule;
	DxSchedule_RemovedReason_e	eRemoveReason = DxSchedule_RemovedReason_Unknown;
	DxSchedule_RemovedInfo_t stRemovedInfo = {0, };

	HxLOG_Trace();
	HxLOG_Debug("hResultErr: %d, ulSlotId: %d \n", hResultErr, ulSlotId);

	if (ERR_OK == hResultErr)
	{
		return MESSAGE_PASS;
	}

	if ((NULL != pstContext) && (NULL != pstContext->pstRunRecList))
	{
		stSchedule.ulSlot = ulSlotId;
		// 어차피 slot으로 찾는다.
		pstListItem = HLIB_LIST_FindEx(pstContext->pstRunRecList, &stSchedule, xmgr_schedule_CmpScheduleSlot_Base);

		if(pstListItem != NULL)
		{
			pstRunRec = (XmgrSch_RunningRec_t *)HLIB_LIST_Data (pstListItem);

			if (NULL != pstRunRec)
			{
				HxLOG_Debug("remove from pstRunRecList \n");
				pstContext->pstRunRecList = HLIB_LIST_Remove(pstContext->pstRunRecList, (const void *)pstRunRec);
			}
		}
	}

	HxLOG_Debug("eRecFailReason is (%d) \n");

	if(eRecFailReason == eMgrRecFailed_NOHDD || eRecFailReason == eMgrRecFailed_NOUSBHDD)
	{
		eRemoveReason = DxSchedule_RemovedReason_NoPvrDevice;
	}
	else if (eRecFailReason == eMgrRecFailed_HDDFULL || eRecFailReason == eMgrRecFailed_INCOMPLETE_HDDFULL)
	{
		eRemoveReason = DxSchedule_RemovedReason_HddFull;
	}
	else if (eRecFailReason == eMgrRecFailed_MAXRECORDLISTNUM )
	{
		eRemoveReason = DxSchedule_RemovedReason_MaxRecList;
	}
	stRemovedInfo.eRemovedReason = eRemoveReason;
	(void)SAPI_IncompleteSchedule(ulSlotId, &stRemovedInfo);


	NOT_USED_PARAM (hAction);
	NOT_USED_PARAM (p3);

	return MESSAGE_PASS;
}

BUS_Result_t xmgr_schedule_MsgRecordSetupFail_Base (void *pvContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	MgrRec_EndReasonType_e			eRecFailReason = (MgrRec_EndReasonType_e)p1; // fail reason은 이미 meta로 간 이후다.
	HUINT32					 		ulSlotId = (HUINT32)p2;
	HxList_t						*pstListItem;
	XmgrSch_RunningRec_t			*pstRunRec;
	XmgrSch_BaseContext_t			*pstContext = (XmgrSch_BaseContext_t *)pvContext;
	DxSchedule_t					stSchedule;
	DxSchedule_RemovedReason_e 	eRemoveReason = DxSchedule_RemovedReason_Unknown;
	DxSchedule_RemovedInfo_t stRemovedInfo = {0, };


	HxLOG_Trace();
	HxLOG_Debug("eRecFailReason: %d, ulSlotId: %d \n", eRecFailReason, ulSlotId);

	// Setup Fail 중 곧바로 Record Stop 일어나는 것들 외에는 반드시 cancel 시킨다.
	switch (eRecFailReason)
	{
	// Record Stop시키지 않는 요인들은 그냥 여기서 skip return 한다.
	case eMgrRecFailed_NOSIGNAL:
	case eMgrRecFailed_SI_TIMEOUT:
	case eMgrRecFailed_NO_VIDEO_AUDIO:
		return MESSAGE_PASS;

	default:
		break;
	}

	if ((NULL != pstContext) && (NULL != pstContext->pstRunRecList))
	{
		stSchedule.ulSlot = ulSlotId;
		// 어차피 slot으로 찾는다.
		pstListItem = HLIB_LIST_FindEx(pstContext->pstRunRecList, &stSchedule, xmgr_schedule_CmpScheduleSlot_Base);

		if(pstListItem != NULL)
		{
			pstRunRec = (XmgrSch_RunningRec_t *)HLIB_LIST_Data (pstListItem);

			if (NULL != pstRunRec)
			{
				HxLOG_Debug("remove from pstRunRecList \n");
				pstContext->pstRunRecList = HLIB_LIST_Remove(pstContext->pstRunRecList, (const void *)pstRunRec);
			}
		}
	}

	HxLOG_Debug("eRecFailReason is (%d) \n");

	if(eRecFailReason == eMgrRecFailed_NOHDD || eRecFailReason == eMgrRecFailed_NOUSBHDD)
	{
		eRemoveReason = DxSchedule_RemovedReason_NoPvrDevice;
	}
	else if (eRecFailReason == eMgrRecFailed_HDDFULL || eRecFailReason == eMgrRecFailed_INCOMPLETE_HDDFULL)
	{
		eRemoveReason = DxSchedule_RemovedReason_HddFull;
	}
	else if (eRecFailReason == eMgrRecFailed_MAXRECORDLISTNUM )
	{
		eRemoveReason = DxSchedule_RemovedReason_MaxRecList;
	}

	stRemovedInfo.eRemovedReason = eRemoveReason;
	(void)SAPI_IncompleteSchedule(ulSlotId, &stRemovedInfo);


	NOT_USED_PARAM (hAction);
	NOT_USED_PARAM (p3);

	return MESSAGE_PASS;
}

BUS_Result_t xmgr_schedule_MsgPvrPlaybackStop_Base (void *pvContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	xmgr_schedule_StopRunRec(pvContext, hAction);

	return MESSAGE_PASS;
}

#if defined(CONFIG_NON_TRANSCODING_STREAMING)
BUS_Result_t xmgr_schedule_MsgNtlsStopped_Base (void *pvContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	xmgr_schedule_StopRunRec(pvContext, hAction);

	return MESSAGE_PASS;
}
#endif

#if defined(CONFIG_STREAMING_SATIP)
BUS_Result_t xmgr_schedule_MsgSatipStartFailed_Base (void *pvContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	MgrSatIp_EndReasonType_e			eSatIpFailReason = (MgrSatIp_EndReasonType_e)p1;
	HUINT32					 			ulSlotId = (HUINT32)p2;
	DxSchedule_RemovedInfo_t stRemovedInfo = {0, };

	HxLOG_Trace();
	HxLOG_Debug("eSatIpFailReason: %d, ulSlotId: %d \n", eSatIpFailReason, ulSlotId);

	// Setup Fail 중 곧바로 Record Stop 일어나는 것들 외에는 반드시 cancel 시킨다.
	switch (eSatIpFailReason)
	{
		case eMgrSatIpFailed_CREATE:
			{
				stRemovedInfo.eRemovedReason = DxSchedule_RemovedReason_SatipFailed;
				stRemovedInfo.eDetailInfo	 = xmgr_schedule_GetSatIpFailInfo_Base(ulSlotId);
			}
			break;
		case eMgrSatIpFailed_NOSIGNAL:
			HxLOG_Debug("Not a stop condition. continue slot(%d) \n",ulSlotId);
			return MESSAGE_PASS;

	// Record Stop시키지 않는 요인들은 그냥 여기서 skip return 한다.
		default:
			stRemovedInfo.eRemovedReason = DxSchedule_RemovedReason_Unknown;
			break;
	}


	(void)SAPI_IncompleteSchedule(ulSlotId, &stRemovedInfo);

	NOT_USED_PARAM (hAction);
	NOT_USED_PARAM (p3);

	return MESSAGE_PASS;
}
#endif

#if defined(CONFIG_NON_TRANSCODING_STREAMING)
BUS_Result_t xmgr_schedule_MsgNtlsStartFailed_Base (void *pvContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	MgrNtls_EndReasonType_e			eNtlsFailReason = (MgrNtls_EndReasonType_e)p1;
	HUINT32					 		ulSlotId = (HUINT32)p2;

	DxSchedule_RemovedInfo_t stRemovedInfo = {0, };

	HxLOG_Trace();
	HxLOG_Debug("eNtlsFailReason: %d, ulSlotId: %d \n", eNtlsFailReason, ulSlotId);

	// Setup Fail 중 곧바로 Record Stop 일어나는 것들 외에는 반드시 cancel 시킨다.
	switch (eNtlsFailReason)
	{
		// Record Stop시키지 않는 요인들은 그냥 여기서 skip return 한다.
		default:
			break;
	}

	stRemovedInfo.eRemovedReason = DxSchedule_RemovedReason_Unknown;
	(void)SAPI_IncompleteSchedule(p2, &stRemovedInfo);

	NOT_USED_PARAM (hAction);
	NOT_USED_PARAM (p3);

	return MESSAGE_PASS;
}
#endif

BUS_Result_t xmgr_schedule_MsgRecordSetupMaxInstance_Base(void *pvContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
#if defined(CONFIG_MW_MM_LIMIT_REC)
	SAPI_RscInfo_t		*pstRscInfo = NULL;
	HERROR			 hErr = ERR_FAIL;
	HUINT32			ulRecMaxCnt = 0;
	SvcFs_DevIdx_e		eDevIdx = (SvcFs_DevIdx_e)p1;
	SvcFs_DevInfo_t		stDevInfo;

	hErr = SVC_FS_GetDeviceInfo(eDevIdx, &stDevInfo);
	if(hErr == ERR_OK)
	{

		pstRscInfo = (SAPI_RscInfo_t *)OxMGR_Calloc (sizeof(SAPI_RscInfo_t));
		if (NULL == pstRscInfo)
		{
			HxLOG_Error ("Memory allocation err:\n");
			goto END_FUNC;
		}

		hErr = SAPI_GetResourceInfo (pstRscInfo);
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("SAPI_GetResourceInfo err:\n");
			goto END_FUNC;
		}

		if(stDevInfo.ulMaxRecInstance > CONFIG_MW_MM_MAX_REC_NUM)
			stDevInfo.ulMaxRecInstance = CONFIG_MW_MM_MAX_REC_NUM;

		ulRecMaxCnt = stDevInfo.ulMaxRecInstance;

		pstRscInfo->anActionNum[eSAPI_ACTION_TYPE_Record] = ulRecMaxCnt;
		pstRscInfo->anMaxActionNum[eSAPI_ACTION_TYPE_Record] = ulRecMaxCnt + NUM_TSRREC_ACTION + NUM_HLS_ACTION;

		hErr = SAPI_SetResourceInfo (pstRscInfo);
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("SAPI_SetResourceInfo err:\n");
			goto END_FUNC;
		}

		BUS_PostMessageToOM(NULL, eMEVT_SCHEDULE_REC_CHANGE_INSTANCE, HANDLE_NULL, (HINT32)ulRecMaxCnt, (HINT32)(ulRecMaxCnt + NUM_TSRREC_ACTION + NUM_HLS_ACTION), 0);

	}

END_FUNC:
	if (NULL != pstRscInfo)					{ OxMGR_Free (pstRscInfo); }
#else
	NOT_USED_PARAM (pvContext);
	NOT_USED_PARAM (p1);

#endif
	NOT_USED_PARAM (hAction);
	NOT_USED_PARAM (p2);
	NOT_USED_PARAM (p3);

	return MESSAGE_PASS;
}

#define _________PLUGIN_FUNC__________

HERROR xmgr_schedule_SetRecordFailReason_Base (HUINT32 ulSlot, DxSchedule_RemovedReason_e eRemovedReason, HUINT32 eDetailedInfo)
{
	DxSchedule_RemovedInfo_t stRemovedInfo = {0, };
	#if 0
	enum // icord-mini 기준의 system msg
	{
		eRec_Sys_Msg60,  // 사용 되지 않음> icord-mini에서는 ui에는 존재하는데 실제 사용은 하지 않음. 추후 사용가능하면 바로 사용되도록 web에서만 만들어주면됨.
		eRec_Sys_Msg61,	// Rec : 녹화를 할수 없다는 msg
		eRec_Sys_Msg62,	// 사용 되지 않음> icord-mini에서는 ui에는 존재하는데 실제 사용은 하지 않음. 추후 사용가능하면 바로 사용되도록 web에서만 만들어주면됨.
		eRec_Sys_Msg63,	// Tsr : 요청된 timeshift를 할수 없다는 msg
		eRec_Sys_Msg64,	// Rec : 녹화중 저장공간이 부족하여 더이상 녹화가 불가능한경우 msg
		eRec_Sys_Msg65, // 사용 되지 않음> icord-mini에서는 ui에는 존재하는데 실제 사용은 하지 않음. 추후 사용가능하면 바로 사용되도록 web에서만 만들어주면됨.
		eRec_Sys_Msg66,	// Rec : 녹화 start 시 저장 공간이 부족하여 녹화가 불강능한경우 msg
		eRec_Sys_Msg68, 	// 사용 되지 않음> icord-mini에서는 ui에는 존재하는데 실제 사용은 하지 않음. 추후 사용가능하면 바로 사용되도록 web에서만 만들어주면됨.
		eRec_Sys_Msg69 	// Playback : tricm mode control에 의한 msg
	};
	#endif

	stRemovedInfo.eRemovedReason = eRemovedReason;
	stRemovedInfo.eDetailInfo = eDetailedInfo;

	return SAPI_IncompleteSchedule(ulSlot, &stRemovedInfo);
}

#define _____DEBUG_FUNCTION_____
#if defined(CONFIG_DEBUG)
STATIC HBOOL xmgr_schedule_base_get_comparePath (const DxRecListData_t *rec, void *path)
{
	return HLIB_STD_StrStartWith(rec->stMeta.szPath, path);
}

HINT32 xmgr_schedule_base_get(void *szArgList)
{
	/*
	 * ContentID ListUp
	 */

	{
		HxVector_t	*list;
		HINT32		i, nListCnt;
		DxRecListData_t 	*pSrcRec;

		list = OTL_HJM_GetRecordingList(xmgr_schedule_base_get_comparePath, (void *)"/mnt/hd2/");
		if (list == NULL)
		{
			HLIB_CMD_Printf("OTL_HJM_GetRecordingList by path:\'/mnt/hd2/\' fail!\n");
			return ERR_FAIL;
		}

		nListCnt = HLIB_VECTOR_Length(list);
		for (i=0; i<nListCnt ; i++)
		{
			pSrcRec = (DxRecListData_t *)HLIB_VECTOR_ItemAt(list, i);
			if (NULL == pSrcRec)
				continue;

			HLIB_CMD_Printf("[CONTENTID : %04d] ", pSrcRec->ulContentId);
			HLIB_CMD_Printf("\t[NAME : %s] ", pSrcRec->szName);
			HLIB_CMD_Printf("[SCRAMBLED : %d] ", pSrcRec->bScrambled);
			HLIB_CMD_Printf("\r\n");
		}
		HLIB_VECTOR_Delete(list);
	}



	return ERR_OK;
}

HINT32 xmgr_schedule_base_start(void *szArgList)
{

#if defined(CONFIG_MW_MM_PVR)
	HCHAR	*pucArg=NULL;
	HCHAR	aucStr[20];

	/* 1st argument */
	pucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
	if (NULL == pucArg)
		return HxCMD_ERR;

	if( sscanf(pucArg, "%s", aucStr) == 0 )
		return HxCMD_ERR;

	HLIB_STD_StrUpper(aucStr);

	if (HxSTD_StrNCmp(aucStr, "SCR2DESC", 7) == 0)
	{
		HUINT32 ulContentId = 0xFFFFFFFF;

		pucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
		if (sscanf(pucArg, "%d", &ulContentId) == 0)
			return HxCMD_ERR;

		if (0xFFFFFFFF == ulContentId)
			return HxCMD_ERR;
		/*Start Playback*/
		{
			HERROR hErr = ERR_FAIL;
			hErr = MGR_PVR_StartPlayback (ulContentId, 0);
			if (ERR_OK != hErr)
			{
				HLIB_CMD_Printf("MGR_PVR_StartPlayback. Error\n");
				return HxCMD_ERR;
			}
		}

		/*Start Recording*/
		{
			HUINT32	ulActionId = 0;
			MgrRec_RecSetup_t stRecSetup;
			MgrRec_EndReasonType_e eFailReason;
			HxSTD_MemSet (&stRecSetup, 0, sizeof(MgrRec_RecSetup_t));

			stRecSetup.eRecType 			= eMgrRecType_SCR2DESC;
			stRecSetup.eRecDst				= eMgrRecOutput_HDD;
			stRecSetup.hMasterSvc			= HANDLE_NULL;
			stRecSetup.hCurrSvc 			= HANDLE_NULL;
			stRecSetup.ulSlot				= -1;
			stRecSetup.ulSourceContentId	= ulContentId;

			eFailReason = MGR_PVR_StartRecording (&stRecSetup, &ulActionId);
			if (eMgrRecFailed_NOFAIL != eFailReason)
			{
				HLIB_CMD_Printf("MGR_PVR_StartRecording. Error\n");
				return HxCMD_ERR;
			}

		}
	}
#endif
#if 0
	HUINT32 ulSvcUid = 0;

	HLIB_CMD_Printf("=====================================================================\n");

	{
		HUINT32 ulMainActionId;

		ulMainActionId = MGR_ACTION_GetMainActionId();
		if (MGR_ACTION_GetType(ulMainActionId) == eViewType_WTV)
		{
			Handle_t		 hCurrSvc = HANDLE_NULL;
			MGR_LIVE_GetServiceHandle(ulMainActionId, eLiveViewPlaySvc_ACTUAL, &hCurrSvc);

			ulSvcUid = DB_SVC_GetServiceUID(hCurrSvc);
HLIB_CMD_Printf("hCurrSvc = 0x%08x, svcuid = 0x%x\n", hCurrSvc, ulSvcUid);
		}
	}

	{
		HERROR						 hErr;
		HUINT32						 ulSlot;
		DxSchedule_t				 stSchedule;
		HUINT32 					 ulCurrTime;

		hErr = SAPI_GetAvailableSlot(&ulSlot);
HLIB_CMD_Printf("hErr = %d, ulSlot = %d\n", hErr, ulSlot);

		HxSTD_MemSet(&stSchedule, 0, sizeof(DxSchedule_t));

		stSchedule.ulSlot			= ulSlot;
		stSchedule.eRsvType			= DxRSVTYPE_NETSTREAMING_LIVE;
		stSchedule.ulSvcUid			= ulSvcUid;
		stSchedule.ulDuration		= 0;
		stSchedule.eRepeat			= DxRSVREPEAT_ONCE;
		stSchedule.eEpgType			= eDxEPG_TYPE_ALL;

		ulCurrTime = HLIB_STD_GetSystemTime();
		(void)HLIB_DATETIME_ConvertUnixTimeToDateTime(ulCurrTime, &(stSchedule.stStartTime));

		hErr = SAPI_MakeSchedule (ulSlot, &stSchedule, NULL);
HLIB_CMD_Printf("startTime = %02d:%02d:%02d-%02d:%02d:%02d\n"
	, stSchedule.stStartTime.stDate.usYear
	, stSchedule.stStartTime.stDate.ucMonth
	, stSchedule.stStartTime.stDate.ucDay
	, stSchedule.stStartTime.stTime.ucHour
	, stSchedule.stStartTime.stTime.ucMinute
	, stSchedule.stStartTime.stTime.ucSecond
	);
HLIB_CMD_Printf("hErr = %d after SAPI_MakeSchedule()\n", hErr);
	}
#endif

	HLIB_CMD_Printf("-----------------------------------------------------------------------\n");

	return HxCMD_OK;
}
HINT32 xmgr_schedule_base_stop(void *szArgList)
{
	HCHAR	*pucArg=NULL;
//	HCHAR	aucStr[20];
	HUINT32	ulSlot;

	/* 1st argument */
	pucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
	if (NULL == pucArg)
		return HxCMD_ERR;

	if( sscanf(pucArg, "%d", &ulSlot) == 0 )
		return HxCMD_ERR;

	SAPI_CancelSchedule(ulSlot);

	HLIB_CMD_Printf("=====================================================================\n");
	return HxCMD_OK;
}

HINT32 xmgr_schedule_base_startMediaServer(void *szArgList)
{
//	HERROR					 hErr;
	HUINT32					 ulSlot=0;
	DxSchedule_t			 stSchedule;
	HUINT32 				 ulCurrTime;
	XmgrSch_BaseContext_t	*pstContext = xmgr_schedule_GetContext_Base();
	HUINT32					 ulMainActionId, ulSvcUid=0;

	ulMainActionId = MGR_ACTION_GetMainActionId();
	if (MGR_ACTION_GetType(ulMainActionId) == eViewType_WTV)
	{
		Handle_t		 hCurrSvc = HANDLE_NULL;
		MGR_LIVE_GetServiceHandle(ulMainActionId, eLiveViewPlaySvc_ACTUAL, &hCurrSvc);

		ulSvcUid = DB_SVC_GetServiceUID(hCurrSvc);
	}

	HxSTD_MemSet(&stSchedule, 0, sizeof(DxSchedule_t));
	ulCurrTime = HLIB_STD_GetSystemTime();
	(void)HLIB_DATETIME_ConvertUnixTimeToDateTime(ulCurrTime, &(stSchedule.stStartTime));

	stSchedule.ulSlot			= ulSlot;
	stSchedule.eRsvType			= DxRSVTYPE_NETSTREAMING_LIVE;
	stSchedule.ulSvcUid			= ulSvcUid;
	stSchedule.ulDuration		= 60;
	stSchedule.eRepeat			= DxRSVREPEAT_ONCE;
	stSchedule.eEpgType			= eDxEPG_TYPE_NONE;
	stSchedule.uExtInfo.stStrm.eStreamType	= DxRSVSTREAM_TYPE_HLS_LIVE_VIDEO;
	stSchedule.uExtInfo.stStrm.ulPlayerPosition	= 0;
	stSchedule.uExtInfo.stStrm.bTranscoding	= TRUE;
//	stSchedule.uExtInfo.stStrm.nPIDsCount = ;
//	stSchedule.uExtInfo.stStrm.szStreamingKey;

	(void)xmgr_schedule_StartMemRecord_Base(pstContext, &stSchedule);

	return HxCMD_OK;
}

STATIC HINT32 xmgr_schedule_base_MakeRsvItem (HUINT32 svcidx, HUINT16	usYear, HUINT8	ucMonth,	HUINT8	ucDay, HUINT8	ucHour, HUINT8 ucMinute, UNIXTIME utDuration)
{
	DxSchedule_t	stSchedule = {0, };
	HERROR			hErr = ERR_FAIL;
	Handle_t 		hSvc;
	SAPI_Conflict_t	stConflict;
	DbSvc_Info_t	stSvcInfo;

	VK_TASK_Sleep(500);

	HxLOG_Debug("========================================\n");
	HxLOG_Debug("[%s:%d] Start ... \n", __FUNCTION__, __LINE__);


	HxLOG_Debug("[%s:%d] STEP 1 \n", __FUNCTION__, __LINE__);

	VK_memset( &stSchedule, 0, sizeof(DxSchedule_t) );

	hErr = SAPI_GetAvailableSlot(&stSchedule.ulSlot);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("There is no empty slot for sama!\n");
		return HxCMD_ERR;
	}
	HxLOG_Debug("[%s:%d] STEP 2 stSchedule.ulSlot(%d)\n", __FUNCTION__, __LINE__, stSchedule.ulSlot);

    stSchedule.stStartTime.stDate.usYear = usYear;
    stSchedule.stStartTime.stDate.ucMonth = ucMonth;
	//if(s_TempHour >= 24)		ucDay++;

    stSchedule.stStartTime.stDate.ucDay = ucDay;
    stSchedule.stStartTime.stTime.ucHour = ucHour;
    stSchedule.stStartTime.stTime.ucMinute = ucMinute;
    stSchedule.stStartTime.stTime.ucSecond = 0;
    stSchedule.stStartTime.stTime.usMillisecond = 0;
#if 0
	stSchedule.eRsvType 		= DxRSVTYPE_RECORD_EBR;
#else
	stSchedule.eRsvType 		= DxRSVTYPE_RECORD_TBR;
#endif
	hErr = DB_SVC_GetServiceHandleBySvcIdx(svcidx, &hSvc);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("[%s:%d] DB_SVC_GetServiceHandleBySvcIdx Error !\n" , __FUNCTION__, __LINE__);
		return HxCMD_ERR;
	}
	if(DB_SVC_GetServiceInfo(hSvc, &stSvcInfo)==ERR_OK)
	{
		snprintf(stSchedule.uExtInfo.stRec.szEventName, DxRSV_EVENT_NAME_LEN, "%s", DbSvc_GetName(&stSvcInfo));
	}

	stSchedule.ulSvcUid			= DB_SVC_GetServiceUID(hSvc);
	stSchedule.eRsvReady		= DxRSVREADY_30_SEC;
	stSchedule.ulDuration		= utDuration*60;
	stSchedule.eRepeat			= DxRSVREPEAT_ONCE;
	stSchedule.ulRepeatDays 	= 0;
	stSchedule.eStatus			= DxRSVSTATUS_WAITING;
	stSchedule.bExceptConflict	= FALSE;
	stSchedule.eEpgType 		= eDxEPG_TYPE_SC;
#if 0
	stSchedule.uExtInfo.stRec.nEvtId			= stSchedule.ulSlot;
	stSchedule.uExtInfo.stRec.ulPreOffsetTime	= 300;
	stSchedule.uExtInfo.stRec.ulPostOffsetTime = 300;
#endif

	HxLOG_Debug("on Solt: %d,  SVC UID: 0x%x\n", stSchedule.ulSlot, stSchedule.ulSvcUid);
	HxLOG_Debug("[%s:%d] STEP 3 \n", __FUNCTION__, __LINE__);

	hErr = SAPI_MakeSchedule (stSchedule.ulSlot, &stSchedule, &stConflict);

	HxLOG_Debug("[%s:%d] Finished ... \n", __FUNCTION__, __LINE__);
	HxLOG_Debug("========================================\n");

	return HxCMD_OK;
}

HINT32 xmgr_schedule_base_makeTBRSchedule(void *szArgList)
{
	typedef enum {
		Kanal5 				= 2, 		// TS1
		TV8					= 6,		// TS2
		DiscoveryChannel	= 12,		// TS3
		DisneyJunior		= 20,		// TS4
		AnimalPlanet		= 29,		// TS5
		TV10				= 34,		// TS6
		CMoreSport			= 40,		// TS7
		NationalGeographic	= 46,		// TS8
	}CMD_MW_TEST_CHANNEL_ENUM;

	HUINT16 usYear	= 2015;
	HUINT8	ucMonth	= 9;
	HUINT8	ucDay	= 2;

	HxLOG_Debug("\n========================================\n");
	HxLOG_Debug("[%s:%d] Start ... \n", __FUNCTION__, __LINE__);
	HxLOG_Debug("========================================\n");

	xmgr_schedule_base_MakeRsvItem(Kanal5,				usYear, ucMonth, ucDay, 0, 	0, 	60);
	xmgr_schedule_base_MakeRsvItem(Kanal5,				usYear, ucMonth, ucDay, 1, 	30,	60);
	xmgr_schedule_base_MakeRsvItem(Kanal5,				usYear, ucMonth, ucDay, 3, 	0,	60);
	xmgr_schedule_base_MakeRsvItem(Kanal5,				usYear, ucMonth, ucDay, 6, 	0,	60);
	xmgr_schedule_base_MakeRsvItem(Kanal5,				usYear, ucMonth, ucDay, 8, 	0,	30);

	xmgr_schedule_base_MakeRsvItem(TV8,					usYear, ucMonth, ucDay, 0, 	30,	60);
	xmgr_schedule_base_MakeRsvItem(TV8,					usYear, ucMonth, ucDay, 2, 	0,	30);
	xmgr_schedule_base_MakeRsvItem(TV8,					usYear, ucMonth, ucDay, 3, 	30,	60);
	xmgr_schedule_base_MakeRsvItem(TV8,					usYear, ucMonth, ucDay, 5, 	30,	30);
	xmgr_schedule_base_MakeRsvItem(TV8,					usYear, ucMonth, ucDay, 6, 	30,	60);
	xmgr_schedule_base_MakeRsvItem(TV8,					usYear, ucMonth, ucDay, 8, 	0,	60);

	xmgr_schedule_base_MakeRsvItem(DiscoveryChannel,	usYear, ucMonth, ucDay, 0, 	0,	60);
	xmgr_schedule_base_MakeRsvItem(DiscoveryChannel,	usYear, ucMonth, ucDay, 1, 	30,	60);
	xmgr_schedule_base_MakeRsvItem(DiscoveryChannel,	usYear, ucMonth, ucDay, 3, 	0,	60);
	xmgr_schedule_base_MakeRsvItem(DiscoveryChannel,	usYear, ucMonth, ucDay, 6, 	0,	60);
	xmgr_schedule_base_MakeRsvItem(DiscoveryChannel,	usYear, ucMonth, ucDay, 8, 	0,	30);

	xmgr_schedule_base_MakeRsvItem(DisneyJunior,		usYear, ucMonth, ucDay, 0, 	30,	60);
	xmgr_schedule_base_MakeRsvItem(DisneyJunior,		usYear, ucMonth, ucDay, 2, 	0,	30);
	xmgr_schedule_base_MakeRsvItem(DisneyJunior,		usYear, ucMonth, ucDay, 3, 	30,	60);
	xmgr_schedule_base_MakeRsvItem(DisneyJunior,		usYear, ucMonth, ucDay, 5, 	30,	30);
	xmgr_schedule_base_MakeRsvItem(DisneyJunior,		usYear, ucMonth, ucDay, 6, 	30,	60);
	xmgr_schedule_base_MakeRsvItem(DisneyJunior,		usYear, ucMonth, ucDay, 8, 	0,	60);

	xmgr_schedule_base_MakeRsvItem(AnimalPlanet,		usYear, ucMonth, ucDay, 1, 	0,	30);
	xmgr_schedule_base_MakeRsvItem(AnimalPlanet,		usYear, ucMonth, ucDay, 4, 	0,	30);
	xmgr_schedule_base_MakeRsvItem(AnimalPlanet,		usYear, ucMonth, ucDay, 5, 	30,	30);
	xmgr_schedule_base_MakeRsvItem(AnimalPlanet,		usYear, ucMonth, ucDay, 7, 	0,	30);
	xmgr_schedule_base_MakeRsvItem(AnimalPlanet,		usYear, ucMonth, ucDay, 8, 	30,	30);

	xmgr_schedule_base_MakeRsvItem(TV10,				usYear, ucMonth, ucDay, 1, 	0,	30);
	xmgr_schedule_base_MakeRsvItem(TV10,				usYear, ucMonth, ucDay, 4, 	0,	30);
	xmgr_schedule_base_MakeRsvItem(TV10,				usYear, ucMonth, ucDay, 5, 	30,	30);
	xmgr_schedule_base_MakeRsvItem(TV10,				usYear, ucMonth, ucDay, 7, 	0,	30);
	xmgr_schedule_base_MakeRsvItem(TV10,				usYear, ucMonth, ucDay, 8, 	30,	30);

	xmgr_schedule_base_MakeRsvItem(CMoreSport,			usYear, ucMonth, ucDay, 0, 	0,	30);
	xmgr_schedule_base_MakeRsvItem(CMoreSport,			usYear, ucMonth, ucDay, 1, 	30,	30);
	xmgr_schedule_base_MakeRsvItem(CMoreSport,			usYear, ucMonth, ucDay, 3, 	0,	30);
	xmgr_schedule_base_MakeRsvItem(CMoreSport,			usYear, ucMonth, ucDay, 6, 	0,	30);

	xmgr_schedule_base_MakeRsvItem(NationalGeographic,	usYear, ucMonth, ucDay, 0, 	0,	30);
	xmgr_schedule_base_MakeRsvItem(NationalGeographic,	usYear, ucMonth, ucDay, 1, 	30,	30);
	xmgr_schedule_base_MakeRsvItem(NationalGeographic,	usYear, ucMonth, ucDay, 3, 	0,	30);
	xmgr_schedule_base_MakeRsvItem(NationalGeographic,	usYear, ucMonth, ucDay, 6, 	0,	30);

	HxLOG_Debug("========================================\n");
	HxLOG_Debug("[%s:%d] Finish ... \n", __FUNCTION__, __LINE__);
	HxLOG_Debug("========================================\n");

	return HxCMD_OK;
}

#endif
