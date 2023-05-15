/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  svc_rec_func_base.c
	@brief

	Description:  									\n
	Module: PlugIn / Recorder						\n

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
#include <octo_common.h>

#include <pal_pipe.h>
#include <pal_rec.h>
#include <pal_sys.h>

#include <isosvc.h>
//#include <mwc_util.h>

#include <svc_rec.h>

#include "./local_include/_svc_rec.h"

#include <hlib.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define	REC_PIDREQ_ID_NULL			0xFFFF
#define REC_DELAYED_TEMP_NAME		".tsr_temp"
//#define EXT_META_FILE				".hjm"

#define ENTER_FUNCTION	HxLOG_Debug("(+)\r\n")
#define LEAVE_FUNCTION	HxLOG_Info("(-)\r\n")


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef enum
{
	svc_rec_pidtype_si,
	svc_rec_pidtype_video,
	svc_rec_pidtype_audio,
	svc_rec_pidtype_subtile,
	svc_rec_pidtype_teletext,
	svc_rec_pidtype_caption,
	svc_rec_pidtype_ecm,
	svc_rec_pidtype_emm,
	svc_rec_pidtype_data,

	svc_rec_pidtype_manual,

	svc_rec_pidtype_all,

	svc_rec_pidtype_max
}svc_rec_pidtype_e;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC PostMsgToMgrCb_t s_fnSvcRec_PostMsgToAp = NULL;

#define ___LOCAL_FUNCTIONS___

STATIC void _svc_rec_DebugPrintRecPidInfo(SvcRec_PIDList_t *pstPIDList, HCHAR *funcName, HINT32 nLine)
{
#ifdef CONFIG_DEBUG
	HINT32		i = 0;


	HxLOG_Debug("[%s:%d] ** mgr_rec_DebugPrintRecPidInfo **\n", funcName, nLine);

	if(NULL == pstPIDList)
	{
		HxLOG_Error("\t pstPIDList is NULL\n");
		return;
	}

	HxLOG_Debug("\t pstPIDList->ePidSetType      = %d \n", pstPIDList->ePidSetType);

	if(eRec_PidSetType_Known == pstPIDList->ePidSetType)
	{
		for(i = 0; i < eSvcRec_MAX_REC_NUM; i++)
		{
			if(PID_NULL != pstPIDList->stKnown.stSI[i].usPid)
			{
				HxLOG_Debug("\t stKnown.stSI[%d]\t= (PID:0x%04X, ReqID:%d) \n", i, pstPIDList->stKnown.stSI[i].usPid, pstPIDList->stKnown.stSI[i].ulRequestId);
			}
		}
		for(i = 0; i < SvcRec_MAX_VIDEO_ES_NUM; i++)
		{
			if(PID_NULL != pstPIDList->stKnown.stVideo[i].usPid)
			{
				HxLOG_Debug("\t stKnown.stVideo[%d]\t= (PID:0x%04X, ReqID:%d) \n", i, pstPIDList->stKnown.stVideo[i].usPid, pstPIDList->stKnown.stVideo[i].ulRequestId);
			}
		}
		for(i = 0; i < SvcRec_MAX_AUDIO_ES_NUM; i++)
		{
			if(PID_NULL != pstPIDList->stKnown.stAudio[i].usPid)
			{
				HxLOG_Debug("\t stKnown.stAudio[%d]\t= (PID:0x%04X, ReqID:%d) \n", i, pstPIDList->stKnown.stAudio[i].usPid, pstPIDList->stKnown.stAudio[i].ulRequestId);
			}
		}
		for(i = 0; i < SvcRec_MAX_SUBTTL_ES_NUM; i++)
		{
			if(PID_NULL != pstPIDList->stKnown.stSubtitle[i].usPid)
			{
				HxLOG_Debug("\t stKnown.stSubtitle[%d]\t= (PID:0x%04X, ReqID:%d) \n", i, pstPIDList->stKnown.stSubtitle[i].usPid, pstPIDList->stKnown.stSubtitle[i].ulRequestId);
			}
		}
		for(i = 0; i < SvcRec_MAX_TELTXT_ES_NUM; i++)
		{
			if(PID_NULL != pstPIDList->stKnown.stTeletext[i].usPid)
			{
				HxLOG_Debug("\t stKnown.stTeletext[%d]\t= (PID:0x%04X, ReqID:%d) \n", i, pstPIDList->stKnown.stTeletext[i].usPid, pstPIDList->stKnown.stTeletext[i].ulRequestId);
			}
		}
		for(i = 0; i < SvcRec_MAX_CAPTION_ES_NUM; i++)
		{
			if(PID_NULL != pstPIDList->stKnown.stCaption[i].usPid)
			{
				HxLOG_Debug("\t stKnown.stCaption[%d]\t= (PID:0x%04X, ReqID:%d) \n", i, pstPIDList->stKnown.stCaption[i].usPid, pstPIDList->stKnown.stCaption[i].ulRequestId);
			}
		}
		for(i = 0; i < SvcRec_MAX_CA_PID_NUM; i++)
		{
			if(PID_NULL != pstPIDList->stKnown.stEcm[i].usPid)
			{
				HxLOG_Debug("\t stKnown.stEcm[%d]\t= (PID:0x%04X, ReqID:%d) \n", i, pstPIDList->stKnown.stEcm[i].usPid, pstPIDList->stKnown.stEcm[i].ulRequestId);
			}
		}
		for(i = 0; i < SvcRec_MAX_CA_PID_NUM; i++)
		{
			if(PID_NULL != pstPIDList->stKnown.stEmm[i].usPid)
			{
				HxLOG_Debug("\t stKnown.stEmm[%d]\t= (PID:0x%04X, ReqID:%d) \n", i, pstPIDList->stKnown.stEmm[i].usPid, pstPIDList->stKnown.stEmm[i].ulRequestId);
			}
		}
		for(i = 0; i < SvcRec_MAX_DATA_ES_NUM; i++)
		{
			if(PID_NULL != pstPIDList->stKnown.stData[i].usPid)
			{
				HxLOG_Debug("\t stKnown.stData[%d]\t= (PID:0x%04X, ReqID:%d) \n", i, pstPIDList->stKnown.stData[i].usPid, pstPIDList->stKnown.stData[i].ulRequestId);
			}
		}
	}
	else if(eRec_PidSetType_Unknown == pstPIDList->ePidSetType)
	{
		for(i = 0; i < DxRSV_PIDTABLE_COUNT; i++)
		{
			if(PID_NULL != pstPIDList->stUnknown.stPIDs[i].usPid)
			{
				HxLOG_Debug("\t stUnknown.stPIDs[%d]\t= (PID:0x%04X, ReqID:%d) \n", i, pstPIDList->stUnknown.stPIDs[i].usPid, pstPIDList->stUnknown.stPIDs[i].ulRequestId);
			}
		}
	}
	else
	{
		HxLOG_Debug("\t stUnknown.stPIDs\t= (PID:ALL, ReqID:%d) \n", pstPIDList->stUnknown.stPIDs[i].ulRequestId);
	}

	HxLOG_Debug("---------------------------------------------------------------\n");
#endif
}

STATIC void svc_rec_PostMsgToAp(HINT32 nMsg, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	if (s_fnSvcRec_PostMsgToAp)
	{
		 (* s_fnSvcRec_PostMsgToAp)(nMsg, hAction, nParam1, nParam2, nParam3);
	}
}

STATIC HERROR svc_rec_FuncGetResourceIds(Handle_t hAction, Handle_t hPBAction, SvcRec_ActType_e	eRecActType, HUINT32 *pulActionId, HUINT32 *pulDemuxId, HUINT32 *pulRecordId)
{
	HERROR	hErr	= ERR_OK;

	if( (NULL == pulActionId) || (NULL == pulDemuxId) || (NULL == pulRecordId) )
		return ERR_FAIL;

	*pulActionId = PAL_PIPE_GetActionId( hAction );

	hErr = PAL_PIPE_GetResourceId(*pulActionId, eRxRSCTYPE_HDD_REC, pulRecordId);
	if( hErr != ERR_OK )
	{
		HxLOG_Error ("PAL_PIPE_GetResourceId error, hErr:0x%X, ulActionId:0x%X\n", hErr, *pulActionId);
		return hErr;
	}

	if( eRec_ActType_Descramble_Record == eRecActType )
	{
		/*
		 * NOTE : SCR2DESC 동작시 DEMUXID는 PB_DEMUXID 사용함.
		 */
		HUINT32 ulPBActionId = PAL_PIPE_GetActionId (hPBAction);

		hErr = PAL_PIPE_GetResourceId(ulPBActionId, eRxRSCTYPE_DEMUX, pulDemuxId);
		if( hErr != ERR_OK )
		{
			HxLOG_Error ("PAL_PIPE_GetResourceId error, DEMUX, hErr:0x%X, ulActionId:0x%X\n", hErr, *pulActionId);
			return hErr;
		}
	}
	else
	{
		hErr = PAL_PIPE_GetResourceId(*pulActionId, eRxRSCTYPE_DEMUX, pulDemuxId);
		if( hErr != ERR_OK )
		{
			HxLOG_Error ("PAL_PIPE_GetResourceId error, DEMUX, hErr:0x%X, ulActionId:0x%X\n", hErr, *pulActionId);
			return hErr;
		}
	}

	HxLOG_Debug ("ulActionId=%x, ulRecordId=%x, ulDemuxId=%x\n", *pulActionId, *pulRecordId, *pulDemuxId);

	return hErr;
}

