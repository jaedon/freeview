/**
	@file     svc_swup_ip.c
	@brief    svc_swup_ip.c

	Description: \n
	Module: \n
	Remarks :\n

	Copyright (c) 2013 HUMAX Co., Ltd. \n
	All rights reserved. \n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <dlib.h>
#include <octo_common.h>

#include <napi_netconf.h>
#include <svc_swup.h>

#include "_svc_swup.h"

#if defined(WIN32)
//#include <winsock2.h>
//#include <ws2tcpip.h>
#else
#include <netdb.h> 	/* ptHostent / gethostbyname */
#endif

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC SvcSwUpdate_DetectResult_t s_hDetectIp;

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/


STATIC void svc_ipupgrade_DetectClear(void)
{
	s_hDetectIp.eSource = eDxSWUPDATE_SOURCE_None;
	s_hDetectIp.bFound = FALSE;
	s_hDetectIp.bAvailable = FALSE;
	s_hDetectIp.ulDataVersion = 0;
}


#ifdef WIN32
#if 0
STATIC const char *inet_ntop_(int af, const void *src, char *dst, socklen_t cnt)
{
    if (af == AF_INET) {
        struct sockaddr_in in;
        memset(&in, 0, sizeof(in));
        in.sin_family = AF_INET;
        memcpy(&in.sin_addr, src, sizeof(struct in_addr));
        getnameinfo((struct sockaddr *)&in, sizeof(struct sockaddr_in), dst, cnt, NULL, 0, NI_NUMERICHOST);
        return dst;
    }

    return NULL;
}
#endif

STATIC int inet_aton(const char *cp, struct in_addr *addr)
{
    if( cp==NULL || addr==NULL )
    {
        return(0);
    }

    addr->s_addr = inet_addr(cp);
    return (addr->s_addr == INADDR_NONE) ? 0 : 1;
}

#endif

#if defined(CONFIG_DEBUG)
STATIC int svc_swup_IpProcessCmd_ipota_detect(void *szArgList)
{
	const char *url = "http://test.humaxtvportal.com/SWUpdate/CheckNewSW?SystemID=809C.5010&SWVersion=0.00.04";
	SvcSwUpdate_DetectParamIp_t tmp;

	HLIB_CMD_Printf("IP OTA CMD : %s \n", url);

	tmp.szUrl = (HINT8*)url;
	SVC_SWUPDATE_StartDetectIp(&tmp);

	return HxCMD_OK;
}
#endif

void svc_swup_IpRegisterCommand(void)
{
#define	hCmdHandle		"svc_swup"

#if defined(CONFIG_DEBUG)
	HLIB_CMD_RegisterWord(hCmdHandle, 
						svc_swup_IpProcessCmd_ipota_detect,
						"ipota_detect", 			/* keyword */
						"IP OTA detect test",		/*	 help  */
						"ipota_detect");		/*	usage  */
#endif
}


STATIC HERROR svc_swup_IpEmergencyParseIp(HINT8 *pszIP, HINT32 *pIPAddr)
{
	HERROR			tResult = ERR_OK;
	HERROR 			hRet = 0;
	struct in_addr	 tIpAddr;

	hRet = inet_aton(pszIP, &tIpAddr);
	if(hRet != 1)
	{
		return ERR_FAIL;
	}
	*pIPAddr = tIpAddr.s_addr;

	return tResult;
}

