/**
@file     file_name.c File Name을 적습니다.
@brief    file_name & simple comment.

Description: File에 대한 설명을 적습니다.		\n
Module: Layer와 모듈 명을 적습니다.			 	\n
Remarks : 										\n

Copyright (c) 2013 HUMAX Co., Ltd.				\n
All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>

#include <bus.h>


#include <mgr_epg.h>
#include <pmgr_epg.h>

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************     Macro         *************************/
/******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

HERROR MGR_EPG_Start (MgrEpg_Kind_e eType, MgrEpg_ProcessType_e eProcessType, MgrEpg_CallType_e eCallType)
{
	return pmgr_epg_Start(eType, eProcessType, eCallType);
}

HERROR MGR_EPG_Stop (MgrEpg_Kind_e eType)
{
	return pmgr_epg_Stop(eType);
}

BUS_Callback_t			MGR_EPG_InitBatch(void **ppvUserData, HUINT32 *pulUserDataLength, DxBatchType_e eBatchType)
{
	return pmgr_epg_InitBatch(ppvUserData, pulUserDataLength);
}

HERROR			MGR_EPG_DeinitBatch(void *pvUserData, HUINT32 ulUserDataLength, DxBatchType_e eBatchType)
{
	return pmgr_epg_DeinitBatch(pvUserData, ulUserDataLength);
}

HERROR			MGR_EPG_RescheduleBatch(DxBatchType_e eBatchType)
{
	return pmgr_epg_RescheduleBatch();
}
HERROR MGR_EPG_GetBatchEpgCBList(MgrAction_BatchProcCBList_t * fnBatchProcCBList)
{
	return pmgr_epg_GetBatchEpgCBList(fnBatchProcCBList);
}

HERROR MGR_EPG_GetBatchEpgTime(UNIXTIME *ulStartTime, HUINT32 *ulDuration)
{
	HxLOG_Trace();
	return ERR_OK;
}