STATIC HERROR svc_rec_FuncSetupRecToPAL(HUINT32 ulDemuxId, HUINT32 ulRecordId,
										SvcRec_ActType_e eRecActType, SvcRec_OutType_e eRecOutType,
										DxSvcType_e eSvcType, DxVideoCodec_e eVideoCodec,
										HUINT8 *pucPath, HUINT8 *pucFileName,
										HBOOL bIsUsbFlashRecord,
										HUINT32 ulLimitTimeSecond,
										HUINT64 ullLimitDataSize,
										HUINT64 ullLimitIndexSize)
{
	HERROR hErr = ERR_OK;
	PalRec_Setup_t stPalSetup;

	HxSTD_MemSet( &stPalSetup, 0, sizeof(PalRec_Setup_t) );

	stPalSetup.ulDemuxId	= ulDemuxId;
	stPalSetup.bTimestamp	= TRUE;

	if( eRecActType == eRec_ActType_Tsr_Record )
	{
		stPalSetup.eFileType = ePAL_FS_FILETYPE_CIRCULAR;
	}
	else
	{
		stPalSetup.eFileType = ePAL_FS_FILETYPE_LINEAR;
	}

	if ((eRec_ActType_Satip == eRecActType)
		|| (eRec_ActType_Ntls == eRecActType))
	{
		stPalSetup.bIsAllPidChannel =	TRUE;
	}
	else if (eRec_ActType_FullTs == eRecActType)
	{
		stPalSetup.bIsAllPidChannel =	TRUE;
		stPalSetup.bTimestamp =	FALSE;
		stPalSetup.stEncryptSettings.eEncType = ePAL_PVR_ENCRYPTION_TYPE_NONE;
	}

	if( eDxSVC_TYPE_TV == eSvcType )
	{
		stPalSetup.eStreamType	=	eSTREAMTYPE_VIDEO;

		switch (eVideoCodec)
		{
			case eDxVIDEO_CODEC_MPEG2:
			case eDxVIDEO_CODEC_H264:
			case eDxVIDEO_CODEC_H265:
			case eDxVIDEO_CODEC_DIVX311:
			case eDxVIDEO_CODEC_VC1:
				stPalSetup.eVideoType	=	eVideoCodec;
				break;
			default :
				HxLOG_Error("Default VideoCodec!\n");
				stPalSetup.eVideoType	=	eDxVIDEO_CODEC_MPEG2;
				break;
		}

	}
	else
	{
		stPalSetup.eStreamType	=	eSTREAMTYPE_AUDIO;
	}

	if( eRec_OutType_File == eRecOutType )
	{
		snprintf( stPalSetup.aucFileName, PAL_PVR_MAX_FILE_NAME_FULLPATH_LENGTH-1, "%s%s", pucPath, pucFileName);
	}
	else
	{
		snprintf( stPalSetup.aucFileName, PAL_PVR_MAX_FILE_NAME_FULLPATH_LENGTH-1, "memio");
		stPalSetup.bTimestamp	= FALSE;
	}

	/* set tsr-limitSize*/
	stPalSetup.stUsbFlash.bIsUsbFlashRecord = bIsUsbFlashRecord;
	stPalSetup.stUsbFlash.ulLimitTimeSecond = ulLimitTimeSecond;
	stPalSetup.stUsbFlash.ullLimitDataSize	= ullLimitDataSize;
	stPalSetup.stUsbFlash.ullLimitIndexSize = ullLimitIndexSize;

	hErr = PAL_REC_Setup((HUINT32)ulRecordId, &stPalSetup);
	if( hErr != ERR_OK )
	{
		HxLOG_Error ("PAL_REC_Setup error, hErr:0x%X\n", hErr);
		return hErr;
	}

	return hErr;
}

STATIC HERROR svc_rec_FuncUpdateReqPid( HUINT32 ulRecordId, svc_rec_pidtype_e ePidType, SvcRec_PIDDes_t *pstCurrInfo, SvcRec_PIDDes_t *pstPIDList, HBOOL *pbChanged)
{
	HERROR hErr			=	ERR_FAIL;
	HUINT16 usMaxNum	=	0;
	EsType_t ePESType = eEsType_None;

	if( (NULL == pstCurrInfo) || (NULL == pstPIDList) || (NULL == pbChanged) )
		return ERR_FAIL;

	switch(ePidType)
	{
		#if 0 // svc_rec_pidtype_si 케이스 에서는 사용하지 않고, svc_rec_FuncUpdateSiReqPid()로 분리 - 2014-02-14
		case svc_rec_pidtype_si:		usMaxNum = eSvcRec_MAX_REC_NUM;			ePESType = eEsType_None;			break;
		#endif
		case svc_rec_pidtype_video:		usMaxNum = SvcRec_MAX_VIDEO_ES_NUM;		ePESType = eEsType_Video;			break;
		case svc_rec_pidtype_audio:		usMaxNum = SvcRec_MAX_AUDIO_ES_NUM;		ePESType = eEsType_Audio;			break;
		case svc_rec_pidtype_subtile:	usMaxNum = SvcRec_MAX_SUBTTL_ES_NUM;	ePESType = eEsType_Subtitle;		break;
		case svc_rec_pidtype_teletext:	usMaxNum = SvcRec_MAX_TELTXT_ES_NUM;	ePESType = eEsType_Teletext;		break;
		case svc_rec_pidtype_caption:	usMaxNum = SvcRec_MAX_CAPTION_ES_NUM;	ePESType = eEsType_ClosedCaption;	break;
		case svc_rec_pidtype_ecm:		usMaxNum = SvcRec_MAX_CA_PID_NUM;		ePESType = eEsType_None;			break;
		case svc_rec_pidtype_emm:		usMaxNum = SvcRec_MAX_CA_PID_NUM;		ePESType = eEsType_None;			break;
		case svc_rec_pidtype_data:		usMaxNum = SvcRec_MAX_DATA_ES_NUM;		ePESType = eEsType_None;			break;
		case svc_rec_pidtype_manual:	usMaxNum = DxRSV_PIDTABLE_COUNT;		ePESType = eEsType_None;			break;
		case svc_rec_pidtype_all:
		{
			hErr = PAL_REC_StartPid(ulRecordId, 0, eEsType_All, &pstCurrInfo[0].ulRequestId );
			if (ERR_OK != hErr)
			{
				HxLOG_Error("PAL_REC_StartPid ulRecordId(%d) eEsType_All failed:\n", ulRecordId);
			}

			return hErr;
		}

		default:
			return ERR_FAIL;
	}

	/*start/stop pid, update*/
	{
		HINT32 i=0, j=0;

		/* stop */
		for( i=0; i<usMaxNum; i++ )
		{
			if( PID_NULL != pstCurrInfo[i].usPid )
			{
				for( j=0; j<usMaxNum; j++ )
				{
					if( pstCurrInfo[i].usPid == pstPIDList[j].usPid )
					{
						pstPIDList[j].usPid = PID_NULL;
						break;
					}
				}

				if( j == usMaxNum ) //not found
				{
					if( (PID_NULL != pstCurrInfo[i].usPid) && (REC_PIDREQ_ID_NULL != pstCurrInfo[i].ulRequestId) )
					{
						hErr = PAL_REC_StopPid( ulRecordId, pstCurrInfo[i].ulRequestId );
						pstCurrInfo[i].usPid 		= PID_NULL;
						pstCurrInfo[i].ulRequestId	= REC_PIDREQ_ID_NULL;

						if( FALSE == *pbChanged )
							*pbChanged	=	TRUE;
					}
				}
			}
		}

		/* start */
		for( i=0; i<usMaxNum; i++ )
		{
			if( PID_NULL != pstPIDList[i].usPid )
			{
				for( j=0; j<usMaxNum; j++ )
				{
					if( PID_NULL == pstCurrInfo[j].usPid )
						break;
				}

				if( j != usMaxNum ) //get a empty-slot
				{
					/* exception - PCR */
					EsType_t _ePESType = ePESType;
					if( (svc_rec_pidtype_si == ePidType) && (eREC_SiPCR == i) )
						_ePESType = eEsType_PCR;

					pstCurrInfo[j].usPid = pstPIDList[i].usPid;
					hErr = PAL_REC_StartPid( ulRecordId, pstCurrInfo[j].usPid, _ePESType, &pstCurrInfo[j].ulRequestId );

#if defined(CONFIG_DEBUG)
					if (ERR_OK == hErr)
					{
						HCHAR *pPidTypeName = NULL;

						switch (ePidType)
						{
							case svc_rec_pidtype_si:		pPidTypeName = "SI   "; break;
							case svc_rec_pidtype_video:		pPidTypeName = "VIDEO"; break;
							case svc_rec_pidtype_audio:		pPidTypeName = "AUDIO"; break;
							case svc_rec_pidtype_subtile:	pPidTypeName = "SUBTI"; break;
							case svc_rec_pidtype_teletext:	pPidTypeName = "TELET"; break;
							case svc_rec_pidtype_caption:	pPidTypeName = "CAPTI"; break;
							case svc_rec_pidtype_ecm:		pPidTypeName = "ECM  "; break;
							case svc_rec_pidtype_emm:		pPidTypeName = "EMM	 "; break;
							case svc_rec_pidtype_data:		pPidTypeName = "DATA "; break;
							case svc_rec_pidtype_manual:	pPidTypeName = "MAN  "; break;
							case svc_rec_pidtype_all:		pPidTypeName = "ALL  "; break;
							default:
								pPidTypeName = "DEF  ";
								break;
						}

						HxLOG_Info ("[%s] 0x%x PID START\n", pPidTypeName, pstCurrInfo[j].usPid);
					}
#endif
					if( FALSE == *pbChanged )
						*pbChanged	=	TRUE;
				}
				else
				{
					HxLOG_Error("not found pid-slot\n" );
					return ERR_FAIL;
				}

			}
		}

	}

	return hErr;
}

STATIC HERROR svc_rec_FuncUpdateSiReqPid( HUINT32 ulRecordId, SvcRec_PIDDes_t *pstCurrInfo, SvcRec_PIDDes_t *pstPIDList, HBOOL *pbChanged)
{
	HERROR		hErr = ERR_FAIL;
	HINT32		i = 0;

	if( (NULL == pstCurrInfo) || (NULL == pstPIDList) || (NULL == pbChanged) )
		return ERR_FAIL;

	/* stop pids */
	for(i = 0; i < eSvcRec_MAX_REC_NUM; i++)
	{
		if( pstCurrInfo[i].usPid != pstPIDList[i].usPid )
		{
			if( (PID_NULL != pstCurrInfo[i].usPid) && (REC_PIDREQ_ID_NULL != pstCurrInfo[i].ulRequestId) )
			{
				HxLOG_Debug("\t stop pid : pstCurrInfo[%d].usPid(0x%x) <== pstPIDList[%d].usPid(0x%x)\n", i, pstCurrInfo[i].usPid, i, pstPIDList[i].usPid);

				hErr = PAL_REC_StopPid( ulRecordId, pstCurrInfo[i].ulRequestId );
				pstCurrInfo[i].usPid 		= PID_NULL;
				pstCurrInfo[i].ulRequestId	= REC_PIDREQ_ID_NULL;

				*pbChanged = TRUE;
			}
		}
	}

	/* update & start pid */
	for(i = 0; i < eSvcRec_MAX_REC_NUM; i++)
	{
		if( pstCurrInfo[i].usPid != pstPIDList[i].usPid )
		{
			if( PID_NULL != pstPIDList[i].usPid )
			{
				HxLOG_Debug("\t start pid : pstCurrInfo[%d].usPid(0x%x) <== pstPIDList[%d].usPid(0x%x)\n", i, pstCurrInfo[i].usPid, i, pstPIDList[i].usPid);

				/* exception - PCR */
				if( eREC_SiPCR == i )
				{
					hErr = PAL_REC_StartPid( ulRecordId, pstPIDList[i].usPid, eEsType_PCR, &pstPIDList[i].ulRequestId );
				}
				else
				{
					hErr = PAL_REC_StartPid( ulRecordId, pstPIDList[i].usPid, eEsType_None, &pstPIDList[i].ulRequestId );
				}

				pstCurrInfo[i].usPid = pstPIDList[i].usPid;
				pstCurrInfo[i].ulRequestId = pstPIDList[i].ulRequestId;

				if (ERR_OK == hErr)
				{
					HxLOG_Info ("[SI   ] 0x%x PID START\n", "SI   ", pstCurrInfo[i].usPid);
				}

				*pbChanged = TRUE;
			}
		}
	}

	return hErr;
}

