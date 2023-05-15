/**************************************************************
*	@file	homma_system.c
*	http://www.humaxdigital.com
*	@author			humax
*	@brief			DLNA / HOMMA
**************************************************************/

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

#define	________________Header_Files_______________________________
#include <vkernel.h>
#include <hlib.h>

#include <hapi.h>
#include <papi.h>
#include <uapi.h>

#include "svc_common.h"
#include "homma_system.h"
#include "homma_bus.h"

#define ________________Typedef____________________________________
/**
 * @struct SYSTEM_Context_t
 * @brief HOMMA_SYSTEM 내부적으로 사용하는 Context.
 */
typedef struct
{
	HBOOL bInited;
	BUS_Handle	hMsgBusHandle;
	HOMMA_BUS_Target_e eSelf;
	HUINT32     semid;

	CO_NET_Info_t 	stNetInfo;
	CO_NET_Iface_t  stIfaces[NETWORK_INTERFACE_NUM_MAX];

	CO_STORAGE_Info_t stStorageInfo;
	CO_STORAGE_Mount_t stMounts[STORAGE_MOUNT_NUM_MAX];
} SYSTEM_Context_t;

#define ________________Static_Variables___________________________
static SYSTEM_Context_t s_stSystemContext = { 0 };

/*
 TRUE ---> shutdown done
 FLASE ---> operation
*/
static HBOOL s_bIsOperation = TRUE;

#define ________________Internal_APIs_Prototype____________________
static HERROR	__system_network_notify_status( HOMMA_BUS_Event_e eIfaceEvent, HINT32 nIfaceIdx );
static HERROR	__system_storage_convert_mountInfo( DxStorage_Info_t * pstStorage, HUINT32 ulIndex , CO_STORAGE_Mount_t *pstMountInfo );
static HBOOL	__system_storage_is_supportable_storage_type(DxStorage_DevType_e eDevType);
static HBOOL	__system_storage_is_scanable_storage_flag(DxStorage_DevType_e devType, HUINT32 ulFlag);

extern void PL_DLNA_IP_Changed(void);

#define ________________Internal_Functions_________________________
/**
 * @fn  static SYSTEM_Context_t * __system_getContext( void )
 * @brief HOMMA_SYSTEM 내부 Context를 얻을 수 있는 함수.
 */
static SYSTEM_Context_t * __system_getContext( void )
{
	return &s_stSystemContext;
}

/**
 * @fn static	void	__system_init( SYSTEM_Context_t *pSystemCtx)
 * @brief HOMMA_SYSTEM 모듈의 내부 초기화 함수.
 * @param pSystemCtx 내부에서 사용하는 Context.
 */
static	void	__system_init( SYSTEM_Context_t *pSystemCtx)
{
	CO_STORAGE_Info_t *pStoInfo = &pSystemCtx->stStorageInfo;
	CO_NET_Info_t *pNetInfo = &pSystemCtx->stNetInfo;

	if (pSystemCtx->bInited == TRUE)
	{
		HxLOG_Error("[HOMMA_SYSTEM] Error : Already initialized system!\n");
		return;
	}

	pSystemCtx->eSelf = eBUS_TARGET_SYS;
	HxSEMT_Create(&pSystemCtx->semid, "homma_system", 0);

	HxSTD_MemSet(pNetInfo, 0x0, sizeof(CO_NET_Info_t) );
	pNetInfo->bValidDev = FALSE;
	pNetInfo->pstIfaces = pSystemCtx->stIfaces; // pNetInfo->pstIfaces = NULL;

	HxSTD_MemSet(pStoInfo, 0x00, sizeof(CO_STORAGE_Info_t) );
	pStoInfo->nDevCnt = 0;
	pStoInfo->nMountCnt = 0;
	pStoInfo->nDefaultIdx = (-1);
	pStoInfo->pstMounts = pSystemCtx->stMounts; // pStoInfo->pstMounts = NULL;

	pSystemCtx->bInited = TRUE;
	return;
}

static HERROR	__system_bus_event_sender(HOMMA_BUS_Event_e evt, void *data, HUINT32 size)
{
	SYSTEM_Context_t 	*pSystemCtx = __system_getContext();
	HOMMA_BUS_Msg_t 	busMsg = { 0 };

	memset(&busMsg, 0, sizeof(HOMMA_BUS_Msg_t));
	busMsg.eMsgType = eBUS_MSGTYPE_EVENT;
	busMsg.uEnum.eEvt = evt;
	busMsg.eDataType = eBUS_DATATYPE_USER_DEF;
	busMsg.bNeedFree = TRUE;

	busMsg.nDataSize = size;
	busMsg.pvData = HLIB_STD_MemDup(data, size);

	HOMMA_BUS_SendMsg( eBUS_TARGET_MOD_ALL, pSystemCtx->hMsgBusHandle, &busMsg );

	return ERR_OK;
}

/*-----------------------------------------------------------------------------
 *  Network Event
 *-----------------------------------------------------------------------------*/
static void __system_print_ms_dev_info( DxStorage_Info_t *pstInfo)
{
	HINT32	i;

	if (pstInfo == NULL)
	{
		HxLOG_Error("[HOMMA_SYSTEM] Error : NULL DxStorage arg\n");
		return;
	}

	HxLOG_Print("================ Storage Info ================ \n");
	HxLOG_Print("ulDeviceId  : [%d]\n", pstInfo->ulDeviceId);
	HxLOG_Print("DevType     : [%s]\n", HOMMA_INTEROP_FILEMGR_STORAGE_GetDevTypeString(pstInfo->eDevType));
	HxLOG_Print("szMountPath : [%s]\n", pstInfo->szMountPath);
	HxLOG_Print("szDevName   : [%s] \n", pstInfo->szDevName);
	HxLOG_Print("szLabel     : [%s]\n", pstInfo->szLabel);
	HxLOG_Print("szUUID      : [%s]\n", pstInfo->szUUID);
	HxLOG_Print("ulFlags     : [0x%x]\n", pstInfo->ulFlags);
	HxLOG_Print("	%s \n", DxSTORAGE_CHECK_FLAG(pstInfo->ulFlags, eDxSTORAGE_FLAG_CONNECTED));
	HxLOG_Print("	%s \n", DxSTORAGE_CHECK_FLAG(pstInfo->ulFlags, eDxSTORAGE_FLAG_MOUNTED));
	HxLOG_Print("	%s \n", DxSTORAGE_CHECK_FLAG(pstInfo->ulFlags, eDxSTORAGE_FLAG_WRITEPROTECTED));
	HxLOG_Print("	%s \n", DxSTORAGE_CHECK_FLAG(pstInfo->ulFlags, eDxSTORAGE_FLAG_SECURED));
	HxLOG_Print("	%s \n", DxSTORAGE_CHECK_FLAG(pstInfo->ulFlags, eDxSTORAGE_FLAG_INTERNAL));
	HxLOG_Print("	%s \n", DxSTORAGE_CHECK_FLAG(pstInfo->ulFlags, eDxSTORAGE_FLAG_PVRDEVICE));
	HxLOG_Print("	%s \n", DxSTORAGE_CHECK_FLAG(pstInfo->ulFlags, eDxSTORAGE_FLAG_NEEDFORMAT));
	HxLOG_Print("	%s \n", DxSTORAGE_CHECK_FLAG(pstInfo->ulFlags, eDxSTORAGE_FLAG_PAIRED));
	HxLOG_Print("	%s \n", DxSTORAGE_CHECK_FLAG(pstInfo->ulFlags, eDxSTORAGE_FLAG_DEFAULTHDD));
	HxLOG_Print("	%s \n", DxSTORAGE_CHECK_FLAG(pstInfo->ulFlags, eDxSTORAGE_FLAG_SUBPVRDEVICE));

	HxLOG_Print("\n--------Partition Info--------\n");
	for (i=0; i < pstInfo->nPartitionNum ; i++)
	{
		HxLOG_Print("\n   [%d] \n",i);
		HxLOG_Print("	bAvailable   : [%d]\n", pstInfo->astPartitionInfos[i].bAvailable);
		HxLOG_Print("	FsType       : [%s]\n",HOMMA_INTEROP_FILEMGR_STORAGE_GetFsString(pstInfo->astPartitionInfos[i].eFsType));
		HxLOG_Print("	nPvrIdx      : [%d]\n", pstInfo->astPartitionInfos[i].nPvrIdx);
		HxLOG_Print("	szMountPath  : [%s]\n", pstInfo->astPartitionInfos[i].szMountPath);
		HxLOG_Print("	szTsrPath    : [%s]\n", pstInfo->astPartitionInfos[i].szTsrPath);
		HxLOG_Print("	szVolumeName : [%s]\n", pstInfo->astPartitionInfos[i].szVolumeName);
		HxLOG_Print("	szMediaPath  : [%s]\n", pstInfo->astPartitionInfos[i].szMediaPath);
		HxLOG_Print("	szMediaVideo : [%s]\n", pstInfo->astPartitionInfos[i].szMediaVideo);
		HxLOG_Print("	szMediaAudio : [%s]\n", pstInfo->astPartitionInfos[i].szMediaAudio);
		HxLOG_Print("	szMediaPic   : [%s]\n", pstInfo->astPartitionInfos[i].szMediaPic);
		HxLOG_Print("	szRecordPath : [%s]\n", pstInfo->astPartitionInfos[i].szRecordPath);
		HxLOG_Print("	ulStartUnit  : [%d], ulEndUnit : [%d]\n", pstInfo->astPartitionInfos[i].ulStartUnit, pstInfo->astPartitionInfos[i].ulEndUnit);
		HxLOG_Print("	ulFlags      : [0x%x]\n", pstInfo->astPartitionInfos[i].ulFlags);
		HxLOG_Print("	   %s \n", DxSTORAGE_CHECK_FLAG(pstInfo->astPartitionInfos[i].ulFlags, eDxSTORAGE_FLAG_MOUNTED));
		HxLOG_Print("	   %s \n", DxSTORAGE_CHECK_FLAG(pstInfo->astPartitionInfos[i].ulFlags, eDxSTORAGE_FLAG_WRITEPROTECTED));
		HxLOG_Print("	   %s \n", DxSTORAGE_CHECK_FLAG(pstInfo->astPartitionInfos[i].ulFlags, eDxSTORAGE_FLAG_SECURED));
		HxLOG_Print("	   %s \n", DxSTORAGE_CHECK_FLAG(pstInfo->astPartitionInfos[i].ulFlags, eDxSTORAGE_FLAG_PVRDEVICE));
		HxLOG_Print("	   %s \n", DxSTORAGE_CHECK_FLAG(pstInfo->astPartitionInfos[i].ulFlags, eDxSTORAGE_FLAG_DEFAULTHDD));
		HxLOG_Print("	   %s \n", DxSTORAGE_CHECK_FLAG(pstInfo->astPartitionInfos[i].ulFlags, eDxSTORAGE_FLAG_SUBPVRDEVICE));
	}
	HxLOG_Print("===========================================\n");
}

