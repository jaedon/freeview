/**************************************************************
*	@file	homma_interop.h
*	http://www.humaxdigital.com
*	@author			humax
*	@brief			HOMMA / interoperation supporting instead
*					of using APK
**************************************************************/

/*
* â“’ 2011-2012 Humax Co., Ltd.
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

#ifndef __HOMMA_INTEROP_H__
#define __HOMMA_INTEROP_H__

/*
 * Author : tjjang
 * Description : This is for replacing apk methods
 *  - HOMMA_INTEROP prefix is used for external methods
 *  - s_HOMMA_INTEROP prefix is used to represent static methods
 */

/////////////////////////////////////////////////////////////////////////////////////////////
// if you want to use XAPI instead of APPKIT API, please disable HOMMA_USE_APPKIT_DIRECTLY //
/////////////////////////////////////////////////////////////////////////////////////////////
//#define HOMMA_USE_APPKIT_DIRECTLY
/////////////////////////////////////////////////////////////////////////////////////////////



#ifdef HOMMA_USE_APPKIT_DIRECTLY

#include <apk.h>

// META RECORD
#define HOMMA_INTEROP_MetaRec_Handle_t ApkMetaRec_Handle_t
#define HOMMA_INTEROP_MetaRecord_Notifier_t ApkMetaRecord_Notifier_t
#define eHOMMA_INTEROP_META_REC_DO_SCAN eAPK_META_REC_DO_SCAN
#define eHOMMA_INTEROP_META_REC_DO_COPY eAPK_META_REC_DO_COPY
#define eHOMMA_INTEROP_META_REC_DO_DELETE eAPK_META_REC_DO_DELETE
#define eHOMMA_INTEROP_META_REC_EVENT_ADDED eAPK_META_REC_EVENT_ADDED
#define eHOMMA_INTEROP_META_REC_EVENT_DELETED eAPK_META_REC_EVENT_DELETED
#define eHOMMA_INTEROP_META_REC_EVENT_UPDATED eAPK_META_REC_EVENT_UPDATED
#define eHOMMA_INTEROP_META_REC_EVENT_START eAPK_META_REC_EVENT_START
#define eHOMMA_INTEROP_META_REC_EVENT_LOAD eAPK_META_REC_EVENT_LOAD

// META SERVICE
#define HOMMA_INTEROP_MetaService_Notifier_t ApkMetaService_Notifier_t

// NETWORK
#define HOMMA_INTEROP_NETIFADDR_t APKS_NETIFADDR_t
#define HOMMA_INTEROP_NETDEVINFO_t APKS_NETDEVINFO_t
#define HOMMA_INTEROP_NETWORK_CbHandler APKS_NETWORK_CbHandler
#define eHOMMA_INTEROP_NETWORK_STATUS_CON_SUCCESS eNETWORK_STATUS_CON_SUCCESS
#define eHOMMA_INTEROP_NETWORK_STATUS_CON_AUTOIP_SUCCESS eNETWORK_STATUS_CON_AUTOIP_SUCCESS
#define eHOMMA_INTEROP_NETWORK_STATUS_WIFI_WPS_SUCCESS eNETWORK_STATUS_WIFI_WPS_SUCCESS
#define eHOMMA_INTEROP_NETWORK_STATUS_CABLE_LINK_DISCONNECTED eNETWORK_STATUS_CABLE_LINK_DISCONNECTED
#define eHOMMA_INTEROP_NETWORK_STATUS_WIFI_USB_EXTRACTED eNETWORK_STATUS_WIFI_USB_EXTRACTED
#define eHOMMA_INTEROP_NETWORK_STATUS_CON_FAIL eNETWORK_STATUS_CON_FAIL
#define eHOMMA_INTEROP_NETWORK_STATUS_CON_AUTOIP_FAIL eNETWORK_STATUS_CON_AUTOIP_FAIL
#define eHOMMA_INTEROP_NETWORK_STATUS_CABLE_LINK_CONNECTED eNETWORK_STATUS_CABLE_LINK_CONNECTED
#define eHOMMA_INTEROP_NETWORK_STATUS_WIFI_AP_CONNECT_SUCCESS eNETWORK_STATUS_WIFI_AP_CONNECT_SUCCESS
#define eHOMMA_INTEROP_NETWORK_STATUS_WIFI_AP_CONNECT_FAIL eNETWORK_STATUS_WIFI_AP_CONNECT_FAIL
#define eHOMMA_INTEROP_NETWORK_STATUS_WIFI_WPS_FAIL eNETWORK_STATUS_WIFI_WPS_FAIL
#define eHOMMA_INTEROP_NETWORK_STATUS_LINK_UPDATED eNETWORK_STATUS_LINK_UPDATED
#define eHOMMA_INTEROP_INTERFACE_TYPE_LAN eINTERFACE_TYPE_LAN
#define eHOMMA_INTEROP_INTERFACE_TYPE_WLAN eINTERFACE_TYPE_WLAN

