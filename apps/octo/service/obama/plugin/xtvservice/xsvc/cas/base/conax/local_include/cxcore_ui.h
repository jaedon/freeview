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
@file				cxcore_ui.h
@brief			cxcore_ui.h

	Description:   \n
	Module: MW/CAS \n

	Copyright (c) 2009 HUMAX Co., Ltd. \n
	All rights reserved. \n
*/

#ifndef	__CXCORE_UI_H__
#define	__CXCORE_UI_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <conax.h>
#include "../local_include/cxcore_sc.h"

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
HERROR CXUI_InitCore(void);

HERROR CXUI_SessionStart(Handle_t hSession);
HERROR CXUI_SessionStop(Handle_t hSession);
HERROR CXUI_ScStateChanged(HINT32 nScSlot, CxSc_State_t eScState, HBOOL bShowAV);
#if defined(CONFIG_MW_CAS_CONAX_TEXT)
HERROR CXUI_CheckUserTextAvailable(void);
HERROR CXUI_CheckScheduledMsgAvailable(CxUserText_t* pstText);
void CXUI_RetryUserText(HUINT32 ucFlagFrom);
HERROR CXUI_UserTextReceived(CxUserText_t* pstText, CxUTFlagFrom_t ucFlagFrom);
HERROR CXAPI_NVMRemoveUserText(HUINT8 ucSeqNo);
#endif
#if defined(CONFIG_MW_CAS_CONAX_FINGERPRINT)
HERROR CXUI_CheckFingerprintAvailable(void);
void CXUI_RetryFingerprint(void);
HERROR CXUI_FingerprintReceived(CxFingerprint_t* pstFingerprint, Handle_t hSession);
#endif
HERROR CXUI_DisplayAccessStatus(Handle_t hEcm, CxAccessStatus_t* pAccess, HUINT32 ulParam);
#if defined(CONFIG_MW_MM_PVR)
HERROR CXUI_DisplayMaturityRate(Handle_t hSession, CxMaturityRatingValue_t *pstMaturityRate, HBOOL bScPinPass);
#endif
HERROR CXUI_UpdateDescrambleStatusBySC(Handle_t hEcm, HBOOL bDescrambled);
#if defined(CONFIG_MW_CAS_CONAX_CPM)
HERROR CXUI_DisplayPairingError(Handle_t hSession, HINT32 nErrCode);
#endif
HERROR CXUI_DisplayOrdering(Handle_t hSession);
HERROR CXUI_ProcessOrderInfo(Handle_t hEcm, CxOrderInfo_t* pInfo);
HERROR CXUI_ProcessAcceptViewingInfo(Handle_t hEcm, CxAcceptViewingInfo_t* pInfo);
HERROR CXUI_ProcessTokenDebitInfo(Handle_t hEcm, CxTokenDebitInfo_t* pInfo);

HBOOL CXUI_GetUiDisplayedState(HINT32 nScSlot);

HUINT8* CXUI_DEBUG_GetMsgTypeStr(CxUiMsgType_t eMsg);
#if defined(CONFIG_DEBUG)
void CXUI_DEBUG_PrintData(CxUiMsgType_t eMsg, HUINT32 ulParam);
#endif

void CXUI_SetRecvUTTimeStamp(void);
// API function (which can be invoked from outside of the conax core)

#endif /* !__CXCORE_UI_H__ */
