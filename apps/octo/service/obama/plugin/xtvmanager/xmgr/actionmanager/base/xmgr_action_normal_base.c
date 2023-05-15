/**
	@file     ap_actionmgr_normal.c
	@brief    Normal�� TV ��û ������ action application���� creation/destroy �� �����ϰ� ���ҽ��� �Ҵ��ϴ� ���� application.

	Description: WatchTV, Recording, TSR, search �� action application���� �ó������� ���ҽ� ��� ��Ȳ�� ����\n
				���� ��Ű�� ���ֱ⵵ �Ѵ�. Action application�� ���ҽ��� �Ҵ� �޴� API�� �����Ѵ�.
	Module: AP/AM			 	\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>

#include <hapi.h>
#include <papi.h>

#include <db_svc.h>
#include <db_param.h>

#include <svc_pipe.h>
#include <svc_sbtl.h>
#include <svc_ttx.h>
#include <svc_sys.h>
#include <svc_cas.h>
#include <svc_av.h>
#include <svc_output.h>
#include <svc_epg.h>
#include <svc_download.h>
#include <svc_si.h>
#include <svc_fs.h>
#include <svc_download.h>
#include <bus.h>

#include <mgr_common.h>
#include <mgr_svclist.h>
#include <mgr_action.h>
#include <mgr_live.h>
#include <mgr_media.h>
#include <mgr_swup.h>
#include <mgr_network.h>
#include <mgr_storage.h>
#include <mgr_pvr_action.h>
#include <mgr_epg.h>
#include <xmgr_epg.h>
#include <mgr_output.h>
#if defined(CONFIG_MW_CAS)
#include <mgr_cas.h>
#endif


/******** local header files *******/
#include "../local_include/_xmgr_action.h"


/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define BACKGROUND_TASK_LAUNCH_TIMER		0xff00
#define BACKGROUND_TASK_LAUNCH_TIMEOUT		(500)	// 0.5 seconds

#if defined(CONFIG_DEBUG_AGING_TEST)
#define BACKGROUND_AGING_LOG_TIMER_ID		0xff01
#define BACKGROUND_AGING_LOG_TIMEOUT		(60*1000) // 1 minute
#endif

// xproc_action_Normal_Base() create �� 10 �� �Ŀ� NETWORK ���� ������ �Ǵ��Ѵ�.
#define NETWORK_CHECK_TIMER					0xfc0d
#define NETWORK_CHECK_TIMEOUT				(5 * 1000)	// 5 seconds

#define REBOOT_TIME							60
#define UNIXTIME_VALID_CHECK				(24 * 60 * 60)

#define RECENTLIST_TIMER_ID				300
#define RECENTLIST_ADD_TIMEOUT			(3*60*1000)	/* 3 min */

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
static HUINT32		s_ulRecentListActionId;
/*******************************************************************/
/***************** Static Function Prototypes  *********************/
/*******************************************************************/

#define _____Static_Function_____

STATIC BUS_Result_t xmgr_am_StartNoDbState(void)
{
	xmgr_action_SetSvcDbState(eSvcDBState_NoSvcDB);

	/* Reset current/last zapping info. */
	MGR_LIVE_ResetZappingInfo(FALSE);

	return ERR_BUS_NO_ERROR;
}

STATIC BUS_Result_t xmgr_am_SetIsSvcDbState(void)
{
	xmgr_action_SetSvcDbState(eSvcDBState_IsSvcDB);
	return ERR_BUS_NO_ERROR;
}

STATIC BUS_Result_t xmgr_am_StartFTPServer(void)
{
	return ERR_BUS_NO_ERROR;
}

STATIC BUS_Result_t xmgr_am_MsgGwmGoOperation(Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR			hErr;

	/* AV ��� ���� �ʱ�ȭ */
	/* Zapping ���� �ʱ�ȭ... */
	MGR_LIVE_ZapInit();
	CHECK_BOOT_TIME("MGR_LIVE_ZapInit");

	SVC_OUT_SetScartPath(eDxSCART_PATH_STB2TV);
	SVC_OUT_SetMasterAudioMute(FALSE);
	MGR_ACTION_SetMainActionId (eActionId_VIEW_0);

#if defined(CONFIG_MW_SUBTITLE)
	hErr = SVC_SBTL_SetMasterAction(MGR_ACTION_GetMainActionId());
	if(ERR_OK!=hErr)	HxLOG_Critical("Critical Eror!!!\n");
#endif

	//db_sm_ResetRatingPassedEventId();
	DB_SVC_ResetRatingPassedEventId();

// ���� Reset / Power off�� ���� ��, Loader�� Video�� Panel �� �� ������� �Ѵ�
	return ERR_BUS_NO_ERROR;

}


