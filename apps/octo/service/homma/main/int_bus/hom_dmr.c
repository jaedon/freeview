/*************************************************************************************************************
	File            : hom_dmr.c
	author          : IPM tipmsw1@humaxdigital.com
	comment         :
	date            :
	attention       :

	(c) 2011-2012 Humax Co., Ltd.
	This program is produced by Humax Co., Ltd. (Humax) and
	the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
	non-assignable, non-transferable and non-exclusive license to use this Software.
	You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
	you agree to the responsibility to take all reasonable efforts to protect the any information
	you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
	reverse engineer or extract the source code of this Software unless you have Humax's written permission
	to do so.
	If you have no authorized license, discontinue using this Software immediately.

	THE SOFTWARE IS PROVIDED ¡°AS IS¡± AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES,
	EITHER EXPRESS, IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
	IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR
	INDIRECT, SPECIAL, CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR
	INABILITY TO USE THE SOFTWARE.

	This License is effective until terminated. You may terminate this License at any time
	by destroying all copies of the Software including all documentation. This License will terminate
	immediately without notice from Humax to you if you fail to comply with any provision of this License.
	Upon termination, you must destroy all copies of the Software and all documentation.

	The laws of the Republic of Korea will apply to any disputes arising out of or relating to
	this Copyright Notice. All claims arising out of or relating to this Copyright Notice will be litigated
	in the Seoul Central District Court, in the Republic of Korea.
*************************************************************************************************************/

#define	________________Header_Files_______________________________
#include 	<vkernel.h>
#include 	<hlib.h>
#include 	<oapi.h>

#include 	"common.h"
#include 	"hom_int.h"
#include 	"homapi.h"
#include 	"homma_int.h"
#include 	"homma_bus.h"
#include 	"homma_ipc_decode.h"
#include 	"dmr.h"
#include 	"dms.h"

#define		DMR_PREFIX_FILE			"file:/"
#define		DMR_PREFIX_CHANNEL		"dvb://ccid:"
#define 	DMR_PREFIX_HDVB			"hdvb://ccid:" /* Allow "hdvb://ccid:", not "hdvb://" */
#define		DMR_PREFIX_DLNA			"hdlna://"
#define 	DMR_PREFIX_RECODINGS	"hpvr://"

#define ________________Typedef____________________________________
typedef struct
{
	HBOOL				bInited;
	BUS_Handle			hMsgBusHandle;
	HOMMA_BUS_Target_e 	eSelf;

	HBOOL				bMonitorNetwork;
	HBOOL				bPrevStartFailed;
	HBOOL				bRequestStart;
	HCHAR 				*pszFriendlyName;

	HBOOL				bMediaPlayInited;
	HBOOL				bMediaPlayCbRegistered;
} HOM_DMR_Context_t;

#define ________________Static_Variables___________________________
static HOM_DMR_Context_t s_stHomDmrContext = { 0 };

#define ________________Private_Static_Variables___________________
static BUS_Handle s_stBusHnd = NULL;

static HOM_DMR_Context_t * __hom_dmr_getContext( void );

#define ________________Internal_Functions_________________________
static void		__hom_dmr_upnpActionCallback(HINT32 p1, HINT32 p2, HINT32 p3);

static	void	__hom_dmr_init( HOM_DMR_Context_t *pstHomDmrCtx)
{
	if (pstHomDmrCtx != NULL)
	{
		pstHomDmrCtx->eSelf = eBUS_TARGET_MOD_DMR;

		/* monitoring storage */
		pstHomDmrCtx->bMonitorNetwork  = TRUE;
		pstHomDmrCtx->bPrevStartFailed = FALSE;
		pstHomDmrCtx->bRequestStart    = FALSE;
		pstHomDmrCtx->pszFriendlyName  = NULL;
		pstHomDmrCtx->bInited          = TRUE;
		pstHomDmrCtx->bMediaPlayInited = FALSE;
		pstHomDmrCtx->bMediaPlayCbRegistered = FALSE;
	}
}

static HOM_DMR_Context_t * __hom_dmr_getContext( void )
{
	return &s_stHomDmrContext;
}