STATIC HERROR  svc_rec_FuncUpdateRecordPid( HUINT32 ulRecordId, SvcRec_PIDList_t *pstCurrPIDList, SvcRec_PIDList_t *pstPIDList, HBOOL *pbChanged )
{
	HERROR hErr = ERR_OK;

	if( (NULL == pstCurrPIDList) || (NULL == pstPIDList) || (NULL == pbChanged) )
		return ERR_FAIL;

	if (eRec_PidSetType_Known == pstPIDList->ePidSetType)
	{
		/* si enum 값이랑 index 오류로 수정 - 2014-02-14 */
#if 0
		hErr |= svc_rec_FuncUpdateReqPid( ulRecordId, svc_rec_pidtype_si,			pstCurrPIDList->stKnown.stSI, 		pstPIDList->stKnown.stSI		,pbChanged);
#else
		hErr |= svc_rec_FuncUpdateSiReqPid( ulRecordId, 							pstCurrPIDList->stKnown.stSI, 		pstPIDList->stKnown.stSI		,pbChanged);
#endif
		hErr |= svc_rec_FuncUpdateReqPid( ulRecordId, svc_rec_pidtype_video,		pstCurrPIDList->stKnown.stVideo, 	pstPIDList->stKnown.stVideo		,pbChanged);
		hErr |= svc_rec_FuncUpdateReqPid( ulRecordId, svc_rec_pidtype_audio, 		pstCurrPIDList->stKnown.stAudio, 	pstPIDList->stKnown.stAudio		,pbChanged);
		hErr |= svc_rec_FuncUpdateReqPid( ulRecordId, svc_rec_pidtype_subtile, 		pstCurrPIDList->stKnown.stSubtitle,	pstPIDList->stKnown.stSubtitle	,pbChanged);
		hErr |= svc_rec_FuncUpdateReqPid( ulRecordId, svc_rec_pidtype_teletext, 	pstCurrPIDList->stKnown.stTeletext,	pstPIDList->stKnown.stTeletext	,pbChanged);
		hErr |= svc_rec_FuncUpdateReqPid( ulRecordId, svc_rec_pidtype_caption, 		pstCurrPIDList->stKnown.stCaption, 	pstPIDList->stKnown.stCaption	,pbChanged);
		hErr |= svc_rec_FuncUpdateReqPid( ulRecordId, svc_rec_pidtype_ecm, 			pstCurrPIDList->stKnown.stEcm, 		pstPIDList->stKnown.stEcm		,pbChanged);
		hErr |= svc_rec_FuncUpdateReqPid( ulRecordId, svc_rec_pidtype_emm, 			pstCurrPIDList->stKnown.stEmm, 		pstPIDList->stKnown.stEmm		,pbChanged);
		hErr |= svc_rec_FuncUpdateReqPid( ulRecordId, svc_rec_pidtype_data, 		pstCurrPIDList->stKnown.stData, 	pstPIDList->stKnown.stData		,pbChanged);
	}
	else if (eRec_PidSetType_Unknown == pstPIDList->ePidSetType)
	{
		hErr |= svc_rec_FuncUpdateReqPid( ulRecordId, svc_rec_pidtype_manual, 		pstCurrPIDList->stUnknown.stPIDs, 	pstPIDList->stUnknown.stPIDs ,pbChanged);
	}
	else if (eRec_PidSetType_All == pstPIDList->ePidSetType)
	{
		hErr |= svc_rec_FuncUpdateReqPid( ulRecordId, svc_rec_pidtype_all, 			pstCurrPIDList->stUnknown.stPIDs, 	pstPIDList->stUnknown.stPIDs ,pbChanged);
	}
	else
	{
		hErr = ERR_FAIL;
	}

	return hErr;
}

void svc_rec_FuncInitPidDes( SvcRec_PIDDes_t *pstPidDesArray, HINT32 nArrayCnt )
{
	HINT32 i=0;

	for( i=0; i<nArrayCnt; i++ )
	{
		pstPidDesArray[i].usPid			=	PID_NULL;
		pstPidDesArray[i].ulRequestId	=	REC_PIDREQ_ID_NULL;
	}

	return ;
}

STATIC HERROR svc_rec_FuncStopPid(HUINT32 ulRecordId, svc_rec_pidtype_e ePidType, SvcRec_PIDDes_t *pstCurrInfo)
{
	HERROR	hErr			=	ERR_OK;
	HUINT16 usMaxNum		=	0;
	HINT32 	i				=	0;

	if( NULL == pstCurrInfo )
		return ERR_FAIL;

	switch(ePidType)
	{
		case svc_rec_pidtype_si:		usMaxNum = eSvcRec_MAX_REC_NUM;			break;
		case svc_rec_pidtype_video:		usMaxNum = SvcRec_MAX_VIDEO_ES_NUM;		break;
		case svc_rec_pidtype_audio:		usMaxNum = SvcRec_MAX_AUDIO_ES_NUM;		break;
		case svc_rec_pidtype_subtile:	usMaxNum = SvcRec_MAX_SUBTTL_ES_NUM;	break;
		case svc_rec_pidtype_teletext:	usMaxNum = SvcRec_MAX_TELTXT_ES_NUM;	break;
		case svc_rec_pidtype_caption:	usMaxNum = SvcRec_MAX_CAPTION_ES_NUM;	break;
		case svc_rec_pidtype_ecm:		usMaxNum = SvcRec_MAX_CA_PID_NUM;		break;
		case svc_rec_pidtype_emm:		usMaxNum = SvcRec_MAX_CA_PID_NUM;		break;
		case svc_rec_pidtype_data:		usMaxNum = SvcRec_MAX_DATA_ES_NUM;		break;
		case svc_rec_pidtype_manual:	usMaxNum = DxRSV_PIDTABLE_COUNT; 		break;
		case svc_rec_pidtype_all:
		{
			if (REC_PIDREQ_ID_NULL != pstCurrInfo[0].ulRequestId)
			{
				hErr = PAL_REC_StopPid(ulRecordId, pstCurrInfo[0].ulRequestId);
				if (ERR_OK == hErr)
				{
					pstCurrInfo[0].ulRequestId	= REC_PIDREQ_ID_NULL;
				}
				else
				{
					HxLOG_Error("PAL_REC_StopPid error: requestId(0x%08x)\n", pstCurrInfo[0].ulRequestId);
				}
			}

			return ERR_OK;
		}

		default:
			return ERR_FAIL;
	}

	for( i=0; i<usMaxNum; i++ )
	{
		if( REC_PIDREQ_ID_NULL != pstCurrInfo[i].ulRequestId )
		{
			hErr = PAL_REC_StopPid( ulRecordId, pstCurrInfo[i].ulRequestId );
			if( ERR_OK != hErr )
			{
				HxLOG_Error("PAL_REC_StopPid error : \n");
				continue;
			}

			pstCurrInfo[i].ulRequestId	= REC_PIDREQ_ID_NULL;
			pstCurrInfo[i].usPid		= PID_NULL;
		}
	}

	return hErr;
}

STATIC HERROR svc_rec_FuncStopAllRecordPid( HUINT32 ulRecordId, SvcRec_PIDList_t *pstPIDList )
{
	HERROR	hErr 	=	ERR_OK;
	HUINT32	i		=	0;

	if( NULL == pstPIDList )
		return ERR_FAIL;

	if (eRec_PidSetType_Known == pstPIDList->ePidSetType)
	{
		for( i=0; i<eSvcRec_MAX_REC_NUM; i++ )
		{
			if( (REC_PIDREQ_ID_NULL != pstPIDList->stKnown.stSI[i].ulRequestId) &&
				(PID_NULL 			!= pstPIDList->stKnown.stSI[i].usPid) )
			{
				hErr = PAL_REC_StopPid( ulRecordId, pstPIDList->stKnown.stSI[i].ulRequestId );
				if( hErr != ERR_OK )
				{
					HxLOG_Error ("[%s:%d] SI(%d), ReqestId:0x%X hErr:0x%X\n",
						__FUNCTION__, __LINE__, i, pstPIDList->stKnown.stSI[i].ulRequestId , hErr);
				}

				pstPIDList->stKnown.stSI[i].usPid		= PID_NULL;
				pstPIDList->stKnown.stSI[i].ulRequestId = REC_PIDREQ_ID_NULL;
			}
		}

		//svc_rec_pidtype_si,
		hErr |= svc_rec_FuncStopPid( ulRecordId, svc_rec_pidtype_video,	pstPIDList->stKnown.stVideo);
		hErr |= svc_rec_FuncStopPid( ulRecordId, svc_rec_pidtype_audio, 	pstPIDList->stKnown.stAudio);
		hErr |= svc_rec_FuncStopPid( ulRecordId, svc_rec_pidtype_subtile,	pstPIDList->stKnown.stSubtitle);
		hErr |= svc_rec_FuncStopPid( ulRecordId, svc_rec_pidtype_teletext, pstPIDList->stKnown.stTeletext);
		hErr |= svc_rec_FuncStopPid( ulRecordId, svc_rec_pidtype_caption, 	pstPIDList->stKnown.stCaption);
		hErr |= svc_rec_FuncStopPid( ulRecordId, svc_rec_pidtype_ecm, 		pstPIDList->stKnown.stEcm);
		hErr |= svc_rec_FuncStopPid( ulRecordId, svc_rec_pidtype_emm, 		pstPIDList->stKnown.stEmm);
		hErr |= svc_rec_FuncStopPid( ulRecordId, svc_rec_pidtype_data, 	pstPIDList->stKnown.stData);
	}
	else if (eRec_PidSetType_Unknown == pstPIDList->ePidSetType)
	{
		hErr |= svc_rec_FuncStopPid( ulRecordId, svc_rec_pidtype_manual,	pstPIDList->stUnknown.stPIDs);
	}
	else if (eRec_PidSetType_All == pstPIDList->ePidSetType)
	{
		hErr |= svc_rec_FuncStopPid( ulRecordId, svc_rec_pidtype_all,	pstPIDList->stUnknown.stPIDs);
	}
	else
	{
		hErr = ERR_FAIL;
	}

	return hErr;
}

