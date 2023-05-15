/**
	@file     ap_mheg_mgr.c
	@brief    MHEG engine 제어 application

	Description: MHEG엔진을 제어하는 application. Key와 system event를 받아 MHEG엔진이 필요한 형태로 가공하여 \n
				전달하고, MHEG이 발행하는 call 중 지연이나 통제가 필요한 이벤트들을 받아서 대신 처리한다.
	Module: AP/MHEG			 	\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <hlib.h>
#include <hapi.h>
#include <octo_common.h>
#include <bus.h>
#include <xmgr_mhegib_base.h>

//#include "./include/ap_mheg_mgr.h"
//#include "./include/ap_mheg_ib_mgr.h"


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
/******************* Static Function Prototypes  *************************/
/*******************************************************************/

/*******************************************************************/
/********************	Application Proc	*************************/
/*******************************************************************/

HERROR pmgr_mhegib_RequestFileOrData(HUINT16	 usSessionId,
									HUINT32		 nReqType,
									HUINT32		 ulReqNameLen,
									HUINT8		*paucReqName)
{
#if defined(CONFIG_SUPPORT_MHEG_IB)
	return XMGR_MhegIbManager_RequestFileOrData(usSessionId, nReqType, ulReqNameLen, paucReqName);
#else
	return ERR_FAIL;
#endif
}

HERROR pmgr_mhegib_RequestAppAbort(HUINT16 usSessionId, HUINT32 nAbortReqCode)
{
#if defined(CONFIG_SUPPORT_MHEG_IB)
	return XMGR_MhegIbManager_ReqAppAbort(usSessionId, nAbortReqCode);
#else
	return ERR_FAIL;
#endif
}


