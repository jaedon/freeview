/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  _svc_rec.h
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

#ifndef __INT_SVC_REC_H__
#define __INT_SVC_REC_H__

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define MAX_DELAYREC_SPLIT_NUM				48

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

typedef enum
{
	eREC_STATE_Init		= 0,
	eREC_STATE_Setup,
	eREC_STATE_Start,
	eREC_STATE_StartDelayed,
	eREC_STATE_Stop,

	eREC_STATE_Pause,

	eREC_STATE_EndOfCase
} svcRec_State_e;

typedef  struct
{
	HINT32			nEvtId;
	HUINT32 		ulStartTime;
	HUINT32 		ulEndTime;
	HUINT8			aucEventName[MAX_FILE_LEN];
} svcRec_Split_t;

typedef  struct
{
	HUINT32			ulTotalNumber;
	HUINT32			ulStartNumber;
	HUINT32			ulCurrentNumber;
	HUINT32			ulDelayRecStartTime;
	HUINT32			ulRecTime;
	svcRec_Split_t	stSplit[MAX_DELAYREC_SPLIT_NUM];
} svcRec_SplitList_t;

typedef  struct
{
	HBOOL				bUse;
	HBOOL				bSplit;
	HUINT32				ulInstanceId;
	HULONG				ulFileCheckTimerId;
	HBOOL				bEncryption;
	HBOOL				bUseUserEncKey;
	HUINT8			 	aucKey[PAL_PVR_CRYPTO_KEY_LEN];

//	HUINT32				ulContentId;
//	HUINT8				aucTargetPath[MAX_FILE_LEN];
//	HUINT8				aucTempPath[MAX_FILE_LEN];
//	HUINT8				aucTempFileName[MAX_FILE_LEN];
	svcRec_SplitList_t	*pstSplitInfo;
} svcRec_Delayed_t;

typedef struct
{
	Handle_t			hAction;
	HUINT32				ulDemuxId;
	HUINT32				ulRecordId;

	svcRec_State_e		eState;

	UNIXTIME			ulStartTime;
	UNIXTIME			ulRecTime;
	UNIXTIME			ulEndTime;

	SvcRec_Setup_t		stSetupInfo;
	SvcRec_PIDList_t	stPIDList;
	HUINT32 			ulPATReplaceHandle;	// only PAT

	/* delayed */
	svcRec_Delayed_t	*pDelayed;

} svcRec_BaseData_t;

/*******************************************************************/
/********************      Member Functions     ********************/
/*******************************************************************/

/* main */
HERROR	svc_rec_InitTask(void);
//#define _SVC_REC_SYNC_SEM_DEBUG_
#ifdef _SVC_REC_SYNC_SEM_DEBUG_
void	_svc_rec_GetSyncSem (HUINT32 ulIndex, const HINT8 *szFunction, HINT32 nLine);
void	_svc_rec_ReleaseSyncSem (HUINT32 ulIndex, const HINT8 *szFunction, HINT32 nLine);
#define svc_rec_GetSyncSem(index)		_svc_rec_GetSyncSem(index, __FUNCTION__, __LINE__)
#define svc_rec_ReleaseSyncSem(index)	_svc_rec_ReleaseSyncSem(index, __FUNCTION__, __LINE__)
#else
void	svc_rec_GetSyncSem (HUINT32 ulIndex);
void	svc_rec_ReleaseSyncSem (HUINT32 ulIndex);
#endif
HINT32	svc_rec_GetContextIndex (Handle_t hAction);
HINT32	svc_rec_AllocContext (Handle_t hAction);
HERROR	svc_rec_SendMsgToRecTask (SvcRec_Msg_t *pstMsg);
HERROR	svc_rec_GetMsgArg (SvcRec_Msg_t *pstMsg, svcRec_MsgArgType_e eArgType, void *pvData);
HERROR	svc_rec_ReplacePSI( Handle_t hAction, HUINT16 usPid, HUINT8 *pucSection, HUINT32 ulSectionSize );
HERROR	svc_rec_CleanReqPidInfo( SvcRec_PidSetType_e ePidSetType, SvcRec_PIDList_t *pstReqInfo );
HERROR	svc_rec_CleanReqPidDes( SvcRec_PIDDes_t stPidDesArray[], HINT32 nArrayCnt );
HERROR	svc_rec_GetInfo(Handle_t hAction, SvcRec_RecInfo *pstRecInfo);
HBOOL	svc_rec_IsRecording( HUINT8 *pucFileName );
HBOOL	svc_rec_IsAvailableDelayed( Handle_t hAction );
HERROR	svc_rec_SetDRMInfo( Handle_t hAction, DxCopyrightControl_t *pstInfo, HBOOL bWriteTimeStamp );
HERROR	svc_rec_SetEncryption( Handle_t hAction );
void 	svc_rec_main_MemIOCallback_t(HUINT32 ulRecordId, HUINT8 *pucBuf, HUINT32 ulLength);
void	svc_rec_main_Callback_t(HUINT32 ulEvent, HUINT32 ulRecordId, HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3);

