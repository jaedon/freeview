/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  xsvc_rec_eusat.c
	@brief

	Description:  									\n
	Module: PlugIn	/  Recorder						\n

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
#include <vkernel.h>
#include <octo_common.h>
#include <isosvc.h>

#include <pal_sys.h>
#include <pal_rec.h>

//#include <db_svc.h>

#include <svc_rec.h>

#include "./local_include/_svc_rec.h"


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
STATIC void svc_rec_proc_MsgCmdSetup (svcRec_BaseData_t *pstData, SvcRec_Msg_t *pstMsg)
{
	HxLOG_Trace();

	svc_rec_FuncSetup( pstData, pstMsg );

	HxLOG_Info("pstData->stSetupInfo.bEncryption: %d \n", pstData->stSetupInfo.bEncryption);
	if( pstData->stSetupInfo.bEncryption == FALSE )
	{
		goto END_ENCRYPTION_SETTING;
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
		if(eEncType == ePAL_PVR_ENCRYPTION_TYPE_AES)
		{
			pucIv = PAL_SYS_GetEncryptionIV();
		}

		ulRet = PAL_REC_SetEncryption( pstData->ulRecordId, eEncType, pucKey, pucIv );
		if( ulRet != ERR_OK )
		{
			HxLOG_Error("PAL_REC_SetEncryption error\n");
			goto END_ENCRYPTION_SETTING;
		}
	}

END_ENCRYPTION_SETTING:

	return ;
}

STATIC void svc_rec_proc_MsgCmdUpdateVideoCodec  (svcRec_BaseData_t *pstData, SvcRec_Msg_t *pstMsg)
{
	HxLOG_Trace();

	svc_rec_FuncUpdateVideoCodec( pstData, pstMsg );

	HxLOG_Info("pstData->stSetupInfo.bEncryption: %d \n", pstData->stSetupInfo.bEncryption);
	if( pstData->stSetupInfo.bEncryption == FALSE )
	{
		goto END_ENCRYPTION_SETTING;
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
		if(eEncType == ePAL_PVR_ENCRYPTION_TYPE_AES)
		{
			pucIv = PAL_SYS_GetEncryptionIV();
		}

		ulRet = PAL_REC_SetEncryption( pstData->ulRecordId, eEncType, pucKey, pucIv );
		if( ulRet != ERR_OK )
		{
			HxLOG_Error("PAL_REC_SetEncryption error\n");
			goto END_ENCRYPTION_SETTING;
		}
	}

END_ENCRYPTION_SETTING:

	return ;
}

STATIC void svc_rec_proc_MsgCmdSetEncryption  (svcRec_BaseData_t *pstData, SvcRec_Msg_t *pstMsg)
{
	HxLOG_Trace();

	svc_rec_FuncSetEncryption( pstData, pstMsg );

	HxLOG_Info("pstData->stSetupInfo.bEncryption: %d \n", pstData->stSetupInfo.bEncryption);
	if( pstData->stSetupInfo.bEncryption == FALSE )
	{
		goto END_ENCRYPTION_SETTING;
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

		ulRet = PAL_REC_SetEncryption( pstData->ulRecordId, eEncType, pucKey, pucIv );
		if( ulRet != ERR_OK )
		{
			HxLOG_Error("PAL_REC_SetEncryption error\n");
			goto END_ENCRYPTION_SETTING;
		}
	}

END_ENCRYPTION_SETTING:

	return ;
}

STATIC void svc_rec_proc_MsgCmdSetPid (svcRec_BaseData_t *pstData, SvcRec_Msg_t *pstMsg)
{
	svc_rec_FuncSetPid( pstData, pstMsg );
}

STATIC void svc_rec_proc_MsgCmdStart (svcRec_BaseData_t *pstData, SvcRec_Msg_t *pstMsg)
{
	svc_rec_FuncStart( pstData, pstMsg );
}

STATIC void svc_rec_proc_MsgCmdPause (svcRec_BaseData_t *pstData, SvcRec_Msg_t *pstMsg)
{
	svc_rec_FuncPause( pstData, pstMsg );
}

STATIC void svc_rec_proc_MsgCmdResume (svcRec_BaseData_t *pstData, SvcRec_Msg_t *pstMsg)
{
	svc_rec_FuncResume( pstData, pstMsg );
}

STATIC void svc_rec_proc_MsgCmdStartDelayed (svcRec_BaseData_t *pstData, SvcRec_Msg_t *pstMsg)
{
	svc_rec_FuncStartDelayed( pstData, pstMsg );
}

