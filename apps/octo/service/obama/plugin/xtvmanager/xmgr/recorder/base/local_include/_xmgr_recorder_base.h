/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  _xmgr_recorder_base.h
	@brief	  internal player 헤더 파일

	Description: player internal use only. \n
	Module: XMGR Recorder			 	\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	__XMGR_RECORDER_BASE_H__
#define	__XMGR_RECORDER_BASE_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <mgr_recorder.h>

#include <_xmgr_recorder.h>

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
HERROR 				xmgr_rec_ClearContext (xmgrRec_Context_t *pstContext);
xmgrRec_Context_t*	xmgr_rec_GetContext (HUINT32 ulIndex);
void 				xmgr_rec_SetState(xmgrRec_Context_t *pstContext, MgrRec_State_e eState);
HERROR				xmgr_rec_StartRec(xmgrRec_Context_t *pstContext);
HERROR 				xmgr_rec_StartCas(xmgrRec_Context_t *pstContext);
HERROR			xmgr_rec_RecPause(Handle_t hAction, xmgrRec_PauseFlag_e ePauseFlag);
HERROR			xmgr_rec_RecResume(Handle_t hAction, xmgrRec_PauseFlag_e ePauseFlag);
HERROR 			xmgr_rec_GetServiceInfoFromSvc (Handle_t hSvc, SvcRec_Setup_t *pstSvcRecSetup);
HERROR			xmgr_rec_SetServiceTimeStamp (xmgrRec_Context_t *pstContext, SvcMeta_Service_t *pstMetaInfo, HBOOL bWriteMeta);
void			xmgr_rec_SetSubtitleInfo(SvcSi_SubtitleList_t *pInput, SvcMeta_SbtlInfoList_t *pstSbtlInfoList);

BUS_Result_t	xmgr_rec_MsgBusCreate_Base (xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t	xmgr_rec_MsgBusDestroy_Base (xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t	xmgr_rec_MsgBusTimer_Base (xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t  	xmgr_rec_MsgBusGoStandby_Base (xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t	xmgr_rec_MsgMgrStartRec_Base (xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t 	xmgr_rec_MsgMgrRecordChangeTsrRecToDelayedRecord_Base (xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t 	xmgr_rec_MsgMgrRecordEndtimeUpdate_Base(xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t	xmgr_rec_MsgEvtSiPmtReceived_Base( xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t	xmgr_rec_MsgEvtSiPmtTimeout_Base( xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t	xmgr_rec_MsgEvtSiPmtPidChanged_Base( xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t	xmgr_rec_MsgEvtSiEitReceived_Base( xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t	xmgr_rec_MsgEvtSiCatReceived_Base( xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t	xmgr_rec_MsgEvtSiEitTimeout_Base( xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t	xmgr_rec_MsgEvtSiMultifeedDetected_Base( xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t	xmgr_rec_MsgEvtSiSdtFreeCaModeChanged_Base( xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t	xmgr_rec_MsgEvtSiSdtRunningStatusChanged_Base( xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t	xmgr_rec_MsgEvtChLocked_Base (xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t	xmgr_rec_MsgEvtChNoSignal_Base( xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t	xmgr_rec_MsgEvtChMovingMotor_Base( xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t	xmgr_rec_MsgEvtDbCheckSvcDb_Base( xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t	xmgr_rec_MsgEvtRecPidChanged_Base( xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t	xmgr_rec_MsgEvtRecTimecodeChange_Base( xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t 	xmgr_rec_MsgEvtRecScrambled_Base( xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t	xmgr_rec_MsgEvtRecDelayedRecStatusChange_Base( xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t	xmgr_rec_MsgEvtRSVDBChanged_Base( xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t	xmgr_rec_MsgEvtReqCommand_Base( xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t	xmgr_rec_MsgEvtExtStorageDisconnected_Base( xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);

BUS_Result_t	xmgr_rec_CiPlusMsgEvtCasDrmSignalled_Base( xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t	xmgr_rec_CiPlusMsgEvtCasOK_Base ( xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t	xmgr_rec_CiPlusMsgEvtCasFail_Base ( xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t	xmgr_rec_CiPlusMsgEvtCasDrmUnknown_Base ( xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t	xmgr_rec_CiPlusMsgEvtCasDrmNone_Base( xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t 	xmgr_rec_CiPlusMsgEvtSiPmtReceived_Base( xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);

void			xmgr_rec_ClearWaitFlag(xmgrRec_Context_t *pstContext, xmgrRec_WaitFlag_e eWaitFlag);
void			xmgr_rec_SetWaitFlag(xmgrRec_Context_t *pstContext, xmgrRec_WaitFlag_e eWaitFlag);
void			xmgr_rec_ClearPauseFlag(xmgrRec_Context_t *pstContext, xmgrRec_PauseFlag_e ePauseFlag);
void			xmgr_rec_SetPauseFlag(xmgrRec_Context_t *pstContext, xmgrRec_PauseFlag_e ePauseFlag);
void			xmgr_rec_RunState (xmgrRec_Context_t *pstContext);
HERROR 			xmgr_rec_NagraProcessDrmInfoByDrmSignalled_Ses(xmgrRec_Context_t *pstContext, Handle_t hAction, HBOOL bChangedDrm);
BUS_Result_t	xmgr_rec_NagraMsgEvtCasDrmUnknown_Ses( xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
HERROR 			xmgr_rec_NagraMsgEvtCasOK_Ses(xmgrRec_Context_t *pstContext, Handle_t hAction);
HERROR 			xmgr_rec_NagraMsgEvtCasFail_Ses(xmgrRec_Context_t *pstContext, Handle_t hAction);
BUS_Result_t	xmgr_rec_NagraMsgEvtCasDrmNone_Ses( xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
HBOOL 			xmgr_rec_NagraIsReceivedNewCasDrmInfo(xmgrRec_Context_t *pstContext);
BUS_Result_t 	xmgr_rec_NagraMsgEvtCasDrmSignalled_Base( xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t	xmgr_rec_NagraMsgEvtCasOK_Base ( xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t	xmgr_rec_NagraMsgEvtCasFail_Base( xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t	xmgr_rec_NagraMsgEvtCasDrmUnknown_Base( xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t	xmgr_rec_NagraMsgEvtCasDrmNone_Base( xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);

BUS_Result_t 	xmgr_rec_IrdetoMsgEvtCasDrmNone_Base(xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t	xmgr_rec_IrdetoMsgEvtCasDrmUnknown_Base(xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t	xmgr_rec_IrdetoMsgEvtCasOK_Base(xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t	xmgr_rec_IrdetoMsgEvtCasFail_Base(xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t	xmgr_rec_IrdetoMsgEvtCasCmd_Base(xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);

/*
	debug functions
*/
void			xmgr_rec_DebugPrintRecPidInfo_Base(SvcRec_PIDList_t *pstPIDList, HCHAR *funcName, HINT32 nLine);

#endif /* __XMGR_RECORDER_BASE_H__ */

