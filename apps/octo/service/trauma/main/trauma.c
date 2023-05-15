
/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding 을 사용하는 source file 입니다.
* MS949 encodingを使用して source fileです。
* Quelldatei, die MS949-Codierung verwendet.
*
*/


/*
* ⓒ 2011-2012 Humax Co., Ltd.
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
*  CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.
*
* This License is effective until terminated. You may terminate this License at any time by destroying all copies
* of the Software including all documentation. This License will terminate immediately without notice from Humax
* to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
* of the Software and all documentation.
*
* The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
* All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
* in the Republic of Korea.
*/

/* *************************************************************************************************** */
#define ___HEADER________________________________________________________________________________________
/* *************************************************************************************************** */
#include <hapi.h>
#include <sapi.h>
#include <trauma_int.h>
#include <trauma_tool.h>
#include <directfb.h>

/* *************************************************************************************************** */
#define ___DEFINE________________________________________________________________________________________
/* *************************************************************************************************** */

/* *************************************************************************************************** */
#define ___TYPEDEF_______________________________________________________________________________________
/* *************************************************************************************************** */

/* *************************************************************************************************** */
#define ___STATIC_VARIABLES______________________________________________________________________________
/* *************************************************************************************************** */
static HLONG s_ulMsgId =0 ;

/* *************************************************************************************************** */
#define ___Private_FUNCTION______________________________________________________________________________
/* *************************************************************************************************** */
static void trauma_process(TRAUMA_MSG_t *pstMsgData)
{
	HxLOG_Debug("[RECV]msg:0x%x,p1:%x,p2:%x,p3:%x,p4:%x\n",
		pstMsgData->msg,pstMsgData->param1,pstMsgData->param2,pstMsgData->param3,pstMsgData->param4);

	if( pstMsgData->msg & eTRAUMA_MSG_ERIN)
		TRAUMA_ERIN_Process(pstMsgData);
	else if( pstMsgData->msg & eTRAUMA_MSG_STATISTICS)
		TRAUMA_STATISTICS_Process(pstMsgData);
	else
	{
		HxLOG_Error("[PASS] msg 0x%x\n",pstMsgData->msg);
	}
}

static void	trauma_mainTask (void *arg)
{
	TRAUMA_MSG_t stMsgData;

	while(1)
	{
		while (APK_EVENT_Dispatch());

		if(APK_SYSINFO_STB_Initialized() == ERR_OK)
		{
			HxSTD_MemSet(&stMsgData,0x0,sizeof(TRAUMA_MSG_t));
			if(VK_MSG_ReceiveTimeout( s_ulMsgId, &stMsgData, sizeof(TRAUMA_MSG_t),TRAUMA_TASK_SLEEP) == VK_OK)
			{
				trauma_process(&stMsgData);
			}
		}
		else
		{
			HxLOG_Debug("APK_SYSINFO_STB_Initialized == FAIL \n");
			VK_TASK_Sleep(TRAUMA_TASK_SLEEP);
		}
	}
}

static HERROR trauma_hapi_OnInputCallback(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	TRAUMA_MSG_t		stMsg;
	DFBInputEventType	eInputType;
	HINT32				nSymbol;

	if (nArgc < 2)
	{
		HxLOG_Error ("Argument wrong: nArgc(%d)\n", nArgc);
		return ERR_FAIL;
	}

	eInputType = (DFBInputEventType)HxOBJECT_INT (apArgv[0]);
	nSymbol = HxOBJECT_INT (apArgv[1]);

	if(eInputType == DIET_KEYPRESS)
	{
		HxSTD_MemSet(&stMsg,0x0,sizeof(TRAUMA_MSG_t));

		stMsg.msg 		= eTRAUMA_MSG_STATISTICS_CODE_KI;
		stMsg.param1 	= nSymbol;

		TRAUMA_MSG_Send(&stMsg);
	}
	return ERR_OK;
}

static void trauma_network_check_callback (unsigned long ulTimerId, void *pvParam)
{
	static HBOOL s_bAvailable = FALSE;
	HBOOL 	bCurAvailable = FALSE;

	if(APK_NETWORK_CONF_IsNetworkAvailable(&bCurAvailable) == ERR_OK)
	{
		if(s_bAvailable != bCurAvailable)
		{
			s_bAvailable = bCurAvailable;
			if(s_bAvailable == TRUE)
				TRAUMA_TVPORTAL_Login();
			else
				TRAUMA_TVPORTAL_LogOut();
		}
	}
}

