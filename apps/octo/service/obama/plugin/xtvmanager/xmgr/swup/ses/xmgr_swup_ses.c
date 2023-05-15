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
#include <svc_sys.h>
#include <svc_pipe.h>
#include <svc_resources.h>
#include <svc_swup.h>
#include <svc_epg.h>

//#include <bus.h>
#include <mgr_common.h>
#include <mgr_action.h>
#include <mgr_swup.h>
#include <mgr_rscmap.h>
#include <mgr_schedule.h>
#include <xmgr_swup.h>

#include <xmgr_module_search.h>
#include "_xmgr_swup_ses.h"

#include <otl.h>

#include <_mgr_swup.h>

#define _____define_____

#define IPOTA_SERVER_URL	CONFIG_IPOTA_SERVER_URL
#define IPOTA_URL_FILE		"/url/url_ipota.txt"
#define IPOTA_INNER_URL_FILE  "/var/lib/humaxtv/url_ipota.txt"

#define _____static_variable_____
static POINTER_LIST_T		*s_pstApOtaInfo_BurstTpList = NULL;

#define _____typedef_____

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
} xsvcSwup_SesContents_t;

typedef enum
{
	eSWUP_BURSTUPDATE_PASSED,
	eSWUP_BURSTUPDATE_EXEC,
	eSWUP_BURSTUPDATE_WAITING,

	eSWUP_BURSTSTATE_MAX
} xmgrSwup_BurstSate_t;

xmgrSwup_BurstSate_t xmgr_swup_isBurstUpdate_Ses(void);

#define _____Static_Function_____

static void xmgr_swup_FreeListItem (void *pvItem)
{
	if (pvItem != NULL)
	{
		OxAP_Free (pvItem);
	}
}

HERROR XMGR_SWUP_ResetBurstUpdateTp (void)
{
	s_pstApOtaInfo_BurstTpList = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList (s_pstApOtaInfo_BurstTpList, (FREE_FUNC)xmgr_swup_FreeListItem);
	return ERR_OK;
}

STATIC HERROR xmgr_swup_AddOtaBurstUpdateTp (DbSvc_TsInfo_t *pstTsInfo)
{
	DbSvc_TsInfo_t	*pstNewTsInfo;

	if (pstTsInfo == NULL)					{ return ERR_FAIL; }

	pstNewTsInfo = (DbSvc_TsInfo_t *)OxAP_Malloc (sizeof(DbSvc_TsInfo_t));
	if (pstNewTsInfo == NULL)				{ return ERR_FAIL; }

	*pstNewTsInfo = *pstTsInfo;

	s_pstApOtaInfo_BurstTpList = UTIL_LINKEDLIST_AppendListItemToPointerList (s_pstApOtaInfo_BurstTpList, (void *)pstNewTsInfo);
	return ERR_OK;
}

HUINT32 XMGR_SWUP_CountBurstUpdateTp (void)
{
	return (HUINT32)UTIL_LINKEDLIST_GetNumListItemsInPointerList (s_pstApOtaInfo_BurstTpList, NULL, NULL);
}

HERROR XMGR_SWUP_GetBurstUpdateTp (HUINT32 ulIndex, DbSvc_TsInfo_t *pstTsInfo)
{
	DbSvc_TsInfo_t		*pstInfo;
	POINTER_LIST_T		*pstItem;

	if (pstTsInfo == NULL)					{ return ERR_FAIL; }

	pstItem = UTIL_LINKEDLIST_FindNthListItemFromPointerList (s_pstApOtaInfo_BurstTpList, (HUINT16)ulIndex, NULL, NULL);
	if (pstItem == NULL)					{ return ERR_FAIL; }

	pstInfo = (DbSvc_TsInfo_t *)pstItem->pvContents;
	if (pstInfo == NULL)					{ return ERR_FAIL; }

	*pstTsInfo = *pstInfo;
	return ERR_OK;
}

