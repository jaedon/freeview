/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  xsvc_pb_eusat.c
	@brief

	Description:  									\n
	Module: PlugIn	/  playback						\n

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

#include <svc_pb.h>

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
STATIC void svc_pb_proc_MsgCmdSetup (svcPb_BaseData_t *pstData, SvcPb_Msg_t *pstMsg)
{
	svc_pb_Func_Setup( pstData, pstMsg );
}

STATIC void svc_pb_proc_MsgCmdStart (svcPb_BaseData_t *pstData, SvcPb_Msg_t *pstMsg)
{
	svc_pb_Func_Start( pstData, pstMsg );
}

STATIC void svc_pb_proc_MsgCmdStop (svcPb_BaseData_t *pstData, SvcPb_Msg_t *pstMsg)
{
	svc_pb_Func_Stop( pstData, pstMsg );
}

STATIC void svc_pb_proc_MsgCmdClose (svcPb_BaseData_t *pstData, SvcPb_Msg_t *pstMsg)
{
	svc_pb_Func_Close( pstData, pstMsg );
}

STATIC void svc_pb_proc_MsgCmdPause (svcPb_BaseData_t *pstData, SvcPb_Msg_t *pstMsg)
{
	svc_pb_Func_Pause( pstData, pstMsg );
}

STATIC void svc_pb_proc_MsgCmdResume (svcPb_BaseData_t *pstData, SvcPb_Msg_t *pstMsg)
{
	svc_pb_Func_Resume( pstData, pstMsg );
}

STATIC void svc_pb_proc_MsgCmdEvtPlayback (svcPb_BaseData_t *pstData, SvcPb_Msg_t *pstMsg)
{
	svc_pb_Func_EvtPlayback( pstData, pstMsg );
}

STATIC void svc_pb_proc_MsgCmdSetDecrypt (svcPb_BaseData_t *pstData, SvcPb_Msg_t *pstMsg)
{
	svc_pb_Func_SetDecrypt( pstData, pstMsg );
}


#define _____PB_MSG_PROCESS_FUNCTION_____
STATIC void svc_pb_proc_ProcessMsg (SvcPb_Msg_t *pstMsg, void *pvData)
{
	svcPb_BaseData_t	*pstData = (svcPb_BaseData_t *)pvData;

	if (NULL == pstData)				{ return; }

	switch (pstMsg->eMsgClass)
	{
	case ePBMSG_CMD_SETUP:
		svc_pb_proc_MsgCmdSetup (pstData, pstMsg);
		break;

	case ePBMSG_CMD_START:
		svc_pb_proc_MsgCmdStart (pstData, pstMsg);
		break;

	case ePBMSG_CMD_STOP:
		svc_pb_proc_MsgCmdStop (pstData, pstMsg);
		break;

	case ePBMSG_CMD_CLOSE:
		svc_pb_proc_MsgCmdClose (pstData, pstMsg);
		break;

	case ePBMSG_CMD_PAUSE:
		svc_pb_proc_MsgCmdPause (pstData, pstMsg);
		break;

	case ePBMSG_CMD_RESUME:
		svc_pb_proc_MsgCmdResume (pstData, pstMsg);
		break;

	case ePBMSG_CMD_EVT_PLAYBACK:
		svc_pb_proc_MsgCmdEvtPlayback(pstData, pstMsg);
		break;

	case ePBMSG_CMD_SETDECRYPT:
		svc_pb_proc_MsgCmdSetDecrypt (pstData, pstMsg);
		break;

	default:
		HxLOG_Debug ("Non-processing MSG (0x%08x) -> skipped:\n", pstMsg->eMsgClass);
		break;
	}
}

#define ___MEMBER_FUNCTIONS___
SvcPb_ActFunc svc_pb_GetActionFunc (void)
{
	return svc_pb_proc_ProcessMsg;
}

void *svc_pb_AllocActionData (void)
{
	svcPb_BaseData_t	*pstData;

	pstData = (svcPb_BaseData_t *)OxMW_Calloc (sizeof(svcPb_BaseData_t));
	if (NULL != pstData)
	{
		HxSTD_memset( pstData, 0, sizeof(svcPb_BaseData_t) );

		pstData->eState = ePB_STATE_Init;
	}

	return pstData;
}

void svc_pb_FreeActionData (void *pvData)
{
	svcPb_BaseData_t	*pstData = (svcPb_BaseData_t *)pvData;

	if (NULL != pstData)
	{
		OxMW_Free (pstData);
	}

	return;
}

Handle_t svc_pb_GetActionHandleFromResourceId ( void *pvData, HUINT32 ulResourceId, svcPb_ResType_e eResType)
{
	svcPb_BaseData_t	*pstData = (svcPb_BaseData_t *)pvData;

	if( NULL == pstData )
		return HANDLE_NULL;

	switch( eResType )
	{
		case ePbResType_PlayerId:
		{
			if( pstData->ulPlayerId == ulResourceId )
				return pstData->hAction;

			break;
		}
		default:
			break;
	}

	return HANDLE_NULL;
}

/* end of file */