/* proc */
SvcRec_ActFunc svc_rec_GetActionFunc (void);
void	*svc_rec_AllocActionData (void);
void	svc_rec_FreeActionData (void *pvData);
Handle_t	svc_rec_GetActionHandleFromResourceId ( void *pvData, HUINT32 ulResourceId, svcRec_ResType_e eResType);

/* func */
HERROR	svc_rec_FuncSetPostMsgCallback (PostMsgToMgrCb_t fnPostMsg);
HERROR	svc_rec_FuncSetup (svcRec_BaseData_t *pstData, SvcRec_Msg_t *pstMsg);
HERROR  svc_rec_FuncUpdateVideoCodec (svcRec_BaseData_t *pstData, SvcRec_Msg_t *pstMsg);
HERROR	svc_rec_FuncSetPid (svcRec_BaseData_t *pstData, SvcRec_Msg_t *pstMsg);
HERROR	svc_rec_FuncStart (svcRec_BaseData_t *pstData, SvcRec_Msg_t *pstMsg);
HERROR	svc_rec_FuncPause (svcRec_BaseData_t *pstData, SvcRec_Msg_t *pstMsg);
HERROR	svc_rec_FuncResume (svcRec_BaseData_t *pstData, SvcRec_Msg_t *pstMsg);
HERROR	svc_rec_FuncStartDelayed(svcRec_BaseData_t *pstData, SvcRec_Msg_t *pstMsg);
HERROR	svc_rec_FuncStop (svcRec_BaseData_t *pstData, SvcRec_Msg_t *pstMsg);
HERROR	svc_rec_FuncClose (svcRec_BaseData_t *pstData, SvcRec_Msg_t *pstMsg);
HERROR	svc_rec_FuncEvtRecord (svcRec_BaseData_t *pstData, SvcRec_Msg_t *pstMsg);
void	svc_rec_FuncInitPid( SvcRec_PIDList_t *pstPIDList, SvcRec_PidSetType_e ePidSetType );
void	svc_rec_FuncInitPidDes( SvcRec_PIDDes_t *pstPidDesArray, HINT32 nArrayCnt );
HERROR	svc_rec_FuncReplacePSI( void *pvData, HUINT16 usPid, HUINT8 *pucSection, HUINT32 ulSectionSize );
HERROR	svc_rec_FuncGetInfo( void *pvData, SvcRec_RecInfo *pstRecInfo );
HERROR	svc_rec_FuncSetDRMInfo( void *pvData, DxCopyrightControl_t *pstInfo, HBOOL bWriteTimeStamp );
HBOOL	svc_rec_FuncIsRecordingWithName( void *pvData, HUINT8 *pucFileName);
HERROR	svc_rec_FuncSetEncryption (svcRec_BaseData_t *pstData, SvcRec_Msg_t *pstMsg);

#endif	// __INT_SVC_REC_H__
/* end of file */