STATIC BUS_Result_t xmgr_am_MsgGwmCreate(HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32			nSvcCnt;

	CHECK_BOOT_TIME("xproc_action_Normal_Base - Create - Start");
	DB_PARAM_Sync();

	/* Create Key Dispatcher */
	nSvcCnt = MGR_SVCLIST_CountSvcInDB(MGR_SVCLIST_GetSvcTypeForListCount()); /* Service DB�� �ִ��� �˾ƺ�. */
	CHECK_BOOT_TIME("MGR_SVCLIST_CountSvcInDB");
	if (nSvcCnt == 0)
		xmgr_am_StartNoDbState();
	else
		xmgr_am_SetIsSvcDbState();

#if 0
	SVC_SYS_OnOffKeyInput(TRUE); /* ��� ��ȯ �� disable�ߴ� key �Է� �ٽ� �츲 */
	CHECK_BOOT_TIME("SVC_SYS_OnOffKeyInput");
#endif

#if defined (CONFIG_MW_CAS_VIACCESS)
	// va�� init�� �� �����̰�, ���⼭���� task start�Ͽ� ���� ó�� ������..
	// �ȱ׷��� ���� ����  camlock, s/c �޽����� ����� ǥ�þȵ�
	AP_CAS_VA_Start();
#endif

	xmgr_am_StartFTPServer();


/* svc_epgrawsection_CheckSvcSection ���� Error�� ��� hangup �Ǵ� ��찡 �־� �켱 ����. */
#if defined(CONFIG_OP_FREESAT)
	XMGR_FSAT_StartEITManager();
#endif

	if(p1 == TRUE)	// p1 is go operation
	{
		xmgr_am_MsgGwmGoOperation((HUINT32)NULL,0,0,0);
	}

#if defined(CONFIG_DEBUG_AGING_TEST)
	BUS_SetTimer(BACKGROUND_AGING_LOG_TIMER_ID, BACKGROUND_AGING_LOG_TIMEOUT);

#if defined(CONFIG_PROD_DEVICE_STORAGE_USB) && !defined(CONFIG_MW_MM_PVR) && !defined(CONFIG_SUPPORT_UMMA)
	SVC_SYS_AgingLog_UsbInit();
#endif
#endif

	CHECK_BOOT_TIME("xproc_action_Normal_Base - Create - End");
	return MESSAGE_BREAK;
}

STATIC BUS_Result_t xmgr_am_MsgGwmDestroy(HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3)
{
//	BUS_SendMessage(NULL, MSG_ACTION_MANUAL_I_FINISH, 0, 0, 0, 0);

#if defined(CONFIG_DEBUG_AGING_TEST)
	BUS_KillTimer(BACKGROUND_AGING_LOG_TIMER_ID);
#endif

	return ERR_BUS_NO_ERROR;
}

#if 0	// ���� ��� ���� ����.
STATIC BUS_Result_t xmgr_am_MsgGwmGoStandby(Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (p2 == eKeyDevice_RCU || p2 == eKeyDevice_FRONT)
	{
	}

	/* MW shutdown */
	MW_SI_ShutDown();
	HxLOG_Print("*******************************************************************\n");
	HxLOG_Print("*******************************************************************\n");
	HxLOG_Print("*******************************************************************\n");
	/* Standby action manager�� ��ü */
	BUS_MGR_Destroy(0);

	//	stanbyaction ��ü.
//	BUS_MGR_Create(NULL, APP_ACTION_MGR_PRIORITY, PROC_StandbyActionMgr, 0, p1, p2, p3);

	return MESSAGE_BREAK; /* ��� ������ PROC_StandbyActionMgr()���� �������� ����... */
}
#endif

