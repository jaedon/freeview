/*******************************************************************************
* File name : di_network.c
* Author :
* description :
*
* Copyright (c)  by Humax Co., Ltd. Since 2012.
* All right reserved
*******************************************************************************/

/*******************************************************************************
* Header and definitions
*******************************************************************************/
/* global header files */
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <resolv.h>

#include <linux/sockios.h>
#include <linux/ethtool.h>
#include <linux/if.h>
#include <linux/route.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <net/ethernet.h>
#include <arpa/inet.h>
#include <errno.h>

#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif
#include <ctype.h>

#include <fcntl.h>

/* chip manufacture's  header files */

/* humax header files */
#include "htype.h"
#include "taskdef.h"
#include "vkernel.h"
#include "nvram_map.h"
#include "di_err.h"
#include "di_uart.h"
#include "di_network.h"
#include "di_nvram.h"
#include "drv_err.h"
#include "drv_network.h"
#include "drv_micom.h"


/*******************************************************************************
* Debug level
*******************************************************************************/
//#define NETWORK_DEBUG
#ifdef NETWORK_DEBUG
#define PrintDebug(fmt, ...)	VK_DBG_Print (fmt, ## __VA_ARGS__)
#define PrintError(fmt, ...)	VK_DBG_Print (fmt, ## __VA_ARGS__)
#define PrintEnter()			VK_printf("(+)%s\n", __FUNCTION__)
#define PrintExit()				VK_printf("(-)%s\n", __FUNCTION__)
#else
#define PrintDebug(fmt, ...)
#define PrintError(fmt, ...)	VK_DBG_Print (fmt, ## __VA_ARGS__)
#define PrintEnter()
#define PrintExit()
#endif

/*******************************************************************************
* Definitions, typdef & const data
*******************************************************************************/
/* move to taskdef.h */
#define	NETWORK_TASK_PRIORITY		USER_PRIORITY4
#define NETWORK_TASK_STACK_SIZE		BASIC_TASK_STACK_SIZE

#define	PROC_NET_DEV_INFO		"/proc/net/dev"
#define	PROC_NET_WIRELESS_INFO	"/proc/net/wireless"
#define RESOLV_CONF 			"/etc/resolv.conf"

#define DHCP_FIFO 			"/tmp/dhcp_fifo"

#if defined(CONFIG_DHCPCD)
#define DHCPCD_CONFIG_DIR	"/tmp"
#define DHCPC_SCRIPT_PATH	DHCPCD_CONFIG_DIR"/dhcpcd.script"
#define DHCPC_SCRIPT		"#!/bin/sh \n\n\
		DHCP_FIFO=/tmp/dhcp_fifo\n\n\
		[ -z '$1' ] && echo 'Error: should be called from dhcpcd' && exit 1\n\n\
		echo -n $3/$2 > $DHCP_FIFO\n\n\
		exit 0"
#else
#define DHCPC_SCRIPT_PATH	"/tmp/udhcpc.script"
#define DHCPC_SCRIPT		"#!/bin/sh \n\n\
		# udhcpc script edited by Hyunuk <huyang@humaxdigital.com>\n\
		DEFAULT_SCRIPT=/usr/share/udhcpc/default.script\n\
		DHCP_FIFO=/tmp/dhcp_fifo\n\n\
		[ -z '$1' ] && echo 'Error: should be called from udhcpc' && exit 1\n\n\
		if [ -f $DEFAULT_SCRIPT ]; then\n\
		    $DEFAULT_SCRIPT $1\n\
		else\n\
		    echo '[udhcpc] Error? there is no ($DEFAULT_SCRIPT). is that OK?'\n\
		fi\n\n\
		echo -n $interface/$1 > $DHCP_FIFO\n\n\
		exit 0"

#endif

#if defined(CONFIG_JAPAN_SPEC)
#define	DHCPS_CONF_FILE			"/tmp/udhcpd.conf"
#else
#define	DHCPS_CONF_FILE			"/var/lib/humaxtv/udhcpd.conf"
#endif
#define	DHCP6C_CONF_FILE		"/etc/dhcp/dhclient-dhcpv6.conf"
#define	DHCP6S_CONF_FILE		"/tmp/dhcpd-dhcpv6.conf"
#define	DHCPC_PID_FILE			"/tmp/udhcpc.pid"
#define	DHCPS_PID_FILE			"/tmp/udhcpd.pid"
#define	DHCP6C_PID_FILE			"/tmp/dhclient.pid"
#define	DHCP6S_PID_FILE			"/tmp/dhcpd.pid"
#define DHCP6C_LEASES_FILE		"/tmp/dhclient6.leases"
#define DHCP6S_LEASES_FILE		"/tmp/dhcpd6.leases"
#define DHCP6S_DEFAULT_LIFE_TIME	604800
#define DHCP6S_DEFAULT_RENEWAL_TIME	3600
#define DHCP6S_DEFAULT_REBINDING_TIME	7200
#define DHCP6S_DEFAULT_INFOREFRESH_TIME	21600

#define	DHCP_VENDOR_SPECIFIC_FILE	"/tmp/vendor_specific"
#define DHCP_VENDOR_SPECIFIC_OPT_ACS_URL		0x01
#define DHCP_VENDOR_SPECIFIC_OPT_PROVISION_CODE		0x02
#define DHCP_VENDOR_SPECIFIC_OPT_CWMP_RETRY_MIN_WAIT	0x03
#define DHCP_VENDOR_SPECIFIC_OPT_CWMP_RETRY_INTER_MULTI	0x04

#define DHCPC_PAUSE_TIME 		10

#define DNS_STR_MAX			64

#define	RTF_UP				0x0001		/* route usable		  	*/
#define	RTF_GATEWAY			0x0002		/* destination is a gateway	*/

#define IPV6_ADDR_SCOPE_MASK    	0x00f0U

#define NETWORK_MESSAGE_QUEUE_SIZE	10

#define LINK_CHECK_POLLING_TIME		1000

#define LINK_CHECK_COUNT 		3


#define IS_VALID_MAC_ADDR(mac_addr) 	VK_memcmp(s_aucMacNull, mac_addr, MAC_ADDR_LEN)
#define IS_VALID_IPV4_ADDR(ipv4_addr) 	VK_memcmp(s_aucIpv4Null, ipv4_addr, IPV4_ADDR_LEN)
#define IS_VALID_IPV6_ADDR(ipv6_addr) 	VK_memcmp(s_aucIpv6Null, ipv6_addr, IPV4_ADDR_LEN)

#define UNREFENCED_PARAM(x) 		(x=x)

#define	GET_NET_DEV_SEM(DevId)								\
do {											\
	if(VK_SEM_Get(s_stNetInst[DevId].s_ulNetSemId) != VK_OK)			\
	{										\
		PrintError("[%s:%d] VK_SEM_Get Error!\n", __FUNCTION__, __LINE__);	\
		return DI_ERR_ERROR;							\
	}										\
} while(0)
#define	REL_NET_DEV_SEM(DevId) 								\
do {											\
	if(VK_SEM_Release(s_stNetInst[DevId].s_ulNetSemId) != VK_OK)			\
	{										\
		PrintError("[%s:%d] VK_SEM_Release Error!\n", __FUNCTION__, __LINE__);	\
		return DI_ERR_ERROR;							\
	}										\
} while(0)

#define	GET_NET_MOD_SEM									\
do {											\
	if(VK_SEM_Get(s_ulNetModSemId) != VK_OK)					\
	{										\
		PrintError("[%s:%d] VK_SEM_Get Error!\n", __FUNCTION__, __LINE__);	\
		return DI_ERR_ERROR;							\
	}										\
} while(0)
#define	REL_NET_MOD_SEM 								\
do {											\
	if(VK_SEM_Release(s_ulNetModSemId) != VK_OK)					\
	{										\
		PrintError("[%s:%d] VK_SEM_Release Error!\n", __FUNCTION__, __LINE__);	\
		return DI_ERR_ERROR;							\
	}										\
} while(0)

typedef enum
{
	P_NETWORK_ADDR_ADD = 0,
	P_NETWORK_ADDR_DEL
} P_NETWORK_ADDR_OP_e;

typedef enum
{
	P_NETWORK_BYTES_RECEIVE = 0,
	P_NETWORK_PACKETS_RECEIVE,
	P_NETWORK_BYTES_SEND,
	P_NETWORK_PACKETS_SEND,
	P_NETWORK_MAX
}P_NETWORK_STATUS_e;

typedef struct
{
	HUINT32 ulDevId;
	HUINT32 ulWaitSeconds;
#if defined(CONFIG_IPV6)
	DI_NETWORK_IPV6_DHCP_MODE_e eDhcpv6Mode;
#endif
} P_NETWORK_DHCP_PARAMS_t;

typedef struct
{
	DRV_NETWORK_DEV_INFO_t *pstDevInfo;
	HBOOL bConnected;
	HBOOL bDhcpcUserKilled;
	HINT32 s_sockfd;
#if defined(CONFIG_IPV6)
	HINT32 s_sockfd6;
#endif

	HUINT32 ulLinkCnt;
	HUINT32 s_ulNetSemId;
	HULONG ulDhcpClientTaskId;
	HULONG ulDhcpServerTaskId;
	HULONG ulDhcp6ClientTaskId;
	HULONG ulDhcp6ServerTaskId;
	HULONG ulAutoIpTaskId;

	DI_NETWORK_DHCPC_PARAMS_t stDhcpcParams;
	DI_NETWORK_DHCPS_PARAMS_t stDhcpsParams;

	pfnDI_NETWORK_EVENT_CALLBACK pfnEventCallback[DI_NETWORK_EVENT_MAX];
} P_NETWORK_INSTANCE_t;

struct in6_ifreq {
        struct in6_addr ifr6_addr;
        uint32_t        ifr6_prefixlen;
        int32_t         ifr6_ifindex;
};

const HUINT8 s_aucMacNull[MAC_ADDR_LEN] = {0, };
const HUINT8 s_aucIpv4Null[IPV4_ADDR_LEN] = {0, };
const HUINT8 s_aucIpv6Null[IPV6_ADDR_LEN] = {0, };

#define UNUSED(a) (void)(a)

/*******************************************************************************
* Global variables and structures
*******************************************************************************/


/*******************************************************************************
* External variables and functions
*******************************************************************************/
/* from di_config.c */
extern DRV_NETWORK_DEV_INFO_t s_stDevList[];
extern HUINT32 DRV_CFG_GetNetMaxDevice(void);
#if defined(CONFIG_WIFI) && !defined(CONFIG_SHARED_COMPILE)
extern DRV_Error DRV_WIFI_GetDongleStatus(HUINT32 ulDevId, HBOOL *pbConnect);
#endif
#if defined(CONFIG_PPPOE) && !defined(CONFIG_SHARED_COMPILE)
extern DRV_Error DRV_PPPOE_GetLinkStatus(HUINT32 ulDevId, HBOOL *pbConnect);
#endif

/*******************************************************************************
* Static variables and structures
*******************************************************************************/
static HUINT32 s_ulEventMsgQId;
static HUINT32 s_ulEventNotifyTaskId;
static HUINT32 s_ulLinkCheckTaskId;
static P_NETWORK_INSTANCE_t *s_stNetInst;
static HUINT32 s_ulNetDevNum;
static HUINT32 s_ulNetModSemId;
static HULONG s_ulCheckDhcpResultTaskId;

/*******************************************************************************
* Functions
*******************************************************************************/

/*******************************************************************************
* function : P_NETWORK_MakeDhcpcScript
* description :
* input :
* output :
* return :
* DRV_OK, DRV_ERR
*******************************************************************************/
DRV_Error P_NETWORK_MakeDhcpcScript(HUINT8 *pucScriptFilePath)
{
	int fd;

	if ( pucScriptFilePath == NULL )
	{
		return DRV_ERR;
	}

	fd = open(pucScriptFilePath, O_RDWR|O_CREAT , 0755);
	if ( fd < 0 )
	{
		PrintError("[%s:%d] dhcpc script creation failed! we can't get event from dhcpc! \n", __FUNCTION__, __LINE__);
		return DRV_ERR;
	}
	else
	{
		write(fd, DHCPC_SCRIPT, VK_strlen(DHCPC_SCRIPT));
		close(fd);
	}

	return DRV_OK;
}

/*******************************************************************************
* function : P_NETWORK_GetMacAddr
* description :
* input :
* output :
* return :
* ??Ÿ ?????ڷ? ?? ????
*******************************************************************************/
DRV_Error P_NETWORK_GetMacAddr(HUINT32 ulDevId, HUINT8 *pucMacAddr)
{
	struct ifreq ifreq;

	VK_MEM_Memset(&ifreq, 0, sizeof(struct ifreq));

	/* get mac addr */
	VK_strncpy(ifreq.ifr_name, s_stNetInst[ulDevId].pstDevInfo->szName, IF_NAME_SIZE-1);
	VK_MEM_Memset(pucMacAddr, 0, MAC_ADDR_LEN);
	if(ioctl(s_stNetInst[ulDevId].s_sockfd, SIOCGIFHWADDR, &ifreq) < 0)
	{
		PrintError("[%s:%d] ioctl(SIOCGIFHWADDR) Error : %s!\n", __FUNCTION__, __LINE__, strerror(errno));
		return DRV_ERR;
	}
	VK_memcpy(pucMacAddr, &ifreq.ifr_hwaddr.sa_data, sizeof(struct ether_addr));

	return DRV_OK;
}

/*******************************************************************************
* function : P_NETWORK_SetMacAddr
* description :
* input :
* output :
* return :
* ??Ÿ ?????ڷ? ?? ????
*******************************************************************************/
DRV_Error P_NETWORK_SetMacAddr(HUINT32 ulDevId, HUINT8 *pucMacAddr)
{
	struct ifreq ifreq;

	VK_MEM_Memset(&ifreq, 0, sizeof(struct ifreq));

	/* set mac addr */
	VK_strncpy(ifreq.ifr_name, s_stNetInst[ulDevId].pstDevInfo->szName, IF_NAME_SIZE-1);
	ifreq.ifr_addr.sa_family = AF_UNIX;
	VK_memcpy(&ifreq.ifr_hwaddr.sa_data, pucMacAddr, sizeof(struct ether_addr));
	if(ioctl(s_stNetInst[ulDevId].s_sockfd, SIOCSIFHWADDR, (char *)&ifreq) < 0)
	{
		PrintError("[%s:%d] ioctl(SIOCSIFHWADDR) Error : %s!\n", __FUNCTION__, __LINE__, strerror(errno));
		return DRV_ERR;
	}

	return DRV_OK;
}

/*******************************************************************************
* function : P_NETWORK_GetDevLinkStatus
* description :
* input :
* output :
* return :
* ??Ÿ ?????ڷ? ?? ????
*******************************************************************************/
DI_ERR_CODE P_NETWORK_GetDevLinkStatus(HUINT32 ulDevId, HBOOL *pbLinkState)
{
	struct ifreq ifreq;
	struct ethtool_value ethdata;

	VK_MEM_Memset(&ifreq, 0, sizeof(struct ifreq));

	/* get link status */
 	VK_strncpy(ifreq.ifr_name, s_stNetInst[ulDevId].pstDevInfo->szName, IF_NAME_SIZE-1);
	ethdata.cmd = ETHTOOL_GLINK;
	ifreq.ifr_data = (caddr_t)&ethdata;
	if(ioctl(s_stNetInst[ulDevId].s_sockfd, SIOCETHTOOL, (char *)&ifreq) < 0)
	{
		//PrintError("[%s:%d] ioctl(SIOCETHTOOL) Error : %s!\n", __FUNCTION__, __LINE__, strerror(errno));
		return DRV_ERR;
	}

	if(ethdata.data)
	{
		*pbLinkState = TRUE;
	}
	else
	{
		*pbLinkState = FALSE;
	}

	PrintDebug("[%s:%d] Link Status : %d : %s!\n", __FUNCTION__, __LINE__, *pbLinkState, strerror(errno));

	return DI_ERR_OK;
}

/*******************************************************************************
* function : P_NETWORK_GetDevStatus
* description :
* input :
* output :
* return :
* ??Ÿ ?????ڷ? ?? ????
*******************************************************************************/
DRV_Error P_NETWORK_GetDevStatus(HUINT32 ulDevId, HBOOL *pbIsUp, HBOOL *pbIsDynamic)
{
	struct ifreq ifreq;

	VK_MEM_Memset(&ifreq, 0, sizeof(struct ifreq));

	/* get status flags */
	VK_strncpy(ifreq.ifr_name, s_stNetInst[ulDevId].pstDevInfo->szName, IF_NAME_SIZE-1);
	if(ioctl(s_stNetInst[ulDevId].s_sockfd, SIOCGIFFLAGS, (char *)&ifreq) < 0)
	{
		PrintError("[%s:%d] ioctl(SIOCGIFFLAGS) Error : %s!\n", __FUNCTION__, __LINE__, strerror(errno));
		return DRV_ERR;
	}

	if(ifreq.ifr_flags&IFF_UP)
		*pbIsUp = TRUE;
	else
		*pbIsUp = FALSE;

	if(ifreq.ifr_flags&IFF_DYNAMIC)
		*pbIsDynamic = TRUE;
	else
		*pbIsDynamic = FALSE;

	return DRV_OK;
}

/*******************************************************************************
* function : P_NETWORK_SetDevStatus
* description :
* input :
* output :
* return :
* ??Ÿ ?????ڷ? ?? ????
*******************************************************************************/
DRV_Error P_NETWORK_SetDevStatus(HUINT32 ulDevId, HBOOL bUp, HBOOL bDynamic)
{
	struct ifreq ifreq;

	VK_MEM_Memset(&ifreq, 0, sizeof(struct ifreq));

	/* get status flags */
	VK_strncpy(ifreq.ifr_name, s_stNetInst[ulDevId].pstDevInfo->szName, IF_NAME_SIZE-1);
	if(ioctl(s_stNetInst[ulDevId].s_sockfd, SIOCGIFFLAGS, (char *)&ifreq) < 0)
	{
		PrintError("[%s:%d] ioctl(SIOCGIFFLAGS) Error : %s!\n", __FUNCTION__, __LINE__, strerror(errno));
		return DRV_ERR;
	}

	if(bUp)
		ifreq.ifr_flags |= (IFF_UP|IFF_RUNNING);
	else
		ifreq.ifr_flags &= (~IFF_UP);

	if(bDynamic)
		ifreq.ifr_flags |= (IFF_DYNAMIC);
	else
		ifreq.ifr_flags &= (~IFF_DYNAMIC);

	/* set status flags */
	VK_strncpy(ifreq.ifr_name, s_stNetInst[ulDevId].pstDevInfo->szName, IF_NAME_SIZE-1);
	if(ioctl(s_stNetInst[ulDevId].s_sockfd, SIOCSIFFLAGS, (char *)&ifreq) < 0)
	{
		PrintError("[%s:%d] ioctl(SIOCSIFFLAGS) Error : %s!\n", __FUNCTION__, __LINE__, strerror(errno));
		return DRV_ERR;
	}

	return DRV_OK;
}

/*******************************************************************************
* function : P_NETWORK_GetIpAddr
* description :
* input :
* output :
* return :
* ??Ÿ ?????ڷ? ?? ????
*******************************************************************************/
DRV_Error P_NETWORK_GetIpAddr(HUINT32 ulDevId, HUINT8 *pucIpAddr, HUINT8 *pucBcastAddr, HUINT8 *pucMaskAddr)
{
	struct ifreq ifreq;
	struct sockaddr_in *sin;

	VK_MEM_Memset(&ifreq, 0, sizeof(struct ifreq));

	/* get ip addr */
	VK_strncpy(ifreq.ifr_name, s_stNetInst[ulDevId].pstDevInfo->szName, IF_NAME_SIZE-1);
	ifreq.ifr_addr.sa_family = AF_INET;
	VK_MEM_Memset(pucIpAddr, 0, IPV4_ADDR_LEN);
	if(ioctl(s_stNetInst[ulDevId].s_sockfd, SIOCGIFADDR, (char *)&ifreq)==0)
	{
		sin = (struct sockaddr_in *)&ifreq.ifr_addr;
		VK_memcpy(pucIpAddr, &sin->sin_addr, sizeof(struct in_addr));

		/* get broadcast addr */
		VK_strncpy(ifreq.ifr_name, s_stNetInst[ulDevId].pstDevInfo->szName, IF_NAME_SIZE-1);
		ifreq.ifr_addr.sa_family = AF_INET;
		VK_MEM_Memset(pucBcastAddr, 0, IPV4_ADDR_LEN);
		if(ioctl(s_stNetInst[ulDevId].s_sockfd, SIOCGIFBRDADDR, (char *)&ifreq) < 0)
		{
			PrintError("[%s:%d] ioctl(SIOCGIFBRDADDR) Error : %s!\n", __FUNCTION__, __LINE__, strerror(errno));
			return DRV_ERR;
		}
		sin = (struct sockaddr_in *)&ifreq.ifr_addr;
		VK_memcpy(pucBcastAddr, &sin->sin_addr, sizeof(struct in_addr));

		/* get netmask addr */
		VK_strncpy(ifreq.ifr_name, s_stNetInst[ulDevId].pstDevInfo->szName, IF_NAME_SIZE-1);
		ifreq.ifr_addr.sa_family = AF_INET;
		VK_MEM_Memset(pucMaskAddr, 0, IPV4_ADDR_LEN);
		if(ioctl(s_stNetInst[ulDevId].s_sockfd, SIOCGIFNETMASK, (char *)&ifreq) < 0)
		{
			PrintError("[%s:%d] ioctl(SIOCGIFNETMASK) Error : %s!\n", __FUNCTION__, __LINE__, strerror(errno));
			return DRV_ERR;
		}
		sin = (struct sockaddr_in *)&ifreq.ifr_addr;
		VK_memcpy(pucMaskAddr, &sin->sin_addr, sizeof(struct in_addr));
	}

	return DRV_OK;
}

