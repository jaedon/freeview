/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  svc_pb_api.c
	@brief

	Description:  									\n
	Module: SVC / Playback							\n

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
#include <octo_common.h>

#include <pal_pipe.h>
#include <pal_pb.h>
#include <pal_sys.h>
#include <pal_param.h>

#include <svc_rec.h>
#include <svc_pb.h>
#include <svc_meta.h>
#include "./local_include/_svc_pb.h"

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
STATIC HERROR svc_pb_BaseSendMsg( Handle_t hAction, HBOOL bSync, svcPb_MsgType_e eMsgClass)
{
	HUINT32 		ulActionId		= PAL_PIPE_GetActionId(hAction);
	HINT32			nContextId		= svc_pb_GetContextIndex(hAction);
	SvcPb_Msg_t 	stMsg;
	HERROR			hErr, hResult = ERR_FAIL;

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

	HxSTD_MemSet (&stMsg, 0, sizeof(SvcPb_Msg_t));

	stMsg.eMsgClass 				= eMsgClass;
	stMsg.hAction					= hAction;
	stMsg.unArg.stBase.bSyncMode	= bSync;
	stMsg.unArg.stBase.phResult		= &hResult;

	hErr = svc_pb_SendMsgToPbTask (&stMsg);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("svc_pb_SendMsgToPbTask err:\n");
		return ERR_FAIL;
	}

	if (TRUE != bSync)
	{
		return ERR_OK;
	}

	svc_pb_GetSyncSem (nContextId);
	return hResult;
}

#define ___MEMBER_FUNCTIONS___

#define ___API_FUNCTIONS___
HERROR SVC_PB_Init( PostMsgToMgrCb_t fnPostMsg )
{
	if(fnPostMsg !=NULL)
	{
		svc_pb_FuncSetPostMsgCallback(fnPostMsg);
	}

	return svc_pb_InitTask();
}

HERROR SVC_PB_Setup( Handle_t hAction, SvcPb_Setup_t *pstSetup, HBOOL bSync )
{
	HUINT32 		ulActionId		= PAL_PIPE_GetActionId(hAction);
	HINT32			nContextId		= svc_pb_AllocContext(hAction);
	SvcPb_Msg_t 	stMsg;
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

	HxSTD_MemSet (&stMsg, 0, sizeof(SvcPb_Msg_t));

	stMsg.eMsgClass 				= ePBMSG_CMD_SETUP;
	stMsg.hAction					= hAction;
	stMsg.unArg.stSetup.bSyncMode	= bSync;
	stMsg.unArg.stSetup.phResult	= &hResult;

	HxSTD_MemCopy (&(stMsg.unArg.stSetup.stSetup), pstSetup, sizeof(SvcPb_Setup_t));

	hErr = svc_pb_SendMsgToPbTask (&stMsg);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("svc_pb_SendMsgToSiTask err:\n");
		return ERR_FAIL;
	}

	if (TRUE != bSync)
	{
		return ERR_OK;
	}

	svc_pb_GetSyncSem (nContextId);
	return hResult;
}

HERROR SVC_PB_Start( Handle_t hAction, HBOOL bSync )
{
	HxLOG_Trace();
	return svc_pb_BaseSendMsg( hAction, bSync, ePBMSG_CMD_START );
}

HERROR SVC_PB_Stop( Handle_t hAction, HBOOL bSync )
{
	HxLOG_Trace();
	return svc_pb_BaseSendMsg( hAction, bSync, ePBMSG_CMD_STOP );
}

HERROR SVC_PB_Close( Handle_t hAction, HBOOL bSync )
{
	HxLOG_Trace();
	return svc_pb_BaseSendMsg( hAction, bSync, ePBMSG_CMD_CLOSE );
}

HERROR SVC_PB_Pause( Handle_t hAction, HBOOL bSync )
{
	HxLOG_Trace();
	return svc_pb_BaseSendMsg( hAction, bSync, ePBMSG_CMD_PAUSE );
}

HERROR SVC_PB_Resume( Handle_t hAction, HBOOL bSync )
{
	HxLOG_Trace();
	return svc_pb_BaseSendMsg( hAction, bSync, ePBMSG_CMD_RESUME );
}