static HINT32	__hom_dmr_handleSystemEvent( HOMMA_BUS_Event_e eEvent, HOMMA_BUS_DataType_e eDataType, void *pvData )
{
#if defined(CONFIG_HOMMA_DMR)
	HERROR	hError = ERR_OK;
	HOM_DMR_Context_t *pHomDmrCtx = __hom_dmr_getContext();
#endif
	HxLOG_Error("++[system event:%d]\n", eEvent);

#if defined(CONFIG_HOMMA_DMR)
	if (pHomDmrCtx->bMonitorNetwork == TRUE)
	{
		if (pHomDmrCtx->bRequestStart == TRUE &&
			pHomDmrCtx->bPrevStartFailed == TRUE &&
			(eEvent == eBUS_EVENT_SYS_NET_IF_ONLINE ||
			eEvent == eBUS_EVENT_SYS_NET_IF_SWITCHING ||
			eEvent ==eBUS_EVENT_SYS_POWER_OPERATION))
		{
			if (DMR_IsStart() == FALSE)
			{
				HxLOG_Debug("[%s:%d] Info> Retry DMR start() because of prev failure(%d)!\n", __func__, __LINE__, pHomDmrCtx->bPrevStartFailed);
				HxLOG_Error("[ HOM_DMR ] ===========> REQ DMR START : %s (prev:%d) ==============\n", (HCHAR *)pHomDmrCtx->pszFriendlyName, pHomDmrCtx->bPrevStartFailed);

				/* 2016/11/3 mhkang: http://svn.humaxdigital.com:3000/issues/112911
				  In order to send a event of dmr to appkit, DMR_RegisterUPnPActionCallback must be called before DMR_Start.
				*/
				DMR_ModuleInit();
				DMR_RegisterUPnPActionCallback(__hom_dmr_upnpActionCallback);

				if (HxSTD_StrEmpty(pHomDmrCtx->pszFriendlyName))
				{
					hError = DMR_Start(NULL);
				}
				else
				{
					hError = DMR_Start(pHomDmrCtx->pszFriendlyName);
				}

				if (hError == ERR_OK)
				{
					pHomDmrCtx->bPrevStartFailed = FALSE;
				}
				else
				{
					HxLOG_Error("[ HOM_DMR ] Error> failed to re-start DMR!\n");
				}
			}
			return ERR_OK; /* event 처리없이 return */
		}

		// 2014/11/14 mhkang: refer to http://svn:3000/issues/97480
		if(eEvent == eBUS_EVENT_SYS_POWER_SHUTDOWN /*|| pHomDmrCtx->bRequestStart == TRUE*/)
			if(DMR_IsStart() == TRUE)
				pHomDmrCtx->bPrevStartFailed = TRUE;
	}
	else
	{
		HxLOG_Error("[ HOM_DMR ] Info> not processed[event:%d, type:%d]\n", eEvent, eDataType);
	}
	return DMR_OnSystemEvent( eEvent, (void *)eDataType, pvData );
#else
	return ERR_OK;
#endif
}

#if defined(CONFIG_HOMMA_DMR)
static HERROR	__hom_dmr_register_media_callback(void)
{
	HOMMA_BUS_Msg_t busMsg;

	HxSTD_MemSet(&busMsg, 0x00, sizeof(HOMMA_BUS_Msg_t));

	busMsg.uEnum.eEvt = eBUS_EVENT_DMR_REGISTER_MEDIA_CALLBACK;

	HOMMA_BUS_SendMsg( eBUS_TARGET_MOD_DMR, s_stBusHnd, &busMsg );

	return ERR_OK;
}

static HERROR	__hom_dmr_unregister_media_callback(void)
{
	HOMMA_BUS_Msg_t busMsg;

	HxSTD_MemSet(&busMsg, 0x00, sizeof(HOMMA_BUS_Msg_t));

	busMsg.uEnum.eEvt = eBUS_EVENT_DMR_UNREGISTER_MEDIA_CALLBACK;

	HOMMA_BUS_SendMsg( eBUS_TARGET_MOD_DMR, s_stBusHnd, &busMsg );

	return ERR_OK;
}

static void __hom_dmr_media_info_notifier( HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eType, OxMediaPlay_Event_e eEvent, OxMediaPlay_NotifierData_t *pstNotifyData, void *pvUserData )
{
	OxMediaPlay_MediaType_e	eMediaType = eOxMP_MEDIATYPE_NONE;

	if (0) /* Commented to prevent unnecessary OAPI IPC consumption because eMediaType is unused */
	{
		HOMMA_INTEROP_MEDIA_PLAY_GetPlayType (ulViewId, &eMediaType);
	}

	switch (eEvent)
	{
		case eOxMP_EVENT_PLAYTIME:
			{
#ifdef CONFIG_TIMESTAMP_64
				HUINT64 ulPosition = 0, ulDuration = 0;
#else
				HUINT32 ulPosition = 0, ulDuration = 0;
#endif

				OAPI_MEDIA_PLAY_GetPlayPosition (ulViewId, &ulPosition);
				OAPI_MEDIA_PLAY_GetDurationTime (ulViewId, &ulDuration);

				HxLOG_Debug("### MEDIA PLAYING TIME CHANGED (%d/%d)  ### \n", ulPosition, ulDuration);
				DMR_SetMediaPlayingPosition(ulPosition);
			}
			break;

		case eOxMP_EVENT_STOPPED:
			{
				DMR_UPnPState_e eState = 0;
				HxLOG_Debug("eOxMP_EVENT_STOPPED\n");
				DMR_GetUPnPStatus(&eState);
				if(eState != eDMR_UPnPState_Stopped)
				{
					DMR_SetUPnPStatus(eDMR_UPnPState_Stopped, NULL);
					__hom_dmr_unregister_media_callback();
				}
			}
			break;

		case  eOxMP_EVENT_PLAYSTATE:
			{
				OxMediaPlay_PlayState_e eState = eOxMP_PLAYSTATE_NONE;
				HOMMA_INTEROP_MEDIA_PLAY_GetPlayState (ulViewId, &eState);
				HxLOG_Debug("eOxMP_EVENT_PLAYSTATE (%d)\n", eState);
				switch(eState)
				{
					case eOxMP_PLAYSTATE_PAUSE:
						{
							DMR_SetUPnPStatus(eDMR_UPnPState_Paused, NULL);
							HxLOG_Debug("eOxMP_PLAYSTATE_PAUSE (%d)\n", eState);
						}
						break;

					case eOxMP_PLAYSTATE_STOPPING:
						HxLOG_Debug("eOxMP_PLAYSTATE_STOPPING (%d)\n", eState);
						break;

					case eOxMP_PLAYSTATE_STOPPED:
						{
							DMR_SetUPnPStatus(eDMR_UPnPState_Stopped, NULL);
							HxLOG_Debug("eOxMP_PLAYSTATE_STOPPED (%d)\n", eState);
						}
						break;

					case eOxMP_PLAYSTATE_REQUESTED:
						HxLOG_Debug("eOxMP_PLAYSTATE_REQUESTED (%d)\n", eState);
						break;

					case eOxMP_PLAYSTATE_CONNECTING:
						HxLOG_Debug("eOxMP_PLAYSTATE_CONNECTING(%d)\n", eState);
						break;

					case eOxMP_PLAYSTATE_BUFFERING:
						HxLOG_Debug("eOxMP_PLAYSTATE_BUFFERING(%d)\n", eState);
						break;

					case eOxMP_PLAYSTATE_PLAYING:
						{
#ifdef CONFIG_TIMESTAMP_64
							HUINT64 ulPosition = 0, ulDuration = 0;
#else
							HUINT32 ulPosition = 0, ulDuration = 0;
#endif
							HUINT64 ullCurrentDurationTime = 0;

							OAPI_MEDIA_PLAY_GetPlayPosition (ulViewId, &ulPosition);
							OAPI_MEDIA_PLAY_GetDurationTime (ulViewId, &ulDuration);

							ullCurrentDurationTime = ulDuration/1000;
							DMR_SetUPnPStatus( eDMR_UPnPState_CurrentTrackDuration, ( void * )&ullCurrentDurationTime);

							DMR_SetMediaPlayingPosition(0);
							DMR_SetMediaDuration(ulDuration);
							DMR_SetUPnPStatus(eDMR_UPnPState_Playing, NULL);

							HxLOG_Debug("eOxMP_PLAYSTATE_PLAYING(%d)\n", eState);
						}
						break;

					default:
						break;
				}
			}
			break;

		default :
			{
				HxLOG_Debug("Not need event. (%d)\n", eEvent );
			}
			return;
	}
}

