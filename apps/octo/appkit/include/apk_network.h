/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  apk_network.h
	@brief

	Description:  									\n
	Module: APPKITv2			\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding ?» »ç¿ëÇÏ´Â source file ?Ô´Ï´Ù.
* MS949 encodingªòÞÅéÄª·ªÆ source fileªÇª¹¡£
* Quelldatei, die MS949-Codierung verwendet.
*
*/
/*
 * (c) 2011-2013 Humax Co., Ltd.
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
 * CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.

 * This License is effective until terminated. You may terminate this License at any time by destroying all copies
 * of the Software including all documentation. This License will terminate immediately without notice from Humax
 * to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
 * of the Software and all documentation.

 * The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
 * All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
 * in the Republic of Korea.
*/

#ifndef __APK_NETWORK_H__
#define __APK_NETWORK_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define	APKD_BLUETOOTH_DEV_NAME_LEN		64
#define	APKD_BLUETOOTH_DEV_MACADDR_LEN	20
#define APKD_BLUETOOTH_DEV_CLASS		(3)

#define	APKD_NETWORK_MAX_IP4ADDR	(16)
#define	APKD_NETWORK_MAX_MACADDR	(20)
#define APKD_NETWORK_MAX_IP6ADDR	(40)

#define	APKD_PPPOE_ID_LEN_MAX		(64)
#define APKD_PPPOE_PW_LEN_MAX		(128)
#define APKD_FW_VERSION_LEN_MAX (128)
#define APKD_WPS_PIN_LEN_MAX 		(64)

#define CHECK_INTERNET_MAX_TIME		60000	/* 60 sec */

typedef enum {
	eBLUETOOTH_STATUS_NotPaired = 0	,
	eBLUETOOTH_STATUS_Connected		,
	eBLUETOOTH_STATUS_Disconnected	,
	eBLUETOOTH_STATUS_Unknown		,
	eBLUETOOTH_STATUS_PinRequested	,
	eBLUETOOTH_STATUS_ProfileStarted
}APKE_BT_STATUS_e;

typedef enum {
	eBLUETOOTH_DEVTYPE_Misc			,
	eBLUETOOTH_DEVTYPE_Computer		,
	eBLUETOOTH_DEVTYPE_Phone		,
	eBLUETOOTH_DEVTYPE_Network		,
	eBLUETOOTH_DEVTYPE_AV			,
	eBLUETOOTH_DEVTYPE_Peripheral	,
	eBLUETOOTH_DEVTYPE_Imaging		,
	eBLUETOOTH_DEVTYPE_Unknown		,
} APKE_BT_DevType_e;

typedef enum {
	eBLUETOOTH_PROFILE_Unknown 		= 0x00000000,
	eBLUETOOTH_PROFILE_A2DP 		= 0x00000001,	/* Adavanced Audio Distribution */
	eBLUETOOTH_PROFILE_AVRCP		= 0x00000002,	/* Audio/Video Remote Control */
	eBLUETOOTH_PROFILEe_HID			= 0x00000004,	/* Human Interface Device */
	eBLUETOOTH_PROFILE_All			= 0x0000FFFF,
	eBLUETOOTH_PROFILE_Auto			= 0x80000000,
}APKE_BT_PROFILE_e;

typedef struct {
	HUINT32				ulDevId;									// device id
	HCHAR				szDevName[APKD_BLUETOOTH_DEV_NAME_LEN];		//device name (È­¸é Ç¥½Ã¿ë)
	APKE_BT_STATUS_e	eStatus;									// device state (not paired / connected / disconnected)
	HUINT8 				dev_class[APKD_BLUETOOTH_DEV_CLASS];
	APKE_BT_PROFILE_e	eSupportedProfile;
	HCHAR 				aucBdAddr[APKD_BLUETOOTH_DEV_MACADDR_LEN];
} APKS_BT_DEVICEINFO_t;

