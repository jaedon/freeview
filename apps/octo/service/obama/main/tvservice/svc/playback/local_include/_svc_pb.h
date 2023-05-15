/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  _svc_pb.h
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


#ifndef __INT_SVC_PB_H__
#define __INT_SVC_PB_H__

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct
{
	Handle_t			hAction;
	HUINT32				ulDemuxId;
	HUINT32				ulPlayerId;

	svcPb_State_e		eState;
	HINT32				nTrickSpeed;
	HUINT32				ulLastPbTimeTick;

	SvcPb_Setup_t		stSetupInfo;


	/* plugin */
	void*				pExtension;

} svcPb_BaseData_t;

/*******************************************************************/
/********************      Member Functions     ********************/
/*******************************************************************/

/* main */
HERROR	svc_pb_InitTask(void);
void	svc_pb_GetSyncSem (HUINT32 ulIndex);
void	svc_pb_ReleaseSyncSem (HUINT32 ulIndex);
HINT32	svc_pb_AllocContext (Handle_t hAction);
HINT32	svc_pb_GetContextIndex (Handle_t hAction);
HERROR	svc_pb_SendMsgToPbTask (SvcPb_Msg_t *pstMsg);
HERROR	svc_pb_GetMsgArg (SvcPb_Msg_t *pstMsg, svcPb_MsgArgType_e eArgType, void *pvData);
HERROR	svc_pb_FrameByFrame (Handle_t hAction, HBOOL bFoward);
HERROR	svc_pb_SetSpeed (Handle_t hAction, HINT32 nSpeed);
HERROR	svc_pb_GetSpeed (Handle_t hAction, HINT32 *pnSpeed);
HERROR	svc_pb_GetSupportSpeeds (Handle_t hAction, HBOOL bScrambled, HUINT32 *pulSpeedNum, HINT32 **ppnSpeedArray);
HERROR	svc_pb_SetPlayTime(Handle_t hAction, HUINT32 ulTime );
HERROR	svc_pb_GetPlayTime(Handle_t hAction, HUINT32 *pulTime );
HERROR	svc_pb_GetTimeInfo(Handle_t hAction, HUINT32 *pulCurrentTime, HUINT32 *pulEndTime );
HERROR	svc_pb_GetState(Handle_t hAction, svcPb_State_e *pePbState);

/* proc */
SvcPb_ActFunc	svc_pb_GetActionFunc (void);
void	*svc_pb_AllocActionData (void);
void	svc_pb_FreeActionData (void *pvData);
Handle_t	svc_pb_GetActionHandleFromResourceId ( void *pvData, HUINT32 ulResourceId, svcPb_ResType_e eResType);

/* func */
HERROR svc_pb_FuncSetPostMsgCallback (PostMsgToMgrCb_t fnPostMsg);
HERROR	svc_pb_Func_Setup (svcPb_BaseData_t *pstData, SvcPb_Msg_t *pstMsg);
HERROR	svc_pb_Func_Start (svcPb_BaseData_t *pstData, SvcPb_Msg_t *pstMsg);
HERROR	svc_pb_Func_Stop (svcPb_BaseData_t *pstData, SvcPb_Msg_t *pstMsg);
HERROR	svc_pb_Func_Close (svcPb_BaseData_t *pstData, SvcPb_Msg_t *pstMsg);
HERROR	svc_pb_Func_Pause (svcPb_BaseData_t *pstData, SvcPb_Msg_t *pstMsg);
HERROR	svc_pb_Func_Resume (svcPb_BaseData_t *pstData, SvcPb_Msg_t *pstMsg);
HERROR	svc_pb_Func_FrameByFrame (svcPb_BaseData_t *pstData, HBOOL bFoward);
HERROR	svc_pb_Func_SetSpeed (svcPb_BaseData_t *pstData, HINT32 nSpeed);
HERROR	svc_pb_Func_GetSpeed (svcPb_BaseData_t *pstData, HINT32 *pnSpeed);
HERROR	svc_pb_Func_GetSupportSpeeds(svcPb_BaseData_t *pstData, HBOOL bScrambled, HUINT32 *pulSpeedNum, HINT32 **ppnSpeedArray);
HERROR	svc_pb_Func_SetPlayTime(svcPb_BaseData_t *pstData, HUINT32 ulTime );
HERROR	svc_pb_Func_GetPlayTime(svcPb_BaseData_t *pstData, HUINT32 *pulTime);
HERROR	svc_pb_Func_GetTimeInfo(svcPb_BaseData_t *pstData, HUINT32 *pulCurrentTime, HUINT32 *pulEndTime );
HERROR	svc_pb_Func_EvtPlayback (svcPb_BaseData_t *pstData, SvcPb_Msg_t *pstMsg);
HERROR  svc_pb_Func_GetState (svcPb_BaseData_t *pstData, svcPb_State_e *pePbState);
HERROR	svc_pb_Func_SetDecrypt (svcPb_BaseData_t *pstData, SvcPb_Msg_t *pstMsg);


#endif	// __INT_SVC_PB_H__
/* end of file */
