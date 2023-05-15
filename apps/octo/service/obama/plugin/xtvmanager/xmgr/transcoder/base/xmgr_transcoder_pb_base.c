/**
	@file     	xmgr_transcoder_base.c
	@brief    	transcode recording control application.	\n
			this could handle n recording instances.

	Description:  						\n
	Module: 						 	\n

	Copyright (c) 2012 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <hlib.h>
#include <octo_common.h>

#include <svc_pipe.h>
#include <svc_pb.h>
#include <svc_rec.h>
#include <svc_meta.h>
#include <svc_transcoder.h>

#include <bus.h>
#include <mgr_common.h>
#include <mgr_action.h>
#include <mgr_transcoder.h>
//#include <xmgr_transcoder.h>
#include "../local_include/_xmgr_transcoder.h"

#define ___LOCAL_DEFINITIONS___

#define ___LOCAL_DATATYPES___

#define ___LOCAL_VARIABLES___

#define ___LOCAL_FUNCTION_PROTOTYPES___

#define ___LOCAL_FUNCTIONS___
#define _____SI_PACKET_INSERTION_____
STATIC HERROR mgr_trcd_pb_base_setPsiTable (xMgrTrcdContext_t *pstContext)
{
	SvcTranscoder_PSITable_t	*pstTrPsiTable = NULL;
	SvcMeta_Service_t			*pstPvrSvcInfo = &(pstContext->unSrc.stPb.stPvrSvcInfo);
	SvcSi_MakeSecArg_t			 stMakeSecArg;
	HERROR						 hErr, hResult = ERR_FAIL;
	HUINT16 					 usAudioPid = PID_NULL;
	SvcSi_MakeSectionSpec_t		 stMakeSecSpec;

	pstTrPsiTable = (SvcTranscoder_PSITable_t *)OxAP_Malloc (sizeof(SvcTranscoder_PSITable_t));
	if (NULL == pstTrPsiTable)
	{
		HxLOG_Error ("Memory allocation failed\n");
		goto END_FUNC;
	}

	/********************************************************/
	///////////////////////// PAT /////////////////////////////
	/********************************************************/
	HxSTD_memset(pstTrPsiTable, 0, sizeof(SvcTranscoder_PSITable_t));

	usAudioPid = pstPvrSvcInfo->usAudioPid;
	if( PID_NULL == usAudioPid ) usAudioPid = pstPvrSvcInfo->usDolbyPid;

	stMakeSecArg.eSiType 		= eSIxTABLETYPE_Pat;
	stMakeSecArg.stPat.usTsId	= pstPvrSvcInfo->usTsId;
	stMakeSecArg.stPat.usSvcId	= pstPvrSvcInfo->usSvcId;
	stMakeSecArg.stPat.usPmtPid	= pstPvrSvcInfo->usPmtPid;

	hErr = MGR_TRANSCODER_GetMakeSectionSiSpec(&stMakeSecSpec);
	if (hErr == ERR_OK)
	{
		hErr = SVC_SI_MakeSiSection(SVC_PIPE_GetActionId(pstContext->hAction), &stMakeSecSpec, &stMakeSecArg, pstTrPsiTable->aucRawSectionBuffer, &(pstTrPsiTable->ulRawSectionSize));
	}

	if ((ERR_OK == hErr) && (pstTrPsiTable->ulRawSectionSize > 0))
	{
		pstTrPsiTable->bContinue		=	TRUE;
		pstTrPsiTable->usPeriodTime 	=	1000;
		pstTrPsiTable->usPid			=	PAT_PID;
		pstTrPsiTable->eSiType			=	eSI_TableType_PAT;
		pstTrPsiTable->usFlag			=	1;

		hErr = SVC_TRANSCODER_SetPSITable (pstContext->hAction, pstTrPsiTable, eSyncMode);
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("SVC_TRANSCODER_SetPSITable err: PAT\n");
		}
	}
	else
	{
		HxLOG_Error ("SVC_PVR_MakeSiSection err: PAT failed\n");
	}

	/********************************************************/
	///////////////////////// PMT /////////////////////////////
	/********************************************************/
	HxSTD_memset(pstTrPsiTable, 0, sizeof(SvcTranscoder_PSITable_t));

	stMakeSecArg.eSiType 			= eSIxTABLETYPE_Pmt;
	stMakeSecArg.stPmt.eMakeSrcType	= eSVCSI_MAKESEC_PMT_SetByArgs;
	stMakeSecArg.stPmt.usSvcId		= pstPvrSvcInfo->usSvcId;
	stMakeSecArg.stPmt.unArgs.stSettingArg.usPcrPid 			= pstPvrSvcInfo->usPcrPid;
	stMakeSecArg.stPmt.unArgs.stSettingArg.ucVideoStreamType	= pstContext->stTranscoderProfile.ucVideoStreamType;			// H.264로 Transcoding 했을 것이므로 ISO_VIDEO_4로
	stMakeSecArg.stPmt.unArgs.stSettingArg.usVideoPid			= pstPvrSvcInfo->usVideoPid;
	stMakeSecArg.stPmt.unArgs.stSettingArg.ucAudioStreamType	= pstContext->stTranscoderProfile.ucAudioStreamType;		// AAC이므로 ISO_AUDIO_4 혹은 ISO_AUDIO_AAC
	stMakeSecArg.stPmt.unArgs.stSettingArg.usAudioPid			= usAudioPid;

	hErr = MGR_TRANSCODER_GetMakeSectionSiSpec(&stMakeSecSpec);
	if (hErr == ERR_OK)
	{
		hErr = SVC_SI_MakeSiSection(SVC_PIPE_GetActionId(pstContext->hAction), &stMakeSecSpec, &stMakeSecArg, pstTrPsiTable->aucRawSectionBuffer, &(pstTrPsiTable->ulRawSectionSize));
	}

	if ((ERR_OK == hErr) && (pstTrPsiTable->ulRawSectionSize > 0))
	{
		pstTrPsiTable->bContinue		=	TRUE;
		pstTrPsiTable->usPeriodTime 	=	1000;
		pstTrPsiTable->usPid			=	pstPvrSvcInfo->usPmtPid;
		pstTrPsiTable->eSiType			=	eSI_TableType_PMT;
		pstTrPsiTable->usFlag			=	1;

		hErr = SVC_TRANSCODER_SetPSITable (pstContext->hAction, pstTrPsiTable, eSyncMode);
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("SVC_TRANSCODER_SetPSITable err: PMT\n");
		}
	}
	else
	{
		HxLOG_Error ("SVC_PVR_MakeSiSection err: PMT failed\n");
	}

	/********************************************************/
	///////////////////////// SIT /////////////////////////////
	/********************************************************/
	HxSTD_memset(pstTrPsiTable, 0, sizeof(SvcTranscoder_PSITable_t));

	stMakeSecArg.eSiType 			= eSIxTABLETYPE_Sit;
	stMakeSecArg.stSit.usTsUniqId	= DB_SVC_GetAllUniqueId();
	stMakeSecArg.stSit.usOnId		= pstPvrSvcInfo->usOnId;
	stMakeSecArg.stSit.usTsId		= pstPvrSvcInfo->usTsId;
	stMakeSecArg.stSit.usSvcId		= pstPvrSvcInfo->usSvcId;

	hErr = MGR_TRANSCODER_GetMakeSectionSiSpec(&stMakeSecSpec);
	if (hErr == ERR_OK)
	{
		hErr = SVC_SI_MakeSiSection(SVC_PIPE_GetActionId(pstContext->hAction), &stMakeSecSpec, &stMakeSecArg, pstTrPsiTable->aucRawSectionBuffer, &(pstTrPsiTable->ulRawSectionSize));
	}

	if ((ERR_OK == hErr) && (pstTrPsiTable->ulRawSectionSize > 0))
	{
		pstTrPsiTable->bContinue		=	TRUE;
		pstTrPsiTable->usPeriodTime 	=	1000;
		pstTrPsiTable->usPid			=	SIT_PID;
		pstTrPsiTable->eSiType			=	eSI_TableType_SIT;
		pstTrPsiTable->usFlag			=	1;

		hErr = SVC_TRANSCODER_SetPSITable (pstContext->hAction, pstTrPsiTable, eSyncMode);
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("SVC_TRANSCODER_SetPSITable err: PMT\n");
		}
	}
	else
	{
		HxLOG_Error ("SVC_PVR_MakeSiSection err: SIT failed\n");
	}

	hResult = ERR_OK;

