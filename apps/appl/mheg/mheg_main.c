/*************************************************************
 * @file		mheg_main.c
 * @date		2013/11/16
 * @brief		Humax SnT Mheg Player
 *
 * http://www.humaxdigital.com
 *************************************************************/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <hlib.h>
#include <vkernel.h>
#include <hapi.h>
#include <apk.h>

#include <directfb.h>
#include <mheg_int.h>
#include <mheg_port_itk.h>


/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define	MHEG_MSG_DATA_MAX		(128)
/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct
{
	HUINT32		ulDummy;
} mheginfo_t;

typedef struct
{
	HINT32		nMsg;
	HINT32		nViewId;
	HINT32		nParam1, nParam2, nParam3, nParam4;
	HUINT8 		aucData[MHEG_MSG_DATA_MAX];
	HUINT32		ulDataSize;
} MHEG_MSG_t;

/*******************************************************************/
/********************      Variables       *************************/
/*******************************************************************/
static HUINT32		s_ulMhegSemId;
static HUINT32		s_ulMhegQueueId;

/*******************************************************************/
/********************      Static Functions ProtoType    ********************/
/*******************************************************************/
static void	mheg_event_main(void *arg);
static HINT32	mheg_postMsgCallback(HINT32 nMsg, HINT32 nViewId, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3, HINT32 nParam4);
static HINT32	mheg_postMsgCallbackEx(HINT32 nMsg, HINT32 nViewId, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3, HINT32 nParam4, void *pData, HUINT32 ulDataSize);