#else

#include <hlib.h>
#include <oapi.h>
#include <dapi.h>

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * network
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/*struct
APKS_NETIFADDR_t
APKS_NETDEVINFO_t
*/
//#define HOMMA_INTEROP_NETWORK_AP_ESSID 48
//#define HOMMA_INTEROP_NETWORK_AP_KEY 64

#define HOMMA_INTEROP_NETWORK_MAX_IP4ADDR (16)
#define HOMMA_INTEROP_NETWORK_MAX_MACADDR (20)
#define HOMMA_INTEROP_NETWORK_MAX_IP6ADDR (40)

typedef void (*hintopNetwork_Notifier_t) (HINT32 event, HINT32 idx);

typedef enum {
	eHOMMA_INTEROP_INTERFACE_TYPE_LAN = 0,
	eHOMMA_INTEROP_INTERFACE_TYPE_WLAN,
	eHOMMA_INTEROP_INTERFACE_TYPE_PPPOE,
} HOMMA_INTEROP_NET_IF_Type_e;

typedef	enum
{
	//	CONNECT_STATUS
	eHOMMA_INTEROP_NET_STATUS_CON_SUCCESS,
	eHOMMA_INTEROP_NET_STATUS_CON_FAIL,
	eHOMMA_INTEROP_NET_STATUS_CON_AUTOIP_SUCCESS,
	eHOMMA_INTEROP_NET_STATUS_CON_AUTOIP_FAIL,
	eHOMMA_INTEROP_NET_STATUS_INTERNET_CONNECTED,
	eHOMMA_INTEROP_NET_STATUS_INTERNET_DISCONNECTED,
	//	CABLE LINK_STATUS
	eHOMMA_INTEROP_NET_STATUS_CABLE_LINK_CONNECTED,
	eHOMMA_INTEROP_NET_STATUS_CABLE_LINK_DISCONNECTED,
	//	WIFI_USB_STATUS
	eHOMMA_INTEROP_NET_STATUS_WIFI_USB_INSERTED,
	eHOMMA_INTEROP_NET_STATUS_WIFI_USB_EXTRACTED,
	//	WIFI_SCANNING_STATUS
	eHOMMA_INTEROP_NET_STATUS_WIFI_SCANNING_START,
	eHOMMA_INTEROP_NET_STATUS_WIFI_SCANNING_END,
	eHOMMA_INTEROP_NET_STATUS_WIFI_SCANNING_OUTPUT,
	eHOMMA_INTEROP_NET_STATUS_WIFI_SCANNING_FAIL,

	eHOMMA_INTEROP_NET_STATUS_WIFI_KEY_INVALID,
	//	WIFI_CONNECT_STATUS
	eHOMMA_INTEROP_NET_STATUS_WIFI_AP_CONNECT_SUCCESS,
	eHOMMA_INTEROP_NET_STATUS_WIFI_AP_CONNECT_FAIL,
	eHOMMA_INTEROP_NET_STATUS_WIFI_WPS_SUCCESS,
	eHOMMA_INTEROP_NET_STATUS_WIFI_WPS_FAIL,
	eHOMMA_INTEROP_NET_STATUS_WIFI_AP_DISCONNECTED,
	eHOMMA_INTEROP_NET_STATUS_WIFI_AP_RECONNECTED,
	eHOMMA_INTEROP_NET_STATUS_WIFI_AP_CONNECT_STOP,

	eHOMMA_INTEROP_NET_STATUS_LINK_UPDATED,

	eHOMMA_INTEROP_NET_STATUS_DHCPD_START,
	eHOMMA_INTEROP_NET_STATUS_DHCPD_STOP,

	eHOMMA_INTEROP_NET_STATUS_BT_LIST_UPDATED,
	eHOMMA_INTEROP_NET_STATUS_BT_CONNECTED,
	eHOMMA_INTEROP_NET_STATUS_BT_CONNECT_FAILED,
	eHOMMA_INTEROP_NET_STATUS_BT_PIN_REQUESTED,
	eHOMMA_INTEROP_NET_STATUS_BT_PASSKEY,
	eHOMMA_INTEROP_NET_STATUS_BT_DISCONNECTED,
	eHOMMA_INTEROP_NET_STATUS_BT_PROFILE_STARTED,
	eHOMMA_INTEROP_NET_STATUS_BT_PROFILE_STOPPED,
	eHOMMA_INTEROP_NET_STATUS_BT_AUDIO_MUTE,

	// for pppoe
	eHOMMA_INTEROP_NET_STATUS_PPPOE_CONNECTED,
	eHOMMA_INTEROP_NET_STATUS_PPPOE_CONNECT_FAILED,
	eHOMMA_INTEROP_NET_STATUS_PPPOE_DISCONNECTED,
	eHOMMA_INTEROP_NET_STATUS_PPPOE_IN_PROGRESS,

	eHOMMA_INTEROP_NET_CONCHK_ARP_SUCCESS,
	eHOMMA_INTEROP_NET_CONCHK_ARP_FAIL,
	eHOMMA_INTEROP_NET_CONCHK_RESOLV_SUCCESS,
	eHOMMA_INTEROP_NET_CONCHK_RESOLV_FAIL,

	eHOMMA_INTEROP_NET_STATUS_UNKNOWN,

} HOMMA_INTEROP_NET_STATUS_e;

