/** **********************************************************************************************************
	File 		: nima_protocol.h
	author 		: STB Component tstbcomp@humaxdigital.com
	comment		:
	date    	: 2013/05/23
	attention 	:

	(c) 2011-2012 Humax Co., Ltd.
	This program is produced by Humax Co., Ltd. ("Humax") and
	the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
	non-assignable, non-transferable and non-exclusive license to use this Software.
	You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
	you agree to the responsibility to take all reasonable efforts to protect the any information
	you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
	reverse engineer or extract the source code of this Software unless you have Humax's written permission
	to do so.
	If you have no authorized license, discontinue using this Software immediately.

	THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES,
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
*********************************************************************************************************** */

#ifndef __NIMA_PROTOCOL_H__
#define __NIMA_PROTOCOL_H__

#include "htype.h"

#define NIMA_SERVER_UDS_PATH		"nima"
#define NIMA_SERV_MAGIC_NO			0xFEFEFEFE

#define NIMA_NETCONF_RENEW_SHPATH	"/etc/netrenew"

#define NIMA_PROTO_AP_ESSID		(33)
#define NIMA_MAX_AP 			(50)

#define NIMA_PROTO_AP_KEY		(128)
#define NIMA_PROTO_WPS_PINCODE	(64)
#define NIMA_PROTO_MAX_AP 		(50)
#define	NIMA_PROTO_MAX_IP4ADDR	(4)
#define NIMA_PROTO_MAX_IP6ADDR	(16)
#define	NIMA_PROTO_MAX_MACADDR	(6)
#define	NIMA_PROTO_MAX_INTERFACE_NAME		(16)
#define NIMA_PROTO_MAX_ADDR     (256)
#define NIMA_PROTO_BT_NAME_MAX 				(248)
#define NIMA_PROTO_BT_BDADDR_LEN			(6)
#define NIMA_PROTO_BT_DEV_MAX 				(20)
#define NIMA_PROTO_BT_PINCODE_LEN			(16)
#define NIMA_PROTO_BT_CLASS_LEN			    (3)
#define NIMA_PROTO_PPPOE_ID_LEN_MAX			(64)
#define NIMA_PROTO_PPPOE_PW_LEN_MAX		    (128)
#define NIMA_PROTO_FW_VERSION_LEN_MAX		(128)


#if 0 //Enable for debugging in flash mode (vdkmai)
#define FuncEnter printf("[%s][%s][%d]  (+)Enter!!\n",HLIB_STD_GetCurProcessName(),__FUNCTION__, __LINE__);
#define FuncLeave printf("[%s][%s][%d]  (-)Leave!!\n",HLIB_STD_GetCurProcessName(),__FUNCTION__, __LINE__);
#else
#define FuncEnter //HxLOG_Debug(" (+)Enter!!\n")
#define FuncLeave //HxLOG_Debug(" (-)Leave -- \n")
#endif

typedef enum {
	NIMA_PROTOTYPE_REQUEST		= 0,
	NIMA_PROTOTYPE_REPLY
} eNimaProtoType;

typedef enum {
	eNIMA_NETCONF_LAN			= 0,
	eNIMA_NETCONF_WAN,
	eNIMA_NETCONF_WIFI,
	eNIMA_NETCONF_CM,
	eNIMA_NETCONF_BLUETOOTH,
	eNIMA_NETCONF_RF4CE,
	eNIMA_NETCONF_PPPOE,
	eNIMA_NETCONF_MAX,
} eNimaNetConfDevType;

typedef enum {
	eNIMA_NETCONF_IPV4			= 0,
	eNIMA_NETCONF_IPV6
} eNimaNetConfIpVersion;

typedef enum {
	eNIMA_Dns_Primary,
	eNIMA_Dns_Secondary,
	eNIMA_Dns_Thirdly,
	eNIMA_Dns_Max,
} eNimaDnsAddress;

typedef enum {
	NIMA_PROTO_NETCONF_STATUS_Undefined,
	NIMA_PROTO_NETCONF_STATUS_Operation,
	NIMA_PROTO_NETCONF_STATUS_Reboot,
	NIMA_PROTO_NETCONF_STATUS_Shutdown,
	NIMA_PROTO_NETCONF_STATUS_RebootDone,
	NIMA_PROTO_NETCONF_STATUS_ShutdownDone,
	NIMA_PROTO_NETCONF_STATUS_Unknow
} eNimaProtoNetStatus;

