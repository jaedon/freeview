/**
	@file     mgr_emmupdate.c
	@brief    file_name & simple comment.

	Description: for portal UI \n
	Module: Application(Layer), Menu(module) \n
	Remarks : \n
	 Date			Modification				Name\n
	-----------	-------------------------------------	------------------\n
	 2009/07/11		modified					jhlee\n

	Copyright (c) 2009 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>

#include <svc_pipe.h>

#include <bus.h>


#include <mgr_action.h>
#include <mgr_live.h>
#include <mgr_common.h>

#include <svc_resources.h>
#include <svc_ch.h>
#include <svc_si.h>
#include <svc_epg.h>
#include <svc_cas.h>

#include <svc_resources.h>
#include <mgr_rscmap.h>
#include <mgr_emmupdate.h>

#include <db_svc.h>
#include "./local_include/_xmgr_emmupdate.h"

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

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
/********************      	Static Function 	********************/
/*******************************************************************/

HERROR	xmgr_emmupdate_GetConfiguration_Ziggo (HUINT32 *pulWakeupInterval, HUINT32 *pulStartMin, HUINT32 *pulUpdateDuration)
{
	HERROR 	hErr;

	hErr = xmgr_emmupdate_GetConfiguration_Base (pulWakeupInterval, NULL, pulUpdateDuration);

	if (pulStartMin)		*pulStartMin		= (SECONDS_PER_MIN * 50);	// After 50min
	if (pulUpdateDuration)	*pulUpdateDuration	= (SECONDS_PER_MIN * 10);	// 10Ка

	hErr = ERR_OK;

	return hErr;
}

BUS_Callback_t	xmgr_emmupdate_GetProc_Ziggo(void)
{
	return xmgr_emmupdate_GetProc_Base();
}

HERROR	xmgr_emmupdate_Start_Ziggo(void)
{
	BUS_Result_t		eRes = ERR_BUS_SOURCE_NULL;

	eRes = BUS_MGR_Create((char*)"proc_emmupdate_Action", APP_ACTION_PRIORITY, xmgr_emmupdate_GetProc_Ziggo(), HANDLE_NULL, 0, 0, 0);

	return (eRes != ERR_BUS_NO_ERROR) ? ERR_FAIL : ERR_OK;
}

HERROR	xmgr_emmupdate_Stop_Ziggo(void)
{
	BUS_Result_t		eRes = ERR_BUS_SOURCE_NULL;

	eRes = BUS_MGR_Destroy(xmgr_emmupdate_GetProc_Ziggo());

	return (eRes != ERR_BUS_NO_ERROR) ? ERR_FAIL : ERR_OK;
}

HERROR	xmgr_emmupdate_GetBatchCBList_Ziggo(MgrAction_BatchProcCBList_t * fnBatchProcCBList)
{
	return xmgr_emmupdate_GetBatchCBList_Base(fnBatchProcCBList);
}

HERROR	xmgr_emmupdate_RescheduleBatch_Ziggo(void)
{
	return xmgr_emmupdate_RescheduleBatch_Base();
}

/*********************** End of File ******************************/