// have to check -> appkitÀ» »ç¿ëÇÏÁö ¾Ê´Â °æ¿ì structure »õ·Î Á¤ÀÇ ÇØ¾ß ÇÔ
typedef struct {
	HOMMA_INTEROP_NET_IF_Type_e eType;
	HBOOL bConnected;
	HBOOL bNetActive;
	HBOOL bRunning;
	HCHAR szMacAddr[HOMMA_INTEROP_NETWORK_MAX_MACADDR];
	HCHAR szDev_name[16];         /* interface name */
} HOMMA_INTEROP_NETDEVINFO_t;

// have to check -> appkitÀ» »ç¿ëÇÏÁö ¾Ê´Â °æ¿ì structure »õ·Î Á¤ÀÇ ÇØ¾ß ÇÔ
typedef struct {
	HBOOL bDhcpOn;
	HBOOL bDnsAuto;

	HULONG ulIpAddr;
	HULONG ulNetmask;
	HULONG ulGateway;
	HULONG ulDns1;
	HULONG ulDns2;

	HBOOL bIpv6; /* whether using ipv6 address or not */
	HBOOL bEnable; /* temp */
	HUINT32 bIpv6Mode; /* whether using dhcp client or not */
	HBOOL bDns6; /* whether static dns or not */
	HCHAR szIp6_addr[HOMMA_INTEROP_NETWORK_MAX_IP6ADDR]; /* ipv6 address */
	HCHAR szIp6_gateway[HOMMA_INTEROP_NETWORK_MAX_IP6ADDR]; /* ipv6 gateway address */
	HCHAR szIp6_dns_addr_1[HOMMA_INTEROP_NETWORK_MAX_IP6ADDR]; /* ipv6 1st dns address */
	HCHAR szIp6_dns_addr_2[HOMMA_INTEROP_NETWORK_MAX_IP6ADDR]; /* ipv6 2nd dns address */
	HUINT32 nPrefix; /* ipv6 prefix */
	HULONG ulTunnelStartAddr; /* tunnel start ipv4 address ipv6 in ipv4 */
	HULONG ulTunnelEndAddr; /* tunnel end ipv4 address ipv6 in ipv4 */
} HOMMA_INTEROP_NETIFADDR_t;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * M E T A
 - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * meta svc
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
typedef void (*hintopMetaSvc_Notifier_t) ( void );

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * meta schedule
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
#if 0 // have to check
#define HOMMA_INTEROP_META_SCHED_MAX_CONFLICT 10
#define HOMMA_INTEROP_META_SCHED_MAX_SERIESID 128
#define HOMMA_INTEROP_MAX_CONFLICT_TPNUM 4

