/**
	@file     xmgr_sw_update_dvbssu.c
	@brief    file_name & simple comment.

	Description: Applicatuion for searching Services\n
	Module: Porting Layer  \n  S/W Update
	Remarks : \n
	 Date			Modification				Name\n
	-----------	-------------------------------------	------------------\n

	Copyright (c) 2012 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/


/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

#include <bus.h>
#include <svc_pipe.h>
#include <db_common.h>
#include <db_param.h>
#include <db_svc.h>
#include <linkedlist.h>
#include <svc_resources.h>

#include <svc_swup.h>
#include <svc_sys.h>

#include <mgr_action.h>
#include <mgr_rscmap.h>
#include <mgr_swup.h>
#include <xmgr_swup.h>

#include <mgr_common.h>

#include "_xmgr_swup_dvbssu.h"

#include <otl.h>

#include <uapi.h>

#include <_mgr_swup.h>


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
} xmgrSwup_DvbssuContents_t;

//#define	IPOTA_URL_PATH		"http://test.humaxtvportal.com/SWUpdate/CheckNewSW?SystemID=809C.7D00&SWVersion=1.00.00"
#define	IPOTA_URL_PATH		"http://test.humaxtvportal.com/SWUpdate/CheckNewSW?SystemID=809C.7D01&SWVersion=1.00.00"



STATIC HERROR xmgr_swup_MakeEmergencySignalInfo_Dvbssu(SvcSwUpdate_EmergencyUpdateInfo_t *pstEmergeInfo, SvcSwUpdate_DetectResult_t *pstDetectResult)
{
	if (NULL == pstDetectResult)
	{
		HxLOG_Error("pstDetectResult is NULL \n");
		return ERR_FAIL;
	}

	pstEmergeInfo->eSource	= pstDetectResult->eSource;

#if defined(CONFIG_DEBUG)
	HxLOG_Print("eSource (%d) \n", XMGR_SWUP_GetEnumStrOfSource(pstEmergeInfo->eSource));
#endif

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

HCHAR	*xmgr_swup_GetSwUpdateVersionString_Dvbssu (HBOOL bDetected, HBOOL bStandby, HCHAR *pszCurVer, HCHAR *pszFoundVer)
{
	STATIC HUINT8		s_szSwUpdateVersion[128];
	HxSTD_memset(s_szSwUpdateVersion, 0, 128);

	if(bStandby)
	{
		HxSTD_StrNCpy((char *)s_szSwUpdateVersion, "STANDBY",16);
	}
	else
	{
		HxSTD_StrNCpy((char *)s_szSwUpdateVersion, "NORMAL",16);
	}

	if(bDetected)
	{
		HxSTD_StrNCat((char *)s_szSwUpdateVersion, pszCurVer, 32);
		HxSTD_StrNCat((char *)s_szSwUpdateVersion, pszFoundVer, 32);
	}
	else
	{
		HxSTD_StrNCpy((char *)s_szSwUpdateVersion, "", 16);
	}

	return (char *)s_szSwUpdateVersion;
}


// eSWUP_STATE_Init
STATIC HERROR xmgr_swup_EnterActionInit_Dvbssu(Handle_t hSWUpdate, MgrSwup_State_e ePrevState, void *pvUserData, HINT32 nMessage, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	HERROR  eRet = ERR_OK;
	static HINT8 szDetectUrl[128];
	HCHAR *szPortaltUrl;
	HINT8 szSWVersion[32];
	HUINT32 ulSWVersion = 0, ulSystemId = 0;
	HUINT32 ulVerNum1, ulVerNum2, ulVerNum3;
	HCHAR	*pszVersion = NULL;

	xmgrSwup_DvbssuContents_t	*pstContents = (xmgrSwup_DvbssuContents_t*)pvUserData;

	szPortaltUrl = UAPI_FM_GetDefaultURL((HCHAR *)IPOTA_SERVER_URL, (HCHAR *)IPOTA_URL_FILE);
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

	// ota check 마다, ota info registry 를 init 해준다. 이렇게 매번하는데 clear 한듯...
	pszVersion = xmgr_swup_GetSwUpdateVersionString_Dvbssu(FALSE, FALSE, NULL, NULL);
	DB_PARAM_SetItem (eDB_PARAM_ITEM_SWUPDATE_VERSION, (HUINT32)pszVersion, 0);


	if (pstContents->bUserTP == TRUE)
	{
		MGR_SWUPINFO_ResetHomeTp();
		MGR_SWUPINFO_AddOtaHomeTp(&pstContents->stTsInfo);
	}
	else
	{
		eRet = MGR_SWUP_SetDefaultTp();
		if(eRet != ERR_OK)
		{
			HxLOG_Error("MGR_SWUP_SetDefaultTp Error \n");
			//return ERR_FAIL;
		}
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


	HxLOG_Print("szDetectUrl : [%s]\n", szDetectUrl);

	//	Try IP Connection
	pstContents->pszIpDetectServerUrl = szDetectUrl;
	pstContents->eSource = nParam1;

	// Set Next State : IP Detect
#if defined(CONFIG_DEBUG)
	HxLOG_Debug("PostMessage [%x]\n", eMEVT_SWUP_DETECT_START);
#endif

	{
		HERROR hErr;
		SvcSwUpdate_EmergencyUpdateInfo_t	*pstEmergeInfo;
		pstEmergeInfo   = (SvcSwUpdate_EmergencyUpdateInfo_t*)HLIB_STD_MemAlloc(sizeof(SvcSwUpdate_EmergencyUpdateInfo_t));

		HxLOG_Print("Initialization Emergency Info...\n");
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
STATIC HERROR xmgr_swup_EnterActionConnect_Dvbssu(Handle_t hSWUpdate, MgrSwup_State_e ePrevState, void *pvUserData, HINT32 nMessage, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	xmgrSwup_DvbssuContents_t		*pstContents = (xmgrSwup_DvbssuContents_t*)pvUserData;

	HxLOG_Trace();
	if (NULL == pstContents)
	{
		HxLOG_Error("pstContents is NULL \n");
		return ERR_FAIL;
	}

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
			HxLOG_Error("Invalid eSource Type (%d)...\n", pstContents->eSource);
			return ERR_FAIL;
	}

	return ERR_OK;
}

// eSWUP_STATE_Detect
STATIC HERROR xmgr_swup_EnterActionDetect_Dvbssu(Handle_t hSWUpdate, MgrSwup_State_e ePrevState, void *pvUserData, HINT32 nMessage, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	xmgrSwup_DvbssuContents_t		*pstContents = (xmgrSwup_DvbssuContents_t *)pvUserData;

	HxLOG_Trace();

	if (NULL == pstContents)
		return ERR_FAIL;

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
			MGR_SWUP_CreateUsb(hSWUpdate);
			break;
		default:
			break;
	}

	return ERR_OK;
}

// eSWUP_STATE_Download
STATIC HERROR xmgr_swup_EnterActionDownload_Dvbssu(Handle_t hSWUpdate, MgrSwup_State_e ePrevState, void *pvUserData, HINT32 nMessage, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	xmgrSwup_DvbssuContents_t		*pstContents = (xmgrSwup_DvbssuContents_t*)pvUserData;
	SvcSwUpdate_DetectResult_t		*pstDetectResult;
	HERROR 						 hErr;
	DbSvc_TsInfo_t	 stTsInfo;
	SvcSi_SwupSignal_t 	 stOtaInfo;

	HxLOG_Trace();
	if (NULL == pstContents)
		return ERR_FAIL;

	pstDetectResult = (SvcSwUpdate_DetectResult_t*)HLIB_STD_MemCalloc(sizeof(SvcSwUpdate_DetectResult_t));

	MGR_SWUP_GetDetectInfo(hSWUpdate, pstDetectResult);
	pstContents->eSource = pstDetectResult->eSource;

	// RF 의 경우 signaling information 을 가져온다.
	// SvcSwUpdate_DetectResult_t 안에도 존재하지만.. 현재 처리구조상..
	if (pstContents->eSource == eDxSWUPDATE_SOURCE_Rf)
	{
		// RF 의 경우.. serched info 가 존재하는지 check
		hErr = MGR_SWUPINFO_GetSearchedInfo (&stTsInfo, sizeof(SvcSi_SwupSignal_t), (void *)&stOtaInfo);
		if (hErr != ERR_OK)
		{
			HxLOG_Critical("\n\n");
			HLIB_STD_MemFree(pstDetectResult);
			return ERR_FAIL;
		}
	}

	switch(pstDetectResult->eSource)
	{
		case eDxSWUPDATE_SOURCE_Usb:
			{
				static	HCHAR	*pszSoftwarePath = NULL;
				if (pszSoftwarePath == NULL)
					pszSoftwarePath = (HCHAR*)HLIB_STD_MemAlloc(MAX_PATH_LENGTH);

				HxSTD_MemSet(pszSoftwarePath, 0, MAX_PATH_LENGTH);
				HLIB_STD_StrNCpySafe(pszSoftwarePath, pstDetectResult->data.stUsbInfo.szSoftwarePath, MAX_PATH_LENGTH);

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
STATIC HERROR xmgr_swup_EnterActionInstall_Dvbssu(Handle_t hSWUpdate, MgrSwup_State_e ePrevState, void *pvUserData, HINT32 nMessage, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	xmgrSwup_DvbssuContents_t			*pstContents = (xmgrSwup_DvbssuContents_t*)pvUserData;
	SvcSwUpdate_EmergencyUpdateInfo_t	*pstSourceInfo;
	HUINT8								*pszSourceFile;

	HxLOG_Trace();
	if (NULL == pstContents)
		return ERR_FAIL;

	if(pstContents->eSource == eDxSWUPDATE_SOURCE_Usb)
		pszSourceFile = (HUINT8 *)nParam2;
	else
		pszSourceFile = (HUINT8 *)DDB_SAVE_FILE_PATH;

	{
		SvcSwUpdate_EmergencyUpdateInfo_t	*pstEmergeInfo;
		pstEmergeInfo   = (SvcSwUpdate_EmergencyUpdateInfo_t*)HLIB_STD_MemAlloc(sizeof(SvcSwUpdate_EmergencyUpdateInfo_t));

		HxLOG_Print("Initialization Emergency Info...\n");
		// detect 성공시 signaling 정보를 nvram 에 set 해준다.
		HxSTD_MemSet(pstEmergeInfo, 0, sizeof(SvcSwUpdate_EmergencyUpdateInfo_t));

		if (DB_PARAM_GetItem(eDB_PARAM_ITEM_OTA_EMERGENCYINFO, (HUINT32 *)pstEmergeInfo, sizeof(SvcSwUpdate_EmergencyUpdateInfo_t)) == ERR_OK)
		{
			HxLOG_Print("\nEmergency Info : RF\n");
			HxLOG_Print("eSource : 0x%X, DataVersion : %ld, eDeliType : 0x%X, downloadType: 0x%X, usPid : 0x%X \n",
				pstEmergeInfo->eSource, pstEmergeInfo->ulDataVersion,
				pstEmergeInfo->unInfo.stRfInfo.eDeliType,
				pstEmergeInfo->unInfo.stRfInfo.eDownloadType,
				pstEmergeInfo->unInfo.stRfInfo.usPid);

		}
		HxSTD_MemCopy(&pstContents->stSourceInfo, pstEmergeInfo, sizeof(SvcSwUpdate_EmergencyUpdateInfo_t));
		pstContents->eSource = pstEmergeInfo->eSource;
		HLIB_STD_MemFree(pstEmergeInfo);
	}

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
				HxLOG_Debug("eDxSWUPDATE_SOURCE_Ip Install....\n");

				// TODO: Get IP info from LOG API
				pstSourceInfo->unInfo.stIpInfo.bHdcp = TRUE;

#if 0
				MWC_NET_GetAddressOfIF( eNet_Inet_1st, eNet_Inet_Addr_IPv4, (HLONG *)&(pstSourceInfo->unInfo.stIpInfo.aucIpSetting[0]) );
				MWC_NET_GetAddressOfIF( eNet_Inet_1st, eNet_Inet_Addr_NetMask, (HLONG *)&(pstSourceInfo->unInfo.stIpInfo.aucIpSetting[1]) );
				MWC_NET_GetAddressOfIF( eNet_Inet_1st, eNet_Inet_Addr_Gateway, (HLONG *)&(pstSourceInfo->unInfo.stIpInfo.aucIpSetting[2]) );
				MWC_NET_GetAddressOfIF( eNet_Inet_1st, eNet_Inet_Addr_DNS, (HLONG *)&(pstSourceInfo->unInfo.stIpInfo.aucIpSetting[3]) );
#endif

				// TODO: Set Portal IP
				// stSourceInfo.unInfo.stIpInfo.ulPortalIp = ;
				// TODO: Set Download Path
				// snprintf(stSourceInfo.unInfo.stIpInfo.aucDownPath, 44, "");

			}
			break;
		default:
			break;
	}

	MGR_SWUP_InstallStart(hSWUpdate, pszSourceFile, pstSourceInfo);

	return ERR_OK;
}

// eSWUP_STATE_Error
STATIC HERROR xmgr_swup_EnterActionError_Dvbssu(Handle_t hSWUpdate, MgrSwup_State_e ePrevState, void *pvUserData, HINT32 nMessage, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	xmgrSwup_DvbssuContents_t		*pstContents = (xmgrSwup_DvbssuContents_t*)pvUserData;
	// nParam3 에는 mgr단의 fail reason, nParam1에는 svc단의 fail reason... 이 부분 정리.

	HxLOG_Trace();
	if (NULL == pstContents)
		return ERR_FAIL;

	BUS_PostMessageToOM(NULL, eMEVT_DN_SWUP_ERROR, HANDLE_NULL, (HINT32)hSWUpdate, 0, nParam3);

	return ERR_OK;
}

// eSWUP_STATE_Finish
STATIC HERROR xmgr_swup_EnterActionFinish_Dvbssu(Handle_t hSWUpdate, MgrSwup_State_e ePrevState, void *pvUserData, HINT32 nMessage, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	xmgrSwup_DvbssuContents_t		*pstContents = (xmgrSwup_DvbssuContents_t*)pvUserData;

	HxLOG_Trace();
	if (NULL == pstContents)
		return ERR_FAIL;


	BUS_PostMessageToOM(NULL, eMEVT_DN_SWUP_FINISH, HANDLE_NULL, (HINT32)hSWUpdate, 0, 0);


	return ERR_OK;
}

// eSWUP_STATE_Init
STATIC HERROR xmgr_swup_LeaveActionInit_Dvbssu(Handle_t hSWUpdate, MgrSwup_State_e eNextState, void *pvUserData, HINT32 nMessage, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	xmgrSwup_DvbssuContents_t		*pstContents = (xmgrSwup_DvbssuContents_t*)pvUserData;

	HxLOG_Trace();
	if (NULL == pstContents)
		return ERR_FAIL;

//	MGR_SWUP_PostMessage(ulTag, eMEVT_SWUP_DETECT_START, ulTag, (HINT32)eDxSWUPDATE_SOURCE_Ip, (HINT32)&pstContents->stDetectParamIp, 0);

	return ERR_OK;
}

// eSWUP_STATE_Connect
STATIC HERROR xmgr_swup_LeaveActionConnect_Dvbssu(Handle_t hSWUpdate, MgrSwup_State_e eNextState, void *pvUserData, HINT32 nMessage, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	xmgrSwup_DvbssuContents_t		*pstContents = (xmgrSwup_DvbssuContents_t*)pvUserData;

	HxLOG_Trace();
	if (NULL == pstContents)
		return ERR_FAIL;

	// TODO: Destory Connect Action

//	MGR_SWUP_PostMessage(ulTag, eMEVT_SWUP_DETECT_START, ulTag, (HINT32)eDxSWUPDATE_SOURCE_Ip, (HINT32)&pstContents->stDetectParamIp, 0);

	return ERR_OK;
}

// eSWUP_STATE_Detect
STATIC HERROR xmgr_swup_LeaveActionDetect_Dvbssu(Handle_t hSWUpdate, MgrSwup_State_e eNextState, void *pvUserData, HINT32 nMessage, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	xmgrSwup_DvbssuContents_t		*pstContents = (xmgrSwup_DvbssuContents_t*)pvUserData;

	HxLOG_Trace();
	if (NULL == pstContents)
		return ERR_FAIL;

	if(nMessage == eMEVT_SWUP_NOTIFY_DETECT_SUCCESS)
	{
		SvcSwUpdate_EmergencyUpdateInfo_t	*pstEmergeInfo;
		SvcSwUpdate_DetectResult_t			*pstDetectResult;
		HUINT32 							ulSystemVersion;

		pstEmergeInfo	= (SvcSwUpdate_EmergencyUpdateInfo_t*)HLIB_STD_MemAlloc(sizeof(SvcSwUpdate_EmergencyUpdateInfo_t));
		pstDetectResult = (SvcSwUpdate_DetectResult_t*)HLIB_STD_MemAlloc(sizeof(SvcSwUpdate_DetectResult_t));

		SVC_SYS_GetSystemVersion(eVERSION_TYPE_APPL, &ulSystemVersion);
		MGR_SWUP_GetDetectInfo(hSWUpdate, pstDetectResult);

		// detect 성공시 signaling 정보를 nvram 에 set 해준다.
		HxSTD_MemSet(pstEmergeInfo, 0, sizeof(SvcSwUpdate_EmergencyUpdateInfo_t));
		if (xmgr_swup_MakeEmergencySignalInfo_Dvbssu(pstEmergeInfo, pstDetectResult) == ERR_OK)
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

		HxLOG_Print("Destroy OTA DetectAction Proc!! sourceType (0x%X)\n",pstContents->eSource);

		switch(pstContents->eSource)
		{
			case eDxSWUPDATE_SOURCE_Usb:		MGR_SWUP_DestroyUsb();					break;
			case eDxSWUPDATE_SOURCE_Ip: 		MGR_SWUP_IpDestroy(hSWUpdate);			break;
			case eDxSWUPDATE_SOURCE_Rf: 		MGR_SWUP_DetectDestroy();				break;
			default:		break;
		}

		pstContents->eSource = eDxSWUPDATE_SOURCE_None;

		//	free는 EVT_DN_SWUP_DETECT_SUCCESS를 받는 곳에서 수행.
		//	BUS상태에 따라서 메모리 릭이 있을수도 있지만, 담당자가 수정해 주겠지..

		HLIB_STD_MemFree(pstEmergeInfo);

		return ERR_OK;

	}
	else if(nMessage ==  eMEVT_SWUP_NOTIFY_DETECT_FAIL)
	{

		switch(pstContents->eSource)
		{
		case eDxSWUPDATE_SOURCE_Usb:
			HxLOG_Print("eSWUP_STATE_Detect : [USB] DETECT_FAIL!!!\n");
			MGR_SWUP_DestroyUsb();
			pstContents->eSource = XMGR_SWUP_GetNextSource(pstContents->eSource);

			break;

		case eDxSWUPDATE_SOURCE_Ip:
			HxLOG_Print("eSWUP_STATE_Detect : [IP] DETECT_FAIL!!!\n");
			MGR_SWUP_IpDestroy(hSWUpdate);
			pstContents->eSource = XMGR_SWUP_GetNextSource(pstContents->eSource);
			break;

		case eDxSWUPDATE_SOURCE_Rf:
			HxLOG_Print("eSWUP_STATE_Detect : [RF] DETECT_FAIL!!!\n");
			pstContents->eSource = XMGR_SWUP_GetNextSource(pstContents->eSource);
			MGR_SWUP_DetectDestroy();
			break;

		default:
			HxLOG_Print("eSWUP_STATE_Detect : [??] DETECT_FAIL!!!\n");
			BUS_PostMessageToOM(NULL, eMEVT_DN_SWUP_DETECT_FAIL, HANDLE_NULL, (HINT32)hSWUpdate, 0, 0);
			break;
		}
	}

	if(pstContents->eSource == eDxSWUPDATE_SOURCE_None)
	{
		BUS_PostMessageToOM(NULL, eMEVT_DN_SWUP_DETECT_FAIL, HANDLE_NULL, (HINT32)hSWUpdate, 0, nParam3);
		BUS_PostMessage((BUS_Callback_t)NULL, eMEVT_ACTION_NOTIFY_RESULT, (Handle_t)NULL, eMGR_EVENT_SWUP_DETECT_FINISH, 0, 0);
	}
	else
	{
		HxLOG_Print("Retry %s....\n", (eDxSWUPDATE_SOURCE_Ip == pstContents->eSource) ? "IP" : ((eDxSWUPDATE_SOURCE_Rf == pstContents->eSource) ? "RF" : "??"));
	}


	return ERR_OK;
}

// eSWUP_STATE_Download
STATIC HERROR xmgr_swup_LeaveActionDownload_Dvbssu(Handle_t hSWUpdate, MgrSwup_State_e eNextState, void *pvUserData, HINT32 nMessage, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	xmgrSwup_DvbssuContents_t		*pstContents = (xmgrSwup_DvbssuContents_t*)pvUserData;
	HERROR						hErr;

	HxLOG_Trace();
	if (NULL == pstContents)
		return ERR_FAIL;

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
		if (hErr != ERR_OK)
		{
			HxLOG_Error("MGR_SWUP_DownloadDestroy failed. \n");
		}

		//	Download Destroy이에 Release Action
		MGR_ACTION_Release(eActionId_VIEW_0, eActionType_SEARCH);
		BUS_PostMessage(NULL, eMEVT_ACTION_NOTIFY_RESULT, (Handle_t)NULL,  0, 0, 0);
		break;

		break;
	case eDxSWUPDATE_SOURCE_Ip:
		MGR_SWUP_DownloadDestroy(eDxSWUPDATE_SOURCE_Ip);
		break;
	default:
		break;
	}

//	MGR_SWUP_PostMessage(ulTag, eMEVT_SWUP_DETECT_START, ulTag, (HINT32)eDxSWUPDATE_SOURCE_Ip, (HINT32)&pstContents->stDetectParamIp, 0);
	if((nMessage == eMEVT_SWUP_NOTIFY_DOWNLOAD_SUCCESS) || (nMessage == eMEVT_SWUP_NOTIFY_DOWNLOAD_FAIL))
	{
		BUS_PostMessage((BUS_Callback_t)NULL, eMEVT_ACTION_NOTIFY_RESULT, (Handle_t)NULL, eMGR_EVENT_SWUP_DOWNLOAD_FINISH, 0, 0);
	}
	return ERR_OK;
}

// eSWUP_STATE_Install
STATIC HERROR xmgr_swup_LeaveActionInstall_Dvbssu(Handle_t hSWUpdate, MgrSwup_State_e eNextState, void *pvUserData, HINT32 nMessage, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	HCHAR							*pszVersion = NULL;
	xmgrSwup_DvbssuContents_t		*pstContents = (xmgrSwup_DvbssuContents_t*)pvUserData;

	HxLOG_Trace();
	if (NULL == pstContents)
		return ERR_FAIL;

	if(nMessage == eMEVT_SWUP_INSTALL_SUCCESS)
	{
		HCHAR				szCurrentVersion[32], szNewSWVersion[32];
		HUINT32 			ulSystemVersion;
		SvcSi_SwupSignal_t	stOtaInfo;
		HERROR				hErr;
		HCHAR				*pszVersion = NULL;

		HxSTD_memset(&stOtaInfo, 0x00, sizeof(SvcSi_SwupSignal_t));

		SVC_SYS_GetSystemVersion(eVERSION_TYPE_APPL, &ulSystemVersion);
		hErr = DB_PARAM_GetItem (eDB_PARAM_ITEM_OTA_OTAINFO, (HUINT32 *)&stOtaInfo, sizeof(SvcSi_SwupSignal_t));
		if(hErr == ERR_OK)
		{
			SVC_SYS_MakeSystemVerString(eVERSION_TYPE_APPL, ulSystemVersion, (HUINT8 *)szCurrentVersion);
			SVC_SYS_MakeSystemVerString(eVERSION_TYPE_APPL, stOtaInfo.ulDataVersion, (HUINT8 *)szNewSWVersion);

			pszVersion = xmgr_swup_GetSwUpdateVersionString_Dvbssu(TRUE, FALSE, szCurrentVersion, szNewSWVersion);
			DB_PARAM_SetItem(eDB_PARAM_ITEM_SWUPDATE_VERSION, (HUINT32)pszVersion, TRUE);
			HxLOG_Debug("\n eMEVT_SWUP_INSTALL_SUCCESS! (%s)\n", pszVersion);

#ifdef CONFIG_DEBUG
			{
				HCHAR	szTempVer[128];
				HxSTD_memset(szTempVer, 0, 128);
				DB_PARAM_GetItem(eDB_PARAM_ITEM_SWUPDATE_VERSION, (HUINT32*)szTempVer, 128);
				HxLOG_Debug("szTempVer (%s)\n", szTempVer);
			}
#endif
		}
		else
		{
			pszVersion = xmgr_swup_GetSwUpdateVersionString_Dvbssu(FALSE, FALSE, NULL, NULL);
			DB_PARAM_SetItem(eDB_PARAM_ITEM_SWUPDATE_VERSION, (HUINT32)pszVersion, 0);
			HxLOG_Debug("\n eMEVT_SWUP_NOTIFY_INSTALL_FAIL! (%s)\n", pszVersion);
		}

		BUS_PostMessageToOM(NULL, eMEVT_DN_SWUP_INSTALL_SUCCESS, HANDLE_NULL, (HINT32)hSWUpdate, 0, 0);
	}
	else if(nMessage == eMEVT_SWUP_NOTIFY_INSTALL_FAIL)
	{
		pszVersion = xmgr_swup_GetSwUpdateVersionString_Dvbssu(FALSE, FALSE, NULL, NULL);
		DB_PARAM_SetItem (eDB_PARAM_ITEM_SWUPDATE_VERSION, (HUINT32)pszVersion, 0);
		HxLOG_Debug("\n eMEVT_SWUP_NOTIFY_INSTALL_FAIL! (%s)\n", pszVersion);

		BUS_PostMessageToOM(NULL, eMEVT_DN_SWUP_INSTALL_FAIL, HANDLE_NULL, (HINT32)hSWUpdate, 0, 0);
	}

	if(nMessage == eMEVT_SWUP_INSTALL_SUCCESS || nMessage == eMEVT_SWUP_NOTIFY_INSTALL_FAIL)
	{
		BUS_PostMessage((BUS_Callback_t)NULL, eMEVT_ACTION_NOTIFY_RESULT, (Handle_t)NULL, eMGR_EVENT_SWUP_INSTALL_FINISH, 0, 0);
	}

	MGR_SWUP_InstallStop();

	return ERR_OK;
}

// eSWUP_STATE_Error
STATIC HERROR xmgr_swup_LeaveActionError_Dvbssu(Handle_t hSWUpdate, MgrSwup_State_e eNextState, void *pvUserData, HINT32 nMessage, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	xmgrSwup_DvbssuContents_t		*pstContents = (xmgrSwup_DvbssuContents_t*)pvUserData;

	HxLOG_Trace();
	if (NULL == pstContents)
		return ERR_FAIL;

	return ERR_OK;
}

// eSWUP_STATE_Finish
STATIC HERROR xmgr_swup_LeaveActionFinish_Dvbssu(Handle_t hSWUpdate, MgrSwup_State_e eNextState, void *pvUserData, HINT32 nMessage, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	xmgrSwup_DvbssuContents_t		*pstContents = (xmgrSwup_DvbssuContents_t*)pvUserData;

	HxLOG_Trace();
	if (NULL == pstContents)
		return ERR_FAIL;


	return ERR_OK;
}

BUS_Result_t	XMGR_SWUP_EnterAction_Dvbssu(Handle_t hSWUpdate, MgrSwup_State_e eCurrState, MgrSwup_State_e ePrevState, void *pvUserData, HINT32 nMessage, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	xmgrSwup_DvbssuContents_t	*pstContents = (xmgrSwup_DvbssuContents_t*)pvUserData;
	BUS_Result_t			 hRes = MESSAGE_PASS;
	HERROR					 hErr = ERR_FAIL;

	HxLOG_Trace();
	if (NULL == pstContents)
		return MESSAGE_BREAK;

	switch (eCurrState)
	{
	case eSWUP_STATE_Init:
		hErr = xmgr_swup_EnterActionInit_Dvbssu(hSWUpdate, ePrevState, pvUserData, nMessage, hAction, nParam1, nParam2, nParam3);
		break;
	case eSWUP_STATE_Connect:
		hErr = xmgr_swup_EnterActionConnect_Dvbssu(hSWUpdate, ePrevState, pvUserData, nMessage, hAction, nParam1, nParam2, nParam3);
		break;
	case eSWUP_STATE_Detect:
		hErr = xmgr_swup_EnterActionDetect_Dvbssu(hSWUpdate, ePrevState, pvUserData, nMessage, hAction, nParam1, nParam2, nParam3);
		break;
	case eSWUP_STATE_Download:
		hErr = xmgr_swup_EnterActionDownload_Dvbssu(hSWUpdate, ePrevState, pvUserData, nMessage, hAction, nParam1, nParam2, nParam3);
		break;
	case eSWUP_STATE_Install:
		hErr = xmgr_swup_EnterActionInstall_Dvbssu(hSWUpdate, ePrevState, pvUserData, nMessage, hAction, nParam1, nParam2, nParam3);
		break;
	case eSWUP_STATE_Error:
		hErr = xmgr_swup_EnterActionError_Dvbssu(hSWUpdate, ePrevState, pvUserData, nMessage, hAction, nParam1, nParam2, nParam3);
		break;
	case eSWUP_STATE_Finish:
		hErr = xmgr_swup_EnterActionFinish_Dvbssu(hSWUpdate, ePrevState, pvUserData, nMessage, hAction, nParam1, nParam2, nParam3);
		break;
	default:
		break;
	}

	if (hErr != ERR_OK)
	{
		HxLOG_Error("EnterAction failed. currState(%d) \n",eCurrState);
		BUS_PostMessageToOM(NULL, eMEVT_DN_SWUP_ERROR, HANDLE_NULL, (HINT32)hSWUpdate, 0, 0);
	}
	return hRes;
}

BUS_Result_t	XMGR_SWUP_LeaveAction_Dvbssu(Handle_t hSWUpdate, MgrSwup_State_e eCurrState, MgrSwup_State_e eNextState, void *pvUserData, HINT32 nMessage, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	xmgrSwup_DvbssuContents_t	*pstContents = (xmgrSwup_DvbssuContents_t*)pvUserData;
	BUS_Result_t			 hRes = MESSAGE_PASS;
	HERROR					 hErr = ERR_FAIL;

	HxLOG_Trace();
	if (NULL == pstContents)
		return MESSAGE_BREAK;

	switch (eCurrState)
	{
	case eSWUP_STATE_Init:
		hErr = xmgr_swup_LeaveActionInit_Dvbssu(hSWUpdate, eNextState, pvUserData, nMessage, hAction, nParam1, nParam2, nParam3);
		break;
	case eSWUP_STATE_Connect:
		hErr = xmgr_swup_LeaveActionConnect_Dvbssu(hSWUpdate, eNextState, pvUserData, nMessage, hAction, nParam1, nParam2, nParam3);
		break;
	case eSWUP_STATE_Detect:
		hErr = xmgr_swup_LeaveActionDetect_Dvbssu(hSWUpdate, eNextState, pvUserData, nMessage, hAction, nParam1, nParam2, nParam3);
		break;
	case eSWUP_STATE_Download:
		hErr = xmgr_swup_LeaveActionDownload_Dvbssu(hSWUpdate, eNextState, pvUserData, nMessage, hAction, nParam1, nParam2, nParam3);
		break;
	case eSWUP_STATE_Install:
		hErr = xmgr_swup_LeaveActionInstall_Dvbssu(hSWUpdate, eNextState, pvUserData, nMessage, hAction, nParam1, nParam2, nParam3);
		break;
	case eSWUP_STATE_Error:
		hErr = xmgr_swup_LeaveActionError_Dvbssu(hSWUpdate, eNextState, pvUserData, nMessage, hAction, nParam1, nParam2, nParam3);
		break;
	case eSWUP_STATE_Finish:
		hErr = xmgr_swup_LeaveActionFinish_Dvbssu(hSWUpdate, eNextState, pvUserData, nMessage, hAction, nParam1, nParam2, nParam3);
		break;
	default:
		break;
	}

	if (hErr != ERR_OK)
	{
		HxLOG_Error("leaveAction failed. eCurrState(%d) \n", eCurrState);
	}

	return hRes;
}

MgrSwup_State_e XMGR_SWUP_GetNextState_Dvbssu(Handle_t hSWUpdate, MgrSwup_State_e eState, void *pvUserData, HINT32 nMessage, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	xmgrSwup_DvbssuContents_t	*pstContents = (xmgrSwup_DvbssuContents_t*)pvUserData;
	MgrSwup_State_e			 eNextState = eSWUP_STATE_Unknown;

	HxLOG_Trace();
	HxLOG_Debug("eState : [%d]\n", eState);

	if(nMessage == eMEVT_SWUP_CANCEL || nMessage == eMEVT_SWUP_DOWNLOAD_STOP)
	{
		eNextState = eSWUP_STATE_Finish;
		return eNextState;
	}

	switch (eState)
	{
		case eSWUP_STATE_Init:
			HxLOG_Trace();
			HxLOG_Debug("nMessage : [%x] [%x]\n", nMessage, eMEVT_SWUP_DETECT_START);
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
					break;
			}
			break;

		case eSWUP_STATE_Connect:
			switch (nMessage)
			{
				case eMEVT_SWUP_DETECT_START:
					eNextState = eSWUP_STATE_Detect;
					break;
				default:
					HxLOG_Error("Invalid nMessage [%x]\n", nMessage);
					break;
			}
			break;

		case eSWUP_STATE_Detect:
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
/*
					{
						SvcSwUpdate_DetectResult_t	*pstDetectResult;
						HUINT32					ulSystemVersion;

						pstDetectResult = (SvcSwUpdate_DetectResult_t*)HLIB_STD_MemAlloc(sizeof(SvcSwUpdate_DetectResult_t));

						PAL_SYS_GetSystemVersion(eVERSION_TYPE_APPL, &ulSystemVersion);
						MGR_SWUP_GetDetectInfo(hSWUpdate, pstDetectResult);

						BUS_PostMessageToOM(NULL, eMEVT_DN_SWUP_DETECT_SUCCESS, HANDLE_NULL, (HINT32)hSWUpdate, ulSystemVersion, pstDetectResult);
						//	free는 EVT_DN_SWUP_DETECT_SUCCESS를 받는 곳에서 수행.
						//	BUS상태에 따라서 메모리 릭이 있을수도 있지만, 담당자가 수정해 주겠지..
					}
*/
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
							eNextState = eSWUP_STATE_Connect;
							break;
						default:
							eNextState = eSWUP_STATE_Error;
							HxLOG_Error("eSWUP_STATE_Detect :: Invalid nMessage [%x]\n", nMessage);
							break;
						}
					}
					break;
				default:
					break;
			}
			break;

		case eSWUP_STATE_Download:
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
						BUS_PostMessageToOM(NULL, eMEVT_DN_SWUP_DOWNLOAD_PROGRESS, HANDLE_NULL, (HINT32)hSWUpdate, nParam1, 100);
					else
					{
						pstContents->hDownload = nParam3;
						BUS_PostMessageToOM(NULL, eMEVT_DN_SWUP_DOWNLOAD_PROGRESS, HANDLE_NULL, (HINT32)hSWUpdate, nParam2, 100);
					}
					eNextState = eSWUP_STATE_Download;
					break;
				case eMEVT_SWUP_DOWNLOAD_STOP :
					break;
				default:
					break;
			}
			break;

		case eSWUP_STATE_Install:
			switch (nMessage)
			{
				case eMEVT_SWUP_INSTALL_SUCCESS:
					eNextState = eSWUP_STATE_Finish;
					//BUS_PostMessageToOM(NULL, eMEVT_DN_SWUP_INSTALL_SUCCESS, HANDLE_NULL, (HINT32)hSWUpdate, 0, 0);
					break;
				case eMEVT_SWUP_NOTIFY_INSTALL_FAIL:
					eNextState = eSWUP_STATE_Error;
					//BUS_PostMessageToOM(NULL, eMEVT_DN_SWUP_INSTALL_FAIL, HANDLE_NULL, (HINT32)hSWUpdate, 0, 0);
					break;
				case eMEVT_SWUP_NOTIFY_INSTALL_PROGRESS:
					BUS_PostMessageToOM(NULL, eMEVT_DN_SWUP_INSTALL_PROGRESS, HANDLE_NULL, (HINT32)hSWUpdate, nParam2, 100);
					eNextState = eSWUP_STATE_Install;
					break;
				default:
					break;
			}
			break;

		case eSWUP_STATE_Error:
			eNextState = eSWUP_STATE_Finish;
			break;

		case eSWUP_STATE_Finish:
			eNextState = eSWUP_STATE_Finish;
			break;

		default:
			break;
	}

	return eNextState;
}