#ifdef HOMMA_USE_APPKIT_DIRECTLY
#else
static void __hom_dmr_media_play_activated(void)
{
	HOM_DMR_Context_t *ctx = __hom_dmr_getContext();

	if(ctx == NULL)
	{
		HxLOG_Error("[__hom_dmr_media_play_activated] ctx is NULL!!\n");
		return;
	}

	ctx->bMediaPlayInited = TRUE;
}
#endif

static HERROR __hom_dmr_handleInternalEvent( HOMMA_BUS_Event_e eEvent, HOMMA_BUS_DataType_e eDataType, void *pvData )
{
	HOM_DMR_Context_t *ctx = __hom_dmr_getContext();
	HERROR	hErr = ERR_FAIL;
	HUINT32 ulViewId = 0;

	if(ctx == NULL)
	{
		HxLOG_Error("[__hom_dmr_handleInternalEvent] ctx is NULL!!\n");
		return ERR_FAIL;
	}

	if(eEvent == eBUS_EVENT_DMR_REGISTER_MEDIA_CALLBACK)
	{
		if(ctx->bMediaPlayInited == FALSE)
		{
			HxLOG_Error("[__hom_dmr_handleInternalEvent] Media play module is not yet initiated!!\n");
			return ERR_FAIL;
		}

		if(ctx->bMediaPlayCbRegistered == FALSE)
		{
			HOMMA_INTEROP_MEDIA_PLAY_GetMainViewId(&ulViewId);
			hErr = HOMMA_INTEROP_MEDIA_PLAY_RegisterNotifier(ulViewId, __hom_dmr_media_info_notifier, NULL);
			if(hErr == ERR_OK)
			{
				HxLOG_Debug("[__hom_dmr_handleInternalEvent] HOMMA_INTEROP_MEDIA_PLAY_RegisterNotifier!!\n");
				ctx->bMediaPlayCbRegistered = TRUE;
			}
			else
			{
				HxLOG_Error("HOMMA_INTEROP_MEDIA_PLAY_RegisterNotifier failed(Err:%d)\n", hErr);
				return ERR_FAIL;
			}
		}
	}
	else if(eEvent == eBUS_EVENT_DMR_UNREGISTER_MEDIA_CALLBACK)
	{
		if(ctx->bMediaPlayInited == FALSE)
		{
			HxLOG_Error("[__hom_dmr_handleInternalEvent] Media play module is not yet initiated!!\n");
			return ERR_FAIL;
		}

		if(ctx->bMediaPlayCbRegistered == TRUE)
		{
			HOMMA_INTEROP_MEDIA_PLAY_GetMainViewId(&ulViewId);
			hErr = HOMMA_INTEROP_MEDIA_PLAY_UnregisterNotifier(ulViewId, __hom_dmr_media_info_notifier, NULL);
			if(hErr == ERR_OK)
			{
				HxLOG_Debug("[__hom_dmr_handleInternalEvent] HOMMA_INTEROP_MEDIA_PLAY_UnregisterNotifier!!\n");
				ctx->bMediaPlayCbRegistered = FALSE;
			}
			else
			{
				HxLOG_Error("HOMMA_INTEROP_MEDIA_PLAY_UnregisterNotifier failed(Err:%d)\n", hErr);
				return ERR_FAIL;
			}
		}
	}
	else
	{
		HxLOG_Error("[__hom_dmr_handleInternalEvent] unknown event : %d\n", eEvent);
		return ERR_FAIL;
	}

	return ERR_OK;
}

