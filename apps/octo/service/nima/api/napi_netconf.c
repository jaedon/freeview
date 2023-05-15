/** **********************************************************************************************************
	File 		: napi_netconf.c
	author 		:
	comment		: Network Configuration
	date    	: 2013/05/27
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

#include "vkernel.h"

#include "napi_netconf.h"
#include "napi_cache.h"
#include "napi_agent_common.h"
#include "napi_protocol.h"
#include "napi_parser.h"
#include "napi_debug.h"

#include <hlib.h>
#include <pthread.h>

/*-----------------------------------------------------------------------------------------------
 *	pre-definition
 *----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------
 *	internal functions
 *----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------
 *	interfaces
 *----------------------------------------------------------------------------------------------*/

/**
 * @brief make lock
 */
static HERROR napi_make_lock(HULONG * lock)
{
	if (VK_SEM_Create(lock, "napi_lock_sema", VK_SUSPENDTYPE_FIFO) != VK_OK)
	{
		HxLOG_Critical("VK_SEM_Create() failed.\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

/**
 * @brief destroy lock
 */
static void napi_destroy_lock(HULONG * lock)
{
	VK_SEM_Destroy(*lock);
}

/**
 * @brief lock
 */
static void napi_lock(HULONG lock)
{
	VK_SEM_Get(lock);
}

/**
 * @brief unlock
 */
static void napi_unlock(HULONG lock)
{
	VK_SEM_Release(lock);
}

HERROR NAPI_NetConf_RegisterCallback(NAPI_NetConfCallback callback)
{
	HERROR	eRet = ERR_OK;
	FuncEnter;

	eRet = napi_agent_register_callback(napi_agent_get_instance(), callback, eAGENT_EVENT_TYPE_NETCONF);

	FuncLeave;
	return eRet;
}

HINT32 NAPI_NetConf_GetCount()
{
	NAPIDevInfo_t *dev_cache = NULL;
	HINT32 dev_count = 0;
	napicache_t *cache = NULL;

	FuncEnter;

	// for fast boot
	napi_agent_init_wait();

	cache = napi_cache_get_instance();
	if ( cache == NULL ) return 0;

	dev_cache = napi_cache_devinfo_get(cache);
	if( !dev_cache ) return 0;

	dev_count = dev_cache->dev_count;

	HxLOG_Debug("  (#)device count : %d \n", dev_count);

	FuncLeave;

	return dev_count;
}

HERROR NAPI_NetConf_IsNetworkAvailable(HBOOL *isEnable)
{
	HERROR hRet = ERR_OK;
	NAPIDevInfo_t *devinfo = NULL;
	HINT32 default_dev = 0;
	napicache_t *cache = NULL;
	FuncEnter;

	// for fast boot
	napi_agent_init_wait();

	cache = napi_cache_get_instance();
	if ( !cache )
	{
		hRet = ERR_FAIL;
	}
	else
	{
		devinfo = napi_cache_devinfo_get(cache);
		if( !devinfo )
		{
			hRet = ERR_FAIL;
		}
		else
		{
			default_dev = devinfo->default_dev;
		}
	}
	*isEnable = default_dev < 0 ? FALSE : TRUE;

	FuncLeave;
	return hRet;
}

/**
* This function is used to get the index of default network interface, which is used to connect to the external network
* Default network interface must have an IP connection already
* @return index of default network interface if this function is called succesfully, 0 if vice versa
*/

HINT32 NAPI_NetConf_GetDefaultIndex(void)
{
	NAPIDevInfo_t *devinfo = NULL;
	HINT32 default_dev = 0;
	napicache_t *cache = NULL;
	FuncEnter;

	// for fast boot
	napi_agent_init_wait();

	cache = napi_cache_get_instance();
	if ( !cache ) return 0;

	devinfo = napi_cache_devinfo_get(cache);
	if( !devinfo ) return 0;
	default_dev = devinfo->default_dev;

	HxLOG_Debug("  (#)default device : %d \n", default_dev);

	FuncLeave;

	return default_dev;
}

HERROR NAPI_NetConf_GetInfo(HINT32 index, NAPINetConfInfo_t *info)
{
	HERROR	eRet = ERR_OK;
	NAPINetConfInfo_t *netinfo = NULL;
	napicache_t *cache = NULL;
	FuncEnter;

	// for fast boot
	napi_agent_init_wait();

	if(!info) return ERR_FAIL;

	cache = napi_cache_get_instance();
	if ( !cache ) return ERR_FAIL;

	netinfo = napi_cache_netconf_get(cache, index);
	if(!netinfo) return ERR_FAIL;

	//debug log
	napi_debug_log_netconf(index, netinfo);

	HxSTD_MemCopy(info, netinfo, sizeof(NAPINetConfInfo_t));

	FuncLeave;
	return eRet;
}

HERROR NAPI_NetConf_SetInfo(HINT32 index, const NAPINetConfInfo_t *info)
{
	HERROR	eRet = ERR_OK;
	napipacket_t *packet = NULL;
	NAPIProtocol_Netconf_t *netconf = NULL;
	NAPIDevInfo_t *dev_info = NULL;
	NAPINetConfInfo_t *net_info = NULL;
	napiagent_t *agent = napi_agent_get_instance();

	FuncEnter;

	if(!info)
	{
		napi_agent_notify_callback(agent, eAGENT_EVENT_TYPE_NETCONF, NAPI_EVENT_NETCONF_CONN_FAIL, NULL);
		return ERR_FAIL;
	}

	net_info = napi_cache_netconf_get(napi_cache_get_instance(), index);
	if( net_info )
	{
		if( !net_info->linked )
		{
			napi_agent_notify_callback(agent, eAGENT_EVENT_TYPE_NETCONF, NAPI_EVENT_NETCONF_CONN_FAIL, NULL);
			return ERR_FAIL;
		}
	}

	dev_info = napi_cache_devinfo_get(napi_cache_get_instance());
	if( !dev_info )
	{
			napi_agent_notify_callback(agent, eAGENT_EVENT_TYPE_NETCONF, NAPI_EVENT_NETCONF_CONN_FAIL, NULL);
			return ERR_FAIL;
	}

	netconf = HLIB_STD_MemAlloc(sizeof(NAPIProtocol_Netconf_t));
	if( NULL != netconf )
	{
		HxSTD_MemSet(netconf, 0x0, sizeof(*netconf));
		netconf->id = info->id;
		HxSTD_MemCopy(netconf->name, info->name, NAPI_PROTO_MAX_INTERFACE_NAME);
		netconf->type = napi_parser_devtype_NapitoProtocol(info->type);
		netconf->linked = info->linked;
		netconf->active = info->active;
		netconf->dhcp = info->dhcp;
		netconf->dns_auto = info->dns_auto;
		netconf->auto_ip = info->auto_ip;
		netconf->ip_ver = info->ip_ver;

		HxLOG_Debug("id : %d (info->id: %d)\n", netconf->id, info->id);

		if( info->ip_ver == eNAPI_NETCONF_IPV4 )
		{
			napi_parser_macAtoN(info->ip_info.ipv4.macaddr, netconf->ip_info.ipv4.macaddr);
			napi_parser_addrAtoN(info->ip_info.ipv4.ipAddr, netconf->ip_info.ipv4.ipAddr);
			napi_parser_addrAtoN(info->ip_info.ipv4.netmask, netconf->ip_info.ipv4.netmask);
			napi_parser_addrAtoN(info->ip_info.ipv4.gateway, netconf->ip_info.ipv4.gateway);
			napi_parser_addrAtoN(info->ip_info.ipv4.dns1, netconf->ip_info.ipv4.dns1);
			napi_parser_addrAtoN(info->ip_info.ipv4.dns2, netconf->ip_info.ipv4.dns2);
		}
		else if( info->ip_ver == eNAPI_NETCONF_IPV6 )
		{
			napi_parser_macAtoN(info->ip_info.ipv6.macaddr, netconf->ip_info.ipv6.macaddr);
			napi_parser_addrAtoN(info->ip_info.ipv6.ipAddr, netconf->ip_info.ipv6.ipAddr);
			napi_parser_addrAtoN(info->ip_info.ipv6.gateway, netconf->ip_info.ipv6.gateway);
			napi_parser_addrAtoN(info->ip_info.ipv6.dns1, netconf->ip_info.ipv6.dns1);
			napi_parser_addrAtoN(info->ip_info.ipv6.dns2, netconf->ip_info.ipv6.dns2);
		}

		packet = HLIB_STD_MemAlloc(sizeof(napipacket_t) + sizeof(NAPIProtocol_Netconf_t));
		if ( NULL != packet )
		{
			HxSTD_MemSet(packet, 0x0, sizeof(napipacket_t) + sizeof(NAPIProtocol_Netconf_t));
			packet->type = NAPI_PROTOTYPE_REQUEST;
			packet->code = NAPI_PROTO_SET_NET_INFO;
			packet->dlen = sizeof(NAPIProtocol_Netconf_t);

			HxSTD_MemCopy(packet->data, netconf, packet->dlen);
			eRet = napi_agent_write_data(napi_agent_get_instance(), (HCHAR *)packet, sizeof(napipacket_t)+packet->dlen);
		}
	}

	if( NULL != packet )
	{
		HLIB_STD_MemFree(packet);
	}
	if( NULL != netconf )
	{
		HLIB_STD_MemFree(netconf);
	}

	FuncLeave;
	return eRet;
}

HERROR NAPI_NetConf_StopConnect(HINT32 index)
{
	HERROR	eRet = ERR_OK;
	napipacket_t *packet = NULL;
	HINT32 dev_id = index;

	FuncEnter;

	packet = HLIB_STD_MemAlloc(sizeof(napipacket_t) + sizeof(HINT32));

	if ( NULL != packet )
	{
		HxSTD_MemSet(packet, 0x0, sizeof(napipacket_t) + sizeof(HINT32));
		packet->type = NAPI_PROTOTYPE_REQUEST;
		packet->code = NAPI_PROTO_DISCONNECT;
		packet->dlen = sizeof(HINT32);

		HxSTD_MemCopy(packet->data, &dev_id, packet->dlen);

		eRet = napi_agent_write_data(napi_agent_get_instance(), (HCHAR *)packet, sizeof(napipacket_t) + packet->dlen);
	}

	if( NULL != packet ) HLIB_STD_MemFree(packet);

	FuncLeave;

	return eRet;
}


HERROR NAPI_NetConf_StartDHCPServer(void)
{
	HERROR	eRet = ERR_OK;
	napipacket_t packet;

	FuncEnter;

	packet.type = NAPI_PROTOTYPE_REQUEST;
	packet.code = NAPI_PROTO_DHCPSERVER_START;
	packet.dlen = 0;

	eRet = napi_agent_write_data(napi_agent_get_instance(), (HCHAR *)&packet, sizeof(packet));

	FuncLeave;

	return eRet;
}

HERROR NAPI_NetConf_StopDHCPServer(void)
{
	HERROR	eRet = ERR_OK;
	napipacket_t packet;

	FuncEnter;

	packet.type = NAPI_PROTOTYPE_REQUEST;
	packet.code = NAPI_PROTO_DHCPSERVER_STOP;
	packet.dlen = 0;

	eRet = napi_agent_write_data(napi_agent_get_instance(), (HCHAR *)&packet, sizeof(packet));

	FuncLeave;

	return eRet;
}

HERROR NAPI_NetConf_GetDHCPServerInfo(NAPIDHCPServerInfo_t *info)
{
	HERROR	eRet = ERR_OK;
	NAPIDHCPServerInfo_t *dhcpserver_info = NULL;
	napicache_t *cache = NULL;

	FuncEnter;

	cache = napi_cache_get_instance();
	if ( cache == NULL ) return ERR_FAIL;

	dhcpserver_info = (NAPIDHCPServerInfo_t *)napi_cache_dhcpserver_get(cache, 0);
	if( !dhcpserver_info ) return ERR_FAIL;

	HxSTD_MemCopy(info, dhcpserver_info, sizeof(NAPIDHCPServerInfo_t));

	FuncLeave;

	return eRet;
}

HERROR NAPI_NetConf_DevUp(HINT32 index)
{
	HERROR	eRet = ERR_OK;
	napipacket_t *packet = NULL;
	HINT32 dev_id = index;

	FuncEnter;

	packet = HLIB_STD_MemAlloc(sizeof(napipacket_t) + sizeof(HINT32));
	if ( !packet ) return ERR_FAIL;
	HxSTD_MemSet(packet, 0x0, sizeof(napipacket_t) + sizeof(HINT32));
	packet->type = NAPI_PROTOTYPE_REQUEST;
	packet->code = NAPI_PROTO_DEV_UP;
	packet->dlen = sizeof(HINT32);

	HxSTD_MemCopy(packet->data, &dev_id, packet->dlen);

	eRet = napi_agent_write_data(napi_agent_get_instance(), (HCHAR *)packet, sizeof(napipacket_t) + packet->dlen);


	HLIB_STD_MemFree(packet);
	FuncLeave;

	return eRet;
}

HERROR NAPI_NetConf_DevDown(HINT32 index)
{
	HERROR	eRet = ERR_OK;
	napipacket_t *packet = NULL;
	HINT32 dev_id = index;

	FuncEnter;

	packet = HLIB_STD_MemAlloc(sizeof(napipacket_t) + sizeof(HINT32));
	if ( !packet ) return ERR_FAIL;
	HxSTD_MemSet(packet, 0x0, sizeof(napipacket_t) + sizeof(HINT32));
	packet->type = NAPI_PROTOTYPE_REQUEST;
	packet->code = NAPI_PROTO_DEV_DOWN;
	packet->dlen = sizeof(HINT32);

	HxSTD_MemCopy(packet->data, &dev_id, packet->dlen);

	eRet = napi_agent_write_data(napi_agent_get_instance(), (HCHAR *)packet, sizeof(napipacket_t) + packet->dlen);


	HLIB_STD_MemFree(packet);
	FuncLeave;

	return eRet;
}

HERROR NAPI_NetConf_SetWOL(HBOOL is_enable)
{
	HERROR eRet = ERR_OK;
	//1 #### Please Insert Code


	return eRet;
}

HERROR NAPI_NetConf_SetDevPriority(HINT32 index, HINT32 prioriry)
{
	HERROR	eRet = ERR_OK;
	napipacket_t *packet = NULL;
	NAPIProtocol_priority_t info;
	HINT32 dlen = 0;

	FuncEnter;

	dlen = sizeof(NAPIProtocol_priority_t);
	packet = HLIB_STD_MemAlloc(sizeof(napipacket_t) + dlen);
	if ( !packet ) return ERR_FAIL;
	HxSTD_MemSet(packet, 0x0, sizeof(napipacket_t) + dlen);
	packet->type = NAPI_PROTOTYPE_REQUEST;
	packet->code = NAPI_PROTO_SET_DEV_PRIORITY;
	packet->dlen = dlen;

	info.id = index;
	info.priority = prioriry;

	HxSTD_MemCopy(packet->data, &info, packet->dlen);

	eRet = napi_agent_write_data(napi_agent_get_instance(), (HCHAR *)packet, sizeof(napipacket_t) + packet->dlen);

	HLIB_STD_MemFree(packet);
	FuncLeave;

	return eRet;
}

HERROR NAPI_NetConf_NTP_Client(HCHAR *addr)
{
	HERROR	eRet = ERR_OK;
	napipacket_t *packet = NULL;
	NAPIProtocol_cmd_ntp_t* cmd_info = NULL;
	HINT32 dLen = 0, packet_len = 0;

	FuncEnter;

	if(!addr) return ERR_FAIL;

	packet_len = sizeof(napipacket_t);
	dLen = sizeof(NAPIProtocol_cmd_ntp_t);
	cmd_info = HLIB_STD_MemAlloc(dLen);
	HxSTD_MemSet(cmd_info, 0x0, dLen);

	HxSTD_MemCopy(cmd_info->ipAddr, addr, NAPI_PROTO_MAX_ADDR);

	packet = HLIB_STD_MemAlloc(packet_len + dLen);
	if ( !packet )
	{
		if( cmd_info ) HLIB_STD_MemFree(cmd_info);
		return ERR_FAIL;
	}
	HxSTD_MemSet(packet, 0x0, packet_len + dLen);
	packet->type = NAPI_PROTOTYPE_REQUEST;
	packet->code = NAPI_PROTO_SET_NTP_CLIENT;
	packet->dlen = dLen;

	HxSTD_MemCopy(packet->data, cmd_info, packet->dlen);
	eRet = napi_agent_write_data(napi_agent_get_instance(), (HCHAR *)packet, packet_len + packet->dlen);

	if( packet ) HLIB_STD_MemFree(packet);
	if( cmd_info ) HLIB_STD_MemFree(cmd_info);

	FuncLeave;
	return eRet;
}

HERROR NAPI_NetConf_GetStatus(HINT32 *status)
{
	HERROR eRet = ERR_OK;
	HINT32 i = 0;
	NAPIDevInfo_t *dev_cache = NULL;
	NAPINetConfInfo_t *net_info = NULL;
	HINT32 net_status = NAPI_PROTO_NETCONF_STATUS_ShutdownDone;
	napicache_t *cache = NULL;

	FuncEnter;

	cache = napi_cache_get_instance();
	if( cache == NULL ) return ERR_FAIL;

	dev_cache = napi_cache_devinfo_get(cache);
	if( !dev_cache ) return ERR_FAIL;

	for( i=0; i<dev_cache->dev_count; i++ )
	{
		net_info = napi_cache_netconf_get(cache, i);
		if(!net_info) return ERR_FAIL;
		if( net_info->active == TRUE )
		{
			net_status = NAPI_PROTO_NETCONF_STATUS_Operation;
		}

	}

	*status = net_status;

	HxLOG_Debug("  (#)net status : %d \n", net_status);

	FuncLeave;

	return eRet;
}

/**
 * @brief
 */
static int napi_connect_with_timeout(int sock, struct addrinfo * res, HUINT32 timeout_milli)
{
	int err;
	socklen_t len;
	fd_set writefds;
	struct timeval timeout;
	int flags;
	FD_ZERO(&writefds);
	flags = fcntl(sock, F_GETFL);
	flags = (flags | O_NONBLOCK);
	if (fcntl(sock, F_SETFL, flags) != 0)
	{
		HxLOG_Error("fcntl() error\n");
		return -1;
	}
	if (connect(sock, res->ai_addr, res->ai_addrlen) != 0)
	{
		if (errno != EINPROGRESS)
		{
			HxLOG_Error("connect() error\n");
			return -1;
		}
	}
	timeout.tv_sec = timeout_milli / 1000;
	timeout.tv_usec = (timeout_milli % 1000) * 1000;
	FD_SET(sock, &writefds);
	if (select(sock+1, NULL, &writefds, NULL, &timeout) <= 0)
	{
		HxLOG_Error("connection timeout\n");
		return -1;
	}
	len = sizeof(err);
	getsockopt(sock, SOL_SOCKET, SO_ERROR, (char *)&err, &len);
	if (err)
	{
		HxLOG_Error("fcntl() error - %d\n", err);
		return -1;
	}
    fcntl(sock, F_GETFL);
	flags = (flags & ~O_NONBLOCK);
	if (fcntl(sock, F_SETFL, flags) != 0)
	{
		HxLOG_Error("fcntl() error\n");
		return -1;
	}
	return 0;
}

/**
 * @brief checking internet connectivity
 */
static HBOOL napi_check_internet(const HCHAR * url)
{
	HBOOL ret = FALSE;
	HINT16 sSocketfd = -1, sResult = -1;
	struct addrinfo hints, * res = NULL;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	sResult = getaddrinfo(url, "http", &hints, &res);
	if (sResult < 0)
	{
		HxLOG_Error("getaddrinfo() error %s\n", gai_strerror(sResult));
		goto quit;
	}

	HxLOG_Debug("Resolve domain name successfully!\n");
	sSocketfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (sSocketfd == -1)
	{
		HxLOG_Error("Cannot create socket to Internet endpoint, errno = %d: %s!\n", errno, strerror(errno));
		goto quit;
	}

	if (napi_connect_with_timeout(sSocketfd, res, 30000) < 0)
	{
		HxLOG_Error("Cannot connect, errno = %d: %s!\n", errno, strerror(errno));
		goto quit;
	}
	
	ret = TRUE;

quit:

	if (res)
	{
		freeaddrinfo(res);
	}
	if(sSocketfd != -1)
	{
		close(sSocketfd);
	}

	return ret;
}

/**
* This function is used by napi_netconf_check_internet_connection_task function
* Set param->bIsInternetConnected = TRUE if STB can connect to the Internet
* @param[in] vparam: A pointer to st_netconf_check_internet_status_info struct variable
*/

static void * napi_netconf_check_internet_connection_subtask (void * vparam)
{
#if !defined(CONFIG_PROD_OS_EMULATOR)
	st_netconf_check_internet_status_info_with_url * param = (st_netconf_check_internet_status_info_with_url *)vparam;
	st_netconf_check_internet_status_info * shared_info = param->info;
	napiagent_t * agent = napi_agent_get_instance();
	int cnt;

	HxLOG_Warning("Internet available test to \"%s\"\n", param->url);

	if (napi_check_internet(param->url) == TRUE)
	{
		shared_info->bIsInternetConnected = TRUE;
	}

	napi_lock(shared_info->lock);
	shared_info->iCount--;
	cnt = shared_info->iCount;
	napi_unlock(shared_info->lock);
	
	if (shared_info->bDone == TRUE)
	{
		return NULL;
	}
	
	if (shared_info->bIsInternetConnected)
	{
		HxLOG_Warning("Internet : Available\n");
		napi_agent_notify_callback(agent, eAGENT_EVENT_TYPE_NETCONF, NAPI_EVENT_NETCONF_INTERNET_CONNECTED, NULL);
		shared_info->bDone = TRUE;
	}
	else if (cnt == 0)
	{
		HxLOG_Warning("Internet : N/A\n");
		napi_agent_notify_callback(agent, eAGENT_EVENT_TYPE_NETCONF, NAPI_EVENT_NETCONF_INTERNET_DISCONNECTED, NULL);
		shared_info->bDone = TRUE;
	}

#endif

	return NULL;
}

/**
 * @brief get url by index
 */
static HCHAR * napi_get_url_by_index(HCHAR * buffer, size_t size, int idx)
{
	switch (idx) {
	case 0:
	{
		HxSTD_snprintf(buffer, size, "%s", INTERNET_TEST_SITE);
		break;
	}
	case 1:
	{
#if defined(SECOND_INTERNET_TEST_SITE)
		HxSTD_snprintf(buffer, size, "%s", SECOND_INTERNET_TEST_SITE);
		break;
#else
		return NULL;
#endif
	}
	default:
	{
		return NULL;
	}
	}

	return buffer;
}

/**
 * @brief make internet status info
 */
static st_netconf_check_internet_status_info * napi_make_internet_status_info(int url_count)
{
	st_netconf_check_internet_status_info * info;
	info = (st_netconf_check_internet_status_info*)HLIB_STD_MemAlloc(sizeof(st_netconf_check_internet_status_info));
	if (info == NULL)
	{
		HxLOG_Critical("HLIB_STD_MemAlloc failed.\n");
		return NULL;
	}
	HxSTD_MemSet(info, 0, sizeof(st_netconf_check_internet_status_info));
	if (napi_make_lock(&info->lock) != ERR_OK)
	{
		HLIB_STD_MemFree(info);
		return NULL;
	}
	info->iCount = url_count;
	
	return info;
}

/**
 * @brief destroy internet status info
 */
static void napi_destroy_internet_status_info(st_netconf_check_internet_status_info * info)
{
	napi_destroy_lock(&info->lock);
	HLIB_STD_MemFree(info);
}

/**
 * @brief make internet status info with url
 */
static st_netconf_check_internet_status_info_with_url * napi_make_internet_status_info_with_url(int url_idx, st_netconf_check_internet_status_info * info)
{
	st_netconf_check_internet_status_info_with_url * info_with_url;
	info_with_url = (st_netconf_check_internet_status_info_with_url*)HLIB_STD_MemAlloc(sizeof(st_netconf_check_internet_status_info_with_url));
	if (info_with_url == NULL)
	{
		HxLOG_Critical("HLIB_STD_MemAlloc failed.\n");
		return NULL;
	}
	HxSTD_MemSet(info_with_url, 0, sizeof(st_netconf_check_internet_status_info_with_url));
	if (napi_get_url_by_index(info_with_url->url, sizeof(info_with_url->url), url_idx) == NULL)
	{
		HxLOG_Error("Wrong operation - no url found\n");
		HLIB_STD_MemFree(info_with_url);
		return NULL;
	}
	info_with_url->info = info;
	return info_with_url;
}

/**
 * @brief run thread
 * @param t pthread_t
 * @param thread_func thread function pointer
 * @param param pthread parameter
 */
static HBOOL napi_run_thread(pthread_t * t, void * (*thread_func)(void *), void * param)
{
	if (pthread_create(t, NULL, thread_func, param) != 0)
	{
		HxLOG_Error("pthread_create() failed\n");
		return FALSE;
	}

	return TRUE;
}

/**
 * @brief run thread (detached)
 * @param t pthread_t
 * @param thread_func thread function pointer
 * @param param pthread parameter
 */
static HBOOL napi_run_thread_detached(pthread_t * t, void * (*thread_func)(void *), void * param)
{
	pthread_attr_t attr;
	if (pthread_attr_init(&attr) != 0)
	{
		HxLOG_Error("pthread_attr_init() failed\n");
		return FALSE;
	}
	if (pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED) != 0)
	{
		HxLOG_Error("pthread_attr_setdetachstate() failed\n");
		pthread_attr_destroy(&attr);
		return FALSE;
	}
	if (pthread_create(t, &attr, thread_func, param) != 0)
	{
		HxLOG_Error("pthread_create() failed\n");
		pthread_attr_destroy(&attr);
		return FALSE;
	}
	pthread_attr_destroy(&attr);

	return TRUE;
}

/**
* This function is used to check the connection from STB to the Internet
* The Internet endpoint which is used is INTERNET_TEST_SITE
* emit event NAPI_EVENT_NETCONF_INTERNET_CONNECTED if the STB can connect to the Internet in Ltimeout miliseconds
* emit event NAPI_EVENT_NETCONF_INTERNET_DISCONNECTED if the STB cannot connect to the Internet in Ltimeout miliseconds
* @param[in] plTimeout: A pointer to timeout value in Miliseconds
*/

static void * napi_netconf_check_internet_connection_task (void * plTimeout)
{

#if !defined(SECOND_INTERNET_TEST_SITE)
	#define URL_COUNT 1
#else
	#define URL_COUNT 2
#endif
	int i;
	HBOOL success = FALSE;
	st_netconf_check_internet_status_info * info = NULL;
	st_netconf_check_internet_status_info_with_url * vParam[URL_COUNT] = {0,};
	pthread_t stThread[URL_COUNT] = {0,};
	napiagent_t * agent = napi_agent_get_instance();
	HINT32 lTimeout = *(HINT32 *)plTimeout;
	HUINT32 wait_tick;
	HLIB_STD_MemFree(plTimeout);

	HxLOG_Debug(" ++ %s\n", __FUNCTION__);

	// minimum 1 second required
	if (lTimeout < 1000) 
	{
		HxLOG_Error("Cannot connect to the Internet in %d miliseconds! Timeout value is too small.\n", lTimeout);
		goto quit;
	}
	
	info = napi_make_internet_status_info(URL_COUNT);
	if (info == NULL)
	{
		goto quit;
	}
	
	for (i = 0; i < URL_COUNT; i++)
	{
		vParam[i] = napi_make_internet_status_info_with_url(i, info);
		if (vParam[i] == NULL)
		{
			goto quit;
		}
		if (napi_run_thread(&stThread[i], &napi_netconf_check_internet_connection_subtask, vParam[i]) == FALSE)
		{
			HLIB_STD_MemFree(vParam[i]);
			vParam[i] = NULL;
			goto quit;
		}
	}

	// 1000 ms is for processing
	wait_tick = HLIB_STD_GetSystemTick();
	while ((info->bDone == FALSE) && (HLIB_STD_GetSystemTick() - wait_tick) < (HUINT32)(lTimeout - 1000))
	{
		HLIB_STD_TaskSleep(10);
	}

	if (info->bDone == FALSE)
	{
		HxLOG_Error("Cannot connect to the Internet in %d miliseconds!\n", lTimeout);
		info->bDone = TRUE;
		goto quit;
	}

	success = TRUE;

quit:

	if (success == FALSE) {
		napi_agent_notify_callback(agent, eAGENT_EVENT_TYPE_NETCONF, NAPI_EVENT_NETCONF_INTERNET_DISCONNECTED, NULL);
	}
	for (i = 0; i < URL_COUNT; i++)
	{
		if (vParam[i])
		{
			pthread_join(stThread[i], NULL);
			HLIB_STD_MemFree(vParam[i]);
		}
	}
	if (info)
	{
		napi_destroy_internet_status_info(info);
	}

	HxLOG_Debug(" -- %s\n", __FUNCTION__);

	return NULL;
}

/**
* This function is used to check the connection from STB to the Internet
* The Internet endpoint which is used is INTERNET_TEST_SITE
* emit event NAPI_EVENT_NETCONF_INTERNET_CONNECTED if the STB can connect to the Internet in Ltimeout miliseconds
* emit event NAPI_EVENT_NETCONF_INTERNET_DISCONNECTED if the STB cannot connect to the Internet in Ltimeout miliseconds
* @param[in] lTimeout: time in Miliseconds
* @return ERR_OK if this function is called succesfully, ERR_FAIL if vice versa
*/


HERROR NAPI_NetConf_CheckInternetConnection(HINT32 lTimeout)
{
	pthread_t stThread;
	HINT32 * plTimeout = NULL;
	HERROR	eRet = ERR_OK;

	FuncEnter;

	plTimeout = HLIB_STD_MemAlloc(sizeof(HINT32));
	if (plTimeout == NULL)
	{
		HxLOG_Critical("HLIB_STD_MemAlloc failed.\n");
		return ERR_FAIL;
	}
	*plTimeout = lTimeout;
	if (napi_run_thread_detached(&stThread, &napi_netconf_check_internet_connection_task, plTimeout) == FALSE)
	{
		HxLOG_Error("napi_run_thread() failed\n");
		HLIB_STD_MemFree(plTimeout);
		eRet = ERR_FAIL;
	}

	FuncLeave;
	return eRet;
}
