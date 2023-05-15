/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  _xmgr_live_zap.h
	@brief

	Description:			\n
	Module: 			 	\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	___XMGR_LIVE_ZAP_H___
#define	___XMGR_LIVE_ZAP_H___

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
//#include <svc_cas.h>

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct
{
	Handle_t			hSvc;				/**< 이 context가 zapping중인 service handle */
	HBOOL				bSvcLockPassed;		/**< 이 서비스의 service lock이 pass된 기록. */
} XmgrZapInfo_t;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/
XmgrZapInfo_t *XMGR_ZAP_GetZapInfo(HUINT32 ulActionId);


void	XMGR_ZAP_Init_Base(void);
void	XMGR_ZAP_ResetZappingInfo (HBOOL bLoadSetup);
void	XMGR_ZAP_RegisterLastSvcInfo (Handle_t hSvc);
HERROR	XMGR_ZAP_GetPrevSvcHandle (Handle_t *phPrevSvc);
HERROR	XMGR_ZAP_GetLastSvcHandle (MgrLive_LastSvcItem_t eLastSvcItem, Handle_t *phLastSvc);
HBOOL	XMGR_ZAP_IsSvcLockPassed (HUINT32 ulActionId, Handle_t hSvc);
HERROR	XMGR_ZAP_SetSvcLockPassed (HUINT32 ulActionId, Handle_t hSvc);
HERROR	XMGR_ZAP_ResetSvcLockPassed (HUINT32 ulActionId);
HERROR	XMGR_ZAP_GetCurrentSvcInfo (HUINT32 ulActionId, Handle_t *phCurSvc);
HERROR	XMGR_ZAP_SetCurrentSvcInfo (HUINT32 ulActionId, Handle_t hCurSvc);

#endif /* !___XMGR_LIVE_ZAP_H___ */