STATIC HERROR xmgr_swup_MakeEmergencySignalInfo_Ses(SvcSwUpdate_EmergencyUpdateInfo_t *pstEmergeInfo, SvcSwUpdate_DetectResult_t *pstDetectResult)
{
	if (NULL == pstDetectResult)
	{
		HxLOG_Error("pstDetectResult is NULL \n");
		return ERR_FAIL;
	}

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

HCHAR* xmgr_swup_GetSwUpdateVersionString_Ses (HBOOL bDetected, HBOOL bStandby, HUINT8 ucSwupType, HCHAR *pszCurVer, HCHAR *pszFoundVer, HCHAR *pszSchedInfo)
{
	STATIC HUINT8		s_szSwUpdateVersion[150];
	HxSTD_memset(s_szSwUpdateVersion, 0, 150);

	if(bStandby)
	{
		HxSTD_StrNCpy(s_szSwUpdateVersion, "STANDBY",16);
	}
	else
	{
		if(ucSwupType == SvcSi_SWUP_TYPE_USER_SELECT)
		{
			HxSTD_StrNCpy(s_szSwUpdateVersion, "USER_SELECT",16);
		}
		else if (ucSwupType == SvcSi_SWUP_TYPE_FORCED)
		{
			HxSTD_StrNCpy(s_szSwUpdateVersion, "FORCED",16);
		}
		else if (ucSwupType == SvcSi_SWUP_TYPE_BURST_USER_SELECT)
		{
			HxSTD_StrNCpy(s_szSwUpdateVersion, "BURST_USER",16);
		}
		else if (ucSwupType == SvcSi_SWUP_TYPE_BURST_FORCED)
		{
			HxSTD_StrNCpy(s_szSwUpdateVersion, "BURST_FORCED",16);
		}
		else
		{
			HxSTD_StrNCpy(s_szSwUpdateVersion, "CONDITIONAL",16);
		}
	}

	if(bDetected)
	{
		HxSTD_StrNCat(s_szSwUpdateVersion, pszCurVer, 32);
		HxSTD_StrNCat(s_szSwUpdateVersion, pszFoundVer, 32);

		if(pszSchedInfo)
			HxSTD_StrNCat(s_szSwUpdateVersion, pszSchedInfo, 60);
	}
	else
	{
		HxSTD_StrNCpy(s_szSwUpdateVersion, "", 16);
	}

	return s_szSwUpdateVersion;
}

#if defined (CONFIG_NAGRA_SSD_OTA_TEST)
static HERROR xmgr_swup_GetAstraAntIdx_Ses (HUINT32 *pulAntIdx)
{
	HBOOL			 bFound;
	HUINT32			 ulAntIdx;
	HINT32			 nAntCnt, nTotalAntNum;
	HINT32			*pnAntIdxArray;
	DbSvc_AntInfo_t		 stAntInfo;
	HERROR			 hErr;

	if (pulAntIdx == NULL)				{ return ERR_BUS_TARGET_NULL; }

	// ASTRA로 설정된 Antenna를 찾아야 한다.
	bFound = FALSE;
	pnAntIdxArray = NULL;
	ulAntIdx = 0;

	hErr = DB_SVC_FindAllAntIdxList (&nTotalAntNum, &pnAntIdxArray);
	if (hErr == ERR_OK)
	{
		for (nAntCnt = 0; nAntCnt < nTotalAntNum; nAntCnt++)
		{
			hErr = DB_SVC_GetAntInfo ((HUINT16)pnAntIdxArray[nAntCnt], &stAntInfo);
			if (hErr == ERR_OK)
			{
				if (stAntInfo.eSatType == eSatType_ASTRA_1)
				{
					bFound = TRUE;
					ulAntIdx = (HUINT32)pnAntIdxArray[nAntCnt];
					break;
				}
			}
		}

		DB_SVC_FreeAntList (pnAntIdxArray);
	}

	if (bFound == FALSE)
	{
		HxLOG_Critical("\n\n\n");
		return ERR_FAIL;
	}

	*pulAntIdx = ulAntIdx;
	return ERR_OK;
}
#endif
// eSWUP_STATE_Init
STATIC HERROR xmgr_swup_EnterActionInit_Ses(Handle_t hSWUpdate, MgrSwup_State_e ePrevState, void *pvUserData, HINT32 nMessage, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	HERROR  eRet = ERR_OK;
	static HINT8 szDetectUrl[128];
	HINT8 *szPortaltUrl;
	HINT8 szSWVersion[32];
	HUINT32 ulSWVersion = 0, ulSystemId = 0;
	HUINT32 ulVerNum1, ulVerNum2, ulVerNum3;
	HCHAR	*pszVersion = NULL;

	xsvcSwup_SesContents_t	*pstContents = (xsvcSwup_SesContents_t*)pvUserData;

	if ( MGR_ACTION_GetState() == eAmState_ACTIVE_STANDBY )
	{
		szPortaltUrl = (HUINT8 *)UAPI_FM_GetDefaultURL((HCHAR *)IPOTA_SERVER_URL, (HCHAR *)IPOTA_URL_FILE);
		UAPI_FM_SetInnerURL(szPortaltUrl, (HCHAR *)IPOTA_INNER_URL_FILE);
	}
	else
	{
	        szPortaltUrl = (HUINT8 *)UAPI_FM_GetInnerURL((HCHAR *)IPOTA_INNER_URL_FILE);

		if ( szPortaltUrl == NULL)
		{
			szPortaltUrl = (HUINT8 *)UAPI_FM_GetDefaultURL((HCHAR *)IPOTA_SERVER_URL, (HCHAR *)IPOTA_URL_FILE);
		}
	}

	if(szPortaltUrl == NULL)
	{
		HxLOG_Error("szPortaltUrl is NULL \n");
		return ERR_FAIL;
	}

	if (NULL == pstContents)
	{
		HxLOG_Error("pstContents is NULL \n");
		return ERR_FAIL;
	}

	// ota check 마다, ota info registry 를 init 해준다. 이렇게 매번하는데 clear 한듯...
	pszVersion = xmgr_swup_GetSwUpdateVersionString_Ses(FALSE, FALSE, 0, NULL, NULL, NULL);
	DB_PARAM_SetItem (eDB_PARAM_ITEM_SWUPDATE_VERSION, (HUINT32)pszVersion, 0);

	XMGR_SWUP_ResetBurstUpdateTp();

	if (pstContents->bUserTP == TRUE)
	{
#if defined (CONFIG_NAGRA_SSD_OTA_TEST)
		HERROR	hErr = ERR_OK;
		HUINT32 				 ulAntIdx;

		hErr = xmgr_swup_GetAstraAntIdx_Ses (&ulAntIdx);
		pstContents->stTsInfo.eDeliType = eDxDELIVERY_TYPE_SAT;
		pstContents->stTsInfo.ucTunerId = (HUINT8)-1;
		pstContents->stTsInfo.stTuningParam.sat.ucAntId = (HUINT8)ulAntIdx;
		pstContents->stTsInfo.stTuningParam.sat.eSatType = eSatType_ASTRA_1;
#endif
		MGR_SWUPINFO_ResetHomeTp();
		MGR_SWUPINFO_AddOtaHomeTp(&pstContents->stTsInfo);
	}
	else if (xmgr_swup_isBurstUpdate_Ses() == eSWUP_BURSTUPDATE_EXEC)
	{
		HERROR	hErr;
		DbSvc_TsInfo_t			stTsInfo;

		//GET TS INFO
		// RF 의 경우.. serched info 가 존재하는지 check
		hErr = MGR_SWUPINFO_GetSearchedInfo (&stTsInfo, 0, NULL);
		if(hErr == ERR_OK)
		{
			HxLOG_Debug("[%s:%d] busrt update tp - tsid (0x%x) onid (0x%x)\n", __FUNCTION__, __LINE__, stTsInfo.usTsId, stTsInfo.usOnId);
			xmgr_swup_AddOtaBurstUpdateTp(&stTsInfo);
		}
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

	HxLOG_Error("szDetectUrl = [%s] \n", szDetectUrl);

	//	Try IP Connection
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
STATIC HERROR xmgr_swup_EnterActionConnect_Ses(Handle_t hSWUpdate, MgrSwup_State_e ePrevState, void *pvUserData, HINT32 nMessage, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	xsvcSwup_SesContents_t		*pstContents = (xsvcSwup_SesContents_t*)pvUserData;

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
#if defined (CONFIG_NAGRA_SSD_OTA_TEST)
			if(pstContents->bUserTP)
			{
				SvcSwUpdate_DetectResult_t	stDetectResult;

				stDetectResult.ulDataVersion	= 0x020000;
				stDetectResult.usOtaPid 	= 0x7d3;
				stDetectResult.eSource		= eDxSWUPDATE_SOURCE_Rf;
				MGR_SWUP_SetDetectInfo(hSWUpdate, &stDetectResult);

				MGR_SWUP_PostMessage(hSWUpdate, eMEVT_SWUP_DOWNLOAD_START, HANDLE_NULL, eDxSWUPDATE_SOURCE_Rf, 0, 0);
			}
			else
#endif
			{
				MGR_SWUP_PostMessage(hSWUpdate, eMEVT_SWUP_DETECT_START, HANDLE_NULL, eDxSWUPDATE_SOURCE_Rf, (HINT32)&pstContents->stDetectRfParam, 0);
			}
			break;
		default:
			HxLOG_Error("Invalid eSource Type...\n");
			break;
	}

	return ERR_OK;
}

// eSWUP_STATE_Detect
STATIC HERROR xmgr_swup_EnterActionDetect_Ses(Handle_t hSWUpdate, MgrSwup_State_e ePrevState, void *pvUserData, HINT32 nMessage, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	xsvcSwup_SesContents_t		*pstContents = (xsvcSwup_SesContents_t *)pvUserData;

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
				MGR_SWUP_DetectCreate(0, hSWUpdate);
			}
			break;
		case eDxSWUPDATE_SOURCE_Ip:
			{
				MGR_SWUP_IpCreate(hSWUpdate, eSWUP_STATE_Detect, pstContents->pszIpDetectServerUrl);
			}
			break;
		case eDxSWUPDATE_SOURCE_Usb:
			{
				MGR_SWUP_CreateUsb(hSWUpdate);
			}
			break;

		default:
			break;
	}

	return ERR_OK;
}