END_FUNC:
	if (NULL != pstTrPsiTable)					{ OxAP_Free (pstTrPsiTable); }

	return hResult;
}

STATIC HERROR mgr_trcd_pb_base_updatePsiTable (xMgrTrcdContext_t *pstContext, SiTableType_e eSiTable)
{
	HBOOL						 bUpdatePsi = FALSE;
	HUINT32						 ulSecLen = 0;
	HUINT8						*pucRawSec = NULL;
	SvcTranscoder_PSITable_t	*pstTrPsiTable = NULL;
	SvcMeta_Service_t			*pstPvrSvcInfo = &(pstContext->unSrc.stPb.stPvrSvcInfo);
	SvcSi_MakeSecArg_t			 stMakeSecArg;
	HERROR						 hErr;
	SvcSi_MakeSectionSpec_t		 stMakeSecSpec;

	pstTrPsiTable = (SvcTranscoder_PSITable_t *)OxAP_Malloc (sizeof(SvcTranscoder_PSITable_t));
	if (NULL == pstTrPsiTable)
	{
		HxLOG_Error ("Memory allocation failed\n");
		return ERR_FAIL;
	}

	switch (eSiTable)
	{
	case eSI_TableType_PMT:
		pucRawSec = (HUINT8 *)OxAP_Malloc (4096);
		if (NULL != pucRawSec)
		{
			stMakeSecArg.eSiType			= eSIxTABLETYPE_Pmt;
			stMakeSecArg.stPmt.eMakeSrcType	= eSVCSI_MAKESEC_PMT_Transcode;
			stMakeSecArg.stPmt.usSvcId		= pstPvrSvcInfo->usSvcId;
			stMakeSecArg.stPmt.usTsUniqId 	= DB_SVC_GetAllUniqueId();

			stMakeSecArg.stPmt.unArgs.stSettingArg.usPcrPid				= pstPvrSvcInfo->usPcrPid;
			stMakeSecArg.stPmt.unArgs.stSettingArg.ucVideoStreamType 	= pstContext->stTranscoderProfile.ucVideoStreamType;			// H.264로 Transcoding 했을 것이므로 ISO_VIDEO_4로
			stMakeSecArg.stPmt.unArgs.stSettingArg.usVideoPid			= pstPvrSvcInfo->usVideoPid;
			stMakeSecArg.stPmt.unArgs.stSettingArg.ucAudioStreamType 	= pstContext->stTranscoderProfile.ucAudioStreamType;		// AAC이므로 ISO_AUDIO_4 혹은 ISO_AUDIO_AAC
			stMakeSecArg.stPmt.unArgs.stSettingArg.usAudioPid			= pstPvrSvcInfo->usAudioPid;

			hErr = MGR_TRANSCODER_GetMakeSectionSiSpec(&stMakeSecSpec);
			if (hErr == ERR_OK)
			{
				hErr = SVC_SI_MakeSiSection(SVC_PIPE_GetActionId(pstContext->hAction), &stMakeSecSpec, &stMakeSecArg, pucRawSec, &ulSecLen);
			}

			if (ERR_OK == hErr)
			{
				pstTrPsiTable->bContinue		=	TRUE;
				pstTrPsiTable->usPeriodTime 	=	1000;
				pstTrPsiTable->usPid			=	pstPvrSvcInfo->usPmtPid;
				pstTrPsiTable->eSiType			=	eSI_TableType_PMT;
				pstTrPsiTable->usFlag			=	1;

				HxSTD_memcpy(pstTrPsiTable->aucRawSectionBuffer, pucRawSec, ulSecLen);
				pstTrPsiTable->ulRawSectionSize = ulSecLen;

				bUpdatePsi = TRUE;
			}
			else
			{
				HxLOG_Error ("SVC_PVR_MakeSiSection err:\n");
			}

			OxAP_Free (pucRawSec);
		}

		break;

	case eSI_TableType_SIT:
#if 0
		hErr = SVC_PVR_GetRawSection (pstContext->hAction, eSI_TableType_SIT, FALSE, 0, 0, &pucRawSec, &ulSecLen);
		if (ERR_OK == hErr)
		{
			pstTrPsiTable->bContinue		=	TRUE;
			pstTrPsiTable->usPeriodTime 	=	1000;
			pstTrPsiTable->usPid			=	SIT_PID;
			pstTrPsiTable->eSiType			=	eSI_TableType_SIT;
			pstTrPsiTable->usFlag			=	1;

			HxSTD_memcpy(pstTrPsiTable->aucRawSectionBuffer, pucRawSec, ulSecLen);
			pstTrPsiTable->ulRawSectionSize = ulSecLen;

			SVC_PVR_FreeRawSection (pucRawSec);
			bUpdatePsi = TRUE;
		}
		else
		{
			HxLOG_Error ("SVC_PVR_GetRawSection eSI_TableType_SIT\n");
		}
#endif
		break;
	default:
		break;

	}

	if (TRUE == bUpdatePsi)
	{
		hErr = SVC_TRANSCODER_SetPSITable (pstContext->hAction, pstTrPsiTable, eSyncMode);
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("SVC_TRANSCODER_SetPSITable err: hAction(0x%08x)\n", pstContext->hAction);
		}
	}

	OxAP_Free (pstTrPsiTable);
	return ERR_OK;
}