void 	__hom_apk_media_init(void)
{
#ifdef HOMMA_USE_APPKIT_DIRECTLY
	HUINT32		ulViewId = 0;
	HERROR      hErr;
	HOM_DMR_Context_t *ctx = __hom_dmr_getContext();

	HOMMA_INTEROP_MEDIA_PLAY_Init();
	ctx->bMediaPlayInited = TRUE;
#else
	HOMMA_INTEROP_MEDIA_PLAY_Init(__hom_dmr_media_play_activated);
#endif
}

static HERROR	__hom_dmr_sendActionEvent(HINT32 cmd, HCHAR *pData, HUINT32	size)
{
	HOMMA_BUS_Msg_t busMsg;
	HOMMA_BUS_IpcSend_t *pMsgData = (HOMMA_BUS_IpcSend_t *)HLIB_STD_MemAlloc( sizeof(HOMMA_BUS_IpcSend_t) );

	HxSTD_MemSet(&busMsg, 0x00, sizeof(HOMMA_BUS_Msg_t));

	pMsgData->pszJson = pData;
	//pMsgData->bMalloc = TRUE;
	pMsgData->bMalloc = FALSE;
	pMsgData->nOpCode = 1;
	pMsgData->cmd = cmd;
	pMsgData->size = size;

	busMsg.eMsgType 	= eBUS_MSGTYPE_RESPONSE;
	busMsg.uEnum.eRes 	= eBUS_REQUEST_SERVER_SENDING;
	busMsg.eDataType 	= eBUS_DATATYPE_IPC_SEDN;
	busMsg.pvData 		= pMsgData;
	busMsg.nDataSize 	= sizeof(HOMMA_BUS_IpcSend_t);
	busMsg.bNeedFree 	= TRUE;

	HOMMA_BUS_SendMsg( eBUS_TARGET_SERVER, s_stBusHnd, &busMsg );

	return ERR_OK;
}

#if 0
/*  for compile warning  */
static HBOOL	__is_dmr_playing(HCHAR *pUrl)
{
	HCHAR 	szPlayingUrl[2048] = {0, };

	if(HxSTD_StrEmpty(pUrl)) return FALSE;

	{
		HUINT32 	ulViewId;
		HOMMA_INTEROP_MEDIA_PLAY_GetMainViewId(&ulViewId);

		APK_MEDIA_PLAY_GetURI(ulViewId, szPlayingUrl);
	}

	if(HxSTD_StrNCmp(pUrl, szPlayingUrl, HxSTD_StrLen(pUrl)) == 0)
		return TRUE;

	return FALSE;
}
#endif

