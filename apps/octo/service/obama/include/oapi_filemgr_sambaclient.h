/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  oapi_filemgr_sambaclient.h
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

#ifndef __OAPI_FILEMGR_SAMBACLIENT_H__
#define __OAPI_FILEMGR_SAMBACLIENT_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define	RPC_OAPI_FILEMGR_SAMBACLIENT_Init				"oapi.filemgr.sambaclient.Init"
#define	RPC_OAPI_FILEMGR_SAMBACLIENT_StartScan			"oapi.filemgr.sambaclient.StartScan"
#define	RPC_OAPI_FILEMGR_SAMBACLIENT_StopScan			"oapi.filemgr.sambaclient.StopScan"
#define	RPC_OAPI_FILEMGR_SAMBACLIENT_UnmountAll			"oapi.filemgr.sambaclient.UnmountAll"
#define	RPC_OAPI_FILEMGR_SAMBACLIENT_Mount				"oapi.filemgr.sambaclient.Mount"
#define	RPC_OAPI_FILEMGR_SAMBACLIENT_Unmount			"oapi.filemgr.sambaclient.Unmount"

#define	RPC_OBAMA_FILEMGR_SAMBACLIENT_OnStarted			"obama.filemgr.sambaclient.onStarted"
#define	RPC_OBAMA_FILEMGR_SAMBACLIENT_OnStopped			"obama.filemgr.sambaclient.onStopped"
#define	RPC_OBAMA_FILEMGR_SAMBACLIENT_OnMounted			"obama.filemgr.sambaclient.onMounted"
#define	RPC_OBAMA_FILEMGR_SAMBACLIENT_OnUnmounted		"obama.filemgr.sambaclient.onUnmounted"
#define	RPC_OBAMA_FILEMGR_SAMBACLIENT_OnAddShared		"obama.filemgr.sambaclient.onAddShared"
#define	RPC_OBAMA_FILEMGR_SAMBACLIENT_OnRemoveShared	"obama.filemgr.sambaclient.onRemoveShared"
#define	RPC_OBAMA_FILEMGR_SAMBACLIENT_OnEndShared		"obama.filemgr.sambaclient.onEndShared"

#define	OxFILEMGR_SMB_SERVERNAME_LEN		64
#define	OxFILEMGR_SMB_SHARENAME_LEN			128
#define	OxFILEMGR_SMB_IP_LEN				16
#define	OxFILEMGR_SMB_ID_LEN				64
#define	OxFILEMGR_SMB_PASSWORD_LEN			64
#define	OxFILEMGR_SMB_MOUNTPATH_LEN			512
#define	OxFILEMGR_SMB_LOGMSG_LEN			128

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef void	(*OxFileMgrSambaClient_Notifier_t)(const HCHAR *pszEvent, void *pvData, void *pvUserData);

typedef struct
{
	HCHAR 	szServerName[OxFILEMGR_SMB_SERVERNAME_LEN];
	HCHAR	szSharedName[OxFILEMGR_SMB_SHARENAME_LEN];
	HCHAR 	szIp[OxFILEMGR_SMB_IP_LEN];
	HCHAR 	szId[OxFILEMGR_SMB_ID_LEN];
	HCHAR 	szPassword[OxFILEMGR_SMB_PASSWORD_LEN];
	HCHAR 	szMountPath[OxFILEMGR_SMB_MOUNTPATH_LEN];
	HCHAR	szLogMsg[OxFILEMGR_SMB_LOGMSG_LEN];
	HBOOL   bMounted;
	HINT32	nState;
} OxFileMgrSambaClient_SharedInfo_t;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/
extern HERROR	OAPI_FILEMGR_SAMBACLIENT_Init(OxFileMgrSambaClient_Notifier_t fnCallback);
extern HERROR	OAPI_FILEMGR_SAMBACLIENT_StartScan(void);
extern HERROR	OAPI_FILEMGR_SAMBACLIENT_StopScan(void);
extern HERROR	OAPI_FILEMGR_SAMBACLIENT_Mount(const HCHAR *pszServerName, const HCHAR *pszSharedName, const HCHAR *pszIp, const HCHAR *pszId, const HCHAR *pszPassword, const HCHAR *pszMountPath, const HCHAR *pszLogMsg, HBOOL bMounted, HINT32 nState);
extern HERROR	OAPI_FILEMGR_SAMBACLIENT_Unmount(const HCHAR *pszServerName, const HCHAR *pszSharedName, const HCHAR *pszIp, const HCHAR *pszId, const HCHAR *pszPassword, const HCHAR *pszMountPath, const HCHAR *pszLogMsg, HBOOL bMounted, HINT32 nState);
extern HERROR	OAPI_FILEMGR_SAMBACLIENT_UnmountAll(void);

#endif // __OAPI_FILEMGR_SAMBACLIENT_H__

