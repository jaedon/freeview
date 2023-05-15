/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  apk_filemgr_sambaclient.h
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

#ifndef __APK_FILEMGR_SAMBACLIENT_H__
#define __APK_FILEMGR_SAMBACLIENT_H__

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

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef enum
{
	eAPKFILEMGR_SMBC_EVT_SCAN_START = 0,
	eAPKFILEMGR_SMBC_EVT_SCAN_STOP,
	eAPKFILEMGR_SMBC_EVT_SCAN_END,
	eAPKFILEMGR_SMBC_EVT_SERVER_ADD,
	eAPKFILEMGR_SMBC_EVT_SERVER_REMOVE,
	eAPKFILEMGR_SMBC_EVT_SERVER_SHARED,
	eAPKFILEMGR_SMBC_EVT_MOUNTED,
	eAPKFILEMGR_SMBC_EVT_UNMOUNTED,

} ApkFileMgr_SMBC_Event_e;

typedef struct
{
	HCHAR 	szServerName[APKFILEMGR_SMB_SERVERNAME_LEN];
	HCHAR	szSharedName[APKFILEMGR_SMB_SHARENAME_LEN];
	HCHAR 	szIp[APKFILEMGR_SMB_IP_LEN];
	HCHAR 	szId[APKFILEMGR_SMB_ID_LEN];
	HCHAR 	szPassword[APKFILEMGR_SMB_PASSWORD_LEN];
	HCHAR 	szMountPath[APKFILEMGR_SMB_MOUNTPATH_LEN];
	HCHAR	szLogMsg[APKFILEMGR_SMB_LOGMSG_LEN];
	HBOOL   	bMounted;
	HINT32	nState;
} ApkFileMgr_SMBSharedFolderInfo_t;

typedef	void	(*APK_FILEMGR_SAMBACLIENT_Notifier_t)(const HCHAR *pszEventName, void *pvData, void *pvUser);

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/
HERROR	APK_FILEMGR_SAMBACLIENT_Init(void);
HERROR	APK_FILEMGR_SAMBACLIENT_RegisterCallback(APK_FILEMGR_SAMBACLIENT_Notifier_t fnNotifier);
HERROR	APK_FILEMGR_SAMBACLIENT_UnregisterCallback(APK_FILEMGR_SAMBACLIENT_Notifier_t fnNotifier);
HERROR	APK_FILEMGR_SAMBACLIENT_StartScan(void);
HERROR	APK_FILEMGR_SAMBACLIENT_StopScan(void);
HERROR	APK_FILEMGR_SAMBACLIENT_Mount(ApkFileMgr_SMBSharedFolderInfo_t *pstInfo);
HERROR	APK_FILEMGR_SAMBACLIENT_Unmount(ApkFileMgr_SMBSharedFolderInfo_t *pstInfo);
HERROR	APK_FILEMGR_SAMBACLIENT_UnmountAll(void);


#endif
/* end of file */
