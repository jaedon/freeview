/** **********************************************************************************************************
	File 		: napi_netconf.h
	author 		:
	comment		: Network Configuration
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


#ifndef __NCAPI_NETCONF_H__
#define __NCAPI_NETCONF_H__

#include "ncapi.h"

/*-----------------------------------------------------------------------------------------------
 *	pre-definition
 *----------------------------------------------------------------------------------------------*/
#define	NCAPI_MAX_IP4ADDR	(4)
#define NCAPI_MAX_IP6ADDR	(16)
#define	NCAPI_MAX_MACADDR	(6)
#define	NCAPI_MAX_NET_INTERFACE_NAME		(16)
#define NCAPI_FW_VERSION_MAX_LEN		(128)

typedef enum {
	eNCAPI_NETCONF_EVENT_CONN_SUCCESS,
	eNCAPI_NETCONF_EVENT_CONN_FAIL,
	eNCAPI_NETCONF_EVENT_AUTOIP_SUCCESS,
	eNCAPI_NETCONF_EVENT_AUTOIP_FAIL,
	eNCAPI_NETCONF_EVENT_LINK_CONNECTED,
	eNCAPI_NETCONF_EVENT_LINK_DISCONNECTED,
	eNCAPI_NETCONF_EVENT_DHCPSERVER_START,
	eNCAPI_NETCONF_EVENT_DHCPSERVER_STOP,

	eNCAPI_NETCONF_EVENT_STATUS_UPDATED,
	eNCAPI_NETCONF_EVENT_WOL_ENABLED,
	eNCAPI_NETCONF_EVENT_WOL_DISABLED,

	eNCAPI_NETCONF_EVENT_UNKNOWN,
} eNcapiNetConfEvent;

typedef enum {
	eNCAPI_NETCONF_LAN			= 0,
	eNCAPI_NETCONF_WAN,
	eNCAPI_NETCONF_WIFI,
	eNCAPI_NETCONF_CM,
	eNCAPI_NETCONF_BLUETOOTH,
	eNCAPI_NETCONF_RF4CE,
	eNCAPI_NETCONF_PPPOE,
	eNCAPI_NETCONF_MAX,
} eNcapiNetConfDevType;

typedef enum {
	eNCAPI_NETCONF_IPV4			= 0,
	eNCAPI_NETCONF_IPV6
} eNcapiNetConfIpVersion;

typedef enum {
	eNCAPI_Dns_Primary,
	eNCAPI_Dns_Secondary,
	eNCAPI_Dns_Thirdly,
	eNCAPI_Dns_Max,
} eNcapi_DnsAddress_t;

typedef enum
{
	eNCAPI_IPV6_ADDR_GLOBAL	= 0x0000U,
	eNCAPI_IPV6_ADDR_LOOPBACK	= 0x0010U,
	eNCAPI_IPV6_ADDR_LINKLOCAL	= 0x0020U,
	eNCAPI_IPV6_ADDR_SITELOCAL	= 0x0040U,
	eNCAPI_IPV6_ADDR_COMPATv4	= 0x0080U
} eNcapi_IPV6_ADDR_SCOPE_e;

typedef struct
{
	HUINT8	macaddr[NCAPI_MAX_MACADDR];
	HUINT8	ipAddr[NCAPI_MAX_IP4ADDR];
	HUINT8	netmask[NCAPI_MAX_IP4ADDR];
	HUINT8	gateway[NCAPI_MAX_IP4ADDR];
	HUINT8	dns[eNCAPI_Dns_Max][NCAPI_MAX_IP4ADDR];
	HUINT8	broadcast[NCAPI_MAX_IP4ADDR];
} __attribute__((packed)) NCAPI_ipv4_t;

typedef struct
{
	HUINT8	macaddr[NCAPI_MAX_MACADDR];
	HUINT8	ipAddr[NCAPI_MAX_IP6ADDR];
	HUINT8	gateway[NCAPI_MAX_IP6ADDR];
	HUINT8	dns[eNCAPI_Dns_Max][NCAPI_MAX_IP6ADDR];
	HUINT8	broadcast[NCAPI_MAX_IP6ADDR];
	HUINT32 prefix;
	eNcapi_IPV6_ADDR_SCOPE_e addrscope;
} __attribute__((packed)) NCAPI_ipv6_t;

