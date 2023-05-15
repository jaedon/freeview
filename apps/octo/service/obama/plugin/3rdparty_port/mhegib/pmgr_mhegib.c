/**
	@file     ap_mheg_mgr.c
	@brief    MHEG engine ���� application

	Description: MHEG������ �����ϴ� application. Key�� system event�� �޾� MHEG������ �ʿ��� ���·� �����Ͽ� \n
				�����ϰ�, MHEG�� �����ϴ� call �� �����̳� ������ �ʿ��� �̺�Ʈ���� �޾Ƽ� ��� ó���Ѵ�.
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


