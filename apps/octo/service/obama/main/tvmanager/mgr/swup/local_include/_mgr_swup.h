/**
	@file     _mgr_swup.h
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

#ifndef	__MGR_SWUP_INT_H__
#define	__MGR_SWUP_INT_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

#include <octo_common.h>
#include <mgr_common.h>

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

// TODO: 각 plugin에 있는 common type들을 옮겨온다.
typedef	struct
{
	DxBatchType_e	eBatchProcess;
} MgrSwup_BaseContents_t;


/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/

BUS_Callback_t	mgr_swup_DetectGetAction (DxSwUpdate_Source_e eSource);

BUS_Callback_t	mgr_swup_DownloadGetAction (DxSwUpdate_Source_e eSource);

BUS_Callback_t	mgr_swup_InstallGetProc (void);

BUS_Callback_t	mgr_swup_IpGetProc(void);

#endif /* __MGR_SWUP_INT_H__ */

