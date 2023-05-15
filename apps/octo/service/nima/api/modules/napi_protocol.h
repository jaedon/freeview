/** **********************************************************************************************************
	File 		: napi_protocol.h
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

#ifndef __NAPI_PROTOCOL_H__
#define __NAPI_PROTOCOL_H__

#include "htype.h"
#include "napi_def.h"

#define NAPI_SERVER_UDS_PATH		"/tmp/.nima_signal.tmp"

#define NAPI_SERV_MAGIC_NO			0xFEFEFEFE

#define NAPI_PROTO_AP_ESSID		(33)
#define NAPI_PROTO_AP_KEY		(128)
#define NAPI_PROTO_WPS_PINCODE	(64)
#define NAPI_PROTO_MAX_AP 		(50)
#define	NAPI_PROTO_MAX_IP4ADDR	(4)
#define NAPI_PROTO_MAX_IP6ADDR	(16)
#define	NAPI_PROTO_MAX_MACADDR	(6)
#define NAPI_PROTO_MAX_INTERFACE_NAME (16)
#define NAPI_PROTO_MAX_ADDR 	(256)
#define NAPI_PROTO_BT_NAME_MAX 		(248)
#define NAPI_PROTO_BT_BDADDR_LEN	(6)
#define NAPI_PROTO_BT_DEV_MAX 		(20)
#define NAPI_PROTO_BT_PINCODE_LEN	(16)
#define NAPI_PROTO_BT_CLASS_LEN		(3)
#define NAPI_PROTO_PPPOE_USER_ID_LEN_MAX	(64)
#define NAPI_PROTO_PPPOE_USER_PW_LEN_MAX	(128)
#define NAPI_PROTO_FW_VERSION_LEN_MAX	(128)

typedef enum {
	NAPI_PROTOTYPE_REQUEST		= 0,
	NAPI_PROTOTYPE_REPLY
} eNApiProtoType;

typedef enum {
	eNAPI_PROTO_NETCONF_LAN			= 0,
	eNAPI_PROTO_NETCONF_WAN,
	eNAPI_PROTO_NETCONF_WIFI,
	eNAPI_PROTO_NETCONF_CM,
	eNAPI_PROTO_NETCONF_BLUETOOTH,
	eNAPI_PROTO_NETCONF_RF4CE,
	eNAPI_PROTO_NETCONF_PPPOE,
	eNAPI_PROTO_NETCONF_MAX,
} eNapiProtoNetConfDevType;

typedef enum {
	eNAPI_PROTO_NETCONF_IPV4			= 0,
	eNAPI_PROTO_NETCONF_IPV6
} eNapiProtoNetConfIpVersion;

typedef enum {
	NAPI_PROTO_NETCONF_STATUS_Undefined,
	NAPI_PROTO_NETCONF_STATUS_Operation,
	NAPI_PROTO_NETCONF_STATUS_Reboot,
	NAPI_PROTO_NETCONF_STATUS_Shutdown,
	NAPI_PROTO_NETCONF_STATUS_RebootDone,
	NAPI_PROTO_NETCONF_STATUS_ShutdownDone,
	NAPI_PROTO_NETCONF_STATUS_Unknow
} eNapiProtoNetStatus;


typedef enum {
	NAPI_PROTO_CONNECT			= 0,
	NAPI_PROTO_DISCONNECT,
	NAPI_PROTO_GET_NET_INFO,
	NAPI_PROTO_SET_NET_INFO,
	NAPI_PROTO_SET_DYNAMIC_IP,
	NAPI_PROTO_SET_STATIC_IP,
	NAPI_PROTO_SET_AUTO_IP,
	NAPI_PROTO_SHUTDOWN_ETH,
	NAPI_PROTO_DEV_UP,
	NAPI_PROTO_DEV_DOWN,
	NAPI_PROTO_DEV_UPDATE,
	NAPI_PROTO_SET_DEV_PRIORITY,
	NAPI_PROTO_SET_NTP_CLIENT,
	NAPI_PROTO_WIFI_SCAN_START,
	NAPI_PROTO_WIFI_SCAN_STOP,
	NAPI_PROTO_WIFI_CONNECT_AP,
	NAPI_PROTO_WIFI_CONNECT_WPS,
	NAPI_PROTO_WIFI_GET_WPS_PINCODE,
	NAPI_PROTO_WIFI_DISCONNECT_AP,
	NAPI_PROTO_WIFI_SET_WAKEUP_ON_WLAN,
	NAPI_PROTO_DHCPSERVER_START,
	NAPI_PROTO_DHCPSERVER_STOP,
	NAPI_PROTO_SET_CM_DOWNSTREAM,
	NAPI_PROTO_BT_ENABLE,
	NAPI_PROTO_BT_SCAN,
	NAPI_PROTO_BT_SEND_PINCODE,
	NAPI_PROTO_BT_CONNECT,
	NAPI_PROTO_BT_DISCONNECT,
	NAPI_PROTO_BT_REMOVE,
	NAPI_PROTO_BT_OPEN_PROFILE,
	NAPI_PROTO_BT_CLOSE_PROFILE,
	NAPI_PROTO_BT_DEVLIST_UPDATE,
	NAPI_PROTO_PPPOE_CONNECT,
	NAPI_PROTO_PPPOE_DISCONNECT,
	NAPI_PROTO_PPPOE_SET_INFO,
	NAPI_PROTO_PPPOE_SET_ONOFF,
} eNApiProtocol;

typedef enum {
	NAPI_PROTO_EVENT_NETCONF 	= 0x1000,
	NAPI_PROTO_EVENT_WIFI		= 0x2000,
	NAPI_PROTO_EVENT_CM			= 0x4000,
	NAPI_PROTO_EVENT_BLUETOOTH	= 0x6000,
	NAPI_PROTO_EVENT_PPPOE		= 0x8000,

	NAPI_PROTO_EVENT_NETCONF_CONN_SUCCESS = NAPI_PROTO_EVENT_NETCONF + 1,
	NAPI_PROTO_EVENT_NETCONF_CONN_FAIL,
	NAPI_PROTO_EVENT_NETCONF_AUTOIP_SUCCESS,
	NAPI_PROTO_EVENT_NETCONF_AUTOIP_FAIL,
	NAPI_PROTO_EVENT_NETCONF_LINK_CONNECTED,
	NAPI_PROTO_EVENT_NETCONF_LINK_DISCONNECTED,
	NAPI_PROTO_EVENT_NETCONF_STATUS_UPDATED,
	NAPI_PROTO_EVENT_NETCONF_DHCPSERVER_START,
	NAPI_PROTO_EVENT_NETCONF_DHCPSERVER_STOP,
	NAPI_PROTO_EVENT_WIFI_INSERTED = NAPI_PROTO_EVENT_WIFI + 1,
	NAPI_PROTO_EVENT_WIFI_EXTRACTED,
	NAPI_PROTO_EVENT_WIFI_SCANNING_START,
	NAPI_PROTO_EVENT_WIFI_SCANNING_END,
	NAPI_PROTO_EVENT_WIFI_SCANNING_OUTPUT,
	NAPI_PROTO_EVENT_WIFI_SCANNING_FAIL,
	NAPI_PROTO_EVENT_WIFI_KEY_INVALID,
	NAPI_PROTO_EVENT_WIFI_AP_CONNECT_SUCCESS,
	NAPI_PROTO_EVENT_WIFI_AP_CONNECT_FAIL,
	NAPI_PROTO_EVENT_WIFI_WPS_SUCCESS,
	NAPI_PROTO_EVENT_WIFI_WPS_FAIL,
	NAPI_PROTO_EVENT_WIFI_AP_DISCONNECTED,
	NAPI_PROTO_EVENT_WIFI_AP_RECONNECTED,
	NAPI_PROTO_EVENT_WIFI_AP_CONNECT_STOP,
	NAPI_PROTO_EVENT_WIFI_LINK_UPDATED,
	NAPI_PROTO_EVENT_BT_INSERTED = NAPI_PROTO_EVENT_BLUETOOTH + 1,
	NAPI_PROTO_EVENT_BT_EXTRACTED,
	NAPI_PROTO_EVENT_BT_ENABLE,
	NAPI_PROTO_EVENT_BT_DISABLE,
	NAPI_PROTO_EVENT_BT_SCAN_SUCCESS,
	NAPI_PROTO_EVENT_BT_SCAN_FAIL,
	NAPI_PROTO_EVENT_BT_AUTHEN_SUCCESS,
	NAPI_PROTO_EVENT_BT_AUTHEN_FAIL,
	NAPI_PROTO_EVENT_BT_PASSKEY,
	NAPI_PROTO_EVENT_BT_CONNECT_SUCCESS,
	NAPI_PROTO_EVENT_BT_CONNECT_FAIL,
	NAPI_PROTO_EVENT_BT_REMOVED,
	NAPI_PROTO_EVENT_BT_REMOTE_CMD,
	NAPI_PROTO_EVENT_BT_MAX,
	NAPI_PROTO_EVENT_PPPOE_CONNECTED = NAPI_PROTO_EVENT_PPPOE + 1,
	NAPI_PROTO_EVENT_PPPOE_DISCONNECTED,
	NAPI_PROTO_EVENT_PPPOE_SET_INFO,
	NAPI_PROTO_EVENT_PPPOE_GET_INFO,
	NAPI_PROTO_EVENT_PPPOE_SET_ONOFF,
}eNapiProtoEvent;

typedef enum {
	eNAPI_TYPE_WIFI_OP_AUTO			= 0,
	eNAPI_TYPE_WIFI_OP_ADHOC,
	eNAPI_TYPE_WIFI_OP_INFRA,
	eNAPI_TYPE_WIFI_OP_MASTER,
	eNAPI_TYPE_WIFI_OP_REPEATER,
	eNAPI_TYPE_WIFI_OP_SECONDARY,
	eNAPI_TYPE_WIFI_OP_MONITOR,
	eNAPI_TYPE_WIFI_OP_UNKNOWN
} eNapiProtoWifiOperation;

typedef enum {
	eNAPI_TYPE_WIFI_SECURITY_OPEN		= 0,
	eNAPI_TYPE_WIFI_SECURITY_WEP,
	eNAPI_TYPE_WIFI_SECURITY_WPA,
	eNAPI_TYPE_WIFI_SECURITY_WPA2,
	eNAPI_TYPE_WIFI_SECURITY_WPA_WPA2,
	eNAPI_TYPE_WIFI_SECURITY_UNKNOWN
} eNapiProtoWifiSecurity;

typedef enum {
	eNAPI_TYPE_WIFI_AUTH_OPEN			= 0,
	eNAPI_TYPE_WIFI_AUTH_PSK,
	eNAPI_TYPE_WIFI_AUTH_802_1X,
	eNAPI_TYPE_WIFI_AUTH_UNKNOWN,
	eNAPI_TYPE_WIFI_AUTH_MAX
} eNapiProtoWifiAuth;

typedef enum {
	eNAPI_TYPE_WIFI_ENCRYPT_NONE 		= 0, /**< needs to 10 hex digits shared key */
	eNAPI_TYPE_WIFI_ENCRYPT_WEP_64,		/*DI_WIFI_ENCRYPT_WEP_64_ASCII*< needs to 10 hex digits shared key */
	eNAPI_TYPE_WIFI_ENCRYPT_WEP_64HEX,
	eNAPI_TYPE_WIFI_ENCRYPT_WEP_128, 	/*DI_WIFI_ENCRYPT_WEP_128_ASCII*< needs to 26 hex digits shared key */
	eNAPI_TYPE_WIFI_ENCRYPT_WEP_128HEX,
	eNAPI_TYPE_WIFI_ENCRYPT_TKIP,
	eNAPI_TYPE_WIFI_ENCRYPT_AES,
	eNAPI_TYPE_WIFI_ENCRYPT_TKIP_AES,
	eNAPI_TYPE_WIFI_ENCRYPT_UNKNOWN,
	eNAPI_TYPE_WIFI_ENCRYPT_MAX
} eNapiProtoWifiEncrypt;

