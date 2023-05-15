/** **********************************************************************************************************
	File 		: napi_def.h
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

#ifndef __NAPI_DEF_H__
#define __NAPI_DEF_H__

#include "htype.h"

#define	NAPI_MAX_IP4ADDR			(16)
#define NAPI_MAX_IP6ADDR			(40)
#define	NAPI_MAX_MACADDR			(20)
#define	NAPI_MAX_INTERFACE_NAME		(16)
#define NAPI_MAX_MACADDR			(20)
#define NAPI_AP_ESSID				(48)
#define NAPI_AP_KEY					(64)
#define NAPI_MAX_AP 				(50)
#define NAPI_BT_NAME_MAX 			(248)
#define NAPI_BT_BDADDR_LEN			(20)
#define NAPI_BT_DEV_MAX 			(20)
#define NAPI_BT_PINCODE_LEN			(16)
#define NAPI_BT_DEV_CLASS			(3)
#define NAPI_PPPOE_USER_ID_LEN_MAX		(64)
#define NAPI_PPPOE_USER_PW_LEN_MAX		(128)
#define NAPI_FW_VERSION_LEN_MAX	(128)


//1 #### NAPI EVENT DEFINE
typedef enum {
	NAPI_EVENT_NETCONF_CONN_SUCCESS = 1,
	NAPI_EVENT_NETCONF_CONN_FAIL,
	NAPI_EVENT_NETCONF_AUTOIP_SUCCESS,
	NAPI_EVENT_NETCONF_AUTOIP_FAIL,
	NAPI_EVENT_NETCONF_LINK_CONNECTED,
	NAPI_EVENT_NETCONF_LINK_DISCONNECTED,
	NAPI_EVENT_NETCONF_STATUS_UPDATED,
	NAPI_EVENT_NETCONF_DHCPSERVER_START,
	NAPI_EVENT_NETCONF_DHCPSERVER_STOP,
	NAPI_EVENT_NETCONF_INTERNET_CONNECTED, //Just occur after calling NAPI_NetConf_CheckInternetConnection API
	NAPI_EVENT_NETCONF_INTERNET_DISCONNECTED, //Just occur after calling NAPI_NetConf_CheckInternetConnection API
}eNApiNetconfEvent;

typedef enum {
	NAPI_EVENT_WIFI_INSERTED = 1,
	NAPI_EVENT_WIFI_EXTRACTED,
	NAPI_EVENT_WIFI_SCANNING_START,
	NAPI_EVENT_WIFI_SCANNING_END,
	NAPI_EVENT_WIFI_SCANNING_OUTPUT,
	NAPI_EVENT_WIFI_SCANNING_FAIL,
	NAPI_EVENT_WIFI_KEY_INVALID,
	NAPI_EVENT_WIFI_AP_CONNECT_SUCCESS,
	NAPI_EVENT_WIFI_AP_CONNECT_FAIL,
	NAPI_EVENT_WIFI_WPS_SUCCESS,
	NAPI_EVENT_WIFI_WPS_FAIL,
	NAPI_EVENT_WIFI_AP_DISCONNECTED,
	NAPI_EVENT_WIFI_AP_RECONNECTED,
	NAPI_EVENT_WIFI_AP_CONNECT_STOP,
	NAPI_EVENT_WIFI_LINK_UPDATED,
}eNApiWifiEvent;

typedef enum {
	NAPI_EVENT_LIST_UPDATED = 1,
	NAPI_EVENT_CONNECTED,
	NAPI_EVENT_CONNECT_FAILED,
	NAPI_EVENT_PIN_REQUESTED,
	NAPI_EVENT_PASSKEY,
	NAPI_EVENT_DISCONNECTED,
	NAPI_EVENT_PROFILE_STARTED,
	NAPI_EVENT_PROFILE_STOPPED,
	NAPI_EVENT_AUDIO_MUTE,
	NAPI_EVENT_REMOVED,
} eNApiBTEvent;

typedef enum {
	NAPI_EVENT_PPPOE_CONNECTED = 1,
	NAPI_EVENT_PPPOE_DISCONNECTED,
	NAPI_EVENT_PPPOE_SET_INFO,
	NAPI_EVENT_PPPOE_GET_INFO,
	NAPI_EVENT_PPPOE_SET_ONOFF,
	NAPI_EVENT_PPPOE_UNKNOWN,
} eNApiPPPOEEvent;


//1 #### NAPI TYPE DEFINE
typedef enum {
	eNAPI_CM_NONE 					= 0,
	eNAPI_CM_OTHER,
	eNAPI_CM_NOT_READY,
	eNAPI_CM_NOT_SYNC,
	eNAPI_CM_PHY_SYNC_DONE,
	eNAPI_CM_PARAM_ACQUIRED,
	eNAPI_CM_RANGING_COMPLETE,
	eNAPI_CM_IP_COMPLETE,
	eNAPI_CM_TOD_CERTIFY,
	eNAPI_CM_SECURITY_CERTIFY,
	eNAPI_CM_PARAM_TRAANSE_COMPLETE,
	eNAPI_CM_REGIST_COMPLETE,
	eNAPI_CM_OPERATION,
	eNAPI_CM_ACCESS_DENIED,
} eNapiCmStatus;

typedef enum {
	eNAPI_CM_QAM_64 = 	0,
	eNAPI_CM_QAM_256,
	eNAPI_CM_QAM_OTHER,
	eNAPI_CM_QAM_UNKNOWN,
} eNapiCmQam;

typedef enum {
	eNAPI_WIFI_OP_AUTO			= 0,
	eNAPI_WIFI_OP_ADHOC,
	eNAPI_WIFI_OP_INFRA,
	eNAPI_WIFI_OP_MASTER,
	eNAPI_WIFI_OP_REPEATER,
	eNAPI_WIFI_OP_SECONDARY,
	eNAPI_WIFI_OP_MONITOR,
	eNAPI_WIFI_OP_UNKNOWN
} eNapiWifiOperation;

typedef enum {
	eNAPI_WIFI_SECURITY_OPEN		= 0,
	eNAPI_WIFI_SECURITY_WEP,
	eNAPI_WIFI_SECURITY_WPA,
	eNAPI_WIFI_SECURITY_WPA2,
	eNAPI_WIFI_SECURITY_WPA_WPA2,
	eNAPI_WIFI_SECURITY_UNKNOWN
} eNapiWifiSecurity;

typedef enum {
	eNAPI_WIFI_AUTH_OPEN			= 0,
	eNAPI_WIFI_AUTH_SHARED,
	eNAPI_WIFI_AUTH_WEPAUTO,
	eNAPI_WIFI_AUTH_WPAPSK,
	eNAPI_WIFI_AUTH_WPA2PSK,
	eNAPI_WIFI_AUTH_802_1x,
	eNAPI_WIFI_AUTH_UNKNOWN
} eNapiWifiAuth;

typedef enum {
	eNAPI_WIFI_ENCRYPT_NONE 		= 0,
	eNAPI_WIFI_ENCRYPT_WEP_64,
	eNAPI_WIFI_ENCRYPT_WPA_TKIP,
	eNAPI_WIFI_ENCRYPT_WRAP,
	eNAPI_WIFI_ENCRYPT_WPA_AES,
	eNAPI_WIFI_ENCRYPT_WPA_TKIP_AES,
	eNAPI_WIFI_ENCRYPT_WEP_128,
	eNAPI_WIFI_ENCRYPT_WEP_64_HEX,
	eNAPI_WIFI_ENCRYPT_WEP_64_ASCII,
	eNAPI_WIFI_ENCRYPT_WEP_128_HEX,
	eNAPI_WIFI_ENCRYPT_WEP_128_ASCII,
	eNAPI_WIFI_ENCRYPT_UNKNOWN
} eNapiWifiEncrypt;

typedef enum {
	eNAPI_WIFI_WPS_PIN,
	eNAPI_WIFI_WPS_PBC,
	eNAPI_WIFI_WPS_UNKNOWN,
	eNAPI_WIFI_WPS_MAX,
} eNapiWifiWPS;

typedef enum
{
	eNAPI_WIFI_WOWL_NONE			= 0x00,		/* Do not wake up on any event. */
	eNAPI_WIFI_WOWL_MAGIC_PATTERN	= 0x01,		/* Wake up on magic pattern. */
	eNAPI_WIFI_WOWL_NET_PATTERN		= 0x02,		/* Wake up on a special net pattern. */
	eNAPI_WIFI_WOWL_DISASSOC_DEAUTH	= 0x04,		/* Wake up on disassociate from AP. */
	eNAPI_WIFI_WOWL_RETROGRADE_TSF	= 0x08,		/* Wake up on retrograde tsf. */
	eNAPI_WIFI_WOWL_BCN_LOSS		= 0x10,		/* Wake up on loss of beacon. */
} eNapiWifiWoWlan ;