STATIC BUS_Result_t xmgr_am_MsgGwmTimer(HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR		err = ERR_FAIL;

	if(p1 == RECENTLIST_TIMER_ID)
	{ /* ���Ⱑ recent list�� ������ �߰��� �Ǵ� ������ */
		Handle_t	hSvc;

		if( (MGR_ACTION_GetType(s_ulRecentListActionId) == eViewType_WTV)
			|| (MGR_ACTION_GetType(s_ulRecentListActionId) == eViewType_ATV)
			|| (MGR_ACTION_GetType(s_ulRecentListActionId) == eViewType_TSR_PB)
			|| (MGR_ACTION_GetType(s_ulRecentListActionId) == eViewType_CHASE_PB)
		)
		{
			err = MGR_LIVE_CurrentSvcInfoGet(s_ulRecentListActionId, &hSvc);
			if(err == ERR_OK)
			{
				HxLOG_Print("[AP:AM(%d)] Register hSvc[0x%x] to recent list.\n",s_ulRecentListActionId, hSvc);
				DB_SVC_AddRecentService(hSvc);
			}
			else
			{
				HxLOG_Print("[AP:AM(%d)] Don't register recent list because current service handle is NOT VALID\n",s_ulRecentListActionId);
			}
		}
		else
		{
				HxLOG_Print("[AP:AM(%d)] Don't register recent list because current action type is [%s]\n",s_ulRecentListActionId,MGR_ACTION_GetTypeStr(MGR_ACTION_GetType(s_ulRecentListActionId)));
		}

		BUS_KillTimer(RECENTLIST_TIMER_ID);
	}
	else if(p1 == BACKGROUND_TASK_LAUNCH_TIMER)
	{
		BUS_KillTimer(BACKGROUND_TASK_LAUNCH_TIMER);
	}
#if defined(CONFIG_DEBUG_AGING_TEST)
	else if(p1 == BACKGROUND_AGING_LOG_TIMER_ID)
	{
		HCHAR	 szPath[HxPATH_MAX];
		HUINT8	 szCmd[HxPATH_MAX];
		HBOOL	 bFoundPath = FALSE;

		HxSTD_MemSet(szPath, 0, sizeof(szPath));

#if defined(CONFIG_MW_MM_PVR)
		{
			HUINT32  ulPvrIdx = 0xffff;
			
			ulPvrIdx = SVC_FS_GetDefaultPvrIdx();
			if (ulPvrIdx < MAX_CONNECTABLE_PVR_STORAGE_NUM)
			{
				err = SVC_FS_GetPvrPath(ulPvrIdx, eSVC_FS_UsageType_MediaVideo, szPath);
				bFoundPath = TRUE;
			}
		}
#endif

#if defined(CONFIG_PROD_DEVICE_STORAGE_USB) && !defined(CONFIG_MW_MM_PVR) && !defined(CONFIG_SUPPORT_UMMA)
		if(SVC_SYS_AgingLog_GetPath(szPath) == ERR_OK)
		{
			bFoundPath = TRUE;
		}
#else
		if ((bFoundPath == FALSE) && (SVC_FS_IsUsbConnected() == TRUE))
		{
			#define __filter (eDxSTORAGE_FILTER_TypeAll | eDxSTORAGE_FILTER_Connected | eDxSTORAGE_FILTER_Mounted | eDxSTORAGE_FILTER_UsageAll | eDxSTORAGE_FILTER_KindAll)

			DxStorage_Info_t	*pDevinfo = NULL, *pDevinfoList = NULL;
			HUINT32 			ulListCount = 0;

			err = UAPI_FS_GetList((HUINT32)__filter, &pDevinfoList, &ulListCount);
			if (err == ERR_OK && ulListCount > 0)
			{
				pDevinfo = &pDevinfoList[0];
				HxSTD_snprintf(szPath, HxPATH_MAX, "%s", pDevinfo->szMountPath);

				bFoundPath = TRUE;
			}
		}
#endif

		if (bFoundPath == FALSE)
		{
			err = ERR_FAIL;
			HxLOG_Error("Aging Information can't add...\n");
		}

		if (bFoundPath == TRUE && strlen(szPath))
		{
			HxSTD_snprintf(szCmd, HxPATH_MAX, "top -n 1 -m >> %s/topinfo.log", szPath);
			VK_SYSTEM_Command(szCmd);
			HxSTD_snprintf(szCmd, HxPATH_MAX, "cat /proc/meminfo >> %s/meminfo.log", szPath);
			VK_SYSTEM_Command(szCmd);

			HxLOG_Warning("Aging Information Added to [topinfo.log] and [meminfo.log]\n");
		}
	}
#endif

	return MESSAGE_BREAK;
}


