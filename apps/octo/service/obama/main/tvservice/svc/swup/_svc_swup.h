/**
	@file     _svc_swup.h
	@brief    _svc_swup.h

	Description: \n
	Module: \n
	Remarks :\n

	Copyright (c) 2013 HUMAX Co., Ltd. \n
	All rights reserved. \n
*/

#ifndef __SVC_SWUP_INT_H__
#define __SVC_SWUP_INT_H__


/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#include <svc_swup.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define MAX_FILE_PATH_LENGTH			200
#define MAX_FILE_NAME_LENGTH			50
#define MAX_IP_ADDR_LENGTH				20
#define MAX_USER_LENGTH					20
#define MAX_PASSWD_LENGTH				20
#define MAX_DESCRIPTION_LENGTH			1024*10
#define MAX_DESCRIPTION_SIZE			5
#define MAX_DESCRIPTION_LANG_LENGTH		10

/*******************************************************************/
/********************	   Typedef		   *************************/
/*******************************************************************/

// Ip Upgrade

typedef void SvcSwup_IpUpgradeHandle_t;

typedef struct
{
	HINT8	szSoftwareVersion[MAX_VERSION_LENGTH];
	HINT32	nSWVersion;
	HINT8	szSoftwarePath[MAX_PATH_LENGTH];
	HBOOL	bAvailable;
	// emergency ota field
	HBOOL	bEnable;
	HINT8	aDirPath[MAX_FILE_PATH_LENGTH];
	HINT8	aFileName[MAX_FILE_NAME_LENGTH];
	HINT8	aServerIP[MAX_IP_ADDR_LENGTH];
	HINT8	aUserName[MAX_USER_LENGTH];
	HINT8	aPasswd[MAX_PASSWD_LENGTH];
	// Forced OTA
	HBOOL	bForced;
	// Descriptions
	HINT8	nDescripCount;
	HINT8	aDescripLang[MAX_DESCRIPTION_SIZE][MAX_DESCRIPTION_LANG_LENGTH];
	HINT8	aDescriptions[MAX_DESCRIPTION_SIZE][MAX_DESCRIPTION_LENGTH];
} SvcSwup_IpUpgradeInfo_t;



#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************
 *	IP Upgrade
 */

SvcSwup_IpUpgradeHandle_t *svc_swup_IpUpgradeCreate(void);
void		svc_swup_IpUpgradeDestroy(SvcSwup_IpUpgradeHandle_t *hUpgrade);
HERROR		svc_swup_IpUpgradeCheckSoftware(SvcSwup_IpUpgradeHandle_t *hUpgrade, const HINT8 *url, SvcSwup_IpUpgradeInfo_t *pInfo);
HERROR		svc_swup_IpUpgradeDownloadStart(SvcSwup_IpUpgradeHandle_t *hUpgrade, const HINT8 *url, const HINT8 *downloadPath);
HERROR		svc_swup_IpUpgradeDownloadStop(SvcSwup_IpUpgradeHandle_t *hUpgrade);
HERROR		svc_swup_IpUpgradeGetSize(SvcSwup_IpUpgradeHandle_t *hUpgrade, HFLOAT64 *pullSize);
HERROR		svc_swup_IpUpgradeGetDownloadedSize(SvcSwup_IpUpgradeHandle_t *hUpgrade, HFLOAT64 *pullSize);
void		svc_swup_IpRegisterCommand(void);

HINT32		svc_swup_UtilPostMsgToMgr(HINT32 nMsg, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3);
HINT32		svc_swup_UtilPostDataToMgr(HINT32 nMsg, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, void *pvData, HUINT32 ulSize);


#ifdef __cplusplus
};
#endif

#endif	/* __SVC_SWUP_INT_H__ */

