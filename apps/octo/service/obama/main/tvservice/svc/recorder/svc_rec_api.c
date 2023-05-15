/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  svc_rec_api.c
	@brief

	Description:  									\n
	Module: SVC / REC								\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding À» »ç¿ëÇÏ´Â source file ÀÔ´Ï´Ù.
* MS949 encodingªòÞÅéÄª·ªÆ source fileªÇª¹¡£
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
#include <octo_common.h>

#include <svc_rec.h>
#include "./local_include/_svc_rec.h"


#include <pal_pipe.h>
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

#define ___LOCAL_FUNCTIONS___
STATIC HERROR svc_rec_BaseSendMsg( Handle_t hAction, HBOOL bSync, svcRec_MsgType_e eMsgClass)
{
	HUINT32 		ulActionId		= PAL_PIPE_GetActionId(hAction);
	HINT32			nContextId		= svc_rec_GetContextIndex(hAction);
	SvcRec_Msg_t 	stMsg;
	HERROR			hErr, hResult = ERR_FAIL;

	if (ulActionId >= eActionId_MAX)
	{
		HxLOG_Error ("Wrong ActionHandle(0x%08x), eMsgClass(%d)\n", hAction, eMsgClass);
		return ERR_FAIL;
	}

	if( -1 >= nContextId )
	{
		HxLOG_Error ("Wrong nContextId(0x%08x), eMsgClass(%d)\n", nContextId, eMsgClass);
		return ERR_FAIL;
	}

	HxSTD_MemSet (&stMsg, 0, sizeof(SvcRec_Msg_t));

	stMsg.eMsgClass 				= eMsgClass;
	stMsg.hAction					= hAction;
	stMsg.unArg.stBase.bSyncMode	= bSync;
	stMsg.unArg.stBase.phResult		= &hResult;

	hErr = svc_rec_SendMsgToRecTask (&stMsg);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("svc_rec_SendMsgToSiTask err:\n");
		return ERR_FAIL;
	}

	if (TRUE != bSync)
	{
		return ERR_OK;
	}

	svc_rec_GetSyncSem (nContextId);
	return hResult;
}

#define ___MEMBER_FUNCTIONS___

#define ___API_FUNCTIONS___
HERROR SVC_REC_Init( PostMsgToMgrCb_t fnPostMsg  )
{
	if(fnPostMsg != NULL)
	{
		svc_rec_FuncSetPostMsgCallback(fnPostMsg);
	}

	return svc_rec_InitTask();
}

HERROR SVC_REC_Setup( Handle_t hAction, SvcRec_Setup_t *pstSetup, HBOOL bSync )
{
	HUINT32 		ulActionId		= PAL_PIPE_GetActionId(hAction);
	HINT32			nContextId		= svc_rec_GetContextIndex(hAction);
	SvcRec_Msg_t 	stMsg;
	HERROR			hErr, hResult = ERR_FAIL;

	HxLOG_Trace();

	if (ulActionId >= eActionId_MAX)
	{
		HxLOG_Error ("Wrong ActionHandle(0x%08x)\n", hAction);
		return ERR_FAIL;
	}

	if( -1 >= nContextId )
	{
		nContextId		= svc_rec_AllocContext(hAction);

		if( -1 >= nContextId )
		{
			HxLOG_Error ("Wrong nContextId(0x%08x)\n", nContextId);
			return ERR_FAIL;
		}
	}

	if (NULL == pstSetup)
	{
		return ERR_FAIL;
	}

	HxSTD_MemSet (&stMsg, 0, sizeof(SvcRec_Msg_t));

	stMsg.eMsgClass 				= eRECMSG_CMD_SETUP;
	stMsg.hAction					= hAction;
	stMsg.unArg.stSetup.bSyncMode	= bSync;
	stMsg.unArg.stSetup.phResult	= &hResult;

	HxSTD_MemCopy (&(stMsg.unArg.stSetup.stSetup), pstSetup, sizeof(SvcRec_Setup_t));


	hErr = svc_rec_SendMsgToRecTask (&stMsg);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("svc_rec_SendMsgToSiTask err:\n");
		return ERR_FAIL;
	}

	if (TRUE != bSync)
	{
		return ERR_OK;
	}

	svc_rec_GetSyncSem (nContextId);
	return hResult;
}

