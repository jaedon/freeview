/*************************************************************************************************************
	File            : dmr.h
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
#include "dapi.h"

#include "dmr.h"
#include "pl_common.h"
#include "pl_op.h"
#include "pl_dmr.h"
#include "pl_dms.h" /* PL_DMS_GetLocalPathfromUri() */

#define ________________Defines____________________________________
/* Default Service Info */
#define DMR_DEFAULT_SERVICE_PORT			55200

typedef enum
{
	eDMR_STATUS_STOPPED=0,
	eDMR_STATUS_READY,
	eDMR_STATUS_STARTED,
	eDMR_STATUS_UNKNOWN,
} DMR_Status_e;

#define DMR_IS_OPERATION_MODE() \
	if(__is_operation_mode() == FALSE ) \
	{\
		HxLOG_Warning( "[ %s:%d ] Warning> The DMR Operation isn't support active standby. \n", __FUNCTION__, __LINE__ );\
		return eHOM_Result_Error;\
	}\

#define ________________Typedef____________________________________
typedef struct
{
	HBOOL						bInited;
	DMR_Status_e				eStatus;
	HBOOL						bReqStart; // request Start Status
	HCHAR						szFriendlyName[ 128 ];
	PL_COMMON_NetworkInfo_t		stNetworkInfo;				/** Network Interface정보 */
	DMR_EVT_HANDLE_Func			cbDmrEvtNotify;		/* result event for upper layer */
	DMR_UPnPActionCallback_t 		action_cb; /* upnp action event by external upnp device */
	HCHAR						*pszLocalHmsUri;
	HBOOL						bMediaReady;

#if 0
	HCHAR						*pCurrentStbUri;			// 현재 시청중인 URI, Bring-In시 사용.
	HCHAR						*pFlingURI;					// Fling용 URI
	CO_MEDIA_Instence_t			*pFlingMeta;				// Fling용 Metadata
#endif

}DMR_Context_t;

#define ________________Private_Static_Variables___________________
static DMR_Context_t s_DMR_Context = {0, };

#define ________________Internal_APIs_Prototype____________________
static 	DMR_Context_t *	__get_context(void);
static 	HBOOL 			__is_operation_mode(void);
static 	HERROR			__set_uri(void);
static	HINT32 			__avt_event_cv(HINT32 evt, HINT32 p1, HINT32 p2);

#define ________________Internal_Functions_________________________
/* DMR Context를 얻는 함수. */
static DMR_Context_t *__get_context( void )
{
	return &s_DMR_Context;
}

/* 현재 STB Mode 확인. Operation/Standby */
static HBOOL __is_operation_mode(void) /* <-- standby */
{
#if 1
	/* PL_OP_GetStandbyMode() 가 동작 하지 않아 임시로 항상 TRUE.. */
	return TRUE;
#else
	PL_OP_STANDBYMODE_e eStandbyMode;

	(void)PL_OP_GetStandbyMode(&eStandbyMode);

	if(eStandbyMode == ePL_OP_STADNBYMODE_OPERATION)
		return TRUE;
	else
		return FALSE;
#endif
}

STATIC void __dmr_CB_GetSvcInfo (const HCHAR *name, HUID uid, const void *data, HUINT32 size, void *userdata)
{
	DAxIterator_t		 stIter;
	DxService_t			*pstDstSvc = (DxService_t *)userdata;

	DAPI_InitIterator(&stIter, data, size, 0);
	while (DAPI_NextIterator(&stIter))
	{
		DxService_t			*pstSrcSvc = (DxService_t *)stIter.data;

		if (stIter.uid != (HUID)pstSrcSvc->uid)
		{
			HxLOG_Error("Getter failed: Wrong UID : (%d, %d)\n", stIter.uid, pstSrcSvc->uid);
			continue;
		}

		HxSTD_MemCopy(pstDstSvc, pstSrcSvc, sizeof(DxService_t));
	}
}