typedef enum {
	NIMA_NETWORK_DISCONNECTED,
	NIMA_NETWORK_CONNECTING,
	NIMA_NETWORK_RECONNECTING,
	NIMA_NETWORK_CONNECTED,
} eNimaNetworkStatus;

typedef enum {
	NIMA_PROTO_CONNECT			= 0,
	NIMA_PROTO_DISCONNECT,
	NIMA_PROTO_GET_NET_INFO,
	NIMA_PROTO_SET_NET_INFO,
	NIMA_PROTO_SET_DYNAMIC_IP,
	NIMA_PROTO_SET_STATIC_IP,
	NIMA_PROTO_SET_AUTO_IP,
	NIMA_PROTO_SHUTDOWN_ETH,
	NIMA_PROTO_DEV_UP,
	NIMA_PROTO_DEV_DOWN,
	NIMA_PROTO_DEV_UPDATE,
	NIMA_PROTO_SET_DEV_PRIORITY,
	NIMA_PROTO_SET_NTP_CLIENT,
	NIMA_PROTO_WIFI_SCAN_START,
	NIMA_PROTO_WIFI_SCAN_STOP,
	NIMA_PROTO_WIFI_CONNECT_AP,
	NIMA_PROTO_WIFI_CONNECT_WPS,
	NIMA_PROTO_WIFI_GET_WPS_PINCODE,
	NIMA_PROTO_WIFI_DISCONNECT_AP,
	NIMA_PROTO_WIFI_SET_WAKEUP_ON_WLAN,
	NIMA_PROTO_DHCPSERVER_START,
	NIMA_PROTO_DHCPSERVER_STOP,
	NIMA_PROTO_SET_CM_DOWNSTREAM,
	NIMA_PROTO_BT_ENABLE,
	NIMA_PROTO_BT_SCAN,
	NIMA_PROTO_BT_SEND_PINCODE,
	NIMA_PROTO_BT_CONNECT,
	NIMA_PROTO_BT_DISCONNECT,
	NIMA_PROTO_BT_REMOVE,
	NIMA_PROTO_BT_OPEN_PROFILE,
	NIMA_PROTO_BT_CLOSE_PROFILE,
	NIMA_PROTO_BT_DEVLIST_UPDATE,
	NIMA_PROTO_PPPOE_CONNECT,
	NIMA_PROTO_PPPOE_DISCONNECT,
	NIMA_PROTO_PPPOE_SET_INFO,
	NIMA_PROTO_PPPOE_SET_ONOFF,
} eNimaProtocol;