/*******************************************************************************
* function : P_NETWORK_SetIpAddr
* description :
* input :
* output :
* return :
* ??Ÿ ?????ڷ? ?? ????
*******************************************************************************/
DRV_Error P_NETWORK_SetIpAddr(HUINT32 ulDevId, HUINT8 *pucIpAddr, HUINT8 *pucBcastAddr, HUINT8 *pucMaskAddr)
{
	struct ifreq ifreq;
	struct sockaddr_in *sin;

	VK_MEM_Memset(&ifreq, 0, sizeof(struct ifreq));

	/* set ip addr */
	if(IS_VALID_IPV4_ADDR(pucIpAddr))
	{
		VK_strncpy(ifreq.ifr_name, s_stNetInst[ulDevId].pstDevInfo->szName, IF_NAME_SIZE-1);
		sin = (struct sockaddr_in *)&ifreq.ifr_addr;
		VK_MEM_Memset(sin, 0, sizeof(struct sockaddr));
		sin->sin_family = AF_INET;
		sin->sin_addr.s_addr = (pucIpAddr[3]<<24)|(pucIpAddr[2]<<16)|(pucIpAddr[1]<<8)|(pucIpAddr[0]);
		if(ioctl(s_stNetInst[ulDevId].s_sockfd, SIOCSIFADDR, (char *)&ifreq) < 0)
		{
			PrintError("[%s:%d] ioctl(SIOCSIFADDR) Error : %s!\n", __FUNCTION__, __LINE__, strerror(errno));
			return DRV_ERR;
		}
	}

	/* set broadcast addr */
	if(IS_VALID_IPV4_ADDR(pucBcastAddr))
	{
		VK_strncpy(ifreq.ifr_name, s_stNetInst[ulDevId].pstDevInfo->szName, IF_NAME_SIZE-1);
		sin = (struct sockaddr_in *)&ifreq.ifr_addr;
		VK_MEM_Memset(sin, 0, sizeof(struct sockaddr));
		sin->sin_family = AF_INET;
		sin->sin_addr.s_addr = (pucBcastAddr[3]<<24)|(pucBcastAddr[2]<<16)|(pucBcastAddr[1]<<8)|(pucBcastAddr[0]);
		if(ioctl(s_stNetInst[ulDevId].s_sockfd, SIOCSIFBRDADDR, (char *)&ifreq) < 0)
		{
			PrintError("[%s:%d] ioctl(SIOCSIFBRDADDR) Error : %s!\n", __FUNCTION__, __LINE__, strerror(errno));
			return DRV_ERR;
		}
	}

	/* set netmask addr */
	if(IS_VALID_IPV4_ADDR(pucMaskAddr))
	{
		VK_strncpy(ifreq.ifr_name, s_stNetInst[ulDevId].pstDevInfo->szName, IF_NAME_SIZE-1);
		sin = (struct sockaddr_in *)&ifreq.ifr_addr;
		VK_MEM_Memset(sin, 0, sizeof(struct sockaddr));
		sin->sin_family = AF_INET;
		sin->sin_addr.s_addr = (pucMaskAddr[3]<<24)|(pucMaskAddr[2]<<16)|(pucMaskAddr[1]<<8)|(pucMaskAddr[0]);
		if(ioctl(s_stNetInst[ulDevId].s_sockfd, SIOCSIFNETMASK, (char *)&ifreq) < 0)
		{
			PrintError("[%s:%d] ioctl(SIOCSIFBRDADDR) Error : %s!\n", __FUNCTION__, __LINE__, strerror(errno));
			return DRV_ERR;
		}
	}

	return DRV_OK;
}

/*******************************************************************************
* function : P_NETWORK_GetGateway
* description :
* input :
* output :
* return :
* ??Ÿ ?????ڷ? ?? ????
*******************************************************************************/
static DRV_Error P_NETWORK_GetRouteInfo(HUINT32 ulDevId, DI_NETWORK_GET_ROUTE_INFO_t *pRouteInfo)
{
	HCHAR aucBuf[256];
	HUINT8 aucIface[IF_NAME_SIZE];
	HUINT32 unDest, unGateway, unMask;
	HINT32 nFlags, nRefCnt, nUse, nMetric, nMtu, nWindow, nIrtt;
	FILE* fp;
	int ret;

	fp = fopen("/proc/net/route", "r");
	if (!fp)
	{
		PrintError("[%s:%d] fopen Error : %s!\n", __FUNCTION__, __LINE__, strerror(errno));
		return DRV_ERR;
	}

	while(fgets(aucBuf, sizeof(aucBuf), fp))
	{
		if(!VK_strncmp(aucBuf, "Iface", 5))
		{
		    continue;
		}

		VK_MEM_Memset(aucIface, 0x0, IF_NAME_SIZE);
		ret = VK_sscanf(aucBuf, "%s\t%x\t%x\t%d\t%d\t%d\t%d\t%x\t%d\t%d\t%d",
			    aucIface, &unDest, &unGateway, &nFlags, &nRefCnt, &nUse, &nMetric, &unMask, &nMtu, &nWindow, &nIrtt);

		if(ret < 11)
		{
			PrintError("[%s:%d] VK_sscanf Error : %s!\n", __FUNCTION__, __LINE__, strerror(errno));
			continue;
		}

		if(!VK_strncmp(s_stNetInst[ulDevId].pstDevInfo->szName, aucIface, IF_NAME_SIZE) &&
			(unDest != 0) &&
			(nFlags != (RTF_GATEWAY | RTF_UP)) &&
			(unMask != 0))
		{
			VK_strncpy(&pRouteInfo->cIfName[0], aucIface, IF_NAME_SIZE);

			pRouteInfo->ucDestination[0] = (unDest&0x000000FF);
			pRouteInfo->ucDestination[1] = (unDest&0x0000FF00)>>8;
			pRouteInfo->ucDestination[2] = (unDest&0x00FF0000)>>16;
			pRouteInfo->ucDestination[3] = (unDest&0xFF000000)>>24;

			pRouteInfo->ucGateway[0] = (unGateway&0x000000FF);
			pRouteInfo->ucGateway[1] = (unGateway&0x0000FF00)>>8;
			pRouteInfo->ucGateway[2] = (unGateway&0x00FF0000)>>16;
			pRouteInfo->ucGateway[3] = (unGateway&0xFF000000)>>24;

			pRouteInfo->ulFlags = (HUINT16)nFlags;
			pRouteInfo->ulRefCnt = nRefCnt;
			pRouteInfo->ulUse = nUse;
			pRouteInfo->ulMetric = nMetric;

			pRouteInfo->ucMaskAddr[0] = (unMask&0x000000FF);
			pRouteInfo->ucMaskAddr[1] = (unMask&0x0000FF00)>>8;
			pRouteInfo->ucMaskAddr[2] = (unMask&0x00FF0000)>>16;
			pRouteInfo->ucMaskAddr[3] = (unMask&0xFF000000)>>24;

			pRouteInfo->ulMtu = nMtu;
			pRouteInfo->ulWindow = nWindow;
			pRouteInfo->ulIrtt = nIrtt;

			break;
		}
	}

	fclose(fp);

	return DRV_OK;
}