typedef enum {
	eHOMMA_INTEROP_APK_METASCH_CONFTYPE_None,

	eHOMMA_INTEROP_APK_METASCH_CONFTYPE_Tp,
	eHOMMA_INTEROP_APK_METASCH_CONFTYPE_OverCapacity,
	eHOMMA_INTEROP_APK_METASCH_CONFTYPE_View,
	eHOMMA_INTEROP_APK_METASCH_CONFTYPE_Duplicated,
	eHOMMA_INTEROP_APK_METASCH_CONFTYPE_PaddingTime,
	eHOMMA_INTEROP_APK_METASCH_CONFTYPE_Alternative,
	eHOMMA_INTEROP_APK_METASCH_CONFTYPE_RfOta,
	eHOMMA_INTEROP_APK_METASCH_CONFTYPE_DataOnly,
	eHOMMA_INTEROP_APK_METASCH_CONFTYPE_Unknown,

	eHOMMA_INTEROP_APK_METASCH_CONFTYPE_EndOfCase
} HOMMA_INTEROP_META_SCH_Conflict_e;

typedef struct {
	HOMMA_INTEROP_META_SCH_Conflict_e type;
	HUINT32 length[HOMMA_INTEROP_MAX_CONFLICT_TPNUM];
	HINT32 uidList[HOMMA_INTEROP_MAX_CONFLICT_TPNUM][HOMMA_INTEROP_META_SCHED_MAX_CONFLICT];
	HUINT32 alt_length;
	HINT32 alt_uidList[HOMMA_INTEROP_META_SCHED_MAX_CONFLICT];
	HINT32 alt_evtuidList[HOMMA_INTEROP_META_SCHED_MAX_CONFLICT];
} HOMMA_INTEROP_SCHED_CONFLICT_t;

typedef struct HOMMA_INTEROP_META_SCHEDULER_t_* HOMMA_INTEROP_MetaSched_Handle_t;
typedef void(* HOMMA_INTEROP_MetaSched_Listener_t) (HOMMA_INTEROP_MetaSched_Handle_t self, HINT32 event, HUINT32 ulUid, DxSCHEDULE_t *pstDxSchedule, HOMMA_INTEROP_SCHED_CONFLICT_t *pstConflict, void *userdata);
#endif

#endif

// e x t e r n   m e t h o d s
//
// extern methods
//
// b e l o w


/* ############################################################################
                          _
                         | |
  ___ __   __ ___  _ __  | |_    ___  ___   _ __ ___   _ __ ___    ___   _ __
 / _ \\ \ / // _ \| '_ \ | __|  / __|/ _ \ | '_ ` _ \ | '_ ` _ \  / _ \ | '_ \
|  __/ \ V /|  __/| | | || |_  | (__| (_) || | | | | || | | | | || (_) || | | |
 \___|  \_/  \___||_| |_| \__|  \___|\___/ |_| |_| |_||_| |_| |_| \___/ |_| |_|
 xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx  event common  xxxxxxxxx
############################################################################ */
#ifdef HOMMA_USE_APPKIT_DIRECTLY
extern HUINT32 HOMMA_INTEROP_EVENT_Dispatch(void);
#else
extern HERROR	HOMMA_INTEROP_Init (void);
#endif


/* ############################################################################
___  ___           _  _                 _
|  \/  |          | |(_)               | |
| .  . |  ___   __| | _   __ _   _ __  | |  __ _  _   _
| |\/| | / _ \ / _` || | / _` | | '_ \ | | / _` || | | |
| |  | ||  __/| (_| || || (_| | | |_) || || (_| || |_| |
\_|  |_/ \___| \__,_||_| \__,_| | .__/ |_| \__,_| \__, |
                                | |                __/ |
                                |_|               |___/
 xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx  media play  xxxxxxxxx
############################################################################ */
#ifdef HOMMA_USE_APPKIT_DIRECTLY
//APK_MEDIA_PLAY_Init();
extern HERROR HOMMA_INTEROP_MEDIA_PLAY_Init (void);
#else
//APK_MEDIA_PLAY_Init();
extern HERROR HOMMA_INTEROP_MEDIA_PLAY_Init (OxMediaPlay_ActivatedCb_t pfActivated);
#endif