HERROR SVC_PB_FrameByFrame(Handle_t hAction, HBOOL bFoward)
{
	return svc_pb_FrameByFrame( hAction, bFoward );
}

HERROR SVC_PB_SetSpeed (Handle_t hAction, HINT32 nSpeed )
{
	return svc_pb_SetSpeed( hAction, nSpeed );
}

HERROR SVC_PB_GetSpeed (Handle_t hAction, HINT32 *pnSpeed )
{
	return svc_pb_GetSpeed( hAction, pnSpeed );
}

HERROR SVC_PB_GetSupportSpeeds (Handle_t hAction, HBOOL bScrambled, HUINT32 *pulSpeedNum, HINT32 **ppnSpeedArray)
{
	return svc_pb_GetSupportSpeeds( hAction, bScrambled, pulSpeedNum, ppnSpeedArray );
}

HERROR SVC_PB_SetPlayTime(Handle_t hAction, HUINT32 ulTime )
{
	return svc_pb_SetPlayTime( hAction, ulTime );
}

HERROR SVC_PB_GetPlayTime(Handle_t hAction, HUINT32 *pulTime )
{
	return svc_pb_GetPlayTime( hAction, pulTime );
}

HERROR SVC_PB_GetTimeInfo(Handle_t hAction, HUINT32 *pulCurrentTime, HUINT32 *pulEndTime )
{
	return svc_pb_GetTimeInfo( hAction, pulCurrentTime, pulEndTime );
}

HERROR SVC_PB_GetState (Handle_t hAction, svcPb_State_e *pePbState)
{
	return svc_pb_GetState( hAction, pePbState );
}

HERROR SVC_PB_SetDecrypt( Handle_t hAction, SvcPb_Setup_t *pstSetup, HBOOL bSync )
{
	HUINT32 		ulActionId		= PAL_PIPE_GetActionId(hAction);
	HINT32			nContextId		= svc_pb_GetContextIndex(hAction);
	SvcPb_Msg_t 	stMsg;
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

	HxSTD_MemSet (&stMsg, 0, sizeof(SvcPb_Msg_t));

	stMsg.eMsgClass 				= ePBMSG_CMD_SETDECRYPT;
	stMsg.hAction					= hAction;
	stMsg.unArg.stSetup.bSyncMode	= bSync;
	stMsg.unArg.stSetup.phResult	= &hResult;

	HxSTD_MemCopy (&(stMsg.unArg.stSetup.stSetup), pstSetup, sizeof(SvcPb_Setup_t));

	hErr = svc_pb_SendMsgToPbTask (&stMsg);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("svc_pb_SendMsgToSiTask err:\n");
		return ERR_FAIL;
	}

	if (TRUE != bSync)
	{
		return ERR_OK;
	}

	svc_pb_GetSyncSem (nContextId);
	return hResult;
}