#if defined(CONFIG_HOMMA_DMS)
static HERROR __set_uri( void )
{
	HERROR hErr = ERR_OK;
	DMR_Context_t 			*hContext = __get_context();
	HCHAR 					*p1 = NULL, *p2 = NULL;
	HCHAR 					szLocalHmsUri[ 256 ] = { 0, };
	HCHAR					szUidFromUri[ 32 ] = { 0, };
	HINT32					nSvcUid = 0;
	CO_MediaInstence_t		*pItem = NULL;

	HOM_FUNC_ENTER;

	DMR_IS_OPERATION_MODE();

	hErr = PL_DMR_GetCurrentItem(&pItem);
	if ((hErr != ERR_OK) || (pItem == NULL))
	{
		HxLOG_Error("Error> Cant get current item!\n");
		return eHOM_Result_Ok;
	}

	HxLOG_Print("[%s:%d] URI (%s) \n", __FUNCTION__, __LINE__, pItem->URI);

	/* 현재는 HTTP에 대해서만 대응함. 향후 RTP or RTSP가 추가시에 대해 고려 필요함. */
	p1 = HxSTD_StrStr( pItem->URI, "http://" );
	if( p1 != NULL )
	{
		p1 += HOM_STRLEN( "http://" );
		if( p1 == NULL )
		{
			HxLOG_Error( "[ %s:%d ] Error> Access point is null.\n", __FUNCTION__, __LINE__ );
			if (hContext->pszLocalHmsUri != NULL)
			{
				HOM_MEMFREE(hContext->pszLocalHmsUri);
				hContext->pszLocalHmsUri = NULL;
			}
			return ERR_OK;
		}
	}
	else
	{
		HxLOG_Error( "[ %s:%d ] Error> Currently, http protocol only support.\n", __FUNCTION__, __LINE__ );
		if (hContext->pszLocalHmsUri != NULL)
		{
			HOM_MEMFREE(hContext->pszLocalHmsUri);
			hContext->pszLocalHmsUri = NULL;
		}
		return ERR_FAIL;
	}

	/* Uri의 IP와 Local IP가 동일한 경우에는 별도 처리 할지 판단 */
#if defined(CONFIG_LXC_ONLY) || defined(CONFIG_CHROOT_AND_LXC)
	// LXC인 경우에는 IP 비교하지 않도록 수정 (외부 IP : 192.168.0.XX, 내부 IP : 10.255.128.213)
	if (hContext->stNetworkInfo.bValid == TRUE)
#else
	if ((hContext->stNetworkInfo.bValid == TRUE) &&
		(HOM_STRNCMP( p1, hContext->stNetworkInfo.szIp, HOM_STRLEN( hContext->stNetworkInfo.szIp) ) == 0) )
#endif
	{
		/****************************************************************************************************/
		/*  Local DMS에서 이용되는 Content 처리																			*/
		/*	- Local DMS에서 이용되는 Content(Local File, Local Recording, Live Channel)등에 대해서는 아래 기준으로 처리함.			*/
		/*		1) Local File																								*/
		/*			=> IP와 Url을 이용하여 확인시 Local File인 경우는 file://~~~(실제 Path)를 전달함. 							*/
		/*			- ex : 실제 URI																						*/
		/*				http://192.168.0.8:9000/web/media/Test.avi															*/
		/*			- ex : UID 241에 대한 채널 전환 URI																		*/
		/*				file://media/drive/Test.avi																			*/
		/*		2) Recording File																							*/
		/*			=> IP와 Url을 이용하여 확인시 Recording File인 경우는 Local File과 동일하게 file://~~~(실제 Path)로 전달함. 		*/
		/*			- ex : 실제 URI																						*/
		/*				http://192.168.0.8:9000/web/media/Test.avi															*/
		/*			- ex : UID 241에 대한 채널 전환 URI																		*/
		/*				file://media/drive/Test.avi																			*/
		/*		3) Live Channel																							*/
		/*			=> IP와 Url을 이용하여 확인시 Live Channel인 경우는 아래와 같이 전달함.							 		*/
		/*			=> URI에서 "HCHANNEL:" prefix가  있고, IP가 Local IP와 동일할 경우에 한함									*/
		/*			=> UID를 찾아 Channel 전환 명령어로 URI를 변경해 전달													*/
		/*			- ex : 실제 URI																						*/
		/*				http://192.168.0.8:9000/web/tuner/HCHANNEL:241.m3u8												*/
		/*			- ex : UID 241에 대한 채널 전환 URI																		*/
		/*				dvb://triplet:OnId.TsId.SvcId																		*/
		/****************************************************************************************************/
		p1 = HOM_STRSTR( pItem->URI, "HCHANNEL:" );
		if( p1 != NULL )
		{
			p1 += HOM_STRLEN( "HCHANNEL:" );
			if( p1 )
			{
				p2 = HOM_STRCHAR( p1, '.' );
			}

			if( p1 && p2 )
			{
				DxService_t 				stService = {0, };
				HUINT32 					ulDapiReqId = 0;
				HERROR						hError = ERR_FAIL;

				HOM_SNPRINTF( szUidFromUri, p2 - p1 + 1, "%s", p1 );
				nSvcUid = HOM_STRTOINT32( szUidFromUri );

				ulDapiReqId = DAPI_Get(DxNAMEOF(DxService_t), (HUID)nSvcUid, __dmr_CB_GetSvcInfo, (void *)&stService);
				hError = DAPI_Wait(6000, ulDapiReqId);
				if ((DERR_TIMEOUT == hError) || (0 == stService.uid))
				{
					HxLOG_Error("Service not found:\n");
					return ERR_FAIL;
				}

				/*	hdvb://triplet:OnId.TsId.SvcId */
				HOM_SNPRINTF( szLocalHmsUri, sizeof( szLocalHmsUri ), "hdvb://ccid:%d.%d.%d.%d", nSvcUid, stService.onid, stService.tsid, stService.svcid);
				HxLOG_Print( "[ %s:%d ] SvcUid(%d) --> Local Channel URI( %s )\n", __FUNCTION__, __LINE__, nSvcUid, szLocalHmsUri );
				if (hContext->pszLocalHmsUri != NULL)
				{
					HOM_MEMFREE(hContext->pszLocalHmsUri);
					hContext->pszLocalHmsUri = NULL;
				}
				hContext->pszLocalHmsUri = HLIB_STD_StrDup(szLocalHmsUri);

				HxSTD_MemSet(pItem->URI, 0x00, sizeof(pItem->URI));
				HOM_SNPRINTF(pItem->URI, sizeof(pItem->URI), "%s", szLocalHmsUri);
			}
			else
			{
				if (hContext->pszLocalHmsUri != NULL)
				{
					HOM_MEMFREE(hContext->pszLocalHmsUri);
					hContext->pszLocalHmsUri = NULL;
				}
			}

			return ERR_OK;
		}

		/* Media file */
		p1 = HOM_STRSTR( pItem->URI, "/web/media/" );
		if( p1 != NULL )
		{
			p2 = PL_DMS_GetLocalPathfromUri(pItem->URI);
			if(p2 != NULL)
			{
				HOM_SNPRINTF( szLocalHmsUri, sizeof( szLocalHmsUri ), "file://%s", p2 );
				HxLOG_Print( "[ %s:%d ] Local File Path( %s )\n", __FUNCTION__, __LINE__, szLocalHmsUri );
				if (hContext->pszLocalHmsUri != NULL)
				{
					HOM_MEMFREE(hContext->pszLocalHmsUri);
					hContext->pszLocalHmsUri = NULL;
				}
				hContext->pszLocalHmsUri = HLIB_STD_StrDup(szLocalHmsUri);
			}
			return ERR_OK;
		}
	}
	else
	{
		if (1) /* For error debugging */
		{
			p1 = HOM_STRSTR( pItem->URI, "HCHANNEL:" );
			if (p1 != NULL)
			{
				HxLOG_Error( "[ %s:%d ] Error> URI(%s) is unmatched w/ network(%d, %s)\n",
						__FUNCTION__, __LINE__,
						pItem->URI,
						hContext->stNetworkInfo.bValid, hContext->stNetworkInfo.szIp);
			}
		}

		if (hContext->pszLocalHmsUri != NULL)
		{
			HOM_MEMFREE(hContext->pszLocalHmsUri);
			hContext->pszLocalHmsUri = NULL;
		}
	}

	HOM_FUNC_LEAVE;

	return ERR_OK;
}
#endif