/**
 * @fn  static void __system_network_convert_to_netInfo( void )
 * @brief APPKIT Network정보를 HOMMA_SYSTEM 내부에서 사용하는 CO_NET_Info_t 구조로 변환하다.
 */
static void __system_network_convert_to_netInfo( void )
{
	HERROR hError = ERR_OK;
	SYSTEM_Context_t *pSystemCtx = __system_getContext();
	CO_NET_Info_t *pNetInfo = &pSystemCtx->stNetInfo;
	CO_NET_Iface_t *pIface = NULL;
	HOMMA_INTEROP_NETDEVINFO_t 	devInfo = { 0 };
	HOMMA_INTEROP_NETIFADDR_t 	devAddr = { 0 };
	HCHAR	macBuf[20] = { 0 };
	HINT32 devCnt = 0;
	HINT32 defaultIdx = 0;
	HINT32 i = 0;
	HxLOG_Error("[HOMMA_SYSTEM] ++%s(%d)\n", __FUNCTION__, __LINE__);

	hError = HOMMA_INTEROP_NETWORK_CONF_GetNumOfNetDev( &devCnt );
	if (hError != ERR_OK)
	{
		HxLOG_Error("[HOMMA_SYSTEM] %s(%d) Error> Cant get devCount!\n", __FUNCTION__, __LINE__);
		return;
	}

	pNetInfo->nDevCnt = devCnt;
	for ( i = 0; i < devCnt && i < NETWORK_INTERFACE_NUM_MAX ; i++ )
	{
		/* init */
		pIface = &pSystemCtx->stIfaces[i];

		pIface->nIndex = i;
		HxSTD_MemSet( &devInfo, 0x00, sizeof(HOMMA_INTEROP_NETDEVINFO_t) );
		HxSTD_MemSet( &devAddr, 0x00, sizeof(HOMMA_INTEROP_NETIFADDR_t) );
		HxSTD_MemSet( macBuf, 0x00, sizeof(macBuf) );

		hError = HOMMA_INTEROP_NETWORK_CONF_GetDevInfo( i, &devInfo);
		if (hError != ERR_OK)
		{
			HxLOG_Error("Error> HOMMA_INTEROP_NETWORK_CONF_GetDevInfo()\n");
		}
		hError = HOMMA_INTEROP_NETWORK_CONF_GetAddrInfo( i, &devAddr);
		if (hError != ERR_OK)
		{
			HxLOG_Error("Error> HOMMA_INTEROP_NETWORK_CONF_GetAddrInfo()\n");
		}
		hError = HOMMA_INTEROP_NETWORK_CONF_GetMacAddr( i, macBuf, sizeof(macBuf) );
		if (hError != ERR_OK)
		{
			HxLOG_Error("Error> HOMMA_INTEROP_NETWORK_CONF_GetMacAddr()\n");
		}

		switch( devInfo.eType )
		{
			case eHOMMA_INTEROP_INTERFACE_TYPE_LAN :
				pIface->eType = eNET_IF_WIRED;
				break;
			case eHOMMA_INTEROP_INTERFACE_TYPE_WLAN :
				pIface->eType = eNET_IF_WIRELESS;
				break;
			default :
				pIface->eType = eNET_IF_UNKNOWN;
				break;
		}

		HxSTD_StrNCpy( pIface->szName, devInfo.szDev_name, sizeof(pIface->szName)-1 );
		HxSTD_StrNCpy( pIface->szMacAddr, devInfo.szMacAddr, sizeof(pIface->szMacAddr)-1 );

		pIface->bConnected = devInfo.bConnected;
		pIface->bEnabled = devInfo.bRunning;
		pIface->bIpSetup = devInfo.bNetActive;

		/* Save IP */
		pIface->ulIpAddr   = devAddr.ulIpAddr;
		pIface->ulNetmask  = devAddr.ulNetmask;
		pIface->ulGateway  = devAddr.ulGateway;

		/* IPv4 */
		pIface->bIPv4_Enable = TRUE; // 현재 지원하지 않는 필드.
		HOMMA_INTEROP_NETWORK_CONF_AddrN2A( devAddr.ulIpAddr, pIface->szIpAddr, sizeof(pIface->szIpAddr) );
		HOMMA_INTEROP_NETWORK_CONF_AddrN2A( devAddr.ulNetmask, pIface->szNetmask, sizeof(pIface->szNetmask) );
		HOMMA_INTEROP_NETWORK_CONF_AddrN2A( devAddr.ulGateway, pIface->szGateway, sizeof(pIface->szGateway) );
		HOMMA_INTEROP_NETWORK_CONF_AddrN2A( devAddr.ulDns1, pIface->szDns1, sizeof(pIface->szDns1) );
		HOMMA_INTEROP_NETWORK_CONF_AddrN2A( devAddr.ulDns2, pIface->szDns2, sizeof(pIface->szDns2) );

		if (pIface->ulIpAddr == 0x0) /* Also check string */
		{
			HxLOG_Error("[HOMMA_SYSTEM] %s(%d) Error> dev(%d, %s) has invalid network(0x%x, %s)!\n", __FUNCTION__, __LINE__, i, devInfo.szDev_name, pIface->ulIpAddr, pIface->szIpAddr);
			pIface->bValidIP = FALSE;
		}

		/* IPv6 */
		pIface->bIPv6_Enable = FALSE; // 현재 지원하지 않는 필드.
		HxSTD_StrNCpy( pIface->szIpAddr_v6, devAddr.szIp6_addr, sizeof(pIface->szIpAddr_v6)-1 );
		HxSTD_StrNCpy( pIface->szGateway_v6, devAddr.szIp6_gateway, sizeof(pIface->szGateway_v6)-1 );
		HxSTD_StrNCpy( pIface->szDns1_v6, devAddr.szIp6_dns_addr_1, sizeof(pIface->szDns1_v6)-1 );
		HxSTD_StrNCpy( pIface->szDns2_v6, devAddr.szIp6_dns_addr_2, sizeof(pIface->szDns2_v6)-1 );

		HxLOG_Print("[HOMMA_SYSTEM] Convert!! Index : %d, DevName : %s \n", i, devInfo.szDev_name);
	}

	HOMMA_INTEROP_NETWORK_CONF_GetDefaultDev( &defaultIdx );
	if ( pNetInfo->nDefIdx != defaultIdx )
	{
		__system_network_notify_status( eBUS_EVENT_SYS_NET_IF_SWITCHING, defaultIdx );
	}
	pNetInfo->nDefIdx = defaultIdx;

	if (1)
	{
		for ( i = 0; i < devCnt && i < NETWORK_INTERFACE_NUM_MAX ; i++ )
		{
			/* init */
			pIface = &pSystemCtx->stIfaces[i];
			HxLOG_Error("[HOMMA_SYSTEM] [%d/%d] - check[IP, 0x%x, %s], results[c:%d, e:%d, s/a:%d]\n",
					i, devCnt, devAddr.ulIpAddr, pIface->szIpAddr,
					pIface->bConnected, pIface->bEnabled, pIface->bIpSetup);
		}
	}
}