HERROR SVC_REC_UpdateVideoCodec( Handle_t hAction, SvcRec_Setup_t *pstSetup, HBOOL bSync )
{
	HUINT32 		ulActionId		= PAL_PIPE_GetActionId(hAction);
	HINT32			nContextId		= svc_rec_GetContextIndex(hAction);
	SvcRec_Msg_t 	stMsg;
	HERROR			hErr, hResult = ERR_FAIL;

	HxLOG_Trace();

	if (ulActionId >= eActionId_MAX)
	{
		HxLOG_Error ("Wrong ActionHandle(0x%08x)\n", hAction);
		return ERR_FAIL;
	}

	if( -1 >= nContextId )
	{
		HxLOG_Error ("Wrong nContextId(0x%08x)\n", nContextId);
		return ERR_FAIL;
	}

	if (NULL == pstSetup)
	{
		return ERR_FAIL;
	}

	HxSTD_MemSet (&stMsg, 0, sizeof(SvcRec_Msg_t));

	stMsg.eMsgClass 				= eRECMSG_CMD_UPDATE_VIDEOCODEC;
	stMsg.hAction					= hAction;
	stMsg.unArg.stSetup.bSyncMode	= bSync;
	stMsg.unArg.stSetup.phResult	= &hResult;

	HxSTD_MemCopy (&(stMsg.unArg.stSetup.stSetup), pstSetup, sizeof(SvcRec_Setup_t));


	hErr = svc_rec_SendMsgToRecTask (&stMsg);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("svc_rec_SendMsgToSiTask err:\n");
		return ERR_FAIL;
	}

	if (TRUE != bSync)
	{
		return ERR_OK;
	}

	svc_rec_GetSyncSem (nContextId);
	return hResult;
}

HERROR SVC_REC_SetEncryption( Handle_t hAction, SvcRec_Setup_t *pstSetup, HBOOL bSync )
{
	HUINT32 		ulActionId		= PAL_PIPE_GetActionId(hAction);
	HINT32			nContextId		= svc_rec_GetContextIndex(hAction);
	SvcRec_Msg_t 	stMsg;
	HERROR			hErr, hResult = ERR_FAIL;

	HxLOG_Trace();

	if (ulActionId >= eActionId_MAX)
	{
		HxLOG_Error ("Wrong ActionHandle(0x%08x)\n", hAction);
		return ERR_FAIL;
	}

	if( -1 >= nContextId )
	{
		HxLOG_Error ("Wrong nContextId(0x%08x)\n", nContextId);
		return ERR_FAIL;
	}

	if (NULL == pstSetup)
	{
		return ERR_FAIL;
	}

	HxSTD_MemSet (&stMsg, 0, sizeof(SvcRec_Msg_t));

	stMsg.eMsgClass 				= eRECMSG_CMD_SET_ENCRYPTION;
	stMsg.hAction					= hAction;
	stMsg.unArg.stSetup.bSyncMode	= bSync;
	stMsg.unArg.stSetup.phResult	= &hResult;

	HxSTD_MemCopy (&(stMsg.unArg.stSetup.stSetup), pstSetup, sizeof(SvcRec_Setup_t));


	hErr = svc_rec_SendMsgToRecTask (&stMsg);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("svc_rec_SendMsgToSiTask err:\n");
		return ERR_FAIL;
	}

	if (TRUE != bSync)
	{
		return ERR_OK;
	}

	svc_rec_GetSyncSem (nContextId);
	return hResult;
}