/*******************************************************************/
/********************      Static Functions ProtoType    ********************/
/*******************************************************************/
static void mheg_lib_cbMediaPlayNotify( HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eType, OxMediaPlay_Event_e eEvent, OxMediaPlay_NotifierData_t *pstNotifyData, void *pvUserData )
{

	HxLOG_Debug("OxMediaPlay_Event_e (eEvent : 0x%x )  OxMediaPlay_MediaType_e(eType : %d )\n", eEvent, eType);

	switch (eEvent)
	{
		case eOxMP_EVENT_LIVE_SvcChanged:
			{
				switch( eType )
				{
					case eOxMP_MEDIATYPE_LIVE:
					{
						Handle_t	hSvcHandle;

						if (NULL != pstNotifyData)
						{
#if defined(CONFIG_TIMESTAMP_64)							
							hSvcHandle = (Handle_t)pstNotifyData->stEventNotify.ullParam1;
#else
							hSvcHandle = (Handle_t)pstNotifyData->stEventNotify.ulParam1;
#endif
						}
						else
						{
							APK_MEDIA_PLAY_GetRequestId (ulViewId, eOxMP_MEDIATYPE_LIVE, (HUINT32*)&hSvcHandle);
						}

						mheg_postMsgCallback(eMHEG_APK_SVCCHANGED, ulViewId, eType, (HINT32)hSvcHandle, 0, 0);
					}
					break;
					case eOxMP_MEDIATYPE_TSRPLAYBACK:
					case eOxMP_MEDIATYPE_PVRPLAYBACK:
					case eOxMP_MEDIATYPE_MEDIAPLAY:
					default :
						break;
				}
			}
			break;
		case eOxMP_EVENT_LIVE_SvcStopped:
			mheg_postMsgCallback(eMHEG_APK_SVCSTOPPED, ulViewId, ulSessionId, eType, 0, 0);
			break;
		case eOxMP_EVENT_AV_STARTED:
			mheg_postMsgCallback(eMHEG_APK_AVSTARTED, ulViewId, ulSessionId, eType, 0, 0);
			break;
		case eOxMP_EVENT_AV_STOPPED:
			mheg_postMsgCallback(eMHEG_APK_AVSTOPPED, ulViewId, ulSessionId, eType, 0, 0);
			break;
		case eOxMP_EVENT_MHEG_MON_STARTED:
			mheg_postMsgCallback(eMHEG_APK_MONITORSTARTED, ulViewId, ulSessionId, eType, 0, 0);
			break;
		case eOxMP_EVENT_MHEG_MON_STOPPED:
			mheg_postMsgCallback(eMHEG_APK_MONITORSTOPPED, ulViewId, ulSessionId, eType, 0, 0);
			break;
		case eOxMP_EVENT_MHEG_PMT_RECEIVED:
#if defined(CONFIG_TIMESTAMP_64)
			mheg_postMsgCallback(eMHEG_APK_PMTCHANGED, ulViewId, ulSessionId, eType, (HINT32)pstNotifyData->stEventNotify.ullParam3, 0);
#else
			mheg_postMsgCallback(eMHEG_APK_PMTCHANGED, ulViewId, ulSessionId, eType, (HINT32)pstNotifyData->stEventNotify.ulParam3, 0);
#endif
			break;
		case eOxMP_EVENT_MHEG_DMX_CHANGED:
#if defined(CONFIG_TIMESTAMP_64)			
			mheg_postMsgCallback(eMHEG_APK_DMXCHANGED, ulViewId, ulSessionId, eType, (HINT32)pstNotifyData->stEventNotify.ullParam2, 0);
#else
			mheg_postMsgCallback(eMHEG_APK_DMXCHANGED, ulViewId, ulSessionId, eType, (HINT32)pstNotifyData->stEventNotify.ulParam2, 0);
#endif
			break;
		case eOxMP_EVENT_TUNE_NoSignal:
			mheg_postMsgCallback(eMHEG_APK_NOSIGNAL, ulViewId, ulSessionId, eType, 0, 0);
			break;
		case eOxMP_EVENT_TUNE_Locked:
			mheg_postMsgCallback(eMHEG_APK_LOCKED, ulViewId, ulSessionId, eType, 0, 0);
			break;
		case eOxMP_EVENT_COMPONENT_SUBTITLE_CHANGED:
			mheg_postMsgCallback(eMHEG_APK_SUBTITLECHANGED, ulViewId, ulSessionId, eType, 0, 0);
			break;
		case eOxMP_EVENT_AUDIOCLIPCOMPLETED:
			mheg_postMsgCallback(eMHEG_APK_AUDIOCLIPCOMPLETED, ulViewId, ulSessionId, eType, 0, 0);
			break;
#if defined (CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
		case eOxMP_EVENT_STARTED:
			mheg_postMsgCallback(eMHEG_APK_MEDIA_EVENT_STARTED, ulViewId, ulSessionId, eType, 0, 0);
			break;
		case eOxMP_EVENT_PLAYTIME:
			mheg_postMsgCallback(eMHEG_APK_MEDIA_EVENT_PLAYTIME, ulViewId, ulSessionId, eType, 0, 0);
			break;
		case eOxMP_EVENT_PLAYSTATE:
			mheg_postMsgCallback(eMHEG_APK_MEDIA_EVENT_PLAYSTATE, ulViewId, ulSessionId, eType, 0, 0);
			break;
		case eOxMP_EVENT_PLAYERROR:
			mheg_postMsgCallback(eMHEG_APK_MEDIA_EVENT_PLAYERROR, ulViewId, ulSessionId, eType, 0, 0);
			break;
		case eOxMP_EVENT_MEDIA_BufferStateChanged:
			mheg_postMsgCallback(eMHEG_APK_MEDIA_EVENT_MEDIA_BufferStateChanged, ulViewId, ulSessionId, eType, 0, 0);
			break;
		case eOxMP_EVENT_MEDIA_BufferInfoChanged:
			mheg_postMsgCallback(eMHEG_APK_MEDIA_EVENT_MEDIA_BufferInfoChanged, ulViewId, ulSessionId, eType, 0, 0);
			break;
		case eOxMP_EVENT_VIDEOSTREAMINFO:
			mheg_postMsgCallback(eMHEG_APK_MEDIA_EVENT_VIDEOSTREAMINFO, ulViewId, ulSessionId, eType, 0, 0);
			break;
		case eOxMP_EVENT_STOPPED:
			mheg_postMsgCallback(eMHEG_APK_MEDIA_EVENT_STOPPED, ulViewId, ulSessionId, eType, 0, 0);
			break;
		case eOxMP_EVENT_SPEED_CHANGED:
			mheg_postMsgCallback(eMHEG_APK_MEDIA_EVENT_SPEED_CHANGED, ulViewId, ulSessionId, eType, 0, 0);
			break;
		case eOxMP_EVENT_SupportedSpeedChanged:
			mheg_postMsgCallback(eMHEG_APK_MEDIA_EVENT_SupportedSpeedChanged, ulViewId, ulSessionId, eType, 0, 0);
			break;
		case eOxMP_EVENT_MEDIA_PlayInfo:
			mheg_postMsgCallback(eMHEG_APK_MEDIA_EVENT_PlayInfo, ulViewId, ulSessionId, eType, 0, 0);
			break;
		case eOxMP_EVENT_MEDIA_PumpIndexChanged:
			mheg_postMsgCallback(eMHEG_APK_MEDIA_EVENT_MEDIA_PumpIndexChanged, ulViewId, ulSessionId, eType, 0, 0);
			break;
		case eOxMP_EVENT_MEDIA_SeekReady:
			mheg_postMsgCallback(eMHEG_APK_MEDIA_EVENT_MEDIA_SeekReady, ulViewId, ulSessionId, eType, 0, 0);
			break;
		case eOxMP_EVENT_AV_RESUME:
			mheg_postMsgCallback(eMHEG_APK_AVRESUME, ulViewId, ulSessionId, eType, 0, 0);
			break;
		case eOxMP_EVENT_AV_UNDERRUN:
			mheg_postMsgCallback(eMHEG_APK_AVUNDERRUN, ulViewId, ulSessionId, eType, 0, 0);
			break;
		case eOxMP_EVENT_ResultPincode:
#if defined(CONFIG_TIMESTAMP_64)
			mheg_postMsgCallback(eMHEG_APK_PFG_RESULT, ulViewId, (HINT32)pstNotifyData->stEventNotify.ullParam1, 0, 0, 0);
#else
			mheg_postMsgCallback(eMHEG_APK_PFG_RESULT, ulViewId, (HINT32)pstNotifyData->stEventNotify.ulParam1, 0, 0, 0);
#endif
			break;
#endif
		case eOxMP_EVENT_SI_RCT_REQUEST_IMAEG_ICON:
			{
				switch( eType )
				{
					case eOxMP_MEDIATYPE_LIVE:
					{
						if (NULL != pstNotifyData)
						{
							HUINT8 *pszImageUrl = NULL;
#if defined(CONFIG_TIMESTAMP_64)
							pszImageUrl = (HUINT8*)pstNotifyData->stEventNotify.ullParam1;
#else
							pszImageUrl = (HUINT8*)pstNotifyData->stEventNotify.ulParam1;						
#endif

							HxLOG_Debug("eOxMP_EVENT_SI_RCT_REQUEST_IMAEG_ICON : pszImageUrl(%s) \n",pszImageUrl);

							mheg_postMsgCallback(eMHEG_APK_DSMCCFS_REQUEST_FILE, ulViewId, ulSessionId, eType, (HUINT32) pszImageUrl,0);
						}
					}
						break;
					default:
						HxLOG_Debug("eOxMP_EVENT_SI_RCT_REQUEST_IMAEG_ICON :: Not Matched the eType \n");
						break;
				}
			}
			break;
		case	eOxMP_EVENT_SI_RCT_CHANGED:
			if (NULL != pstNotifyData)
			{
				if(eType == eOxMP_MEDIATYPE_LIVE)
				{
#if defined(CONFIG_TIMESTAMP_64)					
					mheg_postMsgCallback(eMHEG_APK_RCTCHANGED, ulViewId, (HINT32)pstNotifyData->stEventNotify.ullParam1, 0, 0, 0);
#else
					mheg_postMsgCallback(eMHEG_APK_RCTCHANGED, ulViewId, (HINT32)pstNotifyData->stEventNotify.ulParam1, 0, 0, 0);
#endif
				}
			}
			break;
		default:
			HxLOG_Debug("Not Matched the OxMediaPlay_Event_e \n");
			break;
	}
}