/**
 * @fn static HERROR __system_storage_notify_removedMount( const APKS_MS_DEVINFO_t *pstDevInfo )
 * @brief Network Event를 CO_NET_Iface_t 데이터와 함꼐 다른 모듈에게 알린다.
 */
static HERROR __system_network_notify_status( HOMMA_BUS_Event_e eIfaceEvent, HINT32 nIfaceIdx )
{
	SYSTEM_Context_t *pSystemCtx = __system_getContext();
	CO_NET_Iface_t *pIface = NULL;
	HOMMA_BUS_Msg_t busMsg = { 0 };

	if (s_bIsOperation == FALSE)
	{
		HxLOG_Error("\nCurrent power status is not [ePAMA_STATUS_Operation]!! Do not notify network event!!\n\n");
		return ERR_FAIL;
	}

	switch ( eIfaceEvent )
	{
		case eBUS_EVENT_SYS_NET_IF_ONLINE :
		case eBUS_EVENT_SYS_NET_IF_OFFLINE :
		case eBUS_EVENT_SYS_NET_IF_SWITCHING :
			break;
		default :
			return ERR_FAIL;
	}

	memset(&busMsg, 0, sizeof(HOMMA_BUS_Msg_t));

	busMsg.eMsgType		= eBUS_MSGTYPE_EVENT;
	busMsg.uEnum.eEvt	= eIfaceEvent;
	busMsg.eDataType	= eBUS_DATATYPE_USER_DEF;
	busMsg.bNeedFree	= TRUE;

	pIface = (CO_NET_Iface_t *)HLIB_STD_MemCalloc(sizeof(CO_NET_Iface_t));
	HxSTD_MemCopy( pIface, &pSystemCtx->stIfaces[nIfaceIdx] , sizeof(CO_NET_Iface_t) );
	busMsg.nDataSize = sizeof(CO_NET_Iface_t);
	busMsg.pvData = pIface;
	HxLOG_Error("[HOMMA_SYSTEM] %s(%d) Send event(%d) for network(devidx:%d, IP:0x%x,%s)!\n", __FUNCTION__, __LINE__, eIfaceEvent, nIfaceIdx, pIface->ulIpAddr, pIface->szIpAddr);

	HOMMA_BUS_SendMsg( eBUS_TARGET_MOD_ALL, pSystemCtx->hMsgBusHandle, &busMsg );
	return ERR_OK;
}

/**
 * @fn static void	__system_network_event_notifier( HINT32 eEvent, HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3 )
 * @brief APK_SQCNetwork에 등록된 이벤트 Callback 함수.
 */
#ifdef HOMMA_USE_APPKIT_DIRECTLY
static void	__system_network_event_notifier( HINT32 eEvent, HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3 )
{
	HINT32 devIdx = (HINT32)ulParam1;
	HOMMA_BUS_Event_e eIfEvent = eBUS_EVENT_UNKNOWN;
	HxLOG_Error("[HOMMA_SYSTEM] [NetEvent:%d]\n", eEvent);

	switch ( eEvent )
	{
		case eHOMMA_INTEROP_NETWORK_STATUS_CON_SUCCESS :
		case eHOMMA_INTEROP_NETWORK_STATUS_CON_AUTOIP_SUCCESS :
		case eHOMMA_INTEROP_NETWORK_STATUS_WIFI_WPS_SUCCESS :
			eIfEvent = eBUS_EVENT_SYS_NET_IF_ONLINE;
			break;
		case eHOMMA_INTEROP_NETWORK_STATUS_CABLE_LINK_DISCONNECTED :
		case eHOMMA_INTEROP_NETWORK_STATUS_WIFI_USB_EXTRACTED :
			eIfEvent = eBUS_EVENT_SYS_NET_IF_OFFLINE;
			break;

		// CON_FAIL 와 CON_AUTOIP_FAIL이 반드시 OFF Line은 아니다.
		// 특정 NIC가 연결이 안되는 경우에, 연결이 가능한 NIC가 있을 수 있다.
		// IP는 갱신할 필요성 있어서 처리한다. ON/OFF Line 판정은 기존 코드를 유지한다.
		case eHOMMA_INTEROP_NETWORK_STATUS_CON_FAIL :
		case eHOMMA_INTEROP_NETWORK_STATUS_CON_AUTOIP_FAIL :
			eIfEvent = eBUS_EVENT_UNKNOWN;
			break;

		/* 다른 이벤트는 필요 없음. */
		case eHOMMA_INTEROP_NETWORK_STATUS_CABLE_LINK_CONNECTED :
		case eHOMMA_INTEROP_NETWORK_STATUS_WIFI_AP_CONNECT_SUCCESS :
		case eHOMMA_INTEROP_NETWORK_STATUS_WIFI_AP_CONNECT_FAIL :
		case eHOMMA_INTEROP_NETWORK_STATUS_WIFI_WPS_FAIL :
		case eHOMMA_INTEROP_NETWORK_STATUS_LINK_UPDATED:
		default :
			return ;
	}

	HxLOG_Error("[HOMMA_SYSTEM] received network event(idx:%d, evt:%d,%d)\n", devIdx, eEvent, eIfEvent);

	if(eBUS_EVENT_UNKNOWN != eIfEvent)
	{
		__system_network_convert_to_netInfo();
		__system_network_notify_status( eIfEvent, devIdx );
	}

	/* mxDlna_NIC_change() must be used as an auxiliary for mis-catching IP change(). */
	if ((eEvent == eNETWORK_STATUS_CON_SUCCESS) ||
		/* (eEvent == eNETWORK_STATUS_WIFI_AP_CONNECT_SUCCESS) ||
		(eEvent == eNETWORK_STATUS_WIFI_AP_CONNECT_FAIL) ||
		(eEvent == eNETWORK_STATUS_WIFI_AP_DISCONNECTED) || */
		(eEvent == eNETWORK_STATUS_CON_FAIL))
	{
		/* Use Nima's network event minimally because of the possibility of passing too many events */
		PL_DLNA_IP_Changed();
	}
	return;
}
#else
static void	__system_network_event_notifier( HINT32 eEvent, HINT32 idx )
{
	HINT32 devIdx = 0;
	HOMMA_BUS_Event_e eIfEvent = eBUS_EVENT_UNKNOWN;
	HxLOG_Error("[HOMMA_SYSTEM] [NetEvent:%d]\n", eEvent);

	devIdx = idx;

	switch ( eEvent )
	{
		case eHOMMA_INTEROP_NET_STATUS_CON_SUCCESS :
		case eHOMMA_INTEROP_NET_STATUS_CON_AUTOIP_SUCCESS :
		case eHOMMA_INTEROP_NET_STATUS_WIFI_WPS_SUCCESS :
			eIfEvent = eBUS_EVENT_SYS_NET_IF_ONLINE;
			break;
		case eHOMMA_INTEROP_NET_STATUS_CABLE_LINK_DISCONNECTED :
		case eHOMMA_INTEROP_NET_STATUS_WIFI_USB_EXTRACTED :
			eIfEvent = eBUS_EVENT_SYS_NET_IF_OFFLINE;
			break;

		// CON_FAIL 와 CON_AUTOIP_FAIL이 반드시 OFF Line은 아니다.
		// 특정 NIC가 연결이 안되는 경우에, 연결이 가능한 NIC가 있을 수 있다.
		// IP는 갱신할 필요성 있어서 처리한다. ON/OFF Line 판정은 기존 코드를 유지한다.
		case eHOMMA_INTEROP_NET_STATUS_CON_FAIL :
		case eHOMMA_INTEROP_NET_STATUS_CON_AUTOIP_FAIL :
			eIfEvent = eBUS_EVENT_UNKNOWN;
			break;

		/* 다른 이벤트는 필요 없음. */
		default :
			return ;
	}

	HxLOG_Error("[HOMMA_SYSTEM] received network event(idx:%d, evt:%d,%d)\n", devIdx, eEvent, eIfEvent);

	if(eBUS_EVENT_UNKNOWN != eIfEvent)
	{
		__system_network_convert_to_netInfo();
		__system_network_notify_status( eIfEvent, devIdx );
	}

	/* mxDlna_NIC_change() must be used as an auxiliary for mis-catching IP change(). */
	if ((eEvent == eHOMMA_INTEROP_NET_STATUS_CON_SUCCESS) ||
		/* (eEvent == eHOMMA_INTEROP_NET_STATUS_WIFI_AP_CONNECT_SUCCESS) ||
		(eEvent == eHOMMA_INTEROP_NET_STATUS_WIFI_AP_CONNECT_FAIL) ||
		(eEvent == eHOMMA_INTEROP_NET_STATUS_WIFI_AP_DISCONNECTED) || */
		(eEvent == eHOMMA_INTEROP_NET_STATUS_CON_FAIL))
	{
		/* Use Nima's network event minimally because of the possibility of passing too many events */
		PL_DLNA_IP_Changed();
	}
	return;
}
#endif