//APK_MEDIA_PLAY_GetRequestId (ulViewId, eOxMP_MEDIATYPE_LIVE, &ulSvcUID);
extern HERROR HOMMA_INTEROP_MEDIA_PLAY_GetRequestId(HUINT32 ulViewId, OxMediaPlay_MediaType_e eType, HUINT32 *pulRequestId);
//APK_MEDIA_PLAY_GetPlayType (ulViewId, &eMediaType);
extern HERROR HOMMA_INTEROP_MEDIA_PLAY_GetPlayType (HUINT32 ulViewId, OxMediaPlay_MediaType_e *peType);
//APK_MEDIA_PLAY_GetPlayState (ulViewId, &eState);
extern HERROR HOMMA_INTEROP_MEDIA_PLAY_GetPlayState (HUINT32 ulViewId, OxMediaPlay_PlayState_e *peState);
//APK_MEDIA_PLAY_GetURI(ulViewId, szPlayingUrl);
extern HERROR HOMMA_INTEROP_MEDIA_PLAY_GetURI (HUINT32 ulViewId, HCHAR *pszURI);
//APK_MEDIA_PLAY_GetMainViewId(&ulViewId);
extern HERROR HOMMA_INTEROP_MEDIA_PLAY_GetMainViewId (HUINT32 *pulViewId);
//APK_MEDIA_PLAY_SetPlayPosition(ulViewId, p2);
extern HERROR HOMMA_INTEROP_MEDIA_PLAY_SetPlayPosition (HUINT32 ulViewId, HUINT32 ulPosition);
//APK_MEDIA_PLAY_RegisterNotifier(ulViewId, __hom_dmr_media_info_notifier, NULL);
extern HERROR HOMMA_INTEROP_MEDIA_PLAY_RegisterNotifier (HUINT32 ulViewId, OxMediaPlay_Notifier_t pfNotifier, void *pvUserData);
//APK_MEDIA_PLAY_UnregisterNotifier(ulViewId, __hom_dmr_media_info_notifier, NULL);
extern HERROR HOMMA_INTEROP_MEDIA_PLAY_UnregisterNotifier (HUINT32 ulViewId, OxMediaPlay_Notifier_t pfNotifier, void *pvUserData);


/* ############################################################################
  __  _  _
 / _|(_)| |
| |_  _ | |  ___  _ __ ___    __ _  _ __
|  _|| || | / _ \| '_ ` _ \  / _` || '__|
| |  | || ||  __/| | | | | || (_| || |
|_|  |_||_| \___||_| |_| |_| \__, ||_|
                              __/ |
                             |___/
 xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx  filemgr storage  xxxxxxxxx
############################################################################ */
//APK_FILEMGR_STORAGE_Init();
extern HERROR HOMMA_INTEROP_FILEMGR_STORAGE_Init(void);
// APK_FILEMGR_STORAGE_GetDevTypeString(pstInfo->eDevType));
extern const HCHAR* HOMMA_INTEROP_FILEMGR_STORAGE_GetDevTypeString(const DxStorage_DevType_e	eDevType);
// APK_FILEMGR_STORAGE_GetFsString(pstInfo->astPartitionInfos[i].eFsType));
extern const HCHAR* HOMMA_INTEROP_FILEMGR_STORAGE_GetFsString(const DxStorage_FsType_e	eFsType);
// APK_FILEMGR_STORAGE_GetList(storageFilter, &devList, &devCnt);
extern HERROR HOMMA_INTEROP_FILEMGR_STORAGE_GetList(HUINT32 ulFilter, DxStorage_Info_t **pastInfoList, HUINT32 *pulDevNum);
// APK_FILEMGR_STORAGE_FreeList(devList, devCnt);
extern HERROR HOMMA_INTEROP_FILEMGR_STORAGE_FreeList(DxStorage_Info_t *pastInfoList, HUINT32 ulDevNum);
// APK_FILEMGR_STORAGE_RegisterNotifier(__system_storage_notifier);
extern void HOMMA_INTEROP_FILEMGR_STORAGE_RegisterNotifier(DxStorageNotifier_t pfnObjectHandler);

////////////////////////////////////////////////////////////////////////////////////////////


