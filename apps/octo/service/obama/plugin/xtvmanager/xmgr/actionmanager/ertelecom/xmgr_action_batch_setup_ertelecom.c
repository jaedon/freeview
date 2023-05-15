/****************************************************************************
*
* File Name   : xmgr_action_batch_setup_mostelecom.c
*
* Description : 사양별 background job 등 power down전 shutdown 작업 수행 proc. Shut down 동작이 아무것도 없는 것이 기본.
*
*
* Revision History	:
*
* Date			Modification				Name
* -----------	-------------------------------------	------------------
*
****************************************************************************/


/* ======================================================================== */
/* Include. 																*/
/* ======================================================================== */
/** @brief global header file  */
#include <octo_common.h>

#include <svc_pipe.h>
#include <svc_sys.h>
#include <db_param.h>
#include <bus.h>


#include <mgr_action.h>
#include <mgr_common.h>
#include <svc_resources.h>
#include <mgr_rscmap.h>

#include <mgr_swup.h>
#include <mgr_emmupdate.h>
#include <mgr_search.h>
#include <mgr_epg.h>
#include <mgr_time.h>
#include <mgr_schedule.h>
#include "../local_include/_xmgr_action_batch.h"



STATIC MgrAction_BatchInitInfo s_stBatchInfoColdBoot[] =
{
		{eDxBAT_PROC_NONE, 			NULL, NULL},
		{eDxBAT_PROC_MAX, 			NULL, NULL}
};

STATIC MgrAction_BatchInitInfo s_stBatchInfoUser[] =
{
		{eDxBAT_PROC_NONE,			NULL, NULL},
		{eDxBAT_PROC_MAX,			NULL, NULL}
};

STATIC MgrAction_BatchInitInfo s_stBatchInfoStandby[] =
{
		{eDxBAT_PROC_NONE, 			NULL, NULL},
	#if defined(CONFIG_APCORE_EMM_UPDATE)
		{eDxBAT_PROC_EMMUPDATE, 	MGR_EMMUpdate_GetBatchCBList, MGR_EMMUpdate_GetBatchTime},
	#endif
	#if defined(CONFIG_OTA_DETECT_BG)
		{eDxBAT_PROC_OTACHECK, 		NULL, NULL},
	#endif
		{eDxBAT_PROC_MAX, 			NULL, NULL}
};

HUINT32 xmgr_action_BatchCount_Ertelecom(DxBatchType_e eBatchType)
{
	if(eBatchType == eDxBAT_TYPE_COLDBOOT)
		return sizeof(s_stBatchInfoColdBoot) / sizeof(MgrAction_BatchInitInfo);
	else if(eBatchType == eDxBAT_TYPE_USER)
		return sizeof(s_stBatchInfoUser) / sizeof(MgrAction_BatchInitInfo);
	else if(eBatchType == eDxBAT_TYPE_STANDBY)
		return sizeof(s_stBatchInfoStandby) / sizeof(MgrAction_BatchInitInfo);
	else
		return 0;
}

MgrAction_BatchInitInfo * xmgr_action_BatchGetProcInfo_Ertelecom(DxBatchType_e eBatchType)
{
	if(eBatchType == eDxBAT_TYPE_COLDBOOT)
		return s_stBatchInfoColdBoot;
	else if(eBatchType == eDxBAT_TYPE_USER)
		return s_stBatchInfoUser;
	else if(eBatchType == eDxBAT_TYPE_STANDBY)
		return s_stBatchInfoStandby;
	else
		return NULL;
}

HBOOL xmgr_action_BatchIsSerial_Ertelecom(DxBatchType_e eBatchType)
{
	if(eBatchType == eDxBAT_TYPE_COLDBOOT)
		return FALSE;
	else if(eBatchType == eDxBAT_TYPE_USER)
		return FALSE;
	else if(eBatchType == eDxBAT_TYPE_STANDBY)
		return FALSE;
	else
		return FALSE;
}

/* End of File. ---------------------------------------------------------- */



