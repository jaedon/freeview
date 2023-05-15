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


#ifndef __NAPI_NETCONF_H__
#define __NAPI_NETCONF_H__

#include "htype.h"
#include "napi_def.h"
#if defined(WIN32)
#	include <windows.h>
#	include <winsock.h>
#else/* Not Windows */
#	include <sys/socket.h>
#	include <netinet/in.h>
#	include <arpa/inet.h>
#	include <netdb.h>
#	include <unistd.h>
#	include <fcntl.h>
#	include <vkernel.h>
#endif
/*-----------------------------------------------------------------------------------------------
 *	pre-definition
 *----------------------------------------------------------------------------------------------*/

typedef void (*NAPI_NetConfCallback) (HINT32 event, void* data);

typedef struct
{
	HULONG lock;
	int iCount;
	HBOOL bDone;
	HBOOL bIsInternetConnected;
} st_netconf_check_internet_status_info;

typedef struct
{
	HCHAR url[1024];
	st_netconf_check_internet_status_info * info;
} st_netconf_check_internet_status_info_with_url;

#if defined(CONFIG_PROD_FVP4000T)
	#define INTERNET_TEST_SITE "www.humaxtvportal.com"
	#define SECOND_INTERNET_TEST_SITE "az341951.vo.msecnd.net"
#elif defined(CONFIG_OP_TDC)
	#define INTERNET_TEST_SITE "api.yousee.tv"
#else
	#define INTERNET_TEST_SITE "www.humaxtvportal.com"
#endif

/*-----------------------------------------------------------------------------------------------
 *	interfaces
 *----------------------------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif

/*-----------------------------------------------------------------------------------------------
 * Network Configuration
 *----------------------------------------------------------------------------------------------*/

/**
 * Register callback function for network status.
 *
 * @param	void
 * @return ERR_OK on success, ERR_FAIL on error.
 */
HERROR NAPI_NetConf_RegisterCallback(NAPI_NetConfCallback callback);

/**
 * Get a network device count.
 *
 * @param void
 * @return network device count.
 */
HINT32 NAPI_NetConf_GetCount(void);

/**
 * Get a active device index.
 *
 * @param void
 * @return active device index.
 */
HINT32 NAPI_NetConf_GetDefaultIndex(void);

/**
 * Get a network infomation for device index.
 *
 * @param index network device ID of the device box.
 * @param info network infomation for device index.
 * @return ERR_OK on success, ERR_FAIL on error.
 */
HERROR NAPI_NetConf_GetInfo(HINT32 index, NAPINetConfInfo_t *info);

/**
 * Set a network infomation for device index.
 *
 * @param index network device index of the device box.
 * @return ERR_OK on success, ERR_FAIL on error.
 */
HERROR NAPI_NetConf_SetInfo(HINT32 index, const NAPINetConfInfo_t *info);

HERROR NAPI_NetConf_StopConnect(HINT32 index);

/*-----------------------------------------------------------------------------------------------
 * DHCP Server
 *----------------------------------------------------------------------------------------------*/

/**
 * Start DHCP Server
 *
 * @param	void
 * @return ERR_OK on success, ERR_FAIL on error.
 */
HERROR NAPI_NetConf_StartDHCPServer(void);

/**
 * Stop DHCP Server
 *
 * @param	void
 * @return ERR_OK on success, ERR_FAIL on error.
 */
HERROR NAPI_NetConf_StopDHCPServer(void);

/**
 * Get a DHCP Server Information
 *
 * @param info pointer of dhcp server inforamtion.
 * @return ERR_OK on success, ERR_FAIL on error.
 */
HERROR NAPI_NetConf_GetDHCPServerInfo(NAPIDHCPServerInfo_t *info);

HERROR NAPI_NetConf_DevUp(HINT32 id);
HERROR NAPI_NetConf_DevDown(HINT32 id);
HERROR NAPI_NetConf_SetWOL(HBOOL is_enable);
HERROR NAPI_NetConf_IsNetworkAvailable(HBOOL *isEnable);

HERROR NAPI_NetConf_NTP_Client(HCHAR *addr);

HERROR NAPI_NetConf_GetStatus(HINT32 *status);

/**
* This function is used to check the connection from STB to the Internet
* The Internet endpoint which is used is HUMAX Portal
* emit event NAPI_EVENT_NETCONF_INTERNET_CONNECTED if the STB can connect to the Internet in Ltimeout miliseconds
* emit event NAPI_EVENT_NETCONF_INTERNET_DISCONNECTED if the STB cannot connect to the Internet in Ltimeout miliseconds
* @param[in] lTimeout: time in Miliseconds
* @return ERR_OK if this function is called succesfully, ERR_FAIL if vice versa
*/

HERROR NAPI_NetConf_CheckInternetConnection(HINT32 lTimeout);



#ifdef __cplusplus
}
#endif

#endif	/* __NAPI_NETCONF_H__ */
