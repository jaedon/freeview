/*******************************************************************
	File Description
*******************************************************************/
/*++
    @file        cxmgr_action.h
    @brief       Conax Action

	Description: File에 대한 설명을 적습니다.		\n
	Module: Layer와 모듈 명을 적습니다.			 	\n
	Remarks : 										\n

	Copyright (c) 2011 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
--*/
/*******************************************************************/
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
* ⓒ 2011-2012 Humax Co., Ltd.
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
*  CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.
*
* This License is effective until terminated. You may terminate this License at any time by destroying all copies
* of the Software including all documentation. This License will terminate immediately without notice from Humax
* to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
* of the Software and all documentation.
*
* The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
* All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
* in the Republic of Korea.
*/
#ifndef __CXMGR_ACTION_H__
#define __CXMGR_ACTION_H__
/*******************************************************************/
/********************     Include files    *************************/
/*******************************************************************/
#include <svc_cas.h>
/*******************************************************************/
/********************    Types and structures   ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/
HERROR CXACT_Init(void);
HERROR CXACT_ReInit(void);
HERROR CXACT_UpdateCasInfo(HUINT16 usScSlot, HUINT16* pusCaSysId, HUINT32 ulNumCaSysId, HUINT32 ulMaxNumSvc, HBOOL bAvailable);
HERROR CXACT_UpdateDrmInfo(Handle_t hAction, DxCopyrightControl_ConaxDrmInfo_t *pstDrmInfo);
HERROR CXACT_GetDrmInfo(Handle_t hAction, DxCopyrightControl_ConaxDrmInfo_t *pstDrmInfo);
Handle_t CXACT_GetActionHandle(Handle_t hStbHandle);
Handle_t CXACT_GetStbHandleFromActionHandle(Handle_t hAction);
HBOOL CXACT_IsLiveActionTypeFromStbHandle(Handle_t hStbHandle);
HBOOL CXACT_IsTsrActionTypeFromStbHandle(Handle_t hStbHandle);
HBOOL CXACT_IsRecActionTypeFromStbHandle(Handle_t hStbHandle);
#if defined(CONFIG_MW_MM_DELAYED_REC)
HERROR CXACT_UpdateTsrActionTypeToRecord(Handle_t hAction);
#endif
SvcCas_ActionType_e CXACT_GetActionTypeFromActionHandle(Handle_t hAction);
SvcCas_ActionType_e CXACT_GetActionTypeFromStbHandle(Handle_t hStbHandle);
Handle_t CXACT_GetSvcHandleFromStbHandle(Handle_t hStbHandle);
#if defined(CONFIG_SUPPORT_FCC)
HERROR CXACT_SetMainAction(Handle_t hMainAction, Handle_t hPrevAction);
#endif

#endif	/* !__CXMGR_ACTION_H__ */