void *XMGR_SWUP_Create_Dvbssu(void *pvUserData, DxBatchType_e eBatchProcess)
{
	xmgrSwup_DvbssuContents_t		*pstContents;
	DxTransponder_t					*pstTuningInfo;
	DbSvc_TsInfo_t 					 stSvcTsInfo;

	HxLOG_Trace();

	pstTuningInfo = (DxTransponder_t *)pvUserData;
	pstContents = (xmgrSwup_DvbssuContents_t*)HLIB_STD_MemCalloc(sizeof(xmgrSwup_DvbssuContents_t));
	pstContents->stBaseContents.eBatchProcess = eBatchProcess;

	pstContents->bUserTP = FALSE;
	if (pstTuningInfo)
	{
		OTL_CONV_DxTransponder2SVC_TsInfo(&stSvcTsInfo, pstTuningInfo);

		pstContents->bUserTP = TRUE;
		HxSTD_MemCopy(&pstContents->stTsInfo, &stSvcTsInfo, sizeof(DbSvc_TsInfo_t));
	}

	return (void*)pstContents;
}

HERROR XMGR_SWUP_Destroy_Dvbssu(void *pvContents)
{
	if(pvContents)
	{
		HLIB_STD_MemFree(pvContents);
	}
	return ERR_OK;
}

