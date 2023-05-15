/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  xsvc_pb_lib_base.c
	@brief

	Description:  									\n
	Module: PlugIn / playback						\n

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
#include <vkernel.h>
#include <octo_common.h>
#include <isosvc.h>

#include <pal_pipe.h>
#include <pal_pb.h>
//#include <mwc_util.h>

#include <svc_pb.h>

#include "./local_include/_svc_pb.h"

#include <pal_sys.h>

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
STATIC PostMsgToMgrCb_t s_fnSvcPb_PostMsgToAp = NULL;

#define ___LOCAL_FUNCTIONS___

void svc_pb_PostMsgToAp(HINT32 nMsg, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	if (s_fnSvcPb_PostMsgToAp)
	{
		 (* s_fnSvcPb_PostMsgToAp)(nMsg, hAction, nParam1, nParam2, nParam3);
	}
}

#define ___MEMBER_FUNCTIONS___
HERROR svc_pb_FuncSetPostMsgCallback (PostMsgToMgrCb_t fnPostMsg)
{
	s_fnSvcPb_PostMsgToAp = fnPostMsg;

	return ERR_OK;
}


HERROR svc_pb_Func_Setup (svcPb_BaseData_t *pstData, SvcPb_Msg_t *pstMsg)
{
	HUINT32	ulActionId	= 0;
	HERROR	hErr		= ERR_OK;

	HUINT32 ulPlayerId	= RxRSCID_NULL;
	HUINT32 ulRecordId	= RxRSCID_NULL;
	HUINT32 ulDemuxId	= RxRSCID_NULL;

	SvcPb_ActType_e ePbActType;
	DxSvcType_e		eSvcType;

	HUINT8 *pucKey = NULL, *pucIv = NULL;
	PalPipe_Input_e  ePipeInputType;


	if( NULL == pstMsg )
		return ERR_FAIL;

	hErr = svc_pb_GetMsgArg( pstMsg, ePBMSG_ARGTYPE_Setup, &(pstData->stSetupInfo) );

	ePbActType	= pstData->stSetupInfo.ePbActType;
	eSvcType	= pstData->stSetupInfo.eSvcType;
	ulRecordId	= pstData->stSetupInfo.ulRecordId;

	/* get resource */
	{
		pstData->hAction	=	pstMsg->hAction;
		ulActionId			=	PAL_PIPE_GetActionId (pstData->hAction);

		hErr = PAL_PIPE_GetResourceId(ulActionId, eRxRSCTYPE_PVR_PLAYER, &ulPlayerId);
		if (ERR_OK != hErr)
		{
			HxLOG_Error  ("PAL_PIPE_GetResourceId ulActionId=%d\n", ulActionId);
			return ERR_FAIL;
		}

		hErr = PAL_PIPE_GetResourceId (ulActionId, eRxRSCTYPE_DEMUX, &ulDemuxId);
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("PAL_PIPE_GetResourceId err: ulActionId=%d\n", ulActionId);
			return ERR_FAIL;
		}

		pstData->ulPlayerId	= ulPlayerId;
		pstData->ulDemuxId	= ulDemuxId;
	}

	/* action to PAL */
	{
		PalPb_Setup_t stSetup;
		HxSTD_memset( &stSetup, 0, sizeof(PalPb_Setup_t) );

		stSetup.ulDemuxId = ulDemuxId;
		snprintf(stSetup.aucFileName, PAL_PVR_MAX_FILE_NAME_FULLPATH_LENGTH-1, "%s%s", pstData->stSetupInfo.aucPath, pstData->stSetupInfo.aucFileName );

		if( eDxSVC_TYPE_TV == eSvcType ) stSetup.eStreamType = eSTREAMTYPE_VIDEO;
		else						  stSetup.eStreamType = eSTREAMTYPE_AUDIO;

		switch( pstData->stSetupInfo.eStartPosType )
		{
			case ePb_StartPosition_Begin:	stSetup.eStartPosType =	ePAL_FS_START_POS_BEGIN; break;
			case ePb_StartPosition_LastView:stSetup.eStartPosType =	ePAL_FS_START_POS_LASTVIEW; break;
			case ePb_StartPosition_End:		stSetup.eStartPosType =	ePAL_FS_START_POS_END; break;
			default:
				return ERR_FAIL;
		}

		switch( ePbActType )
		{
			case ePb_ActType_Tsr_Playback:
				stSetup.eFileType = ePAL_FS_FILETYPE_CIRCULAR;
				stSetup.bIsRecording = TRUE;
				break;

			case ePb_ActType_Transcoding_Playback:
				stSetup.eFileType = ePAL_FS_FILETYPE_LINEAR;
				stSetup.bIsRecording = FALSE;
				break;

			case ePb_ActType_Transcoding_TsrPlayback:
				stSetup.eFileType = ePAL_FS_FILETYPE_CIRCULAR;
				stSetup.bIsRecording = TRUE;
				break;

			case ePb_ActType_Playback:
				stSetup.eFileType = ePAL_FS_FILETYPE_LINEAR;
				stSetup.bIsRecording = FALSE;
				break;

			case ePb_ActType_Chase_Playback:
				stSetup.eFileType = ePAL_FS_FILETYPE_LINEAR;
				stSetup.bIsRecording = TRUE;
				break;

			default:
				HxLOG_Error ("Unknown Action Type\n");
				return ERR_FAIL;
		}

		stSetup.ulStartTime = pstData->stSetupInfo.ulStartTime;

		/*output*/
		{
			HUINT32 ulAvDecId, ulTranscoderId;

			hErr = PAL_PIPE_GetResourceId (ulActionId, eRxRSCTYPE_AV_DEC, &ulAvDecId);
			if (ERR_OK == hErr)
			{
				PAL_PIPE_SetOutputResource (ulActionId, ePAL_PIPE_OUTPUT_DECORDER, ulAvDecId);
			}
			else
			{
				hErr = PAL_PIPE_GetResourceId (ulActionId, eRxRSCTYPE_TRANSCODER, &ulTranscoderId);
				if (ERR_OK == hErr)
				{
					PAL_PIPE_SetOutputResource (ulActionId, ePAL_PIPE_OUTPUT_TRANSCODER, ulTranscoderId);
				}
				else
				{
					HxLOG_Error ("No AV Dec, No Transcoder... ActionId(%d)\n", ulActionId);
					return hErr;
				}
			}

			PAL_PIPE_SetDemux (ulActionId, ulDemuxId);
		}

		/* input */
		{
			HUINT32	ulInputResId = 0;

			switch( ePbActType )
			{
				case ePb_ActType_Tsr_Playback :
				case ePb_ActType_Chase_Playback :
				case ePb_ActType_Transcoding_Playback :
				case ePb_ActType_Transcoding_TsrPlayback :
						ulInputResId = ulRecordId;
						ePipeInputType = (TRUE == pstData->stSetupInfo.bDescramblePlay) ? ePAL_PIPE_INPUT_SCRAMBLED_RECODER : ePAL_PIPE_INPUT_RECODER;
						break;
				case ePb_ActType_Playback :
				default :
					ePipeInputType = (TRUE == pstData->stSetupInfo.bDescramblePlay) ? ePAL_PIPE_INPUT_SCRAMBLED_PLAYER : ePAL_PIPE_INPUT_PLAYER;
					ulInputResId = ulPlayerId;
					break;
			}

			PAL_PIPE_SetInputResource (ulActionId, ePipeInputType, (HUINT32)ulInputResId);
		}

		if( TRUE == pstData->stSetupInfo.bEncryption )
		{
			stSetup.stEncryptSettings.eEncType = (PalCrypt_EncryptionType_e)PAL_SYS_GetPvrEncryptMode();
			if (pstData->stSetupInfo.bUseUserEncKey == TRUE)
			{
				pucKey = pstData->stSetupInfo.aucKey;
			}
			else
			{
				pucKey = PAL_SYS_GetEncryptionKey();
			}
			if(pucKey != NULL)
			{
				HxSTD_MemCopy (stSetup.stEncryptSettings.aucKey, pucKey, PAL_PVR_CRYPTO_KEY_LEN);
			}

			if (stSetup.stEncryptSettings.eEncType == ePAL_PVR_ENCRYPTION_TYPE_AES)
			{
				pucIv = PAL_SYS_GetEncryptionIV();
				if(pucIv != NULL)
				{
					HxSTD_MemCopy (stSetup.stEncryptSettings.aucIv, pucIv, PAL_PVR_CRYPTO_IV_LEN);
				}
			}
		}

		/* is play on a usb */
		stSetup.bIsUsbFlashRecord = pstData->stSetupInfo.bIsUsbFlashRecord;
		stSetup.bStartPaused = pstData->stSetupInfo.bStartPaused;

		hErr = PAL_PB_Setup( ulPlayerId, &stSetup );
		if( hErr != ERR_OK )
		{
			HxLOG_Error ("PAL_PB_Setup error.\n");
			return hErr;
		}

		/*finish*/
		pstData->eState = ePB_STATE_Setup;
		pstData->ulLastPbTimeTick = 0;
	}

	svc_pb_PostMsgToAp(eSEVT_PB_SETUP, pstData->hAction, ERR_OK, 0, 0);

	return ERR_OK;
}

