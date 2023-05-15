/**
	@file     _mgr_dynamicupdate.h
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

#ifndef	__MGR_DYNAMICUPDATE_INT_H__
#define	__MGR_DYNAMICUPDATE_INT_H__

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
typedef struct tagDynamicUpdateContent
{
	HBOOL						bManageBgAction;
	HBOOL						bStartedDyUpdate;
	HBOOL						bOnOff;
	HBOOL						bStartTune;
	HUINT32						ulBGActionId;
	Handle_t						hBGAction;
	SvcSi_DynamicUpdateSpec_t		stDynamicUpdateSpec;
	SvcSi_DynamicUpdateOption_t	stDynamicUpdateOption; // not yet used.

	HUINT16						usUniqueId;
	DxTuneParam_t				stBgTuneInfo;

}DynamicUpdate_Content_t;

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/

BUS_Callback_t	xmgr_dynamicupdate_GetProc_Base(void);
#endif /* __MGR_DYNAMICUPDATE_INT_H__ */