typedef enum {
	NIMA_PROTO_EVENT_NETCONF 	= 0x1000,
	NIMA_PROTO_EVENT_WIFI		= 0x2000,
	NIMA_PROTO_EVENT_CM			= 0x4000,
	NIMA_PROTO_EVENT_BLUETOOTH	= 0x6000,
	NIMA_PROTO_EVENT_PPPOE		= 0x8000,

	NIMA_EVENT_NETCONF_CONN_SUCCESS = NIMA_PROTO_EVENT_NETCONF + 1,
	NIMA_EVENT_NETCONF_CONN_FAIL,
	NIMA_EVENT_NETCONF_AUTOIP_SUCCESS,
	NIMA_EVENT_NETCONF_AUTOIP_FAIL,
	NIMA_EVENT_NETCONF_LINK_CONNECTED,
	NIMA_EVENT_NETCONF_LINK_DISCONNECTED,
	NIMA_EVENT_NETCONF_STATUS_UPDATED,
	NIMA_EVENT_NETCONF_DHCPSERVER_START,
	NIMA_EVENT_NETCONF_DHCPSERVER_STOP,
	NIMA_EVENT_WIFI_INSERTED = NIMA_PROTO_EVENT_WIFI + 1,
	NIMA_EVENT_WIFI_EXTRACTED,
	NIMA_EVENT_WIFI_SCANNING_START,
	NIMA_EVENT_WIFI_SCANNING_END,
	NIMA_EVENT_WIFI_SCANNING_OUTPUT,
	NIMA_EVENT_WIFI_SCANNING_FAIL,
	NIMA_EVENT_WIFI_KEY_INVALID,
	NIMA_EVENT_WIFI_AP_CONNECT_SUCCESS,
	NIMA_EVENT_WIFI_AP_CONNECT_FAIL,
	NIMA_EVENT_WIFI_WPS_SUCCESS,
	NIMA_EVENT_WIFI_WPS_FAIL,
	NIMA_EVENT_WIFI_AP_DISCONNECTED,
	NIMA_EVENT_WIFI_AP_RECONNECTED,
	NIMA_EVENT_WIFI_AP_CONNECT_STOP,
	NIMA_EVENT_WIFI_LINK_UPDATED,
	NIMA_EVENT_BT_INSERTED = NIMA_PROTO_EVENT_BLUETOOTH + 1,
	NIMA_EVENT_BT_EXTRACTED,
	NIMA_EVENT_BT_ENABLE,
	NIMA_EVENT_BT_DISABLE,
	NIMA_EVENT_BT_SCAN_SUCCESS,
	NIMA_EVENT_BT_SCAN_FAIL,
	NIMA_EVENT_BT_AUTHEN_SUCCESS,
	NIMA_EVENT_BT_AUTHEN_FAIL,
	NIMA_EVENT_BT_PASSKEY,
	NIMA_EVENT_BT_CONNECT_SUCCESS,
	NIMA_EVENT_BT_CONNECT_FAIL,
	NIMA_EVENT_BT_REMOVED,
	NIMA_EVENT_BT_REMOTE_CMD,
	NIMA_EVENT_BT_MAX,
	NIMA_EVENT_PPPOE_CONNECTED = NIMA_PROTO_EVENT_PPPOE + 1,
	NIMA_EVENT_PPPOE_DISCONNECTED,
	NIMA_EVENT_PPPOE_SET_INFO,
	NIMA_EVENT_PPPOE_GET_INFO,
	NIMA_EVENT_PPPOE_SET_ONOFF,

	/* you have to choose the specific event of final loaded device */
	NIMA_EVENT_START_BOOT_SCENARIO = NIMA_EVENT_WIFI_INSERTED,
}eNimaEvent;

typedef enum {
	eNIMA_WIFI_OP_AUTO			= 0,
	eNIMA_WIFI_OP_ADHOC,
	eNIMA_WIFI_OP_INFRA,
	eNIMA_WIFI_OP_MASTER,
	eNIMA_WIFI_OP_REPEATER,
	eNIMA_WIFI_OP_SECONDARY,
	eNIMA_WIFI_OP_MONITOR,
	eNIMA_WIFI_OP_UNKNOWN
} eNimaWifiOperation;

typedef enum {
	eNIMA_WIFI_SECURITY_OPEN		= 0,
	eNIMA_WIFI_SECURITY_WEP,
	eNIMA_WIFI_SECURITY_WPA,
	eNIMA_WIFI_SECURITY_WPA2,
	eNIMA_WIFI_SECURITY_WPA_WPA2,
	eNIMA_WIFI_SECURITY_UNKNOWN
} eNimaWifiSecurity;

typedef enum {
	eNIMA_WIFI_AUTH_OPEN			= 0,
	eNIMA_WIFI_AUTH_PSK,
	eNIMA_WIFI_AUTH_802_1X,
	eNIMA_WIFI_AUTH_UNKNOWN,
	eNIMA_WIFI_AUTH_MAX
} eNimaWifiAuth;

typedef enum {
	eNIMA_WIFI_ENCRYPT_NONE 		= 0, /**< needs to 10 hex digits shared key */
	eNIMA_WIFI_ENCRYPT_WEP_64,		/*DI_WIFI_ENCRYPT_WEP_64_ASCII*< needs to 10 hex digits shared key */
	eNIMA_WIFI_ENCRYPT_WEP_64HEX,
	eNIMA_WIFI_ENCRYPT_WEP_128, 	/*DI_WIFI_ENCRYPT_WEP_128_ASCII*< needs to 26 hex digits shared key */
	eNIMA_WIFI_ENCRYPT_WEP_128HEX,
	eNIMA_WIFI_ENCRYPT_TKIP,
	eNIMA_WIFI_ENCRYPT_AES,
	eNIMA_WIFI_ENCRYPT_TKIP_AES,
	eNIMA_WIFI_ENCRYPT_UNKNOWN,
	eNIMA_WIFI_ENCRYPT_MAX
} eNimaWifiEncrypt;