Handle_t	SVC_SWUPDATE_StartDetectIp(SvcSwUpdate_DetectParamIp_t *pstIpParam)
{
	HERROR eRet = ERR_OK;
	SvcSwup_IpUpgradeHandle_t *hUpgrade = NULL;
	SvcSwup_IpUpgradeInfo_t *ipInfo = NULL;
	HINT32 ulPortalIp = 0;
	HINT8 i = 0;

	svc_ipupgrade_DetectClear();

	if(pstIpParam->szUrl == NULL)
	{
		svc_swup_UtilPostDataToMgr(eSEVT_SWUP_DETECT_FAIL, 0, (HINT32)hUpgrade, eSWUP_ERROR_IP_NO_DATA, (void*)&s_hDetectIp, sizeof(SvcSwUpdate_DetectResult_t));
	}

	s_hDetectIp.eSource = eDxSWUPDATE_SOURCE_Ip;

	hUpgrade = svc_swup_IpUpgradeCreate();
	if(hUpgrade == NULL)
	{
		return 0;
	}

	ipInfo = (SvcSwup_IpUpgradeInfo_t *)HLIB_STD_MemAlloc(sizeof(SvcSwup_IpUpgradeInfo_t));
	HxSTD_MemSet(ipInfo, 0x0, sizeof(SvcSwup_IpUpgradeInfo_t));

	eRet = svc_swup_IpUpgradeCheckSoftware(hUpgrade, pstIpParam->szUrl, ipInfo);
	if(eRet != ERR_OK)
	{
		HLIB_STD_MemFree(ipInfo);
		svc_swup_IpUpgradeDestroy(hUpgrade);
		return 0;
	}

	s_hDetectIp.bFound = TRUE;
	s_hDetectIp.bAvailable = ipInfo->bAvailable;
	HLIB_STD_StrUtf8NCpy(s_hDetectIp.data.stIpInfo.szURLPath, ipInfo->szSoftwarePath, MAX_PATH_LENGTH);
	s_hDetectIp.ulDataVersion = ipInfo->nSWVersion;

	s_hDetectIp.data.stIpInfo.bForced = ipInfo->bForced;
	s_hDetectIp.data.stIpInfo.nDescripCount = ipInfo->nDescripCount;

#if defined(CONFIG_DEBUG)
	HLIB_CMD_Printf("#### IP OTA : bFound : %d\n", s_hDetectIp.bFound);
	HLIB_CMD_Printf("#### IP OTA : bAvailable : %d\n", s_hDetectIp.bAvailable);
	HLIB_CMD_Printf("#### IP OTA : szSoftwarePath : %s\n", s_hDetectIp.data.stIpInfo.szURLPath);
	HLIB_CMD_Printf("#### IP OTA : bForced : %d\n", s_hDetectIp.data.stIpInfo.bForced);
	HLIB_CMD_Printf("#### IP OTA : nDescripCount : %d\n", s_hDetectIp.data.stIpInfo.nDescripCount);
#endif

	for( i = 0; i<MAX_DESC_SIZE; i++ )
	{
		if( s_hDetectIp.data.stIpInfo.nDescripCount > i )
		{
			HLIB_STD_StrUtf8NCpy(s_hDetectIp.data.stIpInfo.aDescripLangType[i], ipInfo->aDescripLang[i], MAX_DESC_LANG_LENGTH);
			HLIB_STD_StrUtf8NCpy(s_hDetectIp.data.stIpInfo.aDescriptions[i], ipInfo->aDescriptions[i], MAX_DESCRIPTION_LENGTH);
		}
		else
		{
			s_hDetectIp.data.stIpInfo.aDescripLangType[i][0] = '\0';
			s_hDetectIp.data.stIpInfo.aDescriptions[i][0] = '\0';
		}
#if defined(CONFIG_DEBUG)
		HLIB_CMD_Printf("#### IP OTA : aDescripLang[%d] : %s \n", i, s_hDetectIp.data.stIpInfo.aDescripLangType[i]);
		HLIB_CMD_Printf("#### IP OTA : aDescriptions[%d] : %s \n", i, s_hDetectIp.data.stIpInfo.aDescriptions[i]);
#endif
	}

//	svc_swup_UtilPostDataToMgr(eSEVT_SWUP_DETECT_SUCCESS, 0, (HINT32)hUpgrade, 0, (void*)(&s_hDetectIp), sizeof(SvcSwUpdate_DetectResult_t));
	//Emergency OTA Field Setting
	{
#if defined(CONFIG_SUPPORT_NIMA)
		NAPINetConfInfo_t info;
		if(ERR_OK == NAPI_NetConf_GetInfo(0, &info))
		{
			HUINT32 ip = 0, netmask = 0, gateway = 0, dns = 0;
			s_hDetectIp.data.stIpInfo.bHdcp = info.dhcp;
			ip = ((0xff & info.ip_info.ipv4.ipAddr[0]) << 24 |
				  (0xff & info.ip_info.ipv4.ipAddr[1]) << 16 |
				  (0xff & info.ip_info.ipv4.ipAddr[2]) << 8 |
				  (0xff & info.ip_info.ipv4.ipAddr[3]));
			netmask = ((0xff & info.ip_info.ipv4.netmask[0]) << 24 |
					  (0xff & info.ip_info.ipv4.netmask[1]) << 16 |
					  (0xff & info.ip_info.ipv4.netmask[2]) << 8 |
					  (0xff & info.ip_info.ipv4.netmask[3]));
			gateway =  ((0xff & info.ip_info.ipv4.gateway[0]) << 24 |
					  (0xff & info.ip_info.ipv4.gateway[1]) << 16 |
					  (0xff & info.ip_info.ipv4.gateway[2]) << 8 |
					  (0xff & info.ip_info.ipv4.gateway[3]));
			dns = ((0xff & info.ip_info.ipv4.gateway[0]) << 24 |
					  (0xff & info.ip_info.ipv4.gateway[1]) << 16 |
					  (0xff & info.ip_info.ipv4.gateway[2]) << 8 |
					  (0xff & info.ip_info.ipv4.gateway[3]));

			s_hDetectIp.data.stIpInfo.aucIpSetting[0] = ip;
			s_hDetectIp.data.stIpInfo.aucIpSetting[1] = netmask;
			s_hDetectIp.data.stIpInfo.aucIpSetting[2] = gateway;
			s_hDetectIp.data.stIpInfo.aucIpSetting[3] = dns;
		}
#endif
	}
	if(ipInfo->bEnable)
	{
		svc_swup_IpEmergencyParseIp(ipInfo->aServerIP, &ulPortalIp);
		memcpy(s_hDetectIp.data.stIpInfo.stEmInfo.aServerIp, (HINT8*)&ulPortalIp, sizeof(ulPortalIp));
		HLIB_STD_StrUtf8NCpy(s_hDetectIp.data.stIpInfo.stEmInfo.aUserName, ipInfo->aUserName, SWUP_USER_LENGTH );
		HLIB_STD_StrUtf8NCpy(s_hDetectIp.data.stIpInfo.stEmInfo.aPasswd, ipInfo->aPasswd, SWUP_PASSWD_LENGTH );
		HLIB_STD_StrUtf8NCpy(s_hDetectIp.data.stIpInfo.stEmInfo.aDirPath, ipInfo->aDirPath, SWUP_DIR_PATH_LENGTH );
		HLIB_STD_StrUtf8NCpy(s_hDetectIp.data.stIpInfo.stEmInfo.aFileName, ipInfo->aFileName, SWUP_FILE_NAME_LENGTH );

#if defined(CONFIG_DEBUG)
		HLIB_CMD_Printf("#### IP OTA : Emergency : aUserName : %s\n", s_hDetectIp.data.stIpInfo.stEmInfo.aUserName);
		HLIB_CMD_Printf("#### IP OTA : Emergency : aPasswd : %s\n", s_hDetectIp.data.stIpInfo.stEmInfo.aPasswd);
		HLIB_CMD_Printf("#### IP OTA : Emergency : aDirPath : %s\n", s_hDetectIp.data.stIpInfo.stEmInfo.aDirPath);
		HLIB_CMD_Printf("#### IP OTA : Emergency : aServerIp : %d.%d.%d.%d\n", s_hDetectIp.data.stIpInfo.stEmInfo.aServerIp[0], s_hDetectIp.data.stIpInfo.stEmInfo.aServerIp[1], s_hDetectIp.data.stIpInfo.stEmInfo.aServerIp[2], s_hDetectIp.data.stIpInfo.stEmInfo.aServerIp[3]);
#endif
	}

	svc_swup_UtilPostDataToMgr(eSEVT_SWUP_DETECT_SUCCESS, 0, (HINT32)hUpgrade, 0, (void*)(&s_hDetectIp), sizeof(SvcSwUpdate_DetectResult_t));

	HLIB_STD_MemFree(ipInfo);

	return (Handle_t)hUpgrade;
}