HERROR svc_pb_Func_Start (svcPb_BaseData_t *pstData, SvcPb_Msg_t *pstMsg)
{
	HERROR	hErr	=	ERR_FAIL;

	//SvcMeta_Record_t stRecordInfo;
	hErr = PAL_PB_Start( pstData->ulPlayerId );
	if( ERR_OK != hErr )
	{
		HxLOG_Error ("PAL_PB_Start\n");
		return ERR_FAIL;
	}

	svc_pb_PostMsgToAp(eSEVT_PB_START, pstData->hAction, ERR_OK, 0, 0);

	if(	pstData->stSetupInfo.bStartPaused == TRUE )
	{
		pstData->eState			= ePB_STATE_Pause;
		pstData->nTrickSpeed	= 0;
		svc_pb_PostMsgToAp (eSEVT_PB_SPEED_CHANGED, pstData->hAction, pstData->nTrickSpeed, 0, 0);
	}
	else
	{
		pstData->eState			= ePB_STATE_Start;
		pstData->nTrickSpeed	= 100;
	}

	return ERR_OK;
}

HERROR svc_pb_Func_Stop (svcPb_BaseData_t *pstData, SvcPb_Msg_t *pstMsg)
{
	HERROR	hErr	= ERR_OK;

	hErr = PAL_PB_Stop( pstData->ulPlayerId );
	if( ERR_OK != hErr )
	{
		HxLOG_Error ("PAL_PB_Stop error : ulPlayerId(%d)\n", pstData->ulPlayerId);
		return hErr;
	}

	pstData->eState			= ePB_STATE_Stop;
	pstData->nTrickSpeed	= 0;

	svc_pb_PostMsgToAp (eSEVT_PB_SPEED_CHANGED, pstData->hAction, pstData->nTrickSpeed, 0, 0);

	return ERR_OK;
}