typedef enum {
	eNAPI_NETCONF_LAN			= 0,
	eNAPI_NETCONF_WIFI,
	eNAPI_NETCONF_CM,
	eNAPI_NETCONF_BLUETOOTH,
	eNAPI_NETCONF_RF4CE,
	eNAPI_NETCONF_PPPOE,
} eNapiNetConfDevType;

typedef enum {
	eNAPI_NETCONF_IPV4			= 0,
	eNAPI_NETCONF_IPV6
} eNapiNetConfIpVersion;

typedef enum {
	eNAPI_BT_Profile_Unknown	= 0x00000000,
	eNAPI_BT_Profile_A2DP		= 0x00040000,	/* Adavanced Audio Distribution */
	eNAPI_BT_Profile_AVRCP		= 0x00080000,	/* Audio/Video Remote Control */
	eNAPI_BT_Profile_HID		= 0x00100000,	/* Human Interface Device */
	eNAPI_BT_Profile_All		= 0x1FFFFFFF,
	eNAPI_BT_Profile_Auto		= 0x80000000,
} eNapiBTProfile;

typedef enum {
	eNAPI_BT_STATUS_NotPaired = 0,
	eNAPI_BT_STATUS_Connected,
	eNAPI_BT_STATUS_Disconnected,
	eNAPI_BT_STATUS_Unknown,
	eNAPI_BT_STATUS_PinRequested,
	eNAPI_BT_STATUS_ProfileStarted
} eNapiBTStatus;

