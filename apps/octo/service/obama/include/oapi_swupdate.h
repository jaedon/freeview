/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  oapi_swupdate.h
	@brief

	Description:  									\n
	Module: OAPI / API					\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding À» »ç¿ëÇÏ´Â source file ÀÔ´Ï´Ù.
* MS949 encodingªòÞÅéÄª·ªÆ source fileªÇª¹¡£
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

#ifndef __OAPI_SWUPDATE_H__
#define __OAPI_SWUPDATE_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <dlib.h>
/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
// From AKP To OBAMA
#define RPC_OAPI_SWUPDATE_Iint							"oapi.swupdate.Init"
#define RPC_OAPI_SWUPDATE_UnInit						"oapi.swupdate.UnInit"
#define RPC_OAPI_SWUPDATE_CreateHandle					"oapi.swupdate.CreateHandle"
#define RPC_OAPI_SWUPDATE_CreateManualHandle			"oapi.swupdate.CreateManualHandle"
#define RPC_OAPI_SWUPDATE_DetectStart					"oapi.swupdate.DetectStart"
#define RPC_OAPI_SWUPDATE_DetectStop					"oapi.swupdate.DetectStop"
#define RPC_OAPI_SWUPDATE_DownloadStart					"oapi.swupdate.DownloadStart"
#define RPC_OAPI_SWUPDATE_DownloadStop					"oapi.swupdate.DownloadStop"
#define RPC_OAPI_SWUPDATE_InstallStart					"oapi.swupdate.InstallStart"
#define RPC_OAPI_SWUPDATE_InstallStop					"oapi.swupdate.InstallStop"
#define RPC_OAPI_SWUPDATE_Cancel						"oapi.swupdate.Cancel"

// From OBAMA To AKP
#define RPC_OBAMA_SWUPDATE_ResultNotifier				"obama.swupdate.ResultNotifier"

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef void (*OxSwUpdate_Notifier_t) (DxSwUpdate_EventData_t *pstResultEvent);
typedef void	(*OxSwUpdate_EventFreeFunc_t)	(HINT32 nArgc, void *apArgV[]);
typedef	void	(*OxSwUpdate_NotifierForAppkit_t)(OxSwUpdate_Notifier_t pfnCbFunc, OxSwUpdate_EventFreeFunc_t pfnFreeFunc, DxSwUpdate_EventData_t *pstResultEvent);

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/
void OAPI_SWUPDATE_Init(OxSwUpdate_NotifierForAppkit_t pfnAppkitNotifier);
void OAPI_SWUPDATE_UnInit(void);
HERROR OAPI_SWUPDATE_SwupdateInit(OxSwUpdate_Notifier_t fnApkNotifier);
HERROR OAPI_SWUPDATE_DetectStart(HUINT32 hSwUpdate, OxSwUpdate_Notifier_t fnApkNotifier);
HERROR OAPI_SWUPDATE_DetectStop(HUINT32 hSwUpdate);
HERROR OAPI_SWUPDATE_DownloadStart(HUINT32 hSwUpdate, OxSwUpdate_Notifier_t fnApkNotifier);
HERROR OAPI_SWUPDATE_DownloadStop(HUINT32 hSwUpdate);
HERROR OAPI_SWUPDATE_InstallStart(HUINT32 hSwUpdate, OxSwUpdate_Notifier_t fnApkNotifier);
HERROR OAPI_SWUPDATE_InstallStop(HUINT32 hSwUpdate);
HERROR OAPI_SWUPDATE_Cancel(HUINT32 hSwUpdate);
HERROR OAPI_SWUPDATE_CreateHandle(DxSwUpdate_Spec_e eSwUpdateSpec, HUINT32 *phSwUpdate, HBOOL bMaual, void *pvTunningInfo);

#endif // __OAPI_SWUPDATE_H__