static void trauma_appkit_power_callback(HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3)
{
	TRAUMA_MSG_t	stMsg;
	static HBOOL	s_bCurStandby = FALSE;

	HxSTD_MemSet(&stMsg,0x0,sizeof(TRAUMA_MSG_t));

	if(APK_POWER_GoingToOperate() == TRUE)
	{
		stMsg.msg = eTRAUMA_MSG_STATISTICS_CODE_SSTART;
		TRAUMA_MSG_Send(&stMsg);
		s_bCurStandby = FALSE;
	}
	else if(APK_POWER_GoingToShutdown() == TRUE && s_bCurStandby== FALSE)
	{
		stMsg.msg = eTRAUMA_MSG_STATISTICS_CODE_SSTAND;
		TRAUMA_MSG_Send(&stMsg);
		s_bCurStandby = TRUE;	// because active standby... call.. call..call..
	}

	(void)ulParam1;
	(void)ulParam2;
	(void)ulParam3;
}


static void trauma_appkit_audio_callback(void)
{
	static HUINT32 		s_ulVolume = 0;
	HUINT32 			ulCurVolume = 0, ulMaxVolume = 0;
	TRAUMA_MSG_t		stMsg;

	if(APK_OUTPUT_AUDIO_GetVolume(&ulCurVolume) == ERR_OK  && APK_OUTPUT_AUDIO_GetVolumeMax(&ulMaxVolume) == ERR_OK)
	{
		if(s_ulVolume != ulCurVolume)
		{
			s_ulVolume = (ulCurVolume * TRAUMA_VOLUME_STEP) / (ulMaxVolume);

			HxSTD_MemSet(&stMsg,0x0,sizeof(TRAUMA_MSG_t));
			stMsg.msg 		= eTRAUMA_MSG_STATISTICS_CODE_VOL;
			stMsg.param1 	= s_ulVolume;

			TRAUMA_MSG_Send(&stMsg);
		}
	}
}
static void trauma_appkit_storage_callback(DxStorage_NotiType_e eType, HINT32	p1, HINT32 p2, HINT32 p3)
{
	if(eType == eDxSTORAGE_NOTI_DevChange)
	{
		TRAUMA_MSG_t		stMsg;

		HxSTD_MemSet(&stMsg,0x0,sizeof(TRAUMA_MSG_t));
		stMsg.msg 		= eTRAUMA_MSG_STATISTICS_CODE_SUSB;
		TRAUMA_MSG_Send(&stMsg);
	}
}

static void trauma_appkit_hdmi_callback(void)
{
	TRAUMA_MSG_t		stMsg;

	HxSTD_MemSet(&stMsg,0x0,sizeof(TRAUMA_MSG_t));
	stMsg.msg 		= eTRAUMA_MSG_STATISTICS_CODE_SHDMI;
	TRAUMA_MSG_Send(&stMsg);
}