#define ____TRANSCODER_FUNCTIONS_____
STATIC HERROR mgr_trcd_pb_base_setTranscoderSetting (xMgrTrcdContext_t *pstContext, MgrTrcd_Start_t *pstStart)
{
	HxSTD_MemSet (&(pstContext->stTranscoderProfile), 0, sizeof(SvcTranscoder_Profile_t));

	// 기본 설정 : AAC & H.264
	pstContext->stTranscoderProfile.eAudioCodec = eDxAUDIO_CODEC_AAC;
	pstContext->stTranscoderProfile.ucAudioStreamType = ISO_AUDIO_AAC;

	pstContext->stTranscoderProfile.eVideoCodec = eDxVIDEO_CODEC_H264;
	pstContext->stTranscoderProfile.ucVideoStreamType= ISO_VIDEO_4;

	HxSTD_MemCopy (&(pstContext->stProfile), &(pstStart->stProfile), sizeof(MgrTrcd_StreamProfile));

	return ERR_OK;
}

STATIC HERROR mgr_trcd_pb_base_startTranscoder (xMgrTrcdContext_t *pstContext)
{
	HUINT32						 ulActionId = SVC_PIPE_GetActionId (pstContext->hAction);
	HUINT32						 ulHddRecId, ulTranscoderId, ulDemuxId;
	SvcMeta_Service_t				*pstPvrSvcInfo = &(pstContext->unSrc.stPb.stPvrSvcInfo);
	SvcMeta_Record_t				*pstPvrRecInfo = NULL;
	SvcTranscoder_Settings_t	 stSettings;
	HERROR						 hErr, hResult = ERR_FAIL;
	HUINT16 					 usAudioPid = PID_NULL;
	DxAudioCodec_e 				 eAudioCodec;

	pstPvrRecInfo = (SvcMeta_Record_t *)OxAP_Calloc (sizeof(SvcMeta_Record_t));
	if (NULL == pstPvrRecInfo)
	{
		HxLOG_Error ("Memory Allocation failed\n");
		goto END_FUNC;
	}

	hErr = SVC_META_GetRecordInfo (pstContext->unSrc.stPb.ulContentId, pstPvrRecInfo);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("SVC_META_GetRecordInfo err: ulContentId=%d\n", pstContext->unSrc.stPb.ulContentId);
		goto END_FUNC;
	}

	hErr = SVC_PIPE_GetResourceId (ulActionId, eRxRSCTYPE_HDD_REC, &ulHddRecId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("SVC_PIPE_GetResourceId(eRxRSCTYPE_HDD_REC) failed: ActionId(%d)\n", ulActionId);
		goto END_FUNC;
	}

	hErr = SVC_PIPE_GetResourceId (ulActionId, eRxRSCTYPE_TRANSCODER, &ulTranscoderId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("SVC_PIPE_GetResourceId(eRxRSCTYPE_TRANSCODER) failed: ActionId(%d)\n", ulActionId);
		goto END_FUNC;
	}

	hErr = SVC_PIPE_GetResourceId (ulActionId, eRxRSCTYPE_DEMUX, &ulDemuxId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("SVC_PIPE_GetResourceId(eRxRSCTYPE_DEMUX) failed: ActionId(%d)\n", ulActionId);
		goto END_FUNC;
	}

	HxSTD_memset (&stSettings, 0,  sizeof(SvcTranscoder_Settings_t));

	/*include PCR Ts packet*/
	stSettings.bTranscode			=	TRUE;
	stSettings.bPcrTsPacketInsert	=	pstContext->bPcrTsPacketInsert;

	stSettings.bEncryption = pstPvrRecInfo->u1cEncryption;

	/*status*/
	stSettings.eTransEncodeMode		= (eDxSVC_TYPE_RADIO == pstPvrSvcInfo->eSvcType) ? eSVC_TRANSCODER_ENCODEMODE_AUDIO : eSVC_TRANSCODER_ENCODEMODE_AV;
	stSettings.eTransOutputType		= pstContext->eOutputType;
	stSettings.eTransSourceType		= pstContext->eSourceType;

	/*resource id*/
	stSettings.ulRecDeviceId		= ulHddRecId;
	stSettings.ulDeviceId			= ulTranscoderId;
	stSettings.ulDemuxId			= ulDemuxId;

	/*src*/
	usAudioPid	= pstPvrSvcInfo->usAudioPid;
	eAudioCodec	= pstPvrSvcInfo->eAudioCodec;

	if( PID_NULL == usAudioPid )
	{
		usAudioPid = pstPvrSvcInfo->usDolbyPid;
		eAudioCodec = pstPvrSvcInfo->eDolbyCodec;
	}

	stSettings.stAvPid.usVideoPid	=	pstPvrSvcInfo->usVideoPid;
	stSettings.stAvPid.usAudioPid	=	usAudioPid;
	stSettings.stAvPid.usPcrPid		=	pstPvrSvcInfo->usPcrPid;
	stSettings.eVideoCodec			=	pstPvrSvcInfo->eVideoCodec;
	stSettings.eAudioCodec			=	eAudioCodec;

	/*profile*/