/* ############################################################################
               _                  _                       _  _
              | |                | |                     | |(_)
  ___   _   _ | |_  _ __   _   _ | |_    __ _  _   _   __| | _   ___
 / _ \ | | | || __|| '_ \ | | | || __|  / _` || | | | / _` || | / _ \
| (_) || |_| || |_ | |_) || |_| || |_  | (_| || |_| || (_| || || (_) |
 \___/  \__,_| \__|| .__/  \__,_| \__|  \__,_| \__,_| \__,_||_| \___/
                   | |
                   |_|
 xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx  output audio  xxxxxxxxx
############################################################################ */
//APK_OUTPUT_AUDIO_Init()
extern HERROR HOMMA_INTEROP_OUTPUT_AUDIO_Init(void);
//APK_OUTPUT_AUDIO_GetVolumeMax(&ulVolumeRange);
extern HERROR HOMMA_INTEROP_OUTPUT_AUDIO_GetVolumeMax(HUINT32 * outMaxVolume);
//APK_OUTPUT_AUDIO_GetMasterMute(&bSysMute);
extern HERROR HOMMA_INTEROP_OUTPUT_AUDIO_GetMasterMute(HBOOL * outMute);
//APK_OUTPUT_AUDIO_SetVolume(nVolume);
extern HERROR HOMMA_INTEROP_OUTPUT_AUDIO_SetVolume(HUINT32 volume);
//APK_OUTPUT_AUDIO_SetMasterMute((HBOOL)bMute);
extern HERROR HOMMA_INTEROP_OUTPUT_AUDIO_SetMasterMute(HBOOL mute);
//APK_OUTPUT_AUDIO_GetVolume(&ulVolume);
extern HERROR HOMMA_INTEROP_OUTPUT_AUDIO_GetVolume(HUINT32 * outVolume);

/* ############################################################################
                  _
                 | |
 _ __ ___    ___ | |_  __ _
| '_ ` _ \  / _ \| __|/ _` |
| | | | | ||  __/| |_| (_| |
|_| |_| |_| \___| \__|\__,_|
 xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx  meta record & antenna & svn  xxxxxxxxx
############################################################################ */

//APK_META_RECORD_Init();
extern HBOOL HOMMA_INTEROP_META_RECORD_Init(void);
// APK_META_RECORD_EnableLoadEvent();
extern void HOMMA_INTEROP_META_RECORD_EnableLoadEvent(void);
//APK_META_RECORD_GetByUrl((HCHAR*)args);
extern DxRecListData_t * HOMMA_INTEROP_META_RECORD_GetByUrl (const HCHAR *url);
//APK_META_RECORD_Release(pRecData);
extern void HOMMA_INTEROP_META_RECORD_Release (DxRecListData_t *recData);
#ifdef HOMMA_USE_APPKIT_DIRECTLY
//APK_META_RECORD_AddNotifier(__system_record_notifier,  NULL);
extern void HOMMA_INTEROP_META_RECORD_AddNotifier(HOMMA_INTEROP_MetaRecord_Notifier_t notifier, void *userdata);
#else
//APK_META_RECORD_AddNotifier(__system_record_notifier,  NULL);
extern void HOMMA_INTEROP_META_RECORD_AddNotifier(OxMetaRecord_Notifier_t notifier, void *userdata);
#endif
#ifdef HOMMA_USE_APPKIT_DIRECTLY
//APK_META_ANTENNA_Init();
extern HBOOL HOMMA_INTEROP_META_ANTENNA_Init(HBOOL bMainApp);
#else
//APK_META_ANTENNA_Init();
extern HBOOL HOMMA_INTEROP_META_ANTENNA_Init(void);
#endif
//APK_META_ANTENNA_DeInit();
extern HBOOL HOMMA_INTEROP_META_ANTENNA_DeInit(void);
//APK_META_ANTENNA_InfoFree(pAntInfo);
extern void HOMMA_INTEROP_META_ANTENNA_InfoFree (DxAntInfo_t *p);
//APK_META_ANTENNA_Load();
extern HERROR HOMMA_INTEROP_META_ANTENNA_Load (void);
//APK_META_ANTENNA_Count();
extern HUINT32 HOMMA_INTEROP_META_ANTENNA_Count (void);
//APK_META_ANTENNA_GetUidAt(i)
extern HINT32 HOMMA_INTEROP_META_ANTENNA_GetUidAt (HINT32 index);
//APK_META_ANTENNA_GetInfo(APK_META_ANTENNA_GetUidAt(i));
extern DxAntInfo_t * HOMMA_INTEROP_META_ANTENNA_GetInfo (HINT32 uid);
#ifdef HOMMA_USE_APPKIT_DIRECTLY
//APK_META_SVC_Init(FALSE);
extern HBOOL HOMMA_INTEROP_META_SVC_Init (HBOOL bMainApp);
#else
//APK_META_SVC_Init(FALSE);
extern HBOOL HOMMA_INTEROP_META_SVC_Init (void);
#endif
//APK_META_SVC_DeInit();
extern HBOOL HOMMA_INTEROP_META_SVC_DeInit(void);
//APK_META_SVC_GetTransponder(item->tsuid);
extern DxTransponder_t * HOMMA_INTEROP_META_SVC_GetTransponder (HINT32 nUid);
//APK_META_SVC_GetNetwork(pChannel->ts->netuid);
extern DxNetwork_t * HOMMA_INTEROP_META_SVC_GetNetwork (HINT32 nUid);
//APK_META_SVC_GetLogoUrl(item->uid, pLogo);
extern DxChannelLogo_t * HOMMA_INTEROP_META_SVC_GetLogoUrl(HINT32 nUid, DxChannelLogo_t *pstBuf);
//APK_META_SVC_GetServiceList();
extern HxVector_t * HOMMA_INTEROP_META_SVC_GetServiceList (void);
//APK_META_SVC_GetService(ulSvcUID);
extern DxService_t * HOMMA_INTEROP_META_SVC_GetService (HINT32 nUid);
// nothing matching with APPKIT
extern HINT32 HOMMA_INTEROP_META_SVC_GetChannelNumber (HINT32 nSvcUid);
#ifdef HOMMA_USE_APPKIT_DIRECTLY
//APK_META_SVC_SetListener(_plChannelList_EventListener, (void *)NULL);
extern HERROR HOMMA_INTEROP_META_SVC_SetListener (HOMMA_INTEROP_MetaService_Notifier_t pfOnUpdate, void *pvUserdata);
#else
//APK_META_SVC_SetListener(_plChannelList_EventListener, (void *)NULL);
extern HERROR HOMMA_INTEROP_META_SVC_SetListener (hintopMetaSvc_Notifier_t pfOnUpdate);
#endif
//APK_META_SVC_LoadupdatedList();
extern HERROR HOMMA_INTEROP_META_SVC_LoadupdatedList (void);
//APK_META_SVC_Delete(pstService);
extern void HOMMA_INTEROP_META_SVC_Delete (void *pvObject);
#if 0 // have to check
//APK_META_SCHEDULE_AddListener()
extern void HOMMA_INTEROP_META_SCHEDULE_AddListener (HOMMA_INTEROP_MetaSched_Handle_t hMeta, HOMMA_INTEROP_MetaSched_Listener_t listener, void *userdata);
#endif