/* check resource */
STATIC HERROR svc_rec_FuncAllocDelayedInfo(svcRec_BaseData_t *pstData)
{
	if( NULL == pstData )
		return ERR_FAIL;

	if( NULL != pstData->pDelayed )
	{
		/* already started delayrec*/
		return ERR_FAIL;
	}

	/* delayed */
	{
		pstData->pDelayed = (svcRec_Delayed_t*)OxMW_Malloc( sizeof(svcRec_Delayed_t) );
		if( NULL == pstData->pDelayed )
		{
			/* failed alloc memroy*/
			return ERR_FAIL;
		}

		HxSTD_MemSet( pstData->pDelayed, 0, sizeof(svcRec_Delayed_t) );
	}

	/* splitInfo */
	{
		if( NULL != pstData->pDelayed->pstSplitInfo )
		{
			OxMW_Free( pstData->pDelayed->pstSplitInfo );
			pstData->pDelayed->pstSplitInfo = NULL;
		}

		pstData->pDelayed->pstSplitInfo = OxMW_Malloc( sizeof(svcRec_SplitList_t) );
		if( NULL == pstData->pDelayed->pstSplitInfo )
		{
			return ERR_FAIL;
		}

		HxSTD_MemSet( pstData->pDelayed->pstSplitInfo, 0, sizeof(svcRec_SplitList_t) );
	}

	return ERR_OK;
}

STATIC HERROR svc_rec_FuncStartDelayedWithoutSplit( HUINT32 ulRecordId, SvcRec_Setup_t *pstSetupInfo, svcRec_Delayed_t *pDelayed, SvcRec_DelayedRecSetup_t* pSetupDelayed )
{
	HUINT32 hErr 		= ERR_OK;
	HBOOL	bExist 		= FALSE;
	HUINT8 *pucKey 		= NULL, *pucIv = NULL;
	HUINT8	aucSourceName[MAX_FILE_LEN];
	HUINT8	aucTargetName[MAX_FILE_LEN];

	PalRec_DelayedSetup_t stSetup;

	if( (NULL == pstSetupInfo) || (NULL == pSetupDelayed) )
		return ERR_FAIL;

	HxSTD_MemSet( &stSetup, 0, sizeof(PalRec_DelayedSetup_t) );

	stSetup.ulStartTime		=	(HULONG)pSetupDelayed->ulTsrDurationTime;
	stSetup.ulInstanceId	=	pSetupDelayed->ulInstanceId;

	snprintf( stSetup.aucFileName,		PAL_PVR_MAX_FILE_NAME_FULLPATH_LENGTH-1, "%s%s", pSetupDelayed->aucPath, pSetupDelayed->aucFileName);
	snprintf( stSetup.aucTempFileName,	PAL_PVR_MAX_FILE_NAME_FULLPATH_LENGTH-1, "%s%s", pstSetupInfo->unOutArg.stFile.aucPath, REC_DELAYED_TEMP_NAME);

	if( TRUE == pSetupDelayed->bEncryption )
	{
		if( FALSE == pstSetupInfo->bEncryption )
		{
			stSetup.stEncryptSettings.eEncType = (PalCrypt_EncryptionType_e)PAL_SYS_GetPvrEncryptMode();
			pstSetupInfo->bEncryption = TRUE;
		}
		else
		{
			stSetup.stEncryptSettings.eEncType = ePAL_PVR_ENCRYPTION_TYPE_CURRENT;
		}

		if (pSetupDelayed->bUseUserEncKey == TRUE)
		{
			pucKey = pSetupDelayed->aucKey;
		}
		else
		{
			pucKey = PAL_SYS_GetEncryptionKey();
		}
		if( NULL != pucKey )
		{
			HxSTD_MemCopy( stSetup.stEncryptSettings.aucKey, pucKey, PAL_PVR_CRYPTO_KEY_LEN );
		}
		pucIv = PAL_SYS_GetEncryptionIV();
		if( NULL != pucIv )
		{
			HxSTD_MemCopy( stSetup.stEncryptSettings.aucIv, pucIv, PAL_PVR_CRYPTO_IV_LEN );
		}

	}

	pstSetupInfo->ulStartTime		= pSetupDelayed->ulStartTime;
//	pstSetupInfo->ulRecTime			= pSetupDelayed->ulRecTime;
//	pstSetupInfo->ulEndTime			= pSetupDelayed->ulEndTime;
	pstSetupInfo->bEncryption 		= pSetupDelayed->bEncryption;
	pstSetupInfo->bUseUserEncKey	= pSetupDelayed->bUseUserEncKey;
	HxSTD_MemCopy( pstSetupInfo->aucKey, pSetupDelayed->aucKey, PAL_PVR_CRYPTO_KEY_LEN );

	/* move to meta file */
	{
		//HJM
		HLIB_STD_StrUtf8NCpy ( pstSetupInfo->unOutArg.stFile.aucDisplayEvtName, pSetupDelayed->aucDisplayEvtName, MAX_TITLE_LEN-1);
		HxSTD_PrintToStrN ( aucSourceName, MAX_FILE_LEN, "%s%s%s", pstSetupInfo->unOutArg.stFile.aucPath, pstSetupInfo->unOutArg.stFile.aucFileName, EXT_META_FILE);

		bExist = HLIB_FILE_Exist(aucSourceName);
		if( TRUE == bExist )
		{
			HxSTD_PrintToStrN ( aucTargetName, MAX_FILE_LEN, "%s%s%s", pSetupDelayed->aucPath, pSetupDelayed->aucFileName, EXT_META_FILE);
			hErr = HLIB_FILE_Move(aucSourceName, aucTargetName );
			if( ERR_OK != hErr )
			{
				HxLOG_Error ("HLIB_FILE_Move error \n");
				return hErr;
			}
		}

		//HJTSM
		HLIB_STD_StrUtf8NCpy ( pstSetupInfo->unOutArg.stFile.aucDisplayEvtName, pSetupDelayed->aucDisplayEvtName, MAX_TITLE_LEN-1);
		HxSTD_PrintToStrN ( aucSourceName, MAX_FILE_LEN, "%s%s%s", pstSetupInfo->unOutArg.stFile.aucPath, pstSetupInfo->unOutArg.stFile.aucFileName, EXT_JMETA_TSMPFILE);

		bExist = HLIB_FILE_Exist(aucSourceName);
		if( TRUE == bExist )
		{
			HxSTD_PrintToStrN ( aucTargetName, MAX_FILE_LEN, "%s%s%s", pSetupDelayed->aucPath, pSetupDelayed->aucFileName, EXT_JMETA_TSMPFILE);
			hErr = HLIB_FILE_Move(aucSourceName, aucTargetName );
			if( ERR_OK != hErr )
			{
				HxLOG_Error ("HLIB_FILE_Move error \n");
				return hErr;
			}
		}
	}

	HxSTD_MemCopy( pstSetupInfo->unOutArg.stFile.aucPath,		pSetupDelayed->aucPath,		MAX_FILE_LEN );
	HxSTD_MemCopy( pstSetupInfo->unOutArg.stFile.aucFileName,	pSetupDelayed->aucFileName, MAX_FILE_LEN );

	hErr = PAL_REC_StartDelayed((HUINT32)ulRecordId, &stSetup);
	if( hErr != ERR_OK )
	{
		HxLOG_Error ("PAL_REC_StartDelayed error, hErr:0x%X, ulRecordId:%d\n", hErr, ulRecordId);
		return hErr;
	}

	pDelayed->bSplit		= FALSE;
//	pDelayed->ulContentsId	=
	pDelayed->bEncryption 	= pSetupDelayed->bEncryption;
	pDelayed->bUseUserEncKey	= pSetupDelayed->bUseUserEncKey;
	HxSTD_MemCopy( pstSetupInfo->aucKey, pSetupDelayed->aucKey, PAL_PVR_CRYPTO_KEY_LEN );
//	HxSTD_StrNCpy(pDelayed->aucTargetPath, pSetupDelayed->aucPath, MAX_FILE_LEN-1);

	return ERR_OK;
}