typedef	enum
{
	//	CONNECT_STATUS
	eNETWORK_STATUS_CON_SUCCESS, /* When IP configuration is set up successfully (from  DHCP server Or manual by user)*/
	eNETWORK_STATUS_CON_FAIL, /* When IP configuration is NOT set up successfully (from  DHCP server Or manual by user)*/

	eNETWORK_STATUS_CON_AUTOIP_SUCCESS, /* When IP configuration is set up successfully using Auto IP */
	eNETWORK_STATUS_CON_AUTOIP_FAIL, /* When IP configuration is NOT set up successfully using Auto IP */

	eNETWORK_STATUS_INTERNET_CONNECTED, /* After APK_NETWORK_CONF_CheckInternetConnection API is called,in case STB has an Internet connection */
	eNETWORK_STATUS_INTERNET_DISCONNECTED, /* After APK_NETWORK_CONF_CheckInternetConnection API is called,in case STB DOES NOT have an Internet connection */

	//	CABLE LINK_STATUS
	eNETWORK_STATUS_CABLE_LINK_CONNECTED, /* When LAN cable is plugged in */
	eNETWORK_STATUS_CABLE_LINK_DISCONNECTED, /* When LAN cable is unplugged */

	//	WIFI_USB_STATUS
	eNETWORK_STATUS_WIFI_USB_INSERTED, /* When USB Wifi dongle is plugged in */
	eNETWORK_STATUS_WIFI_USB_EXTRACTED, /* When USB Wifi dongle is unplugged */

	//	WIFI_SCANNING_STATUS
	eNETWORK_STATUS_WIFI_SCANNING_START,
	eNETWORK_STATUS_WIFI_SCANNING_END,
	eNETWORK_STATUS_WIFI_SCANNING_OUTPUT,
	eNETWORK_STATUS_WIFI_SCANNING_FAIL,

	eNETWORK_STATUS_WIFI_KEY_INVALID,

	//	WIFI_CONNECT_STATUS
	eNETWORK_STATUS_WIFI_AP_CONNECT_SUCCESS, /* When STB is connected successfully to an AP (not including IP setting) */
	eNETWORK_STATUS_WIFI_AP_CONNECT_FAIL, /* When STB is NOT connected successfully to an AP (not including IP setting) */

	eNETWORK_STATUS_WIFI_WPS_SUCCESS, /* When STB is connected successfully to an AP via WPS (not including IP setting) */
	eNETWORK_STATUS_WIFI_WPS_FAIL, /* When STB is NOT connected successfully to an AP via WPS (not including IP setting) */

	eNETWORK_STATUS_WIFI_WPS_PINCODE, 

	eNETWORK_STATUS_WIFI_AP_DISCONNECTED, /* When STB is disconnected from an AP */
	eNETWORK_STATUS_WIFI_AP_RECONNECTED,
	eNETWORK_STATUS_WIFI_AP_CONNECT_STOP,

	eNETWORK_STATUS_LINK_UPDATED, /*  */

	eNETWORK_STATUS_DHCPD_START,
	eNETWORK_STATUS_DHCPD_STOP,

	eNETWORK_STATUS_BT_LIST_UPDATED,
	eNETWORK_STATUS_BT_CONNECTED,
	eNETWORK_STATUS_BT_CONNECT_FAILED,
	eNETWORK_STATUS_BT_PIN_REQUESTED,
	eNETWORK_STATUS_BT_PASSKEY,
	eNETWORK_STATUS_BT_DISCONNECTED,
	eNETWORK_STATUS_BT_PROFILE_STARTED,
	eNETWORK_STATUS_BT_PROFILE_STOPPED,
	eNETWORK_STATUS_BT_AUDIO_MUTE,

	// for pppoe
	eNETWORK_STATUS_PPPOE_CONNECTED,
	eNETWORK_STATUS_PPPOE_CONNECT_FAILED,
	eNETWORK_STATUS_PPPOE_DISCONNECTED,
	eNETWORK_STATUS_PPPOE_IN_PROGRESS,

	eNETWORK_CONCHK_ARP_SUCCESS, /* After call APK_NETWORK_CONCHK_Arp API, in case STB can receive ARP responses from Gateway */
	eNETWORK_CONCHK_ARP_FAIL, /* After call APK_NETWORK_CONCHK_Arp API, in case STB can NOT receive ARP responses from Gateway */

	eNETWORK_CONCHK_RESOLV_SUCCESS, /* After call APK_NETWORK_CONCHK_ResolvDomain API, in case STB can resolve domain name */
	eNETWORK_CONCHK_RESOLV_FAIL, /* After call APK_NETWORK_CONCHK_ResolvDomain API, in case STB can NOT resolve domain name */

	eNETWORK_STATUS_UNKNOWN,

} APKE_NET_STATUS_e;