typedef enum {
	eNAPI_PROTO_WPS_PIN,
	eNAPI_PROTO_WPS_PBC,
	eNAPI_PROTO_WPS_UNKNOWN,
	eNAPI_PROTO_WPS_MAX,
} eNapiProtoWifiWPS;

//bluetooth enum
typedef enum {
	NAPI_PROTO_BT_Profile_Unknown	= 0x00000000,
	NAPI_PROTO_BT_Profile_A2DP		= 0x00040000,	/* Adavanced Audio Distribution */
	NAPI_PROTO_BT_Profile_AVRCP		= 0x00080000,	/* Audio/Video Remote Control */
	NAPI_PROTO_BT_Profile_HID		= 0x00100000,	/* Human Interface Device */
	NAPI_PROTO_BT_Profile_All		= 0x1FFFFFFF,
	NAPI_PROTO_BT_Profile_Auto		= 0x80000000,
} eNapiProtoBTProfile;

typedef enum {
	NAPI_PROTO_BT_STATUS_NotPaired = 0,
	NAPI_PROTO_BT_STATUS_Connected,
	NAPI_PROTO_BT_STATUS_Disconnected,
	NAPI_PROTO_BT_STATUS_Unknown,
	NAPI_PROTO_BT_STATUS_PinRequested,
	NAPI_PROTO_BT_STATUS_ProfileStarted
} eNapiProtoBTStatus;

