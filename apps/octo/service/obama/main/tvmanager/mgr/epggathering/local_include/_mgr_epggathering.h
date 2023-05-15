/**
	@file     _mgr_epggathering.h
	@brief    file_name & simple comment.

	Description :			\n
	Module :				\n
	Remarks :				\n
	 Date			Modification				Name\n
	-----------	-------------------------------------	------------------\n
	 2013/04/18		initiated					chmin\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	__MGR_EPGGATHERING_INT_H__
#define	__MGR_EPGGATHERING_INT_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

#include <octo_common.h>
#include <mgr_common.h>
#include <svc_si.h>

#include <bus.h>



/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct tagEpgGatheringContent
{
	HBOOL						bManageBgAction;
	HBOOL						bStartedEpgGathering;

	HUINT32						ulBGActionId;
	Handle_t						hBGAction;

	DbSvc_TsInfo_t					*pstTsInfo;
	HINT32						nTsCnt;

	HINT32						nCurrentTsIndex;
	Handle_t						hSvcInCurrentTs;

	HUINT16						usUniqueId;
	DxTuneParam_t				stBgTuneInfo;


}EpgGathering_Content_t;

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/


#endif /* __MGR_EPGGATHERING_INT_H__ */

