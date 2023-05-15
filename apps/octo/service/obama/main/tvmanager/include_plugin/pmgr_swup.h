/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  pmgr_swup.h
	@brief

	Description:			\n
	Module: 			 	\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	___PMGR_SWUP_H___
#define	___PMGR_SWUP_H___


/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#include <mgr_swup.h>
#include <svc_si.h>

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Plug-In Functions    ********************/
/*******************************************************************/

/*		Main
 */
HERROR			pmgr_swup_GetSpec (DxSwUpdate_Spec_e *peSwUpdateSpec);
DxSwUpdate_Spec_e pmgr_swup_GetMultipleSpec (DxBatchType_e eBatchType);

HERROR			pmgr_swup_SetSpec (DxSwUpdate_Spec_e eSwUpdateSpec);
HERROR			pmgr_swup_SetDefaultTp (void);

/*		Detect
 */
BUS_Callback_t	pmgr_swup_DetectGetAction (void);

/*		Download
 */
BUS_Callback_t	pmgr_swup_DownloadGetAction (DxSwUpdate_Spec_e eSwUpdateSpec);

#if 0
/*	Wake Up
 */
HERROR			pmgr_swup_WakeupStart(void);
HERROR			pmgr_swup_WakeupStop(void);
#endif

/*	usb ota
*/
HERROR			pmgr_swup_FindUsbSwImage(Handle_t hSWUpdate, HCHAR *szPath, SvcSwUpdate_UsbDetectInfo_t *pstContents);
HERROR			pmgr_swup_GetStartUpdateSource(DxBatchType_e eBatchType);

#endif /* !___PMGR_SWUP_H___ */

