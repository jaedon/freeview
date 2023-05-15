/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  xmgr_epg.h
	@brief

	Description: main main procedure 관련 내용들을 분류한 모듈이다. \n
	Module:
	Remarks : 										\n

	Copyright (c) 2011 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	__XMGR_EPG_H__
#define	__XMGR_EPG_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <linkedlist.h>
#include <octo_common.h>

/*******************************************************************/
// DB
/*******************************************************************/
#include <db_svc.h>
#include <db_param.h>

/*******************************************************************/
// MW
/*******************************************************************/
#include <svc_pipe.h>
#include <svc_si.h>
#include <svc_resources.h>
#include <isosvc.h>

#include <mgr_action.h>

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/
#if defined(CONFIG_MW_EPG_TVTV)
BUS_Callback_t		XMGR_EPG_TVTV_GetProc (void);
BUS_Callback_t			XMGR_EPG_InitBatchTvtv_Base(void **ppvUserData, HUINT32 *pulUserDataLength);
HERROR			XMGR_EPG_DeinitBatchTvtv_Base(void *pvUserData, HUINT32 ulUserDataLength);
HERROR			XMGR_EPG_RescheduleBatchTvtv_Base(void);
HERROR			XMGR_EPG_GetBatchEpgCBListTvtv_Base(MgrAction_BatchProcCBList_t * fnBatchProcCBList);
#endif

#if defined(CONFIG_OP_FREESAT)
HERROR				XMGR_FSAT_StartEITManager(void);
HERROR				XMGR_FSAT_StopEITManager(void);
HERROR				XMGR_FSAT_DefaultMetaDataFolder(void);
HERROR				XMGR_FSAT_StopAllBGJobsNow(void);
HERROR				XMGR_FSAT_ReceiverDataCheck(void);
#endif


#endif	/*	!__XMGR_EPG_H__	*/