STATIC HERROR svc_rec_FuncCopyDelayed( SvcRec_Setup_t * pstSetupInfo, svcRec_Delayed_t * pstDelayed, HBOOL *pbEnd )
{
#if 0
	HUINT32 hErr	=	 ERR_OK;

	svcRec_SplitList_t *pstSplit	= NULL;
	HUINT32				ulCurIndex	= 0;

	PalRec_DelayedSplitSetup_t stSetup;
	HUINT8 *pucKey = NULL, *pucIv = NULL;
	HUINT8	aucTargetName[PAL_PVR_MAX_FILE_NAME_FULLPATH_LENGTH];

	if( (NULL == pstDelayed) || (NULL == pbEnd) )
	{
		HxLOG_Error ("Error.\n");
		return ERR_FAIL;
	}

	pstSplit = pstDelayed->pstSplitInfo;
	if( (pstSplit->ulCurrentNumber == 0) || ((pstSplit->ulCurrentNumber - 1)< pstSplit->ulStartNumber) )
	{
		*pbEnd = TRUE;
		return ERR_OK;
	}

	// set delayed record info
	HxSTD_memset( &stSetup, 0, sizeof(PalRec_DelayedSplitSetup_t) );

	stSetup.ulInstanceId		= pstDelayed->ulInstanceId;
	pstSplit->ulCurrentNumber	-= 1;
	ulCurIndex					= pstSplit->ulCurrentNumber;

	if( ulCurIndex == pstSplit->ulStartNumber)
	{
		stSetup.ulStartTime = (HULONG)pstSplit->ulDelayRecStartTime;
	}
	else
	{
		stSetup.ulStartTime = (HULONG)pstSplit->stSplit[ulCurIndex].ulStartTime;
	}

	stSetup.ulEndTime = pstSplit->stSplit[ulCurIndex].ulEndTime;

	snprintf( stSetup.aucFileName,		PAL_PVR_MAX_FILE_NAME_FULLPATH_LENGTH-1, "%s%s", pstSetupInfo->unOutArg.stFile.aucPath, pstSplit->stSplit[ulCurIndex].aucEventName );
	snprintf( stSetup.aucTempFileName,	PAL_PVR_MAX_FILE_NAME_FULLPATH_LENGTH-1, "%s%s", pstSetupInfo->unOutArg.stFile.aucPath, REC_DELAYED_TEMP_NAME);

	if( pstDelayed->bEncryption == TRUE )
	{
		if( FALSE == pstSetupInfo->bEncryption )
		{
			stSetup.stEncryptSettings.eEncType = SVC_SYS_GetPvrEncryptMode();
			pstSetupInfo->bEncryption = TRUE;
		}
		else
		{
			stSetup.stEncryptSettings.eEncType = ePAL_PVR_ENCRYPTION_TYPE_CURRENT;
			pstSetupInfo->bEncryption = FALSE;
		}

		if (pstDelayed->bUseUserEncKey == TRUE)
		{
			pucKey = pstDelayed->aucKey;
		}
		else
		{
			pucKey = SVC_SYS_GetEncryptionKey();
		}
		if(pucKey != NULL)
		{
			HxSTD_memcpy( stSetup.stEncryptSettings.aucKey, pucKey, PAL_PVR_CRYPTO_KEY_LEN );
		}
		pucIv = SVC_SYS_GetIV();
		if(pucIv != NULL)
		{
			HxSTD_memcpy( stSetup.stEncryptSettings.aucIv, pucIv, PAL_PVR_CRYPTO_IV_LEN );
		}
	}

	pstSetupInfo->ulStartTime	= stSetup.ulStartTime;
	pstSetupInfo->ulRecTime		= pstSplit->ulRecTime;
	pstSetupInfo->ulEndTime		= stSetup.ulEndTime;

	HxSTD_StrNCpy(pstSetupInfo->unOutArg.stFile.aucDisplayEvtName, pstSplit->stSplit[ulCurIndex].aucEventName, MAX_TITLE_LEN-1);

	// Write changed meta info
	HxSTD_memcpy( pstSetupInfo->aucPath,		pstDelayed->aucTargetPath, MAX_FILE_LEN );
	HxSTD_memcpy( pstSetupInfo->aucFileName,	pstSplit->stSplit[ulCurIndex].aucEventName, MAX_FILE_LEN );
	snprintf( aucTargetName, PAL_PVR_MAX_FILE_NAME_FULLPATH_LENGTH-1, "%s%s%s", pstSetupInfo->aucPath, pstSetupInfo->aucFileName, EXT_META_FILE);

	hErr = PAL_REC_StartDelayedSplit( &stSetup );
	if( hErr != ERR_OK )
	{
		HxLOG_Error ("PAL_REC_StartDelayedSplit error, hErr:0x%X\n", hErr);
		return hErr;
	}
#endif
	return ERR_OK;
}

STATIC HERROR svc_rec_FuncLogicDelayedRecChanged(svcRec_BaseData_t *pstData, SvcRec_EvtParam_t *pstEvtParam)
{
	HUINT32 ulPercent	= 0, ulInstanceId	= 0;
//	HULONG	ulTimerId	= 0;
//	HBOOL	bExist		= FALSE;

//	HUINT8	aucFileName[PAL_PVR_MAX_FILE_NAME_FULLPATH_LENGTH];

	if((NULL == pstData) || (NULL == pstEvtParam))
	{
		HxLOG_Error ("Data Pointer is NULL \n");
		return ERR_FAIL;
	}

	ulInstanceId	= pstEvtParam->ulParam2;
	ulPercent		= pstEvtParam->ulParam1;

	svc_rec_PostMsgToAp(eSEVT_REC_DELAYED_RECORD_STATUS_CHANGE, pstData->hAction, ulPercent, ulInstanceId, 0);

	return ERR_OK;
}

STATIC HERROR svc_rec_FuncStartDelayedWithSplit( HUINT32 ulRecordId, SvcRec_Setup_t *pstSetupInfo, svcRec_Delayed_t *pDelayed, SvcRec_DelayedRecSetup_t* pSetupDelayed )
{
	HERROR	hErr;
	HUINT8	aucTargetName[PAL_PVR_MAX_FILE_NAME_FULLPATH_LENGTH];
	HUINT8	aucSourceName[PAL_PVR_MAX_FILE_NAME_FULLPATH_LENGTH];

	HUINT32 i, ulTotalNumber=0, ulCurrentRecTime=0, ulCurrentStartTime=0, ulStartIndex=0;

	if (NULL == pDelayed)
		return ERR_FAIL;

	/* fix for prevent #149064 */
	if( NULL == pDelayed->pstSplitInfo )
		return ERR_FAIL;

	//***********   Process Delayed Record in case of Split  ***************
	ulTotalNumber = pDelayed->pstSplitInfo->ulTotalNumber;

	// Update Current Record Endtime to split info
	hErr = PAL_REC_GetDuration((HUINT32)ulRecordId, &ulCurrentRecTime );
	if( hErr != ERR_OK )
	{
		HxLOG_Error ("PAL_REC_GetDuration, uRecordId:0x%X, hErr:0x%X\n", ulRecordId, hErr);
		return hErr;
	}

	pDelayed->pstSplitInfo->stSplit[ulTotalNumber-1].ulEndTime = ulCurrentRecTime;

	/* 사용자가 선택한 이벤트 찾기 */
	{
		HBOOL bFound = FALSE;
		ulCurrentStartTime = (HULONG)pSetupDelayed->ulTsrDurationTime;

		for( i=0; i<pDelayed->pstSplitInfo->ulTotalNumber; i++ )
		{
			if( (pDelayed->pstSplitInfo->stSplit[i].ulStartTime <= ulCurrentStartTime) &&
				(ulCurrentStartTime <= pDelayed->pstSplitInfo->stSplit[i].ulEndTime) )
			{
				bFound = TRUE;
				break;
			}
		}

		pDelayed->pstSplitInfo->ulStartNumber = i;
		pDelayed->pstSplitInfo->ulDelayRecStartTime = ulCurrentStartTime;

		if( ( (i+1) >= pDelayed->pstSplitInfo->ulTotalNumber) || (bFound == FALSE) )
		{
			hErr =  svc_rec_FuncStartDelayedWithoutSplit( ulRecordId, pstSetupInfo, pDelayed, pSetupDelayed );
			if( ERR_OK != hErr )
				pDelayed->bUse = FALSE;

			return hErr;
		}
	}

	if( NULL == pDelayed->pstSplitInfo )
		return ERR_FAIL;

	ulStartIndex = pDelayed->pstSplitInfo->ulTotalNumber - 1;
	pDelayed->pstSplitInfo->ulCurrentNumber	= ulStartIndex;
	pDelayed->pstSplitInfo->ulRecTime		= pSetupDelayed->ulRecTime;

	snprintf(aucSourceName, PAL_PVR_MAX_FILE_NAME_FULLPATH_LENGTH-1, "%s%s", pstSetupInfo->unOutArg.stFile.aucPath, pstSetupInfo->unOutArg.stFile.aucFileName);
	snprintf(aucTargetName, PAL_PVR_MAX_FILE_NAME_FULLPATH_LENGTH-1, "%s%s", pSetupDelayed->aucPath, pDelayed->pstSplitInfo->stSplit[ulStartIndex].aucEventName);

	/* set delayed record info */
	{
		HBOOL	bExist = FALSE;
		PalRec_DelayedSetup_t stSetup;
		HUINT8 *pucKey = NULL , *pucIv = NULL;

		pstSetupInfo->ulStartTime		=	pDelayed->pstSplitInfo->stSplit[ulStartIndex].ulStartTime;
//		pstSetupInfo->ulEndTime			=	pDelayed->pstSplitInfo->stSplit[ulStartIndex].ulEndTime;
//		pstSetupInfo->ulRecTime			=	pSetupDelayed->ulRecTime;

		HxSTD_MemSet( &stSetup, 0, sizeof(PalRec_DelayedSetup_t) );

		stSetup.ulStartTime		= (HULONG)pstSetupInfo->ulStartTime;
		stSetup.ulInstanceId	= pSetupDelayed->ulInstanceId;
		snprintf( stSetup.aucFileName, 		PAL_PVR_MAX_FILE_NAME_FULLPATH_LENGTH-1, "%s", aucTargetName);
		snprintf( stSetup.aucTempFileName,	PAL_PVR_MAX_FILE_NAME_FULLPATH_LENGTH-1, "%s%s", pstSetupInfo->unOutArg.stFile.aucPath, REC_DELAYED_TEMP_NAME);

		if( TRUE == pSetupDelayed->bEncryption )
		{
			if( 0 == pstSetupInfo->bEncryption )
			{
				stSetup.stEncryptSettings.eEncType = (PalCrypt_EncryptionType_e)PAL_SYS_GetPvrEncryptMode();
				pstSetupInfo->bEncryption = TRUE;
			}
			else
			{
				stSetup.stEncryptSettings.eEncType = ePAL_PVR_ENCRYPTION_TYPE_CURRENT;
			}

			if (pSetupDelayed->bUseUserEncKey == TRUE)
			{
				pucKey = pSetupDelayed->aucKey;
			}
			else
			{
				pucKey = PAL_SYS_GetEncryptionKey();
			}
			if(pucKey != NULL)
			{
				HxSTD_MemCopy( stSetup.stEncryptSettings.aucKey, pucKey, PAL_PVR_CRYPTO_KEY_LEN );
			}
			pucIv = PAL_SYS_GetEncryptionIV();
			if(pucIv != NULL)
			{
				HxSTD_MemCopy( stSetup.stEncryptSettings.aucIv, pucIv, PAL_PVR_CRYPTO_IV_LEN );
			}
		}

		HxSTD_StrNCpy( pstSetupInfo->unOutArg.stFile.aucDisplayEvtName, pDelayed->pstSplitInfo->stSplit[ulStartIndex].aucEventName, MAX_TITLE_LEN-1);

		// Rename meta file
		HxSTD_StrNCat(aucSourceName, EXT_META_FILE, 4);

		bExist = HLIB_FILE_Exist(aucSourceName);
		if( TRUE == bExist )
		{
			HxSTD_StrNCat(aucTargetName, EXT_META_FILE, 4);

			hErr = HLIB_FILE_Move(aucSourceName, aucTargetName );
			if(hErr != ERR_OK)
				return hErr;
		}

		HxSTD_MemCopy( pstSetupInfo->unOutArg.stFile.aucPath,		pSetupDelayed->aucPath, MAX_FILE_LEN );
		HxSTD_MemCopy( pstSetupInfo->unOutArg.stFile.aucFileName,	pDelayed->pstSplitInfo->stSplit[ulStartIndex].aucEventName, MAX_FILE_LEN );

		hErr = PAL_REC_StartDelayed((HUINT32)ulRecordId, &stSetup );
		if( ERR_OK != hErr )
		{
			HxLOG_Error ("PAL_REC_StartDelayed error, hErr:0x%X, ulRecordId:%d\n", hErr, ulRecordId);
			return hErr;
		}
	}

	pDelayed->bSplit		= TRUE;
//	pDelayed->ulContentsId	=
	pDelayed->bEncryption 	= pSetupDelayed->bEncryption;
	pDelayed->bUseUserEncKey	= pSetupDelayed->bUseUserEncKey;
	HxSTD_MemCopy( pDelayed->aucKey, pSetupDelayed->aucKey, PAL_PVR_CRYPTO_KEY_LEN );
//	HxSTD_StrNCpy(pDelayed->aucTargetPath, pstSetupInfo->unOutArg.stFile.aucPath, MAX_FILE_LEN-1);

	return ERR_OK;
}