STATIC BUS_Result_t xmgr_am_MsgWtvSvcChanged(HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if(MGR_ACTION_GetState() == eAmState_NORMAL)
	{
		 /* Recent list�� wtv, atv �� �ƴ϶� TSR play, chase play ������ ����Ǿ�� �Ѵ�.
			�׷��� action manager���� �������� �����Ѵ�. */
		if(ulActionId == MGR_ACTION_GetMainActionId())
		{
			BUS_KillTimer(RECENTLIST_TIMER_ID);
			BUS_SetTimer(RECENTLIST_TIMER_ID, RECENTLIST_ADD_TIMEOUT);

			/* ���߿� �� action ID�� ���� �о ����� ����. Main���� �����ؼ� sub�� �� �ִ� ��� ���... */
			s_ulRecentListActionId = ulActionId;
		}
	}

	return ERR_BUS_NO_ERROR;
}

STATIC BUS_Result_t xmgr_am_EvtDbCheckSvcDb(HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32			nSvcCnt;

	/* Service DB�� ����� ��쿡�� action manager�� service DB�� ��������� check ��. */
	nSvcCnt = MGR_SVCLIST_CountSvcInDB(MGR_SVCLIST_GetSvcTypeForListCount());

	if(nSvcCnt == 0)
	{
		xmgr_am_StartNoDbState();
	}
	else
	{
		xmgr_am_SetIsSvcDbState();
	}
	return ERR_BUS_NO_ERROR;
}

STATIC BUS_Result_t xmgr_am_EvtTotTdtReceived(HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if( (MGR_ACTION_GetType(ulActionId) == eViewType_ATV) ||
		(MGR_ACTION_GetType(ulActionId) == eViewType_WTV))
	{
#if defined(CONFIG_MW_CAS)
		MGR_CAS_TotTdtUpdated();
#endif
	}

	return ERR_BUS_NO_ERROR;
}

STATIC HBOOL xmgr_am_isUpdateTzCode_byTDT(void)
{
	HBOOL	bUpdateTz = TRUE;

#if defined(CONFIG_OP_NORDIG)
	// do not update TzCode when received TDT
	bUpdateTz = FALSE;
#endif

	return bUpdateTz;
}

STATIC BUS_Result_t xmgr_am_MakeTzFromKnownTzCode(HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3)
{
	static HUINT16 		_s_TzYears	=	1971;

	HERROR 				hErr;
	HxDATETIME_t		tempDate;
	unsigned long		utCurrTime;
	HxCountry_e 		eCountryId = eCountryID_NONE;
	HCHAR				szTzSend[TZ_NAME_LENGTH];
	HUINT32				ulRegionId = 0;


	hErr = VK_CLOCK_GetTime (&utCurrTime);
	if (VK_OK != hErr)		utCurrTime = 0;

	if (HLIB_DATETIME_ConvertUnixTimeToDateTimeWithOutOffset((UNIXTIME)utCurrTime, &tempDate) != ERR_OK)
	{
		return ERR_BUS_NO_ERROR;
	}

	if ( (_s_TzYears == tempDate.stDate.usYear) && (_s_TzYears != 1971 ) )		// �����ǿ����� �����ʿ䰡 ����.
	{
		return ERR_BUS_NO_ERROR;
	}

	// �������� ����� TZ �� ���� ��츦 �����ϰ� , �ϳ⿡ �ѹ�����.
	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_COUNTRY_ID, (HUINT32 *)&eCountryId, 0);
	if (ERR_OK != hErr)
	{
		HxLOG_Warning("DB_PARAM_GetItem(eDB_PARAM_ITEM_COUNTRY_ID) Error!!\n");
	}

	hErr = HAPI_GetRegistryInt(DxDB_APPL_TIME_REGION_ID, &ulRegionId);
	if (ERR_OK != hErr)
	{
		HxLOG_Warning("HAPI_GetRegistryInt(DxDB_APPL_TIME_REGION_ID) Error!!\n");
		ulRegionId = 0;
	}

	if (HLIB_DATETIME_MakeTzFileFromKnownTzCode (utCurrTime, eCountryId, ulRegionId, szTzSend) == ERR_OK)
	{
		HCHAR 	szTzString[TZ_NAME_LENGTH], system_command[TZ_NAME_LENGTH + 5];
		FILE 	*pfTzFile = fopen(TZ_NAME_PATH, "r");

		if (pfTzFile != NULL)
		{
			fgets(szTzString, TZ_NAME_LENGTH, pfTzFile);
			szTzString[HxSTD_StrLen(szTzString)-1] = '\0';

			if (HxSTD_StrCmp(szTzSend, szTzString) == 0)
			{
				// �̹� �����ϴ� TZ �� ����.
				fclose(pfTzFile);
				return ERR_BUS_NO_ERROR;
			}
			fclose(pfTzFile);
		}

		HxSTD_snprintf(system_command, 128, "%s%c", szTzSend, 0x0a);
		HLIB_FILE_WriteFile(TZ_NAME_PATH, system_command, HxSTD_StrLen(system_command));
		HLIB_FILE_Delete(TZ_NAME_PATH_TMP);
		HLIB_FILE_Copy(TZ_NAME_PATH, TZ_NAME_PATH_TMP);

#if defined(CONFIG_MW_SI_LOCALTIME)
		(void)SVC_SI_UpdateLocaltime_linkTzDB(eCountryId, ulRegionId);
#endif
		_s_TzYears	= tempDate.stDate.usYear;
	}

	return ERR_BUS_NO_ERROR;
}

