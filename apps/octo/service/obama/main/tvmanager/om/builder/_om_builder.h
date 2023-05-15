/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  _om_builder.h
	@brief

	Description:  									\n
	Module: TV MANAGER / OAPI					\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding 을 사용하는 source file 입니다.
* MS949 encodingを使用して source fileです。
* Quelldatei, die MS949-Codierung verwendet.
*
*/
/*
 * (c) 2011-2013 Humax Co., Ltd.
 * This program is produced by Humax Co., Ltd. ("Humax") and
 * the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
 * non-assignable, non-transferable and non-exclusive license to use this Software.
 * You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
 * you agree to the responsibility to take all reasonable efforts to protect the any information
 * you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
 * reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
 * If you have no authorized license, discontinue using this Software immediately.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS,
 * IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
 * IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
 * CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.

 * This License is effective until terminated. You may terminate this License at any time by destroying all copies
 * of the Software including all documentation. This License will terminate immediately without notice from Humax
 * to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
 * of the Software and all documentation.

 * The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
 * All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
 * in the Republic of Korea.
*/

#ifndef __INT_OM_BUILDER_H__
#define __INT_OM_BUILDER_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <oapi_builder.h>

// 아래 두개는 temp
#include <mgr_search.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
// tune only에서 주기적으로 signal ui를 업데이트 해주기 위해 사용하는 timer
#define	BUILDER_TUNE_TIMER_ID			0x20130719
#define	BUILDER_TUNE_TIMER_TIME			1000


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct
{
	HUINT32						 ulBuilderId;
	HUINT32						 ulActionId;
	DxBuilder_State_e			 eState, eLastState;
	BUS_Callback_t				 pfnSearchProc;

	OxBuilder_Option_t			 stOption;

	// search state
	OxBuilder_BuilderState_t	 stBuilderState;

	// searched svc info
	HUINT32						 ulLastTpNum;		// 마지막으로 보낸 tp num
	HUINT32						 ulSvcCount;		// 이번 tp에서 찾은 svc 갯수
	DxBuilder_SearchedInfo_t 	*pstTmpSvcInfo;

	// legacy
	MgrSearch_Option_t			 stSearchOption;		// contain tp list
} omBldContext_t;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Member Functions     ********************/
/*******************************************************************/
// om_builder.c
extern omBldContext_t*	om_builder_GetContextByBuilderId (HUINT32 ulBuilderId);
extern omBldContext_t*	om_builder_GetContextByActionId (HUINT32 ulActionId);
extern HERROR			om_builder_CheckAndNotifyStateChanged (HUINT32 ulBuilderId);
extern BUS_Result_t 	om_builder_BaseProc (HINT32 message, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3);

// om_builder_svcsrch.c
extern HERROR			om_builder_svcsrch_StartSearch (HUINT32 ulBuilderId);
extern HERROR			om_builder_svcsrch_StopSearch (HUINT32 ulBuilderId);
extern BUS_Result_t 	om_builder_svcsrch_MsgSearchStatusChanged (Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
extern BUS_Result_t 	om_builder_svcsrch_MsgSearchFinished (Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);

// om_builder_optplistsrch.c
extern HERROR			om_builder_optplistsrch_StartSearch (HUINT32 ulBuilderId);
extern HERROR			om_builder_optplistsrch_StopSearch (HUINT32 ulBuilderId);
extern BUS_Result_t 	om_builder_optplistsrch_MsgSearchStatusChanged (Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
extern BUS_Result_t 	om_builder_optplistsrch_MsgSearchFinished (Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);

// om_builder_antenna.c
extern HERROR			om_builder_antconn_StartSearch (HUINT32 ulBuilderId);
extern HERROR			om_builder_antconn_StopSearch (HUINT32 ulBuilderId);
extern BUS_Result_t 	om_builder_antconn_MsgSearchAntConnectTypeDetected (Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);

// mgr_oapi_builder_diseqc.c
extern HERROR			om_builder_diseqc_StartDetection (HUINT32 ulBuilderId);
extern HERROR			om_builder_diseqc_StopDetection (HUINT32 ulBuilderId);
extern BUS_Result_t 	om_builder_diseqc_MsgSearchFinished (Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);

// om_builder_tune.c
extern HERROR			om_builder_tune_StartTune (HUINT32 ulBuilderId);
extern HERROR			om_builder_tune_StopTune (HUINT32 ulBuilderId);
extern BUS_Result_t 	om_builder_tune_ProcessTuneEvent (HINT32 lMessage, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);

// om_builder_ciplussrch.c
#if defined(CONFIG_MW_CI_PLUS_V_1_3)
extern HERROR 			om_builder_ciplussrch_StartSearch(HUINT32 ulBuilderId);
extern HERROR 			om_builder_ciplussrch_StopSearch(HUINT32 ulBuilderId);
extern BUS_Result_t 	om_builder_ciplussrch_MsgSearchStatusChanged (Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
extern BUS_Result_t 	om_builder_ciplussrch_MsgSearchFinished(Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
#endif

// om_builder_ertsrch.c
#if defined(CONFIG_OP_ERTELECOM)
extern HERROR			om_builder_ertsrch_StartSearch (HUINT32 ulBuilderId);
extern HERROR			om_builder_ertsrch_StopSearch (HUINT32 ulBuilderId);
extern BUS_Result_t 	om_builder_ertsrch_MsgSearchStatusChanged (Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
extern BUS_Result_t 	om_builder_ertsrch_MsgSearchFinished (Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
#endif


#endif // __INT_OM_BUILDER_H__