static void mheg_lib_cbDownloadNotify(HUINT32 ulDownloadId,
												DxDownload_Type_e eType,
												DxDownload_Event_e eEvent,
												DxDownload_NotifierData_t *punNotifyData,
												void *pvUserData)
{
	HxLOG_Debug("OxDownload_Event_e -> eEvent : 0x%x \n", eEvent);

	switch (eEvent)
	{
		case eDxDNLD_EVENT_Started:
			mheg_postMsgCallback(eMHEG_AVSTREAM_DOWNLOAD_START, 0, ulDownloadId, punNotifyData->stDnldSessionInfo.ulSessionId, 0, 0);
			break;
		case eDxDNLD_EVENT_Completed:
			mheg_postMsgCallback(eMHEG_AVSTREAM_DOWNLOAD_COMPLETE, 0, ulDownloadId, 0, 0, 0);
			break;
		case eDxDNLD_EVENT_Failed:
			HxLOG_Debug("eDxDNLD_EVENT_Failed is not Implemented \n");
			break;
		case eDxDNLD_EVENT_DataReceiving:
			HxLOG_Debug("eDxDNLD_EVENT_DataReceiving is is not Implemented \n");
			break;
		case eDxDNLD_EVENT_ProbeSuccess:
#if defined(CONFIG_TIMESTAMP_64)				
			mheg_postMsgCallback(eMHEG_AVSTREAM_DOWNLOAD_PROBE_SUCCESS, 0, ulDownloadId, punNotifyData->stDnldSessionInfo.ulSessionId, (HINT32)punNotifyData->stDnldSessionInfo.ullDuration, punNotifyData->stDnldSessionInfo.ulMaxBitrate);
#else
			mheg_postMsgCallback(eMHEG_AVSTREAM_DOWNLOAD_PROBE_SUCCESS, 0, ulDownloadId, punNotifyData->stDnldSessionInfo.ulSessionId, (HINT32)punNotifyData->stDnldSessionInfo.ulDuration, punNotifyData->stDnldSessionInfo.ulMaxBitrate);
#endif
			break;
		case eDxDNLD_EVENT_ProbeFailed:
			mheg_postMsgCallback(eMHEG_AVSTREAM_DOWNLOAD_PROBE_FAILED, 0, ulDownloadId, punNotifyData->stDnldSessionInfo.ulErrorReason, 0, 0);
			break;
		case eDxDNLD_EVENT_DownloadInfoChanged:
			mheg_postMsgCallback(eMHEG_AVSTREAM_DOWNLOAD_INFO_CHANGED, 0, ulDownloadId, 0 , 0, 0);
			break;
		default:
			HxLOG_Debug("Not Matched the OxDownload_Event_e \n");
			break;
	}

}


