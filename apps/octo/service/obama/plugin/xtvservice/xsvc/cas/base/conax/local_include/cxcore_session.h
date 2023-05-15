/*
 * (c) 2011-2012 Humax Co., Ltd.
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
/*******************************************************************
	File Description
********************************************************************/
/**
@file				cxcore_stream.h
@brief			cxcore_stream.h

	Description:   \n
	Module: MW/CAS \n

	Copyright (c) 2009 HUMAX Co., Ltd. \n
	All rights reserved. \n
*/

#ifndef	__CXCORE_SESSION_H__
#define	__CXCORE_SESSION_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <conax.h>

/*******************************************************************/
/********************      Global Variables 	********************/
/*******************************************************************/

/*******************************************************************/
/********************      Macro Definition 	********************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef  	   *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Public Functions 	********************/
/*******************************************************************/
HERROR CXSESSION_InitCore(void);
#if defined(CONFIG_SUPPORT_S3MODE_FASTBOOT)
HERROR CXSESSION_DeInitCore(void);
#endif
HUINT32 CXSESSION_GetAllSessionByScSlot(HINT32 nScSlot, Handle_t* phList);
Handle_t CXSESSION_GetMainHandle(void);
Handle_t CXSESSION_GetHandleByStbHandle(Handle_t hStbHandle);
Handle_t CXSESSION_GetStbHandle(Handle_t hSession);
HUINT32 CXSESSION_GetStbActionId(Handle_t hSession);
SvcCas_ActionType_e CXSESSION_GetActionType(Handle_t hSession);
SvcCas_PmtInfo_t* CXSESSION_GetPmtInfo(Handle_t hSession);
HINT32 CXSESSION_GetScSlot(Handle_t hSession);
HINT32 CXSESSION_GetSessionIdx(Handle_t hSession);
HINT32 CXSESSION_GetSessionCnt(void);
HBOOL CXSESSION_IsSelected(Handle_t hSession);

HERROR CXSESSION_Select(Handle_t hStbHandle, HBOOL bSelect);
HERROR CXSESSION_StartSvc(Handle_t hStbHandle);
HERROR CXSESSION_StopSvc(Handle_t hStbHandle, HBOOL bNextSvcExist);
HERROR CXSESSION_ResumeSvc(Handle_t hStbHandle);
HERROR CXSESSION_PauseSvc(Handle_t hStbHandle);
HERROR CXSESSION_SetPid(Handle_t hStbHandle, CxEsPidInfo_t* pstEsPidInfo);
//HERROR CXSESSION_SetPid(Handle_t hStbHandle, HUINT16 usPid, HBOOL bAdd);
HERROR CXSESSION_UpdatePmt(Handle_t hStbHandle, SvcCas_PmtInfo_t* pstPmtInfo);
HERROR CXSESSION_UpdateCat(Handle_t hStbHandle, HUINT16 usUniqueTsId, HUINT8* pucCat);

#if defined(CONFIG_MW_MM_DELAYED_REC)
HERROR CXSESSION_MoveSvc(Handle_t hStbHandle);
#endif

#if defined(CONFIG_SUPPORT_FCC)
HERROR CXSESSION_SetMainService(Handle_t hMainStbHandle, Handle_t hPrevStbHandle);
#endif

HERROR CXSESSION_ScInserted(HINT32 nScSlot);
HERROR CXSESSION_ScExtracted(HINT32 nScSlot);
HERROR CXSESSION_ScErrorHandler(HINT32 nScSlot);

HBOOL CXSESSION_IsTsrSession(Handle_t hSession);
HBOOL CXSESSION_IsRecordSession(Handle_t hSession);
HERROR CXSESSION_ResetAllSession(void);
HERROR CXSESSION_ResetSession(Handle_t hStbHandle);

HERROR CXSESSION_ResetLiveSessionOverrided(void);
Handle_t CXSESSION_GetLiveStbHandle(void);
HERROR CXSESSION_ResetLiveSessionOverridedFlag(void);

HERROR CXSESSION_SetCaPinPass(Handle_t hSession, HBOOL bCaPinPass);
HBOOL CXSESSION_GetCaPinPass(Handle_t hSession);

#if defined(CONFIG_MW_CAS_CONAX_URI)
HERROR CXSESSION_UriWait(Handle_t hSession);
HERROR CXSESSION_UriTimeout(Handle_t hSession);
HERROR CXSESSION_UriReceived(Handle_t hSession, DxCopyrightControl_ConaxDrmInfo_t *pstDrmInfo);
#endif

#if defined(CONFIG_DEBUG)
void CMD_CXSESSION_PrintSessionContext(void);
#endif

#endif /* !__CXCORE_SESSION_H__ */