// eSWUP_STATE_Download
STATIC HERROR xmgr_swup_EnterActionDownload_Ses(Handle_t hSWUpdate, MgrSwup_State_e ePrevState, void *pvUserData, HINT32 nMessage, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	xsvcSwup_SesContents_t		*pstContents = (xsvcSwup_SesContents_t*)pvUserData;
	SvcSwUpdate_DetectResult_t		*pstDetectResult;
	HERROR 						 hErr;
	DbSvc_TsInfo_t	 stTsInfo;
	SvcSi_SwupSignal_t	 stOtaInfo;
#if defined (CONFIG_NAGRA_SSD_OTA_TEST)
	if(MGR_ACTION_GetState() == eAmState_ACTIVE_STANDBY)
	{
		SvcSwUpdate_DetectResult_t		stDetectResult;

		stDetectResult.bFound = TRUE;
		stDetectResult.bAvailable = TRUE;
		stDetectResult.eSource = eDxSWUPDATE_SOURCE_Ip;
		stDetectResult.ulDataVersion = 0x20000;

		MGR_SWUP_SetDetectInfo(hSWUpdate, &stDetectResult);
	}
#endif

	if (NULL == pstContents)
	{
		HxLOG_Error("pstHumaxContents is NULL \n");
		return ERR_FAIL;
	}

	pstDetectResult = (SvcSwUpdate_DetectResult_t*)HLIB_STD_MemAlloc(sizeof(SvcSwUpdate_DetectResult_t));

	MGR_SWUP_GetDetectInfo(hSWUpdate, pstDetectResult);
	pstContents->eSource = pstDetectResult->eSource;

#if defined(CONFIG_DEBUG)
	HxLOG_Debug("hSwUpHandle(0x%x) eSource (%s) \n", hSWUpdate, XMGR_SWUP_GetEnumStrOfSource(pstContents->eSource));
#endif

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
			{
				DxTuneParam_t				stChTuneInfo;
				HUINT16 		 			usOtaPid;
				HUINT32 		 			ulDataVersion;

#if defined (CONFIG_NAGRA_SSD_OTA_TEST)
				if(pstContents->bUserTP)
				{
					stTsInfo = pstContents->stTsInfo;
					usOtaPid = pstDetectResult->usOtaPid;
					ulDataVersion = pstDetectResult->ulDataVersion;
				}
				else
#endif
				{
					HxSTD_MemSet(&stOtaInfo, 0x00, sizeof(SvcSi_SwupSignal_t));
					HxSTD_MemSet(&stChTuneInfo, 0x00, sizeof(DxTuneParam_t));

					// RF 의 경우.. serched info 가 존재하는지 check
					hErr = MGR_SWUPINFO_GetSearchedInfo (&stTsInfo, sizeof(SvcSi_SwupSignal_t), (void *)&stOtaInfo);
					if (hErr != ERR_OK)
					{
						HxLOG_Critical("\n\n");
						HLIB_STD_MemFree(pstDetectResult);
						return ERR_FAIL;
					}

					usOtaPid = stOtaInfo.usPid;
					ulDataVersion = stOtaInfo.ulDataVersion;
				}

				// 1. Tuning Info 를 DD용으로 converting 한다.
				// 1-1. DB Tuning Info -> Ch Tuning Info
				hErr = OTL_CONV_DbTuningInfo2DxTuneParam ((const DbSvc_TsInfo_t *)&stTsInfo, &stChTuneInfo);
				if (hErr != ERR_OK)
				{
					HxLOG_Error("OTL_CONV_DbTuningInfo2DxTuneParam Error!!!!\n");
					HLIB_STD_MemFree(pstDetectResult);
					return ERR_FAIL;
				}

				// 3. Cancel 한 정보 등을 다 삭제하도록 한다.
				MGR_SWUPINFO_ResetSearchedInfo();
				MGR_SWUPINFO_ResetOtaRetryCount();
				MGR_SWUPINFO_SetOtaCanceledVersion (0, 0);
				//burst update 일 경우 schedule info 를 clear 한다.
				if (xmgr_swup_isBurstUpdate_Ses() == eSWUP_BURSTUPDATE_EXEC)
				{
					SvcSi_SwupSchedule_t tempSchedule;

					HxSTD_MemSet(&tempSchedule, 0, sizeof(SvcSi_SwupSchedule_t));
					hErr = DB_PARAM_SetItem(eDB_PARAM_ITEM_OTA_SCHEDULEINFO, (HUINT32)&tempSchedule, sizeof(SvcSi_SwupSchedule_t));
				}

				// 4. Now, store the informations
				//	당장 Reboot 하지 않아도 곧 Reboot할 수 있으니 적어놓도록 한다.
				SVC_SYS_SetOtaType(eSWUP_INFO_SLOT_0, eSWUP_DOWNLOAD_DVBSSU); // RF는 eOTA_INFO_SLOT_0를 사용한다.
				SVC_SYS_SetOtaPid(eSWUP_INFO_SLOT_0, usOtaPid);
				SVC_SYS_SetOtaTuningInfo(eSWUP_INFO_SLOT_0, &stChTuneInfo);

				SVC_SYS_SetSystemVersion (eVERSION_TYPE_NEXTOTA, ulDataVersion);

				// Reboot 한다.
				SVC_SYS_SetOtaFlag (eSWUP_INFO_SLOT_0, eSWUP_FLAG_INFO_DETECTED);
				MGR_SWUP_PostMessage((Handle_t)hSWUpdate, eMEVT_SWUP_NOTIFY_DOWNLOAD_SUCCESS, (Handle_t)NULL, 0, 0, 0);
				MGR_ACTION_RebootSystem();
			}
			break;
		case eDxSWUPDATE_SOURCE_Ip:
			{
				static HCHAR	*pszTempURL = NULL;
				if (pszTempURL == NULL)
					pszTempURL = (HCHAR*)HLIB_STD_MemAlloc(MAX_PATH_LENGTH);

				if(MGR_ACTION_GetState() == eAmState_ACTIVE_STANDBY)
				{
					HUINT32 actionId;
					Handle_t hTvtvAction;

					actionId = MGR_ACTION_GetMainActionId();
					hTvtvAction  = SVC_PIPE_GetActionHandle(actionId);

					(void)SVC_EPG_Stop (eEPG_KIND_TVTV, eEPG_EVT_KIND_ALL , hTvtvAction);
				}

#if defined (CONFIG_NAGRA_SSD_OTA_TEST)
				MGR_SWUP_IpCreate(hSWUpdate, eSWUP_STATE_Download, "ftp://ftp.nagra.com/809C.5020/2.00.00");
#else
				HLIB_STD_StrNCpySafe(pszTempURL, pstDetectResult->data.stIpInfo.szURLPath, MAX_PATH_LENGTH - 1);
				MGR_SWUP_IpCreate(hSWUpdate, eSWUP_STATE_Download, pszTempURL);
#endif
			}
			break;

		default:
			break;
	}

	HLIB_STD_MemFree(pstDetectResult);

	return ERR_OK;
}