#define __________THUMNAIL__________
HUINT8*	 SVC_PB_ExtractThumbnailFromFile(HCHAR *pszFileName, HUINT32 ulTime, HINT32 nWidth, HINT32 nHeight, HUINT16 usVideoPid, HUINT32 ulVideoCodec, HBOOL bEncryption)
{
	//	WARNING : the function should be called in om_manager layer only
	STATIC HUINT32	s_ulChipId;
	HCHAR	*p = NULL;
	HUINT8	*pucKey = NULL, *pucIv = NULL;
	HUINT8	*pFrameBuf = NULL;
	DxRecListData_t	*pRec;
	HUINT32			ulChipId;
	HxJSON_t		hJson;
	PalPb_ThumbnailSetup_t stThumbSetup;
#if defined(CONFIG_MW_CAS_CONAX)
	SvcMeta_Record_t stRecInfo;
	SvcMeta_Service_t stSvcInfo;
#endif

	HxSTD_memset(&stThumbSetup, 0x00, sizeof(PalPb_ThumbnailSetup_t));

	hJson = HLIB_JSON_DecodeFile(pszFileName);
	if (!hJson)
	{
		HxLOG_Warning("%s(%s) cannot decode json!\n", __FUNCTION__, pszFileName);
		return NULL;
	}
	pRec = DxRECLISTDATA_Decode(hJson, NULL);
	if (pRec == NULL)
	{
		HxLOG_Warning("%s(%s) cannot decode json to DxRecListData_t!\n", __FUNCTION__, pszFileName);
		return NULL;
	}
	ulChipId = pRec->ulChipId;
	DxRECLISTDATA_Delete(pRec);
	HLIB_JSON_Delete(hJson);

	if (s_ulChipId == 0)
	{
		if (PalParam_Activated())
		{
			PAL_PARAM_GetField(ePAL_PARAM_CHIP_ID, (void *)&s_ulChipId, sizeof(HUINT32));
		}
		else
		{
			PAL_SYS_GetChipId(&s_ulChipId);
		}
	}

	if (ulChipId != s_ulChipId)
	{
		HxLOG_Warning("%s(%s) chipID(rec:0x%X, sys:0x%X)  does NOT match!\n", __FUNCTION__, pszFileName, ulChipId, s_ulChipId);
		return NULL;
	}

	p = (HCHAR*)HLIB_STD_MemAlloc( sizeof(HCHAR) * PAL_PVR_MAX_FILE_NAME_FULLPATH_LENGTH );	
	if(NULL == p)
	{
		HxLOG_Warning("Fail memory allocation\n");
		return NULL;
	}
	
	//	nts화일이 존재 하지 않으면 System Nexus Crash
	HLIB_PATH_SplitExt(pszFileName, p, PAL_PVR_MAX_FILE_NAME_FULLPATH_LENGTH);
	snprintf(stThumbSetup.aucFileName, PAL_PVR_MAX_FILE_NAME_FULLPATH_LENGTH, "%s.nts", p);
	if (HLIB_FILE_Exist(stThumbSetup.aucFileName) == FALSE)
	{
//		VK_SEM_Release(s_stPlayer.ulThumbSemId);
		HLIB_STD_MemFree(p);
		return NULL;
	}
	HLIB_STD_MemFree(p);
	
	HLIB_PATH_SplitExt(pszFileName, stThumbSetup.aucFileName, PAL_PVR_MAX_FILE_NAME_FULLPATH_LENGTH);

	stThumbSetup.ulCaptureTime = ulTime;
	stThumbSetup.ulVideoPid = usVideoPid;
	stThumbSetup.VideoCodec = ulVideoCodec;
	stThumbSetup.bTsr = FALSE;
	stThumbSetup.ulDecId = 0;
	if(bEncryption == TRUE)
	{
		stThumbSetup.stEncryptSettings.eEncType = PAL_SYS_GetPvrEncryptMode();
#if defined(CONFIG_MW_CAS_CONAX)
		SVC_META_ReadContentInfo(pszFileName, &stRecInfo, &stSvcInfo, NULL);
		if(stSvcInfo.eCasType & eDxCAS_TYPE_CONAX)
			CXAPI_PVR_GetEncryptionKey(&stRecInfo, ulTime, stThumbSetup.stEncryptSettings.aucKey);
		else
			HxSTD_memcpy (stThumbSetup.stEncryptSettings.aucKey, PAL_SYS_GetEncryptionKey(), PAL_PVR_CRYPTO_KEY_LEN);
#else
		pucKey = PAL_SYS_GetEncryptionKey();
		if(pucKey != NULL)
		{
			HxSTD_memcpy (stThumbSetup.stEncryptSettings.aucKey, pucKey, PAL_PVR_CRYPTO_KEY_LEN);
		}
#endif
		if (stThumbSetup.stEncryptSettings.eEncType == ePAL_PVR_ENCRYPTION_TYPE_AES)
		{
			pucIv = PAL_SYS_GetEncryptionIV();
			if(pucIv != NULL)
			{
				HxSTD_memcpy (stThumbSetup.stEncryptSettings.aucIv, pucIv, PAL_PVR_CRYPTO_IV_LEN);
			}
		}
	}

	pFrameBuf = PAL_PB_ExtractThumbnail(nWidth, nHeight, &stThumbSetup);
	if (pFrameBuf == NULL)
	{
		return NULL;
	}

	return HLIB_STD_MemDup(pFrameBuf, nWidth * nHeight * sizeof(HUINT32));
}
