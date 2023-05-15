/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  oapi_meta_service.h
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

#ifndef __OAPI_META_SERVICE_H__
#define __OAPI_META_SERVICE_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

// To OBAMA
//#define RPC_OAPI_META_SERVICE_Read					"oapi.meta.service.Read"
#define RPC_OAPI_META_SERVICE_SetDynamicUpdate			"oapi.meta.service.SetDynamicUpdate"
#define RPC_OAPI_META_SERVICE_LoadPreprogrammedList		"oapi.meta.service.LoadPreprogrammedList"
#define RPC_OAPI_META_SERVICE_LoadUpdatedList			"oapi.meta.service.LoadUpdatedList"
#define RPC_OAPI_META_SERVICE_Commit					"oapi.meta.service.Commit"
#define RPC_OAPI_META_SERVICE_Reset					"oapi.meta.service.Reset"

// From OBAMA
#define RPC_OBAMA_META_SERVICE_OnUpdate					"obama.meta.service.OnUpdate"


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

typedef void (*OxMetaSvc_LoadResCb_t) (HxTREE_t *updated, HxTREE_t *removed, HBOOL bForceNoti);

typedef void (*OxMetaSvc_Notifier_t) (void);


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/

void OAPI_META_SVC_SetListener(OxMetaSvc_Notifier_t pfNotifier, void *pvUserdata);
void OAPI_META_SVC_SetDynamicUpdate(HBOOL bEnable);
void OAPI_META_SVC_LoadPreprogrammedList(void);
void OAPI_META_SVC_Commit(void);

void OAPI_META_SVC_LoadUpdatedList(OxMetaSvc_LoadResCb_t OnResponse, HBOOL bFlag);
void OAPI_META_SVC_Reset(HCHAR *pszName);

#endif // __OAPI_META_SERVICE_H__