static void trauma_appkit_media_live_callback(HUINT32 ulViewId,HUINT32 ulSessionId,OxMediaPlay_Event_e eEvent)
{
	OxMediaPlay_LiveViewState_e eViewState = eOxMP_LiveViewState_OK;
	OxMediaPlay_LiveLockState_e eLockState = eOxMP_LiveLockState_OK;
	HBOOL						bCheckState = FALSE;
	TRAUMA_MSG_t				stMsg;

	APK_MEDIA_PLAY_GetViewState(ulViewId,&eViewState);
	APK_MEDIA_PLAY_GetLockState(ulViewId,&eLockState);

	HxSTD_MemSet(&stMsg,0x0,sizeof(TRAUMA_MSG_t));
	switch(eEvent)
	{
	case eOxMP_EVENT_TUNE_Locked:
		{
			HxLOG_Debug("eOxMP_EVENT_TUNE_Locked\n");
			if (ulSessionId == 0)
			{
				HxLOG_Debug("eOxMP_EVENT_TUNE_Locked (ulSessionId == 0)\n");
				if ((eOxMP_LiveViewState_NONE == eViewState) || (eOxMP_LiveLockState_NONE == eLockState))
				{
					HxLOG_Debug("eOxMP_LiveViewState_NONE || eOxMP_LiveLockState_NONE\n");
				}
				else if ((eOxMP_LiveViewState_OK == eViewState) && (eOxMP_LiveLockState_OK == eLockState))
				{
					HxLOG_Debug("eOxMP_LiveViewState_OK || eOxMP_LiveLockState_OK\n");
				}
				else
				{
					bCheckState = TRUE;
				}
			}
			else
			{
				HxLOG_Debug("eOxMP_EVENT_TUNE_Locked- (ulSessionId!=0) \n");
			}
		}
		break;
	case eOxMP_EVENT_LIVE_ViewStateChanged:
		HxLOG_Debug("eOxMP_EVENT_LIVE_ViewStateChanged\n");
		bCheckState = TRUE;
		break;
	case eOxMP_EVENT_LIVE_LockStateChanged:
		HxLOG_Debug("eOxMP_EVENT_LIVE_LockStateChanged\n");
		bCheckState = TRUE;
		break;
	case eOxMP_EVENT_LIVE_SvcChanged:
		HxLOG_Debug("eOxMP_EVENT_LIVE_SvcChanged\n");
		stMsg.msg 	= eTRAUMA_MSG_STATISTICS_CODE_ELWS;
		TRAUMA_MSG_Send(&stMsg);
		break;
	default:
		break;
	}

	if(bCheckState == TRUE)
	{
		HINT32				nError;
		nError= TRAUMA_TOOL_GetErrorByStates(eViewState,eLockState);

		stMsg.msg 		= eTRAUMA_MSG_STATISTICS_CODE_ERSYS;
		stMsg.param1 	= nError;
		TRAUMA_MSG_Send(&stMsg);
	}
}

static void trauma_appkit_media_callback ( HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eType, OxMediaPlay_Event_e eEvent, OxMediaPlay_NotifierData_t *pstNotifyData, void *pvUserData )
{
	switch( eType )
	{
		case eOxMP_MEDIATYPE_LIVE:
			trauma_appkit_media_live_callback(ulViewId,ulSessionId,eEvent);
			break;
		case eOxMP_MEDIATYPE_PVRPLAYBACK:
			break;
		case eOxMP_MEDIATYPE_TSRPLAYBACK:
			break;
		case eOxMP_MEDIATYPE_MEDIAPLAY:
			break;
		default:
			break;
	}
}

static HBOOL trauma_appkit_media_eventfilter(HUINT32 ulViewId, OxMediaPlay_MediaType_e eType, OxMediaPlay_Event_e eEvent, HBOOL *updated)
{
	*updated = FALSE;

	switch(eType)
	{
	case eOxMP_MEDIATYPE_LIVE:
	case eOxMP_MEDIATYPE_TSRPLAYBACK:
	case eOxMP_MEDIATYPE_PVRPLAYBACK:
		*updated = TRUE;
		break;
	default:
		break;
	}
	return TRUE;
}

static void trauma_appkit_swupdate_callback(DxSwUpdate_EventData_t *pstData)
{
	DxSwUpdate_Source_e		eSourceType;
	DxSwUpdate_Event_e		eEvent;
	static HBOOL			s_bDown = FALSE;
	static HBOOL			s_bSucccess = FALSE;
	static HBOOL			s_bDetect = FALSE;
	TRAUMA_MSG_t			stMsg;

	eSourceType = pstData->eSourceType;
	eEvent		= pstData->eEvent;

	HxSTD_MemSet(&stMsg,0x0,sizeof(TRAUMA_MSG_t));
	switch(eEvent)
	{
	case eDxSWUPDATE_EVENT_DetectSuccess:
	case eDxSWUPDATE_EVENT_DetectSuccessInLive:
		{
			HCHAR *pszCurver	= NULL;
			HCHAR *szFoundVer	= NULL;

			s_bDetect = TRUE;
			pszCurver = pstData->unEventData.stDetectEvent.szCurrentVer;
			szFoundVer = pstData->unEventData.stDetectEvent.szFoundVer;

			stMsg.msg 		= eTRAUMA_MSG_STATISTICS_CODE_CKOTA;
			stMsg.param1 	= (HINT32)eSourceType;
			if(HxSTD_StrEmpty(pszCurver)== FALSE)
				stMsg.param2 	= (HINT32)HLIB_STD_StrDup(pszCurver);
			if(HxSTD_StrEmpty(szFoundVer)== FALSE)
				stMsg.param3 	= (HINT32)HLIB_STD_StrDup(szFoundVer);
			TRAUMA_MSG_Send(&stMsg);
		}
		break;
	case eDxSWUPDATE_EVENT_DownloadProgress:
		if(s_bDown == FALSE)
		{
			stMsg.msg 		= eTRAUMA_MSG_STATISTICS_CODE_DLS;
			TRAUMA_MSG_Send(&stMsg);
			s_bDown = TRUE;
		}
		break;
	case eDxSWUPDATE_EVENT_DownloadSuccess:
		s_bSucccess = TRUE;
		break;
	case eDxSWUPDATE_EVENT_Finish:
		if(s_bDetect == TRUE)
		{
			stMsg.msg 		= eTRAUMA_MSG_STATISTICS_CODE_DLE;
			stMsg.param1	= (HINT32)s_bSucccess;
			TRAUMA_MSG_Send(&stMsg);

			s_bDown = FALSE;
			s_bSucccess = FALSE;
			s_bDetect = FALSE;
		}
		break;
	default:
		break;
	}

}