/* Fling: Bring-In -> 현재 재생되는 Media의 URI 및 Meta를 Setting */
static void __dmr_SetBringInUri( HCHAR *pUri, HCHAR *pFilePath )
{
#if defined(CONFIG_HOMMA_DMS)
	HERROR		hErr = ERR_OK;
	HCHAR		*pTransferUri = NULL;
	HCHAR		*pMeta = NULL;
	HERROR		nResult = ERR_FAIL;
	HUINT32		unStringLen = 0;

	if((pUri == NULL) || (pUri[0] == 0))
	{
		HxLOG_Error("Error> uri is null \n" );
		return;
	}

	if(HxSTD_StrLen(pUri) < 8)
	{
		HxLOG_Error("Error> Invalid Uri(%s) \n", pUri);
		return;
	}

	/* pszUrlBuf는 file:/ 의 형태 이므로, prefix를	떼야 한다. */
	if(( HLIB_STD_StrNCaseCmp( pUri, DMR_PREFIX_FILE, HxSTD_StrLen( DMR_PREFIX_FILE ) ) == 0 )
		|| ( HLIB_STD_StrNCaseCmp( pUri, "hfile://", HxSTD_StrLen( "hfile://" ) ) == 0 ))
	{
		HCHAR *pLocalPath = NULL;

		if( HLIB_STD_StrNCaseCmp( pUri, "hfile://", HxSTD_StrLen( "hfile://" ) ) == 0 )
		{
			unStringLen = HxSTD_StrLen( "hfile://" );
		}
		else
		{
			unStringLen = HxSTD_StrLen( DMR_PREFIX_FILE );
		}

		HxLOG_Print( "[%s:%d] Local Media Play( %s )\n", __FUNCTION__, __LINE__, pUri );

		/* HMS에서 현재 Local File 재생을 시작 */
		pLocalPath = pUri + unStringLen;
		{
			nResult = DMS_GetFlingInfoFromPath( pLocalPath, &pTransferUri, &pMeta );
			if( nResult == ERR_OK )
			{
				(void)DMR_SetFlingInfomation( pTransferUri, pMeta );

				if( pTransferUri ) HLIB_STD_MemFree( pTransferUri );
				if( pMeta ) HLIB_STD_MemFree( pMeta );
			}
		}
	}
	else if(( HLIB_STD_StrNCaseCmp( pUri, DMR_PREFIX_CHANNEL, HxSTD_StrLen( DMR_PREFIX_CHANNEL ) ) == 0 )
		|| ( HLIB_STD_StrNCaseCmp( pUri, DMR_PREFIX_HDVB, HxSTD_StrLen( DMR_PREFIX_HDVB ) ) == 0 )) /* "hdvb://ccid:", not "hdvb://" */
	{
		HCHAR *p1 = NULL;
		HCHAR *p2 = NULL;
		HCHAR szSvcUid[ 32 ] = { 0, };
		HINT32	nSvcUID = 0;

		/*
		 * Due to the possibility of crash by abnormal URL string format,
		 *   channel URL only allow the correct prefix format; "hdvb://ccid:", not "hdvb://ccid:"
		 * Less frequently, webapp call wrong URL format while playing media files
		 *   - wrong URL for HDD: "hdvb:///mnt/hd2/Media/Video/MPEG_TS_SD_EU/O-MP2TS_SE-1.mpg"
		 *   - wrong URL for USB: "hdvb:///media/drive1/WMVMED_BASE/O-WVM_B-01_CBR_4M_192.wmv"
		 */
		if( HLIB_STD_StrNCaseCmp( pUri, DMR_PREFIX_HDVB, HxSTD_StrLen( DMR_PREFIX_HDVB ) ) == 0 )
		{
			unStringLen = HxSTD_StrLen( DMR_PREFIX_HDVB );
		}
		else
		{
			unStringLen = HxSTD_StrLen( DMR_PREFIX_CHANNEL );
		}

		HxLOG_Print( "[%s:%d] Live Channel Play( %s )\n", __FUNCTION__, __LINE__, pUri );

		/* HMS에서 Channel Change 발생 - hdvb://ccid:128.1.1079.28008 와 같은 형식에서 UID 파싱 */
		p1 = pUri + unStringLen;
		p2 = HxSTD_StrChar( p1, '.' );
		HxSTD_snprintf( szSvcUid, p2 - p1 + 1, "%s", p1 );

		if( HxSTD_StrLen( szSvcUid ) > 0 )
		{
			nSvcUID = HLIB_STD_StrToINT32( szSvcUid );
			nResult = DMS_GetFlingInfoFromSvcUid( nSvcUID, &pTransferUri, &pMeta );
			if( nResult == ERR_OK )
			{
				(void)DMR_SetFlingInfomation( pTransferUri, pMeta );

				if( pTransferUri ) HLIB_STD_MemFree( pTransferUri );
				if( pMeta ) HLIB_STD_MemFree( pMeta );
			}
			else
			{
				HxLOG_Error("Error> can't find scvuid(%s,%d), uri(%s)!\n", szSvcUid, nSvcUID, pUri);
			}
		}
		else
		{
			HxLOG_Error("Error> invalid scvuid(%s). \n", szSvcUid);
		}
	}
	else if( HLIB_STD_StrNCaseCmp( pUri, DMR_PREFIX_RECODINGS, HxSTD_StrLen( DMR_PREFIX_RECODINGS ) ) == 0 )
	{
		/* Cant use HOMMA_INTEROP_META_RECORD_GetByUrl() because APK_META_RECORD use local static context and opl only maintain record_meta list */
		if (pFilePath != NULL)
		{
			/* HPVR://42ae08fbb5e849429a64096d3233be1d/1.441.2ee3;0;51fbcb07 */
			hErr = DMS_GetFlingInfoOfPVRFromPath( pFilePath, &pUri, &pMeta );
			if( hErr != ERR_OK )
			{
				HxLOG_Error("[ HOM_DMR ] Error> Fail to DMS_GetFlingInfoOfPVRFromPath(Path : %s )\n", pFilePath );
				return;
			}

			DMR_SetFlingInfomation( pUri, pMeta );

			if( pUri ) HLIB_STD_MemFree( pUri );
			if( pMeta ) HLIB_STD_MemFree( pMeta );
		}
	}
	else if(( HLIB_STD_StrNCaseCmp( pUri, DMR_PREFIX_DLNA, HxSTD_StrLen( DMR_PREFIX_DLNA ) ) == 0 )
		|| ( HLIB_STD_StrNCaseCmp( pUri, "hdlna://", HxSTD_StrLen( "hdlna://" ) ) == 0 ))
	{
		HCHAR *pItemUri = NULL;
		HCHAR *pUriStart = NULL;
		HCHAR *pUriEnd = NULL;
		HINT32	nItemUriSize = 0;

		if( HLIB_STD_StrNCaseCmp( pUri, "hdlna://", HxSTD_StrLen( "hdlna://" ) ) == 0 )
		{
			unStringLen = HxSTD_StrLen( "hdlna://" );
		}
		else
		{
			unStringLen = HxSTD_StrLen( DMR_PREFIX_DLNA );
		}

		/* HMS에서 현재 DLNA 재생을 시작 */
		pUriStart = pUri + unStringLen;
		pUriEnd = HxSTD_StrStr( pUriStart, "_Prot" );
		if( pUriEnd )
		{
			nItemUriSize = pUriEnd - pUriStart + 1;
		}
		else
		{
			nItemUriSize = HxSTD_StrLen( pUriStart ) + 1;
		}

		pItemUri = ( HCHAR * )HLIB_STD_MemAlloc( nItemUriSize );
		if( pItemUri )
		{
			HxSTD_MemSet( pItemUri, 0x00, nItemUriSize );
			HxSTD_snprintf( pItemUri, nItemUriSize, "%s", pUriStart );
			HxLOG_Print( "[%s:%d] DLNA-DMP Playback( %s )\n", __FUNCTION__, __LINE__, pItemUri );

			(void)DMR_SetFlingInfomation( pItemUri, NULL );
			HLIB_STD_MemFree( pItemUri );
		}
	}
	else
	{
		HxLOG_Error( "Error> UnKnown Prefix...( %s )\n", pUri );
	}

#endif
}

