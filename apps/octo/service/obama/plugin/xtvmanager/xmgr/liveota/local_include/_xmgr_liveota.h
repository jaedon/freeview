/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  _xmgr_liveota.h
	@brief

	Description:			\n
	Module: 			 	\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	___XMGR_LIVEOTA_INT_H___
#define	___XMGR_LIVEOTA_INT_H___

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <mgr_liveota.h>
#include <_mgr_liveota.h>

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
/********************      Public Functions     ********************/
/*******************************************************************/

/*	Common Functions
 */
//extern HERROR 	xmgr_liveota_UpdateServiceInfo(Handle_t hSvc, DbSvc_Info_t *pSvcInfoOld, DbSvc_Info_t *pSvcInfoNew);
extern HBOOL 	xmgr_liveota_CheckSvcTypeOption (DxSvcType_e eSvcTypeInOption, DxSvcType_e eSvcType);
extern HERROR 	xmgr_liveota_CheckActionId (xmgrLiveOta_Context_t *pstContents, Handle_t hAction);
extern HERROR 	xmgr_liveota_MsgAmActionSetup (xmgrLiveOta_Context_t *pstContents, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
extern HERROR 	xmgr_liveota_MsgAmActionSucceeded (xmgrLiveOta_Context_t *pstContents, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
extern HERROR 	xmgr_liveota_MsgAmActionTaken (xmgrLiveOta_Context_t *pstContents, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
extern HERROR 	xmgr_liveota_EvtChLocked (xmgrLiveOta_Context_t *pstContents, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
extern HERROR 	xmgr_liveota_EvtSiDynamicUpdateDone (xmgrLiveOta_Context_t *pstContents, HBOOL bUpdated);


/*	Base Functions
 */
extern HERROR	xmgr_liveota_Start(void);
extern HERROR	xmgr_liveota_Stop(void);
extern BUS_Callback_t xmgr_liveota_GetProc(void);

#endif /* !___XMGR_LIVEOTA_INT_H___ */