static HINT32 __avt_event_cv(HINT32 evt, HINT32 p1, HINT32 p2)
{
	DMR_Context_t 	*hContext = __get_context();
	HBOOL			bIsCB = hContext->action_cb? TRUE:FALSE;

	DMR_IS_OPERATION_MODE();

	switch(evt)
	{
		case ePL_DMR_AVT_EVT_SETURI:
			{
				HxLOG_Debug("#### SETURI #### \n");
#if defined(CONFIG_HOMMA_DMS)
				__set_uri();
#endif
				if(bIsCB) hContext->action_cb(eDMR_UPnPAction_SETURI, 0,0);
			}
			break;

		case ePL_DMR_AVT_EVT_PLAY:
			{
				HxLOG_Debug("#### PLAY[%d] #### \n", p1);

				if(bIsCB) hContext->action_cb(eDMR_UPnPAction_PLAY, p1,0);
			}
			break;

		case ePL_DMR_AVT_EVT_STOP:
			{
				HxLOG_Debug("#### STOP #### \n");
				if(bIsCB) hContext->action_cb(eDMR_UPnPAction_STOP, 0, 0);
			}
			break;

		case ePL_DMR_AVT_EVT_PAUSE:
			{
				HxLOG_Debug("#### PAUSE #### \n");
				if(bIsCB) hContext->action_cb(eDMR_UPnPAction_PAUSE, 0, 0);
			}
			break;

		case ePL_DMR_AVT_EVT_SEEK:
			{
				HxLOG_Debug("#### SEEK - %d #### \n", p1);
				if(bIsCB) hContext->action_cb(eDMR_UPnPAction_SEEK, p1, 0);
			}
			break;

			/* Volume 관련 사항은 PL에서 바로 처리... */
		case ePL_DMR_AVT_EVT_MUTE:
			{
				HxLOG_Debug("#### MUTE #### \n");
				if(bIsCB) hContext->action_cb(eDMR_UPnPAction_MUTE,0,0);
			}
			break;

		case ePL_DMR_AVT_EVT_VOLUME:
			{
				HxLOG_Debug("#### VOLUME #### \n");
				if(bIsCB) hContext->action_cb(eDMR_UPnPAction_VOLUME, p1, 0);
			}
			break;

		case ePL_DMR_AVT_EVT_BRINGIN:
			{
				HxLOG_Debug("#### BRINGIN #### \n");
				if(bIsCB) hContext->action_cb(eDMR_UPnPAction_BRINGIN, p1, 0);
			}
			break;

		default : break;
	}

	return 0;
}

