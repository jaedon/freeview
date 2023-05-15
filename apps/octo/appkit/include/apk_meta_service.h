/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  apk_meta_service.h
	@brief

	Description:  									\n
	Module: APPKITv2			\n

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

#ifndef ___APK_META_SERVICE_H___
#define ___APK_META_SERVICE_H___
/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef HBOOL (* APK_MetaUpdateChecker) (const HCHAR *typeName, const void *_old, const void *_new, HBOOL *updated);
typedef void  (* ApkMetaService_Notifier_t) (HINT32 event, const HINT32 *args, void *userdata);


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/
HBOOL	APK_META_SVC_Init (HBOOL bMainApp);
HBOOL	APK_META_SVC_DeInit (void);
HBOOL	APK_META_SVC_AllowTableNotifyMyself(HCHAR *pTableName, HBOOL bAllow);

void			APK_META_SVC_Delete (void *object);

void *			APK_META_SVC_Clone (const void *object);

DxService_t		*APK_META_SVC_GetService (HINT32 uid);
DxNetwork_t		*APK_META_SVC_GetNetwork (HINT32 uid);
DxTransponder_t	*APK_META_SVC_GetTransponder (HINT32 uid);
DxProvider_t	*	APK_META_SVC_GetProvider (HINT32 uid);
DxGroup_t		*APK_META_SVC_GetGroup (HINT32 uid);
DxBouquet_t*	APK_META_SVC_GetBouquet (HINT32 uid);
DxChannelLogo_t	*APK_META_SVC_GetLogoUrl(HINT32 uid, DxChannelLogo_t *buf);

HBOOL			APK_META_SVC_GetServiceTriplet (HINT32 uid, HINT32 *tsid, HINT32 *onid, HINT32 *sid);
HxVector_t *		APK_META_SVC_FindServiceByTriplet (HINT32 onid, HINT32 tsid, HINT32 svcid);
HxVector_t *		APK_META_SVC_FindServiceByNumber (HINT32 number);
HxVector_t *		APK_META_SVC_FindServiceBySvcId (HINT32 nSvcid);
HxVector_t *		APK_META_SVC_GetServiceList (void);
HxVector_t *		APK_META_SVC_GetTransponderList (void);
HxVector_t *		APK_META_SVC_GetNetworkList (void);
HxVector_t *		APK_META_SVC_GetGroupList (void);
HERROR			APK_META_SVC_Load (void);
HERROR			APK_META_SVC_Save (void);
HERROR			APK_META_SVC_SetService (const DxService_t *service);
HERROR			APK_META_SVC_AddService (DxService_t *pstService);
HERROR			APK_META_SVC_RemoveService (const DxService_t *service);
HERROR			APK_META_SVC_SetListener (ApkMetaService_Notifier_t onUpdate, void *userdata);
HERROR			APK_META_SVC_Reset (void);
HERROR			APK_META_SVC_LoadPreferredList (void);
HERROR	APK_META_SVC_LoadupdatedList (void);

void		APK_META_SVC_SetUpdateChecker (APK_MetaUpdateChecker checker);
HERROR	APK_META_SVC_ChangeUpdateFlag(DxSvcUpdateFlag_e fromFlag, DxSvcUpdateFlag_e toFlag);
HERROR	APK_META_SVC_RemoveServiceWithFlag(DxSvcUpdateFlag_e eFlag);
void 	APK_META_SVC_InitCommand(void);
#endif

/* end of file */
