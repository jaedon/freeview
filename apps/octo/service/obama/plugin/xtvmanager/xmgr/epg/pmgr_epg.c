/**
	@file     pmgr_epg.c File Name을 적습니다.
	@brief    file_name & simple comment.

	Description: Network 을 통한 humax epg를 managering 한다.	\n
	Module: APP                                 			 	\n
	Remarks : 													\n

	Copyright (c) 2013 HUMAX Co., Ltd.							\n
	All rights reserved.										\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#include <mgr_epg.h>
#include <xmgr_epg.h>
#include "./local_include/_xmgr_epg.h"


HERROR	pmgr_epg_Start (MgrEpg_Kind_e eType, MgrEpg_ProcessType_e eProcessType, MgrEpg_CallType_e eCallType)
{
	HERROR		hErr = ERR_FAIL;

	switch (eType)
	{
	case eEpgKind_RF:
		hErr = xmgr_epg_RfStart_Base();
		break;
	case eEpgKind_HumaxIp:
#if defined(CONFIG_APCORE_IP_EPG)
#if defined(CONFIG_OP_UK_DTT)
		hErr = xmgr_epg_IpHumaxStart_Ukdtt();
#else
		hErr = xmgr_epg_IpHumaxStart_Base();
#endif
#endif
		break;
	case eEpgKind_TVTV:
#if defined(CONFIG_MW_EPG_TVTV)
		hErr = xmgr_epg_TvtvStart_Base(eProcessType, eCallType);
#endif
		break;
	default:
		break;
	}

	return hErr;
}

HERROR	pmgr_epg_Stop (MgrEpg_Kind_e eType)
{
	HERROR		hErr = ERR_FAIL;

	switch (eType)
	{
	case eEpgKind_HumaxIp:
		break;
	case eEpgKind_TVTV:
#if defined(CONFIG_MW_EPG_TVTV)
		return xmgr_epg_TvtvStop_Base();
#endif
		break;
	default:
		break;
	}

	return hErr;
}

BUS_Callback_t			pmgr_epg_InitBatch(void **ppvUserData, HUINT32 *pulUserDataLength)
{
#if defined(CONFIG_MW_EPG_TVTV)
	return XMGR_EPG_InitBatchTvtv_Base(ppvUserData, pulUserDataLength);
#else
	return NULL;
#endif
}

HERROR			pmgr_epg_DeinitBatch(void *pvUserData, HUINT32 ulUserDataLength)
{
#if defined(CONFIG_MW_EPG_TVTV)
	return XMGR_EPG_DeinitBatchTvtv_Base(pvUserData, ulUserDataLength);
#else
	return ERR_FAIL;
#endif
}

HERROR			pmgr_epg_RescheduleBatch(void)
{
#if defined(CONFIG_MW_EPG_TVTV)
	return XMGR_EPG_RescheduleBatchTvtv_Base();
#else
	return ERR_FAIL;
#endif

}

HERROR 			pmgr_epg_GetBatchEpgCBList(MgrAction_BatchProcCBList_t * fnBatchProcCBList)
{
#if defined(CONFIG_MW_EPG_TVTV)
	return XMGR_EPG_GetBatchEpgCBListTvtv_Base(fnBatchProcCBList);
#else
	fnBatchProcCBList->fnAppInit = NULL;
	fnBatchProcCBList->fnAppDeinit = NULL;
	fnBatchProcCBList->fnAppReSchedule = NULL;

	return ERR_FAIL;
#endif
}

/*********************** End of File ******************************/