#define ________________Public_Functions___________________________

void 	DMR_Initialize( void ) /* Called once after spawning homma process */
{
	DMR_Context_t 	*hContext = __get_context();
	if (hContext->bInited != TRUE)
	{
		HOM_MEMSET( hContext, 0x00, sizeof( DMR_Context_t ) );
		hContext->eStatus = eDMR_STATUS_STOPPED;
		hContext->bReqStart = FALSE;
		hContext->cbDmrEvtNotify = NULL;
		hContext->action_cb = NULL;
		hContext->pszLocalHmsUri = NULL;
		hContext->bMediaReady = FALSE;
		hContext->bInited = TRUE;
	}
}

HERROR	DMR_ModuleInit( void )
{
	DMR_Context_t 		*hContext = __get_context();

	/* Dont memset(); */
	if (hContext->bInited != TRUE)
	{
		HxLOG_Error( "Error> uninitialized!\n" );
		DMR_Initialize();
	}
	else
	{
		if (hContext->eStatus != eDMR_STATUS_STOPPED)
		{
			HxLOG_Error( "Error> Non-stopped status(%d)\n", hContext->eStatus);
		}
		hContext->bReqStart = FALSE;
		hContext->action_cb = NULL;
		hContext->bMediaReady = FALSE;
		/* Skip cbDmrEvtNotify, eStatus */
	}

	if (PL_COMMON_GetDefaultNetwork(&hContext->stNetworkInfo) != ERR_OK)
	{
		HxLOG_Error( "Error> cant get networkInfo!\n" );

		/* TODO: Retry PL_COMMON_GetActiveNetwork()? */
	}
	else
	{
		HxLOG_Error("[DMR] %s(%d) + [devIdx    :%d]\n", __FUNCTION__, __LINE__, hContext->stNetworkInfo.nDevIdx);
		HxLOG_Error("[DMR] %s(%d) + [Valid     :%d]\n", __FUNCTION__, __LINE__, hContext->stNetworkInfo.bValid);
		HxLOG_Error("[DMR] %s(%d) + [Connected :%d]\n", __FUNCTION__, __LINE__, hContext->stNetworkInfo.bConnected);
		HxLOG_Error("[DMR] %s(%d) + [Active    :%d]\n", __FUNCTION__, __LINE__, hContext->stNetworkInfo.bActive);
		HxLOG_Error("[DMR] %s(%d) + [IpAddr    :0x%x]\n", __FUNCTION__, __LINE__, hContext->stNetworkInfo.ulIpAddr);
		HxLOG_Error("[DMR] %s(%d) + [Ip        :%s]\n", __FUNCTION__, __LINE__, hContext->stNetworkInfo.szIp);
	}

	return ERR_OK;
}

