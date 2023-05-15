/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  pmgr_earch.h
	@brief

	Description:			\n
	Module: 			 	\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	___PMGR_SEARCH_H___
#define	___PMGR_SEARCH_H___


/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#include <db_svc.h>
#include <db_param.h>
#include <mgr_search.h>

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

typedef struct tagSrchRst_Contents_t
{
	// Current State :
	MgrSearch_AppState_e		 eAppState;				// Current Search State :

	// == Current Transponder ==  : Draw 함수 내에서 따로 TP String을 작성해야 하는 경우 이를 사용
	DxDeliveryType_e			 eChType;
	DbSvc_TuningInfo_t			 stTransponder;
	DbSvc_NetInfo_t				 stNetwork;

	// == Searched Services
	HUINT32						 ulNumSvc, ulSvcArraySize;
	DbSvc_Info_t					*pstSvcArray;

	// == Searched Transponder ON/TS ID (for saving the same-service searching time)
	HUINT32						 ulNumOnTs, ulOnTsArraySize;
	DbSvc_TripleId_t			*pstOnTsIdArray;

	// == Total TP Numbers ==
	HUINT32						 ulCurrTpCnt, ulTotalTpNum;

	// === Satellite Information
	DbSvc_AntInfo_t					 stAntennaInfo;

	HUINT32						ulAntId; // MOONSG_sss
} MgrSearch_ResultContext_t;


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Plug-In Functions    ********************/
/*******************************************************************/
BUS_Callback_t	pmgr_search_GetSearchAction (HUINT32 ulActionId, DxDeliveryType_e eDeliveryType);
BUS_Callback_t	pmgr_search_GetFinadSatAction (HUINT32 ulActionId);
HERROR			pmgr_search_SetSearchMode (MgrSearch_NetType_e eSrcNetType, MgrSearch_TpType_e eSrcTpType, SVC_SatType_t eSatType, DbSvc_TuningInfo_t *pTuningInfo);
HERROR			pmgr_search_GetSearchType (HUINT8 *pucSearchType);
HERROR			pmgr_search_GetNetworkIdAndSearchParam (HUINT16 *pusNetId, HUINT32 *pulStartFreq, HUINT32 *pulStopFreq);
HBOOL			pmgr_search_IsTsInfoAppendableForSameOnTsIdAndDiffTuneInfo (HUINT32 ulActionId, DxDeliveryType_e eDeliType);
BUS_Callback_t	pmgr_search_GetSatConnectionAction (void);
BUS_Callback_t	pmgr_search_GetLcnUpdateSearchAction (void);
BUS_Callback_t	pmgr_search_GetLcnUpdateSearchMainProc (void);



MgrSearch_ResultContext_t *pmgr_search_ResultGetContext (HUINT32 ulActionId);
HERROR			pmgr_search_ResultCountService (HUINT32 ulActionId, DxSvcType_e eSvcType, HUINT32 *pulSvcNum);
HERROR			pmgr_search_ResultGetService (HUINT32 ulActionId, DxSvcType_e eSvcType, HUINT32 ulIndex, DbSvc_Info_t *pstSvcInfo);

#endif /* !___PMGR_SEARCH_H___ */