/* Send upnp action event to upper layer */
static void		__hom_dmr_upnpActionCallback(HINT32 p1, HINT32 p2, HINT32 p3) /* DMR_UPnPAction_e */
{
	CO_MediaInstence_t	*pItem = NULL;
	DMR_UPnPAction_e eAction = (DMR_UPnPAction_e)p1;

	DMR_GetCurrentMedia(&pItem);

	switch(eAction)
	{
		case eDMR_UPnPAction_SETURI:
			{
				if(pItem && pItem->URI)
				{
					if(HLIB_STD_StrNCaseCmp( pItem->URI, DMR_PREFIX_HDVB, HxSTD_StrLen( DMR_PREFIX_HDVB ) ) != 0)
					{
						__hom_dmr_register_media_callback();
					}
				}
				__hom_dmr_sendActionEvent(eHOMAPI_DMR_Event_SetUri, (HCHAR *)pItem, sizeof(CO_MediaInstence_t));
			}
			break;

		case eDMR_UPnPAction_PLAY:
			{
				HINT32		nPlaySpeed = 0;
				HOMxDMR_Event_Play_t	tDMR_PlayEvent;

				nPlaySpeed = p2;

				tDMR_PlayEvent.nPlaySpeed = nPlaySpeed;

				__hom_dmr_sendActionEvent(eHOMAPI_DMR_Event_Play, (HCHAR *)&tDMR_PlayEvent, sizeof(HOMxDMR_Event_Play_t));
			}
			break;

		case eDMR_UPnPAction_STOP:
			{
				__hom_dmr_unregister_media_callback();
				__hom_dmr_sendActionEvent(eHOMAPI_DMR_Event_Stop, NULL, 0);
			}
			break;

		case eDMR_UPnPAction_PAUSE:
			{
				__hom_dmr_sendActionEvent(eHOMAPI_DMR_Event_Pause, NULL, 0);
			}
			break;

		case eDMR_UPnPAction_SEEK:
			{
				HUINT32 	ulViewId;
				HOMMA_INTEROP_MEDIA_PLAY_GetMainViewId(&ulViewId);
				HOMMA_INTEROP_MEDIA_PLAY_SetPlayPosition(ulViewId, p2);
				HxLOG_Debug("### e_DMR_SEEK - %d \n", p2);
			}
			break;

		case eDMR_UPnPAction_VOLUME:
			{
				HINT32		nVolume = 0;

				nVolume = p2;
				__hom_dmr_sendActionEvent(eHOMAPI_DMR_Event_Volume, (HCHAR *)&nVolume, sizeof(HINT32));
				HxLOG_Debug("### e_DMR_VOLUME\n");
			}
			break;

		case eDMR_UPnPAction_MUTE:
			{
				HBOOL		bMute = 0;

				bMute = (HBOOL)p2;
				__hom_dmr_sendActionEvent(eHOMAPI_DMR_Event_Mute, (HCHAR *)&bMute, sizeof(HBOOL));
				HxLOG_Debug("### e_DMR_MUTE\n");
				break;
			}
			break;

		case eDMR_UPnPAction_BRINGIN:
			{
				__hom_dmr_sendActionEvent(eHOMAPI_DMR_Event_BringIn, NULL, 0);
			}
			break;

		default:
			break;
	}
}
#endif