/*  초기화한 DMR 기능을 종료하기 위한 함수 */
HERROR	DMR_ModuleTerm( void )
{
	DMR_Context_t 		*hContext = __get_context();

	/* Dont memset(); */
	if (hContext->bInited == TRUE)
	{
		if (hContext->eStatus != eDMR_STATUS_STARTED)
		{
			HxLOG_Error( "Error> Non-stopped status(%d)\n", hContext->eStatus);
		}
		hContext->bReqStart = FALSE;
		/*hContext->cbDmrEvtNotify = NULL;*/
		hContext->action_cb = NULL;
		hContext->bMediaReady = FALSE;
	}

	return ERR_OK;
}

HERROR 	DMR_OnSystemEvent( HOMMA_BUS_Event_e eEvent, void *eDataType, void *pvData  )
{
	/*DMR_Context_t *hContext = __get_context();*/
	/*CO_NET_Iface_t *pIfaceInfo = NULL;*/
	HxLOG_Error("++[system event:%d]\n", eEvent);

	switch ( eEvent )
	{
		case eBUS_EVENT_SYS_NET_IF_ONLINE :
		case eBUS_EVENT_SYS_NET_IF_SWITCHING :
			{ // 2014/11/14 mhkang: http://svn:3000/issues/97480
				DMR_Context_t       *hContext = __get_context();
				if (PL_COMMON_GetDefaultNetwork(&hContext->stNetworkInfo) != ERR_OK)
				{
					HxLOG_Error( "Error> cant get networkInfo!\n" );

					/* TODO: Retry PL_COMMON_GetActiveNetwork()? */
				}
				else
				{
					HxLOG_Error("[DMR] %s(%d) + [devIdx    :%d]\n", __FUNCTION__, __LINE__, hContext->stNetworkInfo.nDevIdx);
					HxLOG_Error("[DMR] %s(%d) + [Valid     :%d]\n", __FUNCTION__, __LINE__, hContext->stNetworkInfo.bValid);
					HxLOG_Error("[DMR] %s(%d) + [Connected :%d]\n", __FUNCTION__, __LINE__, hContext->stNetworkInfo.bConnected);
					HxLOG_Error("[DMR] %s(%d) + [Active    :%d]\n", __FUNCTION__, __LINE__, hContext->stNetworkInfo.bActive);
					HxLOG_Error("[DMR] %s(%d) + [IpAddr    :0x%x]\n", __FUNCTION__, __LINE__, hContext->stNetworkInfo.ulIpAddr);
					HxLOG_Error("[DMR] %s(%d) + [Ip        :%s]\n", __FUNCTION__, __LINE__, hContext->stNetworkInfo.szIp);
				}
			}
			break;

		case eBUS_EVENT_SYS_NET_IF_OFFLINE :
			break;

		case eBUS_EVENT_SYS_POWER_REBOOT:
		case eBUS_EVENT_SYS_POWER_SHUTDOWN:
#if 0	// 2016/1/23 mhkang: when hdd is fomated, it is not necessary to stop dmr (http://svn.humaxdigital.com:3000/issues/110389)
		case eBUS_EVENT_SYS_MS_FORMAT_TRY:
#endif
			{
				HxLOG_Error( "System Event(%d) - DMR stop()\n", eEvent);
				DMR_Stop();
				DMR_ModuleTerm();
			}
			break;

		default :
			return ERR_OK;
	}

	return ERR_OK;
}