/*-----------------------------------------------------------------------------
 *  Storage Event
 *-----------------------------------------------------------------------------*/
static HERROR __system_storage_convert_mountInfo( DxStorage_Info_t * pstStorage, HUINT32 ulIndex , CO_STORAGE_Mount_t *pstMountInfo )
{
	DxStorage_PartitionInfo_t * pstPartition = NULL;
	HUINT64 ullTotalSize = 0;

	if ( NULL == pstStorage || NULL == pstMountInfo)
	{
		HxLOG_Error("[HOMMA_SYSTEM] Error : NULL DxStorage_Info_t arg\n");
		return ERR_FAIL;
	}

	if (__system_storage_is_supportable_storage_type(pstStorage->eDevType) == FALSE)
	{
		HxLOG_Warning("Not supported DevType:%d\n", pstStorage->eDevType);
		return ERR_FAIL;
	}

	pstPartition = &pstStorage->astPartitionInfos[ulIndex];

	pstMountInfo->ulStorageId = pstStorage->ulDeviceId;
	HxSTD_StrNCpy( pstMountInfo->szUUID, pstStorage->szUUID, sizeof(pstMountInfo->szUUID)-1 );

	HLIB_STORAGE_GetPartitionSize(pstPartition->szMountPath, &ullTotalSize, NULL, NULL);

	pstMountInfo->ulTotalSizeKb = ullTotalSize;
	pstMountInfo->ulTotalSizeGb = ullTotalSize / (1024*1024);

	HxSTD_StrNCpy( pstMountInfo->szPath, pstPartition->szMountPath, sizeof(pstMountInfo->szPath)-1 );

	// Pairing 정보
	pstMountInfo->bPvrFlag		= (pstPartition->ulFlags & eDxSTORAGE_FLAG_PVRDEVICE) ? TRUE : FALSE;
	pstMountInfo->IsScanable	= __system_storage_is_scanable_storage_flag(pstStorage->eDevType, pstPartition->ulFlags);

	// media path
	HxSTD_memset(pstMountInfo->szMediaPath, 0, sizeof(HCHAR)*(MOUNT_PATH_LEN_MAX));
	if ( HxSTD_StrLen(pstPartition->szMountPath) )
	{
		HxSTD_StrNCpy( pstMountInfo->szMediaPath, pstPartition->szMountPath, sizeof(pstMountInfo->szMediaPath)-1 );
	}

	// recording path
	HxSTD_memset(pstMountInfo->szRecordPath, 0, sizeof(HCHAR)*(MOUNT_PATH_LEN_MAX));
	if ( HxSTD_StrLen(pstPartition->szRecordPath) )
	{
		HxSTD_StrNCpy( pstMountInfo->szRecordPath, pstPartition->szRecordPath, sizeof(pstMountInfo->szRecordPath)-1 );
	}

	switch ( pstStorage->eDevType )
	{
		case eDxSTORAGE_DEVTYPE_INTHDD:
			pstMountInfo->eType = eSTORAGE_TYPE_HDD;
			pstMountInfo->eInterface = eSTORAGE_IFACE_SATA;
			pstMountInfo->eLocation = eSTORAGE_LOC_INTERNAL;
			break;

		case eDxSTORAGE_DEVTYPE_DETACHABLE:
			pstMountInfo->eType = eSTORAGE_TYPE_HDD;
			pstMountInfo->eInterface = eSTORAGE_IFACE_SATA;
			pstMountInfo->eLocation = eSTORAGE_LOC_EXTERNAL;
			break;

		case eDxSTORAGE_DEVTYPE_ESATAHDD:
			pstMountInfo->eType = eSTORAGE_TYPE_HDD;
			pstMountInfo->eInterface = eSTORAGE_IFACE_SATA;
			pstMountInfo->eLocation = eSTORAGE_LOC_EXTERNAL;
			break;

		case eDxSTORAGE_DEVTYPE_USBHDD:
			pstMountInfo->eType = eSTORAGE_TYPE_HDD;
			pstMountInfo->eInterface = eSTORAGE_IFACE_USB;
			pstMountInfo->eLocation = eSTORAGE_LOC_EXTERNAL;
			break;

		case eDxSTORAGE_DEVTYPE_USBMEM:
			pstMountInfo->eType = eSTORAGE_TYPE_MEM;
			pstMountInfo->eInterface = eSTORAGE_IFACE_USB;
			pstMountInfo->eLocation = eSTORAGE_LOC_EXTERNAL;
			break;

		case eDxSTORAGE_DEVTYPE_MMC:
			pstMountInfo->eType = eSTORAGE_TYPE_MEM;
			pstMountInfo->eInterface = eSTORAGE_IFACE_SDMMC;
			pstMountInfo->eLocation = eSTORAGE_LOC_EXTERNAL;
			break;

		default:
			pstMountInfo->eType = eSTORAGE_TYPE_UNKNOWN;
			pstMountInfo->eInterface = eSTORAGE_IFACE_UNKNOWN;
			pstMountInfo->eLocation = eSTORAGE_LOC_UNKNOWN;
			break;
	}

	pstMountInfo->eDevType	= pstStorage->eDevType;
	pstMountInfo->szDevType = UAPI_FS_GetDevTypeString(pstStorage->eDevType);

	return ERR_OK;
}

/**
 * @fn  static void __system_storage_convert_storageInfo()
 * @brief APPKIT MassStorage정보를 HOMMA_SYSTEM 내부에서 사용하는 CO_STORAGE_Info_t 구조로 변환하다.
 */
