/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  mgr_schedule.h
	@brief

	Description:  									\n
	Module: OBAMA / tvmanager / schedule		\n

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

#ifndef __MGR_SCHEDULE_H__
#define __MGR_SCHEDULE_H__

#ifdef __cplusplus
extern "C" {
#endif
/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <mgr_action.h>
/*******************************************************************/
/********************      Declaration         *************************/
/*******************************************************************/
enum
{
	eMEVT_SCHEDULE			= eMEVT_SCHEDULE_START,

	eMEVT_SCHEDULE_EVENT_OCCURED,
	/* - Description : Schedule is time up:
			   handle : N.A
			   p1 : ulSlotId
			   p2 : SAPI_TimerItemType_e
			   p3 : DxSchedule_t */
	eMEVT_SCHEDULE_TIME_UP,

	/* - Description : Schedule is time up:
			   handle : N.A
			   p1 : ulSlotId
			   p2 : N/A
			   p3 : DxSchedule_t */
	eMEVT_SCHEDULE_CANCELED,

//	eMEVT_SCHEDULE_EXPIRED,

	eMEVT_SCHEDULE_SATIP_CHANGEPID,
	eMEVT_SCHEDULE_NTLS_CHANGEPID,
	eMEVT_SCHEDULE_HLS_CHANGE_PLAY_POSITION,
	eMEVT_SCHEDULE_NEW_REC_ADDED,
	eMEVT_SCHEDULE_REC_CHANGE_INSTANCE,

	eMEVT_SCHEDULE_END
};

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

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
/********************      Public Functions       ******************/
/*******************************************************************/
extern HERROR	MGR_SCHEDULE_Init (void);
extern HERROR	MGR_SCHEDULE_SetScheduleInCancelDB(HUINT32 nRequestID, DxSchedule_t *pstSchedule);
extern HERROR	MGR_SCHEDULE_CheckFrontPannelDisplay (void);
extern HERROR	MGR_SCHEDULE_ReScheduleBurstUpdate (void);
extern HERROR	MGR_SCHEDULE_CheckConflictSchedule(void);
extern HERROR	MGR_SCHEDULE_StartDescRecordFinedItem(DxRecListData_t	*pstRecData);
extern Handle_t	MGR_SCHEDULE_GetEarliestTrackingSvc (void);
extern HERROR	MGR_SCHEDULE_MakeRegularOta (void);
extern HERROR	MGR_SCHEDULE_SetRecordFailReason (HUINT32 ulSlot, DxSchedule_RemovedReason_e eRemovedReason, HUINT32 eDetailedInfo);
extern void		MGR_SCHEDULE_UpdateAntennaConnectionInfo(void);

// // For Batch.
BUS_Callback_t			MGR_SCHEDULE_InitDescrambleRecordBatch(void **ppvUserData, HUINT32 *pulUserDataLength, DxBatchType_e eBatchType);
HERROR			MGR_SCHEDULE_DeinitDescrambleRecordBatch(void *pvUserData, HUINT32 ulUserDataLength, DxBatchType_e eBatchType);
HERROR			MGR_SCHEDULE_RescheduleDescrambleRecordBatch(DxBatchType_e eBatchType);
HERROR MGR_SCHEDULE_GetBatchDescrambleRecordCBList(MgrAction_BatchProcCBList_t * fnBatchProcCBList);
HERROR MGR_SCHEDULE_GetBatchDescrambleRecordTime(UNIXTIME *ulStartTime, HUINT32 *ulDuration);


#ifdef __cplusplus
}
#endif
#endif	// __MGR_SCHEDULE_H__