typedef enum {

	eINTERFACE_TYPE_LAN = 0,
	eINTERFACE_TYPE_WLAN,
	eINTERFACE_TYPE_PPPOE,

} APKE_NET_IF_Type_e;

typedef enum {
	eInet_Wlan_Dev_Auto,
	eInet_Wlan_Dev_Adhoc,
	eInet_Wlan_Dev_Infra,
	eInet_Wlan_Dev_Master,
	eInet_Wlan_Dev_Repeater,
	eInet_Wlan_Dev_Secondary,
	eInet_Wlan_Dev_Monitor,
	eInet_Wlan_Dev_MAX,
	eInet_Wlan_Dev_Unknown,
} APKE_INET_Wlan_Dev_NetType_e;

typedef enum {
	eInet_Wifi_Sec_Open,
	eInet_Wifi_Sec_WEP,
	eInet_Wifi_Sec_WPA,
	eInet_Wifi_Sec_WPA2,
	eInet_Wifi_Sec_WPA_WPA2,
	eInet_Wifi_Sec_Type_MAX,
	eInet_Wifi_Sec_UnKnown,
} APKE_INET_Wifi_Security_Type_e;

typedef enum {
	eInet_Wifi_Auth_Open,
	eInet_Wifi_Auth_Shared,
	eInet_Wifi_Auth_WEPauto,
	eInet_Wifi_Auth_WPAPSK,
	eInet_Wifi_Auth_WPA2PSK,
	eInet_Wifi_Auth_802_1x,
	eInet_Wifi_Auth_Type_MAX,
	eInet_Wifi_Auth_UnKnown,
} APKE_INET_Wifi_Authen_Type_e;

typedef enum tag_Wifi_encrypt {
	eINet_Wifi_Encrypt_None,
	eINet_Wifi_Encrypt_WEP_64,	/**< needs to 10 hex digits shared key */
	eINet_Wifi_Encrypt_WPA_TKIP,
	eINet_Wifi_Encrypt_WRAP,
	eINet_Wifi_Encrypt_WPA_AES,
	eINet_Wifi_Encrypt_WPA_TKIP_AES,
	eINet_Wifi_Encrypt_WEP_128,	/**< needs to 26 hex digits shared key */
	eINet_Wifi_Encrypt_Type_MAX,

	eINet_Wifi_Encrypt_WEP_64_Hex = 11,
	eINet_Wifi_Encrypt_WEP_64_Ascii,
	eInet_Wifi_Encrypt_WEP_128_Hex = 21,
	eInet_Wifi_Encrypt_WEP_128_Ascii,

	eINet_Wifi_Encrypt_Type_UnKnown,
} APKE_INet_Wifi_Encypt_e;

typedef enum {
	eNETAP_WPS_PIN,
	eNETAP_WPS_PBC,
	eNETAP_WPS_UNKNOWN,
	eNETAP_WPS_MAX
} APKE_NETAP_WPS_Type_e;

typedef enum {
	eNETWORK_WOWL_NONE				= 0x00,		/* Do not wake up on any event. */
	eNETWORK_WOWL_MAGIC_PATTERN		= 0x01,		/* Wake up on magic pattern. */
	eNETWORK_WOWL_NET_PATTERN		= 0x02,		/* Wake up on a special net pattern. */
	eNETWORK_WOWL_DISASSOC_DEAUTH	= 0x04,		/* Wake up on disassociate from AP. */
	eNETWORK_WOWL_RETROGRADE_TSF	= 0x08,		/* Wake up on retrograde tsf. */
	eNETWORK_WOWL_BCN_LOSS			= 0x10,		/* Wake up on loss of beacon. */
}APKS_NETWoWlan_Mode_t;