//	HxSTD_MemCopy (&(stSettings.stProfile), &(pstContext->stTranscoderProfile), sizeof(SvcTranscoder_Profile_t));

	stSettings.nTranscodeProfileId				=	-1;
	stSettings.stProfile.eVideoCodec			=	pstContext->stProfile.eVideoCodec;
	stSettings.stProfile.eAudioCodec			=	pstContext->stProfile.eAudioCodec;
	stSettings.stProfile.ucVideoStreamType		=	0;//pstContext->stProfile.;
	stSettings.stProfile.ucAudioStreamType		=	0;//pstContext->stProfile.;

	stSettings.stProfile.nAudioBitrate_kbps 	=	pstContext->stProfile.nAudioBitrateKbps;
	stSettings.stProfile.nVideoBitrate_kbps 	=	pstContext->stProfile.nVideoBitrateKbps;
	stSettings.stProfile.nHdVideoBitrate_kbps	=	pstContext->stProfile.nHdVideoBitrateKbps;
	stSettings.stProfile.nScreenWidth			=	pstContext->stProfile.nScreenWidth;
	stSettings.stProfile.nScreenHeight			=	pstContext->stProfile.nScreenHeight;
	stSettings.stProfile.nFrameRate 			=	pstContext->stProfile.nFrameRate;

	stSettings.stProfile.bIsInteraceMode		=	pstContext->stProfile.bInterlaceMode;

	stSettings.stProfile.eVideoAspectRatio		=	pstContext->stProfile.eVideoAspectRatio;

	stSettings.stProfile.eVideoCodecProfile 		=	pstContext->stProfile.eVideoCodecProfile;
	stSettings.stProfile.eVideoCodecLevel			=	pstContext->stProfile.eVideoCodecLevel;
	stSettings.stProfile.bUsingLiveAudioCodec		=	pstContext->stProfile.bUsingLiveAudioCodec;
	stSettings.stProfile.bUsingLiveAspectRatio		=	pstContext->stProfile.bUsingLiveAspectRatio;



	/*output setting*/
	switch (pstContext->eOutputType)
	{
	case eSVC_TRANSCODER_OUTPUTTYPE_FILE:
		HxSTD_StrNCpy (stSettings.acuFilename, pstContext->unOutput.stFile.szFileUrl, MAX_TRANS_FILENAME_LEN-1);
		break;

	case eSVC_TRANSCODER_OUTPUTTYPE_BUFFER:
		stSettings.hRingBuf			=	pstContext->unOutput.stRingBuf.hRingBuf;
		break;

	default:
		HxLOG_Error ("OutputType invalid (%d)\n", pstContext->eOutputType);
		goto END_FUNC;
	}

	hErr = SVC_TRANSCODER_Open (pstContext->hAction, &stSettings, eSyncMode);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("SVC_TRANSCODER_Open err: hAction(0x%08x)\n", pstContext->hAction);
		goto END_FUNC;
	}

	// Set PST Table (PAT, PMT, SIT)
	hErr = mgr_trcd_pb_base_setPsiTable (pstContext);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("SVC_TRANSCODER_Open err:\n");
		goto END_FUNC;
	}

	hErr = SVC_TRANSCODER_StartService (pstContext->hAction, eSyncMode);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("OxSVC_Transcoder_Start err: hAction(0x%08x)\n", pstContext->hAction);
		goto END_FUNC;
	}

	pstContext->eState = eTrcdState_Running;
	hResult = ERR_OK;