/* ############################################################################
                  _          __
                 (_)        / _|
 ___  _   _  ___  _  _ __  | |_  ___
/ __|| | | |/ __|| || '_ \ |  _|/ _ \
\__ \| |_| |\__ \| || | | || | | (_) |
|___/ \__, ||___/|_||_| |_||_|  \___/
       __/ |
      |___/
 xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx  sysinfo tuner & stb  xxxxxxxx
############################################################################ */
//APK_SYSINFO_TUNER_Init();
extern HERROR HOMMA_INTEROP_SYSINFO_TUNER_Init(void);
//APK_SYSINFO_TUNER_GetSignalLevel(nIndex, &nSignalLevel);
extern HERROR HOMMA_INTEROP_SYSINFO_TUNER_GetSignalLevel(HINT32 nIndex, HINT32 *pnSignalLevel);
//APK_SYSINFO_TUNER_GetSignalQuality(nIndex, &nSignalQuality);
extern HERROR HOMMA_INTEROP_SYSINFO_TUNER_GetSignalQuality(HINT32 nIndex, HINT32 *pnSignalQuality);
//APK_SYSINFO_TUNER_IsLocked(nIndex, pbLocked);
extern HERROR HOMMA_INTEROP_SYSINFO_TUNER_IsLocked(HINT32 nIndex, HBOOL *pbLocked);
//APK_SYSINFO_STB_Init();
extern HERROR HOMMA_INTEROP_SYSINFO_STB_Init(void);
//APK_SYSINFO_STB_GetManufacturerName(pInfo->szManufacturer, PL_COMMON_NAME_SHORT);
extern HERROR HOMMA_INTEROP_SYSINFO_STB_GetManufacturerName(HCHAR *pszManufacturerName, HINT32 nMax);
//APK_SYSINFO_STB_GetModelName(pInfo->szModelName, PL_COMMON_NAME_MIDDLE);
extern HERROR HOMMA_INTEROP_SYSINFO_STB_GetModelName(HCHAR *pszModelName, HINT32 nMax);
//APK_SYSINFO_STB_GetSerialNumber(pInfo->szSerialNumber, PL_COMMON_NAME_SHORT);
extern HERROR HOMMA_INTEROP_SYSINFO_STB_GetSerialNumber(HCHAR *pszSerialNo, HINT32 nMax);
//APK_SYSINFO_STB_GetChipId(szChipId, 32);
extern HERROR HOMMA_INTEROP_SYSINFO_STB_GetChipId(HCHAR *pszChipId, HINT32 nMax);