HERROR		SVC_SWUPDATE_StopDetectIp(Handle_t hDetect)
{
	svc_swup_UtilPostMsgToMgr(eSEVT_SWUP_DETECT_STOP_RESULT, 0, (HINT32)hDetect, FALSE, 0); // p2 : HBOOL TRUE : Detect Stop Success, FALSE : Detect Stop Fail
	return ERR_OK;
}

Handle_t	SVC_SWUPDATE_StartDownloadIp(SvcSwUpdate_DownloadSourceInfoIp_t *pstIpParam, HUINT8 *szTargetPath)
{
	HERROR eRet = ERR_OK;
	SvcSwup_IpUpgradeHandle_t *hUpgrade = NULL;

	if(pstIpParam == NULL)
	{
		svc_swup_UtilPostMsgToMgr(eSEVT_SWUP_DOWNLOAD_FAIL, 0,  0, eSWUP_ERROR_IP_NO_DATA, 0);
		return 0;
	}

	if(pstIpParam->szUrl == NULL || szTargetPath == NULL)
	{
		svc_swup_UtilPostMsgToMgr(eSEVT_SWUP_DOWNLOAD_FAIL, 0,  0, eSWUP_ERROR_IP_NO_DATA, 0);
		return 0;
	}

	hUpgrade = svc_swup_IpUpgradeCreate();
	if(hUpgrade == NULL)
	{
		return 0;
	}

	HLIB_TRANSPORT_HttpInit();

	eRet = svc_swup_IpUpgradeDownloadStart(hUpgrade, pstIpParam->szUrl, szTargetPath);
	if(eRet != ERR_OK)
	{
		return 0;
	}

	return (Handle_t)hUpgrade;
}

