/**
	@file     mgr_epg.h
	@brief    file_name & simple comment.

	Description: \n
	Module:  \n
	Remarks : \n
	 Date			Modification				Name\n
	-----------	-------------------------------------	------------------\n
	 2013/03/27		initiated					chmin\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	__MGR_EPG_H__
#define	__MGR_EPG_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#include <mgr_action.h>

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

enum
{
	eMEVT_EPG			= eMEVT_EPG_START,

	/* - Description : EPG download or gathering done alarm.
	   - Parameters Usage :
		   handle : N.A.
		   p1 : MgrEpg_Kind_e
		   p2 : TBD
		   p3 : TBD */
	eMEVT_EPG_PROCESS_DONE,

	/* - Description : 특정 Application에서 Application한정적으로 필요한 메시지를 보낼때 사용하기 위한 메시지.
	   - Parameters Usage :
		   handle : NULL.
		   p1 : Message Class
		   p2 : sub message kind.
		   p3 : parameter */
	eMEVT_EPG_USER_COMMAND,
};

typedef enum
{
	eEpgKind_HumaxIp = 0,
	eEpgKind_TVTV,
	eEpgKind_RF
} MgrEpg_Kind_e;

typedef enum
{
	eEpgStartType_Normal,
	eEpgStartType_Parsing
} MgrEpg_ProcessType_e;

typedef enum
{
	eEpgCallType_Nothing,
	eEpgCallType_Shutdown, 		/**< shutdown call */
	eEpgCallType_NomalAction,		/**< nomal action call(by reservation) */
	eEpgCallType_StandbyAction,	/**< standby action call(by reservation) */
	eEpgCallType_FormatAction		/**< format action call(by format, unloading IP EPG) */
} MgrEpg_CallType_e;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/

/*		EPG Action
 */
HERROR			MGR_EPG_Start (MgrEpg_Kind_e eType, MgrEpg_ProcessType_e eProcessType, MgrEpg_CallType_e eCallType);
HERROR			MGR_EPG_Stop (MgrEpg_Kind_e eType);


BUS_Callback_t		MGR_EPG_InitBatch(void **ppvUserData, HUINT32 *pulUserDataLength, DxBatchType_e eBatchType);
HERROR			MGR_EPG_DeinitBatch(void *pvUserData, HUINT32 ulUserDataLength, DxBatchType_e eBatchType);
HERROR			MGR_EPG_RescheduleBatch(DxBatchType_e eBatchType);
HERROR 			MGR_EPG_GetBatchEpgCBList(MgrAction_BatchProcCBList_t * fnBatchProcCBList);
HERROR 			MGR_EPG_GetBatchEpgTime(UNIXTIME *ulStartTime, HUINT32 *ulDuration);


#endif /* __MGR_EPG_H__ */

