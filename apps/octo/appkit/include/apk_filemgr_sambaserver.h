/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  apk_filemgr_sambaserver.h
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

#ifndef __APK_FILEMGR_SAMBASERVER_H__
#define __APK_FILEMGR_SAMBASERVER_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define	APKFILEMGR_SMB_SERVERNAME_LEN		64
#define	APKFILEMGR_SMB_SHARENAME_LEN		128
#define	APKFILEMGR_SMB_IP_LEN				16
#define	APKFILEMGR_SMB_ID_LEN				64
#define	APKFILEMGR_SMB_PASSWORD_LEN			64
#define	APKFILEMGR_SMB_MOUNTPATH_LEN		512
#define	APKFILEMGR_SMB_LOGMSG_LEN			128

#define APK_SMBS_NAME_LEN		128
#define APK_SMBS_PATH_LEN	 	512

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef enum
{
	eAPK_FILEMGR_SMBS_EVT_SERVER_START = 0,
	eAPK_FILEMGR_SMBS_EVT_SERVER_STOP,
	eAPK_FILEMGR_SMBS_EVT_CONFIG_CHANGE,
	eAPK_FILEMGR_SMBS_EVT_PASSWD_CHANGE,
	eAPK_FILEMGR_SMBS_EVT_PERMISSION_CHANGE,
	eAPK_FILEMGR_SMBS_EVT_STATUS_CHANGE,
	eAPK_FILEMGR_SMBS_EVT_UNKNOWN,
	eAPK_FILEMGR_SMBS_EVT_MAX
} APK_FILEMGR_SMBS_EVT_e;

typedef struct 
{
	HCHAR	szWorkgroup[APK_SMBS_NAME_LEN];
	HCHAR	szServerName[APK_SMBS_NAME_LEN];
	HCHAR	szServerDesc[APK_SMBS_NAME_LEN];
} ApkFileMgr_SMBS_CONF_t;

typedef struct 
{
	HINT32 bStarted;
	HINT32 nClientCnt;
	ApkFileMgr_SMBS_CONF_t stConf;
} ApkFileMgr_SMBS_INFO_t;

typedef struct
{
	HCHAR	szName[APK_SMBS_NAME_LEN];
	HCHAR 	szPath[APK_SMBS_PATH_LEN];
	HINT32	nState;
	HBOOL	bWritable;
	HBOOL  	bMounted;
} ApkFileMgr_SMBS_SHARED_t;

typedef	void	(*APK_FILEMGR_SAMBASERVER_Notifier_t)(HINT32 eEvent, HINT32 result, void *pvUser);

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/
HERROR	APK_FILEMGR_SAMBASERVER_Init(void);
HERROR	APK_FILEMGR_SAMBASERVER_RegisterCallback(APK_FILEMGR_SAMBASERVER_Notifier_t fnNotifier);
HERROR	APK_FILEMGR_SAMBASERVER_UnregisterCallback(APK_FILEMGR_SAMBASERVER_Notifier_t fnNotifier);

HINT32	APK_FILEMGR_SAMBASERVER_Start(void);
HINT32	APK_FILEMGR_SAMBASERVER_Stop(void);
HINT32	APK_FILEMGR_SAMBASERVER_SetConfig( const ApkFileMgr_SMBS_CONF_t *pstConfig );
HINT32	APK_FILEMGR_SAMBASERVER_GetServerInfo( ApkFileMgr_SMBS_INFO_t *pstInfo );
HINT32	APK_FILEMGR_SAMBASERVER_SetPassword( const HCHAR *pszUserName, const HCHAR *pszCurPasswd, const HCHAR *pszNewPasswd );
HINT32	APK_FILEMGR_SAMBASERVER_CheckPassword(const HCHAR *pszUserName, const HCHAR *pszPasswd, HBOOL *pbIsValid);
HINT32	APK_FILEMGR_SAMBASERVER_SetPermission(const HCHAR *pszUserName, ApkFileMgr_SMBS_SHARED_t *pstShared );
HERROR	APK_FILEMGR_SAMBASERVER_GetSharedList(const HCHAR *pszUserName, HINT32 *pnCount, ApkFileMgr_SMBS_SHARED_t **ppstList);
HINT32	APK_FILEMGR_SAMBASERVER_SetPublicAccess(const HCHAR *pszUserName, HBOOL bAccess);
HERROR	APK_FILEMGR_SAMBASERVER_GetPublicAccess(const HCHAR *pszUserName, HBOOL *pbAccess);

#endif
/* end of file */
