/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  layer_module.h File Name을 적습니다.
	@brief	  file_name & simple comment.

	Description: File에 대한 설명을 적습니다.		\n
	Module: Layer와 모듈 명을 적습니다.			 	\n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	__PSVC_EPG_H__
#define	__PSVC_EPG_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

#include <octo_common.h>

#include <svc_epg.h>

#include "_svc_epg_common.h"
#include "_svc_epg_eit_lib.h"
#include "_svc_epg_context_mgr.h"
#include "_svc_epg.h"
/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/
HERROR			psvc_epg_InitProduct(void);
HERROR			psvc_epg_InitResource(void);
HERROR			psvc_epg_CreateCheckServiceList(void);
HERROR			psvc_epg_GetSectionInfo(EPG_SpecType_e eSpec, HUINT8 *pucRawSection, svcEpg_ServiceInfo_t *pstSectionInfo);
HERROR			psvc_epg_GetEpgTypeByTableId(HUINT8 ucTableId, SvcEpg_Type_b *pbwEpgType);


svcEpg_RawProcessDataInit_t*psvc_epg_GetEpgProcessInitTable(HUINT32* ulNumberOfTable);


#if defined(CONFIG_MW_EPG_TVTV)
HERROR psvc_epg_CmdCreateBuffer_Tvtv(void);
HERROR psvc_epg_CmdCreateTvstInfo_Tvtv(void);
HERROR psvc_epg_CmdCreateDgktInfo_Tvtv(HBOOL bNeedConverting);
HERROR	psvc_epg_CmdCreateTvtvSystemInfo_Tvtv(HBOOL bNeedConverting);
HERROR	psvc_epg_CmdCreateTsmtInfo_Tvtv(HBOOL bNeedConverting);
HERROR	psvc_epg_CmdCreateLtotInfo_Tvtv(HBOOL bNeedConverting);
HERROR	psvc_epg_CmdCreateEpgInfo_Tvtv(HBOOL bNeedConverting);
void psvc_epg_CmdInitBuffer_Tvtv(void);
#endif

#endif /* !__PSVC_EPG_H__ */