#define ___MEMBER_FUNCTIONS___
HERROR svc_rec_FuncSetPostMsgCallback (PostMsgToMgrCb_t fnPostMsg)
{
	s_fnSvcRec_PostMsgToAp = fnPostMsg;

	return ERR_OK;
}

HERROR svc_rec_FuncSetup (svcRec_BaseData_t *pstData, SvcRec_Msg_t *pstMsg)
{
	HERROR	hErr		= ERR_OK;

	HBOOL	bReTrySetup	= FALSE;

	HUINT32	ulRecordId	= RxRSCID_NULL;
	HUINT32	ulDemuxId	= RxRSCID_NULL;
	HUINT32	ulActionId	= 0;

	SvcRec_ActType_e	eRecActType;
	SvcRec_Setup_t		stSetupInfo;

	ENTER_FUNCTION;

	if( (NULL == pstData) || (NULL == pstMsg) )
		return ERR_FAIL;

	hErr = svc_rec_GetMsgArg( pstMsg, eRECMSG_ARGTYPE_Setup, &stSetupInfo );
	if( ERR_OK != hErr )
		return hErr;
	{
		if (pstData->eState >= eREC_STATE_Setup)
		{
			/*
 			 * Setup 을 다시할 필요 조건을 이곳에 추가함.
			 */
			if (	(pstData->stSetupInfo.eSvcType		!= stSetupInfo.eSvcType) ||
					(pstData->stSetupInfo.eVideoCodec	!= stSetupInfo.eVideoCodec)	)
					bReTrySetup = TRUE;

		}

		/*
		 *  REC을 위한 Resource 해제 작업.
		 */
		if (TRUE == bReTrySetup)
		{
			hErr = PAL_REC_Stop((HUINT32)ulRecordId);
			if( hErr != ERR_OK )
			{
				HxLOG_Error ("PAL_REC_Stop, hAction:0x%X, hErr:0x%X\n", pstMsg->hAction, hErr);
				return hErr;
			}
		}

		HxSTD_memcpy (&(pstData->stSetupInfo), &stSetupInfo, sizeof(SvcRec_Setup_t));
	}

	eRecActType = pstData->stSetupInfo.eRecActType;

	/* get resource */
	hErr = svc_rec_FuncGetResourceIds( pstMsg->hAction, pstData->stSetupInfo.hPBAction, eRecActType, &ulActionId, &ulDemuxId, &ulRecordId);
	if( ERR_OK != hErr )
		return hErr;

#if defined(CONFIG_OP_SES)
	/* REC Setup이 두번 연속 호출되면  tsr이 HDD Format 동작시 삭제가 안되 unmount 현항으로 인해 format이 안되는 현상 발생 */
	if(eREC_STATE_Setup == pstData->eState)
	{
		hErr = PAL_REC_Stop((HUINT32)ulRecordId);
		if(hErr != ERR_OK)
		{
			HxLOG_Error("PAL_REC_Stop, hAction:0x%x, hErr:0x%x\n", pstMsg->hAction, hErr);
		}
	}
#endif

	/* save resource */
	pstData->hAction		=	pstMsg->hAction;
	pstData->ulDemuxId		=	ulDemuxId;
	pstData->ulRecordId		=	ulRecordId;

	switch(eRecActType)
	{
		case eRec_ActType_Satip:
		case eRec_ActType_Ntls:
					PAL_REC_RegisterMemoryCallback (ulRecordId, svc_rec_main_MemIOCallback_t);
					break;
		default:	PAL_REC_RegisterNotifier (ulRecordId, svc_rec_main_Callback_t);
					break;
	}

 	if (eREC_STATE_Init == pstData->eState)
	{
		SvcRec_PidSetType_e		 ePidSetType = eRec_PidSetType_Known;

		if (eRec_ActType_Ntls == eRecActType)
		{
			ePidSetType = eRec_PidSetType_Unknown;
		}
		else if (eRec_ActType_Satip == eRecActType)
		{
			ePidSetType = eRec_PidSetType_Unknown;
		}
		else if (eRec_ActType_FullTs == eRecActType)
		{
			ePidSetType = eRec_PidSetType_Unknown;
		}

		pstData->ulPATReplaceHandle	=	REC_PIDREQ_ID_NULL;
		svc_rec_FuncInitPid( &(pstData->stPIDList), ePidSetType);
		HxLOG_Debug("pstData->eState(%d) is eREC_STATE_Init, call svc_rec_FuncInitPid()\n", pstData->eState);
	}
	else
	{
		HxLOG_Debug("pstData->eState(%d)\n", pstData->eState);
	}

	/* action */
	{
		/* set path */
		PAL_PIPE_SetOutputResource (ulActionId, ePAL_PIPE_OUTPUT_RECORDER, (HUINT32)ulRecordId);

		if( eRec_ActType_Descramble_Record == eRecActType )
		{
			PAL_PIPE_SetDemux (ulActionId, ulDemuxId);
			PAL_PIPE_SetInputResource (ulActionId, ePAL_PIPE_INPUT_SCRAMBLED_PLAYER, ulRecordId);
		}

		/* TSR */
		if( eRec_ActType_Tsr_Record == eRecActType )
		{
			svc_rec_FuncAllocDelayedInfo(pstData);
		}

		/* setup */
		hErr = svc_rec_FuncSetupRecToPAL(	ulDemuxId, ulRecordId,
											pstData->stSetupInfo.eRecActType,
											pstData->stSetupInfo.eRecOutType,
											pstData->stSetupInfo.eSvcType,
											pstData->stSetupInfo.eVideoCodec,
											pstData->stSetupInfo.unOutArg.stFile.aucPath,
											pstData->stSetupInfo.unOutArg.stFile.aucFileName,
											pstData->stSetupInfo.stUsbFlash.bIsUsbFlashRecord,
											pstData->stSetupInfo.stUsbFlash.ulLimitTimeSecond,
											pstData->stSetupInfo.stUsbFlash.ullLimitDataSize,
											pstData->stSetupInfo.stUsbFlash.ullLimitIndexSize);

		if( ERR_OK != hErr )
			return hErr;

	}


	/* finish setup */
	pstData->eState = eREC_STATE_Setup;
	LEAVE_FUNCTION;

	return hErr;
}

HERROR svc_rec_FuncUpdateVideoCodec (svcRec_BaseData_t *pstData, SvcRec_Msg_t *pstMsg)
{
	HERROR	hErr		= ERR_OK;

	HUINT32	ulRecordId	= RxRSCID_NULL;
	HUINT32	ulDemuxId	= RxRSCID_NULL;
	HUINT32	ulActionId	= 0;

	SvcRec_ActType_e	eRecActType;

	ENTER_FUNCTION;

	if( (NULL == pstData) || (NULL == pstMsg) )
		return ERR_FAIL;

	hErr = svc_rec_GetMsgArg( pstMsg, eRECMSG_ARGTYPE_UpdateVideoCodec, &(pstData->stSetupInfo) );
	if( ERR_OK != hErr )
		return hErr;

	eRecActType = pstData->stSetupInfo.eRecActType;

	/* get resource */
	hErr = svc_rec_FuncGetResourceIds( pstMsg->hAction, pstData->stSetupInfo.hPBAction, eRecActType, &ulActionId, &ulDemuxId, &ulRecordId);
	if( ERR_OK != hErr )
		return hErr;

#if 1 // Workaround : DI_PVR_REC_Setup 중복 호출이 가능하면 아래 코드 삭제 가능
	if (eREC_STATE_Setup == pstData->eState)
	{
		hErr = PAL_REC_Stop((HUINT32)ulRecordId);
		if( hErr != ERR_OK )
		{
			HxLOG_Error ("PAL_REC_Stop, hAction:0x%X, hErr:0x%X\n", pstMsg->hAction, hErr);
			return hErr;
		}
	}
#endif

	/* save resource */
	pstData->hAction		=	pstMsg->hAction;
	pstData->ulDemuxId		=	ulDemuxId;
	pstData->ulRecordId		=	ulRecordId;

 	if (eREC_STATE_Init == pstData->eState)
	{
		SvcRec_PidSetType_e		 ePidSetType = eRec_PidSetType_Known;

		if ((eRec_ActType_Satip == eRecActType)	|| (eRec_ActType_Ntls == eRecActType) || (eRec_ActType_FullTs == eRecActType))
		{
			ePidSetType = eRec_PidSetType_Unknown;
		}
		else
		{
			ePidSetType = eRec_PidSetType_Known;
		}

		NOT_USED_PARAM(ePidSetType);
	}
	else
	{
		HxLOG_Debug("pstData->eState(%d)\n", pstData->eState);
	}

	/* action */
	{
		/* set path */
		PAL_PIPE_SetOutputResource (ulActionId, ePAL_PIPE_OUTPUT_RECORDER, (HUINT32)ulRecordId);

		if( eRec_ActType_Descramble_Record == eRecActType )
		{
			PAL_PIPE_SetDemux (ulActionId, ulDemuxId);
			PAL_PIPE_SetInputResource (ulActionId, ePAL_PIPE_INPUT_SCRAMBLED_PLAYER, ulRecordId);
		}

		/* TSR */
		if( eRec_ActType_Tsr_Record == eRecActType )
		{
			svc_rec_FuncAllocDelayedInfo(pstData);
		}

		/* setup */
		hErr = svc_rec_FuncSetupRecToPAL(	ulDemuxId, ulRecordId,
											pstData->stSetupInfo.eRecActType,
											pstData->stSetupInfo.eRecOutType,
											pstData->stSetupInfo.eSvcType,
											pstData->stSetupInfo.eVideoCodec,
											pstData->stSetupInfo.unOutArg.stFile.aucPath,
											pstData->stSetupInfo.unOutArg.stFile.aucFileName,
											pstData->stSetupInfo.stUsbFlash.bIsUsbFlashRecord,
											pstData->stSetupInfo.stUsbFlash.ulLimitTimeSecond,
											pstData->stSetupInfo.stUsbFlash.ullLimitDataSize,
											pstData->stSetupInfo.stUsbFlash.ullLimitIndexSize);

		if( ERR_OK != hErr )
			return hErr;

	}


	/* finish setup */
	pstData->eState = eREC_STATE_Setup;
	LEAVE_FUNCTION;

	return hErr;
}