static void __system_storage_update_storageInfo(void)
{
	SYSTEM_Context_t *pSystemCtx = __system_getContext();
	DxStorage_Info_t * devList = NULL;
	DxStorage_Info_t * pStorage = NULL;
	CO_STORAGE_Info_t *pStoInfo = NULL;
	CO_STORAGE_Mount_t *pMountInfo = NULL;
	HUINT32 devCnt = 0;
	HINT32 defaultIdx = 0;
	//HBOOL mediaFlag = FALSE;
	HINT32 i = 0, j = 0, k = 0;
	/* 다수의 event 발생할 수 있으므로 최소로 사용 */
	/* (eDxSTORAGE_FILTER_Internal | eDxSTORAGE_FILTER_Connected | eDxSTORAGE_FILTER_Mounted | eDxSTORAGE_FILTER_PVR | eDxSTORAGE_FILTER_Hdd); */
	HUINT32 storageFilter = (eDxSTORAGE_FILTER_Mounted | eDxSTORAGE_FILTER_Connected);
	HERROR retErr = ERR_FAIL;

	retErr = HOMMA_INTEROP_FILEMGR_STORAGE_GetList( storageFilter, &devList, &devCnt );
	if ( ERR_OK != retErr )
	{
		HxLOG_Error("[HOMMA_SYSTEM] Error> FAILED  : get HOMMA_INTEROP_FILEMGR_STORAGE_GetList\n");
		return ;
	}

	if (!devCnt)
	{
		HxLOG_Error("[HOMMA_SYSTEM] Error> Cant get valid devCount!\n");
	}

	HxSEMT_Get(pSystemCtx->semid);
	pStoInfo = &pSystemCtx->stStorageInfo;

	{
		HxLOG_Warning ( "[HOMMA_SYSTEM] Storage Count : %d !!!!\n", devCnt );

		for( i = 0; i < devCnt; i++ )
		{
			pStorage = &devList[i];

			__system_print_ms_dev_info( pStorage );

			if (pStorage != NULL)
			{
				for ( j = 0; j < pStorage->nPartitionNum; j++ )
				{
					pMountInfo = &pSystemCtx->stMounts[k];

					/* TODO : default(internal) 이외에 detachable storage 구분할 것: detachable-HDD, USB-HDD */
					if (pStorage->astPartitionInfos[j].ulFlags & eDxSTORAGE_FLAG_DEFAULTHDD)
					{
						defaultIdx = k;
					}
					__system_storage_convert_mountInfo( pStorage, j, pMountInfo );

					k++;

					if( STORAGE_MOUNT_NUM_MAX == k )
						break;
				}
			}
		}
	}

	pStoInfo->nDevCnt = devCnt;
	pStoInfo->nMountCnt = k;
	pStoInfo->nDefaultIdx = defaultIdx;

	if (devList) {
		HOMMA_INTEROP_FILEMGR_STORAGE_FreeList(devList, devCnt);
		devList = NULL;
	}

	HxSEMT_Release(pSystemCtx->semid);
}

/**
 * @brief 제거된 장치에 대한 Mount 정보를 다른 모듈에게 알린다..
 */
static HERROR __system_storage_notify_mount(HBOOL bConnect,  const DxStorage_Info_t *pstDevInfo )
{
	CO_STORAGE_Mount_t *pMountInfo = NULL;
	HINT32 i = 0;
	HINT32 partiCnt = 0;

	if (s_bIsOperation == FALSE)
	{
		HxLOG_Error("\nCurrent power status is not [ePAMA_STATUS_Operation]!! Do not notify storage event!!\n\n");
		return ERR_FAIL;
	}

	if ( pstDevInfo == NULL)
	{
		HxLOG_Error("[HOMMA_SYSTEM] Error> Invalid devInfo arg\n");
		return ERR_FAIL;
	}

	if ( pstDevInfo->nPartitionNum > DxSTORAGE_PARTITION_NUM )
	{
		HxLOG_Error("[HOMMA_SYSTEM] Error> Invalid partition num(%d)\n", pstDevInfo->nPartitionNum);
		return ERR_FAIL;
	}

	partiCnt = pstDevInfo->nPartitionNum;

	for ( i = 0; i < partiCnt; i++ )
	{
		pMountInfo = (CO_STORAGE_Mount_t *)HLIB_STD_MemCalloc( sizeof(CO_STORAGE_Mount_t) );
		if (pMountInfo != NULL)
		{
			__system_storage_convert_mountInfo( (DxStorage_Info_t *)pstDevInfo, i, pMountInfo );

			HxLOG_Print("[HOMMA_SYSTEM] %s ID : %d\n", (bConnect)?"MOUNT":"UNMOUNT", pstDevInfo->ulDeviceId);
			HxLOG_Print("[HOMMA_SYSTEM] MOUNT PATH : %s\n", pMountInfo->szPath);

			if(bConnect)
			{
				__system_bus_event_sender(eBUS_EVENT_SYS_FS_MOUNTED, pMountInfo, sizeof(CO_STORAGE_Mount_t));
			}
			else
			{
				__system_bus_event_sender(eBUS_EVENT_SYS_FS_UNMOUNTED, pMountInfo, sizeof(CO_STORAGE_Mount_t));
			}

			HOM_MEMFREE(pMountInfo);
		}
	}

	return ERR_OK;
}

void __system_storage_notifier(DxStorage_NotiType_e eType, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	HxLOG_Warning(HxANSI_COLOR_YELLOW("event(%d) nParam1 : %d, nParam2 : %x \n"), eType, nParam1, nParam2);

	// 2016/1/23 mhkang: Homme receives FormatDone event from UMMA.
	if (eDxSTORAGE_NOTI_FormatState == eType  && eDxSTORAGE_DEV_STATUS_FormatDone == nParam1)
	{
		SYSTEM_Context_t 	*pSystemCtx = __system_getContext();
		HOMMA_BUS_Msg_t 	busMsg = { 0 };

		memset(&busMsg, 0, sizeof(HOMMA_BUS_Msg_t));
		busMsg.uEnum.eEvt 	= eBUS_EVENT_SYS_MS_FORMAT_DONE;
		busMsg.eMsgType 	= eBUS_MSGTYPE_EVENT;
		busMsg.eDataType 	= eBUS_DATATYPE_USER_DEF;
		busMsg.bNeedFree 	= FALSE;
		busMsg.pvData		= (void*)nParam2;

		HOMMA_BUS_SendMsg( eBUS_TARGET_MOD_ALL, pSystemCtx->hMsgBusHandle, &busMsg );

		return;
	}

	__system_storage_update_storageInfo();

	if (eDxSTORAGE_NOTI_DevChange == eType)
	{
		/* TODO:
		 *   불필요한 call(mount failure, fast storage attach/detach) 막기 위해 homma_system에서 미리 return 처리
		 *   system context에 DMP/DMS/DMR/WOON/... 필요한 storageList 등록 필요;
		 */
		HBOOL	bConnect = (HBOOL)nParam1;
		DxStorage_Info_t	*pstInfo = (DxStorage_Info_t	*)nParam2;

		if (pstInfo == NULL)
		{
			/*
			 * NULL storageInfo can be caused by fast mount/unount action for detachable storage like USB
			 * e.g. fase USB attaching/detaching, mount fail, ...
			 * (eType=eDxSTORAGE_NOTI_PvrReady, nParam1=1, nParam2=0, nParam3=0)
			 */
			HxLOG_Error("Error> NULL DxStorage_Info!\n");
			return;
		}

		/* TODO: 필요한 storageList는 homma_system context로 관리 */
		if (__system_storage_is_supportable_storage_type(pstInfo->eDevType) == FALSE)
		{
			HxLOG_Error("DMS doesn't support '%s'(type:%d)!\n", UAPI_FS_GetDevTypeString(pstInfo->eDevType) , pstInfo->eDevType);
			return;
		}

		__system_storage_notify_mount(bConnect, pstInfo);
		__system_print_ms_dev_info( pstInfo );
	}
}

HBOOL __system_storage_is_supportable_storage_type(DxStorage_DevType_e eDevType)
{
	if (eDevType == eDxSTORAGE_DEVTYPE_INTHDD		||
		eDevType == eDxSTORAGE_DEVTYPE_DETACHABLE	||
		eDevType == eDxSTORAGE_DEVTYPE_USBHDD		||
		eDevType == eDxSTORAGE_DEVTYPE_ESATAHDD)
	{
		return TRUE;
	}

	return FALSE;
}

/* 2016/3/14 mhkang: http://svn.humaxdigital.com:3000/issues/111265#note-5 */
HBOOL	__system_storage_is_scanable_storage_flag(DxStorage_DevType_e devType, HUINT32 ulFlags)
{
	switch(devType)
	{
		case eDxSTORAGE_DEVTYPE_INTHDD		: return ((ulFlags & eDxSTORAGE_FLAG_DEFAULTHDD) != 0);
		case eDxSTORAGE_DEVTYPE_DETACHABLE	: return TRUE;
		case eDxSTORAGE_DEVTYPE_ESATAHDD	: return TRUE;
		case eDxSTORAGE_DEVTYPE_USBHDD		: return TRUE;
		default: return FALSE;
	}
}


/*-----------------------------------------------------------------------------
 *  File Event
 *-----------------------------------------------------------------------------*/
