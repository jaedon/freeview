/**
	@file     xmgr_sw_update_astra.c
	@brief    file_name & simple comment.

	Description: Applicatuion for searching Services\n
	Module: Application(Layer), Menu(module) \n
	Remarks : \n
	 Date			Modification				Name\n
	-----------	-------------------------------------	------------------\n

	Copyright (c) 2012 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/


/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

#include <uapi.h>
#include <linkedlist.h>

#include <db_common.h>
#include <db_svc.h>

#include <napi_netconf.h>
#include <svc_pipe.h>
#include <svc_resources.h>
#include <svc_swup.h>
#include <svc_sys.h>

#include <bus.h>
#include <mgr_common.h>
#include <mgr_action.h>
#include <mgr_swup.h>
#include <mgr_rscmap.h>
#include <xmgr_swup.h>

#include <_mgr_swup.h>

#include <otl.h>
#define _____define_____

#define IPOTA_SERVER_URL	CONFIG_IPOTA_SERVER_URL
#define IPOTA_URL_FILE		"/url/url_ipota.txt"

typedef	struct
{

	MgrSwup_BaseContents_t		stBaseContents;

	HBOOL					bUserTP;
	DbSvc_TsInfo_t			stTsInfo;

	DxSwUpdate_Source_e			eSource;
	SvcSwUpdate_DetectParamRf_t	stDetectRfParam;
	Handle_t				hIpDetectHandle;

	Handle_t						hDownload;
	SvcSwUpdate_DownloadSourceInfoIp_t	stDownloadParamIp;

	SvcSwUpdate_EmergencyUpdateInfo_t	stSourceInfo;

	HCHAR					*pszIpDetectServerUrl;
} xmgrSwup_AstraContents_t;

#define _____Static_Function_____


STATIC HERROR xmgr_swup_MakeEmergencySignalInfo_Astra(SvcSwUpdate_EmergencyUpdateInfo_t *pstEmergeInfo, SvcSwUpdate_DetectResult_t *pstDetectResult)
{
	pstEmergeInfo->eSource	= pstDetectResult->eSource;

	if(pstDetectResult->eSource == eDxSWUPDATE_SOURCE_Rf)
	{
		HxSTD_MemCopy(&pstEmergeInfo->unInfo.stRfInfo, &pstDetectResult->data.stRfInfo, sizeof(SvcSwUpdate_RfDetectInfo_t));
		pstEmergeInfo->unInfo.stRfInfo.eDownloadType= eSWUP_DOWNLOAD_DVBSSU;
		pstEmergeInfo->unInfo.stRfInfo.usPid		= pstDetectResult->usOtaPid;
		pstEmergeInfo->unInfo.stRfInfo.eDeliType	= pstDetectResult->data.stRfInfo.stSvcTsInfo.eDeliType;
		pstEmergeInfo->ulDataVersion				= pstDetectResult->ulDataVersion;

		return ERR_OK;
	}
	else if(pstDetectResult->eSource == eDxSWUPDATE_SOURCE_Ip)
	{
		pstEmergeInfo->ulDataVersion = pstDetectResult->ulDataVersion;
		HxSTD_MemCopy(&pstEmergeInfo->unInfo.stIpInfo, &pstDetectResult->data.stIpInfo, sizeof(SvcSwUpdate_IpDetectInfo_t));

		return ERR_OK;
	}

	return ERR_FAIL;
}

// eSWUP_STATE_Init
STATIC HERROR xmgr_swup_EnterActionInit_Astra(Handle_t hSWUpdate, MgrSwup_State_e ePrevState, void *pvUserData, HINT32 nMessage, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	HERROR  eRet = ERR_OK;
	static HINT8 szDetectUrl[128];
	HINT8 *szPortaltUrl;
	HINT8 szSWVersion[32];
	HUINT32 ulSWVersion = 0, ulSystemId = 0;
	HUINT32 ulVerNum1, ulVerNum2, ulVerNum3;

	xmgrSwup_AstraContents_t	*pstContents = (xmgrSwup_AstraContents_t*)pvUserData;

	szPortaltUrl = (HUINT8 *)UAPI_FM_GetDefaultURL((HCHAR *)IPOTA_SERVER_URL, (HCHAR *)IPOTA_URL_FILE);
	if(szPortaltUrl == NULL)
	{
		HxLOG_Error("szPortaltUrl is NULL \n");
		return ERR_FAIL;
	}

	HxLOG_Trace();

	if (NULL == pstContents)
	{
		HxLOG_Error("pstContents is NULL \n");
		return ERR_FAIL;
	}

	HxLOG_Debug("====== (+) \n");
	HxLOG_Debug("bUserTP (%d) \n", pstContents->bUserTP);

	if (pstContents->bUserTP == TRUE)
	{
		MGR_SWUPINFO_ResetHomeTp();
		MGR_SWUPINFO_AddOtaHomeTp(&pstContents->stTsInfo);
	}
	else
	{
		MGR_SWUP_SetDefaultTp();
	}

	eRet = SVC_SYS_GetSystemVersion(eVERSION_TYPE_APPL, &ulSWVersion);
	if(eRet != ERR_OK)
	{
		HxLOG_Error("SVC_SYS_GetSystemVersion is Error \n");
		return ERR_FAIL;
	}

	eRet = SVC_SYS_GetSystemId(&ulSystemId);
	if (eRet != ERR_OK)
	{
		HxLOG_Error("SVC_SYS_GetSystemId is Error \n");
		return ERR_FAIL;
	}

	ulVerNum1 = (ulSWVersion >> 16);
	ulVerNum2 = (ulSWVersion >> 8) & 0xFF;
	ulVerNum3 = (ulSWVersion) & 0xFF;

	snprintf(szSWVersion, 32, "%d.%02d.%02d", ulVerNum1, ulVerNum2, ulVerNum3);
	snprintf(szDetectUrl, 128, "%s/SWUpdate/CheckNewSW?SystemID=%04X.%04X&SWVersion=%s", szPortaltUrl, (ulSystemId >> 16) & 0xFFFF, (ulSystemId & 0xFFFF),  szSWVersion);

	pstContents->pszIpDetectServerUrl = szDetectUrl;
	pstContents->eSource = nParam1;

#if defined(CONFIG_DEBUG)
		HxLOG_Debug("eSource(%s) szDetectUrl (%s) \n", XMGR_SWUP_GetEnumStrOfSource(pstContents->eSource), szDetectUrl);
#endif

	{
		HERROR hErr;
		SvcSwUpdate_EmergencyUpdateInfo_t	*pstEmergeInfo;
		pstEmergeInfo   = (SvcSwUpdate_EmergencyUpdateInfo_t*)HLIB_STD_MemAlloc(sizeof(SvcSwUpdate_EmergencyUpdateInfo_t));

		HxLOG_Debug("Initialization Emergency Info...\n");
		// detect 성공시 signaling 정보를 nvram 에 set 해준다.
		HxSTD_MemSet(pstEmergeInfo, 0, sizeof(SvcSwUpdate_EmergencyUpdateInfo_t));

		hErr = DB_PARAM_SetItem(eDB_PARAM_ITEM_OTA_EMERGENCYINFO, (HUINT32)pstEmergeInfo, sizeof(SvcSwUpdate_EmergencyUpdateInfo_t));
		if (ERR_OK != hErr)
		{
			HxLOG_Error("DB_PARAM_SetItem() return Error : result [0x%x] in OTA Emergency Setting\n", hErr);
		}
		HLIB_STD_MemFree(pstEmergeInfo);
	}

	return ERR_OK;
}

// eSWUP_STATE_Connect
STATIC HERROR xmgr_swup_EnterActionConnect_Astra(Handle_t hSWUpdate, MgrSwup_State_e ePrevState, void *pvUserData, HINT32 nMessage, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	xmgrSwup_AstraContents_t		*pstContents = (xmgrSwup_AstraContents_t*)pvUserData;

	HxLOG_Trace();
	if (NULL == pstContents)
	{
		HxLOG_Error("pstContents is NULL \n");
		return ERR_FAIL;
	}

#if defined(CONFIG_DEBUG)
	HxLOG_Debug("eSource (%s) \n", XMGR_SWUP_GetEnumStrOfSource(pstContents->eSource));
#endif

	switch (pstContents->eSource)
	{
		case eDxSWUPDATE_SOURCE_Usb:
			MGR_SWUP_PostMessage(hSWUpdate, eMEVT_SWUP_DETECT_START, HANDLE_NULL, eDxSWUPDATE_SOURCE_Usb, 0, 0);
			break;

		case eDxSWUPDATE_SOURCE_Ip:
			MGR_SWUP_PostMessage(hSWUpdate, eMEVT_SWUP_DETECT_START, HANDLE_NULL, eDxSWUPDATE_SOURCE_Ip, (HINT32)pstContents->pszIpDetectServerUrl, 0);
			break;

		case eDxSWUPDATE_SOURCE_Rf:
			MGR_SWUP_PostMessage(hSWUpdate, eMEVT_SWUP_DETECT_START, HANDLE_NULL, eDxSWUPDATE_SOURCE_Rf, (HINT32)&pstContents->stDetectRfParam, 0);
			break;

		default:
			HxLOG_Error("Invalid eSource Type...\n");
			break;
	}

	return ERR_OK;
}

// eSWUP_STATE_Detect
STATIC HERROR xmgr_swup_EnterActionDetect_Astra(Handle_t hSWUpdate, MgrSwup_State_e ePrevState, void *pvUserData, HINT32 nMessage, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	xmgrSwup_AstraContents_t		*pstContents = (xmgrSwup_AstraContents_t *)pvUserData;

	if (NULL == pstContents)
	{
		HxLOG_Error("pstContents is NULL \n");
		return ERR_FAIL;
	}

#if defined(CONFIG_DEBUG)
	HxLOG_Debug("hSwUpHandle(0x%x) eSource(%s) \n", hSWUpdate, XMGR_SWUP_GetEnumStrOfSource(pstContents->eSource));
#endif

	switch(pstContents->eSource)
	{
		case eDxSWUPDATE_SOURCE_Rf:
			{
				HxLOG_Debug("eSource : RF\n");
				MGR_SWUP_DetectCreate(0, hSWUpdate);
			}
			break;
		case eDxSWUPDATE_SOURCE_Ip:
			{
				HxLOG_Debug("eSource : IP\n");
				MGR_SWUP_IpCreate(hSWUpdate, eSWUP_STATE_Detect, pstContents->pszIpDetectServerUrl);
			}
			break;

		case eDxSWUPDATE_SOURCE_Usb:
			{
				HxLOG_Debug("eSource : USB\n");
				MGR_SWUP_CreateUsb(hSWUpdate);
			}
			break;

		default:
			HxLOG_Error("Invalid eSource Type...\n");
			break;
	}

	return ERR_OK;
}

// eSWUP_STATE_Download
STATIC HERROR xmgr_swup_EnterActionDownload_Astra(Handle_t hSWUpdate, MgrSwup_State_e ePrevState, void *pvUserData, HINT32 nMessage, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	xmgrSwup_AstraContents_t		*pstContents = (xmgrSwup_AstraContents_t*)pvUserData;
	SvcSwUpdate_DetectResult_t		*pstDetectResult;
	HERROR 						 hErr;

	HxLOG_Trace();
	if (NULL == pstContents)
		return ERR_FAIL;

	pstDetectResult = (SvcSwUpdate_DetectResult_t*)HLIB_STD_MemCalloc(sizeof(SvcSwUpdate_DetectResult_t));

	MGR_SWUP_GetDetectInfo(hSWUpdate, pstDetectResult);
	pstContents->eSource = pstDetectResult->eSource;

	switch(pstDetectResult->eSource)
	{
		case eDxSWUPDATE_SOURCE_Usb:
			{
				static	HCHAR	*pszSoftwarePath = NULL;
				if (pszSoftwarePath == NULL)
					pszSoftwarePath = (HCHAR*)HLIB_STD_MemAlloc(MAX_PATH_LENGTH);

				HxSTD_MemSet(pszSoftwarePath, 0, MAX_PATH_LENGTH);
				HLIB_STD_StrNCpySafe(pszSoftwarePath, pstDetectResult->data.stUsbInfo.szSoftwarePath, MAX_PATH_LENGTH - 1);
				MGR_SWUP_PostMessage((Handle_t)hSWUpdate, eMEVT_SWUP_NOTIFY_DOWNLOAD_SUCCESS, (Handle_t)NULL, 0, (HINT32)pszSoftwarePath, 0);
			}
			break;

		case eDxSWUPDATE_SOURCE_Rf:
			// TODO: Call RF Download
			{
				HUINT32 			ulActionId;
				Handle_t				hActHandle;
				HERROR			hErr;
				SvcRsc_Info_t 	stRscInfo;
				DxTuneParam_t stChTunningInfo;


				hErr = MGR_ACTION_GetEmptyBgActionId(&ulActionId);
				if (hErr != ERR_OK)
				{
					HxLOG_Error("cannot get action id \n");
					MGR_SWUP_PostMessage(hSWUpdate, eMEVT_SWUP_NOTIFY_DOWNLOAD_FAIL, eSWUP_STATE_Unknown, 0,0, eSWUP_FAILREASON_Action);
					break;
				}

				OTL_CONV_DbTuningInfo2DxTuneParam((const DbSvc_TsInfo_t *)&pstDetectResult->data.stRfInfo.stSvcTsInfo, &stChTunningInfo);

				hErr = MGR_ACTION_SetupSearch(ulActionId, &stChTunningInfo, eAmPriority_SEARCH);
				if (ERR_OK != hErr)
				{
					HxLOG_Critical("cannot setup search\n");
					MGR_SWUP_PostMessage(hSWUpdate, eMEVT_SWUP_NOTIFY_DOWNLOAD_FAIL, eSWUP_STATE_Unknown, 0,0, eSWUP_FAILREASON_Action);
					break;
				}
				hErr = MGR_ACTION_Take(ulActionId, eActionType_SEARCH, BUS_MGR_GetMgrCallback(BUS_MGR_GetThis()));
				if (ERR_OK != hErr)
				{
					HxLOG_Critical("cannot take action\n");
					MGR_SWUP_PostMessage(hSWUpdate, eMEVT_SWUP_NOTIFY_DOWNLOAD_FAIL, eSWUP_STATE_Unknown, 0,0, eSWUP_FAILREASON_Action);
					break;
				}

				hActHandle = SVC_PIPE_GetActionHandle(ulActionId);

				MGR_RSC_GetResourceInfo(ulActionId, &stRscInfo);

				hErr = SVC_CH_StartTune(hActHandle, &stRscInfo, (HUINT16)-1, &stChTunningInfo);
				if (hErr != ERR_OK)
				{
					HxLOG_Critical("Error!!!!\n");
					break;
				}

				hErr = MGR_SWUP_DownloadCreate(hActHandle, eDxSWUPDATE_SOURCE_Rf, pstDetectResult->ulDataVersion, pstDetectResult->usOtaPid, hSWUpdate, FALSE);

			}
			break;
		case eDxSWUPDATE_SOURCE_Ip:
			{
				static HCHAR	*pszTempURL = NULL;
				if (pszTempURL == NULL)
					pszTempURL = (HCHAR*)HLIB_STD_MemAlloc(MAX_PATH_LENGTH);
				HLIB_STD_StrNCpySafe(pszTempURL, pstDetectResult->data.stIpInfo.szURLPath, MAX_PATH_LENGTH - 1);
				MGR_SWUP_IpCreate(hSWUpdate, eSWUP_STATE_Download, pszTempURL);
			}
			break;

		default:
			break;
	}

	HLIB_STD_MemFree(pstDetectResult);

	return ERR_OK;
}

// eSWUP_STATE_Install
STATIC HERROR xmgr_swup_EnterActionInstall_Astra(Handle_t hSWUpdate, MgrSwup_State_e ePrevState, void *pvUserData, HINT32 nMessage, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	xmgrSwup_AstraContents_t				*pstContents = (xmgrSwup_AstraContents_t*)pvUserData;
	SvcSwUpdate_EmergencyUpdateInfo_t	*pstSourceInfo;
	Handle_t							 hSvc;
	HUINT8								*pszSourceFile;

	HxLOG_Trace();
	if (NULL == pstContents)
		return ERR_FAIL;

	hSvc = (HINT32)hSWUpdate;
	pszSourceFile = DDB_SAVE_FILE_PATH;

//	nParam2 = (HINT32)pstContents->pszSourceFileName;
//	nParam2 = (HINT32)"/tmp/hdr1000s_upgrade_app.hdf";
	pstSourceInfo = &pstContents->stSourceInfo;

	pstSourceInfo->eSource = pstContents->eSource;

	//	아래 값을들 다 채워 줘여 Emergency OTA가 동작한다.
	//	Emergency OTA는 Freesat Spec이 아님.
	switch (pstSourceInfo->eSource)
	{
		case eDxSWUPDATE_SOURCE_Usb:
			pszSourceFile = (HUINT8 *)nParam2;
			break;

		case eDxSWUPDATE_SOURCE_Rf:
			HxLOG_Debug("eDxSWUPDATE_SOURCE_Rf Install....\n");
			pstSourceInfo->unInfo.stRfInfo.eDownloadType = eSWUP_DOWNLOAD_DVBSSU;
			//pstSourceInfo->unInfo.stRfInfo.eDeliType
			//pstSourceInfo->unInfo.stRfInfo.stTuningInfo
			//pstSourceInfo->unInfo.stRfInfo.usPid
			break;
		case eDxSWUPDATE_SOURCE_Ip:
			{
				HxLOG_Print("eDxSWUPDATE_SOURCE_Ip\n");

				// TODO: Get IP info from LOG API
				pstSourceInfo->unInfo.stIpInfo.bHdcp = TRUE;
				{
#if defined(CONFIG_SUPPORT_NIMA)
					NAPINetConfInfo_t info;
					if(ERR_OK == NAPI_NetConf_GetInfo(0, &info))
					{
						HUINT32 ip = 0, netmask = 0, gateway = 0, dns = 0;
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

						pstSourceInfo->unInfo.stIpInfo.aucIpSetting[0] = ip;
						pstSourceInfo->unInfo.stIpInfo.aucIpSetting[1] = netmask;
						pstSourceInfo->unInfo.stIpInfo.aucIpSetting[2] = gateway;
						pstSourceInfo->unInfo.stIpInfo.aucIpSetting[3] = dns;
					}
#endif
				}

				// TODO: Set Portal IP
				// stSourceInfo.unInfo.stIpInfo.ulPortalIp = ;
				// TODO: Set Download Path
				// snprintf(stSourceInfo.unInfo.stIpInfo.aucDownPath, 44, "");
			}
			break;
		default:
			break;
	}

	MGR_SWUP_InstallStart(hSvc, pszSourceFile, pstSourceInfo);

	return ERR_OK;
}

// eSWUP_STATE_Error
STATIC HERROR xmgr_swup_EnterActionError_Astra(Handle_t hSWUpdate, MgrSwup_State_e ePrevState, void *pvUserData, HINT32 nMessage, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	xmgrSwup_AstraContents_t		*pstContents = (xmgrSwup_AstraContents_t*)pvUserData;
	// nParam3 에는 mgr단의 fail reason, nParam1에는 svc단의 fail reason... 이 부분 정리.

	if (NULL == pstContents)
		return ERR_FAIL;

	BUS_PostMessageToOM(NULL, eMEVT_DN_SWUP_ERROR, HANDLE_NULL, (HINT32)hSWUpdate, 0, nParam3);

	return ERR_OK;
}

// eSWUP_STATE_Finish
STATIC HERROR xmgr_swup_EnterActionFinish_Astra(Handle_t hSWUpdate, MgrSwup_State_e ePrevState, void *pvUserData, HINT32 nMessage, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	xmgrSwup_AstraContents_t		*pstContents = (xmgrSwup_AstraContents_t*)pvUserData;

	if (NULL == pstContents)
		return ERR_FAIL;


	BUS_PostMessageToOM(NULL, eMEVT_DN_SWUP_FINISH, HANDLE_NULL, (HINT32)hSWUpdate, 0, 0);

	return ERR_OK;
}

// eSWUP_STATE_Init
STATIC HERROR xmgr_swup_LeaveActionInit_Astra(Handle_t hSWUpdate, MgrSwup_State_e eNextState, void *pvUserData, HINT32 nMessage, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	xmgrSwup_AstraContents_t		*pstContents = (xmgrSwup_AstraContents_t*)pvUserData;

	if (NULL == pstContents)
	{
		HxLOG_Error("pstContents is NULL \n");
		return ERR_FAIL;
	}

//	MGR_SWUP_PostMessage(ulTag, eMEVT_SWUP_DETECT_START, ulTag, (HINT32)eDxSWUPDATE_SOURCE_Ip, (HINT32)&pstContents->stDetectParamIp, 0);

	return ERR_OK;
}

// eSWUP_STATE_Connect
STATIC HERROR xmgr_swup_LeaveActionConnect_Astra(Handle_t hSWUpdate, MgrSwup_State_e eNextState, void *pvUserData, HINT32 nMessage, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	xmgrSwup_AstraContents_t		*pstContents = (xmgrSwup_AstraContents_t*)pvUserData;

	if (NULL == pstContents)
	{
		HxLOG_Error("pstContents is NULL \n");
		return ERR_FAIL;
	}

	// TODO: Destory Connect Action

//	MGR_SWUP_PostMessage(ulTag, eMEVT_SWUP_DETECT_START, ulTag, (HINT32)eDxSWUPDATE_SOURCE_Ip, (HINT32)&pstContents->stDetectParamIp, 0);

	return ERR_OK;
}

// eSWUP_STATE_Detect
STATIC HERROR xmgr_swup_LeaveActionDetect_Astra(Handle_t hSWUpdate, MgrSwup_State_e eNextState, void *pvUserData, HINT32 nMessage, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	xmgrSwup_AstraContents_t		*pstContents = (xmgrSwup_AstraContents_t*)pvUserData;

	if (NULL == pstContents)
	{
		HxLOG_Error("pstContents is NULL \n");
		return ERR_FAIL;
	}

	if(nMessage == eMEVT_SWUP_NOTIFY_DETECT_SUCCESS)
	{
		SvcSwUpdate_EmergencyUpdateInfo_t	*pstEmergeInfo;
		SvcSwUpdate_DetectResult_t			*pstDetectResult;
		HUINT32 						ulSystemVersion;

		pstEmergeInfo	= (SvcSwUpdate_EmergencyUpdateInfo_t*)HLIB_STD_MemAlloc(sizeof(SvcSwUpdate_EmergencyUpdateInfo_t));
		pstDetectResult = (SvcSwUpdate_DetectResult_t*)HLIB_STD_MemAlloc(sizeof(SvcSwUpdate_DetectResult_t));

		SVC_SYS_GetSystemVersion(eVERSION_TYPE_APPL, &ulSystemVersion);
		MGR_SWUP_GetDetectInfo(hSWUpdate, pstDetectResult);

		// detect 성공시 signaling 정보를 nvram 에 set 해준다.
		HxSTD_MemSet(pstEmergeInfo, 0, sizeof(SvcSwUpdate_EmergencyUpdateInfo_t));
		if (xmgr_swup_MakeEmergencySignalInfo_Astra(pstEmergeInfo, pstDetectResult) == ERR_OK)
		{
			HERROR hErr;

			SVC_SWUPDATE_InstallWriteEmergencySignalInfo(pstEmergeInfo);

			hErr = DB_PARAM_SetItem(eDB_PARAM_ITEM_OTA_EMERGENCYINFO, (HUINT32)pstEmergeInfo, sizeof(SvcSwUpdate_EmergencyUpdateInfo_t));
			if (ERR_OK == hErr)
			{
				DB_PARAM_Sync();
				HxLOG_Error("DB_PARAM_Sync() result [0x%x] in OTA Emergency Setting\n", hErr);
			}
		}
		BUS_PostMessageToOM(NULL, eMEVT_DN_SWUP_DETECT_SUCCESS, HANDLE_NULL, (HINT32)hSWUpdate, ulSystemVersion,	(HINT32)pstDetectResult);


		HxLOG_Debug("Destroy OTA DetectAction Proc!! sourceType (0x%X)\n",pstContents->eSource);
		switch(pstContents->eSource)
		{
			case eDxSWUPDATE_SOURCE_Usb:		MGR_SWUP_DestroyUsb();					break;
			case eDxSWUPDATE_SOURCE_Ip:			MGR_SWUP_IpDestroy(hSWUpdate);			break;
			case eDxSWUPDATE_SOURCE_Rf:			MGR_SWUP_DetectDestroy();				break;
			default:		break;
		}

		pstContents->eSource = eDxSWUPDATE_SOURCE_None;

		//	free는 EVT_DN_SWUP_DETECT_SUCCESS를 받는 곳에서 수행.
		//	BUS상태에 따라서 메모리 릭이 있을수도 있지만, 담당자가 수정해 주겠지..
		HLIB_STD_MemFree(pstEmergeInfo);
	}
	else
	{
		switch(pstContents->eSource)
		{
		case eDxSWUPDATE_SOURCE_Usb:
			HxLOG_Debug("eSWUP_STATE_Detect : DETECT_FAIL!!!\n\tRetry IP....\n");
			MGR_SWUP_DestroyUsb();
			pstContents->eSource = eDxSWUPDATE_SOURCE_Ip;
			break;

		case eDxSWUPDATE_SOURCE_Ip:
			HxLOG_Debug("eSWUP_STATE_Detect : DETECT_FAIL!!!\n\tRetry RF....\n");
			MGR_SWUP_IpDestroy(hSWUpdate);
			pstContents->eSource = eDxSWUPDATE_SOURCE_Rf;
			break;

		case eDxSWUPDATE_SOURCE_Rf:
			HxLOG_Debug("eSWUP_STATE_Detect : DETECT_FAIL!!!\n");
			BUS_PostMessageToOM(NULL, eMEVT_DN_SWUP_DETECT_FAIL, HANDLE_NULL, (HINT32)hSWUpdate, 0, 0);
			pstContents->eSource = eDxSWUPDATE_SOURCE_None;
			MGR_SWUP_DetectDestroy();
			break;

		default:
			return ERR_FAIL;

		}
	}

	if(pstContents->eSource== eDxSWUPDATE_SOURCE_None)
	{
		BUS_PostMessage((BUS_Callback_t)NULL, eMEVT_ACTION_NOTIFY_RESULT, (Handle_t)NULL, eMGR_EVENT_SWUP_DETECT_FINISH, 0, 0);
	}

	return ERR_OK;
}

// eSWUP_STATE_Download
STATIC HERROR xmgr_swup_LeaveActionDownload_Astra(Handle_t hSWUpdate, MgrSwup_State_e eNextState, void *pvUserData, HINT32 nMessage, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	xmgrSwup_AstraContents_t		*pstContents = (xmgrSwup_AstraContents_t*)pvUserData;
	HERROR						hErr;

	HxLOG_Trace();
	if (NULL == pstContents)
	{
		HxLOG_Error("pstContents is NULL \n");
		return ERR_FAIL;
	}

#if defined(CONFIG_DEBUG)
	HxLOG_Print("[%s][%04d]  hSwUpHandle(0x%x) Source (%s) Msg(%s) \n",
	__FUNCTION__, __LINE__, hSWUpdate, XMGR_SWUP_GetEnumStrOfSource(pstContents->eSource), XMGR_SWUP_GetEnumStrOfMessage(nMessage));
#endif

	if(nMessage == eMEVT_SWUP_NOTIFY_DOWNLOAD_SUCCESS)
	{
		BUS_PostMessageToOM(NULL, eMEVT_DN_SWUP_DOWNLOAD_SUCCESS, HANDLE_NULL, (HINT32)hSWUpdate, 0, 0);
	}
	else if(nMessage == eMEVT_SWUP_NOTIFY_DOWNLOAD_FAIL)
	{
		BUS_PostMessageToOM(NULL, eMEVT_DN_SWUP_DOWNLOAD_FAIL, HANDLE_NULL, (HINT32)hSWUpdate, 0, 0);
	}

	switch(pstContents->eSource)
	{
	case eDxSWUPDATE_SOURCE_Rf:
		hErr = MGR_SWUP_DownloadDestroy(eDxSWUPDATE_SOURCE_Rf);

		//	Download Destroy이에 Release Action
		MGR_ACTION_Release(eActionId_VIEW_0, eActionType_SEARCH);
		BUS_PostMessage(NULL, eMEVT_ACTION_NOTIFY_RESULT, (Handle_t)NULL,  0, 0, 0);
		break;

		break;
	case eDxSWUPDATE_SOURCE_Ip:
		hErr = MGR_SWUP_DownloadDestroy(eDxSWUPDATE_SOURCE_Ip);
		break;

	default:
		break;
	}

	if((nMessage == eMEVT_SWUP_NOTIFY_DOWNLOAD_SUCCESS) || (nMessage == eMEVT_SWUP_NOTIFY_DOWNLOAD_FAIL))
	{
		BUS_PostMessage((BUS_Callback_t)NULL, eMEVT_ACTION_NOTIFY_RESULT, (Handle_t)NULL, eMGR_EVENT_SWUP_DOWNLOAD_FINISH, 0, 0);
	}

	return ERR_OK;
}

// eSWUP_STATE_Install
STATIC HERROR xmgr_swup_LeaveActionInstall_Astra(Handle_t hSWUpdate, MgrSwup_State_e eNextState, void *pvUserData, HINT32 nMessage, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	xmgrSwup_AstraContents_t		*pstContents = (xmgrSwup_AstraContents_t*)pvUserData;

	HxLOG_Trace();
	if (NULL == pstContents)
	{
		HxLOG_Error("pstContents is NULL \n");
		return ERR_FAIL;
	}

	if(nMessage == eMEVT_SWUP_INSTALL_SUCCESS)
		BUS_PostMessageToOM(NULL, eMEVT_DN_SWUP_INSTALL_SUCCESS, HANDLE_NULL, (HINT32)hSWUpdate, 0, 0);
	else if(nMessage == eMEVT_SWUP_NOTIFY_INSTALL_FAIL)
		BUS_PostMessageToOM(NULL, eMEVT_DN_SWUP_INSTALL_FAIL, HANDLE_NULL, (HINT32)hSWUpdate, 0, 0);
	if(nMessage == eMEVT_SWUP_INSTALL_SUCCESS || nMessage == eMEVT_SWUP_NOTIFY_INSTALL_FAIL)
	{
		BUS_PostMessage((BUS_Callback_t)NULL, eMEVT_ACTION_NOTIFY_RESULT, (Handle_t)NULL, eMGR_EVENT_SWUP_INSTALL_FINISH, 0, 0);
	}

	MGR_SWUP_InstallStop();

	return ERR_OK;
}

// eSWUP_STATE_Error
STATIC HERROR xmgr_swup_LeaveActionError_Astra(Handle_t hSWUpdate, MgrSwup_State_e eNextState, void *pvUserData, HINT32 nMessage, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	xmgrSwup_AstraContents_t		*pstContents = (xmgrSwup_AstraContents_t*)pvUserData;

	HxLOG_Trace();
	if (NULL == pstContents)
		return ERR_FAIL;

	return ERR_OK;
}

// eSWUP_STATE_Finish
STATIC HERROR xmgr_swup_LeaveActionFinish_Astra(Handle_t hSWUpdate, MgrSwup_State_e eNextState, void *pvUserData, HINT32 nMessage, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	xmgrSwup_AstraContents_t		*pstContents = (xmgrSwup_AstraContents_t*)pvUserData;

	HxLOG_Trace();
	if (NULL == pstContents)
		return ERR_FAIL;

	return ERR_OK;
}

BUS_Result_t	XMGR_SWUP_EnterAction_Astra(Handle_t hSWUpdate, MgrSwup_State_e eCurrState, MgrSwup_State_e ePrevState, void *pvUserData, HINT32 nMessage, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	xmgrSwup_AstraContents_t	*pstContents = (xmgrSwup_AstraContents_t*)pvUserData;
	BUS_Result_t			 hRes = MESSAGE_PASS;
	HERROR					 hErr = ERR_FAIL;

	if (NULL == pstContents)
	{
		HxLOG_Error("pstContents is NULL \n");
		return MESSAGE_BREAK;
	}

#if defined(CONFIG_DEBUG)
	HxLOG_Print("[%s][%04d] Change State (%s  -> %s)  \n", __FUNCTION__, __LINE__, XMGR_SWUP_GetEnumStrOfState(ePrevState), XMGR_SWUP_GetEnumStrOfState(eCurrState));
#endif

	switch (eCurrState)
	{
	case eSWUP_STATE_Init:
		hErr = xmgr_swup_EnterActionInit_Astra(hSWUpdate, ePrevState, pvUserData, nMessage, hAction, nParam1, nParam2, nParam3);
		break;
	case eSWUP_STATE_Connect:
		hErr = xmgr_swup_EnterActionConnect_Astra(hSWUpdate, ePrevState, pvUserData, nMessage, hAction, nParam1, nParam2, nParam3);
		break;
	case eSWUP_STATE_Detect:
		hErr = xmgr_swup_EnterActionDetect_Astra(hSWUpdate, ePrevState, pvUserData, nMessage, hAction, nParam1, nParam2, nParam3);
		break;
	case eSWUP_STATE_Download:
		hErr = xmgr_swup_EnterActionDownload_Astra(hSWUpdate, ePrevState, pvUserData, nMessage, hAction, nParam1, nParam2, nParam3);
		break;
	case eSWUP_STATE_Install:
		hErr = xmgr_swup_EnterActionInstall_Astra(hSWUpdate, ePrevState, pvUserData, nMessage, hAction, nParam1, nParam2, nParam3);
		break;
	case eSWUP_STATE_Error:
		hErr = xmgr_swup_EnterActionError_Astra(hSWUpdate, ePrevState, pvUserData, nMessage, hAction, nParam1, nParam2, nParam3);
		break;
	case eSWUP_STATE_Finish:
		hErr = xmgr_swup_EnterActionFinish_Astra(hSWUpdate, ePrevState, pvUserData, nMessage, hAction, nParam1, nParam2, nParam3);
		break;
	default:
		break;
	}

	return hRes;
}

BUS_Result_t	XMGR_SWUP_LeaveAction_Astra(Handle_t hSWUpdate, MgrSwup_State_e eCurrState, MgrSwup_State_e eNextState, void *pvUserData, HINT32 nMessage, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	xmgrSwup_AstraContents_t		*pstContents = (xmgrSwup_AstraContents_t*)pvUserData;
	BUS_Result_t			 hRes = MESSAGE_PASS;
	HERROR					 hErr = ERR_FAIL;

	if (NULL == pstContents)
	{
		HxLOG_Error("pstContents is NULL \n");
		return MESSAGE_BREAK;
	}

	switch (eCurrState)
	{
	case eSWUP_STATE_Init:
		hErr = xmgr_swup_LeaveActionInit_Astra(hSWUpdate, eNextState, pvUserData, nMessage, hAction, nParam1, nParam2, nParam3);
		break;
	case eSWUP_STATE_Connect:
		hErr = xmgr_swup_LeaveActionConnect_Astra(hSWUpdate, eNextState, pvUserData, nMessage, hAction, nParam1, nParam2, nParam3);
		break;
	case eSWUP_STATE_Detect:
		hErr = xmgr_swup_LeaveActionDetect_Astra(hSWUpdate, eNextState, pvUserData, nMessage, hAction, nParam1, nParam2, nParam3);
		break;
	case eSWUP_STATE_Download:
		hErr = xmgr_swup_LeaveActionDownload_Astra(hSWUpdate, eNextState, pvUserData, nMessage, hAction, nParam1, nParam2, nParam3);
		break;
	case eSWUP_STATE_Install:
		hErr = xmgr_swup_LeaveActionInstall_Astra(hSWUpdate, eNextState, pvUserData, nMessage, hAction, nParam1, nParam2, nParam3);
		break;
	case eSWUP_STATE_Error:
		hErr = xmgr_swup_LeaveActionError_Astra(hSWUpdate, eNextState, pvUserData, nMessage, hAction, nParam1, nParam2, nParam3);
		break;
	case eSWUP_STATE_Finish:
		hErr = xmgr_swup_LeaveActionFinish_Astra(hSWUpdate, eNextState, pvUserData, nMessage, hAction, nParam1, nParam2, nParam3);
		break;
	default:
		break;
	}

	return hRes;
}

MgrSwup_State_e XMGR_SWUP_GetNextState_Astra(Handle_t hSWUpdate, MgrSwup_State_e eState, void *pvUserData, HINT32 nMessage, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	xmgrSwup_AstraContents_t	*pstContents = (xmgrSwup_AstraContents_t*)pvUserData;
	MgrSwup_State_e				 eNextState = eSWUP_STATE_Unknown;

	HxLOG_Trace();
	HxLOG_Debug("eState : [%d]\n", eState);

#if defined(CONFIG_DEBUG)
	HxLOG_Print("[%s][%04d] (+) Current eState[%s] Message [%s]\n", __FUNCTION__, __LINE__, XMGR_SWUP_GetEnumStrOfState(eState), XMGR_SWUP_GetEnumStrOfMessage(nMessage));
#endif

	if(nMessage == eMEVT_SWUP_CANCEL || nMessage == eMEVT_SWUP_DOWNLOAD_STOP)
	{
		eNextState = eSWUP_STATE_Finish;
		return eNextState;
	}

	switch (eState)
	{
		case eSWUP_STATE_Init:
			{
				switch (nMessage)
				{
					case eMEVT_SWUP_CONNECT_START:
						eNextState = eSWUP_STATE_Connect;
						break;

					case eMEVT_SWUP_DOWNLOAD_START:
						eNextState = eSWUP_STATE_Download;
						break;

					case eMEVT_SWUP_INSTALL_START:
						eNextState = eSWUP_STATE_Install;
						break;

					default:
						HxLOG_Error("eSWUP_STATE_Init :: Invalid nMessage [%x]\n", nMessage);
						break;
				}
			}
			break;

		case eSWUP_STATE_Connect:
			switch (nMessage)
			{
				case eMEVT_SWUP_DETECT_START:
					eNextState = eSWUP_STATE_Detect;
					break;

				default:
					HxLOG_Error("eSWUP_STATE_Connect :: Invalid nMessage [%x]\n", nMessage);
					break;
			}
			break;

		case eSWUP_STATE_Detect:
			{
				switch (nMessage)
				{
					case eMEVT_SWUP_NOTIFY_DETECT_SUCCESS:
						if(pstContents->stBaseContents.eBatchProcess == eDxBAT_TYPE_STANDBY)
						{
							eNextState = eSWUP_STATE_Download;
						}
						else
						{
							eNextState = eSWUP_STATE_Finish;
						}
						break;
					case eMEVT_SWUP_NOTIFY_DETECT_FAIL:
						{
						switch(pstContents->eSource)
							{
							case eDxSWUPDATE_SOURCE_Usb:
								eNextState = eSWUP_STATE_Connect;
								break;
							case eDxSWUPDATE_SOURCE_Ip:
								eNextState = eSWUP_STATE_Connect;
								break;
							case eDxSWUPDATE_SOURCE_Rf:
								eNextState = eSWUP_STATE_Error;
								break;
							default:
								eNextState = eSWUP_STATE_Error;
								HxLOG_Error("eSWUP_STATE_Detect :: Invalid eSource [%x]\n", pstContents->eSource);
								break;
							}
						}
						break;

					default:
						HxLOG_Error("eSWUP_STATE_Detect :: Invalid nMessage [%x]\n", nMessage);
						break;
				}
			}
			break;

		case eSWUP_STATE_Download:
			{
				switch (nMessage)
				{
					case eMEVT_SWUP_NOTIFY_DOWNLOAD_SUCCESS:
						eNextState = XMGR_SWUP_GetNextState_AfterDownload(pstContents->eSource);
						break;
					case eMEVT_SWUP_NOTIFY_DOWNLOAD_FAIL:
						eNextState = eSWUP_STATE_Error;
						break;

					case eMEVT_SWUP_NOTIFY_DOWNLOAD_PROGRESS:
						if (pstContents->eSource == eDxSWUPDATE_SOURCE_Rf)
						{
							BUS_PostMessageToOM(NULL, eMEVT_DN_SWUP_DOWNLOAD_PROGRESS, HANDLE_NULL, (HINT32)hSWUpdate, nParam1, 100);
						}
						else
						{
							pstContents->hDownload = nParam3;
							BUS_PostMessageToOM(NULL, eMEVT_DN_SWUP_DOWNLOAD_PROGRESS, HANDLE_NULL, (HINT32)hSWUpdate, nParam2, 100);
						}
						eNextState = eSWUP_STATE_Download;
						break;
					default:
						HxLOG_Error("eSWUP_STATE_Download :: Invalid nMessage [%x]\n", nMessage);
						break;
				}
			}

			break;

		case eSWUP_STATE_Install:
			{
				switch (nMessage)
				{
					case eMEVT_SWUP_INSTALL_SUCCESS:
						eNextState = eSWUP_STATE_Finish;
						break;
					case eMEVT_SWUP_NOTIFY_INSTALL_FAIL:
						eNextState = eSWUP_STATE_Error;
						break;
					case eMEVT_SWUP_NOTIFY_INSTALL_PROGRESS:
						BUS_PostMessageToOM(NULL, eMEVT_DN_SWUP_INSTALL_PROGRESS, HANDLE_NULL, (HINT32)hSWUpdate, nParam2, 100);
						eNextState = eSWUP_STATE_Install;
						break;
					default:
						HxLOG_Error("eSWUP_STATE_Install :: Invalid nMessage [%x]\n", nMessage);
						break;
				}
			}
			break;

		case eSWUP_STATE_Error:
			eNextState = eSWUP_STATE_Finish;
			break;

		case eSWUP_STATE_Finish:
			eNextState = eSWUP_STATE_Finish;
			break;

		default:
			HxLOG_Error(" :: Invalid eState [%x]\n", eState);
			break;
	}

	return eNextState;
}

void * XMGR_SWUP_Create_Astra(void *pvData, DxBatchType_e eBatchProcess)
{
	xmgrSwup_AstraContents_t		*pstApsContents;
	DbSvc_TsInfo_t				*pstSvcTsInfo;

	HxLOG_Trace();

	pstSvcTsInfo = (DbSvc_TsInfo_t *)pvData;

	pstApsContents = (xmgrSwup_AstraContents_t*)HLIB_STD_MemCalloc(sizeof(xmgrSwup_AstraContents_t));

	pstApsContents->stBaseContents.eBatchProcess = eBatchProcess;

	pstApsContents->bUserTP = FALSE;
	if (pstSvcTsInfo)
	{
		pstApsContents->bUserTP = TRUE;
		HxSTD_MemCopy(&pstApsContents->stTsInfo, pstSvcTsInfo, sizeof(DbSvc_TsInfo_t));
	}

	return (void*)pstApsContents;
}

HERROR XMGR_SWUP_Destroy_Astra(void *pstContents)
{
	if(pstContents)
	{
		HLIB_STD_MemFree(pstContents);
	}
	return ERR_OK;
}

HERROR	XMGR_SWUP_SetManualInfo_Astra(DbSvc_TsInfo_t *pstDstHomeTsInfo, DbSvc_TsInfo_t *pstUserHomeTsInfo)
{
	if ( !pstDstHomeTsInfo )
		return ERR_FAIL;

	HxSTD_MemSet(pstDstHomeTsInfo, 0, sizeof(DbSvc_TsInfo_t));

	pstDstHomeTsInfo->eDeliType							=	pstUserHomeTsInfo->eDeliType;
	pstDstHomeTsInfo->stTuningParam.sat.ulFrequency		=	pstUserHomeTsInfo->stTuningParam.sat.ulFrequency;
	pstDstHomeTsInfo->stTuningParam.sat.ulSymbolRate	=	pstUserHomeTsInfo->stTuningParam.sat.ulSymbolRate;
	pstDstHomeTsInfo->stTuningParam.sat.eTransSpec		=	pstUserHomeTsInfo->stTuningParam.sat.eTransSpec;
	pstDstHomeTsInfo->stTuningParam.sat.eSatType		=	eSatType_ALL;
	pstDstHomeTsInfo->stTuningParam.sat.eFecCodeRate	=	pstUserHomeTsInfo->stTuningParam.sat.eFecCodeRate;
	pstDstHomeTsInfo->stTuningParam.sat.ePskMod			=	pstUserHomeTsInfo->stTuningParam.sat.ePskMod;
	pstDstHomeTsInfo->stTuningParam.sat.ePilot			=	eDxSAT_PILOT_OFF;
	pstDstHomeTsInfo->stTuningParam.sat.ePolarization	=	pstUserHomeTsInfo->stTuningParam.sat.ePolarization;
	pstDstHomeTsInfo->stTuningParam.sat.eRollOff		=	eDxSAT_ROLL_020;
	pstDstHomeTsInfo->stTuningParam.sat.ucAntId			=	2;

	return ERR_OK;
}

HERROR	XMGR_SWUP_GetHomeTpInfo_Astra(DbSvc_TsInfo_t *pstHomeTsInfo)
{
	HERROR hErr		=	ERR_FAIL;

	if (!pstHomeTsInfo)
		return ERR_FAIL;

	HxSTD_MemSet(pstHomeTsInfo, 0, sizeof(DbSvc_TsInfo_t));

	//hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_HOME_TP_INFO_1, (HUINT32 *)&pstHomeTsInfo->stTuningParam.sat, 0);

	return hErr;
}