typedef enum {
	NAPI_PROTO_BT_DEVTYPE_Misc,
	NAPI_PROTO_BT_DEVTYPE_Computer,
	NAPI_PROTO_BT_DEVTYPE_Phone,
	NAPI_PROTO_BT_DEVTYPE_Network,
	NAPI_PROTO_BT_DEVTYPE_AV,
	NAPI_PROTO_BT_DEVTYPE_Peripheral,
	NAPI_PROTO_BT_DEVTYPE_Imaging,
	NAPI_PROTO_BT_DEVTYPE_Unknown
} eNapiProtoBTType;

typedef enum {
	NAPI_PROTO_PPPOE_STATUS_CONNECTED,
	NAPI_PROTO_PPPOE_STATUS_DISCONNECTED,
	NAPI_PROTO_PPPOE_STATUS_IN_PROGRESS,
	NAPI_PROTO_PPPOE_STATUS_UNKNOWN,
} eNapiProtoPPPOEType;


// netconf info
typedef union {
	struct {
		HUINT8	macaddr[NAPI_PROTO_MAX_MACADDR];
		HUINT8	ipAddr[NAPI_PROTO_MAX_IP4ADDR];
		HUINT8	netmask[NAPI_PROTO_MAX_IP4ADDR];
		HUINT8	gateway[NAPI_PROTO_MAX_IP4ADDR];
		HUINT8	dns1[NAPI_PROTO_MAX_IP4ADDR];
		HUINT8	dns2[NAPI_PROTO_MAX_IP4ADDR];
	} ipv4;

	struct {
		HUINT8	macaddr[NAPI_PROTO_MAX_MACADDR];
		HUINT8	ipAddr[NAPI_PROTO_MAX_IP6ADDR];
		HUINT8	gateway[NAPI_PROTO_MAX_IP6ADDR];
		HUINT8	dns1[NAPI_PROTO_MAX_IP6ADDR];
		HUINT8	dns2[NAPI_PROTO_MAX_IP6ADDR];
	} ipv6;
} NAPIProtocol_IpInfo_t;