void 	DMR_SetEvtListener( DMR_EVT_HANDLE_Func cbNotify )
{
	DMR_Context_t *hContext = __get_context();
	hContext->cbDmrEvtNotify = cbNotify;
}

/* 외부에서 참조되는 DMR FriendlyName 설정 */
HERROR	DMR_Start( HCHAR *aModelName )
{
	HERROR hErr = ERR_OK;
	DMR_Context_t 	*hContext = __get_context();
	HLONG			lCurrnetVolume = 0;
	HBOOL			bIpAttachOnName = TRUE; /* This is scenario for HMS base models */
	HBOOL			bValidIP = FALSE;
	PL_COMMON_NetworkInfo_t	net_info;
	PL_COMMON_ProductInfo_t	product_info;
	HxLOG_Error("[ DMR ] (%s)++\n", __FUNCTION__);
	HOM_FUNC_ENTER;

	hContext->bReqStart = TRUE;
	if (1)
	{
		HOMMA_INTEROP_OUTPUT_AUDIO_Init(); /* TODO: Move to dlna_init() */
	}

	if ( eDMR_STATUS_STARTED == hContext->eStatus )
	{
		HxLOG_Error( "Error> DMR Already Started.\n" );
		return ERR_OK;
	}

	HxSTD_MemSet(&product_info, 0x00, sizeof(PL_COMMON_ProductInfo_t));
	PL_COMMON_GetProductInfo(&product_info);

	HxSTD_MemSet(&net_info, 0x00, sizeof(PL_COMMON_NetworkInfo_t));
	if ((bIpAttachOnName == TRUE) && (PL_COMMON_GetActiveNetwork(&net_info) == ERR_OK)) // rather than PL_COMMON_GetDefaultNetwork
	{
		/* Validate IP */
		if ((net_info.ulIpAddr != 0) && !HxSTD_StrEmpty(net_info.szIp))
		{
			bValidIP = TRUE;
		}
		else
		{
			hErr = PL_COMMON_GetDefaultNetwork(&net_info);
			if(hErr != ERR_OK)
			{
				HxLOG_Error("Error> Fail to get default network information.\n");
			}

			if ((net_info.ulIpAddr != 0x0) && !HxSTD_StrEmpty(net_info.szIp))
			{
				bValidIP = TRUE;
			}
			else
			{
				HxLOG_Error("[DMR] %s(%d) Error> [devIdx    :%d]\n", __FUNCTION__, __LINE__, net_info.nDevIdx);
				HxLOG_Error("[DMR] %s(%d) Error> [Valid     :%d]\n", __FUNCTION__, __LINE__, net_info.bValid);
				HxLOG_Error("[DMR] %s(%d) Error> [Connected :%d]\n", __FUNCTION__, __LINE__, net_info.bConnected);
				HxLOG_Error("[DMR] %s(%d) Error> [Active    :%d]\n", __FUNCTION__, __LINE__, net_info.bActive);
				HxLOG_Error("[DMR] %s(%d) Error> [IpAddr    :0x%x]\n", __FUNCTION__, __LINE__, net_info.ulIpAddr);
				HxLOG_Error("[DMR] %s(%d) Error> [Ip        :%s]\n", __FUNCTION__, __LINE__, net_info.szIp);
			}
		}
	}

	if(aModelName == NULL)
	{
		if ((bIpAttachOnName == TRUE) && (bValidIP == TRUE))
			HxSTD_snprintf( hContext->szFriendlyName, sizeof( hContext->szFriendlyName ), "%s DMR %s",	product_info.szModelName, net_info.szIp );
		else
			HxSTD_snprintf( hContext->szFriendlyName, sizeof( hContext->szFriendlyName ), "%s DMR",	product_info.szModelName);
	}
	else
	{
		if ((bIpAttachOnName == TRUE) && (bValidIP == TRUE))
			HxSTD_snprintf( hContext->szFriendlyName, sizeof( hContext->szFriendlyName ), "%s DMR %s", aModelName, net_info.szIp);
		else
			HxSTD_snprintf( hContext->szFriendlyName, sizeof( hContext->szFriendlyName ), "%s DMR", aModelName);
	}

	if (PL_DMR_InitDmr(hContext->szFriendlyName, DMR_DEFAULT_SERVICE_PORT, ePL_DMR_Scope_All) != ERR_OK) /* Initialized once */
	{
		HxLOG_Error( "Error> failed to init DMR(%s)\n", hContext->szFriendlyName);
	}

	PL_DMR_RegisterAVTEventCallback(__avt_event_cv);

	if (PL_DMR_Start() != ERR_OK)
	{
		HxLOG_Error( "Error> failed to start DMR(%s)\n", hContext->szFriendlyName);
	}

	hContext->eStatus = eDMR_STATUS_STARTED;

	/* Current Volume 값 Notify */
	HOMMA_INTEROP_OUTPUT_AUDIO_GetVolume( ( HUINT32 * )&lCurrnetVolume );
	PL_DMR_SetUPnPState( ePL_DMR_UPnPState_Volume, ( void * )&lCurrnetVolume );

	(void) PL_DMR_SetUPnPState( ePL_DMR_UPnPState_Stopped, NULL );
	if (hContext->cbDmrEvtNotify != NULL)
		hContext->cbDmrEvtNotify(eDMR_StatusEvent_START, 0, 0, 0);

	HOM_FUNC_LEAVE;
	HxLOG_Error("[ DMR ] (%s)--\n", __FUNCTION__);
	return ERR_OK;
}

