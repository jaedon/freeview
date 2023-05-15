/**
	@file     _mgr_liveota.h
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

#ifndef	__MGR_LIVEOTA_INT_H__
#define	__MGR_LIVEOTA_INT_H__

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
typedef struct tagLiveOtaContent
{
	HBOOL						bManageBgAction;

	HUINT32						ulBGActionId;
	Handle_t					hBGAction;

	SvcSi_LiveSwupSpec_t		stSiLiveSwupSpec;
	SvcSi_LiveSwupOption_t		stSiLiveSwupOption; // not yet used.

	HUINT16						usUniqueId;
	DxTuneParam_t				stBgTuneInfo;

} xmgrLiveOta_Context_t;

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/


#endif /* __MGR_LIVEOTA_INT_H__ */