typedef enum {
	eNIMA_WIFI_WPS_PIN			= 0,
	eNIMA_WIFI_WPS_PBC,
	eNIMA_WIFI_WPS_UNKNOWN,
	eNIMA_WIFI_WPS_MAX,
} eNimaWifiWPS;

typedef enum
{
	eNIMA_WIFI_WOWL_NONE			= 0x00,		/* Do not wake up on any event. */
	eNIMA_WIFI_WOWL_MAGIC_PATTERN	= 0x01,		/* Wake up on magic pattern. */
	eNIMA_WIFI_WOWL_NET_PATTERN		= 0x02,		/* Wake up on a special net pattern. */
	eNIMA_WIFI_WOWL_DISASSOC_DEAUTH	= 0x04,		/* Wake up on disassociate from AP. */
	eNIMA_WIFI_WOWL_RETROGRADE_TSF	= 0x08,		/* Wake up on retrograde tsf. */
	eNIMA_WIFI_WOWL_BCN_LOSS		= 0x10,		/* Wake up on loss of beacon. */
} eNimaWifiWoWlan ;

//bluetooth enum
typedef enum {
	eNIMA_BT_Profile_Unknown	= 0x00000000,
	eNIMA_BT_Profile_A2DP		= 0x00040000,	/* Adavanced Audio Distribution */
	eNIMA_BT_Profile_AVRCP		= 0x00080000,	/* Audio/Video Remote Control */
	eNIMA_BT_Profile_HID		= 0x00100000,	/* Human Interface Device */
	eNIMA_BT_Profile_All		= 0x1FFFFFFF,
	eNIMA_BT_Profile_Auto		= 0x80000000,
} eNimaBTProfile;

typedef enum {
	eNIMA_BT_STATUS_NotPaired = 0,
	eNIMA_BT_STATUS_Connected,
	eNIMA_BT_STATUS_Disconnected,
	eNIMA_BT_STATUS_Unknown,
	eNIMA_BT_STATUS_PinRequested,
	eNIMA_BT_STATUS_ProfileStarted
} eNimaBTStatus;

typedef enum {
	eNIMA_BT_DEVTYPE_Misc,
	eNIMA_BT_DEVTYPE_Computer,
	eNIMA_BT_DEVTYPE_Phone,
	eNIMA_BT_DEVTYPE_Network,
	eNIMA_BT_DEVTYPE_AV,
	eNIMA_BT_DEVTYPE_Peripheral,
	eNIMA_BT_DEVTYPE_Imaging,
	eNIMA_BT_DEVTYPE_Unknown
} eNimaBTType;

//netconf info
typedef union {
	struct {
		HUINT8	macaddr[NIMA_PROTO_MAX_MACADDR];
		HUINT8	ipAddr[NIMA_PROTO_MAX_IP4ADDR];
		HUINT8	netmask[NIMA_PROTO_MAX_IP4ADDR];
		HUINT8	gateway[NIMA_PROTO_MAX_IP4ADDR];
		HUINT8	dns1[NIMA_PROTO_MAX_IP4ADDR];
		HUINT8	dns2[NIMA_PROTO_MAX_IP4ADDR];
	} ipv4;

	struct {
		HUINT8	macaddr[NIMA_PROTO_MAX_MACADDR];
		HUINT8	ipAddr[NIMA_PROTO_MAX_IP6ADDR];
		HUINT8	gateway[NIMA_PROTO_MAX_IP6ADDR];
		HUINT8	dns1[NIMA_PROTO_MAX_IP6ADDR];
		HUINT8	dns2[NIMA_PROTO_MAX_IP6ADDR];
	} ipv6;
} NIMAProtocol_IpInfo_t;

typedef struct {
	HINT32		id;
	HINT32		priority;
	HCHAR		name[NIMA_PROTO_MAX_INTERFACE_NAME];
	eNimaNetConfDevType	type;
	HBOOL linked;
	HBOOL active;
	HBOOL dhcp;
	HBOOL dns_auto;
	HBOOL auto_ip;
	eNimaNetConfIpVersion ip_ver;
	NIMAProtocol_IpInfo_t ip_info;
	HCHAR 		fw_ver[NIMA_PROTO_FW_VERSION_LEN_MAX]; //Currently, just for WIFI chipset QV940
} NIMAProtocol_Netconf_t;