HBOOL	DMR_IsStart(void)
{
	DMR_Context_t 	*hContext = __get_context();
	return ((eDMR_STATUS_STARTED == hContext->eStatus)?TRUE:FALSE);
}

HERROR	DMR_Stop( void )
{
	DMR_Context_t 	*hContext = __get_context();
	HxLOG_Error("[ DMR ] (%s)++\n", __FUNCTION__);
	HOM_FUNC_ENTER;

	if ( eDMR_STATUS_STOPPED == hContext->eStatus )
	{
		HxLOG_Error( "Error> DMR Already stopped!\n" );
		return ERR_OK;
	}

	PL_DMR_Stop();

	/* 설정된 URI및 Data 해제 */
	if (hContext->pszLocalHmsUri != NULL)
	{
		HOM_MEMFREE(hContext->pszLocalHmsUri);
		hContext->pszLocalHmsUri = NULL;
	}

	hContext->eStatus = eDMR_STATUS_STOPPED;
	hContext->bReqStart = FALSE;
	if (hContext->cbDmrEvtNotify != NULL)
		hContext->cbDmrEvtNotify(eDMR_StatusEvent_STOP, 0, 0, 0);

	HOM_FUNC_LEAVE;
	HxLOG_Error("[ DMR ] (%s)--\n", __FUNCTION__);
	return ERR_OK;
}