HERROR svc_pb_Func_Close (svcPb_BaseData_t *pstData, SvcPb_Msg_t *pstMsg)
{
	/* N/A */
	return ERR_OK;
}

HERROR svc_pb_Func_Pause (svcPb_BaseData_t *pstData, SvcPb_Msg_t *pstMsg)
{
	HERROR	hErr	=	ERR_OK;

	if( ePB_STATE_Pause == pstData->eState )
	{
		HxLOG_Error ("already PBSTATE is PAUSE!!!\n");
		return ERR_FAIL;
	}

	hErr = PAL_PB_Pause( pstData->ulPlayerId );
	if( ERR_OK != hErr )
	{
		HxLOG_Error ("PAL_PB_Pause error. : ulPlayerId(%d)\n", pstData->ulPlayerId);
		return ERR_FAIL;
	}

	pstData->eState			= ePB_STATE_Pause;
	pstData->nTrickSpeed	= 0;

	svc_pb_PostMsgToAp(eSEVT_PB_SPEED_CHANGED, pstData->hAction, pstData->nTrickSpeed, 0, 0);

	return ERR_OK;
}

HERROR svc_pb_Func_Resume (svcPb_BaseData_t *pstData, SvcPb_Msg_t *pstMsg)
{
	HERROR hErr =	ERR_OK;

	if( (ePB_STATE_Pause != pstData->eState) && (ePB_STATE_Trick != pstData->eState) )
	{
		HxLOG_Error ("already PBSTATE is PLAYING!!!\n");
		return ERR_FAIL;
	}

	hErr = PAL_PB_Resume( pstData->ulPlayerId );
	if( ERR_OK != hErr )
	{
		HxLOG_Error ("PAL_PB_Resume error. : ulPlayerId(%d)\n", pstData->ulPlayerId);
		return hErr;
	}

	pstData->eState			= ePB_STATE_Start;
	pstData->nTrickSpeed	= 100;

	svc_pb_PostMsgToAp(eSEVT_PB_SPEED_CHANGED, pstData->hAction, pstData->nTrickSpeed, 0, 0);

	return ERR_OK;
}

HERROR svc_pb_Func_FrameByFrame (svcPb_BaseData_t *pstData, HBOOL bFoward)
{
	HERROR hErr = ERR_OK;

	hErr = PAL_PB_FrameByFrame( pstData->ulPlayerId, bFoward);
	if( ERR_OK != hErr )
	{
		HxLOG_Error ("PAL_PB_FrameByFrame, error. : ulPlayerId(%d)\n", pstData->ulPlayerId);
		return hErr;
	}

	return ERR_OK;
}