static HERROR	__system_file_notifier(HINT32 nItemState, const HCHAR *pszName, void *pvUser)
{
	CO_FileOperation_Event_t evt;

	if (NULL == pszName)
	{
		HxLOG_Error("[%s:%d] Error> args is NULL, (evt:%d) \n", __HxFILE__, __HxLINE__, nItemState);
		return ERR_FAIL;
	}
	HxLOG_Print("[%s:%d] [evt:%d, file:%s]\n", __FUNCTION__, __LINE__, nItemState, pszName);

	/* Filter here without sending event */
	HxSTD_MemSet(&evt, 0x00, sizeof(CO_FileOperation_Event_t));
	HxSTD_snprintf(evt.szPath, sizeof(evt.szPath), "%s", pszName);

	if(nItemState < 0)
	{
		evt.type = FILE_OP_TYPE_REMOVED;
		__system_bus_event_sender(eBUS_EVENT_SYS_FILE_DELETE_DONE, &evt, sizeof(CO_FileOperation_Event_t));
		HxLOG_Print("[%s:%d] FILE REMOVE - %s\n", __FUNCTION__, __LINE__, evt.szPath);
	}
	else if(nItemState == 0)
	{
		evt.type = FILE_OP_TYPE_MODIFIED;
		//__system_bus_event_sender(eBUS_EVENT_SYS_FILE_MOVE_DONE, &evt, sizeof(CO_FileOperation_Event_t));
	}
	else
	{
		evt.type = FILE_OP_TYPE_ADDED;
		__system_bus_event_sender(eBUS_EVENT_SYS_FILE_CREATE_DONE, &evt, sizeof(CO_FileOperation_Event_t));
		HxLOG_Print("[%s:%d] FILE ADD - %s\n", __FUNCTION__, __LINE__, evt.szPath);
	}

	return ERR_OK;
}

#ifdef HOMMA_USE_APPKIT_DIRECTLY
static void 	__system_record_notifier (HOMMA_INTEROP_MetaRec_Handle_t hMetaRec, HINT32 metaEvent, const HINT32 *args, void *userdata)
{
	CO_FileOperation_Event_t evt;
	HCHAR szFullPath[DxRECLIST_URL];
	DxRecListData_t *pRecData = NULL;
	HBOOL bDebug = FALSE;

	if (NULL == args)
	{
		if ((eHOMMA_INTEROP_META_REC_DO_SCAN != metaEvent) &&
			(eHOMMA_INTEROP_META_REC_EVENT_LOAD != metaEvent))
			HxLOG_Error("[%s:%d] Error> args is NULL, metaEvent(%d) \n", __HxFILE__, __HxLINE__, metaEvent);
		return;
	}
	HxLOG_Print("[%s:%d] meta[evt:%d, url:%s]\n", __HxFILE__, __HxLINE__, metaEvent, (HCHAR*)args);

	if ((metaEvent != eHOMMA_INTEROP_META_REC_EVENT_ADDED) &&
		(metaEvent != eHOMMA_INTEROP_META_REC_DO_DELETE) &&
		/* (metaEvent != eHOMMA_INTEROP_META_REC_EVENT_UPDATED) && */
		/* (metaEvent != eHOMMA_INTEROP_META_REC_EVENT_LOAD) && */
		(metaEvent != eHOMMA_INTEROP_META_REC_EVENT_DELETED))
	{
		/* TODO: eHOMMA_INTEROP_META_REC_EVENT_UPDATED 필요함: user-lock, svc-lock, ... */
		return;
	}

	pRecData = HOMMA_INTEROP_META_RECORD_GetByUrl((HCHAR*)args);
	if (pRecData)
	{
		if (bDebug == TRUE)
		{
			/* Too many reporting for eHOMMA_INTEROP_META_REC_EVENT_UPDATED */
			DLIB_Print(DxNAMEOF(DxRecListData_t), NULL, pRecData);
		}
		HxSTD_PrintToStrN(szFullPath, DxRECLIST_URL, "%s%s%s", pRecData->stMeta.szPath, pRecData->stMeta.szFileName, DxEXT_META_FILE);
		/* Make path with .hjm, not .ts if this is PVR record file */
	}
	else
	{
		HxLOG_Error("Error> Cant get record_meta(%s)!\n", (HCHAR*)args);
		return;
	}

	HxSTD_MemSet(&evt, 0x00, sizeof(CO_FileOperation_Event_t));
	HxSTD_snprintf(evt.szPath, sizeof(evt.szPath), "%s", szFullPath);

	if( pRecData != NULL)
	{
		HOMMA_INTEROP_META_RECORD_Release(pRecData);
		pRecData = NULL;
	}

	switch (metaEvent) {
	case eHOMMA_INTEROP_META_REC_EVENT_ADDED: /* completed 확인 가능?  */
		{
			evt.type = FILE_OP_TYPE_ADDED;
			__system_bus_event_sender(eBUS_EVENT_SYS_REC_ADDED, &evt, sizeof(CO_FileOperation_Event_t));
			HxLOG_Print("[%s:%d] RECORD ADD(%d) - %s\n", __FUNCTION__, __LINE__, metaEvent, evt.szPath);
		}
		break;

	case eHOMMA_INTEROP_META_REC_DO_DELETE:
	case eHOMMA_INTEROP_META_REC_EVENT_DELETED:
		{
			evt.type = FILE_OP_TYPE_REMOVED;
			__system_bus_event_sender(eBUS_EVENT_SYS_REC_DELETED, &evt, sizeof(CO_FileOperation_Event_t));
			HxLOG_Print("[%s:%d] RECORD REMOVE(%d) - %s\n", __FUNCTION__, __LINE__, metaEvent, evt.szPath);
		}
		break;

	case eHOMMA_INTEROP_META_REC_EVENT_UPDATED:
		{
			if (0) /* There are too many record_update_event, But update() event by user-lock, ... is needed to send to */
			{
				evt.type = FILE_OP_TYPE_MODIFIED;
				__system_bus_event_sender(eBUS_EVENT_SYS_REC_UPDATED, &evt, sizeof(CO_FileOperation_Event_t));
			}
		}
		break;

	case eHOMMA_INTEROP_META_REC_DO_SCAN:
	case eHOMMA_INTEROP_META_REC_DO_COPY:
	case eHOMMA_INTEROP_META_REC_EVENT_START:
	default:
		HxLOG_Warning("[%s:%d] Unknown Event(%d:0x%X)\n", __HxFILE__, __HxLINE__, metaEvent, metaEvent);
		;
	}

	return;
}
#else
static void __system_record_notifier (
						OxMetaRecord_Event_e evt
						, HINT32 count
						, HUINT32 cid
						, const DxRecListData_t *rec
						, void *unused)
{
	HxLOG_Debug("evt:%d, count:%d, cid:%u \n", evt, count, cid);

	// handle OxMetaRecord_Event_e
	switch (evt)
	{
		case eOxMETA_RECORD_ADDED:
			if(rec == NULL)
			{
				// record info load (DAPI_GetAll) for the first time
			}
			else
			{

			}
			break;
		default:
			break;
	}
}
#endif

/*-----------------------------------------------------------------------------
 *  HAMA Event
 *-----------------------------------------------------------------------------*/
static HERROR	__system_bus_signal_onSystemStateChanged(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	SYSTEM_Context_t 	*pSystemCtx = __system_getContext();
	PxSTATUS_e			ePapiStatus;
	HOMMA_BUS_Msg_t 	busMsg = { 0 };

	HxLOG_Assert(nArgc == 1);

	memset(&busMsg, 0, sizeof(HOMMA_BUS_Msg_t));
	busMsg.eMsgType 	= eBUS_MSGTYPE_EVENT;
	busMsg.eDataType 	= eBUS_DATATYPE_USER_DEF;
	busMsg.bNeedFree 	= FALSE;

	ePapiStatus = HxOBJECT_INT(apArgv[0]);
	switch(ePapiStatus)
	{
		case ePAMA_STATUS_Reboot:
			HxLOG_Print("[%s:%d] ePAMA_STATUS_Reboot \n", __FUNCTION__, __LINE__);
			busMsg.uEnum.eEvt = eBUS_EVENT_SYS_POWER_REBOOT;
			break;

		case ePAMA_STATUS_Shutdown:
			HxLOG_Print("[%s:%d] ePAMA_STATUS_Shutdown\n", __FUNCTION__, __LINE__);
			busMsg.uEnum.eEvt = eBUS_EVENT_SYS_POWER_SHUTDOWN;
			s_bIsOperation = FALSE;
			break;

		case ePAMA_STATUS_Sync:
			HxLOG_Print("[%s:%d] ePAMA_STATUS_Sync\n", __FUNCTION__, __LINE__);
			busMsg.uEnum.eEvt = eBUS_EVENT_SYS_POWER_SYNC;
			s_bIsOperation = FALSE;
			break;

		case ePAMA_STATUS_ShutdownDone:
			HxLOG_Print("[%s:%d] ePAMA_STATUS_ShutdownDone\n", __FUNCTION__, __LINE__);
			busMsg.uEnum.eEvt = eBUS_EVENT_SYS_POWER_SHUTDOWNDONE;
			s_bIsOperation = FALSE;
			break;

		case ePAMA_STATUS_Operation:
			HxLOG_Print("[%s:%d] ePAMA_STATUS_Operation\n", __FUNCTION__, __LINE__);
			busMsg.uEnum.eEvt = eBUS_EVENT_SYS_POWER_OPERATION;
			s_bIsOperation = TRUE;
			break;

		default:
			break;
	}

	if(busMsg.uEnum.eEvt)
		HOMMA_BUS_SendMsg( eBUS_TARGET_MOD_ALL, pSystemCtx->hMsgBusHandle, &busMsg );

	return ERR_OK;
}