typedef enum {
	eNETCM_NONE = 0,
	eNETCM_OTHER,
	eNETCM_NOT_READY,
	eNETCM_NOT_SYNC,
	eNETCM_PHY_SYNC_DONE,
	eNETCM_PARAM_ACQUIRED,
	eNETCM_RANGING_COMPLETE,
	eNETCM_IP_COMPLETE,
	eNETCM_TOD_CERTIFY,
	eNETCM_SECURITY_CERTIFY,
	eNETCM_PARAM_TRAANSE_COMPLETE,
	eNETCM_REGIST_COMPLETE,
	eNETCM_OPERATION,
	eNETCM_ACCESS_DENIED,
} APKE_NETCM_Status_e;

typedef enum {
	eNETCM_DOWN_MOD_QAM64 = 0,
	eNETCM_DOWN_MOD_QAM256,
	eNETCM_DOWN_MOD_OTHER,
	eNETCM_DOWN_MOD_UNKNOWN,
} APKE_NETCM_QAMType_e;

typedef enum {
	eNETCM_DOWN_QAM = 0,
	eNETCM_DOWN_FREQ,
	eNETCM_DOWN_BER,
	eNETCM_DOWN_SNR,
	eNETCM_DOWN_SIGLEVEL,
	eNETCM_DOWN_UNKNOWN,
} APKE_NETCM_DownInfo_e;

typedef enum {
	eNETCM_UP_FREQ  = 0,
	eNETCM_UP_CHID,
	eNETCM_UP_BANDWIDTH,
	eNETCM_UP_SIGLEVEL,
	eNETCM_UP_MODUL,
	eNETCM_UP_UNKNOWN,
} APKE_NETCM_UpInfo_e;

typedef enum {
	ePPPOE_STATUS_CONNECTED = 0,
	ePPPOE_STATUS_DISCONNECTED,
	ePPPOE_STATUS_IN_PROGRESS,
	ePPPOE_STATUS_UNKNOWN,
} APKE_PPPOE_Status_e;

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef void (*APKS_NETWORK_CbHandler) ( HINT32 eEvent, HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3);

typedef struct {
	APKE_NET_IF_Type_e	 eType;
	HBOOL	bConnected;
	HBOOL	bNetActive;
	HBOOL	bRunning;
	HCHAR	szMacAddr[APKD_NETWORK_MAX_MACADDR];
	HCHAR	szDev_name[16];         /* interface name */
	HCHAR 	szFwVer[APKD_FW_VERSION_LEN_MAX];
} APKS_NETDEVINFO_t;

typedef struct {
	HBOOL	bDhcpOn;
	HBOOL	bDnsAuto;

	HULONG 	ulIpAddr;
	HULONG 	ulNetmask;
	HULONG 	ulGateway;
	HULONG 	ulDns1;
	HULONG 	ulDns2;

	HBOOL	bIpv6;                  /* whether using ipv6 address or not */
	HBOOL	bEnable;                /* Link is connected but IP address is not allocated. */
	HUINT32 bIpv6Mode;              /* whether using dhcp client or not */
	HBOOL	bDns6;                  /* whether static dns or not */
	HCHAR	szIp6_addr[APKD_NETWORK_MAX_IP6ADDR];          /* ipv6 address */
	HCHAR	szIp6_gateway[APKD_NETWORK_MAX_IP6ADDR];       /* ipv6 gateway address */
	HCHAR	szIp6_dns_addr_1[APKD_NETWORK_MAX_IP6ADDR];    /* ipv6 1st dns address */
	HCHAR	szIp6_dns_addr_2[APKD_NETWORK_MAX_IP6ADDR];    /* ipv6 2nd dns address */
	HUINT32	nPrefix;                /* ipv6 prefix */
	HULONG	ulTunnelStartAddr;      /* tunnel start ipv4 address ipv6 in ipv4 */
	HULONG	ulTunnelEndAddr;        /* tunnel end ipv4 address ipv6 in ipv4 */
	HBOOL	bActive;				/* IP Address is allocated */
} APKS_NETIFADDR_t;


typedef struct {
	HINT32 nDevIndex;
	HBOOL  bIsRunning;
} APKS_NETDHCPSERVER_Info_t;