/* DMR Friendly Name 변경 */
HERROR	DMR_SetName( HCHAR *aModelName )
{
	HERROR hErr;
	DMR_Context_t 	*hContext = __get_context();
	HCHAR			szFriendlyName[ 128 ] = { 0, };
	HCHAR			szIpAddr[32] = { 0 };

	if( aModelName == NULL )
		return ERR_FAIL;

	if ((hContext->stNetworkInfo.bValid == TRUE) &&
		(hContext->stNetworkInfo.ulIpAddr != 0) &&
		!HxSTD_StrEmpty(hContext->stNetworkInfo.szIp))
	{
		HOM_STRNCPY_SAFE( szIpAddr, hContext->stNetworkInfo.szIp, sizeof(szIpAddr));

		/* NAME+IP의 형태로 가공  : HMS-1000S 192.168.10.32  */
		HxSTD_snprintf( szFriendlyName, sizeof( szFriendlyName ), "%s DMR %s", aModelName, szIpAddr);
	}
	else
	{
		HxLOG_Error( "Error> invalid network info!\n");

		/* Retry to get valid network info */
		hErr = PL_COMMON_GetActiveNetwork(&hContext->stNetworkInfo); // rather than PL_COMMON_GetDefaultNetwork
		if ((hErr != ERR_OK) || (hContext->stNetworkInfo.bValid == FALSE))
		{
			HxLOG_Error("Error> Cant get valid network info(err:%d, valid:%d)!\n", hErr, hContext->stNetworkInfo.bValid );
			hErr = PL_COMMON_GetDefaultNetwork(&hContext->stNetworkInfo);
			if ((hErr != ERR_OK) || (hContext->stNetworkInfo.bValid == FALSE))
			{
				HxLOG_Error( "Error> Cant get active network info(err:%d, valid:%d)!\n", hErr, hContext->stNetworkInfo.bValid );
				/* Nothing TODO... */
			}
		}

		if ((hContext->stNetworkInfo.bValid == TRUE) &&
			(hContext->stNetworkInfo.ulIpAddr != 0) &&
			!HxSTD_StrEmpty(hContext->stNetworkInfo.szIp))
		{
			HxSTD_snprintf( szFriendlyName, sizeof( szFriendlyName ), "%s DMR %s", aModelName, szIpAddr);
		}
		else
		{
			HxSTD_snprintf( szFriendlyName, sizeof( szFriendlyName ), "%s DMR", aModelName);
		}
	}

	HxLOG_Print( "[%s:%d] SetDmrName( %s )\n", __FUNCTION__, __LINE__,  szFriendlyName );

	return  PL_DMR_SetName( szFriendlyName );
}

HERROR	DMR_SetUPnPStatus( DMR_UPnPState_e eState, void *pValue )
{
	return PL_DMR_SetUPnPState((PL_DMR_UPnPState_e)eState, pValue );
}

HERROR	DMR_GetUPnPStatus( DMR_UPnPState_e *eState)
{
	PL_DMR_UPnPState_e ePl_State = 0;

	PL_DMR_GetUpnpState(&ePl_State);

	*eState = (DMR_UPnPState_e)ePl_State;

	return ERR_OK;
}


HERROR	DMR_RegisterUPnPActionCallback(DMR_UPnPActionCallback_t cb)
{
	DMR_Context_t 	*hContext = __get_context();

	hContext->action_cb = cb;

	return ERR_OK;
}

HERROR	DMR_UnregisterUPnPActionCallback(void)
{
	DMR_Context_t 	*hContext = __get_context();

	hContext->action_cb = NULL;

	return ERR_OK;
}

HERROR	DMR_GetCurrentMedia(CO_MediaInstence_t **pItem)
{
	return PL_DMR_GetCurrentItem(pItem);
}

HERROR	DMR_SetMediaDuration(HUINT64 ullTime)
{
	return PL_DMR_SetMediaDuration(ullTime);
}

HERROR	DMR_SetMediaPlayingPosition(HUINT64 ullTime)
{
	return PL_DMR_SetMediaPlayingPosition(ullTime);
}

/* Fling용 URI 및 MetaData를 Set 할수 있는 API */
HERROR	DMR_SetFlingInfomation( HCHAR *pUri, HCHAR *pCurrentMeta )
{
	return  PL_DMR_SetFlingInfo( pUri, pCurrentMeta );
}

/* 현재 설정된 Fling용 URI를 확인하기 위한 함수. 호출부에서 메모리 해제 해야 함 !!!! */
HCHAR* DMR_GetFlingURI( void )
{
	return PL_DMR_GetFlingURI();
}

/* 현재 설정된 Fling용 Meta를 확인하기 위한 함수. 호출부에서 메모리 해제 해야 함 !!!! */
HCHAR* DMR_GetFlingMeta( void )
{
	return PL_DMR_GetFlingMeta();
}
/* end od file */