HERROR svc_rec_FuncSetPid (svcRec_BaseData_t *pstData, SvcRec_Msg_t *pstMsg)
{
	HERROR 				hErr		=	ERR_FAIL;
	HBOOL 				bChanged	=	FALSE;
	SvcRec_PIDList_t	stPIDList;

	Handle_t			hAction		=	HANDLE_NULL;
	HUINT32				ulRecordId	=	RxRSCID_NULL;

	ENTER_FUNCTION;

	if( (NULL == pstData) || (NULL == pstMsg) )
		return ERR_FAIL;

	hAction		= pstData->hAction;
	ulRecordId	= pstData->ulRecordId;

	hErr = svc_rec_GetMsgArg( pstMsg, eRECMSG_ARGTYPE_SetPid, &(stPIDList) );
	if( ERR_OK != hErr )
		return hErr;

	_svc_rec_DebugPrintRecPidInfo(&stPIDList, "to be set : stPIDList", __LINE__);

	_svc_rec_DebugPrintRecPidInfo(&pstData->stPIDList, "before update: pstData->stPIDList", __LINE__);

	hErr = svc_rec_FuncUpdateRecordPid(ulRecordId,
										&(pstData->stPIDList),
										&stPIDList,
										&bChanged);

	_svc_rec_DebugPrintRecPidInfo(&pstData->stPIDList, "after update : pstData->stPIDList", __LINE__);

	if( (ERR_OK == hErr) && (TRUE == bChanged) )
	{
		if (eREC_STATE_Setup < pstData->eState)
		{
			HUINT32 ulRecTime = 0;
			hErr = PAL_REC_GetDuration(ulRecordId, &ulRecTime );
			if( ERR_OK == hErr)
			{
				svc_rec_PostMsgToAp(eSEVT_REC_PID_CHANGED, hAction, ulRecTime, 0, 0);
			}
		}
	}

	return hErr;
}

HERROR svc_rec_FuncStart (svcRec_BaseData_t *pstData, SvcRec_Msg_t *pstMsg)
{
	HERROR 	hErr		=	ERR_FAIL;
	HUINT32	ulRecordId	=	RxRSCID_NULL;

	ENTER_FUNCTION;

	if(pstData == NULL)
	{
		HxLOG_Error ("Data Pointer is NULL \n");
		return ERR_FAIL;
	}

	if( eREC_STATE_Setup != pstData->eState )
		return ERR_FAIL;

	ulRecordId	= pstData->ulRecordId;

	hErr = PAL_REC_Start((HUINT32)ulRecordId);
	if( hErr != ERR_OK )
	{
		HxLOG_Error ("PAL_REC_Start, ulRecordId:0x%X, hErr:0x%X\n", ulRecordId, hErr);
		return ERR_FAIL;
	}

	/*finish*/
	pstData->eState = eREC_STATE_Start;

	return hErr;
}

HERROR svc_rec_FuncPause (svcRec_BaseData_t *pstData, SvcRec_Msg_t *pstMsg)
{
	HERROR 		hErr		=	ERR_FAIL;
	HUINT32		ulRecordId	=	RxRSCID_NULL;
	Handle_t	hAction		=	HANDLE_NULL;

	ENTER_FUNCTION;

	if(pstData == NULL)
	{
		HxLOG_Error ("Data Pointer is NULL \n");
		return ERR_FAIL;
	}

	if( eREC_STATE_Pause == pstData->eState )
		return ERR_FAIL;

	ulRecordId	=	pstData->ulRecordId;
	hAction		=	pstData->hAction;

	hErr = PAL_REC_Pause((HUINT32)ulRecordId);
	if( hErr != ERR_OK )
	{
		HxLOG_Error ("PAL_REC_Pause, hAction:0x%X, hErr:0x%X\n", hAction, hErr);
		return hErr;
	}

	/* finish */
	pstData->eState = eREC_STATE_Pause;

	return hErr;
}

HERROR svc_rec_FuncResume (svcRec_BaseData_t *pstData, SvcRec_Msg_t *pstMsg)
{
	HERROR 		hErr		=	ERR_FAIL;
	HUINT32		ulRecordId	=	RxRSCID_NULL;
	Handle_t	hAction		=	HANDLE_NULL;

	ENTER_FUNCTION;

	if(pstData == NULL)
	{
		HxLOG_Error ("Data Pointer is NULL \n");
		return ERR_FAIL;
	}

	if( eREC_STATE_Pause != pstData->eState )
		return ERR_FAIL;

	ulRecordId = pstData->ulRecordId;

	hErr = PAL_REC_Resume((HUINT32)ulRecordId);
	if( hErr != ERR_OK )
	{
		HxLOG_Error ("PAL_REC_Resume, hAction:0x%X, hErr:0x%X\n", hAction, hErr);
		return ERR_FAIL;
	}

	 /* finish */
	 pstData->eState = eREC_STATE_Start;

	return hErr;
}

HERROR svc_rec_FuncStartDelayed(svcRec_BaseData_t *pstData, SvcRec_Msg_t *pstMsg)
{
	HERROR 				hErr		=	ERR_FAIL;
	SvcRec_DelayedRecSetup_t	stDelayedSetup;

	HUINT32				ulRecordId	=	RxRSCID_NULL;

	ENTER_FUNCTION;

	if( (NULL == pstData) || (NULL == pstMsg) )
	{
		if(pstData) svc_rec_PostMsgToAp(eSEVT_REC_DELAYED_RECORD_STATUS_CHANGE, pstData->hAction, 200/*ERROR*/, 0, 0);
		return ERR_FAIL;
	}

	if( eREC_STATE_Start != pstData->eState )
	{
		svc_rec_PostMsgToAp(eSEVT_REC_DELAYED_RECORD_STATUS_CHANGE, pstData->hAction, 200/*ERROR*/, 0, 0);
		return ERR_FAIL;
	}

	ulRecordId	= pstData->ulRecordId;

	hErr = svc_rec_GetMsgArg( pstMsg, eRECMSG_ARGTYPE_SetupDelayed, &(stDelayedSetup) );
	if( ERR_OK != hErr )
	{
		svc_rec_PostMsgToAp(eSEVT_REC_DELAYED_RECORD_STATUS_CHANGE, pstData->hAction, 200/*ERROR*/, 0, 0);
		return hErr;
	}

	if( (NULL	== pstData->pDelayed->pstSplitInfo) ||
		(FALSE	== pstData->pDelayed->bSplit) ||
		(0		== pstData->pDelayed->pstSplitInfo->ulTotalNumber) )
	{
		hErr = svc_rec_FuncStartDelayedWithoutSplit( ulRecordId, &(pstData->stSetupInfo), pstData->pDelayed, &(stDelayedSetup) );
	}
	else
	{
		hErr = svc_rec_FuncStartDelayedWithSplit( ulRecordId, &(pstData->stSetupInfo), pstData->pDelayed, &(stDelayedSetup) );
	}

	/* finish */
	if( ERR_OK == hErr )
	{
		pstData->stSetupInfo.eRecActType = eRec_ActType_Delayed_Record;

		pstData->pDelayed->bUse			= TRUE;
		pstData->pDelayed->ulInstanceId = stDelayedSetup.ulInstanceId;


		//snprintf( pstData->pDelayed->aucTempPath,		MAX_FILE_LEN-1, "%s", pstData->stSetupInfo.aucPath );
		//snprintf( pstData->pDelayed->aucTempFileName,	MAX_FILE_LEN-1, "%s", REC_DELAYED_TEMP_NAME );
	}
	else
	{
		svc_rec_PostMsgToAp(eSEVT_REC_DELAYED_RECORD_STATUS_CHANGE, pstData->hAction, 200/*ERROR*/, 0, 0);
	}

	return ERR_OK;
}

HERROR svc_rec_FuncStop (svcRec_BaseData_t *pstData, SvcRec_Msg_t *pstMsg)
{
	HERROR 		hErr		=	ERR_FAIL;
	HUINT32		ulRecordId	=	RxRSCID_NULL;
	Handle_t	hAction		=	HANDLE_NULL;

	ENTER_FUNCTION;

	if(pstData == NULL)
	{
		HxLOG_Error ("Data Pointer is NULL \n");
		return ERR_FAIL;
	}

	ulRecordId	=	pstData->ulRecordId;
	hAction		=	pstData->hAction;

	hErr = svc_rec_FuncStopAllRecordPid( ulRecordId, &(pstData->stPIDList) );
	if( hErr != ERR_OK )
	{
		HxLOG_Error ("svc_rec_FuncStopAllRecordPid, hAction:0x%X, hErr:0x%X\n", hAction, hErr);
	}

	/* off replace PAT */
	{
		if( REC_PIDREQ_ID_NULL != pstData->ulPATReplaceHandle )
		{
			hErr = PAL_REC_ReplacePacketModeOff( pstData->ulPATReplaceHandle );
			if( hErr != ERR_OK )
			{
				HxLOG_Error ("PAL_REC_ReplacePacketModeOff, ulReplaceHandle:0x%X\n", pstData->ulPATReplaceHandle);
			}
			pstData->ulPATReplaceHandle = REC_PIDREQ_ID_NULL;
		}
	}

	hErr = PAL_REC_Stop((HUINT32)ulRecordId);
	if( hErr != ERR_OK )
	{
		HxLOG_Error ("PAL_REC_Stop, hAction:0x%X, hErr:0x%X\n", hAction, hErr);
		return hErr;
	}

	/*finish*/
	pstData->eState = eREC_STATE_Stop;

	return hErr;
}