typedef enum {
	eNAPI_BT_DEVTYPE_Misc,
	eNAPI_BT_DEVTYPE_Computer,
	eNAPI_BT_DEVTYPE_Phone,
	eNAPI_BT_DEVTYPE_Network,
	eNAPI_BT_DEVTYPE_AV,
	eNAPI_BT_DEVTYPE_Peripheral,
	eNAPI_BT_DEVTYPE_Imaging,
	eNAPI_BT_DEVTYPE_Unknown
} eNapiBTType;

typedef enum {
	eNAPI_PPPOE_STATUS_CONNECTED,
	eNAPI_PPPOE_STATUS_DISCONNECTED,
	eNAPI_PPPOE_STATUS_IN_PROGRESS,
	eNAPI_PPPOE_STATUS_UNKNOWN
} eNapiPPPOEStatus;


//1 #### NAPI STRUCT DEFINE
// cm info
typedef struct {
	eNapiCmQam	qam_type;
	HINT32 		freq;
	HINT32 		ber;
	HINT32 		snr;
	HINT32 		signal_level;
} NAPICMDownStream_t;

typedef struct {
	HINT32	 	freq;
	HINT32	 	channel_id;
	HINT32		bandwidth;
	HINT32		signal_level;
} NAPICMUpStream_t;