HERROR		SVC_SWUPDATE_StopDownloadIp(Handle_t hDownload)
{
	HERROR eRet = ERR_OK;
	SvcSwup_IpUpgradeHandle_t *hUpgrade = NULL;

	HLIB_TRANSPORT_HttpDestory();

	hUpgrade = (SvcSwup_IpUpgradeHandle_t *)hDownload;

	if(hUpgrade == NULL)
	{
		svc_swup_UtilPostMsgToMgr(eSEVT_SWUP_DOWNLOAD_STOP_RESULT, 0, (HINT32)hUpgrade, FALSE,    0);
		return ERR_FAIL;
	}

	eRet = svc_swup_IpUpgradeDownloadStop(hUpgrade);
	if(eRet != ERR_OK)
	{
		svc_swup_UtilPostMsgToMgr(eSEVT_SWUP_DOWNLOAD_STOP_RESULT, 0, (HINT32)hUpgrade, FALSE, 0);
		return ERR_FAIL;
	}

	svc_swup_UtilPostMsgToMgr(eSEVT_SWUP_DOWNLOAD_STOP_RESULT, 0, (HINT32)hUpgrade, TRUE, 0);

	return ERR_OK;
}

HERROR		SVC_SWUPDATE_GetImageSizeIp(Handle_t hDownload, HUINT64 *pullSize)
{
	HERROR eRet = ERR_OK;
	SvcSwup_IpUpgradeHandle_t *hUpgrade = NULL;

	hUpgrade = (SvcSwup_IpUpgradeHandle_t *)hDownload;

	if(hUpgrade == NULL)
	{
		return ERR_FAIL;
	}

	eRet = svc_swup_IpUpgradeGetSize(hUpgrade, (HFLOAT64 *)pullSize);
	if(eRet != ERR_OK)
	{
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR		SVC_SWUPDATE_GetDownloadedImageSizeIp(Handle_t hDownload, HUINT64 *pullSize)
{
	HERROR eRet = ERR_OK;
	SvcSwup_IpUpgradeHandle_t *hUpgrade = NULL;

	hUpgrade = (SvcSwup_IpUpgradeHandle_t *)hDownload;

	if(hUpgrade == NULL)
	{
		return ERR_FAIL;
	}

	eRet = svc_swup_IpUpgradeGetDownloadedSize(hUpgrade, (HFLOAT64 *)pullSize);
	if(eRet != ERR_OK)
	{
		return ERR_FAIL;
	}

	return ERR_OK;
}