END_FUNC:
	if (NULL != pstPvrRecInfo)					{ OxAP_Free (pstPvrRecInfo); }
	return hResult;
}

STATIC HERROR mgr_trcd_pb_base_stopTranscoder (xMgrTrcdContext_t *pstContext)
{
	HERROR			 hErr;

	if (eTrcdState_Running == pstContext->eState)
	{
		hErr = SVC_TRANSCODER_StopService (pstContext->hAction, eSyncMode);
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("SVC_TRANSCODER_StopService err: hAction(0x%08x)\n", pstContext->hAction);
			return ERR_FAIL;
		}

		pstContext->eState = eTrcdState_Stopped;
	}

	return ERR_OK;
}

#define _____MESSAGE_PROCESS_FUNCTIONS_____
STATIC BUS_Result_t mgr_trcd_pb_base_msgBusCreate (xMgrTrcdContext_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HBOOL					 bStarted = FALSE;
	MgrTrcd_Start_t		*pstStart = (MgrTrcd_Start_t *)p1;
	HERROR					 hErr;

	if (NULL == pstContext)
	{
		HxLOG_Error ("Context NULL!! Procedure Kills.\n");
		goto END_FUNC;
	}

	if (NULL == pstStart)
	{
		HxLOG_Error ("Start Arguments!! Procedure Kills.\n");
		goto END_FUNC;
	}

	XMGR_TRCD_SetStartArguments (pstContext, hAction, pstStart);

	// Start the transcoder:
	hErr = mgr_trcd_pb_base_startTranscoder (pstContext);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("mgr_trcd_pb_base_startTranscoder err: Quit the transcoder:\n");
		goto END_FUNC;
	}

	bStarted = TRUE;