HERROR SVC_REC_SetPID( Handle_t hAction, SvcRec_PIDList_t *pstPIDList, HBOOL bSync )
{
	HUINT32 		ulActionId		= PAL_PIPE_GetActionId(hAction);
	HINT32			nContextId		= svc_rec_GetContextIndex(hAction);
	SvcRec_Msg_t 	stMsg;
	HERROR			hErr, hResult = ERR_FAIL;

	HxLOG_Trace();

	if (ulActionId >= eActionId_MAX)
	{
		HxLOG_Error ("Wrong ActionHandle(0x%08x)\n", hAction);
		return ERR_FAIL;
	}

	if( -1 >= nContextId )
	{
		HxLOG_Error ("Wrong nContextId(0x%08x) (0x%08x)\n", nContextId, hAction);
		return ERR_FAIL;
	}

	if (NULL == pstPIDList)
	{
		return ERR_FAIL;
	}

	HxSTD_MemSet (&stMsg, 0, sizeof(SvcRec_Msg_t));

	stMsg.eMsgClass 				= eRECMSG_CMD_SET_PID;
	stMsg.hAction					= hAction;
	stMsg.unArg.stPIDList.bSyncMode	= bSync;
	stMsg.unArg.stPIDList.phResult	= &hResult;

	HxSTD_MemCopy (&(stMsg.unArg.stPIDList.stPIDList), pstPIDList, sizeof(SvcRec_PIDList_t));

	hErr = svc_rec_SendMsgToRecTask (&stMsg);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("svc_rec_SendMsgToSiTask err:\n");
		return ERR_FAIL;
	}

	if (TRUE != bSync)
	{
		return ERR_OK;
	}

	svc_rec_GetSyncSem (nContextId);
	return hResult;
}

HERROR SVC_REC_Start( Handle_t hAction, HBOOL bSync )
{
	HxLOG_Trace();
	return svc_rec_BaseSendMsg( hAction, bSync, eRECMSG_CMD_START );
}

HERROR SVC_REC_Stop( Handle_t hAction, HBOOL bSync )
{
	HxLOG_Trace();
	return svc_rec_BaseSendMsg( hAction, bSync, eRECMSG_CMD_STOP );
}

HERROR SVC_REC_Close( Handle_t hAction, HBOOL bSync )
{
	HxLOG_Trace();
	return svc_rec_BaseSendMsg( hAction, bSync, eRECMSG_CMD_CLOSE );
}

HERROR SVC_REC_Pause( Handle_t hAction, HBOOL bSync )
{
	HxLOG_Trace();
	return svc_rec_BaseSendMsg( hAction, bSync, eRECMSG_CMD_PAUSE );
}

HERROR SVC_REC_Resume( Handle_t hAction, HBOOL bSync )
{
	HxLOG_Trace();
	return svc_rec_BaseSendMsg( hAction, bSync, eRECMSG_CMD_RESUME );
}

HERROR SVC_REC_StartDelayed( Handle_t hAction, SvcRec_DelayedRecSetup_t *pstSetupInfo, HBOOL bSync )
{
	HUINT32 		ulActionId		= PAL_PIPE_GetActionId(hAction);
	HINT32			nContextId		= svc_rec_GetContextIndex(hAction);
	SvcRec_Msg_t	stMsg;
	HERROR			hErr, hResult = ERR_FAIL;

	HxLOG_Trace();

	if (ulActionId >= eActionId_MAX)
	{
		HxLOG_Error ("Wrong ActionHandle(0x%08x)\n", hAction);
		return ERR_FAIL;
	}

	if( -1 >= nContextId )
	{
		HxLOG_Error ("Wrong nContextId(0x%08x)\n", nContextId);
		return ERR_FAIL;
	}

	HxSTD_MemSet (&stMsg, 0, sizeof(SvcRec_Msg_t));

	stMsg.eMsgClass 						= eRECMSG_CMD_STARTDELAYED;
	stMsg.hAction							= hAction;
	stMsg.unArg.stDelayedSetup.bSyncMode	= bSync;
	stMsg.unArg.stDelayedSetup.phResult		= &hResult;

	HxSTD_MemCopy (&(stMsg.unArg.stDelayedSetup.stDelayedSetup), pstSetupInfo, sizeof(SvcRec_DelayedRecSetup_t));

	hErr = svc_rec_SendMsgToRecTask (&stMsg);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("svc_rec_SendMsgToSiTask err:\n");
		return ERR_FAIL;
	}

	if (TRUE != bSync)
	{
		return ERR_OK;
	}

	svc_rec_GetSyncSem (nContextId);
	return hResult;
}