/*******************************************************************************
* function : P_NETWORK_SetGateway
* description :
* input :
* output :
* return :
* ??Ÿ ?????ڷ? ?? ????
*******************************************************************************/
static DRV_Error P_NETWORK_SetRouteInfo(HUINT32 ulDevId, DI_NETWORK_SET_ROUTE_INFO_t SetRoute)
{
	struct rtentry rm;
	struct sockaddr_in *addr;

	VK_MEM_Memset(&rm, 0, sizeof(rm));

	addr = (struct sockaddr_in*) &rm.rt_gateway;
	addr->sin_family = AF_INET;
	addr->sin_addr.s_addr = ((SetRoute.ucGateway[3]<<24)|
							(SetRoute.ucGateway[2]<<16)|
							(SetRoute.ucGateway[1]<<8)|
							(SetRoute.ucGateway[0]));

	addr = (struct sockaddr_in*) &rm.rt_dst;
	addr->sin_family = AF_INET;
	addr->sin_addr.s_addr = ((SetRoute.ucDestination[3]<<24)|
							(SetRoute.ucDestination[2]<<16)|
							(SetRoute.ucDestination[1]<<8)|
							(SetRoute.ucDestination[0]));

	addr = (struct sockaddr_in*) &rm.rt_genmask;
	addr->sin_family = AF_INET;
	addr->sin_addr.s_addr = ((SetRoute.ucMaskAddr[3]<<24)|
							(SetRoute.ucMaskAddr[2]<<16)|
							(SetRoute.ucMaskAddr[1]<<8)|
							(SetRoute.ucMaskAddr[0]));

	/* Fill ETC info */
	rm.rt_dev = SetRoute.cIfName;
	rm.rt_flags = SetRoute.ulFlags;
	rm.rt_metric = SetRoute.ulMetric + 1; /* +1 for binary compatibility!	*/
	rm.rt_mtu = SetRoute.ulMtu;
	rm.rt_window = SetRoute.ulWindow;
	rm.rt_irtt = SetRoute.ulIrtt;

	if(SetRoute.eRouteMode == DI_NETWORK_ADD_ROUTE)
	{
		if(ioctl(s_stNetInst[ulDevId].s_sockfd, SIOCADDRT, &rm) < 0)
		{
			PrintError("[%s:%d] ioctl(SIOCADDRT) Error : %s!\n", __FUNCTION__, __LINE__, strerror(errno));
			return DI_ERR_ERROR;
		}
	}
	else if(SetRoute.eRouteMode == DI_NETWORK_DELETE_ROUTE)
	{
		if(ioctl(s_stNetInst[ulDevId].s_sockfd, SIOCDELRT, &rm) < 0)
		{
			PrintError("[%s:%d] ioctl(SIOCDELRT) Error : %s!\n", __FUNCTION__, __LINE__, strerror(errno));
			return DI_ERR_ERROR;
		}
	}
	else
	{
		PrintError("[%s:%d] eRouteMode %d : %s!\n", __FUNCTION__, __LINE__, SetRoute.eRouteMode, strerror(errno));
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;
}

/*******************************************************************************
* function : P_NETWORK_GetGateway
* description :
* input :
* output :
* return :
* ??Ÿ ?????ڷ? ?? ????
*******************************************************************************/
DI_ERR_CODE P_NETWORK_GetGateway(HUINT32 ulDevId, DI_NETWORK_ROUTE_PARAMS_t *pstParams)
{
	HCHAR aucBuf[256];
	HUINT8 aucIface[IF_NAME_SIZE];
	HUINT32 unDest, unGateway, unMask;
	HINT32 nFlags, nRefCnt, nUse, nMetric, nMtu, nWindow, nIrtt;
	FILE* fp;
	int ret;

	fp = fopen("/proc/net/route", "r");
	if (!fp)
	{
		PrintError("[%s:%d] fopen Error : %s!\n", __FUNCTION__, __LINE__, strerror(errno));
		return DI_ERR_ERROR;
	}

	while(fgets(aucBuf, sizeof(aucBuf), fp))
	{
		if(!VK_strncmp(aucBuf, "Iface", 5))
		    continue;

		ret = VK_sscanf(aucBuf, "%s\t%x\t%x\t%d\t%d\t%d\t%d\t%x\t%d\t%d\t%d",
			    aucIface, &unDest, &unGateway, &nFlags, &nRefCnt, &nUse, &nMetric, &unMask, &nMtu, &nWindow, &nIrtt);

		if(ret < 11)
		{
			PrintError("[%s:%d] VK_sscanf Error : %s!\n", __FUNCTION__, __LINE__, strerror(errno));
			continue;
		}

		if(!VK_strncmp(s_stNetInst[ulDevId].pstDevInfo->szName, aucIface, VK_strlen(s_stNetInst[ulDevId].pstDevInfo->szName))
			&& unDest==0 && (nFlags&RTF_GATEWAY) && unMask==0)
		{
			pstParams->aucRouteAddr[0] = (unGateway&0x000000FF);
			pstParams->aucRouteAddr[1] = (unGateway&0x0000FF00)>>8;
			pstParams->aucRouteAddr[2] = (unGateway&0x00FF0000)>>16;
			pstParams->aucRouteAddr[3] = (unGateway&0xFF000000)>>24;
			pstParams->ulMetric = nMetric;
			break;
		}
	}

	fclose(fp);

	return DI_ERR_OK;
}

/*******************************************************************************
* function : P_NETWORK_SetGateway
* description :
* input :
* output :
* return :
* ??Ÿ ?????ڷ? ?? ????
*******************************************************************************/
DRV_Error P_NETWORK_SetGateway(HUINT32 ulDevId, P_NETWORK_ADDR_OP_e eOp, DI_NETWORK_ROUTE_PARAMS_t *pstParams)
{
	struct rtentry rm;
	struct sockaddr_in gateway;

	VK_memset(&rm, 0, sizeof(rm));
	VK_memset(&gateway, 0, sizeof(gateway));

	rm.rt_dev = (char *)s_stNetInst[ulDevId].pstDevInfo->szName;

	gateway.sin_family = AF_INET;
	gateway.sin_addr.s_addr = (pstParams->aucRouteAddr[3]<<24)|(pstParams->aucRouteAddr[2]<<16)|(pstParams->aucRouteAddr[1]<<8)|(pstParams->aucRouteAddr[0]);
	gateway.sin_port = 0;

	(( struct sockaddr_in*)&rm.rt_dst)->sin_family = AF_INET;
	(( struct sockaddr_in*)&rm.rt_dst)->sin_addr.s_addr = INADDR_ANY;
	(( struct sockaddr_in*)&rm.rt_dst)->sin_port = 0;

	(( struct sockaddr_in*)&rm.rt_genmask)->sin_family = AF_INET;
	(( struct sockaddr_in*)&rm.rt_genmask)->sin_addr.s_addr = 0;
	(( struct sockaddr_in*)&rm.rt_genmask)->sin_port = 0;

	VK_memcpy((void *) &rm.rt_gateway, &gateway, sizeof(gateway));
	rm.rt_flags = RTF_UP | RTF_GATEWAY;
	rm.rt_metric = pstParams->ulMetric + 1;
	if(eOp == P_NETWORK_ADDR_ADD)
	{
		if(ioctl(s_stNetInst[ulDevId].s_sockfd, SIOCADDRT, &rm) < 0)
		{
			PrintError("[%s:%d] ioctl(SIOCADDRT) Error : %s!\n", __FUNCTION__, __LINE__, strerror(errno));
			return DI_ERR_ERROR;
		}
	}
	else if(eOp == P_NETWORK_ADDR_DEL)
	{
		if(ioctl(s_stNetInst[ulDevId].s_sockfd, SIOCDELRT, &rm) < 0)
		{
			PrintError("[%s:%d] ioctl(SIOCDELRT) Error : %s!\n", __FUNCTION__, __LINE__, strerror(errno));
			return DI_ERR_ERROR;
		}
	}

	return DI_ERR_OK;
}

/*******************************************************************************
* function : P_NETWORK_InitResolver
* description :
* input :
* output :
* return :
* ??Ÿ ?????ڷ? ?? ????
*******************************************************************************/
void P_NETWORK_InitResolver(void)
{
	res_init();
}

/*******************************************************************************
* function : P_NETWORK_CheckDhcpResultTask
* description : check the result of dhcpc.
* input :
* output :
* return :
*
*******************************************************************************/

#define BUF_SIZE 128

void P_NETWORK_CheckDhcpResultTask(void *pvParam)
{
	DI_NETWORK_CALLBACK_DATA_t stCallbackData;

	HINT32 fd;
	HINT32 num;
	HUINT32 i;

	HUINT8 buf[BUF_SIZE] = {0, };
	HCHAR *dev = NULL;
	HCHAR *event = NULL;

	HBOOL bFire = FALSE;

	HAPPY(pvParam);

	/* Open FIFO */
	fd = open(DHCP_FIFO, O_RDWR, 0666);
	if(fd < 0)
	{
		PrintError("[%s:%d] open %s error!\n", __FUNCTION__, __LINE__, DHCP_FIFO);
		return;
	}

	VK_memset(&stCallbackData, 0, 0x00);

	for(i=0; i<s_ulNetDevNum; i++)
	{
		PrintDebug("s_stNetInst[%d].pstDevInfo->ulDevId : %d, szName : %s\n",
						i,
						s_stNetInst[i].pstDevInfo->ulDevId,
						s_stNetInst[i].pstDevInfo->szName);
	}

	/* Read the result from FIFO */
	while(1)
	{
		/* Initialize variable */
		bFire = FALSE;

		VK_memset(buf, 0x00, BUF_SIZE);

		dev = NULL;
		event = NULL;

		num = read(fd, buf, BUF_SIZE-1);
		buf[BUF_SIZE-1] = 0; //for clearing prevent defect
		if(num < 0)
		{
			PrintError("[%s:%d] read %s error!\n", __FUNCTION__, __LINE__, DHCP_FIFO);
			continue;
		}

		/* make null terminated string */
		buf[BUF_SIZE-1] = 0;

		DI_UART_Print("[%s:%d] Input from FIFO : %s\n", __FUNCTION__, __LINE__, buf);

		/* DHCP event string should be [interface]/[event]. so use '/' as token */
		dev = strtok(buf, "/");
		event = strtok(NULL, "/");

		/* Scan and set device id */
		for(i=0; i<s_ulNetDevNum; i++)
		{
			if(!VK_strcmp(s_stNetInst[i].pstDevInfo->szName, dev))
				break;
		}

		if(i == s_ulNetDevNum)
		{
			PrintError("[%s:%d] No Such Device(%s) Error!!\n", __FUNCTION__, __LINE__, dev);
			continue;
		}

		stCallbackData.ulDevId = s_stNetInst[i].pstDevInfo->ulDevId;
		DI_UART_Print("[%s:%d] i : %d, Device name : %s\n", __FUNCTION__, __LINE__,
									i, s_stNetInst[i].pstDevInfo->szName);
#if defined(CONFIG_DHCPCD)
		/*	Check previous event and callback when the event is changed */
		if(!VK_strcmp(event, "down"))
		{
			DI_UART_Print("[%s:%d] DHCP FAIL!!!\n", __FUNCTION__, __LINE__);

			stCallbackData.eEvent = DI_NETWORK_EVENT_DHCP_FAIL;
			bFire = TRUE;
		}
		else if((!VK_strcmp(event, "up"))||(!VK_strcmp(event, "new")))
		{
			DI_UART_Print("[%s:%d] DHCP SUCCESS!!!\n", __FUNCTION__, __LINE__);

			stCallbackData.eEvent = DI_NETWORK_EVENT_DHCP_SUCCESS;
			bFire = TRUE;

			P_NETWORK_InitResolver();
		}
#else
		/*  Check previous event and callback when the event is changed */
		if(!VK_strcmp(event, "leasefail"))
		{
			DI_UART_Print("[%s:%d] DHCP FAIL!!!\n", __FUNCTION__, __LINE__);

			stCallbackData.eEvent = DI_NETWORK_EVENT_DHCP_FAIL;
			bFire = TRUE;
		}
		else if(!VK_strcmp(event, "bound"))
		{
			DI_UART_Print("[%s:%d] DHCP SUCCESS!!!\n", __FUNCTION__, __LINE__);

			stCallbackData.eEvent = DI_NETWORK_EVENT_DHCP_SUCCESS;
			bFire = TRUE;

			P_NETWORK_InitResolver();
		}
		else if(!VK_strcmp(event, "deconfig"))
		{
			DI_UART_Print("[%s:%d] DHCP deconfig !!!\n", __FUNCTION__, __LINE__);

			/* Do nothing */
		}
#endif
		if(!s_stNetInst[i].bDhcpcUserKilled && bFire)
			(void)VK_MSG_SendTimeout(s_ulEventMsgQId, &stCallbackData, sizeof(DI_NETWORK_CALLBACK_DATA_t), 0);

	}
}

/*******************************************************************************
* function : P_NETWORK_ExecDhcpClientTask
* description :
* input :
* output :
* return :
* ??Ÿ ?????ڷ? ?? ????
*******************************************************************************/
void P_NETWORK_ExecDhcpClientTask(void *pvParam)
{
	P_NETWORK_INSTANCE_t *pstInstance;
	DI_NETWORK_CALLBACK_DATA_t stCallbackData;
	HCHAR szCmdBuf[384];
	HCHAR szPidFile[128];
	HUINT32	unStrLen;
	int vkErr;
	HULONG ulCurrentTaskId;

	pstInstance = (P_NETWORK_INSTANCE_t *)pvParam;
	stCallbackData.ulDevId = pstInstance->pstDevInfo->ulDevId;
	ulCurrentTaskId = pstInstance->ulDhcpClientTaskId;

	VK_MEM_Memset(szCmdBuf, 0x0, 128);

	VK_snprintf(szPidFile, 128, "%s.%s", DHCPC_PID_FILE, pstInstance->pstDevInfo->szName);

#if defined(CONFIG_DHCPCD)
	if(pstInstance->stDhcpcParams.bUseVendorClass)
	{
		unStrLen = VK_snprintf(szCmdBuf, 128, "dhcpcd -t %d -i %s -c %s -L %s %s",
									DHCPC_PAUSE_TIME,
									pstInstance->stDhcpcParams.aucVendorClass,
									DHCPC_SCRIPT_PATH,
									DHCPCD_CONFIG_DIR,
									pstInstance->pstDevInfo->szName);
	}
	else
	{
		unStrLen = VK_snprintf(szCmdBuf, 128, "dhcpcd -t %d -c %s -L %s  %s",
									DHCPC_PAUSE_TIME,
									DHCPC_SCRIPT_PATH,
									DHCPCD_CONFIG_DIR,
									pstInstance->pstDevInfo->szName);
	}

	/* Host name & Request specific IP */
	if(pstInstance->stDhcpcParams.bUseRequestIp == TRUE)
	{
		VK_snprintf(&szCmdBuf[VK_strlen(szCmdBuf)], (128-VK_strlen(szCmdBuf)-1), " -s %s", pstInstance->stDhcpcParams.cRequestIp);
	}
#else
	if(pstInstance->stDhcpcParams.bUseVendorClass)
	{
		unStrLen = VK_snprintf(szCmdBuf, 128, "udhcpc -t %d -T %d -p %s -i %s -b -V %s -s %s", pstInstance->stDhcpcParams.ulWaitSeconds/DHCPC_PAUSE_TIME,
									DHCPC_PAUSE_TIME,
									szPidFile,
									pstInstance->pstDevInfo->szName,
									pstInstance->stDhcpcParams.aucVendorClass,
									DHCPC_SCRIPT_PATH);
	}
	else
	{
		unStrLen = VK_snprintf(szCmdBuf, 128, "udhcpc -t %d -T %d -p %s -i %s -b -s %s", pstInstance->stDhcpcParams.ulWaitSeconds/DHCPC_PAUSE_TIME,
									DHCPC_PAUSE_TIME,
									szPidFile,
									pstInstance->pstDevInfo->szName,
									DHCPC_SCRIPT_PATH);
	}

	if(pstInstance->stDhcpcParams.ulWaitSecondsAfterFailure != 0)
	{
		unStrLen = VK_snprintf(&szCmdBuf[unStrLen], 128, " -A %d", pstInstance->stDhcpcParams.ulWaitSecondsAfterFailure);
	}

	/* Host name & Request specific IP */
	if(pstInstance->stDhcpcParams.bUseRequestIp == TRUE)
	{
		VK_snprintf(&szCmdBuf[VK_strlen(szCmdBuf)], (128-VK_strlen(szCmdBuf)-1), " -r %s", pstInstance->stDhcpcParams.cRequestIp);
	}
#endif

	if(pstInstance->stDhcpcParams.bUseHostName == TRUE)
	{
		VK_snprintf(&szCmdBuf[VK_strlen(szCmdBuf)], (128-VK_strlen(szCmdBuf)-1), " -h %s", pstInstance->stDhcpcParams.cHostName);
	}

	vkErr = VK_SYSTEM_Command(szCmdBuf);
	if(vkErr)
	{
		PrintError("[%s:%d] VK_SYSTEM_Command() error!!, cmd : %s\n", __FUNCTION__, __LINE__, szCmdBuf);
	}

	pstInstance->ulDhcpClientTaskId = 0;
	VK_TASK_Destroy(ulCurrentTaskId);	/* async task destroy */

	while(1)	/* wait until self task detroy */
	{
		VK_TASK_Sleep(0xFFFFFFFF);
	}
}

/*******************************************************************************
* function : P_NETWORK_StopDhcpClient
* description :
* input :
* output :
* return :
* ??Ÿ ?????ڷ? ?? ????
*******************************************************************************/
DRV_Error P_NETWORK_StopDhcpClient(HUINT32 ulDevId, HBOOL bUserKill)
{
	HCHAR szPidFile[128], szCmdFileName[128], szPidBuf[128], szCmdBuf[128];
	HUINT32 unPid, ret;
	FILE *fp = NULL;
	FILE *cmd_fp;
	HBOOL bKillOk = FALSE;
#if defined(CONFIG_SHARED_COMPILE)
	struct flock	fl;
	int	fd;
#endif

	s_stNetInst[ulDevId].bDhcpcUserKilled = bUserKill;
#if defined(CONFIG_DHCPCD)
	VK_snprintf(szPidFile, 128, "%s/dhcpcd-%s.pid", DHCPCD_CONFIG_DIR,s_stNetInst[ulDevId].pstDevInfo->szName);
#else
	VK_snprintf(szPidFile, 128, "%s.%s", DHCPC_PID_FILE, s_stNetInst[ulDevId].pstDevInfo->szName);
#endif
#if defined(CONFIG_SHARED_COMPILE)
	fd = open(szPidFile, O_RDWR);
	if (fd < 0)
	{
		PrintError("[%s:%d] open rrror : %s!\n", __FUNCTION__, __LINE__, strerror(errno));
		goto END;
	}
	fp = fdopen(fd, "r");
#else
	fp = fopen(szPidFile, "r");
#endif

	if(fp)
	{
#if defined(CONFIG_SHARED_COMPILE)
		fl.l_type = F_WRLCK;
		fl.l_whence = SEEK_SET;
		fl.l_start = 0;
		fl.l_len = 0;

		if (fcntl(fd, F_SETLKW, &fl) == -1) {
			PrintError("[%s] Can't lock %s file!!\n", __FUNCTION__, szPidFile);
			goto END;
		}
#endif
		if(fgets(szPidBuf, 8, fp))
		{
			unPid = VK_atoi(szPidBuf);

			VK_snprintf(szCmdFileName, 128, "/proc/%d/cmdline", unPid);
			cmd_fp = fopen(szCmdFileName, "r");
			if(cmd_fp)
			{
				if(fgets(szCmdBuf, 128, cmd_fp))
				{
#if defined(CONFIG_DHCPCD)
					if(VK_strstr(szCmdBuf, "dhcpcd"))
#else
					if(VK_strstr(szCmdBuf, "udhcpc"))
#endif
					{
						kill(unPid, SIGKILL);
						bKillOk = TRUE;
					}
				}
				fclose(cmd_fp);
			}
		}
	}

END:
	if(NULL != fp)
		fclose(fp);

#if defined(CONFIG_SHARED_COMPILE)
	if(fd >= 0)
		close(fd);
#endif

	if(NULL != fp)
	{
		ret = remove(szPidFile); /* remove file */
		if(ret != 0)
		{
			PrintError("[%s] Can't delete %s file!!\n", __FUNCTION__, szPidFile);
			return DRV_ERR;
		}
	}
	if(bKillOk)
		return DRV_OK;
	else
		return DRV_ERR;
}

/*******************************************************************************
* function : P_NETWORK_StartDhcpClient
* description :
* input :
* output :
* return :
* ??Ÿ ?????ڷ? ?? ????
*******************************************************************************/
DRV_Error P_NETWORK_StartDhcpClient(HUINT32 ulDevId, DI_NETWORK_DHCPC_PARAMS_t *pstParams)
{
	int vkErr;

	if(s_stNetInst[ulDevId].ulDhcpClientTaskId)
	{
		PrintError("[%s:%d] : the client was already executed!!\n", __FUNCTION__, __LINE__);
		return DRV_ERR;
	}

	P_NETWORK_StopDhcpClient(ulDevId, FALSE);

	VK_memcpy(&s_stNetInst[ulDevId].stDhcpcParams, pstParams, sizeof(DI_NETWORK_DHCPC_PARAMS_t));

	/* Exec udhcpc */
	vkErr = VK_TASK_Create(P_NETWORK_ExecDhcpClientTask,
				NETWORK_TASK_PRIORITY,
				NETWORK_TASK_STACK_SIZE,
				"P_NETWORK_ExecDhcpClientTask",
				&s_stNetInst[ulDevId],
				&s_stNetInst[ulDevId].ulDhcpClientTaskId,
				TRUE);
	if(vkErr != VK_OK)
	{
		PrintError("[%s:%d] VK_TASK_Create Error!\n", __FUNCTION__, __LINE__);
		return DRV_ERR;
	}

	vkErr = VK_TASK_Start(s_stNetInst[ulDevId].ulDhcpClientTaskId);
	if(vkErr != VK_OK)
	{
		PrintError("[%s:%d] VK_TASK_Start Error!\n", __FUNCTION__, __LINE__);
		return DRV_ERR;
	}

	return DRV_OK;
}

/*******************************************************************************
* function : P_NETWORK_ExecDhcpServerTask
* description :
* input :
* output :
* return :
* ??Ÿ ?????ڷ? ?? ????
*******************************************************************************/
void P_NETWORK_ExecDhcpServerTask(void *pvParam)
{
	P_NETWORK_INSTANCE_t *pstInstance;
	HCHAR szCmdBuf[128], szFile[128];	;
	FILE *fp = NULL;
	int vkErr, i;
	HULONG ulCurrentTaskId;
#if defined(CONFIG_SHARED_COMPILE)
	struct flock	fl;
	int	fd;
#endif

	pstInstance = (P_NETWORK_INSTANCE_t *)pvParam;
	ulCurrentTaskId = pstInstance->ulDhcpServerTaskId;

	/* create new dhcp server configuration file */
	VK_snprintf(szFile, 128, "%s.%s", DHCPS_CONF_FILE, pstInstance->pstDevInfo->szName);

#if defined(CONFIG_SHARED_COMPILE)
	fd = open(szFile, O_CREAT | O_RDWR);
	if (fd < 0)
	{
		PrintError("[%s:%d] open rrror : %s!\n", __FUNCTION__, __LINE__, strerror(errno));
		goto END;
	}
	fp = fdopen(fd, "w");
#else
	fp = fopen(szFile, "w");
#endif

	if(fp == NULL)
	{
		PrintError("[%s:%d] fopen Error : %s!\n", __FUNCTION__, __LINE__, strerror(errno));
		goto END;
	}

#if defined(CONFIG_SHARED_COMPILE)
	fl.l_type = F_WRLCK;
	fl.l_whence = SEEK_SET;
	fl.l_start = 0;
	fl.l_len = 0;

	if (fcntl(fd, F_SETLKW, &fl) == -1) {
		PrintError("[%s] Can't lock %s file!!\n", __FUNCTION__, szFile);
		goto END;
	}
#endif

	VK_fprintf(fp, "start		%d.%d.%d.%d\n", pstInstance->stDhcpsParams.aucStart[0],
							pstInstance->stDhcpsParams.aucStart[1],
							pstInstance->stDhcpsParams.aucStart[2],
							pstInstance->stDhcpsParams.aucStart[3]);
	VK_fprintf(fp, "end		%d.%d.%d.%d\n", pstInstance->stDhcpsParams.aucEnd[0],
							pstInstance->stDhcpsParams.aucEnd[1],
							pstInstance->stDhcpsParams.aucEnd[2],
							pstInstance->stDhcpsParams.aucEnd[3]);
	VK_fprintf(fp, "interface		%s\n", pstInstance->pstDevInfo->szName);
	VK_fprintf(fp, "pidfile		%s.%s\n", DHCPS_PID_FILE, pstInstance->pstDevInfo->szName);
	for(i=0; i<3; i++)
	{
		if(IS_VALID_IPV4_ADDR(pstInstance->stDhcpsParams.aucDns[i]))
		{
			VK_fprintf(fp, "opt	dns	%d.%d.%d.%d\n", pstInstance->stDhcpsParams.aucDns[i][0],
									pstInstance->stDhcpsParams.aucDns[i][1],
									pstInstance->stDhcpsParams.aucDns[i][2],
									pstInstance->stDhcpsParams.aucDns[i][3]);
		}
	}
	VK_fprintf(fp, "opt	subnet	%d.%d.%d.%d\n", pstInstance->stDhcpsParams.aucSubnet[0],
							pstInstance->stDhcpsParams.aucSubnet[1],
							pstInstance->stDhcpsParams.aucSubnet[2],
							pstInstance->stDhcpsParams.aucSubnet[3]);
	VK_fprintf(fp, "opt	router	%d.%d.%d.%d\n", pstInstance->stDhcpsParams.aucRouter[0],
							pstInstance->stDhcpsParams.aucRouter[1],
							pstInstance->stDhcpsParams.aucRouter[2],
							pstInstance->stDhcpsParams.aucRouter[3]);
	VK_fprintf(fp, "opt	domain	%s\n", pstInstance->stDhcpsParams.aucDomain);
	VK_fprintf(fp, "opt	lease	%d\n", pstInstance->stDhcpsParams.ulLease);

	/* start dhcp server */
	VK_snprintf(szCmdBuf, 128, "udhcpd %s.%s", DHCPS_CONF_FILE, pstInstance->pstDevInfo->szName);
	vkErr = VK_SYSTEM_Command(szCmdBuf);
	if(vkErr != 0)
	{
		PrintError("[%s:%d] VK_SYSTEM_Command Error(%d)!\n", __FUNCTION__, __LINE__, vkErr);
	}

#if 0
	/* dhcp server run in background, so return value is always success,
	   alternatively, check the pidfile */
	VK_TASK_Sleep(1000);
	VK_snprintf(szPidFile, 128, "/var/lib/humaxtv/udhcpd.%s.pid", pstInstance->pstDevInfo->szName);
	fp = fopen(szPidFile, "r");
	if(fp)
	{
		fclose(fp);
	}
	else
	{
		PrintError("[%s:%d] Dhcp Server Start Fail!\n", __FUNCTION__, __LINE__);
	}
#endif

END:
	if(NULL != fp)
		fclose(fp);

#if defined(CONFIG_SHARED_COMPILE)
	if(fd >= 0)
		close(fd);
#endif

	pstInstance->ulDhcpServerTaskId = 0;
	VK_TASK_Destroy(ulCurrentTaskId);	/* async task destroy */

	while(1)	/* wait until self task detroy */
	{
		VK_TASK_Sleep(0xFFFFFFFF);
	}
}

/*******************************************************************************
* function : P_NETWORK_StopDhcpServer
* description :
* input :
* output :
* return :
* ??Ÿ ?????ڷ? ?? ????
*******************************************************************************/
DRV_Error P_NETWORK_StopDhcpServer(HUINT32 ulDevId)
{
	HCHAR szPidFile[128], szCmdFileName[128], szPidBuf[128], szCmdBuf[128];
	HUINT32 unPid, ret;
	FILE *fp = NULL;
	FILE *cmd_fp;
	HBOOL bKillOk = FALSE;
#if defined(CONFIG_SHARED_COMPILE)
	struct flock	fl;
	int	fd;
#endif

	VK_snprintf(szPidFile, 128, "%s.%s", DHCPS_PID_FILE, s_stNetInst[ulDevId].pstDevInfo->szName);
#if defined(CONFIG_SHARED_COMPILE)
	fd = open(szPidFile, O_RDWR);
	if (fd < 0)
	{
		PrintError("[%s:%d] open rrror : %s!\n", __FUNCTION__, __LINE__, strerror(errno));
		goto END;
	}
	fp = fdopen(fd, "r");
#else
	fp = fopen(szPidFile, "r");
#endif
	if(fp)
	{
#if defined(CONFIG_SHARED_COMPILE)
		fl.l_type = F_WRLCK;
		fl.l_whence = SEEK_SET;
		fl.l_start = 0;
		fl.l_len = 0;

		if (fcntl(fd, F_SETLKW, &fl) == -1) {
			PrintError("[%s] Can't lock %s file!!\n", __FUNCTION__, szPidFile);
			goto END;
		}
#endif
		if(fgets(szPidBuf, 8, fp))
		{
			unPid = VK_atoi(szPidBuf);

			VK_snprintf(szCmdFileName, 128, "/proc/%d/cmdline", unPid);
			cmd_fp = fopen(szCmdFileName, "r");
			if(cmd_fp)
			{
				if(fgets(szCmdBuf, 128, cmd_fp))
				{
					if(VK_strstr(szCmdBuf, "udhcpd"))
					{
						kill(unPid, SIGKILL);
						bKillOk = TRUE;
					}
				}
				fclose(cmd_fp);
			}
		}
	}

END:
	if(NULL != fp)
		fclose(fp);

#if defined(CONFIG_SHARED_COMPILE)
	if(fd >= 0)
		close(fd);
#endif

	if(NULL != fp)
	{
		ret = remove(szPidFile); /* remove file */
		if(ret != 0)
		{
			PrintError("[%s] Can't delete %s file!!\n", __FUNCTION__, szPidFile);
			return DRV_ERR;
		}
	}

	if(bKillOk)
		return DRV_OK;
	else
		return DRV_ERR;

}


/*******************************************************************************
* function : P_NETWORK_StartDhcpServer
* description :
* input :
* output :
* return :
* ??Ÿ ?????ڷ? ?? ????
*******************************************************************************/
DRV_Error P_NETWORK_StartDhcpServer(HUINT32 ulDevId, DI_NETWORK_DHCPS_PARAMS_t *pstParams)
{
	int vkErr;

	if(s_stNetInst[ulDevId].ulDhcpServerTaskId)
	{
		PrintError("[%s:%d] : the server was already executed!!\n", __FUNCTION__, __LINE__);
		return DRV_ERR;
	}

	P_NETWORK_StopDhcpServer(ulDevId);

	VK_memcpy(&s_stNetInst[ulDevId].stDhcpsParams, pstParams, sizeof(DI_NETWORK_DHCPS_PARAMS_t));

	vkErr = VK_TASK_Create(P_NETWORK_ExecDhcpServerTask,
				NETWORK_TASK_PRIORITY,
				NETWORK_TASK_STACK_SIZE,
				"P_NETWORK_ExecDhcpServerTask",
				&s_stNetInst[ulDevId],
				&s_stNetInst[ulDevId].ulDhcpServerTaskId,
				TRUE);
	if(vkErr != VK_OK)
	{
		PrintError("[%s:%d] VK_TASK_Create Error!\n", __FUNCTION__, __LINE__);
		return DRV_ERR;
	}

	vkErr = VK_TASK_Start(s_stNetInst[ulDevId].ulDhcpServerTaskId);
	if(vkErr != VK_OK)
	{
		PrintError("[%s:%d] VK_TASK_Start Error!\n", __FUNCTION__, __LINE__);
		return DRV_ERR;
	}

	return DRV_OK;
}

/*******************************************************************************
* function : P_NETWORK_ExecAutoIpTask
* description :
* input :
* output :
* return :
* ??Ÿ ?????ڷ? ?? ????
*******************************************************************************/
void P_NETWORK_ExecAutoIpTask(void *pvParam)
{
	DI_NETWORK_CALLBACK_DATA_t stCallbackData;
	HCHAR szCmdBuf[128];
	int vkErr;
	HULONG ulCurrentTaskId;

	stCallbackData.ulDevId = *(HUINT32 *)pvParam;
	ulCurrentTaskId = s_stNetInst[stCallbackData.ulDevId].ulAutoIpTaskId;

	VK_snprintf(szCmdBuf, 128, "zcip %s /usr/share/zcip/zcip.script -qv", s_stNetInst[stCallbackData.ulDevId].pstDevInfo->szName);
	vkErr = VK_SYSTEM_Command(szCmdBuf);
	if(!vkErr)
	{
		stCallbackData.eEvent = DI_NETWORK_EVENT_AUTOIP_SUCCESS;
	}
	else
	{
		stCallbackData.eEvent = DI_NETWORK_EVENT_AUTOIP_FAIL;
	}

	(void)VK_MSG_SendTimeout(s_ulEventMsgQId, &stCallbackData, sizeof(DI_NETWORK_CALLBACK_DATA_t), 0);

	s_stNetInst[stCallbackData.ulDevId].ulAutoIpTaskId = 0;
	VK_TASK_Destroy(ulCurrentTaskId);	/* async task destroy */

	while(1)	/* wait until self task detroy */
	{
		VK_TASK_Sleep(0xFFFFFFFF);
	}
}

/*******************************************************************************
* function : P_NETWORK_StartAutoIp
* description :
* input :
* output :
* return :
* ??Ÿ ?????ڷ? ?? ????
*******************************************************************************/
DRV_Error P_NETWORK_StartAutoIp(HUINT32 ulDevId)
{
	int vkErr;

	if(s_stNetInst[ulDevId].ulAutoIpTaskId)
	{
		PrintError("[%s:%d] : the autoip was already executed!!\n", __FUNCTION__, __LINE__);
		return DRV_ERR;
	}

	vkErr = VK_TASK_Create(P_NETWORK_ExecAutoIpTask,
				NETWORK_TASK_PRIORITY,
				NETWORK_TASK_STACK_SIZE,
				"P_NETWORK_ExecAutoIpTask",
				&s_stNetInst[ulDevId].pstDevInfo->ulDevId,
				&s_stNetInst[ulDevId].ulAutoIpTaskId,
				TRUE);
	if(vkErr != VK_OK)
	{
		PrintError("[%s:%d] VK_TASK_Create Error!\n", __FUNCTION__, __LINE__);
		return DRV_ERR;
	}

	vkErr = VK_TASK_Start(s_stNetInst[ulDevId].ulAutoIpTaskId);
	if(vkErr != VK_OK)
	{
		PrintError("[%s:%d] VK_TASK_Start Error!\n", __FUNCTION__, __LINE__);
		return DRV_ERR;
	}

	return DRV_OK;
}

#if defined(CONFIG_IPV6)
/*******************************************************************************
* function : P_NETWORK_SetIpAddr6
* description :
* input :
* output :
* return :
* ??Ÿ ?????ڷ? ?? ????
*******************************************************************************/
DRV_Error P_NETWORK_SetIpAddr6(HUINT32 ulDevId, DI_NETWORK_IPV6_ADDR_OP_e eOp, HUINT8 *pucIpAddr, HUINT32 ulPrefix)
{
	struct ifreq ifreq;
	struct in6_ifreq ifreq6;

	VK_MEM_Memset(&ifreq, 0, sizeof(struct ifreq));

	VK_strncpy(ifreq.ifr_name, s_stNetInst[ulDevId].pstDevInfo->szName, IF_NAME_SIZE-1);
	if(ioctl(s_stNetInst[ulDevId].s_sockfd6, SIOGIFINDEX, (char *)&ifreq) < 0)
	{
		PrintError("[%s:%d] ioctl(SIOGIFINDEX) Error : %s!\n", __FUNCTION__, __LINE__, strerror(errno));
		return DRV_ERR;
	}

	VK_memcpy(&ifreq6.ifr6_addr, pucIpAddr, sizeof(struct in6_addr));
	ifreq6.ifr6_ifindex = ifreq.ifr_ifindex;
	ifreq6.ifr6_prefixlen = ulPrefix;

	if(IS_VALID_IPV6_ADDR(pucIpAddr))
	{
		if(eOp == DI_NETWORK_IPV6_ADDR_ADD)
		{
			if(ioctl(s_stNetInst[ulDevId].s_sockfd6, SIOCSIFADDR, (char *)&ifreq6) < 0)
			{
				PrintError("[%s:%d] ioctl(SIOCSIFADDR) Error : %s!\n", __FUNCTION__, __LINE__, strerror(errno));
				return DRV_ERR;
			}
		}
		else if(eOp == DI_NETWORK_IPV6_ADDR_DEL)
		{
			if(ioctl(s_stNetInst[ulDevId].s_sockfd6, SIOCDIFADDR, (char *)&ifreq6) < 0)
			{
				PrintError("[%s:%d] ioctl(SIOCDIFADDR) Error : %s!\n", __FUNCTION__, __LINE__, strerror(errno));
				return DRV_ERR;
			}
		}
	}

	return DRV_OK;
}

/*******************************************************************************
* function : P_NETWORK_GetIpAddr6
* description :
* input :
* output :
* return :
* ??Ÿ ?????ڷ? ?? ????
*******************************************************************************/
DRV_Error P_NETWORK_GetIpAddr6(HUINT32 ulDevId, DI_NETWORK_IPV6_ADDR_SCOPE_e eScope, HUINT8 *pucIpAddr, HUINT32 *pulPrefix)
{
	FILE* fp = NULL;
	HUINT32 aulAddr6[IPV6_ADDR_LEN];
	HUINT8 aucDevName[IF_NAME_SIZE];
	HUINT32 ulPrefix, ulScope, ulDadStatus, ulIfIdx;
	int i;
	DRV_Error eDrvErr = DRV_OK;

	fp = fopen("/proc/net/if_inet6", "r");
	if (!fp)
	{
		PrintError("[%s:%d] fopen Error : %s!\n", __FUNCTION__, __LINE__, strerror(errno));
		eDrvErr = DI_ERR_ERROR;
		goto END;
	}

	while(fscanf(fp, "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x %08x %02x %02x %02x %15s",
			&aulAddr6[0], &aulAddr6[1], &aulAddr6[2], &aulAddr6[3],
			&aulAddr6[4], &aulAddr6[5], &aulAddr6[6], &aulAddr6[7],
			&aulAddr6[8], &aulAddr6[9], &aulAddr6[10], &aulAddr6[11],
			&aulAddr6[12], &aulAddr6[13], &aulAddr6[14], &aulAddr6[15],
			&ulIfIdx, &ulPrefix, &ulScope, &ulDadStatus, aucDevName) != EOF)
	{
		if(!VK_strcmp(aucDevName, s_stNetInst[ulDevId].pstDevInfo->szName)
			&& ((ulScope&IPV6_ADDR_SCOPE_MASK)==eScope))
		{
			for(i=0; i<IPV6_ADDR_LEN; i++)
			{
				pucIpAddr[i] = (HUINT8)aulAddr6[i];
			}
			*pulPrefix = ulPrefix;
			goto END;
		}
	}

END:
	if(NULL != fp)
	fclose(fp);

	return eDrvErr;
}

/*******************************************************************************
* function : P_NETWORK_ExecDhcp6ClientTask
* description :
* input :
* output :
* return :
* ??Ÿ ?????ڷ? ?? ????
*******************************************************************************/
void P_NETWORK_ExecDhcp6ClientTask(void *pvParam)
{
	P_NETWORK_INSTANCE_t *pstInstance;
	DI_NETWORK_CALLBACK_DATA_t stCallbackData;
	HCHAR szCmdBuf[256];
	HCHAR szPidFile[128];
	HUINT8 aucIpAddr[IPV6_ADDR_LEN];
	HUINT32 ulPrefix;
	int vkErr;
	HULONG ulCurrentTaskId;
	DRV_Error eDrvErr;

	pstInstance = (P_NETWORK_INSTANCE_t *)pvParam;
	stCallbackData.ulDevId = pstInstance->pstDevInfo->ulDevId;
	stCallbackData.eEvent = DI_NETWORK_EVENT_DHCP6_FAIL;
	ulCurrentTaskId = pstInstance->ulDhcp6ClientTaskId;

	if(pstInstance->stDhcpcParams.eMode == DI_NETWORK_IPV6_DHCP_STATEFUL)
	{
		/* disable auto-configuration */
		VK_snprintf(szCmdBuf, 128, "echo 0 > /proc/sys/net/ipv6/conf/%s/accept_ra",pstInstance->pstDevInfo->szName);
		vkErr = VK_SYSTEM_Command(szCmdBuf);
		if(vkErr != 0)
		{
			PrintError("[%s:%d] VK_SYSTEM_Command Error(%d)!\n", __FUNCTION__, __LINE__, vkErr);
		}
		VK_snprintf(szCmdBuf, 128, "echo 0 > /proc/sys/net/ipv6/conf/%s/autoconf",pstInstance->pstDevInfo->szName);
		vkErr = VK_SYSTEM_Command(szCmdBuf);
		if(vkErr != 0)
		{
			PrintError("[%s:%d] VK_SYSTEM_Command Error(%d)!\n", __FUNCTION__, __LINE__, vkErr);
		}

		/* remove a old address */
		VK_MEM_Memset(aucIpAddr, 0, IPV6_ADDR_LEN);
		P_NETWORK_GetIpAddr6(pstInstance->pstDevInfo->ulDevId, DI_NETWORK_IPV6_ADDR_GLOBAL, aucIpAddr, &ulPrefix);
		if(IS_VALID_IPV6_ADDR(aucIpAddr))
		{
			eDrvErr = P_NETWORK_SetIpAddr6(pstInstance->pstDevInfo->ulDevId, DI_NETWORK_IPV6_ADDR_DEL, aucIpAddr, ulPrefix);
			if(eDrvErr != DRV_OK)
			{
				PrintError("[%s:%d] P_NETWORK_SetIpAddr6 Error!\n", __FUNCTION__, __LINE__);
			}
		}
	}
	else
	{
		/* enable auto-configuration */
		VK_snprintf(szCmdBuf, 128, "echo 1 > /proc/sys/net/ipv6/conf/%s/accept_ra",pstInstance->pstDevInfo->szName);
		vkErr = VK_SYSTEM_Command(szCmdBuf);
		if(vkErr != 0)
		{
			PrintError("[%s:%d] VK_SYSTEM_Command Error(%d)!\n", __FUNCTION__, __LINE__, vkErr);
		}
		VK_snprintf(szCmdBuf, 128, "echo 1 > /proc/sys/net/ipv6/conf/%s/autoconf",pstInstance->pstDevInfo->szName);
		vkErr = VK_SYSTEM_Command(szCmdBuf);
		if(vkErr != 0)
		{
			PrintError("[%s:%d] VK_SYSTEM_Command Error(%d)!\n", __FUNCTION__, __LINE__, vkErr);
		}
	}

	VK_snprintf(szPidFile, 128, "%s.%s", DHCP6C_PID_FILE, pstInstance->pstDevInfo->szName);

	if(pstInstance->stDhcpcParams.eMode == DI_NETWORK_IPV6_DHCP_STATEFUL)
	{
		VK_snprintf(szCmdBuf, 256, "dhclient -6 -nw -cf %s -lf %s -pf %s %s", DHCP6C_CONF_FILE, DHCP6C_LEASES_FILE,
									szPidFile, pstInstance->pstDevInfo->szName);
	}
	else
	{
		VK_snprintf(szCmdBuf, 256, "dhclient -6 -S -nw -cf %s -lf %s -pf %s %s", DHCP6C_CONF_FILE, DHCP6C_LEASES_FILE,
									szPidFile, pstInstance->pstDevInfo->szName);
	}

	vkErr = VK_SYSTEM_Command(szCmdBuf);
	if(vkErr != 0)
	{
		PrintError("[%s:%d] VK_SYSTEM_Command Error(%d)!\n", __FUNCTION__, __LINE__, vkErr);
	}

	/* check ip addr */
	VK_MEM_Memset(aucIpAddr, 0, IPV6_ADDR_LEN);
	while(pstInstance->stDhcpcParams.ulWaitSeconds > 0)
	{
		P_NETWORK_GetIpAddr6(pstInstance->pstDevInfo->ulDevId, DI_NETWORK_IPV6_ADDR_GLOBAL, aucIpAddr, &ulPrefix);
		if(IS_VALID_IPV6_ADDR(aucIpAddr))
		{
			stCallbackData.eEvent = DI_NETWORK_EVENT_DHCP6_SUCCESS;
			P_NETWORK_InitResolver();
			break;
		}
		VK_TASK_Sleep(1000);
		pstInstance->stDhcpcParams.ulWaitSeconds--;
	}
	(void)VK_MSG_SendTimeout(s_ulEventMsgQId, &stCallbackData, sizeof(DI_NETWORK_CALLBACK_DATA_t), 0);

	pstInstance->ulDhcp6ClientTaskId = 0;
	VK_TASK_Destroy(ulCurrentTaskId);	/* async task destroy */

	while(1)	/* wait until self task detroy */
	{
		VK_TASK_Sleep(0xFFFFFFFF);
	}
}

/*******************************************************************************
* function : P_NETWORK_StopDhcp6Client
* description :
* input :
* output :
* return :
* ??Ÿ ?????ڷ? ?? ????
*******************************************************************************/
DRV_Error P_NETWORK_StopDhcp6Client(HUINT32 ulDevId)
{
	HCHAR szPidFile[128], szCmdFileName[128], szPidBuf[128], szCmdBuf[128];
	HUINT32 unPid, ret;
	FILE *fp = NULL;
	FILE *cmd_fp;
	HBOOL bKillOk = FALSE;
#if defined(CONFIG_SHARED_COMPILE)
	struct flock	fl;
	int	fd;
#endif

	VK_snprintf(szPidFile, 128, "%s.%s", DHCP6C_PID_FILE, s_stNetInst[ulDevId].pstDevInfo->szName);
#if defined(CONFIG_SHARED_COMPILE)
	fd = open(szPidFile, O_RDWR);
	if (fd < 0)
	{
		PrintError("[%s:%d] open rrror : %s!\n", __FUNCTION__, __LINE__, strerror(errno));
		goto END;
	}
	fp = fdopen(fd, "r");
#else
	fp = fopen(szPidFile, "r");
#endif
	if(fp)
	{
#if defined(CONFIG_SHARED_COMPILE)
		fl.l_type = F_WRLCK;
		fl.l_whence = SEEK_SET;
		fl.l_start = 0;
		fl.l_len = 0;

		if (fcntl(fd, F_SETLKW, &fl) == -1) {
			PrintError("[%s] Can't lock %s file!!\n", __FUNCTION__, szPidFile);
			goto END;
		}
#endif
		if(fgets(szPidBuf, 8, fp))
		{
			unPid = VK_atoi(szPidBuf);

			VK_snprintf(szCmdFileName, 128, "/proc/%d/cmdline", unPid);
			cmd_fp = fopen(szCmdFileName, "r");
			if(cmd_fp)
			{
				if(fgets(szCmdBuf, 128, cmd_fp))
				{
					if(VK_strstr(szCmdBuf, "dhclient"))
					{
						kill(unPid, SIGKILL);
						bKillOk = TRUE;
					}
				}
				fclose(cmd_fp);
			}
		}
	}

END:
	if(NULL != fp)
		fclose(fp);

#if defined(CONFIG_SHARED_COMPILE)
	if(fd >= 0)
		close(fd);
#endif

	if(NULL != fp)
	{
		ret = remove(szPidFile); /* remove file */
		if(ret != 0)
		{
			PrintError("[%s] Can't delete %s file!!\n", __FUNCTION__, szPidFile);
			return DRV_ERR;
		}
	}

	if(bKillOk)
		return DRV_OK;
	else
		return DRV_ERR;

}

/*******************************************************************************
* function : P_NETWORK_StartDhcp6Client
* description :
* input :
* output :
* return :
* ??Ÿ ?????ڷ? ?? ????
*******************************************************************************/
DRV_Error P_NETWORK_StartDhcp6Client(HUINT32 ulDevId, DI_NETWORK_DHCPC_PARAMS_t *pstParams)
{
	int vkErr;

	if(s_stNetInst[ulDevId].ulDhcp6ClientTaskId)
	{
		PrintError("[%s:%d] : the client was already executed!!\n", __FUNCTION__, __LINE__);
		return DRV_ERR;
	}

	P_NETWORK_StopDhcp6Client(ulDevId);

	VK_memcpy(&s_stNetInst[ulDevId].stDhcpcParams, pstParams, sizeof(DI_NETWORK_DHCPC_PARAMS_t));

	vkErr = VK_TASK_Create(P_NETWORK_ExecDhcp6ClientTask,
				NETWORK_TASK_PRIORITY,
				NETWORK_TASK_STACK_SIZE,
				"P_NETWORK_ExecDhcp6ClientTask",
				&s_stNetInst[ulDevId],
				&s_stNetInst[ulDevId].ulDhcp6ClientTaskId,
				TRUE);
	if(vkErr != VK_OK)
	{
		PrintError("[%s:%d] VK_TASK_Create Error!\n", __FUNCTION__, __LINE__);
		return DRV_ERR;
	}

	vkErr = VK_TASK_Start(s_stNetInst[ulDevId].ulDhcp6ClientTaskId);
	if(vkErr != VK_OK)
	{
		PrintError("[%s:%d] VK_TASK_Start Error!\n", __FUNCTION__, __LINE__);
		return DRV_ERR;
	}

	return DRV_OK;
}

/*******************************************************************************
* function : P_NETWORK_ExecDhcp6ServerTask
* description :
* input :
* output :
* return :
* ??Ÿ ?????ڷ? ?? ????
*******************************************************************************/
void P_NETWORK_ExecDhcp6ServerTask(void *pvParam)
{
	P_NETWORK_INSTANCE_t *pstInstance;
	HCHAR szCmdBuf[256], szFile[128], szAddr6Str[40];
	FILE *fp = NULL;
	HCHAR szPidFile[128];
	int vkErr;
	HULONG ulCurrentTaskId;
#if defined(CONFIG_SHARED_COMPILE)
	struct flock	fl;
	int	fd;
#endif
	pstInstance = (P_NETWORK_INSTANCE_t *)pvParam;
	ulCurrentTaskId = pstInstance->ulDhcp6ServerTaskId;

	VK_snprintf(szPidFile, 128, "%s.%s", DHCP6S_PID_FILE, pstInstance->pstDevInfo->szName);

	/* create new dhcp server configuration file */
	VK_snprintf(szFile, 128, "%s.%s", DHCP6S_CONF_FILE, pstInstance->pstDevInfo->szName);

#if defined(CONFIG_SHARED_COMPILE)
	fd = open(szFile, O_CREAT | O_RDWR);
	if (fd < 0)
	{
		PrintError("[%s:%d] open rrror : %s!\n", __FUNCTION__, __LINE__, strerror(errno));
		goto END;
	}
	fp = fdopen(fd, "w");
#else
	fp = fopen(szFile, "w");
#endif

	if(fp == NULL)
	{
		PrintError("[%s:%d] fopen Error : %s!\n", __FUNCTION__, __LINE__, strerror(errno));
		goto END;
	}

#if defined(CONFIG_SHARED_COMPILE)
	fl.l_type = F_WRLCK;
	fl.l_whence = SEEK_SET;
	fl.l_start = 0;
	fl.l_len = 0;

	if (fcntl(fd, F_SETLKW, &fl) == -1) {
		PrintError("[%s] Can't lock %s file!!\n", __FUNCTION__, szFile);
		goto END;
	}
#endif

	VK_fprintf(fp, "default-lease-time 	%d;\n", pstInstance->stDhcpsParams.ulLease);
	VK_fprintf(fp, "preferred-lifetime 	%d;\n", DHCP6S_DEFAULT_LIFE_TIME);
	VK_fprintf(fp, "option dhcp-renewal-time 	%d;\n", DHCP6S_DEFAULT_RENEWAL_TIME);
	VK_fprintf(fp, "option dhcp-rebinding-time %d;\n", DHCP6S_DEFAULT_REBINDING_TIME);
	VK_fprintf(fp, "option dhcp6.info-refresh-time %d;\n", DHCP6S_DEFAULT_INFOREFRESH_TIME);
	VK_fprintf(fp, "allow leasequery;\n"); 	/* RFC 5007 */
	inet_ntop(AF_INET6, (void *)pstInstance->stDhcpsParams.aucIpv6Dns, szAddr6Str, sizeof(szAddr6Str));
	VK_fprintf(fp, "option dhcp6.name-servers	%s;\n", szAddr6Str);
	VK_fprintf(fp, "option dhcp6.domain-search 	\"%s\";\n", pstInstance->stDhcpsParams.aucDomain);
	inet_ntop(AF_INET6, (void *)pstInstance->stDhcpsParams.aucIpv6Subnet, szAddr6Str, sizeof(szAddr6Str));
	VK_fprintf(fp, "subnet6 %s/%d {\n", szAddr6Str, pstInstance->stDhcpsParams.ulIpv6Prefix);
	inet_ntop(AF_INET6, (void *)pstInstance->stDhcpsParams.aucIpv6Start, szAddr6Str, sizeof(szAddr6Str));
	VK_fprintf(fp, "	range6 %s ", szAddr6Str);
	inet_ntop(AF_INET6, (void *)pstInstance->stDhcpsParams.aucIpv6End, szAddr6Str, sizeof(szAddr6Str));
	VK_fprintf(fp, "%s;\n", szAddr6Str);
	VK_fprintf(fp, "}\n");

	VK_snprintf(szCmdBuf, 256, "dhcpd -6 -cf %s.%s -lf %s -pf %s %s", DHCP6S_CONF_FILE,
									pstInstance->pstDevInfo->szName,
									DHCP6S_LEASES_FILE,
									szPidFile,
									pstInstance->pstDevInfo->szName);

	vkErr = VK_SYSTEM_Command(szCmdBuf);
	if(vkErr != 0)
	{
		PrintError("[%s:%d] VK_SYSTEM_Command Error(%d)!\n", __FUNCTION__, __LINE__, vkErr);
	}

END:
	if(NULL != fp)
		fclose(fp);

#if defined(CONFIG_SHARED_COMPILE)
	if(fd >= 0)
		close(fd);
#endif

	pstInstance->ulDhcp6ServerTaskId = 0;
	VK_TASK_Destroy(ulCurrentTaskId);	/* async task destroy */

	while(1)	/* wait until self task detroy */
	{
		VK_TASK_Sleep(0xFFFFFFFF);
	}
}

/*******************************************************************************
* function : P_NETWORK_StopDhcp6Server
* description :
* input :
* output :
* return :
* ??Ÿ ?????ڷ? ?? ????
*******************************************************************************/
DRV_Error P_NETWORK_StopDhcp6Server(HUINT32 ulDevId)
{
	HCHAR szPidFile[128], szCmdFileName[128], szPidBuf[128], szCmdBuf[128];
	HUINT32 unPid, ret;
	FILE *fp = NULL;
	FILE *cmd_fp;
	HBOOL bKillOk = FALSE;
#if defined(CONFIG_SHARED_COMPILE)
	struct flock	fl;
	int	fd;
#endif

	VK_snprintf(szPidFile, 128, "%s.%s", DHCP6S_PID_FILE, s_stNetInst[ulDevId].pstDevInfo->szName);
#if defined(CONFIG_SHARED_COMPILE)
	fd = open(szPidFile, O_RDWR);
	if (fd < 0)
	{
		PrintError("[%s:%d] open rrror : %s!\n", __FUNCTION__, __LINE__, strerror(errno));
		goto END;
	}
	fp = fdopen(fd, "r");
#else
	fp = fopen(szPidFile, "r");
#endif
	if(fp)
	{
#if defined(CONFIG_SHARED_COMPILE)
		fl.l_type = F_WRLCK;
		fl.l_whence = SEEK_SET;
		fl.l_start = 0;
		fl.l_len = 0;

		if (fcntl(fd, F_SETLKW, &fl) == -1) {
			PrintError("[%s] Can't lock %s file!!\n", __FUNCTION__, szPidFile);
			goto END;
		}
#endif
		if(fgets(szPidBuf, 8, fp))
		{
			unPid = VK_atoi(szPidBuf);

			VK_snprintf(szCmdFileName, 128, "/proc/%d/cmdline", unPid);
			cmd_fp = fopen(szCmdFileName, "r");
			if(cmd_fp)
			{
				if(fgets(szCmdBuf, 128, cmd_fp))
				{
					if(VK_strstr(szCmdBuf, "dhcpd"))
					{
						kill(unPid, SIGKILL);
						bKillOk = TRUE;
					}
				}
				fclose(cmd_fp);
			}
		}
	}

END:
	if(NULL != fp)
		fclose(fp);

#if defined(CONFIG_SHARED_COMPILE)
	if(fd >= 0)
		close(fd);
#endif

	if(NULL != fp)
	{
		ret = remove(szPidFile); /* remove file */
		if(ret != 0)
		{
			PrintError("[%s] Can't delete %s file!!\n", __FUNCTION__, szPidFile);
			return DRV_ERR;
		}
	}

	if(bKillOk)
		return DRV_OK;
	else
		return DRV_ERR;

}

/*******************************************************************************
* function : P_NETWORK_StartDhcp6Server
* description :
* input :
* output :
* return :
* ??Ÿ ?????ڷ? ?? ????
*******************************************************************************/
DRV_Error P_NETWORK_StartDhcp6Server(HUINT32 ulDevId, DI_NETWORK_DHCPS_PARAMS_t *pstParams)
{
	int vkErr;

	if(s_stNetInst[ulDevId].ulDhcp6ServerTaskId)
	{
		PrintError("[%s:%d] : the server was already executed!!\n", __FUNCTION__, __LINE__);
		return DRV_ERR;
	}

	P_NETWORK_StopDhcp6Server(ulDevId);

	VK_memcpy(&s_stNetInst[ulDevId].stDhcpsParams, pstParams, sizeof(DI_NETWORK_DHCPS_PARAMS_t));

	vkErr = VK_TASK_Create(P_NETWORK_ExecDhcp6ServerTask,
				NETWORK_TASK_PRIORITY,
				NETWORK_TASK_STACK_SIZE,
				"P_NETWORK_ExecDhcp6ClientTask",
				&s_stNetInst[ulDevId],
				&s_stNetInst[ulDevId].ulDhcp6ServerTaskId,
				TRUE);
	if(vkErr != VK_OK)
	{
		PrintError("[%s:%d] VK_TASK_Create Error!\n", __FUNCTION__, __LINE__);
		return DRV_ERR;
	}

	vkErr = VK_TASK_Start(s_stNetInst[ulDevId].ulDhcp6ServerTaskId);
	if(vkErr != VK_OK)
	{
		PrintError("[%s:%d] VK_TASK_Start Error!\n", __FUNCTION__, __LINE__);
		return DRV_ERR;
	}

	return DRV_OK;
}
#endif


/*******************************************************************************
* function : P_NETWORK_LinkCheckTimer
* description :
* input :
* output :
* return :
* ??Ÿ ?????ڷ? ?? ????
*******************************************************************************/
void P_NETWORK_LinkCheckTask(void *pvParam)
{
	HUINT32 i;
	HBOOL bState;
	DI_NETWORK_CALLBACK_DATA_t stCallbackData;

	UNREFENCED_PARAM(pvParam);

	while(1)
	{
		for(i=0; i<s_ulNetDevNum; i++)
		{
			VK_SEM_Get(s_stNetInst[i].s_ulNetSemId);
			if(s_stNetInst[i].pstDevInfo->eDevType != DI_NETWORK_DEV_WIFI
				&& s_stNetInst[i].pstDevInfo->eDevType != DI_NETWORK_DEV_PPPOE)
			{
				bState = s_stNetInst[i].bConnected;
				P_NETWORK_GetDevLinkStatus(i, &bState);
				if(s_stNetInst[i].bConnected != bState)
				{
					s_stNetInst[i].ulLinkCnt++;
				}
				else
				{
					s_stNetInst[i].ulLinkCnt = 0;
				}

				if(s_stNetInst[i].ulLinkCnt >= LINK_CHECK_COUNT)
				{
					s_stNetInst[i].ulLinkCnt = 0;
					s_stNetInst[i].bConnected = bState;
					if(bState)
					{
						stCallbackData.eEvent = DI_NETWORK_EVENT_CABLE_CONNECT;
					}
					else
					{
						stCallbackData.eEvent = DI_NETWORK_EVENT_CABLE_DISCONNECT;
					}
					stCallbackData.ulDevId = i;
					(void)VK_MSG_SendTimeout(s_ulEventMsgQId, &stCallbackData, sizeof(DI_NETWORK_CALLBACK_DATA_t), 0);
				}

				if(s_ulNetDevNum > 1)
				{
					VK_TASK_Sleep(50);
				}

			}
			VK_SEM_Release(s_stNetInst[i].s_ulNetSemId);
		}
		VK_TASK_Sleep(900);
	}
}
/*******************************************************************************
* function : P_NETWORK_EventNotifyTask
* description :
* input :
* output :
* return :
* ??Ÿ ?????ڷ? ?? ????
*******************************************************************************/
void P_NETWORK_EventNotifyTask(void *pvParam)
{
	VK_ERR_CODE eVkErr;
	DI_NETWORK_CALLBACK_DATA_t stCallbackData;

	UNREFENCED_PARAM(pvParam);

	while(1)
	{
		eVkErr = VK_MSG_Receive(s_ulEventMsgQId, &stCallbackData, sizeof(DI_NETWORK_CALLBACK_DATA_t));
		if (eVkErr != VK_OK)
		{
			PrintError("[%s:%d] VK_MSG_Receive Error!\n", __FUNCTION__, __LINE__);
			continue;
		}

		if(stCallbackData.ulDevId >= s_ulNetDevNum)
		{
			PrintError("[%s:%d] Invalid Device ID!\n", __FUNCTION__, __LINE__);
			continue;
		}

		if(stCallbackData.eEvent >= DI_NETWORK_EVENT_MAX)
		{
			PrintError("[%s:%d] Invalid Event!\n", __FUNCTION__, __LINE__);
			continue;
		}

		if(s_stNetInst[stCallbackData.ulDevId].pfnEventCallback[stCallbackData.eEvent])
			s_stNetInst[stCallbackData.ulDevId].pfnEventCallback[stCallbackData.eEvent](&stCallbackData);

	}
}


/*******************************************************************************
* function : P_NETWORK_CheckDevList
* description :
* input :
* output :
* return :
* ??Ÿ ?????ڷ? ?? ????
*******************************************************************************/
#if 0
DRV_Error P_NETWORK_CheckDevList(void)
{
	HUINT32 i=0;
	char buff[1024] = {0, };
	FILE *inFile = NULL;
	HUINT32 ulDevCnt = 0;

	inFile = fopen(PROC_NET_DEV_INFO, "r");
	if(inFile == NULL)
	{
		PrintError("[%s:%d] fopen Error : %s!\n", __FUNCTION__, __LINE__, strerror(errno));
		return DRV_ERR;
	}

	/* Success : use data from /proc/net/wireless */

	/* Eat 2 lines of header */
	fgets(buff, sizeof(buff), inFile);
	fgets(buff, sizeof(buff), inFile);

	while(fgets(buff, sizeof(buff), inFile) != NULL)
	{
		char *start = buff;
		char *end;

		DI_UART_Print("%s\n", buff);
		/* Skip empty or almost empty lines. It seems that in some
		* cases fgets return a line with only a newline. */
		if((start[0] == '\0') || (start[1] == '\0'))
		{
			PrintError("[%s:%d]\n", __FUNCTION__,__LINE__);
			continue;
		}

		/* Skip leading spaces */
		while(isspace(*start))
		{
			start++;
		}

		/* Get name up to ": "
		* Note : we compare to ": " to make sure to process aliased interfaces
		* properly. Doesn't work on /proc/net/dev, because it doesn't guarantee
		* a ' ' after the ':'*/
		end = strrchr(start, ':');

		/* Not found ??? To big ??? */
		if((end == NULL) || (((end - start) + 1) > IF_NAME_SIZE + 1))
		{
			PrintError("[%s:%d]\n", __FUNCTION__,__LINE__);
			continue;
		}

		for(i=0; i<s_ulNetDevNum; i++)
		{
			if(!VK_strncmp(s_stDevList[i].szName, start, 4))
			{
				DI_UART_Print("macthed dev=%s\n", s_stDevList[i].szName);
				ulDevCnt++;
			}
		}
	}

	fclose(inFile);

	if(ulDevCnt == s_ulNetDevNum)
		return DRV_OK;
	else
		return DRV_ERR;

	return DRV_OK;
}
#endif

/*******************************************************************************
* function : DRV_NETWORK_SetMacAddr
* description :
* input :
* output :
* return :
* ??Ÿ ?????ڷ? ?? ????
*******************************************************************************/
DRV_Error DRV_NETWORK_SetMacAddr(HUINT32 ulDevId, HUINT8 *pucMacAddr)
{
	DRV_Error eDrvErr;
	HBOOL bIsUp, bIsDynamic;

	/* get dev status */
	eDrvErr = P_NETWORK_GetDevStatus(ulDevId, &bIsUp, &bIsDynamic);
	if(eDrvErr != DRV_OK)
	{
		PrintError("[%s:%d] P_NETWORK_GetDevStatus Error!\n", __FUNCTION__, __LINE__);
		return DRV_ERR;
	}

	/* if dev status is up, setting mac addr is failed */
	if(bIsUp)
	{
		eDrvErr = P_NETWORK_SetDevStatus(ulDevId, FALSE, bIsDynamic);
		if(eDrvErr != DRV_OK)
		{
			PrintError("[%s:%d] P_NETWORK_SetDevStatus Error!\n", __FUNCTION__, __LINE__);
			return DRV_ERR;
		}
	}

	/* set the mac addr */
	eDrvErr = P_NETWORK_SetMacAddr(ulDevId, pucMacAddr);
	if(eDrvErr != DRV_OK)
	{
		PrintError("[%s:%d] P_NETWORK_SetMacAddr Error!\n", __FUNCTION__, __LINE__);
		return DRV_ERR;
	}

	/* restore the dev status */
	if(bIsUp)
	{
		eDrvErr = P_NETWORK_SetDevStatus(ulDevId, TRUE, bIsDynamic);
		if(eDrvErr != DRV_OK)
		{
			PrintError("[%s:%d] P_NETWORK_SetDevStatus Error!\n", __FUNCTION__, __LINE__);
			return DRV_ERR;
		}
	}

	return DRV_OK;
}

/*******************************************************************************
* function : DRV_NETWORK_Init
* description :
* input :
* output :
* return :
* ??Ÿ ?????ڷ? ?? ????
*******************************************************************************/
DRV_Error DRV_NETWORK_Init(void)
{
	VK_ERR_CODE eVkErr;
	HUINT32 i;
	HINT32 res;
	DRV_Error eDrvErr;
#if !defined(CONFIG_SHARED_COMPILE)
	HUINT8 aucMacAddr[MAC_ADDR_LEN];
#endif
	char szSemName[20];
#if defined(CONFIG_IPV6)
	char szCmdBuf[128] = {0,};
	FILE *fp;
#endif

	/* get number of devices */
	s_ulNetDevNum = DRV_CFG_GetNetMaxDevice();

	/* malloc for network instance */
	s_stNetInst = (P_NETWORK_INSTANCE_t *)VK_MEM_Alloc(sizeof(P_NETWORK_INSTANCE_t)*s_ulNetDevNum);
	VK_MEM_Memset(s_stNetInst, 0, sizeof(P_NETWORK_INSTANCE_t)*s_ulNetDevNum);

	/* initialize the network instance */
	for(i=0; i<s_ulNetDevNum; i++)
	{
		/* open socket interface */
		s_stNetInst[i].s_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
		if(s_stNetInst[i].s_sockfd < 0)
		{
			PrintError("[%s:%d] socket Error : %s!\n", __FUNCTION__, __LINE__, strerror(errno));
			return DRV_ERR;
		}

#if defined(CONFIG_IPV6)
		s_stNetInst[i].s_sockfd6 = socket(AF_INET6, SOCK_DGRAM, 0);
		if(s_stNetInst[i].s_sockfd6 < 0)
		{
			PrintError("[%s:%d] socket Error : %s!\n", __FUNCTION__, __LINE__, strerror(errno));
			return DRV_ERR;
		}
#endif

		s_stNetInst[i].pstDevInfo = &s_stDevList[i];
		if(s_stNetInst[i].pstDevInfo->eDevType != DI_NETWORK_DEV_WIFI
			&& s_stNetInst[i].pstDevInfo->eDevType != DI_NETWORK_DEV_PPPOE)
		{
			/* get link status */
			P_NETWORK_GetDevLinkStatus(i, &s_stNetInst[i].bConnected);
		}

#if defined(CONFIG_IPV6)
		/* disable auto-configuration */
		VK_snprintf(szCmdBuf, 128, "echo 0 > /proc/sys/net/ipv6/conf/%s/accept_ra",s_stNetInst[i].pstDevInfo->szName);
		VK_SYSTEM_Command(szCmdBuf);
		VK_snprintf(szCmdBuf, 128, "echo 0 > /proc/sys/net/ipv6/conf/%s/autoconf",s_stNetInst[i].pstDevInfo->szName);
		VK_SYSTEM_Command(szCmdBuf);
#endif

#if !defined(CONFIG_SHARED_COMPILE)
		/*
		 * set mac address except eth0 and cm
		 * eth0 and cm mac are set by kernel
		 */
		if(i!=0 && s_stNetInst[i].pstDevInfo->eDevType != DI_NETWORK_DEV_CABLEMODEM
			&& s_stNetInst[i].pstDevInfo->eDevType != DI_NETWORK_DEV_WIFI
			&& s_stNetInst[i].pstDevInfo->eDevType != DI_NETWORK_DEV_PPPOE)
		{
			eDrvErr = DRV_NETWORK_ReadMacAddrFromNvram(i, aucMacAddr);
			if(eDrvErr != DRV_OK)
			{
				PrintError("[%s:%d] DRV_NETWORK_ReadMacAddrFromNvram Error!\n", __FUNCTION__, __LINE__);
				return DRV_ERR;
			}
			if(IS_VALID_MAC_ADDR(aucMacAddr))
			{
				eDrvErr = DRV_NETWORK_SetMacAddr(i, aucMacAddr);
				if(eDrvErr != DRV_OK)
				{
					PrintError("[%s:%d] DRV_NETWORK_SetMacAddr Error!\n", __FUNCTION__, __LINE__);
					return DRV_ERR;
				}
			}
			else
			{
				PrintError("[%s:%d] Dev[%d] has invalid mac address. use default.\n", __FUNCTION__, __LINE__, i);
			}
		}
#endif

		/* create network device semaphore */
		VK_snprintf(szSemName, 12, "NetDevSem%d", i);
		eVkErr = VK_SEM_Create((unsigned long*)&s_stNetInst[i].s_ulNetSemId, szSemName, VK_SUSPENDTYPE_FIFO);
		if(eVkErr != VK_OK)
		{
			PrintError("[%s:%d] VK_SEM_Create Error!\n", __FUNCTION__, __LINE__);
			return DRV_ERR;
		}
	}

	/* create network device semaphore */
	eVkErr = VK_SEM_Create((unsigned long*)&s_ulNetModSemId, "NetModSem", VK_SUSPENDTYPE_FIFO);
	if(eVkErr != VK_OK)
	{
		PrintError("[%s:%d] VK_SEM_Create Error!\n", __FUNCTION__, __LINE__);
		return DRV_ERR;
	}

	/* create event msg */
	eVkErr = VK_MSG_Create(NETWORK_MESSAGE_QUEUE_SIZE, sizeof(DI_NETWORK_CALLBACK_DATA_t), "NetEventMsgQ", (unsigned long *)&s_ulEventMsgQId, VK_SUSPENDTYPE_FIFO);
	if(eVkErr != VK_OK)
	{
		PrintError("[%s:%d] VK_MSG_Create Error!\n", __FUNCTION__, __LINE__);
		return DRV_ERR;
	}

	/* create event notify task */
	eVkErr = VK_TASK_Create(P_NETWORK_EventNotifyTask, NETWORK_TASK_PRIORITY, NETWORK_TASK_STACK_SIZE, "NetEventNotifyTask", (void *)0, (unsigned long *)&s_ulEventNotifyTaskId, FALSE);
	if(eVkErr != VK_OK)
	{
		PrintError("[%s:%d] VK_TASK_Create Error!\n", __FUNCTION__, __LINE__);
		return DRV_ERR;
	}
	VK_TASK_Start(s_ulEventNotifyTaskId);
	/* create task for link check */
	eVkErr = VK_TASK_Create(P_NETWORK_LinkCheckTask, NETWORK_TASK_PRIORITY, NETWORK_TASK_STACK_SIZE, "LinkCheckTask", (void *)0, (unsigned long *)&s_ulLinkCheckTaskId, FALSE);
	if(eVkErr != VK_OK)
	{
		PrintError("[%s:%d] VK_TASK_Create Error!\n", __FUNCTION__, __LINE__);
		return DRV_ERR;
	}
	VK_TASK_Start(s_ulLinkCheckTaskId);

	/* Create FIFO to get result of udhcpc */
	res = mkfifo(DHCP_FIFO, S_IRUSR | S_IWUSR);
	if(res < 0)
	{
		PrintError("[%s:%d] mkfifo error\n", __FUNCTION__, __LINE__);
		return DRV_ERR;
	}

	eDrvErr = P_NETWORK_MakeDhcpcScript(DHCPC_SCRIPT_PATH);
	if(eDrvErr != DRV_OK)
	{
		PrintError("[%s:%d] P_NETWORK_MakeDhcpcScript \n", __FUNCTION__, __LINE__);
		return DRV_ERR;
	}

	/* To get result, listen to FIFO */
	eVkErr = VK_TASK_Create(P_NETWORK_CheckDhcpResultTask,
				NETWORK_TASK_PRIORITY,
				NETWORK_TASK_STACK_SIZE,
				"P_NETWORK_CheckDhcpResultTask",
				NULL,
				&s_ulCheckDhcpResultTaskId,
				TRUE);
	if(eVkErr != VK_OK)
	{
		PrintError("[%s:%d] VK_TASK_Create Error!\n", __FUNCTION__, __LINE__);
		return DRV_ERR;
	}

	eVkErr = VK_TASK_Start(s_ulCheckDhcpResultTaskId);
	if(eVkErr != VK_OK)
	{
		PrintError("[%s:%d] VK_TASK_Start Error!\n", __FUNCTION__, __LINE__);
		return DRV_ERR;
	}

#if defined(CONFIG_IPV6)
	/* create dhcp leases files */
	fp = fopen(DHCP6C_LEASES_FILE, "w");
	if(fp != NULL)
	{
		fclose(fp);
	}
	fp = fopen(DHCP6S_LEASES_FILE, "w");
	if(fp != NULL)
	{
		fclose(fp);
	}
#endif
#if defined(CONFIG_TEMP_HMX4KJP) && defined(CONFIG_CABLE_MODEM)
	{
		HUINT8 aucTempMacAddr[MAC_ADDR_LEN];
		HUINT8 aucMacAddr[MAC_ADDR_LEN];

		DRV_NVRAM_GetField(DI_NVRAM_FIELD_CM_MAC_ADDR, 0, aucMacAddr, MAC_ADDR_LEN);
		DI_CM_GetCmMac(aucTempMacAddr);
		DI_UART_Dump(aucMacAddr,6,6);
		DI_UART_Dump(aucTempMacAddr,6,6);
		if(VK_memcmp(aucMacAddr,aucTempMacAddr,MAC_ADDR_LEN))
		{
			DI_CM_SetCmMac(aucMacAddr);
		}
	}
#endif
	return DRV_OK;
}

/*******************************************************************************
* function : DI_NETWORK_GetDevCount
* description :
* input :
* output :
* return :
* ??Ÿ ?????ڷ? ?? ????
*******************************************************************************/
DI_ERR_CODE DI_NETWORK_GetDevCount(HUINT32 *pulCount)
{
	PrintEnter();

	if(pulCount == NULL)
	{
		PrintError("[%s:%d] pulCount is NULL!\n", __FUNCTION__, __LINE__);
		return DI_ERR_ERROR;
	}
	*pulCount = s_ulNetDevNum;

	PrintExit();

	return DI_ERR_OK;
}

/*******************************************************************************
* function : DI_NETWORK_GetDevInfo
* description :
* input :
* output :
* return :
* ??Ÿ ?????ڷ? ?? ????
*******************************************************************************/
DI_ERR_CODE DI_NETWORK_GetDevInfo(HUINT32 ulDevId, DI_NETWORK_GET_INFO_t *pstGetInfo)
{
	DRV_Error eDrvErr;
	DI_ERR_CODE eDiErr = DI_ERR_OK;
#if defined(CONFIG_SHARED_COMPILE)
	int		fd;
	char	dev_path[256];
	char	link_status;
#endif

	PrintEnter();

	if(ulDevId >= s_ulNetDevNum)
	{
		PrintError("[%s:%d] Invalid Device ID!\n", __FUNCTION__, __LINE__);
		return DI_ERR_ERROR;
	}

	GET_NET_DEV_SEM(ulDevId);

	/* copy dev name and type */
	VK_MEM_Memset(pstGetInfo, 0, sizeof(DI_NETWORK_GET_INFO_t));
	VK_strncpy(pstGetInfo->szName, s_stNetInst[ulDevId].pstDevInfo->szName, IF_NAME_SIZE-1);
	pstGetInfo->eDevType = s_stNetInst[ulDevId].pstDevInfo->eDevType;

	/* get link status */
	if(s_stNetInst[ulDevId].pstDevInfo->eDevType == DI_NETWORK_DEV_WIFI)
	{
#if defined(CONFIG_WIFI)
#if defined(CONFIG_SHARED_COMPILE)
		VK_snprintf(dev_path, sizeof(dev_path)-1, "/sys/class/net/%s/carrier", s_stNetInst[ulDevId].pstDevInfo->szName);
		fd = open(dev_path, O_RDONLY);
		if(fd >= 0) {
			if(read(fd, &link_status, sizeof(char)) > 0) {
				pstGetInfo->bIsConnected = (link_status == '1') ? TRUE : FALSE;
			}
			close(fd);
		}
#else
		eDrvErr = DRV_WIFI_GetDongleStatus(ulDevId, &pstGetInfo->bIsConnected);
		if(eDrvErr != DRV_OK)
		{
			PrintError("[%s:%d] DRV_WIFI_GetDongleStatus Error!\n", __FUNCTION__, __LINE__);
			eDiErr = DI_ERR_ERROR;
			goto END;
		}
#endif
		if(!pstGetInfo->bIsConnected)
		{
			eDiErr = DI_ERR_OK;
			goto END;
		}
#else
		PrintError("[%s:%d] Can't supported WI-FI!\n", __FUNCTION__, __LINE__);
		eDiErr = DI_ERR_ERROR;
		goto END;
#endif
	}
	else if(s_stNetInst[ulDevId].pstDevInfo->eDevType == DI_NETWORK_DEV_PPPOE)
	{
#if defined(CONFIG_PPPOE)
#if defined(CONFIG_SHARED_COMPILE)
		VK_snprintf(dev_path, sizeof(dev_path)-1, "/sys/class/net/%s/carrier", s_stNetInst[ulDevId].pstDevInfo->szName);
		fd = open(dev_path, O_RDONLY);
		if(fd >= 0) {
			if(read(fd, &link_status, sizeof(char)) > 0) {
				pstGetInfo->bIsConnected = (link_status == '1') ? TRUE : FALSE;
			}
			close(fd);
		}
#else
		DRV_PPPOE_GetLinkStatus(ulDevId, &pstGetInfo->bIsConnected);
#endif
#endif
		if(pstGetInfo->bIsConnected == FALSE)
		{
			eDiErr = DI_ERR_OK;
			goto END;
		}
	}
	else
	{
		eDrvErr = P_NETWORK_GetDevLinkStatus(ulDevId, &pstGetInfo->bIsConnected);
		if(eDrvErr != DRV_OK)
		{
			PrintError("[%s:%d] P_NETWORK_GetDevLinkStatus Error!\n", __FUNCTION__, __LINE__);
			eDiErr = DI_ERR_ERROR;
			goto END;
		}
	}

	/* get dev status */
	eDrvErr = P_NETWORK_GetDevStatus(ulDevId, &pstGetInfo->bIsUp, &pstGetInfo->bIsDynamic);
	if(eDrvErr != DRV_OK)
	{
		PrintError("[%s:%d] P_NETWORK_GetDevStatus Error!\n", __FUNCTION__, __LINE__);
		eDiErr = DI_ERR_ERROR;
		goto END;
	}

		/* get mac addr */
#if defined(CONFIG_QV940)
	if(s_stNetInst[ulDevId].pstDevInfo->eDevType == DI_NETWORK_DEV_WIFI)
	{
		HCHAR buf[129] = {0,};
		HUINT8 ulMac[MAC_ADDR_LEN] = {0,};

		DI_WIFI_GetMAC(buf);
		sscanf(buf, "%x:%x:%x:%x:%x:%x", &ulMac[0], &ulMac[1], &ulMac[2], &ulMac[3], &ulMac[4], &ulMac[5]);
		pstGetInfo->aucMacAddr[0] = ulMac[0];
		pstGetInfo->aucMacAddr[1] = ulMac[1];
		pstGetInfo->aucMacAddr[2] = ulMac[2];
		pstGetInfo->aucMacAddr[3] = ulMac[3];
		pstGetInfo->aucMacAddr[4] = ulMac[4];
		pstGetInfo->aucMacAddr[5] = ulMac[5];

		eDrvErr = DRV_OK;
	}
	else
#endif
	{
		eDrvErr = P_NETWORK_GetMacAddr(ulDevId, pstGetInfo->aucMacAddr);
		if(eDrvErr != DRV_OK)
		{
			PrintError("[%s:%d] P_NETWORK_GetMacAddr Error!\n", __FUNCTION__, __LINE__);
			eDiErr = DI_ERR_ERROR;
			goto END;
		}
	}


	/* get ip addr for v4 */
	eDrvErr = P_NETWORK_GetIpAddr(ulDevId, pstGetInfo->stIpv4Addr.aucIpAddr, pstGetInfo->stIpv4Addr.aucBcastAddr, pstGetInfo->stIpv4Addr.aucMaskAddr);
	if(eDrvErr != DRV_OK)
	{
		PrintError("[%s:%d] P_NETWORK_GetIpAddr Error!\n", __FUNCTION__, __LINE__);
		eDiErr = DI_ERR_ERROR;
		goto END;
	}

#if defined(CONFIG_IPV6)
	/* get ip addr for v6 */
	eDrvErr = P_NETWORK_GetIpAddr6(ulDevId, pstGetInfo->stIpv6Addr.eScope, pstGetInfo->stIpv6Addr.aucIpAddr, &pstGetInfo->stIpv6Addr.ulPrefix);
	if(eDrvErr != DRV_OK)
	{
		PrintError("[%s:%d] P_NETWORK_GetIpAddr6 Error!\n", __FUNCTION__, __LINE__);
		eDiErr = DI_ERR_ERROR;
		goto END;
	}
#endif

END:

	REL_NET_DEV_SEM(ulDevId);

	PrintExit();

	return eDiErr;
}

/*******************************************************************************
* function : DI_NETWORK_GetDevDataAmountInfo
* description :
* input :
* output :
* return :
* ??Ÿ ?????ڷ? ?? ????
*******************************************************************************/
DI_ERR_CODE DI_NETWORK_GetDevDataAmountInfo(HUINT32 ulDevId, DI_NETWORK_DATA_AMOUNT_INFO_t *pstGetStatus)
{
	DI_ERR_CODE eDiErr = DI_ERR_OK;
	FILE *fp;
	char szBuf[256] = {0,};
	char szCmdBuf[128] = {0,};
 	HUINT32 ulRecBytes,ulRecPackets,ulRecErrs,ulRecDrop,ulRecFifo,ulRecFrame,ulRecCompressed,ulRecMulticast;
	HUINT32 ulTransBytes,ulTransPackets,ulTransErrs,ulTransDrop,ulTransFifo,ulTransFrame,ulTransCompressed,ulTransMulticast;
	int ret;
	char * pch;

	PrintEnter();

	if(ulDevId >= s_ulNetDevNum)
	{
		PrintError("[%s:%d] Invalid Device ID! %d  %d\n", __FUNCTION__, __LINE__,ulDevId, s_ulNetDevNum);
		return DI_ERR_ERROR;
	}

 	/* copy dev name and type */
	VK_MEM_Memset(pstGetStatus, 0, sizeof(DI_NETWORK_DATA_AMOUNT_INFO_t));

	GET_NET_MOD_SEM;

	fp = fopen("/proc/net/dev", "r");
	if (!fp)
	{
		REL_NET_MOD_SEM;
		PrintError("[%s:%d] fopen Error : %s!\n", __FUNCTION__, __LINE__, strerror(errno));
		return DI_ERR_ERROR;
	}

	while(fgets(szBuf, sizeof(szBuf), fp))
	{
		PrintDebug("%s \n",szBuf);
		pch = VK_strstr(szBuf,s_stNetInst[ulDevId].pstDevInfo->szName);
		if(pch == 0)
		{
			continue;
		}

		ret = VK_sscanf(szBuf, "%s %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d",
				szCmdBuf, (int*)&ulRecBytes,(int*)&ulRecPackets,(int*)&ulRecErrs,(int*)&ulRecDrop,(int*)&ulRecFifo,(int*)&ulRecFrame,(int*)&ulRecCompressed,(int*)&ulRecMulticast,
				(int*)&ulTransBytes,(int*)&ulTransPackets,(int*)&ulTransErrs,(int*)&ulTransDrop,(int*)&ulTransFifo,(int*)&ulTransFrame,(int*)&ulTransCompressed,(int*)&ulTransMulticast);
		if(ret != 17)
		{
			PrintError("[%s:%d] VK_sscanf Error : %s!\n", __FUNCTION__, __LINE__, strerror(errno));
			continue;
		}

		pstGetStatus->ulBytesReceive = ulRecBytes;
		pstGetStatus->ulPacketsReceive = ulRecPackets;
		pstGetStatus->ulBytesSend = ulTransBytes;
		pstGetStatus->ulPacketsSend = ulTransPackets;
		PrintDebug("[%s]_%d ulBytesReceive %d ulPacketsReceive %d ulBytesSend %d ulPacketsSend %d \n",__func__,__LINE__,
			pstGetStatus->ulBytesReceive,pstGetStatus->ulPacketsReceive,pstGetStatus->ulBytesSend,pstGetStatus->ulPacketsSend);

	}

	fclose(fp);
	REL_NET_MOD_SEM;

	PrintExit();
	return eDiErr;
}

/*******************************************************************************
* function : DI_NETWORK_SetDevInfo
* description :
* input :
* output :
* return :
* ??Ÿ ?????ڷ? ?? ????
*******************************************************************************/
DI_ERR_CODE DI_NETWORK_SetDevInfo(HUINT32 ulDevId, DI_NETWORK_SET_INFO_t *pstSetInfo)
{
	DRV_Error eDrvErr;
	DI_ERR_CODE eDiErr = DI_ERR_OK;
	HBOOL bIsUp, bIsDynamic;

	PrintEnter();

	if(ulDevId >= s_ulNetDevNum)
	{
		PrintError("[%s:%d] Invalid Device ID!\n", __FUNCTION__, __LINE__);
		return DI_ERR_ERROR;
	}

	GET_NET_DEV_SEM(ulDevId);

	/* get status flags */
	eDrvErr = P_NETWORK_GetDevStatus(ulDevId, &bIsUp, &bIsDynamic);
	if(eDrvErr != DRV_OK)
	{
		PrintError("[%s:%d] P_NETWORK_GetDevStatus Error!\n", __FUNCTION__, __LINE__);
		eDiErr = DI_ERR_ERROR;
		goto END;
	}

	if(bIsUp)
	{
		/* set ip addr for v4 */
		eDrvErr = P_NETWORK_SetIpAddr(ulDevId, pstSetInfo->stIpv4Addr.aucIpAddr, pstSetInfo->stIpv4Addr.aucBcastAddr, pstSetInfo->stIpv4Addr.aucMaskAddr);
		if(eDrvErr != DRV_OK)
		{
			PrintError("[%s:%d] P_NETWORK_SetIpAddr Error!\n", __FUNCTION__, __LINE__);
			eDiErr = DI_ERR_ERROR;
			goto END;
		}
#if defined(CONFIG_IPV6)
		/* set ip addr for v6 */
		eDrvErr = P_NETWORK_SetIpAddr6(ulDevId, pstSetInfo->eIpv6AddrOp, pstSetInfo->stIpv6Addr.aucIpAddr, pstSetInfo->stIpv6Addr.ulPrefix);
		if(eDrvErr != DRV_OK)
		{
			PrintError("[%s:%d] P_NETWORK_SetIpAddr6 Error!\n", __FUNCTION__, __LINE__);
			eDiErr = DI_ERR_ERROR;
			goto END;
		}
#endif
	}

END:
	REL_NET_DEV_SEM(ulDevId);

	PrintExit();

	return eDiErr;
}

/*******************************************************************************
* function : DI_NETWORK_DevUp
* description :
* input :
* output :
* return :
* ??Ÿ ?????ڷ? ?? ????
*******************************************************************************/
DI_ERR_CODE DI_NETWORK_DevUp(HUINT32 ulDevId)
{
	DRV_Error eDrvErr;
	DI_ERR_CODE eDiErr = DI_ERR_OK;

	PrintEnter();

	if(ulDevId >= s_ulNetDevNum)
	{
		PrintError("[%s:%d] Invalid Device ID!\n", __FUNCTION__, __LINE__);
		return DI_ERR_ERROR;
	}

	GET_NET_DEV_SEM(ulDevId);

	/* dev up */
	eDrvErr = P_NETWORK_SetDevStatus(ulDevId, TRUE, FALSE);
	if(eDrvErr != DRV_OK)
	{
		PrintError("[%s:%d] P_NETWORK_SetDevStatus Error!\n", __FUNCTION__, __LINE__);
		eDiErr = DI_ERR_ERROR;
		goto END;
	}

END:
	REL_NET_DEV_SEM(ulDevId);

	PrintExit();

	return eDiErr;
}

/*******************************************************************************
* function : DI_NETWORK_DevDown
* description :
* input :
* output :
* return :
* ??Ÿ ?????ڷ? ?? ????
*******************************************************************************/
DI_ERR_CODE DI_NETWORK_DevDown(HUINT32 ulDevId)
{
	DRV_Error eDrvErr;
	DI_ERR_CODE eDiErr = DI_ERR_OK;

	PrintEnter();

	if(ulDevId >= s_ulNetDevNum)
	{
		PrintError("[%s:%d] Invalid Device ID!\n", __FUNCTION__, __LINE__);
		return DI_ERR_ERROR;
	}

	GET_NET_DEV_SEM(ulDevId);

	/* dev down */
	eDrvErr = P_NETWORK_SetDevStatus(ulDevId, FALSE, FALSE);
	if(eDrvErr != DRV_OK)
	{
		PrintError("[%s:%d] P_NETWORK_SetDevStatus Error!\n", __FUNCTION__, __LINE__);
		eDiErr = DI_ERR_ERROR;
		goto END;
	}

END:
	REL_NET_DEV_SEM(ulDevId);

	PrintExit();

	return eDiErr;
}

/*******************************************************************************
* function : DI_NETWORK_RegisterEventCallback
* description :
* input :
* output :
* return :
* ??Ÿ ?????ڷ? ?? ????
*******************************************************************************/
DI_ERR_CODE DI_NETWORK_RegisterEventCallback(HUINT32 ulDevId, DI_NETWORK_EVENT_e eEvent, pfnDI_NETWORK_EVENT_CALLBACK pfnEventCallback)
{
	PrintEnter();

	if(ulDevId >= s_ulNetDevNum)
	{
		PrintError("[%s:%d] Invalid Device ID!\n", __FUNCTION__, __LINE__);
		return DI_ERR_ERROR;
	}

	GET_NET_DEV_SEM(ulDevId);

	s_stNetInst[ulDevId].pfnEventCallback[eEvent] = pfnEventCallback;

	REL_NET_DEV_SEM(ulDevId);

	PrintExit();

	return DI_ERR_OK;
}

/*******************************************************************************
* function : DI_NETWORK_GetDnsCount
* description :
* input :
* output :
* return :
* ??Ÿ ?????ڷ? ?? ????
*******************************************************************************/
DI_ERR_CODE DI_NETWORK_GetDnsCount(HUINT32 *pulIPv4Cnt, HUINT32 *pulIPv6Cnt)
{
	DI_ERR_CODE eDiErr = DI_ERR_OK;
	FILE *fp = NULL;
	HCHAR szBuf[128] = {0, };
	HUINT32 ulIpv4Cnt = 0;
	HUINT32 ulIpv6Cnt = 0;
#if defined(CONFIG_SHARED_COMPILE)
	struct flock	fl;
	int fd;
#endif

	PrintEnter();

#if !defined(CONFIG_IPV6)
	UNREFENCED_PARAM(pulIPv6Cnt);
#endif

	GET_NET_MOD_SEM;

#if defined(CONFIG_SHARED_COMPILE)
	fd = open(RESOLV_CONF, O_RDWR);
	if (fd < 0)
	{
		PrintError("[%s:%d] open rrror : %s!\n", __FUNCTION__, __LINE__, strerror(errno));
		eDiErr = DI_ERR_ERROR;
		goto END;
	}
	fp = fdopen(fd, "r");
#else
	fp = fopen(RESOLV_CONF, "r");
#endif

	if(fp == NULL)
	{
		PrintError("[%s:%d] fopen Error : %s!\n", __FUNCTION__, __LINE__, strerror(errno));
		eDiErr = DI_ERR_ERROR;
		goto END;
	}

#if defined(CONFIG_SHARED_COMPILE)
	fl.l_type = F_WRLCK;
	fl.l_whence = SEEK_SET;
	fl.l_start = 0;
	fl.l_len = 0;

	if (fcntl(fd, F_SETLKW, &fl) == -1) {
		PrintError("[%s] Can't lock %s file!!\n", __FUNCTION__, RESOLV_CONF);
		eDiErr = DI_ERR_ERROR;
		goto END;
	}
#endif

	while(fgets(szBuf, sizeof(szBuf), fp))
	{
		/* find nameserver */
		if(VK_strstr(szBuf, "nameserver "))
		{
			if(VK_strstr(szBuf, "."))		/* unstable parsing? */
				ulIpv4Cnt++;
			else if(VK_strstr(szBuf, ":"))
				ulIpv6Cnt++;
		}
	}

	if(pulIPv4Cnt)
		*pulIPv4Cnt = ulIpv4Cnt;
#if defined(CONFIG_IPV6)
	if(pulIPv6Cnt)
		*pulIPv6Cnt = ulIpv6Cnt;
#endif

END:
	if(NULL != fp)
	fclose(fp);

#if defined(CONFIG_SHARED_COMPILE)
	if(fd >= 0)
		close(fd);
#endif

	REL_NET_MOD_SEM;

	PrintExit();

	return eDiErr;
}

/*******************************************************************************
* function : DI_NETWORK_GetDnsInfo
* description :
* input :
* output :
* return :
* ??Ÿ ?????ڷ? ?? ????
*******************************************************************************/
DI_ERR_CODE DI_NETWORK_GetDnsInfo(DI_NETWORK_IP_VER_e eVer, HUINT32 unIdx, HUINT8 *pDns)
{
	DI_ERR_CODE eDiErr = DI_ERR_OK;
	FILE *fp = NULL;
	HCHAR szBuf[128] = {0, };
	HCHAR *pAddr = NULL;
	HUINT32 i = 0;
#if defined(CONFIG_SHARED_COMPILE)
	struct flock	fl;
	int	fd;
#endif

	PrintEnter();

	GET_NET_MOD_SEM;

#if defined(CONFIG_SHARED_COMPILE)
	fd = open(RESOLV_CONF, O_RDWR);
	if (fd < 0)
	{
		PrintError("[%s:%d] open rrror : %s!\n", __FUNCTION__, __LINE__, strerror(errno));
		eDiErr = DI_ERR_ERROR;
		goto END;
	}
	fp = fdopen(fd, "r");
#else
	fp = fopen(RESOLV_CONF, "r");
#endif

	if(fp == NULL)
	{
		PrintError("[%s:%d] fopen Error : %s!\n", __FUNCTION__, __LINE__, strerror(errno));
		eDiErr = DI_ERR_ERROR;
		goto END;
	}

#if defined(CONFIG_SHARED_COMPILE)
	fl.l_type = F_WRLCK;
	fl.l_whence = SEEK_SET;
	fl.l_start = 0;
	fl.l_len = 0;

	if (fcntl(fd, F_SETLKW, &fl) == -1) {
		PrintError("[%s] Can't lock %s file!!\n", __FUNCTION__, RESOLV_CONF);
		eDiErr = DI_ERR_ERROR;
		goto END;
	}
#endif

	if(eVer == DI_NETWORK_IP_VER_4)
	{
		while(fgets(szBuf, sizeof(szBuf), fp))
		{
			pAddr = NULL;

			/* find nameserver */
			if((pAddr = VK_strstr(szBuf, "nameserver ")) != NULL)
			{
				if(VK_strstr(szBuf, "."))
				{
					if(unIdx == i)
					{
						pAddr += sizeof("nameserver");
CHECK_WHITESPACE:
						if(*pAddr == '\0')
						{
							eDiErr = DI_ERR_ERROR;
							goto END;
						}

						if(isalnum(*pAddr) == 0)
						{
							pAddr += 1;	/* jump whitespace */
							goto CHECK_WHITESPACE;
						}

						if(pAddr[VK_strlen(pAddr)-1] == '\n')	/* delete '\n' */
							pAddr[VK_strlen(pAddr)-1] = '\0';

						inet_pton(AF_INET, pAddr, pDns);
					}
					i++;
				}
			}
		}
	}
#if defined(CONFIG_IPV6)
	else if(eVer == DI_NETWORK_IP_VER_6)
	{
		while(fgets(szBuf, sizeof(szBuf), fp))
		{
			pAddr = NULL;

			/* find nameserver */
			if((pAddr = VK_strstr(szBuf, "nameserver ")) != NULL)
			{
				if(VK_strstr(szBuf, ":"))
				{
					if(unIdx == i)
					{
						pAddr += sizeof("nameserver");
CHECK_WHITESPACE_:
						if(*pAddr == '\0')
						{
							eDiErr = DI_ERR_ERROR;
							goto END;
						}

						if(isalnum(*pAddr) == 0)
						{
							pAddr += 1;	/* jump whitespace */
							goto CHECK_WHITESPACE_;
						}

						if(pAddr[VK_strlen(pAddr)-1] == '\n')	/* delete '\n' */
							pAddr[VK_strlen(pAddr)-1] = '\0';

						inet_pton(AF_INET6, pAddr, pDns);
					}
					i++;
				}
			}
		}
	}
#endif

END:
	if(NULL != fp)
	fclose(fp);

#if defined(CONFIG_SHARED_COMPILE)
	if(fd >= 0)
		close(fd);
#endif

	REL_NET_MOD_SEM;

	PrintExit();

	return eDiErr;
}

/*******************************************************************************
* function : DI_NETWORK_SetDnsInfo
* description :
* input :
* output :
* return :
* ??Ÿ ?????ڷ? ?? ????
*******************************************************************************/
DI_ERR_CODE DI_NETWORK_SetDnsInfo(HUINT32 ulIPv4Cnt, HUINT8 *pucIpv4DnsList, HUINT32 ulIPv6Cnt, HUINT8 *pucIpv6DnsList)
{
	DI_ERR_CODE eDiErr = DI_ERR_OK;
	FILE *fp = NULL;
	char acDnsBuf[128];
	HUINT32 i;
#if defined(CONFIG_SHARED_COMPILE)
	struct flock	fl;
	int	fd;
#endif

	PrintEnter();

#if !defined(CONFIG_IPV6)
	UNREFENCED_PARAM(ulIPv6Cnt);
	UNREFENCED_PARAM(pucIpv6DnsList);
#endif

	GET_NET_MOD_SEM;

#if defined(CONFIG_SHARED_COMPILE)
	fd = open(RESOLV_CONF, O_CREAT | O_RDWR);
	if (fd < 0)
	{
		PrintError("[%s:%d] open rrror : %s!\n", __FUNCTION__, __LINE__, strerror(errno));
		eDiErr = DI_ERR_ERROR;
		goto END;
	}
	fp = fdopen(fd, "w");
#else
	fp = fopen(RESOLV_CONF, "w");
#endif

	if(fp == NULL)
	{
		PrintError("[%s:%d] fopen Error : %s!\n", __FUNCTION__, __LINE__, strerror(errno));
		eDiErr = DI_ERR_ERROR;
		goto END;
	}

#if defined(CONFIG_SHARED_COMPILE)
	fl.l_type = F_WRLCK;
	fl.l_whence = SEEK_SET;
	fl.l_start = 0;
	fl.l_len = 0;

	if (fcntl(fd, F_SETLKW, &fl) == -1) {
		PrintError("[%s] Can't lock %s file(%s)!!\n", __FUNCTION__, RESOLV_CONF, strerror(errno));
		eDiErr = DI_ERR_ERROR;
		goto END;
	}
#endif

	for(i=0; i<ulIPv4Cnt; i++)
	{
		VK_MEM_Memset(acDnsBuf, 0, 128);
		VK_snprintf(acDnsBuf, 128, "nameserver %i.%i.%i.%i\n", pucIpv4DnsList[i*4+0], pucIpv4DnsList[i*4+1], pucIpv4DnsList[i*4+2], pucIpv4DnsList[i*4+3]);
		fwrite(acDnsBuf, 1, VK_strlen(acDnsBuf), fp);
	}
#if defined(CONFIG_IPV6)
	for(i=0; i<ulIPv6Cnt; i++)
	{
		VK_MEM_Memset(acDnsBuf, 0, 128);
		VK_snprintf(acDnsBuf, 128, "nameserver %02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X\n",
							pucIpv6DnsList[i*16+0], pucIpv6DnsList[i*16+1], pucIpv6DnsList[i*16+2], pucIpv6DnsList[i*16+3],
							pucIpv6DnsList[i*16+4], pucIpv6DnsList[i*16+5], pucIpv6DnsList[i*16+6], pucIpv6DnsList[i*16+7],
							pucIpv6DnsList[i*16+8], pucIpv6DnsList[i*16+9], pucIpv6DnsList[i*16+10], pucIpv6DnsList[i*16+11],
							pucIpv6DnsList[i*16+12], pucIpv6DnsList[i*16+13], pucIpv6DnsList[i*16+14], pucIpv6DnsList[i*16+15]);
		fwrite(acDnsBuf, 1, VK_strlen(acDnsBuf), fp);
	}
#endif

	P_NETWORK_InitResolver();
END:
	if(NULL != fp)
	fclose(fp);

#if defined(CONFIG_SHARED_COMPILE)
	if(fd >= 0)
		close(fd);
#endif

	REL_NET_MOD_SEM;

	PrintExit();

	return eDiErr;
}

/*******************************************************************************
* function : DI_NETWORK_GetDnsMode
* description :
* input :
* output :
* return :
* ??Ÿ ?????ڷ? ?? ????
*******************************************************************************/
DI_ERR_CODE DI_NETWORK_GetDnsMode(DI_NETWORK_IP_VER_e eVer, HBOOL *pbIsUserSet)
{
	HCHAR *szEnv;

	PrintEnter();

	if(pbIsUserSet == NULL)
	{
		PrintError("[%s:%d] Invalid Parameter!\n", __FUNCTION__, __LINE__);
		return DI_ERR_ERROR;
	}

	GET_NET_MOD_SEM;

	*pbIsUserSet = FALSE;
	if(eVer == DI_NETWORK_IP_VER_4)
	{
		szEnv = getenv("ENV_DNS_USER_SET");
		if(szEnv)
		{
			if(!VK_strcmp(szEnv, "on"))
			{
				*pbIsUserSet = TRUE;
			}
		}
	}
#if defined(CONFIG_IPV6)
	else if(eVer == DI_NETWORK_IP_VER_6)
	{
		szEnv = getenv("ENV_DNS_USER_SET_V6");
		if(szEnv)
		{
			if(!VK_strcmp(szEnv, "on"))
			{
				*pbIsUserSet = TRUE;
			}
		}
	}
#endif
	REL_NET_MOD_SEM;

	PrintExit();

	return DI_ERR_OK;
}

/*******************************************************************************
* function : DI_NETWORK_SetDnsMode
* description :
* input :
* output :
* return :
* ??Ÿ ?????ڷ? ?? ????
*******************************************************************************/
DI_ERR_CODE DI_NETWORK_SetDnsMode(DI_NETWORK_IP_VER_e eVer, HBOOL bIsUserSet)
{
	DI_ERR_CODE eDiErr = DI_ERR_OK;
	int ret;

	PrintEnter();

	GET_NET_MOD_SEM;

	if(eVer == DI_NETWORK_IP_VER_4)
	{
		ret = setenv("ENV_DNS_USER_SET", bIsUserSet ? "on" : "off", TRUE);
		if(ret != 0)
		{
			PrintError("[%s:%d] setenv(ENV_DNS_USER_SET) Error : %s!\n", __FUNCTION__, __LINE__, strerror(errno));
			eDiErr = DI_ERR_ERROR;
			goto END;
		}
	}
#if defined(CONFIG_IPV6)
	else if(eVer == DI_NETWORK_IP_VER_6)
	{
		ret = setenv("ENV_DNS_USER_SET_V6", bIsUserSet ? "on" : "off", TRUE);
		if(ret != 0)
		{
			PrintError("[%s:%d] setenv(ENV_DNS_USER_SET6) Error : %s!\n", __FUNCTION__, __LINE__, strerror(errno));
			eDiErr = DI_ERR_ERROR;
			goto END;
		}
	}
#endif
END:
	REL_NET_MOD_SEM;

	PrintExit();

	return eDiErr;
}

/*******************************************************************************
* function : DI_NETWORK_SetDnsMode
* description :
* input :
* output :
* return :
* ??Ÿ ?????ڷ? ?? ????
*******************************************************************************/
DI_ERR_CODE DI_NETWORK_GetDnsDomainName(HUINT32 *pulDomainCnt, HUINT8 *pucDomainList)
{
	DI_ERR_CODE eDiErr = DI_ERR_OK;
	FILE *fp = NULL;
	HCHAR szBuf[128] = {0, };
	HCHAR *pAddr = NULL;
	HUINT32 i = 0;
	char *pcDomain, *pcSavePtr;
#if defined(CONFIG_SHARED_COMPILE)
	struct flock	fl;
	int fd;
#endif

	PrintEnter();

	GET_NET_MOD_SEM;

#if defined(CONFIG_SHARED_COMPILE)
	fd = open(RESOLV_CONF, O_RDWR);
	if (fd < 0)
	{
		PrintError("[%s:%d] open rrror : %s!\n", __FUNCTION__, __LINE__, strerror(errno));
		eDiErr = DI_ERR_ERROR;
		goto END;
	}
	fp = fdopen(fd, "r");
#else
	fp = fopen(RESOLV_CONF, "r");
#endif

	if(fp == NULL)
	{
		PrintError("[%s:%d] fopen Error : %s!\n", __FUNCTION__, __LINE__, strerror(errno));
		eDiErr = DI_ERR_ERROR;
		goto END;
	}

#if defined(CONFIG_SHARED_COMPILE)
	fl.l_type = F_WRLCK;
	fl.l_whence = SEEK_SET;
	fl.l_start = 0;
	fl.l_len = 0;

	if (fcntl(fd, F_SETLKW, &fl) == -1) {
		PrintError("[%s] Can't lock %s file!!\n", __FUNCTION__, RESOLV_CONF);
		eDiErr = DI_ERR_ERROR;
		goto END;
	}
#endif

	while(fgets(szBuf, sizeof(szBuf), fp))
	{
		pAddr = NULL;

		/* find domain name */
		if((pAddr = VK_strstr(szBuf, "search ")) != NULL)
		{
			i = *pulDomainCnt = 0;
			pAddr += sizeof("search");
			pcDomain = strtok_r(pAddr, " ", &pcSavePtr);
			while(pcDomain)
			{
				if(*pulDomainCnt)
				{
					if(pucDomainList[i-1] == '\n')
						pucDomainList[i-1] = ' ';
					else
						pucDomainList[i++] = ' ';
				}
				(*pulDomainCnt)++;
				VK_strncpy(&pucDomainList[i], pcDomain, VK_strlen(pcDomain));
				i += VK_strlen(pcDomain);

				pcDomain = strtok_r(NULL, " ", &pcSavePtr);
			}
			if(pucDomainList[i-1] == '\n')
				pucDomainList[i-1] = '\0';
			else
			pucDomainList[i] = '\0';
		}
	}

END:
	if(NULL != fp)
	fclose(fp);

#if defined(CONFIG_SHARED_COMPILE)
	if(fd >= 0)
		close(fd);
#endif

	REL_NET_MOD_SEM;

	PrintExit();

	return eDiErr;
}

/*******************************************************************************
* function : DI_NETWORK_GetDefaultRoute
* description :
* input :
* output :
* return :
* ??Ÿ ?????ڷ? ?? ????
*******************************************************************************/
DI_ERR_CODE DI_NETWORK_GetRouteInfo(HUINT32 ulDevId, DI_NETWORK_IP_VER_e eVer, DI_NETWORK_GET_ROUTE_INFO_t *pRouteInfo)
{
	DRV_Error eDrvErr;
	DI_ERR_CODE eDiErr = DI_ERR_OK;

	PrintEnter();

	if(ulDevId >= s_ulNetDevNum)
	{
		PrintError("[%s:%d] Invalid Device ID!\n", __FUNCTION__, __LINE__);
		return DI_ERR_ERROR;
	}

	if(pRouteInfo == NULL)
	{
		PrintError("[%s:%d] pRouteInfo is NULL!\n", __FUNCTION__, __LINE__);
		return DI_ERR_ERROR;
	}

	GET_NET_MOD_SEM;

	if(eVer == DI_NETWORK_IP_VER_4)
	{
		eDrvErr = P_NETWORK_GetRouteInfo(ulDevId, pRouteInfo);
		if(eDrvErr != DRV_OK)
		{
			PrintError("[%s:%d] P_NETWORK_GetRouteInfo Error!\n", __FUNCTION__, __LINE__);
			eDiErr = DI_ERR_ERROR;
			goto END;
		}
	}
#if defined(CONFIG_IPV6)
	else if(eVer == DI_NETWORK_IP_VER_6)
	{
		/* add */
	}
#endif
END:
	REL_NET_MOD_SEM;

	PrintExit();

	return eDiErr;
}

/*******************************************************************************
* function : DI_NETWORK_SetDefaultRoute
* description :
* input :
* output :
* return :
* ??Ÿ ?????ڷ? ?? ????
*******************************************************************************/
DI_ERR_CODE DI_NETWORK_SetRouteInfo(HUINT32 ulDevId, DI_NETWORK_IP_VER_e eVer, DI_NETWORK_SET_ROUTE_INFO_t *pSetRoute)
{
	DRV_Error eDrvErr;
	DI_ERR_CODE eDiErr = DI_ERR_OK;
	DI_NETWORK_SET_ROUTE_INFO_t stParams;

	PrintEnter();

	if(ulDevId >= s_ulNetDevNum)
	{
		PrintError("[%s:%d] Invalid Device ID!\n", __FUNCTION__, __LINE__);
		return DI_ERR_ERROR;
	}

	if(pSetRoute == NULL)
	{
		PrintError("[%s:%d] SetRoute is NULL!\n", __FUNCTION__, __LINE__);
		return DI_ERR_ERROR;
	}

	GET_NET_MOD_SEM;

	VK_MEM_Memset(&stParams, 0, sizeof(DI_NETWORK_SET_ROUTE_INFO_t));
	VK_memcpy(&stParams, pSetRoute, sizeof(DI_NETWORK_SET_ROUTE_INFO_t));

	if(eVer == DI_NETWORK_IP_VER_4)
	{
		eDrvErr = P_NETWORK_SetRouteInfo(ulDevId, stParams);
		if(eDrvErr != DRV_OK)
		{
			PrintError("[%s:%d] P_NETWORK_SetRouteInfo Error!\n", __FUNCTION__, __LINE__);
			eDiErr = DI_ERR_ERROR;
			goto END;
		}
	}
#if defined(CONFIG_IPV6)
	else if(eVer == DI_NETWORK_IP_VER_6)
	{
		/* add */
	}
#endif

END:
	REL_NET_MOD_SEM;

	PrintExit();

	return eDiErr;
}


/*******************************************************************************
* function : DI_NETWORK_GetDefaultRoute
* description :
* input :
* output :
* return :
* ??Ÿ ?????ڷ? ?? ????
*******************************************************************************/
DI_ERR_CODE DI_NETWORK_GetDefaultRoute(HUINT32 ulDevId, DI_NETWORK_IP_VER_e eVer, HUINT8 *pucRouteAddr)
{
	DRV_Error eDrvErr;
	DI_ERR_CODE eDiErr = DI_ERR_OK;
	DI_NETWORK_ROUTE_PARAMS_t stParams;

	PrintEnter();

	if(ulDevId >= s_ulNetDevNum)
	{
		PrintError("[%s:%d] Invalid Device ID!\n", __FUNCTION__, __LINE__);
		return DI_ERR_ERROR;
	}

	VK_MEM_Memset(&stParams, 0, sizeof(DI_NETWORK_ROUTE_PARAMS_t));

	GET_NET_MOD_SEM;

	if(eVer == DI_NETWORK_IP_VER_4)
	{
		eDrvErr = P_NETWORK_GetGateway(ulDevId, &stParams);
		if(eDrvErr != DRV_OK)
		{
			PrintError("[%s:%d] P_NETWORK_GetGateway Error!\n", __FUNCTION__, __LINE__);
			eDiErr = DI_ERR_ERROR;
			goto END;
		}
		VK_memcpy(pucRouteAddr, stParams.aucRouteAddr, IPV4_ADDR_LEN);
	}
#if defined(CONFIG_IPV6)
	else if(eVer == DI_NETWORK_IP_VER_6)
	{
		/* add */
	}
#endif
END:
	REL_NET_MOD_SEM;

	PrintExit();

	return eDiErr;
}

/*******************************************************************************
* function : DI_NETWORK_SetDefaultRoute
* description :
* input :
* output :
* return :
* ??Ÿ ?????ڷ? ?? ????
*******************************************************************************/
DI_ERR_CODE DI_NETWORK_SetDefaultRoute(HUINT32 ulDevId, DI_NETWORK_IP_VER_e eVer, HUINT8 *pucRouteAddr)
{
	DRV_Error eDrvErr;
	DI_ERR_CODE eDiErr = DI_ERR_OK;
	DI_NETWORK_ROUTE_PARAMS_t stParams;

	PrintEnter();

	if(ulDevId >= s_ulNetDevNum)
	{
		PrintError("[%s:%d] Invalid Device ID!\n", __FUNCTION__, __LINE__);
		return DI_ERR_ERROR;
	}

	if(pucRouteAddr == NULL)
	{
		PrintError("[%s:%d] pucRouteAddr is NULL!\n", __FUNCTION__, __LINE__);
		return DI_ERR_ERROR;
	}

	VK_MEM_Memset(&stParams, 0, sizeof(DI_NETWORK_ROUTE_PARAMS_t));

	GET_NET_MOD_SEM;

	if(eVer == DI_NETWORK_IP_VER_4)
	{
		eDrvErr = P_NETWORK_GetGateway(ulDevId, &stParams);
		if(eDrvErr != DRV_OK)
		{
			PrintError("[%s:%d] P_NETWORK_GetGateway Error!\n", __FUNCTION__, __LINE__);
			eDiErr = DI_ERR_ERROR;
			goto END;
		}

		if(IS_VALID_IPV4_ADDR(stParams.aucRouteAddr))
		{
			eDrvErr = P_NETWORK_SetGateway(ulDevId, P_NETWORK_ADDR_DEL, &stParams);
			if(eDrvErr != DRV_OK)
			{
				PrintError("[%s:%d] P_NETWORK_SetGateway Error!\n", __FUNCTION__, __LINE__);
				eDiErr = DI_ERR_ERROR;
				goto END;
			}
		}

		VK_memcpy(stParams.aucRouteAddr, pucRouteAddr, IPV4_ADDR_LEN);
		stParams.ulMetric = 0;
		eDrvErr = P_NETWORK_SetGateway(ulDevId, P_NETWORK_ADDR_ADD, &stParams);
		if(eDrvErr != DRV_OK)
		{
			PrintError("[%s:%d] P_NETWORK_GetGateway Error!\n", __FUNCTION__, __LINE__);
			eDiErr = DI_ERR_ERROR;
			goto END;
		}
	}
#if defined(CONFIG_IPV6)
	else if(eVer == DI_NETWORK_IP_VER_6)
	{
		/* add */
	}
#endif

END:
	REL_NET_MOD_SEM;

	PrintExit();

	return eDiErr;
}

/*******************************************************************************
* function : DI_NETWORK_GetDefaultRoute
* description :
* input :
* output :
* return :
* ??Ÿ ?????ڷ? ?? ????
*******************************************************************************/
DI_ERR_CODE DI_NETWORK_GetDefaultRouteExt(HUINT32 ulDevId, DI_NETWORK_IP_VER_e eVer, DI_NETWORK_ROUTE_PARAMS_t *pstParams)
{
	DRV_Error eDrvErr;
	DI_ERR_CODE eDiErr = DI_ERR_OK;

	PrintEnter();

	if(ulDevId >= s_ulNetDevNum)
	{
		PrintError("[%s:%d] Invalid Device ID!\n", __FUNCTION__, __LINE__);
		return DI_ERR_ERROR;
	}

	if(pstParams == NULL)
	{
		PrintError("[%s:%d] pstParams is NULL!\n", __FUNCTION__, __LINE__);
		return DI_ERR_ERROR;
	}

	GET_NET_MOD_SEM;

	if(eVer == DI_NETWORK_IP_VER_4)
	{
		eDrvErr = P_NETWORK_GetGateway(ulDevId, pstParams);
		if(eDrvErr != DRV_OK)
		{
			PrintError("[%s:%d] P_NETWORK_GetGateway Error!\n", __FUNCTION__, __LINE__);
			eDiErr = DI_ERR_ERROR;
			goto END;
		}
	}
#if defined(CONFIG_IPV6)
	else if(eVer == DI_NETWORK_IP_VER_6)
	{
		/* add */
	}
#endif
END:
	REL_NET_MOD_SEM;

	PrintExit();

	return eDiErr;
}

/*******************************************************************************
* function : DI_NETWORK_SetDefaultRouteExt
* description :
* input :
* output :
* return :
* ??Ÿ ?????ڷ? ?? ????
*******************************************************************************/
DI_ERR_CODE DI_NETWORK_SetDefaultRouteExt(HUINT32 ulDevId, DI_NETWORK_IP_VER_e eVer, DI_NETWORK_ROUTE_PARAMS_t *pstParams)
{
	DRV_Error eDrvErr;
	DI_ERR_CODE eDiErr = DI_ERR_OK;
	DI_NETWORK_ROUTE_PARAMS_t stParams;

	PrintEnter();

	if(ulDevId >= s_ulNetDevNum)
	{
		PrintError("[%s:%d] Invalid Device ID!\n", __FUNCTION__, __LINE__);
		return DI_ERR_ERROR;
	}

	if(pstParams == NULL)
	{
		PrintError("[%s:%d] pstParams is NULL!\n", __FUNCTION__, __LINE__);
		return DI_ERR_ERROR;
	}

	VK_MEM_Memset(&stParams, 0, sizeof(DI_NETWORK_ROUTE_PARAMS_t));

	GET_NET_MOD_SEM;

	if(eVer == DI_NETWORK_IP_VER_4)
	{
		eDrvErr = P_NETWORK_GetGateway(ulDevId, &stParams);
		if(eDrvErr != DRV_OK)
		{
			PrintError("[%s:%d] P_NETWORK_GetGateway Error!\n", __FUNCTION__, __LINE__);
			eDiErr = DI_ERR_ERROR;
			goto END;
		}

		if(IS_VALID_IPV4_ADDR(stParams.aucRouteAddr))
		{
			eDrvErr = P_NETWORK_SetGateway(ulDevId, P_NETWORK_ADDR_DEL, &stParams);
			if(eDrvErr != DRV_OK)
			{
				PrintError("[%s:%d] P_NETWORK_SetGateway Error!\n", __FUNCTION__, __LINE__);
				eDiErr = DI_ERR_ERROR;
				goto END;
			}
		}

		eDrvErr = P_NETWORK_SetGateway(ulDevId, P_NETWORK_ADDR_ADD, pstParams);
		if(eDrvErr != DRV_OK)
		{
			PrintError("[%s:%d] P_NETWORK_GetGateway Error!\n", __FUNCTION__, __LINE__);
			eDiErr = DI_ERR_ERROR;
			goto END;
		}
	}
#if defined(CONFIG_IPV6)
	else if(eVer == DI_NETWORK_IP_VER_6)
	{
		/* add */
	}
#endif

END:
	REL_NET_MOD_SEM;

	PrintExit();

	return eDiErr;
}

/*******************************************************************************
* function : DI_NETWORK_DeleteDefaultRoute
* description :
* input :
* output :
* return :
* ??Ÿ ?????ڷ? ?? ????
*******************************************************************************/
DI_ERR_CODE DI_NETWORK_DeleteDefaultRoute(HUINT32 ulDevId, DI_NETWORK_IP_VER_e eVer)
{
	DRV_Error eDrvErr;
	DI_ERR_CODE eDiErr = DI_ERR_OK;
	DI_NETWORK_ROUTE_PARAMS_t stParams;

	PrintEnter();

	if(ulDevId >= s_ulNetDevNum)
	{
		PrintError("[%s:%d] Invalid Device ID!\n", __FUNCTION__, __LINE__);
		return DI_ERR_ERROR;
	}

	GET_NET_MOD_SEM;

	if(eVer == DI_NETWORK_IP_VER_4)
	{
		eDrvErr = P_NETWORK_GetGateway(ulDevId, &stParams);
		if(eDrvErr != DRV_OK)
		{
			PrintError("[%s:%d] P_NETWORK_GetGateway Error!\n", __FUNCTION__, __LINE__);
			eDiErr = DI_ERR_ERROR;
			goto END;
		}

		if(IS_VALID_IPV4_ADDR(stParams.aucRouteAddr))
		{
			eDrvErr = P_NETWORK_SetGateway(ulDevId, P_NETWORK_ADDR_DEL, &stParams);
			if(eDrvErr != DRV_OK)
			{
				PrintError("[%s:%d] P_NETWORK_SetGateway Error!\n", __FUNCTION__, __LINE__);
				eDiErr = DI_ERR_ERROR;
				goto END;
			}
		}
	}
#if defined(CONFIG_IPV6)
	else if(eVer == DI_NETWORK_IP_VER_6)
	{
		/* add */
	}
#endif

END:
	REL_NET_MOD_SEM;

	PrintExit();

	return eDiErr;
}

/*******************************************************************************
* function : DI_NETWORK_StartDhcpClient
* description :
* input :
* output :
* return :
* ??Ÿ ?????ڷ? ?? ????
*******************************************************************************/
DI_ERR_CODE DI_NETWORK_StartDhcpClient(HUINT32 ulDevId, DI_NETWORK_IP_VER_e eVer, DI_NETWORK_DHCPC_PARAMS_t *pstParams)
{
	DRV_Error eDrvErr;
	DI_ERR_CODE eDiErr = DI_ERR_OK;

	PrintEnter();

	if(ulDevId >= s_ulNetDevNum)
	{
		PrintError("[%s:%d] Invalid Device ID!\n", __FUNCTION__, __LINE__);
		return DI_ERR_ERROR;
	}

	GET_NET_DEV_SEM(ulDevId);

	if(eVer == DI_NETWORK_IP_VER_4)
	{
		eDrvErr = P_NETWORK_StartDhcpClient(ulDevId, pstParams);
		if(eDrvErr != DRV_OK)
		{
			PrintError("[%s:%d] P_NETWORK_StartDhcpClient Error!\n", __FUNCTION__, __LINE__);
			eDiErr = DI_ERR_ERROR;
			goto END;
		}
	}
#if defined(CONFIG_IPV6)
	else if(eVer == DI_NETWORK_IP_VER_6)
	{
		eDrvErr = P_NETWORK_StartDhcp6Client(ulDevId, pstParams);
		if(eDrvErr != DRV_OK)
		{
			PrintError("[%s:%d] P_NETWORK_StartDhcpClient Error!\n", __FUNCTION__, __LINE__);
			eDiErr = DI_ERR_ERROR;
			goto END;
		}
	}
#endif
	else
	{
		PrintError("[%s:%d] Not supported protocol version!\n", __FUNCTION__, __LINE__);
	}

END:
	REL_NET_DEV_SEM(ulDevId);

	PrintExit();

	return eDiErr;
}

/*******************************************************************************
* function : DI_NETWORK_StopDhcpClient
* description :
* input :
* output :
* return :
* ??Ÿ ?????ڷ? ?? ????
*******************************************************************************/
DI_ERR_CODE DI_NETWORK_StopDhcpClient(HUINT32 ulDevId, DI_NETWORK_IP_VER_e eVer)
{
	DRV_Error eDrvErr;
	DI_ERR_CODE eDiErr = DI_ERR_OK;

	PrintEnter();

	if(ulDevId >= s_ulNetDevNum)
	{
		PrintError("[%s:%d] Invalid Device ID!\n", __FUNCTION__, __LINE__);
		return DI_ERR_ERROR;
	}

	GET_NET_DEV_SEM(ulDevId);

	if(eVer == DI_NETWORK_IP_VER_4)
	{
		eDrvErr = P_NETWORK_StopDhcpClient(ulDevId, TRUE);
		if(eDrvErr != DRV_OK)
		{
			PrintError("[%s:%d] P_NETWORK_StopDhcpClient Error!\n", __FUNCTION__, __LINE__);
			eDiErr = DI_ERR_ERROR;
			goto END;
		}
	}
#if defined(CONFIG_IPV6)
	else if(eVer == DI_NETWORK_IP_VER_6)
	{
		eDrvErr = P_NETWORK_StopDhcp6Client(ulDevId);
		if(eDrvErr != DRV_OK)
		{
			PrintError("[%s:%d] P_NETWORK_StopDhcpClient Error!\n", __FUNCTION__, __LINE__);
			eDiErr = DI_ERR_ERROR;
			goto END;
		}
	}
#endif
	else
	{
		PrintError("[%s:%d] Not supported protocol version!\n", __FUNCTION__, __LINE__);
	}

END:
	REL_NET_DEV_SEM(ulDevId);

	PrintExit();

	return eDiErr;
}

/*******************************************************************************
* function : DI_NETWORK_StartDhcpServer
* description :
* input :
* output :
* return :
* ??Ÿ ?????ڷ? ?? ????
*******************************************************************************/
DI_ERR_CODE DI_NETWORK_StartDhcpServer(HUINT32 ulDevId, DI_NETWORK_IP_VER_e eVer, DI_NETWORK_DHCPS_PARAMS_t *pstParams)
{
	DRV_Error eDrvErr;
	DI_ERR_CODE eDiErr = DI_ERR_OK;

	PrintEnter();

	if(ulDevId >= s_ulNetDevNum)
	{
		PrintError("[%s:%d] Invalid Device ID!\n", __FUNCTION__, __LINE__);
		return DI_ERR_ERROR;
	}

	GET_NET_DEV_SEM(ulDevId);

	if(eVer == DI_NETWORK_IP_VER_4)
	{
		eDrvErr = P_NETWORK_StartDhcpServer(ulDevId, pstParams);
		if(eDrvErr != DRV_OK)
		{
			PrintError("[%s:%d] P_NETWORK_StartDhcpServer Error!\n", __FUNCTION__, __LINE__);
			eDiErr = DI_ERR_ERROR;
			goto END;
		}
	}
#if defined(CONFIG_IPV6)
	else if(eVer == DI_NETWORK_IP_VER_6)
	{
		eDrvErr = P_NETWORK_StartDhcp6Server(ulDevId, pstParams);
		if(eDrvErr != DRV_OK)
		{
			PrintError("[%s:%d] P_NETWORK_StartDhcpServer Error!\n", __FUNCTION__, __LINE__);
			eDiErr = DI_ERR_ERROR;
			goto END;
		}
	}
#endif
	else
	{
		PrintError("[%s:%d] Not supported protocol version!\n", __FUNCTION__, __LINE__);
	}

END:
	REL_NET_DEV_SEM(ulDevId);

	PrintExit();

	return eDiErr;
}

/*******************************************************************************
* function : DI_NETWORK_StopDhcpServer
* description :
* input :
* output :
* return :
* ??Ÿ ?????ڷ? ?? ????
*******************************************************************************/
DI_ERR_CODE DI_NETWORK_StopDhcpServer(HUINT32 ulDevId, DI_NETWORK_IP_VER_e eVer)
{
	DRV_Error eDrvErr;
	DI_ERR_CODE eDiErr = DI_ERR_OK;

	PrintEnter();

	if(ulDevId >= s_ulNetDevNum)
	{
		PrintError("[%s:%d] Invalid Device ID!\n", __FUNCTION__, __LINE__);
		return DI_ERR_ERROR;
	}

	GET_NET_DEV_SEM(ulDevId);

	if(eVer == DI_NETWORK_IP_VER_4)
	{
		eDrvErr = P_NETWORK_StopDhcpServer(ulDevId);
		if(eDrvErr != DRV_OK)
		{
			PrintError("[%s:%d] P_NETWORK_StopDhcpServer Error!\n", __FUNCTION__, __LINE__);
			eDiErr = DI_ERR_ERROR;
			goto END;
		}
	}
#if defined(CONFIG_IPV6)
	else if(eVer == DI_NETWORK_IP_VER_6)
	{
		eDrvErr = P_NETWORK_StopDhcp6Server(ulDevId);
		if(eDrvErr != DRV_OK)
		{
			PrintError("[%s:%d] P_NETWORK_StopDhcpServer Error!\n", __FUNCTION__, __LINE__);
			eDiErr = DI_ERR_ERROR;
			goto END;
		}
	}
#endif
	else
	{
		PrintError("[%s:%d] Not supported protocol version!\n", __FUNCTION__, __LINE__);
	}

END:
	REL_NET_DEV_SEM(ulDevId);

	PrintExit();

	return eDiErr;
}

/*******************************************************************************
* function : DI_NETWORK_StartAutoIp
* description :
* input :
* output :
* return :
* ??Ÿ ?????ڷ? ?? ????
*******************************************************************************/
DI_ERR_CODE DI_NETWORK_StartAutoIp(HUINT32 ulDevId)
{
	DRV_Error eDrvErr;
	DI_ERR_CODE eDiErr = DI_ERR_OK;

	PrintEnter();

	if(ulDevId >= s_ulNetDevNum)
	{
		PrintError("[%s:%d] Invalid Device ID!\n", __FUNCTION__, __LINE__);
		return DI_ERR_ERROR;
	}

	GET_NET_DEV_SEM(ulDevId);

	eDrvErr = P_NETWORK_StartAutoIp(ulDevId);
	if(eDrvErr != DRV_OK)
	{
		PrintError("[%s:%d] P_NETWORK_StartAutoIp Error!\n", __FUNCTION__, __LINE__);
		eDiErr = DI_ERR_ERROR;
		goto END;
	}

END:
	REL_NET_DEV_SEM(ulDevId);

	PrintExit();

	return eDiErr;
}

/*******************************************************************************
* function : DI_NETWORK_SetPortForward
* description :
* input :
* output :
* return :
* ??Ÿ ?????ڷ? ?? ????
*******************************************************************************/
DI_ERR_CODE DI_NETWORK_SetPortForward(HUINT32 ulDevId, DI_NETWORK_IP_VER_e eVer)
{
	HCHAR szCmdBuf[128];
	int vkErr;
	DI_ERR_CODE eDiErr = DI_ERR_OK;

	PrintEnter();

	if(ulDevId >= s_ulNetDevNum)
	{
		PrintError("[%s:%d] Invalid Device ID!\n", __FUNCTION__, __LINE__);
		return DI_ERR_ERROR;
	}

	GET_NET_DEV_SEM(ulDevId);

	if(eVer == DI_NETWORK_IP_VER_4)
	{
		VK_snprintf(szCmdBuf, 128, "echo 1 > /proc/sys/net/ipv4/ip_forward");
		vkErr = VK_SYSTEM_Command(szCmdBuf);
		if(vkErr != 0)
		{
			PrintError("[%s:%d] VK_SYSTEM_Command Error(%d)!\n", __FUNCTION__, __LINE__, vkErr);
			eDiErr = DI_ERR_ERROR;
			goto END;
		}

		VK_snprintf(szCmdBuf, 128, "iptables -P FORWARD ACCEPT");
		vkErr = VK_SYSTEM_Command(szCmdBuf);
		if(vkErr != 0)
		{
			PrintError("[%s:%d] VK_SYSTEM_Command Error(%d)!\n", __FUNCTION__, __LINE__, vkErr);
			eDiErr = DI_ERR_ERROR;
			goto END;
		}

		VK_snprintf(szCmdBuf, 128, "iptables -t nat -A POSTROUTING -o %s -j MASQUERADE", s_stNetInst[ulDevId].pstDevInfo->szName);
		vkErr = VK_SYSTEM_Command(szCmdBuf);
		if(vkErr != 0)
		{
			PrintError("[%s:%d] VK_SYSTEM_Command Error(%d)!\n", __FUNCTION__, __LINE__, vkErr);
			eDiErr = DI_ERR_ERROR;
			goto END;
		}
	}
#if defined(CONFIG_IPV6)
	else if(eVer == DI_NETWORK_IP_VER_6)
	{
		/* todo */
	}
#endif
END:
	REL_NET_DEV_SEM(ulDevId);

	PrintExit();

	return eDiErr;
}

/*******************************************************************************
* function : DI_NETWORK_ClearPortForward
* description :
* input :
* output :
* return :
* ??Ÿ ?????ڷ? ?? ????
*******************************************************************************/
DI_ERR_CODE DI_NETWORK_ClearPortForward(HUINT32 ulDevId, DI_NETWORK_IP_VER_e eVer)
{
	HCHAR szCmdBuf[128];
	int vkErr;
	DI_ERR_CODE eDiErr = DI_ERR_OK;

	PrintEnter();

	if(ulDevId >= s_ulNetDevNum)
	{
		PrintError("[%s:%d] Invalid Device ID!\n", __FUNCTION__, __LINE__);
		return DI_ERR_ERROR;
	}

	GET_NET_DEV_SEM(ulDevId);

	if(eVer == DI_NETWORK_IP_VER_4)
	{
		VK_snprintf(szCmdBuf, 128, "echo 1 > /proc/sys/net/ipv4/ip_forward");
		vkErr = VK_SYSTEM_Command(szCmdBuf);
		if(vkErr != 0)
		{
			PrintError("[%s:%d] VK_SYSTEM_Command Error(%d)!\n", __FUNCTION__, __LINE__, vkErr);
			eDiErr = DI_ERR_ERROR;
			goto END;
		}

		VK_snprintf(szCmdBuf, 128, "iptables -t nat -D POSTROUTING -o %s -j MASQUERADE", s_stNetInst[ulDevId].pstDevInfo->szName);
		vkErr = VK_SYSTEM_Command(szCmdBuf);
		if(vkErr != 0)
		{
			PrintError("[%s:%d] VK_SYSTEM_Command Error(%d)!\n", __FUNCTION__, __LINE__, vkErr);
			eDiErr = DI_ERR_ERROR;
			goto END;
		}
	}
#if defined(CONFIG_IPV6)
	else if(eVer == DI_NETWORK_IP_VER_6)
	{
		/* todo */
	}
#endif
END:
	REL_NET_DEV_SEM(ulDevId);

	PrintExit();

	return eDiErr;
}

/*******************************************************************************
* function : DI_NETWORK_GetDhcpOptions
* description :
* input :
* output :
* return :
* ??Ÿ ?????ڷ? ?? ????
*******************************************************************************/
DI_ERR_CODE DI_NETWORK_GetDhcpOptions(HUINT32 ulDevId, DI_NETWORK_IP_VER_e eVer, DI_NETWORK_DHCP_OPTIONS_t *pstOptions)
{
	DI_ERR_CODE eDiErr = DI_ERR_OK;
	FILE* fp;
	HCHAR szBuf[255] = {0, };
	HCHAR szConvertBuf[32] = {0, };
	HUINT8 ucVendorCode, ucVendorSize;
	HUINT8 *pucVendorData;
	int i = 0;

	PrintEnter();

	if(ulDevId >= s_ulNetDevNum)
	{
		PrintError("[%s:%d] Invalid Device ID!\n", __FUNCTION__, __LINE__);
		return DI_ERR_ERROR;
	}

	GET_NET_DEV_SEM(ulDevId);

	VK_MEM_Memset(pstOptions, 0, sizeof(DI_NETWORK_DHCP_OPTIONS_t));
	if(eVer == DI_NETWORK_IP_VER_4)
	{
		fp = fopen(DHCP_VENDOR_SPECIFIC_FILE, "r");
		if(fp == NULL)
		{
			PrintError("[%s:%d] fopen Error : %s!\n", __FUNCTION__, __LINE__, strerror(errno));
			eDiErr = DI_ERR_ERROR;
			goto END;
		}

		if(fgets(szBuf, sizeof(szBuf), fp))
		{
			while ((szBuf[i] != 0) && (i < 253))
			{
				ucVendorCode = szBuf[i];
				ucVendorSize = szBuf[i+1];
				pucVendorData = &szBuf[i+2];
				if(ucVendorCode==DHCP_VENDOR_SPECIFIC_OPT_ACS_URL)
				{
					VK_memcpy(pstOptions->aucAcsUrl, pucVendorData, ucVendorSize);
					pstOptions->aucAcsUrl[ucVendorSize] = '\0';
				}
				else if(ucVendorCode==DHCP_VENDOR_SPECIFIC_OPT_PROVISION_CODE)
				{
					VK_memcpy(pstOptions->aucProvisioningCode, pucVendorData, ucVendorSize+1);
					pstOptions->aucProvisioningCode[ucVendorSize] = '\0';
				}
				else if(ucVendorCode==DHCP_VENDOR_SPECIFIC_OPT_CWMP_RETRY_MIN_WAIT)
				{
					VK_memcpy(szConvertBuf, pucVendorData, ucVendorSize);
					szConvertBuf[ucVendorSize] = '\0';
					pstOptions->ulCWMPRetryMinWait = VK_atoi(szConvertBuf);
				}
				else if(ucVendorCode==DHCP_VENDOR_SPECIFIC_OPT_CWMP_RETRY_INTER_MULTI)
				{
					VK_memcpy(szConvertBuf, pucVendorData, ucVendorSize);
					szConvertBuf[ucVendorSize] = '\0';
					pstOptions->ulCWMPRetryIntervalMulti = VK_atoi(szConvertBuf);
				}
				i += (ucVendorSize+2);
			}
		}
		fclose(fp);
	}
#if defined(CONFIG_IPV6)
	else if(eVer == DI_NETWORK_IP_VER_6)
	{
		/* todo */
	}
#endif
END:
	REL_NET_DEV_SEM(ulDevId);

	PrintExit();

	return eDiErr;
}


#if defined(CONFIG_SHARED_COMPILE)
DI_ERR_CODE DI_NETWORK_Init(void)
{
	DRV_Error	nDrvErr = DRV_OK;

	nDrvErr = DRV_NETWORK_Init();
	if (nDrvErr != DRV_OK) {
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;
}
#endif

