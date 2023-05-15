/*************************************************************************************************************
	File 		: nico._msgh
	author 		: IPM tipmsw1@humaxdigital.com
	comment		:
	date    	: 2013/05/16
	attention 	:

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


#ifndef __NICO_MSG_H_
#define __NICO_MSG_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "htype.h"
#include <arpa/inet.h>
#include <resolv.h>

#define NICO_IPC_UDN "/tmp/.nico_signal.tmp"

#define NICO_PRIORITY_FILE_PATH "/usr/etc/net_profile.xml"

#define NICO_BT_DEVICE_NAME 			"bluetooth"

#define NICO_MAX_DEVICE_COUNT			(10)
#define	NICO_MAX_IP4ADDR				(4)
#define NICO_MAX_IP6ADDR				(16)
#define	NICO_MAX_MACADDR				(6)
#define NICO_MAX_DOMAIN_LEN				(128)
#define NICO_MAX_ID						(32)
#define NICO_AP_ESSID					(33)
#define NICO_AP_KEY						(128)
#define NICO_MAX_AP 					(50)
#define	NICO_MAX_WPS_CODE_LEN 			(64)
#define	NICO_DHCPC_WAIT_TIMEOUT			(30) /* Sec */
#define	NICO_NET_INTERFACE_NAME_MAX		(16)
#define	NICO_MAX_ADDR					(256)
#define NICO_BT_NAME_MAX 				(248)
#define NICO_BT_BDADDR_LEN 				(6)
#define NICO_BT_CLASS_LEN 				(3)
#define NICO_BT_DEV_MAX 				(20)
#define NICO_BT_PINCODE_LEN				(16)
#define NICO_PPPOE_ID_LEN				(64)
#define NICO_PPPOE_PW_LEN				(128)
#define NICO_FW_VERSION_MAX_LEN	(128)