END_FUNC:
	if (TRUE != bStarted)				// 제대로 시작되지 않았으면 Transcoder를 종료시킨다.
	{
		BUS_MGR_Destroy (NULL);
	}

	NOT_USED_PROC_ARGUMENT (hAction, p1, p2, p3);
	return MESSAGE_BREAK;
}

STATIC BUS_Result_t mgr_trcd_pb_base_msgBusDestroy (xMgrTrcdContext_t *pstContext, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	Handle_t				 hAction;
	HERROR					 hErr;

	if (NULL == pstContext)
	{
		HxLOG_Error ("Context NULL!!\n");
		return MESSAGE_BREAK;
	}

	hAction = pstContext->hAction;

	hErr = mgr_trcd_pb_base_stopTranscoder (pstContext);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("mgr_trcd_pb_base_startTranscoder err\n");
	}

	HxSTD_memset (pstContext, 0, sizeof(xMgrTrcdContext_t));
	pstContext->eState			= eTrcdState_Idle;

	BUS_SendMessage (NULL, eMEVT_PVR_PLAYBACK_STOP, hAction, 0, 0, 0);

	NOT_USED_PROC_ARGUMENT (hHandle, p1, p2, p3);
	return MESSAGE_BREAK;
}

STATIC BUS_Result_t mgr_trcd_pb_base_msgAppPlaybackNotifyStopped (xMgrTrcdContext_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32					 ulActionId = SVC_PIPE_GetActionId (hAction);

	if (ulActionId == SVC_PIPE_GetActionId(pstContext->hAction))
	{
		BUS_MGR_Destroy (NULL);
	}

	NOT_USED_PROC_ARGUMENT (hAction, p1, p2, p3);
	return MESSAGE_PASS;
}

STATIC BUS_Result_t mgr_trcd_pb_base_evtPbEof (xMgrTrcdContext_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32					 ulActionId = SVC_PIPE_GetActionId (hAction);
	HERROR					 hErr;

	if (ulActionId == SVC_PIPE_GetActionId(pstContext->hAction))
	{
		hErr = mgr_trcd_pb_base_stopTranscoder (pstContext);
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("mgr_trcd_pb_base_startTranscoder err\n");
		}

		BUS_SendMessage (NULL, eMEVT_PVR_PLAYBACK_STOP, hAction, 0, 0, 0);
	}

	NOT_USED_PROC_ARGUMENT (hAction, p1, p2, p3);
	return MESSAGE_PASS;
}

STATIC BUS_Result_t mgr_trcd_pb_base_evtPvrSitChanged (xMgrTrcdContext_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32					 ulActionId = SVC_PIPE_GetActionId (hAction);

	if (ulActionId == SVC_PIPE_GetActionId(pstContext->hAction))
	{
		mgr_trcd_pb_base_updatePsiTable (pstContext, eSI_TableType_SIT);
	}

	NOT_USED_PROC_ARGUMENT (hAction, p1, p2, p3);
	return MESSAGE_PASS;
}

STATIC BUS_Result_t mgr_trcd_pb_base_evtPvrPmtChanged (xMgrTrcdContext_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32					 ulActionId = SVC_PIPE_GetActionId (hAction);

	if (ulActionId == SVC_PIPE_GetActionId(pstContext->hAction))
	{
		mgr_trcd_pb_base_updatePsiTable (pstContext, eSI_TableType_PMT);
	}

	NOT_USED_PROC_ARGUMENT (hAction, p1, p2, p3);
	return MESSAGE_PASS;
}

STATIC BUS_Result_t mgr_trcd_pb_base_evtStopTranscoding (xMgrTrcdContext_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32					 ulActionId = SVC_PIPE_GetActionId (hAction);
	HERROR					 hErr;

	if (ulActionId == SVC_PIPE_GetActionId(pstContext->hAction))
	{
		hErr = mgr_trcd_pb_base_stopTranscoder (pstContext);
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("mgr_trcd_pb_base_startTranscoder err\n");
		}

		BUS_SendMessage (NULL, eMEVT_PVR_PLAYBACK_STOP, hAction, 0, 0, 0);
	}

	NOT_USED_PROC_ARGUMENT (hAction, p1, p2, p3);
	return MESSAGE_PASS;
}


