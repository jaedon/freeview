/**
	@file     _xmgr_search.h
	@brief    file_name & simple comment.

	Description :			\n
	Module :				\n
	Remarks :				\n
	 Date			Modification				Name\n
	-----------	-------------------------------------	------------------\n
	 2013/04/12		initiated					moonsg\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	__XMGR_SEARCH_INT_H__
#define	__XMGR_SEARCH_INT_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

#include <db_svc.h>
#include <stb_params.h>
#include <mgr_search.h>
#include <pmgr_search.h>

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/

HERROR	xmgr_search_SetSearchMode_Base (MgrSearch_NetType_e eSrcNetType, MgrSearch_TpType_e eSrcTpType, SVC_SatType_t eSatType, DbSvc_TuningInfo_t *pTuningInfo);
HERROR	xmgr_search_GetNetworkIdAndSearchParam_Base (HUINT16 *pusNetId, HUINT32 *pulStartFreq, HUINT32 *pulStopFreq);
HBOOL	xmgr_search_IsTsInfoAppendableForSameOnTsIdAndDiffTuneInfo_Base (HUINT32 ulActionId, DxDeliveryType_e eDeliType);
BUS_Result_t	xproc_search_SatConnectionTypeDetectAction (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t	xproc_search_SatRfDetect (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t	xproc_search_SatFind (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);


/*	Actions
 */
BUS_Result_t	xproc_search_SatAction0_Base(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t	xproc_search_SatAction1_Base(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);

BUS_Result_t	xproc_search_CabAction0_Base(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t	xproc_search_CabAction1_Base(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);

BUS_Result_t	xproc_search_TerAction0_Base(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t	xproc_search_TerAction1_Base(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);


/*		Search Result
 */
HERROR xmgr_search_ResultCountService_Base (HUINT32 ulActionId, DxSvcType_e eSvcType, HUINT32 *pulSvcNum);
HERROR xmgr_search_ResultGetService_Base (HUINT32 ulActionId, DxSvcType_e eSvcType, HUINT32 ulIndex, DbSvc_Info_t *pstSvcInfo);
MgrSearch_ResultContext_t *xmgr_search_ResultGetContext (HUINT32 ulActionId);

HERROR xmgr_search_CheckAddTsInfoCondition (HUINT32 ulDeliType, DbSvc_TuningInfo_t *pstTune1, DbSvc_TuningInfo_t *pstTune2, HBOOL *pbAddTs, HBOOL *pbUpdateTs);


#if defined(CONFIG_OP_FREESAT)
BUS_Callback_t	xmgr_search_GetAction_Freesat (HUINT32 ulActionId);
HERROR xmgr_search_ResultCountService_Freesat (HUINT32 ulActionId, DxSvcType_e eSvcType, HUINT32 *pulSvcNum);
HERROR xmgr_search_ResultGetService_Freesat (HUINT32 ulActionId, DxSvcType_e eSvcType, HUINT32 ulIndex, DbSvc_Info_t *pstSvcInfo);
#endif

#if defined(CONFIG_OP_SES)
BUS_Result_t	xproc_search_lcnupdate_Ses (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t	xproc_search_lcnupdate_Action_Ses (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
#endif

#if defined(CONFIG_OP_ERTELECOM)
BUS_Result_t	xproc_search_ertelecom_Action_0(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t	xproc_search_ertelecom_Action_1(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
#endif

#if defined(CONFIG_OP_ZIGGO)
BUS_Result_t	xproc_search_CabAction0_Ziggo(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t	xproc_search_CabAction1_Ziggo(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
#endif

#if defined(CONFIG_OP_NORDIG)
BUS_Result_t	xproc_search_nordig_Action_0(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t	xproc_search_nordig_Action_1(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
#endif

#if defined(CONFIG_OP_MBC)
BUS_Result_t	xproc_search_mbc_Action_0(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t	xproc_search_mbc_Action_1(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
#endif


#endif /* __XMGR_SEARCH_INT_H__ */