#define CASE_CMD_ENUMTOSTR(a) \
					case a : HxLOG_Debug("received cmd from client : %s (%08x)\n", #a, a);break; \

#define CASE_EVT_ENUMTOSTR(a) \
					case a : HxLOG_Debug("received evt from di : %s (%08x) \n", #a, a);break; \


/*******************************************************************/
/*****************        command & event         ******************/
/*******************************************************************/
typedef enum
{
	CMD_NETCONF 	= 0x00010000,
	CMD_WIFI		= 0x00020000,
	CMD_CM			= 0x00030000,
	CMD_BT			= 0x00040000,
	CMD_PPPOE		= 0x00050000,

	CMD_SET_NETCONF_DEVICE_CONFIG		= (CMD_NETCONF | 0x00000001),		// eNETWORK_PROPERTY_DevConfig
	CMD_SET_NETCONF_RESTART_DHCP_CLIENT,
	CMD_SET_NETCONF_STOP_CONNECTING,	// eNETWORK_PROPERTY_StopConnecting
	CMD_SET_NETCONF_DEVICE_ENABLE,		// eNETWORK_PROPERTY_SaveEnableStatus on/off
	CMD_SET_NETCONF_DEVICE_DISABLE,		// ifdown
	CMD_SET_NETCONF_START_DHCPSERVER,	// eNETWORK_PROPERTY_DHCPServerStart
	CMD_SET_NETCONF_STOP_DHCPSERVER,	// eNETWORK_PROPERTY_DHCPServerStop
	CMD_SET_NETCONF_WOL,
	CMD_SET_NETCONF_AUTOIP,
	CMD_SET_NETCONF_DEVICE_PRIORITY,
	CMD_SET_NETCONF_NTP_CLIENT,			// 0x0001000a
	CMD_SET_NETCONF_RENEW,

	CMD_GET_NETCONF_DEVICE_COUNT,
	CMD_GET_NETCONF_ACTIVE_DEVICEID,
	CMD_GET_NETCONF_NETINFO,			// eNETWORK_PROPERTY_DevConfig
	CMD_GET_NETCONF_DHCPSERVER_INFO,	// eNETWORK_PROPERTY_DHCPServerInfo

	CMD_SET_WIFI_START_SCAN_AP			= (CMD_WIFI | 0x00000001),			// eNETWORK_PROPERTY_ApScanning
	CMD_SET_WIFI_STOP_SCAN_AP,
	CMD_SET_WIFI_CONNECT_AP,			// eNETWORK_PROPERTY_ApConnecting
	CMD_SET_WIFI_CONNECT_WPS,			// eNETWORK_PROPERTY_WPSConnecting
	CMD_SET_WIFI_DISCONNECT_AP, 		// eNETWORK_PROPERTY_ApDisconnecting
	CMD_SET_WIFI_GET_WPS_PINCODE, 			
	CMD_SET_WIFI_WOWLAN,

	CMD_GET_WIFI_AP_INFO_LIST,			// ??
	CMD_GET_WIFI_AP_INFO_CONNECTED, 	// eNETWORK_PROPERTY_ConnectedApInfo
	CMD_GET_WIFI_AP_INFO_WAVE,			// eNETWORK_PROPERTY_ConnectedApWave

	CMD_SET_CM_DOWNSTREAM_INFO			= (CMD_CM | 0x00000001),			// eNETWORK_PROPERTY_CmInfo

	CMD_SET_BT_ENABLE					= (CMD_BT | 0x00000001),
	CMD_SET_BT_START_SCAN,
	CMD_SET_BT_PINCODE,
	CMD_SET_BT_CONNECT,
	CMD_SET_BT_DISCONNECT,
	CMD_SET_BT_REMOVE,
	CMD_SET_BT_OPEN_PROFILE,
	CMD_SET_BT_CLOSE_PROFILE,
	CMD_GET_BT_HOST_INFO,
	CMD_GET_BT_DEVICE_LIST,
	CMD_SET_BT_INFO_DB,
	CMD_SET_BT_REMOVE_DB,

	CMD_SET_PPPOE_CONNECT				= (CMD_PPPOE | 0x00000001),
	CMD_SET_PPPOE_DISCONNECT,
	CMD_SET_PPPOE_INFO,
	CMD_GET_PPPOE_INFO,
	CMD_SET_PPPOE_ONOFF,

	CMD_UNKNOWN,
} eNcapi_command_type;

typedef enum
{
	EVT_WIFI 	= 0x40000000,
	EVT_BT	 	= 0x50000000,
	EVT_CM	 	= 0x60000000,
	EVT_NETCONF = 0x70000000,
	EVT_PPPOE	= 0x80000000,

	EVENT_NET_CONNECT_SUCCESS = (EVT_NETCONF | 0x00000001),
	EVENT_NET_CONNECT_FAIL,
	EVENT_NET_CONNECT_AUTOIP_SUCCESS,
	EVENT_NET_CONNECT_AUTOIP_FAIL,
	EVENT_NET_PLUGGED_IN,
	EVENT_NET_PLUGGED_OUT,
	EVENT_NET_STATUS_UPDATED,
	EVENT_NET_STOP_CONNECTING,
	EVENT_NET_WOL_ENABLED,
	EVENT_NET_WOL_DISABLED,

	EVENT_NET_EXT_STARTED_DHCPSERVER,
	EVENT_NET_EXT_STOPPED_DHCPSERVER,
	EVENT_NET_EXT_CONNECT_SUCCESS = EVENT_NET_CONNECT_SUCCESS,
	EVENT_NET_EXT_CONNECT_FAIL = EVENT_NET_CONNECT_FAIL,
	EVENT_NET_EXT_PLUGGED_IN = EVENT_NET_PLUGGED_IN,
	EVENT_NET_EXT_PLUGGED_OUT = EVENT_NET_PLUGGED_OUT,
	EVENT_NET_EXT_STOP_CONNECTING = EVENT_NET_STOP_CONNECTING,
	EVENT_NET_EXT_WOL_ENABLED = EVENT_NET_WOL_ENABLED,
	EVENT_NET_EXT_WOL_DISABLED = EVENT_NET_WOL_DISABLED,
	EVENT_NET_EXT_CONNECT_AUTOIP_FAIL = EVENT_NET_CONNECT_AUTOIP_FAIL,

	EVENT_NET_WIFI_USB_INSERTED = (EVT_WIFI | 0x00000001),
	EVENT_NET_WIFI_USB_EXTRACTED,

	EVENT_NET_WIFI_SCAN_FINISHED,
	EVENT_NET_WIFI_SCAN_OUTPUT, //?
	EVENT_NET_WIFI_SCAN_FAIL,

	EVENT_NET_WIFI_AP_CONNECT_SUCCESS,
	EVENT_NET_WIFI_AP_CONNECT_FAIL,
	EVENT_NET_WIFI_WPS_CONNECT_SUCCESS,
	EVENT_NET_WIFI_WPS_CONNECT_FAIL,

	EVENT_NET_WIFI_EXT_SCAN_STARTED,
	EVENT_NET_WIFI_EXT_DISCONNECTED,
	EVENT_NET_WIFI_EXT_RECONNECTED,
	EVENT_NET_WIFI_EXT_INVALID_KEY,
	EVENT_NET_WIFI_EXT_SCAN_FINISHED = EVENT_NET_WIFI_SCAN_FINISHED,
	EVENT_NET_WIFI_EXT_SCAN_FAIL = EVENT_NET_WIFI_SCAN_FAIL,
	EVENT_NET_WIFI_EXT_AP_CONNECT_FAIL = EVENT_NET_WIFI_AP_CONNECT_FAIL,
	EVENT_NET_WIFI_EXT_WPS_CONNECT_FAIL = EVENT_NET_WIFI_WPS_CONNECT_FAIL,

	EVENT_NET_BT_INSERTED = (EVT_BT | 0x00000001),
	EVENT_NET_BT_EXTRACTED,
	EVENT_NET_BT_ENABLE,
	EVENT_NET_BT_DISABLE,
	EVENT_NET_BT_SCAN_SUCCESS,
	EVENT_NET_BT_SCAN_FAIL,
	EVENT_NET_BT_AUTHEN_SUCCESS,
	EVENT_NET_BT_AUTHEN_FAIL,
	EVENT_NET_BT_PASSKEY,
	EVENT_NET_BT_CONNECT_SUCCESS,
	EVENT_NET_BT_CONNECT_FAIL,
	EVENT_NET_BT_REMOVED,
	EVENT_NET_BT_REMOTE_CMD,
	EVENT_NET_BT_MAX,

	EVENT_NET_CM_ONLINE = (EVT_CM | 0x00000001),
	EVENT_NET_CM_OFFLINE,

	EVENT_NET_PPPOE_CONNECT_SUCCESS = (EVT_PPPOE | 0x00000001),
	EVENT_NET_PPPOE_CONNECT_FAIL,
	EVENT_NET_PPPOE_DISCONNECT_SUCCESS,
	EVENT_NET_PPPOE_DISCONNECT_FAIL,
	EVENT_NET_PPPOE_SET_INFO,
	EVENT_NET_PPPOE_SET_ONOFF,

	EVENT_NET_STATUS_UNKNOWN,
} eNico_event_type;

typedef enum {
	eNICO_Dns_Primary,
	eNICO_Dns_Secondary,
	eNICO_Dns_Thirdly,
	eNICO_Dns_Max,
} eNicoDnsAddress;

/*******************************************************************/
/************             common(protocol)-wifi      ***************/
/*******************************************************************/
typedef enum {
	eNICO_WIFI_OP_AUTO			= 0,
	eNICO_WIFI_OP_ADHOC,
	eNICO_WIFI_OP_INFRA,
	eNICO_WIFI_OP_MASTER,
	eNICO_WIFI_OP_REPEATER,
	eNICO_WIFI_OP_SECONDARY,
	eNICO_WIFI_OP_MONITOR,
	eNICO_WIFI_OP_UNKNOWN
} eNicoWifiOperation;

typedef enum {
	eNICO_WIFI_SECURITY_OPEN		= 0,
	eNICO_WIFI_SECURITY_WEP,
	eNICO_WIFI_SECURITY_WPA,
	eNICO_WIFI_SECURITY_WPA2,
	eNICO_WIFI_SECURITY_WPA_WPA2,
	eNICO_WIFI_SECURITY_UNKNOWN,
	eNICO_WIFI_SECURITY_MAX,
} eNicoWifiSecurity;

typedef enum {
	eNICO_WIFI_AUTH_OPEN			= 0,
	eNICO_WIFI_AUTHEN_PSK,
	eNICO_WIFI_AUTHEN_802_1X,
	eNICO_WIFI_AUTHEN_UNKNOWN,
	eNICO_WIFI_AUTHEN_MAX
#if 0
	eNICO_WIFI_AUTH_SHARED,
	eNICO_WIFI_AUTH_WEPAUTO,
	eNICO_WIFI_AUTH_WPAPSK,
	eNICO_WIFI_AUTH_WPA2PSK,
	eNICO_WIFI_AUTH_802_1x,
#endif
} eNicoWifiAuth;

typedef enum {
	eNICO_WIFI_ENCRYPT_NONE			= 0, /**< needs to 10 hex digits shared key */
	eNICO_WIFI_ENCRYPT_WEP_64, 		/*DI_WIFI_ENCRYPT_WEP_64_ASCII*< needs to 10 hex digits shared key */
	eNICO_WIFI_ENCRYPT_WEP_64HEX,
	eNICO_WIFI_ENCRYPT_WEP_128,		/*DI_WIFI_ENCRYPT_WEP_128_ASCII*< needs to 26 hex digits shared key */
	eNICO_WIFI_ENCRYPT_WEP_128HEX,
	eNICO_WIFI_ENCRYPT_TKIP,
	eNICO_WIFI_ENCRYPT_AES,
	eNICO_WIFI_ENCRYPT_TKIP_AES,
	eNICO_WIFI_ENCRYPT_UNKNOWN,
	eNICO_WIFI_ENCRYPT_MAX
#if 0
	eNICO_WIFI_ENCRYPT_WEP_64,
	eNICO_WIFI_ENCRYPT_WPA_TKIP,
	eNICO_WIFI_ENCRYPT_WRAP,
	eNICO_WIFI_ENCRYPT_WPA_AES,
	eNICO_WIFI_ENCRYPT_WPA_TKIP_AES,
	eNICO_WIFI_ENCRYPT_WEP_128,
	eNICO_WIFI_ENCRYPT_WEP_64_HEX,
	eNICO_WIFI_ENCRYPT_WEP_64_ASCII,
	eNICO_WIFI_ENCRYPT_WEP_128_HEX,
	eNICO_WIFI_ENCRYPT_WEP_128_ASCII,
	eNICO_WIFI_ENCRYPT_UNKNOWN
#endif
} eNicoWifiEncrypt;

typedef enum {
	eNICO_WIFI_WPS_NONE,
	eNICO_WIFI_WPS_PBC,
	eNICO_WIFI_WPS_PIN,
	eNICO_WIFI_WPS_UNKNOWN,
	eNICO_WIFI_WPS_MAX,
} eNicoWifiWps;

typedef enum
{
	eNICO_WIFI_WOWL_NONE			= 0x00,		/* Do not wake up on any event. */
	eNICO_WIFI_WOWL_MAGIC_PATTERN	= 0x01,		/* Wake up on magic pattern. */
	eNICO_WIFI_WOWL_NET_PATTERN		= 0x02,		/* Wake up on a special net pattern. */
	eNICO_WIFI_WOWL_DISASSOC_DEAUTH	= 0x04,		/* Wake up on disassociate from AP. */
	eNICO_WIFI_WOWL_RETROGRADE_TSF	= 0x08,		/* Wake up on retrograde tsf. */
	eNICO_WIFI_WOWL_BCN_LOSS		= 0x10,		/* Wake up on loss of beacon. */
} eNicoWifiWoWlan ;

typedef struct
{
	eNicoWifiWps eType;
	HCHAR	szCode[NICO_MAX_WPS_CODE_LEN + 1 ];
} __attribute__((packed)) NICO_Wifi_WPSInfo_t;

typedef struct
{
	HCHAR				essid[NICO_AP_ESSID];
	HUINT32				channel;
	HUINT32				freq;
	HBOOL				is_wps;
	eNicoWifiOperation	op_mode;
	HUINT8				macaddr[NICO_MAX_MACADDR];
	HINT32  			wave_strength;
	HINT32				bitrate;
	eNicoWifiSecurity	secure_type;
	eNicoWifiAuth 		authen_type;
	eNicoWifiEncrypt	encrypt_type;
	HUINT32				keyindex;
	HUINT8				id[NICO_MAX_ID];
	HCHAR 				apkey[NICO_AP_KEY];
	HBOOL				is_connected;
} __attribute__((packed)) NICO_Wifi_APInfo_t;

typedef struct
{
	HINT32  id;
	HBOOL  is_run;
} __attribute__((packed)) Cmd_Wifi_StartScan_t, Cmd_Wifi_StopScan_t;

typedef struct
{
	HINT32 id;
	NICO_Wifi_APInfo_t apInfo;
} __attribute__((packed)) Cmd_Wifi_ConnectAp_t, Cmd_Wifi_DisconnectAp_t;

typedef struct
{
	HINT32 count;
} __attribute__((packed)) Cmd_Wifi_DevCount_t;

typedef struct
{
	HINT32 id;
	HINT32 apcount;
	NICO_Wifi_APInfo_t apinfo;
} __attribute__((packed)) Cmd_Wifi_ConnectedApInfo_t;

typedef struct
{
	HINT32 id;
	HINT32 ap_count;
	NICO_Wifi_APInfo_t aplist[NICO_MAX_AP];
} __attribute__((packed)) Cmd_Wifi_GetApInfoList_t;

typedef struct
{
	HINT32 id;
	NICO_Wifi_WPSInfo_t wpsInfo;
} __attribute__((packed)) Cmd_Wifi_ConnectWps_t;

typedef struct
{
	HINT32 id;
	HCHAR	szPincode[NICO_MAX_WPS_CODE_LEN + 1 ];
} __attribute__((packed)) Cmd_Wifi_GetPincodeWPS_t;

typedef struct
{
	HINT32 id;
	eNicoWifiWoWlan mode;
	HERROR ret;
} __attribute__((packed)) Cmd_Wifi_Set_WoWlan_t;


typedef struct
{
	union
	{
		Cmd_Wifi_StartScan_t 		cmd_set_startscan;
		Cmd_Wifi_StopScan_t 		cmd_set_stopscan;
		Cmd_Wifi_ConnectAp_t		cmd_set_connectap;
		Cmd_Wifi_DisconnectAp_t		cmd_set_disconnectap;
		Cmd_Wifi_ConnectWps_t		cmd_set_connectwps;

		Cmd_Wifi_DevCount_t			cmd_get_devcount;
		Cmd_Wifi_ConnectedApInfo_t 	cmd_get_connectedapinfo;
		Cmd_Wifi_GetApInfoList_t 	cmd_get_aplistinfo;
		Cmd_Wifi_GetPincodeWPS_t	cmd_get_pincodewps;
		Cmd_Wifi_Set_WoWlan_t		cmd_set_wowlan;
	} cmd;
} __attribute__((packed))Cmd_Wifi_t;

typedef struct
{
	HINT32 id;
} __attribute__((packed))Evt_Wifi_Dongle_t;

typedef struct
{
	HINT32 id;
	HINT32 apcount;
	NICO_Wifi_APInfo_t apinfo[NICO_MAX_AP];
} __attribute__((packed))Evt_Wifi_ScanResult_t;

typedef struct
{
	HINT32 id;
} __attribute__((packed))Evt_Wifi_AuthenResult_t;

typedef struct
{
	HINT32 id;
	NICO_Wifi_APInfo_t apinfo;
} __attribute__((packed))Evt_Wifi_ConnectResult_t;

typedef struct
{
	union
	{
		Evt_Wifi_Dongle_t evt_dongle;
		Evt_Wifi_ScanResult_t evt_scan;
		Evt_Wifi_AuthenResult_t evt_auth;
		Evt_Wifi_ConnectResult_t evt_connect;
	} evt;
} __attribute__((packed))Evt_Wifi_t;
//1 #### end of wifi

/*******************************************************************/
/************             common(protocol)-cm        ***************/
/*******************************************************************/
typedef enum {
	eNICO_CM_QAM_64 = 	0,
	eNICO_CM_QAM_256,
	eNICO_CM_QAM_OTHER,
	eNICO_CM_QAM_UNKNOWN,
} eNicoCmQam;

typedef enum {
	eNICO_CM_NONE = 0x00,
	eNICO_CM_OTHER,
	eNICO_CM_NOT_READY,
	eNICO_CM_NOT_SYNC,
	eNICO_CM_PHY_SYNC_DONE,
	eNICO_CM_PARAM_ACQUIRED,
	eNICO_CM_RANGING_COMPLETE,
	eNICO_CM_IP_COMPLETE,
	eNICO_CM_TOD_CERTIFY,
	eNICO_CM_SECURITY_CERTIFY,
	eNICO_CM_PARAM_TRAANSE_COMPLETE,
	eNICO_CM_REGIST_COMPLETE,
	eNICO_CM_OPERATION,
	eNICO_CM_ACCESS_DENIED,
} eNICO_CM_Status;

typedef struct {
	eNicoCmQam	qam_type;
	HINT32 		freq;
	HINT32 		ber;
	HINT32 		snr;
	HINT32 		signal_level;
} __attribute__((packed))NICO_CM_DownStream_t;

typedef struct {
	HINT32	 	freq;
	HINT32	 	channel_id;
	HINT32		bandwidth;
	HINT32		signal_level;
} __attribute__((packed))NICO_CM_UpStream_t;

typedef struct {
	eNICO_CM_Status state;
	NICO_CM_DownStream_t down_stream;
	NICO_CM_UpStream_t up_stream;
} __attribute__((packed))NICO_CM_Info_t;

typedef struct
{
	NICO_CM_DownStream_t downstreamInfo;
} __attribute__((packed))Cmd_Cm_SetDownStreamInfo_t, Cmd_Cm_GetDownStreamInfo_t;
typedef struct
{
	NICO_CM_UpStream_t upstreaminfo;
} __attribute__((packed))Cmd_Cm_GetUpStreamInfo_t;

typedef struct
{
	union
	{
		Cmd_Cm_SetDownStreamInfo_t cmd_set_downstreaminfo;

		Cmd_Cm_GetDownStreamInfo_t cmd_get_downstreaminfo;
		Cmd_Cm_GetUpStreamInfo_t cmd_get_upstreaminfo;
	} cmd;
} __attribute__((packed))Cmd_Cm_t;

typedef struct
{
	HBOOL is_online;
} __attribute__((packed))Evt_Cm_lineState_t;

typedef struct
{
	union
	{
		Evt_Cm_lineState_t evt_state;
	} evt;
	NICO_CM_Info_t cminfo;
} __attribute__((packed))Evt_Cm_t;
//1 #### end of netconf

/*******************************************************************/
/************         common(protocol)-netconf       ***************/
/*******************************************************************/
typedef enum {
	eNICO_DEVTYPE_LAN			= 0,
	eNICO_DEVTYPE_WAN,
	eNICO_DEVTYPE_CM,
	eNICO_DEVTYPE_WIFI,
	eNICO_DEVTYPE_BLUETOOTH,
	eNICO_DEVTYPE_RF4CE,
	eNICO_DEVTYPE_PPPOE,
	eNICO_DEVTYPE_MAX,
} eNicoNetConfDevType;

typedef enum {
	eNICO_VER_IPV4			= 0,
	eNICO_VER_IPV6
} eNicoNetConfIpVersion;

typedef enum {
	eNICO_ConnectType_WAN,
	eNICO_ConnectType_LAN,
	eNICO_ConnectType_MAX,
	eNICO_ConnectType_UnKnown = 0x99, //
} eNicoNetConnectType;

typedef enum
{
	eNICO_IPV6_ADDR_GLOBAL	= 0x0000U,
	eNICO_IPV6_ADDR_LOOPBACK	= 0x0010U,
	eNICO_IPV6_ADDR_LINKLOCAL	= 0x0020U,
	eNICO_IPV6_ADDR_SITELOCAL	= 0x0040U,
	eNICO_IPV6_ADDR_COMPATv4	= 0x0080U
} eNICO_IPV6_ADDR_SCOPE_e;

typedef struct
{
	HUINT8	macaddr[NICO_MAX_MACADDR];
	HUINT8	ipAddr[NICO_MAX_IP4ADDR];
	HUINT8	netmask[NICO_MAX_IP4ADDR];
	HUINT8	gateway[NICO_MAX_IP4ADDR];
	HUINT8	dns[eNICO_Dns_Max][NICO_MAX_IP4ADDR];
	HUINT8	broadcast[NICO_MAX_IP4ADDR];
} __attribute__((packed))NICO_ipv4_t;

typedef struct
{
	HUINT8	macaddr[NICO_MAX_MACADDR];
	HUINT8	ipAddr[NICO_MAX_IP6ADDR];
	HUINT8	gateway[NICO_MAX_IP6ADDR];
	HUINT8	dns[eNICO_Dns_Max][NICO_MAX_IP6ADDR];
	HUINT8	broadcast[NICO_MAX_IP6ADDR];
	HUINT32 prefix;
	eNICO_IPV6_ADDR_SCOPE_e addrscope;
} __attribute__((packed))NICO_ipv6_t;

typedef struct {//union {
	NICO_ipv4_t ipv4;
	NICO_ipv6_t ipv6;
} __attribute__((packed)) NICO_NetConf_IpInfo_t;

typedef struct {
	HUINT32  router;
	HUINT32  netmask;
	HUINT8	startip[NICO_MAX_IP4ADDR];
	HUINT8  endip[NICO_MAX_IP4ADDR];
	HUINT8  dns[eNICO_Dns_Max][NICO_MAX_IP4ADDR];
	HUINT32 leasetime;
	HCHAR	domain[NICO_MAX_DOMAIN_LEN];
} __attribute__((packed)) NICO_DHCPSConf_Info_t;

typedef struct
{
	HBOOL is_run;
	eNicoNetConfIpVersion eIPvType;
	NICO_DHCPSConf_Info_t config;
} __attribute__((packed))NICO_DHCPSContext_t;

typedef struct {
	HINT32		id;
	HINT32 		priority;
	HCHAR 		name[NICO_NET_INTERFACE_NAME_MAX];
	eNicoNetConfDevType	dev_type;
	HBOOL		linked;		// GET from DI
	HBOOL		active;		// GET from DI
	HBOOL		dhcp;		// SET from nima
	HBOOL		dns_auto;	// SET from nima
	HBOOL		auto_ip;	// SET from nima, but wasn't use.
	eNicoNetConfIpVersion	ip_ver;
	NICO_NetConf_IpInfo_t	ip_info;
	HCHAR 		fw_ver[NICO_FW_VERSION_MAX_LEN]; // GET from DI
} __attribute__((packed))NICO_NetConf_Info_t;

typedef struct {
	HCHAR 		dev_name[NICO_NET_INTERFACE_NAME_MAX];
	HINT32 		priority;
} __attribute__((packed))NICO_Device_Priority_t;

typedef struct
{
	HINT32 id;
} __attribute__((packed)) Cmd_NetConf_GetActiveDefaultDevIndex_t;

typedef struct
{
	HINT32 device_count;
} __attribute__((packed)) Cmd_NetConf_GetDevCount_t;

typedef struct
{
	HINT32 id;
	NICO_NetConf_Info_t info;
} __attribute__((packed))Cmd_NetConf_GetNetInfo_t, Cmd_NetConf_SetNetInfo_t;

typedef struct
{
	HINT32 id;
	eNicoNetConfIpVersion eIpVer;
} __attribute__((packed))Cmd_NetConf_DHCPServer_t;

typedef struct
{
	HINT32 id;
} __attribute__((packed))Cmd_NetConf_StopConnect_t;

typedef struct
{
	HINT32 id;
	HBOOL is_up;
} __attribute__((packed))Cmd_NetConf_IfUp_t;

typedef struct
{
	HINT32 id;
	HBOOL is_down;
} __attribute__((packed))Cmd_NetConf_IfDown_t;

typedef struct
{
	HBOOL is_enable;
} __attribute__((packed))Cmd_NetConf_WOL_t;

typedef struct
{
	HINT32 id;
} __attribute__((packed)) Cmd_NetConf_SetAutoip_t;

typedef struct
{
	HINT32 id;
	HINT32 priority;
} __attribute__((packed)) Cmd_NetConf_SetDevPriority_t;

typedef struct
{
	HCHAR addr[NICO_MAX_ADDR];
} __attribute__((packed)) Cmd_NetConf_SetNTPClent_t;

typedef struct
{
	HCHAR addr[NICO_MAX_ADDR];
	HBOOL is_set_renew;
} __attribute__((packed)) Cmd_NetConf_SetRenew_t;



typedef struct
{
	union
	{
		Cmd_NetConf_GetDevCount_t cmd_get_devicecount;
		Cmd_NetConf_GetActiveDefaultDevIndex_t cmd_get_activedevice;
		Cmd_NetConf_GetNetInfo_t cmd_get_netinfo;

		Cmd_NetConf_SetNetInfo_t cmd_set_netinfo;
		Cmd_NetConf_DHCPServer_t cmd_set_dhcpserver;
		Cmd_NetConf_StopConnect_t cmd_set_stopconnecting;
		Cmd_NetConf_IfUp_t cmd_set_devup;
		Cmd_NetConf_IfDown_t cmd_set_devdown;
		Cmd_NetConf_WOL_t cmd_set_wol;
		Cmd_NetConf_SetAutoip_t cmd_set_autoip;
		Cmd_NetConf_SetDevPriority_t cmd_set_devpriority;
		Cmd_NetConf_SetNTPClent_t cmd_set_ntpclient;
		Cmd_NetConf_SetRenew_t cmd_set_renew;
	} cmd;
} __attribute__((packed))Cmd_NetConf_t;

typedef struct
{
	HINT32 id;
} __attribute__((packed))Evt_NetConf_PlugState_t;

typedef struct
{
	HINT32 id;
} __attribute__((packed))Evt_NetConf_DhcpResult_t;

typedef struct
{
	HINT32 id;
} __attribute__((packed))Evt_NetConf_PppoeResult_t;

typedef struct
{
	HINT32 id;
} __attribute__((packed))Evt_NetConf_AutoipResult_t;

typedef struct
{
	union
	{
		Evt_NetConf_PlugState_t evt_plugstate;
		Evt_NetConf_DhcpResult_t evt_dhcpc;
		Evt_NetConf_PppoeResult_t evt_pppoe;
		Evt_NetConf_AutoipResult_t evt_autoip;
	} evt;
	NICO_NetConf_Info_t evt_netinfo;
} __attribute__((packed))Evt_NetConf_t;

/*******************************************************************/
/************         common(protocol)-bluetooth     ***************/
/*******************************************************************/
typedef enum {
	eNICO_BT_Profile_Unknown	= 0x00000000,
	eNICO_BT_Profile_A2DP		= 0x00040000, 	/* Adavanced Audio Distribution */
	eNICO_BT_Profile_AVRCP		= 0x00080000,	/* Audio/Video Remote Control */
	eNICO_BT_Profile_HID		= 0x00100000,	/* Human Interface Device */
	eNICO_BT_Profile_All		= 0x1FFFFFFF,
	eNICO_BT_Profile_Auto		= 0x80000000,
} eNicoBTProfile;

typedef enum {
	eNICO_BT_DEV_NOT_PAIRED			= 0,
	eNICO_BT_DEV_CONNECTED,
	eNICO_BT_DEV_DISCONNECTED
} eNicoBTStatus;

typedef struct
{
	HCHAR				name[NICO_BT_NAME_MAX];
	HUINT8				bdaddr[NICO_BT_BDADDR_LEN];
	HUINT8				dev_class[NICO_BT_CLASS_LEN];
	eNicoBTStatus		status;
	HUINT32				available_profile;
} __attribute__((packed)) NICO_BT_DevInfo_t;

typedef struct
{
	HBOOL				enable;
	NICO_BT_DevInfo_t	host_info;
	HINT32				passkey;
	HUINT8				connected_bdaddr[NICO_BT_BDADDR_LEN];
	HUINT32				count;
	NICO_BT_DevInfo_t	dev_list[NICO_BT_DEV_MAX];
} __attribute__((packed)) NICO_BT_Info_t;

typedef struct
{
	HBOOL enable;
} __attribute__((packed)) Cmd_BT_Enable_t;

typedef struct
{
	HUINT32 profile;
} __attribute__((packed)) Cmd_BT_StartScan_t;

typedef struct
{
	HUINT8 bdaddr[NICO_BT_BDADDR_LEN];
	HUINT8 pincode[NICO_BT_PINCODE_LEN];
} __attribute__((packed)) Cmd_BT_SendPincode_t;

typedef struct
{
	HUINT8 bdaddr[NICO_BT_BDADDR_LEN];
} __attribute__((packed)) Cmd_BT_Connect_t;

typedef struct
{
	HUINT8 bdaddr[NICO_BT_BDADDR_LEN];
} __attribute__((packed)) Cmd_BT_Disconnect_t;

typedef struct
{
	HUINT8 bdaddr[NICO_BT_BDADDR_LEN];
} __attribute__((packed)) Cmd_BT_Remove_t;

typedef struct
{
	HUINT8 bdaddr[NICO_BT_BDADDR_LEN];
	HUINT32 profile;
} __attribute__((packed)) Cmd_BT_OpenProfile_t;

typedef struct
{
	HUINT8 bdaddr[NICO_BT_BDADDR_LEN];
	HUINT32 profile;
} __attribute__((packed)) Cmd_BT_CloseProfile_t;

typedef struct
{
	NICO_BT_DevInfo_t dev_info;
} __attribute__((packed)) Cmd_BT_HostInfo_t;

typedef struct
{
	NICO_BT_Info_t info;
} __attribute__((packed)) Cmd_BT_DevList_t;

typedef struct
{
	NICO_BT_Info_t info;
} __attribute__((packed)) Cmd_BT_SetDB_t;

typedef struct
{
	union
	{
		Cmd_BT_Enable_t 		cmd_set_enable;
		Cmd_BT_StartScan_t 		cmd_set_startscan;
		Cmd_BT_SendPincode_t	cmd_set_pincode;
		Cmd_BT_Connect_t		cmd_set_connect;
		Cmd_BT_Disconnect_t		cmd_set_disconnect;
		Cmd_BT_Remove_t			cmd_set_remove;
		Cmd_BT_OpenProfile_t 	cmd_set_openprofile;
		Cmd_BT_CloseProfile_t 	cmd_set_closeprofile;
		Cmd_BT_HostInfo_t 		cmd_get_hostinfo;
		Cmd_BT_DevList_t 		cmd_get_devlist;
		Cmd_BT_SetDB_t 			cmd_set_db;
	} cmd;
} __attribute__((packed)) Cmd_Bluetooth_t;

typedef struct
{
	HINT32 dev_id;
	NICO_BT_Info_t bt_info;
} __attribute__((packed))Evt_BT_Result_t;

typedef struct
{
	union
	{
		Evt_BT_Result_t evt_result;
	} evt;
}__attribute__((packed)) Evt_Bluetooth_t;

//1 #### end of bluetooth

/*******************************************************************/
/************         common(protocol)-PPPoE         ***************/
/*******************************************************************/
typedef struct
{
	HINT32 id;
	HBOOL  enable;
	HUINT8 connect_id[NICO_PPPOE_ID_LEN];
	HUINT8 connect_pw[NICO_PPPOE_PW_LEN];
} __attribute__((packed)) NICO_PPPoE_Info_t;

typedef struct
{
	HINT32 id;
	NICO_PPPoE_Info_t info;
} __attribute__((packed)) Cmd_PPPoE_Connect;

typedef struct
{
	HINT32 id;
} __attribute__((packed)) Cmd_PPPoE_Disconnect;

typedef struct
{
	HINT32 id;
	NICO_PPPoE_Info_t info;
} __attribute__((packed)) Cmd_PPPoE_InfoDB_t;

typedef struct
{
	HINT32 id;
	HBOOL enable;
} __attribute__((packed)) Cmd_PPPoE_Enable_t;

typedef struct
{
	HINT32 id;
	HBOOL  connected;
	NICO_PPPoE_Info_t info;
} __attribute__((packed)) Evt_PPPoE_ConnectResult_t;

typedef struct
{
	HINT32 id;
	HBOOL  connected;
	NICO_PPPoE_Info_t info;
} __attribute__((packed)) Evt_PPPoE_DisconnectResult_t;

typedef struct
{
	HINT32 id;
	NICO_PPPoE_Info_t info;
} __attribute__((packed)) Evt_PPPoE_Result_t;

typedef struct
{
	union
	{
		Cmd_PPPoE_Connect 			cmd_set_connect;
		Cmd_PPPoE_Disconnect 		cmd_set_disconnect;
		Cmd_PPPoE_InfoDB_t			cmd_set_infodb;
		Cmd_PPPoE_Enable_t 			cmd_set_enable;

		Cmd_PPPoE_InfoDB_t			cmd_get_infodb;
	} cmd;
} __attribute__((packed)) Cmd_PPPoE_t;

typedef struct
{
	union
	{
		Evt_PPPoE_ConnectResult_t 		evt_connect;
		Evt_PPPoE_DisconnectResult_t 	evt_disconnect;
		Evt_PPPoE_Result_t				evt_info;
	} evt;
} __attribute__((packed)) Evt_PPPoE_t;

//1 #### end of PPPoE


/*******************************************************************/
/************             common(protocol)           ***************/
/*******************************************************************/
/************* Real Message *************/
typedef struct
{
	HBOOL  sync_call;
	HBOOL  is_not_support;
	HINT32 request_id;
	HINT32 sock_fd;
	HINT32 device_id;

	union
	{
		eNcapi_command_type cmd_type;
		eNico_event_type evt_type;
	} type;

	union
	{
		Cmd_NetConf_t cmd_netconf;
		Evt_NetConf_t evt_netconf;

		Cmd_Wifi_t cmd_wifi;
		Evt_Wifi_t evt_wifi;

		Cmd_Cm_t cmd_cm;
		Evt_Cm_t evt_cm;

		Cmd_Bluetooth_t	cmd_bluetooth;
		Evt_Bluetooth_t evt_bluetooth;

		Cmd_PPPoE_t cmd_pppoe;
		Evt_PPPoE_t evt_pppoe;

	} cmd_protocol, evt_protocol;
}__attribute__((packed)) command_msg,  event_msg, syncreturn_msg;

typedef command_msg* pcommand_msg;
typedef event_msg*	pevent_msg;
typedef syncreturn_msg* psyncreturn_msg;

/*****************************************/

typedef struct
{
	HINT32					id;
	HUINT8					name[NICO_NET_INTERFACE_NAME_MAX];

	HBOOL					is_trywps;

	NICO_DHCPSContext_t		dhcpserverinfo;

	NICO_NetConf_Info_t		netinfo;

	HINT32					ap_count;
	NICO_Wifi_WPSInfo_t		wpsinfo;
	NICO_Wifi_APInfo_t		apinfo_connected;
	NICO_Wifi_APInfo_t		apinfo_list[NICO_MAX_AP];

	NICO_CM_Info_t			cminfo;

	NICO_BT_Info_t			btinfo;

	NICO_PPPoE_Info_t		pppoeinfo;

	//1 #### for ipv4
	HUINT32 				ip4_dns_count;		// ulIp4DnsCnt;

	//2 #### for ipv6
	HUINT32 				ip6_dns_count;		// ulIp6DnsCnt
}__attribute__((packed)) NICO_NetDevice_Info_t;


/*****************************************/

typedef void (*NCAPI_ClientCallback) (pevent_msg event);

typedef struct
{
	HINT32		len;
	command_msg *data;
} nico_cmdmsg_t;

typedef void* p_dimsg;
typedef struct
{
	eNico_event_type 	type;
	HINT32				len;
	p_dimsg 			data;
} nico_dievtmsg_t;
//} __attribute__((packed)) nico_dievtmsg_t;

#ifdef __cplusplus
}
#endif

#endif

