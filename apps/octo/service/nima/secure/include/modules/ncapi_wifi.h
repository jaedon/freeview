/** **********************************************************************************************************
	File 		: napi_wifi.h
	author 		:
	comment		: WIFI
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


#ifndef __NCAPI_WIFI_H__
#define __NCAPI_WIFI_H__

#include "htype.h"

/*-----------------------------------------------------------------------------------------------
 *	pre-definition
 *----------------------------------------------------------------------------------------------*/
#define	NCAPI_MAX_MACADDR		(6)
#define NCAPI_AP_ESSID			(33)
#define NCAPI_AP_KEY			(128)
#define	NCAPI_MAX_WPS_CODE_LEN 	(64)
#define NCAPI_MAX_ID			(32)
#define NCAPI_MAX_AP 			(50)

typedef enum {
	eNCAPI_WIFI_EVENT_INSERTED,
	eNCAPI_WIFI_EVENT_EXTRACTED,
	eNCAPI_WIFI_EVENT_SCANNING_START,
	eNCAPI_WIFI_EVENT_SCANNING_END,
	eNCAPI_WIFI_EVENT_SCANNING_OUTPUT,
	eNCAPI_WIFI_EVENT_SCANNING_FAIL,
	eNCAPI_WIFI_EVENT_KEY_INVALID,
	eNCAPI_WIFI_EVENT_AP_CONNECT_SUCCESS,
	eNCAPI_WIFI_EVENT_AP_CONNECT_FAIL,
	eNCAPI_WIFI_EVENT_WPS_SUCCESS,
	eNCAPI_WIFI_EVENT_WPS_FAIL,
	eNCAPI_WIFI_EVENT_AP_DISCONNECTED,
	eNCAPI_WIFI_EVENT_AP_RECONNECTED,
	eNCAPI_WIFI_EVENT_AP_CONNECT_STOP,
	eNCAPI_WIFI_EVENT_LINK_UPDATED,
	eNCAPI_WIFI_EVENT_UNKNOWN,
} eNcapiWifiEvent;

typedef enum {
	eNCAPI_WIFI_OP_AUTO			= 0,
	eNCAPI_WIFI_OP_ADHOC,
	eNCAPI_WIFI_OP_INFRA,
	eNCAPI_WIFI_OP_MASTER,
	eNCAPI_WIFI_OP_REPEATER,
	eNCAPI_WIFI_OP_SECONDARY,
	eNCAPI_WIFI_OP_MONITOR,
	eNCAPI_WIFI_OP_UNKNOWN
} eNcapiWifiOperation;

typedef enum {
	eNCAPI_WIFI_SECURITY_OPEN		= 0,
	eNCAPI_WIFI_SECURITY_WEP,
	eNCAPI_WIFI_SECURITY_WPA,
	eNCAPI_WIFI_SECURITY_WPA2,
	eNCAPI_WIFI_SECURITY_WPA_WPA2,
	eNCAPI_WIFI_SECURITY_UNKNOWN
} eNcapiWifiSecurity;

typedef enum {
	eNCAPI_WIFI_AUTH_OPEN			= 0,
	eNCAPI_WIFI_AUTHEN_PSK,
	eNCAPI_WIFI_AUTHEN_802_1X,
	eNCAPI_WIFI_AUTHEN_UNKNOWN,
	eNCAPI_WIFI_AUTHEN_MAX
#if 0
	eNCAPI_WIFI_AUTH_OPEN			= 0,
	eNCAPI_WIFI_AUTH_SHARED,
	eNCAPI_WIFI_AUTH_WEPAUTO,
	eNCAPI_WIFI_AUTH_WPAPSK,
	eNCAPI_WIFI_AUTH_WPA2PSK,
	eNCAPI_WIFI_AUTH_802_1x,
	eNCAPI_WIFI_AUTH_UNKNOWN
#endif
} eNcapiWifiAuth;

typedef enum {
	eNCAPI_WIFI_ENCRYPT_NONE			= 0, /**< needs to 10 hex digits shared key */
	eNCAPI_WIFI_ENCRYPT_WEP_64, 		/*DI_WIFI_ENCRYPT_WEP_64_ASCII*< needs to 10 hex digits shared key */
	eNCAPI_WIFI_ENCRYPT_WEP_64HEX,
	eNCAPI_WIFI_ENCRYPT_WEP_128,		/*DI_WIFI_ENCRYPT_WEP_128_ASCII*< needs to 26 hex digits shared key */
	eNCAPI_WIFI_ENCRYPT_WEP_128HEX,
	eNCAPI_WIFI_ENCRYPT_TKIP,
	eNCAPI_WIFI_ENCRYPT_AES,
	eNCAPI_WIFI_ENCRYPT_TKIP_AES,
	eNCAPI_WIFI_ENCRYPT_UNKNOWN,
	eNCAPI_WIFI_ENCRYPT_MAX
#if 0
	eNCAPI_WIFI_ENCRYPT_NONE			= 0,
	eNCAPI_WIFI_ENCRYPT_WEP_64,
	eNCAPI_WIFI_ENCRYPT_WPA_TKIP,
	eNCAPI_WIFI_ENCRYPT_WRAP,
	eNCAPI_WIFI_ENCRYPT_WPA_AES,
	eNCAPI_WIFI_ENCRYPT_WPA_TKIP_AES,
	eNCAPI_WIFI_ENCRYPT_WEP_128,
	eNCAPI_WIFI_ENCRYPT_WEP_64_HEX,
	eNCAPI_WIFI_ENCRYPT_WEP_64_ASCII,
	eNCAPI_WIFI_ENCRYPT_WEP_128_HEX,
	eNCAPI_WIFI_ENCRYPT_WEP_128_ASCII,
	eNCAPI_WIFI_ENCRYPT_UNKNOWN
#endif
} eNcapiWifiEncrypt;