static void trauma_appkit_schedule_callback(void *pvUserData, APK_META_SCH_MsgType_e eMsgType, HUINT32 ulSlot, DxSchedule_t *pstSchedule)
{
	TRAUMA_Msg_e 	eMsg;
	TRAUMA_MSG_t	stMsg;

	if(pstSchedule)
	{
		eMsg = TRAUMA_TOOL_GetCodeByScheduleType(eMsgType,pstSchedule->eRsvType);
		if(eMsg != eTRAUMA_MSG_START)
		{
			HxSTD_MemSet(&stMsg,0x0,sizeof(TRAUMA_MSG_t));
			stMsg.msg 		= eMsg;
			stMsg.param1	= (HINT32)HLIB_MEM_MemDup(pstSchedule,sizeof(DxSchedule_t));

			TRAUMA_MSG_Send(&stMsg);
		}
	}
}

static void trauma_appkit_Init(void)
{
	HUINT32 ulMainViewId = 0;

	APK_POWER_Init();
	APK_NETWORK_Init();
	APK_SCENARIO_Init();
	APK_SYSINFO_STB_Init();
	APK_META_ANTENNA_Init(FALSE);
	APK_META_SVC_Init(FALSE);
	APK_DB_Init();
	APK_META_QUERY_Init(FALSE);
	APK_OUTPUT_AUDIO_Init();
	APK_OUTPUT_VIDEO_Init();
	APK_FILEMGR_STORAGE_Init();
	APK_OUTPUT_HDMI_Init();
	APK_MEDIA_PLAY_Init();
	APK_SWUPDATE_Init();

	APK_POWER_RegisterNotifier(trauma_appkit_power_callback);
	APK_OUTPUT_AUDIO_RegisterNotifier(trauma_appkit_audio_callback);
	APK_FILEMGR_STORAGE_RegisterNotifier(trauma_appkit_storage_callback);
	APK_OUTPUT_HDMI_RegisterNotifier(trauma_appkit_hdmi_callback);
	APK_SWUPDATE_SwupdateInit(trauma_appkit_swupdate_callback);
	APK_META_SCHEDULE_RegisterNotifier(trauma_appkit_schedule_callback, APK_META_SCHEDULE_GetInstance());

	APK_MEDIA_PLAY_GetMainViewId(&ulMainViewId);
	APK_MEDIA_PLAY_RegisterNotifier(ulMainViewId, trauma_appkit_media_callback, trauma_appkit_media_eventfilter, NULL);

	APK_META_SVC_Load();
	SAPI_RefreshScheduleData();

}

static void trauma_hapi_connect(void)
{
	HAPI_ConnectSystemSignal("hama:onInputNotify", "ii", NULL, trauma_hapi_OnInputCallback);
}