// Emit a event when the cable link is connected or disconnected
static void mheg_lib_cbNetworkNotify( HINT32 eEvent, HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3)
{

	switch ( eEvent )
	{
		case eNETWORK_STATUS_CABLE_LINK_CONNECTED :
		case eNETWORK_STATUS_CABLE_LINK_DISCONNECTED :
		case eNETWORK_STATUS_CON_SUCCESS:
		case eNETWORK_STATUS_CON_FAIL:
		case eNETWORK_STATUS_LINK_UPDATED:
		case eNETWORK_STATUS_WIFI_AP_CONNECT_SUCCESS:
		case eNETWORK_STATUS_WIFI_AP_CONNECT_FAIL:
		case eNETWORK_STATUS_WIFI_WPS_SUCCESS:
		case eNETWORK_STATUS_WIFI_WPS_FAIL:
		case eNETWORK_STATUS_WIFI_AP_DISCONNECTED:
		case eNETWORK_STATUS_WIFI_AP_RECONNECTED:
			HxLOG_Debug("Received eMHEG_NETWORK_EVENT event!!\r\n");
			mheg_postMsgCallback(eMHEG_NETWORK_EVENT,  0, 0, 0, 0, 0);
			break;
		default :
			HxLOG_Debug("Received other network events!!\r\n");
			return ;
	}

}