typedef struct {
	HCHAR	szESSID[48];
	HCHAR	szMacAddr[APKD_NETWORK_MAX_MACADDR];
	HINT32	nOpMode;
	HINT32	nSecureType;
	HINT32	nAuthenType;
	HINT32	nEncryptType;
	HINT32  nWaveStrength;
	HINT32  nConnectSpeed;
	HINT32	bIsConnected;
} APKS_NETAP_Info_t;

typedef struct {
	HUINT32 dev_id;
	HCHAR 	szUserID[APKD_PPPOE_ID_LEN_MAX];
	HCHAR 	szUserPW[APKD_PPPOE_PW_LEN_MAX];
} APKS_PPPOE_Info_t;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/
HBOOL	APK_NETWORK_Init(void);
void 	APK_NETWORK_InitTask(void);

HBOOL	APK_NETWORK_DeInit(void);
void	APK_NETWORK_RegisterNotifier(APKS_NETWORK_CbHandler  pfnObjectHandler);
void	APK_NETWORK_UnRegisterNotifier(APKS_NETWORK_CbHandler  pfnObjectHandler);
void	APK_NETWORK_LED_Init(void);


HERROR	APK_NETWORK_BT_SetEnable(HBOOL bEnable);
HERROR	APK_NETWORK_BT_GetEnable(HBOOL *bEnable);
HERROR	APK_NETWORK_BT_Scan(void);
HERROR	APK_NETWORK_BT_Connect(HINT32 nIndex);
HERROR	APK_NETWORK_BT_ConnectByBdAddr(const HCHAR *szBdAddr);
HERROR	APK_NETWORK_BT_Disconnect(HINT32 nIndex);
HERROR	APK_NETWORK_BT_DisconnectByBdAddr(const HCHAR *szBdAddr);
HERROR	APK_NETWORK_BT_Remove(HINT32 nIndex);
HERROR	APK_NETWORK_BT_RemoveByBdAddr(const HCHAR *szBdAddr);
HERROR	APK_NETWORK_BT_SendPinCode(HUINT32 nIndex, HCHAR *pucPinCode);
HERROR	APK_NETWORK_BT_SendPinCodeByBdAddr(const HCHAR *szBdAddr, HCHAR *pucPinCode);
HUINT32	APK_NETWORK_BT_GetDeviceNum(void);
HERROR	APK_NETWORK_BT_GetDeviceInfo(HUINT32 ulDevId, APKS_BT_DEVICEINFO_t *pstDevInfoList);
HERROR	APK_NETWORK_BT_GetMacAddrInfo( HCHAR *szMacAddr);
HUINT32	APK_NETWORK_BT_GetMacAddrInfoSize(void);
HUINT32	APK_NETWORK_BT_GetDevInfoList(HUINT32 ulListSize, APKS_BT_DEVICEINFO_t *pstDevInfoList);

HERROR	APK_NETWORK_CM_Init(void);
HERROR	APK_NETWOKR_CM_DeInit(void);
HERROR	APK_NETWORK_CM_GetStatus( APKE_NETCM_Status_e *peStatus );
HERROR	APK_NETWORK_CM_GetDownStreamInfo( APKE_NETCM_DownInfo_e eDownInfo, HINT32 *pnValue );
HERROR	APK_NETWORK_CM_SetDownStreamInfo( APKE_NETCM_DownInfo_e eDownInfo, HINT32 nValue );
HERROR	APK_NETWORK_CM_GetUpStreamInfo( APKE_NETCM_UpInfo_e eUpInfo, HINT32 *pnValue );

HERROR	APK_NETWORK_DHCPSVR_Init(void);
HERROR	APK_NETWORK_DHCPSVR_DeInit(void);
HERROR	APK_NETWORK_DHCPSVR_UpdateInfo(void);
HERROR	APK_NETWORK_DHCPSVR_IsRunning(HBOOL *pbRunnig);
HERROR	APK_NETWORK_DHCPSVR_GetDevIndex(HINT32 *pnDevIndex);
HERROR	APK_NETWORK_DHCPSVR_Start( HINT32 nIndex );
HERROR	APK_NETWORK_DHCPSVR_Stop( HINT32 nIndex );

