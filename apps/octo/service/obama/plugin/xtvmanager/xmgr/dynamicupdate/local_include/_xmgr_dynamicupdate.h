/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  _xmgr_dynamicupdate.h
	@brief

	Description:			\n
	Module: 			 	\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	___XMGR_DYNAMICUPDATE_INT_H___
#define	___XMGR_DYNAMICUPDATE_INT_H___

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <mgr_dynamicupdate.h>
#include <_mgr_dynamicupdate.h>

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

typedef enum
{
	eDYNAMICUPDATE_INIT,
	eDYNAMICUPDATE_EXCLUDE,
	eDYNAMICUPDATE_IDENTICAL,
	eDYNAMICUPDATE_DELETE,
	eDYNAMICUPDATE_ADD,
	eDYNAMICUPDATE_UPDATE,
} DYNAMICUPDATE_UpdateFlag_e;




/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/

/*	Common Functions
 */
HERROR xmgr_dynamicupdate_updateServiceInfo(Handle_t hSvc, DbSvc_Info_t *pSvcInfoOld, DbSvc_Info_t *pSvcInfoNew);
HBOOL 	xmgr_dynamicupdate_checkSvcTypeOption (DxSvcType_e eSvcTypeInOption, DxSvcType_e eSvcType);
HERROR xmgr_dynamicupdate_checkActionId (DynamicUpdate_Content_t *pstContents, Handle_t hAction);
HERROR xmgr_dynamicupdate_mevtLiveNotifySvcStopped(DynamicUpdate_Content_t *pstContents, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
HERROR xmgr_dynamicupdate_mevtLiveNotifySvcChanged(DynamicUpdate_Content_t *pstContents, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
HERROR xmgr_dynamicupdate_evtChLocked (DynamicUpdate_Content_t *pstContents, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
HERROR xmgr_dynamicupdate_evtSiDynamicUpdateDone (DynamicUpdate_Content_t *pstContents, HBOOL bUpdated);
HERROR xmgr_dynamicupdate_stopSiDynamicUpdate (DynamicUpdate_Content_t *pstContents);
HERROR xmgr_dynamicupdate_setupBackGroundAction (DynamicUpdate_Content_t *pstContents);
HERROR xmgr_dynamicupdate_releaseBackGroundAction (DynamicUpdate_Content_t *pstContents);
HERROR xmgr_dynamicupdate_checkOnOff (DynamicUpdate_Content_t *pstContents);



/*	Base Functions
 */
HERROR	xmgr_dynamicupdate_Start_Base(void);
HERROR	xmgr_dynamicupdate_Stop_Base(void);




HERROR	xmgr_dynamicupdate_Start_Ausdtt(void);
HERROR	xmgr_dynamicupdate_Stop_Ausdtt(void);
BUS_Callback_t xmgr_dynamicupdate_GetProc_Ausdtt(void);

HERROR	xmgr_dynamicupdate_Start_Ukdtt(void);
HERROR	xmgr_dynamicupdate_Stop_Ukdtt(void);
BUS_Callback_t xmgr_dynamicupdate_GetProc_Ukdtt(void);


HERROR	xmgr_dynamicupdate_Start_Nordig(void);
HERROR	xmgr_dynamicupdate_Stop_Nordig(void);
HERROR	xmgr_dynamicupdate_SvcStop_Nordig(Handle_t hAction);
BUS_Callback_t xmgr_dynamicupdate_GetProc_Nordig(void);


#endif /* !___XMGR_DYNAMICUPDATE_INT_H___ */

