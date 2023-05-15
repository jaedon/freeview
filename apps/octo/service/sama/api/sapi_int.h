/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  sapi_int.h
	@brief

	Description:  									\n
	Module: SAMA		\n

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

#ifndef __SAPI_INT_H__
#define __SAPI_INT_H__

#ifdef __cplusplus
extern "C" {
#endif
/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <sama_lib.h>

/*******************************************************************/
/********************      Declaration         *************************/
/*******************************************************************/
#define SAPI_SLOTID_NULL			0xFFFF

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

typedef struct
{
	void				*pvUserData;
	SAPI_NotiFunc_t		 pfNotifier;
} sapi_NotifyItem_t;

typedef struct
{
	HUINT32					 ulSemId;
	HINT32					 nSocket;
	HINT32					 nRpcHandle;
	HINT32					 nPid;
	HUINT32					ulTaskId;
	HBOOL					bSapi_Inited;

	HxList_t				*pstNotifierList;			// sapi_NotifyItem_t

	HBOOL					 bScheduleLoaded;

	// Available Slots
	HUINT32					 ulAvailSlotNum;
	HUINT32					 aulAvailSlots[SAMA_EMPTY_SLOT_PER_PROCESS];

	sama_ConflictCheck_t		stSamaConflictCheck;
} sapi_Inst_t;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Private Functions     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Member Functions       ******************/
/*******************************************************************/
// sapi.c
extern sapi_Inst_t*		sapi_GetInstance (void);
extern HERROR			sapi_GetConflictResult (sapi_Inst_t *pstInst, HUINT32 ulSlotId, SAPI_Conflict_t *pstConflict);
extern void 			sapi_ReleaseInstance (sapi_Inst_t *pstInst);
extern void				sapi_ExecuteNotifiers (sapi_Inst_t *pstInst, SAPI_NotiInfo_t *pstNotify);
extern HERROR			sapi_AppendScheduleToCache (sapi_Inst_t *pstInst, DxSchedule_t *pstSchedule);
extern DxSchedule_t*	sapi_FindScheduleFromCache (sapi_Inst_t *pstInst, HUINT32 ulSlot);
extern HERROR			sapi_RemoveScheduleFromCache (sapi_Inst_t *pstInst, HUINT32 ulSlot);
extern HERROR			sapi_PrintSchedule(DxSchedule_t *pstSchedule);
extern HERROR 			sapi_PrintAllSchedules (void);
// sapi_protocol_base.c
extern HERROR			sapi_base_RegisterRpcCallback (sapi_Inst_t *pstInst);

extern HERROR			sapi_base_CmdInit (sapi_Inst_t *pstInst);
extern HERROR			sapi_base_CmdTerm (sapi_Inst_t *pstInst);
extern HERROR			sapi_base_CmdSetRscInfo (sapi_Inst_t *pstInst, SAPI_RscInfo_t *pstRscInfo);
extern HERROR			sapi_base_CmdSetRfInputAbility (sapi_Inst_t *pstInst, HUINT32 ulRfInputId, HBOOL bAbility);
extern HERROR			sapi_base_CmdRefreshScheduleData (sapi_Inst_t *pstInst);
extern HERROR			sapi_base_CmdMakeSchedule (sapi_Inst_t *pstInst, HUINT32 ulSlotId, DxSchedule_t *pstSchedule, HINT32 *pnRequestId);
extern HERROR			sapi_base_CmdUpdateSchedule (sapi_Inst_t *pstInst, HUINT32 ulSlotId, DxSchedule_t *pstSchedule, HINT32 *pnRequestId);
extern HERROR			sapi_base_CmdCancelSchedule (sapi_Inst_t *pstInst, HUINT32 ulSlotId, HINT32 *pnRequestId);
extern HERROR			sapi_base_CmdChangeSeriesScheduleToNext (sapi_Inst_t *pstInst, HUINT32 ulSlotId, HINT32 *pnRequestId);
extern HERROR			sapi_base_CmdStopSchedule (sapi_Inst_t *pstInst, HUINT32 ulSlotId, HINT32 *pnRequestId);
extern HERROR			sapi_base_CmdPauseSchedule (sapi_Inst_t *pstInst, HUINT32 ulSlotId, HINT32 *pnRequestId);
extern HERROR			sapi_base_CmdResumeSchedule (sapi_Inst_t *pstInst, HUINT32 ulSlotId, HINT32 *pnRequestId);

extern HERROR 			sapi_base_CmdIncompleteSchedule (sapi_Inst_t *pstInst, HUINT32 ulSlotId, DxSchedule_RemovedInfo_t *pstRemovedInfo, HINT32 *pnRequestId);
extern HERROR			sapi_base_CmdInformConflictAddTried (sapi_Inst_t *pstInst, SAPI_Conflict_t *pstConflict, HINT32 *pnRequestId);
extern HERROR			sapi_base_CmdChangePaddingTime (sapi_Inst_t *pstInst, HUINT32 ulOffsetTime, HBOOL bIsPreOffset, HINT32 *pnRequestId);

extern HERROR			sapi_base_OnResourceInfoSet (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData);
extern HERROR			sapi_base_OnRfInputAbilitySet (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData);
extern HERROR			sapi_base_OnAvailableSlotsChanged (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData);
extern HERROR			sapi_base_OnSchedulesLoaded (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData);
extern HERROR			sapi_base_OnScheduleAdded (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData);
extern HERROR			sapi_base_OnScheduleChanged (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData);
extern HERROR 			sapi_base_OnScheduleChangedEx (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData);
extern HERROR			sapi_base_OnScheduleRemoved (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData);
extern HERROR			sapi_base_OnSchedulePaused (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData);
extern HERROR			sapi_base_OnScheduleResumed (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData);
extern HERROR			sapi_base_OnConflictAddTried (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData);
extern HERROR			sapi_base_OnTimeUp (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData);
extern HERROR			sapi_base_OnResourceRefreshed (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData);

// sapi_protocol_TvAnyTime.c
extern HERROR			sapi_tva_RegisterRpcCallback (sapi_Inst_t *pstInst);
extern HERROR			sapi_tva_CmdUpdateSeries (sapi_Inst_t *pstInst, HINT32 *pnRequestId);
extern HERROR			sapi_tva_CmdCancelWholeSeries (sapi_Inst_t *pstInst, HUINT32 ulSlotId, HINT32 *pnRequestId);
extern HERROR			sapi_tva_OnAlternativeAddTried (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData);
extern HERROR			sapi_tva_OnSystemTimeChanged (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData);



#ifdef __cplusplus
}
#endif
#endif	// __SAPI_INT_H__
