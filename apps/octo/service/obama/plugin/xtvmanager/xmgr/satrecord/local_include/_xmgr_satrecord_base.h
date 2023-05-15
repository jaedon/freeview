/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  _xmgr_satrecord_base.h
	@brief	  satrecord 헤더 파일

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef __XMGR_SATRECORD_BASE_H__
#define __XMGR_SATRECORD_BASE_H__


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

typedef struct
{
	HINT32			nDsmccPid;
	DbSvc_TsInfo_t	stDetectTsInfo;
	
}xmgrSatrec_DetectInfo_t;

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/
BUS_Result_t xproc_satrecord_proc_Base(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t xproc_satrecord_DetectPID_Base(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
int 		XMGR_SatRecord_DBG_MakeSchedule(void);
HERROR		XMGR_SatRecord_RescheduleBatch_Base(void);

#endif /* __XMGR_SATRECORD_BASE_H__ */