// 2014/11/14 mhkang: (mklim) Stop DMS when homma receives "onFatoryDefault" signal.
static HERROR   __system_bus_signal_onFactoryDefault(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	SYSTEM_Context_t    *pSystemCtx = __system_getContext();
	HOMMA_BUS_Msg_t     busMsg = { 0 };

	memset(&busMsg, 0, sizeof(HOMMA_BUS_Msg_t));
	busMsg.eMsgType     = eBUS_MSGTYPE_EVENT;
	busMsg.eDataType    = eBUS_DATATYPE_USER_DEF;
	busMsg.bNeedFree    = FALSE;

	HxLOG_Print("[%s:%d] onFactoryDefault \n", __FUNCTION__, __LINE__);
	busMsg.uEnum.eEvt = eBUS_EVENT_SYS_PW_FACTORYDEFAULT;

	if(busMsg.uEnum.eEvt)
		HOMMA_BUS_SendMsg( eBUS_TARGET_MOD_ALL, pSystemCtx->hMsgBusHandle, &busMsg );

	return ERR_OK;
}

static HERROR	__system_bus_signal_onFormatOccurred(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	SYSTEM_Context_t 	*pSystemCtx = __system_getContext();
	HOMMA_BUS_Msg_t 	busMsg = { 0 };

	memset(&busMsg, 0, sizeof(HOMMA_BUS_Msg_t));
	busMsg.uEnum.eEvt 	= eBUS_EVENT_SYS_MS_FORMAT_TRY;
	busMsg.eMsgType 	= eBUS_MSGTYPE_EVENT;
	busMsg.eDataType 	= eBUS_DATATYPE_USER_DEF;
	busMsg.bNeedFree 	= FALSE;
	busMsg.pvData		= (void*)HxOBJECT_INT(apArgv[0]);	// DeviceID
						//HxOBJECT_STR(apArgv[1]);	// MountPath
	HOMMA_BUS_SendMsg( eBUS_TARGET_MOD_ALL, pSystemCtx->hMsgBusHandle, &busMsg );

	return ERR_OK;
}

/*  apk 에서 hama signal 이후 바로 event 를 주기 때문에 아래는 사용하지 않음. */
HERROR __system_bus_signal_onStorageMounted(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	return ERR_OK;
}

HERROR __system_bus_signal_onStorageUnmounted(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	return ERR_OK;
}


static	void 	__system_ext_event_listen( void )
{
	/* HAMA Signal Listen */
	HAPI_ConnectSignal("signal:onSystemStateChanged", NULL, __system_bus_signal_onSystemStateChanged);
	HAPI_ConnectSignal("signal:onFormatOccurred",     NULL, __system_bus_signal_onFormatOccurred);
	HAPI_ConnectSignal("signal:onFactoryDefault",     NULL, __system_bus_signal_onFactoryDefault);
}

/*-----------------------------------------------------------------------------
 *  HOMMA_BUS
 *-----------------------------------------------------------------------------*/
#if 0
/**
 * @fn static HINT32	__system_bus_event_handler( HINT32 event, const HOMMA_BUS_MsgEvt_t *pstMsgEvt, HOMMA_BUS_Target_e eSender )

 * @brief HOMMA_BUS API 중 HOMMA_BUS_Connect()를 통해 등록 되는 함수.
 * @param event  해당 Module의  Event를 받을 수 있음.
 * @param pstEvt 이벤트와 함께 전달되는 데이터.
 * @param eSender 보내는 Module의 Enum 값이 넘어옴.
 */

static HINT32	__system_bus_event_handler( HINT32 event, const HOMMA_BUS_MsgEvt_t *pstMsgEvt, HOMMA_BUS_Target_e eSender )
{
	return 0;
}
#endif

static HINT32	__system_bus_request_handler( HOMMA_BUS_Request_e eReq, HOMMA_BUS_Data_t *pstRetData )
{
	SYSTEM_Context_t *pSystemCtx = __system_getContext();

	if ( !pstRetData )
	{
		return eHOM_Result_Error_NullPoint;
	}

	switch ( eReq )
	{
		case eBUS_REQUEST_NET_INFO :
			{
				pstRetData->eDataType = eBUS_DATATYPE_USER_DEF;
				pstRetData->nDataSize = sizeof(CO_NET_Info_t);
				pstRetData->bNeedFree = FALSE;
				pstRetData->pvData = &pSystemCtx->stNetInfo;
			}
			break;

		case eBUS_REQUEST_STORAGE_INFO :
			{
				pstRetData->eDataType = eBUS_DATATYPE_USER_DEF;
				pstRetData->nDataSize = sizeof(CO_STORAGE_Info_t);
				pstRetData->bNeedFree = FALSE;
				pstRetData->pvData = &pSystemCtx->stStorageInfo;
			}
			break;

		default :
			break;
	}

	return eHOM_Result_Ok;
}

static HINT32	__system_bus_message_handler( HOMMA_BUS_Target_e eSender, const HOMMA_BUS_Msg_t *pstMsg )
{
	switch ( pstMsg->eMsgType )
	{
		case eBUS_MSGTYPE_EVENT :
			break;

		case eBUS_MSGTYPE_REQUEST :
			break;

		case eBUS_MSGTYPE_RESPONSE :
			break;
		default :
			break;
	}
	return 0;
}

/**
 * @fn static HINT32	__system_bus_release_handler( HOMMA_BUS_Target_e eSender, HOMMA_BUS_Msg_t *pstMsg )
 * @brief HOMMA_BUS_Register()를 통해 등록 되는 모듈의 메모리 해제 함수.
 * @param eSender 자신의 Module Enum값이 넘어옴.
 * @param pstMsg 모듈에서 HOMMA_BUS_SendMsg()를 통해 보냈던 Msg 데이터.
 */
static HINT32	__system_bus_release_handler( HOMMA_BUS_Target_e eSender, HOMMA_BUS_Msg_t *pstMsg )
{
	switch ( pstMsg->eDataType )
	{
		case eBUS_DATATYPE_IPC_RECV :
			{
				HOMMA_BUS_IpcRecv_t *pIpcRecv = pstMsg->pvData;
				if ( pIpcRecv && pIpcRecv->bMalloc && pIpcRecv->pvJson )
				{
					HLIB_JSON_Delete( pIpcRecv->pvJson );
					pIpcRecv->pvJson = NULL;
				}
			}
			break;
		default :
			break;
	}

	if ( pstMsg->bNeedFree && pstMsg->pvData )
	{
		HLIB_STD_MemFree( pstMsg->pvData );
		pstMsg->pvData = NULL;
	}
	return 0;
}