// eSWUP_STATE_Install
STATIC HERROR xmgr_swup_EnterActionInstall_Ses(Handle_t hSWUpdate, MgrSwup_State_e ePrevState, void *pvUserData, HINT32 nMessage, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	xsvcSwup_SesContents_t				*pstContents = (xsvcSwup_SesContents_t*)pvUserData;
	SvcSwUpdate_EmergencyUpdateInfo_t	*pstSourceInfo;
	HUINT8								*pszSourceFile;

	if (NULL == pstContents)
	{
		HxLOG_Error("pstHumaxContents is NULL \n");
		return ERR_FAIL;
	}

	pszSourceFile = (HINT8 *)DDB_SAVE_FILE_PATH;

	{
		SvcSwUpdate_EmergencyUpdateInfo_t	*pstEmergeInfo;
		pstEmergeInfo	= (SvcSwUpdate_EmergencyUpdateInfo_t*)HLIB_STD_MemAlloc(sizeof(SvcSwUpdate_EmergencyUpdateInfo_t));

		HxLOG_Debug("Initialization Emergency Info...\n");
		// detect 성공시 signaling 정보를 nvram 에 set 해준다.
		HxSTD_MemSet(pstEmergeInfo, 0, sizeof(SvcSwUpdate_EmergencyUpdateInfo_t));

		if (DB_PARAM_GetItem(eDB_PARAM_ITEM_OTA_EMERGENCYINFO, (HUINT32 *)pstEmergeInfo, sizeof(SvcSwUpdate_EmergencyUpdateInfo_t)) == ERR_OK)
		{
			HxLOG_Debug("\nEmergency Info : RF\n");
			HxLOG_Debug("[%s,%d]eSource : 0x%X, DataVersion : %ld, eDeliType : 0x%X, downloadType: 0x%X, usPid : 0x%X \n",
				__FUNCTION__,__LINE__,
				pstEmergeInfo->eSource, pstEmergeInfo->ulDataVersion,
				pstEmergeInfo->unInfo.stRfInfo.eDeliType,
				pstEmergeInfo->unInfo.stRfInfo.eDownloadType,
				pstEmergeInfo->unInfo.stRfInfo.usPid);

			if (eDxSWUPDATE_SOURCE_None != pstEmergeInfo->eSource)
			{
				HxSTD_MemCopy(&pstContents->stSourceInfo, pstEmergeInfo, sizeof(SvcSwUpdate_EmergencyUpdateInfo_t));
				pstContents->eSource = pstEmergeInfo->eSource;
			}
		}

		HLIB_STD_MemFree(pstEmergeInfo);
	}

#if defined (CONFIG_NAGRA_SSD_OTA_TEST)
	pstContents->stSourceInfo.eSource = eDxSWUPDATE_SOURCE_Ip;		// to do change!!
#endif
	pstSourceInfo = &pstContents->stSourceInfo;

	pstSourceInfo->eSource = pstContents->eSource;

#if defined(CONFIG_DEBUG)
	HxLOG_Debug("hSwUpHandle(0x%x) eSource (%s) \n", hSWUpdate, XMGR_SWUP_GetEnumStrOfSource(pstContents->eSource));
#endif

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
			break;

		case eDxSWUPDATE_SOURCE_Ip:
			{
				HxLOG_Debug("eDxSWUPDATE_SOURCE_Ip Install....\n");

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

	MGR_SWUP_InstallStart(hSWUpdate, pszSourceFile, pstSourceInfo);

	return ERR_OK;
}


// eSWUP_STATE_Error
STATIC HERROR xmgr_swup_EnterActionError_Ses(Handle_t hSWUpdate, MgrSwup_State_e ePrevState, void *pvUserData, HINT32 nMessage, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	xsvcSwup_SesContents_t		*pstContents = (xsvcSwup_SesContents_t*)pvUserData;
	// nParam3 에는 mgr단의 fail reason, nParam1에는 svc단의 fail reason... 이 부분 정리.

	if (NULL == pstContents)
	{
		HxLOG_Error("pstHumaxContents is NULL \n");
		return ERR_FAIL;
	}

	BUS_PostMessageToOM(NULL, eMEVT_DN_SWUP_ERROR, HANDLE_NULL, (HINT32)hSWUpdate, 0, nParam3);

	return ERR_OK;
}

// eSWUP_STATE_Finish
STATIC HERROR xmgr_swup_EnterActionFinish_Ses(Handle_t hSWUpdate, MgrSwup_State_e ePrevState, void *pvUserData, HINT32 nMessage, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	xsvcSwup_SesContents_t		*pstContents = (xsvcSwup_SesContents_t*)pvUserData;

	if (NULL == pstContents)
		return ERR_FAIL;

	BUS_PostMessageToOM(NULL, eMEVT_DN_SWUP_FINISH, HANDLE_NULL, (HINT32)hSWUpdate, 0, 0);

	return ERR_OK;
}

// eSWUP_STATE_Init
STATIC HERROR xmgr_swup_LeaveActionInit_Ses(Handle_t hSWUpdate, MgrSwup_State_e eNextState, void *pvUserData, HINT32 nMessage, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	xsvcSwup_SesContents_t		*pstContents = (xsvcSwup_SesContents_t*)pvUserData;

	HxLOG_Trace();
	if (NULL == pstContents)
	{
		HxLOG_Error("pstHumaxContents is NULL \n");
		return ERR_FAIL;
	}

//	MGR_SWUP_PostMessage(ulTag, eMEVT_SWUP_DETECT_START, ulTag, (HINT32)eDxSWUPDATE_SOURCE_Ip, (HINT32)&pstContents->stDetectParamIp, 0);

	return ERR_OK;
}

// eSWUP_STATE_Connect
STATIC HERROR xmgr_swup_LeaveActionConnect_Ses(Handle_t hSWUpdate, MgrSwup_State_e eNextState, void *pvUserData, HINT32 nMessage, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	xsvcSwup_SesContents_t		*pstContents = (xsvcSwup_SesContents_t*)pvUserData;

	HxLOG_Trace();
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
STATIC HERROR xmgr_swup_LeaveActionDetect_Ses(Handle_t hSWUpdate, MgrSwup_State_e eNextState, void *pvUserData, HINT32 nMessage, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	xsvcSwup_SesContents_t		*pstContents = (xsvcSwup_SesContents_t*)pvUserData;

	if (NULL == pstContents)
	{
		HxLOG_Error("pstHumaxContents is NULL \n");
		return ERR_FAIL;
	}

#if defined(CONFIG_DEBUG)
	HxLOG_Print("[%s][%04d]  hSwUpHandle(0x%x) Source (%s) Msg(%s) \n",
	__FUNCTION__, __LINE__, hSWUpdate, XMGR_SWUP_GetEnumStrOfSource(pstContents->eSource), XMGR_SWUP_GetEnumStrOfMessage(nMessage));
#endif

	if(nMessage == eMEVT_SWUP_NOTIFY_DETECT_SUCCESS)
	{
		SvcSwUpdate_EmergencyUpdateInfo_t	*pstEmergeInfo;
		SvcSwUpdate_DetectResult_t			*pstDetectResult;
		HUINT32 						ulSystemVersion;

		pstEmergeInfo	= (SvcSwUpdate_EmergencyUpdateInfo_t*)HLIB_STD_MemAlloc(sizeof(SvcSwUpdate_EmergencyUpdateInfo_t));
		pstDetectResult = (SvcSwUpdate_DetectResult_t*)HLIB_STD_MemAlloc(sizeof(SvcSwUpdate_DetectResult_t));

		SVC_SYS_GetSystemVersion(eVERSION_TYPE_APPL, &ulSystemVersion);
		MGR_SWUP_GetDetectInfo(hSWUpdate, pstDetectResult);

		HxLOG_Debug("eSWUP_STATE_Detect : eMEVT_SWUP_NOTIFY_DETECT_SUCCESS!!!\n\n");

		// detect 성공시 signaling 정보를 nvram 에 set 해준다.
		HxSTD_MemSet(pstEmergeInfo, 0, sizeof(SvcSwUpdate_EmergencyUpdateInfo_t));
		if (xmgr_swup_MakeEmergencySignalInfo_Ses(pstEmergeInfo, pstDetectResult) == ERR_OK)
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

		if (pstContents->eSource == eDxSWUPDATE_SOURCE_Ip)
		{
			HCHAR				szCurrentVersion[32], szNewSWVersion[32];
			HUINT32 				ulSystemVersion;
			HCHAR				*pszVersion = NULL;

			SVC_SYS_GetSystemVersion(eVERSION_TYPE_APPL, &ulSystemVersion);

			{
				SVC_SYS_MakeSystemVerString(eVERSION_TYPE_APPL, ulSystemVersion, szCurrentVersion);
				SVC_SYS_MakeSystemVerString(eVERSION_TYPE_APPL, pstDetectResult->ulDataVersion, szNewSWVersion);

				if (MGR_ACTION_GetState()== eAmState_ACTIVE_STANDBY)
				{
					// standby 에서의 detected 는 처리하지 않음.
					// 이것은 download 완료 시점에 한다.
					//MWC_PARAM_SetSwUpdateVersion(TRUE, TRUE, szCurrentVersion, szNewSWVersion);
					pszVersion = (HCHAR *)xmgr_swup_GetSwUpdateVersionString_Ses(TRUE, FALSE, SvcSi_SWUP_TYPE_USER_SELECT, szCurrentVersion, szNewSWVersion, NULL);
					DB_PARAM_SetItem(eDB_PARAM_ITEM_IPSWUPDATE_VERSION, (HUINT32)pszVersion, TRUE);
				}
				else
				{
					/*
					pszVersion = (HCHAR *)xmgr_swup_GetSwUpdateVersionString_Ses(TRUE, FALSE, SvcSi_SWUP_TYPE_USER_SELECT, szCurrentVersion, szNewSWVersion, NULL);
					DB_PARAM_SetItem(eDB_PARAM_ITEM_SWUPDATE_VERSION, (HUINT32)pszVersion, TRUE);
					*/
				}
			}
		}

		BUS_PostMessageToOM(NULL, eMEVT_DN_SWUP_DETECT_SUCCESS, HANDLE_NULL, (HINT32)hSWUpdate, ulSystemVersion,	(HINT32)pstDetectResult);

		HxLOG_Debug("Destroy OTA DetectAction Proc!! sourceType (0x%X)\n",pstContents->eSource);

		if(pstContents->stBaseContents.eBatchProcess != eDxBAT_TYPE_STANDBY)
		{
			switch(pstContents->eSource)
			{
				case eDxSWUPDATE_SOURCE_Usb:		MGR_SWUP_DestroyUsb();					break;
				case eDxSWUPDATE_SOURCE_Ip:			MGR_SWUP_IpDestroy(hSWUpdate);			break;
				case eDxSWUPDATE_SOURCE_Rf:			MGR_SWUP_DetectDestroy();				break;
				default:		break;
			}
		}

		pstContents->eSource = eDxSWUPDATE_SOURCE_None;

		//	free는 EVT_DN_SWUP_DETECT_SUCCESS를 받는 곳에서 수행.
		//	BUS상태에 따라서 메모리 릭이 있을수도 있지만, 담당자가 수정해 주겠지..
		HLIB_STD_MemFree(pstEmergeInfo);
	}
	else if(nMessage ==  eMEVT_SWUP_NOTIFY_DETECT_FAIL)
	{
		MgrAction_State_e eActionState;

		switch(pstContents->eSource)
		{
			// TODO: check usb ota
		case eDxSWUPDATE_SOURCE_Usb:
			HxLOG_Debug("eSWUP_STATE_Detect : DETECT_FAIL!!!\n\tRetry IP....\n");
			MGR_SWUP_DestroyUsb();
			pstContents->eSource = eDxSWUPDATE_SOURCE_Ip;
			break;

		case eDxSWUPDATE_SOURCE_Ip:
			HxLOG_Debug("eSWUP_STATE_Detect : DETECT_FAIL!!!\n\tRetry RF....\n");

			eActionState = MGR_ACTION_GetState();
			if (eAmState_ACTIVE_STANDBY == eActionState)
			{
#if defined (CONFIG_NAGRA_SSD_OTA_TEST)
#else
				BUS_PostMessageToOM(NULL, eMEVT_DN_SWUP_DETECT_FAIL, HANDLE_NULL, (HINT32)hSWUpdate, 0, 0);
				pstContents->eSource = eDxSWUPDATE_SOURCE_None;
#endif
			}
			else
			{
				pstContents->eSource = eDxSWUPDATE_SOURCE_Rf;
			}
			HxLOG_Print("YMSEO : [%s:%d] Destroy mgr_swup_IpControlProc\n", __FUNCTION__,__LINE__);
			if(pstContents->stBaseContents.eBatchProcess != eDxBAT_TYPE_STANDBY)
				MGR_SWUP_IpDestroy(hSWUpdate);

			break;

		case eDxSWUPDATE_SOURCE_Rf:
			HxLOG_Debug("eSWUP_STATE_Detect : DETECT_FAIL!!!\n");
			BUS_PostMessageToOM(NULL, eMEVT_DN_SWUP_DETECT_FAIL, HANDLE_NULL, (HINT32)hSWUpdate, 0, 0);
			pstContents->eSource = eDxSWUPDATE_SOURCE_None;
			MGR_SWUP_DetectDestroy();
			break;

		default:
			HxLOG_Debug("eSWUP_STATE_Detect : DETECT_FAIL!!!\n");
			BUS_PostMessageToOM(NULL, eMEVT_DN_SWUP_DETECT_FAIL, HANDLE_NULL, (HINT32)hSWUpdate, 0, 0);
			break;
		}
	}

	if(pstContents->eSource == eDxSWUPDATE_SOURCE_None)
	{
		BUS_PostMessage((BUS_Callback_t)NULL, eMEVT_ACTION_NOTIFY_RESULT, (Handle_t)NULL, eMGR_EVENT_SWUP_DETECT_FINISH, 0, 0);
	}

	return ERR_OK;
}


// eSWUP_STATE_Download
STATIC HERROR xmgr_swup_LeaveActionDownload_Ses(Handle_t hSWUpdate, MgrSwup_State_e eNextState, void *pvUserData, HINT32 nMessage, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	xsvcSwup_SesContents_t			*pSesstContents = (xsvcSwup_SesContents_t*)pvUserData;
	HERROR						hErr;

	if (NULL == pSesstContents)
	{
		HxLOG_Error("pSesstContents is NULL \n");
		return ERR_FAIL;
	}


#if defined(CONFIG_DEBUG)
	HxLOG_Print("[%s][%04d]  hSwUpHandle(0x%x) Source (%s) Msg(%s) \n",
	__FUNCTION__, __LINE__, hSWUpdate, XMGR_SWUP_GetEnumStrOfSource(pSesstContents->eSource), XMGR_SWUP_GetEnumStrOfMessage(nMessage));
#endif

	if(nMessage == eMEVT_SWUP_NOTIFY_DOWNLOAD_SUCCESS)
	{
		BUS_PostMessageToOM(NULL, eMEVT_DN_SWUP_DOWNLOAD_SUCCESS, HANDLE_NULL, (HINT32)hSWUpdate, 0, 0);

		if (MGR_ACTION_GetState()== eAmState_ACTIVE_STANDBY)
		{
			// standby 에서 ota 는 download 성공 한 상태라면,, operation mode 전환시,
			// user 에세 inform 하기 위해, successed 정보를 저장해놓는다. 그럼 상위 app 이 알아서 사용한다.
			// standby 가 아닌 경우 detected 시 저장하게 됨.
			HCHAR				szCurrentVersion[32], szNewSWVersion[32];
			HUINT32 				ulSystemVersion;
			SvcSwUpdate_DetectResult_t			*pstDetectResult	=	NULL;
			HERROR				hErr;

			HxLOG_Print("[%s][%04d] Current State is Download Successed  in eAmState_ACTIVE_STANDBY\n",__FUNCTION__,__LINE__);

			pstDetectResult = (SvcSwUpdate_DetectResult_t*)HLIB_STD_MemAlloc(sizeof(SvcSwUpdate_DetectResult_t));

			if (pstDetectResult != NULL)
			{
				SVC_SYS_GetSystemVersion(eVERSION_TYPE_APPL, &ulSystemVersion);
				hErr = MGR_SWUP_GetDetectInfo(hSWUpdate, pstDetectResult);
				if(hErr == ERR_OK)
				{
					HCHAR		*pszVersion = NULL;

					SVC_SYS_MakeSystemVerString(eVERSION_TYPE_APPL, ulSystemVersion, szCurrentVersion);
					SVC_SYS_MakeSystemVerString(eVERSION_TYPE_APPL, pstDetectResult->ulDataVersion, szNewSWVersion);

					pszVersion = xmgr_swup_GetSwUpdateVersionString_Ses(TRUE, TRUE, 0, szCurrentVersion, szNewSWVersion, NULL);
					DB_PARAM_SetItem(eDB_PARAM_ITEM_SWUPDATE_VERSION, (HUINT32)pszVersion, TRUE);
				}
				HLIB_STD_MemFree(pstDetectResult);
			}
		}
		else
		{
			HxLOG_Debug("Current State is eAmState_NORMAL\n");
		}
	}
	else if(nMessage == eMEVT_SWUP_NOTIFY_DOWNLOAD_FAIL)
	{
		BUS_PostMessageToOM(NULL, eMEVT_DN_SWUP_DOWNLOAD_FAIL, HANDLE_NULL, (HINT32)hSWUpdate, 0, 0);
	}
	else if (nMessage == eMEVT_SWUP_DOWNLOAD_STOP)
	{
		HxLOG_Error("cancelling before 50 percent (eMEVT_SWUP_DOWNLOAD_STOP) \n");
	}

	switch(pSesstContents->eSource)
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

	if((nMessage == eMEVT_SWUP_NOTIFY_DOWNLOAD_SUCCESS) || (nMessage == eMEVT_SWUP_NOTIFY_DOWNLOAD_FAIL))
	{
		BUS_PostMessage((BUS_Callback_t)NULL, eMEVT_ACTION_NOTIFY_RESULT, (Handle_t)NULL, eMGR_EVENT_SWUP_DOWNLOAD_FINISH, nMessage, 0);
	}

	return ERR_OK;
}


// eSWUP_STATE_Install
STATIC HERROR xmgr_swup_LeaveActionInstall_Ses(Handle_t hSWUpdate, MgrSwup_State_e eNextState, void *pvUserData, HINT32 nMessage, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	xsvcSwup_SesContents_t		*pstContents = (xsvcSwup_SesContents_t*)pvUserData;
	HCHAR			*pszVersion = NULL;

	HxLOG_Trace();
	if (NULL == pstContents)
	{
		HxLOG_Error("pstHumaxContents is NULL \n");
		return ERR_FAIL;
	}

	if(nMessage == eMEVT_SWUP_INSTALL_SUCCESS)
	{
		BUS_PostMessageToOM(NULL, eMEVT_DN_SWUP_INSTALL_SUCCESS, HANDLE_NULL, (HINT32)hSWUpdate, 0, 0);

		if((pstContents->eSource == eDxSWUPDATE_SOURCE_Ip))
		{
			pszVersion = XMGR_SWUP_GetSwUpdateVersionString_Ses(FALSE, FALSE, 0, NULL, NULL);		// 시작시마다, 이전 check 정보를 삭제해준다.
			DB_PARAM_SetItem(eDB_PARAM_ITEM_IPSWUPDATE_VERSION, (HUINT32)pszVersion, 0);
			HxLOG_Debug("reboot!!!\n");
			if (UAPI_FM_URLFILEExist((HCHAR *)IPOTA_INNER_URL_FILE) == TRUE)
			{
				UAPI_FM_INNERURLRemove((HCHAR *)IPOTA_INNER_URL_FILE);
			}
			MGR_ACTION_RebootSystem();
		}
	}
	else if(nMessage == eMEVT_SWUP_NOTIFY_INSTALL_FAIL)
	{
		BUS_PostMessageToOM(NULL, eMEVT_DN_SWUP_INSTALL_FAIL, HANDLE_NULL, (HINT32)hSWUpdate, 0, 0);
	}
	else if (nMessage == eMEVT_SWUP_DOWNLOAD_STOP)
	{
		HxLOG_Error("cancelling over 50 percent (eMEVT_SWUP_DOWNLOAD_STOP) \n");
	}

	// ip ota 다운로드중 50% 이상에서 cancel 시 다시 ip ota 시도시 50%에서 더이상 진행안하는 문제.
	switch(pstContents->eSource)
	{
		case eDxSWUPDATE_SOURCE_Ip:
			MGR_SWUP_InstallStop();

			break;

		default:
			break;
	}

	if(nMessage == eMEVT_SWUP_INSTALL_SUCCESS || nMessage == eMEVT_SWUP_NOTIFY_INSTALL_FAIL)
	{
		BUS_PostMessage((BUS_Callback_t)NULL, eMEVT_ACTION_NOTIFY_RESULT, (Handle_t)NULL, eMGR_EVENT_SWUP_INSTALL_FINISH, 0, 0);
		MGR_SWUP_InstallStop();
	}

	return ERR_OK;
}

// eSWUP_STATE_Error
STATIC HERROR xmgr_swup_LeaveActionError_Ses(Handle_t hSWUpdate, MgrSwup_State_e eNextState, void *pvUserData, HINT32 nMessage, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	xsvcSwup_SesContents_t		*pstContents = (xsvcSwup_SesContents_t*)pvUserData;

	HxLOG_Trace();
	if (NULL == pstContents)
	{
		HxLOG_Error("pstHumaxContents is NULL \n");
		return ERR_FAIL;
	}


	return ERR_OK;
}

// eSWUP_STATE_Finish
STATIC HERROR xmgr_swup_LeaveActionFinish_Ses(Handle_t hSWUpdate, MgrSwup_State_e eNextState, void *pvUserData, HINT32 nMessage, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	xsvcSwup_SesContents_t		*pstContents = (xsvcSwup_SesContents_t*)pvUserData;

	HxLOG_Trace();
	if (NULL == pstContents)
	{
		HxLOG_Error("pstHumaxContents is NULL \n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

BUS_Result_t	XMGR_SWUP_EnterAction_Ses(Handle_t hSWUpdate, MgrSwup_State_e eCurrState, MgrSwup_State_e ePrevState, void *pvUserData, HINT32 nMessage, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	xsvcSwup_SesContents_t	*pstContents = (xsvcSwup_SesContents_t*)pvUserData;
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
		hErr = xmgr_swup_EnterActionInit_Ses(hSWUpdate, ePrevState, pvUserData, nMessage, hAction, nParam1, nParam2, nParam3);
		break;
	case eSWUP_STATE_Connect:
		hErr = xmgr_swup_EnterActionConnect_Ses(hSWUpdate, ePrevState, pvUserData, nMessage, hAction, nParam1, nParam2, nParam3);
		break;
	case eSWUP_STATE_Detect:
		hErr = xmgr_swup_EnterActionDetect_Ses(hSWUpdate, ePrevState, pvUserData, nMessage, hAction, nParam1, nParam2, nParam3);
		break;
	case eSWUP_STATE_Download:
		hErr = xmgr_swup_EnterActionDownload_Ses(hSWUpdate, ePrevState, pvUserData, nMessage, hAction, nParam1, nParam2, nParam3);
		break;
	case eSWUP_STATE_Install:
		hErr = xmgr_swup_EnterActionInstall_Ses(hSWUpdate, ePrevState, pvUserData, nMessage, hAction, nParam1, nParam2, nParam3);
		break;
	case eSWUP_STATE_Error:
		hErr = xmgr_swup_EnterActionError_Ses(hSWUpdate, ePrevState, pvUserData, nMessage, hAction, nParam1, nParam2, nParam3);
		break;
	case eSWUP_STATE_Finish:
		hErr = xmgr_swup_EnterActionFinish_Ses(hSWUpdate, ePrevState, pvUserData, nMessage, hAction, nParam1, nParam2, nParam3);
		break;
	default:
		break;
	}
	if (hErr != ERR_OK)
	{
		HxLOG_Error("EnterAction failed. eCurrState(%d) \n",eCurrState);
	}

	return hRes;
}

BUS_Result_t	XMGR_SWUP_LeaveAction_Ses(Handle_t hSWUpdate, MgrSwup_State_e eCurrState, MgrSwup_State_e eNextState, void *pvUserData, HINT32 nMessage, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	xsvcSwup_SesContents_t		*pstContents = (xsvcSwup_SesContents_t*)pvUserData;
	BUS_Result_t			 hRes = MESSAGE_PASS;
	HERROR					 hErr = ERR_FAIL;

	if (NULL == pstContents)
	{
		HxLOG_Error("pstContents is NULL \n");
		return MESSAGE_BREAK;
	}

#if defined(CONFIG_DEBUG)
	HxLOG_Print("[%s][%04d] Change State (%s  -> %s)  \n", __FUNCTION__, __LINE__, XMGR_SWUP_GetEnumStrOfState(eCurrState), XMGR_SWUP_GetEnumStrOfState(eNextState));
#endif

	switch (eCurrState)
	{
	case eSWUP_STATE_Init:
		hErr = xmgr_swup_LeaveActionInit_Ses(hSWUpdate, eNextState, pvUserData, nMessage, hAction, nParam1, nParam2, nParam3);
		break;
	case eSWUP_STATE_Connect:
		hErr = xmgr_swup_LeaveActionConnect_Ses(hSWUpdate, eNextState, pvUserData, nMessage, hAction, nParam1, nParam2, nParam3);
		break;
	case eSWUP_STATE_Detect:
		hErr = xmgr_swup_LeaveActionDetect_Ses(hSWUpdate, eNextState, pvUserData, nMessage, hAction, nParam1, nParam2, nParam3);
		break;
	case eSWUP_STATE_Download:
		hErr = xmgr_swup_LeaveActionDownload_Ses(hSWUpdate, eNextState, pvUserData, nMessage, hAction, nParam1, nParam2, nParam3);
		break;
	case eSWUP_STATE_Install:
		hErr = xmgr_swup_LeaveActionInstall_Ses(hSWUpdate, eNextState, pvUserData, nMessage, hAction, nParam1, nParam2, nParam3);
		break;
	case eSWUP_STATE_Error:
		hErr = xmgr_swup_LeaveActionError_Ses(hSWUpdate, eNextState, pvUserData, nMessage, hAction, nParam1, nParam2, nParam3);
		break;
	case eSWUP_STATE_Finish:
		hErr = xmgr_swup_LeaveActionFinish_Ses(hSWUpdate, eNextState, pvUserData, nMessage, hAction, nParam1, nParam2, nParam3);
		break;
	default:
		break;
	}

	if (hErr != ERR_OK)
	{
		HxLOG_Error("LeaveAction failed. eCurrState(%d) \n",eCurrState);
	}
	return hRes;
}
MgrSwup_State_e XMGR_SWUP_GetNextState_Ses(Handle_t hSWUpdate, MgrSwup_State_e eState, void *pvUserData, HINT32 nMessage, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	xsvcSwup_SesContents_t		*pstContents = (xsvcSwup_SesContents_t*)pvUserData;
	MgrSwup_State_e				 eNextState = eSWUP_STATE_Unknown;

//	HxLOG_Debug("==== (+) Current eState[%s] Message [%s]\n", XMGR_SWUP_GetEnumStrOfState(eState), XMGR_SWUP_GetEnumStrOfMessage(nMessage));

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
#if defined (CONFIG_NAGRA_SSD_OTA_TEST)
				case eMEVT_SWUP_DOWNLOAD_START:
					eNextState = eSWUP_STATE_Download;
					break;
#endif
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
						// detect 된 경우에는 nextstate 가 뭐가 되어야 되나?????
						eNextState = eSWUP_STATE_Finish;
					}
					break;
				case eMEVT_SWUP_NOTIFY_DETECT_FAIL:
				{
					MgrAction_State_e eActionState;

					switch(pstContents->eSource)
					{
						case eDxSWUPDATE_SOURCE_Usb:
							HxLOG_Debug("eSWUP_STATE_Detect : DETECT_FAIL!!!\n");
							eNextState = eSWUP_STATE_Connect;
							break;

						case eDxSWUPDATE_SOURCE_Ip:
							HxLOG_Debug("eSWUP_STATE_Detect : DETECT_FAIL!!!\n");

							eActionState = MGR_ACTION_GetState();
							if (eAmState_ACTIVE_STANDBY == eActionState)
							{
#if defined (CONFIG_NAGRA_SSD_OTA_TEST)
								eNextState = eSWUP_STATE_Download;
#else
								eNextState = eSWUP_STATE_Error;
#endif
							}
							else
							{
								eNextState = eSWUP_STATE_Connect;
							}
							break;

						case eDxSWUPDATE_SOURCE_Rf:
							HxLOG_Debug("eSWUP_STATE_Detect : DETECT_FAIL!!!\n");
							eNextState = eSWUP_STATE_Error;
							break;

						default:
							HxLOG_Debug("eSWUP_STATE_Detect : DETECT_FAIL!!!\n");
							eNextState = eSWUP_STATE_Error;
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
					break;
			}
			break;

		case eSWUP_STATE_Install:
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
					break;
			}
			break;

		case eSWUP_STATE_Error:
			eNextState = eSWUP_STATE_Finish;
			break;
		case eSWUP_STATE_Finish:
			break;
		default:
			break;
	}

//	HxLOG_Debug("[%s][%04d] ==== (-) hSWUpdate (0x%X)  Change eState [%s -> %s  ->%s]  \n",
//		__FUNCTION__, __LINE__, hSWUpdate, XMGR_SWUP_GetEnumStrOfState(eState), XMGR_SWUP_GetEnumStrOfMessage(nMessage), XMGR_SWUP_GetEnumStrOfState(eNextState));

	return eNextState;
}

void * XMGR_SWUP_Create_Ses(void *pvUserData, DxBatchType_e eBatchProcess)
{
	xsvcSwup_SesContents_t	*pstContents;
	DxTransponder_t			*pstTuningInfo;
	DbSvc_TsInfo_t			 stSvcTsInfo;

	HxLOG_Trace();

	pstTuningInfo = (DxTransponder_t *)pvUserData;

	pstContents = (xsvcSwup_SesContents_t *)HLIB_STD_MemCalloc(sizeof(xsvcSwup_SesContents_t));

	pstContents->stBaseContents.eBatchProcess = eBatchProcess;

	pstContents->bUserTP = FALSE;
	if (pstTuningInfo)
	{
		pstContents->bUserTP = TRUE;

		HxSTD_MemSet(&stSvcTsInfo, 0x00, sizeof(DbSvc_TsInfo_t));

		stSvcTsInfo.stTuningParam.sat.ulFrequency = pstTuningInfo->tuningParam.sat.tuningInfo.ulFrequency;
		stSvcTsInfo.stTuningParam.sat.ulSymbolRate = pstTuningInfo->tuningParam.sat.tuningInfo.ulSymbolRate;
		stSvcTsInfo.stTuningParam.sat.eTransSpec = pstTuningInfo->tuningParam.sat.tuningInfo.eTransSpec;
		stSvcTsInfo.stTuningParam.sat.ePskMod = pstTuningInfo->tuningParam.sat.tuningInfo.ePskMod;
		stSvcTsInfo.stTuningParam.sat.ePolarization = pstTuningInfo->tuningParam.sat.tuningInfo.ePolarization;
		stSvcTsInfo.stTuningParam.sat.eFecCodeRate = pstTuningInfo->tuningParam.sat.tuningInfo.eFecCodeRate;

		HxLOG_Print("[%s:%d] ulFrequency (%d), ulSymbolRate (%d), eTransSpec (%d) ePskMod (%d), ePolarization (%d), eFecCodeRate (%d)\n",
			__FUNCTION__, __LINE__, stSvcTsInfo.stTuningParam.sat.ulFrequency, stSvcTsInfo.stTuningParam.sat.ulSymbolRate, stSvcTsInfo.stTuningParam.sat.eTransSpec,
			stSvcTsInfo.stTuningParam.sat.ePskMod, stSvcTsInfo.stTuningParam.sat.ePolarization, stSvcTsInfo.stTuningParam.sat.eFecCodeRate);

		HxSTD_MemCopy(&pstContents->stTsInfo, &stSvcTsInfo, sizeof(DbSvc_TsInfo_t));

	}

	return (void*)pstContents;
}

HERROR XMGR_SWUP_Destroy_Ses(void *pvContents)
{
	if(pvContents)
	{
		HLIB_STD_MemFree(pvContents);
	}
	return ERR_OK;
}

HERROR	XMGR_SWUP_SetManualInfo_Ses(DbSvc_TsInfo_t *pstDstHomeTsInfo, DbSvc_TsInfo_t *pstUserHomeTsInfo)
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

HERROR	XMGR_SWUP_GetHomeTpInfo_Ses(DbSvc_TsInfo_t *pstHomeTsInfo)
{
	HERROR hErr		=	ERR_FAIL;
//	HINT32	nValue;

	if (!pstHomeTsInfo)
		return ERR_FAIL;

	HxSTD_MemSet(pstHomeTsInfo, 0, sizeof(DbSvc_TsInfo_t));

	//hErr = DB_SETUP_GetUserItemBinary(USERCONFIG_ITEMNAME_HOME_TP_INFO_1, &nValue, (HUINT8 *)&pstHomeTsInfo->stTuningParam.sat);

	return hErr;
}

HCHAR* XMGR_SWUP_GetSwUpdateVersionString_Ses(HBOOL bDetected, HBOOL bStandby, HUINT8 ucSwupType, HCHAR *pszCurVer, HCHAR *pszFoundVer)
{
	return xmgr_swup_GetSwUpdateVersionString_Ses(bDetected, bStandby, ucSwupType, pszCurVer, pszFoundVer, NULL);
}

xmgrSwup_BurstSate_t xmgr_swup_isBurstUpdate_Ses(void)
{
	if(MGR_ACTION_GetState() == eAmState_ACTIVE_STANDBY)
	{
		HERROR				hErr;
		UNIXTIME			ulCurTime;


		//check burst update.
		ulCurTime = HLIB_STD_GetSystemTime();

		if (HLIB_DATETIME_IsValidUnixTime(ulCurTime) == ERR_OK )
		{
			SvcSi_SwupSchedule_t	stSchedule;

			hErr = DB_PARAM_GetItem (eDB_PARAM_ITEM_OTA_SCHEDULEINFO, (HUINT32 *)&stSchedule, (HUINT32)sizeof(SvcSi_SwupSchedule_t));

			if(hErr == ERR_OK)
			{
//				HxLOG_Debug("[%s:%d]ulCurTime (%d) startDateTime(%d) endDateTime (%d)\n", __FUNCTION__, __LINE__,
//					ulCurTime, stSchedule.startDateTime[0], stSchedule.endDateTime[0]);

				if(ulCurTime > stSchedule.startDateTime[0] && ulCurTime < stSchedule.endDateTime[0])
				{
					HxLOG_Debug("[%s:%d] return eSWUP_BURSTUPDATE_EXEC\n", __FUNCTION__, __LINE__);
					return eSWUP_BURSTUPDATE_EXEC;
				}
				else if(ulCurTime > stSchedule.endDateTime[0])
				{
					HxLOG_Debug("[%s:%d] return eSWUP_BURSTUPDATE_PASSED\n", __FUNCTION__, __LINE__);
					return eSWUP_BURSTUPDATE_PASSED;
				}
			}
		}
	}

	return eSWUP_BURSTUPDATE_WAITING;
}

HUINT32	xmgr_swup_GetStartUpdateSource_Ses(void)
{
	xmgrSwup_BurstSate_t eBurstState = xmgr_swup_isBurstUpdate_Ses();

	if(eBurstState == eSWUP_BURSTUPDATE_EXEC)
	{
		return (HUINT32)eDxSWUPDATE_SOURCE_Rf;
	}
	else if(eBurstState == eSWUP_BURSTUPDATE_PASSED)
	{
		MGR_SCHEDULE_ReScheduleBurstUpdate();
	}

	return (HUINT32)eDxSWUPDATE_SOURCE_Ip;
}