typedef struct {
	NCAPI_ipv4_t ipv4;
	NCAPI_ipv6_t ipv6;
} __attribute__((packed)) NCAPI_NetConfIpInfo_t;

typedef struct {
	HINT32		id;
	HINT32		priority;
	HCHAR		name[NCAPI_MAX_NET_INTERFACE_NAME];
	eNcapiNetConfDevType	type;
	HBOOL		linked;
	HBOOL		active;
	HBOOL		dhcp;
	HBOOL		dns_auto;
	HBOOL		auto_ip;
	eNcapiNetConfIpVersion	ip_ver;
	NCAPI_NetConfIpInfo_t	ip_info;
	HCHAR		fw_ver[NCAPI_FW_VERSION_MAX_LEN];
} __attribute__((packed)) NCAPI_NetConfInfo_t;

typedef void (*NCAPI_NetConfCallback) (eNcapiNetConfEvent event, HINT32 index,
										NCAPI_NetConfInfo_t *data);


/*-----------------------------------------------------------------------------------------------
 *	interfaces
 *----------------------------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif

/*-----------------------------------------------------------------------------------------------
 * Network Configuration
 *----------------------------------------------------------------------------------------------*/
HERROR NCAPI_NetConf_Init(void);
HERROR NCAPI_NetConf_Deinit(void);

HERROR NCAPI_NetConf_RegisterCallback(NCAPI_NetConfCallback callback);

HERROR NCAPI_NetConf_UnregisterCallback(NCAPI_NetConfCallback callback);

HERROR NCAPI_NetConf_GetDeviceCount(HINT32 *device_count);

HERROR NCAPI_NetConf_GetDeviceInfo(HINT32 index, NCAPI_NetConfInfo_t *info);

HERROR NCAPI_NetConf_GetDefaultDevIndex(HINT32 *device_id );


HERROR NCAPI_NetConf_SetInfo(HINT32 index, const NCAPI_NetConfInfo_t *info);

HERROR NCAPI_NetConf_RestartDHCPClient(HINT32 id);
HERROR NCAPI_NetConf_StopConnect(HINT32 id);
HERROR NCAPI_NetConf_DevUp(HINT32 id);
HERROR NCAPI_NetConf_DevDown(HINT32 id);
/*-----------------------------------------------------------------------------------------------
 * DHCP Server
 *----------------------------------------------------------------------------------------------*/
/**
 * Start DHCP Server
 *
 * @param	void
 */
HERROR NCAPI_NetConf_StartDHCPServer(HINT32 id, eNcapiNetConfIpVersion eIpVer);

/**
 * Stop DHCP Server
 *
 * @param	void
 */
HERROR NCAPI_NetConf_StopDHCPServer(HINT32 id, eNcapiNetConfIpVersion eIpVer);

HERROR NCAPI_NetConf_SetWOL(HBOOL is_enable);

HERROR NCAPI_NetConf_SetDevPriority(HINT32 id, HINT32 priority);

HERROR NCAPI_Netconf_SetNTPClient(HCHAR *addr);

HERROR NCAPI_Netconf_Renew(HCHAR *addr);

/**
* This function is used to check the connection from STB to the Internet
* The Internet endpoint which is used is HUMAX Portal, or www.youtube.com if any
* emit event EVENT_NET_INTERNET_CONNECTED if the STB can connect to the Internet in lTimeout miliseconds
* emit event EVENT_NET_INTERNET_DISCONNECTED if the STB cannot connect to the Internet in lTimeout miliseconds
* @param[in] lTimeout: timeout in Miliseconds
* @return ERR_OK if this function is called succesfully, ERR_FAIL if vice versa
*/


#ifdef __cplusplus
}
#endif

#endif	/* __NCAPI_NETCONF_H__ */