HERROR SVC_REC_ReplacePSI( Handle_t hAction, HUINT16 usPid, HUINT8 *pucSection, HUINT32 ulSectionSize )
{
	HxLOG_Trace();
	return svc_rec_ReplacePSI( hAction, usPid, pucSection, ulSectionSize);
}


HERROR SVC_REC_CleanReqPidInfo( SvcRec_PidSetType_e ePidSetType, SvcRec_PIDList_t *pstReqInfo )
{
	HxLOG_Trace();
	return svc_rec_CleanReqPidInfo( ePidSetType, pstReqInfo );
}

HERROR SVC_REC_CleanReqPidDes (SvcRec_PIDDes_t stPidDesArray[], HINT32 nArrayCnt )
{
	HxLOG_Trace();
	return svc_rec_CleanReqPidDes( stPidDesArray, nArrayCnt );
}

HERROR SVC_REC_GetInfo(Handle_t hAction, SvcRec_RecInfo *pstRecInfo)
{
	HxLOG_Trace();

	if( NULL == pstRecInfo )
		return ERR_FAIL;

	return svc_rec_GetInfo( hAction, pstRecInfo );
}

HBOOL SVC_REC_IsRecording( HUINT8 *pucFileName )
{
	HxLOG_Trace();

	if( NULL == pucFileName )
		return ERR_FAIL;

	return svc_rec_IsRecording( pucFileName );
}

HBOOL SVC_REC_IsAvailableDelayed( Handle_t hAction )
{
	HxLOG_Trace();
	return svc_rec_IsAvailableDelayed(hAction);
}

HERROR SVC_REC_SetDRMInfo( Handle_t hAction, DxCopyrightControl_t *pstInfo, HBOOL bWriteTimeStamp )
{
	HxLOG_Trace();

	if( NULL == pstInfo )
		return ERR_FAIL;

	return svc_rec_SetDRMInfo( hAction, pstInfo, bWriteTimeStamp );
}

#define xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