#define _____PROCEDURE_PROTO_FUNCTION_____
STATIC BUS_Result_t mgr_trcd_pb_base_protoProc (xMgrTrcdContext_t *pstContext, HINT32 message, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t			 eRet = MESSAGE_PASS;

	switch (message)
	{
	case eMEVT_BUS_CREATE:
		HxLOG_Info("[mgr_trcd_pb_base_protoProc]MESSAGE : eMEVT_BUS_CREATE\n");
		mgr_trcd_pb_base_setTranscoderSetting (pstContext, (MgrTrcd_Start_t *)p1);
		eRet = mgr_trcd_pb_base_msgBusCreate (pstContext, hHandle, p1, p2, p3);
		break;

	case eMEVT_BUS_DESTROY:
		HxLOG_Info("[mgr_trcd_pb_base_protoProc]MESSAGE : eMEVT_BUS_DESTROY\n");
		eRet = mgr_trcd_pb_base_msgBusDestroy (pstContext, hHandle, p1, p2, p3);
		break;

	case eMEVT_PVR_NOTIFY_PLAYBACK_STOPPED:
		HxLOG_Info("[mgr_trcd_pb_base_protoProc]MESSAGE : eMEVT_PVR_NOTIFY_PLAYBACK_STOPPED\n");
		eRet = mgr_trcd_pb_base_msgAppPlaybackNotifyStopped (pstContext, hHandle, p1, p2, p3);
		break;

	case eSEVT_PB_EOF:
		HxLOG_Info("[mgr_trcd_pb_base_protoProc]MESSAGE : eSEVT_PB_EOF\n");
		eRet = mgr_trcd_pb_base_evtPbEof (pstContext, hHandle, p1, p2, p3);
		break;
/*
	case EVT_PVR_SIT_CHANGED:
		HxLOG_Info("[mgr_trcd_pb_base_protoProc]MESSAGE : EVT_PVR_SIT_CHANGED\n");
		eRet = mgr_trcd_pb_base_evtPvrSitChanged (pstContext, hHandle, p1, p2, p3);
		break;
*/
	case eMEVT_PVR_STOP_TRANSCODING:
		HxLOG_Info("[mgr_trcd_pb_base_protoProc]MESSAGE : eMEVT_PVR_STOP_TRANSCODING\n");
		eRet = mgr_trcd_pb_base_evtStopTranscoding(pstContext, hHandle, p1, p2, p3);
		break;

	default:
		break;
	}

	return (MESSAGE_BREAK == eRet) ? MESSAGE_BREAK : MESSAGE_PASS;
}

STATIC BUS_Result_t mgr_trcd_pb_base_procView0 (HINT32 message, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (eMEVT_BUS_CREATE == message)
	{
		BUS_MGR_SetMgrName (BUS_MGR_GetThis(), (HINT8 *)__FUNCTION__);
	}

	return mgr_trcd_pb_base_protoProc (XMGR_TRCD_GetContext (eActionId_VIEW_FIRST + 0), message, hHandle, p1, p2, p3);
}

STATIC BUS_Result_t mgr_trcd_pb_base_procView1 (HINT32 message, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (eMEVT_BUS_CREATE == message)
	{
		BUS_MGR_SetMgrName (BUS_MGR_GetThis(), (HINT8 *)__FUNCTION__);
	}

	return mgr_trcd_pb_base_protoProc (XMGR_TRCD_GetContext (eActionId_VIEW_FIRST + 1), message, hHandle, p1, p2, p3);
}

STATIC BUS_Result_t mgr_trcd_pb_base_procBg0 (HINT32 message, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (eMEVT_BUS_CREATE == message)
	{
		BUS_MGR_SetMgrName (BUS_MGR_GetThis(), (HINT8 *)__FUNCTION__);
	}

	return mgr_trcd_pb_base_protoProc (XMGR_TRCD_GetContext (eActionId_BG_FIRST + 0), message, hHandle, p1, p2, p3);
}

STATIC BUS_Result_t mgr_trcd_pb_base_procBg1 (HINT32 message, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (eMEVT_BUS_CREATE == message)
	{
		BUS_MGR_SetMgrName (BUS_MGR_GetThis(), (HINT8 *)__FUNCTION__);
	}

	return mgr_trcd_pb_base_protoProc (XMGR_TRCD_GetContext (eActionId_BG_FIRST + 1), message, hHandle, p1, p2, p3);
}

STATIC BUS_Result_t mgr_trcd_pb_base_procBg2 (HINT32 message, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (eMEVT_BUS_CREATE == message)
	{
		BUS_MGR_SetMgrName (BUS_MGR_GetThis(), (HINT8 *)__FUNCTION__);
	}

	return mgr_trcd_pb_base_protoProc (XMGR_TRCD_GetContext (eActionId_BG_FIRST + 2), message, hHandle, p1, p2, p3);
}

STATIC BUS_Result_t mgr_trcd_pb_base_procBg3 (HINT32 message, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (eMEVT_BUS_CREATE == message)
	{
		BUS_MGR_SetMgrName (BUS_MGR_GetThis(), (HINT8 *)__FUNCTION__);
	}

	return mgr_trcd_pb_base_protoProc (XMGR_TRCD_GetContext (eActionId_BG_FIRST + 3), message, hHandle, p1, p2, p3);
}

STATIC BUS_Result_t mgr_trcd_pb_base_procBg4 (HINT32 message, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (eMEVT_BUS_CREATE == message)
	{
		BUS_MGR_SetMgrName (BUS_MGR_GetThis(), (HINT8 *)__FUNCTION__);
	}

	return mgr_trcd_pb_base_protoProc (XMGR_TRCD_GetContext (eActionId_BG_FIRST + 4), message, hHandle, p1, p2, p3);
}