static void trauma_init(void)
{
	HULONG	ulTaskId=0,ulTimerid=0;

	HAPI_Init();

	(void)VK_TIMER_EventEvery(60*1000, trauma_network_check_callback ,NULL,0,&ulTimerid);
	(void)VK_TASK_Create(trauma_mainTask, VK_TASK_PRIORITY_MW_REF, SIZE_256K, "traumaMain", NULL, &ulTaskId, 0);
	(void)VK_MSG_Create(512, sizeof(TRAUMA_MSG_t), "trauma_msg", &s_ulMsgId, VK_SUSPENDTYPE_FIFO );
	(void)VK_TASK_Start(ulTaskId);

	TRAUMA_STATISTICS_Init();
	TRAUMA_TVPORTAL_Init();
	trauma_appkit_Init();
	trauma_hapi_connect();
	TRAUMA_RPC_Init();
#if defined(CONFIG_DEBUG)
	TRAUMA_CMD_Init();
#endif

}
/* *************************************************************************************************** */
#define ___PUBLIC_FUNCTION_______________________________________________________________________________
/* *************************************************************************************************** */
void TRAUMA_MSG_Send(TRAUMA_MSG_t *pstMsg)
{
	if(s_ulMsgId != 0)
		(void)VK_MSG_SendTimeout(s_ulMsgId,pstMsg,sizeof(TRAUMA_MSG_t),0);
}

HCHAR* TRAUMA_GetLanMac(void)
{
	HUINT32 i, ulCount =0;
	APKS_NETDEVINFO_t Info;
	static HCHAR s_szMacAddr[APKD_NETWORK_MAX_MACADDR] = {0,};

	if(HxSTD_StrEmpty(s_szMacAddr) ==TRUE)
	{
		APK_NETWORK_CONF_GetNumOfNetDev(&ulCount);
		for(i=0;i < ulCount;i++)
		{
			HxSTD_MemSet(&Info, 0x00, sizeof(APKS_NETDEVINFO_t));
			APK_NETWORK_CONF_GetDevInfo(i,&Info);
			if(Info.eType == eINTERFACE_TYPE_LAN)
			{
				HxSTD_StrCpy(s_szMacAddr,Info.szMacAddr);
				break;
			}
		}
	}
	return s_szMacAddr;
}

HBOOL TRAUMA_GetWLanMac(HCHAR *pszMacAddress)
{
	HUINT32				ulCount =0,i;
	HBOOL 				bRet = FALSE;
	APKS_NETDEVINFO_t	stNetInfo;

	APK_NETWORK_CONF_GetNumOfNetDev(&ulCount);
	for(i=0;i < ulCount;i++)
	{
		HxSTD_MemSet(&stNetInfo, 0x00, sizeof(APKS_NETDEVINFO_t));
		APK_NETWORK_CONF_GetDevInfo(i,&stNetInfo);
		if(stNetInfo.eType ==eINTERFACE_TYPE_WLAN )
		{
			HxSTD_StrNCpy(pszMacAddress, stNetInfo.szMacAddr,APKD_NETWORK_MAX_MACADDR);
			bRet = TRUE;
			break;
		}
	}
	return bRet;
}

HCHAR* TRAUMA_GetIPaddress(void)
{
	HUINT32 i, ulCount =0;
	APKS_NETDEVINFO_t 	devInfo;
	APKS_NETIFADDR_t	netInfo;
	static HCHAR s_szIpAddr[APKD_NETWORK_MAX_IP4ADDR] = {0,};

	HxSTD_MemSet(s_szIpAddr,0x0,APKD_NETWORK_MAX_IP4ADDR);
	APK_NETWORK_CONF_GetNumOfNetDev(&ulCount);
	for(i=0;i < ulCount;i++)
	{
		HxSTD_MemSet(&devInfo, 0x00, sizeof(APKS_NETDEVINFO_t));
		APK_NETWORK_CONF_GetDevInfo(i,&devInfo);
		if(devInfo.bConnected && devInfo.bNetActive)
		{
			HxSTD_MemSet(&netInfo, 0x00, sizeof(APKS_NETIFADDR_t));
			if(APK_NETWORK_CONF_GetAddrInfo(i,&netInfo)==ERR_OK)
			{
				APK_NETWORK_CONF_AddrN2A(netInfo.ulIpAddr,s_szIpAddr,APKD_NETWORK_MAX_IP4ADDR);
			}
			break;
		}
	}
	return s_szIpAddr;
}