STATIC BUS_Result_t xmgr_am_EvtSiServiceMove(HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3)
{
	Handle_t		 hOrgSvcHandle;
	HERROR			 hErr;

	// �� Service Handle�� Service Info�� �����Ѵ�.
	hOrgSvcHandle = (Handle_t)p1;

	hErr = DB_SVC_CheckValidServiceHandle(hOrgSvcHandle);
	if (hErr == ERR_OK)
	{
		// �� Service�� ���� : �����Ѵ�.
		DB_SVC_DeleteServiceInfo (hOrgSvcHandle);

		// Flash�� ���� �߰��� �ؾ� �Ѵ�.
		DB_SVC_RemoveSearchFlag ();
	}

	// Message�� �������� ó���ϴ� ���� Action Manager���� �Ѵ�.
	return MESSAGE_BREAK;
}


/************************************************************************************/
/********************	Application Proc.					*************************/
/************************************************************************************/

#define _____Module_Function_____

BUS_Result_t xproc_action_Normal_Base(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t	eGwmResult = ERR_BUS_NO_ERROR;
	HUINT32			ulActionId = SVC_PIPE_GetActionId(hAction);

	switch (message)
	{
		case eMEVT_BUS_CREATE :
			BUS_MGR_SetMgrName(BUS_MGR_GetThis(), "xproc_action_Normal_Base");
			eGwmResult = xmgr_am_MsgGwmCreate(ulActionId, p1, p2, p3);	// P1 is GoPeration
			break;

		case eMEVT_BUS_MGR_ALREADY_EXIST:
			if (p1 == (HINT32)BUS_MGR_GetMgrCallback(BUS_MGR_GetThis()))
			{
				// normal proc�� �ٽ� �����õ� �����Ƿ� create�� ���� �����ϰ� �ʱ�ȭ ���ش�
				eGwmResult = xmgr_am_MsgGwmCreate(ulActionId, 0, 0, 0);
			}
			break;

		case eMEVT_BUS_TIMER:
			eGwmResult = xmgr_am_MsgGwmTimer(ulActionId, p1, p2, p3);
			break;

		case eSEVT_DB_CHECK_SVC_DB :
			HxLOG_Print("MESSAGE : eSEVT_DB_CHECK_SVC_DB\n");
			eGwmResult = xmgr_am_EvtDbCheckSvcDb(ulActionId, p1, p2, p3);
			break;

		case eMEVT_LIVE_NOTIFY_SVC_CHANGED:
			HxLOG_Print("[AP:AM(%d)] MESSAGE : eMEVT_LIVE_NOTIFY_SVC_CHANGED\n",ulActionId);
			if(MGR_ACTION_GetState() == eAmState_NORMAL)
			{
				eGwmResult = xmgr_am_MsgWtvSvcChanged(ulActionId, p1, p2, p3);
			}
			break;

		case eSEVT_SI_SERVICE_MOVE :
			HxLOG_Print("[AP:AM(%d)] MESSAGE : eSEVT_SI_SERVICE_MOVE [%s]\n",ulActionId,MGR_ACTION_GetTypeStr((MgrAction_Type_e)p1));
			if(MGR_ACTION_GetState() == eAmState_NORMAL)
			{
				eGwmResult = xmgr_am_EvtSiServiceMove(ulActionId, p1, p2, p3);
			}
			break;

		case eMEVT_BUS_READY_SHUTDOWN :
			{ /* Normal���� ������ ��쿡�� ó��. IDLE���� ������ ��쿡�� booting�� �ٷ� �� ������ �� ó�� �ʿ����. */
#if defined(CONFIG_MW_MM_PVR)
				MGR_LIVE_PincodeDeleteAllServiceHandle(); /* pvr ���� pincode ���� ���� */
#endif
				DB_PARAM_Sync(); /* Sync setup DB */
				DAPI_Sync(DxDBSETUP_SYSTEM);

				//if ( p1 != TRUE )
				//{
				//	xmgr_action_StandbySetupAutoEMMUpdate();
				//}
			}
			eGwmResult = MESSAGE_PASS;
			break;

		case eMEVT_BUS_GO_OPERATION :
			eGwmResult = xmgr_am_MsgGwmGoOperation(hAction, p1, p2, p3);
			break;

		case eSEVT_SI_TDT :
			HxLOG_Print("MESSAGE : eSEVT_SI_TDT \n");
			if(xmgr_am_isUpdateTzCode_byTDT() == TRUE)
			{
				xmgr_am_MakeTzFromKnownTzCode(ulActionId, p1, p2, p3);
			}
			eGwmResult = xmgr_am_EvtTotTdtReceived(ulActionId, p1, p2, message);
			break;

		case eSEVT_SI_TOT :
			HxLOG_Print("MESSAGE : eSEVT_SI_TOT \n");
			SVC_SI_UpdateLiveTimeByTot(hAction);
			eGwmResult = xmgr_am_EvtTotTdtReceived(ulActionId, p1, p2, message);
			break;

		case eSEVT_SI_TIMEZONE_CHANGED:
			HxLOG_Print("MESSAGE : eSEVT_SI_TIMEZONE_CHANGED \n");
			eGwmResult = MESSAGE_PASS;
			break;

		case eMEVT_BUS_DESTROY :
			HxLOG_Print("\n\t********** xproc_action_Normal_Base() Destroyed. **********\n");
			eGwmResult = xmgr_am_MsgGwmDestroy(ulActionId, p1, p2, p3);
			return MESSAGE_BREAK;

		case eMEVT_STORAGE_NOTIFY_PLUGGED:
			break;

#if defined(CONFIG_MW_DOWNLOAD_HBBTV)
		case eSEVT_SI_PMT:
		case eSEVT_SI_PMT_PIDREMOVED:
		case eSEVT_DOWN_DOWNLOAD_SUCCESS:
		case eSEVT_DOWN_DOWNLOAD_CHANGED:
		case eSEVT_DOWN_DOWNLOAD_FAIL:
		case eSEVT_DOWN_DOWNLOAD_PROGRESS:
		case eSEVT_DOWN_DSMCC_EVENT_FOUND:
		case eSEVT_DOWN_DSMCC_STREAM_DESCRIPTOR_EVENT:
			// TODO: xmgr_dsmcc.c  �����ؼ� �Ű� ���´�.
			// ���� om_dsmcc.c ���� ó��. �ʿ��ϸ� xmgr_dsmcc.c ����.
			break;
#endif
	}

	if( (eGwmResult == MESSAGE_BREAK) || (eGwmResult == MESSAGE_PASS) )
	{
		return eGwmResult;
	}
	else
	{
		return BUS_ProcessMessageDefault(message, hAction, p1, p2, p3);
	}
}

HERROR xmgr_action_RegisterSiTimeoutCallback_Base(void)
{
	HERROR		hErr;

	hErr = SVC_CAS_SiRegisterTimeoutCallback((SvcCas_SiGetTimeOutCb_t)xmgr_action_GetSiTableTimeout);
	hErr |= SVC_DOWNLOAD_SiRegisterTimeoutCallback((SvcDownload_SiGetTimeOutCb_t)xmgr_action_GetSiTableTimeout);
	hErr |= SVC_EPG_SiRegisterTimeoutCallback((SvcDownload_SiGetTimeOutCb_t)xmgr_action_GetSiTableTimeout);

	return hErr;
}

/*********************** End of File ******************************/