typedef struct {
	eNapiCmStatus status;
	NAPICMDownStream_t down_stream;
	NAPICMUpStream_t up_stream;
} NAPICMInfo_t;

// wifi info
typedef struct
{
	HCHAR				essid[NAPI_AP_ESSID];
	HCHAR				apkey[NAPI_AP_KEY];
	HCHAR				macaddr[NAPI_MAX_MACADDR];
	HINT32				wave_strength;
	HINT32				connect_speed;
	eNapiWifiOperation	op_mode;
	eNapiWifiSecurity	secure_type;
	eNapiWifiAuth		authen_type;
	eNapiWifiEncrypt	encrypt_type;
	HINT32				bIsConnected;
	
} NAPIWifiAPInfo_t;

typedef struct
{
	HINT32 dev_id;
	HINT32 count;
	NAPIWifiAPInfo_t ap[0];
} NAPIWifiInfo_t;

// netconf info
typedef union {
	struct {
		HCHAR	macaddr[NAPI_MAX_MACADDR];
		HCHAR	ipAddr[NAPI_MAX_IP4ADDR];
		HCHAR	netmask[NAPI_MAX_IP4ADDR];
		HCHAR	gateway[NAPI_MAX_IP4ADDR];
		HCHAR	dns1[NAPI_MAX_IP4ADDR];
		HCHAR	dns2[NAPI_MAX_IP4ADDR];
	} ipv4;

	struct {
		HCHAR	macaddr[NAPI_MAX_MACADDR];
		HCHAR	ipAddr[NAPI_MAX_IP6ADDR];
		HCHAR	gateway[NAPI_MAX_IP6ADDR];
		HCHAR	dns1[NAPI_MAX_IP6ADDR];
		HCHAR	dns2[NAPI_MAX_IP6ADDR];
	} ipv6;
} NAPINetConfIpInfo_t;

typedef struct {
	HINT32		id;
	HCHAR		name[NAPI_MAX_INTERFACE_NAME];
	eNapiNetConfDevType type;
	HBOOL		linked;
	HBOOL		active;
	HBOOL		dhcp;
	HBOOL		dns_auto;
	HBOOL		auto_ip;
	eNapiNetConfIpVersion	ip_ver;
	NAPINetConfIpInfo_t ip_info;
	HCHAR 		fw_ver[NAPI_FW_VERSION_LEN_MAX]; //Currently, just for WIFI chipset QV940
} NAPINetConfInfo_t;

// dhcpserver info
typedef struct
{
	HINT32		id;
	HBOOL		bActive;
} NAPIDHCPServerInfo_t;

//network device info
typedef struct {
	HINT32 dev_count;
	HINT32 default_dev;
} NAPIDevInfo_t;

// BT Device Info
typedef struct
{
	HUINT32 			dev_id;
	HCHAR				name[NAPI_BT_NAME_MAX];
	eNapiBTStatus		status;
	HUINT8 				dev_class[NAPI_BT_DEV_CLASS];
	eNapiBTProfile		support_profile;
	HCHAR				bdaddr[NAPI_BT_BDADDR_LEN];
} NAPIBTDevInfo_t;

// BT Device List
typedef struct
{
	HBOOL			enable;
	HBOOL			audio_mute;
	HINT32			passkey;
	NAPIBTDevInfo_t host_info;
	HINT32			dev_count;
	NAPIBTDevInfo_t dev_info[NAPI_BT_DEV_MAX];
} NAPIBTInfo_t;

// PPPOE Info
typedef struct
{
	HINT32		dev_id;
	HBOOL		bOn;
	HCHAR 		szUserID[NAPI_PPPOE_USER_ID_LEN_MAX];
	HCHAR 		szUserPW[NAPI_PPPOE_USER_PW_LEN_MAX];
} NAPIPPPOEInfo_t;



#endif /* __NAPI_DEF_H__ */