HERROR svc_rec_FuncClose (svcRec_BaseData_t *pstData, SvcRec_Msg_t *pstMsg)
{
	ENTER_FUNCTION;

	/* unreg */
	{
		HERROR	hErr;
		HUINT32			ulActionId;
		HUINT32			ulRecordId;

		ulActionId = PAL_PIPE_GetActionId( pstMsg->hAction );
		hErr = PAL_PIPE_GetResourceId( ulActionId, eRxRSCTYPE_HDD_REC, &ulRecordId);
		if (ERR_OK != hErr)
			return hErr;

		PAL_REC_UnregisterNotifier (ulRecordId, svc_rec_main_Callback_t);
		PAL_REC_UnregisterMemoryCallback (ulRecordId, svc_rec_main_MemIOCallback_t);
	}

	/* free resources */
	if( NULL != pstData )
	{
		if( NULL != pstData->pDelayed )
		{
			if( NULL != pstData->pDelayed->pstSplitInfo )
			{
				OxMW_Free( pstData->pDelayed->pstSplitInfo );
				pstData->pDelayed->pstSplitInfo = NULL;
			}

			OxMW_Free(pstData->pDelayed);
			pstData->pDelayed = NULL;
		}
	}

	/* N/A */
	return ERR_OK;
}

HERROR svc_rec_FuncEvtRecord (svcRec_BaseData_t *pstData, SvcRec_Msg_t *pstMsg)
{
	HERROR	hErr	=	ERR_OK;
	SvcRec_EvtParam_t	stEvtParam;

	if(pstData == NULL)
	{
		HxLOG_Error ("Data Pointer is NULL \n");
		return ERR_FAIL;
	}

	hErr = svc_rec_GetMsgArg( pstMsg, eRECMSG_ARGTYPE_RecEvtParam, &(stEvtParam) );
	if( ERR_OK != hErr )
		return ERR_FAIL;

	switch( stEvtParam.eType )
	{
		case eRECEVT_TIMECODE:
		{
			pstData->ulStartTime	=	stEvtParam.ulParam3;
			pstData->ulRecTime		=	stEvtParam.ulParam1;
			pstData->ulEndTime		=	stEvtParam.ulParam1;
			//stEvtParam.ulParam3; //Percent

			HxLOG_Info("EVT_TIMECODE_CHANGE : ulStartTime[%d] ulRecTime[%d] ulEndTime[%d]\n", pstData->ulStartTime, pstData->ulRecTime, pstData->ulEndTime);
			svc_rec_PostMsgToAp(eSEVT_REC_TIMECODE_CHANGE, pstData->hAction, stEvtParam.ulParam1,
																			stEvtParam.ulParam3,
																			stEvtParam.ulParam2);
			break;
		}

		case eRECEVT_UNSCRAMBLED:
			svc_rec_PostMsgToAp(eSEVT_REC_SCRAMBLED, pstData->hAction, FALSE,0, 0);
			break;
		case eRECEVT_SCRAMBLED:
			svc_rec_PostMsgToAp(eSEVT_REC_SCRAMBLED, pstData->hAction, TRUE,0, 0);
			break;

		case eRECEVT_DELAYEDREC_CHANGED:
		{
			svc_rec_FuncLogicDelayedRecChanged( pstData, &stEvtParam );
			break;
		}

		case eRECEVT_DISK_FULL:
		default :
			HxLOG_Error("[%s:%d] not support type(%d)\n", stEvtParam.eType );
			return ERR_FAIL;

	}

	return ERR_OK;
}

void svc_rec_FuncInitPid( SvcRec_PIDList_t *pstPIDList, SvcRec_PidSetType_e ePidSetType)
{
	ENTER_FUNCTION;

	if( pstPIDList == NULL )
		return;

	HxSTD_MemSet(pstPIDList, 0, sizeof(SvcRec_PIDList_t));

	if (eRec_PidSetType_Known == ePidSetType)
	{
		svc_rec_FuncInitPidDes(pstPIDList->stKnown.stSI, eSvcRec_MAX_REC_NUM);
		svc_rec_FuncInitPidDes(pstPIDList->stKnown.stVideo, SvcRec_MAX_VIDEO_ES_NUM);
		svc_rec_FuncInitPidDes(pstPIDList->stKnown.stAudio, SvcRec_MAX_AUDIO_ES_NUM);
		svc_rec_FuncInitPidDes(pstPIDList->stKnown.stSubtitle, SvcRec_MAX_SUBTTL_ES_NUM);
		svc_rec_FuncInitPidDes(pstPIDList->stKnown.stTeletext, SvcRec_MAX_TELTXT_ES_NUM);
		svc_rec_FuncInitPidDes(pstPIDList->stKnown.stCaption, SvcRec_MAX_CAPTION_ES_NUM);
		svc_rec_FuncInitPidDes(pstPIDList->stKnown.stEcm, SvcRec_MAX_CA_PID_NUM);
		svc_rec_FuncInitPidDes(pstPIDList->stKnown.stEmm, SvcRec_MAX_CA_PID_NUM);
		svc_rec_FuncInitPidDes(pstPIDList->stKnown.stData, SvcRec_MAX_DATA_ES_NUM);
	}
	else
	{
		svc_rec_FuncInitPidDes(pstPIDList->stUnknown.stPIDs, DxRSV_PIDTABLE_COUNT);
	}

	pstPIDList->ePidSetType = ePidSetType;

	return ;
}



HERROR svc_rec_FuncReplacePSI( void *pvData, HUINT16 usPid, HUINT8 *pucSection, HUINT32 ulSectionSize )
{
	HERROR hErr			=	ERR_FAIL;
	svcRec_BaseData_t	*pstData = (svcRec_BaseData_t*)pvData;
	HUINT32 ulReplaceHandle;

	ENTER_FUNCTION;

	if(pstData == NULL)
	{
		HxLOG_Error ("Data Pointer is NULL \n");
		return ERR_FAIL;
	}

	if (REC_PIDREQ_ID_NULL != pstData->ulPATReplaceHandle)
	{
		HxLOG_Error("already replace PAT!!!\n");
		return ERR_FAIL;
	}

	switch(usPid)
	{
		case PAT_PID :
			break;
		default:
			HxLOG_Error("not support pid(0x%x)\n", usPid);
			return ERR_FAIL;

	}


	hErr = PAL_REC_ReplacePacketModeOn(	pstData->ulDemuxId,
												usPid,
												usPid,
												pucSection,
												ulSectionSize,
												&ulReplaceHandle );

	pstData->ulPATReplaceHandle = ulReplaceHandle;

	return hErr;
}

HERROR svc_rec_FuncGetInfo( void *pvData, SvcRec_RecInfo *pstRecInfo )
{
	HERROR hErr			=	ERR_OK;
	svcRec_BaseData_t	*pstData = (svcRec_BaseData_t*)pvData;


	if((pstData == NULL) || (pstRecInfo == NULL))
	{
		return ERR_FAIL;
	}

	HxSTD_MemSet (pstRecInfo, 0, sizeof(SvcRec_RecInfo));

	if ((eREC_STATE_Start == pstData->eState) || (eREC_STATE_StartDelayed== pstData->eState))
	{
		pstRecInfo->ulStartTime	=	pstData->ulStartTime;
		pstRecInfo->ulRecTime	=	pstData->ulRecTime;
		pstRecInfo->ulEndTime	=	pstData->ulEndTime;

		hErr = PAL_REC_GetDuration((HUINT32)pstData->ulRecordId, &pstRecInfo->ulDuration );
	}

	return hErr;
}

HERROR svc_rec_FuncSetDRMInfo( void *pvData, DxCopyrightControl_t *pstInfo, HBOOL bWriteTimeStamp )
{
	return ERR_OK;
}

HBOOL svc_rec_FuncIsRecordingWithName( void *pvData, HUINT8 *pucFileName)
{
	svcRec_BaseData_t	*pstData = (svcRec_BaseData_t*)pvData;
	HUINT8 aucFileName[MAX_FILE_LEN];

	if((pstData == NULL) || (pucFileName == NULL))
	{
		HxLOG_Error ("Data Pointer is NULL \n");
		return FALSE;
	}

	HxSTD_memset( aucFileName, 0, MAX_FILE_LEN );

	HxSTD_StrNCpy(aucFileName, pstData->stSetupInfo.unOutArg.stFile.aucPath, PAL_PVR_MAX_FILE_NAME_FULLPATH_LENGTH-1);
	HxSTD_StrNCat(aucFileName, pstData->stSetupInfo.unOutArg.stFile.aucFileName, PAL_PVR_MAX_FILE_NAME_FULLPATH_LENGTH-1);
	HxSTD_StrNCat(aucFileName, EXT_TS_FILE, PAL_PVR_MAX_FILE_NAME_FULLPATH_LENGTH-1);

	if( HxSTD_StrNCmp( aucFileName, pucFileName, HxSTD_StrLen(pucFileName) ) == 0 )
	{
		return TRUE;
	}

	return FALSE;
}


HERROR svc_rec_FuncSetEncryption (svcRec_BaseData_t *pstData, SvcRec_Msg_t *pstMsg)
{
	HERROR	hErr		= ERR_OK;

	ENTER_FUNCTION;

	if( (NULL == pstData) || (NULL == pstMsg) )
		return ERR_FAIL;

	hErr = svc_rec_GetMsgArg( pstMsg, eRECMSG_ARGTYPE_SetEncryption, &(pstData->stSetupInfo) );
	if( ERR_OK != hErr )
		return hErr;

	LEAVE_FUNCTION;

	return hErr;
}