typedef enum {
	eNCAPI_WIFI_WPS_NONE,
	eNCAPI_WIFI_WPS_PBC,
	eNCAPI_WIFI_WPS_PIN,
	eNCAPI_WIFI_WPS_UNKNOWN,
	eNCAPI_WIFI_WPS_MAX,
} eNcapiWifiWps;

typedef enum
{
	eNCAPI_WIFI_WOWL_NONE			= 0x00,		/* Do not wake up on any event. */
	eNCAPI_WIFI_WOWL_MAGIC_PATTERN	= 0x01,		/* Wake up on magic pattern. */
	eNCAPI_WIFI_WOWL_NET_PATTERN		= 0x02,		/* Wake up on a special net pattern. */
	eNCAPI_WIFI_WOWL_DISASSOC_DEAUTH	= 0x04,		/* Wake up on disassociate from AP. */
	eNCAPI_WIFI_WOWL_RETROGRADE_TSF	= 0x08,		/* Wake up on retrograde tsf. */
	eNCAPI_WIFI_WOWL_BCN_LOSS		= 0x10,		/* Wake up on loss of beacon. */
} eNcapiWifiWoWlan ;

typedef struct
{
	eNcapiWifiWps eType;
	HCHAR	szCode[NCAPI_MAX_WPS_CODE_LEN + 1 ];
} __attribute__((packed)) NCAPI_WifiWPSInfo_t;

typedef struct
{
	HCHAR				essid[NCAPI_AP_ESSID];
	HUINT32				channel;
	HUINT32				freq;
	HBOOL				is_wps;
	eNcapiWifiOperation	op_mode;
	HUINT8				macaddr[NCAPI_MAX_MACADDR];
	HINT32  			wave_strength;
	HINT32				connect_speed;
	eNcapiWifiSecurity	secure_type;
	eNcapiWifiAuth 		authen_type;
	eNcapiWifiEncrypt	encrypt_type;
	HUINT32				keyindex;
	HUINT8				id[NCAPI_MAX_ID];
	HCHAR 				apkey[NCAPI_AP_KEY];
	HBOOL				is_connected;
} __attribute__((packed)) NCAPI_WifiAPInfo_t;

typedef void (*NCAPI_WifiCallback) (eNcapiWifiEvent event, HINT32 device_id, HINT32 count,
										NCAPI_WifiAPInfo_t *data);

/*-----------------------------------------------------------------------------------------------
 *	interfaces
 *----------------------------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Sample Subject
 *
 * @param[in] server the server instance
 * @param[in] ipaddr the IP address
 * @param[in] port the port number
 *
 * @return ERR_OK on success. if error, return ERR_FAIL.
 */

/**
 * initialize wifi module for ncapi
 *
 * @param[in] void
 *
 * @return ERR_OK on success. if error, return ERR_FAIL.
 */
extern HERROR NCAPI_Wifi_Init(void);
extern HERROR NCAPI_Wifi_Deinit(void);

extern HERROR NCAPI_Wifi_RegisterCallback(NCAPI_WifiCallback callback);

extern HERROR NCAPI_Wifi_UnregisterCallback(NCAPI_WifiCallback callback);

extern HINT32 NCAPI_Wifi_Get_AplistInfo(HINT32 id, HINT32 *ap_count, NCAPI_WifiAPInfo_t *info );

/**
 * Start scan wifi.
 *
 * @param	void
 */
extern HERROR NCAPI_Wifi_ScanStart(HINT32 id);

/**
 * Stop scan wifi.
 *
 * @param	void
 */
extern HERROR NCAPI_Wifi_ScanStop(HINT32 id);

/**
 * Connect to wlan using ap information.
 *
 * @param	info ap information structure
 */
extern HERROR NCAPI_Wifi_Connect(HINT32 id, const NCAPI_WifiAPInfo_t *info);

/**
 * Connect to wps using pin_code information.
 *
 * @param	info ap information structure
 */
extern HERROR NCAPI_Wifi_Connect_byWPS(HINT32 id, const NCAPI_WifiWPSInfo_t *info);

/**
 * Disconnect wlan network connection.
 *
 * @param	void
 */
extern HERROR NCAPI_Wifi_Disconnect(HINT32 id);

/**
 * Get Pincode by WPS
 *
 * @param	void
 */
extern HERROR NCAPI_Wifi_GetPinCodebyWPS(HINT32 id, HCHAR *szPincode);

/**
 * Start scan ap by the network device id.
 *
 * @param info ap information structure
 */
extern HERROR NCAPI_Wifi_GetConnectedAPInfo(HINT32 id, HINT32 *apcount, NCAPI_WifiAPInfo_t *info);

/**
* This function is used to configure WoWLAN feature
* @param[in] id: index of wifi network interface
* @param[in] mode: chosen mode
* @return ERR_OK if this WoWLAN feature is configured succesfully, ERR_FAIL if vice versa
*/

HERROR NCAPI_Wifi_SetWakeupOnWlan(HINT32 id, eNcapiWifiWoWlan mode);


#ifdef __cplusplus
}
#endif

#endif	/* __NCAPI_WIFI_H__ */