typedef struct {
	HINT32 id;
	HINT32 priority;
	HCHAR name[NAPI_PROTO_MAX_INTERFACE_NAME];
	eNapiProtoNetConfDevType type;
	HBOOL linked;
	HBOOL active;
	HBOOL dhcp;
	HBOOL dns_auto;
	HBOOL auto_ip;
	eNapiProtoNetConfIpVersion ip_ver;
	NAPIProtocol_IpInfo_t ip_info;
	HCHAR fw_ver[NAPI_PROTO_FW_VERSION_LEN_MAX]; //Currently, just for WIFI chipset QV940
} NAPIProtocol_Netconf_t;

typedef struct {
	HCHAR	ipAddr[NAPI_PROTO_MAX_ADDR];
} NAPIProtocol_cmd_ntp_t;
// wifi info
typedef struct
{
	HCHAR essid[NAPI_PROTO_AP_ESSID];
	HCHAR apkey[NAPI_PROTO_AP_KEY];
	HUINT8 macaddr[NAPI_PROTO_MAX_MACADDR];
	HINT32 wave_strength;
	HINT32 connect_speed;
	eNapiProtoWifiOperation op_mode;
	eNapiProtoWifiSecurity secure_type;
	eNapiProtoWifiAuth authen_type;
	eNapiProtoWifiEncrypt encrypt_type;
	HINT32 is_connected;
} NAPIProtocol_APInfo_t;

