/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  ukfsat_si.h
	@brief	  UK Freesat 사양의 SI 관련 각종 선언 및 정의

	Description: 		\n

	Module: Layer와 모듈 명을 적습니다.			 	\n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	__EPG_FSAT_H__
#define	__EPG_FSAT_H__

#include <octo_common.h>

#include "_svc_epg_eit_lib.h"

#ifdef __cplusplus
	extern "C" {
#endif


// _svc_epg_context_mgr.h
#if			defined(CONFIG_MW_EPG_TVTV)
HERROR		xsvc_epgcontext_Process_Tvtv(HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3, HUINT32 ulParam4);
HBOOL xsvc_epgcontext_GetSvcUidFromTripleId(HUINT32 ulTvtvId, HUINT16 usOnId, HUINT16 usTsId, HUINT16 usSvcId, HUINT32 *pUsSvcUid);
HERROR xsvc_epgcontext_SendToDamaEventIfno(SvcEpg_Event_t *pNewEvent);
#endif


#ifdef __cplusplus
}
#endif

#endif