HERROR	XMGR_SWUP_SetManualInfo_Dvbssu(DbSvc_TsInfo_t *pstDstHomeTsInfo, DbSvc_TsInfo_t *pstUserHomeTsInfo)
{
	if ( !pstDstHomeTsInfo )
		return ERR_FAIL;

	HxSTD_MemSet(pstDstHomeTsInfo, 0, sizeof(DbSvc_TsInfo_t));

#if defined(CONFIG_MW_CH_SATELLITE)
	pstDstHomeTsInfo->eDeliType 						=eDxDELIVERY_TYPE_SAT;
	pstDstHomeTsInfo->stTuningParam.sat.ulFrequency 		= pstUserHomeTsInfo->stTuningParam.sat.ulFrequency;
	pstDstHomeTsInfo->stTuningParam.sat.ulSymbolRate		= pstUserHomeTsInfo->stTuningParam.sat.ulSymbolRate;
	pstDstHomeTsInfo->stTuningParam.sat.eTransSpec		= pstUserHomeTsInfo->stTuningParam.sat.eTransSpec;
	pstDstHomeTsInfo->stTuningParam.sat.eSatType			= eSatType_ALL;
	pstDstHomeTsInfo->stTuningParam.sat.eFecCodeRate		= pstUserHomeTsInfo->stTuningParam.sat.eFecCodeRate;
	pstDstHomeTsInfo->stTuningParam.sat.ePskMod 			= pstUserHomeTsInfo->stTuningParam.sat.ePskMod;
	pstDstHomeTsInfo->stTuningParam.sat.ePilot			= eDxSAT_PILOT_OFF;
	pstDstHomeTsInfo->stTuningParam.sat.ePolarization		= pstUserHomeTsInfo->stTuningParam.sat.ePolarization;
	pstDstHomeTsInfo->stTuningParam.sat.eRollOff			= eDxSAT_ROLL_020;
	pstDstHomeTsInfo->stTuningParam.sat.ucAntId 			= 2;
#endif
#if defined(CONFIG_MW_CH_TERRESTRIAL)
	pstDstHomeTsInfo->eDeliType 						= eDxDELIVERY_TYPE_TER;
	pstDstHomeTsInfo->stTuningParam.ter.ulFreq			= pstUserHomeTsInfo->stTuningParam.ter.ulFreq;
	pstDstHomeTsInfo->stTuningParam.ter.ucBandwidth		= pstUserHomeTsInfo->stTuningParam.ter.ucBandwidth;
	pstDstHomeTsInfo->stTuningParam.ter.ucConstellation	= pstUserHomeTsInfo->stTuningParam.ter.ucConstellation;
	pstDstHomeTsInfo->stTuningParam.ter.ucHierachy		= pstUserHomeTsInfo->stTuningParam.ter.ucHierachy;
	pstDstHomeTsInfo->stTuningParam.ter.ucStream			= pstUserHomeTsInfo->stTuningParam.ter.ucStream;
	pstDstHomeTsInfo->stTuningParam.ter.ucCodeRate		= pstUserHomeTsInfo->stTuningParam.ter.ucCodeRate;
	pstDstHomeTsInfo->stTuningParam.ter.ucGuardInterval	= pstUserHomeTsInfo->stTuningParam.ter.ucGuardInterval;
	pstDstHomeTsInfo->stTuningParam.ter.ucTransMode		= pstUserHomeTsInfo->stTuningParam.ter.ucTransMode;
	pstDstHomeTsInfo->stTuningParam.ter.ucOffset			= pstUserHomeTsInfo->stTuningParam.ter.ucOffset;
	pstDstHomeTsInfo->stTuningParam.ter.ucChanNum		= pstUserHomeTsInfo->stTuningParam.ter.ucChanNum;
#if defined(CONFIG_MW_CH_DVB_T2)
	pstDstHomeTsInfo->stTuningParam.ter.ucSystem					= pstUserHomeTsInfo->stTuningParam.ter.ucSystem;
	pstDstHomeTsInfo->stTuningParam.ter.stT2param.ulPlpId			= pstUserHomeTsInfo->stTuningParam.ter.stT2param.ulPlpId;
	pstDstHomeTsInfo->stTuningParam.ter.stT2param.ucPreambleFormat	= pstUserHomeTsInfo->stTuningParam.ter.stT2param.ucPreambleFormat;
	pstDstHomeTsInfo->stTuningParam.ter.stT2param.ucMixed			= pstUserHomeTsInfo->stTuningParam.ter.stT2param.ucMixed;
	pstDstHomeTsInfo->stTuningParam.ter.stT2param.ucPilotPattern		= pstUserHomeTsInfo->stTuningParam.ter.stT2param.ucPilotPattern;
	pstDstHomeTsInfo->stTuningParam.ter.stT2param.ucExtenedCarrier	= pstUserHomeTsInfo->stTuningParam.ter.stT2param.ucExtenedCarrier;
	pstDstHomeTsInfo->stTuningParam.ter.stT2param.ucPAPRreduction	= pstUserHomeTsInfo->stTuningParam.ter.stT2param.ucPAPRreduction;
	pstDstHomeTsInfo->stTuningParam.ter.stT2param.ulNumPlpBlocks	= pstUserHomeTsInfo->stTuningParam.ter.stT2param.ulNumPlpBlocks;
#endif
#endif

#if	defined(CONFIG_MW_CH_CABLE)
	pstDstHomeTsInfo->eDeliType 			= eDxDELIVERY_TYPE_CAB;
	pstDstHomeTsInfo->stTuningParam.cab.ulFreq			= pstUserHomeTsInfo->stTuningParam.cab.ulFreq;
	pstDstHomeTsInfo->stTuningParam.cab.usSR			= pstUserHomeTsInfo->stTuningParam.cab.usSR;
	pstDstHomeTsInfo->stTuningParam.cab.ucCon			= pstUserHomeTsInfo->stTuningParam.cab.ucCon;
	pstDstHomeTsInfo->stTuningParam.cab.ucSpec			= pstUserHomeTsInfo->stTuningParam.cab.ucSpec;
#endif
	return ERR_OK;
}

HERROR	XMGR_SWUP_GetHomeTpInfo_Dvbssu(DbSvc_TsInfo_t *pstHomeTsInfo)
{
	HERROR hErr		=	ERR_FAIL;

	if (!pstHomeTsInfo)
		return ERR_FAIL;

	HxSTD_MemSet(pstHomeTsInfo, 0, sizeof(DbSvc_TsInfo_t));

	//hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_HOME_TP_INFO_1, (HUINT32 *)&pstHomeTsInfo->stTuningParam.sat, 0);

	return hErr;
}

