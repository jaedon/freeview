/**
	@file     file_name.c File Name을 적습니다.
	@brief    file_name & simple comment.

	Description: File에 대한 설명을 적습니다.		\n
	Module: Layer와 모듈 명을 적습니다.			 	\n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <xsvc_epg.h>
#include "_svc_epg_common.h"
#include "_svc_epg_db_raw_lib.h"
#include "_svc_epg_context_mgr.h"
#include "_svc_epg_raw_mgr.h"
#include "../local_include/_xsvc_epg.h"

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

#define ____GLOBAL_FUNC____

#define ____LOCAL_FUNC____

HERROR xsvc_epg_Init_Ir4000hd(void)
{
	HERROR		hErr;
	svcEpg_Callback_t	stEpgCb;

	HxSTD_memset(&stEpgCb, 0, sizeof(svcEpg_Callback_t));

	stEpgCb.pfnEpgInitAction	= xsvc_epg_InitAction_Base;
	stEpgCb.pfnEpgStart		= xsvc_epg_Start_Base;
	stEpgCb.pfnEpgStop		= xsvc_epg_Stop_Base;
	stEpgCb.pfnEpgLoadData	= xsvc_epg_LoadData_Base;
	stEpgCb.pfnEpgUnLoadData	= xsvc_epg_UnLoadData_Base;
	stEpgCb.pfnEpgStartContent	= xsvc_epg_StartContent_Base;
	stEpgCb.pfnEpgStopContent	= xsvc_epg_StopContent_Base;

	hErr = SVC_EPG_RegisterCallback(eEPG_KIND_DVB, &stEpgCb);

	return hErr;
}

HERROR xsvc_epg_InitResource_Ir4000hd(void)
{
	HERROR				hError = ERR_EPG_FAIL;

	hError = SVC_EPGSECTION_CreateSvcSectionManager();
	if(hError != ERR_EPG_OK)
	{
		HxLOG_Print("[xsvc_epg_InitResource_Ir4000hd] EPGDB_CreateSvcSectionManager fail!!\n");
		goto EXIT_EPGMAIN_FUNC;
	}
	hError = SVC_EPGCONTEXT_InitContext((void*)xsvc_epgcontext_PfProcessEitCallBack_Base,
							(void*)xsvc_epgcontext_PfProcessEitCallBack_Base,
							(void*)xsvc_epgcontext_SchProcessEitCallBack_Base,
							(void*)xsvc_epgcontext_SchProcessEitCallBack_Base);
	if(hError != ERR_EPG_OK)
	{
		HxLOG_Print("[xsvc_epg_InitResource_Ir4000hd] SVC_EPGCONTEXT_InitContext fail!!\n");
		goto EXIT_EPGMAIN_FUNC;
	}

	hError = ERR_EPG_OK;

EXIT_EPGMAIN_FUNC :

	return hError;
}

/*********************** End of File ******************************/