static HERROR __hom_dmr_msg_decoder( HOMMA_BUS_Target_e eSender, const HOMMA_BUS_IpcRecv_t *stJsonData )
{
#if defined(CONFIG_HOMMA_DMR)
	HERROR	hError = ERR_OK;
#endif
	HINT32	nOper =	stJsonData->nOpCode; /* property json */
	comm_msg_data_t *data = stJsonData->pvJson;
	HOM_DMR_Context_t *pHomDmrCtx = __hom_dmr_getContext();

	if ( eBUS_CMD_GET == nOper )
	{
		HCHAR	*endcode_str = NULL;

		if ( data->size == 0)
		{
			HOMMA_BUS_Msg_t 	busMsg;
			HOMMA_BUS_IpcSend_t *pMsgData = (HOMMA_BUS_IpcSend_t *)HLIB_STD_MemCalloc( sizeof(HOMMA_BUS_IpcSend_t));

			HxSTD_MemSet(&busMsg, 0x00, sizeof(HOMMA_BUS_Msg_t));

			pMsgData->nPairId = stJsonData->nPairId ;
			pMsgData->pszJson = endcode_str ;
			pMsgData->bMalloc = TRUE;
			pMsgData->nClientHandle = stJsonData->nClientHandle;

			busMsg.eMsgType 	= eBUS_MSGTYPE_RESPONSE;
			busMsg.uEnum.eRes 	= eBUS_REQUEST_SERVER_SENDING;
			busMsg.eDataType 	= eBUS_DATATYPE_IPC_SEDN;
			busMsg.pvData 		= pMsgData;
			busMsg.nDataSize 	= sizeof(HOMMA_BUS_IpcSend_t);
			busMsg.bNeedFree 	= TRUE;

			HOMMA_BUS_SendMsg( eSender, s_stBusHnd, &busMsg );
			return ERR_OK;
		}
	}
	else if ( eBUS_CMD_SET == nOper )
	{
		switch ( data->cmd)
		{
			case eHOMAPI_DMR_Command_Start:
				{
#if defined(CONFIG_HOMMA_DMR)
					HOMMA_BUS_Data_t 	retData;
					HERROR				retErr = ERR_FAIL;
					HOMxDMR_Args_Start_t *param = (HOMxDMR_Args_Start_t *)data->data;

					if ((pHomDmrCtx->bPrevStartFailed == TRUE) && (pHomDmrCtx->bRequestStart == TRUE))
					{
						HxLOG_Error("### [%s:%d] Error> Overlapped start() request(prev:%d)! ###\n", __func__, __LINE__, pHomDmrCtx->bPrevStartFailed);
						;
					}
					pHomDmrCtx->bRequestStart = TRUE; /* DMR start() failture 이후 Wait 상태에서 start_request 문제된다 */

					if (DMR_IsStart() == TRUE)
						HxLOG_Error("### [%s:%d] Error> Already DMR started! ###\n", __func__, __LINE__);

					if (pHomDmrCtx->pszFriendlyName != NULL)
					{
						HOM_MEMFREE(pHomDmrCtx->pszFriendlyName);
						pHomDmrCtx->pszFriendlyName= NULL;
					}
					pHomDmrCtx->pszFriendlyName = HLIB_STD_StrDup((HCHAR *)param->szFriendlyName);

					HxSTD_MemSet(&retData, 0x00, sizeof(HOMMA_BUS_Data_t));
					HOMMA_BUS_Request( eBUS_TARGET_SYS, eBUS_REQUEST_NET_INFO, &retData, &retErr );
					if ( ERR_OK != retErr )
					{
						HxLOG_Error("### [%s:%d] Error> Fail HOMMA_BUS_Request(NET_INFO) ###\n", __func__, __LINE__);
						return ERR_FAIL;
					}

					HxLOG_Debug("[ HOM_DMR ] ==============================================\n");
					HxLOG_Debug("[ HOM_DMR ] ===========> REQ DMR START : %s \n", (HCHAR *)param->szFriendlyName);
					HxLOG_Debug("[ HOM_DMR ] ==============================================\n");

					hError = ERR_FAIL;
					if (DMR_IsStart() == FALSE)
					{
						DMR_ModuleInit();
						DMR_RegisterUPnPActionCallback(__hom_dmr_upnpActionCallback);

						if(HxSTD_StrEmpty(param->szFriendlyName))
						{
							hError = DMR_Start(NULL);
						}
						else
						{
							hError = DMR_Start(param->szFriendlyName);
						}
					}
					else if ((pHomDmrCtx->bPrevStartFailed == TRUE) && (pHomDmrCtx->bRequestStart == TRUE))
					{
						HxLOG_Error("### [%s:%d] Error> Already progressed start() request(prev:%d)! ###\n", __func__, __LINE__, pHomDmrCtx->bPrevStartFailed);
						if(HxSTD_StrEmpty(param->szFriendlyName))
						{
							hError = DMR_Start(NULL);
						}
						else
						{
							hError = DMR_Start(param->szFriendlyName);
						}
					}

					if (hError == ERR_OK)
					{
						pHomDmrCtx->bPrevStartFailed = FALSE;
					}
					else
					{
						HxLOG_Error("[ HOM_DMR ] Error> failed to start DMR!\n");
						pHomDmrCtx->bPrevStartFailed = TRUE;
					}
#endif
				}
				break;

			case eHOMAPI_DMR_Command_Stop:
				{
					HxLOG_Debug("[ HOM_DMR ] ==============================================\n");
					HxLOG_Debug("[ HOM_DMR ] ===========> REQ DMR STOP ====================\n");
					HxLOG_Debug("[ HOM_DMR ] ==============================================\n");

					pHomDmrCtx->bPrevStartFailed = FALSE;
					pHomDmrCtx->bRequestStart = FALSE;

#if defined(CONFIG_HOMMA_DMR)
					DMR_Stop();
					DMR_ModuleTerm();
#endif
					if (pHomDmrCtx->pszFriendlyName != NULL)
					{
						HOM_MEMFREE(pHomDmrCtx->pszFriendlyName);
						pHomDmrCtx->pszFriendlyName= NULL;
					}
				}
				break;

			case eHOMAPI_DMR_Command_Rename:
				{
#if defined(CONFIG_HOMMA_DMR)
					HOMMA_BUS_Data_t 	retData;
					HOMxDMR_Args_Rename_t *param = (HOMxDMR_Args_Rename_t *)data->data;

					HxSTD_MemSet(&retData, 0x00, sizeof(HOMMA_BUS_Data_t));

					HxLOG_Debug("[ HOM_DMR ] ==============================================\n");
					HxLOG_Debug("[ HOM_DMR ] ===========> REQ DMR RENAME : %s \n", (HCHAR *)param->szFriendlyName);
					HxLOG_Debug("[ HOM_DMR ] ==============================================\n");

					if ((DMR_IsStart() == TRUE) && !HxSTD_StrEmpty(param->szFriendlyName))
					{
						DMR_SetName(param->szFriendlyName);
					}
#endif
				}
				break;

			case eHOMAPI_DMR_Command_BringIn:
				{
#if defined(CONFIG_HOMMA_DMR)
					HOMxDMR_Args_BringIn_t *param = (HOMxDMR_Args_BringIn_t *)data->data;

					if(param != NULL)
					{
						HxLOG_Debug("CHANGE URL = %s \n", param->szMediaURI);
						__dmr_SetBringInUri(param->szMediaURI, param->szMediaPath);
					}
#endif
				}
				break;

			case eHOMAPI_DMR_Command_OperationMode:
				break;

			default:
				break;

		}
	}

	return ERR_OK;
}