HERROR svc_pb_Func_SetSpeed (svcPb_BaseData_t *pstData, HINT32 nSpeed)
{
	HERROR 			hErr 		= ERR_OK;
	HINT32			nPrevSpeed	= 0;
	HUINT32			ulTime 		= 0;

	nPrevSpeed = pstData->nTrickSpeed;
	pstData->nTrickSpeed = nSpeed;

	if( nSpeed == 100 )		pstData->eState = ePB_STATE_Start;
 	else if( nSpeed == 0 )	pstData->eState = ePB_STATE_Setup;
	else					pstData->eState = ePB_STATE_Trick;

	if( ePB_STATE_Trick == pstData->eState )
	{
		hErr = PAL_PB_GetPosition( pstData->ulPlayerId, &ulTime );
		if( ERR_OK != hErr )
		{
			HxLOG_Error ("PAL_PB_GetPosition, error.\n");
		}
	}

	hErr = PAL_PB_SetSpeed (pstData->ulPlayerId , nSpeed);
	if( hErr != ERR_OK )
	{
		HxLOG_Error ("PAL_PB_SetSpeed, ulPlayerId(%d)\n", pstData->ulPlayerId);
		return hErr;
	}

	if (nSpeed != nPrevSpeed)
	{
		svc_pb_PostMsgToAp(eSEVT_PB_SPEED_CHANGED, pstData->hAction, pstData->nTrickSpeed, 0, 0);
	}

	return ERR_OK;
}

HERROR svc_pb_Func_GetSpeed (svcPb_BaseData_t *pstData, HINT32 *pnSpeed)
{
	HERROR 			hErr		= ERR_OK;

	if( NULL == pnSpeed )
		return ERR_FAIL;

	hErr = PAL_PB_GetSpeed( pstData->ulPlayerId, (PalPb_TrickSpeed_e *) pnSpeed);
	if( hErr != ERR_OK )
	{
		HxLOG_Error ("PAL_PB_GetSpeed, ulPlayerId(%d)\n", pstData->ulPlayerId);
		return hErr;
	}

	return ERR_OK;

}

HERROR svc_pb_Func_GetSupportSpeeds(svcPb_BaseData_t *pstData, HBOOL bScrambled, HUINT32 *pulSpeedNum, HINT32 **ppnSpeedArray)
{
	HERROR	hErr = ERR_OK;
	hErr = PAL_PB_GetSupportSpeeds (bScrambled, pulSpeedNum, ppnSpeedArray);
	if( ERR_OK != hErr )
	{
		HxLOG_Error ("PAL_PB_GetSupportSpeeds error. \n");
		return hErr;
	}

	return ERR_OK;
}

HERROR svc_pb_Func_SetPlayTime(svcPb_BaseData_t *pstData, HUINT32 ulTime )
{
	HERROR	hErr = ERR_OK;
	hErr = PAL_PB_SetPosition( pstData->ulPlayerId, ulTime);
	if( ERR_OK != hErr )
	{
		HxLOG_Error ("PAL_PB_SetPosition error. : ulPlayerId(%d)\n", pstData->ulPlayerId);
		return hErr;
	}

	return hErr;

}

HERROR svc_pb_Func_GetPlayTime(svcPb_BaseData_t *pstData, HUINT32 *pulTime )
{
	HERROR	hErr = ERR_OK;

	if( pulTime == NULL )
		return ERR_FAIL;

	hErr = PAL_PB_GetPosition( pstData->ulPlayerId, pulTime);
	if( ERR_OK != hErr )
	{
		HxLOG_Error ("PAL_PB_GetPosition, error. : ulPlayerId(%d)\n", pstData->ulPlayerId);
		return hErr;
	}

	return ERR_OK;
}

HERROR svc_pb_Func_GetTimeInfo(svcPb_BaseData_t *pstData, HUINT32 *pulCurrentTime, HUINT32 *pulEndTime )
{
	HERROR			hErr		= ERR_OK;

	if( (NULL == pulCurrentTime) || (NULL == pulEndTime) )
		return ERR_FAIL;

	hErr = PAL_PB_GetPosition( pstData->ulPlayerId, pulCurrentTime);
	if( ERR_OK != hErr )
	{
		HxLOG_Error ("PAL_PB_GetPosition, error. : ulPlayerId(%d)\n", pstData->ulPlayerId);
		return hErr;
	}

	hErr = PAL_PB_GetDuration( pstData->ulPlayerId, pulEndTime);
	if( ERR_OK != hErr )
	{
		HxLOG_Error ("PAL_PB_GetDuration, error. : ulPlayerId(%d)\n", pstData->ulPlayerId);
		return hErr;
	}

	return ERR_OK;
}