static void	mheg_lib_cbMetaSvcUpdated(HINT32 event, const HINT32 *args, void *userdata)
{
	switch (event)
	{
		case 0:			//	dynamic update
			mheg_postMsgCallback(eMHEG_APK_SVCLISTUPDATED, 0, 0, 0, 0, 0);
			break;
		default :
			HxLOG_Error("[%s:%d] unknown event\r\n", __FUNCTION__, __LINE__);
			break;
	}
}

static void mheg_lib_cbFactoryDefaultNotify(HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3)
{
	mheg_postMsgCallback(eMHEG_APK_FACTORY_DEFAULT, 0, 0, 0, 0, 0);
}

static HERROR	mheg_lib_cbMetaSvcLoad(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	HxLOG_Assert(nArgc == 1);

	APK_META_SVC_LoadupdatedList();

	return ERR_OK;
}

static void		mheg_lib_initializeAppkit(void)
{
	HUINT32		ulViewId;
	HERROR		hErr;

	APK_META_SVC_Init(FALSE);
	APK_META_SVC_SetListener(mheg_lib_cbMetaSvcUpdated, NULL);

	APK_MEDIA_PLAY_Init();
	APK_MEDIA_PLAY_GetMainViewId(&ulViewId);
	hErr = APK_MEDIA_PLAY_RegisterNotifier(ulViewId, mheg_lib_cbMediaPlayNotify, NULL, NULL);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("APK_MEDIA_PLAY_RegisterNotifier(mheg_lib_cbMediaPlayNotify) Error \n");
	}

	APK_SYSINFO_STB_Init();
	APK_OUTPUT_VIDEO_Init();
	APK_OUTPUT_AUDIO_Init();
	APK_CAS_CI_Init();

	APK_DOWNLOAD_Init();
	hErr = APK_DOWNLOAD_RegisterNotifier(mheg_lib_cbDownloadNotify, NULL);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("APK_DOWNLOAD_RegisterNotifier(mheg_lib_cbDownloadNotify) Error \n");
	}

	APK_NETWORK_Init();
	APK_NETWORK_RegisterNotifier(mheg_lib_cbNetworkNotify);

	hErr =APK_EXE_FACTORYDEFAULT_RegisterNotifier((APK_EXE_FACTORYDEFAULT_Notifier_t)mheg_lib_cbFactoryDefaultNotify);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("APK_EXE_FACTORYDEFAULT_RegisterNotifier((APK_EXE_FACTORYDEFAULT_Notifier_t)mheg_lib_cbFactoryDefaultNotify) Error\n");
	}

	HAPI_ConnectSignal("signal:onLoadServiceListToMheg",	NULL, mheg_lib_cbMetaSvcLoad);
}