typedef struct {
	HCHAR	ipAddr[NIMA_PROTO_MAX_ADDR];
} NIMAProtocol_cmd_ntp_t;

// wifi info
typedef struct
{
	HCHAR				essid[NIMA_PROTO_AP_ESSID];
	HCHAR 				apkey[NIMA_PROTO_AP_KEY];
	HUINT8				macaddr[NIMA_PROTO_MAX_MACADDR];
	HINT32  			wave_strength;
	HINT32  			connect_speed;
	eNimaWifiOperation	op_mode;
	eNimaWifiSecurity	secure_type;
	eNimaWifiAuth 		authen_type;
	eNimaWifiEncrypt	encrypt_type;
	HBOOL				is_connected;
} NIMAProtocol_APInfo_t;

typedef struct
{
	HINT32 id;
	NIMAProtocol_APInfo_t ap;
} NIMAProtocol_WrapApInfo_t;

typedef struct {
	HINT32 dev_id;
	HINT32 count;
	NIMAProtocol_APInfo_t ap[0];
} NIMAProtocol_Wifi_t;

typedef struct {
	HINT32 dev_id;
	eNimaWifiWPS wps_type;
    HCHAR pin_code[NIMA_PROTO_WPS_PINCODE];
} NIMAProtocol_WPS_t;

typedef struct {
	HINT32 dev_id;
	eNimaWifiWoWlan mode;
} NIMAProtocol_WoWLAN_t;

// dhcpserver info
typedef struct
{
	HINT32 		id;
	HBOOL		bActive;
} NIMAProtocol_Dhcpserver_t;

typedef struct
{
	HINT32 		id;
	HINT32		priority;
} NIMAProtocol_priority_t;

//bluetooth protocol
typedef struct
{
	HBOOL enable;
	HINT32 profile;
	HCHAR bdaddr[NIMA_PROTO_BT_BDADDR_LEN];
	HCHAR pincode[NIMA_PROTO_BT_PINCODE_LEN];
} NIMAProtocol_cmd_bt_t;

// BT Device Info
typedef struct
{
	eNimaBTStatus		status;
	eNimaBTProfile		support_profile;
	HCHAR				name[NIMA_PROTO_BT_NAME_MAX];
	HUINT8				bdaddr[NIMA_PROTO_BT_BDADDR_LEN];
	HUINT8				dev_class[NIMA_PROTO_BT_CLASS_LEN];
} NIMAProtocol_bt_dev_t;

// BT Device List
typedef struct
{
	HBOOL			enable;
	NIMAProtocol_bt_dev_t host_info;
	HINT32			passkey;
	HUINT8			connected_bdaddr[NIMA_PROTO_BT_BDADDR_LEN];
	HINT32			dev_count;
	NIMAProtocol_bt_dev_t dev_info[NIMA_PROTO_BT_DEV_MAX];
} NIMAProtocol_bt_info_t;

// PPPOE
typedef struct
{
	HINT32  dev_id;
	HBOOL	bOn;
	HCHAR 	szUserID[NIMA_PROTO_PPPOE_ID_LEN_MAX];
	HCHAR 	szUserPW[NIMA_PROTO_PPPOE_PW_LEN_MAX];
} NIMAProtocol_PPPOE_info_t;

// device info
typedef struct {
	HINT32 dev_count;
	HINT32 default_dev;
	HINT32 netconf_count;
	NIMAProtocol_APInfo_t apinfo;
	NIMAProtocol_Netconf_t netconf[eNIMA_NETCONF_MAX];
	NIMAProtocol_PPPOE_info_t pppoeinfo;
	NIMAProtocol_bt_info_t btinfo;
} NIMAProtocol_devinfo_t;


//2 #### data packet
typedef struct _nimapacket_t {
	HBOOL		sync;
	HCHAR		type;
	HINT32		code;
	HINT32		dlen;
	HCHAR		data[0];
} __attribute__((packed)) nimapacket_t;

#endif /* __NIMA_PROTOCOL_H__ */