#if	defined(CONFIG_DEBUG)
void SVC_REC_DebugPrintRecPidInfo_Base(SvcRec_PIDList_t *pstPIDList, const HCHAR *funcName, HINT32 nLine)
{
	HINT32		i = 0;


	HxLOG_Warning(HxANSI_COLOR_CYAN("call from [%s:%d]\n"), funcName, nLine);

	if(NULL == pstPIDList)
	{
		HxLOG_Error("\t pstPIDList is NULL\n");
		return;
	}

	HxLOG_Warning(HxANSI_COLOR_CYAN("\t pstPIDList->ePidSetType      = %d \n"), pstPIDList->ePidSetType);

	if(eRec_PidSetType_Known == pstPIDList->ePidSetType)
	{
		for(i = 0; i < eSvcRec_MAX_REC_NUM; i++)
		{
			if(PID_NULL != pstPIDList->stKnown.stSI[i].usPid)
			{
				HxLOG_Warning(HxANSI_COLOR_CYAN("\t stKnown.stSI[%d]\t= (PID:0x%04X, ReqID:%d) \n"), i, pstPIDList->stKnown.stSI[i].usPid, pstPIDList->stKnown.stSI[i].ulRequestId);
			}
		}
		for(i = 0; i < SvcRec_MAX_VIDEO_ES_NUM; i++)
		{
			if(PID_NULL != pstPIDList->stKnown.stVideo[i].usPid)
			{
				HxLOG_Warning(HxANSI_COLOR_CYAN("\t stKnown.stVideo[%d]\t= (PID:0x%04X, ReqID:%d) \n"), i, pstPIDList->stKnown.stVideo[i].usPid, pstPIDList->stKnown.stVideo[i].ulRequestId);
			}
		}
		for(i = 0; i < SvcRec_MAX_AUDIO_ES_NUM; i++)
		{
			if(PID_NULL != pstPIDList->stKnown.stAudio[i].usPid)
			{
				HxLOG_Warning(HxANSI_COLOR_CYAN("\t stKnown.stAudio[%d]\t= (PID:0x%04X, ReqID:%d) \n"), i, pstPIDList->stKnown.stAudio[i].usPid, pstPIDList->stKnown.stAudio[i].ulRequestId);
			}
		}
		for(i = 0; i < SvcRec_MAX_SUBTTL_ES_NUM; i++)
		{
			if(PID_NULL != pstPIDList->stKnown.stSubtitle[i].usPid)
			{
				HxLOG_Warning(HxANSI_COLOR_CYAN("\t stKnown.stSubtitle[%d]\t= (PID:0x%04X, ReqID:%d) \n"), i, pstPIDList->stKnown.stSubtitle[i].usPid, pstPIDList->stKnown.stSubtitle[i].ulRequestId);
			}
		}
		for(i = 0; i < SvcRec_MAX_TELTXT_ES_NUM; i++)
		{
			if(PID_NULL != pstPIDList->stKnown.stTeletext[i].usPid)
			{
				HxLOG_Warning(HxANSI_COLOR_CYAN("\t stKnown.stTeletext[%d]\t= (PID:0x%04X, ReqID:%d) \n"), i, pstPIDList->stKnown.stTeletext[i].usPid, pstPIDList->stKnown.stTeletext[i].ulRequestId);
			}
		}
		for(i = 0; i < SvcRec_MAX_CAPTION_ES_NUM; i++)
		{
			if(PID_NULL != pstPIDList->stKnown.stCaption[i].usPid)
			{
				HxLOG_Warning(HxANSI_COLOR_CYAN("\t stKnown.stCaption[%d]\t= (PID:0x%04X, ReqID:%d) \n"), i, pstPIDList->stKnown.stCaption[i].usPid, pstPIDList->stKnown.stCaption[i].ulRequestId);
			}
		}
		for(i = 0; i < SvcRec_MAX_CA_PID_NUM; i++)
		{
			if(PID_NULL != pstPIDList->stKnown.stEcm[i].usPid)
			{
				HxLOG_Warning(HxANSI_COLOR_CYAN("\t stKnown.stEcm[%d]\t= (PID:0x%04X, ReqID:%d) \n"), i, pstPIDList->stKnown.stEcm[i].usPid, pstPIDList->stKnown.stEcm[i].ulRequestId);
			}
		}
		for(i = 0; i < SvcRec_MAX_CA_PID_NUM; i++)
		{
			if(PID_NULL != pstPIDList->stKnown.stEmm[i].usPid)
			{
				HxLOG_Warning(HxANSI_COLOR_CYAN("\t stKnown.stEmm[%d]\t= (PID:0x%04X, ReqID:%d) \n"), i, pstPIDList->stKnown.stEmm[i].usPid, pstPIDList->stKnown.stEmm[i].ulRequestId);
			}
		}
		for(i = 0; i < SvcRec_MAX_DATA_ES_NUM; i++)
		{
			if(PID_NULL != pstPIDList->stKnown.stData[i].usPid)
			{
				HxLOG_Warning(HxANSI_COLOR_CYAN("\t stKnown.stData[%d]\t= (PID:0x%04X, ReqID:%d) \n"), i, pstPIDList->stKnown.stData[i].usPid, pstPIDList->stKnown.stData[i].ulRequestId);
			}
		}
	}
	else if(eRec_PidSetType_Unknown == pstPIDList->ePidSetType)
	{
		for(i = 0; i < DxRSV_PIDTABLE_COUNT; i++)
		{
			if(PID_NULL != pstPIDList->stUnknown.stPIDs[i].usPid)
			{
				HxLOG_Warning(HxANSI_COLOR_CYAN("\t stUnknown.stPIDs[%d]\t= (PID:0x%04X, ReqID:%d) \n"), i, pstPIDList->stUnknown.stPIDs[i].usPid, pstPIDList->stUnknown.stPIDs[i].ulRequestId);
			}
		}
	}
	else if(eRec_PidSetType_All == pstPIDList->ePidSetType)
	{
		HxLOG_Warning(HxANSI_COLOR_CYAN("\t stUnknown.stPIDs\t= (PID:ALL, ReqID:%d) \n"), pstPIDList->stUnknown.stPIDs[i].ulRequestId);
	}

	HxLOG_Warning(HxANSI_COLOR_CYAN("---------------------------------------------------------------\n"));
}
#endif


/* end of file */