#define ________________Public_Interfaces__________________________
void	HOMMA_SYSTEM_Run( void )
{
	/* HERROR hError; */
	SYSTEM_Context_t 			*pSystemCtx = __system_getContext();
	HOMMA_BUS_HndFuncList_t 	stFuncList = { 0 };

	HxLOG_Print("HOMMA System Start!!!\n");

	__system_init(pSystemCtx);

	stFuncList.cbMsgHandleFunc = __system_bus_message_handler;
	stFuncList.cbReqHandleFunc = __system_bus_request_handler;
	stFuncList.cbRelHandleFunc = __system_bus_release_handler;

	pSystemCtx->hMsgBusHandle = HOMMA_BUS_GetHandle( eBUS_TARGET_SYS );

	HOMMA_BUS_Register( pSystemCtx->hMsgBusHandle, &stFuncList );

	/* Network Evnet */
	HOMMA_INTEROP_NETWORK_Init();
	HOMMA_INTEROP_NETWORK_RegisterNotifier( __system_network_event_notifier );
	__system_network_convert_to_netInfo();

#if defined(CONFIG_HOMMA_DMS)
	/* Mass Storage */
	HOMMA_INTEROP_FILEMGR_STORAGE_Init();

	/*
	 * TODO: HAPI, APK signal flow 체크할 것
	 */
	UAPI_FS_SetNotifier(__system_storage_notifier);
	__system_storage_update_storageInfo();

	/* Media File Event  */
	UAPI_FM_Init();
	UAPI_FM_SetGlobalListener(NULL, __system_file_notifier);

#ifdef HOMMA_USE_APPKIT_DIRECTLY
	/* PVR Record Event  */
	/*
	 * (3/28) APK_META_RECORD_EnableLoadEvent() 으로 다시 enable 함
	 *        STB 부팅 시 녹화물 리스트를 ADD로 받지 않고, 이후 실제 Complete되는 녹화물만 ADD/DELETE/... 이벤트 받음
	 * (3/14) 부팅 이전에 녹화물 단일 ADD 이벤트로 전달받은 녹화물이라도 부팅 시 다시 ADD 이벤트를 전달하고 있음.
	 *        corsair는 부팅 시 녹화물 리스트 관리를 위해서 ADD 이벤트는 필요하다고는 하나
	 *        homma는 DMS start 시 FSS scan을 하기 때문에 불필요한 동작이며,
	 *        녹화물이 많은 경우 과다한 record add 이벤트로 homma que_full 발생 가능성으로
	 *        meta event 방식 변경 이후 다시 재활성화 예정 (TODO)
	 */
	HOMMA_INTEROP_META_RECORD_Init();
	HOMMA_INTEROP_META_RECORD_EnableLoadEvent();
	HOMMA_INTEROP_META_RECORD_AddNotifier(__system_record_notifier,  NULL);
#else
	HOMMA_INTEROP_META_RECORD_AddNotifier(__system_record_notifier, NULL);
#endif

#endif

	/*  hama signal receive  */
	__system_ext_event_listen();

	/* product info */
	HOMMA_INTEROP_SYSINFO_STB_Init();

	/*  mxlib init */
	DLNA_Init();
}

// 2015/12/04 mhkang: http://svn:3000/issues/109250
HCHAR* HOMMA_SYSTEM_GetDeviceTypeStringWithMounthPath(HCHAR* MountPath, HBOOL IsFullMatched)
{
	SYSTEM_Context_t* sysContext = NULL;
	CO_STORAGE_Info_t *pStoInfo = NULL;
	HCHAR* szDevType = NULL;
	int m;

	sysContext = __system_getContext();

	HxSEMT_Get(sysContext->semid);

	pStoInfo = &sysContext->stStorageInfo;

	for (m = 0 ; m < pStoInfo->nMountCnt ; m++)
	{
		if (IsFullMatched == TRUE)
		{
			if (strcmp(pStoInfo->pstMounts[m].szPath, MountPath) == 0)
			{
				szDevType = HLIB_STD_StrDup(pStoInfo->pstMounts[m].szDevType);
				break;
			}
		}
		else
		{
			if (strncmp(pStoInfo->pstMounts[m].szPath, MountPath, strlen(pStoInfo->pstMounts[m].szPath)) == 0)
			{
				szDevType = HLIB_STD_StrDup(pStoInfo->pstMounts[m].szDevType);
				break;
			}
		}
	}

	HxSEMT_Release(sysContext->semid);

	return szDevType;
}

DxStorage_DevType_e HOMMA_SYSTEM_GetDeviceTypeWithMounthPath(HCHAR* MountPath, HBOOL IsFullMatched)
{
	SYSTEM_Context_t* sysContext = NULL;
	CO_STORAGE_Info_t *pStoInfo = NULL;
	DxStorage_DevType_e eDevType = eDxSTORAGE_DEVTYPE_NONE;
	int m;

	sysContext = __system_getContext();

	HxSEMT_Get(sysContext->semid);

	pStoInfo = &sysContext->stStorageInfo;

	for (m = 0 ; m < pStoInfo->nMountCnt ; m++)
	{
		if (IsFullMatched == TRUE)
		{
			if (strcmp(pStoInfo->pstMounts[m].szPath, MountPath) == 0)
			{
				eDevType = pStoInfo->pstMounts[m].eDevType;
				break;
			}
		}
		else
		{
			if (strncmp(pStoInfo->pstMounts[m].szPath, MountPath, strlen(pStoInfo->pstMounts[m].szPath)) == 0)
			{
				eDevType = pStoInfo->pstMounts[m].eDevType;
				break;
			}
		}
	}

	HxSEMT_Release(sysContext->semid);

	return eDevType;
}

HBOOL HOMMA_SYSTEM_AddDevice(HUINT32 DeviceID)
{
	CO_STORAGE_Info_t *pStorageInfo = NULL;
	const CO_STORAGE_Mount_t *pMountInfo = NULL;
	SYSTEM_Context_t *systemCtx =  __system_getContext();
	int i;

	HxSEMT_Get(systemCtx->semid);

	pStorageInfo = &systemCtx->stStorageInfo;
	for (i = 0 ; i < pStorageInfo->nMountCnt ; i++)
	{
		pMountInfo = &pStorageInfo->pstMounts[i];
		if (pMountInfo->ulStorageId == DeviceID)
		{
			// AddScanDirectory
			HxLOG_Info(HxANSI_COLOR_WHITE("Device(id:%d) AddScanDirectory: MediaPath=%s RecordingPath=%s\n"), DeviceID, pMountInfo->szMediaPath, pMountInfo->szRecordPath);
			__system_bus_event_sender(eBUS_EVENT_SYS_FS_MOUNTED, (void*)pMountInfo, sizeof(CO_STORAGE_Mount_t));

			HxSEMT_Release(systemCtx->semid);

			return TRUE;
		}
	}

	HxSEMT_Release(systemCtx->semid);

	HxLOG_Warning(HxANSI_COLOR_YELLOW("Unknown DeviceID:%d\n"), DeviceID);

	return FALSE;
}

HBOOL HOMMA_SYSTEM_RemoveDevice(HUINT32 DeviceID)
{
	CO_STORAGE_Info_t *pStorageInfo = NULL;
	const CO_STORAGE_Mount_t *pMountInfo = NULL;
	SYSTEM_Context_t *systemCtx =  __system_getContext();
	int i;

	HxSEMT_Get(systemCtx->semid);

	pStorageInfo = &systemCtx->stStorageInfo;
	for (i = 0 ; i < pStorageInfo->nMountCnt ; i++)
	{
		pMountInfo = &pStorageInfo->pstMounts[i];
		if (pMountInfo->ulStorageId == DeviceID)
		{
			// RemoveScanDirectory
			HxLOG_Info(HxANSI_COLOR_WHITE("Device(id:%d) RemovecanDirectory: MediaPath=%s RecordingPath=%s\n"), DeviceID, pMountInfo->szMediaPath, pMountInfo->szRecordPath);
			__system_bus_event_sender(eBUS_EVENT_SYS_FS_UNMOUNTED, (void*)pMountInfo, sizeof(CO_STORAGE_Mount_t));

			HxSEMT_Release(systemCtx->semid);

			return TRUE;
		}
	}

	HxLOG_Warning(HxANSI_COLOR_YELLOW("Unknown DeviceID:%d\n"), DeviceID);

	HxSEMT_Release(systemCtx->semid);

	return FALSE;
}


/**
	@param media_path:  /mnt/hd2/Recordings/A.hjm or /media/drive#/.recordings/B.hjm
**/
HCHAR* HOMMA_SYSTEM_GetMountPath(char* media_path)
{
	CO_STORAGE_Info_t *pStorageInfo = NULL;
	const CO_STORAGE_Mount_t *pMountInfo = NULL;
	SYSTEM_Context_t *systemCtx =  __system_getContext();
	int i;
	char* mount_path = NULL;

	HxSEMT_Get(systemCtx->semid);

	pStorageInfo = &systemCtx->stStorageInfo;
	for (i = 0 ; i < pStorageInfo->nMountCnt ; i++)
	{
		pMountInfo = &pStorageInfo->pstMounts[i];
		if (strncmp(media_path, pMountInfo->szPath, strlen(pMountInfo->szPath)) == 0)
		{
			mount_path = strdup(pMountInfo->szPath);
			break;
		}
	}

	HxSEMT_Release(systemCtx->semid);

	return mount_path;
}

#define _____END_OF_FILE_________