static HINT32	__hom_dmr_bus_request_handler( HOMMA_BUS_Request_e eReq, HOMMA_BUS_Data_t *pstRetData )
{
	return 0;
}

// 2014/2/28 mhakng:  homma_server.c의 __server_bus_release_handler 함수가 공통적으로 메모리를 해제하도록 되어 있다.
// 아래 함수가 호출되는 일은 현재 구현된 상태에서 없다.
static HINT32	__hom_dmr_bus_release_handler( HOMMA_BUS_Target_e eSender, HOMMA_BUS_Msg_t *pstMsg )
{
	switch ( pstMsg->eDataType )
	{
		case eBUS_DATATYPE_IPC_SEDN :
			{
				HOMMA_BUS_IpcSend_t *pIpcSend = pstMsg->pvData;
				if ( pIpcSend && pIpcSend->bMalloc && pIpcSend->pszJson )
				{
					HOM_MEMFREE( pIpcSend->pszJson );
					pIpcSend->pszJson = NULL;
				}
			}
			break;

		default :
			break;
	}

	if ( pstMsg->bNeedFree && pstMsg->pvData )
	{
		HOM_MEMFREE( pstMsg->pvData );
		pstMsg->pvData = NULL;
	}

	return 0;
}

static HINT32	__hom_dmr_bus_message_handler( HOMMA_BUS_Target_e eSender, const HOMMA_BUS_Msg_t *pstMsg )
{
	switch ( pstMsg->eMsgType )
	{
		case eBUS_MSGTYPE_EVENT :
			if ( eBUS_TARGET_SYS == eSender )
			{
				__hom_dmr_handleSystemEvent( pstMsg->uEnum.eEvt, pstMsg->eDataType, pstMsg->pvData );
			}
#if defined(CONFIG_HOMMA_DMR)
			else if ( eBUS_TARGET_MOD_DMR == eSender)
			{
				__hom_dmr_handleInternalEvent( pstMsg->uEnum.eEvt, pstMsg->eDataType, pstMsg->pvData );
			}
#endif
			break;

		case eBUS_MSGTYPE_REQUEST :
			if ( eBUS_REQUEST_JSON_DECODE == pstMsg->uEnum.eReq && eBUS_DATATYPE_IPC_RECV == pstMsg->eDataType )
				__hom_dmr_msg_decoder( eSender, pstMsg->pvData );
			break;

		case eBUS_MSGTYPE_RESPONSE :
			break;

		default :
			break;
	}
	return 0;
}

#if defined(CONFIG_HOMMA_DMR)
static HINT32	__hom_dmr_callback_process( DMR_StatusEvent_e eEvent, HINT32 p1, HINT32 p2, HINT32 p3 )
{
	HOM_DMR_Context_t *pHomDmrCtx = __hom_dmr_getContext();
	HOMMA_BUS_Msg_t stMsg = { 0 };

	HxSTD_MemSet(&stMsg, 0x00, sizeof(HOMMA_BUS_Msg_t));
	stMsg.eMsgType = eBUS_MSGTYPE_EVENT;

	switch ( eEvent )
	{
		case eDMR_StatusEvent_START :
			stMsg.uEnum.eEvt = eBUS_EVENT_MOD_START;
			HOMMA_BUS_SendMsg( eBUS_TARGET_ALL, pHomDmrCtx->hMsgBusHandle, &stMsg );
			break;

		case eDMR_StatusEvent_STOP :
			stMsg.uEnum.eEvt = eBUS_EVENT_MOD_STOP;
			HOMMA_BUS_SendMsg( eBUS_TARGET_ALL, pHomDmrCtx->hMsgBusHandle, &stMsg );
			break;

		default :
			break;
	}
	return 0;
}
#endif

#define ________________Public_Functions___________________________
void	HOM_DMR_Run( void )
{
	HOM_DMR_Context_t 		*pHomDmrCtx = __hom_dmr_getContext();
	HOMMA_BUS_HndFuncList_t stFuncList;
	HOMMA_BUS_Msg_t stMsg;

	HxSTD_MemSet(&stFuncList, 0x00, sizeof(HOMMA_BUS_HndFuncList_t));
	HxSTD_MemSet(&stMsg, 0x00, sizeof(HOMMA_BUS_Msg_t));

	__hom_dmr_init( pHomDmrCtx );

	stFuncList.cbMsgHandleFunc = __hom_dmr_bus_message_handler;
	stFuncList.cbReqHandleFunc = __hom_dmr_bus_request_handler;
	stFuncList.cbRelHandleFunc = __hom_dmr_bus_release_handler;

	s_stBusHnd = HOMMA_BUS_GetHandle( eBUS_TARGET_MOD_DMR );
	HOMMA_BUS_Register( s_stBusHnd, &stFuncList );

	pHomDmrCtx->hMsgBusHandle = s_stBusHnd;

	stMsg.eMsgType = eBUS_MSGTYPE_EVENT;
	stMsg.uEnum.eEvt = 0;

	HOMMA_BUS_SendMsg( eBUS_TARGET_ALL, s_stBusHnd, &stMsg );

#if defined(CONFIG_HOMMA_DMR)
	DMR_Initialize();
	DMR_SetEvtListener( __hom_dmr_callback_process );
	__hom_apk_media_init();
#endif

	HxLOG_Print("RO_DMR Run!!!\n");
}