/* ############################################################################
              _                          _
             | |                        | |
 _ __    ___ | |_ __      __ ___   _ __ | | __
| '_ \  / _ \| __|\ \ /\ / // _ \ | '__|| |/ /
| | | ||  __/| |_  \ V  V /| (_) || |   |   <
|_| |_| \___| \__|  \_/\_/  \___/ |_|   |_|\_\
  xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx  network  xxxxxxxx
############################################################################ */
//APK_NETWORK_Init();
extern HBOOL HOMMA_INTEROP_NETWORK_Init (void);
//APK_NETWORK_CONF_GetMacAddr(i, macBuf, sizeof(macBuf));
extern HERROR HOMMA_INTEROP_NETWORK_CONF_GetMacAddr (HINT32 nIndex, HCHAR *pszMacAddr, HINT32 nBufSize);
//APK_NETWORK_CONF_AddrN2A(devAddr.ulDns2, pIface->szDns2, sizeof(pIface->szDns2));
extern HERROR HOMMA_INTEROP_NETWORK_CONF_AddrN2A (HULONG ulAddr, HCHAR *pszAddr, HINT32 nbufSize);
//APK_NETWORK_CONF_GetNumOfNetDev(&devCnt);
extern HERROR HOMMA_INTEROP_NETWORK_CONF_GetNumOfNetDev(HINT32 *pnCount);
//APK_NETWORK_CONF_GetDefaultDev(&dev_idx);
extern HERROR HOMMA_INTEROP_NETWORK_CONF_GetDefaultDev(HINT32 *pnIndex);
//APK_NETWORK_CONF_IsNetworkAvailable(&bIsAvailable)
extern HERROR HOMMA_INTEROP_NETWORK_CONF_IsNetworkAvailable(HBOOL *isEnable);
// it is called from APK_NETWORK_CONF_GetAddrInfo
extern HERROR HOMMA_INTEROP_NETWORK_CONF_AddrA2N (HCHAR *pszAddr, HULONG *pulAddr);
//APK_NETWORK_CONF_GetAddrInfo(didx, &tNetAddr);
extern HERROR HOMMA_INTEROP_NETWORK_CONF_GetAddrInfo (HINT32 nIndex, HOMMA_INTEROP_NETIFADDR_t *pstNetAddr);
//APK_NETWORK_CONF_GetDevInfo(dev_idx, &dev_info);
extern HERROR HOMMA_INTEROP_NETWORK_CONF_GetDevInfo (HINT32 nIndex, HOMMA_INTEROP_NETDEVINFO_t *pstDevInfo);
#ifdef HOMMA_USE_APPKIT_DIRECTLY
//APK_NETWORK_RegisterNotifier(__system_network_event_notifier);
extern void HOMMA_INTEROP_NETWORK_RegisterNotifier(HOMMA_INTEROP_NETWORK_CbHandler pfnObjectHandler);
#else
//APK_NETWORK_RegisterNotifier(__system_network_event_notifier);
extern void HOMMA_INTEROP_NETWORK_RegisterNotifier(hintopNetwork_Notifier_t pfnObjectHandler);
#endif

/* ############################################################################
                                     _
                                    (_)
 ___   ___  ___  _ __    __ _  _ __  _   ___
/ __| / __|/ _ \| '_ \  / _` || '__|| | / _ \
\__ \| (__|  __/| | | || (_| || |   | || (_) |
|___/ \___|\___||_| |_| \__,_||_|   |_| \___/
 xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx  scenario  xxxxxxxx
############################################################################ */
//APK_SCENARIO_Init();
extern HERROR HOMMA_INTEROP_SCENARIO_Init(void);
//APK_SCENARIO_GetAntennaConnection(&eAntCon);
extern HERROR HOMMA_INTEROP_SCENARIO_GetAntennaConnection(DxAntennaConnection_e *peConnectionType);


#endif