HERROR TRAUMA_GetSystemInfo(HCHAR **ppszSysId,HCHAR **ppSysVer)
{
	static HCHAR	s_pszSysId[32]={0,};
	static HCHAR 	s_pszSysVer[32] ={0,};

	if(HxSTD_StrEmpty(s_pszSysId) == TRUE || HxSTD_StrEmpty(s_pszSysVer) == TRUE)
	{
		HCHAR	szSWVersion[32] = {0,};
		HUINT32 uppper = 0, lower=0,middle =0;
		HCHAR	*save, *token, *ptr;
#if 0	// 아직 정보가 올라오지 않음...
 		(void)APK_SYSINFO_STB_GetSystemId(s_pszSysId, 32); // is string... "9010.7D80"
#else
		HLIB_STD_StrNCpySafe(s_pszSysId,"9010.7D80",32);
#endif
		(void)APK_SYSINFO_STB_GetSoftwareVersion(szSWVersion, 32); // is string... "SASNAA 0.00.01"
		if(HxSTD_StrEmpty(s_pszSysId)==TRUE || HxSTD_StrEmpty(szSWVersion) == TRUE)
		{
			HxLOG_Error("APK_SYSINFO_STB_GetSystemId(%s) / APK_SYSINFO_STB_GetSoftwareVersion(%s)..Failed\n",s_pszSysId,szSWVersion);
			return ERR_FAIL;
		}
		ptr 	= szSWVersion + 7; //SKIP prefix.. SASNAA...
		token 	= HxSTD_strtok_r(ptr, ".", &save);
		if(token)
		{
			uppper 	= strtol(token, NULL, 16);
			token 	= HxSTD_strtok_r(NULL, ".", &save);
			middle 	= strtol(token, NULL, 16);
			token 	= HxSTD_strtok_r(NULL , ".", &save);
			lower 	= strtol(token, NULL, 16);
			HxSTD_snprintf(s_pszSysVer,32,"%X.%X.%X",uppper, middle, lower);
		}
	}
	*ppszSysId = s_pszSysId;
	*ppSysVer = s_pszSysVer;

	return ERR_OK;
}

HBOOL TRAUMA_GetRemoteInfo(HxDATETIME_t *pstStartTime,HxDATETIME_t *pstEndTime)
{
	HBOOL 			nRemoteProgram = FALSE;
	HUINT32			ulStartTime;

	(void)APK_DB_GetInt(USERDB_RP_ONOFF, &nRemoteProgram);
	if(nRemoteProgram == TRUE)
	{
		ulStartTime = 0;
		(void)APK_DB_GetInt(USERDB_RP_PERIOD_START, (HINT32*)&ulStartTime);
		(void)HLIB_DATETIME_ConvertUnixTimeToDateTime(ulStartTime, pstStartTime);
		ulStartTime = 0;
		(void)APK_DB_GetInt(USERDB_RP_PERIOD_END, (HINT32*)&ulStartTime);
		(void)HLIB_DATETIME_ConvertUnixTimeToDateTime(ulStartTime, pstEndTime);
	}

	return nRemoteProgram;
}

HBOOL	TRAUMA_GetPowerOnTimerInfo(HCHAR *pszChannel,HUINT32 *pulVolume,DxRsvRepeat_e *peRepeat)
{
	HBOOL bRet = FALSE;
	DxSchedule_t stSchedule;

	HxSTD_MemSet(&stSchedule,0x0,sizeof(DxSchedule_t));
	if(SAPI_GetScheduleByType(DxRSVTYPE_POWER_ON,0,&stSchedule) == ERR_OK)
	{
		HxVector_t *pList	= NULL;
		DxService_t	*pService = NULL;
		HUINT32		ulMaxVolume = 0;
		pList = APK_META_SVC_FindServiceByNumber(stSchedule.uExtInfo.stPwr.usChNum);
		pService = HLIB_VECTOR_First(pList);

		HxSTD_StrNCpy(pszChannel,pService->name,DxNAME_LEN);
	 	(void)APK_OUTPUT_AUDIO_GetVolumeMax(&ulMaxVolume);
		*pulVolume = (stSchedule.uExtInfo.stPwr.ucVolume * TRAUMA_VOLUME_STEP) / (ulMaxVolume);
		*peRepeat	=  stSchedule.eRepeat;

		HLIB_VECTOR_Delete(pList);
		bRet = TRUE;
	}
	return bRet;
}

int main(void)
{
	VK_Init();

 	HLIB_EXTRA_DropRootPrivileges("trauma");
	trauma_init();

	while(1)
	{
		VK_TASK_Sleep(10000000);
	}
}