STATIC BUS_Result_t mgr_trcd_pb_base_procBg5 (HINT32 message, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (eMEVT_BUS_CREATE == message)
	{
		BUS_MGR_SetMgrName (BUS_MGR_GetThis(), (HINT8 *)__FUNCTION__);
	}

	return mgr_trcd_pb_base_protoProc (XMGR_TRCD_GetContext (eActionId_BG_FIRST + 5), message, hHandle, p1, p2, p3);
}

STATIC BUS_Result_t mgr_trcd_pb_base_procBg6 (HINT32 message, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (eMEVT_BUS_CREATE == message)
	{
		BUS_MGR_SetMgrName (BUS_MGR_GetThis(), (HINT8 *)__FUNCTION__);
	}

	return mgr_trcd_pb_base_protoProc (XMGR_TRCD_GetContext (eActionId_BG_FIRST + 6), message, hHandle, p1, p2, p3);
}

STATIC BUS_Result_t mgr_trcd_pb_base_procBg7 (HINT32 message, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (eMEVT_BUS_CREATE == message)
	{
		BUS_MGR_SetMgrName (BUS_MGR_GetThis(), (HINT8 *)__FUNCTION__);
	}

	return mgr_trcd_pb_base_protoProc (XMGR_TRCD_GetContext (eActionId_BG_FIRST + 7), message, hHandle, p1, p2, p3);
}

#define ___MEMBER_FUNCTIONS___
BUS_Result_t MGR_TRCD_PB_BASE_MsgBusCreate (xMgrTrcdContext_t *pstContext, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return mgr_trcd_pb_base_msgBusCreate (pstContext, hHandle, p1, p2, p3);
}

BUS_Result_t MGR_TRCD_PB_BASE_MsgBusDestroy (xMgrTrcdContext_t *pstContext, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return mgr_trcd_pb_base_msgBusDestroy (pstContext, hHandle, p1, p2, p3);
}

BUS_Result_t MGR_TRCD_PB_BASE_MsgAppPlaybackNotifyStopped (xMgrTrcdContext_t *pstContext, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return mgr_trcd_pb_base_msgAppPlaybackNotifyStopped (pstContext, hHandle, p1, p2, p3);
}

BUS_Result_t MGR_TRCD_PB_BASE_EvtPbEof (xMgrTrcdContext_t *pstContext, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return mgr_trcd_pb_base_evtPbEof (pstContext, hHandle, p1, p2, p3);
}

BUS_Result_t MGR_TRCD_PB_BASE_EvtPvrSitChanged (xMgrTrcdContext_t *pstContext, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return mgr_trcd_pb_base_evtPvrSitChanged (pstContext, hHandle, p1, p2, p3);
}

BUS_Result_t MGR_TRCD_PB_BASE_EvtPvrPmtChanged (xMgrTrcdContext_t *pstContext, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return mgr_trcd_pb_base_evtPvrPmtChanged (pstContext, hHandle, p1, p2, p3);
}

BUS_Result_t MGR_TRCD_PB_BASE_EvtStopTranscoding(xMgrTrcdContext_t *pstContext, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return mgr_trcd_pb_base_evtStopTranscoding (pstContext, hHandle, p1, p2, p3);
}

#define ___PORTING_LAYER_FUNCTIONS___
BUS_Callback_t XMGR_TRCD_PB_BASE_GetActionProc (HUINT32 ulActionId)
{
	if (XMGR_TRCD_GetContext (ulActionId) == NULL)
	{
		HxLOG_Error ("Action ID(%d) is not allowed for transcoding\n");
		return NULL;
	}

	switch (ulActionId)
	{
	case (eActionId_VIEW_FIRST + 0):
		return mgr_trcd_pb_base_procView0;

	case (eActionId_VIEW_FIRST + 1):
		return mgr_trcd_pb_base_procView1;

	case (eActionId_BG_FIRST + 0):
		return mgr_trcd_pb_base_procBg0;

	case (eActionId_BG_FIRST + 1):
		return mgr_trcd_pb_base_procBg1;

	case (eActionId_BG_FIRST + 2):
		return mgr_trcd_pb_base_procBg2;

	case (eActionId_BG_FIRST + 3):
		return mgr_trcd_pb_base_procBg3;

	case (eActionId_BG_FIRST + 4):
		return mgr_trcd_pb_base_procBg4;

	case (eActionId_BG_FIRST + 5):
		return mgr_trcd_pb_base_procBg5;

	case (eActionId_BG_FIRST + 6):
		return mgr_trcd_pb_base_procBg6;

	case (eActionId_BG_FIRST + 7):
		return mgr_trcd_pb_base_procBg7;

	default:
		break;
	}

	return NULL;
}

// end of file
