/**
	@file     _mgr_epgbgtracking.h
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

#ifndef	__MGR_EPGBGTRACKING_INT_H__
#define	__MGR_EPGBGTRACKING_INT_H__

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
typedef struct
{
	HUINT32				ulActionId;
	Handle_t				hAction;

	Handle_t				hTrackService;
	DbSvc_TsInfo_t			stTsInfo;
	DxTuneParam_t		stTuningInfo;
} EpgBGTracking_Content_t;


/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/


#endif /* __MGR_EPGBGTRACKING_INT_H__ */