typedef struct {
	HINT32 dev_id;
	HINT32 count;
	NAPIProtocol_APInfo_t ap[0];
} NAPIProtocol_Wifi_t;

typedef struct {
	HINT32 dev_id;
	eNapiProtoWifiWPS wps_type;
	HCHAR pin_code[NAPI_PROTO_WPS_PINCODE];
} NAPIProtocol_WPS_t;

typedef struct {
	HINT32 dev_id;
	eNapiWifiWoWlan mode;
} NAPIProtocol_WoWLAN_t;

// dhcpserver info
typedef struct
{
	HINT32 		id;
	HBOOL		bActive;
} NAPIProtocol_Dhcpserver_t;

typedef struct
{
	HINT32 		id;
	HINT32		priority;
} NAPIProtocol_priority_t;

//bluetooth protocol
typedef struct
{
	HBOOL enable;
	HINT32 profile;
	HCHAR bdaddr[NAPI_PROTO_BT_BDADDR_LEN];
	HCHAR pincode[NAPI_PROTO_BT_PINCODE_LEN];
} NAPIProtocol_cmd_bt_t;

// BT Device Info
typedef struct
{
	eNapiProtoBTStatus		status;
	eNapiProtoBTProfile		support_profile;
	HCHAR				name[NAPI_PROTO_BT_NAME_MAX];
	HUINT8				bdaddr[NAPI_PROTO_BT_BDADDR_LEN];
	HUINT8				dev_class[NAPI_PROTO_BT_CLASS_LEN];
} NAPIProtocol_bt_dev_t;

// BT Device List
typedef struct
{
	HBOOL			enable;
	NAPIProtocol_bt_dev_t host_info;
	HINT32			passkey;
	HUINT8			connected_bdaddr[NAPI_PROTO_BT_BDADDR_LEN];
	HINT32			dev_count;
	NAPIProtocol_bt_dev_t dev_info[NAPI_PROTO_BT_DEV_MAX];
} NAPIProtocol_bt_info_t;

typedef struct {
	HINT32					dev_id;
	HBOOL					bOn;
	HCHAR 					szUserID[NAPI_PROTO_PPPOE_USER_ID_LEN_MAX];
	HCHAR 					szUserPW[NAPI_PROTO_PPPOE_USER_PW_LEN_MAX];
} NAPIProtocol_PPPOE_info_t;


// device info
typedef struct {
	HINT32 dev_count;
	HINT32 default_dev;
	HINT32 netconf_count;
	NAPIProtocol_APInfo_t apinfo;
	NAPIProtocol_Netconf_t netconf[eNAPI_PROTO_NETCONF_MAX];
	NAPIProtocol_PPPOE_info_t pppoeinfo;
	NAPIProtocol_bt_info_t btinfo;
} NAPIProtocol_devinfo_t;

//2 #### data packet
typedef struct _napipacket_t {
	HBOOL 		sync;
	HCHAR		type;
	HINT32		code;
	HINT32		dlen;
	HCHAR		data[0];
} PACKED napipacket_t;

#endif /* __NAPI_PROTOCOL_H__ */