HERROR svc_pb_Func_GetState(svcPb_BaseData_t *pstData, svcPb_State_e *pePbState)
{
	if (NULL == pePbState)
		return ERR_FAIL;

	*pePbState = pstData->eState;

	return ERR_OK;
}

HERROR svc_pb_Func_EvtPlayback (svcPb_BaseData_t *pstData, SvcPb_Msg_t *pstMsg)
{
	HERROR	hErr	=	ERR_OK;
	SvcPb_EvtParam_t	stEvtParam;

	hErr = svc_pb_GetMsgArg( pstMsg, ePBMSG_ARGTYPE_PbEvtParam, &(stEvtParam) );
	if( ERR_OK != hErr )
		return ERR_FAIL;

	switch( stEvtParam.eType )
	{
		case ePBEVT_TIMECODE:
		{
			HUINT32 ulTime = 0, ulEndTime=0, ulTimeStamp=0;
#if defined(CONFIG_MW_CAS_CONAX)
			if (stEvtParam.ulParam1 - pstData->ulLastPbTimeTick < 1)
			{
				return ERR_OK;
			}

			pstData->ulLastPbTimeTick = stEvtParam.ulParam1;
#else
			/* skip */
			{
				const HINT32 TRIGGER_TIME	= 1000; //1초
				HBOOL	bEvtTrigger = FALSE;
				HUINT32 ulTick = (HUINT32)VK_TIMER_GetSystemTick();

				if ((ulTick - pstData->ulLastPbTimeTick) >= TRIGGER_TIME)
				{
					// 1초 간격으로 timecode를 올린다.
					pstData->ulLastPbTimeTick = ulTick;
					bEvtTrigger = TRUE;
				}

				if( FALSE == bEvtTrigger )
				{
					/*Skip Message*/
					return ERR_OK;
				}
			}
#endif
			ulTime		= stEvtParam.ulParam1;
			ulEndTime	= stEvtParam.ulParam2;
			ulTimeStamp = stEvtParam.ulParam3;

			svc_pb_PostMsgToAp(eSEVT_PB_TIMECODE_CHANGE, pstData->hAction, ulTime, ulEndTime, ulTimeStamp);
			break;
		}

		case ePBEVT_EOF:
		{
			svc_pb_PostMsgToAp(eSEVT_PB_EOF, pstData->hAction, 0, 0, 0);
			break;
		}

		case ePBEVT_BOF:
		{
			svc_pb_PostMsgToAp(eSEVT_PB_BOF, pstData->hAction, 0, 0, 0);
			break;
		}

		default :
			HxLOG_Error("not support type(%d)\n", stEvtParam.eType );
			return ERR_FAIL;

	}

	return ERR_OK;
}

HERROR svc_pb_Func_SetDecrypt (svcPb_BaseData_t *pstData, SvcPb_Msg_t *pstMsg)
{
	HERROR hErr;
	HxLOG_Trace();

	hErr = svc_pb_GetMsgArg( pstMsg, ePBMSG_ARGTYPE_SetDecryption, &(pstData->stSetupInfo) );
	if (hErr != ERR_OK)
	{
		HxLOG_Error("svc_pb_GetMsgArg failed. \n");
		return ERR_FAIL;
	}

	HxLOG_Info("pstData->stSetupInfo.bEncryption: %d \n", pstData->stSetupInfo.bEncryption);
	if( pstData->stSetupInfo.bEncryption == FALSE )
	{
		goto END_FUNC;
	}
	else
	{
		HUINT32 	ulRet = ERR_OK;
		HUINT8		*pucKey = NULL, *pucIv = NULL;
		PalCrypt_EncryptionType_e eEncType;

		eEncType = PAL_SYS_GetPvrEncryptMode();
		if (pstData->stSetupInfo.bUseUserEncKey == TRUE)
		{
			pucKey = pstData->stSetupInfo.aucKey;
		}
		else
		{
			pucKey = PAL_SYS_GetEncryptionKey();
		}

		HxLOG_Debug("user enc(%d) \n",pstData->stSetupInfo.bUseUserEncKey);

		if(eEncType == ePAL_PVR_ENCRYPTION_TYPE_AES)
		{
			pucIv = PAL_SYS_GetEncryptionIV();
		}

		ulRet = PAL_PB_SetDecryption(pstData->ulPlayerId, eEncType, pucKey, pucIv, NULL);
		if( ulRet != ERR_OK )
		{
			HxLOG_Error("PAL_PB_SetDecryption error\n");
			goto END_FUNC;
		}
	}

END_FUNC:

	return ERR_OK;
}