static mheginfo_t*		mheg_getInstance(void)
{
	static mheginfo_t	*s_pstInfo = NULL;

	if (s_pstInfo == NULL)
	{
		unsigned long	tid;

		s_pstInfo = HLIB_STD_MemCalloc(sizeof(mheginfo_t));

		HxSEMT_Create(&s_ulMhegSemId, "mhegsem", HxSEMT_FIFO);
		HxSEMT_Get(s_ulMhegSemId);

		HAPI_Init();

		mheg_lib_initializeAppkit();

		//	TODO: APK_/*Module*/_Init....
		VK_TASK_Create(mheg_event_main, 70, SIZE_128K, "mhegevent", NULL, &tid, 0); /* EXCEPTIONS(vkernel)가 발생하여, 64K에서 128로 늘림 (stack overflow?) */
		VK_TASK_Start(tid);

		HLIB_MSG_Create(128, sizeof(MHEG_MSG_t), "mhegmsg", (HULONG*)&s_ulMhegQueueId, eHLIB_SUSPENDTYPE_FIFO);

	}
	else
	{
		HxSEMT_Get(s_ulMhegSemId);
	}

	return s_pstInfo;
}

void	mheg_releaseInstance(mheginfo_t *info)
{
	HxSEMT_Release(s_ulMhegSemId);
}

static HINT32	mheg_postMsgCallback(HINT32 nMsg, HINT32 nViewId, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3, HINT32 nParam4)
{
	return mheg_postMsgCallbackEx(nMsg, nViewId, nParam1, nParam2, nParam3, nParam4, NULL, 0);
}

static HINT32	mheg_postMsgCallbackEx(HINT32 nMsg, HINT32 nViewId, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3, HINT32 nParam4, void *pData, HUINT32 ulDataSize)
{
	MHEG_MSG_t	stMsg;

	memset(&stMsg, 0, sizeof(MHEG_MSG_t));
	stMsg.nMsg    = nMsg;
	stMsg.nViewId = nViewId;
	stMsg.nParam1 = nParam1;
	stMsg.nParam2 = nParam2;
	stMsg.nParam3 = nParam3;
	stMsg.nParam4 = nParam4;
	if  ((pData != NULL) && (ulDataSize != 0) && (ulDataSize < MHEG_MSG_DATA_MAX))
	{
		HxSTD_memcpy(stMsg.aucData, pData, ulDataSize);
		stMsg.ulDataSize = ulDataSize;
	}

	return HLIB_MSG_Send(s_ulMhegQueueId, (const void*)&stMsg, sizeof(MHEG_MSG_t));
}

static void		mheg_event_main(void *arg)
{
	MHEG_MSG_t	stMsg;

	if (MHEG_PORT_Init(mheg_postMsgCallbackEx) != ERR_OK)
	{
		HxLOG_Error("fail to initialize mheg library...\n");
		HxLOG_Assert(0);
	}

	HLIB_EXTRA_DropRootPrivileges("mheg");

	while (1)
	{
		while (APK_EVENT_Dispatch());

		if (HLIB_MSG_ReceiveTimeout(s_ulMhegQueueId, &stMsg, sizeof(MHEG_MSG_t), 100) == ERR_OK)
		{
			if (MHEG_PORT_ProcessMsg(stMsg.nMsg, stMsg.nViewId, stMsg.nParam1, stMsg.nParam2, stMsg.nParam3, stMsg.nParam4, stMsg.aucData, stMsg.ulDataSize) != ERR_OK)
			{
				//	TODO: process other message
			}
		}
	}
}

int		main(int argc, char *argv[])
{
	VK_Init();
#ifdef	CONFIG_DEBUG
	HLIB_DBG_Init();
#endif
	mheg_releaseInstance(mheg_getInstance());

#ifdef	CONFIG_DEBUG
	//	TODO: add command tool for mheg play.
#endif

	while (1)
		HLIB_STD_TaskSleep(360000);
}


HINT32		MHEG_SendMessage(HINT32 nMsg, HINT32 nViewId, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3, HINT32 nParam4)
{
	return	mheg_postMsgCallback( nMsg,  nViewId,  nParam1,  nParam2,  nParam3,  nParam4);
}