HERROR	APK_NETWORK_CONF_GetNumOfNetDev(HINT32 *pnCount );
HERROR	APK_NETWORK_CONF_GetDefaultDev(HINT32 *pnIndex );
HERROR	APK_NETWORK_CONF_GetDevInfo( HINT32 nIndex, APKS_NETDEVINFO_t *pstDevInfo );
HERROR	APK_NETWORK_CONF_GetMacAddr( HINT32 nIndex, HCHAR *pszMacAddr, HINT32 nBufSize );
HERROR	APK_NETWORK_CONF_GetAddrInfo( HINT32 nIndex, APKS_NETIFADDR_t *pstNetAddr );
HERROR	APK_NETWORK_CONF_SetAddrInfo( HINT32 nIndex, APKS_NETIFADDR_t *pstNetAddr  );
HERROR	APK_NETWORK_CONF_StopConnecting( HINT32 nIndex );
HERROR	APK_NETWORK_CONF_SaveEnableStatus( HINT32 nIndex, HBOOL bOn );
HERROR	APK_NETWORK_CONF_AddrN2A( HULONG ulAddr, HCHAR *pszAddr, HINT32 nbufSize );
HERROR	APK_NETWORK_CONF_AddrA2N( HCHAR *pszAddr, HULONG *pulAddr );
HERROR	APK_NETWORK_CONF_IsSupportNetwork(HBOOL *bIsSupport );
HERROR 	APK_NETWORK_CONF_IsNetworkAvailable(HBOOL *isEnable);
HERROR	APK_NETWORK_CONF_CheckInternetConnection(HINT32 lTimeout);



HERROR	APK_NETWORK_WIFI_StartScan(HINT32 nIndex);
HERROR	APK_NETWORK_WIFI_ConnectAP( HINT32 nIndex, APKS_NETAP_Info_t *pstApInfo, HCHAR *szKey );
HERROR	APK_NETWORK_WIFI_ConnectAPByWPS( HINT32 nIndex, APKE_NETAP_WPS_Type_e eWPSType, HCHAR *szPinCode );
HERROR	APK_NETWORK_WIFI_DisconnectAP( HINT32 nIndex );
HERROR	APK_NETWORK_WIFI_GetPincodeByWPS( HINT32 nIndex, HCHAR *szPincode );
HERROR	APK_NETWORK_WIFI_GetConnectedInfo( HINT32 nIndex, APKS_NETAP_Info_t *pstApInfo, HCHAR *pszKey, HINT32 nBufSize );

/**
* This synchonous function is used to configure WoWLAN feature
* @param[in] nIndex: index of wifi network interface
* @param[in] etMode: chosen mode
* @return ERR_OK if this WoWLAN feature is configured succesfully, ERR_FAIL if vice versa
*/

HERROR	APK_NETWORK_WIFI_SetWakeupOnWlan( HINT32 lIndex, APKS_NETWoWlan_Mode_t etMode);

HERROR	APK_NETWORK_PPPOE_Connect(APKS_PPPOE_Info_t *pstPPPoEInfo);
HERROR	APK_NETWORK_PPPOE_Disconnect(HINT32 nIndex);
HERROR	APK_NETWORK_PPPOE_GetInfo(APKS_PPPOE_Info_t *pstPPPOEInfo);
HERROR	APK_NETWORK_PPPOE_SetInfo(APKS_PPPOE_Info_t *pstPPPoEInfo);
HERROR	APK_NETWORK_PPPOE_GetOnOff(HINT32 nIndex, HBOOL *bOn);
HERROR	APK_NETWORK_PPPOE_SetOnOff(HINT32 nIndex, HBOOL bOn);


HINT32	APK_NETWORK_CONCHK_Gateway( HINT32 nIndex, APKS_NETWORK_CbHandler pfnCallback, HINT32 nTimeout );
HINT32	APK_NETWORK_CONCHK_Dns( HINT32 nIndex, APKS_NETWORK_CbHandler pfnCallback, HINT32 nTimeout );
HINT32	APK_NETWORK_CONCHK_Arp( HINT32 nIndex, HCHAR *pszAddr, APKS_NETWORK_CbHandler pfnCallback, HINT32 nTimeout );
HINT32	APK_NETWORK_CONCHK_ResolvDomain( HINT32 nIndex, HCHAR *pszDomain, APKS_NETWORK_CbHandler pfnCallback, HINT32 nTimeout );

#endif
/* end of file */
