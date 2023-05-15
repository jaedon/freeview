/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  _xsvc_epg.h
	@brief	  epg plugin local api.

	Description: 		\n

	Module: Layer와 모듈 명을 적습니다.			 	\n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	__XSVC_EPG_INT_H__
#define	__XSVC_EPG_INT_H__

#ifdef __cplusplus
	extern "C" {
#endif
#include <pal_sef.h>
#include "_svc_epg_eit_lib.h"
#include "_svc_epg_context_mgr.h"

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

#if defined(CONFIG_PROD_HMS1000S)
HERROR xsvc_epg_Init_Hms1000s(void);
HERROR xsvc_epg_InitResource_Hms1000s(void);
#elif defined(CONFIG_PROD_IR4000HD)
HERROR xsvc_epg_Init_Ir4000hd(void);
HERROR xsvc_epg_InitResource_Ir4000hd(void);
#elif defined(CONFIG_PROD_ICORDPRO)
HERROR xsvc_epg_Init_Icordpro(void);
HERROR xsvc_epg_InitResource_Icordpro(void);
#elif defined(CONFIG_PROD_HDNEO)
HERROR xsvc_epg_Init_Hdneo(void);
HERROR xsvc_epg_InitResource_Hdneo(void);
#elif defined(CONFIG_PROD_HGS1000S)
HERROR xsvc_epg_Init_Hgs1000s(void);
HERROR xsvc_epg_InitResource_Hgs1000s(void);
#elif defined(CONFIG_PROD_HMS1000T)
HERROR xsvc_epg_Init_Hms1000t(void);
HERROR xsvc_epg_InitResource_Hms1000t(void);
#elif defined(CONFIG_PROD_HD9000I)
HERROR xsvc_epg_Init_hd9000i(void);
HERROR xsvc_epg_InitResource_hd9000i(void);
#elif defined(CONFIG_PROD_IHDR6100C)
HERROR xsvc_epg_Init_Ihdr6100c(void);
HERROR xsvc_epg_InitResource_Ihdr6100c(void);
#elif defined(CONFIG_PROD_FVP4000T)
HERROR xsvc_epg_Init_fvp4000t(void);
HERROR xsvc_epg_InitResource_fvp4000t(void);
#elif defined(CONFIG_PROD_DIGIMAXT2)
HERROR xsvc_epg_Init_Digimaxt2(void);
HERROR xsvc_epg_InitResource_Digimaxt2(void);
#elif defined(CONFIG_PROD_HDFOXIRCONNECT)
HERROR xsvc_epg_Init_HdFoxIrConnect(void);
HERROR xsvc_epg_InitResource_HdFoxIrConnect(void);
#elif defined(CONFIG_PROD_HA7100S)
HERROR xsvc_epg_Init_Ha7100s(void);
HERROR xsvc_epg_InitResource_Ha7100s(void);
#elif defined(CONFIG_PROD_YSR2000)
HERROR xsvc_epg_Init_ysr2000(void);
HERROR xsvc_epg_InitResource_ysr2000(void);
#elif defined(CONFIG_PROD_YS1000)
HERROR xsvc_epg_Init_ys1000(void);
HERROR xsvc_epg_InitResource_ys1000(void);
#elif defined(CONFIG_PROD_M1)
HERROR xsvc_epg_Init_M1(void);
HERROR xsvc_epg_InitResource_M1(void);
#endif


// xsvc_epg_context_raw_eit_pf.c
HERROR		xsvc_epgcontext_PfProcess_Base(HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3, HUINT32 ulParam4);
HERROR		xsvc_epgcontext_PfProcessEitCallBack_Base(PalSef_ReceiveMessage_t *pstMsg, HUINT32 ulParam);

// xsvc_epg_context_raw_eit_sch.c
HERROR		xsvc_epgcontext_SchProcess_Base(HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3, HUINT32 ulParam4);
HERROR		xsvc_epgcontext_SchProcessEitCallBack_Base(PalSef_ReceiveMessage_t *pstMsg, HUINT32 ulParam);
HERROR		xsvc_epgcontext_StopSchFilter_Base(Handle_t hAction);

// xsvc_epg_db_raw_base.c
HERROR		xsvc_epgdb_GetEpgTypeByTableId_Base(HUINT8 ucTableId, SvcEpg_Type_b *pbwEpgType);


#if defined(CONFIG_MW_EPG_RAW_SAVE_DATA)
HERROR 				xsvc_epgdb_LoadEitSection_Base(SvcEpg_EitSectionFileFlag_e eEitSecFlag);
HERROR 				xsvc_epgdb_SaveEitSection_Base(SvcEpg_EitSectionFileFlag_e eEitSecFlag);
HERROR				xsvc_epgdb_DeleteEpgFile_Base(SvcEpg_EitSectionFileFlag_e eEitSecFlag);
HBOOL				xsvc_epgdb_GetNeedSaveEpgFlag_Base(void);
void					xsvc_epgdb_SetNeedSaveEpgFlag_Base(void);
#endif

// xsvc_epg_eit_base.c
HERROR				xsvc_epgeit_GetSectionInfo_Base(HUINT8 *pucRawSection, svcEpg_ServiceInfo_t *pstSectionInfo);
HERROR				xsvc_epgeit_GetSectionSimpleInfo_Base(	HUINT8 *pucRawSection,
														HUINT8 *pucTableId,
														HUINT8 *pucVersion,
														HUINT8 *pucSecNum,
														HUINT32 *pulCrc);
//context
svcEpg_RawProcessDataInit_t*	xsvc_epgcontext_GetEpgProcessInitTable_Base(HUINT32* ulNumberOfTable);
#if defined(CONFIG_MW_EPG_TVTV)
svcEpg_RawProcessDataInit_t*	xsvc_epgcontext_GetEpgProcessInitTable_Tvtv(HUINT32* ulNumberOfTable);
HERROR	xsvc_epg_GetTvtvExtEventByProgId(SvcEpg_FindParam_t *pParam, POINTER_LIST_T **ppExtList);
HERROR	xsvc_epg_FreeTvtvSchEvent(void);
#endif

#if defined(CONFIG_MW_EPG_RAW_SAVE_DATA)&& defined(CONFIG_MW_EPG_RAW_SAVE_DATA_TO_HDD)
HERROR					xsvc_epgloadtask_Init(void);
HERROR					xsvc_epgloadtask_LoadDataFromHddUsingTask(void);
HERROR					xsvc_epgloadtask_SaveDataToHddUsingTask(HINT32 evType);
#endif

// Base
HERROR xsvc_epg_InitAction_Base(SvcEpg_EpgKind_e eEpgKind);
HERROR xsvc_epg_Start_Base(SvcEpg_EpgKind_e eEpgKind, SvcEpg_EventKind_e eEvtKind, Handle_t hAction, Handle_t hService, HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3);
HERROR xsvc_epg_Stop_Base(SvcEpg_EpgKind_e eEpgKind, SvcEpg_EventKind_e eEvtKind, Handle_t hAction);
HERROR xsvc_epg_LoadData_Base(SvcEpg_EpgKind_e eEpgKind, Handle_t hAction, Handle_t hService);
HERROR xsvc_epg_UnLoadData_Base(SvcEpg_EpgKind_e eEpgKind, Handle_t hAction);
HERROR xsvc_epg_StartContent_Base(SvcEpg_EpgKind_e eEpgKind, Handle_t hAction, Handle_t hService);
HERROR xsvc_epg_StopContent_Base(SvcEpg_EpgKind_e eEpgKind, Handle_t hAction);

// Tvtv
#if defined(CONFIG_MW_EPG_TVTV)
HERROR xsvc_epg_InitAction_Tvtv(SvcEpg_EpgKind_e eEpgKind);
HERROR xsvc_epg_Start_Tvtv(SvcEpg_EpgKind_e eEpgKind, SvcEpg_EventKind_e eEvtKind, Handle_t hAction, Handle_t hService, HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3);
HERROR xsvc_epg_Stop_Tvtv(SvcEpg_EpgKind_e eEpgKind, SvcEpg_EventKind_e eEvtKind, Handle_t hAction);
HERROR xsvc_epg_CreateBuffer_Tvtv(void);
HERROR xsvc_epg_CreateTvstInfo_Tvtv(void);
HERROR xsvc_epg_CreateDgktInfo_Tvtv(HBOOL bNeedConverting);
HERROR xsvc_epg_CreateTvtvSystemInfo_Tvtv(HBOOL bNeedConverting);
HERROR xsvc_epg_CreateTsmtInfo_Tvtv(HBOOL bNeedConverting);
HERROR xsvc_epg_CreateLtotInfo_Tvtv(HBOOL bNeedConverting);
HERROR xsvc_epg_CreateEpgInfo_Tvtv(HBOOL bNeedConverting);
void xsvc_epg_InitBuffer_Tvtv(void);
#endif


#ifdef __cplusplus
}
#endif

#endif