STATIC void svc_rec_proc_MsgCmdStop (svcRec_BaseData_t *pstData, SvcRec_Msg_t *pstMsg)
{
	svc_rec_FuncStop( pstData, pstMsg );
}

STATIC void svc_rec_proc_MsgCmdClose (svcRec_BaseData_t *pstData, SvcRec_Msg_t *pstMsg)
{
	svc_rec_FuncClose( pstData, pstMsg );
}

STATIC void svc_rec_proc_MsgCmdEvtRecord (svcRec_BaseData_t *pstData, SvcRec_Msg_t *pstMsg)
{
	svc_rec_FuncEvtRecord( pstData, pstMsg );
}

#define _____REC_MSG_PROCESS_FUNCTION_____
STATIC void svc_rec_proc_ProcessMsg (SvcRec_Msg_t *pstMsg, void *pvData)
{
	svcRec_BaseData_t	*pstData = (svcRec_BaseData_t *)pvData;

	if (NULL == pstData)				{ return; }

	switch (pstMsg->eMsgClass)
	{
	case eRECMSG_CMD_SETUP:
		svc_rec_proc_MsgCmdSetup (pstData, pstMsg);
		break;

	case eRECMSG_CMD_UPDATE_VIDEOCODEC:
		svc_rec_proc_MsgCmdUpdateVideoCodec(pstData, pstMsg);
		break;

	case eRECMSG_CMD_SET_ENCRYPTION:
		svc_rec_proc_MsgCmdSetEncryption(pstData, pstMsg);
		break;

	case eRECMSG_CMD_SET_PID:
		svc_rec_proc_MsgCmdSetPid (pstData, pstMsg);
		break;

	case eRECMSG_CMD_START:
		svc_rec_proc_MsgCmdStart (pstData, pstMsg);
		break;

	case eRECMSG_CMD_PAUSE:
		svc_rec_proc_MsgCmdPause (pstData, pstMsg);
		break;

	case eRECMSG_CMD_RESUME:
		svc_rec_proc_MsgCmdResume (pstData, pstMsg);
		break;

	case eRECMSG_CMD_STARTDELAYED:
		svc_rec_proc_MsgCmdStartDelayed (pstData, pstMsg);
		break;

	case eRECMSG_CMD_STOP:
		svc_rec_proc_MsgCmdStop (pstData, pstMsg);
		break;

	case eRECMSG_CMD_CLOSE:
		svc_rec_proc_MsgCmdClose (pstData, pstMsg);
		break;

	case eRECMSG_CMD_EVT_RECORD:
		svc_rec_proc_MsgCmdEvtRecord (pstData, pstMsg);
		break;

	default:
		HxLOG_Debug ("Non-processing MSG (0x%08x) -> skipped:\n", pstMsg->eMsgClass);
		break;
	}
}

#define ___MEMBER_FUNCTIONS___
SvcRec_ActFunc svc_rec_GetActionFunc (void)
{
	return svc_rec_proc_ProcessMsg;
}

void *svc_rec_AllocActionData (void)
{
	svcRec_BaseData_t	*pstData;

	pstData = (svcRec_BaseData_t *)OxMW_Calloc (sizeof(svcRec_BaseData_t));
	if (NULL != pstData)
	{
		HxSTD_memset( pstData, 0, sizeof(svcRec_BaseData_t) );

		pstData->eState		= eREC_STATE_Init;
		pstData->hAction	= HANDLE_NULL;
	}

	return pstData;
}

void svc_rec_FreeActionData (void *pvData)
{
	svcRec_BaseData_t	*pstData = (svcRec_BaseData_t *)pvData;

	if (NULL != pstData)
	{
		OxMW_Free (pstData);
	}

	return;
}

Handle_t svc_rec_GetActionHandleFromResourceId ( void *pvData, HUINT32 ulResourceId, svcRec_ResType_e eResType)
{
	svcRec_BaseData_t	*pstData = (svcRec_BaseData_t *)pvData;

	if( NULL == pstData )
		return HANDLE_NULL;

	switch( eResType )
	{
		case eRecResType_RecordId:
		{
			if( pstData->ulRecordId == ulResourceId )
				return pstData->hAction;

			break;
		}
		case eRecResType_DelayedId:
		{
			if( NULL != pstData->pDelayed )
			{
				if( pstData->pDelayed->ulInstanceId == ulResourceId )
					return pstData->hAction;
			}

			break;
		}
		default:
			break;
	}

	return HANDLE_NULL;
}
/* end of file */
